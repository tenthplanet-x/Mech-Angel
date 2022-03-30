#ifndef __ACT_SIGNIN_H__
#define __ACT_SIGNIN_H__

#pragma once

#include "LC_Helper.h"

#include "ACT_ActivitySchedule.h"

#include "CF_ACTSignIn.h"
#include "CF_ACTSignInA.h"
#include "LC_Define.h"

class LC_ServerPlayer;
namespace Activity
{
	class ACT_SignInDataEntry
	{
	public:
		ACT_SignInDataEntry(void)
			: m_iID(0)
			, m_iSchdeule(0)
			, m_iType(0)
			, m_iDay(0)
			, m_iSaveIndex(0)
			, m_bMailEnable(true)
		{}

		inline int32_t Catagory(void) const {return m_iType;}
		inline int32_t Schedule(void) const {return m_iSchdeule;}
		inline int32_t Key(void) const {return m_iDay;}

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
			m_iDay = pkEntry->_iDay;
			m_iSaveIndex = pkEntry->_iSaveIndex;
			m_kDoubleConsumes = pkEntry->_kDoubleConsumes;
			m_kRewardItems.SetUp(pkEntry->_kReward);
			m_bMailEnable = pkEntry->_iMailEnable>0;
			m_sMailTitle = pkEntry->_sMailTitle;
			m_sMailDesc = pkEntry->_sMailDesc;
			return true;
		}

		ResultType DoReward(LC_ServerPlayer* pkPlayer, ACT_ScheduleSaveEntry* pkEntry,
							GameLogic::LC_RewardItemsExtend& rkExtend, int32_t iMultiFlag);

		int32_t m_iID;
		int32_t m_iSchdeule;
		int32_t m_iType;
		int32_t m_iDay;
		int32_t m_iSaveIndex;
		GameLogic::LC_ItemsReward m_kRewardItems;
		Utility::LC_CashMap m_kDoubleConsumes;
		bool  m_bMailEnable;
		StringType m_sMailTitle;
		StringType m_sMailDesc;
	};

	class ACT_SignInManager 
		: public ACT_ScheduleDataActivity<ACT_SignInDataEntry, CSVFile::CF_ACTSignIn, CSVFile::CF_ACTSignInA>
	{
	public:
		ACT_SignInManager(void)
			:ACT_ScheduleDataActivity<ACT_SignInDataEntry, CSVFile::CF_ACTSignIn, CSVFile::CF_ACTSignInA>(ACT_SCHEDULE_TP_SIGNIN)
		{}

		virtual ResultType DrawReward(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t iSchedule, int32_t iKey, GameLogic::LC_RewardItemsExtend& rkExtend, const GameLogic::LC_IDVector* pkExtraParams, int& count);
	};
}

#endif //__ACT_SIGNIN_H__
