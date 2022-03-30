
#include "ClientPCH.h"
#include "LM_LocaleManager.h"
#include "LC_SkillBookItemTooltip.h"
#include "LC_SkillTooltipBase.h"
#include "LC_SkillAsset.h"
#include "LC_PlayerBase.h"
#include "LC_ClientToyPetManager.h"

#include "CF_ItemList.h"
#include "CF_SkillExecuteList.h"
#include "CF_SecretSkillClient.h"
#include "CF_BuddyUpgrade.h"
#include "CF_BuddyTransform.h"
#include "SK_Define.h"
#include "SC_ClientSkillInterface.h"
#include "SC_ClientScriptInterface.h"
using namespace GameLogic;
using namespace CSVFile;

LC_SkillBookItemTooltip::LC_SkillBookItemTooltip(void)
{
}

LC_SkillBookItemTooltip::~LC_SkillBookItemTooltip(void)
{
}

bool LC_SkillBookItemTooltip::BuildTooltip()
{
	CF_ItemList::DataEntry* pkItemData = GetItemInfoEntry();
	if (NULL == pkItemData)
	{
		return false;
	}

	//清除所有的tooltip字段
	TooltipFieldVector& kFieldVector = GetTooltipFields();
	kFieldVector.clear();

	long lCategory2 = pkItemData->_lCategory2;

	//物品名称
	_addTooltipFieldName(kFieldVector);

	//星级
	if(ITEM_CATEGORY_CONSUME_PETTOY == lCategory2)
		_addTooltipFieldStarInfo(kFieldVector);

	//子类型
	if(ITEM_CATEGORY_CONSUME_TRANSFORM == lCategory2
		|| ITEM_CATEGORY_CONSUME_CHARACTER == lCategory2
		|| ITEM_CATEGORY_CONSUME_PETTOY == lCategory2
		|| ITEM_CATEGORY_CONSUME_BUDDYTRANSFORM == lCategory2
		|| ITEM_CATEGORY_BUDDYSKIN_PIECES == lCategory2
		|| ITEM_CATEGORY_CIMELIASKIN_PIECES == lCategory2
		|| ITEM_CATEGORY_DEBRIS_BUDDY == lCategory2)
		_addTooltipFieldAvatar(kFieldVector);
	else
		_addTooltipFieldSubCategory(kFieldVector);

	//技能学习
	//_addTooltipFieldLearned(kFieldVector);
	if(ITEM_CATEGORY_CONSUME_MENTAL != lCategory2)
	{
		//详细描述
		_addTooltipDetailDescript(kFieldVector);
	}
	
	

	//修为技能书不显示战斗力
	bool bShowBattle = true;
	SC_ClientScriptInterface* pkClientInterface = SC_ClientScriptInterface::GetSingletonPtr();
	SC_ClientSkillInterface* pkInterface = pkClientInterface->GetSkillInterface();

	if(pkInterface->GetSecretTypeByBookID(pkItemData->_iID)  == 3)
		bShowBattle =false;

	if (bShowBattle)
	{
		//技能战斗力
		_addTooltipScore(kFieldVector);
	}

	//技能属性
	_addTooltipAttrib(kFieldVector);

	//天赋描述
	bool bCharacter = ITEM_CATEGORY_CONSUME_CHARACTER == lCategory2
		|| ITEM_CATEGORY_DEBRIS_BUDDY == lCategory2;
	bool bToy = ITEM_CATEGORY_CONSUME_PETTOY == lCategory2;
	bool bTransform = ITEM_CATEGORY_CONSUME_TRANSFORM == lCategory2
		|| ITEM_CATEGORY_CIMELIASKIN_PIECES == lCategory2;
	bool bBuddyTransform = ITEM_CATEGORY_CONSUME_BUDDYTRANSFORM == lCategory2 
		|| ITEM_CATEGORY_BUDDYSKIN_PIECES == lCategory2;
	if(bCharacter || bTransform)
		_addTooltipFieldTalentInfo(kFieldVector);

	//技能描述
	if(bCharacter || bToy)
		_addTooltipFieldSkillInfo(kFieldVector);

	if(bTransform || bBuddyTransform || bCharacter || bToy
		|| ITEM_CATEGORY_CONSUME_MENTAL == lCategory2)
	{
		//技能描述
		_addTooltipSkillDes(kFieldVector);
	}

	//限时
	if (ITEM_CATEGORY_CONSUME_BUDDYTRANSFORM == lCategory2)
		_addTooltipTimeLimit(kFieldVector);

	//物品描述
	_addTooltipFieldDescript(kFieldVector);
	//视频
	//_addTooltipVideo(kFieldVector);

	//技能学习限制相关字段
	//等级限制
	//_addTooltipFieldCimeliaLimit(kFieldVector);

	//绑定性质
	//_addTooltipFieldBind(kFieldVector);

	//价格
	//_addTooltipFieldPrice(kFieldVector);
	//公会贡献
	//_addTooltipFieldGuildCash(kFieldVector);

	//时间描述
	//_addTooltipFieldTimeLimitDescript(kFieldVector);

	//if(!m_bShopOpen)
	//{
	//	//拆分提示
	//	_addTooltipSplitInfo(kFieldVector);
	//}
	//else
	//{
	//	//出售提示
	//	_addTooltipSellTip(kFieldVector);
	//}

	////发送道具信息

	//_addTooltipFieldAddItemLinkInfo(kFieldVector);
	//格式化tooltip
	_formatTooltip(kFieldVector);
	
	return true;
}

