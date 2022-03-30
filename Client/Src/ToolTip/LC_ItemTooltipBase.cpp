
#include "ClientPCH.h"
#include "LC_ItemTooltipBase.h"
#include "LM_LocaleManager.h"
#include "LC_ItemFactoryBase.h"
#include "LC_PackStruct.h"
#include "LC_ActorBase.h"
#include "CF_ItemList.h"
#include "LC_PlayerBase.h"
#include "LC_ClientGamePlayer.h"
#include "LC_ShopBase.h"
#include "LC_Helper.h"
#include "CF_ItemCategory.h"
#include "CF_ItemEquipLimitFunc.h"
#include "CF_ItemUseLimitFunc.h"
#include "CF_ItemPropertyFunc.h"
#include "CF_ItemExistenceFunc.h"

#include "CF_Helper.h"
//#include "atltime.h"

using namespace GameLogic;
using namespace CSVFile;

LC_ItemTooltipBase::LC_ItemTooltipBase(void)
: m_pkItemInfoEntry(NULL)
, m_pkBackPackEntry(NULL)
, m_pkShopEntry(NULL)
, m_bShopOpen(false)
, m_bGuildWarehouse(false)
, m_pkItemExistenceFuncEntry(NULL)
, m_iStarry(0)
{
}

LC_ItemTooltipBase::~LC_ItemTooltipBase(void)
{
}

void LC_ItemTooltipBase::Init(CSVFile::CF_ItemList::DataEntry *pkItemInfoEntry, LC_BackPackEntry* pkBackPackEntry, LC_NPCShopItemEntry* pkShopEntry, LC_PlayerBase* pkObserver,
							  CSVFile::CF_ItemExistenceFunc::DataEntry *pkItemExistenceFuncEntry, bool bShopOpen, bool bGuildWarehouse, int iStarry)
{
	m_pkItemInfoEntry = pkItemInfoEntry;
	m_pkBackPackEntry = pkBackPackEntry;
	m_pkShopEntry = pkShopEntry;
	m_bShopOpen = bShopOpen;
	m_bGuildWarehouse = bGuildWarehouse;
	m_pkItemExistenceFuncEntry = pkItemExistenceFuncEntry;
	m_iStarry = iStarry;
	SetObserver(pkObserver);
}

bool LC_ItemTooltipBase::_addTooltipFieldSelfEquipHit(TooltipFieldVector& kFieldVector)
{
	unsigned short usFont = FONT_LARGE;
	unsigned short usFontSize = FONTSIZE_LARGE;
	int iFontColor = GRAY;

	StringType kContent = GET_MSG(LC_ITEMTOOLTIPBASE_CPP_001);
	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_SELF_EQUIP_HIT, GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_002), true, kContent, usFont, usFontSize, iFontColor, 0, 0));

	return true;
}

bool LC_ItemTooltipBase::_addTooltipFieldName( TooltipFieldVector& kFieldVector )
{
	CF_ItemList::DataEntry* pkItemData = GetItemInfoEntry();
	if (NULL == pkItemData)
	{
		return false;
	}

	unsigned short usFont = FONT_LARGE;
	unsigned short usFontSize = FONTSIZE_LARGE;

	//物品品阶
	int lGrade = pkItemData->_lGrade;
	int iFontColor = GetGradeColor(lGrade);

	StringStreamType kItemNameStrStream;
	if(ITEM_CATEGORY_CONSUME_CHARACTER == pkItemData->_lCategory2 && pkItemData->_lProductID > 0)
	{
		const CF_Buddy::DataEntry *pkBuddy = SafeGetCSVFileDataEntryPtr<CF_Buddy>(pkItemData->_lProductID);
		int idx=UT_MathBase::ClampInt(lGrade-3,0,3);
		int sizex[4]= {11,21,33,22};
		if(pkBuddy != NULL)
		{
			kItemNameStrStream << "<color val=white><image val=";
			kItemNameStrStream << sizex[idx];
			kItemNameStrStream << ",12>Incompress2/Incompress2_image_HeroGrade" << idx << "</image>";
			kItemNameStrStream  << "<image val=28,28>Incompress/Attribute_image_系别" << (int)pkBuddy->job << "</image></color>";
			kItemNameStrStream<<"  ";
		}
	}
	else if(ITEM_CATEGORY_DEBRIS_BUDDY == pkItemData->_lCategory2 && pkItemData->_iTransformID > 0)
	{
		const CF_Buddy::DataEntry *pkBuddy = SafeGetCSVFileDataEntryPtr<CF_Buddy>(pkItemData->_iTransformID);
		int idx=UT_MathBase::ClampInt(lGrade-3,0,3);
		int sizex[4]= {11,21,33,22};
		if(pkBuddy != NULL)
		{
			kItemNameStrStream << "<color val=white><image val=";
			kItemNameStrStream << sizex[idx];
			kItemNameStrStream << ",12>Incompress2/Incompress2_image_HeroGrade" << idx << "</image>";
			kItemNameStrStream  << "<image val=28,28>Incompress/Attribute_image_系别" << (int)pkBuddy->job << "</image></color>";
			kItemNameStrStream<<"  ";
		}
	}
	
	kItemNameStrStream << pkItemData->_kName;

	if(ITEM_CATEGORY_CONSUME_MENTAL == pkItemData->_lCategory2)
	{
		CF_ItemPropertyFunc::DataEntry *pkItemProperty = SafeGetCSVFileDataEntryPtr<CF_ItemPropertyFunc>(pkItemData->_lPropertyFuncID);
		if(NULL != pkItemProperty)
		{
			bool bLearned = false;
			
			StringType &sExecScript = pkItemProperty->_sToExecScript;
			UT_BaseStringUtility::Trim(sExecScript);
			if("ItemUseOpenSkillBook" == sExecScript)
			{
				LC_PlayerBase *pkPlayer = GetObserver();
				if(pkPlayer)
				{
					int iID = atoi(pkItemProperty->_sScriptArgs.c_str());
					LC_SkillBookAsset& skillBookAsset = pkPlayer->GetSkillBookAsset();
					bLearned = skillBookAsset.GetLevel(iID) > 0;
				}
			}
			else
			{
				Utility::UT_SIMDataList	kSkills;
				Utility::ParseFmtString2DataIFs(pkItemProperty->_kSkills, kSkills);

				if(!kSkills.empty())
				{
					LC_ActorBase *pkObserver = GetObserver();
					if(NULL != pkObserver)
					{
						LC_SkillAsset* pkSkillAsset = ((LC_PlayerBase*)pkObserver)->GetSkillAsset();
						if(NULL != pkSkillAsset)
						{
							for (Utility::UT_SIMDataList::iterator it=kSkills.begin(); it!=kSkills.end(); ++it)
							{
								if (!it->IsValid())
								{
									continue;
								}

								if(pkSkillAsset->FindLearnedSkill(it->ID(), false) != NULL)
								{
									bLearned = true;
									break;
								}
							}
						}
					}
				}
			}

			if(bLearned)
				kItemNameStrStream << GET_MSG(LC_ITEMTOOLTIPBASE_CPP_002);
		}
	}
	
	else if(ITEM_CATEGORY_CONSUME_PETTOY == pkItemData->_lCategory2 && pkItemData->_lProductID > 0)
	{
		const CF_Pet::DataEntry *pkPet = SafeGetCSVFileDataEntryPtr<CF_Pet>(pkItemData->_lProductID);
		if(pkPet != NULL)
		{
			kItemNameStrStream << "<color val=white><image val=-1>" << pkPet->_qualityicon  << "</image>";
			kItemNameStrStream  << "<image val=-1>" << pkPet->_jobicon << "</image></color>";
		}
	}

#ifndef _SHIPPING
	kItemNameStrStream << " " << pkItemData->_iID;
#endif

	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_NAME, GET_MSG(LC_ITEMTOOLTIPBASE_CPP_003), true, kItemNameStrStream.str(), usFont, usFontSize, iFontColor, 20, 0));

	return true;
}

