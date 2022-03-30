#include "ACT_SignIn.h"
#include "ACT_ActivitySchedule.h"

#include "LC_ServerPlayer.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerLogicManager.h"

#include "UT_ServerHelper.h"

using namespace CSVFile;
using namespace GameLogic;
using namespace Activity;

ResultType ACT_SignInDataEntry::DoReward(LC_ServerPlayer* pkPlayer, ACT_ScheduleSaveEntry* pkEntry,
										GameLogic::LC_RewardItemsExtend& rkExtend, int32_t iMultiFlag)
{
	if (NULL==pkPlayer || NULL==pkEntry)
	{
		return RE_FAIL;
	}

	ResultType uiRslt = RE_SUCCESS;
	LC_CashMap kConsumes;
	if (iMultiFlag == 2)
	{
		kConsumes = m_kDoubleConsumes;
	}
	if (!kConsumes.empty())
	{
		uiRslt = LC_Helper::CheckCashMap(pkPlayer, kConsumes);
		if (RE_SUCCESS != uiRslt)
		{
			return uiRslt;
		}
	}
	else if (iMultiFlag > 1)
	{
		//多倍领奖必须配置消耗。
		return RE_FAIL;
	}

	do
	{
		Utility::UT_SIMDataList kItems;
		rkExtend.SetChoice(m_kRewardItems.CheckChoiceID(pkPlayer->GetWeaponAttribute(), rkExtend.Choice()));
		m_kRewardItems.Resolve(pkPlayer, kItems, &rkExtend);
		if (!kItems.empty())
		{
			if (iMultiFlag == 2)
			{
				Utility::UT_SIMDataList::iterator it = kItems.begin();
				for (; it != kItems.end(); ++it)
				{
					it->ItemCount(it->ItemCount() * 2);
				}
			}
			uiRslt = UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, kItems, pkPlayer, SYS_FUNC_TP_ACTIVITY_SIGNIN
				, m_iID, m_sMailTitle, m_sMailDesc, m_bMailEnable);
			if (RE_SUCCESS != uiRslt)
			{
				break;
			}
			if (!kConsumes.empty())
			{
				LC_Helper::PayCashMap(pkPlayer, kConsumes, SYS_FUNC_TP_ACTIVITY_SIGNIN, m_iID);
			}
		}
	}while(0);

	return uiRslt;
}

ResultType ACT_SignInManager::DrawReward(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t iSchedule, int32_t iKey, GameLogic::LC_RewardItemsExtend& rkExtend, const GameLogic::LC_IDVector* pkExtraParams, int& count)
{
	if (NULL==pkPlayer)
	{
		return RE_FAIL;
	}

	DataEntryType* pkRewardDataEntry = GetDataEntry(iCatagory, iSchedule, iKey);
	if (NULL == pkRewardDataEntry)
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

	ACT_ScheduleEntry* pkScheduleEntry = pkScheduleMgr->GetSchedule(pkPlayer->GetServerID(), iSchedule);
	if (NULL == pkScheduleEntry)
	{
		return RE_FAIL;
	}
	int32_t iDay = LC_Helper::GetDeltaDay(pkScheduleEntry->CurrentStartTime()/ONE_SECOND_MILI, GET_CURRENT_TIMESTAMP_IN_SECONDS());
	if (iDay < pkRewardDataEntry->m_iDay)
	{
		return RE_ACTIVITY_CONDITION_FAIL;
	}

	if (NULL==pkExtraParams || pkExtraParams->empty())
	{
		return RE_ACTIVITY_BAD_PARAMS;
	}
	int32_t iMultiFlag = pkExtraParams->at(ACT_EXTRA_PARAM_INDEX_1);
	if (iMultiFlag > 1 && iDay > pkRewardDataEntry->m_iDay)
	{
		return RE_ACTIVITY_CONDITION_FAIL;
	}

	if (pkSaveSchedule->IsRewardReg(pkRewardDataEntry->m_iSaveIndex))
	{
		return RE_ACTIVITY_REWARDED;
	}
	
	ResultType rslt = pkRewardDataEntry->DoReward(pkPlayer, pkSaveSchedule, rkExtend, iMultiFlag);
	if (RE_SUCCESS == rslt)
	{
		pkSaveSchedule->RegReward(pkRewardDataEntry->m_iSaveIndex);
		pkSaveCatagory->UpdateDirtyFlag();
		pkSaveActivity->UpdateDirtyFlag();
		pkPlayer->GetACTScheduleAsset().UpdateDirtyFlag();
	}
	return rslt;
}
