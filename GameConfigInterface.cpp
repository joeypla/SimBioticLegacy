#include "GameConfigInterface.h"

#include <fstream>
#include <sstream>

GlobalConfiguration* GlobalConfiguration::_singleton = 0;

GlobalConfiguration::GlobalConfiguration(std::string path)
{
	//This constructor is the container for all non-file based config values
	_iMaxPlayers = 8;
	_fPlayerHeight = 2.8f;



	//Now we load the things from the file
	std::ifstream file;
	file.open(path.c_str(), std::ios::binary);
	if (!file.is_open())
	{
		_str_netAddress = GLOBALCONFIG_DEFAULTNETADDRESS;

		std::stringstream ss;
		ss.str("");
		ss<<GLOBALCONFIG_DEFAULTSCREENNAME<<rand() % 100000;

		_str_screenName = ss.str();
	}
}

GlobalConfiguration::~GlobalConfiguration(void)
{
}

bool GlobalConfiguration::loadConfiguration(void)
{
	std::ifstream file;
	file.open(GLOBALCONFIG_FILENAME, std::ios::binary);
	if (!file.is_open()) return false;
	/* read map proper name */
	std::stringstream ss;
	ss.str("");
	unsigned int screenNameSize = 0;
	file.read((char*)&screenNameSize, sizeof(int));
	char single;
	for (int i = 0; i < screenNameSize; i++)
	{
		file.read((char*)&single, sizeof(char));
		ss<<single;
	}

	_str_screenName = ss.str();

	/*read net ip string */
	ss.str("");
	unsigned int netIPStringSize = 0;
	file.read((char*)&netIPStringSize, sizeof(int));
	for (int i = 0; i < netIPStringSize; i++)
	{
		file.read((char*)&single, sizeof(char));
		ss<<single;
	}
	_str_netAddress = ss.str();

	/* read resolution */
	file.read((char*)&_iResolutionWidth, sizeof(int));
	file.read((char*)&_iResolutionHeight, sizeof(int));

	/* read lighting quality */
	char lightingQuality;
	file.read(&lightingQuality, sizeof(char));
	_iLightingQuality = lightingQuality;

	/* ssao , motionblur, DOF, selection */
	file.read((char*)&_bSSAO, sizeof(bool));
	
	file.read((char*)&_bMotionBlur, sizeof(bool));

	file.read((char*)&_bDOF, sizeof(bool));


	file.close();
	return true;
}
GlobalConfiguration* GlobalConfiguration::getSingleton(void)
{
	if (_singleton == 0)
		_singleton = new GlobalConfiguration(GLOBALCONFIG_FILENAME);

	return _singleton;
}

std::string		GlobalConfiguration::str_netAddress(void){return _str_netAddress;}
std::string		GlobalConfiguration::str_screenName(void){return _str_screenName;}
int				GlobalConfiguration::iMaxPlayers(void){return _iMaxPlayers;}
float			GlobalConfiguration::fPlayerHeight(void){return _fPlayerHeight;}
unsigned int    GlobalConfiguration::iResolutionWidth(void){return _iResolutionWidth;}
unsigned int    GlobalConfiguration::iResolutionHeight(void){return _iResolutionHeight;}
int             GlobalConfiguration::iLightingQuality(void){return _iLightingQuality;}
bool            GlobalConfiguration::bSSAO(void){return _bSSAO;}
bool            GlobalConfiguration::bMotionBlur(void){return _bMotionBlur;}
bool            GlobalConfiguration::bDOF(void){return _bDOF;}