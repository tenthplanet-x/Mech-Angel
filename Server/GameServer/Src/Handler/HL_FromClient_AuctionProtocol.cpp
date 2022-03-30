#include "MG_CS_Auction.h"
#include "NW_ServerSession.h"
#include "TS_TranAuctionSellTran.h"
#include "HL_PacketHandler.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "GameServerApp.h"
#include "LC_SequenceManager.h"
#include "MG_Game2Group.h"

#include "UT_ServerHelper.h"
#include "GlobalSettings.h"
using namespace Protocol;
using namespace GameLogic;

namespace Protocol
{
#define this_source_type SERVER_TYPE_CLIENT
//--------------------------------------------
ON_HANDLE(MGPT_REQ_ADD_ITEM_AUCTIONSELL, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_AddItemToAuctionSell kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	if (!pkPlayer->IsInGame())
	{
		return;
	}
	LC_ActionForbid* pkActionForbid = pkPlayer->GetActionForbid();
	if (!pkActionForbid || pkActionForbid->IsActionForbid(LAFT_AUCTION, LAF_AUCTION_MAX_COUNT))
	{
		return;
	}
	////安全系统检查
	//LC_SafeSystemAsset* pkSafeSystemAsset = pkPlayer->GetSafeSystemAsset();
	//if (NULL == pkSafeSystemAsset)
	//{
	//	return;
	//}
	//ResultType eSafeCheckResult = pkSafeSystemAsset->CheckOperateWithSafeLock();
	//if (eSafeCheckResult != RE_SUCCESS)
	//{
	//	return;
	//}
	//eSafeCheckResult = pkSafeSystemAsset->CheckOperateWithPassword();
	//if (eSafeCheckResult != RE_SUCCESS)
	//{
	//	return;
	//}
	MG_RLT_AddItemToAuctionSell kRltMsg;
	kRltMsg.m_byPackType = kMsg.m_byPackType;
	kRltMsg.m_usItemIndex = kMsg.m_usItemIndex;

	do
	{
		LC_PackAsset* pkPackAsset = pkPlayer->GetPackAsset();
		if (NULL == pkPackAsset)
		{
			kRltMsg.m_iResultType = RE_FAIL;
			break;
		}

		LC_BackPackEntry* pkEntry = NULL;
		int32_t packtype = pkPlayer->GetAuctionItemPackType();
		int32_t packindex = pkPlayer->GetAuctionItemPackIndex();
		if (packtype >= 0 && packindex >= 0)
		{
			pkEntry = pkPackAsset->GetBackPackEntry(packtype, packindex);
			if (pkEntry != NULL && pkEntry->GetBackPackValueByType(LPSFET_VALID) && pkEntry->GetBackPackValueByType(LPSFET_LOCKED) && !pkEntry->GetBackPackValueByType(LPSFET_BINDMODE))
			{
				pkEntry->SetBackPackValueByType(LPSFET_LOCKED, false);
				pkPlayer->SetAuctionItemPackType(-1);
				pkPlayer->SetAuctionItemPackIndex(-1);
				pkPlayer->GetPackAsset()->UpdateDirtyFlag(packtype);
			}
		}

		pkEntry = pkPackAsset->GetBackPackEntry(kMsg.m_byPackType, kMsg.m_usItemIndex);
		if (pkEntry != NULL && pkEntry->GetBackPackValueByType(LPSFET_VALID) && !pkEntry->GetBackPackValueByType(LPSFET_LOCKED) && !pkEntry->GetBackPackValueByType(LPSFET_BINDMODE))
		{
			pkEntry->SetBackPackValueByType(LPSFET_LOCKED, true);
			kRltMsg.m_iResultType = RE_SUCCESS;
			pkPlayer->SetAuctionItemPackType(kMsg.m_byPackType);
			pkPlayer->SetAuctionItemPackIndex(kMsg.m_usItemIndex);
			pkPlayer->GetPackAsset()->UpdateDirtyFlag(kMsg.m_byPackType);
		}
		else
		{
			kRltMsg.m_iResultType = RE_FAIL;
		}
	}while(0);

	pkPlayer->SendMsgToClient(MGPT_RLT_ADD_ITEM_AUCTIONSELL, &kRltMsg);
}
//--------------------------------------------
ON_HANDLE(MGPT_REQ_DEL_ITEM_AUCTIONSELL, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_DeleteItemFromAuctionSell kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	if (!pkPlayer->IsInGame())
	{
		return;
	}
	LC_ActionForbid* pkActionForbid = pkPlayer->GetActionForbid();
	if (!pkActionForbid || pkActionForbid->IsActionForbid(LAFT_AUCTION, LAF_AUCTION_MAX_COUNT))
	{
		return;
	}

	////安全系统检查
	//LC_SafeSystemAsset* pkSafeSystemAsset = pkPlayer->GetSafeSystemAsset();
	//if (NULL == pkSafeSystemAsset)
	//{
	//	return;
	//}
	//ResultType eSafeCheckResult = pkSafeSystemAsset->CheckOperateWithSafeLock();

	//if (eSafeCheckResult != RE_SUCCESS)
	//{
	//	return;
	//}
	//eSafeCheckResult = pkSafeSystemAsset->CheckOperateWithPassword();

	//if (eSafeCheckResult != RE_SUCCESS)
	//{
	//	return;
	//}
	MG_RLT_DeleteItemFromAuctionSell kRltMsg;	
	kRltMsg.m_byPackType = kMsg.m_byPackType;
	kRltMsg.m_usItemIndex = kMsg.m_usItemIndex;

	do 
	{
		LC_PackAsset* pkPackAsset = pkPlayer->GetPackAsset();
		if (!pkPackAsset)
		{
			kRltMsg.m_iResultType = RE_FAIL;
			break;
		}

		LC_BackPackEntry* pkEntry = pkPackAsset->GetBackPackEntry(kMsg.m_byPackType, kMsg.m_usItemIndex);
		if (pkEntry != NULL && pkEntry->GetBackPackValueByType(LPSFET_VALID) && pkEntry->GetBackPackValueByType(LPSFET_LOCKED) && !pkEntry->GetBackPackValueByType(LPSFET_BINDMODE))
		{
			pkEntry->SetBackPackValueByType(LPSFET_LOCKED, false);
			kRltMsg.m_iResultType = RE_SUCCESS;
			pkPlayer->SetAuctionItemPackType(-1);
			pkPlayer->SetAuctionItemPackIndex(-1);
			pkPlayer->GetPackAsset()->UpdateDirtyFlag(kMsg.m_byPackType);
		}
		else
		{
			kRltMsg.m_iResultType = RE_FAIL;
		}
	} while (0);

	pkPlayer->SendMsgToClient(MGPT_RLT_DEL_ITEM_AUCTIONSELL, &kRltMsg);
}
//--------------------------------------------
ON_HANDLE(MGPT_REQ_AUCTION_SELL, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_CS_REQ_Auction_Sell kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	if (!pkPlayer->IsInGame())
	{
		return;
	}
	LC_ActionForbid* pkActionForbid = pkPlayer->GetActionForbid();
	if (!pkActionForbid || pkActionForbid->IsActionForbid(LAFT_AUCTION, LAF_AUCTION_MAX_COUNT))
	{
		return;
	}
	MG_GG_REQ_PreAuction_Sell kReqMsg;
	kReqMsg.m_iSellCount = GetGlobalSetting.nAuctionMaxSellCount;
	kReqMsg.m_iServerMaxCount = GetGlobalSetting.nServerMaxSellCount;
	pkPlayer->SetAuctionSellData(kMsg.m_kClientActionSell);
	SendMsgToTrade(MGPT_SS_REQ_PRE_AUCTION_SELL, &kReqMsg, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID, pkPlayer->GetCitizenship());
}
//--------------------------------------------
ON_HANDLE(MGPT_REQ_AUCTION_LIST, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_AuctionList kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_GG_REQ_Auction_List kReqMsg;
	if (kMsg.m_byReqType < REQ_ALL_AUCTION || kMsg.m_byReqType > REQ_USER_BUY_AUCTION)
	{
		return;
	}

	kReqMsg.m_lReqType = kMsg.m_byReqType;
	kReqMsg.m_uiPage = kMsg.m_uiPage;
	//kReqMsg.m_iServerID = GetGameServerID();
	SendMsgToTrade(MGPT_SS_REQ_AUCTION_LIST, &kReqMsg, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID);
}
//--------------------------------------------
ON_HANDLE(MGPT_REQ_AUCTION_CANCEL, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_CS_REQ_Auction_Cancel kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	if (!pkPlayer->IsInGame())
	{
		return;
	}
	LC_ActionForbid* pkActionForbid = pkPlayer->GetActionForbid();
	if (!pkActionForbid || pkActionForbid->IsActionForbid(LAFT_AUCTION, LAF_AUCTION_MAX_COUNT))
	{
		return;
	}

	MG_GG_REQ_Auction_Cancel kReqMsg;
	int32_t iSequence = LC_SequenceManager::Instance().GetSequenceID();
	PrepareGameTagString(kReqMsg.m_szBillingNO, sizeof(kReqMsg.m_szBillingNO), GetGameServerID(), int32_t(GET_CURRENT_TIMESTAMP_IN_SECONDS()), iSequence);
	kReqMsg.m_lAuctionID = kMsg.m_uiAuctionID;
	SendMsgToTrade(MGPT_SS_REQ_AUCTION_CANCEL, &kReqMsg, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID, pkPlayer->GetCitizenship());
}
//--------------------------------------------
ON_HANDLE(MGPT_REQ_AUCTION_BUY, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_CS_REQ_Auction_Buy kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ActionForbid* pkActionForbid = pkPlayer->GetActionForbid();
	if (!pkActionForbid || pkActionForbid->IsActionForbid(LAFT_AUCTION, LAF_AUCTION_MAX_COUNT))
	{
		return;
	}
	if (!pkPlayer->IsInGame())
	{
		return;
	}
	int m_iResultType = 0;
	if (GetGameServerApp()->IsBonusRole(pkPlayer->GetInstanceUniqueID()))
	{
		m_iResultType = RE_BONUS_TRADE_EXPORT_FORBID;
	}
	//背包是否满
	bool full = pkPlayer->GetPackAsset()->IsPackFull(PACK_TYPE_BASIC);
	if(true == full)
	{
		m_iResultType =  RE_ITEM_PACKAGE_FULL;
	}
	if(m_iResultType)
	{
		MG_CS_RLT_Aution_Buy rltMsg; 
		rltMsg.m_iResultType = m_iResultType;
		pkPlayer->SendMsgToClient(MGPT_RLT_AUCTION_BUY, &rltMsg);	
		return;
	}

	MG_GG_REQ_PreAuction_Buy kReqMsg;
	kReqMsg.m_kAuctionUID = kMsg.m_kAuctionUID;
	kReqMsg.m_kCash = kMsg.m_kCash;
	kReqMsg.m_kCashType = kMsg.m_kCashType;
	kReqMsg.m_iCount = kMsg.m_iCount;

	pkPlayer->SetAuctionBuyID(kMsg.m_kAuctionUID);

	pkPlayer->SetAuctionBuyPrice(kMsg.m_kCashType, kMsg.m_kCash);


	SendMsgToTrade(MGPT_SS_REQ_PRE_AUCTION_BUY, &kReqMsg, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID);
}
//--------------------------------------------
ON_HANDLE(MGPT_REQ_AUCTION_SEARCH, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_Auction_Search kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	if (kMsg.m_iSearchType < AUCTION_SEARCH_BY_ID || kMsg.m_iSearchType >= AUCTION_SEARCH_TYPE_MAX_COUNT)
	{
		return;
	}

	MG_GG_REQ_Auction_Search kReqMsg;
	kReqMsg.m_uiPage = kMsg.m_usPage;
	kReqMsg.m_uiAscend = kMsg.m_uiAscend;
	kReqMsg.m_uiPriority = kMsg.m_uiPriority;
	kReqMsg.m_uiPropery = kMsg.m_uiPropery;
	kReqMsg.m_iServerID = GetGameServerID();
	kReqMsg.m_iGrade = kMsg.m_iGrade;
	kReqMsg.m_iSearchType = kMsg.m_iSearchType;
	kReqMsg.m_kSearchParams = kMsg.m_kSearchParams;
	if (kMsg.m_bUseFilter)
	{
		kReqMsg.m_iPlayerLevel = pkPlayer->GetLevel();
		kReqMsg.m_iWeaponType = pkPlayer->GetCurrentBandWeapon();
	}
	else
	{
		kReqMsg.m_iPlayerLevel = 0;
		kReqMsg.m_iWeaponType = 0;
	}

	SendMsgToTrade(MGPT_SS_REQ_AUCTION_SEARCH, &kReqMsg, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID);
}

//--------------------------------------------
ON_HANDLE(MGPT_REQ_AUCTION_GETAWARD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_AuctionGetAward kMsg;
	if (!kMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_GG_REQ_AuctionGetAward kReqMsg;

	kReqMsg.m_byReqType = kMsg.m_byReqType;
	kReqMsg.m_kAuctionUID = kMsg.m_kAuctionUID;
	SendMsgToTrade(MGPT_SS_REQ_AUCTION_GETAWARD, &kReqMsg, rkMsgHead.m_uiUserID, rkMsgHead.m_uiCharID);
}

}
