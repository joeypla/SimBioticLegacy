#ifndef PSEBASE_H
#define PSEBASE_H
#include "SSGame.h"

#include "SRender.h"
#include "SInterface.h"
#include "psGui.h"
#include <vector>

/* Grid Orientation Definitions */
#define PSE_GO_X 0
#define PSE_GO_Y 1
#define PSE_GO_Z 2

#define PSE_EDITING_MODE_GRID 1
#define PSE_EDITING_MODE_HAND 2

#define PSE_LIGHTS_MAX 128
#define PSE_SETPOINTS_MAX 128

#define PSE_SETPOINTS_USE_SPWN1 0
#define PSE_SETPOINTS_USE_SPWN2 1
#define PSE_SETPOINTS_USE_SCA   12
#define PSE_SETPOINTS_USE_SCB   13
#define PSE_SETPOINTS_USE_SCC   14
#define PSE_SETPOINTS_USE_CTF1  15
#define PSE_SETPOINTS_USE_CTF2  16

/* Class to communicate with Core */
class SEditor: public S_Interface{
public:
	SEditor(void);
	~SEditor(void);

	bool updateInterface(DWORD lastTimeTaken);
	bool renderToDevice(LPDIRECT3DDEVICE9 dev);
	void receiveSystemMessage(UINT message, UINT param);
};

/* Actual pse stuff here */
namespace pse{
	struct VoxelCoordinate{
		int x;
		int y;
		int z;
	};
	struct Light{
		Light(void);
		~Light(void);
		SLight* light;
		S3D* handle;
	};
	struct SetPoint{
		SetPoint(float x, float y, float z, int use);
		~SetPoint(void);

		float x;
		float y;
		float z;

		int gravityField;

		int use;
		/* 0 = spawn
		1 = Sector
		2 = Flag
		*/

		float rotation;

		S3D* handle;
		S3D* selectedHandle;
		void setUse(int use); //also sets the model thats used for it. Thats why its important.
		void setSelected(bool selected); // just changes whether or not the box around it is showing  
		void syncGraphic(void); //forces visual representation to be synced with actual values
	};
	struct WorldData{
		std::string properName;
		std::string texturePath;

		int xDim;
		int yDim;
		int zDim;

		float fogStart;
		float fogEnd;
		float fogRed;
		float fogGreen;
		float fogBlue;
		float ambientRed;
		float ambientGreen;
		float ambientBlue;

		ps::VoxelData* voxelData;

		S3D* world;
		SMutableMesh* voxelMesh;


		Light** lights;

		SetPoint** setPoints;
	};
	struct EditorData{

		int gravityConfig;

		SCamera* editorCam;
		S3D* gridX;
		S3D* gridY;
		S3D* gridZ;

		//Block Editing
		int editingMode;
		bool gridSameSideSelection;
		int gridOrientation;
		int gridLevelX;
		int gridLevelY;
		int gridLevelZ;

		int typeSelection;

		VoxelCoordinate fillSelection1;
		VoxelCoordinate fillSelection2;

		int lightSelection;
		bool lightGridLock;

		int setPointTypeSelection;
		int setPointSelection; //-1 for no selection
	};
	WorldData* getWorldData(void);
	EditorData* getEditorData(void);

	void setupWithoutFile(void);

	
	void processInput(WorldData* worldData, EditorData* editorData);

	//BLOCK EDITING - pseBlockEditing.cpp
	void setModuleActive_voxel(bool active);
	bool getModuleActive_voxel(void);
	psGui::Frame* getBlockFrame(void);
	void setupBlockEditing(void);
	void updateBlockEditing(void);
	VoxelCoordinate getVoxelCoordinateFromScreen(int x, int y);

	//LIGHT AND ATMOSPHERICS - pseLA.cpp
	void setModuleActive_lighting(bool active);
	bool getModuleActive_lighting(void);

	void shiftSelectedLightUp(void);
	void shiftSelectedLightDown(void);
	void shiftSelectedLightLeft(void);
	void shiftSelectedLightRight(void);

	void setAllLightsUnselected(void);
	psGui::Frame* getLAFrame(void);
	void setupLAEditing(void);
	void updateLAEditing(void);
	void updateAtmosphericsFromWorldData();
	int getLightFromClick(int x, int y);

	//GENERAL - pseGeneral.cpp
	void setModuleActive_setPoints(bool active);
	bool getModuleActive_setPoints(void);
	void shiftSelectedSetPoint(void);
	psGui::Frame* getGeneralFrame(void);
	void setupGeneral(void);
	void updateGeneral(void);
	void generalSyncFields(void);//syncs fields with world data
	void updateTargetFilePath(std::wstring newPath);

	//LOAD - pseLoad.cpp
	psGui::Frame* getLoadFrame(void);
	void setupLoad(void);
	bool getLoadOpened(void);
	void showLoadDialog(void);
	psGui::TextField* getLoadTextField(void);
}
#endif