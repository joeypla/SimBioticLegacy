#include "psPhysics.h"

#include "SInput.h"
void psPhysics::update(ps::GameData* gameData)
{

	

	using namespace ps;
	//THE FOLLOWING IS A SECURITY ISSUE. PLEASE DONT KEEP THIS.
	//Going to perform physics for only the player (this client)
	
	Actor* a = gameData->pClientActor();
	a->x += a->dx;
	a->y += a->dy;
	a->z += a->dz;

	D3DXVECTOR3 correction = psPhysics::getBoxVoxelFieldCorrection(D3DXVECTOR3(a->x, a->y, a->z), D3DXVECTOR3(a->xSize(), a->ySize(), a->zSize()), gameData->pWorldData()->pVoxelData());
	a->x += correction.x;
	a->y += correction.y;
	a->z += correction.z;

	if (a->gravityField==0)
	{
		if (correction.x > 0.0f){
			a->isGrounded = true;
			a->dx = 0.0f;

			a->dy = 0.0f;
			a->dz = 0.0f;
		} else {
			a->dx -= a->ag;
		}
	}

	if (a->gravityField==1)
	{
		if (correction.x < 0.0f){
			a->isGrounded = true;
			a->dx = 0.0f;

			a->dy = 0.0f;
			a->dz = 0.0f;
		} else {
			a->dx += a->ag;
		}
	}

	if (a->gravityField==2){
		if (correction.y > 0.0f){
			a->isGrounded = true;
			a->dy = 0.0f;

			a->dx = 0.0f;
			a->dz = 0.0f;
		} else {
			a->dy -= a->ag;
		}
	}

	if (a->gravityField==3)
	{
		if (correction.y < 0.0f){
			a->isGrounded = true;
			a->dy = 0.0f;

			a->dx = 0.0f;
			a->dz = 0.0f;
		} else {
			a->dy += a->ag;
		}
	}
	if (a->gravityField==4)
	{
		if (correction.z > 0.0f){
			a->isGrounded = true;
			a->dz = 0.0f;

			a->dx = 0.0f;
			a->dy = 0.0f;
		} else {
			a->dz -= a->ag; 
		}
	}

	if (a->gravityField==5)
	{
		if (correction.z < 0.0f){
			a->isGrounded = true;
			a->dz = 0.0f;

			a->dx = 0.0f;
			a->dy = 0.0f;
		} else {
			a->dz += a->ag;
		}
	}
	
}
bool inRange(float number, int min, int max);

