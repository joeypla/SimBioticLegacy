#include "SParticleSystem.h"

#include "SRender.h"
#include "SGUI.h"
#include "SLOG.h"

D3DMATERIAL9 defaultMaterial_test2;
SParticleSystem::SParticleSystem(LPDIRECT3DDEVICE9 dev)
{
	_dev = dev;

	ZeroMemory(&defaultMaterial_test2, sizeof(D3DMATERIAL9));
	defaultMaterial_test2.Ambient.a = 1.0f;
	defaultMaterial_test2.Ambient.r = 1.0f;
	defaultMaterial_test2.Ambient.g = 1.0f;
	defaultMaterial_test2.Ambient.b = 1.0f;
	defaultMaterial_test2.Diffuse.a = 1.0f;
	defaultMaterial_test2.Diffuse.r = 1.0f;
	defaultMaterial_test2.Diffuse.g = 1.0f;
	defaultMaterial_test2.Diffuse.b = 1.0f;
	//setup so that every particle can be used
	
	ZeroMemory(particles, sizeof(SParticle) * SYMBIOTE_MAX_PARTICLES);
	ZeroMemory(glowParticles, sizeof(SParticle) * SYMBIOTE_MAX_PARTICLES);

	//load in all particle textures
	basicParticleTexture = 0;
	D3DXCreateTextureFromFile(S_GUI::getDevice(), L"../Resources/Textures/Particles/smokeparticle.png", &basicParticleTexture);

	basicGlowParticleTexture = 0;
	D3DXCreateTextureFromFile(S_GUI::getDevice(), L"../Resources/Textures/Particles/basicGlowParticle.png", &basicGlowParticleTexture);
	/*particleTextures = new LPDIRECT3DTEXTURE9[5];
	D3DXCreateTextureFromFile(S_GUI::getDevice(), L"../Resources/Textures/binary0.png", &particleTextures[0]);
	D3DXCreateTextureFromFile(S_GUI::getDevice(), L"../Resources/Textures/binary1.png", &particleTextures[1]);
	D3DXCreateTextureFromFile(S_GUI::getDevice(), L"../Resources/Textures/smoke_particle.png", &particleTextures[2]);
	D3DXCreateTextureFromFile(S_GUI::getDevice(), L"../Resources/Textures/Blocks/block0.png", &particleTextures[3]);
	D3DXCreateTextureFromFile(S_GUI::getDevice(), L"../Resources/Textures/Blocks/block0.png", &particleTextures[4]);*/
	//Fill vb and ib
	ParticleVertex* pv = new ParticleVertex[4];
	S_GUI::getDevice()->CreateVertexBuffer(sizeof(ParticleVertex)*4, 0, SRENDER_POSDIFTX1, D3DPOOL_MANAGED, &quadVertexBuffer, 0);
	S_GUI::getDevice()->CreateIndexBuffer(sizeof(short)*6, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &quadIndexBuffer, 0);
	quadVertexBuffer->Lock(0, 0, (void**)&pv, 0);
	pv[0].x = -0.5f; pv[0].y = 0.5f; pv[0].z = 0.0f; pv[0].u1 = 1.0f; pv[0].v1 = 0.0f;
	pv[1].x = -0.5f; pv[1].y = -0.5f; pv[1].z = 0.0f; pv[1].u1 = 1.0f; pv[1].v1 = 1.0f;
	pv[2].x = 0.5f; pv[2].y = 0.5f; pv[2].z = 0.0f; pv[2].u1 = 0.0f; pv[2].v1 = 0.0f;
	pv[3].x = 0.5f; pv[3].y = -0.5f; pv[3].z = 0.0f; pv[3].u1 = 0.0f; pv[3].v1 = 1.0f;
	pv[0].color = 0x2fffffff;
	pv[1].color = 0x2fffffff;
	pv[2].color = 0x2fffffff;
	pv[3].color = 0x2fffffff;

	quadVertexBuffer->Unlock();

	short* ind = new short[6];
	quadIndexBuffer->Lock(0, 0, (void**)&ind, 0);
	ind[0] = 0; ind[1] = 1; ind[2] = 2;
	ind[3] = 2; ind[4] = 1; ind[5] = 3;
	quadIndexBuffer->Unlock();


	LPDIRECT3DTEXTURE9 pTex = 0;
	D3DXCreateTextureFromFile(S_GUI::getDevice(), L"../Resources/Textures/Particles/smokeparticle.png", &pTex);
	particleTextures.push_back(pTex);




	{
		DWORD dwShaderFlags = D3DXFX_NOT_CLONEABLE | D3DXSHADER_NO_PRESHADER;
		LPD3DXBUFFER compilationErrors = 0;

		HRESULT hr = D3DXCreateEffectFromFile(SRender::getSingleton()->dev, L"../Resources/Shaders/glowparticle.fx", 0, 0, dwShaderFlags, 0, &glowParticleShader, &compilationErrors);
		if (FAILED(hr)){
			S_LOG::write("glowparticle.fx shader was not loaded correctly. ERROR: ");

			char* data = new char[compilationErrors->GetBufferSize()];
			memcpy( data, compilationErrors->GetBufferPointer(), compilationErrors->GetBufferSize());
			S_LOG::write(data);
			delete data;
		}
	}


	{
		DWORD dwShaderFlags = D3DXFX_NOT_CLONEABLE | D3DXSHADER_NO_PRESHADER;
		LPD3DXBUFFER compilationErrors = 0;

		HRESULT hr = D3DXCreateEffectFromFile(SRender::getSingleton()->dev, L"../Resources/Shaders/smokeparticle.fx", 0, 0, dwShaderFlags, 0, &smokeParticleShader, &compilationErrors);
		if (FAILED(hr)){
			S_LOG::write("smokeparticle.fx shader was not loaded correctly. ERROR: ");

			char* data = new char[compilationErrors->GetBufferSize()];
			memcpy( data, compilationErrors->GetBufferPointer(), compilationErrors->GetBufferSize());
			S_LOG::write(data);
			delete data;
		}
	}
}

