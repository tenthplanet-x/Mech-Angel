//服务器玩家管理器　负责分配和回收玩家对象
#ifndef _LC_SERVER_PLAYER_MANAGER_H
#define _LC_SERVER_PLAYER_MANAGER_H

#include "LC_PlayerManagerBase.h"
#include "LC_Define.h"
#include "LC_ServerPlayer.h"
#include "UT_MemoryPool.h"

class NW_ServerSession;

//using namespace Buddy;
class BuddyInstance;
namespace Utility
{
	class UT_TimeManager;
};
using namespace GameLogic;

typedef mem::map<uint32_t, object_id_set>	LC_IP2IDSIndices;

typedef mem::map<unique_id_type, LC_ServerPlayer*> LC_PlayerUIDPtrMap;

class LC_ServerPlayerManager : public LC_PlayerManagerBase
{
public:
	LC_ServerPlayerManager();
	~LC_ServerPlayerManager();

	virtual void DestroyPlayer(LC_PlayerBase* pkPlayer);

	virtual void Update(float fCurrentTime,float fDeltaTime);
	LC_ServerPlayer *SummorControlPlayer(const unique_id_impl& charID, logic_id_type iMapLogicID, const Utility::UT_Vec3Int& pos, uint32_t bits, bool isRobot=false, LC_ServerPlayer* Owner=NULL, user_id_type userID = 0);
	LC_ServerPlayer *NewSummorControlPlayer(const unique_id_impl& charID, uint32_t bits, bool isRobot=false, LC_ServerPlayer* Owner=NULL, user_id_type userID = 0);
	virtual void BroadCast(uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody = NULL, GameLogic::LC_PlayerOperator* pkFilter=NULL);

	int GetControllerPlayerCount();
	inline int GetPlayerCount(void){return (int)m_kPlayerIDPtrMap.size();}
	inline int GetInGamePlayerCount(void){return m_iInGamePlayerCount;}
	mem::vector<LC_ServerPlayer*> GetAllPlayerPtr(void); //disgusting!!! use it carefully!!!
	void GetAllPlayerCharID(unique_id_set& charIDs);

	int GetInGamePlayerCountLessLevel(int level);

	LC_ServerPlayer* CreatePlayerBySession(user_id_type userID, const unique_id_impl& charID, NW_ServerSession* session);
	LC_ServerPlayer* GetPlayerBySession(user_id_type userID, const unique_id_impl& charID, NW_ServerSession* session );

	//LC_ServerPlayer* CreatePlayerByBuddy(user_id_type userID, const unique_id_impl& charID, BuddyInstance* pBuddy);

	StringType		GetRTInfo(void);

	void OnPlayerLoginIPSet(LC_PlayerBase* pkPlayer, uint32_t preIP, uint32_t postIP);
	uint32_t GetOnlineIPCount() const { return (uint32_t)m_kIP2IDSIndices.size(); }
	uint32_t GetOnlinePlayerCount() const;

	const object_id_set& GetIDSetAcrdLoginIP(uint32_t ip);
	void PostPlayerOnline(LC_PlayerBase* pkPlayer);
	void PrePlayerOffline(LC_PlayerBase* pkPlayer);
protected:
	virtual LC_PlayerBase* _allocPlayer();
	virtual void		   _freePlayer(LC_PlayerBase* pkPlayer);
	Utility::UT_MemoryPool<LC_ServerPlayer>*		m_pkServerPlayerPool;

	LC_IP2IDSIndices	m_kIP2IDSIndices;

	float		 m_fLastUpdateTime;
	float		 m_fLastReportTime;
	int			 m_iFreshmanCount;

	Utility::UT_Timer	m_kUpdateTimer;

	//客户端验时
	float		 m_fLastClientCheckTime;
	int			 m_iInGamePlayerCount;	
private:
};
#endif
