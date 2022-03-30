
#include "ClientPCH.h"
#include "LM_LocaleManager.h"
#include "LC_SkillTooltipBase.h"
#include "LC_PlayerBase.h"

#include "CF_ItemList.h"
#include "CF_SkillLevelUp.h"
#include "CF_SkillCDModifyFunc.h"
#include "CF_WorldMapList.h"
#include "CF_OpenLevel.h"

#include "LC_Helper.h"

using namespace GameLogic;
using namespace CSVFile;
using namespace std;
using namespace Skill;

LC_SkillTooltipBase::LC_SkillTooltipBase(void)
: m_lSkillID(0)
{
}

LC_SkillTooltipBase::~LC_SkillTooltipBase(void)
{
}

void LC_SkillTooltipBase::Init( int lSkillID, LC_PlayerBase* pkObserver )
{
	m_lSkillID = lSkillID;
	SetObserver(pkObserver);
}

bool LC_SkillTooltipBase::BuildTooltip()
{
	return BuildTooltip(false);
}

bool LC_SkillTooltipBase::BuildTooltip(bool bCurrentLevelOnly)
{
	CF_SkillExecuteList::DataEntry* pkSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(m_lSkillID);
	if (pkSkillData == 0)
	{
		return false;
	}

	TooltipFieldVector& kFieldVector = GetTooltipFields();
	kFieldVector.clear(); 

	//技能名称
	_addTooltipFieldName(kFieldVector);

	////技能副标题
	//_addTooltipFieldDescript(kFieldVector);

	////技能当前等级
	//_addTooltipFieldCurLevel(kFieldVector);
	
	//技能主被动类型
	_addTooltipFieldPosNegType(kFieldVector);

	//技能需要达到的人物等级
	//_addTooltipFieldPlayerLevel(kFieldVector);
		

	//技能等级
	_addTooltipFieldLevel(kFieldVector);

	//技能消耗
	_addTooltipFieldConsume(kFieldVector);

	//施放距离
	//_addTooltipFieldExecuteDistance(kFieldVector);
	
	//施放时间
	//_addTooltipFieldExecuteTime(kFieldVector);

	//技能类型
	//_addTooltipFieldCategory(kFieldVector);

	//冷却时间
	_addTooltipFieldCDTime(kFieldVector);

	//if (bUseGuildSkillDecl)
	//{
	//	//门派技能研究需要说明
	//	_addGuildSkillToolTipResearchDecl(kFieldVector);
	//}
	//技能需求
	//_addTooltipFieldMaterialRequired(kFieldVector);

	//使用效果
	_addTooltipFieldUseResult(kFieldVector);
	
	//解锁条件
	_addTooltipFieldUnlockContent(kFieldVector);

	//用法介绍
	//_addTooltipFieldUseDescript(kFieldVector);

	//氛围介绍
	//_addTooltipFieldAtmosDescript(kFieldVector);
	//if (bUseGuildSkillDecl)
	//{
	//	//门派技能研究状态
	//	_addGuildSkillToolTipResearchState(kFieldVector);
	//}
	//if (!bCurrentLevelOnly)
	//{
	//	//下一重技能
	//	_addTooltipFieldNextLevel(kFieldVector);

	//}

	//格式化tooltip
	_formatTooltip(kFieldVector);

	return true;
}

bool LC_SkillTooltipBase::AddTooltipFieldExecuteDistance(TooltipFieldVector& kFieldVector)
{
	return _addTooltipFieldExecuteDistance(kFieldVector);
}

bool LC_SkillTooltipBase::AddTooltipFieldExecuteTime(TooltipFieldVector& kFieldVector)
{
	return _addTooltipFieldExecuteTime(kFieldVector);
}

bool LC_SkillTooltipBase::AddTooltipFieldCDTime(TooltipFieldVector& kFieldVector)
{
	return _addTooltipFieldCDTime(kFieldVector);
}

bool LC_SkillTooltipBase::AddTooltipFieldUseResult(TooltipFieldVector& kFieldVector)
{
	return _addTooltipFieldUseResult(kFieldVector);
}

//bool LC_SkillTooltipBase::AddTooltipFieldAtmosDescript(TooltipFieldVector& kFieldVector)
//{
//	return _addTooltipFieldAtmosDescript(kFieldVector);
//}

bool LC_SkillTooltipBase::_addTooltipFieldName( TooltipFieldVector& kFieldVector )
{
	CF_SkillExecuteList::DataEntry* pkSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(m_lSkillID);
	if (NULL == pkSkillData)
	{
		return false;
	}

	unsigned short usFont = FONT_LARGE;
	unsigned short usFontSize = FONTSIZE_LARGE;
	int iFontColor = WHITE;

	StringType kContent = pkSkillData->_kName;
#ifndef _SHIPPING
	kContent += " " + Int2String(m_lSkillID);
#endif
	kFieldVector.push_back( LC_TooltipField(TOOLTIP_FIELD_SKILL_NAME, GET_MSG(LC_SKILLDETAIL_CPP_001), true, kContent, usFont, usFontSize, iFontColor, 0, 0, true));

	return true;
}

bool LC_SkillTooltipBase::_addTooltipFieldPosNegType(TooltipFieldVector &kFieldVector)
{
	CF_SkillExecuteList::DataEntry* pkSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(m_lSkillID);
	if (NULL == pkSkillData)
	{
		return false;
	}

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = WHITE;

	int lPosNeg = pkSkillData->_lPosNegType;
	if(lPosNeg > 0)
	{
		StringType kContent;
		kContent += ((SPNT_POSITIVE == lPosNeg) ? GET_MSG(LC_TOOLTIPMANAGER_CPP_001) : GET_MSG(LC_TOOLTIPMANAGER_CPP_002));
		kFieldVector.push_back( LC_TooltipField(TOOLTIP_FIELD_SKILL_POSNEG_TYPE, GET_MSG(LC_SKILLTOOLTIPBASE_CPP_001), true, kContent, usFont, usFontSize, iFontColor, 0, 0));
		return true;
	}	

	return false;
}

bool LC_SkillTooltipBase::_addTooltipFieldLevel(TooltipFieldVector& kFieldVector)

