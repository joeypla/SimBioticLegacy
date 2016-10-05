#ifndef SGAMESERVERWORLD_H
#define SGAMESERVERWORLD_H

#include "VoxelData.h"
#include "World.h"
#include <string>

/* Is a very similar class to the original world class, but without any of the render components
because this is the server version ofcourse. */
class SGameServerWorld
{
public:
	SGameServerWorld(std::string path);
	~SGameServerWorld(void);

	int setPointCount();
	ps::SetPoint* pSetPoint(int index);

	//some set point accessing - specific to game modes

	int setPointCount_spawn1(void);
	ps::SetPoint* pSetPoint_spawn1(int index);
	int setPointCount_spawn2(void);
	ps::SetPoint* pSetPoint_spawn2(int index);
	//Sector Control
	ps::SetPoint* pSetPoint_SC_A(void);
	ps::SetPoint* pSetPoint_SC_B(void);
	ps::SetPoint* pSetPoint_SC_C(void);

	//Capture the flag
	ps::SetPoint* pSetPoint_CTF_1(void);
	ps::SetPoint* pSetPoint_CTF_2(void);

	ps::VoxelData* pVoxelData(void);
private:
	std::string _properName;
	ps::VoxelData* _pVoxelData;


	//ALL SET POINTS
	int _setPointCount;
	ps::SetPoint** _ppSetPoints;
	
	//Team 1 Spawn points
	int _setPointCount_spawn1;
	ps::SetPoint** _ppSetPoints_spawn1;

	//Team 2 Spawn points
	int _setPointCount_spawn2;
	ps::SetPoint** _ppSetPoints_spawn2;

	//Sector control points
	ps::SetPoint* _pSetPoint_SC_A;
	ps::SetPoint* _pSetPoint_SC_B;
	ps::SetPoint* _pSetPoint_SC_C;

	//Cap the flag points
	ps::SetPoint* _pSetPoint_CTF_1;
	ps::SetPoint* _pSetPoint_CTF_2;
};
#endif