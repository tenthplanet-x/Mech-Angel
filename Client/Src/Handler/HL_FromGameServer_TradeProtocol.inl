
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerRequestTradeItem(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//�����δ������Ϸ�����˳�
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;


	MG_REQ_RequestTrade reqMsg;
	if(!reqMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "������Ʒ������Ϣ��������");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

	if(!pkGamePlayer)
		return;

	if (reqMsg.m_lTradeTargetCharID == pkGamePlayer->GetInstance())
	{
		unique_id_type lRequestTradeID = reqMsg.m_lTradeCharID;
		pkGamePlayer->GetTradeManager()->AddToRequestTradeList(lRequestTradeID, STDSTR2TPSTR(reqMsg.m_szCharName));

		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_RECEIVE_TRADE);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltTradeItemInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//�����δ������Ϸ�����˳�
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;


	MG_RLT_RequestTrade rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "������Ʒ���׻ظ���������");
		return;
	}

	//StringType strMessage = GFxSystemInfo(rltMsg.m_iReqResult);

	SendLogicInfoMessage(rltMsg.m_iReqResult);	
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerRelpyTradeItem(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//�����δ������Ϸ�����˳�
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;


	MG_RLT_ReplyTrade rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "��������𸴻ظ���Ϣ��������");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

	unique_id_type lTargetTradeID = INVALID_UNIQUE_ID;
	if (rltMsg.m_lReplyCharID == pkGamePlayer->GetInstance())
	{
		lTargetTradeID = rltMsg.m_lRequestCharID;
		if (rltMsg.m_iResult == RTS_REFUSE)
		{
			rltMsg.m_iResult = RE_ITEM_TRADE_REQEST_REFUSE_SUCCESS;
		}
	}
	else if (rltMsg.m_lRequestCharID == pkGamePlayer->GetInstance())
	{
		lTargetTradeID = rltMsg.m_lReplyCharID;
		if (rltMsg.m_iResult == RTS_REFUSE)
		{
			rltMsg.m_iResult = RE_ITEM_TRADE_REQEST_REFUSE;
		}
	}
	if (rltMsg.m_iResult == RE_ITEM_TRADE_START || rltMsg.m_iResult == RE_ITEM_TRADE_REQEST_REFUSE_SUCCESS)
	{
		StringType TargetName = "";
		LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
		LC_ClientNetPlayer*	pkNetPlayer = (LC_ClientNetPlayer*)(pkNetPlayerManager->FindPlayer(lTargetTradeID));
		if(pkNetPlayer)
		{
			TargetName = pkNetPlayer->GetCharName();
			pkGamePlayer->RltTrade(lTargetTradeID);
		}
		else
			TargetName = "�������";
		SendLogicInfoMessage(rltMsg.m_iResult,TargetName.c_str());
	}
	else
	{
		SendLogicInfoMessage(rltMsg.m_iResult);
	}
	
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerAddItemToTradePack(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//�����δ������Ϸ�����˳�
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;


	MG_RLT_AddItemToTradePack rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "�������������Ʒ��Ϣ��������");
		return;
	}

	StringType TargetName = "";

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

	if (rltMsg.m_lTradeCharID == pkGamePlayer->GetInstance())
	{
		TargetName = "��";
	}
	else if (rltMsg.m_lTradeTargetCharID == pkGamePlayer->GetInstance())
	{
		LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
		LC_ClientNetPlayer*	pkNetPlayer = (LC_ClientNetPlayer*)(pkNetPlayerManager->FindPlayer(rltMsg.m_lTradeCharID));
		if(pkNetPlayer)
		{
			TargetName = pkNetPlayer->GetCharName();
		}
		else
			TargetName = "���׶���";
	}

	if (rltMsg.m_iResult == RE_ITEM_TRADE_ADD_SUCCESS)
	{
		SendLogicInfoMessage(rltMsg.m_iResult,TargetName.c_str());
	}
	else
	{
		//ֻ��ʾ��������صĴ�����ʾ
		if (rltMsg.m_lTradeCharID == pkGamePlayer->GetInstance())
		{
			SendLogicInfoMessage(rltMsg.m_iResult);
		}
		
	}
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerRemoveItemFormTradePack(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//�����δ������Ϸ�����˳�
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;


	MG_RLT_RemoveItemFromTradePack rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "���������Ƴ���Ʒ��Ϣ��������");
		return;
	}

	StringType TargetName = "";

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

	if (rltMsg.m_lTradeCharID == pkGamePlayer->GetInstance())
	{
		TargetName = "��";
	}
	else if (rltMsg.m_lTradeTargetCharID == pkGamePlayer->GetInstance())
	{
		LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
		LC_ClientNetPlayer*	pkNetPlayer = (LC_ClientNetPlayer*)(pkNetPlayerManager->FindPlayer(rltMsg.m_lTradeCharID));
		if(pkNetPlayer)
		{
			TargetName = pkNetPlayer->GetCharName();
		}
		else
			TargetName = "���׶���";
	}
	if (rltMsg.m_iResult == RE_ITEM_TRADE_REMOVE_SUCCESS)
	{
		SendLogicInfoMessage(rltMsg.m_iResult,TargetName.c_str());
	}
	else
	{
		//ֻ��ʾ��������صĴ�����ʾ
		if (rltMsg.m_lTradeCharID == pkGamePlayer->GetInstance())
		{
			SendLogicInfoMessage(rltMsg.m_iResult);
		}

	}
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerUpdateTradeCash(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//�����δ������Ϸ�����˳�
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;


	MG_RLT_UpdateTradeCash rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "����������½����ֽ���Ϣ��������");
		return;
	}

	StringType TargetName = "";

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

	if (rltMsg.m_lTradeCharID == pkGamePlayer->GetInstance())
	{
		TargetName = "��";
	}
	else if (rltMsg.m_lTradeTargetCharID == pkGamePlayer->GetInstance())
	{
		LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
		LC_ClientNetPlayer*	pkNetPlayer = (LC_ClientNetPlayer*)(pkNetPlayerManager->FindPlayer(rltMsg.m_lTradeCharID));
		if(pkNetPlayer)
		{
			TargetName = pkNetPlayer->GetCharName();
		}
		else
			TargetName = "���׶���";
	}
	else
	{
		return;
	}

	if (rltMsg.m_iResult == RE_ITEM_TRADE_UPDATE_CASH_SUCCESS)
	{
		SendLogicInfoMessage(rltMsg.m_iResult,TargetName.c_str());
	}
	else
	{
		//ֻ��ʾ��������صĴ�����ʾ
		if (rltMsg.m_lTradeCharID == pkGamePlayer->GetInstance())
		{
			SendLogicInfoMessage(rltMsg.m_iResult);
		}
	}
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerLockTrade(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//�����δ������Ϸ�����˳�
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;


	MG_RLT_LockTrade rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "������������������Ϣ��������");
		return;
	}

	StringType TargetName = "";

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

	if (rltMsg.m_lTradeCharID == pkGamePlayer->GetInstance())
	{
		TargetName = "��";
	}
	else if (rltMsg.m_lTradeTargetCharID == pkGamePlayer->GetInstance())
	{
		LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
		LC_ClientNetPlayer*	pkNetPlayer = (LC_ClientNetPlayer*)(pkNetPlayerManager->FindPlayer(rltMsg.m_lTradeCharID));
		if(pkNetPlayer)
		{
			TargetName = pkNetPlayer->GetCharName();
		}
		else
			TargetName = "���׶���";
	}
	else
	{
		return;
	}

	if (rltMsg.m_iResult == RE_ITEM_TRADE_LOCK_SUCCESS)
	{
		SendLogicInfoMessage(rltMsg.m_iResult,TargetName.c_str());
	}
	else
	{
		//ֻ��ʾ��������صĴ�����ʾ
		if (rltMsg.m_lTradeCharID == pkGamePlayer->GetInstance())
		{
			SendLogicInfoMessage(rltMsg.m_iResult);
		}
	}
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerUnLockTrade(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//�����δ������Ϸ�����˳�
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;


	MG_RLT_UnLockTrade rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "��������ȡ������������Ϣ��������");
		return;
	}

	StringType TargetName = "";

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

	unique_id_type lTargetID = 0;
	if (rltMsg.m_lTradeCharID == pkGamePlayer->GetInstance())
	{
		TargetName = "��";
		lTargetID = rltMsg.m_lTradeTargetCharID;
	}
	else if (rltMsg.m_lTradeTargetCharID == pkGamePlayer->GetInstance())
	{
		lTargetID = rltMsg.m_lTradeCharID;
		LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
		LC_ClientNetPlayer*	pkNetPlayer = (LC_ClientNetPlayer*)(pkNetPlayerManager->FindPlayer(rltMsg.m_lTradeCharID));
		if(pkNetPlayer)
		{
			TargetName = pkNetPlayer->GetCharName();
		}
		else
			TargetName = "���׶���";
	}
	else
	{
		return;
	}

	if (rltMsg.m_iResult == RE_ITEM_TRADE_UNLOCKED_SUCCESS)
	{
		LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
		LC_ClientNetPlayer*	pkNetPlayer = (LC_ClientNetPlayer*)(pkNetPlayerManager->FindPlayer(lTargetID));
		if(pkNetPlayer)
		{
			pkNetPlayer->GetTradeManager()->SetTradeState(PTS_HAVE_TRADE);
		}

		SendLogicInfoMessage(rltMsg.m_iResult,TargetName.c_str());
	}
	else
	{
		//ֻ��ʾ��������صĴ�����ʾ
		if (rltMsg.m_lTradeCharID == pkGamePlayer->GetInstance()
			|| rltMsg.m_iResult != RE_ITEM_TRADE_TARGET_ERROR)
		{
			SendLogicInfoMessage(rltMsg.m_iResult);
		}
	}
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerInterruptTrade(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//�����δ������Ϸ�����˳�
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;


	MG_RLT_InterruptTrade rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "���������жϽ�����Ϣ��������");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

	if (rltMsg.m_lTradeCharID == pkGamePlayer->GetInstance())
	{
		SendLogicInfoMessage(rltMsg.m_iResult);
	}
	else if (rltMsg.m_lTradeTargetCharID == pkGamePlayer->GetInstance())
	{
		SendLogicInfoMessage(rltMsg.m_iResult);
	}
	else
	{
		return;
	}
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerFinishTrade(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//�����δ������Ϸ�����˳�
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;


	MG_RLT_FinishTrade rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "����������ɽ�����Ϣ��������");
		return;
	}

	StringType TargetName = "";
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

	if (rltMsg.m_lTradeCharID == pkGamePlayer->GetInstance())
	{
		TargetName = "��";
	}
	else if (rltMsg.m_lTradeTargetCharID == pkGamePlayer->GetInstance())
	{
		if (rltMsg.m_iResult == RE_ITEM_TRADE_FINISH_SUCCESS && IS_PLAYER_ID(rltMsg.m_lTradeCharID))
		{
			LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
			LC_ClientNetPlayer*	pkNetPlayer = (LC_ClientNetPlayer*)(pkNetPlayerManager->FindPlayer(rltMsg.m_lTradeCharID));
			if(pkNetPlayer)
			{
				pkNetPlayer->GetTradeManager()->SetTradeState(PTS_FINISH_TRADE);
			}
		}

		LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
		LC_ClientNetPlayer*	pkNetPlayer = (LC_ClientNetPlayer*)(pkNetPlayerManager->FindPlayer(rltMsg.m_lTradeCharID));
		if(pkNetPlayer)
		{
			TargetName = pkNetPlayer->GetCharName();
		}
		else
			TargetName = "���׶���";
	}
	else
	{
		return;
	}

	if (rltMsg.m_iResult == RE_ITEM_TRADE_FINISH_SUCCESS)
	{
		SendLogicInfoMessage(rltMsg.m_iResult,TargetName.c_str());
	}
	else if (rltMsg.m_iResult == RE_ITEM_TRADE_SUCCESS)
	{
		SendLogicInfoMessage(rltMsg.m_iResult);
	}
	else
	{
		unique_id_type lTargetID = 0;
		//ֻ��ʾ��������صĴ�����ʾ
		if (rltMsg.m_lTradeCharID == pkGamePlayer->GetInstance())
		{
			SendLogicInfoMessage(rltMsg.m_iResult);
			lTargetID = rltMsg.m_lTradeTargetCharID;
		}

		if (rltMsg.m_lTradeTargetCharID == pkGamePlayer->GetInstance())
		{
			if (rltMsg.m_iResult == RE_ITEM_TRADE_PLAYER_NO_SPACE)
			{
				rltMsg.m_iResult = RE_ITEM_TRADE_TARGET_NO_SPACE;
			}
			else if (rltMsg.m_iResult == RE_ITEM_TRADE_TARGET_NO_SPACE)
				rltMsg.m_iResult = RE_ITEM_TRADE_PLAYER_NO_SPACE;

			SendLogicInfoMessage(rltMsg.m_iResult);
			lTargetID = rltMsg.m_lTradeCharID;
		}

		if (rltMsg.m_iResult == RE_ITEM_TRADE_PLAYER_NO_SPACE||
			rltMsg.m_iResult == RE_ITEM_TRADE_CASH_NOT_ENOUGH||
			rltMsg.m_iResult == RE_ITEM_TRADE_TARGET_NO_SPACE||
			rltMsg.m_iResult == RE_ITEM_TRADE_LARGER_MAX_CASH)
		{
			LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
			LC_ClientNetPlayer*	pkNetPlayer = (LC_ClientNetPlayer*)(pkNetPlayerManager->FindPlayer(lTargetID));
			if(pkNetPlayer)
			{
				pkNetPlayer->GetTradeManager()->SetTradeState(PTS_HAVE_TRADE);
			}
		}
	}
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncTradePack(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//�����δ������Ϸ�����˳�
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;


#pragma TODO("��Ϣ������Ҫ�޸�")
 	MG_SyncTradePack msg;
 	if(msg.Decode(pMsg,wSize))
 	{
 		object_id_type lPlayerID		= msg.m_lPlayerID;
 		unique_id_type lTradeTarget   = msg.m_lTradeTarget;
 		GameLogic::PlayerTradeState eTradeState = (GameLogic::PlayerTradeState)msg.m_iPlayerTradeState;
 
 		LC_PlayerBase* pkPlayer = pkSession->GetPlayer();
 		if(!pkPlayer)
 			return;
 
 		if(pkPlayer->GetSelfID() != lPlayerID)
 		{
//ToCheck Ŀǰ�Ȳ������� ���濴��Ҫ��ô��
			LC_ClientNetPlayerManager* clientNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
			if(clientNetPlayerManager->FindPlayer(lPlayerID) != NULL)
			{
				GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_ERROR,"����:ͬ�������߼�����, ����������netplayerID,��������");
				return;
			}
			else
			{
				GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"����:ͬ�������߼�����");
				return;
			}
 		}

		LC_TradeManager *pkTradeManager = pkPlayer->GetTradeManager();
		if (NULL == pkTradeManager)
		{
			return;
		}