{
	CF_SkillSpecialAttrib::DataEntry* pkSkillAttrib = SafeGetCSVFileDataEntryPtr<CF_SkillSpecialAttrib>( m_lSkillID );
	if (NULL == pkSkillAttrib)
	{
		return false;
	}

	CF_SkillExecuteList::DataEntry* pkSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(m_lSkillID);
	if(NULL == pkSkillData)
	{
		return false;
	}

	if(pkSkillData->_lCategory1 == SKILL_TYPE_SUB_ACTOR_GODTHING_ACTIVATE)
	{
		return false;
	}

	//基础技能不显示技能重数
	if(pkSkillData->_lCategory2 == SKILL_TYPE_BASE
		|| pkSkillData->_lCategory2 == SKILL_TYPE_JUEXUE_BEIDONG)
	{
		return false;
	}

	LC_PlayerBase *pkPlayer = ((LC_PlayerBase*)GetObserver());
	if(NULL == pkPlayer)
		return false;

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = WHITE;	

	//bool bLearned = pkPlayer->GetSkillAsset()->IsLearnedSkill(m_lSkillID, true);
	int iCurLevel = pkSkillAttrib->_lCurLevel;

	StringStreamType kStringStream;
	int lTopLevel = pkSkillAttrib->_lTopLevel;

	if (GameLogic::LC_Helper::IsSecretClientSkill(m_lSkillID))
	{
		int level1 = (iCurLevel-1)/10+1;
		int level2 = iCurLevel + 10 - level1 * 10;
		kStringStream << "<color val=0xff00ee00>" << level1 << GET_MSG(LC_SKILLTOOLTIPBASE_CPP_002) << level2 << GET_MSG(LC_SKILLTOOLTIPBASE_CPP_003);
	}
	else
	{
		kStringStream << "<color val=0xff00ee00>" << iCurLevel << "/" << lTopLevel << GET_MSG(LC_SKILLTOOLTIPBASE_CPP_004);
	}	

	kFieldVector.push_back( LC_TooltipField(TOOLTIP_FIELD_SKILL_LEVEL, GET_MSG(LC_SKILLDETAIL_CPP_004), true, kStringStream.str(), usFont, usFontSize, iFontColor, 0, 0));

	return true;
}

bool LC_SkillTooltipBase::_addTooltipFieldLevelNext(TooltipFieldVector& kFieldVector)
{
	CF_SkillSpecialAttrib::DataEntry* pkSkillData = SafeGetCSVFileDataEntryPtr<CF_SkillSpecialAttrib>( m_lSkillID );
	if (NULL == pkSkillData)
	{
		return false;
	}

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = WHITE;

	StringStreamType kStringStream;
	int lCurLevel = pkSkillData->_lCurLevel;
	int lTopLevel = pkSkillData->_lTopLevel;
	kStringStream << "<color val=0xff00ee00>" << lCurLevel << "/" << lTopLevel << GET_MSG(LC_SKILLTOOLTIPBASE_CPP_004);

	kFieldVector.push_back( LC_TooltipField(TOOLTIP_FIELD_SKILL_LEVEL, GET_MSG(LC_SKILLDETAIL_CPP_004), true, kStringStream.str(), usFont, usFontSize, iFontColor, 0, 0));

	return true;
}

bool LC_SkillTooltipBase::_addTooltipFieldDescript( TooltipFieldVector& kFieldVector )
{
	CF_SkillExecuteList::DataEntry* pkSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(m_lSkillID);
	if (0 == pkSkillData)
	{
		return false;
	}

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = WHITE;


	StringStreamType kStringStream;
	kStringStream << pkSkillData->_kDescript;

	kFieldVector.push_back( LC_TooltipField(TOOLTIP_FIELD_SKILL_DESCRIPT, GET_MSG(LC_SKILLTOOLTIPBASE_CPP_005), true, kStringStream.str(), usFont, usFontSize, iFontColor, 0, 0));

	return true;
}

bool LC_SkillTooltipBase::_addTooltipFieldCategory( TooltipFieldVector& kFieldVector )
{
	CF_SkillExecuteList::DataEntry* pkSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(m_lSkillID);
	if (NULL == pkSkillData)
	{
		return false;
	}

//	int lPosNegType = pkSkillData->_lPosNegType;			//主被动类型
	int lRangeType = pkSkillData->_lRangeType;				//距离类型
//	int lDamageType = pkSkillData->_lDamageType;			//伤害类型

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = WHITE;

	/*if (lPosNegType > 0)
	{
		StringStreamType kPosNegStrStream;
		switch (lPosNegType)
		{
		case 0:
			kPosNegStrStream << "";
		case 1:
			kPosNegStrStream << "主动";
			break;
		case 2:
			kPosNegStrStream << "被动";
			break;
		default:
			kPosNegStrStream << "";
			break;
		}

		kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_SKILL_CATEGORY, "技能主被动类型", true, kPosNegStrStream.str(), usFont, usFontSize, iFontColor, 0, 0));
	}*/	

	
	////如果是伤害性机能
	//if (lDamageType > 0)
	{
		StringStreamType kDistanceStrStream;
		switch (lRangeType)
		{
		case SRT_COMMON:
			kDistanceStrStream << GET_MSG(LC_DEFINE_H_008);
			break;
		case SRT_MELEE:
			kDistanceStrStream << GET_MSG(LC_TOOLTIPMANAGER_CPP_003);
			break;
		case SRT_RANGE:
			kDistanceStrStream << GET_MSG(LC_TOOLTIPMANAGER_CPP_004);
			break;
		default:
			kDistanceStrStream << "";
			break;
		}
		bool bNewLine = true;
		kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_SKILL_CATEGORY, GET_MSG(LC_SKILLTOOLTIPBASE_CPP_006), true, kDistanceStrStream.str(), usFont, usFontSize, iFontColor, 0, 0, bNewLine));
	}

	return false;
}

bool LC_SkillTooltipBase::_addTooltipFieldCurLevel( TooltipFieldVector& kFieldVector )
{
	CF_SkillSpecialAttrib::DataEntry* pkSkillData = SafeGetCSVFileDataEntryPtr<CF_SkillSpecialAttrib>( m_lSkillID );
	if (0 == pkSkillData)
	{
		return false;
	}

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = WHITE;

	StringStreamType kStringStream;
	int lCurLevel = pkSkillData->_lCurLevel;
	StringType kCurLevel = _numArabToChinese(lCurLevel);
	kStringStream << GET_MSG(LC_ITEMTOOLTIPBASE_H_003) << kCurLevel << GET_MSG(LC_MEDITATETOOLTIP_CPP_010);

	kFieldVector.push_back( LC_TooltipField(TOOLTIP_FIELD_SKILL_CURLEVEL, GET_MSG(LC_SKILLTOOLTIPBASE_CPP_007), true, kStringStream.str(), usFont, usFontSize, iFontColor, 0, 0));

	return true;
}

