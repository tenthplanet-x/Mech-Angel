#ifndef __ACT_TIME_LIMITED_SHOP_H__
#define __ACT_TIME_LIMITED_SHOP_H__

#pragma once

#include "LC_Helper.h"

#include "ACT_ActivityDataManager.h"
#include "ACT_ActivitySchedule.h"

#include "CF_ACTShopTimeLimited.h"

class LC_ServerPlayer;
namespace Activity
{
	class ACT_TimeLimitedShopDataEntry
	{
	public:
		ACT_TimeLimitedShopDataEntry(void)
			: m_iID(0)
			, m_iSchdeule(0)
			, m_iType(0)
			, m_iKey(0)
			, m_iSaveIndex(0)
		{}

		int32_t Catagory() const;
		int32_t Schedule() const;
		int32_t Key() const;

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
			m_kLimits = pkEntry->_kLimits;
			m_kOriginalPrices = pkEntry->_kOriginalPrices;
			m_kCurrentPrices = pkEntry->_kCurrentPrices;
			m_kItems.SetUp(pkEntry->_kItems);
			return true;
		}

		ResultType DoReward(LC_ServerPlayer* pkPlayer, ACT_ScheduleSaveEntry* pkEntry, GameLogic::LC_RewardItemsExtend& rkExtend, int32_t iDrawCount);
		ResultType CheckConditions(LC_ServerPlayer* pkPlayer, ACT_ScheduleSaveEntry* pkSaveSchedule);

		int32_t m_iID;
		int32_t m_iSchdeule;
		int32_t m_iType;
		int32_t m_iKey;
		int32_t m_iSaveIndex;
		Utility::UT_SIMDataMap		m_kLimits;
		Utility::LC_CashMap			m_kOriginalPrices;
		Utility::LC_CashMap			m_kCurrentPrices;
		GameLogic::LC_ItemsReward	m_kItems;
	};

	class ACT_TimeLimitedShop
		: public ACT_ScheduleDataActivity<ACT_TimeLimitedShopDataEntry, CSVFile::CF_ACTShopTimeLimited>
	{
	public:
		ACT_TimeLimitedShop()
			:ACT_ScheduleDataActivity<ACT_TimeLimitedShopDataEntry, CSVFile::CF_ACTShopTimeLimited>(ACT_SCHEDULE_TP_TIMELIMITED_SHOP)
		{}

		void AddCharge(LC_ServerPlayer* pkPlayer, int32_t val, const mem::vector<int32_t>& kCatagorys);
		virtual void OnCharge(LC_ServerPlayer* pkPlayer, int32_t val, const GameLogic::LC_IDSet* pkCatagorys);
		virtual ResultType DrawReward(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t iSchedule, int32_t iKey, GameLogic::LC_RewardItemsExtend& rkExtend, const GameLogic::LC_IDVector* pkExtraParams, int& count);
	};
}

#endif //__ACT_TIME_LIMITED_SHOP_H__