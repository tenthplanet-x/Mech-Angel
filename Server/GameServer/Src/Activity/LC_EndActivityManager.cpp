#include "LC_EndActivityManager.h"
#include "LC_ServerPlayer.h"
#include "LC_ActorBase.h"
#include "LC_Helper.h"
#include "LC_Define.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerLogicManager.h"
#include "ACT_ActivityAsset.h"
#include "LC_ActShopManager.h"
#include "ChargeProfitConfigManager.h"
#include "GameServerApp.h"
#include "CF_HolidayActivitySecondary.h"
#include "CF_ACTShopA.h"
#include "CF_ACTChargeProfitB.h"

SendRewardMailAndResetCash::SendRewardMailAndResetCash(StringType& str, int type)
{
	m_vTypes.clear();
	LC_Helper::SpliteStringInt(str, m_vTypes);
	m_iType = type;
}

SendRewardMailAndResetCash::~SendRewardMailAndResetCash()
{

}

void SendRewardMailAndResetCash::GetACTScheduleEndInfo(LC_ActorBase* pkActor, const CSVFile::CF_ACTShopA::DataEntry* pkData, MailInfo& mailInfo, std::set<int>& cashType)
{
	if(NULL==pkData || NULL==pkActor)
	{
		return;
	}

	LC_ServerPlayer* pkOwnerPlayer = dynamic_cast<LC_ServerPlayer*>(pkActor);
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return;
	}

	Activity::ACT_ScheduleSaveActivity* pkSaveActivity = pkOwnerPlayer->GetACTScheduleAsset()._addScheduleSaveActivity(pkData->_iType);
	if (NULL == pkSaveActivity)
	{
		return;
	}

	Activity::ACT_ScheduleSaveCatagory* pkSaveCatagory = pkSaveActivity->_addScheduleSaveCatagory(pkData->_iType);
	if (NULL == pkSaveCatagory)
	{
		return;
	}

	Activity::ACT_ScheduleSaveEntry* pkSaveSchedule = pkSaveCatagory->_addScheduleSaveEntry(pkData->_iSchedule);
	if (NULL == pkSaveSchedule)
	{
		return;
	}

	Activity::ACT_ScheduleSaveCommon* pkSaveDetail = pkSaveSchedule->_addScheduleSaveDetail(pkData->_iKey);
	if (NULL == pkSaveDetail)
	{
		return;
	}

	for(Utility::LC_ComMap::const_iterator it=pkData->_kCurrentPrices.begin(); it!=pkData->_kCurrentPrices.end(); ++it)
	{
		cashType.insert(it->first);
	}

	for (Utility::UT_SIMDataMap::const_iterator it = pkData->_kLimits.begin(); it!=pkData->_kLimits.end(); ++it)
	{
		const Utility::UT_SIMDataInfo& rkLimit = it->second;
		if(!GameLogic::IS_VALID_STAT_TYPE(rkLimit.StatType()))
		{
			continue;
		}

		int32_t iStatKey = Activity::GET_SCHEDULE_KEY(rkLimit.StatType(), Activity::ACT_SCHEDULE_DETAIL_COUNT);
		if ((int32_t)pkSaveDetail->GetValue(iStatKey) +1 > rkLimit.StatValue())
		{
			continue;
		}

		ResultType res = LC_Helper::CheckCashMap(pkOwnerPlayer, pkData->_kCurrentPrices);
		if(RE_SUCCESS != res)
		{
			continue;
		}

		mailInfo.m_nPlayerID = pkOwnerPlayer->GetInstance();
		mailInfo.m_nMailTitle = pkData->_sMailTitle;
		mailInfo.m_nMailDesc = pkData->_sMailDesc;
		mailInfo.m_nReward.insert(mailInfo.m_nReward.end(), pkData->_kItems.begin(), pkData->_kItems.end());
		
		pkSaveDetail->AddValue(iStatKey, 1);

		pkSaveCatagory->UpdateDirtyFlag();
		pkSaveActivity->UpdateDirtyFlag();
		pkOwnerPlayer->GetACTScheduleAsset().UpdateDirtyFlag();
	}
}

