#pragma  once
#include "LC_Helper.h"
#include "ACT_ActivityDataManager.h"
#include "ACT_ActivitySchedule.h"
#include "CF_ACTSpringTree.h"

namespace Activity
{
	class ACT_SpringTreeDataEntry
	{
	public:
		ACT_SpringTreeDataEntry(void)
			: m_iID(0)
			, m_iSchedule(0)
			, m_iType(0)
			, m_iKey(0)
			, m_iScoreLimit(0)
			, m_bMailEnable(false)
		{

		}
		int32_t	m_iID;   
		int32_t	m_iSchedule;   
		int		m_iType;    
		int		m_iKey;   
		int		m_iScoreLimit;  
		GameLogic::LC_ItemsReward	m_kReward;  
		bool  m_bMailEnable;
		StringType m_sMailTitle;
		StringType m_sMailDesc;

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
			m_iKey = pkEntry->_iKey;
			m_iScoreLimit = pkEntry->_iScoreLimit;
			m_kReward.SetUp(pkEntry->_kReward);
			m_bMailEnable = (pkEntry->_iMailEnable > 0);
			m_sMailTitle = pkEntry->_sMailTitle;
			m_sMailDesc = pkEntry->_sMailDesc;
			return true;
		}
		ResultType DoReward(LC_ServerPlayer* pkPlayer, ACT_ScheduleSaveEntry* pkEntry, GameLogic::LC_RewardItemsExtend& rkExtend);
	};
	enum 
	{
		E_TYPE_SCORE_CHARGE = 1,
		E_TYPE_SCORE_CONSUME = 2,
		E_TYPE_SCORE_ONLINE = 3,
	};
	class ACT_SpringTreeManager 
		: public ACT_ScheduleDataActivity<ACT_SpringTreeDataEntry, CSVFile::CF_ACTSpringTree>
	{
	public:
		ACT_SpringTreeManager(void)
			:ACT_ScheduleDataActivity<ACT_SpringTreeDataEntry, CSVFile::CF_ACTSpringTree>(ACT_SCHEDULE_TP_SPRING_TREE)
		{
		}
		virtual ResultType DrawReward(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t iSchedule, int32_t iKey, GameLogic::LC_RewardItemsExtend& rkExtend, const GameLogic::LC_IDVector* pkExtraParams, int& count);
		virtual void OnCharge(LC_ServerPlayer* pkPlayer, int32_t val, const GameLogic::LC_IDSet* pkCatagorys);
		virtual void OnConsume(LC_ServerPlayer* pkPlayer, int32_t val, const GameLogic::LC_IDSet* pkCatagorys);
		virtual void OnOnlineTime(LC_ServerPlayer* pkPlayer, int32_t val, const GameLogic::LC_IDSet* pkCatagorys);
		
		void AddScore(LC_ServerPlayer* pkPlayer, int32_t val,  const mem::vector<int32_t>& kCatagorys);
	protected:
		void _onAddScore(LC_ServerPlayer* pkPlayer, int iType, int32_t val, const GameLogic::LC_IDSet* pkCatagorys);
		
	};

}