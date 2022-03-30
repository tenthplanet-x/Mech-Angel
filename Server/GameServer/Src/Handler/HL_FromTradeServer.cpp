#include "LM_LocaleManager.h"
#include "GameServerApp.h"

#include "HL_PacketHandler.h"

#include "LC_ServerPlayer.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerWorldManager.h"

#include "TS_TranManager.h"
#include "TS_TranType.h"

#include "TS_TranAuctionSellTran.h"
#include "TS_TranAuctionBuyTran.h"

#include "NW_Helper.h"
#include "NW_ServerSession.h"

#include "MG_CS_Auction.h"
#include "MG_CS_CurrencyTradeProtocol.h"
#include "LC_SequenceManager.h"
#include "MG_Common.h"
#include "CF_ItemList.h"
#include "MG_Game2Group.h"
#include "UT_OperateLog.h"
#include "GlobalSettings.h"

using namespace OPERATELOG;
using namespace Protocol;
using namespace GameLogic;
using namespace Utility;

//----------------------------------------------------------------------------
namespace Protocol
{
#define this_source_type SERVER_TYPE_TRADE

//----------------------------------------------------------------------------
ON_HANDLE(MGPT_SS_RLT_AUCTION_SELL, this_source_type, false)
{
	MG_GG_RLT_Auction_Sell kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	TS_TranManager* pkTranManager = TS_TranManager::GetSingletonPtr();
	if (!pkTranManager)
	{
		return;
	}

	TS_TranBase* pkTran = pkTranManager->GetTran(rkMsgHead.m_iSequence);
	if (!pkTran)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_GG_RLT_Auction_Sell error1");
		return;
	}