bool LC_SkillBookItemTooltip::_addTooltipTimeLimit(TooltipFieldVector& kFieldVector)
{
	CF_ItemList::DataEntry* pkItemData = GetItemInfoEntry();
	if (NULL == pkItemData)
	{
		return false;
	}
	StringStreamType sStream;
	CF_ItemPropertyFunc::DataEntry *entry = SafeGetCSVFileDataEntryPtr<CF_ItemPropertyFunc>(pkItemData->_lPropertyFuncID);
	if(entry == NULL)
		return false;

	CF_BuddyTransform::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_BuddyTransform>(atoi(entry->_sScriptArgs.c_str()));
	if(pkEntry == NULL)
		return false;

	int itime = pkEntry->_iTime;
	if (itime < 0)
	{
		return false;
	}

	if (itime <= 24)
		sStream << itime << GET_MSG(LC_HELPER_CPP_010);
	else
	{
		sStream << UT_MathBase::Floor(itime / 24.0f) << GET_MSG(LC_HELPER_CPP_009);
		int remain = itime % 24;
		if (remain > 0)
			sStream << remain << GET_MSG(LC_HELPER_CPP_010);
	}

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = RED;
	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_EXPIRE_TIME, "Time", true, sStream.str(), usFont, usFontSize, iFontColor, 0, 0));
	return true;
}

bool LC_SkillBookItemTooltip::_addTooltipFieldAvatar(TooltipFieldVector& kFieldVector)
{
	CF_ItemList::DataEntry* pkItemData = GetItemInfoEntry();
	if (NULL == pkItemData)
	{
		return false;
	}

	StringStreamType kStringStream("");
	switch(pkItemData->_lCategory2)
	{
	case ITEM_CATEGORY_CONSUME_TRANSFORM:
		{
			if(pkItemData->_lProductID == 0)
				return false;

			CF_SubordinateTransform::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_SubordinateTransform>(pkItemData->_lProductID);
			if(pkData == NULL)
				return false;

			kStringStream << pkData->_iType << '#' << pkData->_iCharType;
		}
		break;
	
	case ITEM_CATEGORY_CONSUME_CHARACTER:
		{
			if(pkItemData->_lProductID == 0)
				return false;

			kStringStream << "-2#" << pkItemData->_lProductID;
		}
		break;
	case ITEM_CATEGORY_CONSUME_PETTOY:
		{
			if(pkItemData->_lProductID == 0)
				return false;

			const CF_Pet::DataEntry *pkPet = SafeGetCSVFileDataEntryPtr<CF_Pet>(pkItemData->_lProductID);
			if(pkPet == NULL)
				return false;

			kStringStream << "-1#" << pkPet->_charType;
		}
		break;
	case ITEM_CATEGORY_CONSUME_BUDDYTRANSFORM:
		{
			if(pkItemData->_lProductID == 0)
				return false;

			kStringStream << "-3#" << pkItemData->_lProductID;
		}
		break;
	case ITEM_CATEGORY_BUDDYSKIN_PIECES:
		{
			kStringStream << "-3#" << pkItemData->_iTransformID;
		}
		break;
	case ITEM_CATEGORY_CIMELIASKIN_PIECES:
		{
			CF_SubordinateTransform::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_SubordinateTransform>(pkItemData->_iTransformID);
			if(pkData == NULL)
				return false;

			kStringStream << pkData->_iType << '#' << pkData->_iCharType;
		}
		break;
	case ITEM_CATEGORY_DEBRIS_BUDDY:
		{
			kStringStream << "-2#" << pkItemData->_iTransformID;
		}
		break;
	default:
		break;
	}

	
	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = WHITE;
	
	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_AVATAR, "Avatar", true, kStringStream.str(), usFont, usFontSize, 0, 0, 0));
	return true;
}

bool LC_SkillBookItemTooltip::_addTooltipFieldTalentInfo(TooltipFieldVector& kFieldVector)
{
	CF_ItemList::DataEntry* pkItemData = GetItemInfoEntry();
	if (NULL == pkItemData)
	{
		return false;
	}

	StringStreamType sStream;
	switch(pkItemData->_lCategory2)
	{
	case ITEM_CATEGORY_CONSUME_TRANSFORM:
		{
			if(pkItemData->_lProductID == 0)
				return false;

			CF_SubordinateTransform::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_SubordinateTransform>(pkItemData->_lProductID);
			if(pkData == NULL)
				return false;

			if(pkData->_iSkill == 0)
				return false;
			
			sStream << pkData->_iSkill;
		}
		break;
	case ITEM_CATEGORY_CONSUME_CHARACTER:
	case ITEM_CATEGORY_DEBRIS_BUDDY:
		{
			int iId = pkItemData->_lCategory2 == ITEM_CATEGORY_CONSUME_CHARACTER ? pkItemData->_lProductID : pkItemData->_iTransformID;
			if(iId == 0)
				return false;

			CF_BuddyUpgrade::DataEntryMapExternIterator kIter = CF_BuddyUpgrade::GetSingletonPtr()->GetIterator();
			while(kIter.HasMoreElements())
			{
				const CF_BuddyUpgrade::DataEntry* pkData = kIter.PeekNextValuePtr();
				if(pkData->_cID == iId)
				{
					if(pkData->_unlockskill > 0)
					{
						sStream << pkData->_unlockskill;
						break;
					}
				}

				kIter.MoveNext();
			}
		}
		break;
	default:
		return false;
	}

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;

	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_SKILLBOOK_TALENTINFO, "TalentInfo", true, sStream.str(), usFont, usFontSize, WHITE, 0, 0));
	return true;
}

