#ifndef SGAMEOBJECT_H
#define SGAMEOBJECT_H

#include "psMath.h"

#define SGAMEOBJECT_NEWINSTANCE_FIREBOLT          10
#define SGAMEOBJECT_NEWINSTANCE_HEALTHGRENADE     11
#define SGAMEOBJECT_NEWINSTANCE_SHIELDGRENADE     12

#include <map>
namespace ps
{
	class GameData;
}

namespace psNetworking
{
	struct ServerOutGameObject;
}

class SGameObject
{
public:
	static void loadGameObjectTemplates(void);
	static void unloadGameObjectTemplates(void);
	
	static SGameObject* new_instance(unsigned int templateNumber, bool isServerInstance, unsigned int ID);
	SGameObject(void);
	SGameObject(SGameObject* original);
	~SGameObject(void);
	unsigned int getID(void);
	unsigned int getTemplateNumber(void);

	virtual void clientUpdate(ps::GameData* pGameData);
	virtual void serverUpdate(void);
	virtual void serverSerializeData(psNetworking::ServerOutGameObject* pData);
	virtual void clientDeserializeData(psNetworking::ServerOutGameObject* pData);

	psMath::float3 position;

	bool getIsDead(void);
private:
	unsigned int ID;
	unsigned int templateNumber;

	static std::map<unsigned int, SGameObject*> _templates;
protected:
	
	bool _isServerInstance;
	bool _isDead;


	
	
};
#endif
