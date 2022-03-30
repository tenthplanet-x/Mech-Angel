
#include "ClientPCH.h"
#include "LM_LocaleManager.h"
#include "LC_SkillDetail.h"
#include "LC_SkillTooltipBase.h"
#include "LC_PlayerBase.h"
#include "LC_SkillAsset.h"

#include "CF_SkillLevelUp.h"

using namespace GameLogic;
using namespace CSVFile;

//------------------------------------------------------------------------------
void LC_SkillInfoEntry::Reset()
{
	m_iTypeID = 0;
	m_iLevel = 0;
	m_iTopLevel = -1;
	m_kSkillName = "";
	m_kIconPath = "";
	m_kSkillAttri1 = "";
	m_kSkillAttri2 = "";
	m_iPosNegType = 0;
	m_iRangeType = 0;
	m_iCategory = 0;
	m_kSkillDescription = "";
	m_iSP = 0;
	m_iMoney = 0;
	m_kUseResult = "";
	m_iActorLevel = 0;
	m_iActorStarLevel = 0;
	m_iMPValueCost = 0;
	m_fExecuteTime = 0;
	m_fCoolDownTime = 0;
	m_bIsLearned = false;
	m_bIsBreakThrough = false;
	m_iStarLevel = 0;
}

LC_SkillDetail::LC_SkillDetail(void)
{
}

LC_SkillDetail::~LC_SkillDetail(void)
{
}

StringType LC_SkillDetail::GetSkillUseResults(int lSkillID)
{
	SetSkillID(lSkillID);
	TooltipFieldVector kField;
	if ( !_addTooltipFieldUseResult(kField) )
	{
		return "";
	}
	return kField[0].GetContent();
}

StringType LC_SkillDetail::GetSkillUseResults2(int lSkillID)
{
	StringType kUseResult;
	LC_TooltipBase::_getUseResult<CSVFile::CF_SkillSpecialAttrib>(lSkillID, kUseResult, true);
	return kUseResult;
}

bool LC_SkillDetail::_addTooltipFieldObserverLevel( std::vector<int>& kSkillLevels, TooltipFieldVector& kFieldVector )
{
	int lCurTopSkillID = 0;
	std::vector<int>::const_iterator iter;
	LC_ActorBase* pkObserver = GetObserver();
	if ( NULL == pkObserver || IS_PLAYER_ID(pkObserver->GetID()) )
	{
		return false;
	}
	for (iter = kSkillLevels.begin(); iter != kSkillLevels.end(); ++iter)
	{
		int lSkillID = *iter;
		LC_SkillAsset* pkSkillAsset = ((LC_PlayerBase*)pkObserver)->GetSkillAsset();
		//如果技能已经学会
		if ( pkSkillAsset->IsLearnedSkill(lSkillID) )
		{
			lCurTopSkillID = lSkillID;
		}
	}

	CF_SkillSpecialAttrib::DataEntry* pkSkillData = SafeGetCSVFileDataEntryPtr<CF_SkillSpecialAttrib>(lCurTopSkillID);
	if (0 == pkSkillData)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, Utility::LOG_SWITCH_ERROR, "Can't find %d in SkillLevelUp!", lCurTopSkillID);
		return false;
	}

	int lCurTopLevel = pkSkillData->_lCurLevel;
	StringType kCurTopLevel = _numArabToChinese(lCurTopLevel);
	StringStreamType kStringStream;
	kStringStream << GET_MSG(LC_SKILLDETAIL_CPP_010) << kCurTopLevel << GET_MSG(LC_MEDITATETOOLTIP_CPP_010);

	unsigned short usFont = FONT_LARGE;
	unsigned short usFontSize = FONTSIZE_LARGE;
	int iFontColor = WHITE;
	bool bNewLine = false;
	kFieldVector.push_back( LC_TooltipField(TOOLTIP_FIELD_SKILL_OBSERVER_LEVEL, GET_MSG(LC_SKILLDETAIL_CPP_011), true, kStringStream.str(), usFont, usFontSize, iFontColor, 0, 0, bNewLine) );

	return true;
}

bool LC_SkillDetail::_addTooltipFieldUpdateRequest( TooltipFieldVector& kFieldVector )
{
	CF_SkillLevelUp* pkSkillLevelUp = CF_SkillLevelUp::GetSingletonPtr();
	CF_SkillLevelUp::DataEntry* pkLevelUpData = pkSkillLevelUp->GetEntryPtr( GetSkillID() );
	if (0 == pkLevelUpData)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, Utility::LOG_SWITCH_ERROR, "Can't find %d in SkillLevelUp!", GetSkillID());
		return false;
	}

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = WHITE;

	StringStreamType kStringStream;
	int lNeedSP = pkLevelUpData->_lNeedSP;
	kStringStream << CTSTR_SKILLPOINT << ":"  << lNeedSP;
	kFieldVector.push_back( LC_TooltipField(TOOLTIP_FIELD_SKILL_UPDATE_NEEDEXP, GET_MSG(LC_SKILLDETAIL_CPP_012), true, kStringStream.str(), usFont, usFontSize, iFontColor, 0, 0) );

	int lNeedLevel = pkLevelUpData->_lNeedLevel;
	kStringStream.str("");
	kStringStream << CTSTR_LEVEL << ":" << lNeedLevel;
	bool bNewLine = false;
	kFieldVector.push_back( LC_TooltipField(TOOLTIP_FIELD_SKILL_UPDATE_NEEDLEVEL, GET_MSG(LC_SKILLDETAIL_CPP_013), true, kStringStream.str(), usFont, usFontSize, iFontColor, 0, 0, bNewLine) );

	int lNeedCash = pkLevelUpData->_lNeedCash;
	kStringStream.str("");
	kStringStream << CTSTR_GOLD << ":" << lNeedCash;
	kFieldVector.push_back( LC_TooltipField(TOOLTIP_FIELD_SKILL_UPDATE_NEEDCASH, GET_MSG(LC_SKILLDETAIL_CPP_014), true, kStringStream.str(), usFont, usFontSize, iFontColor, 0, 0) );

	return true;
}

bool LC_SkillDetail::_addTooltipFieldAchieve( TooltipFieldVector& kFieldVector )
{
	return false;
}

StringType LC_SkillDetail::_getSkillMainName( const StringType& kSkillName )
{
	//技能名称以技能等级（数字）结尾，剪除技能等级
	StringType::size_type stPos = kSkillName.find_last_not_of("1234567890");
	return kSkillName.substr(0, stPos + 1);
}



