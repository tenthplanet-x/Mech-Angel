#ifndef __ACT_ACTIVITY_SCHEDULE_H__
#define __ACT_ACTIVITY_SCHEDULE_H__

#pragma once

#include "LC_DirtyFlag.h"
#include "LC_Define.h"

#include "UT_Callback.h"
#include "UT_Timer.h"
#include "UT_ComplexData.h"

#include "ACT_ActivityAsset.h"
#include "ACT_ActivityDataManager.h"

#include "char_data.pb.h"
#include "msg.pb.h"

#include "CF_ActivitySchedule.h"




class LC_ServerPlayer;
namespace Activity
{

	class ACT_ScheduleEntry;
	class ACT_ScheduleServer;
	class ACT_ScheduleManager;
	typedef void (*ActivityScheduleCallback)(Utility::timer_id_type, int32_t, int32_t);
	enum ACT_ActivityTimeType
	{
		AATT_Rank_Acitity = 1,
		AATT_Rank_Reset = 2,
	};
	class ACT_ScheduleEntry
	{
	public:
		ACT_ScheduleEntry(void)
			:m_iID(0), m_iTimeLen(0), m_bActive(false), m_pkServer(NULL)
		{}

		~ACT_ScheduleEntry(void) {Unit();}
		
		bool Init(CSVFile::CF_ActivitySchedule::DataEntry* pkEntry);
		void Unit(void);

		inline void AttachTo(ACT_ScheduleServer* pkServer){m_pkServer = pkServer;}
		inline ACT_ScheduleServer* GetServer(void){return m_pkServer;}
		int32_t GetServerID(void);

		void SetUp(uint32_t starttime, uint32_t conflatetime);

		void Rechedule(Utility::timer_id_type timerID);
		void Active(Utility::timer_id_type timerID);
		void Deactive(Utility::timer_id_type timerID);

		int64_t GetNextScheduleTime(Utility::timer_id_type timerID);

		bool IsServerValid(int32_t iUnionID, int32_t iServerID);
		bool IsTimestampIn(int64_t timestamp);

		inline bool GetActive(void) {return m_bActive;}
	
		inline bool IsScheduleValid(void) {return m_kSchedule.IsValid();}

		inline int64_t PeriodStartTime(void) {return m_kSchedule.m_Period.m_iStart;}
		inline int64_t PeriodEndTime(void) {return m_kSchedule.m_Period.m_iEnd;}

		inline int64_t CurrentStartTime(void) {return m_kSchedule.m_iStartTime;}
		inline int64_t CurrentEndTime(void) {return m_kSchedule.m_iEndTime;}

		void SetDataToPB(msg_define::schedule_entry& info);

		int32_t						m_iID;
		StringType					m_sCron;
		StringType					m_sStartTime;
		int64_t						m_iTimeLen;
		CSVFile::StartTime		m_kStartCoolTime;
		CSVFile::StartTime			m_kStartValidTime;
		CSVFile::StartTime			m_kConflateCoolTime;
		CSVFile::StartTime			m_kConflateValidTime;
		Utility::UT_SIMDataList		m_kValidServers;
		Utility::UT_SIMDataList		m_kExceptServers;		
		int32_t						m_iTimeType;
		int32_t						m_iStateId;
		bool						m_bActive;
		Utility::UT_TimerSchedule	m_kSchedule;

		ACT_ScheduleServer*			m_pkServer;
	};
	typedef mem::map<int32_t, ACT_ScheduleEntry> ACT_ScheduleEntryMap;
	typedef mem::map<int32_t, GameLogic::LC_IDSet> ACT_ScheduleHookMap;

	class ACT_ScheduleServer
	{
	public:
		ACT_ScheduleServer(void){}

		bool Init(const msg_define::server_time_info& rk_svr_time);
		void Unit(void);

		void UpdateShopResetTimes(LC_ServerPlayer* pkPlayer);
		void NotifyToPlayerBossFirstKillInfo(LC_ServerPlayer* pkPlayer);
		void NotifyToPlayerACTRewardInfo(LC_ServerPlayer* pkPlayer);
		void NotifyToPlayer(LC_ServerPlayer* pkPlayer);
		void BroadCast(ACT_ScheduleEntry* pkSchedule);

		void SetDataToPB(msg_define::schedule_info& info);

		inline ACT_ScheduleEntry* GetSchedule(int32_t iSchedule)
		{
			ACT_ScheduleEntryMap::iterator it = m_kScheduleMap.find(iSchedule);
			return (it!=m_kScheduleMap.end()) ? &it->second : NULL;
		}

		void  AddScheduleEntry(const ACT_ScheduleEntry& rkEntry)
		{
			m_kScheduleMap[rkEntry.m_iID] = rkEntry;
			m_kScheduleMap[rkEntry.m_iID].AttachTo(this);
		}

		inline int32_t ServerID(void) {return m_kServerTime.server_id();}
		inline uint32_t StartTime(void) {return m_kServerTime.start_time();}
		inline uint32_t ConflateTime(void) {return m_kServerTime.conflate_time();}
		inline bool IsServerConflicate(const msg_define::server_time_info& rkSvrTime)
		{
			return m_kServerTime.server_id()!=rkSvrTime.server_id() 
				|| m_kServerTime.start_time()!=rkSvrTime.start_time() 
				|| m_kServerTime.conflate_time()!=rkSvrTime.conflate_time() ;
		}

