#include "ACT_Lottery.h"
#include "ACT_ActivitySchedule.h"

#include "LC_ServerPlayer.h"
#include "LC_ServerLogicManager.h"

#include "UT_ServerHelper.h"
#include "UT_MathBase.h"
#include "oss_define.h"
#include <functional>
#include <algorithm>
#include "GlobalSettings.h"
#include "CF_ACTLottery.h"

using namespace CSVFile;
using namespace GameLogic;
using namespace Activity;

ACT_LotteryDataEntry::LotteryCountMap ACT_LotteryDataEntry::m_LotteryCountMap;
Utility::UT_SIMDataList ACT_LotteryManager::m_MultiRewardItems;

uint32_t ACT_LotteryDataEntry::BuildKey(int16_t nSchedule, int16_t nCategory)
{
	return (nSchedule << 16) | nCategory;
}

void ACT_LotteryDataEntry::UpdateLotteryCountMapData( int16_t nSchedule, int16_t nCategory, int32_t nValue )
{
	uint32_t nKey = BuildKey(nSchedule, nCategory);
	LotteryCountMap::iterator it = m_LotteryCountMap.find(nKey);
	if ( it == m_LotteryCountMap.end() )
	{
		mem::vector<int32_t> vec;
		vec.push_back(nValue);
		m_LotteryCountMap.insert(std::make_pair(nKey, vec));
	}
	else
	{
		mem::vector<int32_t>& vec = it->second;
		vec.push_back(nValue);
		std::sort(vec.begin(),vec.end(), std::greater<int32_t>());
	}
}

int32_t ACT_LotteryDataEntry::FetchLotteryCount(int16_t nSchedule, int16_t nCategory, int32_t nValue)
{
	int32_t nResult = 0;
	uint32_t nKey = BuildKey(nSchedule, nCategory);
	LotteryCountMap::iterator it = m_LotteryCountMap.find(nKey);
	if (it == m_LotteryCountMap.end())
		return nResult;

	nResult = 1;
	mem::vector<int32_t> vecResult = it->second;
	for ( int i = 0; i < vecResult.size(); i++ )
	{
		if (vecResult[i] == 0)
		{
			break;
		}
		if ( nValue % vecResult[i] == 0)
		{
			nResult = vecResult[i];
			break;
		}
	}

	return nResult;
}

ResultType ACT_LotteryDataEntry::DoStaticReward(LC_ServerPlayer* pkPlayer, ACT_ScheduleSaveEntry* pkEntry, int32_t iRewardCount)
{
	if (NULL == pkPlayer || NULL == pkEntry)
	{
		return RE_FAIL;
	}

	mem::map<int32_t, LotteryStaticReward>::iterator it = m_StaticRewardData.find(iRewardCount);
	if ( it == m_StaticRewardData.end() )
		return RE_FAIL;

	LotteryStaticReward& reward = it->second;

	int32_t nBroadcastId = reward.iBroadcastId;
	const Utility::UT_SIMDataList& kItems = reward.kItems;
	if (kItems.empty())
	{
		return RE_SUCCESS;
	}

	ResultType iResult = UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, kItems, pkPlayer, SYS_FUNC_TP_ACTIVITY_LOTTERY,
		m_iCategory, m_sMailTitle, m_sMailDesc, m_bMailEnable, true, true);

	// 广播
	if (nBroadcastId)
	{
		for ( int i = 0; i < kItems.size(); i++ )
		{
			UT_ServerHelper::_GameNoticeFmort noticeFmort;
			noticeFmort.SetMsgID(nBroadcastId);
			noticeFmort.AddString(pkPlayer->GetOwnerCharName());//玩家名
			noticeFmort.AddActivityId(  BuildKey(m_iSchdeule, m_iCategory) );
			noticeFmort.AddItemQuality(kItems[i].X());
			noticeFmort.AddItemName(kItems[i].X());

			UT_ServerHelper::BrocastNoticeMsg(noticeFmort, pkPlayer->GetCitizenship());
		}
	}

	return RE_SUCCESS;
}

