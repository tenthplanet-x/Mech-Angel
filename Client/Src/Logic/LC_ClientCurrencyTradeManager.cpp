#include "ClientPCH.h"

#include "LC_ClientCurrencyTradeManager.h"
#include "MG_CS_CurrencyTradeProtocol.h"
#include "LC_ClientLogicManager.h"
#include "LC_ClientGamePlayer.h"
#include "MG_MessageBase.h"
#include "NW_Helper.h"
#include "UT_LogicInfoMessage.h"

using namespace GameLogic;
using namespace Protocol;

LC_ClientCurrencyTradeManager::LC_ClientCurrencyTradeManager()
: m_llAccountMoney(0)
, m_llAccountYuanBao(0)
, m_bRevokeFlag(false)
{
	ClearDirtyFlag();
	ClearDirtyFlagRevisionID();
	ResetTradeRecordPageLoadFlag();
}

LC_ClientCurrencyTradeManager::~LC_ClientCurrencyTradeManager()
{

}

void LC_ClientCurrencyTradeManager::Reset()
{

}

void LC_ClientCurrencyTradeManager::RequestQuickBuy(MoneyType iRequestPriceGold, MoneyType iRequestPriceSilver, MoneyType iRequestPriceCopper, int32_t iRequestCount)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager)
	{
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (NULL == pkGamePlayer)
	{
		return;
	}
	
	MoneyType iRequestPrice = iRequestPriceGold * 10000 + iRequestPriceSilver * 100 + iRequestPriceCopper;

	MoneyType iNeedTotalMoney = iRequestCount * iRequestPrice * 102 / 100;
	if(m_llAccountMoney < iNeedTotalMoney)
	{
		SendLogicInfoMessage(RE_MONEY_NOT_ENOUGH);
		return;
	}

	MG_REQ_CurrencyTradeQuickBuy kMsg;

	kMsg.m_llRequestPrice	= iRequestPrice;
	kMsg.m_llRequestCount	= iRequestCount;
	kMsg.m_uiPlayerID		= pkGamePlayer->GetUserID();		

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_CURRENCY_QUICK_BUY, &kMsg);

	if (bCreateMsg)
	{
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}

void LC_ClientCurrencyTradeManager::RequestQuickSell(MoneyType iRequestPriceGold, MoneyType iRequestPriceSilver, MoneyType iRequestPriceCopper, int32_t iRequestCount)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager)
	{
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (NULL == pkGamePlayer)
	{
		return;
	}

	MoneyType iRequestPrice = iRequestPriceGold * 10000 + iRequestPriceSilver * 100 + iRequestPriceCopper;
	if (iRequestPrice <= 0)
	{
		SendLogicInfoMessage(RE_REQUEST_PRICE_INVALID);
		return;
	}

	MG_REQ_CurrencyTradeQuickSell kMsg;

	kMsg.m_llRequestPrice	= iRequestPrice;
	kMsg.m_llRequestCount	= iRequestCount;
	kMsg.m_uiPlayerID		= pkGamePlayer->GetUserID();

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_CURRENCY_QUICK_SELL, &kMsg);

	if (bCreateMsg)
	{
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}

void LC_ClientCurrencyTradeManager::RequestCommissionBuy(MoneyType iRequestPriceGold, MoneyType iRequestPriceSilver, MoneyType iRequestPriceCopper, int32_t iRequestCount)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager)
	{
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (NULL == pkGamePlayer)
	{
		return;
	}

	MoneyType iRequestPrice = iRequestPriceGold * 10000 + iRequestPriceSilver * 100 + iRequestPriceCopper;

	if (iRequestPrice <= 0)
	{
		SendLogicInfoMessage(RE_REQUEST_PRICE_INVALID);
		return;
	}

	MG_REQ_CurrencyTradeCommissionBuy kMsg;

	kMsg.m_llRequestPrice	= iRequestPrice;
	kMsg.m_llRequestCount	= iRequestCount;
	kMsg.m_uiPlayerID		= pkGamePlayer->GetUserID();

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_CURRENCY_COMMISSION_BUY, &kMsg);

	if (bCreateMsg)
	{
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}

