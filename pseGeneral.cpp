#include "pseBase.h"

static psGui::Frame* frame = 0;


static psGui::TextField* targetFileField = 0;
static psGui::TextField* properNameField = 0;
static void saveClick(void);
static void loadClick(void);
static void exportRawVoxelDataClick(void);
/*stuff for voxel import tex */
static psGui::TextField* importTextureField = 0;
static psGui::Label* importTextureLabel = 0;
static void importTexturesClick(void);
/* all stuff for the voxel selection box */
static psGui::Simple* voxelSelectingSimple = 0;
static psGui::Simple* voxelSelectedSimple = 0;
static const float voxelSelectorPosY = -0.6f;
static psGui::Simple* voxelSelectionBox = 0;

static psGui::SimpleButton* exportRawVoxelDataButton = 0;
static psGui::Label* dataActionNotificationLabel = 0;
/*-----------------------------------*/

static psGui::Label* setPointLabel = 0;

#define PSE_SETPOINTS_TYPES_BUTTONS 7

static psGui::SimpleButton** setPointButtons = 0;
static void updateSetPointTypeSelection(void);
static void updateSetPointSelection(void);
static psGui::SimpleButton* setPointAddButton = 0;
static psGui::SimpleButton* setPointRemoveButton = 0;
static psGui::SimpleButton* setPointGridLockButton = 0;

static void SPWN1_Click(void);
static void SPWN2_Click(void);
static void SC_A_Click(void);
static void SC_B_Click(void);
static void SC_C_Click(void);
static void CTF1_Click(void);
static void CTF2_Click(void);

static void setPointAddClick(void);
static void setPointRemoveClick(void);
static void setPointGridLockClick(void);
static int getSetPointFromClick(int x, int y);
static void setAllSetPointsUnselected(void);

static void setupFrame(void);

psGui::Frame* pse::getGeneralFrame(void)
{
	return frame;
}
void pse::setupGeneral(void)
{
	setupFrame();
}

