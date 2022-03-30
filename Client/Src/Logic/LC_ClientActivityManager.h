#pragma once

#include "LC_DirtyFlag.h"
#include "LC_Define.h"

#include "msg.pb.h"
#include "UT_Timer.h"
#include "UT_ComplexData.h"

#include "CF_ActivitySchedule.h"
#include "CF_ACTSubordinateReward.h"
#include "CF_ACTSubordinateRewardA.h"
#include "CF_ACTSubordinateRewardB.h"
#include "CF_ACTShop.h"
#include "CF_ACTShopA.h"
#include "CF_ACTShopB.h"
#include "CF_ACTChargeProfit.h"
#include "CF_ACTChargeProfitA.h"
#include "CF_ACTShot.h"
#include "CF_ACTLottery.h"
#include "CF_ACTSubordinateConsume.h"
#include "CF_HolidayActivity_PinTuReward.h"
#include "CF_HolidayActivity_ChargeContinuous.h"
#include "CF_ShotActivityLimitRankRewards.h"
#include "CF_ActivityRankReward.h"

using namespace Activity;
namespace GameLogic
{
	typedef void (*ActivityScheduleCallback)(Utility::timer_id_type, int32_t);
	typedef mem::map<int32_t, uint32_t> ACT_DataMap;
	class ACT_ScheduleSaveCommon
	{
		friend class ACT_ScheduleSaveEntry;
	public:
		ACT_ScheduleSaveCommon(void)
			:m_iID(0)
		{}

		inline void Init(int32_t iID){m_iID = iID;}
		inline void Clear(void){m_kDatas.clear();}

		void Clear(int32_t iBegin, int32_t iEnd);

		inline int32_t	GetID(void) const {return m_iID;}
		inline void		SetValue(int32_t key, uint32_t val){m_kDatas[key] = val;}
		inline void		AddValue(int32_t key, int32_t val){m_kDatas[key] += val;}
		inline uint32_t GetValue(int32_t key){return m_kDatas[key];}

		void GetDataFromPB(const char_data_define::activity_schedule_common& info);
	protected:
		bool _regBit(int32_t iBegin, int32_t iEnd, int32_t iValue);
		bool _isBitReg(int32_t iBegin, int32_t iEnd, int32_t iValue);

		int32_t			m_iID;
		ACT_DataMap		m_kDatas;
	};
	typedef mem::map<int32_t, ACT_ScheduleSaveCommon> ACT_ScheduleSaveCommonMap;

	class ACT_ScheduleSaveEntry : public LC_DirtyFlagAble
	{
	public:
		ACT_ScheduleSaveEntry(void)
			:m_uiTimestamp(0)
		{}

		inline void Init(int32_t iID, uint32_t timestamp)
		{
			m_kSelf.Init(iID);
			m_uiTimestamp = timestamp;
		}

		void Clear(uint32_t timestamp = 0)
		{
			m_uiTimestamp = timestamp;
			m_kSelf.Clear();
			m_kDetails.clear();
		}

		inline int32_t GetID(void) {return m_kSelf.GetID();}

		inline void RegFire(int32_t iValue, int32_t iStatType=GameLogic::LC_ST_TP_FORERVER)
		{
			m_kSelf._regBit(GET_SCHEDULE_KEY(iStatType, ACT_SCHEDULE_FIRE_BEGIN), GET_SCHEDULE_KEY(iStatType, ACT_SCHEDULE_FIRE_END), iValue);
		}
		bool IsFireReg(int32_t iValue, int32_t iStatType=GameLogic::LC_ST_TP_FORERVER)
		{
			return m_kSelf._isBitReg(GET_SCHEDULE_KEY(iStatType, ACT_SCHEDULE_FIRE_BEGIN), GET_SCHEDULE_KEY(iStatType, ACT_SCHEDULE_FIRE_END), iValue);
		}

		inline void RegReward(int32_t iValue, int32_t iStatType=GameLogic::LC_ST_TP_FORERVER)
		{
			m_kSelf._regBit(GET_SCHEDULE_KEY(iStatType, ACT_SCHEDULE_REWARD_BEGIN), GET_SCHEDULE_KEY(iStatType, ACT_SCHEDULE_REWARD_END), iValue);
		}
		bool IsRewardReg(int32_t iValue, int32_t iStatType=GameLogic::LC_ST_TP_FORERVER)
		{
			return m_kSelf._isBitReg(GET_SCHEDULE_KEY(iStatType, ACT_SCHEDULE_REWARD_BEGIN), GET_SCHEDULE_KEY(iStatType, ACT_SCHEDULE_REWARD_END), iValue);
		}

		inline uint32_t GetCount(int32_t iStatType=GameLogic::LC_ST_TP_FORERVER)
		{
			return m_kSelf.GetValue(GET_SCHEDULE_KEY(iStatType, ACT_SCHEDULE_COUNT));
		}

