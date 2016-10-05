#include "psGui.h"

using namespace psGui;

Simple::Simple(void)
{
	box = SRender::getSingleton()->createS2D();
}

Simple::~Simple(void)
{
	SRender::getSingleton()->releaseS2D(box);
}

void Simple::setSize(float width, float height)
{
	box->setDimensions(width, height);
}

void Simple::setColor(DWORD color)
{
	box->setColor(color);
}

void Simple::setRelativePosition(float rx, float ry)
{
	this->rx = rx;
	this->ry = ry;
}

void Simple::setAbsolutePosition(float ax, float ay)
{
	box->setPosition(ax, ay);
}

void Simple::sendSystemMessage(UINT message, UINT param)
{
}

void Simple::setVisible(bool visible)
{
	box->setVisible(visible);
}

void Simple::update(void)
{
}

void Simple::x_SetTexture(LPDIRECT3DTEXTURE9 tex)
{
	box->x_SetTexture(tex);
}

#include "SInput.h"
bool Simple::getMouseWithin(void)
{

	int mouseX = S_INPUT::getMouseX();
	int mouseY = S_INPUT::getMouseY();

	//normalise mouse coords
	SRender* sr = SRender::getSingleton();
	float screenWidth = sr->getWidth();
	float screenHeight = sr->getHeight();
	float x = (float)mouseX / screenWidth * 2.0f - 1.0f;
	float y = (-1.0f) * ((float)mouseY / screenHeight * 2.0f - 1.0f);

	float x2 = box->getPositionX();
	float y2 = box->getPositionY();
	float width = box->getDimensionsX();
	float height = box->getDimensionsY();
	if (x > x2 - width*0.5f &&
		x < x2 + width*0.5f &&
		y > y2 - height*0.5f &&
		y < y2 + height*0.5f)
	{
		return true;
	}
	return false;
}

void psGui::Simple::setTextureFromPath(std::wstring path)
{
	box->setTextureFromPath(path.c_str());
}

void psGui::Simple::setRotation(float rotation)
{
	box->setRotation(rotation);
}

void psGui::Simple::setImage(SImage* image)
{
	box->setImage(image);
}