#include "ClientPCH.h"

#include "LC_ClientAuctionManager.h"

#include "LC_ClientLogicManager.h"
#include "MG_MessageBase.h"
#include "NW_Helper.h"
#include "LC_ClientGamePlayer.h"
#include "MG_CS_Auction.h"
#include "UT_LogicInfoMessage.h"
#include "CF_ItemCategory.h"
#include "CF_AuctionCategory.h"
#include "TM_ClientTimeManager.h"
#include "UT_FileManagerBase.h"

using namespace GameLogic;
using namespace Protocol;
using namespace Utility;
using namespace CSVFile;

static int iHistorySize = 50;

LC_ClientAuctionManager::LC_ClientAuctionManager()
{
	m_vHistoryBuy.reserve(iHistorySize);
	m_vHistorySell.reserve(iHistorySize);

	Reset();
}

LC_ClientAuctionManager::~LC_ClientAuctionManager()
{

}

void LC_ClientAuctionManager::Reset()
{
	m_kSearchList.clear();
	m_kBuyList.clear();
	m_kSellList.clear();
	m_kMarketMap.clear();

	m_vHistoryBuy.clear();
	m_vHistoryBuy.clear();

	m_kAddItemInfo.Reset();

	m_uiID = 0;
	m_iRewardNum = 0;
	m_bInit = false;

	ClearDirtyFlag();
	ClearDirtyFlagRevisionID();
}


void LC_ClientAuctionManager::RequestAuctionSell(uint32_t iFixPrice, int iCashType, int iPackType, int iIndex, int iCount)
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

	LC_ActionForbid* pkActionForbid = pkGamePlayer->GetActionForbid();
	if(pkActionForbid->IsActionForbid(LAFT_AUCTION,LAF_AUCTION_MAX_COUNT))
	{
		SendLogicInfoMessage(RE_ACTION_FORBID);
		return;
	}

	MG_CS_REQ_Auction_Sell kMsg;

	PS_ClientAuctionSell& info = kMsg.m_kClientActionSell;
	info.m_kPriceType = (int8_t)iCashType;
	info.m_kPrices = iFixPrice;
	info.m_byPackType = iPackType;
	info.m_usEntryIndex = iIndex;
	info.m_uItemCount = iCount;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;

	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_AUCTION_SELL, &kMsg);
	if(bCreateMsg)
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

ResultType LC_ClientAuctionManager::RequestCancelAuction(int iIndex)
{
	LC_ClientAuctionSellData* pkData = GetAuctionSellData(iIndex);
	if ( NULL == pkData )
	{
		return RE_FAIL;
	}

	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager)
	{
		return RE_FAIL;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (NULL == pkGamePlayer)
	{
		return RE_FAIL;
	}

	LC_ActionForbid* pkActionForbid = pkGamePlayer->GetActionForbid();
	if(pkActionForbid->IsActionForbid(LAFT_AUCTION,LAF_AUCTION_MAX_COUNT))
	{
		SendLogicInfoMessage(RE_ACTION_FORBID);
		return RE_ACTION_FORBID;
	}	
	
	MG_CS_REQ_Auction_Cancel kMsg;
	kMsg.m_uiAuctionID = pkData->GetAuctionID();

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_AUCTION_CANCEL, &kMsg);

	if (bCreateMsg)
	{
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
		return RE_SUCCESS;
	}

	return RE_FAIL;
}

void LC_ClientAuctionManager::ReqGetReward(int iIndex)
{
	LC_ClientAuctionBuyData* pkData = GetAuctionBuyData(iIndex);
	if ( NULL == pkData )
	{
		return;
	}

	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (NULL == pkGamePlayer)
	{
		return;
	}

	LC_ActionForbid* pkActionForbid = pkGamePlayer->GetActionForbid();
	if(pkActionForbid->IsActionForbid(LAFT_AUCTION,LAF_AUCTION_MAX_COUNT))
	{
		SendLogicInfoMessage(RE_ACTION_FORBID);
		return;
	}

	MG_REQ_AuctionGetAward kMsg;
	kMsg.m_kAuctionUID = pkData->GetAuctionID();
	kMsg.m_byReqType = 1;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_AUCTION_GETAWARD, &kMsg);

	if (bCreateMsg)
	{
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}

void LC_ClientAuctionManager::ReqOnekeyGetReward()
{
	if(m_kBuyList.empty())
		return;

	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (NULL == pkGamePlayer)
	{
		return;
	}

	MG_REQ_AuctionGetAward kMsg;
	kMsg.m_byReqType = 2;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;

	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_AUCTION_GETAWARD, &kMsg);
	if (bCreateMsg)
	{
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}


