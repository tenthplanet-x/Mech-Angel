#include "LC_ClientShopCity.h"

#include "CF_ItemList.h"
#include "ClientConfigManager.h"
#include "TM_ClientTimeManager.h"

#include "LC_ClientNPCManager.h"
#include "LC_ClientActivityManager.h"
#include "LC_ClientAuctionManager.h"



//----------------------------------------------------------------------------
void HL_FromGameServer::_handlerSignaturePass(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	//fyz temp
	//请求登录游戏
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkSessionGroup			= pkSessionManager->GetSessionGroup(pkSession);
	if(!pkSessionGroup)
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"无法找到SessionGroup");
		return;
	}

	MG_GC_REQ_Login		reqMsg;
	reqMsg.m_iCharID	= pkSessionGroup->GetSelectChar();
	reqMsg.m_fClientTime = GET_PROCESS_TIME;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_GC_REQ_LOGIN, &reqMsg);
	if (bCreateMsg)
		pkSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	
	return;
}
//----------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltCreateChar(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_LC_Rlt_CreateChar CreateCharMsg;
	if (!CreateCharMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_LC_Rlt_CreateChar解析错误");
		return;
	}

	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);

	DWORD dwUserID = pkClientSessionGroup->m_dwUserID;
	if (CreateCharMsg.m_dwUserID == dwUserID && CreateCharMsg.m_iResult == RE_LOGIN_CREATE_CHAR_SUCCESS)
	{
		pkClientSessionGroup->SetNetState(NLS_NONE);

		//返回角色列表选择界面
		pkClientSessionGroup->m_pkGameStageMachine->SetNextState(ST_GAME_STAGE_SEL_CHARACTER);
		pkClientSessionGroup->m_pkGameStageMachine->Update(0.0f,0.0f);
	}
	else if (CreateCharMsg.m_dwUserID == dwUserID && CreateCharMsg.m_iResult == RE_LOGIN_NAME_ALREADY_USED)
	{
		pkClientSessionGroup->SetMessage(RE_LOGIN_NAME_ALREADY_USED);
		pkClientSessionGroup->SetNetState(NLS_FAIL_CREATE_CHAR);
	}
	else
	{
		pkClientSessionGroup->SetMessage(RE_LOGIN_CREATE_CHAR_FAIL);
		pkClientSessionGroup->SetNetState(NLS_FAIL_CREATE_CHAR);
	}

}
//----------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltDeleteChar(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_LC_Rlt_DeleteChar DeleteCharMsg;
	if(!DeleteCharMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"请求删除角色协议解析错误");
		return;
	}

	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);

	DWORD dwUserID = pkClientSessionGroup->m_dwUserID;
	if(DeleteCharMsg.m_dwUserID != dwUserID || DeleteCharMsg.m_iResult != RE_LOGIN_DELETE_CHAR_SUCCESS)
	{
		//提示删除角色失败
		pkClientSessionGroup->SetNetState(NLS_FAIL_DELETE_CHAR);

		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"请求删除角色失败");
		return;
	}
	else
	{
		pkClientSessionGroup->SetNetState(NLS_NONE);

		//提示删除角色成功
		SendLogicInfoMessage(DeleteCharMsg.m_iResult);

		//在本地暂时删除角色
		int iDeleteTime = DeleteCharMsg.m_iDeleteTime;
		pkClientSessionGroup->DeleteCharacter(DeleteCharMsg.m_iCharID, iDeleteTime);

		//重新请求角色列表
		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		Protocol::MG_GC_REQ_CharList kMsgReq;
		kMsgReq.m_dwUserID = pkClientSessionGroup->m_dwUserID;
		kMsgReq.m_dwServerID = pkClientSessionGroup->m_iCurCitizenShip;
		bool bCreateMsg = CreateMsgPacketHandler(pkClientSessionGroup, CRYPT_ALGORITHM_NONE, hMsgHandler, MGPT_LC_REQ_CHAR_LIST, &kMsgReq);
		if (bCreateMsg)
			pkSession->Send( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );

	}

}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltLogin(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	commitPatchProgress(EVENT_CLIENT_GC_RLT_LOGIN, 0);

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_GC_RLT_Login rltMsg;
	if (rltMsg.Decode(pMsg, wSize))
	{
		NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
		NW_ClientSessionGroup*	 pkSessionGroup			= pkSessionManager->GetSessionGroup(pkSession);

		if (rltMsg.m_lResult != RE_SUCCESS)
		{
			GfxWriteLog(LOG_TAG_ERROR, LOG_SWITCH_ERROR, "[网络][收]Game Server 验证失败，不允许登陆, Code=%d", rltMsg.m_lResult);
			
			if(rltMsg.m_lResult == RE_LOGIN_SERVER_BUSY)
				SendMessageDialog(RE_LOGIN_SERVER_BUSY,0,0,0,RE_LOGIN_SERVER_BUSY,0,0,0,0,0);
			else
				SendLogicInfoMessage(rltMsg.m_lResult);

			pkSessionGroup->SetNetState(NLS_NONE);
	
			return;
		}

		//MG_GC_REQ_EnterGame kMsg;
		//kMsg.m_iCharID          = rltMsg.m_iCharID;
		//bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_GC_REQ_ENTER_GAME, &kMsg);
		//if(bCreateMsg)
		//	pkSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

		//正在请求进入游戏
		//pkSessionGroup->m_pkGameStageMachine->SetChangeServerFlag(false);
		pkSessionGroup->SetNetState(NLS_WAIT_ENTER_GAME);
	}
}
//----------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltRecoverChar(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_LC_Rlt_RecoverChar RecoverCharMsg;
	if (!RecoverCharMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RecoverCharMsg解析错误");
		return;
	}

	NW_ClientSessionManager* pkSessionManager = NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);

	DWORD dwUserID = pkClientSessionGroup->m_dwUserID;
	if (RecoverCharMsg.m_dwUserID != dwUserID || RecoverCharMsg.m_iResult != RE_LOGIN_RECOVER_CHAR_SUCCESS)
	{
		//提示恢复角色出错
		pkClientSessionGroup->SetNetState(NLS_FAIL_RECOVER_CHAR);
		return;
	}
	else
	{
		//提示恢复角色成功
		pkClientSessionGroup->SetNetState(NLS_NONE);
		SendLogicInfoMessage(RecoverCharMsg.m_iResult);
		pkClientSessionGroup->RecoverCharacter(RecoverCharMsg.m_iCharID);

		//重新请求角色列表
		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		Protocol::MG_GC_REQ_CharList kMsgReq;
		kMsgReq.m_dwUserID = pkClientSessionGroup->m_dwUserID;
		kMsgReq.m_dwServerID = pkClientSessionGroup->m_iCurCitizenShip;
		bool bCreateMsg = CreateMsgPacketHandler(pkClientSessionGroup, CRYPT_ALGORITHM_NONE, hMsgHandler, MGPT_LC_REQ_CHAR_LIST, &kMsgReq);
		if (bCreateMsg)
			pkSession->Send( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}

}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltEnterGame(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
#pragma TODO("消息调整需要修改")

#pragma TODO("关于Session和StageMachine的关系需要重新设计")

	commitPatchProgress(EVENT_CLIENT_GC_RLT_ENTER_GAME, 0);
	MG_GC_RLT_EnterGame rltMsg;
	if (!rltMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_TAG_ERROR, LOG_SWITCH_ERROR, "[网络][收]MG_RLT_EnterGame解析错误");
        return;
	}
	if(rltMsg.m_lEnterGameStatus != 0)
    {
		GfxWriteLog(LOG_TAG_ERROR, LOG_SWITCH_ERROR, "[网络][收]MG_RLT_EnterGame状态错误, %d", rltMsg.m_lEnterGameStatus);
        return;
    }

	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	pkSessionManager->SetServerUnionID(rltMsg.m_lUnionID);
	NW_ClientSessionGroup* pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	
	pkClientSessionGroup->SetNetState(NLS_NONE);
	pkClientSessionGroup->SetCurGameServer(	rltMsg.m_lServerEntryID );
	pkClientSessionGroup->SetCurGameServerNode( rltMsg.m_lServerID );
	GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"收到地图分线信息，设置节点m_iServerID=%d",rltMsg.m_lServerID);
	pkClientSessionGroup->SetInGameFlag(true);
	pkClientSessionGroup->SetServerGameVersion(STDSTR2TPSTR(rltMsg.m_strGameVersion));
	pkClientSessionGroup->SetServerResVersion(STDSTR2TPSTR(rltMsg.m_strResVersion));

    user_id_type lUserID		= rltMsg.m_lUserID;
	unique_id_type lCharID		= rltMsg.m_lCharID;
    int iCharType		= rltMsg.m_lCharType;
	int iNewPlayerLevel = rltMsg.m_iNewPlayerLevel;


	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	pkLogicManager->Init();
	pkLogicManager->SetInputControlActorID(0, true);	// clear old

    LC_PlayerManagerBase* pkGamePlayerManager = CLIENT_GET_GAMEPLAYER_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkGamePlayerManager->FindPlayer(lCharID);
	if (pkGamePlayer == NULL)
	{
		object_id_type playerID = GET_PLAYERID(lUserID);
		pkGamePlayer = (LC_ClientGamePlayer*)pkGamePlayerManager->CreatePlayer(playerID, lCharID, iCharType);
//TODO: 添加NETPLAYER
	}

	pkGamePlayer->CreateNewBuddyInstance(iCharType);
	pkGamePlayer->SetAllOwner(iCharType, pkGamePlayer);
	pkGamePlayer->SetEnterGameTime();
	pkGamePlayer->SetUserID(lUserID);

	pkGamePlayer->GetCommander()->Reset();
	pkGamePlayer->GetCommander()->SetMainControllerID(pkGamePlayer->GetID());
	GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM,"[数据][收]MGPT_GC_RLT_ENTER_GAME GamePlayerID=%d",pkGamePlayer->GetID());

	pkGamePlayer->SetNewPlayerLevel(iNewPlayerLevel);
	if(!rltMsg.m_bSwitchLine)
		pkGamePlayer->InitLogicData();
	//pkGamePlayer->SetCharName(STDSTR2TPSTR(rltMsg.m_szCharName));	
	pkGamePlayer->SetCharName(pkGamePlayer->GetCharNameByID());
	pkGamePlayer->SetPlayerGroupIDType(0, E_PlayerTeamType_Local);
	pkGamePlayer->SetWeaponAttribute((WeaponType)rltMsg.m_lWeaponType);

	pkLogicManager->SetOperationPlayerID(pkGamePlayer->GetID());
	pkLogicManager->SetInputControlActorID(pkGamePlayer->GetID());
	pkLogicManager->SetOperationCharID(lCharID);

	pkGamePlayer->DestoryVisibleData();
    pkGamePlayer->CreateVisibleData();

 	pkSession->SetPlayer(pkGamePlayer);
	pkGamePlayer->SetSession(pkSession);
	pkGamePlayer->Respawn(1,1);

	pkGamePlayer->ResetLogicAreaInfo();

	//激活保存的技能状态
	pkGamePlayer->GetStateMap()->StartSavedSkillState();	

	//设置默认装备
	NW_ClientSessionGroup* pkGameSessionGroup = pkSessionManager->GetGameSessionGroup();

	//换装
	pkGamePlayer->HideBeforeSuitApplied();
	bool change = false;
	//change = pkGamePlayer->ChangeAvatar(0,pkGamePlayer->GetCurrentEquipItem(ET_HEAD));
	//if (change)		
	//	pkGamePlayer->SetLastEquiptID(pkGamePlayer->GetCurrentEquipItem(ET_HEAD), ET_HEAD);
	//change = pkGamePlayer->ChangeAvatar(0,pkGamePlayer->GetCurrentEquipItem(ET_BODY));
	//if (change)		
	//	pkGamePlayer->SetLastEquiptID( pkGamePlayer->GetCurrentEquipItem(ET_BODY),ET_BODY);
	//change = pkGamePlayer->ChangeAvatar(0,pkGamePlayer->GetCurrentEquipItem(ET_FOOT));
	//if (change)		
	//	pkGamePlayer->SetLastEquiptID(pkGamePlayer->GetCurrentEquipItem(ET_FOOT), ET_FOOT);

	pkGamePlayer->ShowAfterSuitApplied();

	pkLogicManager->GetOperationPlayer()->SetLastEquiptID(0,ET_WEAPON);
	pkGamePlayer->ChangeAvatar(0,pkGamePlayer->GetCurrentEquipItem(ET_WEAPON));
	pkGamePlayer->SetEquipWeapon(pkGamePlayer->GetCurrentEquipItem(ET_WEAPON));

	pkGamePlayer->DoUpdateWingAvatar();
	pkGamePlayer->DoUpdateBowAvatar();

	pkGamePlayer->DoUpdateAnqiAvatar();
	pkGamePlayer->DoUpdateFabaoAvatar();

	pkGamePlayer->DoUpdateWeaponAvatar();

	pkGamePlayer->SetLastOffLineTime(rltMsg.m_uiLastOffLineTime);

	pkClientSessionGroup->NotifyAntiWallowStateToGameServer();

	PS_GameSvrInfo *pkGameSvrInfo = pkSessionManager->GetGameServerInfo(pkGamePlayer->GetCitizenship());
	TM_ClientTimeManager* pkTimeMgr = (TM_ClientTimeManager*)UT_TimeManager::GetSingletonPtr();
	if (pkTimeMgr)
	{
		pkTimeMgr->SyncServerTime(rltMsg.m_uiTimestamp);
		if (!pkGameSvrInfo)
		{
			pkTimeMgr->SyncServerOpenTime(rltMsg.m_uiOpenTime);
		}
		else
		{
			pkTimeMgr->SyncServerOpenTime(pkGameSvrInfo->m_iServerOpenTime);
		}
		if (!pkGameSvrInfo)
		{
			pkTimeMgr->SyncServerConflateTime(rltMsg.m_uiConflateTime);
		}
		else
		{
			pkTimeMgr->SyncServerConflateTime(pkGameSvrInfo->m_iServerConflateTime);
		}
	}
	ClientCSVScheduleManager *pkCSVScheduleManager = (ClientCSVScheduleManager*)ClientCSVScheduleManager::GetSingletonPtr();
	if (pkCSVScheduleManager)
	{
		pkCSVScheduleManager->OnGameLoaded(GET_SERVER_OPEN_TIME);
	}

	LC_ClientActivityManager *pkClientActivityManager = (LC_ClientActivityManager*)LC_ClientActivityManager::GetSingletonPtr();
	if (pkClientActivityManager)
	{
		pkClientActivityManager->Schedule(GET_SERVER_OPEN_TIME, GET_SERVER_CONFLAT_TIME);
	}
	//ResVersion
	ClientConfigReloadManager *pkConfigReloadManager = (ClientConfigReloadManager*)ClientConfigReloadManager::GetSingletonPtr();
	if (pkConfigReloadManager)
	{
		GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM, "_handlerRltEnterGame, ResVersion=%s", rltMsg.m_strResVersion.c_str());
		pkConfigReloadManager->SetResVersion(STDSTR2TPSTR(rltMsg.m_strResVersion),TStringVector(),GET_SERVER_SYSTEM_TIME);
	}

	pkGamePlayer->SetIsSwitchLine(rltMsg.m_bSwitchLine);
	if (!rltMsg.m_bSwitchLine)
		pkGamePlayer->SetPostEnterGameFlag(true);

	pkClientSessionGroup->m_pkGameStageMachine->SetChangeServerFlag(false);

	LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
	if (pkNetPlayerManager)
		pkNetPlayerManager->DestroyAllPlayer(false);

	LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
	if (pkNPCManager )	
		pkNPCManager->DestroyAllNPC();

	LC_ClientAuctionManager::GetSingletonPtr()->LoadHistory(lCharID);

	GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM,"角色登陆游戏，登陆服务器ID为[%i]，当前服务器时间为[%lu]，当前服务器开服时间为[%lu]，当前服务器合服时间为[%lu], 角色最后一次离线时间为[%lu]",rltMsg.m_lServerID,rltMsg.m_uiTimestamp,rltMsg.m_uiOpenTime,rltMsg.m_uiConflateTime,rltMsg.m_uiLastOffLineTime);

	//进入游戏界面
	//pkClientSessionGroup->m_pkGameStageMachine->SetNextState(ST_GAME_STAGE_GAME);
	//pkClientSessionGroup->m_pkGameStageMachine->Update(0.0f,0.0f);
	
	
	UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
	if(pkUIManager)
	{
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_CLIENT_ON_ENTER_GAME);
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
	
}
//----------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltLeaveGame(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
#pragma TODO("流程错误")
	MG_RLT_LeaveGame rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "请求进入游戏协议解析错误");
		return;
	}