	if (pkTran->GetStep() != TS_Server_AuctionSellTran::STEP_RLT_GG_AUCTIONSELL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_GG_RLT_Auction_Sell error2");
	}
	else
	{
		pkTran->HandlerMsg(rkMsgHead, &kMsg);
	}
	pkTranManager->UnRegisteTran(pkTran->GetID());
}
//----------------------------------------------------------------------------
ON_HANDLE(MGPT_SS_RLT_AUCTION_BUY, this_source_type, false)
{
	MG_GG_RLT_Auction_Buy kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	TS_TranManager* pkTranManager = TS_TranManager::GetSingletonPtr();
	if (!pkTranManager)
	{
		return;
	}

	TS_TranBase* pkTran = pkTranManager->GetTran(rkMsgHead.m_iSequence);
	if (!pkTran)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_GG_RLT_Auction_Sell error3");
		return;
	}

	if (pkTran->GetStep() != TS_Server_TranAuctionBuyTran::STEP_RLT_GG_AUCTIONBUY)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_GG_RLT_Auction_Sell error4");
	}
	else
	{
		pkTran->HandlerMsg(rkMsgHead, &kMsg);
	}

	pkTranManager->UnRegisteTran(pkTran->GetID());
}
//----------------------------------------------------------------------------
ON_HANDLE(MGPT_SS_RLT_AUCTION_CANCEL, this_source_type, false)
{
	MG_GG_RLT_Aution_Cancel kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_CS_RLT_Aution_Cancel kRltMsg;
	kRltMsg.m_uiAuctionID = kMsg.m_lAuctionID;
	if (AUCTION_NOTIFY_SUCESS == kMsg.m_iResultType)
	{
		kRltMsg.m_iResultType = RE_AUCTIONSELLCANCEL_SUCCESS;
		//返还道具
		Protocol::PS_ItemAssetEntry& rkEntry = kMsg.m_kItemAssetEntry;
		int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, SELL_OPERATE, OPERATE_LOGTYPE2);
		LC_BackPackEntry kTempEntry;
		kTempEntry.GetFromProtocol(rkEntry);
		GfxWriteLog(LOG_SYSTEM_FILELINE, LOG_SWITCH_INFO, "AUCTION_CANCEL",rkMsgHead.m_uiCharID, kMsg.m_lAuctionID);
		kTempEntry.WriteToLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM);
		GfxWriteLog(LOG_SYSTEM_FILELINE, LOG_SWITCH_INFO, "AUCTION_CANCEL End");
		if (kTempEntry.GetBackPackValueByType(LPSFET_TYPE_ID) == SPEC_ITEM_TID_CASH)
		{
			pkPlayer->AddCash(kTempEntry.GetBackPackValueByType(LPSFET_OVERLAP_COUNT), CT_UNBIND_CASH, SYS_FUNC_TP_AUCTION_BEGIN, 0);
		}
		if (kTempEntry.GetBackPackValueByType(LPSFET_TYPE_ID) == SPEC_ITEM_TID_YUANBAO)
		{
			pkPlayer->AddCash(kTempEntry.GetBackPackValueByType(LPSFET_OVERLAP_COUNT), CT_UNBIND_YUANBAO, SYS_FUNC_TP_AUCTION_BEGIN, 0);
		}
		PackOperateInfoList objOPList;
		objOPList.clear();
		PackOperateInfo objData;
		objData.Reset();
		kTempEntry.SetBackPackValueByType(LPSFET_UNIQUE_ID_0, 0);
		kTempEntry.SetBackPackValueByType(LPSFET_UNIQUE_ID_1, 0);
		kTempEntry.SetBackPackValueByType(LPSFET_UNIQUE_ID_2, 0);
		kTempEntry.SetBackPackValueByType(LPSFET_UNIQUE_ID_3, 0);
		kTempEntry.SetBackPackValueByType(LPSFET_UNIQUE_ID_4, 0);

		objData.m_iOPType = OPT_ADD_ITEM;
		objData.m_bOPEntry = true;
		objData.m_bMerger  = true;
		objData.m_iPackType  = PACK_TYPE_BASIC;
		objData.m_kPackEntry.Clone(kTempEntry);
		objOPList.push_back(objData);

		ResultType iResult = pkPlayer->GetPackAsset()->ExecutePackOP(objOPList,true);
		if (iResult != RE_SUCCESS)
		{
			LC_BackPackEntryList kEntryList;
			kEntryList.push_back(kTempEntry);
			UT_ServerHelper::SendToMail("223;", "224;", pkPlayer->GetInstance(), pkPlayer->GetOwnerCharName(), 0, kEntryList, "", nLogCode);
		}
		else
		{
			// 成功，就显示响应消息，并通知
			pkPlayer->SyncBackPackToClient(PACK_TYPE_BASIC);
			PackOPRecordSet * record_set = pkPlayer->GetPackAsset()->GetRecordSet();
			pkPlayer->HandlePackOPRecord(record_set, SYS_FUNC_TP_AUCTION_BEGIN, 0);
		}
	}
	else
	{
		kRltMsg.m_iResultType = RE_NEW_AUCTION_BUY_FAILED_10226;
	}
	
	pkPlayer->SendMsgToClient(MGPT_RLT_AUCTION_CANCEL, &kRltMsg);
	
}
//----------------------------------------------------------------------------
ON_HANDLE(MGPT_SS_RLT_AUCTION_LIST, this_source_type, false)
{
	MG_GG_RLT_Auction_List kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_AuctionList kRltMsg;
	kRltMsg.m_byReqType			= kMsg.m_cReqType;
	kRltMsg.m_byAuctionCount	= kMsg.m_cAuctionCount;
	kRltMsg.m_uiMore	= kMsg.m_uiMore;
	if (kRltMsg.m_byAuctionCount > AUCTION_PER_MSG)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_GG_RLT_Auction_List error1");
		return;
	}

	for (int i = 0; i < kRltMsg.m_byAuctionCount; ++i)
	{
		kRltMsg.m_Auction[i] = kMsg.m_Auction[i];
	}

	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (pkPlayer)
	{
		pkPlayer->SendMsgToClient(MGPT_RLT_AUCTION_LIST, &kRltMsg);
	}
}

