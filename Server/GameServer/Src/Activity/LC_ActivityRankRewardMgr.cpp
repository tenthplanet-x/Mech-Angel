#include "LC_ActivityRankRewardMgr.h"
#include "LC_Helper.h"
#include "CF_ActivityRankReward.h"

using namespace CSVFile;

CActivityRankRewardMgr::CActivityRankRewardMgr()
{
	m_mRankRewardData.clear();
}

CActivityRankRewardMgr::~CActivityRankRewardMgr()
{

}

void CActivityRankRewardMgr::Init()
{
	CF_ActivityRankReward::DataEntryMapExternIterator kIt = CF_ActivityRankReward::GetSingletonPtr()->GetIterator();
	for(; kIt.HasMoreElements(); kIt.MoveNext())
	{
		CF_ActivityRankReward::DataEntry* pkData = kIt.PeekNextValuePtr();
		uint64_t id = LC_Helper::EncodeScore(pkData->_GroupID, pkData->_iParam);
		std::map<uint64_t, RewardInfo>::iterator findIt = m_mRankRewardData.find(id);
		if(findIt == m_mRankRewardData.end())
		{
			RewardInfo data;
			data.m_sMailTitle = pkData->_mailTitle;
			data.m_sMailDesc = pkData->_mailDesc;
			data.m_mRewardItem.insert(std::make_pair(pkData->_iRankStartIndex, pkData->_kReward));

			m_mRankRewardData.insert(std::make_pair(id, data));
		}
		else
		{
			std::map<int32_t, StringType>::iterator kIt = findIt->second.m_mRewardItem.find(pkData->_iRankStartIndex);
			if(kIt != findIt->second.m_mRewardItem.end())
			{
				//配表有问题
			}
			else
			{
				findIt->second.m_mRewardItem.insert(std::make_pair(pkData->_iRankStartIndex, pkData->_kReward));
			}
		}
	}
}

void CActivityRankRewardMgr::Unit()
{

}

const RewardInfo& CActivityRankRewardMgr::GetRankRewardInfo(int type, int param)
{
	uint64_t id = LC_Helper::EncodeScore(type, param);
	std::map<uint64_t, RewardInfo>::iterator findIt = m_mRankRewardData.find(id);
	return findIt!=m_mRankRewardData.end() ? findIt->second : m_nEmptyRewardInfo;
}

StringType CActivityRankRewardMgr::GetRankRewardItem(int type, int param, int rankIndex, StringType& mailTitle, StringType& mailDesc)
{
	const RewardInfo& reward = GetRankRewardInfo(type, param);
	mailTitle = reward.m_sMailTitle;
	mailDesc = reward.m_sMailDesc;
	StringType rewardItem;
	for(std::map<int32_t, StringType>::const_iterator it=reward.m_mRewardItem.begin(); it!=reward.m_mRewardItem.end(); ++it)
	{
		if(rankIndex >= it->first)
		{
			rewardItem = it->second;
		}
		else
		{
			break;
		}
	}

	return rewardItem;
}



