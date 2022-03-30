#include "LC_CombatYaZhiManager.h"
#include "CF_CombatPower.h"

using namespace CSVFile;

CYaZhiManager::CYaZhiManager()
{
	m_combatYaZhiInfoMap.clear();
}

CYaZhiManager::~CYaZhiManager()
{

}

void CYaZhiManager::Load_CombatYaZhi()
{
	CF_CombatPower::DataEntryMapExternIterator kIter = CF_CombatPower::GetSingletonPtr()->GetIterator();
	for(; kIter.HasMoreElements(); kIter.MoveNext())
	{
		CF_CombatPower::DataEntry* pkData = kIter.PeekNextValuePtr();

		YaZhiData data(pkData->_fCombatPower_PVP, pkData->_fCombatPower_PVE);
		m_combatYaZhiInfoMap.insert(std::make_pair(pkData->_iCombatPowerPercent, data));
	}
}

float CYaZhiManager::FetchCombatYaZhiData(int64_t srcCombatScore, int64_t destCombatScore, int yazhiType)
{
	//ս������Ϊ�������Ų���ѹ�Ƽ���
	if(srcCombatScore<=0 || destCombatScore<=0)
	{
		return 1.0;
	}

	//��ѹ������
	mem::map<float, YaZhiData>::iterator slowIt = m_combatYaZhiInfoMap.begin();
	if(slowIt == m_combatYaZhiInfoMap.end())
	{
		return 1.0;
	}

	float combatPrecent = (float)srcCombatScore/(float)destCombatScore;

	mem::map<float, YaZhiData>::iterator fastIt = m_combatYaZhiInfoMap.begin();
	++fastIt;

	while(slowIt != m_combatYaZhiInfoMap.end())
	{
		if(fastIt == m_combatYaZhiInfoMap.end())
		{
			break;
		}

		if(slowIt->first<combatPrecent && combatPrecent<fastIt->first)
		{
			break;
		}

		++slowIt;
		++fastIt;
	}

	//slowIt������Ϊnull
	return yazhiType==YaZhiType_PVP ?  slowIt->second.YaZhi_PVP : slowIt->second.YaZhi_PVE;
}