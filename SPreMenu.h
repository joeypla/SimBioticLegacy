#ifndef SPREMENU
#define SPREMENU

#include "SInterface.h"
#include "SRender.h"


#include "SAUDIO.h"
class SPreMenu: public S_Interface
{
public:
	SPreMenu(void);
	~SPreMenu(void);
	bool renderToDevice(const LPDIRECT3DDEVICE9 dev);
	bool updateInterface(DWORD lastTimeTaken);
	void receiveSystemMessage(UINT message, UINT param);

	psAudio::Sound* _enthusiastSound;
private:
	void setup(void);
};

#endif