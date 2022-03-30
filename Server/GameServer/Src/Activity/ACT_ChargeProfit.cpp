#include "ACT_ChargeProfit.h"
#include "ACT_ActivitySchedule.h"

#include "LC_ServerPlayer.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerLogicManager.h"

#include "UT_ServerHelper.h"
#include "CF_ACTChargeProfitType.h"
#include "CF_ACTChargeProfit.h"
#include "CF_ACTChargeProfitA.h"
using namespace CSVFile;
using namespace GameLogic;
using namespace Activity;

ResultType ACT_ChargeProfitDataEntry::DoReward(LC_ServerPlayer* pkPlayer, GameLogic::LC_RewardItemsExtend& rkExtend)
{
	if (NULL==pkPlayer)
	{
		return RE_FAIL;
	}

	ResultType uiRslt = RE_SUCCESS;
	do
	{
		Utility::UT_SIMDataList kItems;
		GameLogic::LC_ItemsReward kRewardItems;
		kRewardItems.SetUp(m_kRewardItems);

		rkExtend.SetChoice(kRewardItems.CheckChoiceID(pkPlayer->GetWeaponAttribute(), rkExtend.Choice()));
		kRewardItems.Resolve(pkPlayer, kItems, &rkExtend);
		if( LC_Helper::CanAddItemEx( pkPlayer, kItems, CIET_NONE ) != RE_SUCCESS )
		{
			uiRslt = RE_ITEM_CANT_PUT_PACKAGE;
			break;
		}
		if (!kItems.empty())
		{
			uiRslt = UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, kItems, pkPlayer, SYS_FUNC_TP_ACTIVITY_CHARGE
				, m_iID, m_sMailTitle, m_sMailDesc, m_bMailEnable);// m_sMailTitle, m_sMailDesc, m_bMailEnable 用于补偿邮件的发送
			if (RE_SUCCESS != uiRslt)
			{
				break;
			}
		}
	}while(0);

	return uiRslt;
}

struct ChargeScheduleOper : public Activity::ACT_ScheduleSaveDataOper<ACT_ChargeProfitManager::DataEntryType>
{
	ChargeScheduleOper(ACT_ScheduleManager* schedule, LC_ServerPlayer* player, ACT_ScheduleAsset* asset, ACT_ScheduleSaveActivity* act, ACT_ScheduleSaveCatagory* cat)
		:Activity::ACT_ScheduleSaveDataOper<ACT_ChargeProfitManager::DataEntryType>(schedule, player, asset, act, cat)
		,iValue(0)
	{}
	virtual bool operator ()(ACT_ChargeProfitManager::ScheduleDataType* pkSchedule)
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

void ACT_ChargeProfitManager::AddCharge(LC_ServerPlayer* pkPlayer, int32_t val, const mem::vector<int32_t>& kCatagorys)
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

	ChargeScheduleOper kOper(pkScheduleMgr, pkPlayer, &rkAsset, pkSaveActivity, NULL);
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

void ACT_ChargeProfitManager::OnCharge(LC_ServerPlayer* pkPlayer, int32_t val, const GameLogic::LC_IDSet* pkCatagorys)
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

	ChargeScheduleOper kOper(pkScheduleMgr, pkPlayer, &rkAsset, pkSaveActivity, NULL);
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


void ACT_ChargeProfitManager::NewOnCharge(LC_ServerPlayer* pkPlayer,int32_t nValueType, int32_t val, const GameLogic::LC_IDSet* pkCatagorys)
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

	ChargeScheduleOper kOper(pkScheduleMgr, pkPlayer, &rkAsset, pkSaveActivity, NULL);
	kOper.iValue = val;

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
			CF_ACTChargeProfitType::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_ACTChargeProfitType>(*cit);
			if(NULL == pkData)
			{
				continue;
			}
			if(pkData->_Moneytype != nValueType)
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

ResultType ACT_ChargeProfitManager::DrawReward(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t iSchedule, int32_t iKey, GameLogic::LC_RewardItemsExtend& rkExtend, const GameLogic::LC_IDVector* pkExtraParams, int& count)
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
	if (!pkScheduleMgr->IsScheduleValid(pkPlayer->GetServerID(), pkDataEntry->m_iShowSchdeule))
	{
		return RE_ACTIVITY_NOT_VALID;
	}