bool LC_SkillTooltipBase::_addTooltipFieldPlayerLevel(TooltipFieldVector &kFieldVector)
{
	CF_SkillLevelUp *pkSkillLevelUp = CF_SkillLevelUp::GetSingletonPtr();
	CF_SkillLevelUp::DataEntry *pkSkillLvUpData = pkSkillLevelUp->GetEntryPtr(m_lSkillID);

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = WHITE;
	StringStreamType kStringStream;
	if(NULL != pkSkillLvUpData)
	{
		int lNeedLevel = pkSkillLvUpData->_lNeedLevel;				//需要达到的人物等级	
		if(lNeedLevel > 0)
		{
			kStringStream << GET_MSG(LC_SKILLTOOLTIPBASE_CPP_008);
			kStringStream << lNeedLevel;
			LC_ActorBase* pkObserver = GetObserver();
			if (NULL != pkObserver)
			{
				iFontColor = (pkObserver->GetLevel() < lNeedLevel ? RED : WHITE );
			}
		}
		else
		{
			kStringStream << " ";
		}
	}
	else
	{
		kStringStream << " ";
	}

	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_SKILL_PLAYER_LEVEL, GET_MSG(LC_SKILLTOOLTIPBASE_CPP_009), true, kStringStream.str(), usFont, usFontSize, iFontColor, 0, 0, false));
	return true;
}

bool LC_SkillTooltipBase::_addTooltipFieldConsume( TooltipFieldVector& kFieldVector )
{
	CF_SkillExecuteList::DataEntry* pkSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(m_lSkillID);
	if (0 == pkSkillData)
	{
		return false;
	}

	LC_ActorBase* pkObserver = GetObserver();
	if (NULL == pkObserver || !IS_PLAYER_ID(pkObserver->GetID()))
	{
		return false;
	}

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = WHITE;
	
	LC_PlayerBase *pkPlayer = ((LC_PlayerBase*)pkObserver);
	
	/*int iCostHP = pkSkillCostData->_iHPValueCost;
	int iCostMP = pkSkillCostData->_iMPValueCost;
	float fCostHPPercent = pkSkillCostData->_fHPPercentCost;
	float fCostMPPercent = pkSkillCostData->_fMPPercentCost;
	
	StringStreamType kStringStream;
	kStringStream.str("");
	if (iCostHP > 0)
	{
		kStringStream << iCostHP << "生命";
	}
	if (iCostMP > 0)
	{
		if ("" != kStringStream.str())
		{
			kStringStream << " ";
		}
		kStringStream << iCostMP << "内息";
	}
	if (fCostHPPercent > FLT_EPSILON)
	{
		if ("" != kStringStream.str())
		{
			kStringStream << " ";
		}
		kStringStream << fCostHPPercent * 100 << "% 生命";
	}
	if (fCostMPPercent > FLT_EPSILON)
	{
		if ("" != kStringStream.str())
		{
			kStringStream << " ";
		}
		kStringStream << fCostMPPercent * 100 << "% 内息";
	}*/


	/*
	*注意：
	*1、技能消耗MP、消耗物品在SkillExecuteList中取，
	*2、技能消耗HP只在小绿字中显示
	*/
	
	StringStreamType kStrStream;
	kStrStream << GET_MSG(LC_SKILLTOOLTIPBASE_CPP_012);

	if (SKILL_TYPE_QINGGONG == pkSkillData->_lCategory2)	//轻功
	{
		int iStamina = pkSkillData->_iStamina;
		if (pkPlayer->GetStamina() < iStamina)
		{
			kStrStream << RED;
		}
		else
		{
			kStrStream << GREEN;
		}

		
		kStrStream << ">" << iStamina << GET_MSG(LC_SKILLTOOLTIPBASE_CPP_013);
		kFieldVector.push_back( LC_TooltipField(TOOLTIP_FIELD_SKILL_CONSUME, GET_MSG(LC_SKILLTOOLTIPBASE_CPP_014), true, kStrStream.str(), usFont, usFontSize, iFontColor, 0, 0));
	}
	else if (SKILL_TYPE_ARROW == pkSkillData->_lCategory2)		//弩炮
	{
		int iArrowAmount = pkSkillData->_iArrowAmount;
		if(iArrowAmount <= 0)
			return false;

		if (pkPlayer->GetBowAmount() < iArrowAmount)
		{
			kStrStream << RED;
		}
		else
		{
			kStrStream << GREEN;
		}

		kStrStream << ">" << iArrowAmount << GET_MSG(LC_SKILLTOOLTIPBASE_CPP_015);
		kFieldVector.push_back( LC_TooltipField(TOOLTIP_FIELD_SKILL_CONSUME, GET_MSG(LC_SKILLTOOLTIPBASE_CPP_016), true, kStrStream.str(), usFont, usFontSize, iFontColor, 0, 0));
	}
	else	//技能消耗
	{
		int64_t iDefaultMPCost = LC_Helper::GetSkillDefaultMPCost(GetObserver(), pkSkillData);
		int64_t iAdditionHPCost = LC_Helper::GetSkillAdditionHPCost(GetObserver(), pkSkillData);
		int64_t iAdditionMPCost = LC_Helper::GetSkillAdditionMPCost(GetObserver(), pkSkillData);

		int64_t iCostHP = iAdditionHPCost;
		int64_t iCostMP = iDefaultMPCost + iAdditionMPCost;
		if (iCostHP > 0)
		{
			if (pkPlayer->GetHP() < iCostHP)
			{
				kStrStream << RED;
			}
			else
			{
				kStrStream << GREEN;
			}
			kStrStream << ">" << iCostHP << GET_MSG(LC_SKILLTOOLTIPBASE_CPP_017) << ",";
		}
		else if (iCostMP > 0)
		{
			if (pkPlayer->GetMP() < iCostMP)
			{
				kStrStream << RED;
			}
			else
			{
				kStrStream << GREEN;
			}
			kStrStream << ">" << iCostMP << GET_MSG(LC_SKILLTOOLTIPBASE_CPP_018);
		}
		else
		{
			kStrStream.str("");
		}

		StringType kString = kStrStream.str();
		if (kString == "")
		{
			return false;
		}

		//剪除末尾的逗号
		if (',' == kString[kString.length() - 1])
		{
			kString = kString.substr(0, kString.length() - 1);
		}
		
		kFieldVector.push_back( LC_TooltipField(TOOLTIP_FIELD_SKILL_CONSUME, GET_MSG(LC_SKILLTOOLTIPBASE_CPP_019), true, kString, usFont, usFontSize, iFontColor, 0, 0));
	}

	return true;
}

