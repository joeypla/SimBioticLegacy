#include "SGameServer.h"
#include "psGameClient.h"


#pragma comment(lib, "Ws2_32.lib")
//static SOCKET Socket;
//static WSADATA winSOCK;
//static sockaddr_in serverAddress;
//static sockaddr_in address;
//static unsigned char clientIndex = 0;
//unsigned char psGameClient::getClientIndex(void){return clientIndex;}
//
#include "SLOG.h"
#include "SInput.h"
#include "SymbiotePacketStructs.h"
#include "psHUD.h"
//
#include <sstream>
//
#include "GameConfigInterface.h"
//int psGameClient::connectToGameServer(ps::GameInitInfo* gii)
//{ 
//	
//	
//	Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
//	if (Socket == INVALID_SOCKET || Socket == SOCKET_ERROR)
//		S_LOG::write("Socket Problem");
//
//	//make the socket a non-blocking socket
//	unsigned long on = 1;
//	ioctlsocket(Socket, FIONBIO, &on);
//
//	//We use the address we get in the GAME INIT INFO STRUCT
//	ZeroMemory(&serverAddress, sizeof(serverAddress));
//	serverAddress.sin_port = 12371;//htons(667);
//	serverAddress.sin_family = AF_INET;
//	serverAddress.sin_addr.s_addr = inet_addr(gii->ip.c_str());
//
//	ZeroMemory(&address, sizeof(address));
//	address.sin_port = 12370;//htons(667);
//	address.sin_family = AF_INET;
//	address.sin_addr.s_addr = INADDR_ANY;
//	int result = bind(Socket, (sockaddr*)&address, sizeof(address));
//
//	//make first connection request and wait for a response - everything here is going to start as blocking but not-ideal
//	psNetworking::ClientOutPacket_UDP_ConnectRequest packet;
//	ZeroMemory(&packet, sizeof(psNetworking::ClientOutPacket_UDP));
//	packet.type = 200;
//	std::string screenName = gii->screenName;
//	int screenNameSize = screenName.size();
//	if (screenNameSize > 15) screenNameSize = 15;
//	memcpy(packet.name, screenName.c_str(), screenNameSize + 1);
//	
//
//	int bytesSent = sendto(Socket, (char*)&packet, sizeof(psNetworking::ClientOutPacket_UDP), 0, (sockaddr*)&serverAddress, sizeof(sockaddr)); 
//
//	//now wait for response back from server
//	bool receivedResponse = false;
//	DWORD timeout = 5000;//5s until timeout occurs and issues failure OR press space to cancel
//	static DWORD lastTime;
//	lastTime = GetTickCount();
//	while (GetTickCount() - lastTime < timeout)
//	{
//		
//		psNetworking::ServerOutPacket_UDP_ConnectRequestResponse packet;
//		recv(Socket, (char*)&packet, sizeof(psNetworking::ServerOutPacket_UDP), 0);
//		if (packet.type == 200)
//		{
//			if (packet.response == 0)//we can join
//			{
//				//here we fill the GII struct with what we need so that the game can be initialized 
//
//				//set gii client index 
//				clientIndex = packet.index;
//				gii->clientActorIndex = clientIndex;   
//
//				//set gii level path
//				char lp[64]; 
//				std::stringstream mapStringStream;
//				mapStringStream.str("");
//				for (int i = 0; i < packet.mapPathSize; i++)
//				{
//					mapStringStream<<packet.mapPath[i];
//				}
//				std::string levelPath = mapStringStream.str().c_str();
//				S_LOG::write(levelPath.c_str());
//				gii->levelPath = levelPath.c_str();
//				gii->type = 0;//packet.gameMode;
//				return 0;
//			}
//			if (packet.response == 1)//server is full
//			{
//				return 1;
//			}
//		}
//	}
//	//if the program has gotten here, we've experienced a timeout.
//
//	return -1;
//
//}
//
//
//void psGameClient::update(ps::GameData* pGameData)
//{
//	//this is where the game client will receive incoming packets of information from the server
//	//and then update what it needs to.
//	psNetworking::ServerOutPacket_UDP briefPacket;
//	briefPacket.type = 0;
//	int bytesReceived = recv(Socket, (char*)&briefPacket, sizeof(psNetworking::ServerOutPacket_UDP), 0);
//	//IMPORTANT... we dont want to update there is nothing to receive.
//	if (bytesReceived < 1)
//	{
//		return;
//	}
//	
//	//first thing is to check the packet type and then act accordingly
//	if (briefPacket.type == 0)
//	{
//		
//		psNetworking::ServerOutSnapshot* packet = (psNetworking::ServerOutSnapshot*)&briefPacket;
//		pGameData->gameState = packet->gameState;
//
//		for (int i = 0; i < 8; i++)
//		{
//				//IMPORTANT, we dont want to update the client actor just yet. I know trusting the client
//				//is dangerous, but its just for now. We'll figure something else out later on.
//				
//				
//				//There are some things that the client gives to the server (position, direction, gravity)
//				//others that client gets from server (health, shield, kills, deaths, score, modespec1, modespec2)
//				
//				
//				ps::Actor* actor = pGameData->pActorAtIndex(i);
//
//				actor->state = packet->actors[i].state;
//
//				if (i != clientIndex || packet->actors[i].state == 2)
//				{
//					actor->x = packet->actors[i].x;
//					actor->y = packet->actors[i].y;
//					actor->z = packet->actors[i].z;
//
//					actor->view()->dx = packet->actors[i].dirx;
//					actor->view()->dy = packet->actors[i].diry;
//					actor->view()->dz = packet->actors[i].dirz;
//					actor->view()->ux = 0.0f;
//					actor->view()->uy = 0.0f;
//					actor->view()->uz = 0.0f;
//
//					actor->gravityField = packet->actors[i].gravity;
//					if (actor->gravityField == 0) actor->view()->ux = 1.0f;
//					if (actor->gravityField == 1) actor->view()->ux = -1.0f;
//					if (actor->gravityField == 2) actor->view()->uy = 1.0f;
//					if (actor->gravityField == 3) actor->view()->uy = -1.0f;
//					if (actor->gravityField == 4) actor->view()->uz = 1.0f;
//					if (actor->gravityField == 5) actor->view()->uz = -1.0f;
//
//					actor->dx = 0.0f;
//					actor->dy = 0.0f;
//					actor->dz = 0.0f; 
//				}
//				
//				actor->health = packet->actors[i].health;
//				actor->shield = packet->actors[i].shield;
//
//				//if the client receives more kills than it has, it means its gotten a kill and should be notified.
//				if (i == clientIndex && packet->actors[i].kills > actor->kills)
//				{
//					psHUD::award((packet->actors[i].kills - actor->kills) * 100, L"Kill");
//				}
//				actor->kills = packet->actors[i].kills;
//				actor->deaths = packet->actors[i].deaths;
//				actor->score = packet->actors[i].score;
//
//				
//				actor->modeSpecificVal1 = packet->actors[i].modeSpecificVal1;
//				actor->modeSpecificVal2 = packet->actors[i].modeSpecificVal2;
//
//				actor->renderSync();
//		}
//	}
//
//
//	if (briefPacket.type == PSNETWORKING_SERVEROUT_DISPLAYNAMEUPDATE)
//	{
//		psNetworking::ServerOutPacket_UDP_DisplayNameUpdate* packet = (psNetworking::ServerOutPacket_UDP_DisplayNameUpdate*)&briefPacket;
//
//		for (int i = 0; i < 8; i++)
//		{
//			char playerNameChars[16]; 
//			std::stringstream playerStringStream;
//			playerStringStream.str("");
//			memcpy(playerNameChars, packet->name[i], 16);
//			for (int j = 0; j < 16; j++)
//			{
//				playerStringStream<<packet->name[i][j];
//			}
//			std::string playerName = playerStringStream.str();
//			std::wstring wPlayerName(playerName.begin(), playerName.end());
//			pGameData->pActorAtIndex(i)->setName(wPlayerName.c_str());
//		}
//	}
//}
//
//#include <sstream>
//
//bool psGameClient::sendActorStatePacket(ps::Actor* actor)
//{
//	psNetworking::ClientOutPacket_PlayerUpdate_UDP packet;
//	packet.type = 0;
//
//	packet.actor.index = clientIndex;
//
//	packet.actor.x = actor->x;
//	packet.actor.y = actor->y;
//	packet.actor.z = actor->z;
//
//	packet.actor.dirx = actor->view()->dx;
//	packet.actor.diry = actor->view()->dy;
//	packet.actor.dirz = actor->view()->dz;
//
//	packet.actor.gravity = actor->gravityField;
//	int bytesSent = sendto(Socket, (char*)&packet, sizeof(psNetworking::ClientOutPacket_PlayerUpdate_UDP), 0, (sockaddr*)&serverAddress, sizeof(sockaddr));
//	return true;
//}
//
//void psGameClient::sendImmediateShotPacket(unsigned char shooterIndex, unsigned char victimIndex, unsigned char dmg)
//{
//	psNetworking::ClientOutPacket_UDP packet;
//	packet.type = 1;
//
//	packet.data[0] = shooterIndex;
//	packet.data[1] = victimIndex;
//	packet.data[2] = dmg;
//
//	int bytesSent = sendto(Socket, (char*)&packet, sizeof(psNetworking::ClientOutPacket_UDP), 0, (sockaddr*)&serverAddress, sizeof(sockaddr));
//}

