#ifndef __LC_RANK_ACTIVITY_H__
#define __LC_RANK_ACTIVITY_H__

#pragma once
#include <map>

#include "LC_Define.h"
#include "LC_DirtyFlag.h"
#include "LC_Helper.h"

#include "CF_ShotActivity.h"

#include "char_data.pb.h"
#include "redis.pb.h"
#include "msg.pb.h"
#include "SystemError.h"
#include "MG_ProtocolStruct.h"
#include "UT_MathBase.h"
#include "UT_TimeManager.h"
#include "UT_Timer.h"

using namespace Utility;

class LC_ServerPlayer;

namespace ShotActivityData
{
	struct RankEntryData
	{
		unique_id_type d;
		int64_t		v;
		int			i;
		RankEntryData():d(0), v(0), i(0) {}
		int32_t citizenship(void) {return unique_id_impl(d).catagory();}
	};

	typedef mem::vector<RankEntryData> RankEntryDataVec;
	typedef mem::map<unique_id_type, RankEntryData*> RankEntryDataIndices;

	struct RankCharInfo
	{
		unique_id_impl		cid;
		std::string			name;
		std::string			lite;
		std::string			detail;
		Utility::UT_Timer	_timer;

		RankCharInfo(void)
			:cid(0)
		{
			_timer.SetTimer(GET_INTERVAL(CMN_INTER_ENOUGH));
		}

		inline bool IsExpired(void){return _timer.IsExpired();}

		void	SetDataToPB(redis_define::char_sim_info& info)
		{
			info.set_cid(cid);
			info.set_name(name);
			info.set_lite(lite);
			info.set_detail(detail);
		}
		void	GetDataFromPB(redis_define::char_sim_info& info)
		{
			cid = info.cid();
			name = info.name();
			lite = info.lite();
			detail = info.detail();
		}
	};
	typedef mem::map<unique_id_type, RankCharInfo> RankCharIndices;

	struct WhetherDetailOper
	{
		inline bool operator () (int32_t idx) const
		{
			return switch_ && (idx<size_);
		}
		bool	switch_;
		int32_t	size_;
	};

	struct RankSnapData
	{
		int32_t					rankid;
		int64_t					timestamp;
		RankEntryDataVec		rankdata;
		RankEntryDataIndices	kDataIndices;
		int32_t					ranksize;
		static RankCharIndices	kCharIndices;

		RankSnapData():rankid(0), timestamp(0), ranksize(0) {}

		inline void Clear(void)
		{
			kDataIndices.clear();
			rankdata.clear();
		}
		void						SetUpDetail(void);
		void						SetUpIndices(void);
		RankEntryData*				GetRankDataAcrdIndex(int32_t idx);
		RankEntryData*				GetRankData(object_id_type cid);
		inline void					AddRankData(const RankEntryData& rkEntry)
		{
			rankdata.push_back(rkEntry);
		}
		static inline std::string	GetName(unique_id_type cid)
		{
			RankCharIndices::iterator it = kCharIndices.find(cid);
			return (it!=kCharIndices.end()) ? it->second.name : std::string("");
		}
		static inline RankCharInfo*	GetCharInfo(const unique_id_impl& cid)
		{
			RankCharIndices::iterator it = kCharIndices.find(cid);
			return (it!=kCharIndices.end()) ? &it->second : NULL;
		}
		void						OnPlayerPBDetailResp(const Protocol::PS_User_Detail_PBInfo& info);
		void						OnPlayerPBDetailResp(const unique_id_impl& charID, const std::string& info);
	};
}

enum LC_RankSettleType
{
	LC_RK_STL_TP_GROUP = 0,
	LC_RK_STL_TP_REDIS,
};

class LC_ShotActivityEntry;
class LC_ShotActivitySaveEntry : public LC_DirtyFlagAble
{
public:
	LC_ShotActivitySaveEntry(void)
		:m_iServer(0)
	{
		m_kSyncTimer.SetTimer(GET_INTERVAL(CMN_INTER_ENOUGH));
	}

	void Init(int32_t iServerID, LC_ShotActivityEntry* pkEntry);	

	inline void Reset(uint32_t timestamp)
	{
		m_kSave.set_bench_time(timestamp);
		m_kSave.set_flag(0);
		m_kSave.set_count(0);
		UpdateDirtyFlag();
	}

	inline int32_t GetServer(void) {return m_iServer;}

