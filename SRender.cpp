#include "SRender.h"

#include "SAnimatedMesh.h"
#include "SDeferredRenderer.h"
//some globals that should be invisible to everything besides SRENDER
LPDIRECT3DDEVICE9 gDev;
SRender* SRender::singleton;
SRender* SRender::getSingleton(void){
	return singleton;
}
bool SRender::createSingleton(){
	singleton = 0;
	singleton = new SRender();
	return true;
}
SRender::SRender(void)
{
}

SRender::~SRender(void)
{
}

void SRender::setup(LPDIRECT3DDEVICE9 device)
{
	dev = device;
	gDev = dev;

	camera = 0;
	setupConfigs();
	setupRenderTargets();
	setupQuad();
	setupDefaultMaterial();
	setupObjectContainers();
	setupMeshContainers();
	setupMaterialSetContainers();
	//setupShaders();
	//class specifics
	S2D::setupModule(dev);
	SText::setupModule(dev);

	deferredRenderingObject = new SDeferredRenderer();
	particleSystemObject = new SParticleSystem(dev);
}

void SRender::render(void)
{
	dev->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clearColor, 1.0f, 0);
	if (camera!=0){
		D3DXMatrixLookAtLH(&matView, &camera->getPositionX(), &camera->getLookAtX(), &camera->getUpVectorX());
		D3DXMatrixPerspectiveFovLH(&matProj, camera->fov, (float)iWidth/(float)iHeight, fZNear, fZFar);
	} else {
		D3DXMatrixIdentity(&matView);
		D3DXMatrixPerspectiveFovLH(&matProj, 0.8f, (float)iWidth/(float)iHeight, fZNear, fZFar);
	}
	

	dev->SetTransform(D3DTS_VIEW, &matView);
	dev->SetTransform(D3DTS_PROJECTION, &matProj);
	
	dev->SetMaterial(&defaultMaterial);
	
	
	dev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	dev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
	dev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_ANISOTROPIC);
	dev->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 16);

	if (wireframe) dev->SetRenderState(D3DRS_FILLMODE, 2);
	else dev->SetRenderState(D3DRS_FILLMODE, 3);

	//Perform Deferred pass - Render S3Ds (Deferred Only)
	deferredRenderingObject->render();

	dev->SetRenderState(D3DRS_ZENABLE, TRUE);
	//Render S3Ds - Stage 2
	for (int i = 0; i < SRENDER_OBJECTCONTAINERSIZE; i++) 
	{
		if (obj3Ds[i] != 0) obj3Ds[i]->render(dev, 0);
	}

	//Render particles
	particleSystemObject->render();


	//Render S2Ds - Stage 3
	for (int i = 0; i < SRENDER_OBJECTCONTAINERSIZE; i++)
	{
		if (obj2Ds[i] != 0) obj2Ds[i]->render(dev);
	}
	//Render STexts - Stage 4
	for (int i = 0; i < SRENDER_OBJECTCONTAINERSIZE; i++)
	{
		if (objTexts[i] != 0) objTexts[i]->render(dev);
	}

	
	//now that all is finished, we want to set fill the "OLD" matrices
	oldMatView = matView;
	oldMatProj = matProj;
}

void SRender::destroy(void)
{
}

