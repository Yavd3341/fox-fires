//
// Ilya Yavdoschuk
// FGroup (c) 2019
//

#include "FoxFires.hpp"
#include <random>
#include <ctime>
#include <vector>
#include <string>

using namespace FG;

class CustomFoxFires : public FoxFires {
  public:

    int cffOffset = 0;

    CustomFoxFires(Controller * controller) : FoxFires(controller) {
    }

    void postcalc(int i) {
      totalSize = map(i, 0, dataLength, 0.2, 0.9);
      yOffset = map(i, 0, dataLength, 1, 0);
      yOffset = map(1 - yOffset, 0, 1, controller->h - (controller->h * 0.6), -(controller->h * 0.3)) + cffOffset;
    }
};

Controller::Controller() {
  backColor = Color(0, 0, 25);
  w = 800;
  h = 600;
}


int fps = 60;
ContextSettings settings;

void Controller::initLayers() {
  for (RenderLayer * l : layers)
    delete l;
  layers.clear();

  if (skyAmbient != NULL)
    delete[] skyAmbient;

  dataLength = VideoMode::getDesktopMode().width;
  skyAmbient = new Color[dataLength];

  envs = 0;
  envr = 0;
  fires = 0;

  timeInternal = 0;

  for (int i = 0; i < dataLength; i++)
    skyAmbient[i] = ambientColor;

  CustomFoxFires * ff1 = new CustomFoxFires(this);
  CustomFoxFires * ff2 = new CustomFoxFires(this);
  CustomFoxFires * ff3 = new CustomFoxFires(this);

  ff1->cffOffset = (h / 7.0);
  ff1->ySineOffset = map(rand() % 4096, 0, 4096, 0, 360);
  ff1->waneSineOffset = map(rand() % 4096, 0, 4096, 0, 360);
  ff1->colorOffset = map(rand() % 4096, 0, 4096, 0, 0.6);

  ff2->ySineOffset = map(rand() % 4096, 0, 4096, 0, 360);
  ff2->waneSineOffset = map(rand() % 4096, 0, 4096, 0, 360);
  ff2->colorOffset = map(rand() % 4096, 0, 4096, 0, 0.6);

  ff3->cffOffset = -(h / 7.0);
  ff3->ySineOffset = map(rand() % 4096, 0, 4096, 0, 360);
  ff3->waneSineOffset = map(rand() % 4096, 0, 4096, 0, 360);
  ff3->colorOffset = map(rand() % 4096, 0, 4096, 0, 0.6);

  Ground * grd1 = new Ground(this);
  Ground * grd2 = new Ground(this);
  Ground * grd3 = new Ground(this);

  grd1->yOffset = h / 30.0 * 3;
  grd1->sineMod = (rand() % 3 + 1) / 2.0;
  grd1->dark = Color(0xBBBBBBFF);
  grd1->bakeTrees();

  grd2->yOffset = h / 30.0 * 2;
  grd2->sineMod = (rand() % 3 + 1) / 2.0;
  grd2->dark = Color(0x888888FF);
  grd2->bakeTrees();

  grd3->yOffset = h / 30.0 * 1;
  grd3->sineMod = (rand() % 3 + 1) / 2.0;
  grd3->dark = Color(0x444444FF);
  grd3->bakeTrees();

  layers.push_back(new Sky(this));
  layers.push_back(new Stars(this));

  layers.push_back(ff1);
  layers.push_back(ff2);
  layers.push_back(ff3);

  layers.push_back(grd1);
  layers.push_back(grd2);
  layers.push_back(grd3);

  debugLabel.setFont(font);
  debugLabel.setCharacterSize(16);
  debugLabel.setFillColor(Color::White);
  debugLabel.setOutlineColor(Color::Black);
  debugLabel.setOutlineThickness(1.5);
  debugLabel.setPosition(Vector2f(10, 10));
}

void Controller::init() {

  // Setting flags

  flags[FLAG_NODRAW] = false;
  flags[FLAG_FULLSCREEN] = false;
  flags[FLAG_PAUSE] = false;
  flags[FLAG_FONT_FAIL] = !font.loadFromFile("font.ttf");
  flags[FLAG_DRAW_GUI] = false;
  flags[FLAG_SHOW_CURSOR] = true;
  flags[FLAG_UPDATE_CLOCK] = true;
  flags[FLAG_REALISTIC_FF] = false;

  std::cout << "Font status: " << (flags[FLAG_FONT_FAIL] ? "load failed" : "loaded") << std::endl;

  // Continue init

  initLayers();

  settings.antialiasingLevel = 8;

  if (window != NULL)
    window->close();

  window = new RenderWindow(flags[FLAG_FULLSCREEN] ? VideoMode::getDesktopMode() : VideoMode(w, h),
                            "Fox Fires", flags[FLAG_FULLSCREEN] ? Style::Fullscreen : Style::Default, settings);
  window->setFramerateLimit(fps);
  window->setMouseCursorVisible(flags[FLAG_SHOW_CURSOR]);

  srand(time(NULL));
}

