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
			yOffset = map(i, 0, controller->w, 1, 0);
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
		skyAmbient[i] = ambientColor;
	
	CustomFoxFires * ff1 = new CustomFoxFires(this);
	CustomFoxFires * ff2 = new CustomFoxFires(this);
	CustomFoxFires * ff3 = new CustomFoxFires(this);
	
	ff1->cffOffset = (h / 7.0);
	
	ff2->ySineOffset = 90;
	ff2->waneSineOffset = 90;
	ff2->colorOffset = 3;
	
	ff3->cffOffset = -(h / 7.0);
	ff3->ySineOffset = 180;
	ff3->waneSineOffset = 180;
	ff3->colorOffset = 6;
	
	Ground * grd1 = new Ground(this);
	Ground * grd2 = new Ground(this);
	Ground * grd3 = new Ground(this);
	
	grd1->yOffset = h / 30.0 * 3;
	grd1->sineMod = (rand() % 3 + 1) / 2.0;
	grd1->dark = Color(0xCCCCCCFF);
	
	grd2->yOffset = h / 30.0 * 2;
	grd2->sineMod = (rand() % 3 + 1) / 2.0;
	grd2->dark = Color(0x999999FF);
	
	grd3->yOffset = h / 30.0 * 1;
	grd3->sineMod = (rand() % 3 + 1) / 2.0;
	grd3->dark = Color(0x555555FF);
	
	layers.push_back(new Sky(this));
	layers.push_back(new Stars(this));
	
	layers.push_back(grd1);
	layers.push_back(grd2);
	layers.push_back(grd3);
	
	layers.push_back(ff1);
	layers.push_back(ff2);
	layers.push_back(ff3);
	
	Pine * p = new Pine(this, Vector2f(0.5, 1), 0.25);
	p->stickCount = 10;
	
	layers.push_back(p);
	
	settings.antialiasingLevel = 8;
	
	window = new RenderWindow(flags[FLAG_FULLSCREEN] ? VideoMode::getDesktopMode() : VideoMode(w, h),
		"Fox Fires", flags[FLAG_FULLSCREEN] ? Style::Fullscreen : Style::Default, settings);
	window->setFramerateLimit(fps);
	window->setMouseCursorVisible(!flags[FLAG_FULLSCREEN]);
	
	srand(time(NULL));
	
	//timeInternal = 86400 / 3;
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
		
		if(!flags[FLAG_NODRAW])
			requestDraw();
			
		if(!flags[FLAG_PAUSE])
			requestUpdate();
        
        window->display();
    }
}

void Controller::requestDraw() {
    window->clear(backColor);
        
	for (RenderLayer * layer : layers)
		layer->draw();
}

void Controller::requestUpdate() {
	timeInternal += timeDelta;
	if (timeInternal > 86400)
		timeInternal -= 86400;
		
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