void LC_ClientCurrencyTradeManager::RequestCommissionSell(MoneyType iRequestPriceGold, MoneyType iRequestPriceSilver, MoneyType iRequestPriceCopper, int32_t iRequestCount)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager)
	{
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (NULL == pkGamePlayer)
	{
		return;
	}

	MoneyType iRequestPrice = iRequestPriceGold * 10000 + iRequestPriceSilver * 100 + iRequestPriceCopper;

	if (iRequestPrice <= 0)
	{
		SendLogicInfoMessage(RE_REQUEST_PRICE_INVALID);
		return;
	}

	MG_REQ_CurrencyTradeCommissionSell kMsg;

	kMsg.m_llRequestPrice	= iRequestPrice;
	kMsg.m_llRequestCount	= iRequestCount;
	kMsg.m_uiPlayerID		= pkGamePlayer->GetUserID();

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_CURRENCY_COMMISSION_SELL, &kMsg);

	if (bCreateMsg)
	{
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}

void LC_ClientCurrencyTradeManager::RequestYuanBaoDeposit(int32_t iYuanBaoCount)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager)
	{
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (NULL == pkGamePlayer)
	{
		return;
	}

	if (iYuanBaoCount <= 0)
	{
		SendLogicInfoMessage(RE_YUANBAO_NOT_ENOUGH);
		return;
	}

	MG_REQ_CurrencyYuanBaoDeposit kMsg;
	kMsg.m_uiPlayerID = pkGamePlayer->GetUserID();
	kMsg.m_llYuanBaoCount = iYuanBaoCount;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_CURRENCY_YUANBAO_DEPOSIT, &kMsg);

	if (bCreateMsg)
	{
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}

void LC_ClientCurrencyTradeManager::RequestYuanBaoDraw(int32_t iYuanBaoCount)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager)
	{
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (NULL == pkGamePlayer)
	{
		return;
	}

	if (iYuanBaoCount <= 0)
	{
		SendLogicInfoMessage(RE_YUANBAO_NOT_ENOUGH);
		return;
	}

	MG_REQ_CurrencyYuanBaoDraw kMsg;
	kMsg.m_uiPlayerID		= pkGamePlayer->GetUserID();
	kMsg.m_llYuanBaoCount	= -iYuanBaoCount;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_CURRENCY_YUANBAO_DRAW, &kMsg);

	if (bCreateMsg)
	{
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}

void LC_ClientCurrencyTradeManager::RequestMoneyDeposit(MoneyType iRequestPriceGold, MoneyType iRequestPriceSilver, MoneyType iRequestPriceCopper)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager)
	{
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (NULL == pkGamePlayer)
	{
		return;
	}

	MoneyType iMoneyCount = iRequestPriceGold * 10000 + iRequestPriceSilver * 100 + iRequestPriceCopper;
	if (iMoneyCount <= 0)
	{
		SendLogicInfoMessage(RE_MONEY_NOT_ENOUGH);
		return;
	}

	MG_REQ_CurrencyMoneyDeposit kMsg;
	kMsg.m_uiPlayerID = pkGamePlayer->GetUserID();
	kMsg.m_llMoneyCount = iMoneyCount;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_CURRENCY_MONEY_DEPOSIT, &kMsg);

	if (bCreateMsg)
	{
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}

void LC_ClientCurrencyTradeManager::RequestMoneyDraw(MoneyType iRequestPriceGold, MoneyType iRequestPriceSilver, MoneyType iRequestPriceCopper)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager)
	{
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (NULL == pkGamePlayer)
	{
		return;
	}

	MoneyType iMoneyCount = iRequestPriceGold * 10000 + iRequestPriceSilver * 100 + iRequestPriceCopper;
	if (iMoneyCount <= 0)
	{
		SendLogicInfoMessage(RE_MONEY_NOT_ENOUGH);
		return;
	}

	MG_REQ_CurrencyMoneyDraw kMsg;
	kMsg.m_uiPlayerID = pkGamePlayer->GetUserID();
	kMsg.m_llMoneyCount = -iMoneyCount;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_CURRENCY_MONEY_DRAW, &kMsg);

	if (bCreateMsg)
	{
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}

void LC_ClientCurrencyTradeManager::RequestQuickTradeList()
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager)
	{
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (NULL == pkGamePlayer)
	{
		return;
	}
	
	MG_REQ_CurrencyQuickTradeList kMsg;

	kMsg.m_uiPlayerID = pkGamePlayer->GetUserID();

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_CURRENCY_QUICK_TRADE_LIST, &kMsg);

	if (bCreateMsg)
	{
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}

void LC_ClientCurrencyTradeManager::RequestTradeRecordPages(uint32_t uiPage)
{
	// 如果本次打开游戏币交易系统后，已经load过相应的page，则不需要再次load
	if (m_bRecordPageLoadFlag[uiPage] == true)
	{
		SendLogicInfoMessage(RE_TRADE_RECORD_PAGE_WAS_LOADED);
		return;
	}
	RequestTradeRecordList(uiPage);
}

