#include "pseBase.h"


static pse::WorldData* worldData;
static pse::EditorData* editorData;
static SRender* r;

static bool moduleActive_voxel = true;
static bool moduleActive_lighting = true;
static bool moduleActive_setPoints = true;

static bool loadSetPointMeshes(void);
static SMesh* setPoint_spwn1_mesh = 0;
static SMesh* setPoint_spwn2_mesh = 0;
static SMesh* setPoint_CTFRed_mesh = 0;
static SMesh* setPoint_CTFBlue_mesh = 0;
static SMesh* setPoint_SC_A_mesh = 0;
static SMesh* setPoint_SC_B_mesh = 0;
static SMesh* setPoint_SC_C_mesh = 0;
static SLineBoxMesh* setPoint_selected_mesh = 0;

#include "SLOG.h"
#include <sstream>
#include "SInput.h"

SEditor::SEditor(void)
{
	r = SRender::getSingleton();
	//init sub modules
	pse::setupWithoutFile();


	pse::setupBlockEditing();
	pse::setupLAEditing();
	pse::setupGeneral();
	pse::setupLoad();
	

}

SEditor::~SEditor(void)
{
}

bool SEditor::updateInterface(DWORD lastTimeTaken)
{
	if (pse::getLoadOpened())
	{
		return true;
	}
	pse::processInput(worldData, editorData);
	pse::updateBlockEditing();
	pse::updateLAEditing();
	pse::updateGeneral();
	return true;
}

bool SEditor::renderToDevice(LPDIRECT3DDEVICE9 dev)
{
	r->render();
	return true;
}

void SEditor::receiveSystemMessage(UINT message, UINT param)
{

	/* if load is opened, its the only thing
	that is important... */
	if (pse::getLoadOpened())
	{
		pse::getLoadFrame()->sendSystemMessage(message, param);
		return;
	}

	/* if load isnt opened, then proceed :) */
	pse::getGeneralFrame()->sendSystemMessage(message, param);
	switch (message)
	{
	case WM_LBUTTONDOWN:
		if (pse::getBlockFrame()->getMouseWithinFrame())
		{
			pse::getBlockFrame()->sendSystemMessage(message, param);
			break;
		}

		if (pse::getLAFrame()->getMouseWithinFrame())
		{
			pse::getLAFrame()->sendSystemMessage(message, param);
			break;
		}

		if (pse::getGeneralFrame()->getMouseWithinFrame())
		{
			pse::getGeneralFrame()->sendSystemMessage(message, param);
			break;
		}


		if (moduleActive_voxel)
		{
			if (pse::getEditorData()->editingMode == PSE_EDITING_MODE_HAND)
			{
				pse::WorldData* wd = pse::getWorldData();
				pse::VoxelCoordinate vc = pse::getVoxelCoordinateFromScreen(S_INPUT::getMouseX(), S_INPUT::getMouseY());
				if (vc.x > -1) wd->voxelData->setData(pse::getEditorData()->typeSelection, vc.x,vc.y,vc.z);
				wd->voxelMesh->updateGeometry(wd->voxelData);
			}
		}

		
		break;
	case WM_KEYDOWN:
		if (param == VK_LEFT)
		{
			pse::WorldData* wd = pse::getWorldData();
		}

		if (param == VK_RIGHT)
		{
		}

		if (param == VK_UP)
		{
		}

		if (param == VK_DOWN)
		{
		}
	default:
		break;
	}
}


