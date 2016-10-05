#include "SRender.h"

#include <sstream>

//used for finding which texture comes from where
//in the texture atlas.
static float getUStart(int type);
static float getVStart(int type);

SMutableMesh::SMutableMesh(void) 
{
	atlas = 0;
	vertexBuffer = 0;
	indexBuffer = 0;
	
	_deferredObject = true;
}

SMutableMesh::~SMutableMesh(void)
{
}


#include "SLOG.h"
void SMutableMesh::render(D3DXMATRIX* ref, float x, float y, float z,
					 float rx, float ry, float rz,
					 float sx, float sy, float sz,
					 float cx, float cy, float cz,
					 LPDIRECT3DDEVICE9 dev)
{
	
	//Render States
	dev->SetRenderState(D3DRS_FOGENABLE, TRUE);
	dev->SetRenderState(D3DRS_LIGHTING, TRUE);
	D3DXMATRIX translation, scaling, rotationX, rotationY, rotationZ;
	D3DXMatrixTranslation(&translation, x, y, z);
	D3DXMatrixScaling(&scaling, sx, sy, sz);
	D3DXMatrixRotationX(&rotationX, rx);
	D3DXMatrixRotationY(&rotationX, ry);
	D3DXMatrixRotationZ(&rotationX, rz);
	
	dev->SetStreamSource(0, vertexBuffer, 0, sizeof(DetailedTexturedVertex));
	dev->SetIndices(indexBuffer);
	dev->SetFVF(SRENDER_POSNORTX1);
	//dev->SetTransform(D3DTS_WORLD, &(scaling*rotationX*rotationY*rotationZ*translation));
	dev->SetTransform(D3DTS_WORLD, &(scaling*translation));
	dev->SetTexture(0, atlas);
	dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, faceCount*4, 0, faceCount*2);
}

int SMutableMesh::createAtlasFromPath(const wchar_t* path)
{
	
	if (atlas != NULL) atlas->Release();
	D3DXCreateTexture(dev, 1024, 1024, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &atlas);
	D3DXCreateTexture(dev, 1024, 1024, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &atlasN);
	D3DXCreateTexture(dev, 1024, 1024, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &atlasS);
	LPDIRECT3DSURFACE9 surface;
	LPDIRECT3DSURFACE9 surfaceN;
	LPDIRECT3DSURFACE9 surfaceS;
	LPDIRECT3DSURFACE9 origSurface;
	atlas->GetSurfaceLevel(0, &surface);
	atlasN->GetSurfaceLevel(0, &surfaceN);
	atlasS->GetSurfaceLevel(0, &surfaceS);
	LPDIRECT3DTEXTURE9 importTexGroup[64];
	LPDIRECT3DTEXTURE9 importTexGroupN[64];
	LPDIRECT3DTEXTURE9 importTexGroupS[64];
	for (int x = 0; x < 64; x++){

		importTexGroup[x] = NULL;
		importTexGroupN[x] = NULL;
		importTexGroupS[x] = NULL;
	}
	std::wstringstream pathstream;
	std::wstringstream pathstreamN;
	std::wstringstream pathstreamS;
	pathstream.str(L"");
	pathstreamN.str(L"");
	pathstreamS.str(L"");
	int successfulImports = 0;
	for (int i = 0; i < 64; i++)
	{
		pathstream.str(L"");
		pathstreamN.str(L"");
		pathstreamS.str(L"");

		pathstream<<path<<L"/block"<<i<<L".png";
		pathstreamN<<path<<L"/block"<<i<<L"n.png";
		pathstreamS<<path<<L"/block"<<i<<L"s.png";
		D3DXCreateTextureFromFileEx(dev, pathstream.str().c_str(), 128, 128, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0xff000000, 0, 0, &importTexGroup[i]);
		D3DXCreateTextureFromFileEx(dev, pathstreamN.str().c_str(), 128, 128, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0xff000000, 0, 0, &importTexGroupN[i]);
		D3DXCreateTextureFromFileEx(dev, pathstreamS.str().c_str(), 128, 128, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0xff000000, 0, 0, &importTexGroupS[i]);
		if (importTexGroup[i] != NULL)
		{
			successfulImports++;
		}
	}
	
	LPD3DXSPRITE sprite;
	D3DXCreateSprite(dev, &sprite);
	dev->GetRenderTarget(0, &origSurface);

	/* render to color map atlas */
	dev->SetRenderTarget(0, surface);
	dev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
	dev->BeginScene();
	sprite->Begin(0);
	int x, y;
	x = 0.0f; y = 0.0f;
	for (int i = 0; i < 64; i++)
	{
		sprite->Draw(importTexGroup[i], NULL, NULL, &D3DXVECTOR3(x*128.0f, y*128.0f, 0.0f), 0xffffffff);
		x++;
		if (x>7)
		{
			x = 0;
			y++;
		}

	}
	sprite->End();
	dev->EndScene();

	/* render to normal map atlas */

	dev->SetRenderTarget(0, surfaceN);
	dev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(127, 127, 235), 1.0f, 0);
	dev->BeginScene();
	sprite->Begin(0);
	x = 0.0f; y = 0.0f;
	for (int i = 0; i < 64; i++)
	{
		sprite->Draw(importTexGroupN[i], NULL, NULL, &D3DXVECTOR3(x*128.0f, y*128.0f, 0.0f), 0xffffffff);
		x++;
		if (x>7)
		{
			x = 0;
			y++;
		}

	}
	sprite->End();
	dev->EndScene();
	
	/* render to specular map atlas */
	dev->SetRenderTarget(0, surfaceS);
	dev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
	dev->BeginScene();
	sprite->Begin(0);
	x = 0.0f; y = 0.0f;
	for (int i = 0; i < 64; i++)
	{
		sprite->Draw(importTexGroupS[i], NULL, NULL, &D3DXVECTOR3(x*128.0f, y*128.0f, 0.0f), 0xffffffff);
		x++;
		if (x>7)
		{
			x = 0;
			y++;
		}

	}
	sprite->End();
	dev->EndScene();
	dev->SetRenderTarget(0, origSurface);

	for (int i = 0; i < 64; i++)
	{
		if (importTexGroup[i] != NULL)
		{
			importTexGroup[i]->Release();
		}
		if (importTexGroupN[i] != NULL)
		{
			importTexGroupN[i]->Release();
		}
		if (importTexGroupS[i] != NULL)
		{
			importTexGroupS[i]->Release();
		}
	}
	
	D3DXSaveTextureToFile(L"normalsTestOutput.jpg", D3DXIFF_JPG, atlasN, 0);
	return successfulImports;
}

