#include "LC_DamageRankManager.h"
#include "LC_ServerPlayer.h"
#include "UT_ServerHelper.h"

void SingleContainer::Clear()
{
	m_bSort = false;
	m_nTotalValue = 0;
	m_nValue.clear(); 
	m_kSortList.clear(); 
}

void SingleContainer::UpdateValue(uint64_t key, const string& name, uint64_t detalValue)
{
	std::map<uint64_t, PlayerData>::iterator findIt = m_nValue.find(key);
	if(findIt == m_nValue.end())
	{
		m_kSortList.push_back(key);
	}

	m_nValue[key].value += detalValue;
	m_nValue[key].name = name;
	m_nTotalValue += detalValue;

	m_bSort = false;
}

uint64_t SingleContainer::GetValue(uint64_t key)
{
	std::map<uint64_t, PlayerData>::iterator findIt = m_nValue.find(key);
	return findIt==m_nValue.end()? 0 : findIt->second.value;
}

ResultType SingleContainer::GetRankIndex(uint64_t key, int32_t& keyIndex, uint64_t& value)
{
	keyIndex = SPECIES_RANK_NONE;//Œ¥…œ∞Ò
	value = 0;
	std::map<uint64_t, PlayerData>::iterator findIt = m_nValue.find(key);
	if(findIt == m_nValue.end())
	{
		return RE_SUCCESS;
	}

	value = findIt->second.value;
	
	DoSort();

	for(uint32_t i=0; i<m_kSortList.size(); ++i)
	{
		if(key == m_kSortList[i])
		{
			keyIndex = i;
			break;
		}
	}

	return RE_SUCCESS;
}

ResultType SingleContainer::FillRankDetailInfo(uint64_t key, Protocol::PS_PlayerInfo& selfInfo, std::vector<Protocol::PS_PlayerInfo>& playerInfo, int num)
{
	std::map<uint64_t, PlayerData>::iterator findIt = m_nValue.find(key);
	if(findIt != m_nValue.end())
	{
		selfInfo.m_nValue = findIt->second.value;
	}

	playerInfo.reserve(num);
	DoSort();

	bool tag = false;
	for(uint32_t i=0; i<m_kSortList.size()&&i<num; ++i)
	{
		std::map<uint64_t, PlayerData>::iterator findIt = m_nValue.find(m_kSortList[i]);
		if(findIt == m_nValue.end())
		{
			continue;
		}

		Protocol::PS_PlayerInfo data;
		data.m_nRankIndex = i;
		data.m_nValue = findIt->second.value;
		data.m_nCharName = findIt->second.name;
		playerInfo.push_back(data);

		if(m_kSortList[i] == key)
		{
			tag = true;
			selfInfo.m_nRankIndex = i;
		}
	}

	if(!tag)
	{
		for(int i=num; i<m_kSortList.size(); ++i)
		{
			if(m_kSortList[i] == key)
			{
				selfInfo.m_nRankIndex = i;
				break;
			}
		}
	}

	return RE_SUCCESS;
}

void SingleContainer::DoSort()
{
	if(m_bSort)
	{
		return;
	}

	std::sort(m_kSortList.begin(), m_kSortList.end(), SingleContainerCompare(*this));
	m_bSort = true;
}

void EncountedSortContainer::Clear()
{
	m_kStatMap.clear();
	m_kSortList.clear();
}

void EncountedSortContainer::UpdateValue(uint64_t key1, uint64_t key2, const string& name, uint64_t value)
{
	EncountedStatMapIter findIt = m_kStatMap.find(key1);
	if (findIt == m_kStatMap.end())
	{
		m_kSortList.push_back(key1);
	}

	SingleContainer& singleContainer = m_kStatMap[key1];
	singleContainer.UpdateValue(key2, name, value);

	m_kStatValue.UpdateValue(key2, name, value);

	m_bSort = false;
}

uint64_t EncountedSortContainer::GetValue(uint64_t key1)
{
	EncountedStatMap::iterator findIt = m_kStatMap.find(key1);
	return findIt==m_kStatMap.end() ? 0 : findIt->second.m_nTotalValue;
}

uint64_t EncountedSortContainer::GetValue(uint64_t key1, uint64_t key2)
{
	EncountedStatMap::iterator findIt = m_kStatMap.find(key1);
	return findIt==m_kStatMap.end()? 0 : findIt->second.GetValue(key2);
}

uint64_t EncountedSortContainer::GetTotalValue(uint64_t key)
{
	EncountedStatMap::iterator findIt = m_kStatMap.find(key);
	return findIt==m_kStatMap.end()? 0 : findIt->second.m_nTotalValue;
}