void pse::setupWithoutFile(void)
{
	r->setClearColor(0xff8f8f8f);
	worldData = new WorldData();
	editorData = new EditorData();
	editorData->editingMode = PSE_EDITING_MODE_HAND;

	//set voxel data - ground only
	worldData->xDim = 64;
	worldData->yDim = 64;
	worldData->zDim = 64;
	worldData->voxelData = new ps::VoxelData(worldData->xDim, worldData->yDim, worldData->zDim);
	for (int x = 0; x < 64; x++)
	{
		for (int y = 0; y < 64; y++)
		{
			for (int z = 0; z < 64; z++)
			{
				worldData->voxelData->setData(0, x, y, z);
			}
		}
	}
	for (int x = 0; x < 64; x++)
	{
		for (int z = 0; z < 64; z++)
		{
			worldData->voxelData->setData(1,x,0,z);
		}
	}


	
	//set render object + mutable mesh
	//SRender* r = SRender::getSingleton();
	
	S3D* world2 = r->createS3D();
	worldData->world = world2;
	worldData->voxelMesh = r->createMutableMesh();
	world2->setMesh(worldData->voxelMesh);
	int num = worldData->voxelMesh->createAtlasFromPath(L"../Resources/Levels/Allocation/Textures");
	worldData->voxelMesh->updateGeometry(worldData->voxelData);
	
	/* init lights */
	worldData->lights = new pse::Light*[PSE_LIGHTS_MAX];
	for (int i = 0; i < PSE_LIGHTS_MAX; i++)
	{
		worldData->lights[i] = 0;
	}

	/* init Set Points */
	loadSetPointMeshes();
	editorData->setPointTypeSelection = 0;
	editorData->setPointSelection = -1;
	worldData->setPoints = new pse::SetPoint*[PSE_SETPOINTS_MAX];
	for (int i = 0; i < PSE_SETPOINTS_MAX; i++)
	{
		worldData->setPoints[i] = 0;
	}
	//Editor Data
	SGridMesh* gridMeshX = r->createGridMesh();
	SGridMesh* gridMeshY = r->createGridMesh();
	SGridMesh* gridMeshZ = r->createGridMesh();
	gridMeshX->loadGrid(64, 64, 0, 0xffff0000);
	gridMeshY->loadGrid(64, 64, 1, 0xff00ff00);
	gridMeshZ->loadGrid(64, 64, 2, 0xff0000ff);
	editorData->editorCam = new SCamera();
	r->setCamera(editorData->editorCam);
	editorData->editorCam->setPosition(32.0f, 32.0f, 32.0f);

	
	editorData->gridX = r->createS3D();
	editorData->gridY = r->createS3D();
	editorData->gridZ = r->createS3D();
	editorData->gridX->setMesh(gridMeshX);
	editorData->gridY->setMesh(gridMeshY);
	editorData->gridZ->setMesh(gridMeshZ);

	editorData->editingMode = PSE_EDITING_MODE_HAND;
	editorData->gridLevelX=0;
	editorData->gridLevelY=0;
	editorData->gridLevelZ=0;
	editorData->gridOrientation = PSE_GO_Y;
	editorData->typeSelection = 0;
	editorData->editorCam->lookAt(0.0f, 0.0f, 0.0f);
	
	editorData->lightSelection = -1;
	editorData->lightGridLock = false;

	editorData->gravityConfig = 2;
	

	
	
	

	
}

pse::EditorData* pse::getEditorData(void)
{
	return editorData;
}

pse::WorldData* pse::getWorldData(void)
{
	return worldData;
}

bool pse::getModuleActive_voxel(void)
{
	return moduleActive_voxel;
}

bool pse::getModuleActive_lighting(void)
{
	return moduleActive_lighting;
}

void pse::setModuleActive_voxel(bool active)
{
	moduleActive_voxel = active;
}

void pse::setModuleActive_lighting(bool active)
{
	moduleActive_lighting = active;
}

void pse::setModuleActive_setPoints(bool active)
{
	moduleActive_setPoints = true;
}
bool pse::getModuleActive_setPoints(void)
{
	return moduleActive_setPoints;
}
/*
static SMesh* setPoint_spwn1_mesh = 0;
static SMesh* setPoint_spwn2_mesh = 0;
static SMesh* setPoint_CTFRed_mesh = 0;
static SMesh* setPoint_CTFBlue_mesh = 0;
static SMesh* setPoint_SC_A_mesh = 0;
static SMesh* setPoint_SC_B_mesh = 0;
static SMesh* setPoint_SC_C_mesh = 0;
*/
bool loadSetPointMeshes(void)
{

	//Spawns
	setPoint_spwn1_mesh = SRender::getSingleton()->createMesh();
	ps::VoxelData* vd = new ps::VoxelData(L"../Resources/Meshes/RawVox/Editor/SpawnModelwHoopOne.psm.rv");
	setPoint_spwn1_mesh->loadMeshFromVoxelData(vd);
	setPoint_spwn1_mesh->setMaterialSet(SRender::getSingleton()->createMaterialSet(L"../Resources/Meshes/RawVox/Editor/Textures/Red"));
	delete vd;
	vd = 0;

	setPoint_spwn2_mesh = SRender::getSingleton()->createMesh();
	vd = new ps::VoxelData(L"../Resources/Meshes/RawVox/Editor/SpawnModelwHoopTwo.psm.rv");
	setPoint_spwn2_mesh->loadMeshFromVoxelData(vd);
	setPoint_spwn2_mesh->setMaterialSet(SRender::getSingleton()->createMaterialSet(L"../Resources/Meshes/RawVox/Editor/Textures/Blue"));
	delete vd;
	vd = 0;

	//CTF
	setPoint_CTFRed_mesh = SRender::getSingleton()->createMesh();
	vd = new ps::VoxelData(L"../Resources/Meshes/RawVox/Editor/SpawnModelwHoopFlag.psm.rv");
	setPoint_CTFRed_mesh->loadMeshFromVoxelData(vd);
	setPoint_CTFRed_mesh->setMaterialSet(SRender::getSingleton()->createMaterialSet(L"../Resources/Meshes/RawVox/Editor/Textures/Red"));
	delete vd;
	vd = 0;

	setPoint_CTFBlue_mesh = SRender::getSingleton()->createMesh();
	vd = new ps::VoxelData(L"../Resources/Meshes/RawVox/Editor/SpawnModelwHoopFlag.psm.rv");
	setPoint_CTFBlue_mesh->loadMeshFromVoxelData(vd);
	setPoint_CTFBlue_mesh->setMaterialSet(SRender::getSingleton()->createMaterialSet(L"../Resources/Meshes/RawVox/Editor/Textures/Blue"));
	delete vd;
	vd = 0;

	//SECTOR CONTROL
	setPoint_SC_A_mesh = SRender::getSingleton()->createMesh();
	vd = new ps::VoxelData(L"../Resources/Meshes/RawVox/Editor/SpawnModelwHoopA.psm.rv");
	setPoint_SC_A_mesh->loadMeshFromVoxelData(vd);
	setPoint_SC_A_mesh->setMaterialSet(SRender::getSingleton()->createMaterialSet(L"../Resources/Meshes/RawVox/Editor/Textures/Neutral"));
	delete vd;
	vd = 0;

	setPoint_SC_B_mesh = SRender::getSingleton()->createMesh();
	vd = new ps::VoxelData(L"../Resources/Meshes/RawVox/Editor/SpawnModelwHoopB.psm.rv");
	setPoint_SC_B_mesh->loadMeshFromVoxelData(vd);
	setPoint_SC_B_mesh->setMaterialSet(SRender::getSingleton()->createMaterialSet(L"../Resources/Meshes/RawVox/Editor/Textures/Neutral"));
	delete vd;
	vd = 0;

	setPoint_SC_C_mesh = SRender::getSingleton()->createMesh();
	vd = new ps::VoxelData(L"../Resources/Meshes/RawVox/Editor/SpawnModelwHoopC.psm.rv");
	setPoint_SC_C_mesh->loadMeshFromVoxelData(vd);
	setPoint_SC_C_mesh->setMaterialSet(SRender::getSingleton()->createMaterialSet(L"../Resources/Meshes/RawVox/Editor/Textures/Neutral"));
	delete vd;
	vd = 0;

	setPoint_selected_mesh = SRender::getSingleton()->createLineBoxMesh();
	setPoint_selected_mesh->loadBox(0xffffffff);

	return true;
}

