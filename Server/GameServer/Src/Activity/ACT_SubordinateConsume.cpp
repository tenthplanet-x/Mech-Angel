#include "ACT_SubordinateConsume.h"
#include "ACT_ActivityDataManager.h"
#include "ACT_ActivitySchedule.h"
#include "LC_ServerPlayer.h"
#include "UT_ServerHelper.h"
#include "SC_ScriptAction.h"
#include "UT_Choice.h"

using namespace CSVFile;
using namespace GameLogic;
using namespace Activity;

static int32_t GetPersistentIndex(int32_t iCategory)
{
	return ACT_SCHEDULE_PERSISTENT_COUNT + 1000 * (iCategory - 1);
}

static int32_t _resolveCntByConsume(int32_t iVal)
{
	int32_t iCnt = 0;
	SA_RT_TRIGGER_ACTION(int32_t, iCnt, "ActSubordinateConusmeResolveCnt", iVal);
	return iCnt;
}

struct SubordinateConsumeScheduleOper : public Activity::ACT_ScheduleSaveDataOper<ACT_SubordinateConsumeManager::DataEntryType>
{
	SubordinateConsumeScheduleOper(ACT_ScheduleManager* schedule, LC_ServerPlayer* player, ACT_ScheduleAsset* asset, ACT_ScheduleSaveActivity* act, ACT_ScheduleSaveCatagory* cat)
		:Activity::ACT_ScheduleSaveDataOper<ACT_SubordinateConsumeManager::DataEntryType>(schedule, player, asset, act, cat)
		, iSubType(0)
		, iValue(0)
	{}
	
	virtual bool operator ()(ACT_SubordinateConsumeManager::ScheduleDataType* pkSchedule)
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
				ACT_ScheduleSaveCommon* pkSaveDetail = pkSaveEntry->_addScheduleSaveDetail(iSubType);
				if (NULL != pkSaveDetail)
				{
					int iDetailKey = GET_SCHEDULE_KEY(LC_ST_TP_FORERVER, ACT_SCHEDULE_DETAIL_COUNT);
					int iOldVal = pkSaveDetail->GetValue(iDetailKey);
					int iNewVal = iOldVal + iValue;
					pkSaveDetail->AddValue(iDetailKey, iValue);
					int iOldCnt = _resolveCntByConsume(iOldVal);
					int iNewCnt = _resolveCntByConsume(iNewVal);
					if (iNewCnt > iOldCnt)
					{
						pkSaveActivity->AddPersistentVal(GetPersistentIndex(pkSaveCatagory->ID()), iNewCnt - iOldCnt);
					}
					pkSaveEntry->UpdateDirtyFlag();
					pkSaveCatagory->UpdateDirtyFlag();
					pkSaveActivity->UpdateDirtyFlag();
					pkAsset->UpdateDirtyFlag();
				}
				return true;
			}
		}
		return false;
	}
	int32_t iSubType;
	int32_t iValue;
	StringType kExchangeCnt;
};

void ACT_SubordinateConsumeManager::OnSubordinateConsume(LC_ServerPlayer* pkPlayer, int32_t iSubType, int32_t iValue, const GameLogic::LC_IDSet* pkCatagorys)
{
	if (NULL == pkPlayer )
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
	SubordinateConsumeScheduleOper kOper(pkScheduleMgr, pkPlayer, &rkAsset, pkSaveActivity, NULL);
	kOper.iSubType = iSubType;
	kOper.iValue = iValue;
	CatagoryDataType* pkCatagory = GetCatagoryData(E_SUBORDINATE_CONSUME);
	if (NULL != pkCatagory)
	{
		kOper.pkSaveCatagory = pkSaveActivity->_addScheduleSaveCatagory(E_SUBORDINATE_CONSUME);
		if (NULL != kOper.pkSaveCatagory)
		{
			pkCatagory->ForAll(&kOper);
		}
	}

}

static int32_t _resolveCntByRecharge(int32_t iVal)
{
	return iVal / 1000;
}

struct SubordinateRechargeScheduleOper : public Activity::ACT_ScheduleSaveDataOper<ACT_SubordinateConsumeManager::DataEntryType>
{
	SubordinateRechargeScheduleOper(ACT_ScheduleManager* schedule, LC_ServerPlayer* player, ACT_ScheduleAsset* asset, ACT_ScheduleSaveActivity* act, ACT_ScheduleSaveCatagory* cat)
	:Activity::ACT_ScheduleSaveDataOper<ACT_SubordinateConsumeManager::DataEntryType>(schedule, player, asset, act, cat)
	, iValue(0)
	{}