void setupFrame(void)
{
	using namespace psGui;
	frame = new Frame(-0.75f, 0.0f, 0.4f, 1.9f);
	frame->setColor(0x7f3f3f3f);

	Label* dataLabel = new Label();
	dataLabel->setTextColor(0xcfffffff);
	dataLabel->setSize(0.4f, 0.05f);
	dataLabel->setRelativePosition(0.0f, 0.9f);
	dataLabel->setTextAlignment(DT_VCENTER | DT_CENTER);
	dataLabel->setTextSize(6);
	dataLabel->setText(L"Data Managing");

	Label* filenameLabel = new Label();
	filenameLabel->setTextColor(0xcfffffff);
	filenameLabel->setSize(0.35f, 0.05f);
	filenameLabel->setRelativePosition(0.0f, 0.85f);
	filenameLabel->setTextAlignment(DT_VCENTER | DT_LEFT);
	filenameLabel->setTextSize(5);
	filenameLabel->setText(L"Target File");

	targetFileField = new TextField();
	targetFileField->setSize(0.37f, 0.05f);
	targetFileField->setFieldColor(0x7f2f2f2f);
	targetFileField->setText(L"../Resources/Levels/Default/default.psm");
	targetFileField->setTextSize(4);
	targetFileField->setTextColor(0xcfffffff);
	targetFileField->setRelativePosition(0.0f, 0.8f);

	Label* si_properName = new Label();
	si_properName->setTextColor(0xcfffffff);
	si_properName->setSize(0.37f, 0.05f);
	si_properName->setRelativePosition(0.0f, 0.7f);
	si_properName->setTextAlignment(DT_VCENTER | DT_LEFT);
	si_properName->setTextSize(5);
	si_properName->setText(L"Official Name");

	properNameField = new TextField();
	properNameField->setSize(0.2f, 0.05f);
	properNameField->setFieldColor(0x7f2f2f2f);
	properNameField->setText(L"MyMap");
	properNameField->setTextSize(4);
	properNameField->setTextColor(0xcfffffff);
	properNameField->setRelativePosition(0.085f, 0.7f);

	Label* si_voxelDimensions = new Label();
	si_voxelDimensions->setTextColor(0xcfffffff);
	si_voxelDimensions->setSize(0.37f, 0.05f);
	si_voxelDimensions->setRelativePosition(0.0f, 0.65f);
	si_voxelDimensions->setTextAlignment(DT_VCENTER | DT_LEFT);
	si_voxelDimensions->setTextSize(5);
	si_voxelDimensions->setText(L"Voxel Dimensions");

	Label* si_lightCount = new Label();
	si_lightCount->setTextColor(0xcfffffff);
	si_lightCount->setSize(0.37f, 0.05f);
	si_lightCount->setRelativePosition(0.0f, 0.60f);
	si_lightCount->setTextAlignment(DT_VCENTER | DT_LEFT);
	si_lightCount->setTextSize(5);
	si_lightCount->setText(L"Total Light Count");

	SimpleButton* saveButton = new SimpleButton();
	saveButton->setCaption(L"Save Map");
	saveButton->setCaptionSize(5);
	saveButton->setSize(0.15f, 0.04f);
	saveButton->setColor(0x9f0f0f0f);
	saveButton->setRelativePosition(0.11f, 0.5f);
	saveButton->setFunctionPointer(saveClick);

	SimpleButton* loadButton = new SimpleButton();
	loadButton->setCaption(L"Load File...");
	loadButton->setCaptionSize(5);
	loadButton->setSize(0.15f, 0.04f);
	loadButton->setColor(0x9f0f0f0f);
	loadButton->setRelativePosition(-0.11f, 0.5f);
	loadButton->setFunctionPointer(loadClick);

	exportRawVoxelDataButton = new SimpleButton();
	exportRawVoxelDataButton->setCaption(L"Save RawVox");
	exportRawVoxelDataButton->setCaptionSize(5);
	exportRawVoxelDataButton->setSize(0.15f, 0.04f);
	exportRawVoxelDataButton->setColor(0x9f0f0f0f);
	exportRawVoxelDataButton->setRelativePosition(0.11f, 0.4f);
	exportRawVoxelDataButton->setFunctionPointer(exportRawVoxelDataClick);

	dataActionNotificationLabel = new Label();
	dataActionNotificationLabel->setTextColor(0xcfffffff);
	dataActionNotificationLabel->setSize(0.35f, 0.05f);
	dataActionNotificationLabel->setRelativePosition(0.0f, 0.4f);
	dataActionNotificationLabel->setTextAlignment(DT_VCENTER | DT_LEFT);
	dataActionNotificationLabel->setTextSize(5);
	dataActionNotificationLabel->setText(L"Map Saved.");

	setPointLabel = new Label();
	setPointLabel->setTextColor(0xcfffffff);
	setPointLabel->setSize(0.35f, 0.05f);
	setPointLabel->setRelativePosition(0.0f, 0.35f);
	setPointLabel->setTextAlignment(DT_VCENTER | DT_CENTER);
	setPointLabel->setTextSize(5);
	setPointLabel->setText(L"Set Point Editing.");

	setPointButtons = new psGui::SimpleButton*[PSE_SETPOINTS_TYPES_BUTTONS];
	for (int i = 0; i < PSE_SETPOINTS_TYPES_BUTTONS; i++)
	{
		setPointButtons[i] = new psGui::SimpleButton();
		setPointButtons[i]->setCaptionSize(4);
		setPointButtons[i]->setSize(0.06f, 0.03f);
		setPointButtons[i]->setColor(0x9f0f0f0f);
	}

	setPointButtons[0]->setCaption(L"SPWN1");
	setPointButtons[0]->setRelativePosition(-0.15f, 0.32f);
	setPointButtons[0]->setFunctionPointer(SPWN1_Click);

	
	setPointButtons[1]->setCaption(L"SPWN2");
	setPointButtons[1]->setRelativePosition(-0.15f, 0.28f);
	setPointButtons[1]->setFunctionPointer(SPWN2_Click);

	setPointButtons[2]->setCaption(L"SC_A");
	setPointButtons[2]->setRelativePosition(-0.15f, 0.24f);
	setPointButtons[2]->setFunctionPointer(SC_A_Click);

	setPointButtons[3]->setCaption(L"SC_B");
	setPointButtons[3]->setRelativePosition(-0.15f, 0.20f);
	setPointButtons[3]->setFunctionPointer(SC_B_Click);

	setPointButtons[4]->setCaption(L"SC_C");
	setPointButtons[4]->setRelativePosition(-0.15f, 0.16f);
	setPointButtons[4]->setFunctionPointer(SC_C_Click);

	setPointButtons[5]->setCaption(L"CTF_1");
	setPointButtons[5]->setRelativePosition(-0.15f, 0.11f);
	setPointButtons[5]->setFunctionPointer(CTF1_Click);

	setPointButtons[6]->setCaption(L"CTF_2");
	setPointButtons[6]->setRelativePosition(-0.15f, 0.07f);
	setPointButtons[6]->setFunctionPointer(CTF2_Click);


	setPointAddButton = new psGui::SimpleButton();
	setPointAddButton->setCaptionSize(4);
	setPointAddButton->setSize(0.09f, 0.05f);
	setPointAddButton->setColor(0x9f0f0f0f);
	setPointAddButton->setCaption(L"Place");
	setPointAddButton->setRelativePosition(-0.15f, 0.0f);
	setPointAddButton->setFunctionPointer(setPointAddClick);

	setPointRemoveButton = new psGui::SimpleButton();
	setPointRemoveButton->setCaptionSize(4);
	setPointRemoveButton->setSize(0.09f, 0.05f);
	setPointRemoveButton->setColor(0x9f0f0f0f);
	setPointRemoveButton->setCaption(L"Remove Selected");
	setPointRemoveButton->setRelativePosition(0.15f, 0.0f);
	setPointRemoveButton->setFunctionPointer(setPointRemoveClick);

	setPointGridLockButton = new psGui::SimpleButton();
	setPointGridLockButton->setCaptionSize(4);
	setPointGridLockButton->setSize(0.09f, 0.05f);
	setPointGridLockButton->setColor(0x9f0f0f0f);
	setPointGridLockButton->setCaption(L"Grid Lock");
	setPointGridLockButton->setRelativePosition(0.0f, 0.0f);
	setPointGridLockButton->setFunctionPointer(setPointGridLockClick);

	/* VOXEL TEXTURE BULLSHIT*/
	psGui::Label* voxelTextureLabel = new Label();
	voxelTextureLabel->setTextColor(0xcfffffff);
	voxelTextureLabel->setSize(0.37f, 0.04f);
	voxelTextureLabel->setRelativePosition(0.0f, -0.15f);
	voxelTextureLabel->setTextAlignment(DT_VCENTER | DT_CENTER);
	voxelTextureLabel->setText(L"Voxel Texturing");

	importTextureField = new TextField();
	importTextureField->setSize(0.37f, 0.05f);
	importTextureField->setFieldColor(0x7f2f2f2f);
	importTextureField->setText(L"../Resources/Levels/Default/Textures");
	importTextureField->setTextSize(4);
	importTextureField->setTextColor(0xcfffffff);
	importTextureField->setRelativePosition(0.0f, -0.2f);
	SimpleButton* loadTextureButton = new SimpleButton();
	loadTextureButton->setCaption(L"Import Textures");
	loadTextureButton->setCaptionSize(4);
	loadTextureButton->setSize(0.15f, 0.04f);
	loadTextureButton->setColor(0x7f2f2f2f);
	loadTextureButton->setRelativePosition(-0.11f, -0.26f);
	loadTextureButton->setFunctionPointer(importTexturesClick);
	importTextureLabel = new Label();
	importTextureLabel->setTextColor(0xcfffffff);
	importTextureLabel->setSize(0.37f, 0.04f);
	importTextureLabel->setRelativePosition(0.0f, -0.26f);
	importTextureLabel->setTextAlignment(DT_VCENTER | DT_RIGHT);
	importTextureLabel->setText(L"64 Textures imported");
	
	/* All this code here is for the voxel selection box.
	I was thinking of making my own component for it, but for
	now, fuck it. Everything is sort makeshift and shitty,
	but its going to work very well when finished. Not too scalable
	though :( nor is it in the least bit maintainable. */
	float aspectRatio = (float)SRender::getSingleton()->getWidth() / (float)SRender::getSingleton()->getHeight();
	voxelSelectionBox = new Simple();
	voxelSelectionBox->setColor(0xffffffff);
	voxelSelectionBox->setSize(0.6f / aspectRatio, 0.6f);
	voxelSelectionBox->setRelativePosition(0.0f, voxelSelectorPosY);
	voxelSelectionBox->x_SetTexture(pse::getWorldData()->voxelMesh->x_GetAtlas());

	for (int i = 0; i < 9; i++)
	{
		Simple* s = new Simple();
		s->setColor(0xaf1f1f1f);
		s->setSize(0.005f, 0.6f);
		s->setRelativePosition((-0.3f / aspectRatio) + i*(0.6f / aspectRatio / 8.0f), voxelSelectorPosY);
		frame->addComponent(s);
	}

	for (int i = 0; i < 9; i++)
	{
		Simple* s = new Simple();
		s->setColor(0xaf1f1f1f);
		s->setSize(0.6f/aspectRatio, 0.005f);
		s->setRelativePosition(0.0f, (voxelSelectorPosY - 0.3f) + i*(0.6f / 8.0f));
		frame->addComponent(s);
	}

	voxelSelectingSimple = new Simple();
	voxelSelectingSimple->setColor(0x5fffff00);
	voxelSelectingSimple->setSize(0.6f / aspectRatio / 8.0f, 0.6f /8.0f);
	voxelSelectingSimple->setRelativePosition(0.0f, -0.6f);

	voxelSelectedSimple = new Simple();
	voxelSelectedSimple->setColor(0x4fffff00);
	voxelSelectedSimple->setSize(0.6f / aspectRatio / 8.0f, 0.6f / 8.0f);
	voxelSelectedSimple->setVisible(false);

	frame->addComponent(dataLabel);
	frame->addComponent(filenameLabel);
	frame->addComponent(targetFileField);
	frame->addComponent(si_properName);
	frame->addComponent(properNameField);
	frame->addComponent(si_voxelDimensions);
	frame->addComponent(si_lightCount);
	frame->addComponent(saveButton);
	frame->addComponent(loadButton);
	frame->addComponent(exportRawVoxelDataButton);
	frame->addComponent(dataActionNotificationLabel);
	frame->addComponent(setPointLabel);
	for (int i = 0; i < PSE_SETPOINTS_TYPES_BUTTONS; i++) frame->addComponent(setPointButtons[i]);
	frame->addComponent(setPointAddButton);
	frame->addComponent(setPointRemoveButton);
	frame->addComponent(setPointGridLockButton);
	frame->addComponent(voxelTextureLabel);
	frame->addComponent(importTextureField);
	frame->addComponent(loadTextureButton);
	frame->addComponent(importTextureLabel);

	frame->addComponent(voxelSelectionBox);
	frame->addComponent(voxelSelectedSimple);
	frame->addComponent(voxelSelectingSimple);
	frame->setPosition(-0.75f, 0.0f);

	//so the textures match what is in the field
	importTexturesClick();
}
#include "SInput.h"
#include "SLOG.h"
void pse::updateGeneral(void)
{
	frame->update();
	//check if voxel selecting
	if (frame->getMouseWithinFrame())
	{
		float aspectRatio = (float)SRender::getSingleton()->getWidth() / (float)SRender::getSingleton()->getHeight();
		float screenWidth = SRender::getSingleton()->getWidth();
		float screenHeight = SRender::getSingleton()->getHeight();
		float x = (float)S_INPUT::getMouseX() / screenWidth * 2.0f - 1.0f;
		float y = (-1.0f) * ((float)S_INPUT::getMouseY() / screenHeight * 2.0f - 1.0f);
		if (voxelSelectionBox->getMouseWithin())
		{
			voxelSelectingSimple->setVisible(true);
			int xIn = 5;
			int yIn = 1;

			for (int xi = 0; xi < 8; xi++)
			{
				if (x > -0.75f - (0.3f / aspectRatio) + xi*(0.6f / aspectRatio / 8.0f) &&
					x < -0.75f - (0.3f / aspectRatio) + (xi+1)*(0.6f / aspectRatio / 8.0f))
				{
					xIn = xi+1;
					break;
				}
			}

			for (int yi = 0; yi < 8; yi++)
			{
				if (y < (voxelSelectorPosY + 0.3f) - yi*(0.6f / 8.0f) &&
					y > (voxelSelectorPosY + 0.3f) - (yi+1)*(0.6f / 8.0f))
				{
					yIn = yi+1;
					break;
				}
			}

			int selection = xIn + (8*(yIn-1));
			voxelSelectingSimple->setRelativePosition((-0.3f / aspectRatio) + (xIn-1)*(0.6f / aspectRatio / 8.0f) + (0.6f / aspectRatio / 8.0f / 2.0f), voxelSelectorPosY + 0.3f - yIn*0.6f/8.0f + 0.6f / 8.0f / 2.0f);
			frame->setPosition(-0.75f, 0.0f);

			if (S_INPUT::mousePressed(0))
			{
				pse::getEditorData()->typeSelection = selection;
				voxelSelectedSimple->setVisible(true);
				voxelSelectedSimple->setRelativePosition((-0.3f / aspectRatio) + (xIn-1)*(0.6f / aspectRatio / 8.0f) + (0.6f / aspectRatio / 8.0f / 2.0f), voxelSelectorPosY + 0.3f - yIn*0.6f/8.0f + 0.6f / 8.0f / 2.0f);
				frame->setPosition(-0.75f, 0.0f);
			}
		}
		else {
			voxelSelectingSimple->setVisible(false);
		}
	} else {
		voxelSelectingSimple->setVisible(false);
	}

	//update edit box active states
	if (S_INPUT::mousePressed(0))
	{
		targetFileField->setActive(false);
		targetFileField->setFieldColor(0x7f2f2f2f);
		properNameField->setActive(false);
		properNameField->setFieldColor(0x7f2f2f2f);
		importTextureField->setActive(false);
		importTextureField->setFieldColor(0x7f2f2f2f);
		if (targetFileField->getMouseWithin())
		{
			targetFileField->setActive(true);
			targetFileField->setFieldColor(0x7f5f5f5f);
		}
		if (properNameField->getMouseWithin())
		{
			properNameField->setActive(true);
			properNameField->setFieldColor(0x7f5f5f5f);
		}
		if (importTextureField->getMouseWithin())
		{
			importTextureField->setActive(true);
			importTextureField->setFieldColor(0x7f5f5f5f);
		}
	}

	updateSetPointSelection();
	updateSetPointTypeSelection();

	/* update set point rotation */

	if (S_INPUT::keyPressed(DIK_J))
	{
		if (pse::getEditorData()->setPointSelection >= 0 && pse::getEditorData()->setPointSelection < PSE_SETPOINTS_MAX)
		{
			if (pse::getWorldData()->setPoints[pse::getEditorData()->setPointSelection] != 0)
			{
				pse::getWorldData()->setPoints[pse::getEditorData()->setPointSelection]->rotation += 0.05f;
				pse::getWorldData()->setPoints[pse::getEditorData()->setPointSelection]->syncGraphic();
			}
		}
	}

	if (S_INPUT::keyPressed(DIK_K))
	{
		if (pse::getEditorData()->setPointSelection >= 0 && pse::getEditorData()->setPointSelection < PSE_SETPOINTS_MAX)
		{
			if (pse::getWorldData()->setPoints[pse::getEditorData()->setPointSelection] != 0)
			{
				pse::getWorldData()->setPoints[pse::getEditorData()->setPointSelection]->rotation -= 0.05f;
				pse::getWorldData()->setPoints[pse::getEditorData()->setPointSelection]->syncGraphic();
			}
		}
	}
}

