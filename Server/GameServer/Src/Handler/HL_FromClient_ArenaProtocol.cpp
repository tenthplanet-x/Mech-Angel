#include "MG_ArenaProtocol.h"
#include "LC_ServerLogicManager.h"
#include "LC_Dule_Manager.h"
#include "HL_PacketHandler.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "GameServerApp.h"
#include "LC_SequenceManager.h"
#include "NW_ServerSession.h"
#include "LC_ServerPlayerGroup.h"
#include "LC_ServerPlayerEnterLeaveManager.h"
#include "LC_RankManager.h"
#include "LC_Arena_Manager.h"
#include "GlobalSettings.h"
#include "ACT_ActivitySchedule.h"
#include "UT_OperateLog.h"
#include "CF_SkyArenaRank.h"
#include "LC_Define.h"
using namespace OPERATELOG;
using namespace Protocol;
using namespace GameLogic;
using namespace CSVFile;

namespace Protocol
{
#define this_source_type SERVER_TYPE_CLIENT
//--------------------------------------------------
ON_HANDLE(MGPT_REQ_SIGNUPARENA, this_source_type, false)
{
	//一般游戏服务器
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_PB<msg_define::arena_signup_req> reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	msg_define::arena_signup_req& rk_req = reqMsg.m_value;

	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "req sign up arena!", pkPlayer->GetInstance());
	static LC_Arena_Manager* pkArenaMgr = SERVER_GET_ARENA_MANAGER();
	MG_PB<msg_define::arena_signup_resp> respMsg;
	respMsg.m_value.set_type(reqMsg.m_value.type());
	respMsg.m_value.set_charid(pkPlayer->GetInstance());
	do
	{
		CF_ArenaSetting::DataEntry* pkSettingDataEntry = SafeGetCSVFileDataEntryPtr<CF_ArenaSetting>(reqMsg.m_value.type());
		if (NULL == pkSettingDataEntry)
		{
			respMsg.m_value.set_rslt(RE_FAIL);
			break;
		}

		static Activity::ACT_ScheduleManager* pkScheduleMgr = Activity::ACT_ScheduleManager::GetSingletonPtr();
		if (!pkScheduleMgr->IsScheduleValid(GetGameServerID(), pkSettingDataEntry->_iSchedule))
		{
			respMsg.m_value.set_rslt(RE_ACTIVITY_NOT_VALID);
			break;
		}

		if(UT_ServerHelper::IsInCross(pkPlayer))
		{
			respMsg.m_value.set_rslt(RE_ON_CROSS_SERVER);
			break;
		}
 
		if (LC_Helper::isInArena(pkPlayer) || LC_Helper::isInTranscript(pkPlayer))//正在副本中不允许
		{
			respMsg.m_value.set_rslt(RE_SOLO_DULE_IN_TRANSCRIPTION);
			break;
		}

		if (pkArenaMgr->IsCharSignValid(pkPlayer->GetInstance(), GET_PROCESS_TIME))
		{
			respMsg.m_value.set_rslt(RE_ARENA_IN_SIGN);
			break;
		}

		LC_ArenaEntry* pkArenaEntry = pkPlayer->GetArenaAsset()->GetArenaEntry(rk_req.type());
		if (NULL == pkArenaEntry)
		{
			respMsg.m_value.set_rslt(RE_FAIL);
			break;
		}

		LC_ArenaStatEntry* pkStatEntry = pkArenaEntry->AddStatEntry(LC_ST_TP_DAILY);
		if (NULL == pkStatEntry)
		{
			respMsg.m_value.set_rslt(RE_FAIL);
			break;
		}
		
		int32_t iTimes = pkStatEntry->m_iTimes;
		if (iTimes >= CF_ArenaTimes::GetSingletonPtr()->GetCurrentMaxID())
		{
			iTimes = CF_ArenaTimes::GetSingletonPtr()->GetCurrentMaxID() - 1;
		}
		CF_ArenaTimes::DataEntry* pkDataEntry = SafeGetCSVFileDataEntryPtr<CF_ArenaTimes>(iTimes);
		if (NULL == pkDataEntry)
		{
			respMsg.m_value.set_rslt(RE_FAIL);
			break;
		}

		if (!pkDataEntry->_kPrices.empty() && !pkStatEntry->_isBitSet(LC_ARN_ST_EXTRA))
		{
			respMsg.m_value.set_rslt(RE_FAIL);
			break;
		}

		reqMsg.m_value.set_charid(pkPlayer->GetInstance());
		//reqMsg.m_value.set_battle(pkPlayer->GetTotalScore());
		reqMsg.m_value.set_battle(pkPlayer->GetControllerCombatScore());
		char_data_define::char_lite_info linfo;
		pkPlayer->PBSaveCharLiteInfo(linfo);
		reqMsg.m_value.set_liteinfo(linfo.SerializeAsString());
		char_data_define::char_base_info binfo;
		pkPlayer->PBSaveCharBaseInfo(binfo);
		reqMsg.m_value.set_baseinfo(binfo.SerializeAsString());
		SendMsgToGlobal(MGPT_REQ_SIGNUPARENA, &reqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());
		return;
	}while(0);
	pkPlayer->SendMsgToClient(MGPT_RLT_SIGNUPARENA, &respMsg);
}
//--------------------------------------------------
ON_HANDLE(MGPT_REQ_ARENA_OPERTAION, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_PB<msg_define::arena_opertaion_req> reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	const msg_define::arena_opertaion_req& rk_req = reqMsg.m_value;
	MG_PB<msg_define::arena_opertaion_resp> respMsg;
	respMsg.m_value.mutable_req()->CopyFrom(rk_req);
	respMsg.m_value.set_rslt(RE_FAIL);
	
	LC_ArenaEntry* pkArenaEntry = pkPlayer->GetArenaAsset()->GetArenaEntry(rk_req.type());

	if (pkArenaEntry)
	{
		switch(rk_req.op_code())
		{
		case LC_ARN_OP_SECTION_REWARD:
			{
				if (rk_req.op_params_size() < 1)
				{
					break;
				}
				CF_ArenaSection::DataEntry* pkDataEntry = SafeGetCSVFileDataEntryPtr<CF_ArenaSection>(rk_req.op_params(0));
				if (NULL == pkDataEntry || pkDataEntry->_iScore > pkArenaEntry->GetScore())
				{
					break;
				}
				if (pkArenaEntry->_isBitSet(pkDataEntry->_iIndex))
				{
					break;
				}

				respMsg.m_value.set_rslt(UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, pkDataEntry->_Rewards, pkPlayer, SYS_FUNC_TP_ARENA_SECTION, pkDataEntry->_iID, "", "", false));
				if (RE_SUCCESS != respMsg.m_value.rslt())
				{
					break;
				}
				pkArenaEntry->_regBit(pkDataEntry->_iIndex);
				pkPlayer->GetArenaAsset()->UpdateDirtyFlag();
			}
			break;
		case LC_ARN_OP_DAILY_REWARD:
			{
				LC_ArenaStatEntry* pkStatEntry = pkArenaEntry->AddStatEntry(LC_ST_TP_DAILY);
				if (NULL == pkStatEntry)
				{
					break;
				}

				if (pkStatEntry->_isBitSet(LC_ARN_ST_REWARD))
				{
					break;
				}
				
				CF_ArenaSection::DataEntry* pkDataEntry = GameLogic::GetArenaSectionEntry(pkArenaEntry->GetScore());
				if (NULL == pkDataEntry || pkDataEntry->_DailyRewards.empty())
				{
					break;
				}

				respMsg.m_value.set_rslt(UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, pkDataEntry->_DailyRewards, pkPlayer, SYS_FUNC_TP_ARENA_DAILY, pkDataEntry->_iID, "", "", false));
				if (RE_SUCCESS != respMsg.m_value.rslt())
				{
					break;
				}
				pkStatEntry->_regBit(LC_ARN_ST_REWARD);
				pkPlayer->GetArenaAsset()->UpdateDirtyFlag();
			}
			break;
		case LC_ARN_OP_BUY_TIMES:
			{
				LC_ArenaStatEntry* pkStatEntry = pkArenaEntry->AddStatEntry(LC_ST_TP_DAILY);
				if (NULL == pkStatEntry)
				{
					break;
				}

				if (pkStatEntry->_isBitSet(LC_ARN_ST_EXTRA))
				{
					break;
				}

				int32_t iTimes = pkStatEntry->m_iTimes;
				if (iTimes >= CF_ArenaTimes::GetSingletonPtr()->GetCurrentMaxID())
				{
					iTimes = CF_ArenaTimes::GetSingletonPtr()->GetCurrentMaxID() - 1;
				}
				CF_ArenaTimes::DataEntry* pkDataEntry = SafeGetCSVFileDataEntryPtr<CF_ArenaTimes>(iTimes);
				if (NULL==pkDataEntry || pkDataEntry->_kPrices.empty())
				{
					break;
				}

				respMsg.m_value.set_rslt(LC_Helper::CheckCashMap(pkPlayer, pkDataEntry->_kPrices));
				if (RE_SUCCESS != respMsg.m_value.rslt())
				{
					break;
				}

				LC_Helper::PayCashMap(pkPlayer, pkDataEntry->_kPrices, SYS_FUNC_TP_ARENA_TIMES, pkDataEntry->_iID);

				pkStatEntry->_regBit(LC_ARN_ST_EXTRA);
				pkPlayer->GetArenaAsset()->UpdateDirtyFlag();
			}
			break;
		default:
			break;
		}
	}

	pkPlayer->SendMsgToClient(MGPT_RESP_ARENA_OPERTAION, &respMsg);
}
//--------------------------------------------------
ON_HANDLE(MGPT_REQ_LEAVEARENA, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	static LC_Arena_Manager* pkArenaMgr = SERVER_GET_ARENA_MANAGER();
	Arena_Info* pkArena = pkArenaMgr->GetArenaInfoByCharID(pkPlayer->GetInstance());
	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "req leave arena!", pkPlayer->GetInstance(), pkArena ? pkArena->ID() : 0);
	if (NULL == pkArena)
	{
		return;
	}

	if (pkPlayer->GetAsAControler().IsValid())
	{
		MG_PB<msg_define::arena_leave_req> reqMsg;
		msg_define::arena_leave_req& rk_req = reqMsg.m_value;
		rk_req.mutable_arena()->CopyFrom(pkArena->arena_if);
		rk_req.set_objectid(pkPlayer->GetControleePeer().ObjectID());
		SendMsgToGame(pkPlayer->GetControleePeer().ServerID(), MGPT_G2G_REQ_LEAVEARENA, &reqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());
	}
}
//--------------------------------------------------
ON_HANDLE(MGPT_REQ_RANK_QUERY_CHALLENGE_INFO, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, " pkPlayer is NULL ");
		return;
	}
	MG_REQ_UserChallengeInfo AllInfoMsg;
	if (!AllInfoMsg.Decode(pMsgBody, usBodySize))
	{
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, " msg Decode ! id[%lld]", pkPlayer->GetInstance());
		return;
	}
	LC_RankManager* rank_Manager = SERVER_GET_RANK_MANAGER();
	ResultType ret = rank_Manager->QueryUserChallenge(pkPlayer);
	if (ret != RE_SUCCESS)
	{
		MG_GG_RESP_Query_User_Challenge sendmsg;
		sendmsg.m_iResult = RE_FAIL;
		sendmsg.m_Size = 0;
		sendmsg.m_TotalSize = 0;
		pkPlayer->SendMsgToClient(MGPT_RESP_RANK_QUERY_CHALLENGE_INFO, &sendmsg);
	}
}
//--------------------------------------------------
ON_HANDLE(MGPT_REQ_RANK_INFO, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_RankInfo AllInfoMsg;
	if (!AllInfoMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	const unique_id_impl& instance = pkPlayer->GetInstance();
	LC_RankManager* rank_Manager = SERVER_GET_RANK_MANAGER();
	ResultType res = rank_Manager->QueryRankInfo(pkPlayer, AllInfoMsg);
	if(res != RE_SUCCESS)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "QueryRankInfo error:%d, c_cid:%d, c_citizenship:%d, c_uid:%d\n", res, instance.data(), instance.catagory(), pkPlayer->GetUserID());
	}
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, " MGPT_REQ_RANK_INFO :%d, c_cid:%d, c_citizenship:%d, c_uid:%d\n", res, instance.data(), instance.catagory(), pkPlayer->GetUserID());
}