void SParticleSystem::emitParticle(SParticle* data){
	//find particle to take place
	bool searching = true;
	int count = 0;
	while (searching){
		if (!particles[count].active){
			searching = false;
			particles[count].life = data->life;
			particles[count].dLife = data->dLife;
			particles[count].scale = data->scale;
			particles[count].dScale = data->dScale;
			particles[count].acceleration = data->acceleration;
			particles[count].velocity = data->velocity;
			particles[count].position = data->position;
			particles[count].active = true;
			particles[count].texture = basicParticleTexture;//particleTextures[0];
			particles[count].color[0] = data->color[0];
			particles[count].color[1] = data->color[1];
			particles[count].color[2] = data->color[2];
			particles[count].color[3] = data->color[3];
			particles[count].dColor[0] = data->dColor[0];
			particles[count].dColor[1] = data->dColor[1];
			particles[count].dColor[2] = data->dColor[2];
			particles[count].dColor[3] = data->dColor[3];

		}
		count++;
		if (count > SYMBIOTE_MAX_PARTICLES - 1) break;
	}
}
void SParticleSystem::emitGlowParticle(SParticle* data){
	//find particle to take place
	bool searching = true;
	int count = 0;
	while (searching){
		if (!glowParticles[count].active){
			searching = false;
			glowParticles[count].life = data->life;
			glowParticles[count].dLife = data->dLife;
			glowParticles[count].scale = data->scale;
			glowParticles[count].dScale = data->dScale;
			glowParticles[count].acceleration = data->acceleration;
			glowParticles[count].velocity = data->velocity;
			glowParticles[count].position = data->position;
			glowParticles[count].active = true;
			glowParticles[count].texture = basicGlowParticleTexture;//particleTextures[0];
			glowParticles[count].color[0] = data->color[0];
			glowParticles[count].color[1] = data->color[1];
			glowParticles[count].color[2] = data->color[2];
			glowParticles[count].color[3] = data->color[3];
			glowParticles[count].dColor[0] = data->dColor[0];
			glowParticles[count].dColor[1] = data->dColor[1];
			glowParticles[count].dColor[2] = data->dColor[2];
			glowParticles[count].dColor[3] = data->dColor[3];

		}
		count++;
		if (count > SYMBIOTE_MAX_PARTICLES - 1) break;
	}
}

SParticle* SParticleSystem::reserveGlowParticle(void)
{
	//find particle to take place
	bool searching = true;
	int count = 0;
	while (searching){
		if (!glowParticles[count].active){
			glowParticles[count].active = true;
			glowParticles[count].dLife = 0.0f;
			return &glowParticles[count];
			

		}
		count++;
		if (count > SYMBIOTE_MAX_PARTICLES - 1) break;
	}

	return NULL;
}

