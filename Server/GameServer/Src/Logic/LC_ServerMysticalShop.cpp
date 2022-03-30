#include "LC_ServerMysticalShop.h"
#include "CF_MysticalShop.h"
#include "CF_MysticalShopReset.h"
#include "CF_MysticalShopVip.h"
#include "CF_ShopList.h"

#include "LC_Helper.h"
#include "LC_Define.h"

#include "UT_RedisChannel.h"
#include "GameServerApp.h"

#include <time.h>

using namespace CSVFile;
using namespace GameLogic;

ServerMysticalShop::ServerMysticalShop()
{
	Clear();
}

ServerMysticalShop::~ServerMysticalShop()
{
	Clear();
}

void ServerMysticalShop::Clear()
{
	m_ChooseMap.clear();
	m_tRefreshActive = 0;
	m_pMysticalShop = NULL;
}

void ServerMysticalShop::InitConfig(CSVFile::CSVDataLoader* pkLoader, const CSVFile::CSVDataSourceList* pkCSVs)
{
	CF_MysticalShop* pkConfig = CF_MysticalShop::GetSingletonPtr();
	if(IS_VALID_RELATED_CSV(pkConfig, pkCSVs) == true)
	{
		m_ChooseMap.clear();
		CF_MysticalShop::DataEntryMapExternIterator iter = pkConfig->GetIterator();
		while(iter.HasMoreElements())
		{
			const CF_MysticalShop::DataEntry * entry = iter.PeekNextValuePtr();
			{
				shopChooseNode choNode;
				choNode.m_nID = entry->_iID;
				choNode.m_nWeight = entry->_iValue;

				shopChooseList& choList = m_ChooseMap[entry->_iGroup];
				choList.m_ChooseList.push_back(choNode);
			}
			iter.MoveNext();
		}

		for(ShopChooseNodeMap::iterator iter = m_ChooseMap.begin(); iter != m_ChooseMap.end(); iter++)
		{
			shopChooseList& choList = iter->second;
			choList.m_nTotalWeight = 0;
			for(mem::list<shopChooseNode>::iterator it = choList.m_ChooseList.begin();it != choList.m_ChooseList.end(); it++)
			{
				choList.m_nTotalWeight += it->m_nWeight;
			}
		}
	}
	
	CF_MysticalShopRefresh* pShopRefresh = CF_MysticalShopRefresh::GetSingletonPtr();
	if(IS_VALID_RELATED_CSV(pShopRefresh, pkCSVs) == true)
	{
		m_pMysticalShop = NULL;
		CF_MysticalShopRefresh::DataEntryMapExternIterator iter = pShopRefresh->GetIterator();
		while(iter.HasMoreElements())
		{
			const CF_MysticalShopRefresh::DataEntry * entry = iter.PeekNextValuePtr();
			if(entry != NULL)
			{
				m_pMysticalShop = entry;
				break;
			}
			iter.MoveNext();
		}
		if(m_pMysticalShop != NULL)
		{
			time_t nNowTime = (time_t)GET_CURRENT_TIMESTAMP_IN_SECONDS();
			m_tRefreshActive = m_pMysticalShop->_iRefreshTime.GetNextTime(nNowTime);
		}
	}
}

