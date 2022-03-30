#include "LC_ClientArenaManager.h"
#include "LC_ClientLogicManager.h"
#include "LC_ClientGameEvent.h"

using namespace GameLogic;

//---------------------------------------------------------------------
void HL_FromGameServer::_handlerRltArenaOpeartion(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_PB<msg_define::arena_opertaion_resp> rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	msg_define::arena_opertaion_resp& rk_resp = rltMsg.m_value;
	if(rk_resp.rslt() != RE_SUCCESS)
	{
		SendLogicInfoMessage(rk_resp.rslt());		
	}
	msg_define::arena_opertaion_req rk_req = rk_resp.req();
	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_PEAK_ARENA_OPERATION);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1,rk_resp.rslt());
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2,rk_req.op_code());
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_3,rk_req.op_params(0));
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}
//---------------------------------------------------------------------
void HL_FromGameServer::_handlerRltSignArena( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_PB<msg_define::arena_signup_resp> rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	msg_define::arena_signup_resp& rk_resp = rltMsg.m_value;

	if(rk_resp.rslt() != RE_SUCCESS)
	{
		SendLogicInfoMessage(rk_resp.rslt());		
	}

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_PEAK_ARENA_SIGNUP_RESP);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1,rk_resp.rslt());
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}
//---------------------------------------------------------------------
void HL_FromGameServer::_handlerRltCancelArena( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RLT_CancelArena rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
		return;

	ResultType iResult   = rltMsg.m_iResult;
	if(iResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(rltMsg.m_iResult);		
		return;
	}

	LC_ClientArenaManager* pkArenaManager = LC_ClientArenaManager::GetSingletonPtr();
	if(pkArenaManager)
		pkArenaManager->RecvCancelSignUpArena(rltMsg);
}
//---------------------------------------------------------------------
void HL_FromGameServer::_handlerRltMatchArena( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RLT_MatchArena rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
		return;

	LC_ClientArenaManager* pkArenaManager = LC_ClientArenaManager::GetSingletonPtr();
	if(pkArenaManager)
		pkArenaManager->RecvMatchArena(rltMsg);
}
//---------------------------------------------------------------------
void HL_FromGameServer::_handlerRltEnterArena( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RLT_EnterArena rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
		return;

	ResultType iResult   = rltMsg.m_rslt;
	if(iResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(rltMsg.m_rslt);		
		return;
	}

	LC_ClientArenaManager* pkArenaManager = LC_ClientArenaManager::GetSingletonPtr();
	if(pkArenaManager)
		pkArenaManager->RecvEnterArena(rltMsg);
}
//---------------------------------------------------------------------
void HL_FromGameServer::_handlerRltLeaveArena( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
// 	MG_RLT_LeaveArena rltMsg;
// 	if(!rltMsg.Decode(pMsg,wSize))
// 		return;
// 
// 	LC_ClientArenaManager* pkArenaManager = LC_ClientArenaManager::GetSingletonPtr();
// 	if(pkArenaManager)
// 		pkArenaManager->RecvLeaveArena(rltMsg);
}
//---------------------------------------------------------------------
void HL_FromGameServer::_handlerRltArenaState( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RLT_ArenaState rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
		return;

	LC_ClientArenaManager* pkArenaManager = LC_ClientArenaManager::GetSingletonPtr();
	if(pkArenaManager)
		pkArenaManager->RecvArenaState(rltMsg);
}
//---------------------------------------------------------------------
void HL_FromGameServer::_handlerRltArenaResult( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RLT_ArenaResult rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
		return;

	LC_ClientArenaManager* pkArenaManager = LC_ClientArenaManager::GetSingletonPtr();
	if(pkArenaManager)
		pkArenaManager->RecvArenaResult(rltMsg);
}
//---------------------------------------------------------------------
void HL_FromGameServer::_handlerRltArenaOpenDoor( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	LC_ClientArenaManager* pkArenaManager = LC_ClientArenaManager::GetSingletonPtr();
	if(pkArenaManager)
		pkArenaManager->RecvArenaDoorOpen();

}
//---------------------------------------------------------------------

//---------------------------------------------------------------------
void HL_FromGameServer::_handlerRltArenaTip(NW_Game2ClientSession* pkSession,char* pMsg,uint16_t wSize)
{
	MG_RLT_ArenaTip rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
		return;

	LC_ClientArenaManager* pkArenaManager = LC_ClientArenaManager::GetSingletonPtr();
	if(pkArenaManager)
		pkArenaManager->RecvArenaTip(rltMsg);
}

void HL_FromGameServer::_handlerRltGiveUpArena(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
// 	MG_RLT_LeaveArena rltMsg;
// 	if(!rltMsg.Decode(pMsg,wSize))
// 		return;
// 
// 	LC_ClientArenaManager* pkArenaManager = LC_ClientArenaManager::GetSingletonPtr();
// 	if(pkArenaManager)
// 		pkArenaManager->RecvGiveUpArena(rltMsg);
}

void HL_FromGameServer::_handlerRltArenaConfirmCancel(NW_Game2ClientSession* pkSession,char* pMsg,uint16_t wSize)
{
	LC_ClientArenaManager* pkArenaManager = LC_ClientArenaManager::GetSingletonPtr();
	if(pkArenaManager)
		pkArenaManager->RecvArenaConfirmCancel();
}

