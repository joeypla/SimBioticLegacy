#include "Weapon.h"

#include "Actor.h"
#include "GameData.h"

#include "psMath.h"
ps::Weapon::Weapon(void){}
void ps::Weapon::updateAnimation(ps::Actor* owner)
{
	s3d->setReference(owner->view()->x_getInverseViewMat());
	spread -= _spreadRecuperate;
	if (spread < _baseSpread) spread = _baseSpread;

	static float breathing = 0.0f;
	//static float aiming = 0.0f;
	if (owner->isAiming && owner->isGrounded && !_reloading && owner->sprint < 0.1f )
	{
		aiming += _scopeSpeed;
		if (aiming >= 1.0f)
		{
			aiming = 1.0f; 
			
		} 
		
	} 
	else 
	{
		aiming -= _scopeSpeed;
		if (aiming < 0.0f) aiming = 0.0f;
	}
	float temp = 0.0f;
	
	breathing+=0.02f;
	s3d->setPosition(
		_xOff - (_xOff*aiming*aiming  ) + _xOffMod - owner->sprint*0.05f*sin(_sprintXShake), 
		_yOff + 0.01*sin(breathing) + _yOffMod - 0.5*sin(3.14159f*_reloadProgress) - owner->sprint*0.1f - owner->sprint*0.05f*sin(_sprintXShake+0.78f), 
		_zOff + _zOffMod - 0.5*sin(3.14159f*_reloadProgress));
	s3d->setXRotation(_xRot);
	s3d->setYRotation(_yRot -owner->sprint*1.5f);
	s3d->setZRotation(_zRot);
	if (owner->sprint > 0.5f) _sprintXShake += 0.2f;
	/*static float lastDeltaX = 0.0f;
	static float curDeltaX = 0.0f;
	lastDeltaX = curDeltaX;
	curDeltaX = S_INPUT::getMouseDeltaX();
	float avgDeltaX = (curDeltaX + lastDeltaX) * 0.5f;
	a
	static float lastDeltaY = 0.0f;
	static float curDeltaY = 0.0f;
	lastDeltaY = curDeltaY;
	curDeltaY = S_INPUT::getMouseDeltaY();
	float avgDeltaY = (curDeltaY + lastDeltaY) * 0.5f;
	stairs->setYRotation(0.01f*avgDeltaX);
	stairs->setXRotation(0.01f*avgDeltaY);*/

	//update cooldown
	_cooldownLeft -= 1;
	if (_cooldownLeft < 0) _cooldownLeft = 0;

	//update recoil
	_zOffMod += 0.05f;
	if (_zOffMod > 0.0f) _zOffMod = 0.0f;
	if (_zOffMod < -0.2f) _zOffMod = -0.2f;

	//update zoom
	owner->view()->fov = 0.8f - aiming*0.2f;

	//update reloading (if reloading)
	if (_reloading)
	{
		_reloadProgress += _reloadSpeed;
		if (_reloadProgress > 1.0f)
		{
			_reloadProgress = 0.0f;
			_reloading = false;
			_ammoMag = _magSize;
			_ammo -= _magSize;
		}
	}
}

#include "psPhysics.h"
#include "psHUD.h"
#include "psGameClient.h"
#include "SProjectile.h"

