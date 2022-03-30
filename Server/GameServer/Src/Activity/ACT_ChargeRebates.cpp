#include "ACT_ChargeRebates.h"
#include "ACT_ActivitySchedule.h"

#include "LC_ServerPlayer.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerLogicManager.h"

#include "UT_ServerHelper.h"
#include "CF_ACTChargeProfitType.h"
using namespace CSVFile;
using namespace GameLogic;
using namespace Activity;

ResultType ACT_ChargeRebatesDataEntry::DoReward(LC_ServerPlayer* pkPlayer, GameLogic::LC_RewardItemsExtend& rkExtend)
{
	if (NULL==pkPlayer)
	{
		return RE_FAIL;
	}

	ResultType uiRslt = RE_SUCCESS;
	do
	{
		Utility::UT_SIMDataList kItems;
		GameLogic::LC_ItemsReward RewardItems;
		RewardItems.SetUp(m_kRewardItems);
		rkExtend.SetChoice(RewardItems.CheckChoiceID(pkPlayer->GetWeaponAttribute(), rkExtend.Choice()));
		RewardItems.Resolve(pkPlayer, kItems, &rkExtend);
		if (!kItems.empty())
		{
			uiRslt = UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, kItems, pkPlayer, SYS_FUNC_TP_ACTIVITY_CHARGE_REBATES
				, m_iID, m_sMailTitle, m_sMailDesc, m_bMailEnable);
			if (RE_SUCCESS != uiRslt)
			{
				break;
			}
		}
	}while(0);

	return uiRslt;
}

struct ChargeRebatesScheduleOper : public Activity::ACT_ScheduleSaveDataOper<ACT_ChargeRebatesManager::DataEntryType>
{
	ChargeRebatesScheduleOper(ACT_ScheduleManager* schedule, LC_ServerPlayer* player, ACT_ScheduleAsset* asset, ACT_ScheduleSaveActivity* act, ACT_ScheduleSaveCatagory* cat)
		:Activity::ACT_ScheduleSaveDataOper<ACT_ChargeRebatesManager::DataEntryType>(schedule, player, asset, act, cat)
		,iValue(0)
	{}
	virtual bool operator ()(ACT_ChargeRebatesManager::ScheduleDataType* pkSchedule)
	{
		if (pkScheduleMgr->IsScheduleValid(pkPlayer->GetServerID(), pkSchedule->m_iID))
		{
			ACT_ScheduleSaveEntry* pkSaveEntry = pkSaveCatagory->_addScheduleSaveEntry(pkSchedule->m_iID);
			if (pkSaveEntry)
			{
				static ACT_ScheduleManager* pkScheduleMgr = ACT_ScheduleManager::GetSingletonPtr();
				ACT_ScheduleEntry* pkScheduleEntry = pkScheduleMgr->GetSchedule(pkPlayer->GetServerID(), pkSchedule->m_iID);
				if (NULL == pkScheduleEntry)
				{
					return RE_FAIL;
				}
				int32_t inRecodeDayKey = GET_SCHEDULE_KEY(iType, 0); //活动期间充值的最后一天
				int32_t iDay = LC_Helper::GetDeltaDay(pkScheduleEntry->CurrentStartTime()/ONE_SECOND_MILI, GET_CURRENT_TIMESTAMP_IN_SECONDS()) + 1;
				ACT_ScheduleSaveCommon* pkScheduleSaveCommon = pkSaveEntry->_addScheduleSaveDetail(iType);
				if(pkScheduleSaveCommon)
				{
					int nRecodeDay = pkScheduleSaveCommon->GetValue(inRecodeDayKey);
					if(nRecodeDay != iDay)
					{
						pkSaveEntry->SetCount(0);
						pkScheduleSaveCommon->SetValue(inRecodeDayKey, iDay);
					}
					pkSaveEntry->AddCount(iValue);
					int32_t nCount = pkSaveEntry->GetCount();
					std::map<uint32_t, uint32_t >::iterator iter = pMgr->m_ChargeRebatesMap.begin();
					for(; iter != pMgr->m_ChargeRebatesMap.end(); iter++)
					{
						if(nCount < iter->first)
						{
							continue;
						}
						int nDayKey = GET_SCHEDULE_KEY(iType, iter->first);

						int nRecodeDay = pkScheduleSaveCommon->GetValue(nDayKey);
						if(nRecodeDay != iDay)
						{
							int nFlag = pkScheduleSaveCommon->GetValue(iter->second);
							pkScheduleSaveCommon->SetValue(iter->second, ++nFlag);	
						}
						pkScheduleSaveCommon->SetValue(nDayKey, iDay);

					}
					pkSaveCatagory->UpdateDirtyFlag();
					pkSaveActivity->UpdateDirtyFlag();
					pkAsset->UpdateDirtyFlag();	
				}

			}
		}
		return false;
	}
	ACT_ChargeRebatesManager* pMgr;
	int32_t iValue;
	int32_t iType;
};

