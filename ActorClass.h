#ifndef PSACTORCLASS_H
#define PSACTORCLASS_H

#include "Weapon.h"
namespace ps
{
	
	class ActorClass{
		static const int UPGRADE_NONE=                        0;
		static const int UPGRADE_LIGHTWEIGHT=                 1;
		static const int UPGRADE_FASTHANDS=                   2;
		static const int UPGRADE_LIGHTWEIGHT=                 3;
		static const int UPGRADE_FASTHANDS=                   4;
		static const int UPGRADE_LIGHTWEIGHT=                 5;
		static const int UPGRADE_FASTHANDS=                   6;
		static const int UPGRADE_LIGHTWEIGHT=                 7;
		static const int UPGRADE_FASTHANDS=                   8;
		static const int UPGRADE_LIGHTWEIGHT=                 9;
		static const int UPGRADE_FASTHANDS= 10;
		static const int UPGRADE_LIGHTWEIGHT= 11;
		static const int UPGRADE_FASTHANDS= 12;
		ActorClass(void);
		~ActorClass(void);
		
		ps::WeaponBase* weapon1;
		ps::WeaponBase* weapon2;
		int upgrade1;
		int upgrade2;
		int upgrade3;
	};
};
#endif
