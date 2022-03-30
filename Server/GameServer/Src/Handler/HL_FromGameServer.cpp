#include "HL_PacketHandler.h"
#include "UT_Log.h"
#include "UT_TimeManager.h"

#include "GameServerApp.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerPlayerEnterLeaveManager.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ActionForbid.h"
#include "LC_ServerPlayer.h"
#include "LC_Arena_Manager.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerMapLogic_ArenaPlay.h"
#include "LC_GuildLadderManager.h"
#include "LC_GameStory_Manager.h"
#include "ConfigManager.h"
#include "CF_TransferArea.h"

#include "MG_Game2Game.h"
#include "MG_ArenaProtocol.h"
#include "NW_ServerSession.h"

using namespace Protocol;
using namespace Utility;
using namespace GameLogic;
using namespace GameStory;

namespace Protocol
{
#define this_source_type SERVER_TYPE_GAME

ON_HANDLE(MGPT_G2G_REQ_ENTERARENA, this_source_type, false)
{
	MG_REQ_EnterArena reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		//GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "decode MG_REQ_EnterArena failed!");
		return;
	}

	static LC_Arena_Manager* pkArenaMgr = SERVER_GET_ARENA_MANAGER();
	static LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();

	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "game enter arena play! c_cid:%d, c_citizenship:%d, m_arenaID:%d", 
		reqMsg.m_lite_info.m_uiCharID, reqMsg.m_lite_info.m_iCitizenship, reqMsg.m_arenaID);

	Arena_Info* pkArena = pkArenaMgr->GetArenaInfo(reqMsg.m_arenaID);
	if (NULL == pkArena)
	{
		return;
	}

	LC_ServerMapLogic_Place* pkPlace = (LC_ServerMapLogic_Place*)pkWorldManager->GetMapLogic(pkArena->MapLogicID());
	if (NULL == pkPlace)
	{
		return;
	}

	LC_ServerMapLogic_ArenaPlay* pkPlay = (LC_ServerMapLogic_ArenaPlay*)pkPlace->GetPlay(PlayType_Arena);
	if (NULL == pkPlay)
	{
		return;
	}

	MG_RLT_EnterArena rltMsg;
	rltMsg.m_arenaType = reqMsg.m_arenaType;
	rltMsg.m_arenaID = reqMsg.m_arenaID;
	unique_id_impl charID = GEN_UNIQUE_ID(reqMsg.m_platformID, reqMsg.m_lite_info.m_iCitizenship, reqMsg.m_lite_info.m_uiCharID);
 	
	LC_ServerPlayer* pkPlayer = pkPlay->FindPlayer(charID);
	if (pkPlayer && !pkPlayer->GetLoadDataFromDBFlag())
	{
		static LC_ServerPlayerEnterLeaveManager* enterLeaveManager = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
		pkPlayer->LoadCharPB(reqMsg.m_lite_info, reqMsg.m_baseinfo, reqMsg.m_fullinfo);
		pkPlayer->SetLoadDataFromDBFlag(true);
		enterLeaveManager->LoadDataFromDB(pkPlayer, true);

		pkPlayer->GetAsAControlee().OppositePeer().ServerID(rkMsgHead.m_iSrcID);
		pkPlayer->GetAsAControlee().OppositePeer().UserID(rkMsgHead.m_uiUserID);
		pkPlayer->GetAsAControlee().OppositePeer().CharID(rkMsgHead.m_uiCharID);
		pkPlayer->GetAsAControlee().OppositePeer().GatewayID(reqMsg.m_gatewayID);
		pkPlayer->GetAsAControlee().OppositePeer().ObjectID(reqMsg.m_objectID);
		
		pkPlayer->SyncAsControleeInfo();
		pkPlayer->SyncLocationInfo();
	
		rltMsg.m_rslt = RE_SUCCESS;
		rltMsg.m_gatewayID = pkPlayer->GetSession() ? pkPlayer->GetSession()->PeerID() : 0;
		rltMsg.m_objectID = pkPlayer->GetID();
	}
	else
	{
		rltMsg.m_rslt = RE_FAIL;
	}
	SendMsgToGame(rkMsgHead.m_iSrcID, MGPT_G2G_RLT_ENTERARENA, &rltMsg, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID);
}