void LC_ClientCurrencyTradeManager::RequestTradeRecordList(uint32_t uiPage)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager)
	{
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (NULL == pkGamePlayer)
	{
		return;
	}

  /*
	if (uiPage < 0)
	{
		SendLogicInfoMessage(RE_TRADE_RECORD_PAGE_INVALID);
		return;
	}
  */

	MG_REQ_CurrencyTradeRecordList kMsg;

	kMsg.m_iPage	  = uiPage;
	kMsg.m_uiPlayerID = pkGamePlayer->GetUserID();

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_CURRENCY_TRADE_RECORD_LIST, &kMsg);

	if (bCreateMsg)
	{
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}
 
void LC_ClientCurrencyTradeManager::RequestCommissionItemList()
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager)
	{
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (NULL == pkGamePlayer)
	{
		return;
	}

	MG_REQ_CurrencyCommissionItemList kMsg;

	kMsg.m_uiPlayerID = pkGamePlayer->GetUserID();

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_CURRENCY_COMMISSION_ITEM_LIST, &kMsg);

	if (bCreateMsg)
	{
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}

int LC_ClientCurrencyTradeManager::GetCurrencyQuickBuyListSize()
{
	return m_kCurrencyTradeQuickBuyArray.size();
}

int LC_ClientCurrencyTradeManager::GetCurrencyQuickSellListSize()
{
	return m_kCurrencyTradeQuickSellArray.size();
}

LC_CurrencyTradeQuickTradeItem* LC_ClientCurrencyTradeManager::GetCurrencyQuickTradeItem(int iTradeType, int index)
{
	if (index > 3 || index < 0)
	{
		return NULL;
	}
	// 求购列表
	if (TRADE_BUY == iTradeType)
	{
		if (index < m_kCurrencyTradeQuickBuyArray.size())
		{
			LC_CurrencyTradeQuickTradeItem* pkData = &m_kCurrencyTradeQuickBuyArray.at(index);
			return pkData;
		}
		else
		{
			return NULL;
		}		
	}
	// 出售列表
	else if(TRADE_SELL == iTradeType)
	{
		if (index < m_kCurrencyTradeQuickSellArray.size())
		{
			LC_CurrencyTradeQuickTradeItem* pkData = &m_kCurrencyTradeQuickSellArray.at(index);
			return pkData;
		}
		else
		{
			return NULL;
		}
	}
	return NULL;
}

int LC_ClientCurrencyTradeManager::GetCurrencyCommissionListSize()
{
	return m_kCurrencyTradeCommssionItemArray.size();
}

LC_CurrencyTradeCommssionItem* LC_ClientCurrencyTradeManager::GetCurrencyCommissionItem(int index)
{
	if (index < 0 || m_kCurrencyTradeCommssionItemArray.size() <= 0)
	{
		return NULL;
	}

	LC_CurrencyTradeCommssionItem* pkData = &m_kCurrencyTradeCommssionItemArray.at(index);

	return pkData;
}

int LC_ClientCurrencyTradeManager::GetCurrencyTradeRecordListSize()
{
	return m_kCurrencyTradeRecordItemArray.size();
}

int LC_ClientCurrencyTradeManager::GetTradeRecordListPage()
{
	return m_uiTradeRecordPage;
};

LC_CurrencyTradeRecordItem* LC_ClientCurrencyTradeManager::GetCurrencyTradeRecordItem(int index)
{
	if (index < 0 || m_kCurrencyTradeRecordItemArray.size() <= 0)
	{
		return NULL;
	}

	LC_CurrencyTradeRecordItem* pkData = &m_kCurrencyTradeRecordItemArray.at(index);

	return pkData;
}

bool LC_ClientCurrencyTradeManager::GetRevokeCommissionFlag()
{
	return m_bRevokeFlag;
}

void LC_ClientCurrencyTradeManager::SetRevokeCommissionFlag(bool bFlag)
{
	m_bRevokeFlag = bFlag;
}

void LC_ClientCurrencyTradeManager::RequestRevokeCommission(uint32_t uiCommissionID, int index)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager)
	{
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (NULL == pkGamePlayer)
	{
		return;
	}

	if (index < 0 || index >= m_kCurrencyTradeCommssionItemArray.size())
	{
		SendLogicInfoMessage(RE_REQUEST_REVOKE_INVALID);
		return;
	}
	LC_CurrencyTradeCommssionItem commissionItem = m_kCurrencyTradeCommssionItemArray.at(index);
	
	MG_REQ_CurrencyRevokeCommission kMsg;
	kMsg.m_uiPlayerID		= pkGamePlayer->GetUserID();
	kMsg.m_uiCommissionID	= uiCommissionID;
	kMsg.m_uiType			= commissionItem.m_eTradeType;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_CURRENCY_REVOKE_COMMISSION, &kMsg);
	if (bCreateMsg)
	{
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}

