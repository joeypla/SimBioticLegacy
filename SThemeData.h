#ifndef STHEMEDATA_H
#define STHEMEDATA_H

#include <string>
#include <vector>
#include "SBlockData.h"
class SThemeData
{
public:
	SThemeData(void);
	~SThemeData(void);
private:
	std::string name;
	std::string path;
	std::vector<SBlockData> blocks;

protected:
};
#endif