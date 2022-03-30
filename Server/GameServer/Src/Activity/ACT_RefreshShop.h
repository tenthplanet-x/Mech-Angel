#pragma  once
#include "LC_Helper.h"
#include "ACT_ActivityDataManager.h"
#include "ACT_ActivitySchedule.h"
#include "CF_ACTRefreshShop.h"
namespace Activity
{
	class ACT_RefreshShopDataEntry
	{
	public:
		ACT_RefreshShopDataEntry(void)
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
		int		m_iGrade;
		int		m_iRefreshRate;
		int		m_iRefreshCount;
		int		m_iLimitRefresh;
		mem::vector<int32_t> m_iRefreshCost;
		int			m_iPrice;
		int			m_iLimitBuy;
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
			m_iGrade = pkEntry->_iGrade;
			m_iRefreshRate = pkEntry->_iRefreshRate;
			m_iRefreshCount = pkEntry->_iRefreshCount;
			m_iLimitRefresh = pkEntry->_iLimitRefresh;
			m_iRefreshCost = pkEntry->_iRefreshCost;
			m_iPrice = pkEntry->_iPrice;
			m_iLimitBuy = pkEntry->_iLimitBuy;
			m_kReward.SetUp(pkEntry->_kReward);
			m_bMailEnable = (pkEntry->_iMailEnable > 0);
			m_sMailTitle = pkEntry->_sMailTitle;
			m_sMailDesc = pkEntry->_sMailDesc;
			return true;
		}
		ResultType DoReward(LC_ServerPlayer* pkPlayer, ACT_ScheduleSaveEntry* pkEntry, GameLogic::LC_RewardItemsExtend& rkExtend);
	};
	class ACT_RefreshShopManager
		: public ACT_ScheduleDataActivity<ACT_RefreshShopDataEntry, CSVFile::CF_ACTRefreshShop>
	{
	public:
		ACT_RefreshShopManager(void)
			:ACT_ScheduleDataActivity<ACT_RefreshShopDataEntry, CSVFile::CF_ACTRefreshShop>(ACT_SCHEDULE_TP_REFRSH_SHOP)
		{
		}
		virtual ResultType DrawReward(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t iSchedule, int32_t iKey, GameLogic::LC_RewardItemsExtend& rkExtend, const GameLogic::LC_IDVector* pkExtraParams, int& count);	
		virtual ResultType DoQuery(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t iSchedule, mem::vector<int32_t>& kReqpVals, mem::vector<int32_t>& kRespVals);
		//手动刷新商店
		virtual ResultType DoBehaveSpec(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t iSchedule, mem::vector<int32_t>& kReqpVals, mem::vector<int32_t>& kRespVals);

	protected:
		ResultType _refreshShop(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t iSchedule, mem::vector<int32_t>& kItemVec);
	};

}