void LC_ClientAuctionManager::RequestAuctionBuyByAuctionID(unique_id_type iAuction, int iCount, int iCashType, int iPrice)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (NULL == pkGamePlayer)
	{
		return;
	}

	LC_ActionForbid* pkActionForbid = pkGamePlayer->GetActionForbid();
	if(pkActionForbid->IsActionForbid(LAFT_AUCTION,LAF_AUCTION_MAX_COUNT))
	{
		SendLogicInfoMessage(RE_ACTION_FORBID);
		return;
	}

	MG_CS_REQ_Auction_Buy kMsg;
	kMsg.m_kAuctionUID = iAuction;
	kMsg.m_iCount = iCount;
	kMsg.m_kCashType = iCashType;
	kMsg.m_kCash = iPrice;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;

	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_AUCTION_BUY, &kMsg);
	if (bCreateMsg)
	{
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}

void LC_ClientAuctionManager::Search(const StringType& strSearchName, int iItemCategory, int iStartLevel, int iGrade, int eSortType)
{
	ClearAuctionSearchList();

	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (NULL == pkGamePlayer)
	{
		return;
	}

	// 确定物品分类
	mem::set<int32_t> vTypeList;

	CF_AuctionCategory* pkAuctionCategory = CF_AuctionCategory::GetSingletonPtr();
	
	bool bEmptySearchName = strSearchName.empty();

	if (iItemCategory > 0)
	{
		CF_AuctionCategory::DataEntry* pkData = pkAuctionCategory->GetEntryPtr(iItemCategory);
		if (NULL == pkData)
		{
			return;
		}

		const StringType& strTypeList = pkData->_kItemTypeList;

		// 表里用 - 符号进行间隔
		TStringVector kStringVector= UT_BaseStringUtility::SplitString(strTypeList, '-');
		for(int i = 0; i < kStringVector.size(); ++i)
		{
			const StringType& strTemp = kStringVector[i];
			vTypeList.insert(String2Int(strTemp));
		}
	}
	else
	{
		CF_AuctionCategory::DataEntryMapExternIterator kIter = pkAuctionCategory->GetIterator();
		while(kIter.HasMoreElements())
		{
			CF_AuctionCategory::DataEntry* pkData = kIter.PeekNextValuePtr();
			if (pkData->_lParentID != -1)
			{
				const StringType& strTypeList = pkData->_kItemTypeList;
				TStringVector kStringVector= UT_BaseStringUtility::SplitString(strTypeList, '-');
				for(int i = 0; i < kStringVector.size(); ++i)
				{
					const StringType& strTemp = kStringVector[i];
					vTypeList.insert(String2Int(strTemp));
				}
			}
			kIter.MoveNext();
		}
	}

	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	int64_t iCurTimeVal = pkGamePlayer->GetServerTimeVal();

	AUCTION_SELL_MAP::iterator iter = m_kMarketMap.begin();
	for( ; iter!=m_kMarketMap.end(); )
	{
		LC_ClientAuctionSellData& kInfo = iter->second;
		if(kInfo.GetLeftTime() <= iCurTimeVal)
		{
			//已超时，从列表中移除
			iter = m_kMarketMap.erase(iter);
			continue;
		}
		
		const LC_BackPackEntry* kEntry = kInfo.GetEntry();
		LC_ItemBase* pkItem = pkItemFactory->RequestItem(kEntry->GetTypeID());
		if(pkItem == NULL)
		{
			++iter;
			continue;
		}

		if (iItemCategory != 0
			&& vTypeList.find(pkItem->GetCategory()) == vTypeList.end())
		{
			++iter;
			continue;
		}

		if (!bEmptySearchName
			&& UT_BaseStringUtility::FindSubString(pkItem->GetProperty_Name(), strSearchName) == -1)
		{
			++iter;
			continue;
		}

		int iItemLevel = pkItem->GetLevel();
		if (iStartLevel != 0 && iItemLevel < iStartLevel )
		{
			++iter;
			continue;
		}

		int iItemGrade = pkItem->GetGrade();
		if (iGrade != 0 && iItemGrade != iGrade)
		{
			++iter;
			continue;
		}

		kInfo.m_iItemGrade = iItemGrade;
		kInfo.m_iItemLevel = iItemLevel;
		kInfo.m_iTotalPrice = kInfo.m_iFixedPrice*kEntry->GetOverlapCount();
		m_kSearchList.push_back(&kInfo);

		++iter;
	}

	switch(eSortType)
	{
	case SEARCH_SORTTYPE_NORMAL:
		std::sort(m_kSearchList.begin(), m_kSearchList.end(), _sortNormal);
		break;
	case SEARCH_SORTTYPE_PRICE_UP:
		std::sort(m_kSearchList.begin(), m_kSearchList.end(), _sortByPriceUp);
		break;
	case SEARCH_SORTTYPE_PRICE_DOWN:
		std::sort(m_kSearchList.begin(), m_kSearchList.end(), _sortByPriceDown);
		break;
	case SEARCH_SORTTYPE_TOTALPRICE_UP:
		std::sort(m_kSearchList.begin(), m_kSearchList.end(), _sortByTotalPriceUp);
		break;
	case SEARCH_SORTTYPE_TOTALPRICE_DOWN:
		std::sort(m_kSearchList.begin(), m_kSearchList.end(), _sortByTotalPriceDown);
		break;
	default:
		T_ASSERT_CRITICAL_S(false);
		break;
	}
	

	SetDirtyFlag(AUCTION_DIRTY_FLAG_SEARCH, true);
	AddDirtyFlagRevisionID(AUCTION_DIRTY_FLAG_SEARCH);
}