SGameClient* SGameClient::_singleton = 0;

SGameClient::SGameClient(void)
{
	_mainSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (_mainSocket == INVALID_SOCKET || _mainSocket == SOCKET_ERROR)
		S_LOG::write("GAME CLIENT: SOCKET CREATION PROBLEM!");

	unsigned long on = 1;
	ioctlsocket(_mainSocket, FIONBIO, &on);

	ZeroMemory(&_localAddress, sizeof(_localAddress));
	_localAddress.sin_port = 12370;
	_localAddress.sin_family = AF_INET;
	_localAddress.sin_addr.s_addr = INADDR_ANY;
	int result = bind(_mainSocket, (sockaddr*)&_localAddress, sizeof(_localAddress));


	ZeroMemory(&_centralServerAddress, sizeof(_centralServerAddress));
	_centralServerAddress.sin_port = 12373;
	_centralServerAddress.sin_family = AF_INET;
	_centralServerAddress.sin_addr.s_addr = inet_addr(GlobalConfiguration::getSingleton()->str_netAddress().c_str());

	_inProcessOfConnecting = false;
}

SGameClient::~SGameClient(void)
{
}

SGameClient* SGameClient::getSingleton(void)
{
	if (_singleton == 0)
		_singleton = new SGameClient();

	return _singleton;
}

