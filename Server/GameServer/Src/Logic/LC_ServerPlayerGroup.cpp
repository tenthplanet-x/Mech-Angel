#include "LC_ServerPlayerGroup.h"
#include "LC_ServerPlayer.h"

#include "NW_Helper.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerPlayerGroupManager.h"
#include "LC_ServerMapLogic_Place.h"
#include "UT_ServerHelper.h"
#include "GameServerApp.h"
#include "LC_GameEventManager.h"
#include "MG_GroupProtocol.h"
using namespace GameLogic;
using namespace Utility;
using namespace Protocol;

int sn_seq = 1;

//--------------------------------------------------------
LC_ServerPlayerGroup::LC_ServerPlayerGroup(): LC_PlayerGroupBase(), m_fLastSyncTime(0.0f), m_lRecycleCountDown(0), m_bNewCreate(false), m_nGildMapId(0)
{
}
//--------------------------------------------------------
LC_ServerPlayerGroup::~LC_ServerPlayerGroup()
{
}
//--------------------------------------------------------
void LC_ServerPlayerGroup::ReUse()
{
	LC_PlayerGroupBase::ReUse();
	m_fLastSyncTime = 0.0f;
	m_lRecycleCountDown = 0;
	m_bStartRecycleCountDown = false;
	m_bNewCreate = false;
	m_nGildMapId = 0;
}
//--------------------------------------------------------
void LC_ServerPlayerGroup::Update(float fCurrentTime)
{
	//服务器组队每一段时间更新一次组队信息
	_updateGroupMemberList();
	CheckAllMember(fCurrentTime);
	m_fLastSyncTime = fCurrentTime;
	//m_GUID.clear();
}