void LC_ClientAuctionManager::RequestAuctionList(int iType)
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

	switch(iType)
	{
	/*case REQ_ALL_AUCTION:
		m_kMarketMap.clear();
		break;*/
	case REQ_USER_SELL_AUCTION:
		m_kSellList.clear();
		break;
	case REQ_USER_BUY_AUCTION:
		m_kBuyList.clear();
		break;
	default:
		break;
	}

	MG_REQ_AuctionList kMsg;
	kMsg.m_byReqType = (uint8_t)iType;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_AUCTION_LIST, &kMsg);

	if (bCreateMsg)
	{
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}

	m_bInit = true;
}

void LC_ClientAuctionManager::UpdateAuctionBuyDataList(const Protocol::MG_RLT_AuctionList& kAuctionList)
{
	for (int i = 0; i < kAuctionList.m_byAuctionCount; ++i)
	{
		UpdateAuctionBuyData( kAuctionList.m_Auction[i] );
	}

	if(kAuctionList.m_uiMore == AUCTION_UIMORE_ADD
		|| kAuctionList.m_uiMore == AUCTION_UIMORE_END)
	{
		_onBuyDataUpdated();
	}
}

void LC_ClientAuctionManager::UpdateAuctionBuyData(const Protocol::PS_Auction& kAuction)
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

	for (AUCTION_BUY_LIST::iterator it = m_kBuyList.begin(); it != m_kBuyList.end(); ++it)
	{
		LC_ClientAuctionBuyData& kData = *it;
		if (kData.GetAuctionID() == kAuction.m_uiAuctionID)
		{
			kData.SetLeftTime( kAuction.m_uiLeftTime );
			if (TPSTReqSTDSTR(kAuction.szCharName, pkGamePlayer->GetCharName()) == true )
			{
				kData.SetStatus(true);
			}
			else
			{
				kData.SetStatus(false);
			}

			kData.GetEntry()->GetFromProtocol(kAuction.m_kItemAssetEntry, true);
			_onBuyDataUpdated();
			return;
		}
	}

	LC_ClientAuctionBuyData kData;

	kData.SetAuctionID( kAuction.m_uiAuctionID );
	kData.SetLeftTime( kAuction.m_uiLeftTime );
	kData.m_iCashType = kAuction.m_kPriceType;
	kData.m_iFixedPrice = kAuction.m_kPrices;

	if (TPSTReqSTDSTR(kAuction.szCharName, pkGamePlayer->GetCharName()) == true )
	{
		kData.SetStatus(true);
	}
	else
	{
		kData.SetStatus(false);
	}

	kData.GetEntry()->GetFromProtocol(kAuction.m_kItemAssetEntry, true);

	m_kBuyList.push_back(kData);

	AddHistory(&kData);

	_onBuyDataUpdated();
}

void LC_ClientAuctionManager::UpdateAuctionSellDataList(const Protocol::MG_RLT_AuctionList& kAuctionList)
{
	for (int i = 0; i < kAuctionList.m_byAuctionCount; ++i)
	{
		UpdateAuctionSellData( kAuctionList.m_Auction[i] );
	}

	if(kAuctionList.m_uiMore == AUCTION_UIMORE_ADD
		|| kAuctionList.m_uiMore == AUCTION_UIMORE_END)
	{
		SetDirtyFlag(AUCTION_DIRTY_FLAG_SELL, true);
		AddDirtyFlagRevisionID(AUCTION_DIRTY_FLAG_SELL);
	}
}

