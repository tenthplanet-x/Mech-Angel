#pragma  once
#include "LC_Helper.h"

#include "ACT_ActivityDataManager.h"
#include "ACT_ActivitySchedule.h"
#include "CF_ACTClimbTower.h"
namespace Activity
{
	class ACT_ClimbTowerManager;
	class ACT_ClimbTowerDataEntry
	{
	public:
		ACT_ClimbTowerDataEntry(void)
			:m_iID(0)
			,m_iSchedule(0)
			,m_iType(0)
			,m_iKey(0)
			,m_iTotalWeight(0)
			,m_iReplaceGold(0)
			,m_bMailEnable(false)
		{

		}
		int32_t	m_iID;
		int32_t	m_iSchedule;
		int		m_iType;
		int		m_iKey;
		int32_t	m_iTotalWeight;
		int64_t	m_iReplaceGold;
		CSVFile::TypeNumList m_kJumpWeight;
		Utility::UT_SIMDataList m_kConsumes;
		GameLogic::LC_ItemsReward m_kReward;
		bool	m_bMailEnable;
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
			m_iTotalWeight = 0;
			for (size_t i = 0; i < pkEntry->_kJumpWeight.size(); ++i)
			{
				m_iTotalWeight += pkEntry->_kJumpWeight.at(i).getNum();
			}
			m_kJumpWeight = pkEntry->_kJumpWeight;
			m_kConsumes = pkEntry->_kConsumes;
			m_iReplaceGold = pkEntry->_iReplaceGold;
			m_kReward.SetUp(pkEntry->_kReward);
			m_bMailEnable = pkEntry->_iMailEnable ? true : false;
			m_sMailTitle = pkEntry->_sMailTitle;
			m_sMailDesc = pkEntry->_sMailDesc;
			return true;
		}
		ResultType DoReward(LC_ServerPlayer* pkPlayer, ACT_ScheduleSaveEntry* pkEntry, GameLogic::LC_RewardItemsExtend& rkExtend, Utility::UT_SIMDataList& kRewards);
	};
	class ACT_ClimbTowerManager 
		: public ACT_ScheduleDataActivity<ACT_ClimbTowerDataEntry, CSVFile::CF_ACTClimbTower>
	{
	public:
		ACT_ClimbTowerManager(void)
			:ACT_ScheduleDataActivity<ACT_ClimbTowerDataEntry, CSVFile::CF_ACTClimbTower>(ACT_SCHEDULE_TP_CLIMB_TOWER)
		{}
		virtual ResultType RandomReward(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t iSchedule, int32_t iKey, GameLogic::LC_RewardItemsExtend& rkExtend, const GameLogic::LC_IDVector* pkExtraParams, Utility::UT_SIMDataList& kRewards);

		//for gm debug
		void AddClimbTowerCount(LC_ServerPlayer* pkPlayer,int32_t iAddCount);

	};

}
