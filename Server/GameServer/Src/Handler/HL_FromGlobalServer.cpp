#include "zlib.h"
#include "GameServerApp.h"
#include "HL_PacketHandler.h"
#include "NW_ServerSession.h"
#include "NW_Helper.h"

#include "LC_ServerPlayer.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerWorldManager.h"

#include "TS_TranManager.h"
#include "TS_TranType.h"

#include "TS_TranAuctionSellTran.h"
#include "TS_TranAuctionBuyTran.h"

#include "MG_CS_Auction.h"
#include "MG_CS_CurrencyTradeProtocol.h"
#include "MG_TeamPlatformProtocol.h"
#include "MG_ArenaProtocol.h"
#include "MG_CS_GuildHome.h"

#include "LC_ServerPlayerGroupManager.h"
#include "LC_ServerPlayerGroup.h"
#include "LC_ServerPlayerEnterLeaveManager.h"

#include "LC_Arena_Manager.h"
#include "LC_RankManager.h"
#include "LC_PackEntryFactory.h"
#include "MG_GroupProtocol.h"
#include "MG_Common.h"
#include "MG_PB.h"
#include "msg.pb.h"

#include "CF_ArenaSettle.h"
#include "LC_GameEventManager.h"
#include "LC_GuildLadderManager.h"
#include "LC_ServerGuildManager.h"

using namespace Protocol;
using namespace GameLogic;
using namespace Utility;
using namespace CSVFile;

namespace Protocol
{
#define this_source_type SERVER_TYPE_GLOBAL

ON_HANDLE(MGPT_RLT_SIGNUPARENA, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_PB<msg_define::arena_signup_resp> respMsg;
	if (!respMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	static LC_Arena_Manager* pkArenaMgr = SERVER_GET_ARENA_MANAGER();
	if (RE_SUCCESS == respMsg.m_value.rslt())
	{
		pkArenaMgr->ArenaSign(pkPlayer, GET_PROCESS_TIME);		
	}
	else
	{
		pkArenaMgr->ArenaUnsign(pkPlayer->GetInstance());	
	}

	pkPlayer->SendMsgToClient(MGPT_RLT_SIGNUPARENA, &respMsg);
}
ON_HANDLE(MGPT_SS_GLOBAL_ARENA_REGISTER_SERVER_REQ, this_source_type, false)
{
	MG_PB<msg_define::arena_register_server_req> msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	
	const msg_define::arena_register_server_req& rk_req = msg.m_value;
	const msg_define::server_info& rk_svr_info = rk_req.arena().target();
	PS_ServerInfo kSvrInfo;
	kSvrInfo.m_iType = rk_svr_info.type();
	kSvrInfo.m_iServerID = rk_svr_info.id();
	kSvrInfo.m_szIP = rk_svr_info.ip();
	kSvrInfo.m_iPort = rk_svr_info.port();

	MG_PB<msg_define::arena_register_server_resp> resp;
	resp.m_value.mutable_req()->CopyFrom(rk_req);

	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "arena register server", kSvrInfo.m_iType, kSvrInfo.m_iServerID, kSvrInfo.m_szIP, kSvrInfo.m_iPort);
	
	do 
	{
		static GameServerApp* pkApp = GetGameServerApp();
		//id 大的链接小的
		if (pkApp->IsBranchNode() && GetGameServerID() < kSvrInfo.m_iServerID)
		{
			resp.m_value.set_rslt(RE_FAIL);
			break;
		}

		NW_ServerSession* pkServerSession = pkApp->RegisterGameServer(kSvrInfo);
		if (pkServerSession)
		{
			if(pkServerSession->GetConnectIP().compare(kSvrInfo.m_szIP.c_str())!=0 || pkServerSession->GetConnectPort()!=kSvrInfo.m_iPort)
			{
				pkServerSession->Disconnect(TRUE);
				pkServerSession->SetServerInfo(kSvrInfo);
			}
			pkServerSession->TryToConnect();
		}
	} while (0);
	SendMsgToGlobal(MGPT_SS_GLOBAL_ARENA_REGISTER_SERVER_RESP, &resp);
}

ON_HANDLE(MGPT_SS_GLOBAL_ARENA_PREPARE_REQ, this_source_type, false)
{
	MG_PB<msg_define::arena_prepare_req> msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	const msg_define::arena_prepare_req& rk_req = msg.m_value;
	const msg_define::arena_info& rk_arena_info = rk_req.arena();
	MG_PB<msg_define::arena_prepare_resp> resp;
	resp.m_value.mutable_req()->CopyFrom(rk_req);

	static LC_Arena_Manager* pkArenaMgr = SERVER_GET_ARENA_MANAGER();
	static LC_ServerPlayerManager* pkPlayManager = SERVER_GET_PLAYER_MANAGER();

	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "arena prepare req", rk_arena_info.id(), rk_arena_info.target().id());
	ResultType rslt = RE_SUCCESS;
	do 
	{
		Arena_Info* pkArna = pkArenaMgr->AddArenaInfo(rk_arena_info, rk_req.chars());
		
		rslt = pkArenaMgr->ArenaPrepare(rk_arena_info.id());
		if (RE_SUCCESS != rslt)
		{
			break;
		}
		
		pkArenaMgr->ArenaPrepareTimes(rk_arena_info.id());
		pkArenaMgr->SyncArenaInfoToClient(rk_arena_info.id());
	} while (0);