void HL_FromGameServer::_handlerRltArenaMatchAgain(NW_Game2ClientSession* pkSession,char* pMsg,uint16_t wSize)
{
	LC_ClientArenaManager* pkArenaManager = LC_ClientArenaManager::GetSingletonPtr();
	if(pkArenaManager)
		pkArenaManager->RecvArenaMatchAgain();
}

//----------------------新竞技场消息处理----------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncArenaBattleInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_GC_SyncArenaBattleInfo rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return;
	}
	pkGamePlayer->GetArenaBattleAssert()->GetDataFromProtocolStruct(rltMsg.m_DetailInfo);

	pkGamePlayer->SetDirtyFlag(DIRTY_FLAG_ARENA_BATTLE_ASSET, true);
	pkGamePlayer->AddDirtyFlagRevisionID(DIRTY_FLAG_ARENA_BATTLE_ASSET);
}
void HL_FromGameServer::_handlerRankQueryChallengeInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	//对应3个人
	MG_GG_RESP_Query_User_Challenge rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	//for (int i=0;i<rltMsg.m_Size;i++)
	//{
	//	int isize = rltMsg.m_UserInfos[i].m_UserInfo.m_BuddyData.m_nBuddyInfo.size();
	//	if (isize > 3 || isize < 1)
	//	{
	//		StringType sStream = "id:" + Utility::Int2String(rltMsg.m_UserInfos[i].m_UserInfo.m_CharID) + "\n size:" + Utility::Int2String(isize);
	//		MessageBox(NULL,sStream.c_str(),"Warning",MB_OK|MB_ICONWARNING);
	//	}
	//}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return;
	}
	pkGamePlayer->GetArenaBattleChallengeInfoAsset()->UpdateArenaBattleInfo(rltMsg);
	
	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_CLIENT_RLT_RANK_INFO);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, MGPT_RESP_RANK_QUERY_CHALLENGE_INFO);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}
void HL_FromGameServer::_handlerRltRankChallenge(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_FAILED_Challenge rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	if(rltMsg.m_iResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(rltMsg.m_iResult);		
		return;
	}

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_BATTLE_RLT);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, rltMsg.m_ChallengeType);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}
void HL_FromGameServer::_handlerNotifyRankChallenge(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_NOTIFY_Challenge rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	LC_ClientGameEvent kGameEvent;
	kGameEvent.SetClientEventType(GameLogic::CGET_ARENA_SETTLE_INFO);

	LC_ClientGameEventUserData_ArenaSettleInfo kUserData = LC_ClientGameEventUserData_ArenaSettleInfo(rltMsg);
	kGameEvent.SetUserData(&kUserData);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);

	switch(rltMsg.m_nSoloType)
	{
	case eSoloDuleType_Arena:
		{
			if (rltMsg.m_PreIndex != rltMsg.m_CurIndex)
			{
				LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
				//pkGamePlayer->GetArenaBattleChallengeInfoAsset().GetDataFromProtocolStruct(rltMsg);
			}
		}
		break;
	}
}
void HL_FromGameServer::_handlerNotifyRankBeChallenge(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_NOTIFY_beChallenge rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return;
	}

	if(rltMsg.nType == Arena_Type_Firmament)
	{
		//苍穹战场被别人挑战引发积分发生变化时，需要重新请求一下对战信息
		pkGamePlayer->SetSkyArenaPoint(rltMsg.m_Score);
		//pkGamePlayer->ReqGetSkyArenaBattleInfo(false, MAPID_SKYARENA);
		pkGamePlayer->ReqSkyArenaMyScore();
	}

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_BATTLERECORD_NEW);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, rltMsg.nType);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerGetRankGuildBossActivity(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_GetGuildMapBossRank rltMsg;
	if (!rltMsg.Decode(pMsg, wSize))
		return;

	if (rltMsg.m_nResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(rltMsg.m_nResult);
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
		return;

	pkGamePlayer->SetGuildBossActivityMyRank(rltMsg.m_nMyselfInfo);
	if (!rltMsg.m_nPlayerInfo.empty())
	{
		bool rankthree = rltMsg.m_nPlayerInfo[0].m_nRankIndex == 0;
		pkGamePlayer->SetGuildBossActivityRank(rltMsg.m_nPlayerInfo, rankthree);
	}
	else
	{
		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
		pkGamePlayer->ReSetGuildBossActivityRankList();
	}

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_CLIENT_RLT_RANK_INFO);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, MGPT_RLT_GETGUILDMAPBOSSRANK);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, rltMsg.m_nMapID);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_3, rltMsg.m_nTotalCount);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_4, rltMsg.m_nMyselfInfo.m_nDamageData);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_5, rltMsg.m_nMyselfInfo.m_nRankIndex);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerGetRankYiZhongQinxi(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_GetRankYiZhongQinXi rltMsg;
	if (!rltMsg.Decode(pMsg, wSize))
		return;

	if (rltMsg.m_nResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(rltMsg.m_nResult);
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
		return;
	
	pkGamePlayer->SetRaidBossSpeciesMyRank(rltMsg.m_nMyselfInfo);
	pkGamePlayer->SetRaidBossSpeciesMyDamageCount(rltMsg.m_nStoryID,rltMsg.m_nMyselfInfo.m_nDamageData);
	if (!rltMsg.m_nPlayerInfo.empty())
	{
		bool rankthree = rltMsg.m_nPlayerInfo[0].m_nRankIndex == 0;
		pkGamePlayer->SetRaidBossSpeciesRank(rltMsg.m_nPlayerInfo, rankthree);
	}
	else
	{
		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
		pkGamePlayer->ReSetRaidBossSpeciesRankThree();
	}

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_CLIENT_RLT_RANK_INFO);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, MGPT_RLT_GETRANKYIZHONGQINXI);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, rltMsg.m_nStoryID);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_3, rltMsg.m_nTotalCount);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_4, rltMsg.m_nMyselfInfo.m_nDamageData);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_5, rltMsg.m_nMyselfInfo.m_nRankIndex);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerRltYizhongqingxiDamageData(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_YiZhongQinXiDamageData rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	if(rltMsg.m_nResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(rltMsg.m_nResult);
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->RltRaidBossSpeciesDamageData(rltMsg);
}

void HL_FromGameServer::_handlerRltYizhongqingxiDetailDamageData(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_YiZhongQinXiDetailDamageData rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	if(rltMsg.m_nResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(rltMsg.m_nResult);
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->RltRaidBossSpeciesDamageDataDetail(rltMsg);
}

void HL_FromGameServer::_handlerGetAnswerRankInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_AnswerRankInfo rltMsg;
	if (!rltMsg.Decode(pMsg, wSize))
		return;

	if (rltMsg.m_nResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(rltMsg.m_nResult);
		return;
	}

	StringStreamType sStream("");
	sStream << rltMsg.m_nTotalCount <<"#"<< (int)rltMsg.m_nRankType<<"#" ;
	sStream << rltMsg.m_nMyselfInfo.m_nRankIndex << "|" << rltMsg.m_nMyselfInfo.m_nCharName << "|" << rltMsg.m_nMyselfInfo.m_nAnswerAccPoint << "#";
	for (int i=0;i<rltMsg.m_nPlayerInfo.size();++i)
	{
		sStream << rltMsg.m_nPlayerInfo[i].m_nRankIndex << "|" << rltMsg.m_nPlayerInfo[i].m_nCharName << "|" << rltMsg.m_nPlayerInfo[i].m_nAnswerAccPoint << ";";
	}
	sStream << "#";
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerMsgGameEvent(MGPT_RLT_ANSWERRANKINFO, sStream.str());
}

void HL_FromGameServer::_handlerGetYijituxiRankInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_YiJiTuXiRankInfo rltMsg;
	if (!rltMsg.Decode(pMsg, wSize))
		return;

	if (rltMsg.m_nResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(rltMsg.m_nResult);
		return;
	}

	StringStreamType sStream("");
	sStream << rltMsg.m_bFlag << "#" << rltMsg.m_nTotalCount << "#" ;
	sStream << rltMsg.m_nMyselfInfo.m_nRankIndex << "|" << rltMsg.m_nMyselfInfo.m_nCharName << "|" << rltMsg.m_nMyselfInfo.m_nValue << "#";
	for (int i=0;i<rltMsg.m_nPlayerInfo.size();++i)
	{
		sStream << rltMsg.m_nPlayerInfo[i].m_nRankIndex << "|" << rltMsg.m_nPlayerInfo[i].m_nCharName << "|" << rltMsg.m_nPlayerInfo[i].m_nValue << ";";
	}
	sStream << "#";
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerMsgGameEvent(MGPT_RLT_YIJITUXIRANKINFO, sStream.str());
}