bool SMutableMesh::OLD_createAtlasFromPath(const wchar_t* path, int textureCount)
{
	//JUST SO IT COMPILES PROPERLY
	LPDIRECT3DDEVICE9 dev;
	//IS THIS FUNCTION IS TO BE USED, THIS IS WRONG

	D3DXCreateTexture(dev, 1024, 1024, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &atlas);
	LPDIRECT3DSURFACE9 surface;
	LPDIRECT3DSURFACE9 origSurface;
	atlas->GetSurfaceLevel(0, &surface);

	LPDIRECT3DTEXTURE9 importTexGroup[64];
	for (int x = 0; x < 64; x++){
		importTexGroup[x] = NULL;
	}
	std::wstringstream pathstream;
	pathstream.str(L"");
	int maxTexIndex = textureCount;
	for (int i = 0; i < 64; i++)
	{
		pathstream.str(L"");
		int pathMod = i;
		if (pathMod > maxTexIndex) pathMod = 0; 
		pathstream<<path<<L"/block"<<pathMod<<L".png";
		D3DXCreateTextureFromFileEx(dev, pathstream.str().c_str(), 128, 128, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0xff000000, 0, 0, &importTexGroup[i]);
	}
	
	LPD3DXSPRITE sprite;
	D3DXCreateSprite(dev, &sprite);
	dev->GetRenderTarget(0, &origSurface);
	dev->SetRenderTarget(0, surface);
	dev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
	dev->BeginScene();
	sprite->Begin(0);
	float x, y;
	x = 0.0f; y = 0.0f;
	for (int i = 0; i < 64; i++)
	{
		sprite->Draw(importTexGroup[i], NULL, NULL, &D3DXVECTOR3(x, y, 0.0f), 0xffffffff);
		x+=128.0f;
		if (x>1024.0f)
		{
			x = 0.0f;
			y += 128.0f;
		}

	}
	sprite->End();
	dev->EndScene();
	dev->SetRenderTarget(0, origSurface);
	
	return true;
}
#include "SLOG.h"
void SMutableMesh::updateGeometry(ps::VoxelData* voxelData)
{
	if (vertexBuffer == 0) dev->CreateVertexBuffer(sizeof(SRENDER_POSNORTX1)*64*64*64*24,
		0,
		0,
		D3DPOOL_DEFAULT,
		&vertexBuffer,
		NULL);
	if (indexBuffer == 0) dev->CreateIndexBuffer(sizeof(int)*64*64*64*36,
		0,
		D3DFMT_INDEX32,
		D3DPOOL_DEFAULT,
		&indexBuffer,
		NULL);

	faceCount = 0;

	int xDim = voxelData->x();
	int yDim = voxelData->y();
	int zDim = voxelData->z();
	for (int z = 0; z<zDim; z++){
		for (int y = 0; y<yDim; y++){
			for (int x = 0; x<xDim; x++){
				if (voxelData->data(x,y,z) != 0){
					//check for x-
					if (x==0) faceCount++;
					if (x>0){
						if (voxelData->data(x-1,y,z) == 0) faceCount++;
					}
					//check for x+
					if (x==(xDim-1)) faceCount++;
					if (x<(xDim-1)){
						if (voxelData->data(x+1,y,z) == 0) faceCount++;
					}
					//check for y-
					if (y==0) faceCount++;
					if (y>0){
						if (voxelData->data(x,y-1,z) == 0) faceCount++;
					}
					//check for y+
					if (y==(yDim-1)) faceCount++;
					if (y<(yDim-1)){
						if (voxelData->data(x,y+1,z) == 0) faceCount++;
					}
					//check for z-
					if (z==0) faceCount++;
					if (z>0){
						if (voxelData->data(x,y,z-1) == 0) faceCount++;
					}
					//check for z+
					if (z==(zDim-1)) faceCount++;
					if (z<(zDim-1)){
						if (voxelData->data(x,y,z+1) == 0) faceCount++;
					}
				}
			}
		}
	}
	//alloc 24 verts per cube
	DetailedTexturedVertex* verts;
	//alloc 36 shorts per cube
	int* indices;
	//index: v for vertex, i for index
	unsigned long cubeIndex_v = 0;
	unsigned long cubeIndex_i = 0;
	
	vertexBuffer->Lock(0, 0, (void**)&verts, 0);
	indexBuffer->Lock(0, 0, (void**)&indices, 0);
	for (int z = 0; z<zDim; z++){ 
		for (int y = 0; y<yDim; y++){ 
			for (int x = 0; x<xDim; x++){
				

				char s = voxelData->data(x,y,z);
				if (s > 0){
					
					float uStart = 0.0f;
					float vStart = 0.0f;
					uStart = getUStart(s);
					vStart = getVStart(s);
					//BUILD A CUBE

					//--VERTICES--and INDICES
					bool addVertex = false;
					//SIDE Z+
					if (z==(zDim-1)) addVertex = true;
					if (z<(zDim-1)){
						if (voxelData->data(x,y,z+1) == 0) addVertex = true;
					}
					if (addVertex){
						verts[cubeIndex_v].x = x+0.0f; verts[cubeIndex_v].y = y+0.0f; verts[cubeIndex_v].z = z+1.0f; verts[cubeIndex_v].u1 = uStart + 0.125f; verts[cubeIndex_v].v1 = vStart+0.125f; verts[cubeIndex_v].normal = D3DXVECTOR3(0.0f, 0.0f, 1.0f); verts[cubeIndex_v].tangent = D3DXVECTOR3(1.0f, 0.0f, 0.0f); verts[cubeIndex_v].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
						verts[cubeIndex_v+1].x = x+1.0f; verts[cubeIndex_v+1].y = y+0.0f; verts[cubeIndex_v+1].z = z+1.0f; verts[cubeIndex_v+1].u1 = uStart; verts[cubeIndex_v+1].v1 = vStart+0.125f; verts[cubeIndex_v+1].normal = D3DXVECTOR3(0.0f, 0.0f, 1.0f); verts[cubeIndex_v+1].tangent = D3DXVECTOR3(1.0f, 0.0f, 0.0f); verts[cubeIndex_v+1].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
						verts[cubeIndex_v+2].x = x+1.0f; verts[cubeIndex_v+2].y = y+1.0f; verts[cubeIndex_v+2].z = z+1.0f; verts[cubeIndex_v+2].u1 = uStart; verts[cubeIndex_v+2].v1 = vStart; verts[cubeIndex_v+2].normal = D3DXVECTOR3(0.0f, 0.0f, 1.0f); verts[cubeIndex_v+2].tangent = D3DXVECTOR3(1.0f, 0.0f, 0.0f); verts[cubeIndex_v+2].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
						verts[cubeIndex_v+3].x = x+0.0f; verts[cubeIndex_v+3].y = y+1.0f; verts[cubeIndex_v+3].z = z+1.0f; verts[cubeIndex_v+3].u1 = uStart + 0.125f; verts[cubeIndex_v+3].v1 = vStart; verts[cubeIndex_v+3].normal = D3DXVECTOR3(0.0f, 0.0f, 1.0f); verts[cubeIndex_v+3].tangent = D3DXVECTOR3(1.0f, 0.0f, 0.0f); verts[cubeIndex_v+3].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
						indices[cubeIndex_i] = cubeIndex_v; indices[cubeIndex_i+1] = cubeIndex_v+1; indices[cubeIndex_i+2] = cubeIndex_v+2;
						indices[cubeIndex_i+3] = cubeIndex_v; indices[cubeIndex_i+4] = cubeIndex_v+2; indices[cubeIndex_i+5] = cubeIndex_v+3;
						cubeIndex_v += 4;
						cubeIndex_i += 6;
						addVertex = false;
						
					}
					//Side Z-
					if (z==0) addVertex = true;
					if (z>0){
						if (voxelData->data(x,y,z-1) == 0) addVertex = true;
					}
					if (addVertex){
						verts[cubeIndex_v].x = x+0.0f; verts[cubeIndex_v].y = y+0.0f; verts[cubeIndex_v].z = z+0.0f; verts[cubeIndex_v].u1 = uStart; verts[cubeIndex_v].v1 = vStart+0.125f; verts[cubeIndex_v].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f); verts[cubeIndex_v].tangent = D3DXVECTOR3(-1.0f, 0.0f, 0.0f); verts[cubeIndex_v].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
						verts[cubeIndex_v+1].x = x+1.0f; verts[cubeIndex_v+1].y = y+0.0f; verts[cubeIndex_v+1].z = z+0.0f; verts[cubeIndex_v+1].u1 = uStart + 0.125f; verts[cubeIndex_v+1].v1 = vStart+0.125f; verts[cubeIndex_v+1].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f); verts[cubeIndex_v+1].tangent = D3DXVECTOR3(-1.0f, 0.0f, 0.0f); verts[cubeIndex_v+1].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
						verts[cubeIndex_v+2].x = x+1.0f; verts[cubeIndex_v+2].y = y+1.0f; verts[cubeIndex_v+2].z = z+0.0f; verts[cubeIndex_v+2].u1 = uStart + 0.125f; verts[cubeIndex_v+2].v1 = vStart; verts[cubeIndex_v+2].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f); verts[cubeIndex_v+2].tangent = D3DXVECTOR3(-1.0f, 0.0f, 0.0f); verts[cubeIndex_v+2].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
						verts[cubeIndex_v+3].x = x+0.0f; verts[cubeIndex_v+3].y = y+1.0f; verts[cubeIndex_v+3].z = z+0.0f; verts[cubeIndex_v+3].u1 = uStart; verts[cubeIndex_v+3].v1 = vStart; verts[cubeIndex_v+3].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f); verts[cubeIndex_v+3].tangent = D3DXVECTOR3(-1.0f, 0.0f, 0.0f); verts[cubeIndex_v+3].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
						indices[cubeIndex_i] = cubeIndex_v; indices[cubeIndex_i+1] = cubeIndex_v+2; indices[cubeIndex_i+2] = cubeIndex_v+1;
						indices[cubeIndex_i+3] = cubeIndex_v; indices[cubeIndex_i+4] = cubeIndex_v+3; indices[cubeIndex_i+5] = cubeIndex_v+2;
						cubeIndex_v += 4;
						cubeIndex_i += 6;
						addVertex = false;
						
					}
					//Side X+
					if (x==(xDim-1)) addVertex = true;
					if (x<(xDim-1)){
						if (voxelData->data(x+1,y,z) == 0) addVertex = true;
					}
					if (addVertex){
						verts[cubeIndex_v].x = x+1.0f; verts[cubeIndex_v].y = y+0.0f; verts[cubeIndex_v].z = z+0.0f; verts[cubeIndex_v].u1 = uStart; verts[cubeIndex_v].v1 = vStart+0.125f; verts[cubeIndex_v].normal = D3DXVECTOR3(1.0f, 0.0f, 0.0f); verts[cubeIndex_v].tangent = D3DXVECTOR3(0.0f, 0.0f, 1.0f); verts[cubeIndex_v].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
						verts[cubeIndex_v+1].x = x+1.0f; verts[cubeIndex_v+1].y = y+0.0f; verts[cubeIndex_v+1].z = z+1.0f; verts[cubeIndex_v+1].u1 = uStart + 0.125f; verts[cubeIndex_v+1].v1 = vStart+0.125f; verts[cubeIndex_v+1].normal = D3DXVECTOR3(1.0f, 0.0f, 0.0f); verts[cubeIndex_v+1].tangent = D3DXVECTOR3(0.0f, 0.0f, 1.0f); verts[cubeIndex_v+1].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
						verts[cubeIndex_v+2].x = x+1.0f; verts[cubeIndex_v+2].y = y+1.0f; verts[cubeIndex_v+2].z = z+1.0f; verts[cubeIndex_v+2].u1 = uStart + 0.125f; verts[cubeIndex_v+2].v1 = vStart; verts[cubeIndex_v+2].normal = D3DXVECTOR3(1.0f, 0.0f, 0.0f); verts[cubeIndex_v+2].tangent = D3DXVECTOR3(0.0f, 0.0f, 1.0f); verts[cubeIndex_v+2].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
						verts[cubeIndex_v+3].x = x+1.0f; verts[cubeIndex_v+3].y = y+1.0f; verts[cubeIndex_v+3].z = z+0.0f; verts[cubeIndex_v+3].u1 = uStart; verts[cubeIndex_v+3].v1 = vStart; verts[cubeIndex_v+3].normal = D3DXVECTOR3(1.0f, 0.0f, 0.0f); verts[cubeIndex_v+3].tangent = D3DXVECTOR3(0.0f, 0.0f, 1.0f); verts[cubeIndex_v+3].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
						indices[cubeIndex_i] = cubeIndex_v; indices[cubeIndex_i+1] = cubeIndex_v+2; indices[cubeIndex_i+2] = cubeIndex_v+1;
						indices[cubeIndex_i+3] = cubeIndex_v; indices[cubeIndex_i+4] = cubeIndex_v+3; indices[cubeIndex_i+5] = cubeIndex_v+2;
						cubeIndex_v += 4;
						cubeIndex_i += 6;
						addVertex = false;
						
					}
					//Side X-
					if (x==0) addVertex = true;
					if (x>0){
						if (voxelData->data(x-1,y,z) == 0) addVertex = true;
					}
					if (addVertex){
						verts[cubeIndex_v].x = x+0.0f; verts[cubeIndex_v].y = y+0.0f; verts[cubeIndex_v].z = z+0.0f; verts[cubeIndex_v].u1 = uStart + 0.125f; verts[cubeIndex_v].v1 = vStart+0.125f; verts[cubeIndex_v].normal = D3DXVECTOR3(-1.0f, 0.0f, 0.0f); verts[cubeIndex_v].tangent = D3DXVECTOR3(0.0f, 0.0f, -1.0f); verts[cubeIndex_v].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
						verts[cubeIndex_v+1].x = x+0.0f; verts[cubeIndex_v+1].y = y+0.0f; verts[cubeIndex_v+1].z = z+1.0f; verts[cubeIndex_v+1].u1 = uStart; verts[cubeIndex_v+1].v1 = vStart+0.125f; verts[cubeIndex_v+1].normal = D3DXVECTOR3(-1.0f, 0.0f, 0.0f); verts[cubeIndex_v+1].tangent = D3DXVECTOR3(0.0f, 0.0f, -1.0f); verts[cubeIndex_v+1].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
						verts[cubeIndex_v+2].x = x+0.0f; verts[cubeIndex_v+2].y = y+1.0f; verts[cubeIndex_v+2].z = z+1.0f; verts[cubeIndex_v+2].u1 = uStart; verts[cubeIndex_v+2].v1 = vStart; verts[cubeIndex_v+2].normal = D3DXVECTOR3(-1.0f, 0.0f, 0.0f); verts[cubeIndex_v+2].tangent = D3DXVECTOR3(0.0f, 0.0f, -1.0f); verts[cubeIndex_v+2].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
						verts[cubeIndex_v+3].x = x+0.0f; verts[cubeIndex_v+3].y = y+1.0f; verts[cubeIndex_v+3].z = z+0.0f; verts[cubeIndex_v+3].u1 = uStart + 0.125f; verts[cubeIndex_v+3].v1 = vStart; verts[cubeIndex_v+3].normal = D3DXVECTOR3(-1.0f, 0.0f, 0.0f); verts[cubeIndex_v+3].tangent = D3DXVECTOR3(0.0f, 0.0f, -1.0f); verts[cubeIndex_v+3].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
						indices[cubeIndex_i] = cubeIndex_v; indices[cubeIndex_i+1] = cubeIndex_v+1; indices[cubeIndex_i+2] = cubeIndex_v+2;
						indices[cubeIndex_i+3] = cubeIndex_v; indices[cubeIndex_i+4] = cubeIndex_v+2; indices[cubeIndex_i+5] = cubeIndex_v+3;
						cubeIndex_v += 4;
						cubeIndex_i += 6;
						addVertex = false;
						
					}
					//Side Y+
					if (y==(yDim-1)) addVertex = true;
					if (y<(yDim-1)){
						if (voxelData->data(x,y+1,z) == 0) addVertex = true;
					}
					if (addVertex){
						verts[cubeIndex_v].x = x+0.0f; verts[cubeIndex_v].y = y+1.0f; verts[cubeIndex_v].z = z+0.0f; verts[cubeIndex_v].u1 = uStart; verts[cubeIndex_v].v1 = vStart+0.125f; verts[cubeIndex_v].normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f); verts[cubeIndex_v].tangent = D3DXVECTOR3(1.0f, 0.0f, 0.0f); verts[cubeIndex_v].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
						verts[cubeIndex_v+1].x = x+1.0f; verts[cubeIndex_v+1].y = y+1.0f; verts[cubeIndex_v+1].z = z+0.0f; verts[cubeIndex_v+1].u1 = uStart + 0.125f; verts[cubeIndex_v+1].v1 = vStart+0.125f; verts[cubeIndex_v+1].normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f); verts[cubeIndex_v+1].tangent = D3DXVECTOR3(1.0f, 0.0f, 0.0f); verts[cubeIndex_v+1].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
						verts[cubeIndex_v+2].x = x+1.0f; verts[cubeIndex_v+2].y = y+1.0f; verts[cubeIndex_v+2].z = z+1.0f; verts[cubeIndex_v+2].u1 = uStart + 0.125f; verts[cubeIndex_v+2].v1 = vStart; verts[cubeIndex_v+2].normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f); verts[cubeIndex_v+2].tangent = D3DXVECTOR3(1.0f, 0.0f, 0.0f); verts[cubeIndex_v+2].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
						verts[cubeIndex_v+3].x = x+0.0f; verts[cubeIndex_v+3].y = y+1.0f; verts[cubeIndex_v+3].z = z+1.0f; verts[cubeIndex_v+3].u1 = uStart; verts[cubeIndex_v+3].v1 = vStart; verts[cubeIndex_v+3].normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f); verts[cubeIndex_v+3].tangent = D3DXVECTOR3(1.0f, 0.0f, 0.0f); verts[cubeIndex_v+3].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
						indices[cubeIndex_i] = cubeIndex_v; indices[cubeIndex_i+1] = cubeIndex_v+2; indices[cubeIndex_i+2] = cubeIndex_v+1;
						indices[cubeIndex_i+3] = cubeIndex_v; indices[cubeIndex_i+4] = cubeIndex_v+3; indices[cubeIndex_i+5] = cubeIndex_v+2;
						cubeIndex_v += 4;
						cubeIndex_i += 6;
						addVertex = false;
						
					}
					//Side Y-
					if (y==0) addVertex = true;
					if (y>0){
						if (voxelData->data(x,y-1,z) == 0) addVertex = true;
					}
					if (addVertex){
						verts[cubeIndex_v].x = x+0.0f; verts[cubeIndex_v].y = y+0.0f; verts[cubeIndex_v].z = z+0.0f; verts[cubeIndex_v].u1 = uStart + 0.125f; verts[cubeIndex_v].v1 = vStart+0.125f; verts[cubeIndex_v].normal = D3DXVECTOR3(0.0f, -1.0f, 0.0f); verts[cubeIndex_v].tangent = D3DXVECTOR3(-1.0f, 0.0f, 0.0f); verts[cubeIndex_v].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
						verts[cubeIndex_v+1].x = x+1.0f; verts[cubeIndex_v+1].y = y+0.0f; verts[cubeIndex_v+1].z = z+0.0f; verts[cubeIndex_v+1].u1 = uStart; verts[cubeIndex_v+1].v1 = vStart+0.125f; verts[cubeIndex_v+1].normal = D3DXVECTOR3(0.0f, -1.0f, 0.0f); verts[cubeIndex_v+1].tangent = D3DXVECTOR3(-1.0f, 0.0f, 0.0f); verts[cubeIndex_v+1].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
						verts[cubeIndex_v+2].x = x+1.0f; verts[cubeIndex_v+2].y = y+0.0f; verts[cubeIndex_v+2].z = z+1.0f; verts[cubeIndex_v+2].u1 = uStart; verts[cubeIndex_v+2].v1 = vStart; verts[cubeIndex_v+2].normal = D3DXVECTOR3(0.0f, -1.0f, 0.0f); verts[cubeIndex_v+2].tangent = D3DXVECTOR3(-1.0f, 0.0f, 0.0f); verts[cubeIndex_v+2].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
						verts[cubeIndex_v+3].x = x+0.0f; verts[cubeIndex_v+3].y = y+0.0f; verts[cubeIndex_v+3].z = z+1.0f; verts[cubeIndex_v+3].u1 = uStart + 0.125f; verts[cubeIndex_v+3].v1 = vStart; verts[cubeIndex_v+3].normal = D3DXVECTOR3(0.0f, -1.0f, 0.0f); verts[cubeIndex_v+3].tangent = D3DXVECTOR3(-1.0f, 0.0f, 0.0f); verts[cubeIndex_v+3].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
						indices[cubeIndex_i] = cubeIndex_v; indices[cubeIndex_i+1] = cubeIndex_v+1; indices[cubeIndex_i+2] = cubeIndex_v+2;
						indices[cubeIndex_i+3] = cubeIndex_v; indices[cubeIndex_i+4] = cubeIndex_v+2; indices[cubeIndex_i+5] = cubeIndex_v+3;
						cubeIndex_v += 4;
						cubeIndex_i += 6;
						addVertex = false;
						
					}
				}
			}
		}
	}
	vertexBuffer->Unlock();
	indexBuffer->Unlock();
}


