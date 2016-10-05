#include "SDeferredRenderer.h"

#include "SRender.h"

#include "SLOG.h"
#include <sstream>
#include "SInput.h"//remove when not testing

#include "GameConfigInterface.h"
SDeferredRenderer::SDeferredRenderer(void)
{
	_sh_ClearGBuffer = 0;
	_setup();
}

SDeferredRenderer::~SDeferredRenderer(void)
{
}

bool SDeferredRenderer::_setup(void)
{
	SRender* r = SRender::getSingleton();
	/* create point light mesh */
	HRESULT hr = D3DXLoadMeshFromX(L"../Resources/Meshes/sphere.x", D3DXMESH_SYSTEMMEM, r->dev, NULL, NULL, NULL, NULL, &_geom_point);
	hr = D3DXLoadMeshFromX(L"../Resources/Meshes/spotlight.x", D3DXMESH_SYSTEMMEM, r->dev, NULL, NULL, NULL, NULL, &_geom_spotlight);
	/* create screen quad */
	r->dev->CreateVertexBuffer(sizeof(TexturedVertex)*4, 0, SRENDER_POSTX1, D3DPOOL_MANAGED, &_vb_quad, 0);
	r->dev->CreateIndexBuffer(sizeof(short)*6, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &_ib_quad, 0);
	TexturedVertex* verts;
	short* ind;

	_vb_quad->Lock(0, 0, (void**)&verts, 0);
	verts[0].x = -1.0f; verts[0].y = -1.0f; verts[0].z = 1.0f; verts[0].u = 0.0f; verts[0].v = 1.0f;
	verts[1].x = 1.0f; verts[1].y = -1.0f; verts[1].z = 1.0f; verts[1].u = 1.0f; verts[1].v = 1.0f;
	verts[2].x = -1.0f; verts[2].y = 1.0f; verts[2].z = 1.0f; verts[2].u = 0.0f; verts[2].v = 0.0f;
	verts[3].x = 1.0f; verts[3].y = 1.0f; verts[3].z = 1.0f; verts[3].u = 1.0f; verts[3].v = 0.0f;
	_vb_quad->Unlock();
	_ib_quad->Lock(0,0, (void**)&ind, 0);
	ind[0] = 0; ind[1] = 1; ind[2] = 2;
	ind[3] = 3; ind[4] = 2; ind[5] = 1;
	_ib_quad->Unlock();

	/* -------------------------------------------- */
	/* create special noise texture for the SSAO effect */
	D3DXCreateTextureFromFile(r->dev, L"../Resources/Textures/SSAONoiseBig.png", &_SSAO_noise);
	/* Create Deferred Rendering RT's and SF's + Depth Stencil Surface*/
	int width, height;
	int width_2, height_2;
	width = r->iWidth;
	height = r->iHeight;
	width_2 = width / 2;
	height_2 = height / 2;

	//D3DFMT_A8R8G8B8;
	//D3DFMT_A16B16G16B16
	D3DXCreateTexture(r->dev, width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &_rt_GBuffer[0]);
	D3DXCreateTexture(r->dev, width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &_rt_GBuffer[1]);
	D3DXCreateTexture(r->dev, width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &_rt_GBuffer[2]);
	D3DXCreateTexture(r->dev, width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &_rt_GBuffer[3]);

	D3DXCreateTexture(r->dev, width, height, 1, D3DUSAGE_RENDERTARGET, /*D3DFMT_A16B16G16R16*/D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &_rt_LightBuffer);

	D3DXCreateTexture(r->dev, width_2, height_2, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &_rt_BloomBuffer);

	D3DXCreateTexture(r->dev, width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &_rt_BloomBlurChain[0]);
	D3DXCreateTexture(r->dev, width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &_rt_BloomBlurChain[1]);
	D3DXCreateTexture(r->dev, width_2, height_2, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16, D3DPOOL_DEFAULT, &_rt_BloomBlurChain[2]);

	D3DXCreateTexture(r->dev, width_2, height_2, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &_rt_SSAO);
	D3DXCreateTexture(r->dev, width_2, height_2, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &_rt_SSAOBlur);

	D3DXCreateTexture(r->dev, width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16, D3DPOOL_DEFAULT, &_rt_MotionBlur);

	D3DXCreateTexture(r->dev, width_2, height_2, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &_rt_downscaledScene);
	D3DXCreateTexture(r->dev, width_2, height_2, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &_rt_downscaledSceneBlurredH);
	D3DXCreateTexture(r->dev, width_2, height_2, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &_rt_downscaledSceneBlurredHV);

	D3DXCreateTexture(r->dev, width, height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8B8G8R8, D3DPOOL_DEFAULT, &_rt_Final);


	_rt_GBuffer[0]->GetSurfaceLevel(0, &_sf_GBuffer[0]);
	_rt_GBuffer[1]->GetSurfaceLevel(0, &_sf_GBuffer[1]);
	_rt_GBuffer[2]->GetSurfaceLevel(0, &_sf_GBuffer[2]);
	_rt_GBuffer[3]->GetSurfaceLevel(0, &_sf_GBuffer[3]);

	_rt_LightBuffer->GetSurfaceLevel(0, &_sf_LightBuffer);

	_rt_BloomBuffer->GetSurfaceLevel(0, &_sf_BloomBuffer);

	_rt_BloomBlurChain[0]->GetSurfaceLevel(0, &_sf_BloomBlurChain[0]);
	_rt_BloomBlurChain[1]->GetSurfaceLevel(0, &_sf_BloomBlurChain[1]);
	_rt_BloomBlurChain[2]->GetSurfaceLevel(0, &_sf_BloomBlurChain[2]);

	_rt_SSAO->GetSurfaceLevel(0, &_sf_SSAO);
	_rt_SSAOBlur->GetSurfaceLevel(0, &_sf_SSAOBlur);

	_rt_MotionBlur->GetSurfaceLevel(0, &_sf_MotionBlur);

	_rt_downscaledScene->GetSurfaceLevel(0, &_sf_downscaledScene);
	_rt_downscaledSceneBlurredH->GetSurfaceLevel(0, &_sf_downscaledSceneBlurredH);
	_rt_downscaledSceneBlurredHV->GetSurfaceLevel(0, &_sf_downscaledSceneBlurredHV);

	_rt_Final->GetSurfaceLevel(0, &_sf_Final);

	/* ---------------------------------------------------------------------------- */

	/* Compile Shaders */
	if (!compileFX(L"../Resources/Shaders/cleargbuffer.fx", &_sh_ClearGBuffer)) return false;
	if (!compileFX(L"../Resources/Shaders/gbuffer.fx", &_sh_GBuffer)) return false;
	if (!compileFX(L"../Resources/Shaders/point.fx", &_sh_Light_Point)) return false;
	if (!compileFX(L"../Resources/Shaders/spotlight.fx", &_sh_Light_Spotlight)) return false;
	if (!compileFX(L"../Resources/Shaders/depthWrite.fx", &_sh_Light_DepthWrite)) return false;
	if (!compileFX(L"../Resources/Shaders/ssaoOLD.fx", &_sh_SSAO)) return false;
	if (!compileFX(L"../Resources/Shaders/ssaoBlur.fx", &_sh_SSAOblur)) return false;
	if (!compileFX(L"../Resources/Shaders/bloomBlurHorizontalLarger.fx", &_sh_Bloom_Horizontal)) return false;
	if (!compileFX(L"../Resources/Shaders/bloomBlurVerticalLarger.fx", &_sh_Bloom_Vertical)) return false;
	if (!compileFX(L"../Resources/Shaders/gaussBlurX.fx", &_sh_gaussBlurX)) return false;
	if (!compileFX(L"../Resources/Shaders/gaussBlurY.fx", &_sh_gaussBlurY)) return false;
	if (!compileFX(L"../Resources/Shaders/motionBlur.fx", &_sh_MotionBlur)) return false;
	if (!compileFX(L"../Resources/Shaders/downscale16.fx", &_sh_downscale16)) return false;
	if (!compileFX(L"../Resources/Shaders/final.fx", &_sh_Compose)) return false;
	if (!compileFX(L"../Resources/Shaders/combineSceneDOF.fx", &_sh_combineFinalDOF)) return false;




	//create random ssao sample kernel

	for (int i = 0; i < 64; i++)
	{
		ssaoKernel[i] = psMath::float3(
			(float)(rand()%20000) / 10000.0f - 1.0f, 
			(float)(rand()%20000) / 10000.0f - 1.0f,
			(float)(rand()%20000) / 10000.0f - 1.0f);

		psMath::normalize(&ssaoKernel[i]);

		ssaoKernel[i] *= (float)(rand()%10000) / 10000.0f;

		
		float scale = (float)i / 64.0f;
		scale = psMath::lerp(0.1f, 1.0f, scale * scale);
		ssaoKernel[i] *= scale;
	}



	//Create skybox vertex and index buffer shitaaaa
	skyboxMesh = SRender::getSingleton()->createMesh();
	skyboxMesh->createSkyboxMesh();

	skyboxMaterial = SRender::getSingleton()->createMaterialSet(L"../Resources/Themes/Default/Skybox");
	skyboxMesh->setMaterialSet(skyboxMaterial);

	skyboxObject = SRender::getSingleton()->createS3D();
	skyboxObject->setMesh(skyboxMesh);
	skyboxObject->setScale(70.0f);

}

