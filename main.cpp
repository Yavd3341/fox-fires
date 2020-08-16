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

    void postcalc(int x) {
      totalSize = map(x, 0, controller->w, 0.2, 0.9);
      yOffset = map(x, 0, controller->w, 1, 0);
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
  grd1->dark = Color(0xBBBBBBFF);
  grd1->bakeTrees();

  grd2->yOffset = h / 30.0 * 2;
  grd2->dark = Color(0x888888FF);
  grd2->bakeTrees();

  grd3->yOffset = h / 30.0 * 1;
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

void Controller::init(bool windowOnly) {

  if (!windowOnly) {
    // Setting flags

    flags = Flags::ShowCursor | Flags::UpdateClock;

    if (!font.loadFromFile("font.ttf"))
      flags |= Flags::FontFailure;

    std::cout << "Font status: " << (flags & Flags::FontFailure ? "load failed" : "loaded") << std::endl;

    // Continue init

    srand(time(NULL));

    initLayers();
    settings.antialiasingLevel = 8;
  }

  if (window != NULL)
    window->close();

  window = new RenderWindow(flags & Flags::Fullscreen ? VideoMode::getDesktopMode() : VideoMode(w, h),
                            "Fox Fires", flags & Flags::Fullscreen ? Style::Fullscreen : Style::Default, settings);
  window->setFramerateLimit(fps);
  window->setMouseCursorVisible(flags & Flags::ShowCursor);
}

void Controller::run() {
  while (window->isOpen()) {
    renderTarget = window;

    Event event;
    while (window->pollEvent(event)) {
      if (event.type == Event::Closed)
        window->close();

      if (event.type == Event::KeyPressed) {
        if (event.key.code == Keyboard::Escape)
          window->close();

        if (event.key.code == Keyboard::Space)
          flags ^= Flags::Pause;

        if (event.key.code == Keyboard::T)
          flags ^= Flags::UpdateClock;

        if (event.key.code == Keyboard::E) {
          flags |= Flags::TargetChanged;
          RenderTexture* rt = new RenderTexture();
          rt->create(VideoMode::getDesktopMode().width, VideoMode::getDesktopMode().height, settings);
          renderTarget = rt;
        }

        if (event.key.code == Keyboard::R)
          flags ^= Flags::OverrideFFColors;

        if (event.key.code == Keyboard::S)
          if (Keyboard::isKeyPressed(Keyboard::F3))
            flags ^= Flags::LegacyStars;
          else if (timeInternal > 15000 && timeInternal < 70000)
            timeInternal = 70000;

        if (event.key.code == Keyboard::F3)
          flags ^= Flags::DrawGUI;

        if (event.key.code == Keyboard::F5) {
          if (Keyboard::isKeyPressed(Keyboard::LShift) || Keyboard::isKeyPressed(Keyboard::RShift))
            init(false);
          else if (Keyboard::isKeyPressed(Keyboard::LControl) || Keyboard::isKeyPressed(Keyboard::RControl))
            init(true);
          else
            initLayers();
        }

        if (event.key.code == Keyboard::H) {
          flags ^= Flags::ShowCursor;
          window->setMouseCursorVisible(flags & Flags::ShowCursor);
        }

        if (event.key.code == Keyboard::F || event.key.code == Keyboard::F11) {
          flags ^= Flags::Fullscreen;
          window->create(flags & Flags::Fullscreen ? VideoMode::getDesktopMode() : VideoMode(w, h),
                         "Fox Fires", flags & Flags::Fullscreen ? Style::Fullscreen : Style::Default, settings);
          window->setFramerateLimit(fps);
          window->setMouseCursorVisible(flags & Flags::ShowCursor);
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

        if (event.key.code == Keyboard::RBracket) {
          if (settings.antialiasingLevel < 16)
            settings.antialiasingLevel++;
        }

        if (event.key.code == Keyboard::LBracket) {
          if (settings.antialiasingLevel > 0)
            settings.antialiasingLevel--;
        }
      }

      if (event.type == Event::Resized) {
        FloatRect visibleArea(0, 0, event.size.width, event.size.height);
        window->setView(View(visibleArea));
      }
    }

    w = renderTarget->getSize().x;
    h = renderTarget->getSize().y;

    if (!(flags & Flags::Pause))
      requestUpdate();

    if (!(flags & Flags::NoDraw))
      requestDraw();

    if (flags & Flags::TargetChanged) {
      flags &= ~Flags::TargetChanged;
      RenderTexture* rt = (RenderTexture*)renderTarget;
      rt->display();
      window->draw(Sprite(rt->getTexture()));

      time_t curr_time;
      tm * ct;
      time(&curr_time);
      ct = localtime(&curr_time);

      rt->getTexture().copyToImage().saveToFile(std::to_string(ct->tm_year + 1900) + "-" + std::to_string(ct->tm_mon + 1) + "-" + std::to_string(ct->tm_mday) + "T"
                                                + std::to_string(ct->tm_hour) + "-" + std::to_string(ct->tm_min) + "-" + std::to_string(ct->tm_sec) + ".png");
    }
    else
      window->display();
  }
}

void Controller::requestDraw() {
  renderTarget->clear(backColor);

  for (RenderLayer * layer : layers)
    layer->draw(renderTarget);

  if (flags & Flags::DrawGUI && ~flags & Flags::FontFailure)
    renderTarget->draw(debugLabel);
}

void Controller::requestUpdate() {
  debugLabelText = "";
  debugLabelText += "FoxFires ver. 1.2.0\n";
  debugLabelText += "By Ilya Yavdoschuk\n";
  debugLabelText += "\n";
  debugLabelText += "Time " + (std::string)(!(flags & Flags::UpdateClock)? "[paused]" : "        ") + "   : " + std::to_string(timeInternal) + " seconds\n";
  debugLabelText += "Time delta      : " + std::to_string(timeDelta) + " (" + std::to_string(timeManualDelta) + ") seconds per update\n";
  debugLabelText += "Sky data length : " + std::to_string(dataLength) + "\n";
  debugLabelText += "Fires registered: " + std::to_string(fires) + "\n";
  debugLabelText += "Window size     : " + std::to_string(w) + "x" + std::to_string(h) + " (px)\n";
  debugLabelText += "AA level        : " + std::to_string(window->getSettings().antialiasingLevel) + " (next: " + std::to_string(settings.antialiasingLevel) + ")\n";
  debugLabelText += "\n";
  debugLabelText += "Fox Fires data:\n";

  if (flags & Flags::UpdateClock) {
    timeInternal += timeDelta;
    while (timeInternal > 86400)
      timeInternal -= 86400;
  }

  for (RenderLayer * layer : layers)
    layer->update();

  debugLabel.setString(debugLabelText);
}

int main(){
  Controller controller;

  controller.init(false);
  controller.run();

  return 0;
}