#include "SConfigs.h"
#include "SLOG.h"
void SRender::setupConfigs(void)
{
	bBloomEnabled = true;

	iWidth = S_Configs::s_bufferX;
	iHeight = S_Configs::s_bufferY;

	fZNear = 0.1f;
	fZFar = 120.0f;

	fFOV = 0.8f;

	clearColor = 0xffffffff;

	fogColor[0] = 1.0f;
	fogColor[1] = 1.0f;
	fogColor[2] = 1.0f;
	fFogStart = 10.0f;
	fFogEnd = 75.0f;

	focalDistance = 16.0f;
	focalWidth = 32.0f;
	wireframe = false;

	//setup vertex declarations

	//POSITION, NORMAL, TANGENT, BINORMAL, TEXCOORD
	D3DVERTEXELEMENT9 vd1[] =
	{
		{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
		{0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0},
		{0, 36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0},
		{0, 48, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};
	//POSITION, TEXCOORD
	D3DVERTEXELEMENT9 vd2[] =
	{
		{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};

	//POSITION, Diffuse, TEXCOORD
	D3DVERTEXELEMENT9 vd3[] =
	{
		{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
		{0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};
	_VD_POS_NOR_TX1_TAN_BIN = 0;
	_VD_POS_TX1 = 0;
	_VD_POS_DIF_TX1 = 0;
	dev->CreateVertexDeclaration(vd1, &_VD_POS_NOR_TX1_TAN_BIN);
	dev->CreateVertexDeclaration(vd2, &_VD_POS_TX1);
	dev->CreateVertexDeclaration(vd3, &_VD_POS_DIF_TX1);
	if (_VD_POS_NOR_TX1_TAN_BIN == 0) S_LOG::write("_VD_POS_NOR_TX1_TAN_BIN NOT LOADED!");
	if (_VD_POS_TX1 == 0) S_LOG::write("_VD_POS_TX1 NOT LOADED!");
	if (_VD_POS_DIF_TX1 == 0) S_LOG::write("_VD_POS_DIF_TX1 NOT LOADED!");
}
void SRender::setupRenderTargets(void)
{
	dev->CreateTexture(iWidth, iHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &targetBase, 0);
	dev->CreateTexture(iWidth, iHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &targetBloom, 0);
	dev->CreateTexture(iWidth, iHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &targetFinal, 0);

	targetBase->GetSurfaceLevel(0, &surfaceBase);
	targetBloom->GetSurfaceLevel(0, &surfaceBloom);
	targetFinal->GetSurfaceLevel(0, &surfaceFinal);
}

void SRender::setupQuad(void)
{
	dev->CreateVertexBuffer(sizeof(TexturedVertex)*4, 0, 0, D3DPOOL_DEFAULT, &quadVertexBuffer, 0);
	dev->CreateIndexBuffer(sizeof(short)*6, 0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &quadIndexBuffer, 0);
	ColoredTexturedVertex* verts = new ColoredTexturedVertex[4];
	ZeroMemory(verts, sizeof(ColoredTexturedVertex)*4);
	short* ind = new short[6];

	quadVertexBuffer->Lock(0, 0, (void**)&verts, 0);
	verts[0].x = -1.0f; verts[0].y = -1.0f; verts[0].z = 0.0f; verts[0].u1 = 0.0f; verts[0].v1 = 1.0f;
	verts[1].x = 1.0f; verts[1].y = -1.0f; verts[1].z = 0.0f; verts[1].u1 = 1.0f; verts[1].v1 = 1.0f;
	verts[2].x = -1.0f; verts[2].y = 1.0f; verts[2].z = 0.0f; verts[2].u1 = 0.0f; verts[2].v1 = 0.0f;
	verts[3].x = 1.0f; verts[3].y = 1.0f; verts[3].z = 0.0f; verts[3].u1 = 1.0f; verts[3].v1 = 0.0f;
	quadVertexBuffer->Unlock();
	quadIndexBuffer->Lock(0,0, (void**)&ind, 0);
	ind[0] = 0; ind[1] = 1; ind[2] = 2;
	ind[3] = 3; ind[4] = 2; ind[5] = 1;
	quadIndexBuffer->Unlock();
}

void SRender::setupDefaultMaterial(void)
{
	ZeroMemory(&defaultMaterial, sizeof(D3DMATERIAL9));

	defaultMaterial.Ambient.a = 1.0f;
	defaultMaterial.Ambient.r = 1.0f;
	defaultMaterial.Ambient.g = 1.0f;
	defaultMaterial.Ambient.b = 1.0f;

	defaultMaterial.Diffuse.a = 1.0f;
	defaultMaterial.Diffuse.r = 1.0f;
	defaultMaterial.Diffuse.g = 1.0f;
	defaultMaterial.Diffuse.b = 1.0f;

	defaultMaterial.Specular.a = 1.0f;
	defaultMaterial.Specular.r = 1.0f;
	defaultMaterial.Specular.g = 1.0f;
	defaultMaterial.Specular.b = 1.0f; 
}
void SRender::setupObjectContainers(void)
{
	int size = SRENDER_OBJECTCONTAINERSIZE;
	obj3Ds = new S3D*[size];
	obj2Ds = new S2D*[size];
	objTexts = new SText*[size];
	objLights = new SLight*[size];
	lightEffects = new LightEffect*[size];
	for (int i = 0; i < size; i++){
		//set pointers in array to null
		obj3Ds[i] = 0;
		obj2Ds[i] = 0;
		objTexts[i] = 0;
		objLights[i] = 0;
		lightEffects[i] = 0;
	}
}
void SRender::setupMeshContainers(void)
{
	int size = SRENDER_MESHCONTAINERSIZE;
	meshes = new SMesh*[size];
	for (int i = 0; i < size; i++)
	{
		meshes[i] = 0;
	}
}
void SRender::setupMaterialSetContainers(void)
{
	int size = SRENDER_MATERIALSETCONTAINERSIZE;
	materialSets = new SMaterialSet*[size];
	for (int i = 0; i < size; i++){
		//set pointers in array to null
		materialSets[i] = 0;
	}
}

//Material Handling
SMaterialSet* SRender::createMaterialSet(std::wstring path)
{
	SMaterialSet* set = new SMaterialSet(path);
	return set;
}


SMesh* SRender::createMesh(void)
{
	SMesh* obj = new SMesh();
	//look through container to find available spots
	bool wasSuccessful = false;
	for (int i = 0; i < SRENDER_MESHCONTAINERSIZE; i++){
		if (meshes[i] == 0){
			meshes[i] = obj;
			meshes[i]->setDevice(dev);
			wasSuccessful = true;
			break;
		}
	}
	if (wasSuccessful) return obj;
	delete obj;
	return 0;
}

SMutableMesh* SRender::createMutableMesh(void)
{
	SMutableMesh* obj = new SMutableMesh();
	//look through container to find available spots
	bool wasSuccessful = false;
	for (int i = 0; i < SRENDER_MESHCONTAINERSIZE; i++){
		if (meshes[i] == 0){
			meshes[i] = obj;
			meshes[i]->setDevice(dev);
			wasSuccessful = true;
			break;
		}
	}
	if (wasSuccessful) return obj;
	delete obj;
	return 0;
}

SGridMesh* SRender::createGridMesh(void)
{
	SGridMesh* obj = new SGridMesh();
	//look through container to find available spots
	bool wasSuccessful = false;
	for (int i = 0; i < SRENDER_MESHCONTAINERSIZE; i++){
		if (meshes[i] == 0){
			meshes[i] = obj;
			meshes[i]->setDevice(dev);
			wasSuccessful = true;
			break;
		}
	}
	if (wasSuccessful) return obj;
	delete obj;
	return 0;
}

SLineBoxMesh* SRender::createLineBoxMesh(void)
{
	SLineBoxMesh* obj = new SLineBoxMesh();
	//look through container to find available spots
	bool wasSuccessful = false;
	for (int i = 0; i < SRENDER_MESHCONTAINERSIZE; i++){
		if (meshes[i] == 0){
			meshes[i] = obj;
			meshes[i]->setDevice(dev);
			wasSuccessful = true;
			break;
		}
	}
	if (wasSuccessful) return obj;
	delete obj;
	return 0;
}

SAnimatedMesh* SRender::createAnimatedMesh(void)
{
	SAnimatedMesh* obj = new SAnimatedMesh();
	//look through container to find available spots
	bool wasSuccessful = false;
	for (int i = 0; i < SRENDER_MESHCONTAINERSIZE; i++){
		if (meshes[i] == 0){
			meshes[i] = obj;
			meshes[i]->setDevice(dev);
			wasSuccessful = true;
			break;
		}
	}
	if (wasSuccessful) return obj;
	delete obj;
	return 0;
}
void SRender::releaseMesh(SMesh* mesh)
{
	for (int i = 0; i < SRENDER_MESHCONTAINERSIZE; i++)
	{
		if (meshes[i] == mesh)
		{
			delete meshes[i];
			meshes[i] = 0;
			break;
		}
	}
}

S3D* SRender::createS3D(void)
{
	S3D* obj = new S3D();
	bool wasSuccessful = false;
	for (int i = 0; i < SRENDER_OBJECTCONTAINERSIZE; i++){
		if (obj3Ds[i] == 0){
			obj3Ds[i] = obj;
			wasSuccessful = true;
			break;
		}
	}
	if (wasSuccessful) return obj;
	delete obj;
	return 0;
}

void SRender::releaseS3D(S3D* s3d)
{
	for (int i = 0; i < SRENDER_OBJECTCONTAINERSIZE; i++)
	{
		if (obj3Ds[i] == s3d)
		{
			delete obj3Ds[i];
			obj3Ds[i] = 0;
			break;
		}
	}
}

S2D* SRender::createS2D(void)
{
	S2D* obj = new S2D();
	//look through container to find available spots
	bool wasSuccessful = false;
	for (int i = 0; i < SRENDER_OBJECTCONTAINERSIZE; i++){
		if (obj2Ds[i] == 0){
			obj2Ds[i] = obj;
			wasSuccessful = true;
			break;
		}
	}
	if (wasSuccessful) return obj;
	delete obj;
	return 0;
}

void SRender::releaseS2D(S2D* s2d)
{
	for (int i = 0; i < SRENDER_OBJECTCONTAINERSIZE; i++)
	{
		if (obj2Ds[i] == s2d)
		{
			delete obj2Ds[i];
			obj2Ds[i] = 0;
			break;
		}
	}
}

void SRender::releaseMaterialSet(SMaterialSet* materialSet)
{
	delete materialSet;//kind of pointless for now. Delete can be used or this.
}

SText* SRender::createText(void)
{
	SText* obj = new SText();
	//look through container to find available spots
	bool wasSuccessful = false;
	for (int i = 0; i < SRENDER_OBJECTCONTAINERSIZE; i++){
		if (objTexts[i] == 0){
			objTexts[i] = obj;
			wasSuccessful = true;
			break;
		}
	}
	if (wasSuccessful) return obj;
	delete obj;
	return 0;
}

void SRender::releaseText(SText* text)
{
	for (int i = 0; i < SRENDER_OBJECTCONTAINERSIZE; i++)
	{
		if (objTexts[i] == text)
		{
			delete objTexts[i];
			objTexts[i] = 0;
			break;
		}
	}
}

SLight* SRender::createLight(void)
{
	SLight* l = new SLight();
	bool wasSuccessful = false;
	for (int i = 0; i < SRENDER_OBJECTCONTAINERSIZE; i++)
	{
		if (objLights[i] == 0)
		{
			objLights[i] = l;
			wasSuccessful = true;
			break;
		}
	}
	if (wasSuccessful) return l;
	delete l;
	return 0;
}

SLight* SRender::createLight(SLight& referenceLight)
{
	SLight* l = new SLight(referenceLight);
	bool wasSuccessful = false;
	for (int i = 0; i < SRENDER_OBJECTCONTAINERSIZE; i++)
	{
		if (objLights[i] == 0)
		{
			objLights[i] = l;
			wasSuccessful = true;
			break;
		}
	}
	if (wasSuccessful) return l;
	delete l;
	return 0;
}
SLight** SRender::getStaticLights(void)
{
	return objLights;
}
void SRender::releaseLight(SLight* l)
{
	for (int i = 0; i < SRENDER_OBJECTCONTAINERSIZE; i++)
	{
		if (objLights[i] == l)
		{
			delete objLights[i];
			objLights[i] = 0;
			break;
		}
	}
}

void SRender::setCamera(SCamera* camera)
{
	this->camera = camera;
}

SCamera* SRender::getCurrentCamera(void)
{
	return camera;
}
void SRender::setWireframe(bool b)
{
	wireframe = b;
}

void SRender::setClearColor(DWORD color)
{
	clearColor = color;
}

void SRender::setProjection(float fov, float nearZ, float farZ)
{
	float aspect = (float)iWidth / (float)iHeight;
	D3DXMatrixPerspectiveFovLH(&matProj, fov, aspect, nearZ, farZ);
}

D3DXMATRIX SRender::getProjection(void)
{
	return matProj;
}

D3DXMATRIX SRender::getView(void)
{
	D3DXMATRIX matView;
	if (camera==0)
	{
		D3DXMatrixIdentity(&matView);
		return matView;
	}
	
	D3DXVECTOR3 pos = camera->getPositionX();
	D3DXVECTOR3 at = camera->getLookAtX();
	D3DXVECTOR3 up = camera->getUpVectorX();
	D3DXMatrixLookAtLH(&matView, &pos, &at, &up);
	return matView;
}

float SRender::getWidth(void)
{
	return (float)iWidth;
}

float SRender::getHeight(void)
{
	return (float)iHeight;
}

void SRender::setAmbientColor(char r, char g, char b)
{
	dev->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(r, g, b));
	ambientColor[0] = (float)r/255.0f;
	ambientColor[1] = (float)g/255.0f;
	ambientColor[2] = (float)b/255.0f;
}

void SRender::setAmbientColor(float r, float g, float b)
{
	ambientColor[0] = r;
	ambientColor[1] = g;
	ambientColor[2] = b;
}


void SRender::setFogColor(float r, float g, float b)
{
	fogColor[0] = r;
	fogColor[1] = g;
	fogColor[2] = b;
}

void SRender::setFogStart(float f)
{
	fFogStart = f;
}
void SRender::setFogEnd(float f)
{
	fFogEnd = f;
}

void SRender::setLight(int index, const SLight& light)
{
	if (index < 0) return;
	if (index > SRENDER_MAXLIGHTS -1) return;


	//Fixed Function Pipeline case
	D3DLIGHT9 dLight;
	ZeroMemory(&dLight, sizeof(D3DLIGHT9));
	dLight.Type = D3DLIGHT_POINT;
	dLight.Position.x = light.x;
	dLight.Position.y = light.y;
	dLight.Position.z = light.z;
	dLight.Attenuation0 = light.att0;
	dLight.Attenuation1 = light.att1;
	dLight.Attenuation2 = light.att2;
	dLight.Diffuse.r = light.r;
	dLight.Diffuse.g = light.g;
	dLight.Diffuse.b = light.b;
	dLight.Diffuse.a = 1.0f;
	dLight.Ambient.r = light.r * light.ambientFactor;
	dLight.Ambient.g = light.g * light.ambientFactor;
	dLight.Ambient.b = light.b * light.ambientFactor;
	dLight.Ambient.a = 1.0f;
	dLight.Range = light.radius;
	dLight.Specular.a = 1.0f;
	dLight.Specular.r = 0.0f;
	dLight.Specular.g = 0.0f;
	dLight.Specular.b = 0.0f;
	dev->SetLight(index, &dLight);
	dev->LightEnable(index, TRUE);
}

int SRender::d_GetS2DUsed(void)
{
	int count = 0;
	for (int i = 0; i < SRENDER_OBJECTCONTAINERSIZE; i++)
	{
		if (obj2Ds[i] != 0) count++;
	}
	return count;
}
//int SRender::d_GetS3DUsed(void);
int SRender::d_GetSTextUsed(void)
{
	int count = 0;
	for (int i = 0; i < SRENDER_OBJECTCONTAINERSIZE; i++)
	{
		if (objTexts[i] != 0) count++;
	}
	return count;
}

void SRender::createLightEffect(
	float x, float y, float z,
	float dx, float dy, float dz,
	float ax, float ay, float az,
	float cr, float cg, float cb)
{
	for (int i = 0; i < SRENDER_OBJECTCONTAINERSIZE; i++)
	{
		if (lightEffects[i] == 0)
		{
			lightEffects[i] = new LightEffect();
			lightEffects[i]->x = x;
			lightEffects[i]->y = y;
			lightEffects[i]->z = z;

			lightEffects[i]->dx = dx;
			lightEffects[i]->dy = dy;
			lightEffects[i]->dz = dz;

			lightEffects[i]->ax = ax;
			lightEffects[i]->ay = ay;
			lightEffects[i]->az = az;

			lightEffects[i]->color[0] = cr;
			lightEffects[i]->color[1] = cg;
			lightEffects[i]->color[2] = cb;

			lightEffects[i]->lifetime = 0.03f;
			lightEffects[i]->radius = 15.0f;
			break;
		}
	}
}


void SRender::createLightEffect(LightEffectData* data)
{
	for (int i = 0; i < SRENDER_OBJECTCONTAINERSIZE; i++)
	{
		if (lightEffects[i] == 0)
		{
			lightEffects[i] = new LightEffect();
			lightEffects[i]->x = data->x;
			lightEffects[i]->y = data->y;
			lightEffects[i]->z = data->z;

			lightEffects[i]->dx = data->dx;
			lightEffects[i]->dy = data->dy;
			lightEffects[i]->dz = data->dz;

			lightEffects[i]->ax = data->ax;
			lightEffects[i]->ay = data->ay;
			lightEffects[i]->az = data->az;

			lightEffects[i]->color[0] = data->color[0];
			lightEffects[i]->color[1] = data->color[1];
			lightEffects[i]->color[2] = data->color[2];

			lightEffects[i]->lifetime = data->lifetime;
			lightEffects[i]->radius = data->radius;
			lightEffects[i]->dColor[0] = data->dColor[0];
			lightEffects[i]->dColor[1] = data->dColor[1]; 
			lightEffects[i]->dColor[2] = data->dColor[2];
			break;
		}
	}
}
//getting vertex declarations from outside if need be... completely safe.
LPDIRECT3DVERTEXDECLARATION9 SRender::VD_POS_NOR_TX1_TAN_BIN(void){return _VD_POS_NOR_TX1_TAN_BIN;}
LPDIRECT3DVERTEXDECLARATION9 SRender::VD_POS_TX1(void){return _VD_POS_TX1;}
LPDIRECT3DVERTEXDECLARATION9 SRender::VD_POS_DIF_TX1(void){return _VD_POS_DIF_TX1;}



void SRender::emitParticle(SParticle* data)
{
	particleSystemObject->emitParticle(data);
}

SParticleSystem* SRender::getParticleSystem(void)
{
	return particleSystemObject;
}