void LC_ClientAuctionManager::UpdateAuctionSellData(const Protocol::PS_Auction& kAuction)
{
	bool bFind = false;
	for (AUCTION_SELL_LIST::iterator it = m_kSellList.begin(); it != m_kSellList.end(); ++it)
	{
		LC_ClientAuctionSellData& kData = *it;
		if (kData.GetAuctionID() == kAuction.m_uiAuctionID)
		{
			kData.SetLeftTime( kAuction.m_uiLeftTime );
			kData.SetName( STDSTR2TPSTR(kAuction.szCharName));

			LC_BackPackEntry *pkEntry = kData.GetEntry();
			pkEntry->GetFromProtocol(kAuction.m_kItemAssetEntry, true);
			pkEntry->SetOverlapCount(kAuction.m_iItemCount);
			bFind = true;
			break;
		}
	}

	if(!bFind)
	{
		LC_ClientAuctionSellData kData;

		kData.SetAuctionID(kAuction.m_uiAuctionID);
		kData.SetLeftTime(kAuction.m_uiLeftTime);
		kData.SetName(STDSTR2TPSTR(kAuction.szCharName));
		kData.m_iCashType = kAuction.m_kPriceType;
		kData.m_iFixedPrice = kAuction.m_kPrices;

		LC_BackPackEntry *pkEntry = kData.GetEntry();
		pkEntry->GetFromProtocol(kAuction.m_kItemAssetEntry, true);
		pkEntry->SetOverlapCount(kAuction.m_iItemCount);

		m_kSellList.push_back(kData);
	}
	
	SetDirtyFlag(AUCTION_DIRTY_FLAG_SELL, true);
	AddDirtyFlagRevisionID(AUCTION_DIRTY_FLAG_SELL);
}

void LC_ClientAuctionManager::DeleteAuctionSellData(unique_id_type uiAuctionID)
{
	for (AUCTION_SELL_LIST::iterator it = m_kSellList.begin(); it != m_kSellList.end(); ++it)
	{
		LC_ClientAuctionSellData& kData = *it;
		if (kData.GetAuctionID() == uiAuctionID)
		{
			m_kSellList.erase(it);
			SetDirtyFlag(AUCTION_DIRTY_FLAG_SELL, true);
			AddDirtyFlagRevisionID(AUCTION_DIRTY_FLAG_SELL);
			return;
		}
	}
}
void LC_ClientAuctionManager::DeleteAuctionBuyData(unique_id_type uiAuctionID)
{
	for (AUCTION_BUY_LIST::iterator it = m_kBuyList.begin(); it != m_kBuyList.end(); ++it)
	{
		LC_ClientAuctionBuyData& kData = *it;
		if (kData.GetAuctionID() == uiAuctionID)
		{
			m_kBuyList.erase(it);
			_onBuyDataUpdated();
			return;
		}
	}
}
bool LC_ClientAuctionManager::DeleteAuctionMarketData(unique_id_type uiAuctionID, int iCount)
{
	//iCount == -1表示直接移除
	AUCTION_SELL_MAP::iterator iter = m_kMarketMap.find(uiAuctionID);
	if(iter == m_kMarketMap.end())
		return false;
	
	LC_ClientAuctionSellData& kData = iter->second;

	LC_BackPackEntry* pkItemEntry = kData.GetEntry();
	int iCurCount = pkItemEntry->GetOverlapCount();
	if(iCount == -1 || iCurCount <= iCount)
	{
		m_kMarketMap.erase(iter);
	}
	else
	{
		pkItemEntry->SetOverlapCount(iCurCount - iCount);
	}
	return true;
}


void LC_ClientAuctionManager::ClearAuctionBuyList()
{
	m_kBuyList.clear();
	_onBuyDataUpdated();
}

void LC_ClientAuctionManager::ClearAuctionSellList()
{
	m_kSellList.clear();
}

