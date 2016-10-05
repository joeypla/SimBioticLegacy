#include "SRender.h"

#include "fbxsdk.h"
#pragma comment(lib, "libfbxsdk.lib")
SMesh::SMesh(void)
{
	mesh = 0;
	 vertexBuffer = 0;
	 indexBuffer = 0;
	 materialSet = 0;
	 
	 _deferredObject = true;

	 _staticMeshType = SRENDER_STATICMESHTYPE_NONE;
}

bool SMesh::getDeferred(void)
{
	return _deferredObject;
}
SMesh::~SMesh(void)
{
	if (vertexBuffer != 0)
	{
		vertexBuffer->Release();
		vertexBuffer = 0;
	}

	if (indexBuffer != 0)
	{
		indexBuffer->Release();
		indexBuffer = 0;
	}
}

void SMesh::setDevice(LPDIRECT3DDEVICE9 dev)
{
	this->dev = dev;
}

void SMesh::setMaterialSet(SMaterialSet* set)
{
	materialSet = set;
}

SMaterialSet* SMesh::getMaterialSet(void)
{
	return materialSet;
}
void SMesh::render(D3DXMATRIX* ref, float x, float y, float z,
				   float rx, float ry, float rz,
				   float sx, float sy, float sz,
				   float cx, float cy, float cz,
				   LPDIRECT3DDEVICE9 dev){

	dev->SetRenderState(D3DRS_FOGENABLE, TRUE);
	dev->SetRenderState(D3DRS_LIGHTING, FALSE);

	D3DXMATRIX translate, rotate, scale;

	D3DXMatrixTranslation(&translate, x, y, z);
	D3DXMATRIX world;

	D3DXMATRIX matRotX, matRotY, matRotZ;
	D3DXMatrixRotationX(&matRotX, rx);
	D3DXMatrixRotationY(&matRotY, ry);
	D3DXMatrixRotationZ(&matRotZ, rz);
	rotate = matRotX*matRotY*matRotZ;

	D3DXMatrixScaling(&scale, sx, sy, sz);
	D3DXMATRIX centerTrans1, centerTrans2;
	
	D3DXMatrixTranslation(&centerTrans1, -cx, -cy, -cz);
	D3DXMatrixTranslation(&centerTrans2, cx, cy, cz);
	//world = centerTrans1*rotate*centerTrans2*translate;
	world = scale*rotate*translate*(*ref);
	dev->SetTransform(D3DTS_WORLD, &world);
	
	dev->SetStreamSource(0, vertexBuffer, 0, sizeof(DetailedTexturedVertex));
	dev->SetIndices(indexBuffer);
	//Buffer offsets for vertex and index buffers respectively
	int i = 0;
	int v = 0;
	for (int c = 0; c < materialSet->iMaterials(); c++)
	{
		int faceCount = perTextureFaceCount[c];
		dev->SetTexture(0, materialSet->materialAtIndex(c)->colorMap());
		
		dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, v, faceCount*4, i, faceCount*2);

		v += faceCount*4;
		i += faceCount*6;
	}
}

