//
// Ilya Yavdoschuk
// FGroup (c) 2019
// 

#include "FoxFires.hpp"
#include <random>
#include <ctime>
#include <vector>

using namespace FG;

class CustomFoxFires : public FoxFires {
	
	public:
		
		int cffOffset = 0;
		
		CustomFoxFires(Controller * controller) : FoxFires(controller) {}
		
		void postcalc(int i) {
			totalSize = map(i, 0, controller->w, 0.2,0.9);
			yOffset = ease(map(i, 0, controller->w, 1, 0));
			yOffset = map(1 - yOffset, 0, 1, controller->h - (controller->h * 0.6), -(controller->h * 0.3)) + cffOffset;
		}
	
};

std::vector<RenderLayer*> layers;

Controller::Controller() {
	backColor = Color(0,0,25);
	w = 800;
	h = 600;
}

	
int fps = 60;
ContextSettings settings;

void Controller::init() {
	
	// Setting flags
	
	flags[FLAG_NODRAW] = false;
	flags[FLAG_FULLSCREEN] = false;
	flags[FLAG_PAUSE] = false;
	
	// Continue init

	for(int i = 0; i < dataLength; i++)
		sky[i] = Color(0xFFFFFF11);
	
	CustomFoxFires * ff1 = new CustomFoxFires(this);
	CustomFoxFires * ff2 = new CustomFoxFires(this);
	CustomFoxFires * ff3 = new CustomFoxFires(this);
	
	ff1->cffOffset = 75;
	
	ff2->ySineOffset = 90;
	ff2->waneSineOffset = 90;
	ff2->colorOffset = 3;
	
	ff3->cffOffset = -75;
	ff3->ySineOffset = 180;
	ff3->waneSineOffset = 180;
	ff3->colorOffset = 6;
	
	Environment * env1 = new Environment(this);
	Environment * env2 = new Environment(this);
	Environment * env3 = new Environment(this);
	
	env1->yOffset = 100;
	env1->sineOffset = rand() % 360;
	env1->sineMod = (rand() % 3 + 1) / 2.0;
	env1->dark = Color(0xBBBBBBFF);
	
	env2->yOffset = 70;
	env2->sineOffset = rand() % 360;
	env2->sineMod = (rand() % 3 + 1) / 2.0;
	env2->dark = Color(0x888888FF);
	
	env3->yOffset = 40;
	env3->sineOffset = rand() % 360;
	env3->sineMod = (rand() % 3 + 1) / 2.0;
	env3->dark = Color(0x555555FF);
	
	layers.push_back(new Stars(this));
	
	layers.push_back(env1);
	layers.push_back(env2);
	layers.push_back(env3);
	
	layers.push_back(ff1);
	layers.push_back(ff2);
	layers.push_back(ff3);
	
	settings.antialiasingLevel = 8;
	
	window = new RenderWindow(flags[FLAG_FULLSCREEN] ? VideoMode::getDesktopMode() : VideoMode(w, h),
		"Fox Fires", flags[FLAG_FULLSCREEN] ? Style::Fullscreen : Style::Default, settings);
	window->setFramerateLimit(fps);
	window->setMouseCursorVisible(!flags[FLAG_FULLSCREEN]);
	
	srand(time(NULL));
}

void Controller::run() {
	while (window->isOpen())
    {
        Event event;
        while (window->pollEvent(event))
        {
            if (event.type == Event::Closed)
                window->close();
                
            if (event.type == Event::KeyPressed)
			{
				if (event.key.code == Keyboard::Escape)
					window->close();
				
				if (event.key.code == Keyboard::Space)
					flags[FLAG_PAUSE] = !flags[FLAG_PAUSE];	
				
				if (event.key.code == Keyboard::F || event.key.code == Keyboard::F11)
				{
					flags[FLAG_FULLSCREEN] = !flags[FLAG_FULLSCREEN];
					window->create(flags[FLAG_FULLSCREEN] ? VideoMode::getDesktopMode() : VideoMode(w, h),
						"Fox Fires", flags[FLAG_FULLSCREEN] ? Style::Fullscreen : Style::Default, settings);
					window->setFramerateLimit(fps);
					window->setMouseCursorVisible(!flags[FLAG_FULLSCREEN]);
				}	
			}
					
			if (event.type == Event::Resized)
			{
				FloatRect visibleArea(0, 0, event.size.width, event.size.height);
				window->setView(View(visibleArea));
			}
        }
			
		w = window->getSize().x;
		h = window->getSize().y;
		
		requestDraw();
		requestUpdate();
        
        window->display();
    }
}

void Controller::requestDraw() {
    window->clear(backColor);
        
	if(!flags[FLAG_NODRAW])
		for (RenderLayer * layer : layers)
			layer->draw();
}

void Controller::requestUpdate() {
	if(!flags[FLAG_PAUSE])
		for (RenderLayer * layer : layers)
			layer->update();
}

int main()
{
	Controller controller;
	controller.init();
	controller.run();

    return 0;
}
