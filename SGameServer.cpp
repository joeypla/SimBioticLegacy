#include "SGameServer.h"


#include "SymbiotePacketStructs.h"
#include "SLOG.h"

#include "SGameServerModeDM.h"
#include "SGameServerModeSC.h"

#include "SGameStateDefines.h"

#include <sstream>
#include "GameConfigInterface.h"

#include "SCharacter.h"
#include "SAbility.h"
SGameServerInitInfo SGameServer::_globalInitInfo;
SGameServerInitInfo* SGameServer::pGlobalInitInfo(void)
{
	return &_globalInitInfo;
}
DWORD WINAPI SGameServerThread(LPVOID paramData)
{
	SGameServerInitInfo* pData = (SGameServerInitInfo*)paramData;
	
	SGameServer* gameServer = new SGameServer(pData);
	gameServer->run();

	return 0;
}

SGameServer::SGameServer(SGameServerInitInfo* data)
{
	//net address
	centralNetServerAddress.sin_family = AF_INET;    // set the address family
    centralNetServerAddress.sin_addr.s_addr = inet_addr(GlobalConfiguration::getSingleton()->str_netAddress().c_str());    // set the IP address
    centralNetServerAddress.sin_port = 12373;

	_lobbyName = "SimBiotic Lobby";

	//Print sizes of structs - temp
	S_LOG::write(std::to_string(sizeof(psNetworking::ServerOutSnapshot)).c_str());
	_usable = true;
	_mapPath = data->mapPath;
	_lobbyName = data->lobbyName;
	_gameMode = data->gameMode;
	_scoreLimit = data->scoreLimit;
	_timeLimit = data->timeLimit;
	_preMatchTime = data->preMatchTime;
	_spawnDelay = data->spawnDelay;
	S_LOG::write(std::to_string(_timeLimit).c_str());
	/* init player slots */
	for (int i = 0; i < SGAMESERVER_MAXPLAYERS; i++)
	{
		_players[i] = 0;
	}
	
	
	/* CREATE UDP SERVER SOCKET */
	socketUDP = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (socketUDP == INVALID_SOCKET)
	{
		S_LOG::write("UDP create failed");
	}
	sockaddr_in serverAddress;
	ZeroMemory(&serverAddress, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = 12371;
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	int result = bind(socketUDP, (sockaddr*)&serverAddress, sizeof(serverAddress));
	if (result == SOCKET_ERROR)
	{
		S_LOG::write("udp bind failed");
	}
	/* ----------------------------------------------- */

	/* CREATE TCP SERVER SOCKET */
	socketTCP = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socketTCP == INVALID_SOCKET) S_LOG::write("SGameServer: TCP socket invalid");

	result = bind(socketTCP, (sockaddr*)&serverAddress, sizeof(serverAddress));
	if (result = SOCKET_ERROR) S_LOG::write("SGameServer: TCP socket bind failure");

	result = listen(socketTCP, SOMAXCONN);
	if (result = SOCKET_ERROR) S_LOG::write("SGameServer: TCP socket listen failure");

	//make both sockets non-blocking which is very important
	unsigned long onUDP = 1;
	unsigned long onTCP = 1;
	ioctlsocket(socketTCP, FIONBIO, &onTCP);
	ioctlsocket(socketUDP, FIONBIO, &onUDP);

	//now instantiate game mode
	if (_gameMode == 0) _mode = new SGameServerMode_DM();
	if (_gameMode == 2) _mode = new SGameServerMode_SC();
	_mode = new SGameServerMode_DM();
	_world = new SGameServerWorld(_mapPath);

	//The player 0 is unique because it is the player on this computer. Thefore the address
	//127.0.0.1 will suffice, along with the port 12370
	sockaddr_in hostAddress;
	hostAddress.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	hostAddress.sin_port = 12370;
	hostAddress.sin_family = AF_INET;
	_players[0] = new SGameServerPlayer(data->hostName, hostAddress);


	//Set Object ID to initial state
	currentObjectID = 1;
}

SGameServer::~SGameServer(void)
{
}

int SGameServer::scoreLimit(void){return _scoreLimit;}

SGameServerPlayer* SGameServer::pPlayerAtIndex(int index)
{
	if (index < 0 || index >= SGAMESERVER_MAXPLAYERS) return NULL;

	return _players[index];
}