bool SDeferredRenderer::compileFX(std::wstring path, LPD3DXEFFECT* effect)
{
	DWORD dwShaderFlags = D3DXFX_NOT_CLONEABLE | D3DXSHADER_NO_PRESHADER;
	LPD3DXBUFFER compilationErrors = 0;
	
	HRESULT hr = D3DXCreateEffectFromFile(SRender::getSingleton()->dev, path.c_str(), 0, 0, dwShaderFlags, 0, effect, &compilationErrors);
	if (FAILED(hr)){
		S_LOG::write("A shader was not loaded correctly. ERROR: ");
		
		char* data = new char[compilationErrors->GetBufferSize()];
		memcpy( data, compilationErrors->GetBufferPointer(), compilationErrors->GetBufferSize());
		S_LOG::write(data);
		delete data;
		return false;
	}
	return true;
}

void SDeferredRenderer::_render_ClearGBuffer(void)
{
	SRender* r = SRender::getSingleton();
	
	D3DXHANDLE technique = _sh_ClearGBuffer->GetTechniqueByName("Technique0");
	_sh_ClearGBuffer->SetTechnique(technique);
	r->dev->SetVertexDeclaration(r->VD_POS_TX1());

	r->dev->SetStreamSource(0, _vb_quad, 0, sizeof(TexturedVertex));
	r->dev->SetIndices(_ib_quad);
	_sh_ClearGBuffer->Begin(0, 0);
	_sh_ClearGBuffer->BeginPass(0);
	r->dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);
	_sh_ClearGBuffer->EndPass();
	_sh_ClearGBuffer->End();
}
void SDeferredRenderer::_render_GBuffer(void)
{
	SRender* r = SRender::getSingleton();
	int width = r->iWidth;
	int height = r->iHeight;

	

	#if 0
	
	//when filling the gbuffer, all sampler states should be awesome
	r->dev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
	r->dev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	r->dev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_ANISOTROPIC);
	r->dev->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 8);

	r->dev->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
	r->dev->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	r->dev->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_ANISOTROPIC);
	r->dev->SetSamplerState(1, D3DSAMP_MAXANISOTROPY, 8);

	r->dev->SetSamplerState(2, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
	r->dev->SetSamplerState(2, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	r->dev->SetSamplerState(2, D3DSAMP_MIPFILTER, D3DTEXF_ANISOTROPIC);
	r->dev->SetSamplerState(2, D3DSAMP_MAXANISOTROPY, 8);

#else
	
	r->dev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	r->dev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	r->dev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	r->dev->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	r->dev->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	r->dev->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	r->dev->SetSamplerState(2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	r->dev->SetSamplerState(2, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	r->dev->SetSamplerState(2, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
#endif
	r->dev->SetVertexDeclaration(r->VD_POS_NOR_TX1_TAN_BIN());
	static UINT totalPasses;
	static D3DXHANDLE hTechnique;
	hTechnique = _sh_GBuffer->GetTechniqueByName("technique0");

	
	_sh_GBuffer->SetTechnique(hTechnique);
	r->dev->SetFVF(SRENDER_POSNORDIFTX1);

	//first render skybox mesh
	skyboxObject->setPosition(r->getCurrentCamera()->x, r->getCurrentCamera()->y, r->getCurrentCamera()->z);
	//skyboxObject->render(r->dev, _sh_GBuffer);


	for (int i = 0; i < SRENDER_OBJECTCONTAINERSIZE; i++)
	{
		if (r->obj3Ds[i] != 0 && r->obj3Ds[i]->getMesh() != 0 && r->obj3Ds[i]->getMesh()->getDeferred())
		{
			r->obj3Ds[i]->render(r->dev, _sh_GBuffer);
		}
	}
			

		
		
		
	
}

void SDeferredRenderer::_render_ShadowMaps(void)
{
	SRender* r = SRender::getSingleton();
	

	static D3DXHANDLE hTechnique;
	hTechnique =_sh_Light_DepthWrite->GetTechniqueByName("technique0");
	_sh_Light_DepthWrite->SetTechnique(hTechnique);
	SLight** lightContainer = SRender::getSingleton()->objLights;
	S3D** objectContainer = SRender::getSingleton()->obj3Ds;
	int maxSize = SRENDER_OBJECTCONTAINERSIZE;
	int width = r->iWidth;
	int height = r->iHeight;
	r->dev->SetVertexDeclaration(r->VD_POS_NOR_TX1_TAN_BIN());
	//loop through all lights, if it is a spot light or has a shadow, draw its shadow map

	for (int i = 0; i < maxSize; i++)
	{
		if (lightContainer[i] == 0) continue;

		//remove if you want shadows
		
		if (lightContainer[i]->type == SRENDER_LIGHT_TYPE_SPOTLIGHT)
		{
			//First thing we want to do is clear the z buffer before the render. Now this means
			//that everything rendered after is kinda gonna go to shit. I'm going to have to try to
			//use a seperate depth buffer while doing this rendering to not fuck up the forward pass,
			//but I'm tired and just want to get a nice exponential shadow map on my screen as fast
			//as possible.
			r->dev->SetRenderTarget(0, lightContainer[i]->shadowMapSurface);
			r->dev->Clear(0,0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);

			D3DXMATRIX world, view, projection;//world is obviously filled in by object
			D3DXVECTOR3 lightPosition, lightLookAt;
			lightPosition.x = lightContainer[i]->x;
			lightPosition.y = lightContainer[i]->y;
			lightPosition.z = lightContainer[i]->z;

			lightLookAt.x = lightPosition.x + lightContainer[i]->dx;
			lightLookAt.y = lightPosition.y + lightContainer[i]->dy;
			lightLookAt.z = lightPosition.z + lightContainer[i]->dz;
			D3DXMatrixIdentity(&world);
			D3DXMatrixLookAtLH(&view, &lightPosition, &lightLookAt, &D3DXVECTOR3(0.0f, 1.0f, 0.0f));//constant up vector i guess
			D3DXMatrixPerspectiveFovLH(&projection, lightContainer[i]->phi, 1.0f, 0.1f, lightContainer[i]->radius);
			//Now because we want to output linear depth, we need to make a little change to the matrix.
			//projection._33/=lightContainer[i]->radius;
			//projection._43/=lightContainer[i]->radius;


			_sh_Light_DepthWrite->SetMatrix("world", &world);
			_sh_Light_DepthWrite->SetMatrix("view", &view);
			_sh_Light_DepthWrite->SetMatrix("projection", &projection);
			
			float pos[3]; pos[0] = lightPosition.x; pos[1] = lightPosition.y; pos[2] = lightPosition.z;
			_sh_Light_DepthWrite->SetValue("lightPosition", &pos, sizeof(float)*3);
			
			_sh_Light_DepthWrite->SetFloat("depthPrecision", lightContainer[i]->radius);

			for (int j = 0; j < SRENDER_OBJECTCONTAINERSIZE; j++)
			{
				if (objectContainer[j] != 0)
				{
					objectContainer[j]->renderShadowMap(_sh_Light_DepthWrite);
				}
			}
			if (S_INPUT::keyPressed(DIK_L))
			{
				std::wstringstream ss;
				ss.str(L"");
				ss<<"shadowmap"<<i<<".jpg";
				D3DXSaveTextureToFile(ss.str().c_str(), D3DXIFF_JPG, lightContainer[i]->shadowMap, 0);
			}

			r->dev->SetRenderTarget(0, lightContainer[i]->filteredShadowMapSurface);
			r->dev->Clear(0,0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);

			
			//set source texture size for both shaders
			float sourceTextureSize[2], sourceTextureSize_4[2];
			sourceTextureSize[0] = (float)1024.0f;
			sourceTextureSize[1] = (float)1024.0f;
			sourceTextureSize_4[0] = 256.0f * 0.5f;  
			sourceTextureSize_4[1] = 256.0f* 0.5f;
			_sh_gaussBlurX->SetValue("texSize", sourceTextureSize, sizeof(float)*2);
			_sh_gaussBlurX->SetValue("texSize", sourceTextureSize, sizeof(float)*2);
			_sh_gaussBlurX->SetFloat("inverseFactor", 1024.0f);
			_sh_gaussBlurY->SetFloat("inverseFactor", 1024.0f);
			//Set bilinear filtering for cleaner samples
			r->dev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
			r->dev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
			r->dev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
			//_sh_gaussBlurX->set
			D3DXHANDLE technique = _sh_gaussBlurX->GetTechniqueByName("technique0");
			_sh_gaussBlurX->SetTechnique(technique);
			r->dev->SetVertexDeclaration(r->VD_POS_TX1());
			r->dev->SetStreamSource(0, _vb_quad, 0, sizeof(TexturedVertex));
			r->dev->SetIndices(_ib_quad);
			r->dev->SetTexture(0, lightContainer[i]->shadowMap );
			_sh_gaussBlurX->Begin(0, 0);
			_sh_gaussBlurX->BeginPass(0);
			r->dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);
			_sh_gaussBlurX->EndPass();
			_sh_gaussBlurX->End();

			r->dev->SetRenderTarget(0, lightContainer[i]->shadowMapSurface);
			r->dev->Clear(0, 0, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0);
			r->dev->SetTexture(0, lightContainer[i]->filteredShadowMap);
			technique = _sh_gaussBlurY->GetTechniqueByName("technique0");
			_sh_gaussBlurY->SetTechnique(technique);
			_sh_gaussBlurY->Begin(0, 0);
			_sh_gaussBlurY->BeginPass(0);
			r->dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);
			_sh_gaussBlurY->EndPass();
			_sh_gaussBlurY->End();

			if (S_INPUT::keyPressed(DIK_L))
			{
				std::wstringstream ss;
				ss.str(L"");
				ss<<"shadowmapFILTER"<<i<<".jpg";
				D3DXSaveTextureToFile(ss.str().c_str(), D3DXIFF_JPG, lightContainer[i]->filteredShadowMap, 0);
			}
		}
	}

	
	
}

void SDeferredRenderer::_render_LightBuffer(void)
{

	SRender* r = SRender::getSingleton();
	//because this is right after the GBuffer pass, we want to disable all the sexy sampling
	/*r->dev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	r->dev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	r->dev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
	r->dev->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 16);

	r->dev->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	r->dev->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	r->dev->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
	r->dev->SetSamplerState(1, D3DSAMP_MAXANISOTROPY, 16);

	r->dev->SetSamplerState(2, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	r->dev->SetSamplerState(2, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	r->dev->SetSamplerState(2, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
	r->dev->SetSamplerState(2, D3DSAMP_MAXANISOTROPY, 16);

	r->dev->SetSamplerState(3, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	r->dev->SetSamplerState(3, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	r->dev->SetSamplerState(3, D3DSAMP_MIPFILTER, D3DTEXF_POINT);*/
	

	SLight** lightContainer = SRender::getSingleton()->objLights;
	int maxSize = SRENDER_OBJECTCONTAINERSIZE;
	int width = r->iWidth;
	int height = r->iHeight;
	
	r->dev->Clear(0, 0, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0,0,0,0), 1.0f, 0);

	D3DXMATRIX world, view, proj;
	D3DXMatrixLookAtLH(&view, &r->camera->getPositionX(), &r->camera->getLookAtX(), &r->camera->getUpVectorX());
	D3DXMatrixPerspectiveFovLH(&proj, r->camera->fov, (float)width/(float)height, 0.1f, 120.0f);
	D3DXMATRIX invView, inverseViewProjection;
	D3DXMatrixInverse(&invView, NULL, &view);
	D3DXMatrixInverse(&inverseViewProjection, NULL, &(view*proj));
	
	//technique setting for each shader
	static D3DXHANDLE hTechniquePoint = _sh_Light_Point->GetTechniqueByName("technique0");
	static D3DXHANDLE hTechniqueSpotlight = _sh_Light_Spotlight->GetTechniqueByName("technique0");
	_sh_Light_Point->SetTechnique(hTechniquePoint);
	_sh_Light_Spotlight->SetTechnique(hTechniqueSpotlight);

	//value setting for point light shader
	_sh_Light_Point->SetMatrix("view", &view);
	_sh_Light_Point->SetMatrix("projection", &proj);
	_sh_Light_Point->SetMatrix("inverseViewProjection", &inverseViewProjection);
	_sh_Light_Point->SetMatrix("inverseView", &invView);
	
	float cameraPos[3];
	cameraPos[0] = r->camera->getPositionX().x;
	cameraPos[1] = r->camera->getPositionX().y;
	cameraPos[2] = r->camera->getPositionX().z;
	_sh_Light_Point->SetValue("cameraPos", cameraPos, sizeof(float)*3);
	float GBufferTextureSize[2];
	GBufferTextureSize[0] = width;
	GBufferTextureSize[1] = height;
	_sh_Light_Point->SetValue("GBufferTextureSize",  GBufferTextureSize, sizeof(float)*2);
	

	//value setting for spotlight shader
	_sh_Light_Spotlight->SetMatrix("view", &view);
	_sh_Light_Spotlight->SetMatrix("projection", &proj);
	_sh_Light_Spotlight->SetMatrix("inverseViewProjection", &inverseViewProjection);
	_sh_Light_Spotlight->SetMatrix("inverseView", &invView);
	_sh_Light_Spotlight->SetValue("cameraPos", cameraPos, sizeof(float)*3);
	_sh_Light_Spotlight->SetValue("GBufferTextureSize",  GBufferTextureSize, sizeof(float)*2);
	//_sh_Light_Spotlight->SetFloat("depthBias", 1.0f / 2000.0f);//might need to change
	_sh_Light_Spotlight->SetFloat("shadowMapSize", 1024.0f);
	//both shaders use same texture registers
	r->dev->SetTexture(0, _rt_GBuffer[0]);
	r->dev->SetTexture(1, _rt_GBuffer[1]);
	r->dev->SetTexture(2, _rt_GBuffer[2]);
	r->dev->SetTexture(3, _rt_GBuffer[3]);
	//Render normal lights
	for (int i = 0; i < maxSize; i++){
		SLight* light = lightContainer[i];
		if (light==0) continue;

		if (light->type == SRENDER_LIGHT_TYPE_POINT)
		{
			//find if cam is in light

			float distance = sqrt((r->camera->getPositionX().x - light->x)*(r->camera->getPositionX().x - light->x) +
				(r->camera->getPositionX().y - light->y)*(r->camera->getPositionX().y - light->y) +
				(r->camera->getPositionX().z - light->z)*(r->camera->getPositionX().z - light->z));

			if (distance <= light->radius/2.0f + r->fZNear){ r->dev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW); r->dev->SetRenderState(D3DRS_ZENABLE, FALSE);}
			if (distance > light->radius/2.0f + r->fZNear){ r->dev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW); r->dev->SetRenderState(D3DRS_ZENABLE, TRUE);}


			D3DXMATRIX worldMat, scale;
			D3DXMatrixTranslation(&worldMat, light->x, light->y, light->z);


			D3DXMatrixScaling(&scale, light->radius/2.0f, light->radius/2.0f, light->radius/2.0f);
			_sh_Light_Point->SetMatrix("world", &(scale*worldMat));
			float lightPos[3];
			lightPos[0] = light->x;
			lightPos[1] = light->y;
			lightPos[2] = light->z;
			_sh_Light_Point->SetValue("lightPos", lightPos, sizeof(float)*3);
			float lightColor[4];
			lightColor[0] = light->r;
			lightColor[1] = light->g;
			lightColor[2] = light->b;
			lightColor[3] = 1.0f;
			_sh_Light_Point->SetValue("lightColor", lightColor, sizeof(float)*4);
			
			_sh_Light_Point->SetFloat("radius", light->radius);
			_sh_Light_Point->SetFloat("ambientFactor", light->ambientFactor);
			_sh_Light_Point->Begin(0, 0);
			_sh_Light_Point->BeginPass(0);
			_geom_point->DrawSubset(0);
			_sh_Light_Point->EndPass();
			_sh_Light_Point->End();
		}
		if (light->type == SRENDER_LIGHT_TYPE_SPOTLIGHT)
		{
			//find if cam is in the light



			r->dev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
			r->dev->SetTexture(0, light->shadowMap);

			float xyScale = tan(light->phi) * light->radius;
			D3DXMATRIX worldMat, scale, lightView, lightProjection;
			D3DXMATRIX coneRotationMatrix;
			D3DXMATRIX halfOffsetTranslation, rotationAboutY, rotationAboutX;
			D3DXVECTOR3 newPitchAxis(1.0f, 0.0f, 0.0f);

			D3DXMatrixRotationY(&rotationAboutY, atan2(light->dx, light->dz));
			D3DXVec3TransformCoord(&newPitchAxis, &newPitchAxis, &rotationAboutY);
			D3DXMatrixRotationAxis(&rotationAboutX, &newPitchAxis, atan2(-light->dy, light->dz));
			//D3DXMATRIX shiftZ;
			D3DXMatrixTranslation(&halfOffsetTranslation, 0.0f, 0.0f, 0.5f);
			D3DXMatrixTranslation(&worldMat, light->x, light->y, light->z);
			D3DXMatrixScaling(&scale, xyScale, xyScale, light->radius);
			D3DXMatrixScaling(&scale, light->radius / 2.0f, light->radius / 2.0f, light->radius / 2.0f);
			D3DXMatrixLookAtLH(&lightView, &D3DXVECTOR3(light->x, light->y, light->z), &D3DXVECTOR3(light->x + light->dx, light->y + light->dy, light->z + light->dz), &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
			D3DXMatrixLookAtLH(&coneRotationMatrix, &D3DXVECTOR3(0.0f, 0.0f, 0.0f), &D3DXVECTOR3(light->dx, light->dy, light->dz), &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
			D3DXMatrixPerspectiveFovLH(&lightProjection, light->phi, 1.0f, 0.1f, light->radius);
			//_sh_Light_Spotlight->SetMatrix("world", &(halfOffsetTranslation*scale*coneRotationMatrix**worldMat));
			_sh_Light_Spotlight->SetMatrix("world", &(scale*worldMat));
			_sh_Light_Spotlight->SetMatrix("lightViewProjection", &(lightView*lightProjection));
			float lightPos[3]; lightPos[0] = light->x; lightPos[1] = light->y; lightPos[2] = light->z;
			_sh_Light_Spotlight->SetValue("lightPosition", lightPos, sizeof(float)*3);
			
			float lightDir[3]; lightDir[0] = light->dx; lightDir[1] = light->dy; lightDir[2] = light->dz;
			_sh_Light_Spotlight->SetValue("lightDirection", lightDir, sizeof(float)*3);
			float lightColor[3]; lightColor[0] = light->r; lightColor[1] = light->g; lightColor[2] = light->b;
			_sh_Light_Spotlight->SetValue("lightColor", lightColor, sizeof(float)*3);
			_sh_Light_Spotlight->SetFloat("ambientFactor", light->ambientFactor);
			_sh_Light_Spotlight->SetFloat("theta", light->theta);
			_sh_Light_Spotlight->SetFloat("phi", light->phi);
			_sh_Light_Spotlight->SetFloat("radius", light->radius);
			_sh_Light_Spotlight->SetFloat("depthPrecision", light->radius);
			_sh_Light_Spotlight->SetValue("fogColor", r->fogColor, sizeof(float)*3);
			_sh_Light_Spotlight->SetFloat("fogStart", r->fFogStart);
			_sh_Light_Spotlight->SetFloat("fogEnd", r->fFogEnd);
			_sh_Light_Spotlight->Begin(0, 0);
			_sh_Light_Spotlight->BeginPass(0);
			_geom_point->DrawSubset(0);
			_sh_Light_Spotlight->EndPass();
			_sh_Light_Spotlight->End();

		}

	}
	
	//Render light effects
	_updateLightEffects();
	LightEffect** lightEffects = SRender::getSingleton()->lightEffects;
	for (int i = 0; i < SRENDER_OBJECTCONTAINERSIZE; i++){
		LightEffect* light = lightEffects[i];
		if (light==0) continue;

		//find if cam is in light
		
		float distance = sqrt((r->camera->getPositionX().x - light->x)*(r->camera->getPositionX().x - light->x) +
			(r->camera->getPositionX().y - light->y)*(r->camera->getPositionX().y - light->y) +
			(r->camera->getPositionX().z - light->z)*(r->camera->getPositionX().z - light->z));
		
		if (distance <= light->radius){ r->dev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW); r->dev->SetRenderState(D3DRS_ZENABLE, FALSE);}
		if (distance > light->radius){ r->dev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW); r->dev->SetRenderState(D3DRS_ZENABLE, TRUE);}
		

		D3DXMATRIX worldMat, scale;
		D3DXMatrixTranslation(&worldMat, light->x, light->y, light->z);
		
		
		D3DXMatrixScaling(&scale, light->radius, light->radius, light->radius);
		_sh_Light_Point->SetMatrix("world", &(scale*worldMat));
		float lightPos[3];
		lightPos[0] = light->x;
		lightPos[1] = light->y;
		lightPos[2] = light->z;
		_sh_Light_Point->SetValue("lightPos", lightPos, sizeof(float)*3);
		float lightColor[4];
		lightColor[0] = light->color[0];
		lightColor[1] = light->color[1];
		lightColor[2] = light->color[2];
		lightColor[3] = 1.0f;
		_sh_Light_Point->SetValue("lightColor", lightColor, sizeof(float)*4);
		
		_sh_Light_Point->SetFloat("radius", light->radius);
		_sh_Light_Point->Begin(0, 0);
		_sh_Light_Point->BeginPass(0);
		_geom_point->DrawSubset(0);
		_sh_Light_Point->EndPass();
		_sh_Light_Point->End();
	}
}