void LC_ServerPlayerGroup::MakeGUID()
{
	m_GUID = UT_BaseStringUtility::MakeGUID(GetGameServerID(), int32_t(GET_CURRENT_TIMESTAMP_IN_SECONDS()), ++sn_seq, GetID(), 0);
}
//----------------------------------------------------------
ResultType LC_ServerPlayerGroup::Create(LC_ServerPlayer* player)
{
	if (player == NULL)
	{
		return RE_FAIL;
	}
	const unique_id_impl& nCharID = player->GetInstance();
	LC_ServerPlayerGroupManager* pkPlayerGroupManager = SERVER_GET_PLAYER_GROUP_MANAGER();
	LC_ServerPlayerGroup* pGroup = pkPlayerGroupManager->GetPlayerGroup(nCharID);
	if (pGroup != NULL && pGroup->IsInGroupMemeberList(nCharID) == true)
	{
		return RE_GROUP_CREATE_FAIL_EXIST_TEAM;
	}
	if (pGroup != NULL)
	{
		pkPlayerGroupManager->DelPlayerIndex(nCharID);
	}
	ResultType ret = LC_PlayerGroupBase::AddPlayerToMemberList(player);
	if (ret != RE_GROUP_JOIN_SUCCESS)
	{
		return ret;
	}
	LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
	if (((LC_ServerPlayer*)player)->CheckTranscriptionValidity())
	{
		LC_GameEvent* pkGameEvent = pkEventManager->AllocGameEvent();
		if (pkGameEvent)
		{
			pkGameEvent->SetEventType(GLET_PLAYER_JOIN_GROUP);
			pkGameEvent->SetEventParam(EVENT_BASE_PARAM_1, player->GetID());
			pkGameEvent->SetEventParam(EVENT_BASE_PARAM_2, player->GetMapLogicID());
			pkEventManager->InsertGameEvent(pkGameEvent);
		}
	}
	pkPlayerGroupManager->AddPlayerIndex(nCharID, GetID());
	MG_RLT_CreateGroup rltMsg;
	rltMsg.m_lGroupID		= GetID();
	rltMsg.m_iCreateResult	= RE_GROUP_CREATE_SUCCESS;
	player->SendMsgToClient(MGPT_RLT_CREATE_GROUP, &rltMsg);
	//发送成员信息
	MG_SyncPlayerGroupMemberList rkSyncMsg;
	PackageGroupMemberList(rkSyncMsg);
	BroadCast(MGPT_SYNC_PLAYER_GROUP_MEMBER_LIST, &rkSyncMsg);
	LC_ServerMapLogic_Place* place = UT_ServerHelper::GetPlayerPlace(player);
	if (place != NULL)
	{
		if (place->GetGroupShare() == Share_Normal)
		{
			SetRaidPortal(place->GetMapLogicID(), place->GetMapResID(), place->GetMapLogicType(), place->GetGroupShare(), place->GetExclusive());
			place->AddGroup(GetID());
		}
	}
	m_fLastSyncTime = GET_PROCESS_TIME;
	m_bNewCreate = true;
	return RE_SUCCESS;
}
//-----------------------------------------------------------
ResultType LC_ServerPlayerGroup::DelPlayer(const unique_id_impl& lCharID)
{
	LC_PlayerGroupEntry* entry = FindGroupEntryInMemberList(lCharID);
	if (entry == NULL)
	{
		return RE_FAIL;
	}
	bool bIsGroupCheif = (GetGroupCheifCharID() == lCharID);
	ResultType ret = RemovePlayerFromMemberList(lCharID);
	if (ret != RE_GROUP_REMOVE_MEMBER_FAIL_EMPTY && ret != RE_GROUP_REMOVE_MEMBER_SUCCESS)
	{
		return ret;
	}
	LC_ServerPlayerGroupManager* pkPlayerGroupManager = SERVER_GET_PLAYER_GROUP_MANAGER();
	pkPlayerGroupManager->DelPlayerIndex(lCharID);
	//Insert Event, The TranscriptionMgr should handle
	LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
	{
		LC_GameEvent* pkGameEvent = pkEventManager->AllocGameEvent();
		if (pkGameEvent)
		{
			pkGameEvent->SetEventType(GLET_PLAYER_LEAVE_GROUP);
			pkGameEvent->SetEventParam(EVENT_BASE_PARAM_1, entry->GetPlayerID());
			pkGameEvent->SetEventParam(EVENT_BASE_PARAM_2, GetID());
			pkEventManager->InsertGameEvent(pkGameEvent);
		}
	}
	if (bIsGroupCheif && ret != RE_GROUP_REMOVE_MEMBER_FAIL_EMPTY)
	{
		MG_RLT_AppointmentCheif rltAppointmentCheifMsg;
		rltAppointmentCheifMsg.m_lGroupID				= GetID();
		rltAppointmentCheifMsg.m_lBeAppointmentCharID	= GetGroupCheifCharID();
		rltAppointmentCheifMsg.m_iAppointmentResult		= RE_GROUP_APPOINTMENT_CHEIF_SUCCESS;
		BroadCast(MGPT_RLT_APPOINTMENT_CHEIF, &rltAppointmentCheifMsg);
	}
	MG_RLT_ExitGroup rltMsg;
	rltMsg.m_lExitCharID		= lCharID;
	rltMsg.m_lGroupID			= GetID();
	TPSTR2STDSTR(entry->GetName(), rltMsg.m_szExitPlayerName);
	rltMsg.m_iExitResult		= RE_GROUP_EXIT_SUCCESS;
	if (ret != RE_GROUP_REMOVE_MEMBER_FAIL_EMPTY)
	{
		BroadCast(MGPT_RLT_EXIT_GROUP, &rltMsg);
		MG_SyncPlayerGroupMemberList rkSyncMsg;
		PackageGroupMemberList(rkSyncMsg);
		BroadCast(MGPT_SYNC_PLAYER_GROUP_MEMBER_LIST, &rkSyncMsg);
	}
	m_bNewCreate = false;
	return RE_SUCCESS;
}
//----------------------------------------------------------
ResultType LC_ServerPlayerGroup::Leave(LC_ServerPlayer* player)
{
	if (player == NULL)
	{
		return RE_FAIL;
	}
	if (FindGroupEntryInMemberList(player->GetInstance()) == NULL)
	{
		return RE_FAIL;
	}
	bool bIsGroupCheif = (GetGroupCheifCharID() == player->GetInstance());
	ResultType ret = RemovePlayerFromMemberList(player->GetInstance());
	if (ret != RE_GROUP_REMOVE_MEMBER_FAIL_EMPTY && ret != RE_GROUP_REMOVE_MEMBER_SUCCESS)
	{
		return ret;
	}
	LC_ServerPlayerGroupManager* pkPlayerGroupManager = SERVER_GET_PLAYER_GROUP_MANAGER();
	pkPlayerGroupManager->DelPlayerIndex(player->GetInstance());
	LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
	{
		LC_GameEvent* pkGameEvent = pkEventManager->AllocGameEvent();
		if (pkGameEvent)
		{
			pkGameEvent->SetEventType(GLET_PLAYER_LEAVE_GROUP);
			pkGameEvent->SetEventParam(EVENT_BASE_PARAM_1, player->GetID());
			pkGameEvent->SetEventParam(EVENT_BASE_PARAM_2, GetID());
			pkEventManager->InsertGameEvent(pkGameEvent);
		}
	}
	if (bIsGroupCheif && ret != RE_GROUP_REMOVE_MEMBER_FAIL_EMPTY)
	{
		MG_RLT_AppointmentCheif rltAppointmentCheifMsg;
		rltAppointmentCheifMsg.m_lRequestCharID		= player->GetInstance();
		rltAppointmentCheifMsg.m_lGroupID				= GetID();
		rltAppointmentCheifMsg.m_lBeAppointmentCharID	= GetGroupCheifCharID();
		rltAppointmentCheifMsg.m_iAppointmentResult		= RE_GROUP_APPOINTMENT_CHEIF_SUCCESS;
		BroadCast(MGPT_RLT_APPOINTMENT_CHEIF, &rltAppointmentCheifMsg);
		player->SendMsgToClient(MGPT_RLT_APPOINTMENT_CHEIF, &rltAppointmentCheifMsg);
	}
	MG_RLT_ExitGroup rltMsg;
	rltMsg.m_lExitCharID		= player->GetInstance();
	rltMsg.m_lGroupID			= GetID();
	TPSTR2STDSTR(player->GetOwnerCharName(), rltMsg.m_szExitPlayerName);
	rltMsg.m_iExitResult		= RE_GROUP_EXIT_SUCCESS;
	//发给该玩家
	player->SendMsgToClient(MGPT_RLT_EXIT_GROUP, &rltMsg);
	if (ret != RE_GROUP_REMOVE_MEMBER_FAIL_EMPTY)
	{
		BroadCast(MGPT_RLT_EXIT_GROUP, &rltMsg);
		MG_SyncPlayerGroupMemberList rkSyncMsg;
		PackageGroupMemberList(rkSyncMsg);
		BroadCast(MGPT_SYNC_PLAYER_GROUP_MEMBER_LIST, &rkSyncMsg);
	}
	if (IsLessOneValidMember())
	{
		Dismiss();
		pkPlayerGroupManager->DestroyPlayerGroup(this);
	}
	m_bNewCreate = false;
	return RE_SUCCESS;
}
//----------------------------------------------------------
ResultType LC_ServerPlayerGroup::Kick(const unique_id_impl& lCharID)
{
	LC_PlayerGroupEntry* entry = FindGroupEntryInMemberList(lCharID);
	if (entry == NULL)
	{
		return RE_FAIL;
	}
	if (entry->GetCheif())
	{
		return RE_FAIL;
	}
	int entrystate = entry->GetState();
	StringType name = entry->GetName();
	LC_ServerPlayer* targetplayer = NULL;
	if (entry->GetState() != PLAYER_OFF_LINE)
	{
		targetplayer = (LC_ServerPlayer*)entry->GetPlayer();
	}
	ResultType ret = RemovePlayerFromMemberList(lCharID);
	if (ret != RE_GROUP_REMOVE_MEMBER_SUCCESS)
	{
		return ret;
	}
	LC_ServerPlayerGroupManager* pkPlayerGroupManager = SERVER_GET_PLAYER_GROUP_MANAGER();
	if (entrystate != PLAYER_CROSSREALMTEAM)
	{
		pkPlayerGroupManager->DelPlayerIndex(lCharID);
	}
	//Insert Event, The TranscriptionMgr should handle
	LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
	{
		LC_GameEvent* pkGameEvent = pkEventManager->AllocGameEvent();
		if (pkGameEvent)
		{
			pkGameEvent->SetEventType(GLET_PLAYER_LEAVE_GROUP);
			pkGameEvent->SetEventParam(EVENT_BASE_PARAM_1, entry->GetPlayerID());
			pkGameEvent->SetEventParam(EVENT_BASE_PARAM_2, GetID());
			pkEventManager->InsertGameEvent(pkGameEvent);
		}
	}
	MG_RLT_KickPlayerFromGroup rltMsg;
	rltMsg.m_iKickResult	= RE_GROUP_KICK_SUCCESS;
	rltMsg.m_lGroupID		= GetID();
	rltMsg.m_lKickCharID	= lCharID;
	TPSTR2STDSTR(name, rltMsg.m_szKickPlayerName);
	//通知给当前队伍
	BroadCast(MGPT_RLT_KICK_PLAYER_FROM_GROUP, &rltMsg);
	//发给该玩家
	if (targetplayer != NULL)
	{
		targetplayer->SendMsgToClient(MGPT_RLT_KICK_PLAYER_FROM_GROUP, &rltMsg);
	}
	MG_SyncPlayerGroupMemberList rkSyncMsg;
	PackageGroupMemberList(rkSyncMsg);
	BroadCast(MGPT_SYNC_PLAYER_GROUP_MEMBER_LIST, &rkSyncMsg);
	if (IsLessOneValidMember())
	{
		Dismiss();
		pkPlayerGroupManager->DestroyPlayerGroup(this);
	}
	m_bNewCreate = false;
	return RE_SUCCESS;
}
//--------------------------------------------------------
void LC_ServerPlayerGroup::Dismiss()
{
	MG_RLT_DismissGroup rltMsg;
	rltMsg.m_iDisimissResult	= RE_GROUP_DISMISS_SUCCESS;
	rltMsg.m_lGroupID			= GetID();
	//通知给当前队伍
	BroadCast(MGPT_RLT_DISMISS_GROUP, &rltMsg);
	LC_ServerPlayerGroupManager* pkPlayerGroupManager = SERVER_GET_PLAYER_GROUP_MANAGER();
	for (int iMemberIndex = 0; iMemberIndex < MAX_PLAYER_GROUP_MEMBER_LIST_SIZE; ++iMemberIndex)
	{
		LC_PlayerGroupEntry& rkEntry = m_kMemberList[iMemberIndex];
		if (!rkEntry.GetValid())
		{
			continue;
		}
		if (rkEntry.GetState() != PLAYER_CROSSREALMTEAM)
		{
			pkPlayerGroupManager->DelPlayerIndex(rkEntry.GetPlayerID());
		}
	}
	//Insert Event, The TranscriptionMgr should handle
	LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
	{
		LC_GameEvent* pkGameEvent = pkEventManager->AllocGameEvent();
		if (pkGameEvent)
		{
			pkGameEvent->SetEventType(GLET_PLAYERGROUP_DISMISS);
			pkGameEvent->SetEventParam(EVENT_BASE_PARAM_1, GetGroupCheifID());
			pkGameEvent->SetEventParam(EVENT_BASE_PARAM_2, GetID());
			pkEventManager->InsertGameEvent(pkGameEvent);
		}
	}
	m_bNewCreate = false;
	LC_PlayerGroupBase::Dismiss();
}
//--------------------------------------------------------
bool LC_ServerPlayerGroup::UpdatePlayerState(const unique_id_impl& lCharID, int online)
{
	LC_PlayerGroupEntry* entry = FindGroupEntryInMemberList(lCharID);
	if (entry == NULL)
	{
		return false;
	}
	entry->SetState(online);
	bool bChief = entry->GetCheif();
	if (online != PLAYER_OFF_LINE)
	{
		_updateEntryInfo(*entry);
	}
	//_regroupMemberList();
	//发送所有成员信息
	MG_SyncPlayerGroupMemberList rkSyncMsg;
	PackageGroupMemberList(rkSyncMsg);
	BroadCast(MGPT_SYNC_PLAYER_GROUP_MEMBER_LIST, &rkSyncMsg);
	//同步申请列表信息给新队长
	if (bChief)
	{
		CheifSyncGroupCandidateListToClient(GetGroupCheifCharID());
	}
	return true;
}
//--------------------------------------------------------
void LC_ServerPlayerGroup::BroadCast(uint16_t usMsgType, MG_BODY* pkMsgBody)
{
	LC_PlayerPtrList playerList;
	for (int iMemberIndex = 0; iMemberIndex < MAX_PLAYER_GROUP_MEMBER_LIST_SIZE; ++iMemberIndex)
	{
		LC_PlayerGroupEntry& rkEntry = m_kMemberList[iMemberIndex];
		if (!rkEntry.GetValid())
		{
			continue;
		}
		LC_ServerPlayer* pkMember = (LC_ServerPlayer*)(rkEntry.GetPlayer());
		if (pkMember/* && rkEntry.GetState() == PLAYER_ON_LINE*/)
		{
			playerList.push_back(pkMember);
		}
	}
	MulticastMsgToGateway(playerList, usMsgType, pkMsgBody);
}
//-------------------------------------------------------------------
void LC_ServerPlayerGroup::SyncRaidPortal()
{
	MG_NOTIFY_GroupRaid notify;
	RaidPortalEntry* portal = GetRaidPortal();
	if (portal == NULL)
	{
		notify.m_Vaild = false;
	}
	else
	{
		notify.m_Vaild = portal->m_Vaild;
		notify.m_LogicId = portal->m_LogicId;
		notify.m_LogicType = portal->m_LogicType;
		notify.m_Resid = portal->m_Resid;
		notify.m_Shartype = portal->m_Shartype;
		notify.m_Excludetype = portal->m_Excludetype;
	}
	BroadCast(MGPT_NOTIFY_GROUP_RAID, &notify);
}
//---------------------------------------------------------------
bool LC_ServerPlayerGroup::CheckGroupRaidExclude(LC_ServerPlayer* addplayer, LC_ServerMapLogic_Place*& raidportal, bool& addto)
{
	raidportal = NULL;
	addto = false;
	RaidPortalEntry* portal = GetRaidPortal();
	if (portal != NULL)
	{
		LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
		raidportal = pkWorldManager->GetMapLogic(portal->m_LogicId);
		if (raidportal == NULL)
		{
			return true;
		}

		LC_ServerMapLogic_Place* place = UT_ServerHelper::GetPlayerPlace(addplayer);
		if (place == NULL)
		{
			return true;
		}
		if (place->GetMapLogicID() == portal->m_LogicId)
		{
			return true;
		}
		if (ExclusiveTable[place->GetExclusive()][portal->m_Excludetype] == 0)
		{
			return true;
		}
		addto = true;
		ResultType ret = raidportal->CheckEnterMap(addplayer, LMIOT_TRANSFER);
		if (ret != RE_SUCCESS)
		{
			return false;
		}
	}
	return true;
}
//--------------------------------------------------------
void LC_ServerPlayerGroup::SendToCheif(uint16_t usMsgType, MG_BODY* pkMsgBody)
{
	LC_PlayerGroupEntry& rkEntry = m_kMemberList[0];
	if (!rkEntry.GetValid())
	{
		return;
	}
	LC_ServerPlayer* pkCheif = (LC_ServerPlayer*)(rkEntry.GetPlayer());
	if (!pkCheif)
	{
		return;
	}
	pkCheif->SendMsgToClient(usMsgType, pkMsgBody);
}
//--------------------------------------------------------
void LC_ServerPlayerGroup::PackageGroupMemberList(MG_SyncPlayerGroupMemberList& rkSyncMsg)
{
	rkSyncMsg.m_lGroupID = m_iID;
	rkSyncMsg.m_iItemGrade = m_iItemGrade;
	rkSyncMsg.m_akMemberEntries.reserve(MAX_PLAYER_GROUP_MEMBER_LIST_SIZE);

	for (uint8_t ucMemberIndex = 0; ucMemberIndex < MAX_PLAYER_GROUP_MEMBER_LIST_SIZE; ++ucMemberIndex)
	{
		LC_PlayerGroupEntry& rkEntry = m_kMemberList[ucMemberIndex];
		if (rkEntry.GetValid())
		{
			rkSyncMsg.m_akMemberEntries.resize(ucMemberIndex + 1);

			MemberEntry& psEntry = rkSyncMsg.m_akMemberEntries[ucMemberIndex];
			psEntry.m_iBornCharType = rkEntry.GetBornCharType();
			psEntry.m_ucIndex				= ucMemberIndex;
			psEntry.m_bCheif				= rkEntry.GetCheif();
			psEntry.m_lPlayerID				= rkEntry.GetPlayerID();
			psEntry.m_uiCharID				= rkEntry.GetCharID();
			psEntry.m_lPlayerLevel			= (uint16_t)rkEntry.GetLevel();
			psEntry.m_iPlayerCareerType		= (uint8_t)rkEntry.GetCareerType();
			psEntry.m_iWeaponType			= (uint8_t)rkEntry.GetWeaponType();
			psEntry.m_iVehicleLevel			= (uint8_t)rkEntry.GetVehicleLevel();
			psEntry.m_bPlayerDead			= rkEntry.GetDead();
			psEntry.m_bPlayerOffline		= rkEntry.GetState();
			psEntry.m_lCharType				= rkEntry.GetCharType();
			psEntry.m_lMaxHP				= rkEntry.GetMaxHP();
			psEntry.m_iBowLevel				= (uint8_t)rkEntry.GetBowLevel();
			psEntry.m_lHP					= rkEntry.GetHP();
			psEntry.m_iDevaEyeLevel			= (uint8_t)rkEntry.GetDevaEyeLevel();
			psEntry.m_lHeadAvatarID			= rkEntry.GetHeadAvatarID();
			psEntry.m_lBodyAvatarID			= rkEntry.GetBodyAvatarID();
			psEntry.m_lFootAvatarID			= rkEntry.GetFootAvatarID();
			psEntry.m_lWeaponAvatarID		= rkEntry.GetWeaponAvatarID();
			psEntry.m_lPosX					= rkEntry.GetPosX();
			psEntry.m_lPosY					= rkEntry.GetPosY();
			psEntry.m_lMapID				= rkEntry.GetMapID();
			psEntry.m_lMapResID				= rkEntry.GetMapResID();
			psEntry.m_lLogicAreaID			= rkEntry.GetLogicAreaID();
			psEntry.m_iScore				= rkEntry.GetScore();
			psEntry.m_nTransformID = rkEntry.GetTransformID();
			TPSTR2STDSTR(rkEntry.GetName(), psEntry.m_strPlayerName);
		}
	}
}
//--------------------------------------------------------
bool LC_ServerPlayerGroup::GetNeedRecycle()
{
	return (m_bStartRecycleCountDown && m_lRecycleCountDown <= 0);
}
//--------------------------------------------------------
void LC_ServerPlayerGroup::CheckAllMember(float fCurrentTime)
{
	LC_ServerPlayerGroupManager* pkPlayerGroupManager = SERVER_GET_PLAYER_GROUP_MANAGER();
	if (NULL == pkPlayerGroupManager)
	{
		return;
	}
	int32_t iOnlineCount = 0;
	int32_t iValidCount = 0;
	int32_t iGoneTime = Float2Long(fCurrentTime - m_fLastSyncTime);
	mem::vector<object_id_type> idList;
	for (int iMemberIndex = 0; iMemberIndex < MAX_PLAYER_GROUP_MEMBER_LIST_SIZE; ++iMemberIndex)
	{
		LC_PlayerGroupEntry& rkEntry = m_kMemberList[iMemberIndex];
		if (rkEntry.GetValid())
		{
			uint32_t nLeaveTick = rkEntry.GetLeaveTick();
			if (rkEntry.GetState() == PLAYER_ON_LINE)
			{
				if(nLeaveTick != 0)
				{
					rkEntry.SetLeaveTick(0);
				}
				++iOnlineCount;
				++iValidCount;
			}
			else
			{
				if (nLeaveTick >= 300)	//300秒倒计时	BUG #21706 【BUG】【其他】组队规则简化
				{
					idList.push_back(rkEntry.GetPlayerID());
				}
				else
				{
					rkEntry.SetLeaveTick(nLeaveTick + iGoneTime);
					++iValidCount;
				}
			}
		}
	}

	//if((iValidCount == 1 && m_bNewCreate == false) || iValidCount <= 0)
	if(iValidCount <= 0)
	{
		Dismiss();
		_startRecycleCountDown(0);
	}
	else
	{
		if (iOnlineCount > 0)
		{
			if (m_bStartRecycleCountDown)
			{
				_stopRecycleCountDown();
			}
		}
		else
		{
			if (!m_bStartRecycleCountDown)
			{
				_startRecycleCountDown(300);
			}
			else
			{
				m_lRecycleCountDown -= iGoneTime;
			}
		}
	}

	for(size_t i=0;i<idList.size();i++)
	{
		DelPlayer(idList[i]);
	}
}
//--------------------------------------------------------
LC_PlayerGroupEntry* LC_ServerPlayerGroup::GetCheif()
{
	for (int iMemberIndex = 0; iMemberIndex < MAX_PLAYER_GROUP_MEMBER_LIST_SIZE; iMemberIndex++)
	{
		if (m_kMemberList[iMemberIndex].GetValid() && m_kMemberList[iMemberIndex].GetCheif())
		{
			return &m_kMemberList[iMemberIndex];
		}
	}
	return NULL;
}
//--------------------------------------------------------
ResultType LC_ServerPlayerGroup::AddPlayerToMemberList(LC_PlayerBase* pkPlayer)
{
	if (pkPlayer == NULL)
	{
		return RE_FAIL;
	}
	LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)pkPlayer;
	//检查互斥性
	bool combin = false;
	LC_ServerMapLogic_Place* currentlogicbase = NULL;
	bool ret = CheckGroupRaidExclude((LC_ServerPlayer*)pkPlayer, currentlogicbase, combin);
	if (!ret)
	{
		return RE_RAID_EXCLUDE_CANT_COMBIN;
	}
	ResultType eResult = _checkGuildType(pkServerPlayer);
	if( eResult != RE_SUCCESS)
	{
		return eResult;
	}
	const unique_id_impl& nCharID = pkPlayer->GetInstance();
	LC_ServerPlayerGroupManager* pkPlayerGroupManager = SERVER_GET_PLAYER_GROUP_MANAGER();
	LC_ServerPlayerGroup* pGroup = pkPlayerGroupManager->GetPlayerGroup(nCharID);
	if (pGroup != NULL && pGroup->IsInGroupMemeberList(nCharID) == true)
	{
		return RE_GROUP_CREATE_FAIL_EXIST_TEAM;
	}
	if (pGroup != NULL)
	{
		pkPlayerGroupManager->DelPlayerIndex(nCharID);
	}
	RemoveCandidateInfo(pkPlayer->GetInstance());
	//清空旧队伍
	pkServerPlayer->ClearPlayerGroup(LC_ServerPlayer::E_PlayerGroupMapOpt_Clear, LC_ServerPlayer::E_PlayerGroupLimit_All);
	
	eResult = LC_PlayerGroupBase::AddPlayerToMemberList(pkPlayer);
	if (eResult == RE_GROUP_JOIN_SUCCESS)
	{
		//拉人
		if (combin && currentlogicbase != NULL)
		{
			LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
			pkWorldManager->RequestChangePlayerLocation(
				(LC_ServerPlayer*)pkPlayer,
				LMIOT_TRANSFER,
				currentlogicbase->GetMapLogicID(),
				currentlogicbase->GetMapResID(),
				UT_Vec3Int());
		}
		else
		{
			//增加队伍共享副本
			LC_ServerMapLogic_Place* logicbase = UT_ServerHelper::GetPlayerPlace((LC_ServerPlayer*)pkPlayer);
			if (logicbase != NULL)
			{
				if (logicbase->GetGroupShare() == Share_Normal)
				{
					SetRaidPortal(logicbase->GetMapLogicID(), logicbase->GetMapResID(), logicbase->GetMapLogicType(), logicbase->GetGroupShare(), logicbase->GetExclusive(), false);
					logicbase->AddGroup(GetID());
				}
			}
		}
		//插入事件
		//Insert Event, The TranscriptionMgr should handle
		LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
		if (((LC_ServerPlayer*)pkPlayer)->CheckTranscriptionValidity())
		{
			LC_GameEvent* pkGameEvent = pkEventManager->AllocGameEvent();
			if (pkGameEvent)
			{
				pkGameEvent->SetEventType(GLET_PLAYER_JOIN_GROUP);
				pkGameEvent->SetEventParam(EVENT_BASE_PARAM_1, pkPlayer->GetID());
				pkGameEvent->SetEventParam(EVENT_BASE_PARAM_2, pkPlayer->GetMapLogicID());
				pkEventManager->InsertGameEvent(pkGameEvent);
			}
		}
		pkPlayerGroupManager->AddPlayerIndex(pkPlayer->GetInstance(), GetID());

		//发送加入消息
		MG_RLT_JoinGroup rltMsg;
		rltMsg.m_lGroupID = GetID();
		rltMsg.m_lJoinCharID = pkPlayer->GetInstance();
		TPSTR2STDSTR(pkPlayer->GetOwnerCharName(), rltMsg.m_strJoinPlayerName);
		LC_PlayerGroupEntry* chief = GetCheif();
		if (chief != NULL)
		{
			TPSTR2STDSTR(chief->GetName(), rltMsg.m_strCheifName);
		}
		rltMsg.m_iJoinResult = RE_SUCCESS;
		BroadCast(MGPT_RLT_JOIN_GROUP, &rltMsg);
		//发送所有成员信息
		MG_SyncPlayerGroupMemberList rkSyncMsg;
		PackageGroupMemberList(rkSyncMsg);
		BroadCast(MGPT_SYNC_PLAYER_GROUP_MEMBER_LIST, &rkSyncMsg);

		//同步共享副本
		SyncRaidPortal();

		//通知被邀请人者队伍分配方式
		MG_RLT_ModityQuality rltQualityMsg;
		rltQualityMsg.m_iResult				= RE_GROUP_MODIFY_QUALITY_SUCCESS;
		rltQualityMsg.m_lGroupID			= m_iID;
		rltQualityMsg.m_iItemGrade			= m_iItemGrade;
		pkServerPlayer->SendMsgToClient(MGPT_RLT_MODIFY_QUALITY, &rltQualityMsg);

		m_bNewCreate = false;
	}
	return eResult;
}
//----------------------------------------------------------
ResultType LC_ServerPlayerGroup::AddPlayerToCandidateList(LC_PlayerBase* pkPlayer)
{
	ResultType eResult = _checkGuildType((LC_ServerPlayer*)pkPlayer);
	if( eResult != RE_SUCCESS)
	{
		return eResult;
	}

	eResult = LC_PlayerGroupBase::AddPlayerToCandidateList(pkPlayer);
	if (eResult != RE_GROUP_CANDIDATE_ADD_SUCCESS)
	{
		return eResult;
	}
	_syncGroupCandidateListToClient();
	SetDirtyFlag(DIRTY_FLAG_PLAYER_GROUP_CANDIDATE_LIST, false);
	return RE_SUCCESS;
}
//--------------------------------------------------------
ResultType LC_ServerPlayerGroup::RemoveCandidateInfo(const unique_id_impl& lCharID)
{
	ResultType ret = LC_PlayerGroupBase::RemoveCandidateInfo(lCharID);
	if (ret != RE_GROUP_CANDIDATE_REMOVE_SUCCESS)
	{
		return ret;
	}
	_syncGroupCandidateListToClient();
	SetDirtyFlag(DIRTY_FLAG_PLAYER_GROUP_CANDIDATE_LIST, false);
	return RE_SUCCESS;
}
//--------------------------------------------------------
void LC_ServerPlayerGroup::_startRecycleCountDown(int cooldown)
{
	m_bStartRecycleCountDown = true;
	m_lRecycleCountDown		 = cooldown;
}
//--------------------------------------------------------
void LC_ServerPlayerGroup::_stopRecycleCountDown()
{
	m_bStartRecycleCountDown = false;
	m_lRecycleCountDown		 = 0;
}
//--------------------------------------------------------
bool LC_ServerPlayerGroup::_updateEntryInfo(LC_PlayerGroupEntry& rkEntry)
{
	if (rkEntry.GetState() == PLAYER_OFF_LINE)
	{
		return false;
	}
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(rkEntry.GetPlayer());
	if (NULL == pkPlayer)
	{
		rkEntry.SetState(PLAYER_OFF_LINE);
		return true;
	}
	LC_PlayerGroupEntry* entry = FindGroupEntryInMemberListByPlayerID(pkPlayer->GetID());
	if (rkEntry.GetState() != PLAYER_CROSSREALMTEAM && pkPlayer->GetPlayerGroupID() != m_iID)
	{
		pkPlayer->SetPlayerGroupIDType(m_iID, m_nGroupType);
	}
	if (pkPlayer->GetPlayerGroupCheifFlag() != rkEntry.GetCheif())
	{
		pkPlayer->SetPlayerGroupCheifFlag(rkEntry.GetCheif());
	}
	rkEntry.SetBornCharType(pkPlayer->GetProfilePictureID());
	rkEntry.SetCharID(pkPlayer->GetInstance());
	rkEntry.SetLevel(pkPlayer->GetLevel());
	rkEntry.SetCareerType(pkPlayer->GetCareerType());
	rkEntry.SetWeaponType(pkPlayer->GetWeaponAttribute());
	rkEntry.SetVehicleLevel(pkPlayer->GetVehicleLevel());
	rkEntry.SetBowLevel(pkPlayer->GetBowLevel());
	rkEntry.SetDevaEyeLevel(pkPlayer->GetDevaEyeLevel());
	rkEntry.SetDead(pkPlayer->GetDead());
	if (entry)
	{
		rkEntry.SetState(entry->GetState());
	}
	rkEntry.SetName(pkPlayer->GetOwnerCharName());
	rkEntry.SetCharType(pkPlayer->GetCharType());
	rkEntry.SetMaxHP(pkPlayer->GetFinalAttributeMap()->GetAttribute(ATT_MAX_HP));
	//rkEntry.SetMaxMP(pkPlayer->GetAttributeMap()->GetAttribute(ATT_MAX_MP));
	rkEntry.SetHP(pkPlayer->GetHP());
	//rkEntry.SetMP(pkPlayer->GetMP());
	rkEntry.SetHeadAvatarID(pkPlayer->GetCurrentEquipItem(ET_HEAD, true));
	rkEntry.SetBodyAvatarID(pkPlayer->GetCurrentEquipItem(ET_BODY, true));
	rkEntry.SetFootAvatarID(pkPlayer->GetCurrentEquipItem(ET_FOOT, true));
	rkEntry.SetWeaponAvatarID(pkPlayer->GetCurrentEquipItem(ET_WEAPON, true));
	rkEntry.SetPosX(pkPlayer->GetCurrentLocation().x);
	rkEntry.SetPosY(pkPlayer->GetCurrentLocation().y);
	rkEntry.SetMapResID(pkPlayer->GetMapResID());
	rkEntry.SetMapID(pkPlayer->GetMapLogicID());
	rkEntry.SetLogicAreaID(pkPlayer->GetCurrentLogicAreaID());
	//rkEntry.SetScore(pkPlayer->GetTotalScore());
	//rkEntry.SetScore(pkPlayer->GetControllerCombatScore());
	rkEntry.SetScore(pkPlayer->GetActivateBuddyCombatScore());
	rkEntry.SetTransformID(pkPlayer->GetCurrentActivateBuddyTransformID());
	return true;
}
//--------------------------------------------------------
void LC_ServerPlayerGroup::_updateGroupMemberList()
{
	bool bUpdate = false;
	for (int iMemberIndex = 0; iMemberIndex < MAX_PLAYER_GROUP_MEMBER_LIST_SIZE; iMemberIndex++)
	{
		LC_PlayerGroupEntry& rkEntry = m_kMemberList[iMemberIndex];
		if (rkEntry.GetValid())
		{
			bUpdate |=	_updateEntryInfo(rkEntry);
		}
	}
	if (bUpdate)
	{
		//发送所有成员信息
		MG_SyncPlayerGroupMemberList rkSyncMsg;
		PackageGroupMemberList(rkSyncMsg);
		BroadCast(MGPT_SYNC_PLAYER_GROUP_MEMBER_LIST, &rkSyncMsg);
	}
}
//--------------------------------------------------------
void LC_ServerPlayerGroup::_syncGroupCandidateListToClient()
{
	MG_SyncPlayerGroupCandidateList syncMsg;
	syncMsg.m_akCandidateEntries.reserve(MAX_PLAYER_GROUP_CANDIDATE_LIST_SIZE);

	for (uint8_t ucCandidateIndex = 0; ucCandidateIndex < MAX_PLAYER_GROUP_CANDIDATE_LIST_SIZE; ++ucCandidateIndex)
	{
		LC_PlayerGroupEntry& rkEntry = m_kCandidateList[ucCandidateIndex];
		if (rkEntry.GetValid())
		{
			syncMsg.m_akCandidateEntries.resize(ucCandidateIndex+1);

			CandidateEntry& psEntry = syncMsg.m_akCandidateEntries[ucCandidateIndex];
			psEntry.m_ucIndex			= ucCandidateIndex;
			psEntry.m_lPlayerID		= rkEntry.GetPlayerID();
			psEntry.m_uiCharID		= rkEntry.GetCharID();
			psEntry.m_lPlayerLevel	= (uint16_t)rkEntry.GetLevel();
			psEntry.m_lCharType		= rkEntry.GetCharType();
			psEntry.m_lHeadAvatarID	= rkEntry.GetHeadAvatarID();
			psEntry.m_lBodyAvatarID	= rkEntry.GetBodyAvatarID();
			psEntry.m_lFootAvatarID	= rkEntry.GetFootAvatarID();
			psEntry.m_lWeaponAvatarID = rkEntry.GetWeaponAvatarID();
			//psEntry.m_lMapID			= rkEntry.GetMapID();
			psEntry.m_lMapID			= rkEntry.GetMapResID();
			psEntry.m_lVehicleLevel	= (uint8_t)rkEntry.GetVehicleLevel();
			psEntry.m_lBowLevel		= (uint8_t)rkEntry.GetBowLevel();
			psEntry.m_lDevaEyeLevel	= (uint8_t)rkEntry.GetDevaEyeLevel();
			psEntry.m_iScore			= rkEntry.GetScore();
			psEntry.m_nTransformID = rkEntry.GetTransformID();
			TPSTR2STDSTR(rkEntry.GetName(), psEntry.m_strPlayerName);
		}
	}
	SendToCheif(MGPT_SYNC_PLAYER_GROUP_CANDIDATE_LIST, &syncMsg);
}