		inline uint32_t GetValue(int32_t key)
		{
			return m_kSelf.GetValue(key);
		}

		inline uint32_t GetTimeStamp() { return m_uiTimestamp; }

		inline ACT_ScheduleSaveCommon* GetScheduleSaveDetail(int32_t iDetail)
		{
			ACT_ScheduleSaveCommonMap::iterator it = m_kDetails.find(iDetail);
			return (it!=m_kDetails.end()) ? &it->second : NULL;
		}

		inline uint32_t GetScheduleSaveDetailAndValue (int32_t iDetail,int32_t key){
			ACT_ScheduleSaveCommonMap::iterator it = m_kDetails.find(iDetail);
			ACT_ScheduleSaveCommon*  dataPtr= (it!=m_kDetails.end()) ? &it->second : NULL;
			if(dataPtr!=NULL) return dataPtr->GetValue(key); 
			return 0 ; 
		}


		ACT_ScheduleSaveCommon*		_addScheduleSaveDetail(int32_t iDetail);

		void GetDataFromPB(const char_data_define::activity_schedule_entry& info);

		uint32_t						m_uiTimestamp;
		ACT_ScheduleSaveCommon			m_kSelf;
		ACT_ScheduleSaveCommonMap		m_kDetails;
	};
	typedef mem::map<int32_t, ACT_ScheduleSaveEntry> ACT_ScheduleSaveEntryMap;

	class ACT_ScheduleSaveCatagory : public LC_DirtyFlagAble
	{
	public:
		ACT_ScheduleSaveCatagory(void):m_iSchedule(0){}

		inline ACT_ScheduleSaveEntry* GetScheduleSaveEntry(int32_t iID)
		{
			ACT_ScheduleSaveEntryMap::iterator it = m_kSchedules.find(iID);
			return (it!=m_kSchedules.end()) ? &it->second : NULL;
		}
		void GetDataFromPB(const char_data_define::activity_schedule_catagory& info);

		ACT_ScheduleSaveEntry*			_addScheduleSaveEntry(int32_t iSchedule);
		int32_t							m_iSchedule;
		ACT_ScheduleSaveEntryMap		m_kSchedules;
	};
	typedef mem::map<int32_t, ACT_ScheduleSaveCatagory> ACT_ScheduleSaveCatagoryMap;
	typedef mem::map<int32_t, int32_t> ACT_SchedulePersistentInfoMap;

	class ACT_ScheduleSaveContainer : public LC_DirtyFlagAble
	{
	public:
		ACT_ScheduleSaveContainer(void) {}

		virtual void Clear(void)
		{
			m_kCatagorys.clear();
			LC_DirtyFlagAble::Reset();
		}

		inline ACT_ScheduleSaveCatagory* GetScheduleSaveCatagory(int32_t iCatagory)
		{
			ACT_ScheduleSaveCatagoryMap::iterator it = m_kCatagorys.find(iCatagory);
			return (it!=m_kCatagorys.end()) ? &it->second : NULL;
		}

		void GetDataFromPB(const char_data_define::activity_schedule_info& info);

		ACT_ScheduleSaveCatagory* _addScheduleSaveCatagory(int32_t iCatagory);

		int32_t GetPersistentVal(int32_t id)
		{
			ACT_SchedulePersistentInfoMap::iterator iter = m_kPersistentInfo.find(id);
			if (iter != m_kPersistentInfo.end())
			{
				return iter->second;
			}
			return 0;
		}
		void AddPersistentVal(int32_t id, int32_t val){ m_kPersistentInfo[id] += val; }
		void SetPersistentVal(int32_t id, int32_t val){ m_kPersistentInfo[id] = val; }
	protected:
		ACT_ScheduleSaveCatagoryMap		m_kCatagorys;
		ACT_SchedulePersistentInfoMap   m_kPersistentInfo;
	};
	typedef mem::map<int32_t, ACT_ScheduleSaveCatagoryMap> ACT_ScheduleSaveContainerMap;

	class LC_ClientActivityAsset : public LC_DirtyFlagAble
{	
	public:
		LC_ClientActivityAsset()
		{
			Clear();
		}
		~LC_ClientActivityAsset(void)
		{
			Clear();
		}
		void Clear()
		{
			m_SavaContainerMap.clear();
			LC_DirtyFlagAble::Reset();
		}

