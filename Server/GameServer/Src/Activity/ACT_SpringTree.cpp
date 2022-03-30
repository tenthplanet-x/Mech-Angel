#include "ACT_SpringTree.h"
#include "ACT_ActivityDataManager.h"
#include "ACT_ActivitySchedule.h"
#include "LC_ServerPlayer.h"
#include "UT_ServerHelper.h"
#include "SC_ScriptAction.h"
#include "GameServerApp.h"
#include "LC_RumorManager.h"

using namespace CSVFile;
using namespace GameLogic;
using namespace Activity;

ResultType ACT_SpringTreeDataEntry::DoReward(LC_ServerPlayer* pkPlayer, ACT_ScheduleSaveEntry* pkEntry, GameLogic::LC_RewardItemsExtend& rkExtend)
{
	if (NULL == pkPlayer || NULL == pkEntry)
	{
		return RE_FAIL;
	}

	ResultType uiRslt = RE_SUCCESS;
	do
	{
		Utility::UT_SIMDataList kItems;
		rkExtend.SetChoice(m_kReward.CheckChoiceID(pkPlayer->GetWeaponAttribute(), rkExtend.Choice()));
		m_kReward.Resolve(pkPlayer, kItems, &rkExtend);
		if (!kItems.empty())
		{
			uiRslt = UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, kItems, pkPlayer, SYS_FUNC_TP_ACTIVITY_SNOW_MAN
				, m_iID, m_sMailTitle, m_sMailDesc, m_bMailEnable);
			if (RE_SUCCESS != uiRslt)
			{
				break;
			}
		}
	} while (0);

	return uiRslt;
}

int32_t _resolveScoreByType(int32_t iType, int32_t iVal)
{
	int32_t iScore = 0;
	SA_RT_TRIGGER_ACTION(int32_t, iScore, "ActSpringTreeResolveScore", iType, iVal);
	return iScore;
}
struct SpringTreeScheduleOper : public Activity::ACT_ScheduleSaveDataOper<ACT_SpringTreeManager::DataEntryType>
{
	SpringTreeScheduleOper(ACT_ScheduleManager* schedule, LC_ServerPlayer* player, ACT_ScheduleAsset* asset, ACT_ScheduleSaveActivity* act, ACT_ScheduleSaveCatagory* cat)
	:Activity::ACT_ScheduleSaveDataOper<ACT_SpringTreeManager::DataEntryType>(schedule, player, asset, act, cat)
	, iType(0)
	, iValue(0)
	{}