bool LC_SkillTooltipBase::_addTooltipFieldExecuteDistance( TooltipFieldVector& kFieldVector )
{
	CF_SkillExecuteList::DataEntry* pkSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(m_lSkillID);
	if (NULL == pkSkillData)
	{
		return false;
	}

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = WHITE;

	int lMinExecuteDistance = pkSkillData->_lMinExecuteDistance;
	int lMaxExecuteDistance = pkSkillData->_lMaxExecuteDistance;

	StringStreamType kStringStream;
	if (lMinExecuteDistance <= 0 && lMaxExecuteDistance <= 0)
	{
		//kStringStream << "";
	}
	else if (lMinExecuteDistance <= lMaxExecuteDistance && lMaxExecuteDistance > 0)
	{
		kStringStream << (double)lMaxExecuteDistance / (double)100 << GET_MSG(LC_SKILLTOOLTIPBASE_CPP_020);
	}
	else if (lMinExecuteDistance > 0 && lMaxExecuteDistance > 0 && lMinExecuteDistance < lMaxExecuteDistance)
	{
		kStringStream << (double)lMinExecuteDistance / (double)100 << " -" << (double)lMaxExecuteDistance / (double)100 << GET_MSG(LC_SKILLTOOLTIPBASE_CPP_020);
	}
	
	kFieldVector.push_back( LC_TooltipField(TOOLTIP_FIELD_SKILL_EXECUTE_DISTANCE, GET_MSG(LC_SKILLTOOLTIPBASE_CPP_021), true, kStringStream.str(), usFont, usFontSize, iFontColor, 0, 0));

	return true;
}

bool LC_SkillTooltipBase::_addTooltipFieldExecuteTime( TooltipFieldVector& kFieldVector )
{
	CF_SkillExecuteList::DataEntry* pkSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(m_lSkillID);
	if (NULL == pkSkillData)
	{
		return false;
	}

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = WHITE;

	bool bImmediate = pkSkillData->_bExecuteImmediately;
	if (bImmediate)
	{
		StringStreamType kStringStream;
		kStringStream << GET_MSG(LC_SKILLTOOLTIPBASE_CPP_022);
		kFieldVector.push_back( LC_TooltipField(TOOLTIP_FIELD_SKILL_EXECUTE_TIME, GET_MSG(LC_SKILLTOOLTIPBASE_CPP_021), true, kStringStream.str(), usFont, usFontSize, iFontColor, 0, 0));
		return true;
	}
	
	int lExecuteType = pkSkillData->_lExecuteType;
	float fExecuteTime = pkSkillData->_fExecuteTime;
	if (lExecuteType < 0 || fExecuteTime < 0)
	{
		return false;
	}

	
	//判断数值是否为整数
	bool bInteger = true;
	if (fExecuteTime - (int)fExecuteTime < 0.01)
	{
		bInteger = true;
	}
	else
	{
		bInteger = false;
	}

	if (lExecuteType > 0)
	{
		StringStreamType kStringStream;
		switch(lExecuteType)
		{
		case 0:
			kStringStream << "";
			break;
		case 1:
			if (fExecuteTime <= 0)
			{
				kStringStream << GET_MSG(LC_SKILLTOOLTIPBASE_CPP_022);
			}
			else
			{
				if (bInteger)
				{
					kStringStream << (int)fExecuteTime << GET_MSG(LC_SKILLTOOLTIPBASE_CPP_023);
				}
				else
				{
					//显示一位小数
					kStringStream << setiosflags(ios::fixed) << setprecision(1) << fExecuteTime << GET_MSG(LC_SKILLTOOLTIPBASE_CPP_023);
				}
			}
			break;
		case 2:
			if (fExecuteTime <= 0)
			{
				kStringStream << GET_MSG(LC_SKILLTOOLTIPBASE_CPP_022);
			}
			else
			{
				if (bInteger)
				{
					kStringStream << (int)fExecuteTime << GET_MSG(LC_SKILLTOOLTIPBASE_CPP_024);
				}
				else
				{
					//显示一位小数
					kStringStream << setiosflags(ios::fixed) << setprecision(1) << fExecuteTime << GET_MSG(LC_SKILLTOOLTIPBASE_CPP_024);
				}
			}
			break;
		default:
			kStringStream << "";
			break;
		}
		kFieldVector.push_back( LC_TooltipField(TOOLTIP_FIELD_SKILL_EXECUTE_TIME, GET_MSG(LC_SKILLTOOLTIPBASE_CPP_021), true, kStringStream.str(), usFont, usFontSize, iFontColor, 0, 0));
	}

	return true;
}

