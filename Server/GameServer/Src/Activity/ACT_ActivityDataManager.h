#ifndef __ACT_ACTIVITY_DATA_MANAGER_H__
#define __ACT_ACTIVITY_DATA_MANAGER_H__

#pragma once
#include "SystemError.h"
#include "LC_DirtyFlag.h"
#include "LC_Define.h"

#include "UT_Callback.h"
#include "UT_Timer.h"
#include "LC_Task.h"
#include "UT_ServerHelper.h"

#include "CSVDataSource.h"

namespace GameLogic
{
	struct LC_RewardItemsExtend;
}
class LC_ServerPlayer;
namespace Activity
{
	template <typename DataEntry>
	class ACT_ScheduleDataCatagory;

	template <typename DataEntry, typename DataSourceA, typename DataSourceB>
	class ACT_ScheduleDataActivity;

	class ACT_ScheduleEntry;

	template <typename DataEntry>
	class ACT_ScheduleDataEntry
	{
		friend class ACT_ScheduleDataCatagory<DataEntry>;
	public:
		typedef mem::map<int32_t, DataEntry>			DataEntryMap;
		typedef typename DataEntryMap::iterator			DataEntryMapIter;
		typedef typename DataEntryMap::const_iterator	DataEntryMapConstIter;
	public:
		ACT_ScheduleDataEntry(void)
			:m_iID(0)
		{}

		DataEntry* GetDataEntry(int32_t iID)
		{
			DataEntryMapIter it = m_kDatas.find(iID);
			return it!=m_kDatas.end() ? &it->second : NULL;
		}

		inline DataEntry* _addDataEntry(int32_t iID, const DataEntry& rkEntry)
		{
			m_kDatas[iID] = rkEntry;
			m_kKeys.push_back(iID);
			return &m_kDatas[iID];
		}
		inline const GameLogic::LC_IDVector& Keys(void) const {return m_kKeys;}

		inline const DataEntryMap& GetDataEntryMap(void)const { return m_kDatas; }

		int32_t					m_iID;
		GameLogic::LC_IDVector	m_kKeys;
		DataEntryMap			m_kDatas;
	};

	template <typename DataEntry>
	class ACT_ScheduleDataCatagory
	{
	public:
		typedef ACT_ScheduleDataEntry<DataEntry>							ScheduleDataType;
		typedef mem::map<int32_t, ScheduleDataType>							ScheduleDataMap;
		typedef typename ScheduleDataMap::iterator							ScheduleDataMapIter;
		typedef Utility::UT_SimOperator<bool, ScheduleDataType*>			ScheduleDataOper;
	public:
		ACT_ScheduleDataCatagory(void)
			:m_iCatagory(0)
		{}

		ScheduleDataType* GetScheduleData(int32_t iSchedule)
		{
			ScheduleDataMapIter it = m_kSchedules.find(iSchedule);
			return it!=m_kSchedules.end() ? &it->second : NULL;
		}

		DataEntry* GetDataEntry(int32_t iSchedule, int32_t iID)
		{
			ScheduleDataType* pkSchedule = GetScheduleData(iSchedule);
			return pkSchedule ? pkSchedule->GetDataEntry(iID) : NULL;
		}

		void ForAll(ScheduleDataOper* pkOper)
		{
			if (NULL == pkOper)
			{
				return;
			}
			ScheduleDataMapIter it = m_kSchedules.begin();
			for (; it!=m_kSchedules.end(); ++it)
			{
				(*pkOper)(&it->second);
			}
		}
	
		ScheduleDataType* _addScheduleData(int32_t iSchedule)
		{
			ScheduleDataMapIter it = m_kSchedules.find(iSchedule);
			if (it != m_kSchedules.end())
			{
				return &it->second;
			}
			m_kSchedules[iSchedule].m_iID = iSchedule;
			return &m_kSchedules[iSchedule];
		}

		inline DataEntry* _addDataEntry(int32_t iSchedule, int32_t iID, const DataEntry& rkEntry)
		{
			ScheduleDataType* pkSchedule = _addScheduleData(iSchedule);
			return pkSchedule->_addDataEntry(iID, rkEntry);
		}

		int32_t				m_iCatagory;
		ScheduleDataMap		m_kSchedules;
	};

