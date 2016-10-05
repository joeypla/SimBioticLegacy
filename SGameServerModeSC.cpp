#include "SGameServerModeSC.h" 
#include "SGameServer.h"
#include <Windows.h>


static const float DISTANCE_TO_CAPTURE = 5.0f;

SGameServerMode_SC::SGameServerMode_SC(void)
{
}

SGameServerMode_SC::~SGameServerMode_SC(void)
{
}

int SGameServerMode_SC::update(SGameServer* server)
{
	static DWORD lastUpdate = GetTickCount();

	//check to see if any of the players are on any of the flags
	//starting with flag A.
	static 
	int players1OnA = 0;
	int players1OnB = 0;
	int players1OnC = 0;

	int players2OnA = 0;
	int players2OnB = 0;
	int players2OnC = 0;

	for (int i = 0; i < SGAMESERVER_MAXPLAYERS; i++)
	{
		SGameServerPlayer* player = server->pPlayerAtIndex(i);

		float flag_x, flag_y, flag_z;
		
		//find distance for player to flag a
		ps::SetPoint* flag = server->pWorld()->pSetPoint_SC_A();
		float distance = sqrt(pow(player->x - flag->x, 2) + pow(player->y - flag->y, 2) + pow(player->z - flag->z, 2));
		if (distance <= DISTANCE_TO_CAPTURE)
		{
			if (player->team == 1)
				players1OnA++;

			if (player->team == 2)
				players2OnA++;
		}

		//find distance for player to flag B
		flag = server->pWorld()->pSetPoint_SC_B();
		distance = sqrt(pow(player->x - flag->x, 2) + pow(player->y - flag->y, 2) + pow(player->z - flag->z, 2));
		if (distance <= DISTANCE_TO_CAPTURE)
		{
			if (player->team == 1)
				players1OnB++;

			if (player->team == 2)
				players2OnB++;
		}

		//find distance for player to flag C
		flag = server->pWorld()->pSetPoint_SC_C();
		distance = sqrt(pow(player->x - flag->x, 2) + pow(player->y - flag->y, 2) + pow(player->z - flag->z, 2));
		if (distance <= DISTANCE_TO_CAPTURE)
		{
			if (player->team == 1)
				players1OnC++;

			if (player->team == 2)
				players2OnC++;
		}

	}

	int playerDifferenceOnA = players1OnA - players2OnA;
	int playerDifferenceOnB = players1OnB - players2OnB;
	int playerDifferenceOnC = players1OnC - players2OnC;
	//perform progress logic for flag a when its owned by no one.
	if (ownerA == 0)
	{
		if (playerDifferenceOnA == 0)
			balanceA = 100;

		balanceA += playerDifferenceOnA;
		if (balanceA <= 0)
		{
			balanceA = 0;
			ownerA = 1;
		}

		if (balanceA >= 200)
		{
			balanceA = 200;
			ownerA = 2;
		}
	}

	//perform progress logic for flag a when its owned by no one.
	if (ownerB == 0)
	{
		if (playerDifferenceOnB == 0)
			balanceB = 100;

		balanceB += playerDifferenceOnB;
		if (balanceB <= 0)
		{
			balanceB = 0;
			ownerB = 1;
		}

		if (balanceB >= 200)
		{
			balanceB = 200;
			ownerB = 2;
		}
	}

	//perform progress logic for flag a when its owned by no one.
	if (ownerC == 0)
	{
		if (playerDifferenceOnC == 0)
			balanceC = 100;

		balanceC += playerDifferenceOnC;
		if (balanceC <= 0)
		{
			balanceC = 0;
			ownerC = 1;
		}

		if (balanceC >= 200)
		{
			balanceC = 200;
			ownerC = 2;
		}
	}
	//check ownership of flags to award points
	if (ownerA == 1)
	{
		score1 += 50;
	} else if (ownerA == 2)
	{
		score2 += 50;
	}

	if (ownerB == 1)
	{
		score1 += 50;
	} else if (ownerB == 2)
	{
		score2 += 50;
	}

	if (ownerC == 1)
	{
		score1 += 50;
	} else if (ownerC == 2)
	{
		score2 += 50;
	}

	return 0;
}

void SGameServerMode_SC::spawnPlayer(SGameServer* server, SGameServerPlayer* player)
{
}