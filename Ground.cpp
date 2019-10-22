//
// Ilya Yavdoschuk
// FGroup (c) 2019
// 

#include "FoxFires.hpp"
#include <vector>

using namespace FG;

const double prop = 0.9;

Ground::Ground(Controller * controller) : RenderLayer(controller) {
	this->controller = controller;
	
	controller->envs++;
	controller->envr++;
	
	color1 = Color(0xCCCCCCFF);
	color2 = Color(0x888888FF);
	
	mask = Color(0x888888FF);
	dark = Color(0x777777FF);
	
	sineOffset = rand() % 360;
	treesSineOffset = rand() % 360;
}

void Ground::bakeTrees() {
	
}
		
void Ground::draw() {	
	for(int i = 0; i < controller->w; i++)
	{
		double mapI = map(i, 0, controller->w, 0, 360);
		double yMod = map(sin((sineOffset + mapI * sineMod) * M_PI / 180.0), -1, 1, 0, 1);
		
		//std::cout<<(int) controller->skyAmbient[i].r<<" "<<(int) controller->skyAmbient[i].g<<" "<<(int) controller->skyAmbient[i].b<<" "<<(int) controller->skyAmbient[i].a<<std::endl;
		controller->skyAmbient[i] = applyAlpha(controller->skyAmbient[i]) * getMask(prop) + controller->ambientColor * getMask(1 - prop);
		//controller->skyAmbient[i].a = 0xFF;
		//controller->skyAmbient[i] *= controller->ambientColor;
		
				
		Vertex line[] =
		{
			Vertex(Vector2f(i, controller->h - (yMod * (yHeight + haloHeight)) - yOffset), (color1 * controller->skyAmbient[i] + color1) * dark),
			Vertex(Vector2f(i, controller->h - (yMod * yHeight) - yOffset), (color2 * controller->skyAmbient[i] + color2) * dark),
			Vertex(Vector2f(i, controller->h + (yMod * yHeight) + yOffset), (color2 + color2 * mask) * dark)
		};

		controller->window->draw(line, 3, LineStrip);
	}
	
	controller->envs--;
	
	if(controller->envs == 0) {
		for(int i = 0; i < controller->w; i++)
			controller->skyAmbient[i] = Color::Transparent;//controller->ambientColor;
		
		controller->envs = controller->envr;
	}
}

void Ground::update() {}