ON_HANDLE(MGPT_SS_RLT_PRE_AUCTION_SELL, this_source_type, false)
{
	MG_GG_RLT_PreAuction_Sell kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}
	if (kRltMsg.m_uiResult != AUCTION_NOTIFY_SUCESS)
	{
		MG_CS_RLT_Aution_Sell kRltMsg2C;
		if(kRltMsg.m_uiResult == AUCTION_NOTIFY_SELL_FAILED_40)
		{
			kRltMsg2C.m_iResultType = RE_NEW_AUCTION_SELL_FAILED_10233;
		}
		else if(kRltMsg.m_uiResult == AUCTION_NOTIFY_SELL_FAILED_41)
		{
			kRltMsg2C.m_iResultType = RE_NEW_AUCTION_SELL_FAILED_10234;
		}	
		else if(kRltMsg.m_uiResult == AUCTION_NOTIFY_SELL_FAILED_42)
		{
			kRltMsg2C.m_iResultType = RE_NEW_AUCTION_SELL_FAILED_10235;
		}
		pkPlayer->SendMsgToClient(MGPT_RLT_AUCTION_SELL, &kRltMsg2C);
		return;
	}

	
	TS_TranManager* pkTranManager = TS_TranManager::GetSingletonPtr();
	if (!pkTranManager)
	{
		return;
	}

	TS_TranBase* pkTran = pkTranManager->RegisteTran(LC_SequenceManager::Instance().GetSequenceID(), TRAN_TYPE_AUCTION_SELL_TRAN, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID);
	if (pkTran)
	{
		if (pkTran->GetStep() != TS_Server_AuctionSellTran::STEP_REQ_GG_AUCTIONSELL)
		{
			GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_GG_RLT_PreAuction_Sell error1!");
			pkTranManager->UnRegisteTran(pkTran->GetID());
			return;
		}

		SS_MG_HEAD kSSMsgHead;
		kSSMsgHead.m_usMsgType = MGPT_REQ_AUCTION_SELL;
		kSSMsgHead.m_uiUserID = rkMsgHead.m_uiUserID;
		kSSMsgHead.m_uiCharID = rkMsgHead.m_uiCharID;

		MG_CS_REQ_Auction_Sell kMsg;
		kMsg.m_kClientActionSell = pkPlayer->GetAuctionSellData();

		ResultType iResultType = pkTran->HandlerMsg(kSSMsgHead, &kMsg);
		if (RE_FAIL == iResultType)
		{
			pkTranManager->UnRegisteTran(pkTran->GetID());
		}
	}
}
ON_HANDLE(MGPT_SS_RLT_PRE_AUCTION_BUY, this_source_type, false)
{
	MG_GG_RLT_PreAuction_Buy kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}

	if (kRltMsg.m_iResultType != AUCTION_NOTIFY_SUCESS)
	{
		MG_CS_RLT_Aution_Buy rltMsg;
		rltMsg.m_kAuctionUID = kRltMsg.m_kAuctionUID;
		//rltMsg.m_kCash	= kRltMsg.m_kCash;
		switch (kRltMsg.m_iResultType)
		{
			case AUCTION_NOTIFY_BUY_FAILED_24:
				{
					rltMsg.m_iResultType = RE_NEW_AUCTION_BUY_FAILED_10226;
				}
				break;
			case AUCTION_NOTIFY_BUY_FAILED_25:
				{
					rltMsg.m_iResultType = RE_NEW_AUCTION_BUY_FAILED_10227;
				}
				break;
			case AUCTION_NOTIFY_BUY_FAILED_26:
				{
					rltMsg.m_iResultType = RE_NEW_AUCTION_BUY_FAILED_10228;
				}
				break;
			case AUCTION_NOTIFY_BUY_FAILED_27:
				{
					rltMsg.m_iResultType = RE_NEW_AUCTION_BUY_FAILED_10225;
				}
				break;
			case AUCTION_NOTIFY_BUY_FAILED_28:
				{
					rltMsg.m_iResultType = RE_NEW_AUCTION_BUY_FAILED_10229;
				}
				break;
			case AUCTION_NOTIFY_BUY_FAILED_29:
				{
					rltMsg.m_iResultType = RE_NEW_AUCTION_BUY_FAILED_10230;
				}
				break;
			case AUCTION_NOTIFY_DATAERR_FAILED:
				{
					rltMsg.m_iResultType = RE_NEW_AUCTION_NOTSELL_FAILED_10223;
				}
				break;
			case AUCTION_NOTIFY_DBERR_FAILED:
				{
					rltMsg.m_iResultType = RE_NEW_AUCTION_DBERR_FAILED_10231;
				}
				break;
			default:
				{
					rltMsg.m_iResultType = kRltMsg.m_iResultType;
				}
				break;
		}
		pkPlayer->SendMsgToClient(MGPT_RLT_AUCTION_BUY, &rltMsg);
		return;
	}

	if (pkPlayer->GetAuctionBuyID() != kRltMsg.m_kAuctionUID)
	{
		return;
	}

	{
		if (pkPlayer->GetAuctionBuyPrice(kRltMsg.m_kCashType) != kRltMsg.m_kCash)
		{
			return;
		}
	}

	TS_TranManager* pkTranManager = TS_TranManager::GetSingletonPtr();
	if (!pkTranManager)
	{
		return;
	}

	TS_TranBase* pkTran = pkTranManager->RegisteTran(LC_SequenceManager::Instance().GetSequenceID(), TRAN_TYPE_AUCTION_BUY_TRAN, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID);
	if (pkTran)
	{
		if (pkTran->GetStep() != TS_Server_TranAuctionBuyTran::STEP_REQ_GG_AUCTIONBUY)
		{
			GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_GG_RLT_PreAuction_Sell error2!");
			pkTranManager->UnRegisteTran(pkTran->GetID());
			return;
		}

		MG_CS_REQ_Auction_Buy kMsg;
		kMsg.m_kAuctionUID = pkPlayer->GetAuctionBuyID();
		pkPlayer->SetAuctionBuyPrice(kRltMsg.m_kCashType, 0);

		// Reset
		pkPlayer->SetAuctionBuyID(0);

		SS_MG_HEAD kSSMsgHead;
		kSSMsgHead.m_usMsgType = rkMsgHead.m_usMsgType;
		kSSMsgHead.m_uiUserID = rkMsgHead.m_uiUserID;
		kSSMsgHead.m_uiCharID = rkMsgHead.m_uiCharID;
		kMsg.m_kCash = kRltMsg.m_kCash;
		kMsg.m_iCount = kRltMsg.m_iCount;
		kMsg.m_kCashType = kRltMsg.m_kCashType;

		ResultType iResultType = pkTran->HandlerMsg(kSSMsgHead, &kMsg);
		if (RE_FAIL == iResultType)
		{
			pkTranManager->UnRegisteTran(pkTran->GetID());
		}
	}
}

