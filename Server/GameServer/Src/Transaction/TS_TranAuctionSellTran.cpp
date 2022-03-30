

#include "TS_TranAuctionSellTran.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerPlayer.h"
#include "NW_Helper.h"
#include "LC_PackStruct.h"
#include "MG_CS_Auction.h"
#include "MG_Game2Group.h"
#include "GameServerApp.h"
#include "LC_SequenceManager.h"
#include "GlobalSettings.h"
#include "UT_OperateLog.h"

using namespace OPERATELOG;
using namespace Protocol;
using namespace Utility;
using namespace GameLogic;

AO_Server_ReqAuctionSell::AO_Server_ReqAuctionSell()
: m_uiCost(0)
{

}

emTranOperate AO_Server_ReqAuctionSell::Execute(Protocol::MG_BODY* pkMsg, uint32_t uiID)
{
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer( GetCharID() );
	if( !pkPlayer )
	{
		SetResultID(RE_FAIL);
		return TRAN_OP_FAILED;
	}
	
	MG_CS_REQ_Auction_Sell* pkReqAutionSellMsg = static_cast<MG_CS_REQ_Auction_Sell*>(pkMsg);
	if ( !pkReqAutionSellMsg )
	{
		SetResultID(RE_FAIL);
		return TRAN_OP_FAILED;
	}
	if(pkReqAutionSellMsg->m_kClientActionSell.m_uItemCount <= 0)
	{
		SetResultID(RE_FAIL);
		return TRAN_OP_FAILED;
	}
	
	if(GetGlobalSetting.nAuctionCostCount > 0)
	{
		if (!pkPlayer->IsCashEnough(GetGlobalSetting.nAuctionCostCount, CT_UNBIND_CASH))
		{
			SetResultID(RE_NEW_AUCTION_SELL_FAILED_COIN_10238);
			return TRAN_OP_FAILED;
		}
	}
	
	MG_GG_REQ_Auction_Sell kReqAuctionSell;

	int32_t iSequence = LC_SequenceManager::Instance().GetSequenceID();
	//PrepareGameTagString(kReqAuctionSell.m_szBillingNO, sizeof(kReqAuctionSell.m_szBillingNO), GetGameServerID(), int32_t(GET_CURRENT_TIMESTAMP_IN_SECONDS()), iSequence);
	kReqAuctionSell.m_iSellCount = GetGlobalSetting.nAuctionMaxSellCount;
	kReqAuctionSell.m_iServerMaxCount = GetGlobalSetting.nServerMaxSellCount;
	ResultType iResultType = pkPlayer->AuctionSell(pkReqAutionSellMsg->m_kClientActionSell, kReqAuctionSell.m_kAuctionSell);	

	if (RE_AUCTIONSELL_SUCCESS == iResultType)
	{
		// 1. 设置上下文
		{
			SetContentFlag(true);
			
			m_kItemEntry = kReqAuctionSell.m_kAuctionSell.m_kItemEntry;
			m_uiCost = GetGlobalSetting.nAuctionCostCount;
		}
		//手续费
		int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, SELL_OPERATE, OPERATE_LOGTYPE1);
		pkPlayer->ReduceCash(GetGlobalSetting.nAuctionCostCount, CT_UNBIND_CASH, SYS_FUNC_TP_AUCTION_SELL, 0, nLogCode);

		LC_PackAsset* pkPackAsset = pkPlayer->GetPackAsset();
		PackOPRecordSet * record_set = pkPackAsset->GetRecordSet();
		pkPlayer->HandlePackOPRecord(record_set, SYS_FUNC_TP_AUCTION_SELL, 0);

		// 继续填充拍卖信息

		kReqAuctionSell.m_kAuctionSell.m_kPrices = pkReqAutionSellMsg->m_kClientActionSell.m_kPrices;
		kReqAuctionSell.m_kAuctionSell.m_kPriceType = pkReqAutionSellMsg->m_kClientActionSell.m_kPriceType;
		kReqAuctionSell.m_kAuctionSell.m_uiTime = GetGlobalSetting.nAuctionSellTime;
		TPSTR2STDSTR(pkPlayer->GetOwnerCharName(), kReqAuctionSell.m_kAuctionSell.m_szCharName);
		SendMsgToTrade(MGPT_SS_REQ_AUCTION_SELL, &kReqAuctionSell, pkPlayer->GetUserID(), pkPlayer->GetInstance(), uiID);
		// OSS	
		LC_BackPackEntry kTempEntry;
		kTempEntry.GetFromProtocol(kReqAuctionSell.m_kAuctionSell.m_kItemEntry);

		SetResultID(RE_SUCCESS);
		return TRAN_OP_NEXT;
	}
	else
	{
		SetResultID(iResultType);
		return TRAN_OP_FAILED;
	}	
}

