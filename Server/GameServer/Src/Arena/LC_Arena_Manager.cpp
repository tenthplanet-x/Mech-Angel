#include "LM_LocaleManager.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerPlayer.h"
#include "GameServerApp.h"
#include "MG_ArenaProtocol.h"
#include "LC_ArenaBattleAssert.h"
#include "LC_ServerWorldManager.h"
#include "UT_ServerHelper.h"
#include "LC_Arena_Manager.h"
#include "LC_ServerPlayerManager.h"
#include "LC_RankManager.h"
#include "LC_ServerMapLogic_ArenaPlay.h"
#include "LC_ServerMapLogic_Place.h"
#include "oss_define.h"
#include "LC_GameEventManager.h"
#include "LC_PackEntryFactory.h"

using namespace GameLogic;
using namespace CSVFile;
using namespace Protocol;
using namespace Utility;

Arena_Info::Arena_Info(const msg_define::arena_info& aif, const ::google::protobuf::RepeatedField< ::google::protobuf::uint64 >& chars)
:arena_if(aif), map_logic_id(0), state(0), uptime(GET_PROCESS_TIME)
{
	for (int i=0; i<chars.size(); ++i)
	{
		AddIn(chars.Get(i));
	}
}
//--------------------------------------------------------------------------------------------------------------
LC_Arena_Manager::LC_Arena_Manager()
:m_LastUpdateTime(0)
{

}
LC_Arena_Manager::~LC_Arena_Manager()
{
	fin();
}
bool LC_Arena_Manager::init()
{
	m_LastUpdateTime = 0;
	m_ArenaMap.clear();
	m_ArenaSignups.clear();
	return true;
}

void LC_Arena_Manager::fin()
{
	m_ArenaMap.clear();
}
Arena_Info*	LC_Arena_Manager::GetArenaInfo(int64_t iArenaID)
{
	ArenaInfoMap::iterator it = m_ArenaMap.find(iArenaID);
	return (it != m_ArenaMap.end()) ? &it->second : NULL;
}
Arena_Info* LC_Arena_Manager::GetArenaInfoByCharID(const unique_id_impl& charID)
{
	ArenaInfoMap::iterator it = m_ArenaMap.begin();
	for (; it!=m_ArenaMap.end(); ++it)
	{
		Arena_Info& rkArena = it->second;
		if (rkArena.IsIn(charID))
		{
			return &rkArena;
		}
	}
	return NULL;
}

Arena_Info* LC_Arena_Manager::AddArenaInfo(const msg_define::arena_info& aif, const ::google::protobuf::RepeatedField< ::google::protobuf::uint64 >& chars)
{
	m_ArenaMap[aif.id()] = Arena_Info(aif, chars);
	return &m_ArenaMap[aif.id()];
}

