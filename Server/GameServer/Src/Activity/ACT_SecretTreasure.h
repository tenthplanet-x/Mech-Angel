#ifndef __ACT_SECRET_TREASURE_H__
#define __ACT_SECRET_TREASURE_H__

#pragma once

#include "LC_Define.h"
#include "LC_Helper.h"

#include "UT_Timer.h"
#include "UT_TimeManager.h"

#include "char_data.pb.h"
#include "redis.pb.h"

#include "CF_SecretTreasurePool.h"
#include "CF_SecretTreasureChests.h"
#include "CF_SecretTreasureSchedule.h"
#include "CF_SecretTreasureShop.h"
#include "CF_SecretTreasurePut.h"
#include "CF_SecretTreasureTimes.h"

class LC_ServerPlayer;

namespace Activity
{
	enum ACT_PutType
	{
		ACT_PUT_TP_LOCAL	= 0,		//本服
		ACT_PUT_TP_GROUP,				//组服
		ACT_PUT_TP_UNION,				//平台
	};

	enum ACT_TimesType
	{
		ACT_TIMES_TP_ONLINE	= 0,
		ACT_TIMES_TP_LIVENESS = 1,
		ACT_TIMES_TP_RECHARGE = 2,
		ACT_TIMES_TP_CONSUME = 3,
	};

	class ACT_SecretTreasurePoolDataEntry
	{
	public:
		ACT_SecretTreasurePoolDataEntry(void)
			:m_iID(0), m_iSchedule(0), m_iCatagory(0)
			,m_iMinLevel(0), m_iMaxLevel(0)
			,m_iRumorType(0)
			,m_iSelfTimes(0), m_iGlobalTimes(0)
		{}

		bool Init(CSVFile::CF_SecretTreasurePool::DataEntry* pkEntry);

		inline bool operator < (const ACT_SecretTreasurePoolDataEntry& o) const
		{
			return m_iGlobalTimes==o.m_iGlobalTimes ? m_iSelfTimes>o.m_iSelfTimes : m_iGlobalTimes>o.m_iGlobalTimes; 
		}

		void Resolve(LC_ServerPlayer* pkPlayer, Utility::UT_SIMDataList& kRewards, GameLogic::LC_RewardItemsExtend& rkExtend);

		int32_t						m_iID;
		int32_t						m_iSchedule;
		int32_t						m_iCatagory;
		int32_t						m_iMinLevel;
		int32_t						m_iMaxLevel;
		int32_t						m_iRumorType;
		int32_t						m_iSelfTimes;
		int32_t						m_iGlobalTimes;
		Utility::UT_SIMDataMap		m_kPuts;

		GameLogic::LC_ItemsReward	m_kRewardItems;
	};
	typedef mem::list<ACT_SecretTreasurePoolDataEntry> ACT_SecretTreasurePoolDataEntries;

	class ACT_SecretTreasurePutDataEntry : public LC_DirtyFlagAble
	{
	public:
		ACT_SecretTreasurePutDataEntry(void) 
			:m_iID(0), m_iSchedule(0)
			,m_iType(0), m_iMaxTimes(0)
			,m_iTimer(Utility::TIMER_ID_NA)
			,m_uiTimestamp(0)
			,m_iTimes(0)
		{}

		bool Init(CSVFile::CF_SecretTreasurePut::DataEntry* pkEntry);
		void Unit(void);

		void SetUp(void);
		void CheckReset(uint32_t timestamp);

		void CheckPut(void);
		bool GetPut(void);
		void ReleasePut(void);
		void ConsumePut(void);

		void PushSave(void);
		void Update(float fTime);

		void GetDataFromPB(const redis_define::secret_treasure_put_entry& info);
		void SetDataToPB(redis_define::secret_treasure_put_entry& info);

		inline uint32_t TotalRandomPro(int32_t iWeaponType, GameLogic::LC_RewardItemsExtend* pkExtend) {return m_kRewards.TotalRandomPro(iWeaponType, pkExtend);}

		int32_t						m_iID;
		int32_t						m_iSchedule;
		StringType					m_sCron;
		int32_t						m_iType;
		int32_t						m_iMaxTimes;
		Utility::UT_SIMDataMap		m_kInChargeServers;
		GameLogic::LC_ItemsReward	m_kRewards;

		std::string					m_sID;
		std::string					m_sServerID;
		std::string					m_sPutEntryKey;
		std::string					m_sPutPoolKey;
		std::string					m_sPutOccupyKey;

		Utility::timer_id_type					m_iTimer;
		uint32_t								m_uiTimestamp;
		int32_t									m_iTimes;
		redis_define::secret_treasure_put_entry	m_save;
	};
	typedef mem::map<int32_t, ACT_SecretTreasurePutDataEntry> ACT_SecretTreasurePutDataEntryMap;

