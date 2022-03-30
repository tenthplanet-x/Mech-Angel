

#include "TS_TranAuctionBuyTran.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerPlayer.h"
#include "NW_Helper.h"
#include "LC_PackStruct.h"
#include "MG_CS_Auction.h"
#include "MG_Game2Group.h"
#include "GameServerApp.h"
#include "LC_SequenceManager.h"
#include "UT_OperateLog.h"

using namespace OPERATELOG;
using namespace Protocol;
using namespace Utility;
using namespace GameLogic;

AO_Server_ReqAuctionBuy::AO_Server_ReqAuctionBuy()
{
	
}

emTranOperate AO_Server_ReqAuctionBuy::Execute(Protocol::MG_BODY* pkMsg, uint32_t uiID)
{
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer( GetCharID() );
	if( !pkPlayer )
	{
		SetResultID(RE_FAIL);
		return TRAN_OP_FAILED;
	}

	MG_CS_REQ_Auction_Buy* pkReqAutionBuy = static_cast<MG_CS_REQ_Auction_Buy*>(pkMsg);
	if ( !pkReqAutionBuy )
	{
		SetResultID(RE_FAIL);
		return TRAN_OP_FAILED;
	}

	if (!pkPlayer->IsCashEnough(pkReqAutionBuy->m_kCash, pkReqAutionBuy->m_kCashType))
	{
		SetResultID(RE_AUCTIONBUY_FAILED_CASH_NOT_ENOUGH);
		return TRAN_OP_FAILED;
	}

	{

		MG_GG_REQ_Auction_Buy kReqBuyMsg;

		kReqBuyMsg.m_kAuctionUID = pkReqAutionBuy->m_kAuctionUID;
		STRNCPY(kReqBuyMsg.m_szCharName, pkPlayer->GetOwnerCharName().c_str(), sizeof(kReqBuyMsg.m_szCharName));


		m_kAuctionUID = kReqBuyMsg.m_kAuctionUID;
		kReqBuyMsg.m_kCash = pkReqAutionBuy->m_kCash;
		kReqBuyMsg.m_kCashType = pkReqAutionBuy->m_kCashType;
		kReqBuyMsg.m_iCount = pkReqAutionBuy->m_iCount;
		pkPlayer->ReduceCash(pkReqAutionBuy->m_kCash, (CashType)(pkReqAutionBuy->m_kCashType), SYS_FUNC_TP_AUCTION_BUY, kReqBuyMsg.m_kAuctionUID);

		m_kCashs[pkReqAutionBuy->m_kCashType] = pkReqAutionBuy->m_kCash;

		SetContentFlag(true);	

		SendMsgToTrade(MGPT_SS_REQ_AUCTION_BUY, &kReqBuyMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance(), uiID);
		
		SetResultID(RE_SUCCESS);
		return TRAN_OP_NEXT;	
	}	
}

void AO_Server_ReqAuctionBuy::RespondRltMsg()
{	 
	MG_CS_RLT_Aution_Buy kRltMsg;
	kRltMsg.m_iResultType = GetResultID();

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer( GetCharID() );
	if( pkPlayer )
	{
		pkPlayer->SendMsgToClient(MGPT_RLT_AUCTION_BUY, &kRltMsg);		
	}
}

void AO_Server_ReqAuctionBuy::Rollback()
{
	if ( GetContentFlag() )
	{
		LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer( GetCharID() );
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "ReqAuctionBuy UserID, CharID",  GetUserID(), GetCharID());
		TRADE_CASH_TYPES(_kCashTypes, _iCashSize);
		for (int i=0; i<_iCashSize; ++i)
		{
			CashType iCashType = (CashType)_kCashTypes[i];
			if (pkPlayer && m_kCashs[iCashType])
			{
				GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "AO_Server_ReqAuctionBuy buy cash:",m_kAuctionUID.data(), iCashType, m_kCashs[iCashType]);	
				//pkPlayer->AddCash(m_kCashs[iCashType], iCashType, SYS_FUNC_TP_AUCTION_BUY_ROLL, 0);
			}
		}
	}
}

