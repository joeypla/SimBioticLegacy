#include "pseBase.h"
static void setupFrame(void);
static psGui::Frame* frame = 0;

static psGui::Slider* ambientRed = 0;
static psGui::Slider* ambientGreen = 0;
static psGui::Slider* ambientBlue = 0;

static psGui::Slider* fogRed = 0;
static psGui::Slider* fogGreen = 0;
static psGui::Slider* fogBlue = 0;

static psGui::Slider* fogStart = 0;
static psGui::Slider* fogEnd = 0;

static psGui::Slider* lightRed = 0;
static psGui::Slider* lightGreen = 0;
static psGui::Slider* lightBlue = 0;



static psGui::Slider* radiusSlider = 0;
static psGui::Slider* ambientFactorSlider = 0;
static psGui::Checkbox* isSpotlightOption = 0;
static psGui::Slider* phiSlider = 0;
static psGui::Slider* thetaSlider = 0;

static psGui::SimpleButton* lightLockButton    = 0;
static psGui::SimpleButton* placeCamAtLightButton = 0;
static psGui::SimpleButton* addLightButton = 0;
static psGui::SimpleButton* removeLightButton = 0;

static SLineBoxMesh* lightHandleMesh = 0;
static SLineBoxMesh* lightHandleMeshSelected = 0;

static void addLightClick(void);
static void removeLightClick(void);
static void camToLightClick(void);
static void lightLockClick(void);

psGui::Frame* pse::getLAFrame(void)
{
	return frame;
}
void pse::setupLAEditing(void)
{
	//Load mesh for light
	lightHandleMesh = SRender::getSingleton()->createLineBoxMesh();
	lightHandleMesh->loadBox(0xffffff00);

	lightHandleMeshSelected = SRender::getSingleton()->createLineBoxMesh();
	lightHandleMeshSelected->loadBox(0xff00ff00);

	//Setup the frame
	setupFrame();
}

#include "SInput.h"
#include "SLOG.h"
void pse::updateLAEditing(void)
{
	frame->update();
	//update ambient lighting
	//SRender::getSingleton()->setAmbientColor((unsigned char)ambientRed->getValue(), (unsigned char)ambientGreen->getValue(), (unsigned char)ambientBlue->getValue());
	SRender::getSingleton()->setAmbientColor((float)ambientRed->getValue() / 255.0f, (float)ambientGreen->getValue() / 255.0f, (float)ambientBlue->getValue() / 255.0f);

	//update Fog
	SRender::getSingleton()->setFogColor((float)fogRed->getValue()/255.0f, (float)fogGreen->getValue()/255.0f, (float)fogBlue->getValue()/255.0f);
	SRender::getSingleton()->setClearColor(D3DCOLOR_XRGB((char)fogRed->getValue(), (char)fogGreen->getValue(), (char)fogBlue->getValue()));
	SRender::getSingleton()->setFogStart((float)fogStart->getValue());
	SRender::getSingleton()->setFogEnd((float)fogEnd->getValue());

	pse::getWorldData()->ambientRed = (float)ambientRed->getValue()/255.0f;
	pse::getWorldData()->ambientGreen = (float)ambientGreen->getValue()/255.0f;
	pse::getWorldData()->ambientBlue = (float)ambientBlue->getValue()/255.0f;

	pse::getWorldData()->fogRed = (float)fogRed->getValue()/255.0f;
	pse::getWorldData()->fogGreen = (float)fogGreen->getValue()/255.0f;
	pse::getWorldData()->fogBlue = (float)fogBlue->getValue()/255.0f;

	pse::getWorldData()->fogStart = (float)fogStart->getValue();
	pse::getWorldData()->fogEnd = (float)fogEnd->getValue();


	if (!pse::getGeneralFrame()->getMouseWithinFrame() && !pse::getLAFrame()->getMouseWithinFrame() && !pse::getBlockFrame()->getMouseWithinFrame())
	{
		if (pse::getModuleActive_lighting() && S_INPUT::mousePressed(0))
		{
			pse::EditorData* ed = pse::getEditorData();
			static float lightToCam = 5.0f;
			int lastSelection = ed->lightSelection;
			int selection = pse::getLightFromClick(S_INPUT::getMouseX(), S_INPUT::getMouseY());
			
			if (selection != -1)
			{
				ed->lightSelection = selection;
				pse::Light* light = pse::getWorldData()->lights[selection];
				if (light!=0)
				{
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

					light->light->setPosition(
						rayOrigin.x + rayDir.x*lightToCam, 
						rayOrigin.y + rayDir.y*lightToCam, 
						rayOrigin.z + rayDir.z*lightToCam);

					light->handle->setPosition(
						rayOrigin.x + rayDir.x*lightToCam, 
						rayOrigin.y + rayDir.y*lightToCam, 
						rayOrigin.z + rayDir.z*lightToCam);

					pse::setAllLightsUnselected();
					light->handle->setMesh(lightHandleMeshSelected);
					ed->lightSelection = selection;
				}
			}

			
		}

		if (S_INPUT::keyPressed(DIK_L))
		{
			//remove asap
			std::stringstream test;
			test.str("");
			test<<"RED: "<<(int)ambientRed->getValue();
			S_LOG::write(test.str().c_str());
			pse::EditorData* ed = pse::getEditorData();
			pse::WorldData* wd = pse::getWorldData();
			if (ed->lightSelection > -1)
			{
				if (wd->lights[ed->lightSelection] != 0)
				{
					wd->lights[ed->lightSelection]->light->dx = ed->editorCam->dx;
					wd->lights[ed->lightSelection]->light->dy = ed->editorCam->dy;
					wd->lights[ed->lightSelection]->light->dz = ed->editorCam->dz;
				}
			}
			
		}
	}
	
	/* update Selected light with sliders and shit if one is selected */
	pse::EditorData* ed = pse::getEditorData();
	pse::WorldData* wd = pse::getWorldData();

	
	if (ed->lightSelection != -1)
	{
		if (wd->lights[ed->lightSelection]!=0)
		{
			pse::Light* light = wd->lights[ed->lightSelection];
			light->light->ambientFactor = (float)ambientFactorSlider->getValue() / 100.0f;
			light->light->r = (float)lightRed->getValue() / 255.0f;
			light->light->g = (float)lightGreen->getValue() / 255.0f;
			light->light->b = (float)lightBlue->getValue() / 255.0f;
			
			light->light->radius = (float)radiusSlider->getValue();

			if (isSpotlightOption->getState()) light->light->type = SRENDER_LIGHT_TYPE_SPOTLIGHT;
			else light->light->type = SRENDER_LIGHT_TYPE_POINT;

			light->light->phi = (float)phiSlider->getValue() / 100.0f;
			light->light->theta = (float)thetaSlider->getValue() / 100.0f;
		}
	}
}

