#include "SLeaderboard.h"

#include <sstream>

#include "GameData.h"
SLeaderboard::SLeaderboard(LeaderboardInitData* data)
{
	_visible = false;
	_pLastGameData = 0;

	for (int i = 0; i < 4; i++) valueSources[i] = data->valueSources[i];
	//init fields
	for (int i = 0; i < 6; i++) teamColors[i] = data->teamColors[i];
	for (int i = 0; i < 6; i++) teamNames[i] = data->teamNames[i];

	mapAndModeTitle = SRender::getSingleton()->createText();
	mapAndModeTitle->setDimensions(1.0f, 0.05f);
	mapAndModeTitle->setPosition(0.0f, 0.7f);
	mapAndModeTitle->setAlignment(DT_CENTER | DT_VCENTER);
	
	//set the text of the map and mode header accordingly
	std::stringstream ss;
	ss.str("");
	ss<<data->mapName<<" - "<<data->gameMode;
	//std::wstring wTitle(ss.str().begin(), ss.str().end());
	//mapAndModeTitle->setText(wTitle.c_str());
	mapAndModeTitle->setColor(0xffffffff);
	mapAndModeTitle->setSize(6);

	table_team1_rowInfos[0] = SRender::getSingleton()->createText();
	table_team1_rowInfos[0]->setDimensions(0.8f, 0.05f);
	table_team1_rowInfos[0]->setPosition(-0.35f, 0.6f);
	table_team1_rowInfos[0]->setText(L"Player");
	table_team1_rowInfos[0]->setSize(5);
	table_team1_rowInfos[0]->setAlignment(DT_VCENTER|DT_LEFT);
	table_team1_rowInfos[0]->setColor(0xafcfcfcf);
	table_team1_rowInfos[0]->setVisible(false); 

	table_team1_rowInfos[1] = SRender::getSingleton()->createText();
	table_team1_rowInfos[1]->setDimensions(0.1f, 0.05f);
	table_team1_rowInfos[1]->setPosition(0.04f, 0.6f);
	table_team1_rowInfos[1]->setText(L"Score");
	table_team1_rowInfos[1]->setSize(5);
	table_team1_rowInfos[1]->setAlignment(DT_VCENTER|DT_RIGHT);
	table_team1_rowInfos[1]->setColor(0xafcfcfcf);
	table_team1_rowInfos[1]->setVisible(false);

	numCustomValues = data->numValues;
	float leaderboardWidth = 0.5f + data->numValues*0.2f;

	_info_playerName = SRender::getSingleton()->createText();
	_info_playerName->setDimensions(leaderboardWidth, 0.05f);
	_info_playerName->setPosition(0.0f, 0.65f);
	_info_playerName->setText(L"Player");
	_info_playerName->setSize(6);
	_info_playerName->setAlignment(DT_VCENTER|DT_LEFT);
	_info_playerName->setColor(0xffffffff);
	_info_playerName->setVisible(false);

	_info_playerScore = SRender::getSingleton()->createText();
	_info_playerScore->setDimensions(leaderboardWidth, 0.05f);  
	_info_playerScore->setPosition(0.0f, 0.65f);
	_info_playerScore->setText(L"Score");
	_info_playerScore->setSize(6);
	_info_playerScore->setAlignment(DT_VCENTER|DT_RIGHT);
	_info_playerScore->setColor(0xffffffff);
	_info_playerScore->setVisible(false);

	float customValuesStart = (leaderboardWidth / 2.0f) - (0.2f*numCustomValues) - 0.1f;
	for (int i = 0; i < data->numValues; i++)
	{
		_info_playerCustom[i] = SRender::getSingleton()->createText();
		_info_playerCustom[i]->setDimensions(0.2f, 0.05f);
		_info_playerCustom[i]->setPosition(customValuesStart + i*0.2f, 0.65f);
		_info_playerCustom[i]->setText(data->valueNames[i].c_str());
		_info_playerCustom[i]->setSize(6);
		_info_playerCustom[i]->setAlignment(DT_VCENTER|DT_RIGHT);
		_info_playerCustom[i]->setColor(0xffffffff);
		_info_playerCustom[i]->setVisible(false);
	}

	for (int i = 0; i < 8; i++)
	{
		_table_playerNames[i] = SRender::getSingleton()->createText();
		_table_playerNames[i]->setDimensions(leaderboardWidth, 0.07f);
		_table_playerNames[i]->setPosition(0.0f, 0.65f);
		_table_playerNames[i]->setText(L"Player");
		_table_playerNames[i]->setSize(6);
		_table_playerNames[i]->setAlignment(DT_VCENTER|DT_LEFT);
		_table_playerNames[i]->setColor(0xffffffff);
		_table_playerNames[i]->setVisible(false);

		_table_playerScores[i] = SRender::getSingleton()->createText();
		_table_playerScores[i]->setDimensions(leaderboardWidth, 0.07f);
		_table_playerScores[i]->setPosition(0.0f, 0.65f);
		_table_playerScores[i]->setText(L"0");
		_table_playerScores[i]->setSize(6);
		_table_playerScores[i]->setAlignment(DT_VCENTER|DT_RIGHT);
		_table_playerScores[i]->setColor(0xffffffff);
		_table_playerScores[i]->setVisible(false);

		for (int j = 0; j < numCustomValues; j++)
		{
			_table_playerCustom[i][j] = SRender::getSingleton()->createText();
			_table_playerCustom[i][j]->setDimensions(0.2f, 0.05f);
			_table_playerCustom[i][j]->setPosition(customValuesStart + j*0.2f, 0.65f);
			_table_playerCustom[i][j]->setText(L"0");
			_table_playerCustom[i][j]->setSize(6);
			_table_playerCustom[i][j]->setAlignment(DT_VCENTER|DT_RIGHT);
			_table_playerCustom[i][j]->setColor(0xffffffff);
			_table_playerCustom[i][j]->setVisible(false);
		}
	}


	float rowHeight = 0.55f;
	for (int i = 0; i < 8; i++)
	{
		 
		_table_backs[i] = SRender::getSingleton()->createS2D();
		_table_backs[i]->setColor(0x7fffffff);
		_table_backs[i]->setDimensions(leaderboardWidth, 0.07f);
		_table_backs[i]->setPosition(0.0f, rowHeight);
		_table_backs[i]->setVisible(false);
		rowHeight -= 0.1f;
	}
}

