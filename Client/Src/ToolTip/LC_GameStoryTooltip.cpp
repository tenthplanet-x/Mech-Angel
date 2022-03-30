
#include "ClientPCH.h"
#include "LC_GameStoryTooltip.h"
#include "LM_LocaleManager.h"
#include "CF_GameStory.h"
#include "CF_GameStoryClientUI.h"

using namespace GameLogic;
using namespace CSVFile;

LC_GameStoryTooltip::LC_GameStoryTooltip(void)
: m_pkInfoEntry(NULL)
, m_pkGameStoryInfo(NULL)
, m_pStoryRaidInfoEntry(NULL)
, m_iTotalCost(0)
{
}

LC_GameStoryTooltip::~LC_GameStoryTooltip(void)
{
}

void LC_GameStoryTooltip::Init(int iGameStoryID, int iTotalCost, int iRaidInfoID, GameStoryInfo *pkGameStoryInfo)
{
	m_pkGameStoryInfo = pkGameStoryInfo;
	m_pkInfoEntry = SafeGetCSVFileDataEntryPtr<CF_GameStory>(iGameStoryID);
	m_pStoryRaidInfoEntry = SafeGetCSVFileDataEntryPtr<CF_GameStoryRaidList>(iRaidInfoID);
	m_iTotalCost = iTotalCost;
}

bool LC_GameStoryTooltip::BuildTooltip()
{
	//清除所有的tooltip字段
	TooltipFieldVector& kFieldVector = GetTooltipFields();
	kFieldVector.clear();
	
	//活动名称
	_addTooltipFieldGameStoryName(kFieldVector);
	//boss名称
	_addTooltipFieldBossDesc(kFieldVector);
	//boss等级
	_addTooltipFieldBossLevel(kFieldVector);
	//玩法说明
	_addTooltipFieldPlayRule(kFieldVector);
	//刷新时间
	_addTooltipFieldResetTime(kFieldVector);
	//刷新地图
	_addTooltipFieldResetMap(kFieldVector);
	//携带宝物
	_addTooltipFieldReward(kFieldVector);
	//驻守地图
	_addTooltipFieldMap(kFieldVector);
	//基本属性
	_addTooltipFieldBaseAttr(kFieldVector);
	//提示信息
	_addTooltipFieldGameStoryTip(kFieldVector);

	//格式化tooltip
	_formatTooltip(kFieldVector);

	return true;
}

bool LC_GameStoryTooltip::_addTooltipFieldGameStoryName(TooltipFieldVector& kFieldVector)
{
	StringType * pkContent = NULL;

	if (m_pStoryRaidInfoEntry != NULL)
		pkContent = &m_pStoryRaidInfoEntry->_kName;
	else if (m_pkGameStoryInfo != NULL)
		pkContent = &m_pkGameStoryInfo->m_kClientShowName;
	else
		return false;

	unsigned short usFont = FONT_LARGE;
	unsigned short usFontSize = FONTSIZE_LARGE;
	int iFontColor = GOLDENROD;

	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_GAMESTORY_NAME, GET_MSG(LC_GAMESTORYTOOLTIP_CPP_001), true, *pkContent, usFont, usFontSize, iFontColor, 0, 0));

	return true;
}

bool LC_GameStoryTooltip::_addTooltipFieldBossDesc(TooltipFieldVector& kFieldVector)
{
	if (NULL == m_pStoryRaidInfoEntry)
		return false;

	const StringType & kContent = m_pStoryRaidInfoEntry->_kBossDesc;
	if (kContent == "NULL")
		return false;

	unsigned short usFont = FONT_LARGE;
	unsigned short usFontSize = FONTSIZE_LARGE;
	int iFontColor = PURPLE;


	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_GAMESTORY_BOOS_NAME, GET_MSG(LC_GAMESTORYTOOLTIP_CPP_002), true, kContent, usFont, usFontSize, iFontColor, 0, 0));

	return true;
}

bool LC_GameStoryTooltip::_addTooltipFieldBossLevel(TooltipFieldVector& kFieldVector)
{
	if (NULL == m_pStoryRaidInfoEntry)
		return false;
	if (m_pStoryRaidInfoEntry->_level <= 0)
		return false;

	unsigned short usFont = FONT_LARGE;
	unsigned short usFontSize = FONTSIZE_LARGE;
	int iFontColor = GREEN;

	StringStreamType sStream;
	sStream << m_pStoryRaidInfoEntry->_level << GET_MSG(LC_GAMESTORYTOOLTIP_CPP_003);
	
	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_GAMESTORY_BOOS_LEVEL, GET_MSG(LC_GAMESTORYTOOLTIP_CPP_004), true, sStream.str(), usFont, usFontSize, iFontColor, 0, 0));

	return true;
}

