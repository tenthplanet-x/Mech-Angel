#include "ACT_ChargeInterval.h"
#include "ACT_ActivitySchedule.h"

#include "LC_ServerPlayer.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerLogicManager.h"

#include "UT_ServerHelper.h"

using namespace CSVFile;
using namespace GameLogic;
using namespace Activity;

ResultType ACT_ChargeIntervalDataEntry::DoReward(LC_ServerPlayer* pkPlayer, ACT_ScheduleSaveEntry* pkEntry, GameLogic::LC_RewardItemsExtend& rkExtend)
{
	if (NULL==pkPlayer || NULL==pkEntry)
	{
		return RE_FAIL;
	}

	ResultType uiRslt = RE_SUCCESS;
	do
	{
		Utility::UT_SIMDataList kItems;
		rkExtend.SetChoice(m_kRewardItems.CheckChoiceID(pkPlayer->GetWeaponAttribute(), rkExtend.Choice()));
		m_kRewardItems.Resolve(pkPlayer, kItems, &rkExtend);
		if (!kItems.empty())
		{
			uiRslt = UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, kItems, pkPlayer, SYS_FUNC_TP_ACTIVITY_CHARGE_INTERVAL
				, m_iID, m_sMailTitle, m_sMailDesc, m_bMailEnable);
			if (RE_SUCCESS != uiRslt)
			{
				break;
			}
		}
	}while(0);

	return uiRslt;
}

struct ChargeIntevalScheduleOper : public Activity::ACT_ScheduleSaveDataOper<ACT_ChargeIntervalManager::DataEntryType>
{
	ChargeIntevalScheduleOper(ACT_ScheduleManager* schedule, LC_ServerPlayer* player, ACT_ScheduleAsset* asset, ACT_ScheduleSaveActivity* act, ACT_ScheduleSaveCatagory* cat)
		:Activity::ACT_ScheduleSaveDataOper<ACT_ChargeIntervalManager::DataEntryType>(schedule, player, asset, act, cat)
		,iValue(0)
	{}
	virtual bool operator ()(ACT_ChargeIntervalManager::ScheduleDataType* pkSchedule)
	{
		if (pkScheduleMgr->IsScheduleValid(pkPlayer->GetServerID(), pkSchedule->m_iID))
		{
			ACT_ScheduleSaveEntry* pkSaveEntry = pkSaveCatagory->_addScheduleSaveEntry(pkSchedule->m_iID);
			if (pkSaveEntry)
			{
				const LC_IDVector& rkKeys = pkSchedule->Keys();
				ACT_ChargeIntervalManager::DataEntryType* pkTarDataEntry = NULL;
				for (LC_IDVector::const_iterator citKey = rkKeys.begin(); citKey != rkKeys.end(); ++citKey)
				{
					ACT_ChargeIntervalManager::DataEntryType* pkRewardDataEntry = pkSchedule->GetDataEntry(*citKey);
					if (NULL == pkRewardDataEntry)
					{
						continue;  
					}

					if (iValue<pkRewardDataEntry->m_iKey)
					{
						continue;
					}

					if (pkTarDataEntry && pkTarDataEntry->m_iKey>=pkRewardDataEntry->m_iKey)
					{
						continue;
					}

					pkTarDataEntry = pkRewardDataEntry;
				}

				if (pkTarDataEntry)
				{
					ACT_ScheduleSaveCommon* pkSaveDetail = pkSaveEntry->_addScheduleSaveDetail(pkTarDataEntry->Key());
					int32_t iTimesKey = GET_SCHEDULE_KEY(LC_ST_TP_FORERVER, ACT_SCHEDULE_DETAIL_COUNT);
					if (pkSaveDetail && (pkTarDataEntry->m_iTimeLimits<0 || (int32_t)pkSaveDetail->GetValue(iTimesKey)<pkTarDataEntry->m_iTimeLimits))
					{
						//as reward times!
						pkSaveDetail->AddValue(iTimesKey, 1);
						pkSaveEntry->RegFire(pkTarDataEntry->m_iSaveIndex);	
						pkSaveEntry->RegFire(pkTarDataEntry->m_iSaveIndex, LC_ST_TP_DAILY);	
						pkSaveCatagory->UpdateDirtyFlag();
						pkSaveActivity->UpdateDirtyFlag();
						pkAsset->UpdateDirtyFlag();
						return true;
					}
				}
			}
		}
		return false;
	}
	int32_t iValue;
};

void ACT_ChargeIntervalManager::AddCharge(LC_ServerPlayer* pkPlayer, int32_t val, const mem::vector<int32_t>& kCatagorys)
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

	ChargeIntevalScheduleOper kOper(pkScheduleMgr, pkPlayer, &rkAsset, pkSaveActivity, NULL);
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
void ACT_ChargeIntervalManager::OnCharge(LC_ServerPlayer* pkPlayer, int32_t val, const GameLogic::LC_IDSet* pkCatagorys)
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

	ChargeIntevalScheduleOper kOper(pkScheduleMgr, pkPlayer, &rkAsset, pkSaveActivity, NULL);
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
ResultType ACT_ChargeIntervalManager::DrawReward(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t iSchedule, int32_t iKey, GameLogic::LC_RewardItemsExtend& rkExtend, const GameLogic::LC_IDVector* pkExtraParams, int& count)
{
	if (NULL==pkPlayer)
	{
		return RE_FAIL;
	}

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

	ACT_ScheduleSaveActivity* pkSaveActivity = pkPlayer->GetACTScheduleAsset().GetScheduleSaveActivity(GetType());
	if (NULL == pkSaveActivity)
	{
		return RE_FAIL;
	}
 	ACT_ScheduleSaveCatagory* pkSaveCatagory = pkSaveActivity->GetScheduleSaveCatagory(iCatagory);
	if (NULL == pkSaveCatagory)
	{
		return RE_FAIL;
	}

	ACT_ScheduleSaveEntry* pkSaveSchedule = pkSaveCatagory->GetScheduleSaveEntry(iSchedule);
	if (NULL == pkSaveSchedule)
	{
		return RE_FAIL;
	}

	if (!pkSaveSchedule->IsFireReg(pkDataEntry->m_iSaveIndex))
	{
		return RE_ACTIVITY_CONDITION_FAIL;
	}

	ACT_ScheduleSaveCommon* pkSaveDetail = pkSaveSchedule->GetScheduleSaveDetail(iKey);
	int32_t iTimesKey = GET_SCHEDULE_KEY(LC_ST_TP_FORERVER, ACT_SCHEDULE_DETAIL_COUNT);
	int32_t iRewardKey = GET_SCHEDULE_KEY(LC_ST_TP_FORERVER, ACT_SCHEDULE_REWARD_COUNT);
	if (NULL == pkSaveDetail || pkSaveDetail->GetValue(iRewardKey) >= pkSaveDetail->GetValue(iTimesKey))
	{
		return RE_ACTIVITY_REWARDED;
	}

	ResultType rslt = pkDataEntry->DoReward(pkPlayer, pkSaveSchedule, rkExtend);
	if (RE_SUCCESS == rslt)
	{
		pkSaveSchedule->RegReward(pkDataEntry->m_iSaveIndex);
		pkSaveDetail->AddValue(iRewardKey, 1);
		pkSaveCatagory->UpdateDirtyFlag();
		pkSaveActivity->UpdateDirtyFlag();
		pkPlayer->GetACTScheduleAsset().UpdateDirtyFlag();
	}
	return rslt;
}
