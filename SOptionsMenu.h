#ifndef SOPTIONSMENU_H
#define SOPTIONSMENU_H

#include "psGui.h"
class SOptionsMenu
{
public:
	SOptionsMenu(void);
	~SOptionsMenu(void);

	void setFrameVisibility(bool visibility);
	bool getFrameVisibility(void);

	void receiveSystemMessage(UINT message, UINT param);
	void update(void);

	psGui::Frame* frame(void);
	psGui::TextField* nameField(void);
	psGui::TextField* netIPField(void);
	unsigned char getLightingSelection(void);
	bool getSSAOSelection(void);
	bool getMotionBlurSelection(void);
	bool getDepthOfFieldSelection(void);
	unsigned int getResolutionWidthSelection(void);
	unsigned int getResolutionHeightSelection(void);

	void syncWithGlobalConfiguration(void);
private:
	psGui::Frame* _frame;
	psGui::Simple* _frameHeader;
	psGui::Label* _frameTitle;

	psGui::Simple* _generalOptionsBack;
	psGui::Label* _generalOptionsLabel;

	psGui::Label* _nameLabel;
	psGui::TextField* _nameField;

	psGui::Label* _netIPLabel;
	psGui::TextField* _netIPField;

	psGui::Simple* _graphicalOptionsBack; 
	psGui::Label* _graphicalOptionsLabel;

	unsigned char _lightingSelection;
	psGui::Label* _lightingLabel;
	psGui::SimpleButton* _lightingButtonLow;
	psGui::SimpleButton* _lightingButtonMed;
	psGui::SimpleButton* _lightingButtonHigh;

	bool _ssaoSelection;
	psGui::Label* _ssaoLabel;
	psGui::Checkbox* _ssaoBox;

	bool _motionBlurSelection;
	psGui::Label* _motionBlurLabel;
	psGui::Checkbox* _motionBlurBox; 

	bool _depthOfFieldSelection;
	psGui::Label* _depthOfFieldLabel;
	psGui::Checkbox* _depthOfFieldBox;

	unsigned int _resolutionWidthSelection;
	unsigned int _resolutionHeightSelection;
	psGui::Listbox* _resolutionList;

	psGui::SimpleButton* saveAndCloseButton;
};
#endif