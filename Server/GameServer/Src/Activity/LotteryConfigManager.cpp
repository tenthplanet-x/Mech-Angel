#include "LotteryConfigManager.h"
#include "CF_ACTTreasureHunt.h"
#include "CF_ACTTreasureHuntReward.h"
#include "CF_ACTTreasureHuntStageReward.h"
#include "LC_Helper.h"
using namespace CSVFile;
using namespace Utility;
CLotteryConfigManager::CLotteryConfigManager()
{

}

CLotteryConfigManager::~CLotteryConfigManager()
{
	if( !m_LotteryConfigMap.empty() )
	{
		m_LotteryConfigMap.clear();
	}

	if( !m_LotteryItemConfigMap.empty() )
	{
		m_LotteryItemConfigMap.clear();
	}

	if( !m_LotteryRewardConfigMap.empty() )
	{
		m_LotteryRewardConfigMap.clear();
	}
	
	if( !m_LotteryPondConfigMap.empty() )
	{
		m_LotteryPondConfigMap.clear();
	}
}

void CLotteryConfigManager::Init()
{
	InitTreasureHunt();
	InitTreasureHuntReward();
	InitTreasureHuntStageReward();
}

void CLotteryConfigManager::Unit()
{

}

void CLotteryConfigManager::Reload()
{

}

void CLotteryConfigManager::InitTreasureHunt()
{
	CF_ACTTreasureHunt::DataEntryMapExternIterator kIter = CF_ACTTreasureHunt::GetSingletonPtr()->GetIterator();
	while( kIter.HasMoreElements() )
	{
		CF_ACTTreasureHunt::DataEntry* p = kIter.PeekNextValuePtr();
		SLotteryConfig data;
		int id = p->_iID;
		data.m_Schedule = p->_iSchedule;
		data.m_ExtraSchedule = p->_iScheduleGetReward;
		data.m_Category = p->_iCategory;
		data.m_Cost = p->_iPerCharge;
		data.m_MaxNum = p->_iMaxGamble;
		data.m_DayMaxNum = p->_iLotteryMaxTimes;
		data.m_ItemRewardId = p->_iCrystalReward;
		//data.m_CostNum = p->_iTreasureRewardNum;
		data.m_CostNum = 10;

		//VIP打折id
		if( !p->_iVIPDiscount.empty() )
		{
			mem::vector<int> vec;
			LC_Helper::SpliteStringInt( p->_iVIPDiscount, vec );
			if( vec.size() == 2 )
			{
				data.m_VipItem = vec[0];
				data.m_VipCost = vec[1];
			}
		}

		//权重次数
		if( !p->_iKey.empty() )
		{
			LC_Helper::SpliteStringInt( p->_iKey, data.m_CountVec );
		}

		//奖池id
		if( !p->_iReward.empty() )
		{
			LC_Helper::SpliteStringInt( p->_iReward, data.m_RewardPondVec );
		}

		if( data.m_CountVec.size() != data.m_RewardPondVec.size() )
		{
			kIter.MoveNext();
			continue;
		}

		data.m_ItemCost.assign( p->_kPerItems.begin(), p->_kPerItems.end() );
		m_LotteryConfigMap[id] = data;

		//额外奖励2
		if( !p->_iTimes.empty() && !p->_sReward.empty() )
		{
			SLotteryRewardConfig info;
			LC_Helper::SpliteStringInt( p->_iTimes, info.m_NumVec );
			info.m_RewardVec.assign( p->_sReward.begin(), p->_sReward.end() );
			if( info.m_NumVec.size() == info.m_RewardVec.size() )
			{
				m_LotteryRewardConfigMap[id] = info;
			}
		}

		kIter.MoveNext();
	}
}

void CLotteryConfigManager::InitTreasureHuntReward()
{
	CF_ACTTreasureHuntReward::DataEntryMapExternIterator kIter = CF_ACTTreasureHuntReward::GetSingletonPtr()->GetIterator();
	while( kIter.HasMoreElements() )
	{
		CF_ACTTreasureHuntReward::DataEntry* p = kIter.PeekNextValuePtr();
		SLotteryPondConfig data;
		int id = p->_iID;

		if( !p->_iWeight.empty() )
		{
			LC_Helper::SpliteStringInt( p->_iWeight, data.m_WeightVec );
		}

		if( !p->_lBroadcastID.empty() )
		{
			LC_Helper::SpliteStringInt( p->_lBroadcastID, data.m_BroadcastVec );
		}

		data.m_RewardVec.assign( p->_kReward.begin(), p->_kReward.end() );

		if( data.m_WeightVec.size() == data.m_RewardVec.size() && 
			data.m_WeightVec.size() == data.m_BroadcastVec.size() )
		{
			for( int i = 0; i < data.m_WeightVec.size(); ++i )
			{
				data.m_TotalWeight += data.m_WeightVec[i];
			}
			m_LotteryPondConfigMap[id] = data;
		}
		kIter.MoveNext();
	}
}

