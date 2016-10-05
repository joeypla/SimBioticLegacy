#include "pseBase.h"

#include "SInput.h"

#include "psPhysics.h"
#include "psMath.h"
void pse::processInput(pse::WorldData* worldData, pse::EditorData* editorData)
{
	SCamera* cam = editorData->editorCam;
	
	//check for G press which modifies the gravity towards where the camera is looking
	if (S_INPUT::keyPressed(DIK_G))
	{
		psMath::float3 currentDir(cam->dx, cam->dy, cam->dz);
		editorData->gravityConfig = psMath::gravityConfigNumberFromVector(&currentDir);
		psMath::float3 newUpVector = -psMath::dominantUnitVector(&currentDir);
		
		editorData->editorCam->ux = newUpVector.x;
		editorData->editorCam->uy = newUpVector.y;
		editorData->editorCam->uz = newUpVector.z;
		
		//for some fucking reason that im not seeing, dir is not normalized or some shit. I
		//only know this because the movement becomes very slow and the movement is based on the dir of the camera
		psMath::float3 camDir = psMath::float3(editorData->editorCam->dx, editorData->editorCam->dy, editorData->editorCam->dz);
		psMath::normalize(&camDir);
		editorData->editorCam->dx = camDir.x;
		editorData->editorCam->dy = camDir.y;
		editorData->editorCam->dz = camDir.z;
	}
	// check for W keypress
	float camMoveModifier = 0.2f;
	if (S_INPUT::keyPressed(DIK_LSHIFT))
	{
		camMoveModifier *= 3.0f;
	}


	/*Orig Forward Backward Controls
	if (S_INPUT::keyPressed(DIK_W))
	{
		//move player forwards
		cam->x += cam->dx*camMoveModifier;
		cam->y += cam->dy*camMoveModifier;
		cam->z += cam->dz*camMoveModifier;
	}

	if (S_INPUT::keyPressed(DIK_S))
	{
		//move player backwards
		cam->x -= cam->dx*camMoveModifier;
		cam->y -= cam->dy*camMoveModifier;
		cam->z -= cam->dz*camMoveModifier;
	}
	*/

	
	if (S_INPUT::keyPressed(DIK_W))
	{
		psMath::float3 dir;
		psMath::float3 up;
		up.x = 0.0f; up.y = 0.0f; up.z = 0.0f;
		switch (editorData->gravityConfig)
		{
		case 0:
			up.x = 1.0f;
			break;
		case 1:
			up.x = -1.0f;
			break;
		case 2:
			up.y = 1.0f;
			break;
		case 3:
			up.y = -1.0f;
			break;
		case 4:
			up.z = 1.0f;
			break;
		case 5:
			up.z = -1.0f; 
			break;
		default:
			break;
			//fuck off
		}
		dir = psMath::cross(&cam->x_getStrafeVector(), &up);
		psMath::normalize(&dir);
		cam->x -= dir.x * camMoveModifier;
		cam->y -= dir.y * camMoveModifier;
		cam->z -= dir.z * camMoveModifier;
	}

	if (S_INPUT::keyPressed(DIK_S))
	{
		psMath::float3 dir;
		psMath::float3 up;
		up.x = 0.0f; up.y = 0.0f; up.z = 0.0f;
		switch (editorData->gravityConfig)
		{
		case 0:
			up.x = 1.0f;
			break;
		case 1:
			up.x = -1.0f;
			break;
		case 2:
			up.y = 1.0f;
			break;
		case 3:
			up.y = -1.0f;
			break;
		case 4:
			up.z = 1.0f;
			break;
		case 5:
			up.z = -1.0f;
			break;
		default:
			break;
			//fuck off
		}
		dir = psMath::cross(&cam->x_getStrafeVector(), &up);
		psMath::normalize(&dir);
		cam->x += dir.x * camMoveModifier;
		cam->y += dir.y * camMoveModifier;
		cam->z += dir.z * camMoveModifier;
	}

	if (S_INPUT::keyPressed(DIK_A))
	{
		cam->x += cam->x_getStrafeVector().x*camMoveModifier;
		cam->y += cam->x_getStrafeVector().y*camMoveModifier;
		cam->z += cam->x_getStrafeVector().z*camMoveModifier;
	}

	if (S_INPUT::keyPressed(DIK_D))
	{
		cam->x -= cam->x_getStrafeVector().x*camMoveModifier;
		cam->y -= cam->x_getStrafeVector().y*camMoveModifier;
		cam->z -= cam->x_getStrafeVector().z*camMoveModifier;
	}

	if (S_INPUT::keyPressed(DIK_E) || S_INPUT::keyPressed(DIK_SPACE))
	{
		switch (editorData->gravityConfig)
		{
		case 0:
			cam->x += camMoveModifier;
			break;
		case 1:
			cam->x -= camMoveModifier;
			break;
		case 2:
			cam->y += camMoveModifier;
			break;
		case 3:
			cam->y -= camMoveModifier;
			break;
		case 4:
			cam->z += camMoveModifier;
			break;
		case 5:
			cam->z -= camMoveModifier;
			break;
		default:
			break;
			//fuck off
		}
	}

	if (S_INPUT::keyPressed(DIK_Q) || S_INPUT::keyPressed(DIK_LCONTROL))
	{
		switch (editorData->gravityConfig)
		{
		case 0:
			cam->x -= camMoveModifier;
			break;
		case 1:
			cam->x += camMoveModifier;
			break;
		case 2:
			cam->y -= camMoveModifier;
			break;
		case 3:
			cam->y += camMoveModifier;
			break;
		case 4:
			cam->z -= camMoveModifier;
			break;
		case 5:
			cam->z += camMoveModifier;
			break;
		default:
			break;
			//fuck off
		}
	}

	//mouse looking
	float mouseX_modifier = 0.01f;
	float mouseY_modifier = -0.01f;
	float mouseX = S_INPUT::getMouseDeltaX();
	float mouseY = S_INPUT::getMouseDeltaY();

	if (S_INPUT::mousePressed(1))cam->rotateView(mouseY*mouseY_modifier, mouseX*mouseX_modifier, 0.0f, editorData->gravityConfig);
	//D3DXVECTOR3 cor = psPhysics::getBoxVoxelFieldCorrection(D3DXVECTOR3(cam->x, cam->y, cam->z), D3DXVECTOR3(0.5f, 2.7f, 0.5f), pse::getWorldData()->voxelData);
	//cam->x += cor.x;
	//cam->y += cor.y;
	//cam->z += cor.z;
}