bool LC_SkillTooltipBase::_addTooltipFieldCDTime( TooltipFieldVector& kFieldVector )
{
	CF_SkillExecuteList::DataEntry* pkSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(m_lSkillID);
	if (NULL == pkSkillData)
	{
		return false;
	}

	if(pkSkillData->_lCategory1 == SKILL_TYPE_SUB_ACTOR_GODTHING_ACTIVATE)
	{
		return false;
	}

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = WHITE;

	//根据角色身上保存的CD修改功能列表，修改技能CD
	float fCDTime = pkSkillData->_fCoolDownTime;
	float fCDTimeBase = pkSkillData->_fCoolDownTime;
	LC_ActorBase* pkObserver = GetObserver();
	if (NULL != pkObserver && IS_PLAYER_ID(pkObserver->GetID()))
	{
		LC_SkillCoolDownMap* pkSkillCDMap = ((LC_PlayerBase*)pkObserver)->GetSkillCoolDownMap();
		if (NULL != pkSkillCDMap)
		{
			CF_SkillCDModifyFunc* pkCDModifyFuncList = CF_SkillCDModifyFunc::GetSingletonPtr();
			CF_SkillCDModifyFunc::DataEntry* pkCDModifyFuncData = NULL;
			for (int i = 0; i < MAX_ACTOR_SKILL_CD_MODIFY_FUNC_COUNT; i ++)
			{
				int iFuncID = pkSkillCDMap->GetCDModifyFuncByIndex(i);
				pkCDModifyFuncData = pkCDModifyFuncList->GetEntryPtr(iFuncID);
				if (NULL != pkCDModifyFuncData)
				{
					if (pkSkillData->_lCoolDownType1 == pkCDModifyFuncData->_lCDType1
						&& pkSkillData->_lCoolDownType2 == pkCDModifyFuncData->_lCDType2
						/*&& pkSkillData->_lCoolDownType3 == pkCDModifyFuncData->_lCDType3*/)
					{
						fCDTime = fCDTime - pkCDModifyFuncData->_fValueModify - fCDTimeBase * pkCDModifyFuncData->_fPercentModify;
					}
				}
			}
		}
	}

	fCDTime = UT_MathBase::Max(0, fCDTime);
	if (fCDTime <= 0.01)
	{
		return false;
	}

	StringStreamType kStringStream;
	kStringStream << "<color val=" << GOLD << GET_MSG(LC_SKILLTOOLTIPBASE_CPP_025) << GREEN << ">" << fCDTime << GET_MSG(LC_SKILLTOOLTIPBASE_CPP_026);

	kFieldVector.push_back( LC_TooltipField(TOOLTIP_FIELD_SKILL_CD_TIME, GET_MSG(LC_SKILLTOOLTIPBASE_CPP_027), true, kStringStream.str(), usFont, usFontSize, iFontColor, 0, 0));

	return true;
}

bool LC_SkillTooltipBase::_addTooltipFieldMaterialRequired( TooltipFieldVector& kFieldVector )
{
	CF_SkillExecuteList::DataEntry* pkSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(m_lSkillID);
	if (NULL == pkSkillData)
	{
		return false;
	}

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = WHITE;

	/*int lItem1ID = pkSkillCostData->_kItemCost1.x;
	int lItem2ID = pkSkillCostData->_kItemCost2.x;
	int lItem3ID = pkSkillCostData->_kItemCost3.x;*/

	int lItem1ID = pkSkillData->_kAdditionItemCost.x;
	/*int lItem2ID = 0;
	int lItem3ID = 0;*/

	//该技能不需要消耗任何物品
	if (lItem1ID <= 0/* && lItem2ID <= 0 && lItem3ID <= 0*/)
	{
		return true;
	}

	StringStreamType kRequry;
	kRequry << GET_MSG(LC_TOOLTIPMANAGER_CPP_009);
	kFieldVector.push_back( LC_TooltipField(TOOLTIP_FIELD_SKILL_MATERIAL_REQUIRED, GET_MSG(LC_SKILLTOOLTIPBASE_CPP_028), true, kRequry.str(), usFont, usFontSize, iFontColor, 0, 0));

	CF_ItemList* pkItemList = CF_ItemList::GetSingletonPtr();
	if (IS_ITEM_ID(lItem1ID))
	{
		StringStreamType kItem1Required;

		CF_ItemList::DataEntry* pkItem1Data = pkItemList->GetEntryPtr(lItem1ID);
		if (0 == pkItem1Data)
		{
			GfxWriteLog(LOG_ERROR_FILELINE, Utility::LOG_SWITCH_ERROR, "Can't find %d in ItemList!", lItem1ID);
			return false;
		}

		kItem1Required << pkItem1Data->_kName;

		LC_ActorBase* pkObserver = GetObserver();
		if (NULL == pkObserver || !IS_PLAYER_ID(pkObserver->GetID()))
		{
			iFontColor = WHITE;
		}
		else
		{
			LC_PlayerBase* pkPlayer = static_cast<LC_PlayerBase*>(pkObserver);
			LC_PackAsset* pkPackAsset = pkPlayer->GetPackAsset();

			int iItem1PackType = GetWhichPackAutoPut(lItem1ID);
			//如果该物品在背包中不存在
			if ( !pkPackAsset->IsExistItem(iItem1PackType, lItem1ID) )
			{
				iFontColor = RED;
			}

			int lCountRequired = pkSkillData->_kAdditionItemCost.y;
			kItem1Required << "(" << lCountRequired << ")";
			//如果背包中物品数量不够			
			if (lCountRequired > pkPackAsset->GetItemCountInPack(iItem1PackType, lItem1ID))
			{
				iFontColor = RED;
			}
		}

		bool bNewLine = false;
		kFieldVector.push_back( LC_TooltipField(TOOLTIP_FIELD_SKILL_MATERIAL_REQUIRED, GET_MSG(LC_SKILLTOOLTIPBASE_CPP_029), true, kItem1Required.str(), usFont, usFontSize, iFontColor, 0, 0, bNewLine));
	}
	
	//if (IS_ITEM_ID(lItem2ID))
	//{
	//	StringStreamType kItem2Required;

	//	CF_ItemList::DataEntry* pkItem2Data = pkItemList->GetEntryPtr(lItem2ID);
	//	if (0 == pkItem2Data)
	//	{
	//		GfxWriteLog(LOG_ERROR_FILELINE, Utility::LOG_SWITCH_ERROR, "Can't find %d in ItemList!", lItem2ID);
	//		return false;
	//	}

	//	kItem2Required << pkItem2Data->_kName;

	//	LC_ActorBase* pkObserver = GetObserver();
	//	if (0 == pkObserver || !IS_PLAYER_ID(pkObserver->GetID()))
	//	{
	//		iFontColor = WHITE;
	//	}
	//	else
	//	{
	//		LC_PlayerBase* pkPlayer = dynamic_cast<LC_PlayerBase*>(pkObserver);			
	//		LC_PackAsset* pkPackAsset = pkPlayer->GetPackAsset();
	//		//如果该物品在背包中不存在			
	//		int iItem2PackType = GetWhichPackAutoPut(lItem2ID);
	//		if (!pkPackAsset->IsExistItem(iItem2PackType, lItem2ID))
	//		{
	//			iFontColor = RED;
	//		}
	//	}
	//	
	//	bool bNewLine = false;
	//	kFieldVector.push_back( LC_TooltipField(TOOLTIP_FIELD_SKILL_MATERIAL_REQUIRED, "技能需求Item2", true, kItem2Required.str(), usFont, usFontSize, iFontColor, 0, 0, bNewLine));
	//}
	//
	//if (IS_ITEM_ID(lItem3ID))
	//{
	//	StringStreamType kItem3Required;

	//	CF_ItemList::DataEntry* pkItem3Data = pkItemList->GetEntryPtr(lItem3ID);
	//	if (0 == pkItem3Data)
	//	{
	//		GfxWriteLog(LOG_ERROR_FILELINE, Utility::LOG_SWITCH_ERROR, "Can't find %d in ItemList!", lItem3ID);
	//		return false;
	//	}

	//	kItem3Required << pkItem3Data->_kName;

	//	LC_ActorBase* pkObserver = GetObserver();
	//	if (0 == pkObserver || !IS_PLAYER_ID(pkObserver->GetID()))
	//	{
	//		iFontColor = WHITE;
	//	}
	//	else
	//	{
	//		LC_PlayerBase* pkPlayer = dynamic_cast<LC_PlayerBase*>(pkObserver);
	//		LC_PackAsset* pkPackAsset = pkPlayer->GetPackAsset();
	//		int iItem3PackType = GetWhichPackAutoPut(lItem3ID);

	//		//如果该物品在背包中不存在
	//		if (!pkPackAsset->IsExistItem(iItem3PackType, lItem3ID))
	//		{
	//			iFontColor = RED;
	//		}
	//	}?

	//	bool bNewLine = true;
	//	kFieldVector.push_back( LC_TooltipField(TOOLTIP_FIELD_SKILL_MATERIAL_REQUIRED, "技能需求Item3", true, kItem3Required.str(), usFont, usFontSize, iFontColor, 0, 0, bNewLine));
	//}

	return true;
}