ResultType ACT_LotteryDataEntry::DoReward(LC_ServerPlayer* pkPlayer, ACT_ScheduleSaveEntry* pkEntry, int32_t iRewardCount)
{
	if (NULL == pkPlayer || NULL == pkEntry)
	{
		return RE_FAIL;
	}

	int32_t iRewardIndex = -1;
	mem::map<int32_t, int32_t>::iterator it = m_CountIndexMap.find(iRewardCount);
	if (it != m_CountIndexMap.end())
	{
		iRewardIndex = it->second;
	}
	else
	{
		//first traversal calc total weight
		int32_t iTotalWeight = 0, iCurWeight = 0;
		for (size_t index = 0; index < m_RewardVec.size(); ++index)
		{
			LotteryReward& kReward = m_RewardVec.at(index);
			if (kReward.iWeight <= 0)
			{
				continue;
			}
			iTotalWeight += kReward.iWeight;
		}
		//second traversal calc reward index;
		int32_t iRand = UT_MathBase::RandInRangeInt(1, iTotalWeight);
		for (size_t index = 0; index < m_RewardVec.size(); ++index)
		{
			LotteryReward& kReward = m_RewardVec.at(index);
			if (kReward.iWeight <= 0)
			{
				continue;
			}
			iCurWeight += kReward.iWeight;
			if (iCurWeight >= iRand)
			{
				iRewardIndex = (int32_t)index;
                break;
			}
		}
	}
	if (iRewardIndex < 0 || iRewardIndex >= m_RewardVec.size())
	{
		return RE_ACTIVITY_CONFIG_INVALID;
	}

	int32_t nBroadcastId = m_RewardVec.at(iRewardIndex).iBroadcastId;
	const Utility::UT_SIMDataList& kItems = m_RewardVec.at(iRewardIndex).kItems;
	if (kItems.empty())
	{
		return RE_ACTIVITY_CONFIG_INVALID;
	}

	ResultType iResult = UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, kItems, pkPlayer, SYS_FUNC_TP_ACTIVITY_LOTTERY,
														   m_iCategory, m_sMailTitle, m_sMailDesc, m_bMailEnable, true, true);
	if (RE_SUCCESS != iResult)
	{
		return iResult;
	}

	ResultType rResult;
	pkEntry->RegReward(iRewardIndex);
	//int nSize = kItems.size();
	//for (int i = 0; i <nSize; i++)
	//{
	//	ACT_LotteryManager::AddSingleRewardItem(kItems[i].X(), kItems[i].Y());
	//}
	// 单次奖励pkDataEntry->m_kPerGetItems
	Utility::UT_SIMDataList::iterator itReward = m_kPerGetItems.begin();
	for (; itReward != m_kPerGetItems.end(); ++itReward)
	{
		UT_SIMDataInfo info = *itReward;
		int32_t nItemId = info.GetItemId();
		int32_t nCount = info.GetItemNum();
		rResult = LC_Helper::AddItem(((LC_ServerPlayer*)pkPlayer)->GetPackAsset(), nItemId, nCount, SYS_FUNC_TP_ACTIVITY_LOTTERY, false);
		//rResult = pkPlayer->AddItem(nItemId, nCount);
	}

	pkPlayer->SyncBackPackToClient(PACK_TYPE_BASIC);

	// 广播
	if (nBroadcastId)
	{
		for ( int i = 0; i < kItems.size(); i++ )
		{
			UT_ServerHelper::_GameNoticeFmort noticeFmort;
			noticeFmort.SetMsgID(nBroadcastId);
			noticeFmort.AddString(pkPlayer->GetOwnerCharName());//玩家名
			noticeFmort.AddActivityId(  BuildKey(m_iSchdeule, m_iCategory) );
			noticeFmort.AddItemQuality(kItems[i].X());
			noticeFmort.AddItemName(kItems[i].X());

			UT_ServerHelper::BrocastNoticeMsg(noticeFmort, pkPlayer->GetCitizenship());
		}
	}

	return RE_SUCCESS;
}

void ACT_LotteryDataEntry::FillStaticReward(const CSVFile::CF_ACTLotteryStaticReward::DataEntry* pkEntry)
{
	if (NULL == pkEntry)
	{
		return;
	}
	if (pkEntry->_kReward.empty())
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "1-Lottery Reward empty id:%d", pkEntry->_iID);
		T_ASSERT_CRITICAL_S(false);
		return;
	}

	LotteryStaticReward kReward;
	kReward.kItems = pkEntry->_kReward; 
	kReward.iBroadcastId = pkEntry->_lBroadcastID;
	m_StaticRewardData.insert( std::make_pair( pkEntry->_iTimes, kReward ) );
}