void Controller::run() {
  while (window->isOpen()) {
    Event event;
    while (window->pollEvent(event)) {
      if (event.type == Event::Closed)
        window->close();

      if (event.type == Event::KeyPressed) {
        if (event.key.code == Keyboard::Escape)
          window->close();

        if (event.key.code == Keyboard::Space)
          flags[FLAG_PAUSE] = !flags[FLAG_PAUSE];

        if (event.key.code == Keyboard::T)
          flags[FLAG_UPDATE_CLOCK] = !flags[FLAG_UPDATE_CLOCK];

        if (event.key.code == Keyboard::R)
          flags[FLAG_REALISTIC_FF] = !flags[FLAG_REALISTIC_FF];

        if (event.key.code == Keyboard::S && timeInternal > 15000 && timeInternal < 70000)
          timeInternal = 70000;

        if (event.key.code == Keyboard::F3)
          flags[FLAG_DRAW_GUI] = !flags[FLAG_DRAW_GUI];

        if (event.key.code == Keyboard::F5) {
          if (Keyboard::isKeyPressed(Keyboard::LShift) || Keyboard::isKeyPressed(Keyboard::RShift))
            init();
          else
            initLayers();
        }

        if (event.key.code == Keyboard::H) {
          flags[FLAG_SHOW_CURSOR] = !flags[FLAG_SHOW_CURSOR];
          window->setMouseCursorVisible(flags[FLAG_SHOW_CURSOR]);
        }

        if (event.key.code == Keyboard::F || event.key.code == Keyboard::F11) {
          flags[FLAG_FULLSCREEN] = !flags[FLAG_FULLSCREEN];
          window->create(flags[FLAG_FULLSCREEN] ? VideoMode::getDesktopMode() : VideoMode(w, h),
                         "Fox Fires", flags[FLAG_FULLSCREEN] ? Style::Fullscreen : Style::Default, settings);
          window->setFramerateLimit(fps);
          window->setMouseCursorVisible(flags[FLAG_SHOW_CURSOR]);
        }

        if (event.key.code == Keyboard::Right) {
          if (Keyboard::isKeyPressed(Keyboard::LControl) || Keyboard::isKeyPressed(Keyboard::RControl)) {
          }
          else {
            if (Keyboard::isKeyPressed(Keyboard::LShift) || Keyboard::isKeyPressed(Keyboard::RShift))
              timeInternal += timeManualDelta * 10;
            else
              timeInternal += timeManualDelta;

            if (timeInternal > 86400)
              timeInternal -= 86400;
          }
        }

        if (event.key.code == Keyboard::Left) {
          if (Keyboard::isKeyPressed(Keyboard::LControl) || Keyboard::isKeyPressed(Keyboard::RControl)) {
          }
          else {
            if (Keyboard::isKeyPressed(Keyboard::LShift) || Keyboard::isKeyPressed(Keyboard::RShift))
              timeInternal -= timeManualDelta * 10;
            else
              timeInternal -= timeManualDelta;

            if (timeInternal < 0)
              timeInternal += 86400;
          }
        }
      }

      if (event.type == Event::Resized) {
        FloatRect visibleArea(0, 0, event.size.width, event.size.height);
        window->setView(View(visibleArea));
      }
    }

    w = window->getSize().x;
    h = window->getSize().y;

    if (!flags[FLAG_NODRAW])
      requestDraw();

    if (!flags[FLAG_PAUSE])
      requestUpdate();

    window->display();
  }
}

void Controller::requestDraw() {
  window->clear(backColor);

  for (RenderLayer * layer : layers)
    layer->draw();

  if (flags[FLAG_DRAW_GUI] && !flags[FLAG_FONT_FAIL])
    window->draw(debugLabel);
}

void Controller::requestUpdate() {
  debugLabelText = "";
  debugLabelText += "FoxFires ver. 1.0.1\n";
  debugLabelText += "By Ilya Yavdoschuk\n";
  debugLabelText += "\n";
  debugLabelText += "Time " + (std::string)(!flags[FLAG_UPDATE_CLOCK] ? "[paused]" : "        ") + "   : " + std::to_string(timeInternal) + " seconds\n";
  debugLabelText += "Time delta      : " + std::to_string(timeDelta) + " (" + std::to_string(timeManualDelta) + ") seconds per update\n";
  debugLabelText += "Sky data length : " + std::to_string(dataLength) + "\n";
  debugLabelText += "Fires registered: " + std::to_string(fires) + "\n";
  debugLabelText += "Window size     : " + std::to_string(w) + "x" + std::to_string(h) + " (px)\n";
  debugLabelText += "AA level        : " + std::to_string(window->getSettings().antialiasingLevel) + "\n";
  debugLabelText += "\n";
  debugLabelText += "Fox Fires data:\n";

  if (flags[FLAG_UPDATE_CLOCK]) {
    timeInternal += timeDelta;
    if (timeInternal > 86400)
      timeInternal -= 86400;
  }

  for (RenderLayer * layer : layers)
    layer->update();

  debugLabel.setString(debugLabelText);
}

int main(){
  Controller controller;

  controller.init();
  controller.run();

  return 0;
}