	resp.m_value.set_rslt(rslt);
	SendMsgToGlobal(MGPT_SS_GLOBAL_ARENA_PREPARE_RESP, &resp);
}

ON_HANDLE(MGPT_SS_GLOBAL_ARENA_START_REQ, this_source_type, false)
{
	MG_PB<msg_define::arena_start_req> msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	msg_define::arena_start_req& rk_req = msg.m_value;
	const msg_define::arena_info& rk_arena_info = rk_req.arena();
	MG_PB<msg_define::arena_start_resp> resp;
	resp.m_value.mutable_req()->CopyFrom(rk_req);

	static LC_Arena_Manager* pkArenaMgr = SERVER_GET_ARENA_MANAGER();
	static LC_ServerPlayerManager* pkPlayManager = SERVER_GET_PLAYER_MANAGER();

	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "arena start req", rk_arena_info.id(), rk_arena_info.target().id());
	ResultType rslt = RE_SUCCESS;
	do 
	{
		Arena_Info* pkArena = pkArenaMgr->GetArenaInfo(rk_arena_info.id());
		if (NULL == pkArena)
		{
			rslt = RE_FAIL;
			break;
		}
	
		unique_id_set::const_iterator itch = pkArena->InChars().begin();
		for (; itch!=pkArena->InChars().end(); ++itch)
		{
			LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayManager->FindPlayer(*itch);
			if (NULL == pkPlayer)
			{
				rslt = RE_ARENA_IN_CHAR_OFFLINE;
				break;
			}
			
			MG_REQ_EnterArena Info;
			Info.m_arenaType = rk_arena_info.type();
			Info.m_arenaID = rk_arena_info.id();
			Info.m_platformID = GET_PLATFORM_ID();
			Info.m_gatewayID = pkPlayer->GetSession()->PeerID();
			Info.m_objectID = pkPlayer->GetID();
			char_data_define::char_base_info baseinfo;
			char_data_define::char_full_info fullinfo;
			pkPlayer->SaveCharPB(Info.m_lite_info, baseinfo, fullinfo);
			fullinfo.SerializeToString(&Info.m_fullinfo);
			if (Info.m_fullinfo.size() >= 0xC000)
			{
				static std::string _zipbuf;
				uLongf length = ::compressBound((uLong)Info.m_fullinfo.size());
				_zipbuf.resize((size_t)length);

				if (Z_OK == compress((Bytef*)(Utility::get_buff_ptr(_zipbuf)), (uLongf*)&length, (Bytef*)Info.m_fullinfo.c_str(), (uLong)Info.m_fullinfo.size())) 
				{
					_zipbuf.resize(length);
					GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "ARENA SaveCharPB ZIP", pkPlayer->GetID(), pkPlayer->GetInstance(), Info.m_fullinfo.size(), length);
					baseinfo.set_data_flag(1);
					Info.m_fullinfo = _zipbuf;
				}
			}
			baseinfo.SerializeToString(&Info.m_baseinfo);
			SendMsgToGame(rk_arena_info.target().id(), MGPT_G2G_REQ_ENTERARENA, &Info, pkPlayer->GetUserID(), pkPlayer->GetInstance());
		}
	} while (0);

	resp.m_value.set_rslt(rslt);
	SendMsgToGlobal(MGPT_SS_GLOBAL_ARENA_START_RESP, &resp);
}