bool SendRewardMailAndResetCash::operator()(GameLogic::LC_ActorBase* pkActor)
{
	if (pkActor && IS_PLAYER_ID(pkActor->GetID()))
	{
		LC_ServerPlayer* pkOwnerPlayer = (LC_ServerPlayer*)pkActor;
		if(NULL == pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
		{
			return false;
		}

		std::set<int> cashType;
		MailInfo mail;
		std::map<uint32_t, GameLogic::LC_IDSet> nClearActDataMaps;

		for(uint32_t i=0; i<m_vTypes.size(); ++i)
		{
			CActShopManager* pkManager = CActShopManager::GetSingletonPtr();
			const mem::vector<const CSVFile::CF_ACTShopA::DataEntry*>* pkCSVDataVector = pkManager->GetActShopCSVDataByShopType(m_vTypes[i]);
			if(NULL == pkCSVDataVector)
			{
				continue;
			}

			for(uint32_t i=0; i<(*pkCSVDataVector).size(); ++i)
			{				
				const CSVFile::CF_ACTShopA::DataEntry* pkData = (*pkCSVDataVector)[i];
				if(NULL != pkData)
				{
					GameLogic::LC_IDSet& idLists = nClearActDataMaps[pkData->_iSchedule];
					idLists.insert(pkData->_iType);

					GetACTScheduleEndInfo(pkActor, pkData, mail, cashType);
				}
			}
		}

		for(std::set<int>::iterator it=cashType.begin(); it!=cashType.end(); ++it)
		{
			pkOwnerPlayer->ResetCash((CashType)*it);
			pkOwnerPlayer->ClearScheduleData(m_iType, nClearActDataMaps);
		}

		UT_ServerHelper::SendMail(pkOwnerPlayer, mail);
	}
	return true;
}

SendRankRewardMailToPlayer::SendRankRewardMailToPlayer(StringType& str) : m_iType(0)
{
	m_vChargeProfitID.clear();
	LC_Helper::SpliteStringInt(str, m_vChargeProfitID);

	if(!m_vChargeProfitID.empty())
	{
		CF_ACTChargeProfitB::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_ACTChargeProfitB>(m_vChargeProfitID[0]);
		if(NULL != pkData)
		{
			m_iType = pkData->_iType;
		}
	}
}

SendRankRewardMailToPlayer::~SendRankRewardMailToPlayer()
{

}

void SendRankRewardMailToPlayer::DoEndActivity()
{
	//entry¸ºÔð·¢½±
	if(GetGameServerID() != GetGameServerEntryID())
	{
		return;
	}

	for(uint32_t i=0; i<m_vChargeProfitID.size(); ++i)
	{
		mem::vector<uint64_t> allPlayers;
		bool tag = UT_ServerHelper::GetPlayerRankRangeFromRedis(m_iType, m_vChargeProfitID[i], allPlayers);
		if(!tag && !allPlayers.empty())
		{
			UT_ServerHelper::DoSendRankRewardToPlayer(m_iType, m_vChargeProfitID[i], allPlayers, RewardXlsx_Type_2);
		}
	}
}

SendChargeProfitReward::SendChargeProfitReward(int id) : m_vChargeProfitBID(id)
{

}

SendChargeProfitReward::~SendChargeProfitReward()
{

}

bool SendChargeProfitReward::operator()(GameLogic::LC_ActorBase* pkActor)
{
	if (NULL==pkActor || !IS_PLAYER_ID(pkActor->GetID()))
	{
		return false;
	}

	LC_ServerPlayer* pkOwnerPlayer = (LC_ServerPlayer*)pkActor;
	if(NULL == pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return false;
	}

	CChargeProfitConfigManager* pMgr = CChargeProfitConfigManager::GetSingletonPtr();
	SChargeProfitConfig* conf = pMgr->GetChargeProfitConfig( m_vChargeProfitBID );
	if(NULL == conf)
	{
		return false;
	}

	Utility::UT_SIMDataList mailItemList;
	pkOwnerPlayer->GetChargeProfitAsset().FillCanChargeProfitReward(m_vChargeProfitBID, mailItemList);
	if(!mailItemList.empty())
	{
		Utility::UT_SIMDataList mergedMailItemList;
		UT_ServerHelper::MergeItemList(mailItemList, mergedMailItemList);
		UT_ServerHelper::SendMailToPlayer(pkOwnerPlayer->GetInstance(), conf->m_sMailTitle, conf->m_sMailDesc, mergedMailItemList);
	}

	pkOwnerPlayer->GetChargeProfitAsset().ResetChargeProfitInfo(m_vChargeProfitBID);

	return true;
}

CEndActivityManager::CEndActivityManager()
{
	m_SendRewardMailAndResetCashMap.clear();
	m_SendRankRewardMailToPlayerMap.clear();
	m_SendChargeProfitRewardMap.clear();
}

CEndActivityManager::~CEndActivityManager()
{

}

void CEndActivityManager::Init()
{
	CF_HolidayActivitySecondary::DataEntryMapExternIterator kIt = CF_HolidayActivitySecondary::GetSingletonPtr()->GetIterator();
	for(; kIt.HasMoreElements(); kIt.MoveNext())
	{
		CF_HolidayActivitySecondary::DataEntry* pkData = kIt.PeekNextValuePtr();
		if(HolidayActivityType_9 == pkData->_iType)
		{
			SendRewardMailAndResetCash data(pkData->_sActivity, Activity::ACT_ScheduleActivityType::ACT_SCHEDULE_TP_SHOP);
			m_SendRewardMailAndResetCashMap.insert(std::make_pair(pkData->_iScheduleShow, data));
		}
		else if(HolidayActivityType_8 == pkData->_iType)
		{
			SendRankRewardMailToPlayer data(pkData->_sActivity);
			m_SendRankRewardMailToPlayerMap.insert(std::make_pair(pkData->_iSchedule, data));
		}
	}

	CF_ACTChargeProfitB::DataEntryMapExternIterator kIt2 = CF_ACTChargeProfitB::GetSingletonPtr()->GetIterator();
	for(; kIt2.HasMoreElements(); kIt2.MoveNext())
	{
		CF_ACTChargeProfitB::DataEntry* pkData = kIt2.PeekNextValuePtr();
		if(pkData->_iType == Charge_Type_2)
		{
			SendChargeProfitReward data(pkData->_iID);
			m_SendChargeProfitRewardMap.insert(std::make_pair(pkData->_iScheduleShow, data));
		}
	}
}

void CEndActivityManager::Unit()
{
	m_SendRewardMailAndResetCashMap.clear();
	m_SendRankRewardMailToPlayerMap.clear();
	m_SendChargeProfitRewardMap.clear();
}

void CEndActivityManager::EndActivity(int32_t scheduleID)
{
	static LC_ServerWorldManager* pkWorldMgr = SERVER_GET_WORLD_MANAGER();
	std::map<int32_t, SendRewardMailAndResetCash>::iterator findIt1 = m_SendRewardMailAndResetCashMap.find(scheduleID);
	if(findIt1 != m_SendRewardMailAndResetCashMap.end())
	{
		pkWorldMgr->ForAllPlayer(&findIt1->second);
	}

	std::map<int32_t, SendRankRewardMailToPlayer>::iterator findIt2 = m_SendRankRewardMailToPlayerMap.find(scheduleID);
	if(findIt2 != m_SendRankRewardMailToPlayerMap.end())
	{
		findIt2->second.DoEndActivity();
	}

	std::map<int32_t, SendChargeProfitReward>::iterator findIt3 = m_SendChargeProfitRewardMap.find(scheduleID);
	if(findIt3 != m_SendChargeProfitRewardMap.end())
	{
		pkWorldMgr->ForAllPlayer(&findIt3->second);
	}
}

void CEndActivityManager::CheckEndActivity(LC_ServerPlayer* pkOwnerPlayer)
{
	for(std::map<int32_t, SendRewardMailAndResetCash>::iterator it=m_SendRewardMailAndResetCashMap.begin(); it!=m_SendRewardMailAndResetCashMap.end(); ++it)
	{
		if(!UT_ServerHelper::CheckActivityVaild(it->first))
		{
			it->second(pkOwnerPlayer);
		}
	}

	for(std::map<int32_t, SendChargeProfitReward>::iterator it2=m_SendChargeProfitRewardMap.begin(); it2!=m_SendChargeProfitRewardMap.end(); ++it2)
	{
		if(!UT_ServerHelper::CheckActivityVaild(it2->first))
		{
			it2->second(pkOwnerPlayer);
		}
	}	
}

void CEndActivityManager::RestartCheckEndActivity()
{
	for(std::map<int32_t, SendRankRewardMailToPlayer>::iterator it=m_SendRankRewardMailToPlayerMap.begin();it!=m_SendRankRewardMailToPlayerMap.end(); ++it)
	{
		it->second.DoEndActivity();
	}
}