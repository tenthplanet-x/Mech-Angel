#include "LC_ClientCurrencyTradeManager.h"

#include "MG_CS_CurrencyTradeProtocol.h"

#define ABS_INT_64_TO_32(inputNum) int((inputNum >= 0) ? inputNum : (-inputNum))

void HL_FromGameServer::_handlerRltCurrencyQuickBuy(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_CurrencyTradeQuickBuy kMsg;

	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "协议解析错误 MG_RLT_CurrencyTradeQuickBuy");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return;
	}

	LC_ClientCurrencyTradeManager* pkClientCurrencyTradeManager = LC_ClientCurrencyTradeManager::GetSingletonPtr();
	if ( NULL == pkClientCurrencyTradeManager)
	{
		return;
	}

	if (RE_SUCCESS == kMsg.m_iResult)
	{
		pkClientCurrencyTradeManager->RequestQuickTradeList();
		pkClientCurrencyTradeManager->RequestCommissionItemList();
		pkClientCurrencyTradeManager->ClearTradeRecordList();
		pkClientCurrencyTradeManager->RequestTradeRecordList(0);

		int iGold = ABS_INT_64_TO_32(kMsg.m_llRequestPrice / 10000);
		int iSilver = ABS_INT_64_TO_32((kMsg.m_llRequestPrice / 100) % 100);
		int iCopper = ABS_INT_64_TO_32(kMsg.m_llRequestPrice % 100);

		StringType strTip = pkClientCurrencyTradeManager->GetCurrencyTipString(iGold, iSilver, iCopper);

		SendLogicInfoMessage(RE_QUICK_BUY_SUCCESS, ABS_INT_64_TO_32(kMsg.m_llRequestCount), strTip);
	}
	else
	{
		SendLogicInfoMessage(RE_QUICK_BUY_FAIL);
	}	
}

void HL_FromGameServer::_handlerRltCurrencyQuickSell(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_CurrencyTradeQuickSell kMsg;

	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "协议解析错误 MG_RLT_CurrencyTradeQuickSell");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return;
	}

	LC_ClientCurrencyTradeManager* pkClientCurrencyTradeManager = LC_ClientCurrencyTradeManager::GetSingletonPtr();
	if ( NULL == pkClientCurrencyTradeManager)
	{
		return;
	}

	if (RE_SUCCESS == kMsg.m_iResult)
	{
		pkClientCurrencyTradeManager->RequestQuickTradeList();
		pkClientCurrencyTradeManager->RequestCommissionItemList();
		pkClientCurrencyTradeManager->ClearTradeRecordList();
		pkClientCurrencyTradeManager->RequestTradeRecordList(0);

		int iGold = ABS_INT_64_TO_32(kMsg.m_llRequestPrice / 10000);
		int iSilver = ABS_INT_64_TO_32((kMsg.m_llRequestPrice / 100) % 100);
		int iCopper = ABS_INT_64_TO_32(kMsg.m_llRequestPrice % 100);
		StringType strTip = pkClientCurrencyTradeManager->GetCurrencyTipString(iGold, iSilver, iCopper);

		SendLogicInfoMessage(RE_QUICK_SELL_SUCCESS, ABS_INT_64_TO_32(kMsg.m_llRequestPrice), strTip);
	}
	else
	{
		SendLogicInfoMessage(RE_QUICK_SELL_FAIL);
	}
}

void HL_FromGameServer::_handlerRltCurrencyCommissionBuy(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_CurrencyTradeCommissionBuy kMsg;

	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "协议解析错误 MG_RLT_CurrencyTradeCommissionBuy");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return;
	}

	LC_ClientCurrencyTradeManager* pkClientCurrencyTradeManager = LC_ClientCurrencyTradeManager::GetSingletonPtr();
	if (NULL == pkClientCurrencyTradeManager)
	{
		return;
	}

	if (RE_SUCCESS == kMsg.m_iResult)
	{
		if (kMsg.m_llDealCount > 0 && kMsg.m_llDealPrice > 0)
		{
			pkClientCurrencyTradeManager->ClearTradeRecordList();
			pkClientCurrencyTradeManager->RequestTradeRecordList(0);
		}

		pkClientCurrencyTradeManager->RequestQuickTradeList();
		pkClientCurrencyTradeManager->RequestCommissionItemList();

		int iGold = ABS_INT_64_TO_32(kMsg.m_llRequestPrice / 10000);
		int iSilver = ABS_INT_64_TO_32((kMsg.m_llRequestPrice / 100) % 100);
		int iCopper = ABS_INT_64_TO_32(kMsg.m_llRequestPrice % 100);
		StringType strTip = pkClientCurrencyTradeManager->GetCurrencyTipString(iGold, iSilver, iCopper);

		SendLogicInfoMessage(RE_COMMISSION_BUY_SUCCESS, ABS_INT_64_TO_32(kMsg.m_llRequestCount), strTip);
	}
	else
	{
		SendLogicInfoMessage(RE_COMMISSION_BUY_FAIL);
	}
}

