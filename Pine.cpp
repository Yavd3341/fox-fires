//
// Ilya Yavdoschuk
// FGroup (c) 2019
//

#include "FoxFires.hpp"

using namespace FG;

Vector2f Pine::defaultSize = Vector2f(0.1999995, 1);

bool drawBoundingBox = false;

const double prop = 0.7;

float ease(float f) {
	return pow(f, 2);
}

Color getColor(char i) {
	return Color(i, i, i, 0xFF);
}

Pine::Pine(Controller * controller, Vector2f position, Vector2f size) : RenderLayer(controller) {
	this->controller = controller;
	this->position = position;
	this->size = size;

	logColor = Color(0x2A2016FF);
	sticksColor = Color(0x01796FFF);
}

Pine::Pine(Controller * controller, Vector2f position, float sizeMult) : RenderLayer(controller) {
	this->controller = controller;
	this->position = position;
	this->sizeMult = sizeMult;
	this->size = Pine::defaultSize;

	logColor = Color(0x2A2016FF);
	sticksColor = Color(0x01796FFF);
}

void Pine::drawPine(RenderWindow * window, Vector2f pos, Vector2f size, Color log, Color sticks, unsigned int stickCount) {

	Vector2f s1l = Vector2f(pos.x, pos.y + size.y) - Vector2f(pos.x + size.x / 2, pos.y);
	Vector2f s2l = Vector2f(pos.x, pos.y + size.y) - Vector2f(pos.x + size.x / 2, pos.y);

	float step = size.y / stickCount;

	Vertex logVertex[] =
	{
		Vertex(Vector2f(pos.x + size.x / 2, pos.y + size.y), log),
		Vertex(Vector2f(pos.x + size.x / 2, pos.y), log)
	};

	window->draw(logVertex, 2, Lines);
	
	int px = pos.x;
	
	if (px < 0)
		px = 0;
	else if(px > controller->w - 1)
		px = controller->w - 1;
		
	px = map(px, 0, controller->w, 0, controller->dataLength);

	Color sky = controller->skyAmbient[px] * Color(0x222222FF);
	Color shade = getColor(0x88);

	for (int i = 1; i <= stickCount; i++) {
		Color mappedColor = (sticks * getColor(map(i, stickCount, 1, 0xFF, 0x88)) + sky) * (controller->backColor + Color(0x666666FF));
		Color layerShade = getColor(map(i, 1, stickCount, 0x88, 0xFF));
		mappedColor.a = 0xFF;

		float tmp = (step * i);
 
		Vector2f base = Vector2f(pos.x + size.x / 2, pos.y + size.y - tmp);
		
		Vector2f s1ln = Vector2f(pos.x, pos.y + size.y) - base;
		Vertex stick1[] = {
			Vertex(base, mappedColor * layerShade),
			Vertex(setVectorLenght(s1ln, map(getVectorLenght(s1ln), 0, getVectorLenght(s1l), tmp, step)) + base, mappedColor * shade * layerShade),
			Vertex(base + Vector2f(0, step * 1.5), mappedColor * layerShade)
		};
		window->draw(stick1, 3, Triangles);

		Vector2f s2ln = Vector2f(pos.x + size.x, pos.y + size.y) - base;
		Vertex stick2[] = {
			Vertex(base, mappedColor * layerShade),
			Vertex(setVectorLenght(s2ln, map(getVectorLenght(s2ln), 0, getVectorLenght(s2l), tmp, step)) + base, mappedColor * shade * layerShade),
			Vertex(base + Vector2f(0, step * 1.5), mappedColor * layerShade)
		};
		window->draw(stick2, 3, Triangles);
	}
}

void Pine::draw() {

	Vector2f localSize = size * sizeMult;

	Vector2f winSize = Vector2f(controller->w, controller->h);
	Vector2f tmpPos = position - Vector2f(localSize.x / 2, localSize.y);
	Vector2f tmpSize = Vector2f(localSize.x * winSize.x, localSize.y * winSize.y);
	tmpPos = Vector2f(tmpPos.x * winSize.x, tmpPos.y * winSize.y);

	Pine::drawPine(controller->window, tmpPos, tmpSize, logColor, sticksColor, stickCount);

	if (drawBoundingBox) {
		RectangleShape rectangle;
		rectangle.setSize(tmpSize);
		rectangle.setOutlineColor(Color::Red);
		rectangle.setFillColor(Color::Transparent);
		rectangle.setOutlineThickness(1);
		rectangle.setPosition(tmpPos);
		controller->window->draw(rectangle);

		CircleShape circle;
		circle.setRadius(2);
		circle.setFillColor(sf::Color::Yellow);
		circle.setPosition(tmpPos - Vector2f(2, 2));
		controller->window->draw(circle);

		CircleShape circle2;
		circle2.setRadius(2);
		circle2.setFillColor(sf::Color::Green);
		circle2.setPosition(Vector2f(position.x * winSize.x, position.y * winSize.y) - Vector2f(2, 2));
		controller->window->draw(circle2);
	}
}

void Pine::update() {}