		msg_define::server_time_info	m_kServerTime;
		ACT_ScheduleEntryMap			m_kScheduleMap;
	};
	typedef mem::map<int32_t, ACT_ScheduleServer> ACT_ScheduleServerMap;

	class ACT_ScheduleManager : public Memory::MM_BaseObject
		,public Utility::UT_Singleton<ACT_ScheduleManager>
	{
		friend class ACT_ScheduleEntry;
	public:
		ACT_ScheduleManager(void){}
		~ACT_ScheduleManager(void){Unit();}

		bool Init(CSVFile::CSVDataLoader* pkLoader=NULL, const CSVFile::CSVDataSourceList* pkCSVs=NULL);
		void Unit(void);

		void CleanSchedule(void);

		void Schedule(void);

		inline bool RegHook(int32_t iSchedule, int32_t iType)
		{
			return m_kHookMap[iSchedule].insert(iType).second;
		}

		inline const GameLogic::LC_IDSet* GetHooks(int32_t iSchedule)
		{
			ACT_ScheduleHookMap::iterator it = m_kHookMap.find(iSchedule);
			return (it!=m_kHookMap.end()) ? &it->second : NULL;
		}

		inline bool UnregHook(int32_t iSchedule, int32_t iType)
		{
			ACT_ScheduleHookMap::iterator it = m_kHookMap.find(iSchedule);
			return (it!=m_kHookMap.end()) ? it->second.erase(iType)>0 : false;
		}

		bool IsScheduleValid(LC_ServerPlayer* pkPlayer, int32_t iSchedule);
		bool IsScheduleValid(int32_t iServerID, int32_t iSchedule);
		bool IsScheduleEndValid(int32_t iServerID, int32_t iSchedule);
		inline bool IsTimestampIn(int32_t iServerID, int32_t iSchedule, int64_t timestamp)
		{
			ACT_ScheduleEntry* pkSchedule = GetSchedule(iServerID, iSchedule);
			return pkSchedule ? pkSchedule->IsTimestampIn(timestamp) : false;
		}

		inline ACT_ScheduleServer* GetScheduleServer(int32_t iServerID)
		{
			ACT_ScheduleServerMap::iterator it = m_kServerMap.find(iServerID);
			return (it!=m_kServerMap.end()) ? &it->second : NULL;
		}
				
		inline ACT_ScheduleEntry* GetSchedule(int32_t iServerID, int32_t iSchedule)
		{
			ACT_ScheduleServer* pkScheduleServer = GetScheduleServer(iServerID);
			return pkScheduleServer ? pkScheduleServer->GetSchedule(iSchedule) : NULL;
		}

		void NotifyToPlayer(LC_ServerPlayer* pkPlayer);
		void NotifyToPlayerACTRewardInfo(LC_ServerPlayer* pkPlayer);
		void NotifyToPlayerBossFirstKillInfo(LC_ServerPlayer* pkPlayer);
		void UpdateShopResetTimes(LC_ServerPlayer* pkPlayer);
	protected:
		static void _rescheduleCB(Utility::timer_id_type timerID, int32_t iServerID, int32_t iID);
		static void _activeCB(Utility::timer_id_type timerID, int32_t iServerID, int32_t iID);
		static void _deactiveCB(Utility::timer_id_type timerID, int32_t iServerID, int32_t iID);
	protected:
		ACT_ScheduleServerMap	m_kServerMap;
		ACT_ScheduleHookMap		m_kHookMap;
	};

	template <typename DataEntry>
	struct ACT_ScheduleSaveDataOper : public Utility::UT_SimOperator<bool, ACT_ScheduleDataEntry<DataEntry>* >
	{
		ACT_ScheduleSaveDataOper(ACT_ScheduleManager* schedule, LC_ServerPlayer* player, ACT_ScheduleAsset* asset, ACT_ScheduleSaveActivity* act, ACT_ScheduleSaveCatagory* cat)
			:pkScheduleMgr(schedule), pkPlayer(player), pkAsset(asset), pkSaveActivity(act), pkSaveCatagory(cat)
		{}

		ACT_ScheduleManager*			pkScheduleMgr;
		LC_ServerPlayer*				pkPlayer;
		ACT_ScheduleAsset*				pkAsset;
		ACT_ScheduleSaveActivity*		pkSaveActivity;
		ACT_ScheduleSaveCatagory*		pkSaveCatagory;
	};

	template <typename DataEntry>
	struct ACT_ScheduleValidPeekOper : public Utility::UT_SimOperator<bool, ACT_ScheduleDataEntry<DataEntry>* >
	{
		ACT_ScheduleValidPeekOper(ACT_ScheduleManager* schedule, int32_t server, GameLogic::LC_IDVector& scheduleIDs)
			:pkScheduleMgr(schedule), iServerID(server), rkScheduleIDs(scheduleIDs)
		{}
		virtual bool operator ()(ACT_ScheduleDataEntry<DataEntry>* pkSchedule)
		{
			if (pkScheduleMgr->IsScheduleValid(iServerID, pkSchedule->m_iID))
			{
				rkScheduleIDs.push_back(pkSchedule->m_iID);
				return true;
			}
			return false;
		}

		ACT_ScheduleManager*			pkScheduleMgr;
		int32_t							iServerID;
		GameLogic::LC_IDVector&			rkScheduleIDs;
	};
}

#endif //__ACT_ACTIVITY_SCHEDULE_H__