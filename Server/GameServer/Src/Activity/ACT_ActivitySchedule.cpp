#include "ACT_ActivitySchedule.h"

#include "GameServerApp.h"
#include "GameServerManager.h"

#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "LC_ActSubordinateRewardManager.h"

#include "UT_Log.h"
#include "ACT_Subordinate.h"
#include "CF_ACTSubordinateRewardA.h"
#include "CF_BossFirstKill.h"
#include "UT_ServerHelper.h"
#include "GlobalSettings.h"
#include "CF_ACTSubordinateRewardB.h"

using namespace CSVFile;
using namespace Activity;
using namespace Utility;

bool ACT_ScheduleEntry::Init(CSVFile::CF_ActivitySchedule::DataEntry* pkEntry )
{
	if (NULL == pkEntry)
	{
		return false;
	}

	m_iID = pkEntry->_iID;
	m_sCron = pkEntry->_sCron;
	m_sStartTime = pkEntry->_sStartTime;
	m_iTimeLen = pkEntry->_iTimeLen;
	m_kStartCoolTime = pkEntry->_StartCoolTime;
	m_kStartValidTime = pkEntry->_StartValidTime;
	m_kConflateCoolTime = pkEntry->_ConflateCoolTime;
	m_kConflateValidTime = pkEntry->_ConflateValidTime;
	m_kValidServers = pkEntry->_kValidServers;
	m_kExceptServers = pkEntry->_kExceptServers;	
	m_iTimeType = pkEntry->_iTimeType;
	m_iStateId = pkEntry->_iStateId;
	return true;
}

void ACT_ScheduleEntry::Unit()
{
	m_kSchedule.Clear();
}
int32_t ACT_ScheduleEntry::GetServerID(void)
{
	return m_pkServer ? m_pkServer->ServerID() : 0;
}
void ACT_ScheduleEntry::SetUp(uint32_t starttime, uint32_t conflatetime)
{
	m_kSchedule.Clear();
	Utility::UT_TimePeriod kStart;
	
	int64_t iStartCoolTime = (int64_t)m_kStartCoolTime.GetTimeStramp(starttime)*ONE_SECOND_MILI;
	int64_t iStartValidTime = m_kStartValidTime.GetTimeStramp()>0 ? (int64_t)m_kStartValidTime.GetTimeStramp(starttime)*ONE_SECOND_MILI : ST_MAX_TIMESTAMP_MILI;
	kStart.Init(iStartCoolTime, iStartValidTime);

	if (m_kConflateCoolTime.GetTimeStramp() || m_kConflateValidTime.GetTimeStramp())
	{
		Utility::UT_TimePeriod kConflate;
		if (conflatetime)
		{
			kConflate.Init(m_kConflateCoolTime.GetTimeStramp(conflatetime)*ONE_SECOND_MILI, m_kConflateValidTime.GetTimeStramp(conflatetime)*ONE_SECOND_MILI);
		}
		else
		{
			//end should not be 0!
			kConflate.Init(0, 1);
		}
		m_kSchedule.SetPeriod(Merge(kStart, kConflate));
	}
	else
	{
		m_kSchedule.SetPeriod(kStart);
	}

	m_kSchedule.m_sCronStr = m_sCron;
	m_kSchedule.m_sStartTime = m_sStartTime;
	m_kSchedule.m_iTimeLen = (int64_t)m_iTimeLen * ONE_MINUTE_SECONDS * ONE_SECOND_MILI;
}

