
#include "ClientPCH.h"
#include "LC_SkillStateTooltip.h"
#include "LM_LocaleManager.h"
#include "CF_SkillStateList.h"

using namespace GameLogic;
using namespace CSVFile;

LC_SkillStateTooltip::LC_SkillStateTooltip(void)
: m_lSkillStateID(0)
{
}

LC_SkillStateTooltip::~LC_SkillStateTooltip(void)
{
}

void LC_SkillStateTooltip::Init( int lSkillStateID, LC_PlayerBase* pkObserver )
{
	m_lSkillStateID = lSkillStateID;
	SetObserver(pkObserver);
}

bool LC_SkillStateTooltip::BuildTooltip()
{
	TooltipFieldVector& kFieldVector = GetTooltipFields();
	kFieldVector.clear();

	//状态名
	_addTooltipFieldStateName(kFieldVector);

	//使用效果
	_addTooltipFieldUseResult(kFieldVector);

	//格式化tooltip
	_formatTooltip(kFieldVector);

	return true;
}

bool LC_SkillStateTooltip::_addTooltipFieldStateName(TooltipFieldVector& kFieldVector)
{
	CF_SkillStateList* pkStateList = CF_SkillStateList::GetSingletonPtr();
	CF_SkillStateList::DataEntry* pkStateData = pkStateList->GetEntryPtr( m_lSkillStateID );
	if (NULL == pkStateData)
	{
		return false;
	}

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = GREEN;

	StringStreamType sStream("");
	sStream << pkStateData->_kName << "\n";

	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_SKILL_NAME, GET_MSG(LC_SKILLSTATETOOLTIP_CPP_001), true, sStream.str(), usFont, usFontSize, iFontColor, 0, 0, false));

	return true;
}

bool LC_SkillStateTooltip::_addTooltipFieldUseResult( TooltipFieldVector& kFieldVector )
{
	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = WHITE;

// 	int lRecordID = pkStateData->_iID;
// 	if (lRecordID <= 0)
// 	{
// 		return false;
// 	}
	StringType kUseResult = "";
	if ( !(LC_TooltipBase::_getUseResult<CSVFile::CF_SkillStateList>( m_lSkillStateID , kUseResult)) )
	{
		return false;
	}
	bool bNewLine = false;
	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_SKILLSTATE_USE_RESULT, GET_MSG(LC_SKILLSTATETOOLTIP_CPP_002), true, kUseResult, usFont, usFontSize, iFontColor, 0, 0, bNewLine));

	return true;
}



