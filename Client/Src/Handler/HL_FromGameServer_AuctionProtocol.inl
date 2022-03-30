#include "LC_ClientAuctionManager.h"

#include "MG_CS_Auction.h"

void HL_FromGameServer::_handlerRltAddItemToAuctionSell(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_AddItemToAuctionSell kMsg;

	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "协议解析错误 MG_RLT_AddItemToAuctionSell");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return;
	}

	if (RE_SUCCESS == kMsg.m_iResultType)
	{
		LC_ClientAuctionManager* pkClientAuctionManager = LC_ClientAuctionManager::GetSingletonPtr();
		if (pkClientAuctionManager)
		{
			pkClientAuctionManager->GetAddItemInfo()->AddItemAffix(kMsg.m_byPackType, kMsg.m_usItemIndex);

			LC_BackPackEntry* pkEntry = pkGamePlayer->GetPackAsset()->GetBackPackEntry(kMsg.m_byPackType, kMsg.m_usItemIndex);
			if (NULL == pkEntry || !pkEntry->GetValid() )
			{
				return;
			}
			/*pkEntry->SetLocked(true);*/
			pkGamePlayer->GetPackAsset()->AddDirtyFlagRevisionID(kMsg.m_byPackType);
		}
	}

	SendLogicInfoMessage(kMsg.m_iResultType);
}

void HL_FromGameServer::_handlerRltDelItemFromAuctionSell(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_DeleteItemFromAuctionSell kMsg;

	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "删除信件附件结果协议解析错误 MG_RLT_AddItemToMailAffix");	
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return;
	}

	if (RE_SUCCESS == kMsg.m_iResultType)
	{
		LC_ClientAuctionManager* pkClientAuctionManager = LC_ClientAuctionManager::GetSingletonPtr();
		if (NULL == pkClientAuctionManager)
		{
			return;
		}	
		pkClientAuctionManager->GetAddItemInfo()->DeleletItemAffix();

		LC_BackPackEntry* pkEntry = pkGamePlayer->GetPackAsset()->GetBackPackEntry(kMsg.m_byPackType, kMsg.m_usItemIndex);
		if (NULL == pkEntry || !pkEntry->GetValid() )
		{
			return;
		}
		/*pkEntry->SetLocked(false);*/
		pkGamePlayer->GetPackAsset()->AddDirtyFlagRevisionID(kMsg.m_byPackType);
	}
	SendLogicInfoMessage(kMsg.m_iResultType);
}

void HL_FromGameServer::_handlerRltAuctionSell(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_CS_RLT_Aution_Sell kMsg;

	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "协议解析错误 MG_CS_RLT_Aution_Sell");
		return;
	}

	if (RE_SUCCESS == kMsg.m_iResultType)
	{
		SendLogicInfoMessage(RE_NEW_AUCTION_NOTSELL_SUCCESS_10252);

		LC_ClientAuctionManager* pkClientAuctionMgr = LC_ClientAuctionManager::GetSingletonPtr();
		pkClientAuctionMgr->UpdateAuctionMarket(kMsg.m_kAuction);
		
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_CLIENT_AUCTION_LIST_INIT_SUCCESS);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
	else
	{
		SendLogicInfoMessage(kMsg.m_iResultType);
	}
}

void HL_FromGameServer::_handlerRltAuctionList(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_AuctionList kMsg;
	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "协议解析错误 MG_RLT_AuctionList");
		return;
	}

	LC_ClientAuctionManager* pkClientAuctionManager = LC_ClientAuctionManager::GetSingletonPtr();
	if ( NULL == pkClientAuctionManager )
	{
		return;
	}

	// 将数据放到
	if (REQ_USER_SELL_AUCTION == (int)kMsg.m_byReqType)
	{
		pkClientAuctionManager->UpdateAuctionSellDataList(kMsg);
	}
	else if (REQ_USER_BUY_AUCTION == (int)kMsg.m_byReqType)
	{
		pkClientAuctionManager->UpdateAuctionBuyDataList(kMsg);
	}
	else
	{
		if(pkClientAuctionManager->UpdateAuctionMarketList(kMsg))
		{
			LC_GameEvent kGameEvent;
			kGameEvent.SetEventType(GLET_CLIENT_AUCTION_LIST_INIT_SUCCESS);

			UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
			pkUIManager->HandlerGameEvent(kGameEvent);
		}
	}
}

