#include "psInput.h"
#include "SInput.h"

#include "Actor.h"

#include "psMath.h"
#include "psHUD.h"

#include "SGameStateDefines.h"
#include "GameMode.h"

static ps::GameData* lastGameData = 0;
void psInput::update(ps::GameData* gameData)
{
	lastGameData = gameData;
	//Set player pointer to the correct client actor (could be anything from 0-15)
	ps::Actor* player = gameData->pClientActor();
	if (player == 0) return;
	(S_INPUT::mousePressed(0)) ? player->isFiring = true : player->isFiring = false;
	(S_INPUT::mousePressed(1)) ? player->isAiming = true : player->isAiming = false;
	

	player->isSprinting = (S_INPUT::keyPressed(DIK_LSHIFT)) ? true : false;
	
	player->isMoving_f = (S_INPUT::keyPressed(DIK_W)) ? true : false;
	player->isMoving_b = (S_INPUT::keyPressed(DIK_S)) ? true : false;
	player->isMoving_l = (S_INPUT::keyPressed(DIK_A)) ? true : false;
	player->isMoving_r = (S_INPUT::keyPressed(DIK_D)) ? true : false;

	player->isJumping = (S_INPUT::keyPressed(DIK_SPACE)) ? true : false;

	player->isGravityLefting = (S_INPUT::keyPressed(DIK_Q)) ? true : false;
	player->isGravityRighting = (S_INPUT::keyPressed(DIK_E)) ? true : false;

	//player->isGravityForwarding = (S_INPUT::keyPressed(DIK_F)) ? true : false;

	//mouse looking
	float mouseX_modifier = 0.0015f;
	float mouseY_modifier = -0.0015f;
	if (player->isAiming)
	{
		mouseX_modifier *= 0.6f;
		mouseY_modifier *= 0.6f;
	}
	float mouseX = S_INPUT::getMouseDeltaX();
	float mouseY = S_INPUT::getMouseDeltaY();
	
	if (!player->changingGravity ) player->view()->rotateView(mouseY*mouseY_modifier, mouseX*mouseX_modifier, 0.0f, player->gravityField);

	if (S_INPUT::keyPressed(DIK_T)) player->view()->fixUpVector(player->gravityField);
	//player reload
	if (S_INPUT::keyPressed(DIK_R))
	{
		
		if (player->weaponSelected() > 0)
		{
			if (player->weaponSelected() == 1) player->weapon1()->reload(player);
			if (player->weaponSelected() == 2) player->weapon2()->reload(player);
		}
	}

	//player crouch
	if (S_INPUT::keyPressed(DIK_C))
	{
		player->isCrouching = true;
	} else player->isCrouching = false;
	if (S_INPUT::keyPressed(DIK_X)) player->isRolling = true; 
	else player->isRolling = false;

	
	//View leaderboard
	gameData->pGameMode()->pLeaderboard()->setVisibility(false);
	if (S_INPUT::keyPressed(DIK_TAB))
	{
		gameData->pGameMode()->pLeaderboard()->setVisibility(true);
	}
	
	//test remove asap
	if (S_INPUT::keyPressed(DIK_M))
	{
		gameData->pWorldData()->pVoxelData()->writeCompressedDataToFile("compressedMap.psm");
	}
}

void psInput::receiveSystemMessage(UINT message, UINT param)
{
	if (lastGameData == 0) return;
	ps::Actor* player = lastGameData->pClientActor();
	if (message == WM_KEYDOWN)
	{
		if (param == 0x31)
			player->shouldActivateAbility1 = true;

		if (param == 0x32)
			player->shouldActivateAbility2 = true;

		if (param == 0x33)
			player->shouldActivateAbility3 = true;

		if (param == 0x34)
			player->shouldActivateAbility4 = true;
	}
}