ON_HANDLE(MGPT_G2G_RLT_ENTERARENA, this_source_type, false)
{
	//一般游戏服务器处理此消息
	MG_RLT_EnterArena rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		//GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "decode MG_RLT_EnterArena failed!");
		return;
	}

	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	static LC_Arena_Manager* pkArenaMgr = SERVER_GET_ARENA_MANAGER();
	if (RE_SUCCESS == rltMsg.m_rslt)
	{
		pkArenaMgr->ArenaEnter(rltMsg.m_arenaID, pkPlayer, rkMsgHead.m_iSrcID, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID, rltMsg.m_objectID);
	}
}

ON_HANDLE(MGPT_G2G_SYN_ARENASTATUS, this_source_type, false)
{
	//一般游戏服务器处理此消息
	MG_PB<msg_define::arena_status_sync> msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	msg_define::arena_status_sync& rk_sync = msg.m_value;
	const msg_define::arena_info& rk_arena_info = rk_sync.arena();

	static LC_Arena_Manager* pkArenaMgr = SERVER_GET_ARENA_MANAGER();
	//static LC_ServerPlayerManager* playerManager = SERVER_GET_PLAYER_MANAGER();
	ResultType rslt = RE_SUCCESS;
	do 
	{
		Arena_Info* pkArena = pkArenaMgr->GetArenaInfo(rk_arena_info.id());
		if (NULL == pkArena)
		{
			rslt = RE_FAIL;
			break;
		}

		pkArena->state = rk_sync.state();
		pkArena->uptime = GET_PROCESS_TIME;
	}while(0);
}

ON_HANDLE(MGPT_G2G_SYN_MATCHARENA, this_source_type, false)
{
	//一般游戏服务器处理此消息
}

ON_HANDLE(MGPT_G2G_REQ_LEAVEARENA, this_source_type, false)
{
	MG_PB<msg_define::arena_leave_req> msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	
	msg_define::arena_leave_req& rk_req = msg.m_value;
	const msg_define::arena_info& rk_arena_info = rk_req.arena();

	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "game req leave arena!", rkMsgHead.m_uiCharID, rk_arena_info.id(), rk_req.objectid());
	static LC_Arena_Manager* pkArenaMgr = SERVER_GET_ARENA_MANAGER();
	static LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();

	MG_PB<msg_define::arena_leave_resp> resp;
	resp.m_value.mutable_req()->CopyFrom(rk_req);
	do 
	{
		Arena_Info* pkArena = pkArenaMgr->GetArenaInfo(rk_arena_info.id());
		if (NULL == pkArena)
		{
			GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "game req leave arena null!", rk_arena_info.id());
			break;
		}

		LC_ServerMapLogic_Place* pkPlace = (LC_ServerMapLogic_Place*)pkWorldManager->GetMapLogic(pkArena->MapLogicID());
		if (NULL == pkPlace)
		{
			GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "game req leave arena null map!", pkArena->MapLogicID());
			break;
		}

		LC_ServerMapLogic_ArenaPlay* pkPlay = (LC_ServerMapLogic_ArenaPlay*)pkPlace->GetPlay(PlayType_Arena);
		if (NULL == pkPlay)
		{
			GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "game req leave arena null play!");
			break;
		}

		LC_ServerPlayer* pkPlayer = pkPlay->FindPlayer(rkMsgHead.m_uiCharID);
		if (NULL == pkPlayer)
		{
			GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "game req leave arena null player!", rkMsgHead.m_uiCharID);
			break;
		}
		pkPlace->SetPlayerQuitState(pkPlayer);
	} while (0);
	SendMsgToGame(rkMsgHead.m_iSrcID, MGPT_G2G_RLT_LEAVEARENA, &resp, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID);
}

