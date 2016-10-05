#ifndef SYMBIOTE_SCORE_H
#define SYMBIOTE_SCORE_H



#define _WINSOCKAPI_
//#include <Windows.h>

#include "SGUI.h"
#include "SInput.h"
#include "SInterface.h"
#include "SAUDIO.h"

//The core static class is the base of everything in this engine. It
//deals with setuping and maintaining every module through a branch system.

//This class should not be instantiated in any way please. It
//is only to be used statically.


	//Static Class Core
	class S_Core{
	public:
		static S_Interface *p_InterfaceObject;
		static S_Interface *p_nextInterface;

		static bool update(void);
		static bool setup(HINSTANCE p_hInstance, int p_nCmdShow);
		static HINSTANCE getHINSTANCE(void);
		static void destroy(void);
		static void goToInterfaceObject(S_Interface* p_interfaceObject);
	private:
		S_Core(void);//Private Constructor = No instantiation
		~S_Core(void);//Private Destructor = No destruction
		static HINSTANCE hInstance;//app handle
		static int nCmdShow;
		static bool _goToNextInterface;
		//static DWORD elapsedTime;
	};

#endif