ON_HANDLE(MGPT_SS_GLOBAL_ARENA_SETTLE_RESP, this_source_type, false)
{
	MG_PB<msg_define::arena_settle_resp> msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	const msg_define::arena_settle_resp& rk_resp = msg.m_value;
	const msg_define::arena_info& rk_arena_info = rk_resp.req().arena();
	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "arena settle resp", rk_arena_info.id(), rk_arena_info.target().id());

	static LC_Arena_Manager* pkArenaMgr = SERVER_GET_ARENA_MANAGER();
	static LC_ServerPlayerManager* pkPlayManager = SERVER_GET_PLAYER_MANAGER();
	
	do 
	{
		Arena_Info* pkArena = pkArenaMgr->GetArenaInfo(rk_arena_info.id());
		if (NULL == pkArena)
		{
			break;
		}
		
		for (int i=0; i<rk_resp.req().settle_size(); ++i)
		{
			const msg_define::arena_char_settle& rk_settle = rk_resp.req().settle(i);
			LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayManager->FindPlayer(rk_settle.charid());
			if (NULL == pkPlayer)
			{
				continue;
			}

			MG_PB<msg_define::game_event> EventMsg;
			msg_define::game_event& msg_val = EventMsg.m_value;
			msg_val.set_event_type(GLET_TRANSCRIPTION_EVENT);
			msg_val.add_event_param(EVENT_TYPE_ARENA_SETTLE);
			CF_ArenaSettle::DataEntry* pkEntry = SafeGetCSVFileDataEntryPtr<CF_ArenaSettle>(rk_settle.result());
			if (pkEntry)
			{
				attr_value_type exp = attr_value_type(pkPlayer->GetNoPeakLevel()*5+20) * pkEntry->_iExp;
				MoneyType point	= pkEntry->_iPoint;

				pkPlayer->AddExpWithGamePlay(exp, SYS_FUNC_TP_ARENA_SETTLE, rk_settle.result());
				pkPlayer->AddCash(point, CT_ARENA_POINT, SYS_FUNC_TP_ARENA_SETTLE, rk_settle.result());
				
				msg_val.add_event_param(exp);
				msg_val.add_event_param(point);
				msg_val.add_event_param(rk_settle.result());

				LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
				LC_GameEvent* pkGameEvent = pkEventManager->AllocGameEvent();
				if (pkGameEvent)
				{
					pkGameEvent->SetEventType(GLET_CROSS_ARENA_CHALLENGE);
					pkGameEvent->SetEventParam(EP_CHALLENGE_PLAYER_ID, pkPlayer->GetID());
					pkEventManager->InsertGameEvent(pkGameEvent);
				}
			}
			pkPlayer->SendMsgToClient(MGPT_RLT_GAME_EVENT, &EventMsg);	
		}

		for (int i=0; i<rk_resp.achievements_size(); ++i)
		{
			const msg_define::arena_char_achievement& rk_ach = rk_resp.achievements(i);
			LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayManager->FindPlayer(rk_ach.charid());
			if (NULL == pkPlayer)
			{
				continue;
			}

			LC_ArenaEntry* pkArenaEntry = pkPlayer->GetArenaAsset()->AddArenaEntry(rk_arena_info.type(), rk_arena_info.timestamp());
			if (pkArenaEntry)
			{
				pkArenaEntry->m_kAchievement.CopyFrom(rk_ach);
			}
		}
	}while(0);
}
ON_HANDLE(MGPT_SS_GLOBAL_ARENA_INFO_SYNC, this_source_type, false)
{
	MG_PB<msg_define::arena_info> msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	const msg_define::arena_info& rk_arena_info = msg.m_value;
	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "arena settle resp", rk_arena_info.id(), rk_arena_info.target().id(), rk_arena_info.status());

	static LC_Arena_Manager* pkArenaMgr = SERVER_GET_ARENA_MANAGER();
	switch (rk_arena_info.status())
	{
	case ladder_match_st_fail:
		pkArenaMgr->DelArenaInfo(rk_arena_info.id());
		break;
	default:
		break;
	}
}
ON_HANDLE(MGPT_SS_GLOBAL_ARENA_ACHIEVE_RESP, this_source_type, false)
{
	MG_PB<msg_define::arena_achievement_resp> msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	const msg_define::arena_achievement_resp& rk_resp = msg.m_value;
	static LC_ServerPlayerManager* pkPlayManager = SERVER_GET_PLAYER_MANAGER();
	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "arena achieve resp", rk_resp.req().type());

	for (int i=0; i<rk_resp.achievements_size(); ++i)
	{
		const msg_define::arena_char_achievement& rk_ach = rk_resp.achievements(i);
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayManager->FindPlayer(rk_ach.charid());
		if (NULL == pkPlayer)
		{
			continue;
		}

		LC_ArenaEntry* pkArenaEntry = pkPlayer->GetArenaAsset()->AddArenaEntry(rk_resp.req().type(), rk_resp.timestamp());
		if (pkArenaEntry)
		{
			pkArenaEntry->m_kAchievement.CopyFrom(rk_ach);
		}
	}

	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (pkPlayer)
	{
		pkPlayer->SendMsgToClient(MGPT_SS_GLOBAL_ARENA_ACHIEVE_RESP, &msg);
	}
}
ON_HANDLE(MGPT_SS_GLOBAL_ARENA_LADDER_RESP, this_source_type, false)
{
	MG_PB<msg_define::arena_ladder_info_resp> msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	const msg_define::arena_ladder_info_resp& rk_resp = msg.m_value;
	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "arena ladder resp", rk_resp.req().type(), rk_resp.req().index(), rk_resp.req().size(), rk_resp.ByteSize());
	pkPlayer->SendMsgToClient(MGPT_SS_GLOBAL_ARENA_LADDER_RESP, &msg);
}

