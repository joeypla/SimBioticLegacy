#include "SPhysics.h"
D3DXVECTOR3 SPhysics::getResponseProjectionVector(D3DXVECTOR3* characterPosition, float character_XL, float character_YL, float character_ZL, D3DXVECTOR3* cubePositionArray, int arrayLength){
	D3DXVECTOR3 projectionVector;
	projectionVector.x = 0.0f;
	projectionVector.y = 0.0f;
	projectionVector.z = 0.0f;
	D3DXVECTOR3 pos1 = *characterPosition;
	float x1 = pos1.x;
	float y1 = pos1.y;
	float z1 = pos1.z;
	//bool checkNext = true;
	
	for (int i = 0; i<arrayLength; i++){

		
		D3DXVECTOR3 pos2 = cubePositionArray[i];
		//Character position is already a point in the middle of the bounding box

		//Cube Position in general is the corner, in this function i'm changing it so that it represents the center
		float x2 = pos2.x+0.5f;
		float y2 = pos2.y+0.5f;
		float z2 = pos2.z+0.5f;

		if ( (x1+(character_XL/2)>(x2-0.5f))&&(x1-(character_XL/2)<x2+0.5f) ){
			if ( (y1+(character_YL/2)>(y2-0.5f))&&(y1-(character_YL/2)<y2+0.5f) ){
				if ( (z1+(character_ZL/2)>(z2-0.5f))&&(z1-(character_ZL/2)<z2+0.5f) ){
					//THEN THERE IS COLLISION
					//calculate overlap values on each axis
					//X AXIS
					float xOverlap = 0.0f;
					if (x1 < x2){
						//character is to left of box
						xOverlap = ((x1 + (character_XL/2)) - (x2-0.5f))*-1.0f;
					}
					if (x1 > x2){
						//character is to right of box
						xOverlap = (x2 + 0.5f) - (x1 - (character_XL/2));
					}


					//Y AXIS
					float yOverlap = 0.0f;
					if (y1 < y2){
						//character is below of box
						//yOverlap = (y2 + 0.5f) - (y1 - (character_YL/2));
						yOverlap = ((y1 + (character_YL/2)) - (y2-0.5f))*-1.0f;
					}
					if (y1 > y2){
						//character is on top of box
						yOverlap = (y2 + 0.5f) - (y1 - (character_YL/2));
						//yOverlap = ((y1 + (character_YL/2)) - (y2-0.5f))*-1.0f;
					}


					//Z AXIS
					float zOverlap = 0.0f;
					if (z1 < z2){
						//character is behind of box
						zOverlap = ((z1 + (character_ZL/2)) - (z2-0.5f))*-1.0f;
					}
					if (z1 > z2){
						//character is infront of box
						zOverlap = (z2 + 0.5f) - (z1 - (character_ZL/2));
					}


					//NOW FIND WHICH IS SMALLEST
					//x is smallest
					if (abs(xOverlap) < abs(yOverlap) && abs(xOverlap) < abs(zOverlap)){
						if (projectionVector.x < abs(xOverlap)) 
						projectionVector.x = xOverlap;
					}
					//y is smallest
					if (abs(yOverlap) < abs(xOverlap) && abs(yOverlap) < abs(zOverlap)){
						if (projectionVector.y < abs(yOverlap)) 
						projectionVector.y = yOverlap;
					}
					//z is smallest
					if (abs(zOverlap) < abs(xOverlap) && abs(zOverlap) < abs(yOverlap)){
						if (projectionVector.z < abs(zOverlap)) 
						projectionVector.z = zOverlap;
					}


					
				}

			}
		}
		
	}
	

	//Now lets minimize projection components. Can we resolve collision with just one component?
	/////////////////////////////////////X RESOLUTION/////////////////////////////////////
	bool canResolveWithX = false;
	if (abs(projectionVector.x) > 0.0f){
		canResolveWithX = true;
		float newPosX;
		newPosX = x1 + projectionVector.x;
		for (int i = 0; i<arrayLength; i++){
			D3DXVECTOR3 pos2 = cubePositionArray[i];
			//Character position is already a point in the middle of the bounding box

			//Cube Position in general is the corner, in this function i'm changing it so that it represents the center
			float x2 = pos2.x+0.5f;
			float y2 = pos2.y+0.5f;
			float z2 = pos2.z+0.5f;
			if ( (newPosX+(character_XL/2)>(x2-0.5f))&&(newPosX-(character_XL/2)<x2+0.5f) ){
				if ( (y1+(character_YL/2)>(y2-0.5f))&&(y1-(character_YL/2)<y2+0.5f) ){
					if ( (z1+(character_ZL/2)>(z2-0.5f))&&(z1-(character_ZL/2)<z2+0.5f) ){
						canResolveWithX = false;
					}
				}
			}
		}
	}

	if (canResolveWithX){
		projectionVector.y = 0.0f;
		projectionVector.z = 0.0f;
		return projectionVector;
	}
	///////////////////////////////////Y RESOLUTION//////////////////////////////////////////
	bool canResolveWithY = false;
	if (abs(projectionVector.y) > 0.0f){
		canResolveWithY = true;
		float newPosY;
		newPosY = y1 + projectionVector.y;
		for (int i = 0; i<arrayLength; i++){
			D3DXVECTOR3 pos2 = cubePositionArray[i];
			//Character position is already a point in the middle of the bounding box

			//Cube Position in general is the corner, in this function i'm changing it so that it represents the center
			float x2 = pos2.x+0.5f;
			float y2 = pos2.y+0.5f;
			float z2 = pos2.z+0.5f;
			if ( (x1+(character_XL/2)>(x2-0.5f))&&(x1-(character_XL/2)<x2+0.5f) ){
				if ( (newPosY+(character_YL/2)>(y2-0.5f))&&(newPosY-(character_YL/2)<y2+0.5f) ){
					if ( (z1+(character_ZL/2)>(z2-0.5f))&&(z1-(character_ZL/2)<z2+0.5f) ){
						canResolveWithY = false;
					}
				}
			}
		}
	}

	if (canResolveWithY){
		projectionVector.x = 0.0f;
		projectionVector.z = 0.0f;
		return projectionVector;
	}
	//////////////////////////////////////Z RESOLUTION///////////////////////////////////////////
	bool canResolveWithZ = false;
	if (abs(projectionVector.z) > 0.0f){
		canResolveWithZ = true;
		float newPosZ;
		newPosZ = z1 + projectionVector.z;
		for (int i = 0; i<arrayLength; i++){
			D3DXVECTOR3 pos2 = cubePositionArray[i];
			//Character position is already a point in the middle of the bounding box

			//Cube Position in general is the corner, in this function i'm changing it so that it represents the center
			float x2 = pos2.x+0.5f;
			float y2 = pos2.y+0.5f;
			float z2 = pos2.z+0.5f;
			if ( (x1+(character_XL/2)>(x2-0.5f))&&(x1-(character_XL/2)<x2+0.5f) ){
				if ( (y1+(character_YL/2)>(y2-0.5f))&&(y1-(character_YL/2)<y2+0.5f) ){
					if ( (newPosZ+(character_ZL/2)>(z2-0.5f))&&(newPosZ-(character_ZL/2)<z2+0.5f) ){
						canResolveWithZ = false;
					}
				}
			}
		}
	}

	if (canResolveWithZ){
		projectionVector.x = 0.0f;
		projectionVector.y = 0.0f;
		return projectionVector;
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	//If its up to this point, then obviously the collision could not be resolved with 1 component, try 2
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////XY RESOLUTION///////////////////////////////////////////////
	bool canResolveWithXY = false;
	if (abs(projectionVector.x) > 0.0f && abs(projectionVector.y) > 0.0f){
		canResolveWithXY = true;
		float newPosX;
		float newPosY;
		newPosX = x1 + projectionVector.x;
		newPosY = y1 + projectionVector.y;
		for (int i = 0; i<arrayLength; i++){
			D3DXVECTOR3 pos2 = cubePositionArray[i];
			//Character position is already a point in the middle of the bounding box

			//Cube Position in general is the corner, in this function i'm changing it so that it represents the center
			float x2 = pos2.x+0.5f;
			float y2 = pos2.y+0.5f;
			float z2 = pos2.z+0.5f;
			if ( (newPosX+(character_XL/2)>(x2-0.5f))&&(newPosX-(character_XL/2)<x2+0.5f) ){
				if ( (newPosY+(character_YL/2)>(y2-0.5f))&&(newPosY-(character_YL/2)<y2+0.5f) ){
					if ( (z1+(character_ZL/2)>(z2-0.5f))&&(z1-(character_ZL/2)<z2+0.5f) ){
						canResolveWithXY = false;
					}
				}
			}
		}
	}

	if (canResolveWithXY){
		projectionVector.z = 0.0f;
		return projectionVector;
	}
	///////////////////////////////////////XZ RESOLUTION/////////////////////////////////////////////
	bool canResolveWithXZ = false;
	if (abs(projectionVector.x) > 0.0f && abs(projectionVector.z) > 0.0f){
		canResolveWithXZ = true;
		float newPosX;
		float newPosZ;
		newPosX = x1 + projectionVector.x;
		newPosZ = z1 + projectionVector.z;
		for (int i = 0; i<arrayLength; i++){
			D3DXVECTOR3 pos2 = cubePositionArray[i];
			//Character position is already a point in the middle of the bounding box

			//Cube Position in general is the corner, in this function i'm changing it so that it represents the center
			float x2 = pos2.x+0.5f;
			float y2 = pos2.y+0.5f;
			float z2 = pos2.z+0.5f;
			if ( (newPosX+(character_XL/2)>(x2-0.5f))&&(newPosX-(character_XL/2)<x2+0.5f) ){
				if ( (y1+(character_YL/2)>(y2-0.5f))&&(y1-(character_YL/2)<y2+0.5f) ){
					if ( (newPosZ+(character_ZL/2)>(z2-0.5f))&&(newPosZ-(character_ZL/2)<z2+0.5f) ){
						canResolveWithXZ = false;
					}
				}
			}
		}
	}

	if (canResolveWithXZ){
		projectionVector.y = 0.0f;
		return projectionVector;
	}
	///////////////////////////////////YZ RESOLUTION///////////////////////////////////////////
	bool canResolveWithYZ = false;
	if (abs(projectionVector.y) > 0.0f && abs(projectionVector.z) > 0.0f){
		canResolveWithYZ = true;
		float newPosY;
		float newPosZ;
		newPosY = y1 + projectionVector.y;
		newPosZ = z1 + projectionVector.z;
		for (int i = 0; i<arrayLength; i++){
			D3DXVECTOR3 pos2 = cubePositionArray[i];
			//Character position is already a point in the middle of the bounding box

			//Cube Position in general is the corner, in this function i'm changing it so that it represents the center
			float x2 = pos2.x+0.5f;
			float y2 = pos2.y+0.5f;
			float z2 = pos2.z+0.5f;
			if ( (x1+(character_XL/2)>(x2-0.5f))&&(x1-(character_XL/2)<x2+0.5f) ){
				if ( (newPosY+(character_YL/2)>(y2-0.5f))&&(newPosY-(character_YL/2)<y2+0.5f) ){
					if ( (newPosZ+(character_ZL/2)>(z2-0.5f))&&(newPosZ-(character_ZL/2)<z2+0.5f) ){
						canResolveWithYZ = false;
					}
				}
			}
		}
	}

	if (canResolveWithYZ){
		projectionVector.x = 0.0f;
		return projectionVector;
	}
	return projectionVector;
	
}