bool LC_SkillBookItemTooltip::_addTooltipFieldSkillInfo(TooltipFieldVector& kFieldVector)
{
	CF_ItemList::DataEntry* pkItemData = GetItemInfoEntry();
	if (NULL == pkItemData)
	{
		return false;
	}
	
	StringStreamType sStream;
	switch(pkItemData->_lCategory2)
	{
	case ITEM_CATEGORY_CONSUME_CHARACTER:
	case ITEM_CATEGORY_DEBRIS_BUDDY:
		{
			int iId = pkItemData->_lCategory2 == ITEM_CATEGORY_CONSUME_CHARACTER ? pkItemData->_lProductID : pkItemData->_iTransformID;
			if(iId == 0)
				return false;

			const CF_Buddy::DataEntry *pkBuddy = SafeGetCSVFileDataEntryPtr<CF_Buddy>(iId);
			if(pkBuddy == NULL)
				return false;

			TStringVector kVec = UT_BaseStringUtility::SplitString(pkBuddy->_skillid, ';');
			if(kVec.size() == 0)
				return false;

			sStream << "-2#";
			for(int i=0; i<kVec.size(); ++i)
			{
				TStringVector kVecDetail = UT_BaseStringUtility::SplitString(kVec[i], ':');
				T_ASSERT_CRITICAL_S(kVecDetail.size() > 0);
				
				sStream << String2Int(kVecDetail[0]) << "#";
			}
		}
		break;
	case ITEM_CATEGORY_CONSUME_PETTOY:
		{
			if(pkItemData->_lProductID == 0)
				return false;

			const CF_Pet::DataEntry *pkPet = SafeGetCSVFileDataEntryPtr<CF_Pet>(pkItemData->_lProductID);
			if(pkPet == NULL)
				return false;

			TStringVector kVec = UT_BaseStringUtility::SplitString(pkPet->_skillid, ':');
			if(kVec.size() == 0)
				return false;

			StringType sResult;
			LC_TooltipBase::_getUseResult<CF_SkillSpecialAttrib>(String2Int(kVec[0]), sResult);
			sStream << sResult;
		}
		break;
	default:
		break;
	}


	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;

	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_SKILLBOOK_SKILLINFO, "SkillInfo", true, sStream.str(), usFont, usFontSize, WHITE, 0, 0));
	return true;
}

bool LC_SkillBookItemTooltip::_addTooltipFieldStarInfo(TooltipFieldVector& kFieldVector)
{
	CF_ItemList::DataEntry* pkItemData = GetItemInfoEntry();
	if (NULL == pkItemData)
		return false;

	if(pkItemData->_lProductID == 0)
		return false;

	const CF_Pet::DataEntry *pkPet = SafeGetCSVFileDataEntryPtr<CF_Pet>(pkItemData->_lProductID);
	if(pkPet == NULL)
		return false;

	StringStreamType sStream;
	sStream << (int)pkPet->_InitLevel << "#" << (int)pkPet->_MaxLevel;

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;	

	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_SKILLBOOK_STARINFO, "StarInfo", true, sStream.str(), usFont, usFontSize, WHITE, 0, 0));
	return true;
}