bool LC_GameStoryTooltip::_addTooltipFieldPlayRuleTitle(TooltipFieldVector& kFieldVector)
{
	if (m_pStoryRaidInfoEntry != NULL)
		return false;

	unsigned short usFont = FONT_LARGE;
	unsigned short usFontSize = FONTSIZE_LARGE;
	int iFontColor = GOLD;

	StringType kContent = GET_MSG(LC_GAMESTORYTOOLTIP_CPP_005);
	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_GAMESTORY_PLAY_RULE_TITLE, GET_MSG(LC_GAMESTORYTOOLTIP_CPP_006), true, kContent, usFont, usFontSize, iFontColor, 0, 0));

	return true;
}

bool LC_GameStoryTooltip::_addTooltipFieldPlayRule(TooltipFieldVector& kFieldVector)
{
	if (m_pStoryRaidInfoEntry != NULL)
		return false;
	if(NULL == m_pkGameStoryInfo)
	{
		return false;
	}

	const StringType & kContent = m_pkGameStoryInfo->m_kClientUIPlayRule;
	if("NULL" == kContent)
	{
		return false;
	}
	
	_addTooltipFieldPlayRuleTitle(kFieldVector);

	unsigned short usFont = FONT_LARGE;
	unsigned short usFontSize = FONTSIZE_LARGE;
	int iFontColor = WHITE;


	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_GAMESTORY_PLAY_RULE, GET_MSG(LC_GAMESTORYTOOLTIP_CPP_007), true, kContent, usFont, usFontSize, iFontColor, 0, 0));

	return true;
}

bool LC_GameStoryTooltip::_addTooltipFieldRewardTitle(TooltipFieldVector& kFieldVector)
{
	unsigned short usFont = FONT_LARGE;
	unsigned short usFontSize = FONTSIZE_LARGE;
	int iFontColor = GOLD;

	StringType kContent = GET_MSG(LC_GAMESTORYTOOLTIP_CPP_008);
	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_GAMESTORY_REWARD_TITLE, GET_MSG(LC_GAMESTORYTOOLTIP_CPP_009), true, kContent, usFont, usFontSize, iFontColor, 0, 0));

	return true;
}

bool LC_GameStoryTooltip::_addTooltipFieldReward(TooltipFieldVector& kFieldVector)
{
	StringType * pkContent = NULL;

	if (m_pStoryRaidInfoEntry)
		pkContent = &m_pStoryRaidInfoEntry->_kItemDrops;
	else if(m_pkGameStoryInfo)
		pkContent = &m_pkGameStoryInfo->m_kClientUITooltipReward;
	else
		return false;

	if(pkContent->empty() || "NULL" == (*pkContent))
	{
		return false;
	}

	_addTooltipFieldRewardTitle(kFieldVector);

	unsigned short usFont = FONT_LARGE;
	unsigned short usFontSize = FONTSIZE_LARGE;
	int iFontColor = WHITE;

	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_GAMESTORY_REWARD, GET_MSG(LC_GAMESTORYTOOLTIP_CPP_010), true, *pkContent, usFont, usFontSize, iFontColor, 0, 0));

	return true;
}

bool LC_GameStoryTooltip::_addTooltipFieldResetTimeTitle(TooltipFieldVector& kFieldVector)
{
	unsigned short usFont = FONT_LARGE;
	unsigned short usFontSize = FONTSIZE_LARGE;
	int iFontColor = GOLD;

	StringType kContent = GET_MSG(LC_GAMESTORYTOOLTIP_CPP_011);
	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_GAMESTORY_RESET_TIME_TITLE, GET_MSG(LC_GAMESTORYTOOLTIP_CPP_012), true, kContent, usFont, usFontSize, iFontColor, 0, 0));

	return true;
}

bool LC_GameStoryTooltip::_addTooltipFieldResetTime(TooltipFieldVector& kFieldVector)
{
	if (m_pStoryRaidInfoEntry != NULL)
		return false;

	if(NULL == m_pkGameStoryInfo)
	{
		return false;
	}

	const StringType & kContent = m_pkGameStoryInfo->m_kClientUIResetTime;
	if("NULL" == kContent)
	{
		return false;
	}

	_addTooltipFieldResetTimeTitle(kFieldVector);

	unsigned short usFont = FONT_LARGE;
	unsigned short usFontSize = FONTSIZE_LARGE;
	int iFontColor = WHITE;

	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_GAMESTORY_RESET_TIME, GET_MSG(LC_GAMESTORYTOOLTIP_CPP_013), true, kContent, usFont, usFontSize, iFontColor, 0, 0));

	return true;
}

bool LC_GameStoryTooltip::_addTooltipFieldResetMapTitle(TooltipFieldVector& kFieldVector)
{
	unsigned short usFont = FONT_LARGE;
	unsigned short usFontSize = FONTSIZE_LARGE;
	int iFontColor = GOLD;

	StringType kContent = GET_MSG(LC_GAMESTORYTOOLTIP_CPP_014);
	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_GAMESTORY_RESET_MAP_TITLE, GET_MSG(LC_GAMESTORYTOOLTIP_CPP_015), true, kContent, usFont, usFontSize, iFontColor, 0, 0));

	return true;
}