//----------------------------------------------------------------------------
ON_HANDLE(MGPT_SS_RLT_AUCTION_GETAWARD, this_source_type, false)
{
	MG_GG_RLT_AuctionGetAward kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if(NULL == pkPlayer)
	{
		return;
	}
	MG_RLT_AuctionGetAward kRltMsg;
	kRltMsg.m_iResultType = kMsg.m_iResultType;
	kRltMsg.m_byReqType = kMsg.m_byReqType;
	if(kMsg.m_iResultType != AUCTION_NOTIFY_SUCESS)
	{
		if(AUCTION_NOTIFY_DATAERR_FAILED == kMsg.m_iResultType)
		{
			kRltMsg.m_iResultType = RE_NEW_AUCTION_DATAERR_FAILED_10231;
		}
		if(AUCTION_NOTIFY_DBERR_FAILED == kMsg.m_iResultType)
		{
			kRltMsg.m_iResultType = RE_NEW_AUCTION_DBERR_FAILED_10231;
		}
		
		pkPlayer->SendMsgToClient(MGPT_RLT_AUCTION_GETAWARD, &kRltMsg);
		return;
	}

	kRltMsg.m_nAuctionCount = kMsg.m_nAuctionCount;
	for (int i = 0; i < kRltMsg.m_nAuctionCount; ++i)
	{
		kRltMsg.m_Auction[i] = kMsg.m_Auction[i];
	}
	int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, SELL_OPERATE, OPERATE_LOGTYPE4);
	TRADE_CASH_TYPES(_kCashTypes, _iCashSize);
	for (int i=0; i<_iCashSize; ++i)
	{
		CashType iCashType = (CashType)_kCashTypes[i];
		if(kMsg.m_kCash[iCashType] > 0) 
		{
			double nValue = kMsg.m_kCash[iCashType];
			double dTax = (nValue * (double)(GetGlobalSetting.nAuctionTax)) / (double)(10000);
			int nTax = ceil(dTax);
			if(nTax <= 0)
				nTax = 1;
			nValue -= nTax;
			kRltMsg.m_kCash[iCashType] = nValue;
			pkPlayer->AddCash(nValue, iCashType, SYS_FUNC_TP_AUCTION_BEGIN,0, 0, true, NULL, nLogCode);
		}
	}
	
	pkPlayer->SendMsgToClient(MGPT_RLT_AUCTION_GETAWARD, &kRltMsg);
	
}



