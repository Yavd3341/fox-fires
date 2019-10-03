#include <SFML/Graphics.hpp>
#include <random>
#include <ctime>
#include <cmath>
#include <stdio.h>

// DRAW (MAIN) FLAGS
#define FLAG_NODRAW 0
#define FLAG_FULLSCREEN 1
#define FLAG_FADE_OUT 2
#define FLAG_PAUSE 3

// DATA FLAGS
#define FLAG_UPDATE_DATA 4
#define FLAG_CYCLE_DATA 5

// Y SINE FLAGS
#define FLAG_USE_Y_SINE 6
#define FLAG_UPDATE_Y_SINE 7
#define FLAG_USE_Y_SINE_WANE 8

// COLOR FLAGS
#define FLAG_MONOCHROME 9
#define FLAG_UPDATE_COLOR 10

// WANE SINE FLAGS
#define FLAG_USE_WANE_SINE 11
#define FLAG_UPDATE_WANE_SINE 12

unsigned int w = 800;
unsigned int h = 600;

int coreHeight = 5;
int bottomMargin = 100;
double size = 0.9;

double ySineOffset;
double waneSineOffset;
double colorOffset = 0.5;

double ySineDelta = 1.25;
double waneSineDelta = 2.5;
double colorDelta = 0.001;
double colorDeltaManual = colorDelta * 10;

bool flags[13];

int dataLength = sf::VideoMode::getDesktopMode().width;
char* data = new char[dataLength];

int currentColor = 0;
const int colorsLength = 4;
sf::Color colors[colorsLength] = {
	sf::Color(0, 255 * 0.9, 255 * 0.9),
	sf::Color(0, 255 * 0.9, 192 * 0.9),
	sf::Color(192 * 0.9, 255 * 0.9, 0),
	sf::Color(255 * 0.9, 64 * 0.9, 255 * 0.9)
};
sf::Color backColor(0,0,25,0);

double map(double v, double min1, double max1, double min2, double max2){
	return (v - min1) / (max1 - min1) * (max2 - min2) + min2;
}

sf::Color getMask(double val){
	int data = round(map(val, 0, 1, 0, 255));
	return sf::Color(data, data, data);
}

void draw(sf::RenderWindow * window){
	window->clear(backColor);

	for(int i = 0; i < w; i++)
	{		
		char currentData = data[i];
				
		double c = (currentData & 0xFF) / 255.0;
		double mapI = map(i, 0, w, 0, 180);
		double maskDouble = flags[FLAG_FADE_OUT] ? fabs(sin(mapI * M_PI / 180.0)) : 1;
		double yMod = flags[FLAG_USE_Y_SINE] ? map(sin((ySineOffset + mapI * 2) * M_PI / 180.0), -1, 1, 0, 1) : 0;
		
		if(flags[FLAG_USE_Y_SINE_WANE])
			maskDouble *= map(yMod, 0, 1, 1, 0.8);

		if(flags[FLAG_USE_WANE_SINE])
		{
			double waneSine = sin((waneSineOffset + mapI * 4) * M_PI / 180.0);
			double waneStage = map(waneSine, -1, 1, flags[FLAG_USE_Y_SINE_WANE] && flags[FLAG_USE_Y_SINE] ? 0.9: 0.8, 1);
			maskDouble *= waneStage;
		}

		double oneI = flags[FLAG_MONOCHROME] ? 0.5 : map(i, 0, w, 0, 1);
		double x1 = colorOffset;
		int colorBaseIndex = (int) x1;
		
		x1 -= colorBaseIndex;
		x1 += oneI;

		if(x1 >= 1)
		{
			colorBaseIndex++;
			x1--;
		}

		double x2 = 1 - x1;
		
		sf::Color color = colors[colorBaseIndex % colorsLength];
		sf::Color mixedColor = colors[(colorBaseIndex + 1) % colorsLength];

		sf::Color mask1 = getMask(x2); 
		sf::Color mask2 = getMask(x1);

		color = color * mask1 + mixedColor * mask2;
		
		sf::Color mask(maskDouble * 255, maskDouble * 255, maskDouble * 255, maskDouble * 255);

		if (w-i < w)
		{
			sf::Vertex line[] =
			{
				sf::Vertex(sf::Vector2f(w-i, (yMod * h * (1 - size)) + map(h-(h * c), 0, h, 0, h * size)), backColor),
				sf::Vertex(sf::Vector2f(w-i, (yMod * h * (1 - size)) + map(h-bottomMargin-coreHeight, 0, h, 0, h * size)), color * mask),
				sf::Vertex(sf::Vector2f(w-i, (yMod * h * (1 - size)) + map(h-bottomMargin, 0, h, 0, h * size)), color * mask),
				sf::Vertex(sf::Vector2f(w-i, (yMod * h * (1 - size)) + map(h-(bottomMargin-(bottomMargin * c)), 0, h, 0, h * size)), backColor)
			};

			window->draw(line, 4, sf::LineStrip);
		}
	}
}

