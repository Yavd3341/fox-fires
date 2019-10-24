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

void Pine::drawPine(RenderWindow * window, Vector2f pos, Vector2f size, Color log, Color sticks, unsigned int stickCount, unsigned int parts) {

	Vector2f s1l = Vector2f(pos.x, pos.y + size.y) - Vector2f(pos.x + size.x / 2, pos.y);
	Vector2f s2l = Vector2f(pos.x, pos.y + size.y) - Vector2f(pos.x + size.x / 2, pos.y);

	float step = size.y / stickCount;

	Vertex logVertex[] =
	{
		Vertex(Vector2f(pos.x + size.x / 2, pos.y + size.y), log),
		Vertex(Vector2f(pos.x + size.x / 2, pos.y), log)
	};

	window->draw(logVertex, 2, Lines);

	float tmpStep = step / parts;

	Color sky = controller->skyAmbient[(int) pos.x] * Color(0x010101FF);

	int deb = 0;

	for (int i = stickCount; i > 0; i--) {
		Color mappedColor = sticks * getColor(map(i, stickCount, 1, 0xFF, 0x77)) + sky;
		mappedColor.a = 0xFF;

		float tmp = (step * i);

		for (int j = 0; j < parts; j++) {

			std::cout<<deb++<<std::endl;

			float eased = map(ease(map(j, 0, parts, 0, 1)), 1, 0, 1, parts + 2);
			//std::cout<<step<<" "<<j<<" "<<map(j, 0, parts, 0, 1)<<" "<<ease(map(j, 0, parts, 0, 1))<<" "<<eased<<" "<<tmpStep * eased<<std::endl;

			Vector2f base = Vector2f(pos.x + size.x / 2, pos.y + size.y - tmp);

			ConvexShape stick1(3);
			Vector2f s1ln = Vector2f(pos.x, pos.y + size.y) - base;

			stick1.setPoint(0, base);
			stick1.setPoint(1, setVectorLenght(s1ln, map(getVectorLenght(s1ln), 0, getVectorLenght(s1l), tmp, step)) + base);
			stick1.setPoint(2, base + Vector2f(0, tmpStep * eased));
			stick1.setFillColor(mappedColor * getColor(map(j, 0, parts-1, 0x88, 0xDD)));

			window->draw(stick1);

			ConvexShape stick2(3);
			Vector2f s2ln = Vector2f(pos.x + size.x, pos.y + size.y) - base;

			stick2.setPoint(0, base);
			stick2.setPoint(1, setVectorLenght(s2ln, map(getVectorLenght(s2ln), 0, getVectorLenght(s2l), tmp, step)) + base);
			stick2.setPoint(2, base + Vector2f(0, tmpStep * eased));
			stick2.setFillColor(mappedColor * getColor(map(j, 0, parts-1, 0x88, 0xDD)));

			window->draw(stick2);

		}
	}
}

void Pine::draw() {

	Vector2f localSize = size * sizeMult;

	Vector2f winSize = Vector2f(controller->w, controller->h);
	Vector2f tmpPos = position - Vector2f(localSize.x / 2, localSize.y);
	Vector2f tmpSize = Vector2f(localSize.x * winSize.x, localSize.y * winSize.y);
	tmpPos = Vector2f(tmpPos.x * winSize.x, tmpPos.y * winSize.y);

	Pine::drawPine(controller->window, tmpPos, tmpSize, logColor, sticksColor, stickCount, parts);

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
