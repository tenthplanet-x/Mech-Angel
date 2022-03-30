#include "ClientPCH.h"
#include "LC_ClientBoxInstance.h"
#include "GlobalSettings.h"
#include "MG_BotProtocol.h"
#include "LC_ClientBotManager.h"
#include "CF_Bot.h"
#include "CF_BotUpgrade.h"

using namespace CSVFile;
using namespace Utility;
using namespace Data;
using namespace GameLogic;

LC_ClientBotManager::LC_ClientBotManager()
{
	m_kBotDataMap.clear();
	m_kBotProfitData.clear();
	m_iTotalReward1 = 0;
	m_iTotalReward2 = 0;
	m_iTotalReward3 = 0;
	m_iTotalReward4 = 0;
	m_iTotalReward5 = 0;
	m_iTotalReward6 = 0;
	m_nBeginTime = 0;

	m_kVipNewProfitData.clear();

	LC_DirtyFlagAble::Reset();
}

LC_ClientBotManager::~LC_ClientBotManager()
{
	BotDataMapIter iter = m_kBotDataMap.begin();
	for (; iter != m_kBotDataMap.end(); ++iter)
	{
		T_SAFE_DELETE(iter->second);
		iter->second = NULL;
	}
	m_kBotDataMap.clear();
	m_kBotProfitData.clear();
	m_kVipNewProfitData.clear();
	LC_DirtyFlagAble::Reset();
}

void	LC_ClientBotManager::init()
{
	CF_Bot::DataEntryMapExternIterator kIter = CF_Bot::GetSingleton().GetIterator();
	for (; kIter.HasMoreElements(); kIter.MoveNext())
	{
		CF_Bot::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		BotDataMapIter iter = m_kBotDataMap.find(pkEntry->_cID);
		if ( iter == m_kBotDataMap.end() )
		{
			LC_ClientBotInstance* BotInstance = T_NEW_D LC_ClientBotInstance();
			BotInstance->SetCID(pkEntry->_cID);
			BotInstance->Init();
			m_kBotDataMap[pkEntry->_cID] = BotInstance;
		}
	}
}

void	LC_ClientBotManager::Reset()
{
	BotDataMapIter iter = m_kBotDataMap.begin();
	for( ;iter != m_kBotDataMap.end(); ++iter)
	{
		iter->second->Init();
	}
	m_kBotProfitData.clear();
	m_iTotalReward1 = 0;
	m_iTotalReward2 = 0;
	m_iTotalReward3 = 0;
	m_iTotalReward4 = 0;
	m_iTotalReward5 = 0;
	m_iTotalReward6 = 0;
	m_nBeginTime = 0;

	m_kVipNewProfitData.clear();


	LC_DirtyFlagAble::Reset();
}

void	LC_ClientBotManager::ParseProtocol(const MG_SC_BotDataSync& BotDataSync)
{
	Reset();
	m_nBeginTime = BotDataSync.m_nBeginTime;
	int count  = BotDataSync.m_nCount;
	for (int i = 0; i < count; i++)
	{
		const BotData_Base &BotDataBase  = BotDataSync.m_Data[i];
		int cID = BotDataBase.m_nResId;
		BotDataMapIter iter = m_kBotDataMap.find(cID);
		if ( iter == m_kBotDataMap.end() )
		{
			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "CF_Bot表找不到该数据 cID = %d", cID);
			continue;
		}
		else
		{
			LC_ClientBotInstance* BotInstance = iter->second;
			BotInstance->ParseProtocol(BotDataBase);
		}
	}

	ConstructBenefitData();
	UpdateDirtyFlag();
}

void LC_ClientBotManager::ParseProfitDataFromProtocol(const MG_SC_BotBonusCacheSync& BotProfitData)
{
	m_kBotProfitData.clear();
	m_kBotProfitData.reserve(BotProfitData.m_nCount);
	for (int index = 0; index < BotProfitData.m_nCount; index ++)
	{
		BotBonusData data = BotProfitData.m_Data[index];
		BotProfit profit(data.m_nItemId, data.m_nCount);
		m_kBotProfitData.push_back(profit);
	}
	UpdateDirtyFlag();
}