int main()
{
	// Setting flags
	
	flags[FLAG_NODRAW] = false;
	flags[FLAG_FULLSCREEN] = false;
	flags[FLAG_FADE_OUT] = false;
	flags[FLAG_PAUSE] = false;

	flags[FLAG_UPDATE_DATA] = true;
	flags[FLAG_CYCLE_DATA] = false;

	flags[FLAG_USE_Y_SINE] = true;
	flags[FLAG_UPDATE_Y_SINE] = true;
	flags[FLAG_USE_Y_SINE_WANE] = true;

	flags[FLAG_MONOCHROME] = false;
	flags[FLAG_UPDATE_COLOR] = true;

	flags[FLAG_USE_WANE_SINE] = true;
	flags[FLAG_UPDATE_WANE_SINE] = true;
	
	// Continue init
	
	int fps = 60;
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	
	sf::RenderWindow window(flags[FLAG_FULLSCREEN] ? sf::VideoMode::getDesktopMode() : sf::VideoMode(w, h),
		"Fox Fires", flags[FLAG_FULLSCREEN] ? sf::Style::Fullscreen : sf::Style::Default, settings);
	window.setFramerateLimit(fps);
	window.setMouseCursorVisible(!flags[FLAG_FULLSCREEN]);
	
	srand(time(NULL));
	
	for(int i = 0; i < dataLength; i++)
		data[i] = 128 + rand() % 128;
	
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
                
            if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Escape)
					window.close();
				
				if (event.key.code == sf::Keyboard::Right)
				{
					if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift))
						colorOffset += 1;
					else
						colorOffset += colorDeltaManual;
						
					if(colorOffset > colorsLength + 1)
						colorOffset -= colorsLength;
				}
				
				if (event.key.code == sf::Keyboard::Left)
				{
					if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift))
						colorOffset -= 1;
					else
						colorOffset -= colorDeltaManual;
					
					if(colorOffset < 0)
						colorOffset += colorsLength;
				}
				
				if (event.key.code == sf::Keyboard::Space)
					flags[FLAG_PAUSE] = !flags[FLAG_PAUSE];
				
				if (event.key.code == sf::Keyboard::D)
				{
					if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift))
						flags[FLAG_CYCLE_DATA] = !flags[FLAG_CYCLE_DATA];
					else
						flags[FLAG_UPDATE_DATA] = !flags[FLAG_UPDATE_DATA];
				}
				
				if (event.key.code == sf::Keyboard::F || event.key.code == sf::Keyboard::F11)
				{
					flags[FLAG_FULLSCREEN] = !flags[FLAG_FULLSCREEN];
					window.create(flags[FLAG_FULLSCREEN] ? sf::VideoMode::getDesktopMode() : sf::VideoMode(w / 2, h / 2),
						"Fox Fires", flags[FLAG_FULLSCREEN] ? sf::Style::Fullscreen : sf::Style::Default, settings);
					window.setFramerateLimit(fps);
					window.setMouseCursorVisible(!flags[FLAG_FULLSCREEN]);
				}
				
				if (event.key.code == sf::Keyboard::S)
				{
					if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift))
						flags[FLAG_USE_Y_SINE] = !flags[FLAG_USE_Y_SINE];
					else if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))
						flags[FLAG_USE_Y_SINE_WANE] = !flags[FLAG_USE_Y_SINE_WANE];
					else
						flags[FLAG_UPDATE_Y_SINE] = !flags[FLAG_UPDATE_Y_SINE];
				}
				
				if (event.key.code == sf::Keyboard::O)
					flags[FLAG_FADE_OUT] = !flags[FLAG_FADE_OUT];
				
				if (event.key.code == sf::Keyboard::C)
				{
					if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift))
						flags[FLAG_MONOCHROME] = !flags[FLAG_MONOCHROME];
					else
						flags[FLAG_UPDATE_COLOR] = !flags[FLAG_UPDATE_COLOR];
				}
				
				if (event.key.code == sf::Keyboard::W)
				{
					if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift))
						flags[FLAG_USE_WANE_SINE] = !flags[FLAG_USE_WANE_SINE];
					else
						flags[FLAG_UPDATE_WANE_SINE] = !flags[FLAG_UPDATE_WANE_SINE];
				}				
			}
					
			if (event.type == sf::Event::Resized)
			{
				sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
				window.setView(sf::View(visibleArea));
			}
        }
			
		w = window.getSize().x;
		h = window.getSize().y;
		
		if(!flags[FLAG_NODRAW])
			draw(&window);

		if(flags[FLAG_UPDATE_DATA] && !flags[FLAG_PAUSE]){
			int firstData = data[0];
			for(int i = 0; i < dataLength; i++)
				if (i > 0)
					data[i - 1] = data[i];
			
			if(flags[FLAG_CYCLE_DATA])
				data[dataLength - 1] = firstData;
			else
				data[dataLength - 1] = 128 + rand() % 128;
		}
		
		if(flags[FLAG_UPDATE_Y_SINE] && flags[FLAG_USE_Y_SINE] && !flags[FLAG_PAUSE])
			ySineOffset += ySineDelta;
		
		if(flags[FLAG_UPDATE_COLOR] && !flags[FLAG_MONOCHROME] && !flags[FLAG_PAUSE])
			colorOffset += colorDelta;
		
		if(flags[FLAG_UPDATE_WANE_SINE] && flags[FLAG_USE_WANE_SINE] && !flags[FLAG_PAUSE])
			waneSineOffset += waneSineDelta;
		
		if(ySineOffset > 360)
			ySineOffset - 360;
		
		if(waneSineOffset > 360)
			waneSineOffset - 360;
		
		if(colorOffset > colorsLength + 1)
			colorOffset -= colorsLength;
        
        window.display();
    }

    return 0;
}
