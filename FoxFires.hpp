//
// Ilya Yavdoschuk
// FGroup (c) 2019
//

#pragma once

#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>

using namespace sf;

namespace FG {

  static double map(double v, double min1, double max1, double min2, double max2){
    return (v - min1) / (max1 - min1) * (max2 - min2) + min2;
  }

  static Color getMask(double val){
    int data = round(map(val, 0, 1, 0, 255));

    return Color(data, data, data, data);
  }

  static Color applyAlpha(Color col){
    return col * Color(col.a, col.a, col.a);
  }

  static Vector2f rotateVector(Vector2f vec, float deg) {
    float rad = deg * M_PI / 180.0;

    return Vector2f(vec.x * cos(rad) - vec.y * sin(rad), vec.x * sin(rad) + vec.y * cos(rad));
  }

  static float getVectorLenght(Vector2f vec) {
    return sqrt(vec.x * vec.x + vec.y * vec.y);
  }

  static Vector2f setVectorLenght(Vector2f vec, float len) {
    return vec / getVectorLenght(vec) * len;
  }

  class RenderLayer;
  class Controller;
  class FoxFires;
  class Stars;
  class Sky;
  class Pine;
  class Ground;

  class Controller {

    public:

      class Flags {

        public:

          static const short NoDraw = 1 << 0;
          static const short Fullscreen = 1 << 1;
          static const short Pause = 1 << 2;
          static const short FontFailure = 1 << 3;
          static const short DrawGUI = 1 << 4;
          static const short ShowCursor = 1 << 5;
          static const short UpdateClock = 1 << 6;
          static const short OverrideFFColors = 1 << 7;
          static const short TargetChanged = 1 << 8;
          static const short DarkNight = 1 << 9;
      };

      short flags;
      Font font;

      Text debugLabel;
      std::string debugLabelText;

      RenderTarget * renderTarget = NULL;
      RenderWindow * window = NULL;
      unsigned int w;
      unsigned int h;

      double timeInternal = 0;
      double timeDelta = 10;
      double timeManualDelta = timeDelta * 10;

      int dataLength;
      Color * skyAmbient = NULL;
      int envs = 0;
      int envr = 0;
      int fires = 0;
      int litNights = 0;

      Color backColor;
      Color ambientColor;

      Controller();

      void init(bool windowOnly);
      void initLayers();
      void run();

      void requestDraw();
      void requestUpdate();

    public:

      std::vector<RenderLayer*> layers;
  };

  class RenderLayer {

    public:

      Controller * controller;

      RenderLayer(Controller * controller);

      virtual void draw(RenderTarget * renderTarget) = 0;
      virtual void update() = 0;
  };

  class FoxFires : public RenderLayer {

    protected:

      //
      // Controller variables
      //

      unsigned int step = 10;
      unsigned int dataLength = VideoMode::getDesktopMode().width / step;
      double* data = new double[dataLength];

      int currentColor = 0;
      const static int colorsLength = 5;
      const Color colors[colorsLength] = {
        Color(0x00FF8888),
        Color(0x0088FF88),
        Color(0xDD00DD88),
        Color(0xDD00DD88),
        Color(0x0088FF88)
      };

      //
      // Draw variables
      //

      unsigned int ambientDiff = 10;

      double prevData;
      double currentData;
      double nextData;

      double mapI;
      double maskDouble;
      double yMod;
      double yOffset = 0;
      double sineMod[5];

      double oneI;
      double x1;
      int colorBaseIndex;

      double x2;

      Color color;
      Color mixedColor;
      Color mask1;
      Color mask2;
      Color mask;

    public:

      //
      // Public variables
      //

      class Flags {

        public:

          // DATA FLAGS
          static const short UseData = 1 << 0;
          static const short UpdateData = 1 << 1;
          static const short CycleData = 1 << 2;

          // Y SINE FLAGS
          static const short UseYSine = 1 << 3;
          static const short UpdateYSine = 1 << 4;
          static const short UseYSineWane = 1 << 5;

          // COLOR FLAGS
          static const short Monochrome = 1 << 6;
          static const short UpdateColor = 1 << 7;

          // WANE SINE FLAGS
          static const short UseWaneSine = 1 << 8;
          static const short UpdateWaneSine = 1 << 9;

          // OTHER FLAGS
          static const short Disabled = 1 << 10;
      };

      static const short defaultFlags = Flags::UseData
                                        | Flags::UpdateData
                                        | Flags::UseYSine
                                        | Flags::UpdateYSine
                                        | Flags::UseYSineWane
                                        | Flags::UpdateColor
                                        | Flags::UseWaneSine
                                        | Flags::UpdateWaneSine;

      short flags;
      int coreHeight = 5;
      int bottomMargin = 100;
      double size = 0.9;
      double totalSize = 1;

      double energySineOffset = 0;
      double ySineOffset = 0;
      double waneSineOffset = 0;
      double colorOffset = 0;

      double energySineDelta = 1.5;
      double ySineDelta = 1.25;
      double waneSineDelta = 2.5;
      double colorDelta = 0.001;

      FoxFires(Controller * controller);

      virtual void postcalc(int x) = 0;
      double getSine(double a);

      void draw(RenderTarget * renderTarget);
      void update();
  };

  class Stars : public RenderLayer {

    public:

      double ** stars;
      unsigned int starsCount;

      double maxBrightness = 0.75;
      double blinkOffset = 0;
      double blinkDelta = 0.25;
      bool autoSetDelta = true;
      double hMod = 1;
      int radius = 6;
      Color color;

      Stars (Controller * controller);

      void draw(RenderTarget * renderTarget);
      void update();
  };

  class Sky : public RenderLayer {

    public:

      const Color skies[2] = {
        Color(0x000019FF),
        Color(0x73C8F0FF)
      };

      const Color ambients[2] = {
        Color(0x8888FFFF),
        Color(0x00ADFFFF)
      };

      Sky (Controller * controller);

      void draw(RenderTarget * renderTarget);
      void update();
  };

  class Pine : public RenderLayer {

    public:

      static Vector2f defaultSize;

      Color sticksColor;
      Color logColor;

      Vector2f size;
      Vector2f position;
      float sizeMult = 1;

      unsigned int stickCount = 20;

      void drawPine(RenderTarget * renderTarget, Vector2f pos, Vector2f size,
                    Color log, Color sticks, unsigned int stickCount, float targetAngle);

      Pine(Controller * controller, Vector2f position, Vector2f size);
      Pine(Controller * controller, Vector2f position, float sizeMult);

      void draw(RenderTarget * renderTarget);
      void update();
  };

  class Ground : public RenderLayer {

    public:

      Color color1;
      Color color2;

      Color mask;
      Color dark;

      double sineOffset = 0;
      double sineMod[5];
      double yOffset = 10;
      double yHeight = 25;
      double haloHeight = 15;
      unsigned int step = 10;

      Ground (Controller * controller);

      double treesSineOffset = 0;
      double treesSineMod = 1;
      double treeDensity = 1.5;
      void bakeTrees();
      double getSine(double a);

      void draw(RenderTarget * renderTarget);
      void update();

    protected:

      std::vector<RenderLayer*> layers;
  };
}
