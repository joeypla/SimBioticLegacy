#ifndef SSINGLEMATCHMENU_H
#define SSINGLEMATCHMENU_H

#include "psGui.h"
#include "SInterface.h"
class SSingleMatchMenu : public S_Interface
{
public:
	SSingleMatchMenu(void);
	~SSingleMatchMenu(void);

	void setFrameVisibility(bool visibility);
	bool getFrameVisibility(void);

	bool renderToDevice(const LPDIRECT3DDEVICE9 dev);
	bool updateInterface(DWORD lastTimeTaken);
	void receiveSystemMessage(UINT message, UINT param);
	
private:
	psGui::Frame* frame;
	psGui::Label* nameLabel;
	psGui::TextField* nameField;

	psGui::SimpleButton* saveAndCloseButton;
};
#endif