void LC_ClientAuctionManager::UpdateAuctionMarket(const Protocol::PS_Auction& kAuction)
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
	
	unique_id_type iAuctionID = kAuction.m_uiAuctionID;
	LC_ClientAuctionSellData& kData = m_kMarketMap[iAuctionID];

	kData.SetAuctionID( iAuctionID );
	kData.SetLeftTime( kAuction.m_uiLeftTime );
	kData.m_iCashType = kAuction.m_kPriceType;
	kData.m_iFixedPrice = kAuction.m_kPrices;
	kData.SetName(STDSTR2TPSTR(kAuction.szCharName));
	kData.SetIsItemOwner(pkGamePlayer->GetInstance() == kAuction.m_uiSellCharID);

	LC_BackPackEntry *pkEntry = kData.GetEntry();
	pkEntry->GetFromProtocol(kAuction.m_kItemAssetEntry, true);
	pkEntry->SetOverlapCount(kAuction.m_iItemCount);
}

void LC_ClientAuctionManager::UpdateAuctionSearchList(const Protocol::MG_RLT_AuctionList& kAuctionList)
{
	ClearAuctionSearchList();
	
	SetDirtyFlag(AUCTION_DIRTY_FLAG_SEARCH, true);
	AddDirtyFlagRevisionID(AUCTION_DIRTY_FLAG_SEARCH);
}

bool LC_ClientAuctionManager::UpdateAuctionMarketList(const Protocol::MG_RLT_AuctionList& kAuctionList)
{
	if(kAuctionList.m_uiMore == AUCTION_UIMORE_CLEAR)
	{
		m_kMarketMap.clear();
		return true;
	}

	bool bAdd = kAuctionList.m_uiMore == AUCTION_UIMORE_ADD;
	for (int i = 0; i < kAuctionList.m_byAuctionCount; ++i)
	{
		const PS_Auction& kData = kAuctionList.m_Auction[i];
		if(bAdd && kData.m_uiSellState == AUCTION_SELLSTATE_REMOVE)
		{
			DeleteAuctionMarketData( kData.m_uiAuctionID );
		}
		else
		{
			UpdateAuctionMarket( kData );
		}
	}

	if(kAuctionList.m_uiMore == AUCTION_UIMORE_END || bAdd)
		return true;
	
	return false;
}


void LC_ClientAuctionManager::ClearAuctionSearchList()
{
	m_kSearchList.clear();

	/*SetDirtyFlag(AUCTION_DIRTY_FLAG_SEARCH, true);
	AddDirtyFlagRevisionID(AUCTION_DIRTY_FLAG_SEARCH);*/
}

LC_ClientAddItemInfo* LC_ClientAuctionManager::GetAddItemInfo()
{
	return &m_kAddItemInfo;
}

int	LC_ClientAuctionManager::GetAuctionBuyCount()
{
	return static_cast < int > ( m_kBuyList.size() );
}

LC_ClientAuctionBuyData* LC_ClientAuctionManager::GetAuctionBuyData(int iIndex)
{
	if ( iIndex >= 0 && iIndex < GetAuctionBuyCount() )
	{
		LC_ClientAuctionBuyData* pkData = & m_kBuyList.at(iIndex);
		return pkData;
	}
	
	return NULL;
}

bool LC_ClientAuctionManager::IsSellItemInBuyList(unique_id_type lAuctionID)
{
	for (AUCTION_BUY_LIST::iterator it = m_kBuyList.begin();
		it != m_kBuyList.end(); ++it)
	{
		LC_ClientAuctionBuyData& kData = *it;
		if (kData.GetAuctionID() == lAuctionID && kData.GetStatus())
		{
			return true;
		}
	}

	return false;
}

LC_ClientAuctionSellData* LC_ClientAuctionManager::GetAuctionMarketDataByAuctionID(unique_id_type lAuctionID)
{
	AUCTION_SELL_MAP::iterator iter = m_kMarketMap.find(lAuctionID);
	if(iter == m_kMarketMap.end())
		return NULL;
	
	return &(iter->second);
}

int LC_ClientAuctionManager::GetAuctionSellCount()
{
	return static_cast < int > ( m_kSellList.size() );
}

LC_ClientAuctionSellData* LC_ClientAuctionManager::GetAuctionSellData(int iIndex)
{
	if ( iIndex >= 0 && iIndex < GetAuctionSellCount() )
	{
		LC_ClientAuctionSellData* pkData = & m_kSellList.at(iIndex);
		return pkData;		
	}

	return NULL;
}

int LC_ClientAuctionManager::GetAuctionSearchCount()
{
	return static_cast < int > ( m_kSearchList.size() );
}

LC_ClientAuctionSellData* LC_ClientAuctionManager::GetAuctionSearchData(int iIndex)
{
	if ( iIndex >= 0 && iIndex < GetAuctionSearchCount() )
	{
		return m_kSearchList.at(iIndex);
	}

	return NULL;
}

