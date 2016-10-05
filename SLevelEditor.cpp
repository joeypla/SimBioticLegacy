#include "SLevelEditor.h"

#include <fstream>
#include "SGUI.h"
#include <sstream>
#include <d3dx9math.h>
#include <d3d9.h>
#include "SFont.h"
#include "SPhysics.h"
#include <fstream>

#define SEDITOR_GRAVITYCONFIG_XNEG 0
#define SEDITOR_GRAVITYCONFIG_XPOS 1
#define SEDITOR_GRAVITYCONFIG_YNEG 2 // THE STANDARD
#define SEDITOR_GRAVITYCONFIG_YPOS 3
#define SEDITOR_GRAVITYCONFIG_ZNEG 4
#define SEDITOR_GRAVITYCONFIG_ZPOS 5

/// some globals or other shit
float getUStart(int type);
float getVStart(int type);
LPD3DXSPRITE texSprite;
SLevelEditor::SLevelEditor(void){
	D3DXCreateSprite(S_GUI::getDevice(), &texSprite);
	firstClick.x = 0;
	firstClick.y = 0;
	firstClick.z = 0;
	secondClick.x = 0;
	secondClick.y = 0;
	secondClick.z = 0;
	textureAtlasPath = L"voxelTextureDefault.jpg";
	loadLevelFromFile(L"../Resources/Levels/Allocation/allocationLastSaved.psm");
	loadVoxelGeometry();
	createTextureAtlasFromFileGroup();
	//loadTextures();
	//loadVoxelGeometryProto();
	//loadTexturesProto();
	//user_direction.x = 0.0f; user_direction.y = 0.0f; user_direction.z = 1;
	user_position.x = 32.0f; user_position.y = 32.0f; user_position.z = 32.0f;
	user_upVector.x = 0.0f; user_upVector.y = 1.0f; user_upVector.z = 0.0f;
	user_lookAt.x = 0.0f; user_lookAt.y = 32.0f; user_lookAt.z = 0.0f;

	//Load Up Grid
	

	//some dx stuff
	S_GUI::getDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	S_GUI::getDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
	S_GUI::getDevice()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_ANISOTROPIC);

	//SET FAV BLOCKS
	favBlocks[0] = 1;
	favBlocks[1] = 2;
	favBlocks[2] = 3;
	favBlocks[3] = 4;
	favBlocks[4] = 5;
	favBlocks[5] = 6;
	favBlocks[6] = 7;
	favBlocks[7] = 8;
	favBlocks[8] = 9;
	favBlocks[9] = 10;

	
	lightSelection = 0;
	
	//SETUP BASIC MATERIAL
	ZeroMemory(&defaultMaterial, sizeof(defaultMaterial));
	defaultMaterial.Diffuse.r = 1.0f;
	defaultMaterial.Diffuse.g = 1.0f;
	defaultMaterial.Diffuse.b = 1.0f;
	defaultMaterial.Ambient.r = 1.0f;
	defaultMaterial.Ambient.g = 1.0f;
	defaultMaterial.Ambient.b = 1.0f;
	defaultMaterial.Specular.a = 1.0f;
	defaultMaterial.Specular.r = 1.0f;
	defaultMaterial.Specular.g = 1.0f;
	defaultMaterial.Specular.b = 1.0f;
	defaultMaterial.Power = 1700.0f;
	//S_GUI::getDevice()->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
	multiSelectClick = false;

	gravityConfig = SEDITOR_GRAVITYCONFIG_YNEG;
	roamingMode = true;
	gridMode = true;

	xBox = 0.5f;
	yBox = 1.5f;
	zBox = 0.5f;

	acceleration = D3DXVECTOR3(0.0f, -0.002f, 0.0f);
	velocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	isGrounded = false;

	grid.setOrientation(SEDITOR_GRID_ORIENTATION_Z);
	grid.updateGrid();
	selectedType = 0;
	//S_GUI::getDevice()->SetRenderState(D3DRS_FOGENABLE, true);
	//S_GUI::getDevice()->SetRenderState(D3DRS_RANGEFOGENABLE, true);
	S_GUI::getDevice()->SetRenderState(D3DRS_FOGCOLOR, D3DCOLOR_XRGB(0,0,0));
	S_GUI::getDevice()->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR);
	float fogEnd, fogStart;
	fogEnd = 160.0f;
	fogStart = 60.0f;
	S_GUI::getDevice()->SetRenderState(D3DRS_FOGSTART, *(DWORD*)(&fogStart));
	S_GUI::getDevice()->SetRenderState(D3DRS_FOGEND, *(DWORD*)(&fogEnd));
	S_GUI::setBackBufferColor(0,0,0);
	//S_GUI::getDevice()->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 16);

	createBoxBuffers();
	numLights = 100;
	lightingMode = false;
	//for (int i = 1; i < SLEVELEDITOR_MAXLIGHTS; i++){
		//lights[i].active = false;
	//}
}

SLevelEditor::~SLevelEditor(void){
}


bool SLevelEditor::loadLevelFromFile(std::wstring filename){
	for (int i = 0; i < SLEVELEDITOR_MAXLIGHTS; i++){
		lights[i].active = false;
	}
	std::ifstream file;
	file.open(filename, std::ios::binary);
	//now read raw voxel data
	if (file.is_open()){
		//READ VOXEL DATA
		for (int z = 0; z<MAP_DIMENSION; z++){
			for (int y = 0; y<MAP_DIMENSION; y++){
				for (int x = 0; x<MAP_DIMENSION; x++){
					voxelData[x][y][z].type = file.get();


				}
			}
		}
		//PART 2 - Read light data - FFP
		int totalLights_ffp = 0;
		file.read((char*)&totalLights_ffp, sizeof(int));
		
		for (int i = 0; i < totalLights_ffp; i++){
			lights[i].active = true;
			lights[i].light.Type = D3DLIGHT_POINT;
			//POSITION
			file.read((char*)&lights[i].light.Position.x, sizeof(float));
			file.read((char*)&lights[i].light.Position.y, sizeof(float));
			file.read((char*)&lights[i].light.Position.z, sizeof(float));

			//DIFFUSE
			file.read((char*)&lights[i].light.Diffuse.r, sizeof(float));
			file.read((char*)&lights[i].light.Diffuse.g, sizeof(float));
			file.read((char*)&lights[i].light.Diffuse.b, sizeof(float));

			//AMBIENT
			file.read((char*)&lights[i].light.Ambient.r, sizeof(float));
			file.read((char*)&lights[i].light.Ambient.g, sizeof(float));
			file.read((char*)&lights[i].light.Ambient.b, sizeof(float));

			//RANGE
			file.read((char*)&lights[i].light.Range, sizeof(float));

			//Attenuations - a0, a1, a2
			file.read((char*)&lights[i].light.Attenuation0, sizeof(float));
			file.read((char*)&lights[i].light.Attenuation1, sizeof(float));
			file.read((char*)&lights[i].light.Attenuation2, sizeof(float));

			lights[i].light.Specular.a = 0.0f;
			lights[i].light.Specular.r = 0.0f;
			lights[i].light.Specular.g = 0.0f;
			lights[i].light.Specular.b = 0.0f;

			lights[i].light.Attenuation0 = 0.0f;
			lights[i].light.Attenuation1 = 1.0f;
			lights[i].light.Attenuation2 = 0.0f;

			//
			if (i < 8){
				lightBuffer[i] = i;
			}
		}
		file.close();
	} else {
		for (int z = 0; z<MAP_DIMENSION; z++){
			for (int y = 0; y<MAP_DIMENSION; y++){
				for (int x = 0; x<MAP_DIMENSION; x++){
					voxelData[x][y][z].type = 0;


				}
			}
		}


		for (int x = 0; x<64; x++){
			for (int z = 0; z<64; z++){
				voxelData[x][0][z].type = 1;
				voxelData[x][63][z].type = 1;

				voxelData[x][z][0].type = 1;
				voxelData[x][z][63].type = 1;

				voxelData[0][x][z].type = 1;
				voxelData[63][x][z].type = 1;
			}
		}
	}
	
	return true;
}
#include "SLOG.h"
bool SLevelEditor::loadVoxelGeometry(void){
	totalMapVertexBuffer = NULL;
	totalMapIndexBuffer = NULL;

	faceCount = 0;
	for (int z = 0; z<MAP_DIMENSION; z++){
		for (int y = 0; y<MAP_DIMENSION; y++){
			for (int x = 0; x<MAP_DIMENSION; x++){
				if (voxelData[x][y][z].type != 0){
					//check for x-
					if (x==0) faceCount++;
					if (x>0){
						if (voxelData[x-1][y][z].type == 0) faceCount++;
					}
					//check for x+
					if (x==(MAP_DIMENSION-1)) faceCount++;
					if (x<(MAP_DIMENSION-1)){
						if (voxelData[x+1][y][z].type == 0) faceCount++;
					}
					//check for y-
					if (y==0) faceCount++;
					if (y>0){
						if (voxelData[x][y-1][z].type == 0) faceCount++;
					}
					//check for y+
					if (y==(MAP_DIMENSION-1)) faceCount++;
					if (y<(MAP_DIMENSION-1)){
						if (voxelData[x][y+1][z].type == 0) faceCount++;
					}
					//check for z-
					if (x==0) faceCount++;
					if (x>0){
						if (voxelData[x][y][z-1].type == 0) faceCount++;
					}
					//check for z+
					if (z==(MAP_DIMENSION-1)) faceCount++;
					if (z<(MAP_DIMENSION-1)){
						if (voxelData[x][y][z+1].type == 0) faceCount++;
					}
				}
			}
		}
	}

	//alloc 24 verts per cube
	SEDITOR_VOXELVERTEX* verts = new SEDITOR_VOXELVERTEX[32*32*32*24];
	//alloc 36 shorts per cube
	int* indices = new int[32*32*32*36];
	//index: v for vertex, i for index
	unsigned long cubeIndex_v = 0;
	unsigned long cubeIndex_i = 0;
	
	S_GUI::getDevice()->CreateVertexBuffer(sizeof(SEDITOR_VOXELVERTEX)*32*32*32*24,
			0,
			SEDITOR_VOXELFVF,
			D3DPOOL_MANAGED,
			&totalMapVertexBuffer,
			NULL);
	S_GUI::getDevice()->CreateIndexBuffer(sizeof(int)*32*32*32*36,
			0,
			D3DFMT_INDEX32,
			D3DPOOL_MANAGED,
			&totalMapIndexBuffer,
			NULL);
	
	totalMapVertexBuffer->Lock(0, 0, (void**)&verts, 0);
	totalMapIndexBuffer->Lock(0, 0, (void**)&indices, 0);
	for (int z = 0; z<MAP_DIMENSION; z++){ 
		for (int y = 0; y<MAP_DIMENSION; y++){ 
			for (int x = 0; x<MAP_DIMENSION; x++){
				

				char s = voxelData[x][y][z].type;
				if (s > 0){
					std::ostringstream ss;
					float uStart = 0.0f;
					float vStart = 0.0f;
					uStart = getUStart(s);
					vStart = getVStart(s);
					//BUILD A CUBE

					//--VERTICES--and INDICES
					bool addVertex = false;
					//SIDE Z+
					if (z==(MAP_DIMENSION-1)) addVertex = true;
					if (z<(MAP_DIMENSION-1)){
						if (voxelData[x][y][z+1].type == 0) addVertex = true;
					}
					if (addVertex){
						verts[cubeIndex_v].x = x+0.0f; verts[cubeIndex_v].y = y+0.0f; verts[cubeIndex_v].z = z+1.0f; verts[cubeIndex_v].color = 0xffffffff; verts[cubeIndex_v].u1 = uStart; verts[cubeIndex_v].v1 = vStart+0.125f; verts[cubeIndex_v].normal = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
						verts[cubeIndex_v+1].x = x+1.0f; verts[cubeIndex_v+1].y = y+0.0f; verts[cubeIndex_v+1].z = z+1.0f; verts[cubeIndex_v+1].color = 0xffffffff; verts[cubeIndex_v+1].u1 = uStart + 0.125f; verts[cubeIndex_v+1].v1 = vStart+0.125f; verts[cubeIndex_v+1].normal = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
						verts[cubeIndex_v+2].x = x+1.0f; verts[cubeIndex_v+2].y = y+1.0f; verts[cubeIndex_v+2].z = z+1.0f; verts[cubeIndex_v+2].color = 0xffffffff; verts[cubeIndex_v+2].u1 = uStart + 0.125f; verts[cubeIndex_v+2].v1 = vStart; verts[cubeIndex_v+2].normal = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
						verts[cubeIndex_v+3].x = x+0.0f; verts[cubeIndex_v+3].y = y+1.0f; verts[cubeIndex_v+3].z = z+1.0f; verts[cubeIndex_v+3].color = 0xffffffff; verts[cubeIndex_v+3].u1 = uStart; verts[cubeIndex_v+3].v1 = vStart; verts[cubeIndex_v+3].normal = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
						indices[cubeIndex_i] = cubeIndex_v; indices[cubeIndex_i+1] = cubeIndex_v+1; indices[cubeIndex_i+2] = cubeIndex_v+2;
						indices[cubeIndex_i+3] = cubeIndex_v; indices[cubeIndex_i+4] = cubeIndex_v+2; indices[cubeIndex_i+5] = cubeIndex_v+3;
						cubeIndex_v += 4;
						cubeIndex_i += 6;
						addVertex = false;
						ss<<"POS: "<<(float)5.0f<<","<<(float)verts[cubeIndex_v].y<<","<<(float)verts[cubeIndex_v].z;
						S_LOG::write(ss.str().c_str());
						ss.str("");
					}
					//Side Z-
					if (z==0) addVertex = true;
					if (z>0){
						if (voxelData[x][y][z-1].type == 0) addVertex = true;
					}
					if (addVertex){
						verts[cubeIndex_v].x = x+0.0f; verts[cubeIndex_v].y = y+0.0f; verts[cubeIndex_v].z = z+0.0f; verts[cubeIndex_v].color = 0xffffffff; verts[cubeIndex_v].u1 = uStart; verts[cubeIndex_v].v1 = vStart+0.125f; verts[cubeIndex_v].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
						verts[cubeIndex_v+1].x = x+1.0f; verts[cubeIndex_v+1].y = y+0.0f; verts[cubeIndex_v+1].z = z+0.0f; verts[cubeIndex_v+1].color = 0xffffffff; verts[cubeIndex_v+1].u1 = uStart + 0.125f; verts[cubeIndex_v+1].v1 = vStart+0.125f; verts[cubeIndex_v+1].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
						verts[cubeIndex_v+2].x = x+1.0f; verts[cubeIndex_v+2].y = y+1.0f; verts[cubeIndex_v+2].z = z+0.0f; verts[cubeIndex_v+2].color = 0xffffffff; verts[cubeIndex_v+2].u1 = uStart + 0.125f; verts[cubeIndex_v+2].v1 = vStart; verts[cubeIndex_v+2].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
						verts[cubeIndex_v+3].x = x+0.0f; verts[cubeIndex_v+3].y = y+1.0f; verts[cubeIndex_v+3].z = z+0.0f; verts[cubeIndex_v+3].color = 0xffffffff; verts[cubeIndex_v+3].u1 = uStart; verts[cubeIndex_v+3].v1 = vStart; verts[cubeIndex_v+3].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
						indices[cubeIndex_i] = cubeIndex_v; indices[cubeIndex_i+1] = cubeIndex_v+2; indices[cubeIndex_i+2] = cubeIndex_v+1;
						indices[cubeIndex_i+3] = cubeIndex_v; indices[cubeIndex_i+4] = cubeIndex_v+3; indices[cubeIndex_i+5] = cubeIndex_v+2;
						cubeIndex_v += 4;
						cubeIndex_i += 6;
						addVertex = false;
						ss<<verts[cubeIndex_v].x<<","<<verts[cubeIndex_v].y<<","<<verts[cubeIndex_v].z;
						S_LOG::write(ss.str().c_str());
						ss.str("");
					}
					//Side X+
					if (x==(MAP_DIMENSION-1)) addVertex = true;
					if (x<(MAP_DIMENSION-1)){
						if (voxelData[x+1][y][z].type == 0) addVertex = true;
					}
					if (addVertex){
						verts[cubeIndex_v].x = x+1.0f; verts[cubeIndex_v].y = y+0.0f; verts[cubeIndex_v].z = z+0.0f; verts[cubeIndex_v].color = 0xffffffff; verts[cubeIndex_v].u1 = uStart; verts[cubeIndex_v].v1 = vStart+0.125f; verts[cubeIndex_v].normal = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
						verts[cubeIndex_v+1].x = x+1.0f; verts[cubeIndex_v+1].y = y+0.0f; verts[cubeIndex_v+1].z = z+1.0f; verts[cubeIndex_v+1].color = 0xffffffff; verts[cubeIndex_v+1].u1 = uStart + 0.125f; verts[cubeIndex_v+1].v1 = vStart+0.125f; verts[cubeIndex_v+1].normal = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
						verts[cubeIndex_v+2].x = x+1.0f; verts[cubeIndex_v+2].y = y+1.0f; verts[cubeIndex_v+2].z = z+1.0f; verts[cubeIndex_v+2].color = 0xffffffff; verts[cubeIndex_v+2].u1 = uStart + 0.125f; verts[cubeIndex_v+2].v1 = vStart; verts[cubeIndex_v+2].normal = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
						verts[cubeIndex_v+3].x = x+1.0f; verts[cubeIndex_v+3].y = y+1.0f; verts[cubeIndex_v+3].z = z+0.0f; verts[cubeIndex_v+3].color = 0xffffffff; verts[cubeIndex_v+3].u1 = uStart; verts[cubeIndex_v+3].v1 = vStart; verts[cubeIndex_v+3].normal = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
						indices[cubeIndex_i] = cubeIndex_v; indices[cubeIndex_i+1] = cubeIndex_v+2; indices[cubeIndex_i+2] = cubeIndex_v+1;
						indices[cubeIndex_i+3] = cubeIndex_v; indices[cubeIndex_i+4] = cubeIndex_v+3; indices[cubeIndex_i+5] = cubeIndex_v+2;
						cubeIndex_v += 4;
						cubeIndex_i += 6;
						addVertex = false;
						ss<<verts[cubeIndex_v].x<<","<<verts[cubeIndex_v].y<<","<<verts[cubeIndex_v].z;
						S_LOG::write(ss.str().c_str());
						ss.str("");
					}
					//Side X-
					if (x==0) addVertex = true;
					if (x>0){
						if (voxelData[x-1][y][z].type == 0) addVertex = true;
					}
					if (addVertex){
						verts[cubeIndex_v].x = x+0.0f; verts[cubeIndex_v].y = y+0.0f; verts[cubeIndex_v].z = z+0.0f; verts[cubeIndex_v].color = 0xffffffff; verts[cubeIndex_v].u1 = uStart; verts[cubeIndex_v].v1 = vStart+0.125f; verts[cubeIndex_v].normal = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
						verts[cubeIndex_v+1].x = x+0.0f; verts[cubeIndex_v+1].y = y+0.0f; verts[cubeIndex_v+1].z = z+1.0f; verts[cubeIndex_v+1].color = 0xffffffff; verts[cubeIndex_v+1].u1 = uStart + 0.125f; verts[cubeIndex_v+1].v1 = vStart+0.125f; verts[cubeIndex_v+1].normal = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
						verts[cubeIndex_v+2].x = x+0.0f; verts[cubeIndex_v+2].y = y+1.0f; verts[cubeIndex_v+2].z = z+1.0f; verts[cubeIndex_v+2].color = 0xffffffff; verts[cubeIndex_v+2].u1 = uStart + 0.125f; verts[cubeIndex_v+2].v1 = vStart; verts[cubeIndex_v+2].normal = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
						verts[cubeIndex_v+3].x = x+0.0f; verts[cubeIndex_v+3].y = y+1.0f; verts[cubeIndex_v+3].z = z+0.0f; verts[cubeIndex_v+3].color = 0xffffffff; verts[cubeIndex_v+3].u1 = uStart; verts[cubeIndex_v+3].v1 = vStart; verts[cubeIndex_v+3].normal = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
						indices[cubeIndex_i] = cubeIndex_v; indices[cubeIndex_i+1] = cubeIndex_v+1; indices[cubeIndex_i+2] = cubeIndex_v+2;
						indices[cubeIndex_i+3] = cubeIndex_v; indices[cubeIndex_i+4] = cubeIndex_v+2; indices[cubeIndex_i+5] = cubeIndex_v+3;
						cubeIndex_v += 4;
						cubeIndex_i += 6;
						addVertex = false;
						ss<<verts[cubeIndex_v].x<<","<<verts[cubeIndex_v].y<<","<<verts[cubeIndex_v].z;
						S_LOG::write(ss.str().c_str());
						ss.str("");
					}
					//Side Y+
					if (y==(MAP_DIMENSION-1)) addVertex = true;
					if (y<(MAP_DIMENSION-1)){
						if (voxelData[x][y+1][z].type == 0) addVertex = true;
					}
					if (addVertex){
						verts[cubeIndex_v].x = x+0.0f; verts[cubeIndex_v].y = y+1.0f; verts[cubeIndex_v].z = z+0.0f; verts[cubeIndex_v].color = 0xffffffff; verts[cubeIndex_v].u1 = uStart; verts[cubeIndex_v].v1 = vStart+0.125f; verts[cubeIndex_v].normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
						verts[cubeIndex_v+1].x = x+1.0f; verts[cubeIndex_v+1].y = y+1.0f; verts[cubeIndex_v+1].z = z+0.0f; verts[cubeIndex_v+1].color = 0xffffffff; verts[cubeIndex_v+1].u1 = uStart + 0.125f; verts[cubeIndex_v+1].v1 = vStart+0.125f; verts[cubeIndex_v+1].normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
						verts[cubeIndex_v+2].x = x+1.0f; verts[cubeIndex_v+2].y = y+1.0f; verts[cubeIndex_v+2].z = z+1.0f; verts[cubeIndex_v+2].color = 0xffffffff; verts[cubeIndex_v+2].u1 = uStart + 0.125f; verts[cubeIndex_v+2].v1 = vStart; verts[cubeIndex_v+2].normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
						verts[cubeIndex_v+3].x = x+0.0f; verts[cubeIndex_v+3].y = y+1.0f; verts[cubeIndex_v+3].z = z+1.0f; verts[cubeIndex_v+3].color = 0xffffffff; verts[cubeIndex_v+3].u1 = uStart; verts[cubeIndex_v+3].v1 = vStart; verts[cubeIndex_v+3].normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
						indices[cubeIndex_i] = cubeIndex_v; indices[cubeIndex_i+1] = cubeIndex_v+2; indices[cubeIndex_i+2] = cubeIndex_v+1;
						indices[cubeIndex_i+3] = cubeIndex_v; indices[cubeIndex_i+4] = cubeIndex_v+3; indices[cubeIndex_i+5] = cubeIndex_v+2;
						cubeIndex_v += 4;
						cubeIndex_i += 6;
						addVertex = false;
						ss<<verts[cubeIndex_v].x<<","<<verts[cubeIndex_v].y<<","<<verts[cubeIndex_v].z;
						S_LOG::write(ss.str().c_str());
						ss.str("");
					}
					//Side Y-
					if (y==0) addVertex = true;
					if (y>0){
						if (voxelData[x][y-1][z].type == 0) addVertex = true;
					}
					if (addVertex){
						verts[cubeIndex_v].x = x+0.0f; verts[cubeIndex_v].y = y+0.0f; verts[cubeIndex_v].z = z+0.0f; verts[cubeIndex_v].color = 0xffffffff; verts[cubeIndex_v].u1 = uStart; verts[cubeIndex_v].v1 = vStart+0.125f; verts[cubeIndex_v].normal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
						verts[cubeIndex_v+1].x = x+1.0f; verts[cubeIndex_v+1].y = y+0.0f; verts[cubeIndex_v+1].z = z+0.0f; verts[cubeIndex_v+1].color = 0xffffffff; verts[cubeIndex_v+1].u1 = uStart + 0.125f; verts[cubeIndex_v+1].v1 = vStart+0.125f; verts[cubeIndex_v+1].normal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
						verts[cubeIndex_v+2].x = x+1.0f; verts[cubeIndex_v+2].y = y+0.0f; verts[cubeIndex_v+2].z = z+1.0f; verts[cubeIndex_v+2].color = 0xffffffff; verts[cubeIndex_v+2].u1 = uStart + 0.125f; verts[cubeIndex_v+2].v1 = vStart; verts[cubeIndex_v+2].normal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
						verts[cubeIndex_v+3].x = x+0.0f; verts[cubeIndex_v+3].y = y+0.0f; verts[cubeIndex_v+3].z = z+1.0f; verts[cubeIndex_v+3].color = 0xffffffff; verts[cubeIndex_v+3].u1 = uStart; verts[cubeIndex_v+3].v1 = vStart; verts[cubeIndex_v+3].normal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
						indices[cubeIndex_i] = cubeIndex_v; indices[cubeIndex_i+1] = cubeIndex_v+1; indices[cubeIndex_i+2] = cubeIndex_v+2;
						indices[cubeIndex_i+3] = cubeIndex_v; indices[cubeIndex_i+4] = cubeIndex_v+2; indices[cubeIndex_i+5] = cubeIndex_v+3;
						cubeIndex_v += 4;
						cubeIndex_i += 6;
						addVertex = false;
						ss<<verts[cubeIndex_v].x<<","<<verts[cubeIndex_v].y<<","<<verts[cubeIndex_v].z;
						S_LOG::write(ss.str().c_str());
						ss.str("");
					}
				}
			}
		}
	}
	totalMapVertexBuffer->Unlock();
	totalMapIndexBuffer->Unlock();

	return true;
}