ON_HANDLE(MGPT_G2G_RLT_LEAVEARENA, this_source_type, false)
{
	//一般游戏服务器处理此消息
	MG_PB<msg_define::arena_leave_resp> msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	static LC_Arena_Manager* pkArenaMgr = SERVER_GET_ARENA_MANAGER();
	msg_define::arena_leave_resp& rk_resp = msg.m_value;
	const msg_define::arena_info& rk_arena_info = rk_resp.req().arena();

	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "game rlt leave arena!", pkPlayer->GetInstance(), rk_arena_info.id());
	pkArenaMgr->ArenaLeave(rk_arena_info.id(), pkPlayer);
}

ON_HANDLE(MGPT_G2G_GUILD_LADDER_ENTER_REQ, this_source_type, false)
{
	ResultType iResult = RE_FAIL;
	int32_t iObjectID = 0;
	do 
	{
		MG_PB<msg_define::guild_ladder_ss_enter_req> kReqMsg;
		if (!kReqMsg.Decode(pMsgBody, usBodySize))
		{
			break;
		}
		msg_define::guild_ladder_ss_enter_req& msg = kReqMsg.m_value;

		user_id_type userID = rkMsgHead.m_uiUserID;
		unique_id_type charID = rkMsgHead.m_uiCharID;
		static LC_GuildLadderManager* pkGuildLadderMgr = LC_GuildLadderManager::GetSingletonPtr();
		if (NULL == pkGuildLadderMgr)
		{
			break;
		}
		int32_t iMapResID = pkGuildLadderMgr->GetMapResID();
		static LC_GameStory_Manager* pkStoryMgr = SERVER_GET_GAMESTORY_MANAGER;
		if (NULL == pkStoryMgr)
		{
			break;
		}
		CF_WorldMapList* pkCSVWorldMapList = CF_WorldMapList::GetSingletonPtr();
		CF_WorldMapList::DataEntry* pkMapDataEntry = pkCSVWorldMapList->GetEntryPtr(iMapResID);
		if (pkMapDataEntry == NULL)
		{
			break;
		}
		if (!pkStoryMgr->CheckIsActiveStory(pkMapDataEntry->_iStoryID))
		{
			break;
		}
		unique_id_impl uid = 1;
		//for test 
		if (pkGuildLadderMgr->GetMatchSize() <= 0)
		{
			pkGuildLadderMgr->FillLadderMatchInfo();
		}

		GuildLadderMatchData* pkMatchData = pkGuildLadderMgr->GetMatchDataByGuildUID(uid);
		if (NULL == pkMatchData)
		{
			break;
		}

		//fix the spwan pos by faction
		int32_t iFactionId = pkMatchData->GetFaction(uid);
		if (FACTION_NONE == iFactionId)
		{
			break;
		}
		int32_t iPosIndex = iFactionId - FACTION_RED;
		MapRoadmapManager* inst = SafeGetSingleton<MapRoadmapManager>();
		CF_MapCategory::DataEntry* roadmap = inst->GetRoadmap(iMapResID);
		UT_Vec3Int	kTargetLocation;
		if (roadmap)
		{
			CF_TransferArea* tr = CF_TransferArea::GetSingletonPtr();
			CF_TransferArea::DataEntry* dt = tr->GetEntryPtr(roadmap->_iTransferAreaID[iPosIndex]);
			if (NULL == dt)
			{
				break;
			}
			kTargetLocation = dt->_kPos;
		}

		//first enter to create maplogic
		LC_ServerWorldManager* pkServerWorldManager = SERVER_GET_WORLD_MANAGER();
		LC_ServerMapLogic_Place* pkPlace = NULL;
		int32_t iMaplogicID = pkMatchData->m_iMapLogicID;
		if (iMaplogicID <= 0)
		{
			pkPlace = pkServerWorldManager->CreateServerMapLogic(0, iMapResID, pkMapDataEntry->_iMapLogicType);
			if (NULL == pkPlace)
			{
				break;
			}
			pkMatchData->m_iMapLogicID = pkPlace->GetMapLogicID();
		}
		else
		{
			pkPlace = pkServerWorldManager->GetMapLogic(iMaplogicID);
			if (NULL == pkPlace)
			{
				break;
			}
		}

		LC_ServerPlayer* pkTargetPlayer = NULL;
		GameLogic::LC_ActorPtrList kAllActorPtr;
		pkPlace->GetAllPlayer(kAllActorPtr);
		GameLogic::LC_ActorPtrList::iterator iter;
		TRAV(kAllActorPtr, iter)
		{
			LC_ServerPlayer* pkPlayerTmp = (LC_ServerPlayer*)(*iter);
			object_id_type id = pkPlayerTmp->GetID();
			if (IS_PLAYER_CLONE_ID(id) && pkPlayerTmp->GetInstanceUniqueID() == charID)
			{
				pkTargetPlayer = pkPlayerTmp;
				break;
			}
		}
		if (NULL != pkTargetPlayer)
		{
			pkPlace->ClearPlayer(pkTargetPlayer);
		}
		Protocol::PS_CharBaseInfoLite kLiteInfo;
		const std::string& strLiteInfo = msg.lite_info();
		const std::string& kBaseInfo = msg.base_info();
		const std::string& kFullInfo = msg.full_info();
		static GameServerApp* pkGameServerApp = GetGameServerApp();
		if (NULL == pkGameServerApp)
		{
			break;
		}
		MG_INPUT_CDR kLiteInfoStream(strLiteInfo.c_str(), strLiteInfo.length());
		if (!kLiteInfo.Decompress(&kLiteInfoStream))
		{
			break;
		}
		if (!kLiteInfoStream.GoodBit())
		{
			break;
		}
		LC_ServerPlayer* pkSpawnPlayer = pkPlace->SpawnPlayer(charID, kTargetLocation, PLY_DEF_PROXY_BITS, NULL, NULL, (FactionType)iFactionId);
		if (NULL == pkSpawnPlayer)
		{
			break;
		}
		static LC_ServerPlayerEnterLeaveManager* enterLeaveManager = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
		if (NULL == enterLeaveManager)
		{
			break;
		}
		pkSpawnPlayer->LoadCharPB(kLiteInfo, kBaseInfo, kFullInfo);
		pkSpawnPlayer->SetLoadDataFromDBFlag(true);
		pkSpawnPlayer->SetUserID(rkMsgHead.m_uiUserID);
		enterLeaveManager->LoadDataFromDB(pkSpawnPlayer, true);

		pkSpawnPlayer->GetAsAControlee().OppositePeer().ServerID(rkMsgHead.m_iSrcID);
		pkSpawnPlayer->GetAsAControlee().OppositePeer().UserID(rkMsgHead.m_uiUserID);
		pkSpawnPlayer->GetAsAControlee().OppositePeer().CharID(rkMsgHead.m_uiCharID);
		pkSpawnPlayer->GetAsAControlee().OppositePeer().GatewayID(rkMsgHead.m_iSequence);
		pkSpawnPlayer->GetAsAControlee().OppositePeer().ObjectID(msg.object_id());

		pkSpawnPlayer->SyncAsControleeInfo();
		pkSpawnPlayer->SyncLocationInfo();

		iResult = RE_SUCCESS;
		iObjectID = pkSpawnPlayer->GetID();
		pkGuildLadderMgr->OnSpawPlayerFinished(pkSpawnPlayer, pkMatchData);
	} while (0);
	MG_PB<msg_define::guild_ladder_ss_enter_resp> kRespMsg;
	kRespMsg.m_value.set_result(iResult);
	kRespMsg.m_value.set_object_id(iObjectID);
	SendMsgToGame(rkMsgHead.m_iSrcID, MGPT_G2G_GUILD_LADDER_ENTER_RSP, &kRespMsg, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID);
}

