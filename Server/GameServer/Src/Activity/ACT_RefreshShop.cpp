#include "ACT_RefreshShop.h"
#include "ACT_ActivityDataManager.h"
#include "ACT_ActivitySchedule.h"
#include "LC_ServerPlayer.h"
#include "UT_ServerHelper.h"
#include "GameServerApp.h"
#include "UT_Choice.h"

using namespace CSVFile;
using namespace GameLogic;
using namespace Activity;

ResultType ACT_RefreshShopDataEntry::DoReward(LC_ServerPlayer* pkPlayer, ACT_ScheduleSaveEntry* pkEntry, GameLogic::LC_RewardItemsExtend& rkExtend)
{
	if (NULL==pkPlayer || NULL==pkEntry)
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
	}while(0);

	return uiRslt;
}


ResultType ACT_RefreshShopManager::_refreshShop(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t iSchedule, mem::vector<int32_t>& kItemVec)
{
	if (NULL == pkPlayer)
	{
		return RE_FAIL;
	}

	CatagoryDataType* pkCatagory = GetCatagoryData(iCatagory);
	if (NULL == pkCatagory)
	{
		return RE_FAIL;
	}
	ScheduleDataType* pkSchedule = pkCatagory->GetScheduleData(iSchedule);
	if (NULL == pkSchedule)
	{
		return RE_FAIL;
	}
	mem::map<int, Utility::Choice<int> > kChoiceMap;
	mem::map<int, int > kChoiceCountMap;
	const ScheduleDataType::DataEntryMap& kDatas = pkSchedule->GetDataEntryMap();
	ScheduleDataType::DataEntryMapConstIter iter = kDatas.begin();
	for (; iter != kDatas.end(); ++iter)
	{
		Utility::Choice<int>& kChoice = kChoiceMap[iter->second.m_iGrade];
		kChoice.Add(iter->second.m_iRefreshRate, iter->second.m_iKey);
		kChoiceCountMap[iter->second.m_iGrade] = iter->second.m_iRefreshCount;
	}
	mem::map<int, Utility::Choice<int> >::iterator kChoiceIter = kChoiceMap.begin();
	for (; kChoiceIter != kChoiceMap.end(); ++kChoiceIter)
	{
		mem::vector<int*> kChoiceRslt;
		kChoiceIter->second.makeMulti(kChoiceCountMap[kChoiceIter->first], kChoiceRslt);
		for (size_t index = 0; index < kChoiceRslt.size(); ++index)
		{
			kItemVec.push_back(*kChoiceRslt[index]);
		}
	}
	return RE_SUCCESS;
}
ResultType ACT_RefreshShopManager::DoQuery(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t iSchedule, mem::vector<int32_t>& kReqpVals, mem::vector<int32_t>& kRespVals)
{
	if (NULL == pkPlayer)
	{
		return RE_FAIL;
	}
	CatagoryDataType* pkCatagory = GetCatagoryData(iCatagory);
	if (NULL == pkCatagory)
	{
		return RE_FAIL;
	}
	ScheduleDataType* pkSchedule = pkCatagory->GetScheduleData(iSchedule);
	if (NULL == pkSchedule)
	{
		return RE_FAIL;
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
	int iCount = pkSaveSchedule->GetValue(ACT_SCHEDULE_REFRESH_SHOP_KEY_BEGIN, GameLogic::LC_ST_TP_DAILY);
	kRespVals.clear();
	if (iCount <= 0)
	{	
		_refreshShop(pkPlayer, iCatagory, iSchedule, kRespVals);
		if (kRespVals.empty())
		{
			return RE_FAIL;
		}
		if ((int)kRespVals.size() > ACT_SCHEDULE_REFRESH_SHOP_KEY_END - ACT_SCHEDULE_REFRESH_SHOP_KEY_BEGIN)
		{
			kRespVals.clear();
			return RE_FAIL;
		}
		pkSaveSchedule->_clearDetails();
		pkSaveSchedule->SetValue((int)kRespVals.size(), ACT_SCHEDULE_REFRESH_SHOP_KEY_BEGIN, GameLogic::LC_ST_TP_DAILY);
		for (int i = 0; i < (int)kRespVals.size(); ++i)
		{
			pkSaveSchedule->SetValue(kRespVals[i], ACT_ScheduleDataFlag(ACT_SCHEDULE_REFRESH_SHOP_KEY_BEGIN + i + 1), GameLogic::LC_ST_TP_DAILY);
		}
	}
	else
	{
		for (int i = 0; i < iCount; ++i)
		{
			kRespVals.reserve(iCount);
			kRespVals.push_back(pkSaveSchedule->GetValue(ACT_ScheduleDataFlag(ACT_SCHEDULE_REFRESH_SHOP_KEY_BEGIN + i + 1), GameLogic::LC_ST_TP_DAILY));
		}
	}
	return RE_SUCCESS;
}
ResultType ACT_RefreshShopManager::DrawReward(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t iSchedule, int32_t iKey, GameLogic::LC_RewardItemsExtend& rkExtend, const GameLogic::LC_IDVector* pkExtraParams, int& count)
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
	ACT_ScheduleSaveCommon*	pkSaveDetail = pkSaveSchedule->_addScheduleSaveDetail(iKey);
	if (NULL == pkSaveDetail)
	{
		return RE_FAIL;
	}
	int iCount = pkSaveSchedule->GetValue(ACT_SCHEDULE_REFRESH_SHOP_KEY_BEGIN, GameLogic::LC_ST_TP_DAILY);
	if (iCount <= 0)
	{
		return RE_FAIL;
	}
	bool bFind = false;
	for (int i = 0; i < iCount; ++i)
	{
		if (pkSaveSchedule->GetValue(ACT_ScheduleDataFlag(ACT_SCHEDULE_REFRESH_SHOP_KEY_BEGIN + i + 1), GameLogic::LC_ST_TP_DAILY) == iKey)
		{
			bFind = true;
			break;
		}
	}
	if (!bFind)
	{
		return RE_FAIL;
	}
	int32_t iRewardKey = GET_SCHEDULE_KEY(LC_ST_TP_DAILY, ACT_SCHEDULE_REWARD_COUNT);
	if ((int)pkSaveDetail->GetValue(iRewardKey) >= pkRewardDataEntry->m_iLimitBuy)
	{
		return RE_ACTIVITY_TIMES_OUT;
	}
	if (!pkPlayer->IsCashEnough(pkRewardDataEntry->m_iPrice, CT_UNBIND_YUANBAO))
	{
		return RE_SHOPCITY_CASH_NOT_ENOUGH;
	}	
	ResultType rslt = pkRewardDataEntry->DoReward(pkPlayer, pkSaveSchedule, rkExtend);
	if (RE_SUCCESS == rslt)
	{
		pkSaveDetail->AddValue(iRewardKey, 1);
		pkSaveSchedule->UpdateDirtyFlag();
		pkSaveCatagory->UpdateDirtyFlag();
		pkSaveActivity->UpdateDirtyFlag();
		pkPlayer->GetACTScheduleAsset().UpdateDirtyFlag();
	}
	pkPlayer->ReduceCash(pkRewardDataEntry->m_iPrice, CT_UNBIND_YUANBAO, SYS_FUNC_TP_ACTIVITY_REFRESH_SHOP_BUY, iKey);
	return rslt;
}

