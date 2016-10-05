#pragma once

#include <Windows.h>
#include "OperatingSystemInterface.h"
namespace adAPT {
	class OSIWin32 : public OperatingSystemInterface {
	public:
		OSIWin32(void);
		~OSIWin32(void);

		bool initialize(const OperatingSystemInterfaceProperties& properties);
		void update(void);
	private:
		HWND _hWnd;
		MSG _msg;
		int _nCmdShow;
	};
};