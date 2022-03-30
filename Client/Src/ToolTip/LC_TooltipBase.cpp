
#include "ClientPCH.h"
#include "LM_LocaleManager.h"
#include "LC_TooltipBase.h"
#include "LC_PackStruct.h"
#include "LC_ActorBase.h"

#include "CF_ItemList.h"
#include "CF_ItemPropertyFunc.h"
#include "LC_ClientLogicManager.h"
#include "LC_ClientWorldManager.h"
#include "LC_ClientGamePlayer.h"
#include "SC_ClientScriptInterface.h"


using namespace GameLogic;
using namespace CSVFile;
using namespace Utility;

LC_TooltipField::LC_TooltipField( void )
: m_kType(TOOLTIP_FIELD_ITEM_ICON)				//字段类型
, m_kCaption("")								//
, m_bEnable(false)								//是否显示该字段
, m_usFont(0)									//字体
, m_usFontSize(0)								//字体尺寸
, m_iFontColor(0)								//字体颜色
, m_kContent("")								//字段内容
, m_usLengthLimit(0)							//字段长度限制
, m_usEntryNumLimit(0)							//字段条目限制
, m_bNewLine(false)
{
}

LC_TooltipField::LC_TooltipField( TooltipFieldType kFieldType, const StringType& kCaption, bool bEnable, const StringType& kContent, 
	unsigned short usFont, unsigned short usFontSize, int iFontColor, 
	unsigned short usLengthLimit, unsigned short usEntryNumLimit, bool bNewLine )
: m_kType(kFieldType)							//字段类型
, m_kCaption(kCaption)							//
, m_bEnable(bEnable)							//是否显示该字段
, m_kContent(kContent)							//字段内容
, m_usFont(usFont)								//字体
, m_usFontSize(usFontSize)						//字体尺寸
, m_iFontColor(iFontColor)						//字体颜色
, m_usLengthLimit(usLengthLimit)				//字段长度限制
, m_usEntryNumLimit(usEntryNumLimit)			//字段条目限制
, m_bNewLine(bNewLine)							//是否换行
{
}

LC_TooltipField::~LC_TooltipField( void )
{

}

LC_TooltipBase::LC_TooltipBase(void)
: m_pkObserver(NULL)
{
	m_kTooltipFieldVector.clear();
}

LC_TooltipBase::~LC_TooltipBase(void)
{
}

void LC_TooltipBase::Init( LC_PlayerBase* pkObserver )
{
	m_pkObserver = pkObserver;
}

bool LC_TooltipBase::GetSkillStateUseResult(int lRecordID, StringType &kUseResult)
{
	return _getUseResult<CF_SkillSpecialAttrib>(lRecordID, kUseResult);
}

StringType LC_TooltipBase::_numArabToChinese( int lArabNum )
{
	const char * kChineseNum[9] = {GET_MSG(GUILDCOMMDEF_H_005), GET_MSG(GUILDCOMMDEF_H_006), GET_MSG(GUILDCOMMDEF_H_007), GET_MSG(GUILDCOMMDEF_H_008), GET_MSG(GUILDCOMMDEF_H_009), GET_MSG(GUILDCOMMDEF_H_010), GET_MSG(GUILDCOMMDEF_H_011), GET_MSG(GUILDCOMMDEF_H_012), GET_MSG(GUILDCOMMDEF_H_013)};

	StringType kResult;

	StringStreamType kStringStream;
	kStringStream << lArabNum;
	StringType kArabNum = kStringStream.str();
	int iLength = kArabNum.length();
	switch (iLength)
	{
	case 1:
		kResult = kChineseNum[lArabNum - 1];				//个位数
		break;
	case 2:
		{
			int lMedian = atol( kArabNum.substr(0, 1).c_str() );
			int lSingle = atol( kArabNum.substr(1, 1).c_str() );
			if (lMedian >= 1 && lMedian <= 9)
			{
				kResult = kChineseNum[lMedian - 1];
				kResult.append(GET_MSG(GUILDCOMMDEF_H_014));
				if (lSingle >= 1 && lSingle <= 9)
				{
					kResult.append(kChineseNum[lSingle - 1]);
				}
			}
			else
			{
				GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "解析十进制错误，十位[%d]个位[%d]", lMedian, lSingle);
			}
		}
		break;
	case 3:
		{
			int lHundred = atol( kArabNum.substr(0, 1).c_str() );
			int lMedian = atol( kArabNum.substr(1, 1).c_str() );
			int lSingle = atol( kArabNum.substr(2, 1).c_str() );
			
			if (lHundred >= 1 && lHundred <= 9)
			{
				kResult = kChineseNum[lHundred - 1];
				kResult.append(GET_MSG(LC_TOOLTIPBASE_CPP_001));

				if (0 == lMedian)
				{
					if (lSingle >= 1 && lSingle <= 9)
					{
						kResult.append("0");
						kResult.append(kChineseNum[lSingle - 1]);
					}
				}
				else if (lMedian >= 1 && lMedian <= 9)
				{
					kResult.append(kChineseNum[lMedian - 1]);
					kResult.append(GET_MSG(GUILDCOMMDEF_H_014));
					if (lSingle >= 1 && lSingle <= 9)
					{
						kResult.append(kChineseNum[lSingle - 1]);
					}
				}
				else
				{
					GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "解析十进制错误，百位[%d]十位[%d]个位[%d]", lHundred, lMedian, lSingle);
				}
			}
			else
			{
				GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "解析十进制错误，百位[%d]十位[%d]个位[%d]", lHundred, lMedian, lSingle);
			}
		}
		break;
	}

	return kResult;
}

