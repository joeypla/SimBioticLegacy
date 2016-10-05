#include "OSIWin32.h"

using namespace adAPT;
OSIWin32::OSIWin32(void) {
}

OSIWin32::~OSIWin32(void) {
}

bool OSIWin32::initialize(const OperatingSystemInterfaceProperties& properties) {

	HINSTANCE hInstance = GetModuleHandle(0);

	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hInstance = hInstance;
	wc.lpszClassName = std::wstring(properties.windowClassName.begin(), properties.windowClassName.end()).c_str();
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpfnWndProc = DefWindowProc;
	RegisterClassEx(&wc);
	_hWnd = CreateWindowEx(NULL, 
		std::wstring(properties.windowClassName.begin(), properties.windowClassName.end()).c_str(),
		std::wstring(properties.windowName.begin(), properties.windowName.end()).c_str(),
		WS_EX_TOPMOST,
		0,0,
		properties.windowWidth, properties.windowHeight,
		NULL,
		NULL,
		hInstance,
		NULL);

	ShowWindow(_hWnd, 1);
	SetFocus(_hWnd);
	return true;
}

void OSIWin32::update(void) {
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (msg.message == WM_QUIT || msg.message == WM_CLOSE){
	}
}