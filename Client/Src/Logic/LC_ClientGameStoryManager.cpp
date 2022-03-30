#include "ClientPCH.h"
#include "LC_ClientGameStoryManager.h"
#include "UT_LogicInfoMessage.h"
#include "LC_ClientGamePlayer.h"
#include "LC_ClientTaskMap.h"
#include "LC_ClientLogicManager.h"
#include "UI_UIManager.h"
#include "UT_Timer.h"
#include "CF_GameStory.h"
#include "CF_GameStoryRaidList.h"
#include "CF_GameStoryClientUI.h"
#include "CF_TranscriptionRule.h"
#include "CF_ShopList.h"
#include "CF_CrossRealmMap.h"
#include "LC_CommonStatAsset.h"

#include "AS_ClientSoundManager.h"

using namespace CSVFile;
using namespace Utility;
using namespace Data;
using namespace GameLogic;
using namespace Path;

bool IsTimeToday(int64_t iTime)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (!pkGamePlayer)
		return false;

	//当前时间
	uint64_t iSystemTime = pkGamePlayer->GetServerTimeVal();
	return LC_Helper::CheckIsSameDay((uint32_t)(iTime / 1000), (uint32_t)(iSystemTime));

	//return (kSystemTime.wYear == kTime.tm_year && kSystemTime.wMonth == kTime.tm_mon && kSystemTime.wDay == kTime.tm_mday);
}

//----------------------------------------------------------------------------------------
int GameStoryRaidInfo::GetEnterMinLevel()
{
	CF_TranscriptionRule* pkTranscriptionRule = CF_TranscriptionRule::GetSingletonPtr();
	CF_TranscriptionRule::DataEntry* pkData = pkTranscriptionRule->GetEntryPtr(m_lMapResID);
	if (!pkData)
		return -1;
	return pkData->_iPlayerMinLevel;
}
//----------------------------------------------------------------------------------------
int GameStoryRaidInfo::GetEnterMaxLevel()
{
	CF_TranscriptionRule* pkTranscriptionRule = CF_TranscriptionRule::GetSingletonPtr();
	CF_TranscriptionRule::DataEntry* pkData = pkTranscriptionRule->GetEntryPtr(m_lMapResID);
	if (!pkData)
		return -1;
	return pkData->_iPlayerMaxLevel;
}
//----------------------------------------------------------------------------------------
const StringType& GameStoryRaidInfo::GetShowName()
{
	CF_GameStoryRaidList* pkCSVGameStoryRaidList = CF_GameStoryRaidList::GetSingletonPtr();
	CF_GameStoryRaidList::DataEntry* pkData = pkCSVGameStoryRaidList->GetEntryPtr(m_lID);
	T_ASSERT_CRITICAL_S(pkData != NULL);

	return pkData->_kName;
}
//----------------------------------------------------------------------------------------
const StringType& GameStoryRaidInfo::GetIcon()
{
	CF_GameStoryRaidList* pkCSVGameStoryRaidList = CF_GameStoryRaidList::GetSingletonPtr();
	CF_GameStoryRaidList::DataEntry* pkData = pkCSVGameStoryRaidList->GetEntryPtr(m_lID);
	T_ASSERT_CRITICAL_S(pkData != NULL);

	return pkData->_kBossIcon;
}

//----------------------------------------------------------------------------------------
bool GameStoryRaidInfo::IsActive(void)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (!pkGamePlayer)
		return false;
	LC_ClientTaskMap* pkTaskMap = pkGamePlayer->GetClientTaskMap();
	if (!pkTaskMap)
		return false;

	std::set<int>::const_iterator it = m_kPreTasks.begin();
	for (; it != m_kPreTasks.end(); ++it)
	{
		if (!pkTaskMap->IsInFinishedTaskMap(*it))
			return false;
	}

	return true;
}

int GameStoryRaidInfo::GetEnterCost()
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (!pkGamePlayer)
		return 0;
	LC_ClientGameStoryManager* pkGameStoryMgr = pkGamePlayer->GetGameStoryManager();
	if (!pkGameStoryMgr)
		return 0;

	GameStoryInfo* pkGameStoryInfo = pkGameStoryMgr->GetGameStoryInfo(m_lBelongStoryID);
	if (!pkGameStoryInfo)
		return 0;

	return pkGameStoryInfo->GetEnterCost();
}

long GameStoryRaidInfo::GetTrueStroyID()
{
	if (m_lBelongStoryID >= GAMESTORY_TYPE_GameStory)
	{
		return m_lBelongStoryID;
	}
	else
	{
		return m_lBelongStoryID % 1000;
	}

}

//----------------------------------------------------------------------------------------
int GameStoryInfo::GetAlreadyFinishedTimes()
{
	return m_lAlreadyFinishedTimes;
}
//----------------------------------------------------------------------------------------
void GameStoryInfo::AddRaidInfo(const GameStoryRaidInfo& info)
{
	int index = GetRaidInfoIndex(info.GetCurRaidTID());
	if (index == -1)
	{
		m_kRaidIDWithIndexMap.insert(std::make_pair(info.GetCurRaidTID(), m_kStoryRaidList.size()));
		m_kStoryRaidList.push_back(info);
	}
	else
	{
		GameStoryRaidInfo &myInfo = m_kStoryRaidList[index];
		myInfo.m_lID				= info.m_lID;
		myInfo.m_lBelongStoryID		= info.m_lBelongStoryID;
		myInfo.m_lMapResID			= info.m_lMapResID;
		myInfo.m_kBossName			= info.m_kBossName;
		myInfo.m_iContentType		= info.m_iContentType;
		myInfo.m_iType				= info.m_iType;
	}
}
//----------------------------------------------------------------------------------------
void GameStoryInfo::UpdateRaidPassed()
{
	for (int i = 0; i < m_kStoryRaidList.size(); i++)
	{
		m_kStoryRaidList[i].m_lActive = m_kStoryRaidList[i].IsActive() ? 1 : 0;
		if (m_kStoryRaidList[i].GetActive() <= 0)
			m_kStoryRaidList[i].m_iState = int(GameStoryRaidInfo::GSRIST_UNPASSED);
	}
}
int GameStoryInfo::GetEnterCost()
{
	CF_ShopList::DataEntry* pkData = NULL;

	if (m_iRaidEnterTimes < m_iRaidFreeTimes)
	{
		return 0;
	}
	else
	{
		int iStepCost = 0;
		int iBaseCost = 0;
		pkData = SafeGetCSVFileDataEntryPtr<CF_ShopList>(m_iRaidBaseShopID);
		if(pkData)
			iBaseCost = LC_Helper::GetComMapValue(pkData->_ActualPriceList, CT_UNBIND_YUANBAO);
		pkData = SafeGetCSVFileDataEntryPtr<CF_ShopList>(m_iRaidStepShopID);
		if(pkData)
			iStepCost = LC_Helper::GetComMapValue(pkData->_ActualPriceList, CT_UNBIND_YUANBAO);
		int totalcost = iStepCost * (m_iRaidEnterTimes - m_iRaidFreeTimes) + iBaseCost;
		return totalcost;
	}
}

int GameStoryInfo::GetRaidInfoIndex(long lRaidID)
{
	mem::map<long, int>::iterator it = m_kRaidIDWithIndexMap.find(lRaidID);
	if (it == m_kRaidIDWithIndexMap.end())
		return -1;
	return (*it).second;
}

long GameStoryInfo::GetAllAlreadyFinishedTimes()
{
	int AllTimes = 0;
	
	for (mem::vector<GameStoryInfoTimesStat>::iterator it = m_kRaidStat.begin(); it != m_kRaidStat.end(); ++it)
	{
		AllTimes += it->m_iFinishedTimes;
	}
	return AllTimes;
}

bool GameStoryInfo::IsTimePassToday()
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	int iSystemTime = 0;
	if (pkGamePlayer && m_iTabType == ACTIVITY_TT_TIMELIMIT)
	{
		iSystemTime = (int)pkGamePlayer->GetServerTimeVal();
		//这里也进行宽松处理
		//由于同步的时间可能会有的几秒钟误差，所以当两个值差特别小的时候考虑看活动m_lActive是否大于0
		if (UT_MathBase::AbsInt(iSystemTime-(int)m_iCSVEndTime) < 10)
		{
			return GetActive() == 0;
		}
		else if (iSystemTime < m_iCSVEndTime)
		{
			return false;
		}
	}
	return true;
}
long GameStoryInfo::GetActive()
{
	if (m_iTabType != ACTIVITY_TT_TIMELIMIT)
	{
		return m_lActive;
	}
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (!pkGamePlayer)
		return m_lActive;
	//当前时间
	uint32_t iSystemTime = (uint32_t)pkGamePlayer->GetServerTimeVal();
	if (m_iType == GAMESTORY_TYPE_Species)
	{
		return m_lActive;
	}
	if (m_lActive == 1 && iSystemTime < m_iCSVOpenTime || iSystemTime > m_iCSVEndTime)
	{
		return 0;
	}
	return m_lActive;

};
long GameStoryInfo::GetActive() const
{
	if (m_iTabType != ACTIVITY_TT_TIMELIMIT)
	{
		return m_lActive;
	}
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (!pkGamePlayer)
		return m_lActive;
	//当前时间
	uint32_t iSystemTime = (uint32_t)pkGamePlayer->GetServerTimeVal();
	if (m_iType == GAMESTORY_TYPE_Species)
	{
		return m_lActive;
	}
	if (m_lActive == 1 && iSystemTime < m_iCSVOpenTime || iSystemTime > m_iCSVEndTime)//这里由于延时进行宽松的处理
	{
		return 0;
	}
	return m_lActive;

};
//----------------------------------------------------------------------------------------
int GameStoryInfo::GetMapID(int iIndex)
{
	if (!pkConfig || iIndex < 0)
	{
		return -1;
	}
	MapCtrlSetting &kMapCtrlSetting = pkConfig->m_kMapCtrlSetting;
	BatchScheduleData& kBatchScheduleData = pkConfig->m_kBatchSechedule;
	int iMapResID = -1;
	if (kMapCtrlSetting.m_kMapResIDList.empty())
	{
		GSResIDSet& kMapResIDs = kBatchScheduleData.kMapResIDs;
		GSResIDSet::iterator itMID = kMapResIDs.begin();
		for(int ii=0; itMID!=kMapResIDs.end(); ++itMID, ++ii)
		{
			if(ii == iIndex)
			{
				iMapResID = *itMID;
				break;
			}
		}
	}
	else
	{
		GSResIDList& kMapResIDs = kMapCtrlSetting.m_kMapResIDList;
		GSResIDList::iterator itMID = kMapResIDs.begin();

		for(int ii=0; itMID!=kMapResIDs.end(); ++itMID, ++ii)
		{
			if(ii == iIndex)
			{
				iMapResID = *itMID;
				break;
			}
		}
	}
	return iMapResID;
}
//----------------------------------------------------------------------------------------
StringType GameStoryInfo::GetBatchSecheduleInfo(int index)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_PlayerBase* pkPlayer = pkLogicManager->GetOperationPlayer();
	if (!pkPlayer)
		return 0;

	if (!pkConfig || index < 0)
	{
		return "";
	}

	StringType sContent("");

	MapCtrlSetting &kMapCtrlSetting = pkConfig->m_kMapCtrlSetting;
	BatchScheduleData& kBatchScheduleData = pkConfig->m_kBatchSechedule;
	BatchDataList& kBatchInfos = kBatchScheduleData.kBatchInfos;
	if(/*index >= 0 && */index < kBatchInfos.size())
	{
		BatchData& kBatchData = kBatchInfos[index];
		RewardData& kReward =  kBatchData.kReward;

		GSCommonReward gsReward;
		kReward.ResolveReward(RewardParam(), gsReward, pkPlayer, 1, 1);
		Utility::UT_SIMDataList::iterator itItem = gsReward.kItems.begin();
		for (; itItem!=gsReward.kItems.end(); ++itItem)
		{
			sContent += Utility::Int2String(itItem->ID()) + "_" + Utility::Int2String(itItem->ParamA())+"#";
		}
		return sContent;
	}
	return sContent;

}
//----------------------------------------------------------------------------------------
StringType GameStoryInfo::GetBatchSecheduleExpAndCash(int curIndex, int maxIndex)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_PlayerBase* pkPlayer = pkLogicManager->GetOperationPlayer();
	if (!pkPlayer)
		return 0;

	if (!pkConfig)
	{
		return "";
	}

	StringType sContent("");
	BatchScheduleData& kBatchScheduleData = pkConfig->m_kBatchSechedule;
	BatchConfig& kBatchConfig = kBatchScheduleData.kConfig;

	RewardData& kBatchData = kBatchConfig.kBatchReward;

	GSCommonReward gsReward;
	kBatchData.ResolveReward(RewardParam(), gsReward, pkPlayer, curIndex, maxIndex);
	sContent += Utility::Double2String(gsReward.dExp) + "#" + Utility::Double2String(gsReward.dCash) + "#";
	
	return sContent;
}
//----------------------------------------------------------------------------------------
StringType  GameStoryInfo::GetMonsterSetting(int index)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_PlayerBase* pkPlayer = pkLogicManager->GetOperationPlayer();
	if (!pkPlayer)
		return 0;

	if (!pkConfig || index < 0)
	{
		return "";
	}

	StringType sContent("");

	MapCtrlSetting &kMapCtrlSetting = pkConfig->m_kMapCtrlSetting;
	MonsterSetting &kMonsterSetting = kMapCtrlSetting.m_kMonsterSetting;
	MonsterDataList &kMonsterDataList = kMonsterSetting.kMonsterDatas;
	if (/*index >= 0 && */index < kMonsterDataList.size())
	{
		MonsterData & MonsterData = kMonsterDataList[index];
		sContent +=  Utility::Int2String(MonsterData.lCharTypeID) + "_" + Utility::Int2String(MonsterData.iCount);
	}
	return sContent;

}
//----------------------------------------------------------------------------------------
long GameStoryInfo::GetTimeStamp(void)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (!pkGamePlayer)
		return 0;

	//当前时间
	uint64_t iSystemTime = pkGamePlayer->GetServerTimeVal();
	long EndTimes = (long)GetEndTimes();
	if (m_iTabType == ACTIVITY_TT_TIMELIMIT && m_iType != GAMESTORY_TYPE_Species&& m_iType != GAMESTORY_TYPE_ZhuaHouZi&& m_iType != GAMESTORY_TYPE_Chest)
	{
		EndTimes = (long)m_iCSVEndTime;
	}
	long iTimeStamp =  EndTimes - (long)iSystemTime;
	if (iTimeStamp <= 0)
		return 0;
	else
		return iTimeStamp;
}
//----------------------------------------------------------------------------------------
long GameStoryInfo::GetBeginTimeStamp(void)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (!pkGamePlayer)
		return 0;

	//当前时间
	uint64_t iSystemTime = pkGamePlayer->GetServerTimeVal();
	long iTimeStamp = (long)m_iCSVOpenTime - (long)iSystemTime;
	if (iTimeStamp <= 0)
		return 0;
	else
		return iTimeStamp;
}
long GameStoryInfo::GetNextMonsterTimeStamp(void)
{
	Protocol::PS_GAMESTORY_BATCH_DETAIL* pkDetail = GetBatchDetail();
	if (NULL==pkDetail || pkDetail->m_iTriTimestamp < 0 || pkDetail->m_iEndTimestamp < 0)
	{
		return -1;
	}
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (!pkGamePlayer)
		return 0;

	//当前时间
	uint64_t iSystemTime = pkGamePlayer->GetServerTimeVal();
	long lNextTimeStamp = (long)UT_MathBase::MaxInt(pkDetail->m_iTriTimestamp,pkDetail->m_iEndTimestamp);
	long iTimeStamp = lNextTimeStamp - (long)iSystemTime;
	if (iTimeStamp <= 0)
		return 0;
	else
		return iTimeStamp;
}
long GameStoryInfo::GetNextMonsterTimeStampByBatchIndex(int index)
{
	Protocol::PS_GAMESTORY_BATCH_DETAIL* pkDetail = GetBatchDetailByIndex(index);
	if (NULL==pkDetail || pkDetail->m_iTriTimestamp < 0 || pkDetail->m_iEndTimestamp < 0)
	{
		return -1;
	}
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (!pkGamePlayer)
		return 0;

	//当前时间
	uint64_t iSystemTime = pkGamePlayer->GetServerTimeVal();
	long lNextTimeStamp = (long)UT_MathBase::MaxInt(pkDetail->m_iTriTimestamp,pkDetail->m_iEndTimestamp);
	long iTimeStamp = lNextTimeStamp - (long)iSystemTime;
	if (iTimeStamp <= 0)
		return 0;
	else
		return iTimeStamp;
}