void LC_Arena_Manager::DelArenaInfo(int64_t iArenaID)
{
	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "arena del", iArenaID);
	static LC_ServerPlayerManager* playManager = SERVER_GET_PLAYER_MANAGER();
	Arena_Info* pkArena = GetArenaInfo(iArenaID);
	if (pkArena)
	{
		if (pkArena->map_logic_id && pkArena->TargetServer() == GetGameServerID())
		{
			static LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
			LC_ServerMapLogic_Place* logic = (LC_ServerMapLogic_Place*)pkWorldManager->GetMapLogic(pkArena->map_logic_id);
			if (logic)
			{
				logic->EndAllPlay();
			}
		}

		unique_id_set inChars = pkArena->InChars();
		unique_id_set::const_iterator itch = inChars.begin();
		for (; itch!=inChars.end(); ++itch)
		{
			LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)playManager->FindPlayer(*itch);
			if (pkPlayer)
			{
				ArenaLeave(iArenaID, pkPlayer);
			}
		}		
	}

	m_ArenaMap.erase(iArenaID);
}
void LC_Arena_Manager::SyncArenaInfo(int64_t iArenaID)
{
	Arena_Info* pkArena = GetArenaInfo(iArenaID);
	if (pkArena && pkArena->TargetServer()==GetGameServerID())
	{
		static LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();

		MG_PB<msg_define::arena_status_sync> syncMsg;
		syncMsg.m_value.mutable_arena()->CopyFrom(pkArena->arena_if);

		LC_ServerMapLogic_Place* logic = (LC_ServerMapLogic_Place*)pkWorldManager->GetMapLogic(pkArena->map_logic_id);
		if (logic)
		{
			LC_ServerMapLogic_ArenaPlay* play = (LC_ServerMapLogic_ArenaPlay*)logic->GetPlay(PlayType_Arena);
			if (play)
			{
				pkArena->state = play->State();
				pkArena->uptime = GET_PROCESS_TIME;
			}
			else
			{
				pkArena->state = Arena_State_End;
			}
		}
		else
		{
			pkArena->state = Arena_State_End;
		}
		syncMsg.m_value.set_state(pkArena->state);

		for (int i=0; i<pkArena->arena_if.servers_size(); ++i)
		{
			if (pkArena->arena_if.servers(i) != GetGameServerID())
			{
				SendMsgToGame(pkArena->arena_if.servers(i), MGPT_G2G_SYN_ARENASTATUS, &syncMsg);
			}
		}
	}
}
void LC_Arena_Manager::SyncArenaInfoToClient(int64_t iArenaID)
{
	Arena_Info* pkArena = GetArenaInfo(iArenaID);
	if (NULL == pkArena)
	{
		return;
	}

	static LC_ServerPlayerManager* pkPlayManager = SERVER_GET_PLAYER_MANAGER();
	MG_PB<msg_define::arena_status_sync> syncMsg;
	syncMsg.m_value.mutable_arena()->CopyFrom(pkArena->arena_if);
	syncMsg.m_value.set_state(pkArena->state);

	unique_id_set::const_iterator itch = pkArena->InChars().begin();
	for (; itch!=pkArena->InChars().end(); ++itch)
	{
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayManager->FindPlayer(*itch);
		if (NULL == pkPlayer)
		{
			continue;
		}
		pkPlayer->SendMsgToClient(MGPT_RLT_ARENASTATUS, &syncMsg);
	}
}
void LC_Arena_Manager::ResetArenaOnServer(int32_t iServerID)
{
	//static LC_ServerPlayerManager* playManager = SERVER_GET_PLAYER_MANAGER();

	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "arena reset server", iServerID);
	unique_id_set resetIDs;
	{
		ArenaInfoMap::iterator it = m_ArenaMap.begin();
		for(; it!=m_ArenaMap.end(); ++it)
		{
			Arena_Info& rkArena = it->second;
			if (rkArena.TargetServer() == iServerID)
			{
				//do reset
				resetIDs.insert(it->first);
			}
		}
	}

	{
		unique_id_set::iterator it = resetIDs.begin();
		for (; it!=resetIDs.end(); ++it)
		{
			DelArenaInfo(*it);
		}
	}
}
void LC_Arena_Manager::CharLeave(LC_ServerPlayer* pkPlayer)
{
	IF_A_NA_PLAYER(pkPlayer) return;
	if (!pkPlayer->IsInstance())
	{
		return;
	}
	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "arena char leave", pkPlayer->GetID(), pkPlayer->GetInstance());

	if (IsCharSignValid(pkPlayer->GetInstance(), GET_PROCESS_TIME))
	{
		MG_PB<msg_define::arena_cancel_req> reqMsg;
		reqMsg.m_value.set_type(LC_ARENA_TP_SOLO);
		reqMsg.m_value.set_charid(pkPlayer->GetInstance());
		SendMsgToGlobal(MGPT_REQ_CANCELARENA, &reqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());
	}

	ArenaInfoMap::iterator it = m_ArenaMap.begin();
	for(; it!=m_ArenaMap.end(); ++it)
	{
		Arena_Info& rkArena = it->second;
		if (rkArena.IsIn(pkPlayer->GetInstance()))
		{
			MG_PB<msg_define::arena_leave_req> msg;
			msg.m_value.mutable_arena()->CopyFrom(rkArena.arena_if);
			msg.m_value.set_objectid(pkPlayer->GetControleePeer().ObjectID());
			SendMsgToGame(rkArena.TargetServer(), MGPT_G2G_REQ_LEAVEARENA, &msg, pkPlayer->GetUserID(), pkPlayer->GetInstance());

// 			LC_ArenaEntry* pkArenaEntry = pkPlayer->GetArenaAsset()->GetArenaEntry(rkArena.Type());
// 			if (pkArenaEntry)
// 			{
// 				pkArenaEntry->CommitJoinTimes();
// 			}
		}
	}
}
void LC_Arena_Manager::CharEnter(LC_ServerPlayer* pkPlayer)
{
	IF_A_NA_PLAYER(pkPlayer) return;
	if (!pkPlayer->IsInstance())
	{
		return;
	}

	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "arena char enter", pkPlayer->GetID(), pkPlayer->GetInstance());

	MG_PB<msg_define::arena_achievement_req> reqMsg;
	reqMsg.m_value.set_type(LC_ARENA_TP_SOLO);
	reqMsg.m_value.add_charids(pkPlayer->GetInstance());
	SendMsgToGlobal(MGPT_SS_GLOBAL_ARENA_ACHIEVE_REQ, &reqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());
}
ResultType LC_Arena_Manager::ArenaCheck(int64_t iArenaID)
{
	static LC_ServerPlayerManager* playManager = SERVER_GET_PLAYER_MANAGER();
	Arena_Info* pkArena = GetArenaInfo(iArenaID);
	if (NULL == pkArena)
	{
		return RE_FAIL;
	}

	unique_id_set::const_iterator itch = pkArena->InChars().begin();
	for (; itch!=pkArena->InChars().end(); ++itch)
	{
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)playManager->FindPlayer(*itch);
		if (NULL == pkPlayer)
		{
			return RE_ARENA_IN_CHAR_OFFLINE;
		}
		
		if(UT_ServerHelper::IsInCross(pkPlayer))
		{
			return RE_ON_CROSS_SERVER;
		}

		if (LC_Helper::isInArena(pkPlayer) || LC_Helper::isInTranscript(pkPlayer))//正在副本中不允许
		{
			return RE_SOLO_DULE_IN_TRANSCRIPTION;
		}
	}
	return RE_SUCCESS;
}