void CLotteryConfigManager::InitTreasureHuntStageReward()
{
	CF_ACTTreasureHuntStageReward::DataEntryMapExternIterator kIter = CF_ACTTreasureHuntStageReward::GetSingletonPtr()->GetIterator();
	while( kIter.HasMoreElements() )
	{
		CF_ACTTreasureHuntStageReward::DataEntry* p = kIter.PeekNextValuePtr();
		SLotteryItemConfig data;
		int id = p->_iID;
		data.m_WeightVec.assign( p->_Weight.begin(), p->_Weight.end() );
		data.m_LevelVec.assign( p->_Level.begin(), p->_Level.end() );
		data.m_RewardVec.assign( p->_Award.begin(), p->_Award.end() );

		if( data.m_WeightVec.size() == data.m_LevelVec.size() &&
			data.m_WeightVec.size() == data.m_RewardVec.size() )
		{
			m_LotteryItemConfigMap[id] = data;
		}
		
		kIter.MoveNext();
	}
}


SLotteryConfig* CLotteryConfigManager::GetLotteryConfig( int _id )
{
	mem::map<int, SLotteryConfig >::iterator it = m_LotteryConfigMap.find( _id );
	if( it != m_LotteryConfigMap.end() )
	{
		return &(it->second);
	}
	return NULL;
}

SLotteryItemConfig* CLotteryConfigManager::GetLotteryItemConfig( int _id )
{
	mem::map<int, SLotteryItemConfig>::iterator it = m_LotteryItemConfigMap.find( _id );
	if( it != m_LotteryItemConfigMap.end() )
	{
		return &(it->second);
	}
	return NULL;
}

SLotteryRewardConfig* CLotteryConfigManager::GetLotteryRewardConfig( int _id )
{
	mem::map<int, SLotteryRewardConfig>::iterator it =  m_LotteryRewardConfigMap.find( _id );
	if( it != m_LotteryRewardConfigMap.end() )
	{
		return &(it->second);
	}
	return NULL;
}

bool CLotteryConfigManager::GetLotteryPondReward( Utility::UT_SIMDataList& _rewardItem, int _id, int _totalNum, int _num, bool _bFree )
{
	if( _bFree )
	{
		_totalNum = 1;
	}

	if( !_rewardItem.empty() )
	{
		_rewardItem.clear();
	}

	mem::map<int, SLotteryConfig >::iterator it = m_LotteryConfigMap.find( _id );
	if( it == m_LotteryConfigMap.end() )
	{
		return false;
	}

	if( it->second.m_CountVec.size() != it->second.m_RewardPondVec.size() )
	{
		return false;
	}

	//得到奖池id
	mem::vector<int> pondVec;
	for( int i = 0 ; i < _num; ++i )
	{
		_totalNum += 1;
		for( int temp = 0; temp < it->second.m_CountVec.size(); ++temp )
		{
			if( ( _totalNum % it->second.m_CountVec[temp] ) == 0 )
			{
				pondVec.push_back( it->second.m_RewardPondVec[temp] );
				break;
			}
		}
	}

	int size = (int)pondVec.size();
	if( size != _num )
	{
		return false;
	}

	//开始抽奖
	for( int i = 0 ; i < size; ++i )
	{
		mem::map<int, SLotteryPondConfig>::iterator itp =  m_LotteryPondConfigMap.find( pondVec[i] );
		if( itp == m_LotteryPondConfigMap.end() )
		{
			return false;
		}

		Utility::UT_SIMDataInfo reward;
		int random = rand() % itp->second.m_TotalWeight;
		for( int k = 0; k < itp->second.m_WeightVec.size(); ++k )
		{
			if( random < itp->second.m_WeightVec[k] )
			{
				reward = itp->second.m_RewardVec[k];
				reward.SetParamC( itp->second.m_BroadcastVec[k] );	//广播id
				break;
			}
			random -= itp->second.m_WeightVec[k];
		}

		_rewardItem.push_back( reward );
	}
	return true;
}

int CLotteryConfigManager::GetLotteryItemReward( Utility::UT_SIMDataList& _rewardItem, int _id, int _level, int remainTimes)
{
	if( !_rewardItem.empty() )
	{
		_rewardItem.clear();
	}

	mem::map<int, SLotteryItemConfig>::iterator it = m_LotteryItemConfigMap.find( _id );
	if( it == m_LotteryItemConfigMap.end() )
	{
		return RE_Lottery_Reward_NoId;
	}

	if( it->second.m_WeightVec.size() != it->second.m_LevelVec.size() || 
		it->second.m_WeightVec.size() != it->second.m_RewardVec.size() )
	{
		return RE_Lottery_Reward_SizeError;
	}

	int index = 0;
	int totalWeight = 0;
	for( int i = 0; i < it->second.m_LevelVec.size(); ++i )
	{
		if( _level >= it->second.m_LevelVec[i] )
		{
			index = i;
			totalWeight += it->second.m_WeightVec[i];
		}
		else
		{
			break;
		}
	}

	if( totalWeight == 0)
	{
		return RE_Lottery_Reward_ParamError;
	}

	for(int i=0; i<remainTimes; ++i)
	{
		int random = rand() % totalWeight;
		for( int k = 0; k <= index; ++k )
		{
			if( random < it->second.m_WeightVec[k] )
			{
				_rewardItem.push_back( it->second.m_RewardVec[k] );
				break;
			}
			random -= it->second.m_WeightVec[k];
		}
	}

	return RE_SUCCESS;
}
