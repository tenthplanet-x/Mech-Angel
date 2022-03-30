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

#include "LC_Dule_Manager.h"
#include "LC_RankManager.h"
#include "LC_PackEntryFactory.h"
#include "MG_GroupProtocol.h"
#include "MG_Common.h"
#include "UT_OperateLog.h"
#include "GlobalSettings.h"
#include "LC_ServerMapLogic_SummitTeamPlay.h"
#include "LC_ServerMapLogic_SkyPlay.h"
using namespace OPERATELOG;
using namespace Protocol;
using namespace GameLogic;
using namespace Utility;

namespace Protocol
{
#define this_source_type SERVER_TYPE_GROUP

ON_HANDLE(MGPT_SS_RANK_QUERY_BASEINFO_RESP, this_source_type, false)
{
	MG_GG_RESP_Query_User_Base_Rank rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_RankManager* rank_Manager = SERVER_GET_RANK_MANAGER();
	rank_Manager->QueryBaseInfoResponse(rkMsgHead.m_uiCharID, rltMsg);

	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (pkPlayer == NULL)
	{
		return;
	}
	pkPlayer->SendMsgToClient(MGPT_RANK_QUERY_BASEINFO_RESP, &rltMsg);
}

ON_HANDLE(MGPT_SS_RANK_QUERY_GAINGIFT_RESP, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(SERVER_GET_PLAYER_MANAGER()->FindPlayer(rkMsgHead.m_uiCharID));
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_GG_RTL_Gain_Rank_Gift gainMsg;
	if (!gainMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	//竞技场排名奖励--埋点日志
	vector<int> nParams;
	nParams.clear();
	nParams.push_back(gainMsg.m_RankIndex+1);
	int nLogCode = LC_Helper::SerializeOperateLogCode(FUBEN_LOGTYPE, FUBEN11_OPERATE, OPERATE_LOGTYPE2);
	LC_Dule_Manager* dule_Manager = SERVER_GET_DULE_MANAGER();
	dule_Manager->GetSoloDuleManager(GameLogic::eSoloDuleType_Arena)->GetDailyReward(pkPlayer, gainMsg.m_RankID, true, &gainMsg, nLogCode, nParams);

	//请求数据
	//if (gainMsg.m_RankGain == true)
	//{
		//SendMsgToGroup(MGPT_SS_RANK_QUERY_BASEINFO_REQ, NULL, pkPlayer->GetUserID(), pkPlayer->GetInstance());
	//}
}

ON_HANDLE(MGPT_SS_RANK_QUERY_RANK_RESP, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = NULL;
	if (rkMsgHead.m_uiCharID != INVALID_ACTOR_ID)
	{
		LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
		pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	}

	if(NULL == pkPlayer)
	{
		return;
	}

	MG_GG_RESP_Query_Rank rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer->SendMsgToClient(MGPT_RESP_RANK_INFO, &rltMsg);
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, " MGPT_RESP_RANK_INFO  c_uid:%d\n", pkPlayer->GetUserID());
}



ON_HANDLE(MGPT_SS_QUERY_CHALLENG_RANK_RESP, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = NULL;
	if (rkMsgHead.m_uiCharID != INVALID_ACTOR_ID)
	{
		LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
		pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	}

	if(NULL == pkPlayer)
	{
		return;
	}

	MG_GG_RESP_QueryChalleng_Rank rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer->SendMsgToClient(MGPT_CHALLENG_RANK_RESP, &rltMsg);
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, " MGPT_CHALLENG_RANK_RESP  c_uid:%d\n", pkPlayer->GetUserID());
}

ON_HANDLE(MGPT_GG_RESP_GETRANK_PLAYERBUDDY_INFO, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = NULL;
	if (rkMsgHead.m_uiCharID != INVALID_ACTOR_ID)
	{
		LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
		pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	}

	if(NULL == pkPlayer)
	{
		return;
	}

	MG_GG_RESP_GetRankPlayerBuddyInfo rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer->SendMsgToClient(MGPT_RESP_GETRANK_PLAYERBUDDY_INFO, &rltMsg);
}

