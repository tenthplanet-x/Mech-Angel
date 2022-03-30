#include "ClientPCH.h"
#include "LC_ClientArenaManager.h"
#include "LC_ClientLogicManager.h"
#include "LC_ClientGamePlayer.h"

#include "NW_ClientSessionManager.h"
#include "NW_Helper.h"

#include "SM_GameStageMachine.h"

#include "UT_LogicInfoMessage.h"
#include "UI_UIManager.h"
#include "LC_GameEvent.h"
#include "SystemError.h"

#include "CF_RankRewardGrade.h"

//#include "CF_ArenaModeConfig.h"
//#include "CF_ArenaLevelUp.h"

using namespace Protocol;
using namespace GameLogic;
using namespace StateMachine;

//--------------------------------------------------------------------
LC_ClientArenaPlayerDataEntry::LC_ClientArenaPlayerDataEntry()
{
	Reset();
}
//--------------------------------------------------------------------
LC_ClientArenaPlayerDataEntry::~LC_ClientArenaPlayerDataEntry()
{

}
//--------------------------------------------------------------------
void LC_ClientArenaPlayerDataEntry::GetDataFromProtocolStruct(Protocol::PS_ArenaPlayerStateData& rkStruct)
{
	Reset();
	SetValid(true);
	SetID(rkStruct.m_iPlayerID);
	SetPlayerInArenaState(rkStruct.m_iPlayerArenaState);
	SetPlayerName(STDSTR2TPSTR(rkStruct.m_szPlayerName));
	SetKillNum(rkStruct.m_iPlayerKillNum);
	SetDeadNum(rkStruct.m_iPlayerDeadNum);
}
//--------------------------------------------------------------------
void LC_ClientArenaPlayerDataEntry::GetDataFromProtocolStruct(Protocol::PS_ArenaPlayerResultData& rkStruct)
{
	Reset();
	SetValid(true);
	SetID(rkStruct.m_iPlayerID);
	SetPlayerInArenaState(LPIAS_IN_ARENA);
	SetPlayerName(STDSTR2TPSTR(rkStruct.m_szPlayerName));
	SetKillNum(rkStruct.m_iPlayerKillNum);
	SetDeadNum(rkStruct.m_iPlayerDeadNum);
	SetGainScore(rkStruct.m_iPlayerGainScore);
	SetGainExp(rkStruct.m_iPlayerGainExp);
}
//--------------------------------------------------------------------
LC_ClientArenaManager::LC_ClientArenaManager()
{
	m_eArenaState					= LAS_WAIT_USER_ENTER;
	m_eArenaStage					= LPAS_NONE;
	m_eArenaType					= LAT_MAX_COUNT;
	m_iArenaSignUpUserCount			= 0;
	m_fStartWaitEnterArenaTime		= 0.0f;
	m_fNeedWaitEnterArenaTime		= 0.0f;
	m_fLastRequestArenaStateTime	= 0.0f;
	m_fStartWaitFinishArenaTime		= 0.0f;
	m_fNeedWaitFinishAreanTime		= 0.0f;
	m_fArenaCurrentTurnLeftTime		= 0.0f;

	m_lArenaTurnCount				= 0;
	m_lRedGroupID					= 0;
	m_lBlueGroupID					= 0;
	m_lWinGroupID					= 0;
	m_lRedGroupScore				= 0;
	m_lBlueGroupScore				= 0;
	m_lRedGroupToken				= 0;
	m_lBlueGroupToken				= 0;
	m_lRedGroupDoorID				= 0;
	m_lBlueGroupDoorID				= 0;

	for(int i = 0; i < MAX_PLAYER_GROUP_MEMBER_LIST_SIZE; i++)
	{
		m_akRedGroupPlayerData[i].Reset();
		m_akBlueGroupPlayerData[i].Reset();
	}
	m_kPeakArenaLadderTopUsersMap.clear();
	ResetPeakLadderData();
  /*
	CF_ArenaModeConfig* pkCSVArenaModeConfig = CF_ArenaModeConfig::GetSingletonPtr();
	if(NULL == pkCSVArenaModeConfig)
		return;
	CF_ArenaModeConfig::DataEntry* pkData = pkCSVArenaModeConfig->GetEntryPtr(0);
	if(NULL == pkData)
		return;
	m_lSignUpLevel = pkData->_lSignUpLevel;
  */
  m_lSignUpLevel = 0;
}
//--------------------------------------------------------------------
LC_ClientArenaManager::~LC_ClientArenaManager()
{

}
//--------------------------------------------------------------------
void LC_ClientArenaManager::Reset()
{
	m_eArenaStage = LPAS_NONE;
	m_lRedGroupDoorID = 0;
	m_lBlueGroupDoorID = 0;

	m_lArenaTurnCount = 0;
	m_lRedGroupID = 0;
	m_lBlueGroupID = 0;
	m_lWinGroupID = 0;
	m_lRedGroupScore = 0;
	m_lBlueGroupScore = 0;
	m_lRedGroupToken = 0;
	m_lBlueGroupToken = 0;

	for(int i = 0; i < MAX_PLAYER_GROUP_MEMBER_LIST_SIZE; i++)
	{
		m_akRedGroupPlayerData[i].Reset();
		m_akBlueGroupPlayerData[i].Reset();
	}
	m_kPeakArenaLadderTopUsersMap.clear();
	ResetPeakLadderData();
}
//--------------------------------------------------------------------
bool LC_ClientArenaManager::CanRespawn()
{
	switch(m_eArenaType)
	{
	case LAT_TEST_MODE_1V1:
	case LAT_MATCH_TURN_2V2:
	case LAT_MATCH_TURN_3V3:
	case LAT_MATCH_TURN_5V5:
	case LAT_TRAIN_TURN_2V2:
	case LAT_TRAIN_TURN_3V3:
	case LAT_TRAIN_TURN_5V5:
		return false;
	case LAT_MATCH_FREE_5V5:
	case LAT_TRAIN_FREE_5V5:
	case LAT_FREE_MODE_1V1:
		return true;
	default:
		return false;
	}
}
//--------------------------------------------------------------------
void LC_ClientArenaManager::SetArenaState(int iState)
{
	m_eArenaState = (LC_ArenaState)iState;
}
//--------------------------------------------------------------------
void LC_ClientArenaManager::SetArenaStage(int iStage)
{
	m_eArenaStage = (LC_ArenaStage)iStage;
}
//--------------------------------------------------------------------
bool LC_ClientArenaManager::GetArenaModeOpen(int iArenaType)
{
  return false;
  /*
	CF_ArenaModeConfig* pkCSVArenaModeConfig = CF_ArenaModeConfig::GetSingletonPtr();
	if(NULL == pkCSVArenaModeConfig)
		return false;
	CF_ArenaModeConfig::DataEntry* pkData = pkCSVArenaModeConfig->GetEntryPtr(iArenaType);
	if(NULL == pkData)
		return false;

	return pkData->_bOpen;
  */
}
//--------------------------------------------------------------------
StringType LC_ClientArenaManager::GetArenaModeName(int iArenaType)
{
  return "";
  /*
	CF_ArenaModeConfig* pkCSVArenaModeConfig = CF_ArenaModeConfig::GetSingletonPtr();
	if(NULL == pkCSVArenaModeConfig)
		return "";
	CF_ArenaModeConfig::DataEntry* pkData = pkCSVArenaModeConfig->GetEntryPtr(iArenaType);
	if(NULL == pkData)
		return "";

	return pkData->_kArenaModeName;
  */
}
//--------------------------------------------------------------------
StringType LC_ClientArenaManager::GetArenaModeDescript(int iArenaType)
{
  return "";
  /*
	CF_ArenaModeConfig* pkCSVArenaModeConfig = CF_ArenaModeConfig::GetSingletonPtr();
	if(NULL == pkCSVArenaModeConfig)
		return "";
	CF_ArenaModeConfig::DataEntry* pkData = pkCSVArenaModeConfig->GetEntryPtr(iArenaType);
	if(NULL == pkData)
		return "";

	return pkData->_kArenaModeDescript;
  */
}
//--------------------------------------------------------------------
long LC_ClientArenaManager::GetEXPNeedToArenaLevelUp(int iLevel)
{
  return 100000000L;
  /*
	CF_ArenaLevelUp* pkCSVArenaLevelUp = CF_ArenaLevelUp::GetSingletonPtr();
	if(NULL == pkCSVArenaLevelUp)
		return 0;

	CF_ArenaLevelUp::DataEntry* pkData = pkCSVArenaLevelUp->GetEntryPtr(iLevel);
	if(NULL == pkData)
		return 0;

	return pkData->_lNeedEXPToLevelUp;
  */
}
//--------------------------------------------------------------------
StringType LC_ClientArenaManager::GetArenaLevelName(int iLevel)
{
  return "";
  /*
	CF_ArenaLevelUp* pkCSVArenaLevelUp = CF_ArenaLevelUp::GetSingletonPtr();
	if(NULL == pkCSVArenaLevelUp)
		return "";

	CF_ArenaLevelUp::DataEntry* pkData = pkCSVArenaLevelUp->GetEntryPtr(iLevel);
	if(NULL == pkData)
		return "";

	return pkData->_kLevelName;
  */
}
//--------------------------------------------------------------------
long LC_ClientArenaManager::GetArenaTurnCount()
{
	return m_lArenaTurnCount;
}
//--------------------------------------------------------------------
long LC_ClientArenaManager::GetArenaGroupID(int iArenaPlayerGroupType)
{
	switch(iArenaPlayerGroupType)
	{
	case LAPGT_RED_GROUP:
		return m_lRedGroupID;
	case LAPGT_BLUE_GROUP:
		return m_lBlueGroupID;
	default:
		return 0;
	}	
}
//--------------------------------------------------------------------
long LC_ClientArenaManager::GetArenaWinGroupType()
{	
	if(m_lWinGroupID == m_lRedGroupID)
		return LAPGT_RED_GROUP;
	
	if(m_lWinGroupID == m_lBlueGroupID)
		return LAPGT_BLUE_GROUP;

	return LAPGT_MAX_COUNT;
}
//--------------------------------------------------------------------
long LC_ClientArenaManager::GetPlayerArenaGroupType()
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkPlayer = pkLogicManager->GetOperationPlayer();
	if(NULL == pkPlayer)
	{	
		return LAPGT_BLUE_GROUP;
	}

	if(pkPlayer->GetPlayerGroupID() == m_lRedGroupID)
		return LAPGT_RED_GROUP;
	else
		return LAPGT_BLUE_GROUP;
}
//--------------------------------------------------------------------
long LC_ClientArenaManager::GetArenaGroupScore(int iArenaPlayerGroupType)
{
	switch(iArenaPlayerGroupType)
	{
	case LAPGT_RED_GROUP:
		return m_lRedGroupScore;
	case LAPGT_BLUE_GROUP:
		return m_lBlueGroupScore;
	default:
		return 0;
	}	
}
//--------------------------------------------------------------------
long LC_ClientArenaManager::GetArenaGroupToken(int iArenaPlayerGroupType)
{
	switch(iArenaPlayerGroupType)
	{
	case LAPGT_RED_GROUP:
		return m_lRedGroupToken;
	case LAPGT_BLUE_GROUP:
		return m_lBlueGroupToken;
	default:
		return 0;
	}	
}
//--------------------------------------------------------------------
LC_ClientArenaPlayerDataEntry* LC_ClientArenaManager::GetPlayerDataEntryInGroup(int iArenaPlayerGroupType,int iIndex)
{
	if(iIndex < 0 || iIndex >= MAX_PLAYER_GROUP_MEMBER_LIST_SIZE)
		return NULL;

	switch(iArenaPlayerGroupType)
	{
	case LAPGT_BLUE_GROUP:
		return &m_akBlueGroupPlayerData[iIndex];
	case LAPGT_RED_GROUP:
		return &m_akRedGroupPlayerData[iIndex];
	default:
		return NULL;
	}	
}
//--------------------------------------------------------------------
void LC_ClientArenaManager::RequestSignUpArena(int iArenaType)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkPlayer = pkLogicManager->GetOperationPlayer();
	if(NULL == pkPlayer)
	{	
		SendLogicInfoMessage(RE_ARENA_SYSTEM_ERROR);
		return;
	}


	NW_ClientSessionManager* pkSessionManager = NW_ClientSessionManager::GetSingletonPtr();
	if(NULL == pkSessionManager)
	{
		SendLogicInfoMessage(RE_ARENA_SYSTEM_ERROR);
		return;
	}



	LC_ActionForbid* pkActionForbid = pkPlayer->GetActionForbid();
	if(/*NULL != pkActionForbid &&*/ pkActionForbid->IsActionForbid(LAFT_ARENA,LAF_ARENA_SIGNUP))
	{
		SendLogicInfoMessage(RE_ACTION_FORBID);
		return;
	}
	
	////不能重复报名
	//if(m_eArenaStage != LPAS_NONE)
	//{
	//	SendLogicInfoMessage(RE_ARENA_SIGN_FAIL_ALEADY_SIGN);
	//	return;
	//}

	//if(pkPlayer->GetLevel() < m_lSignUpLevel)
	if(pkPlayer->GetCommanderLevel() < m_lSignUpLevel)
	{
		SendLogicInfoMessage(RE_ARENA_SIGN_FAIL_LOW_LEVEL,m_lSignUpLevel);
		return;
	}

	if(m_eArenaStage == LPAS_WAIT_ENTER)
		return;

	if(iArenaType < 0)
		iArenaType = 0;

	LC_PlayerGroupBase* pkGroup = pkPlayer->GetPlayerGroup();
	if(pkGroup)
	{
		//只有队长能报名
		if(pkGroup->GetGroupCheifID() != pkPlayer->GetOwnerObjectID())
		{
			SendLogicInfoMessage(RE_ARENA_SIGN_FAIL_NOT_CHEIF);
			return;
		}

		//队伍人数不能超过模式需求
		int iMemberCount = pkGroup->GetMemberCount(false);
		if(iMemberCount > LC_Helper::GetMaxSignUpCountByArenaType((LC_ArenaType)iArenaType))
		{
			SendLogicInfoMessage(RE_ARENA_SIGN_FAIL_TOO_MANY_MEMBER);
			return;
		}

		for(int i = 0; i < MAX_PLAYER_GROUP_MEMBER_LIST_SIZE; i++)
		{
			LC_PlayerGroupEntry* pkMember = pkGroup->GetGroupEntryInMemberList(i);
			if(pkMember && pkMember->GetValid())
			{
				if(pkMember->GetLevel() < m_lSignUpLevel)
				{
					SendLogicInfoMessage(RE_ARENA_SIGN_FAIL_GROUP_LOW_LEVEL,m_lSignUpLevel);
					return;
				}

				if(pkMember->GetState() == PLAYER_OFF_LINE)
				{
					SendLogicInfoMessage(RE_ARENA_SIGN_FAIL_GROUP_OFFLINE);
					return;
				}
			}
		}
	}


	//SetArenaStage(LPAS_WAIT_RLT_SIGNUP);

	//MG_REQ_SignUpArena signArenaMsg;
	//signArenaMsg.m_arenaType = iArenaType;
	//signArenaMsg.m_serverID = 0;

	//MG_PACKET_ENCODE_HANDLER hMsgHandler;
	//bool bCreateMsg = false;
	//bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SIGNUPARENA, &signArenaMsg);

	//if(bCreateMsg)
	//	pkPlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