void LC_ServerPlayerGroup::CheifSyncGroupCandidateListToClient(const unique_id_impl& lCharID)
{
	if (GetGroupCheifCharID() == lCharID)
	{
		_syncGroupCandidateListToClient();
	}
}

ResultType	LC_ServerPlayerGroup::_checkGuildType(LC_ServerPlayer* addplayer)
{
	if(addplayer == NULL)
	{
		return RE_FAIL;
	}

	switch(m_nGroupType)
	{
	case E_PlayerTeamType_Cross:
	case E_PlayerTeamType_CLocal:
		{
			LC_ServerMapLogic_Place* place = UT_ServerHelper::GetPlayerPlace(addplayer);
			if (place == NULL)
			{
				return RE_FAIL;
			}
			if(m_nGildMapId == 0)
			{
				LC_PlayerGroupEntry& rkEntry = m_kMemberList[0];
				if (!rkEntry.GetValid())
				{
					break;
				}
				LC_ServerPlayer* pkCheif = (LC_ServerPlayer*)(rkEntry.GetPlayer());
				if (pkCheif == NULL)
				{
					break;
				}
				LC_ServerMapLogic_Place* cheifPlace = UT_ServerHelper::GetPlayerPlace(pkCheif);
				if( cheifPlace == NULL)
				{
					break;
				}
				m_nGildMapId = cheifPlace->GetMapLogicID();
			}
			if (place->GetMapLogicID() != m_nGildMapId)
			{
				return RE_GROUP_LOCAL_CROSS_MAP_ID;
			}
		}
		break;
	case E_PlayerTeamType_None:
	case E_PlayerTeamType_Local:
	default:
		{
			if(UT_ServerHelper::IsInCross(addplayer) == true)
			{
				return RE_GROUP_LOCAL_CROSS_MAP_ID;
			}
		}
		break;
	}
	return RE_SUCCESS;
}