ResultType LC_Arena_Manager::ArenaPrepare(int64_t iArenaID)
{
	Arena_Info* pkArena = GetArenaInfo(iArenaID);
	if (NULL == pkArena)
	{
		return RE_FAIL;
	}

	CF_ArenaSetting::DataEntry* pkDataEntry = SafeGetCSVFileDataEntryPtr<CF_ArenaSetting>(pkArena->Type());
	if (NULL == pkDataEntry)
	{
		return RE_FAIL;
	}

	unique_id_set::const_iterator itch = pkArena->InChars().begin();
	for (; itch!=pkArena->InChars().end(); ++itch)
	{
		ArenaUnsign(*itch);
	}

	ResultType rslt = ArenaCheck(iArenaID);
	if(RE_SUCCESS != rslt)
	{
		return rslt;
	}

	if (pkArena->TargetServer() == GetGameServerID())
	{
		static LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
		LC_ServerMapLogic_Place* logic = (LC_ServerMapLogic_Place*)pkWorldManager->CreateServerMapLogic(0, pkDataEntry->_iMapResID, MT_ARENA_PVP, false);
		if (NULL == logic)
		{
			return RE_FAIL;
		}

		LC_ServerMapLogic_ArenaPlay* play = (LC_ServerMapLogic_ArenaPlay*)logic->GetPlay(PlayType_Arena);
		if (NULL == play)
		{
			return RE_FAIL;
		}
		
		pkArena->map_logic_id = logic->GetMapLogicID();
		play->ReserveArenaInfo(*pkArena);
		logic->Start();
	}

	return RE_SUCCESS;
}

ResultType LC_Arena_Manager::ArenaSign(LC_ServerPlayer* pkPlayer, float currentTime)
{
	IF_A_NA_PLAYER(pkPlayer) return RE_FAIL;
	if (!pkPlayer->IsInstance())
	{
		return RE_FAIL;
	}

	m_ArenaSignups[pkPlayer->GetInstance()] = currentTime;
	return RE_SUCCESS;
}