void HL_FromGameServer::_handlerRltAuctionCancel(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_CS_RLT_Aution_Cancel kMsg;
	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "协议解析错误 MG_CS_RLT_Aution_Cancel");
		return;
	}

	if (RE_AUCTIONSELLCANCEL_SUCCESS == kMsg.m_iResultType
		|| RE_NEW_AUCTION_BUY_FAILED_10226 == kMsg.m_iResultType)
	{
		LC_ClientAuctionManager* pkClientAuctionMgr = LC_ClientAuctionManager::GetSingletonPtr();
		pkClientAuctionMgr->DeleteAuctionSellData(kMsg.m_uiAuctionID);
		pkClientAuctionMgr->DeleteAuctionMarketData(kMsg.m_uiAuctionID);
	}

	SendLogicInfoMessage(kMsg.m_iResultType);
}

void HL_FromGameServer::_handlerRltAuctionBuy(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_CS_RLT_Aution_Buy kMsg;
	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "协议解析错误 MG_CS_RLT_Aution_Buy");
		return;
	}

	ResultType iResult = kMsg.m_iResultType;
	SendLogicInfoMessage(iResult);

	bool bNeedRefresh = iResult == RE_NEW_AUCTION_NOTSELL_FAILED_10223
						|| iResult == RE_NEW_AUCTION_BUY_FAILED_10225;

	bool bNotExist = iResult == RE_NEW_AUCTION_BUY_FAILED_10226
					|| iResult == RE_NEW_AUCTION_BUY_FAILED_10230
					|| bNeedRefresh;
	if (RE_SUCCESS != iResult && !bNotExist)
	{
		return;
	}

	int iCount = kMsg.m_iCount;
	if(bNotExist)
		iCount = -1;

	LC_ClientAuctionManager* pkClientAuctionManager = LC_ClientAuctionManager::GetSingletonPtr();

	bool Flush = true;
	if(RE_SUCCESS == iResult)
	{
		LC_ClientAuctionSellData* pkData = pkClientAuctionManager->GetAuctionMarketDataByAuctionID(kMsg.m_kAuctionUID);
		if(pkData)
			pkClientAuctionManager->AddHistory(pkData, kMsg.m_iCount);
	}
	else if(bNeedRefresh)
	{
		pkClientAuctionManager->RequestAuctionList(REQ_ALL_AUCTION);
		Flush = false;
	}

	if(Flush && pkClientAuctionManager->DeleteAuctionMarketData(kMsg.m_kAuctionUID, iCount))
	{
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_CLIENT_AUCTION_LIST_INIT_SUCCESS);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
}

void HL_FromGameServer::_handlerRltAuctionGetAward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_AuctionGetAward kMsg;
	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "协议解析错误 MG_RLT_AuctionGetAward");
		return;
	}

	if(kMsg.m_iResultType != RE_SUCCESS)
	{
		SendLogicInfoMessage(kMsg.m_iResultType);
		return;
	}

	LC_ClientAuctionManager* pkClientAuctionManager = LC_ClientAuctionManager::GetSingletonPtr();

	//一键领取成功后清空列表
	if(kMsg.m_byReqType == 2)
	{
		pkClientAuctionManager->ClearAuctionBuyList();
	}
	else
	{
		for(int i=0; i<kMsg.m_nAuctionCount; ++i)
		{
			pkClientAuctionManager->DeleteAuctionBuyData(kMsg.m_Auction[i]);
		}
	}
}

void HL_FromGameServer::_handlerRltAuctionNotice(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_CS_RLT_Aution_NOTICE kMsg;
	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "协议解析错误 MG_CS_RLT_Aution_NOTICE");
		return;
	}

	LC_ClientAuctionManager* pkClientAuctionManager = LC_ClientAuctionManager::GetSingletonPtr();
	pkClientAuctionManager->UpdateRewardNum(kMsg.m_iCount);
	pkClientAuctionManager->RequestAuctionList(REQ_USER_BUY_AUCTION);

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_CLIENT_AUCTION_GETAWARD_NOTIFY);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, kMsg.m_iCount);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}