/* both these functions are probably the 2 stupidest functions
i've ever written in my entire programming "life". These could
probably be 1 function with like 3 lines of code, but i'm not gonna
bother with it now cause it works.
*/
float getUStart(int type){
	if (type == 1 || type == 9 || type == 17 || type == 25 || type == 33 || type == 41 || type == 49 || type == 57) return 0.0f;
	if (type == 2 || type == 10 || type == 18 || type == 26 || type == 34 || type == 42 || type == 50 || type == 58) return 0.125f; 
	if (type == 3 || type == 11 || type == 19 || type == 27 || type == 35 || type == 43 || type == 51 || type == 59) return 0.250f;
	if (type == 4 || type == 12 || type == 20 || type == 28 || type == 36 || type == 44 || type == 52 || type == 60) return 0.375f;
	if (type == 5 || type == 13 || type == 21 || type == 29 || type == 37 || type == 45 || type == 53 || type == 61) return 0.5f;
	if (type == 6 || type == 14 || type == 22 || type == 30 || type == 38 || type == 46 || type == 54 || type == 62) return 0.625f;
	if (type == 7 || type == 15 || type == 23 || type == 31 || type == 39 || type == 47 || type == 55 || type == 63) return 0.750f;
	if (type == 8 || type == 16 || type == 24 || type == 32 || type == 40 || type == 48 || type == 56 || type == 64) return 0.875f;
}
float getVStart(int type){
	if (type == 1 || type == 2 || type == 3 || type == 4 || type == 5 || type == 6 || type == 7 ||type == 8) return 0.0f;
	if (type == 9 || type == 10 || type == 11 || type == 12 || type == 13 || type == 14 || type == 15 ||type == 16) return 0.125f;
	if (type == 17 || type == 18 || type == 19 || type == 20 || type == 21 || type == 22 || type == 23 ||type == 24) return 0.250f;
	if (type == 25 || type == 26 || type == 27 || type == 28 || type == 29 || type == 30 || type == 31 ||type == 32) return 0.375f;
	if (type == 33 || type == 34 || type == 35 || type == 36 || type == 37 || type == 38 || type == 39 ||type == 40) return 0.5f;
	if (type == 41 || type == 42 || type == 43 || type == 44 || type == 45 || type == 46 || type == 47 ||type == 48) return 0.625f;
	if (type == 49 || type == 50 || type == 51 || type == 52 || type == 53 || type == 54 || type == 55 ||type == 56) return 0.750f;
	if (type == 57 || type == 58 || type == 59 || type == 60 || type == 61 || type == 62 || type == 63 ||type == 64) return 0.875f;
}