void ACT_ScheduleEntry::Rechedule(timer_id_type timerID)
{
	static UT_TimerManager* pkTimerMgr = UT_TimerManager::GetSingletonPtr();
	if (m_bActive)
	{
		return;
	}

	if (m_kSchedule.IsValid())
	{
		int64_t curtimestamp = (int64_t)GET_CURRENT_TIMESTAMP_IN_MILISECONDS();
		Utility::UT_TimePeriod& rkPeriod = m_kSchedule.m_Period;
		if (curtimestamp >= rkPeriod.m_iEnd)
		{
			m_kSchedule.ResoveAsLastTime();
		}
		else if (curtimestamp < rkPeriod.m_iStart)
		{
			m_kSchedule.m_iScheduleTimer = pkTimerMgr->DelayCall(rkPeriod.m_iStart-curtimestamp, ACT_ScheduleManager::_rescheduleCB, (ActivityScheduleCallback)NULL, GetServerID(), m_iID);
			m_kSchedule.ResoveAsFirstTime();
		}
		else
		{
			if(!m_kSchedule.m_sCronStr.empty() && m_kSchedule.m_sStartTime.empty())
			{
				m_kSchedule.m_iScheduleTimer = pkTimerMgr->SetCron(m_kSchedule.m_sCronStr, m_kSchedule.m_iTimeLen, ACT_ScheduleManager::_activeCB, GetServerID(), m_iID);
			}
			else if(m_kSchedule.m_sCronStr.empty() && !m_kSchedule.m_sStartTime.empty())
			{
				time_t time = LC_Helper::DateTimeToTimeStamp(m_kSchedule.m_sStartTime.c_str());
				int64_t startTime = time*ONE_MILISECOND_MICRO;
 				m_kSchedule.m_iScheduleTimer = pkTimerMgr->SetStartTime(startTime, m_kSchedule.m_iTimeLen, ACT_ScheduleManager::_activeCB, GetServerID(), m_iID);
			}
			else
			{
				return;
			}

			m_kSchedule.ResolveTimeInfo();
			if (!IsTimerIDValid(m_kSchedule.m_iScheduleTimer))
			{	
				ACT_ScheduleManager::_activeCB(Utility::TIMER_ID_NA, GetServerID(), m_iID);
			}
		}
	}	
	//GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "activity schedule re_sch", timerID, GetServerID(), m_iID, m_bActive, m_kSchedule.Desc());
}

void ACT_ScheduleEntry::Active(Utility::timer_id_type timerID)
{
	static UT_TimerManager* pkTimerMgr = UT_TimerManager::GetSingletonPtr();
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "activity schedule active. timeID:%d, serverID:%d, scheduleID:%d, active:%d", timerID, GetServerID(), m_iID, m_bActive);
	if (!m_bActive)
	{
 		m_bActive = true;
  		int64_t curTimeStamp = (int64_t)GET_CURRENT_TIMESTAMP_IN_MILISECONDS();
		m_kSchedule.ResolveTimeInfo();
		int64_t iDelay = m_kSchedule.m_iEndTime-curTimeStamp;

		if(IsTimerIDValid(m_kSchedule.m_iCancelTimer))
		{
			pkTimerMgr->CancelTimer(m_kSchedule.m_iCancelTimer);
			m_kSchedule.m_iCancelTimer = Utility::TIMER_ID_NA;
		}
	
		static ACT_ScheduleDataManager* pkDataMgr = ACT_ScheduleDataManager::GetSingletonPtr();
		pkDataMgr->_onScheduleActived(timerID, this);

		if (m_pkServer)
		{
			m_pkServer->BroadCast(this);
		}
		//last
		if (iDelay < 0)
		{
			Deactive(timerID);
		}
		else
		{
			m_kSchedule.m_iCancelTimer = pkTimerMgr->DelayCall(iDelay, ACT_ScheduleManager::_deactiveCB, (ActivityScheduleCallback)NULL, GetServerID(), m_iID);
		}
	}
}