void pse::updateAtmosphericsFromWorldData(void)
{
	pse::WorldData* wd = getWorldData();
	ambientRed->setValue((int)(wd->ambientRed * 255.0f));
	ambientGreen->setValue((int)(wd->ambientGreen * 255.0f));
	ambientBlue->setValue((int)(wd->ambientBlue * 255.0f));
	fogRed->setValue((int)(wd->fogRed * 255.0f));
	fogGreen->setValue((int)(wd->fogGreen * 255.0f));
	fogBlue->setValue((int)(wd->fogBlue * 255.0f));
	fogStart->setValue(wd->fogStart);
	fogEnd->setValue(wd->fogEnd);

}
void setupFrame(void)
{
	using namespace psGui;
	frame = new Frame(0.75f, -0.5f, 0.4f, 0.9f);
	frame->setColor(0xaf3f3f3f);

	Label* frameLabel = new Label();
	frameLabel->setText(L"Lighting and Atmospherics");
	frameLabel->setTextColor(0xafffffff);
	frameLabel->setSize(0.4f, 0.1f);
	frameLabel->setTextSize(5);
	frameLabel->setRelativePosition(0.0f, 0.4f);
	frameLabel->setTextAlignment(DT_CENTER | DT_VCENTER);

	Label* ambientLabel = new Label();
	ambientLabel->setText(L"Global Ambient");
	ambientLabel->setTextColor(0xafffffff);
	ambientLabel->setSize(0.38f, 0.1f);
	ambientLabel->setTextSize(5);
	ambientLabel->setRelativePosition(0.0f, 0.35f);
	ambientLabel->setTextAlignment(DT_LEFT | DT_VCENTER);

	ambientRed = new Slider();
	ambientRed->setMin(0);
	ambientRed->setMax(255);
	ambientRed->setLineColor(0xaf3f3f3f);
	ambientRed->setSliderColor(0x5fff0000);
	ambientRed->setSize(0.11f, 0.03f);
	ambientRed->setLineThickness(0.005f);
	ambientRed->setSliderThickness(0.007f);
	ambientRed->setRelativePosition(-0.13f, 0.30f);
	ambientRed->setValue(100);

	ambientGreen = new Slider();
	ambientGreen->setMin(0);
	ambientGreen->setMax(255);
	ambientGreen->setLineColor(0xaf3f3f3f);
	ambientGreen->setSliderColor(0x5f00ff00);
	ambientGreen->setSize(0.11f, 0.03f);
	ambientGreen->setLineThickness(0.005f);
	ambientGreen->setSliderThickness(0.007f);
	ambientGreen->setRelativePosition(0.0f, 0.30f);
	ambientGreen->setValue(100);

	ambientBlue = new Slider();
	ambientBlue->setMin(0);
	ambientBlue->setMax(255);
	ambientBlue->setLineColor(0xaf3f3f3f);
	ambientBlue->setSliderColor(0x5f0000ff);
	ambientBlue->setSize(0.11f, 0.03f);
	ambientBlue->setLineThickness(0.005f);
	ambientBlue->setSliderThickness(0.007f);
	ambientBlue->setRelativePosition(0.13f, 0.30f);
	ambientBlue->setValue(100);

	Label* fogLabel = new Label();
	fogLabel->setText(L"Fog Color");
	fogLabel->setTextColor(0xafffffff);
	fogLabel->setSize(0.38f, 0.1f);
	fogLabel->setTextSize(5);
	fogLabel->setRelativePosition(0.0f, 0.25f);
	fogLabel->setTextAlignment(DT_LEFT | DT_VCENTER);

	fogRed = new Slider();
	fogRed->setMin(0);
	fogRed->setMax(255);
	fogRed->setLineColor(0xaf3f3f3f);
	fogRed->setSliderColor(0x5fff0000);
	fogRed->setSize(0.11f, 0.03f);
	fogRed->setLineThickness(0.005f);
	fogRed->setSliderThickness(0.007f);
	fogRed->setRelativePosition(-0.13f, 0.2f);

	fogGreen = new Slider();
	fogGreen->setMin(0);
	fogGreen->setMax(255);
	fogGreen->setLineColor(0xaf3f3f3f);
	fogGreen->setSliderColor(0x5f00ff00);
	fogGreen->setSize(0.11f, 0.03f);
	fogGreen->setLineThickness(0.005f);
	fogGreen->setSliderThickness(0.007f);
	fogGreen->setRelativePosition(0.0f, 0.2f);

	fogBlue = new Slider();
	fogBlue->setMin(0);
	fogBlue->setMax(255);
	fogBlue->setLineColor(0xaf3f3f3f);
	fogBlue->setSliderColor(0x5f0000ff);
	fogBlue->setSize(0.11f, 0.03f);
	fogBlue->setLineThickness(0.005f);
	fogBlue->setSliderThickness(0.007f);
	fogBlue->setRelativePosition(0.13f, 0.2f);

	Label* fogStartLabel = new Label();
	fogStartLabel->setText(L"Fog Begin");
	fogStartLabel->setTextColor(0xafffffff);
	fogStartLabel->setSize(0.38f, 0.1f);
	fogStartLabel->setTextSize(5);
	fogStartLabel->setRelativePosition(0.0f, 0.15f);
	fogStartLabel->setTextAlignment(DT_LEFT | DT_VCENTER);

	Label* fogEndLabel = new Label();
	fogEndLabel->setText(L"Fog End");
	fogEndLabel->setTextColor(0xafffffff);
	fogEndLabel->setSize(0.38f, 0.1f);
	fogEndLabel->setTextSize(5);
	fogEndLabel->setRelativePosition(0.0f, 0.15f);
	fogEndLabel->setTextAlignment(DT_RIGHT | DT_VCENTER);

	fogStart = new Slider();
	fogStart->setMin(0);
	fogStart->setMax(255);
	fogStart->setLineColor(0xaf3f3f3f);
	fogStart->setSliderColor(0x9f000000);
	fogStart->setSize(0.16f, 0.03f);
	fogStart->setLineThickness(0.005f);
	fogStart->setSliderThickness(0.007f);
	fogStart->setRelativePosition(-0.1f, 0.1f);

	fogEnd = new Slider();
	fogEnd->setMin(0);
	fogEnd->setMax(255);
	fogEnd->setLineColor(0xaf3f3f3f);
	fogEnd->setSliderColor(0x9f000000);
	fogEnd->setSize(0.16f, 0.03f);
	fogEnd->setLineThickness(0.005f);
	fogEnd->setSliderThickness(0.007f);
	fogEnd->setRelativePosition(0.1f, 0.1f);

	Label* lightEditingLabel = new Label();
	lightEditingLabel->setText(L"Specific Light Editing");
	lightEditingLabel->setTextColor(0xafffffff);
	lightEditingLabel->setSize(0.38f, 0.1f);
	lightEditingLabel->setTextSize(5);
	lightEditingLabel->setRelativePosition(0.0f, 0.0f);
	lightEditingLabel->setTextAlignment(DT_LEFT | DT_VCENTER);

	lightRed = new Slider();
	lightRed->setMin(0);
	lightRed->setMax(255);
	lightRed->setLineColor(0xaf3f3f3f);
	lightRed->setSliderColor(0x5fff0000);
	lightRed->setSize(0.11f, 0.03f);
	lightRed->setLineThickness(0.005f);
	lightRed->setSliderThickness(0.007f);
	lightRed->setRelativePosition(-0.13f, -0.05f);

	lightGreen = new Slider();
	lightGreen->setMin(0);
	lightGreen->setMax(255);
	lightGreen->setLineColor(0xaf3f3f3f);
	lightGreen->setSliderColor(0x5f00ff00);
	lightGreen->setSize(0.11f, 0.03f);
	lightGreen->setLineThickness(0.005f);
	lightGreen->setSliderThickness(0.007f);
	lightGreen->setRelativePosition(0.0f, -0.05f);

	lightBlue = new Slider();
	lightBlue->setMin(0);
	lightBlue->setMax(255);
	lightBlue->setLineColor(0xaf3f3f3f);
	lightBlue->setSliderColor(0x5f0000ff);
	lightBlue->setSize(0.11f, 0.03f);
	lightBlue->setLineThickness(0.005f);
	lightBlue->setSliderThickness(0.007f);
	lightBlue->setRelativePosition(0.13f, -0.05f);

	

	

	Label* radiusLabel = new Label();
	radiusLabel->setText(L"Radius");
	radiusLabel->setTextColor(0xafffffff);
	radiusLabel->setSize(0.38f, 0.1f);
	radiusLabel->setTextSize(5);
	radiusLabel->setRelativePosition(0.0f, -0.1f);
	radiusLabel->setTextAlignment(DT_LEFT | DT_VCENTER);

	Label* ambientFactorLabel = new Label();
	ambientFactorLabel->setText(L"Ambient Factor");
	ambientFactorLabel->setTextColor(0xafffffff);
	ambientFactorLabel->setSize(0.38f, 0.1f);
	ambientFactorLabel->setTextSize(5);
	ambientFactorLabel->setRelativePosition(0.0f, -0.1f);
	ambientFactorLabel->setTextAlignment(DT_CENTER | DT_VCENTER);

	radiusSlider = new Slider();
	radiusSlider->setMin(0);
	radiusSlider->setMax(200);
	radiusSlider->setLineColor(0xcf1f1f1f);
	radiusSlider->setSliderColor(0x6f6f6f6f);
	radiusSlider->setSize(0.11f, 0.03f);
	radiusSlider->setLineThickness(0.005f);
	radiusSlider->setSliderThickness(0.007f);
	radiusSlider->setRelativePosition(-0.13f, -0.15f);

	ambientFactorSlider = new Slider();
	ambientFactorSlider->setMin(0);
	ambientFactorSlider->setMax(100);
	ambientFactorSlider->setLineColor(0xcf1f1f1f);
	ambientFactorSlider->setSliderColor(0x6f6f6f6f);
	ambientFactorSlider->setSize(0.11f, 0.03f);
	ambientFactorSlider->setLineThickness(0.005f);
	ambientFactorSlider->setSliderThickness(0.007f);
	ambientFactorSlider->setRelativePosition(0.0f, -0.15f);

	lightLockButton = new SimpleButton();
	lightLockButton->setCaption(L"Grid Lock");
	lightLockButton->setSize(0.10f, 0.06f);
	lightLockButton->setColor(0x7f3f3f3f);
	lightLockButton->setRelativePosition(0.13f, -0.15f);
	lightLockButton->setFunctionPointer(lightLockClick);

	Label* spotlightOptionLabel = new Label();
	spotlightOptionLabel->setText(L"Spotlight");
	spotlightOptionLabel->setTextColor(0xafffffff);
	spotlightOptionLabel->setSize(0.38f, 0.1f);
	spotlightOptionLabel->setTextSize(5);
	spotlightOptionLabel->setRelativePosition(0.0f, -0.2f);
	spotlightOptionLabel->setTextAlignment(DT_LEFT| DT_VCENTER);

	isSpotlightOption = new Checkbox();
	isSpotlightOption->setCheckColor(0xcfffffff);
	isSpotlightOption->setColor(0x7f7f7f7f);
	isSpotlightOption->setSize(0.05f);
	isSpotlightOption->setRelativePosition(-0.05f, -0.2f);

	Label* phiLabel = new Label();
	phiLabel->setText(L"Phi");
	phiLabel->setTextColor(0xafffffff);
	phiLabel->setSize(0.38f, 0.1f);
	phiLabel->setTextSize(5);
	phiLabel->setRelativePosition(0.0f, -0.25f);
	phiLabel->setTextAlignment(DT_LEFT | DT_VCENTER);

	Label* thetaLabel = new Label();
	thetaLabel->setText(L"Theta");
	thetaLabel->setTextColor(0xafffffff);
	thetaLabel->setSize(0.38f, 0.1f);
	thetaLabel->setTextSize(5);
	thetaLabel->setRelativePosition(0.0f, -0.25f);
	thetaLabel->setTextAlignment(DT_CENTER | DT_VCENTER);

	phiSlider = new Slider();
	phiSlider->setMin(10);
	phiSlider->setMax(240);
	phiSlider->setLineColor(0xcf1f1f1f);
	phiSlider->setSliderColor(0x6f6f6f6f);
	phiSlider->setSize(0.11f, 0.03f);
	phiSlider->setLineThickness(0.005f);
	phiSlider->setSliderThickness(0.007f);
	phiSlider->setRelativePosition(-0.13f, -0.3f);

	thetaSlider = new Slider();
	thetaSlider->setMin(1);
	thetaSlider->setMax(120);
	thetaSlider->setLineColor(0xcf1f1f1f);
	thetaSlider->setSliderColor(0x6f6f6f6f);
	thetaSlider->setSize(0.11f, 0.03f);
	thetaSlider->setLineThickness(0.005f);
	thetaSlider->setSliderThickness(0.007f);
	thetaSlider->setRelativePosition(0.0f, -0.3f);

	placeCamAtLightButton = new SimpleButton();
	placeCamAtLightButton->setCaption(L"Cam To Light");
	placeCamAtLightButton->setSize(0.10f, 0.06f);
	placeCamAtLightButton->setColor(0x7f3f3f3f);
	placeCamAtLightButton->setRelativePosition(0.13f, -0.3f);
	placeCamAtLightButton->setFunctionPointer(camToLightClick);

	addLightButton = new SimpleButton();
	addLightButton->setCaption(L"Add Light");
	addLightButton->setSize(0.16f, 0.06f);
	addLightButton->setColor(0x8f1f1f1f);
	addLightButton->setRelativePosition(-0.1f, -0.4f);
	addLightButton->setFunctionPointer(addLightClick);

	removeLightButton = new SimpleButton();
	removeLightButton->setCaption(L"Remove Light");
	removeLightButton->setSize(0.16f, 0.06f);
	removeLightButton->setColor(0x7f3f3f3f);
	removeLightButton->setRelativePosition(0.1f, -0.4f);
	removeLightButton->setFunctionPointer(removeLightClick);
	frame->addComponent(frameLabel);
	
	frame->addComponent(ambientLabel);
	frame->addComponent(ambientRed);
	frame->addComponent(ambientBlue);
	frame->addComponent(ambientGreen);
	
	frame->addComponent(fogLabel);
	frame->addComponent(fogRed);
	frame->addComponent(fogGreen);
	frame->addComponent(fogBlue);
	frame->addComponent(fogStartLabel);
	frame->addComponent(fogEndLabel);
	frame->addComponent(fogStart);
	frame->addComponent(fogEnd);

	
	frame->addComponent(lightEditingLabel);
	frame->addComponent(lightRed);
	frame->addComponent(lightGreen);
	frame->addComponent(lightBlue);
	
	frame->addComponent(radiusLabel);
	frame->addComponent(ambientFactorLabel);
	frame->addComponent(radiusSlider);
	frame->addComponent(ambientFactorSlider);
	frame->addComponent(lightLockButton);
	frame->addComponent(isSpotlightOption);
	frame->addComponent(spotlightOptionLabel);
	frame->addComponent(phiLabel);
	frame->addComponent(thetaLabel);
	frame->addComponent(phiSlider);
	frame->addComponent(thetaSlider);
	frame->addComponent(placeCamAtLightButton);

	frame->addComponent(addLightButton);
	frame->addComponent(removeLightButton);
	frame->setPosition(0.75f, -0.5f);
}

