#include "SGameServerWorld.h"

#include <fstream>
#include <sstream>
#include "SLOG.h"
SGameServerWorld::SGameServerWorld(std::string path)
{

	std::ifstream mapFile;
	mapFile.open(path, std::ios::binary);
	
	if (!mapFile.is_open())
	{
		S_LOG::write("Server Side Map was not loaded correctly!");
		S_LOG::write(path.c_str());
	}

	//READ PROPER NAME
	std::stringstream ss;
	unsigned int mapNameSize = 0;
	char properName[100];
	mapFile.read((char*)&mapNameSize, sizeof(int));
	mapFile.read((char*)&properName, mapNameSize);
	for (int i = 0; i < mapNameSize; i++)
	{
		ss<<properName[i];
	}
	_properName = ss.str().c_str();


	//READ TEXTURE PATH
	std::stringstream textureStringStream;
	unsigned int texturePathSize = 0;
	char texturePath[100];
	mapFile.read((char*)&texturePathSize, sizeof(int));
	mapFile.read((char*)&texturePath, texturePathSize);
	for (int i = 0; i < texturePathSize; i++)
	{
		textureStringStream<<texturePath[i];
	}
	std::string textureString = textureStringStream.str().c_str();
	std::wstring wTextureString(textureString.begin(), textureString.end());

	int xVox = 0;
	int yVox = 0;
	int zVox = 0;
	mapFile.read((char*)&xVox, sizeof(int));
	mapFile.read((char*)&yVox, sizeof(int));
	mapFile.read((char*)&zVox, sizeof(int));
	
	_pVoxelData = new ps::VoxelData(xVox, yVox, zVox);


	//READ ATMOSPHERICS
	float dummyFloat;//we dont actually need these values, but do need to advance the parser.
	mapFile.read((char*)&dummyFloat, sizeof(float));
	mapFile.read((char*)&dummyFloat, sizeof(float));
	mapFile.read((char*)&dummyFloat, sizeof(float));

	mapFile.read((char*)&dummyFloat, sizeof(float));
	mapFile.read((char*)&dummyFloat, sizeof(float));
	mapFile.read((char*)&dummyFloat, sizeof(float));

	mapFile.read((char*)&dummyFloat, sizeof(float));
	mapFile.read((char*)&dummyFloat, sizeof(float));

	//read set point and light count
	int dummyLightCount;
	mapFile.read((char*)&dummyLightCount, sizeof(int));
	mapFile.read((char*)&_setPointCount, sizeof(int));
	//PART 1 - Read voxel data
	for (int z = 0; z < _pVoxelData->x(); z++){
		for (int y = 0; y < _pVoxelData->y(); y++){
			for (int x = 0; x < _pVoxelData->z(); x++){
				char data = mapFile.get();
				_pVoxelData->setData(data, x, y, z);
			}
		}
	}
	
	//PART 2 - Read light data - FF
	SLight** dummyLights;
	dummyLights = new SLight*[dummyLightCount];
	for (int i = 0; i < dummyLightCount; i++){
		dummyLights[i] = new SLight();
		//POSITION
		mapFile.read((char*)&dummyLights[i]->x, sizeof(float));
		mapFile.read((char*)&dummyLights[i]->y, sizeof(float));
		mapFile.read((char*)&dummyLights[i]->z, sizeof(float));

		//DIFFUSE
		mapFile.read((char*)&dummyLights[i]->r, sizeof(float));
		mapFile.read((char*)&dummyLights[i]->g, sizeof(float));
		mapFile.read((char*)&dummyLights[i]->b, sizeof(float));

		//AMBIENT
		mapFile.read((char*)&dummyLights[i]->ambientFactor, sizeof(float));

		
		

		//Radius
		mapFile.read((char*)&dummyLights[i]->radius, sizeof(float));

		//SPOTLIGHT SHIT
		mapFile.read((char*)&dummyLights[i]->dx, sizeof(float));
		mapFile.read((char*)&dummyLights[i]->dy, sizeof(float));
		mapFile.read((char*)&dummyLights[i]->dz, sizeof(float));

		mapFile.read((char*)&dummyLights[i]->type, sizeof(int));
		mapFile.read((char*)&dummyLights[i]->phi, sizeof(float));
		mapFile.read((char*)&dummyLights[i]->theta, sizeof(float));
	}
	//PART 3 - read Set Point Data
	_ppSetPoints = new ps::SetPoint*[_setPointCount];
	for (int i = 0; i < _setPointCount; i++){
		_ppSetPoints[i] = new ps::SetPoint();
		//POSITION
		mapFile.read((char*)&_ppSetPoints[i]->x, sizeof(float));
		mapFile.read((char*)&_ppSetPoints[i]->y, sizeof(float));
		mapFile.read((char*)&_ppSetPoints[i]->z, sizeof(float));

		//OTHER
		mapFile.read((char*)&_ppSetPoints[i]->gravityConfig, sizeof(int));
		mapFile.read((char*)&_ppSetPoints[i]->use, sizeof(int));
		mapFile.read((char*)&_ppSetPoints[i]->rotation, sizeof(float));

		
	}

	mapFile.close();


	//now delete the light memory because we dont need it.

	for (int i = 0; i < dummyLightCount; i++)
	{
		delete dummyLights[i];

	}
	delete dummyLights;


	//The general set points have been filled, but we also want to fill
	//the specific set points for quicker accessing.

	//once the world has been created, we want to get counts for all the specific set points
	_setPointCount_spawn1 = 0;
	_setPointCount_spawn2 = 0; 
	for (int i = 0; i < _setPointCount; i++)
	{
		if (_ppSetPoints[i]->use == PS_SETPOINTS_USE_SPWN1) _setPointCount_spawn1++;
		if (_ppSetPoints[i]->use == PS_SETPOINTS_USE_SPWN2) _setPointCount_spawn2++;

	}

	_ppSetPoints_spawn1 = (_setPointCount_spawn1 > 0) ? new ps::SetPoint*[_setPointCount_spawn1] : NULL;
	_ppSetPoints_spawn2 = (_setPointCount_spawn2 > 0) ? new ps::SetPoint*[_setPointCount_spawn2] : NULL;

	//now we actually set the point by looping through all the set points again
	int spawn1Index = 0;
	int spawn2Index = 0;
	for (int i = 0; i < _setPointCount; i++)
	{
		switch (_ppSetPoints[i]->use)
		{
		case PS_SETPOINTS_USE_SPWN1:
			_ppSetPoints_spawn1[spawn1Index++] = _ppSetPoints[i];
			break;
		case PS_SETPOINTS_USE_SPWN2:
			_ppSetPoints_spawn2[spawn2Index++] = _ppSetPoints[i];
			break;
		default:
			break;
		}
	}
}