bool LC_SkillBookItemTooltip::_addTooltipFieldLearned( TooltipFieldVector& kFieldVector )
{
	LC_ActorBase* pkObserver = GetObserver();
	if (NULL == pkObserver)
	{
		return false;
	}

	if(!IS_PLAYER_ID(pkObserver->GetID()))
		return false;
	
	CF_ItemList::DataEntry* pkItemData = GetItemInfoEntry();
	if (NULL == pkItemData)
	{
		return false;
	}

	LC_SkillAsset* pkSkillAsset = ((LC_PlayerBase*)pkObserver)->GetSkillAsset();
	if(NULL == pkSkillAsset)
	{
		return false;
	}

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = WHITE;

	if (pkItemData->_lUseTriggerSkillTypeID <= 0)
	{
		return false;
	}

	CF_SkillExecuteList::DataEntry* pkSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(pkItemData->_lUseTriggerSkillTypeID);
	if (0 == pkSkillData)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, Utility::LOG_SWITCH_ERROR, "Can't find %d in SkillExecute!", pkItemData->_lUseTriggerSkillTypeID);
		return false;
	}
	
	if(pkItemData->_lUseTriggerSkillTypeID == SKILL_LEANR_SKILL)
	{
		//通过技能书最终学会的技能
		int lSkillID = pkItemData->_lUseTriggerSkillPara1;
		//如果技能已经学会
		if ( pkSkillAsset->IsLearnedSkill(lSkillID) )
		{
			StringStreamType kStringStream;

			iFontColor = RED;
			kStringStream << GET_MSG(LC_SKILLBOOKITEMTOOLTIP_CPP_001);
			kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_SKILL_BOOK_LEARNED, GET_MSG(LC_SKILLBOOKITEMTOOLTIP_CPP_002), true, kStringStream.str(), usFont, usFontSize, iFontColor, 0, 0));
		}
		/*else
		{
		CF_SkillExecuteList::DataEntry* pkLearnSkillData = pkSkillList->GetEntryPtr(lSkillID);
		if (0 == pkLearnSkillData)
		{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "Can't find %d in SkillExecute!", lSkillID);
		return false;
		}

		kStringStream << "学习目标：" << pkLearnSkillData->_kName;
		}*/
	}
	
	//if(pkItemData->_lUseTriggerSkillTypeID == SKILL_LEANR_PRODUCTION_FORMULA)
	//{
	//	LC_ProductionAsset* pkProductionAsset = ((LC_PlayerBase*)pkObserver)->GetProductionAsset();
	//	//通过物品最终学会的配方
	//	int lProductionID = pkItemData->_lUseTriggerSkillPara1;
	//	//如果配方已经学会
	//	if ( pkProductionAsset->IsLearnedProductionFormula(lProductionID) )
	//	{
	//		StringStreamType kStringStream;

	//		iFontColor = RED;
	//		kStringStream << "已学会";
	//		kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_SKILL_BOOK_LEARNED, "技能学习", true, kStringStream.str(), usFont, usFontSize, iFontColor, 0, 0));
	//	}
	//	/*else
	//	{
	//	CF_SkillExecuteList::DataEntry* pkLearnSkillData = pkSkillList->GetEntryPtr(lSkillID);
	//	if (0 == pkLearnSkillData)
	//	{
	//	GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "Can't find %d in SkillExecute!", lSkillID);
	//	return false;
	//	}

	//	kStringStream << "学习目标：" << pkLearnSkillData->_kName;
	//	}*/
	//}
	

	return true;
}

bool LC_SkillBookItemTooltip::_addTooltipFieldUseResult( TooltipFieldVector& kFieldVector )
{
	CF_ItemList::DataEntry* pkItemData = GetItemInfoEntry();
	if (NULL == pkItemData)
	{
		return false;
	}

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = GREEN;

	if (pkItemData->_lUseTriggerSkillTypeID <= 0)
	{
		return false;
	}

	CF_SkillExecuteList::DataEntry* pkSkillBookData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(pkItemData->_lUseTriggerSkillTypeID);
	if (0 == pkSkillBookData)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, Utility::LOG_SWITCH_ERROR, "Can't find %d in SkillExecuteList!", pkItemData->_lUseTriggerSkillTypeID);
		return false;
	}

	//通过技能书学会的技能
	int lLearnSkillID = pkItemData->_lUseTriggerSkillPara1;
	CF_SkillExecuteList::DataEntry* pkSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(lLearnSkillID);
	if (0 == pkSkillData)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, Utility::LOG_SWITCH_ERROR, "Can't find %d in SkillExecuteList!", lLearnSkillID);
		return false;
	}

	StringStreamType kStringStream;
	kStringStream << GET_MSG(LC_SKILLBOOKITEMTOOLTIP_CPP_003) << pkSkillData->_kName;
	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_SKILL_BOOK_LEARNED, GET_MSG(LC_SKILLBOOKITEMTOOLTIP_CPP_002), true, kStringStream.str(), usFont, usFontSize, iFontColor, 0, 0));

	return true;
}

bool LC_SkillBookItemTooltip::_addTooltipFieldMaterialRequied( TooltipFieldVector& kFieldVector )
{
	CF_ItemList::DataEntry* pkItemData = GetItemInfoEntry();
	if(NULL == pkItemData)
	{
		return false;
	}

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = WHITE;

	////只有生产技能才有物品需求，学习技能没有物品需求
	//StringType kMaterialRequired = pkItemData->_kMaterialRequired;
	//if ("" == kMaterialRequired)
	//{
	//	return false;
	//}

	//StringStreamType kStringStream;
	//kStringStream << "需求材料:" << pkItemData->_kMaterialRequired;


	int lTriggerSkill = pkItemData->_lUseTriggerSkillTypeID;
	if (lTriggerSkill <= 0)
	{
		return false;
	}
	CF_SkillExecuteList::DataEntry* pkSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(lTriggerSkill);
	if (NULL == pkSkillData)
	{
		return false;
	}

	StringStreamType kStringStream;
	kStringStream.str("");
	Utility::UT_Vec3Int kItemCost[1];
	/*kItemCost[0] = pkCostData->_kItemCost1;
	kItemCost[1] = pkCostData->_kItemCost2;
	kItemCost[2] = pkCostData->_kItemCost3;*/
	kItemCost[0] = pkSkillData->_kAdditionItemCost;
	for (int i = 0; i < 1; i ++)
	{
		int iItemID = kItemCost[i].x;
		int iItemCount = kItemCost[i].y;
		if (iItemID > 0)
		{
			CF_ItemList::DataEntry* pkItemData = SafeGetCSVFileDataEntryPtr<CF_ItemList>(iItemID);
			if (0 != pkItemData)
			{
				kStringStream << GET_MSG(LC_FORMULAITEMTOOLTIP_CPP_005) << pkItemData->_kName;
				if (iItemCount > 0)
				{
					kStringStream << "(" << iItemCount << ")" << ",";
				}
			}
		}
	}
	StringType kString = kStringStream.str();
	if (kString == "")
	{
		return false;
	}
	//如果字符串以逗号结尾，剪除末尾的逗号
	if (kString.substr(kString.length() - 1, 1) == ",")
	{
		kString = kString.substr(0, kString.length() - 1);
	}

	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_SKILL_BOOK_MATERIAL_REQUIRED, GET_MSG(LC_SKILLBOOKITEMTOOLTIP_CPP_004), true, kString, usFont, usFontSize, iFontColor, 0, 0));
	return true;
}

