#pragma once

#ifndef GMCAPTURETHEFLAG_H
#define GMCAPTURETHEFLAG_H

#include "GameInitInfo.h"
#include "GameMode.h"
namespace ps{
	class GMCaptureTheFlag : public GameMode
	{
	public:
		GMCaptureTheFlag(const ps::GameInitInfo* gii);
		~GMCaptureTheFlag(void);

		int update(ps::GameData* pGameData);
		void respawnActor(ps::Actor* actor, ps::GameData* pGameData);
		void spawnAllActors(ps::GameData* pGameData);
		//gamemode variables

		//which team the flags belong to at a given moment
		ps::SetPoint* pointA;
		ps::SetPoint* pointB;
		ps::SetPoint* pointC;

		
		SMaterialSet* controlPointMaterialSetRed;
		SMaterialSet* controlPointMaterialSetBlue;
		SMaterialSet* controlPointMaterialSetNeutral;

		SMesh* controlPointMeshRed;
		SMesh* controlPointMeshBlue;
		SMesh* controlPointMeshNeutral;

		S3D* pointA_renderObject;
		S3D* pointB_renderObject;
		S3D* pointC_renderObject;

		psMath::float3 secA_position;
		psMath::float3 secB_position;
		psMath::float3 secC_position;

		int secA_ownership;
		int secB_ownership;
		int secC_ownership;

		int secA_team1_progress;
		int secB_team1_progress;
		int secC_team1_progress;

		int secA_team2_progress;
		int secB_team2_progress;
		int secC_team2_progress;

		//global score
		int team1_score;
		int team2_score;

		int timeLeft;
	};
};
#endif