//----------------------------------------------------------------------------
ON_HANDLE(MGPT_SS_NOTICE_AUCTION, this_source_type, false)
{
	MG_GG_RLT_NOTICE kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if(NULL == pkPlayer)
	{
		return;
	}
	MG_CS_RLT_Aution_NOTICE kRltMsg;
	kRltMsg.m_iCount = kMsg.m_iCount;

	if( kMsg.m_iItemId)
	{
		CSVFile::CF_ItemList::DataEntry* entry = CSVFile::SafeGetCSVFileDataEntryPtr<CSVFile::CF_ItemList>( kMsg.m_iItemId, false, false);
		if (entry)
		{
			UT_ServerHelper::_GameNoticeFmort noticeFmort;
			noticeFmort.SetMsgID(46);
			noticeFmort.AddItemName( kMsg.m_iItemId);
			UT_ServerHelper::BrocastNoticeMsg(noticeFmort, pkPlayer->GetCitizenship(), pkPlayer);
		}
	}
	
	//kRltMsg.m_iItemId = kMsg.m_iItemId;
	pkPlayer->SendMsgToClient(MGPT_RLT_AUCTION_NOTICE, &kRltMsg);

}

ON_HANDLE(MGPT_SS_RLT_BUY_YUANBAO, this_source_type, false)
{
	MG_GG_RLT_Buy_Yuanbao kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	// Quick Buy
	if (kMsg.m_iType == EXCHANGE_IMMEDIATE)
	{
		MG_RLT_CurrencyTradeQuickBuy kRltMsg;
		kRltMsg.m_iResult			= kMsg.m_iResultType;
		kRltMsg.m_llRequestCount	= kMsg.m_llCount;
		kRltMsg.m_llRequestPrice	= kMsg.m_llUnitPrice;


		LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);

		if (pkPlayer != NULL)
		{
			pkPlayer->SendMsgToClient(MGPT_RLT_CURRENCY_QUICK_BUY, &kRltMsg);
		}
	}

	// Commission Buy
	if (kMsg.m_iType == EXCHANGE_NORMAL)
	{
		MG_RLT_CurrencyTradeCommissionBuy kRltMsg;
		kRltMsg.m_iResult			= kMsg.m_iResultType;
		kRltMsg.m_llRequestCount	= kMsg.m_llCount;
		kRltMsg.m_llRequestPrice	= kMsg.m_llUnitPrice;
		kRltMsg.m_llDealCount		= kMsg.m_llDealCount;
		kRltMsg.m_llDealPrice		= kMsg.m_llDealPrice;

		LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
		if (pkPlayer != NULL)
		{
			pkPlayer->SendMsgToClient(MGPT_RLT_CURRENCY_COMMISSION_BUY, &kRltMsg);
		}
	}
}

ON_HANDLE(MGPT_SS_RLT_SELL_YUANBAO, this_source_type, false)
{
	MG_GG_RLT_Sell_Yuanbao kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	// 快速卖出
	if (kMsg.m_iType == EXCHANGE_IMMEDIATE)
	{
		MG_RLT_CurrencyTradeQuickSell kRltMsg;
		kRltMsg.m_iResult			= kMsg.m_iResultType;
		kRltMsg.m_llRequestCount	= kMsg.m_llCount;
		kRltMsg.m_llRequestPrice	= kMsg.m_llUnitPrice;

		LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
		if (pkPlayer != NULL)
		{
			pkPlayer->SendMsgToClient(MGPT_RLT_CURRENCY_QUICK_SELL, &kRltMsg);
		}
	}

	// 委托卖出
	if (kMsg.m_iType == EXCHANGE_NORMAL)
	{
		MG_RLT_CurrencyTradeCommissionSell kRltMsg;
		kRltMsg.m_iResult			= kMsg.m_iResultType;
		kRltMsg.m_llRequestCount	= kMsg.m_llCount;
		kRltMsg.m_llRequestPrice	= kMsg.m_llUnitPrice;
		kRltMsg.m_llDealCount		= kMsg.m_llDealCount;
		kRltMsg.m_llDealPrice		= kMsg.m_llDealPrice;

		LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
		if (pkPlayer != NULL)
		{
			pkPlayer->SendMsgToClient(MGPT_RLT_CURRENCY_COMMISSION_SELL, &kRltMsg);
		}
	}
}

