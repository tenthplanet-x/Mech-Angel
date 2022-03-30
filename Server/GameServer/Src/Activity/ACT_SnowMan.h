#pragma  once
#include "LC_Helper.h"

#include "ACT_ActivityDataManager.h"
#include "ACT_ActivitySchedule.h"
#include "CF_ACTSnowMan.h"
#include "UT_TimeManager.h"
namespace Activity
{
	class ACT_SnowManManager;
	class ACT_SnowManDataEntry
	{
	public:
		ACT_SnowManDataEntry(void)
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
		int		m_iCondType;  
		int		m_iGroupSaveIndex;
		int		m_iSaveIndex;    
		int		m_iLevelLimit;   
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
			m_iCondType = pkEntry->_iCondType;
			m_iSaveIndex = pkEntry->_iSaveIndex;
			m_iGroupSaveIndex = pkEntry->_iGroupSaveIndex;
			m_iLevelLimit = pkEntry->_iLevelLimit;
			m_kReward.SetUp(pkEntry->_kReward);
			m_bMailEnable = (pkEntry->_iMailEnable > 0);
			m_sMailTitle = pkEntry->_sMailTitle;
			m_sMailDesc = pkEntry->_sMailDesc;
			return true;
		}
		ResultType DoReward(LC_ServerPlayer* pkPlayer, ACT_ScheduleSaveEntry* pkEntry, GameLogic::LC_RewardItemsExtend& rkExtend);
	};
	struct SnowManGroupScoreKey{
		SnowManGroupScoreKey() : iSchedule(0), iType(0)
		{}
		int iSchedule;
		int iType;
		bool operator < (const SnowManGroupScoreKey& rhs) const
		{
			if (iSchedule != rhs.iSchedule)
			{
				return iSchedule < rhs.iSchedule;
			}
			return iType < rhs.iType;
		}
	};
	class ACT_SnowManManager 
		: public ACT_ScheduleDataActivity<ACT_SnowManDataEntry, CSVFile::CF_ACTSnowMan>
	{
	public:
		ACT_SnowManManager(void)
			:ACT_ScheduleDataActivity<ACT_SnowManDataEntry, CSVFile::CF_ACTSnowMan>(ACT_SCHEDULE_TP_SNOW_MAN)
		{
		}
		virtual void SetUp(void);
		virtual void Update(float fCurrentTime);
		virtual ResultType DrawReward(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t iSchedule, int32_t iKey, GameLogic::LC_RewardItemsExtend& rkExtend, const GameLogic::LC_IDVector* pkExtraParams, int& count);
		ResultType DoUseItem(LC_ServerPlayer* pkPlayer, mem::vector<int32_t>&kParams);
		void AddScore(LC_ServerPlayer* pkPlayer, int32_t val, int32_t iAchieve, const mem::vector<int32_t>& kCatagorys);	
		void AddGroupScore(int iSchedule, int iType, int32_t val, StringType charName, int citizen, unique_id_impl charID);
		int32_t QueryGroupScore(LC_ServerPlayer* pkPlayer, int iSchedule, int iType);
	protected:
		int	 _getGroupScore(int iSchedule, int iType);
		void _requestGroupScore(int iSchedule, int iType);		
		void _onGetActSnowManGroupScore(int32_t err, const std::string& strScoreInfo, int iSchedule, int iType);
		void _onAddActSnowManGroupScore(int32_t err, const int64_t& iScore, int iSchedule, int iType, StringType charName, int citizen, unique_id_impl charID);
		Utility::UT_Timer m_kUpdateTimer;
		mem::map<SnowManGroupScoreKey, int> m_kGroupScoreMap;
	};

}