#ifndef SFUNCS_H
#define SFUNCS_H

namespace SFUNCS{
	//calculates normalized screen space coordinate given backbuffer coord
	float getNSS_X(float x);
	float getNSS_Y(float y);
	//calculates backbuffer coord from screen space coordinate
	float getBBC_X(float x);
	float getBBC_Y(float y);
	//calc backbuffer aspect ratio
	float getAspectRatio(void);
};
#endif