		DECL_RTTI_NAME(LC_ClientActivityAsset);
		bool IsFireReg(int32_t ActivityType, int32_t iCatagory, int32_t ScheduleID, int32_t iKey);
		bool IsRewardReg(int32_t ActivityType, int32_t iCatagory, int32_t ScheduleID, int32_t iKey);
		bool IsFeedbackRewardReg(int32_t ActivityType, int32_t iCatagory, int32_t ScheduleID, int32_t iKey, int32_t iIndex);
		bool IsFireRegStatType(int32_t ActivityType, int32_t iCatagory, int32_t ScheduleID, int32_t iKey, int32_t iStatType);
		bool IsRewardRegStatType(int32_t ActivityType, int32_t iCatagory, int32_t ScheduleID, int32_t iKey, int32_t iStatType);
		uint32_t GetValue(int32_t ActivityType, int32_t iCatagory, int32_t ScheduleID, int32_t iStatType, int32_t iValueType);
		uint32_t GetCount(int32_t ActivityType, int32_t iCatagory, int32_t ScheduleID, int32_t iStatType);
		uint32_t GetDetailCount(int32_t ActivityType, int32_t iCatagory, int32_t ScheduleID, int32_t iKey, int32_t iStatType);
		uint32_t GetDetailValue(int32_t ActivityType, int32_t iCatagory, int32_t ScheduleID, int32_t iKey, int32_t iStatType, int32_t iValueType);
		uint32_t GetTimeStamp(int32_t ActivityType, int32_t iCatagory, int32_t ScheduleID);
		StringType GetShopInfo(int32_t ActivityType, int32_t iCatagory, int32_t ScheduleID, int32_t iKey);
		StringType GetShopRewardInfo(int32_t ActivityType, int32_t iCatagory, int32_t ScheduleID, int32_t iKey);
		StringType GetShopRewardName(int32_t ActivityType, int32_t iCatagory, int32_t ScheduleID, int32_t iKey);
		StringType GetStarShopInfo(int32_t ActivityType, int32_t iCatagory, int32_t ScheduleID, int32_t iKey);
		StringType GetLotteryActivityButton(int32_t ActivityType, int32_t iCatagory, int32_t ScheduleID, int32_t iKey);
		int32_t GetShopRewardiID(int32_t ActivityType, int32_t iCatagory, int32_t ScheduleID, int32_t iKey);
		int32_t GetShopRewardMaxCount(int32_t ActivityType, int32_t iCatagory, int32_t ScheduleID, int32_t iKey);
		int32_t GetPersistentVal(int32_t ActivityType,int32_t id);
		StringType GetLotteryCardInfo(int32_t ActivityType, int32_t iCatagory, int32_t ScheduleID, int32_t iKey);
		int32_t   GetChargeDay(int32_t ActivityType, int32_t iCatagory, int32_t ScheduleID); 
		int32_t   GetCharge(int32_t ActivityType, int32_t iCatagory, int32_t ScheduleID,int32_t iRebateType); 
		
		int GetDirtyFlagRevisionIDByActivityType(int32_t ActivityType)
		{
			return GetScheduleSaveContainer(ActivityType)->GetDirtyFlagRevisionID();
		}
		int GetDirtyFlagRevisionID()
		{
			return LC_DirtyFlagAble::GetDirtyFlagRevisionID();
		}

		inline ACT_ScheduleSaveContainer* GetScheduleSaveContainer(int32_t ActivityType)
		{
			return &(m_SavaContainerMap[ActivityType]);
		}

		void GetDataFromPB(const char_data_define::char_activity_info& info);
	protected:
		ACT_ScheduleSaveEntry* _GetScheduleSaveEntry(int32_t ActivityType, int32_t iCatagory, int32_t ScheduleID)
		{
			ACT_ScheduleSaveCatagory* pkCatagory = GetScheduleSaveContainer(ActivityType)->GetScheduleSaveCatagory(iCatagory);
			return pkCatagory == NULL ? NULL : pkCatagory->GetScheduleSaveEntry(ScheduleID);
		}
		mem::map<int32_t, ACT_ScheduleSaveContainer> m_SavaContainerMap;
	};

	class ACT_ScheduleDataCatagory;

	class ACT_ScheduleDataContainer;