void GameLogic::LC_ClientBotManager::ParseVipNewProfitDataFromProtocol(const MG_SC_NewVIPBonusCacheSync& rltMsg)
{
	m_kVipNewProfitData.clear();
	m_kVipNewProfitData.reserve(rltMsg.m_nCount);
	for (int index = 0; index < rltMsg.m_nCount; index ++)
	{
		NewVIPBonusData data = rltMsg.m_Data[index];
		BotProfit profit(data.m_nItemId, data.m_nCount);
		m_kVipNewProfitData.push_back(profit);
	}
	UpdateDirtyFlag();
}

void LC_ClientBotManager::GetBeginFromProtocol(const MG_SC_BotDataSync_BeginTime& rltMsg)
{
	m_nBeginTime = rltMsg.m_nBeginTime;
}

void	LC_ClientBotManager::GetDataFromProtocol(const BotData_Base& BotDataBase)
{
	int cID = BotDataBase.m_nResId;
	BotDataMapIter iter = m_kBotDataMap.find(cID);
	if ( iter == m_kBotDataMap.end() )
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "CF_Bot表找不到该数据 cID = %d", cID);
	}
	else
	{
		LC_ClientBotInstance* BotInstance = iter->second;
		BotInstance->ParseProtocol(BotDataBase);
	}
	ConstructBenefitData();
	UpdateDirtyFlag();
}

StringType LC_ClientBotManager::GetProfitInfo()
{
	
	std::map<uint32_t,uint32_t> ItemMap;

	for (int index = 0; index < m_kBotProfitData.size(); index++)
	{
		uint32_t id= m_kBotProfitData[index].itemID;
		uint32_t count= m_kBotProfitData[index].itemCount;
		std::map<uint32_t,uint32_t>::iterator it=ItemMap.find(id);
		
		if(it!=ItemMap.end())
		{
			it->second=count;
		}
		else
		{
			ItemMap.insert(std::pair<uint32_t,uint32_t>(id,count));
		}	
	}
	int itemCount=0;
	StringStreamType sStream1("");
	for (int index = 0; index < m_kVipNewProfitData.size(); index++)
	{	
		uint32_t id= m_kVipNewProfitData[index].itemID;
		uint32_t count= m_kVipNewProfitData[index].itemCount;
		std::map<uint32_t,uint32_t>::iterator it=ItemMap.find(id);
		CF_ItemList::DataEntry* pkItem=SafeGetCSVFileDataEntryPtr<CF_ItemList>(id);
		if (pkItem && pkItem->_lMaxOverlapCount<=1)
		{
			for (int i=0;i<count;i++)
			{
				if (itemCount<500)
				{
					sStream1 << id << "_" << 1 << "#";
					itemCount++;
				}
				else
				{
					break;
				}
			}
			if (itemCount>=500)
			{
				break;
			}
		}
		else
		{
			if(it!=ItemMap.end())
			{
				it->second+=count;
			}
			else
			{
				ItemMap.insert(std::pair<uint32_t,uint32_t>(id,count));
				itemCount++;
			}	
		}
	}
	
	StringStreamType sStream("");
	std::map<uint32_t,uint32_t>::iterator it=ItemMap.begin();
	for(;it!=ItemMap.end();++it)
	{
		sStream <<it->first << "_" << it->second << "#";
	}
	sStream<<sStream1.str();
	
	return sStream.str();
}

StringType LC_ClientBotManager::GetBotInfoByCID(int cID)
{
	BotDataMapIter iter = m_kBotDataMap.find(cID);
	if (iter != m_kBotDataMap.end())
	{
		return iter->second->GetBotInfo();
	}
	
	return "";
}

StringType LC_ClientBotManager::GetLevelUpBotInfoByCID(int cID)
{
	BotDataMapIter iter = m_kBotDataMap.find(cID);
	if (iter != m_kBotDataMap.end())
	{
		LC_ClientBotInstance *BotInstance = iter->second;
		return BotInstance->GetLevelUpData();
	}

	return "0;0;0;";
}

