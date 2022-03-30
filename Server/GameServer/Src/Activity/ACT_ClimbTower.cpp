#include "ACT_ClimbTower.h"
#include "ACT_ActivityDataManager.h"
#include "ACT_ActivitySchedule.h"
#include "LC_ServerPlayer.h"
#include "UT_ServerHelper.h"
#include "LC_RumorManager.h"

using namespace CSVFile;
using namespace GameLogic;
using namespace Activity;
using namespace Utility;

ResultType ACT_ClimbTowerDataEntry::DoReward(LC_ServerPlayer* pkPlayer, ACT_ScheduleSaveEntry* pkEntry, GameLogic::LC_RewardItemsExtend& rkExtend, UT_SIMDataList& kRewards)
{
	if (NULL==pkPlayer || NULL==pkEntry)
	{
		return RE_FAIL;
	}

	ResultType uiRslt = RE_SUCCESS;
	do
	{
		rkExtend.SetChoice(m_kReward.CheckChoiceID(pkPlayer->GetWeaponAttribute(), rkExtend.Choice()));
		m_kReward.Resolve(pkPlayer, kRewards, &rkExtend);
		if (!kRewards.empty())
		{
			uiRslt = UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, kRewards, pkPlayer,
				SYS_FUNC_TP_ACTIVITY_CLIMB_TOWER, m_iKey, m_sMailTitle, m_sMailDesc, m_bMailEnable);
			if (RE_SUCCESS != uiRslt)
			{
				break;
			}
		}
	}while(0);

	return uiRslt;
}

ResultType ACT_ClimbTowerManager::RandomReward(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t iSchedule, int32_t iKey, GameLogic::LC_RewardItemsExtend& rkExtend, const GameLogic::LC_IDVector* pkExtraParams, UT_SIMDataList& kRewards)
{
	if (NULL == pkPlayer)
	{
		return RE_FAIL;
	}
	ACT_ScheduleManager* pkScheduleMgr = ACT_ScheduleManager::GetSingletonPtr();
	if (NULL == pkScheduleMgr)
	{
		return RE_FAIL;
	}
	if (!pkScheduleMgr->IsScheduleValid(pkPlayer->GetServerID(), iSchedule))
	{
		return RE_ACTIVITY_NOT_VALID;
	}
	ACT_ScheduleAsset& kScheduleAsset =  pkPlayer->GetACTScheduleAsset();
	ACT_ScheduleSaveActivity* pkSaveActivity = kScheduleAsset._addScheduleSaveActivity(GetType());
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

	int32_t iCurAchieve = pkSaveSchedule->GetAchieve() + 1;
	DataEntryType* pkEntry = GetDataEntry(iCatagory, iSchedule, iCurAchieve);
	if (NULL == pkEntry)
	{
		return RE_ACTIVITY_BAD_PARAMS;
	}

	int32_t iJumpAchieve = 0;
	LC_Helper::RandomTypeByWeight(pkEntry->m_kJumpWeight, pkEntry->m_iTotalWeight, iJumpAchieve);
	DataEntryType* pkJumpEntry = GetDataEntry(iCatagory, iSchedule, iJumpAchieve);
	if (NULL == pkJumpEntry)
	{
		return RE_ACTIVITY_CONFIG_INVALID;
	}

	if (NULL==pkExtraParams || pkExtraParams->size() < ACT_EXTRA_PARAM_MAX_COUNT)
	{
		return RE_FAIL;
	}
	int32_t iPayIndex = pkExtraParams->at(ACT_EXTRA_PARAM_INDEX_1);
	switch (iPayIndex)
	{
		case LC_PAY_WITH_ITEM:
		{
			if (RE_SUCCESS != LC_Helper::IsItemsEnough(pkPlayer->GetPackAsset(), pkEntry->m_kConsumes))
			{
				return RE_CONSUME_ITEM_NOT_ENOUGH;
			}
			break;
		}
		case LC_PAY_WITH_UNBIND_YUANBAO:
		{
			if (!pkPlayer->IsCashEnough(pkEntry->m_iReplaceGold, CT_UNBIND_YUANBAO))
			{
				return RE_UNBIND_YUANBAO_NOT_ENOUGH;
			}
			break;
		}
		default:
		{
			return RE_FAIL;
		}
	}
	ResultType rslt = pkEntry->DoReward(pkPlayer, pkSaveSchedule, rkExtend, kRewards);
	if (RE_SUCCESS != rslt)
	{
		return rslt;
	}
	LC_RumorManager* pkRumorMgr = LC_RumorManager::GetSingletonPtr();
	switch (iPayIndex)
	{
		case LC_PAY_WITH_ITEM:
		{
			PackOperateInfoList kConsumeOPs;
			LC_Helper::BuildConsumeItemOPs(pkEntry->m_kConsumes, kConsumeOPs);
			LC_Helper::DoItemOPs(pkPlayer, kConsumeOPs, SYS_FUNC_TP_ACTIVITY_CLIMB_TOWER, iCurAchieve);
			break;
		}
		case LC_PAY_WITH_UNBIND_YUANBAO:
		{
			pkPlayer->ReduceCash(pkEntry->m_iReplaceGold, CT_UNBIND_YUANBAO, SYS_FUNC_TP_ACTIVITY_CLIMB_TOWER, iCurAchieve);
			break;
		}
	}
	pkSaveSchedule->SetAchieve(iJumpAchieve - 1);
	kScheduleAsset.UpdateDirtyFlag();
	pkSaveActivity->UpdateDirtyFlag();
	pkSaveCatagory->UpdateDirtyFlag();
	return RE_SUCCESS;
}