void ACT_ScheduleEntry::Deactive(Utility::timer_id_type timerID)
{
	static UT_TimerManager* pkTimerMgr = UT_TimerManager::GetSingletonPtr();
	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "activity schedule deactive", timerID, GetServerID(), m_iID, m_bActive, m_kSchedule.Desc());
	if (m_bActive)
	{
		static ACT_ScheduleDataManager* pkDataMgr = ACT_ScheduleDataManager::GetSingletonPtr();

		m_bActive = false;
		
		if(IsTimerIDValid(m_kSchedule.m_iCancelTimer))
		{
			pkTimerMgr->CancelTimer(m_kSchedule.m_iCancelTimer);
			m_kSchedule.m_iCancelTimer = TIMER_ID_NA;

			int currentServerId = GetGameServerApp()->GetServerManager()->GetCurServerId();

			//存储普通竞技榜活动结束的时间
			if(m_iID==GetGlobalSetting.ACTSubordinateScheduleID && currentServerId==GetServerID())
			{
				mem::vector<uint64_t> input;
				input.push_back(m_iID);
				int currentTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();
				UT_ServerHelper::RedisSetData_Int(UT_REDIS_KEY_ACT_SUBORDINATE_TIME, input, currentTime);
				UT_ServerHelper::SetSubSaveTime(currentTime);
			}

			//存储进阶竞技榜活动结束的时间
			if(m_iID==GetGlobalSetting.ACTSubordinateScheduleID_Plus && currentServerId==GetServerID())
			{
				mem::vector<uint64_t> input;
				input.push_back(m_iID);
				int currentTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();
				UT_ServerHelper::RedisSetData_Int(UT_REDIS_KEY_ACT_SUBORDINATE_TIME_PLUS, input, currentTime);
				UT_ServerHelper::SetSubPlusSaveTime(currentTime);
			}

			//活动结束，结算任务排名奖励
			if(GetGlobalSetting.ScheduleID==m_iID && currentServerId==GetServerID())
			{
				UT_ServerHelper::RankReward(Rank_Type_FinishMainTaskID, false);
			}

			static CEndActivityManager* pkMgr = CEndActivityManager::GetSingletonPtr();
			pkMgr->EndActivity(m_iID);
		}

		pkDataMgr->_onScheduleDeactived(timerID, this);

		if (m_pkServer)
		{
			m_pkServer->BroadCast(this);
		}

		if (m_kSchedule.GetNextScheduleTime(timerID) >= PeriodEndTime())
		{
			pkTimerMgr->CancelTimer(m_kSchedule.m_iScheduleTimer);
			m_kSchedule.m_iScheduleTimer = TIMER_ID_NA;
		}
	}
}
int64_t ACT_ScheduleEntry::GetNextScheduleTime(Utility::timer_id_type timerID)
{
	return m_kSchedule.GetNextScheduleTime(timerID);
}
bool ACT_ScheduleEntry::IsServerValid(int32_t iUnionID, int32_t iServerID)
{
	if (!m_kExceptServers.empty())
	{
		Utility::UT_SIMDataList::iterator it = m_kExceptServers.begin();
		for (; it!=m_kExceptServers.end(); ++it)
		{
			UT_SIMDataInfo& rkSvrInfo = *it;
			if (rkSvrInfo.IsServerIn(iUnionID, iServerID))
			{
				return false;
			}
		}
	}

	if (!m_kValidServers.empty())
	{
		Utility::UT_SIMDataList::iterator it = m_kValidServers.begin();
		for (; it!=m_kValidServers.end(); ++it)
		{
			UT_SIMDataInfo& rkSvrInfo = *it;
			if (rkSvrInfo.IsServerIn(iUnionID, iServerID))
			{
				return true;
			}
		}
		return false;
	}
	return true;
}