bool LC_SkillBookItemTooltip::_addTooltipFieldCimeliaLimit(TooltipFieldVector &kFieldVector)
{
	LC_ActorBase *pkObserver = GetObserver();
	if (NULL == pkObserver)
	{
		return false;
	}

	LC_PlayerBase *pkPlayer = static_cast<LC_PlayerBase*>(pkObserver);
	//if(NULL == pkPlayer)
	//	return false;

	CF_ItemList::DataEntry *pkEtnry = GetItemInfoEntry();
	if(NULL == pkEtnry)
		return false;

	CF_ItemUseLimitFunc::DataEntry *pkItemLimit = SafeGetCSVFileDataEntryPtr<CF_ItemUseLimitFunc>(pkEtnry->_lUseLimitFuncID);
	if(NULL == pkItemLimit)
		return false;

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = 0xffcc9966;
	StringStreamType kStringStream("");


	LC_SubordinateAsset pkSubordinateAsset = pkPlayer->GetSubordinateAsset();

	Utility::UT_SIMDataList kSubLevels;
	Utility::ParseFmtString2DataIFs(pkItemLimit->_kSubLevels, kSubLevels);
	if(kSubLevels.size() > 0)
	{
		Utility::UT_SIMDataList::iterator it = kSubLevels.begin();
		for (; it!=kSubLevels.end(); ++it)
		{
			LC_SubUpgradeData *pkSubordinateData = pkSubordinateAsset.GetSubData(it->ID());
			if(NULL != pkSubordinateData && pkSubordinateData->GetStage() >= it->ParamA())
			{
				kStringStream <<"<color val=0xffcc9966>";
			}
			else
			{
				kStringStream <<"<color val=0xffcd1d06>";
			}
			
			if(it->ID() < LC_SUB_TP_END)
				kStringStream << SUB_TP_NAME(it->ID()) << GET_MSG(LC_SKILLBOOKITEMTOOLTIP_CPP_005) << it->ParamA() << " </color>";
		}
	}

	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_LEVEL_LIMIT, GET_MSG(LC_ITEMTOOLTIPBASE_H_002), true, kStringStream.str(), usFont, usFontSize, iFontColor, 0, 0));
	return true;
}

bool LC_SkillBookItemTooltip::_addTooltipVideo(TooltipFieldVector& kFieldVector)
{
	CF_ItemList::DataEntry *pkEntry = GetItemInfoEntry();
	CF_ItemPropertyFunc::DataEntry *pkProperty = SafeGetCSVFileDataEntryPtr<CF_ItemPropertyFunc>(pkEntry->_lPropertyFuncID);
	if(NULL == pkProperty)
		return false;

	TStringVector kVect = UT_BaseStringUtility::SplitString(pkProperty->_kSkills, ';');
	if(kVect.size() == 0)
		return false;

	int iSkillID = atoi(kVect[0].c_str());
	CF_SkillExecuteList::DataEntry *pkSkillInfo = LC_Helper::SafeGetSkillExecuteDataEntryPtr(iSkillID);
	if(NULL == pkSkillInfo)
		return false;

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = WHITE;
	if (pkSkillInfo->_kVideo != "NULL")
	{
		kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_VIDEO, GET_MSG(LC_SKILLBOOKITEMTOOLTIP_CPP_006), true, pkSkillInfo->_kVideo, usFont, usFontSize, iFontColor, 0, 0));
	}
	return true;
}