int LC_TooltipBase::_variable2Value(const StringType& kMemberName, int lRecordID)
{
	int fParamResult = -1;
	//坐骑技能状态特殊处理
	if("VLevel" == kMemberName)
	{
		LC_PlayerBase* pkPlayer = (LC_PlayerBase*)GetObserver();
		if(NULL != pkPlayer)
		{
			fParamResult = pkPlayer->GetVehicleCurrentLevel();
		}
	}
	else if ("VCurStateLevel" == kMemberName)
	{
		LC_PlayerBase* pkPlayer = (LC_PlayerBase*)GetObserver();
		LC_ActorStateMap* stateMap = pkPlayer->GetStateMap();
		LC_ActorStateEntry* pk = stateMap->GetStateEntryByID((int)lRecordID);
		if (pk)
		{
			fParamResult = pk->GetOverlapCount();
		}
	}
	else if ("VLevelSpeedDelta" == kMemberName)
	{
		LC_PlayerBase* pkPlayer = (LC_PlayerBase*)GetObserver();
		if (pkPlayer)
			fParamResult = pkPlayer->GetVehicleCurrentSpeedDelta();
	}
	else if ("GuildWarExp" == kMemberName)
	{
		LC_PlayerBase* pkPlayer = (LC_PlayerBase*)GetObserver();
		if (pkPlayer && pkPlayer->GetGuildWarAssert())
		{
			LC_GuildWarAssert* asset = pkPlayer->GetGuildWarAssert();
			//fParamResult = int((float(pkPlayer->GetLevel()) * 5.0f + 20.0f) * float(asset->GetMultiple()) / 10.0f * 12.0f);
			fParamResult = int((float(pkPlayer->GetCommanderLevel()) * 5.0f + 20.0f) * float(asset->GetMultiple()) / 10.0f * 12.0f);
		}
	}
	else if ( "basedamage" == kMemberName )		//dot伤害值特殊处理
	{
		LC_PlayerBase* pkPlayer = (LC_PlayerBase*)GetObserver();
		if (pkPlayer && pkPlayer->GetGuildWarAssert())
		{
			LC_ActorStateMap* pkStateMap = pkPlayer->GetStateMap();
			for(int i = 0 ;i < MAX_DEBUFF_STATE_SIZE; i++)
			{
				LC_ActorStateEntry* entry = pkStateMap->GetStateEntryInDebuffSlot(i);
				if (entry)
				{
					if (entry->GetTypeID() == lRecordID)
					{
						object_id_type iid = entry->GetAttacherID();
						LC_ClientWorldManager* pkWorldManager = CLIENT_GET_WORLD_MANAGER();
						LC_ActorBase* pkTarget = (LC_ActorBase*)pkWorldManager->FindObject(iid);
						if (pkTarget)
						{
							LC_ActorAttributeMap* attrMap =  pkTarget->GetAttributeMap();
							fParamResult = attrMap->GetAttribute(ATT_MAX_ATK);;
						}
					}
				}
			}	
		}
	}
	else if ( kMemberName.find("CurMasterGuild") != StringType::npos )
	{
		LC_PlayerBase* pkPlayer = (LC_PlayerBase*)GetObserver();
		LC_ActorStateMap* pkMap = pkPlayer->GetStateMap();
		if ( pkMap )
		{
			LC_ActorStateEntry* entry = pkMap->GetStateEntryByID(lRecordID);
			if ( entry )
			{
				LC_ClientGamePlayer* pkGamePlayer = (CLIENT_GET_LOGIC_MANAGER())->GetOperationPlayer();
				SC_ClientSkillInterface *pkClientSkillInterface = SC_ClientScriptInterface::GetSingletonPtr()->GetSkillInterface();
				float fr = 0;
				int64_t selfScore = entry->m_rextendData.getStateExtend(MBPI_MASTER_SELF);
				int WorldLevel = entry->m_rextendData.getStateExtend(MBPI_MASTER_WORLD_LEVEL);
				float param1 = pkClientSkillInterface->CalcuteMultiply(WorldLevel, entry->m_rextendData.getStateExtend(1));
				float param2 = pkClientSkillInterface->CalcuteMasterPosCorrect(1);
				fr = (float)selfScore * param1 * param2;

				param1 = pkClientSkillInterface->CalcuteMultiply(WorldLevel, entry->m_rextendData.getStateExtend(2));
				param2 = pkClientSkillInterface->CalcuteMasterPosCorrect(2);
				fr += (float)selfScore * param1 * param2;

				param1 = pkClientSkillInterface->CalcuteMultiply(WorldLevel, entry->m_rextendData.getStateExtend(3));
				param2 = pkClientSkillInterface->CalcuteMasterPosCorrect(3);
				fr += (float)selfScore * param1 * param2;

				param1 = pkClientSkillInterface->CalcuteMultiply(WorldLevel, entry->m_rextendData.getStateExtend(4));
				param2 = pkClientSkillInterface->CalcuteMasterPosCorrect(4);
				fr += (float)selfScore * param1 * param2;

				param1 = pkClientSkillInterface->CalcuteMultiply(WorldLevel, entry->m_rextendData.getStateExtend(5));
				param2 = pkClientSkillInterface->CalcuteMasterPosCorrect(5);
				fr += (float)selfScore * param1 * param2;

				fParamResult = (int)fr;
			}
		}
	}
	return fParamResult;
}