bool ACT_ScheduleEntry::IsTimestampIn(int64_t timestamp)
{
	return m_kSchedule.IsTimestampIn(timestamp);
}
void ACT_ScheduleEntry::SetDataToPB(msg_define::schedule_entry& info)
{
	info.set_id(m_iID);
	info.set_active(m_bActive);
	info.set_start(m_kSchedule.m_iStartTime);
	info.set_end(m_kSchedule.m_iEndTime);
}
bool ACT_ScheduleServer::Init(const msg_define::server_time_info& rk_svr_time)
{
	if (!IsServerConflicate(rk_svr_time))
	{
		return true;
	}
	Unit();	
	m_kServerTime = rk_svr_time;
	static int32_t iUnionID = GetGameUnionID();
	static CSVFile::CF_ActivitySchedule* inst = SafeGetSingleton<CSVFile::CF_ActivitySchedule>();
	CSVFile::CF_ActivitySchedule::DataEntryMapExternIterator iter = inst->GetIterator();
	while(iter.HasMoreElements())
	{
		CSVFile::CF_ActivitySchedule::DataEntry* entry = iter.PeekNextValuePtr();

		ACT_ScheduleEntry kDataEntry;
		if (kDataEntry.Init(entry) && kDataEntry.IsServerValid(iUnionID, ServerID()))
		{
			AddScheduleEntry(kDataEntry);
		}
		iter.MoveNext();
	}

	ACT_ScheduleEntryMap::iterator it = m_kScheduleMap.begin();
	for (; it!=m_kScheduleMap.end(); ++it)
	{
		it->second.SetUp(StartTime(), ConflateTime());
		it->second.Rechedule(Utility::TIMER_ID_NA);
	}
	return true;
}

