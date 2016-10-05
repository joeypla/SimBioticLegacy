#ifndef SMATERIALSET_H
#define SMATERIALSET_H

#include <string>

#include "SMaterial.h"
class SMaterialSet{
	

public:
	SMaterialSet(std::wstring path);
	SMaterialSet(int materialCount);

	int iMaterials();
	SMaterial* materialAtIndex(int index);
private:
	int _iMaterials;
	SMaterial** _materials;
};
#endif