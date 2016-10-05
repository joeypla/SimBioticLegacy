#include "GMSectorControl.h"

#include "SAnimatedMesh.h"
//ALL OF THE FOLLOWING MUST BE REMOVED ASAP ROCKY
SAnimatedMesh* animMesh = 0;
ps::GMSectorControl::GMSectorControl(const ps::GameInitInfo* gii)
{
	//animMesh = SRender::getSingleton()->createAnimatedMesh();
	//animMesh->loadFromFileX(L"../Resources/Meshes/tiny.x");


	_pLeaderboard = 0;
	//init leaderboard
	LeaderboardInitData lid;
	lid.gameMode = "Sector Control";
	lid.mapName = "Birth";
	lid.teamColors[0] = 0xffff0000;
	lid.teamColors[1] = 0xff0000ff;
	lid.numValues = 4;

	lid.valueNames[0] = L"Kills";
	lid.valueNames[1] = L"Deaths";
	lid.valueNames[2] = L"Captures";
	lid.valueNames[3] = L"Defends";

	lid.valueSources[0] = LEADERBOARD_INFOTYPE_SOURCE_KILLS;
	lid.valueSources[1] = LEADERBOARD_INFOTYPE_SOURCE_DEATHS;
	lid.valueSources[2] = LEADERBOARD_INFOTYPE_SOURCE_MODESPECIFIC_1;
	lid.valueSources[3] = LEADERBOARD_INFOTYPE_SOURCE_MODESPECIFIC_2;

	_pLeaderboard = new SLeaderboard(&lid);
	//Set name of gamemode
	modeName = "Sector Control";

	//Set up flags
	pointA_renderObject = SRender::getSingleton()->createS3D();
	pointB_renderObject = SRender::getSingleton()->createS3D();
	pointC_renderObject = SRender::getSingleton()->createS3D();
	 
	controlPointMaterialSetBlue = SRender::getSingleton()->createMaterialSet(L"../Resources/Meshes/RawVox/Game/Modes/SectorControl/Textures/Blue");
	//controlPointMaterialSetNeutral = SRender::getSingleton()->createMaterialSet(L"../Resources/Meshes/RawVox/Game/Modes/SectorControl/Textures/Neutral");
	controlPointMaterialSetNeutral = SRender::getSingleton()->createMaterialSet(L"../Resources/Meshes/Warrior");

	controlPointMaterialSetRed = SRender::getSingleton()->createMaterialSet(L"../Resources/Meshes/RawVox/Game/Modes/SectorControl/Textures/Red");

	ps::VoxelData* sectorControlPointVoxelData = new ps::VoxelData(L"../Resources/Meshes/RawVox/Game/Modes/SectorControl/sectorcontrolpoint.rv");
	controlPointMeshRed = SRender::getSingleton()->createMesh();
	controlPointMeshNeutral = SRender::getSingleton()->createAnimatedMesh();
	controlPointMeshBlue = SRender::getSingleton()->createMesh();

	controlPointMeshRed->loadMeshFromVoxelData(sectorControlPointVoxelData);

	//controlPointMeshNeutral->loadMeshFromVoxelData(sectorControlPointVoxelData);
	//controlPointMeshNeutral->loadMeshFromX(L"../Resources/Meshes/Warrior/warrior.x");
	controlPointMeshNeutral->loadFromFileX(L"../Resources/Meshes/Warrior/warrior.x");

	controlPointMeshBlue->loadMeshFromVoxelData(sectorControlPointVoxelData);
	 
	controlPointMeshRed->setMaterialSet(controlPointMaterialSetRed);
	controlPointMeshNeutral->setMaterialSet(controlPointMaterialSetNeutral);
	controlPointMeshBlue->setMaterialSet(controlPointMaterialSetBlue); 

	pointA_renderObject->setScale(0.1f);
	pointB_renderObject->setScale(0.4f);
	pointC_renderObject->setScale(0.4f);

	pointA_renderObject->setMesh(controlPointMeshNeutral);
	//pointA_renderObject->setMesh(animMesh);

	pointB_renderObject->setMesh(controlPointMeshRed);
	pointC_renderObject->setMesh(controlPointMeshBlue);
}

ps::GMSectorControl::~GMSectorControl(void)
{
}

int ps::GMSectorControl::update(ps::GameData* pGameData)
{
	//update positions of flags based on the set point data from the map data
	//pGameData->pWorldData()->

	static bool sectorControlPointsPlaced = false;
	if (!sectorControlPointsPlaced)
	{
		int mapSetPointCount = pGameData->pWorldData()->setPointCount();
		bool pointAfound = false, pointBfound = false, pointCfound = false;

		for (int i = 0; i < mapSetPointCount; i++)
		{
			ps::SetPoint* setpoint = pGameData->pWorldData()->getSetPointAtIndex(i);
			if (!pointAfound && setpoint->use == PS_SETPOINTS_USE_SCA)
			{
				pointA_renderObject->setPosition(setpoint->x, setpoint->y - 0.5f, setpoint->z);
				secA_position.x = setpoint->x;
				secA_position.y = setpoint->y;
				secA_position.z = setpoint->z;
			}
			if (!pointBfound && setpoint->use == PS_SETPOINTS_USE_SCB)
			{
				pointB_renderObject->setPosition(setpoint->x, setpoint->y, setpoint->z);
				secB_position.x = setpoint->x;
				secB_position.y = setpoint->y;
				secB_position.z = setpoint->z;
			}
			if (!pointCfound && setpoint->use == PS_SETPOINTS_USE_SCC)
			{
				pointC_renderObject->setPosition(setpoint->x, setpoint->y, setpoint->z);
				secC_position.x = setpoint->x;
				secC_position.y = setpoint->y;
				secC_position.z = setpoint->z;
			}
		}
		sectorControlPointsPlaced = true;
	}


	_pLeaderboard->update(pGameData);
	return 0;
}

void ps::GMSectorControl::respawnActor(ps::Actor* actor, ps::GameData* pGameData)
{
}

void ps::GMSectorControl::spawnAllActors(ps::GameData* pGameData)
{
	//We are going to sync the local actor data here with the game actor data
	//in this function. I know this doesn't seem EXACTLY like an init function,
	//but i needed access to game data and didn't feel like having a completely
	//new function to do that. This seems alright for now.
	
	//extract all spawns
	ps::World* wData = pGameData->pWorldData();
	int totalSpawns = 0;
	//create a list of spawns thats larger than the actual spawns, but only use whats needed.
	ps::SetPoint** extractedSpawnPoints = new ps::SetPoint*[wData->setPointCount()];
	for (int i = 0; i < wData->setPointCount(); i++)
	{
		if (wData->getSetPointAtIndex(i) != 0)
		{
			if (wData->getSetPointAtIndex(i)->use >= 0 && wData->getSetPointAtIndex(i)->use < 12)
			{
				extractedSpawnPoints[totalSpawns] = wData->getSetPointAtIndex(i);
				totalSpawns++;
			}
		}
	}


	
	int actorCount = pGameData->actorCount();
	int spawnIndex = rand() % totalSpawns;
	for (int i = 0; i < actorCount; i++)
	{
		//spawn actor at set point
		ps::Actor* actor = pGameData->pActorAtIndex(i);
		actor->spawn(pGameData, extractedSpawnPoints[spawnIndex]);
		spawnIndex++;
		if (spawnIndex >= totalSpawns) spawnIndex = 0;


	}
}