bool SMesh::loadMeshFromVoxelData(ps::VoxelData* voxelData)
{
	
	//set buffers to null
	vertexBuffer = NULL;
	indexBuffer = NULL;
	//find the amount of faces per texture
	for (int x = 0; x < 64; x++){
		perTextureFaceCount[x] = 0;
	}

	int xDim = voxelData->x();
	int yDim = voxelData->y();
	int zDim = voxelData->z();

	for (int z = 0; z<zDim; z++){
		for (int y = 0; y<yDim; y++){
			for (int x = 0; x<xDim; x++){
				if (voxelData->data(x,y,z) > 0 && voxelData->data(x,y,z) < 65){

					char data = voxelData->data(x,y,z);
					//check for x-
					if (x==0) {
						perTextureFaceCount[data - 1]++;
					}
					if (x>0){
						if (voxelData->data(x-1,y,z) == 0){
							perTextureFaceCount[data - 1]++;
						}
					}
					//check for x+
					if (x==(xDim-1)){
						perTextureFaceCount[data - 1]++;
					}
					if (x<(xDim-1)){
						if (voxelData->data(x+1,y,z) == 0){
							perTextureFaceCount[data - 1]++;
						}
					}
					//check for y-
					if (y==0){
						perTextureFaceCount[data - 1]++;
					}
					if (y>0){
						if (voxelData->data(x,y-1,z) == 0){
							perTextureFaceCount[data - 1]++;
						}
					}
					//check for y+
					if (y==(yDim-1)){
						perTextureFaceCount[data - 1]++;
					}
					if (y<(yDim-1)){
						if (voxelData->data(x,y+1,z) == 0){
							perTextureFaceCount[data - 1]++;
						}
					}
					//check for z-
					if (z==0){
						perTextureFaceCount[data - 1]++;
					}
					if (z>0){
						if (voxelData->data(x,y,z-1) == 0){
							perTextureFaceCount[data - 1]++;
						}
					}
					//check for z+
					if (z==(zDim-1)){
						perTextureFaceCount[data - 1]++;
					}
					if (z<(zDim-1)){
						if (voxelData->data(x,y,z+1) == 0){
							perTextureFaceCount[data - 1]++;
						}
					}
				}
			}
		}
	}

	float centeringOffsetX = (float)xDim * 0.5f;
	float centeringOffsetY = (float)yDim * 0.5f;
	float centeringOffsetZ = (float)zDim * 0.5f;
	//index: v for vertex, i for index
	unsigned long cubeIndex_v = 0;
	unsigned long cubeIndex_i = 0;
	
	unsigned long size = 0;
	//alloc 24 verts per cube
	DetailedTexturedVertex* verts;
	//alloc 36 shorts per cube
	int* indices;
	for (int i = 0; i < 64; i++)
	{
		size+=perTextureFaceCount[i];
	}
	dev->CreateVertexBuffer(sizeof(DetailedTexturedVertex)*size*4,
			0,
			0,
			D3DPOOL_MANAGED,
			&vertexBuffer,
			NULL);
	dev->CreateIndexBuffer(sizeof(int)*size*6,
			0,
			D3DFMT_INDEX32,
			D3DPOOL_MANAGED,
			&indexBuffer,
			NULL);

	vertexBuffer->Lock(0, 0, (void**)&verts, 0);
	indexBuffer->Lock(0, 0, (void**)&indices, 0);
	for (int type = 1; type < 64; type++){
		for (int z = 0; z<zDim; z++){ 
			for (int y = 0; y<yDim; y++){ 
				for (int x = 0; x<xDim; x++){


					char s = voxelData->data(x,y,z);
					if (s > 0){

						if (type == s){
							//BUILD A CUBE

							//--VERTICES--and INDICES
							bool addVertex = false;
							//SIDE Z+
							if (z==(zDim-1)) addVertex = true;
							if (z<(zDim-1)){
								if (voxelData->data(x,y,z+1) == 0) addVertex = true;
							}
							if (addVertex){
								
								verts[cubeIndex_v].x = x+0.0f; verts[cubeIndex_v].y = y+0.0f; verts[cubeIndex_v].z = z+1.0f; verts[cubeIndex_v].u1 = 1.0f; verts[cubeIndex_v].v1 = 1.0f; verts[cubeIndex_v].normal = D3DXVECTOR3(0.0f, 0.0f, 1.0f); verts[cubeIndex_v].tangent = D3DXVECTOR3(-1.0f, 0.0f, 0.0f); verts[cubeIndex_v].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
								verts[cubeIndex_v+1].x = x+1.0f; verts[cubeIndex_v+1].y = y+0.0f; verts[cubeIndex_v+1].z = z+1.0f;verts[cubeIndex_v+1].u1 = 0.0f; verts[cubeIndex_v+1].v1 = 1.0f; verts[cubeIndex_v+1].normal = D3DXVECTOR3(0.0f, 0.0f, 1.0f); verts[cubeIndex_v+1].tangent = D3DXVECTOR3(-1.0f, 0.0f, 0.0f); verts[cubeIndex_v+1].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
								verts[cubeIndex_v+2].x = x+1.0f; verts[cubeIndex_v+2].y = y+1.0f; verts[cubeIndex_v+2].z = z+1.0f;verts[cubeIndex_v+2].u1 = 0.0f; verts[cubeIndex_v+2].v1 = 0.0f; verts[cubeIndex_v+2].normal = D3DXVECTOR3(0.0f, 0.0f, 1.0f); verts[cubeIndex_v+2].tangent = D3DXVECTOR3(-1.0f, 0.0f, 0.0f); verts[cubeIndex_v+2].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
								verts[cubeIndex_v+3].x = x+0.0f; verts[cubeIndex_v+3].y = y+1.0f; verts[cubeIndex_v+3].z = z+1.0f;verts[cubeIndex_v+3].u1 = 1.0f; verts[cubeIndex_v+3].v1 = 0.0f; verts[cubeIndex_v+3].normal = D3DXVECTOR3(0.0f, 0.0f, 1.0f); verts[cubeIndex_v+3].tangent = D3DXVECTOR3(-1.0f, 0.0f, 0.0f); verts[cubeIndex_v+3].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
								indices[cubeIndex_i] = cubeIndex_v; indices[cubeIndex_i+1] = cubeIndex_v+1; indices[cubeIndex_i+2] = cubeIndex_v+2;
								indices[cubeIndex_i+3] = cubeIndex_v; indices[cubeIndex_i+4] = cubeIndex_v+2; indices[cubeIndex_i+5] = cubeIndex_v+3;
								for (int v = 0; v < 4; v++)
								{
									verts[cubeIndex_v+v].x -= centeringOffsetX;
									verts[cubeIndex_v+v].y -= centeringOffsetY;
									verts[cubeIndex_v+v].z -= centeringOffsetZ;
									//verts[cubeIndex_v+v].diffuse = 0xffffffff;
								}
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
								
								verts[cubeIndex_v].x = x+0.0f; verts[cubeIndex_v].y = y+0.0f; verts[cubeIndex_v].z = z+0.0f; verts[cubeIndex_v].u1 = 0.0f; verts[cubeIndex_v].v1 = 1.0f; verts[cubeIndex_v].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f); verts[cubeIndex_v].tangent = D3DXVECTOR3(1.0f, 0.0f, 0.0f); verts[cubeIndex_v].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
								verts[cubeIndex_v+1].x = x+1.0f; verts[cubeIndex_v+1].y = y+0.0f; verts[cubeIndex_v+1].z = z+0.0f; verts[cubeIndex_v+1].u1 = 1.0f; verts[cubeIndex_v+1].v1 = 1.0f; verts[cubeIndex_v+1].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f); verts[cubeIndex_v+1].tangent = D3DXVECTOR3(1.0f, 0.0f, 0.0f); verts[cubeIndex_v+1].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
								verts[cubeIndex_v+2].x = x+1.0f; verts[cubeIndex_v+2].y = y+1.0f; verts[cubeIndex_v+2].z = z+0.0f; verts[cubeIndex_v+2].u1 = 1.0f; verts[cubeIndex_v+2].v1 = 0.0f; verts[cubeIndex_v+2].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f); verts[cubeIndex_v+2].tangent = D3DXVECTOR3(1.0f, 0.0f, 0.0f); verts[cubeIndex_v+2].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
								verts[cubeIndex_v+3].x = x+0.0f; verts[cubeIndex_v+3].y = y+1.0f; verts[cubeIndex_v+3].z = z+0.0f; verts[cubeIndex_v+3].u1 = 0.0f; verts[cubeIndex_v+3].v1 = 0.0f; verts[cubeIndex_v+3].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f); verts[cubeIndex_v+3].tangent = D3DXVECTOR3(1.0f, 0.0f, 0.0f); verts[cubeIndex_v+3].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
								indices[cubeIndex_i] = cubeIndex_v; indices[cubeIndex_i+1] = cubeIndex_v+2; indices[cubeIndex_i+2] = cubeIndex_v+1;
								indices[cubeIndex_i+3] = cubeIndex_v; indices[cubeIndex_i+4] = cubeIndex_v+3; indices[cubeIndex_i+5] = cubeIndex_v+2;
								for (int v = 0; v < 4; v++)
								{
									verts[cubeIndex_v+v].x -= centeringOffsetX;
									verts[cubeIndex_v+v].y -= centeringOffsetY;
									verts[cubeIndex_v+v].z -= centeringOffsetZ;
									//verts[cubeIndex_v+v].diffuse = 0xffffffff;
								}
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
								
								verts[cubeIndex_v].x = x+1.0f; verts[cubeIndex_v].y = y+0.0f; verts[cubeIndex_v].z = z+0.0f; verts[cubeIndex_v].u1 = 0.0f; verts[cubeIndex_v].v1 = 1.0f; verts[cubeIndex_v].normal = D3DXVECTOR3(1.0f, 0.0f, 0.0f); verts[cubeIndex_v].tangent = D3DXVECTOR3(0.0f, 0.0f, 1.0f); verts[cubeIndex_v].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
								verts[cubeIndex_v+1].x = x+1.0f; verts[cubeIndex_v+1].y = y+0.0f; verts[cubeIndex_v+1].z = z+1.0f; verts[cubeIndex_v+1].u1 = 1.0f; verts[cubeIndex_v+1].v1 = 1.0f; verts[cubeIndex_v+1].normal = D3DXVECTOR3(1.0f, 0.0f, 0.0f); verts[cubeIndex_v+1].tangent = D3DXVECTOR3(0.0f, 0.0f, 1.0f); verts[cubeIndex_v+1].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
								verts[cubeIndex_v+2].x = x+1.0f; verts[cubeIndex_v+2].y = y+1.0f; verts[cubeIndex_v+2].z = z+1.0f; verts[cubeIndex_v+2].u1 = 1.0f; verts[cubeIndex_v+2].v1 = 0.0f; verts[cubeIndex_v+2].normal = D3DXVECTOR3(1.0f, 0.0f, 0.0f); verts[cubeIndex_v+2].tangent = D3DXVECTOR3(0.0f, 0.0f, 1.0f); verts[cubeIndex_v+2].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
								verts[cubeIndex_v+3].x = x+1.0f; verts[cubeIndex_v+3].y = y+1.0f; verts[cubeIndex_v+3].z = z+0.0f; verts[cubeIndex_v+3].u1 = 0.0f; verts[cubeIndex_v+3].v1 = 0.0f; verts[cubeIndex_v+3].normal = D3DXVECTOR3(1.0f, 0.0f, 0.0f); verts[cubeIndex_v+3].tangent = D3DXVECTOR3(0.0f, 0.0f, 1.0f); verts[cubeIndex_v+3].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
								indices[cubeIndex_i] = cubeIndex_v; indices[cubeIndex_i+1] = cubeIndex_v+2; indices[cubeIndex_i+2] = cubeIndex_v+1;
								indices[cubeIndex_i+3] = cubeIndex_v; indices[cubeIndex_i+4] = cubeIndex_v+3; indices[cubeIndex_i+5] = cubeIndex_v+2;
								for (int v = 0; v < 4; v++)
								{
									verts[cubeIndex_v+v].x -= centeringOffsetX;
									verts[cubeIndex_v+v].y -= centeringOffsetY;
									verts[cubeIndex_v+v].z -= centeringOffsetZ;
									//verts[cubeIndex_v+v].diffuse = 0xffffffff;
								}
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
								
								verts[cubeIndex_v].x = x+0.0f; verts[cubeIndex_v].y = y+0.0f; verts[cubeIndex_v].z = z+0.0f; verts[cubeIndex_v].u1 = 1.0f; verts[cubeIndex_v].v1 = 1.0f; verts[cubeIndex_v].normal = D3DXVECTOR3(-1.0f, 0.0f, 0.0f); verts[cubeIndex_v].tangent = D3DXVECTOR3(0.0f, 0.0f, -1.0f); verts[cubeIndex_v].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
								verts[cubeIndex_v+1].x = x+0.0f; verts[cubeIndex_v+1].y = y+0.0f; verts[cubeIndex_v+1].z = z+1.0f; verts[cubeIndex_v+1].u1 = 0.0f; verts[cubeIndex_v+1].v1 = 1.0f; verts[cubeIndex_v+1].normal = D3DXVECTOR3(-1.0f, 0.0f, 0.0f); verts[cubeIndex_v+1].tangent = D3DXVECTOR3(0.0f, 0.0f, -1.0f); verts[cubeIndex_v+1].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
								verts[cubeIndex_v+2].x = x+0.0f; verts[cubeIndex_v+2].y = y+1.0f; verts[cubeIndex_v+2].z = z+1.0f; verts[cubeIndex_v+2].u1 = 0.0f; verts[cubeIndex_v+2].v1 = 0.0f; verts[cubeIndex_v+2].normal = D3DXVECTOR3(-1.0f, 0.0f, 0.0f); verts[cubeIndex_v+2].tangent = D3DXVECTOR3(0.0f, 0.0f, -1.0f); verts[cubeIndex_v+2].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
								verts[cubeIndex_v+3].x = x+0.0f; verts[cubeIndex_v+3].y = y+1.0f; verts[cubeIndex_v+3].z = z+0.0f; verts[cubeIndex_v+3].u1 = 1.0f; verts[cubeIndex_v+3].v1 = 0.0f; verts[cubeIndex_v+3].normal = D3DXVECTOR3(-1.0f, 0.0f, 0.0f); verts[cubeIndex_v+3].tangent = D3DXVECTOR3(0.0f, 0.0f, -1.0f); verts[cubeIndex_v+3].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
								indices[cubeIndex_i] = cubeIndex_v; indices[cubeIndex_i+1] = cubeIndex_v+1; indices[cubeIndex_i+2] = cubeIndex_v+2; 
								indices[cubeIndex_i+3] = cubeIndex_v; indices[cubeIndex_i+4] = cubeIndex_v+2; indices[cubeIndex_i+5] = cubeIndex_v+3;
								for (int v = 0; v < 4; v++)
								{
									verts[cubeIndex_v+v].x -= centeringOffsetX;
									verts[cubeIndex_v+v].y -= centeringOffsetY;
									verts[cubeIndex_v+v].z -= centeringOffsetZ;
									//verts[cubeIndex_v+v].diffuse = 0xffffffff;
								}
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
								
								verts[cubeIndex_v].x = x+0.0f; verts[cubeIndex_v].y = y+1.0f; verts[cubeIndex_v].z = z+0.0f; verts[cubeIndex_v].u1 = 0.0f; verts[cubeIndex_v].v1 = 1.0f; verts[cubeIndex_v].normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f); verts[cubeIndex_v].tangent = D3DXVECTOR3(1.0f, 0.0f, 0.0f); verts[cubeIndex_v].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
								verts[cubeIndex_v+1].x = x+1.0f; verts[cubeIndex_v+1].y = y+1.0f; verts[cubeIndex_v+1].z = z+0.0f; verts[cubeIndex_v+1].u1 = 1.0f; verts[cubeIndex_v+1].v1 = 1.0f; verts[cubeIndex_v+1].normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f); verts[cubeIndex_v+1].tangent = D3DXVECTOR3(1.0f, 0.0f, 0.0f); verts[cubeIndex_v+1].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
								verts[cubeIndex_v+2].x = x+1.0f; verts[cubeIndex_v+2].y = y+1.0f; verts[cubeIndex_v+2].z = z+1.0f; verts[cubeIndex_v+2].u1 = 1.0f; verts[cubeIndex_v+2].v1 = 0.0f; verts[cubeIndex_v+2].normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f); verts[cubeIndex_v+2].tangent = D3DXVECTOR3(1.0f, 0.0f, 0.0f); verts[cubeIndex_v+2].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
								verts[cubeIndex_v+3].x = x+0.0f; verts[cubeIndex_v+3].y = y+1.0f; verts[cubeIndex_v+3].z = z+1.0f; verts[cubeIndex_v+3].u1 = 0.0f; verts[cubeIndex_v+3].v1 = 0.0f; verts[cubeIndex_v+3].normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f); verts[cubeIndex_v+3].tangent = D3DXVECTOR3(1.0f, 0.0f, 0.0f); verts[cubeIndex_v+3].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
								indices[cubeIndex_i] = cubeIndex_v; indices[cubeIndex_i+1] = cubeIndex_v+2; indices[cubeIndex_i+2] = cubeIndex_v+1;
								indices[cubeIndex_i+3] = cubeIndex_v; indices[cubeIndex_i+4] = cubeIndex_v+3; indices[cubeIndex_i+5] = cubeIndex_v+2;
								for (int v = 0; v < 4; v++)
								{
									verts[cubeIndex_v+v].x -= centeringOffsetX;
									verts[cubeIndex_v+v].y -= centeringOffsetY;
									verts[cubeIndex_v+v].z -= centeringOffsetZ;
									//verts[cubeIndex_v+v].diffuse = 0xffffffff;
								}
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
								
								verts[cubeIndex_v].x = x+0.0f; verts[cubeIndex_v].y = y+0.0f; verts[cubeIndex_v].z = z+0.0f;verts[cubeIndex_v].u1 = 1.0f; verts[cubeIndex_v].v1 = 1.0f; verts[cubeIndex_v].normal = D3DXVECTOR3(0.0f, -1.0f, 0.0f); verts[cubeIndex_v].tangent = D3DXVECTOR3(-1.0f, 0.0f, 0.0f); verts[cubeIndex_v].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
								verts[cubeIndex_v+1].x = x+1.0f; verts[cubeIndex_v+1].y = y+0.0f; verts[cubeIndex_v+1].z = z+0.0f; verts[cubeIndex_v+1].u1 = 0.0f; verts[cubeIndex_v+1].v1 = 1.0f; verts[cubeIndex_v+1].normal = D3DXVECTOR3(0.0f, -1.0f, 0.0f); verts[cubeIndex_v+1].tangent = D3DXVECTOR3(-1.0f, 0.0f, 0.0f); verts[cubeIndex_v+1].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
								verts[cubeIndex_v+2].x = x+1.0f; verts[cubeIndex_v+2].y = y+0.0f; verts[cubeIndex_v+2].z = z+1.0f; verts[cubeIndex_v+2].u1 = 0.0f; verts[cubeIndex_v+2].v1 = 0.0f; verts[cubeIndex_v+2].normal = D3DXVECTOR3(0.0f, -1.0f, 0.0f); verts[cubeIndex_v+2].tangent = D3DXVECTOR3(-1.0f, 0.0f, 0.0f); verts[cubeIndex_v+2].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
								verts[cubeIndex_v+3].x = x+0.0f; verts[cubeIndex_v+3].y = y+0.0f; verts[cubeIndex_v+3].z = z+1.0f; verts[cubeIndex_v+3].u1 = 1.0f; verts[cubeIndex_v+3].v1 = 0.0f; verts[cubeIndex_v+3].normal = D3DXVECTOR3(0.0f, -1.0f, 0.0f); verts[cubeIndex_v+3].tangent = D3DXVECTOR3(-1.0f, 0.0f, 0.0f); verts[cubeIndex_v+3].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
								indices[cubeIndex_i] = cubeIndex_v; indices[cubeIndex_i+1] = cubeIndex_v+1; indices[cubeIndex_i+2] = cubeIndex_v+2;
								indices[cubeIndex_i+3] = cubeIndex_v; indices[cubeIndex_i+4] = cubeIndex_v+2; indices[cubeIndex_i+5] = cubeIndex_v+3;
								for (int v = 0; v < 4; v++)
								{
									verts[cubeIndex_v+v].x -= centeringOffsetX;
									verts[cubeIndex_v+v].y -= centeringOffsetY;
									verts[cubeIndex_v+v].z -= centeringOffsetZ;
									//verts[cubeIndex_v+v].diffuse = 0xffffffff;
								}
								cubeIndex_v += 4;
								cubeIndex_i += 6;
								addVertex = false;
							}
						}

					}
				}
			}
		}
	}
	vertexBuffer->Unlock();
	indexBuffer->Unlock();


	_staticMeshType = SRENDER_STATICMESHTYPE_VOXEL;
	return true;
}

