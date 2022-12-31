//
// Ilya Yavdoschuk
// FGroup (c) 2019
//

#include "FoxFires.hpp"
#include <random>
#include <stdio.h>

using namespace FG;

Shader FoxFires::shader;

FoxFires::FoxFires(Controller * controller) : RenderLayer(controller) {
  this->controller = controller;
  
	for (int i = 0; i < 3; i++)
		colors_s[i] = Glsl::Vec4(colors[(currentColor + i) % colorsLength]);

  controller->fires++;
}

void FoxFires::update() {
  time += timeDelta;
  bool updateColors = stateOverride != (controller->flags & Controller::Flags::OverrideFFColors);

  if (time >= 1) {
    time -= 1;
    currentColor = ++currentColor % colorsLength;
    updateColors = true;
  }

  if (updateColors) {
    stateOverride = controller->flags & Controller::Flags::OverrideFFColors;
    if (controller->flags & Controller::Flags::OverrideFFColors) {
      colors_s[1] = Glsl::Vec4(Color(0x22FF88FF));
      colors_s[0] = Glsl::Vec4(Color(0xFF440000));
    }
    else {
      for (int i = 0; i < 3; i++)
        colors_s[i] = Glsl::Vec4(colors[(currentColor + i) % colorsLength]);
    }
  }
		
  // DEBUG
  controller->debugLabelText += "\n";
  controller->debugLabelText += "Time   : " + std::to_string(time) +" (+" + std::to_string(timeDelta) + ")\n";
  controller->debugLabelText += "Strokes: " + ((controller->flags & Controller::Flags::FillMode)
    ? "[fill mode]" : std::to_string(strokes)) + "\n";
  controller->debugLabelText += "Color  : " + ((controller->flags & Controller::Flags::OverrideFFColors) 
    ? "[realistic mode]" : std::to_string(currentColor + 1) +" out of " + std::to_string(colorsLength)) + "\n";
  controller->debugLabelText += "Seed   : " + std::to_string(randomSeed) + "\n";
}

void FoxFires::draw(RenderTarget * renderTarget) {
  if (controller->flags & Controller::Flags::DarkNight)
    return;

  Vertex surface[] = {
    Vertex(Vector2f(0, 0), Color::Red),
    Vertex(Vector2f(controller->w, 0), Color::Red),
    Vertex(Vector2f(controller->w, controller->h), Color::Red),
    Vertex(Vector2f(0, controller->h), Color::Red)
  };

  shader.setUniformArray("colors", colors_s, 3);
  shader.setUniform("seed", randomSeed);
  shader.setUniform("strokes", (float)25.0);
  shader.setUniform("f_fill", controller->flags & Controller::Flags::FillMode);
  shader.setUniform("f_static", controller->flags & Controller::Flags::OverrideFFColors);
  shader.setUniform("time", time);
  shader.setUniform("ctime", (float)(controller->timeInternal - 86400 / 2.));
  shader.setUniform("pos", Glsl::Vec2(0, controller->h * (1 - ySize - yPos)));
  shader.setUniform("size", Glsl::Vec2(controller->w, controller->h * ySize));
  shader.setUniform("offsets", Glsl::Vec2(leftOffset,  rightOffset));
  shader.setUniform("heights", Glsl::Vec4(leftTopFraction, leftMiddleFraction, rightTopFraction, rightMiddleFraction));

  renderTarget->draw(surface, 4, Quads, &shader);
}
