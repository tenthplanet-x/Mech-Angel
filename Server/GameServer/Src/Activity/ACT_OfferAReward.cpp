#include "ACT_OfferAReward.h"
#include "ACT_ActivitySchedule.h"

#include "LC_ServerPlayer.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerLogicManager.h"

#include "UT_ServerHelper.h"

using namespace CSVFile;
using namespace GameLogic;
using namespace Activity;

void ACT_OfferARewardDataEntry::GetValidActivitys(ACT_Type2CatasMap& rkActivitys)
{
	static ACT_ScheduleDataManager* pkScheduleDataMgr = ACT_ScheduleDataManager::GetSingletonPtr();
	{
		ACT_Type2CatasMap::iterator it = m_kInActivitys.begin();
		for (; it!=m_kInActivitys.end(); ++it)
		{
			int32_t iType = it->first;
			ACT_ScheduleDataActivityBase* pkActivity = pkScheduleDataMgr->GetActivity(iType);
			if (NULL == pkActivity)
			{
				continue;
			}
			GameLogic::LC_IDSet& rkSrcCatas = it->second;
			GameLogic::LC_IDSet& rkTarCatas = rkActivitys[iType];
			if (rkSrcCatas.empty())
			{
				const GameLogic::LC_IDSet& rkAllCatas = pkActivity->CatagoryIDs();
				TRANSFER_CONTAINER_DATAS(GameLogic::LC_IDSet, rkAllCatas, rkTarCatas, insert);
			}
			else
			{
				TRANSFER_CONTAINER_DATAS(GameLogic::LC_IDSet, rkSrcCatas, rkTarCatas, insert);
			}
		}
	}

	{
		ACT_Type2CatasMap::iterator it = m_kExcludeActivitys.begin();
		for (; it!=m_kExcludeActivitys.end(); ++it)
		{
			int32_t iType = it->first;
			ACT_ScheduleDataActivityBase* pkActivity = pkScheduleDataMgr->GetActivity(iType);
			if (NULL == pkActivity)
			{
				continue;
			}
			ACT_Type2CatasMap::iterator itTar = rkActivitys.find(iType);
			if (itTar == rkActivitys.end())
			{
				continue;
			}

			GameLogic::LC_IDSet& rkSrcCatas = it->second;
			if (rkSrcCatas.empty())
			{
				rkActivitys.erase(iType);
			}
			else
			{
				GameLogic::LC_IDSet& rkTarCatas = itTar->second;
				GameLogic::LC_IDSet::const_iterator citE = rkSrcCatas.begin();
				for (; citE!=rkSrcCatas.end(); ++citE)
				{
					rkTarCatas.erase(*citE);
				}
			}
		}
	}
}
ResultType ACT_OfferARewardDataEntry::DoReward(LC_ServerPlayer* pkPlayer, ACT_ScheduleSaveEntry* pkEntry, GameLogic::LC_RewardItemsExtend& rkExtend, int32_t iDrawCount)
{
	if (NULL==pkPlayer || NULL==pkEntry)
	{
		return RE_FAIL;
	}

	ResultType uiRslt = RE_SUCCESS;
	do
	{
		static ACT_ScheduleDataManager* pkScheduleDataMgr = ACT_ScheduleDataManager::GetSingletonPtr();
		int32_t iFinalScore = m_iScore * iDrawCount;
		ACT_Type2CatasMap kValidActivitys;
		GetValidActivitys(kValidActivitys);
		pkScheduleDataMgr->OfferScore(pkPlayer, iFinalScore, kValidActivitys);
	}while(0);

	return uiRslt;
}

ResultType ACT_OfferARewardDataEntry::CheckConditions(LC_ServerPlayer* pkPlayer, ACT_ScheduleSaveEntry* pkSaveSchedule, ACT_ScheduleSaveCommon* pkSaveDetail, int32_t iDrawCount)
{
	IF_A_NA_PLAYER(pkPlayer) return RE_FAIL;

	Utility::UT_SIMDataList::iterator it = m_kBuyConditions.begin();
	for (; it != m_kBuyConditions.end(); ++it)
	{
		switch (it->ID())
		{
		case ACT_OAR_COND_CHARGE:
			{
				int32_t iCountKey = GET_SCHEDULE_KEY(LC_ST_TP_FORERVER, ACT_SCHEDULE_DETAIL_COUNT);
			
				if ((int32_t)pkSaveSchedule->GetCount() / it->ParamA() < (int32_t)pkSaveDetail->GetValue(iCountKey) + iDrawCount)
				{
					return RE_ACTIVITY_CONDITION_FAIL;
				}
			}
			break;
		default:
			return RE_FAIL;
		}
	}
	return RE_SUCCESS;
}