//----------------------------------------------------------------------------------------
int64_t GameStoryInfo::GetBasicStatInfoCount(int iType)
{
	int64_t nResult = 0;
	Protocol::PS_GAMESTORY_BATCH_DETAIL* pkDetail = GetBatchDetail();
	switch(iType)
	{
	case GSI_BST_TOTAL_EXP:
		nResult = m_kBasicStat.m_uiTotalExp;
		break;
	case GSI_BST_TOTAL_SCORE:
		nResult = m_kBasicStat.m_uiTotalScore;
		break;
	case GSI_BST_TOTAL_CASH:
		nResult = m_kBasicStat.m_uiTotalCash;
		break;
	case GSI_BST_TOTAL_SP:
		nResult = m_kBasicStat.m_uiToalSP;
		break;
	case GSI_BST_COUNT_EXP:
		nResult = m_kBasicStat.m_kStatCount[GameStory::GSRTPlayerCommonStat::ST_CNT_DAMAGE];
		break;
	case GSI_BST_COUNT_SCORE:
		nResult = m_kBasicStat.m_kStatCount[GameStory::GSRTPlayerCommonStat::ST_CNT_INJURED];
		break;
	case GSI_BST_COUNT_KILLED:
		nResult = m_kBasicStat.m_kStatCount[GameStory::GSRTPlayerCommonStat::ST_CNT_KILL_NPC];
		break;
	case GSI_BST_CURRENT_INDEX:
		nResult = pkDetail ? pkDetail->m_uiSeqIdx : -1;
		break;
	case GSI_BST_ALL_INDEX:
		nResult = pkConfig ? pkConfig->m_kBatchSechedule.SeqSize() : 0;
		break;
	case GSI_BST_BATCH_INDEX:
		nResult = pkDetail ? pkDetail->m_uiBatchIdx : -1;
		break;
	case GSI_BST_BATCH_FLAG:
		nResult = pkDetail ? pkDetail->m_iTriTimestamp>pkDetail->m_iEndTimestamp : 0;
		break;
	case GSI_BST_CLEAR_TIME:
		nResult = m_kBatchInfo.m_uiClearTime;
		break;
	case GSI_BST_INIT_TIME:
		nResult = m_kBatchInfo.m_uiInitTime;
		break;
	case GSI_BST_TRIGER_SIZE:
		nResult = pkConfig ? pkConfig->m_kBatchSechedule.TriggerSize() : 0;
		break;
	case GSI_BST_COUNT_DEAD:
		nResult = m_kBasicStat.m_kStatCount[GameStory::GSRTPlayerCommonStat::ST_CNT_DEAD];
		break;
	case GSI_BST_COUNT_KILLPLAYER:
		nResult = m_kBasicStat.m_kStatCount[GameStory::GSRTPlayerCommonStat::ST_CNT_KILL_PLAYER];
		break;
	case GSI_BST_COUNT_KILLREWARD:
		nResult = m_kBasicStat.m_kStatCount[GameStory::GSRTPlayerCommonStat::ST_CNT_KILL_REWARD];
		break;
	}
	return nResult;
}

//----------------------------------------------------------------------------------------
StringType GameStoryInfo::GetBasicStatItemInfo(int iIndex)
{
	if (iIndex >= m_kBasicStat.m_kItems.size())
		return "";
	std::map<int32_t, int32_t>::iterator it = m_kBasicStat.m_kItems.begin();
	do 
	{
		if (iIndex == 0)
		{
			StringStreamType sStream;
			sStream << (*it).first << "#" << (*it).second;
			return sStream.str();
		}

		iIndex--;
	} while (iIndex >= 0 && it != m_kBasicStat.m_kItems.end());
	
	return "";
}

int GameStoryInfo::GetPassedRaidCount()
{
	return m_iRaidEnterTimes;
}

int GameStoryInfo::GetCanJoinRaidCount()
{
	return m_iRaidTTLTimesLimit;
}

long GameStoryInfo::GetAlreadyEnteredTimes()
{
	return m_lAlreadyEnteredTimes;
}

int GameStoryInfo::GetLastAreaInfosStateByIndex(int iIndex)
{
	std::vector<PS_GAMESTORY_AREA_INFO> &AreaInfo = m_kBatchInfo.m_kAreaInfos;
	int iSize = AreaInfo.size();
	if (iIndex < 0 || iIndex >= iSize)
	{
		return -1;
	}
	std::vector<PS_GAMESTORY_KEY_INFO> &KeyInfos = AreaInfo[iIndex].m_kKeyInfos;
	if (KeyInfos.empty())
	{
		return -1;
	}
	for (int i = 0; i < KeyInfos.size(); i++)
	{
		PS_GAMESTORY_KEY_INFO &kKeyInfo = KeyInfos[i];
		if (kKeyInfo.m_uiCharID == INVALID_ACTOR_ID)
		{
			return 1;//表示已刷新没人杀
		}
	}
	return 0;
}

long GameStoryInfo::GetTrueStroyID()
{
	if (m_lStoryID >= GAMESTORY_TYPE_GameStory)
	{
		return m_lStoryID;
	}
	else
	{
		return m_lStoryID % 1000;
	}
}

bool GameStoryInfo::IsCrossMap(int iMapResID)
{
	if (pkConfig == NULL)
	{
		return false;
	}
	const MapSetting* pkMapSetting = pkConfig->m_kMapCtrlSetting.GetMapSetting(iMapResID);
	return pkMapSetting ? pkMapSetting->IsCross() : false;
}

