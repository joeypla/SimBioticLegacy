#ifndef PS_WEAPON_H
#define PS_WEAPON_H

#include "SRender.h"
#include <string>

#include "VoxelData.h"
#include "SAUDIO.h"
namespace ps{
	class Actor;
	class GameData;

	class Weapon{
		friend class WeaponBase;
	public:
		~Weapon(void);
		void fire(Actor* owner, GameData* pGameData);
		bool reload(Actor* owner);
		void updateAnimation(Actor* owner);
		const wchar_t* name(void);

		float aiming;
		float spread;
		bool hasDot(void);

		int ammo(void);
		int ammoMag(void);

		float movementMultiplier(void);
		float strafeMultiplier(void);

		float getCurrentMobilityFactor(void);
	private:
		/* Private constructor - should not be called
		use WeaponBase::new_Weapon(...) instead */
		Weapon(void);

		
		//gun stats
		std::wstring _name;
		int _dmg;//base weapon damage
		float _range;//range at which the dmg gets reduced
		float _rangeMultiplier;//amount to be reduced after dropoff
		float _movementMultiplier;//self explanatory
		float _strafeMultiplier;//self explanatory
		float _scopeSpeed;//speed at which scope comes up
		bool _automatic;//either automatic or semi-automatic
		bool _projectile;//either projectile or ray tracing
		bool _hasDot;//the dot sight thingy
		int _cooldownLeft;//basically fire rate
		int _cooldown;

		float _baseSpread;//base spread at hip
		float _spread;
		float _spreadIncreasePerShot;
		float _spreadRecuperate;
		float _maxSpread;

		int _ammoMag;
		int _magSize;
		int _ammo;

		bool _reloading;
		float _reloadProgress;
		float _reloadSpeed;
		//render members
		float _flashColor[3];
		float x, y, z;
		float _xRot, _yRot, _zRot; 
		float _xOff, _yOff, _zOff;
		float _xOffMod, _yOffMod, _zOffMod;
		float _sprintXShake;
		float _scale;
		S3D* s3d;//used per instance

		//sound
		psAudio::Sound* _fireSound;
		psAudio::Sound* _hitSound;
		psAudio::Sound* _reloadSound;
	};

	class WeaponBase{
		friend class Weapon;

	public:
		WeaponBase(std::wstring name, std::wstring rvPath, std::wstring texturePath);
		~WeaponBase(void);
		static Weapon* new_Weapon(const WeaponBase* base);

		static void loadAllBaseWeapons(void);
		static void releaseAllBaseWeapons(void);

		static WeaponBase* getBaseWeaponByName(std::wstring name);
		static WeaponBase* getBaseWeaponByIndex(int index);
	private:
		static const int    _weaponBaseCount = 3;
		static WeaponBase** _weaponBaseList;

		std::wstring _name;
		/* Base Offset, Scale, and Rotate before ANYTHING */
		
		//gun stats
		int _dmg;
		float _range;
		bool _projectile;
		bool _hasDot;
		float _rangeMultiplier;
		float _movementMultiplier;
		float _strafeMultiplier;
		float _scopeSpeed;

		float _baseSpread;
		float _spreadIncreasePerShot;
		float _spreadRecuperate;
		float _maxSpread;

		int _cooldown;

		int _ammoMag;
		int _magSize;
		int _ammo;

		float _reloadSpeed;
		//render members
		float _flashColor[3];
		float _xRot, _yRot, _zRot;
		float _xOff, _yOff, _zOff;
		float _scale;
		SMesh* _mesh;
		SMaterialSet* _materialSet;
		//sound
		psAudio::Sound* _fireSound;
		psAudio::Sound* _hitSound;
		psAudio::Sound* _reloadSound;
	};
};

#endif