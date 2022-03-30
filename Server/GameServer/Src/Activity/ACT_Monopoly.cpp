#include "ACT_Monopoly.h"
#include "ACT_ActivityDataManager.h"
#include "ACT_ActivitySchedule.h"
#include "LC_ServerPlayer.h"
#include "UT_ServerHelper.h"
#include "SC_ScriptAction.h"

using namespace CSVFile;
using namespace GameLogic;
using namespace Activity;

struct MonopolyScheduleOper : public Activity::ACT_ScheduleSaveDataOper<ACT_MonopolyManager::DataEntryType>
{
	MonopolyScheduleOper(ACT_ScheduleManager* schedule, LC_ServerPlayer* player, ACT_ScheduleAsset* asset, ACT_ScheduleSaveActivity* act, ACT_ScheduleSaveCatagory* cat)
		:Activity::ACT_ScheduleSaveDataOper<ACT_MonopolyManager::DataEntryType>(schedule, player, asset, act, cat)
		,iAddCount(0)
	{}
	
	virtual bool operator ()(ACT_MonopolyManager::ScheduleDataType* pkSchedule)
	{
		if (pkScheduleMgr->IsScheduleValid(pkPlayer->GetServerID(), pkSchedule->m_iID))
		{
			ACT_ScheduleSaveEntry* pkSaveEntry = pkSaveCatagory->_addScheduleSaveEntry(pkSchedule->m_iID);
			if (NULL != pkSaveEntry)
			{
				pkSaveEntry->AddCount(iAddCount);	
				pkSaveCatagory->UpdateDirtyFlag();
				pkSaveActivity->UpdateDirtyFlag();
				pkAsset->UpdateDirtyFlag();
				return true;
			}
		}
		return false;
	}
	int32_t iAddCount;
};

void ACT_MonopolyManager::OnTaskFinished(LC_ServerPlayer* pkPlayer, GameLogic::LC_Task* pkTask, const GameLogic::LC_IDSet* pkCatagorys)
{
	if (NULL == pkPlayer || NULL == pkTask)
	{
		return;
	}
	if (pkTask->GetTaskType() != STT_MONOPOLY_TASK)
	{
		return;
	}
	ACT_ScheduleManager* pkScheduleMgr = ACT_ScheduleManager::GetSingletonPtr();
	if (NULL == pkScheduleMgr)
	{
		return;
	}
	Activity::ACT_ScheduleAsset& rkAsset = pkPlayer->GetACTScheduleAsset();
	Activity::ACT_ScheduleSaveActivity*	pkSaveActivity = rkAsset._addScheduleSaveActivity(GetType());
	if (NULL==pkSaveActivity)
	{
		return;
	}
	MonopolyScheduleOper kOper(pkScheduleMgr, pkPlayer, &rkAsset, pkSaveActivity, NULL);
	kOper.iAddCount = 1;
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

ResultType ACT_MonopolyManager::MonopolyForward(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t iSchedule, bool bUseYuanbao, int32_t& iRetMoveStep)
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
	if (bUseYuanbao)
	{
		if (!pkPlayer->IsCashEnough(E_MONOPOLY_YUANBAO_COST, CT_UNBIND_YUANBAO))
		{ 
			return RE_FAIL;
		}
	}
	if (pkSaveSchedule->GetCount() <= 0)
	{
		return RE_FAIL;
	}

	int iCurAchieve = pkSaveSchedule->GetAchieve();
	int iCurFloor = iCurAchieve/E_MONOPOLY_MAX_FLOOR_STEP%E_MONOPOLY_MAX_FLOOR + 1;
	int iCurStep = iCurAchieve%E_MONOPOLY_MAX_FLOOR_STEP;
	
	CatagoryDataType* pCategoryData = GetCatagoryData(iCatagory);
	if (NULL == pCategoryData)
	{
		return RE_FAIL;
	}

	int iMoveStep = 0;
	if (bUseYuanbao)
	{
		iMoveStep = E_MONOPOLY_POINT_MAX;
		pkPlayer->ReduceCash(E_MONOPOLY_YUANBAO_COST, CT_UNBIND_YUANBAO, SYS_FUNC_TP_ACTIVITY_MONOPOLY, 0);
	}
	else
	{
		SA_RT_TRIGGER_ACTION(int32_t, iMoveStep, "ActMonopolyForwardStep",0);		
	}
	iRetMoveStep = iMoveStep;
	Utility::UT_SIMDataList kItems;
	StringType kMailTitle;
	StringType kMailDesc;
	MoneyType lAwardPoint = 0;
	while(iMoveStep > 0)
	{
		++iCurAchieve;
		++iCurStep;		
		DataEntryType* pkEntry = pCategoryData->GetDataEntry(iSchedule, iCurStep + (iCurFloor - 1)*E_MONOPOLY_MAX_FLOOR_STEP );
		if (NULL != pkEntry)
		{
			kItems.insert(kItems.end(), pkEntry->m_kReward.begin(),  pkEntry->m_kReward.end());
			if (kMailTitle.empty())
			{
				kMailTitle = pkEntry->m_kMailTitle;
			}
			if (kMailDesc.empty())
			{
				kMailDesc = pkEntry->m_kMailDesc;
			}
		}
		int32_t iPoint = 0;
		SA_RT_TRIGGER_ACTION(int32_t, iPoint, "ActMonopolyPointAward", iCurFloor, int32_t(iCurStep >= E_MONOPOLY_MAX_FLOOR_STEP), int32_t(NULL != pkEntry));		
		lAwardPoint += iPoint;
		if (iCurStep >= E_MONOPOLY_MAX_FLOOR_STEP)
		{
			break;
		}
		--iMoveStep;
	}
	pkSaveSchedule->SetAchieve(iCurAchieve);
	pkSaveSchedule->AddCount(-1);
	pkPlayer->AddCash(lAwardPoint,CT_MONOPOLY_POINT,SYS_FUNC_TP_ACTIVITY_MONOPOLY,iCurAchieve);
	if (!kItems.empty())
	{
		 UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, kItems, pkPlayer, SYS_FUNC_TP_ACTIVITY_MONOPOLY
			, iCurAchieve, kMailTitle, kMailDesc, true);
	}
	kScheduleAsset.UpdateDirtyFlag();
	pkSaveActivity->UpdateDirtyFlag();
	pkSaveCatagory->UpdateDirtyFlag();
	return RE_SUCCESS;
}

void ACT_MonopolyManager::AddMonopolyCount(LC_ServerPlayer* pkPlayer,int32_t iAddCount)
{
	ACT_ScheduleManager* pkScheduleMgr = ACT_ScheduleManager::GetSingletonPtr();
	if (NULL == pkScheduleMgr)
	{
		return;
	}
	Activity::ACT_ScheduleAsset& rkAsset = pkPlayer->GetACTScheduleAsset();
	Activity::ACT_ScheduleSaveActivity*	pkSaveActivity = rkAsset._addScheduleSaveActivity(GetType());
	if (NULL==pkSaveActivity)
	{
		return;
	}
	MonopolyScheduleOper kOper(pkScheduleMgr, pkPlayer, &rkAsset, pkSaveActivity, NULL);
	kOper.iAddCount = iAddCount;
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