ON_HANDLE(MGPT_SS_RLT_EXCHANGEVIEWLIST, this_source_type, false)
{
	MG_GG_RLT_ExchangeViewList kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_CurrencyQuickTradeList kRltMsg;
	kRltMsg.m_iResult = kMsg.m_iResultType;

	for (int32_t i = 0; i < MAX_EXCHANGE_VIEW_LIST; ++i)
	{
		// 出售列表
		if (kMsg.m_stSellList[i].m_uiCount != 0)
		{
			kRltMsg.m_stSellList[i].m_iUnitPrice = kMsg.m_stSellList[i].m_iUnitPrice;
			kRltMsg.m_stSellList[i].m_uiCount = kMsg.m_stSellList[i].m_uiCount;
			kRltMsg.m_llSellItemCount ++;
		}
		// 求购列表
		if (kMsg.m_stBuyList[i].m_uiCount != 0)
		{
			kRltMsg.m_stBuyList[i].m_iUnitPrice = kMsg.m_stBuyList[i].m_iUnitPrice;
			kRltMsg.m_stBuyList[i].m_uiCount = kMsg.m_stBuyList[i].m_uiCount;
			kRltMsg.m_llBuyItemCount ++;
		}
	}
	// 账户钱币和元宝
	kRltMsg.m_llMoney	= kMsg.m_llMoney;
	kRltMsg.m_llYuanBao	= kMsg.m_llYuanBao;

	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (pkPlayer != NULL)
	{
		pkPlayer->SendMsgToClient(MGPT_RLT_CURRENCY_QUICK_TRADE_LIST, &kRltMsg);
	}
}

ON_HANDLE(MGPT_SS_RLT_EXCHANGELIST, this_source_type, false)
{
	MG_GG_RLT_ExchangeList kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	if (kMsg.m_iCount < 0)
	{
		return;
	}

	MG_RLT_CurrencyCommissionItemList kRltMsg;
	kRltMsg.m_iCount = kMsg.m_iCount;

	if (kRltMsg.m_iCount < 0 || kRltMsg.m_iCount >= MAX_EXCHANGE_LIST_PER_MSG)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_RLT_CurrencyCommissionItemList error1");
		return;
	}

	for (int32_t i = 0; i < kMsg.m_iCount; ++i)
	{
		kRltMsg.m_stExchangeList[i].m_iDealPrice	= kMsg.m_stExchangeList[i].m_iDealPrice;		// 成交的价格
		kRltMsg.m_stExchangeList[i].m_iType			= kMsg.m_stExchangeList[i].m_iType;
		kRltMsg.m_stExchangeList[i].m_iUnitPrice	= kMsg.m_stExchangeList[i].m_iUnitPrice;
		kRltMsg.m_stExchangeList[i].m_uiAutoid		= kMsg.m_stExchangeList[i].m_uiAutoid;
		kRltMsg.m_stExchangeList[i].m_uiCount		= kMsg.m_stExchangeList[i].m_uiCount;
		kRltMsg.m_stExchangeList[i].m_uiDealCount	= kMsg.m_stExchangeList[i].m_uiDealCount;		// 交易数量

		// 计算剩余时间：当前时间 - 委托时间戳
		time_t ltime = time(0);
		tm tDealtime;
		localtime_s(&tDealtime, &ltime);
		int32_t iLeftTime = (int32_t)(ltime - kMsg.m_stExchangeList[i].m_uiTime);
		uint32_t uiLeftTime = (iLeftTime > 0) ? iLeftTime : 0;

		// 挂单时间最长 MAX_CURRENCY_COMMISSION_TIME
		kRltMsg.m_stExchangeList[i].m_uiTime = (MAX_CURRENCY_COMMISSION_TIME - uiLeftTime) / 3600;
	}

	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (pkPlayer != NULL)
	{
		pkPlayer->SendMsgToClient(MGPT_RLT_CURRENCY_COMMISSION_ITEM_LIST, &kRltMsg);
	}
}

