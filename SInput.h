#ifndef SYMBIOTE_INPUT_H
#define SYMBIOTE_INPUT_H

#include "dinput.h"
#include "SCore.h"
#include "SGUI.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

//DEFINITIONS FOR MOUSE BUTTONS
#define DIMOUSE_LEFTBUTTON 0
#define DIMOUSE_RIGHTBUTTON 1
#define	DIMOUSE_MIDDLEBUTTON 2
#define DIMOUSE_4BUTTON 3
#define DIMOUSE_5BUTTON 4
#define DIMOUSE_6BUTTON 5
#define DIMOUSE_7BUTTON 6
#define DIMOUSE_8BUTTON 7

//define vertex format for cursor
#define D3DFVF_CURSORVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)
namespace S_INPUT{
	//VERTEX STRUCT
	struct D3DCURSORVERTEX{
		float x;
		float y;
		float z;

		DWORD color;
		
		float u;
		float v;
	};

	bool setup(void);
	bool release(void);

	//MESSAGE HANDLING
	bool update(void);
	bool keyPressed(int);
	extern char key_buffer[256];
	extern LPDIRECTINPUT8 inputInterface;
	extern LPDIRECTINPUTDEVICE8 keyboardDevice;

	extern LPDIRECTINPUTDEVICE8 mouseDevice;
	extern DIMOUSESTATE mouseState;
	LONG getMouseDeltaX(void);
	LONG getMouseDeltaY(void);
	int getMouseX(void);
	int getMouseY(void);
	bool mousePressed(int);
}
#endif