void HL_FromGameServer::_handlerRespRankInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RESP_RankInfoResult rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"排行榜  收到回复MGPT_RESP_RANK_INFO，m_RankID=%d, m_Begin=%d, m_End=%d",rltMsg.m_RankID, rltMsg.m_Begin, rltMsg.m_End);

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return;
	}
	pkGamePlayer->GetArenaBattleChallengeInfoAsset()->UpdateShortInfo(rltMsg);

	const PS_Rank_SinglePlayerInfo& myInfo = rltMsg.m_MyselfInfo;
	if(rltMsg.m_RankID == Rank_Type_SkyArena && myInfo.m_RankIndex >= 0)
	{
		pkGamePlayer->SetSkyArenaPoint(myInfo.m_RankValue);
	}
	
	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_CLIENT_RLT_RANK_INFO);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, MGPT_RESP_RANK_INFO);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, rltMsg.m_RankID);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_3, myInfo.m_RankIndex);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_4, myInfo.m_RankValue);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_5, rltMsg.m_TotalSize);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_6, rltMsg.m_nGroupId);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerChallengeRankInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_GG_RESP_QueryChalleng_Rank rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"竞技场  收到回复MGPT_CHALLENG_RANK_RESP");

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return;
	}
	pkGamePlayer->GetArenaBattleChallengeInfoAsset()->UpdateArenaRankListInfo(rltMsg);

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_CLIENT_RLT_RANK_INFO);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, MGPT_CHALLENG_RANK_RESP);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerRespRankPlayerBuddyInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RESP_GetRankPlayerBuddyInfo rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return;
	}
	pkGamePlayer->GetArenaBattleChallengeInfoAsset()->UpdateDetailInfo(rltMsg);

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_CLIENT_RLT_RANK_INFO);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, MGPT_RESP_GETRANK_PLAYERBUDDY_INFO);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerRespRankBuddyInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RESP_GetRankBuddyInfo rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return;
	}
	pkGamePlayer->GetArenaBattleChallengeInfoAsset()->UpdateDetailInfo(rltMsg);

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_CLIENT_RLT_RANK_INFO);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, MGPT_RESP_GETRANK_BUDDY_INFO);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, rltMsg.m_BuddyID);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerRespRankPetInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RESP_GetRankPetInfo rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return;
	}
	pkGamePlayer->GetArenaBattleChallengeInfoAsset()->UpdateDetailInfo(rltMsg);

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_CLIENT_RLT_RANK_INFO);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, MGPT_RESP_GETRANK_PET_INFO);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerRespRankSubInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RESP_GetRankSubInfo rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return;
	}
	pkGamePlayer->GetArenaBattleChallengeInfoAsset()->UpdateDetailInfo(rltMsg);

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_CLIENT_RLT_RANK_INFO);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, MGPT_RESP_GETRANK_SUB_INFO);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerRespRankTopnPlayerInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RESP_RankTopNPlayerInfo rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return;
	}
	pkGamePlayer->GetArenaBattleChallengeInfoAsset()->UpdateDetailInfo(rltMsg);

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_CLIENT_RLT_RANK_INFO);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, MGPT_RESP_RANK_TOPN_PLAYER_INFO);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerRespBuddyRankIndexEx(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RESP_GetBuddyRankIndex rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return;
	}
	pkGamePlayer->GetArenaBattleChallengeInfoAsset()->UpdateDetailInfo(rltMsg);

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_CLIENT_RLT_RANK_INFO);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, MGPT_RESP_GETBUDDYRANKINDEX);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerSkyAllShowInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_NOTIFY_SkyAllShowInfo msg;
	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_NOTIFY_SkyAllShowInfo解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return;
	}
	pkGamePlayer->GetArenaBattleChallengeInfoAsset()->UpdateDetailInfo(msg);
	pkGamePlayer->SetSkyArenaCommanderBaseData(msg);

	StringStreamType sStream("");
	sStream << msg.m_Left.ControllerBase.m_CharID << "#" << msg.m_Right.ControllerBase.m_CharID << "#" 
		<< msg.m_Left.m_nAllTeamData.m_vSchemeData.size() << "#" << msg.m_Time;

	LC_ClientGameEvent kGameEvent;
	kGameEvent.SetClientEventType(GameLogic::GameEventType(MGPT_SC_SKY_ALL_SHOW_INFO));
	LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(sStream.str());
	kGameEvent.SetUserData(&kUserData);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerSkyRoundShowInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_NOTIFY_SkyRoundShowInfo msg;
	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_NOTIFY_SkyRoundShowInfo解析错误");
		return;
	}

	StringStreamType sStream("");
	sStream << msg.nRound << "%";
	sStream << msg.m_Time << "%";
	sStream << msg.m_Left.m_nSchemeGroupID << "%";
	for(int i = 0; i < msg.m_Left.m_nBuddyData.size(); ++i)
	{
		PS_Buddy_Info_BuddyCS& kInfo = msg.m_Left.m_nBuddyData[i];
		sStream << kInfo.m_nBuddyID << "#" << kInfo.m_nBuddyLevel << "#" << kInfo.m_nBuddyStarLevel << "#" << kInfo.m_nBuddyTransformID << "#";
		sStream << kInfo.m_nBuddyCombatScore << "#" << kInfo.m_nAllEquipSlotReinforceLevel << "#" << kInfo.m_bActivateAllSkill << "##" ;
		std::map<int32_t,PS_EquipItemInfo_BuddyCS>::iterator itor = kInfo.m_nEquipSlotInfo.begin();
		for(; itor!=kInfo.m_nEquipSlotInfo.end(); ++itor)
			sStream << itor->first << ":" << itor->second.m_nEquipItemID << ":" << itor->second.m_nEquipSlotStarLevel << ":" << itor->second.m_nEquipSlotLevel << ";";
		sStream << "#";
		std::map<int32_t,PS_RuneItemInfo_BuddyCS>::iterator iter = kInfo.m_nBuddyRuneInfo.begin();
		for(; iter!=kInfo.m_nBuddyRuneInfo.end(); ++iter)
			sStream << iter->first << ":" << iter->second.m_nRuneItemID << ":" << iter->second.m_nRuneStarLevel << ":" << iter->second.m_nRuneLevel << ";" ;
		sStream << "#";
		sStream << "|";
	}
	sStream << "%";
	sStream << msg.m_Right.m_nSchemeGroupID << "%";
	for(int i = 0; i < msg.m_Right.m_nBuddyData.size(); ++i)
	{
		PS_Buddy_Info_BuddyCS& kInfo = msg.m_Right.m_nBuddyData[i];
		sStream << kInfo.m_nBuddyID << "#" << kInfo.m_nBuddyLevel << "#" << kInfo.m_nBuddyStarLevel << "#" << kInfo.m_nBuddyTransformID << "#";
		sStream << kInfo.m_nBuddyCombatScore << "#" << kInfo.m_nAllEquipSlotReinforceLevel << "#" << kInfo.m_bActivateAllSkill << "##" ;
		std::map<int32_t,PS_EquipItemInfo_BuddyCS>::iterator itor = kInfo.m_nEquipSlotInfo.begin();
		for(; itor!=kInfo.m_nEquipSlotInfo.end(); ++itor)
			sStream << itor->first << ":" << itor->second.m_nEquipItemID << ":" << itor->second.m_nEquipSlotStarLevel << ":" << itor->second.m_nEquipSlotLevel << ";";
		sStream << "#";
		std::map<int32_t,PS_RuneItemInfo_BuddyCS>::iterator iter = kInfo.m_nBuddyRuneInfo.begin();
		for(; iter!=kInfo.m_nBuddyRuneInfo.end(); ++iter)
			sStream << iter->first << ":" << iter->second.m_nRuneItemID << ":" << iter->second.m_nRuneStarLevel << ":" << iter->second.m_nRuneLevel << ";" ;
		sStream << "#";
		sStream << "|";
	}
	sStream << "%";
	for(int i = 0; i < msg.m_LeftDeadTeamIdVec.size(); ++i)
	{
		TeamShowInfo& kInfo = msg.m_LeftDeadTeamIdVec[i];
		sStream << kInfo.m_nTeamID << ";" << kInfo.m_nState << "|";
	}
	sStream << "%";
	for(int i = 0; i < msg.m_RightDeadTeamIdVec.size(); ++i)
	{
		TeamShowInfo& kInfo = msg.m_RightDeadTeamIdVec[i];
		sStream << kInfo.m_nTeamID << ";" << kInfo.m_nState << "|";
	}
	sStream << "%";

	LC_ClientGameEvent kGameEvent;
	kGameEvent.SetClientEventType(GameLogic::GameEventType(MGPT_SC_SKY_ROUND_SHOW_INFO));
	LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(sStream.str());
	kGameEvent.SetUserData(&kUserData);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerRespBattleInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RESP_BattleInfo rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	/*GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"竞技场  收到回复MGPT_RESP_BATTLE_INFO，战报m_battlesize=%d, m_topsize=%d, m_RankID=%d",rltMsg.m_battlesize, rltMsg.m_topsize, rltMsg.m_RankID);*/

	LC_ClientGameEvent kGameEvent;
	kGameEvent.SetClientEventType(CGET_ARENA_BATTLE_BATTLE_INFO);

	LC_ClientGameEventUserData_ArenaBattle_BattleInfo kUserData = LC_ClientGameEventUserData_ArenaBattle_BattleInfo(rltMsg);
	kGameEvent.SetUserData(&kUserData);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}