void LC_ClientCurrencyTradeManager::SetAccountMoney(int64_t iMoney)
{
	m_llAccountMoney = iMoney;
	SetDirtyFlag(true, DIRTY_FLAG_ACCOUNT_EXCHANGE);
	AddDirtyFlagRevisionID(DIRTY_FLAG_ACCOUNT_EXCHANGE);
}

StringType LC_ClientCurrencyTradeManager::GetAccountMoney()
{
	return LC_Money::MoneyNumber2String(m_llAccountMoney, true);
}

void LC_ClientCurrencyTradeManager::SetAccountYuanBao(int64_t iYuanBao)
{
	m_llAccountYuanBao = iYuanBao;
	SetDirtyFlag(true, DIRTY_FLAG_ACCOUNT_EXCHANGE);
	AddDirtyFlagRevisionID(DIRTY_FLAG_ACCOUNT_EXCHANGE);
}

StringType LC_ClientCurrencyTradeManager::GetAccountYuanBao()
{
	return LC_Money::MoneyNumber2String(m_llAccountYuanBao, false);
}

MoneyType LC_ClientCurrencyTradeManager::GetAccountYuanBaoNum() const
{
	return m_llAccountYuanBao;
}

void LC_ClientCurrencyTradeManager::UpdateQuickTradeList(MG_RLT_CurrencyQuickTradeList& kMsg)
{
	ClearQuickTradeList();

	for (int i = 0; i < kMsg.m_llSellItemCount; ++i)
	{
		LC_CurrencyTradeQuickTradeItem sellItem;
		sellItem.m_iLeftAmount		= kMsg.m_stSellList[i].m_uiCount;
		sellItem.m_uiUnitPrice		= kMsg.m_stSellList[i].m_iUnitPrice;

		m_kCurrencyTradeQuickSellArray.push_back(sellItem);
	}

	for (int i = 0; i < kMsg.m_llBuyItemCount; ++i)
	{
		LC_CurrencyTradeQuickTradeItem buyItem;
		buyItem.m_iLeftAmount		= kMsg.m_stBuyList[i].m_uiCount;
		buyItem.m_uiUnitPrice		= kMsg.m_stBuyList[i].m_iUnitPrice;

		m_kCurrencyTradeQuickBuyArray.push_back(buyItem);
	}

	// 账户钱币和元宝
	m_llAccountMoney		= kMsg.m_llMoney;
	m_llAccountYuanBao	= kMsg.m_llYuanBao;
	SetDirtyFlag(true, DIRTY_FLAG_QUICK_TRADE);
	AddDirtyFlagRevisionID(DIRTY_FLAG_QUICK_TRADE);
}

void LC_ClientCurrencyTradeManager::UpdateTradeRecordList(MG_RLT_CurrencyTradeRecordList& kMsg)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager)
	{
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (NULL == pkGamePlayer)
	{
		return;
	}

	for (int i = 0; i < kMsg.m_iCount; ++i)
	{
		LC_CurrencyTradeRecordItem item;		
		item.m_uiDealTime		= kMsg.m_stExchangeHistory[i].m_uiTime;
		item.m_uiTradeAmount	= kMsg.m_stExchangeHistory[i].m_iDealPrice;  //总额，非单价
		item.m_uiTradeVolume	= kMsg.m_stExchangeHistory[i].m_uiDealCount;
		item.m_uiTradeID		= kMsg.m_stExchangeHistory[i].m_uiExchangeID;
		item.m_uiUnitPrice		= kMsg.m_stExchangeHistory[i].m_iDealPrice / kMsg.m_stExchangeHistory[i].m_uiDealCount;

		// 如果buyer的ID和玩家ID一致，说明玩家买了元宝
		if (kMsg.m_stExchangeHistory[i].m_uiBuyID == pkGamePlayer->GetUserID())
		{
			item.m_eTradeType = TRADE_BUY;
			m_kCurrencyTradeRecordItemArray.push_back(item);
		}
		// 如果seller的ID和玩家ID一致，说明玩家卖了元宝
		if (kMsg.m_stExchangeHistory[i].m_uiSellID == pkGamePlayer->GetUserID())
		{
			item.m_eTradeType = TRADE_SELL;
			m_kCurrencyTradeRecordItemArray.push_back(item);
		}	
	}
	m_uiTradeRecordPage = kMsg.m_iPage;
	m_bRecordPageLoadFlag[kMsg.m_iPage] = true;
	SetDirtyFlag(true, DIRTY_FLAG_TRADE_RECORD);
	AddDirtyFlagRevisionID(DIRTY_FLAG_TRADE_RECORD);
}

