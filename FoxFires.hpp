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

	static double ease(double v){
		return v; //pow(v, 1.3);
	}
	
	static Color getMask(double val){
		int data = round(map(val, 0, 1, 0, 255));
		return Color(data, data, data, data);
	}
	
	static Color applyAlpha(Color col){
		return col * Color(col.a, col.a, col.a);
	}

	// DRAW (Controller) FLAGS
	#define FLAG_NODRAW 0
	#define FLAG_FULLSCREEN 1
	#define FLAG_PAUSE 2
	
	class Controller {
		
		public:			
			RenderWindow * window;
			unsigned int w;
			unsigned int h;
			
			double timeInternal = 0;
			double timeDelta = 24;
			
			int dataLength = VideoMode::getDesktopMode().width;
			Color * skyAmbient = new Color[dataLength];
			int envs = 0;
			int envr = 0;
			int fires = 0;
			
			bool flags[3];

			Color backColor;
			Color ambientColor;
			
			Controller();
		
			void init();
			void run();
			
			void requestDraw();
			void requestUpdate();
		
	};
	
	class RenderLayer {
		
		public:
			
			Controller * controller;
			
			RenderLayer(Controller * controller);
			
			virtual void draw() = 0;
			virtual void update() = 0;
		
	};
	
	// DATA FLAGS
	#define FLAG_UPDATE_DATA 0
	#define FLAG_CYCLE_DATA 1

	// Y SINE FLAGS
	#define FLAG_USE_Y_SINE 2
	#define FLAG_UPDATE_Y_SINE 3
	#define FLAG_USE_Y_SINE_WANE 4

	// COLOR FLAGS
	#define FLAG_MONOCHROME 5
	#define FLAG_UPDATE_COLOR 6

	// WANE SINE FLAGS
	#define FLAG_USE_WANE_SINE 7
	#define FLAG_UPDATE_WANE_SINE 8
	
	enum Flags
	{
		UpdateData     = 1 << FLAG_UPDATE_DATA,
		CycleData      = 1 << FLAG_CYCLE_DATA,
		UseYSine       = 1 << FLAG_USE_Y_SINE,
		UpdateYSine    = 1 << FLAG_UPDATE_Y_SINE,
		UseYSineWane   = 1 << FLAG_USE_Y_SINE_WANE,
		Monochrome     = 1 << FLAG_MONOCHROME,
		UpdateColor    = 1 << FLAG_UPDATE_COLOR,
		UseWaneSine    = 1 << FLAG_USE_WANE_SINE,
		UpdateWaneSine = 1 << FLAG_UPDATE_WANE_SINE
	};

	constexpr inline Flags operator|(Flags a, Flags b)
	{
		return static_cast<Flags>(static_cast<int>(a) | static_cast<int>(b));
	}
	
	constexpr inline Flags operator&(Flags a, Flags b)
	{
		return static_cast<Flags>(static_cast<int>(a) & static_cast<int>(b));
	}
	
	class FoxFires : public RenderLayer{
			
		protected:
			
			//
			// Controller variables
			//

			int dataLength = VideoMode::getDesktopMode().width;
			char* data = new char[dataLength];

			int currentColor = 0;
			const static int colorsLength = 4;
			const Color colors[colorsLength] = {
				Color(0x00, 0xFF, 0xFF, 0x88),
				Color(0x00, 0xFF, 0x88, 0x88),
				Color(0x88, 0xFF, 0x00, 0x88),
				Color(0xFF, 0x44, 0xFF, 0x88)
			};
			
			//
			// Draw variables
			//
			
			char currentData;
			
			double c;
			double mapI;
			double maskDouble;
			double yMod;
			double yOffset = 0;
			
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
			
			static const Flags defaultFlags = Flags::UpdateData
				| Flags::UseYSine
				| Flags::UpdateYSine
				| Flags::UseYSineWane
				| Flags::UpdateColor
				| Flags::UseWaneSine
				| Flags::UpdateWaneSine;
				
			Flags flags; 
			int coreHeight = 5;
			int bottomMargin = 100;
			double size = 0.9;
			double totalSize = 1;
			
			double ySineOffset = 0;
			double waneSineOffset = 0;
			double colorOffset = 0;
			
			double ySineDelta = 1.25;
			double waneSineDelta = 2.5;
			double colorDelta = 0.001;
			
			FoxFires(Controller * controller);
			
			virtual void postcalc(int i) = 0;
			
			void draw();
			void update();
	};
	
	class Stars : public RenderLayer{
		
		public:
			
			double ** stars;
			unsigned int starsCount;
			
			double blinkOffset = 0;
			double blinkDelta = 2.5;
			bool autoSetDelta = true;
			double hMod = 1;
			int radius = 6;
			Color color;
			
			Stars (Controller * controller);
			
			void draw();
			void update();
	};
	
	class Sky : public RenderLayer{
		
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
			
			void draw();
			void update();
	};
	
	class Ground: public RenderLayer {
		
		public:
			
			Color color1;
			Color color2;
			
			Color mask;
			Color dark;
			
			double sineOffset = 0;
			double sineMod = 1;
			double yOffset = 10;
			double yHeight = 25;
			double haloHeight = 15;
			
			Ground (Controller * controller);
			
			void draw();
			void update();
	};
}
