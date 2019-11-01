//
// Ilya Yavdoschuk
// FGroup (c) 2019
//

#include "FoxFires.hpp"

using namespace FG;

Sky::Sky(Controller * controller) : RenderLayer(controller) {
	this->controller = controller;
}

void Sky::draw() {}

void Sky::update() {
	double timeMapped = map(controller->timeInternal, 0, 86400, -1, 1);

	if (timeMapped >= -0.66 && timeMapped <= 0.66)
		timeMapped = 0;
	else if (timeMapped < 0 && timeMapped > -0.75 && timeMapped < -0.66)
		timeMapped = map(timeMapped, -0.75, -0.66, 1, 0);
	else if (timeMapped > 0 && timeMapped > 0.66 && timeMapped < 0.75)
		timeMapped = map(timeMapped, 0.66, 0.75, 0, 1);
	else
		timeMapped = 1;

	controller->backColor = skies[0] * getMask(timeMapped) + skies[1] * getMask(1 - timeMapped);
	controller->ambientColor = (ambients[0] * getMask(timeMapped) + ambients[1] * getMask(1 - timeMapped));
}