	virtual bool operator ()(ACT_SpringTreeManager::ScheduleDataType* pkSchedule)
	{
		if (iValue <= 0)
		{
			return false;
		}
		if (pkScheduleMgr->IsScheduleValid(pkPlayer->GetServerID(), pkSchedule->m_iID))
		{
			ACT_ScheduleSaveEntry* pkSaveEntry = pkSaveCatagory->_addScheduleSaveEntry(pkSchedule->m_iID);
			if (NULL != pkSaveEntry)
			{
				int32_t iAddScore = 0;
				int32_t iOldVal = 0;
				int32_t iOldScore = 0;
				int32_t iCurVal = 0;
				int32_t iCurScore = 0;
				ACT_ScheduleDataFlag iFlag = ACT_SCHEDULE_BEGIN;
				switch (iType)
				{
				case E_TYPE_SCORE_CHARGE:
					iFlag = ACT_SCHEDULE_CHARGE;
					break;
				case E_TYPE_SCORE_CONSUME:
					iFlag = ACT_SCHEDULE_COMSUME;
					break;
				case E_TYPE_SCORE_ONLINE:
					iFlag = ACT_SCHEDULE_ONLINE;
					break;
				default:
					return false;
				}
				iOldVal = pkSaveEntry->GetValue(iFlag, GameLogic::LC_ST_TP_DAILY);
				iOldScore = _resolveScoreByType(iType, iOldVal);
				iCurVal = iOldVal + iValue;
				iCurScore = _resolveScoreByType(iType, iCurVal);
				pkSaveEntry->SetValue(iCurVal, iFlag, GameLogic::LC_ST_TP_DAILY);
				iAddScore = iCurScore - iOldScore;
				if (iAddScore > 0)
				{
					pkSaveEntry->AddActShopScore(iAddScore);
					pkSaveEntry->AddCount(iAddScore);
					pkSaveCatagory->UpdateDirtyFlag();
					pkSaveActivity->UpdateDirtyFlag();
					pkAsset->UpdateDirtyFlag();
				}
				return true;
			}
		}
		return false;
	}
	int32_t iType;
	int32_t iValue;
};
void ACT_SpringTreeManager::_onAddScore(LC_ServerPlayer* pkPlayer, int iType, int32_t val, const GameLogic::LC_IDSet* pkCatagorys)
{
	if (NULL == pkPlayer || val <= 0)
	{
		return;
	}
	static ACT_ScheduleManager* pkScheduleMgr = ACT_ScheduleManager::GetSingletonPtr();
	Activity::ACT_ScheduleAsset& rkAsset = pkPlayer->GetACTScheduleAsset();
	Activity::ACT_ScheduleSaveActivity*	pkSaveActivity = rkAsset._addScheduleSaveActivity(GetType());
	if (NULL == pkSaveActivity)
	{
		return;
	}

	SpringTreeScheduleOper kOper(pkScheduleMgr, pkPlayer, &rkAsset, pkSaveActivity, NULL);
	kOper.iValue = val;
	kOper.iType = iType;

	if (NULL == pkCatagorys)
	{
		CatagoryDataMap::iterator it = CatagoryDatas().begin();
		for (; it != CatagoryDatas().end(); ++it)
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
		for (; cit != pkCatagorys->end(); ++cit)
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
void ACT_SpringTreeManager::OnCharge(LC_ServerPlayer* pkPlayer, int32_t val, const GameLogic::LC_IDSet* pkCatagorys)
{
	_onAddScore(pkPlayer, E_TYPE_SCORE_CHARGE, val, pkCatagorys);
}
void ACT_SpringTreeManager::OnConsume(LC_ServerPlayer* pkPlayer, int32_t val, const GameLogic::LC_IDSet* pkCatagorys)
{
	_onAddScore(pkPlayer, E_TYPE_SCORE_CONSUME, val, pkCatagorys);
}
void ACT_SpringTreeManager::OnOnlineTime(LC_ServerPlayer* pkPlayer, int32_t val, const GameLogic::LC_IDSet* pkCatagorys)
{
	_onAddScore(pkPlayer, E_TYPE_SCORE_ONLINE, val, pkCatagorys);
}

struct SpringTreeScheduleAddScoreOper : public Activity::ACT_ScheduleSaveDataOper<ACT_SpringTreeManager::DataEntryType>
{
	SpringTreeScheduleAddScoreOper(ACT_ScheduleManager* schedule, LC_ServerPlayer* player, ACT_ScheduleAsset* asset, ACT_ScheduleSaveActivity* act, ACT_ScheduleSaveCatagory* cat)
		:Activity::ACT_ScheduleSaveDataOper<ACT_SpringTreeManager::DataEntryType>(schedule, player, asset, act, cat)
		, iValue(0)
	{}

	virtual bool operator ()(ACT_SpringTreeManager::ScheduleDataType* pkSchedule)
	{
		if (iValue <= 0)
		{
			return false;
		}
		if (pkScheduleMgr->IsScheduleValid(pkPlayer->GetServerID(), pkSchedule->m_iID))
		{
			ACT_ScheduleSaveEntry* pkSaveEntry = pkSaveCatagory->_addScheduleSaveEntry(pkSchedule->m_iID);
			if (NULL != pkSaveEntry)
			{
				pkSaveEntry->AddActShopScore(iValue);
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
void ACT_SpringTreeManager::AddScore(LC_ServerPlayer* pkPlayer, int32_t val,  const mem::vector<int32_t>& kCatagorys)
{
	if (NULL == pkPlayer || val <= 0)
	{
		return;
	}
	static ACT_ScheduleManager* pkScheduleMgr = ACT_ScheduleManager::GetSingletonPtr();
	Activity::ACT_ScheduleAsset& rkAsset = pkPlayer->GetACTScheduleAsset();
	Activity::ACT_ScheduleSaveActivity*	pkSaveActivity = rkAsset._addScheduleSaveActivity(GetType());
	if (NULL == pkSaveActivity)
	{
		return;
	}

	SpringTreeScheduleAddScoreOper kOper(pkScheduleMgr, pkPlayer, &rkAsset, pkSaveActivity, NULL);
	kOper.iValue = val;

	mem::vector<int32_t>::const_iterator cit = kCatagorys.begin();
	for (; cit != kCatagorys.end(); ++cit)
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
ResultType ACT_SpringTreeManager::DrawReward(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t iSchedule, int32_t iKey, GameLogic::LC_RewardItemsExtend& rkExtend, const GameLogic::LC_IDVector* pkExtraParams, int& count)
{
	if (NULL == pkPlayer)
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
	if (pkRewardDataEntry->m_iScoreLimit > (int32_t)pkSaveSchedule->GetCount())
	{
		return RE_ACTIVITY_CONDITION_FAIL;
	}

	if (pkSaveSchedule->IsRewardReg(pkRewardDataEntry->m_iKey))
	{
		return RE_ACTIVITY_REWARDED;
	}

	ResultType rslt = pkRewardDataEntry->DoReward(pkPlayer, pkSaveSchedule, rkExtend);
	if (RE_SUCCESS == rslt)
	{
		pkSaveSchedule->RegReward(pkRewardDataEntry->m_iKey);
		pkSaveCatagory->UpdateDirtyFlag();
		pkSaveActivity->UpdateDirtyFlag();
		pkPlayer->GetACTScheduleAsset().UpdateDirtyFlag();
	}
	return rslt;
}