bool SMesh::loadMeshFromX(std::wstring path)
{
	HRESULT hr = D3DXLoadMeshFromX(path.c_str(), D3DXMESH_MANAGED, dev, NULL, NULL, NULL, NULL, &mesh);
	if (hr == 0)
	{
		_staticMeshType = SRENDER_STATICMESHTYPE_STANDARD;
		return true;
	}

	return false;

}

bool SMesh::loadMeshFromFBX(std::string path)
{
	_staticMeshType = SRENDER_STATICMESHTYPE_STANDARD;
      FbxManager* g_pFbxSdkManager = FbxManager::Create();

      FbxIOSettings* pIOsettings = FbxIOSettings::Create(g_pFbxSdkManager, IOSROOT );
      g_pFbxSdkManager->SetIOSettings(pIOsettings);
   

   FbxImporter* pImporter = FbxImporter::Create(g_pFbxSdkManager,"");
   FbxScene* pFbxScene = FbxScene::Create(g_pFbxSdkManager,"");

   bool bSuccess = pImporter->Initialize(path.c_str(), -1, g_pFbxSdkManager->GetIOSettings() );
   if(!bSuccess) return false;

   bSuccess = pImporter->Import(pFbxScene);
   if(!bSuccess) return false;

   pImporter->Destroy();

   FbxNode* pFbxRootNode = pFbxScene->GetRootNode();

   std::vector<DetailedTexturedVertex> meshVerts;
   if(pFbxRootNode)
   {
      for(int i = 0; i < pFbxRootNode->GetChildCount(); i++)
      {
         FbxNode* pFbxChildNode = pFbxRootNode->GetChild(i);

         if(pFbxChildNode->GetNodeAttribute() == NULL)
            continue;

         FbxNodeAttribute::EType AttributeType = pFbxChildNode->GetNodeAttribute()->GetAttributeType();

         if(AttributeType != FbxNodeAttribute::eMesh)
            continue;

         FbxMesh* pMesh = (FbxMesh*) pFbxChildNode->GetNodeAttribute();

         FbxVector4* pVertices = pMesh->GetControlPoints();
		 FbxLayerElementArrayTemplate<FbxVector2>* pUVs = 0;
		 pMesh->GetTextureUV(&pUVs, FbxLayerElement::eTextureDiffuse);

         for (int j = 0; j < pMesh->GetPolygonCount(); j++)
         {
            int iNumVertices = pMesh->GetPolygonSize(j);
            

            for (int k = 0; k < iNumVertices; k++)
            {
               int iControlPointIndex = pMesh->GetPolygonVertex(j, k);

               DetailedTexturedVertex vertex;
			   vertex.x = (float)pVertices[iControlPointIndex].mData[0];
               vertex.y = (float)pVertices[iControlPointIndex].mData[1];
               vertex.z = (float)pVertices[iControlPointIndex].mData[2];
               meshVerts.push_back( vertex );
            }
         }

      }

   }
   return S_OK;
	return true;
}