bool LC_TooltipBase::_formatTooltip(TooltipFieldVector& kFieldVector)
{
	////一行最长为32个西文字符
	//int MAX_TEXT_LENGTH = 32;

	//int iFieldCount = kFieldVector.size();
	//if (iFieldCount <= 0)
	//{
	//	return false;
	//}

	//for (int i = 0; i < iFieldCount; i ++)
	//{
	//	LC_TooltipField& kField = kFieldVector[i];
	//	if (kField.GetNewLine())
	//	{
	//		continue;
	//	}

	//	int iPreIndex = i - 1;
	//	if (iPreIndex < 0)
	//	{
	//		continue;
	//	}

	//	LC_TooltipField& kPreField = kFieldVector[iPreIndex];
	//	//出现连续两个不换行项，填表逻辑错误
	//	if (!kPreField.GetNewLine())
	//	{
	//		kField.SetNewLine(true);
	//		continue;
	//	}

	//	int iGapLength = MAX_TEXT_LENGTH - kField.GetContent().size() - kPreField.GetContent().size();
	//	StringType kGap = "";
	//	for (int j = 0; j < iGapLength; j ++)
	//	{
	//		kGap += " ";
	//	}
	//	kField.SetContent(kGap + kField.GetContent());
	//}

	return true;
}

