#ifndef PSSETPOINT_H
#define PSSETPOINT_H

#define PS_SETPOINTS_USE_SPWN1 0
#define PS_SETPOINTS_USE_SPWN2 1
#define PS_SETPOINTS_USE_SCA   12
#define PS_SETPOINTS_USE_SCB   13
#define PS_SETPOINTS_USE_SCC   14
#define PS_SETPOINTS_USE_CTF1  15
#define PS_SETPOINTS_USE_CTF2  16

#include "psMath.h"
#include "VoxelData.h"
namespace ps
{

	class SetPoint{
	public:
		float x;
		float y;
		float z;

		int gravityConfig;
		int use;
		float rotation;

		psMath::float3 getDirectionSamplingVoxelData(ps::VoxelData* vd);
	};
}
#endif