	class ACT_ScheduleDataActivityBase : public Memory::MM_BaseObject
	{
	public:
		typedef mem::map<int32_t, GameLogic::LC_IDSet>		ScheduleIndices;

		ACT_ScheduleDataActivityBase(int32_t iType):m_iType(iType){}
		virtual ~ACT_ScheduleDataActivityBase(void)
		{
			Clear();	
		}

		virtual void SetUp(void)
		{
			return RegHooks();
		}

		virtual void InitConfig(CSVFile::CSVDataLoader* pkLoader, const CSVFile::CSVDataSourceList* pkCSVs) = 0;
		virtual void Clear(void)
		{
			UnregHooks();
			m_kScheduleIndices.clear();
		}

		virtual ResultType DoQuery(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t iSchedule, mem::vector<int32_t>& kReqpVals, mem::vector<int32_t>& kRespVals){ return RE_SUCCESS; }		
		virtual ResultType DrawReward(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t iSchedule, int32_t iKey, GameLogic::LC_RewardItemsExtend& rkExtend, const GameLogic::LC_IDVector* pkExtraParams, int& count){return RE_SUCCESS;}
		virtual ResultType RandomReward(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t iSchedule, int32_t iKey,
			GameLogic::LC_RewardItemsExtend& rkExtend, const GameLogic::LC_IDVector* pkExtraParams, Utility::UT_SIMDataList& kRewards){return RE_SUCCESS;}
		virtual ResultType DoBehaveSpec(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t iSchedule, mem::vector<int32_t>& kReqpVals, mem::vector<int32_t>& kRespVals){ return RE_SUCCESS; }
		virtual ResultType CheckData(LC_ServerPlayer* pkPlayer){return RE_SUCCESS;};
		virtual ResultType DrawLimitReward(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t iSchedule, int32_t iKey, GameLogic::LC_RewardItemsExtend& rkExtend, int count) {return RE_SUCCESS;}
		virtual void Update(float fCurrentTime){}
		//活动结束删除当前活动数据
		virtual ResultType ACTEndClearScheduleData(LC_ServerPlayer* pkPlayer, uint32_t iCatagory, uint32_t iSchedule) {return RE_SUCCESS; }

		inline int32_t GetType(void) {return m_iType;}
		inline void AddScheduleIndex(int32_t iSchedule, int32_t iCatagory)
		{
			m_kScheduleIndices[iSchedule].insert(iCatagory);
			m_kCatagoryIDs.insert(iCatagory);
		}
		inline const GameLogic::LC_IDSet* GetScheduleIndices(int32_t iSchedule) 
		{
			ScheduleIndices::iterator it = m_kScheduleIndices.find(iSchedule);
			return (it!=m_kScheduleIndices.end()) ? &it->second : NULL;
		}
		inline GameLogic::LC_IDSet& CatagoryIDs(void) {return m_kCatagoryIDs;}

		virtual void OnSubUpgrade(LC_ServerPlayer* pkPlayer, int32_t iSubType, int32_t iLevel, const GameLogic::LC_IDSet* pkCatagorys){};
		virtual void OnCharge(LC_ServerPlayer* pkPlayer, int32_t lValue, const GameLogic::LC_IDSet* pkCatagorys){};
		virtual void OnConsume(LC_ServerPlayer* pkPlayer, int32_t lValue, const GameLogic::LC_IDSet* pkCatagorys){};
		virtual void OnLogin(LC_ServerPlayer* pkPlayer, uint32_t timestamp, const GameLogic::LC_IDSet* pkCatagorys){};
		virtual void OnShop(LC_ServerPlayer* pkPlayer, int32_t lScore, const GameLogic::LC_IDSet* pkCatagorys){};
		virtual void OnSubordinateConsume(LC_ServerPlayer* pkPlayer, int32_t lValue, const GameLogic::LC_IDSet* pkCatagorys){};
		void RegHooks(void);
		void UnregHooks(void);

		int32_t				m_iType;
		ScheduleIndices		m_kScheduleIndices;
		GameLogic::LC_IDSet	m_kCatagoryIDs;
	};
	typedef mem::map<int32_t, ACT_ScheduleDataActivityBase*> ACT_ScheduleDataActivityMap;

	struct NullDataResource{};

	template <typename DT>
	class ActivityDataLoader
	{
	public:
		static inline bool IsValidReleated(const CSVFile::CSVDataSourceList* pkCSVs)
		{
			DT* inst = SafeGetSingleton<DT>();
			return IS_VALID_RELATED_CSV(inst, pkCSVs);
		}
		
