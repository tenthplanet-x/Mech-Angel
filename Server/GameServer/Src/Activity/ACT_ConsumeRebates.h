#ifndef __ACT_CONSUME_REBATES_H__
#define __ACT_CONSUME_REBATES_H__

#pragma once

#include "LC_Helper.h"

#include "ACT_ActivityDataManager.h"
#include "ACT_ActivitySchedule.h"

#include "CF_ACTConsumeRebates.h"
#include "CF_ACTConsumeRebatesA.h"

class LC_ServerPlayer;
namespace Activity
{
	class ACT_ConsumeRebatesDataEntry
	{
	public:
		ACT_ConsumeRebatesDataEntry(void)
			: m_iID(0)
			, m_iSchdeule(0)
			, m_iType(0)
			, m_iKey(0)
			, m_iSaveIndex(0)
			, m_bMailEnable(true)
		{}

		inline int32_t Catagory(void) const {return m_iType;}
		inline int32_t Schedule(void) const {return m_iSchdeule;}
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
			m_iType = pkEntry->_iType;
			m_iKey = pkEntry->_iKey;
			m_kProportions = pkEntry->_kProportions;
			m_iSaveIndex = pkEntry->_iSaveIndex;
			m_kRewardItems.SetUp(pkEntry->_kReward);
			m_bMailEnable = pkEntry->_iMailEnable>0;
			m_sMailTitle = pkEntry->_sMailTitle;
			m_sMailDesc = pkEntry->_sMailDesc;
			return true;
		}

		ResultType DoReward(LC_ServerPlayer* pkPlayer, ACT_ScheduleSaveEntry* pkEntry, GameLogic::LC_RewardItemsExtend& rkExtend);

		int32_t m_iID;
		int32_t m_iSchdeule;
		int32_t m_iType;
		int32_t m_iKey;
		Utility::UT_SIMDataList m_kProportions;
		int32_t m_iSaveIndex;
		GameLogic::LC_ItemsReward m_kRewardItems;
		bool  m_bMailEnable;
		StringType m_sMailTitle;
		StringType m_sMailDesc;
	};

	class ACT_ConsumeRebatesManager 
		: public ACT_ScheduleDataActivity<ACT_ConsumeRebatesDataEntry, CSVFile::CF_ACTConsumeRebates, CSVFile::CF_ACTConsumeRebatesA>
	{
	public:
		ACT_ConsumeRebatesManager(void)
			:ACT_ScheduleDataActivity<ACT_ConsumeRebatesDataEntry, CSVFile::CF_ACTConsumeRebates, CSVFile::CF_ACTConsumeRebatesA>(ACT_SCHEDULE_TP_CONSUME_REBATES)
		{}
		void AddConsume(LC_ServerPlayer* pkPlayer, int32_t val, const mem::vector<int32_t>& kCatagorys);
		virtual void OnConsume(LC_ServerPlayer* pkPlayer, int32_t val, const GameLogic::LC_IDSet* pkCatagorys);
		virtual ResultType DrawReward(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t iSchedule, int32_t iKey, GameLogic::LC_RewardItemsExtend& rkExtend, const GameLogic::LC_IDVector* pkExtraParams, int& count);
	};
}

#endif //__ACT_CONSUME_REBATES_H__
