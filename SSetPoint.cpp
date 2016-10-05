#include "SSetPoint.h"


psMath::float3 ps::SetPoint::getDirectionSamplingVoxelData(ps::VoxelData* vd)
{
	psMath::float3 rayOrigin(x, y, z);

	psMath::float3 ray = rayOrigin;
	float xNegTravel = 0.0f;
	float xPosTravel = 0.0f;
	float zNegTravel = 0.0f;
	float zPosTravel = 0.0f;
	for (int i = 0; i < 5; i++)
	{
		ray.x -= 1.0f;
		if ((int)ray.x < 0 || (int)ray.y < 0 || (int)ray.z) break;
		if ((int)ray.x >= vd->x() || (int)ray.y >= vd->y() || (int)ray.z >= vd->z()) break;
		if (vd->data((int)ray.x, (int)ray.y, (int)ray.z)) break;
		xNegTravel += 1.0f;
	}
	for (int i = 0; i < 5; i++)
	{
		ray.x += 1.0f;
		if ((int)ray.x < 0 || (int)ray.y < 0 || (int)ray.z) break;
		if ((int)ray.x >= vd->x() || (int)ray.y >= vd->y() || (int)ray.z >= vd->z()) break;
		if (vd->data((int)ray.x, (int)ray.y, (int)ray.z)) break;
		xPosTravel += 1.0f;
	}
	for (int i = 0; i < 5; i++)
	{
		ray.z -= 1.0f;
		if ((int)ray.x < 0 || (int)ray.y < 0 || (int)ray.z) break;
		if ((int)ray.x >= vd->x() || (int)ray.y >= vd->y() || (int)ray.z >= vd->z()) break;
		if (vd->data((int)ray.x, (int)ray.y, (int)ray.z)) break;
		zNegTravel += 1.0f;
	}
	for (int i = 0; i < 5; i++)
	{
		ray.z += 1.0f;
		if ((int)ray.x < 0 || (int)ray.y < 0 || (int)ray.z) break;
		if ((int)ray.x >= vd->x() || (int)ray.y >= vd->y() || (int)ray.z >= vd->z()) break;
		if (vd->data((int)ray.x, (int)ray.y, (int)ray.z)) break;
		zPosTravel += 1.0f;
	}
	return psMath::float3(0.0f, 1.0f, 0.0f);
}