	inline void SetID(int32_t iID) {m_kSave.set_id(iID);}
	inline int32_t GetID(void) {return m_kSave.id();}

	inline void SetCount(int32_t count) 
	{
		if (m_kSave.count() != count)
		{
			m_kSave.set_count(count);
		}
	}
	inline int32_t GetCount(void) {return m_kSave.count();}

	inline void SetBencheTime(uint32_t benchTime) {m_kSave.set_bench_time(benchTime);UpdateDirtyFlag();}
	inline uint32_t GetBencheTime(void) {return m_kSave.bench_time();}

	inline void SetEndTime(uint32_t endTime) {m_kSave.set_end_time(endTime);UpdateDirtyFlag();}
	inline uint32_t GetEndTime(void) {return m_kSave.end_time();}

	void SetFlag(int32_t bit, bool val) 
	{
		uint32_t flag = m_kSave.flag();
		if (val)
		{
			m_kSave.set_flag(flag | (1 << bit));
		}
		else
		{
			m_kSave.set_flag(flag & ~(1 << bit));
		}
		UpdateDirtyFlag();
	}
	inline bool IsFlagSet(int32_t flag)
	{
		return (m_kSave.flag() & (1<<flag)) != 0;
	}
	inline uint32_t GetFlag(void) {return m_kSave.flag();}

	std::string GetRankKey(uint32_t timestamp);
	std::string GetCountKey(uint32_t timestamp);
	std::string GetRankCharKey(void);

	void OnShotAvailResp(uint32_t timestamp, int32_t ret, int32_t count);

	void Update(LC_ShotActivityEntry* pkEntry, bool bActive);

	int32_t								m_iServer;
	redis_define::shot_activity_entry	m_kSave;
	std::string							m_sSaveKey;
	std::string							m_sID;
	std::string							m_sRankID;			
	std::string							m_sRankPrefix;
	std::string							m_sAvailCountsKey;
	std::string							m_sRankCharKey;

	Utility::UT_Timer					m_kSyncTimer;
};
typedef mem::map<int32_t, LC_ShotActivitySaveEntry> LC_ShotActivitySaveEntryMap;

class LC_ShotActivityEntry
{
public:
	typedef mem::map<int32_t, redis_define::shot_activity_entry> shot_entry_map;
	LC_ShotActivityEntry(): m_iID(0), m_pkData(NULL) {}

	DECL_RTTI_NAME(LC_ShotActivityEntry);
	inline int32_t				ID(void) {return m_iID;}
	inline bool					IsCountLimit(void) {return m_pkData->_iCount > 0;}
	inline bool					IsRankNeedFixed(void) { return m_pkData->_iRankSize > 0; }

	inline int32_t				RankType(void) const	{ return m_pkData->_iRankType; }
	inline int32_t				RankFlag(void) const	{ return m_pkData->_iRankFlag; }
	inline int32_t				RankSettle(void) const	{ return m_pkData->_iRankSettle; }
	inline int32_t				ScheduleID(void) const	{ return m_pkData->_iSchedule; }
	inline int32_t				SettleDelay(void) const	{ return m_pkData->_iSettleDelay; }
	inline int32_t				RankID(void) const { return getRankIDByType(RankType(), RankFlag()); }
	inline int32_t				RankStart(void) const { return m_pkData->_iRankStartIndex; }
	inline int32_t				RankSize(void) const { return m_pkData->_iRankSize > 0 ? m_pkData->_iRankStartIndex + m_pkData->_iRankSize : 0; }
	inline int32_t				RankThreshold(void) const { return m_pkData->_iRankSize > 0 ? m_pkData->_iScore : 0; }
	inline int32_t				ACTKey(void) const { return m_pkData->_iACTKey; }
	inline bool					IsScoreValid(int64_t iVal) const { return iVal >= m_pkData->_iScore; }
	inline bool					IsRankValid(int32_t iIdx) const { return iIdx >= m_pkData->_iRankStartIndex && iIdx < RankSize(); }
	inline bool					IsMailReward(void) const { return m_pkData->_iMailReward > 0; }

	inline bool				IsMailed(int32_t iServerID) 
	{
		LC_ShotActivitySaveEntry* pkEntry = GetSaveEntry(iServerID);
		return pkEntry ? pkEntry->IsFlagSet(redis_define::shot_activity_entry::MAILED) : true;
	}
	
