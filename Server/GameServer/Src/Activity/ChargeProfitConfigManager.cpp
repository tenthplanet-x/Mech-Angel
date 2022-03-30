#include "ChargeProfitConfigManager.h"
#include "CF_ACTChargeProfitB.h"
#include "CF_ACTChargeProfitBReward.h"
#include "LC_Helper.h"
using namespace CSVFile;
using namespace Utility;
CChargeProfitConfigManager::CChargeProfitConfigManager()
{
	m_ChargeProfitConfigMap.clear();
}

CChargeProfitConfigManager::~CChargeProfitConfigManager()
{
	m_ChargeProfitConfigMap.clear();
}

void CChargeProfitConfigManager::InitChargeProfit()
{
	CF_ACTChargeProfitB::DataEntryMapExternIterator kIter = CF_ACTChargeProfitB::GetSingletonPtr()->GetIterator();
	while( kIter.HasMoreElements() )
	{
		CF_ACTChargeProfitB::DataEntry* p = kIter.PeekNextValuePtr();
		SChargeProfitConfig data;
		int id = p->_iID;
		data.m_Type = p->_iType;
		data.m_CashType = p->_iCashType;
		data.m_Schedule = p->_iSchedule;
		data.m_FinishSchedule = p->_iScheduleShow;
		data.m_CondtionVec.assign( p->_sKey.begin(), p->_sKey.end() );
		data.m_LimitNumVec.assign( p->_sTimeLimits.begin(), p->_sTimeLimits.end() );
		data.m_IdVec.assign( p->_sReward.begin(), p->_sReward.end() );
		data.m_sMailTitle = p->_sMailTitle;
		data.m_sMailDesc = p->_sMailDesc;

		if( data.m_CondtionVec.size() == data.m_LimitNumVec.size() && 
			data.m_CondtionVec.size() == data.m_IdVec.size() )
		{
			m_ChargeProfitConfigMap[id] = data;
		}

		kIter.MoveNext();
	}
}

void CChargeProfitConfigManager::InitChargeProfitReward()
{
	mem::map<int, Utility::UT_SIMDataList> data;
	CF_ACTChargeProfitBReward::DataEntryMapExternIterator kIter = CF_ACTChargeProfitBReward::GetSingletonPtr()->GetIterator();
	while( kIter.HasMoreElements() )
	{
		CF_ACTChargeProfitBReward::DataEntry* p = kIter.PeekNextValuePtr();
		data[p->_iID] = p->_kReward;
		kIter.MoveNext();
	}

	mem::map<int, SChargeProfitConfig>::iterator it = m_ChargeProfitConfigMap.begin();
	while( it != m_ChargeProfitConfigMap.end() )
	{
		for( mem::vector<int>::iterator temp = it->second.m_IdVec.begin(); 
			temp != it->second.m_IdVec.end(); ++temp )
		{
			mem::map<int, Utility::UT_SIMDataList>::iterator reward = data.find( (*temp) );
			if( reward != data.end() )
			{
				it->second.m_RewardVec.push_back( reward->second );
			}
		}

		//数据异常，删除这个记录
		if( it->second.m_RewardVec.size() != it->second.m_IdVec.size() )
		{
			it = m_ChargeProfitConfigMap.erase( it );
		}
		else
		{
			++it ;
		}
	}
}

void CChargeProfitConfigManager::Init()
{
	InitChargeProfit();
	InitChargeProfitReward();
}

void CChargeProfitConfigManager::Unit()
{

}

void CChargeProfitConfigManager::Reload()
{

}

SChargeProfitConfig* CChargeProfitConfigManager::GetChargeProfitConfig( int _id )
{
	mem::map<int, SChargeProfitConfig>::iterator it =  m_ChargeProfitConfigMap.find( _id );
	if( it != m_ChargeProfitConfigMap.end() )
	{
		return &(it->second);
	}

	return NULL;
}

void CChargeProfitConfigManager::GetChargeProfitScheduleByCashType( int32_t cashType,  int type, mem::map<int,int>& _map )
{
	if( !_map.empty() )
	{
		_map.clear();
	}

	mem::map<int, SChargeProfitConfig>::iterator it =  m_ChargeProfitConfigMap.begin();
	while( it != m_ChargeProfitConfigMap.end() )
	{
		if(cashType == it->second.m_CashType && type==it->second.m_Type) 
		{
			_map[it->first] = it->second.m_Schedule;
		}
		++it;
	}
}