ON_HANDLE(MGPT_GG_RESP_GETRANK_BUDDY_INFO, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = NULL;
	if (rkMsgHead.m_uiCharID != INVALID_ACTOR_ID)
	{
		LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
		pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	}

	if(NULL == pkPlayer)
	{
		return;
	}

	MG_GG_RESP_GetRankBuddyInfo rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer->SendMsgToClient(MGPT_RESP_GETRANK_BUDDY_INFO, &rltMsg);
}

ON_HANDLE(MGPT_GG_RESP_GETRANK_PET_INFO, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = NULL;
	if (rkMsgHead.m_uiCharID != INVALID_ACTOR_ID)
	{
		LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
		pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	}

	if(NULL == pkPlayer)
	{
		return;
	}

	MG_GG_RESP_GetRankPetInfo rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer->SendMsgToClient(MGPT_RESP_GETRANK_PET_INFO, &rltMsg);
}

ON_HANDLE(MGPT_GG_RESP_GETRANK_SUB_INFO, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = NULL;
	if (rkMsgHead.m_uiCharID != INVALID_ACTOR_ID)
	{
		LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
		pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	}

	if(NULL == pkPlayer)
	{
		return;
	}

	MG_GG_RESP_GetRankSubInfo rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer->SendMsgToClient(MGPT_RESP_GETRANK_SUB_INFO, &rltMsg);
}

ON_HANDLE(MGPT_GG_RESP_RANK_TOPN_PLAYER_INFO, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = NULL;
	if (rkMsgHead.m_uiCharID != INVALID_ACTOR_ID)
	{
		LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
		pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	}

	if(NULL == pkPlayer)
	{
		return;
	}

	MG_GG_RESP_RankTopNPlayerInfo rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer->SendMsgToClient(MGPT_RESP_RANK_TOPN_PLAYER_INFO, &rltMsg);
}

ON_HANDLE(MGPT_GG_RESP_GETBUDDYRANKINDEX, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = NULL;
	if (rkMsgHead.m_uiCharID != INVALID_ACTOR_ID)
	{
		LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
		pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	}

	if(NULL == pkPlayer)
	{
		return;
	}

	MG_RESP_GetBuddyRankIndex rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer->SendMsgToClient(MGPT_RESP_GETBUDDYRANKINDEX, &rltMsg);
}

ON_HANDLE(MGPT_SS_RANK_QUERY_USER_CHALLENGE_RANK_RESP, this_source_type, false)
{
	MG_GG_RESP_Query_User_Challenge rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_RankManager* rank_Manager = SERVER_GET_RANK_MANAGER();
	rank_Manager->QueryUserChallengeResponse(rkMsgHead.m_uiCharID, rltMsg);
}

ON_HANDLE(MGPT_SS_RANK_QUERY_BATTLE_RESP, this_source_type, false)
{
	MG_GG_RESP_Query_User_Battle rltMsg;

	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_RankManager* rank_Manager = SERVER_GET_RANK_MANAGER();
	rank_Manager->QueryBattleResponse(rkMsgHead.m_uiCharID, rltMsg);
}