#include "SLOG.h"
#include <sstream>
void addLightClick(void)
{
	S_LOG::write("LIGHT BUTTON PRESS");
	pse::EditorData* ed = pse::getEditorData();
	SCamera* cam = ed->editorCam;
	pse::WorldData* wd = pse::getWorldData();
	for (int i = 0; i < PSE_LIGHTS_MAX; i++)
	{
		if (wd->lights[i] == 0)
		{
			wd->lights[i] = new pse::Light();
			wd->lights[i]->light = SRender::getSingleton()->createLight();
			wd->lights[i]->light->setPosition(cam->x + cam->dx*3.0f, cam->y + cam->dy*3.0f, cam->z + cam->dz*3.0f);
			wd->lights[i]->handle = SRender::getSingleton()->createS3D();
			wd->lights[i]->handle->setMesh(lightHandleMesh);
			wd->lights[i]->handle->setPosition(
				wd->lights[i]->light->x,
				wd->lights[i]->light->y,
				wd->lights[i]->light->z);
			SLight* light = wd->lights[i]->light;
			light->ambientFactor = (float)ambientFactorSlider->getValue() / 100.0f;
			light->r = (float)lightRed->getValue() / 255.0f;
			light->g = (float)lightGreen->getValue() / 255.0f;
			light->b = (float)lightBlue->getValue() / 255.0f;
			
			light->radius = (float)radiusSlider->getValue();
			
			light->dx = ed->editorCam->dx;
			light->dy = ed->editorCam->dy;
			light->dz = ed->editorCam->dz;

			if (isSpotlightOption->getState()) light->type = SRENDER_LIGHT_TYPE_SPOTLIGHT;
			else light->type = SRENDER_LIGHT_TYPE_POINT;

			light->phi = (float)phiSlider->getValue() / 100.0f;
			light->theta = (float)thetaSlider->getValue() / 100.0f;
			break;
		}
	}
}