bool SGameClient::connectToGameServer(ps::GameInitInfo* gii)
{
	_lastConnectCalled = GetTickCount();
	//We use the address we get in the GAME INIT INFO STRUCT
	ZeroMemory(&_gameServerAddress, sizeof(_gameServerAddress));
	_gameServerAddress.sin_port = gii->port;
	_gameServerAddress.sin_family = AF_INET;
	_gameServerAddress.sin_addr.s_addr = inet_addr(gii->ip.c_str());

	

	//make first connection request and wait for a response - everything here is going to start as blocking but not-ideal
	psNetworking::ClientOutPacket_UDP_ConnectRequest packet;
	ZeroMemory(&packet, sizeof(psNetworking::ClientOutPacket_UDP));
	packet.type = 200;
	std::string screenName = gii->screenName;
	int screenNameSize = screenName.size();
	if (screenNameSize > 15) screenNameSize = 15;
	memcpy(packet.name, screenName.c_str(), screenNameSize + 1);
	

	int bytesSent = sendto(_mainSocket, (char*)&packet, sizeof(psNetworking::ClientOutPacket_UDP), 0, (sockaddr*)&_gameServerAddress, sizeof(_gameServerAddress)); 

	

	_inProcessOfConnecting = true;
	return false;
}

int SGameClient::updateConnection(void)
{
	if (!_inProcessOfConnecting) return SGAMECLIENT_UPDATECONNECTION_NOTHING;


	DWORD timeout = 10000;//5s until timeout occurs and issues failure OR press space to cancel
	DWORD resendPacketTime = 2000;
	static DWORD lastResendTime = GetTickCount();

	psNetworking::ServerOutPacket_UDP_ConnectRequestResponse packet;
	int bytesReceived = recv(_mainSocket, (char*)&packet, sizeof(psNetworking::ServerOutPacket_UDP), 0);

	if (bytesReceived > 0) 
	{
		if (packet.type == 200 && packet.response == 0)//we can join
		{
			//here we fill the GII struct with what we need so that the game can be initialized 

			//set gii client index 
			_clientIndex = packet.index;
			_lastGii.clientActorIndex = _clientIndex;   

			//set gii level path
			char lp[64]; 
			std::stringstream mapStringStream;
			mapStringStream.str("");
			for (int i = 0; i < packet.mapPathSize; i++)
			{
				mapStringStream<<packet.mapPath[i];
			}
			std::string levelPath = mapStringStream.str().c_str();
			S_LOG::write(levelPath.c_str());
			_lastGii.levelPath = levelPath.c_str();
			_lastGii.type = 0;//packet.gameMode;

			_inProcessOfConnecting = false;
			return SGAMECLIENT_UPDATECONNECTION_SUCCESSFULJOIN;
		}
		if (packet.response == 1)//server is full
		{
			_inProcessOfConnecting = false;
			return SGAMECLIENT_UPDATECONNECTION_LOBBYFULL;
		}
		
	} else
	{
		if (GetTickCount() - _lastConnectCalled > timeout)
		{
			_inProcessOfConnecting = false;
			return SGAMECLIENT_UPDATECONNECTION_NORESPONSE;
		}
		if (GetTickCount() - lastResendTime > resendPacketTime)
		{
			//ALSO resend a nat punchthrough request for the game server
			sendNATPunchthroughRequest(_gameServerAddress.sin_addr.S_un.S_addr, _gameServerAddress.sin_port);
			//if its been a second and we havent received anything, lets resend a connection packet
			psNetworking::ClientOutPacket_UDP_ConnectRequest packet;
			ZeroMemory(&packet, sizeof(psNetworking::ClientOutPacket_UDP));
			packet.type = 200;
			std::string screenName = _lastGii.screenName;
			int screenNameSize = screenName.size();
			if (screenNameSize > 15) screenNameSize = 15;
			memcpy(packet.name, screenName.c_str(), screenNameSize + 1);
	
			lastResendTime = GetTickCount();
			int bytesSent = sendto(_mainSocket, (char*)&packet, sizeof(psNetworking::ClientOutPacket_UDP), 0, (sockaddr*)&_gameServerAddress, sizeof(_gameServerAddress)); 
		}
	}
	
	return SGAMECLIENT_UPDATECONNECTION_STILLTRYING;
}

