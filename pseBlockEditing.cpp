  #include "pseBase.h"
#include "SInput.h"

static void setupFrame(void);
static psGui::Frame* frame = 0;

//gui elements that need to be global (in this file)
static psGui::SimpleButton* editingModeGrid = 0;
static void editingModeGridClick(void);
static psGui::SimpleButton* editingModeHand = 0;
static void editingModeHandClick(void);

static psGui::Checkbox* sidePlacementCheckbox;

static psGui::Checkbox* gridNormalXCheckbox;
static psGui::Checkbox* gridNormalYCheckbox;
static psGui::Checkbox* gridNormalZCheckbox;

static psGui::Slider* gridPositionX;
static psGui::Slider* gridPositionY;
static psGui::Slider* gridPositionZ;

static psGui::SimpleButton* xEnabledButton = 0;
static psGui::SimpleButton* yEnabledButton = 0;
static psGui::SimpleButton* zEnabledButton = 0;

static psGui::SimpleButton* xCenterButton = 0;
static psGui::SimpleButton* yCenterButton = 0;
static psGui::SimpleButton* zCenterButton = 0;

static psGui::Label* voxelFillLabel1 = 0;
static psGui::Label* voxelFillLabel2 = 0;
static psGui::Label* voxelFillStatus1LabelX = 0;
static psGui::Label* voxelFillStatus2LabelX = 0;
static psGui::Label* voxelFillStatus1LabelY = 0;
static psGui::Label* voxelFillStatus2LabelY = 0;
static psGui::Label* voxelFillStatus1LabelZ = 0;
static psGui::Label* voxelFillStatus2LabelZ = 0;

static psGui::SimpleButton* voxelFillButton = 0;
static psGui::SimpleButton* voxelClearButton = 0;

static void enableXClick(void);
static void enableYClick(void);
static void enableZClick(void);

static void xCenterClick(void);
static void yCenterClick(void);
static void zCenterClick(void);

static void gridToCameraClick(void);

static void voxelFillClick(void);
static void voxelClearClick(void);

static pse::VoxelCoordinate getVoxelCoordinateFromScreen(int x, int y);
void pse::setupBlockEditing(void)
{
	setupFrame();
}
psGui::Frame* pse::getBlockFrame(void){
	return frame;
}
void pse::updateBlockEditing(void)
{

	//first update frame
	if (frame->getMouseWithinFrame())frame->update();

	
	pse::EditorData* ed = pse::getEditorData();
	pse::WorldData* wd = pse::getWorldData();

	//update edit mode buttons
	if (ed->editingMode == PSE_EDITING_MODE_GRID)
	{
		editingModeGrid->setColor(0x7f6f3f3f);
		editingModeHand->setColor(0x7f3f3f3f);
	}
	if (ed->editingMode == PSE_EDITING_MODE_HAND)
	{
		editingModeHand->setColor(0x7f6f3f3f);
		editingModeGrid->setColor(0x7f3f3f3f);
	}

	//update grid enable buttons
	ed->gridX->setVisible(gridNormalXCheckbox->getState());
	ed->gridY->setVisible(gridNormalYCheckbox->getState());
	ed->gridZ->setVisible(gridNormalZCheckbox->getState());

	//update grid position sliders
	ed->gridLevelX = gridPositionX->getValue();
	ed->gridLevelY = gridPositionY->getValue();
	ed->gridLevelZ = gridPositionZ->getValue();

	ed->gridX->setPosition((float)ed->gridLevelX, 0.0f, 0.0f);
	ed->gridY->setPosition(0.0f, (float)ed->gridLevelY, 0.0f);
	ed->gridZ->setPosition(0.0f, 0.0f, (float)ed->gridLevelZ);

	if (pse::getModuleActive_voxel()&& 
		S_INPUT::mousePressed(0) &&
		!frame->getMouseWithinFrame() &&
		!pse::getLAFrame()->getMouseWithinFrame() &&
		!pse::getGeneralFrame()->getMouseWithinFrame() &&
		pse::getEditorData()->editingMode == PSE_EDITING_MODE_GRID)
	{
		VoxelCoordinate vc = getVoxelCoordinateFromScreen(S_INPUT::getMouseX(), S_INPUT::getMouseY());
		if (vc.x > -1)
		{
			if (S_INPUT::keyPressed(DIK_Z))
			{
				ed->fillSelection1.x = vc.x;
				ed->fillSelection1.y = vc.y;
				ed->fillSelection1.z = vc.z;
			} else if (S_INPUT::keyPressed(DIK_X))
			{
				ed->fillSelection2.x = vc.x;
				ed->fillSelection2.y = vc.y;
				ed->fillSelection2.z = vc.z;
			} else {
				
				wd->voxelData->setData(ed->typeSelection,vc.x,vc.y,vc.z);
				wd->voxelMesh->updateGeometry(wd->voxelData);
			}
		}
	}
	
	
	if (S_INPUT::keyPressed(DIK_0)) ed->typeSelection = 0;

	//update fill values
	static std::wstringstream wss1x;
	wss1x.str(L"");
	wss1x<<5;
	voxelFillStatus1LabelX->setText(wss1x.str().c_str());

	static std::wstringstream wss1y;
	wss1y.str(L"");
	wss1y<<ed->fillSelection1.y;
	voxelFillStatus1LabelY->setText(wss1y.str().c_str());

	static std::wstringstream wss1z;
	wss1z.str(L"");
	wss1z<<ed->fillSelection1.z;
	voxelFillStatus1LabelZ->setText(wss1z.str().c_str());

	static std::wstringstream wss2x;
	wss2x.str(L"");
	wss2x<<ed->fillSelection2.x;
	voxelFillStatus2LabelX->setText(wss2x.str().c_str());

	static std::wstringstream wss2y;
	wss2y.str(L"");
	wss2y<<ed->fillSelection2.y;
	voxelFillStatus2LabelY->setText(wss2y.str().c_str());

	static std::wstringstream wss2z;
	wss2z.str(L"");
	wss2z<<ed->fillSelection2.z;
	voxelFillStatus2LabelZ->setText(wss2z.str().c_str());
}