bool SLevelEditor::loadTextures(void){
	//LOAD TEXTURE ATLAS
	D3DXCreateTextureFromFile(S_GUI::getDevice(), textureAtlasPath.c_str(), &totalMapTextureAtlas);
	//D3DXCreateTextureFromFileEx(S_GUI::getDevice(), textureAtlasPath.c_str(), 1024, 1024, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0xff000000, 0, 0, &totalMapTextureAtlas);
	return true;
}

bool SLevelEditor::renderToDevice(const LPDIRECT3DDEVICE9 dev){
	S_GUI::getDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	//Set Transforms
	D3DXMATRIX matWorld, matView, matProjection;
	D3DXMatrixIdentity(&matWorld);
	D3DXVECTOR3 camOffset;
	if (gravityConfig==SEDITOR_GRAVITYCONFIG_XNEG) camOffset = D3DXVECTOR3(1.75f, 0.0f, 0.0f);
	if (gravityConfig==SEDITOR_GRAVITYCONFIG_XPOS) camOffset = D3DXVECTOR3(-1.75f, 0.0f, 0.0f);
	if (gravityConfig==SEDITOR_GRAVITYCONFIG_YNEG) camOffset = D3DXVECTOR3(0.0f, 1.75f, 0.0f);
	if (gravityConfig==SEDITOR_GRAVITYCONFIG_YPOS) camOffset = D3DXVECTOR3(0.0f, -1.75f, 0.0f);
	if (gravityConfig==SEDITOR_GRAVITYCONFIG_ZNEG) camOffset = D3DXVECTOR3(0.0f, 0.0f, 1.75f);
	if (gravityConfig==SEDITOR_GRAVITYCONFIG_ZPOS) camOffset = D3DXVECTOR3(0.0f, 0.0f, -1.75f);
	D3DXMatrixLookAtLH(&matView, &(user_position+camOffset), &(user_lookAt+camOffset), &user_upVector);
	
	//D3DXMatrixLookAtLH(&matView, &user_position, &(user_position+user_direction), &user_upVector);
	D3DXMatrixPerspectiveFovLH(&matProjection, 0.9f, (float)S_Configs::s_bufferX/(float)S_Configs::s_bufferY, 0.1f, 100.0f);
	S_GUI::getDevice()->SetTransform(D3DTS_WORLD, &matWorld);
	S_GUI::getDevice()->SetTransform(D3DTS_VIEW, &matView);
	S_GUI::getDevice()->SetTransform(D3DTS_PROJECTION, &matProjection);

	//RENDER VOXELS
	
	S_GUI::getDevice()->SetRenderState(D3DRS_LIGHTING, true);
	S_GUI::getDevice()->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(10,10,10));
	S_GUI::getDevice()->SetRenderState(D3DRS_ZENABLE, true);
	S_GUI::getDevice()->SetFVF(SEDITOR_VOXELFVF);
	S_GUI::getDevice()->SetStreamSource(0, totalMapVertexBuffer, 0, sizeof(SEDITOR_VOXELVERTEX));
	S_GUI::getDevice()->SetIndices(totalMapIndexBuffer);
	S_GUI::getDevice()->SetMaterial(&defaultMaterial);
	/*int offsetv = 0;
	int offseti = 0;
	for (int x = 0; x < 8; x++){
		S_GUI::getDevice()->SetTexture(0, blockTextures[x]);
		S_GUI::getDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, offsetv, perTextureFaceCount[x] * 4, offseti, perTextureFaceCount[x] * 2);
		offsetv+=(perTextureFaceCount[x]*4);
		offseti+=(perTextureFaceCount[x]*6);

	}*/
	S_GUI::getDevice()->SetTexture(0, totalMapTextureAtlas);
	S_GUI::getDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, faceCount*4, 0, faceCount*2);
	
	//RENDER GRID
	grid.draw();

	//RENDER LIGHTS
	S_GUI::getDevice()->SetTexture(0, NULL);
	S_GUI::getDevice()->SetStreamSource(0, boxVertexBuffer, 0, sizeof(SEDITOR_SIMPLEVERTEX));
	S_GUI::getDevice()->SetIndices(boxIndexBuffer);
	S_GUI::getDevice()->SetRenderState(D3DRS_LIGHTING, FALSE);
	for (int i = 0; i < numLights; i ++){
		if (lights[i].active){
			D3DXMATRIX lightTranslate;
			D3DXMatrixTranslation(&lightTranslate, lights[i].light.Position.x, lights[i].light.Position.y, lights[i].light.Position.z);
			S_GUI::getDevice()->SetTransform(D3DTS_WORLD, &lightTranslate);
			S_GUI::getDevice()->DrawIndexedPrimitive(D3DPT_LINELIST, 0, 0, 8, 0, 12);
		}
	}
	//DEBUG INFO
	RECT fontPosition;
	fontPosition.bottom = 1080;
	fontPosition.top = 0;
	fontPosition.right = 1920;
	fontPosition.left = 0;

	std::ostringstream ss;
	

	D3DXVECTOR3 direction;
	D3DXVec3Normalize(&direction, &(user_lookAt-user_position));
	ss<<"Project Symbiote Editor 0.1a";
	S_FONT::defaultFont->DrawTextA(NULL, ss.str().c_str(), -1, &fontPosition, DT_LEFT, 0xffffffff);
	ss.str("");
	ss<<"_DEBUG_";
	fontPosition.top = 50;
	S_FONT::defaultFont->DrawTextA(NULL, ss.str().c_str(), -1, &fontPosition, DT_LEFT, 0xffffffff);
	ss.str("");
	ss<<"Gravity Configuration: ";
	if (gravityConfig==SEDITOR_GRAVITYCONFIG_XNEG)ss<<"X-NEGATIVE";
	if (gravityConfig==SEDITOR_GRAVITYCONFIG_XPOS)ss<<"X-POSITIVE";
	if (gravityConfig==SEDITOR_GRAVITYCONFIG_YNEG)ss<<"Y-NEGATIVE";
	if (gravityConfig==SEDITOR_GRAVITYCONFIG_YPOS)ss<<"Y-POSITIVE";
	if (gravityConfig==SEDITOR_GRAVITYCONFIG_ZNEG)ss<<"Z-NEGATIVE";
	if (gravityConfig==SEDITOR_GRAVITYCONFIG_ZPOS)ss<<"Z-POSITIVE";
	//ss<<"GRAVITY CONFIG: "<<gravityConfig<<"      |      DIRECTION: "<<direction.x<<", "<<direction.y<<", "<<direction.z;
	fontPosition.top = 70;
	S_FONT::defaultFont->DrawTextA(NULL, ss.str().c_str(), -1, &fontPosition, DT_LEFT, 0xffffffff);
	ss.str("");
	ss<<"POS: "<<user_position.x<<", "<<user_position.y<<", "<<user_position.z<<" && DIR: "<<direction.x<<", "<<direction.y<<", "<<direction.z;
	fontPosition.top = 90;
	S_FONT::defaultFont->DrawTextA(NULL, ss.str().c_str(), -1, &fontPosition, DT_LEFT, 0xffffffff);

	ss.str("");
	ss<<"_USER_SHIT_";
	fontPosition.top = 140;
	S_FONT::defaultFont->DrawTextA(NULL, ss.str().c_str(), -1, &fontPosition, DT_LEFT, 0xffffffff);
	ss.str("");
	ss<<"(N) EDITING MODE: ";
	if (gridMode){
		ss<<"GRID EDITING";
	} else {
		ss<<"ROAM EDITING";
	}
	fontPosition.top = 160;
	S_FONT::defaultFont->DrawTextA(NULL, ss.str().c_str(), -1, &fontPosition, DT_LEFT, 0xffffffff);

	ss.str("");
	ss<<"(M) MOVEMENT MODE: ";
	if (roamingMode){
		ss<<"GRAVITY + COLLISION";
	} else {
		ss<<"ZERO GRAVITY + NO COLLISION";
	}
	fontPosition.top = 180;
	S_FONT::defaultFont->DrawTextA(NULL, ss.str().c_str(), -1, &fontPosition, DT_LEFT, 0xffffffff);

	ss.str("");
	ss<<"(B) SAME SIDE PLACEMENT: ";
	if (grid.getSameSideSelection()){
		ss<<"ON";
	} else {
		ss<<"OFF";
	}
	fontPosition.top = 200;
	S_FONT::defaultFont->DrawTextA(NULL, ss.str().c_str(), -1, &fontPosition, DT_LEFT, 0xffffffff);

	ss.str("");
	ss<<"(0-8) Block Type: "<<(int)selectedType;
	fontPosition.top = 220;
	S_FONT::defaultFont->DrawTextA(NULL, ss.str().c_str(), -1, &fontPosition, DT_LEFT, 0xffffffff);

	ss.str("");
	ss<<"(P) Save Raw Voxel Data";
	fontPosition.top = 240;
	S_FONT::defaultFont->DrawTextA(NULL, ss.str().c_str(), -1, &fontPosition, DT_LEFT, 0xffffffff);

	ss.str("");
	ss<<"(V) Grid Axis: ";
	if (grid.getOrientation()==0) ss<<"X";
	if (grid.getOrientation()==1) ss<<"Y";
	if (grid.getOrientation()==2) ss<<"Z";
	fontPosition.top = 260;
	S_FONT::defaultFont->DrawTextA(NULL, ss.str().c_str(), -1, &fontPosition, DT_LEFT, 0xffffffff);

	ss.str("");
	ss<<"(Z) Fill Selection 1: "<<firstClick.x<<", "<<firstClick.y<<", "<<firstClick.z;
	fontPosition.top = 280;
	S_FONT::defaultFont->DrawTextA(NULL, ss.str().c_str(), -1, &fontPosition, DT_LEFT, 0xffffffff);

	ss.str("");
	ss<<"(X) Fill Selection 2: "<<secondClick.x<<", "<<secondClick.y<<", "<<secondClick.z;
	fontPosition.top = 300;
	S_FONT::defaultFont->DrawTextA(NULL, ss.str().c_str(), -1, &fontPosition, DT_LEFT, 0xffffffff);

	ss.str("");
	ss<<"(F) FILL";
	fontPosition.top = 320;
	S_FONT::defaultFont->DrawTextA(NULL, ss.str().c_str(), -1, &fontPosition, DT_LEFT, 0xffffffff);

	ss.str("");
	ss<<"30 / 120";
	fontPosition.top = 950;
	fontPosition.right = 1900;
	S_FONT::ammoFont->DrawTextA(NULL, ss.str().c_str(), -1, &fontPosition, DT_RIGHT, 0x4f99ccff);

	//IF SUPPOSED TO, RENDER LIGHTING INFO
	if (lightingMode){
		renderLightingText();
	}


	//if L is Y is pressed show textures
	if (S_INPUT::keyPressed(DIK_Y))
	{
		texSprite->Begin(0);
		texSprite->Draw(totalMapTextureAtlas, 0, 0, &D3DXVECTOR3(0.0f, 0.0f, 0.0f), 0xffffffff);
		texSprite->End();
	}
	return true;
}

