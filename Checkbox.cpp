#include "psGui.h"

using namespace psGui;

Checkbox::Checkbox(void)
{
	square = SRender::getSingleton()->createS2D();
	check = SRender::getSingleton()->createText();
	check->setText(L"X\0");
	check->setAlignment(DT_CENTER | DT_VCENTER);
	
}

Checkbox::~Checkbox(void)
{
	SRender::getSingleton()->releaseS2D(square);
	SRender::getSingleton()->releaseText(check);
}

void Checkbox::setSize(float size)
{
	float aspectRatio = SRender::getSingleton()->getWidth() / SRender::getSingleton()->getHeight();
	square->setDimensions(size/aspectRatio, size);
	check->setDimensions(size/aspectRatio, size);
}

void Checkbox::setCheckSize(int size)
{
	check->setSize(size);
}
void Checkbox::setState(bool state)
{
	this->state = state;
	if (state) check->setVisible(true);
	if (!state) check->setVisible(false);
}

bool Checkbox::getState(void)
{
	return state;
}
void Checkbox::setRelativePosition(float rx, float ry)
{
	this->rx = rx;
	this->ry = ry;
}

void Checkbox::setAbsolutePosition(float ax, float ay)
{
	square->setPosition(ax, ay);
	check->setPosition(ax, ay);
}
float Checkbox::getRelativeX(void) {return rx;}
float Checkbox::getRelativeY(void) {return ry;}

void Checkbox::setColor(DWORD color)
{
	square->setColor(color);
}

void Checkbox::setCheckColor(DWORD color)
{
	check->setColor(color);
}

void Checkbox::update(void)
{
}

#include "SInput.h"
void Checkbox::sendSystemMessage(UINT message, UINT param){
	if (message == WM_LBUTTONDOWN)
	{
		int mouseX = S_INPUT::getMouseX();
		int mouseY = S_INPUT::getMouseY();
		//normalise mouse coords
		SRender* sr = SRender::getSingleton();
		float screenWidth = sr->getWidth();
		float screenHeight = sr->getHeight();
		float x = (float)mouseX / screenWidth * 2.0f - 1.0f;
		float y = (-1.0f) * ((float)mouseY / screenHeight * 2.0f - 1.0f);
		if (x > square->getPositionX() - square->getDimensionsX()*0.5f &&
			x < square->getPositionX() + square->getDimensionsX()*0.5f &&
			y > square->getPositionY() - square->getDimensionsY()*0.5f &&
			y < square->getPositionY() + square->getDimensionsY()*0.5f)
		{
			state = !state;
			if (state)
			{
				if (square->getVisible())
				{
					check->setVisible(true);
				} else check->setVisible(false);
			}
			if (!state) check->setVisible(false);

			
		}

		
	}
}

void Checkbox::setVisible(bool visible)
{
	this->visible = visible;

	square->setVisible(visible);
	check->setVisible(visible);
	//reset the state
	if (visible)
	setState(state);
}