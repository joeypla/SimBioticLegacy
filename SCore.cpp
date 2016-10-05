
#include "SCore.h"
#include <iostream>
#include <ctime>
#include "SFont.h"
#include "SymbioteWinsock.h"
#include "SMainMenu.h"
#include "SLevelEditor.h"
#include "SPreMenu.h"
#include "SLOG.h"
#include "SSGame.h"
#include "pseBase.h"
#include "GameConfigInterface.h"
//define static members from core
HINSTANCE S_Core::hInstance;
int S_Core::nCmdShow;
S_Interface* S_Core::p_InterfaceObject;
S_Interface* S_Core::p_nextInterface;
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR lpCmdLine, int nCmdShow){
	//Setup Core.
	srand((unsigned)time(0));
	if (!S_Core::setup(hInstance, nCmdShow)) return 0;
	const int updatesPerSecond = 60;
	const int SKIP_TICKS = (int)(float)1000/updatesPerSecond;
	DWORD next_tick = GetTickCount();
	int sleepTime = 0;
	while (true){

		if (!S_Core::update()) break;
		next_tick += SKIP_TICKS;
		sleepTime = next_tick - GetTickCount();
		if (sleepTime >= 0){
			//Sleep(sleepTime);
		}
	}
	S_Core::destroy();
	return 0;
}

bool S_Core::setup(HINSTANCE p_hInstance, int p_nCmdShow){
	S_LOG::start();
	S_LOG::clear();

	GlobalConfiguration::getSingleton()->loadConfiguration();
	//Setup Windows Members
	hInstance = p_hInstance;
	nCmdShow = p_nCmdShow;
	 
	
	//Setup SGUIwd
	S_GUI::setup(hInstance, nCmdShow);
	//SETUP SFONT AFTER SGUI!!!
	S_FONT::loadAllFonts();
	//Setup SINPUT
	S_INPUT::setup();
	//SETUP AUDIO
	S_AUDIO::createSingleton(); 
	//SETUP NETWORKING IF NEED BE
	psNetworking::init();
	if (!S_AUDIO::getSingleton()->init()){}//do nothing for now - no audio isnt a big deal;
	//Instantiate Interface Object (SETUP LIES IN CONSTRUCTION)
	p_InterfaceObject = new SPreMenu();
	
	
	return true;
}

bool S_Core::update(void){
	//Assume that all should take 1/60 seconds to complete
	//obviously not true, but for game logic sake :)
	static DWORD lastTimeTaken = 17; //"takes 17 ms to run for first time"

	//before anything, if we need to update the interface, we do it here
	if (p_nextInterface != 0)
	{
		delete p_InterfaceObject;
		p_InterfaceObject = p_nextInterface;
		p_nextInterface = 0;
	}
	if (!S_INPUT::update())
		return false;
	

	
	if (p_InterfaceObject != NULL){
		if (!p_InterfaceObject->updateInterface(lastTimeTaken)){
			return false;
		}
	}


	
	if (S_AUDIO::getSingleton()){
		if (!S_AUDIO::getSingleton()->update()){
			return false;
		}
	}
	
	if (!S_GUI::update()){
		return false;
	}
	
	
	return true;
}

HINSTANCE S_Core::getHINSTANCE(void){
	return hInstance;
}

void S_Core::destroy(void){
	//S_GUI::release();
	//S_INPUT::release();
	psNetworking::destroy();
	S_LOG::finish();
}

void S_Core::goToInterfaceObject(S_Interface* pInterface){
	if (p_nextInterface != 0) delete p_nextInterface;
	p_nextInterface = pInterface;
	
	
}