SGameServerWorld* SGameServer::pWorld(void)
{
	return _world;
}

#include "SInput.h"
bool SGameServer::run(void)
{
	_timeWhenStarted = GetTickCount();
	//accept any incoming connections
	DWORD buildUpMax = 33;

	DWORD buildUpCentralCom = 4000;

	while (_usable)
	{
		receiveUDPPacket();

		static DWORD timeWhenLastSent = GetTickCount();
		if (GetTickCount() - timeWhenLastSent >= buildUpMax)
		{
			timeWhenLastSent = GetTickCount();
			updateLogic();
			//checkForInactivePlayers();
			sendOutSnapshotToPlayers();

		}

		//check if we need to send a central presence packet
		static DWORD timeLastPresenceSent = GetTickCount();
		if (GetTickCount() - timeLastPresenceSent >= buildUpCentralCom)
		{
			timeLastPresenceSent = GetTickCount();
			sendNetPresencePacket();  
		}
		//check if any players need to be respawned

		//ONLY FOR TESTING ////////////////////////////
		sockaddr_in stupid;
		stupid.sin_port = 843;
		if (S_INPUT::keyPressed(DIK_U))
		{
			if (_players[1] == 0) _players[1] = new SGameServerPlayer("AI PLAYER", stupid);


			_players[1]->state2 = 2 ^ 1;
			//sendOutDisplayNameUpdateToPlayers();
		} else {
			if (_players[1] != 0) _players[1]->state2 = 0;
		}
	}
	return true;
}

void SGameServer::updateLogic(void)
{
	//keep track of the time and state.
	DWORD currentTime = GetTickCount();
	DWORD prematchMillis = _preMatchTime * 1000;
	DWORD gameLengthMillis = _timeLimit * 1000;
	DWORD elapsedTime = currentTime - _timeWhenStarted;

	if (elapsedTime < prematchMillis)
	{
		_gameState = SGAMESTATE_PREMATCH;
	}
	if (elapsedTime >= prematchMillis && elapsedTime < prematchMillis + gameLengthMillis)
	{
		_gameState = SGAMESTATE_PLAYING;//these is over ridden by the score when asking the state of the gamemode.
	}
	//_gameState = _mode->update(this);//game mode overrides the gamestate

	if (elapsedTime >= prematchMillis + gameLengthMillis)
	{
		_gameState = SGAMESTATE_OVER;
	}
	


	//check to see if any players have 0 health
	for (int i = 0; i < SGAMESERVER_MAXPLAYERS; i++)
	{
		if (_players[i] == 0) continue;
		if (_players[i]->state == 1)
		{
			if (_players[i]->health == 0)
			{
				_mode->spawnPlayer(this, _players[i]);
			}
		}

		//check if player is outside of map range, if so murder the crap out of them.
		if (_players[i]->x < -32.0f || _players[i]->x > 96.0f) _players[i]->health = 0;
		if (_players[i]->y < -32.0f || _players[i]->y > 96.0f) _players[i]->health = 0;
		if (_players[i]->z < -32.0f || _players[i]->z > 96.0f) _players[i]->health = 0;

		//ONLY FOR TESTING!!!!!!!! respawns all players
		if (S_INPUT::keyPressed(DIK_P))
		{
			_players[i]->health = 0;
		}
	}
}

void SGameServer::checkForInactivePlayers(void)
{
	for (int i = 0; i<SGAMESERVER_MAXPLAYERS; i++)
	{
		if (_players[i] == 0) continue;
		if (!_players[i]->getActive())
		{
			delete _players[i];
			_players[i] = 0;
		}
	}
}


void SGameServer::acceptNextConnection(void)
{
	//accept a connection no matter what
	SOCKET potentialClientSocket;
	sockaddr_in potentialClientAddress;
	int potentialClientAddressSize = sizeof(potentialClientAddress);
	potentialClientSocket = accept(socketTCP, (SOCKADDR*)&potentialClientAddress, &potentialClientAddressSize);
	//Search to see if a connection available
	for (int i = 0; i < SGAMESERVER_MAXPLAYERS; i++)
	{
		if (_players[i] == 0)
		{
			
		}
	}
}

