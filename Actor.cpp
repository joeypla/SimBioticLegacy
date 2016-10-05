#include "Actor.h"

using namespace ps;

SMesh* Actor::teamBlackMesh = 0;
SMesh* Actor::teamWhiteMesh = 0;
#include "SAnimatedMesh.h"
#include "GameData.h"
#include "SSetPoint.h"
Actor::Actor(bool isPlayer)
{
	state = 0;
	team = 1;
	this->isPlayer = isPlayer;
	//first setup meshes if they need to be setup
	if (teamBlackMesh == 0)
	{
		teamBlackMesh = SRender::getSingleton()->createMesh();
		teamBlackMesh->loadMeshFromX(L"../Resources/Meshes/SimSoldier/StaticSoldier.X");
		SMaterialSet* ms = SRender::getSingleton()->createMaterialSet(L"../Resources/Meshes/SimSoldier/BlackTextures");
		teamBlackMesh->setMaterialSet(ms);
	}
	if (teamWhiteMesh == 0)
	{
		teamWhiteMesh = SRender::getSingleton()->createMesh();
		teamWhiteMesh->loadMeshFromX(L"../Resources/Meshes/SimSoldier/StaticSoldier.X");
		SMaterialSet* ms = SRender::getSingleton()->createMaterialSet(L"../Resources/Meshes/SimSoldier/WhiteTextures");
		teamWhiteMesh->setMaterialSet(ms);
	}

	

	x = 32.0f;
	y = 16.0f;
	z = 32.0f;

	dx = 0.0f;
	dy = 0.0f;
	dz = 0.0f;

	ag = 0.004f; //0.004f original

	isJumping = false;
	isGrounded = false;
	isAiming = false;
	isFiring = false;
	isCrouching = false;
	isGravityLefting = false;
	isGravityRighting = false;
	isGravityForwarding = false;
	changingGravity = false;
	gravityChangeProgress = 0.0f;

	isMoving_f = false;
	isMoving_b = false;
	isMoving_r = false;
	isMoving_l = false;

	shouldActivateAbility1 = false;
	shouldActivateAbility2 = false;
	shouldActivateAbility3 = false;
	shouldActivateAbility4 = false;

	modMoving_f = 0.0f;
	modMoving_b = 0.0f;
	modMoving_l = 0.0f;
	modMoving_r = 0.0f;

	kills = 0;
	deaths = 0;
	score = 0;
	modeSpecificVal1 = 0;
	modeSpecificVal2 = 0;
	modeSpecificVal3 = 0;

	gravityField = 2;

	_xSize =  0.5f;
	_ySize = 2.75f;
	_zSize =  0.5f;

	health = 100;
	shield = 100;

	_pCharacter = SCharacter::new_instance("paragon", false);

	_weaponSelected = 1;
	_weapon1 = 0;
	_weapon2 = 0;
	

	_lastOffender = 0;
	_lastOffended = 0;
	
	_maxJogSpeed = 0.15f;
	_maxSprintSpeed = 0.2f;
	_maxStrafeSpeed = 0.08f;

	_crouching = 0.0f;
	_view = new SCamera();
	_viewPositionalOffset.x = 0.0f;
	_viewPositionalOffset.y = 1.1f;
	_viewPositionalOffset.z = 0.0f;

	_renderObj = 0;
	_renderObj = SRender::getSingleton()->createS3D();
	
		_renderObj->setMesh(teamBlackMesh);
	
	
	_renderObj->setPosition(x, y, z);
	_renderObj->setScale(0.015f);

	_name = L"None";


	leftGravitySwitchSound = S_AUDIO::getSingleton()->createSound("../Resources/Audio/Sounds/gravl.wav");
	rightGravitySwitchSound = S_AUDIO::getSingleton()->createSound("../Resources/Audio/Sounds/gravr.wav");
	jumpSound = S_AUDIO::getSingleton()->createSound("../Resources/Audio/Sounds/jump.wav");
	_timeLastUpdated = GetTickCount();

	_weapon1 = WeaponBase::new_Weapon(WeaponBase::getBaseWeaponByIndex(2));
}

Actor::~Actor(void)
{ 
}

void Actor::setName(std::wstring name)
{
	_name = name;
}
std::wstring Actor::name(void)
{
	return _name;
}
SCamera* Actor::view(void)
{
	return _view;
}