//----------------------------------------------------------------------------------------
LC_ClientGameStoryManager::LC_ClientGameStoryManager()
: m_lGameStoryInfoRevisionID(0)
, m_TargetGameStoryID(0)
, m_iTargetPosX(0)
, m_iTargetPosY(0)
, m_iDirectionFxID(0)
{
	memset(m_iTodayFinishedStoryNum, 0, sizeof(int)*ACTIVITY_TT_MAX);
	InitAllGameStoryInfo();
}
//----------------------------------------------------------------------------------------
LC_ClientGameStoryManager::~LC_ClientGameStoryManager()
{

}
//----------------------------------------------------------------------------------------
void LC_ClientGameStoryManager::Update( float fCurrentTime,float fDeltaTime )
{
}
//---------------------------------------------------------------------------------------------
void LC_ClientGameStoryManager::ReceiveStoryTipsContent(Protocol::MG_TIPS_SERVER_TO_CLIENT& pkMsg)
{
	StringType sShowString = STDSTR2TPSTR(pkMsg.m_kTipsContent.szStringContent);
	switch(pkMsg.m_kTipsContent.m_iShowPosition)
	{
	case TIPPOSITION_MIDDLE_AND_RIGHTBOTTOM:
		SendLogicInfoMessage(RE_TIP_GAMESTORY,sShowString);
		break;
	case TIPPOSITION_HIGH_BLUE:
		SendLogicInfoMessage(RE_TRANSCRIPTION_BLUE_HIGH_TIP,sShowString);
		break;
	default:
		break;
	}
}
//---------------------------------------------------------------------------------------------
void LC_ClientGameStoryManager::ReceiveGameStoryContentInfosync(Protocol::MG_GAMESTORY_CONTENT_INFO_SYNC& pkMsg)
{
	T_PROFILE_TIMER(__FUNCTION__);
	/**/
	StoryStateMap	kGameStoryStateMap;

	memset(m_iTodayFinishedStoryNum, 0, sizeof(int)*ACTIVITY_TT_MAX);
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (!pkGamePlayer)
		return;

	//当前时间
	T_ASSERT(pkMsg.m_iCount <= MAX_GAMESTORY_BRODCAST_TO_CLIENT);
	for(int i = 0; i < pkMsg.m_iCount; ++i)
	{
		Protocol::GAMESTORY_CONTENT_INFO &kStoryContentInfo = pkMsg.m_kGameStoryContent[i];

		long iStoryID = kStoryContentInfo.m_uiStoryID;

		if (m_kNormalIDWithIndexMap.find(iStoryID) == m_kNormalIDWithIndexMap.end())
		{
			continue;
		}
		const GameStoryMapForVector& kMappingData = m_kNormalIDWithIndexMap[iStoryID];
		GameStoryInfo& kStoryInfo = m_kNormalGameStoryInfoList[kMappingData.m_iType][kMappingData.m_iIndex];
		if (kStoryInfo.m_iTabType != ACTIVITY_TT_TIMELIMIT && kStoryInfo.m_iTabType != ACTIVITY_TT_RAIDBOSSSPECIES)
		{
			bool oldCanJoin = CanJoin(kStoryInfo);

			kStoryInfo.m_lStoryID				= iStoryID;
			kStoryInfo.m_lActive				= kStoryContentInfo.m_iActive;
			if (kStoryInfo.m_iTabType != ACTIVITY_TT_NUMBERLIMIT)///做得我想shi
			{
				kStoryInfo.m_lAlreadyEnteredTimes	= kStoryContentInfo.m_iAlreadyEnteredTimes;
				kStoryInfo.m_iAlreadyBuyTimes	= kStoryContentInfo.m_iAlreadyBuyTimes;
			}
			kStoryInfo.m_bIsFinished			= kStoryContentInfo.m_iAlreadyFinishedTimes>0;
			kStoryInfo.m_lAlreadyFinishedTimes  = kStoryContentInfo.m_iAlreadyFinishedTimes;

			kStoryInfo.m_kBasicStat				= kStoryContentInfo.m_kBasicStat;

			kStoryInfo.m_iTimeBegin				= kStoryContentInfo.m_iTimeBegin;
			kStoryInfo.m_iTimeEnd				= kStoryContentInfo.m_iTimeEnd;
			kStoryInfo.m_kCronTime				= STDSTR2TPSTR(kStoryContentInfo.m_szCronStr);
			kStoryInfo.m_iRaidEnterTimes = 0;
			kStoryInfo.m_kRaidStat.clear();
			for (size_t j = 0; j < kStoryContentInfo.m_kRaidStat.size(); j++)
			{
				kStoryInfo.m_kRaidStat.push_back(GameStoryInfo::GameStoryInfoTimesStat(kStoryContentInfo.m_kRaidStat[j].m_iType, kStoryContentInfo.m_kRaidStat[j].m_iEnterTimes, kStoryContentInfo.m_kRaidStat[j].m_iFinishedTimes));
				
 				if (kStoryInfo.m_kStoryRaidList.size() > j)
 				{
 					kStoryInfo.m_iRaidEnterTimes += kStoryInfo.m_kRaidStat[j].m_iEnterTimes;
 				}
			}

			if (kStoryInfo.m_iCurRaidTID < 0)
				kStoryInfo.m_iCurRaidTID = kStoryInfo.GetLastEnteredRaidID();
			kStoryInfo.UpdateRaidPassed();

			if (kStoryContentInfo.m_iTimeEnd == kStoryContentInfo.m_iTimeBegin)
			{
				kStoryInfo.m_bIsOpened		= false;
			}
			else
			{
				kStoryInfo.m_bIsOpened		= true;
			}

			if (kStoryInfo.m_bIsFinished && IsTimeToday(kStoryContentInfo.m_iTimeBegin))
				m_iTodayFinishedStoryNum[kMappingData.m_iType]++; 

			bool newCanJoin = CanJoin(kStoryInfo);
			
			kGameStoryStateMap[iStoryID] = newCanJoin - oldCanJoin;
		}
		else if(kStoryInfo.m_iTabType == ACTIVITY_TT_RAIDBOSSSPECIES)
		{
			int iSecondStory = kStoryInfo.m_lStoryID;
			const GameStoryMapForVector& kMappingData = m_kNormalIDWithIndexMap[iSecondStory];
			GameStoryInfo& SecondkStoryInfo = m_kNormalGameStoryInfoList[kMappingData.m_iType][kMappingData.m_iIndex];

			bool oldCanJoin = CanJoin(SecondkStoryInfo);

			SecondkStoryInfo.m_lStoryID				= iSecondStory;
			SecondkStoryInfo.m_lActive				= kStoryContentInfo.m_iActive;
			SecondkStoryInfo.m_lAlreadyEnteredTimes	= kStoryContentInfo.m_iAlreadyEnteredTimes;
			SecondkStoryInfo.m_iAlreadyBuyTimes	= kStoryContentInfo.m_iAlreadyBuyTimes;
			SecondkStoryInfo.m_bIsFinished			= kStoryContentInfo.m_iAlreadyFinishedTimes > 0;
			SecondkStoryInfo.m_lAlreadyFinishedTimes  = kStoryContentInfo.m_iAlreadyFinishedTimes;
			SecondkStoryInfo.m_kBasicStat			= kStoryContentInfo.m_kBasicStat;
			SecondkStoryInfo.m_iTimeBegin			= kStoryContentInfo.m_iTimeBegin;
			SecondkStoryInfo.m_iTimeEnd				= kStoryContentInfo.m_iTimeEnd;
			SecondkStoryInfo.m_kCronTime			= STDSTR2TPSTR(kStoryContentInfo.m_szCronStr);
			SecondkStoryInfo.m_iRaidEnterTimes = 0;
			SecondkStoryInfo.m_kRaidStat.clear();
			for (size_t j = 0; j < kStoryContentInfo.m_kRaidStat.size(); j++)
			{
				const PS_GAME_RAID_STAT & info = kStoryContentInfo.m_kRaidStat[j];
				SecondkStoryInfo.m_kRaidStat.push_back(GameStoryInfo::GameStoryInfoTimesStat(info.m_iType, info.m_iEnterTimes, info.m_iFinishedTimes));

				if (SecondkStoryInfo.m_kStoryRaidList.size() > j)
				{
					SecondkStoryInfo.m_iRaidEnterTimes += SecondkStoryInfo.m_kRaidStat[j].m_iEnterTimes;
				}
			}

			if (SecondkStoryInfo.m_iCurRaidTID < 0)
				SecondkStoryInfo.m_iCurRaidTID = SecondkStoryInfo.GetLastEnteredRaidID();
			SecondkStoryInfo.UpdateRaidPassed();

			if (SecondkStoryInfo.m_iCSVOpenTime == SecondkStoryInfo.m_iCSVEndTime)
			{
				SecondkStoryInfo.m_bIsOpened		= false;
			}
			else
			{

				char strTemp[MAX_GAMESTORY_TIME_DESCRIPTION_LENGTH + 1];
				time_t bt = time_t(SecondkStoryInfo.m_iCSVOpenTime);
				time_t et = time_t(SecondkStoryInfo.m_iCSVEndTime);

				tm bLT = *localtime(&bt);
				tm bET = *localtime(&et);

				sprintf_s(strTemp,sizeof(strTemp),"%d:%02d - %d:%02d",bLT.tm_hour, bLT.tm_min, bET.tm_hour, bET.tm_min);

				SecondkStoryInfo.m_bIsOpened		= true;
				SecondkStoryInfo.m_strTimeSlot	= strTemp;
			}

			if (SecondkStoryInfo.m_bIsFinished && IsTimeToday(kStoryContentInfo.m_iTimeBegin))
				m_iTodayFinishedStoryNum[kMappingData.m_iType]++; 

			bool newCanJoin = CanJoin(SecondkStoryInfo);

			kGameStoryStateMap[iSecondStory] = newCanJoin - oldCanJoin;
		}
		else
		{

			kStoryInfo.m_kCronTime	= STDSTR2TPSTR(kStoryContentInfo.m_szCronStr);
			std::vector<uint32_t> kCronTime;
			_FilterCronTimestampSameDay(kStoryInfo.m_kCronTime,kCronTime);
			for (int i = 0; i < kCronTime.size(); i++)
			{
				int iSecondStory = kStoryInfo.m_lStoryID + 1000 * i;
				if (m_kNormalIDWithIndexMap.find(iSecondStory) == m_kNormalIDWithIndexMap.end())
				{
					continue;
				}
				const GameStoryMapForVector& kMappingData = m_kNormalIDWithIndexMap[iSecondStory];
				GameStoryInfo& SecondkStoryInfo = m_kNormalGameStoryInfoList[kMappingData.m_iType][kMappingData.m_iIndex];

				SecondkStoryInfo.m_iCSVOpenTime = kCronTime[i];
				SecondkStoryInfo.m_iCSVEndTime = SecondkStoryInfo.m_iCSVOpenTime + SecondkStoryInfo.m_iTimeLen * 60;
				bool oldCanJoin = CanJoin(SecondkStoryInfo);

				SecondkStoryInfo.m_lStoryID				= iSecondStory;
				SecondkStoryInfo.m_lActive				= kStoryContentInfo.m_iActive;
				if (SecondkStoryInfo.m_iTabType != ACTIVITY_TT_NUMBERLIMIT)///做得我想shi
				{
					SecondkStoryInfo.m_lAlreadyEnteredTimes	= kStoryContentInfo.m_iAlreadyEnteredTimes;
					SecondkStoryInfo.m_iAlreadyBuyTimes	= kStoryContentInfo.m_iAlreadyBuyTimes;
				}
				SecondkStoryInfo.m_bIsFinished			= kStoryContentInfo.m_iAlreadyFinishedTimes > 0;
				SecondkStoryInfo.m_lAlreadyFinishedTimes  = kStoryContentInfo.m_iAlreadyFinishedTimes;

				SecondkStoryInfo.m_kBasicStat			= kStoryContentInfo.m_kBasicStat;

				SecondkStoryInfo.m_iTimeBegin			= kStoryContentInfo.m_iTimeBegin;
				SecondkStoryInfo.m_iTimeEnd				= kStoryContentInfo.m_iTimeEnd;
				SecondkStoryInfo.m_kCronTime			= STDSTR2TPSTR(kStoryContentInfo.m_szCronStr);
				SecondkStoryInfo.m_iRaidEnterTimes = 0;
				SecondkStoryInfo.m_kRaidStat.clear();
				for (size_t j = 0; j < kStoryContentInfo.m_kRaidStat.size(); j++)
				{
					SecondkStoryInfo.m_kRaidStat.push_back(GameStoryInfo::GameStoryInfoTimesStat(kStoryContentInfo.m_kRaidStat[j].m_iType, kStoryContentInfo.m_kRaidStat[j].m_iEnterTimes, kStoryContentInfo.m_kRaidStat[j].m_iFinishedTimes));

					if (SecondkStoryInfo.m_kStoryRaidList.size() > j)
					{
						SecondkStoryInfo.m_iRaidEnterTimes += SecondkStoryInfo.m_kRaidStat[j].m_iEnterTimes;
					}
				}

				if (SecondkStoryInfo.m_iCurRaidTID < 0)
					SecondkStoryInfo.m_iCurRaidTID = SecondkStoryInfo.GetLastEnteredRaidID();
				SecondkStoryInfo.UpdateRaidPassed();

				if (SecondkStoryInfo.m_iCSVOpenTime == SecondkStoryInfo.m_iCSVEndTime)
				{
					SecondkStoryInfo.m_bIsOpened		= false;
				}
				else if (SecondkStoryInfo.m_lStoryID >= 38 && SecondkStoryInfo.m_lStoryID <= 41)   // 进阶副本特殊显示
				{
					char strTemp[MAX_GAMESTORY_TIME_DESCRIPTION_LENGTH + 1];
					time_t bt = time_t(SecondkStoryInfo.m_iCSVOpenTime);
					time_t et = time_t(SecondkStoryInfo.m_iCSVEndTime);

					tm bLT = *localtime(&bt);
					tm bET = *localtime(&et);

					sprintf_s(strTemp,sizeof(strTemp),"%s%d","星期",bLT.tm_wday);
					
					SecondkStoryInfo.m_bIsOpened		= true;
					SecondkStoryInfo.m_strTimeSlot	= strTemp;
				}
				else
				{
					
						char strTemp[MAX_GAMESTORY_TIME_DESCRIPTION_LENGTH + 1];
						time_t bt = time_t(SecondkStoryInfo.m_iCSVOpenTime);
						time_t et = time_t(SecondkStoryInfo.m_iCSVEndTime);

						tm bLT = *localtime(&bt);
						tm bET = *localtime(&et);

						sprintf_s(strTemp,sizeof(strTemp),"%d:%02d - %d:%02d",bLT.tm_hour, bLT.tm_min, bET.tm_hour, bET.tm_min);

						SecondkStoryInfo.m_bIsOpened		= true;
						SecondkStoryInfo.m_strTimeSlot	= strTemp;
					
					
				}

				if (SecondkStoryInfo.m_bIsFinished && IsTimeToday(kStoryContentInfo.m_iTimeBegin))
					m_iTodayFinishedStoryNum[kMappingData.m_iType]++; 

				bool newCanJoin = CanJoin(SecondkStoryInfo);

				kGameStoryStateMap[iSecondStory] = newCanJoin - oldCanJoin;
			}
		}
	}
	/**/
	//_updateGameStoryYunBiao();
	_updateGameStoryState(kGameStoryStateMap);
	std::sort(m_pkClientGameStoryInfoList[ACTIVITY_TT_TIMELIMIT].begin(),m_pkClientGameStoryInfoList[ACTIVITY_TT_TIMELIMIT].end(),_sortTimeLimitGameStoryList);
	AddGameStoryRevisionID(ACTIVITY_DIRTY_FLAG_UPDATE_LIST);
}
//----------------------------------------------------------------------------------------
bool LC_ClientGameStoryManager::CheckGameStoryStartInfo(long lStoryID)
{	
	if (m_kNormalIDWithIndexMap.find(lStoryID) == m_kNormalIDWithIndexMap.end())
		return false;
	bool bActiveFlag = false;
	int j = 0;
	do 
	{
		int iSecondStory = lStoryID + 1000 * j;
		if (m_kNormalIDWithIndexMap.find(iSecondStory) == m_kNormalIDWithIndexMap.end())
		{
			break;
		}
		const GameStoryMapForVector& kMappingData = m_kNormalIDWithIndexMap[iSecondStory];
		const GameStoryInfo& kStoryInfo = m_kNormalGameStoryInfoList[kMappingData.m_iType][kMappingData.m_iIndex];
		if (kStoryInfo.GetActive() != 0)
		{
			bActiveFlag = true;
			break;
		}
		j++;
	} while (1);
	return bActiveFlag;
}
//----------------------------------------------------------------------------------------
StringType	LC_ClientGameStoryManager::GetGameStoryTimeSlot(long lGameStoryID)
{
	StringStreamType sStream; 
	CF_GameStory* pkCSVGameStory = CF_GameStory::GetSingletonPtr();
	if (pkCSVGameStory)
	{
		CF_GameStory::DataEntryMapExternIterator iter = pkCSVGameStory->GetIterator();
		while (iter.HasMoreElements())
		{
			CF_GameStory::DataEntry* pkData = iter.PeekNextValuePtr();			
			if (pkData->_iID >= lGameStoryID)
			{
				std::vector<uint32_t> kCronTime;
				_FilterCronTimestampSameDay(pkData->_kCronStr,kCronTime);
				if (!kCronTime.empty())
				{
					sStream << kCronTime.size()<<"#";
					for (int j = 0; j < kCronTime.size(); j++)
					{
						uint32_t OpenTime = kCronTime[j];
						uint32_t EndTime = OpenTime + pkData->_kTimeLen * 60;
						long StoryID = 1000 * j  + pkData->_iID;

						char strTemp[MAX_GAMESTORY_TIME_DESCRIPTION_LENGTH + 1];
						time_t bt = time_t(OpenTime);
						time_t et = time_t(EndTime);

						tm bLT = *localtime(&bt);
						tm bET = *localtime(&et);

						sprintf_s(strTemp,sizeof(strTemp),"%d:%02d - %d:%02d",bLT.tm_hour, bLT.tm_min, bET.tm_hour, bET.tm_min);

						sStream<<StoryID<<"#"<<strTemp<<"#";
					}
				}
			}
			iter.MoveNext();
		}
	}
	return sStream.str();
}
//----------------------------------------------------------------------------------------
StringType	LC_ClientGameStoryManager::GetGameStoryTimeSlotOnWeekday(long lGameStoryID)
{
	StringStreamType sStream; 
	CF_GameStory* pkCSVGameStory = CF_GameStory::GetSingletonPtr();
	if (pkCSVGameStory)
	{
		CF_GameStory::DataEntryMapExternIterator iter = pkCSVGameStory->GetIterator();
		while (iter.HasMoreElements())
		{
			CF_GameStory::DataEntry* pkData = iter.PeekNextValuePtr();			
			if (pkData->_iID == lGameStoryID)
			{
				const StringType& timeStr = pkData ->_kCronStr;
				TStringVector res = UT_BaseStringUtility::SplitString(timeStr, ' ');
				if (res.size() == 6)
				{
					StringType& weekday = res[5];
					TStringVector timeline = UT_BaseStringUtility::SplitString(weekday, ':');
					if(timeline.size() == 7)
					{
						sStream << GET_MSG(CLIENT_EVERYDAY) << "/";
						break;
					}
					else
					{
						const char* Words[7] = {"1", "2", "3" ,"4", "5", "6", CLIENT_TEXT_DAY };
						for (int i = 0; i < timeline.size() ; i++ )
						{
							sStream << Words[String2Int(timeline[i])-1] << "/";
						}
						break;
					}
				}
			}
			iter.MoveNext();
		}
	}
	return sStream.str();
}
//----------------------------------------------------------------------------------------
GameStoryInfo*	LC_ClientGameStoryManager::GetGameStoryInfo(long lStoryID)
{
	if (m_kNormalIDWithIndexMap.find(lStoryID) == m_kNormalIDWithIndexMap.end())
		return NULL;

	const GameStoryMapForVector& kMappingData = m_kNormalIDWithIndexMap[lStoryID];

	GameStoryInfo& Info = m_kNormalGameStoryInfoList[kMappingData.m_iType][kMappingData.m_iIndex];
	long bactive = Info.m_lActive;
	return &m_kNormalGameStoryInfoList[kMappingData.m_iType][kMappingData.m_iIndex];
}


