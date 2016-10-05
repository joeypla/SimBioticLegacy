#include "psGui.h"
#include "pseBase.h"
#include <fstream>
static psGui::Frame* frame = 0;
static psGui::TextField* loadTargetTextField = 0;

static void loadClick(void);
static void cancelClick(void);

static bool isOpened = false;
psGui::Frame* pse::getLoadFrame(void)
{
	return frame;
}

void pse::showLoadDialog(void)
{
	isOpened = true;
	frame->setVisible(isOpened);
}
void pse::setupLoad(void)
{
	using namespace psGui;
	frame = new Frame(-0.2f, 0.5f, 0.4f, 0.3f);
	frame->setColor(0x8f2f2f2f);

	Label* loadLabel = new Label();
	loadLabel->setTextColor(0xcfffffff);
	loadLabel->setSize(0.35f, 0.05f);
	loadLabel->setRelativePosition(0.0f, 0.1f);
	loadLabel->setTextAlignment(DT_VCENTER | DT_LEFT);
	loadLabel->setTextSize(5);
	loadLabel->setText(L"Load File From Path");

	loadTargetTextField = new TextField();
	loadTargetTextField->setSize(0.37f, 0.05f);
	loadTargetTextField->setFieldColor(0x7f2f2f2f);
	loadTargetTextField->setText(L"<Path>");
	loadTargetTextField->setTextSize(4);
	loadTargetTextField->setTextColor(0xcfffffff);
	loadTargetTextField->setRelativePosition(0.0f, 0.0f);
	loadTargetTextField->setActive(true);

	SimpleButton* loadButton = new SimpleButton();  
	loadButton->setCaption(L"Load File");
	loadButton->setCaptionSize(5);
	loadButton->setSize(0.15f, 0.04f);
	loadButton->setColor(0x7f2f2f2f);
	loadButton->setRelativePosition(0.1f, -0.1f);
	loadButton->setFunctionPointer(loadClick);

	SimpleButton* cancelButton = new SimpleButton();
	cancelButton->setCaption(L"Cancel");
	cancelButton->setCaptionSize(5);
	cancelButton->setSize(0.15f, 0.04f);
	cancelButton->setColor(0x7f2f2f2f);
	cancelButton->setRelativePosition(-0.1f, -0.1f);
	cancelButton->setFunctionPointer(cancelClick);

	frame->addComponent(loadLabel);
	frame->addComponent(loadButton);
	frame->addComponent(cancelButton);
	frame->addComponent(loadTargetTextField);
	frame->setPosition(-0.3f, 0.5f);
	frame->setVisible(isOpened);
}
void cancelClick(void)
{
	isOpened = false;
	frame->setVisible(false);
	
}