bool SLevelEditor::updateInterface(DWORD lastTimeTaken){
	timeTest = lastTimeTaken;
	if (S_INPUT::keyPressed(DIK_T)){
		resetPosition();
	}

	//ROAM TOGGLING
	static int roamDelay = 0;
	roamDelay++;
	if (S_INPUT::keyPressed(DIK_M) && roamDelay > 30){
		roamDelay = 0;
		roamingMode = !roamingMode;
	}
	//SAME SIDE PLACEMENT TOGGLING
	static int sameSideDelay = 0;
	sameSideDelay++;
	if (S_INPUT::keyPressed(DIK_B) && sameSideDelay > 30){
		sameSideDelay = 0;
		grid.toggleSameSideSelection();
	}

	//GRID ORIENTATION CHANGING
	static int gridOrientationDelay = 0;
	gridOrientationDelay++;
	if (S_INPUT::keyPressed(DIK_V) && gridOrientationDelay > 30){
		gridOrientationDelay = 0;
		if (grid.getOrientation()==0){
			grid.setOrientation(1);
		} else if (grid.getOrientation()==1){
			grid.setOrientation(2);
		} else if (grid.getOrientation()==2){
			grid.setOrientation(0);
		}
		grid.updateGrid();
	}
	
	if (S_INPUT::keyPressed(DIK_0)) selectedType = 0;
	if (S_INPUT::keyPressed(DIK_1)) selectedType = 1;
	if (S_INPUT::keyPressed(DIK_2)) selectedType = 2;
	if (S_INPUT::keyPressed(DIK_3)) selectedType = 3;
	if (S_INPUT::keyPressed(DIK_4)) selectedType = 4;
	if (S_INPUT::keyPressed(DIK_5)) selectedType = 5;
	if (S_INPUT::keyPressed(DIK_6)) selectedType = 6;
	if (S_INPUT::keyPressed(DIK_7)) selectedType = 7;
	if (S_INPUT::keyPressed(DIK_8)) selectedType = 8;
	//update USER ORIENTATION
	updateVoxelCollisionBuffer();
	orientationUpdate();

	
	//GRID CLICK
	
	static int delay = 0;
	delay++;

	bool avoidNormalSelection = false;
	if (S_INPUT::keyPressed(DIK_Z) && S_INPUT::mousePressed(0)){
		avoidNormalSelection = true;
		POINT p;
		GetCursorPos(&p);
		firstClick = grid.getGridSelection(p.x, p.y);
	}
	if (S_INPUT::keyPressed(DIK_X) && S_INPUT::mousePressed(0)){
		avoidNormalSelection = true;
		POINT p;
		GetCursorPos(&p);
		secondClick = grid.getGridSelection(p.x, p.y);
	}
	if (S_INPUT::keyPressed(DIK_F)){
		fillVoxelData();
		updateVoxelGeometry();
	}
	if (!S_INPUT::keyPressed(DIK_Y) && !lightingMode && S_INPUT::mousePressed(0) && !avoidNormalSelection){ //&& delay > 500){
		delay = 0;
		if (gridMode){
			POINT p;
			GetCursorPos(&p);
			SEDITOR_BLOCKSELECTION bs;
			bs = grid.getGridSelection(p.x, p.y);
			if (bs.x > -1){
				voxelData[bs.x][bs.y][bs.z].type = selectedType;
				updateVoxelGeometry();
			}
		}
		
		
		
		
	}
	//grid move
	static int i;
	i++;
	if (S_INPUT::keyPressed(DIK_SPACE) && i > 10){
		if (grid.getOrientation() == 0){
			if (user_position.x > grid.getGridLevel()){
				grid.incrementGridLevel();
			} else {
				grid.decrementGridLevel();
			}
		}
		if (grid.getOrientation() == 1){
			if (user_position.y > grid.getGridLevel()){
				grid.incrementGridLevel();
			} else {
				grid.decrementGridLevel();
			}
		}
		if (grid.getOrientation() == 2){
			if (user_position.z > grid.getGridLevel()){
				grid.incrementGridLevel();
			} else {
				grid.decrementGridLevel();
			}
		}
		i = 0;
	}
	if (S_INPUT::keyPressed(DIK_LSHIFT) && i > 10){
		if (grid.getOrientation() == 0){
			if (user_position.x < grid.getGridLevel()){
				grid.incrementGridLevel();
			} else {
				grid.decrementGridLevel();
			}
		}
		if (grid.getOrientation() == 1){
			if (user_position.y < grid.getGridLevel()){
				grid.incrementGridLevel();
			} else {
				grid.decrementGridLevel();
			}
		}
		if (grid.getOrientation() == 2){
			if (user_position.z < grid.getGridLevel()){
				grid.incrementGridLevel();
			} else {
				grid.decrementGridLevel();
			}
		}
		i = 0;
	}



	if (S_INPUT::keyPressed(DIK_P)){
		saveRawVoxelData();
		saveMap();
	}

	static int gravityDelay = 0;
	gravityDelay++;
	if (S_INPUT::keyPressed(DIK_G) && gravityDelay > 60){
		gravityDelay = 0;
		//FIND WHICH CONFIG TO PUT IT TO
		D3DXVECTOR3 direction;
		D3DXVec3Normalize(&direction, &(user_lookAt - user_position));

		if (abs(direction.x) > abs(direction.y) && abs(direction.x) > abs(direction.z)){
			if (direction.x > 0){
				gravityConfig = SEDITOR_GRAVITYCONFIG_XPOS;
				D3DXVECTOR3 strafe;
				D3DXVECTOR3 dir;
				D3DXVec3Normalize(&dir, &(user_lookAt - user_position));
				D3DXVec3Cross(&strafe, &dir, &D3DXVECTOR3(1.0f, 0.0f, 0.0f));
				D3DXVec3Cross(&user_upVector, &dir, &strafe);
				//user_lookAt = user_position + dir;
				//user_upVector.x = -1.0f; user_upVector.y = 0.0f; user_upVector.z = 0.0f;
				//user_lookAt = user_position + D3DXVECTOR3(0.0f, 1.0f, 0.0f);
				acceleration.x = 0.004f; acceleration.y = 0.0f; acceleration.z = 0.0f;
				xBox = 1.5f; yBox = 0.5f; zBox = 0.5f;
			}
			if (direction.x < 0){
				D3DXVECTOR3 strafe;
				D3DXVECTOR3 dir;
				D3DXVec3Normalize(&dir, &(user_lookAt - user_position));
				D3DXVec3Cross(&strafe, &dir, &D3DXVECTOR3(-1.0f, 0.0f, 0.0f));
				D3DXVec3Cross(&user_upVector, &dir, &strafe);
				//user_lookAt = user_position + dir;
				gravityConfig = SEDITOR_GRAVITYCONFIG_XNEG;
				//user_upVector.x = 1.0f; user_upVector.y = 0.0f; user_upVector.z = 0.0f;
				//user_lookAt = user_position + D3DXVECTOR3(0.0f, -1.0f, 0.0f);
				acceleration.x = -0.004f; acceleration.y = 0.0f; acceleration.z = 0.0f;
				xBox = 1.5f; yBox = 0.5f; zBox = 0.5f;
			}
		}
		if (abs(direction.y) > abs(direction.x) && abs(direction.y) > abs(direction.z)){
			if (direction.y > 0){
				gravityConfig = SEDITOR_GRAVITYCONFIG_YPOS;
				D3DXVECTOR3 strafe;
				D3DXVECTOR3 dir;
				D3DXVec3Normalize(&dir, &(user_lookAt - user_position));
				D3DXVec3Cross(&strafe, &dir, &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
				D3DXVec3Cross(&user_upVector, &dir, &strafe);
				//user_lookAt = user_position + dir;
				//user_upVector.x = 0.0f; user_upVector.y = -1.0f; user_upVector.z = 0.0f;
				//user_lookAt = user_position + D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
				acceleration.x = 0.0f; acceleration.y = 0.004f; acceleration.z = 0.0f;
				xBox = 0.5f; yBox = 1.5f; zBox = 0.5f;
			}
			if (direction.y < 0){
				gravityConfig = SEDITOR_GRAVITYCONFIG_YNEG;
				D3DXVECTOR3 strafe;
				D3DXVECTOR3 dir;
				D3DXVec3Normalize(&dir, &(user_lookAt - user_position));
				D3DXVec3Cross(&strafe, &dir, &D3DXVECTOR3(0.0f, -1.0f, 0.0f));
				D3DXVec3Cross(&user_upVector, &dir, &strafe);
				//user_lookAt = user_position + dir;
				//user_upVector.x = 0.0f; user_upVector.y = 1.0f; user_upVector.z = 0.0f;
				//user_lookAt = user_position + D3DXVECTOR3(1.0f, 0.0f, 0.0f);
				acceleration.x = 0.0f; acceleration.y = -0.00003f; acceleration.z = 0.0f;
				xBox = 0.5f; yBox = 1.5f; zBox = 0.5f;
			}
		}
		if (abs(direction.z) > abs(direction.x) && abs(direction.z) > abs(direction.y)){
			if (direction.z > 0){
				gravityConfig = SEDITOR_GRAVITYCONFIG_ZPOS;
				D3DXVECTOR3 strafe;
				D3DXVECTOR3 dir;
				D3DXVec3Normalize(&dir, &(user_lookAt - user_position));
				D3DXVec3Cross(&strafe, &dir, &D3DXVECTOR3(0.0f, 0.0f, 1.0f));
				D3DXVec3Cross(&user_upVector, &dir, &strafe);
				//user_lookAt = user_position + dir;
				//user_upVector.x = 0.0f; user_upVector.y = 0.0f; user_upVector.z = -1.0f;
				//user_lookAt = user_position + D3DXVECTOR3(1.0f, 0.0f, 0.0f);
				acceleration.x = 0.0f; acceleration.y = 0.0f; acceleration.z = 0.004f;
				xBox = 0.5f; yBox = 0.5f; zBox = 1.5f;
			}
			if (direction.z < 0){
				gravityConfig = SEDITOR_GRAVITYCONFIG_ZNEG;
				D3DXVECTOR3 strafe;
				D3DXVECTOR3 dir;
				D3DXVec3Normalize(&dir, &(user_lookAt - user_position));
				D3DXVec3Cross(&strafe, &dir, &D3DXVECTOR3(0.0f, 0.0f, -1.0f));
				D3DXVec3Cross(&user_upVector, &dir, &strafe);
				//user_lookAt = user_position + dir;
				//user_upVector.x = 0.0f; user_upVector.y = 0.0f; user_upVector.z = 1.0f;
				//user_lookAt = user_position + D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
				acceleration.x = 0.0f; acceleration.y = 0.0f; acceleration.z = -0.004f;
				xBox = 0.5f; yBox = 0.5f; zBox = 1.5f;
			}
		}
	}


	//LIGHTING TOGGLING AND SHIT
	static int lightingDelay = 0;
	lightingDelay++;
	if (S_INPUT::keyPressed(DIK_L) && lightingDelay > 30){
		lightingDelay = 0;
		lightingMode = !lightingMode;
	}
	if (lightingMode && !S_INPUT::keyPressed(DIK_L)){
		if (S_INPUT::keyPressed(DIK_BACKSPACE)){
			if (lightSelection >= 0){
				
				lights[lightSelection].active = false;
				lightSelection = -1;
			}
		}
		static int addLightDelay = 0;
		addLightDelay++;
		if (S_INPUT::keyPressed(DIK_O) && addLightDelay > 50){
			addLightDelay = 0;
			int addedLightIndex = requestAddLight();
			if (addedLightIndex >= 0){
				lightSelection = addedLightIndex;
			}
			
		}
		static int downDelay = 0;
		downDelay++;
		if (S_INPUT::keyPressed(DIK_DOWN) && downDelay > 30){
			downDelay = 0;
			lightModSelection++;
			if (lightModSelection > 9){
				lightModSelection = 9;
			}
		}
		static int upDelay = 0;
		upDelay++;
		if (S_INPUT::keyPressed(DIK_UP) && upDelay > 30){
			upDelay = 0;
			lightModSelection--;
			if (lightModSelection < 0){
				lightModSelection = 0;
			}
		}


		if (S_INPUT::keyPressed(DIK_LEFT)){
			if (lightModSelection == 0) lights[lightSelection].light.Diffuse.r -= 0.01f;
			if (lightModSelection == 1) lights[lightSelection].light.Diffuse.g -= 0.01f;
			if (lightModSelection == 2) lights[lightSelection].light.Diffuse.b -= 0.01f;
			if (lightModSelection == 3) lights[lightSelection].light.Ambient.r -= 0.01f;
			if (lightModSelection == 4) lights[lightSelection].light.Ambient.g -= 0.01f;
			if (lightModSelection == 5) lights[lightSelection].light.Ambient.b -= 0.01f;
			if (lightModSelection == 6) lights[lightSelection].light.Range -= 1.0f;
			if (lightModSelection == 7) lights[lightSelection].light.Attenuation0 -= 0.01f;
			if (lightModSelection == 8) lights[lightSelection].light.Attenuation1 -= 0.01f;
			if (lightModSelection == 9) lights[lightSelection].light.Attenuation2 -= 0.01f;
		}
		if (S_INPUT::keyPressed(DIK_RIGHT)){
			if (lightModSelection == 0) lights[lightSelection].light.Diffuse.r += 0.01f;
			if (lightModSelection == 1) lights[lightSelection].light.Diffuse.g += 0.01f;
			if (lightModSelection == 2) lights[lightSelection].light.Diffuse.b += 0.01f;
			if (lightModSelection == 3) lights[lightSelection].light.Ambient.r += 0.01f;
			if (lightModSelection == 4) lights[lightSelection].light.Ambient.g += 0.01f;
			if (lightModSelection == 5) lights[lightSelection].light.Ambient.b += 0.01f;
			if (lightModSelection == 6) lights[lightSelection].light.Range += 1.0f;
			if (lightModSelection == 7) lights[lightSelection].light.Attenuation0 += 0.01f;
			if (lightModSelection == 8) lights[lightSelection].light.Attenuation1 += 0.01f;
			if (lightModSelection == 9) lights[lightSelection].light.Attenuation2 += 0.01f;
		}

		if (S_INPUT::mousePressed(0)){
			
				int intersectLight = checkRayIntersectLight();
				if (intersectLight >= 0){
					lights[intersectLight].light.Position = getPositionAlongRay(100);
					lightSelection = intersectLight;
					giveToLightBuffer(lightSelection);
				}
		}
	}
	//update all lights and shit
	
	updateLightBuffer();


	//choosing tex and shit
	if (S_INPUT::keyPressed(DIK_Y)){
		if (S_INPUT::mousePressed(0)){
			POINT p;
			GetCursorPos(&p);
			int x, y;
			if (p.x < 128) x = 1;
			if (p.x > 128 && p.x < 256) x = 2;
			if (p.x > 256 && p.x < 384) x = 3;
			if (p.x > 384 && p.x < 512) x = 4;
			if (p.x > 512 && p.x < 640) x = 5;
			if (p.x > 640 && p.x < 768) x = 6;
			if (p.x > 768 && p.x < 896) x = 7;
			if (p.x > 896 && p.x < 1024) x = 8;
			if (p.y < 128) y = 1;
			if (p.y > 128 && p.y < 256) y = 2;
			if (p.y > 256 && p.y < 384) y = 3;
			if (p.y > 384 && p.y < 512) y = 4;
			if (p.y > 512 && p.y < 640) y = 5;
			if (p.y > 640 && p.y < 768) y = 6;
			if (p.y > 768 && p.y < 896) y = 7;
			if (p.y > 896 && p.y < 1024) y = 8;
			x = x - 1;
			y = y - 1;
			
			selectedType = x + (8*y) + 1;
			
		}
	}
	return true;
}

void SLevelEditor::orientationUpdate(void){
	
	D3DXVECTOR3 user_direction;
	D3DXVec3Normalize(&user_direction, &(user_lookAt - user_position));

	float xAmount, yAmount;
	if (S_INPUT::mousePressed(1)){
		xAmount = S_INPUT::getMouseDeltaX()*0.01f;
		yAmount = S_INPUT::getMouseDeltaY()*-0.01f;
	} else {
		xAmount = 0.0f;
		yAmount = 0.0f;
	}

	D3DXVECTOR3 sideAxis;
	D3DXMATRIX matrixRotationAxis, matrixRotationZ;

	//D3DXVec3Normalize(&user_direction, &user_direction);
	D3DXVec3Cross(&sideAxis, &user_direction, &user_upVector);
	D3DXVec3Normalize(&sideAxis, &sideAxis);

	D3DXMatrixRotationAxis(&matrixRotationAxis, &sideAxis, yAmount/5);
	if (gravityConfig == SEDITOR_GRAVITYCONFIG_XNEG){
		D3DXMatrixRotationX(&matrixRotationZ, xAmount/5);
	}
	if (gravityConfig == SEDITOR_GRAVITYCONFIG_XPOS){
		D3DXMatrixRotationX(&matrixRotationZ, -xAmount/5);
	}
	if (gravityConfig == SEDITOR_GRAVITYCONFIG_YNEG){
		D3DXMatrixRotationY(&matrixRotationZ, xAmount/5);
	}
	if (gravityConfig == SEDITOR_GRAVITYCONFIG_YPOS){
		D3DXMatrixRotationY(&matrixRotationZ, -xAmount/5);
	}
	if (gravityConfig == SEDITOR_GRAVITYCONFIG_ZNEG){
		D3DXMatrixRotationZ(&matrixRotationZ, xAmount/5);
	}
	if (gravityConfig == SEDITOR_GRAVITYCONFIG_ZPOS){
		D3DXMatrixRotationZ(&matrixRotationZ, -xAmount/5);
	}
	//D3DXMatrixRotationY(&matrixRotationZ, xAmount/5);

	D3DXVec3TransformCoord(&user_direction, &user_direction, &(matrixRotationAxis*matrixRotationZ));
	D3DXVec3TransformCoord(&user_upVector, &user_upVector, &(matrixRotationAxis*matrixRotationZ));
	//lookAt = direction + position;
	//D3DXVec3Normalize(&user_direction, &user_direction);
	//D3DXVec3Normalize(&user_upVector, &user_upVector);


	//KEYS FOR WORLD/CELL CAMERA MOVEMENT
	D3DXVECTOR3 moveVector = user_direction;
	if (roamingMode){
		if (gravityConfig==SEDITOR_GRAVITYCONFIG_XNEG || gravityConfig==SEDITOR_GRAVITYCONFIG_XPOS){
			moveVector.x = 0.0f;
		}
		if (gravityConfig==SEDITOR_GRAVITYCONFIG_YNEG || gravityConfig==SEDITOR_GRAVITYCONFIG_YPOS){
			moveVector.y = 0.0f;
		}
		if (gravityConfig==SEDITOR_GRAVITYCONFIG_ZNEG || gravityConfig==SEDITOR_GRAVITYCONFIG_ZPOS){
			moveVector.z = 0.0f;
		}
	}
	static float moveVelW = 0.0f;
	if (S_INPUT::keyPressed(DIK_W)){
		if (moveVelW < 0.2f){
			moveVelW = moveVelW + 0.01f;
		}
		
	} else {
		if (moveVelW > 0.0f){
			moveVelW = moveVelW - 0.01f;
		}
	}
	user_position += moveVector*moveVelW;
	
	static float moveVelS = 0.0f;
	if (S_INPUT::keyPressed(DIK_S)){
		if (moveVelS < 0.1f){
			moveVelS = moveVelS + 0.01f;
		}
		
	} else {
		if (moveVelS > 0.0f){
			moveVelS = moveVelS - 0.01f;
		}
	}
	user_position -= moveVector*moveVelS;
	if (S_INPUT::keyPressed(DIK_A)){
		user_position += sideAxis*0.1f;
	}
	if (S_INPUT::keyPressed(DIK_D)){
		user_position -= sideAxis*0.1f;
	}
	user_lookAt = user_direction + user_position;
	//ONLY DO REST IF IN ROAMING
	if (roamingMode){
		//JUMPING
		if (S_INPUT::keyPressed(DIK_LCONTROL) && isGrounded == true){
			isGrounded = false;
			if (gravityConfig==SEDITOR_GRAVITYCONFIG_YNEG){
				velocity.y = 0.02f;
			}
			if (gravityConfig==SEDITOR_GRAVITYCONFIG_YPOS){
				velocity.y = -0.02f;
			}
			if (gravityConfig==SEDITOR_GRAVITYCONFIG_XNEG){
				velocity.x = 0.02f;
			}
			if (gravityConfig==SEDITOR_GRAVITYCONFIG_XPOS){
				velocity.x = -0.02f;
			}
			if (gravityConfig==SEDITOR_GRAVITYCONFIG_ZNEG){
				velocity.z = 0.02f;
			}
			if (gravityConfig==SEDITOR_GRAVITYCONFIG_ZPOS){
				velocity.z = -0.02f;
			}
		}
		//update the lookat vector after position change

		if (!isGrounded){
			velocity += acceleration;
		}
		user_position += velocity;
		user_lookAt += velocity;

		D3DXVECTOR3 projVector = SPhysics::getResponseProjectionVector(&user_position, xBox, yBox, zBox, collisionBuffer, collisionBuffer_solidCount);
		if (gravityConfig==SEDITOR_GRAVITYCONFIG_YNEG){
			if (projVector.y > 0){
				velocity.x = 0.0f; velocity.y = -0.001f; velocity.z = 0.0f;
				isGrounded = true;
			} else {
				isGrounded = false;
			}
			if (projVector.y < 0){
				velocity.y = 0.0f;
			}
		}
		if (gravityConfig==SEDITOR_GRAVITYCONFIG_YPOS){
			if (projVector.y < 0){
				velocity.x = 0.0f; velocity.y = 0.001f; velocity.z = 0.0f;
				isGrounded = true;
			} else {
				isGrounded = false;
			}
			if (projVector.y > 0){
				velocity.y = 0.0f;
			}
		}
		if (gravityConfig==SEDITOR_GRAVITYCONFIG_XNEG){
			if (projVector.x > 0){
				velocity.x = -0.001f; velocity.y = 0.0f; velocity.z = 0.0f;
				isGrounded = true;
			} else {
				isGrounded = false;
			}
			if (projVector.x < 0){
				velocity.x = 0.0f;
			}
		}
		if (gravityConfig==SEDITOR_GRAVITYCONFIG_XPOS){
			if (projVector.x < 0){
				velocity.x = 0.001f; velocity.y = 0.0f; velocity.z = 0.0f;
				isGrounded = true;
			} else {
				isGrounded = false;
			}
			if (projVector.x > 0){
				velocity.x = 0.0f;
			}
		}
		if (gravityConfig==SEDITOR_GRAVITYCONFIG_ZNEG){
			if (projVector.z > 0){
				velocity.x = 0.0f; velocity.y = 0.0f; velocity.z = -0.001f;
				isGrounded = true;
			} else {
				isGrounded = false;
			}
			if (projVector.z < 0){
				velocity.z = 0.0f;
			}
		}
		if (gravityConfig==SEDITOR_GRAVITYCONFIG_ZPOS){
			if (projVector.z < 0){
				velocity.x = 0.0f; velocity.y = 0.0f; velocity.z = 0.001f;
				isGrounded = true;
			} else {
				isGrounded = false;
			}
			if (projVector.z > 0){
				velocity.z = 0.0f;
			}
		}
		user_position += projVector;
		user_lookAt += projVector;
	}
}

bool SLevelEditor::updateVoxelGeometry(void){
	faceCount = 0;
	for (int z = 0; z<MAP_DIMENSION; z++){
		for (int y = 0; y<MAP_DIMENSION; y++){
			for (int x = 0; x<MAP_DIMENSION; x++){
				if (voxelData[x][y][z].type != 0){
					//check for x-
					if (x==0) faceCount++;
					if (x>0){
						if (voxelData[x-1][y][z].type == 0) faceCount++;
					}
					//check for x+
					if (x==(MAP_DIMENSION-1)) faceCount++;
					if (x<(MAP_DIMENSION-1)){
						if (voxelData[x+1][y][z].type == 0) faceCount++;
					}
					//check for y-
					if (y==0) faceCount++;
					if (y>0){
						if (voxelData[x][y-1][z].type == 0) faceCount++;
					}
					//check for y+
					if (y==(MAP_DIMENSION-1)) faceCount++;
					if (y<(MAP_DIMENSION-1)){
						if (voxelData[x][y+1][z].type == 0) faceCount++;
					}
					//check for z-
					if (x==0) faceCount++;
					if (x>0){
						if (voxelData[x][y][z-1].type == 0) faceCount++;
					}
					//check for z+
					if (z==(MAP_DIMENSION-1)) faceCount++;
					if (z<(MAP_DIMENSION-1)){
						if (voxelData[x][y][z+1].type == 0) faceCount++;
					}
				}
			}
		}
	}

	//alloc 24 verts per cube
	SEDITOR_VOXELVERTEX* verts;
	//alloc 36 shorts per cube
	int* indices;
	//index: v for vertex, i for index
	unsigned long cubeIndex_v = 0;
	unsigned long cubeIndex_i = 0;
	
	totalMapVertexBuffer->Lock(0, 0, (void**)&verts, 0);
	totalMapIndexBuffer->Lock(0, 0, (void**)&indices, 0);
	for (int z = 0; z<MAP_DIMENSION; z++){ 
		for (int y = 0; y<MAP_DIMENSION; y++){ 
			for (int x = 0; x<MAP_DIMENSION; x++){
				

				char s = voxelData[x][y][z].type;
				if (s > 0){

					float uStart = 0.0f;
					float vStart = 0.0f;
					uStart = getUStart(s);
					vStart = getVStart(s);
					//BUILD A CUBE

					//--VERTICES--and INDICES
					bool addVertex = false;
					//SIDE Z+
					if (z==(MAP_DIMENSION-1)) addVertex = true;
					if (z<(MAP_DIMENSION-1)){
						if (voxelData[x][y][z+1].type == 0) addVertex = true;
					}
					if (addVertex){
						verts[cubeIndex_v].x = x+0.0f; verts[cubeIndex_v].y = y+0.0f; verts[cubeIndex_v].z = z+1.0f; verts[cubeIndex_v].color = 0xffffffff; verts[cubeIndex_v].u1 = uStart; verts[cubeIndex_v].v1 = vStart+0.125f; verts[cubeIndex_v].normal = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
						verts[cubeIndex_v+1].x = x+1.0f; verts[cubeIndex_v+1].y = y+0.0f; verts[cubeIndex_v+1].z = z+1.0f; verts[cubeIndex_v+1].color = 0xffffffff; verts[cubeIndex_v+1].u1 = uStart + 0.125f; verts[cubeIndex_v+1].v1 = vStart+0.125f; verts[cubeIndex_v+1].normal = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
						verts[cubeIndex_v+2].x = x+1.0f; verts[cubeIndex_v+2].y = y+1.0f; verts[cubeIndex_v+2].z = z+1.0f; verts[cubeIndex_v+2].color = 0xffffffff; verts[cubeIndex_v+2].u1 = uStart + 0.125f; verts[cubeIndex_v+2].v1 = vStart; verts[cubeIndex_v+2].normal = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
						verts[cubeIndex_v+3].x = x+0.0f; verts[cubeIndex_v+3].y = y+1.0f; verts[cubeIndex_v+3].z = z+1.0f; verts[cubeIndex_v+3].color = 0xffffffff; verts[cubeIndex_v+3].u1 = uStart; verts[cubeIndex_v+3].v1 = vStart; verts[cubeIndex_v+3].normal = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
						indices[cubeIndex_i] = cubeIndex_v; indices[cubeIndex_i+1] = cubeIndex_v+1; indices[cubeIndex_i+2] = cubeIndex_v+2;
						indices[cubeIndex_i+3] = cubeIndex_v; indices[cubeIndex_i+4] = cubeIndex_v+2; indices[cubeIndex_i+5] = cubeIndex_v+3;
						cubeIndex_v += 4;
						cubeIndex_i += 6;
						addVertex = false;
					}
					//Side Z-
					if (z==0) addVertex = true;
					if (z>0){
						if (voxelData[x][y][z-1].type == 0) addVertex = true;
					}
					if (addVertex){
						verts[cubeIndex_v].x = x+0.0f; verts[cubeIndex_v].y = y+0.0f; verts[cubeIndex_v].z = z+0.0f; verts[cubeIndex_v].color = 0xffffffff; verts[cubeIndex_v].u1 = uStart; verts[cubeIndex_v].v1 = vStart+0.125f; verts[cubeIndex_v].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
						verts[cubeIndex_v+1].x = x+1.0f; verts[cubeIndex_v+1].y = y+0.0f; verts[cubeIndex_v+1].z = z+0.0f; verts[cubeIndex_v+1].color = 0xffffffff; verts[cubeIndex_v+1].u1 = uStart + 0.125f; verts[cubeIndex_v+1].v1 = vStart+0.125f; verts[cubeIndex_v+1].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
						verts[cubeIndex_v+2].x = x+1.0f; verts[cubeIndex_v+2].y = y+1.0f; verts[cubeIndex_v+2].z = z+0.0f; verts[cubeIndex_v+2].color = 0xffffffff; verts[cubeIndex_v+2].u1 = uStart + 0.125f; verts[cubeIndex_v+2].v1 = vStart; verts[cubeIndex_v+2].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
						verts[cubeIndex_v+3].x = x+0.0f; verts[cubeIndex_v+3].y = y+1.0f; verts[cubeIndex_v+3].z = z+0.0f; verts[cubeIndex_v+3].color = 0xffffffff; verts[cubeIndex_v+3].u1 = uStart; verts[cubeIndex_v+3].v1 = vStart; verts[cubeIndex_v+3].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
						indices[cubeIndex_i] = cubeIndex_v; indices[cubeIndex_i+1] = cubeIndex_v+2; indices[cubeIndex_i+2] = cubeIndex_v+1;
						indices[cubeIndex_i+3] = cubeIndex_v; indices[cubeIndex_i+4] = cubeIndex_v+3; indices[cubeIndex_i+5] = cubeIndex_v+2;
						cubeIndex_v += 4;
						cubeIndex_i += 6;
						addVertex = false;
					}
					//Side X+
					if (x==(MAP_DIMENSION-1)) addVertex = true;
					if (x<(MAP_DIMENSION-1)){
						if (voxelData[x+1][y][z].type == 0) addVertex = true;
					}
					if (addVertex){
						verts[cubeIndex_v].x = x+1.0f; verts[cubeIndex_v].y = y+0.0f; verts[cubeIndex_v].z = z+0.0f; verts[cubeIndex_v].color = 0xffffffff; verts[cubeIndex_v].u1 = uStart; verts[cubeIndex_v].v1 = vStart+0.125f; verts[cubeIndex_v].normal = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
						verts[cubeIndex_v+1].x = x+1.0f; verts[cubeIndex_v+1].y = y+0.0f; verts[cubeIndex_v+1].z = z+1.0f; verts[cubeIndex_v+1].color = 0xffffffff; verts[cubeIndex_v+1].u1 = uStart + 0.125f; verts[cubeIndex_v+1].v1 = vStart+0.125f; verts[cubeIndex_v+1].normal = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
						verts[cubeIndex_v+2].x = x+1.0f; verts[cubeIndex_v+2].y = y+1.0f; verts[cubeIndex_v+2].z = z+1.0f; verts[cubeIndex_v+2].color = 0xffffffff; verts[cubeIndex_v+2].u1 = uStart + 0.125f; verts[cubeIndex_v+2].v1 = vStart; verts[cubeIndex_v+2].normal = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
						verts[cubeIndex_v+3].x = x+1.0f; verts[cubeIndex_v+3].y = y+1.0f; verts[cubeIndex_v+3].z = z+0.0f; verts[cubeIndex_v+3].color = 0xffffffff; verts[cubeIndex_v+3].u1 = uStart; verts[cubeIndex_v+3].v1 = vStart; verts[cubeIndex_v+3].normal = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
						indices[cubeIndex_i] = cubeIndex_v; indices[cubeIndex_i+1] = cubeIndex_v+2; indices[cubeIndex_i+2] = cubeIndex_v+1;
						indices[cubeIndex_i+3] = cubeIndex_v; indices[cubeIndex_i+4] = cubeIndex_v+3; indices[cubeIndex_i+5] = cubeIndex_v+2;
						cubeIndex_v += 4;
						cubeIndex_i += 6;
						addVertex = false;
					}
					//Side X-
					if (x==0) addVertex = true;
					if (x>0){
						if (voxelData[x-1][y][z].type == 0) addVertex = true;
					}
					if (addVertex){
						verts[cubeIndex_v].x = x+0.0f; verts[cubeIndex_v].y = y+0.0f; verts[cubeIndex_v].z = z+0.0f; verts[cubeIndex_v].color = 0xffffffff; verts[cubeIndex_v].u1 = uStart; verts[cubeIndex_v].v1 = vStart+0.125f; verts[cubeIndex_v].normal = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
						verts[cubeIndex_v+1].x = x+0.0f; verts[cubeIndex_v+1].y = y+0.0f; verts[cubeIndex_v+1].z = z+1.0f; verts[cubeIndex_v+1].color = 0xffffffff; verts[cubeIndex_v+1].u1 = uStart + 0.125f; verts[cubeIndex_v+1].v1 = vStart+0.125f; verts[cubeIndex_v+1].normal = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
						verts[cubeIndex_v+2].x = x+0.0f; verts[cubeIndex_v+2].y = y+1.0f; verts[cubeIndex_v+2].z = z+1.0f; verts[cubeIndex_v+2].color = 0xffffffff; verts[cubeIndex_v+2].u1 = uStart + 0.125f; verts[cubeIndex_v+2].v1 = vStart; verts[cubeIndex_v+2].normal = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
						verts[cubeIndex_v+3].x = x+0.0f; verts[cubeIndex_v+3].y = y+1.0f; verts[cubeIndex_v+3].z = z+0.0f; verts[cubeIndex_v+3].color = 0xffffffff; verts[cubeIndex_v+3].u1 = uStart; verts[cubeIndex_v+3].v1 = vStart; verts[cubeIndex_v+3].normal = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
						indices[cubeIndex_i] = cubeIndex_v; indices[cubeIndex_i+1] = cubeIndex_v+1; indices[cubeIndex_i+2] = cubeIndex_v+2;
						indices[cubeIndex_i+3] = cubeIndex_v; indices[cubeIndex_i+4] = cubeIndex_v+2; indices[cubeIndex_i+5] = cubeIndex_v+3;
						cubeIndex_v += 4;
						cubeIndex_i += 6;
						addVertex = false;
					}
					//Side Y+
					if (y==(MAP_DIMENSION-1)) addVertex = true;
					if (y<(MAP_DIMENSION-1)){
						if (voxelData[x][y+1][z].type == 0) addVertex = true;
					}
					if (addVertex){
						verts[cubeIndex_v].x = x+0.0f; verts[cubeIndex_v].y = y+1.0f; verts[cubeIndex_v].z = z+0.0f; verts[cubeIndex_v].color = 0xffffffff; verts[cubeIndex_v].u1 = uStart; verts[cubeIndex_v].v1 = vStart+0.125f; verts[cubeIndex_v].normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
						verts[cubeIndex_v+1].x = x+1.0f; verts[cubeIndex_v+1].y = y+1.0f; verts[cubeIndex_v+1].z = z+0.0f; verts[cubeIndex_v+1].color = 0xffffffff; verts[cubeIndex_v+1].u1 = uStart + 0.125f; verts[cubeIndex_v+1].v1 = vStart+0.125f; verts[cubeIndex_v+1].normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
						verts[cubeIndex_v+2].x = x+1.0f; verts[cubeIndex_v+2].y = y+1.0f; verts[cubeIndex_v+2].z = z+1.0f; verts[cubeIndex_v+2].color = 0xffffffff; verts[cubeIndex_v+2].u1 = uStart + 0.125f; verts[cubeIndex_v+2].v1 = vStart; verts[cubeIndex_v+2].normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
						verts[cubeIndex_v+3].x = x+0.0f; verts[cubeIndex_v+3].y = y+1.0f; verts[cubeIndex_v+3].z = z+1.0f; verts[cubeIndex_v+3].color = 0xffffffff; verts[cubeIndex_v+3].u1 = uStart; verts[cubeIndex_v+3].v1 = vStart; verts[cubeIndex_v+3].normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
						indices[cubeIndex_i] = cubeIndex_v; indices[cubeIndex_i+1] = cubeIndex_v+2; indices[cubeIndex_i+2] = cubeIndex_v+1;
						indices[cubeIndex_i+3] = cubeIndex_v; indices[cubeIndex_i+4] = cubeIndex_v+3; indices[cubeIndex_i+5] = cubeIndex_v+2;
						cubeIndex_v += 4;
						cubeIndex_i += 6;
						addVertex = false;
					}
					//Side Y-
					if (y==0) addVertex = true;
					if (y>0){
						if (voxelData[x][y-1][z].type == 0) addVertex = true;
					}
					if (addVertex){
						verts[cubeIndex_v].x = x+0.0f; verts[cubeIndex_v].y = y+0.0f; verts[cubeIndex_v].z = z+0.0f; verts[cubeIndex_v].color = 0xffffffff; verts[cubeIndex_v].u1 = uStart; verts[cubeIndex_v].v1 = vStart+0.125f; verts[cubeIndex_v].normal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
						verts[cubeIndex_v+1].x = x+1.0f; verts[cubeIndex_v+1].y = y+0.0f; verts[cubeIndex_v+1].z = z+0.0f; verts[cubeIndex_v+1].color = 0xffffffff; verts[cubeIndex_v+1].u1 = uStart + 0.125f; verts[cubeIndex_v+1].v1 = vStart+0.125f; verts[cubeIndex_v+1].normal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
						verts[cubeIndex_v+2].x = x+1.0f; verts[cubeIndex_v+2].y = y+0.0f; verts[cubeIndex_v+2].z = z+1.0f; verts[cubeIndex_v+2].color = 0xffffffff; verts[cubeIndex_v+2].u1 = uStart + 0.125f; verts[cubeIndex_v+2].v1 = vStart; verts[cubeIndex_v+2].normal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
						verts[cubeIndex_v+3].x = x+0.0f; verts[cubeIndex_v+3].y = y+0.0f; verts[cubeIndex_v+3].z = z+1.0f; verts[cubeIndex_v+3].color = 0xffffffff; verts[cubeIndex_v+3].u1 = uStart; verts[cubeIndex_v+3].v1 = vStart; verts[cubeIndex_v+3].normal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
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
	totalMapVertexBuffer->Unlock();
	totalMapIndexBuffer->Unlock();

	return true;
}




void SLevelEditor::fillVoxelData(void){
	if (firstClick.x < secondClick.x || firstClick.x == secondClick.x){
		for (int x = firstClick.x; x < secondClick.x + 1; x++){
			if (firstClick.y < secondClick.y || firstClick.y == secondClick.y){
				for (int y = firstClick.y; y < secondClick.y + 1; y++){
					if (firstClick.z < secondClick.z || firstClick.z == secondClick.z){
						for (int z = firstClick.z; z < secondClick.z + 1; z++){
							voxelData[x][y][z].type = selectedType;
						}
					} else if (firstClick.z > secondClick.z){
						for (int z = secondClick.z; z < firstClick.z + 1; z++){
							voxelData[x][y][z].type = selectedType;
						}
					}
				}
			} else if (firstClick.y > secondClick.y){
				for (int y = secondClick.y; y < firstClick.y + 1; y++){
					if (firstClick.z < secondClick.z || firstClick.z == secondClick.z){
						for (int z = firstClick.z; z < secondClick.z + 1; z++){
							voxelData[x][y][z].type = selectedType;
						}
					} else if (firstClick.z > secondClick.z){
						for (int z = secondClick.z; z < firstClick.z + 1; z++){
							voxelData[x][y][z].type = selectedType;
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
							voxelData[x][y][z].type = selectedType;
						}
					} else if (firstClick.z > secondClick.z){
						for (int z = secondClick.z; z < firstClick.z + 1; z++){
							voxelData[x][y][z].type = selectedType;
						}
					}
				}
			} else if (firstClick.y > secondClick.y){
				for (int y = secondClick.y; y < firstClick.y + 1; y++){
					if (firstClick.z < secondClick.z || firstClick.z == secondClick.z){
						for (int z = firstClick.z; z < secondClick.z + 1; z++){
							voxelData[x][y][z].type = selectedType;
						}
					} else if (firstClick.z > secondClick.z){
						for (int z = secondClick.z; z < firstClick.z + 1; z++){
							voxelData[x][y][z].type = selectedType;
						}
					}
				}
			}
		}
	} 
}

void SLevelEditor::saveRawVoxelData(void){
	std::ofstream file;
	file.open("../Resources/Levels/Allocation/allocationLastSaved.psm", std::ios::binary);
	for (int z = 0; z<MAP_DIMENSION; z++){
		for (int y = 0; y<MAP_DIMENSION; y++){
			for (int x = 0; x<MAP_DIMENSION; x++){
				file.put(voxelData[x][y][z].type);
			}
		}
	}
	file.close();
}
void SLevelEditor::saveMap(void){

	std::ofstream file;
	file.open("../Resources/Levels/Allocation/allocationLastSaved.psm", std::ios::binary);
	//RAW VOXEL DATA
	for (int z = 0; z<MAP_DIMENSION; z++){
		for (int y = 0; y<MAP_DIMENSION; y++){
			for (int x = 0; x<MAP_DIMENSION; x++){
				file.put(voxelData[x][y][z].type);
			}
		}
	}

	//LIGHTING DATA
	int activeLights = getNumActiveLights();
	file.write(reinterpret_cast<const char*>(&activeLights), sizeof(int));
	for (int i = 0; i < SLEVELEDITOR_MAXLIGHTS; i++){
		if (lights[i].active){
			//write it to file - should happen activeLights times...

			//POSITION
			D3DXVECTOR3 pos = lights[i].light.Position;
			file.write(reinterpret_cast<const char*>(&pos.x), sizeof(float));
			file.write(reinterpret_cast<const char*>(&pos.y), sizeof(float));
			file.write(reinterpret_cast<const char*>(&pos.z), sizeof(float));

			//DIFFUSE
			float dr = lights[i].light.Diffuse.r;
			float dg = lights[i].light.Diffuse.g;
			float db = lights[i].light.Diffuse.b;
			file.write(reinterpret_cast<const char*>(&dr), sizeof(float));
			file.write(reinterpret_cast<const char*>(&dg), sizeof(float));
			file.write(reinterpret_cast<const char*>(&db), sizeof(float));

			//Ambient
			float ar = lights[i].light.Ambient.r;
			float ag = lights[i].light.Ambient.g;
			float ab = lights[i].light.Ambient.b;
			file.write(reinterpret_cast<const char*>(&ar), sizeof(float));
			file.write(reinterpret_cast<const char*>(&ag), sizeof(float));
			file.write(reinterpret_cast<const char*>(&ab), sizeof(float));

			//RANGE
			float r = lights[i].light.Range;
			file.write(reinterpret_cast<const char*>(&r), sizeof(float));

			//Attenuations - c, x, and x^2
			float a0 = lights[i].light.Attenuation0;
			float a1 = lights[i].light.Attenuation1;
			float a2 = lights[i].light.Attenuation2;
			file.write(reinterpret_cast<const char*>(&a0), sizeof(float));
			file.write(reinterpret_cast<const char*>(&a1), sizeof(float));
			file.write(reinterpret_cast<const char*>(&a2), sizeof(float));
		}
	}
	file.close();

}
void SLevelEditor::updateVoxelCollisionBuffer(void){
	collisionBuffer_solidCount = 0;
	int colIndex = 0;
	//gets confusing because over truncation in floats less than 1, therefore even if
	//number is 0.69, we dont want any negative truncation close to 0.
	if (user_position.x >= 0.0f && user_position.y >= 0.0f && user_position.z >= 0.0f){
		for (int x = (int)(user_position.x); x<=(int)(user_position.x + 2.0f); x++){
			for (int y = (int)(user_position.y); y<=(int)(user_position.y + 2.0f); y++){
				for (int z = (int)(user_position.z); z<=(int)(user_position.z + 2.0f);z++){
					
					collisionBuffer[colIndex].x = 0.0f; collisionBuffer[colIndex].y = 0.0f; collisionBuffer[colIndex].z = 0.0f;
					if (x>=0&&x<=64&&y>=0&&y<=64&&z>=0&&z<=64){
						if (voxelData[x-1][y-1][z-1].type != 0){
							collisionBuffer_solidCount++;
							collisionBuffer[colIndex].x = x-1.0f;
							collisionBuffer[colIndex].y = y-1.0f;
							collisionBuffer[colIndex].z = z-1.0f;
							colIndex++;
						}
					}
					
				}
			}
		}
	}
}


void SLevelEditor::resetPosition(void){
	user_position.x = 32.0f; user_position.y = 32.0f; user_position.z = 32.0f;
	user_upVector.x = 0.0f; user_upVector.y = 1.0f; user_upVector.z = 0.0f;
	user_lookAt.x = 0.0f; user_lookAt.y = 32.0f; user_lookAt.z = 0.0f;

	gravityConfig = SEDITOR_GRAVITYCONFIG_YNEG;
	user_upVector.x = 0.0f; user_upVector.y = 1.0f; user_upVector.z = 0.0f;
	user_lookAt = user_position + D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	acceleration.x = 0.0f; acceleration.y = -0.00001f; acceleration.z = 0.0f;
	velocity.x = 0.0f; velocity.y = -0.001f; velocity.z = 0.0f;
	xBox = 0.5f; yBox = 1.5f; zBox = 0.5f;

}

bool SLevelEditor::loadVoxelGeometryProto(void){
	totalMapVertexBuffer = NULL;
	totalMapIndexBuffer = NULL;

	faceCount = 0;
	for (int x = 0; x < 8; x++){
		perTextureFaceCount[x] = 0;
	}
	for (int z = 0; z<MAP_DIMENSION; z++){
		for (int y = 0; y<MAP_DIMENSION; y++){
			for (int x = 0; x<MAP_DIMENSION; x++){
				if (voxelData[x][y][z].type != 0){
					//check for x-
					if (x==0) {
						faceCount++;
						perTextureFaceCount[voxelData[x][y][z].type - 1]++;
					}
					if (x>0){
						if (voxelData[x-1][y][z].type == 0){
							faceCount++;
							perTextureFaceCount[voxelData[x][y][z].type - 1]++;
						}
					}
					//check for x+
					if (x==(MAP_DIMENSION-1)){
						faceCount++;
						perTextureFaceCount[voxelData[x][y][z].type - 1]++;
					}
					if (x<(MAP_DIMENSION-1)){
						if (voxelData[x+1][y][z].type == 0){
							faceCount++;
							perTextureFaceCount[voxelData[x][y][z].type - 1]++;
						}
					}
					//check for y-
					if (y==0){
						faceCount++;
						perTextureFaceCount[voxelData[x][y][z].type - 1]++;
					}
					if (y>0){
						if (voxelData[x][y-1][z].type == 0){
							faceCount++;
							perTextureFaceCount[voxelData[x][y][z].type - 1]++;
						}
					}
					//check for y+
					if (y==(MAP_DIMENSION-1)){
						faceCount++;
						perTextureFaceCount[voxelData[x][y][z].type - 1]++;
					}
					if (y<(MAP_DIMENSION-1)){
						if (voxelData[x][y+1][z].type == 0){
							faceCount++;
							perTextureFaceCount[voxelData[x][y][z].type - 1]++;
						}
					}
					//check for z-
					if (x==0){
						faceCount++;
						perTextureFaceCount[voxelData[x][y][z].type - 1]++;
					}
					if (x>0){
						if (voxelData[x][y][z-1].type == 0){
							faceCount++;
							perTextureFaceCount[voxelData[x][y][z].type - 1]++;
						}
					}
					//check for z+
					if (z==(MAP_DIMENSION-1)){
						faceCount++;
						perTextureFaceCount[voxelData[x][y][z].type - 1]++;
					}
					if (z<(MAP_DIMENSION-1)){
						if (voxelData[x][y][z+1].type == 0){
							faceCount++;
							perTextureFaceCount[voxelData[x][y][z].type - 1]++;
						}
					}
				}
			}
		}
	}

	//alloc 24 verts per cube
	SEDITOR_VOXELVERTEX* verts = new SEDITOR_VOXELVERTEX[32*32*32*24];
	//alloc 36 shorts per cube
	int* indices = new int[32*32*32*36];
	//index: v for vertex, i for index
	unsigned long cubeIndex_v = 0;
	unsigned long cubeIndex_i = 0;
	
	S_GUI::getDevice()->CreateVertexBuffer(sizeof(SEDITOR_VOXELVERTEX)*32*32*32*24,
			0,
			SEDITOR_VOXELFVF,
			D3DPOOL_MANAGED,
			&totalMapVertexBuffer,
			NULL);
	S_GUI::getDevice()->CreateIndexBuffer(sizeof(int)*32*32*32*36,
			0,
			D3DFMT_INDEX32,
			D3DPOOL_MANAGED,
			&totalMapIndexBuffer,
			NULL);
	for (int type = 1; type < 9; type++){
		totalMapVertexBuffer->Lock(0, 0, (void**)&verts, 0);
		totalMapIndexBuffer->Lock(0, 0, (void**)&indices, 0);
		for (int z = 0; z<MAP_DIMENSION; z++){ 
			for (int y = 0; y<MAP_DIMENSION; y++){ 
				for (int x = 0; x<MAP_DIMENSION; x++){


					char s = voxelData[x][y][z].type;
					if (s > 0){

						if (type == voxelData[x][y][z].type){
							//BUILD A CUBE

							//--VERTICES--and INDICES
							bool addVertex = false;
							//SIDE Z+
							if (z==(MAP_DIMENSION-1)) addVertex = true;
							if (z<(MAP_DIMENSION-1)){
								if (voxelData[x][y][z+1].type == 0) addVertex = true;
							}
							if (addVertex){
								verts[cubeIndex_v].x = x+0.0f; verts[cubeIndex_v].y = y+0.0f; verts[cubeIndex_v].z = z+1.0f; verts[cubeIndex_v].color = 0xffffffff; verts[cubeIndex_v].u1 = 0.0f; verts[cubeIndex_v].v1 = 1.0f; verts[cubeIndex_v].normal = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
								verts[cubeIndex_v+1].x = x+1.0f; verts[cubeIndex_v+1].y = y+0.0f; verts[cubeIndex_v+1].z = z+1.0f; verts[cubeIndex_v+1].color = 0xffffffff; verts[cubeIndex_v+1].u1 = 1.0f; verts[cubeIndex_v+1].v1 = 1.0f; verts[cubeIndex_v+1].normal = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
								verts[cubeIndex_v+2].x = x+1.0f; verts[cubeIndex_v+2].y = y+1.0f; verts[cubeIndex_v+2].z = z+1.0f; verts[cubeIndex_v+2].color = 0xffffffff; verts[cubeIndex_v+2].u1 = 1.0f; verts[cubeIndex_v+2].v1 = 0.0f; verts[cubeIndex_v+2].normal = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
								verts[cubeIndex_v+3].x = x+0.0f; verts[cubeIndex_v+3].y = y+1.0f; verts[cubeIndex_v+3].z = z+1.0f; verts[cubeIndex_v+3].color = 0xffffffff; verts[cubeIndex_v+3].u1 = 0.0f; verts[cubeIndex_v+3].v1 = 0.0f; verts[cubeIndex_v+3].normal = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
								indices[cubeIndex_i] = cubeIndex_v; indices[cubeIndex_i+1] = cubeIndex_v+1; indices[cubeIndex_i+2] = cubeIndex_v+2;
								indices[cubeIndex_i+3] = cubeIndex_v; indices[cubeIndex_i+4] = cubeIndex_v+2; indices[cubeIndex_i+5] = cubeIndex_v+3;
								cubeIndex_v += 4;
								cubeIndex_i += 6;
								addVertex = false;
							}
							//Side Z-
							if (z==0) addVertex = true;
							if (z>0){
								if (voxelData[x][y][z-1].type == 0) addVertex = true;
							}
							if (addVertex){
								verts[cubeIndex_v].x = x+0.0f; verts[cubeIndex_v].y = y+0.0f; verts[cubeIndex_v].z = z+0.0f; verts[cubeIndex_v].color = 0xffffffff; verts[cubeIndex_v].u1 = 0.0f; verts[cubeIndex_v].v1 = 1.0f; verts[cubeIndex_v].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
								verts[cubeIndex_v+1].x = x+1.0f; verts[cubeIndex_v+1].y = y+0.0f; verts[cubeIndex_v+1].z = z+0.0f; verts[cubeIndex_v+1].color = 0xffffffff; verts[cubeIndex_v+1].u1 = 1.0f; verts[cubeIndex_v+1].v1 = 1.0f; verts[cubeIndex_v+1].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
								verts[cubeIndex_v+2].x = x+1.0f; verts[cubeIndex_v+2].y = y+1.0f; verts[cubeIndex_v+2].z = z+0.0f; verts[cubeIndex_v+2].color = 0xffffffff; verts[cubeIndex_v+2].u1 = 1.0f; verts[cubeIndex_v+2].v1 = 0.0f; verts[cubeIndex_v+2].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
								verts[cubeIndex_v+3].x = x+0.0f; verts[cubeIndex_v+3].y = y+1.0f; verts[cubeIndex_v+3].z = z+0.0f; verts[cubeIndex_v+3].color = 0xffffffff; verts[cubeIndex_v+3].u1 = 0.0f; verts[cubeIndex_v+3].v1 = 0.0f; verts[cubeIndex_v+3].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
								indices[cubeIndex_i] = cubeIndex_v; indices[cubeIndex_i+1] = cubeIndex_v+2; indices[cubeIndex_i+2] = cubeIndex_v+1;
								indices[cubeIndex_i+3] = cubeIndex_v; indices[cubeIndex_i+4] = cubeIndex_v+3; indices[cubeIndex_i+5] = cubeIndex_v+2;
								cubeIndex_v += 4;
								cubeIndex_i += 6;
								addVertex = false;
							}
							//Side X+
							if (x==(MAP_DIMENSION-1)) addVertex = true;
							if (x<(MAP_DIMENSION-1)){
								if (voxelData[x+1][y][z].type == 0) addVertex = true;
							}
							if (addVertex){
								verts[cubeIndex_v].x = x+1.0f; verts[cubeIndex_v].y = y+0.0f; verts[cubeIndex_v].z = z+0.0f; verts[cubeIndex_v].color = 0xffffffff; verts[cubeIndex_v].u1 = 0.0f; verts[cubeIndex_v].v1 = 1.0f; verts[cubeIndex_v].normal = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
								verts[cubeIndex_v+1].x = x+1.0f; verts[cubeIndex_v+1].y = y+0.0f; verts[cubeIndex_v+1].z = z+1.0f; verts[cubeIndex_v+1].color = 0xffffffff; verts[cubeIndex_v+1].u1 = 1.0f; verts[cubeIndex_v+1].v1 = 1.0f; verts[cubeIndex_v+1].normal = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
								verts[cubeIndex_v+2].x = x+1.0f; verts[cubeIndex_v+2].y = y+1.0f; verts[cubeIndex_v+2].z = z+1.0f; verts[cubeIndex_v+2].color = 0xffffffff; verts[cubeIndex_v+2].u1 = 1.0f; verts[cubeIndex_v+2].v1 = 0.0f; verts[cubeIndex_v+2].normal = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
								verts[cubeIndex_v+3].x = x+1.0f; verts[cubeIndex_v+3].y = y+1.0f; verts[cubeIndex_v+3].z = z+0.0f; verts[cubeIndex_v+3].color = 0xffffffff; verts[cubeIndex_v+3].u1 = 0.0f; verts[cubeIndex_v+3].v1 = 0.0f; verts[cubeIndex_v+3].normal = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
								indices[cubeIndex_i] = cubeIndex_v; indices[cubeIndex_i+1] = cubeIndex_v+2; indices[cubeIndex_i+2] = cubeIndex_v+1;
								indices[cubeIndex_i+3] = cubeIndex_v; indices[cubeIndex_i+4] = cubeIndex_v+3; indices[cubeIndex_i+5] = cubeIndex_v+2;
								cubeIndex_v += 4;
								cubeIndex_i += 6;
								addVertex = false;
							}
							//Side X-
							if (x==0) addVertex = true;
							if (x>0){
								if (voxelData[x-1][y][z].type == 0) addVertex = true;
							}
							if (addVertex){
								verts[cubeIndex_v].x = x+0.0f; verts[cubeIndex_v].y = y+0.0f; verts[cubeIndex_v].z = z+0.0f; verts[cubeIndex_v].color = 0xffffffff; verts[cubeIndex_v].u1 = 0.0f; verts[cubeIndex_v].v1 = 1.0f; verts[cubeIndex_v].normal = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
								verts[cubeIndex_v+1].x = x+0.0f; verts[cubeIndex_v+1].y = y+0.0f; verts[cubeIndex_v+1].z = z+1.0f; verts[cubeIndex_v+1].color = 0xffffffff; verts[cubeIndex_v+1].u1 = 1.0f; verts[cubeIndex_v+1].v1 = 1.0f; verts[cubeIndex_v+1].normal = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
								verts[cubeIndex_v+2].x = x+0.0f; verts[cubeIndex_v+2].y = y+1.0f; verts[cubeIndex_v+2].z = z+1.0f; verts[cubeIndex_v+2].color = 0xffffffff; verts[cubeIndex_v+2].u1 = 1.0f; verts[cubeIndex_v+2].v1 = 0.0f; verts[cubeIndex_v+2].normal = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
								verts[cubeIndex_v+3].x = x+0.0f; verts[cubeIndex_v+3].y = y+1.0f; verts[cubeIndex_v+3].z = z+0.0f; verts[cubeIndex_v+3].color = 0xffffffff; verts[cubeIndex_v+3].u1 = 0.0f; verts[cubeIndex_v+3].v1 = 0.0f; verts[cubeIndex_v+3].normal = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
								indices[cubeIndex_i] = cubeIndex_v; indices[cubeIndex_i+1] = cubeIndex_v+1; indices[cubeIndex_i+2] = cubeIndex_v+2;
								indices[cubeIndex_i+3] = cubeIndex_v; indices[cubeIndex_i+4] = cubeIndex_v+2; indices[cubeIndex_i+5] = cubeIndex_v+3;
								cubeIndex_v += 4;
								cubeIndex_i += 6;
								addVertex = false;
							}
							//Side Y+
							if (y==(MAP_DIMENSION-1)) addVertex = true;
							if (y<(MAP_DIMENSION-1)){
								if (voxelData[x][y+1][z].type == 0) addVertex = true;
							}
							if (addVertex){
								verts[cubeIndex_v].x = x+0.0f; verts[cubeIndex_v].y = y+1.0f; verts[cubeIndex_v].z = z+0.0f; verts[cubeIndex_v].color = 0xffffffff; verts[cubeIndex_v].u1 = 0.0f; verts[cubeIndex_v].v1 = 1.0f; verts[cubeIndex_v].normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
								verts[cubeIndex_v+1].x = x+1.0f; verts[cubeIndex_v+1].y = y+1.0f; verts[cubeIndex_v+1].z = z+0.0f; verts[cubeIndex_v+1].color = 0xffffffff; verts[cubeIndex_v+1].u1 = 1.0f; verts[cubeIndex_v+1].v1 = 1.0f; verts[cubeIndex_v+1].normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
								verts[cubeIndex_v+2].x = x+1.0f; verts[cubeIndex_v+2].y = y+1.0f; verts[cubeIndex_v+2].z = z+1.0f; verts[cubeIndex_v+2].color = 0xffffffff; verts[cubeIndex_v+2].u1 = 1.0f; verts[cubeIndex_v+2].v1 = 0.0f; verts[cubeIndex_v+2].normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
								verts[cubeIndex_v+3].x = x+0.0f; verts[cubeIndex_v+3].y = y+1.0f; verts[cubeIndex_v+3].z = z+1.0f; verts[cubeIndex_v+3].color = 0xffffffff; verts[cubeIndex_v+3].u1 = 0.0f; verts[cubeIndex_v+3].v1 = 0.0f; verts[cubeIndex_v+3].normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
								indices[cubeIndex_i] = cubeIndex_v; indices[cubeIndex_i+1] = cubeIndex_v+2; indices[cubeIndex_i+2] = cubeIndex_v+1;
								indices[cubeIndex_i+3] = cubeIndex_v; indices[cubeIndex_i+4] = cubeIndex_v+3; indices[cubeIndex_i+5] = cubeIndex_v+2;
								cubeIndex_v += 4;
								cubeIndex_i += 6;
								addVertex = false;
							}
							//Side Y-
							if (y==0) addVertex = true;
							if (y>0){
								if (voxelData[x][y-1][z].type == 0) addVertex = true;
							}
							if (addVertex){
								verts[cubeIndex_v].x = x+0.0f; verts[cubeIndex_v].y = y+0.0f; verts[cubeIndex_v].z = z+0.0f; verts[cubeIndex_v].color = 0xffffffff; verts[cubeIndex_v].u1 = 0.0f; verts[cubeIndex_v].v1 = 1.0f; verts[cubeIndex_v].normal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
								verts[cubeIndex_v+1].x = x+1.0f; verts[cubeIndex_v+1].y = y+0.0f; verts[cubeIndex_v+1].z = z+0.0f; verts[cubeIndex_v+1].color = 0xffffffff; verts[cubeIndex_v+1].u1 = 1.0f; verts[cubeIndex_v+1].v1 = 1.0f; verts[cubeIndex_v+1].normal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
								verts[cubeIndex_v+2].x = x+1.0f; verts[cubeIndex_v+2].y = y+0.0f; verts[cubeIndex_v+2].z = z+1.0f; verts[cubeIndex_v+2].color = 0xffffffff; verts[cubeIndex_v+2].u1 = 1.0f; verts[cubeIndex_v+2].v1 = 0.0f; verts[cubeIndex_v+2].normal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
								verts[cubeIndex_v+3].x = x+0.0f; verts[cubeIndex_v+3].y = y+0.0f; verts[cubeIndex_v+3].z = z+1.0f; verts[cubeIndex_v+3].color = 0xffffffff; verts[cubeIndex_v+3].u1 = 0.0f; verts[cubeIndex_v+3].v1 = 0.0f; verts[cubeIndex_v+3].normal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
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
		}
	}
	totalMapVertexBuffer->Unlock();
	totalMapIndexBuffer->Unlock();

	return true;
}

bool SLevelEditor::loadTexturesProto(void){
	//LOAD TEXTURE ATLAS
	//std::ostringstream stream;
	
	D3DXCreateTextureFromFile(S_GUI::getDevice(), L"blocktexture0.dds", &blockTextures[0]);
	D3DXCreateTextureFromFile(S_GUI::getDevice(), L"blocktexture1.dds", &blockTextures[1]);
	D3DXCreateTextureFromFile(S_GUI::getDevice(), L"blocktexture2.dds", &blockTextures[2]);
	D3DXCreateTextureFromFile(S_GUI::getDevice(), L"blocktexture3.dds", &blockTextures[3]);
	D3DXCreateTextureFromFile(S_GUI::getDevice(), L"blocktexture4.dds", &blockTextures[4]);
	D3DXCreateTextureFromFile(S_GUI::getDevice(), L"blocktexture5.dds", &blockTextures[5]);
	D3DXCreateTextureFromFile(S_GUI::getDevice(), L"blocktexture0.dds", &blockTextures[6]);
	D3DXCreateTextureFromFile(S_GUI::getDevice(), L"blocktexture0.dds", &blockTextures[7]);
	//D3DXCreateTextureFromFileEx(S_GUI::getDevice(), textureAtlasPath.c_str(), 1024, 1024, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0xff000000, 0, 0, &totalMapTextureAtlas);
	return true;
}

bool SLevelEditor::updateVoxelGeometryProto(void){
	faceCount = 0;
	for (int i = 0; i < 8; i++){
		perTextureFaceCount[i] = 0;
	}
	for (int z = 0; z<MAP_DIMENSION; z++){
		for (int y = 0; y<MAP_DIMENSION; y++){
			for (int x = 0; x<MAP_DIMENSION; x++){
				if (voxelData[x][y][z].type != 0){
					//check for x-
					if (x==0){
						faceCount++;
						perTextureFaceCount[voxelData[x][y][z].type -1]++;
					}
					if (x>0){
						if (voxelData[x-1][y][z].type == 0){
							faceCount++;
							perTextureFaceCount[voxelData[x][y][z].type -1]++;
						}
					}
					//check for x+
					if (x==(MAP_DIMENSION-1)){
						faceCount++;
						perTextureFaceCount[voxelData[x][y][z].type -1]++;
					}
					if (x<(MAP_DIMENSION-1)){
						if (voxelData[x+1][y][z].type == 0){
							faceCount++;
							perTextureFaceCount[voxelData[x][y][z].type -1]++;
						}
					}
					//check for y-
					if (y==0){
						faceCount++;
						perTextureFaceCount[voxelData[x][y][z].type -1]++;
					}
					if (y>0){
						if (voxelData[x][y-1][z].type == 0){
							faceCount++;
							perTextureFaceCount[voxelData[x][y][z].type -1]++;
						}
					}
					//check for y+
					if (y==(MAP_DIMENSION-1)){
						faceCount++;
						perTextureFaceCount[voxelData[x][y][z].type -1]++;
					}
					if (y<(MAP_DIMENSION-1)){
						if (voxelData[x][y+1][z].type == 0){
							faceCount++;
							perTextureFaceCount[voxelData[x][y][z].type -1]++;
						}
					}
					//check for z-
					if (x==0){
						faceCount++;
						perTextureFaceCount[voxelData[x][y][z].type -1]++;
					}
					if (x>0){
						if (voxelData[x][y][z-1].type == 0){
							faceCount++;
							perTextureFaceCount[voxelData[x][y][z].type -1]++;
						}
					}
					//check for z+
					if (z==(MAP_DIMENSION-1)){
						faceCount++;
						perTextureFaceCount[voxelData[x][y][z].type -1]++;
					}
					if (z<(MAP_DIMENSION-1)){
						if (voxelData[x][y][z+1].type == 0){
							faceCount++;
							perTextureFaceCount[voxelData[x][y][z].type -1]++;
						}
					}
				}
			}
		}
	}

	//alloc 24 verts per cube
	SEDITOR_VOXELVERTEX* verts;
	//alloc 36 shorts per cube
	int* indices;
	//index: v for vertex, i for index
	unsigned long cubeIndex_v = 0;
	unsigned long cubeIndex_i = 0;
	
	totalMapVertexBuffer->Lock(0, 0, (void**)&verts, 0);
	totalMapIndexBuffer->Lock(0, 0, (void**)&indices, 0);
	for (int type = 1; type < 65; type++){
		for (int z = 0; z<MAP_DIMENSION; z++){ 
			for (int y = 0; y<MAP_DIMENSION; y++){ 
				for (int x = 0; x<MAP_DIMENSION; x++){


					char s = voxelData[x][y][z].type;
					if (s > 0){
						if (type == voxelData[x][y][z].type){
							//BUILD A CUBE

							//--VERTICES--and INDICES
							bool addVertex = false;
							//SIDE Z+
							if (z==(MAP_DIMENSION-1)) addVertex = true;
							if (z<(MAP_DIMENSION-1)){
								if (voxelData[x][y][z+1].type == 0) addVertex = true;
							}
							if (addVertex){
								verts[cubeIndex_v].x = x+0.0f; verts[cubeIndex_v].y = y+0.0f; verts[cubeIndex_v].z = z+1.0f; verts[cubeIndex_v].color = 0xffffffff; verts[cubeIndex_v].u1 = 0.0f; verts[cubeIndex_v].v1 = 1.0f; verts[cubeIndex_v].normal = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
								verts[cubeIndex_v+1].x = x+1.0f; verts[cubeIndex_v+1].y = y+0.0f; verts[cubeIndex_v+1].z = z+1.0f; verts[cubeIndex_v+1].color = 0xffffffff; verts[cubeIndex_v+1].u1 = 1.0f; verts[cubeIndex_v+1].v1 = 1.0f; verts[cubeIndex_v+1].normal = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
								verts[cubeIndex_v+2].x = x+1.0f; verts[cubeIndex_v+2].y = y+1.0f; verts[cubeIndex_v+2].z = z+1.0f; verts[cubeIndex_v+2].color = 0xffffffff; verts[cubeIndex_v+2].u1 = 1.0f; verts[cubeIndex_v+2].v1 = 0.0f; verts[cubeIndex_v+2].normal = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
								verts[cubeIndex_v+3].x = x+0.0f; verts[cubeIndex_v+3].y = y+1.0f; verts[cubeIndex_v+3].z = z+1.0f; verts[cubeIndex_v+3].color = 0xffffffff; verts[cubeIndex_v+3].u1 = 0.0f; verts[cubeIndex_v+3].v1 = 0.0f; verts[cubeIndex_v+3].normal = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
								indices[cubeIndex_i] = cubeIndex_v; indices[cubeIndex_i+1] = cubeIndex_v+1; indices[cubeIndex_i+2] = cubeIndex_v+2;
								indices[cubeIndex_i+3] = cubeIndex_v; indices[cubeIndex_i+4] = cubeIndex_v+2; indices[cubeIndex_i+5] = cubeIndex_v+3;
								cubeIndex_v += 4;
								cubeIndex_i += 6;
								addVertex = false;
							}
							//Side Z-
							if (z==0) addVertex = true;
							if (z>0){
								if (voxelData[x][y][z-1].type == 0) addVertex = true;
							}
							if (addVertex){
								verts[cubeIndex_v].x = x+0.0f; verts[cubeIndex_v].y = y+0.0f; verts[cubeIndex_v].z = z+0.0f; verts[cubeIndex_v].color = 0xffffffff; verts[cubeIndex_v].u1 = 0.0f; verts[cubeIndex_v].v1 = 1.0f; verts[cubeIndex_v].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
								verts[cubeIndex_v+1].x = x+1.0f; verts[cubeIndex_v+1].y = y+0.0f; verts[cubeIndex_v+1].z = z+0.0f; verts[cubeIndex_v+1].color = 0xffffffff; verts[cubeIndex_v+1].u1 = 1.0f; verts[cubeIndex_v+1].v1 = 1.0f; verts[cubeIndex_v+1].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
								verts[cubeIndex_v+2].x = x+1.0f; verts[cubeIndex_v+2].y = y+1.0f; verts[cubeIndex_v+2].z = z+0.0f; verts[cubeIndex_v+2].color = 0xffffffff; verts[cubeIndex_v+2].u1 = 1.0f; verts[cubeIndex_v+2].v1 = 0.0f; verts[cubeIndex_v+2].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
								verts[cubeIndex_v+3].x = x+0.0f; verts[cubeIndex_v+3].y = y+1.0f; verts[cubeIndex_v+3].z = z+0.0f; verts[cubeIndex_v+3].color = 0xffffffff; verts[cubeIndex_v+3].u1 = 0.0f; verts[cubeIndex_v+3].v1 = 0.0f; verts[cubeIndex_v+3].normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
								indices[cubeIndex_i] = cubeIndex_v; indices[cubeIndex_i+1] = cubeIndex_v+2; indices[cubeIndex_i+2] = cubeIndex_v+1;
								indices[cubeIndex_i+3] = cubeIndex_v; indices[cubeIndex_i+4] = cubeIndex_v+3; indices[cubeIndex_i+5] = cubeIndex_v+2;
								cubeIndex_v += 4;
								cubeIndex_i += 6;
								addVertex = false;
							}
							//Side X+
							if (x==(MAP_DIMENSION-1)) addVertex = true;
							if (x<(MAP_DIMENSION-1)){
								if (voxelData[x+1][y][z].type == 0) addVertex = true;
							}
							if (addVertex){
								verts[cubeIndex_v].x = x+1.0f; verts[cubeIndex_v].y = y+0.0f; verts[cubeIndex_v].z = z+0.0f; verts[cubeIndex_v].color = 0xffffffff; verts[cubeIndex_v].u1 = 0.0f; verts[cubeIndex_v].v1 = 1.0f; verts[cubeIndex_v].normal = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
								verts[cubeIndex_v+1].x = x+1.0f; verts[cubeIndex_v+1].y = y+0.0f; verts[cubeIndex_v+1].z = z+1.0f; verts[cubeIndex_v+1].color = 0xffffffff; verts[cubeIndex_v+1].u1 = 1.0f; verts[cubeIndex_v+1].v1 = 1.0f; verts[cubeIndex_v+1].normal = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
								verts[cubeIndex_v+2].x = x+1.0f; verts[cubeIndex_v+2].y = y+1.0f; verts[cubeIndex_v+2].z = z+1.0f; verts[cubeIndex_v+2].color = 0xffffffff; verts[cubeIndex_v+2].u1 = 1.0f; verts[cubeIndex_v+2].v1 = 0.0f; verts[cubeIndex_v+2].normal = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
								verts[cubeIndex_v+3].x = x+1.0f; verts[cubeIndex_v+3].y = y+1.0f; verts[cubeIndex_v+3].z = z+0.0f; verts[cubeIndex_v+3].color = 0xffffffff; verts[cubeIndex_v+3].u1 = 0.0f; verts[cubeIndex_v+3].v1 = 0.0f; verts[cubeIndex_v+3].normal = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
								indices[cubeIndex_i] = cubeIndex_v; indices[cubeIndex_i+1] = cubeIndex_v+2; indices[cubeIndex_i+2] = cubeIndex_v+1;
								indices[cubeIndex_i+3] = cubeIndex_v; indices[cubeIndex_i+4] = cubeIndex_v+3; indices[cubeIndex_i+5] = cubeIndex_v+2;
								cubeIndex_v += 4;
								cubeIndex_i += 6;
								addVertex = false;
							}
							//Side X-
							if (x==0) addVertex = true;
							if (x>0){
								if (voxelData[x-1][y][z].type == 0) addVertex = true;
							}
							if (addVertex){
								verts[cubeIndex_v].x = x+0.0f; verts[cubeIndex_v].y = y+0.0f; verts[cubeIndex_v].z = z+0.0f; verts[cubeIndex_v].color = 0xffffffff; verts[cubeIndex_v].u1 = 0.0f; verts[cubeIndex_v].v1 = 1.0f; verts[cubeIndex_v].normal = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
								verts[cubeIndex_v+1].x = x+0.0f; verts[cubeIndex_v+1].y = y+0.0f; verts[cubeIndex_v+1].z = z+1.0f; verts[cubeIndex_v+1].color = 0xffffffff; verts[cubeIndex_v+1].u1 = 1.0f; verts[cubeIndex_v+1].v1 = 1.0f; verts[cubeIndex_v+1].normal = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
								verts[cubeIndex_v+2].x = x+0.0f; verts[cubeIndex_v+2].y = y+1.0f; verts[cubeIndex_v+2].z = z+1.0f; verts[cubeIndex_v+2].color = 0xffffffff; verts[cubeIndex_v+2].u1 = 1.0f; verts[cubeIndex_v+2].v1 = 0.0f; verts[cubeIndex_v+2].normal = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
								verts[cubeIndex_v+3].x = x+0.0f; verts[cubeIndex_v+3].y = y+1.0f; verts[cubeIndex_v+3].z = z+0.0f; verts[cubeIndex_v+3].color = 0xffffffff; verts[cubeIndex_v+3].u1 = 0.0f; verts[cubeIndex_v+3].v1 = 0.0f; verts[cubeIndex_v+3].normal = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
								indices[cubeIndex_i] = cubeIndex_v; indices[cubeIndex_i+1] = cubeIndex_v+1; indices[cubeIndex_i+2] = cubeIndex_v+2;
								indices[cubeIndex_i+3] = cubeIndex_v; indices[cubeIndex_i+4] = cubeIndex_v+2; indices[cubeIndex_i+5] = cubeIndex_v+3;
								cubeIndex_v += 4;
								cubeIndex_i += 6;
								addVertex = false;
							}
							//Side Y+
							if (y==(MAP_DIMENSION-1)) addVertex = true;
							if (y<(MAP_DIMENSION-1)){
								if (voxelData[x][y+1][z].type == 0) addVertex = true;
							}
							if (addVertex){
								verts[cubeIndex_v].x = x+0.0f; verts[cubeIndex_v].y = y+1.0f; verts[cubeIndex_v].z = z+0.0f; verts[cubeIndex_v].color = 0xffffffff; verts[cubeIndex_v].u1 = 0.0f; verts[cubeIndex_v].v1 = 1.0f; verts[cubeIndex_v].normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
								verts[cubeIndex_v+1].x = x+1.0f; verts[cubeIndex_v+1].y = y+1.0f; verts[cubeIndex_v+1].z = z+0.0f; verts[cubeIndex_v+1].color = 0xffffffff; verts[cubeIndex_v+1].u1 = 1.0f; verts[cubeIndex_v+1].v1 = 1.0f; verts[cubeIndex_v+1].normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
								verts[cubeIndex_v+2].x = x+1.0f; verts[cubeIndex_v+2].y = y+1.0f; verts[cubeIndex_v+2].z = z+1.0f; verts[cubeIndex_v+2].color = 0xffffffff; verts[cubeIndex_v+2].u1 = 1.0f; verts[cubeIndex_v+2].v1 = 0.0f; verts[cubeIndex_v+2].normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
								verts[cubeIndex_v+3].x = x+0.0f; verts[cubeIndex_v+3].y = y+1.0f; verts[cubeIndex_v+3].z = z+1.0f; verts[cubeIndex_v+3].color = 0xffffffff; verts[cubeIndex_v+3].u1 = 0.0f; verts[cubeIndex_v+3].v1 = 0.0f; verts[cubeIndex_v+3].normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
								indices[cubeIndex_i] = cubeIndex_v; indices[cubeIndex_i+1] = cubeIndex_v+2; indices[cubeIndex_i+2] = cubeIndex_v+1;
								indices[cubeIndex_i+3] = cubeIndex_v; indices[cubeIndex_i+4] = cubeIndex_v+3; indices[cubeIndex_i+5] = cubeIndex_v+2;
								cubeIndex_v += 4;
								cubeIndex_i += 6;
								addVertex = false;
							}
							//Side Y-
							if (y==0) addVertex = true;
							if (y>0){
								if (voxelData[x][y-1][z].type == 0) addVertex = true;
							}
							if (addVertex){
								verts[cubeIndex_v].x = x+0.0f; verts[cubeIndex_v].y = y+0.0f; verts[cubeIndex_v].z = z+0.0f; verts[cubeIndex_v].color = 0xffffffff; verts[cubeIndex_v].u1 = 0.0f; verts[cubeIndex_v].v1 = 1.0f; verts[cubeIndex_v].normal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
								verts[cubeIndex_v+1].x = x+1.0f; verts[cubeIndex_v+1].y = y+0.0f; verts[cubeIndex_v+1].z = z+0.0f; verts[cubeIndex_v+1].color = 0xffffffff; verts[cubeIndex_v+1].u1 = 1.0f; verts[cubeIndex_v+1].v1 = 1.0f; verts[cubeIndex_v+1].normal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
								verts[cubeIndex_v+2].x = x+1.0f; verts[cubeIndex_v+2].y = y+0.0f; verts[cubeIndex_v+2].z = z+1.0f; verts[cubeIndex_v+2].color = 0xffffffff; verts[cubeIndex_v+2].u1 = 1.0f; verts[cubeIndex_v+2].v1 = 0.0f; verts[cubeIndex_v+2].normal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
								verts[cubeIndex_v+3].x = x+0.0f; verts[cubeIndex_v+3].y = y+0.0f; verts[cubeIndex_v+3].z = z+1.0f; verts[cubeIndex_v+3].color = 0xffffffff; verts[cubeIndex_v+3].u1 = 0.0f; verts[cubeIndex_v+3].v1 = 0.0f; verts[cubeIndex_v+3].normal = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
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
		}
	}
	totalMapVertexBuffer->Unlock();
	totalMapIndexBuffer->Unlock();

	return true;
}

void SLevelEditor::createTextureAtlasFromFileGroup(void){
	D3DXCreateTexture(S_GUI::getDevice(), 1024, 1024, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &totalMapTextureAtlas);
	LPDIRECT3DSURFACE9 surface;
	LPDIRECT3DSURFACE9 origSurface;
	totalMapTextureAtlas->GetSurfaceLevel(0, &surface);

	LPDIRECT3DTEXTURE9 importTexGroup[64];
	for (int x = 0; x < 64; x++){
		importTexGroup[x] = NULL;
	}
#pragma region TexturePathModification
	std::wstringstream path;
	path.str(L"");
	int maxTexIndex = 13;
	for (int i = 0; i < 64; i++)
	{
		path.str(L"");
		int pathMod = i;
		if (pathMod > maxTexIndex) pathMod = 0; 
		path<<L"../Resources/Levels/Allocation/Textures/block"<<pathMod<<L".png";
		D3DXCreateTextureFromFileEx(S_GUI::getDevice(), path.str().c_str(), 128, 128, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0xff000000, 0, 0, &importTexGroup[i]);
	}
#pragma endregion
	
	LPD3DXSPRITE sprite;
	D3DXCreateSprite(S_GUI::getDevice(), &sprite);
	S_GUI::getDevice()->GetRenderTarget(0, &origSurface);
	S_GUI::getDevice()->SetRenderTarget(0, surface);
	S_GUI::getDevice()->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
	S_GUI::getDevice()->BeginScene();
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
	S_GUI::getDevice()->EndScene();
	S_GUI::getDevice()->SetRenderTarget(0, origSurface);
	//importTexGroup[0]->LockRect(
}

void SLevelEditor::createBoxBuffers(void){
	//Fill a vb with lines - pretty simple, no worries bro
	SEDITOR_SIMPLEVERTEX* lineVerts = new SEDITOR_SIMPLEVERTEX[8];
	S_GUI::getDevice()->CreateVertexBuffer(sizeof(SEDITOR_SIMPLEVERTEX)*8, 0, SEDITOR_SIMPLEFVF, D3DPOOL_MANAGED, &boxVertexBuffer, 0);
	boxVertexBuffer->Lock(0, 0, (void**)&lineVerts, 0);
	lineVerts[0].x = -1.0f; lineVerts[0].y = 1.0f; lineVerts[0].z = -1.0f; lineVerts[0].color = 0xffffffff;
	lineVerts[1].x = 1.0f; lineVerts[1].y = 1.0f; lineVerts[1].z = -1.0f; lineVerts[1].color = 0xffffffff;
	lineVerts[2].x = -1.0f; lineVerts[2].y = -1.0f; lineVerts[2].z = -1.0f; lineVerts[2].color = 0xffffffff;
	lineVerts[3].x = 1.0f; lineVerts[3].y = -1.0f; lineVerts[3].z = -1.0f; lineVerts[3].color = 0xffffffff;

	lineVerts[4].x = -1.0f; lineVerts[4].y = 1.0f; lineVerts[4].z = 1.0f; lineVerts[4].color = 0xffffffff;
	lineVerts[5].x = 1.0f; lineVerts[5].y = 1.0f; lineVerts[5].z = 1.0f; lineVerts[5].color = 0xffffffff;
	lineVerts[6].x = -1.0f; lineVerts[6].y = -1.0f; lineVerts[6].z = 1.0f; lineVerts[6].color = 0xffffffff;
	lineVerts[7].x = 1.0f; lineVerts[7].y = -1.0f; lineVerts[7].z = 1.0f; lineVerts[7].color = 0xffffffff;
	boxVertexBuffer->Unlock();

	short* lineIndices = new short[24];
	S_GUI::getDevice()->CreateIndexBuffer(sizeof(short)*24, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &boxIndexBuffer, 0);
	boxIndexBuffer->Lock(0, 0, (void**)&lineIndices, 0);
	lineIndices[0] = 0; lineIndices[1] = 1;//front-topline
	lineIndices[2] = 1; lineIndices[3] = 3;//front-rightline
	lineIndices[4] = 0; lineIndices[5] = 2;//front-leftline
	lineIndices[6] = 2; lineIndices[7] = 3;//front-bottomline

	lineIndices[8] = 4; lineIndices[9] = 5;//back-topline
	lineIndices[10] = 5; lineIndices[11] = 7;//back-rightline
	lineIndices[12] = 4; lineIndices[13] = 6;//back-leftline
	lineIndices[14] = 6; lineIndices[15] = 7;//back-bottomline

	lineIndices[16] = 2; lineIndices[17] = 6;//leftside-bottom
	lineIndices[18] = 0; lineIndices[19] = 4;//leftside-top
	lineIndices[20] = 1; lineIndices[21] = 5;//rightside-top
	lineIndices[22] = 3; lineIndices[23] = 7;//rightside-bottom
	boxIndexBuffer->Unlock();
}

int SLevelEditor::checkRayIntersectLight(void){
	POINT p;
	GetCursorPos(&p);
	int x = p.x;
	int y = p.y;
	D3DXMATRIX matProj;
	S_GUI::getDevice()->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXVECTOR3 v;
	v.x = (((2.0f*x)/1680)-1.0f)/matProj._11;
	v.y = -(((2.0f*y)/1050)-1.0f)/matProj._22;
	v.z = 1.0f;

	D3DXMATRIX m, view;
	S_GUI::getDevice()->GetTransform(D3DTS_VIEW, &view);
	D3DXMatrixInverse(&m, NULL, &view);

	D3DXVECTOR3 rayDir, rayOrigin;
	rayDir.x = v.x*m._11 + v.y*m._21 + v.z*m._31;
	rayDir.y = v.x*m._12 + v.y*m._22 + v.z*m._32;
	rayDir.z = v.x*m._13 + v.y*m._23 + v.z*m._33;
	rayOrigin.x = m._41;
	rayOrigin.y = m._42;
	rayOrigin.z = m._43;

	D3DXVECTOR3 p1 = rayOrigin;
	for (int i = 0; i < 500; i++){
		p1 = p1 + (rayDir*0.1f);
		//check for intersection with light
		for (int x = 0; x < numLights; x++){
			if (lights[x].active){
				if (p1.x < lights[x].light.Position.x + 1.0f && p1.x > lights[x].light.Position.x - 1.0f && p1.y < lights[x].light.Position.y + 1.0f && p1.y > lights[x].light.Position.y - 1.0f
					&& p1.z < lights[x].light.Position.z + 1.0f && p1.z > lights[x].light.Position.z - 1.0f){
						return x;
				}
			}
		}
	}
	return -1;
}

void SLevelEditor::renderLightingText(void){
	RECT fontPosition;
	fontPosition.bottom = 1080;
	fontPosition.top = 0;
	fontPosition.right = 1920;
	fontPosition.left = 0;
	std::ostringstream ss;
	ss.str("");
	if (lightSelection < 0){
		ss<<"NO LIGHT SELECTION";
	} else {
		ss<<"Light Selected: "<<lightSelection<<"||Total Active Lights: "<<getNumActiveLights();
	}
	
	fontPosition.top = 350;
	S_FONT::defaultFont->DrawTextA(NULL, ss.str().c_str(), -1, &fontPosition, DT_LEFT, 0xffffffff);
	ss.str("");
	if (lightModSelection==0)ss<<"->";
	ss<<"dRED: "<<lights[lightSelection].light.Diffuse.r;
	fontPosition.top = 370;
	S_FONT::defaultFont->DrawTextA(NULL, ss.str().c_str(), -1, &fontPosition, DT_LEFT, 0xffffffff);
	ss.str("");
	if (lightModSelection==1)ss<<"->";
	ss<<"dGREEN: "<<lights[lightSelection].light.Diffuse.g;
	fontPosition.top = 390;
	S_FONT::defaultFont->DrawTextA(NULL, ss.str().c_str(), -1, &fontPosition, DT_LEFT, 0xffffffff);
	ss.str("");
	if (lightModSelection==2)ss<<"->";
	ss<<"dBLUE: "<<lights[lightSelection].light.Diffuse.b;
	fontPosition.top = 410;
	S_FONT::defaultFont->DrawTextA(NULL, ss.str().c_str(), -1, &fontPosition, DT_LEFT, 0xffffffff);
	ss.str("");
	if (lightModSelection==3)ss<<"->";
	ss<<"aRED: "<<lights[lightSelection].light.Ambient.r;
	fontPosition.top = 430;
	S_FONT::defaultFont->DrawTextA(NULL, ss.str().c_str(), -1, &fontPosition, DT_LEFT, 0xffffffff);
	ss.str("");
	if (lightModSelection==4)ss<<"->";
	ss<<"aGREEN: "<<lights[lightSelection].light.Ambient.g;
	fontPosition.top = 450;
	S_FONT::defaultFont->DrawTextA(NULL, ss.str().c_str(), -1, &fontPosition, DT_LEFT, 0xffffffff);
	ss.str("");
	if (lightModSelection==5)ss<<"->";
	ss<<"aBLUE: "<<lights[lightSelection].light.Ambient.b;
	fontPosition.top = 470;
	S_FONT::defaultFont->DrawTextA(NULL, ss.str().c_str(), -1, &fontPosition, DT_LEFT, 0xffffffff);
	ss.str("");
	if (lightModSelection==6)ss<<"->";
	ss<<"RANGE: "<<lights[lightSelection].light.Range;
	fontPosition.top = 490;
	S_FONT::defaultFont->DrawTextA(NULL, ss.str().c_str(), -1, &fontPosition, DT_LEFT, 0xffffffff);
	ss.str("");
	if (lightModSelection==7)ss<<"->";
	ss<<"Att0: "<<lights[lightSelection].light.Attenuation0;
	fontPosition.top = 510;
	S_FONT::defaultFont->DrawTextA(NULL, ss.str().c_str(), -1, &fontPosition, DT_LEFT, 0xffffffff);
	ss.str("");
	if (lightModSelection==8)ss<<"->";
	ss<<"Att1: "<<lights[lightSelection].light.Attenuation1;
	fontPosition.top = 530;
	S_FONT::defaultFont->DrawTextA(NULL, ss.str().c_str(), -1, &fontPosition, DT_LEFT, 0xffffffff);
	ss.str("");
	if (lightModSelection==9)ss<<"->";
	ss<<"Att2: "<<lights[lightSelection].light.Attenuation2;
	fontPosition.top = 550;
	S_FONT::defaultFont->DrawTextA(NULL, ss.str().c_str(), -1, &fontPosition, DT_LEFT, 0xffffffff);
}

D3DXVECTOR3 SLevelEditor::getPositionAlongRay(int loops){
	POINT p;
	GetCursorPos(&p);
	int x = p.x;
	int y = p.y;
	D3DXMATRIX matProj;
	S_GUI::getDevice()->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXVECTOR3 v;
	v.x = (((2.0f*x)/1680)-1.0f)/matProj._11;
	v.y = -(((2.0f*y)/1050)-1.0f)/matProj._22;
	v.z = 1.0f;

	D3DXMATRIX m, view;
	S_GUI::getDevice()->GetTransform(D3DTS_VIEW, &view);
	D3DXMatrixInverse(&m, NULL, &view);

	D3DXVECTOR3 rayDir, rayOrigin;
	rayDir.x = v.x*m._11 + v.y*m._21 + v.z*m._31;
	rayDir.y = v.x*m._12 + v.y*m._22 + v.z*m._32;
	rayDir.z = v.x*m._13 + v.y*m._23 + v.z*m._33;
	rayOrigin.x = m._41;
	rayOrigin.y = m._42;
	rayOrigin.z = m._43;

	D3DXVECTOR3 p1 = rayOrigin;
	float l = (float) loops;
	for (int i = 0; i < 100; i++){
		p1 += (rayDir*0.1f);
	}
		//check for intersection with light
	return p1;
	
}
int SLevelEditor::requestAddLight(void){
	int returnIndex = 0;
	for (int i = 0; i < numLights; i++){
		if (!lights[i].active){
			lights[i].active = true;
			lights[i].light.Type = D3DLIGHT_POINT;
			lights[i].light.Position = user_position;
			lights[i].light.Attenuation0 = 1.0f;
			lights[i].light.Attenuation1 = 0.0f;
			lights[i].light.Attenuation2 = 0.0f;
			lights[i].light.Range = 10.0f;
			lights[i].light.Diffuse.r = 1.0f;
			lights[i].light.Diffuse.g = 1.0f;
			lights[i].light.Diffuse.b = 1.0f;
			lights[i].light.Diffuse.a = 1.0f;
			lights[i].light.Ambient.r = 0.0f;
			lights[i].light.Ambient.g = 0.0f;
			lights[i].light.Ambient.b = 0.0f;
			lights[i].light.Ambient.a = 0.0f;
			return i;
			
		}
	}
	return -1;
}

//Just commenting here because im actually in class coding and he told us to right something down. LOL hes looking at me with pride,
//thinking that im paying attention but really im just commenting my code. FUCK_THE_SYSTEM();
int SLevelEditor::getNumActiveLights(void){
	int count = 0;
	for (int i = 0; i < SLEVELEDITOR_MAXLIGHTS; i++){
		if (lights[i].active) count++;
	}
	return count;
}
void SLevelEditor::updateLightBuffer(void){
	
	/*for (int i = 0; i < 8; i++){
		int index = lightBuffer[i];
		if (lights[index].active){
			S_GUI::getDevice()->SetLight(i, &lights[index].light);
			S_GUI::getDevice()->LightEnable(i, true);
		} else {
			S_GUI::getDevice()->LightEnable(i, false);
		}
	}*/
	for (int i = 0; i < 8; i++){
		if (lights[lightBuffer[i]].active){
			S_GUI::getDevice()->SetLight(i, &lights[lightBuffer[i]].light);
			S_GUI::getDevice()->LightEnable(i, TRUE);
		} else {
			S_GUI::getDevice()->LightEnable(i, FALSE);
		}
	}
}

void SLevelEditor::giveToLightBuffer(int index){
	lightBuffer[7] = lightBuffer[6];
	lightBuffer[6] = lightBuffer[5];
	lightBuffer[5] = lightBuffer[4];
	lightBuffer[4] = lightBuffer[3];
	lightBuffer[3] = lightBuffer[2];
	lightBuffer[2] = lightBuffer[1];
	lightBuffer[1] = lightBuffer[0];
	lightBuffer[0] = index;
}

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