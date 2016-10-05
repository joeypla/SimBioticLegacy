#ifndef SMATCHMAKINGMENU_H
#define SMATCHMAKINGMENU_H

#include "psGui.h"

#include "SimBioticNetClient.h"
#include "SCreateLobbyMenu.h"
#include "SJoinLobbyMenu.h"
class SMatchmakingMenu
{
public:
	SMatchmakingMenu(void);
	~SMatchmakingMenu(void);

	void setFrameVisibility(bool visibility);
	bool getFrameVisibility(void);

	void receiveSystemMessage(UINT message, UINT param);
	
	void update(void);

	psGui::Frame* frame(void);
	psGui::TextField* nameField(void);

	SCreateLobbyMenu& getCreateLobbyMenu(void);
	SJoinLobbyMenu& getJoinLobbyMenu(void);

	SimBioticNetClient& getNetClient(void);
	psGui::Listbox* getInternetListbox(void);

	void showConnecting(void);

private:
	SCreateLobbyMenu createLobbyMenu;
	SJoinLobbyMenu joinLobbyMenu;
	psGui::Frame* _frame;
	psGui::Simple* _frameHeader;
	psGui::Label* _frameTitle;

	psGui::Listbox* _internetServerBox;
	psGui::SimpleButton* _joinServerButton;

	SText* _netStatusText;
	SText* _netMessageText;
	SimBioticNetClient netClient;
	
	psGui::SimpleButton* _joinLobbyButton;
	psGui::SimpleButton* _createLobbyButton;
	psGui::SimpleButton* _joinManualButton;
	psGui::SimpleButton* _refreshButton;

	//Lobby Connecting Notification Frame
	psGui::Frame*           _lobbyConnectingFrame;
	psGui::Label*           _lobbyConnectingLabel;
	psGui::Simple*         _lobbyConnectingSymbol;
	psGui::SimpleButton*   _lobbyConnectingCancel;
};
#endif