D3DXVECTOR3 psPhysics::getBoxVoxelFieldCorrection(D3DXVECTOR3 boxPos, D3DXVECTOR3 boxSize, ps::VoxelData* field)
{
	/* first we find the amount of voxels that need to be sampled */
	/* goes by following equation: samples = 1 + 2 * (1+trunc(halfBox + 0.5)) */
	int xSamples = 1 + 2*(1 + ((int)(boxSize.x*0.5f + 0.5f)));
	int ySamples = 1 + 2*(1 + ((int)(boxSize.y*0.5f + 0.5f)));
	int zSamples = 1 + 2*(1 + ((int)(boxSize.z*0.5f + 0.5f)));

	/* second thing to do is construct a small set of voxel data
	that represents a contextual field around the player. This makes
	the collision detection and correction extremely fast as only 27 checks have to be performed
	and the checks consist of very few instructions. */

	/* build a set of data around the player */
	/*char*** localData;
	localData = new char**[xSamples];
	for (int x = 0; x < xSamples; x++)
	{
		localData[x] = new char*[ySamples];
		for (int y = 0; y < ySamples; y++)
		{
			localData[x][y] = new char[zSamples];
		}
	}*/
	
	char localData[5][5][5];
	/* X-Z SLICE WHERE FROM Y -ySamples to +ySamples*/
	int xSampleOffset = (xSamples - 1) / 2;
	int ySampleOffset = (ySamples - 1) / 2;
	int zSampleOffset = (zSamples - 1) / 2;
	/* The following may be a bit overkill, but we have to make sure that everything is in range */
	for (int x = -xSampleOffset; x <= xSampleOffset; x++)
	{
		for (int y = -ySampleOffset; y <= ySampleOffset; y++)
		{
			for (int z = -zSampleOffset; z <= zSampleOffset; z++)
			{
				localData[x + xSampleOffset][y + ySampleOffset][z + zSampleOffset]
				=( 
					inRange((int)(boxPos.x + x), 0, 63) && 
					inRange((int)(boxPos.y + y), 0, 63) &&
					inRange((int)(boxPos.z + z), 0, 63))?

					field->data((int)(boxPos.x + x),(int)(boxPos.y + y),(int)(boxPos.z + z)) : 0;
			}
		}
	}
	
	///* X-Z SLICE WHERE Y = 0 */
	//localData[0][0][0] = (inRange(boxPos.x - 1.0f, 0, 63) && inRange(boxPos.y - 1.0f, 0, 63) && inRange(boxPos.z - 1.0f, 0, 63))?field->voxels    [(int)(boxPos.x - 1.0f)]      [(int)(boxPos.y - 1.0f)]       [(int)(boxPos.z - 1.0f)] : 0;
	//localData[1][0][0] = (inRange(boxPos.x, 0, 63) && inRange(boxPos.y - 1.0f, 0, 63) && inRange(boxPos.z - 1.0f, 0, 63))?field->voxels           [(int)boxPos.x]               [(int)(boxPos.y - 1.0f)]       [(int)(boxPos.z - 1.0f)] : 0;
	//localData[2][0][0] = (inRange(boxPos.x + 1.0f, 0, 63) && inRange(boxPos.y - 1.0f, 0, 63) && inRange(boxPos.z - 1.0f, 0, 63))?field->voxels    [(int)(boxPos.x + 1.0f)]      [(int)(boxPos.y - 1.0f)]       [(int)(boxPos.z - 1.0f)] : 0;

	//localData[0][0][1] = (inRange(boxPos.x - 1.0f, 0, 63) && inRange(boxPos.y - 1.0f, 0, 63) && inRange(boxPos.z, 0, 63))?field->voxels           [(int)(boxPos.x - 1.0f)]      [(int)(boxPos.y - 1.0f)]       [(int)boxPos.z]          : 0;
	//localData[1][0][1] = (inRange(boxPos.x, 0, 63) && inRange(boxPos.y - 1.0f, 0, 63) && inRange(boxPos.z, 0, 63))?field->voxels                  [(int)boxPos.x]               [(int)(boxPos.y - 1.0f)]       [(int)boxPos.z]          : 0;
	//localData[2][0][1] = (inRange(boxPos.x + 1.0f, 0, 63) && inRange(boxPos.y - 1.0f, 0, 63) && inRange(boxPos.z, 0, 63))?field->voxels           [(int)(boxPos.x + 1.0f)]      [(int)(boxPos.y - 1.0f)]       [(int)boxPos.z]          : 0;

	//localData[0][0][2] = (inRange(boxPos.x - 1.0f, 0, 63) && inRange(boxPos.y - 1.0f, 0, 63) && inRange(boxPos.z + 1.0f, 0, 63))?field->voxels    [(int)(boxPos.x - 1.0f)]      [(int)(boxPos.y - 1.0f)]       [(int)(boxPos.z + 1.0f)] : 0;
	//localData[1][0][2] = (inRange(boxPos.x, 0, 63) && inRange(boxPos.y - 1.0f, 0, 63) && inRange(boxPos.z + 1.0f, 0, 63))?field->voxels           [(int)boxPos.x]               [(int)(boxPos.y - 1.0f)]       [(int)(boxPos.z + 1.0f)] : 0;
	//localData[2][0][2] = (inRange(boxPos.x + 1.0f, 0, 63) && inRange(boxPos.y - 1.0f, 0, 63) && inRange(boxPos.z + 1.0f, 0, 63))?field->voxels    [(int)(boxPos.x + 1.0f)]      [(int)(boxPos.y - 1.0f)]       [(int)(boxPos.z + 1.0f)] : 0;

	///* X-Z SLICE WHERE Y = 1 */

	//localData[0][1][0] = (inRange(boxPos.x - 1.0f, 0, 63) && inRange(boxPos.y, 0, 63) && inRange(boxPos.z - 1.0f, 0, 63))?field->voxels           [(int)(boxPos.x - 1.0f)]      [(int)boxPos.y]                [(int)(boxPos.z - 1.0f)] : 0;
	//localData[1][1][0] = (inRange(boxPos.x, 0, 63) && inRange(boxPos.y, 0, 63) && inRange(boxPos.z - 1.0f, 0, 63))?field->voxels                  [(int)boxPos.x]               [(int)boxPos.y]                [(int)(boxPos.z - 1.0f)] : 0;
	//localData[2][1][0] = (inRange(boxPos.x + 1.0f, 0, 63) && inRange(boxPos.y, 0, 63) && inRange(boxPos.z - 1.0f, 0, 63))?field->voxels           [(int)(boxPos.x + 1.0f)]      [(int)boxPos.y]                [(int)(boxPos.z - 1.0f)] : 0;

	//localData[0][1][1] = (inRange(boxPos.x - 1.0f, 0, 63) && inRange(boxPos.y, 0, 63) && inRange(boxPos.z, 0, 63))?field->voxels                  [(int)(boxPos.x - 1.0f)]      [(int)boxPos.y]                [(int)boxPos.z]          : 0;
	//localData[1][1][1] = (inRange(boxPos.x, 0, 63) && inRange(boxPos.y, 0, 63) && inRange(boxPos.z, 0, 63))?field->voxels                         [(int)boxPos.x]               [(int)boxPos.y]                [(int)boxPos.z]          : 0;
	//localData[2][1][1] = (inRange(boxPos.x + 1.0f, 0, 63) && inRange(boxPos.y, 0, 63) && inRange(boxPos.z, 0, 63))?field->voxels                  [(int)(boxPos.x + 1.0f)]      [(int)boxPos.y]                [(int)boxPos.z]          : 0;

	//localData[0][1][2] = (inRange(boxPos.x - 1.0f, 0, 63) && inRange(boxPos.y, 0, 63) && inRange(boxPos.z + 1.0f, 0, 63))?field->voxels           [(int)(boxPos.x - 1.0f)]      [(int)boxPos.y]                [(int)(boxPos.z + 1.0f)] : 0;
	//localData[1][1][2] = (inRange(boxPos.x, 0, 63) && inRange(boxPos.y, 0, 63) && inRange(boxPos.z + 1.0f, 0, 63))?field->voxels                  [(int)boxPos.x]               [(int)boxPos.y]                [(int)(boxPos.z + 1.0f)] : 0;
	//localData[2][1][2] = (inRange(boxPos.x + 1.0f, 0, 63) && inRange(boxPos.y, 0, 63) && inRange(boxPos.z + 1.0f, 0, 63))?field->voxels           [(int)(boxPos.x + 1.0f)]      [(int)boxPos.y]                [(int)(boxPos.z + 1.0f)] : 0;

	///* X-Z SLICE WHERE Y = 2 */

	//localData[0][2][0] = (inRange(boxPos.x - 1.0f, 0, 63) && inRange(boxPos.y + 1.0f, 0, 63) && inRange(boxPos.z - 1.0f, 0, 63))?field->voxels    [(int)(boxPos.x - 1.0f)]      [(int)(boxPos.y + 1.0f)]       [(int)(boxPos.z - 1.0f)] : 0;
	//localData[1][2][0] = (inRange(boxPos.x, 0, 63) && inRange(boxPos.y + 1.0f, 0, 63) && inRange(boxPos.z - 1.0f, 0, 63))?field->voxels           [(int)boxPos.x]               [(int)(boxPos.y + 1.0f)]       [(int)(boxPos.z - 1.0f)] : 0;
	//localData[2][2][0] = (inRange(boxPos.x + 1.0f, 0, 63) && inRange(boxPos.y + 1.0f, 0, 63) && inRange(boxPos.z - 1.0f, 0, 63))?field->voxels    [(int)(boxPos.x + 1.0f)]      [(int)(boxPos.y + 1.0f)]       [(int)(boxPos.z - 1.0f)] : 0;

	//localData[0][2][1] = (inRange(boxPos.x - 1.0f, 0, 63) && inRange(boxPos.y + 1.0f, 0, 63) && inRange(boxPos.z, 0, 63))?field->voxels           [(int)(boxPos.x - 1.0f)]      [(int)(boxPos.y + 1.0f)]       [(int)boxPos.z]          : 0;
	//localData[1][2][1] = (inRange(boxPos.x, 0, 63) && inRange(boxPos.y + 1.0f, 0, 63) && inRange(boxPos.z, 0, 63))?field->voxels                  [(int)boxPos.x]               [(int)(boxPos.y + 1.0f)]       [(int)boxPos.z]          : 0;
	//localData[2][2][1] = (inRange(boxPos.x + 1.0f, 0, 63) && inRange(boxPos.y + 1.0f, 0, 63) && inRange(boxPos.z, 0, 63))?field->voxels           [(int)(boxPos.x + 1.0f)]      [(int)(boxPos.y + 1.0f)]       [(int)boxPos.z]          : 0;

	//localData[0][2][2] = (inRange(boxPos.x - 1.0f, 0, 63) && inRange(boxPos.y + 1.0f, 0, 63) && inRange(boxPos.z + 1.0f, 0, 63))?field->voxels    [(int)(boxPos.x - 1.0f)]      [(int)(boxPos.y + 1.0f)]       [(int)(boxPos.z + 1.0f)] : 0;
	//localData[1][2][2] = (inRange(boxPos.x, 0, 63) && inRange(boxPos.y + 1.0f, 0, 63) && inRange(boxPos.z + 1.0f, 0, 63))?field->voxels           [(int)boxPos.x]               [(int)(boxPos.y + 1.0f)]       [(int)(boxPos.z + 1.0f)] : 0;
	//localData[2][2][2] = (inRange(boxPos.x + 1.0f, 0, 63) && inRange(boxPos.y + 1.0f, 0, 63) && inRange(boxPos.z + 1.0f, 0, 63))?field->voxels    [(int)(boxPos.x + 1.0f)]      [(int)(boxPos.y + 1.0f)]       [(int)(boxPos.z + 1.0f)] : 0;



	/*Now we loop through the field and check for collisions. If we find a collision,
	we take the player position and offset it by the smallest overlap. We don't actually
	change the real position, its just eased in the algorithm for the total correction. We continue checking
	with the updated position and perform the same operation until there are no more blocks
	to be checked. Take note: the order of block checking is actually extremely important. First we
	check mid blocks. This way we get the least amount of correction. This avoids that shitty train
	track problem when colliding with the ground. I know, I'm a genius right.
	First check X Direct Neighbours: [0][1][1] and [2][1][1]
	Next check Y Direct Neighbours: [1][0][1] and [1][2][1]
	Next chechk Z Direct Neighbours: [1][1][0] and [1][1][2]
	Then follow as normal, but we may be able to simply exit, not sure yet. Wait,
	no, we still have to check the rest in cause you're like on an edge and going to fall so fuck
	that, nevermind.*/

	D3DXVECTOR3 dynamicPosition;
	dynamicPosition = boxPos;

	D3DXVECTOR3 totalCorrection;
	totalCorrection.x = 0.0f;
	totalCorrection.y = 0.0f;
	totalCorrection.z = 0.0f;

	/*First check direct neighbours like mentioned above */

	/*Y Central sampling*/
	for (int y = -ySampleOffset; y <= ySampleOffset; y++)
	{
		int x = xSampleOffset;
		int z = zSampleOffset;
		
		if (localData[x][y + ySampleOffset][z] != 0)
		{
			
			D3DXVECTOR3 voxelPosition;
			voxelPosition.x = (int)(dynamicPosition.x) + 0.5f;
			voxelPosition.y = (int)(dynamicPosition.y + y) + 0.5f;
			voxelPosition.z = (int)(dynamicPosition.z) + 0.5f;

			psPhysics::DualBoxCollisionInfo info = getDualBoxCollision(dynamicPosition, boxSize, voxelPosition, D3DXVECTOR3(1.0f, 1.0f, 1.0f));
			if (info.colliding)
			{
				dynamicPosition = dynamicPosition + info.smallestCorrection;
				totalCorrection += info.smallestCorrection;
			}
		}
	}
	/* X Central sampling */
	for (int x = -xSampleOffset; x <= xSampleOffset; x++)
	{
		int y = ySampleOffset;
		int z = zSampleOffset;
		if (localData[x + xSampleOffset][y][z] != 0)
		{
			D3DXVECTOR3 voxelPosition;
			voxelPosition.x = (int)(dynamicPosition.x + x) + 0.5f;
			voxelPosition.y = (int)(dynamicPosition.y) + 0.5f;
			voxelPosition.z = (int)(dynamicPosition.z) + 0.5f;

			psPhysics::DualBoxCollisionInfo info = getDualBoxCollision(dynamicPosition, boxSize, voxelPosition, D3DXVECTOR3(1.0f, 1.0f, 1.0f));
			if (info.colliding)
			{
				dynamicPosition = dynamicPosition + info.smallestCorrection;
				totalCorrection += info.smallestCorrection;
			}
		}
	}

	/* Z Central sampling */
	for (int z = -zSampleOffset; z <= zSampleOffset; z++)
	{
		int x = xSampleOffset;
		int y = ySampleOffset;
		if (localData[x][y][z + zSampleOffset] != 0)
		{
			D3DXVECTOR3 voxelPosition;
			voxelPosition.x = (int)(dynamicPosition.x) + 0.5f;
			voxelPosition.y = (int)(dynamicPosition.y) + 0.5f;
			voxelPosition.z = (int)(dynamicPosition.z + z) + 0.5f;

			psPhysics::DualBoxCollisionInfo info = getDualBoxCollision(dynamicPosition, boxSize, voxelPosition, D3DXVECTOR3(1.0f, 1.0f, 1.0f));
			if (info.colliding)
			{
				dynamicPosition = dynamicPosition + info.smallestCorrection;
				totalCorrection += info.smallestCorrection;
			}
		}
	}
	
	/* Now do another pass as normal but most likely not needed.
	We really need to optimize this part as to not do extra checks */
	for (int x = -xSampleOffset; x < xSampleOffset; x++)
	{
		for (int y = -ySampleOffset; y < ySampleOffset; y++)
		{
			for (int z = -zSampleOffset; z < zSampleOffset; z++)
			{
				if (localData[x + xSampleOffset][y + ySampleOffset][z + zSampleOffset] != 0) /* is not an empty box */
				{
					D3DXVECTOR3 voxelPosition;
					voxelPosition.x = (int)(dynamicPosition.x + x) + 0.5f;
					voxelPosition.y = (int)(dynamicPosition.y + y) + 0.5f;
					voxelPosition.z = (int)(dynamicPosition.z + z) + 0.5f;

					psPhysics::DualBoxCollisionInfo info = getDualBoxCollision(dynamicPosition, boxSize, voxelPosition, D3DXVECTOR3(1.0f, 1.0f, 1.0f));
					if (info.colliding)
					{
						dynamicPosition = dynamicPosition + info.smallestCorrection;
						totalCorrection += info.smallestCorrection;
					} else {
						
					}
				}
			}
		}
	}
	return totalCorrection;
}