StringType LC_ClientBotManager::GetAllBotInfo()
{
	StringStreamType sStream("");
	BotDataMapIter iter = m_kBotDataMap.begin();
	for ( ;iter != m_kBotDataMap.end(); ++iter)
	{
		sStream << iter->second->GetBotInfo() << "|";
	}

	return sStream.str();
}
// 不改动原有逻辑，增加新功能
void   LC_ClientBotManager::ConstructBenefitData()
{
	m_iTotalReward1 = 0;
	m_iTotalReward2 = 0;
	m_iTotalReward3 = 0;
	m_iTotalReward4 = 0;
	m_iTotalReward5	= 0;
	m_iTotalReward6 = 0;
	BotDataMapIter iter = m_kBotDataMap.begin();
	for ( ;iter != m_kBotDataMap.end(); ++iter)
	{
		LC_ClientBotInstance* BotInstance = iter->second;
		if (!BotInstance->IsAvtivce())
		{
			continue;
		}
		// 固定前三个显示
		m_iTotalReward1 += BotInstance->GetEfficencyByIndex(0);
		m_iTotalReward2 += BotInstance->GetEfficencyByIndex(1);
		m_iTotalReward3 += BotInstance->GetEfficencyByIndex(2);
		m_iTotalReward4 += BotInstance->GetEfficencyByIndex(3);
		m_iTotalReward5 += BotInstance->GetEfficencyByIndex(4);
		m_iTotalReward6 += BotInstance->GetEfficencyByIndex(5);
	}
}

int LC_ClientBotManager::GetTotalRewardByIndex(int index)
{
	int totalCount = 0;
	BotDataMapIter iter = m_kBotDataMap.begin();
	for ( ;iter != m_kBotDataMap.end(); ++iter)
	{
		LC_ClientBotInstance* BotInstance = iter->second;
		if (!BotInstance->IsAvtivce())
		{
			continue;
		}
		totalCount += BotInstance->GetEfficencyByIndex(index);
	}
	return totalCount;
}

int LC_ClientBotManager::GetRewardItemIDByIndex(int index)
{
	BotDataMapIter iter = m_kBotDataMap.begin();
	if (iter != m_kBotDataMap.end())
	{
		return iter->second->GetEfficencyItemIDByIndex(index);
	}
	return 0;
}

StringType LC_ClientBotManager::GetTarentInfo()
{
	StringStreamType sStreamActive("");
	StringStreamType sStreamInactive("");
	BotDataMapIter iter = m_kBotDataMap.begin();
	for ( ;iter != m_kBotDataMap.end(); ++iter)
	{
		LC_ClientBotInstance* BotInstance = iter->second;
		if (BotInstance->IsAvtivce())
			if (BotInstance->IsActiveTalent())
				sStreamActive << BotInstance->ContructTarentInfo() << "#";
			else
				sStreamInactive << BotInstance->ContructTarentInfo() << "#";
		else
			sStreamInactive << BotInstance->ContructTarentInfo() << "#";
	}
	sStreamActive << "@"  << sStreamInactive.str();
	return sStreamActive.str();
}

BotPromptType LC_ClientBotManager::GetPrompt(LC_ClientGamePlayer* pkGamePlayer)
{
	BotDataMapIter iter = m_kBotDataMap.begin();
	for ( ;iter != m_kBotDataMap.end(); ++iter)
	{
		BotPromptType iPromptType = iter->second->GetPrompt(pkGamePlayer);
		if(iPromptType != BOT_PROMPTTYPE_CANNOT)
			return iPromptType;
	}

	return BOT_PROMPTTYPE_CANNOT;
}

int LC_ClientBotManager::GetActiveCount()
{
	int iCount = 0;
	BotDataMapIter iter = m_kBotDataMap.begin();
	for ( ;iter != m_kBotDataMap.end(); ++iter)
	{
		LC_ClientBotInstance* pkBotInstance = iter->second;
		if(pkBotInstance->IsAvtivce())
			++iCount;
	}

	return iCount;
}


















