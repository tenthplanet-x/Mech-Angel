#ifndef __ACT_CHARGE_PROFIT_H__
#define __ACT_CHARGE_PROFIT_H__

#pragma once

#include "LC_Helper.h"

#include "ACT_ActivityDataManager.h"
#include "ACT_ActivitySchedule.h"

#include "CF_ACTChargeProfit.h"
#include "CF_ACTChargeProfitA.h"

class LC_ServerPlayer;
namespace Activity
{
	class ACT_ChargeProfitDataEntry
	{
	public:
		ACT_ChargeProfitDataEntry(void)
			: m_iID(0)
			, m_iSchdeule(0)
			, m_iType(0)
			, m_iKey(0)
			, m_iSaveIndex(0)
			, m_iTimeLimits(0)
			, m_bMailEnable(true)
			, m_iShowSchdeule(0)
		{}

		inline int32_t Catagory(void) const {return m_iType;}
		inline int32_t Schedule(void) const {return m_iSchdeule;}
		inline int32_t ShowSchedule(void) const {return m_iShowSchdeule;}
		inline int32_t Key(void) const {return m_iKey;}

		template <typename DataSourceEntry>
		bool Init(DataSourceEntry* pkEntry)
		{
			if (NULL == pkEntry)
			{
				return false;
			}

			m_iID = pkEntry->_iID;
			m_iSchdeule = pkEntry->_iSchedule;
			m_iShowSchdeule = pkEntry->_iScheduleShow;
			m_iType = pkEntry->_iType;
			m_iKey = pkEntry->_iKey;
			m_iSaveIndex = pkEntry->_iSaveIndex;
			m_iTimeLimits = pkEntry->_iTimeLimits;
			m_kRewardItems = pkEntry->_kReward;
			m_bMailEnable = pkEntry->_iMailEnable>0;
			m_sMailTitle = pkEntry->_sMailTitle;
			m_sMailDesc = pkEntry->_sMailDesc;
			return true;
		}

		ResultType DoReward(LC_ServerPlayer* pkPlayer, GameLogic::LC_RewardItemsExtend& rkExtend);

		int32_t m_iID;
		int32_t m_iSchdeule;
		int32_t m_iShowSchdeule;
		int32_t m_iType;
		int32_t m_iKey;
		int32_t m_iTimeLimits;
		int32_t m_iSaveIndex;
		Utility::UT_SIMDataList m_kRewardItems;
		bool  m_bMailEnable;
		StringType m_sMailTitle;
		StringType m_sMailDesc;
	};

	class ACT_ChargeProfitManager 
		: public ACT_ScheduleDataActivity<ACT_ChargeProfitDataEntry, CSVFile::CF_ACTChargeProfitA>,public ACT_ScheduleEndSendMail<ACT_ChargeProfitDataEntry>
	{
	public:
		ACT_ChargeProfitManager(void)
			:ACT_ScheduleDataActivity<ACT_ChargeProfitDataEntry, CSVFile::CF_ACTChargeProfitA>(ACT_SCHEDULE_TP_CHARGE)
			/*,ACT_ScheduleEndSendMail<ACT_ChargeProfitDataEntry>(ACT_SCHEDULE_TP_CHARGE)*/
		{}
		void AddCharge(LC_ServerPlayer* pkPlayer, int32_t val, const mem::vector<int32_t>& kCatagorys);
		virtual void OnCharge(LC_ServerPlayer* pkPlayer, int32_t val, const GameLogic::LC_IDSet* pkCatagorys);
		//新的充值接口
		virtual void NewOnCharge(LC_ServerPlayer* pkPlayer, int32_t nValueType, int32_t val, const GameLogic::LC_IDSet* pkCatagorys);
		virtual ResultType DrawReward(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t iSchedule, int32_t iKey, GameLogic::LC_RewardItemsExtend& rkExtend, const GameLogic::LC_IDVector* pkExtraParams, int& count);
		void InitConfig(CSVFile::CSVDataLoader* pkLoader, const CSVFile::CSVDataSourceList* pkCSVs);
		
		ResultType CheckScheduleData(LC_ServerPlayer* pkPlayer, DataEntryType* pkDataEntry, int32_t iCatagory, int32_t iSchedule, int32_t iKey);
		ResultType EndDoReward(LC_ServerPlayer* pkPlayer, DataEntryType* pkDataEntry, int32_t iCatagory, int32_t iSchedule, int32_t iKey);
		//

		virtual ResultType ACTScheduleEndSendMail(LC_ServerPlayer* pkPlayer, DataEntryType* pkData, MailInfo& mail);
	};
}

#endif //__ACT_CHARGE_PROFIT_H__