ON_HANDLE(MGPT_RLT_GUILD_LADDER_SIGNUP, this_source_type, false)
{
	MG_PB<msg_define::guild_ladder_signup_resp> respMsg;
	if (!respMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}
	LC_ServerGuildInfo* guild = pkGuildManager->GetGuildInfoByID(pkPlayer->GetGuildID());
	if (guild == NULL)
	{
		return;
	}
	LC_GuildLadderManager* pkLadderMgr = LC_GuildLadderManager::GetSingletonPtr();
	if (NULL == pkLadderMgr)
	{
		return;
	}
	msg_define::guild_ladder_signup_resp& resp = respMsg.m_value;
	do {
		if (RE_SUCCESS != resp.result())
		{
			break;
		}
		mem::map<int, int64_t> cost;
		cost.insert(std::make_pair(0, pkLadderMgr->GetConfig().need_money));
		ResultType ret = guild->HandleAssert(pkPlayer, cost, true, false, GuildItemCost_Ladder, 0, 0);
		if (RE_SUCCESS != ret)
		{
			resp.set_result(ret);
			break;
		}
	} while(0);
	pkPlayer->SendMsgToClient(MGPT_RLT_GUILD_LADDER_SIGNUP, &respMsg);
}

ON_HANDLE(MGPT_SS_GLOBAL_GUILD_LADDER_REGISTER_SERVER_REQ, this_source_type, false)
{
	MG_PB<msg_define::guild_ladder_register_server_req> msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	const msg_define::guild_ladder_register_server_req& rk_req = msg.m_value;
	const msg_define::server_info& rk_svr_info = rk_req.target();
	PS_ServerInfo kSvrInfo;
	kSvrInfo.m_iType = rk_svr_info.type();
	kSvrInfo.m_iServerID = rk_svr_info.id();
	kSvrInfo.m_szIP = rk_svr_info.ip();
	kSvrInfo.m_iPort = rk_svr_info.port();

	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "guild ladder register server", kSvrInfo.m_iType, kSvrInfo.m_iServerID, kSvrInfo.m_szIP, kSvrInfo.m_iPort);
	
	MG_PB<msg_define::guild_ladder_register_server_resp> resp;
	do 
	{
		static GameServerApp* pkApp = GetGameServerApp();
		//id 大的链接小的
		if (pkApp->IsBranchNode() && GetGameServerID() < kSvrInfo.m_iServerID)
		{
			resp.m_value.set_result(RE_FAIL);
			break;
		}

		NW_ServerSession* pkServerSession = pkApp->RegisterGameServer(kSvrInfo);
		if (pkServerSession)
		{
			if(pkServerSession->GetConnectIP().compare(kSvrInfo.m_szIP.c_str())!=0 || pkServerSession->GetConnectPort()!=kSvrInfo.m_iPort)
			{
				pkServerSession->Disconnect(TRUE);
				pkServerSession->SetServerInfo(kSvrInfo);
			}
			pkServerSession->TryToConnect();
		}
	} while (0);
	SendMsgToGlobal(MGPT_SS_GLOBAL_GUILD_LADDER_REGISTER_SERVER_RESP, &resp);
}