void Actor::renderSync(void)
{
	
	_xSize = 0.5f;
	_ySize = 0.5f;
	_zSize = 0.5f;
	//we also have to sync the direction
	psMath::float3 nVec( _view->dx, _view->dy, _view->dz);
	 //nVec.y = 0.0f;
	psMath::normalize(&nVec);
	_renderObj->setXRotation(0.0f);
	_renderObj->setYRotation(0.0f);
	_renderObj->setZRotation(0.0f);
	float rotation;
	if (gravityField == 0)
	{
		_xSize = ACTOR_DEFAULTHEIGHT;
		rotation = atan2(nVec.z, nVec.y); 
		_renderObj->setYRotation(rotation + D3DX_PI/2.0f);
		_renderObj->setPosition(x - 1.5f, y, z);
		
	}

	if (gravityField == 1)
	{
		_xSize = ACTOR_DEFAULTHEIGHT;
		rotation = atan2(-nVec.z, nVec.y); 
		_renderObj->setYRotation(rotation  - D3DX_PI/2.0f);
		_renderObj->setPosition(x + 1.5f, y, z);
	}
	  
	if (gravityField == 2)
	{
		_ySize = ACTOR_DEFAULTHEIGHT;
		rotation = atan2(-nVec.z, nVec.x);
		_renderObj->setYRotation(rotation - D3DX_PI/2.0f);
		_renderObj->setPosition(x, y - 1.5f, z);
	}
	if (gravityField == 3)
	{
		_ySize = ACTOR_DEFAULTHEIGHT;
		rotation = atan2(nVec.z, nVec.x);
		_renderObj->setYRotation(rotation + D3DX_PI/2.0f);
		_renderObj->setPosition(x, y + 1.5f, z);
	}
	
	if (gravityField == 4)
	{
		_zSize = ACTOR_DEFAULTHEIGHT;
		rotation = atan2(nVec.x, nVec.y);
		_renderObj->setZRotation(rotation + D3DX_PI);
		_renderObj->setPosition(x, y, z - 1.5f);
	}

	if (gravityField == 5)
	{
		_zSize = ACTOR_DEFAULTHEIGHT;
		rotation = atan2(-nVec.x, nVec.y);
		_renderObj->setZRotation(rotation - D3DX_PI);
		_renderObj->setPosition(x, y, z + 1.5f);
	}

	if (gravityField == 0) _renderObj->setZRotation(-D3DX_PI/2.0f);
	if (gravityField == 1) _renderObj->setZRotation(D3DX_PI/2.0f);
	if (gravityField == 2) _renderObj->setZRotation(0.0f);
	if (gravityField == 3) _renderObj->setZRotation(D3DX_PI);
	if (gravityField == 4) _renderObj->setXRotation(D3DX_PI/2.0f);
	if (gravityField == 5) _renderObj->setXRotation(-D3DX_PI/2.0f);
	_renderObj->setVisible(true);
	if (state == 0 || health == 0) _renderObj->setVisible(false);
	if (state == 1 && health > 0) _renderObj->setVisible(true);
	if (isPlayer) _renderObj->setVisible(false);
	//_renderObj->setVisible(true);
}
float Actor::maxJogSpeed(void){return _maxJogSpeed;}
float Actor::maxSprintSpeed(void){return _maxSprintSpeed;}
float Actor::maxStrafeSpeed(void){return _maxStrafeSpeed;}