ResultType ACT_RefreshShopManager::DoBehaveSpec(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t iSchedule, mem::vector<int32_t>& kReqpVals, mem::vector<int32_t>& kRespVals)
{
	if (NULL == pkPlayer)
	{
		return RE_FAIL;
	}
	CatagoryDataType* pkCatagory = GetCatagoryData(iCatagory);
	if (NULL == pkCatagory)
	{
		return RE_FAIL;
	}
	ScheduleDataType* pkSchedule = pkCatagory->GetScheduleData(iSchedule);
	if (NULL == pkSchedule)
	{
		return RE_FAIL;
	}

	static ACT_ScheduleManager* pkScheduleMgr = ACT_ScheduleManager::GetSingletonPtr();
	if (!pkScheduleMgr->IsScheduleValid(pkPlayer->GetServerID(), iSchedule))
	{
		return RE_ACTIVITY_NOT_VALID;
	}

	const ScheduleDataType::DataEntryMap& kDataEntryMap = pkSchedule->GetDataEntryMap();
	if (kDataEntryMap.empty())
	{
		return RE_FAIL;
	}
	const DataEntryType* pkEntry = &(kDataEntryMap.begin()->second);
	
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
	int iRefresh = pkSaveSchedule->GetCount(GameLogic::LC_ST_TP_DAILY);
	if (iRefresh >= pkEntry->m_iLimitRefresh)
	{
		return RE_ACTIVITY_TIMES_OUT;
	}
	if (pkEntry->m_iRefreshCost.empty())
	{
		return RE_FAIL;
	}
	int iCost = 0;
	if (pkEntry->m_iRefreshCost.size() < iRefresh + 1)
	{
		iCost = *(pkEntry->m_iRefreshCost.rbegin());
	}
	else
	{
		iCost = pkEntry->m_iRefreshCost[iRefresh];
	}
	if (iCost <= 0)
	{
		return RE_FAIL;
	}
	if (!pkPlayer->IsCashEnough(iCost, CT_UNBIND_YUANBAO))
	{
		return RE_SHOPCITY_CASH_NOT_ENOUGH;
	}
	kRespVals.clear();
	_refreshShop(pkPlayer, iCatagory, iSchedule, kRespVals);
	if (kRespVals.empty())
	{
		return RE_FAIL;
	}
	if ((int)kRespVals.size() > ACT_SCHEDULE_REFRESH_SHOP_KEY_END - ACT_SCHEDULE_REFRESH_SHOP_KEY_BEGIN)
	{
		kRespVals.clear();
		return RE_FAIL;
	}

	pkPlayer->ReduceCash(iCost, CT_UNBIND_YUANBAO, SYS_FUNC_TP_ACTIVITY_REFRESH_SHOP, 0);
	pkSaveSchedule->_clearDetails();
	pkSaveSchedule->SetValue((int)kRespVals.size(), ACT_SCHEDULE_REFRESH_SHOP_KEY_BEGIN, GameLogic::LC_ST_TP_DAILY);
	for (int i = 0; i < (int)kRespVals.size(); ++i)
	{
		pkSaveSchedule->SetValue(kRespVals[i], ACT_ScheduleDataFlag(ACT_SCHEDULE_REFRESH_SHOP_KEY_BEGIN + i + 1), GameLogic::LC_ST_TP_DAILY);
	}
	pkSaveSchedule->AddCount(1, GameLogic::LC_ST_TP_DAILY);
	pkSaveSchedule->UpdateDirtyFlag();
	pkSaveCatagory->UpdateDirtyFlag();
	pkSaveActivity->UpdateDirtyFlag();
	pkPlayer->GetACTScheduleAsset().UpdateDirtyFlag();
	return RE_SUCCESS;
}