bool LC_ItemTooltipBase::_addTooltipFieldSubCategory(TooltipFieldVector& kFieldVector)
{
	CF_ItemList::DataEntry* pkItemData = GetItemInfoEntry();
	if (NULL == pkItemData)
	{
		return false;
	}
	
	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = WHITE;

	StringType kSubCategoryName = "";
	CF_ItemCategory* pkItemCategory = CF_ItemCategory::GetSingletonPtr();
	CF_ItemCategory::DataEntry* pkItemCategoryDataEntry = pkItemCategory->GetEntryPtr(pkItemData->_lCategory2);	
	int lSubCategory = 0;
	if (NULL == pkItemCategoryDataEntry)
	{
		GfxWriteLog( LOG_TAG_ERROR, LOG_SWITCH_ERROR, "ItemList.csv gets foreign data ERROR: In ItemCategory.csv key==%ld  is not found!" ,pkItemData->_lCategory2 );
	}
	else
	{
		lSubCategory = pkItemCategoryDataEntry->_iID;

		CF_ItemCategory::DataEntry* pkCategoryData = SafeGetCSVFileDataEntryPtr<CF_ItemCategory>(lSubCategory);
		if (NULL == pkCategoryData )
		{
			return false;
		}
		kSubCategoryName = pkCategoryData->_kName;
		int lParentLevel = pkItemCategoryDataEntry->_lParentLevel;
		if(ITEM_CATEGORY_CONSUME == lParentLevel || ITEM_CATEGORY_MATERIAL == lParentLevel || ITEM_CATEGORY_DISASSEMBLE == lParentLevel || 
			ITEM_CATEGORY_TASK == lSubCategory || ITEM_CATEGORY_SKILLBOOK == lSubCategory || ITEM_CATEGORY_EQUIP_COST == pkItemData->_lCategory2)
		{
			iFontColor = GREEN;
		}
	}

	
	if ("" == kSubCategoryName)
	{
		return false;
	}
	
	kFieldVector.push_back( LC_TooltipField(TOOLTIP_FIELD_ITEM_SUBCATEGORY, StringType(GET_MSG(LC_ITEMTOOLTIPBASE_CPP_004)), true, kSubCategoryName, usFont, usFontSize, iFontColor, 0, 0) );

	return true;
}

//bool LC_ItemTooltipBase::_addTooltipFieldLevel( TooltipFieldVector& kFieldVector )
//{
//	CF_ItemList* pkItemList = CF_ItemList::GetSingletonPtr();
//	CF_ItemList::DataEntry* pkItemData = pkItemList->GetEntryPtr( GetItemID() );
//	if (0 == pkItemData)
//	{
//		return false;
//	}
//
//	unsigned short usFont = FONT_MIDDLE;
//	unsigned short usFontSize = FONTSIZE_MIDDLE;
//	int iFontColor = WHITE;
//
//	StringStreamType kLevelStrStream;
//	kLevelStrStream << "物品等级 " << pkItemData->_lLevel;
//	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_LEVEL, "物品等级", true, kLevelStrStream.str(), usFont, usFontSize, iFontColor, 0, 0));
//
//	return true;
//}

bool LC_ItemTooltipBase::_addTooltipFieldBind( TooltipFieldVector& kFieldVector )
{
	CF_ItemList::DataEntry* pkItemData = GetItemInfoEntry();
	if (NULL == pkItemData)
	{
		return false;
	}

	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	LC_ItemBase* pkItem = pkItemFactory->RequestItem(pkItemData->_iID);
	if(!pkItem)
	{
		return false;
	}

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = GREEN;

	StringType kContent = "";
	LC_BackPackEntry* pkBackPackEntry = GetBackPackEntry();
	if (NULL != pkBackPackEntry && pkBackPackEntry->GetBind())
	{
		kContent = GET_MSG(LC_ITEMTOOLTIPBASE_CPP_005);
	}
	
	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_BIND, GET_MSG(LC_ITEMTOOLTIPBASE_CPP_009), true, kContent, usFont, usFontSize, iFontColor, 0, 0));

	return true;
}

bool LC_ItemTooltipBase::_addTooltipFieldDecomposition(TooltipFieldVector& kFieldVector)
{
	CF_ItemList::DataEntry* pkItemData = GetItemInfoEntry();
	if (NULL == pkItemData)
	{
		return false;
	}

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = BLUE;

	int lDecompositionFuncID = pkItemData->_lDecompositionFuncID;
	if (lDecompositionFuncID <= 0)
	{
		return false;
	}

	StringStreamType kStringStream;
	kStringStream << GET_MSG(LC_ITEMTOOLTIPBASE_CPP_010);
	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_DECOMPOSITION, GET_MSG(LC_ITEMTOOLTIPBASE_CPP_010), true, kStringStream.str(), usFont, usFontSize, iFontColor, 0, 0));
	
	return true;
}

