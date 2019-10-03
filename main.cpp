#include <SFML/Graphics.hpp>
#include <random>
#include <ctime>
#include <cmath>
#include <stdio.h>

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

/*
	0 - pause
	1 - fade
	2 - no draw
	3 - update sine
	4 - update color
	5 - update wane
	6 - use wane
	7 - fullscreen
*/

bool controls[] = {false, false, false, true, true, true, true, false};

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
		double s = controls[1] ? fabs(sin(mapI * M_PI / 180.0)) : 1;

		double ySine = sin((ySineOffset + mapI * 2) * M_PI / 180.0);
		int yOffset = map(ySine, -1, 1, 0, h * (1 - size));

		if(controls[6])
		{
			double waneSine = sin((waneSineOffset + mapI * 4) * M_PI / 180.0);
			double waneStage = map(waneSine, -1, 1, 0.8, 1);
			s *= waneStage;
		}

		double oneI = map(i, 0, w, 0, 1);
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

		sf::Color color1 = colors[colorBaseIndex % colorsLength];
		sf::Color color2 = colors[(colorBaseIndex + 1) % colorsLength];

		sf::Color mask1 = getMask(x1); 
		sf::Color mask2 = getMask(x2);

		sf::Color color = color1 * mask2 + color2 * mask1;
		sf::Color mask(s * 255, s * 255, s * 255, s * 255);

		if (w-i < w)
		{
			sf::Vertex line[] =
			{
				sf::Vertex(sf::Vector2f(w-i, yOffset + map(h-(h * c), 0, h, 0, h * size)), backColor),
				sf::Vertex(sf::Vector2f(w-i, yOffset + map(h-bottomMargin-coreHeight, 0, h, 0, h * size)), color * mask),
				sf::Vertex(sf::Vector2f(w-i, yOffset + map(h-bottomMargin, 0, h, 0, h * size)), color * mask),
				sf::Vertex(sf::Vector2f(w-i, yOffset + map(h-(bottomMargin-(bottomMargin * c)), 0, h, 0, h * size)), backColor)
			};

			window->draw(line, 4, sf::LineStrip);
		}
	}
}

int main()
{
	int fps = 60;
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	
	sf::RenderWindow window(controls[7] ? sf::VideoMode::getDesktopMode() : sf::VideoMode(w, h), "Fox Fires", controls[7] ? sf::Style::Fullscreen : sf::Style::Default, settings);
	window.setFramerateLimit(fps);
	window.setMouseCursorVisible(!controls[7]);
	
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
					controls[0] = !controls[0];
				
				if (event.key.code == sf::Keyboard::F)
					controls[1] = !controls[1];
				
				if (event.key.code == sf::Keyboard::S)
					controls[3] = !controls[3];
				
				if (event.key.code == sf::Keyboard::C)
					controls[4] = !controls[4];
				
				if (event.key.code == sf::Keyboard::W)
				{
					if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift))
						controls[6] = !controls[6];
					else
						controls[5] = !controls[5];
				}
				
				if (event.key.code == sf::Keyboard::M)
				{
					controls[7] = !controls[7];
					window.create(controls[7] ? sf::VideoMode::getDesktopMode() : sf::VideoMode(w / 2, h / 2), "Fox Fires", controls[7] ? sf::Style::Fullscreen : sf::Style::Default, settings);
					window.setFramerateLimit(fps);
					window.setMouseCursorVisible(!controls[7]);
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
		
		if(!controls[2])
			draw(&window);

		if(!controls[0]){
			for(int i = 0; i < dataLength; i++)
				if (i > 0)
					data[i - 1] = data[i];
				
			data[dataLength - 1] = 128 + rand() % 128;
		}
		
		if(controls[3])
			ySineOffset += ySineDelta;
		
		if(controls[4])
			colorOffset += colorDelta;
		
		if(controls[5])
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