void ACT_ChargeRebatesManager::AddCharge(LC_ServerPlayer* pkPlayer, int32_t val, const mem::vector<int32_t>& kCatagorys)
{
}
void ACT_ChargeRebatesManager::NewOnCharge(LC_ServerPlayer* pkPlayer,int32_t nValueType, int32_t val, const GameLogic::LC_IDSet* pkCatagorys)
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

	ChargeRebatesScheduleOper kOper(pkScheduleMgr, pkPlayer, &rkAsset, pkSaveActivity, NULL);
	kOper.iValue = val;
	kOper.pMgr = this;//

	if (NULL==pkCatagorys)
	{
		CatagoryDataMap::iterator it = CatagoryDatas().begin();
		for (; it!=CatagoryDatas().end(); ++it)
		{
			CF_ACTChargeProfitType::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_ACTChargeProfitType>(it->first);
			if(NULL == pkData)
			{
				continue;
			}
			if(pkData->_Moneytype != nValueType)
			{
				continue;
			}
			kOper.iType = it->first;
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
			CF_ACTChargeProfitType::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_ACTChargeProfitType>(*cit);
			if(NULL == pkData)
			{
				continue;
			}
			if(pkData->_Moneytype != nValueType)
			{
				continue;
			}
			CatagoryDataType* pkCatagory = GetCatagoryData(*cit);
			if (NULL == pkCatagory)
			{
				continue;
			}
			kOper.iType = *cit;
			kOper.pkSaveCatagory = pkSaveActivity->_addScheduleSaveCatagory(*cit);
			if (kOper.pkSaveCatagory)
			{
				pkCatagory->ForAll(&kOper);
			}
		}
	}
}

ResultType ACT_ChargeRebatesManager::DrawReward(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t iSchedule, int32_t iKey, GameLogic::LC_RewardItemsExtend& rkExtend, const GameLogic::LC_IDVector* pkExtraParams, int& count)
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
	ACT_ScheduleEntry* pkScheduleEntry = pkScheduleMgr->GetSchedule(pkPlayer->GetServerID(), pkDataEntry->m_iShowSchdeule);
	if (NULL == pkScheduleEntry)
	{
		return RE_FAIL;
	}
	int32_t iDay = LC_Helper::GetDeltaDay(pkScheduleEntry->CurrentStartTime()/ONE_SECOND_MILI, GET_CURRENT_TIMESTAMP_IN_SECONDS())+1;
	/*if (iDay < pkDataEntry->m_iRebateType)
	{
		return RE_ACTIVITY_NOT_VALID;
	}*/

	ResultType rslt = CheckScheduleData(pkPlayer, pkDataEntry, iCatagory, iSchedule ,iKey);
	if(rslt != RE_SUCCESS)
	{
		return rslt;
	}
	
	rslt = pkDataEntry->DoReward(pkPlayer, rkExtend);
	if (RE_SUCCESS == rslt)
	{
		EndDoReward(pkPlayer, pkDataEntry, iCatagory, iSchedule ,iKey);
	}
	return rslt;
}