bool LC_SkillTooltipBase::_addTooltipFieldUseResult( TooltipFieldVector& kFieldVector )
{
	CF_SkillSpecialAttrib::DataEntry *pkSkillData = SafeGetCSVFileDataEntryPtr<CF_SkillSpecialAttrib>(m_lSkillID);
	if (NULL == pkSkillData)
	{
		return false;
	}

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = WHITE;

	StringType kUseResult = "";
	//回城技能
	if (SKILL_RETURN_CITY == m_lSkillID)
	{
		if (NULL == GetObserver() || !IS_PLAYER_ID(GetObserver()->GetID()))
		{
			return false;
		}
		LC_PlayerBase* pkObserver = (LC_PlayerBase*)GetObserver();

		LC_LogicMapManager* pkLogicMapManager = LC_LogicMapManager::GetSingletonPtr();
		CF_WorldMapList* pkWorldMapList = CF_WorldMapList::GetSingletonPtr();
		CF_WorldMapList::DataEntry* pkWorldMapData = pkWorldMapList->GetEntryPtr(1);
		if (NULL == pkWorldMapData)
		{
			return false;
		}

		//int lTransferToMapID = pkObserver->GetDefaultTransferMapID();
		//UT_Vec3Int kTransferToPos = pkObserver->GetDefaultTransferPos();
		//if (lTransferToMapID <= 0)
		//{
			//lTransferToMapID = 1;
			//kTransferToPos = pkWorldMapData->_kSpawnLocation;
		//}
		int lTransferToMapID = pkWorldMapData->_iReturnBackResId;
		UT_Vec3Int kTransferToPos = pkWorldMapData->_iReturnBackPos;

		LC_LogicAreaInfo* kLogicAreaInfo = pkLogicMapManager->QueryAreaInfo(kTransferToPos,lTransferToMapID);
		StringType sd = kLogicAreaInfo ? kLogicAreaInfo->kAreaName : "";
		kUseResult = GET_MSG(LC_SKILLTOOLTIPBASE_CPP_030) + sd;
	}
	else if ( !(LC_TooltipBase::_getUseResult<CSVFile::CF_SkillSpecialAttrib>(m_lSkillID, kUseResult)) )
	{
		kUseResult = pkSkillData->_kUseResult;
	}

	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_SKILL_USE_RESULT, GET_MSG(LC_SKILLTOOLTIPBASE_CPP_031), true, kUseResult, usFont, usFontSize, iFontColor, 0, 0));

	return true;
}