GameStoryInfo* LC_ClientGameStoryManager::GetGameStoryInfoByIndex(int iType, int iIndex)
{
	if (iType < 0 || iType >= ACTIVITY_TT_MAX)
		return NULL;
	
	if (iIndex < 0 || iIndex >= m_pkClientGameStoryInfoList[iType].size())
		return NULL;
	return m_pkClientGameStoryInfoList[iType][iIndex];
}

//----------------------------------------------------------------------------------------
int	LC_ClientGameStoryManager::GetGameStoryType(long lGameStoryID)	
{
	GameStoryInfo*	pkStoryInfo = GetGameStoryInfo(lGameStoryID);
	if (pkStoryInfo)
		return pkStoryInfo->m_iType;
	return 0;
}

int LC_ClientGameStoryManager::GetGameStoryMonsterState(long lGameStoryID, int CharTypeID)
{
	GameStoryInfo*	pkStoryInfo = GetGameStoryInfo(lGameStoryID);
	if (pkStoryInfo)
	{
		Protocol::GAMESTORY_BATCH_INFO	&kBatchInfo = pkStoryInfo->m_kBatchInfo;
		for (int i = 0; i < kBatchInfo.m_kAreaInfos.size(); ++i)
		{
			std::vector<PS_GAMESTORY_KEY_INFO> &keyInfo = kBatchInfo.m_kAreaInfos[i].m_kKeyInfos;
			for (int j = 0; j < keyInfo.size(); ++j)
			{
				if (keyInfo[j].m_iResID == CharTypeID)
				{
					return keyInfo[j].m_nState;
				}
			}
		}
	}
	return 0;
}

