

#include "TS_TranSkyArena.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerPlayer.h"
#include "NW_Helper.h"
#include "LC_PackStruct.h"
#include "MG_CS_Auction.h"
#include "MG_Game2Group.h"
#include "GameServerApp.h"
#include "LC_SequenceManager.h"
#include "GlobalSettings.h"
#include "UT_OperateLog.h"
#include "MG_GroupProtocol.h"
#include "MG_Game2Group.h"

using namespace OPERATELOG;

using namespace Protocol;
using namespace Utility;
using namespace GameLogic;

AO_Server_ReqSkyArenaBattleInfo::AO_Server_ReqSkyArenaBattleInfo()
: m_uiCost(0)
{

}

emTranOperate AO_Server_ReqSkyArenaBattleInfo::Execute(Protocol::MG_BODY* pkMsg, uint32_t uiID)
{
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer( GetCharID() );
	if( !pkPlayer )
	{
		SetResultID(RE_FAIL);

		return TRAN_OP_FAILED;
	}
	
	MG_Req_SkyArenaBattlePlays* kReqMGBattle = static_cast<MG_Req_SkyArenaBattlePlays*>(pkMsg);
	if ( !kReqMGBattle )
	{
		SetResultID(RE_FAIL);
		return TRAN_OP_FAILED;
	}
	pkPlayer->CheckSkyRankNewSeanson();
	int nRefreshTimes = pkPlayer->GetSkyArenaRefreshTimes();
	if(kReqMGBattle->m_bRefresh && nRefreshTimes >= GetGlobalSetting.iSkyArenaDailyFreeRefreshTimes)
	{
		if (!pkPlayer->IsCashEnough(GetGlobalSetting.iSkyArenaRefreshCostCount, CT_UNBIND_YUANBAO))
		{
			SetResultID(RE_SKYARENA_REFRESH_COIN);
			return TRAN_OP_FAILED;
		}
		//int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, SELL_OPERATE, OPERATE_LOGTYPE1);
		pkPlayer->ReduceCash(GetGlobalSetting.iSkyArenaRefreshCostCount, CT_UNBIND_YUANBAO, SYS_FUNC_TP_ARENA_BEGIN, 0);
		SetContentFlag(true);
		m_uiCost = GetGlobalSetting.iSkyArenaRefreshCostCount;
	}
	
	MG_GG_Req_SkyArenaBattlePlays kReqGGBattle;

	kReqGGBattle.m_bRefresh = kReqMGBattle->m_bRefresh;
	kReqGGBattle.m_nSchemeType = kReqMGBattle->m_nSchemeType;
	UT_ServerHelper::GetSchemeLinkID(kReqMGBattle->m_nMapID, kReqGGBattle.m_nSchemeLinkID);
	SendMsgToGroup(MGPT_GG_ReqSkyArena_BattleInfo, &kReqGGBattle, pkPlayer->GetUserID(), pkPlayer->GetInstance(), uiID);
	SetResultID(RE_SUCCESS);
	return TRAN_OP_NEXT;

	
}

void AO_Server_ReqSkyArenaBattleInfo::RespondRltMsg()
{
	MG_Resp_SkyArenaBattlePlays kRltMsg;
	kRltMsg.m_Result = GetResultID();

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer( GetCharID() );
	if( pkPlayer )
	{
		pkPlayer->SendMsgToClient(MGPT_SC_GetSkyArena_BattleInfo, &kRltMsg);		
	}
}

void AO_Server_ReqSkyArenaBattleInfo::Rollback()
{
	if ( GetContentFlag() )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "苍穹竞技场刷新数据 UserID:%d, CharID: %lld, m_uiCost:%d",  GetUserID(), GetCharID(), m_uiCost);
		m_uiCost = 0;
	}
}

void AO_Server_ReqSkyArenaBattleInfo::Reset()
{
	TS_AtomOperate::Reset();
	m_kItemEntry.clear();
	m_uiCost = 0;
}

//////////////////////////////////////////////////////////////////////////

emTranOperate AO_Server_RltSkyArenaBattleInfo::Execute(Protocol::MG_BODY* pkMsg, uint32_t uiID) 
{ 
	MG_GG_Resp_SkyArenaBattlePlays* pkRltMsg = static_cast<MG_GG_Resp_SkyArenaBattlePlays*>(pkMsg);

	if (success == pkRltMsg->m_Result)
	{
		SetResultID(success);
		_respendSuccessMsg(pkRltMsg);
		return TRAN_OP_NEXT;
	}else
	{
		SetResultID(pkRltMsg->m_Result);
		return TRAN_OP_FAILED;
	}
}

void AO_Server_RltSkyArenaBattleInfo::RespondRltMsg() 
{
	MG_Resp_SkyArenaBattlePlays rltMsg;	
	rltMsg.m_Result = GetResultID();

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer( GetCharID() );
	if( pkPlayer )
	{
		pkPlayer->SendMsgToClient(MGPT_SC_GetSkyArena_BattleInfo, &rltMsg);	
	}
}

void AO_Server_RltSkyArenaBattleInfo::_respendSuccessMsg(Protocol::MG_BODY* pkMsg)
{
	MG_GG_Resp_SkyArenaBattlePlays* pkRltMsg = static_cast<MG_GG_Resp_SkyArenaBattlePlays*>(pkMsg);
	MG_Resp_SkyArenaBattlePlays rltMsg;	
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer( GetCharID() );
	if(NULL == pkPlayer)
	{
		return;
	}
	
	rltMsg.m_Result = pkRltMsg->m_Result;
	if(true == pkRltMsg->m_bRefresh)
	{
		pkPlayer->AddSkyArenaRefreshTimes(1);
		
	}
	rltMsg.m_nMyScore = pkRltMsg->m_nMyScore;
	rltMsg.m_nChangeScore = pkRltMsg->m_nChangeScore;
	rltMsg.m_battlePlays = pkRltMsg->m_battlePlays;
	rltMsg.m_nIsNewSeason = pkRltMsg->m_nIsNewSeason;
	rltMsg.m_nRefreshTimes = pkPlayer->GetSkyArenaRefreshTimes();
	rltMsg.m_nBattleTimes = pkPlayer->GetSkyArenaChallengeTimes();
	rltMsg.m_nSchemeID = pkRltMsg->m_nSchemeID;
	rltMsg.m_nSchemeType = pkRltMsg->m_nSchemeType;
	rltMsg.n_HonorValue = pkPlayer->GetSkyArenaHonorValue();
	pkPlayer->SetSkyArenaScoreValue(rltMsg.m_nMyScore);

	pkPlayer->SendMsgToClient(MGPT_SC_GetSkyArena_BattleInfo, &rltMsg);	
	
}

//////////////////////////////////////////////////////////////////////////
TS_Server_SkyArenaBattleInfo::TS_Server_SkyArenaBattleInfo()
{
	m_kStepTranMap[STEP_REQ_GG_SkyArenaBattleInfo] = T_NEW_D AO_Server_ReqSkyArenaBattleInfo;
	m_kStepTranMap[STEP_RLT_GG_SkyArenaBattleInfo] = T_NEW_D AO_Server_RltSkyArenaBattleInfo;
}

TS_Server_SkyArenaBattleInfo::~TS_Server_SkyArenaBattleInfo()
{
	for (StepAtomMapIter it = m_kStepTranMap.begin(); it != m_kStepTranMap.end(); ++it)
	{
		T_SAFE_DELETE( it->second);
	}

	m_kStepTranMap.clear();
}