psPhysics::DualBoxCollisionInfo psPhysics::getDualBoxCollision(D3DXVECTOR3 box1Position, D3DXVECTOR3 box1Size, D3DXVECTOR3 box2Position, D3DXVECTOR3 box2Size)
{
	psPhysics::DualBoxCollisionInfo info;
	info.colliding = false;
	info.smallestCorrection.x = 0.0f;
	info.smallestCorrection.y = 0.0f;
	info.smallestCorrection.z = 0.0f;

	/* Position of first box */
	float x1 = box1Position.x;
	float y1 = box1Position.y;
	float z1 = box1Position.z;

	/* Size of first box but divided by 2 - because
	this function should receive the total size of the box. */
	float sx1 = box1Size.x * 0.5f;
	float sy1 = box1Size.y * 0.5f;
	float sz1 = box1Size.z * 0.5f;

	/* Position of second box */
	float x2 = box2Position.x;
	float y2 = box2Position.y;
	float z2 = box2Position.z;

	/* Size of second box but divided by 2 - because
	this function should receive the total size of the box. */
	float sx2 = box2Size.x * 0.5f;
	float sy2 = box2Size.y * 0.5f;
	float sz2 = box2Size.z * 0.5f;


	/* Now to perform the check */
	if (x1+sx1 > x2-sx2 && x1-sx1 < x2+sx2) /* they meet on the x axis */
	{
		if (y1+sy1 > y2-sy2 && y1-sy1 < y2+sy2) /* they meet on the y axis */
		{
			if (z1+sz1 > z2-sz2 && z1-sz1 < z2+sz2) /* they meet on the z axis */
			{
				info.colliding = true;

				/* now calculate per axis overlap */
				float xol = 0.0f;
				float yol = 0.0f;
				float zol = 0.0f;

				/* not amazingly readable, but it sets the overlaps depending on
				which side the first box is of the second box. That last sentence
				is most likely gramatically incorrect as well. */
				xol = (x1 < x2)?((x1+sx1) - (x2-sx2))*-1.0f : (x2+sx2) - (x1-sx1);
				yol = (y1 < y2)?((y1+sy1) - (y2-sy2))*-1.0f : (y2+sy2) - (y1-sy1);
				zol = (z1 < z2)?((z1+sz1) - (z2-sz2))*-1.0f : (z2+sz2) - (z1-sz1);

				if (abs(xol)<abs(yol)&&abs(xol)<abs(zol)) info.smallestCorrection.x = xol;
				if (abs(yol)<abs(xol)&&abs(yol)<abs(zol)) info.smallestCorrection.y = yol;
				if (abs(zol)<abs(xol)&&abs(zol)<abs(yol)) info.smallestCorrection.z = zol;
			}
		}
	}

	return info;
}

bool inRange(float number, int min, int max)
{
	if ((int)number >= min && (int)number <= max) return true;
	return false;
}

bool psPhysics::isPointWithinBox(D3DXVECTOR3 point, D3DXVECTOR3 boxPosition, D3DXVECTOR3 boxSize)
{
	float xLower = boxPosition.x - (boxSize.x*0.5f);
	float xHigher = boxPosition.x + (boxSize.x*0.5f);
	float yLower = boxPosition.y - (boxSize.y*0.5f);
	float yHigher = boxPosition.y + (boxSize.y*0.5f);
	float zLower = boxPosition.z - (boxSize.z*0.5f);
	float zHigher = boxPosition.z + (boxSize.z*0.5f);

	if (point.x > xLower && point.x < xHigher && 
		point.y > yLower && point.y < yHigher && 
		point.z > zLower && point.z < zHigher) 
		return true;

	return false;
}