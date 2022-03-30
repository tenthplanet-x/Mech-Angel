#ifndef __ACT_SHOT_H__
#define __ACT_SHOT_H__

#pragma once

#include "LC_Helper.h"

#include "ACT_ActivitySchedule.h"

#include "CF_ACTShot.h"
#include "CF_ACTShotA.h"

class LC_ServerPlayer;
namespace Activity
{
	class ACT_ShotDataEntry
	{
	public:
		ACT_ShotDataEntry(void)
			: m_iID(0), m_iSchdeule(0)
			, m_iRankType(0)
			, m_iKey(0), m_iSaveIndex(0)
			, m_bMailEnable(true)
		{}

		int32_t Catagory(void) const;
		int32_t Schedule(void) const;
		int32_t Key(void) const;

		template <typename DataSourceEntry>
		bool Init(DataSourceEntry* pkEntry)
		{
			if (NULL == pkEntry)
			{
				return false;
			}

			m_iID = pkEntry->_iID;
			m_iSchdeule = pkEntry->_iSchedule;
			m_iRankType = pkEntry->_iType;
			m_iKey = pkEntry->_iKey;
			m_iSaveIndex = pkEntry->_iSaveIndex;
			//m_kRewardItems.SetUp(pkEntry->_kReward);
			m_kRewardItems = pkEntry->_kReward;
			m_bMailEnable = pkEntry->_iMailEnable>0;
			m_sMailTitle = pkEntry->_sMailTitle;
			m_sMailDesc = pkEntry->_sMailDesc;
			return true;
		}

		ResultType CheckReward(LC_ServerPlayer* pkPlayer, ACT_ScheduleSaveEntry* pkEntry, GameLogic::LC_RewardItemsExtend& rkExtend);
		ResultType DoReward(LC_ServerPlayer* pkPlayer, ACT_ScheduleSaveEntry* pkEntry, GameLogic::LC_RewardItemsExtend& rkExtend);

		int32_t m_iID;
		int32_t m_iSchdeule;
		int32_t m_iRankType;
		int32_t m_iKey;
		int32_t m_iSaveIndex;
		UT_SIMDataList m_kRewardItems;
		//GameLogic::LC_ItemsReward m_kRewardItems;
		bool  m_bMailEnable;
		StringType m_sMailTitle;
		StringType m_sMailDesc;
	};

	typedef ACT_ScheduleDataActivity<ACT_ShotDataEntry, CSVFile::CF_ACTShot, CSVFile::CF_ACTShotA> ACT_ShotDataActivity;
	class ACT_ShotManager : public ACT_ShotDataActivity
	{
	public:
		ACT_ShotManager(void)
			:ACT_ShotDataActivity(ACT_SCHEDULE_TP_SHOT)
		{}

		void GetValidScheduleIDs(int32_t iServerID, int32_t iCatagory, GameLogic::LC_IDVector& rkScheduleIDs);
		virtual ResultType DrawReward(LC_ServerPlayer* pkPlayer, int32_t, int32_t, int32_t iShotID, GameLogic::LC_RewardItemsExtend& rkExtend, const GameLogic::LC_IDVector* pkExtraParams, int& count);
	};
}

#endif //__ACT_SHOT_H__