bool LC_ItemTooltipBase::_addToolTipFieldOwnUnique( TooltipFieldVector& kFieldVector )
{
	CF_ItemList::DataEntry* pkItemData = GetItemInfoEntry();
	if (NULL == pkItemData)
	{
		return false;
	}

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = WHITE;

	bool bOwnUnique = pkItemData->_bOwnUnique;
	if (!bOwnUnique)
	{
		return false;
	}

	StringStreamType kStringStream;
	kStringStream << GET_MSG(LC_ITEMTOOLTIPBASE_CPP_011);
	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_OWN_UNIQUE, GET_MSG(LC_ITEMTOOLTIPBASE_CPP_011), true, kStringStream.str(), usFont, usFontSize, iFontColor, 0, 0));

	return true;
}

bool LC_ItemTooltipBase::_addTooltipFieldOverlapCount( TooltipFieldVector& kFieldVector )
{
	CF_ItemList::DataEntry* pkItemData = GetItemInfoEntry();
	if (NULL == pkItemData)
	{
		return false;
	}

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = WHITE;

	int lMaxOverlapCount = pkItemData->_lMaxOverlapCount;
	//不可叠加的物品不显示叠加信息
	if (lMaxOverlapCount <= 1)
	{
		return false;
	}

	StringStreamType kItemCountStrStream;
	kItemCountStrStream << GET_MSG(LC_ITEMTOOLTIPBASE_CPP_012) << lMaxOverlapCount;	
	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_OVERLAP_COUNT, GET_MSG(LC_ITEMTOOLTIPBASE_CPP_013), true, kItemCountStrStream.str(), usFont, usFontSize, iFontColor, 0, 0));

	return true;
}

bool LC_ItemTooltipBase::_addTooltipFieldOrigin( TooltipFieldVector& kFieldVector )
{
	/*CF_ItemList* pkItemList = CF_ItemList::GetSingletonPtr();
	CF_ItemList::DataEntry* pkItemData = pkItemList->GetEntryPtr( GetItemID() );
	if (0 == pkItemData)
	{
		return false;
	}

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = WHITE;

	StringType kContent = pkItemData->_kOrigin;
	if (kContent == "由生产获得")
	{
		bEnable = true;
		iFontColor = GREEN;
		kContent = "<由生产获得>";
		kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_ORIGIN, "物品来源", true, kContent, usFont, usFontSize, iFontColor, 0, 0));
	}
	*/

#pragma TODO("物品来源在游戏中记录，不在ItemList里面读取")

	return true;
}

bool LC_ItemTooltipBase::_addTooltipFieldSexLimit( TooltipFieldVector& kFieldVector )
{
	CF_ItemList::DataEntry* pkItemData = GetItemInfoEntry();
	if (NULL == pkItemData)
	{
		return false;
	}
	CF_ItemEquipLimitFunc* pkItemEquip = CF_ItemEquipLimitFunc::GetSingletonPtr();
	CF_ItemEquipLimitFunc::DataEntry* pkItemEquipData = pkItemEquip->GetEntryPtr( pkItemData->_lEquipLimitFuncID );
	if ( 0 == pkItemEquipData )
	{
		GfxWriteLog( LOG_TAG_ERROR, LOG_SWITCH_ERROR, "ItemList.csv gets foreign data ERROR: In ItemEquipLimitFunc.csv key==%ld  is not found!", pkItemData->_lEquipLimitFuncID );
		return false;
	}
	else if ( pkItemEquipData->_iID <= 0 )
	{
		return false;
	}

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = WHITE;

	int lSexLimit = pkItemEquipData->_lSexLimit;
	if (lSexLimit <= 0)
	{
		return false;
	}

	StringStreamType kSexStrStream;
	switch (lSexLimit)
	{
		//case 0:
		//	//kSexStrStream << "性别 通用";
		//	kSexStrStream << "";
		//	break;
	case 1:
		kSexStrStream << GET_MSG(LC_ITEMTOOLTIPBASE_CPP_014);
		break;
	case 2:
		kSexStrStream << GET_MSG(LC_ITEMTOOLTIPBASE_CPP_015);
		break;
	default:
		kSexStrStream << "";
		break;
	}

	//当不提供观察者时，统一设置成白色显示
	LC_ActorBase* pkObserver = GetObserver();
	if (NULL == pkObserver)
	{
		iFontColor = WHITE;
	}
	else
	{
		//1表示男，2表是女
		if (pkObserver->GetCharType() != lSexLimit)
		{
			iFontColor = RED;						//设置颜色为红色
		}
		else
		{
			iFontColor = WHITE;						//设置颜色为普通
		}
	}

	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_SEX_LIMIT, GET_MSG(LC_ITEMTOOLTIPBASE_CPP_016), true, kSexStrStream.str(), usFont, usFontSize, iFontColor, 0, 0));
	return true;
}

