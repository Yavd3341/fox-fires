#include <SFML/Graphics.hpp>
#include <random>
#include <ctime>
#include <stdio.h>

int main()
{
	unsigned int w = 800;
	unsigned int h = 600;
	
	int bottomMargin = 100;
	int coreHeight = 20;
	
	int FPS = 60;
	
	int dataLength = sf::VideoMode::getDesktopMode().width;
	char* data = new char[dataLength];
	
	sf::Color mainColor(0, 255 * 0.9, 255 * 0.9);
	sf::Color backColor(0,0,15);
	
	sf::ContextSettings settings;
	//settings.antialiasingLevel = 8;
	
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
                
            if (event.type == sf::Event::KeyPressed)
				if (event.key.code == sf::Keyboard::Escape || event.key.code == sf::Keyboard::Space)
					window.close();
					
			if (event.type == sf::Event::Resized)
			{
				sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
				window.setView(sf::View(visibleArea));
			}
        }

        window.clear(backColor);
        
        w = window.getSize().x;
        h = window.getSize().y;
        
        for(int i = 0; i < dataLength; i++)
        {
        	char currentData = data[i];
        	
        	sf::Color color(currentData, currentData, currentData);
        	
        	double c = (currentData & 0xFF) / 255.0;
        	
        	if (i > 0)
        		data[i - 1] = data[i];
        	
        	if (w-i < w)
        	{
		    	sf::Vertex line[] =
				{
					sf::Vertex(sf::Vector2f(w-i, h-(h * c)), backColor),
					sf::Vertex(sf::Vector2f(w-i, h-bottomMargin-coreHeight), mainColor),
					sf::Vertex(sf::Vector2f(w-i, h-bottomMargin), mainColor),
					sf::Vertex(sf::Vector2f(w-i, h-(bottomMargin-(bottomMargin * c))), backColor)
				};

				window.draw(line, 4, sf::LineStrip);
			}
        }
		data[dataLength - 1] = 128 + rand() % 128;
		//sf::sleep(sf::milliseconds(1000/FPS));
        
        window.display();
    }

    return 0;
}