ResultType LC_Arena_Manager::ArenaUnsign(const unique_id_impl& charID)
{
	m_ArenaSignups.erase(charID);
	return RE_SUCCESS;
}

ResultType LC_Arena_Manager::ArenaEnter(int64_t iArenaID, LC_ServerPlayer* pkPlayer, int32_t serverID, user_id_type userID, const unique_id_impl& charID, object_id_type objectID)
{
	IF_A_NA_PLAYER(pkPlayer) return RE_FAIL;
	if (!pkPlayer->IsInstance())
	{
		return RE_FAIL;
	}

	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "on arena enter", iArenaID, pkPlayer->GetID(), pkPlayer->GetInstance(), serverID, userID, charID, objectID);
	Arena_Info* pkArena = GetArenaInfo(iArenaID);
	if (NULL == pkArena)
	{
		return RE_FAIL;
	}

	m_ArenaSignups.erase(pkPlayer->GetInstance());

	LC_ActionForbid* actionForbid = pkPlayer->GetActionForbid();
	actionForbid->AddActionForbid(LAFT_GAME, LAF_GAME_PLAYER_LEVEL_UP);
	actionForbid->AddActionForbid(LAFT_GAME, LAF_GAME_SKILL_LEVEL_UP);
	actionForbid->AddActionForbid(LAFT_GROUP, LAF_GROUP_MAX_COUNT);
	actionForbid->AddActionForbid(LAFT_TRANSCRIPTION, LAF_TRANSCRIPTION_MAX_COUNT);
	actionForbid->AddActionForbid(LAFT_HOOK, LAF_HOOK_MAX_COUNT);
	actionForbid->AddActionForbid(LAFT_TEAMPLATFORM, LAF_TEAMPLATFORM_MAX_COUNT);
	actionForbid->AddActionForbid(LAFT_SACREDWEAPON, LAF_SACREDWEAPON_MAX_COUNT);

	LC_ArenaEntry* pkArenaEntry = pkPlayer->GetArenaAsset()->GetArenaEntry(pkArena->Type());
	if (pkArenaEntry)
	{
		pkArenaEntry->CommitJoinTimes();
		pkPlayer->GetArenaAsset()->UpdateDirtyFlag();
	}

	pkPlayer->GetAsAControler().OppositePeer().ServerID(serverID);
	pkPlayer->GetAsAControler().OppositePeer().UserID(userID);
	pkPlayer->GetAsAControler().OppositePeer().CharID(charID);
	pkPlayer->GetAsAControler().OppositePeer().ObjectID(objectID);

	pkPlayer->SetOutSector(true);
	pkPlayer->SyncAsControlerInfo();
	return RE_SUCCESS;
}

ResultType LC_Arena_Manager::ArenaLeave(int64_t iArenaID, LC_ServerPlayer* pkPlayer)
{
	IF_A_NA_PLAYER(pkPlayer) return RE_FAIL;
	if (!pkPlayer->IsInstance())
	{
		return RE_FAIL;
	}

	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "on arena leave", iArenaID, pkPlayer->GetID(), pkPlayer->GetInstance());
	Arena_Info* pkArena = GetArenaInfo(iArenaID);
	if (NULL == pkArena)
	{
		return RE_FAIL;
	}
		
	pkArena->CharLeave(pkPlayer->GetInstance());
	LC_ActionForbid* actionForbid = pkPlayer->GetActionForbid();
	actionForbid->RemoveActionForbid(LAFT_GAME, LAF_GAME_PLAYER_LEVEL_UP);
	actionForbid->RemoveActionForbid(LAFT_GAME, LAF_GAME_SKILL_LEVEL_UP);
	actionForbid->RemoveActionForbid(LAFT_GROUP, LAF_GROUP_MAX_COUNT);
	actionForbid->RemoveActionForbid(LAFT_TRANSCRIPTION, LAF_TRANSCRIPTION_MAX_COUNT);
	actionForbid->RemoveActionForbid(LAFT_HOOK, LAF_HOOK_MAX_COUNT);
	actionForbid->RemoveActionForbid(LAFT_TEAMPLATFORM, LAF_TEAMPLATFORM_MAX_COUNT);
	actionForbid->RemoveActionForbid(LAFT_SACREDWEAPON, LAF_SACREDWEAPON_MAX_COUNT);

	pkPlayer->ResetAsControler();
	return RE_SUCCESS;
}

