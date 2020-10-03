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
  float treeWidth = Pine::defaultSize.x * treeSize;
  int treesCount = ceil(1 / treeWidth * treeDensity);

  for (int i = 0; i < treesCount; i++) {
    double x = controller->w * (i * treeWidth / treeDensity + map(rand() % 4096, 0, 4096, -treeWidth / 2, treeWidth / 2));

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
  std::vector<Vertex> topVert;
  std::vector<Vertex> botVert;

  for (int x = 0; x <= controller->w / step + 1; x++) {
    double mapI = map(x, 0, controller->w / step, 0, 360);
    double yMod = map(getSine(mapI), -1, 1, 0, 1);

    int ambientI = map(x, 0, controller->w / step + 1, 0, controller->dataLength);
    Color ambient = applyAlpha(controller->skyAmbient[ambientI]) * getMask(prop) + controller->ambientColor * getMask(1 - prop);

    Vertex top(Vector2f(x * step, controller->h - (yMod * (yHeight + haloHeight)) - yOffset), (color1 * ambient + color1) * dark);
    Vertex mid(Vector2f(x * step, controller->h - (yMod * yHeight) - yOffset), (color2 * ambient + color2) * dark);
    Vertex bot(Vector2f(x * step, controller->h), (color2 + color2 * mask) * dark);

    topVert.push_back(top);
    topVert.push_back(mid);

    botVert.push_back(mid);
    botVert.push_back(bot);
  }

  renderTarget->draw(&topVert[0], topVert.size(), TriangleStrip);
  renderTarget->draw(&botVert[0], botVert.size(), TriangleStrip);

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