bool LC_ItemTooltipBase::addTooltipFieldSchoolLimit( TooltipFieldVector& kFieldVector )
{
	//门派跟武器一一对应，可以用武器来区分门派
	CF_ItemList::DataEntry* pkItemData = GetItemInfoEntry();
	if(NULL == pkItemData)
	{
		return false;
	}

	int lWeaponType = 0;
	int lWeaponLimit = 0;
	if(!IS_EQUIP_ID(pkItemData->_iID))
	{
		CF_ItemUseLimitFunc *pkItemLimit = CF_ItemUseLimitFunc::GetSingletonPtr();
		CF_ItemUseLimitFunc::DataEntry *pkItemLimitData = pkItemLimit->GetEntryPtr(pkItemData->_lUseLimitFuncID);
		if (NULL == pkItemLimitData)
		{
			GfxWriteLog(LOG_TAG_ERROR, LOG_SWITCH_ERROR, "ItemList.csv gets foreign data ERROR: In ItemUseLimitFunc.csv key==%ld  is not found!", pkItemData->_lUseLimitFuncID);
			return false;
		}

		lWeaponLimit = pkItemLimitData->_lWeaponLimit;
	}
	else
	{
		CF_ItemEquipLimitFunc *pkEquipLimit = CF_ItemEquipLimitFunc::GetSingletonPtr();
		CF_ItemEquipLimitFunc::DataEntry *pkEquipLimitData = pkEquipLimit->GetEntryPtr(pkItemData->_lEquipLimitFuncID);
		if (NULL == pkEquipLimitData)
		{
			GfxWriteLog(LOG_TAG_ERROR, LOG_SWITCH_ERROR, "ItemList.csv gets foreign data ERROR: In ItemEquipLimitFunc.csv key==%ld  is not found!", pkItemData->_lEquipLimitFuncID);
			return false;
		}

		lWeaponType = pkEquipLimitData->_lEquipType;
		lWeaponLimit = pkEquipLimitData->_lWeaponType;
	}
	
	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = WHITE;

	StringStreamType kWeaponStrStream;

	bool bCommon = false;
	if(pkItemData->_lCategory1 == ITEM_CATEGORY_SUB_EQUIP)
	{
		int iCategory2 = pkItemData->_lCategory2;
		if(iCategory2 >= ITEM_CATEGORY_VEHICLE_EQUIP_BEGIN && iCategory2 <= ITEM_CATEGORY_VEHICLE_EQUIP_END)
		{
			kWeaponStrStream << GET_MSG(LC_ITEMTOOLTIPBASE_CPP_020);
		}
		else if(iCategory2 >= ITEM_CATEGORY_GODWING_EQUIP_BEGIN && iCategory2 <= ITEM_CATEGORY_GODWING_EQUIP_END)
		{
			kWeaponStrStream << GET_MSG(LC_ITEMTOOLTIPBASE_CPP_021);
		}
		else if(iCategory2 >= ITEM_CATEGORY_DEVAEYE_EQUIP_BEGIN && iCategory2 <= ITEM_CATEGORY_DEVAEYE_EQUIP_END)
		{
			kWeaponStrStream << GET_MSG(LC_ITEMTOOLTIPBASE_CPP_022);
		}
		else if(iCategory2 >= ITEM_CATEGORY_ARROW_EQUIP_BEGIN && iCategory2 <= ITEM_CATEGORY_ARROW_EQUIP_END)
		{
			kWeaponStrStream << GET_MSG(LC_ITEMTOOLTIPBASE_CPP_023);
		}
		else if(iCategory2 >= ITEM_CATEGORY_VEHICLEFIGHTING_EQUIP_BEGIN && iCategory2 <= ITEM_CATEGORY_VEHICLEFIGHTING_EQUIP_END)
		{
			kWeaponStrStream << GET_MSG(LC_ITEMTOOLTIPBASE_CPP_024);
		}
		else if(iCategory2 >= ITEM_CATEGORY_SPIRITPET_EQUIP_BEGIN && iCategory2 <= ITEM_CATEGORY_SPIRITPET_EQUIP_END)
		{
			kWeaponStrStream << GET_MSG(LC_ITEMTOOLTIPBASE_CPP_025);
		}
		else if(iCategory2 >= ITEM_CATEGORY_SPIRITWEAPON_EQUIP_BEGIN && iCategory2 <= ITEM_CATEGORY_SPIRITWEAPON_EQUIP_END)
		{
			kWeaponStrStream << GET_MSG(LC_ITEMTOOLTIPBASE_CPP_026);
		}
		else if(iCategory2 >= ITEM_CATEGORY_HIDDENWEAPON_EQUIP_BEGIN && iCategory2 <= ITEM_CATEGORY_HIDDENWEAPON_EQUIP_END)
		{
			kWeaponStrStream << GET_MSG(LC_ITEMTOOLTIPBASE_CPP_027);
		}
		else if(iCategory2 >= ITEM_CATEGORY_PLOUGH_EQUIP_BEGIN && iCategory2 <= ITEM_CATEGORY_PLOUGH_EQUIP_END)
		{
			kWeaponStrStream << GET_MSG(LC_ITEMTOOLTIPBASE_CPP_028);
		}
		else if(iCategory2 >= ITEM_CATEGORY_EVIL_EQUIP_BEGIN && iCategory2 <= ITEM_CATEGORY_EVIL_EQUIP_END)
		{
			kWeaponStrStream << GET_MSG(LC_ITEMTOOLTIPBASE_CPP_029);
		}
		else if(iCategory2 >= ITEM_CATEGORY_FAN_EQUIP_BEGIN && iCategory2 <= ITEM_CATEGORY_FAN_EQUIP_END)
		{
			kWeaponStrStream << GET_MSG(LC_ITEMTOOLTIPBASE_CPP_056);
		}
		else if(iCategory2 >= ITEM_CATEGORY_SUPERBOW_EQUIP_BEGIN && iCategory2 <= ITEM_CATEGORY_SUPERBOW_EQUIP_END)
		{
			kWeaponStrStream << GET_MSG(LC_ITEMTOOLTIPBASE_CPP_057);
		}
		else if(iCategory2 >= ITEM_CATEGORY_HOOK_EQUIP_BEGIN && iCategory2 <= ITEM_CATEGORY_HOOK_EQUIP_END)
		{
			kWeaponStrStream << GET_MSG(LC_ITEMTOOLTIPBASE_CPP_058);
		}
		else if(iCategory2 >= ITEM_CATEGORY_SPIRITFAN_EQUIP_BEGIN && iCategory2 <= ITEM_CATEGORY_SPIRITFAN_EQUIP_END)
		{
			kWeaponStrStream << GET_MSG(LC_ITEMTOOLTIPBASE_CPP_061);
		}
		bCommon = true;
	}
	else
	{
		switch (lWeaponLimit)
		{
		case WT_SWORD:
			kWeaponStrStream << SCHOOL_NAME_WUDANG;
			break;
		case WT_KNIFE:
			kWeaponStrStream << SCHOOL_NAME_KUNLUN;
			break;
		case WT_STICK:
			kWeaponStrStream << SCHOOL_NAME_SHAOLIN;
			break;
		case WT_SPEAR:
			kWeaponStrStream << SCHOOL_NAME_KONGTONG;
			break;
		case WT_DOUBLE_KNIFE:
			kWeaponStrStream << SCHOOL_NAME_EMEI;
			break;
		case WT_CLAYMORE:
			kWeaponStrStream << SCHOOL_NAME_HUASHAN;
			break;
		default:
			kWeaponStrStream << SCHOOL_NAME_COMMON;
			bCommon = true;
			break;
		}
	}

	LC_ActorBase* pkObserver = GetObserver();
	if (NULL == pkObserver)
	{
		iFontColor = WHITE;
	}
	else
	{
		WeaponType kWeaponType = pkObserver->GetWeaponAttribute();
		if (kWeaponType != lWeaponLimit && !bCommon)
		{
			iFontColor = RED;
		}
		else
		{
			iFontColor = WHITE;
		}
	}

	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_SCHOOL_LIMIT, GET_MSG(LC_ITEMTOOLTIPBASE_CPP_030), true, kWeaponStrStream.str(), usFont, usFontSize, iFontColor, 0, 0));
	return true;
}
bool LC_ItemTooltipBase::_addTooltipFieldCareerLimit( TooltipFieldVector& kFieldVector )
{
	CF_ItemList::DataEntry* pkItemData = GetItemInfoEntry();
	if (NULL == pkItemData)
	{
		return false;
	}

	int lUseLimitID = 0; 
	int lCareerLimit = 0;
	CF_ItemUseLimitFunc* pkItemLimit = CF_ItemUseLimitFunc::GetSingletonPtr();
	CF_ItemUseLimitFunc::DataEntry* pkItemLimitData = pkItemLimit->GetEntryPtr( pkItemData->_lUseLimitFuncID );
	if ( 0 == pkItemLimitData )
	{
		GfxWriteLog( LOG_TAG_ERROR, LOG_SWITCH_ERROR, "ItemList.csv gets foreign data ERROR: In ItemUseLimitFunc.csv key==%ld  is not found!", pkItemData->_lUseLimitFuncID );
	}
	else
	{
		lUseLimitID = pkItemLimitData->_iID;
		lCareerLimit = pkItemLimitData->_lCareerLimit;
	}
	if (lUseLimitID <= 0)
	{
		return false;
	}

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = WHITE;

	//获取Observer的职业
	if (lCareerLimit <= 0)
	{
		return false;
	}

	LC_ActorBase* pkObserver = GetObserver();
	if (NULL != pkObserver && IS_PLAYER_ID(pkObserver->GetID()) && ((LC_PlayerBase*)pkObserver)->GetCareerType() != lCareerLimit)
	{
		iFontColor = RED;
	}
	StringStreamType kCareerStrStream;
	//kCareerStrStream << "需求心法 ";
	//switch (lCareerLimit)
	//{
	//	/*case 0:
	//	kCareerStrStream << "无需求";
	//	break;*/
	//case CT_SHENXIANG:
	//	kCareerStrStream << CT_SHENXIANG_NAME;
	//	break;
	//case CT_CHANGXIU:
	//	kCareerStrStream << CT_CHANGXIU_NAME;
	//	break;
	//case CT_BAIZHAN:
	//	kCareerStrStream << CT_BAIZHAN_NAME;
	//	break;
	//case CT_LINGSU:
	//	kCareerStrStream << CT_LINGSU_NAME;
	//	break;
	//case CT_TAYUE:
	//	kCareerStrStream << CT_TAYUE_NAME;
	//	break;
	//default:
	//	break;
	//}

	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_SKILL_BOOK_LIMIT, GET_MSG(LC_ITEMTOOLTIPBASE_CPP_031), true, kCareerStrStream.str(), usFont, usFontSize, iFontColor, 0, 0));
	return true;
}