void ACT_LotteryDataEntry::FillReward(const CSVFile::CF_ACTLotteryReward::DataEntry* pkEntry)
{
	if (NULL == pkEntry)
	{
		return;
	}
	if (pkEntry->_kReward.empty())
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "2-Lottery Reward empty! id:%d", pkEntry->_iID);
		T_ASSERT_CRITICAL_S(false);
		return;
	}

	LotteryReward kReward;
	kReward.kItems = pkEntry->_kReward; 
	kReward.iWeight = pkEntry->_iCount > 0 ? 0 : pkEntry->_iWeight;
	kReward.iCount = pkEntry->_iCount;
	kReward.iBroadcastId = pkEntry->_lBroadcastID;

	m_RewardVec.push_back(kReward);
	if (kReward.iCount > 0)
	{
		m_CountIndexMap[kReward.iCount] = (int32_t)m_RewardVec.size() - 1;
	}
}

struct LotteryChargeScheduleOper : public Activity::ACT_ScheduleSaveDataOper<ACT_LotteryManager::DataEntryType>
{
	LotteryChargeScheduleOper(ACT_ScheduleManager* schedule, LC_ServerPlayer* player, ACT_ScheduleAsset* asset, ACT_ScheduleSaveActivity* act, ACT_ScheduleSaveCatagory* cat)
		:Activity::ACT_ScheduleSaveDataOper<ACT_LotteryManager::DataEntryType>(schedule, player, asset, act, cat)
		,iValue(0)
	{}
	virtual bool operator ()(ACT_LotteryManager::ScheduleDataType* pkSchedule)
	{
		if (pkScheduleMgr->IsScheduleValid(pkPlayer->GetServerID(), pkSchedule->m_iID))
		{
			ACT_ScheduleSaveEntry* pkSaveEntry = pkSaveCatagory->_addScheduleSaveEntry(pkSchedule->m_iID);
			if (pkSaveEntry)
			{
				pkSaveEntry->AddValue(iValue, ACT_SCHEDULE_CHARGE);
				pkSaveCatagory->UpdateDirtyFlag();
				pkSaveActivity->UpdateDirtyFlag();
				pkAsset->UpdateDirtyFlag();
				return true;
			}
		}
		return false;
	}
	int32_t iValue;
};

void ACT_LotteryManager::InitConfig(CSVFile::CSVDataLoader* pkLoader, const CSVFile::CSVDataSourceList* pkCSVs)
{
	ACT_ScheduleDataActivity<ACT_LotteryDataEntry, CSVFile::CF_ACTLottery>::InitConfig(pkLoader, pkCSVs);

	CF_ACTLotteryReward* pkLotteryReward = CF_ACTLotteryReward::GetSingletonPtr();
	CF_ACTLotteryReward::DataEntryMapExternIterator kIter = CF_ACTLotteryReward::GetSingletonPtr()->GetIterator();
	while (kIter.HasMoreElements())
	{
		do {
			const CF_ACTLotteryReward::DataEntry* pkEntry = kIter.PeekNextValuePtr();
			DataEntryType* pkDataEntry = GetDataEntry(pkEntry->_iCategory, pkEntry->_iSchedule, pkEntry->_iKey);
			if (NULL == pkDataEntry)
			{
				break;
			}
			pkDataEntry->FillReward(pkEntry);
		} while (0);
		kIter.MoveNext();
	}
	// static reward
	CF_ACTLotteryStaticReward* pkLotteryStaticReward = CF_ACTLotteryStaticReward::GetSingletonPtr();
	CF_ACTLotteryStaticReward::DataEntryMapExternIterator kStaticIter = CF_ACTLotteryStaticReward::GetSingletonPtr()->GetIterator();
	while (kStaticIter.HasMoreElements())
	{
		do {
			const CF_ACTLotteryStaticReward::DataEntry* pkEntry = kStaticIter.PeekNextValuePtr();
			DataEntryType* pkDataEntry = GetDataEntry(pkEntry->_iCategory, pkEntry->_iSchedule, 1);
			if (NULL == pkDataEntry)
			{
				break;
			}
			pkDataEntry->FillStaticReward(pkEntry);
		} while (0);
		kStaticIter.MoveNext();
	}
}

void ACT_LotteryManager::CheckConfig()
{
	CatagoryDataMap::iterator it = CatagoryDatas().begin();
	for (; it!=CatagoryDatas().end(); ++it)
	{
		CatagoryDataType& kCatagory = it->second;
		mem::map<int32_t, ACT_ScheduleDataEntry<DataEntryType> >::iterator sit = kCatagory.m_kSchedules.begin();
		for (; sit != kCatagory.m_kSchedules.end(); ++sit)
		{
			mem::map<int32_t, DataEntryType>::iterator dit = sit->second.m_kDatas.begin();
			for (; dit != sit->second.m_kDatas.end(); ++dit)
			{
				if (dit->second.m_RewardVec.size() <= 0)
				{
					GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "3-Lottery Reward Empty! id:%d", sit->first);
					T_ASSERT_CRITICAL_S(false);
					return;
				}
			}
		}
	}
}

