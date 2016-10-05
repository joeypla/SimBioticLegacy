#ifndef SYMBIOTEPACKETSTRUCTS_H
#define SYMBIOTEPACKETSTRUCTS_H

//packet type definitions
#define PSNETWORKING_SERVEROUT_UPDATE 0
#define PSNETWORKING_SERVEROUT_SPAWN  1
#define PSNETWORKING_SERVEROUT_CONNECTIONREQUESTRESPONSE 200
#define PSNETWORKING_SERVEROUT_DISPLAYNAMEUPDATE 201

#define PSNETWORKING_CLIENTOUT_UPDATE 0
#define PSNETWORKING_CLIENTOUT_HIT    1
#define PSNETWORKING_CLIENTOUT_USEABILITY 2
#define PSNETWORKING_CLIENTOUT_CONNECTIONREQUEST 200
#define PSNETWORKING_CLIENTOUT_DISCONNECT 201

#define PSNETWORKING_SERVEROUT_CONNECTIONREQUESTRESPONSE_FULL 1
#define PSNETWORKING_SERVEROUT_CONNECTIONREQEUSTRESPONSE_OK   0

#define PSNETWORKING_SERVEROUT_PLAYERSTATE_EMPTY 0
#define PSNETWORKING_SERVEROUT_PLAYERSTATE_PLAYING 1
#define PSNETWORKING_SERVEROUT_PLAYERSTATE_SPAWNING 2

#define PSNETWORKING_CENTRALCOM_SERVEROUT_PRESENCE 0
#define PSNETWORKING_CENTRALCOM_SERVEROUT_NATPUNCHTHROUGH 1
#define PSNETWORKING_CENTRALCOM_NETOUT_PUNCHTHROUGHREQUEST 236
namespace psNetworking
{
	struct ServerOutCentralComPresence
	{
		//56 bytes
		unsigned int specialSignatureNumber;
		char name[32];
		char creatorName[16];
		char playerCount;
		char maxPlayerCount;
		char data[2];
	};
	struct SimBioticNetServerOut_PunchthroughRequest
	{
		//here we want a total of 48 Bytes so that it can match a typical
		//client out UDP packet. This way we can reuse the same socket on the lobby side.
		unsigned char type;
		unsigned char data1[3];
		unsigned int clientIP;
		unsigned short clientPort;
		char data2[38];
	};
	struct ClientOutActor{
		//COMPILER: 44 BYTES
		
		unsigned char state;//state should always be 1 for now, client shouldnt be able to tell server if connected
		unsigned char index;
		unsigned char state2;//bit1 = isRunningForward, bit2 = isSprinting, bit3 = isRunningBackward, bit4 = isStrafingLeft, 
		//bit5 =  isStrafingRight, bit6 = isJumping, bit7 = isShooting, bit8 = isCrouching
		unsigned char gravity;
		float x;
		float y;
		float z;
		float dirx;
		float diry;
		float dirz;
	};
	struct ServerOutActor{
		//COMPILER: 36 BYTES
		
		
		//8 Bytes
		unsigned char state;
		unsigned char health;
		unsigned char shield;
		unsigned char kills;
		unsigned char deaths; 
		unsigned char gravity;
		unsigned char flux;
		unsigned char state2;//bit1 = isRunningForward, bit2 = isSprinting, bit3 = isRunningBackward, bit4 = isStrafingLeft, 
		//bit5 =  isStrafingRight, bit6 = isJumping, bit7 = isShooting, bit8 = isCrouching
		//4 Bytes
		unsigned int score;

		//24 Bytes
		float x;
		float y;
		float z;
		float dirx;
		float diry;
		float dirz;
	};

	struct ServerOutGameObject
	{
		unsigned int id;
		unsigned char objectTemplate;
		unsigned char owner;
		unsigned char data[2];
		float x;
		float y;
		float z;
		float dx;
		float dy;
		float dz;
		float ax;
		float ay;
		float az;//44 Bytes

	};
	struct ServerOutSnapshot{
		//COMPILER: 492
		unsigned char type;
		unsigned char gameState;//playing, paused, prematch waiting, over etc.
		unsigned char val1;
		unsigned char val2;
		ServerOutActor actors[8];
		ServerOutGameObject objects[4];
		float gameModeVal1;
		float gameModeVal2;
		float gameModeVal3;

		int score1;
		int score2;

		unsigned char gameModeVal4;
		unsigned char gameModeVal5;
		unsigned char gameModeVal6;
		unsigned char gameModeVal7;
	};
	struct ServerOutPacket_UDP_ConnectRequestResponse
	{
		unsigned char type;
		unsigned char response;
		unsigned char index;
		unsigned char mapPathSize;

		unsigned char mapPath[64];
		unsigned char gameMode;
		unsigned char spawnDelay;
		unsigned char preMatchTime;

		unsigned int scoreLimit;
		unsigned int timeLimit;

	};
	struct ServerOutPacket_UDP_DisplayNameUpdate{
		unsigned char type;
		unsigned char data[7];

		unsigned char name[8][16];
		unsigned char data2[180];
	};

	struct ClientOutPacket_UDP_ConnectRequest
	{
		//COMPILER: 48 BYTES
		unsigned char type;
		unsigned char data[7];
		unsigned char name[16];
		unsigned char data2[24];
	};
	struct ClientOutPacket_PlayerUpdate_UDP{
		//COMPILER: 48 BYTES
		unsigned char type; 
		ClientOutActor actor;
	};
	struct ClientOutPacket_PlayerUseAbility_UDP
	{
		unsigned char type;
		unsigned char index;
		unsigned char data[2];

		char abilityName[20];

		float posX;
		float posY;
		float posZ;

		float dirX;
		float dirY;
		float dirZ;
	};
	struct ServerOutPacket_UDP_PlayerRespawn{
		unsigned char type;
		unsigned char data[3];
		unsigned int setPointIndex;
		unsigned char data2[308];
	};
	struct ServerOutPacket_UDP{
		//COMPILER: 316
		unsigned char type;
		unsigned char data[491]; 
	};

	struct ClientOutPacket_UDP{
		//COMPILER: 48 BYTES
		unsigned char type;
		unsigned char data[47];
	};

	struct SimBioticNetClientOut_UDP_PunchthroughRequest
	{
		unsigned int type;
		unsigned int targetIP;
		unsigned short targetPort;
		char data[46];
	};
}
			
#endif