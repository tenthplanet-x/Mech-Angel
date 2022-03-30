#include "ClientPCH.h"
#include "LC_TooltipDirector.h"

#include "LC_ConsumeItemTooltip.h"
#include "LC_EquipItemTooltip.h"
#include "LC_SkillBookItemTooltip.h"
#include "LC_MaterialItemTooltip.h"
#include "LC_SkillTooltipBase.h"
#include "LC_SkillStateTooltip.h"
#include "LC_MeditateTooltip.h"
#include "LC_GameStoryTooltip.h"

#include "CF_ItemList.h"
#include "CF_ItemCategory.h"

using namespace GameLogic;
using namespace CSVFile;
using namespace Utility;


LC_TooltipDirector::LC_TooltipDirector(void)
: m_pkConsumItemTooltip(NULL)
, m_pkEquipItemTooltip(NULL)
, m_pkSkillBookTooltip(NULL)
, m_pkMaterialTooltip(NULL)
, m_pkSkillTooltipBase(NULL)
, m_pkSkillStateTooltip(NULL)
, m_pkMeditateTooltip(NULL)
, m_pkGameStoryTooltip(NULL)
{
	Init();
}

LC_TooltipDirector::~LC_TooltipDirector(void)
{
	UnInit();
}
void LC_TooltipDirector::Init()
{
	m_pkConsumItemTooltip = T_NEW_D LC_ConsumeItemTooltip();
	m_pkEquipItemTooltip = T_NEW_D LC_EquipItemTooltip();
	m_pkSkillBookTooltip = T_NEW_D LC_SkillBookItemTooltip();	
	m_pkMaterialTooltip = T_NEW_D LC_MaterialItemTooltip();
	m_pkSkillTooltipBase = T_NEW_D LC_SkillTooltipBase();
	m_pkSkillStateTooltip = T_NEW_D LC_SkillStateTooltip();
	m_pkMeditateTooltip = T_NEW_D LC_MeditateTooltip();
	m_pkGameStoryTooltip = T_NEW_D LC_GameStoryTooltip();
}
void LC_TooltipDirector::UnInit()
{
	T_SAFE_DELETE(m_pkConsumItemTooltip);
	T_SAFE_DELETE(m_pkEquipItemTooltip);
	T_SAFE_DELETE(m_pkSkillBookTooltip);	
	T_SAFE_DELETE(m_pkMaterialTooltip);
	T_SAFE_DELETE(m_pkSkillTooltipBase);
	T_SAFE_DELETE(m_pkSkillStateTooltip);
	T_SAFE_DELETE(m_pkMeditateTooltip);
	T_SAFE_DELETE(m_pkGameStoryTooltip);
}