void SGameServer::sendNetPresencePacket(void)
{
	psNetworking::ServerOutCentralComPresence packet;
	memcpy(packet.name, _lobbyName.c_str(), _lobbyName.size() + 1);
	if (_players[0] != 0)
		memcpy(packet.creatorName, _players[0]->name().c_str(), _players[0]->name().size() + 1);
	else memcpy(packet.creatorName, "Unknown\0", 8);
	packet.playerCount = getPlayerCount();
	packet.maxPlayerCount = SGAMESERVER_MAXPLAYERS;
	packet.specialSignatureNumber = 1357986420;

	sendto(socketUDP, (char*)&packet, sizeof(packet), 0, (sockaddr*)&centralNetServerAddress, sizeof(centralNetServerAddress));
}
void SGameServer::receiveUDPPacket(void)
{
	
	psNetworking::ClientOutPacket_UDP packet;
	sockaddr_in sourceAddress;
	
	int sourceAddressSize = sizeof(sourceAddress);
	int result = recvfrom(socketUDP, (char*)&packet, sizeof(psNetworking::ClientOutPacket_UDP), 0, (SOCKADDR*)&sourceAddress, &sourceAddressSize);
	if (result < 1) return;

	//depending on its packet type, handle accordingly
	unsigned char type = packet.type;
	 
	switch (type)
	{
	case PSNETWORKING_CLIENTOUT_UPDATE:
		processUDPPacket_playerUpdate(&packet);
		break;
	case PSNETWORKING_CLIENTOUT_HIT:
		processUDPPacket_playerHit(&packet);
		break;
	case PSNETWORKING_CLIENTOUT_USEABILITY:
		processUDPPacket_playerUseAbility(&packet);
		break;
	case PSNETWORKING_CLIENTOUT_CONNECTIONREQUEST:
		processUDPPacket_connectionRequest((psNetworking::ClientOutPacket_UDP_ConnectRequest*)&packet, &sourceAddress);
		break;
	case PSNETWORKING_CENTRALCOM_NETOUT_PUNCHTHROUGHREQUEST:
		processUDPPacket_udpPunchthrough((psNetworking::SimBioticNetServerOut_PunchthroughRequest *)&packet);
		break;
	default:
		break;
	}
	
}

void SGameServer::processUDPPacket_connectionRequest(psNetworking::ClientOutPacket_UDP_ConnectRequest* packet, sockaddr_in* source)
{
	std::stringstream ss;
	ss.str("");
	ss<<"received connection request from: ";
	for (int i = 0; i < 16; i++)
	{
		ss<<packet->name[i];
	}
	S_LOG::write(ss.str().c_str());


	psNetworking::ServerOutPacket_UDP_ConnectRequestResponse response;
	response.type = PSNETWORKING_SERVEROUT_CONNECTIONREQUESTRESPONSE;
	//Search to see if a connection available
	for (int i = 0; i < SGAMESERVER_MAXPLAYERS; i++)
	{
		if (_players[i] == 0)
		{
			response.response = PSNETWORKING_SERVEROUT_CONNECTIONREQEUSTRESPONSE_OK;
			response.index = i;
			response.mapPathSize = _mapPath.size();
			memcpy(response.mapPath, _mapPath.c_str(), _mapPath.size());

			response.gameMode = _gameMode;
			response.spawnDelay = _spawnDelay;
			response.preMatchTime = _preMatchTime;
			response.scoreLimit = _scoreLimit;
			response.timeLimit = _timeLimit;
			sendto(socketUDP, (const char*)&response, sizeof(response), 0, (SOCKADDR*)source, sizeof(sockaddr_in));

			//Now create the new player in this spot
			_players[i] = new SGameServerPlayer(std::string((const char*)packet->name).c_str(), (*source));
			sendOutDisplayNameUpdateToPlayers();
			return;
		}
	}

	//if function has gotten here, there is no connection available
	response.response = PSNETWORKING_SERVEROUT_CONNECTIONREQUESTRESPONSE_FULL;
}

void SGameServer::processUDPPacket_playerUpdate(psNetworking::ClientOutPacket_UDP* packet)
{
	psNetworking::ClientOutPacket_PlayerUpdate_UDP* playerPacket = (psNetworking::ClientOutPacket_PlayerUpdate_UDP*) packet;
	unsigned char playerIndex = playerPacket->actor.index;

	if (playerIndex < 0 || playerIndex > SGAMESERVER_MAXPLAYERS - 1) return;
	if (_players[playerIndex] == 0) return;

	_players[playerIndex]->updateFromPacket(playerPacket);
}