		template <typename ACT>
		static void Load(ACT& rkActivity)
		{
			DT* inst = SafeGetSingleton<DT>();
			typename DT::DataEntryMapExternIterator iter = inst->GetIterator();
			while(iter.HasMoreElements())
			{
				typename DT::DataEntry* entry = iter.PeekNextValuePtr();
				typename ACT::DataEntryType kDataEntry;
				if (kDataEntry.Init(entry))
				{
					rkActivity._addDataEntry(kDataEntry);
				}
				iter.MoveNext();
			}
		}
	};

	template <>
	class ActivityDataLoader<NullDataResource>
	{
	public:
		static inline bool IsValidReleated(const CSVFile::CSVDataSourceList* pkCSVs)
		{
			return false;
		}

		template <typename ACT>
		static inline void Load(ACT& rkActivity){}
	};

	template <typename DataEntry, typename DataSourceA, typename DataSourceB=NullDataResource>
	class ACT_ScheduleDataActivity : public ACT_ScheduleDataActivityBase
	{
		template <typename DT>
		friend class ActivityDataLoader;
	public:
		typedef DataEntry											DataEntryType;							
		typedef ACT_ScheduleDataCatagory<DataEntryType>				CatagoryDataType;
		typedef mem::map<int32_t, CatagoryDataType>					CatagoryDataMap;
		typedef typename CatagoryDataMap::iterator					CatagoryDataMapIter;
		typedef typename CatagoryDataType::ScheduleDataOper			ScheduleDataOper;
		typedef ACT_ScheduleDataEntry<DataEntryType>				ScheduleDataType;

		ACT_ScheduleDataActivity(int32_t iType)
			:ACT_ScheduleDataActivityBase(iType)
		{}

		virtual void InitConfig(CSVFile::CSVDataLoader* pkLoader, const CSVFile::CSVDataSourceList* pkCSVs)
		{
			if(ActivityDataLoader<DataSourceA>::IsValidReleated(pkCSVs) || ActivityDataLoader<DataSourceB>::IsValidReleated(pkCSVs))
			{
				Clear();
				ActivityDataLoader<DataSourceA>::Load(*this);
				ActivityDataLoader<DataSourceB>::Load(*this);
				SetUp();
			}
		}

		virtual void Clear(void) 
		{
			m_kCatagorys.clear();
			ACT_ScheduleDataActivityBase::Clear();
		}

		CatagoryDataType* GetCatagoryData(int32_t iCatagory)
		{
			CatagoryDataMapIter it = m_kCatagorys.find(iCatagory);
			return it!=m_kCatagorys.end() ? &it->second : NULL;
		}

		DataEntryType* GetDataEntry(int32_t iCatagory, int32_t iSchedule, int32_t iID)
		{
			CatagoryDataType* pkCatagory = GetCatagoryData(iCatagory);
			return pkCatagory ? pkCatagory->GetDataEntry(iSchedule, iID) : NULL;
		}

		CatagoryDataMap&	CatagoryDatas(void) {return m_kCatagorys;}
	protected:
		DataEntryType* _addDataEntry(const DataEntryType& rkEntry)
		{
			CatagoryDataType* pkCatagory = _addCatagoryData(rkEntry.Catagory());
			DataEntry* pkEntry = pkCatagory ? pkCatagory->_addDataEntry(rkEntry.Schedule(), rkEntry.Key(), rkEntry) : NULL;
			if (pkEntry)
			{
				AddScheduleIndex(rkEntry.Schedule(), rkEntry.Catagory());
			}
			return pkEntry;
		}
		CatagoryDataType* _addCatagoryData(int32_t iCatagory)
		{
			CatagoryDataMapIter it = m_kCatagorys.find(iCatagory);
			if (it != m_kCatagorys.end())
			{
				return &it->second;
			}
			m_kCatagorys[iCatagory].m_iCatagory = iCatagory;
			return &m_kCatagorys[iCatagory];
		}
		
		CatagoryDataMap		m_kCatagorys;
	};


