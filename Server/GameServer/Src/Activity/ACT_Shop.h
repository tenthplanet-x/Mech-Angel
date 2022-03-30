#ifndef __ACT_SHOP_H__
#define __ACT_SHOP_H__

#pragma once

#include "LC_Helper.h"

#include "ACT_ActivityDataManager.h"
#include "ACT_ActivitySchedule.h"

#include "CF_ACTShop.h"
#include "CF_ACTShopA.h"
#include "CF_ACTShopB.h"


class LC_ServerPlayer;
namespace Activity
{
	class ACT_ShopDataEntry
	{
	public:
		ACT_ShopDataEntry(void)
			: m_iID(0)
			, m_iSchdeule(0)
			, m_iType(0)
			, m_iKey(0)
			, m_iSaveIndex(0)
			, m_iScore(0)
			, m_bMailEnable(true)
			, m_bOssLog(false)
			, m_bForbidExpire(false)
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
			m_iType = pkEntry->_iType;
			m_iKey = pkEntry->_iKey;
			m_iSaveIndex = pkEntry->_iSaveIndex;
			m_kBuyConditions = pkEntry->_kBuyConditions;
			m_kLimits = pkEntry->_kLimits;
			m_kOriginalPrices = pkEntry->_kOriginalPrices;
			m_kCurrentPrices = pkEntry->_kCurrentPrices;
			m_kActScorePrices = pkEntry->kActScorePrices;
			m_iScore = pkEntry->_iScore;
			m_kItems.SetUp(pkEntry->_kItems);
			m_bMailEnable = pkEntry->_iMailEnable>0;
			m_sMailTitle = pkEntry->_sMailTitle;
			m_sMailDesc = pkEntry->_sMailDesc;
			m_bOssLog = pkEntry->_bOssLog;
			m_bForbidExpire = pkEntry->_bForbidExpire;
			m_nGuildLevel = pkEntry->_GuildLevel;
			return true;
		}

		ResultType DoReward(LC_ServerPlayer* pkPlayer, ACT_ScheduleSaveEntry* pkEntry, GameLogic::LC_RewardItemsExtend& rkExtend, int32_t iDrawCount, uint32_t& nLogItemId, int32_t& nLogItemCount, int nLogCode = 0, const vector<int>& nParams = vector<int>());
		ResultType CheckConditions(LC_ServerPlayer* pkPlayer, ACT_ScheduleSaveEntry* pkSaveSchedule);
		StringType FormatCond2String();

		int32_t m_iID;
		int32_t m_iSchdeule;
		int32_t m_iType;
		int32_t m_iKey;
		int32_t m_iSaveIndex;
		Utility::UT_SIMDataList		m_kBuyConditions;
		Utility::UT_SIMDataMap		m_kLimits;
		Utility::LC_CashMap			m_kOriginalPrices;
		Utility::LC_CashMap			m_kCurrentPrices;
		Utility::UT_SIMDataList		m_kActScorePrices;
		int32_t	m_iScore;
		GameLogic::LC_ItemsReward	m_kItems;
		bool		m_bMailEnable;
		StringType	m_sMailTitle;
		StringType	m_sMailDesc;
		bool		m_bOssLog;
		bool		m_bForbidExpire;
		int		m_nGuildLevel;
	};

	class ACT_ShopManager 
		: public ACT_ScheduleDataActivity<ACT_ShopDataEntry, CSVFile::CF_ACTShop, CSVFile::CF_ACTShopA>
	{
	public:
		ACT_ShopManager(void)
			:ACT_ScheduleDataActivity<ACT_ShopDataEntry, CSVFile::CF_ACTShop, CSVFile::CF_ACTShopA>(ACT_SCHEDULE_TP_SHOP)
		{}
		
		void AddCharge(LC_ServerPlayer* pkPlayer, int32_t val, const mem::vector<int32_t>& kCatagorys);
		virtual void OnCharge(LC_ServerPlayer* pkPlayer, int32_t val, const GameLogic::LC_IDSet* pkCatagorys);
		virtual ResultType DrawReward(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t iSchedule, int32_t iKey, GameLogic::LC_RewardItemsExtend& rkExtend, const GameLogic::LC_IDVector* pkExtraParams, int& count);
	};

	class ACT_ShopManagerB 
		: public ACT_ScheduleDataActivity<ACT_ShopDataEntry, CSVFile::CF_ACTShopB>
	{
	public:
		ACT_ShopManagerB(void)
			:ACT_ScheduleDataActivity<ACT_ShopDataEntry, CSVFile::CF_ACTShopB>(ACT_SCHEDULE_TP_SHOP_B)
		{}

		virtual ResultType DrawReward(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t iSchedule, int32_t iKey, GameLogic::LC_RewardItemsExtend& rkExtend, const GameLogic::LC_IDVector* pkExtraParams, int& count);
	};
}

#endif //__ACT_SHOP_H__