bool SMesh::createSkyboxMesh(void)
{
	
	perTextureFaceCount[0] = 6;
	_staticMeshType = SRENDER_STATICMESHTYPE_VOXEL;
	//Create skybox vertex and index buffer shitaaaa
	dev->CreateVertexBuffer(sizeof(DetailedTexturedVertex)*24, 0, SRENDER_POSNORTX1, D3DPOOL_MANAGED, &vertexBuffer, 0);
	dev->CreateIndexBuffer(sizeof(short)*36, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &indexBuffer, 0);

	DetailedTexturedVertex* verts;
	short* indices;

	vertexBuffer->Lock(0, 0, (void**)&verts, 0);
	indexBuffer->Lock(0, 0, (void**)&indices, 0);

	//SIDE Z+
	int cubeIndex_v = 0;
	int cubeIndex_i = 0;

	verts[cubeIndex_v].x = -1.0f; verts[cubeIndex_v].y = -1.0f; verts[cubeIndex_v].z = 1.0f; verts[cubeIndex_v].u1 = 1.0f; verts[cubeIndex_v].v1 = 0.6666f; verts[cubeIndex_v].normal = D3DXVECTOR3(0.0f, 0.0f, 1.0f); verts[cubeIndex_v].tangent = D3DXVECTOR3(-1.0f, 0.0f, 0.0f); verts[cubeIndex_v].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	verts[cubeIndex_v+1].x = 1.0f; verts[cubeIndex_v+1].y = -1.0f; verts[cubeIndex_v+1].z = 1.0f;verts[cubeIndex_v+1].u1 = 0.75f; verts[cubeIndex_v+1].v1 = 0.6666f; verts[cubeIndex_v+1].normal = D3DXVECTOR3(0.0f, 0.0f, 1.0f); verts[cubeIndex_v+1].tangent = D3DXVECTOR3(-1.0f, 0.0f, 0.0f); verts[cubeIndex_v+1].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	verts[cubeIndex_v+2].x = 1.0f; verts[cubeIndex_v+2].y = 1.0f; verts[cubeIndex_v+2].z = 1.0f;verts[cubeIndex_v+2].u1 = 0.75f; verts[cubeIndex_v+2].v1 = 0.3333f; verts[cubeIndex_v+2].normal = D3DXVECTOR3(0.0f, 0.0f, 1.0f); verts[cubeIndex_v+2].tangent = D3DXVECTOR3(-1.0f, 0.0f, 0.0f); verts[cubeIndex_v+2].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	verts[cubeIndex_v+3].x = -1.0f; verts[cubeIndex_v+3].y = 1.0f; verts[cubeIndex_v+3].z = 1.0f;verts[cubeIndex_v+3].u1 = 1.0f; verts[cubeIndex_v+3].v1 = 0.3333f; verts[cubeIndex_v+3].normal = D3DXVECTOR3(0.0f, 0.0f, 1.0f); verts[cubeIndex_v+3].tangent = D3DXVECTOR3(-1.0f, 0.0f, 0.0f); verts[cubeIndex_v+3].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	indices[cubeIndex_i] = cubeIndex_v; indices[cubeIndex_i+1] = cubeIndex_v+2; indices[cubeIndex_i+2] = cubeIndex_v+1;
	indices[cubeIndex_i+3] = cubeIndex_v; indices[cubeIndex_i+4] = cubeIndex_v+3; indices[cubeIndex_i+5] = cubeIndex_v+2;

	cubeIndex_v += 4;
	cubeIndex_i += 6;


	//Side Z-



	verts[cubeIndex_v].x = -1.0f; verts[cubeIndex_v].y = -1.0f; verts[cubeIndex_v].z = -1.0f; verts[cubeIndex_v].u1 = 0.25f; verts[cubeIndex_v].v1 = 0.6666f; verts[cubeIndex_v].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f); verts[cubeIndex_v].tangent = D3DXVECTOR3(1.0f, 0.0f, 0.0f); verts[cubeIndex_v].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	verts[cubeIndex_v+1].x = 1.0f; verts[cubeIndex_v+1].y = -1.0f; verts[cubeIndex_v+1].z = -1.0f; verts[cubeIndex_v+1].u1 = 0.5f; verts[cubeIndex_v+1].v1 = 0.6666f; verts[cubeIndex_v+1].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f); verts[cubeIndex_v+1].tangent = D3DXVECTOR3(1.0f, 0.0f, 0.0f); verts[cubeIndex_v+1].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	verts[cubeIndex_v+2].x = 1.0f; verts[cubeIndex_v+2].y = 1.0f; verts[cubeIndex_v+2].z = -1.0f; verts[cubeIndex_v+2].u1 = 0.25f; verts[cubeIndex_v+2].v1 = 0.3333f; verts[cubeIndex_v+2].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f); verts[cubeIndex_v+2].tangent = D3DXVECTOR3(1.0f, 0.0f, 0.0f); verts[cubeIndex_v+2].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	verts[cubeIndex_v+3].x = -1.0f; verts[cubeIndex_v+3].y = 1.0f; verts[cubeIndex_v+3].z = -1.0f; verts[cubeIndex_v+3].u1 = 0.5f; verts[cubeIndex_v+3].v1 = 0.3333f; verts[cubeIndex_v+3].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f); verts[cubeIndex_v+3].tangent = D3DXVECTOR3(1.0f, 0.0f, 0.0f); verts[cubeIndex_v+3].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	indices[cubeIndex_i] = cubeIndex_v; indices[cubeIndex_i+1] = cubeIndex_v+1; indices[cubeIndex_i+2] = cubeIndex_v+2;
	indices[cubeIndex_i+3] = cubeIndex_v; indices[cubeIndex_i+4] = cubeIndex_v+2; indices[cubeIndex_i+5] = cubeIndex_v+3;

	cubeIndex_v += 4;
	cubeIndex_i += 6;

	//Side X+


	verts[cubeIndex_v].x = 1.0f; verts[cubeIndex_v].y = -1.0f; verts[cubeIndex_v].z = -1.0f; verts[cubeIndex_v].u1 = 0.75f; verts[cubeIndex_v].v1 = 0.6666f; verts[cubeIndex_v].normal = D3DXVECTOR3(1.0f, 0.0f, 0.0f); verts[cubeIndex_v].tangent = D3DXVECTOR3(0.0f, 0.0f, 1.0f); verts[cubeIndex_v].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	verts[cubeIndex_v+1].x = 1.0f; verts[cubeIndex_v+1].y = -1.0f; verts[cubeIndex_v+1].z = 1.0f; verts[cubeIndex_v+1].u1 = 0.5f; verts[cubeIndex_v+1].v1 = 0.6666f; verts[cubeIndex_v+1].normal = D3DXVECTOR3(1.0f, 0.0f, 0.0f); verts[cubeIndex_v+1].tangent = D3DXVECTOR3(0.0f, 0.0f, 1.0f); verts[cubeIndex_v+1].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	verts[cubeIndex_v+2].x = 1.0f; verts[cubeIndex_v+2].y = 1.0f; verts[cubeIndex_v+2].z = 1.0f; verts[cubeIndex_v+2].u1 = 0.5f; verts[cubeIndex_v+2].v1 = 0.3333f; verts[cubeIndex_v+2].normal = D3DXVECTOR3(1.0f, 0.0f, 0.0f); verts[cubeIndex_v+2].tangent = D3DXVECTOR3(0.0f, 0.0f, 1.0f); verts[cubeIndex_v+2].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	verts[cubeIndex_v+3].x = 1.0f; verts[cubeIndex_v+3].y = 1.0f; verts[cubeIndex_v+3].z = -1.0f; verts[cubeIndex_v+3].u1 = 0.75f; verts[cubeIndex_v+3].v1 = 0.3333f; verts[cubeIndex_v+3].normal = D3DXVECTOR3(1.0f, 0.0f, 0.0f); verts[cubeIndex_v+3].tangent = D3DXVECTOR3(0.0f, 0.0f, 1.0f); verts[cubeIndex_v+3].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	indices[cubeIndex_i] = cubeIndex_v; indices[cubeIndex_i+1] = cubeIndex_v+1; indices[cubeIndex_i+2] = cubeIndex_v+2;
	indices[cubeIndex_i+3] = cubeIndex_v; indices[cubeIndex_i+4] = cubeIndex_v+2; indices[cubeIndex_i+5] = cubeIndex_v+3;

	cubeIndex_v += 4;
	cubeIndex_i += 6;

	//Side X-

	verts[cubeIndex_v].x = -1.0f; verts[cubeIndex_v].y = -1.0f; verts[cubeIndex_v].z = -1.0f; verts[cubeIndex_v].u1 = 0.0f; verts[cubeIndex_v].v1 = 0.6666f; verts[cubeIndex_v].normal = D3DXVECTOR3(-1.0f, 0.0f, 0.0f); verts[cubeIndex_v].tangent = D3DXVECTOR3(0.0f, 0.0f, -1.0f); verts[cubeIndex_v].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	verts[cubeIndex_v+1].x = -1.0f; verts[cubeIndex_v+1].y = -1.0f; verts[cubeIndex_v+1].z = 1.0f; verts[cubeIndex_v+1].u1 = 0.25f; verts[cubeIndex_v+1].v1 = 0.6666f; verts[cubeIndex_v+1].normal = D3DXVECTOR3(-1.0f, 0.0f, 0.0f); verts[cubeIndex_v+1].tangent = D3DXVECTOR3(0.0f, 0.0f, -1.0f); verts[cubeIndex_v+1].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	verts[cubeIndex_v+2].x = -1.0f; verts[cubeIndex_v+2].y = 1.0f; verts[cubeIndex_v+2].z = 1.0f; verts[cubeIndex_v+2].u1 = 0.25f; verts[cubeIndex_v+2].v1 = 0.3333f; verts[cubeIndex_v+2].normal = D3DXVECTOR3(-1.0f, 0.0f, 0.0f); verts[cubeIndex_v+2].tangent = D3DXVECTOR3(0.0f, 0.0f, -1.0f); verts[cubeIndex_v+2].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	verts[cubeIndex_v+3].x = -1.0f; verts[cubeIndex_v+3].y = 1.0f; verts[cubeIndex_v+3].z = -1.0f; verts[cubeIndex_v+3].u1 = 0.0f; verts[cubeIndex_v+3].v1 = 0.3333f; verts[cubeIndex_v+3].normal = D3DXVECTOR3(-1.0f, 0.0f, 0.0f); verts[cubeIndex_v+3].tangent = D3DXVECTOR3(0.0f, 0.0f, -1.0f); verts[cubeIndex_v+3].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	indices[cubeIndex_i] = cubeIndex_v; indices[cubeIndex_i+1] = cubeIndex_v+2; indices[cubeIndex_i+2] = cubeIndex_v+1; 
	indices[cubeIndex_i+3] = cubeIndex_v; indices[cubeIndex_i+4] = cubeIndex_v+3; indices[cubeIndex_i+5] = cubeIndex_v+2;

	cubeIndex_v += 4;
	cubeIndex_i += 6;


	//Side Y+

	verts[cubeIndex_v].x = -1.0f; verts[cubeIndex_v].y = 1.0f; verts[cubeIndex_v].z = -1.0f; verts[cubeIndex_v].u1 = 0.25f; verts[cubeIndex_v].v1 = 0.0f; verts[cubeIndex_v].normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f); verts[cubeIndex_v].tangent = D3DXVECTOR3(1.0f, 0.0f, 0.0f); verts[cubeIndex_v].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	verts[cubeIndex_v+1].x = 1.0f; verts[cubeIndex_v+1].y = 1.0f; verts[cubeIndex_v+1].z = -1.0f; verts[cubeIndex_v+1].u1 = 0.5f; verts[cubeIndex_v+1].v1 = 0.0f; verts[cubeIndex_v+1].normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f); verts[cubeIndex_v+1].tangent = D3DXVECTOR3(1.0f, 0.0f, 0.0f); verts[cubeIndex_v+1].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	verts[cubeIndex_v+2].x = 1.0f; verts[cubeIndex_v+2].y = 1.0f; verts[cubeIndex_v+2].z = 1.0f; verts[cubeIndex_v+2].u1 = 0.5f; verts[cubeIndex_v+2].v1 = 0.3333f; verts[cubeIndex_v+2].normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f); verts[cubeIndex_v+2].tangent = D3DXVECTOR3(1.0f, 0.0f, 0.0f); verts[cubeIndex_v+2].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	verts[cubeIndex_v+3].x = -1.0f; verts[cubeIndex_v+3].y = 1.0f; verts[cubeIndex_v+3].z = 1.0f; verts[cubeIndex_v+3].u1 = 0.25f; verts[cubeIndex_v+3].v1 = 0.3333f; verts[cubeIndex_v+3].normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f); verts[cubeIndex_v+3].tangent = D3DXVECTOR3(1.0f, 0.0f, 0.0f); verts[cubeIndex_v+3].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	indices[cubeIndex_i] = cubeIndex_v; indices[cubeIndex_i+1] = cubeIndex_v+1; indices[cubeIndex_i+2] = cubeIndex_v+2;
	indices[cubeIndex_i+3] = cubeIndex_v; indices[cubeIndex_i+4] = cubeIndex_v+2; indices[cubeIndex_i+5] = cubeIndex_v+3;

	cubeIndex_v += 4;
	cubeIndex_i += 6;

	//Side Y-

	verts[cubeIndex_v].x = -1.0f; verts[cubeIndex_v].y = -1.0f; verts[cubeIndex_v].z = -1.0f;verts[cubeIndex_v].u1 = 0.25f; verts[cubeIndex_v].v1 = 1.0f; verts[cubeIndex_v].normal = D3DXVECTOR3(0.0f, -1.0f, 0.0f); verts[cubeIndex_v].tangent = D3DXVECTOR3(-1.0f, 0.0f, 0.0f); verts[cubeIndex_v].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	verts[cubeIndex_v+1].x = 1.0f; verts[cubeIndex_v+1].y = -1.0f; verts[cubeIndex_v+1].z = -1.0f; verts[cubeIndex_v+1].u1 = 0.5f; verts[cubeIndex_v+1].v1 = 1.0f; verts[cubeIndex_v+1].normal = D3DXVECTOR3(0.0f, -1.0f, 0.0f); verts[cubeIndex_v+1].tangent = D3DXVECTOR3(-1.0f, 0.0f, 0.0f); verts[cubeIndex_v+1].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	verts[cubeIndex_v+2].x = 1.0f; verts[cubeIndex_v+2].y = -1.0f; verts[cubeIndex_v+2].z = 1.0f; verts[cubeIndex_v+2].u1 = 0.5f; verts[cubeIndex_v+2].v1 = 0.6666f; verts[cubeIndex_v+2].normal = D3DXVECTOR3(0.0f, -1.0f, 0.0f); verts[cubeIndex_v+2].tangent = D3DXVECTOR3(-1.0f, 0.0f, 0.0f); verts[cubeIndex_v+2].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	verts[cubeIndex_v+3].x = -1.0f; verts[cubeIndex_v+3].y = -1.0f; verts[cubeIndex_v+3].z = 1.0f; verts[cubeIndex_v+3].u1 = 0.25f; verts[cubeIndex_v+3].v1 = 0.6666f; verts[cubeIndex_v+3].normal = D3DXVECTOR3(0.0f, -1.0f, 0.0f); verts[cubeIndex_v+3].tangent = D3DXVECTOR3(-1.0f, 0.0f, 0.0f); verts[cubeIndex_v+3].binormal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
	indices[cubeIndex_i] = cubeIndex_v; indices[cubeIndex_i+1] = cubeIndex_v+2; indices[cubeIndex_i+2] = cubeIndex_v+1;
	indices[cubeIndex_i+3] = cubeIndex_v; indices[cubeIndex_i+4] = cubeIndex_v+3; indices[cubeIndex_i+5] = cubeIndex_v+2;

	cubeIndex_v += 4;
	cubeIndex_i += 6;



	vertexBuffer->Unlock();
	indexBuffer->Unlock();


	return true;
}
//temp function
float getDistance(float x1, float y1, float z1, float x2, float y2, float z2){
	return sqrt(((x1-x2)*(x1-x2)) + ((y1-y2)*(y1-y2)) + ((z1-z2)*(z1-z2)));
}

