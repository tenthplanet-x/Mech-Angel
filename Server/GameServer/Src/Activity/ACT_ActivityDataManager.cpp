#include "ACT_ActivityDataManager.h"
#include "ACT_ActivitySchedule.h"

#include "ACT_Subordinate.h"
#include "ACT_ChargeProfit.h"
#include "ACT_ContinuousCharge.h"
#include "ACT_ConsumeProfit.h"
#include "ACT_Shop.h"
#include "ACT_Login.h"
#include "ACT_Shot.h"
#include "ACT_ChargeInterval.h"
#include "ACT_ConsumeRebates.h"
#include "ACT_SignIn.h"
#include "ACT_ChargeRebates.h"
#include "ACT_OfferAReward.h"
#include "ACT_Monopoly.h"
#include "ACT_SnowMan.h"
#include "ACT_ClimbTower.h"
#include "ACT_SpringTree.h"
#include "ACT_RefreshShop.h"
#include "ACT_SubordinateConsume.h"
#include "ACT_Lottery.h"
#include "ACT_TimeLimitedShop.h"
#include "CF_BossFirstKill.h"

#include "LC_ServerPlayer.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerLogicManager.h"
#include "GameServerApp.h"
#include "UT_ServerHelper.h"
#include "CF_TaskRankRewards.h"
#include "GlobalSettings.h"
#include "CF_InvestmentPlan.h"
#include "CF_InvestmentCard.h"
#include "MG_Game2Guild.h"
#include "LC_ServerGuildManager.h"
#include "LC_ActSubordinateRewardManager.h"
#include "ACT_PicturePuzzle.h"
#include "UT_OperateLog.h"
using namespace OPERATELOG;
using namespace Activity;
using namespace CSVFile;

