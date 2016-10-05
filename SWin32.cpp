#include "SGUI.h"
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);



bool S_GUI::setup_32(void){
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hInstance = hInstance;
	wc.lpszClassName = L"PS_RENDER_WINDOW";
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpfnWndProc = WindowProc;
	RegisterClassEx(&wc);
	hWnd = CreateWindowEx(NULL, 
		L"PS_RENDER_WINDOW",  
		L"Project Symbiote 1.0a", 
		WS_EX_TOPMOST,
		0,0,
		S_Configs::s_windowX, S_Configs::s_windowY,
		NULL,
		NULL,
		hInstance,
		NULL);

	ShowWindow(hWnd, nCmdShow);
	SetFocus(hWnd);
	return true;
}
bool S_GUI::update_32(void){
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (msg.message == WM_QUIT || msg.message == WM_CLOSE){
		return false;
	}

	return true;
}

#include "SAUDIO.h"
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	
	switch (message){
	case WM_LBUTTONDOWN:
		{
			S_Core::p_InterfaceObject->receiveSystemMessage(message, wParam);
			return 0;
			
		}break;
	case WM_RBUTTONDOWN:
		{
			S_Core::p_InterfaceObject->receiveSystemMessage(message, wParam);
		}break;
	case WM_KEYDOWN:
		{
			S_Core::p_InterfaceObject->receiveSystemMessage(message, wParam);
		}break;
	case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		} break;
	case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		} break;
	case WM_QUIT:
		{
			PostQuitMessage(0);
			return 0;
		} break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}