//only used as a quickset, therefore no interpolatiom (not smooth, literally clicks to it)
void Actor::setToGravityConfig(int gravityConfig)
{
	isCrouching = false;
	gravityField = gravityConfig;

	psMath::float3 camDir;
	psMath::float3 camStrafe;
	psMath::float3 camUp;
	switch (gravityConfig)
	{
	case 0:
		_view->setUpVector(1.0f, 0.0f, 0.0f);
		_ySize = ACTOR_DEFAULTWIDTH;
		_zSize = ACTOR_DEFAULTWIDTH;
		_xSize = ACTOR_DEFAULTHEIGHT;
		break;
	case 1:
		camDir = _view->getDirectionX();
		camStrafe = psMath::cross(&camDir, &psMath::float3(1.0f, 0.0f, 0.0f));
		camUp = psMath::cross(&camDir, &camStrafe);
		_view->setUpVector(camUp.x, camUp.y, camUp.z);
		
		_ySize = ACTOR_DEFAULTWIDTH;
		_zSize = ACTOR_DEFAULTWIDTH;
		_xSize = ACTOR_DEFAULTHEIGHT;
		break;
	case 2:
		_view->setUpVector(0.0f, 1.0f, 0.0f);
		_xSize = ACTOR_DEFAULTWIDTH;
		_zSize = ACTOR_DEFAULTWIDTH;
		_ySize = ACTOR_DEFAULTHEIGHT;
		break;
	case 3:
		_view->setUpVector(0.0f, -1.0f, 0.0f);
		_xSize = ACTOR_DEFAULTWIDTH;
		_zSize = ACTOR_DEFAULTWIDTH;
		_ySize = ACTOR_DEFAULTHEIGHT;
		break;
	case 4:
		_view->setUpVector(0.0f, 0.0f, 1.0f);
		_xSize = ACTOR_DEFAULTWIDTH;
		_ySize = ACTOR_DEFAULTWIDTH;
		_zSize = ACTOR_DEFAULTHEIGHT;
		break;
	case 5:
		_view->setUpVector(0.0f, 0.0f, -1.0f);
		_xSize = ACTOR_DEFAULTWIDTH;
		_ySize = ACTOR_DEFAULTWIDTH;
		_zSize = ACTOR_DEFAULTHEIGHT;
		break;
	default:
		break;
	}
}

#include "psMath.h"
void ps::Actor::spawn(ps::GameData* pGameData, ps::SetPoint* sp)
{
	setToGravityConfig(sp->gravityConfig);
	
	//now project ray down from spawn to hit floor, and place
	//actor directly on floor. Trust me, its important. Either he'll
	//be in the air, or be set directly in the floor, and thats not good.
	psMath::float3 rayOrigin;
	rayOrigin.x = sp->x;
	rayOrigin.y = sp->y;
	rayOrigin.z = sp->z;

	psMath::float3 rayDir;
	rayDir.x = 0.0f;
	rayDir.y = 0.0f;
	rayDir.z = 0.0f;

	switch (gravityField)
	{
	case 0:
		rayDir.x = -0.1f;
		break;
	case 1:
		rayDir.x = 0.1f;
		break;
	case 2:
		rayDir.y = -0.1f;
		break;
	case 3:
		rayDir.y = 0.1f;
		break;
	case 4:
		rayDir.z = -0.1f;
		break;
	case 5:
		rayDir.z = 0.1f;
		break;
	default:
		break;
	}

	//given ray, trace until hitting a block and get that position
	//not recursive, a max of a certain amount. Just don't be stupid and place the spawns
	//50 feet in the air. Its not that fucking hard.
	ps::VoxelData* worldVoxelData = pGameData->pWorldData()->pVoxelData();
	int mx = worldVoxelData->x();
	int my = worldVoxelData->y();
	int mz = worldVoxelData->z();
	for (int i = 0; i < 100; i++)
	{
		rayOrigin += rayDir;
		if ((int)rayOrigin.x < 0 || (int)rayOrigin.x >= mx) break;
		if ((int)rayOrigin.y < 0 || (int)rayOrigin.y >= my) break;
		if ((int)rayOrigin.z < 0 || (int)rayOrigin.z >= mz) break;

		if (worldVoxelData->data((int)rayOrigin.x, (int)rayOrigin.y, (int)rayOrigin.z) != 0)
		{
			break;
		}

	}

	psMath::float3 finalPosition;
	finalPosition = rayOrigin;

	//Now add height depending on default actor height as well as
	//the gravity config.

	switch (gravityField)
	{
	case 0:
		finalPosition.x += ACTOR_DEFAULTHEIGHT * 0.5f;
		finalPosition.y = sp->y;
		finalPosition.z = sp->z;
		break;
	case 1:
		finalPosition.x -= ACTOR_DEFAULTHEIGHT * 0.5f;
		finalPosition.y = sp->y;
		finalPosition.z = sp->z;
		break;
	case 2:
		finalPosition.x = sp->x;
		finalPosition.y += ACTOR_DEFAULTHEIGHT * 0.5f;
		finalPosition.z = sp->z;
		break;
	case 3:
		finalPosition.x = sp->x;
		finalPosition.y -= ACTOR_DEFAULTHEIGHT * 0.5f;
		finalPosition.z = sp->z;
		break;
	case 4:
		finalPosition.x = sp->x;
		finalPosition.y = sp->y;
		finalPosition.z += ACTOR_DEFAULTHEIGHT * 0.5f;
		break;
	case 5:
		finalPosition.x = sp->x;
		finalPosition.y = sp->y;
		finalPosition.z -= ACTOR_DEFAULTHEIGHT * 0.5f;
		break;
	default:
		break;
	}

	//Now set the position
	x = finalPosition.x;
	y = finalPosition.y;
	z = finalPosition.z;

	dx = 0.0f;
	dy = 0.0f;
	dz = 0.0f;
	isGrounded = false;
	isAiming = false;
	isCrouching = false;
	//Now set up weapons shit
	_weaponSelected = 1;
	_weapon1 = WeaponBase::new_Weapon(WeaponBase::getBaseWeaponByIndex(2));
	renderSync();
}