void SGameClient::cancelConnecting(void)
{
	_inProcessOfConnecting = false;
}
void SGameClient::hostServer(ps::GameInitInfo* gii)
{
	_gameServerAddress.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	_gameServerAddress.sin_port = 12371;
	_gameServerAddress.sin_family = AF_INET;

	gii->clientActorIndex = 0;
	_clientIndex = 0;
	SGameServerInitInfo* serverParameters = new SGameServerInitInfo();
	serverParameters->timeLimit = gii->timeLimit;
	serverParameters->scoreLimit = gii->scoreLimit;
	serverParameters->preMatchTime = gii->prematchTime;
	std::string hostScreenName = GlobalConfiguration::getSingleton()->str_screenName();
	memcpy(serverParameters->hostName, hostScreenName.c_str(), hostScreenName.size() + 1);
	std::string mapPath = "../Resources/Levels/Birth/birth.psm";
	memcpy(serverParameters->mapPath, mapPath.c_str(), mapPath.size() + 1);
	std::string lobbyName = gii->lobbyName;
	memcpy(serverParameters->lobbyName, lobbyName.c_str(), lobbyName.size() + 1);

	CreateThread( NULL, 0, SGameServerThread, (void*)serverParameters, 0, NULL);
}
void SGameClient::update(ps::GameData* pGameData)
{
	//this is where the game client will receive incoming packets of information from the server
	//and then update what it needs to.
	psNetworking::ServerOutPacket_UDP briefPacket;
	briefPacket.type = 0;
	int bytesReceived = recv(_mainSocket, (char*)&briefPacket, sizeof(psNetworking::ServerOutPacket_UDP), 0);
	//IMPORTANT... we dont want to update there is nothing to receive.
	if (bytesReceived < 1)
	{
		return;
	}
	
	//first thing is to check the packet type and then act accordingly
	if (briefPacket.type == 0)
	{
		
		psNetworking::ServerOutSnapshot* packet = (psNetworking::ServerOutSnapshot*)&briefPacket;
		pGameData->gameState = packet->gameState;

		pGameData->setMinutesLeft(packet->val1);
		pGameData->setSecondsLeft(packet->val2);

		for (int i = 0; i < 8; i++)
		{
				//IMPORTANT, we dont want to update the client actor just yet. I know trusting the client
				//is dangerous, but its just for now. We'll figure something else out later on.
				
				
				//There are some things that the client gives to the server (position, direction, gravity)
				//others that client gets from server (health, shield, kills, deaths, score, modespec1, modespec2)
				
				
				ps::Actor* actor = pGameData->pActorAtIndex(i);

				actor->state = packet->actors[i].state;

				if (i != _clientIndex || packet->actors[i].state == 2)
				{
					actor->x = packet->actors[i].x;
					actor->y = packet->actors[i].y;
					actor->z = packet->actors[i].z;

					actor->view()->dx = packet->actors[i].dirx;
					actor->view()->dy = packet->actors[i].diry;
					actor->view()->dz = packet->actors[i].dirz;
					actor->view()->ux = 0.0f;
					actor->view()->uy = 0.0f;
					actor->view()->uz = 0.0f;

					actor->gravityField = packet->actors[i].gravity;
					if (actor->gravityField == 0) actor->view()->ux = 1.0f;
					if (actor->gravityField == 1) actor->view()->ux = -1.0f;
					if (actor->gravityField == 2) actor->view()->uy = 1.0f;
					if (actor->gravityField == 3) actor->view()->uy = -1.0f;
					if (actor->gravityField == 4) actor->view()->uz = 1.0f;
					if (actor->gravityField == 5) actor->view()->uz = -1.0f;

					actor->dx = 0.0f;
					actor->dy = 0.0f;
					actor->dz = 0.0f;

					//Now we set other actor properties accordingly against state2, BIT OPERATIONS :)
					actor->isMoving_f = (packet->actors[i].state2 & 2 ^ 7);
					actor->isSprinting = (packet->actors[i].state2 & 2 ^ 6);
					actor->isMoving_b = (packet->actors[i].state2 & 2 ^ 5);
					actor->isMoving_l = (packet->actors[i].state2 & 2 ^ 4);
					actor->isMoving_r = (packet->actors[i].state2 & 2 ^ 3);
					actor->isJumping = (packet->actors[i].state2 & 2 ^ 2);
					actor->isFiring = (packet->actors[i].state2 > 0) ? true : false;//(packet->actors[i].state2 & (2 ^ 1));
					actor->isCrouching = (packet->actors[i].state2 & 2 ^ 0);
				}
				
				actor->health = packet->actors[i].health;
				actor->shield = packet->actors[i].shield;

				//if the client receives more kills than it has, it means its gotten a kill and should be notified.
				if (i == _clientIndex && packet->actors[i].kills > actor->kills)
				{
					psHUD::award((packet->actors[i].kills - actor->kills) * 100, L"Kill");
				}
				actor->kills = packet->actors[i].kills;
				actor->deaths = packet->actors[i].deaths;
				actor->score = packet->actors[i].score;

				
				actor->flux = (int)packet->actors[i].flux * 100;
				

				actor->renderSync();
		}

		//Read object data
		for (int i = 0; i < 4; i++)
		{
			if (packet->objects[i].id == 0) continue;

			
			pGameData->addOrModifyObject(&packet->objects[i]);
		}
	}


	if (briefPacket.type == PSNETWORKING_SERVEROUT_DISPLAYNAMEUPDATE)
	{
		psNetworking::ServerOutPacket_UDP_DisplayNameUpdate* packet = (psNetworking::ServerOutPacket_UDP_DisplayNameUpdate*)&briefPacket;

		for (int i = 0; i < 8; i++)
		{
			char playerNameChars[16]; 
			std::stringstream playerStringStream;
			playerStringStream.str("");
			memcpy(playerNameChars, packet->name[i], 16);
			for (int j = 0; j < 16; j++)
			{
				playerStringStream<<packet->name[i][j];
			}
			std::string playerName = playerStringStream.str();
			std::wstring wPlayerName(playerName.begin(), playerName.end());
			pGameData->pActorAtIndex(i)->setName(wPlayerName.c_str());
		}
	}
}