void SDeferredRenderer::_render_BloomBuffer(void)
{
	SRender* r = SRender::getSingleton();
	//set source texture size for both shaders
	float sourceTextureSize[2], sourceTextureSize_2[2];
	sourceTextureSize[0] = (float)r->iWidth;
	sourceTextureSize[1] = (float)r->iHeight;
	sourceTextureSize_2[0] = (float)r->iWidth * 0.5f;
	sourceTextureSize_2[1] = (float)r->iHeight * 0.5f;
	
	
	_sh_Bloom_Horizontal->SetValue("texSize", sourceTextureSize, sizeof(float)*2);
	_sh_Bloom_Vertical->SetValue("texSize", sourceTextureSize, sizeof(float)*2);
	//_sh_Bloom_Horizontal->set
	D3DXHANDLE technique = _sh_Bloom_Horizontal->GetTechniqueByName("technique0");
	_sh_Bloom_Horizontal->SetTechnique(technique);
	r->dev->SetVertexDeclaration(r->VD_POS_TX1());
	r->dev->SetStreamSource(0, _vb_quad, 0, sizeof(TexturedVertex));
	r->dev->SetIndices(_ib_quad);
	r->dev->SetTexture(0, _rt_GBuffer[0]);
	r->dev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	r->dev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	_sh_Bloom_Horizontal->Begin(0, 0);
	_sh_Bloom_Horizontal->BeginPass(0);
	r->dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);
	_sh_Bloom_Horizontal->EndPass();
	_sh_Bloom_Horizontal->End();

	r->dev->SetRenderTarget(0, _sf_BloomBlurChain[1]);
	r->dev->Clear(0, 0, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0);
	r->dev->SetTexture(0, _rt_BloomBlurChain[0]);
	
	technique = _sh_Bloom_Vertical->GetTechniqueByName("technique0");
	_sh_Bloom_Vertical->SetTechnique(technique);
	_sh_Bloom_Vertical->Begin(0, 0);
	_sh_Bloom_Vertical->BeginPass(0);
	r->dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);
	_sh_Bloom_Vertical->EndPass();
	_sh_Bloom_Vertical->End();
}


