//
// Ilya Yavdoschuk
// FGroup (c) 2019
// 

#include "FoxFires.hpp"
#include <random>

using namespace FG;

FoxFires::FoxFires(Controller * controller) : RenderLayer(controller) {
	this->controller = controller;
	
	controller->fires++;
	flags = defaultFlags;

	for(int i = 0; i < dataLength; i++)
		data[i] = 128 + rand() % 128;
}
		
void FoxFires::draw() {
	
	double timeMapped = map(controller->timeInternal, 0, 86400, -1, 1);
	
	if (timeMapped >= -0.70 && timeMapped <= 0.70)
		return;
		
	for(int i = 0; i < controller->w; i++)
	{
		currentData = data[i];
			
		timeMapped = map(controller->timeInternal, 0, 86400, -1, 1);
			
		c = (currentData & 0xFF) / 255.0;
		mapI = map(i, 0, controller->w, 0, 180);
		maskDouble = 1;
		yMod = (flags & Flags::UseYSine) ? map(sin((ySineOffset + mapI * 2) * M_PI / 180.0), -1, 1, 0, 1) : 0;
		
		if((flags & Flags::UseYSineWane))
			maskDouble *= map(yMod, 0, 1, 1, 0.8);

		if((flags & Flags::UseWaneSine))
		{
			double waneSine = sin((waneSineOffset + mapI * 4) * M_PI / 180.0);
			maskDouble *= map(waneSine, -1, 1, (flags & Flags::UseYSineWane) && (flags & Flags::UseYSine) ? 0.9: 0.8, 1);
		}
		
		if (timeMapped < 0 && timeMapped > -0.80 && timeMapped < -0.70)
			timeMapped = map(timeMapped, -0.80, -0.70, 1, 0);
		else if (timeMapped > 0 && timeMapped > 0.70 && timeMapped < 0.80) 
			timeMapped = map(timeMapped, 0.70, 0.80, 0, 1);
		else 
			timeMapped = 1;
			
		maskDouble *= timeMapped;

		oneI = (flags & Flags::Monochrome) ? 0.5 : map(i, 0, controller->w, 0, 1);
		x1 = colorOffset;
		colorBaseIndex = (int) x1;
		
		x1 -= colorBaseIndex;
		x1 += oneI;

		if(x1 >= 1)
		{
			colorBaseIndex++;
			x1--;
		}

		x2 = 1 - x1;
		
		color = colors[colorBaseIndex % colorsLength];
		mixedColor = colors[(colorBaseIndex + 1) % colorsLength];

		mask1 = getMask(x2); 
		mask2 = getMask(x1);
		
		mask1.a = 0xFF;
		mask2.a = 0xFF;

		color = color * mask1 + mixedColor * mask2;
		
		mask = Color(0xFF, 0xFF, 0xFF, 0xFF * maskDouble);
		
		postcalc(i);
		
		color *= mask;

		if (controller->w-i < controller->w)
		{
			Vertex line[] =
			{
				Vertex(Vector2f(controller->w-i, yOffset + totalSize * ((yMod * controller->h * (1 - size)) + map(controller->h-(controller->h * c), 0, controller->h, 0, controller->h * size))), Color::Transparent),
				Vertex(Vector2f(controller->w-i, yOffset + totalSize * ((yMod * controller->h * (1 - size)) + map(controller->h-bottomMargin-coreHeight, 0, controller->h, 0, controller->h * size))), color),
				Vertex(Vector2f(controller->w-i, yOffset + totalSize * ((yMod * controller->h * (1 - size)) + map(controller->h-bottomMargin, 0, controller->h, 0, controller->h * size))), color),
				Vertex(Vector2f(controller->w-i, yOffset + totalSize * ((yMod * controller->h * (1 - size)) + map(controller->h-(bottomMargin-(bottomMargin * c)), 0, controller->h, 0, controller->h * size))), Color::Transparent)
			};
		
			color *= Color(0xFF * maskDouble, 0xFF * maskDouble, 0xFF * maskDouble, 0xFF) * getMask(1.0 / controller->fires);			
			controller->skyAmbient[controller->w-i] += color;
			
			controller->window->draw(line, 4, LineStrip);
		}
	}
}

void FoxFires::update() {	
	if((flags & Flags::UpdateData)){
		int firstData = data[0];
		for(int i = 0; i < dataLength; i++)
			if (i > 0)
				data[i - 1] = data[i];
		
		if((flags & Flags::CycleData))
			data[dataLength - 1] = firstData;
		else
			data[dataLength - 1] = 128 + rand() % 128;
	}
	
	if((flags & Flags::UpdateYSine) && (flags & Flags::UseYSine))
		ySineOffset += ySineDelta;
	
	if((flags & Flags::UpdateColor) && !(flags & Flags::Monochrome))
		colorOffset += colorDelta;
	
	if((flags & Flags::UpdateWaneSine) && (flags & Flags::UseWaneSine))
		waneSineOffset += waneSineDelta;
	
	if(ySineOffset > 360)
		ySineOffset - 360;
	
	if(waneSineOffset > 360)
		waneSineOffset - 360;
	
	if(colorOffset > colorsLength + 1)
		colorOffset -= colorsLength;
}