void AO_Server_ReqAuctionBuy::Reset()
{
	TS_AtomOperate::Reset();
	m_kItemEntry.clear();
	m_kCashs.clear();
}

//////////////////////////////////////////////////////////////////////////
emTranOperate AO_Server_RltAuctionBuy::Execute(Protocol::MG_BODY* pkMsg, uint32_t uiID)
{
	MG_GG_RLT_Auction_Buy* pkRltMsg = static_cast<MG_GG_RLT_Auction_Buy*>(pkMsg);

	if (AUCTION_NOTIFY_SUCESS == pkRltMsg->m_iResultType)
	{
		m_kAuctionUID	= pkRltMsg->m_kAuctionUID;
		m_kCashs[pkRltMsg->m_kCashType] = pkRltMsg->m_kCash;
		BuySuccess(pkMsg, uiID);
		SetResultID(AUCTION_NOTIFY_SUCESS);	

		//RespondRltMsg();
		MG_CS_RLT_Aution_Buy rltMsg;	
		rltMsg.m_iResultType = GetResultID();

		rltMsg.m_kAuctionUID = m_kAuctionUID;
		TRADE_CASH_TYPES(_kCashTypes, _iCashSize);
		for (int i=0; i<_iCashSize; ++i)
		{
			CashType iCashType = (CashType)_kCashTypes[i];
			rltMsg.m_kCash[iCashType] = m_kCashs[iCashType];
		}
		rltMsg.m_iCount = pkRltMsg->m_iCount;

		LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer( GetCharID() );
		if( pkPlayer )
		{
			pkPlayer->SendMsgToClient(MGPT_RLT_AUCTION_BUY, &rltMsg);	
		}
		return TRAN_OP_NEXT;
	}
	int nResult = pkRltMsg->m_iResultType;
	switch (pkRltMsg->m_iResultType)
	{
	case AUCTION_NOTIFY_BUY_FAILED_24:
		{
			nResult = RE_NEW_AUCTION_BUY_FAILED_10226;
		}
		break;
	case AUCTION_NOTIFY_BUY_FAILED_25:
		{
			nResult = RE_NEW_AUCTION_BUY_FAILED_10227;
		}
		break;
	case AUCTION_NOTIFY_BUY_FAILED_26:
		{
			nResult = RE_NEW_AUCTION_BUY_FAILED_10228;
		}
		break;
	case AUCTION_NOTIFY_BUY_FAILED_27:
		{
			nResult = RE_NEW_AUCTION_BUY_FAILED_10225;
		}
		break;
	case AUCTION_NOTIFY_BUY_FAILED_28:
		{
			nResult = RE_NEW_AUCTION_BUY_FAILED_10229;
		}
		break;
	case AUCTION_NOTIFY_BUY_FAILED_29:
		{
			nResult = RE_NEW_AUCTION_BUY_FAILED_10230;
		}
		break;
	case AUCTION_NOTIFY_DATAERR_FAILED:
		{
			nResult = RE_NEW_AUCTION_NOTSELL_FAILED_10223;
		}
		break;
	case AUCTION_NOTIFY_DBERR_FAILED:
		{
			nResult = RE_NEW_AUCTION_DBERR_FAILED_10231;
		}
		break;
	}
	SetResultID(nResult);
	return TRAN_OP_FAILED;	
}