LC_TooltipBase::TooltipFieldVector
LC_TooltipDirector::ConstructItemTooltip( int lItemID, LC_BackPackEntry* pkBackPackEntry, LC_NPCShopItemEntry* pkShopEntry, LC_PlayerBase* pkObserver,
										 bool bCompareSelfEquip, LC_BackPackEntry *pkCurrentEquip, LC_ClientRankUserInfo *pkRankUserInfo, bool bShopOpen, 
										 int iExistFuncId, bool bGuildWarehouse, int iStarry)
{
	CF_ItemList* pkItemList = CF_ItemList::GetSingletonPtr();
	CF_ItemList::DataEntry* pkDataEntry = pkItemList->GetEntryPtr(lItemID);
	if(!pkDataEntry)
		return LC_TooltipBase::TooltipFieldVector();

	CF_ItemCategory* pkItemCategory = CF_ItemCategory::GetSingletonPtr();
	CF_ItemCategory::DataEntry* pkItemCategoryDataEntry = pkItemCategory->GetEntryPtr(pkDataEntry->_lCategory1);
	int lMainCateogory = 0;
	if ( 0 == pkItemCategoryDataEntry )
	{
		GfxWriteLog( LOG_TAG_ERROR, LOG_SWITCH_ERROR, "ItemList.csv gets foreign data ERROR: In ItemCategory.csv key==%ld  is not found!" ,pkDataEntry->_lCategory1 );
	}
	else
	{
		lMainCateogory = pkItemCategoryDataEntry->_iID;	//根据ItemCategory获取物品类型
	}

	CF_ItemExistenceFunc::DataEntry *pkExistFunc = SafeGetCSVFileDataEntryPtr<CF_ItemExistenceFunc>(iExistFuncId);
	/*StringStreamType kStringStream;
	kStringStream << lItemID;
	StringType kItemCategory = kStringStream.str();
	int iMainCateogy = atoi(kItemCategory.substr(0, 2).c_str());*/

	if(pkDataEntry->_lCategory2 == ITEM_CATEGORY_EQUIP_COST || pkDataEntry->_lCategory2 == ITEM_CATEGORY_SUB_EQUIP_COST)
	{
		//洗炼代币特殊处理，其类型为装备，但是又不是装备
		m_pkMaterialTooltip->Init(pkDataEntry, pkBackPackEntry, pkShopEntry, pkObserver, pkExistFunc, bShopOpen, bGuildWarehouse);
		m_pkMaterialTooltip->BuildTooltip();
		return m_pkMaterialTooltip->GetTooltipFields();
	}
	else
	{
		switch (lMainCateogory)
		{
		case ITEM_CATEGORY_PIECES:
			{
				if(pkDataEntry->_lCategory2 == ITEM_CATEGORY_RUNE_PIECES 
					|| pkDataEntry->_lCategory2 == ITEM_CATEGORY_RECIPE_PIECES)
				{
					m_pkMaterialTooltip->Init(pkDataEntry, pkBackPackEntry, pkShopEntry, pkObserver, pkExistFunc, bShopOpen, bGuildWarehouse);
					m_pkMaterialTooltip->BuildTooltip();
					return m_pkMaterialTooltip->GetTooltipFields();
				}
				else
				{
					m_pkSkillBookTooltip->Init(pkDataEntry, pkBackPackEntry, pkShopEntry, pkObserver, pkExistFunc, bShopOpen, bGuildWarehouse);
					m_pkSkillBookTooltip->BuildTooltip();
					return m_pkSkillBookTooltip->GetTooltipFields();
				}
			}
			break;
		case ITEM_CATEGORY_CONSUME:											//消耗类
		case ITEM_CATEGORY_DISASSEMBLE:
		case ITEM_CATEGORY_TASK:
		case ITEM_CATEGORY_STARSOURL:
			{
				if(pkDataEntry->_lCategory2 == ITEM_CATEGORY_CONSUME_CHARACTER	|| pkDataEntry->_lCategory2 == ITEM_CATEGORY_CONSUME_PETTOY)
				{
					m_pkSkillBookTooltip->Init(pkDataEntry, pkBackPackEntry, pkShopEntry, pkObserver, pkExistFunc, bShopOpen, bGuildWarehouse);
					m_pkSkillBookTooltip->BuildTooltip();
					return m_pkSkillBookTooltip->GetTooltipFields();
				}
				else
				{
					m_pkConsumItemTooltip->Init(pkDataEntry, pkBackPackEntry, pkShopEntry, pkObserver, pkExistFunc, bShopOpen, bGuildWarehouse);
					if(pkDataEntry->_lCategory2 == ITEM_CATEGORY_MULTI_GIFTBAG)
						m_pkConsumItemTooltip->BuildMultiGiftTooltip(bCompareSelfEquip);
					else
						m_pkConsumItemTooltip->BuildTooltip(bCompareSelfEquip);

					return m_pkConsumItemTooltip->GetTooltipFields();
				}
			}			
			break;
		case ITEM_CATEGORY_EQUIP:												//装备类
		case ITEM_CATEGORY_SUB_EQUIP:
			m_pkEquipItemTooltip->Init(pkDataEntry, pkBackPackEntry, pkShopEntry, pkObserver, pkRankUserInfo, pkExistFunc, bShopOpen, bGuildWarehouse, iStarry);
			m_pkEquipItemTooltip->SetCurrentEquip(pkCurrentEquip);
			m_pkEquipItemTooltip->BuildTooltip(bCompareSelfEquip);
			return m_pkEquipItemTooltip->GetTooltipFields();
			break;
		case ITEM_CATEGORY_SKILLBOOK:												//技能书类
			pkItemCategoryDataEntry = pkItemCategory->GetEntryPtr(pkDataEntry->_lCategory2);
			if (0== pkItemCategoryDataEntry )
			{
				GfxWriteLog( LOG_TAG_ERROR, LOG_SWITCH_ERROR, "ItemList.csv gets foreign data ERROR: In ItemCategory.csv key==%ld  is not found!" , pkDataEntry->_lCategory2 );
			}
			else if (pkItemCategoryDataEntry->_iID == ITEM_CATEGORY_CONSUME_TRANSFORM
				|| pkItemCategoryDataEntry->_iID == ITEM_CATEGORY_CONSUME_BUDDYTRANSFORM)			//技能书
			{
				m_pkSkillBookTooltip->Init(pkDataEntry, pkBackPackEntry, pkShopEntry, pkObserver, pkExistFunc, bShopOpen, bGuildWarehouse);
				m_pkSkillBookTooltip->BuildTooltip();
				return m_pkSkillBookTooltip->GetTooltipFields();
			}
			break;
		case ITEM_CATEGORY_MATERIAL:												//材料类
		case ITEM_CATEGORY_DEBRIS:
			m_pkMaterialTooltip->Init(pkDataEntry, pkBackPackEntry, pkShopEntry, pkObserver, pkExistFunc, bShopOpen, bGuildWarehouse);
			m_pkMaterialTooltip->BuildTooltip();
			return m_pkMaterialTooltip->GetTooltipFields();
			break;
		default:
			break;
		}
	}

	return LC_TooltipBase::TooltipFieldVector();
}

