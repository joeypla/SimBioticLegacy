#include "SCharacter.h"

std::map<std::string, SCharacter*> SCharacter::_characters;

void SCharacter::loadCharacters(void)
{
	//The paragon
	SCharacter* paragon = new SCharacter();
	paragon->_image = new SImage(L"../Resources/Textures/Characters/hackericon.jpg");
	paragon->_moveMultiplier = 1.0f;
	paragon->_abilities.push_back(SAbility::new_instance("wallhack", false));
	paragon->_abilities.push_back(SAbility::new_instance("blink", false));
	paragon->_abilities.push_back(SAbility::new_instance("firebolt", false));
	_characters.insert(std::pair<std::string, SCharacter*>(std::string("paragon"), paragon));
	//paragon->
}

void SCharacter::unloadCharacters(void)
{
}

SCharacter* SCharacter::new_instance(std::string characterName, bool isServerInstance)
{
	//see if it can be found
	std::map<std::string, SCharacter*>::iterator it;
	it = _characters.find(characterName);

	if (it == _characters.end()) return NULL;

	SCharacter* instance = new SCharacter();
	SCharacter* original = it->second;

	instance->_jumpVelocity = original->_jumpVelocity;
	instance->_moveMultiplier = original->_moveMultiplier;

	if (!isServerInstance)
	{
		instance->_image = original->_image;
		instance->_mesh = original->_mesh;
	}

	for (int i = 0; i < original->_abilities.size(); i++)
		instance->_abilities.push_back(SAbility::new_instance(original->_abilities.at(i)->getName(), isServerInstance));


	return it->second;
}
SCharacter::SCharacter(void)
{
	_mesh = 0;
	_image = 0;
}

SCharacter::~SCharacter(void)
{
}

float SCharacter::getMovementMultiplier(void)
{
	return _moveMultiplier;
}

float SCharacter::getJumpVelocity(void)
{
	return _jumpVelocity;
}

SAbility* SCharacter::getAbility(int index)
{
	if (index < 0) return NULL;
	if (index >= _abilities.size()) return NULL;

	return _abilities.at(index);
}

SImage* SCharacter::getIcon(void)
{
	return _image;
}