	ResultType rslt = CheckScheduleData(pkPlayer, pkDataEntry, iCatagory, iSchedule, iKey);
	if (RE_SUCCESS != rslt)
	{
		return rslt;
	}
	int32_t iRewardKey = GET_SCHEDULE_KEY(LC_ST_TP_FORERVER, ACT_SCHEDULE_REWARD_COUNT);

	rslt = pkDataEntry->DoReward(pkPlayer, rkExtend);
	if (RE_SUCCESS == rslt)
	{
		EndDoReward(pkPlayer, pkDataEntry, iCatagory, iSchedule, iKey);
	}
	return rslt;
}

ResultType ACT_ChargeProfitManager::EndDoReward(LC_ServerPlayer* pkPlayer, DataEntryType* pkDataEntry, int32_t iCatagory, int32_t iSchedule, int32_t iKey)
{
	if(NULL == pkPlayer || NULL == pkDataEntry)
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
	pkSaveSchedule->RegReward(pkDataEntry->m_iSaveIndex);
	ACT_ScheduleSaveCommon* pkSaveDetail = pkSaveSchedule->_addScheduleSaveDetail(iKey);
	if (NULL == pkSaveDetail)
	{
		return RE_FAIL;
	}

	int32_t iRewardKey = GET_SCHEDULE_KEY(LC_ST_TP_FORERVER, ACT_SCHEDULE_REWARD_COUNT);

	pkSaveDetail->AddValue(iRewardKey, 1);
	
	pkSaveCatagory->UpdateDirtyFlag();
	pkSaveActivity->UpdateDirtyFlag();
	pkPlayer->GetACTScheduleAsset().UpdateDirtyFlag();
	return RE_SUCCESS;
}


ResultType ACT_ChargeProfitManager::CheckScheduleData(LC_ServerPlayer* pkPlayer, DataEntryType* pkDataEntry, int32_t iCatagory, int32_t iSchedule, int32_t iKey)
{
	if(NULL == pkPlayer || NULL == pkDataEntry)
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

	if (iKey > (int32_t)pkSaveSchedule->GetCount())
	{
		return RE_ACTIVITY_CONDITION_FAIL;
	}

	ACT_ScheduleSaveCommon* pkSaveDetail = pkSaveSchedule->_addScheduleSaveDetail(iKey);
	if (NULL == pkSaveDetail)
	{
		return RE_FAIL;
	}

	int32_t iTimes = (int32_t)pkSaveSchedule->GetCount() / iKey;
	int32_t iRewardKey = GET_SCHEDULE_KEY(LC_ST_TP_FORERVER, ACT_SCHEDULE_REWARD_COUNT);
	int32_t iRewardTimes = (int32_t)pkSaveDetail->GetValue(iRewardKey);

	//compatible with the old datas, if rewarded should had been rewared once at least!
	if (pkSaveSchedule->IsRewardReg(pkDataEntry->m_iSaveIndex))
	{
		iRewardTimes = UT_MathBase::MaxInt(iRewardTimes, 1);
	}

	if (iRewardTimes >= iTimes)
	{
		return RE_ACTIVITY_REWARDED;
	}

	if (pkDataEntry->m_iTimeLimits>=0 && iRewardTimes>=pkDataEntry->m_iTimeLimits)
	{
		return RE_ACTIVITY_TIMES_OUT;
	}
	return RE_SUCCESS;
}

void ACT_ChargeProfitManager::InitConfig(CSVFile::CSVDataLoader* pkLoader, const CSVFile::CSVDataSourceList* pkCSVs)
{
	ACT_ScheduleDataActivity<ACT_ChargeProfitDataEntry, CSVFile::CF_ACTChargeProfitA>::InitConfig(pkLoader, pkCSVs);
	CF_ACTChargeProfitA::DataEntryMapExternIterator iter = CF_ACTChargeProfitA::GetSingletonPtr()->GetIterator();
	while (iter.HasMoreElements())
	{
		do {
			const CF_ACTChargeProfitA::DataEntry* pkEntry = iter.PeekNextValuePtr();
			DataEntryType* pkDataEntry = GetDataEntry(pkEntry->_iType, pkEntry->_iSchedule, pkEntry->_iKey);
			if (NULL == pkDataEntry)
			{
				continue;
			}
			AddACTEndDataEntryType(pkDataEntry);
		} while (0);
		iter.MoveNext();
	}
	InitiType(GetType());
}