// 	pkSession->SetAutoReconnect(false);
// 	pkSession->Disconnect();

	/*NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	
	pkClientSessionGroup->m_pkLoginSession->TryToConnect();
	pkClientSessionGroup->m_pkGameStageMachine->SetNextState(ST_GAME_STAGE_SEL_CHARACTER);
	pkClientSessionGroup->m_pkGameStageMachine->Update(0.0f,0.0f);*/
}
//----------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltResetGame(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RLT_ResetGame rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_RLT_ResetGame解析错误");
		return;
	}

	NW_ClientSessionManager* pkSessionManager	= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup* pkSessionGroup		= pkSessionManager->GetSessionGroup(pkSession);
	if (NULL == pkSessionGroup)
		return;
// 	pkSession->SetAutoReconnect(false);
// 	pkSession->Disconnect();

	//pkSessionGroup->m_pkGameStageMachine->SetCloseGameFlag(false);
	//pkSessionGroup->m_pkGameStageMachine->SetNextState(ST_GAME_STAGE_QUIT);
}
//-------------------------------------------------------
void HL_FromGameServer::_handlerRltJumpGame( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RLT_Jumper jumpMsg;
	if(!jumpMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_RLT_Jumper解析错误");
		return;
	}

	if (RE_CHANGE_SERVER_SUCCESS != jumpMsg.m_iResult)
	{
		if(RE_CHANGE_SERVER_FAIL_NEED_CD == jumpMsg.m_iResult)
		{
			long lCDTime = jumpMsg.m_uiCDTime;
			long lCDHour = lCDTime / 60;
			long lCDMin  = lCDTime - lCDHour * 60;

			SendLogicInfoMessage(RE_CHANGE_SERVER_FAIL_NEED_CD,lCDHour,lCDMin);
		}

		if(RE_CHANGE_SERVER_FAIL_SAME_SERVER == jumpMsg.m_iResult)
		{
			SendLogicInfoMessage(RE_CHANGE_SERVER_FAIL_SAME_SERVER);
		}
		
		return;
	}
	

	//Sleep(2000);
	
	NW_ClientSessionManager* pkSessionManager	= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup* pkSessionGroup		= pkSessionManager->GetGameSessionGroup();
	if (NULL == pkSessionGroup)
		return;

	pkSessionGroup->m_pkGameStageMachine->SetCloseGameFlag(false);
	pkSessionGroup->m_pkGameStageMachine->SetChangeServerFlag(true);
	pkSessionGroup->m_pkGameStageMachine->SetNeedResetGameFlag(false);

	pkSessionGroup->m_pkGameStageMachine->SetNextState(ST_GAME_STAGE_QUIT);
}
//-------------------------------------------------------
void HL_FromGameServer::_handlerRltWaiting(NW_Game2ClientSession* pkSession,char* pMsg,uint16_t wSize)
{
	MG_RLT_Waiting_Info rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
		return;
	
	long lWaitingCount = rltMsg.m_waitingCount;
	uint8_t uWaitingType = rltMsg.m_waitingType;


	if(lWaitingCount > 0)
	{
		char cWaitTime[64];
		int iWaitTimeMin;

		iWaitTimeMin = Float2Long(lWaitingCount * 0.005);

		if(iWaitTimeMin < 60)
		{
			iWaitTimeMin = UT_MathBase::MaxInt(1,iWaitTimeMin);				
			sprintf(cWaitTime,"%d分钟",iWaitTimeMin);
		}
		else
		{
			int iWaitHour = UT_MathBase::MinInt(iWaitTimeMin / 60,3);
			sprintf(cWaitTime,"%d小时",iWaitHour); 
		}

		if(uWaitingType == 0)
			SendLogicInfoMessage(RE_WAIT_FOR_MOMENT,lWaitingCount,"新手",cWaitTime);
		else
			SendLogicInfoMessage(RE_WAIT_FOR_MOMENT,lWaitingCount,"玩家",cWaitTime);
	}
	else
	{
		SendLogicInfoMessage(RE_WAIT_FOR_MOMENT_0);
	}

	NW_ClientSessionManager* pkSessionManager = NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup* pkSessionGroup = pkSessionManager->GetGameSessionGroup();
	pkSessionGroup->SetNetState(NLS_WAIT_ENTER_GAME);
}
//-------------------------------------------------------------