void SParticleSystem::releaseGlowParticle(SParticle* particle)
{
	particle->active = false;
}
void SParticleSystem::updateGlowParticles(void){
	

	for (int x = 0; x < SYMBIOTE_MAX_PARTICLES; x++){
		if (glowParticles[x].active){
			glowParticles[x].life -= glowParticles[x].dLife;
			if (glowParticles[x].life <= 0.0f){
				glowParticles[x].active = false;
			}

			glowParticles[x].velocity += glowParticles[x].acceleration;
			glowParticles[x].position += glowParticles[x].velocity;
			particles[x].color[0] += particles[x].dColor[0];
			particles[x].color[1] += particles[x].dColor[1];
			particles[x].color[2] += particles[x].dColor[2];
			particles[x].color[3] += particles[x].dColor[3];
			glowParticles[x].scale += glowParticles[x].dScale;
		}
	}
}
void SParticleSystem::update(void){
	

	for (int x = 0; x < SYMBIOTE_MAX_PARTICLES; x++){
		if (particles[x].active){
			particles[x].life -= particles[x].dLife;
			if (particles[x].life <= 0.0f){
				particles[x].active = false;
			}

			particles[x].velocity += particles[x].acceleration;
			particles[x].position += particles[x].velocity;
			particles[x].color[0] += particles[x].dColor[0];
			particles[x].color[1] += particles[x].dColor[1];
			particles[x].color[2] += particles[x].dColor[2];
			particles[x].color[3] += particles[x].dColor[3];
			particles[x].scale += particles[x].dScale;
		}
	}
}


//rendering of particles is extremely unoptimized. Like seriously, its not even considered brute force. Its like
//slower than that. Its pretty bad lol.