void HL_FromGameServer::_handlerRespAddChallengeTime(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RESP_AddChallengeTimes rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	ResultType iResult   = rltMsg.m_iResult;
	if(iResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(rltMsg.m_iResult);			
	}
}
void HL_FromGameServer::_handlerRespClearChallengeCD(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RESP_ClearChallengeCD rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	ResultType iResult   = rltMsg.m_iResult;
	if(iResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(rltMsg.m_iResult);		
		return;
	}

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_ARENA_BATTLE_CLEAR_CHALLENGE_CD);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, LOCAL_SERVER);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}
void HL_FromGameServer::_handlerRespGetChallengeCD(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RESP_GetChallengeCD rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	/*GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"竞技场  收到回复MGPT_RESP_GET_CHALLENGE_CD，m_CD=%d",rltMsg.m_CD);*/
	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_ARENA_BATTLE_GET_CHALLENGE_CD);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, LOCAL_SERVER);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, rltMsg.m_CD);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}
void HL_FromGameServer::_handlerRltDailyAward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RESP_Daily_Award rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	ResultType iResult   = rltMsg.m_iResult;
	if(iResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(rltMsg.m_iResult);			
	}
	else
	{
		StringStreamType sStream("");
		sStream << getRankFlagByID(rltMsg.m_RankID);
		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerMsgGameEvent(MGPT_RLT_DAILY_AWARD, sStream.str());
	}
}