void SDeferredRenderer::_render_SSAO(void)
{
	
	SRender* r = SRender::getSingleton();

	int width = r->iWidth;
	int height = r->iHeight;
	D3DXMATRIX view, proj;
	D3DXMatrixLookAtLH(&view, &r->camera->getPositionX(), &r->camera->getLookAtX(), &r->camera->getUpVectorX());
	D3DXMatrixPerspectiveFovLH(&proj, r->camera->fov, (float)width/(float)height, 0.1f, 120.0f);
	D3DXMATRIX invView, inverseViewProjection, inverseProjectionMatrix;
	D3DXMatrixInverse(&invView, NULL, &view);
	D3DXMatrixInverse(&inverseViewProjection, NULL, &(view*proj));
	D3DXMatrixInverse(&inverseProjectionMatrix, NULL, &proj);

	D3DXMatrixInverse(&invView, NULL, &view);
	//_sh_Bloom_Horizontal->set
	D3DXHANDLE technique = _sh_SSAO->GetTechniqueByName("technique0");
	_sh_SSAO->SetTechnique(technique);
	r->dev->SetVertexDeclaration(r->VD_POS_TX1());
	r->dev->SetStreamSource(0, _vb_quad, 0, sizeof(TexturedVertex));
	r->dev->SetIndices(_ib_quad);
	r->dev->SetSamplerState(2, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	r->dev->SetSamplerState(2, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	_sh_SSAO->SetTexture("tex0", _rt_GBuffer[1]);
	_sh_SSAO->SetTexture("tex1", _rt_GBuffer[2]);
	_sh_SSAO->SetTexture("tex2", _SSAO_noise);
	_sh_SSAO->SetMatrix("inverseView", &invView);
	//_sh_SSAO->SetMatrix("inverseViewProjection", &inverseViewProjection);
	//_sh_SSAO->SetMatrix("inverseProjection", &inverseProjectionMatrix);
	//_sh_SSAO->SetMatrix("viewProjectionMatrix", &(view*proj));
	//_sh_SSAO->SetMatrix("projectionMatrix", &proj);
	_sh_SSAO->SetFloat("radius", 0.02f);
	D3DXVECTOR4 kernelVecs[64];
	for (int i = 0; i < 64; i++)
	{
		kernelVecs[i].x = ssaoKernel[i].x;
		kernelVecs[i].y = ssaoKernel[i].y;
		kernelVecs[i].z = ssaoKernel[i].z;
		kernelVecs[i].w = 1.0f;
	}
	_sh_SSAO->SetVectorArray("kernelOffsets", kernelVecs, 64);
	_sh_SSAO->SetVector("viewRay", &D3DXVECTOR4(r->camera->dx, r->camera->dy, r->camera->dz, 1.0f));
	_sh_SSAO->Begin(0, 0);
	_sh_SSAO->BeginPass(0);
	r->dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);
	_sh_SSAO->EndPass();
	_sh_SSAO->End();
	
	float ssaoTextureSize[2];
	ssaoTextureSize[0] = width / 2;
	ssaoTextureSize[1] = height / 2;

	r->dev->SetRenderTarget(0, _sf_SSAOBlur);
	r->dev->Clear(0, 0, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0);
	_sh_SSAOblur->SetTechnique(_sh_SSAOblur->GetTechniqueByName("technique0"));
	_sh_SSAOblur->SetValue("inputTextureSize", ssaoTextureSize, sizeof(float)*2);
	
	r->dev->SetTexture(0, _rt_SSAO);
	_sh_SSAOblur->Begin(0, 0);
	_sh_SSAOblur->BeginPass(0);
	r->dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);
	_sh_SSAOblur->EndPass();
	_sh_SSAOblur->End();

}