int ps::Actor::weaponSelected(void)
{
	return _weaponSelected;
}

ps::Weapon* ps::Actor::weapon1(void)
{
	return _weapon1;
}

ps::Weapon* ps::Actor::weapon2(void)
{
	return _weapon2;
}

float ps::Actor::xSize(void){return _xSize;}
float ps::Actor::ySize(void){return _ySize;}
float ps::Actor::zSize(void){return _zSize;}

void ps::Actor::doDamage(Actor* offender, int damage)
{
	_lastOffender = offender;

	shield -= damage;
	int remainingDamage = 0;
	if (shield < 0)
	{
		remainingDamage = -shield;
	}

	health -= remainingDamage;
	if (health <= 0)
	{
		health = 0;
		_renderObj->setMesh(0);
		deaths += 1;
		if (offender != 0)
		{
			offender->kills += 1;
			offender->score += 100;
		}
	}
}

void ps::Actor::update(void)
{
	DWORD currentTime = GetTickCount();
	DWORD timeSinceLastUpdate = currentTime - _timeLastUpdated;

	float timeFactor = 1.0f;//(float)timeSinceLastUpdate / (1.0f/60.0f*1000.0f);
	//reload automatically if no bullets
	if (_weapon1->ammoMag() < 1) _weapon1->reload(this);

	//update gravity switching
	if (isGravityLefting)
	{
		if (!changingGravity)
		{
			S_AUDIO::getSingleton()->playSoundOnce(leftGravitySwitchSound);
			int targetGravity = psMath::gravityConfigNumberFromVector(&_view->x_getStrafeVector());
			gravityChangeProgress = 0.0f;
			changingGravity = true;
			gravityChangeType = 0;
			gravityField = targetGravity;
		}
	}
	if (isGravityRighting)
	{
		if (!changingGravity)
		{
			S_AUDIO::getSingleton()->playSoundOnce(rightGravitySwitchSound);
			int targetGravity = psMath::gravityConfigNumberFromVector(&(-_view->x_getStrafeVector()));
			changingGravity = true;
			gravityChangeProgress = 0.0f;
			gravityChangeType = 1;
			gravityField = targetGravity;
		}
	}
	if (isGravityForwarding)
	{
		if (!changingGravity)
		{
			int targetGravity = psMath::gravityConfigNumberFromVector(&(_view->getDirectionX()));
			//changingGravity = true;
			gravityChangeProgress = 0.0f;
			gravityChangeType = 2;
			gravityField = targetGravity;
		}
	}
	if (changingGravity)
	{
		if (gravityChangeType == 0)
		{
			_view->rotateView(0.0f, 0.0f, -3.1415926f/20.0f, gravityField);//no up rotation so dont worry about last argument

			float checkedValue = 0.0f;
			if (gravityField == 0 || gravityField == 1) checkedValue = abs(_view->x_getStrafeVector().x);
			if (gravityField == 2 || gravityField == 3) checkedValue = abs(_view->x_getStrafeVector().y);
			if (gravityField == 4 || gravityField == 5) checkedValue = abs(_view->x_getStrafeVector().z);
			if (checkedValue < 0.08f)
			{
				psMath::float3 planarStrafe = _view->x_getStrafeVector();
				psMath::float3 strafe = _view->x_getStrafeVector();
				if (gravityField == 0 || gravityField == 1) planarStrafe.x = 0.0f;
				if (gravityField == 2 || gravityField == 3) planarStrafe.y = 0.0f;
				if (gravityField == 4 || gravityField == 5) planarStrafe.z = 0.0f;
				
				psMath::normalize(&planarStrafe);

				_view->rotateView(0.0f, 0.0f, acos(psMath::dot(&planarStrafe, &strafe)), gravityField);
				float newCheckedValue = 0.0f;
				if (gravityField == 0 || gravityField == 1) newCheckedValue = abs(_view->x_getStrafeVector().x);
				if (gravityField == 2 || gravityField == 3) newCheckedValue = abs(_view->x_getStrafeVector().y);
				if (gravityField == 4 || gravityField == 5) newCheckedValue = abs(_view->x_getStrafeVector().z);
				if (newCheckedValue > checkedValue)
				{
					_view->rotateView(0.0f, 0.0f, -2.0f*acos(psMath::dot(&planarStrafe, &strafe)), gravityField);
				}
				changingGravity = false;
			}
		}
		if (gravityChangeType == 1)
		{
			_view->rotateView(0.0f, 0.0f, 3.1415926f/20.0f, gravityField);//no up rotation so dont worry about last argument

			float checkedValue = 0.0f;
			if (gravityField == 0 || gravityField == 1) checkedValue = abs(_view->x_getStrafeVector().x);
			if (gravityField == 2 || gravityField == 3) checkedValue = abs(_view->x_getStrafeVector().y);
			if (gravityField == 4 || gravityField == 5) checkedValue = abs(_view->x_getStrafeVector().z);
			if (checkedValue < 0.08f)
			{
				psMath::float3 planarStrafe = _view->x_getStrafeVector();
				psMath::float3 strafe = _view->x_getStrafeVector();
				if (gravityField == 0 || gravityField == 1) planarStrafe.x = 0.0f;
				if (gravityField == 2 || gravityField == 3) planarStrafe.y = 0.0f;
				if (gravityField == 4 || gravityField == 5) planarStrafe.z = 0.0f;
				
				psMath::normalize(&planarStrafe);

				_view->rotateView(0.0f, 0.0f, -acos(psMath::dot(&planarStrafe, &strafe)), gravityField);
				float newCheckedValue = 0.0f;
				if (gravityField == 0 || gravityField == 1) newCheckedValue = abs(_view->x_getStrafeVector().x);
				if (gravityField == 2 || gravityField == 3) newCheckedValue = abs(_view->x_getStrafeVector().y);
				if (gravityField == 4 || gravityField == 5) newCheckedValue = abs(_view->x_getStrafeVector().z);
				if (newCheckedValue > checkedValue)
				{
					_view->rotateView(0.0f, 0.0f, 2.0f*acos(psMath::dot(&planarStrafe, &strafe)), gravityField);
				}
				changingGravity = false;
			}
		}
		if (gravityChangeType == 2)
		{

		}

		
	}
	//update jumping
	if (isGrounded)
	{
		if (isJumping)
		{
			S_AUDIO::getSingleton()->playSoundOnce(jumpSound);
			switch (gravityField)
			{
			case 0: dx = 0.1f; break;
			case 1: dx = -0.1f; break;
			case 2: dy = 0.1f; break;
			case 3: dy = -0.1f; break;
			case 4: dz = 0.1f; break;
			case 5: dz = -0.1f; break;
			default: break;
			}
			isGrounded = false;
		}
	}
	//update the view positional offset... maybe change to render sync after??? we might run into problems of the camera snapping
	//might have to interpolate later on.
	_viewPositionalOffset.x = 0.0f; _viewPositionalOffset.y = 0.0f; _viewPositionalOffset.z = 0.0f;
	switch (gravityField)
	{
	case 0: _viewPositionalOffset.x = ACTOR_DEFAULTVIEWOFFSET - _crouching*ACTOR_DEFAULTVIEWOFFSETCROUCHMULTIPLIER; break;
	case 1: _viewPositionalOffset.x = -ACTOR_DEFAULTVIEWOFFSET + _crouching*ACTOR_DEFAULTVIEWOFFSETCROUCHMULTIPLIER; break;
	case 2: _viewPositionalOffset.y = ACTOR_DEFAULTVIEWOFFSET - _crouching*ACTOR_DEFAULTVIEWOFFSETCROUCHMULTIPLIER; break;
	case 3: _viewPositionalOffset.y = -ACTOR_DEFAULTVIEWOFFSET + _crouching*ACTOR_DEFAULTVIEWOFFSETCROUCHMULTIPLIER; break;
	case 4: _viewPositionalOffset.z = ACTOR_DEFAULTVIEWOFFSET - _crouching*ACTOR_DEFAULTVIEWOFFSETCROUCHMULTIPLIER; break;
	case 5: _viewPositionalOffset.z = -ACTOR_DEFAULTVIEWOFFSET + _crouching*ACTOR_DEFAULTVIEWOFFSETCROUCHMULTIPLIER; break;
	default:
		break;
	}
	_view->setPosition(x + _viewPositionalOffset.x, y + _viewPositionalOffset.y, z + _viewPositionalOffset.z);

	//update the size of the collision box... i know its inefficient to check gravity config twice, but its not a huge deal and is more readable right now
	switch (gravityField)
	{
	case 0: _xSize = ACTOR_DEFAULTHEIGHT; _ySize = ACTOR_DEFAULTWIDTH; _zSize = ACTOR_DEFAULTWIDTH; break;
	case 1: _xSize = ACTOR_DEFAULTHEIGHT; _ySize = ACTOR_DEFAULTWIDTH; _zSize = ACTOR_DEFAULTWIDTH; break;
	case 2: _xSize = ACTOR_DEFAULTWIDTH; _ySize = ACTOR_DEFAULTHEIGHT; _zSize = ACTOR_DEFAULTWIDTH; break;
	case 3: _xSize = ACTOR_DEFAULTWIDTH; _ySize = ACTOR_DEFAULTHEIGHT; _zSize = ACTOR_DEFAULTWIDTH; break;
	case 4: _xSize = ACTOR_DEFAULTWIDTH; _ySize = ACTOR_DEFAULTWIDTH; _zSize = ACTOR_DEFAULTHEIGHT; break;
	case 5: _xSize = ACTOR_DEFAULTWIDTH; _ySize = ACTOR_DEFAULTWIDTH; _zSize = ACTOR_DEFAULTHEIGHT; break;
	default:
		break;
	}


	psMath::float3 dir;
	psMath::float3 strafe;
	dir = _view->getDirectionX();
	strafe = _view->x_getStrafeVector();

	switch (gravityField)
	{
	case 0: dir.x = 0.0f; break;
	case 1: dir.x = 0.0f; break;
	case 2: dir.y = 0.0f; break;
	case 3: dir.y = 0.0f; break;
	case 4: dir.z = 0.0f; break;
	case 5: dir.z = 0.0f; break;
	default:
		break;
	}
	psMath::normalize(&dir);
	//sprinting
	if (modMoving_f > 0.3f)
	{
		if (isSprinting && !isAiming && isGrounded)
		{
			sprint += 0.1f*timeFactor;
			if (sprint > 1.0f) sprint = 1.0f;
		}
	}
	if (!isSprinting)
	{
		sprint -= 0.1f*timeFactor;
		if (sprint < 0.0f) sprint = 0.0f;

	}
	//moving forwards
	
	float moveFactor = 0.1f;
	float sprintMultiplier = 1.2f;
	float mobility = 1.0f;
	if (_weapon1!=0)
	{
		mobility = _weapon1->getCurrentMobilityFactor();
	}
	
	 if (isMoving_f)
	 {
		 
		 if (modMoving_f > 1.0f)
		 {
			modMoving_f = 1.0f;
		 } else
		 {
			 modMoving_f += 0.1f*timeFactor;
		 }
	 }
	 else 
	 {
		 
		 if (modMoving_f < 0.0f)
		 {
			 modMoving_f = 0.0f;
		 }else
		 {
			 modMoving_f -= 0.1f*timeFactor;
			 
		 }
	 } 
	 x += dir.x*(moveFactor+(sprint*sprintMultiplier*moveFactor))*modMoving_f*mobility*timeFactor;
	 y += dir.y*(moveFactor+(sprint*sprintMultiplier*moveFactor))*modMoving_f*mobility*timeFactor;
	 z += dir.z*(moveFactor+(sprint*sprintMultiplier*moveFactor))*modMoving_f*mobility*timeFactor;
	 //moving backwards
	 if (isMoving_b)
	 {
		 
		 if (modMoving_b > 1.0f)
		 {
				 modMoving_b = 1.0f;
		 } else
		 {
			 modMoving_b += 0.1f*timeFactor;
			 
		 }
	 }
	 else 
	 {
		 
		 if (modMoving_b < 0.0f)
		 {
			 modMoving_b = 0.0f;
		 }else
		 {
			 modMoving_b -= 0.1f*timeFactor;
			 
		 }
	 }
	 x -= dir.x*moveFactor*modMoving_b*mobility*timeFactor;
	 y -= dir.y*moveFactor*modMoving_b*mobility*timeFactor;
	 z -= dir.z*moveFactor*modMoving_b*mobility*timeFactor;
	 //moving left
	 if (isMoving_l)
	 {
		
		 if (modMoving_l > 1.0f)
		 {
			 modMoving_l = 1.0f;
		 } else 
		 {
			 modMoving_l += 0.1f*timeFactor;
			 
		 }
	 }
	 else 
	 {
		 
		 if (modMoving_l < 0.0f)
		 {
				 modMoving_l = 0.0f;
		 }else 
		 {
			 modMoving_l -= 0.1f*timeFactor;
			 
		 }
	 }
	 x += strafe.x*moveFactor*modMoving_l*mobility*timeFactor;
	 y += strafe.y*moveFactor*modMoving_l*mobility*timeFactor;
	 z += strafe.z*moveFactor*modMoving_l*mobility*timeFactor;
	 //moving right
	 if (isMoving_r)
	 {
		
		 if (modMoving_r > 1.0f)
		 {
			 modMoving_r = 1.0f;
		 }else
		 {
			 modMoving_r += 0.1f*timeFactor;
		 }
	 }
	 else 
	 {
		 
		 if (modMoving_r < 0.0f)
		 {
			 modMoving_r = 0.0f;
		 }else 
		 {
			 modMoving_r -= 0.1f*timeFactor;
		 }
	 }
	 x -= strafe.x*moveFactor*modMoving_r*mobility*timeFactor;
	 y -= strafe.y*moveFactor*modMoving_r*mobility*timeFactor;
	 z -= strafe.z*moveFactor*modMoving_r*mobility*timeFactor;

	 
	 //update crouching mechanism
	 if (isCrouching)
	 {
		 _crouching += 0.1f*timeFactor;
		 if (_crouching > 1.0f) _crouching = 1.0f;
		 
	 } else 
	 {
		 _crouching -= 0.1f*timeFactor;
		 if (_crouching < 0.0f) _crouching = 0.0f;
	 }
	 

	 //update rolling
	 if (isRolling)
	 {
	 }


	 _timeLastUpdated = GetTickCount();


}
#include "GameData.h"
void ps::Actor::updateAbilities(ps::GameData* pGameData)
{
	if (_pCharacter == NULL) return;

	if (shouldActivateAbility1)
	{
		shouldActivateAbility1 = false;
		SAbility* ability = _pCharacter->getAbility(0);
		
		if (ability != NULL)
		{
			S_AUDIO::getSingleton()->playSoundOnceAtPosition(ability->getCastSound(), psMath::float3(x, y, z));
			if (ability->getFlux(0) <= flux)
			{
				ability->clientCast(pGameData, this);
			}
		}
	}

	if (shouldActivateAbility2)
	{
		shouldActivateAbility2 = false;
		SAbility* ability = _pCharacter->getAbility(1);
		
		if (ability != NULL)
		{
			S_AUDIO::getSingleton()->playSoundOnceAtPosition(ability->getCastSound(), psMath::float3(x, y, z));
			if (ability->getFlux(0) <= flux)
			{
				ability->clientCast(pGameData, this);
			}
		}
	}

	if (shouldActivateAbility3)
	{
		shouldActivateAbility3 = false;
		SAbility* ability = _pCharacter->getAbility(2);
		
		if (ability != NULL)
		{
			S_AUDIO::getSingleton()->playSoundOnceAtPosition(ability->getCastSound(), psMath::float3(x, y, z));
			if (ability->getFlux(0) <= flux)
			{
				ability->clientCast(pGameData, this);
			}
		}
	}

	if (shouldActivateAbility4)
	{
		shouldActivateAbility4 = false;
		SAbility* ability = _pCharacter->getAbility(3);
		
		if (ability != NULL)
		{
			S_AUDIO::getSingleton()->playSoundOnceAtPosition(ability->getCastSound(), psMath::float3(x, y, z));
			if (ability->getFlux(0) <= flux)
			{
				ability->clientCast(pGameData, this);
			}
		}
	}
}
void ps::Actor::startRoll(void)
{
}

SCharacter* ps::Actor::getCharacter(void)
{
	return _pCharacter;
}

void ps::Actor::setWallhacked(bool b)
{
	_renderObj->setWallhacked(b, psMath::float3(1.0f, 0.0f, 0.0f));
}