LC_TooltipBase::TooltipFieldVector 
LC_TooltipDirector::ConstructEquipEnhanceTooltip(int lItemID, LC_BackPackEntry* pkBackPackEntry, LC_PlayerBase* pkObserver)
{
	CF_ItemList* pkItemList = CF_ItemList::GetSingletonPtr();
	CF_ItemList::DataEntry* pkDataEntry = pkItemList->GetEntryPtr(lItemID);
	if(!pkDataEntry)
		return LC_TooltipBase::TooltipFieldVector();

	CF_ItemCategory* pkItemCategory = CF_ItemCategory::GetSingletonPtr();
	CF_ItemCategory::DataEntry* pkItemCategoryDataEntry = pkItemCategory->GetEntryPtr(pkDataEntry->_lCategory1);
	int lMainCateogory = 0;
	if ( 0 == pkItemCategoryDataEntry )
	{
		GfxWriteLog( LOG_TAG_ERROR, LOG_SWITCH_ERROR, "ItemList.csv gets foreign data ERROR: In ItemCategory.csv key==%ld  is not found!", pkDataEntry->_lCategory1 );
	}
	else
	{
		lMainCateogory = pkItemCategoryDataEntry->_iID;	//根据ItemCategory获取物品类型
	}

//	LC_TooltipBase* pkToolTipBase = NULL;

	if (ITEM_CATEGORY_EQUIP == lMainCateogory)
	{
		m_pkEquipItemTooltip->Init(pkDataEntry, pkBackPackEntry, NULL, pkObserver);
		m_pkEquipItemTooltip->BuildEnhanceTooltip();
		return m_pkEquipItemTooltip->GetTooltipFields();
	}
	return LC_TooltipBase::TooltipFieldVector();

}

LC_TooltipBase::TooltipFieldVector LC_TooltipDirector::ConstructEquipCompotisionTooltip(int lItemID, int iCompotisionLevel, LC_PlayerBase* pkObserver)
{
	CF_ItemList* pkItemList = CF_ItemList::GetSingletonPtr();
	CF_ItemList::DataEntry* pkDataEntry = pkItemList->GetEntryPtr(lItemID);
	if(!pkDataEntry)
		return LC_TooltipBase::TooltipFieldVector();

	CF_ItemCategory* pkItemCategory = CF_ItemCategory::GetSingletonPtr();
	CF_ItemCategory::DataEntry* pkItemCategoryDataEntry = pkItemCategory->GetEntryPtr(pkDataEntry->_lCategory1);
	int lMainCateogory = 0;
	if ( 0 == pkItemCategoryDataEntry )
	{
		GfxWriteLog( LOG_TAG_ERROR, LOG_SWITCH_ERROR, "ItemList.csv gets foreign data ERROR: In ItemCategory.csv key==%ld  is not found!", pkDataEntry->_lCategory1 );
	}
	else
	{
		lMainCateogory = pkItemCategoryDataEntry->_iID;	//根据ItemCategory获取物品类型
	}
//	LC_TooltipBase* pkToolTipBase = NULL;

	if (ITEM_CATEGORY_EQUIP == lMainCateogory)
	{
		m_pkEquipItemTooltip->Init(pkDataEntry, NULL, NULL, pkObserver);
		m_pkEquipItemTooltip->BuildCompositionTooltip(iCompotisionLevel);
		return m_pkEquipItemTooltip->GetTooltipFields();
	}
	return LC_TooltipBase::TooltipFieldVector();
}

LC_TooltipBase::TooltipFieldVector 
LC_TooltipDirector::ConstructSkillTooltip( int lSkillID, LC_PlayerBase* pkObserver, bool bCurrentLevelOnly)
{
	m_pkSkillTooltipBase->Init(lSkillID, pkObserver);
	m_pkSkillTooltipBase->BuildTooltip(bCurrentLevelOnly);
	return m_pkSkillTooltipBase->GetTooltipFields();
}