void SDeferredRenderer::_render_MotionBlur(void)
{
	SRender* r = SRender::getSingleton();
	//set source texture size for both shaders
	float sourceTextureSize[2];
	sourceTextureSize[0] = (float)r->iWidth;
	sourceTextureSize[1] = (float)r->iHeight;

	_sh_MotionBlur->SetValue("textureSize", sourceTextureSize, sizeof(float)*2);
	_sh_MotionBlur->SetFloat("velocityScale", 0.3f);

	//_sh_Bloom_Horizontal->set
	D3DXHANDLE technique = _sh_MotionBlur->GetTechniqueByName("technique0");
	_sh_MotionBlur->SetTechnique(technique);
	r->dev->SetVertexDeclaration(r->VD_POS_TX1());
	r->dev->SetStreamSource(0, _vb_quad, 0, sizeof(TexturedVertex));
	r->dev->SetIndices(_ib_quad);
	r->dev->SetTexture(0, _rt_GBuffer[3]);
	r->dev->SetTexture(1, _rt_Final);
	r->dev->SetTexture(2, _rt_GBuffer[2]);
	_sh_MotionBlur->Begin(0, 0);
	_sh_MotionBlur->BeginPass(0);
	r->dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);
	_sh_MotionBlur->EndPass();
	_sh_MotionBlur->End();

	
}



