#include "SFont.h"
#include "SGUI.h"

LPD3DXFONT S_FONT::defaultFont;
LPD3DXFONT S_FONT::ammoFont;
void S_FONT::loadAllFonts(void){

	//LOAD DEFAULT FONT
	{
		defaultFont = NULL;
		D3DXFONT_DESC fontDescription = {
			18,
			0,
			400,
			0,
			false,
			DEFAULT_CHARSET,
			OUT_TT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_PITCH,
			L"RussellSquare"};
			fontDescription.Quality = ANTIALIASED_QUALITY;
			D3DXCreateFontIndirect(S_GUI::getDevice(), &fontDescription, &defaultFont);
	}
	{
		ammoFont = NULL;
		D3DXFONT_DESC fontDescription = {
			90,
			0,
			400,
			0,
			false,
			DEFAULT_CHARSET,
			OUT_TT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_PITCH,
			L"RussellSquare"};
			fontDescription.Quality = ANTIALIASED_QUALITY;
	D3DXCreateFontIndirect(S_GUI::getDevice(), &fontDescription, &ammoFont);
	}
}

LPD3DXFONT S_FONT::getDefaultFont(void){
	return defaultFont;
}

LPD3DXFONT S_FONT::getAmmoFont(void){
	return ammoFont;
}
void S_FONT::defaultDrawTextToScreen(std::wstring str, int top, int bottom, int left, int right){
	RECT rect;
	rect.bottom = bottom;
	rect.top = top;
	rect.left = left;
	rect.right = right;
	defaultFont->DrawText(NULL, str.c_str(), -1, &rect, DT_CENTER | DT_VCENTER, 0xff000000);
}
S_FONT::S_FONT(void){}
S_FONT::~S_FONT(void){}