	inline int32_t			CurrentCount(int32_t iServerID)
	{
		LC_ShotActivitySaveEntry* pkEntry = GetSaveEntry(iServerID);
		return pkEntry ? pkEntry->GetCount() : 0;
	}
	inline int32_t			AvailableCount(int32_t iServerID)
	{
		return m_pkData->_iCount > 0 ? m_pkData->_iCount - CurrentCount(iServerID) : 0xFFFF;
	}
	
	inline int32_t			CountLimit(void) {return  m_pkData->_iCount > 0;}

	int32_t					PeekCount(int32_t iServerID);
	void					RegMailed(int32_t iServerID);
	void					ResetMailed(int32_t iServerID);

	uint32_t				BenchTimestamp(int32_t iServerID) const ;
	uint32_t				ValidTimestamp(int32_t iServerID) const ;

	LC_ShotActivitySaveEntry*	GetSaveEntry(int32_t iServerID);

	void OnShotAvailResp(int32_t iServerID, uint32_t timestamp, int32_t ret, int32_t count);

	int32_t										m_iID;
	CSVFile::CF_ShotActivity::DataEntry*		m_pkData;
	LC_ShotActivitySaveEntryMap					m_kSaves;
};
typedef mem::map<int32_t, LC_ShotActivityEntry> LC_ShotActivityEntryMap;

enum LC_RankStatus
{
	LC_RK_ST_INIT = 0,
	LC_RK_ST_SUC,
	LC_RK_ST_FAIL,
};

class LC_ShotActivityRankEntryBase: public Memory::MM_BaseObject
{
public:
	LC_ShotActivityRankEntryBase(void)
		:iSettle(0), uiFlag(0), uiTimestamp(0), iRankID(0), iScheduleID(0), iSize(0), iThreshold(0), iRankOff(0)
	{}

	inline void SetRankSettle(int32_t settle) {iSettle = settle;}
	inline int32_t RankSettle(void) {return iSettle;}

	inline void SetRankID(int32_t rankID) 
	{
		iRankID = rankID;
		kData.rankid = rankID;
	}
	inline int32_t RankID(void) {return iRankID;}

	inline void SetRankScheduleID(int32_t scheduleID) {	iScheduleID = scheduleID;}
	inline int32_t RankScheduleID(void) {return iScheduleID;}

	inline void AddServer(int32_t iServerID) {kServers.insert(iServerID);}
	inline bool IsRelatedServer(int32_t iServerID) {return kServers.find(iServerID) != kServers.end();}

	inline void AddRelatedID(int32_t iID) {kRelatedIDS.insert(iID);}
	inline bool IsRelatedID(int32_t iID) {return kRelatedIDS.find(iID) != kRelatedIDS.end();}

	inline int32_t RankThreshold(void) {return iThreshold;}

	inline void SetFlag(LC_RankStatus e, bool st)
	{
		if (st)
		{
			uiFlag |= (1 << e);
		}
		else
		{
			uiFlag &= ~(1 << e);
		}
	}

	inline bool		IsFlagSet(LC_RankStatus e) { return (uiFlag & (1 << e)) > 0;}
	inline void		SetSize(int32_t sz)	{ iSize = UT_MathBase::MaxInt(iSize, sz); }
	inline void		SetThreshold(int32_t threshold)	{ iThreshold = UT_MathBase::MaxInt(iThreshold, threshold); }
	inline ShotActivityData::RankEntryData* GetRankDataAcrdIndex(int32_t idx) {	return kData.GetRankDataAcrdIndex(idx+iRankOff);	}
	inline ShotActivityData::RankEntryData* GetRankData(object_id_type cid)	{ return kData.GetRankData(cid);}
	inline bool		IsSnapFor(const LC_ShotActivityEntry& rkShot) const 
	{ 
		return rkShot.RankID()==iRankID && rkShot.RankSettle()==iSettle && rkShot.ScheduleID()==iScheduleID; 
	}
	inline void		ResetRankOff(void) {iRankOff = 0;}
	inline void		IncRankOff(void){++iRankOff;}
	inline int32_t	RankOff(void){return iRankOff;}

	virtual void	Update(uint32_t timestamp){};