ON_HANDLE(MGPT_RLT_GUILD_LADDER_BASE_INFO, this_source_type, false)
{
	MG_PB<msg_define::guild_ladder_base_info_resp> respMsg;
	if (!respMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return;
	}
	LC_ServerGuildInfo* guild = pkGuildManager->GetGuildInfoByID(pkPlayer->GetGuildID());
	if (guild == NULL)
	{
		return;
	}
	guild->CacheLadderInfo(respMsg.m_value);
	pkPlayer->SendMsgToClient(MGPT_RLT_GUILD_LADDER_BASE_INFO, &respMsg);
}

ON_HANDLE(MGPT_SS_GLOBAL_GUILD_LADDER_SETTLE_RESP, this_source_type, false)
{
	MG_PB<msg_define::guild_ladder_settle_resp> respMsg;
	if (!respMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	LC_GuildLadderManager* pkLadderMgr = LC_GuildLadderManager::GetSingletonPtr();
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	msg_define::guild_ladder_settle_resp resp = respMsg.m_value;
	for (int i = 0; i < resp.settles_size(); ++i)
	{
		const msg_define::guild_ladder_settle& settle = resp.settles(i);
		if (UT_ServerHelper::GetActiveId(settle.server_id()) != GetGameServerID())
		{
			continue;
		}
		Utility::UT_SIMDataList kRewards;
		StringType sMailContent;
		if (settle.rank() > 0)
		{
			StringType rewardStr = STDSTR2TPSTR(pkLadderMgr->GetConfig().GetRewardStr(settle.rank()));
			ParseFmtString2DataIFs(rewardStr, kRewards);
			stringstream score;
			score<<settle.battle_score(); 

			stringstream rank;
			rank<<settle.rank();
			sMailContent = "48;" + STDSTR2TPSTR(score.str()) + ";" +   STDSTR2TPSTR(rank.str())+ ";" ;
		}
		else
		{
			sMailContent = "49;";
		}
		for (int j = 0; j < settle.members_size(); ++j)
		{
			//信任 global传来的参赛成员ID。直接发奖励与结算协议
			LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(settle.members(j));
			if (NULL != pkPlayer)
			{
				pkPlayer->SendMsgToClient(MGPT_SS_GLOBAL_GUILD_LADDER_SETTLE_RESP, &respMsg);
			}
			UT_ServerHelper::SendToMail("50;", sMailContent, settle.members(j), "", 0, kRewards);
		}
	}
}

ON_HANDLE(MGPT_RLT_GUILD_LADDER_RANK, this_source_type, false)
{
	MG_PB<msg_define::guild_ladder_rank_resp> respMsg;
	if (!respMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	pkPlayer->SendMsgToClient(MGPT_RLT_GUILD_LADDER_RANK, &respMsg);
}
}