int LC_ClientGameStoryManager::GetGameStoryMonsterTimestamp(long lGameStoryID, int CharTypeID)
{
	GameStoryInfo*	pkStoryInfo = GetGameStoryInfo(lGameStoryID);
	if (pkStoryInfo)
	{
		const Protocol::GAMESTORY_BATCH_INFO	&kBatchInfo = pkStoryInfo->m_kBatchInfo;
		for (int i = 0; i < kBatchInfo.m_kAreaInfos.size(); ++i)
		{
			const std::vector<PS_GAMESTORY_KEY_INFO> &keyInfo = kBatchInfo.m_kAreaInfos[i].m_kKeyInfos;
			for (int j = 0; j < keyInfo.size(); ++j)
			{
				if (keyInfo[j].m_iResID == CharTypeID)
				{
					return keyInfo[j].m_uiTimestamp;
				}
			}
		}
	}
	return -1;
}
//----------------------------------------------------------------------------------------
void LC_ClientGameStoryManager::InitAllGameStoryInfo()
{
	for (int i = 0; i < ACTIVITY_TT_MAX; i++)
	{
		m_kNormalGameStoryInfoList[i].clear();
		m_kSpecialGameStoryInfoList[i].clear();
	}
	m_kNormalIDWithIndexMap.clear();
	m_kSpecialIDWithIndexMap.clear();

	//常规活动
	CF_GameStory* pkCSVGameStory = CF_GameStory::GetSingletonPtr();
	if (pkCSVGameStory)
	{
		CF_GameStory::DataEntryMapExternIterator iter = pkCSVGameStory->GetIterator();
		while (iter.HasMoreElements())
		{
			CF_GameStory::DataEntry* pkData = iter.PeekNextValuePtr();			
			GameStoryInfo kData;
			if (pkData->_kContentType >= 0)
			{
				kData.m_bValid					= true;
				kData.m_lStoryID				= pkData->_iID;
				kData.m_lActive					= 0;
				kData.m_lMinEnterLevel			= pkData->_iPlayerMinLevel;
				kData.m_lMaxEnterLevel			= pkData->_iPlayerMaxLevel;
				kData.m_lAlreadyEnteredTimes	= 0;
				kData.m_iAlreadyBuyTimes		= 0;
				kData.m_lAlreadyFinishedTimes	= 0;
				kData.m_bIsFinished				= false;
				kData.m_strTimeSlot				= "";
				kData.m_iContentType			= pkData->_kContentType;
				kData.m_iTabType				= pkData->_lTabType;
				kData.m_iType					= pkData->_lGameStoryType;
				kData.m_iCSVStoryID				= pkData->_iID;
				kData.m_iTimeLen				= pkData->_kTimeLen;
				///////UI显示相关，由于有两个不同的表，所以存储在这里
				kData.m_kClientShowName				= pkData->_kName;
				kData.m_iClientUIGroupRecommendNumber			= pkData->_iGroupRecommendNumber;
				kData.m_kClientUINotice				= pkData->_kNotice;
				kData.m_kClientUIReward				= pkData->_kReward;
				kData.m_kClientUIDescription			= pkData->_kDescription;
				kData.m_kClientUIPlayRule			= pkData->_kPlayRule;
				kData.m_kClientUIGameStoryRule			= pkData->_kGameStoryRule;
				kData.m_kClientUIResetTime			= pkData->_kResetTime;
				kData.m_kClientUIResetMap			= pkData->_kResetMap;
				kData.m_kClientUIStatisticsTip			= pkData->_kStatisticsTips;
				kData.m_kClientUIBackImage			= pkData->_BackImage;
				kData.m_kClientNameImage			= pkData->_NameImage;
				kData.m_kClientUITooltipReward			= pkData->_kTooltipReward;
				kData.m_kClientUITip				= pkData->_kTip;
				kData.m_kClientUIMapListText		= pkData->_kMapListText;
				kData.m_kCronTime					= pkData->_kCronStr;
				kData.m_iCurRaidTID					= pkData->_lGameStoryRaidID;
				kData.m_iGameStoryInfoRaidID		= pkData->_lGameStoryRaidID;
				kData.m_kScript						= pkData->_sScript;
				/*kData.m_iValidDays					= pkData->_iValidDay;*/
				kData.m_iFirstLightTaskID			= pkData->_lFirstLightTaskID;
				kData.m_kClientRewardType			= pkData->_RewardType;
				kData.m_iElementDamageType			= pkData->_ielementdamagetype;
				kData.m_IsGuildActivity             =pkData->_IsGuildActivity ; 
				if (!pkData->_sScript.empty())
				{
					GameStory::GameStoryCommonManager* pkGSMgr = GameStory::GameStoryCommonManager::GetSingletonPtr();
					if (pkGSMgr->LoadStoryConfig(pkData->_sScript, ""))
					{
						kData.pkConfig = pkGSMgr->GetStoryConfig(pkData->_sScript);
					}
				}
				if (kData.pkConfig)
				{
					MapCtrlSetting& kMapCtrlSetting = kData.pkConfig->m_kMapCtrlSetting;
					kData.m_iRaidTTLTimesLimit = kData.pkConfig->m_iFinishTimes;
					kData.m_lMaxEnteredTimes = kData.pkConfig->m_iJoinTimes;
					kData.m_iRaidFreeTimes = kMapCtrlSetting.m_iMapFreeTimes;
					kData.m_iRaidBaseShopID = kMapCtrlSetting.m_iBaseCostShopID;
					kData.m_iRaidStepShopID	= kMapCtrlSetting.m_iStepCostShopID;
					if (kMapCtrlSetting.m_kMapResIDList.empty())
					{
						BatchScheduleData& kBatchScheduleData = kData.pkConfig->m_kBatchSechedule;
						GSResIDSet& kMapResIDs = kBatchScheduleData.kMapResIDs;
						//kData.m_kStoryRaidList.resize(kMapResIDs.size());
						GSResIDSet::iterator itMID = kMapResIDs.begin();
						for (; itMID != kMapResIDs.end(); ++itMID)
						{
							int32_t iMapResID = *itMID;
							GameStoryRaidInfo kRaidData;
							kRaidData.m_lMapResID = iMapResID;

							 const MapSetting* pkMapSetting = kMapCtrlSetting.GetMapSetting(iMapResID);
				 			if (pkMapSetting)
		 					{
		 						kRaidData.m_iRespawnShopID = pkMapSetting->iRespawnCostShopID;
								//kRaidSet.m_uiTimesLimit = pkMapSetting->iTimesLmt;
		 						kRaidData.m_kPreTasks.insert(pkMapSetting->kPreTaskIDs.begin(), pkMapSetting->kPreTaskIDs.end());
							}
							kData.AddRaidInfo(kRaidData);
						}
					}
					else
					{
						GSResIDList& kMapResIDs = kMapCtrlSetting.m_kMapResIDList;
						//kData.m_kStoryRaidList.resize(kMapResIDs.size());
						GSResIDList::iterator itMID = kMapResIDs.begin();
						for (; itMID != kMapResIDs.end(); ++itMID)
						{
							int32_t iMapResID = *itMID;
							GameStoryRaidInfo kRaidData;
							kRaidData.m_lMapResID = iMapResID;
		 					const MapSetting* pkMapSetting = kMapCtrlSetting.GetMapSetting(iMapResID);
		 					if (pkMapSetting)
		 					{
		 						kRaidData.m_iRespawnShopID = pkMapSetting->iRespawnCostShopID;
		 						//kRaidSet.m_uiTimesLimit = pkMapSetting->iTimesLmt;
		 
		 						kRaidData.m_kPreTasks.insert(pkMapSetting->kPreTaskIDs.begin(), pkMapSetting->kPreTaskIDs.end());
							}
							kData.AddRaidInfo(kRaidData);
						}
					}
				}
				//区域id
				kData.m_kAreaList.clear();
				TStringVector kStringVector = UT_BaseStringUtility::SplitString(pkData->_kArea,';');

				for (int j = 0; j < kStringVector.size(); j++)
				{
					if(kStringVector[j] != "")
					{
						int iAreaID = atoi(kStringVector[j].c_str());
						kData.m_kAreaList.push_back(iAreaID);
					}
				}
				int iTabType = pkData->_lTabType;
				if (iTabType == ACTIVITY_TT_TIMELIMIT)
				{
					if(kData.m_iType == GAMESTORY_TYPE_Species) //做得我想shi
					{
						m_kNormalIDWithIndexMap.insert(std::make_pair(kData.m_lStoryID, GameStoryMapForVector(iTabType, m_kNormalGameStoryInfoList[iTabType].size())));
						m_kNormalGameStoryInfoList[iTabType].push_back(kData);
					}
					else
					{
						std::vector<uint32_t> kCronTime;
						_FilterCronTimestampSameDay(kData.m_kCronTime,kCronTime);
						if (!kCronTime.empty())
						{
							for (int j = 0; j < kCronTime.size(); j++)
							{
								kData.m_iCSVOpenTime = kCronTime[j];
								kData.m_iCSVEndTime = kData.m_iCSVOpenTime + kData.m_iTimeLen * 60;
								kData.m_lStoryID = 1000 * j  + pkData->_iID;
								m_kNormalIDWithIndexMap.insert(std::make_pair(kData.m_lStoryID, GameStoryMapForVector(iTabType, m_kNormalGameStoryInfoList[iTabType].size())));
								m_kNormalGameStoryInfoList[iTabType].push_back(kData);
							}
						}
						else
						{
							kData.m_iCSVOpenTime = 0;
							kData.m_iCSVEndTime = kData.m_iCSVOpenTime + kData.m_iTimeLen * 60;
							kData.m_lStoryID = pkData->_iID;
							m_kNormalIDWithIndexMap.insert(std::make_pair(kData.m_lStoryID, GameStoryMapForVector(iTabType, m_kNormalGameStoryInfoList[iTabType].size())));
							m_kNormalGameStoryInfoList[iTabType].push_back(kData);
						}			
					}
				}
				else
				{
					m_kNormalIDWithIndexMap.insert(std::make_pair(kData.m_lStoryID, GameStoryMapForVector(iTabType, m_kNormalGameStoryInfoList[iTabType].size())));
					m_kNormalGameStoryInfoList[iTabType].push_back(kData);
				}
			}
			else
			{
				if (!pkData->_sScript.empty())
				{
					GameStory::GameStoryCommonManager* pkGSMgr = GameStory::GameStoryCommonManager::GetSingletonPtr();
					pkGSMgr->LoadStoryConfig(pkData->_sScript, "");
				}
			}
			iter.MoveNext();
		}
	}
	
	//特殊活动
	CF_GameStoryClientUI* pkClientCSVGameStory = CF_GameStoryClientUI::GetSingletonPtr();
	if (pkClientCSVGameStory)
	{
		CF_GameStoryClientUI::DataEntryMapExternIterator iter = pkClientCSVGameStory->GetIterator();
		while (iter.HasMoreElements())
		{
			CF_GameStoryClientUI::DataEntry* pkData = iter.PeekNextValuePtr();			

			GameStoryInfo kData;
			if (pkData->_lTabType == ACTIVITY_TT_TIMELIMIT)
			{
				kData.m_bValid					= true;
				kData.m_lStoryID				= pkData->_iID;
				kData.m_lActive					= 0;
				kData.m_lMinEnterLevel			= pkData->_iPlayerMinLevel;
				kData.m_lMaxEnterLevel			= pkData->_iPlayerMaxLevel;
				kData.m_lAlreadyEnteredTimes	= 0;
				kData.m_iAlreadyBuyTimes		= 0;
				kData.m_lAlreadyFinishedTimes	= 0;
				kData.m_bIsFinished				= false;
				kData.m_strTimeSlot				= "";
				kData.m_iContentType			= pkData->_kContentType;
				kData.m_iTabType				= pkData->_lTabType;
				kData.m_iType					= pkData->_lGameStoryType;
				
				///////UI显示相关，由于有两个不同的表，所以存储在这里
				kData.m_kClientShowName			= pkData->_kName;
				kData.m_iClientUIGroupRecommendNumber			= pkData->_iGroupRecommendNumber;
				kData.m_kClientUINotice			= pkData->_kNotice;
				kData.m_kClientUIReward				= pkData->_kReward;
				kData.m_kClientUIDescription			= pkData->_kDescription;
				kData.m_kClientUIPlayRule			= pkData->_kPlayRule;
				kData.m_kClientUIGameStoryRule			= pkData->_kGameStoryRule;
				kData.m_kClientUIResetTime			= pkData->_kResetTime;
				kData.m_kClientUIResetMap			= pkData->_kResetMap;
				kData.m_kClientUIStatisticsTip			= pkData->_kStatisticsTips;
				kData.m_kClientUIBackImage			= pkData->_BackImage;
				kData.m_kClientNameImage			= pkData->_NameImage;
				kData.m_kClientUITooltipReward			= pkData->_kTooltipReward;
				kData.m_kClientUITip			= pkData->_kTip;
				kData.m_kClientUIMapListText			= pkData->_kMapListText;
				kData.m_iFirstLightTaskID			= pkData->_lFirstLightTaskID;
				int iTabType = pkData->_lTabType;
				m_kSpecialIDWithIndexMap.insert(std::make_pair(pkData->_iID, GameStoryMapForVector(iTabType, m_kSpecialGameStoryInfoList[iTabType].size())));
				m_kSpecialGameStoryInfoList[iTabType].push_back(kData);
			}
			else if (pkData->_lTabType == ACTIVITY_TT_NUMBERLIMIT)
			{
				kData.m_bValid					= true;
				kData.m_lStoryID				= pkData->_iID;
				kData.m_lActive					= 1;
				kData.m_lMinEnterLevel			= pkData->_iPlayerMinLevel;
				kData.m_lMaxEnterLevel			= pkData->_iPlayerMaxLevel;
				kData.m_lAlreadyEnteredTimes	= 0;
				kData.m_iAlreadyBuyTimes		= 0;
				kData.m_lAlreadyFinishedTimes	= 0;
				kData.m_bIsFinished				= false;
				kData.m_strTimeSlot				= "";
				kData.m_kCronTime				= pkData->_kCronStr;
				kData.m_iTimeLen				= pkData->_kTimeLen;
				kData.m_iContentType			= pkData->_kContentType;
				kData.m_iTabType				= pkData->_lTabType;
				kData.m_iType					= pkData->_lGameStoryType;

				///////UI显示相关，由于有两个不同的表，所以存储在这里
				kData.m_kClientShowName			= pkData->_kName;
				kData.m_iClientUIGroupRecommendNumber			= pkData->_iGroupRecommendNumber;
				kData.m_kClientUINotice			= pkData->_kNotice;
				kData.m_kClientUIReward				= pkData->_kReward;
				kData.m_kClientUIDescription			= pkData->_kDescription;
				kData.m_kClientUIPlayRule			= pkData->_kPlayRule;
				kData.m_kClientUIGameStoryRule			= pkData->_kGameStoryRule;
				kData.m_kClientUIResetTime			= pkData->_kResetTime;
				kData.m_kClientUIResetMap			= pkData->_kResetMap;
				kData.m_kClientUIStatisticsTip			= pkData->_kStatisticsTips;
				kData.m_kClientUIBackImage			= pkData->_BackImage;
				kData.m_kClientNameImage			= pkData->_NameImage;
				kData.m_kClientUITooltipReward			= pkData->_kTooltipReward;
				kData.m_kClientUITip			= pkData->_kTip;
				kData.m_kClientUIMapListText			= pkData->_kMapListText;
				kData.m_iCurRaidTID						= pkData->_lGameStoryRaidID;
				kData.m_iGameStoryInfoRaidID		= pkData->_lGameStoryRaidID;
				kData.m_iFirstLightTaskID			= pkData->_lFirstLightTaskID;
				int iTabType = pkData->_lTabType;
				m_kNormalIDWithIndexMap.insert(std::make_pair(pkData->_iID, GameStoryMapForVector(iTabType, m_kNormalGameStoryInfoList[iTabType].size())));
				m_kNormalGameStoryInfoList[iTabType].push_back(kData);
			}
			else if (pkData->_lTabType == ACTIVITY_TT_YUNBIAO)
			{
				kData.m_bValid					= true;
				kData.m_lStoryID				= pkData->_iID;
				kData.m_lActive					= 0;
				kData.m_lMinEnterLevel			= pkData->_iPlayerMinLevel;
				kData.m_lMaxEnterLevel			= pkData->_iPlayerMaxLevel;
				kData.m_lAlreadyEnteredTimes	= 0;
				kData.m_iAlreadyBuyTimes		= 0;
				kData.m_lAlreadyFinishedTimes	= 0;
				kData.m_bIsFinished				= false;
				kData.m_strTimeSlot				= "";
				kData.m_kCronTime				= pkData->_kCronStr;
				kData.m_iTimeLen				= pkData->_kTimeLen;
				kData.m_iContentType			= pkData->_kContentType;
				//kData.m_iTabType				= pkData->_lTabType;
				kData.m_iTabType				= ACTIVITY_TT_TIMELIMIT;
				kData.m_iType					= pkData->_lGameStoryType;

				///////UI显示相关，由于有两个不同的表，所以存储在这里
				kData.m_kClientShowName			= pkData->_kName;
				kData.m_iClientUIGroupRecommendNumber			= pkData->_iGroupRecommendNumber;
				kData.m_kClientUINotice			= pkData->_kNotice;
				kData.m_kClientUIReward				= pkData->_kReward;
				kData.m_kClientUIDescription			= pkData->_kDescription;
				kData.m_kClientUIPlayRule			= pkData->_kPlayRule;
				kData.m_kClientUIGameStoryRule			= pkData->_kGameStoryRule;
				kData.m_kClientUIResetTime			= pkData->_kResetTime;
				kData.m_kClientUIResetMap			= pkData->_kResetMap;
				kData.m_kClientUIStatisticsTip			= pkData->_kStatisticsTips;
				kData.m_kClientUIBackImage			= pkData->_BackImage;
				kData.m_kClientNameImage			= pkData->_NameImage;
				kData.m_kClientUITooltipReward			= pkData->_kTooltipReward;
				kData.m_kClientUITip			= pkData->_kTip;
				kData.m_kClientUIMapListText			= pkData->_kMapListText;
				kData.m_iCurRaidTID						= pkData->_lGameStoryRaidID;
				kData.m_iGameStoryInfoRaidID		= pkData->_lGameStoryRaidID;
				kData.m_iFirstLightTaskID			= pkData->_lFirstLightTaskID;
				int iTabType = pkData->_lTabType;
				m_kSpecialIDWithIndexMap.insert(std::make_pair(pkData->_iID, GameStoryMapForVector(iTabType, m_kSpecialGameStoryInfoList[iTabType].size())));
				m_kSpecialGameStoryInfoList[iTabType].push_back(kData);
			}

			iter.MoveNext();
		}
	}
	
	CF_GameStoryRaidList* pkCSVGameStoryRaidList = CF_GameStoryRaidList::GetSingletonPtr();
	if (pkCSVGameStoryRaidList)
	{
		CF_GameStoryRaidList::DataEntryMapExternIterator itear = pkCSVGameStoryRaidList->GetIterator();
		while (itear.HasMoreElements())
		{
			CF_GameStoryRaidList::DataEntry* pkData = itear.PeekNextValuePtr();

			GameStoryInfo* pkGameStoryInfo = GetGameStoryInfo(pkData->_lBelongStoryID);
			if (pkGameStoryInfo)
			{
				GameStoryRaidInfo kData;
				kData.m_lID					= pkData->_iID;
				kData.m_lBelongStoryID		= pkData->_lBelongStoryID;
				kData.m_lMapResID			= pkData->_MapResID;
				kData.m_kBossName			= pkData->_kName;
				kData.m_iContentType		= pkGameStoryInfo->GetContentType();
				kData.m_iType				= pkGameStoryInfo->GetType();
				pkGameStoryInfo->AddRaidInfo(kData);
			}

			itear.MoveNext();
		}
	}
	_ClearAndAddNormalGameStoryInfoList();
	//限次活动（组队副本特殊处理，采用CROSSREALMAP方案）
	for	(int32_t i = 0; i < GetGameStoryInfoCount(ACTIVITY_TT_NUMBERLIMIT); i++)
	{
		GameStoryInfo* pkGameStoryInfo = GetGameStoryInfoByIndex(ACTIVITY_TT_NUMBERLIMIT,i);
		if (pkGameStoryInfo)
		{
			int MapTypeID = pkGameStoryInfo->GetCurRaidTID();
			CF_CrossRealmMap::DataEntry *pkDataEntry = SafeGetCSVFileDataEntryPtr<CF_CrossRealmMap>(MapTypeID);
			if(pkDataEntry)
			{
				pkGameStoryInfo->m_lMaxEnteredTimes = pkDataEntry->_Limit;
			}
		}
	}
}

