#include "SimBioticNetClient.h"

#include <iostream>
#include <sstream>
//#pragma comment(lib, "Ws2_32.lib")    // WinSock Library

//#include <windows.h>
#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 12373



#include "GameConfigInterface.h"
SimBioticNetClient::SimBioticNetClient(void)
{
	 dailyMessageChanged = false;
	 internetLobbiesChanged = false;

	 _clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	 
	 _serverAddress.sin_family = AF_INET;    // set the address family
     _serverAddress.sin_addr.s_addr = inet_addr(GlobalConfiguration::getSingleton()->str_netAddress().c_str());    // set the IP address
     _serverAddress.sin_port = SERVER_PORT; 

	 
	 
}

SimBioticNetClient::~SimBioticNetClient(void)
{
	shutdown(_clientSocket, 0);
	closesocket(_clientSocket);
}

bool SimBioticNetClient::netConnect(void)
{
	
	//Net connect should pretty much always return false. But it will put this client into
	//a connecting state, and the user can poll this class later on to see if something new
	//has happened.
	int result = connect(_clientSocket, (sockaddr*)&_serverAddress, sizeof(_serverAddress));
	u_long iMode = 1;
	int iResult = ioctlsocket(_clientSocket, FIONBIO, &iMode);
	return (result == 0) ? true : false;
}

void SimBioticNetClient::sendNatPunchthroughRequest(unsigned int ipAddress)
{
	SimBioticNetClientOut_PunchthroughRequest requestPacket;
	requestPacket.type = SIMBIOTICNET_CLIENTOUT_PUNCHTHROUGHREQUEST;

	requestPacket.targetIP = ipAddress;
	requestPacket.clientIP = 0;//this isnt needed right now because the server will know where the packet comes from.

	int bytesSent = send(_clientSocket, (const char*)&requestPacket, sizeof(requestPacket), 0);
}

void SimBioticNetClient::sendNatPunchthroughRequest(std::string ipAddress)
{
	//convert into an unsigned 32bit integer
	//sockaddr_in address;
	//address.sin_addr.S_un.S_addr
	unsigned int ip = inet_addr(ipAddress.c_str());

	SimBioticNetClientOut_PunchthroughRequest requestPacket;
	requestPacket.type = SIMBIOTICNET_CLIENTOUT_PUNCHTHROUGHREQUEST;

	requestPacket.targetIP = ip;
	requestPacket.clientIP = 0;//this isnt needed right now because the server will know where the packet comes from.

	int bytesSent = send(_clientSocket, (const char*)&requestPacket, sizeof(requestPacket), 0);
}

bool SimBioticNetClient::getConnectionStatus(void)
{ 
	return true;
}

void SimBioticNetClient::update(void)
{
	SimBioticNetServerOut packet;
	int bytesReceived = recv(_clientSocket, (char*)&packet, sizeof(packet), 0);

	if (bytesReceived < 1) return;


	//now depending on the packet type, handle accordingly.
	if (packet.type == SIMBIOTICNET_PACKETTYPE_BASEINFO)
	{
		SimBioticNetServerOut_NetInfo* netInfo = (SimBioticNetServerOut_NetInfo*)&packet;
		lobbyCount = netInfo->lobbyCount;

		//now write info message into the string
		std::stringstream infoStream;
		infoStream.str("");
		int index = 0;
		while (netInfo->infoMessage[index] != '\0')
		{
			infoStream<<netInfo->infoMessage[index];
			index++;
			if (index >= 512) break;
		}

		dailyMessage = infoStream.str();
		dailyMessageChanged = true;
	}

	if (packet.type == SIMBIOTICNET_PACKETTYPE_LOBBYUPDATE)
	{
		SimBioticNetServerOut_LobbyList* lobbyListPacket = (SimBioticNetServerOut_LobbyList*)&packet;
		
		
		int lobbyCount = lobbyListPacket->lobbyCount;
		
		for (int i = 0; i < lobbyCount; i++)
		{
			SimBioticLobby lobby;
			lobby.name = lobbyListPacket->lobbies[i].name;
			lobby.creatorName = lobbyListPacket->lobbies[i].creatorName;
			lobby.playerCount = lobbyListPacket->lobbies[i].playerCount;
			lobby.maxPlayerCount = lobbyListPacket->lobbies[i].maxPlayerCount;
			unsigned int ipAddress = lobbyListPacket->lobbies[i].ipAddress;
			lobby.ipAddress = ipAddress;
			lobby.port = lobbyListPacket->lobbies[i].port;
			internetLobbies.insert(std::pair<int, SimBioticLobby>(ipAddress, lobby));
			internetLobbiesChanged = true;
		}
	}
}
std::map<int, SimBioticLobby>& SimBioticNetClient::getInternetLobbies(void)
{
	return internetLobbies;
}
bool SimBioticNetClient::getInternetLobbiesChanged(void)
{
	return internetLobbiesChanged;
}
bool SimBioticNetClient::getDailyMessageChanged(void)
{
	return dailyMessageChanged;
}
std::string SimBioticNetClient::getDailyMessage(void)
{
	dailyMessageChanged = false;
	return dailyMessage;
}



