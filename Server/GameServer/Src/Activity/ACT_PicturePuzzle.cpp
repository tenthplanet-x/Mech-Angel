#include "ACT_PicturePuzzle.h"
#include "ACT_ActivityDataManager.h"
#include "ACT_ActivitySchedule.h"
#include "LC_ServerPlayer.h"
#include "UT_ServerHelper.h"
#include "SC_ScriptAction.h"
#include "CF_HolidayActivity_PinTuUnlock.h"
using namespace CSVFile;
using namespace GameLogic;
using namespace Activity;


ResultType ACT_PicturePuzzleDataEntry::DoReward(LC_ServerPlayer* pkPlayer, GameLogic::LC_RewardItemsExtend& rkExtend)
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
			uiRslt = UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, kItems, pkPlayer, SYS_FUNC_TP_ACT_PinTuReslut
				, m_iID, m_sMailTitle, m_sMailDesc, m_bMailEnable);
			if (RE_SUCCESS != uiRslt)
			{
				break;
			}
		}
	}while(0);

	return uiRslt;
}

void ACT_PicturePuzzleManager::OnFinishTaskAndAchievement(LC_ServerPlayer* pkPlayer, int32_t nType, int32_t nFinishId, const GameLogic::LC_IDSet* pkCatagorys)
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
	if (NULL==pkSaveActivity)
	{
		return;
	}
	CF_HolidayActivity_PinTuUnlock::DataEntryMapExternIterator kIter = CF_HolidayActivity_PinTuUnlock::GetSingletonPtr()->GetIterator();
	for(; kIter.HasMoreElements(); kIter.MoveNext())
	{
		CF_HolidayActivity_PinTuUnlock::DataEntry* pkData = kIter.PeekNextValuePtr();
		if(NULL == pkData)
		{
			continue;
		}
		if (!pkScheduleMgr->IsScheduleValid(pkPlayer->GetServerID(), pkData->_iSchedule))
		{
			continue;
		}

		if(nType == 1 && pkData->_iTaskID != nFinishId)
		{
			continue;
		}
		else if(nType == 2 && pkData->_iAchievementID != nFinishId)
		{
			continue;
		}

		ACT_ScheduleSaveCatagory* pkSaveCatagory = pkSaveActivity->_addScheduleSaveCatagory(pkData->_iType);
		if (pkSaveCatagory)
		{
			ACT_ScheduleSaveEntry* pkSaveEntry = pkSaveCatagory->_addScheduleSaveEntry(pkData->_iSchedule);
			if (NULL != pkSaveEntry)
			{
				pkSaveEntry->AddCount(1);	
				pkSaveCatagory->UpdateDirtyFlag();
				pkSaveActivity->UpdateDirtyFlag();
				rkAsset.UpdateDirtyFlag();

			}
		}
	}

}

ResultType ACT_PicturePuzzleManager::DrawReward(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t iSchedule, int32_t iKey, GameLogic::LC_RewardItemsExtend& rkExtend, const GameLogic::LC_IDVector* pkExtraParams, int& count)
{
	if (NULL == pkPlayer)
	{
		return RE_FAIL;
	}
	DataEntryType* pkDataEntry = GetDataEntry(iCatagory, iSchedule, iKey);
	if (NULL == pkDataEntry)
	{
		return RE_ACTIVITY_BAD_PARAMS;
	}

	static ACT_ScheduleManager* pkScheduleMgr = ACT_ScheduleManager::GetSingletonPtr();
	if (NULL == pkScheduleMgr)
	{
		return RE_FAIL;
	}
	if (!pkScheduleMgr->IsScheduleValid(pkPlayer->GetServerID(), pkDataEntry->m_iShowSchdeule))
	{
		return RE_ACTIVITY_NOT_VALID;
	}

	ResultType rslt = CheckScheduleData(pkPlayer, pkDataEntry, iCatagory, iSchedule, iKey);
	if(rslt != RE_SUCCESS)
	{
		return rslt;
	}

	rslt = pkDataEntry->DoReward(pkPlayer, rkExtend);
	if (RE_SUCCESS == rslt)
	{
		EndDoReward(pkPlayer, pkDataEntry, iCatagory, iSchedule, iKey);
	}
	return RE_SUCCESS;
}


