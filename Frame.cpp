#include "psGui.h"

using namespace psGui;

Frame::Frame(void)
{
	visible = true;
	frame = SRender::getSingleton()->createS2D();
	this->x = 0.0f;
	this->y = 0.0f;
}

Frame::Frame(float x, float y)
{
	visible = true;
	frame = SRender::getSingleton()->createS2D();

	this->x = x;
	this->y = y;
	frame->setPosition(x,y);
}

Frame::Frame(float x, float y, float width, float height)
{
	visible = true;
	frame = SRender::getSingleton()->createS2D();

	this->x = x;
	this->y = y;
	frame->setPosition(x,y);

	this->width = width;
	this->height = height;
	frame->setDimensions(width,height);
}

Frame::~Frame(void)
{
	 SRender::getSingleton()->releaseS2D(frame);
}

void Frame::setPosition(float x, float y)
{
	this->x = x;
	this->y = y;
	frame->setPosition(x,y);

	for (int i = 0; i < components.size(); i++)
	{
		components[i]->setAbsolutePosition(x + components[i]->getRelativeX(), y + components[i]->getRelativeY());
	}
}

void Frame::setSize(float width, float height)
{
	this->width = width;
	this->height = height;
	frame->setDimensions(width, height);
}

void Frame::setColor(DWORD color)
{
	frame->setColor(color);
}

void Frame::update(void)
{
	for (int i = 0; i < components.size(); i++)
	{
		components[i]->setAbsolutePosition(x + components[i]->getRelativeX(), y + components[i]->getRelativeY());
		components[i]->update();
	}
}

void Frame::sendSystemMessage(UINT message, UINT param)
{
	for (int i = 0; i < components.size(); i++)
	{
		if (components[i] == NULL) continue;
		components[i]->sendSystemMessage(message, param);
	}
}

void Frame::addComponent(Component* component)
{
	components.push_back(component);
}

void Frame::setVisible(bool visible)
{
	frame->setVisible(visible);
	this->visible = visible;
	for (int i = 0; i < components.size(); i++)
	{
		components[i]->setVisible(visible);
	}
	
}

bool Frame::getVisible(void)
{
	return visible;
}

#include "SInput.h"
bool Frame::getMouseWithinFrame(void)
{
	int mouseX = S_INPUT::getMouseX();
	int mouseY = S_INPUT::getMouseY();
	//normalise mouse coords
	SRender* sr = SRender::getSingleton();
	float screenWidth = sr->getWidth();
	float screenHeight = sr->getHeight();
	float x = (float)mouseX / screenWidth * 2.0f - 1.0f;
	float y = (-1.0f) * ((float)mouseY / screenHeight * 2.0f - 1.0f);
	if (x > this->x - this->width*0.5f &&
		x < this->x + this->width*0.5f &&
		y > this->y - this->height*0.5f &&
		y < this->y + this->height*0.5f)
	{
		return true;
	}
	return false;
}