void LC_ClientGameStoryManager::AddGameStoryRevisionID(int iFlag, long iStoryID/* = 0*/)
{
	switch(iFlag)
	{
	case ACTIVITY_DIRTY_FLAG_UPDATE_LIST:
		m_lGameStoryInfoRevisionID++;
		break;
	case ACTIVITY_DIRTY_FLAG_GAME_STORY:
		{
			m_lGameStoryIDRevisionMap[iStoryID]++;
		}
		break;
	}
}

int LC_ClientGameStoryManager::GetGameStoryRevisionID(int iFlag, long iStoryID/* = 0*/)
{
	int nResult = 0;
	switch(iFlag)
	{
	case ACTIVITY_DIRTY_FLAG_UPDATE_LIST:
		nResult = m_lGameStoryInfoRevisionID;
		break;
	case ACTIVITY_DIRTY_FLAG_GAME_STORY:
		{
			nResult = m_lGameStoryIDRevisionMap[iStoryID];
		}
		break;
	}
	return nResult;
}

int	LC_ClientGameStoryManager::GetGameStoryInfoCount(int iType)
{
	int iCount = 0;
	T_ASSERT_CRITICAL_S(iType >= 0 && iType < ACTIVITY_TT_MAX);
	if (iType == ACTIVITY_TT_WILD_BOSS)
	{
		if (m_pkClientGameStoryInfoList[iType].size() > 0)
		{
			iCount = m_pkClientGameStoryInfoList[iType][0]->GetMapListCount();
		}
	}
	else
		iCount = m_pkClientGameStoryInfoList[iType].size();
	
	return iCount;
}

void LC_ClientGameStoryManager::UpdateGameStoryRaidPassed(int iType)
{
	T_ASSERT_CRITICAL_S(iType >= 0 && iType < ACTIVITY_TT_MAX);
	for (int i = 0; i < m_kNormalGameStoryInfoList[iType].size(); i++)
	{
		m_kNormalGameStoryInfoList[iType][i].UpdateRaidPassed();
	}
}

int LC_ClientGameStoryManager::GetGameStoryTodayFinisnedStoryNum(int iTabType)
{
	return m_iTodayFinishedStoryNum[iTabType];
}

int LC_ClientGameStoryManager::GetGameStoryTodayActiveStoryNum(int iTabType)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (!pkGamePlayer)
		return 0;
	//int iCurLevel = pkGamePlayer->GetLevel();
	int iCurLevel = pkGamePlayer->GetCommanderLevel();

	int iCount = 0;

	if (iTabType < 0 || iTabType >= ACTIVITY_TT_MAX)
		return iCount;

	for (int i = 0; i < m_kNormalGameStoryInfoList[iTabType].size(); i++)
	{
		const GameStoryInfo& kGameStory = m_kNormalGameStoryInfoList[iTabType][i];
		if (kGameStory.m_bIsFinished)
		{
			iCount++;
		}
		else if (kGameStory.m_iTimeLen < 0 || IsTimeToday(kGameStory.m_iTimeBegin))
		{
			//判断等级
			int iMinLevel = kGameStory.GetEnterMinLevel();
			int iMaxLevel = kGameStory.GetEnterMaxLevel();
			if (iCurLevel >= iMinLevel && iCurLevel <= iMaxLevel)
				iCount++;
		}
	}

	return iCount;
}

void LC_ClientGameStoryManager::OnMapChanged(int lMapResID)
{
	bool bShowImmediate = true;
	LC_GameEvent kGameEvent;
	
	if (lMapResID >= 1701&&lMapResID <= 1711)
	{
		lMapResID = 1701;
	}

	long lStoryID = GetActiveStoryID(lMapResID);
	if (lStoryID == -1)
	{
		kGameEvent.SetEventType(GLET_CLIENT_HIDE_ACTIVITY_STATISTICS);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, -1);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_2,GSI_GSSOAHT_MAPCHANGE);
	}
	else
	{
		kGameEvent.SetEventType(GLET_CLIENT_SHOW_ACTIVITY_STATISTICS);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, lStoryID);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_2,GSI_GSSOAHT_MAPCHANGE);

		GameStoryInfo* pkGameStoryInfo = GetGameStoryInfo(lStoryID);
		if(pkGameStoryInfo)
		{
			pkGameStoryInfo->m_iCurAreaIndex = -1;
			pkGameStoryInfo->m_iCurAreaState = -1;
			if (pkGameStoryInfo->GetMapAreaListCount() > 0)
				bShowImmediate = false;
		}
	}


	/*kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, bShowImmediate);*/
	if (bShowImmediate)
	{
		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
}

long LC_ClientGameStoryManager::GetActiveStoryID(int lMapResID)
{
	for (int iType = 0; iType < ACTIVITY_TT_MAX; iType++)
	{
		int iSize = m_kNormalGameStoryInfoList[iType].size();
		for (int i = 0; i < iSize; i++)
		{
			GameStoryInfo& kGameStoryInfo = m_kNormalGameStoryInfoList[iType][i];
			if (!CanJoin(kGameStoryInfo, lMapResID))
				continue;
			if (_isMapResIDInGameStory(kGameStoryInfo, lMapResID))
				return kGameStoryInfo.m_lStoryID;
			
		}
	}
	return -1;
}
//重置活动状态
void LC_ClientGameStoryManager::Reset()
{	
	for (int iType = 0; iType < ACTIVITY_TT_MAX; iType++)
	{
		int iSize = m_kNormalGameStoryInfoList[iType].size();
		for (int i = 0; i < iSize; i++)
		{
			GameStoryInfo& kGameStoryInfo = m_kNormalGameStoryInfoList[iType][i];
			kGameStoryInfo.ResetActive();
		}
	}
	m_TargetGameStoryID = 0;
	m_iTargetPosX = 0;
	m_iTargetPosY = 0;
	m_iDirectionFxID = 0;
}
bool LC_ClientGameStoryManager::_isMapResIDInGameStory(const GameStoryInfo& kGameStoryInfo, int lMapResID)
{
	if (lMapResID <= 0)
		return false;

	if (kGameStoryInfo.pkConfig)
	{
		MapCtrlSetting &kMapCtrlSetting = kGameStoryInfo.pkConfig->m_kMapCtrlSetting;
		GSResIDList& kMapResIDs = kMapCtrlSetting.m_kMapResIDList;
		GSResIDList::iterator itMID = kMapResIDs.begin();
		for(; itMID!=kMapResIDs.end(); ++itMID)
		{
			if (*itMID == lMapResID)
				return true;
		}
	}
	if (kGameStoryInfo.m_iGameStoryInfoRaidID == lMapResID)
		return true;

	return false;
}

bool LC_ClientGameStoryManager::CanJoin(const GameStoryInfo& kGameStoryInfo)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (!pkGamePlayer)
		return false;
	return pkGamePlayer->CheckGameStoryCondition(kGameStoryInfo);
}

bool LC_ClientGameStoryManager::CanJoin(const GameStoryInfo& kGameStoryInfo, int iMapID)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (!pkGamePlayer)
		return false;
	return pkGamePlayer->CheckGameStoryCondition(kGameStoryInfo, iMapID);
}

void LC_ClientGameStoryManager::_updateGameStoryState(const StoryStateMap & rkSSMap)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (!pkGamePlayer)
		return;

	int lCurMapResID = pkGamePlayer->GetMapResID();

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();

	StoryStateMap::const_iterator it = rkSSMap.begin();
	for (; it != rkSSMap.end(); ++it)
	{
		if (it->second == -1)
		{
			LC_GameEvent kGameEvent;
			kGameEvent.SetEventType(GLET_CLIENT_HIDE_ACTIVITY_STATISTICS);
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, it->first);
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_2,GSI_GSSOAHT_NORMAL);
			
			pkUIManager->HandlerGameEvent(kGameEvent);
		}
		else if (it->second == 1)
		{	
			GameStoryInfo* pkGameStoryInfo = GetGameStoryInfo(it->first);
			if(!pkGameStoryInfo)
				continue;
			if (!_isMapResIDInGameStory(*pkGameStoryInfo, lCurMapResID) && pkGameStoryInfo->GetTabType() != ACTIVITY_TT_TIMELIMIT)//限时活动的特殊处理
				continue;
			LC_GameEvent kGameEvent;
			kGameEvent.SetEventType(GLET_CLIENT_SHOW_ACTIVITY_STATISTICS);
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, it->first);
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_2,GSI_GSSOAHT_NORMAL);

			pkUIManager->HandlerGameEvent(kGameEvent);
		}
		//_IsGameStoryTypeSpecial
		//碰到内置UI不出现的情况记得改这里
		else if (it->second == 0)
		{
			GameStoryInfo* pkGameStoryInfo = GetGameStoryInfo(it->first);
			if(!pkGameStoryInfo)
				continue;
			if (!_IsGameStoryTypeSpecial(*pkGameStoryInfo) )
				continue;
			if (!_isMapResIDInGameStory(*pkGameStoryInfo, lCurMapResID))
				continue;
			if (!CanJoin(*pkGameStoryInfo))
				continue;
			LC_GameEvent kGameEvent;
			kGameEvent.SetEventType(GLET_CLIENT_SHOW_ACTIVITY_STATISTICS);
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, it->first);
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_2,GSI_GSSOAHT_NORMAL);
			pkUIManager->HandlerGameEvent(kGameEvent);
		}
	}
}

void LC_ClientGameStoryManager::UpdateMapAreaInfo(int lMapResID, int iAreaIndex, int iState)
{
	CF_WorldMapList::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(lMapResID);
	if (!pkData)
		return ;
	GameStoryInfo* pkGameStoryInfo = GetActiveGameStoryInfoByTrueID(pkData->_iStoryID);
	//GameStoryInfo* pkGameStoryInfo = GetGameStoryInfo(pkData->_iStoryID);
	if (!pkGameStoryInfo)
		return ;

	bool bEnterNextArea = false;
	if (pkGameStoryInfo->m_iCurAreaIndex != iAreaIndex && pkGameStoryInfo->m_iCurAreaIndex >= 0 && iAreaIndex >= 0 && iAreaIndex < pkGameStoryInfo->GetMapAreaListCount() - 1)
		bEnterNextArea = true;

	pkGameStoryInfo->m_iCurAreaIndex = iAreaIndex;
	pkGameStoryInfo->m_iCurAreaState = iState;

	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	if(pkLogicManger)
	{
		LC_ClientGamePlayer* pkClientPlayer = pkLogicManger->GetOperationPlayer();	
		if(pkClientPlayer)
		{
			if (pkClientPlayer->GetCurrentMapID() == lMapResID)
			{
				AddGameStoryRevisionID(ACTIVITY_DIRTY_FLAG_UPDATE_LIST);

				if(pkGameStoryInfo->GetStroyID() == GetActiveStoryID(lMapResID))
				{
					LC_GameEvent kGameEvent;
					kGameEvent.SetEventType(GLET_CLIENT_SHOW_ACTIVITY_STATISTICS);
					kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, pkGameStoryInfo->GetStroyID());
					kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, true);
					kGameEvent.SetEventParam(EVENT_BASE_PARAM_3, bEnterNextArea);
					UI_UIManager::GetSingletonPtr()->HandlerGameEvent(kGameEvent);
				}
			}
		}
	}
}