ON_HANDLE(MGPT_G2G_GUILD_LADDER_ENTER_RSP, this_source_type, false)
{
	MG_PB<msg_define::guild_ladder_ss_enter_resp> kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	if (RE_SUCCESS == kMsg.m_value.result())
	{
		if (!pkPlayer->IsInstance())
		{
			return;
		}
		
		LC_ActionForbid* actionForbid = pkPlayer->GetActionForbid();
		actionForbid->AddActionForbid(LAFT_GAME, LAF_GAME_PLAYER_LEVEL_UP);
		actionForbid->AddActionForbid(LAFT_GAME, LAF_GAME_SKILL_LEVEL_UP);
		actionForbid->AddActionForbid(LAFT_GROUP, LAF_GROUP_MAX_COUNT);
		actionForbid->AddActionForbid(LAFT_TRANSCRIPTION, LAF_TRANSCRIPTION_MAX_COUNT);
		actionForbid->AddActionForbid(LAFT_HOOK, LAF_HOOK_MAX_COUNT);
		actionForbid->AddActionForbid(LAFT_TEAMPLATFORM, LAF_TEAMPLATFORM_MAX_COUNT);
		actionForbid->AddActionForbid(LAFT_SACREDWEAPON, LAF_SACREDWEAPON_MAX_COUNT);

		pkPlayer->GetAsAControler().OppositePeer().ServerID(rkMsgHead.m_iSrcID);
		pkPlayer->GetAsAControler().OppositePeer().UserID(rkMsgHead.m_uiUserID);
		pkPlayer->GetAsAControler().OppositePeer().CharID(rkMsgHead.m_uiCharID);
		pkPlayer->GetAsAControler().OppositePeer().ObjectID(kMsg.m_value.object_id());

		pkPlayer->SetOutSector(true);
		pkPlayer->SyncAsControlerInfo();
	}
}