void LC_ClientAuctionManager::SetDirtyFlag(int iType, bool bFlag)
{
	if (iType >= AUCTION_DIRTY_FLAG_SEARCH && iType < MAX_DIRTY_FLAG_TYPE)
	{
		m_bDirtyFlag[iType] = bFlag;
	}
}

bool LC_ClientAuctionManager::GetDirtyFlag(int iType)
{
	if (iType >= AUCTION_DIRTY_FLAG_SEARCH && iType < MAX_DIRTY_FLAG_TYPE)
	{
		return m_bDirtyFlag[iType];
	}

	return false;
}

void LC_ClientAuctionManager::ClearDirtyFlag()
{
	for (int i = AUCTION_DIRTY_FLAG_SEARCH; i < MAX_DIRTY_FLAG_TYPE; ++i)
	{
		m_bDirtyFlag[i] = false;
	}
}

void LC_ClientAuctionManager::AddDirtyFlagRevisionID(int iType)
{
	if (iType >= AUCTION_DIRTY_FLAG_SEARCH && iType < MAX_DIRTY_FLAG_TYPE)
	{
		++m_lDirtyFlagRevisionID[iType];
	}	
}

long LC_ClientAuctionManager::GetDirtyFlagRevisionID(int iType)
{
	if (iType >= AUCTION_DIRTY_FLAG_SEARCH && iType < MAX_DIRTY_FLAG_TYPE)
	{
		return m_lDirtyFlagRevisionID[iType];
	}
	
	return 0;
}

void LC_ClientAuctionManager::SetDirtyFlagRevisionID(int iType, long lRevisionID)
{
	if (iType >= AUCTION_DIRTY_FLAG_SEARCH && iType < MAX_DIRTY_FLAG_TYPE)
	{
		m_lDirtyFlagRevisionID[iType] = lRevisionID;
	}
}

void LC_ClientAuctionManager::ClearDirtyFlagRevisionID()
{	
	for (int i = AUCTION_DIRTY_FLAG_SEARCH; i < MAX_DIRTY_FLAG_TYPE; ++i)
	{
		m_lDirtyFlagRevisionID[i] = 0;
	}
}
int LC_ClientAuctionManager::GetCategoryCount()
{
	return CF_AuctionCategory::GetSingletonPtr()->GetEntryCount();
}

int LC_ClientAuctionManager::GetCategoryID(int iIndex)
{
	CF_AuctionCategory::DataEntryMapExternIterator kIter =  CF_AuctionCategory::GetSingletonPtr()->GetIterator();
	for (; kIter.HasMoreElements(); kIter.MoveNext())
	{
		const CF_AuctionCategory::DataEntry& kInfo = kIter.PeekNextValue();
		if (iIndex == kInfo._iSortID)
		{
			return kInfo._iID;
		}
	}

	return -1;
}

StringType LC_ClientAuctionManager::GetCategoryName(int iIndex)
{
	CF_AuctionCategory::DataEntryMapExternIterator kIter =  CF_AuctionCategory::GetSingletonPtr()->GetIterator();
	for (; kIter.HasMoreElements(); kIter.MoveNext())
	{
		const CF_AuctionCategory::DataEntry& kInfo = kIter.PeekNextValue();
		if (iIndex == kInfo._iSortID)
		{
			return kInfo._kName;
		}
	}

	return "";
}

StringType LC_ClientAuctionManager::GetCategoryPic(int iIndex)
{
	CF_AuctionCategory::DataEntryMapExternIterator kIter =  CF_AuctionCategory::GetSingletonPtr()->GetIterator();
	for (; kIter.HasMoreElements(); kIter.MoveNext())
	{
		const CF_AuctionCategory::DataEntry& kInfo = kIter.PeekNextValue();
		if (iIndex == kInfo._iSortID)
		{
			return kInfo._kPic;
		}
	}

	return "";
}

int LC_ClientAuctionManager::GetCategoryParentID(int iIndex)
{
	CF_AuctionCategory::DataEntryMapExternIterator kIter =  CF_AuctionCategory::GetSingletonPtr()->GetIterator();
	for (; kIter.HasMoreElements(); kIter.MoveNext())
	{
		const CF_AuctionCategory::DataEntry& kInfo = kIter.PeekNextValue();
		if (iIndex == kInfo._iSortID)
		{
			return kInfo._lParentID;
		}
	}

	return -1;
}

