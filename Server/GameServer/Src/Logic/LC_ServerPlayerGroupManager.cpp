#include "LC_ServerPlayerGroupManager.h"
#include "LC_LogicIDManager.h"
#include "LC_ServerPlayerGroup.h"

using namespace GameLogic;
//----------------------------------------------------
LC_ServerPlayerGroupManager::LC_ServerPlayerGroupManager(): LC_PlayerGroupManagerBase()
{
	m_pkServerPlayerGroupPool = T_NEW_D Utility::UT_MemoryPool<LC_ServerPlayerGroup>;
	m_pkServerPlayerGroupPool->Init(50, -1, "LC_ServerPlayerGroupManager.m_pkServerPlayerGroupPool");
	m_ActorIndex.clear();
	m_fLastSyncTime = 0.0f;
}
//----------------------------------------------------
LC_ServerPlayerGroupManager::~LC_ServerPlayerGroupManager()
{
	LC_PlayerGroupIDPtrMapIter IterBe = m_kPlayerGroupIDPtrMap.begin();
	LC_PlayerGroupIDPtrMapIter IterEd = m_kPlayerGroupIDPtrMap.end();
	for (; IterBe != IterEd; ++IterBe)
	{
		LC_PlayerGroupBase* pkPlayerGroupBase = (*IterBe).second;
		_freePlayerGroup(pkPlayerGroupBase);
	}
	T_SAFE_DELETE(m_pkServerPlayerGroupPool);
}
//----------------------------------------------------
void LC_ServerPlayerGroupManager::Update(float fCurrentTime, float fDeltaTime)
{
	if (fCurrentTime <= m_fLastSyncTime + 10.0f)
	{
		return;
	}
	mem::list<LC_ServerPlayerGroup*> playerGroupList;
	LC_PlayerGroupIDPtrMapIter Iter = m_kPlayerGroupIDPtrMap.begin();
	while (Iter != m_kPlayerGroupIDPtrMap.end())
	{
		LC_ServerPlayerGroup* pkGroup = (LC_ServerPlayerGroup*)(Iter->second);
		pkGroup->Update(fCurrentTime);
		++Iter;
		if (pkGroup->GetNeedRecycle())
		{
			playerGroupList.push_back(pkGroup);
		}
	}
	for (mem::list<LC_ServerPlayerGroup*>::iterator iter = playerGroupList.begin(); iter != playerGroupList.end(); iter ++)
	{
		DestroyPlayerGroup(*iter);
	}
	m_fLastSyncTime = fCurrentTime;
}
//----------------------------------------------------
LC_ServerPlayerGroup* LC_ServerPlayerGroupManager::CreateGroup(LC_ServerPlayer* player, uint32_t nGroupType)
{
	LC_LogicIDManager* pkLogicIDManager	= LC_LogicIDManager::GetSingletonPtr();
	int lGroupID = pkLogicIDManager->RequestLogicID(LOT_PLAYER_GROUP);
	if (lGroupID == 0)
	{
		return NULL;
	}
	LC_ServerPlayerGroup* group = (LC_ServerPlayerGroup*)CreatePlayerGroup(lGroupID);
	if (group == NULL)
	{
		return NULL;
	}
	group->SetGroupType(nGroupType);
	if (group->Create(player) != RE_SUCCESS)
	{
		_freePlayerGroup(group);
		return NULL;
	}
	return group;
}
//----------------------------------------------------
ResultType LC_ServerPlayerGroupManager::Leave(LC_ServerPlayer* player, bool& destory)
{
	if (player == NULL)
	{
		return RE_FAIL;
	}
	LC_ServerPlayerGroup* group = GetPlayerGroup(player->GetInstance());
	if (group == NULL)
	{
		return RE_FAIL;
	}
	return leaveGroup(player, group, destory);
}

ResultType LC_ServerPlayerGroupManager::leaveGroup(LC_ServerPlayer* player, LC_ServerPlayerGroup* group, bool& destory)
{
	if (player == NULL || group == NULL)
	{
		return RE_FAIL;
	}
	ResultType ret = group->Leave(player);
	if (ret != RE_SUCCESS)
	{
		return ret;
	}
	if (group->IsMemberListEmpty())
	{
		destory = true;
		//DestroyPlayerGroup(group);
	}
	return RE_SUCCESS;
}

