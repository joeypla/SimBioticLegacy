#include "SOptionsMenu.h"

#include "SInput.h"
#include "GameConfigInterface.h"
static SOptionsMenu* lastInstance = 0;
static void saveAndCloseClick(void);
SOptionsMenu::SOptionsMenu(void)
{
	lastInstance = this;
	_frame = new psGui::Frame(0.0f, 0.0f, 1.0f, 1.0f);
	_frame->setColor(0x7f3f3f3f);
	_frame->setVisible(true);

	_frameHeader = new psGui::Simple();
	_frameHeader->setColor(0x7f2f2f2f);
	_frameHeader->setSize(1.0f, 0.05f);
	_frameHeader->setRelativePosition(0.0f, 0.475f);

	_frame->addComponent(_frameHeader);


	_frameTitle = new psGui::Label();
	_frameTitle->setText(L"Options");
	_frameTitle->setTextAlignment(DT_CENTER | DT_VCENTER);
	_frameTitle->setTextSize(6);
	_frameTitle->setTextColor(0xffffffff);
	_frameTitle->setRelativePosition(0.0f, 0.475f);
	_frameTitle->setSize(1.0f, 0.05f);
	_frame->addComponent(_frameTitle);

	_generalOptionsBack = new psGui::Simple();
	_generalOptionsBack->setColor(0x7f2f2f2f);
	_generalOptionsBack->setSize(0.4f, 0.8f);
	_generalOptionsBack->setRelativePosition(-0.25f, 0.0f);
	_frame->addComponent(_generalOptionsBack);

	_generalOptionsLabel = new psGui::Label();
	_generalOptionsLabel->setText(L"General");
	_generalOptionsLabel->setTextAlignment(DT_CENTER | DT_VCENTER);
	_generalOptionsLabel->setTextSize(6);
	_generalOptionsLabel->setTextColor(0xffffffff);
	_generalOptionsLabel->setRelativePosition(-0.25f, 0.375f);
	_generalOptionsLabel->setSize(0.4f, 0.05f);
	_frame->addComponent(_generalOptionsLabel);

	_nameLabel = new psGui::Label();
	_nameLabel->setText(L"Online Persona");
	_nameLabel->setTextAlignment(DT_LEFT | DT_VCENTER);
	_nameLabel->setTextSize(6);
	_nameLabel->setTextColor(0xffffffff);
	_nameLabel->setRelativePosition(-0.25f, 0.3f);
	_nameLabel->setSize(0.38f, 0.05f);
	_frame->addComponent(_nameLabel);

	_nameField = new psGui::TextField();
	_nameField->setFieldColor(0xff4f4f4f);
	_nameField->setSize(0.38f, 0.05f);
	_nameField->setTextSize(6);
	_nameField->setTextColor(0xffffffff);
	_nameField->setRelativePosition(-0.25f, 0.22f);
	_frame->addComponent(_nameField);

	_netIPLabel = new psGui::Label();
	_netIPLabel->setText(L"NET IP Address");
	_netIPLabel->setTextAlignment(DT_LEFT | DT_VCENTER);
	_netIPLabel->setTextSize(6);
	_netIPLabel->setTextColor(0xffffffff);
	_netIPLabel->setRelativePosition(-0.25f, 0.12f);
	_netIPLabel->setSize(0.38f, 0.05f);
	_frame->addComponent(_netIPLabel);

	_netIPField = new psGui::TextField();
	_netIPField->setFieldColor(0xff4f4f4f);
	_netIPField->setSize(0.38f, 0.05f);
	_netIPField->setTextSize(6);
	_netIPField->setTextColor(0xffffffff);
	_netIPField->setRelativePosition(-0.25f, 0.04f);
	_frame->addComponent(_netIPField);

	_graphicalOptionsBack = new psGui::Simple();
	_graphicalOptionsBack->setColor(0x7f2f2f2f);
	_graphicalOptionsBack->setSize(0.4f, 0.8f);
	_graphicalOptionsBack->setRelativePosition(0.25f, 0.0f);
	_frame->addComponent(_graphicalOptionsBack);

	_graphicalOptionsLabel = new psGui::Label();
	_graphicalOptionsLabel->setText(L"Graphics");
	_graphicalOptionsLabel->setTextAlignment(DT_CENTER | DT_VCENTER);
	_graphicalOptionsLabel->setTextSize(6);
	_graphicalOptionsLabel->setTextColor(0xffffffff);
	_graphicalOptionsLabel->setRelativePosition(0.25f, 0.375f);
	_graphicalOptionsLabel->setSize(0.4f, 0.05f);
	_frame->addComponent(_graphicalOptionsLabel);


	_lightingLabel = new psGui::Label();
	_lightingLabel->setText(L"Lighting Quality");
	_lightingLabel->setTextAlignment(DT_LEFT | DT_VCENTER);
	_lightingLabel->setTextSize(6);
	_lightingLabel->setTextColor(0xffffffff);
	_lightingLabel->setRelativePosition(0.25f, 0.3f);
	_lightingLabel->setSize(0.38f, 0.05f);
	_frame->addComponent(_lightingLabel);
	 
	_lightingButtonLow = new psGui::SimpleButton();
	_lightingButtonLow->setCaption(L"LOW");
	_lightingButtonLow->setColor(0x8f3f3f3f);
	_lightingButtonLow->setRelativePosition(0.12f, 0.22f);
	_lightingButtonLow->setCaptionColor(0xffffffff);
	_lightingButtonLow->setSize(0.1f, 0.05f);
	_frame->addComponent(_lightingButtonLow);

	_lightingButtonMed = new psGui::SimpleButton();
	_lightingButtonMed->setCaption(L"MEDIUM");
	_lightingButtonMed->setColor(0x8f3f3f3f);
	_lightingButtonMed->setRelativePosition(0.24f, 0.22f);
	_lightingButtonMed->setCaptionColor(0xffffffff);
	_lightingButtonMed->setSize(0.1f, 0.05f);
	_frame->addComponent(_lightingButtonMed);

	_lightingButtonHigh = new psGui::SimpleButton();
	_lightingButtonHigh->setCaption(L"HIGH");
	_lightingButtonHigh->setColor(0x8f3f3f3f);
	_lightingButtonHigh->setRelativePosition(0.36f, 0.22f);
	_lightingButtonHigh->setCaptionColor(0xffffffff);
	_lightingButtonHigh->setSize(0.1f, 0.05f);
	_frame->addComponent(_lightingButtonHigh);
	
	_resolutionList = new psGui::Listbox(5, 0.13f, 0.0f, 0.15f, 0.3f, 0.03f, 1, 0.05f);
	_resolutionList->setRelativePosition(0.2f, 0.0f);
	_resolutionList->setColumnTitle(0, L"Resolution");
	_resolutionList->setHeaderColor(0x9f3f3f3f);
	_resolutionList->setBackColor1(0x8f6f6f6f);
	_resolutionList->setBackColor2(0x8f8f8f8f);
	
	psGui::ListboxEntry entry;
	entry.values.push_back(L"640x480");
	_resolutionList->addRow(&entry);
	entry.values[0] = L"800x600";
	_resolutionList->addRow(&entry);
	entry.values[0] = L"1024x768";
	_resolutionList->addRow(&entry);
	entry.values[0] = L"1280x720";
	_resolutionList->addRow(&entry);
	entry.values[0] = L"1280x768";
	_resolutionList->addRow(&entry);
	entry.values[0] = L"1280x800";
	_resolutionList->addRow(&entry);
	entry.values[0] = L"1360x768";
	_resolutionList->addRow(&entry);
	entry.values[0] = L"1440x900";
	_resolutionList->addRow(&entry);
	entry.values[0] = L"1600x900";
	_resolutionList->addRow(&entry);
	entry.values[0] = L"1680x1050";
	_resolutionList->addRow(&entry);
	entry.values[0] = L"1920x1080";
	_resolutionList->addRow(&entry);
	entry.values[0] = L"1920x1200";
	_resolutionList->addRow(&entry);
	entry.values[0] = L"1920x1440";
	_resolutionList->addRow(&entry);
	entry.values[0] = L"2048x1536";
	_resolutionList->addRow(&entry);
	entry.values[0] = L"2560x1600";
	_resolutionList->addRow(&entry);
	_frame->addComponent(_resolutionList);

	_ssaoLabel = new psGui::Label();
	_ssaoLabel->setText(L"Ambient Occlusion");
	_ssaoLabel->setTextColor(0xffffffff);
	_ssaoLabel->setTextAlignment(DT_VCENTER | DT_LEFT);
	_ssaoLabel->setSize(0.2f, 0.05f);
	_ssaoLabel->setTextSize(6);
	_ssaoLabel->setRelativePosition(0.30f, 0.1f);

	_ssaoBox = new psGui::Checkbox();
	_ssaoBox->setRelativePosition(0.4f, 0.1f);
	_ssaoBox->setSize(0.05f);
	_ssaoBox->setColor(0x9f3f3f3f);
	
	_frame->addComponent(_ssaoLabel);
	_frame->addComponent(_ssaoBox);


	_motionBlurLabel = new psGui::Label();
	_motionBlurLabel->setText(L"Motion Blur");
	_motionBlurLabel->setTextColor(0xffffffff);
	_motionBlurLabel->setTextAlignment(DT_VCENTER | DT_LEFT);
	_motionBlurLabel->setSize(0.2f, 0.05f);
	_motionBlurLabel->setTextSize(6);
	_motionBlurLabel->setRelativePosition(0.30f, 0.0f);

	_motionBlurBox = new psGui::Checkbox();
	_motionBlurBox->setRelativePosition(0.4f, 0.0f);
	_motionBlurBox->setSize(0.05f);
	_motionBlurBox->setColor(0x9f3f3f3f);

	_frame->addComponent(_motionBlurLabel);
	_frame->addComponent(_motionBlurBox);

	_depthOfFieldLabel = new psGui::Label();
	_depthOfFieldLabel->setText(L"Depth Of Field");
	_depthOfFieldLabel->setTextColor(0xffffffff);
	_depthOfFieldLabel->setTextAlignment(DT_VCENTER | DT_LEFT);
	_depthOfFieldLabel->setSize(0.2f, 0.05f);
	_depthOfFieldLabel->setTextSize(6);
	_depthOfFieldLabel->setRelativePosition(0.30f, -0.1f);

	_depthOfFieldBox = new psGui::Checkbox();
	_depthOfFieldBox->setRelativePosition(0.4f, -0.1f);
	_depthOfFieldBox->setSize(0.05f);
	_depthOfFieldBox->setColor(0x9f3f3f3f);

	_frame->addComponent(_depthOfFieldLabel);
	_frame->addComponent(_depthOfFieldBox);

	saveAndCloseButton = new psGui::SimpleButton();
	saveAndCloseButton->setSize(0.2f, 0.05f);
	saveAndCloseButton->setRelativePosition(0.3f, -0.45f);
	saveAndCloseButton->setColor(0x3f8f0fef);
	saveAndCloseButton->setCaptionColor(0xffffffff);
	saveAndCloseButton->setCaption(L"Save and Close");
	saveAndCloseButton->setFunctionPointer(saveAndCloseClick);
	saveAndCloseButton->setCaptionSize(4);
	_frame->addComponent(saveAndCloseButton);

	_frame->setPosition(0.0f, 0.0f);
	_frame->setVisible(false);
}