void HL_FromGameServer::_handlerRltRankBaseInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_GG_RESP_Query_User_Base_Rank rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return;
	}
	pkGamePlayer->UpdateRankInfoData(rltMsg.m_UserRank);
	
	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_CLIENT_RLT_RANK_BASEINFO);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerRespRankDalySettleNotify(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_GG_Daily_Settle_Notify_Once rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_CLIENT_RANK_SETTLE_NOTIFY);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

inline bool SortItemByGrade(const int32_t &p1,int32_t &p2 )
{	
	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	LC_ItemDescriptionEntry *itemDescript1 = pkItemFactory->GetItemDescriptionEntry(p1);
	LC_ItemDescriptionEntry *itemDescript2 = pkItemFactory->GetItemDescriptionEntry(p2);
	
	if(itemDescript1->GetGrade() > itemDescript2->GetGrade())
		return true;

	return false;
}

void HL_FromGameServer::_handlerRespBasicProfitNotify(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_PB<char_data_define::common_stat_profit> notifyMsg;
	if (!notifyMsg.Decode(pMsg, wSize))
		return;
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return;
	}
	LC_CommonStatAsset& kCmnStatAsset = pkGamePlayer->GetCommonStatAsset();
	LC_CommonStatBasic* pkBasic = kCmnStatAsset.GetCommonStatBasic(LC_ST_TP_DAILY);
	if (NULL == pkBasic)
	{
		return;
	}

	pkBasic->GetDataFromPB(notifyMsg.m_value);
	

	StringStreamType sStream("");

	LC_ClientGameEvent kGameEvent;
	kGameEvent.SetClientEventType(CGET_DAILY_INCOME_NOTIFY);
	uint16_t index = 0;
	if (pkBasic->GetExpProfit() > 0)
	{
		sStream <<index<<":1:"<<pkBasic->GetExpProfit()<<";";
		index ++;
	}
	if (pkBasic->GetCashProfit())
	{
		sStream <<index<<":2:"<<pkBasic->GetCashProfit()<<";";
		index ++;
	}
	if (pkBasic->GetSPProfit())
	{
		sStream <<index<<":3:"<<pkBasic->GetSPProfit()<<";";
		index ++;
	}
	//按需求增加的Buddy经验，与上面的经验一样
	if (pkBasic->GetExpProfit() > 0)
	{
		sStream <<index<<":10:"<<pkBasic->GetExpProfit()<<";";
		index ++;
	}
	LC_CommonProfitStat::ItemInfoMap m_kItems = pkBasic->GetProrfitItemsInfo();

	vector<int32_t> vItems;
	LC_CommonProfitStat::ItemInfoMap::iterator it = m_kItems.begin();
	for (; it!=m_kItems.end(); ++it)
	{
		if (pkBasic->GetItemProfit(it->first,false) != 0 || pkBasic->GetItemProfit(it->first,true) !=0)
		{
			vItems.push_back(it->first);
		}
	}
	sort(vItems.begin(),vItems.end(),SortItemByGrade);


	for (vector<int32_t>::iterator it=vItems.begin();it != vItems.end();++it)
	{

		uint16_t bCount =pkBasic->GetItemProfit(*(it),true);
		uint16_t ubCount = pkBasic->GetItemProfit(*(it),false);

		uint16_t count = (bCount==0)?ubCount:bCount;
		uint16_t bBind = (bCount==0)?0:1;

		sStream <<index<<":"<<*(it)<<":"<<count<<":"<<bBind<<";";
		index ++;
	}

	LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(sStream.str());
	kGameEvent.SetUserData(&kUserData);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);

	vItems.clear();
}