void ACT_ScheduleServer::Unit(void)
{
	ACT_ScheduleEntryMap::iterator it = m_kScheduleMap.begin();
	for (; it!=m_kScheduleMap.end(); ++it)
	{
		it->second.Unit();
	}
	m_kScheduleMap.clear();
}
void ACT_ScheduleServer::UpdateShopResetTimes(LC_ServerPlayer* pkPlayer)
{
	static ACT_ScheduleDataManager* pkACTDataMgr = ACT_ScheduleDataManager::GetSingletonPtr();
	ACT_SubordinateManager* pkActivity = (ACT_SubordinateManager*)(pkACTDataMgr->GetActivity(ACT_SCHEDULE_TP_SHOP));
	if(NULL != pkActivity)
	{
		for(ACT_ScheduleEntryMap::iterator it=m_kScheduleMap.begin(); it!=m_kScheduleMap.end(); ++it)
		{
			int scheduleID = it->second.m_iID;
			pkPlayer->UpdataACTShopInfo(scheduleID);
		}
	}
}
void ACT_ScheduleServer::NotifyToPlayerBossFirstKillInfo(LC_ServerPlayer* pkPlayer)
{
	bool tag = false;
	static ACT_ScheduleDataManager* pkACTDataMgr = ACT_ScheduleDataManager::GetSingletonPtr();
	ACT_SubordinateManager* pkActivity = (ACT_SubordinateManager*)(pkACTDataMgr->GetActivity(ACT_SCHEDULE_TP_SUBORDINATE));
	if(NULL != pkActivity)
	{
		for(ACT_ScheduleEntryMap::iterator it=m_kScheduleMap.begin(); it!=m_kScheduleMap.end(); ++it)
		{
			int scheduleID = it->second.m_iID;
			if(scheduleID == GetGlobalSetting.BossFirstKillScheduleID)
			{
				tag = true;
				int timer = it->second.m_kSchedule.m_iStartTime;
				CF_BossFirstKill::DataEntryMapExternIterator kIter = CF_BossFirstKill::GetSingletonPtr()->GetIterator();
				for(; kIter.HasMoreElements(); kIter.MoveNext())
				{
					std::string result;
					const CF_BossFirstKill::DataEntry* pkData = kIter.PeekNextValuePtr();
					mem::vector<uint64_t> input;
					input.push_back(scheduleID);
					input.push_back(pkData->_CharTypeID);
					bool tag = UT_ServerHelper::RedisGetData_String(UT_REDIS_KEY_BOSS_FIRST_KILL, input, result);	
					if(tag)
					{
						BossFirstKillPlayerInfo info;
						if(result == "0")
						{
							pkPlayer->SyncBossFirstKillInfo(pkData->_CharTypeID, info);
						}
						else
						{
							std::string errorMsg;
							if(boost::ajson::load_from_buff(info, result.c_str(), errorMsg))
							{
								pkPlayer->SyncBossFirstKillInfo(pkData->_CharTypeID, info);
							}
						}
					}
				}
			}
		}
	}

	if(tag)
	{
		pkPlayer->SyncKillBossReward();
		pkPlayer->SyncServerFirstKillBossReward();
		//pkPlayer->SyncServeFirstKillBossInfo();
	}
}
void ACT_ScheduleServer::NotifyToPlayerACTRewardInfo(LC_ServerPlayer* pkPlayer)
{
	static ACT_ScheduleDataManager* pkACTDataMgr = ACT_ScheduleDataManager::GetSingletonPtr();
	ACT_SubordinateManager* pkActivity = (ACT_SubordinateManager*)(pkACTDataMgr->GetActivity(ACT_SCHEDULE_TP_SUBORDINATE));
	if(NULL != pkActivity)
	{
		CF_ACTSubordinateRewardA::DataEntryMapExternIterator kIter = CF_ACTSubordinateRewardA::GetSingletonPtr()->GetIterator();
		for(; kIter.HasMoreElements(); kIter.MoveNext())
		{
			const CF_ACTSubordinateRewardA::DataEntry* pkEntry = kIter.PeekNextValuePtr();
			if(pkEntry->_iType != ACT_BOSS_FIRST_KILL_TYPE)
			{
				bool tag = UT_ServerHelper::CheckActivityVaild(pkEntry->_iSchedule);
				if(tag)
				{
					//玩家上线时，奖励的领取数据，从gs的缓存中拿
					int count = UT_ServerHelper::GetLimitRewardRecordByID(pkEntry->_iID);
					pkPlayer->SyncACTActiveRewardInfo(pkEntry->_iID, count);
				}
			}
		}
	}

	pkActivity = (ACT_SubordinateManager*)(pkACTDataMgr->GetActivity(ACT_SCHEDULE_TP_SUBORDINATE_B));
	if(NULL != pkActivity)
	{
		const mem::map<int, CActSubordinateRewardManager::ACTSubordinateRewardEntryPtrs>& entryPtrs = CActSubordinateRewardManager::GetSingletonPtr()->GetActSubordinateScheduleIDDataMap();
		for(mem::map<int, CActSubordinateRewardManager::ACTSubordinateRewardEntryPtrs>::const_iterator it=entryPtrs.begin(); it!=entryPtrs.end(); ++it)
		{
			bool tag = UT_ServerHelper::CheckActivityVaild(it->first);
			if(tag)
			{
				for(CActSubordinateRewardManager::ACTSubordinateRewardEntryPtrs::const_iterator kIt=it->second.begin(); kIt!=it->second.end(); ++kIt)
				{
					const CF_ACTSubordinateRewardB::DataEntry* pkEntry =*kIt;
					if(NULL != pkEntry)
					{
						//玩家上线时，奖励的领取数据，从gs的缓存中拿
						int count = UT_ServerHelper::GetLimitRewardRecordBByID(pkEntry->_iID);
						pkPlayer->SyncACTActiveRewardInfoB(pkEntry->_iID, count);
					}
				}
			}
		}
	}
}
void ACT_ScheduleServer::NotifyToPlayer(LC_ServerPlayer* pkPlayer)
{
	IF_A_NA_PLAYER(pkPlayer){return;}
	MG_PB<msg_define::schedule_info> kNotify;
	SetDataToPB(kNotify.m_value);
	pkPlayer->SendMsgToClient(MGPT_ACTIVITY_SCHEDULE_INFO, &kNotify);
}
void ACT_ScheduleServer::BroadCast(ACT_ScheduleEntry* pkSchedule)
{
	if (NULL == pkSchedule)
	{
		return;
	}
	static LC_ServerPlayerManager* pkPlayerMgr = SERVER_GET_PLAYER_MANAGER();
	MG_PB<msg_define::schedule_info> kNotify;
	msg_define::schedule_entry* pkentry = kNotify.m_value.add_schedules();
	if (pkentry)
	{
		pkSchedule->SetDataToPB(*pkentry);
		LC_SimPlayerFilter kFilter;
		kFilter.SetNation(ServerID(), false);
		pkPlayerMgr->BroadCast(MGPT_ACTIVITY_SCHEDULE_INFO, &kNotify, &kFilter);
	}
}
void ACT_ScheduleServer::SetDataToPB(msg_define::schedule_info& info)
{
	ACT_ScheduleEntryMap::iterator it = m_kScheduleMap.begin();
	for (; it!=m_kScheduleMap.end(); ++it)
	{
		ACT_ScheduleEntry& rkEntry = it->second;
		msg_define::schedule_entry* pkentry = info.add_schedules();
		if (pkentry)
		{
			rkEntry.SetDataToPB(*pkentry);		
		}
	}
}
bool ACT_ScheduleManager::Init(CSVFile::CSVDataLoader* pkLoader, const CSVFile::CSVDataSourceList* pkCSVs)
{
	CSVFile::CF_ActivitySchedule* inst = SafeGetSingleton<CSVFile::CF_ActivitySchedule>();
	if(!IS_VALID_RELATED_CSV(inst, pkCSVs))
	{
		return false;
	}
	
	CleanSchedule();	
	Schedule();
	return true;
}