void setupFrame(void)
{
	using namespace psGui;
	frame = new Frame(1.5f, 1.0f, 0.4f, 0.9f);
	frame->setColor(0xaf3f3f3f);

	Label* frameLabel = new Label();
	frameLabel->setText(L"Voxel Editing\0");
	frameLabel->setSize(0.4f, 0.1f);
	frameLabel->setTextSize(8);
	frameLabel->setRelativePosition(0.0f, 0.4f);
	frameLabel->setTextColor(0xafffffff);
	frameLabel->setTextAlignment(DT_CENTER | DT_VCENTER);
	
	Label* editingModeLabel = new Label();
	editingModeLabel->setText(L"Editing Mode");
	editingModeLabel->setSize(0.38f, 0.1f);
	editingModeLabel->setTextSize(5);
	editingModeLabel->setRelativePosition(0.0f, 0.3f);
	editingModeLabel->setTextColor(0xafffffff);
	editingModeLabel->setTextAlignment(DT_LEFT | DT_VCENTER);

	editingModeGrid = new SimpleButton();
	editingModeGrid->setCaption(L"GRID");
	editingModeGrid->setSize(0.1f, 0.05f);
	editingModeGrid->setColor(0x7f3f3f3f);
	editingModeGrid->setRelativePosition(0.02f, 0.3f);
	editingModeGrid->setFunctionPointer(editingModeGridClick);

	editingModeHand = new SimpleButton();
	editingModeHand->setCaption(L"HAND");
	editingModeHand->setSize(0.1f, 0.05f);
	editingModeHand->setColor(0x7f3f3f3f);
	editingModeHand->setRelativePosition(0.14f, 0.3f);
	editingModeHand->setFunctionPointer(editingModeHandClick);

	Label* showGridsLabel = new Label();
	showGridsLabel->setText(L"Enable Grids");
	showGridsLabel->setSize(0.38f, 0.1f);
	showGridsLabel->setTextSize(5);
	showGridsLabel->setRelativePosition(0.0f, 0.23f);
	showGridsLabel->setTextColor(0xafffffff);
	showGridsLabel->setTextAlignment(DT_LEFT | DT_VCENTER);

	gridNormalXCheckbox = new Checkbox();
	gridNormalXCheckbox->setCheckColor(0xcfffffff);
	gridNormalXCheckbox->setColor(0x7f7f7f7f);
	gridNormalXCheckbox->setSize(0.05f);
	gridNormalXCheckbox->setRelativePosition(0.0f, 0.23f);

	gridNormalYCheckbox = new Checkbox();
	gridNormalYCheckbox->setCheckColor(0xcfffffff);
	gridNormalYCheckbox->setColor(0x7f7f7f7f);
	gridNormalYCheckbox->setSize(0.05f);
	gridNormalYCheckbox->setRelativePosition(0.05f, 0.23f);

	gridNormalZCheckbox = new Checkbox();
	gridNormalZCheckbox->setCheckColor(0xcfffffff);
	gridNormalZCheckbox->setColor(0x7f7f7f7f);
	gridNormalZCheckbox->setSize(0.05f);
	gridNormalZCheckbox->setRelativePosition(0.1, 0.23f);
	
	Label* gridPositionsLabel = new Label();
	gridPositionsLabel->setText(L"Grid Positions");
	gridPositionsLabel->setSize(0.38f, 0.1f);
	gridPositionsLabel->setTextSize(5);
	gridPositionsLabel->setRelativePosition(0.0f, 0.16f);
	gridPositionsLabel->setTextColor(0xafffffff);
	gridPositionsLabel->setTextAlignment(DT_LEFT | DT_VCENTER);

	gridPositionX = new Slider();
	gridPositionX->setMin(0);
	gridPositionX->setMax(65);
	gridPositionX->setLineColor(0xaf3f3f3f);
	gridPositionX->setSliderColor(0x5fff0000);
	gridPositionX->setSize(0.11f, 0.03f);
	gridPositionX->setLineThickness(0.005f);
	gridPositionX->setSliderThickness(0.007f);
	gridPositionX->setRelativePosition(-0.13f, 0.10f);

	gridPositionY = new Slider();
	gridPositionY->setMin(0);
	gridPositionY->setMax(65);
	gridPositionY->setLineColor(0xaf3f3f3f);
	gridPositionY->setSliderColor(0x5f00ff00);
	gridPositionY->setSize(0.11f, 0.03f);
	gridPositionY->setLineThickness(0.005f);
	gridPositionY->setSliderThickness(0.007f);
	gridPositionY->setRelativePosition(0.0f, 0.10f);

	gridPositionZ = new Slider();
	gridPositionZ->setMin(0);
	gridPositionZ->setMax(65);
	gridPositionZ->setLineColor(0xaf3f3f3f);
	gridPositionZ->setSliderColor(0x5f0000ff);
	gridPositionZ->setSize(0.11f, 0.03f);
	gridPositionZ->setLineThickness(0.005f);
	gridPositionZ->setSliderThickness(0.007f);
	gridPositionZ->setRelativePosition(0.13f, 0.10f);

	SimpleButton* gridToCamera = new SimpleButton();
	gridToCamera->setCaption(L"Grid To Camera");
	gridToCamera->setSize(0.2f, 0.05f);
	gridToCamera->setColor(0x7f3f3f3f);
	gridToCamera->setRelativePosition(0.09f, 0.16f);
	gridToCamera->setFunctionPointer(gridToCameraClick);

	xEnabledButton = new SimpleButton();
	xEnabledButton->setCaption(L"X GRID");
	xEnabledButton->setSize(0.1f, 0.05f);
	xEnabledButton->setColor(0x7f3f3f3f);
	xEnabledButton->setRelativePosition(-0.13f, 0.04f);
	xEnabledButton->setFunctionPointer(enableXClick);

	yEnabledButton = new SimpleButton();
	yEnabledButton->setCaption(L"Y GRID");
	yEnabledButton->setSize(0.1f, 0.05f);
	yEnabledButton->setColor(0x7f3f3f3f);
	yEnabledButton->setRelativePosition(0.0f, 0.04f);
	yEnabledButton->setFunctionPointer(enableYClick);

	zEnabledButton = new SimpleButton();
	zEnabledButton->setCaption(L"Z GRID");
	zEnabledButton->setSize(0.1f, 0.05f);
	zEnabledButton->setColor(0x7f3f3f3f);
	zEnabledButton->setRelativePosition(0.13f, 0.04f);
	zEnabledButton->setFunctionPointer(enableZClick);

	xCenterButton = new SimpleButton();
	xCenterButton->setCaption(L"Center");
	xCenterButton->setSize(0.1f, 0.05f);
	xCenterButton->setColor(0x7f3f3f3f);
	xCenterButton->setRelativePosition(-0.13f, -0.04f);
	xCenterButton->setFunctionPointer(xCenterClick);

	yCenterButton = new SimpleButton();
	yCenterButton->setCaption(L"Center");
	yCenterButton->setSize(0.1f, 0.05f);
	yCenterButton->setColor(0x7f3f3f3f);
	yCenterButton->setRelativePosition(0.0f, -0.04f);
	yCenterButton->setFunctionPointer(yCenterClick);

	zCenterButton = new SimpleButton();
	zCenterButton->setCaption(L"Center");
	zCenterButton->setSize(0.1f, 0.05f);
	zCenterButton->setColor(0x7f3f3f3f);
	zCenterButton->setRelativePosition(0.13f, -0.04f);
	zCenterButton->setFunctionPointer(zCenterClick);

	Label* sidePlacementLabel = new Label();
	sidePlacementLabel->setText(L"Same Side Placement");
	sidePlacementLabel->setSize(0.25f, 0.1f);
	sidePlacementLabel->setTextSize(5);
	sidePlacementLabel->setRelativePosition(0.0f, -0.12f);
	sidePlacementLabel->setTextColor(0xafffffff);
	sidePlacementLabel->setTextAlignment(DT_LEFT | DT_VCENTER);

	sidePlacementCheckbox = new Checkbox();
	sidePlacementCheckbox->setCheckColor(0xcfffffff);
	sidePlacementCheckbox->setColor(0x7f4f4f4f);
	sidePlacementCheckbox->setSize(0.04f);
	sidePlacementCheckbox->setRelativePosition(0.10f, -0.12f);

	voxelFillLabel1 = new Label();
	voxelFillLabel1->setText(L"Voxel Fill Selection 1 (hold Z then Click)");
	voxelFillLabel1->setSize(0.38f, 0.1f);
	voxelFillLabel1->setTextSize(5);
	voxelFillLabel1->setRelativePosition(0.0f, -0.18f);
	voxelFillLabel1->setTextColor(0xafffffff);
	voxelFillLabel1->setTextAlignment(DT_CENTER | DT_VCENTER);

	voxelFillLabel2 = new Label();
	voxelFillLabel2->setText(L"Voxel Fill Selection 2 (hold X then Click)");
	voxelFillLabel2->setSize(0.38f, 0.1f);
	voxelFillLabel2->setTextSize(5);
	voxelFillLabel2->setRelativePosition(0.0f, -0.28f);
	voxelFillLabel2->setTextColor(0xafffffff);
	voxelFillLabel2->setTextAlignment(DT_CENTER | DT_VCENTER);

	voxelFillStatus1LabelX = new Label();
	voxelFillStatus1LabelX->setText(L"0");
	voxelFillStatus1LabelX->setSize(0.38f, 0.1f);
	voxelFillStatus1LabelX->setTextSize(5);
	voxelFillStatus1LabelX->setRelativePosition(-0.15f, -0.22f);
	voxelFillStatus1LabelX->setTextColor(0xafffffff);
	voxelFillStatus1LabelX->setTextAlignment(DT_CENTER | DT_VCENTER);

	voxelFillStatus2LabelX = new Label();
	voxelFillStatus2LabelX->setText(L"0");
	voxelFillStatus2LabelX->setSize(0.38f, 0.1f);
	voxelFillStatus2LabelX->setTextSize(5);
	voxelFillStatus2LabelX->setRelativePosition(-0.15f, -0.32f);
	voxelFillStatus2LabelX->setTextColor(0xafffffff);
	voxelFillStatus2LabelX->setTextAlignment(DT_CENTER | DT_VCENTER);

	voxelFillStatus1LabelY = new Label();
	voxelFillStatus1LabelY->setText(L"0");
	voxelFillStatus1LabelY->setSize(0.38f, 0.1f);
	voxelFillStatus1LabelY->setTextSize(5);
	voxelFillStatus1LabelY->setRelativePosition(0.0f, -0.22f);
	voxelFillStatus1LabelY->setTextColor(0xafffffff);
	voxelFillStatus1LabelY->setTextAlignment(DT_CENTER | DT_VCENTER);

	voxelFillStatus2LabelY = new Label();
	voxelFillStatus2LabelY->setText(L"0");
	voxelFillStatus2LabelY->setSize(0.38f, 0.1f);
	voxelFillStatus2LabelY->setTextSize(5);
	voxelFillStatus2LabelY->setRelativePosition(0.0f, -0.32f);
	voxelFillStatus2LabelY->setTextColor(0xafffffff);
	voxelFillStatus2LabelY->setTextAlignment(DT_CENTER | DT_VCENTER);

	voxelFillStatus1LabelZ = new Label();
	voxelFillStatus1LabelZ->setText(L"0");
	voxelFillStatus1LabelZ->setSize(0.38f, 0.1f);
	voxelFillStatus1LabelZ->setTextSize(5);
	voxelFillStatus1LabelZ->setRelativePosition(0.15f, -0.22f);
	voxelFillStatus1LabelZ->setTextColor(0xafffffff);
	voxelFillStatus1LabelZ->setTextAlignment(DT_CENTER | DT_VCENTER);

	voxelFillStatus2LabelZ = new Label();
	voxelFillStatus2LabelZ->setText(L"0");
	voxelFillStatus2LabelZ->setSize(0.38f, 0.1f);
	voxelFillStatus2LabelZ->setTextSize(5);
	voxelFillStatus2LabelZ->setRelativePosition(0.15f, -0.32f);
	voxelFillStatus2LabelZ->setTextColor(0xafffffff);
	voxelFillStatus2LabelZ->setTextAlignment(DT_CENTER | DT_VCENTER);

	voxelFillButton = new SimpleButton();
	voxelFillButton->setCaption(L"Fill");
	voxelFillButton->setSize(0.1f, 0.05f);
	voxelFillButton->setColor(0x7f3f3f3f);
	voxelFillButton->setRelativePosition(0.12f, -0.38f);
	voxelFillButton->setFunctionPointer(voxelFillClick);

	voxelClearButton = new SimpleButton();
	voxelClearButton->setCaption(L"Clear");
	voxelClearButton->setSize(0.1f, 0.05f);
	voxelClearButton->setColor(0x7f8f3f3f);
	voxelClearButton->setRelativePosition(-0.12f, -0.38f);
	voxelClearButton->setFunctionPointer(voxelClearClick);
	frame->addComponent(frameLabel);

	frame->addComponent(editingModeLabel);
	frame->addComponent(editingModeGrid);
	frame->addComponent(editingModeHand);

	frame->addComponent(showGridsLabel);
	frame->addComponent(gridNormalXCheckbox);
	frame->addComponent(gridNormalYCheckbox);
	frame->addComponent(gridNormalZCheckbox);

	frame->addComponent(gridPositionsLabel);
	frame->addComponent(gridPositionX);
	frame->addComponent(gridPositionY);
	frame->addComponent(gridPositionZ);

	frame->addComponent(gridToCamera);
	frame->addComponent(xEnabledButton);
	frame->addComponent(yEnabledButton);
	frame->addComponent(zEnabledButton);
	frame->addComponent(xCenterButton);
	frame->addComponent(yCenterButton);
	frame->addComponent(zCenterButton);
	frame->addComponent(sidePlacementLabel);
	frame->addComponent(sidePlacementCheckbox);

	frame->addComponent(voxelFillLabel1);
	frame->addComponent(voxelFillLabel2);
	frame->addComponent(voxelFillStatus1LabelX);
	frame->addComponent(voxelFillStatus2LabelX);
	frame->addComponent(voxelFillStatus1LabelY);
	frame->addComponent(voxelFillStatus2LabelY);
	frame->addComponent(voxelFillStatus1LabelZ);
	frame->addComponent(voxelFillStatus2LabelZ);

	frame->addComponent(voxelFillButton);
	frame->addComponent(voxelClearButton);

	frame->setPosition(0.75f, 0.5f);
}

