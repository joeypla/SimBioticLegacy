#ifndef GAMELOADOUTMENU_H
#define GAMELOADOUTMENU_H

#include "psGui.h"
#include "SInterface.h"




#include <deque>
class GameLoadoutMenu
{
public:
	GameLoadoutMenu(void);
	~GameLoadoutMenu(void);

	

	void setFrameVisibility(bool visibility);
	bool getFrameVisibility(void);

	
	void receiveSystemMessage(UINT message, UINT param);
	int update(void);

	void createEvent(int event);
private:
	

	psGui::Frame* _frame;
	psGui::Label* nameLabel;
	
	std::deque<int> actions;

	psGui::SimpleButton* _magentorButton;
	psGui::SimpleButton* _prophetButton;
	psGui::SimpleButton* _conspiracyButton;

};
#endif