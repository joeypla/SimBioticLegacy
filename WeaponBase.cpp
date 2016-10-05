#include "Weapon.h"

using namespace ps;

WeaponBase** WeaponBase::_weaponBaseList;



void WeaponBase::loadAllBaseWeapons(void)
{
	_weaponBaseList = new WeaponBase*[_weaponBaseCount];

	WeaponBase* w; 

	
	_weaponBaseList[0] = new WeaponBase(L"Magentor", L"../Resources/Meshes/Weapons/gun.X", L"../Resources/Weapons/Magentor");
	w = _weaponBaseList[0];
	w->_projectile = false;
	w->_hasDot = true;
	w->_dmg = 7;
	w->_baseSpread = 0.05f;
	w->_scopeSpeed = 0.07f;
	w->_movementMultiplier = 1.1f;
	w->_strafeMultiplier = 0.7f;
	w->_cooldown = 5;
	w->_ammo = 70000;
	w->_magSize = 35;
	w->_ammoMag = 35;
	w->_reloadSpeed = 0.03f;
	w->_scale = 0.05f;
	w->_xOff = 0.2f;
	w->_yOff = -0.3f;
	w->_zOff = 0.5f;
	w->_xRot = 0.0f;
	w->_yRot = 0.0f;
	w->_zRot = 0.0f;
	w->_flashColor[0] = 1.0f;
	w->_flashColor[1] = 0.75f;
	w->_flashColor[2] = 0.0f;
	w->_fireSound = S_AUDIO::getSingleton()->createSound("../Resources/Audio/Sounds/magentor.wav");
	//w->_xOff = 

	_weaponBaseList[1] = new WeaponBase(L"Conspiracy", L"../Resources/Meshes/Weapons/gun.X", L"../Resources/Weapons/Conspiracy");
	w = _weaponBaseList[1];
	w->_projectile = false;
	w->_hasDot = true;
	w->_dmg = 7;
	w->_baseSpread = 0.1f;
	w->_scopeSpeed = 0.07f;
	w->_cooldown = 5;
	w->_ammo = 70000;
	w->_magSize = 35;
	w->_ammoMag = 35;
	w->_reloadSpeed = 0.03f;
	w->_scale = 0.02f;
	w->_xOff = 0.05f;
	w->_yOff = 0.3f;
	w->_zOff = 0.6f;
	w->_xRot = 0.0f;
	w->_yRot = 3.14159f;
	w->_zRot = 0.0f;
	w->_flashColor[0] = 1.0f;
	w->_flashColor[1] = 0.25f;
	w->_flashColor[2] = 0.0f;
	w->_fireSound = S_AUDIO::getSingleton()->createSound("../Resources/Audio/Sounds/magentor.wav");

	_weaponBaseList[2] = new WeaponBase(L"Prophet", L"../Resources/Meshes/Weapons/gun.X", L"../Resources/Meshes/Weapons");
	w = _weaponBaseList[2];
	w->_projectile = false;
	w->_hasDot = true;
	w->_dmg = 31;
	w->_baseSpread = 0.07f;
	w->_maxSpread = 0.3f;
	w->_spreadIncreasePerShot = 0.021f;
	w->_spreadRecuperate = 0.003f;
	w->_scopeSpeed = 0.07f;
	w->_movementMultiplier = 0.9f;
	w->_strafeMultiplier = 0.2f;
	w->_cooldown = 4;
	w->_ammo = 70000;
	w->_magSize = 100;
	w->_ammoMag = 100;
	w->_reloadSpeed = 0.01f;
	w->_scale = 0.0025f;
	w->_xOff = 0.4f;
	w->_yOff = -0.3f;
	w->_zOff = 0.9f;
	w->_xRot = 0.0f;
	w->_yRot = 3.14159f*0.5f;
	w->_zRot = 0.0f;
	w->_flashColor[0] = 0.8f;
	w->_flashColor[1] = 1.0f;
	w->_flashColor[2] = 0.8f;
	w->_fireSound = S_AUDIO::getSingleton()->create3DSound("../Resources/Audio/Sounds/magentor.wav");
	w->_hitSound = S_AUDIO::getSingleton()->createSound("../Resources/Audio/Sounds/hitmarkersound.wav");
	w->_reloadSound = S_AUDIO::getSingleton()->createSound("../Resources/Audio/Sounds/reloadin.wav");

}

