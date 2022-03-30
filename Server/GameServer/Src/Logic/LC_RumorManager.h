#ifndef LC_RUMORMANAGER_H
#define LC_RUMORMANAGER_H

#include "LC_Rumor.h"
#include "MM_BaseObject.h"
#include "UT_Singleton.h"
#include "MG_SyncProtocol.h"
#include "UT_TimeManager.h"

#define RUMOR_CONFIG "./Data/Script/Logic/Rumor/RumorPoint.lua"

class LC_ServerPlayer;

namespace GameLogic
{
	class LC_RumorGrade : public LC_DirtyFlagAble
	{
	public:
		LC_RumorGrade(void) : m_iGrade(0) {}
		
		LC_RumorLogInfo*	GetRumorLogInfo(int32_t iType);
		void				Update(void);

		int32_t SetDataToProtocolStruct(Protocol::PS_RumorLogInfos& rkStruct, bool all=false);
		void GetDataFromProtocolStruct(Protocol::PS_RumorLogInfos& rkStruct);

		int32_t				m_iGrade;
		LC_RumorLogInfoMap	m_kRumorLogs;
	};

	class LC_RumorManager : public Memory::MM_BaseObject, public Utility::UT_Singleton<LC_RumorManager>
	{
	public:
		LC_RumorManager();
		virtual ~LC_RumorManager();
		void ReadConfigFile();

		LC_Rumor* GetRumorByType(int32_t type);

		LC_RumorGrade* GetRumorGrade(int32_t iGrade);

		LC_RumorLogInfo* GetRumorLogInfo(int32_t iType, int32_t iRumorGrade);

		void OnGameServerLoaded(void);
		void Update(float ftime);
		
		void AddLog(int32_t iType, const LC_RumorLog& kLog, LC_RumorDataEntry* pkRumorDataEntry);

		void SyncRumorLogs(LC_ServerPlayer* pkPlayer);

		int32_t SetDataToProtocolStruct(Protocol::MG_SyncRumorLogsInfo& rkStruct, bool all=false);
		void GetDataFromProtocolStruct(Protocol::MG_SyncRumorLogsInfo& rkStruct);

	protected:
	private:
		LC_RumorMap m_kRumorMap;
		
		LC_RumorGrade m_kRumorGrades[INF_SPH_TP_MAX];

		Utility::UT_Timer m_kUpdateTimer;
	};
}

#endif
