#include "psGui.h"

using namespace psGui;

SimpleButton::SimpleButton(void)
{
	inner = SRender::getSingleton()->createS2D();
	outer = SRender::getSingleton()->createS2D();
	text = SRender::getSingleton()->createText();
	text->setAlignment(DT_CENTER | DT_VCENTER);
	textHorizontalProportion = 1.0f;
	pFunc = 0;
}

SimpleButton::~SimpleButton(void)
{
	SRender::getSingleton()->getSingleton()->releaseS2D(inner);
	SRender::getSingleton()->getSingleton()->releaseS2D(outer);
	SRender::getSingleton()->getSingleton()->releaseText(text);
}

void SimpleButton::setRelativePosition(float rx, float ry)
{
	this->rx = rx;
	this->ry = ry;
}

void SimpleButton::setAbsolutePosition(float ax, float ay)
{
	inner->setPosition(ax, ay);
	outer->setPosition(ax, ay);
	text->setPosition(ax, ay);
}

void SimpleButton::setCaption(wchar_t* caption)
{
	text->setText(caption);
}

void SimpleButton::setSize(float x, float y)
{
	inner->setDimensions(x, y);
	outer->setDimensions(x, y);
	text->setDimensions(x*textHorizontalProportion,y);
}

void SimpleButton::setColor(DWORD color)
{
	inner->setColor(color);
	outer->setColor(color);
}

void SimpleButton::setCaptionColor(DWORD color)
{
	text->setColor(color);
}

void SimpleButton::setCaptionSize(int size)
{
	text->setSize(size);
}

#include "SInput.h"
void SimpleButton::update(void)
{
	int mouseX = S_INPUT::getMouseX();
	int mouseY = S_INPUT::getMouseY();
	//normalise mouse coords
	SRender* sr = SRender::getSingleton();
	float screenWidth = sr->getWidth();
	float screenHeight = sr->getHeight();
	float x = (float)mouseX / screenWidth * 2.0f - 1.0f;
	float y = (-1.0f) * ((float)mouseY / screenHeight * 2.0f - 1.0f);
	if (x < outer->getPositionX() + outer->getDimensionsX()*0.5f &&
		x > outer->getPositionX() - outer->getDimensionsX()*0.5f &&
		y < outer->getPositionY() + outer->getDimensionsY()*0.5f &&
		y > outer->getPositionY() - outer->getDimensionsY()*0.5f)
	{
		//inner->setDimensions(outer->getDimensionsX() - 0.01f, outer->getDimensionsY() - 0.01f);
		inner->setVisible(true);

	} else {
		//inner->setDimensions(outer->getDimensionsX(), outer->getDimensionsY());
		inner->setVisible(false);
	}
}


void SimpleButton::sendSystemMessage(UINT message, UINT param){
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
		if (x > outer->getPositionX() - outer->getDimensionsX()*0.5f &&
			x < outer->getPositionX() + outer->getDimensionsX()*0.5f &&
			y > outer->getPositionY() - outer->getDimensionsY()*0.5f &&
			y < outer->getPositionY() + outer->getDimensionsY()*0.5f)
		{
			if (pFunc == 0) return;
			pFunc();
		}
	}
}

void SimpleButton::setFunctionPointer(void (*pF)(void))
{
	pFunc = pF;
}

void SimpleButton::setVisible(bool visible)
{
	inner->setVisible(visible);
	outer->setVisible(visible);
	text->setVisible(visible);
}

void SimpleButton::setInnerImage(SImage* image)
{
	inner->setImage(image);
}

void SimpleButton::setInnerColor(DWORD color)
{
	inner->setColor(color);
}

void SimpleButton::setOuterColor(DWORD color)
{
	outer->setColor(color);
}

void SimpleButton::setTextHorizontalProportion(float proportion)
{
	textHorizontalProportion = proportion;
}

void SimpleButton::setTextAlignment(DWORD alignment)
{
	text->setAlignment(alignment);
}

bool SimpleButton::getMouseWithin(void)
{
	int mouseX = S_INPUT::getMouseX();
	int mouseY = S_INPUT::getMouseY();
	//normalise mouse coords
	SRender* sr = SRender::getSingleton();
	float screenWidth = sr->getWidth();
	float screenHeight = sr->getHeight();
	float x = (float)mouseX / screenWidth * 2.0f - 1.0f;
	float y = (-1.0f) * ((float)mouseY / screenHeight * 2.0f - 1.0f);
	if (x < outer->getPositionX() + outer->getDimensionsX()*0.5f &&
		x > outer->getPositionX() - outer->getDimensionsX()*0.5f &&
		y < outer->getPositionY() + outer->getDimensionsY()*0.5f &&
		y > outer->getPositionY() - outer->getDimensionsY()*0.5f)
	{
		//inner->setDimensions(outer->getDimensionsX() - 0.01f, outer->getDimensionsY() - 0.01f);
		return true;

	} else {
		//inner->setDimensions(outer->getDimensionsX(), outer->getDimensionsY());
		return false;
	}
	return false;
}