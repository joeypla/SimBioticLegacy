#ifndef STEXT_H
#define STEXT_H

#include <d3dx9.h>
#include <string>
class SText
{
public:
	SText(void);
	~SText(void);
	static void setupModule(LPDIRECT3DDEVICE9 dev);
	void render(LPDIRECT3DDEVICE9 dev);
	void setText(const wchar_t*);
	void setPosition(float newx, float newy);
	void setX(float newx);
	void setY(float newy);

	void setDimensions(float newWidth, float newHeight);
	void setColor(DWORD col);
	void setColor(char a, char r, char g, char b);
	void setSize(int size);//size is technically the index in the font array
	void setVisible(bool visible);
	void setAlignment(DWORD flags);
private:
	static LPD3DXFONT fontDefault[30];
	std::wstring text;
	LPD3DXFONT font;

	float x;
	float y;
	float width;
	float height;
	DWORD color;
	DWORD alignmentFlags;
	bool visible;
};

#endif