bool LC_SkillTooltipBase::_addTooltipFieldUnlockContent(TooltipFieldVector& kFieldVector)
{
	LC_PlayerBase *pkPlayer = ((LC_PlayerBase*)GetObserver());
	
	bool bLearned = NULL != pkPlayer ? pkPlayer->GetSkillAsset()->IsLearnedSkill(m_lSkillID, true) : true;
	if(bLearned)
	{
		return false;
	}

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = RED;

	StringStreamType sStream("");

	CF_OpenLevel *pkCSVOpenLevel = CF_OpenLevel::GetSingletonPtr();
	CF_OpenLevel::DataEntryMapExternIterator iter = pkCSVOpenLevel->GetIterator();

	int iNeedLevel = -1;
	int iSignCount = -1;
	//查找openlevel表里是否有该技能ID
	while(iter.HasMoreElements())
	{
		CF_OpenLevel::DataEntry *pkData = iter.PeekNextValuePtr();
		if(m_lSkillID == pkData->_KeyID)
		{
			if(pkData->_CharLevel > 1)
				iNeedLevel = pkData->_CharLevel;
			else if(pkData->_SignCount > 0)
				iSignCount = pkData->_SignCount;

			break;
		}

		iter.MoveNext();
	}
	if(-1 != iNeedLevel)
	{
		sStream << GET_MSG(LC_SKILLTOOLTIPBASE_CPP_032) << iNeedLevel << GET_MSG(LC_SKILLTOOLTIPBASE_CPP_033);
	}
	else if(-1 != iSignCount)
	{
		sStream << GET_MSG(LC_SKILLTOOLTIPBASE_CPP_034) << iSignCount << GET_MSG(LC_SKILLTOOLTIPBASE_CPP_035);
	}
	else
	{
		CF_SkillExecuteList::DataEntry *pkSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(m_lSkillID);
		if(NULL != pkSkillData)
		{
			/*switch(pkSkillData->_lCategory2)
			{
			case SKILL_TYPE_MOUNT:
				{
					CF_Vehicle *pkCsvVehicle = CF_Vehicle::GetSingletonPtr();
					CF_Vehicle::DataEntryMapExternIterator it = pkCsvVehicle->GetIterator();
					for( ; it.HasMoreElements(); it.MoveNext())
					{
						CF_Vehicle::DataEntry *pkVehicle = it.PeekNextValuePtr();
						if(pkVehicle->_DisplaySkillID == m_lSkillID)
						{
							iNeedLevel = pkVehicle->_iID;
							sStream << "坐骑" << iNeedLevel << "阶解锁";
							break;
						}
					}
				}
				break;
			case SKILL_TYPE_ARROW:
				{
					CF_Arrow *pkCsvArrow = CF_Arrow::GetSingletonPtr();
					CF_Arrow::DataEntryMapExternIterator it = pkCsvArrow->GetIterator();
					for( ; it.HasMoreElements(); it.MoveNext())
					{
						CF_Arrow::DataEntry *pkArrow = it.PeekNextValuePtr();
						if(pkArrow->_DisplaySkillID == m_lSkillID)
						{
							iNeedLevel = pkArrow->_iID;
							sStream << "弩炮" << iNeedLevel << "阶解锁";
							break;
						}
					}
				}
				break;
			case SKILL_TYPE_DEVAEYE:
				{
					CF_DevaEye *pkCsvDevaEye = CF_DevaEye::GetSingletonPtr();
					CF_DevaEye::DataEntryMapExternIterator it = pkCsvDevaEye->GetIterator();
					for( ; it.HasMoreElements(); it.MoveNext())
					{
						CF_DevaEye::DataEntry *pkDevaEye = it.PeekNextValuePtr();
						if(pkDevaEye->_DisplaySkillID == m_lSkillID)
						{
							iNeedLevel = pkDevaEye->_iID;
							sStream << "宠物" << iNeedLevel << "阶解锁";
							break;
						}
					}
				}
				break;
			case SKILL_TYPE_VEHICLE_FIGHTING:
				{
					CF_VehicleFighting *pkCsvVf = CF_VehicleFighting::GetSingletonPtr();
					CF_VehicleFighting::DataEntryMapExternIterator it = pkCsvVf->GetIterator();
					for( ; it.HasMoreElements(); it.MoveNext())
					{
						CF_VehicleFighting::DataEntry *pkVf = it.PeekNextValuePtr();
						if(pkVf->_DisplaySkillID == m_lSkillID)
						{
							iNeedLevel = pkVf->_iID;
							sStream << "骑兵" << iNeedLevel << "阶解锁";
							break;
						}
					}
				}
				break;
			}		*/
		
			if(pkSkillData->_lSkillBookID > 0)
			{
				CF_ItemList::DataEntry *pkItemData = SafeGetCSVFileDataEntryPtr<CF_ItemList>(pkSkillData->_lSkillBookID);
				if(NULL != pkItemData)
				{
					sStream << GET_MSG(LC_SKILLTOOLTIPBASE_CPP_036) << pkItemData->_kName << "]";
					iNeedLevel = 0;
				}
			}
		}
	}
	
	if(-1 == iNeedLevel && -1 == iSignCount)
	{
		return false;
	}

	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_SKILL_UNLOCK_LEVEL, GET_MSG(LC_SKILLTOOLTIPBASE_CPP_037), true, sStream.str(), usFont, usFontSize, iFontColor, 0, 0));

	return true;
}

//bool LC_SkillTooltipBase::_addTooltipFieldUseDescript(TooltipFieldVector& kFieldVector)
//{
//	CF_SkillExecuteList* pkSkillList = CF_SkillExecuteList::GetSingletonPtr();
//	CF_SkillExecuteList::DataEntry* pkSkillData = pkSkillList->GetEntryPtr( m_lSkillID );
//	if (0 == pkSkillList)
//	{
//		return false;
//	}
//
//	unsigned short usFont = FONT_MIDDLE;
//	unsigned short usFontSize = FONTSIZE_MIDDLE;
//	int iFontColor = DARKORANGE;
//
//	StringType kUseDescript = pkSkillData->_kUseDescript;
//	if ("" == kUseDescript || "NULL" == kUseDescript)
//	{
//		return false;
//	}
//	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_SKILL_USE_DESCRIPT, "技能用法介绍", true, kUseDescript, usFont, usFontSize, iFontColor, 0, 0));
//
//	return true;
//}

//bool LC_SkillTooltipBase::_addTooltipFieldAtmosDescript(TooltipFieldVector& kFieldVector)
//{
//	CF_SkillExecuteList* pkSkillList = CF_SkillExecuteList::GetSingletonPtr();
//	CF_SkillExecuteList::DataEntry* pkSkillData = pkSkillList->GetEntryPtr( m_lSkillID );
//	if (NULL == pkSkillList || NULL == pkSkillData)
//	{
//		return false;
//	}
//
//	unsigned short usFont = FONT_MIDDLE;
//	unsigned short usFontSize = FONTSIZE_MIDDLE;
//	int iFontColor = DARKORANGE;
//
//	StringType kAtmosDescript = pkSkillData->_kAtmosDescript;
//	if ("" == kAtmosDescript || "NULL" == kAtmosDescript)
//	{
//		return false;
//	}
//	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_SKILL_ATMOS_DESCRIPT, "技能氛围介绍", true, kAtmosDescript, usFont, usFontSize, iFontColor, 0, 0));
//
//	return true;
//}

