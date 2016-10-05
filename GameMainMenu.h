#ifndef GAMEMAINMENU_H
#define GAMEMAINMENU_H

#include "psGui.h"
#include "SInterface.h"

#include "GameLoadoutMenu.h"
#define GAMEMAINMENU_EVENT_NOTHING 0
#define GAMEMAINMENU_EVENT_RESUME 1
#define GAMEMAINMENU_EVENT_QUIT 2
#define GAMEMAINMENU_EVENT_LEAVELOBBY 3

#include <deque>
class GameMainMenu
{
public:
	GameMainMenu(void);
	~GameMainMenu(void);

	

	void setFrameVisibility(bool visibility);
	bool getFrameVisibility(void);

	
	void receiveSystemMessage(UINT message, UINT param);
	int update(void);

	void createEvent(int event);
private:
	

	psGui::Frame* _frame;
	psGui::Label* nameLabel;
	
	std::deque<int> actions;

	psGui::SimpleButton* _resumeButton;
	psGui::SimpleButton* _loadoutConfigButton;
	psGui::SimpleButton* _leaveLobbyButton;
	psGui::SimpleButton* _quitButton;

	GameLoadoutMenu* _loadoutMenu;

};
#endif