bool LC_SkillBookItemTooltip::_addTooltipScore(TooltipFieldVector& kFieldVector)
{
	CF_ItemList::DataEntry* pkItemData = GetItemInfoEntry();
	if(NULL == pkItemData)
	{
		return false;
	}
	if(ITEM_CATEGORY_CONSUME_MENTAL != pkItemData->_lCategory2
		&& ITEM_CATEGORY_CONSUME_TRANSFORM != pkItemData->_lCategory2
		&& ITEM_CATEGORY_CONSUME_CHARACTER != pkItemData->_lCategory2
		&& ITEM_CATEGORY_CONSUME_PETTOY != pkItemData->_lCategory2)
	{
		return false;
	}

	combat_score_type iBattleScore = 0;
	switch(pkItemData->_lCategory2 )
	{
	case ITEM_CATEGORY_CONSUME_TRANSFORM:
		{
			CF_SubordinateTransform::DataEntryMapExternIterator kIter = CF_SubordinateTransform::GetSingleton().GetIterator();
			while(kIter.HasMoreElements())
			{
				CF_SubordinateTransform::DataEntry* pkData = kIter.PeekNextValuePtr();
				Utility::UT_SIMDataList::iterator it = pkData->_kItems.begin();
				for (; it!=pkData->_kItems.end(); ++it)
				{
					if(pkItemData->_iID == it->ID())
					{
						iBattleScore = LC_Helper::GetCharAttrCombatScore(pkData->_iAttrFunc);
						break;
					}
				}
				kIter.MoveNext();
			}
		}
		break;
	
	case ITEM_CATEGORY_CONSUME_CHARACTER:
		{
			if(pkItemData->_lProductID == 0)
				return false;

			const CF_Buddy::DataEntry *pkBuddy = SafeGetCSVFileDataEntryPtr<CF_Buddy>(pkItemData->_lProductID);
			if(pkBuddy != NULL)
			{
				iBattleScore += LC_Helper::CalcSingleAttributeCombatScore(ATT_MAX_HP, pkBuddy->_lMaxHP, false);
				iBattleScore += LC_Helper::CalcSingleAttributeCombatScore(ATT_MAX_ATK, pkBuddy->_lMaxPHA, false);
				iBattleScore += LC_Helper::CalcSingleAttributeCombatScore(ATT_DEF, pkBuddy->_lPHD, false);
				iBattleScore += LC_Helper::CalcSingleAttributeCombatScore(ATT_ATK_RATE, pkBuddy->_lATKRateAdd, false);
				iBattleScore += LC_Helper::CalcSingleAttributeCombatScore(ATT_DOG_RATE, pkBuddy->_lDogRATE, false);
				iBattleScore += LC_Helper::CalcSingleAttributeCombatScore(ATT_CIR_RATE, pkBuddy->_lCirRATE, false);
				iBattleScore += LC_Helper::CalcSingleAttributeCombatScore(ATT_CIR_DEF, pkBuddy->_lCirDefAdd, false);
			}
		}
		break;
	case ITEM_CATEGORY_CONSUME_PETTOY:
		{
			if(pkItemData->_lProductID == 0)
				return false;

			const CF_Pet::DataEntry *pkPet = SafeGetCSVFileDataEntryPtr<CF_Pet>(pkItemData->_lProductID);
			if(pkPet == NULL)
				return false;

			const CF_PetUpgrade::DataEntry *pkUpgrade = LC_ClientToyPetManager::GetSingletonPtr()->GetPetUpgradeData(pkItemData->_lProductID, pkPet->_InitLevel);
			if(pkUpgrade == NULL)
				return false;

			mem::vector<raw_attr_value_type> kAttributes;
			kAttributes.resize(ATT_MAX_COUNT);
			LC_Helper::OnPetLevelUp_Attribute(kAttributes, pkPet, pkUpgrade, false);
			for (int i = 0; i < ATT_MAX_COUNT; ++i )
			{
				iBattleScore += LC_Helper::CalcSingleAttributeCombatScore((AttributeType)i, kAttributes[i], false);
			}
		}
		break;
	default:
		{
			CF_SecretSkillClient::DataEntryMapExternIterator kIter = CF_SecretSkillClient::GetSingleton().GetIterator();
			while(kIter.HasMoreElements())
			{
				CF_SecretSkillClient::DataEntry* pkData = kIter.PeekNextValuePtr();
				if (pkData->_SkillBookID == pkItemData->_iID)
				{
					CF_SkillSpecialAttrib::DataEntry *pkSkillInfo = SafeGetCSVFileDataEntryPtr<CF_SkillSpecialAttrib>(pkData->_SkillID);
					if(NULL != pkSkillInfo)
						iBattleScore += pkSkillInfo->_lCombatScore;
					CF_SkillSpecialAttrib::DataEntry *pkStateInfo = SafeGetCSVFileDataEntryPtr<CF_SkillSpecialAttrib>(pkData->_StateID);
					if(NULL != pkStateInfo)
						iBattleScore += pkStateInfo->_lCombatScore;
					break;
				}
				kIter.MoveNext();
			}
		}
		break;
	}
	

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = WHITE;
	if (iBattleScore > 0)
	{
		kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_SKILLBOOK_SCORE, GET_MSG(LC_SKILLBOOKITEMTOOLTIP_CPP_007), true, Int642String((combat_score_type)iBattleScore), usFont, usFontSize, iFontColor, 0, 0));
		return true;
	}
	return false;
}
//构造tooltip的属性
bool LC_SkillBookItemTooltip::_addTooltipAttrib(TooltipFieldVector& kFieldVector)
{
	CF_ItemList::DataEntry* pkItemData = GetItemInfoEntry();
	if(NULL == pkItemData)
	{
		return false;
	}
	
	int iParameter1 =0;
	int iParameter2 =0;
	int iParameter3 =0;
	int iParameter4 =0;
	int iParameter5 =0;
	int iParameter6 =0;
	int iParameter7 =0;
	int iParameter8 =0;
	int iParameter9 =0;
	int iParameter10 =0;
	int iParameter11 =0;
	int iParameter12 =0;
	int iParameter13 =0;
	StringStreamType sStream("");
	bool bHasAttrib = false;

	const int iCategory2 = pkItemData->_lCategory2;
	switch(iCategory2)
	{
	case ITEM_CATEGORY_CONSUME_TRANSFORM:
	case ITEM_CATEGORY_CIMELIASKIN_PIECES:
		{
			if(iCategory2 == ITEM_CATEGORY_CONSUME_TRANSFORM
				&&pkItemData->_lProductID == 0)
				return false;

			int iTransformID = pkItemData->_lProductID;
			if(iCategory2 == ITEM_CATEGORY_CIMELIASKIN_PIECES)
				iTransformID = pkItemData->_iTransformID;

			if(iTransformID == 0)
				return false;

			CF_SubordinateTransform::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_SubordinateTransform>(iTransformID);
			if(pkData == NULL)
				return false;

			CF_CharAttributeFunc::DataEntry *pkAttrFunc  = SafeGetCSVFileDataEntryPtr<CF_CharAttributeFunc>(pkData->_iAttrFunc);
			if(pkAttrFunc)
			{
				iParameter1 = Float2Int(pkAttrFunc->_Atk);
				iParameter2 = Float2Int(pkAttrFunc->_Def);
				iParameter3 = Float2Int(pkAttrFunc->_HP);
				iParameter4 = Float2Int(pkAttrFunc->_ATKRate);
				iParameter5 = Float2Int(pkAttrFunc->_Dogge);
				iParameter6 = Float2Int(pkAttrFunc->_Cir);
				iParameter7 = Float2Int(pkAttrFunc->_CirDef);

				bHasAttrib =true;
			}
		}
		break;
	case ITEM_CATEGORY_CONSUME_BUDDYTRANSFORM:
	case ITEM_CATEGORY_BUDDYSKIN_PIECES:
		{
			if(iCategory2 == ITEM_CATEGORY_CONSUME_BUDDYTRANSFORM 
				&& pkItemData->_lProductID == 0)
				return false;

			int iTransformID = pkItemData->_lProductID;
			if(iCategory2 == ITEM_CATEGORY_BUDDYSKIN_PIECES)
				iTransformID = pkItemData->_iTransformID;

			if(iTransformID == 0)
				return false;

			CF_BuddyTransform::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_BuddyTransform>(iTransformID);
			if(pkData == NULL)
				return false;

			const UT_SIMDataList* attribList = &pkData->_kAttribute;
			if(attribList->empty())
			{
				attribList = &pkData->_kAttributeGlobal;
			}

			Utility::UT_SIMDataList::const_iterator it = attribList->begin();
			for (; it!=attribList->end(); ++it)
			{
				switch(it->ParamB())
				{
				case ATT_MAX_HP:
					{
						_getAttribValue(*it, iParameter3, iParameter11);
						break;
					}
				case ATT_MAX_ATK:
					{
						_getAttribValue(*it, iParameter1, iParameter12);
						break;
					}
				case ATT_DEF:
					{
						_getAttribValue(*it, iParameter2, iParameter13);
						break;
					}
				case ATT_ATK_RATE:
					{
						_getAttribValue(*it, iParameter4, iParameter4);
						break;
					}
				case ATT_DOG_RATE:
					{
						_getAttribValue(*it, iParameter5, iParameter5);
						break;
					}
				case ATT_CIR_RATE:
					{
						_getAttribValue(*it, iParameter6, iParameter6);
						break;
					}
				case ATT_CIR_DEF:
					{
						_getAttribValue(*it, iParameter7, iParameter7);
						break;
					}
				case ATT_CIR_DAM:
					{
						_getAttribValue(*it, iParameter8, iParameter8);
						break;
					}
				case ATT_DMG_RATE:
					{
						_getAttribValue(*it, iParameter9, iParameter9);
						break;
					}
				case ATT_DMG_DERATE:
					{
						_getAttribValue(*it, iParameter10, iParameter10);
						break;
					}
				}
			}
			bHasAttrib =true;
		}
		break;
	
	case ITEM_CATEGORY_CONSUME_CHARACTER:
	case ITEM_CATEGORY_DEBRIS_BUDDY:
		{
			int iId = iCategory2 == ITEM_CATEGORY_CONSUME_CHARACTER ? pkItemData->_lProductID : pkItemData->_iTransformID;
			if(iId == 0)
				return false;

			const CF_Buddy::DataEntry *pkBuddy = SafeGetCSVFileDataEntryPtr<CF_Buddy>(iId);
			if(pkBuddy == NULL)
				return false;

			iParameter1 = pkBuddy->_lMaxPHA;
			iParameter2 = pkBuddy->_lPHD;
			iParameter3 = pkBuddy->_lMaxHP;
			iParameter4 = pkBuddy->_lATKRateAdd;
			iParameter5 = pkBuddy->_lDogRATE;
			iParameter6 = pkBuddy->_lCirRATE;
			iParameter7 = pkBuddy->_lCirDefAdd;

			bHasAttrib = true;
		}
		break;
	case ITEM_CATEGORY_CONSUME_PETTOY:
		{
			if(pkItemData->_lProductID == 0)
				return false;

			const CF_Pet::DataEntry *pkPet = SafeGetCSVFileDataEntryPtr<CF_Pet>(pkItemData->_lProductID);
			if(pkPet == NULL)
				return false;

			const CF_PetUpgrade::DataEntry *pkUpgrade = LC_ClientToyPetManager::GetSingletonPtr()->GetPetUpgradeData(pkItemData->_lProductID, pkPet->_InitLevel);
			if(pkUpgrade == NULL)
				return false;

			mem::vector<raw_attr_value_type> kAttributes;
			kAttributes.resize(ATT_MAX_COUNT);
			LC_Helper::OnPetLevelUp_Attribute(kAttributes, pkPet, pkUpgrade, false);

			iParameter1 = kAttributes[ATT_MAX_ATK];
			iParameter2 = kAttributes[ATT_DEF];
			iParameter3 = kAttributes[ATT_MAX_HP];
			iParameter4 = kAttributes[ATT_ATK_RATE];
			iParameter5 = kAttributes[ATT_DOG_RATE];
			iParameter6 = kAttributes[ATT_CIR_RATE];
			iParameter7 = kAttributes[ATT_CIR_DEF];

			bHasAttrib = true;
		}
		break;
	default:
		{
			CF_SecretSkillClient::DataEntryMapExternIterator kIter = CF_SecretSkillClient::GetSingleton().GetIterator();
			while(kIter.HasMoreElements())
			{
				CF_SecretSkillClient::DataEntry* pkData = kIter.PeekNextValuePtr();
				if (pkData->_SkillBookID == pkItemData->_iID)
				{
					CF_SkillSpecialAttrib::DataEntry *pkStateInfo = SafeGetCSVFileDataEntryPtr<CF_SkillSpecialAttrib>(pkData->_StateID);
					if(NULL != pkStateInfo)
					{
						iParameter1 += pkStateInfo->_lSkillConstParameter1;
						iParameter2 += pkStateInfo->_lSkillConstParameter2;
						iParameter3 += pkStateInfo->_lSkillConstParameter3;
						iParameter4 += pkStateInfo->_lSkillConstParameter4;
						iParameter5 += pkStateInfo->_lSkillConstParameter5;
						iParameter6 += pkStateInfo->_lSkillConstParameter6;
						iParameter7 += pkStateInfo->_lSkillConstParameter7;
						iParameter8 += pkStateInfo->_lSkillConstParameter7;
						bHasAttrib =true;
					}
					break;
				}
				kIter.MoveNext();
			}
		}
		break;
	}
	
	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = WHITE;

	sStream <<"#"<< iParameter1 << "#" << iParameter2 << "#";
	sStream << iParameter3 << "#" << iParameter4 << "#";
	sStream << iParameter5 << "#" << iParameter6 << "#";
	sStream << iParameter7 << "#" << iParameter8 << "#";
	sStream << iParameter9 << "#" << iParameter10 << "#";
	sStream << iParameter11 << "#" << iParameter12 << "#";
	sStream << iParameter13 << "#";
	if (bHasAttrib)
	{
		kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_SKILLBOOK_ATTRIB, GET_MSG(LC_SKILLBOOKITEMTOOLTIP_CPP_008), true, sStream.str(), usFont, usFontSize, iFontColor, 0, 0));
		return true;
	}
	return false;
}