	template <typename DataEntry, typename DataSourceB=NullDataResource>
	class ACT_ScheduleEndSendMail
	{
	public:
		typedef DataEntry											DataEntryDef;							
		typedef std::vector<DataEntryDef*>							DataEntryVec;//
		typedef std::map<uint32_t,DataEntryVec>						DataEntryTypeMap;//
		typedef std::map<uint32_t, DataEntryTypeMap >				DataEntryTypeMaps;//

		/*ACT_ScheduleEndSendMail(int32_t iType):m_iScheduleEndType(iType){
		
		}*/
		ACT_ScheduleEndSendMail(){

		}
		virtual void InitiType(int32_t iType)
		{
			m_iScheduleEndType = iType;
		}

		void AddACTEndDataEntryType(DataEntryDef* rkEntry)
		{
			if(NULL == rkEntry )
			{
				return;
			}
			DataEntryTypeMap& dataMap = m_nDataEntryTypeMaps[rkEntry->ShowSchedule()];
			int key = GET_SCHEDULE_KEY(rkEntry->Schedule(), rkEntry->Catagory());
			DataEntryVec& v = dataMap[key];
			v.push_back(rkEntry);	
		}
		

		ResultType ACTScheduleEnd(uint32_t iSchedule, LC_ServerPlayer* pkPlayer) {
			if (NULL==pkPlayer) {
				return RE_FAIL;
			}

			DataEntryTypeMaps::iterator iter = m_nDataEntryTypeMaps.find(iSchedule);
			if(iter ==  m_nDataEntryTypeMaps.end()) {
				return RE_FAIL;
			}

			std::map<uint32_t, GameLogic::LC_IDSet> nClearActDataMaps;

			for(DataEntryTypeMap::iterator it = iter->second.begin(); it != iter->second.end();it++) {

				MailInfo mail;//
				int size = it->second.size();
				for(int i = 0; i < size; i++) {
					DataEntryDef* pkData = it->second[i];
					ACTScheduleEndSendMail(pkPlayer, pkData, mail);

					if(pkData) {
						GameLogic::LC_IDSet& idLists = nClearActDataMaps[pkData->Schedule()];
						idLists.insert(pkData->Catagory());
					}
				}
				UT_ServerHelper::SendMail(pkPlayer, mail);
			}
			ACTEndClearPlayerScheduleData(pkPlayer, nClearActDataMaps);
			return RE_SUCCESS;
		}

		virtual ResultType ACTScheduleEndSendMail(LC_ServerPlayer* pkPlayer, DataEntryDef* pkData, MailInfo& mail) {return RE_SUCCESS;}

		void LoginCheckACTScheduleEnd(LC_ServerPlayer* pkPlayer) {

			if (NULL==pkPlayer) return ;

			if(m_nDataEntryTypeMaps.size() <= 0) return ;

			std::map<uint32_t, GameLogic::LC_IDSet> nClearActDataMaps;
			static ACT_ScheduleManager* pkScheduleMgr = ACT_ScheduleManager::GetSingletonPtr();
			DataEntryTypeMaps::iterator iter = m_nDataEntryTypeMaps.begin();

			for(;iter !=  m_nDataEntryTypeMaps.end(); iter++) {
				if (pkScheduleMgr->IsScheduleValid(pkPlayer, iter->first)) {
					continue;
				}

				for(DataEntryTypeMap::iterator it = iter->second.begin(); it != iter->second.end();it++) {
					MailInfo mail;//
					int size = it->second.size();

					for(int i = 0; i < size; i++) {
						DataEntryDef* pkData = it->second[i];

						if(pkData) {
							GameLogic::LC_IDSet& idLists = nClearActDataMaps[pkData->Schedule()];
							idLists.insert(pkData->Catagory());
						}
						ACTScheduleEndSendMail(pkPlayer, pkData, mail);
					}
					UT_ServerHelper::SendMail(pkPlayer, mail);
				}
				ACTEndClearPlayerScheduleData(pkPlayer, nClearActDataMaps);
			}
		}

		void ACTEndClearPlayerScheduleData(LC_ServerPlayer* pkPlayer, std::map<uint32_t, GameLogic::LC_IDSet>& ClearActDataMaps)
		{
			if(NULL == pkPlayer)
			{
				return;
			}
			pkPlayer->ClearScheduleData(m_iScheduleEndType, ClearActDataMaps);
		}

	protected:
		int32_t								m_iScheduleEndType;
		DataEntryTypeMaps                   m_nDataEntryTypeMaps;//
	};