void ACT_ScheduleDataActivityBase::RegHooks(void)
{
	static ACT_ScheduleManager* pkMgr = ACT_ScheduleManager::GetSingletonPtr();
	ScheduleIndices::iterator it = m_kScheduleIndices.begin();
	for (; it!=m_kScheduleIndices.end(); ++it)
	{
		pkMgr->RegHook(it->first, m_iType);
	}
}
void ACT_ScheduleDataActivityBase::UnregHooks(void)
{
	static ACT_ScheduleManager* pkMgr = ACT_ScheduleManager::GetSingletonPtr();
	ScheduleIndices::iterator it = m_kScheduleIndices.begin();
	for (; it!=m_kScheduleIndices.end(); ++it)
	{
		pkMgr->UnregHook(it->first, m_iType);
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ACT_ScheduleDataManager::Init(void)
{
	for (int i=ACT_SCHEDULE_TP_BEGIN; i<ACT_SCHEDULE_TP_END; ++i)
	{
		ACT_ScheduleDataActivityBase* pkActivity = NULL;
		switch (i)
		{
		case ACT_SCHEDULE_TP_SUBORDINATE:
			pkActivity = T_NEW_D ACT_SubordinateManager();
			break;
		case ACT_SCHEDULE_TP_CHARGE:
			pkActivity = T_NEW_D ACT_ChargeProfitManager();
			break;
		case ACT_SCHEDULE_TP_CONTINUOUS_CHARGE:
			pkActivity = T_NEW_D ACT_ContinuousChargeManager();  //累积充值
			break;
		case ACT_SCHEDULE_TP_CONSUME:
			pkActivity = T_NEW_D ACT_ConsumeProfitManager();
			break;
		case ACT_SCHEDULE_TP_SHOP:
			pkActivity = T_NEW_D ACT_ShopManager();
			break;
		case ACT_SCHEDULE_TP_LOGIN:
			pkActivity = T_NEW_D ACT_LoginManager();
			break;
		case ACT_SCHEDULE_TP_SHOT:
			pkActivity = T_NEW_D ACT_ShotManager();
			break;
		case ACT_SCHEDULE_TP_CHARGE_INTERVAL:
			pkActivity = T_NEW_D ACT_ChargeIntervalManager();
			break;
		case ACT_SCHEDULE_TP_CONSUME_REBATES:
			pkActivity = T_NEW_D ACT_ConsumeRebatesManager();
			break;
		case ACT_SCHEDULE_TP_SIGNIN:
			pkActivity = T_NEW_D ACT_SignInManager();
			break;
		case ACT_SCHEDULE_TP_CHARGE_REBATES:
			pkActivity = T_NEW_D ACT_ChargeRebatesManager();
			break;
		case ACT_SCHEDULE_TP_OFFER_A_REWARD:
			pkActivity = T_NEW_D ACT_OfferARewardManager();
			break;
		case ACT_SCHEDULE_TP_MONOPOLY:
			pkActivity = T_NEW_D ACT_MonopolyManager();
			break;
		case ACT_SCHEDULE_TP_SNOW_MAN:
			pkActivity = T_NEW_D ACT_SnowManManager();
			break;
		case ACT_SCHEDULE_TP_CLIMB_TOWER:
			pkActivity = T_NEW_D ACT_ClimbTowerManager();
			break;
		case ACT_SCHEDULE_TP_SPRING_TREE:
			pkActivity = T_NEW_D ACT_SpringTreeManager();
			break;
		case ACT_SCHEDULE_TP_REFRSH_SHOP:
			pkActivity = T_NEW_D ACT_RefreshShopManager();
			break;
		case ACT_SCHEDULE_TP_SUBORDINATE_CONSUME:
			pkActivity = T_NEW_D ACT_SubordinateConsumeManager();
			break;
		case ACT_SCHEDULE_TP_LOTTERY:
			pkActivity = T_NEW_D ACT_LotteryManager();
			break;
		case ACT_SCHEDULE_TP_TIMELIMITED_SHOP:
			pkActivity = T_NEW_D ACT_TimeLimitedShop();
			break;
		case ACT_SCHEDULE_TP_SUBORDINATE_B:
			pkActivity = T_NEW_D ACT_SubordinateManagerB();
			break;
		case ACT_SCHEDULE_TP_SHOP_B:
			pkActivity = T_NEW_D ACT_ShopManagerB();
			break;
		case ACT_SCHEDULE_TP_PINTU:
			pkActivity = T_NEW_D ACT_PicturePuzzleManager();
			break;
		default:
			break;
		}
		_addActivity(pkActivity);
	}
	InitConfig();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ACT_ScheduleDataManager::Unit(void)
{
	ACT_ScheduleDataActivityMap::iterator it = m_kActivities.begin();
	for (; it!=m_kActivities.end(); ++it)
	{
		T_SAFE_DELETE(it->second);
	}
	m_kActivities.clear();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ResultType ACT_ScheduleDataManager::InitConfig(CSVFile::CSVDataLoader* pkLoader, const CSVFile::CSVDataSourceList* pkCSVs)
{
	ACT_ScheduleDataActivityMap::iterator it = m_kActivities.begin();
	for (; it!=m_kActivities.end(); ++it)
	{
		if (NULL == it->second)
		{
			continue;
		}
		it->second->InitConfig(pkLoader, pkCSVs);
	}
	return RE_SUCCESS;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ResultType ACT_ScheduleDataManager::CheckData(LC_ServerPlayer* pkPlayer)
{
	ACT_ScheduleDataActivityMap::iterator it = m_kActivities.begin();
	for (; it!=m_kActivities.end(); ++it)
	{
		it->second->CheckData(pkPlayer);
	}
	return RE_SUCCESS;
}

void ACT_ScheduleDataManager::Update(float fCurrentTime)
{
	ACT_ScheduleDataActivityMap::iterator iter = m_kActivities.begin();
	for (; iter != m_kActivities.end(); ++iter)
	{
		iter->second->Update(fCurrentTime);
	}
}
/////////////////////////////////////////////////////////////////
bool ACT_ScheduleDataManager::ActBegin( int nSchedID,  time_t nTimeStamp )
{
	ACT_SubordinateManager* pkActivity = (ACT_SubordinateManager*)GetActivity(ACT_SCHEDULE_TP_SUBORDINATE);
	if (pkActivity)
	{
		CF_ACTSubordinateRewardA::DataEntryMapExternIterator kIter = CF_ACTSubordinateRewardA::GetSingletonPtr()->GetIterator();
		while (kIter.HasMoreElements())
		{
			const CF_ACTSubordinateRewardA::DataEntry* pkEntry = kIter.PeekNextValuePtr();
			if(pkEntry->_iSchedule == nSchedID)
			{
				if(pkEntry->_iType == ACT_BOSS_FIRST_KILL_TYPE)
				{
					CF_BossFirstKill::DataEntryMapExternIterator it = CF_BossFirstKill::GetSingletonPtr()->GetIterator();
					while (it.HasMoreElements())
					{
						const CF_BossFirstKill::DataEntry* pkData = it.PeekNextValuePtr();
						mem::vector<uint64_t> input;
						input.push_back(nSchedID);
						input.push_back(pkData->_CharTypeID);
						UT_ServerHelper::RedisCreateData(UT_REDIS_KEY_BOSS_FIRST_KILL, input);
						it.MoveNext();
					}					
				}
				else
				{
					//活动开始的时候，先从redis拿数据，拿不到，存下pkEntry->_iID，gs和society的连接建立好后，通知society在redis上创建数据
					int count = 0;
					mem::vector<uint64_t> input;
					input.push_back(pkEntry->_iID);
					bool tag = UT_ServerHelper::RedisGetData_Int_EntryServerID(UT_REDIS_KEY_ACTIVITY_LIMIT_REWARD, input, count);
					if(tag)
					{
						UT_ServerHelper::UpdateLimitRewardRecord(pkEntry->_iID, count);
					}
					else
					{
						UT_ServerHelper::AddLimitActivityID(pkEntry->_iID);
					}
				}
			}
			kIter.MoveNext();
		}		
	}

	return true;
}

bool ACT_ScheduleDataManager::ActBeginB(int nSchedID, int serverID, time_t nTimeStamp)
{
	bool res = false;
	ACT_SubordinateManagerB* pkActivity = (ACT_SubordinateManagerB*)GetActivity(ACT_SCHEDULE_TP_SUBORDINATE_B);
	if (NULL == pkActivity)
	{
		return res;
	}
	
	const CActSubordinateRewardManager::ACTSubordinateRewardEntryPtrs* entryPtrs = CActSubordinateRewardManager::GetSingletonPtr()->GetActSubordinateRewardEntrysByScheduleID(nSchedID);
	if(NULL == entryPtrs)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "ActBeginB not find scheduleID! scheduleID:%d\n", nSchedID);
		return res;
	}

	for(CActSubordinateRewardManager::ACTSubordinateRewardEntryPtrs::const_iterator it=(*entryPtrs).begin(); it!=(*entryPtrs).end(); ++it)
	{
		const CF_ACTSubordinateRewardB::DataEntry* pkEntry = *it;
		if(NULL == pkEntry)
		{
			continue;
		}
		
		//活动有效时间
		mem::vector<uint64_t> input;
		input.push_back(nSchedID);
		input.push_back(pkEntry->_iType);

		uint32_t currentEndTime = nTimeStamp/1000;
		int32_t oldEndTime = 0;
		char str1[100] = ":sub_b_info_endtime:";
		const char* szNameSpace1 = UT_ServerHelper::GetRedisNameSpace(str1, pkEntry->_GroupID);
		bool tag = UT_ServerHelper::RedisGetData_Int(szNameSpace1, input, oldEndTime);
		if(!tag)
		{
			UT_ServerHelper::RedisCreateData(szNameSpace1, input);
		}
		if(currentEndTime != oldEndTime)
		{
			UT_ServerHelper::RedisSetData_Int(szNameSpace1, input, currentEndTime);	
		}
		UT_ServerHelper::SetSubBEndTime(pkEntry->_GroupID, currentEndTime);

		//活动奖励领取次数
		//活动开始的时候，先从redis拿数据，拿不到，存下pkEntry->_iID，gs和society的连接建立好后，通知society在redis上创建数据
		int count = 0;
		mem::vector<uint64_t> key;
		key.push_back(pkEntry->_iID);
		tag = UT_ServerHelper::RedisGetData_Int_EntryServerID(UT_REDIS_KEY_ACTIVITY_LIMIT_REWARD_SUB_B, key, count);
		if(tag)
		{
			UT_ServerHelper::UpdateLimitRewardRecordB(pkEntry->_iID, count);
		}
		else
		{
			UT_ServerHelper::AddLimitActivityIDB(pkEntry->_iID);
		}

		//活动结束时间
		static ACT_ScheduleManager* pkScheduleMgr = ACT_ScheduleManager::GetSingletonPtr();
		ACT_ScheduleEntry* pkScheduleEntry = pkScheduleMgr->GetSchedule(serverID, pkEntry->_iFinishSchedule);
		if (NULL != pkScheduleEntry)
		{
			mem::vector<uint64_t> input;
			input.push_back(pkEntry->_iFinishSchedule);
			input.push_back(pkEntry->_iType);

			char str1[100] = ":sub_b_info_cleartime:";
			const char* szNameSpace1 = UT_ServerHelper::GetRedisNameSpace(str1, pkEntry->_GroupID);
			uint32_t currentEndTime = pkScheduleEntry->m_kSchedule.m_iEndTime/1000;
			int32_t oldEndTime = 0;
			bool tag = UT_ServerHelper::RedisGetData_Int(szNameSpace1, input, oldEndTime);
			if(!tag)
			{
				UT_ServerHelper::RedisCreateData(szNameSpace1, input);
			}
		
			if(currentEndTime != oldEndTime)
			{
				UT_ServerHelper::RedisSetData_Int(szNameSpace1, input, currentEndTime);	
			}
			UT_ServerHelper::SetSubBClearTime(pkEntry->_GroupID, currentEndTime);
		}
	}

	return true;
}

bool ACT_ScheduleDataManager::ActGetB(int nSchedID, time_t nTimeStamp, mem::map<int, int>& resultSub)
{
	ACT_SubordinateManagerB* pkActivity = (ACT_SubordinateManagerB*)GetActivity(ACT_SCHEDULE_TP_SUBORDINATE_B);
	if (NULL == pkActivity)
	{
		return false;
	}

	const CActSubordinateRewardManager::ACTSubordinateRewardEntryPtrs* entryPtrs = CActSubordinateRewardManager::GetSingletonPtr()->GetActSubordinateRewardEntrysByScheduleID(nSchedID);
	if(NULL == entryPtrs)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "ActGetB not find scheduleID! scheduleID:%d\n", nSchedID);
		return false;
	}

	for(CActSubordinateRewardManager::ACTSubordinateRewardEntryPtrs::const_iterator it=(*entryPtrs).begin(); it!=(*entryPtrs).end(); ++it)
	{
		const CF_ACTSubordinateRewardB::DataEntry* pkEntry = *it;
		if(NULL == pkEntry)
		{
			continue;
		}
		
		//Get直接从redis读数据
		int count = 0;
		mem::vector<uint64_t> input;
		input.push_back(pkEntry->_iID);
		UT_ServerHelper::RedisGetData_Int_EntryServerID(UT_REDIS_KEY_ACTIVITY_LIMIT_REWARD_SUB_B, input, count);
		UT_ServerHelper::UpdateLimitRewardRecordB(pkEntry->_iID, count);
		resultSub.insert(std::make_pair(pkEntry->_iID, count));
	}

	return true;
}

bool ACT_ScheduleDataManager::ActGet(int nSchedID, time_t nTimeStamp, mem::map<int, int>& resultSub, mem::map<int, BossFirstKillPlayerInfo>& resultBoss)
{
	ACT_SubordinateManager* pkActivity = (ACT_SubordinateManager*)GetActivity(ACT_SCHEDULE_TP_SUBORDINATE);
	if (pkActivity)
	{
		CF_ACTSubordinateRewardA::DataEntryMapExternIterator kIter = CF_ACTSubordinateRewardA::GetSingletonPtr()->GetIterator();
		for(; kIter.HasMoreElements(); kIter.MoveNext())
		{
			const CF_ACTSubordinateRewardA::DataEntry* pkEntry = kIter.PeekNextValuePtr();
			if(pkEntry->_iSchedule == nSchedID)
			{
				if(pkEntry->_iType != ACT_BOSS_FIRST_KILL_TYPE)
				{
					//Get直接从redis读数据
					int count = 0;
					mem::vector<uint64_t> input;
					input.push_back(pkEntry->_iID);
					UT_ServerHelper::RedisGetData_Int_EntryServerID(UT_REDIS_KEY_ACTIVITY_LIMIT_REWARD, input, count);
					UT_ServerHelper::UpdateLimitRewardRecord(pkEntry->_iID, count);
					resultSub.insert(std::make_pair(pkEntry->_iID, count));
				}
				else
				{
					CF_BossFirstKill::DataEntryMapExternIterator it = CF_BossFirstKill::GetSingletonPtr()->GetIterator();
					for(; it.HasMoreElements(); it.MoveNext())
					{
						std::string str;
						const CF_BossFirstKill::DataEntry* pkData = it.PeekNextValuePtr();
						mem::vector<uint64_t> input;
						input.push_back(nSchedID);
						input.push_back(pkData->_CharTypeID);
						bool tag = UT_ServerHelper::RedisGetData_String(UT_REDIS_KEY_BOSS_FIRST_KILL, input, str);
						if(tag)
						{
							BossFirstKillPlayerInfo info;
							std::string errorMsg;
							if(boost::ajson::load_from_buff(info, str.c_str(), errorMsg))
							{
								resultBoss.insert(std::make_pair(pkData->_CharTypeID, info));
							}
						}
					}					
				}
			}
		}
	}
	return true;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ACT_ScheduleDataManager::OnSubUpgrade(LC_ServerPlayer* pkPlayer, int32_t iSubType, int32_t iLevel, const ACT_Type2CatasMap* pkActivitys)
{
	{
		ACT_SubordinateManager* pkActivity = (ACT_SubordinateManager*)GetActivity(ACT_SCHEDULE_TP_SUBORDINATE);
		if (pkActivity)
		{
			pkActivity->OnSubUpgrade(pkPlayer, iSubType, iLevel, pkActivitys?GetActCatagorys(*pkActivitys, ACT_SCHEDULE_TP_SUBORDINATE) : NULL);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ACT_ScheduleDataManager::OnCharge(LC_ServerPlayer* pkPlayer, uint32_t lValue, const ACT_Type2CatasMap* pkActivitys)
{
	//{
	//	ACT_ChargeProfitManager* pkActivity = (ACT_ChargeProfitManager*)GetActivity(ACT_SCHEDULE_TP_CHARGE);
	//	if (pkActivity)
	//	{
	//		pkActivity->OnCharge(pkPlayer, lValue, pkActivitys?GetActCatagorys(*pkActivitys, ACT_SCHEDULE_TP_CHARGE) : NULL);
	//	}
	//}
	////以下这些活动策划说暂时没用
	//{
	//	ACT_ContinuousChargeManager* pkActivity = (ACT_ContinuousChargeManager*)GetActivity(ACT_SCHEDULE_TP_CONTINUOUS_CHARGE);
	//	if (pkActivity)
	//	{
	//		pkActivity->OnCharge(pkPlayer, lValue, pkActivitys?GetActCatagorys(*pkActivitys, ACT_SCHEDULE_TP_CONTINUOUS_CHARGE) : NULL);
	//	}
	//}
	//{
	//	ACT_ShopManager* pkActivity = (ACT_ShopManager*)GetActivity(ACT_SCHEDULE_TP_SHOP);
	//	if (pkActivity)
	//	{
	//		pkActivity->OnCharge(pkPlayer, lValue, pkActivitys?GetActCatagorys(*pkActivitys, ACT_SCHEDULE_TP_SHOP) : NULL);
	//	}
	//}
	//{
	//	ACT_ShotManager *pkActivity = (ACT_ShotManager*)GetActivity(ACT_SCHEDULE_TP_SHOT);
	//	if (pkActivity)
	//	{
	//		pkActivity->OnCharge(pkPlayer, lValue, pkActivitys?GetActCatagorys(*pkActivitys, ACT_SCHEDULE_TP_SHOT) : NULL);
	//	}
	//}
	//{
	//	ACT_ChargeIntervalManager* pkActivity = (ACT_ChargeIntervalManager*)GetActivity(ACT_SCHEDULE_TP_CHARGE_INTERVAL);
	//	if (pkActivity)
	//	{
	//		pkActivity->OnCharge(pkPlayer, lValue, pkActivitys?GetActCatagorys(*pkActivitys, ACT_SCHEDULE_TP_CHARGE_INTERVAL) : NULL);
	//	}
	//}
	//{
	//	ACT_ChargeRebatesManager* pkActivity = (ACT_ChargeRebatesManager*)GetActivity(ACT_SCHEDULE_TP_CHARGE_REBATES);
	//	if (pkActivity)
	//	{
	//		pkActivity->OnCharge(pkPlayer, lValue, pkActivitys?GetActCatagorys(*pkActivitys, ACT_SCHEDULE_TP_CHARGE_REBATES) : NULL);
	//	}
	//}
	//{
	//	ACT_OfferARewardManager* pkActivity = (ACT_OfferARewardManager*)GetActivity(ACT_SCHEDULE_TP_OFFER_A_REWARD);
	//	if (pkActivity)
	//	{
	//		pkActivity->OnCharge(pkPlayer, lValue, pkActivitys?GetActCatagorys(*pkActivitys, ACT_SCHEDULE_TP_OFFER_A_REWARD) : NULL);
	//	}
	//}
	//{
	//	ACT_SpringTreeManager* pkActivity = (ACT_SpringTreeManager*)GetActivity(ACT_SCHEDULE_TP_SPRING_TREE);
	//	if (pkActivity)
	//	{
	//		pkActivity->OnCharge(pkPlayer, lValue, pkActivitys ? GetActCatagorys(*pkActivitys, ACT_SCHEDULE_TP_SPRING_TREE) : NULL);
	//	}
	//}
	//{
	//	ACT_SubordinateConsumeManager* pkActivity = (ACT_SubordinateConsumeManager*)GetActivity(ACT_SCHEDULE_TP_SUBORDINATE_CONSUME);
	//	if (pkActivity)
	//	{
	//		pkActivity->OnCharge(pkPlayer, lValue, pkActivitys ? GetActCatagorys(*pkActivitys, ACT_SCHEDULE_TP_SUBORDINATE_CONSUME) : NULL);
	//	}
	//}
	//{
	//	ACT_LotteryManager* pkActivity = (ACT_LotteryManager*)GetActivity(ACT_SCHEDULE_TP_LOTTERY);
	//	if (pkActivity)
	//	{
	//		pkActivity->OnCharge(pkPlayer, lValue, pkActivitys ? GetActCatagorys(*pkActivitys, ACT_SCHEDULE_TP_LOTTERY) : NULL);
	//	}
	//}
	//{
	//	ACT_TimeLimitedShop* pkActivity = (ACT_TimeLimitedShop*)GetActivity(ACT_SCHEDULE_TP_TIMELIMITED_SHOP);
	//	if (pkActivity)
	//	{
	//		pkActivity->OnCharge(pkPlayer, lValue, pkActivitys ? GetActCatagorys(*pkActivitys, ACT_SCHEDULE_TP_TIMELIMITED_SHOP) : NULL);
	//	}
	//}
}

//充值活动的新接口
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ACT_ScheduleDataManager::NewOnCharge(LC_ServerPlayer* pkPlayer,uint32_t nValueType, uint32_t lValue, const ACT_Type2CatasMap* pkActivitys )
{
	
	{
		//每日充值
		ACT_ChargeProfitManager* pkActivity = (ACT_ChargeProfitManager*)GetActivity(ACT_SCHEDULE_TP_CHARGE);
		if (pkActivity)
		{
			pkActivity->NewOnCharge(pkPlayer, nValueType, lValue, pkActivitys?GetActCatagorys(*pkActivitys, ACT_SCHEDULE_TP_CHARGE) : NULL);
		}
	}
	
	{
		//连充返利
		ACT_ChargeRebatesManager* pkActivity = (ACT_ChargeRebatesManager*)GetActivity(ACT_SCHEDULE_TP_CHARGE_REBATES);
		if (pkActivity)
		{
			pkActivity->NewOnCharge(pkPlayer, nValueType, lValue, pkActivitys?GetActCatagorys(*pkActivitys, ACT_SCHEDULE_TP_CHARGE_REBATES) : NULL);
		}
	}
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ACT_ScheduleDataManager::ACTScheduleEnd(uint32_t iSchedule, LC_ServerPlayer* pkPlayer)
{
	{
		ACT_ChargeProfitManager* pkActivity = (ACT_ChargeProfitManager*)GetActivity(ACT_SCHEDULE_TP_CHARGE);
		if (pkActivity)
		{
			pkActivity->ACTScheduleEnd(iSchedule, pkPlayer);
		}
	}
	{
		ACT_ChargeRebatesManager* pkActivity = (ACT_ChargeRebatesManager*)GetActivity(ACT_SCHEDULE_TP_CHARGE_REBATES);
		if (pkActivity)
		{
			pkActivity->ACTScheduleEnd(iSchedule, pkPlayer);
		}
	}
	{
		ACT_PicturePuzzleManager* pkActivity = (ACT_PicturePuzzleManager*)GetActivity(ACT_SCHEDULE_TP_PINTU);
		if (pkActivity)
		{
			pkActivity->ACTScheduleEnd(iSchedule, pkPlayer);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ACT_ScheduleDataManager::LoginCheckACTScheduleEnd(LC_ServerPlayer* pkPlayer)
{
	{
		ACT_ChargeProfitManager* pkActivity = (ACT_ChargeProfitManager*)GetActivity(ACT_SCHEDULE_TP_CHARGE);
		if (pkActivity)
		{
			pkActivity->LoginCheckACTScheduleEnd(pkPlayer);
		}
	}
	{
		ACT_ChargeRebatesManager* pkActivity = (ACT_ChargeRebatesManager*)GetActivity(ACT_SCHEDULE_TP_CHARGE_REBATES);
		if (pkActivity)
		{
			pkActivity->LoginCheckACTScheduleEnd(pkPlayer);
		}
	}
	{
		ACT_PicturePuzzleManager* pkActivity = (ACT_PicturePuzzleManager*)GetActivity(ACT_SCHEDULE_TP_PINTU);
		if (pkActivity)
		{
			pkActivity->LoginCheckACTScheduleEnd(pkPlayer);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ACT_ScheduleDataManager::OnConsume(LC_ServerPlayer* pkPlayer, uint32_t lValue, const ACT_Type2CatasMap* pkActivitys)
{
	{
		ACT_ConsumeProfitManager* pkActivity = (ACT_ConsumeProfitManager*)GetActivity(ACT_SCHEDULE_TP_CONSUME);
		if (pkActivity)
		{
			pkActivity->OnConsume(pkPlayer, lValue, pkActivitys?GetActCatagorys(*pkActivitys, ACT_SCHEDULE_TP_CONSUME) : NULL);
		}
	}
	{
		ACT_ShotManager *pkActivity = (ACT_ShotManager*)GetActivity(ACT_SCHEDULE_TP_SHOT);
		if (pkActivity)
		{
			pkActivity->OnConsume(pkPlayer, lValue, pkActivitys?GetActCatagorys(*pkActivitys, ACT_SCHEDULE_TP_SHOT) : NULL);
		}
	}
	{
		ACT_ConsumeRebatesManager *pkActivity = (ACT_ConsumeRebatesManager*)GetActivity(ACT_SCHEDULE_TP_CONSUME_REBATES);
		if (pkActivity)
		{
			pkActivity->OnConsume(pkPlayer, lValue, pkActivitys?GetActCatagorys(*pkActivitys, ACT_SCHEDULE_TP_CONSUME_REBATES) : NULL);
		}
	}
	{
		ACT_SpringTreeManager* pkActivity = (ACT_SpringTreeManager*)GetActivity(ACT_SCHEDULE_TP_SPRING_TREE);
		if (pkActivity)
		{
			pkActivity->OnConsume(pkPlayer, lValue, pkActivitys ? GetActCatagorys(*pkActivitys, ACT_SCHEDULE_TP_SPRING_TREE) : NULL);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ACT_ScheduleDataManager::OnLogin(LC_ServerPlayer* pkPlayer, uint32_t timestamp, const ACT_Type2CatasMap* pkActivitys)
{
	{
		ACT_LoginManager* pkActivity = (ACT_LoginManager*)GetActivity(ACT_SCHEDULE_TP_LOGIN);
		if (pkActivity)
		{
			pkActivity->OnLogin(pkPlayer, timestamp, pkActivitys?GetActCatagorys(*pkActivitys, ACT_SCHEDULE_TP_LOGIN) : NULL);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ACT_ScheduleDataManager::OnShop(LC_ServerPlayer* pkPlayer, int32_t lScore, const ACT_Type2CatasMap* pkActivitys)
{
	{
		ACT_ShotManager* pkActivity = (ACT_ShotManager*)GetActivity(ACT_SCHEDULE_TP_SHOT);
		if (pkActivity)
		{
			pkActivity->OnShop(pkPlayer, lScore, pkActivitys?GetActCatagorys(*pkActivitys, ACT_SCHEDULE_TP_SHOT) : NULL);
		}
	}
}
void ACT_ScheduleDataManager::OnTaskFinished(LC_ServerPlayer* pkPlayer, GameLogic::LC_Task* pkTask, const ACT_Type2CatasMap* pkActivitys)
{
	{
		ACT_MonopolyManager* pkActivity = (ACT_MonopolyManager*)GetActivity(ACT_SCHEDULE_TP_MONOPOLY);
		if (pkActivity)
		{
			pkActivity->OnTaskFinished(pkPlayer, pkTask, pkActivitys?GetActCatagorys(*pkActivitys, ACT_SCHEDULE_TP_MONOPOLY) : NULL);
		}
	}
	
}

void ACT_ScheduleDataManager::OnFinishTaskAndAchievement(LC_ServerPlayer* pkPlayer, int32_t nType, int32_t nFinishId, const ACT_Type2CatasMap* pkActivitys)
{
	{
		ACT_PicturePuzzleManager* pkActivity = (ACT_PicturePuzzleManager*)GetActivity(ACT_SCHEDULE_TP_PINTU);
		if (pkActivity)
		{
			pkActivity->OnFinishTaskAndAchievement(pkPlayer, nType, nFinishId, pkActivitys?GetActCatagorys(*pkActivitys, ACT_SCHEDULE_TP_PINTU) : NULL);
		}
	}
}
void ACT_ScheduleDataManager::OnOnlineTime(LC_ServerPlayer* pkPlayer, uint32_t lValue, const ACT_Type2CatasMap* pkActivitys)
{
	{
		ACT_SpringTreeManager* pkActivity = (ACT_SpringTreeManager*)GetActivity(ACT_SCHEDULE_TP_SPRING_TREE);
		if (pkActivity)
		{
			pkActivity->OnOnlineTime(pkPlayer, lValue, pkActivitys ? GetActCatagorys(*pkActivitys, ACT_SCHEDULE_TP_SPRING_TREE) : NULL);
		}
	}
}
void ACT_ScheduleDataManager::OnSubordinateConsume(LC_ServerPlayer* pkPlayer, int32_t iSubType, int iPayType, int32_t iValue, const ACT_Type2CatasMap* pkActivitys)
{
	{
		ACT_SubordinateConsumeManager* pkActivity = (ACT_SubordinateConsumeManager*)GetActivity(ACT_SCHEDULE_TP_SUBORDINATE_CONSUME);
		if (pkActivity)
		{
			int32_t iRealCnt = 0;
			if (LC_PAY_WITH_ITEM != iPayType)
			{
				iRealCnt = iValue;				
			}
			else
			{				
				LC_PackAsset* pkPackAsset = pkPlayer->GetPackAsset();
				if (NULL == pkPackAsset)
				{
					return;
				}
				PackOPRecordSet* pkPackOpRecords = pkPackAsset->GetRecordSet();
				if (NULL == pkPackOpRecords || pkPackOpRecords->m_Delete_Items.empty())
				{
					return;
				}
				for (PackOPRecordSet::PackOPRecords::iterator it = pkPackOpRecords->m_Delete_Items.begin(); it != pkPackOpRecords->m_Delete_Items.end(); ++it)
				{
					PackEntryOPRecord& rd = *it;
					iRealCnt += rd.m_lNum;
				}				
			}
			if (iRealCnt > 0)
			{
				pkActivity->OnSubordinateConsume(pkPlayer, iSubType, iRealCnt, pkActivitys ? GetActCatagorys(*pkActivitys, ACT_SCHEDULE_TP_SUBORDINATE_CONSUME) : NULL);
			}			
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ACT_ScheduleDataManager::AddScore(LC_ServerPlayer* pkPlayer, int32_t iType, int32_t iVal, const mem::vector<int32_t>& kCatagorys)
{
	switch (iType)
	{
		case ACT_SCHEDULE_TP_CHARGE:
			{
				ACT_ChargeProfitManager* pkActivity = (ACT_ChargeProfitManager*)GetActivity(ACT_SCHEDULE_TP_CHARGE);
				if (pkActivity)
				{
					pkActivity->AddCharge(pkPlayer, iVal, kCatagorys);
				}
			}break;
		case ACT_SCHEDULE_TP_CONTINUOUS_CHARGE:
			{
				ACT_ContinuousChargeManager* pkActivity = (ACT_ContinuousChargeManager*)GetActivity(ACT_SCHEDULE_TP_CONTINUOUS_CHARGE);
				if (pkActivity)
				{
					pkActivity->AddCharge(pkPlayer, iVal, kCatagorys);
				}
			}break;
		case ACT_SCHEDULE_TP_CONSUME:
			{
				ACT_ConsumeProfitManager* pkActivity = (ACT_ConsumeProfitManager*)GetActivity(ACT_SCHEDULE_TP_CONSUME);
				if (pkActivity)
				{
					pkActivity->AddConsume(pkPlayer, iVal, kCatagorys);
				}
			}break;
		case ACT_SCHEDULE_TP_CHARGE_INTERVAL:
			{
				ACT_ChargeIntervalManager* pkActivity = (ACT_ChargeIntervalManager*)GetActivity(ACT_SCHEDULE_TP_CHARGE_INTERVAL);
				if (pkActivity)
				{
					pkActivity->AddCharge(pkPlayer, iVal, kCatagorys);
				}
			}break;
		case ACT_SCHEDULE_TP_CONSUME_REBATES:
			{
				ACT_ConsumeRebatesManager* pkActivity = (ACT_ConsumeRebatesManager*)GetActivity(ACT_SCHEDULE_TP_CONSUME_REBATES);
				if (pkActivity)
				{
					pkActivity->AddConsume(pkPlayer, iVal, kCatagorys);
				}
			}break;
		case ACT_SCHEDULE_TP_CHARGE_REBATES:
			{
				ACT_ChargeRebatesManager* pkActivity = (ACT_ChargeRebatesManager*)GetActivity(ACT_SCHEDULE_TP_CHARGE_REBATES);
				if (pkActivity)
				{
					pkActivity->AddCharge(pkPlayer, iVal, kCatagorys);
				}
			}break;
		case ACT_SCHEDULE_TP_SUBORDINATE_CONSUME:
			{
				ACT_SubordinateConsumeManager* pkActivity = (ACT_SubordinateConsumeManager*)GetActivity(iType);
				if (pkActivity)
				{
					pkActivity->AddDrawCount(pkPlayer, iVal, kCatagorys);
				}
			}break;
		case ACT_SCHEDULE_TP_SPRING_TREE:
			{
				ACT_SpringTreeManager* pkActivity = (ACT_SpringTreeManager*)GetActivity(iType);
				if (pkActivity)
				{
					pkActivity->AddScore(pkPlayer, iVal, kCatagorys);
				}
			}break;
		case ACT_SCHEDULE_TP_LOTTERY:
			{
				ACT_LotteryManager* pkActivity = (ACT_LotteryManager*)GetActivity(iType);
				if (pkActivity)
				{
					pkActivity->AddCharge(pkPlayer, iVal, kCatagorys);
				}
			}break;
		case ACT_SCHEDULE_TP_OFFER_A_REWARD:
			{
				ACT_OfferARewardManager* pkActivity = (ACT_OfferARewardManager*)GetActivity(iType);
				if(pkActivity)
				{
					pkActivity->AddCharge(pkPlayer, iVal, kCatagorys);
				}
			}break;
		case ACT_SCHEDULE_TP_TIMELIMITED_SHOP:
			{
				ACT_TimeLimitedShop* pkActivity = (ACT_TimeLimitedShop*)GetActivity(iType);
				if(pkActivity)
				{
					pkActivity->AddCharge(pkPlayer, iVal, kCatagorys);
				}
			}
			break;
		default:
			return;
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ACT_ScheduleDataManager::OfferScore(LC_ServerPlayer* pkPlayer, int32_t lScore, const ACT_Type2CatasMap& rkActivitys)
{
	ACT_Type2CatasMap::const_iterator cit = rkActivitys.begin();
	for (; cit!=rkActivitys.end(); ++cit)
	{
		int32_t iType = cit->first;
		ACT_ScheduleDataActivityBase* pkActivity = GetActivity(iType);
		if (NULL == pkActivity)
		{
			continue;
		}
		const GameLogic::LC_IDSet& rkCatagorys = cit->second;
		switch (iType)
		{
		case ACT_SCHEDULE_TP_SUBORDINATE:
			break;
		case ACT_SCHEDULE_TP_CHARGE:
		case ACT_SCHEDULE_TP_CONTINUOUS_CHARGE:
		case ACT_SCHEDULE_TP_CHARGE_INTERVAL:
		case ACT_SCHEDULE_TP_CHARGE_REBATES:
		case ACT_SCHEDULE_TP_SHOP:
		case ACT_SCHEDULE_TP_OFFER_A_REWARD:
			pkActivity->OnCharge(pkPlayer, lScore, &rkCatagorys);
			break;
		case ACT_SCHEDULE_TP_CONSUME:
		case ACT_SCHEDULE_TP_CONSUME_REBATES:
			pkActivity->OnConsume(pkPlayer, lScore, &rkCatagorys);
			break;
		case ACT_SCHEDULE_TP_LOGIN:
			break;
		case ACT_SCHEDULE_TP_SHOT:
			{
				pkActivity->OnConsume(pkPlayer, lScore, &rkCatagorys);
				pkActivity->OnShop(pkPlayer, lScore, &rkCatagorys);
			}
			break;
		case ACT_SCHEDULE_TP_SIGNIN:
			break;
		default:
			break;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ACT_ScheduleDataManager::_addActivity(ACT_ScheduleDataActivityBase* pkActivity)
{
	if (NULL == pkActivity)
	{
		return false;
	}
	ACT_ScheduleDataActivityMap::iterator it = m_kActivities.find(pkActivity->GetType());
	if (it != m_kActivities.end())
	{
		T_SAFE_DELETE(it->second);
		m_kActivities.erase(it);
	}
	m_kActivities[pkActivity->GetType()] = pkActivity;
	return true;
}

struct OnScheduleActiveOper : public GameLogic::LC_ActorOperator
{
	OnScheduleActiveOper(ACT_ScheduleDataActivityBase* act, ACT_ScheduleEntry* schedule, const GameLogic::LC_IDSet& indices)
		:pkActivity(act), pkSchedule(schedule), rkIndices(indices), iServerID(0), uiTimestamp(0)
	{
		iServerID = pkSchedule ? pkSchedule->GetServerID() : 0;
		uiTimestamp = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	}

	virtual bool operator ()(GameLogic::LC_ActorBase* pkActor)
	{
		if (pkActor && IS_PLAYER_ID(pkActor->GetID()))
		{
			static int32_t iUnionID = GetGameUnionID();
			LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkActor;
			ACT_ScheduleAsset& rkScheduleAsset = pkPlayer->GetACTScheduleAsset();
			ACT_ScheduleSaveActivity* pkSaveActivity = rkScheduleAsset.GetScheduleSaveActivity(pkActivity->GetType());
			if (pkSaveActivity && pkSaveActivity->CheckReset(pkSchedule, uiTimestamp, rkIndices))
			{
				rkScheduleAsset.UpdateDirtyFlag();
				return true;
			}
		}
		return false;
	}
	ACT_ScheduleDataActivityBase* pkActivity;
	ACT_ScheduleEntry* pkSchedule;
	const GameLogic::LC_IDSet& rkIndices;
	int32_t iServerID;
	uint32_t uiTimestamp;
};

struct OnSyncScheduleActiveInfo : public GameLogic::LC_ActorOperator
{
	OnSyncScheduleActiveInfo(ACT_ScheduleEntry* schedule, mem::map<int, int>& rewardInfo, mem::map<int, BossFirstKillPlayerInfo>& bossFirstKillInfo):pkSchedule(schedule)
	{
		iServerID = pkSchedule ? pkSchedule->GetServerID() : 0;
		for(mem::map<int, int>::iterator it1=rewardInfo.begin(); it1!=rewardInfo.end(); ++it1)
		{
			m_RewardInfo.insert(std::make_pair(it1->first, it1->second));
		}

		for(mem::map<int, BossFirstKillPlayerInfo>::iterator it2=bossFirstKillInfo.begin(); it2!=bossFirstKillInfo.end(); ++it2)
		{
			m_BossFirstKillInfo.insert(std::make_pair(it2->first, it2->second));
		}
		
	}

	virtual bool operator ()(GameLogic::LC_ActorBase* pkActor)
	{
		if (pkActor && IS_PLAYER_ID(pkActor->GetID()))
		{
			static int32_t iUnionID = GetGameUnionID();
			LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkActor;
			for(mem::map<int, int>::iterator it=m_RewardInfo.begin(); it!=m_RewardInfo.end(); ++it)
			{
				pkPlayer->SyncACTActiveRewardInfo(it->first, it->second);
			}

			for(mem::map<int, BossFirstKillPlayerInfo>::iterator it=m_BossFirstKillInfo.begin(); it!=m_BossFirstKillInfo.end(); ++it)
			{
				pkPlayer->SyncBossFirstKillInfo(it->first, it->second);
			}
		}

		return true;
	}

	ACT_ScheduleEntry* pkSchedule;
	mem::map<int, int> m_RewardInfo;
	mem::map<int, BossFirstKillPlayerInfo> m_BossFirstKillInfo;
	int32_t iServerID;
};

struct OnSyncScheduleActiveInfoB : public GameLogic::LC_ActorOperator
{
	OnSyncScheduleActiveInfoB(ACT_ScheduleEntry* schedule, mem::map<int, int>& rewardInfo):pkSchedule(schedule)
	{
		iServerID = pkSchedule ? pkSchedule->GetServerID() : 0;
		for(mem::map<int, int>::iterator it1=rewardInfo.begin(); it1!=rewardInfo.end(); ++it1)
		{
			m_RewardInfo.insert(std::make_pair(it1->first, it1->second));
		}
	}

	virtual bool operator ()(GameLogic::LC_ActorBase* pkActor)
	{
		if (pkActor && IS_PLAYER_ID(pkActor->GetID()))
		{
			static int32_t iUnionID = GetGameUnionID();
			LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkActor;
			for(mem::map<int, int>::iterator it=m_RewardInfo.begin(); it!=m_RewardInfo.end(); ++it)
			{
				pkPlayer->SyncACTActiveRewardInfoB(it->first, it->second);
			}
		}
		return true;
	}

	ACT_ScheduleEntry* pkSchedule;
	mem::map<int, int> m_RewardInfo;
	int32_t iServerID;
};

struct OnUpdatePlayerSubBInfo : public GameLogic::LC_ActorOperator
{
	OnUpdatePlayerSubBInfo(ACT_ScheduleEntry* schedule):pkSchedule(schedule)
	{
		iServerID = pkSchedule ? pkSchedule->GetServerID() : 0;
	}

	virtual bool operator ()(GameLogic::LC_ActorBase* pkActor)
	{
		if (pkActor && IS_PLAYER_ID(pkActor->GetID()))
		{
			static int32_t iUnionID = GetGameUnionID();
			LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkActor;
			pkPlayer->UpdateSubBInfo(true);
		}
		return true;
	}

	ACT_ScheduleEntry* pkSchedule;
	int32_t iServerID;
};

struct ActivityBestGuild : public GameLogic::LC_ActorOperator
{
	ActivityBestGuild(int32_t endTime, int32_t serverID):m_nEndTime(endTime), m_nServerID(serverID)	{}

	virtual bool operator ()(GameLogic::LC_ActorBase* pkActor)
	{
		if (pkActor && IS_PLAYER_ID(pkActor->GetID()))
		{
			LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkActor;
			pkPlayer->ResetBestGuild(false, m_nEndTime);
		}

		return true;
	}
	int32_t m_nEndTime;
	int32_t m_nServerID;
};

struct AssignRewardTask : public GameLogic::LC_ActorOperator
{
	AssignRewardTask(int32_t endTime, int32_t serverID):m_nEndTime(endTime), m_nServerID(serverID){}

	virtual bool operator ()(GameLogic::LC_ActorBase* pkActor)
	{
		if (pkActor && IS_PLAYER_ID(pkActor->GetID()))
		{
			LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkActor;
			pkPlayer->AssignRewardTask(AssignRewardTaskType_Activity, m_nEndTime);
		}
		return true;
	}

	int32_t m_nEndTime;
	int32_t m_nServerID;
};

struct OnUpdateShopSchedulePlayerInfo : public GameLogic::LC_ActorOperator
{
	OnUpdateShopSchedulePlayerInfo(ACT_ScheduleEntry* schedule, int scheduleID):pkSchedule(schedule), iScheduleID(scheduleID)
	{
		iServerID = pkSchedule ? pkSchedule->GetServerID() : 0;
	}

	virtual bool operator ()(GameLogic::LC_ActorBase* pkActor)
	{
		if (pkActor && IS_PLAYER_ID(pkActor->GetID()))
		{
			static int32_t iUnionID = GetGameUnionID();
			LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkActor;
			pkPlayer->UpdataACTShopInfo(iScheduleID);
		}

		return true;
	}

	ACT_ScheduleEntry* pkSchedule;
	int32_t iScheduleID;
	int32_t iServerID;
};

struct OnFlushBestGuildCounter : public GameLogic::LC_ActorOperator
{
	OnFlushBestGuildCounter(int scheduleID, int serverID) : iScheduleID(scheduleID), iServerID(serverID)
	{
	}

	virtual bool operator ()(GameLogic::LC_ActorBase* pkActor)
	{
		if (pkActor && IS_PLAYER_ID(pkActor->GetID()))
		{
			static int32_t iUnionID = GetGameUnionID();
			LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkActor;
			pkPlayer->FlushBestGuildCounter(iScheduleID);
		}

		return true;
	}

	int32_t iScheduleID;
	int32_t iServerID;
};


struct OnACTEndSendMail : public GameLogic::LC_ActorOperator
{
	OnACTEndSendMail(int scheduleID) : iScheduleID(scheduleID)
	{
	}

	virtual bool operator ()(GameLogic::LC_ActorBase* pkActor)
	{
		if (pkActor && IS_PLAYER_ID(pkActor->GetID()))
		{
			LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkActor;
			pkPlayer->ACTEnd(iScheduleID);
		}

		return true;
	}

	int32_t iScheduleID;
};

void ACT_ScheduleDataManager::_onScheduleActived(Utility::timer_id_type timerID, ACT_ScheduleEntry* pkSchedule)
{
	if (NULL==pkSchedule || NULL==pkSchedule->GetServer())
		return;
	int currentServerId = GetGameServerApp()->GetServerManager()->GetCurServerId();
	uint32_t activityTimerId = pkSchedule->m_iID;
	uint32_t uNewStartTime = (uint32_t)(pkSchedule->CurrentStartTime()/1000);
	uint32_t uNewEndTime = (uint32_t)(pkSchedule->CurrentEndTime()/1000);
	int32_t iStateId = pkSchedule->m_iStateId;
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "_onScheduleActived currentServerId:%d, activityTimerId:%d, uNewStartTime:%d, uNewEndTime:%d , iStateId:%d  \n",currentServerId, activityTimerId, uNewStartTime, uNewEndTime,iStateId);
	//
	
	//
	static ACT_ScheduleManager* pkScheduleMgr = ACT_ScheduleManager::GetSingletonPtr();
	const GameLogic::LC_IDSet* pkHooks = pkScheduleMgr->GetHooks(pkSchedule->m_iID);
	if (pkHooks && !pkHooks->empty())
	{
		GameLogic::LC_IDSet::const_iterator cit = pkHooks->begin();
		for (; cit!=pkHooks->end(); ++cit)
		{
			int32_t iType = *cit;
			ACT_ScheduleDataActivityBase* pkActivity = GetActivity(iType);
			if (pkActivity)
			{
				const GameLogic::LC_IDSet* pkIndices = pkActivity->GetScheduleIndices(pkSchedule->m_iID);
				if (pkIndices && !pkIndices->empty())
				{
					OnScheduleActiveOper kOper(pkActivity, pkSchedule, *pkIndices);
					static LC_ServerWorldManager* pkWorldMgr = SERVER_GET_WORLD_MANAGER();
					pkWorldMgr->ForAllPlayer(&kOper);
				}
			}

			if(iType == ACT_SCHEDULE_TP_SUBORDINATE && pkSchedule->m_bActive && currentServerId==pkSchedule->GetServerID())
			{
				mem::map<int, int> resultSub;
				mem::map<int, BossFirstKillPlayerInfo> resultBoss;
				int64_t timer = pkSchedule->m_kSchedule.m_iStartTime;
				ActBegin(pkSchedule->m_iID, timer);
				ActGet(pkSchedule->m_iID, timer, resultSub, resultBoss);
				OnSyncScheduleActiveInfo info(pkSchedule, resultSub, resultBoss);
				static LC_ServerWorldManager* pkWorldMgr = SERVER_GET_WORLD_MANAGER();
				pkWorldMgr->ForAllPlayer(&info);
			}

			//活动开始，创建全服奖励数量，通知客户端活动奖励剩余数量
			if(iType == ACT_SCHEDULE_TP_SUBORDINATE_B && pkSchedule->m_bActive && currentServerId==pkSchedule->GetServerID())
			{
				mem::map<int, int> resultSub;
				int64_t endTimer = pkSchedule->m_kSchedule.m_iEndTime;
				ActBeginB(pkSchedule->m_iID, currentServerId, endTimer);
				ActGetB(pkSchedule->m_iID, endTimer, resultSub);
				OnSyncScheduleActiveInfoB info(pkSchedule, resultSub);
				static LC_ServerWorldManager* pkWorldMgr = SERVER_GET_WORLD_MANAGER();
				pkWorldMgr->ForAllPlayer(&info);

				OnUpdatePlayerSubBInfo updateSubInfo(pkSchedule);
				pkWorldMgr->ForAllPlayer(&updateSubInfo);
			}
		}
	}

	//普通竞技榜活动开始时，创建redis数据
	if(pkSchedule->m_iID==GetGlobalSetting.ACTSubordinateScheduleID && currentServerId==pkSchedule->GetServerID())
	{
		mem::vector<uint64_t> input;
		input.push_back(pkSchedule->m_iID);
		int res = -1;
		UT_ServerHelper::RedisGetData_Int(UT_REDIS_KEY_ACT_SUBORDINATE_TIME, input, res);

		if(res == -1)
		{
			UT_ServerHelper::RedisCreateData(UT_REDIS_KEY_ACT_SUBORDINATE_TIME, input);
		}
		else
		{
			UT_ServerHelper::SetSubSaveTime(res);
		}
	}

	//进阶竞技榜活动开始时，创建redis数据
	if(pkSchedule->m_iID==GetGlobalSetting.ACTSubordinateScheduleID_Plus && currentServerId==pkSchedule->GetServerID())
	{
		mem::vector<uint64_t> input;
		input.push_back(pkSchedule->m_iID);
		int res = -1;
		UT_ServerHelper::RedisGetData_Int(UT_REDIS_KEY_ACT_SUBORDINATE_TIME_PLUS, input, res);

		if(res == -1)
		{
			UT_ServerHelper::RedisCreateData(UT_REDIS_KEY_ACT_SUBORDINATE_TIME_PLUS, input);
		}
		else
		{
			UT_ServerHelper::SetSubPlusSaveTime(res);
		}
	}

	//最强公会
	if(pkSchedule->m_iID==GetGlobalSetting.BestGuildScheduleID && currentServerId==pkSchedule->GetServerID())
	{
		int64_t endTime = pkSchedule->CurrentEndTime();
		int32_t time = (int32_t)(endTime/1000);

		mem::vector<uint64_t> input;
		input.push_back(pkSchedule->m_iID);
		int res = -1;
		UT_ServerHelper::RedisGetData_Int(UT_REDIS_KEY_BESTGUILD_ENDTIME, input, res);
		if(res == -1)
		{
			UT_ServerHelper::RedisCreateData(UT_REDIS_KEY_BESTGUILD_ENDTIME, input);
		}
		else if(res != time)
		{
			UT_ServerHelper::RedisSetData_Int(UT_REDIS_KEY_BESTGUILD_ENDTIME, input, time);
			/*LC_ServerGuildManager* manager = LC_ServerGuildManager::GetSingletonPtr();
			manager->ResetAllGuildBestGuildTotalAccountPoint();*/
		}
		UT_ServerHelper::SetBestGuildEndTime(time);
		UT_ServerHelper::GameServerAppToSendBestGuildReward1(false);

		
		ActivityBestGuild info(time, currentServerId);
		static LC_ServerWorldManager* pkWorldMgr = SERVER_GET_WORLD_MANAGER();
		pkWorldMgr->ForAllPlayer(&info);
	}

	/*
	if(currentServerId==pkSchedule->GetServerID())
	{
		bool tag = UT_ServerHelper::IsBestGuildTaskScheduleID(pkSchedule->m_iID);
		if(tag)
		{
			int64_t endTime = pkSchedule->CurrentEndTime();
			int32_t time = (int32_t)(endTime/1000);

			AssignBestGuildTask info(GetGameServerID(), pkSchedule->m_iID, time);
			static LC_ServerWorldManager* pkWorldMgr = SERVER_GET_WORLD_MANAGER();
			pkWorldMgr->ForAllPlayer(&info);
		}
	}
	*/

	//悬赏任务
	if(pkSchedule->m_iID==GetGlobalSetting.RewardTaskActivityScheduleID_1 && currentServerId==pkSchedule->GetServerID())
	{
		int64_t endTime = pkSchedule->CurrentEndTime();
		int32_t time = (int32_t)(endTime/1000);

		mem::vector<uint64_t> input;
		input.push_back(pkSchedule->m_iID);
		int res = -1;
		UT_ServerHelper::RedisGetData_Int(UT_REDIS_KEY_REWARD_TASK_ENDTIME, input, res);
		if(res == -1)
		{
			UT_ServerHelper::RedisCreateData(UT_REDIS_KEY_REWARD_TASK_ENDTIME, input);
		}
		else if(res != time)
		{
			UT_ServerHelper::RedisSetData_Int(UT_REDIS_KEY_REWARD_TASK_ENDTIME, input, time);
		}
		UT_ServerHelper::SetRewardTaskEndTime(time);

		AssignRewardTask info(time, currentServerId);
		static LC_ServerWorldManager* pkWorldMgr = SERVER_GET_WORLD_MANAGER();
		pkWorldMgr->ForAllPlayer(&info);
	}


	/*
	2020.9.14 七日试炼修改，活动时间每个玩家不一样
	//没有这段代码会导致：七日试炼活动结束后，CT_TRIAL货币没有被清空
	//2020.07.18 和策划讨论过，七日试炼现在只会开一次，这部分代码先不加，因此：提交代码，但暂时屏蔽
	//七日试炼活动开始时，创建redis数据
	//2020.07.21 这段代码放开，开服的时候，把数据缓存到gs上
	if(pkSchedule->m_iID==GetGlobalSetting.SevenDayTrialRewardScheduleID)
	{
		mem::vector<uint64_t> input;
		input.push_back(pkSchedule->m_iID);
		int res = 0;
		UT_ServerHelper::RedisGetData_Int(UT_REDIS_KEY_ACT_SEVENDAY_TRIAL, input, res);

		int64_t endTime = pkSchedule->CurrentEndTime();
		int32_t time = (int32_t)(endTime/1000);
		if(res == -1)
		{
			UT_ServerHelper::RedisCreateData(UT_REDIS_KEY_ACT_SEVENDAY_TRIAL, input);
		}
		UT_ServerHelper::RedisSetData_Int(UT_REDIS_KEY_ACT_SEVENDAY_TRIAL, input, time);
		UT_ServerHelper::SetSevenDayTrailEndTime(time);
	}
	*/

	if((pkSchedule->m_iID==GetGlobalSetting.UnbindYuanbaoShopScheduleID || pkSchedule->m_iID==GetGlobalSetting.GuildShopScheduleID ||pkSchedule->m_iID==GetGlobalSetting.MomeyCourageScheduleID || pkSchedule->m_iID==GetGlobalSetting.BossJPScheduleID || pkSchedule->m_iID==GetGlobalSetting.LimitScheduleID) 
		&& currentServerId==pkSchedule->GetServerID())
	{
		//获取时间
		mem::vector<uint64_t> input;
		input.push_back(pkSchedule->m_iID);
		int res = -1;
		UT_ServerHelper::RedisGetData_Int(UT_REDIS_KEY_ACT_SHOP_START_TIME, input, res);

		int64_t startTime = pkSchedule->CurrentStartTime();
		int32_t time = (int32_t)(startTime/1000);

		if(res == -1)
		{
			//redis中没数据，第一次开启这个活动
			UT_ServerHelper::RedisCreateData(UT_REDIS_KEY_ACT_SHOP_START_TIME, input);
			UT_ServerHelper::RedisSetData_Int(UT_REDIS_KEY_ACT_SHOP_START_TIME, input, time);
		}
		else if(time != res)
		{
			//更新redis中的活动开启时间
			UT_ServerHelper::RedisSetData_Int(UT_REDIS_KEY_ACT_SHOP_START_TIME, input, time);
		}

		OnUpdateShopSchedulePlayerInfo info(pkSchedule, pkSchedule->m_iID);
		static LC_ServerWorldManager* pkWorldMgr = SERVER_GET_WORLD_MANAGER();
		pkWorldMgr->ForAllPlayer(&info);
	}

	if(currentServerId==pkSchedule->GetServerID())
	{
		//星券商城投资计划活动
		CF_InvestmentPlan::DataEntryMapExternIterator kIter = CF_InvestmentPlan::GetSingletonPtr()->GetIterator();
		for(; kIter.HasMoreElements(); kIter.MoveNext())
		{
			CF_InvestmentPlan::DataEntry* pkData = kIter.PeekNextValuePtr();
			int typeID = pkData->_iType;
			int scheduleID = pkData->_iSchedule;
			if(pkSchedule->m_iID == scheduleID)
			{
				//获取时间
				mem::vector<uint64_t> input;
				input.push_back(pkSchedule->m_iID);
				int res = -1;
				UT_ServerHelper::RedisGetData_Int(UT_REDIS_KEY_INVESTMENTS, input, res);

				int64_t endTime = pkSchedule->CurrentEndTime();
				int32_t time = (int32_t)(endTime/1000);

				if(res == -1)
				{
					//redis中没数据，第一次开启这个活动
					UT_ServerHelper::RedisCreateData(UT_REDIS_KEY_INVESTMENTS, input);
				}
				//更新redis中的活动开启时间
				UT_ServerHelper::RedisSetData_Int(UT_REDIS_KEY_INVESTMENTS, input, time);
				UT_ServerHelper::SetInvestmentEndTime(pkSchedule->m_iID, time);
			}
		}
	}

	if(currentServerId==pkSchedule->GetServerID())
	{
		CF_InvestmentCard::DataEntryMapExternIterator kIt = CF_InvestmentCard::GetSingletonPtr()->GetIterator();
		for(; kIt.HasMoreElements(); kIt.MoveNext())
		{
			CF_InvestmentCard::DataEntry* pkData = kIt.PeekNextValuePtr();
			if(pkSchedule->m_iID == pkData->_iSchedule)
			{
				int32_t startTime = (int32_t)(pkSchedule->CurrentStartTime()/1000);
				int32_t endTime = (int32_t)(pkSchedule->CurrentEndTime()/1000);

				//获取时间
				mem::vector<uint64_t> input;
				uint64_t nKey1 = LC_Helper::BuildKey(pkData->_iType, pkData->_iSchedule, 0, 0);
				input.clear();
				input.push_back(nKey1);
				int startRes = 0;
				UT_ServerHelper::RedisGetData_Int(UT_REDIS_KEY_INVESTMENTS_CARD_STARTTIME, input, startRes);
				if(startRes == 0)
				{
					//redis中没数据，第一次开启这个活动
					UT_ServerHelper::RedisCreateData(UT_REDIS_KEY_INVESTMENTS_CARD_STARTTIME, input);
				}
				//更新redis中的活动开启时间
				UT_ServerHelper::RedisSetData_Int(UT_REDIS_KEY_INVESTMENTS_CARD_STARTTIME, input, startTime);
				UT_ServerHelper::UpdateBattlePassStartTime(nKey1, startTime);

				uint64_t nKey2 = LC_Helper::BuildKey(pkData->_iType, pkData->_iSchedule, startTime, 0);
				input.clear();
				input.push_back(nKey2);
				int endRes = 0;
				UT_ServerHelper::RedisGetData_Int(UT_REDIS_KEY_INVESTMENTS_CARD_ENDTIME, input, endRes);
				if(endRes == 0)
				{
					//redis中没数据，第一次开启这个活动
					UT_ServerHelper::RedisCreateData(UT_REDIS_KEY_INVESTMENTS_CARD_ENDTIME, input);
				}
				//更新redis中的活动开启时间
				UT_ServerHelper::RedisSetData_Int(UT_REDIS_KEY_INVESTMENTS_CARD_ENDTIME, input, endTime);
				UT_ServerHelper::UpdateBattlePassEndTime(nKey2, endTime);
			}
		}
	}

	if(currentServerId==pkSchedule->GetServerID())
	{
		bool tag = LC_Helper::IsBestGuildAchievementScheduleID(pkSchedule->m_iID);
		if(tag)
		{
			OnFlushBestGuildCounter info(pkSchedule->m_iID, currentServerId);
			static LC_ServerWorldManager* pkWorldMgr = SERVER_GET_WORLD_MANAGER();
			pkWorldMgr->ForAllPlayer(&info);
		}
	}
}
void ACT_ScheduleDataManager::_onScheduleDeactived(Utility::timer_id_type timerID, ACT_ScheduleEntry* pkSchedule)
{
	if (NULL==pkSchedule || NULL==pkSchedule->GetServer())
		return;

	OnACTEndSendMail data(pkSchedule->m_iID);
	static LC_PlayerManagerBase* pkPlayerManager = ENGINE_GET_GAMEPLAYER_MANAGER();
	pkPlayerManager->ForAllPlayer(&data);
}