void LC_ClientAuctionManager::LoadHistory(unique_id_type uiID)
{
	if(m_uiID > 0)
		return;

	m_uiID = uiID;

	char szFileName[64] = {0};
	sprintf(szFileName,"%s/%lld.data", STR_AUCTION_HISTORY_DIR, m_uiID);

	FILE* f = fopen(szFileName, "rb");
	if (NULL == f)
	{
		DWORD errorValue = GetLastError();
		GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM, "LC_ClientAuctionManager::LoadHistory Error=%d", errorValue);
		return;
	}

	fseek(f, 0, SEEK_END);
	unsigned int sz = ftell(f);
	fseek(f, 0, SEEK_SET);

	StringType sVecContent;
	sVecContent.resize(sz);
	fread(&sVecContent[0],sz, 1, f);

	fclose(f);

	_initHistoryData(sVecContent);
}

void LC_ClientAuctionManager::SaveHistory()
{
	if(m_uiID == 0)
		return;

	::CreateDirectory(STR_AUCTION_HISTORY_DIR , NULL );

	const StringType& sContent = _getSaveString();

	char szFileName[64] = {0};
	sprintf_s(szFileName, 64, "%s/%I64d.data", STR_AUCTION_HISTORY_DIR, m_uiID);

	FILE* f = fopen(szFileName, "w+");
	if(!f)
	{
		DWORD errorValue = GetLastError();
		GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM, "LC_ClientAuctionManager::_saveFile Error=%d", errorValue);
		return;
	}

	fwrite(sContent.c_str(),sContent.length(), 1, f);
	fclose(f);
}

void LC_ClientAuctionManager::AddHistory(LC_ClientAuctionSellData* data, int iCount)
{
	unique_id_type iAuctionID = data->GetAuctionID();
	if(m_vHistoryBuy.size() == iHistorySize)
	{
		AuctionHistoryVec::iterator it = m_vHistoryBuy.begin();
		m_vHistoryBuy.erase(it);
	}

	int64_t iTimeVal = TM_ClientTimeManager::GetSingletonPtr()->GetServerTime();// : data->GetLeftTime();
	
	LC_BackPackEntry* pkEntry = data->GetEntry();
	T_ASSERT_CRITICAL_S(pkEntry->GetValid());
	
	int iItemID = pkEntry->GetTypeID();

	StringStreamType sStream;
	sStream << iAuctionID << '#' << iTimeVal << '#' << data->GetFixedPrice()*iCount << '#' << iItemID << '#' << iCount;

	HistoryData kData;
	kData.iTimeVal = iTimeVal;
	kData.iAuctionID = iAuctionID;
	kData.sContent = sStream.str();
	m_vHistoryBuy.push_back(kData);

	std::sort(m_vHistoryBuy.begin(), m_vHistoryBuy.end(), _sortRecordByTime);

	SaveHistory();
}

void LC_ClientAuctionManager::AddHistory(LC_ClientAuctionBuyData* data)
{
	unique_id_type iAuctionID = data->GetAuctionID();
	if(IsHistoryExist(HISTORY_TYPE_SELL, iAuctionID))
		return;

	if(m_vHistorySell.size() == iHistorySize)
	{
		AuctionHistoryVec::iterator it = m_vHistorySell.begin();
		m_vHistorySell.erase(it);
	}

	int64_t iTimeVal = data->GetLeftTime();
	
	LC_BackPackEntry* pkEntry = data->GetEntry();
	T_ASSERT_CRITICAL_S(pkEntry->GetValid());

	int iItemID = pkEntry->GetTypeID();
	int iCount = pkEntry->GetOverlapCount();

	StringStreamType sStream;
	sStream << iAuctionID << '#' << iTimeVal << '#' << data->GetFixedPrice() << '#' << iItemID << '#' << iCount;

	HistoryData kData;
	kData.iTimeVal = iTimeVal;
	kData.iAuctionID = iAuctionID;
	kData.sContent = sStream.str();
	m_vHistorySell.push_back(kData);

	std::sort(m_vHistorySell.begin(), m_vHistorySell.end(), _sortRecordByTime);
}

bool LC_ClientAuctionManager::IsHistoryExist(HistoryType eType, unique_id_type iAuctionID)
{
	AuctionHistoryVec* pkVec = eType == HISTORY_TYPE_BUY ? &m_vHistoryBuy : &m_vHistorySell;
	AuctionHistoryVec::const_iterator it = pkVec->begin();
	for(; it!=pkVec->end(); ++it)
	{
		if(it->iAuctionID == iAuctionID)
			return true;
	}

	return false;
}

