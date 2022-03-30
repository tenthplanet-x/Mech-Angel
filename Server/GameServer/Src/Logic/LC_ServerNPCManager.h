//服务器专用NPC管理器
#ifndef _LC_SERVER_NPC_MANAGER_H
#define _LC_SERVER_NPC_MANAGER_H

#include "LC_NPCManagerBase.h"
#include "UT_MemoryPool.h"
#include "UT_TimeManager.h"

class LC_ServerNPC;
class LC_ServerNPCManager : public GameLogic::LC_NPCManagerBase
{
public:
	LC_ServerNPCManager();
	virtual ~LC_ServerNPCManager();

	void Update(float fCurrentTime,float fDeltaTime);
	virtual GameLogic::LC_NPCBase*	CreateNPC(object_id_type lID,int lCharType);
	virtual void					DestoryNPC(GameLogic::LC_NPCBase* pkNPC);

	StringType		GetRTInfo(void);

	int OnEnableNPCUpdateFlag( LC_ServerNPC* pNPC );
	int OnDisableNPCUpdateFlag( LC_ServerNPC* pNPC );



protected:
	virtual GameLogic::LC_NPCBase*		_allocNPC();
	virtual void						_freeNPC(GameLogic::LC_NPCBase* pkNPC);

	void _refreshEnableDisableObj();

	Utility::UT_MemoryPool<LC_ServerNPC>*		m_pkServerNPCPool;
	int m_nNPCKilledCount;

	Utility::UT_Timer	m_kUpdateTimer;

	LC_NPCIDPtrMap m_kNPCIDPtrMapCacheForEnable;
	LC_NPCIDPtrMap m_kNPCIDPtrMapCacheForDisable;
};

#endif