ResultType ACT_PicturePuzzleManager::CheckScheduleData(LC_ServerPlayer* pkPlayer, DataEntryType* pkDataEntry, int32_t iCatagory, int32_t iSchedule, int32_t iKey)
{
	if(NULL == pkPlayer || NULL == pkDataEntry)
	{
		return RE_FAIL;
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

	ACT_ScheduleSaveCommon* pkSaveDetail = pkSaveSchedule->_addScheduleSaveDetail(pkDataEntry->m_iKey);
	if (NULL == pkSaveDetail)
	{
		return RE_FAIL;
	}
	int32_t iRewardKey = GET_SCHEDULE_KEY(LC_ST_TP_FORERVER, ACT_SCHEDULE_REWARD_COUNT);
	if(pkSaveSchedule->IsRewardReg(pkDataEntry->m_kSaveIndex))
	{
		return RE_ACTIVITY_REWARDED;
	}
	if (pkSaveSchedule->GetCount() < pkDataEntry->m_iKey)
	{
		return RE_ACTIVITY_CONDITION_FAIL;
	}
	return RE_SUCCESS;
}


ResultType ACT_PicturePuzzleManager::EndDoReward(LC_ServerPlayer* pkPlayer, DataEntryType* pkDataEntry, int32_t iCatagory, int32_t iSchedule, int32_t iKey)
{
	if(NULL == pkPlayer || NULL == pkDataEntry)
	{
		return RE_FAIL;
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
	pkSaveSchedule->RegReward(pkDataEntry->m_kSaveIndex);

	ACT_ScheduleSaveCommon* pkSaveDetail = pkSaveSchedule->_addScheduleSaveDetail(pkDataEntry->m_iKey);
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



void ACT_PicturePuzzleManager::InitConfig(CSVFile::CSVDataLoader* pkLoader, const CSVFile::CSVDataSourceList* pkCSVs)
{
	ACT_ScheduleDataActivity<ACT_PicturePuzzleDataEntry, CSVFile::CF_HolidayActivity_PinTuReward>::InitConfig(pkLoader, pkCSVs);

	CF_HolidayActivity_PinTuReward* pkLotteryReward = CF_HolidayActivity_PinTuReward::GetSingletonPtr();
	CF_HolidayActivity_PinTuReward::DataEntryMapExternIterator kIter = CF_HolidayActivity_PinTuReward::GetSingletonPtr()->GetIterator();
	while (kIter.HasMoreElements())
	{
		do {
			const CF_HolidayActivity_PinTuReward::DataEntry* pkEntry = kIter.PeekNextValuePtr();
			DataEntryType* pkDataEntry = GetDataEntry(pkEntry->_iType, pkEntry->_iSchedule, pkEntry->_iKey);
			if (NULL == pkDataEntry)
			{
				break;
			}
			AddACTEndDataEntryType(pkDataEntry);
		} while (0);
		kIter.MoveNext();
	}
	InitiType(GetType());
}

//ResultType ACT_PicturePuzzleManager::ACTScheduleEnd(uint32_t iSchedule, LC_ServerPlayer* pkPlayer)
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

 ResultType ACT_PicturePuzzleManager::ACTScheduleEndSendMail(LC_ServerPlayer* pkPlayer, DataEntryType* pkData, MailInfo& mail)
{
	if(NULL == pkData || pkPlayer == NULL)
	{
		return RE_FAIL ;
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
	return RE_SUCCESS;
}
//
//void ACT_PicturePuzzleManager::LoginCheckACTScheduleEnd(LC_ServerPlayer* pkPlayer)
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
