#include "SFuncs.h"

#include "SConfigs.h"
float SFUNCS::getNSS_X(float x){
	return (float)x/(S_Configs::s_bufferX/2.0f) - 1.0f/(float)(S_Configs::s_bufferX/S_Configs::s_bufferY);
}

float SFUNCS::getNSS_Y(float y){
	return 1.0f-(y/(float)(S_Configs::s_bufferY/2.0f));
}

float SFUNCS::getBBC_X(float x){
	
	//return ((x/(float)(S_Configs::s_bufferX/S_Configs::s_bufferY)) + 1.0f)*(float)(S_Configs::s_bufferX/2.0f);
	return (x + 1.0f)*(float)(S_Configs::s_bufferX/2.0f);
}

float SFUNCS::getBBC_Y(float y){
	return (1.0f - y)*((float)S_Configs::s_bufferY*0.5f);
}

float SFUNCS::getAspectRatio(void){
	return (float)S_Configs::s_bufferX/S_Configs::s_bufferY;
}