	class ACT_RewardDataEntry
	{
	public:
		ACT_RewardDataEntry(void)
			: m_iID(0)
			, m_iSchdeule(0)
			, m_iType(0)
			, m_iKey(0)
			, m_iSaveIndex(0)
			, m_iParentKey(0)
			, m_iCustomA(0)
			, m_iCustomB(0)
			, m_iRank(0)
			, m_iScore(0)
			, m_iShopID(0)
			, m_iMaxCount(0)
			, m_iPerCharge(0)
			, m_lCharTypeID1(0)
			, m_lCharTypeID2(0)
			, m_sGiftName("")
			, m_sDescSign("")
			, m_sRuleDesc("")
			, m_sCardDesc("")
			, m_sIcon1("")
			, m_sIcon2("")
			, m_sIcon3("")
			, m_sIcon4("")
			, m_bForbidExpire(false)
			, m_bHighReward(false)
			, m_iGuildLevel(0)
			, m_iOpenConsumeUnbindyuanbao(0)
			, m_iOpenVipLevel(0)
			, m_iBuddyID(0)
			, m_iMaxTimes(0)
		{}
		bool Init(CSVFile::CF_ACTSubordinateReward::DataEntry* pkEntry);
		bool Init(CSVFile::CF_ACTSubordinateRewardA::DataEntry* pkEntry);
		bool Init(CSVFile::CF_ACTSubordinateRewardB::DataEntry* pkEntry);
		bool Init(CSVFile::CF_ACTShop::DataEntry* pkEntry);
		bool Init(CSVFile::CF_ACTShopA::DataEntry* pkEntry);
		bool Init(CSVFile::CF_ACTShopB::DataEntry* pkEntry);
		bool Init(CSVFile::CF_ACTSubordinateConsume::DataEntry* pkEntry);
		bool Init(CSVFile::CF_ACTChargeProfit::DataEntry* pkEntry);
		bool Init(CSVFile::CF_ACTChargeProfitA::DataEntry* pkEntry);
		bool Init(CSVFile::CF_ACTShot::DataEntry* pkEntry);
		bool Init(CSVFile::CF_ACTLottery::DataEntry* pkEntry);
		bool Init(CSVFile::CF_HolidayActivity_PinTuReward::DataEntry* pkEntry);
		bool Init(CSVFile::CF_HolidayActivity_ChargeContinuous::DataEntry* pkEntry); 

		int32_t m_iID;
		int32_t m_iSchdeule;
		int32_t m_iType;
		int32_t m_iKey;
		int32_t m_iSaveIndex;
		int32_t	m_iParentKey;
		int32_t m_iCustomA;
		int32_t m_iCustomB;
		int32_t m_iRank;  // 现用作sortID
		combat_score_type m_iScore;
		int32_t m_iMaxCount;
		int32_t m_iShopID;
		int32_t m_iPerCharge;
		StringType m_SCustomStr;
		StringType m_sGiftName;  // 现用作各种名字
		StringType m_sDescSign;
		StringType m_sRuleDesc;
		StringType m_sCardDesc;
		StringType m_sButtonIcon1;
		StringType m_sIcon1;
		StringType m_sIcon2;
		StringType m_sIcon3;
		StringType m_sIcon4;
		StringType m_sNameIcon1;
		StringType m_sNameIcon2;
		StringType m_sNameIcon3;
		StringType m_sNameIcon4;
		bool    m_bForbidExpire;
		bool	m_bHighReward;
		long    m_lCharTypeID1;
		long    m_lCharTypeID2;
		int		m_iGuildLevel;
		int		m_iOpenConsumeUnbindyuanbao;
		int		m_iOpenVipLevel;
		int		m_iBuddyID;
		int		m_iMaxTimes;
		int     m_iRebateID; 

		Utility::UT_SIMDataList		m_kRewardCondition;
		Utility::UT_SIMDataList		m_kUICondition;
		Utility::UT_SIMDataMap		m_kLimits;
		Utility::LC_CashMap			m_kOriginalPrices;
		Utility::LC_CashMap			m_kCurrentPrices;
		Utility::UT_SIMDataList		m_kActScore;
		StringType		m_Position1;
		StringType		m_Position2;
		GameLogic::LC_ItemsReward	m_kRewardItems;

	};

	typedef mem::map<int32_t, ACT_RewardDataEntry>				ACT_RewardDataEntryMap;
	typedef ACT_RewardDataEntryMap::iterator					ACT_RewardDataEntryMapIter;
	class ACT_ScheduleDataEntry
	{
		friend class ACT_ScheduleDataCatagory;
	public:
		ACT_ScheduleDataEntry(void)
			:m_iSchedule(0)
			,m_iGroupID(0)
		{}

		inline ACT_RewardDataEntry* GetDataEntry(int32_t iKey)
		{
			ACT_RewardDataEntryMapIter it = m_kDatas.find(iKey);
			return it!=m_kDatas.end() ? &it->second : NULL;
		}

		inline ACT_RewardDataEntry* _addDataEntry(int32_t iKey, const ACT_RewardDataEntry& rkEntry)
		{
			m_kDatas[iKey] = rkEntry;
			m_KeyIDs.insert(iKey);
			return &m_kDatas[iKey];
		}

		inline const GameLogic::LC_IDSet& KeyIDs() { return m_KeyIDs; }
		inline const ACT_RewardDataEntryMap& entrys() { return m_kDatas;}

		int32_t						m_iSchedule;
		int32_t						m_iGroupID;
		GameLogic::LC_IDSet			m_KeyIDs;
		ACT_RewardDataEntryMap		m_kDatas;
	};