	class ACT_SecretTreasureScheduleDataEntry
	{
	public:
		ACT_SecretTreasureScheduleDataEntry(void)
			:m_iSchedule(0)
		{}

		bool Init(CSVFile::CF_SecretTreasureSchedule::DataEntry* pkEntry);

		inline void ClearPool(void) 
		{
			m_kPools.clear();
			m_kKeys.clear();
		}
		inline void AddPoolEntry(const ACT_SecretTreasurePoolDataEntry& rkEntry)
		{
			m_kPools.push_back(rkEntry);
			m_kKeys.insert(rkEntry.m_iID);
			m_kPools.sort();
		}
		ACT_SecretTreasurePoolDataEntry* ResolvePool(LC_ServerPlayer* pkPlayer, int32_t iSelfTimes, int32_t iGlobalTimes);
				
		int32_t									m_iID;
		int32_t									m_iSchedule;
		int32_t									m_iCatagory;
		int32_t									m_iCluster;
		StringType								m_sDesc;
		Utility::UT_SIMDataList					m_kIcons;

		ACT_SecretTreasurePoolDataEntries		m_kPools;
		GameLogic::LC_IDSet						m_kKeys;
	};
	typedef mem::map<int32_t, ACT_SecretTreasureScheduleDataEntry> ACT_SecretTreasureScheduleDataEntryMap;

	class ACT_SecretTreasureCatagoryDataEntry : public LC_DirtyFlagAble 
	{
	public:
		ACT_SecretTreasureCatagoryDataEntry(void)
			:m_iCatagory(0), m_uiTimestamp(0), m_iTimes(0)
		{}

		void SetUp(void);
		void CheckReset(uint32_t timestamp);

		void PushSave();
		void Update(float fTime);

		void ClearPools(void)
		{
			ACT_SecretTreasureScheduleDataEntryMap::iterator it = m_kSchedules.begin();
			for (; it!=m_kSchedules.end(); ++it)
			{
				it->second.ClearPool();
			}
		}

		inline ACT_SecretTreasureScheduleDataEntry* GetSchedule(int32_t iScheduleID)
		{
			ACT_SecretTreasureScheduleDataEntryMap::iterator it = m_kSchedules.find(iScheduleID);
			return (it!=m_kSchedules.end()) ? &it->second : NULL;
		}

		inline bool IsValidCluster(int32_t iCluster)
		{
			return m_kClusters.find(iCluster) != m_kClusters.end();
		}

		void GetDataFromPB(const redis_define::secret_treasure_cat_entry& info);
		void SetDataToPB(redis_define::secret_treasure_cat_entry& info);

		int32_t									m_iCatagory;
		ACT_SecretTreasureScheduleDataEntryMap	m_kSchedules;

		GameLogic::LC_IDSet						m_kClusters;
		std::string								m_sCatagory;
		std::string								m_sSaveKey;
		uint32_t								m_uiTimestamp;
		int32_t									m_iTimes;
		redis_define::secret_treasure_cat_entry m_save;
	};
	typedef mem::map<int32_t, ACT_SecretTreasureCatagoryDataEntry> ACT_SecretTreasureCatagoryDataEntryMap;

	class ACT_SecretTreasureChestDataEntry 
	{
	public:
		ACT_SecretTreasureChestDataEntry(void)
			:m_iID(0), m_iCatagory(0), m_iTimesLimit(0)
			,m_iGold(0), m_iRandomTimes(0)
			,m_iBind(0), m_iScore(0)
			,m_iContribute(0), m_iPackType(PACK_TYPE_UNDEFINE)
		{}

		bool Init(CSVFile::CF_SecretTreasureChests::DataEntry* pkEntry);

		int32_t					m_iID;
		int32_t					m_iCatagory;
		int32_t					m_iTimesLimit;
		int32_t					m_iGold;
		Utility::UT_SIMDataList	m_kItems;
		GameLogic::LC_ShopIDMap	m_kShopIDMap;
		int32_t					m_iRandomTimes;
		int32_t					m_iBind;
		int32_t					m_iScore;
		int32_t					m_iContribute;
		int32_t					m_iPackType;
		StringType				m_sMailTitle;
		StringType				m_sMailDesc;
	};
	typedef mem::map<int32_t, ACT_SecretTreasureChestDataEntry> ACT_SecretTreasureChestDataEntryMap;

	class ACT_SecretTreasureShopDataEntry
	{
	public:
		ACT_SecretTreasureShopDataEntry(void)
			:m_iID(0), m_iCatagory(0), m_iSchedule(0), m_iScore(0)
		{}

		bool Init(CSVFile::CF_SecretTreasureShop::DataEntry* pkEntry);

