#include "ClientPCH.h"
#include "LC_AchievementManager.h"
#include "CF_AchievementList.h"
#include "CF_AchievementRewards.h"
#include "LC_AchievementAsset_New.h"
#include "LC_TitleManager.h"

using namespace GameLogic;

LC_AchievementManager::LC_AchievementManager()
{

}

LC_AchievementManager::~LC_AchievementManager()
{

}

void LC_AchievementManager::Init()
{
	
	CSVFile::CF_AchievementList* pkAchievementList = CSVFile::CF_AchievementList::GetSingletonPtr();
	if(NULL == pkAchievementList)
		return;
	CSVFile::CF_AchievementList::DataEntryMapExternIterator kIter = pkAchievementList->GetIterator();
	while(kIter.HasMoreElements())
	{
		const CSVFile::CF_AchievementList::DataEntry* pkData = kIter.PeekNextValuePtr();
		if (pkData->_iAchievementType==2)
		{
			AchievementIdMap[pkData->_iSubclass][pkData->_lID]=pkData;
		}
		kIter.MoveNext();
	}
}

void LC_AchievementManager::ResetData()
{
	
}

int32_t LC_AchievementManager::GetAchieveProgress(GameLogic::LC_PlayerBase* pkPlayer, int iID)
{
	if(NULL == pkPlayer)
		return -1;

	CAchievementAsset& pkAsset = pkPlayer->GetAchievementAsset();
	
	return pkAsset.GetAchievementProcessing(iID);
}

bool LC_AchievementManager::IsFinishedAchieve(GameLogic::LC_PlayerBase* pkPlayer, int iID)
{
	if(NULL == pkPlayer)
		return false;

	CAchievementAsset& pkAsset = pkPlayer->GetAchievementAsset();

	return pkAsset.IsInFinishAchievementRecord(iID);
}

StringType LC_AchievementManager::GetAchievePointInfo()
{
	StringStreamType sStream("");
	CF_AchievementRewards::DataEntryMapExternIterator kIter = CF_AchievementRewards::GetSingletonPtr()->GetIterator();
	while(kIter.HasMoreElements())
	{
		const CF_AchievementRewards::DataEntry* pkData = kIter.PeekNextValuePtr();
		sStream << pkData->_lID << "#" << pkData->_iAchievementPoint << "#" << pkData->_sGiftInfos << "#";
		sStream << pkData->_sDescription << "#" << pkData->_CharTypeID << "#" << pkData->_Position << "|";
		kIter.MoveNext();
	}

	return sStream.str();
}

StringType LC_AchievementManager::GetAchieveSevenDaysTotalReward(GameLogic::LC_PlayerBase* pkPlayer)
{
	if(NULL == pkPlayer)
		return "";

	CAchievementAsset& pkAsset = pkPlayer->GetAchievementAsset();

	StringStreamType sStream("");
	const mem::set<int32_t>& kRwdVector = pkAsset.GetSevenDayTrialRewordIDs();
	mem::set<int32_t>::const_iterator it = kRwdVector.begin();
	for(; it!=kRwdVector.end(); ++it)
	{
		sStream << (*it) << "#";
	}

	return sStream.str();
}

StringType LC_AchievementManager::GetTaskTraceAchievement(GameLogic::LC_PlayerBase* pkPlayer)
{
	if(NULL == pkPlayer)
		return "";

	CAchievementAsset& pkAsset = pkPlayer->GetAchievementAsset();

	mem::map<int32_t,AchievementListMap>::const_iterator itemIter = AchievementIdMap.begin();
	StringStreamType sStream("");

	for(; itemIter != AchievementIdMap.end(); ++itemIter)
	{
		const AchievementListMap& dataMap=itemIter->second;
		AchievementListMap::const_iterator it2 = dataMap.begin();
		for(; it2 != dataMap.end(); ++it2)
		{
			const CSVFile::CF_AchievementList::DataEntry* pkData = it2->second;
			sStream << pkData->_lID << "@" << pkData->_sAchievementinfo << "@" << pkData->_iAchievementPoint << "@";
			sStream << pkData->_iConditionThreshold << "@" << pkData->_iLinkID << "@" << pkData->_CharLevel << "@";
			sStream << pkData->_kPreTasks << "@";
			sStream << pkData->_kPreAchievements <<"@";
			sStream << pkData->_sRewardItems << "@" << 0 << "@" << pkData->_iAchievementType << "@";
			sStream << pkData->_iAchievementName << "@" <<pkData->_iSubclass<< "#";
		}
	}
	return sStream.str();
}