#include "SLOG.h"
#include <sstream>
pse::VoxelCoordinate getVoxelCoordinateHand(D3DXVECTOR3 rayOrigin, D3DXVECTOR3 rayDir)
{
	/* need access to world data to get voxel data */
	pse::WorldData* wd = pse::getWorldData();

	/* now trace ray until there is a hit with the voxel data */
	D3DXVECTOR3 p1 = rayOrigin;
	bool intersect = false;
	for (int i = 0; i<10000; i++){
		p1 = p1+(rayDir*0.01f);
		if (p1.x < 0.0f || p1.x > 63.0f) continue;
		if (p1.y < 0.0f || p1.y > 63.0f) continue;
		if (p1.z < 0.0f || p1.z > 63.0f) continue;

		if (wd->voxelData->data((int)p1.x,(int)p1.y,(int)p1.z) != 0)
		{
			intersect = true;
			break;
		}
	}
	
	std::ostringstream ss;
	ss.str("");
	ss<<p1.x<<","<<p1.y<<","<<p1.z;
	S_LOG::write(ss.str().c_str());

	pse::VoxelCoordinate v;
	v.x = 0;
	v.y = 0;
	v.z = 0;
	if (!intersect)
	{
		v.x = -1;
		v.y = -1;
		v.z = -1;
		return v;
	}
	if (pse::getEditorData()->typeSelection == 0)
	{
		v.x = (int)p1.x;
		v.y = (int)p1.y;
		v.z = (int)p1.z;
	} else {
		/* find smallest overlap */
		float midx = 0.5f + (int)p1.x;
		float xol = 0.5f - abs(midx - p1.x);

		float midy = 0.5f + (int)p1.y;
		float yol = 0.5f - abs(midy - p1.y);

		float midz = 0.5f + (int)p1.z;
		float zol = 0.5f - abs(midz - p1.z);

		if (xol < yol && xol < zol)
		{
			if (p1.x > midx)
			{
				v.x = (int)(p1.x + 1.0f);
				
			} else {
				v.x = (int)(p1.x - 1.0f);
			}

			v.y = (int)p1.y;
			v.z = (int)p1.z;
		}

		if (yol < xol && yol < zol)
		{
			if (p1.y > midy)
			{
				v.y = (int)(p1.y + 1.0f);
			} else {
				v.y = (int)(p1.y - 1.0f);
			}
			v.x = (int)p1.x;
			v.z = (int)p1.z;
		}

		if (zol < xol && zol < yol)
		{
			if (p1.z > midz)
			{
				v.z = (int)(p1.z + 1.0f);
			} else {
				v.z = (int)(p1.z - 1.0f);
			}
			v.x = (int)p1.x;
			v.y = (int)p1.y;
		}
	}
	if (v.x < 0 || v.x > 63 || v.y < 0 || v.y > 63 || v.z < 0 || v.z > 63)
	{
		v.x = -1;
		v.y = -1;
		v.z = -1;
	}
	return v;
}
pse::VoxelCoordinate pse::getVoxelCoordinateFromScreen(int x, int y)
{

	int gridOrientation = pse::getEditorData()->gridOrientation;
	D3DXMATRIX matProj = SRender::getSingleton()->getProjection();

	//get the actual size of screen
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

	D3DXVECTOR3 p1 = rayOrigin;
	int gridLevel = 0;
	pse::VoxelCoordinate blockSelect;
	blockSelect.x = 0;
	blockSelect.y = 0;
	blockSelect.z = 0;


	/* If its hand mode, go to a different function and just return that */
	if (pse::getEditorData()->editingMode==PSE_EDITING_MODE_HAND)
	{
		blockSelect = getVoxelCoordinateHand(rayOrigin, rayDir);
		return blockSelect;
	}


	/* If not, continue with grid function */
	bool sameSideSelection = sidePlacementCheckbox->getState();

	if (gridOrientation==PSE_GO_X){
		gridLevel = pse::getEditorData()->gridLevelX;
		if (rayOrigin.x > (float)gridLevel){
			for (int i = 0; i<1000; i++){
				p1 = p1+(rayDir*0.1f);
				if (p1.x < (float)gridLevel) break;
			}
			blockSelect.y = (int) p1.y;
			blockSelect.z = (int) p1.z;
			if (sameSideSelection){
				blockSelect.x = gridLevel;
			} else {
				blockSelect.x = gridLevel - 1;
			}
		}
		if (rayOrigin.x < (float)gridLevel){
			for (int i = 0; i<1000; i++){
				p1 = p1+(rayDir*0.1f);
				if (p1.x > (float)gridLevel) break;
			}
			blockSelect.y = (int) p1.y;
			blockSelect.z = (int) p1.z;
			if (sameSideSelection){
				blockSelect.x = gridLevel - 1;
			} else {
				blockSelect.x = gridLevel;
			}
		}
	}
	if (gridOrientation==PSE_GO_Y){
		gridLevel = pse::getEditorData()->gridLevelY;
		if (rayOrigin.y > (float)gridLevel){
			for (int i = 0; i<1000; i++){
				p1 = p1+(rayDir*0.1f);
				if (p1.y < (float)gridLevel) break;
			}
			blockSelect.x = (int) p1.x;
			blockSelect.z = (int) p1.z;
			if (sameSideSelection){
				blockSelect.y = gridLevel;
			} else {
				blockSelect.y = gridLevel - 1;
			}
		}
		if (rayOrigin.y < (float)gridLevel){
			for (int i = 0; i<1000; i++){
				p1 = p1+(rayDir*0.1f);
				if (p1.y > (float)gridLevel) break;
			}
			blockSelect.x = (int) p1.x;
			blockSelect.z = (int) p1.z;
			if (sameSideSelection){
				blockSelect.y = gridLevel - 1;
			} else {
				blockSelect.y = gridLevel;
			}
		}
	}
	if (gridOrientation==PSE_GO_Z){
		gridLevel = pse::getEditorData()->gridLevelZ;
		if (rayOrigin.z > (float)gridLevel){
			for (int i = 0; i<1000; i++){
				p1 = p1+(rayDir*0.1f);
				if (p1.z < (float)gridLevel) break;
			}
			blockSelect.y = (int) p1.y;
			blockSelect.x = (int) p1.x;
			if (sameSideSelection){
				blockSelect.z = gridLevel;
			} else {
				blockSelect.z = gridLevel - 1;
			}
		}
		if (rayOrigin.z < (float)gridLevel){
			for (int i = 0; i<1000; i++){
				p1 = p1+(rayDir*0.1f);
				if (p1.z > (float)gridLevel) break;
			}
			blockSelect.y = (int) p1.y;
			blockSelect.x = (int) p1.x;
			if (sameSideSelection){
				blockSelect.z = gridLevel - 1;
			} else {
				blockSelect.z = gridLevel;
			}
		}
	}
	
	
	if (blockSelect.x < 0 || blockSelect.x > 64-1 || blockSelect.y < 0 || blockSelect.y > 64-1 || blockSelect.z < 0 || blockSelect.z > 64-1){
		blockSelect.x = -1; //in the main editor, if it receives x = -1, it doesnt place a block :)
	}
	return blockSelect;
}