void SDeferredRenderer::_render_Final(void)
{
	SRender* r = SRender::getSingleton();
	int width = r->iWidth;
	int height = r->iHeight;
	r->dev->Clear(0, 0, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0,0,0,0), 1.0f, 0);
	r->dev->SetRenderState(D3DRS_ZENABLE, FALSE);
	r->dev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	r->dev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	D3DXHANDLE technique = _sh_Compose->GetTechniqueByName("technique0");
	_sh_Compose->SetTechnique(technique);
	float GBufferTextureSize[2];

	GBufferTextureSize[0] = (float)width;
	GBufferTextureSize[1] = (float)height;

	
	_sh_Compose->SetValue("GBufferTextureSize", GBufferTextureSize, sizeof(float)*2);
	_sh_Compose->SetValue("ambientLight", r->ambientColor, sizeof(float)*3);
	_sh_Compose->SetValue("fogColor", r->fogColor, sizeof(float)*3);
	_sh_Compose->SetFloat("fogStart", r->fFogStart);
	_sh_Compose->SetFloat("fogEnd", r->fFogEnd);
	r->dev->SetTexture(0, _rt_GBuffer[0]);
	r->dev->SetTexture(1, _rt_LightBuffer);
	r->dev->SetTexture(2, _rt_BloomBlurChain[1]);
	r->dev->SetTexture(3, _rt_GBuffer[2]);
	r->dev->SetTexture(4, _rt_SSAOBlur);
	r->dev->SetSamplerState(4, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	r->dev->SetSamplerState(4, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	r->dev->SetSamplerState(4, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	r->dev->SetVertexDeclaration(r->VD_POS_TX1());
	r->dev->SetStreamSource(0, _vb_quad, 0, sizeof(TexturedVertex));
	r->dev->SetIndices(_ib_quad);
	_sh_Compose->Begin(0, 0);
	_sh_Compose->BeginPass(0);
	r->dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);
	_sh_Compose->EndPass();
	_sh_Compose->End();
}

void SDeferredRenderer::_render_downscaleScene(void)
{
	SRender* r = SRender::getSingleton();
	//set source texture size for both shaders
	float sourceTextureSize[2];
	sourceTextureSize[0] = (float)r->iWidth;
	sourceTextureSize[1] = (float)r->iHeight;

	_sh_downscale16->SetValue("textureSize", sourceTextureSize, sizeof(float)*2);
	r->dev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	r->dev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	r->dev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	//_sh_Bloom_Horizontal->set
	D3DXHANDLE technique = _sh_downscale16->GetTechniqueByName("technique0");
	_sh_downscale16->SetTechnique(technique);
	r->dev->SetVertexDeclaration(r->VD_POS_TX1());
	r->dev->SetStreamSource(0, _vb_quad, 0, sizeof(TexturedVertex));
	r->dev->SetIndices(_ib_quad);
	r->dev->SetTexture(0, _rt_Final);
	_sh_downscale16->Begin(0, 0);
	_sh_downscale16->BeginPass(0);
	r->dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);
	_sh_downscale16->EndPass();
	_sh_downscale16->End();
}