SGameServerWorld::~SGameServerWorld(void)
{
}

int SGameServerWorld::setPointCount(void)
{
	return _setPointCount;
}

ps::SetPoint* SGameServerWorld::pSetPoint(int index)
{
	if (index < 0 || index >= _setPointCount) return NULL;

	return _ppSetPoints[index];

}

int SGameServerWorld::setPointCount_spawn1(void)
{
	return _setPointCount_spawn1;
}

ps::SetPoint* SGameServerWorld::pSetPoint_spawn1(int index)
{
	if (index < 0 || index >= _setPointCount_spawn1) return NULL;

	return _ppSetPoints_spawn1[index];
}

int SGameServerWorld::setPointCount_spawn2(void)
{
	return _setPointCount_spawn2;
}

ps::SetPoint* SGameServerWorld::pSetPoint_spawn2(int index)
{
	if (index < 0 || index >= _setPointCount_spawn2) return NULL;

	return _ppSetPoints_spawn2[index];
}

ps::SetPoint* SGameServerWorld::pSetPoint_SC_A(void)
{
	return _pSetPoint_SC_A;
}

ps::SetPoint* SGameServerWorld::pSetPoint_SC_B(void)
{
	return _pSetPoint_SC_B;
}

ps::SetPoint* SGameServerWorld::pSetPoint_SC_C(void)
{
	return _pSetPoint_SC_C;
}

ps::SetPoint* SGameServerWorld::pSetPoint_CTF_1(void)
{
	return _pSetPoint_CTF_1;
}

ps::SetPoint* SGameServerWorld::pSetPoint_CTF_2(void)
{
	return _pSetPoint_CTF_2;
}

ps::VoxelData* SGameServerWorld::pVoxelData(void)
{
	return _pVoxelData;
}