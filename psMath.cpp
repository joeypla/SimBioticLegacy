#include "psmath.h"

void psMath::normalize(psMath::float3* vec)
{
	D3DXVECTOR3 vec2;
	D3DXVec3Normalize(&vec2, (D3DXVECTOR3*)vec);
	vec->x = vec2.x;
	vec->y = vec2.y;
	vec->z = vec2.z;

}

psMath::float3 psMath::cross(psMath::float3* vec1, psMath::float3* vec2)
{
	float3 returnVector;
	D3DXVec3Cross((D3DXVECTOR3*)&returnVector, vec1, vec2);
	return returnVector;
}

psMath::float3 psMath::dominantUnitVector(psMath::float3* vec)
{
	float3 returnVec;
	returnVec.x = 0.0f;
	returnVec.y = 0.0f;
	returnVec.z = 0.0f;

	if (abs(vec->x) > abs(vec->y) && abs(vec->x) > abs(vec->z))
	{
		if (vec->x > 0.0f) returnVec.x = 1.0f;
		if (vec->x < 0.0f) returnVec.x = -1.0f;
	}

	if (abs(vec->y) > abs(vec->x) && abs(vec->y) > abs(vec->z))
	{
		if (vec->y > 0.0f) returnVec.y = 1.0f;
		if (vec->y < 0.0f) returnVec.y = -1.0f;
	}

	if (abs(vec->z) > abs(vec->x) && abs(vec->z) > abs(vec->y))
	{
		if (vec->z > 0.0f) returnVec.z = 1.0f;
		if (vec->z < 0.0f) returnVec.z = -1.0f;
	}

	return returnVec;
}

psMath::Ray psMath::rayFromScreenSpaceCoordinate(float4x4* projection, float4x4 view, float x, float y, int width, int height)
{

	D3DXVECTOR3 v;
	v.x = x;
	v.y = y;
	v.z = 1.0f;

	psMath::float4x4 m;//inverse view
	D3DXMatrixInverse(&m, NULL, &view);

	D3DXVECTOR3 rayDir, rayOrigin;
	rayDir.x = v.x*m._11 + v.y*m._21 + v.z*m._31;
	rayDir.y = v.x*m._12 + v.y*m._22 + v.z*m._32;
	rayDir.z = v.x*m._13 + v.y*m._23 + v.z*m._33;
	rayOrigin.x = m._41;
	rayOrigin.y = m._42;
	rayOrigin.z = m._43;

	Ray ray;
	ray.origin = rayOrigin;
	ray.direction = rayDir;
	return ray;
}

float psMath::angleBetweenNormalizedVectors(float3* vec1, float3* vec2)
{
	//assume the vectors are normalized.
	float dot = D3DXVec3Dot(vec1, vec2);
	float angle = acos(dot);
	return angle;
}

int psMath::gravityConfigNumberFromVector(psMath::float3* vec)
{
	int gravity = 0;
	//gravity is X- or X+
	if (abs(vec->x) > abs(vec->y) && abs(vec->x) > abs(vec->z)) gravity = (vec->x <= 0.0f) ? 0 : 1;

	//gravity is Y- or Z+
	if (abs(vec->y) > abs(vec->x) && abs(vec->y) > abs(vec->z)) gravity = (vec->y <= 0.0f) ? 2 : 3;

	//gravity is Z- or Z+
	if (abs(vec->z) > abs(vec->x) && abs(vec->z) > abs(vec->y)) gravity = (vec->z <= 0.0f) ? 4 : 5;
	return gravity;
}

float psMath::dot(float3* vec1, float3* vec2)
{
	return D3DXVec3Dot(vec1, vec2);
}

float psMath::lerp(float value1, float value2, float t)
{
	return (1.0f - t)*value1 + t*value2;
}