LC_TooltipBase::TooltipFieldVector 
LC_TooltipDirector::ConstructSkillStateTooltip( int lSkillStateID, LC_PlayerBase* pkObserver )
{
	m_pkSkillStateTooltip->Init(lSkillStateID, pkObserver);
	m_pkSkillStateTooltip->BuildTooltip();
	return m_pkSkillStateTooltip->GetTooltipFields();
}


//LC_TooltipBase::TooltipFieldIterator 
//LC_TooltipDirector::ConstructItemTooltip( int lItemID, LC_BackPackEntry* pkBackPackEntry, LC_ActorBase* pkObserver )
//{
//	CF_ItemList* pkItemList = CF_ItemList::GetSingletonPtr();
//	CF_ItemList::DataEntry* pkDataEntry = pkItemList->GetEntryPtr(lItemID);
//	//int lItemCategory = pkDataEntry->_lCategory1._iID;							//根据ItemCategory获取物品类型
//	int lItemCategory = pkDataEntry->_iID;											//根据物品ID号获取物品类型
//
//	StringStreamType kStringStream;
//	kStringStream << lItemCategory;
//	StringType kItemCategory = kStringStream.str();
//	int iMainCateogy = atoi(kItemCategory.substr(0, 2).c_str());
//
//	LC_TooltipBase* pkToolTipBase = 0;
//	switch (iMainCateogy)
//	{
//	case 11:
//		pkToolTipBase = LC_ConsumeItemTooltip::GetSingletonPtr();
//		break;
//	case 12:
//		pkToolTipBase = LC_EquipItemTooltip::GetSingletonPtr();
//		break;
//	case 14:
//		pkToolTipBase = LC_SkillBookItemTooltip::GetSingletonPtr();
//		break;
//	default:
//		break;
//	}
//
//	pkToolTipBase->BuildTooltip(lItemID, pkBackPackEntry, pkObserver);
//	return pkToolTipBase->GetIterator();
//}
//
//LC_TooltipBase::TooltipFieldIterator 
//LC_TooltipDirector::ConstructSkillTooltip( int lItemID, LC_ActorBase* pkObserver )
//{
//	LC_SkillTooltipBase* pkSkillTooltip = LC_SkillTooltipBase::GetSingletonPtr();
//	pkSkillTooltip->BuildTooltip(lItemID, 0, pkObserver);
//	return pkSkillTooltip->GetIterator();
//}
//
//LC_TooltipBase::TooltipFieldIterator 
//LC_TooltipDirector::ConstructSkillStateTooltip( int lItemID, LC_ActorBase* pkObserver )
//{
//	LC_SkillStateTooltip* pkSkillStateTooltip = LC_SkillStateTooltip::GetSingletonPtr();
//	pkSkillStateTooltip->BuildTooltip(lItemID, 0, pkObserver);
//	return pkSkillStateTooltip->GetIterator();
//}


LC_TooltipBase::TooltipFieldVector LC_TooltipDirector::ConstructMeditateInfoTooltip(LC_PlayerBase *pkObserver)
{
	m_pkMeditateTooltip->Init(pkObserver);
	m_pkMeditateTooltip->BuildTooltip();
	return m_pkMeditateTooltip->GetTooltipFields();
}

LC_TooltipBase::TooltipFieldVector LC_TooltipDirector::ConstructGameStoryTooltip(int iGameStoryID, int iRaidInfoID, int iTotalCost, GameStoryInfo *pkGameStoryInfo)
{
	m_pkGameStoryTooltip->Init(iGameStoryID, iTotalCost, iRaidInfoID, pkGameStoryInfo);
	m_pkGameStoryTooltip->BuildTooltip();
	return m_pkGameStoryTooltip->GetTooltipFields();
}

StringType LC_TooltipDirector::GetItemAttributeFuncDescript(LC_ActorAttributeAppendMap& rkBaseAttributeAppendMap,LC_ActorAttributeAppendMap& rkAddAttributeAppendMap)
{
	return m_pkEquipItemTooltip->GetItemAttributeFuncDescript(rkBaseAttributeAppendMap,rkAddAttributeAppendMap);
}

StringType LC_TooltipDirector::GetAttributeFuncDescript(int lAttributeFuncID)
{
	return m_pkEquipItemTooltip->GetItemAttributeFuncDescript(lAttributeFuncID);
}