#include "psPhysics.h"
int pse::getLightFromClick(int x, int y)
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
	pse::Light** lights = pse::getWorldData()->lights;
	for (int i = 0; i < 500; i++)
	{
		p1 += rayDir*0.1f;
		//check for any collision with voxels. If so, get the fuck out.
		if ((int)p1.x >= 0 && (int)p1.x < pse::getWorldData()->voxelData->x())
			if ((int)p1.y >= 0 && (int)p1.y < pse::getWorldData()->voxelData->y())
				if ((int)p1.z >= 0 && (int)p1.z < pse::getWorldData()->voxelData->z())
					if (pse::getWorldData()->voxelData->data(p1.x, p1.y, p1.z)) break;

		for (int j = 0; j < PSE_LIGHTS_MAX; j++)
		{
			if (lights[j] == 0) continue;
			
			if (psPhysics::isPointWithinBox(p1, D3DXVECTOR3(lights[j]->light->x, lights[j]->light->y, lights[j]->light->z), D3DXVECTOR3(1.0f, 1.0f, 1.0f)))
			{
				//set all the sliders to this lights properties
				lightRed->setValue((int)(lights[j]->light->r*255.0f));
				lightGreen->setValue((int)(lights[j]->light->g*255.0f));
				lightBlue->setValue((int)(lights[j]->light->b*255.0f));
				radiusSlider->setValue((int)lights[j]->light->radius);
				ambientFactorSlider->setValue((int)(lights[j]->light->ambientFactor*100.0f));
				if (lights[j]->light->type == SRENDER_LIGHT_TYPE_SPOTLIGHT) isSpotlightOption->setState(true);
				else isSpotlightOption->setState(false);
				phiSlider->setValue((int)(lights[j]->light->phi*100.0f));
				thetaSlider->setValue((int)(lights[j]->light->theta*100.0f));
				return j;
			}
		}
	}
	/* No light was clicked on therefore return -1 */
	return -1;
}