void importTexturesClick(void)
{
	int count = pse::getWorldData()->voxelMesh->createAtlasFromPath(importTextureField->getText().c_str());
	static std::wstringstream wss;
	wss.str(L"");
	wss<<count;
	wss<<L" Textures Imported";
	//importTextureLabel->setText(wss.str().c_str());
	voxelSelectionBox->x_SetTexture(pse::getWorldData()->voxelMesh->x_GetAtlas());
}

void saveClick(void)
{
	pse::WorldData* wd = pse::getWorldData();
	std::ofstream file;
	file.open(targetFileField->getText().c_str(), std::ios::trunc | std::ios::binary);
	/* map official name */
	{
	std::wstring firstString = properNameField->getText();
	std::string converted(firstString.begin(), firstString.end()); 
	unsigned int properNameSize = converted.size();
	file.write(reinterpret_cast<const char*>(&properNameSize), sizeof(int));
	file.write(converted.c_str(), converted.size());
	}
	/* map texture path */
	{
	std::wstring firstString = importTextureField->getText();
	std::string converted(firstString.begin(), firstString.end());
	unsigned int pathSize = converted.size();
	file.write(reinterpret_cast<const char*>(&pathSize), sizeof(int)); 
	file.write(converted.c_str(), converted.size());
	}
	/* map 
	/*map dimensions*/
	file.write(reinterpret_cast<const char*>(&wd->xDim), sizeof(int));
	file.write(reinterpret_cast<const char*>(&wd->yDim), sizeof(int));
	file.write(reinterpret_cast<const char*>(&wd->zDim), sizeof(int));
	/*global atmospherics */
	file.write(reinterpret_cast<const char*>(&wd->ambientRed), sizeof(float));
	file.write(reinterpret_cast<const char*>(&wd->ambientGreen), sizeof(float));
	file.write(reinterpret_cast<const char*>(&wd->ambientBlue), sizeof(float));

	file.write(reinterpret_cast<const char*>(&wd->fogRed), sizeof(float));
	file.write(reinterpret_cast<const char*>(&wd->fogGreen), sizeof(float));
	file.write(reinterpret_cast<const char*>(&wd->fogBlue), sizeof(float));

	file.write(reinterpret_cast<const char*>(&wd->fogStart), sizeof(float));
	file.write(reinterpret_cast<const char*>(&wd->fogEnd), sizeof(float));
	/*number of lights */
	int numLights = 0;
	for (int i = 0; i < PSE_LIGHTS_MAX; i++)
	{
		if (wd->lights[i]!=0)
		{
			numLights++;
		}
	}
	
	file.write(reinterpret_cast<const char*>(&numLights), sizeof(int));

	/*number of setpoints */
	int numSetPoints = 0;
	for (int i = 0; i < PSE_SETPOINTS_MAX; i++)
	{
		if (wd->setPoints[i]!=0)
		{
			numSetPoints++;
		}
	}

	file.write(reinterpret_cast<const char*>(&numSetPoints), sizeof(int));
	/*base lighting and atmospherics */


	/*raw voxel Data*/
	for (int z = 0; z<wd->zDim; z++){
		for (int y = 0; y<wd->yDim; y++){
			for (int x = 0; x<wd->zDim; x++){
				file.put(wd->voxelData->data(x,y,z));
			}
		}
	}

	/* raw lighting data */
	for (int i = 0; i < PSE_LIGHTS_MAX; i++){
		if (wd->lights[i]!=0){
			//POSITION
			file.write(reinterpret_cast<const char*>(&wd->lights[i]->light->x), sizeof(float));
			file.write(reinterpret_cast<const char*>(&wd->lights[i]->light->y), sizeof(float));
			file.write(reinterpret_cast<const char*>(&wd->lights[i]->light->z), sizeof(float));

			//DIFFUSE
			file.write(reinterpret_cast<const char*>(&wd->lights[i]->light->r), sizeof(float));
			file.write(reinterpret_cast<const char*>(&wd->lights[i]->light->g), sizeof(float));
			file.write(reinterpret_cast<const char*>(&wd->lights[i]->light->b), sizeof(float));

			//Ambient
			file.write(reinterpret_cast<const char*>(&wd->lights[i]->light->ambientFactor), sizeof(float));

			//RANGE
			file.write(reinterpret_cast<const char*>(&wd->lights[i]->light->radius), sizeof(float));

			//SPOTLIGHT STUFF
			file.write(reinterpret_cast<const char*>(&wd->lights[i]->light->dx), sizeof(float));
			file.write(reinterpret_cast<const char*>(&wd->lights[i]->light->dy), sizeof(float));
			file.write(reinterpret_cast<const char*>(&wd->lights[i]->light->dz), sizeof(float));

			file.write(reinterpret_cast<const char*>(&wd->lights[i]->light->type), sizeof(int));
			file.write(reinterpret_cast<const char*>(&wd->lights[i]->light->phi), sizeof(float));
			file.write(reinterpret_cast<const char*>(&wd->lights[i]->light->theta), sizeof(float));
		}
	}

	/* raw set point data */
	for (int i = 0; i < PSE_SETPOINTS_MAX; i++)
	{
		if (wd->setPoints[i]!=0)
		{
			file.write(reinterpret_cast<const char*>(&wd->setPoints[i]->x), sizeof(float));
			file.write(reinterpret_cast<const char*>(&wd->setPoints[i]->y), sizeof(float));
			file.write(reinterpret_cast<const char*>(&wd->setPoints[i]->z), sizeof(float));

			file.write(reinterpret_cast<const char*>(&wd->setPoints[i]->gravityField), sizeof(int));
			file.write(reinterpret_cast<const char*>(&wd->setPoints[i]->use), sizeof(int));
			file.write(reinterpret_cast<const char*>(&wd->setPoints[i]->rotation), sizeof(float));
		}
	}
	file.close();
}