	typedef mem::map<int32_t, ACT_ScheduleDataEntry>		ACT_ScheduleDataMap;
	typedef ACT_ScheduleDataMap::iterator					ACT_ScheduleDataMapIter;
	typedef mem::map<int32_t, GameLogic::LC_IDSet>			ACT_ScheduleIndices;
	class ACT_ScheduleDataCatagory
	{
		friend class ACT_ScheduleDataContainer;
	public:
		ACT_ScheduleDataCatagory(void)
			:m_iCatagory(0)
		{}

		inline ACT_ScheduleDataEntry* GetScheduleData(int32_t iSchedule)
		{
			ACT_ScheduleDataMapIter it = m_kSchedules.find(iSchedule);
			return it!=m_kSchedules.end() ? &it->second : NULL;
		}

		inline ACT_RewardDataEntry* GetDataEntry(int32_t iSchedule, int32_t iKey)
		{
			ACT_ScheduleDataEntry* pkSchedule = GetScheduleData(iSchedule);
			return pkSchedule ? pkSchedule->GetDataEntry(iKey) : NULL;
		}

		inline const GameLogic::LC_IDSet& GetScheduleIDs() {return m_ScheduleIDs;}
	protected:
		ACT_ScheduleDataEntry* _addScheduleData(int32_t iSchedule)
		{
			ACT_ScheduleDataMapIter it = m_kSchedules.find(iSchedule);
			if (it != m_kSchedules.end())
			{
				return &it->second;
			}
			m_kSchedules[iSchedule].m_iSchedule = iSchedule;
			m_ScheduleIDs.insert(iSchedule);
			return &m_kSchedules[iSchedule];
		}

		inline ACT_RewardDataEntry* _addDataEntry(int32_t iSchedule, int32_t iKey, const ACT_RewardDataEntry& rkEntry)
		{
			ACT_ScheduleDataEntry* pkSchedule = _addScheduleData(iSchedule);
			return pkSchedule->_addDataEntry(iKey, rkEntry);
		}

		int32_t					m_iCatagory;
		GameLogic::LC_IDSet		m_ScheduleIDs;
		ACT_ScheduleDataMap		m_kSchedules;

	};

	typedef mem::map<int32_t, ACT_ScheduleDataCatagory>	ACT_CatagoryDataMap;
	typedef ACT_CatagoryDataMap::iterator				ACT_CatagoryDataMapIter;
	class ACT_ScheduleDataContainer
	{
		friend class LC_ClientActivityManager;
	public:
		ACT_ScheduleDataContainer(void){}

		inline void Clear() 
		{
			m_kCatagorys.clear();
			m_kScheduleIndices.clear();
		}

		inline ACT_ScheduleDataCatagory* GetCatagoryData(int32_t iCatagory)
		{
			ACT_CatagoryDataMapIter it = m_kCatagorys.find(iCatagory);
			return it!=m_kCatagorys.end() ? &it->second : NULL;
		}

		inline ACT_RewardDataEntry* GetDataEntry(int32_t iCatagory, int32_t iSchedule, int32_t iKey)
		{
			ACT_ScheduleDataCatagory* pkCatagory = GetCatagoryData(iCatagory);
			return pkCatagory ? pkCatagory->GetDataEntry(iSchedule, iKey) : NULL;
		}

		inline ACT_RewardDataEntry* AddDataEntry(int32_t iCatagory, int32_t iSchedule, int32_t iKey, const ACT_RewardDataEntry& rkEntry)
		{
			ACT_ScheduleDataCatagory* pkCatagory = _addCatagoryData(iCatagory);
			m_kScheduleIndices[iSchedule].insert(iCatagory);
			return pkCatagory ? pkCatagory->_addDataEntry(iSchedule, iKey, rkEntry) : NULL;
		}

		inline const GameLogic::LC_IDSet* GetScheduleIndices(int32_t iSchedule) 
		{
			ACT_ScheduleIndices::iterator it = m_kScheduleIndices.find(iSchedule);
			return (it!=m_kScheduleIndices.end()) ? &it->second : NULL;
		}

	protected:
		ACT_ScheduleDataCatagory* _addCatagoryData(int32_t iCatagory)
		{
			ACT_CatagoryDataMapIter it = m_kCatagorys.find(iCatagory);
			if (it != m_kCatagorys.end())
			{
				return &it->second;
			}
			m_kCatagorys[iCatagory].m_iCatagory = iCatagory;
			return &m_kCatagorys[iCatagory];
		}

		ACT_CatagoryDataMap		m_kCatagorys;
		ACT_ScheduleIndices		m_kScheduleIndices;
	};
	typedef mem::map<int32_t, ACT_ScheduleDataContainer> ACT_ScheduleDataContainerMap;
	class ACT_ScheduleEntry
	{
	public:
		ACT_ScheduleEntry(void)
			:m_iID(0), m_iTimeLen(0), m_bActive(false)
		{}

