#include "World.h"

#include <fstream>
using namespace ps;
World::World(void){}
#include "SLOG.h"
#include <sstream>
World::~World(void)
{
	SRender::getSingleton()->releaseMaterialSet(_pMaterialSet);
	SRender::getSingleton()->releaseMesh(_pMesh);
	SRender::getSingleton()->releaseS3D(_pRenderObject);

	for (int i = 0; i < _lightCount; i++)
	{
		SRender::getSingleton()->releaseLight(_ppLightObjects[i]);
	}
}
World* World::new_World(std::string mapPath)
{
	World* world = new World();

	std::ifstream mapFile;
	mapFile.open(mapPath, std::ios::binary);
	
	if (!mapFile.is_open())
	{
		S_LOG::write("Map was not loaded correctly!");
		S_LOG::write(mapPath.c_str());
			return 0; // Exit function - map data not loaded correctly
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
	world->_properName = ss.str().c_str();


	//READ TEXTURE PATH
	std::stringstream textureStringStream;
	textureStringStream.str("");
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
	
	world->_pVoxelData = new VoxelData(xVox, yVox, zVox);


	//READ ATMOSPHERICS
	mapFile.read((char*)&world->ambientRed, sizeof(float));
	mapFile.read((char*)&world->ambientGreen, sizeof(float));
	mapFile.read((char*)&world->ambientBlue, sizeof(float));

	mapFile.read((char*)&world->fogRed, sizeof(float));
	mapFile.read((char*)&world->fogGreen, sizeof(float));
	mapFile.read((char*)&world->fogBlue, sizeof(float));

	mapFile.read((char*)&world->fogStart, sizeof(float));
	mapFile.read((char*)&world->fogEnd, sizeof(float));

	//read set point and light count
	mapFile.read((char*)&world->_lightCount, sizeof(int));
	mapFile.read((char*)&world->_setPointCount, sizeof(int));
	//PART 1 - Read voxel data
	for (int z = 0; z < world->_pVoxelData->x(); z++){
		for (int y = 0; y < world->_pVoxelData->y(); y++){
			for (int x = 0; x < world->_pVoxelData->z(); x++){
				char data = mapFile.get();
				world->_pVoxelData->setData(data, x, y, z);
			}
		}
	}
	
	//PART 2 - Read light data - FF
	world->_ppLightObjects = new SLight*[world->_lightCount];
	for (int i = 0; i < world->_lightCount; i++){
		world->_ppLightObjects[i] = SRender::getSingleton()->createLight();
		//POSITION
		mapFile.read((char*)&world->_ppLightObjects[i]->x, sizeof(float));
		mapFile.read((char*)&world->_ppLightObjects[i]->y, sizeof(float));
		mapFile.read((char*)&world->_ppLightObjects[i]->z, sizeof(float));

		//DIFFUSE
		mapFile.read((char*)&world->_ppLightObjects[i]->r, sizeof(float));
		mapFile.read((char*)&world->_ppLightObjects[i]->g, sizeof(float));
		mapFile.read((char*)&world->_ppLightObjects[i]->b, sizeof(float));

		//AMBIENT
		mapFile.read((char*)&world->_ppLightObjects[i]->ambientFactor, sizeof(float));

		
		

		//Radius
		mapFile.read((char*)&world->_ppLightObjects[i]->radius, sizeof(float));

		//SPOTLIGHT SHIT
		mapFile.read((char*)&world->_ppLightObjects[i]->dx, sizeof(float));
		mapFile.read((char*)&world->_ppLightObjects[i]->dy, sizeof(float));
		mapFile.read((char*)&world->_ppLightObjects[i]->dz, sizeof(float));

		mapFile.read((char*)&world->_ppLightObjects[i]->type, sizeof(int));
		mapFile.read((char*)&world->_ppLightObjects[i]->phi, sizeof(float));
		mapFile.read((char*)&world->_ppLightObjects[i]->theta, sizeof(float));
	}
	//PART 3 - read Set Point Data
	world->_ppSetPoints = new ps::SetPoint*[world->_setPointCount];
	for (int i = 0; i < world->_setPointCount; i++){
		world->_ppSetPoints[i] = new ps::SetPoint();
		//POSITION
		mapFile.read((char*)&world->_ppSetPoints[i]->x, sizeof(float));
		mapFile.read((char*)&world->_ppSetPoints[i]->y, sizeof(float));
		mapFile.read((char*)&world->_ppSetPoints[i]->z, sizeof(float));

		//OTHER
		mapFile.read((char*)&world->_ppSetPoints[i]->gravityConfig, sizeof(int));
		mapFile.read((char*)&world->_ppSetPoints[i]->use, sizeof(int));
		mapFile.read((char*)&world->_ppSetPoints[i]->rotation, sizeof(float));

		
	}

	mapFile.close();

	
	/* Now that map is loaded from file, lets set some shit up */
	world->_pMesh = SRender::getSingleton()->createMesh();
	world->_pMaterialSet = SRender::getSingleton()->createMaterialSet(wTextureString);
	world->_pMesh->setMaterialSet(world->_pMaterialSet);
	
	world->_pMesh->loadMeshFromVoxelData(world->pVoxelData());
	world->_pRenderObject = SRender::getSingleton()->createS3D();
	world->_pRenderObject->setMesh(world->_pMesh);
	world->_pRenderObject->setPosition((float)world->pVoxelData()->x() * 0.5f, (float)world->pVoxelData()->y() * 0.5f, (float)world->pVoxelData()->z() * 0.5f);
	world->_pRenderObject->castsShadow = true;
	SRender::getSingleton()->setAmbientColor(world->ambientRed, world->ambientGreen, world->ambientBlue);
	SRender::getSingleton()->setFogColor(world->fogRed, world->fogGreen, world->fogBlue);
	SRender::getSingleton()->setFogStart(world->fogStart);
	SRender::getSingleton()->setFogEnd(world->fogEnd);
	return world;
}

VoxelData* World::pVoxelData(void)
{
	return _pVoxelData;
}

int World::setPointCount(void)
{
	return _setPointCount;
}

ps::SetPoint* World::getSetPointAtIndex(int index)
{
	if (index < 0 || index >= _setPointCount) return 0;

	return _ppSetPoints[index];
}

std::string World::properName(void){return _properName;}