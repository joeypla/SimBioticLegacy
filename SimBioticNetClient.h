#ifndef SSIMBIOTICNETCLIENT
#define SSIMBIOTICNETCLIENT

#include <WinSock2.h>

#include <string>
#include <map>
#define SIMBIOTICNET_PACKETTYPE_BASEINFO 0
#define SIMBIOTICNET_PACKETTYPE_LOBBYUPDATE 1

#define SIMBIOTICNET_CLIENTOUT_PUNCHTHROUGHREQUEST 0
#define SIMBIOTICNET_SERVEROUT_PUNCHTHROUGHREQUEST 236
struct SimBioticNetLobby
{
	//56 bytes in 32 bitmode

	char name[32];
	char creatorName[16];
	unsigned int ipAddress;
	char playerCount;
	char maxPlayerCount;
	unsigned short port;
};

//total of 1024 bytes per tcp packet
struct SimBioticNetServerOut
{
	int type;
	char data[1020];
};
struct SimBioticNetServerOut_NetInfo
{
	int type;
	int lobbyCount;
	int playerCount;
	char infoMessage[512];

	char data[500];//filler data
};
struct SimBioticNetServerOut_LobbyList
{
	int type;
	int lobbyCount;
	SimBioticNetLobby lobbies[15];
	char data[176];//filler data
};
struct SimBioticNetClientOut_PunchthroughRequest
{
	int type;
	unsigned int targetIP;
	unsigned int clientIP;//this actually doesn't really matter
	char data[1012];
};

struct SimBioticLobby
{
	std::string name;
	std::string creatorName;
	unsigned int ipAddress;
	unsigned short port;
	char playerCount;
	char maxPlayerCount;
};
class SimBioticNetClient
{
public:
	SimBioticNetClient(void);
	~SimBioticNetClient(void);

	bool netConnect(void);
	bool getConnectionStatus(void);

	void sendNatPunchthroughRequest(unsigned int ipAddress);
	void sendNatPunchthroughRequest(std::string ipAddress);

	bool getInternetLobbiesChanged(void);
	std::map<int, SimBioticLobby> &getInternetLobbies(void);
	int getLobbyCount(void);

	void update(void);

	std::string getDailyMessage(void);
	bool getDailyMessageChanged(void);
private:
	SOCKET _clientSocket;
	sockaddr_in _serverAddress;


	bool internetLobbiesChanged;
	int lobbyCount;
	std::map<int, SimBioticLobby> internetLobbies;

	bool dailyMessageChanged;
	std::string dailyMessage;

protected:
};

#endif