//ToCheck ����ܿ��������� Ŀǰbuddy IDΪclone playerID �ܿ��ܻ������
 		if (IS_PLAYER_ID(pkTradeManager->GetTradeTargetID()) && eTradeState == PTS_NO_TRADE)
 		{
 			LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
 			LC_ClientNetPlayer*	pkNetPlayer = (LC_ClientNetPlayer*)(pkNetPlayerManager->FindPlayer(pkTradeManager->GetTradeTargetID()));
 			if(pkNetPlayer)
 			{
 				pkNetPlayer->GetTradeManager()->SetTradeState(PTS_NO_TRADE);
 			}
 		}
 
 		pkTradeManager->SetTradeTargetID(lTradeTarget);
 		pkTradeManager->SetTradeState(eTradeState);
 		
 		TradePackData* pkTradePackData = pkTradeManager->GetTradePackData();
		pkTradePackData->m_bMyTradeLock = msg.m_bMyTradeLock;
		pkTradePackData->m_bTradeTargeTradeLock = msg.m_bTradeTargeTradeLock;
		pkTradePackData->m_kMyTradeCash.clear();
		pkTradePackData->m_kMyTradeCash.insert(msg.m_kMyTradeCash.begin(), msg.m_kMyTradeCash.end());
		pkTradePackData->m_kTradeTargetCash.clear();
		pkTradePackData->m_kTradeTargetCash.insert(msg.m_kTradeTargetCash.begin(), msg.m_kTradeTargetCash.end());
		for (int i = 0;i < MAX_TRADE_PACK_SIZE;i++)
		{
			pkTradePackData->m_akMyTradePackage[i].m_iBackPackIndex = msg.m_iMyTradeItemType[i];
			pkTradePackData->m_akMyTradePackage[i].m_iItemType = msg.m_iMyTradeItemIndex[i];
			pkTradePackData->m_akMyTradePackage[i].m_kEntry.GetFromProtocol(msg.m_akMyTradePackage[i]);
			pkTradePackData->m_akTradeTargetPackage[i].GetFromProtocol(msg.m_akTradeTargetPackage[i]);
		}
 
 		pkPlayer->UpdateDirtyFlag(DIRTY_FLAG_TRADEPACK);
 	}
}