ON_HANDLE(MGPT_SS_RESP_GET_EXPORTROBOTFLAG, this_source_type, false)
{
	MG_GG_RESP_GetExportRobotFlag rltMsg;
	if(!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	UT_ServerHelper::SetExportRobotFlag(rltMsg.m_nRobotType, rltMsg.m_bFlag);
}

ON_HANDLE(MGPT_SS_RANK_CHALLENGE_RESP, this_source_type, false)
{
	MG_GG_RESP_Challenge rltMsg;

	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_RankManager* rank_Manager = SERVER_GET_RANK_MANAGER();
	rank_Manager->ChallengeResponse(rkMsgHead.m_uiCharID, rltMsg);
}
ON_HANDLE( MGPT_SS_RESP_SUMMITARENA_CHECKCHALLENGE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(SERVER_GET_PLAYER_MANAGER()->FindPlayer(rkMsgHead.m_uiCharID));
	if (NULL == pkPlayer)
		return;
	MG_Resp_G2G_SummitArenaCheckChallenge rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
		return;
	if(rltMsg.m_eType == Arena_Type_Firmament)
	{
		pkPlayer->AddSkyArenaChallengeTimes(1);
		uint64_t nInstanceID = pkPlayer->GetInstance();
		SERVER_GET_RANK_MANAGER()->ChallengeResponse_Sky(nInstanceID, rltMsg.m_nChallengeId, RE_SUCCESS,rltMsg.m_uScore );
	}
	else if (rltMsg.m_eType == Arena_Type_Peak_Group)
	{
		SERVER_GET_RANK_MANAGER()->SummitTeamChallengeResponse(rkMsgHead.m_uiCharID, rltMsg);
	}
	
}
ON_HANDLE(MGPT_SS_RANK_CHALLENGE_RESULT_RESP, this_source_type, false)
{
	MG_GG_RESP_Challenge_Result rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_Dule_Manager* pkDuleManager = SERVER_GET_DULE_MANAGER();
	pkDuleManager->GetSoloDuleManager(GameLogic::eSoloDuleType_Arena)->SetChallengeResultResponse(rkMsgHead.m_uiCharID, rltMsg);
}

ON_HANDLE(MGPT_SS_RANK_NOTIFY_BECHALLENGE, this_source_type, false)
{
	MG_GG_Rank_Notify_beChallenge rltMsg;

	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_RankManager* rank_Manager = SERVER_GET_RANK_MANAGER();
	rank_Manager->BeChallengeNotify(rkMsgHead.m_uiCharID, rltMsg,  GameLogic::eSoloDuleType_Arena);
}


ON_HANDLE(MGPT_SS_RANK_DAILY_SETTLE_NOTIFY, this_source_type, false)
{
}

ON_HANDLE(MGPT_SS_RANK_DAILY_SETTLE_NOTIFY_ONCE, this_source_type, false)
{
	MG_GG_Daily_Settle_Notify_Once rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	pkWorldManager->BroadCast(MGPT_RANK_RANK_DAILY_SETTLE_NOTIFY, &rltMsg, true);
}
//---------
ON_HANDLE(MGPT_SS_RANK_QUERY_GIFT_RESP, this_source_type, false)
{
	MG_SS_ChallengGiftState rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (pkPlayer == NULL)
	{
		return;
	}

	pkPlayer->SendMsgToClient(MGPT_GG_RESP_CHALLENGGIFTSTATE, &rltMsg);
}

ON_HANDLE(MGPT_SS_UPDATERANKLIST, this_source_type, false)
{
	MG_SS_UpdateRankListToGS rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	UT_ServerHelper::UpdateRankList(rltMsg.m_RankType, rltMsg.m_CharID);
}



//Group返回对战记录
ON_HANDLE(MGPT_SS_BATTLE_LOG_RESP, this_source_type, false)
{
	MG_Resp_BattlefieldLog_List rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (pkPlayer == NULL)
	{
		return;
	}

	pkPlayer->SendMsgToClient(MGPT_SC_BATTLE_LOG_RESP, &rltMsg);
}
//Group返回对战详情
ON_HANDLE(MGPT_SS_BATTLEFEILD_REPORT_RESP, this_source_type, false)
{
	MG_Resp_BattlefieldReport rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (pkPlayer == NULL)
	{
		return;
	}

	pkPlayer->SendMsgToClient(MGPT_SC_BATTLEFEILD_REPORT_RESP, &rltMsg);
}


//---------
ON_HANDLE(MGPT_SS_SKYARENA_AWARD_NOTIFY, this_source_type, false)
{
	MG_GG_NOTIFY_SkyArenaAward rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	if(rltMsg.m_UserIdLists.size() <=0 )
	{
		return;
	}
	for(int i = 0; i < rltMsg.m_UserIdLists.size(); i++)
	{
		UT_ServerHelper::SendSkyArenaReward(rltMsg.m_UserIdLists[i].n_CharId, rltMsg.m_UserIdLists[i].nType, rltMsg.m_UserIdLists[i].n_Index, rltMsg.m_UserIdLists[i].nScore, rltMsg.m_UserIdLists[i].nGradeId);
	}	
}

ON_HANDLE(MGPT_SS_RESP_EXPORTROBOT_NEW, this_source_type, false)
{
	MG_GG_RESP_ExportRobot_New kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	if(RE_SUCCESS == kMsg.m_Result)
	{
		if(kMsg.m_nExportType == ERT_INIT)
		{
			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "导入苍穹战场机器人成功:%d", kMsg.m_IndexBegin);
		}
	}

	/*
	LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)pkServerPlayerManager->FindPlayer(kMsg.m_uOpId);
	if (pkServerPlayer == NULL)
	{
		return;
	}

	StringStreamType sStream("");
	if(kMsg.m_nType == 0)
	{
		if(kMsg.m_Result == RE_SUCCESS)
		{
			int num = kMsg.m_IndexBegin ;
			sStream<<"ExportRobot! num:"<<num;
		}
		else
		{
			int errorNum = kMsg.m_IndexBegin;
			sStream<<"ExportRobot fail! errcode:"<< errorNum;
		}
	}
	else if(kMsg.m_nType == 1)
	{
		sStream<<"Refresh ExportRobot success!";
	}
	else if(kMsg.m_nType == 2)
	{
		sStream<<" ClearRobot success! ";
	}

	pkServerPlayer->SendNotifyStrToClient(sStream.str());
	*/
}