void SGameServer::processUDPPacket_playerHit(psNetworking::ClientOutPacket_UDP* packet)
{
	
	unsigned char shooterIndex = packet->data[0];
	unsigned char victimIndex = packet->data[1];
	unsigned char damage = packet->data[2];

	if (_players[shooterIndex]->health == 0|| _players[victimIndex]->health == 0) return;

	if (damage > _players[victimIndex]->shield)
	{
		unsigned char leftoverDamage = damage - _players[victimIndex]->shield;
		_players[victimIndex]->shield = 0;
		if (leftoverDamage > _players[victimIndex]->health)
		{
			_players[victimIndex]->timeOfLastDeath = GetTickCount();
			_players[victimIndex]->health = 0;
			_players[victimIndex]->zeroFlux();
			_players[victimIndex]->deaths += 1;
			_players[shooterIndex]->kills += 1;
			_players[shooterIndex]->score += 100;
			_players[shooterIndex]->increaseFlux(100);
		}
		else
		{
			_players[victimIndex]->health -= leftoverDamage;
			if (_players[victimIndex]->health == 0)
			{
				_players[victimIndex]->timeOfLastDeath = GetTickCount();
				_players[victimIndex]->zeroFlux();
				_players[victimIndex]->deaths += 1;
				_players[shooterIndex]->kills += 1;
				_players[shooterIndex]->score += 100;
				_players[shooterIndex]->increaseFlux(100);
			}
		}

	}
	else
	{
		_players[victimIndex]->shield -= damage;
	}
}

void SGameServer::processUDPPacket_playerUseAbility(psNetworking::ClientOutPacket_UDP* packet)
{
	
	psNetworking::ClientOutPacket_PlayerUseAbility_UDP* abilityPacket = (psNetworking::ClientOutPacket_PlayerUseAbility_UDP*) packet;

	unsigned char playerIndex = abilityPacket->index;

	if (playerIndex < 0 || playerIndex > SGAMESERVER_MAXPLAYERS - 1) return;
	if (_players[playerIndex] == 0) return;

	std::string abilityName(abilityPacket->abilityName);

	
	SCharacter* playerCharacter = _players[playerIndex]->getCharacter();
	if (playerCharacter == NULL)
		return;

	int i = 0;
	for (;;)
	{
		SAbility* ability = playerCharacter->getAbility(i);
		
		if (ability == 0)
			break;

		std::string name = ability->getName();
		psMath::float3 position(_players[playerIndex]->x, _players[playerIndex]->y, _players[playerIndex]->z);
		psMath::float3 direction(_players[playerIndex]->dirx, _players[playerIndex]->diry, _players[playerIndex]->dirz);

		if (abilityName.compare(name) == 0)
		{
			ability->serverCast(this, playerIndex, position, direction);
			break;
		}

		i++;
	}
	
}
void SGameServer::processUDPPacket_udpPunchthrough(psNetworking::SimBioticNetServerOut_PunchthroughRequest* packet)
{
	unsigned int clientIP = packet->clientIP;

	sockaddr_in address;
	address.sin_addr.S_un.S_addr = clientIP;
	address.sin_family = AF_INET;
	address.sin_port = packet->clientPort;

	psNetworking::ServerOutPacket_UDP punchPacket;
	punchPacket.type = 255;//hopefully this will get discarded even on the offchance that it passes through the clients NAT
	sendto(socketUDP, (const char*)&punchPacket, sizeof(punchPacket), 0, (sockaddr*)&address, sizeof(address));
	
	std::stringstream ss;
	ss.str("");
	ss<<"Sending out packet to open up to following address: "<<inet_ntoa(address.sin_addr);
	S_LOG::write(ss.str().c_str());
}
#include <sstream>
void SGameServer::sendOutSnapshotToPlayers(void)
{
	
	psNetworking::ServerOutSnapshot packet;
	packet.type = 0;
	packet.gameState = _gameState;
	//now we want to send the time in minutes and seconds left depending on the server state
	unsigned int seconds, minutes;
	DWORD currentTime = GetTickCount();
	DWORD prematchMillis = _preMatchTime * 1000;
	DWORD gameLengthMillis = _timeLimit * 1000;
	DWORD elapsedTime = currentTime - _timeWhenStarted;
	DWORD timeToJudge;//depends on gamestate
	if (_gameState == SGAMESTATE_PREMATCH)
		timeToJudge = prematchMillis - elapsedTime;

	if (_gameState == SGAMESTATE_PLAYING)
		timeToJudge = gameLengthMillis + prematchMillis - elapsedTime;

	if (_gameState == SGAMESTATE_OVER)
		timeToJudge = 0;

	float preciseSeconds = (timeToJudge / 1000.0f);
	minutes = (unsigned int)(preciseSeconds/ 60);

	float fSeconds = (preciseSeconds - (60 * minutes));
	seconds = (unsigned int) fSeconds;
	
	packet.val1 = (unsigned char)minutes;
	packet.val2 = (unsigned char)seconds;

	for (int i = 0; i < SGAMESERVER_MAXPLAYERS; i++)
	{
		if (_players[i] == 0)
		{
			packet.actors[i].state = 0;
			continue;
		}
		if (_players[i]->state == 2)
		{
			packet.actors[i].state = 2;
			_players[i]->state = 1;
		}

		packet.actors[i].x = _players[i]->x;
		packet.actors[i].y = _players[i]->y;
		packet.actors[i].z = _players[i]->z;

		packet.actors[i].dirx = _players[i]->dirx;
		packet.actors[i].diry = _players[i]->diry;
		packet.actors[i].dirz = _players[i]->dirz;

		packet.actors[i].health = _players[i]->health;
		packet.actors[i].shield = _players[i]->shield;
		packet.actors[i].kills = _players[i]->kills;
		packet.actors[i].deaths = _players[i]->deaths;
		packet.actors[i].score = _players[i]->score;
		packet.actors[i].gravity = _players[i]->gravity;

		packet.actors[i].flux = (unsigned char) (_players[i]->getFlux() / 100);
		packet.actors[i].state2 = _players[i]->state2;

	}

	
	
	//NOW FILL GAME OBJECTS
	for (int i = 0; i < 4; i++)
	{
		packet.objects[i].id = 0;
		//first priority is whats in the new objects queue
		if (_newObjects.empty())
		{
			continue;
		}
		//std::deque<SGameObject*>::iterator it = _newObjects.back();
		SGameObject* object = _newObjects.back();
		object->serverSerializeData(&packet.objects[i]);
		delete object;
		_newObjects.pop_back();

	}
	for (int i = 0; i < SGAMESERVER_MAXPLAYERS; i++)
	{
		if (_players[i]!=0)
		{
			sendto(socketUDP, (const char*)&packet, sizeof(psNetworking::ServerOutSnapshot), 0, (sockaddr*)_players[i]->pAddress(), sizeof(sockaddr));
		}
	}
	
}

