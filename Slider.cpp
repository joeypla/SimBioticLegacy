#include "psGui.h"

using namespace psGui;

Slider::Slider(void)
{
	min = 0;
	max = 100;
	value = 25;
	line = SRender::getSingleton()->createS2D();
	slider = SRender::getSingleton()->createS2D();
	line->setColor(0x7f000000);
	line->setDimensions(0.2f, 0.005f);
	slider->setDimensions(0.005f, 0.05f);
	lineThickness = 0.005f;
	sliderThickness = 0.005f;
}

Slider::Slider(int min, int max)
{
	this->min = min;
	this->max = max;
}

Slider::~Slider(void)
{
	SRender::getSingleton()->releaseS2D(line);
	SRender::getSingleton()->releaseS2D(slider);
}

void Slider::setMin(int min)
{
	this->min = min;
}

void Slider::setMax(int max)
{
	this->max = max;
}

void Slider::setRange(int min, int max)
{
	this->min = min;
	this->max = max;
}

void Slider::setRelativePosition(float rx, float ry)
{
	this->rx = rx;
	this->ry = ry;
}

#include "SLOG.h"
#include <sstream>
void Slider::setAbsolutePosition(float ax, float ay)
{
	line->setPosition(ax, ay);
	//find position for slider in relation to ax based on value, min and max.
	int average = (min + max) / 2;//50
	int halfRange = (max - min) / 2;//50
	int deviation = value - average;//-25
	float normalised = (float)deviation / (float)halfRange;//-25/50 = -0.5f;
	slider->setPosition(ax + (normalised*line->getDimensionsX()*0.5f), ay);
	
	//slider->setPosition(ax - 0.1f, ay);
}

void Slider::setSize(float width, float height)
{
	line->setDimensions(width, lineThickness);
	slider->setDimensions(sliderThickness, height);
}

void Slider::setLineThickness(float thickness)
{
	lineThickness = thickness;
}

void Slider::setSliderThickness(float thickness)
{
	sliderThickness = thickness;
}

#include "SInput.h"

#include "SLOG.h"
void Slider::update(void)
{
	if (S_INPUT::mousePressed(0))
	{
		//get mouse coordinates
		int mouseX = S_INPUT::getMouseX();
		int mouseY = S_INPUT::getMouseY();
		//normalise mouse coords
		SRender* sr = SRender::getSingleton();
		float screenWidth = sr->getWidth();
		float screenHeight = sr->getHeight();
		float x = (float)mouseX / screenWidth * 2.0f - 1.0f;
		float y = (-1.0f) * ((float)mouseY / screenHeight * 2.0f - 1.0f);
		if (x > line->getPositionX() - line->getDimensionsX()*0.5f &&
			x < line->getPositionX() + line->getDimensionsX()*0.5f &&
			y > slider->getPositionY() - slider->getDimensionsY()*0.5f &&
			y < slider->getPositionY() + slider->getDimensionsY()*0.5f)
		{
			slider->setPosition(x, slider->getPositionY());
			
			float center = line->getPositionX();
			float pos = slider->getPositionX();
			float normal = (pos - center) / (line->getDimensionsX()*0.5f);
			value = ((float)(max + min)/2.0f) + (((float)(max - min)/2.0f)*normal);
		} else {
			

		}
	}
	

}

void Slider::sendSystemMessage(UINT message, UINT param){
}

void Slider::setVisible(bool visible)
{
	line->setVisible(visible);
	slider->setVisible(visible);
}

void Slider::setLineColor(DWORD color)
{
	line->setColor(color);
}

void Slider::setSliderColor(DWORD color)
{
	slider->setColor(color);
}

void Slider::setValue(int value)
{
	if (value < min) value = min;
	if (value > max) value = max;
	this->value = value;
}

int Slider::getValue(void)
{
	return value;
}