	virtual bool operator ()(ACT_SubordinateConsumeManager::ScheduleDataType* pkSchedule)
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

				int iOldVal = pkSaveEntry->GetCount();
				int iNewVal = iOldVal + iValue;
				pkSaveEntry->AddCount(iValue);
				int iOldCnt = _resolveCntByRecharge(iOldVal);
				int iNewCnt = _resolveCntByRecharge(iNewVal);
				if (iNewCnt > iOldCnt)
				{
					pkSaveActivity->AddPersistentVal(GetPersistentIndex(pkSaveCatagory->ID()), iNewCnt - iOldCnt);
				}
				pkSaveEntry->UpdateDirtyFlag();
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

void ACT_SubordinateConsumeManager::OnCharge(LC_ServerPlayer* pkPlayer, int32_t val, const GameLogic::LC_IDSet* pkCatagorys)
{
	if (NULL == pkPlayer)
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
	if (NULL == pkSaveActivity)
	{
		return;
	}
	SubordinateRechargeScheduleOper kOper(pkScheduleMgr, pkPlayer, &rkAsset, pkSaveActivity, NULL);
	kOper.iValue = val;
	CatagoryDataType* pkCatagory = GetCatagoryData(E_SUBORDINATE_RECHARGE);
	if (NULL != pkCatagory)
	{
		kOper.pkSaveCatagory = pkSaveActivity->_addScheduleSaveCatagory(E_SUBORDINATE_RECHARGE);
		if (NULL != kOper.pkSaveCatagory)
		{
			pkCatagory->ForAll(&kOper);
		}
	}

}

void ACT_SubordinateConsumeManager::AddDrawCount(LC_ServerPlayer* pkPlayer, int32_t iValue, const mem::vector<int32_t>& kCatagorys)
{
	if (NULL == pkPlayer )
	{
		return;
	}
	ACT_ScheduleManager* pkScheduleMgr = ACT_ScheduleManager::GetSingletonPtr();
	if (NULL == pkScheduleMgr)
	{
		return;
	}
	Activity::ACT_ScheduleAsset& kAsset = pkPlayer->GetACTScheduleAsset();
	Activity::ACT_ScheduleSaveActivity*	pkSaveActivity = kAsset._addScheduleSaveActivity(GetType());
	if (NULL==pkSaveActivity)
	{
		return;
	}
	mem::vector<int32_t>::const_iterator iter = kCatagorys.begin();
	for (; iter != kCatagorys.end(); ++iter)
	{
		pkSaveActivity->AddPersistentVal(GetPersistentIndex(*iter), iValue);
	}
	pkSaveActivity->UpdateDirtyFlag();
	kAsset.UpdateDirtyFlag();
}

ResultType ACT_SubordinateConsumeManager::DoBehaveSpec(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t iSchedule, mem::vector<int32_t>& kReqpVals, mem::vector<int32_t>& kRespVals)
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

	CatagoryDataType* pCategoryData = GetCatagoryData(iCatagory);
	if (NULL == pCategoryData)
	{
		return RE_FAIL;
	}

	ScheduleDataType* pkScheduleData = pCategoryData->GetScheduleData(iSchedule);
	if (NULL == pkScheduleData)
	{
		return RE_FAIL;
	}

	const ScheduleDataType::DataEntryMap& kDataEntryMap = pkScheduleData->GetDataEntryMap();
	if (kDataEntryMap.empty())
	{
		return RE_FAIL;
	}
	const DataEntryType* pkDataEntry = &(kDataEntryMap.rbegin()->second);

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

