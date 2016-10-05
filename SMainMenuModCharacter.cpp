#include "SMainMenuModCharacter.h"

static SMainMenuModCharacter* lastInstance = 0;
static void saveAndCloseClick(void);
SMainMenuModCharacter::SMainMenuModCharacter(void)
{
	//just used so that the static functions for buttons can use this class properly
	lastInstance = this;

	_frame = 0;
	_nameLabel = 0;
	_nameField = 0;

	_frame = new psGui::Frame(0.0f, 0.0f, 0.4f, 0.6f);
	_frame->setColor(0xcf1f1f1f);

	_nameLabel = new psGui::Label();
	_nameLabel->setSize(0.5f, 0.05f);
	_nameLabel->setTextColor(0xffffffff);
	_nameLabel->setRelativePosition(-0.1f, 0.2f);
	_nameLabel->setText(L"Display Name");
	_nameLabel->setTextSize(5);
	_nameLabel->setTextAlignment(DT_LEFT | DT_VCENTER);
	_frame->addComponent(_nameLabel);

	_nameField = new psGui::TextField();
	_nameField->setSize(0.2f, 0.05f);
	_nameField->setFieldColor(0xcf2f2f2f);
	_nameField->setTextColor(0xffffffff);
	_nameField->setTextSize(6);
	_nameField->setRelativePosition(0.1f, 0.2f);
	_nameField->setActive(true);
	_frame->addComponent(_nameField);


	saveAndCloseButton = new psGui::SimpleButton();
	saveAndCloseButton->setSize(0.2f, 0.05f);
	saveAndCloseButton->setRelativePosition(0.0f, -0.25f);
	saveAndCloseButton->setColor(0x3f8f0fef);
	saveAndCloseButton->setCaptionColor(0xffffffff);
	saveAndCloseButton->setCaption(L"Save and Close");
	saveAndCloseButton->setFunctionPointer(saveAndCloseClick);
	saveAndCloseButton->setCaptionSize(4);
	_frame->addComponent(saveAndCloseButton);


	_frame->setPosition(0.0f, 0.0f);
	_frame->setVisible(false);
}
 
SMainMenuModCharacter::~SMainMenuModCharacter(void)
{
	delete _frame;
	delete _nameLabel;
	delete _nameField;
	delete saveAndCloseButton;

	_frame = 0;
	_nameLabel = 0;
	_nameField = 0;
	saveAndCloseButton = 0;
}

void SMainMenuModCharacter::update(void)
{
	_frame->update();
}
void SMainMenuModCharacter::setFrameVisibility(bool visibility)
{
	_frame->setVisible(visibility);

	_nameField->setActive(visibility);
}

bool SMainMenuModCharacter::getFrameVisibility(void)
{
	return _frame->getVisible();
}

void SMainMenuModCharacter::receiveSystemMessage(UINT message, UINT param)
{
	_frame->sendSystemMessage(message, param); 
}

#include <fstream>
void saveAndCloseClick(void)
{
	//First save all info into the file
	std::ofstream file;
	file.open(L"../Resources/Configs/char.cfg", std::ios::trunc | std::ios::binary);
	/* map official name */
	{
	std::wstring firstString = lastInstance->nameField()->getText();
	std::string converted(firstString.begin(), firstString.end()); 
	unsigned int nameSize = converted.size();
	file.write(reinterpret_cast<const char*>(&nameSize), sizeof(int));
	file.write(converted.c_str(), converted.size());
	}
	file.close();

	lastInstance->setFrameVisibility(false);


}

psGui::Frame* SMainMenuModCharacter::frame(void)
{
	return _frame;
}

psGui::TextField* SMainMenuModCharacter::nameField(void)
{
	return _nameField;
}