int64_t LC_AchievementManager::GetItemCondition(int iID)
{
	CSVFile::CF_AchievementList::DataEntry *pkEntry = CSVFile::SafeGetCSVFileDataEntryPtr<CSVFile::CF_AchievementList>(iID);
	if (pkEntry != NULL)
	{
		return pkEntry->_iConditionThreshold;
	}
	return -1;
}

long LC_AchievementManager::GetLinkID(int iID)
{
	CSVFile::CF_AchievementList::DataEntry *pkEntry = CSVFile::SafeGetCSVFileDataEntryPtr<CSVFile::CF_AchievementList>(iID);
	if (pkEntry != NULL)
	{
		return pkEntry->_iLinkID;
	}
	return -1;
}

int LC_AchievementManager::GetPreAchievementID(int iID)
{
	CSVFile::CF_AchievementList::DataEntry *pkEntry = CSVFile::SafeGetCSVFileDataEntryPtr<CSVFile::CF_AchievementList>(iID);
	if (pkEntry != NULL)
	{
		return pkEntry->_kPreAchievements;
	}
	return 0;
}

int LC_AchievementManager::GetPreTaskID(int iID)
{
	CSVFile::CF_AchievementList::DataEntry *pkEntry = CSVFile::SafeGetCSVFileDataEntryPtr<CSVFile::CF_AchievementList>(iID);
	if (pkEntry != NULL)
	{
		return pkEntry->_kPreTasks;
	}
	return 0;
}

StringType LC_AchievementManager::GetItemReward(int iID)
{
	CSVFile::CF_AchievementList::DataEntry *pkEntry = CSVFile::SafeGetCSVFileDataEntryPtr<CSVFile::CF_AchievementList>(iID);
	if (pkEntry != NULL)
	{
		return pkEntry->_sRewardItems;
	}
	return "";
}

int LC_AchievementManager::GetSubClass(int iID)
{
	CSVFile::CF_AchievementList::DataEntry *pkEntry = CSVFile::SafeGetCSVFileDataEntryPtr<CSVFile::CF_AchievementList>(iID);
	if (pkEntry != NULL)
	{
		return pkEntry->_iSubclass;
	}
	return 0;
}

int LC_AchievementManager::GetAchievementTypeID(int iID)
{
	CSVFile::CF_AchievementList::DataEntry *pkEntry = CSVFile::SafeGetCSVFileDataEntryPtr<CSVFile::CF_AchievementList>(iID);
	if (pkEntry != NULL)
	{
		return pkEntry->_iAchievementTypeID;
	}
	return 0;
}

StringType LC_AchievementManager::GetAchieveName(int iID)
{
	CSVFile::CF_AchievementList::DataEntry *pkEntry = CSVFile::SafeGetCSVFileDataEntryPtr<CSVFile::CF_AchievementList>(iID);
	if (pkEntry != NULL)
	{
		return pkEntry->_iAchievementName;
	}
	return "";
}

int LC_AchievementManager::GetAchieveCharLevel(int iID)
{
	CSVFile::CF_AchievementList::DataEntry *pkEntry = CSVFile::SafeGetCSVFileDataEntryPtr<CSVFile::CF_AchievementList>(iID);
	if (pkEntry != NULL)
	{
		return pkEntry->_CharLevel;
	}
	return 0;
}

StringType LC_AchievementManager::GetAchievedetail(int iID)
{
	CSVFile::CF_AchievementList::DataEntry *pkEntry = CSVFile::SafeGetCSVFileDataEntryPtr<CSVFile::CF_AchievementList>(iID);
	if (pkEntry != NULL)
	{
		return pkEntry->_sAchievementinfo;
	}
	return "";
}

int LC_AchievementManager::GetAchievePoint(int iID)
{
	CSVFile::CF_AchievementList::DataEntry *pkEntry = CSVFile::SafeGetCSVFileDataEntryPtr<CSVFile::CF_AchievementList>(iID);
	if (pkEntry != NULL)
	{
		return pkEntry->_iAchievementPoint;
	}
	return 0;
}