bool LC_SkillTooltipBase::_addTooltipFieldNextLevel( TooltipFieldVector& kFieldVector )
{
	CF_SkillLevelUp* pkSkillLevelup = CF_SkillLevelUp::GetSingletonPtr();
	CF_SkillLevelUp::DataEntry* pkLevelUpData = pkSkillLevelup->GetEntryPtr( m_lSkillID );
	if (0 == pkLevelUpData)
	{
		//GfxWriteLog(LOG_ERROR_FILELINE, Utility::LOG_SWITCH_ERROR, "Can't find %d in SkillLevelUp!",  m_lSkillID );
		return false;
	}

	LC_PlayerBase *pkPlayer = ((LC_PlayerBase*)GetObserver());
	if(NULL == pkPlayer)
	{
		return false;
	}

	int lNextLevelID = pkLevelUpData->_lNextLevelSkillTypeID;
	bool bLearned = pkPlayer->GetSkillAsset()->IsLearnedSkill(m_lSkillID, true);
	//如果已经顶级，则不显示下一重
	//如果是未学技能，不显示下一重
	if (lNextLevelID <= 0 || !bLearned)
	{
		return false;
	}

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = WHITE;

	StringType kContent = GET_MSG(LC_SKILLTOOLTIPBASE_CPP_038);
	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_SKILL_NEXT_LEVEL, GET_MSG(LC_SKILLTOOLTIPBASE_CPP_039), true, kContent, usFont, usFontSize, iFontColor, 0, 0));

	/*
	*引用下一重技能信息
	*/
	{
		//暂存并设置需要构造tooltip的技能ID
		int lSaveSkillID = m_lSkillID;
		SetSkillID(lNextLevelID);

		//技能名称，下一重不需要显示技能名称
		//_addTooltipFieldName(lNextLevelID, pkBackPackEntry, pkObserver, kFieldVector);
		//技能当前等级 不需判断是否为已学技能
		_addTooltipFieldLevelNext(kFieldVector);
		////技能限制
		//_addTooltipFieldLimit(kFieldVector);
		//技能消耗
		_addTooltipFieldConsume(kFieldVector);
		//施放距离
		//_addTooltipFieldExecuteDistance(kFieldVector);
		//施放时间
		//_addTooltipFieldExecuteTime(kFieldVector);
		//技能类型
		//_addTooltipFieldCategory(kFieldVector);
		//冷却时间
		_addTooltipFieldCDTime(kFieldVector);
		//技能需求
		//_addTooltipFieldMaterialRequired(kFieldVector);
		//技能效果
		_addTooltipFieldUseResult(kFieldVector);

		//回滚技能ID
		SetSkillID(lSaveSkillID);
	}
	
	
	return true;
}

bool LC_SkillTooltipBase::_getSkillLevels( int lSkillID, std::vector<int>& kSkillLevels )
{
	CF_SkillLevelUp* pkSkillLevelUp = CF_SkillLevelUp::GetSingletonPtr();
	CF_SkillLevelUp::DataEntry* pkLevelUpData = pkSkillLevelUp->GetEntryPtr( m_lSkillID );
	//如果在技能升级表中找不到该技能，则这是一个单一技能，直接在SkillExecuteList中查找该技能
	if (0 == pkLevelUpData)
	{
		/*GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "Can't find %d in SkillLevelUp!",  m_lSkillID );
		return false;*/

		CF_SkillExecuteList::DataEntry* pkSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(m_lSkillID);
		if (0 == pkSkillData)
		{
			GfxWriteLog(LOG_ERROR_FILELINE, Utility::LOG_SWITCH_ERROR, "Can't find %d in SkillExecuteList!", m_lSkillID);
			return false;
		}
		kSkillLevels.push_back(pkSkillData->_iID);
		return true;
	}

	int iLevelUpCount = 0;
	int iLevelUpLimit = 10;
	//查找第一级技能
	while (pkLevelUpData->_lLastLevelSkillTypeID > -1)
	{
		++ iLevelUpCount;
		//升级不能过多，防止填表错误出现循环
		if (iLevelUpCount > iLevelUpLimit)
		{
			break;
		}

		pkLevelUpData = pkSkillLevelUp->GetEntryPtr(pkLevelUpData->_lLastLevelSkillTypeID);
		if (0 == pkLevelUpData)
		{
			GfxWriteLog(LOG_ERROR_FILELINE, Utility::LOG_SWITCH_ERROR, "Can't find %d in SkillLevelUp!", m_lSkillID);
			return false;
		}
	}

	iLevelUpCount = 0;
	//升级的下一级技能
	int lNextSkillID = 0;
	do 
	{
		++iLevelUpCount;
		//升级不能过多，防止填表错误出现循环
		if (iLevelUpCount > iLevelUpLimit)
		{
			break;
		}

		int lSkillID = pkLevelUpData->_iID;
		kSkillLevels.push_back(lSkillID);
		lNextSkillID = pkLevelUpData->_lNextLevelSkillTypeID;
		pkLevelUpData = pkSkillLevelUp->GetEntryPtr(lNextSkillID);

	} while (pkLevelUpData != 0);
	kSkillLevels.push_back(lNextSkillID);

	return true;
}

//bool GameLogic::LC_SkillTooltipBase::_addGuildSkillToolTipResearchDecl( TooltipFieldVector& kFieldVector )
//{
//	CF_SkillExecuteList* pkSkillList = CF_SkillExecuteList::GetSingletonPtr();
//	if (NULL == pkSkillList)
//	{
//		return false;
//	}
//
//	CF_SkillExecuteList::DataEntry* pkSkillData = pkSkillList->GetEntryPtr( m_lSkillID );
//	if (NULL == pkSkillData)
//	{
//		return false;
//	}
//	unsigned short usFont = FONT_MIDDLE;
//	unsigned short usFontSize = FONTSIZE_MIDDLE;
//	int iFontColor = WHITE;
//
//	StringType kGuildSkillResearchDecl = pkSkillData->_kGuildSkillResearchDecl;
//	if ("" == kGuildSkillResearchDecl || "NULL" == kGuildSkillResearchDecl)
//	{
//		return false;
//	}
//	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_SKILL_GUILDSKILL_RESEARCHDECL, "门派技能研究需要说明", true, kGuildSkillResearchDecl, usFont, usFontSize, iFontColor, 0, 0));
//
//	return true;
//}
//
//bool GameLogic::LC_SkillTooltipBase::_addGuildSkillToolTipResearchState( TooltipFieldVector& kFieldVector )
//{
//	CF_SkillExecuteList* pkSkillList = CF_SkillExecuteList::GetSingletonPtr();
//	if (NULL == pkSkillList)
//	{
//		return false;
//	}
//	CF_SkillExecuteList::DataEntry* pkSkillData = pkSkillList->GetEntryPtr( m_lSkillID );
//	if (NULL == pkSkillData)
//	{
//		return false;
//	}
//
//	unsigned short usFont = FONT_MIDDLE;
//	unsigned short usFontSize = FONTSIZE_MIDDLE;
//	int iFontColor = RED;
//
//	StringType kGuildSkillResearchState = pkSkillData->_kGuildSkillResearchState;
//	if ("" == kGuildSkillResearchState || "NULL" == kGuildSkillResearchState)
//	{
//		return false;
//	}
//	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_SKILL_GUILDSKILL_RESEARCHDECL, "门派技能研究状态", true, kGuildSkillResearchState, usFont, usFontSize, iFontColor, 0, 0));
//
//	return true;
//}

