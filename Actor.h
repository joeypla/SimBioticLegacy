#ifndef PS_ACTOR_H
#define PS_ACTOR_H

#include <string>

#include "SRender.h"

#include "Weapon.h"

#include "psMath.h"
#include "SCharacter.h"
namespace ps{
	class GameData;

	static const float ACTOR_DEFAULTHEIGHT = 2.8f;
	static const float ACTOR_DEFAULTWIDTH  = 0.5f;
	static const float ACTOR_DEFAULTVIEWOFFSET = 1.1f;
	static const float ACTOR_DEFAULTVIEWOFFSETCROUCHMULTIPLIER = 0.3f;

	class SetPoint;
	class GameData;
	class Actor{
	public:
		
		Actor(bool isPlayer);

		unsigned char state;
		

		~Actor(void);
		void renderSync(void);
		void update(void);
		void updateAbilities(ps::GameData* pGameData);

		void setName(std::wstring name);
		std::wstring name(void);
		SCamera* view();
		/*Physical Properties */
		float x, y, z;
		float dx, dy, dz;
		float ag;
		bool isJumping;
		bool isGrounded;
		bool isAiming;
		bool isFiring;
		bool isCrouching;
		bool isSprinting;
		bool isGravityLefting;
		bool isGravityRighting;
		bool isGravityForwarding;

		bool isRolling;//used differently than the above. It is not a control sort of thing, but more a status.

		bool isMoving_f;//is actor moving forwards
		bool isMoving_b;//is actor moving forwards
		bool isMoving_l;//is actor moving forwards
		bool isMoving_r;//is actor moving forwards

		//the following are for speed in each direction.
		float sprint;
		float modMoving_f;
		float modMoving_b;
		float modMoving_l;
		float modMoving_r;

		bool changingGravity;
		float gravityChangeProgress;
		int gravityChangeType;
		//rolling properties
		bool isDoingRoll;
		void startRoll(void);
		float roll, rollMax;
		psMath::float3 rollDirection;

		int flux;
		unsigned char health;
		unsigned char shield;

		int kills, deaths, score;
		int modeSpecificVal1;
		int modeSpecificVal2;
		int modeSpecificVal3;
		//sets the actor to a certain gravity config without any animation. Just
		//a quickset. Used for spawning for example. 
		void setToGravityConfig(int gravityConfig);

		int gravityField;

		float maxJogSpeed(void);
		float maxStrafeSpeed(void);
		float maxSprintSpeed(void);

		

		
		void spawn(ps::GameData* pGameData, ps::SetPoint* sp);

		int weaponSelected(void);
		ps::Weapon* weapon1(void);
		ps::Weapon* weapon2(void);

		float xSize(void);
		float ySize(void);
		float zSize(void);

		void doDamage(Actor* offender, int damage);
		unsigned char lastEnemyTargeted;

		int team;
		bool isPlayer; 

		SCharacter* getCharacter(void);

		int castingAbility;//if 0, then casting nothing, else it is the ability being cast by
		//index in character ability vector

		void setWallhacked(bool b);

		bool shouldActivateAbility1;
		bool shouldActivateAbility2;
		bool shouldActivateAbility3;
		bool shouldActivateAbility4;
	private:
		DWORD _timeLastUpdated;
		
		static SMesh* teamWhiteMesh;
		static SMesh* teamBlackMesh;
		std::wstring _name;

		
		//dimensions
		float _xSize;
		float _ySize;
		float _zSize;

		
		//energy attribute
		int _energyMax;
		int _energy;

		//Weapon
		int _weaponSelected;//0 = none
		Weapon *_weapon1, *_weapon2;
		//who hurt this entity last.
		Actor *_lastOffender, *_lastOffended;


		float _maxJogSpeed;
		float _maxStrafeSpeed;
		float _maxSprintSpeed;

		float _crouching;

		psMath::float3 _viewPositionalOffset;

		SCharacter* _pCharacter;

		SCamera* _view; 
		//SRender member
		S3D* _renderObj;

		//SOUND MEMBERS - SO FUCKING TEMPORARY. YOU BETTER FUCKING REMOVE THIS ASAP.
		psAudio::Sound* leftGravitySwitchSound;
		psAudio::Sound* rightGravitySwitchSound;
		psAudio::Sound* jumpSound;
	};
};

#endif