//--------------------------------------------------------------------
void LC_ClientArenaManager::RequestCancelSignUpArena()
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkPlayer = pkLogicManager->GetOperationPlayer();
	if(NULL == pkPlayer)
	{	
		SendLogicInfoMessage(RE_ARENA_SYSTEM_ERROR);
		return;
	}

	NW_ClientSessionManager* pkSessionManager = NW_ClientSessionManager::GetSingletonPtr();
	if(NULL == pkSessionManager)
	{
		SendLogicInfoMessage(RE_ARENA_SYSTEM_ERROR);
		return;
	}

	LC_PlayerGroupBase* pkGroup = pkPlayer->GetPlayerGroup();
	if(pkGroup)
	{
		//只有队长能报名
		if(pkGroup->GetGroupCheifID() != pkPlayer->GetOwnerObjectID())
		{
			SendLogicInfoMessage(RE_ARENA_SIGN_CANCEL_FAIL_NOT_CHEIF);
			return;
		}
	}

	////不能重复取消
	//if(m_eArenaStage != LPAS_WAIT_MATCH && m_eArenaState != LPAS_WAIT_RLT_SIGNUP)
	//{
	//	//SendLogicInfoMessage(RE_ARENA_SIGN_FAIL_ALEADY_SIGN);
	//	return;
	//}

	//MG_REQ_CancelArena reqMsg;
	//reqMsg.m_arenaType	= m_eArenaType;
	//reqMsg.m_serverID	= 0;

	//MG_PACKET_ENCODE_HANDLER hMsgHandler;
	//bool bCreateMsg = false;
	//bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_CANCELARENA, &reqMsg);

	//if(bCreateMsg)
	//	pkPlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