//
//ResultType ACT_ChargeProfitManager::ACTScheduleEnd(uint32_t iSchedule, LC_ServerPlayer* pkPlayer)
//{
//	if (NULL==pkPlayer)
//	{
//		return RE_FAIL;
//	}
//	DataEntryTypeMaps::iterator iter = m_nDataEntryTypeMaps.find(iSchedule);
//	if(iter ==  m_nDataEntryTypeMaps.end())
//	{
//		return RE_FAIL;
//	}
//	std::map<uint32_t, GameLogic::LC_IDSet> nClearActDataMaps;
//
//	for(DataEntryTypeMap::iterator it = iter->second.begin(); it != iter->second.end();it++)
//	{
//		MailInfo mail;//
//		for(int i = 0; i < it->second.size(); i++)
//		{
//			DataEntryType* pkData = iter->second[i];
//			ACTScheduleEndSendMail(pkPlayer, pkData, mail);
//			if(pkData)
//			{
//				GameLogic::LC_IDSet& idLists = nClearActDataMaps[pkData->m_iSchdeule];
//				idLists.insert(pkData->m_iType);
//			}
//		}
//		if(mail.m_nPlayerID)
//		{
//			UT_ServerHelper::SendMailToPlayer(pkPlayer->GetInstance(), mail.m_nMailTitle, mail.m_nMailDesc, mail.m_nReward);
//		}
//		
//	}
//	pkPlayer->ClearScheduleData(GetType(), nClearActDataMaps);
//
//	return RE_SUCCESS;
//}

ResultType ACT_ChargeProfitManager::ACTScheduleEndSendMail(LC_ServerPlayer* pkPlayer, DataEntryType* pkData, MailInfo& mail)
{
	if(NULL == pkData || pkPlayer == NULL)
	{
		return RE_FAIL;
	}
	ResultType rslt = CheckScheduleData(pkPlayer, pkData, pkData->m_iType, pkData->m_iSchdeule, pkData->m_iKey);
	if (RE_SUCCESS != rslt)
	{
		return RE_FAIL;
	}
	mail.m_nMailDesc = pkData->m_sMailTitle;
	mail.m_nMailTitle =  pkData->m_sMailTitle;
	mail.m_nReward.insert(mail.m_nReward.end(), pkData->m_kRewardItems.begin(), pkData->m_kRewardItems.end());
	mail.m_nPlayerID = pkPlayer->GetInstance();
	return EndDoReward(pkPlayer, pkData, pkData->m_iType, pkData->m_iSchdeule, pkData->m_iKey);
}
//
//void ACT_ChargeProfitManager::LoginCheckACTScheduleEnd(LC_ServerPlayer* pkPlayer)
//{
//	if (NULL==pkPlayer)
//	{
//		return ;
//	}
//	if(m_nDataEntryTypeMaps.size() <= 0) return ;
//
//	std::map<uint32_t, GameLogic::LC_IDSet> nClearActDataMaps;
//
//	DataEntryTypeMaps::iterator iter = m_nDataEntryTypeMaps.begin();
//	for(;iter !=  m_nDataEntryTypeMaps.end(); iter++)
//	{
//		static ACT_ScheduleManager* pkScheduleMgr = ACT_ScheduleManager::GetSingletonPtr();
//		if (pkScheduleMgr->IsScheduleValid(pkPlayer, iter->first))
//		{
//			continue;
//		}
//		for(DataEntryTypeMap::iterator it = iter->second.begin(); it != iter->second.end();it++)
//		{
//			MailInfo mail;//
//			for(int i = 0; i < it->second.size(); i++)
//			{
//				DataEntryType* pkData = iter->second[i];
//				if(pkData)
//				{
//					GameLogic::LC_IDSet& idLists = nClearActDataMaps[pkData->m_iSchdeule];
//					idLists.insert(pkData->m_iType);
//				}
//				ACTScheduleEndSendMail(pkPlayer, pkData, mail);
//			}
//			if(mail.m_nPlayerID)
//			{
//				UT_ServerHelper::SendMailToPlayer(pkPlayer->GetInstance(), mail.m_nMailTitle, mail.m_nMailDesc, mail.m_nReward);
//			}	
//			
//		}
//		pkPlayer->ClearScheduleData(GetType(), nClearActDataMaps);
//	}
//}