void removeLightClick(void)
{
	pse::EditorData* ed = pse::getEditorData();
	pse::WorldData* wd = pse::getWorldData();

	int selected = ed->lightSelection;

	if (selected < 0) return;
	if (selected >= PSE_LIGHTS_MAX) return;
	if (wd->lights[selected] == 0) return;

	delete wd->lights[selected];
	wd->lights[selected] = 0;
	ed->lightSelection = -1;
}

void pse::setAllLightsUnselected(void)
{
	pse::WorldData* wd = pse::getWorldData();
	pse::EditorData* ed = pse::getEditorData();

	for (int i = 0; i < PSE_LIGHTS_MAX; i++)
	{
		if (wd->lights[i] != 0)
		{
			wd->lights[i]->handle->setMesh(lightHandleMesh);
		}
	}
	ed->lightSelection = -1;
}
void pse::shiftSelectedLightUp(void)
{
	pse::EditorData* ed = pse::getEditorData();
	pse::WorldData* wd = pse::getWorldData();
	if (ed->lightSelection < 0) return;
	pse::Light* light = wd->lights[ed->lightSelection];
	switch (ed->gravityConfig)
	{
	case 0:
		break;
	case 1:
		break;
	case 2:
		light->light->y+=1.0f;
		light->light->y = (int)wd->lights[ed->lightSelection]->light->y + 0.5f;
		light->handle->setPosition(light->light->x, light->light->y, light->light->z); 
	case 3:
		break;
	case 4:
		break;
	case 5:
		break;
	default:
		break;
	} 
}
void pse::shiftSelectedLightDown(void)
{
}
void pse::shiftSelectedLightLeft(void)
{
}
void pse::shiftSelectedLightRight(void)
{
}