ON_HANDLE(MGPT_GG_RespGetSkyArena_GradeAward, this_source_type, false)
{
	MG_GG_RESP_SkyArenaGradeAward rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (pkPlayer == NULL)
	{
		return;
	}
	MG_Resp_SkyArenaGradeAward resp;
	resp.m_nResult = rltMsg.m_nResult;
	resp.m_nGrade = rltMsg.m_nGrade;
	resp.m_nGradeBits = rltMsg.m_nGradeBits;
	resp.m_nMyScore = rltMsg.m_nScore;
	resp.m_nMyTopScore = rltMsg.m_nMyTopScore;
	if(resp.m_nResult == success)
	{
		rltMsg.m_nResult = pkPlayer->GetSkyArenaGrade(rltMsg.m_nGrade, rltMsg.m_nMyTopScore, resp.m_nGradeBits);
	}
	
	pkPlayer->SendMsgToClient(MGPT_SC_GetSkyArena_GradeAward, &resp);

}


ON_HANDLE(MGPT_GG_RespSkyArena_BattleInfo, this_source_type, false)
{
	MG_GG_Resp_SkyArenaBattlePlays kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	TS_TranManager* pkTranManager = TS_TranManager::GetSingletonPtr();
	if (!pkTranManager)
	{
		return;
	}

	TS_TranBase* pkTran = pkTranManager->GetTran(rkMsgHead.m_iSequence);
	if (!pkTran)
	{
		//GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "");
		return;
	}

	if (pkTran->GetStep() != TS_Server_SkyArenaBattleInfo::STEP_RLT_GG_SkyArenaBattleInfo)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "刷新逻辑!");
	}
	else
	{
		pkTran->HandlerMsg(rkMsgHead, &kMsg);
	}
	pkTranManager->UnRegisteTran(pkTran->GetID());
}


ON_HANDLE(MGPT_GG_NOTIFY_ArenaTimeData, this_source_type, false)
{
	MG_GG_NOTIFY_SkyArenaTimeData rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_NOTIFY_SkyArenaTimeData resp;
	resp.m_timeData = rltMsg.m_timeData;
	LC_RankManager* rank_Manager = SERVER_GET_RANK_MANAGER();
	rank_Manager->SetRankOpenTime(rltMsg.m_timeData);
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	pkWorldManager->BroadCast(MGPT_SC_NOTIFY_ArenaTimeData, &resp, true);

}


ON_HANDLE(MGPT_SS_NOTIFY_SUMMIT_ARENA_TIME, this_source_type, false)
{
	MG_NOTIFY_SummitArenaTimeData rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_RankManager* rank_Manager = SERVER_GET_RANK_MANAGER();
	rank_Manager->SetSummitArenaOpenTime(rltMsg.m_timeData);
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	pkWorldManager->BroadCast(MGPT_NOTIFY_SUMMIT_ARENA_TIME, &rltMsg, true);

}

ON_HANDLE(MGPT_SS_NOTIFY_SUMMIT_ARENA_REDPOINT, this_source_type, false)
{
	MG_NOTIFY_SummitArenaRedPoint rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (pkPlayer == NULL)
	{
		return;
	}

	pkPlayer->SendMsgToClient(MGPT_NOTIFY_SUMMIT_ARENA_REDPOINT, &rltMsg);

}