	int32_t iFreeCountLimit = 0;
	if (pkDataEntry->m_kVipProfit.size() >= 2)
	{
		LC_VIPCmnManager* pkVIPMgr = LC_VIPCmnManager::GetSingletonPtr();
		if (NULL == pkVIPMgr)
		{
			return RE_FAIL;
		}
		LC_VIPProfitDataEntryPtrs& kProfitEntryPtrs = pkVIPMgr->GetVIPProfitDataEntryPtrs(pkDataEntry->m_kVipProfit[0], pkDataEntry->m_kVipProfit[1]);
		if (kProfitEntryPtrs.empty())
		{
			return RE_FAIL;
		}
		LC_VIPProfitDataEntry* pkVIPProfitEntry = *(kProfitEntryPtrs.begin());
		if (NULL == pkVIPProfitEntry)
		{
			return RE_FAIL;
		}
		LC_VIPAsset& kVipAsset = pkPlayer->GetVIPAsset();
		iFreeCountLimit = pkVIPProfitEntry->GetValue(pkPlayer->GetVIPLevel(), pkPlayer->GetVIPAsset().IsFlagSet(LC_VIP_FG_FAKE));
	}
	int32_t iCurFreeCount = pkSaveSchedule->GetValue(ACT_SCHEDULE_FREE_COUNT, LC_ST_TP_DAILY);	
	bool bFree = (iCurFreeCount < iFreeCountLimit);
	if (!bFree)
	{
		if (pkSaveActivity->GetPersistentVal(GetPersistentIndex(iCatagory)) <= 0)
		{
			return RE_FAIL;
		}
	}
	
	int iCurAchieve = pkSaveSchedule->GetAchieve();

	if (pkDataEntry->m_kForwardRate.empty())
	{
		return RE_FAIL;
	}
	int iAddRate = pkSaveSchedule->GetValue(ACT_SCHEDULE_RATE);
	int iRateSize = (int)pkDataEntry->m_kForwardRate.size();
	Utility::Choice<int> kChoice(iRateSize);
	for (int i = 0; i < iRateSize; ++i)
	{
		if (i == iRateSize - 1)
		{
			kChoice.Add(pkDataEntry->m_kForwardRate[i] + iAddRate, i + 1);
		}
		else
		{
			kChoice.Add(pkDataEntry->m_kForwardRate[i], i + 1);
		}		
	}
	Utility::UT_SIMDataList kItems;

	int iMoveStep = 0;
	int* pkRandVal = kChoice.make();
	if (NULL == pkRandVal)
	{
		return RE_FAIL;
	}
	iMoveStep = *pkRandVal;
	if (iMoveStep == iRateSize)
	{
		pkSaveSchedule->SetValue(0, ACT_SCHEDULE_RATE);
	}
	else
	{
		pkSaveSchedule->AddValue(pkDataEntry->m_iAddRate, ACT_SCHEDULE_RATE);
	}
	kRespVals.push_back(iMoveStep);
	while (iMoveStep > 0)
	{
		++iCurAchieve;
		int iCurFloor = (iCurAchieve - 1) / pkDataEntry->m_iFloorStep % pkDataEntry->m_iFloor + 1;
		int iCurStep = (iCurAchieve - 1) % pkDataEntry->m_iFloorStep + 1;
		DataEntryType* pkEntryAward = pCategoryData->GetDataEntry(iSchedule, iCurStep + (iCurFloor-1)*pkDataEntry->m_iFloorStep);
		if (NULL != pkEntryAward)
		{
			kItems.insert(kItems.end(), pkEntryAward->m_kReward.begin(), pkEntryAward->m_kReward.end());
		}
		--iMoveStep;		
	}
	kRespVals.push_back(iMoveStep);

	if (!kItems.empty())
	{
		UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, kItems, pkPlayer, SYS_FUNC_TP_ACTIVITY_SUBORDINATE_CONSUME
			, iCurAchieve, pkDataEntry->m_kMailTitle, pkDataEntry->m_kMailDesc, true);
	}

	pkSaveSchedule->SetAchieve(iCurAchieve);
	if (bFree)
	{
		pkSaveSchedule->AddValue(1,ACT_SCHEDULE_FREE_COUNT, LC_ST_TP_DAILY);
	}
	else
	{
		pkSaveActivity->AddPersistentVal(GetPersistentIndex(iCatagory), -1);
	}
	if (pkDataEntry->m_iActScore > 0)
	{
		pkSaveSchedule->AddActShopScore(pkDataEntry->m_iActScore);
	}
	kScheduleAsset.UpdateDirtyFlag();
	pkSaveActivity->UpdateDirtyFlag();
	pkSaveCatagory->UpdateDirtyFlag();
	return RE_SUCCESS;
}