void editingModeGridClick(void)
{
	pse::getEditorData()->editingMode = PSE_EDITING_MODE_GRID;
}

void editingModeHandClick(void)
{
	pse::getEditorData()->editingMode = PSE_EDITING_MODE_HAND;
}

void gridToCameraClick(void)
{
	SCamera* cam = pse::getEditorData()->editorCam;
	pse::EditorData* ed = pse::getEditorData();
	float x = cam->getDirectionX().x;
	float y = cam->getDirectionX().y;
	float z = cam->getDirectionX().z;
	if (abs(x) > abs(y) && abs(x) > abs(z))
	{
		if (x > 0.0f)
		{
			gridNormalXCheckbox->setState(true);
			pse::getEditorData()->gridLevelX = cam->getPositionX().x + 5.0f;
			gridPositionX->setValue((int)ed->gridLevelX);
		}

		if (x < 0.0f)
		{
			gridNormalXCheckbox->setState(true);
			pse::getEditorData()->gridLevelX = cam->getPositionX().x - 5.0f;
			gridPositionX->setValue((int)ed->gridLevelX);
		}
	}
	if (abs(y) > abs(x) && abs(y) > abs(z))
	{
		if (y > 0.0f)
		{
			gridNormalYCheckbox->setState(true);
			pse::getEditorData()->gridLevelY = cam->getPositionX().y + 5.0f;
			gridPositionY->setValue((int)ed->gridLevelY);
		}

		if (y < 0.0f)
		{
			gridNormalYCheckbox->setState(true);
			pse::getEditorData()->gridLevelY = cam->getPositionX().y - 5.0f;
			gridPositionY->setValue((int)ed->gridLevelY);
		}
	}
	if (abs(z) > abs(x) && abs(z) > abs(y))
	{
		if (z > 0.0f)
		{
			gridNormalZCheckbox->setState(true);
			pse::getEditorData()->gridLevelZ = cam->getPositionX().z + 5.0f;
			gridPositionZ->setValue((int)ed->gridLevelZ);
		}

		if (z < 0.0f)
		{
			gridNormalZCheckbox->setState(true);
			pse::getEditorData()->gridLevelZ = cam->getPositionX().z - 5.0f;
			gridPositionZ->setValue((int)ed->gridLevelZ);
		}
	}

	frame->update();
}