//--------------------------------------------------
ON_HANDLE(MGPT_CHALLENG_RANK_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_RankInfo AllInfoMsg;
	if (!AllInfoMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	const unique_id_impl& instance = pkPlayer->GetInstance();
	LC_RankManager* rank_Manager = SERVER_GET_RANK_MANAGER();
	ResultType res = rank_Manager->QueryChallengRankInfo(pkPlayer, AllInfoMsg.m_Begin, AllInfoMsg.m_End, AllInfoMsg.m_ServerID);
	if(res != RE_SUCCESS)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, " MGPT_REQ_RANK_INFO error:%d, c_cid:%d, c_citizenship:%d, c_uid:%d\n", res, instance.data(), instance.catagory(), pkPlayer->GetUserID());
	}
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, " MGPT_CHALLENG_RANK_REQ :%d, c_cid:%d, c_citizenship:%d, c_uid:%d\n", res, instance.data(), instance.catagory(), pkPlayer->GetUserID());
}

ON_HANDLE(MGPT_REQ_GETRANK_PLAYERBUDDY_INFO, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_GetRankPlayerBuddyInfo reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	if(reqMsg.m_ServerID==0 || reqMsg.m_CharID==0)
	{
		return;
	}

	SendMsgToGroup(MGPT_GG_REQ_GETRANK_PLAYERBUDDY_INFO, &reqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());
}