void ACT_ScheduleManager::Unit(void)
{
	CleanSchedule();
	m_kHookMap.clear();
}
void ACT_ScheduleManager::CleanSchedule(void)
{
	ACT_ScheduleServerMap::iterator it = m_kServerMap.begin();
	for (; it!=m_kServerMap.end(); ++it)
	{
		it->second.Unit();
	}
	m_kServerMap.clear();
}
void ACT_ScheduleManager::Schedule(void)
{
	static GameServerManager* pkServerMgr = GetGameServerApp()->GetServerManager();
	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "activity schedule", pkServerMgr->isLoaded());
	if (!pkServerMgr->isLoaded())
	{
		return;
	}

	GameLogic::LC_IDSet kValidSvrIDs;
	{
		const msg_define::game_init_info& rk_game_init = pkServerMgr->GetGameInitInfo();
		for (int i=0; i<rk_game_init.time_infos_size(); ++i)
		{
			const msg_define::server_time_info& rk_svr_time = rk_game_init.time_infos(i);
			m_kServerMap[rk_svr_time.server_id()].Init(rk_svr_time);

			kValidSvrIDs.insert(rk_svr_time.server_id());
		}
	}

	{
		//clean
		ACT_ScheduleServerMap::iterator it = m_kServerMap.begin();
		while(it != m_kServerMap.end())
		{
			if (kValidSvrIDs.find(it->first) == kValidSvrIDs.end())
			{
				it->second.Unit();
				m_kServerMap.erase(it++);
			}
			else
			{
				++it;
			}
		}
	}
}
bool ACT_ScheduleManager::IsScheduleValid(int32_t iServerID, int32_t iSchedule)
{
	ACT_ScheduleEntry* pkScheduleEntry = GetSchedule(iServerID, iSchedule);
	if (NULL == pkScheduleEntry)
	{
		return false;
	}
	static int32_t iUnionID = GetGameUnionID();
	int currentTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	int startTime = pkScheduleEntry->m_kSchedule.m_iStartTime/1000;
	int endTime = pkScheduleEntry->m_kSchedule.m_iEndTime/1000;
	return ((startTime<=currentTime && endTime>currentTime) || pkScheduleEntry->m_bActive) && pkScheduleEntry->IsServerValid(iUnionID, iServerID);
}
bool ACT_ScheduleManager::IsScheduleValid(LC_ServerPlayer* pkPlayer, int32_t iSchedule)
{
	IF_A_NA_PLAYER(pkPlayer)
	{
		return false;
	}

	ACT_ScheduleEntry* pkScheduleEntry = GetSchedule(pkPlayer->GetServerID(), iSchedule);
	if (NULL == pkScheduleEntry)
	{
		return false;
	}
	static int32_t iUnionID = GetGameUnionID();
	return pkScheduleEntry->m_bActive && pkScheduleEntry->IsServerValid(iUnionID, pkPlayer->GetServerID());
}
void ACT_ScheduleManager::NotifyToPlayer(LC_ServerPlayer* pkPlayer)
{
	IF_A_NA_PLAYER(pkPlayer){return;}

	ACT_ScheduleServer* pkScheduleServer = GetScheduleServer(pkPlayer->GetServerID());
	if (pkScheduleServer)
	{
		pkScheduleServer->NotifyToPlayer(pkPlayer);
	}
}
void ACT_ScheduleManager::NotifyToPlayerACTRewardInfo(LC_ServerPlayer* pkPlayer)
{
	IF_A_NA_PLAYER(pkPlayer){return;}

	ACT_ScheduleServer* pkScheduleServer = GetScheduleServer(pkPlayer->GetServerID());
	if (pkScheduleServer)
	{
		pkScheduleServer->NotifyToPlayerACTRewardInfo(pkPlayer);
	}
}
void ACT_ScheduleManager::NotifyToPlayerBossFirstKillInfo(LC_ServerPlayer* pkPlayer)
{
	IF_A_NA_PLAYER(pkPlayer){return;}

	ACT_ScheduleServer* pkScheduleServer = GetScheduleServer(pkPlayer->GetServerID());
	if (pkScheduleServer)
	{
		pkScheduleServer->NotifyToPlayerBossFirstKillInfo(pkPlayer);
	}
}

