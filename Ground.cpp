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

	layers = std::vector<RenderLayer*>();

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
	for (auto p : layers)
		delete p;
	layers.clear();

	float treeSize = 0.25;
	int treesCount = rand() % (int) ceil(1 / (Pine::defaultSize.x * treeSize));

	std::cout<<treesCount<<std::endl<<std::endl;

	for(int i = 0; i < treesCount; i++)
	{
		double x = controller->w * ((Pine::defaultSize.x * treeSize) / 2 + i * (Pine::defaultSize.x * treeSize));

		double mapI = map(x, 0, controller->w, 0, 360);
		double yMod = map(sin((sineOffset + mapI * sineMod) * M_PI / 180.0), -1, 1, 0, 1);

		double y = controller->h - (yMod * yHeight) - yOffset;

		std::cout<<x<<" "<<mapI<<" "<<yMod<<" "<<y<<std::endl;

		Pine * pine = new Pine(controller, Vector2f(x, y), 0.25);
		layers.push_back(pine);
	}

	std::cout<<std::endl;
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
		for(int i = 0; i < controller->dataLength; i++)
			controller->skyAmbient[controller->dataLength - i - 1] = Color::Transparent;//controller->ambientColor;

		controller->envs = controller->envr;
	}

	for (RenderLayer * layer : layers)
		layer->draw();
}

void Ground::update() {
	//std::cout<<"Update"<<std::endl;
	for (RenderLayer * layer : layers)
		layer->update();
}