void HL_FromGameServer::_handlerRltCurrencyCommissionSell(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_CurrencyTradeCommissionSell kMsg;

	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "协议解析错误 MG_RLT_CurrencyTradeCommissionSell");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return;
	}

	LC_ClientCurrencyTradeManager* pkClientCurrencyTradeManager = LC_ClientCurrencyTradeManager::GetSingletonPtr();
	if ( NULL == pkClientCurrencyTradeManager)
	{
		return;
	}

	if (RE_SUCCESS == kMsg.m_iResult)
	{
		if (kMsg.m_llDealCount > 0 && kMsg.m_llDealPrice > 0)
		{
			pkClientCurrencyTradeManager->ClearTradeRecordList();
			pkClientCurrencyTradeManager->RequestTradeRecordList(0);
		}		

		pkClientCurrencyTradeManager->RequestQuickTradeList();
		pkClientCurrencyTradeManager->RequestCommissionItemList();

		int iGold = ABS_INT_64_TO_32(kMsg.m_llRequestPrice / 10000);
		int iSilver = ABS_INT_64_TO_32((kMsg.m_llRequestPrice / 100) % 100);
		int iCopper = ABS_INT_64_TO_32(kMsg.m_llRequestPrice % 100);
		StringType strTip = pkClientCurrencyTradeManager->GetCurrencyTipString(iGold, iSilver, iCopper);

		SendLogicInfoMessage(RE_COMMISSION_SELL_SUCCESS, ABS_INT_64_TO_32(kMsg.m_llRequestCount), strTip);
	}
	else
	{
		SendLogicInfoMessage(RE_COMMISSION_SELL_FAIL);
	}
}

void HL_FromGameServer::_handlerRltCurrencyQuickTradeList(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_CurrencyQuickTradeList kMsg;
	if (!kMsg.Decode(pMsg, wSize))
	{
		return;
	}

	LC_ClientCurrencyTradeManager* pkClientCurrencyTradeManager = LC_ClientCurrencyTradeManager::GetSingletonPtr();
	if (NULL == pkClientCurrencyTradeManager)
	{
		return;
	}

	if (RE_SUCCESS == kMsg.m_iResult)
	{
		pkClientCurrencyTradeManager->UpdateQuickTradeList(kMsg);
		SendLogicInfoMessage(RE_REQUEST_QUICK_TRADE_LIST_SUCCESS);
	}
	else
	{
		SendLogicInfoMessage(RE_REQUEST_QUICK_TRADE_LIST_FAIL);
	}	
}

void HL_FromGameServer::_handlerRltCurrencyCommissionItemList(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_CurrencyCommissionItemList kMsg;
	if (!kMsg.Decode(pMsg, wSize))
	{
		return;
	}

	LC_ClientCurrencyTradeManager* pkClientCurrencyTradeManager = LC_ClientCurrencyTradeManager::GetSingletonPtr();
	if (NULL == pkClientCurrencyTradeManager)
	{
		return;
	}

	if (RE_SUCCESS == kMsg.m_iResult)
	{
		pkClientCurrencyTradeManager->UpdateCommissionItemList(kMsg);
		SendLogicInfoMessage(RE_REQUEST_COMMISSION_LIST_SUCCESS);
	}
	else
	{
		SendLogicInfoMessage(RE_REQUEST_COMMISSION_LIST_FAIL);
	}	
}

void HL_FromGameServer::_handlerRltCurrencyTradeRecordList(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_CurrencyTradeRecordList kMsg;
	if (!kMsg.Decode(pMsg, wSize))
	{
		return;
	}

	LC_ClientCurrencyTradeManager* pkClientCurrencyTradeManager = LC_ClientCurrencyTradeManager::GetSingletonPtr();
	if (NULL == pkClientCurrencyTradeManager)
	{
		return;
	}

	if (RE_SUCCESS == kMsg.m_iResult && kMsg.m_iCount > 0)
	{
		pkClientCurrencyTradeManager->UpdateTradeRecordList(kMsg);
		SendLogicInfoMessage(RE_REQUEST_TRADE_RECORD_LIST_SUCCESS);
	}
	else
	{
		SendLogicInfoMessage(RE_REQUEST_TRADE_RECORD_LIST_FAIL);
	}	
}

void HL_FromGameServer::_handlerRltCurrencyRevokeCommission(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_CurrencyRevokeCommission kMsg;
	if (!kMsg.Decode(pMsg, wSize))
	{
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return;
	}

	LC_ClientCurrencyTradeManager* pkClientCurrencyTradeManager = LC_ClientCurrencyTradeManager::GetSingletonPtr();
	if (NULL == pkClientCurrencyTradeManager)
	{
		return;
	}

	if (RE_SUCCESS == kMsg.m_iResult)
	{
		pkClientCurrencyTradeManager->SetRevokeCommissionFlag(true);
		pkClientCurrencyTradeManager->RequestCommissionItemList();
		pkClientCurrencyTradeManager->RequestQuickTradeList();
		SendLogicInfoMessage(RE_REQUEST_COMMISSION_REVOKE_SUCCESS);
	}
	else
	{
		SendLogicInfoMessage(RE_REQUEST_COMMISSION_REVOKE_FAIL);
	}	
}