void ACT_ScheduleManager::UpdateShopResetTimes(LC_ServerPlayer* pkPlayer)
{
	IF_A_NA_PLAYER(pkPlayer){return;}
	ACT_ScheduleServer* pkScheduleServer = GetScheduleServer(pkPlayer->GetServerID());
	if (pkScheduleServer)
	{
		pkScheduleServer->UpdateShopResetTimes(pkPlayer);
	}	
}

void ACT_ScheduleManager::_rescheduleCB(Utility::timer_id_type timerID, int32_t iServerID, int32_t iID)
{
	static ACT_ScheduleManager* pkSchMgr = ACT_ScheduleManager::GetSingletonPtr();
	ACT_ScheduleEntry* pkSchedule = pkSchMgr->GetSchedule(iServerID, iID);
	if (pkSchedule)
	{
		pkSchedule->Rechedule(timerID);
	}
}
void ACT_ScheduleManager::_activeCB(Utility::timer_id_type timerID, int32_t iServerID, int32_t iID)
{
	static ACT_ScheduleManager* pkSchMgr = ACT_ScheduleManager::GetSingletonPtr();
	ACT_ScheduleEntry* pkSchedule = pkSchMgr->GetSchedule(iServerID, iID);
	if (pkSchedule)
	{
		pkSchedule->Active(timerID);
	}
}
void ACT_ScheduleManager::_deactiveCB(Utility::timer_id_type timerID, int32_t iServerID, int32_t iID)
{
	static ACT_ScheduleManager* pkSchMgr = ACT_ScheduleManager::GetSingletonPtr();
	ACT_ScheduleEntry* pkSchedule = pkSchMgr->GetSchedule(iServerID, iID);
	if (pkSchedule)
	{
		pkSchedule->Deactive(timerID);
	}
}

bool ACT_ScheduleManager::IsScheduleEndValid(int32_t iServerID, int32_t iSchedule)
{
	ACT_ScheduleEntry* pkScheduleEntry = GetSchedule(iServerID, iSchedule);
	if (NULL == pkScheduleEntry)
	{
		return false;
	}
	int currentTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	int endTime = pkScheduleEntry->m_kSchedule.m_iEndTime/1000;
	return  endTime < currentTime;
}