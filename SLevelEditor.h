#ifndef SLEVELEDITOR_H
#define SLEVELEDITOR_H

#include <string>
#include <d3d9.h>
#include <d3dx9.h>
#include "SInterface.h"

#include "SEditorUniversal.h"
#include "SEditorGrid.h"
//DIFFERENT MODES OF EDITING
#define SLEVELEDITOR_MODE_GRID 1
#define SLEVELEDITOR_MODE_SPECIFICNOCOLLISION 2
#define SLEVELEDITOR_MODE_SPECIFICCOLLISION 3

#define SLEVELEDITOR_MAXLIGHTS 100

struct SEDITOR_VOXEL{
	char type;
};

struct SEditorLight{
	D3DLIGHT9 light;
	bool active;
};
class SLevelEditor: public S_Interface{
public:
	SLevelEditor(void);
	~SLevelEditor(void);

	bool loadLevelFromFile(std::wstring filename);
	void saveRawVoxelData(void);
	void saveMap(void);
	bool renderToDevice(const LPDIRECT3DDEVICE9 dev);
	bool updateInterface(DWORD lastTimeTaken);
private:
	DWORD timeTest;
	//MAP DATA
	std::wstring textureAtlasPath;
	SEDITOR_VOXEL voxelData[MAP_DIMENSION][MAP_DIMENSION][MAP_DIMENSION];
	bool loadVoxelGeometry(void);
	bool loadVoxelGeometryProto(void);
	bool updateVoxelGeometry(void);
	bool updateVoxelGeometryProto(void);
	bool loadTextures(void);
	bool loadTexturesProto(void);
	void createTextureAtlasFromFileGroup(void);
	//SHIT FOR DA MAP RENDERING
	D3DMATERIAL9 defaultMaterial;
	D3DLIGHT9 flashLight;
	LPDIRECT3DVERTEXBUFFER9 totalMapVertexBuffer;
	LPDIRECT3DINDEXBUFFER9 totalMapIndexBuffer;
	LPDIRECT3DTEXTURE9 totalMapTextureAtlas;
	LPDIRECT3DTEXTURE9 blockTextures[8];
	int perTextureFaceCount[8];
	int faceCount;
	//SHIT FOR DA GRID
	SEditorGrid grid;
	
	void getGridSelection(SEDITOR_BLOCKSELECTION* blockSelect, int x, int y);
	void fillVoxelData(void);
	//int gridLevel;
	//USER DATA
	char selectedType;
	
	SEDITOR_BLOCKSELECTION firstClick, secondClick;
	bool multiSelectClick;
	char favBlocks[10];
	void resetPosition(void);
	D3DXVECTOR3 user_position;
	D3DXVECTOR3 user_upVector;
	D3DXVECTOR3 user_lookAt;
	bool isGrounded;
	D3DXVECTOR3 acceleration;
	D3DXVECTOR3 velocity;
	int gravityConfig;
	bool roamingMode;
	bool gridMode;
	void roamingUpdate(void);

	void updateVoxelCollisionBuffer(void);
	float xBox, yBox, zBox;
	D3DXVECTOR3 collisionBuffer[27];
	int collisionBuffer_solidCount;

	void orientationUpdate(void);

	
	//Other shit - Spawns, lights
	void createBoxBuffers(void);
	LPDIRECT3DVERTEXBUFFER9 boxVertexBuffer;
	LPDIRECT3DINDEXBUFFER9 boxIndexBuffer;
	//JUST STUFF FOR PLACING LIGHTING
	bool lightingMode;
	int lightSelection;
	int lightModSelection;
	SEditorLight lights[SLEVELEDITOR_MAXLIGHTS];
	int checkRayIntersectLight(void);
	D3DXVECTOR3 getPositionAlongRay(int loops);
	void renderLightingText(void);
	int requestAddLight(void);
	int getNumActiveLights(void);
	int numLights;
	void updateLightBuffer(void);
	int lightBuffer[8];
	void giveToLightBuffer(int index);
	//STUFF FOR PLACING SPAWNS
};
#endif