void enableXClick(void)
{
	pse::getEditorData()->gridOrientation = PSE_GO_X;
	xEnabledButton->setColor(0x8f5f5f5f);
	yEnabledButton->setColor(0x7f3f3f3f);
	zEnabledButton->setColor(0x7f3f3f3f);
}

void enableYClick(void)
{
	pse::getEditorData()->gridOrientation = PSE_GO_Y;
	yEnabledButton->setColor(0x8f5f5f5f);
	xEnabledButton->setColor(0x7f3f3f3f);
	zEnabledButton->setColor(0x7f3f3f3f);
}

void enableZClick(void)
{
	pse::getEditorData()->gridOrientation = PSE_GO_Z;
	zEnabledButton->setColor(0x8f5f5f5f);
	yEnabledButton->setColor(0x7f3f3f3f);
	xEnabledButton->setColor(0x7f3f3f3f);
}

void xCenterClick(void)
{
	pse::EditorData* ed = pse::getEditorData();
	pse::WorldData* wd = pse::getWorldData();
	ed->gridLevelX = wd->voxelData->x() / 2;
	
	ed->gridX->setPosition((float)ed->gridLevelX, 0.0f, 0.0f);
	gridPositionX->setValue(ed->gridLevelX);
}

void yCenterClick(void)
{
	pse::EditorData* ed = pse::getEditorData();
	pse::WorldData* wd = pse::getWorldData();
	ed->gridLevelY = wd->voxelData->y() / 2;
	
	ed->gridY->setPosition(0.0f, (float)ed->gridLevelY, 0.0f);
	gridPositionY->setValue(ed->gridLevelY);
}