void SDeferredRenderer::_render_blurDownscaledScene(void)
{
	SRender* r = SRender::getSingleton();
	//set source texture size for both shaders
	float sourceTextureSize[2], sourceTextureSize_4[2];
	sourceTextureSize[0] = (float)r->iWidth;
	sourceTextureSize[1] = (float)r->iHeight;
	sourceTextureSize_4[0] = (float)r->iWidth * 0.5f;  
	sourceTextureSize_4[1] = (float)r->iHeight * 0.5f;
	_sh_Bloom_Horizontal->SetValue("texSize", sourceTextureSize_4, sizeof(float)*2);
	_sh_Bloom_Vertical->SetValue("texSize", sourceTextureSize_4, sizeof(float)*2);

	//Set bilinear filtering for cleaner samples
	r->dev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	r->dev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	r->dev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	//_sh_Bloom_Horizontal->set
	D3DXHANDLE technique = _sh_Bloom_Horizontal->GetTechniqueByName("technique0");
	_sh_Bloom_Horizontal->SetTechnique(technique);
	r->dev->SetVertexDeclaration(r->VD_POS_TX1());
	r->dev->SetStreamSource(0, _vb_quad, 0, sizeof(TexturedVertex));
	r->dev->SetIndices(_ib_quad);
	r->dev->SetTexture(0, _rt_downscaledScene );
	_sh_Bloom_Horizontal->Begin(0, 0);
	_sh_Bloom_Horizontal->BeginPass(0);
	r->dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);
	_sh_Bloom_Horizontal->EndPass();
	_sh_Bloom_Horizontal->End();

	r->dev->SetRenderTarget(0, _sf_downscaledSceneBlurredHV );
	r->dev->Clear(0, 0, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0);
	r->dev->SetTexture(0, _rt_downscaledSceneBlurredH);
	technique = _sh_Bloom_Vertical->GetTechniqueByName("technique0");
	_sh_Bloom_Vertical->SetTechnique(technique);
	_sh_Bloom_Vertical->Begin(0, 0);
	_sh_Bloom_Vertical->BeginPass(0);
	r->dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);
	_sh_Bloom_Vertical->EndPass();
	_sh_Bloom_Vertical->End();
}

