#include "EntrustTaskConfigManager.h"
#include "CF_EntrustTask.h"
#include "CF_EntrustType.h"
#include "LC_Helper.h"
using namespace CSVFile;
using namespace Utility;

CEntrustTaskConfigManager::CEntrustTaskConfigManager()
{

}

CEntrustTaskConfigManager::~CEntrustTaskConfigManager()
{
	if( !m_WeightVec.empty() )
	{
		m_WeightVec.clear();
	}

	if( !m_EntrustTaskMap.empty() )
	{
		m_EntrustTaskMap.clear();
	}
}

void CEntrustTaskConfigManager::Init()
{
	InitEntrustType();
	InitEntrustTask();
}

void CEntrustTaskConfigManager::Unit()
{

}

void CEntrustTaskConfigManager::InitEntrustType()
{
	int32_t total = 0;
	CF_EntrustType::DataEntryMapExternIterator kIter = CF_EntrustType::GetSingletonPtr()->GetIterator();
	while( kIter.HasMoreElements() )
	{
		CF_EntrustType::DataEntry* p = kIter.PeekNextValuePtr();
		SKeyValueConfig data;
		data.key = p->_iStar;
		data.value = p->_iRate;
		m_WeightVec.push_back(data);
		total += data.value;
		kIter.MoveNext();
	}
	m_TotalWeight = total;
}

void CEntrustTaskConfigManager::InitEntrustTask()
{
	CF_EntrustTask::DataEntryMapExternIterator kIter = CF_EntrustTask::GetSingletonPtr()->GetIterator();
	while( kIter.HasMoreElements() )
	{
		CF_EntrustTask::DataEntry* p = kIter.PeekNextValuePtr();
		SEntrustTaskConfig data;
		int32_t id = p->_iTaskID;
		data.star = p->_iStar;
		data.type = p->_iType;
		data.needTime = p->_iTime * 3600;
		//data.needTime = 120;
		mem::vector<int> result;
		if( !p->_sCondition.empty() )
		{
			bool b = LC_Helper::SpliteString2Int( p->_sCondition, result );
			size_t count = result.size() / 2;
			if( b && ((count % 2) == 0) )
			{
				for (size_t i = 0 ; i < count ; ++i)
				{
					SKeyValueConfig info;
					info.key = result[i * 2];
					info.value = result[i * 2 + 1];
					data.conditionVec.push_back( info );
				}
			}
		}
		result.clear();

		if( !p->_sConsume.empty() )
		{
			bool b = LC_Helper::SpliteString2Int( p->_sConsume, result );
			size_t count = result.size();
			if( b && count == 2 )
			{
				data.acceptCost.key = result[0];
				data.acceptCost.value = result[1];
			}
		}
		result.clear();

		//if( !p->_kCurrentPrices.empty() )
		//{
			//for(LC_ComMap::const_iterator it = p->_kCurrentPrices.begin(); it != p->_kCurrentPrices.end(); ++it)
			//{
				//data.speedupCost.key = it->first;
				//data.speedupCost.value = (int)it->second;
			//}
		//}
		if( !p->_kCurrentPrices.empty() )
		{
			bool b = LC_Helper::SpliteString2Int( p->_kCurrentPrices, result );
			size_t count = result.size();
			if( b && count == 2 )
			{
				data.speedupCost.key = result[0];
				data.speedupCost.value = result[1];
			}
		}
		result.clear();

		if( !p->_kReward.empty() )
		{
			for( UT_SIMDataList::const_iterator it = p->_kReward.begin(); it != p->_kReward.end(); ++it )
			{
				SItemConfig info;
				info.id = (*it).IID();
				info.num = (*it).ParamA();
				info.bind = (*it).ParamB();
				data.rewardVec.push_back( info );
			}
		}

		m_StarMap[data.star].push_back( id );
		m_EntrustTaskMap[id] = data;
		kIter.MoveNext();
	}
}

void CEntrustTaskConfigManager::Reload()
{

}

SEntrustTaskConfig* CEntrustTaskConfigManager::GetEnstrustTaskConfig( int32_t _id )
{
	mem::map<int32_t, SEntrustTaskConfig>::iterator it = m_EntrustTaskMap.find( _id );	
	if( it != m_EntrustTaskMap.end() )
	{
		return &it->second;
	}
	return NULL;
}

bool CEntrustTaskConfigManager::GetRandomEnstrustTask( mem::list<int32_t>& _list, int32_t _maxNum, int32_t _star )
{
	if( !_list.empty() )
	{
		_list.clear();
	}

	if( _maxNum < 0 )
	{
		_maxNum = 5;
	}

	mem::vector<int32_t> vec;
	vec.reserve( _maxNum );

	if( _star != 0 )
	{
		vec.push_back( _star );
		_maxNum -= 1;
	}

	//随机任务星级
	for( int i = 0 ; i < _maxNum; ++i )
	{
		int random = rand() % m_TotalWeight;
		for( int k = 0; k < m_WeightVec.size(); ++k )
		{
			if( random < m_WeightVec[k].value )
			{
				vec.push_back( m_WeightVec[k].key );
				break;
			}
			random -= m_WeightVec[k].value;
		}
	}

	//随机任务id
	for( int i = 0; i < vec.size(); ++i )
	{
		mem::map< int32_t, mem::vector<int32_t> >::iterator it = m_StarMap.find( vec[i] );
		if( it == m_StarMap.end() )
		{
			return false;
		}

		int num = (int)it->second.size();
		if( num == 0 )
		{
			return false;
		}

		if( num == 1 )
		{
			_list.push_back( it->second[0] );
		}
		else
		{
			int random = rand() % num;
			_list.push_back( it->second[random] );
		}
	}
	return true;
}
