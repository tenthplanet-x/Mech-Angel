#ifndef __ACT_SUBORDINATE_H__
#define __ACT_SUBORDINATE_H__

#pragma once

#include "LC_Helper.h"

#include "ACT_ActivitySchedule.h"

#include "CF_ACTSubordinateReward.h"
#include "CF_ACTSubordinateRewardA.h"
#include "CF_ACTSubordinateRewardB.h"

class LC_ServerPlayer;
namespace Activity
{
	class ACT_SubordinateRewardDataEntry
	{
	public:
		ACT_SubordinateRewardDataEntry(void)
			: m_iID(0)
			, m_iSchdeule(0)
			, m_iType(0)
			, m_iLevel(0)
			, m_iSaveIndex(0)
			, m_bMailEnable(true)
		{}

		inline int32_t Catagory(void) const {return m_iType;}
		inline int32_t Schedule(void) const {return m_iSchdeule;}
		inline int32_t Key(void) const {return m_iLevel;}

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
			m_iLevel = pkEntry->_iLevel;
			m_iSaveIndex = pkEntry->_iSaveIndex;
			m_kRewardItems.SetUp(pkEntry->_kReward);
			m_bMailEnable = pkEntry->_iMailEnable>0;
			m_sMailTitle = pkEntry->_sMailTitle;
			m_sMailDesc = pkEntry->_sMailDesc;
			return true;
		}

		ResultType DoReward(LC_ServerPlayer* pkPlayer, ACT_ScheduleSaveEntry* pkEntry, GameLogic::LC_RewardItemsExtend& rkExtend, int nLogCode = 0, const vector<int>& nParams = vector<int>());

		int32_t m_iID;
		int32_t m_iSchdeule;
		int32_t m_iType;
		int32_t m_iLevel;
		int32_t m_iSaveIndex;
		GameLogic::LC_ItemsReward m_kRewardItems;
		bool  m_bMailEnable;
		StringType m_sMailTitle;
		StringType m_sMailDesc;
	};

	class ACT_SubordinateManager 
		: public ACT_ScheduleDataActivity<ACT_SubordinateRewardDataEntry, CSVFile::CF_ACTSubordinateReward, CSVFile::CF_ACTSubordinateRewardA>
	{
	public:
		ACT_SubordinateManager(void)
			:ACT_ScheduleDataActivity<ACT_SubordinateRewardDataEntry, CSVFile::CF_ACTSubordinateReward, CSVFile::CF_ACTSubordinateRewardA>(ACT_SCHEDULE_TP_SUBORDINATE)
		{}
	
		virtual void OnSubUpgrade(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t val, const GameLogic::LC_IDSet* pkCatagorys);
		virtual ResultType DrawReward(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t iSchedule, int32_t iKey, GameLogic::LC_RewardItemsExtend& rkExtend, const GameLogic::LC_IDVector* pkExtraParams, int& count);
		virtual ResultType DrawLimitReward(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t iSchedule, int32_t iKey, GameLogic::LC_RewardItemsExtend& rkExtend, int count);
	};


	class ACT_SubordinateManagerB 
		: public ACT_ScheduleDataActivity<ACT_SubordinateRewardDataEntry, CSVFile::CF_ACTSubordinateRewardB, CSVFile::CF_ACTSubordinateRewardB>
	{
	public:
		ACT_SubordinateManagerB(void)
			:ACT_ScheduleDataActivity<ACT_SubordinateRewardDataEntry, CSVFile::CF_ACTSubordinateRewardB, CSVFile::CF_ACTSubordinateRewardB>(ACT_SCHEDULE_TP_SUBORDINATE_B)
		{}

		virtual ResultType DrawReward(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t iSchedule, int32_t iKey, GameLogic::LC_RewardItemsExtend& rkExtend, const GameLogic::LC_IDVector* pkExtraParams, int& count);
		virtual ResultType DrawLimitReward(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t iSchedule, int32_t iKey, GameLogic::LC_RewardItemsExtend& rkExtend, int count);
	};
}

#endif //__ACT_SUBORDINATE_H__
