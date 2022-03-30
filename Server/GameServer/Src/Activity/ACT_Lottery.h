#pragma once

#include "LC_Helper.h"

#include "ACT_ActivityDataManager.h"
#include "ACT_ActivitySchedule.h"

#include "CF_ACTLottery.h"
#include "CF_ACTLotteryReward.h"
#include "CF_ACTLotteryStaticReward.h"
#include <map>

class LC_ServerPlayer;
namespace Activity
{
struct LotteryReward
{
	LotteryReward()
		: iWeight(0)
		, iCount(0) 
		, iBroadcastId(0)
	{

	}

	Utility::UT_SIMDataList kItems;
	int32_t iWeight;
	int32_t iCount; //固定第几次出
	int32_t iBroadcastId;
};

struct LotteryStaticReward
{
	LotteryStaticReward() 
		: iBroadcastId(0)
	{
	}

	Utility::UT_SIMDataList kItems;
	int32_t iBroadcastId;
};

class ACT_LotteryDataEntry
{
public:
	ACT_LotteryDataEntry(void)
		: m_iID(0)
		, m_iSchdeule(0)
		, m_iCategory(0)
		, m_iKey(0)
		, m_iPerCharge(0)
		, m_bEmptyReset(false)
		, m_iResetLimit(0)
		, m_bMailEnable(true)
	{}

	inline int32_t Catagory(void) const {return m_iCategory;}
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
		m_iCategory = pkEntry->_iCategory;
		m_iKey = pkEntry->_iKey;
		m_iPerCharge = pkEntry->_iPerCharge;
		m_bEmptyReset = pkEntry->_iEmptyReset > 0;
		m_iResetLimit = pkEntry->_iResetLimit;
		m_bMailEnable = pkEntry->_iMailEnable > 0;
		m_sMailTitle = pkEntry->_sMailTitle;
		m_sMailDesc = pkEntry->_sMailDesc;
		m_kPerItems = pkEntry->_kPerItems;
		m_kPerGetItems = pkEntry->_kPerGetItems;
		//Utility::ParseFmtString2DataIFs(pkEntry->_, m_kPerItems);
		UpdateLotteryCountMapData(m_iSchdeule, m_iCategory, m_iKey );
		return true;
	}

	void FillReward(const CSVFile::CF_ACTLotteryReward::DataEntry* pkEntry);
	void FillStaticReward(const CSVFile::CF_ACTLotteryStaticReward::DataEntry* pkEntry);
	

	ResultType DoReward(LC_ServerPlayer* pkPlayer, ACT_ScheduleSaveEntry* pkEntry, int32_t iRewardCount);
	ResultType DoStaticReward(LC_ServerPlayer* pkPlayer, ACT_ScheduleSaveEntry* pkEntry, int32_t iRewardCount);

	int32_t m_iID;
	int32_t m_iSchdeule;
	int32_t m_iCategory;
	int32_t m_iKey;
	int32_t m_iPerCharge;
	Utility::UT_SIMDataList m_kPerItems;
	Utility::UT_SIMDataList m_kPerGetItems;
	bool m_bEmptyReset;
	int32_t m_iResetLimit;
	bool m_bMailEnable;
	StringType m_sMailTitle;
	StringType m_sMailDesc;
	mem::vector<LotteryReward> m_RewardVec;
	mem::map<int32_t, LotteryStaticReward> m_StaticRewardData;
	mem::map<int32_t, int32_t> m_CountIndexMap; //count to index

	typedef mem::map<uint32_t, mem::vector<int32_t> > LotteryCountMap;
	static int32_t FetchLotteryCount(int16_t nSchedule, int16_t nCategory, int32_t nValue);
	static uint32_t BuildKey(int16_t nSchedule, int16_t nCategory);
private:
	static LotteryCountMap m_LotteryCountMap;
	void UpdateLotteryCountMapData( int16_t nSchedule, int16_t nCategory, int32_t nValue );
};


class ACT_LotteryManager 
	: public ACT_ScheduleDataActivity<ACT_LotteryDataEntry, CSVFile::CF_ACTLottery>
{
public:
	ACT_LotteryManager(void)
		:ACT_ScheduleDataActivity<ACT_LotteryDataEntry, CSVFile::CF_ACTLottery>(ACT_SCHEDULE_TP_LOTTERY)
	{}

	virtual void InitConfig(CSVFile::CSVDataLoader* pkLoader, const CSVFile::CSVDataSourceList* pkCSVs);
	void CheckConfig();

	void AddCharge(LC_ServerPlayer* pkPlayer, int32_t val, const mem::vector<int32_t>& kCatagorys);
	virtual void OnCharge(LC_ServerPlayer* pkPlayer, int32_t val, const GameLogic::LC_IDSet* pkCatagorys);
	virtual ResultType DrawReward(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t iSchedule, int32_t iKey, GameLogic::LC_RewardItemsExtend& rkExtend, const GameLogic::LC_IDVector* pkExtraParams, int& count);
private:
	ResultType DrawSingleReward(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t iSchedule, int32_t iKey, GameLogic::LC_RewardItemsExtend& rkExtend, const GameLogic::LC_IDVector* pkExtraParams);
private:
	static Utility::UT_SIMDataList m_MultiRewardItems;
public:
	static int AddSingleRewardItem( int32_t nItemId, int32_t nCount, bool bAssociation = false);
};
}