void loadClick(void)
{
	pse::WorldData* wd = pse::getWorldData();
	int xDim, yDim, zDim;
	int lightCount = 0;
	int setPointCount = 0;

	std::ifstream file;
	file.open(loadTargetTextField->getText().c_str(), std::ios::binary);
	if (!file.is_open()) return;
	/* read map proper name */
	std::stringstream ss;
	unsigned int mapNameSize = 0;
	char properName[100];
	file.read((char*)&mapNameSize, sizeof(int));
	file.read((char*)&properName, mapNameSize);
	for (int i = 0; i < mapNameSize; i++)
	{
		ss<<properName[i];
	}
	wd->properName = ss.str().c_str();

	/* read texture path */
	std::stringstream textureStringStream;
	unsigned int texturePathSize = 0;
	char texturePath[100];
	file.read((char*)&texturePathSize, sizeof(int));
	file.read((char*)&texturePath, texturePathSize);
	for (int i = 0; i < texturePathSize; i++)
	{
		textureStringStream<<texturePath[i];
	}
	wd->texturePath = textureStringStream.str().c_str();

	pse::generalSyncFields();
	/* read voxel count */
	file.read((char*)&xDim, sizeof(int));
	file.read((char*)&yDim, sizeof(int));
	file.read((char*)&zDim, sizeof(int));

	
	wd->xDim = xDim;
	wd->yDim = yDim;
	wd->zDim = zDim;

	/*read global atmospherics */
	file.read((char*)&wd->ambientRed, sizeof(float));
	file.read((char*)&wd->ambientGreen, sizeof(float));
	file.read((char*)&wd->ambientBlue, sizeof(float));

	file.read((char*)&wd->fogRed, sizeof(float));
	file.read((char*)&wd->fogGreen, sizeof(float));
	file.read((char*)&wd->fogBlue, sizeof(float));

	file.read((char*)&wd->fogStart, sizeof(float));
	file.read((char*)&wd->fogEnd, sizeof(float));

	/*read light count */
	file.read((char*)&lightCount, sizeof(int));
	/*read setPoint count */
	file.read((char*)&setPointCount, sizeof(int));
	/*read raw voxel data*/
	//release previous voxel data
	delete wd->voxelData;
	wd->voxelData = new ps::VoxelData(xDim, yDim, zDim);
	for (int z = 0; z<zDim; z++)
	{
		for (int y = 0; y<yDim; y++)
		{
			for (int x = 0; x<xDim; x++)
			{
				char data = file.get();
				wd->voxelData->setData(data, x, y, z);
			}
		}
	}
	//LIGHTS
	//First clear all lights in structure
	for (int i = 0; i < PSE_LIGHTS_MAX; i++)
	{
		if (wd->lights[i] != 0)
		{
			//first clear members -_- i know im an idiot for this
			
			delete wd->lights[i];
			wd->lights[i] = 0;
		}
	}

	for (int i = 0; i < lightCount; i++){
		wd->lights[i] = new pse::Light();
		wd->lights[i]->light = SRender::getSingleton()->createLight();
		wd->lights[i]->handle = SRender::getSingleton()->createS3D();
		//POSITION
		file.read((char*)&wd->lights[i]->light->x, sizeof(float));
		file.read((char*)&wd->lights[i]->light->y, sizeof(float));
		file.read((char*)&wd->lights[i]->light->z, sizeof(float));

		//DIFFUSE
		file.read((char*)&wd->lights[i]->light->r, sizeof(float));
		file.read((char*)&wd->lights[i]->light->g, sizeof(float));
		file.read((char*)&wd->lights[i]->light->b, sizeof(float));

		
		//AMBIENT
		file.read((char*)&wd->lights[i]->light->ambientFactor, sizeof(float));

		
		//radius
		file.read((char*)&wd->lights[i]->light->radius, sizeof(float));

		//SPOTLIGHT SHIT
		file.read((char*)&wd->lights[i]->light->dx, sizeof(float));
		file.read((char*)&wd->lights[i]->light->dy, sizeof(float));
		file.read((char*)&wd->lights[i]->light->dz, sizeof(float));

		file.read((char*)&wd->lights[i]->light->type, sizeof(int));
		file.read((char*)&wd->lights[i]->light->phi, sizeof(float));
		file.read((char*)&wd->lights[i]->light->theta, sizeof(float));
		//sync up handle
		wd->lights[i]->handle->setPosition(wd->lights[i]->light->x, wd->lights[i]->light->y, wd->lights[i]->light->z);
	}
	pse::setAllLightsUnselected();

	//SET POINTS
	for (int i = 0; i < PSE_SETPOINTS_MAX; i++)
	{
		if (wd->setPoints[i] != 0)
		{
			delete wd->setPoints[i];
			wd->setPoints[i] = 0;
		}
	}

	for (int i = 0; i < setPointCount; i++)
	{
		wd->setPoints[i] = new pse::SetPoint(0.0f, 0.0f, 0.0f, 0);

		file.read((char*)&wd->setPoints[i]->x, sizeof(float));
		file.read((char*)&wd->setPoints[i]->y, sizeof(float));
		file.read((char*)&wd->setPoints[i]->z, sizeof(float));

		file.read((char*)&wd->setPoints[i]->gravityField, sizeof(int));
		file.read((char*)&wd->setPoints[i]->use, sizeof(int));
		file.read((char*)&wd->setPoints[i]->rotation, sizeof(float));

		wd->setPoints[i]->setSelected(false);
		wd->setPoints[i]->setUse(wd->setPoints[i]->use);
		wd->setPoints[i]->syncGraphic();
	}
	

	//then update all
	wd->voxelMesh->updateGeometry(wd->voxelData);
	pse::updateAtmosphericsFromWorldData();
	file.close();

	pse::updateTargetFilePath(loadTargetTextField->getText());

	isOpened = false;
	frame->setVisible(isOpened);
}

bool pse::getLoadOpened(void)
{
	return isOpened;
}

psGui::TextField* pse::getLoadTextField(void)
{
	return loadTargetTextField; 
}