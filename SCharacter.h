#ifndef SCHARACTER_H
#define SCHARACTER_H

#include "SRender.h"
#include "SAbility.h"

#include <string>
#include <map>

#include "SAUDIO.h"
class SCharacter
{
public:
	static void loadCharacters(void);
	static void unloadCharacters(void);
	static SCharacter* new_instance(std::string characterName, bool isServerInstance);
	
	SCharacter(void);
	
	~SCharacter(void);
	float getMovementMultiplier(void);
	float getJumpVelocity(void);
	SMesh* getMesh(void);
	SAbility* getAbility(int index);

	SImage* getIcon(void);

private:
	static std::map<std::string, SCharacter*> _characters;

	std::string _name;
	float _moveMultiplier;
	float _jumpVelocity;//basically height
	SMesh* _mesh;

	std::vector <SAbility*> _abilities;

	//Now for various audio things
	std::vector <psAudio::Sound*> _spawnPhrases;

	//The character icon
	SImage* _image;
protected:
};
#endif

