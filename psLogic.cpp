#include "psLogic.h"

#include "Actor.h"

//some functions that kind of don't know where to go yet :S
static void updateGunAnimations(ps::GameData* pGameData);

#include "SLOG.h"
void psLogic::update(ps::GameData* gameData)
{
	std::map<unsigned int, SGameObject*>& objectData = gameData->getObjectData();
	std::vector<std::map<unsigned int, SGameObject*>::iterator> membersToDelete;
	for (std::map<unsigned int, SGameObject*>::iterator it = objectData.begin(); it != objectData.end(); it++)
	{
		SGameObject* gameObject = it->second;
		gameObject->clientUpdate(gameData);
		if (gameObject->position.x > 100.0f || gameObject->position.x < -32.0f ||
			gameObject->position.y > 100.0f || gameObject->position.y < -32.0f ||
			gameObject->position.z > 100.0f || gameObject->position.z < -32.0f)
		{
			membersToDelete.push_back(it);
		}

		if (gameObject->getIsDead())
			membersToDelete.push_back(it);
	}

	for (int i = 0; i < membersToDelete.size(); i++)
	{
		SGameObject* gameObject = membersToDelete[i]->second;
		objectData.erase(membersToDelete[i]);
		delete gameObject;
	}



	//Some things need to be updated for all actors (which are those acting for other clients)
	//while some only need to be updated for the client.
	using namespace ps;
	//make actor views locked to actor
	for (int i = 0; i < gameData->actorCount(); i++)
	{
		Actor* actor = gameData->pActorAtIndex(i);
		if (actor->state == 0) continue;
		//fire their guns IF NEED BE
		
		
		
		

		
		if (actor == gameData->pClientActor()) actor->update();

		if (actor == gameData->pClientActor()) actor->updateAbilities(gameData);
		//Lock Gun to player
		actor->renderSync();


		//Perform the player on this client stuff only
		//if (actor == gameData->pClientActor())
		//{
			if (actor->state != 0 && actor->isFiring)
			{
				ps::Weapon* weaponToFire = 0;
				if (actor->weaponSelected() == 1) weaponToFire = actor->weapon1();
				if (actor->weaponSelected() == 2) weaponToFire = actor->weapon2();

				if (weaponToFire != 0) weaponToFire->fire(actor, gameData);
			}
		//}
	}


	//Logic Testing for animation
	updateGunAnimations(gameData);

	
}

void psLogic::awardActorPoints(ps::Actor* actor, int points)
{
}

void updateGunAnimations(ps::GameData* pGameData)
{
	//do them per actor
	int actorCount = pGameData->actorCount();
	for (int i = 0; i < actorCount; i++)
	{
		ps::Actor* actor = pGameData->pActorAtIndex(i);
		//update animations for selected gun obviously
		if (actor->weaponSelected() == 0) continue;

		ps::Weapon* weapon = (actor->weaponSelected() == 1) ? actor->weapon1() : actor->weapon2();
		weapon->updateAnimation(actor);
	}
}