		int32_t						m_iID;
		int32_t						m_iCatagory;
		int32_t						m_iSchedule;
		int32_t						m_iScore;
		int32_t						m_iTimes;
		Utility::UT_SIMDataList		m_kItems;
		Utility::UT_SIMDataMap		m_kLimits;
	};
	typedef mem::map<int32_t, ACT_SecretTreasureShopDataEntry> ACT_SecretTreasureShopDataEntryMap;

	class ACT_SecretTreasureCluserEntry 
	{
	public:
		ACT_SecretTreasureCluserEntry(void)
			:m_iID(0), m_iScore(0), m_iTimes(0)
		{}
		
		void GetDataFromPB(const char_data_define::secret_treasure_cluster& info);
		void SetDataToPB(char_data_define::secret_treasure_cluster& info);

		int32_t				m_iID;
		int32_t				m_iScore;
		int32_t				m_iTimes;
	};
	typedef mem::map<int32_t, ACT_SecretTreasureCluserEntry> ACT_SecretTreasureCluserEntryMap;

	class ACT_TreasureLog 
	{
	public:
		ACT_TreasureLog(void)
			:m_iTypeID(0)
			,m_iCount(0)
		{}

		void GetDataFromPB(const char_data_define::secret_treasure_log& info);
		void SetDataToPB(char_data_define::secret_treasure_log& info);

		int32_t			m_iTypeID;
		int32_t			m_iCount;
	};
	typedef mem::deque<ACT_TreasureLog>	ACT_TreasureLogList;

	class ACT_SecretTreasureChestEntry 
	{
	public:
		ACT_SecretTreasureChestEntry(void)
			:m_iID(0)
			,m_iTimes(0)
		{}

		void GetDataFromPB(const char_data_define::secret_treasure_chest& info);
		void SetDataToPB(char_data_define::secret_treasure_chest& info);

		inline int GetTreasureChestTimes(void){return m_iTimes;}
	
		int32_t		m_iID;
		int32_t		m_iTimes;
	};
	typedef mem::map<int32_t, ACT_SecretTreasureChestEntry> ACT_SecretTreasureChestEntryMap;

	class ACT_SecretTreasureShopEntry
	{
	public:
		typedef mem::map<int32_t, int32_t> TimesMap;
		ACT_SecretTreasureShopEntry(void)
			:m_iID(0)
		{}

		void GetDataFromPB(const char_data_define::secret_treasure_shop& info);
		void SetDataToPB(char_data_define::secret_treasure_shop& info);

		int32_t						m_iID;
		TimesMap					m_kTimes;
	};
	typedef mem::map<int32_t, ACT_SecretTreasureShopEntry> ACT_SecretTreasureShopEntryMap;

	class ACT_SecretTreasureCatagoryEntry : public LC_DirtyFlagAble
	{
	public:
		ACT_SecretTreasureCatagoryEntry(void)
			:m_iCatagory(0)
		{}

		inline ACT_SecretTreasureCluserEntry* GetClusterEntry(int32_t iClusterID)
		{
			ACT_SecretTreasureCluserEntryMap::iterator it = m_kClusters.find(iClusterID);
			return (it!=m_kClusters.end()) ? &it->second : NULL;
		}

		inline ACT_SecretTreasureChestEntry* GetChestEntry(int32_t iChestID)
		{
			ACT_SecretTreasureChestEntryMap::iterator it = m_kChests.find(iChestID);
			return (it!=m_kChests.end()) ? &it->second : NULL;
		}

		inline ACT_SecretTreasureShopEntry* GetShopEntry(int32_t iShopID)
		{
			ACT_SecretTreasureShopEntryMap::iterator it = m_kShops.find(iShopID);
			return (it!=m_kShops.end()) ? &it->second : NULL;
		}

		int32_t GetTimes(void);

		void GetDataFromPB(const char_data_define::secret_treasure_catagory& info);
		void SetDataToPB(char_data_define::secret_treasure_catagory& info);

		void RegLog(const Utility::UT_SIMDataInfo& kSimDataInfo);

		ACT_SecretTreasureCluserEntry*	_addClusterEntry(int32_t iClusterID);
		ACT_SecretTreasureChestEntry*	_addChestEntry(int32_t iChestID);
		ACT_SecretTreasureShopEntry*	_addShopEntry(int32_t iShopID);
	
		int32_t								m_iCatagory;
		ACT_SecretTreasureCluserEntryMap	m_kClusters;
		ACT_TreasureLogList					m_kLogs;
		ACT_SecretTreasureChestEntryMap		m_kChests;
		ACT_SecretTreasureShopEntryMap		m_kShops;
	};
	typedef mem::map<int32_t, ACT_SecretTreasureCatagoryEntry> ACT_SecretTreasureCatagoryEntryMap;

	class ACT_SecretTreasureAsset : public LC_DirtyFlagAble
	{
		friend class ACT_SecretTreasureManager;
	public:
		ACT_SecretTreasureAsset(void){}
		~ACT_SecretTreasureAsset(void){}

