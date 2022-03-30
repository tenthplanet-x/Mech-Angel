
#include "ClientPCH.h"
#include "LM_LocaleManager.h"
#include "LC_MeditateTooltip.h"
#include "LC_TooltipDirector.h"
#include "LC_PlayerBase.h"

using namespace GameLogic;

//------------------------------------------------------------
LC_MeditateTooltip::LC_MeditateTooltip(void)
{
}
//------------------------------------------------------------
LC_MeditateTooltip::~LC_MeditateTooltip(void)
{
}
//------------------------------------------------------------
bool LC_MeditateTooltip::BuildTooltip()
{
	LC_ActorBase *pkObserver = GetObserver();
	if (NULL == pkObserver || !IS_PLAYER_ID(pkObserver->GetID()))
	{
		return false;
	}

	TooltipFieldVector& kFieldVector = GetTooltipFields();
	kFieldVector.clear();

	_addTooltipFieldTitle(kFieldVector);
	_addTooltipFieldTime(kFieldVector);
	//_addTooltipFieldSpRecoverRate(kFieldVector);
	//_addTooltipFieldSkillPointLevel(kFieldVector);
	_addTooltipFieldExpGained(kFieldVector);
	_addTooltipFieldSkillPointGained(kFieldVector);
	//_addTooltipFieldTotalSkillPoint(kFieldVector);

	return true;
}
//------------------------------------------------------------
bool LC_MeditateTooltip::_addTooltipFieldTitle(TooltipFieldVector& kFieldVector)
{
	
	LC_PlayerBase *pkPlayer = static_cast<LC_PlayerBase*>(GetObserver());
	if (NULL == pkPlayer)
	{
		return false;
	}

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONT_MIDDLE;
	int iFontColor = WHITE;

	StringStreamType kStrStream;
	kStrStream << GET_MSG(LC_MEDITATETOOLTIP_CPP_001);

	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_MEDITATE_TITLE, GET_MSG(LC_MEDITATETOOLTIP_CPP_003), true, kStrStream.str(), usFont, usFontSize, iFontColor, 0, 0));

	return true;
}
//------------------------------------------------------------
bool LC_MeditateTooltip::_addTooltipFieldTime(TooltipFieldVector &kFieldVector)
{
	LC_PlayerBase *pkPlayer = static_cast<LC_PlayerBase*>(GetObserver());
	if (NULL == pkPlayer)
	{
		return false;
	}

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = BROWN;

	StringStreamType kStrStream;
	kStrStream << GET_MSG(LC_MEDITATETOOLTIP_CPP_004);

	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_MEDITATE_TIME, GET_MSG(LC_MEDITATETOOLTIP_CPP_005), true, kStrStream.str(), usFont, usFontSize, iFontColor, 0, 0, false));

	return true;
}
//------------------------------------------------------------
bool LC_MeditateTooltip::_addTooltipFieldSpRecoverRate(TooltipFieldVector &kFieldVector)
{
	LC_PlayerBase *pkPlayer = static_cast<LC_PlayerBase*>(GetObserver());
	if (NULL == pkPlayer)
	{
		return false;
	}

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = BROWN;

	StringStreamType kStrStream;
	kStrStream << GET_MSG(LC_MEDITATETOOLTIP_CPP_006);

	int iSpRecoverRate = pkPlayer->GetSpRecoverRate();
	kStrStream << "<color val=" << GREEN << ">" << iSpRecoverRate << GET_MSG(LC_MEDITATETOOLTIP_CPP_007);

	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_MEDITATE_TIME, GET_MSG(LC_MEDITATETOOLTIP_CPP_008), true, kStrStream.str(), usFont, usFontSize, iFontColor, 0, 0, false));

	return true;
}
//------------------------------------------------------------
bool LC_MeditateTooltip::_addTooltipFieldSkillPointLevel(TooltipFieldVector &kFieldVector)
{
	LC_PlayerBase *pkPlayer = static_cast<LC_PlayerBase*>(GetObserver());
	if (NULL == pkPlayer)
	{
		return false;
	}

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = WHITE;

	StringStreamType kStrStream;
	kStrStream << GET_MSG(LC_MEDITATETOOLTIP_CPP_009) << NUM_TO_CHN(pkPlayer->GetSitRecoverLevel()) << GET_MSG(LC_MEDITATETOOLTIP_CPP_010);
	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_MEDITATE_SKILLPOINT_LEVEL, GET_MSG(LC_MEDITATETOOLTIP_CPP_011), true, kStrStream.str(), usFont, usFontSize, iFontColor, 0, 0));

	return true;
}
//------------------------------------------------------------
bool LC_MeditateTooltip::_addTooltipFieldSkillPointGained(TooltipFieldVector &kFieldVector)
{
	LC_PlayerBase *pkPlayer = static_cast<LC_PlayerBase*>(GetObserver());
	if (NULL == pkPlayer)
	{
		return false;
	}

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = BROWN;

	attr_value_type iRecoverSp = pkPlayer->GetCurrentSitRecoverSkillPoint();
	attr_value_type iRecoverVipSp = pkPlayer->GetCurrentSitRecoverVipSkillPoint();
	StringStreamType kStrStream;
	kStrStream << GET_MSG(LC_MEDITATETOOLTIP_CPP_012) << "<color val=0xff0096ff>" << iRecoverSp + iRecoverVipSp;
	if(iRecoverVipSp > 0)
	{
		kStrStream << GET_MSG(LC_MEDITATETOOLTIP_CPP_013) << iRecoverVipSp << "£©";
	}

	kStrStream << "\n</color>";

	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_MEDITATE_SKILLPOINT_GAINED, GET_MSG(LC_MEDITATETOOLTIP_CPP_014), true, kStrStream.str(), usFont, usFontSize, iFontColor, 0, 0, false));

	return true;
}
//------------------------------------------------------------
bool LC_MeditateTooltip::_addTooltipFieldTotalSkillPoint(TooltipFieldVector &kFieldVector)
{
	LC_PlayerBase *pkPlayer = static_cast<LC_PlayerBase*>(GetObserver());
	if (NULL == pkPlayer)
	{
		return false;
	}

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = BROWN;

	StringStreamType kStrStream;
	kStrStream << GET_MSG(LC_MEDITATETOOLTIP_CPP_015) << "<color val=0xff0096ff>" << pkPlayer->GetSkillLevelupPoints() << "</color>";

	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_MEDITATE_TOTAL_SKILLPOINT, GET_MSG(LC_MEDITATETOOLTIP_CPP_016), true, kStrStream.str(), usFont, usFontSize, iFontColor, 0, 0));

	return true;
}
//------------------------------------------------------------
bool LC_MeditateTooltip::_addTooltipFieldExpGained(TooltipFieldVector &kFieldVector)
{
	LC_PlayerBase *pkPlayer = static_cast<LC_PlayerBase*>(GetObserver());
	if (NULL == pkPlayer)
	{
		return false;
	}

	unsigned short usFont = FONT_MIDDLE;
	unsigned short usFontSize = FONTSIZE_MIDDLE;
	int iFontColor = BROWN;

	int64_t iRecoverSp = pkPlayer->GetCurrentSitRecoverExp();
	int64_t iRecoverVipSp = pkPlayer->GetCurrentSitRecoverExpVip();
	StringStreamType kStrStream;
	kStrStream << GET_MSG(LC_MEDITATETOOLTIP_CPP_017) << "<color val=0xff0096ff>" << iRecoverSp + iRecoverVipSp;
	if(iRecoverVipSp > 0)
	{
		kStrStream << GET_MSG(LC_MEDITATETOOLTIP_CPP_013) << iRecoverVipSp << "£©";
	}

	kStrStream << "\n</color>";

	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_MEDITATE_EXP_GAINED, GET_MSG(LC_MEDITATETOOLTIP_CPP_018), true, kStrStream.str(), usFont, usFontSize, iFontColor, 0, 0, false));

	return true;
}
//------------------------------------------------------------
StringType LC_MeditateTooltip::_formatTimeStr(float fTime)
{	
	StringType sRslt;
	char cTime[5];

	sprintf(cTime, "%02d", (int)(fTime / 3600));
	sRslt += StringType(cTime);

	sprintf(cTime, ":%02d", (((int)fTime % 3600) / 60));
	sRslt += StringType(cTime);

	sprintf(cTime, ":%02d", ((int)fTime % 60));
	sRslt += StringType(cTime);
	
	return sRslt;
}