	typedef mem::map<int32_t, GameLogic::LC_IDSet> ACT_Type2CatasMap;
	inline const GameLogic::LC_IDSet* GetActCatagorys(const ACT_Type2CatasMap& rkActivitys, int32_t iType)
	{
		ACT_Type2CatasMap::const_iterator cit = rkActivitys.find(iType);
		return cit!=rkActivitys.end() ? &cit->second : NULL;
	}

	class ACT_ScheduleDataManager: public Memory::MM_BaseObject
		,public Utility::UT_Singleton<ACT_ScheduleDataManager>
	{
	public:
		ACT_ScheduleDataManager(){}

		void Init(void);
		void Unit(void);

		ResultType InitConfig(CSVFile::CSVDataLoader* pkLoader=NULL, const CSVFile::CSVDataSourceList* pkCSVs=NULL);

		ResultType CheckData(LC_ServerPlayer* pkPlayer);

		void Update(float fCurrentTime);

		bool ActBegin( int nSchedID,  time_t nTimeStamp );
		bool ActGet(int nSchedID, time_t nTimeStamp, mem::map<int, int>& resultSub, mem::map<int, BossFirstKillPlayerInfo>& resultBoss);

		bool ActBeginB( int nSchedID, int serverID, time_t nTimeStamp );
		bool ActGetB(int nSchedID, time_t nTimeStamp, mem::map<int, int>& resultSub);

		void OnSubUpgrade(LC_ServerPlayer* pkPlayer, int32_t iSubType, int32_t iLevel, const ACT_Type2CatasMap* pkActivitys=NULL);
		void OnCharge(LC_ServerPlayer* pkPlayer, uint32_t lValue, const ACT_Type2CatasMap* pkActivitys=NULL);
		void NewOnCharge(LC_ServerPlayer* pkPlayer,uint32_t nValueType, uint32_t lValue, const ACT_Type2CatasMap* pkActivitys=NULL);
		void OnConsume(LC_ServerPlayer* pkPlayer, uint32_t lValue, const ACT_Type2CatasMap* pkActivitys=NULL);
		void OnLogin(LC_ServerPlayer* pkPlayer, uint32_t timestamp, const ACT_Type2CatasMap* pkActivitys=NULL);
		void OnShop(LC_ServerPlayer* pkPlayer, int32_t lScore, const ACT_Type2CatasMap* pkActivitys=NULL);
		void OnTaskFinished(LC_ServerPlayer* pkPlayer, GameLogic::LC_Task* pkTask, const ACT_Type2CatasMap* pkActivitys=NULL);
		void OnOnlineTime(LC_ServerPlayer* pkPlayer, uint32_t lValue, const ACT_Type2CatasMap* pkActivitys = NULL);
		void OnSubordinateConsume(LC_ServerPlayer* pkPlayer, int32_t iSubType, int iPayType, int32_t iValue, const ACT_Type2CatasMap* pkActivitys = NULL);

		void AddScore(LC_ServerPlayer* pkPlayer, int32_t iType, int32_t iVal, const mem::vector<int32_t>& kCatagorys);
		void OfferScore(LC_ServerPlayer* pkPlayer, int32_t lScore, const ACT_Type2CatasMap& rkActivitys);
		void OnFinishTaskAndAchievement(LC_ServerPlayer* pkPlayer,  int32_t nType, int32_t nFinishId, const ACT_Type2CatasMap* pkActivitys=NULL);
		inline ACT_ScheduleDataActivityBase* GetActivity(int32_t iType)
		{
			ACT_ScheduleDataActivityMap::iterator it = m_kActivities.find(iType);
			return (it!=m_kActivities.end()) ? it->second : NULL;
		}

		bool _addActivity(ACT_ScheduleDataActivityBase* pkActivity);

		void _onScheduleActived(Utility::timer_id_type timerID, ACT_ScheduleEntry* pkSchedule);
		void _onScheduleDeactived(Utility::timer_id_type timerID, ACT_ScheduleEntry* pkSchedule);
		void ACTScheduleEnd(uint32_t iSchedule, LC_ServerPlayer* pkPlayer);
		void LoginCheckACTScheduleEnd(LC_ServerPlayer* pkPlayer);

		ACT_ScheduleDataActivityMap		m_kActivities;
	};
}

#endif //__ACT_ACTIVITY_DATA_MANAGER_H__
