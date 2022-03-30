//NPCπÈ Ùπ‹¿Ì
#ifndef _LC_SERVER_NPC_OWNER_LIST_H
#define _LC_SERVER_NPC_OWNER_LIST_H

#include "LC_ActorOwnerListBase.h"
#include "LC_ServerPlayerManager.h"

class LC_ServerPlayerManager;
class LC_ServerNPCOwnerList : public GameLogic::LC_ActorOwnerListBase
{
public:
	LC_ServerNPCOwnerList();
	virtual ~LC_ServerNPCOwnerList();

	virtual void AddOwnerID(object_id_type lOwnerID, bool bConsiderGroupMember = true);
	virtual void Update(float fCurrentTime,int lNPCMapLogicID,const Utility::UT_Vec3Int& rkNPCLocation,bool bBeforDead,bool bCheckDistance);

	void AddOwnerList(const object_id_set& ids);
	void AddDamageID(object_id_type id);
	void GetDamageList(object_id_set& id);
	
	void ResolveTotalLevel(GameLogic::LC_MapBase*);

protected:
	
	bool	_isDistanceAndStateValidToOwn(LC_ServerPlayerManager* pkPlayerManager,object_id_type lPlayerID,int lNPCMapLogicID,const Utility::UT_Vec3Int& rkNPCLocation,bool bCheckDead,float fCheckDistance);
};

#endif