//--------------------------------------------------------------------
void LC_ClientArenaManager::RequestCancleSignUpArenaInPrison()
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkPlayer = pkLogicManager->GetOperationPlayer();
	if(NULL == pkPlayer)
	{	
		SendLogicInfoMessage(RE_ARENA_SYSTEM_ERROR);
		return;
	}

	NW_ClientSessionManager* pkSessionManager = NW_ClientSessionManager::GetSingletonPtr();
	if(NULL == pkSessionManager)
	{
		SendLogicInfoMessage(RE_ARENA_SYSTEM_ERROR);
		return;
	}

	if(m_eArenaStage != LPAS_NONE)
	{
		//MG_REQ_CancelArena reqMsg;
		//reqMsg.m_arenaType	= m_eArenaType;
		//reqMsg.m_serverID	= 0;
		//
		//MG_PACKET_ENCODE_HANDLER hMsgHandler;
		//bool bCreateMsg = false;
		//bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_CANCELARENA, &reqMsg);

		//if(bCreateMsg)
		//	pkPlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}
//--------------------------------------------------------------------
void LC_ClientArenaManager::RequestSignUpPeakArena()
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkPlayer = pkLogicManager->GetOperationPlayer();
	if(NULL == pkPlayer)
	{	
		SendLogicInfoMessage(RE_ARENA_SYSTEM_ERROR);
		return;
	}

	NW_ClientSessionManager* pkSessionManager = NW_ClientSessionManager::GetSingletonPtr();
	if(NULL == pkSessionManager)
	{
		SendLogicInfoMessage(RE_ARENA_SYSTEM_ERROR);
		return;
	}

	MG_PB<msg_define::arena_signup_req> reqMsg;
	reqMsg.m_value.set_type(0);
	reqMsg.m_value.set_charid(pkPlayer->GetInstance());

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SIGNUPARENA,&reqMsg);

	if(bCreateMsg)
		pkPlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