//bool LC_ItemTooltipBase::_addTooltipFieldUseResultProperty( TooltipFieldVector& kFieldVector )
//{
//	CF_ItemList* pkItemList = CF_ItemList::GetSingletonPtr();
//	CF_ItemList::DataEntry* pkItemData = pkItemList->GetEntryPtr( GetItemID() );
//	if (0 == pkItemData)
//	{
//		return false;
//	}
//
//	unsigned short usFont = FONT_MIDDLE;
//	unsigned short usFontSize = FONTSIZE_MIDDLE;
//	int iFontColor = GREEN;
//
//	//瞬发效果
//	int lPropertyID = 0;
//	CF_ItemPropertyFunc* pkItemProperty = CF_ItemPropertyFunc::GetSingletonPtr();
//	CF_ItemPropertyFunc::DataEntry* pkItemPropertyData = pkItemProperty->GetEntryPtr( pkItemData->_lPropertyFuncID );
//	if ( 0 == pkItemPropertyData )
//	{
//		GfxWriteLog( LOG_TAG_ERROR, LOG_SWITCH_ERROR, "ItemList.csv gets foreign data ERROR: In ItemPropertyFunc.csv key==%ld  is not found!", pkItemData->_lPropertyFuncID );	
//	}
//	else
//	{
//		lPropertyID = pkItemPropertyData->_iID;
//	}
//	if (lPropertyID <= 0)
//	{
//		return false;
//	}
//	StringType kUseResult = "";
//	if (  !(LC_TooltipBase::_getUseResult<CSVFile::CF_ItemPropertyFunc>(lPropertyID, kUseResult))  )
//	{
//		return false;
//	}
//	kUseResult = "使用：" + kUseResult;
//	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_USE_RESULT, "瞬发效果", true, kUseResult, usFont, usFontSize, iFontColor, 0, 0));
//	return true;
//}

//bool LC_ItemTooltipBase::_addTooltipFieldUseResultTriggerSkill( TooltipFieldVector& kFieldVector )
//{
//	CF_ItemList* pkItemList = CF_ItemList::GetSingletonPtr();
//	CF_ItemList::DataEntry* pkItemData = pkItemList->GetEntryPtr( GetItemID() );
//	CF_ItemCategory* pkItemCategory = CF_ItemCategory::GetSingletonPtr();
//	CF_ItemCategory::DataEntry* pkItemCategoryDataEntry = NULL;
//	if (0 == pkItemData)
//	{
//		return false;
//	}
//
//	unsigned short usFont = FONT_MIDDLE;
//	unsigned short usFontSize = FONTSIZE_MIDDLE;
//	int iFontColor = GREEN;
//
//	StringType kUseResult = "";
//	//技能消耗类物品的使用效果从描述中读取
//	pkItemCategoryDataEntry = pkItemCategory->GetEntryPtr(pkItemData->_lCategory2);	
//	if ( 0 == pkItemCategoryDataEntry )
//	{
//		GfxWriteLog( LOG_TAG_ERROR, LOG_SWITCH_ERROR, "ItemList.csv gets foreign data ERROR: In ItemCategory.csv key==%ld  is not found!" ,pkItemData->_lCategory2 );
//	}
//	else if (1112 == pkItemCategoryDataEntry->_iID)
//	{
//		kUseResult = pkItemData->_kDescript;
//	}
//	else
//	{
//		//触发技能
//		int lTriggerSkillID = pkItemData->_lUseTriggerSkillTypeID;
//		if (lTriggerSkillID <= 0)
//		{
//			return false;
//		}
//
//		CF_SkillExecuteList* pkSkillList = CF_SkillExecuteList::GetSingletonPtr();
//		CF_SkillExecuteList::DataEntry* pkSkillData = pkSkillList->GetEntryPtr(lTriggerSkillID);
//		if (0 == pkSkillData)
//		{
//			GfxWriteLog(LOG_ERROR_FILELINE, Utility::LOG_SWITCH_ERROR, "Can't find %d in SkillExecuteList!", lTriggerSkillID);
//			return false;
//		}
//		if ( !(LC_TooltipBase::_getUseResult<CSVFile::CF_SkillExecuteList>(lTriggerSkillID, kUseResult)) )
//		{
//			return false;
//		}
//	}
//	if ("" == kUseResult)
//	{
//		return false;
//	}
//	kUseResult = "使用:" + kUseResult;
//	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_USE_RESULT, "技能触发效果", true, kUseResult, usFont, usFontSize, iFontColor, 0, 0));
//	return true;
//}

