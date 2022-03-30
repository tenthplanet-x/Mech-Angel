#ifndef __ACT_ACTIVITY_ASSET_H__
#define __ACT_ACTIVITY_ASSET_H__

#pragma once

#include "LC_DirtyFlag.h"
#include "LC_Define.h"

#include "char_data.pb.h"

class LC_ServerPlayer;
namespace Activity
{	
	class ACT_ScheduleEntry;
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
		void ClearEx(int32_t iBegin, int32_t iEnd);
		
		inline int32_t	GetID(void) const {return m_iID;}
		inline void		SetValue(int32_t key, uint32_t val){m_kDatas[key] = val;}
		inline void		AddValue(int32_t key, int32_t val){m_kDatas[key] += val;}
		inline uint32_t GetValue(int32_t key){return m_kDatas[key];}

		void GetDataFromPB(const char_data_define::activity_schedule_common& info);
		void SetDataToPB(char_data_define::activity_schedule_common& info);
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
			m_uiTimestamp = timestamp;
			m_kSelf.Init(iID);
		}

		void Clear(uint32_t timestamp = 0);

		bool CheckReset(ACT_ScheduleEntry* pkSchedule, uint32_t timestamp);

		inline int32_t GetID(void) const {return m_kSelf.GetID();}

		inline void RegFire(int32_t iValue, int32_t iStatType=GameLogic::LC_ST_TP_FORERVER)
		{
			if(m_kSelf._regBit(GET_SCHEDULE_KEY(iStatType, ACT_SCHEDULE_FIRE_BEGIN), GET_SCHEDULE_KEY(iStatType, ACT_SCHEDULE_FIRE_END), iValue))
			{
				UpdateDirtyFlag();
			}
		}
		bool IsFireReg(int32_t iValue, int32_t iStatType=GameLogic::LC_ST_TP_FORERVER)
		{
			return m_kSelf._isBitReg(GET_SCHEDULE_KEY(iStatType, ACT_SCHEDULE_FIRE_BEGIN), GET_SCHEDULE_KEY(iStatType, ACT_SCHEDULE_FIRE_END), iValue);
		}

		inline void RegReward(int32_t iValue, int32_t iStatType=GameLogic::LC_ST_TP_FORERVER)
		{
			if(m_kSelf._regBit(GET_SCHEDULE_KEY(iStatType, ACT_SCHEDULE_REWARD_BEGIN), GET_SCHEDULE_KEY(iStatType, ACT_SCHEDULE_REWARD_END), iValue))
			{
				UpdateDirtyFlag();
			}
		}
		bool IsRewardReg(int32_t iValue, int32_t iStatType=GameLogic::LC_ST_TP_FORERVER)
		{
			return m_kSelf._isBitReg(GET_SCHEDULE_KEY(iStatType, ACT_SCHEDULE_REWARD_BEGIN), GET_SCHEDULE_KEY(iStatType, ACT_SCHEDULE_REWARD_END), iValue);
		}
		inline void ClearRewardReg(int32_t iStatType = GameLogic::LC_ST_TP_FORERVER)
		{
			m_kSelf.ClearEx(GET_SCHEDULE_KEY(iStatType, ACT_SCHEDULE_REWARD_BEGIN), GET_SCHEDULE_KEY(iStatType, ACT_SCHEDULE_REWARD_END));
			UpdateDirtyFlag();
		}

		inline void SetCount(int32_t iValue, int32_t iStatType=GameLogic::LC_ST_TP_FORERVER)
		{
			m_kSelf.SetValue(GET_SCHEDULE_KEY(iStatType, ACT_SCHEDULE_COUNT), iValue);
			UpdateDirtyFlag();
		}
		inline void AddCount(int32_t iValue, int32_t iStatType=GameLogic::LC_ST_TP_FORERVER)
		{
			m_kSelf.AddValue(GET_SCHEDULE_KEY(iStatType, ACT_SCHEDULE_COUNT), iValue);
			UpdateDirtyFlag();
		}
		inline uint32_t GetCount(int32_t iStatType=GameLogic::LC_ST_TP_FORERVER)
		{
			return m_kSelf.GetValue(GET_SCHEDULE_KEY(iStatType, ACT_SCHEDULE_COUNT));
		}

		inline void SetAchieve(int32_t iValue, int32_t iStatType=GameLogic::LC_ST_TP_FORERVER)
		{
			m_kSelf.SetValue(GET_SCHEDULE_KEY(iStatType, ACT_SCHEDULE_ACHIEVE), iValue);
			UpdateDirtyFlag();
		}
		inline void AddAchieve(int32_t iValue, int32_t iStatType=GameLogic::LC_ST_TP_FORERVER)
		{
			m_kSelf.AddValue(GET_SCHEDULE_KEY(iStatType, ACT_SCHEDULE_ACHIEVE), iValue);
			UpdateDirtyFlag();
		}
		inline uint32_t GetAchieve(int32_t iStatType=GameLogic::LC_ST_TP_FORERVER)
		{
			return m_kSelf.GetValue(GET_SCHEDULE_KEY(iStatType, ACT_SCHEDULE_ACHIEVE));
		}

		inline void SetActShopScore(int32_t iValue, int32_t iStatType = GameLogic::LC_ST_TP_FORERVER)
		{
			m_kSelf.SetValue(GET_SCHEDULE_KEY(iStatType, ACT_SCHEDULE_ACTSHOP_SCORE), iValue);
			UpdateDirtyFlag();
		}
		inline void AddActShopScore(int32_t iValue, int32_t iStatType = GameLogic::LC_ST_TP_FORERVER)
		{
			m_kSelf.AddValue(GET_SCHEDULE_KEY(iStatType, ACT_SCHEDULE_ACTSHOP_SCORE), iValue);
			UpdateDirtyFlag();
		}
		inline uint32_t GetActShopScore(int32_t iStatType = GameLogic::LC_ST_TP_FORERVER)
		{
			return m_kSelf.GetValue(GET_SCHEDULE_KEY(iStatType, ACT_SCHEDULE_ACTSHOP_SCORE));
		}

		inline void SetValue(int32_t iValue, ACT_ScheduleDataFlag iFlag, int32_t iStatType = GameLogic::LC_ST_TP_FORERVER)
		{
			m_kSelf.SetValue(GET_SCHEDULE_KEY(iStatType, iFlag), iValue);
			UpdateDirtyFlag();
		}
		inline void AddValue(int32_t iValue, ACT_ScheduleDataFlag iFlag, int32_t iStatType = GameLogic::LC_ST_TP_FORERVER)
		{
			m_kSelf.AddValue(GET_SCHEDULE_KEY(iStatType, iFlag), iValue);
			UpdateDirtyFlag();
		}
		inline uint32_t GetValue(ACT_ScheduleDataFlag iFlag, int32_t iStatType = GameLogic::LC_ST_TP_FORERVER)
		{
			return m_kSelf.GetValue(GET_SCHEDULE_KEY(iStatType, iFlag));
		}

		inline ACT_ScheduleSaveCommon* GetScheduleSaveDetail(int32_t iDetail)
		{
			ACT_ScheduleSaveCommonMap::iterator it = m_kDetails.find(iDetail);
			return (it!=m_kDetails.end()) ? &it->second : NULL;
		}

		void GetDataFromPB(const char_data_define::activity_schedule_entry& info);
		void SetDataToPB(char_data_define::activity_schedule_entry& info, bool all);
	
		ACT_ScheduleSaveCommon*		_addScheduleSaveDetail(int32_t iDetail);
		void						_clearDetails(){ m_kDetails.clear(); }
		void						_clearData(int32_t iStatType, uint32_t timestamp);

		uint32_t						m_uiTimestamp;
		ACT_ScheduleSaveCommon			m_kSelf;
		ACT_ScheduleSaveCommonMap		m_kDetails;
	};
	typedef mem::map<int32_t, ACT_ScheduleSaveEntry> ACT_ScheduleSaveEntryMap;

	class ACT_ScheduleSaveCatagory : public LC_DirtyFlagAble
	{
	public:
		ACT_ScheduleSaveCatagory(void):m_iID(0){}

		inline int32_t ID(void) const {return m_iID;}
		inline void Clear(void)
		{
			m_kSchedules.clear();
			LC_DirtyFlagAble::Reset();
		}
		void ForceDirty(void);

		bool CheckReset(LC_ServerPlayer* pkPlayer, uint32_t timestamp);
		bool CheckReset(ACT_ScheduleEntry* pkSchedule, uint32_t timestamp);

		void ClearScheuleSaveEntry(int32_t iID);

		inline ACT_ScheduleSaveEntry* GetScheduleSaveEntry(int32_t iID)
		{
			ACT_ScheduleSaveEntryMap::iterator it = m_kSchedules.find(iID);
			return (it!=m_kSchedules.end()) ? &it->second : NULL;
		}
		void GetDataFromPB(const char_data_define::activity_schedule_catagory& info);
		void SetDataToPB(char_data_define::activity_schedule_catagory& info, bool all);

		ACT_ScheduleSaveEntry*		_addScheduleSaveEntry(int32_t iSchedule);
		int32_t							m_iID;
		ACT_ScheduleSaveEntryMap		m_kSchedules;
	};
	typedef mem::map<int32_t, ACT_ScheduleSaveCatagory> ACT_ScheduleSaveCatagoryMap;
	typedef mem::map<int32_t, int32_t> ACT_SchedulePersistentInfoMap;

	class ACT_ScheduleSaveActivity : public LC_DirtyFlagAble
	{
	public:
		ACT_ScheduleSaveActivity(void)
			:m_iType(0)
		{}

		inline void Init(int32_t iType)
		{
			m_iType = iType;
		}

		inline int32_t GetType(void) {return m_iType;}

		void Clear(void)
		{
			m_kCatagorys.clear();
			LC_DirtyFlagAble::Reset();
		}
		void ForceDirty(void);

		bool CheckReset(LC_ServerPlayer* pkPlayer, uint32_t timestamp);
		bool CheckReset(ACT_ScheduleEntry* pkSchedule, uint32_t timestamp, const GameLogic::LC_IDSet& rkIndices);
		bool ClearActData(uint32_t nScheduleId, const GameLogic::LC_IDSet& rkIndices);
		inline ACT_ScheduleSaveCatagory* GetScheduleSaveCatagory(int32_t iCatagory)
		{
			ACT_ScheduleSaveCatagoryMap::iterator it = m_kCatagorys.find(iCatagory);
			return (it!=m_kCatagorys.end()) ? &it->second : NULL;
		}

		void GetDataFromPB(const char_data_define::activity_schedule_info& info);
		void SetDataToPB(char_data_define::activity_schedule_info& info, bool all);

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
		int32_t							m_iType;
		ACT_ScheduleSaveCatagoryMap		m_kCatagorys;
		ACT_SchedulePersistentInfoMap   m_kPersistentInfo;
	};
	typedef mem::map<int32_t, ACT_ScheduleSaveActivity> ACT_ScheduleSaveActivityMap;

	class ACT_ScheduleAsset : public LC_DirtyFlagAble
	{
	public:
		ACT_ScheduleAsset(void)
			:m_pkOwner(NULL)
		{}

		inline void SetOwner(LC_ServerPlayer* pkPlayer){m_pkOwner = pkPlayer;}
		inline LC_ServerPlayer* GetOwner(void) {return m_pkOwner;}
		inline void Clear(void) 
		{
			m_kActivities.clear();
			LC_DirtyFlagAble::Reset();
		}
		void ForceDirty(void);

		void CheckReset(uint32_t timestamp);

		inline ACT_ScheduleSaveActivity* GetScheduleSaveActivity(int32_t iType)
		{
			ACT_ScheduleSaveActivityMap::iterator it = m_kActivities.find(iType);
			return (it!=m_kActivities.end()) ? &it->second : NULL;
		}

		void GetDataFromPB(const char_data_define::char_activity_info& info);
		bool SetDataToPB(char_data_define::char_activity_info& info, bool all);

		ACT_ScheduleSaveActivity* _addScheduleSaveActivity(int32_t iType);

		LC_ServerPlayer*				m_pkOwner;
		ACT_ScheduleSaveActivityMap		m_kActivities;
	};
}

#endif //__ACT_ACTIVITY_ASSET_H__