void WeaponBase::releaseAllBaseWeapons(void)
{
}
WeaponBase::WeaponBase(std::wstring name, std::wstring rvPath, std::wstring texturePath)
{
	/* Weapon Name */
	_name = name;

	/* Get Voxel Data from the raw voxel data path + CREATE MESH*/
	//ps::VoxelData* rv = new ps::VoxelData(rvPath);
	_mesh = SRender::getSingleton()->createMesh();
	//_mesh->loadMeshFromVoxelData(rv);
	_mesh->loadMeshFromX(rvPath); 

	/* Create MaterialSet from path and assign to weapon mesh */
	_materialSet = new SMaterialSet(texturePath);
	_mesh->setMaterialSet(_materialSet);

	/* SET REMAINING FIELDS TO DEFAULT */
	_flashColor[0] = 0.0f;
	_flashColor[1] = 0.0f;
	_flashColor[2] = 0.0f;
	_xOff = 0.0f;
	_yOff = 0.0f;
	_zOff = 0.0f;
	_xRot = 0.0f;
	_yRot = 0.0f;
	_zRot = 0.0f;
	_scale = 0.1f;
	_dmg = 10;

}

Weapon* WeaponBase::new_Weapon(const WeaponBase* base)
{
	Weapon* w = new Weapon();
	//gun stats
	w->aiming = 0.0f;

	w->_name = base->_name;
	w->_dmg = base->_dmg;
	w->_range = base->_range;
	w->_rangeMultiplier = base->_rangeMultiplier;
	w->_movementMultiplier = base->_movementMultiplier;
	w->_strafeMultiplier = base->_strafeMultiplier;
	w->_scopeSpeed = base->_scopeSpeed;
	w->_baseSpread = base->_baseSpread;
	w->spread = base->_baseSpread;
	w->_maxSpread = base->_maxSpread;
	w->_spreadRecuperate = base->_spreadRecuperate;
	w->_spreadIncreasePerShot = base->_spreadIncreasePerShot;

	w->_cooldown = base->_cooldown;
	w->_cooldownLeft = 0;
	w->_projectile = base->_projectile;
	w->_hasDot = base->_hasDot;

	w->_ammo = base->_ammo;
	w->_ammoMag = base->_ammoMag;
	w->_magSize = base->_magSize;
	w->_reloadSpeed = base->_reloadSpeed;
	w->_reloading = false;
	w->_reloadProgress = 0.0f;
	//setting up render shit
	w->_flashColor[0] = base->_flashColor[0];
	w->_flashColor[1] = base->_flashColor[1];
	w->_flashColor[2] = base->_flashColor[2];
	w->_xOff = base->_xOff;
	w->_yOff = base->_yOff;
	w->_zOff = base->_zOff;
	w->_xOffMod = 0.0f;
	w->_yOffMod = 0.0f;
	w->_zOffMod = 0.0f;
	w->_sprintXShake = 0.0f;
	w->_xRot = base->_xRot;
	w->_yRot = base->_yRot;
	w->_zRot = base->_zRot;
	w->_scale = base->_scale;
	w->s3d = SRender::getSingleton()->createS3D();
	w->s3d->setMesh(base->_mesh);
	w->s3d->setScale(w->_scale);
	w->_fireSound = base->_fireSound;
	w->_hitSound = base->_hitSound;
	w->_reloadSound = base->_reloadSound;
	return w;
}
WeaponBase::~WeaponBase(void)
{
}

WeaponBase* WeaponBase::getBaseWeaponByName(std::wstring name)
{
	for (int i = 0; i < _weaponBaseCount; i++)
	{
		//std::wstring::compare(
		if (name == _weaponBaseList[i]->_name)
		{
			return _weaponBaseList[i];
		}
	}

	return 0;
}

WeaponBase* WeaponBase::getBaseWeaponByIndex(int index)
{
	if (index < 0 || index >= _weaponBaseCount) return 0;

	return _weaponBaseList[index];
}