ON_HANDLE(MGPT_RESP_G2G_GETPLAYERSCHEMEDATA, this_source_type, false)
{
	MG_RESP_GetPlayerSchemeData rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	
	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (pkPlayer == NULL)
	{
		return;
	}

	pkPlayer->SendMsgToClient(MGPT_RESP_GETPLAYERSCHEMEDATA, &rltMsg);
}

ON_HANDLE(MGPT_RESP_G2G_GETRANKTYPEINFO, this_source_type, false)
{
	MG_RESP_GetRankTypeInfo rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (pkPlayer == NULL)
	{
		return;
	}

	pkPlayer->SendMsgToClient(MGPT_RESP_GETRANKTYPEINFO, &rltMsg);
}

ON_HANDLE(MGPT_GG_RespSkyArena_GradeAwardInfo, this_source_type, false)
{
	MG_Resp_SkyArenaMyScore rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (pkPlayer == NULL)
	{
		return;
	}
	pkPlayer->SetSkyArenaGradeAwardState(rltMsg.m_nGradeBits);
	pkPlayer->SetSkyArenaTopScore(rltMsg.m_nMyTopScore);

	pkPlayer->SendMsgToClient(MGPT_SC_GetSkyArena_GradeAwardInfo, &rltMsg);
}

ON_HANDLE(MGPT_GG_SkyArena_ChallengeResult, this_source_type, false)
{
	MG_GG_SkyArena_Challenge_Result rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	if(rltMsg.m_iResult != success)
	{
		return;
	}
	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (pkPlayer == NULL)
	{
		return;
	}
	MG_CS_SkyArena_Challenge_Result resp;
	pkPlayer->SetSkyArenaScoreValue(rltMsg.m_Score);
	pkPlayer->SkyArenaChallengeResult(rltMsg.m_sucess, rltMsg.m_GradeId, resp.m_soloAward,resp.m_nSkyHonorErrorCode);
	
	resp.m_iResult = rltMsg.m_iResult;
	resp.m_Score = rltMsg.m_Score;
	resp.m_SoloType = rltMsg.m_SoloType;
	resp.m_changeScore = rltMsg.m_changeScore;
	resp.m_leftWinNum = rltMsg.m_leftWinNum;
	resp.m_rightWinNum = rltMsg.m_rightWinNum;
	resp.m_sucess = rltMsg.m_sucess;
	resp.m_RankIndex = rltMsg.m_RankIndex;
	resp.m_ChangeIndex = rltMsg.m_ChangeIndex;
	for(uint32_t i = 0; i < rltMsg.m_ReportList.size(); ++i)
		resp.m_ReportList.push_back(rltMsg.m_ReportList[i]);
	pkPlayer->SendMsgToClient(MGPT_SC_SkyArena_ChallengeResult, &resp);
	LC_ServerMapLogic_Place* logicbase = UT_ServerHelper::GetPlayerPlace(pkPlayer);
	if (logicbase == NULL)
		return;
	if(resp.m_SoloType == Arena_Type_Firmament)
	{
		LC_ServerMapLogic_SkyPlay* play = (LC_ServerMapLogic_SkyPlay*)logicbase->GetPlay(PlayType_Sky);
		if (play == NULL)
			return;
		play->End();
		//play->SetSkyNextState(Sky_State_End);
	}
	else if(resp.m_SoloType == Arena_Type_Peak_Group)
	{
		LC_ServerMapLogic_SummitTeamPlay* play = (LC_ServerMapLogic_SummitTeamPlay*)logicbase->GetPlay(PlayType_SummitTeam);
		if (play == NULL)
			return;
		//play->SetSummitTeamNextState(SummitTeam_State_End);
		play->End();
	}

}

ON_HANDLE(MGPT_RESP_G2G_GETSUMMITSINGLEDATA, this_source_type, false)
{
	MG_RESP_G2G_GetSummitSingleData rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	
	std::stringstream logStr;
	logStr << "MG_RESP_G2G_GetSummitSingleData summitType:[" << rltMsg.m_nBattleInfo.m_nGuessInfo.m_nSummitType << "],  res:" << rltMsg.m_nResult;
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "%s", logStr.str().c_str());

	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (pkPlayer == NULL)
	{
		return;
	}

	pkPlayer->SendMsgToClient(MGPT_RESP_GETSUMMITSINGLEDATA, &rltMsg);
	if(RE_SUCCESS == rltMsg.m_nResult)
	{
		pkPlayer->GetSummitGuessAsset().UpdateSingleSummitData(rltMsg.m_nBattleInfo.m_nGuessInfo);
	}
}