bool SGameClient::sendActorStatePacket(ps::Actor* actor)
{
	psNetworking::ClientOutPacket_PlayerUpdate_UDP packet;
	packet.type = 0;

	packet.actor.index = _clientIndex;

	packet.actor.x = actor->x;
	packet.actor.y = actor->y;
	packet.actor.z = actor->z;

	packet.actor.dirx = actor->view()->dx;
	packet.actor.diry = actor->view()->dy;
	packet.actor.dirz = actor->view()->dz;

	packet.actor.gravity = actor->gravityField;
	packet.actor.state2 = 0;
	//Now we fill the state2 thing per bit (animations and shit)
	if (actor->isMoving_f) packet.actor.state2 += 2 ^ 7;
	if (actor->isSprinting) packet.actor.state2 += 2 ^ 6;
	if (actor->isMoving_b) packet.actor.state2 += 2 ^ 5;
	if (actor->isMoving_l) packet.actor.state2 += 2 ^ 4;
	if (actor->isMoving_r) packet.actor.state2 += 2 ^ 3;
	if (actor->isJumping) packet.actor.state2 += 2 ^ 2;
	if (actor->isFiring) packet.actor.state2 += 2 ^ 1;
	if (actor->isCrouching) packet.actor.state2 += 2 ^ 0;

	int bytesSent = sendto(_mainSocket, (char*)&packet, sizeof(psNetworking::ClientOutPacket_PlayerUpdate_UDP), 0, (sockaddr*)&_gameServerAddress, sizeof(sockaddr));
	return true;
}