		inline void Clear(void) {m_kCatagorys.clear();}

		void CheckReset(uint32_t timestamp);

		inline ACT_SecretTreasureCatagoryEntry* GetCatagoryEntry(int32_t iCatagoryID)
		{
			ACT_SecretTreasureCatagoryEntryMap::iterator it = m_kCatagorys.find(iCatagoryID);
			return (it!=m_kCatagorys.end()) ? &it->second : NULL;
		}

		void GetDataFromPB(const char_data_define::secret_treasure_info& info);
		void SetDataToPB(char_data_define::secret_treasure_info& info);
	protected:
		ACT_SecretTreasureCatagoryEntry* _addCatagoryEntry(int32_t iCatagoryID);

	protected:
		uint32_t								m_uiTimestamp;
		ACT_SecretTreasureCatagoryEntryMap		m_kCatagorys;
	};

	class ACT_SecretTreasureManager: public Memory::MM_BaseObject
		,public Utility::UT_Singleton<ACT_SecretTreasureManager>
	{
		friend class ACT_SecretTreasurePutDataEntry;
	public:
		ACT_SecretTreasureManager(void)
			:m_bInited(false)
		{}
		
		void Init(void);
		void Unit(void);
		ResultType InitConfig(CSVFile::CSVDataLoader* pkLoader=NULL, const CSVFile::CSVDataSourceList* pkCSVs=NULL);

		ResultType OnGameServerLoaded(void);

		void CheckReset(uint32_t timestamp);

		void Update(float fTime);

		int32_t GetExtraTimes(LC_ServerPlayer* pkPlayer, int32_t iChestID);

		ResultType CheckTreasureOpen(LC_ServerPlayer* pkPlayer, int32_t iChestID, int32_t iScheduleID);
		ResultType CheckTreasureShop(LC_ServerPlayer* pkPlayer, int32_t iShopID);

		ResultType DoOpen(LC_ServerPlayer* pkPlayer, int32_t iChestID, int32_t iScheduleID, Utility::UT_SIMDataList& kRewards);
		ResultType DoShop(LC_ServerPlayer* pkPlayer, int32_t iShopID);

		inline ACT_SecretTreasureCatagoryDataEntry* GetCatagory(int32_t iCatagory)
		{
			ACT_SecretTreasureCatagoryDataEntryMap::iterator it = m_kCatagorys.find(iCatagory);
			return (it!=m_kCatagorys.end()) ? &it->second : NULL;
		}

		inline ACT_SecretTreasureScheduleDataEntry* GetSchedule(int32_t iCatagory, int32_t iScheduleID)
		{
			ACT_SecretTreasureCatagoryDataEntry* pkCatagory = GetCatagory(iCatagory);
			return pkCatagory ? pkCatagory->GetSchedule(iScheduleID) : NULL;
		}

		inline ACT_SecretTreasureChestDataEntry* GetChest(int32_t iChestID)
		{
			ACT_SecretTreasureChestDataEntryMap::iterator it = m_kChests.find(iChestID);
			return (it!=m_kChests.end()) ? &it->second : NULL;
		}

		inline ACT_SecretTreasureShopDataEntry* GetShop(int32_t iShopID)
		{
			ACT_SecretTreasureShopDataEntryMap::iterator it = m_kShops.find(iShopID);
			return (it!=m_kShops.end()) ? &it->second : NULL;
		}

		inline ACT_SecretTreasurePutDataEntry* GetPut(int32_t iPutID)
		{
			ACT_SecretTreasurePutDataEntryMap::iterator it = m_kPuts.find(iPutID);
			return (it!=m_kPuts.end()) ? &it->second : NULL;
		}

		void ClearPuts(void)
		{
			ACT_SecretTreasurePutDataEntryMap::iterator it = m_kPuts.begin();
			for (; it!=m_kPuts.end(); ++it)
			{
				it->second.Unit();
			}
		}
	protected:
		inline ACT_SecretTreasureCatagoryDataEntry* _addCatagory(int32_t iCatagoryID)
		{
			m_kCatagorys[iCatagoryID].m_iCatagory = iCatagoryID;
			return &m_kCatagorys[iCatagoryID];
		}
	protected:
		static void _onCronPut(Utility::timer_id_type timerID, int32_t iPut);
	protected:
		ACT_SecretTreasureCatagoryDataEntryMap	m_kCatagorys;
		ACT_SecretTreasureChestDataEntryMap		m_kChests;
		ACT_SecretTreasureShopDataEntryMap		m_kShops;
		ACT_SecretTreasurePutDataEntryMap		m_kPuts;
	private:
		bool									m_bInited;
		Utility::UT_Timer						m_kUpdateTimer;
	};
}

#endif //__ACT_SECRET_TREASURE_H__