void loadClick(void)
{
	pse::showLoadDialog();
}

#include <sstream>
void exportRawVoxelDataClick(void)
{
	pse::WorldData* wd = pse::getWorldData();
	std::ofstream file;
	std::wstringstream ss;
	ss.str(L"");
	ss<<targetFileField->getText().c_str()<<L".rv";
	
	file.open(ss.str().c_str(), std::ios::trunc | std::ios::binary);
	
	/*map dimensions*/
	file.write(reinterpret_cast<const char*>(&wd->xDim), sizeof(int));
	file.write(reinterpret_cast<const char*>(&wd->yDim), sizeof(int));
	file.write(reinterpret_cast<const char*>(&wd->zDim), sizeof(int));
	
	/*raw voxel Data*/
	for (int z = 0; z<wd->zDim; z++){
		for (int y = 0; y<wd->yDim; y++){
			for (int x = 0; x<wd->zDim; x++){
				file.put(wd->voxelData->data(x,y,z));
			}
		}
	}
}

void updateSetPointTypeSelection(void)
{
	//make all the buttons dimmed out so that we only dim the right one later
	DWORD goToColor = 0x9f7f7f7f;
	DWORD dimColor = 0x9f0f0f0f;
	for (int i = 0; i < PSE_SETPOINTS_TYPES_BUTTONS; i++)
	{
		setPointButtons[i]->setColor(dimColor);
	}

	
	switch (pse::getEditorData()->setPointTypeSelection)
	{
	case PSE_SETPOINTS_USE_SPWN1:
		setPointButtons[0]->setColor(goToColor);
		break;
	case PSE_SETPOINTS_USE_SPWN2:
		setPointButtons[1]->setColor(goToColor);
		break;
	case PSE_SETPOINTS_USE_SCA:
		setPointButtons[2]->setColor(goToColor);
		break;
	case PSE_SETPOINTS_USE_SCB:
		setPointButtons[3]->setColor(goToColor);
		break;
	case PSE_SETPOINTS_USE_SCC:
		setPointButtons[4]->setColor(goToColor);
		break;
	case PSE_SETPOINTS_USE_CTF1:
		setPointButtons[5]->setColor(goToColor);
		break;
	case PSE_SETPOINTS_USE_CTF2:
		setPointButtons[6]->setColor(goToColor);
		break;
	default:
		break;
	}
}