//-------------------排行榜------------------------------------------------------------------------------

void HL_FromGameServer::_handleSyncVIPInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_SyncVIPInfo syncMsg;
	if(!syncMsg.Decode(pMsg,wSize))
		return;
	

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return;
	}

	pkGamePlayer->UpdateRotaryTableIsCharged(syncMsg.m_bTodayCharge);
	LC_VIPAsset& vipAsset = pkGamePlayer->GetVIPAsset();
	vipAsset.GetDataFromProtocolStruct(syncMsg);

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_CLIENT_VIPGIFT_UPDATE);
	UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handleNotifyVipLevelChange(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_Notify_VIPLevel_Change rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
		return;

	if(rltMsg.m_nCurrentVIPLevel > rltMsg.m_nOldVIPLevel)
	{
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_CLIENT_VIP_LEVELUP);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, rltMsg.m_nOldVIPLevel + 1);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, rltMsg.m_nCurrentVIPLevel);

		UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
}

void HL_FromGameServer::_handerRltBuyCourageItem( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_BuyCourageItem_Resp resqMsg;
	if (!resqMsg.Decode(pMsg, wSize))
		return ;
	SendLogicInfoMessage(resqMsg.m_Result);

	if (resqMsg.m_Result == RE_SUCCESS)
	{
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_CLIENT_COURAGE_BUY);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, resqMsg.m_ItemId);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, resqMsg.m_Count);

		UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
}

void HL_FromGameServer::_handlerRespGetPlayerSchemeData(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RESP_GetPlayerSchemeData  msg;
	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_RESP_GetPlayerSchemeData解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

	pkGamePlayer->GetArenaBattleChallengeInfoAsset()->UpdateDetailInfo(msg);

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_RESP_SCHEME_UPDATE);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_nSystemType);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, msg.m_nSchemeID);
	UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);

}

void HL_FromGameServer::_handlerRespGetRankTypeInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
 	MG_RESP_GetRankTypeInfo    msg;
	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_RESP_GetGetRankTypeInfo解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_RESP_RANKINFO_UPDATE);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1,msg.m_nRankIndex);	//名次
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2,msg.m_nValue);		//积分
	UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);

}

