#ifndef SYMBIOTE_INTERFACE_H
#define SYMBIOTE_INTERFACE_H
#include <d3d9.h>
class S_Interface{
public:
	S_Interface(void);
	virtual ~S_Interface();
	virtual bool renderToDevice(const LPDIRECT3DDEVICE9 dev);
	virtual bool updateInterface(DWORD lastTimeTaken);
	virtual void receiveSystemMessage(UINT message, UINT wParam);
private:
	
protected:
};

#endif