int LC_ClientAuctionManager::GetHistoryCount(int eType)
{
	if(eType == HISTORY_TYPE_BUY)
		return m_vHistoryBuy.size();
	else
		return m_vHistorySell.size();
}

const StringType& LC_ClientAuctionManager::GetHistoryStr(int eType, int iIndex)
{
	if(eType == HISTORY_TYPE_BUY)
	{
		return m_vHistoryBuy[iIndex].sContent;
	}
	else
	{
		return m_vHistorySell[iIndex].sContent;
	}
}

void LC_ClientAuctionManager::_setRecord(AuctionHistoryVec& kVec, const StringType& sContent)
{
	kVec.clear();

	TStringVector strVec = UT_BaseStringUtility::SplitString(sContent, '|');
	for (int i = 0; i < strVec.size(); ++i)
	{
		const StringType& s = strVec[i];
		TStringVector detail = UT_BaseStringUtility::SplitString(s, '#');
		T_ASSERT_CRITICAL_S(detail.size() >= 3);

		HistoryData kData;
		kData.iAuctionID = UT_BaseStringUtility::toUInt64(detail[0]);
		kData.iTimeVal = UT_BaseStringUtility::toUInt64(detail[1]);
		kData.sContent = s;
		kVec.push_back(kData);
	}

	std::sort(kVec.begin(), kVec.end(), _sortRecordByTime);
}

StringType LC_ClientAuctionManager::_getSaveString()
{
	StringStreamType sStream;
	
	AuctionHistoryVec::const_iterator iter = m_vHistoryBuy.begin();
	for( ; iter!=m_vHistoryBuy.end(); ++iter)
	{
		sStream << iter->sContent << '|';
	}

	sStream << '@';

	AuctionHistoryVec::const_iterator iter2 = m_vHistorySell.begin();
	for( ; iter2!=m_vHistorySell.end(); ++iter2)
	{
		sStream << iter2->sContent << '|';
	}

	return sStream.str();
}

void LC_ClientAuctionManager::_onBuyDataUpdated()
{
	UpdateRewardNum(GetAuctionBuyCount());
	SetDirtyFlag(AUCTION_DIRTY_FLAG_BUY, true);
	AddDirtyFlagRevisionID(AUCTION_DIRTY_FLAG_BUY);
}

void LC_ClientAuctionManager::_initHistoryData(const StringType& str)
{
	TStringVector kVector = UT_BaseStringUtility::SplitString(str, '@');
	for (int k = 0; k < kVector.size(); ++k)
	{
		if (k == HISTORY_TYPE_BUY)
			_setRecord(m_vHistoryBuy, kVector[k]);
		else if (k == HISTORY_TYPE_SELL)
			_setRecord(m_vHistorySell, kVector[k]);
		else
			break;
	}
}

bool LC_ClientAuctionManager::_sortNormal(const LC_ClientAuctionSellData* v1, const LC_ClientAuctionSellData* v2)
{
	if(v1->m_iItemGrade != v2->m_iItemGrade)
		return v1->m_iItemGrade > v2->m_iItemGrade;

	if(v1->m_iFixedPrice != v2->m_iFixedPrice)
		return v1->m_iFixedPrice < v2->m_iFixedPrice;

	return v1->m_iItemLevel > v2->m_iItemLevel;
}

bool LC_ClientAuctionManager::_sortByPriceDown(const LC_ClientAuctionSellData* v1, const LC_ClientAuctionSellData* v2)
{
	return v1->m_iFixedPrice > v2->m_iFixedPrice;
}

bool LC_ClientAuctionManager::_sortByPriceUp(const LC_ClientAuctionSellData* v1, const LC_ClientAuctionSellData* v2)
{
	return v1->m_iFixedPrice < v2->m_iFixedPrice;
}

bool LC_ClientAuctionManager::_sortByTotalPriceDown(const LC_ClientAuctionSellData* v1, const LC_ClientAuctionSellData* v2)
{
	return v1->m_iTotalPrice > v2->m_iTotalPrice;
}

bool LC_ClientAuctionManager::_sortByTotalPriceUp(const LC_ClientAuctionSellData* v1, const LC_ClientAuctionSellData* v2)
{
	return v1->m_iTotalPrice < v2->m_iTotalPrice;
}

bool LC_ClientAuctionManager::_sortRecordByTime(const HistoryData& v1, const HistoryData& v2)
{
	return v1.iTimeVal > v2.iTimeVal;
}