void HL_FromGameServer::_handlerRltCurrencyYuanBaoDeposit(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_CurrencyYuanBaoDeposit kMsg;

	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "协议解析错误 MG_RLT_CurrencyYuanBaoDeposit");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return;
	}

	LC_ClientCurrencyTradeManager* pkClientCurrencyTradeManager = LC_ClientCurrencyTradeManager::GetSingletonPtr();
	if (NULL == pkClientCurrencyTradeManager)
	{
		return;
	}

	if (RE_SUCCESS == kMsg.m_iResult && kMsg.m_llResultYuanBao >= 0)
	{
		pkClientCurrencyTradeManager->SetAccountYuanBao(kMsg.m_llResultYuanBao);

		SendLogicInfoMessage(RE_YUANBAO_DEPOSIT_SUCCESS, ABS_INT_64_TO_32(kMsg.m_llYuanBaoCount));
	}
	else
	{
		SendLogicInfoMessage(RE_YUANBAO_DEPOSIT_FAIL);
	}	
}

void HL_FromGameServer::_handlerRltCurrencyYuanBaoDraw(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_CurrencyYuanBaoDraw kMsg;

	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "协议解析错误 MG_RLT_CurrencyYuanBaoDraw");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return;
	}

	LC_ClientCurrencyTradeManager* pkClientCurrencyTradeManager = LC_ClientCurrencyTradeManager::GetSingletonPtr();
	if (NULL == pkClientCurrencyTradeManager)
	{
		return;
	}
 
	if (RE_SUCCESS == kMsg.m_iResult && kMsg.m_llResultYuanBao >= 0)
	{
		MoneyType iAccountYuanBao = pkClientCurrencyTradeManager->GetAccountYuanBaoNum();
		pkClientCurrencyTradeManager->SetAccountYuanBao(iAccountYuanBao - kMsg.m_llYuanBaoCount);

		SendLogicInfoMessage(RE_YUANBAO_DRAW_SUCCESS, ABS_INT_64_TO_32(kMsg.m_llYuanBaoCount));
	}
	else
	{
		SendLogicInfoMessage(RE_YUANBAO_DRAW_FAIL);
	}	
}

void HL_FromGameServer::_handlerRltCurrencyMoneyDeposit(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_CurrencyMoneyDeposit kMsg;

	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "协议解析错误 MG_RLT_CurrencyMoneyDeposit");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return;
	}

	LC_ClientCurrencyTradeManager* pkClientCurrencyTradeManager = LC_ClientCurrencyTradeManager::GetSingletonPtr();
	if (NULL == pkClientCurrencyTradeManager)
	{
		return;
	}

	if (RE_SUCCESS == kMsg.m_iResult && kMsg.m_llResultMoney >= 0)
	{
		pkClientCurrencyTradeManager->SetAccountMoney(kMsg.m_llResultMoney);

		int iGold = ABS_INT_64_TO_32(kMsg.m_llMoneyCount / 10000);
		int iSilver = ABS_INT_64_TO_32((kMsg.m_llMoneyCount / 100) % 100);
		int iCopper = ABS_INT_64_TO_32(kMsg.m_llMoneyCount % 100);
		StringType strTip = pkClientCurrencyTradeManager->GetCurrencyTipString(iGold, iSilver, iCopper);

		SendLogicInfoMessage(RE_MONEY_DEPOSIT_SUCCESS, strTip);
	}
	else
	{
		SendLogicInfoMessage(RE_MONEY_DEPOSIT_FAIL);
	}	
}

void HL_FromGameServer::_handlerRltCurrencyMoneyDraw(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_CurrencyMoneyDraw kMsg;

	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "协议解析错误 MG_RLT_CurrencyMoneyDraw");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return;
	}

	LC_ClientCurrencyTradeManager* pkClientCurrencyTradeManager = LC_ClientCurrencyTradeManager::GetSingletonPtr();
	if (NULL == pkClientCurrencyTradeManager)
	{
		return;
	}

	if (RE_SUCCESS == kMsg.m_iResult && kMsg.m_llResultMoney >= 0)
	{
		pkClientCurrencyTradeManager->SetAccountMoney(kMsg.m_llResultMoney);

		int iGold = ABS_INT_64_TO_32(kMsg.m_llMoneyCount / 10000);
		int iSilver = ABS_INT_64_TO_32((kMsg.m_llMoneyCount / 100) % 100);
		int iCopper = ABS_INT_64_TO_32(kMsg.m_llMoneyCount % 100);
		StringType strTip = pkClientCurrencyTradeManager->GetCurrencyTipString(iGold, iSilver, iCopper);

		SendLogicInfoMessage(RE_MONEY_DRAW_SUCCESS, strTip);
	}
	else
	{
		SendLogicInfoMessage(RE_MONEY_DRAW_FAIL);
	}	
}