		~ACT_ScheduleEntry(void) {Unit();}

		bool Init(CSVFile::CF_ActivitySchedule::DataEntry* pkEntry);
		void Unit(void);

		void SetUp(uint32_t starttime, uint32_t conflatetime);

		void Rechedule(Utility::timer_id_type timerID);
		void Active(Utility::timer_id_type timerID);
		void Deactive(Utility::timer_id_type timerID);

		bool IsServerValid(int32_t iUnionID, int32_t iServerID);
		bool IsTimestampIn(int64_t timestamp);

		int64_t GetBeginTime()
		{
			return m_kSchedule.m_iStartTime;
		}
		int64_t GetEndTime()
		{
			return m_kSchedule.m_iEndTime;
		}

		inline bool GetActive(void) {return m_bActive;}

		int32_t						m_iID;
		StringType					m_sCron;
		int32_t						m_iTimeLen;
		CSVFile::StartTime			m_kStartCoolTime;
		CSVFile::StartTime			m_kStartValidTime;
		CSVFile::StartTime			m_kConflateCoolTime;
		CSVFile::StartTime			m_kConflateValidTime;
		Utility::UT_SIMDataList		m_kValidServers;
		Utility::UT_SIMDataList		m_kExceptServers;		

		bool						m_bActive;
		Utility::UT_TimerSchedule	m_kSchedule;
	};
	typedef mem::map<int32_t, ACT_ScheduleEntry> ACT_ScheduleEntryMap;
	struct ScheduleEvent
	{
		int32_t iActivityType;
		int32_t iCatagory;

		ScheduleEvent()
			:iActivityType(0)
			,iCatagory(0)
		{}
		ScheduleEvent(int32_t iType, int32_t Catagory)
			:iActivityType(iType)
			,iCatagory(Catagory)
		{}

		inline bool operator < (const ScheduleEvent& o) const
		{
			return iActivityType < o.iActivityType ? true : iCatagory < o.iCatagory;
		}
	};
	typedef mem::set<ScheduleEvent> ScheduleEventSet;

	struct ScheduleActiveInfo
	{
		int32_t iScheduleID;
		bool bActive;
		int64_t m_iStartTime;
		int64_t m_iEndTime;

		ScheduleActiveInfo()
			:iScheduleID(0)
			,bActive(false)
			,m_iStartTime(0)
			,m_iEndTime(0)
		{}
		ScheduleActiveInfo(int32_t iID, bool active, int64_t begintime, int64_t endtime)
			:iScheduleID(iID)
			,bActive(active)
			,m_iStartTime(begintime)
			,m_iEndTime(endtime)
		{}
	};

	typedef mem::map<int32_t, ScheduleActiveInfo> ScheduleActiveInfoMap;

