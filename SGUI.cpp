#include "SGUI.h"
#include "SRender.h"
//define static members
LPDIRECT3D9 S_GUI::d3d;
LPDIRECT3DDEVICE9 S_GUI::dev;
HWND S_GUI::hWnd;
MSG S_GUI::msg;
HINSTANCE S_GUI::hInstance;

int S_GUI::nCmdShow;
short S_GUI::screenHeight;
short S_GUI::screenWidth;
short S_GUI::bufferHeight;
short S_GUI::bufferWidth;


bool S_GUI::setup(HINSTANCE p_hInstance, int p_nCmdShow){
	hInstance = p_hInstance;
	nCmdShow = p_nCmdShow;

	S_GUI::setup_32();
	S_GUI::setup_D3D();

	//create the rendersystem
	SRender::createSingleton();
	SRender::getSingleton()->setup(dev);
	return true;
}

bool S_GUI::update(void){
	if (!S_GUI::update_32()){
		return false;
	}

	if (!S_GUI::update_D3D()){
		return false;
	}

	return true;
}

HWND S_GUI::getHWND(void){
	return hWnd;
}