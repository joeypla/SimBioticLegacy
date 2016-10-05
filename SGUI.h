#ifndef SYMBIOTE_SGUI_H
#define SYMBIOTE_SGUI_H

#include "SConfigs.h"
#include "SCore.h"
//Include DX9 headers + Link with DX9 Libraries
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#include <d3d9.h>
#include <d3dx9.h>

//Include Windows Header

#define _WINSOCKAPI_
//#include <Windows.h>
//DX9 Defintions
#define CUSTOMFVF1 (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)
#define CUSTOMFVF_BASIC (D3DFVF_XYZ | D3DFVF_DIFFUSE)

	struct CUSTOMVERTEX1 {float x;float y;float z;D3DVECTOR normal;float u;float v;};
	struct CUSTOMVERTEX_BASIC {float x;float y;float z;DWORD color;};
	class S_GUI{
	private:
		S_GUI(void);
		~S_GUI(void);
		//Direct3D Members
		
		static LPDIRECT3D9 d3d;
		static LPDIRECT3DDEVICE9 dev;

		static bool setup_D3D(void);
		static bool update_D3D(void);
		
		LPD3DXFONT debug_font;//Font used in lowest level

		static char backBufferColorR, backBufferColorG, backBufferColorB;

		//Win32 Members
		static HWND hWnd;
		static MSG msg;
		static HINSTANCE hInstance;

		static int nCmdShow;
		static bool setup_32(void);
		static bool update_32(void);



		//GENERAL GUI
		static short screenHeight, screenWidth;
		static short bufferHeight, bufferWidth;

	public:
		static bool setup(HINSTANCE phInstance, int pnCmdShow);
		static bool update(void);
		static HWND getHWND(void);
		static LPDIRECT3DDEVICE9 getDevice(void);
		static bool loadMap_VertexIndex_Buffers(const CUSTOMVERTEX1* map_vertices, const short* map_indices, int map_blockCount);
		static void setBackBufferColor(char,char,char);
		static void forcePresent(void);//not sure if dangerous
	};

#endif