#include "SLOG.h"
void SMesh::calculateStaticLightVertexDiffuse64(ps::VoxelData* voxelData)
{

	
	//get size of light container
	int size = SRENDER_OBJECTCONTAINERSIZE;
	//get number of lights in calculation
	int lightCount = 0;
	for (int i = 0; i < SRENDER_OBJECTCONTAINERSIZE; i++)
	{
		if (SRender::getSingleton()->objLights[i] != 0) lightCount++;
	}

	//now export lights to buffer
	SLight* pLights;
	if (lightCount > 0) pLights = new SLight[lightCount];
	int bufferIndex = 0;
	SLight** ppLights = SRender::getSingleton()->objLights;
	//WE need to put the lights in the original untransformed object space

	for (int i = 0; i < SRENDER_OBJECTCONTAINERSIZE; i++)
	{
		if (SRender::getSingleton()->objLights[i] != 0)
		{
			pLights[bufferIndex].x = ppLights[i]->x;
			pLights[bufferIndex].y = ppLights[i]->y;
			pLights[bufferIndex].z = ppLights[i]->z;

			pLights[bufferIndex].r = ppLights[i]->r;
			pLights[bufferIndex].g = ppLights[i]->g;
			pLights[bufferIndex].b = ppLights[i]->b;
			 
			pLights[bufferIndex].ambientFactor = ppLights[i]->ambientFactor;

			pLights[bufferIndex].att0 = ppLights[i]->att0;
			pLights[bufferIndex].att1 = ppLights[i]->att1;
			pLights[bufferIndex].att2 = ppLights[i]->att2;

			pLights[bufferIndex].radius = ppLights[i]->radius;

			bufferIndex++;
		}
	}
	//get number of verts in calculation
	int vertexCount = 0;
	for (int i = 0; i < 64; i++)
	{
		vertexCount+=(perTextureFaceCount[i]*4);
	}

	TexturedDirectionalVertex* pVerts;
	vertexBuffer->Lock(0, 0, (void**)&pVerts, 0);
	float r, g, b;
	float vx, vy, vz;
	float lx, ly, lz;

	float toVoxelSpaceXOffset = (float)voxelData->x() * 0.5f;
	float toVoxelSpaceYOffset = (float)voxelData->y() * 0.5f;
	float toVoxelSpaceZOffset = (float)voxelData->z() * 0.5f;
	for (int i = 0; i < vertexCount; i++)
	{
		r = 0.0f;
		g = 0.0f;
		b = 0.0f;
		vx = pVerts[i].x + toVoxelSpaceXOffset;
		vy = pVerts[i].y + toVoxelSpaceYOffset;
		vz = pVerts[i].z + toVoxelSpaceZOffset;
		for (int j = 0; j < lightCount; j++)
		{
			
			lx = pLights[j].x;
			ly = pLights[j].y;
			lz = pLights[j].z;
			float d = getDistance(vx, vy, vz, lx, ly, lz);
			if (d > pLights[j].radius) continue;
			float attenuation = 1/((pLights[j].att0) + (pLights[j].att1*d) + (pLights[j].att2*d*d));
			r += attenuation*pLights[j].ambientFactor*pLights[j].r;
			g += attenuation*pLights[j].ambientFactor*pLights[j].g;
			b += attenuation*pLights[j].ambientFactor*pLights[j].b;


			float dot = 0.0f;
			D3DXVECTOR3 lightRay = D3DXVECTOR3(pLights[j].x, pLights[j].y, pLights[j].z) - D3DXVECTOR3(vx, vy, vz);
			D3DXVec3Normalize(&lightRay, &lightRay);
			D3DXVECTOR3 rayPos;
			rayPos.x = pLights[j].x;
			rayPos.y = pLights[j].y;
			rayPos.z = pLights[j].z;

			int alongRay = (int)d/0.1f;
			int checkNum = 0;
			bool applyLight = true;
			//set=0;
			if (voxelData!=0)
			{
				while (checkNum < alongRay)
				{
					checkNum++;
					rayPos -= lightRay*0.1f;
					if ((int)rayPos.x >= voxelData->x() || (int)rayPos.x < 0) continue;
					if ((int)rayPos.y >= voxelData->y() || (int)rayPos.y < 0) continue;
					if ((int)rayPos.y >= voxelData->z() || (int)rayPos.z < 0) continue;
					if (voxelData->data((int)rayPos.x,(int)rayPos.y,(int)rayPos.z) != 0)
					{
						applyLight = false;
						break;
					}
				}
			}
			
			if (applyLight)
			{
				dot = D3DXVec3Dot(&pVerts[i].normal, &lightRay);
				if (dot > 1.0f) dot = 1.0f;
				if (dot < 0.0f) dot = 0.0f;

				r += attenuation*pLights[j].r*dot;
				g += attenuation*pLights[j].g*dot;
				b += attenuation*pLights[j].b*dot;
			}
		}
		
		if (r > 1.0f) r = 1.0f;
		if (g > 1.0f) g = 1.0f;
		if (b > 1.0f) b = 1.0f;
		if (r < 0.0f) r = 0.0f;
		if (g < 0.0f) g = 0.0f;
		if (b < 0.0f) b = 0.0f;

		pVerts[i].diffuse = D3DCOLOR_XRGB((int)(r*255.0f), (int)(g*255.0f), (int)(b*255.0f));
		//pVerts[i].diffuse = 0xff6f6f6f;
	}

	vertexBuffer->Unlock();
	
}


