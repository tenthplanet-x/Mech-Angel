#ifndef LC_RUMOR_H
#define LC_RUMOR_H

#include "StringDefine.h"
#include "LC_PackStruct.h"
#include "CF_SkillExecuteList.h"
#include "LC_DirtyFlag.h"
#include "redis.pb.h"

namespace GameLogic
{
	enum RumorTriggerType
	{
		NoTriggerType = -1,
		SingleNoTriggerTypeType,
		ConditioNoTriggerTypenType
	};

	////////////////////////////////////////////////////////////
	struct LC_RumorKey
	{
		enum
		{
			RUMER_KEY_COUNT = 2,
		};

		LC_RumorKey() { memset(kData, 0, sizeof(kData));}
		inline bool operator < (const LC_RumorKey& o) const { return *((uint64_t*)kData) < *((uint64_t*)o.kData);}
		inline int32_t operator [](int i) const { return kData[i];};
		inline int32_t& operator [](int i) { return kData[i]; };

		int32_t kData[RUMER_KEY_COUNT];
	};
	typedef mem::vector<LC_RumorKey> LC_RumorKeyVector;
	////////////////////////////////////////////////////////////
	struct LC_RumorDataEntry
	{
		LC_RumorDataEntry(void): iGrade(INF_SPH_TP_SERVER) {}

		inline bool IsGroup(void) const {return INF_SPH_TP_GROUP==iGrade;}
		inline bool IsUnion(void) const { return INF_SPH_TP_UNION==iGrade;}

		int32_t iGrade;
		LC_RumorKey kKey;
		int32_t noticeID;
	};
	typedef mem::map<LC_RumorKey, LC_RumorDataEntry> LC_RumorDataEntryMap;
	////////////////////////////////////////////////////////////
	struct LC_RumorRecord
	{
		LC_RumorRecord(void)
		{
			memset(uiMaxCount, 0, sizeof(uiMaxCount));
		}
		uint32_t uiMaxCount[INF_SPH_TP_MAX];
	};
	////////////////////////////////////////////////////////////
	class LC_Rumor
	{
	public:
		typedef mem::map<int32_t, LC_RumorKeyVector> RumorKeyIndices;
		LC_Rumor();
		virtual ~LC_Rumor() {};

		void SetUP(void);

		void SetRumorType(RumorType type);
		int  GetRumorType() {return m_iRumorType;};
		void SetTriggerType(RumorTriggerType type);
		void SetChatChannel(int iChannel);
		int GetChatChannel(void){return m_iChatChannel;}

		void AddRumerEntry(const LC_RumorDataEntry& kEntry) { m_kRumerEntryMap[kEntry.kKey] = kEntry; }
		const LC_RumorDataEntry* GetRumerEntry(int32_t iKey, int32_t iDetail = 0);
		const LC_RumorDataEntry* GetRumerEntryBetween(int32_t iKey, int32_t iDetailMin, int32_t iDetailMax);
		const LC_RumorDataEntry* GetRumerEntry(const LC_RumorKey& kKey) const
		{
			LC_RumorDataEntryMap::const_iterator cit = m_kRumerEntryMap.find(kKey);
			if (cit != m_kRumerEntryMap.end())
			{
				return &cit->second;
			}
			return NULL;
		}

		inline const LC_RumorRecord& GetRecord(void) const { return m_kRecord;}

		inline void SetRecord(int32_t iMaxU, int32_t iMaxG, int32_t iMaxL)
		{
			m_kRecord.uiMaxCount[INF_SPH_TP_UNION] = iMaxU;
			m_kRecord.uiMaxCount[INF_SPH_TP_GROUP] = iMaxG;
			m_kRecord.uiMaxCount[INF_SPH_TP_SERVER] = iMaxL;
		}

		uint32_t GetRumorLimit(int32_t iGrade);

		int GetRumorID(const LC_RumorKey& key);

		typedef mem::map<int, StringType> ConditionMap;
		typedef mem::map<int, StringType>::iterator ConditionMapIter;
	protected:
	private:
		int m_iRumorType;
		int m_iTriggerType;
		int m_iChatChannel;
		LC_RumorRecord m_kRecord;
		RumorKeyIndices m_kKeyIndices;
		LC_RumorDataEntryMap m_kRumerEntryMap;
	};
	typedef mem::map<int32_t, LC_Rumor> LC_RumorMap;
	////////////////////////////////////////////////////////////
	class LC_RumorLog : public LC_DirtyFlagAble
	{
	public:
		LC_RumorLog(void): m_uiTimestamp(0) {}

		inline bool operator < (const LC_RumorLog& o) const {return m_uiTimestamp!=o.m_uiTimestamp ? (m_uiTimestamp<o.m_uiTimestamp) : (m_sInfo.compare(0, stMaxLogCompareSize, o.m_sInfo, 0, stMaxLogCompareSize) < 0);}
		inline bool operator == (const LC_RumorLog& o) const {return m_uiTimestamp == o.m_uiTimestamp && m_sInfo.compare(0, stMaxLogCompareSize, o.m_sInfo, 0, stMaxLogCompareSize) == 0;}

		void SetDataToProtoclStruct(Protocol::PS_RumorLog& rkStruct);
		void GetDataFromProtocolStruct(Protocol::PS_RumorLog& rkStruct);

		uint32_t		m_uiTimestamp;
		std::string		m_sInfo;

		static const int32_t stMaxLogCompareSize = 20;
	};
	typedef mem::list<LC_RumorLog> LC_RumorLogList;
	////////////////////////////////////////////////////////////
	class LC_RumorLogInfo: public LC_DirtyFlagAble
	{
	public:
		LC_RumorLogInfo(): m_iGrade(0), m_iType(0), m_uiLimit(0) {}

		bool AddLog(const LC_RumorLog& kLog);

		void SetDataToProtoclStruct(Protocol::PS_RumorLogInfo& rkStruct, bool all);
		void GetDataFromProtocolStruct(Protocol::PS_RumorLogInfo& rkStruct);

		//bool GetDataFromPB(const redis_define::rumor_info& info, GameLogic::LC_Rumor* pkRumer);
		//void SetDataToPB(redis_define::rumor_info& info);
		void SyncRumor(void);

		virtual void OnSync(int32_t err, std::vector<std::string>& infos);

		int32_t m_iGrade;
		int32_t m_iType;
		uint32_t m_uiLimit;
		LC_RumorLogList m_kRumors;
	};
	typedef mem::map<int32_t, LC_RumorLogInfo> LC_RumorLogInfoMap;
}

#endif