ResultType EncountedSortContainer::GetRankIndex(uint64_t key1, uint64_t key2, int32_t& key1RankIndex, uint64_t& key1Value, int32_t& key2RankIndex, uint64_t& key2Value)
{
	key1RankIndex = SPECIES_RANK_NONE;//Œ¥…œ∞Ò
	key2RankIndex = SPECIES_RANK_NONE;//Œ¥…œ∞Ò
	key1Value = 0;
	key2Value = 0;

	EncountedStatMap::iterator findIt = m_kStatMap.find(key1);
	if(findIt == m_kStatMap.end())
	{
		return RE_SUCCESS;
	}

	key1Value = GetValue(key1);
	
	DoSort();

	for(uint32_t i=0; i<m_kSortList.size(); ++i)
	{
		if(key1 == m_kSortList[i])
		{
			key1RankIndex = i;
			break;
		}
	}

	return m_kStatValue.GetRankIndex(key2, key2RankIndex, key2Value);
}

ResultType EncountedSortContainer::FillRankDetailInfo(uint64_t key1, uint64_t key2, Protocol::PS_PlayerInfo& selfInfo, std::vector<Protocol::PS_PlayerInfo>& playerInfo)
{
	EncountedStatMap::iterator findIt = m_kStatMap.find(key1);
	if(findIt == m_kStatMap.end())
	{
		return RE_SUCCESS;
	}

	return findIt->second.FillRankDetailInfo(key2, selfInfo, playerInfo, 10);
}

void EncountedSortContainer::DoSort()
{
	if(m_bSort)
	{
		return;
	}

	std::sort(m_kSortList.begin(), m_kSortList.end(), EncountedSortContainerCompare(*this));
	m_bSort = true;
}

CDamageRankManager::CDamageRankManager()
{
	Clear();
}

CDamageRankManager::~CDamageRankManager()
{
	
}

void CDamageRankManager::Clear()
{
	m_EncountedSortContainer.clear();
}

void CDamageRankManager::Init()
{
	Clear();
}

void CDamageRankManager::Unit()
{
	Clear();
}

void CDamageRankManager::UpdatePlayerDamage(int32_t mapID, int64_t guildID, uint64_t playerID, const string& name, uint64_t detalDamage)
{
	if(0 == detalDamage)
	{
		return;
	}

	EncountedSortContainer& container = m_EncountedSortContainer[mapID];
	container.UpdateValue(guildID, playerID, name, detalDamage);
}

uint64_t CDamageRankManager::GetPlayerDamage(int32_t mapID, int64_t guildID, uint64_t playerID)
{
	std::map<int32_t, EncountedSortContainer>::iterator findIt = m_EncountedSortContainer.find(mapID);
	return findIt==m_EncountedSortContainer.end() ? 0 : findIt->second.GetValue(guildID, playerID);
}

uint64_t CDamageRankManager::GetGuildTotalDamage(int32_t mapID, int64_t guildID)
{
	std::map<int32_t, EncountedSortContainer>::iterator findIt = m_EncountedSortContainer.find(mapID);
	return findIt==m_EncountedSortContainer.end() ? 0 : findIt->second.GetTotalValue(guildID);
}

ResultType CDamageRankManager::FillRankIndex(int32_t mapID, uint64_t guildID, uint64_t playerID, int32_t& selfGuildRankIndex, uint64_t& selfGuildTotalDamage, int32_t& selfRankIndex, uint64_t& selfTotalDamage)
{
	std::map<int32_t, EncountedSortContainer>::iterator findIt = m_EncountedSortContainer.find(mapID);
	if(findIt == m_EncountedSortContainer.end())
	{
		selfGuildRankIndex = SPECIES_RANK_NONE;//Œ¥…œ∞Ò
		selfRankIndex = SPECIES_RANK_NONE;//Œ¥…œ∞Ò
		selfGuildTotalDamage = 0;
		selfTotalDamage= 0;
		return RE_SUCCESS;
	}

	return findIt->second.GetRankIndex(guildID, playerID, selfGuildRankIndex, selfGuildTotalDamage, selfRankIndex, selfTotalDamage);
}

ResultType CDamageRankManager::FillGuildDamageRankInfo(int32_t mapID, LC_ServerPlayer* pkOwnerPlayer, PS_PlayerInfo& selfInfo, std::vector<PS_PlayerInfo>& playerInfo)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return RE_FAIL;
	}

	selfInfo.m_nCharName = TPSTR2STDSTR(pkOwnerPlayer->GetCharName());
	selfInfo.m_nRankIndex = SPECIES_RANK_NONE;//Œ¥…œ∞Ò
	selfInfo.m_nValue = 0;

	uint64_t guildID = pkOwnerPlayer->GetGuildID();
	uint64_t playerID = pkOwnerPlayer->GetInstance();
	std::map<int32_t, EncountedSortContainer>::iterator findIt = m_EncountedSortContainer.find(mapID);
	if(findIt == m_EncountedSortContainer.end())
	{
		return RE_SUCCESS;
	}
	
	return  findIt->second.FillRankDetailInfo(guildID, playerID, selfInfo, playerInfo);
}