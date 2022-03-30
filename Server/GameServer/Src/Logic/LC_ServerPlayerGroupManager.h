//服务器组队管理器
#ifndef _LC_SERVER_PLAYER_GROUP_MANAGER_H
#define _LC_SERVER_PLAYER_GROUP_MANAGER_H

#include "UT_MemoryPool.h"
#include "LC_PlayerGroupManagerBase.h"
#include "LC_ServerPlayer.h"
#include "LC_PlayerGroupBase.h"
#include "LC_PlayerTeamAsset.h"

class LC_ServerPlayerGroup;
class LC_ServerPlayerGroupManager : public GameLogic::LC_PlayerGroupManagerBase
{
public:
	typedef mem::map<unique_id_type, int> Actor_Index_Type;
	LC_ServerPlayerGroupManager();
	virtual ~LC_ServerPlayerGroupManager();

	LC_ServerPlayerGroup*	CreateGroup(LC_ServerPlayer* player, uint32_t nGroupType = GameLogic::E_PlayerTeamType_Local);
	ResultType				Leave(LC_ServerPlayer* player, bool& destory);
	ResultType				DelPlayer(int groupid, const unique_id_impl& lCharID, bool& destory);
	void					Dismiss(LC_ServerPlayerGroup* group);
	bool					UpdatePlayerState(const unique_id_impl& lCharID, int online = PLAYER_ON_LINE);
	//void					PlayerTempLeave(LC_ServerPlayer* player);
	void					PlayerRecover(LC_ServerPlayer* player, int groupid);
	virtual void			Update(float fCurrentTime, float fDeltaTime);
	void					ClearPlayerFromGroup(LC_ServerPlayer* player, uint32_t nGroupID);

	//玩家索引
	bool					AddPlayerIndex(const unique_id_impl& lCharID, int groupid);
	void					DelPlayerIndex(const unique_id_impl& lCharID);
	LC_ServerPlayerGroup*	GetPlayerGroup(const unique_id_impl& lCharID);
	ResultType				leaveGroup(LC_ServerPlayer* player, LC_ServerPlayerGroup* group, bool& destory);

protected:
	virtual GameLogic::LC_PlayerGroupBase*		_allocPlayerGroup();
	virtual void								_freePlayerGroup(GameLogic::LC_PlayerGroupBase* pkPlayerGroup);
	Utility::UT_MemoryPool<LC_ServerPlayerGroup>*	m_pkServerPlayerGroupPool;
	Actor_Index_Type								m_ActorIndex;
	float											m_fLastSyncTime;

};

#endif