void SDeferredRenderer::_render_combineFinalDOF(void)
{
	SRender* r = SRender::getSingleton();
	//set source texture size for both shaders
	float sourceTextureSize[2], sourceTextureSize_4[2];
	sourceTextureSize[0] = (float)r->iWidth;
	sourceTextureSize[1] = (float)r->iHeight;
	sourceTextureSize_4[0] = (float)r->iWidth * 0.5f *0.5f;
	sourceTextureSize_4[1] = (float)r->iHeight * 0.5f *0.5f;
	
	_sh_combineFinalDOF->SetFloat("focalWidth", r->focalWidth);
	_sh_combineFinalDOF->SetFloat("focalDistance", r->focalDistance);
	//Set bilinear filtering for cleaner samples
	r->dev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	r->dev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	r->dev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	r->dev->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	r->dev->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	r->dev->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	r->dev->SetSamplerState(2, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	r->dev->SetSamplerState(2, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	r->dev->SetSamplerState(2, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
	//_sh_Bloom_Horizontal->set
	D3DXHANDLE technique = _sh_Bloom_Horizontal->GetTechniqueByName("technique0");
	_sh_combineFinalDOF->SetTechnique(technique);
	r->dev->SetVertexDeclaration(r->VD_POS_TX1());
	r->dev->SetStreamSource(0, _vb_quad, 0, sizeof(TexturedVertex));
	r->dev->SetIndices(_ib_quad);
	r->dev->SetTexture(0, _rt_Final );
	r->dev->SetTexture(1, _rt_downscaledSceneBlurredHV);
	r->dev->SetTexture(2, _rt_GBuffer[2]);
	_sh_combineFinalDOF->Begin(0, 0);
	_sh_combineFinalDOF->BeginPass(0);
	r->dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);
	_sh_combineFinalDOF->EndPass();
	_sh_combineFinalDOF->End(); 
}
void SDeferredRenderer::render(void)
{
	
	SRender* r = SRender::getSingleton();
	r->dev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	
	//Get a pointer to the original backbuffer
	//then set render targets for gbuffer then render
	r->dev->GetRenderTarget(0, &_sf_backbuffer);
	r->dev->SetRenderTarget(0, _sf_GBuffer[0]);
	r->dev->SetRenderTarget(1, _sf_GBuffer[1]);
	r->dev->SetRenderTarget(2, _sf_GBuffer[2]);
	r->dev->SetRenderTarget(3, _sf_GBuffer[3]);
	_render_ClearGBuffer();
	_render_GBuffer();

	//Shadow maps pass
	r->dev->SetRenderTarget(1, 0);
	r->dev->SetRenderTarget(2, 0);
	r->dev->SetRenderTarget(3, 0);
	_render_ShadowMaps();
	//Now for the complex lighting pass
	r->dev->SetRenderTarget(0, _sf_LightBuffer);
	r->dev->SetRenderTarget(1, 0);
	r->dev->SetRenderTarget(2, 0);
	r->dev->SetRenderTarget(3, 0);
	r->dev->Clear(0,0,D3DCLEAR_TARGET, 0x00000000, 1.0f, 0);
	_render_LightBuffer();

	//Now render glow particles to albedo (changes colour + glow is stored inside w component)
	r->dev->SetRenderTarget(0, _sf_GBuffer[0]);
	r->particleSystemObject->renderGlowParticles();


	//Now extract and blur luminance for bloom buffer
	r->dev->SetRenderTarget(0, _sf_BloomBlurChain[0]);
	r->dev->SetRenderTarget(1, 0);
	r->dev->SetRenderTarget(2, 0);
	r->dev->Clear(0,0,D3DCLEAR_TARGET, 0x00000000, 1.0f, 0);
	_render_BloomBuffer();

	//Now perform SSAO calculations based on normals and depth
	r->dev->SetRenderTarget(0, _sf_SSAO);
	r->dev->Clear(0, 0, D3DCLEAR_TARGET, 0xffffffff, 1.0f, 0);
	if (GlobalConfiguration::getSingleton()->bSSAO()) _render_SSAO();


	//Now combine gbuffer albedo with lighting pass and bloom buffer
	//r->dev->SetRenderTarget(0, _sf_Final);
	r->dev->SetRenderTarget(0, _sf_Final);
	
	_render_Final();
	
	//now create downscaled scene
	r->dev->SetRenderTarget(0, _sf_downscaledScene);
	r->dev->Clear(0, 0, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0);
	//_render_downscaleScene();

	//now blur downscaled scene
	r->dev->SetRenderTarget(0, _sf_downscaledSceneBlurredH);
	r->dev->Clear(0, 0, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0);
	//_render_blurDownscaledScene();

	if (S_INPUT::keyPressed(DIK_L))
	{
		//D3DXSaveTextureToFile(L"bloomBuffer.jpg", D3DXIFF_JPG, _rt_BloomBlurChain[0], 0);
	}

	if (S_INPUT::keyPressed(DIK_UP)) r->focalDistance += 1.0f;
	if (S_INPUT::keyPressed(DIK_DOWN)) r->focalDistance -= 1.0f;
	if (S_INPUT::keyPressed(DIK_LEFT)) r->focalWidth -= 1.0f;
	if (S_INPUT::keyPressed(DIK_RIGHT)) r->focalWidth += 1.0f;
	//r->dev->SetRenderTarget(0, _sf_GBuffer[0]);
	//r->dev->Clear(0, 0, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0);
	//_render_combineFinalDOF();

	////Now set backbuffer back to normal and deferred pass is finished :)
	r->dev->SetRenderTarget(0, _sf_backbuffer);
	r->dev->Clear(0,0, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0);
	_render_MotionBlur();

	
	
	
}


void SDeferredRenderer::_updateLightEffects(void)
{
	LightEffect** lights = SRender::getSingleton()->lightEffects;

	for (int i = 0; i < SRENDER_OBJECTCONTAINERSIZE; i++)
	{
		
		LightEffect* l = lights[i];
		if (l == 0) continue;
		l->dx += l->ax;
		l->dy += l->ay;
		l->dz += l->az;

		l->x += l->dx;
		l->y += l->dy;
		l->z += l->dz;

		

		l->color[0] += l->dColor[0];
		l->color[1] += l->dColor[1];
		l->color[2] += l->dColor[2];
		l->lifetime -= 0.01f;
		if (l->lifetime <= 0.0f)
		{

			//delete doesn't matter which pointer its called on, BUT we can't set
			//l to 0 because its just the local pointer. THe original has to be set
			//to 0. Important

			delete lights[i];
			lights[i] = 0;
		}
	}
}