void AO_Server_RltAuctionBuy::BuySuccess(Protocol::MG_BODY* pkMsg, uint32_t uiID)
{
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer( GetCharID() );
	if( pkPlayer )
	{
		MG_GG_RLT_Auction_Buy* pkRltMsg = static_cast<MG_GG_RLT_Auction_Buy*>(pkMsg);
		if (pkRltMsg )
		{

			Protocol::PS_ItemAssetEntry& rkEntry = pkRltMsg->m_kItemAssetEntry;
			int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, SELL_OPERATE, OPERATE_LOGTYPE3);
			LC_BackPackEntry kTempEntry;
			kTempEntry.GetFromProtocol(rkEntry);
			GfxWriteLog(LOG_SYSTEM_FILELINE, LOG_SWITCH_INFO, "Auction BuySuccess %llu %llu ", GetCharID(), pkRltMsg->m_kAuctionUID);
			kTempEntry.WriteToLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM);
			GfxWriteLog(LOG_SYSTEM_FILELINE, LOG_SWITCH_INFO, "Auction BuySuccess End");
			int itemID = kTempEntry.GetBackPackValueByType(LPSFET_TYPE_ID);
			if (itemID == SPEC_ITEM_TID_CASH)
			{
				pkPlayer->AddCash(kTempEntry.GetBackPackValueByType(LPSFET_OVERLAP_COUNT), CT_UNBIND_CASH, SYS_FUNC_TP_MAIL_SEND,0 );
			}
			if (itemID == SPEC_ITEM_TID_YUANBAO)
			{
				pkPlayer->AddCash(kTempEntry.GetBackPackValueByType(LPSFET_OVERLAP_COUNT), CT_UNBIND_YUANBAO, SYS_FUNC_TP_MAIL_SEND, INVALID_MAIL_ID);
			}
			kTempEntry.SetBackPackValueByType(LPSFET_UNIQUE_ID_0, 0);
			kTempEntry.SetBackPackValueByType(LPSFET_UNIQUE_ID_1, 0);
			kTempEntry.SetBackPackValueByType(LPSFET_UNIQUE_ID_2, 0);
			kTempEntry.SetBackPackValueByType(LPSFET_UNIQUE_ID_3, 0);
			kTempEntry.SetBackPackValueByType(LPSFET_UNIQUE_ID_4, 0);
			
			PackOperateInfoList objOPList;
			objOPList.clear();
			PackOperateInfo objData;
			objData.Reset();

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
				UT_ServerHelper::SendToMail("221;", "222;", pkPlayer->GetInstance(), pkPlayer->GetOwnerCharName(), 0, kEntryList, "", nLogCode);
			}
			else
			{
				// 成功，就显示响应消息，并通知
				pkPlayer->SyncBackPackToClient(PACK_TYPE_BASIC);
				PackOPRecordSet * record_set = pkPlayer->GetPackAsset()->GetRecordSet();
				pkPlayer->HandlePackOPRecord(record_set, SYS_FUNC_TP_AUCTION_BUY, 0);
			}
		}
	}

}

void AO_Server_RltAuctionBuy::RespondRltMsg()
{
	MG_CS_RLT_Aution_Buy rltMsg;	
	rltMsg.m_iResultType = GetResultID();

	rltMsg.m_kAuctionUID = m_kAuctionUID;
	TRADE_CASH_TYPES(_kCashTypes, _iCashSize);
	for (int i=0; i<_iCashSize; ++i)
	{
		CashType iCashType = (CashType)_kCashTypes[i];
		rltMsg.m_kCash[iCashType] = m_kCashs[iCashType];
	}

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer( GetCharID() );
	if( pkPlayer )
	{
		pkPlayer->SendMsgToClient(MGPT_RLT_AUCTION_BUY, &rltMsg);	
	}
}

void AO_Server_RltAuctionBuy::Reset()
{
	m_kAuctionUID = 0;
	m_kCashs.clear();
}

//////////////////////////////////////////////////////////////////////////
TS_Server_TranAuctionBuyTran::TS_Server_TranAuctionBuyTran()
{
	m_kStepTranMap[STEP_REQ_GG_AUCTIONBUY] = T_NEW_D AO_Server_ReqAuctionBuy;
	m_kStepTranMap[STEP_RLT_GG_AUCTIONBUY] = T_NEW_D AO_Server_RltAuctionBuy;
}

TS_Server_TranAuctionBuyTran::~TS_Server_TranAuctionBuyTran()
{
	for (StepAtomMapIter it = m_kStepTranMap.begin(); it != m_kStepTranMap.end(); ++it)
	{
		T_SAFE_DELETE( it->second);
	}

	m_kStepTranMap.clear();
}