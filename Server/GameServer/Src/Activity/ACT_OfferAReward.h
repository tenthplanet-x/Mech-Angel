#ifndef __ACT_OFFER_A_REWARD_H__
#define __ACT_OFFER_A_REWARD_H__

#pragma once

#include "LC_Helper.h"

#include "ACT_ActivityDataManager.h"
#include "ACT_ActivitySchedule.h"

#include "CF_ACTOfferAReward.h"

class LC_ServerPlayer;
namespace Activity
{
	class ACT_OfferARewardDataEntry
	{
	public:
		ACT_OfferARewardDataEntry(void)
			: m_iID(0)
			, m_iSchdeule(0)
			, m_iType(0)
			, m_iKey(0)
			, m_iSaveIndex(0)
			, m_iTimeLimits(0)
			, m_iScore(0)
			, m_bForbidExpire(true)
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
			m_iSaveIndex = pkEntry->_iSaveIndex;
			m_kBuyConditions = pkEntry->_kBuyConditions;
			m_kLimits = pkEntry->_kLimits;
			m_kItems = pkEntry->_kItems;
			m_iScore = pkEntry->_iScore;

			{
				Utility::UT_SIMDataList::iterator it = pkEntry->_kInActivitys.begin();
				for (; it!=pkEntry->_kInActivitys.end(); ++it)
				{
					GameLogic::LC_IDSet& rkCatas = m_kInActivitys[it->IID()];
					if (it->ParamA())
					{
						rkCatas.insert(it->ParamA());
					}
				}
			}

			{
				Utility::UT_SIMDataList::iterator it = pkEntry->_kExcludeActivitys.begin();
				for (; it!=pkEntry->_kExcludeActivitys.end(); ++it)
				{
					m_kExcludeActivitys[it->IID()].insert(it->ParamA());
				}
			}
			m_bForbidExpire = pkEntry->_bForbidExpire;
			return true;
		}

		void GetValidActivitys(ACT_Type2CatasMap& rkActivitys);
		ResultType CheckConditions(LC_ServerPlayer* pkPlayer, ACT_ScheduleSaveEntry* pkSaveSchedule, ACT_ScheduleSaveCommon* pkSaveDetail, int32_t iDrawCount);
		ResultType DoReward(LC_ServerPlayer* pkPlayer, ACT_ScheduleSaveEntry* pkEntry, GameLogic::LC_RewardItemsExtend& rkExtend, int32_t iDrawCount);

		int32_t m_iID;
		int32_t m_iSchdeule;
		int32_t m_iType;
		int32_t m_iKey;
		int32_t m_iTimeLimits;
		int32_t m_iSaveIndex;
		Utility::UT_SIMDataList		m_kBuyConditions;
		Utility::UT_SIMDataMap		m_kLimits;
		Utility::LC_CashMap			m_kItems;
		int32_t						m_iScore;
		ACT_Type2CatasMap			m_kInActivitys;
		ACT_Type2CatasMap			m_kExcludeActivitys;
		bool						m_bForbidExpire;
	};

	class ACT_OfferARewardManager 
		: public ACT_ScheduleDataActivity<ACT_OfferARewardDataEntry, CSVFile::CF_ACTOfferAReward>
	{
	public:
		ACT_OfferARewardManager(void)
			:ACT_ScheduleDataActivity<ACT_OfferARewardDataEntry, CSVFile::CF_ACTOfferAReward>(ACT_SCHEDULE_TP_OFFER_A_REWARD)
		{}
		virtual void AddCharge(LC_ServerPlayer* pkPlayer, int32_t val, const mem::vector<int32_t>& kCatagorys);
		virtual void OnCharge(LC_ServerPlayer* pkPlayer, int32_t val, const GameLogic::LC_IDSet* pkCatagorys);
		virtual ResultType DrawReward(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t iSchedule, int32_t iKey, GameLogic::LC_RewardItemsExtend& rkExtend, const GameLogic::LC_IDVector* pkExtraParams, int& count);
	};
}

#endif //__ACT_OFFER_A_REWARD_H__