#include "SLOG.h"
void ps::Weapon::fire(ps::Actor* owner, ps::GameData* pGameData)
{
	

	if (_cooldownLeft != 0) return;
	if (_ammoMag < 1) return;
	if (_reloading) return;
	if (owner->sprint > 0.1f) return;
	_ammoMag -= 1;
	//only fire if the weapon is ready

	spread += _spreadIncreasePerShot;
	if (spread > _maxSpread) spread = _maxSpread;

	psMath::float3 muzzleFlashLocation;
	muzzleFlashLocation.x = s3d->getPosition().x + owner->view()->dx*1.1f;
	muzzleFlashLocation.y = s3d->getPosition().y + owner->view()->dy*1.1f;
	muzzleFlashLocation.z = s3d->getPosition().z + owner->view()->dz*1.1f;
	SRender::getSingleton()->createLightEffect(muzzleFlashLocation.x, muzzleFlashLocation.y, muzzleFlashLocation.z, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, _flashColor[0], _flashColor[1], _flashColor[2]);
	S_AUDIO::getSingleton()->playSoundOnceAtPosition(_fireSound, psMath::float3(owner->x, owner->y, owner->z));

	for (int i = 0; i < 20; i++)
	{
	SParticle particle;
						particle.position.x = muzzleFlashLocation.x + ((rand() % 100) - 50) * 0.002f;;
						particle.position.y = muzzleFlashLocation.y + ((rand() % 100) - 50) * 0.002f;;
						particle.position.z = muzzleFlashLocation.z + ((rand() % 100) - 50) * 0.002f;
						particle.velocity.x = 0.0f;
						particle.velocity.y = 0.0f;
						particle.velocity.z = 0.0f;
						particle.life = 1.0f;
						particle.dLife = 0.1f;
						particle.acceleration.x = 0.0f;
						particle.acceleration.y = 0.0f;
						particle.acceleration.z = 0.0f;
						particle.scale = 0.3f;
						particle.dScale = 0.0f;
						particle.color[0] = _flashColor[0];
						particle.color[1] = _flashColor[1];
						particle.color[2] = _flashColor[2];
						particle.color[3] = 1.0f;
						particle.dColor[0] = 0.0f;
						particle.dColor[1] = 0.0f;
						particle.dColor[2] = 0.0f;
						particle.dColor[3] = -0.1f;
					SRender::getSingleton()->getParticleSystem()->emitParticle(&particle);
	}
	//after all this, reset the cooldown left
	_cooldownLeft = _cooldown;
	float xRecoil = rand() % 100;
	xRecoil -= 50;
	xRecoil /= 10000;
	float yRecoil = rand() % 50;
	//yRecoil -= 50;
	yRecoil /= 10000;
	if (aiming > 0.9f)
	owner->view()->rotateView(yRecoil, xRecoil, 0.0f, owner->gravityField);
	_zOffMod -= 0.4f;

	//find screen coordinate at which to fire from
	float x, y;
	if (aiming > 0.9f)
	{
		x = 0.0f; 
		y = 0.0f;
	} else 
	{
		//rand() spread
		int spreadMax = (int)(spread * 1000);//100
		int spreadMaxDiv2 = spreadMax /2;//50
		int spreadRandomX = rand() % spreadMax - spreadMaxDiv2;
		int spreadRandomY = rand() % spreadMax - spreadMaxDiv2;
		x = (float)spreadRandomX * 0.001f;
		y = (float)spreadRandomY * 0.001f;
	}
	//find ray at which to follow
	int width, height;
	width = SRender::getSingleton()->getWidth();
	height = SRender::getSingleton()->getHeight();
	psMath::Ray ray = psMath::rayFromScreenSpaceCoordinate(&SRender::getSingleton()->getProjection(), owner->view()->x_getMat(), x, y, width, height);
	//do a tracer object test, delete right after
	static int tracerID = 0;
	SProjectile* tracer = new SProjectile();
	tracer->position = psMath::float3(owner->view()->x, owner->view()->y, owner->view()->z);
	tracer->velocity = ray.direction*1.0f;
	
	//tracer->velocity.y += 0.2f;
	tracer->acceleration = psMath::float3(0.0f, 0.0f, 0.0f);
	int sizeBefore = pGameData->getObjectData().size();
	//pGameData->getObjectData().insert(std::pair<int, SGameObject*>(tracerID, tracer));
	int sizeAfter = pGameData->getObjectData().size();
	if (sizeAfter == sizeBefore) S_LOG::write("SIZE DIDNT CHANGE");

	tracerID++;
	LightEffectData ld;
				ld.x = owner->x;
				ld.y = owner->y;
				ld.z = owner->z;
				ld.dx = ray.direction.x*1.0f; ld.dy = ray.direction.y*1.0f; ld.dz = ray.direction.z*1.0f;
				ld.ax = 0.0f; ld.ay = 0.0f; ld.az = 0.0f;
				ld.lifetime = 4.5f;
				ld.color[0] = 0.5f; ld.color[1] = 0.6f; ld.color[2] = 1.0f;
				ld.dColor[0] = 0.0f; ld.dColor[1] = 0.0f; ld.dColor[2] = 0.0f;
				ld.radius = 8.0f;
				//SRender::getSingleton()->createLightEffect(&ld);

	//get map voxel data
	ps::VoxelData* vd = pGameData->pWorldData()->pVoxelData();
	int actorCount = pGameData->actorCount();
	int damage = _dmg;
	if (!_projectile) 
	{
		for (int i = 0; i < 1500; i++)
		{
			ray.origin += ray.direction*0.1f;
			if ((int)ray.origin.x >= vd->x() || (int)ray.origin.y >= vd->y() || (int)ray.origin.z >= vd->z()) break;
			if ((int)ray.origin.x < 0 || (int)ray.origin.y < 0 || (int)ray.origin.z < 0) break;
			if (vd->data((int)ray.origin.x, (int)ray.origin.y, (int)ray.origin.z))
			{
				for (int i = 0; i < 20; i++)
				{
					SParticle particle;
						particle.position.x = ray.origin.x;
						particle.position.y = ray.origin.y;
						particle.position.z = ray.origin.z;
						particle.velocity.x = ((rand() % 100) - 50) * 0.001f;
						particle.velocity.y = ((rand() % 100) - 50) * 0.001f;
						particle.velocity.z = ((rand() % 100) - 50) * 0.001f;
						particle.life = 10.0f;
						particle.dLife = 0.2f;
						particle.acceleration.x = 0.0f;
						particle.acceleration.y = 0.0f;
						particle.acceleration.z = 0.0f;
						particle.scale = 0.02f;
						particle.dScale = 0.0f;
						particle.color[0] = _flashColor[0];
						particle.color[1] = _flashColor[1];
						particle.color[2] = _flashColor[2];
						particle.color[3] = 1.0f;
						
						particle.dColor[0] = 0.0f;
						particle.dColor[1] = 0.0f;
						particle.dColor[2] = 0.0f;
						particle.dColor[3] = 0.0f;
					SRender::getSingleton()->getParticleSystem()->emitGlowParticle(&particle);
				}
				LightEffectData ld;
				ld.x = ray.origin.x - ray.direction.x*0.5f;
				ld.y = ray.origin.y - ray.direction.y*0.5f;
				ld.z = ray.origin.z - ray.direction.z*0.5f;
				ld.dx = 0.0f; ld.dy = 0.0f; ld.dz = 0.0f;
				ld.ax = 0.0f; ld.ay = 0.0f; ld.az = 0.0f;
				ld.lifetime = 0.2f;
				ld.color[0] = 1.0f; ld.color[1] = 1.0f; ld.color[2] = 1.0f;
				ld.dColor[0] = -0.05f; ld.dColor[1] = -0.05f; ld.dColor[2] = -0.05f;
				ld.radius = 2.0f;
				SRender::getSingleton()->createLightEffect(&ld);
				break;
			}
			//check for ray hit with actors... very approximate and NOT PERMANENT, just so we can implement other shit in the game
			for (int j = 0; j < actorCount; j++)
			{
				ps::Actor* actor = pGameData->pActorAtIndex(j);
				if (actor == owner) continue;
				if (psPhysics::isPointWithinBox(psMath::float3(ray.origin.x, ray.origin.y, ray.origin.z), psMath::float3(actor->x, actor->y, actor->z), psMath::float3(actor->xSize(), actor->ySize(), actor->zSize())))
				{
				 
					SGameClient::getSingleton()->sendImmediateShotPacket(SGameClient::getSingleton()->getClientIndex(), j, _dmg);
					
					psHUD::resetHitmarker();
					psHUD::showLastEnemy(actor); 
					S_AUDIO::getSingleton()->playSoundOnce(_hitSound);
					for (int i = 0; i < 20; i++)
					{
						SParticle particle;
						particle.position.x = ray.origin.x;
						particle.position.y = ray.origin.y;
						particle.position.z = ray.origin.z;
						particle.velocity.x = ((rand() % 100) - 50) * 0.0001f;
						particle.velocity.y = 0.001f;//((rand() % 100) - 50) * 0.001f;
						particle.velocity.z = ((rand() % 100) - 50) * 0.0001f;
						particle.life = 10.0f;
						particle.dLife = 0.1f;
						particle.acceleration.x = 0.0f;
						particle.acceleration.y = 0.0f;
						particle.acceleration.z = 0.0f;
						particle.scale = 1.0f;

						//SRender::getSingleton()->emitParticle(&particle);
					}
					return; 
				}
			}
		}
	}


}

bool ps::Weapon::hasDot(void){return _hasDot;}

int ps::Weapon::ammo(void){return _ammo;}
int ps::Weapon::ammoMag(void){return _ammoMag;}

float ps::Weapon::movementMultiplier(void)
{
	return _movementMultiplier;
}

float ps::Weapon::strafeMultiplier(void)
{
	return _strafeMultiplier;
}
const wchar_t* ps::Weapon::name(void)
{
	return _name.c_str();
}

bool ps::Weapon::reload(ps::Actor* owner)
{
	
	if (_reloading) return false;
	S_AUDIO::getSingleton()->playSoundOnce(_reloadSound);
	//returns true if reload is ok, and false if it fails. pretty self explanatory.
	if (_ammoMag < _magSize)
	{
		if (_magSize - _ammoMag <= _ammo)
		{
			_reloading = true;
			return true;
		}
	}
	return false;
}

float ps::Weapon::getCurrentMobilityFactor(void)
{
	float mobility = _movementMultiplier;
	if (aiming > 0.5f) mobility *= _strafeMultiplier;
	return mobility;
}