ResultType LC_Arena_Manager::ArenaPrepareTimes(int64_t iArenaID)
{
	static LC_ServerPlayerManager* playManager = SERVER_GET_PLAYER_MANAGER();
	Arena_Info* pkArena = GetArenaInfo(iArenaID);
	if (NULL == pkArena)
	{
		return RE_FAIL;
	}

	unique_id_set::const_iterator itch = pkArena->InChars().begin();
	for (; itch!=pkArena->InChars().end(); ++itch)
	{
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)playManager->FindPlayer(*itch);
		if (NULL == pkPlayer)
		{
			continue;
		}

		LC_ArenaEntry* pkArenaEntry = pkPlayer->GetArenaAsset()->GetArenaEntry(pkArena->Type());
		if (pkArenaEntry)
		{
			pkArenaEntry->PrepareJoinTimes();
		}
	}
	return RE_SUCCESS;
}

ResultType LC_Arena_Manager::SetArenaResult(int64_t iArenaID, ArenaCharMap& rkArenaData)
{
	Arena_Info* pkArena = GetArenaInfo(iArenaID);

	if (NULL == pkArena)
	{
		return RE_FAIL;
	}

	static LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	//send to global
	MG_PB<msg_define::arena_settle_req> msg;
	msg_define::arena_settle_req& rk_req = msg.m_value;
	
	rk_req.mutable_arena()->CopyFrom(pkArena->arena_if);

	LC_ServerMapLogic_Place* logic = (LC_ServerMapLogic_Place*)pkWorldManager->GetMapLogic(pkArena->map_logic_id);
	ArenaCharMap::iterator it = rkArenaData.begin();
	for (; it!=rkArenaData.end(); ++it)
	{
		ArenaChar& rkChar = it->second;
		msg_define::arena_char_settle* pk_settle = rk_req.add_settle();
		if (pk_settle)
		{
			pk_settle->set_charid(rkChar.CharID());
			pk_settle->set_result(logic ? logic->GetFactionResult((FactionType)rkChar.FactionID()) : FACTION_RESULT_DRAW);
		}
	}
	
	SendMsgToGlobal(MGPT_SS_GLOBAL_ARENA_SETTLE_REQ, &msg);
	return RE_SUCCESS;
}
void LC_Arena_Manager::Update(float fCurrentTime)
{
	if (fCurrentTime < m_LastUpdateTime+60.0f)
	{
		return;
	}

	m_LastUpdateTime = fCurrentTime;
	static LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	mem::set<int64_t> failed_aids;
	ArenaInfoMap::iterator it = m_ArenaMap.begin();
	for(; it!=m_ArenaMap.end(); ++it)
	{
		Arena_Info& rkArena = it->second;
		if (rkArena.TargetServer() == GetGameServerID())
		{
			LC_ServerMapLogic_Place* logic = (LC_ServerMapLogic_Place*)pkWorldManager->GetMapLogic(rkArena.map_logic_id);
			if (NULL == logic)
			{
				failed_aids.insert(rkArena.ID());
				continue;
			}

			SyncArenaInfo(rkArena.ID());
		}
		else if(rkArena.uptime > fCurrentTime + 600.0f)
		{
			 failed_aids.insert(rkArena.ID());
		}
		else if(Arena_State_End==rkArena.state && rkArena.uptime < fCurrentTime + 60.0f)
		{
			failed_aids.insert(rkArena.ID());
		}
	}

	mem::set<int64_t>::iterator itID = failed_aids.begin();
	for (; itID!=failed_aids.end(); ++itID)
	{
		DelArenaInfo(*itID);
	}
}