//--------------------------------------------------------------------
void LC_ClientArenaManager::RequestLeavePeakArena()
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkPlayer = pkLogicManager->GetOperationPlayer();
	if(NULL == pkPlayer)
		return;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_LEAVEARENA);

	if(bCreateMsg)
		pkPlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
//--------------------------------------------------------------------
void LC_ClientArenaManager::ResetPeakLadderData()
{
	m_akPeakArenaLadderRankUsers.clear();
}
//-------------------------------------------------------------------
LC_ClientRankUserInfo* LC_ClientArenaManager::GetLadderUserInfo(int iIndex)
{
	return NULL;
}

//--------------------------------------------------------------------
LC_ClientRankUserInfo* LC_ClientArenaManager::GetTopLadderUserInfo(int iRanking)
{
	mem::map<int32_t, LC_ClientRankUserInfo>::iterator it = m_kPeakArenaLadderTopUsersMap.find(iRanking);
	if (it == m_kPeakArenaLadderTopUsersMap.end())
	{
		return NULL;
	}
	else
	{
		return &it->second;
	}
}

//--------------------------------------------------------------------
LC_ClientRankUserInfo* LC_ClientArenaManager::GetLadderRankUserInfo(int iRanking)
{
	if (iRanking < PEAKARENA_RANKINGLIST_TOP)
	{
		LC_ClientRankUserInfo *info = GetTopLadderUserInfo(iRanking);
		if (NULL != info)
		{
			return info;
		}
	}

	mem::map<int32_t, LC_ClientRankUserInfo>::iterator it = m_akPeakArenaLadderRankUsers.find(iRanking);
	if (it == m_akPeakArenaLadderRankUsers.end())
	{
		return NULL;
	}
	else
	{
		return &it->second;
	}
}
//--------------------------------------------------------------------
void LC_ClientArenaManager::RequestLeaveArena()
{
 	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
 	LC_ClientGamePlayer* pkPlayer = pkLogicManager->GetOperationPlayer();
 	if(NULL == pkPlayer)
 		return;
 
 	MG_PACKET_ENCODE_HANDLER hMsgHandler;
 	bool bCreateMsg = false;
 	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_LEAVEARENA);
 
 	if(bCreateMsg)
 		pkPlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