ON_HANDLE(MGPT_REQ_GETRANK_BUDDY_INFO, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_GetRankBuddyInfo reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	if(reqMsg.m_ServerID==0 || reqMsg.m_CharID==0)
	{
		return;
	}

	SendMsgToGroup(MGPT_GG_REQ_GETRANK_BUDDY_INFO, &reqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());
}

ON_HANDLE(MGPT_REQ_GETRANK_PET_INFO, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_GetRankPetInfo reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	if(reqMsg.m_ServerID==0 || reqMsg.m_CharID==0)
	{
		return;
	}

	SendMsgToGroup(MGPT_GG_REQ_GETRANK_PET_INFO, &reqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());
}

ON_HANDLE(MGPT_REQ_GETRANK_SUB_INFO, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_GetRankSubInfo reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	if(reqMsg.m_ServerID==0 || reqMsg.m_CharID==0)
	{
		return;
	}

	SendMsgToGroup(MGPT_GG_REQ_GETRANK_SUB_INFO, &reqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());
}

ON_HANDLE(MGPT_REQ_RANK_TOPN_PLAYER_INFO, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_RankTopNPlayerInfo reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	SendMsgToGroup(MGPT_GG_REQ_RANK_TOPN_PLAYER_INFO, &reqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());
}

ON_HANDLE(MGPT_REQ_GETBUDDYRANKINDEX, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_GetBuddyRankIndex reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	SendMsgToGroup(MGPT_GG_REQ_GETBUDDYRANKINDEX, &reqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());
}

