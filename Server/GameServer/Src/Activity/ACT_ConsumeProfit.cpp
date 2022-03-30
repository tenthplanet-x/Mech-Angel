#include "ACT_ConsumeProfit.h"
#include "ACT_ActivitySchedule.h"

#include "LC_ServerPlayer.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerLogicManager.h"

#include "UT_ServerHelper.h"

using namespace CSVFile;
using namespace GameLogic;
using namespace Activity;

ResultType ACT_ConsumeProfitDataEntry::DoReward(LC_ServerPlayer* pkPlayer, ACT_ScheduleSaveEntry* pkEntry, GameLogic::LC_RewardItemsExtend& rkExtend)
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
			uiRslt = UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, kItems, pkPlayer, SYS_FUNC_TP_ACTIVITY_CONSUME
				, m_iID, m_sMailTitle, m_sMailDesc, m_bMailEnable);
			if (RE_SUCCESS != uiRslt)
			{
				break;
			}
		}
	}while(0);

	return uiRslt;
}

struct ConsumeScheduleOper : public Activity::ACT_ScheduleSaveDataOper<ACT_ConsumeProfitManager::DataEntryType>
{
	ConsumeScheduleOper(ACT_ScheduleManager* schedule, LC_ServerPlayer* player, ACT_ScheduleAsset* asset, ACT_ScheduleSaveActivity* act, ACT_ScheduleSaveCatagory* cat)
		:Activity::ACT_ScheduleSaveDataOper<ACT_ConsumeProfitManager::DataEntryType>(schedule, player, asset, act, cat)
		,iValue(0)
	{}

	virtual bool operator ()(ACT_ConsumeProfitManager::ScheduleDataType* pkSchedule)
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

void ACT_ConsumeProfitManager::AddConsume(LC_ServerPlayer* pkPlayer, int32_t val, const mem::vector<int32_t>& kCatagorys)
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

	ConsumeScheduleOper kOper(pkScheduleMgr, pkPlayer, &rkAsset, pkSaveActivity, NULL);
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
void ACT_ConsumeProfitManager::OnConsume(LC_ServerPlayer* pkPlayer, int32_t val, const GameLogic::LC_IDSet* pkCatagorys)
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

	ConsumeScheduleOper kOper(pkScheduleMgr, pkPlayer, &rkAsset, pkSaveActivity, NULL);
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
ResultType ACT_ConsumeProfitManager::DrawReward(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t iSchedule, int32_t iKey, GameLogic::LC_RewardItemsExtend& rkExtend, const GameLogic::LC_IDVector* pkExtraParams, int& count)
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

	if (iKey > (int32_t)pkSaveSchedule->GetCount())
	{
		return RE_ACTIVITY_CONDITION_FAIL;
	}

	if (pkSaveSchedule->IsRewardReg(pkDataEntry->m_iSaveIndex))
	{
		return RE_ACTIVITY_REWARDED;
	}
	
	ResultType rslt = pkDataEntry->DoReward(pkPlayer, pkSaveSchedule, rkExtend);
	if (RE_SUCCESS == rslt)
	{
		pkSaveSchedule->RegReward(pkDataEntry->m_iSaveIndex);
		pkSaveCatagory->UpdateDirtyFlag();
		pkSaveActivity->UpdateDirtyFlag();
		pkPlayer->GetACTScheduleAsset().UpdateDirtyFlag();
	}
	return rslt;
}