void zCenterClick(void)
{
	pse::EditorData* ed = pse::getEditorData();
	pse::WorldData* wd = pse::getWorldData();
	ed->gridLevelZ = wd->voxelData->z() / 2;
	
	ed->gridZ->setPosition(0.0f, 0.0f, (float)ed->gridLevelZ);
	gridPositionZ->setValue(ed->gridLevelZ);
}

void voxelFillClick(void)
{
	pse::WorldData* wd = pse::getWorldData();
	pse::EditorData* ed = pse::getEditorData();

	pse::VoxelCoordinate firstClick, secondClick;

	firstClick.x = ed->fillSelection1.x;
	firstClick.y = ed->fillSelection1.y;
	firstClick.z = ed->fillSelection1.z;

	secondClick.x = ed->fillSelection2.x;
	secondClick.y = ed->fillSelection2.y;
	secondClick.z = ed->fillSelection2.z;

	if (firstClick.x < secondClick.x || firstClick.x == secondClick.x){
		for (int x = firstClick.x; x < secondClick.x + 1; x++){
			if (firstClick.y < secondClick.y || firstClick.y == secondClick.y){
				for (int y = firstClick.y; y < secondClick.y + 1; y++){
					if (firstClick.z < secondClick.z || firstClick.z == secondClick.z){
						for (int z = firstClick.z; z < secondClick.z + 1; z++){
							wd->voxelData->setData(ed->typeSelection, x, y, z);
						}
					} else if (firstClick.z > secondClick.z){
						for (int z = secondClick.z; z < firstClick.z + 1; z++){
							wd->voxelData->setData(ed->typeSelection, x, y, z);
						}
					}
				}
			} else if (firstClick.y > secondClick.y){
				for (int y = secondClick.y; y < firstClick.y + 1; y++){
					if (firstClick.z < secondClick.z || firstClick.z == secondClick.z){
						for (int z = firstClick.z; z < secondClick.z + 1; z++){
							wd->voxelData->setData(ed->typeSelection, x, y, z);
						}
					} else if (firstClick.z > secondClick.z){
						for (int z = secondClick.z; z < firstClick.z + 1; z++){
							wd->voxelData->setData(ed->typeSelection, x, y, z);
						}
					}
				}
			}
		}
	} else if (firstClick.x > secondClick.x){
		for (int x = secondClick.x; x < firstClick.x + 1; x++){
			if (firstClick.y < secondClick.y || firstClick.y == secondClick.y){
				for (int y = firstClick.y; y < secondClick.y + 1; y++){
					if (firstClick.z < secondClick.z || firstClick.z == secondClick.z){
						for (int z = firstClick.z; z < secondClick.z + 1; z++){
							wd->voxelData->setData(ed->typeSelection, x, y, z);
						}
					} else if (firstClick.z > secondClick.z){
						for (int z = secondClick.z; z < firstClick.z + 1; z++){
							wd->voxelData->setData(ed->typeSelection, x, y, z);
						}
					}
				}
			} else if (firstClick.y > secondClick.y){
				for (int y = secondClick.y; y < firstClick.y + 1; y++){
					if (firstClick.z < secondClick.z || firstClick.z == secondClick.z){
						for (int z = firstClick.z; z < secondClick.z + 1; z++){
							wd->voxelData->setData(ed->typeSelection, x, y, z);
						}
					} else if (firstClick.z > secondClick.z){
						for (int z = secondClick.z; z < firstClick.z + 1; z++){
							wd->voxelData->setData(ed->typeSelection, x, y, z);
						}
					}
				}
			}
		}
	}
	wd->voxelMesh->updateGeometry(wd->voxelData);
}