void updateSetPointSelection(void)
{
	if (!pse::getGeneralFrame()->getMouseWithinFrame() && !pse::getLAFrame()->getMouseWithinFrame() && !pse::getBlockFrame()->getMouseWithinFrame())
	{
		if (pse::getModuleActive_setPoints() && S_INPUT::mousePressed(0))
		{
			pse::EditorData* ed = pse::getEditorData();
			static float setPointToCam = 5.0f; 
			
			int selection = getSetPointFromClick(S_INPUT::getMouseX(), S_INPUT::getMouseY());
			setAllSetPointsUnselected();
			if (selection != -1)
			{

				// Find the origin and direction of the vector shooting into the screen (from mouseclick)
				D3DXMATRIX matProj = SRender::getSingleton()->getProjection();
				int width = SRender::getSingleton()->getWidth();
				int height = SRender::getSingleton()->getHeight();

				D3DXVECTOR3 v;
				v.x = (((2.0f*S_INPUT::getMouseX())/width)-1.0f)/matProj._11;
				v.y = -(((2.0f*S_INPUT::getMouseY())/height)-1.0f)/matProj._22;
				v.z = 1.0f;

				D3DXMATRIX m, view;
				view = SRender::getSingleton()->getView();
				D3DXMatrixInverse(&m, NULL, &view);

				D3DXVECTOR3 rayDir, rayOrigin;
				rayDir.x = v.x*m._11 + v.y*m._21 + v.z*m._31;
				rayDir.y = v.x*m._12 + v.y*m._22 + v.z*m._32;
				rayDir.z = v.x*m._13 + v.y*m._23 + v.z*m._33;
				rayOrigin.x = m._41;
				rayOrigin.y = m._42;
				rayOrigin.z = m._43;
				//----------------------------------------------------------------------------------------

				ed->setPointSelection = selection;
				pse::SetPoint* setPoint = pse::getWorldData()->setPoints[selection];
				if (setPoint!=0)
				{
					setPoint->x = rayOrigin.x + rayDir.x * setPointToCam;
					setPoint->y = rayOrigin.y + rayDir.y * setPointToCam;
					setPoint->z = rayOrigin.z + rayDir.z * setPointToCam;

					setPoint->syncGraphic();
					setPoint->setSelected(true);

					//then update the type in the menu based on the selection
					if (setPoint->use == 0) SPWN1_Click();
					if (setPoint->use == 1) SPWN2_Click();
					if (setPoint->use == 12) SC_A_Click();
					if (setPoint->use == 13) SC_B_Click();
					if (setPoint->use == 14) SC_C_Click();
					if (setPoint->use == 15) CTF1_Click();
					if (setPoint->use == 16) CTF2_Click();
				}
			}

			
		}
	}
}
void SPWN1_Click(void)
{
	pse::getEditorData()->setPointTypeSelection = 0;
	if (pse::getEditorData()->setPointSelection >= 0 && pse::getEditorData()->setPointSelection < PSE_SETPOINTS_MAX)
	{
		pse::getWorldData()->setPoints[pse::getEditorData()->setPointSelection]->setUse(0);
	}
}