SOptionsMenu::~SOptionsMenu(void)
{
	delete _frame;
	_frame = 0;
	delete _frameHeader;
	delete _frameTitle;
	delete _graphicalOptionsBack;
	delete _generalOptionsBack;
	delete _graphicalOptionsLabel;
	delete _generalOptionsLabel;
	delete _lightingLabel;
	delete _lightingButtonLow;
	delete _lightingButtonMed;
	delete _lightingButtonHigh;

	delete _ssaoLabel;
	delete _ssaoBox;

	delete _motionBlurLabel;
	delete _motionBlurBox;
	
	delete _depthOfFieldLabel;
	delete _depthOfFieldBox;

	delete _resolutionList;

	delete saveAndCloseButton;
}

bool SOptionsMenu::getFrameVisibility()
{
	if (_frame == 0) return false;
	return _frame->getVisible();
}

void SOptionsMenu::setFrameVisibility(bool v)
{
	_frame->setVisible(v);

	syncWithGlobalConfiguration();
}

void SOptionsMenu::update(void)
{
	_frame->update();

	if (S_INPUT::mousePressed(0))
	{
		if (_nameField->getMouseWithin()) _nameField->setActive(true);
		else _nameField->setActive(false);

		if (_netIPField->getMouseWithin()) _netIPField->setActive(true);
		else _netIPField->setActive(false);
	}
}