void LC_ClientCurrencyTradeManager::UpdateCommissionItemList(MG_RLT_CurrencyCommissionItemList& kMsg)
{
	ClearCommissionItemList();

	for (int i = 0; i < kMsg.m_iCount; ++i)
	{
		LC_CurrencyTradeCommssionItem item;
		item.m_eTradeType		= (TradeType)kMsg.m_stExchangeList[i].m_iType;
		item.m_uiLeftTime		= kMsg.m_stExchangeList[i].m_uiTime;
		item.m_iLeftAmount		= kMsg.m_stExchangeList[i].m_uiCount - kMsg.m_stExchangeList[i].m_uiDealCount;
		item.m_iTradeVolume		= kMsg.m_stExchangeList[i].m_uiDealCount;
		item.m_uiCommssionID	= kMsg.m_stExchangeList[i].m_uiAutoid;
		item.m_uiUnitPrice		= kMsg.m_stExchangeList[i].m_iUnitPrice;

		m_kCurrencyTradeCommssionItemArray.push_back(item);
	}
	if (m_bRevokeFlag)
	{
		SetDirtyFlag(true, DIRTY_FLAG_COMMISSION_REVOKE);
		AddDirtyFlagRevisionID(DIRTY_FLAG_COMMISSION_REVOKE);
		m_bRevokeFlag = false;
	}
	else
	{
		SetDirtyFlag(true, DIRTY_FLAG_COMMISSION);
		AddDirtyFlagRevisionID(DIRTY_FLAG_COMMISSION);
	}
}

void LC_ClientCurrencyTradeManager::ClearQuickTradeList()
{
	m_kCurrencyTradeQuickBuyArray.clear();
	m_kCurrencyTradeQuickSellArray.clear();	
}

void LC_ClientCurrencyTradeManager::ClearTradeRecordList()
{
	m_kCurrencyTradeRecordItemArray.clear();
	ResetTradeRecordPageLoadFlag();
}

void LC_ClientCurrencyTradeManager::ClearCommissionItemList()
{
	m_kCurrencyTradeCommssionItemArray.clear();	
}

void LC_ClientCurrencyTradeManager::SetDirtyFlag(bool bFlag, DirtyType eDirtyType)
{
	m_bDirtyFlag[eDirtyType] = bFlag;
}

bool LC_ClientCurrencyTradeManager::GetDirtyFlag(int iDirtyType)
{
	return m_bDirtyFlag[iDirtyType];	
}

void LC_ClientCurrencyTradeManager::ClearDirtyFlag()
{
	for (int i = 0; i < MAX_DIRTY_TYPE_NUM; ++i)
	{
		m_bDirtyFlag[i] = false;
	}
}

void LC_ClientCurrencyTradeManager::AddDirtyFlagRevisionID(DirtyType eDirtyType)
{
	++m_lDirtyFlagRevisionID[eDirtyType];
}

long LC_ClientCurrencyTradeManager::GetDirtyFlagRevisionID(int iDirtyType)
{
	return m_lDirtyFlagRevisionID[iDirtyType];
}

void LC_ClientCurrencyTradeManager::SetDirtyFlagRevisionID(uint32_t lRevisionID, DirtyType eDirtyType)
{
	m_lDirtyFlagRevisionID[eDirtyType] = lRevisionID;
}

void LC_ClientCurrencyTradeManager::ClearDirtyFlagRevisionID()
{	
	for (int i = 0; i < MAX_DIRTY_TYPE_NUM; ++i)
	{
		m_lDirtyFlagRevisionID[i] = 0;
	}
}

void LC_ClientCurrencyTradeManager::ResetTradeRecordPageLoadFlag()
{
	for(int i = 0; i < MAX_TRADE_RECORD_PAGES_NUM; ++i)
	{
		m_bRecordPageLoadFlag[i] = false;
	}
}

StringType LC_ClientCurrencyTradeManager::GetCurrencyTipString( int iGold, int iSilver, int iCopper )
{
	StringType strTip = "";
	char cTipBuffer[32];

	sprintf_s(cTipBuffer, sizeof(cTipBuffer), "%I32d金", iGold * 10000 + iSilver * 100 + iCopper);
	strTip = strTip + cTipBuffer;

	return strTip;
}