bool LC_GameStoryTooltip::_addTooltipFieldResetMap(TooltipFieldVector& kFieldVector)
{
	if (m_pStoryRaidInfoEntry != NULL)
		return false;

	if(NULL == m_pkGameStoryInfo)
	{
		return false;
	}

	const StringType & kContent = m_pkGameStoryInfo->m_kClientUIResetMap;
	if("NULL" == kContent)
	{
		return false;
	}

	_addTooltipFieldResetMapTitle(kFieldVector);

	unsigned short usFont = FONT_LARGE;
	unsigned short usFontSize = FONTSIZE_LARGE;
	int iFontColor = WHITE;

	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_GAMESTORY_RESET_MAP, GET_MSG(LC_GAMESTORYTOOLTIP_CPP_016), true, kContent, usFont, usFontSize, iFontColor, 0, 0));

	return true;
}

bool LC_GameStoryTooltip::_addTooltipFieldGameStoryTip(TooltipFieldVector& kFieldVector)
{
	StringType kContent;
	if (NULL != m_pStoryRaidInfoEntry)
	{
		//明教禁地BOSS花费为动态值
		if (m_iTotalCost <= 0)
		{
			kContent = GET_MSG(LC_GAMESTORYTOOLTIP_CPP_017);
		}
		else
		{
			StringType coststr = Int2String(m_iTotalCost);
			UT_BaseStringUtility::ReplaceAll(m_pStoryRaidInfoEntry->_kCostDesc, "[NUMBER]", coststr, kContent);
		}
	}
	else if (NULL != m_pkGameStoryInfo)
		kContent = m_pkGameStoryInfo->m_kClientUITip;
	else
		 return false;

	if(kContent.empty() || "NULL" == kContent)
	{
		return false;
	}

	unsigned short usFont = FONT_LARGE;
	unsigned short usFontSize = FONTSIZE_LARGE;
	int iFontColor = BLUE;

	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_GAMESTORY_TIP, GET_MSG(LC_GAMESTORYTOOLTIP_CPP_018), true, kContent, usFont, usFontSize, iFontColor, 0, 0));

	return true;
}

bool LC_GameStoryTooltip::_addTooltipFieldMapTitle(TooltipFieldVector& kFieldVector)
{
	unsigned short usFont = FONT_LARGE;
	unsigned short usFontSize = FONTSIZE_LARGE;
	int iFontColor = GOLD;

	StringType kContent = GET_MSG(LC_GAMESTORYTOOLTIP_CPP_019);

	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_GAMESTORY_BOOS_MAP_TITLE, GET_MSG(LC_GAMESTORYTOOLTIP_CPP_018), true, kContent, usFont, usFontSize, iFontColor, 0, 0));

	return true;
}

bool LC_GameStoryTooltip::_addTooltipFieldMap(TooltipFieldVector& kFieldVector)
{
	if (NULL == m_pStoryRaidInfoEntry)
		return false;

	const StringType & kContent = m_pStoryRaidInfoEntry->_kMapDesc;
	if (kContent.empty() || "NULL" == kContent)
		return false;

	_addTooltipFieldMapTitle(kFieldVector);

	unsigned short usFont = FONT_LARGE;
	unsigned short usFontSize = FONTSIZE_LARGE;
	int iFontColor = WHITE;

	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_GAMESTORY_BOOS_MAP, GET_MSG(LC_GAMESTORYTOOLTIP_CPP_018), true, kContent, usFont, usFontSize, iFontColor, 0, 0));

	return true;
}

bool LC_GameStoryTooltip::_addTooltipFieldBaseAttrTitle(TooltipFieldVector& kFieldVector)
{
	unsigned short usFont = FONT_LARGE;
	unsigned short usFontSize = FONTSIZE_LARGE;
	int iFontColor = GOLD;

	StringType kContent = GET_MSG(LC_GAMESTORYTOOLTIP_CPP_020);

	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_GAMESTORY_BOOS_BASE_ATTR_TITLE, GET_MSG(LC_GAMESTORYTOOLTIP_CPP_018), true, kContent, usFont, usFontSize, iFontColor, 0, 0));

	return true;
}

bool LC_GameStoryTooltip::_addTooltipFieldBaseAttr(TooltipFieldVector& kFieldVector)
{
	if (NULL == m_pStoryRaidInfoEntry)
		return false;

	_addTooltipFieldBaseAttrTitle(kFieldVector);

	const StringType & kContent = m_pStoryRaidInfoEntry->_kBassAttr;

	unsigned short usFont = FONT_LARGE;
	unsigned short usFontSize = FONTSIZE_LARGE;
	int iFontColor = WHITE;

	kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_GAMESTORY_BOOS_BASE_ATTR, GET_MSG(LC_GAMESTORYTOOLTIP_CPP_018), true, kContent, usFont, usFontSize, iFontColor, 0, 0));

	return true;
}