void AO_Server_ReqAuctionSell::RespondRltMsg()
{
	MG_CS_RLT_Aution_Sell kRltMsg;
	kRltMsg.m_iResultType = GetResultID();

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer( GetCharID() );
	if( pkPlayer )
	{
		pkPlayer->SendMsgToClient(MGPT_RLT_AUCTION_SELL, &kRltMsg);		
	}
}

void AO_Server_ReqAuctionSell::Rollback()
{
	if ( GetContentFlag() )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "Auction exception, data UserID:%d, CharID: %llu",  GetUserID(), GetCharID());
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "Auction cost:%d", m_uiCost);						
	
		// 回滚物品 		
		LC_BackPackEntry kTempEntry;
		kTempEntry.GetFromProtocol(m_kItemEntry);
		kTempEntry.WriteToLog(LOG_TAG_ERROR,LOG_SWITCH_ERROR);
	}
}

void AO_Server_ReqAuctionSell::Reset()
{
	TS_AtomOperate::Reset();
	m_kItemEntry.clear();
	m_uiCost = 0;
}

//////////////////////////////////////////////////////////////////////////

emTranOperate AO_Server_RltAuctionSell::Execute(Protocol::MG_BODY* pkMsg, uint32_t uiID) 
{ 
	MG_GG_RLT_Auction_Sell* pkRltMsg = static_cast<MG_GG_RLT_Auction_Sell*>(pkMsg);

	if (AUCTION_NOTIFY_SUCESS == pkRltMsg->m_uiResult)
	{
		SetResultID(AUCTION_NOTIFY_SUCESS);
		_respendSuccessMsg(pkRltMsg->m_kAuction);
		return TRAN_OP_NEXT;
	}
	else if (AUCTION_NOTIFY_SELL_FULL == pkRltMsg->m_uiResult)		//用户拍卖列表到最大值
	{
		SetResultID(RE_AUCTIONSELL_FAILED_SELLLIST_IF_FULL);		
		return TRAN_OP_FAILED;
	}
	else if (AUCTION_NOTIFY_AUCTION_FULL == pkRltMsg->m_uiResult)	//拍卖总列表到最大值
	{
		SetResultID(RE_AUCTIONSELL_FAILED_TOTALLIST_IF_FULL);		
		return TRAN_OP_FAILED;
	}
	else
	{
		SetResultID(pkRltMsg->m_uiResult);
		return TRAN_OP_FAILED;
	}
}

void AO_Server_RltAuctionSell::RespondRltMsg() 
{
	MG_CS_RLT_Aution_Sell rltMsg;	
	rltMsg.m_iResultType = GetResultID();

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer( GetCharID() );
	if( pkPlayer )
	{
		pkPlayer->SendMsgToClient(MGPT_RLT_AUCTION_SELL, &rltMsg);	
	}
}

void AO_Server_RltAuctionSell::_respendSuccessMsg(const Protocol::PS_Auction& kAuction)
{
	MG_CS_RLT_Aution_Sell rltMsg;	
	rltMsg.m_iResultType = GetResultID();
	rltMsg.m_kAuction = kAuction;

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer( GetCharID() );
	if( pkPlayer )
	{
		pkPlayer->SendMsgToClient(MGPT_RLT_AUCTION_SELL, &rltMsg);	
	}
}

//////////////////////////////////////////////////////////////////////////
TS_Server_AuctionSellTran::TS_Server_AuctionSellTran()
{
	m_kStepTranMap[STEP_REQ_GG_AUCTIONSELL] = T_NEW_D AO_Server_ReqAuctionSell;
	m_kStepTranMap[STEP_RLT_GG_AUCTIONSELL] = T_NEW_D AO_Server_RltAuctionSell;
}

TS_Server_AuctionSellTran::~TS_Server_AuctionSellTran()
{
	for (StepAtomMapIter it = m_kStepTranMap.begin(); it != m_kStepTranMap.end(); ++it)
	{
		T_SAFE_DELETE( it->second);
	}

	m_kStepTranMap.clear();
}