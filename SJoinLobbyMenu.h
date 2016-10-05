#ifndef SJOINLOBBYMENU_H
#define SJOINLOBBYMENU_H

#include "psGui.h"


class SMatchmakingMenu;

class SJoinLobbyMenu
{
public:
	SJoinLobbyMenu(void);
	~SJoinLobbyMenu(void);

	void setFrameVisibility(bool visibility);
	bool getFrameVisibility(void);

	void receiveSystemMessage(UINT message, UINT param);
	void update(void);

	psGui::Frame* frame(void);
	std::string getIP(void);

	void setParentMenu(SMatchmakingMenu* pMatchmakingMenu);
	SMatchmakingMenu* getParentMenu(void);
private:
	SMatchmakingMenu* _pMatchmakingMenu;//we need this so we have control over the closing of some frames

	psGui::Frame* _frame;
	psGui::Simple* _frameHeader;
	psGui::Label* _headerTitle;
	psGui::TextField* _IPField;
	psGui::SimpleButton* _joinButton;
	psGui::SimpleButton* _backButton;
};
#endif