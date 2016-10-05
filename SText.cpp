#include "SRender.h"

SText::SText(void){
	setPosition(0.0f, 0.0f);
	setDimensions(1.0f, 1.0f);
	setColor(0xffffffff);
	font = fontDefault[5];
	visible = true;
}

SText::~SText(void){
}
LPD3DXFONT SText::fontDefault[30];
#include <sstream>
#include "SConfigs.h"
void SText::setupModule(LPDIRECT3DDEVICE9 dev){
	D3DXFONT_DESC fontDescription = {
			0,
			0,
			400,
			0,
			false,
			DEFAULT_CHARSET,
			OUT_TT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_PITCH,
			L"Myriad Pro"};
		fontDescription.Quality = ANTIALIASED_QUALITY;
		for (int i = 0; i < 30; i++){
			
			//now we want to perform a division so that no matter the resolution, the text stays around the same size
			fontDescription.Height = (int)((i + 1)*5*((float)S_Configs::s_bufferY/1600.0f));
			D3DXCreateFontIndirect(dev, &fontDescription, &fontDefault[i]);
			
		}
}

#include "SLOG.h"
void SText::render(LPDIRECT3DDEVICE9 dev){
	if (!visible) return;
	float screenWidth = SRender::getSingleton()->getWidth();
	float screenHeight = SRender::getSingleton()->getHeight();
	float left = (x - (width*0.5f) + 1.0f)*(screenWidth/2.0f);
	float right = (x + (width*0.5f) + 1.0f)*(screenWidth/2.0f);
	float top = ((y*-1.0f) - (height*0.5f) + 1.0f)*(screenHeight/2.0f);
	float bottom = ((y*-1.0f) + (height*0.5f) + 1.0f)*(screenHeight/2.0f);
	RECT position;
	position.left = (int)left;
	position.right = (int)right;
	position.bottom = (int)bottom;
	position.top = (int)top;
	
	//S_LOG::write(text.c_str());
	
	//setup alignment flags
	
	font->DrawText(NULL, text.c_str(), -1, &position, alignmentFlags, color);

}

void SText::setText(const wchar_t* txt){ 
	//if (text != 0) delete text;
	text = txt;
}
void SText::setPosition(float newx, float newy){
	x = newx;
	y = newy;
}
void SText::setX(float newx)
{
	x = newx;
}

void SText::setY(float newy)
{
	y = newy;
}
void SText::setDimensions(float newWidth, float newHeight){
	width = newWidth;
	height = newHeight;
}

void SText::setColor(DWORD col){
	color = col;
}

void SText::setColor(char a, char r, char g, char b){
	color = D3DCOLOR_ARGB(a, r, g, b);
}

void SText::setSize(int size)
{
	if (size > 29) return;
	if (size < 0) return;
	font = fontDefault[size]; 
}

void SText::setVisible(bool visible){this->visible = visible;}

void SText::setAlignment(DWORD flags){alignmentFlags = flags;}