ON_HANDLE(MGPT_G2G_GUILD_LADDER_LEAVE, this_source_type, false)
{
	user_id_type userID = rkMsgHead.m_uiUserID;
	unique_id_impl charID = rkMsgHead.m_uiCharID;
	static LC_GuildLadderManager* pkLadderGuildMgr = LC_GuildLadderManager::GetSingletonPtr();
	if (NULL == pkLadderGuildMgr)
	{
		return;
	}
	GuildLadderMatchData* pkMatchData = pkLadderGuildMgr->GetMatchDataByCharID(charID);
	if (NULL == pkMatchData)
	{
		return;
	}
	int32_t iMapLogicID = pkMatchData->m_iMapLogicID;
	if (iMapLogicID <= 0)
	{
		return;
	}
	static LC_ServerWorldManager* pkWorldMgr = SERVER_GET_WORLD_MANAGER();
	LC_ServerMapLogic_Place* pkPlace = pkWorldMgr->GetMapLogic(iMapLogicID);
	if (NULL == pkPlace)
	{
		return;
	}
	LC_ServerPlayer* pkTargetPlayer = NULL;
	GameLogic::LC_ActorPtrList kAllActorPtr;
	pkPlace->GetAllPlayer(kAllActorPtr);
	GameLogic::LC_ActorPtrList::iterator iter;
	TRAV(kAllActorPtr, iter)
	{
		LC_ServerPlayer* pkPlayerTmp = (LC_ServerPlayer*)(*iter);
		object_id_type id = pkPlayerTmp->GetID();
		if (IS_PLAYER_CLONE_ID(id) && pkPlayerTmp->GetInstanceUniqueID() == charID)
		{
			pkTargetPlayer = pkPlayerTmp;
			break;
		}
	}
	if (NULL != pkTargetPlayer)
	{
		pkPlace->ClearPlayer(pkTargetPlayer);
	}
}

}