void SPWN2_Click(void)
{
	pse::getEditorData()->setPointTypeSelection = 1;
	if (pse::getEditorData()->setPointSelection >= 0 && pse::getEditorData()->setPointSelection < PSE_SETPOINTS_MAX)
	{
		pse::getWorldData()->setPoints[pse::getEditorData()->setPointSelection]->setUse(1);
	}
}

void SC_A_Click(void)
{
	pse::getEditorData()->setPointTypeSelection = 12;
	if (pse::getEditorData()->setPointSelection >= 0 && pse::getEditorData()->setPointSelection < PSE_SETPOINTS_MAX)
	{
		pse::getWorldData()->setPoints[pse::getEditorData()->setPointSelection]->setUse(12);
	}
}

void SC_B_Click(void)
{
	pse::getEditorData()->setPointTypeSelection = 13;
	if (pse::getEditorData()->setPointSelection >= 0 && pse::getEditorData()->setPointSelection < PSE_SETPOINTS_MAX)
	{
		pse::getWorldData()->setPoints[pse::getEditorData()->setPointSelection]->setUse(13);
	}
}

void SC_C_Click(void)
{
	pse::getEditorData()->setPointTypeSelection = 14;
	if (pse::getEditorData()->setPointSelection >= 0 && pse::getEditorData()->setPointSelection < PSE_SETPOINTS_MAX)
	{
		pse::getWorldData()->setPoints[pse::getEditorData()->setPointSelection]->setUse(14);
	}
}