void HL_FromGameServer::_handlerResp_GetSummitSingleData(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RESP_GetSummitSingleData  msg;
	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_RESP_GetSummitSingleData解析错误");
		return;
	}

	ResultType nResult   = msg.m_nResult;
	if(nResult != RE_SUCCESS)
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"竞猜错误码：" + msg.m_nResult);
		SendLogicInfoMessage(msg.m_nResult);		
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	LC_ClientGuessDataBase*  GuessBaseData = pkGamePlayer->GetGuessBaseData();
	if (GuessBaseData->GetAgainstID() != 0)	//如果不为空
	{
		std::vector<LC_ClientGuessDataBase>* GuessDataLog = pkGamePlayer->GetGuessLogData();
		GuessDataLog->push_back(*GuessBaseData);
	}
	pkGamePlayer->GetArenaBattleChallengeInfoAsset()->UpdateDetailInfo(msg);
	GuessBaseData->SetGuessBaseData(msg.m_nBattleInfo);
	pkGamePlayer->SetGuessGroupID(msg.m_nBattleInfo.m_nGuessInfo.m_nGroupID); 
	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_RESP_GETSUMMITSINGLEDATA);
	UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerResp_SetSummitGuessInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RESP_SetSummitGuessInfo  msg;
	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_RESP_SetSummitGuessInfo解析错误");
		return; 
	}

	ResultType nResult   = msg.m_nResult;
	if(nResult != RE_SUCCESS)
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"竞猜错误码：" + msg.m_nResult);
		SendLogicInfoMessage(msg.m_nResult);		
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	LC_ClientGuessDataBase* GuessBaseData = pkGamePlayer->GetGuessBaseData();

	GuessBaseData->SetChoseCharID(msg.m_nChoseCharID);
	std::vector<KeyValue_BaseUint64>& GuessSupportInfo =  msg.m_nGuessSupportInfo;
	StringStreamType sStream("");
	sStream<<GuessSupportInfo[0].key<<"|"<<GuessSupportInfo[0].value<<"|"<<GuessSupportInfo[1].key<<"|"<<GuessSupportInfo[1].value<<"|";
	
	LC_ClientGameEvent kGameEvent;
	LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(sStream.str());
	kGameEvent.SetUserData(&kUserData);

	kGameEvent.SetClientEventType(GLET_RESP_SETSUMMITGUESSINFO);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void  HL_FromGameServer::_handlerResp_GetSummitSupportInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RESP_GetSummitSupportInfo msg;
	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_RESP_GetSummitSupportInfo解析错误");
		return;
	}
	std::vector<KeyValue_BaseUint64>& GuessSupportInfo =  msg.m_nGuessSupportInfo;
	StringStreamType sStream("");
	sStream<<GuessSupportInfo[0].key<<"|"<<GuessSupportInfo[0].value<<"|"<<GuessSupportInfo[1].key<<"|"<<GuessSupportInfo[1].value<<"|";

	LC_ClientGameEvent kGameEvent;
	LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(sStream.str());
	kGameEvent.SetUserData(&kUserData);

	kGameEvent.SetClientEventType(GLET_RESP_GETSUMMITSUPPROTINFO);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerResp_GetSummitWiner(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RESP_GetSummitWiner msg;
	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_RESP_GetSummitWiner解析错误");
		return;
	}

	ResultType nResult = msg.m_nResult;
	if(nResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_nResult);		
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	LC_ClientGuessDataBase* GuessData = pkGamePlayer->GetGuessBaseData();
	PS_GuessInfo_SummitArena& GuessInfo = msg.m_nGuessInfo;
	
	if (GuessInfo.m_nChoseCharID == 0)
	{
		return;
	}
	if (GuessData->m_nAgainstID != 0)
	{
		GuessData->m_bGetWiner = msg.m_nGuessInfo.m_bGetWiner;
		GuessData->m_nWinerCharID = msg.m_nGuessInfo.m_nWinerCharID;
		std::vector<LC_ClientGuessDataBase>* GuessDataLog = pkGamePlayer->GetGuessLogData();
		GuessDataLog->push_back(*GuessData);
		GuessData->Reset();
	}
	else
	{
		int maxlog = pkGamePlayer->GetLogMaxIndex();
		if (maxlog >= 0 && maxlog < pkGamePlayer->GetLogSize())
		{
			LC_ClientGuessDataBase* GuessLog = pkGamePlayer->GetGuessLogDataByIndex(maxlog);
			if(GuessLog != NULL)
			{
				GuessLog->m_bGetWiner = msg.m_nGuessInfo.m_bGetWiner;
				GuessLog->m_nWinerCharID = msg.m_nGuessInfo.m_nWinerCharID;
			}
		}
	}

	StringStreamType sStream("");
	sStream<<GuessInfo.m_nSummitType<<"|"<<GuessInfo.m_nAgainstID<<"|"<<GuessInfo.m_nChoseCharID<<"|"<<GuessInfo.m_nWinerCharID<<"|";

	LC_ClientGameEvent kGameEvent;

	LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(sStream.str());
	kGameEvent.SetUserData(&kUserData);

	kGameEvent.SetClientEventType(GLET_RESP_GETSUMMITGUESSWINER);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer:: _handlerSyncSummitData(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncSummitData msg;
	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MGPT_SYNC_SUMMITDATA解析错误");
		return;
	}


	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	LC_ClientGuessDataBase* GuessData = pkGamePlayer->GetGuessBaseData();
	std::vector<LC_ClientGuessDataBase>* GuessDataLog = pkGamePlayer->GetGuessLogData();
	//GuessDataLog->clear();
	GuessDataLog->swap(std::vector<LC_ClientGuessDataBase>());
	GuessData->Reset();

	if(msg.m_nBattleInfo.size() != 0)
	{
		PS_GuessInfo_SummitInfo& SummitInfo = msg.m_nBattleInfo.back();
		
		if (SummitInfo.m_nGuessInfo.m_nSummitType == pkGamePlayer->GetStageType() && !SummitInfo.m_nGuessInfo.m_bGetWiner)
		{ 
			GuessData->SetGuessBaseData(SummitInfo);
			pkGamePlayer->SetGuessGroupID(SummitInfo.m_nGuessInfo.m_nGroupID);
			int count = msg.m_nBattleInfo.size() - 1;
			for (int i = 0;i < count ; i++)
			{
				LC_ClientGuessDataBase GuessBaseData;
				GuessBaseData.SetGuessBaseData(msg.m_nBattleInfo[i]);
				GuessDataLog->push_back(GuessBaseData);
			}

		}
		else
		{
			int count = msg.m_nBattleInfo.size();
			for (int i = 0;i < count ; i++)
			{
				LC_ClientGuessDataBase GuessBaseData;
				GuessBaseData.SetGuessBaseData(msg.m_nBattleInfo[i]);
				GuessDataLog->push_back(GuessBaseData);
			}
		}
	}
	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_SYNC_SUMMITDATA);
	UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer:: _handlerRespSummitArenaHonor(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_Resp_SummitArenaHonorMsg msg;
	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MGPT_SC_SUMMIT_ARENA_HONOR_RESP解析错误");
		return;
	}

	LC_ClientGameEvent kGameEvent;
	ResultType iResult = msg.m_Result;
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(iResult == MGPT_SC_SUMMIT_ARENA_HONOR_RESP)
	{
		pkGamePlayer->SetTopErr(TOPRANKINGERROR_ERROR_ZONE);
		return;
	}

	if(iResult != RE_SUCCESS && iResult != RE_SUMMITARENA_HONORLIST_NODATA)
	{
		SendLogicInfoMessage(msg.m_Result);		
		return;
	}
	StringStreamType sStream("");
	sStream<<msg.nIndex<<"|"<<msg.m_nTotalIndex<<"|"<<msg.m_nTotalZoneId<<"|"<<msg.m_honorMsg.nStartTime<<"|"<<msg.m_honorMsg.nEndTime<<"|"<<msg.m_honorMsg.nChampionId<<"|";
	LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(sStream.str());
	kGameEvent.SetUserData(&kUserData);
	
	pkGamePlayer->SetPlayerList(msg.m_honorMsg.nPlayerList);

	kGameEvent.SetClientEventType(GLET_RESP_SUMMITARENAHONOR);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer:: _handlerRespSummitArenaMyTopHonor(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_Resp_SummitArenaMyTopHonorMsg msg;
	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MGPT_SC_SUMMITARENA_MY_HONORTOPID_RESP解析错误");
		return;
	}
	ResultType iResult = msg.m_Result;
	if(iResult == MGPT_SC_SUMMIT_ARENA_HONOR_RESP)
	{
		return;
	}

	if(iResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_Result);		
		return;
	}
	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_RESP_SUMMITARENAMYTOPHONOR);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1,msg.nHonorTopId);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer:: _handlerMgptSummitArenGuessClose(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_RESP_SUMMITARENAMYTOPHONOR);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerRltGetGuildDamageRank(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_GetGuildDamageRank  msg;
	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MGPT_RLT_GETGUILDDAMAGERANK解析错误");
		return;
	}
	ResultType iResult = msg.m_nResult;
	if(iResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_nResult);		
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->SetGuildDamageRank(msg.m_nGuildInfo);

	LC_ClientGameEvent kGameEvent;

	StringStreamType sStream("");
	sStream<<msg.m_nMyselfGuildInfo.m_nRankIndex<<"|"<<msg.m_nMyselfGuildInfo.m_nGuildName<<"|"<<msg.m_nMyselfGuildInfo.m_nGuildID<<"|"<<msg.m_nMyselfGuildInfo.m_nDamage<<"|"<<msg.m_nTotalCount;
	LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(sStream.str());
	kGameEvent.SetUserData(&kUserData);
	kGameEvent.SetClientEventType(GLET_MGPT_GETGUILDDAMAGERANK);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerRespDamageRankInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RESP_GetDamageRankInfo  msg;
	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_RESP_GetDamageRankInfo解析错误");
		return;
	}
	ResultType iResult = msg.m_nResult;
	if(iResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_nResult);		
		return;
	}

	StringStreamType sStream("");
	sStream << msg.m_nMapResID<< "#" << msg.m_nSelfTotalDamage << "#" << msg.m_nSelfGuildTotalDamage << "#" << msg.m_nSelfDamageIndex << "#" << msg.m_nSelfGuildRankIndex;

	LC_ClientGameEvent kGameEvent;
	LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(sStream.str());
	kGameEvent.SetUserData(&kUserData);
	kGameEvent.SetClientEventType(GLET_MGPT_GUILDBOSSBEFALL_DEMAGERANKINFO);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerRespGuildDamageRankInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RESP_GetGuildDamageRankInfo  msg;
	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_RESP_GetGuildDamageRankInfo解析错误");
		return;
	}
	ResultType iResult = msg.m_nResult;
	if(iResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_nResult);		
		return;
	}

	StringStreamType sStream("");
	sStream << msg.m_nMapResID << "#";
	PS_PlayerInfo& selfinfo = msg.m_nSelfInfo;
	sStream << selfinfo.m_nRankIndex << "|" << selfinfo.m_nCharName << "|" << selfinfo.m_nValue << "#";
	for (int i = 0; i < msg.m_nPlayerInfo.size(); ++i)
	{
		PS_PlayerInfo& info = msg.m_nPlayerInfo[i];
		sStream << info.m_nRankIndex << ";" << info.m_nCharName << ";" << info.m_nValue << "|";
	}
	sStream << "#";

	LC_ClientGameEvent kGameEvent;
	LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(sStream.str());
	kGameEvent.SetUserData(&kUserData);
	kGameEvent.SetClientEventType(GLET_MGPT_GUILDBOSSBEFALL_GUILDRANKINFO);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}