void LC_ClientGameStoryManager::_ClearAndAddNormalGameStoryInfoList()
{
	for (int iType = 0; iType < ACTIVITY_TT_MAX; iType++)
	{
		m_pkClientGameStoryInfoList[iType].clear();
		int iSize = m_kNormalGameStoryInfoList[iType].size();
		for (int i = 0; i < iSize; i++)
		{
			GameStoryInfo *kGameStoryInfo = &m_kNormalGameStoryInfoList[iType][i];
			m_pkClientGameStoryInfoList[iType].push_back(kGameStoryInfo);
		}
		///BUG #10567 【建议】【运营反馈】在活动里加一个双倍护送推送图标
		///ACTIVITY_TT_YUNBIAO类型需要增加进ACTIVITY_TT_TIMELIMIT中
		if (iType == ACTIVITY_TT_YUNBIAO)
		{
			iSize = m_kSpecialGameStoryInfoList[iType].size();
			for (int i = 0; i < iSize; i++)
			{
				GameStoryInfo *kGameStoryInfo = &m_kSpecialGameStoryInfoList[iType][i];
				m_pkClientGameStoryInfoList[ACTIVITY_TT_TIMELIMIT].push_back(kGameStoryInfo);
			}
		}
		else if (iType == ACTIVITY_TT_TIMELIMIT_EX)
		{
			iSize = m_kNormalGameStoryInfoList[iType].size();
			for (int i = 0; i < iSize; i++)
			{
				GameStoryInfo *kGameStoryInfo = &m_kNormalGameStoryInfoList[iType][i];
				m_pkClientGameStoryInfoList[ACTIVITY_TT_TIMELIMIT].push_back(kGameStoryInfo);
			}
		}
	}
	//试炼活动特殊排序
	std::sort(m_pkClientGameStoryInfoList[ACTIVITY_TT_TRIAL].begin(),m_pkClientGameStoryInfoList[ACTIVITY_TT_TRIAL].end(),_sortByLevelGameStoryList);
	//BUG #14490 【BUG】【活动相关】副本排列按照等级排列
	std::sort(m_pkClientGameStoryInfoList[ACTIVITY_TT_NUMBERLIMIT].begin(),m_pkClientGameStoryInfoList[ACTIVITY_TT_NUMBERLIMIT].end(),_sortByLevelGameStoryList);
}


//现实类的互动针对开始时间的排序
bool LC_ClientGameStoryManager::_sortTimeLimitGameStoryList(GameStoryInfo *a,GameStoryInfo *b)
{
	bool aTimePass = a->IsTimePassToday();
	bool bTimePass = b->IsTimePassToday();
	if (aTimePass && !bTimePass)
	{
		return false;
	}
	else if (bTimePass && !aTimePass)
	{
		return true;
	}
	//只有都是ACTIVITY_TT_TIMELIMIT 且未开启的留了下来
	time_t at = time_t(a->m_iCSVOpenTime);
	time_t bt = time_t(b->m_iCSVOpenTime);

	tm aLT = *localtime(&at);
	tm bLT = *localtime(&bt);

	//由于活动结束后服务器会把第二天的时间发下来，因此不能简单的比较时间戳，这里比较小时和分钟
	if (aLT.tm_hour < bLT.tm_hour)
	{
		return true;
	}
	else if (aLT.tm_hour > bLT.tm_hour)
	{
		return false;
	}
	else
	{
		if (aLT.tm_min < bLT.tm_min)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool LC_ClientGameStoryManager::IsMapResIDInGameStory( long lGameStoryID )
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (!pkGamePlayer)
		return false;

	int lCurMapResID = pkGamePlayer->GetMapResID();
	const GameStoryInfo *pkGameStoryInfo = GetGameStoryInfo(lGameStoryID);
	if (!pkGameStoryInfo)
		return false;
	return _isMapResIDInGameStory(*pkGameStoryInfo,lCurMapResID);
}

void LC_ClientGameStoryManager::_FilterCronTimestampSameDay( const StringType& cronStr, std::vector<uint32_t>& vecTime )
{
	if (cronStr.empty())
	{
		return;
	}
	Utility::UT_CronData kCron;

	if(kCron.ParseCronStr(cronStr))
	{
		LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
		LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
		uint64_t iSystemTime = 0;
		if (!pkGamePlayer)
			iSystemTime = GET_SYSTEM_TIME;
		else
			iSystemTime = pkGamePlayer->GetServerTimeVal();
		time_t cur = (time_t)(iSystemTime);
		tm curLT = *localtime ( &cur );
		tm todayLT = curLT;
		if (iSystemTime != 0)
		{
			todayLT.tm_hour = 0;
			todayLT.tm_min = 0;
			todayLT.tm_sec = 1;
		}
		time_t today = mktime(&todayLT);
		time_t p = today;

		do 
		{
			time_t t = kCron.GetNextTime(p);

			if (!LC_Helper::CheckIsSameDay(curLT, (uint32_t)t))
			{
				break;
			}

			vecTime.push_back((uint32_t)t);
			p = t + 1;

		} while (1);
	}
}

int LC_ClientGameStoryManager::GetGameStoryTabType( long lGameStoryID )
{
	GameStoryInfo*	pkStoryInfo = GetGameStoryInfo(lGameStoryID);
	if (pkStoryInfo)
		return pkStoryInfo->m_iTabType;
	return 0;

}

bool gamestory_key_comp(const PS_GAMESTORY_KEY_INFO& rs,const PS_GAMESTORY_KEY_INFO& rh)
{
	return rs.m_uiTimestamp > rh.m_uiTimestamp;
}

void LC_ClientGameStoryManager::ReceiveGameStoryBatchInfosync( Protocol::MG_GAMESTORY_BATCH_INFO_SYNC& pkMsg )
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (!pkGamePlayer)
		return;

	Protocol::GAMESTORY_BATCH_INFO &kStoryBatchInfo = pkMsg.m_kGameStoryBatch;

	long iStoryID = kStoryBatchInfo.m_uiStoryID;
	if (m_kNormalIDWithIndexMap.find(iStoryID) == m_kNormalIDWithIndexMap.end())
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR ,"The DesignCSV GameStory and GameStoryClientUI Not Match, StoryID = %d", iStoryID);
		return;
	}
	const GameStoryMapForVector& kMappingData = m_kNormalIDWithIndexMap[iStoryID];
	GameStoryInfo& kStoryInfo = m_kNormalGameStoryInfoList[kMappingData.m_iType][kMappingData.m_iIndex];
	
	if (kStoryInfo.m_iTabType != ACTIVITY_TT_TIMELIMIT)
	{
		Protocol::GAMESTORY_BATCH_INFO	&kBatchInfo = kStoryInfo.m_kBatchInfo;
		kBatchInfo.m_uiFlag = kStoryBatchInfo.m_uiFlag;
		kBatchInfo.m_uiStoryID = kStoryBatchInfo.m_uiStoryID;
		kBatchInfo.m_uiClearTime = kStoryBatchInfo.m_uiClearTime;
		kBatchInfo.m_uiInitTime = kStoryBatchInfo.m_uiInitTime;
		kBatchInfo.m_kDetails = kStoryBatchInfo.m_kDetails;
		kBatchInfo.m_kAreaInfos.clear();
		for (int i = 0; i < kStoryBatchInfo.m_kAreaInfos.size(); ++i)
		{
			kBatchInfo.m_kAreaInfos.push_back(kStoryBatchInfo.m_kAreaInfos[i]);
			std::sort(kBatchInfo.m_kAreaInfos[i].m_kKeyInfos.begin(), kBatchInfo.m_kAreaInfos[i].m_kKeyInfos.end(), gamestory_key_comp);
		}
		kBatchInfo.m_kMonExtraSkills.clear();
		std::set<int32_t>::iterator pkSkills = kStoryBatchInfo.m_kMonExtraSkills.begin();
		for (; pkSkills!=kStoryBatchInfo.m_kMonExtraSkills.end(); ++pkSkills)
		{
			kBatchInfo.m_kMonExtraSkills.insert(*pkSkills);
		}
	}
	else
	{
		std::vector<uint32_t> kCronTime;
		_FilterCronTimestampSameDay(kStoryInfo.m_kCronTime,kCronTime);
		for (int j = 0; j < kCronTime.size(); j++)
		{
			int iSecondStory = kStoryInfo.m_lStoryID + 1000 * j;
			if (m_kNormalIDWithIndexMap.find(iSecondStory) == m_kNormalIDWithIndexMap.end())
			{
				continue;
			}
			const GameStoryMapForVector& kMappingData = m_kNormalIDWithIndexMap[iSecondStory];
			GameStoryInfo& SecondkStoryInfo = m_kNormalGameStoryInfoList[kMappingData.m_iType][kMappingData.m_iIndex];

			Protocol::GAMESTORY_BATCH_INFO	&kBatchInfo = SecondkStoryInfo.m_kBatchInfo;
			kBatchInfo.m_uiFlag = kStoryBatchInfo.m_uiFlag;
			kBatchInfo.m_uiStoryID = kStoryBatchInfo.m_uiStoryID;
			kBatchInfo.m_uiClearTime = kStoryBatchInfo.m_uiClearTime;
			kBatchInfo.m_uiInitTime = kStoryBatchInfo.m_uiInitTime;
			kBatchInfo.m_kDetails = kStoryBatchInfo.m_kDetails;
			kBatchInfo.m_kAreaInfos.clear();
			for (int i = 0; i < kStoryBatchInfo.m_kAreaInfos.size(); ++i)
			{
				kBatchInfo.m_kAreaInfos.push_back(kStoryBatchInfo.m_kAreaInfos[i]);
				if(GetGameStoryType(iStoryID) !=  GAMESTORY_TYPE_ZangLongMoXue) //藏龙魔穴Boss不要排序
				{
					std::sort(kBatchInfo.m_kAreaInfos[i].m_kKeyInfos.begin(), kBatchInfo.m_kAreaInfos[i].m_kKeyInfos.end(), gamestory_key_comp);
				}
			}
		}
	}
	Protocol::PS_GAMESTORY_BATCH_DETAIL* pkDetail = kStoryInfo.GetBatchDetail();
	uint32_t iSeqIdx = pkDetail ? pkDetail->m_uiSeqIdx : 0;
	uint32_t iSeqSize = kStoryInfo.pkConfig ? kStoryInfo.pkConfig->m_kBatchSechedule.SeqSize() : 0;
	if (IsMapResIDInGameStory(kStoryInfo.GetStroyID()) && (kStoryInfo.IsFailed() || iSeqIdx >= iSeqSize))
	{	
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_CLIENT_GAMESTORY_FAIL);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, kStoryInfo.GetStroyID());
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, kStoryInfo.GetType());
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_3, iSeqIdx);
		//kGameEvent.SetEventParam(EVENT_BASE_PARAM_3, bEnterNextArea);
		UI_UIManager::GetSingletonPtr()->HandlerGameEvent(kGameEvent);
	}
	AddGameStoryRevisionID(ACTIVITY_DIRTY_FLAG_GAME_STORY, iStoryID);
	AddGameStoryRevisionID(ACTIVITY_DIRTY_FLAG_UPDATE_LIST);
}

void LC_ClientGameStoryManager::ActiveStory(Utility::timer_id_type timerID,  int storyID )
{
	LC_ClientGameStoryManager *pkGameStoryManager = CLIENT_GET_GAMESTORY_MANAGER();
	GameStoryInfo *pkGameStory = pkGameStoryManager->GetGameStoryInfo(storyID);
	if (pkGameStory)
	{
		StoryStateMap	kGameStoryStateMap;
		int oldCanJoin = pkGameStoryManager->CanJoin(*pkGameStory);
		pkGameStory->m_lActive = 1;
		kGameStoryStateMap[storyID] = pkGameStoryManager->CanJoin(*pkGameStory) - oldCanJoin;
		pkGameStoryManager->_updateGameStoryState(kGameStoryStateMap);
		pkGameStoryManager->AddGameStoryRevisionID(ACTIVITY_DIRTY_FLAG_UPDATE_LIST);
	}
	
}

