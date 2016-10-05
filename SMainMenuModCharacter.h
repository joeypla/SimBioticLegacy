#ifndef SMAINMENUMODCHARACTER_H
#define SMAINMENUMODCHARACTER_H

#include "psGui.h"
class SMainMenuModCharacter
{
public:
	SMainMenuModCharacter(void);
	~SMainMenuModCharacter(void);

	void setFrameVisibility(bool visibility);
	bool getFrameVisibility(void);

	void receiveSystemMessage(UINT message, UINT param);
	void update(void);

	psGui::Frame* frame(void);
	psGui::TextField* nameField(void);
private:
	psGui::Frame* _frame;
	psGui::Label* _nameLabel;
	psGui::TextField* _nameField;
	unsigned char _characterSelected;
	psGui::SimpleButton* characterButtons[4];


	psGui::SimpleButton* saveAndCloseButton;
};
#endif