//--------------------------------------------------------------------
void LC_ClientArenaManager::RequestArenaState()
{
// 	float fCurrentTime = GET_PROCESS_TIME;
// 	if(fCurrentTime - m_fLastRequestArenaStateTime > 15.0f)
// 	{
// 		m_fLastRequestArenaStateTime = fCurrentTime;
// 
// 		LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
// 		LC_ClientGamePlayer* pkPlayer = pkLogicManager->GetOperationPlayer();
// 		if(NULL == pkPlayer)
// 			return;
// 
// 		MG_REQ_ArenaState reqMsg;
// 
// 
// 		MG_PACKET_ENCODE_HANDLER hMsgHandler;
// 		bool bCreateMsg = false;
// 		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_ARENASTATE, &reqMsg);
// 
// 		if(bCreateMsg)
// 			pkPlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
// 	}
}
//--------------------------------------------------------------------
//void LC_ClientArenaManager::RecvSignUpArena(Protocol::MG_RLT_SignUpArena& rkMsg)
//{
//	if (rkMsg.m_iResult == RE_ARENA_SIGN_FAIL_GROUP_LOW_LEVEL
//		||rkMsg.m_iResult == RE_ARENA_SIGN_FAIL_LOW_LEVEL)
//	{
//		SendLogicInfoMessage(rkMsg.m_iResult,m_lSignUpLevel);
//	}
//	else
//	{
//		SendLogicInfoMessage(rkMsg.m_iResult);
//	}
//
//	
//	if(rkMsg.m_iResult != RE_ARENA_SIGN_SUCCESS && 
//		rkMsg.m_iResult != RE_ARENA_SIGN_FAIL_ALEADY_SIGN)
//	{
//		SetArenaStage(LPAS_NONE);
//		return;
//	}
//
//	SetArenaStage(LPAS_WAIT_MATCH);
//	m_eArenaType			= (LC_ArenaType)rkMsg.m_arenaType;
//	m_iArenaSignUpUserCount = rkMsg.m_userCount;
//}
//--------------------------------------------------------------------
void LC_ClientArenaManager::RecvCancelSignUpArena(Protocol::MG_RLT_CancelArena& rkMsg)
{
	if(m_eArenaStage != LPAS_NONE)
		SendLogicInfoMessage(rkMsg.m_iResult);

	//if(rkMsg.m_iResult != RE_ARENA_CANCEL_SIGN_SUCCESS )
	//	return;


	SetArenaStage(LPAS_NONE);
	m_eArenaType				= LAT_MAX_COUNT;
	m_iArenaSignUpUserCount		= 0;
}
//--------------------------------------------------------------------
void LC_ClientArenaManager::RecvMatchArena(Protocol::MG_RLT_MatchArena& rkMsg)
{
	//SendLogicInfoMessage(RE_TIP_MESSAGE,rkMsg.m_serverID);

	m_eArenaType = (LC_ArenaType)rkMsg.m_arenaType;

	SetArenaStage(LPAS_WAIT_ENTER);

	m_fStartWaitEnterArenaTime = GET_PROCESS_TIME;
	m_fNeedWaitEnterArenaTime  = rkMsg.m_waitingEnterTime;
}
//--------------------------------------------------------------------
void LC_ClientArenaManager::RecvEnterArena(Protocol::MG_RLT_EnterArena& rkMsg)
{
	if (RE_SUCCESS != rkMsg.m_rslt)
	{
		SendLogicInfoMessage(rkMsg.m_rslt);
		return;
	}



	NW_ClientSessionManager* pkSessionManager	= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup* pkSessionGroup		= pkSessionManager->GetGameSessionGroup();
	if (NULL == pkSessionGroup)
		return;
	
	pkSessionGroup->m_pkGameStageMachine->SetCloseGameFlag(false);
	pkSessionGroup->m_pkGameStageMachine->SetChangeServerFlag(true);
	pkSessionGroup->m_pkGameStageMachine->SetNeedResetGameFlag(false);

	pkSessionGroup->m_pkGameStageMachine->SetNextState(ST_GAME_STAGE_QUIT);
}
//--------------------------------------------------------------------
// void LC_ClientArenaManager::RecvLeaveArena(Protocol::MG_RLT_LeaveArena& rkMsg)
// {
// 	LC_GameEvent kGameEvent;
// 	kGameEvent.SetEventType(GLET_ARENA_HIDE_ALL_TIP);
// 
// 	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
// 	pkUIManager->HandlerGameEvent(kGameEvent);
// 
// 	NW_ClientSessionManager* pkSessionManager	= NW_ClientSessionManager::GetSingletonPtr();
// 	NW_ClientSessionGroup* pkSessionGroup		= pkSessionManager->GetGameSessionGroup();
// 	if (NULL == pkSessionGroup)
// 		return;
// 
// 	pkSessionGroup->m_pkGameStageMachine->SetCloseGameFlag(false);
// 	pkSessionGroup->m_pkGameStageMachine->SetChangeServerFlag(true);
// 	pkSessionGroup->m_pkGameStageMachine->SetNeedResetGameFlag(false);
// 
// 	pkSessionGroup->m_pkGameStageMachine->SetNextState(ST_GAME_STAGE_QUIT);
// 
// 
// 	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
// 	LC_ClientGamePlayer* pkPlayer = pkLogicManager->GetOperationPlayer();
// 	if(NULL == pkPlayer)
// 		return;
// 	SetArenaStage(LPAS_NONE);
// }
//--------------------------------------------------------------------
void LC_ClientArenaManager::RecvArenaState(Protocol::MG_RLT_ArenaState& rkMsg)
{
	SetArenaState(rkMsg.m_kArenaStateData.m_iArenaState);
	m_eArenaType				= (LC_ArenaType)rkMsg.m_kArenaStateData.m_iArenaType;
	m_lRedGroupID				= rkMsg.m_kArenaStateData.m_iRedGroupID;
	m_lBlueGroupID				= rkMsg.m_kArenaStateData.m_iBlueGroupID;
	m_lArenaTurnCount			= rkMsg.m_kArenaStateData.m_iArenaTurnCount;
	m_lRedGroupScore			= rkMsg.m_kArenaStateData.m_iRedGroupScore;
	m_lBlueGroupScore			= rkMsg.m_kArenaStateData.m_iBlueGroupScore;
	m_fArenaCurrentTurnLeftTime = rkMsg.m_kArenaStateData.m_iArenaTime;


	for(int i = 0; i < MAX_PLAYER_GROUP_MEMBER_LIST_SIZE; i++)
	{
		LC_ClientArenaPlayerDataEntry& rkRedPlayerGroupMemeberData = m_akRedGroupPlayerData[i];
		
		if(i >= rkMsg.m_kArenaStateData.m_iRedGroupMemberCount)
			rkRedPlayerGroupMemeberData.Reset();
		else
		{
			rkRedPlayerGroupMemeberData.GetDataFromProtocolStruct(rkMsg.m_kArenaStateData.m_akRedGroupPlayerArenaData[i]);
		}
		
		LC_ClientArenaPlayerDataEntry& rkBluePlayerGroupMemeberData = m_akBlueGroupPlayerData[i];
		if(i >= rkMsg.m_kArenaStateData.m_iBlueGroupMemberCount)
			rkBluePlayerGroupMemeberData.Reset();
		else
		{
			rkBluePlayerGroupMemeberData.GetDataFromProtocolStruct(rkMsg.m_kArenaStateData.m_akBlueGroupPlayerArenaData[i]);
		}
	}
}
//--------------------------------------------------------------------
void LC_ClientArenaManager::RecvArenaResult(Protocol::MG_RLT_ArenaResult& rkMsg)
{
	SetArenaStage(LPAS_WAIT_FINISH);
	
	m_eArenaType				= (LC_ArenaType)rkMsg.m_kArenaResultData.m_iArenaType;
	m_lRedGroupID				= rkMsg.m_kArenaResultData.m_iRedGroupID;
	m_lBlueGroupID				= rkMsg.m_kArenaResultData.m_iBlueGroupID;
	m_lWinGroupID				= rkMsg.m_kArenaResultData.m_iWinGroupID;
	m_lRedGroupScore			= rkMsg.m_kArenaResultData.m_iRedGroupScore;
	m_lBlueGroupScore			= rkMsg.m_kArenaResultData.m_iBlueGroupScore;
	m_lRedGroupToken			= rkMsg.m_kArenaResultData.m_iRedGroupToken;
	m_lBlueGroupToken			= rkMsg.m_kArenaResultData.m_iBlueGroupToken;
	m_fNeedWaitFinishAreanTime  = rkMsg.m_kArenaResultData.m_iShowTime;

	m_fStartWaitFinishArenaTime = GET_PROCESS_TIME;


	for(int i = 0; i < MAX_PLAYER_GROUP_MEMBER_LIST_SIZE; i++)
	{
		LC_ClientArenaPlayerDataEntry& rkRedPlayerGroupMemeberData = m_akRedGroupPlayerData[i];

		if(i >= rkMsg.m_kArenaResultData.m_iRedGroupMemberCount)
			rkRedPlayerGroupMemeberData.Reset();
		else
		{
			rkRedPlayerGroupMemeberData.GetDataFromProtocolStruct(rkMsg.m_kArenaResultData.m_akRedGroupPlayerResultData[i]);
		}

		LC_ClientArenaPlayerDataEntry& rkBluePlayerGroupMemeberData = m_akBlueGroupPlayerData[i];
		if(i >= rkMsg.m_kArenaResultData.m_iBlueGroupMemberCount)
			rkBluePlayerGroupMemeberData.Reset();
		else
		{
			rkBluePlayerGroupMemeberData.GetDataFromProtocolStruct(rkMsg.m_kArenaResultData.m_akBlueGroupPlayerResultData[i]);
		}
	}
}
//--------------------------------------------------------------------
void LC_ClientArenaManager::RecvArenaDoorOpen()
{
	/*LC_ClientLocalNPCManager* pkLocalNpcManager = CLIENT_GET_LOCAL_NPC_MANAGER();
	if(!pkLocalNpcManager)
		return;

	LC_ClientNPC* pkRedGroupDoor = (LC_ClientNPC*)pkLocalNpcManager->FindNPC(m_lRedGroupDoorID);
	if(pkRedGroupDoor)
    {
        pkRedGroupDoor->PlayAnimation("特殊动作1");
        pkRedGroupDoor->DisableDynamicCollision();		
    }

	LC_ClientNPC* pkBlueGroupDoor = (LC_ClientNPC*)pkLocalNpcManager->FindNPC(m_lBlueGroupDoorID);
	if(pkBlueGroupDoor)
    {
        pkBlueGroupDoor->PlayAnimation("特殊动作1");
        pkBlueGroupDoor->DisableDynamicCollision();		
    }*/
}
//--------------------------------------------------------------------
void LC_ClientArenaManager::RecvArenaDoorClose()
{
	//LC_ClientLocalNPCManager* pkLocalNpcManager = CLIENT_GET_LOCAL_NPC_MANAGER();
	//if(!pkLocalNpcManager)
	//	return;

	//if (!m_lRedGroupDoorID)
	//{
	//	m_lRedGroupDoorID = pkLocalNpcManager->CreateLocalNPC(960000,"", true);
	//}
	//if (!m_lBlueGroupDoorID)
	//{
	//	m_lBlueGroupDoorID = pkLocalNpcManager->CreateLocalNPC(960001,"", true);
	//}
	//
	//LC_ClientNPC* pkRedGroupDoor = (LC_ClientNPC*)pkLocalNpcManager->FindNPC(m_lRedGroupDoorID);
	//if(pkRedGroupDoor)
	//{	
	//	//pkRedGroupDoor->PlayAnimation("特殊动作2");
	//	pkRedGroupDoor->PlayIdleAnimation();
	//}
	//LC_ClientNPC* pkBlueGroupDoor = (LC_ClientNPC*)pkLocalNpcManager->FindNPC(m_lBlueGroupDoorID);
	//if(pkBlueGroupDoor)
	//{
	//	//pkBlueGroupDoor->PlayAnimation("特殊动作2");
	//	pkBlueGroupDoor->PlayIdleAnimation();
	//}

}
//--------------------------------------------------------------------
void LC_ClientArenaManager::RecvArenaTip(Protocol::MG_RLT_ArenaTip& rkMsg)
{
	StringType kSrcPlayerName = STDSTR2TPSTR(rkMsg.m_szSrcPlayerName);
	StringType kDesPlayerName = STDSTR2TPSTR(rkMsg.m_szDesPlayerName);

	if(rkMsg.m_iTipResult == RE_ARENA_TURN_PREPARE_END)
	{
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_ARENA_START);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);

	}else if (rkMsg.m_iTipResult == RE_ARENA_TURN_PREPARE_BEGIN)
	{
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_ARENA_TURN_PREPARE_START);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);

	}else if (rkMsg.m_iTipResult == RE_ARENA_NO_GROUP_WIN)
	{
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_ARENA_DEUCE);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);

	}else if (rkMsg.m_iTipResult == RE_ARENA_RED_GROUP_WIN_BY_KILL_ALL
				||rkMsg.m_iTipResult == RE_ARENA_RED_GROUP_WIN_BY_TIME_END)
	{
		long lPlayerGroupType = GetPlayerArenaGroupType();
		
		if (lPlayerGroupType == LAPGT_RED_GROUP)
		{
			LC_GameEvent kGameEvent;
			kGameEvent.SetEventType(GLET_ARENA_WIN);

			UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
			pkUIManager->HandlerGameEvent(kGameEvent);
		}else
		{
			LC_GameEvent kGameEvent;
			kGameEvent.SetEventType(GLET_ARENA_LOSE);

			UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
			pkUIManager->HandlerGameEvent(kGameEvent);
		}

	}else if (rkMsg.m_iTipResult == RE_ARENA_BLUE_GROUP_WIN_BY_KILL_ALL
				||rkMsg.m_iTipResult == RE_ARENA_BLUE_GROUP_WIN_BY_TIME_END)
	{
		long lPlayerGroupType = GetPlayerArenaGroupType();

		if (lPlayerGroupType == LAPGT_BLUE_GROUP)
		{
			LC_GameEvent kGameEvent;
			kGameEvent.SetEventType(GLET_ARENA_WIN);

			UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
			pkUIManager->HandlerGameEvent(kGameEvent);
		}else
		{
			LC_GameEvent kGameEvent;
			kGameEvent.SetEventType(GLET_ARENA_LOSE);

			UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
			pkUIManager->HandlerGameEvent(kGameEvent);
		}
	}
	SendLogicInfoMessage(rkMsg.m_iTipResult,kSrcPlayerName,kDesPlayerName);
}