void voxelClearClick(void)
{
	pse::WorldData* wd = pse::getWorldData();
	pse::EditorData* ed = pse::getEditorData();

	pse::VoxelCoordinate firstClick, secondClick;

	firstClick.x = ed->fillSelection1.x;
	firstClick.y = ed->fillSelection1.y;
	firstClick.z = ed->fillSelection1.z;

	secondClick.x = ed->fillSelection2.x;
	secondClick.y = ed->fillSelection2.y;
	secondClick.z = ed->fillSelection2.z;

	if (firstClick.x < secondClick.x || firstClick.x == secondClick.x){
		for (int x = firstClick.x; x < secondClick.x + 1; x++){
			if (firstClick.y < secondClick.y || firstClick.y == secondClick.y){
				for (int y = firstClick.y; y < secondClick.y + 1; y++){
					if (firstClick.z < secondClick.z || firstClick.z == secondClick.z){
						for (int z = firstClick.z; z < secondClick.z + 1; z++){
							wd->voxelData->setData(0, x, y, z);
						}
					} else if (firstClick.z > secondClick.z){
						for (int z = secondClick.z; z < firstClick.z + 1; z++){
							wd->voxelData->setData(0, x, y, z);
						}
					}
				}
			} else if (firstClick.y > secondClick.y){
				for (int y = secondClick.y; y < firstClick.y + 1; y++){
					if (firstClick.z < secondClick.z || firstClick.z == secondClick.z){
						for (int z = firstClick.z; z < secondClick.z + 1; z++){
							wd->voxelData->setData(0, x, y, z);
						}
					} else if (firstClick.z > secondClick.z){
						for (int z = secondClick.z; z < firstClick.z + 1; z++){
							wd->voxelData->setData(0, x, y, z);
						}
					}
				}
			}
		}
	} else if (firstClick.x > secondClick.x){
		for (int x = secondClick.x; x < firstClick.x + 1; x++){
			if (firstClick.y < secondClick.y || firstClick.y == secondClick.y){
				for (int y = firstClick.y; y < secondClick.y + 1; y++){
					if (firstClick.z < secondClick.z || firstClick.z == secondClick.z){
						for (int z = firstClick.z; z < secondClick.z + 1; z++){
							wd->voxelData->setData(0, x, y, z);
						}
					} else if (firstClick.z > secondClick.z){
						for (int z = secondClick.z; z < firstClick.z + 1; z++){
							wd->voxelData->setData(0, x, y, z);
						}
					}
				}
			} else if (firstClick.y > secondClick.y){
				for (int y = secondClick.y; y < firstClick.y + 1; y++){
					if (firstClick.z < secondClick.z || firstClick.z == secondClick.z){
						for (int z = firstClick.z; z < secondClick.z + 1; z++){
							wd->voxelData->setData(0, x, y, z);
						}
					} else if (firstClick.z > secondClick.z){
						for (int z = secondClick.z; z < firstClick.z + 1; z++){
							wd->voxelData->setData(0, x, y, z);
						}
					}
				}
			}
		}
	}
	wd->voxelMesh->updateGeometry(wd->voxelData);
}

