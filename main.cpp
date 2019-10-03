#include <SFML/Graphics.hpp>
#include <random>
#include <ctime>
#include <cmath>
#include <stdio.h>

double map(double v, double min1, double max1, double min2, double max2){
	return (v - min1) / max1 * max2 + min2;
}

int main()
{
	unsigned int w = 800;
	unsigned int h = 600;
	
	int coreHeight = 20;
	int bottomMargin = 150;
	
	double size = 0.9;
	
	int yOffset = 0;
	double sinOffset;
	
	double sinDelta = 1.25;
	
	bool pause = false;
	
	int FPS = 60;
	
	int dataLength = sf::VideoMode::getDesktopMode().width;
	char* data = new char[dataLength];
	
	sf::Color topColor(0, 255 * 0.9, 255 * 0.9);
	sf::Color bottomColor(0, 255 * 0.9, 192 * 0.9);
	sf::Color backColor(0,0,15);
	
	//backColor = sf::Color::Red;
	
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	
	sf::RenderWindow window(sf::VideoMode(w, h), "Fox Fires", sf::Style::Default, settings);
	
	window.setFramerateLimit(60);
	
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
                
            if (event.type == sf::Event::KeyPressed){
				if (event.key.code == sf::Keyboard::Escape)
					window.close();
				
				if (event.key.code == sf::Keyboard::Space)
					pause = !pause;
			}
					
			if (event.type == sf::Event::Resized)
			{
				sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
				window.setView(sf::View(visibleArea));
			}
        }
		
		if (!pause){
			window.clear(backColor);
			
			w = window.getSize().x;
			h = window.getSize().y;
			
			for(int i = 0; i < dataLength; i++)
			{
				char currentData = data[i];
				
				double c = (currentData & 0xFF) / 255.0;
				double mapI = map(i, 0, w, 0, 180);
				double s = 1; //fabs(sin(mapI * M_PI / 180.0));
				
				sf::Color mask(s * 255, s * 255, s * 255, s * 255);
				
				yOffset = map(sin((sinOffset + mapI * 2) * M_PI / 180.0), -1, 1, 0, h * (1 - size) / 2);
				
				if (i > 0)
					data[i - 1] = data[i];
				
				if (w-i < w)
				{
					sf::Vertex line[] =
					{
						sf::Vertex(sf::Vector2f(w-i, yOffset + map(h-(h * c), 0, h, 0, h * size)), backColor * mask),
						sf::Vertex(sf::Vector2f(w-i, yOffset + map(h-bottomMargin-coreHeight, 0, h, 0, h * size)), topColor * mask),
						sf::Vertex(sf::Vector2f(w-i, yOffset + map(h-bottomMargin, 0, h, 0, h * size)), bottomColor * mask),
						sf::Vertex(sf::Vector2f(w-i, yOffset + map(h-(bottomMargin-(bottomMargin * c)), 0, h, 0, h * size)), backColor * mask)
					};

					window.draw(line, 4, sf::LineStrip);
				}
			}
			data[dataLength - 1] = 128 + rand() % 128;
			sinOffset += sinDelta;
			
			if(sinOffset > 360)
				sinOffset - 360;
		}
        
        window.display();
    }

    return 0;
}