ResultType ServerMysticalShop::RefreshItems(LC_ServerPlayer* pPlayer, bool bCheck)
{
	if(m_ChooseMap.empty())
	{
		InitConfig();
	}

	if(m_ChooseMap.empty() == true)
	{
		return RE_FAIL;
	}

	uint32_t nNowTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	if(bCheck == true)
	{
		uint32_t tLastRefresh = pPlayer->GetMysticalShopTimeStamp();
		bool bSameDay = LC_Helper::CheckIsSameDay(nNowTime, tLastRefresh);
		if(bSameDay == true)
		{
			if(m_pMysticalShop == NULL || !m_pMysticalShop->_iRefreshTime.IsValid())
			{
				return RE_FAIL;
			}
			else
			{
				uint32_t nActiveTime = (uint32_t)m_pMysticalShop->_iRefreshTime.GetNextTime(tLastRefresh);
				if(nActiveTime > nNowTime || nActiveTime == 0 || nActiveTime == tLastRefresh)
				{
					return RE_FAIL;
				}
			}
		}
	}

	MySyicalShopMark& rMySyicalShopMark = pPlayer->GetMySyicalShopMark();
	mem::vector<uint32_t>& rItemList = rMySyicalShopMark.m_itemidlist;
	rItemList.clear();

	for(ShopChooseNodeMap::iterator iter = m_ChooseMap.begin(); iter != m_ChooseMap.end(); iter++)
	{
		shopChooseList& choList = iter->second;
		uint32_t nRandNum = rand()%choList.m_nTotalWeight;
		uint32_t nIndex = 0;
		bool bAdd = false;
		for(mem::list<shopChooseNode>::iterator it = choList.m_ChooseList.begin();it != choList.m_ChooseList.end(); it++)
		{
			if(nRandNum >= nIndex && nRandNum < nIndex + it->m_nWeight)
			{
				rItemList.push_back(it->m_nID);
				bAdd = true;
				break;
			}
			nIndex += it->m_nWeight;
		}
		if(bAdd == false && !choList.m_ChooseList.empty())
		{
			rItemList.push_back(choList.m_ChooseList.rbegin()->m_nID);
		}
	}
	rMySyicalShopMark.m_buyCntMark.clear();
	rMySyicalShopMark.m_resetCntMark.clear();
	rMySyicalShopMark.m_nTimeStamp = nNowTime;

	pPlayer->SetDBSaveMaskFlag(LPDST_CHAR_BASE_INFO, true);
	pPlayer->sendMysticalShopInfo();
	return RE_SUCCESS;
}

ResultType ServerMysticalShop::BuyItem(LC_ServerPlayer* pPlayer, uint32_t itemIndex, uint32_t nRefreshTime)
{
	if(pPlayer == NULL)
	{
		return RE_FAIL;
	}

	if(nRefreshTime != pPlayer->GetMysticalShopTimeStamp())
	{
		return RE_MYSTICALSHOP_BUY_CNT;
	}

	mem::vector<uint32_t>& rItemList = pPlayer->GetMysticalShopItemList();
	if(itemIndex < 0 || itemIndex >= rItemList.size())
	{
		return RE_MYSTICALSHOP_ID_ERR;
	}

	uint32_t nShopID = rItemList[itemIndex];
	CF_MysticalShop* pkConfig = CF_MysticalShop::GetSingletonPtr();
	CF_ShopList* pkShopList = CF_ShopList::GetSingletonPtr();

	const CF_MysticalShop::DataEntry * pMysticalShop = pkConfig->GetEntryPtr(nShopID);
	if(NULL == pMysticalShop)
	{
		return RE_MYSTICALSHOP_ID_ERR;
	}

	const CF_ShopList::DataEntry * pShop = pkShopList->GetEntryPtr(pMysticalShop->_iShopID);
	if(NULL == pShop)
	{
		return RE_MYSTICALSHOP_ID_ERR;
	}

	uint32_t nBuyCnt = pPlayer->GetMysticalShopBuyCnt(nShopID);
	if(nBuyCnt >= (uint32_t)pMysticalShop->_iBuyCnt)
	{
		return RE_MYSTICALSHOP_BUY_CNT;
	}

	LC_ShopIDMap ShopIDMap;
	ShopIDMap[pMysticalShop->_iShopID] = 1;

	ResultType nRel = LC_Helper::IsShopAvailable(pPlayer, ShopIDMap);
	if (RE_SUCCESS != nRel)
	{
		return nRel;
	}

	nRel = LC_Helper::PayForShop(pPlayer, ShopIDMap, SYS_FUNC_TP_MYSTICAL_SHOP, nShopID);
	if(nRel == RE_SUCCESS)
	{
		pPlayer->SetMysticalShopBuyCnt(nShopID, nBuyCnt + 1);
	}
	return nRel;
}