void LC_ClientArenaManager::RequestGiveUpArena()
{
// 	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
// 	LC_ClientGamePlayer* pkPlayer = pkLogicManager->GetOperationPlayer();
// 	if(NULL == pkPlayer)
// 		return;
// 
// 	MG_REQ_LeaveArena reqMsg;
// 
// 	MG_PACKET_ENCODE_HANDLER hMsgHandler;
// 	bool bCreateMsg = false;
// 	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GIVEUPARENA, &reqMsg);
// 
// 	if(bCreateMsg)
// 		pkPlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

// void LC_ClientArenaManager::RecvGiveUpArena( Protocol::MG_RLT_LeaveArena& rkMsg )
// {
// 	LC_GameEvent kGameEvent;
// 	kGameEvent.SetEventType(GLET_ARENA_HIDE_ALL_TIP);
// 
// 	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
// 	pkUIManager->HandlerGameEvent(kGameEvent);
// 
// 	NW_ClientSessionManager* pkSessionManager	= NW_ClientSessionManager::GetSingletonPtr();
// 	NW_ClientSessionGroup* pkSessionGroup		= pkSessionManager->GetGameSessionGroup();
// 	if (NULL == pkSessionGroup)
// 		return;
// 
// 	pkSessionGroup->m_pkGameStageMachine->SetCloseGameFlag(false);
// 	pkSessionGroup->m_pkGameStageMachine->SetChangeServerFlag(true);
// 	pkSessionGroup->m_pkGameStageMachine->SetNeedResetGameFlag(false);
// 
// 	pkSessionGroup->m_pkGameStageMachine->SetNextState(ST_GAME_STAGE_QUIT);
// 
// 
// 	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
// 	LC_ClientGamePlayer* pkPlayer = pkLogicManager->GetOperationPlayer();
// 	if(NULL == pkPlayer)
// 		return;
// 	SetArenaStage(LPAS_NONE);
// }
//-----------------------------------------------------------------------
void LC_ClientArenaManager::RecvArenaMatchAgain()
{
	SetArenaStage(LPAS_WAIT_MATCH);

	SendLogicInfoMessage(RE_ARENA_MATCH_FAIL);
}
//-----------------------------------------------------------------------
void LC_ClientArenaManager::RecvArenaConfirmCancel()
{
	SetArenaStage(LPAS_NONE);
	m_eArenaType				= LAT_MAX_COUNT;
	m_iArenaSignUpUserCount		= 0;

	SendLogicInfoMessage(RE_ARENA_CANCEL_SIGN_SUCCESS);
}

StringType LC_ClientArenaManager::GetArenaRankRewardGrade()
{
	StringStreamType sStream;

	CF_RankRewardGrade::DataEntryMapExternIterator iter = CF_RankRewardGrade::GetSingletonPtr()->GetIterator();
	for(; iter.HasMoreElements(); iter.MoveNext())
	{
		const CF_RankRewardGrade::DataEntry *pkData = iter.PeekNextValuePtr();
		sStream << pkData->_iRankStartIndex << '#' << pkData->_iRankEndIndex << '#' << pkData->_kReward << '|';
	}

	return sStream.str();
}