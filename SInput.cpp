#include "SInput.h"

//definitions
namespace S_INPUT{
	//For interface
	static LPDIRECTINPUT8 inputInterface;
	//For Keyboard
	static LPDIRECTINPUTDEVICE8 keyboardDevice;
	static char key_buffer[256];
	//For mouse
	static LPDIRECTINPUTDEVICE8 mouseDevice;
	static DIMOUSESTATE mouseState;
}
bool S_INPUT::setup(void){
	//Initialize DirectInput Interface
	DirectInput8Create(S_Core::getHINSTANCE(),
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&inputInterface,
		NULL);

	//Initialize Keyboard
	inputInterface->CreateDevice(GUID_SysKeyboard,
		&keyboardDevice,
		NULL);

	keyboardDevice->SetDataFormat(&c_dfDIKeyboard);
	keyboardDevice->SetCooperativeLevel(S_GUI::getHWND(), (DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)); 
	keyboardDevice->Acquire();

	//Initialize Mouse
	inputInterface->CreateDevice(GUID_SysMouse,&mouseDevice, NULL);
	mouseDevice->SetDataFormat(&c_dfDIMouse);
	mouseDevice->SetCooperativeLevel(S_GUI::getHWND(), (DISCL_BACKGROUND | DISCL_NONEXCLUSIVE));
	mouseDevice->Acquire();

	return true;
}
bool S_INPUT::release(void){
	keyboardDevice->Unacquire();
	keyboardDevice->Release();

	mouseDevice->Unacquire();
	mouseDevice->Release();

	inputInterface->Release();
	inputInterface = NULL;

	return true;
}

bool S_INPUT::update(void){
	keyboardDevice->Acquire();
	keyboardDevice->GetDeviceState(sizeof(key_buffer), (LPVOID)&key_buffer);
	
	mouseDevice->Acquire();
	mouseDevice->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID) &mouseState);
	
	//if (keyPressed(DIK_DOWN)) return false;
	//if (mousePressed(0)) return false;
	return true;
}

bool S_INPUT::keyPressed(int key){
	if (key_buffer[key] & 0x80)
		return true;
	return false;
}


bool S_INPUT::mousePressed(int button){
	if (mouseState.rgbButtons[button] & 0x80)
		return true;
	return false;
}

LONG S_INPUT::getMouseDeltaX(void){
	return mouseState.lX;
}
LONG S_INPUT::getMouseDeltaY(void){
	return mouseState.lY;
}

int S_INPUT::getMouseX(void){
	POINT p;
	GetCursorPos(&p);
	return p.x;
}

int S_INPUT::getMouseY(void){
	POINT p;
	GetCursorPos(&p);
	return p.y;
}