#include "psGui.h"

using namespace psGui;
#include "SLOG.h"
TextField::TextField(void)
{
	field = SRender::getSingleton()->createS2D();
	text = SRender::getSingleton()->createText();
	text->setAlignment(DT_LEFT | DT_VCENTER);
	stream.clear();
	active = false;
}

TextField::~TextField(void)
{
	SRender::getSingleton()->releaseS2D(field);
	SRender::getSingleton()->releaseText(text);
}

void TextField::setRelativePosition(float rx, float ry)
{
	this->rx = rx;
	this->ry = ry;
}

void TextField::setAbsolutePosition(float ax, float ay)
{
	field->setPosition(ax, ay);
	text->setPosition(ax, ay);
}

void TextField::setText(const wchar_t* text)
{
	stream.clear();
	stream.append(text);
	this->text->setText(text);
}

void TextField::setSize(float x, float y)
{
	field->setDimensions(x,y);
	text->setDimensions(x,y);
}

void TextField::setFieldColor(DWORD color)
{
	field->setColor(color);
}

void TextField::setTextSize(int size)
{
	text->setSize(size);
}

#include "SInput.h"
void TextField::update(void)
{
	
}

#include "SLOG.h"
void TextField::sendSystemMessage(UINT message, UINT param){
	if (!active) return;
	if (message == WM_LBUTTONDOWN)
	{
		
	}

	if (message == WM_KEYDOWN)
	{
		if (S_INPUT::keyPressed(DIK_LSHIFT))
		{
			switch (param)
			{

			case 0x41:
				stream.append(L"A");
				break;
			case 0x42:
				stream.append(L"B");
				break;
			case 0x43:
				stream.append(L"C");
				break;
			case 0x44:
				stream.append(L"D");
				break;
			case 0x45:
				stream.append(L"E");
				break;
			case 0x46:
				stream.append(L"F");
				break;
			case 0x47:
				stream.append(L"G");
				break;
			case 0x48:
				stream.append(L"H");
				break;
			case 0x49:
				stream.append(L"I");
				break;
			case 0x4A:
				stream.append(L"J");
				break;
			case 0x4B:
				stream.append(L"K");
				break;
			case 0x4C:
				stream.append(L"L");
				break;
			case 0x4D:
				stream.append(L"M");
				break;
			case 0x4E:
				stream.append(L"N");
				break;
			case 0x4F:
				stream.append(L"O");
				break;
			case 0x50:
				stream.append(L"P");
				break;
			case 0x51:
				stream.append(L"Q");
				break;
			case 0x52:
				stream.append(L"R");
				break;
			case 0x53:
				stream.append(L"S");
				break;
			case 0x54:
				stream.append(L"T");
				break;
			case 0x55:
				stream.append(L"U");
				break;
			case 0x56:
				stream.append(L"V");
				break;
			case 0x57:
				stream.append(L"W");
				break;
			case 0x58:
				stream.append(L"X");
				break;
			case 0x59:
				stream.append(L"Y");
				break;
			case 0x5A:
				stream.append(L"Z");
				break;
			case VK_BACK:
				stream = stream.substr(0, stream.size()-1);
				break;
			case VK_DELETE:
				stream = stream.substr(0, stream.size()-1);
				break;
			default:
				break;
			}
		} else {
			switch (param)
			{
			case 0x30:
				stream.append(L"0");
				break;
			case 0x31:
				stream.append(L"1");
				break;
			case 0x32:
				stream.append(L"2");
				break;
			case 0x33:
				stream.append(L"3");
				break;
			case 0x34:
				stream.append(L"4");
				break;
			case 0x35:
				stream.append(L"5");
				break;
			case 0x36:
				stream.append(L"6");
				break;
			case 0x37:
				stream.append(L"7");
				break; 
			case 0x38:
				stream.append(L"8");
				break;
			case 0x39:
				stream.append(L"9");
				break;
			case 0x41:
				stream.append(L"a");
				break;
			case 0x42:
				stream.append(L"b");
				break;
			case 0x43:
				stream.append(L"c");
				break;
			case 0x44:
				stream.append(L"d");
				break;
			case 0x45:
				stream.append(L"e");
				break;
			case 0x46:
				stream.append(L"f");
				break;
			case 0x47:
				stream.append(L"g");
				break;
			case 0x48:
				stream.append(L"h");
				break;
			case 0x49:
				stream.append(L"i");
				break;
			case 0x4A:
				stream.append(L"j");
				break;
			case 0x4B:
				stream.append(L"k");
				break;
			case 0x4C:
				stream.append(L"l");
				break;
			case 0x4D:
				stream.append(L"m");
				break;
			case 0x4E:
				stream.append(L"n");
				break;
			case 0x4F:
				stream.append(L"o");
				break;
			case 0x50:
				stream.append(L"p");
				break;
			case 0x51:
				stream.append(L"q");
				break;
			case 0x52:
				stream.append(L"r");
				break;
			case 0x53:
				stream.append(L"s");
				break;
			case 0x54:
				stream.append(L"t");
				break;
			case 0x55:
				stream.append(L"u");
				break;
			case 0x56:
				stream.append(L"v");
				break;
			case 0x57:
				stream.append(L"w");
				break;
			case 0x58:
				stream.append(L"x");
				break;
			case 0x59:
				stream.append(L"y");
				break;
			case 0x5A:
				stream.append(L"z");
				break;
			case VK_BACK:
				stream = stream.substr(0, stream.size()-1);
				break;
			case VK_DELETE:
				stream = stream.substr(0, stream.size()-1);
				break;
			case VK_OEM_PERIOD:
				stream.append(L".");
				break;
			case VK_OEM_2:
				stream.append(L"/");
				break;
			default:
				break;
			}
		}
	}
	text->setText(stream.c_str());
}

void TextField::setVisible(bool visible)
{
	field->setVisible(visible);
	text->setVisible(visible);
}

void TextField::setTextColor(DWORD color)
{
	text->setColor(color);
}

std::wstring TextField::getText(void)
{
	return stream;
}

bool TextField::getMouseWithin(void)
{
	int mouseX = S_INPUT::getMouseX();
	int mouseY = S_INPUT::getMouseY();
	//normalise mouse coords
	SRender* sr = SRender::getSingleton();
	float screenWidth = sr->getWidth();
	float screenHeight = sr->getHeight();
	float x = (float)mouseX / screenWidth * 2.0f - 1.0f;
	float y = (-1.0f) * ((float)mouseY / screenHeight * 2.0f - 1.0f);
	if (x < field->getPositionX() + field->getDimensionsX()*0.5f &&
		x > field->getPositionX() - field->getDimensionsX()*0.5f &&
		y < field->getPositionY() + field->getDimensionsY()*0.5f &&
		y > field->getPositionY() - field->getDimensionsY()*0.5f)
	{
		return true;

	}

	return false;
}