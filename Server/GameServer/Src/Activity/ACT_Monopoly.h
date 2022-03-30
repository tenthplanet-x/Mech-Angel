#pragma  once
#include "LC_Helper.h"

#include "ACT_ActivityDataManager.h"
#include "ACT_ActivitySchedule.h"
#include "CF_ACTMonopoly.h"
#include "CF_ACTMonopolyA.h"
namespace Activity
{
	class ACT_MonopolyManager;
	enum
	{
		E_MONOPOLY_YUANBAO_COST = 50,
		E_MONOPOLY_POINT_MIN = 1,
		E_MONOPOLY_POINT_MAX = 6,
		E_MONOPOLY_MAX_FLOOR_STEP = 65,
		E_MONOPOLY_MAX_FLOOR = 5,
	};
	class ACT_MonopolyDataEntry
	{
	public:
		ACT_MonopolyDataEntry(void)
			:m_iID(0)
			,m_iSchedule(0)
			,m_iType(0)
			,m_iKey(0)
		{

		}
		int32_t	m_iID;   
		int32_t	m_iSchedule;   
		int		m_iType;    
		int		m_iKey;   
		Utility::UT_SIMDataList	m_kReward;    
		StringType m_kMailTitle;
		StringType m_kMailDesc;

		inline int32_t Catagory(void) const {return m_iType;}
		inline int32_t Schedule(void) const {return m_iSchedule;}
		inline int32_t Key(void) const {return m_iKey;}

		template <typename DataSourceEntry>
		bool Init(DataSourceEntry* pkEntry)
		{
			if (NULL == pkEntry)
			{
				return false;
			}
			m_iID = pkEntry->_iID;
			m_iSchedule = pkEntry->_iSchedule;
			m_iType = pkEntry->_iType;
			m_iKey = pkEntry->_iKey + (pkEntry->_iFloor-1)*E_MONOPOLY_MAX_FLOOR_STEP;
			m_kReward = pkEntry->_kReward;
			m_kMailTitle = pkEntry->_sMailTitle;
			m_kMailDesc = pkEntry->_sMailDesc;
			return true;
		}
	};
	class ACT_MonopolyManager 
		: public ACT_ScheduleDataActivity<ACT_MonopolyDataEntry, CSVFile::CF_ACTMonopoly, CSVFile::CF_ACTMonopolyA>
	{
	public:
		ACT_MonopolyManager(void)
			:ACT_ScheduleDataActivity<ACT_MonopolyDataEntry, CSVFile::CF_ACTMonopoly, CSVFile::CF_ACTMonopolyA>(ACT_SCHEDULE_TP_MONOPOLY)
		{}
		virtual void OnTaskFinished(LC_ServerPlayer* pkPlayer, GameLogic::LC_Task* pkTask, const GameLogic::LC_IDSet* pkCatagorys =NULL);
		virtual ResultType MonopolyForward(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t iSchedule, bool bUseYuanbao, int32_t& iRetMoveStep);

		//for gm debug
		void AddMonopolyCount(LC_ServerPlayer* pkPlayer,int32_t iAddCount);

	};

}