void SGameClient::sendImmediateShotPacket(unsigned char shooterIndex, unsigned char victimIndex, unsigned char dmg)
{
	psNetworking::ClientOutPacket_UDP packet;
	packet.type = 1;

	packet.data[0] = shooterIndex;
	packet.data[1] = victimIndex;
	packet.data[2] = dmg;

	int bytesSent = sendto(_mainSocket, (char*)&packet, sizeof(psNetworking::ClientOutPacket_UDP), 0, (sockaddr*)&_gameServerAddress, sizeof(sockaddr));
}

void SGameClient::sendAbilityUsePacket(SAbility* ability, ps::Actor* actor)
{
	
	psNetworking::ClientOutPacket_PlayerUseAbility_UDP packet;
	packet.type = PSNETWORKING_CLIENTOUT_USEABILITY;
	packet.index = _clientIndex;
	packet.posX = actor->view()->x;
	packet.posY = actor->view()->y;
	packet.posZ = actor->view()->z;

	packet.dirX = actor->view()->dx;
	packet.dirY = actor->view()->dy;
	packet.dirZ = actor->view()->dz;

	std::string abilityName = ability->getName();
	
	if (abilityName.size() < 20)
		memcpy(packet.abilityName, abilityName.c_str(), abilityName.size() + 1);
	else
		memcpy(packet.abilityName, abilityName.c_str(), 20);

	int bytesSent = sendto(_mainSocket, (char*)&packet, sizeof(psNetworking::ClientOutPacket_UDP), 0, (sockaddr*)&_gameServerAddress, sizeof(sockaddr));
}
int SGameClient::getClientIndex(void)
{
	return _clientIndex;
}

ps::GameInitInfo* SGameClient::getLastGameInitInfo(void)
{
	return &_lastGii;
}
void SGameClient::sendNATPunchthroughRequest(unsigned int ip, unsigned short port)
{
	//This function will send over a punchthrough request with this own socket in order to potentially
	//open up the target server's NAT device to this clients external ip and port.

	psNetworking::SimBioticNetClientOut_UDP_PunchthroughRequest packet;
	packet.type = 0;
	packet.targetIP = ip;
	packet.targetPort = port;
	
	int bytesSent = sendto(_mainSocket, (const char*)&packet, sizeof(packet), 0, (sockaddr*)&_centralServerAddress, sizeof(_centralServerAddress));

	sockaddr_in test;
	test.sin_addr.S_un.S_addr = packet.targetIP;
	S_LOG::write(inet_ntoa(test.sin_addr));
}