LPDIRECT3DTEXTURE9 SMutableMesh::x_GetAtlas(void)
{
	return atlas;
}
#include "SInput.h"
void SMutableMesh::renderDeferredO(D3DXMATRIX* ref, float x, float y, float z,
				float rx, float ry, float rz,
				float sx, float sy, float sz,
				float cx, float cy, float cz,
				LPD3DXEFFECT shader)
{

	SRender* r = SRender::getSingleton();
//Render States
	
	D3DXMATRIX translation, scaling, rotationX, rotationY, rotationZ;
	D3DXMatrixTranslation(&translation, x, y, z);
	D3DXMatrixScaling(&scaling, sx, sy, sz);
	D3DXMatrixRotationX(&rotationX, rx);
	D3DXMatrixRotationY(&rotationX, ry);
	D3DXMatrixRotationZ(&rotationX, rz);
	D3DXMATRIX world = scaling*translation;
	dev->SetStreamSource(0, vertexBuffer, 0, sizeof(TexturedDirectionalVertex));
	dev->SetIndices(indexBuffer);
	dev->SetFVF(SRENDER_POSNORDIFTX1);
	
	shader->SetMatrix("world", &world);
	dev->SetTexture(0, atlas);
	dev->SetTexture(1, atlasN);
	dev->SetTexture(2, atlasS);
	shader->Begin(0,0);
	shader->BeginPass(0);
	dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, faceCount*4, 0, faceCount*2);
	shader->EndPass();
	shader->End();
}