ResultType ServerMysticalShop::ResetBuyCount(LC_ServerPlayer* pPlayer, uint32_t itemIndex, uint32_t nRefreshTime)
{
	if(pPlayer == NULL)
	{
		return RE_FAIL;
	}

	if(nRefreshTime != pPlayer->GetMysticalShopTimeStamp())
	{
		return RE_MYSTICALSHOP_BUY_CNT;
	}

	mem::vector<uint32_t>& rItemList = pPlayer->GetMysticalShopItemList();
	if(itemIndex < 0 || itemIndex >= rItemList.size())
	{
		return RE_MYSTICALSHOP_RESET_MAX;
	}

	uint32_t nShopID = rItemList[itemIndex];
	CF_MysticalShop* pkConfig = CF_MysticalShop::GetSingletonPtr();
	CF_MysticalShopVip* pkConfigVip = CF_MysticalShopVip::GetSingletonPtr();
	CF_MysticalShopReset* pkConfigReset = CF_MysticalShopReset::GetSingletonPtr();
	const CF_MysticalShop::DataEntry* pMysticalShop = pkConfig->GetEntryPtr(nShopID);
	if(NULL == pMysticalShop)
	{
		return RE_MYSTICALSHOP_ID_ERR;
	}

	uint32_t nBuyCnt = pPlayer->GetMysticalShopBuyCnt(nShopID);
	if((uint32_t)pMysticalShop->_iBuyCnt > nBuyCnt)
	{
		return RE_MYSTICALSHOP_BUY_CNT_HAS;
	}

	uint32_t nVipLvl = pPlayer->GetVIPLevel();
// 	if(nVipLvl <= 0)
// 	{
// 		return RE_MYSTICALSHOP_RESET_VIP_0;
// 	}

	const CF_MysticalShopVip::DataEntry* pVipCnt = pkConfigVip->GetEntryPtr(nVipLvl);
	if(pVipCnt == NULL)
	{
		pVipCnt = pkConfigVip->GetEntryPtr(pkConfigVip->GetCurrentMaxID()-1);
	}

	if(pVipCnt == NULL)
	{
		return RE_MYSTICALSHOP_RESET_VIP_MAX;
	}

	uint32_t nResetCnt = pPlayer->GetMysticalShopResetCnt(nShopID);
	if(pVipCnt->_iResetCnt >= 0 && nResetCnt >= (uint32_t)pVipCnt->_iResetCnt)
	{
		return RE_MYSTICALSHOP_RESET_MAX;
	}

	nResetCnt = nResetCnt + 1;
	const CF_MysticalShopReset::DataEntry* pResetCnt = pkConfigReset->GetEntryPtr(nResetCnt);
	if(pResetCnt == NULL)
	{
		pResetCnt = pkConfigReset->GetEntryPtr(pkConfigReset->GetCurrentMaxID()-1);
	}

	if(pResetCnt == NULL)
	{
		return RE_MYSTICALSHOP_ID_NONE;
	}

	if(pPlayer->ReduceCash(pResetCnt->_iCostNum, (CashType)pResetCnt->_iCostType, SYS_FUNC_TP_MYSTICAL_SHOP, pResetCnt->_iResetCnt) == false)
	{
		return RE_MYSTICALSHOP_RESET_CASH;
	}

	pPlayer->SetMysticalShopResetCnt(nShopID, nResetCnt);
	pPlayer->SetMysticalShopBuyCnt(nShopID, 0);

	return RE_SUCCESS;
}

ResultType	ServerMysticalShop::ReqRefresh(LC_ServerPlayer* pPlayer)
{
	if(pPlayer == NULL || m_pMysticalShop == NULL)
	{
		return RE_FAIL;
	}

	ResultType nRel = RE_SUCCESS;
	{
		nRel = LC_Helper::CheckCashMap(pPlayer, m_pMysticalShop->_rCost1);
		if(nRel == RE_SUCCESS)
		{
			nRel = LC_Helper::PayCashMap(pPlayer, m_pMysticalShop->_rCost1, SYS_FUNC_TP_MYSTICAL_SHOP, m_pMysticalShop->_iID);
		}
	}

	if(nRel != RE_SUCCESS)
	{
		nRel = LC_Helper::CheckCashMap(pPlayer, m_pMysticalShop->_rCost2);
		if(nRel == RE_SUCCESS)
		{
			nRel = LC_Helper::PayCashMap(pPlayer, m_pMysticalShop->_rCost2, SYS_FUNC_TP_MYSTICAL_SHOP, m_pMysticalShop->_iID);
		}
	}

	if(nRel != RE_SUCCESS)
	{
		return nRel;
	}

	RefreshItems(pPlayer, false);
	return RE_SUCCESS;
}