/* Some SET POINT STUFF */
pse::SetPoint::SetPoint(float x, float y, float z, int use)
{
	this->x = x;
	this->y = y;
	this->z = z;

	gravityField = 2;
	rotation = 0.0f;
	handle = SRender::getSingleton()->createS3D();
	handle->setScale(0.05f);
	handle->setPosition(this->x, this->y, this->z);

	selectedHandle = SRender::getSingleton()->createS3D();
	selectedHandle->setPosition(this->x, this->y, this->z);
	selectedHandle->setMesh(setPoint_selected_mesh);
	setUse(this->use);
}

pse::SetPoint::~SetPoint(void)
{
	SRender::getSingleton()->releaseS3D(handle);
	SRender::getSingleton()->releaseS3D(selectedHandle);
	handle = 0;
	selectedHandle = 0;
}
void pse::SetPoint::setUse(int use)
{
	this->use = use;

	//here, we set the mesh accordingly
	if (use == PSE_SETPOINTS_USE_SPWN1) handle->setMesh(setPoint_spwn1_mesh);
	if (use == PSE_SETPOINTS_USE_SPWN2) handle->setMesh(setPoint_spwn2_mesh);

	if (use == PSE_SETPOINTS_USE_SCA) handle->setMesh(setPoint_SC_A_mesh);
	if (use == PSE_SETPOINTS_USE_SCB) handle->setMesh(setPoint_SC_B_mesh);
	if (use == PSE_SETPOINTS_USE_SCC) handle->setMesh(setPoint_SC_C_mesh);

	if (use == PSE_SETPOINTS_USE_CTF1) handle->setMesh(setPoint_CTFRed_mesh);
	if (use == PSE_SETPOINTS_USE_CTF2) handle->setMesh(setPoint_CTFBlue_mesh);
}

void pse::SetPoint::setSelected(bool selected)
{
	selectedHandle->setVisible(selected);
}
void pse::SetPoint::syncGraphic(void)
{
	handle->setPosition(x, y, z);
	selectedHandle->setPosition(x, y, z);
	setUse(use);

	switch (gravityField)
	{
	case 0:
		handle->setXRotation(0.0f);
		handle->setYRotation(rotation);
		handle->setZRotation(-3.14159f / 2.0f);
		break;
	case 1:
		handle->setXRotation(0.0f);
		handle->setYRotation(rotation);
		handle->setZRotation(3.14159f / 2.0f);
		break;
	case 2:
		handle->setXRotation(0.0f);
		handle->setYRotation(rotation);
		handle->setZRotation(0.0f);
		break;
	case 3:
		handle->setXRotation(3.14159f);
		handle->setYRotation(rotation);
		handle->setZRotation(0.0f);
		break;
	case 4:
		handle->setXRotation(-3.14159f / 2.0f);
		handle->setYRotation(rotation);
		handle->setZRotation(0.0f);
		break;
	case 5:
		handle->setXRotation(3.14159f / 2.0f);
		handle->setYRotation(rotation);
		handle->setZRotation(0.0f);
		break;
	default:
		break;
	}
}

/* Some editor Light Struct stuff */
pse::Light::Light(void)
{
}
pse::Light::~Light(void)
{
	SRender::getSingleton()->releaseLight(light);
	light = 0;
	SRender::getSingleton()->releaseS3D(handle);
	handle = 0;
}