bool LC_ItemTooltipBase::_addTooltipFieldUseCount( TooltipFieldVector& kFieldVector )
{
	CF_ItemList::DataEntry* pkItemData = GetItemInfoEntry();
	if(NULL == pkItemData)
	{
		return false;
	}

	////背包项为空，对于商店中的物品不涉及背包项
	//if (0 == pkBackPackEntry)
	//{
	//	return false;
	//}

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = BLUE;

	CF_ItemUseLimitFunc* pkItemLimit = CF_ItemUseLimitFunc::GetSingletonPtr();
	CF_ItemUseLimitFunc::DataEntry* pkItemLimitData = pkItemLimit->GetEntryPtr( pkItemData->_lUseLimitFuncID );
	int lUseLimit = 0;
	if ( NULL == pkItemLimitData )
	{
		GfxWriteLog( LOG_TAG_ERROR, LOG_SWITCH_ERROR, "ItemList.csv gets foreign data ERROR: In ItemUseLimitFunc.csv key==%ld  is not found!", pkItemData->_lUseLimitFuncID );
		return false;
	}
	else
	{
		lUseLimit = pkItemLimitData->_lCountLimit;
	}
	if (lUseLimit < 1)
	{
		return false;
	}

	int lCountLimit = pkItemLimitData->_lCountLimit;				//使用次数限制
	int lRemainCount = 0;
	//如果是商店中的物品，背包项为空，剩余使用次数为满次
	LC_BackPackEntry* pkBackPackEntry = GetBackPackEntry();
	if (NULL == pkBackPackEntry)
	{
		lRemainCount = lCountLimit;
	}
	
	StringStreamType kStringStream;
	
	//如果是加血、加内息类的容量类消耗物品，则需要显示具体点数
	CF_ItemCategory::DataEntry* pkItemCategoryDataEntry = SafeGetCSVFileDataEntryPtr<CF_ItemCategory>(pkItemData->_lCategory2);
	if(NULL == pkItemCategoryDataEntry)
	{
		GfxWriteLog( LOG_TAG_ERROR, LOG_SWITCH_ERROR, "ItemList.csv gets foreign data ERROR: In ItemCategory.csv key==%ld  is not found!" ,pkItemData->_lCategory2 );
	}
	else if(pkItemCategoryDataEntry->_iID == ITEM_CATEGORY_CONSUME_CAPACITY)
	{
		bool bAddHP = false;
		//int lAddMP = 0;
		CF_ItemPropertyFunc::DataEntry* pkItemPropertyData = SafeGetCSVFileDataEntryPtr<CF_ItemPropertyFunc>(pkItemData->_lPropertyFuncID);
		if(NULL == pkItemPropertyData)
		{
			GfxWriteLog( LOG_TAG_ERROR, LOG_SWITCH_ERROR, "ItemList.csv gets foreign data ERROR: In ItemPropertyFunc.csv key==%ld  is not found!", pkItemData->_lPropertyFuncID );
		}
		else
		{
			bAddHP = pkItemPropertyData->_lHPRecoveryValue > 0 || pkItemPropertyData->_lHPRecoveryPercent > 0;
			//lAddMP = pkItemPropertyData->_lMPRecoveryValue;
		}
		if (bAddHP)// && lAddMP <= 0)
		{
			kStringStream << GET_MSG(LC_ITEMTOOLTIPBASE_CPP_032);
		}
		else
		{
			return false;
		}
	}
	else if (lCountLimit <= 1)					//对于其它类型物品，如果使用次数小于等于1，则不显示使用次数
	{
		return false;
	}
	else
	{
		kStringStream << GET_MSG(LC_ITEMTOOLTIPBASE_CPP_033);
	}
	kStringStream << lRemainCount;

	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_USE_COUNT, GET_MSG(LC_ITEMTOOLTIPBASE_CPP_034), true, kStringStream.str(), usFont, usFontSize, iFontColor, 0, 0));
	return true;
}
bool LC_ItemTooltipBase::_addTooltipFieldPrice( TooltipFieldVector& kFieldVector )
{
	CF_ItemList::DataEntry* pkItemData = GetItemInfoEntry();
	if (NULL == pkItemData)
	{
		return false;
	}

	unsigned short usFont = FONTSIZE_XSMALL;
	unsigned short usFontSize = FONTSIZE_XSMALL;
	int iFontColor = WHITE;
	

	if (!pkItemData->_bSell || pkItemData->_sSoldCashs == "" || pkItemData->_sSoldCashs == "0")
	{
		StringStreamType kPriceStrStream;
		kPriceStrStream << GET_MSG(LC_ITEMTOOLTIPBASE_CPP_035);
		kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_PRICE, GET_MSG(LC_ITEMTOOLTIPBASE_CPP_036), true, kPriceStrStream.str(), usFont, usFontSize, iFontColor, 0, 0));
		return true;
	}


	StringStreamType kPriceStrStream;
		
	LC_NPCShopItemEntry* pkShopEntry = GetShopEntry();
	//如果商店不为空，则显示物品在商店中的价格
	if (NULL != pkShopEntry)
	{
		kPriceStrStream << GET_MSG(LC_ITEMTOOLTIPBASE_CPP_037);

		if(pkShopEntry->PriceType1() > 0 && pkShopEntry->PriceValue1() > 0)
		{
			kPriceStrStream << pkShopEntry->PriceValue1();
			kPriceStrStream << CASH_TYPE_NAME(pkShopEntry->PriceType1());
			kPriceStrStream << " ";
		}
		if(pkShopEntry->PriceType2() > 0 && pkShopEntry->PriceValue2() > 0)
		{
			kPriceStrStream << pkShopEntry->PriceValue2();
			kPriceStrStream << CASH_TYPE_NAME(pkShopEntry->PriceType2());
			kPriceStrStream << " ";
		}
	}
	else
	{
		kPriceStrStream << GET_MSG(LC_ITEMTOOLTIPBASE_CPP_038);

		TStringVector kVector = UT_BaseStringUtility::SplitString(pkItemData->_sSoldCashs, ';');
		for(int i=0; i<kVector.size(); ++i)
		{
			TStringVector kDetail = UT_BaseStringUtility::SplitString(kVector[i], ':');
			if(kDetail.size() == 2)
			{
				int iType = atoi(kDetail[0].c_str());
				int iValue = atoi(kDetail[1].c_str());
				kPriceStrStream << iValue << CASH_TYPE_NAME(iType);
				kPriceStrStream << " ";
			}
		}
	}

	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_PRICE, GET_MSG(LC_ITEMTOOLTIPBASE_CPP_036), true, kPriceStrStream.str(), usFont, usFontSize, iFontColor, 0, 0));
	
	return true;
}