void SParticleSystem::renderGlowParticles(void)
{
	updateGlowParticles();

	SRender* r = SRender::getSingleton();
	D3DXMATRIX view, proj;
	D3DXMatrixLookAtLH(&view, &D3DXVECTOR3(r->camera->x, r->camera->y, r->camera->z),
		&D3DXVECTOR3(r->camera->x + r->camera->dx, r->camera->y + r->camera->dy, r->camera->z + r->camera->dz),
		&D3DXVECTOR3(r->camera->ux, r->camera->uy, r->camera->uz));

	D3DXMatrixPerspectiveFovLH(&proj, r->getCurrentCamera()->fov, (float)r->iWidth / (float)r->iHeight, 0.1f, 120.0f);
	D3DXHANDLE technique = glowParticleShader->GetTechniqueByName("technique0");
	glowParticleShader->SetTechnique(technique);
	r->dev->SetVertexDeclaration(r->VD_POS_DIF_TX1());

	r->dev->SetStreamSource(0, quadVertexBuffer, 0, sizeof(ColoredTexturedVertex));
	r->dev->SetIndices(quadIndexBuffer);
	glowParticleShader->Begin(0, 0);
	glowParticleShader->BeginPass(0);

	glowParticleShader->SetMatrix("view", &view);
	glowParticleShader->SetMatrix("projection", &proj);
	
	
	//glowParticleShader
	for (int x = 0; x < SYMBIOTE_MAX_PARTICLES; x++){
		if (glowParticles[x].active){
			
			SCamera* cam = SRender::getSingleton()->getCurrentCamera();
			float difx = cam->x - glowParticles[x].position.x;
			float dify = cam->y - glowParticles[x].position.y;
			float difz = cam->z - glowParticles[x].position.z;

			float flatDist = sqrt(difx * difx + difz * difz);
			float totalDist = sqrt(flatDist * flatDist + dify * dify);




			D3DXMATRIX matRotationY;
			D3DXMatrixIdentity(&matRotationY);
			matRotationY._11 = matRotationY._33 = difz / flatDist;
			matRotationY._31 = difx / flatDist;
			matRotationY._13 = -matRotationY._31; 


			D3DXMATRIX matRotationX;
			D3DXMatrixIdentity(&matRotationX);
			matRotationX._22 = matRotationX._33 = flatDist / totalDist;
			matRotationX._32 = dify / totalDist;
			matRotationX._23 = -matRotationX._32; 

			D3DXMATRIX matTranslate;
			D3DXMatrixIdentity(&matTranslate);
			D3DXMatrixTranslation(&matTranslate, glowParticles[x].position.x, glowParticles[x].position.y, glowParticles[x].position.z);

			D3DXMATRIX matScale;
			D3DXMatrixScaling(&matScale, glowParticles[x].scale, glowParticles[x].scale, glowParticles[x].scale);
			
			glowParticleShader->SetMatrix("world", &(matScale * matRotationX * matRotationY * matTranslate));
			glowParticleShader->SetValue("color", glowParticles[x].color, sizeof(float)*4);
			_dev->SetTexture(0, basicGlowParticleTexture);
			glowParticleShader->CommitChanges();
			_dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

		}
	}
	glowParticleShader->EndPass();
	glowParticleShader->End();
}
void SParticleSystem::render(void){

	update();
	//_dev->SetVertexDeclaration(SRender::getSingleton()->VD_POS_DIF_TX1());
	SRender* r = SRender::getSingleton();
	D3DXMATRIX view, proj;
	D3DXMatrixLookAtLH(&view, &D3DXVECTOR3(r->camera->x, r->camera->y, r->camera->z),
		&D3DXVECTOR3(r->camera->x + r->camera->dx, r->camera->y + r->camera->dy, r->camera->z + r->camera->dz),
		&D3DXVECTOR3(r->camera->ux, r->camera->uy, r->camera->uz));

	D3DXMatrixPerspectiveFovLH(&proj, r->getCurrentCamera()->fov, (float)r->iWidth / (float)r->iHeight, 0.1f, 120.0f);
	D3DXHANDLE technique = smokeParticleShader->GetTechniqueByName("technique0");
	smokeParticleShader->SetTechnique(technique);
	r->dev->SetVertexDeclaration(r->VD_POS_DIF_TX1());

	r->dev->SetStreamSource(0, quadVertexBuffer, 0, sizeof(ColoredTexturedVertex));
	r->dev->SetIndices(quadIndexBuffer);
	smokeParticleShader->Begin(0, 0);
	smokeParticleShader->BeginPass(0);

	smokeParticleShader->SetMatrix("view", &view);
	smokeParticleShader->SetMatrix("projection", &proj);

	/*_dev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	_dev->SetRenderState(D3DRS_ZENABLE, TRUE);
	_dev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	DWORD oldSourceBlend;
	DWORD oldDestBlend;
	DWORD oldBlendOp;
	_dev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	_dev->SetRenderState(D3DRS_COLORVERTEX, TRUE);
	_dev->GetRenderState(D3DRS_SRCBLEND, &oldSourceBlend);
	_dev->GetRenderState(D3DRS_DESTBLEND, &oldDestBlend);
	_dev->GetRenderState(D3DRS_BLENDOP, &oldBlendOp);

	_dev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	_dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	_dev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	_dev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	_dev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	_dev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

	_dev->SetMaterial(&defaultMaterial_test2);
	_dev->SetRenderState(D3DRS_LIGHTING, FALSE);*/
	for (int x = 0; x < SYMBIOTE_MAX_PARTICLES; x++){
		if (particles[x].active){
			
			SCamera* cam = SRender::getSingleton()->getCurrentCamera();
			float difx = cam->x - particles[x].position.x;
			float dify = cam->y - particles[x].position.y;
			float difz = cam->z - particles[x].position.z;

			float flatDist = sqrt(difx * difx + difz * difz);
			float totalDist = sqrt(flatDist * flatDist + dify * dify);




			D3DXMATRIX matRotationY;
			D3DXMatrixIdentity(&matRotationY);
			matRotationY._11 = matRotationY._33 = difz / flatDist;
			matRotationY._31 = difx / flatDist;
			matRotationY._13 = -matRotationY._31; 


			D3DXMATRIX matRotationX;
			D3DXMatrixIdentity(&matRotationX);
			matRotationX._22 = matRotationX._33 = flatDist / totalDist;
			matRotationX._32 = dify / totalDist;
			matRotationX._23 = -matRotationX._32; 

			D3DXMATRIX matTranslate;
			D3DXMatrixIdentity(&matTranslate);
			D3DXMatrixTranslation(&matTranslate, particles[x].position.x, particles[x].position.y, particles[x].position.z);

			D3DXMATRIX matScale;
			D3DXMatrixScaling(&matScale, particles[x].scale, particles[x].scale, particles[x].scale);
			
			_dev->SetTransform(D3DTS_WORLD, &(matScale * matRotationX * matRotationY * matTranslate));
			smokeParticleShader->SetMatrix("world", &(matScale * matRotationX * matRotationY * matTranslate));
			smokeParticleShader->SetValue("color", particles[x].color, sizeof(float)*4);
			_dev->SetTexture(0, basicParticleTexture);
			smokeParticleShader->CommitChanges();
			_dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

		}
	}
	smokeParticleShader->EndPass();
	smokeParticleShader->End();
	//_dev->SetRenderState(D3DRS_SRCBLEND, oldSourceBlend);
	//_dev->SetRenderState(D3DRS_DESTBLEND, oldDestBlend);
	//_dev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	//_dev->SetRenderState(D3DRS_BLENDOP, oldBlendOp);
	
}