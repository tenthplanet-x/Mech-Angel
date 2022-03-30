#pragma  once
#include "LC_Helper.h"

#include "ACT_ActivityDataManager.h"
#include "ACT_ActivitySchedule.h"
#include "CF_ACTSubordinateConsume.h"
namespace Activity
{
	class ACT_SubordinateConsumeManager;
	class ACT_SubordinateConsumeDataEntry
	{
	public:
		ACT_SubordinateConsumeDataEntry(void)
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
		int		m_iFloor;
		int		m_iFloorStep;
		mem::vector<int>			m_kForwardRate;
		int		m_iAddRate;
		mem::vector<int>			m_kVipProfit;
		int		m_iActScore;
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
			m_iKey = pkEntry->_iKey + (pkEntry->_iFloor - 1)*pkEntry->_iFloorStep;
			m_iFloor = pkEntry->_iFloor;
			m_iFloorStep = pkEntry->_iFloorStep;
			m_kForwardRate = pkEntry->_kForwardRate;
			m_iAddRate = pkEntry->_iAddRate;
			m_kVipProfit = pkEntry->_kVipProfit;
			m_iActScore = pkEntry->_iActScore;
			m_kReward = pkEntry->_kReward;
			m_kMailTitle = pkEntry->_sMailTitle;
			m_kMailDesc = pkEntry->_sMailDesc;
			return true;
		}
	};
	enum SubordinateCateType
	{
		E_SUBORDINATE_CONSUME  = 1,
		E_SUBORDINATE_RECHARGE = 2,
	};
	class ACT_SubordinateConsumeManager 
		: public ACT_ScheduleDataActivity<ACT_SubordinateConsumeDataEntry, CSVFile::CF_ACTSubordinateConsume>
	{
	public:
		ACT_SubordinateConsumeManager(void)
			:ACT_ScheduleDataActivity<ACT_SubordinateConsumeDataEntry, CSVFile::CF_ACTSubordinateConsume>(ACT_SCHEDULE_TP_SUBORDINATE_CONSUME)
		{}
		virtual void OnSubordinateConsume(LC_ServerPlayer* pkPlayer, int32_t iSubType, int32_t iValue, const GameLogic::LC_IDSet* pkCatagorys =NULL);
		virtual void OnCharge(LC_ServerPlayer* pkPlayer, int32_t val, const GameLogic::LC_IDSet* pkCatagorys);
		virtual ResultType DoBehaveSpec(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t iSchedule, mem::vector<int32_t>& kReqpVals, mem::vector<int32_t>& kRespVals);
		void AddDrawCount(LC_ServerPlayer* pkPlayer, int32_t iValue, const mem::vector<int32_t>& kCatagorys);
	};

}