ON_HANDLE(MGPT_RESP_G2G_GETSUMMITSUPPORTINFO, this_source_type, false)
{
	MG_RESP_G2G_GetSummitSupportInfo rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (pkPlayer == NULL)
	{
		return;
	}

	pkPlayer->SendMsgToClient(MGPT_RESP_GETSUMMITSUPPORTINFO, &rltMsg);
}

ON_HANDLE(MGPT_RESP_G2G_SETSUMMITGUESSINFO, this_source_type, false)
{
	MG_RESP_G2G_SetSummitGuessInfo rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (pkPlayer == NULL)
	{
		return;
	}

	MG_RESP_SetSummitGuessInfo respMsg;
	respMsg.m_nResult = rltMsg.m_nResult;
	respMsg.m_nAgainstID = rltMsg.m_nAgainstID;	
	respMsg.m_nChoseCharID = rltMsg.m_nChoseCharID;
	respMsg.m_nSummitType = rltMsg.m_nSummitType;
	respMsg.m_nGuessSupportInfo = rltMsg.m_nGuessSupportInfo;
	pkPlayer->SendMsgToClient(MGPT_RESP_SETSUMMITGUESSINFO, &respMsg);
	if(RE_SUCCESS == rltMsg.m_nResult)
	{
		pkPlayer->GetSummitGuessAsset().UpdateChoseCharID(rltMsg.m_nSummitType, rltMsg.m_nChoseCharID);
	}
	else if(rltMsg.m_nLastChoseCharID == 0)
	{
		pkPlayer->GetSummitGuessAsset().RevertGuessComsume(pkPlayer, GuessType_SummitArena);
	}
}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    

ON_HANDLE(MGPT_RESP_G2G_GETSUMMITWINER, this_source_type, false)
{
	MG_RESP_G2G_GetSummitWiner rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (pkPlayer == NULL)
	{
		return;
	}

	pkPlayer->SendMsgToClient(MGPT_RESP_GETSUMMITWINER, &rltMsg);
	if(RE_SUCCESS == rltMsg.m_nResult)
	{
		pkPlayer->GetSummitGuessAsset().UpdateSingleSummitData(rltMsg.m_nGuessInfo);
	}
}

ON_HANDLE(MGPT_NOTIFY_GG_SUMMITDATA, this_source_type, false)
{
	MG_Notify_GG_SummitData msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (pkPlayer == NULL)
	{
		return;
	}

	pkPlayer->GetSummitGuessAsset().SyncSummitGuessData(pkPlayer, msg);
}

ON_HANDLE(MGPT_NOTIFY_GG_SENDGUESSMAIL, this_source_type, false)
{
	MG_Notify_GG_SendGuessMail msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	UT_ServerHelper::SendGuessMailToPlayer(msg.m_nGuessType, msg.m_bSuccess, msg.m_nName, msg.m_nSummitType, msg.m_nAgainstID, msg.m_nSupportPlayerIDVec);
}

ON_HANDLE(MGPT_SS_SUMMIT_ARENA_INFO_RESP, this_source_type, false)
{
	MG_Resp_SummitArenaInfo rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (pkPlayer == NULL)
	{
		return;
	}

	pkPlayer->SendMsgToClient(MGPT_SC_SUMMIT_ARENA_INFO_RESP, &rltMsg);
}



ON_HANDLE(MGPT_SS_SUMMIT_ARENA_AGAINSTLIST_RESP, this_source_type, false)
{
	MG_Resp_SummitArenaAgainstList rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (pkPlayer == NULL)
	{
		return;
	}

	pkPlayer->SendMsgToClient(MGPT_SC_SUMMIT_ARENA_AGAINSTLIST_RESP, &rltMsg);
}



ON_HANDLE(MGPT_SS_SUMMIT_ARENA_MY_INFO_RESP, this_source_type, false)
{
	MG_Resp_SummitArenaMyGroupStage rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (pkPlayer == NULL)
	{
		return;
	}

	pkPlayer->SendMsgToClient(MGPT_SC_SUMMIT_ARENA_MY_INFO_RESP, &rltMsg);

}