	class LC_ClientActivityManager : public Memory::MM_BaseObject
		,public Utility::UT_Singleton<LC_ClientActivityManager>
	{
	public:
		LC_ClientActivityManager(void)
			: m_iReversionID (0)
			, m_iSubRankListCount(0)
			, m_iSubRankFirstCID(0)
			, m_iSubRankFirstUID(0)
		{
		}
		~LC_ClientActivityManager(void){Unit();}

		DECL_RTTI_NAME(LC_ClientActivityManager);

		void Init(CSVFile::CSVDataLoader* pkLoader=NULL, const CSVFile::CSVDataSourceList* pkCSVs=NULL);
		void Unit(void);

		void UnitSchedule(void);

		StringType GetScheduleImage(int32_t iScheduleId);
		StringType GetTimeIdImage(int iTimeID);
		void Schedule(uint32_t starttime, uint32_t conflatetime);

		bool IsScheduleValid(LC_PlayerBase* pkPlayer, int32_t iSchedule);
		inline bool IsTimestampIn(int32_t iSchedule, int64_t timestamp)
		{
			ACT_ScheduleEntry* pkSchedule = GetSchedule(iSchedule);
			return pkSchedule ? pkSchedule->IsTimestampIn(timestamp) : false;
		}
		inline int64_t GetScheduleBeginTimes(int32_t iSchedule)
		{
			ACT_ScheduleEntry* pkSchedule = GetSchedule(iSchedule);
			return pkSchedule ? pkSchedule->GetBeginTime() / ONE_SECOND_MILI : 0;
		}
		//BUG #37390 【决战武林】运营活动时间描述优化
		//这里活动时间进行减1s处理
		inline int64_t GetScheduleEndTimes(int32_t iSchedule)
		{
			ACT_ScheduleEntry* pkSchedule = GetSchedule(iSchedule);
			return pkSchedule ? (pkSchedule->GetEndTime() / ONE_SECOND_MILI - 1): 0;
		}

		bool IsTimeIdValid(LC_ClientGamePlayer* pkPlayer, int32_t iTimeID);
		int GetActiveTimeId(int32_t ActivityType, int32_t iCatagory, LC_ClientGamePlayer* pkPlayer);
		int64_t GetTimeIdEndTimes(int32_t iTimeID, uint32_t iRegTime);

		StringType GetSubActiveScheduleInfos(int32_t iCatagory, LC_PlayerBase* pkPlayer);
		int GetSubRankListCount() { return m_iSubRankListCount; }
		StringType GetSubRankInfo(int iGroupID, int iRank);
		StringType GetSelfSubRankInfo(int iGroupID);
		StringType GetHolidayACTRankInfo(int iID, int iRank);
		StringType GetHolidayACTSelfSubRankInfo(int iID);

		StringType GetSubRankCondition(int iScheduleID, int iType);
		int64_t GetSubBValue(int iGroupID);
		int		GetSubRankFirstCID() { return m_iSubRankFirstCID; }
		uint64_t GetSubRankFirstUID() { return m_iSubRankFirstUID; }

		bool CheckActivityChoice(int32_t ActivityType, int32_t iCatagory, int32_t iSchedule, LC_PlayerBase* pkPlayer);
		int32_t GetActiveSchedule(int32_t ActivityType, int32_t iCatagory, LC_PlayerBase* pkPlayer, bool bFlag = false);
		int32_t GetActiveScheduleAlways(int32_t ActivityType, int32_t iCatagory, LC_PlayerBase* pkPlayer, bool bFlag = false);
		StringType GetScheduleList(int32_t ActivityType, int32_t iCatagory, LC_ClientGamePlayer* pkPlayer); 
		int32_t GetRewardMaxKey(int32_t ActivityType, int32_t iCatagory, int32_t iSchedule);
		StringType GetRewardKeyIDsInfo(int32_t ActivityType, int32_t iCatagory, int32_t iSchedule);
		StringType GetRewardKeyIDsInfoEx(int32_t ActivityType, int32_t iCatagory, int32_t iSchedule);
		StringType GetRewardKeyIDsWithUIRank(int32_t ActivityType, int32_t iCatagory, int32_t iSchedule); // 现用作排序id
		StringType GetRewardChoiceIDsInfo(int32_t ActivityType, int32_t iCatagory, int32_t iSchedule, int32_t iKey, LC_PlayerBase* pkPlayer);
		StringType GetShopPriceInfo(int32_t ActivityType, int32_t iCatagory, int32_t iSchedule, int32_t iKey, bool bOrigin);
		StringType GetActScoreInfo(int32_t ActivityType, int32_t iCatagory, int32_t iSchedule, int32_t iKey, bool bOrigin);
		int	GetDirtyFlagRevisionID(){return m_iReversionID;}
		StringType GetScheduleIDEventParams(int32_t iScheule);

		void SyncLotteryRecord(const MG_SyncLotteryRecord& msg);
		StringType GetLotteryRecordInfo(int iType);

		void SyncChargeProfitData(const MG_SC_SynChargeProfit_Ret& msg);
		void UpdateChargeProfitData(const MG_SC_ChargeProfit_Reward_Ret& msg);
		StringType GetChargeProfitBInfo(int iId, bool bWithRwd=true);
		StringType GetChargeProfitData(int iId);

		void SyncLotteryData(const MG_SC_SyncLottery_Ret& msg);
		void UpdateLottryData(const MG_SC_Lottery_Draw_Ret& msg);
		void UpdateLottryData(const MG_SC_Lottery_ExtraReward_Ret& msg, bool bStaus);
		StringType GetLotteryDataById(int iId);

		StringType GetTreasureHuntInfo();

		inline ACT_ScheduleEntry* GetSchedule(int32_t iSchedule)
		{
			ACT_ScheduleEntryMap::iterator it = m_kScheduleMap.find(iSchedule);
			return (it!=m_kScheduleMap.end()) ? &it->second : NULL;
		}
		inline ACT_ScheduleDataContainer* GetScheduleDataContainer(int32_t ActivityType)
		{
			ACT_ScheduleDataContainerMap::iterator it = m_DataContainerMap.find(ActivityType);
			return it!=m_DataContainerMap.end() ? &it->second : NULL;
		}
		inline ACT_ScheduleDataCatagory* GetScheduleDataCatagory(int32_t ActivityType, int32_t iCatagory)
		{
			ACT_ScheduleDataContainer* pkContainer = GetScheduleDataContainer(ActivityType);
			return pkContainer == NULL ? NULL : pkContainer->GetCatagoryData(iCatagory);
		}
		inline ACT_ScheduleDataEntry* GetScheduleDataEntry(int32_t ActivityType, int32_t iCatagory, int32_t iSchedule)
		{
			ACT_ScheduleDataCatagory* pkCatagory = GetScheduleDataCatagory(ActivityType, iCatagory);
			return pkCatagory == NULL ? NULL : pkCatagory->GetScheduleData(iSchedule);
		}
		inline ACT_RewardDataEntry *GetRewardDataEntry(int32_t ActivityType, int32_t iCatagory, int32_t iSchedule, int32_t ikey)
		{
			ACT_ScheduleDataContainer* pkContainer = GetScheduleDataContainer(ActivityType);
			return pkContainer == NULL ? NULL : pkContainer->GetDataEntry(iCatagory,iSchedule,ikey);
		}
		static void _rescheduleCB(Utility::timer_id_type timerID, int32_t iID);
		static void _activeCB(Utility::timer_id_type timerID, int32_t iID);
		static void _deactiveCB(Utility::timer_id_type timerID, int32_t iID);

		void AddDirtyFlagRevisionID() {m_iReversionID++;}

		inline void	_AddScheduleEvent(int32_t iSheduleID, int32_t ActivityType, int32_t iCatagory)
		{
			m_kShceduleEventIndices[iSheduleID].insert(ScheduleEvent(ActivityType, iCatagory));
		}

		void GetDataFromPB(const msg_define::schedule_info& info);


		
		void GetDataFromStruct(const MG_RLT_GetRankSubBInfo& msg);
		
		//开服争霸榜
		void							UpdateSubBValue(std::vector<PS_SubBValue>& m_nSubBValue);
	protected:

		void _UpdateAllScheduleActiveInfo();
		int32_t _getActiveSchedule(int32_t ActivityType, int32_t iCatagory, LC_PlayerBase* pkPlayer, bool bFlag = false, bool bCheckValid=true);

		template<typename _Ty>
		StringType _getSubRankReward(int iGroupID, int iRank)
		{
			_Ty::DataEntryMapExternIterator iter = _Ty::GetSingletonPtr()->GetIterator();
			for( ; iter.HasMoreElements(); iter.MoveNext())
			{
				const _Ty::DataEntry *pkData = iter.PeekNextValuePtr();
				if(pkData->_GroupID == iGroupID && iRank >= pkData->_iRankStartIndex && iRank < pkData->_iRankEndIndex)
				{
					return CSVDataSourceBase::ReadFiledFromValue(pkData->_kReward);
				}
			}

			return "";
		}

		StringType _genSubRankData(int iGroupID, const PS_SubBInfo& info);
		StringType _genHolidayACTRankData(int iID, const PS_SubBInfo& info);

		template <typename DataSource, typename DataSourceA>
		void _InitActivityData(CSVFile::CSVDataLoader* pkLoader, const CSVFile::CSVDataSourceList* pkCSVs, int iActivityType);
		template <typename DataSource>
		void _InitActivityData(CSVFile::CSVDataLoader* pkLoader, const CSVFile::CSVDataSourceList* pkCSVs, int iActivityType);

		ACT_ScheduleEntryMap			m_kScheduleMap;
		ACT_ScheduleDataContainerMap	m_DataContainerMap;

		int								m_iReversionID;

		mem::map<int32_t, ScheduleEventSet> m_kShceduleEventIndices;
		ScheduleActiveInfoMap			m_kScheduleActiveMap;

		typedef mem::map<int, mem::map<int, PS_SubBInfo>> SuperSubRankMap;
		//争霸榜
		int											m_iSubRankListCount;		//排行榜玩家数量
		SuperSubRankMap	m_kSuperSubRankInfoMap;		//排行榜map, GruopID为key
		SuperSubRankMap	m_kHolidayActRankInfoMap;		//节日活动排行榜map, ACTChargeProfitB的ID为key
		mem::map<int32_t, int64_t>					m_kSuperSubValueMap;		//活动结算后，服务器下发最终value值,活动进行时客户端自己获取相应的值，结束后已服务器下发的为准
		mem::map<int32_t, int64_t>					m_kSuperSubBValueMap;		//服务器同步当前值的map，key为groupid
		//自己的排名信息
		PS_SubBInfo					m_kSelfSubRankInfo;
		//第一名的cid
		int							m_iSubRankFirstCID;
		//第一名的uid
		uint64_t					m_iSubRankFirstUID;

		//寻宝记录
		mem::map<int, PS_LotteryRecord> m_mLotteryRecord;
		//寻宝数据
		mem::map<int, Lottery_Base> m_mLotteryDataMap;

		//充值奖励数据
		mem::map<int, ChargeProfit_Base> m_mChargeProfitDataMap;
	};
}
