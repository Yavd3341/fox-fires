#include <SFML/Graphics.hpp>
#include <random>
#include <ctime>
#include <cmath>
#include <stdio.h>

double map(double v, double min1, double max1, double min2, double max2){
	return (v - min1) / max1 * max2 + min2;
}

sf::Color getMask(double val){
	int data = round(map(val, 0, 1, 0, 255));
	return sf::Color(data, data, data);
}

int main()
{
	unsigned int w = 800;
	unsigned int h = 600;
	
	int coreHeight = 20;
	int bottomMargin = 100;
	double size = 0.9;
	
	int yOffset = 0;
	double ySinOffset;
	double colorOffset = 0.5;
	
	double ySinDelta = 1.25;
	double colorDelta = 0.001;
	double colorDeltaManual = colorDelta * 10;
	
	/*
		0 - pause
		1 - fade
		2 - redraw only
		3 - update once
		4 - update color
	*/
	
	bool controls[] = {false, false, false, false, true};
	
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
	sf::Color backColor(0,0,15);
	
	int fps = 60;
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	
	sf::RenderWindow window(sf::VideoMode(w, h), "Fox Fires", sf::Style::Default, settings);
	
	window.setFramerateLimit(fps);
	
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
				{
					controls[0] = !controls[0];
					controls[2] = false;
				}
				
				if (event.key.code == sf::Keyboard::S)
				{
					controls[1] = !controls[1];
					controls[3] = true;
					
					if (controls[0])
						controls[2] = true;
				}
				
				if (event.key.code == sf::Keyboard::C)
					controls[4] = !controls[4];
			}
					
			if (event.type == sf::Event::Resized)
			{
				sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
				window.setView(sf::View(visibleArea));
			}
        }
		
		if (!controls[0] || controls[3])
		{
			
			if (controls[3])
				controls[3] = false;
			
			window.clear(backColor);
			
			w = window.getSize().x;
			h = window.getSize().y;
			
			for(int i = 0; i < dataLength; i++)
			{
				if (i > 0 && !controls[2])
					data[i - 1] = data[i];
				
				char currentData = data[i];
				
				double c = (currentData & 0xFF) / 255.0;
				double mapI = map(i, 0, w, 0, 180);
				double s = controls[1] ? fabs(sin(mapI * M_PI / 180.0)) : 1;
				
				double ySin = sin((ySinOffset + mapI * 2) * M_PI / 180.0);
				yOffset = map(ySin, -1, 1, 0, h * (1 - size) / 2);
				
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
						sf::Vertex(sf::Vector2f(w-i, yOffset + map(h-(h * c), 0, h, 0, h * size)), backColor * mask),
						sf::Vertex(sf::Vector2f(w-i, yOffset + map(h-bottomMargin-coreHeight, 0, h, 0, h * size)), color * mask),
						sf::Vertex(sf::Vector2f(w-i, yOffset + map(h-bottomMargin, 0, h, 0, h * size)), color * mask),
						sf::Vertex(sf::Vector2f(w-i, yOffset + map(h-(bottomMargin-(bottomMargin * c)), 0, h, 0, h * size)), backColor * mask)
					};

					window.draw(line, 4, sf::LineStrip);
				}
			}
			data[dataLength - 1] = 128 + rand() % 128;
			ySinOffset += ySinDelta;
			
			if(controls[4])
				colorOffset += colorDelta;
			
			if(ySinOffset > 360)
				ySinOffset - 360;
			
			if(colorOffset > colorsLength + 1)
				colorOffset -= colorsLength;
		}
        
        window.display();
    }

    return 0;
}
