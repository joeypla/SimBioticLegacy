#ifndef GAMECONFIGINTERFACE_H
#define GAMECONFIGINTERFACE_H

#define GLOBALCONFIG_FILENAME "../Resources/Configs/config.cfg"
#define GLOBALCONFIG_DEFAULTNETADDRESS "127.0.0.1"
#define GLOBALCONFIG_DEFAULTSCREENNAME "Soldier"
#include <string>




class GlobalConfiguration
{
public:
	GlobalConfiguration(std::string path);
	~GlobalConfiguration(void);
	static GlobalConfiguration* getSingleton();

	std::string		str_screenName(void);
	std::string		str_netAddress(void);
	int				iMaxPlayers(void);
	float			fPlayerHeight(void);

	unsigned int	iResolutionWidth(void);
	unsigned int	iResolutionHeight(void);
	int				iLightingQuality(void);
	bool			bSSAO(void);
	bool			bMotionBlur(void);
	bool			bDOF(void);

	bool            loadConfiguration(void);
private:
	
	static GlobalConfiguration* _singleton;

	std::string		_str_screenName;
	std::string		_str_netAddress;
	int				_iMaxPlayers;
	float			_fPlayerHeight;

	unsigned int    _iResolutionWidth;
	unsigned int    _iResolutionHeight;
	int             _iLightingQuality;
	bool            _bSSAO;
	bool            _bMotionBlur;
	bool            _bDOF;

protected:
};
#endif