void CTF1_Click(void)
{
	pse::getEditorData()->setPointTypeSelection = 15;
	if (pse::getEditorData()->setPointSelection >= 0 && pse::getEditorData()->setPointSelection < PSE_SETPOINTS_MAX)
	{
		pse::getWorldData()->setPoints[pse::getEditorData()->setPointSelection]->setUse(15);
	}
}

void CTF2_Click(void)
{
	pse::getEditorData()->setPointTypeSelection = 16;
	if (pse::getEditorData()->setPointSelection >= 0 && pse::getEditorData()->setPointSelection < PSE_SETPOINTS_MAX)
	{
		pse::getWorldData()->setPoints[pse::getEditorData()->setPointSelection]->setUse(16);
	}
}

void setPointAddClick(void)
{
	
	pse::EditorData* ed = pse::getEditorData();
	

	SCamera* cam = ed->editorCam;
	pse::WorldData* wd = pse::getWorldData();
	for (int i = 0; i < PSE_SETPOINTS_MAX; i++)
	{
		if (wd->setPoints[i] == 0)
		{
			setAllSetPointsUnselected();
			wd->setPoints[i] = new pse::SetPoint(cam->x + cam->dx * 3.0f, cam->y + cam->dy * 3.0f,  cam->z + cam->dz * 3.0f, ed->setPointTypeSelection);
			
 			
			
			wd->setPoints[i]->setUse(ed->setPointTypeSelection);
			wd->setPoints[i]->gravityField = ed->gravityConfig; 
			wd->setPoints[i]->syncGraphic();
			ed->setPointSelection = i;
			break;
		}
	}
}

void setPointRemoveClick(void)
{
	pse::EditorData* ed = pse::getEditorData();
	pse::WorldData* wd = pse::getWorldData();

	int selected = ed->setPointSelection;

	if (selected < 0) return;
	if (selected >= PSE_SETPOINTS_MAX) return;
	if (wd->setPoints[selected] == 0) return;

	delete wd->setPoints[selected];
	wd->setPoints[selected] = 0;
	ed->setPointSelection = -1;
}