void SMesh::renderDeferredO(D3DXMATRIX* ref, float x, float y, float z,
				float rx, float ry, float rz,
				float sx, float sy, float sz,
				float cx, float cy, float cz,
				LPD3DXEFFECT shader)
{
	SRender* r = SRender::getSingleton();


	D3DXMATRIX translate, rotate, scale;

	D3DXMatrixTranslation(&translate, x, y, z);
	D3DXMATRIX world;

	D3DXMATRIX matRotX, matRotY, matRotZ;
	D3DXMatrixRotationX(&matRotX, rx);
	D3DXMatrixRotationY(&matRotY, ry);
	D3DXMatrixRotationZ(&matRotZ, rz);
	rotate = matRotX*matRotY*matRotZ;

	D3DXMatrixScaling(&scale, sx, sy, sz);
	
	world = scale*rotate*translate*(*ref);
	
	shader->SetMatrix("world", &world);
	r->dev->SetStreamSource(0, vertexBuffer, 0, sizeof(DetailedTexturedVertex));
	r->dev->SetIndices(indexBuffer);
	//Buffer offsets for vertex and index buffers respectively
	int i = 0;
	int v = 0;
	for (int c = 0; c < materialSet->iMaterials(); c++)
	{
		int faceCount = perTextureFaceCount[c];
		r->dev->SetTexture(0, materialSet->materialAtIndex(c)->colorMap());
		r->dev->SetTexture(1, materialSet->materialAtIndex(c)->normalMap());
		
		r->dev->SetTexture(2, materialSet->materialAtIndex(c)->specularMap());

		shader->Begin(0,0);
		shader->BeginPass(0);
		dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, v, faceCount*4, i, faceCount*2);
		shader->EndPass();
		shader->End();

		v += faceCount*4;
		i += faceCount*6;
	}
}