//---------
ON_HANDLE(MGPT_SS_NOTIFY_SUMMIT_ARENA_BRISEUP, this_source_type, false)
{
	MG_Notify_GG_SummitArenaMailInfo rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	UT_ServerHelper::SendSummitArenaRiseUpMail(rltMsg.m_nCharID,rltMsg.nRankIndex,rltMsg.nStageType);
}


//---------
ON_HANDLE(MGPT_SS_NOTIFY_SUMMIT_ARENA_RANKINDEX, this_source_type, false)
{
	MG_Notify_GG_SummitArenaMailInfo rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	if(rltMsg.nStageType <= 0)
	{
		return;
	}
	UT_ServerHelper::SendSummitArenaRankAwardMail(rltMsg.m_nCharID, rltMsg.nStageType, rltMsg.nRankIndex);
	
}



//---------
ON_HANDLE(MGPT_SS_NOTIFY_SUMMITARENA_GROUPSTAGE_MAIL, this_source_type, false)
{
	MG_Notify_GG_SummitArenaMailInfo rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	UT_ServerHelper::SendSummitArenaGroupStageMail(rltMsg.m_nCharID);
}


ON_HANDLE(MGPT_SS_SUMMIT_ARENA_ENTRYLIST_RESP, this_source_type, false)
{
	MG_Resp_SummitArenaGroupStageEntryList rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (pkPlayer == NULL)
	{
		return;
	}

	pkPlayer->SendMsgToClient(MGPT_SC_SUMMIT_ARENA_ENTRYLIST_RESP, &rltMsg);
}


ON_HANDLE(MGPT_SS_SUMMIT_ARENA_GROUPSTAGE_RESP, this_source_type, false)
{
	MG_Resp_SummitArenaGroupStage rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (pkPlayer == NULL)
	{
		return;
	}

	pkPlayer->SendMsgToClient(MGPT_SC_SUMMIT_ARENA_GROUPSTAGE_RESP, &rltMsg);
}


ON_HANDLE(MGPT_SS_SUMMIT_ARENA_HONOR_RESP, this_source_type, false)
{
	MG_Resp_SummitArenaHonorMsg rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (pkPlayer == NULL)
	{
		return;
	}

	pkPlayer->SendMsgToClient(MGPT_SC_SUMMIT_ARENA_HONOR_RESP, &rltMsg);
}

ON_HANDLE(MGPT_SS_NOTIFY_SUMMIT_ARENA_HONORAWARD, this_source_type, false)
{
	MG_Notify_GG_SummitArenaMailInfo rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	if(rltMsg.nStageType <= 0)
	{
		return;
	}
	UT_ServerHelper::SendSummitArenaGloryRankRewardMail(rltMsg.m_nCharID, rltMsg.nStageType, rltMsg.nRankIndex);
}


ON_HANDLE(MGPT_SS_SUMMITARENA_MY_HONORTOPID_RESP, this_source_type, false)
{
	MG_Resp_SummitArenaMyTopHonorMsg rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (pkPlayer == NULL)
	{
		return;
	}

	pkPlayer->SendMsgToClient(MGPT_SC_SUMMITARENA_MY_HONORTOPID_RESP, &rltMsg);
}



ON_HANDLE(MGPT_SS_NWEARENA_SYNCINTEGRAL, this_source_type, false)
{
	MG_SYNC_NewArenaPointMsg rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (pkPlayer == NULL)
	{
		return;
	}

	pkPlayer->SetSkyArenaScoreValue(rltMsg.nSyncPoint);
	pkPlayer->SetSkyArenaMaxScoreValue(rltMsg.nGetMaxPoint);
}


ON_HANDLE(MGPT_SS_NOTIFY_SUMMITARENA_MY_TOPSTAGEID, this_source_type, false)
{
	MG_Resp_SummitArenaMyTopHonorMsg rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (pkPlayer == NULL)
	{
		return;
	}

	pkPlayer->SendMsgToClient(MGPT_SC_NOTIFY_SUMMITARENA_MY_TOPSTAGEID, &rltMsg);
}


