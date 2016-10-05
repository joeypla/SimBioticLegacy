#include <d3dx9.h>
#include <string>

class S_FONT{
public:
	static LPD3DXFONT defaultFont;
	static LPD3DXFONT ammoFont;
	static LPD3DXFONT getDefaultFont(void);
	static LPD3DXFONT getAmmoFont(void);
	static void loadAllFonts(void);
	static void defaultDrawTextToScreen(std::wstring, int top, int bottom, int left, int right);
private:
	S_FONT(void);
	~S_FONT(void);
};