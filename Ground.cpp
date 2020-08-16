//
// Ilya Yavdoschuk
// FGroup (c) 2019
//

#include "FoxFires.hpp"
#include <vector>
#include <algorithm>

using namespace FG;

const double prop = 0.9;

Ground::Ground(Controller * controller) : RenderLayer(controller) {
  this->controller = controller;

  layers = std::vector<RenderLayer*>();

  controller->envs++;
  controller->envr++;

  color1 = Color(0xCCCCCCFF);
  color2 = Color(0x888888FF);

  mask = Color(0x888888FF);
  dark = Color(0x777777FF);

  sineOffset = rand() % 360;
  treesSineOffset = rand() % 360;

  for (size_t i = 0; i < sizeof(sineMod) / sizeof(double); i++)
    sineMod[i] = (rand() % 1000 + 1) / 250.0;
}

double Ground::getSine(double a) {
  double result = 0;

  for (size_t i = 0; i < sizeof(sineMod) / sizeof(double); i++)
    result += sin((sineOffset + a * sineMod[i]) * M_PI / 180.0);
  return result / (sizeof(sineMod) / sizeof(double));
}

void Ground::bakeTrees() {
  for (auto p : layers)
    delete p;
  layers.clear();

  float treeSize = 0.125;
  int treesCount = ceil(1 / (Pine::defaultSize.x * treeSize));

  for (int i = 0; i < treesCount; i++) {
    double x = controller->w * ((Pine::defaultSize.x * treeSize) / 2 + i * (Pine::defaultSize.x * treeSize)) + map(rand() % 4096, 0, 4096, -10, 10);

    double mapI = map(x, 0, controller->w, 0, 360);
    double yMod = map(getSine(mapI), -1, 1, 0, 1);

    double y = controller->h - (yMod * yHeight) - yOffset + map(rand() % 4096, 0, 4095, 0, 20) + 10;

    Pine * pine = new Pine(controller, Vector2f(x / controller->w, y / controller->h), map(rand() % 4096, 0, 4095, 0.2, 0.3));

    pine->sticksColor *= dark + Color(0x333333FF);
    pine->logColor *= dark + Color(0x333333FF);

    layers.push_back(pine);
  }

  std::sort(layers.begin(), layers.end(), [](RenderLayer* a, RenderLayer* b) {
    Pine* pa = dynamic_cast<Pine*>(a);
    Pine* pb = dynamic_cast<Pine*>(b);
    return pa->position.y < pb->position.y;
  });
}

void Ground::draw(RenderTarget * renderTarget) {
  for (int x = 0; x < controller->w; x++) {
    double mapI = map(x, 0, controller->w, 0, 360);
    double yMod = map(getSine(mapI), -1, 1, 0, 1);

    int ambientI = map(x, 0, controller->w, 0, controller->dataLength);
    controller->skyAmbient[ambientI] = applyAlpha(controller->skyAmbient[ambientI]) * getMask(prop) + controller->ambientColor * getMask(1 - prop);

    Vertex line[] =
    {
      Vertex(Vector2f(x, controller->h - (yMod * (yHeight + haloHeight)) - yOffset), (color1 * controller->skyAmbient[ambientI] + color1) * dark),
      Vertex(Vector2f(x, controller->h - (yMod * yHeight) - yOffset), (color2 * controller->skyAmbient[ambientI] + color2) * dark),
      Vertex(Vector2f(x, controller->h), (color2 + color2 * mask) * dark)
    };

    renderTarget->draw(line, 3, LineStrip);
  }

  for (RenderLayer * layer : layers)
    layer->draw(renderTarget);

  controller->envs--;

  if (controller->envs == 0) {
    for (int i = 0; i < controller->dataLength; i++)
      controller->skyAmbient[controller->dataLength - i - 1] = Color::Transparent;

    controller->envs = controller->envr;
  }
}

void Ground::update() {
  for (RenderLayer * layer : layers)
    layer->update();
}