SLeaderboard::~SLeaderboard(void)
{
}

#include "SInput.h"
#include "SLOG.h"
void SLeaderboard::update(ps::GameData* pGameData)
{
	_pLastGameData = pGameData;
	
	if (!_visible)
	{
		for (int i = 0; i < LEADERBOARD_MAXPLAYERS; i++)
		{
			_table_backs[i]->setVisible(false);
			_table_playerNames[i]->setVisible(false);
			_table_playerScores[i]->setVisible(false);
			for (int j = 0; j < numCustomValues; j++) _table_playerCustom[i][j]->setVisible(false);
		}
		_info_playerName->setVisible(false);
		_info_playerScore->setVisible(false);
	
		for (int i = 0; i < numCustomValues; i++) _info_playerCustom[i]->setVisible(false);
		return;
	}
	_info_playerName->setVisible(true);
	_info_playerScore->setVisible(true);
	for (int i = 0; i < numCustomValues; i++) _info_playerCustom[i]->setVisible(true);


	//I guess for now a max of 6 teams
	int playerCountsPerTeam[6];
	for (int i = 0; i < 6; i++)
	{
		playerCountsPerTeam[i] = 0;
		for (int j = 0; j < LEADERBOARD_MAXPLAYERS; j++)
		{
			if (pGameData->pActorAtIndex(j)->state != 0 && pGameData->pActorAtIndex(j)->team == i + 1)
			
			{
					playerCountsPerTeam[i]++;
			}
		}
	}
	//if there is only 1 team, it means that its a free for all sort of gametype

	//first step is to sort the teams by their scores
	DWORD teamColors[6];
	teamColors[0] = 0x7fff9f00;
	teamColors[1] = 0xff00ff00;
	teamColors[2] = 0xff0000ff;
	teamColors[3] = 0xffffff00;
	teamColors[4] = 0xff00ffff;
	teamColors[5] = 0xffff00ff;
	int teamScores[6];
	teamScores[0] = 1000;
	teamScores[1] = 900;
	teamScores[2] = 800;
	teamScores[3] = 700;
	teamScores[4] = 1100;
	teamScores[5] = 500;
	int team[6];//index is their place (1st, 2nd, 3rd etc.) value is the actual team number (1 red, 2 blue etc.)
	team[0] = 0;
	team[1] = 1;
	team[2] = 2;
	team[3] = 3;
	team[4] = 4;
	team[5] = 5;

	//very simple bubble sort. We dont need a more efficient algorithm due to the data set being processed (6 teams)
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			if (teamScores[j] < teamScores[j+1])
			{

				//swap the scores in the array
				int tempScore = teamScores[j];
				teamScores[j] = teamScores[j+1];
				teamScores[j+1] = tempScore;

				//swap the teams in the array
				int tempTeam = team[j];
				team[j] = team[j+1];
				team[j+1] = tempTeam;

				//swap player counts on team
				int tempCount = playerCountsPerTeam[j];
				playerCountsPerTeam[j] = playerCountsPerTeam[j+1];
				playerCountsPerTeam[j+1] = tempCount;

				//swap team colors
				DWORD tempColor = teamColors[j];
				teamColors[j] = teamColors[j+1];
				teamColors[j+1] = tempColor;
			}
			
		}
	}
	
	//TEST CHECK TO SEE IF TEAM SORTING IS EVEN CORRECT
	if (S_INPUT::keyPressed(DIK_O))
	{
		for (int i = 0; i < 6; i++)
		{
			std::stringstream ss;
			ss.str("");
			ss<<"Team in place "<<i<<" is team "<<team[i]<<"with "<<playerCountsPerTeam[i]<<" players on it.";
			S_LOG::write(ss.str().c_str());
		}
	}
	//Now that the teams are sorted, we sort their players

	//these two are used for the gui components displaying the stats
	int backIndex = 0;
	float backPosition = 0.55f;

	for (int i = 0; i < 6; i++)
	{
		if (playerCountsPerTeam[i] < 1) continue;

		int n = playerCountsPerTeam[i];
		ps::Actor** teamPlayers = new ps::Actor*[n];
		int currentIndex = 0;
		for (int playerIndex = 0; playerIndex < LEADERBOARD_MAXPLAYERS; playerIndex++)
		{
			if (pGameData->pActorAtIndex(playerIndex)->state != 0 && pGameData->pActorAtIndex(playerIndex)->team == team[i]+1)
			{
				teamPlayers[currentIndex] = pGameData->pActorAtIndex(playerIndex);
				currentIndex++;
			}
		}

		//Now that we have the list in teamPlayers, we bubble sort it
		for (int j = 0; j < n - 1; j++)
		{
			for (int k = 0; k < n - 1; k++)
			{
				if (teamPlayers[k]->score < teamPlayers[k+1]->score)
				{
					ps::Actor* tempPlayer = teamPlayers[k];
					teamPlayers[k] = teamPlayers[k+1];
					teamPlayers[k+1] = tempPlayer;
				}
			}
		}

		//now that we have the players for this team sorted, we display these fuckers
		std::wstringstream ss;
		for (int j = 0; j < n; j++)
		{
			
			_table_backs[backIndex]->setColor(teamColors[i]);
			_table_backs[backIndex]->setPosition(0.0f, backPosition);
			_table_backs[backIndex]->setVisible(true);

			_table_playerNames[backIndex]->setPosition(0.0f, backPosition); 
			_table_playerNames[backIndex]->setText(teamPlayers[j]->name().c_str());
			_table_playerNames[backIndex]->setVisible(true);

			_table_playerScores[backIndex]->setPosition(0.0f, backPosition);
			_table_playerScores[backIndex]->setText(std::to_wstring(teamPlayers[j]->score).c_str());
			_table_playerScores[backIndex]->setVisible(true);

			for (int k = 0; k < numCustomValues; k++) 
			{
				_table_playerCustom[backIndex][k]->setVisible(true);
				_table_playerCustom[backIndex][k]->setY(backPosition);
				if (valueSources[k] == LEADERBOARD_INFOTYPE_SOURCE_KILLS)
				{
					_table_playerCustom[backIndex][k]->setText(std::to_wstring((int)teamPlayers[j]->kills).c_str());
				} else if (valueSources[k] == LEADERBOARD_INFOTYPE_SOURCE_DEATHS)
				{
					_table_playerCustom[backIndex][k]->setText(std::to_wstring((int)teamPlayers[j]->deaths).c_str());
				} else if (valueSources[k] == LEADERBOARD_INFOTYPE_SOURCE_RATIO)
				{
					float ratio = 0.0f;
					if (teamPlayers[j]->deaths < 1)
					{
						ratio = (float)teamPlayers[j]->kills;
					} else {
						ratio = (float)teamPlayers[j]->kills / (float)teamPlayers[j]->deaths;
					}
					
					ss.str(L"");
					ss.precision(2);
					ss<<std::fixed<<ratio;
					_table_playerCustom[backIndex][k]->setText(ss.str().c_str());
				} else if (valueSources[k] == LEADERBOARD_INFOTYPE_SOURCE_MODESPECIFIC_1)
				{
					_table_playerCustom[backIndex][k]->setText(std::to_wstring(teamPlayers[j]->modeSpecificVal1).c_str());
				} else if (valueSources[k] == LEADERBOARD_INFOTYPE_SOURCE_MODESPECIFIC_2)
				{
					_table_playerCustom[backIndex][k]->setText(std::to_wstring(teamPlayers[j]->modeSpecificVal2).c_str());
				} else if (valueSources[k] == LEADERBOARD_INFOTYPE_SOURCE_MODESPECIFIC_3)
				{
					_table_playerCustom[backIndex][k]->setText(std::to_wstring(teamPlayers[j]->modeSpecificVal3).c_str());
				} else 
				{
					_table_playerCustom[backIndex][k]->setText(L"NoSource");
				}
			}
			backIndex += 1;
			backPosition -= 0.1f;
		}
	}

	
		
}

void SLeaderboard::setVisibility(bool v)
{
	_visible = v;
	
}

bool SLeaderboard::getVisibility(void)
{
	return _visible;
}