ON_HANDLE(MGPT_SS_SKYARENA_CHECKBATTLESTAGE_RESP, this_source_type, false)
{
	MG_Resp_SkyArenaCheckBattleStage rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (pkPlayer == NULL)
	{
		return;
	}

	pkPlayer->SendMsgToClient(MGPT_SC_SKYARENA_CHECKBATTLESTAGE_RESP, &rltMsg);
}

ON_HANDLE(MGPT_SS_NOTIFY_SUMMITARENA_AUTOSOLO, this_source_type, false)
{
	MG_Notify_G2G_SummitArenaAutoSolo rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	if(rltMsg.m_AutoSoloList.size() <= 0)
	{
		return;
	}
	for(int i = 0; i < rltMsg.m_AutoSoloList.size(); i++)
	{
		NotifySummitArenaAutoSoloData& info = rltMsg.m_AutoSoloList[i];
		if(info.m_nSoloCharIdList.size() < 2)
		{
			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, " ------------  MGPT_SS_NOTIFY_SUMMITARENA_AUTOSOLO challengeList Error ----- m_uniqueID:%lld\n", info.m_nAgainstId);
			continue;
		}
		uint64_t uChallengeId = info.m_nSoloCharIdList[0];
		uint64_t uBeChallengeId = info.m_nSoloCharIdList[1];
		if(!IS_SOLODULE_ROBOT_PLAYERID(info.m_nSoloCharIdList[0]))
		{
			uChallengeId = info.m_nSoloCharIdList[1];
			uBeChallengeId = info.m_nSoloCharIdList[0];

		}
		UT_ServerHelper::WheelFightReq(info.m_nAgainstId, uChallengeId, uBeChallengeId);
	}
	
}


ON_HANDLE(MGPT_SS_NOTIFY_SUMMITARENA_GUESSCLOSE, this_source_type, false)
{
	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (pkPlayer == NULL)
	{
		return;
	}

	pkPlayer->SendMsgToClient(MGPT_NOTIFY_SUMMITARENA_GUESSCLOSE, NULL);
}

ON_HANDLE(MGPT_SS_NOTIFY_SUMMITARENA_HONORTIMES, this_source_type, false)
{
	MG_SYNC_SummitArenaHonorTimes rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (pkPlayer == NULL)
	{
		return;
	}

	GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "SetSummitArenaHonorTimes");
	pkPlayer->SetSummitArenaHonorTimes( rltMsg.m_HonorMap);
}

ON_HANDLE(MGPT_SS_NOTIFY_ACTIVATE_SUMMITARENA, this_source_type, false)
{
	MG_NOTIFY_ActivateSummitArena rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (pkPlayer == NULL)
	{
		return;
	}
	pkPlayer->SetActivateSummitArena(rltMsg.m_Vaild);
}



ON_HANDLE(MGPT_SC_NOTIFY_GROUPARENA_NOTICE, this_source_type, false)
{
	MG_GG_Notify_GroupArena_Notice rltMsg;

	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	UT_ServerHelper::_GameNoticeFmort noticeFmort;
	LC_ServerPlayer* pkPlayer = NULL;
	if(rltMsg.m_nNoticeId == GroupArena_Notice_SummitArenChampion)
	{
		noticeFmort.SetMsgID( GetGlobalSetting.iSummitArenaNoticeId );
		StringType sValueStr = rltMsg.m_sValueStr.c_str();
		noticeFmort.AddString(sValueStr);
	}
	else if(rltMsg.m_nNoticeId == GroupArena_Notice_SkyArenGrade)
	{
		if(rltMsg.m_nValue < GetGlobalSetting.iSkyArenaNoticeLimit)
		{
			return;
		}
		LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
		pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
		if (pkPlayer == NULL)
		{
			return;
		}
		noticeFmort.SetMsgID( GetGlobalSetting.iSkyArenaNoticeId);
		noticeFmort.AddSkyGrade(rltMsg.m_nValue);
	}
	else if(rltMsg.m_nNoticeId == GroupArena_Notice_SkyArenStart)
	{
		noticeFmort.SetMsgID( GetGlobalSetting.iSkyArenaStartNoticeId);
	}
	UT_ServerHelper::BrocastNoticeMsg(noticeFmort, 0, pkPlayer);
}
}
