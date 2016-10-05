#include "SInterface.h"

bool S_Interface::renderToDevice(const LPDIRECT3DDEVICE9 dev){return true;}
bool S_Interface::updateInterface(DWORD lastTimeTaken){return true;}
void S_Interface::receiveSystemMessage(UINT message, UINT wParam){}
S_Interface::S_Interface(void){}
S_Interface::~S_Interface(){} 