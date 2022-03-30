
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerRequestTradeItem(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//如果还未进入游戏，则退出
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;


	MG_REQ_RequestTrade reqMsg;
	if(!reqMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "请求物品交易消息解析错误");
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

	//如果还未进入游戏，则退出
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;


	MG_RLT_RequestTrade rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "请求物品交易回复解析错误");
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

	//如果还未进入游戏，则退出
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;


	MG_RLT_ReplyTrade rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "交易请求答复回复消息解析错误");
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
			TargetName = "请求对象";
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

	//如果还未进入游戏，则退出
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;


	MG_RLT_AddItemToTradePack rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "交易请求添加物品消息解析错误");
		return;
	}

	StringType TargetName = "";

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

	if (rltMsg.m_lTradeCharID == pkGamePlayer->GetInstance())
	{
		TargetName = "您";
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
			TargetName = "交易对象";
	}

	if (rltMsg.m_iResult == RE_ITEM_TRADE_ADD_SUCCESS)
	{
		SendLogicInfoMessage(rltMsg.m_iResult,TargetName.c_str());
	}
	else
	{
		//只显示与自身相关的错误提示
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

	//如果还未进入游戏，则退出
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;


	MG_RLT_RemoveItemFromTradePack rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "交易请求移除物品消息解析错误");
		return;
	}

	StringType TargetName = "";

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

	if (rltMsg.m_lTradeCharID == pkGamePlayer->GetInstance())
	{
		TargetName = "您";
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
			TargetName = "交易对象";
	}
	if (rltMsg.m_iResult == RE_ITEM_TRADE_REMOVE_SUCCESS)
	{
		SendLogicInfoMessage(rltMsg.m_iResult,TargetName.c_str());
	}
	else
	{
		//只显示与自身相关的错误提示
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

	//如果还未进入游戏，则退出
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;


	MG_RLT_UpdateTradeCash rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "交易请求更新交易现金消息解析错误");
		return;
	}

	StringType TargetName = "";

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

	if (rltMsg.m_lTradeCharID == pkGamePlayer->GetInstance())
	{
		TargetName = "您";
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
			TargetName = "交易对象";
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
		//只显示与自身相关的错误提示
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

	//如果还未进入游戏，则退出
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;


	MG_RLT_LockTrade rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "交易请求锁定交易消息解析错误");
		return;
	}

	StringType TargetName = "";

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

	if (rltMsg.m_lTradeCharID == pkGamePlayer->GetInstance())
	{
		TargetName = "您";
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
			TargetName = "交易对象";
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
		//只显示与自身相关的错误提示
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

	//如果还未进入游戏，则退出
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;


	MG_RLT_UnLockTrade rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "交易请求取消锁定交易消息解析错误");
		return;
	}

	StringType TargetName = "";

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

	unique_id_type lTargetID = 0;
	if (rltMsg.m_lTradeCharID == pkGamePlayer->GetInstance())
	{
		TargetName = "您";
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
			TargetName = "交易对象";
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
		//只显示与自身相关的错误提示
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

	//如果还未进入游戏，则退出
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;


	MG_RLT_InterruptTrade rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "交易请求中断交易消息解析错误");
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

	//如果还未进入游戏，则退出
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;


	MG_RLT_FinishTrade rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "交易请求完成交易消息解析错误");
		return;
	}

	StringType TargetName = "";
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

	if (rltMsg.m_lTradeCharID == pkGamePlayer->GetInstance())
	{
		TargetName = "您";
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
			TargetName = "交易对象";
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
		//只显示与自身相关的错误提示
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

	//如果还未进入游戏，则退出
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;


#pragma TODO("消息调整需要修改")
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
//ToCheck 目前先不做处理 后面看需要怎么改
			LC_ClientNetPlayerManager* clientNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
			if(clientNetPlayerManager->FindPlayer(lPlayerID) != NULL)
			{
				GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_ERROR,"网络:同步背包逻辑错误, 发过来的是netplayerID,不做处理");
				return;
			}
			else
			{
				GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"网络:同步背包逻辑错误");
				return;
			}
 		}

		LC_TradeManager *pkTradeManager = pkPlayer->GetTradeManager();
		if (NULL == pkTradeManager)
		{
			return;
		}
//ToCheck 这里很可能有问题 目前buddy ID为clone playerID 很可能会出问题
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