void LC_ClientGameStoryManager::DeactiveStory(Utility::timer_id_type timerID,  int storyID )
{
	LC_ClientGameStoryManager *pkGameStoryManager = CLIENT_GET_GAMESTORY_MANAGER();
	GameStoryInfo *pkGameStory = pkGameStoryManager->GetGameStoryInfo(storyID);
	if (pkGameStory)
	{
		StoryStateMap	kGameStoryStateMap;
		int oldCanJoin = pkGameStoryManager->CanJoin(*pkGameStory);
		pkGameStory->m_lActive = 0;
		kGameStoryStateMap[storyID] = pkGameStoryManager->CanJoin(*pkGameStory) - oldCanJoin;
		pkGameStoryManager->_updateGameStoryState(kGameStoryStateMap);
		pkGameStoryManager->AddGameStoryRevisionID(ACTIVITY_DIRTY_FLAG_UPDATE_LIST);
	}
}

void LC_ClientGameStoryManager::ReceiveCrossRealmTeamAssetSync(GameLogic::LC_CrossRealmTeamAsset *pkCrossRealTeamAsset)
{
	bool bChangeFlag = false;
	for (int j = 0; j < GetGameStoryInfoCount(ACTIVITY_TT_NUMBERLIMIT); j++)
	{
		GameStoryInfo* pkGameStoryInfo = GetGameStoryInfoByIndex(ACTIVITY_TT_NUMBERLIMIT,j);
		if (pkGameStoryInfo)
		{
			int iOldDailyTime = pkGameStoryInfo->m_lAlreadyEnteredTimes;
			int iDailyTime = pkCrossRealTeamAsset->GetDailyMapTime(pkGameStoryInfo->GetGameStoryInfoRaidID());
			pkGameStoryInfo->m_lAlreadyEnteredTimes = iDailyTime;
			if (iDailyTime != iOldDailyTime)
				bChangeFlag = true;
		}
	}
	if (bChangeFlag)
	{
		AddGameStoryRevisionID(ACTIVITY_DIRTY_FLAG_UPDATE_LIST);
	}
}

bool LC_ClientGameStoryManager::_sortByLevelGameStoryList(GameStoryInfo *a,GameStoryInfo *b)
{
	if (a->GetEnterMinLevel() <= b->GetEnterMinLevel())
	{
		return true;
	}
	else
	{
		return false;
	}
}

long LC_ClientGameStoryManager::GetGameStoryIDBylMapResID(int lMapResID)
{
	for (int iType = 0; iType < ACTIVITY_TT_MAX; iType++)
	{
		int iSize = m_pkClientGameStoryInfoList[iType].size();
		for (int i = 0; i < iSize; i++)
		{
			GameStoryInfo *kGameStoryInfo = m_pkClientGameStoryInfoList[iType][i];
			if (_isMapResIDInGameStory(*kGameStoryInfo, lMapResID))
				return kGameStoryInfo->m_lStoryID;
		}
	}
	return -1;
}

void LC_ClientGameStoryManager::ResetNumberLimitRemainTimes()
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (!pkGamePlayer)
		return;

	ReceiveCrossRealmTeamAssetSync(pkGamePlayer->GetCrossRealmTeamAssert());
}

void LC_ClientGameStoryManager::ReceiveDataFormServer(const MG_RLT_GetBuyTimes& rltMsg)
{
	mem::map<int, GameStoryVIPChallengeInfo>::iterator Iter = m_kClientGameStoryVIPInfo.find(rltMsg.m_iGameStoryId);
	if (Iter == m_kClientGameStoryVIPInfo.end())
	{
		GameStoryVIPChallengeInfo VIPInfo(rltMsg.m_iGameStoryId, rltMsg.m_buyTimes, rltMsg.m_totalTimes, rltMsg.m_costType,  rltMsg.m_costCount);
		m_kClientGameStoryVIPInfo[rltMsg.m_iGameStoryId] = VIPInfo;
	}
	else
	{
		GameStoryVIPChallengeInfo *VIPInfo = &(Iter->second);
		VIPInfo->m_iGameStoryID = rltMsg.m_iGameStoryId;
		VIPInfo->m_iBuyTimes = rltMsg.m_buyTimes;
		VIPInfo->m_iTotalTimes = rltMsg.m_totalTimes;
		VIPInfo->m_iCostType = rltMsg.m_costType;
		VIPInfo->m_iCostCount = rltMsg.m_costCount;

	}
}

int LC_ClientGameStoryManager::GetVIPBuyTimesByStoryID(int iGameStoryID)
{
	mem::map<int, GameStoryVIPChallengeInfo>::iterator Iter = m_kClientGameStoryVIPInfo.find(iGameStoryID);
	if (Iter == m_kClientGameStoryVIPInfo.end())
	{
		return 0;
	}
	return Iter->second.m_iBuyTimes;
}


int LC_ClientGameStoryManager::GetVIPTotalTimesByStoryID(int iGameStoryID)
{
	mem::map<int, GameStoryVIPChallengeInfo>::iterator Iter = m_kClientGameStoryVIPInfo.find(iGameStoryID);
	if (Iter == m_kClientGameStoryVIPInfo.end())
	{
		return 0;
	}
	return Iter->second.m_iTotalTimes;
}

int LC_ClientGameStoryManager::GetVIPCostTimesByStoryID(int iGameStoryID)
{
	mem::map<int, GameStoryVIPChallengeInfo>::iterator Iter = m_kClientGameStoryVIPInfo.find(iGameStoryID);
	if (Iter == m_kClientGameStoryVIPInfo.end())
	{
		return 0;
	}
	return Iter->second.m_iCostCount;
}

void LC_ClientGameStoryManager::_updateGameStoryYunBiao()
{
	_ClearAndAddNormalGameStoryInfoList();
	int iSize = m_kSpecialGameStoryInfoList[ACTIVITY_TT_YUNBIAO].size();
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (!pkGamePlayer)
		return;
	int64_t iGameSeverTime = pkGamePlayer->GetServerTimeVal();
	for (int i = 0; i < iSize; i++)
	{
		GameStoryInfo *pkGameStoryInfo = &m_kSpecialGameStoryInfoList[ACTIVITY_TT_YUNBIAO][i];
		std::vector<uint32_t> kCronTime;
		_FilterCronTimestampSameDay(pkGameStoryInfo->m_kCronTime,kCronTime);
		for (int j = 0; j < kCronTime.size(); j++)
		{
			pkGameStoryInfo->m_iCSVOpenTime = kCronTime[j];
			pkGameStoryInfo->m_iCSVEndTime = pkGameStoryInfo->m_iCSVOpenTime + pkGameStoryInfo->m_iTimeLen * 60;
			if (pkGameStoryInfo->m_iCSVOpenTime - 10 < iGameSeverTime  && pkGameStoryInfo->m_iCSVEndTime > iGameSeverTime)
			{
				pkGameStoryInfo->m_lActive = 1;
			}
			else
			{
				pkGameStoryInfo->m_lActive = 0;
			}
			UT_TimerManager *pkTimerMgr = UT_TimerManager::GetSingletonPtr();
			if (pkGameStoryInfo->m_iStartTimerID > 0)
			{
				pkTimerMgr->CancelTimer(pkGameStoryInfo->m_iStartTimerID);
				pkGameStoryInfo->m_iStartTimerID = 0;
			}
			int iOpenTimeDelay = pkGameStoryInfo->m_iCSVOpenTime - iGameSeverTime;
			if (iOpenTimeDelay > 0)
			{
				pkGameStoryInfo->m_iStartTimerID = pkTimerMgr->DelayCall((int64_t)iOpenTimeDelay * 1000, LC_ClientGameStoryManager::ActiveStory, (void (*)(Utility::timer_id_type, int))NULL, (int)pkGameStoryInfo->m_lStoryID);
			}

			if (pkGameStoryInfo->m_iEndTimerID > 0)
			{
				pkTimerMgr->CancelTimer(pkGameStoryInfo->m_iEndTimerID);
				pkGameStoryInfo->m_iEndTimerID = 0;
			}
			int iCloseTimeDelay =  pkGameStoryInfo->m_iCSVEndTime - iGameSeverTime;
			if (iCloseTimeDelay > 0)
			{
				pkGameStoryInfo->m_iEndTimerID = pkTimerMgr->DelayCall((int64_t)iCloseTimeDelay * 1000, LC_ClientGameStoryManager::DeactiveStory, (void (*)(Utility::timer_id_type, int))NULL, (int)pkGameStoryInfo->m_lStoryID);
			}
			char strTemp[MAX_GAMESTORY_TIME_DESCRIPTION_LENGTH + 1];
			time_t bt = time_t(pkGameStoryInfo->m_iCSVOpenTime);
			time_t et = time_t(pkGameStoryInfo->m_iCSVEndTime);

			tm bLT = *localtime(&bt);
			tm bET = *localtime(&et);

			sprintf_s(strTemp,sizeof(strTemp),"%d:%02d - %d:%02d",bLT.tm_hour, bLT.tm_min, bET.tm_hour, bET.tm_min);
			pkGameStoryInfo->m_strTimeSlot	= strTemp;
		}
	}

}

//这里主要针对m_kGameStoryStateMap中为0的情况
//针对一些活动内置UI不出现的问题，尤其是跨服活动
bool LC_ClientGameStoryManager::_IsGameStoryTypeSpecial(GameStoryInfo& kGameStoryInfo)
{
	int StoryType = kGameStoryInfo.GetType();
	if (StoryType == GAMESTORY_TYPE_LanJieTianYingJiao || StoryType == GAMESTORY_TYPE_ZangLongMoXue
		|| StoryType == GAMESTORY_TYPE_BangHuiYunBiao || StoryType == GAMESTORY_TYPE_YouLinMiJing
		|| StoryType == GAMESTORY_TYPE_MiJing || StoryType == GAMESTORY_TYPE_Chest
		|| StoryType == GAMESTORY_TYPE_PK || StoryType == GAMESTORY_TYPE_JiXianMoShi ||StoryType == GAMESTORY_TYPE_BianJingYunBiao || StoryType == GAMESTORY_TYPE_KUAFU_BOSS || StoryType == GAMESTORY_TYPE_LOULANBAOZANG
		|| StoryType == GAMESTORY_TYPE_SKYARENA)
	{
		return true;
	}
	else
	{
		return false;
	}
}

GameStoryInfo* LC_ClientGameStoryManager::GetActiveGameStoryInfoByTrueID(long lStoryID)
{
	mem::map<long, GameStoryMapForVector> &IDWithIndexMap = m_kNormalIDWithIndexMap;
	mem::vector<GameStoryInfo> *GameStoryInfoList = m_kNormalGameStoryInfoList;
	if (lStoryID >= GAMESTORY_TYPE_GameStory && lStoryID < GAMESTORY_TYPE_TEAM_RAID)
	{
		IDWithIndexMap = m_kSpecialIDWithIndexMap;
		GameStoryInfoList = m_kSpecialGameStoryInfoList;
	}
	if (IDWithIndexMap.find(lStoryID) == IDWithIndexMap.end())
		return NULL;
	int j = 0;
	do 
	{
		int iSecondStory = lStoryID + 1000 * j;
		if (IDWithIndexMap.find(iSecondStory) == IDWithIndexMap.end())
		{
			break;
		}
		GameStoryMapForVector& kMappingData = IDWithIndexMap[iSecondStory];
		GameStoryInfo& kStoryInfo = GameStoryInfoList[kMappingData.m_iType][kMappingData.m_iIndex];
		if (kStoryInfo.GetActive() != 0)
		{
			return &kStoryInfo;
		}
		j++;
	} while (1);
	return NULL;
}

void  LC_ClientGameStoryManager::GetDataFromProtocolStruct(Protocol::MG_GAMESTORY_RANK_INFO_SYNC& rkMsg)
{
	m_kRanks = rkMsg.kRanks;
}

bool LC_ClientGameStoryManager::IsBatchBeginByGameStoryID(long lGameStoryID)
{
	GameStoryInfo *pkGameStoryInfo = GetGameStoryInfo(lGameStoryID);
	if (!pkGameStoryInfo)
		return false;
	return pkGameStoryInfo->GetBasicStatInfoCount(GameStoryInfo::GSI_BST_BATCH_INDEX) >= 0;
}