//template<typename T>
//bool LC_TooltipBase::_getUseResult(int lRecordID, StringType& kUseResult)
//{
//	if (lRecordID <= 0)
//	{
//		return false;
//	}
//
//	typename T* pkRecordList = typename T::GetSingletonPtr();
//	if (0 == pkRecordList)
//	{
//		return false;
//	}
//	typename T::DataEntry* pkRecordData = pkRecordList->GetEntryPtr(lRecordID);
//	if (0 == pkRecordData)
//	{
//		return false;
//	}
//
//	StringStreamType kStringStream;
//
//	StringType kContent = pkRecordData->_kUseResult;
//	PME kTotalPattern("(.*)\\[(.*)\\](.*)", "gi");
//	PME kParamMultiAddPattern("(.*)\\*(.*)\\+(.*)", "gi");
//	PME kParamMultiPattern("(.*)\\*(.*)", "gi");
//	PME kParamAddPattern("(.*)\\+(.*)", "gi");
//
//	while (kTotalPattern.match(kContent) > 0)
//	{
//		StringType kPrefix = kTotalPattern[1];
//		StringType kParams = kTotalPattern[2];
//		StringType kPostfix = kTotalPattern[3];
//
//		//添加前缀
//		kStringStream << kPrefix;
//
//		double fParamResult = 0;
//		if (kParamMultiAddPattern.match(kParams) > 0)
//		{
//			StringType kParam = kParamMultiAddPattern[1];
//			StringType kMultiNum = kParamMultiAddPattern[2];
//			StringType kAddNum = kParamMultiAddPattern[3];
//
//			//去除字符串首尾空格
//			kParam = _trim(kParam);
//			kMultiNum = _trim(kMultiNum);
//			kAddNum = _trim(kAddNum);
//
//			double fMultiNum = atof(kMultiNum.c_str());
//			double fAddNum = atof(kAddNum.c_str());
//
//			StringType kMemberName = kParam;
//			StringType kMemberType = "";
//			StringType kMemberData = "";
//			if (!pkRecordList->GetEntryMember(lRecordID, kMemberName, kMemberType, kMemberData))
//			{
//				return false;
//			}
//
//			if (kMemberType == "int" || kMemberType == "int")
//			{
//				fParamResult = atol(kMemberData.c_str()) * fMultiNum + fAddNum;
//			}
//			else if (kMemberType == "float")
//			{
//				fParamResult = atof(kMemberData.c_str()) * fMultiNum + fAddNum;
//			}
//		}
//		else if (kParamMultiPattern.match(kParams) > 0)
//		{
//			StringType kParam = kParamMultiAddPattern[1];
//			StringType kMultiNum = kParamMultiAddPattern[2];
//
//			//去除字符串首尾空格
//			kParam = _trim(kParam);
//			kMultiNum = _trim(kMultiNum);
//
//			double fMultiNum = atof(kMultiNum.c_str());
//
//			StringType kMemberName = "";
//			StringType kMemberType = "";
//			StringType kMemberData = "";
//			if (!pkRecordList->GetEntryMember(lRecordID, kMemberName, kMemberType, kMemberData))
//			{
//				return false;
//			}
//
//			if (kMemberType == "int" || kMemberType == "int")
//			{
//				fParamResult = atol(kMemberData.c_str()) * fMultiNum;
//			}
//			else if (kMemberType == "float")
//			{
//				fParamResult = atof(kMemberData.c_str()) * fMultiNum;
//			}
//		}
//		else if (kParamAddPattern.match(kParams))
//		{
//			StringType kParam = kParamMultiAddPattern[1];
//			StringType kAddNum = kParamMultiAddPattern[3];
//
//			//去除字符串首尾空格
//			kParam = _trim(kParam);
//			kAddNum = _trim(kAddNum);
//
//			double fAddNum = atof(kAddNum.c_str());
//
//			StringType kMemberName = "";
//			StringType kMemberType = "";
//			StringType kMemberData = "";
//			if (!pkRecordList->GetEntryMember(lRecordID, kMemberName, kMemberType, kMemberData))
//			{
//				return false;
//			}
//
//			if (kMemberType == "int" || kMemberType == "int")
//			{
//				fParamResult = atol(kMemberData.c_str()) + fAddNum;
//			}
//			else if (kMemberType == "float")
//			{
//				fParamResult = atof(kMemberData.c_str()) + fAddNum;
//			}
//		}
//		kStringStream << fParamResult;
//
//		kStringStream << kPostfix;
//	}
//
//	kUseResult = kStringStream.str();
//
//	return true;
//}



