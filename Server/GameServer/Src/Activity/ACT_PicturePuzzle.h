#ifndef __ACT_PICTURE_PUZZLE_H__
#define __ACT_PICTURE_PUZZLE_H__

#pragma  once

#include "LC_Helper.h"

#include "ACT_ActivityDataManager.h"
#include "ACT_ActivitySchedule.h"
#include "CF_ACTMonopoly.h"
#include "CF_HolidayActivity_PinTuReward.h"
namespace Activity
{

	class ACT_PicturePuzzleDataEntry
	{
	public:
		ACT_PicturePuzzleDataEntry(void)
			:m_iID(0)
			,m_iSchdeule(0)
			,m_iType(0)
			,m_iKey(0)
			,m_kSaveIndex(0)
			,m_bMailEnable(false)
			,m_iShowSchdeule(0)
		{

		}
		int32_t m_iID;
		int32_t m_iSchdeule;
		int32_t m_iShowSchdeule;
		int32_t m_iType;
		int32_t m_iKey;
		int32_t m_kSaveIndex;
		Utility::UT_SIMDataList m_kRewardItems;
		bool	m_bMailEnable;
		StringType m_sMailTitle;
		StringType m_sMailDesc;

		inline int32_t Catagory(void) const {return m_iType;}
		inline int32_t Schedule(void) const {return m_iSchdeule;}
		inline int32_t Key(void) const {return m_iKey;}
		inline int32_t ShowSchedule(void) const {return m_iShowSchdeule;}

		ResultType DoReward(LC_ServerPlayer* pkPlayer, GameLogic::LC_RewardItemsExtend& rkExtend);

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
			m_iKey = pkEntry->_iKey ;
			m_kSaveIndex = pkEntry->_iSaveIndex;
			m_bMailEnable = pkEntry->_iMailEnable > 0;
			m_kRewardItems = pkEntry->_kReward;
			m_sMailTitle = pkEntry->_sMailTitle;
			m_sMailDesc = pkEntry->_sMailDesc;
			return true;
		}
	};
	class ACT_PicturePuzzleManager 
		: public ACT_ScheduleDataActivity<ACT_PicturePuzzleDataEntry, CSVFile::CF_HolidayActivity_PinTuReward>,public ACT_ScheduleEndSendMail<ACT_PicturePuzzleDataEntry>
	{
	public:
		ACT_PicturePuzzleManager(void)
			:ACT_ScheduleDataActivity<ACT_PicturePuzzleDataEntry, CSVFile::CF_HolidayActivity_PinTuReward>(ACT_SCHEDULE_TP_PINTU)
			/*,ACT_ScheduleEndSendMail<ACT_PicturePuzzleDataEntry>(ACT_SCHEDULE_TP_PINTU)*/
		{}
		void OnFinishTaskAndAchievement(LC_ServerPlayer* pkPlayer, int32_t nType, int32_t nFinishId, const GameLogic::LC_IDSet* pkCatagorys =NULL);
		ResultType DrawReward(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t iSchedule, int32_t iKey, GameLogic::LC_RewardItemsExtend& rkExtend, const GameLogic::LC_IDVector* pkExtraParams, int& count);
		void InitConfig(CSVFile::CSVDataLoader* pkLoader, const CSVFile::CSVDataSourceList* pkCSVs);
		
		ResultType CheckScheduleData(LC_ServerPlayer* pkPlayer, DataEntryType* pkDataEntry, int32_t iCatagory, int32_t iSchedule, int32_t iKey);
		ResultType EndDoReward(LC_ServerPlayer* pkPlayer, DataEntryType* pkDataEntry, int32_t iCatagory, int32_t iSchedule, int32_t iKey);

		virtual ResultType ACTScheduleEndSendMail(LC_ServerPlayer* pkPlayer, DataEntryType* pkData, MailInfo& mail);
	};

}
#endif //__ACT_PICTURE_PUZZLE_H__