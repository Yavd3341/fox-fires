//
// Ilya Yavdoschuk
// FGroup (c) 2019
// 

#include "FoxFires.hpp"
#include <vector>

using namespace FG;

Environment::Environment(Controller * controller) : RenderLayer(controller) {
	this->controller = controller;
	
	controller->envs++;
	controller->envr++;
	
	color1 = Color(0xAAAAAAFF);
	color2 = Color(0x888888FF);
	
	mask = Color(0x888888FF);
	dark = Color(0x777777FF);
}
		
void Environment::draw() {	
	for(int i = 0; i < controller->w; i++)
	{
		double mapI = map(i, 0, controller->w, 0, 360);
		double yMod = map(sin((sineOffset + mapI * sineMod) * M_PI / 180.0), -1, 1, 0, 1);
		
		controller->sky[i].a = 0xFF;
				
		Vertex line[] =
		{
			Vertex(Vector2f(i, controller->h - (yMod * (yHeight + haloHeight)) - yOffset), (color1 * controller->sky[i] + color1) * dark),
			Vertex(Vector2f(i, controller->h - (yMod * yHeight) - yOffset), (color2 * controller->sky[i] + color2) * dark),
			Vertex(Vector2f(i, controller->h + (yMod * yHeight) + yOffset), (color2 + color2 * mask) * dark)
		};

		controller->window->draw(line, 3, LineStrip);
	}
	
	controller->envs--;
	
	if(controller->envs == 0) {
		for(int i = 0; i < controller->w; i++)
			controller->sky[i] = Color(0xFFFFFF11);
		
		controller->envs = controller->envr;
	}
}

void Environment::update() {}
