#ifndef PS_MATH_H
#define PS_MATH_H

#include <d3dx9.h>
namespace psMath
{
	typedef D3DXVECTOR3 float3;
	typedef D3DXMATRIX float4x4;

	struct Ray{
		float3 origin;
		float3 direction;
	};
	void translation3(const float4x4* mat, float x, float y, float z);
	void normalize(float3* vec);
	void scale3(const float4x4* mat, float sx, float sy, float sz);

	void rotateX(const float4x4* mat, float angle);
	void rotateY(const float4x4* mat, float angle);
	void rotateZ(const float4x4* mat, float angle);
	void rotateA(const float4x4* mat, float3 axis, float angle);

	float angleBetweenNormalizedVectors(float3* vec1, float3* vec2);
	int gravityConfigNumberFromVector(float3* vec);

	float3 cross(float3* vec1, float3* vec2);
	float dot(float3* vec1, float3* vec2);

	float3 dominantUnitVector(float3* vec);
	Ray rayFromScreenSpaceCoordinate(float4x4* projection, float4x4 view, float x, float y, int width, int height);

	float lerp(float value1, float value2, float t);
};
#endif