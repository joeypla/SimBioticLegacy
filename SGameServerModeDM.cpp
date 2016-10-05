#include "SGameServerModeDM.h" 

#include "SGameServer.h"

#include "SGameStateDefines.h"
SGameServerMode_DM::SGameServerMode_DM(void)
{
}

SGameServerMode_DM::~SGameServerMode_DM(void)
{
}

int SGameServerMode_DM::update(SGameServer* server)
{
	//see if any players have reached the limit and if so, the game has ended
	for (int i = 0; i < SGAMESERVER_MAXPLAYERS; i++)
	{
		if (server->pPlayerAtIndex(i)->score >= server->scoreLimit())
		{ 
			return SGAMESTATE_OVER;
		}
	}

	return SGAMESTATE_PLAYING;
}

void SGameServerMode_DM::spawnPlayer(SGameServer* server, SGameServerPlayer* player)
{
	//The following spawning algorithm is not permanent at all. I just want to get
	//people dying and spawning first, then we can look at respawning at locations that
	//are based on where everyone is on the map. 
	int numSpawns1 = server->pWorld()->setPointCount_spawn1();
	int numSpawns2 = server->pWorld()->setPointCount_spawn2();

	int teamSpawn = rand() % 2;
	ps::SetPoint* spawnPoint = NULL;
	int spawn = 0;
	switch (teamSpawn)
	{
		
	case 0:
		spawn = rand() % numSpawns1;
		spawnPoint = server->pWorld()->pSetPoint_spawn1(spawn);
		break;
	case 1:
		spawn = rand() % numSpawns2;
		spawnPoint = server->pWorld()->pSetPoint_spawn2(spawn);
		break;
	default:
		break;
	}
	psMath::float3 direction = spawnPoint->getDirectionSamplingVoxelData(server->pWorld()->pVoxelData());
	player->health = 100;
	player->shield = 100;
	player->x = spawnPoint->x;
	player->y = spawnPoint->y;
	player->z = spawnPoint->z;
	
	player->dirx = 0.0f;//direction.x;
	player->diry = 0.0f;//direction.y;
	player->dirz = 1.0f;//direction.z;
	player->gravity = spawnPoint->gravityConfig;

	player->state = 2;//set spawning state
	//The rotation isnt working properly for now, so the alternative is finding which direction will have no voxels. This way,
	//if you spawn against a wall, you will automatically face open space. This may even be a permanent solution to simplify things.
	
}