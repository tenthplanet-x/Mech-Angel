#include "LC_WildPrincesManager.h"
#include "CF_WildPrinces.h"

using namespace CSVFile;
CWildPrincesManager::CWildPrincesManager()
{
	m_WildPrincesMapInfo.clear();
}

CWildPrincesManager::~CWildPrincesManager()
{
}

void CWildPrincesManager::Init()
{
	m_WildPrincesMapInfo.clear();
	CF_WildPrinces::DataEntryMapExternIterator kIter = CF_WildPrinces::GetSingletonPtr()->GetIterator();
	for(; kIter.HasMoreElements(); kIter.MoveNext())
	{
		CF_WildPrinces::DataEntry* pkData = kIter.PeekNextValuePtr();
		TypeIDAndMapInfoMap::iterator findIt1 = m_WildPrincesMapInfo.find(pkData->_lType);
		if(findIt1 == m_WildPrincesMapInfo.end())
		{
			mem::vector<uint32_t> bossData;
			bossData.reserve(10);
			bossData.push_back(pkData->_lCharType);

			MapIdAndBossIdMap info;
			info[pkData->_lMapResID] = bossData;

			m_WildPrincesMapInfo[pkData->_lType] = info;
		}
		else
		{
			MapIdAndBossIdMap::iterator findIt2 = findIt1->second.find(pkData->_lMapResID);
			if(findIt2 == findIt1->second.end())
			{
				mem::vector<uint32_t> bossData;
				bossData.reserve(10);
				bossData.push_back(pkData->_lCharType);

				findIt1->second[pkData->_lMapResID] = bossData;
			}
			else
			{
				mem::vector<uint32_t>::iterator findIt3 = find(findIt2->second.begin(), findIt2->second.end(), pkData->_lCharType);
				if(findIt3 == findIt2->second.end())
				{
					findIt2->second.push_back(pkData->_lCharType);
				}
			}
		}
	}
}

void CWildPrincesManager::Unit()
{
	m_WildPrincesMapInfo.clear();
}

ResultType CWildPrincesManager::CheckMapBossIDVaild(uint32_t type, uint32_t mapID, uint32_t bossID)
{
	TypeIDAndMapInfoMap::iterator findIt1 = m_WildPrincesMapInfo.find(type);
	if(findIt1 == m_WildPrincesMapInfo.end())
	{
		return RE_WILDPRINCES_ERROR_1;
	}

	MapIdAndBossIdMap::iterator findIt2 = findIt1->second.find(mapID);
	if(findIt2 == findIt1->second.end())
	{
		return RE_WILDPRINCES_ERROR_2;
	}

	mem::vector<uint32_t>::iterator findIt3 = find(findIt2->second.begin(), findIt2->second.end(), bossID);
	if(findIt3 == findIt2->second.end())
	{
		return RE_WILDPRINCES_ERROR_3;
	}

	return RE_SUCCESS;
}