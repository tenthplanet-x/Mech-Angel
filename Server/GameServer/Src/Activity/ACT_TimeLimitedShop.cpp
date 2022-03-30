#include "ACT_TimeLimitedShop.h"
#include "ACT_ActivitySchedule.h"

#include "LC_ServerPlayer.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerLogicManager.h"

#include "UT_ServerHelper.h"
#include "oss_define.h"

using namespace CSVFile;
using namespace GameLogic;
using namespace Activity;

int32_t ACT_TimeLimitedShopDataEntry::Catagory() const
{
	return m_iType;
}

int32_t ACT_TimeLimitedShopDataEntry::Schedule() const
{
	return m_iSchdeule;
}

int32_t ACT_TimeLimitedShopDataEntry::Key() const
{
	return m_iKey;
}

ResultType ACT_TimeLimitedShopDataEntry::DoReward(LC_ServerPlayer* pkPlayer, ACT_ScheduleSaveEntry* pkEntry, GameLogic::LC_RewardItemsExtend& rkExtend, int32_t iDrawCount)
{
	if (NULL==pkPlayer || NULL==pkEntry)
	{
		return RE_FAIL;
	}

	ResultType uiRslt = RE_SUCCESS;
	Utility::UT_SIMDataList kItems;
	do
	{
		rkExtend.SetChoice(m_kItems.CheckChoiceID(pkPlayer->GetWeaponAttribute(), rkExtend.Choice()));
		m_kItems.Resolve(pkPlayer, kItems, &rkExtend);
		Utility::UT_SIMDataList::iterator it = kItems.begin();
		for (; it != kItems.end(); ++it)
		{
			Utility::UT_SIMDataInfo& DataEntry = *it;
			DataEntry.ItemCount(DataEntry.ItemCount() * iDrawCount);
		}
		if (!kItems.empty())
		{
			StringType mailTitle;
			StringType mailDesc;
			uiRslt = UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, kItems, pkPlayer, SYS_FUNC_TP_TIMELIMITED_SHOP
				, m_iID, mailTitle, mailDesc, false);
			if (RE_SUCCESS != uiRslt)
			{
				break;
			}
		}
	}while(0);

	return uiRslt;
}

ResultType ACT_TimeLimitedShopDataEntry::CheckConditions(LC_ServerPlayer* pkPlayer, ACT_ScheduleSaveEntry* pkSaveSchedule)
{
	return RE_SUCCESS;
}

void ACT_TimeLimitedShop::AddCharge(LC_ServerPlayer* pkPlayer, int32_t val, const mem::vector<int32_t>& kCatagorys)
{

}

void ACT_TimeLimitedShop::OnCharge(LC_ServerPlayer* pkPlayer, int32_t val, const GameLogic::LC_IDSet* pkCatagorys)
{

}

ResultType ACT_TimeLimitedShop::DrawReward(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t iSchedule, int32_t iKey, GameLogic::LC_RewardItemsExtend& rkExtend, const GameLogic::LC_IDVector* pkExtraParams, int& count)
{
	IF_A_NA_PLAYER(pkPlayer) return RE_FAIL;

	DataEntryType* pkDataEntry = GetDataEntry(iCatagory, iSchedule, iKey);
	if (NULL == pkDataEntry)
	{
		return RE_ACTIVITY_BAD_PARAMS;
	}

	//检测领取时间是否有效 领取的礼包是否正确
	int ACTShopTimeLimitedID = pkDataEntry->m_iID;
	if(!pkPlayer->CheckTimeLimitedActivityValid(ACTShopTimeLimitedID))
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

	if (NULL==pkExtraParams || pkExtraParams->size() < ACT_EXTRA_PARAM_MAX_COUNT)
	{
		return RE_FAIL;
	}
	int32_t iDrawCount = pkExtraParams->at(ACT_EXTRA_PARAM_INDEX_1);
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

	ResultType rslt = LC_Helper::CheckCashMap(pkPlayer, pkDataEntry->m_kCurrentPrices, iDrawCount);
	if (RE_SUCCESS != rslt)
	{
		return rslt;
	}

	rslt = pkDataEntry->DoReward(pkPlayer, pkSaveSchedule, rkExtend, iDrawCount);
	if (RE_SUCCESS == rslt)
	{
		//reg stat
		//reduce price
		LC_Helper::PayCashMap(pkPlayer, pkDataEntry->m_kCurrentPrices, SYS_FUNC_TP_ACTIVITY_SHOP, pkDataEntry->m_iID, iDrawCount);
		
		Utility::UT_SIMDataMap::iterator it = pkDataEntry->m_kLimits.begin();
		for (; it!=pkDataEntry->m_kLimits.end(); ++it)
		{
			Utility::UT_SIMDataInfo& rkLimit = it->second;
			if(!GameLogic::IS_VALID_STAT_TYPE(rkLimit.StatType()))
			{
				continue;
			}

			int32_t iStatKey = GET_SCHEDULE_KEY(rkLimit.StatType(), ACT_SCHEDULE_DETAIL_COUNT);
			pkSaveDetail->AddValue(iStatKey, iDrawCount);
		}

		pkSaveSchedule->UpdateDirtyFlag();
		pkSaveCatagory->UpdateDirtyFlag();
		pkSaveActivity->UpdateDirtyFlag();
		pkPlayer->GetACTScheduleAsset().UpdateDirtyFlag();

		//updateTimeLimitedActivity
		pkPlayer->SetTimeLimitedActivityState(ACTShopTimeLimitedID, ACTIVITE_REWARD);

	}
	return rslt;
}
