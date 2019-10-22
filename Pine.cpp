//
// Ilya Yavdoschuk
// FGroup (c) 2019
//

#include "FoxFires.hpp"

using namespace FG;

bool drawBoundingBox = false;

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

void Pine::drawWithLines(RenderWindow * window, Vector2f pos, Vector2f size, Color log, Color sticks, unsigned int stickCount) {
	
	Vector2f stick1long = Vector2f(pos.x, pos.y + size.y) - Vector2f(pos.x + size.x / 2, pos.y);
	Vector2f stick2long = Vector2f(pos.x, pos.y + size.y) - Vector2f(pos.x + size.x / 2, pos.y);
	
	float step = size.y / stickCount;
		
	Vertex logVertex[] =
	{
		Vertex(Vector2f(pos.x + size.x / 2, pos.y + size.y), log),
		Vertex(Vector2f(pos.x + size.x / 2, pos.y), log)
	};

	window->draw(logVertex, 2, Lines);
	
	for (int i = 1; i <= stickCount; i++) {
		
		float tmp = (step * i);
		
		Vector2f base = Vector2f(pos.x + size.x / 2, pos.y + size.y - tmp);
		
		Vector2f stick1 = Vector2f(pos.x, pos.y + size.y) - base;
		Vertex stick1Vertex[] =
		{
			Vertex(base, sticks),
			Vertex(setVectorLenght(stick1, map(getVectorLenght(stick1), 0, getVectorLenght(stick1long), tmp, step)) + base, sticks)
		};

		window->draw(stick1Vertex, 2, Lines);
		
		Vector2f stick2 = Vector2f(pos.x + size.x, pos.y + size.y) - base;
		Vertex stick2Vertex[] =
		{
			Vertex(base, sticks),
			Vertex(setVectorLenght(stick2, map(getVectorLenght(stick2), 0, getVectorLenght(stick2long), tmp, step)) + base, sticks)
		};

		window->draw(stick2Vertex, 2, Lines);
	}
}

void Pine::drawWithTriangles(RenderWindow * window, Vector2f pos, Vector2f size, Color log, Color sticks, unsigned int stickCount, unsigned int parts) {
	
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
		
	for (int i = stickCount; i > 0; i--) {
		
		float tmp = (step * i);
		
		for (int j = 0; j < parts; j++) {
		
			Vector2f base = Vector2f(pos.x + size.x / 2, pos.y + size.y - tmp);
			
			ConvexShape stick1(3);
			Vector2f s1ln = Vector2f(pos.x, pos.y + size.y) - base;
			
			stick1.setPoint(0, base);
			stick1.setPoint(1, setVectorLenght(s1ln, map(getVectorLenght(s1ln), 0, getVectorLenght(s1l), tmp, step)) + base);
			stick1.setPoint(2, base + Vector2f(0, tmpStep * (parts - j + 2)));
			stick1.setFillColor(sticks * getColor(map(j, 0, parts-1, 0x88, 0xDD)));

			window->draw(stick1);
			
			ConvexShape stick2(3);
			Vector2f s2ln = Vector2f(pos.x + size.x, pos.y + size.y) - base;
			
			stick2.setPoint(0, base);
			stick2.setPoint(1, setVectorLenght(s2ln, map(getVectorLenght(s2ln), 0, getVectorLenght(s2l), tmp, step)) + base);
			stick2.setPoint(2, base + Vector2f(0, tmpStep * (parts - j + 2)));
			stick2.setFillColor(sticks * getColor(map(j, 0, parts-1, 0x88, 0xDD)));

			window->draw(stick2);
			
		}
	}
}

void Pine::draw() {
	
	Vector2f winSize = Vector2f(controller->w, controller->h);
	Vector2f tmpPos = position - Vector2f(size.x / 2, size.y);
	Vector2f tmpSize = Vector2f(size.x * winSize.x, size.y * winSize.y);
	tmpPos = Vector2f(tmpPos.x * winSize.x, tmpPos.y * winSize.y);
	
	Pine::drawWithTriangles(controller->window, tmpPos, tmpSize, logColor, sticksColor, 20, 3);
	//Pine::drawWithLines(controller->window, tmpPos, tmpSize, logColor, sticksColor, 50);
	
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