bool LC_ItemTooltipBase::_addTooltipFieldGuildCash(TooltipFieldVector& kFieldVector)
{
	CF_ItemList::DataEntry* pkItemData = GetItemInfoEntry();
	if (NULL == pkItemData)
	{
		return false;
	}

	if(pkItemData->_GuildCashV <= 0)
	{
		return false;
	}

	StringStreamType kStrStream("");
	if(!m_bGuildWarehouse)
	{
		kStrStream << GET_MSG(LC_ITEMTOOLTIPBASE_CPP_039) << pkItemData->_GuildCashV;
	}
	else
		kStrStream << GET_MSG(LC_ITEMTOOLTIPBASE_CPP_040) << pkItemData->_GuildCashV*2;

	 kStrStream << "<color val=0xffffffff><image val=16>Item/Item_image_点数-橙色</image></color>";

	unsigned short usFont = FONTSIZE_XSMALL;
	unsigned short usFontSize = FONTSIZE_XSMALL;
	int iFontColor = WHITE;

	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_GUILD_CASH, GET_MSG(LC_ITEMTOOLTIPBASE_CPP_041), true, kStrStream.str(), usFont, usFontSize, iFontColor, 0, 0));
	
	return true;
}

bool LC_ItemTooltipBase::_addTooltipFieldDescript( TooltipFieldVector& kFieldVector )
{
	CF_ItemList::DataEntry* pkItemData = GetItemInfoEntry();
	if(NULL == pkItemData)
	{
		return false;
	}

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = GREEN;

	////不读取技能消耗类物品的描述项
	//CF_ItemCategory* pkItemCategory = CF_ItemCategory::GetSingletonPtr();
	//CF_ItemCategory::DataEntry* pkItemCategoryDataEntry = pkItemCategory->GetEntryPtr(pkItemData->_lCategory2);	
	//if ( 0 == pkItemCategoryDataEntry )
	//{
	//	GfxWriteLog( LOG_TAG_ERROR, LOG_SWITCH_ERROR, "ItemList.csv gets foreign data ERROR: In ItemCategory.csv key==%ld  is not found!" ,pkItemData->_lCategory2 );
	//}
	//else if (1112 == pkItemCategoryDataEntry->_iID)
	//{
	//	return false;
	//}
	StringType kContent = pkItemData->_kDescript;
	if (kContent == "" || kContent == "NULL")
	{
		return false;
	}

	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_DESCRIPT, GET_MSG(LC_GOODTOOLTIP_CPP_001), true, kContent, usFont, usFontSize, iFontColor, 0, 0));
	return true;
}

bool LC_ItemTooltipBase::_addTooltipDetailDescript(TooltipFieldVector& kFieldVector)
{
	CF_ItemList::DataEntry* pkItemData = GetItemInfoEntry();
	if (NULL == pkItemData)
	{
		return false;
	}

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = WHITE;

	StringType &kContent = pkItemData->_kDetailDescript;
	if (kContent == "" || kContent == "NULL")
	{
		return false;
	}

	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_DETAIL_DESCRIPT, GET_MSG(LC_GOODTOOLTIP_CPP_003), true, kContent, usFont, usFontSize, iFontColor, 0, 0));
	return true;
}

bool LC_ItemTooltipBase::_addTooltipFieldAtmosDescript(TooltipFieldVector& kFieldVector)
{
	CF_ItemList::DataEntry* pkItemData = GetItemInfoEntry();
	if (0 == pkItemData)
	{
		return false;
	}

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = GREEN;

	StringType &kContent = pkItemData->_kAtmosDescript;
	if (kContent == "" || kContent == "NULL")
	{
		return false;	
	}

	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_ATMOS_DESCRIPT, GET_MSG(LC_ITEMTOOLTIPBASE_CPP_042), true, kContent, usFont, usFontSize, iFontColor, 0, 0));
	return true;
}

bool LC_ItemTooltipBase::_addTooltipFieldTimeLimitDescript(TooltipFieldVector& kFieldVector)
{
	LC_BackPackEntry* pkEntry = GetBackPackEntry();
	if(NULL == pkEntry)
		return false;

	if(NULL == m_pkItemExistenceFuncEntry)
		return false;

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = BLUE;

	time_t ttRemainTime = (time_t)m_pkItemExistenceFuncEntry->_lTimeLimit * 60;

	StringStreamType kStringStream;
	kStringStream << GET_MSG(LC_ITEMTOOLTIPBASE_CPP_044);

	int iMinute = (int)ttRemainTime / 60;

	if (iMinute <= 60)
	{
		if(iMinute >= 1)
			kStringStream << iMinute << GET_MSG(LC_HELPER_CPP_011);
		else
			kStringStream << GET_MSG(LC_ITEMTOOLTIPBASE_CPP_045);

		if(iMinute < 10)
			iFontColor = RED;						//设置字体为红色
	}
	else
	{
		kStringStream << LC_Helper::FormateTimeStringByMinutes(iMinute);
	}

	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_ATMOS_DESCRIPT, GET_MSG(LC_ITEMTOOLTIPBASE_CPP_046), true, kStringStream.str(),usFont, usFontSize, iFontColor, 0, 0));

	return true;
}

bool LC_ItemTooltipBase::_addTooltipFieldBatchUseDescript(TooltipFieldVector& kFieldVector)
{
	LC_BackPackEntry *pkItemEntry = GetBackPackEntry();
	if(NULL == pkItemEntry)
	{
		return false;
	}

	CF_ItemList::DataEntry *pkItemInfo = GetItemInfoEntry();
	if(NULL == pkItemInfo)
		return false;

	if(!pkItemInfo->_bBatchUse)
		return false;

	if(pkItemEntry->GetOverlapCount() <= 1)
		return false;

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = YELLOW;
	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_BATCH_USE, "批量使用", true, "<image val=16>Common1/Common1_image_鼠标</image>右键点击批量使用", usFont, usFontSize, iFontColor, 0, 0));
	return true;
}

