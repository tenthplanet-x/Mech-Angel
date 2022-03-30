#include "LC_ServerPackEntryFactory.h"
#include "MG_GM2Game.h"
#include "NW_Helper.h"
#include "LC_ServerRewardManager.h"
#include "LC_ItemFactoryBase.h"
#include "LC_ItemBase.h"
#include "UT_MathBase.h"
#include "CF_RuneStar.h"

using namespace GameLogic;
using namespace Protocol;
using namespace Utility;

//---------------------------------------------------------------------------------
LC_ServerPackEntryFactory::LC_ServerPackEntryFactory()
: LC_PackEntryFactory()
{

}
//---------------------------------------------------------------------------------
bool LC_ServerPackEntryFactory::CreateItemEntry(int32_t lItemID, int32_t lItemCount, int32_t iCreateType, GameLogic::LC_BackPackEntryList& rEntryList,  int32_t bindMode, int32_t param1, int32_t param2, bool bAutoSplit)
{
	if (lItemID<=0 || lItemCount<=0)
	{
		return false;
	}

	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	LC_ItemBase* pkItem = pkItemFactory->RequestItem(lItemID);
	if(NULL == pkItem)
	{
		return false;
	}

	int32_t lNeedCreateCount = lItemCount;
	while( lNeedCreateCount > 0)
	{	
		int32_t lOverlapCount = lNeedCreateCount;
		if (bAutoSplit && !IsSpecialItemTypeID(lItemID))
		{
	 		if ( lNeedCreateCount > pkItem->GetMaxOverlapCount())
			{
	 			lOverlapCount = pkItem->GetMaxOverlapCount();
	 		}
		}

		LC_BackPackEntry kResult;
		kResult.Reset();

		//设置基本数据
		kResult.SetBackPackValueByType(LPSFET_VALID, true);
		kResult.SetBackPackValueByType(LPSFET_TYPE_ID, lItemID);
		kResult.SetBackPackValueByType(LPSFET_OVERLAP_COUNT,lOverlapCount);
		kResult.SetBackPackValueByType(LCSPET_SRC_1, param1);
		kResult.SetBackPackValueByType(LCSPET_SRC_2, param2);		
		kResult.SetBackPackValueByType(LCSPET_EQUIP_AFFILIATED_SKILL, pkItem->GetAffiliatedSkill());
		kResult.SetBackPackValueByType(LCSPET_EQUIP_STAR, pkItem->GetInitStar());

		//设定绑定模式
		int mode = bindMode==-1 ? pkItem->GetBindMode() : bindMode;
		kResult.SetBackPackValueByType(LPSFET_BINDMODE, mode);

		//宝具的初始等级是：pkItemData->_lRuneLevel  装备的初始等级是：0
		int runeLevel = LC_Helper::CheckIsRune(lItemID) ? pkItem->GetRuneLevel() : 0;
		kResult.SetBackPackValueByType(LCSPET_EQUIP_CURRENT_REINFORCE_LEVEL, runeLevel);

		rEntryList.push_back(kResult);
		lNeedCreateCount -= lOverlapCount;
	}

	return true;
}
bool LC_ServerPackEntryFactory::CreateItemEntry(const Utility::UT_SIMDataInfo& itemInfo, int32_t iCreateType, GameLogic::LC_BackPackEntryList& rEntryList, bool bAutoSplit)
{
	int32_t itemID = (int32_t)itemInfo.ID();
	int32_t itemNum = (int32_t)itemInfo.ItemCount();
	if(itemID <= 0 || itemNum <=0 )
	{
		return false;
	}

	int32_t bindMode = (int32_t)itemInfo.ParamB();
	int32_t bestGuildGoalID = (int32_t)itemInfo.ParamC();
	int32_t gameStoryID = (int32_t)itemInfo.ParamD();

	return CreateItemEntry( itemID, itemNum, iCreateType, rEntryList, bindMode, bestGuildGoalID, gameStoryID, bAutoSplit);

}
void LC_ServerPackEntryFactory::LoadItemLimit(char* pMsg, uint16_t wSize)
{
	MG_GMG_RLT_Load_Item_Control kRltBody;
	if (!kRltBody.Decode(pMsg, wSize))
	{
		return;
	}

	m_akLimitInfo.clear();

	size_t tSize = kRltBody.m_akLimitInfo.size();
	for (size_t i=0; i<tSize; ++i)
	{
		m_akLimitInfo.push_back(kRltBody.m_akLimitInfo[i]);
	}
}

void LC_ServerPackEntryFactory::UpdateItemLimit(char* pMsg, uint16_t wSize)
{
	MG_GMG_RLT_Update_Item_Control kRltBody;
	if (!kRltBody.Decode(pMsg, wSize))
	{
		return;
	} 

	size_t tSize = m_akLimitInfo.size();
	for (size_t i=0; i<tSize; ++i)
	{
		if (m_akLimitInfo[i].m_iItemID == kRltBody.m_kLimitInfo.m_iItemID)
		{
			m_akLimitInfo[i] = kRltBody.m_kLimitInfo;
			break;
		}
	}
}