void SMesh::renderDeferred(D3DXMATRIX* world, D3DXMATRIX* oldWorld, LPD3DXEFFECT shader)
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

	if (_staticMeshType == SRENDER_STATICMESHTYPE_VOXEL)
	{
		//Buffer offsets for vertex and index buffers respectively
		int i = 0;
		int v = 0;
		for (int c = 0; c < materialSet->iMaterials(); c++)
		{
			int faceCount = perTextureFaceCount[c];
			r->dev->SetTexture(0, materialSet->materialAtIndex(c)->colorMap());
			r->dev->SetTexture(1, materialSet->materialAtIndex(c)->normalMap());

			r->dev->SetTexture(2, materialSet->materialAtIndex(c)->specularMap());

			shader->Begin(0,0);
			shader->BeginPass(0);
			dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, v, faceCount*4, i, faceCount*2);
			shader->EndPass();
			shader->End();

			v += faceCount*4;
			i += faceCount*6;
		}
	}
	else if (_staticMeshType == SRENDER_STATICMESHTYPE_STANDARD)
	{
		r->dev->SetTexture(0, materialSet->materialAtIndex(0)->colorMap());
		r->dev->SetTexture(1, materialSet->materialAtIndex(0)->normalMap());
		r->dev->SetTexture(2, materialSet->materialAtIndex(0)->specularMap());

			shader->Begin(0,0);
			shader->BeginPass(0);
			mesh->DrawSubset(0);
			shader->EndPass();
			shader->End();
	}
	
	
}

void SMesh::renderDeferredDepth(D3DXMATRIX* world, LPD3DXEFFECT shader)
{
	SRender* r = SRender::getSingleton();
	shader->SetMatrix("world", world);
	
	r->dev->SetVertexDeclaration(r->VD_POS_NOR_TX1_TAN_BIN());
	r->dev->SetStreamSource(0, vertexBuffer, 0, sizeof(DetailedTexturedVertex));
	r->dev->SetIndices(indexBuffer);
	//Buffer offsets for vertex and index buffers respectively
	int i = 0;
	int v = 0;
	for (int c = 0; c < materialSet->iMaterials(); c++)
	{
		int faceCount = perTextureFaceCount[c];
		shader->Begin(0,0);
		shader->BeginPass(0);
		dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, v, faceCount*4, i, faceCount*2);
		shader->EndPass();
		shader->End();

		v += faceCount*4;
		i += faceCount*6;
	}
}