//----------------------------------------------------
ResultType LC_ServerPlayerGroupManager::DelPlayer(int groupid, const unique_id_impl& lCharID, bool& destory)
{
	LC_ServerPlayerGroup* group = (LC_ServerPlayerGroup*)FindPlayerGroupByGroupID(groupid);
	if (group == NULL)
	{
		return RE_FAIL;
	}

	ResultType ret = group->DelPlayer(lCharID);
	if (ret != RE_SUCCESS)
	{
		return ret;
	}
	if (group->IsMemberListEmpty())
	{
		destory = true;
		DestroyPlayerGroup(group);
	}
	return RE_SUCCESS;
}
//------------------------------------------------------
void LC_ServerPlayerGroupManager::Dismiss(LC_ServerPlayerGroup* group)
{
	if (group == NULL)
	{
		return;
	}
	group->Dismiss();
	DestroyPlayerGroup(group);
}
//---------------------------------------------------
bool LC_ServerPlayerGroupManager::UpdatePlayerState(const unique_id_impl& lCharID, int online)
{
	LC_ServerPlayerGroup* group = GetPlayerGroup(lCharID);
	if (group == NULL)
	{
		return false;
	}
	return group->UpdatePlayerState(lCharID, online);
}
//---------------------------------------------------
/*
void LC_ServerPlayerGroupManager::PlayerTempLeave(LC_ServerPlayer* player)
{
	if (player == NULL)
	{
		return;
	}
	LC_ServerPlayerGroup* group = GetPlayerGroup(player->GetID());
	if (group == NULL)
	{
		return;
	}
	if (!UpdatePlayerState(player->GetID(), PLAYER_CROSSREALMTEAM))
	{
		return;
	}
	player->SetPlayerGroupIDType(INVALID_GROUP_ID, group->GetGroupType());
	player->SetPlayerGroupCheifFlag(false);
	player->DeactiveSkillState(TEAMGROUP_BUFF1);
	player->DeactiveSkillState(TEAMGROUP_BUFF2);
	player->DeactiveSkillState(TEAMGROUP_BUFF3);
	DelPlayerIndex(player->GetID());
}
*/
//----------------------------------------------------
void LC_ServerPlayerGroupManager::PlayerRecover(LC_ServerPlayer* player, int groupid)
{
	if (player == NULL)
	{
		return;
	}
	LC_ServerPlayerGroup* group = (LC_ServerPlayerGroup*)FindPlayerGroupByGroupID(groupid);
	if (group == NULL)
	{
		return;
	}
	AddPlayerIndex(player->GetInstance(), groupid);
	if (!UpdatePlayerState(player->GetInstance(), PLAYER_ON_LINE))
	{
		return;
	}
	group->AddPlayerToMemberList(player);
	player->SetPlayerGroupIDType(groupid, group->GetGroupType());
	if (group->GetGroupCheifID() == player->GetID())
	{
		player->SetPlayerGroupCheifFlag(true);
	}
	else
	{
		player->SetPlayerGroupCheifFlag(false);
	}
	player->ActiveSkillState(TEAMGROUP_BUFF1, 0, 0);
	player->ActiveSkillState(TEAMGROUP_BUFF2, 0, 0);
	player->ActiveSkillState(TEAMGROUP_BUFF3, 0, 0);

}
//----------------------------------------------------
void LC_ServerPlayerGroupManager::ClearPlayerFromGroup(LC_ServerPlayer* player, uint32_t nGroupID)
{
	if(player == NULL)
	{
		return;
	}
	bool bDestroy = false;
	if(INVALID_GROUP_ID != nGroupID)
	{
		LC_PlayerGroupIDPtrMapIter iter = m_kPlayerGroupIDPtrMap.find(nGroupID);
		if(iter != m_kPlayerGroupIDPtrMap.end())
		{
			LC_ServerPlayerGroup* pkGroup = (LC_ServerPlayerGroup*)(iter->second);
			if(pkGroup != NULL && leaveGroup(player, pkGroup, bDestroy) == RE_SUCCESS)
			{
				return;
			}
		}
	}

	LC_ServerPlayerGroup* pkGroup = GetPlayerGroup(player->GetInstance());
	if(pkGroup != NULL && leaveGroup(player, pkGroup, bDestroy) == RE_SUCCESS)
	{
		return;
	}

	for (LC_PlayerGroupIDPtrMapIter Iter = m_kPlayerGroupIDPtrMap.begin(); Iter != m_kPlayerGroupIDPtrMap.end(); ++Iter)
	{
		LC_ServerPlayerGroup* pkGroup = (LC_ServerPlayerGroup*)(Iter->second);
		if(pkGroup != NULL && leaveGroup(player, pkGroup, bDestroy) == RE_SUCCESS)
		{
			return;
		}
	}
}
//----------------------------------------------------
bool LC_ServerPlayerGroupManager::AddPlayerIndex(const unique_id_impl& lCharID, int groupid)
{
	m_ActorIndex[lCharID] = groupid;
	return true;
}
//--------------------------------------------------------
void LC_ServerPlayerGroupManager::DelPlayerIndex(const unique_id_impl& lCharID)
{
	m_ActorIndex.erase(lCharID);
}
//----------------------------------------------------------
LC_ServerPlayerGroup* LC_ServerPlayerGroupManager::GetPlayerGroup(const unique_id_impl& lCharID)
{
	Actor_Index_Type::iterator it = m_ActorIndex.find(lCharID);
	if (it == m_ActorIndex.end())
	{
		return NULL;
	}
	return (LC_ServerPlayerGroup*)FindPlayerGroupByGroupID(it->second);
}
//----------------------------------------------------
LC_PlayerGroupBase* LC_ServerPlayerGroupManager::_allocPlayerGroup()
{
	LC_ServerPlayerGroup* group = m_pkServerPlayerGroupPool->Alloc();
	group->MakeGUID();
	return group;
}
//--------------------------------------------------------
void LC_ServerPlayerGroupManager::_freePlayerGroup(LC_PlayerGroupBase* pkPlayerGroup)
{
	LC_LogicIDManager::GetSingletonPtr()->ReleaseLogicID(LOT_PLAYER_GROUP, pkPlayerGroup->GetID());
	m_pkServerPlayerGroupPool->Free((LC_ServerPlayerGroup*)pkPlayerGroup);
}