void setPointGridLockClick(void)
{
	pse::EditorData* ed = pse::getEditorData();
	pse::WorldData* wd = pse::getWorldData();

	if (ed->setPointSelection < 0) return;
	if (wd->setPoints[ed->setPointSelection] == 0) return;

	pse::SetPoint* sp = wd->setPoints[ed->setPointSelection];

	//x axis
	float xLower = (float)((int)(sp->x - 0.5f)) + 0.5f;
	float xHigher = (float)((int)(sp->x + 0.5f)) + 0.5f;

	float xLowerDifference = sp->x - xLower;
	float xHigherDifference = xHigher - sp->x;

	sp->x = (xLowerDifference > xHigherDifference) ? xHigher : xLower;
	

	//y axis
	float yLower = (float)((int)(sp->y - 0.5f)) + 0.5f;
	float yHigher = (float)((int)(sp->y + 0.5f)) + 0.5f;

	float yLowerDifference = sp->y - yLower;
	float yHigherDifference = yHigher - sp->y;

	sp->y = (yLowerDifference > yHigherDifference) ? yHigher : yLower;


	//z axis
	float zLower = (float)((int)(sp->z - 0.5f)) + 0.5f;
	float zHigher = (float)((int)(sp->z + 0.5f)) + 0.5f;

	float zLowerDifference = sp->z - zLower;
	float zHigherDifference = zHigher - sp->z;

	sp->z = (zLowerDifference > zHigherDifference) ? zHigher : zLower;



	sp->handle->setPosition(sp->x, sp->y, sp->z);
	sp->selectedHandle->setPosition(sp->x, sp->y, sp->z);
}
#include "psPhysics.h"
int getSetPointFromClick(int x, int y)
{
	D3DXMATRIX matProj = SRender::getSingleton()->getProjection();
	int width = SRender::getSingleton()->getWidth();
	int height = SRender::getSingleton()->getHeight();

	D3DXVECTOR3 v;
	v.x = (((2.0f*x)/width)-1.0f)/matProj._11;
	v.y = -(((2.0f*y)/height)-1.0f)/matProj._22;
	v.z = 1.0f;

	D3DXMATRIX m, view;
	view = SRender::getSingleton()->getView();
	D3DXMatrixInverse(&m, NULL, &view);

	D3DXVECTOR3 rayDir, rayOrigin;
	rayDir.x = v.x*m._11 + v.y*m._21 + v.z*m._31;
	rayDir.y = v.x*m._12 + v.y*m._22 + v.z*m._32;
	rayDir.z = v.x*m._13 + v.y*m._23 + v.z*m._33;
	rayOrigin.x = m._41;
	rayOrigin.y = m._42;
	rayOrigin.z = m._43;

	/* very brute force, but its pretty much instantenous and
	it really doesn't matter. Its called on a click so who fucking
	cares. I'm such a bad programmer :( */
	D3DXVECTOR3 p1 = rayOrigin;
	pse::SetPoint** setPoints = pse::getWorldData()->setPoints;
	for (int i = 0; i < 500; i++)
	{
		p1 += rayDir*0.1f;

		//check for any collision with voxels. If so, get the fuck out.
		if ((int)p1.x >= 0 && (int)p1.x < pse::getWorldData()->voxelData->x())
			if ((int)p1.y >= 0 && (int)p1.y < pse::getWorldData()->voxelData->y())
				if ((int)p1.z >= 0 && (int)p1.z < pse::getWorldData()->voxelData->z())
					if (pse::getWorldData()->voxelData->data(p1.x, p1.y, p1.z)) break;
		for (int j = 0; j < PSE_SETPOINTS_MAX; j++)
		{
			if (setPoints[j] == 0) continue;

			if (psPhysics::isPointWithinBox(p1, D3DXVECTOR3(setPoints[j]->x, setPoints[j]->y, setPoints[j]->z), D3DXVECTOR3(1.0f, 1.0f, 1.0f)))
			{
				return j;
			}
		}
	}
	//nothing clicked, return -1
	return -1;
}

void setAllSetPointsUnselected(void)
{
	pse::WorldData* wd = pse::getWorldData();
	for (int i = 0; i < PSE_SETPOINTS_MAX; i++)
	{
		if (wd->setPoints[i] != 0)
		{
			wd->setPoints[i]->setSelected(false);
		}
	}
}

void pse::shiftSelectedSetPoint(void)
{
	pse::WorldData* wd = pse::getWorldData();
	pse::EditorData* ed = pse::getEditorData();


}

void pse::generalSyncFields(void)
{
	pse::WorldData* wd = pse::getWorldData();
	
	//Map Proper Name
	std::wstring properNameW(wd->properName.begin(), wd->properName.end());
	properNameField->setText(properNameW.c_str());

	//Texture Path
	std::wstring texturePathW(wd->texturePath.begin(), wd->texturePath.end());
	importTextureField->setText(texturePathW.c_str());

	importTexturesClick();//import the textures for the new map obviously
}

void pse::updateTargetFilePath(std::wstring newPath)
{
	targetFileField->setText(newPath.c_str());
}