//构造tooltip描述
bool LC_SkillBookItemTooltip::_addTooltipSkillDes(TooltipFieldVector& kFieldVector)
{
	CF_ItemList::DataEntry* pkItemData = GetItemInfoEntry();
	if (NULL == pkItemData)
	{
		return false;
	}
	
	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = 0xffbf4f0a;

	StringType &kContent = pkItemData->_kDetailDescript;
	if (kContent == "" || kContent == "NULL")
	{
		return false;
	}

	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_TRANSFORMCARD_DES, GET_MSG(LC_GOODTOOLTIP_CPP_003), true, kContent, usFont, usFontSize, iFontColor, 0, 0));
	return true;
}

bool LC_SkillBookItemTooltip::_addTooltipDescript(TooltipFieldVector& kFieldVector)
{
	CF_ItemList::DataEntry* pkItemData = GetItemInfoEntry();
	if (NULL == pkItemData)
	{
		return false;
	}

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = 0xffbf4f0a;

	StringType &kContent = pkItemData->_kDescript;
	if (kContent == "" || kContent == "NULL")
	{
		return false;
	}

	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_TRANSFORMCARD_DES, GET_MSG(LC_GOODTOOLTIP_CPP_003), true, kContent, usFont, usFontSize, iFontColor, 0, 0));

	return true;
}

void LC_SkillBookItemTooltip::_getAttribValue(const UT_SIMDataInfo& data, int& iParam, int& iParamPercent)
{
	if (data.ParamA() == 1 || data.ParamA() == 3)
		iParam = data.ParamC();
	else if (data.ParamA() == 2 || data.ParamA() == 4)
		iParamPercent = data.ParamC();
	else
		T_ASSERT_CRITICAL_S(false);
}