void camToLightClick(void)
{
	pse::EditorData* ed = pse::getEditorData();
	pse::WorldData* wd = pse::getWorldData();
	if (ed->lightSelection < 0) return;
	if (wd->lights[ed->lightSelection] == 0) return;

	ed->editorCam->x = wd->lights[ed->lightSelection]->light->x;
	ed->editorCam->y = wd->lights[ed->lightSelection]->light->y;
	ed->editorCam->z = wd->lights[ed->lightSelection]->light->z;

	ed->editorCam->setDirection(wd->lights[ed->lightSelection]->light->dx, wd->lights[ed->lightSelection]->light->dy, wd->lights[ed->lightSelection]->light->dz, ed->gravityConfig);
}


void lightLockClick(void)
{
	pse::EditorData* ed = pse::getEditorData();
	pse::WorldData* wd = pse::getWorldData();

	if (ed->lightSelection < 0) return;
	if (wd->lights[ed->lightSelection] == 0) return;

	pse::Light* light = wd->lights[ed->lightSelection];

	//x axis
	float xLower = (float)((int)(light->light->x - 0.5f)) + 0.5f;
	float xHigher = (float)((int)(light->light->x + 0.5f)) + 0.5f;

	float xLowerDifference = light->light->x - xLower;
	float xHigherDifference = xHigher - light->light->x;

	light->light->x = (xLowerDifference > xHigherDifference) ? xHigher : xLower;
	

	//y axis
	float yLower = (float)((int)(light->light->y - 0.5f)) + 0.5f;
	float yHigher = (float)((int)(light->light->y + 0.5f)) + 0.5f;

	float yLowerDifference = light->light->y - yLower;
	float yHigherDifference = yHigher - light->light->y;

	light->light->y = (yLowerDifference > yHigherDifference) ? yHigher : yLower;


	//z axis
	float zLower = (float)((int)(light->light->z - 0.5f)) + 0.5f;
	float zHigher = (float)((int)(light->light->z + 0.5f)) + 0.5f;

	float zLowerDifference = light->light->z - zLower;
	float zHigherDifference = zHigher - light->light->z;

	light->light->z = (zLowerDifference > zHigherDifference) ? zHigher : zLower;



	light->handle->setPosition(light->light->x, light->light->y, light->light->z);

	psMath::float3 lightDirection;
	lightDirection.x = light->light->dx;
	lightDirection.y = light->light->dy;
	lightDirection.z = light->light->dz;

	psMath::float3 newLightDirection;
	newLightDirection = psMath::dominantUnitVector(&lightDirection);
}