	int32_t			iSettle;
	uint32_t		uiFlag;
	uint32_t		uiTimestamp;
	int32_t			iRankID;
	int32_t			iScheduleID;
	std::string		sKey;
	int32_t			iSize;
	int32_t			iThreshold;
	GameLogic::LC_IDSet kServers;
	GameLogic::LC_IDSet kRelatedIDS;
	ShotActivityData::RankSnapData kData;
	int32_t			iRankOff;
	std::string		sRankCharKey;

	static const int32_t iCharCacheSize = 5;
};

class LC_ShotActivityRankRedisEntry : public LC_ShotActivityRankEntryBase
{
public:
	LC_ShotActivityRankRedisEntry(void){
		m_kSyncTimer.SetTimer(GET_INTERVAL(CMN_INTER_INERT));
		SetSize(st_max_granu);
	}
	void			OnSync(int32_t ret, std::vector<std::string>& vals);
	void			OnRankSize(int32_t ret, int64_t size);

	void			CheckSync(uint32_t timestamp);

	void			OnResp(const unique_id_impl& charID, const msg_define::shot_activity_rank_info_req& req, int32_t ret, std::vector<std::string>& vals);
	void			OnRankIndex(const unique_id_impl& charID, const msg_define::shot_activity_rank_index_req& req, int32_t ret, int64_t index);

	void			RequestRank(LC_ServerPlayer* pkPlayer, const msg_define::shot_activity_rank_info_req& req);
	void			RequestRankIndex(LC_ServerPlayer* pkPlayer, const msg_define::shot_activity_rank_index_req& req);

	virtual void	Update(uint32_t timestamp)
	{
		CheckSync(timestamp);
	}

	bool			UpdateChar(LC_ServerPlayer* pkPlayer);

	Utility::UT_Timer	m_kSyncTimer;

	static const int32_t st_max_granu = 10; 
};

typedef mem::map<std::string, LC_ShotActivityRankEntryBase*> LC_ShotActivityRankEntryMap;
typedef mem::map<int32_t, LC_ShotActivityRankEntryBase*> LC_ShotActivityRankEntryIndices;

class LC_ShotActivityManager : public Memory::MM_BaseObject, public Utility::UT_Singleton<LC_ShotActivityManager>
{
public:
	LC_ShotActivityManager(): m_bInited(false){}

	ResultType				Init(void);
	void					Unit(void);
	ResultType				InitConfig(CSVFile::CSVDataLoader* pkLoader=NULL, const CSVFile::CSVDataSourceList* pkCSVs=NULL);
	ResultType				OnGameServerLoaded(void);
	void					Update(float fCurrentTime);

	void					OnShotAvailResp(int32_t iID, int32_t iServerID, uint32_t timestamp, int32_t ret, const std::string& count);

	void					OnCharInfo(unique_id_impl charID, std::string sRankKey, int32_t ret, const std::string& sInfo);

	void					OnRankResp(unique_id_impl charID, msg_define::shot_activity_rank_info_req rank_req, int32_t ret, std::vector<std::string>& vals);
	void					OnRankIndex(unique_id_impl charID, msg_define::shot_activity_rank_index_req rank_idx_req, int32_t ret, int64_t size);

	void					OnRankSync(std::string sKey, int32_t ret, std::vector<std::string>& vals);
	void					OnRankSize(std::string sKey, int32_t ret, int64_t size);
	
	LC_ShotActivityEntry*	GetShotActivityEntry(int32_t iID);
	LC_ShotActivityRankEntryBase* GetShotActivityRankEntry(const std::string& key)
	{
		LC_ShotActivityRankEntryMap::iterator it = m_kShotActivityRankEntryMap.find(key);
		return (it!=m_kShotActivityRankEntryMap.end()) ? it->second : NULL;
	}
	void OnRankInfoReq(LC_ServerPlayer* pkPlayer, const msg_define::shot_activity_rank_info_req& rank_req);
	void OnRankIndexReq(LC_ServerPlayer* pkPlayer, const msg_define::shot_activity_rank_index_req& rank_idx_req);

	void OnPlayerPBDetailResp(const Protocol::PS_User_Detail_PBInfo& info);
	void OnShotFixed(LC_ShotActivityRankEntryBase* pkShotRank);

protected:
	LC_ShotActivityEntryMap				m_kShotActivityEntryMap;
	LC_ShotActivityRankEntryMap			m_kShotActivityRankEntryMap;
	bool								m_bInited;
	Utility::UT_Timer					m_kUpdateTimer;
};

#endif