struct OfferARewardChargeScheduleOper : public Activity::ACT_ScheduleSaveDataOper<ACT_OfferARewardManager::DataEntryType>
{
	OfferARewardChargeScheduleOper(ACT_ScheduleManager* schedule, LC_ServerPlayer* player, ACT_ScheduleAsset* asset, ACT_ScheduleSaveActivity* act, ACT_ScheduleSaveCatagory* cat)
		:Activity::ACT_ScheduleSaveDataOper<ACT_OfferARewardManager::DataEntryType>(schedule, player, asset, act, cat)
		,iValue(0)
	{}
	virtual bool operator ()(ACT_OfferARewardManager::ScheduleDataType* pkSchedule)
	{
		if (pkScheduleMgr->IsScheduleValid(pkPlayer->GetServerID(), pkSchedule->m_iID))
		{
			ACT_ScheduleSaveEntry* pkSaveEntry = pkSaveCatagory->_addScheduleSaveEntry(pkSchedule->m_iID);
			if (pkSaveEntry)
			{
				pkSaveEntry->AddCount(iValue);	
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

void ACT_OfferARewardManager::AddCharge(LC_ServerPlayer* pkPlayer, int32_t val, const mem::vector<int32_t>& kCatagorys)
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

	OfferARewardChargeScheduleOper kOper(pkScheduleMgr, pkPlayer, &rkAsset, pkSaveActivity, NULL);
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

void ACT_OfferARewardManager::OnCharge(LC_ServerPlayer* pkPlayer, int32_t val, const GameLogic::LC_IDSet* pkCatagorys)
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

	OfferARewardChargeScheduleOper kOper(pkScheduleMgr, pkPlayer, &rkAsset, pkSaveActivity, NULL);
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
ResultType ACT_OfferARewardManager::DrawReward(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t iSchedule, int32_t iKey, GameLogic::LC_RewardItemsExtend& rkExtend, const GameLogic::LC_IDVector* pkExtraParams, int& count)
{
	IF_A_NA_PLAYER(pkPlayer) return RE_FAIL;

	DataEntryType* pkDataEntry = GetDataEntry(iCatagory, iSchedule, iKey);
	if (NULL == pkDataEntry)
	{
		return RE_ACTIVITY_BAD_PARAMS;
	}

	static ACT_ScheduleManager* pkScheduleMgr = ACT_ScheduleManager::GetSingletonPtr();
	if (!pkScheduleMgr->IsScheduleValid(pkPlayer->GetServerID(), iSchedule))
	{
		return RE_ACTIVITY_NOT_VALID;
	}

	ACT_ScheduleSaveActivity* pkSaveActivity = pkPlayer->GetACTScheduleAsset()._addScheduleSaveActivity(GetType());
	if (NULL == pkSaveActivity)
	{
		return RE_FAIL;
	}
	ACT_ScheduleSaveCatagory* pkSaveCatagory = pkSaveActivity->_addScheduleSaveCatagory(iCatagory);
	if (NULL == pkSaveCatagory)
	{
		return RE_FAIL;
	}

	ACT_ScheduleSaveEntry* pkSaveSchedule = pkSaveCatagory->_addScheduleSaveEntry(iSchedule);
	if (NULL == pkSaveSchedule)
	{
		return RE_FAIL;
	}

	ACT_ScheduleSaveCommon* pkSaveDetail = pkSaveSchedule->_addScheduleSaveDetail(iKey);
	if (NULL == pkSaveDetail)
	{
		return RE_FAIL;
	}

	if (NULL==pkExtraParams || pkExtraParams->size() < 2)
	{
		return RE_FAIL;
	}

	int32_t iDrawCount = pkExtraParams->at(1);
	iDrawCount = iDrawCount < 1 ? 1 : iDrawCount;
	{
		//check limit
		bool  bValid = true;
		Utility::UT_SIMDataMap::iterator it = pkDataEntry->m_kLimits.begin();
		for (; it!=pkDataEntry->m_kLimits.end(); ++it)
		{
			Utility::UT_SIMDataInfo& rkLimit = it->second;
			if(!GameLogic::IS_VALID_STAT_TYPE(rkLimit.StatType()))
			{
				continue;
			}

			int32_t iStatKey = GET_SCHEDULE_KEY(rkLimit.StatType(), ACT_SCHEDULE_DETAIL_COUNT);
			if ((int32_t)pkSaveDetail->GetValue(iStatKey) + iDrawCount > rkLimit.StatValue())
			{
				bValid = false;
				break;
			}
		}
		if (!bValid)
		{
			return RE_ACTIVITY_TIMES_OUT;
		}
	}

	ResultType rslt = pkDataEntry->CheckConditions(pkPlayer, pkSaveSchedule, pkSaveDetail, iDrawCount);
	if (RE_SUCCESS != rslt)
	{
		return rslt;
	}

	rslt = LC_Helper::CheckCashMap(pkPlayer, pkDataEntry->m_kItems, iDrawCount, pkDataEntry->m_bForbidExpire);
	if (RE_SUCCESS != rslt)
	{
		return rslt;
	}

	rslt = pkDataEntry->DoReward(pkPlayer, pkSaveSchedule, rkExtend, iDrawCount);
	if (RE_SUCCESS == rslt)
	{
		//StringType sRewards;
		//reg stat
		//reduce price
		LC_Helper::PayCashMap(pkPlayer, pkDataEntry->m_kItems, SYS_FUNC_TP_ACTIVITY_OFF_A_REWARD, pkDataEntry->m_iID, iDrawCount, pkDataEntry->m_bForbidExpire);

		Utility::UT_SIMDataMap::iterator it = pkDataEntry->m_kLimits.begin();
		for (; it!=pkDataEntry->m_kLimits.end(); ++it)
		{
			Utility::UT_SIMDataInfo& rkLimit = it->second;
			if(!GameLogic::IS_VALID_STAT_TYPE(rkLimit.StatType()))
			{
				continue;
			}
			if (LC_ST_TP_FORERVER == rkLimit.StatType())
			{
				continue;
			}
			int32_t iStatKey = GET_SCHEDULE_KEY(rkLimit.StatType(), ACT_SCHEDULE_DETAIL_COUNT);
			pkSaveDetail->AddValue(iStatKey, iDrawCount);
		}

		int32_t iCountKey = GET_SCHEDULE_KEY(LC_ST_TP_FORERVER, ACT_SCHEDULE_DETAIL_COUNT);
		pkSaveDetail->AddValue(iCountKey, iDrawCount);
		pkSaveSchedule->UpdateDirtyFlag();
		pkSaveCatagory->UpdateDirtyFlag();
		pkSaveActivity->UpdateDirtyFlag();
		pkPlayer->GetACTScheduleAsset().UpdateDirtyFlag();
	}
	return RE_SUCCESS;
}
