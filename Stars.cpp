//
// Ilya Yavdoschuk
// FGroup (c) 2019
//

#include "FoxFires.hpp"

using namespace FG;

Stars::Stars(Controller * controller) : RenderLayer(controller) {
  this->controller = controller;

  unsigned int dw = VideoMode::getDesktopMode().width;
  unsigned int dh = VideoMode::getDesktopMode().height;

  if (autoSetDelta)
    blinkDelta *= ((double)std::max(dw, dh) / std::min(dw, dh));

  starsCount = (dw * dh) / 1000;

  stars = new double*[starsCount];
  for (int i = 0; i < starsCount; i++)
    stars[i] = new double[2];

  color = Color(0xFF, 0xFF, 0xAA);

  for (int i = 0; i < starsCount; i++) {
    stars[i][0] = map(rand() % 4096, 0, 4095, 0, 1);
    stars[i][1] = map(rand() % 4096, 0, 4095, 0, 1);
  }
}

void Stars::draw(RenderTarget * renderTarget) {
  double timeMapped = map(controller->timeInternal, 0, 86400, -1, 1);

  if (timeMapped >= -0.66 && timeMapped <= 0.66)
    return;

  for (int i = 0; i < starsCount; i++) {
    unsigned int x = stars[i][0] * controller->w + radius;
    unsigned int y = stars[i][1] * controller->h * hMod + radius;

    double waneSine = sin((blinkOffset + i) * M_PI / 180);

    timeMapped = map(controller->timeInternal, 0, 86400, -1, 1);

    if (timeMapped < 0 && timeMapped > -0.75 && timeMapped < -0.66)
      timeMapped = map(timeMapped, -0.75, -0.66, 1, 0);
    else if (timeMapped > 0 && timeMapped > 0.66 && timeMapped < 0.75)
      timeMapped = map(timeMapped, 0.66, 0.75, 0, 1);
    else
      timeMapped = 1;

    Color mask(0xFF, 0xFF, 0xFF, map(waneSine, -1, 1, 0.25, 1) * timeMapped * 0xFF);

    Vertex line1[] =
    {
      Vertex(Vector2f(x - radius, y), Color::Transparent),
      Vertex(Vector2f(x, y), color * mask),
      Vertex(Vector2f(x + radius, y), Color::Transparent)
    };

    Vertex line2[] =
    {
      Vertex(Vector2f(x, y - radius), Color::Transparent),
      Vertex(Vector2f(x, y), color * mask),
      Vertex(Vector2f(x, y + radius), Color::Transparent)
    };

    renderTarget->draw(line1, 3, LineStrip);
    renderTarget->draw(line2, 3, LineStrip);
  }
}

void Stars::update() {
  blinkOffset += blinkDelta;

  while (blinkOffset > 360)
    blinkOffset -= 360;
}
