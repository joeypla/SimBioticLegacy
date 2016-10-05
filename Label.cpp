#include "psGui.h"

using namespace psGui;

Label::Label(void)
{
	label = SRender::getSingleton()->createText();
}

Label::Label(const wchar_t* text)
{
	label = SRender::getSingleton()->createText();
	label->setText(text);
}

Label::~Label(void)
{
	SRender::getSingleton()->releaseText(label);
}

void Label::setText(const wchar_t* text)
{
	label->setText(text);
}

void Label::setTextSize(int size)
{
	label->setSize(size);
}

void Label::setSize(float width, float height)
{
	label->setDimensions(width, height);
}

void Label::setTextColor(DWORD color)
{
	label->setColor(color);
}

void Label::setRelativePosition(float rx, float ry)
{
	this->rx = rx;
	this->ry = ry;
	label->setPosition(this->rx, this->ry);
}

void Label::setAbsolutePosition(float ax, float ay)
{
	label->setPosition(ax, ay);
}


void Label::setTextAlignment(DWORD flags)
{
	label->setAlignment(flags);
}
void Label::update(void)
{
}

void Label::sendSystemMessage(UINT message, UINT param){
}

void Label::setVisible(bool visible)
{
	label->setVisible(visible);
}