#ifndef SCREATELOBBYMENU_H
#define SCREATELOBBYMENU_H

#include "psGui.h"


class SMatchmakingMenu;

class SCreateLobbyMenu
{
public:
	SCreateLobbyMenu(void);
	~SCreateLobbyMenu(void);

	void setFrameVisibility(bool visibility);
	bool getFrameVisibility(void);

	void receiveSystemMessage(UINT message, UINT param);
	void update(void);

	psGui::Frame* frame(void);
	psGui::TextField* nameField(void);

	void setParentMenu(SMatchmakingMenu* pMatchmakingMenu);
	SMatchmakingMenu* getParentMenu(void);
private:
	SMatchmakingMenu* _pMatchmakingMenu;//we need this so we have control over the closing of some frames

	psGui::Frame* _frame;
	psGui::Simple* _frameHeader;
	psGui::Label* _headerTitle;
	psGui::TextField* _lobbyName;
	psGui::SimpleButton* _createButton;
	psGui::SimpleButton* _cancelButton;
};
#endif