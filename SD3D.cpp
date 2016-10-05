#include "SGUI.h"
#include <iostream>
char S_GUI::backBufferColorR, S_GUI::backBufferColorG, S_GUI::backBufferColorB;
//SGUI is one module, but in terms of implementations, it is seperated into
//3 files: SGUI, SWin32 and SD3D. SGUI is in charge of acting as a base for the latter two.
//SWin32 deals with win32 interfacing and SD3D deals with device interfacing.



#include "SFont.h"
bool S_GUI::update_D3D(void){
	
	//Render whatever the interface object is supposed to render
	dev->BeginScene();
	{
		
		S_Core::p_InterfaceObject->renderToDevice(dev);
		
		
	}
	dev->EndScene();
	
	//Present the rendered frame
	dev->Present(NULL, NULL, NULL, NULL);
	//For now always return true, remember to implement some
	//sort of error catch...
	return true;
	
}


bool S_GUI::setup_D3D(){
	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.hDeviceWindow = hWnd;
	bool windowed = !S_Configs::b_fullscreen;
	d3dpp.Windowed = windowed;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferHeight = S_Configs::s_bufferY;
	d3dpp.BackBufferWidth = S_Configs::s_bufferX;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	
	d3d->CreateDevice(D3DADAPTER_DEFAULT, 
		D3DDEVTYPE_HAL, 
		hWnd, 
		D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, 
		&d3dpp, 
		&dev);
	
	setBackBufferColor(255, 255, 255); 
	return true;
}

LPDIRECT3DDEVICE9 S_GUI::getDevice(void){
	LPDIRECT3DDEVICE9 devPointer = dev;
	return devPointer;
}

void S_GUI::setBackBufferColor(char r, char g, char b){
	backBufferColorR = r;
	backBufferColorG = g;
	backBufferColorB = b;
}

void S_GUI::forcePresent(void)
{
	dev->Present(NULL, NULL, NULL, NULL);
}