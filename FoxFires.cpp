//
// Ilya Yavdoschuk
// FGroup (c) 2019
//

#include "FoxFires.hpp"
#include <random>

using namespace FG;

FoxFires::FoxFires(Controller * controller) : RenderLayer(controller) {
  this->controller = controller;

  controller->fires++;
  flags = defaultFlags;

  for (int i = 0; i < dataLength; i++)
    data[i] = map(rand() % 4096, 0, 4096, 0, 360);

  for (size_t i = 0; i < sizeof(sineMod) / sizeof(double); i++)
    sineMod[i] = (rand() % 1000 + 1) / 250.0;
}

double FoxFires::getSine(double a) {
  double result = 0;

  for (size_t i = 0; i < sizeof(sineMod) / sizeof(double); i++)
    result += sin((ySineOffset + a * sineMod[i]) * M_PI / 180.0);
  return result / (sizeof(sineMod) / sizeof(double));
}

void FoxFires::draw(RenderTarget * renderTarget) {

  double timeMapped = map(controller->timeInternal, 0, 86400, -1, 1);

  if (timeMapped >= -0.70 && timeMapped <= 0.70 || flags & Flags::Disabled)
    return;

  for (int x = 0; x < controller->w; x++) {
    int i = map(x, 0, controller->w, 0, dataLength);
    currentData = data[i];
    nextData = (i == dataLength - 1 ? currentData : data[i + 1]);

    mapI = map(i, 0, dataLength, 0, 180);
    maskDouble = 1;
    yMod = (flags & Flags::UseYSine)? map(getSine(mapI), -1, 1, 0, 1) : 0;

    if ((flags & Flags::UseYSineWane))
      maskDouble *= map(yMod, 0, 1, 1, 0.8);

    if ((flags & Flags::UseWaneSine)) {
      double waneSine = sin((waneSineOffset + mapI * 4) * M_PI / 180.0);
      maskDouble *= map(waneSine, -1, 1, (flags & Flags::UseYSineWane) && (flags & Flags::UseYSine)? 0.9 : 0.8, 1);
    }

    if (timeMapped < 0 && timeMapped > -0.80 && timeMapped < -0.70)
      maskDouble *= map(timeMapped, -0.80, -0.70, 1, 0);
    else if (timeMapped > 0 && timeMapped > 0.70 && timeMapped < 0.80)
      maskDouble *= map(timeMapped, 0.70, 0.80, 0, 1);

    Color borders;

    if (!(controller->flags & Controller::Flags::OverrideFFColors)) {
      oneI = (flags & Flags::Monochrome)? 0.5 : map(i, 0, dataLength, 0, 1);
      x1 = colorOffset;
      colorBaseIndex = (int)x1;

      x1 -= colorBaseIndex;
      x1 += oneI;

      if (x1 >= 1) {
        colorBaseIndex++;
        x1--;
      }

      x2 = 1 - x1;

      color = colors[colorBaseIndex % colorsLength];
      mixedColor = colors[(colorBaseIndex + 1) % colorsLength];

      mask1 = getMask(x2);
      mask2 = getMask(x1);

      mask1.a = 0xFF;
      mask2.a = 0xFF;

      color = color * mask1 + mixedColor * mask2;
      borders = Color::Transparent;
    }
    else {
      borders = Color(0xFF440000);
      color = Color(0x22FF88FF);
    }

    if (flags & Flags::UseData) {
      double pm = map(x - i * ((float)controller->w / dataLength), 0, ((float)controller->w / dataLength), 0, 1);
      double curr = map(sin((energySineOffset + currentData) * M_PI / 180.0), -1, 1, 0.1, 1);
      double next = map(sin((energySineOffset + nextData) * M_PI / 180.0), -1, 1, 0.1, 1);
      next -= curr;
      maskDouble *= (curr + next * pm);
    }

    mask = Color(0xFF, 0xFF, 0xFF, 0xFF * maskDouble);

    postcalc(x);

    color *= mask;

    Vertex line[] =
    {
      Vertex(Vector2f(controller->w - x, yOffset + totalSize * ((yMod * controller->h * (1 - size)))), borders),
      Vertex(Vector2f(controller->w - x, yOffset + totalSize * ((yMod * controller->h * (1 - size)) + (controller->h - bottomMargin - coreHeight) * size)), color),
      Vertex(Vector2f(controller->w - x, yOffset + totalSize * ((yMod * controller->h * (1 - size)) + (controller->h - bottomMargin) * size)), color),
      Vertex(Vector2f(controller->w - x, yOffset + totalSize * ((yMod * controller->h * (1 - size)) + controller->h * size)), borders)
    };

    color *= Color(0xFF * maskDouble, 0xFF * maskDouble, 0xFF * maskDouble, 0xFF) * getMask(1.0 / controller->fires);

    for (int j = 0; j < ambientDiff; j++) {
      int tmp = map(x, 0, controller->w, 0, controller->dataLength) + j - ambientDiff / 2;
      if (tmp < 0 || tmp > controller->dataLength - 1)
        continue;

      controller->skyAmbient[controller->dataLength - tmp - 1] += color;
    }

    renderTarget->draw(line, 4, LineStrip);
  }
}

void FoxFires::update() {
  if ((flags & Flags::UpdateData))
    energySineOffset += energySineDelta;

  if ((flags & Flags::UpdateYSine) && (flags & Flags::UseYSine))
    ySineOffset += ySineDelta;

  if ((flags & Flags::UpdateColor) && !(flags & Flags::Monochrome) && !(controller->flags & Controller::Flags::OverrideFFColors))
    colorOffset += colorDelta;

  if ((flags & Flags::UpdateWaneSine) && (flags & Flags::UseWaneSine))
    waneSineOffset += waneSineDelta;

  while (ySineOffset > 360)
    ySineOffset -= 360;

  while (waneSineOffset > 360)
    waneSineOffset -= 360;

  while (energySineOffset > 360)
    energySineOffset -= 360;

  while (colorOffset > colorsLength + 1)
    colorOffset -= colorsLength;

  double timeMapped = map(controller->timeInternal, 0, 86400, -1, 1);
  if (timeMapped >= -0.70 && timeMapped <= 0.70) {
    bool ctSet = (controller->flags & Controller::Flags::DarkNight);
    bool ffSet = (flags & Flags::Disabled);
    if (ctSet != ffSet)
      flags ^= Flags::Disabled;
  }

  // DEBUG
  controller->debugLabelText += "\n";
  controller->debugLabelText += "Flags       : " + std::to_string(flags) + "\n";
  controller->debugLabelText += "Energy sine : " + std::to_string(energySineOffset) +  +" (+" + std::to_string(energySineDelta) + ")\n";
  controller->debugLabelText += "Y sine      : " + std::to_string(ySineOffset) +  +" (+" + std::to_string(ySineDelta) + ")\n";
  controller->debugLabelText += "Wane sine   : " + std::to_string(waneSineOffset) +  +" (+" + std::to_string(waneSineDelta) + ")\n";
  controller->debugLabelText += "Color offset: " + ((controller->flags & Controller::Flags::OverrideFFColors) ? "[realistic mode]\n" : (std::to_string(colorOffset) + " (+" + std::to_string(colorDelta) + ")\n"));
}