ResultType ACT_ChargeRebatesManager::EndDoReward(LC_ServerPlayer* pkPlayer, DataEntryType* pkDataEntry, int32_t iCatagory, int32_t iSchedule, int32_t iKey)
{
	if(pkPlayer == NULL || pkDataEntry == NULL)
	{
		return RE_FAIL;
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
	pkSaveSchedule->RegReward(pkDataEntry->m_kSaveIndex);

	ACT_ScheduleSaveCommon* pkSaveDetail = pkSaveSchedule->_addScheduleSaveDetail(pkDataEntry->m_iKey);
	if (NULL == pkSaveDetail)
	{
		return RE_FAIL;
	}
	int32_t iRewardKey = GET_SCHEDULE_KEY(LC_ST_TP_FORERVER, ACT_SCHEDULE_REWARD_COUNT);
	
	ACT_ScheduleSaveCommon* pkSaveScheduleCommon = pkSaveSchedule->GetScheduleSaveDetail(iCatagory);
	if (NULL == pkSaveScheduleCommon)
	{
		return RE_FAIL;
	}

	pkSaveDetail->AddValue(iRewardKey,1);
	
	pkSaveCatagory->UpdateDirtyFlag();
	pkSaveActivity->UpdateDirtyFlag();
	pkPlayer->GetACTScheduleAsset().UpdateDirtyFlag();
	return RE_SUCCESS;
}

ResultType ACT_ChargeRebatesManager::CheckScheduleData(LC_ServerPlayer* pkPlayer, DataEntryType* pkDataEntry, int32_t iCatagory, int32_t iSchedule, int32_t iKey)
{
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
	ACT_ScheduleSaveCommon* pkSaveDetail = pkSaveSchedule->_addScheduleSaveDetail(pkDataEntry->m_iKey);
	if (NULL == pkSaveDetail)
	{
		return RE_FAIL;
	}

	int32_t iRewardKey = GET_SCHEDULE_KEY(LC_ST_TP_FORERVER, ACT_SCHEDULE_REWARD_COUNT);

	if (pkSaveSchedule->IsRewardReg(pkDataEntry->m_kSaveIndex))
	{
		return RE_ACTIVITY_REWARDED;
	}

	//int32_t iTotalDayKey = GET_SCHEDULE_KEY(pkDataEntry->m_iType, 0);//累冲天数
	//int32_t iRecodeDayKey = GET_SCHEDULE_KEY(pkDataEntry->m_iType, 1); //活动期间充值的最后一天
	ACT_ScheduleSaveCommon* pkSaveScheduleCommon = pkSaveSchedule->GetScheduleSaveDetail(iCatagory);
	if (NULL == pkSaveScheduleCommon)
	{
		return RE_FAIL;
	}

	//充值累积天数不足
	if(pkSaveScheduleCommon->GetValue(pkDataEntry->m_iRebateID) < pkDataEntry->m_iRebateType)
	{
		return RE_ACTIVITY_CONDITION_FAIL;
	}
	return RE_SUCCESS;
}

struct ChargeRebatesDailyOper : public Activity::ACT_ScheduleSaveDataOper<ACT_ChargeRebatesManager::DataEntryType>
{
	ChargeRebatesDailyOper(ACT_ScheduleManager* schedule, LC_ServerPlayer* player, ACT_ScheduleAsset* asset, ACT_ScheduleSaveActivity* act, ACT_ScheduleSaveCatagory* cat)
		:Activity::ACT_ScheduleSaveDataOper<ACT_ChargeRebatesManager::DataEntryType>(schedule, player, asset, act, cat)
	{}

	virtual bool operator ()(ACT_ChargeRebatesManager::ScheduleDataType* pkSchedule)
	{
		
		if (pkScheduleMgr->IsScheduleValid(pkPlayer->GetServerID(), pkSchedule->m_iID))
		{
			ACT_ScheduleSaveEntry* pkSaveEntry = pkSaveCatagory->_addScheduleSaveEntry(pkSchedule->m_iID);
			if (pkSaveEntry)
			{
				ACT_ScheduleEntry* pkScheduleEntry = pkScheduleMgr->GetSchedule(pkPlayer->GetServerID(), pkSchedule->m_iID);
				if (NULL == pkScheduleEntry)
				{
					return false;
				}
				int32_t inRecodeDayKey = GET_SCHEDULE_KEY(iType, 0); //活动期间充值的最后一天
				int32_t iDay = LC_Helper::GetDeltaDay(pkScheduleEntry->CurrentStartTime()/ONE_SECOND_MILI, GET_CURRENT_TIMESTAMP_IN_SECONDS()) + 1;
				ACT_ScheduleSaveCommon* pkScheduleSaveCommon = pkSaveEntry->_addScheduleSaveDetail(iType);
				if(pkScheduleSaveCommon)
				{
					int nRecodeDay = pkScheduleSaveCommon->GetValue(inRecodeDayKey);
					if(nRecodeDay != iDay)
					{
						pkSaveEntry->SetCount(0);
						pkScheduleSaveCommon->SetValue(inRecodeDayKey, iDay);
						pkSaveCatagory->UpdateDirtyFlag();
						pkSaveActivity->UpdateDirtyFlag();
						pkAsset->UpdateDirtyFlag();
					}
				}

			}
		}
		return true;
	}
	ACT_ChargeRebatesManager* pkMgr;
	int32_t iType;
};

ResultType ACT_ChargeRebatesManager::CheckData(LC_ServerPlayer* pkPlayer)
{
	IF_A_NA_PLAYER (pkPlayer) return RE_FAIL;
	
	static ACT_ScheduleManager* pkScheduleMgr = ACT_ScheduleManager::GetSingletonPtr();
	Activity::ACT_ScheduleAsset& rkAsset = pkPlayer->GetACTScheduleAsset();
	Activity::ACT_ScheduleSaveActivity*	pkSaveActivity = rkAsset.GetScheduleSaveActivity(GetType());
	if (NULL == pkSaveActivity)
	{
		return RE_FAIL;
	}

	ChargeRebatesDailyOper kOper(pkScheduleMgr, pkPlayer, &rkAsset, pkSaveActivity, NULL);
	kOper.pkMgr = this;

	CatagoryDataMap::iterator it = CatagoryDatas().begin();
	for (; it!=CatagoryDatas().end(); ++it)
	{
		kOper.pkSaveCatagory = pkSaveActivity->GetScheduleSaveCatagory(it->first);
		kOper.iType = it->first;
		if (kOper.pkSaveCatagory)
		{
			it->second.ForAll(&kOper);
		}
	}

	return RE_SUCCESS;
}

void ACT_ChargeRebatesManager::InitConfig(CSVFile::CSVDataLoader* pkLoader, const CSVFile::CSVDataSourceList* pkCSVs)
{
	m_ChargeRebatesMap.clear();
	ACT_ScheduleDataActivity<ACT_ChargeRebatesDataEntry, CSVFile::CF_HolidayActivity_ChargeContinuous>::InitConfig(pkLoader, pkCSVs);

	CF_HolidayActivity_ChargeContinuous* pkLotteryReward = CF_HolidayActivity_ChargeContinuous::GetSingletonPtr();
	CF_HolidayActivity_ChargeContinuous::DataEntryMapExternIterator kIter = CF_HolidayActivity_ChargeContinuous::GetSingletonPtr()->GetIterator();
	while (kIter.HasMoreElements())
	{
		do {
			const CF_HolidayActivity_ChargeContinuous::DataEntry* pkEntry = kIter.PeekNextValuePtr();
			DataEntryType* pkDataEntry = GetDataEntry(pkEntry->_iType, pkEntry->_iSchedule, pkEntry->_iKey);
			if (NULL == pkDataEntry)
			{
				break;
			}
			AddACTEndDataEntryType(pkDataEntry);
			m_ChargeRebatesMap[pkEntry->_iThreshold] = pkEntry->_iRebateID;//
		} while (0);
		kIter.MoveNext();
	}
	InitiType(GetType());
}
//
//ResultType ACT_ChargeRebatesManager::ACTScheduleEnd(uint32_t iSchedule, LC_ServerPlayer* pkPlayer)
//{
//	if (NULL==pkPlayer)
//	{
//		return RE_FAIL;
//	}
//	std::map<uint32_t, std::vector<DataEntryType*> >::iterator iter = m_ReadySendMailData.find(iSchedule);
//	if(iter ==  m_ReadySendMailData.end())
//	{
//		return RE_FAIL;
//	}
//	std::map<uint32_t, GameLogic::LC_IDSet> nClearActDataMaps;
//
//	for(int i = 0; i < iter->second.size(); i++)
//	{
//		DataEntryType* pkData = iter->second[i];
//		ACTScheduleEndSendMail(pkPlayer, pkData);
//		if(pkData)
//		{
//			GameLogic::LC_IDSet& idLists = nClearActDataMaps[pkData->m_iSchdeule];
//			idLists.insert(pkData->m_iType);
//		}
//	}
//	pkPlayer->ClearScheduleData(GetType(), nClearActDataMaps);
//	return RE_SUCCESS;
//}

ResultType ACT_ChargeRebatesManager::ACTScheduleEndSendMail(LC_ServerPlayer* pkPlayer, DataEntryType* pkData, MailInfo& mail)
{
	if(NULL == pkData || pkPlayer == NULL)
	{
		return RE_FAIL;
	}

	ResultType rslt = CheckScheduleData(pkPlayer, pkData, pkData->m_iType, pkData->m_iSchdeule, pkData->m_iKey);
	if(rslt != RE_SUCCESS)
	{
		return RE_FAIL;
	}
	mail.m_nMailDesc = pkData->m_sMailTitle;
	mail.m_nMailTitle =  pkData->m_sMailTitle;
	mail.m_nReward.insert(mail.m_nReward.end(), pkData->m_kRewardItems.begin(), pkData->m_kRewardItems.end());
	mail.m_nPlayerID = pkPlayer->GetInstance();
	ResultType res = RE_SUCCESS;//UT_ServerHelper::SendMailToPlayer(pkPlayer->GetInstance(), pkData->m_sMailTitle, pkData->m_sMailDesc, pkData->m_kRewardItems);
	if(RE_SUCCESS == res)
	{
		EndDoReward(pkPlayer, pkData, pkData->m_iType, pkData->m_iSchdeule, pkData->m_iKey);
	}
	return res;
}
//
//void ACT_ChargeRebatesManager::LoginCheckACTScheduleEnd(LC_ServerPlayer* pkPlayer)
//{
//	if (NULL==pkPlayer)
//	{
//		return ;
//	}
//	if(m_ReadySendMailData.size() <= 0) return ;
//
//	std::map<uint32_t, GameLogic::LC_IDSet> nClearActDataMaps;
//
//	std::map<uint32_t, std::vector<DataEntryType*> >::iterator iter = m_ReadySendMailData.begin();
//	for(;iter !=  m_ReadySendMailData.end(); iter++)
//	{
//		static ACT_ScheduleManager* pkScheduleMgr = ACT_ScheduleManager::GetSingletonPtr();
//		if (pkScheduleMgr->IsScheduleValid(pkPlayer, iter->first))
//		{
//			continue;
//		}
//		for(int i = 0; i < iter->second.size(); i++)
//		{
//			DataEntryType* pkData = iter->second[i];
//			ACTScheduleEndSendMail(pkPlayer, pkData);
//			if(pkData)
//			{
//				GameLogic::LC_IDSet& idLists = nClearActDataMaps[pkData->m_iSchdeule];
//				idLists.insert(pkData->m_iType);
//			}
//		}
//		pkPlayer->ClearScheduleData(GetType(), nClearActDataMaps);
//	}
//}