void SGameServer::sendOutDisplayNameUpdateToPlayers(void)
{
	psNetworking::ServerOutPacket_UDP_DisplayNameUpdate packet;
	packet.type = PSNETWORKING_SERVEROUT_DISPLAYNAMEUPDATE;

	for (int i = 0; i < SGAMESERVER_MAXPLAYERS; i++)
	{
		if (_players[i] == 0)
		{
			std::string noPlayerString("<Offline>");
			memcpy(packet.name[i], noPlayerString.c_str(), noPlayerString.size() + 1);
			continue;
		}

		int playerNameSize = _players[i]->name().size();
		if (playerNameSize > 15) playerNameSize = 15;
		memcpy(packet.name[i], _players[i]->name().c_str(), playerNameSize + 1);
	}

	for (int i = 0; i < SGAMESERVER_MAXPLAYERS; i++)
	{
		if (_players[i]!=0)
		{
			sendto(socketUDP, (const char*)&packet, sizeof(psNetworking::ServerOutPacket_UDP_DisplayNameUpdate), 0, (sockaddr*)_players[i]->pAddress(), sizeof(sockaddr));
		}
	}

}

int SGameServer::getPlayerCount(void)
{
	int count = 0;
	for (int i = 0; i < SGAMESERVER_MAXPLAYERS; i++)
	{
		if (_players[i] != 0) count++;
	}

	return count;
}

SGameObject* SGameServer::spawnObject(int templateNumber)
{
	SGameObject* object = SGameObject::new_instance(templateNumber, true, currentObjectID);
	currentObjectID++;

	_newObjects.push_front(object);
	_objects.insert(std::pair<unsigned int, SGameObject*>(object->getID(), object));

	return object;
}