ON_HANDLE(MGPT_SS_RLT_EXCHANGEHISTORY, this_source_type, false)
{
	MG_GG_RLT_ExchangeHistory kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_CurrencyTradeRecordList kRltMsg;
	kRltMsg.m_iPage		= kMsg.m_iPage;
	kRltMsg.m_iCount	= kMsg.m_iCount;
	kRltMsg.m_iResult	= kMsg.m_iResultType;
	for (int32_t i = 0; i < kMsg.m_iCount; ++i)
	{
		kRltMsg.m_stExchangeHistory[i].m_iDealPrice		= kMsg.m_stExchangeHistory[i].m_iDealPrice;			// 交易价格
		kRltMsg.m_stExchangeHistory[i].m_uiSellID		= kMsg.m_stExchangeHistory[i].m_uiSellID;			// 出售者ID
		kRltMsg.m_stExchangeHistory[i].m_uiBuyID		= kMsg.m_stExchangeHistory[i].m_uiBuyID;			// 购买者ID
		kRltMsg.m_stExchangeHistory[i].m_uiExchangeID	= kMsg.m_stExchangeHistory[i].m_uiExchangeID;		// 交易记录单号
		kRltMsg.m_stExchangeHistory[i].m_uiDealCount	= kMsg.m_stExchangeHistory[i].m_uiDealCount;		// 交易数量
		kRltMsg.m_stExchangeHistory[i].m_uiTime			= kMsg.m_stExchangeHistory[i].m_uiTime;				// 交易时间戳
	}
	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);

	if (pkPlayer != NULL && kRltMsg.m_iCount > 0)
	{
		pkPlayer->SendMsgToClient(MGPT_RLT_CURRENCY_TRADE_RECORD_LIST, &kRltMsg);
	}
}

ON_HANDLE(MGPT_SS_RLT_DEPOSITYUANBAO, this_source_type, false)
{
	MG_GG_RLT_DepositYuanbao kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);

	if (!pkPlayer)
	{
		return;
	}

	MG_RLT_CurrencyYuanBaoDeposit kMsg;
	kMsg.m_iResult			= kRltMsg.m_iResultType;
	kMsg.m_llYuanBaoCount	= kRltMsg.m_llTransYuanBao;
	kMsg.m_llResultYuanBao	= kRltMsg.m_llCurYuanBao;
	pkPlayer->SendMsgToClient(MGPT_RLT_CURRENCY_YUANBAO_DEPOSIT, &kMsg);
}

ON_HANDLE(MGPT_SS_RLT_WITHDRAWYUANBAO, this_source_type, false)
{
	MG_GG_RLT_WithdrawYuanbao kRltMsg;
	if (!kRltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);

	if (!pkPlayer)
	{
		return;
	}

	MoneyType llTransYuanBao = -kRltMsg.m_llTransYuanBao;
	if (!IS_VALID_CHAR_MONEY(llTransYuanBao))
	{
		return;
	}
	MG_REQ_Deposit_Money kReqMsg;
	kReqMsg.m_iBusinessID	 = 1;
	kReqMsg.m_llDeltaYuanBao = llTransYuanBao;
	STRNCPY(kReqMsg.m_szBillingNO, kRltMsg.m_szBillingNO, sizeof(kReqMsg.m_szBillingNO));

	SendMsgToLogin(MGPT_SS_REQ_DEPOSIT_MONEY, &kReqMsg, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID);
}

ON_HANDLE(MGPT_SS_RLT_DEPOSITMONEY, this_source_type, false)
{
	MG_GG_RLT_DepositMoney kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);

	if (!pkPlayer)
	{
		return;
	}

	// 转账钱币为正，转入账户
	if (kMsg.m_llTransMoney > 0)
	{
		MG_RLT_CurrencyMoneyDeposit kRltMsg;
		kRltMsg.m_iResult = kMsg.m_iResultType;
		kRltMsg.m_llMoneyCount	= kMsg.m_llTransMoney;						// 转账钱币数量
		kRltMsg.m_llResultMoney = kMsg.m_llCurMoney;
		pkPlayer->SendMsgToClient(MGPT_RLT_CURRENCY_MONEY_DEPOSIT, &kRltMsg);
	}
}

ON_HANDLE(MGPT_SS_RLT_WITHDRAWMONEY, this_source_type, false)
{
}

ON_HANDLE(MGPT_SS_RLT_EXCHANGECANCEL, this_source_type, false)
{
	MG_GG_RLT_ExchangeCancel kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_CurrencyRevokeCommission kRltMsg;
	kRltMsg.m_iResult			= kMsg.m_iResultType;
	kRltMsg.m_uiCommissionID	= kMsg.m_uiExchangeID;		// 委托交易单号

	LC_PlayerManagerBase* pkPlayerManager = LC_LogicManagerBase::GetSingletonPtr()->GetGamePlayerManager();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);

	if (pkPlayer != NULL)
	{
		pkPlayer->SendMsgToClient(MGPT_RLT_CURRENCY_REVOKE_COMMISSION, &kRltMsg);
	}
}

}


