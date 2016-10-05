#include "psGui.h"

using namespace psGui;


Component::Component(void){
	active = false;
}
Component::~Component(void)
{
}

void Component::setRelativePosition(float rx, float ry)
{
	this->rx = rx;
	this->ry = ry;

}
void Component::setAbsolutePosition(float ax, float ay){}
float Component::getRelativeX(void)
{
	return rx;
}

float Component::getRelativeY(void)
{
	return ry;
}

void Component::update(void)
{
}

void Component::sendSystemMessage(UINT message, UINT param)
{
}

void Component::setVisible(bool visible)
{
}

void Component::setActive(bool active){
	this->active = active;
}

bool Component::getActive(void)
{
	return active;
}