//--------------------------------------------------
ON_HANDLE(MGPT_REQ_BATTLE_INFO, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_BattleInfo AllInfoMsg;
	if (!AllInfoMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_RankManager* rank_Manager = SERVER_GET_RANK_MANAGER();
	ResultType ret = rank_Manager->QueryBattleInfo(pkPlayer, AllInfoMsg.m_RankID);
	if (ret != RE_SUCCESS)
	{
		MG_RESP_BattleInfo sendmsg;
		sendmsg.m_battlesize = 0;
		sendmsg.m_topsize = 0;
		sendmsg.m_RankID = AllInfoMsg.m_RankID;
		pkPlayer->SendMsgToClient(MGPT_RESP_BATTLE_INFO, &sendmsg);
	}
}
//--------------------------------------------------
ON_HANDLE(MGPT_CS_GUILD_RANK_CHALLENGE_REQ, this_source_type, false)
{
	if(!UT_ServerHelper::CheckStoryIsActive(GameStoryIDType_BestGuild))
		return;
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;
	MG_REQ_GuildChallenge AllInfoMsg;
	if (!AllInfoMsg.Decode(pMsgBody, usBodySize))
		return;
	LC_Dule_Manager* dule_Manager = SERVER_GET_DULE_MANAGER();
	ResultType ret = dule_Manager->GetSoloDuleManager(GameLogic::eSoloDuleType_GuildOffline)->ChallengeCheck(pkPlayer, AllInfoMsg.m_beChallengeCharID, 0);
	if (ret != RE_SUCCESS)
	{
		pkPlayer->SendChallengeResToClient(ret,AllInfoMsg.m_beChallengeCharID,GameLogic::eSoloDuleType_GuildOffline );
	}
}
//--------------------------------------------------
ON_HANDLE(MGPT_REQ_RANK_CHALLENGE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;
	MG_REQ_Challenge AllInfoMsg;
	if (!AllInfoMsg.Decode(pMsgBody, usBodySize))
		return;
	int nChallengeType = AllInfoMsg.m_ChallengeType;
	ResultType ret = 0;
	LC_Dule_Manager* dule_Manager = SERVER_GET_DULE_MANAGER();
	if( Arena_Type::Arena_Type_Challenge == nChallengeType)
		ret = dule_Manager->GetSoloDuleManager(GameLogic::eSoloDuleType_Arena)->ChallengeCheck(pkPlayer, AllInfoMsg.m_beChallengeCharID, AllInfoMsg.m_BeChallengeIndex);
	else if ( Arena_Type::Arena_Type_Firmament  == nChallengeType)
		ret = dule_Manager->GetSoloDuleManager(GameLogic::eSoloDuleType_Sky)->ChallengeCheck(pkPlayer, AllInfoMsg.m_beChallengeCharID, AllInfoMsg.m_BeChallengeIndex);
	else if (Arena_Type::Arena_Type_Peak_Group == nChallengeType)
		ret = dule_Manager->GetSoloDuleManager(GameLogic::eSoloDuleType_Peak_Team)->ChallengeCheck(pkPlayer, AllInfoMsg.m_beChallengeCharID, AllInfoMsg.m_BeChallengeIndex);
	
	pkPlayer->SendChallengeResToClient(ret,AllInfoMsg.m_beChallengeCharID,(GameLogic::SOLO_DULE_TYPE)AllInfoMsg.m_ChallengeType );
	
}
//--------------------------------------------------
ON_HANDLE(MGPT_REQ_GET_CHALLENGE_CD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_GetChallengeCD AllInfoMsg;
	if (!AllInfoMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_Dule_Manager* dule_Manager = SERVER_GET_DULE_MANAGER();
	dule_Manager->GetSoloDuleManager(GameLogic::eSoloDuleType_Arena)->GetChallengeCD(pkPlayer);
}
//--------------------------------------------------
ON_HANDLE(MGPT_REQ_CLEAR_CHALLENGE_CD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_ClearChallengeCD AllInfoMsg;
	if (!AllInfoMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_Dule_Manager* dule_Manager = SERVER_GET_DULE_MANAGER();
	//竞技场冷却刷新--埋点日志
	int nLogCode = LC_Helper::SerializeOperateLogCode(FUBEN_LOGTYPE, FUBEN11_OPERATE, OPERATE_LOGTYPE4);
	ResultType ret = dule_Manager->GetSoloDuleManager(GameLogic::eSoloDuleType_Arena)->ClearChallengeCD(pkPlayer,nLogCode);
	if (ret != RE_SUCCESS)
	{
		MG_RESP_ClearChallengeCD sendmsg;
		sendmsg.m_iResult = ret;
		pkPlayer->SendMsgToClient(MGPT_RESP_CLEAR_CHALLENGE_CD, &sendmsg);
	}
}
//--------------------------------------------------
ON_HANDLE(MGPT_REQ_ADD_CHALLENGE_TIME, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_AddChallengeTimes AllInfoMsg;
	if (!AllInfoMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	//竞技场次数购买--埋点日志
	int localDailyChallengeBuyCount = pkPlayer->GetArenaBattleAssert()->GetLocalDailyChallengeBuyCount();
	vector<int> nParams;
	nParams.clear();
	nParams.push_back(localDailyChallengeBuyCount+1);
 	int nLogCode = LC_Helper::SerializeOperateLogCode(FUBEN_LOGTYPE, FUBEN11_OPERATE, OPERATE_LOGTYPE3);
	LC_Dule_Manager* dule_Manager = SERVER_GET_DULE_MANAGER();
	
	ResultType ret = dule_Manager->GetSoloDuleManager(GameLogic::eSoloDuleType_Arena)->AddChallengeTime(pkPlayer,nLogCode, nParams);
	if (ret != RE_SUCCESS)
	{
		MG_RESP_AddChallengeTimes sendmsg;
		sendmsg.m_iResult = ret;
		pkPlayer->SendMsgToClient(MGPT_RESP_ADD_CHALLENGE_TIME, &sendmsg);
	}
	pkPlayer->SyncArenaBattleToClient();
}
ON_HANDLE(MGPT_REQ_GUILD_FIGHT_ROB_SCORE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_Req_GuildFightRobScore_Opt AllInfoMsg;
	if (!AllInfoMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_Resp_GuildFightRobScore_Opt sendmsg;
	LC_Dule_Manager* dule_Manager = SERVER_GET_DULE_MANAGER();
	sendmsg.m_uiResult = dule_Manager->GetSoloDuleManager(GameLogic::eSoloDuleType_GuildOffline)->GuildOfflineChallengeTimeOP(pkPlayer, AllInfoMsg.m_OptType,AllInfoMsg.m_nBuyNum, sendmsg);
	
	pkPlayer->SendMsgToClient(MGPT_RESP_GUILD_FIGHT_ROB_SCORE, &sendmsg);
}
//--------------------------------------------------
ON_HANDLE(MGPT_REQ_DAILY_AWARD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_Daily_Award AllInfoMsg;
	if (!AllInfoMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_Dule_Manager* dule_Manager = SERVER_GET_DULE_MANAGER();
	//竞技场排名奖励--埋点日志
	vector<int> nParams;
	nParams.clear();
	nParams.push_back(AllInfoMsg.m_RankID);
	int nLogCode = LC_Helper::SerializeOperateLogCode(FUBEN_LOGTYPE, FUBEN11_OPERATE, OPERATE_LOGTYPE2);
	dule_Manager->GetSoloDuleManager(GameLogic::eSoloDuleType_Arena)->GetDailyReward(pkPlayer, AllInfoMsg.m_RankID, false, NULL, nLogCode,nParams);
}
//--------------------------------------------------
ON_HANDLE(MGPT_RANK_QUERY_BASEINFO_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	SendMsgToGroup(MGPT_SS_RANK_QUERY_BASEINFO_REQ, NULL, pkPlayer->GetUserID(), pkPlayer->GetInstance());
}
//--------------------------------------------------
ON_HANDLE(MGPT_SEND_DAILY_CLEAN_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	pkPlayer->SyncDailyCleanInfo();
}
//--------------------------------------------------
ON_HANDLE(MGPT_SS_GLOBAL_ARENA_ACHIEVE_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_PB<msg_define::arena_achievement_req> reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	SendMsgToGlobal(MGPT_SS_GLOBAL_ARENA_ACHIEVE_REQ, &reqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());
}
//--------------------------------------------------
ON_HANDLE(MGPT_SS_GLOBAL_ARENA_LADDER_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_PB<msg_define::arena_ladder_info_req> reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	SendMsgToGlobal(MGPT_SS_GLOBAL_ARENA_LADDER_REQ, &reqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());
}

//--------------
//新接口--请求竞技场奖励信息
ON_HANDLE(MGPT_GG_REQ_CHALLENGGIFTSTATE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_Req_ChallengGiftState reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	SendMsgToGroup(MGPT_SS_RANK_QUERY_GIFT_REQ, NULL, pkPlayer->GetUserID(), pkPlayer->GetInstance());
}

//对战记录
ON_HANDLE(MGPT_CS_BATTLE_LOG_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_Req_BattlefieldLog_List reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	SendMsgToGroup(MGPT_SS_BATTLE_LOG_REQ, &reqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());
}
//对战详情
ON_HANDLE(MGPT_CS_BATTLEFEILD_REPORT_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_Req_BattlefieldReport reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	SendMsgToGroup(MGPT_SS_BATTLEFEILD_REPORT_REQ, &reqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());
}

ON_HANDLE(MGPT_CS_GetSkyArena_GradeAward, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;
	MG_Req_SkyArenaGradeAward reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	MG_Resp_SkyArenaGradeAward resp;
	resp.m_nGrade = reqMsg.m_nGrade;
	resp.m_nResult = success;

	do 
	{
		CF_SkyArenaRank::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_SkyArenaRank>(resp.m_nGrade);
		if(pkData == NULL)
		{
			resp.m_nResult = RE_SKYARENA_NO_GRADE_AWARD;
			break ;
		}
		if(pkPlayer->GetSkyArenaGradeAwardState(reqMsg.m_nGrade))
		{
			resp.m_nResult = RE_SKYARENA_ALREADY_GET_GRADE_AWARD;
			break ;
		}

		if(pkPlayer->GetSkyArenaTopScore() < pkData->_iScoreNeed)
		{
			resp.m_nResult = RE_SKYARENA_GET_GRADEAWARD_LIMITSCORE;
			break ;
		}


		if( LC_Helper::CanAddItemEx( pkPlayer, pkData->_LevelRewards, CIET_NONE ) != RE_SUCCESS )
		{
			resp.m_nResult = RE_SKYARENA_PACK_LIMIT;
			break ;
		}
		SendMsgToGroup(MGPT_GG_ReqGetSkyArena_GradeAward, &reqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());
		return;
	} while (false);
	
	pkPlayer->SendMsgToClient(MGPT_SC_GetSkyArena_GradeAward, &resp);
}
}
