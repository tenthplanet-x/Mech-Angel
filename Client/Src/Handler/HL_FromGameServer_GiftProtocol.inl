//void HL_FromGameServer::_handlerSyncGiftAsset(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
//{
//	//NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
//	//NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
//	//int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();
//
//	////如果还未进入游戏，则退出
//	//if (eGameStage != ST_GAME_STAGE_GAME)
//	//	return;
//	
//	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
//	if(!pkGamePlayer)
//		return;
//
//	MG_Sync_GiftAssetData syncMsg;
//	if(!syncMsg.Decode(pMsg,wSize))
//	{
//		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_Sync_GiftAssetData解析错误");
//		return;
//	}
//
//	LC_GiftAsset* pkGiftAsset = pkGamePlayer->GetGiftAsset();
//	pkGiftAsset->GetDataFromProtocolStruct(syncMsg.m_kGiftAssetData);
//
//	LC_GameEvent kGameEvent;
//	kGameEvent.SetEventType(GLET_RECEIVE_GIFT);
//
//	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
//	pkUIManager->HandlerGameEvent(kGameEvent);
//}

//void HL_FromGameServer::_handlerRltGetGift(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
//{
//	//NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
//	//NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
//	//int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();
//
//	////如果还未进入游戏，则退出
//	//if (eGameStage != ST_GAME_STAGE_GAME)
//	//	return;
//
//	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
//	if(!pkGamePlayer)
//		return;
//
//	MG_RLT_GET_GIFT rltMsg;
//	if(!rltMsg.Decode(pMsg,wSize))
//	{
//		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_RLT_GET_GIFT解析错误");
//		return;
//	}
//
//	ResultType iResult = rltMsg.m_iResult;
//	long lItemTypeID = rltMsg.m_lItemTypeID;
//	long lGiftID = rltMsg.m_lGiftID;
//
//	CF_ItemList* pkCSVItemList = CF_ItemList::GetSingletonPtr();
//	CF_ItemList::DataEntry* pkItemData = pkCSVItemList->GetEntryPtr(lItemTypeID);
//
//	if (!pkItemData)
//		return;
//
//	StringType strItemName = pkItemData->_kName;
//
//	SendLogicInfoMessage(iResult, strItemName.c_str());
//}