void SOptionsMenu::receiveSystemMessage(UINT message, UINT param)
{
	_frame->sendSystemMessage(message, param);
}

psGui::TextField* SOptionsMenu::nameField(void){return _nameField;}
psGui::TextField* SOptionsMenu::netIPField(void){return _netIPField;}
unsigned char SOptionsMenu::getLightingSelection(void){return _lightingSelection;}
bool SOptionsMenu::getSSAOSelection(void){return _ssaoBox->getState();}
bool SOptionsMenu::getMotionBlurSelection(void){return _motionBlurSelection;}
bool SOptionsMenu::getDepthOfFieldSelection(void){return _depthOfFieldSelection;}
unsigned int SOptionsMenu::getResolutionWidthSelection(void){return _resolutionWidthSelection;}
unsigned int SOptionsMenu::getResolutionHeightSelection(void){return _resolutionHeightSelection;}
#include <fstream>
void saveAndCloseClick(void)
{
	S_AUDIO::getSingleton()->playClick0();
	//First save all info into the file
	std::ofstream file;
	file.open(GLOBALCONFIG_FILENAME, std::ios::trunc | std::ios::binary);
	/* first store persona */
	{
	std::wstring firstString = lastInstance->nameField()->getText();
	std::string converted(firstString.begin(), firstString.end()); 
	unsigned int nameSize = converted.size();
	file.write(reinterpret_cast<const char*>(&nameSize), sizeof(int));
	file.write(converted.c_str(), converted.size());
	}

	/* then store NET ip address string */
	{
		std::wstring firstString = lastInstance->netIPField()->getText();
		std::string converted(firstString.begin(), firstString.end()); 
		unsigned int IPSize = converted.size();
		file.write(reinterpret_cast<const char*>(&IPSize), sizeof(int));
		file.write(converted.c_str(), converted.size());
	}
	/* store resolution */
	unsigned int resWidth = lastInstance->getResolutionWidthSelection();
	unsigned int resHeight = lastInstance->getResolutionHeightSelection();
	file.write(reinterpret_cast<const char*>(&resWidth), sizeof(int));
	file.write(reinterpret_cast<const char*>(&resHeight), sizeof(int));
	/* then store lighting quality value */
	unsigned char lightingSelection = lastInstance->getLightingSelection();
	file.write(reinterpret_cast<const char*>(&lightingSelection), sizeof(char));

	/* store ssao selection */
	bool ssaoSelection = lastInstance->getSSAOSelection();
	file.write(reinterpret_cast<const char*>(&ssaoSelection), sizeof(bool));

	/* store motionBlur selection */
	bool motionBlurSelection = lastInstance->getMotionBlurSelection();
	file.write(reinterpret_cast<const char*>(&motionBlurSelection), sizeof(bool));

	/* store depth of field selection */
	bool depthOfFieldSelection = lastInstance->getDepthOfFieldSelection();
	file.write(reinterpret_cast<const char*>(&depthOfFieldSelection), sizeof(bool));
	file.close();

	GlobalConfiguration::getSingleton()->loadConfiguration();
	lastInstance->setFrameVisibility(false);


}


void SOptionsMenu::syncWithGlobalConfiguration(void)
{
	std::string screenName = GlobalConfiguration::getSingleton()->str_screenName();
	std::wstring wScreenName(screenName.begin(), screenName.end());
	_nameField->setText(wScreenName.c_str());

	std::string netIP = GlobalConfiguration::getSingleton()->str_netAddress();
	std::wstring wNetIP(netIP.begin(), netIP.end());
	_netIPField->setText(wNetIP.c_str());

	_ssaoBox->setState(GlobalConfiguration::getSingleton()->bSSAO());
	_motionBlurBox->setState(GlobalConfiguration::getSingleton()->bMotionBlur());
	_depthOfFieldBox->setState(GlobalConfiguration::getSingleton()->bDOF());
}