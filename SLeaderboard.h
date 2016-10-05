#ifndef SLEADERBOARD_H
#define SLEADERBOARD_H

#include "Actor.h"
#define LEADERBOARD_MAXPLAYERS 8

#define LEADERBOARD_INFOTYPE_SOURCE_KILLS             0
#define LEADERBOARD_INFOTYPE_SOURCE_DEATHS            1
#define LEADERBOARD_INFOTYPE_SOURCE_RATIO             2
#define LEADERBOARD_INFOTYPE_SOURCE_MODESPECIFIC_1    3
#define LEADERBOARD_INFOTYPE_SOURCE_MODESPECIFIC_2    4
#define LEADERBOARD_INFOTYPE_SOURCE_MODESPECIFIC_3    5

struct LeaderboardInitData
{
	std::string mapName;
	std::string gameMode;

	int numberOfTeams;//1 or 2 for now
	std::wstring teamNames[6];

	DWORD teamColors[6];

	int numValues;//other than name and score, max of 4
	std::wstring valueNames[4];
	int valueSources[4];
};

 
class SLeaderboard
{
public:
	SLeaderboard(LeaderboardInitData* data);
	~SLeaderboard(void);
	void update(ps::GameData* pGameData);
	void setVisibility(bool v);
	bool getVisibility(void);
private:
	bool _visible;
	ps::GameData* _pLastGameData;//need this so that we can call an update from within

	DWORD teamColors[6];
	std::wstring teamNames[6];

	int team1_val1;
	int team2_val1;

	
	SText* mapAndModeTitle;
	SText* table_team1_rowInfos[6];
	S2D* _table_backs[LEADERBOARD_MAXPLAYERS];
	SText* _table_stats[8][5];
	 
	SText* _info_playerName;
	SText* _info_playerScore;
	int numCustomValues;
	int valueSources[4];
	SText* _info_playerCustom[4];

	SText* _table_playerNames[LEADERBOARD_MAXPLAYERS];
	SText* _table_playerScores[LEADERBOARD_MAXPLAYERS];
	SText* _table_playerCustom[LEADERBOARD_MAXPLAYERS][4];
};
#endif