void SMutableMesh::renderDeferred(D3DXMATRIX* world, D3DXMATRIX* oldWorld, LPD3DXEFFECT shader)
{
	SRender* r = SRender::getSingleton();
	
	//setup matrices that need setting up
	D3DXMATRIX worldView;
	worldView = (*world) * r->matView;
	D3DXMATRIX worldViewInv;
	D3DXMatrixInverse(&worldViewInv, NULL, &worldView);
	static D3DXMATRIX worldViewIT;
	D3DXMatrixTranspose(&worldViewIT, &worldViewInv);

	D3DXMATRIX oldWorldViewProjection = (*oldWorld) * r->oldMatView * r->oldMatProj;

	shader->SetMatrix("world", world);
	shader->SetMatrix("view", &r->matView);
	shader->SetMatrix("projection", &r->matProj);
	shader->SetMatrix("worldViewIT", &worldViewIT);
	shader->SetMatrix("oldWorldViewProjection", &oldWorldViewProjection);

	r->dev->SetStreamSource(0, vertexBuffer, 0, sizeof(DetailedTexturedVertex));
	r->dev->SetIndices(indexBuffer);
	r->dev->SetVertexDeclaration(r->VD_POS_NOR_TX1_TAN_BIN());
	dev->SetTexture(0, atlas);
	dev->SetTexture(1, atlasN);
	dev->SetTexture(2, atlasS);
	shader->Begin(0,0);
	shader->BeginPass(0);
	dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, faceCount*4, 0, faceCount*2);
	shader->EndPass();
	shader->End();
}

void SMutableMesh::renderDeferredDepth(D3DXMATRIX* world, LPD3DXEFFECT shader)
{
	SRender* r = SRender::getSingleton(); 
	shader->SetMatrix("world", world);
	
	r->dev->SetVertexDeclaration(r->VD_POS_NOR_TX1_TAN_BIN());
	r->dev->SetStreamSource(0, vertexBuffer, 0, sizeof(DetailedTexturedVertex));
	r->dev->SetIndices(indexBuffer);
	
	shader->Begin(0,0);
	shader->BeginPass(0);
	dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, faceCount*4, 0, faceCount*2);
	shader->EndPass();
	shader->End();
}