bool LC_ItemTooltipBase::_addTooltipFieldAddItemLinkInfo(TooltipFieldVector& kFieldVector)
{
	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = WHITE2;
	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ADDITEM_LINKINFO, GET_MSG(LC_ITEMTOOLTIPBASE_CPP_047), true, GET_MSG(LC_ITEMTOOLTIPBASE_CPP_048), usFont, usFontSize, iFontColor, 0, 0));
	return true;
}


bool LC_ItemTooltipBase::_addTooltipSplitInfo(TooltipFieldVector &kFieldVector)
{
	LC_BackPackEntry *pkItemEntry = GetBackPackEntry();
	if(NULL == pkItemEntry)
	{
		return false;
	}

	CF_ItemList::DataEntry *pkEntry = GetItemInfoEntry();
	if(NULL == pkEntry)
	{
		return false;
	}

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = GREEN;

	if(pkEntry->_lMaxOverlapCount > 1)
	{
		kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_SPLIT_TIP, "拆分提示", true, GET_MSG(LC_ITEMTOOLTIPBASE_CPP_059), usFont, usFontSize, iFontColor, 0, 0));
	}
	else if(pkEntry->_lCategory1 == ITEM_CATEGORY_DISASSEMBLE)
	{
		iFontColor = GRAY2;
		kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_SPLIT_TIP, "拆分提示", true, GET_MSG(LC_ITEMTOOLTIPBASE_CPP_060), usFont, usFontSize, iFontColor, 0, 0));
	}

	return true;
}

bool LC_ItemTooltipBase::_addTooltipSellTip(TooltipFieldVector &kFieldVector)
{
	LC_BackPackEntry *pkItemEntry = GetBackPackEntry();
	if(NULL == pkItemEntry)
	{
		return false;
	}

	CF_ItemList::DataEntry *pkEntry = GetItemInfoEntry();
	if(NULL == pkEntry)
	{
		return false;
	}

	if(!pkEntry->_bSell)
	{
		return false;
	}

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = GREEN;

	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_SELL_TIP, "出售提示", true, "<image val=16>Common1/Common1_image_鼠标左键</image>左键点击进行出售", usFont, usFontSize, iFontColor, 0, 0));
	return true;
}

/*
bool LC_ItemTooltipBase::_addTooltipFieldPackDataFormatDescript(TooltipFieldVector& kFieldVector)
{
	CF_ItemList* pkItemList = CF_ItemList::GetSingletonPtr();
	CF_ItemList::DataEntry* pkItemData = pkItemList->GetEntryPtr( GetItemID() );
	if (0 == pkItemData)
	{
		return false;
	}

	LC_BackPackEntry* pkEntry = GetBackPackEntry();

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = GOLD;


	CF_ItemDataFormatFunc* pkCSVItemDataFormat = CF_ItemDataFormatFunc::GetSingletonPtr();
	CF_ItemDataFormatFunc::DataEntry* pkFormatData = pkCSVItemDataFormat->GetEntryPtr(pkItemData->_lItemDataFormatFuncID);

	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_ATMOS_DESCRIPT, "数据格式", true, "数据格式", usFont, usFontSize, iFontColor, 0, 0));

	if (NULL == pkFormatData)
	{
		iFontColor = RED;
		kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_ATMOS_DESCRIPT, "数据格式", true, "无法识别数据格式", usFont, usFontSize, iFontColor, 0, 0));
	}
	else
	{
		iFontColor = GREEN;

		//if(pkFormatData->_bSpawnTimeFunc)
			kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_ATMOS_DESCRIPT, "数据格式", true, "基础物品功能", usFont, usFontSize, iFontColor, 0, 0));
		
		if(pkFormatData->_bUniqueID)
		{
			kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_ATMOS_DESCRIPT, "数据格式", true, "唯一ID功能", usFont, usFontSize, iFontColor, 0, 0));
			
			if(pkEntry)
			{
				StringStreamType kStrStream;
				kStrStream << "唯一ID";
				kStrStream << "   " << pkEntry->GetUniqueID1();
				kStrStream << "   " << pkEntry->GetUniqueID2();
				kStrStream << "   " << pkEntry->GetUniqueID3();
				kStrStream << "   " << pkEntry->GetUniqueID4();


				kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_ATMOS_DESCRIPT, "数据格式", true, kStrStream.str() , usFont, usFontSize, iFontColor, 0, 0));
			}
		}

		if(pkFormatData->_bSpawnTimeFunc)
			kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_ATMOS_DESCRIPT, "数据格式", true, "生存时间功能", usFont, usFontSize, iFontColor, 0, 0));

		if(pkFormatData->_bUsedCountFunc)
			kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_ATMOS_DESCRIPT, "数据格式", true, "多次使用功能", usFont, usFontSize, iFontColor, 0, 0));

		if(pkFormatData->_bEnhanceFunc)
			kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_ATMOS_DESCRIPT, "数据格式", true, "强化功能", usFont, usFontSize, iFontColor, 0, 0));

		if(pkFormatData->_bTextFunc)
			kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_ATMOS_DESCRIPT, "数据格式", true, "刻字功能", usFont, usFontSize, iFontColor, 0, 0));

		if(pkFormatData->_bAppendAttributeFunc)
			kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_ATMOS_DESCRIPT, "数据格式", true, "附魔功能", usFont, usFontSize, iFontColor, 0, 0));

		if(pkFormatData->_bHobbyFunc)
			kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_ATMOS_DESCRIPT, "数据格式", true, "爱好功能", usFont, usFontSize, iFontColor, 0, 0));

		if(pkFormatData->_bEnduranceFunc)
			kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_ATMOS_DESCRIPT, "数据格式", true, "耐久度功能", usFont, usFontSize, iFontColor, 0, 0));

		if(pkFormatData->_bAmuletFunc)
			kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_ATMOS_DESCRIPT, "数据格式", true, "护身符功能", usFont, usFontSize, iFontColor, 0, 0));
	
		if(pkFormatData->_bEnhanceLevelLimit)
			kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_ATMOS_DESCRIPT, "数据格式", true, "强化等级上限功能", usFont, usFontSize, iFontColor, 0, 0));
	}

	return true;
}
*/
