#ifndef PSWORLD_H
#define PSWORLD_H

#include <string>

#include "VoxelData.h"
#include "SRender.h"
#include "SSetPoint.h"
namespace ps
{

	class World{
	public:
		static World* new_World(std::string mapPath);
		~World(void);



		/* public members, do not break class no
		matter what they are set to, so it doesn't matter that
		they are public */
		float fogStart;
		float fogEnd;
		float fogRed;
		float fogGreen;
		float fogBlue;
		float ambientRed;
		float ambientGreen;
		float ambientBlue;

		VoxelData* pVoxelData();
		std::string properName(void);

		int setPointCount();
		ps::SetPoint* getSetPointAtIndex(int index);
	private:
		/*private constructor*/
		World(void);
		/*factory method, sort of like overloading new a bit */
		
		std::string _properName;
		

		VoxelData* _pVoxelData;

		S3D* _pRenderObject;
		SMesh* _pMesh;
		SMaterialSet* _pMaterialSet;

		int _lightCount;
		SLight** _ppLightObjects;

		int _setPointCount;
		ps::SetPoint** _ppSetPoints;
	};
};
#endif