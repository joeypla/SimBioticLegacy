#ifndef SMAINMENU_H
#define SMAINMENU_H

#include "SInterface.h"
#include <string>

#include "SMatchmakingMenu.h"
#include "SMainMenuModCharacter.h"
#include "SOptionsMenu.h"


class SMainMenu: public S_Interface{
public:
	SMainMenu(void);
	~SMainMenu(void);
	

	bool renderToDevice(const LPDIRECT3DDEVICE9 dev);
	bool updateInterface(DWORD lastTimeTaken);
	void receiveSystemMessage(UINT message, UINT wParam);

	SOptionsMenu* getOptionsMenu(void);
	SMatchmakingMenu* getMatchmakingMenu(void);
	void setManualJoinFrameVisibility(bool visible);
private:
	
	SMainMenuModCharacter modCharObject;
	SOptionsMenu* optionsMenu;
	SMatchmakingMenu* matchmakingMenu;
	
	S2D* _enthusiastLogo;
	void setup(void);
	void tearDown(void);
};
#endif