int ACT_LotteryManager::AddSingleRewardItem( int32_t nItemId, int32_t nCount, bool bAssociation )
{
	UT_SIMDataInfo data;
	data._iX = nItemId;
	data._iY = nCount;
	data._iZ = bAssociation;
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "LOTTERY_REWARD item: %d count: %d, Association: %d", nItemId , nCount, bAssociation);
	m_MultiRewardItems.push_back(data);
	return 1;
}

void ACT_LotteryManager::AddCharge(LC_ServerPlayer* pkPlayer, int32_t val, const mem::vector<int32_t>& kCatagorys)
{
	if (NULL==pkPlayer || !val)
	{
		return;
	}
	static ACT_ScheduleManager* pkScheduleMgr = ACT_ScheduleManager::GetSingletonPtr();
	Activity::ACT_ScheduleAsset& rkAsset = pkPlayer->GetACTScheduleAsset();
	Activity::ACT_ScheduleSaveActivity*	pkSaveActivity = rkAsset._addScheduleSaveActivity(GetType());
	if (NULL==pkSaveActivity)
	{
		return;
	}

	LotteryChargeScheduleOper kOper(pkScheduleMgr, pkPlayer, &rkAsset, pkSaveActivity, NULL);
	kOper.iValue = val;

	for (size_t i = 0; i < kCatagorys.size(); ++i)
	{
		int32_t iCatagory = kCatagorys.at(i);
		CatagoryDataType* pkCatagory = GetCatagoryData(iCatagory);
		if (NULL == pkCatagory)
		{
			continue;
		}

		kOper.pkSaveCatagory = pkSaveActivity->_addScheduleSaveCatagory(iCatagory);
		if (kOper.pkSaveCatagory)
		{
			pkCatagory->ForAll(&kOper);
		}
	}
}

void ACT_LotteryManager::OnCharge(LC_ServerPlayer* pkPlayer, int32_t val, const GameLogic::LC_IDSet* pkCatagorys)
{
	if (NULL==pkPlayer || !val)
	{
		return;
	}
	static ACT_ScheduleManager* pkScheduleMgr = ACT_ScheduleManager::GetSingletonPtr();
	Activity::ACT_ScheduleAsset& rkAsset = pkPlayer->GetACTScheduleAsset();
	Activity::ACT_ScheduleSaveActivity*	pkSaveActivity = rkAsset._addScheduleSaveActivity(GetType());
	if (NULL==pkSaveActivity)
	{
		return;
	}

	LotteryChargeScheduleOper kOper(pkScheduleMgr, pkPlayer, &rkAsset, pkSaveActivity, NULL);
	kOper.iValue = val;

	if (NULL==pkCatagorys)
	{
		CatagoryDataMap::iterator it = CatagoryDatas().begin();
		for (; it!=CatagoryDatas().end(); ++it)
		{
			kOper.pkSaveCatagory = pkSaveActivity->_addScheduleSaveCatagory(it->first);
			if (kOper.pkSaveCatagory)
			{
				it->second.ForAll(&kOper);
			}
		}
	}
	else
	{
		LC_IDSet::const_iterator cit = pkCatagorys->begin();
		for (; cit!=pkCatagorys->end(); ++cit)
		{
			CatagoryDataType* pkCatagory = GetCatagoryData(*cit);
			if (NULL == pkCatagory)
			{
				continue;
			}

			kOper.pkSaveCatagory = pkSaveActivity->_addScheduleSaveCatagory(*cit);
			if (kOper.pkSaveCatagory)
			{
				pkCatagory->ForAll(&kOper);
			}
		}
	}
}

ResultType ACT_LotteryManager::DrawReward(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t iSchedule, int32_t iKey, GameLogic::LC_RewardItemsExtend& rkExtend, const GameLogic::LC_IDVector* pkExtraParams, int& count)
{
	return RE_FAIL;
}

ResultType ACT_LotteryManager::DrawSingleReward(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t iSchedule, int32_t iKey, GameLogic::LC_RewardItemsExtend& rkExtend, const GameLogic::LC_IDVector* pkExtraParams)
{
	return RE_FAIL;
}
