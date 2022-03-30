#include "ClientPCH.h"
#include "LC_ClientActivityManager.h"
#include "TM_ClientTimeManager.h"
#include "YunYingPlatform.h"
#include "LC_ClientHelp.h"
#include "UI_UIManager.h"
#include "LC_GameEvent.h"
#include "NW_ClientSessionManager.h"

#include "CF_ACTTreasureHunt.h"
#include "CF_ACTChargeProfitB.h"
#include "CF_ACTChargeProfitBReward.h"
#include "CF_HolidayActivity_PinTuReward.h"
#include "CF_HolidayActivity_ChargeContinuous.h"
#include "CF_RankCondition.h"
#include "CF_ActivityTime.h"

using namespace CSVFile;
using namespace GameLogic;
using namespace Utility;
using namespace Activity;

void ACT_ScheduleSaveCommon::Clear(int32_t iBegin, int32_t iEnd)
{
	for (int32_t i=iBegin; i<iEnd; ++i)
	{
		m_kDatas.erase(i);
	}
}
void ACT_ScheduleSaveCommon::GetDataFromPB(const char_data_define::activity_schedule_common& info)
{
	m_iID  = info.id();
	m_kDatas.clear();
	for (int i=0; i<info.datas_size()/2; ++i)
	{
		m_kDatas[(int32_t)info.datas(i<<1)] = info.datas((i<<1)+1);
	}
}
bool ACT_ScheduleSaveCommon::_regBit(int32_t iBegin, int32_t iEnd, int32_t iValue)
{
	uint16_t idx = 0;
	uint16_t bit = 0;
	FORMAT_VALUE_TO_BIT<ACT_DataMap::mapped_type>(iValue, idx, bit);
	idx += iBegin;
	if (idx >= iEnd)
	{
		return false;
	}
	m_kDatas[idx] |= (1 << bit);
	return true;
}
bool ACT_ScheduleSaveCommon::_isBitReg(int32_t iBegin, int32_t iEnd, int32_t iValue)
{
	uint16_t idx = 0;
	uint16_t bit = 0;
	FORMAT_VALUE_TO_BIT<ACT_DataMap::mapped_type>(iValue, idx, bit);
	idx += iBegin;
	return (idx<iEnd) ? (m_kDatas[idx]&(1<<bit))>0 : false;
}

void ACT_ScheduleSaveEntry::GetDataFromPB(const char_data_define::activity_schedule_entry& info)
{
	Clear();
	m_uiTimestamp = info.timestamp();
	m_kSelf.GetDataFromPB(info.self());
	for (int i=0; i<info.details_size(); ++i)
	{
		const char_data_define::activity_schedule_common& rk_detail = info.details(i);
		ACT_ScheduleSaveCommon* pkDetail = _addScheduleSaveDetail(rk_detail.id());
		if (pkDetail)
		{
			pkDetail->GetDataFromPB(rk_detail);
		}
	}
}
ACT_ScheduleSaveCommon* GameLogic::ACT_ScheduleSaveEntry::_addScheduleSaveDetail(int32_t iDetail)
{
	ACT_ScheduleSaveCommonMap::iterator it = m_kDetails.find(iDetail);
	if (it != m_kDetails.end())
	{
		return &it->second;
	}

	ACT_ScheduleSaveCommon& rkDetail = m_kDetails[iDetail];
	rkDetail.Init(iDetail);
	return &rkDetail;
}

void ACT_ScheduleSaveCatagory::GetDataFromPB(const char_data_define::activity_schedule_catagory& info)
{
	m_iSchedule = info.id();
	for (int i=0; i<info.schedules_size(); ++i)
	{
		const char_data_define::activity_schedule_entry& rkentry = info.schedules(i);
		ACT_ScheduleSaveEntry* pkEntry = _addScheduleSaveEntry(rkentry.self().id());
		if (pkEntry)
		{
			pkEntry->GetDataFromPB(rkentry);
		}
	}
	UpdateDirtyFlag();
}
ACT_ScheduleSaveEntry* ACT_ScheduleSaveCatagory::_addScheduleSaveEntry(int32_t iSchedule)
{
	ACT_ScheduleSaveEntryMap::iterator it = m_kSchedules.find(iSchedule);
	if (it != m_kSchedules.end())
	{
		return &it->second;
	}

	ACT_ScheduleSaveEntry& rkEntry = m_kSchedules[iSchedule];
	rkEntry.Init(iSchedule, GET_CURRENT_TIMESTAMP_IN_SECONDS());
	return &rkEntry;
}

void ACT_ScheduleSaveContainer::GetDataFromPB(const char_data_define::activity_schedule_info& info)
{
	for (int i=0; i<info.catagorys_size(); ++i)
	{
		const char_data_define::activity_schedule_catagory& rkentry = info.catagorys(i);
		ACT_ScheduleSaveCatagory* pkEntry = _addScheduleSaveCatagory(rkentry.id());
		if (pkEntry)
		{
			pkEntry->GetDataFromPB(rkentry);
		}
	}
	for (int i = 0; i < info.persistent_info_size(); ++i)
	{
		const char_data_define::activity_schedule_persistent_info& rkentry = info.persistent_info(i);
		m_kPersistentInfo[rkentry.id()] = rkentry.value();
	}
	UpdateDirtyFlag();
}

ACT_ScheduleSaveCatagory* ACT_ScheduleSaveContainer::_addScheduleSaveCatagory(int32_t iCatagory)
{
	ACT_ScheduleSaveCatagoryMap::iterator it = m_kCatagorys.find(iCatagory);
	if (it != m_kCatagorys.end())
	{
		return &it->second;
	}

	ACT_ScheduleSaveCatagory& rkEntry = m_kCatagorys[iCatagory];
	rkEntry.m_iSchedule = iCatagory;
	return &rkEntry;
}


bool ACT_RewardDataEntry::Init(CSVFile::CF_ACTSubordinateReward::DataEntry* pkEntry)
{
	if (NULL == pkEntry)
	{
		return false;
	}

	m_iID = pkEntry->_iID;
	m_iSchdeule = pkEntry->_iSchedule;
	m_iType = pkEntry->_iType;
	m_iKey = pkEntry->_iLevel;
	m_iSaveIndex = pkEntry->_iSaveIndex;
	m_kRewardItems.SetUp(pkEntry->_kReward);
	return true;
}

bool ACT_RewardDataEntry::Init(CSVFile::CF_ACTSubordinateRewardA::DataEntry* pkEntry)
{
	if (NULL == pkEntry)
	{
		return false;
	}

	m_iID = pkEntry->_iID;
	m_iSchdeule = pkEntry->_iSchedule;
	m_iType = pkEntry->_iType;
	m_iKey = pkEntry->_iLevel;
	m_iRank = pkEntry->_iSortID;
	m_iSaveIndex = pkEntry->_iSaveIndex;
	m_iMaxCount = pkEntry->_iGetMaxCounts;
	m_bHighReward = pkEntry->_bHighReward == 1;
	m_kRewardItems.SetUp(pkEntry->_kReward);
	return true;
}

bool ACT_RewardDataEntry::Init(CSVFile::CF_ACTSubordinateRewardB::DataEntry* pkEntry)
{
	if (NULL == pkEntry)
	{
		return false;
	}

	m_iID = pkEntry->_iID;
	m_iSchdeule = pkEntry->_iSchedule;
	m_iType = pkEntry->_iType;
	m_iKey = pkEntry->_iLevel;
	m_iRank = pkEntry->_iSortID;
	m_iSaveIndex = pkEntry->_iSaveIndex;
	m_iMaxCount = pkEntry->_iGetMaxCounts;
	m_iParentKey = pkEntry->_GroupID;
	m_iCustomA = pkEntry->_iFinishSchedule;
	m_kRewardCondition = pkEntry->_Params;
	m_kRewardItems.SetUp(pkEntry->_kReward);
	return true;
}

bool ACT_RewardDataEntry::Init(CSVFile::CF_HolidayActivity_PinTuReward::DataEntry* pkEntry)
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
	m_kRewardItems.SetUp(pkEntry->_kReward);
	return true;
}

bool ACT_RewardDataEntry::Init(CSVFile::CF_HolidayActivity_ChargeContinuous::DataEntry* pkEntry)
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
	m_kRewardItems.SetUp(pkEntry->_kReward);
	m_iRebateID=pkEntry->_iRebateID; 
	return true;
}



bool GameLogic::ACT_RewardDataEntry::Init(CSVFile::CF_ACTShop::DataEntry* pkEntry)
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
	m_iRank = pkEntry->_uiRank;
	m_kRewardCondition = pkEntry->_kBuyConditions;
	m_kOriginalPrices = pkEntry->_kOriginalPrices;  
	m_kCurrentPrices = pkEntry->_kCurrentPrices;
	m_kActScore	= pkEntry->kActScorePrices;
	m_kRewardItems.SetUp(pkEntry->_kItems);
	m_iScore = pkEntry->_iScore;
	m_bForbidExpire = pkEntry->_bForbidExpire;
	return true;
}

static mem::map<int32_t,StringType> ScheduleImageMap;
bool GameLogic::ACT_RewardDataEntry::Init(CSVFile::CF_ACTShopA::DataEntry* pkEntry)
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
	m_iRank = pkEntry->_uiRank;
	m_sGiftName = pkEntry->_name;
	m_sIcon1 = pkEntry->_kIcon1;
	m_kRewardCondition = pkEntry->_kBuyConditions;
	m_kUICondition = pkEntry->_kUIConditions;
	m_kOriginalPrices = pkEntry->_kOriginalPrices;
	m_kCurrentPrices = pkEntry->_kCurrentPrices;
	m_kActScore	= pkEntry->kActScorePrices;
	m_kRewardItems.SetUp(pkEntry->_kItems);
	m_iScore = pkEntry->_iScore;
	m_bForbidExpire = pkEntry->_bForbidExpire;
	m_lCharTypeID1 = pkEntry->_CharTypeID1;
	m_lCharTypeID2 = pkEntry->_CharTypeID2;
	m_Position1 = pkEntry->_Position1;
	m_Position2 = pkEntry->_Position2;
	m_iGuildLevel = pkEntry->_GuildLevel;
	m_sIcon2 = pkEntry->_kDiscountIcon;
	m_iOpenVipLevel = pkEntry->_iUpgradeType;	//升级助力类型
	if (pkEntry->_sHeroIcon!="")
	{
		ScheduleImageMap[m_iSchdeule]=pkEntry->_sHeroIcon;
	}
	return true;
}

static mem::map<int32_t,StringType> TimeIdImageMap;
bool GameLogic::ACT_RewardDataEntry::Init(CSVFile::CF_ACTShopB::DataEntry* pkEntry)
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
	m_iRank = pkEntry->_uiRank;
	m_sGiftName = pkEntry->_name;
	m_sIcon1 = pkEntry->_kIcon1;
	m_kRewardCondition = pkEntry->_kBuyConditions;
	m_kUICondition = pkEntry->_kUIConditions;
	m_kOriginalPrices = pkEntry->_kOriginalPrices;
	m_kCurrentPrices = pkEntry->_kCurrentPrices;
	m_kActScore	= pkEntry->kActScorePrices;
	m_kRewardItems.SetUp(pkEntry->_kItems);
	m_iScore = pkEntry->_iScore;
	m_bForbidExpire = pkEntry->_bForbidExpire;
	m_lCharTypeID1 = pkEntry->_CharTypeID1;
	m_lCharTypeID2 = pkEntry->_CharTypeID2;
	m_Position1 = pkEntry->_Position1;
	m_Position2 = pkEntry->_Position2;
	m_iGuildLevel = pkEntry->_GuildLevel;
	m_sIcon2 = pkEntry->_kDiscountIcon;
	m_iOpenVipLevel = pkEntry->_iUpgradeType;	//升级助力类型
	if (pkEntry->_sHeroIcon!="")
	{
		TimeIdImageMap[m_iSchdeule]=pkEntry->_sHeroIcon;
	}
	
	return true;
}


bool GameLogic::ACT_RewardDataEntry::Init(CSVFile::CF_ACTSubordinateConsume::DataEntry* pkEntry)
{
	if (NULL == pkEntry)
	{
		return false;
	}
	m_iID = pkEntry->_iID;
	m_iSchdeule = pkEntry->_iSchedule;
	m_iType = pkEntry->_iType;
	m_iKey = pkEntry->_iKey + (pkEntry->_iFloor-1)*pkEntry->_iFloorStep;
	m_kRewardItems.SetUp(pkEntry->_kReward);

	return true;
}

bool ACT_RewardDataEntry::Init(CSVFile::CF_ACTChargeProfit::DataEntry* pkEntry)
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
	m_iParentKey = pkEntry->_ParentKey;
	m_iCustomA = pkEntry->_iTimeLimits;
	m_kRewardItems.SetUp(pkEntry->_kReward);
	return true;
}

bool ACT_RewardDataEntry::Init(CSVFile::CF_ACTChargeProfitA::DataEntry* pkEntry)
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
	m_iParentKey = pkEntry->_ParentKey;
	m_iCustomA = pkEntry->_iTimeLimits;
	m_kRewardItems.SetUp(pkEntry->_kReward);
	return true;
}

bool ACT_RewardDataEntry::Init(CSVFile::CF_ACTShot::DataEntry* pkEntry)
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
	m_kRewardItems.SetUp(pkEntry->_kReward);
	return true;
}

bool GameLogic::ACT_RewardDataEntry::Init(CSVFile::CF_ACTLottery::DataEntry* pkEntry)
{
	if (NULL == pkEntry)
	{
		return false;
	}
	m_iID = pkEntry->_iID;
	m_iSchdeule = pkEntry->_iSchedule;
	m_iType = pkEntry->_iCategory;
	m_iKey = pkEntry->_iKey;
	m_sGiftName = pkEntry->_name;
	m_iShopID = pkEntry->_iShopID;
	m_iMaxCount = pkEntry->_iMaxGamble;
	m_kRewardCondition = pkEntry->_kPerItems;
	m_iPerCharge = pkEntry->_iPerCharge;
	m_iOpenConsumeUnbindyuanbao = pkEntry->_openConditonConsumeUnbindyuanbao;
	m_iOpenVipLevel = pkEntry->_openVipLevel;
	m_iBuddyID = pkEntry->_iBuddyID;
	m_iMaxTimes = pkEntry->_iLotteryMaxTimes;

	m_sDescSign = pkEntry->_sDescSign;
	m_sRuleDesc = pkEntry->_sRuleDesc;
	m_sCardDesc = pkEntry->_sCardDesc;
	m_sButtonIcon1 = pkEntry->_kButtonIcon1;
	m_sIcon1 = pkEntry->_kIcon1;
	m_sIcon2 = pkEntry->_kIcon2;
	m_sIcon3 = pkEntry->_kIcon3;
	//CF_ACTLottery m_sIcon4 为道具展示内容
	m_sIcon4 = pkEntry->_sRewardShow;
	m_sNameIcon1 = pkEntry->_sIconName1;
	m_sNameIcon2 = pkEntry->_sIconName2;
	m_sNameIcon3 = pkEntry->_sIconName3;
	m_sNameIcon4 = pkEntry->_sIconName4;

	return true;
}

uint32_t GameLogic::LC_ClientActivityAsset::GetValue(int32_t ActivityType, int32_t iCatagory, int32_t ScheduleID, int32_t iStatType, int32_t iValueType)
{
	ACT_ScheduleSaveEntry* pkSaveEntry = _GetScheduleSaveEntry(ActivityType, iCatagory, ScheduleID);
	if (pkSaveEntry == NULL)
	{
		return 0;
	}
	return pkSaveEntry->GetValue(GET_SCHEDULE_KEY(iStatType, iValueType));
}

uint32_t GameLogic::LC_ClientActivityAsset::GetCount(int32_t ActivityType, int32_t iCatagory, int32_t ScheduleID, int32_t iStatType)
{
	ACT_ScheduleSaveEntry* pkSaveEntry = _GetScheduleSaveEntry(ActivityType, iCatagory, ScheduleID);
	if (pkSaveEntry == NULL)
	{
		return 0;
	}
	return pkSaveEntry->GetCount(iStatType);
}

uint32_t GameLogic::LC_ClientActivityAsset::GetDetailCount(int32_t ActivityType, int32_t iCatagory, int32_t ScheduleID, int32_t iKey, int32_t iStatType)
{
	ACT_ScheduleSaveEntry* pkSaveEntry = _GetScheduleSaveEntry(ActivityType, iCatagory, ScheduleID);
	if (pkSaveEntry == NULL)
	{
		return 0;
	}
	ACT_ScheduleSaveCommon* pkSaveDetail = pkSaveEntry->GetScheduleSaveDetail(iKey);
	if (pkSaveDetail == NULL)
	{
		return 0;
	}
	return pkSaveDetail->GetValue(GET_SCHEDULE_KEY(iStatType, ACT_SCHEDULE_DETAIL_COUNT));
}

uint32_t GameLogic::LC_ClientActivityAsset::GetDetailValue(int32_t ActivityType, int32_t iCatagory, int32_t ScheduleID, int32_t iKey, int32_t iStatType, int32_t iValueType)
{
	ACT_ScheduleSaveEntry* pkSaveEntry = _GetScheduleSaveEntry(ActivityType, iCatagory, ScheduleID);
	if (pkSaveEntry == NULL)
	{
		return 0;
	}
	ACT_ScheduleSaveCommon* pkSaveDetail = pkSaveEntry->GetScheduleSaveDetail(iKey);
	if (pkSaveDetail == NULL)
	{
		return 0;
	}
	return pkSaveDetail->GetValue(GET_SCHEDULE_KEY(iStatType, iValueType));
}

int32_t GameLogic::LC_ClientActivityAsset::GetCharge(int32_t ActivityType, int32_t iCatagory, int32_t ScheduleID,int32_t iRebateType){
	ACT_ScheduleSaveEntry* pkSaveEntry = _GetScheduleSaveEntry(ActivityType, iCatagory, ScheduleID);
	if (pkSaveEntry == NULL)
	{
		return 0;
	}
	return pkSaveEntry->GetScheduleSaveDetailAndValue(iCatagory,iRebateType); 
}

int32_t GameLogic::LC_ClientActivityAsset::GetChargeDay(int32_t ActivityType, int32_t iCatagory, int32_t ScheduleID) {
	ACT_ScheduleSaveEntry* pkSaveEntry = _GetScheduleSaveEntry(ActivityType, iCatagory, ScheduleID);
	if (pkSaveEntry == NULL)
	{
		return 0;
	}
	return pkSaveEntry->GetScheduleSaveDetailAndValue(iCatagory,GET_SCHEDULE_KEY(iCatagory, 0));
}


uint32_t GameLogic::LC_ClientActivityAsset::GetTimeStamp(int32_t ActivityType, int32_t iCatagory, int32_t ScheduleID)
{
	ACT_ScheduleSaveEntry* pkSaveEntry = _GetScheduleSaveEntry(ActivityType, iCatagory, ScheduleID);
	if (pkSaveEntry == NULL)
	{
		return 0;
	}
	return pkSaveEntry->GetTimeStamp();
}

bool GameLogic::LC_ClientActivityAsset::IsRewardReg(int32_t ActivityType, int32_t iCatagory, int32_t ScheduleID, int32_t iKey)
{
	LC_ClientActivityManager* pkActivityMgr = LC_ClientActivityManager::GetSingletonPtr();

	ACT_RewardDataEntry* pkRewardEntry = pkActivityMgr->GetRewardDataEntry(ActivityType,iCatagory,ScheduleID,iKey);
	if (pkRewardEntry == NULL)
	{
		return false;
	}
	ACT_ScheduleSaveEntry* pkSaveEntry = _GetScheduleSaveEntry(ActivityType, iCatagory, ScheduleID);
	if (pkSaveEntry == NULL)
	{
		return false;
	}
	return pkSaveEntry->IsRewardReg(pkRewardEntry->m_iSaveIndex);
}

bool GameLogic::LC_ClientActivityAsset::IsFeedbackRewardReg(int32_t ActivityType, int32_t iCatagory, int32_t ScheduleID, int32_t iKey, int32_t iIndex)
{
	LC_ClientActivityManager* pkActivityMgr = LC_ClientActivityManager::GetSingletonPtr();

	ACT_RewardDataEntry* pkRewardEntry = pkActivityMgr->GetRewardDataEntry(ActivityType,iCatagory,ScheduleID,iKey);
	if (pkRewardEntry == NULL)
	{
		return false;
	}
	ACT_ScheduleSaveEntry* pkSaveEntry = _GetScheduleSaveEntry(ActivityType, iCatagory, ScheduleID);
	if (pkSaveEntry == NULL)
	{
		return false;
	}
	return pkSaveEntry->IsRewardReg(iIndex);
}

bool GameLogic::LC_ClientActivityAsset::IsFireReg(int32_t ActivityType, int32_t iCatagory, int32_t ScheduleID, int32_t iKey)
{
	LC_ClientActivityManager* pkActivityMgr = LC_ClientActivityManager::GetSingletonPtr();

	ACT_RewardDataEntry* pkRewardEntry = pkActivityMgr->GetRewardDataEntry(ActivityType,iCatagory,ScheduleID,iKey);
	if (pkRewardEntry == NULL)
	{
		return false;
	}
	ACT_ScheduleSaveEntry* pkSaveEntry = _GetScheduleSaveEntry(ActivityType, iCatagory, ScheduleID);
	if (pkSaveEntry == NULL)
	{
		return false;
	}
	return pkSaveEntry->IsFireReg(pkRewardEntry->m_iSaveIndex);
}

bool GameLogic::LC_ClientActivityAsset::IsRewardRegStatType(int32_t ActivityType, int32_t iCatagory, int32_t ScheduleID, int32_t iKey, int32_t iStatType)
{
	LC_ClientActivityManager* pkActivityMgr = LC_ClientActivityManager::GetSingletonPtr();

	ACT_RewardDataEntry* pkRewardEntry = pkActivityMgr->GetRewardDataEntry(ActivityType,iCatagory,ScheduleID,iKey);
	if (pkRewardEntry == NULL)
	{
		return false;
	}
	ACT_ScheduleSaveEntry* pkSaveEntry = _GetScheduleSaveEntry(ActivityType, iCatagory, ScheduleID);
	if (pkSaveEntry == NULL)
	{
		return false;
	}
	return pkSaveEntry->IsRewardReg(pkRewardEntry->m_iSaveIndex, iStatType);
}

bool GameLogic::LC_ClientActivityAsset::IsFireRegStatType(int32_t ActivityType, int32_t iCatagory, int32_t ScheduleID, int32_t iKey, int32_t iStatType)
{
	LC_ClientActivityManager* pkActivityMgr = LC_ClientActivityManager::GetSingletonPtr();

	ACT_RewardDataEntry* pkRewardEntry = pkActivityMgr->GetRewardDataEntry(ActivityType,iCatagory,ScheduleID,iKey);
	if (pkRewardEntry == NULL)
	{
		return false;
	}
	ACT_ScheduleSaveEntry* pkSaveEntry = _GetScheduleSaveEntry(ActivityType, iCatagory, ScheduleID);
	if (pkSaveEntry == NULL)
	{
		return false;
	}
	return pkSaveEntry->IsFireReg(pkRewardEntry->m_iSaveIndex, iStatType);
}
void GameLogic::LC_ClientActivityAsset::GetDataFromPB(const char_data_define::char_activity_info& info)
{
	for (int i=0; i<info.activities_size(); ++i)
	{
		const char_data_define::activity_schedule_info& rkentry = info.activities(i);
		ACT_ScheduleSaveContainer* pkEntry = GetScheduleSaveContainer(rkentry.type());
		//if (pkEntry)
		{
			pkEntry->GetDataFromPB(rkentry);
		}
	}
	UpdateDirtyFlag();
}

StringType GameLogic::LC_ClientActivityAsset::GetLotteryCardInfo(int32_t ActivityType, int32_t iCatagory, int32_t ScheduleID, int32_t iKey)
{
	LC_ClientActivityManager* pkActivityMgr = LC_ClientActivityManager::GetSingletonPtr();

	ACT_RewardDataEntry* pkRewardEntry = pkActivityMgr->GetRewardDataEntry(ActivityType,iCatagory,ScheduleID,iKey);
	if (pkRewardEntry == NULL)
	{
		return "";
	}
	StringStreamType sStream("");

	sStream << pkRewardEntry->m_sGiftName << "#";
	sStream << pkRewardEntry->m_iShopID << "#";
	sStream << pkRewardEntry->m_iPerCharge << "#";
	for (UT_SIMDataList::const_iterator conit = pkRewardEntry->m_kRewardCondition.begin(); conit != pkRewardEntry->m_kRewardCondition.end(); ++conit)
	{
		const UT_SIMDataInfo& kinfo = *conit;
		sStream << kinfo.ID() << "|" << kinfo.ItemCount() << "|";
	}
	sStream << "#";

	sStream << pkRewardEntry->m_iMaxCount << "#";
	sStream << pkRewardEntry->m_sDescSign << "#";
	sStream << pkRewardEntry->m_sRuleDesc << "#";
	sStream << pkRewardEntry->m_sCardDesc << "#";
	sStream << pkRewardEntry->m_sIcon1 << "#";
	sStream << pkRewardEntry->m_sIcon2 << "#";
	sStream << pkRewardEntry->m_sIcon3 << "#";
	sStream << pkRewardEntry->m_sIcon4 << "#";
	sStream << pkRewardEntry->m_sNameIcon1 << "#";
	sStream << pkRewardEntry->m_sNameIcon2 << "#";
	sStream << pkRewardEntry->m_sNameIcon3 << "#";
	sStream << pkRewardEntry->m_sNameIcon4 << "#";
	sStream << pkRewardEntry->m_sButtonIcon1 << "#";
	sStream << Int2String(pkRewardEntry->m_iOpenConsumeUnbindyuanbao) << "#";
	sStream << Int2String(pkRewardEntry->m_iOpenVipLevel) << "#";
	sStream << pkRewardEntry->m_iBuddyID << "#" << pkRewardEntry->m_iMaxTimes << "#";

	return sStream.str();
}

StringType GameLogic::LC_ClientActivityAsset::GetStarShopInfo(int32_t ActivityType, int32_t iCatagory, int32_t ScheduleID, int32_t iKey)
{
	LC_ClientActivityManager* pkActivityMgr = LC_ClientActivityManager::GetSingletonPtr();

	ACT_RewardDataEntry* pkRewardEntry = pkActivityMgr->GetRewardDataEntry(ActivityType,iCatagory,ScheduleID,iKey);
	if (pkRewardEntry == NULL)
	{
		return "";
	}
	StringStreamType sStream("");
	const Utility::UT_SIMDataList *pkRewardList = pkRewardEntry->m_kRewardItems.GetAllItemList();
	if (pkRewardList->empty())
	{
		sStream << "#";
	}
	else
	{
		for (Utility::UT_SIMDataList::const_iterator Iter = pkRewardList->begin(); Iter != pkRewardList->end(); ++Iter)
		{
			sStream << Iter->ID() << ":" << Iter->ItemCount() << ";";
		}
		sStream << "#";
	}
	
	int iCostType=0;
	int64_t iActualPrice=0;
	LC_Helper::GetComMapOnlyPrice(pkRewardEntry->m_kCurrentPrices,iCostType,iActualPrice);

	sStream << iCostType << "#";
	sStream << iActualPrice  << "#";
	//购买限数
	Utility::UT_SIMDataMap::iterator it = pkRewardEntry->m_kLimits.begin();
	int iType = LC_ST_TP_NA;
	int iValue = 0;
	if (it != pkRewardEntry->m_kLimits.end())
	{
		Utility::UT_SIMDataInfo& rkLimit = it->second;
		iType = rkLimit.StatType();
		iValue = rkLimit.StatValue();
	}
	int iAvailableCount = 0;
	ACT_ScheduleSaveEntry* pkSaveEntry = _GetScheduleSaveEntry(ActivityType, iCatagory, ScheduleID);
	ACT_ScheduleSaveCommon* pkSaveCommon = pkSaveEntry ? pkSaveEntry->GetScheduleSaveDetail(iKey) : NULL;
	if (pkSaveCommon == NULL)
	{
		iAvailableCount = iValue;
	}
	else
	{
		iAvailableCount = iValue - pkSaveCommon->GetValue(GET_SCHEDULE_KEY(iType, ACT_SCHEDULE_DETAIL_COUNT));
	}
	sStream << iAvailableCount << "#" << iValue << "#";
	//购买限制
	for (UT_SIMDataList::const_iterator conit = pkRewardEntry->m_kUICondition.begin(); conit != pkRewardEntry->m_kUICondition.end(); ++conit)
	{
		const UT_SIMDataInfo& kinfo = *conit;
		sStream << kinfo.ItemCount() << ";";
	}
	sStream << "#";
	sStream << pkRewardEntry->m_sIcon1 << "#";
	sStream << pkRewardEntry->m_iRank << "#";
	sStream << pkRewardEntry->m_sGiftName << "#";
	sStream << pkRewardEntry->m_iOpenVipLevel << "#";

	return sStream.str();
}

StringType GameLogic::LC_ClientActivityAsset::GetShopInfo(int32_t ActivityType, int32_t iCatagory, int32_t ScheduleID, int32_t iKey)
{
	LC_ClientActivityManager* pkActivityMgr = LC_ClientActivityManager::GetSingletonPtr();

	ACT_RewardDataEntry* pkRewardEntry = pkActivityMgr->GetRewardDataEntry(ActivityType,iCatagory,ScheduleID,iKey);
	if (pkRewardEntry == NULL)
	{
		return "";
	}
	StringStreamType sTemp("");
	const Utility::UT_SIMDataList *pkRewardList = pkRewardEntry->m_kRewardItems.GetAllItemList();
	bool bBind = false;
	if (pkRewardList->empty())
	{
		sTemp << "0#0#";
	}
	else
	{
		sTemp << pkRewardList->front().ID() << "#" << pkRewardList->front().ItemCount() << "#";
		bBind = pkRewardList->front().ItemBindMode() > 0;
	}
	
	//原价可能为空，现价不能
	Utility::LC_ComMap::const_iterator iter = pkRewardEntry->m_kOriginalPrices.begin();
	if (iter != pkRewardEntry->m_kOriginalPrices.end())
		sTemp << iter->first << "#" << iter->second << "#";
	else
		sTemp << "0#0#";

	LC_ComMap::iterator itor = pkRewardEntry->m_kCurrentPrices.begin();
	sTemp << itor->first << "#" << itor->second << "#";

	Utility::UT_SIMDataMap::iterator it = pkRewardEntry->m_kLimits.begin();
	int iType = LC_ST_TP_NA;
	int iValue = 0;
	if (it != pkRewardEntry->m_kLimits.end())
	{
		Utility::UT_SIMDataInfo& rkLimit = it->second;
		iType = rkLimit.StatType();
		iValue = rkLimit.StatValue();
	}
	int iAvailableCount = 0;
	ACT_ScheduleSaveEntry* pkSaveEntry = _GetScheduleSaveEntry(ActivityType, iCatagory, ScheduleID);
	ACT_ScheduleSaveCommon* pkSaveCommon = pkSaveEntry ? pkSaveEntry->GetScheduleSaveDetail(iKey) : NULL;
	if (pkSaveCommon == NULL)
	{
		iAvailableCount = iValue;
	}
	else
	{
		iAvailableCount = iValue - pkSaveCommon->GetValue(GET_SCHEDULE_KEY(iType, ACT_SCHEDULE_DETAIL_COUNT));
	}
	sTemp << iAvailableCount << "#" << iType << "#" << iValue << "#";
	sTemp << pkRewardEntry->m_iScore <<"#";
	sTemp << (bBind ? 1 : 0) << "#";
	sTemp << (pkRewardEntry->m_bForbidExpire ? 1 : 0) << "#";

	//购买限制
	for (UT_SIMDataList::const_iterator conit = pkRewardEntry->m_kRewardCondition.begin(); conit != pkRewardEntry->m_kRewardCondition.end(); ++conit)
	{
		const UT_SIMDataInfo& kinfo = *conit;
		sTemp << kinfo.ID() << "|" << kinfo.ParamA() << "|";
	}
	sTemp << "#"<<pkRewardEntry->m_iCustomA<<"#"<<pkRewardEntry->m_iCustomB<<"#"<<pkRewardEntry->m_SCustomStr<<"#";

	sTemp << pkRewardEntry->m_iGuildLevel << "#";
	sTemp << pkRewardEntry->m_sIcon2 << "#" << Int2String(pkRewardEntry->m_iRank);
	return sTemp.str();
}

StringType GameLogic::LC_ClientActivityAsset::GetShopRewardInfo(int32_t ActivityType, int32_t iCatagory, int32_t ScheduleID, int32_t iKey)
{
	LC_ClientActivityManager* pkActivityMgr = LC_ClientActivityManager::GetSingletonPtr();

	ACT_RewardDataEntry* pkRewardEntry = pkActivityMgr->GetRewardDataEntry(ActivityType,iCatagory,ScheduleID,iKey);
	if (pkRewardEntry == NULL)
	{
		return "";
	}

	const Utility::UT_SIMDataList *pkRewardList = pkRewardEntry->m_kRewardItems.GetAllItemList();
	if (pkRewardList->empty())
	{
		return "";
	}

	StringStreamType sTemp("");	
	bool bBind = false;

	//购买限制
	for (UT_SIMDataList::const_iterator conit = pkRewardList->begin(); conit != pkRewardList->end(); ++conit)
	{
		const UT_SIMDataInfo& kinfo = *conit;
		sTemp << kinfo.ID() << "|" << kinfo.ParamA() << "|"<<kinfo.ItemBindMode()<<"#";
	}
	return sTemp.str();
}


StringType GameLogic::LC_ClientActivityAsset::GetShopRewardName(int32_t ActivityType, int32_t iCatagory, int32_t ScheduleID, int32_t iKey)
{
	LC_ClientActivityManager* pkActivityMgr = LC_ClientActivityManager::GetSingletonPtr();

	ACT_RewardDataEntry* pkRewardEntry = pkActivityMgr->GetRewardDataEntry(ActivityType,iCatagory,ScheduleID,iKey);
	if (pkRewardEntry == NULL)
	{
		return "";
	}

	return pkRewardEntry->m_sGiftName;
}

StringType GameLogic::LC_ClientActivityAsset::GetLotteryActivityButton(int32_t ActivityType, int32_t iCatagory, int32_t ScheduleID, int32_t iKey)
{
	LC_ClientActivityManager* pkActivityMgr = LC_ClientActivityManager::GetSingletonPtr();

	ACT_RewardDataEntry* pkRewardEntry = pkActivityMgr->GetRewardDataEntry(ActivityType,iCatagory,ScheduleID,iKey);
	if (pkRewardEntry == NULL)
	{
		return "";
	}

	return pkRewardEntry->m_sButtonIcon1;
}

int32_t GameLogic::LC_ClientActivityAsset::GetShopRewardiID(int32_t ActivityType, int32_t iCatagory, int32_t ScheduleID, int32_t iKey)
{
	LC_ClientActivityManager* pkActivityMgr = LC_ClientActivityManager::GetSingletonPtr();

	ACT_RewardDataEntry* pkRewardEntry = pkActivityMgr->GetRewardDataEntry(ActivityType,iCatagory,ScheduleID,iKey);
	if (pkRewardEntry == NULL)
	{
		return 0;
	}

	return pkRewardEntry->m_iID;
}

int32_t GameLogic::LC_ClientActivityAsset::GetShopRewardMaxCount(int32_t ActivityType, int32_t iCatagory, int32_t ScheduleID, int32_t iKey)
{
	LC_ClientActivityManager* pkActivityMgr = LC_ClientActivityManager::GetSingletonPtr();

	ACT_RewardDataEntry* pkRewardEntry = pkActivityMgr->GetRewardDataEntry(ActivityType,iCatagory,ScheduleID,iKey);
	if (pkRewardEntry == NULL)
	{
		return 0;
	}

	return pkRewardEntry->m_iMaxCount;
}

int32_t GameLogic::LC_ClientActivityAsset::GetPersistentVal(int32_t ActivityType,int32_t id)
{
	int mType = ACT_SCHEDULE_PERSISTENT_COUNT + 1000 * (id - 1);
	return GetScheduleSaveContainer(ActivityType)->GetPersistentVal(mType);
}

bool ACT_ScheduleEntry::Init(CSVFile::CF_ActivitySchedule::DataEntry* pkEntry )
{
	if (NULL == pkEntry)
	{
		return false;
	}

	m_iID = pkEntry->_iID;
	m_sCron = pkEntry->_sCron;
	m_iTimeLen = pkEntry->_iTimeLen;
	m_kStartCoolTime = pkEntry->_StartCoolTime;
	m_kStartValidTime = pkEntry->_StartValidTime;
	m_kConflateCoolTime = pkEntry->_ConflateCoolTime;
	m_kConflateValidTime = pkEntry->_ConflateValidTime;
	m_kValidServers = pkEntry->_kValidServers;
	m_kExceptServers = pkEntry->_kExceptServers;	
	m_kSchedule.Clear();
	return true;
}

void ACT_ScheduleEntry::Unit()
{
	m_kSchedule.Clear();
}

void ACT_ScheduleEntry::SetUp(uint32_t starttime, uint32_t conflatetime)
{
	/*m_kSchedule.Clear();
	Utility::UT_TimePeriod kStart;
	
	int64_t iStartCoolTime = (int64_t)m_kStartCoolTime.GetTimeStramp(starttime)*ONE_SECOND_MILI;
	int64_t iStartValidTime = m_kStartValidTime.GetTimeStramp()>0 ? (int64_t)m_kStartValidTime.GetTimeStramp(starttime)*ONE_SECOND_MILI : ST_MAX_TIMESTAMP_MILI;
	kStart.Init(iStartCoolTime, iStartValidTime);

	if (m_kConflateCoolTime.GetTimeStramp() || m_kConflateValidTime.GetTimeStramp())
	{
		Utility::UT_TimePeriod kConflate;
		if (conflatetime)
		{
			kConflate.Init(m_kConflateCoolTime.GetTimeStramp(conflatetime)*ONE_SECOND_MILI, m_kConflateValidTime.GetTimeStramp(conflatetime)*ONE_SECOND_MILI);
		}
		else
		{
			kConflate.Init(0, 1);
		}
		m_kSchedule.SetPeriod(Merge(kStart, kConflate));
	}
	else
	{
		m_kSchedule.SetPeriod(kStart);
	}

	m_kSchedule.m_sCronStr = m_sCron;
	m_kSchedule.m_iTimeLen = m_iTimeLen * ONE_MINUTE_SECONDS * ONE_SECOND_MILI;*/
}

bool ACT_ScheduleEntry::IsServerValid(int32_t iUnionID, int32_t iServerID)
{
	if (!m_kExceptServers.empty())
	{
		Utility::UT_SIMDataList::iterator it = m_kExceptServers.begin();
		for (; it!=m_kExceptServers.end(); ++it)
		{
			UT_SIMDataInfo& rkSvrInfo = *it;
			if (rkSvrInfo.IsServerIn(iUnionID, iServerID))
			{
				return false;
			}
		}
	}

	if (!m_kValidServers.empty())
	{
		Utility::UT_SIMDataList::iterator it = m_kValidServers.begin();
		for (; it!=m_kValidServers.end(); ++it)
		{
			UT_SIMDataInfo& rkSvrInfo = *it;
			if (rkSvrInfo.IsServerIn(iUnionID, iServerID))
			{
				return true;
			}
		}
		return false;
	}
	return true;
}

bool ACT_ScheduleEntry::IsTimestampIn(int64_t timestamp)
{
	return m_kSchedule.IsTimestampIn(timestamp);
}

void GameLogic::ACT_ScheduleEntry::Rechedule(Utility::timer_id_type timerID)
{
	/*UT_TimerManager* pkTimerMgr = UT_TimerManager::GetSingletonPtr();
	if (m_bActive)
	{
		return;
	}

	int64_t curtimestamp = (int64_t)(GET_SERVER_SYSTEM_TIME * ONE_SECOND_MILI);
	Utility::UT_TimePeriod& rkPeriod = m_kSchedule.m_Period;
	if (curtimestamp >= rkPeriod.m_iEnd)
	{
		return;
	}
	else if (curtimestamp < rkPeriod.m_iStart)
	{
		m_kSchedule.m_iScheduleTimer = pkTimerMgr->DelayCall(rkPeriod.m_iStart-curtimestamp, LC_ClientActivityManager::_rescheduleCB, (ActivityScheduleCallback)NULL, m_iID);
	}
	else
	{
		uint32_t endTime = 0;
		uint32_t beginTime = 0;
		bool bCronTime = LC_ClientHelp::CheckCronTimeStamp(m_kSchedule.m_sCronStr, m_iTimeLen, beginTime, endTime);
		if (bCronTime || beginTime > 0)
		{
			m_kSchedule.m_iStartTime = (int64_t)beginTime * ONE_SECOND_MILI > rkPeriod.m_iStart ? (int64_t)beginTime * ONE_SECOND_MILI : rkPeriod.m_iStart;
			m_kSchedule.m_iEndTime = (int64_t)endTime * ONE_SECOND_MILI < rkPeriod.m_iEnd ? (int64_t)endTime * ONE_SECOND_MILI : rkPeriod.m_iEnd;
		}
		else
		{
			m_kSchedule.m_iStartTime = rkPeriod.m_iStart;
			m_kSchedule.m_iEndTime = (rkPeriod.m_iStart + m_kSchedule.m_iTimeLen) < rkPeriod.m_iEnd ? (rkPeriod.m_iStart + m_kSchedule.m_iTimeLen) : rkPeriod.m_iEnd;
		}
		int64_t iDelay = m_kSchedule.m_iStartTime - curtimestamp;
		m_kSchedule.m_iScheduleTimer = pkTimerMgr->DelayCall(iDelay, LC_ClientActivityManager::_activeCB, (ActivityScheduleCallback)NULL, m_iID);
	}*/
}

void GameLogic::ACT_ScheduleEntry::Active(Utility::timer_id_type timerID)
{
	//UT_TimerManager* pkTimerMgr = UT_TimerManager::GetSingletonPtr();
	//if (!m_bActive)
	//{
	//	m_bActive = true;
	//	int64_t curTimeStamp = (int64_t)(GET_SERVER_SYSTEM_TIME * ONE_SECOND_MILI);
	//	int64_t iDelay = m_kSchedule.m_iEndTime-curTimeStamp;

	//	if(IsTimerIDValid(m_kSchedule.m_iCancelTimer))
	//	{
	//		pkTimerMgr->CancelTimer(m_kSchedule.m_iCancelTimer);
	//		m_kSchedule.m_iCancelTimer = Utility::TIMER_ID_NA;
	//	}

	//	m_kSchedule.m_iCancelTimer = pkTimerMgr->DelayCall(iDelay, LC_ClientActivityManager::_deactiveCB, (ActivityScheduleCallback)NULL, m_iID);

	//	LC_GameEvent kGameEvent;
	//	kGameEvent.SetEventType(GLET_CLIENT_ACTIVITY_ACTIVE_CHANGE);
	//	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, m_iID);
	//	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, 1);//active
	//	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	//	pkUIManager->HandlerGameEvent(kGameEvent);	
	//}
}

void GameLogic::ACT_ScheduleEntry::Deactive(Utility::timer_id_type timerID)
{
	//UT_TimerManager* pkTimerMgr = UT_TimerManager::GetSingletonPtr();
	//if (m_bActive)
	//{
	//	m_bActive = false;
	//	Rechedule(timerID);

	//	if(IsTimerIDValid(m_kSchedule.m_iCancelTimer))
	//	{
	//		pkTimerMgr->CancelTimer(m_kSchedule.m_iCancelTimer);
	//		m_kSchedule.m_iCancelTimer = TIMER_ID_NA;
	//	}
	//	LC_GameEvent kGameEvent;
	//	kGameEvent.SetEventType(GLET_CLIENT_ACTIVITY_ACTIVE_CHANGE);
	//	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, m_iID);
	//	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, 0);//deactive
	//	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	//	pkUIManager->HandlerGameEvent(kGameEvent);	
	//}
}
template <typename DataSource, typename DataSourceA>
void LC_ClientActivityManager::_InitActivityData(CSVFile::CSVDataLoader* pkLoader, const CSVFile::CSVDataSourceList* pkCSVs, int iActivityType)
{
	DataSource* inst = SafeGetSingleton<DataSource>();
	DataSourceA* instA = SafeGetSingleton<DataSourceA>();
	if(IS_VALID_RELATED_CSV(inst, pkCSVs) || IS_VALID_RELATED_CSV(instA, pkCSVs))
	{
		ACT_ScheduleDataContainer& kContainer = m_DataContainerMap[iActivityType];
		kContainer.Clear();

		DataSource::DataEntryMapExternIterator iter = inst->GetIterator();
		while(iter.HasMoreElements())
		{
			DataSource::DataEntry* entry = iter.PeekNextValuePtr();
			ACT_RewardDataEntry kDataEntry;
			if (kDataEntry.Init(entry))
			{
				kContainer.AddDataEntry(kDataEntry.m_iType, kDataEntry.m_iSchdeule, kDataEntry.m_iKey, kDataEntry);
				_AddScheduleEvent(kDataEntry.m_iSchdeule, iActivityType, kDataEntry.m_iType);
			}
			iter.MoveNext();
		}
		DataSourceA::DataEntryMapExternIterator iterA = instA->GetIterator();
		while(iterA.HasMoreElements())
		{
			DataSourceA::DataEntry* entry = iterA.PeekNextValuePtr();
			ACT_RewardDataEntry kDataEntry;
			if (kDataEntry.Init(entry))
			{
				kContainer.AddDataEntry(kDataEntry.m_iType, kDataEntry.m_iSchdeule, kDataEntry.m_iKey, kDataEntry);
				_AddScheduleEvent(kDataEntry.m_iSchdeule, iActivityType, kDataEntry.m_iType);
			}
			iterA.MoveNext();
		}
	}
}
template <typename DataSource>
void LC_ClientActivityManager::_InitActivityData(CSVFile::CSVDataLoader* pkLoader, const CSVFile::CSVDataSourceList* pkCSVs, int iActivityType)
{
	DataSource* inst = SafeGetSingleton<DataSource>();
	if(IS_VALID_RELATED_CSV(inst, pkCSVs))
	{
		ACT_ScheduleDataContainer& kContainer = m_DataContainerMap[iActivityType];
		kContainer.Clear();

		DataSource::DataEntryMapExternIterator iter = inst->GetIterator();
		while(iter.HasMoreElements())
		{
			DataSource::DataEntry* entry = iter.PeekNextValuePtr();
			ACT_RewardDataEntry kDataEntry;
			if (kDataEntry.Init(entry))
			{
				kContainer.AddDataEntry(kDataEntry.m_iType, kDataEntry.m_iSchdeule, kDataEntry.m_iKey, kDataEntry);
				_AddScheduleEvent(kDataEntry.m_iSchdeule, iActivityType, kDataEntry.m_iType);
			}
			iter.MoveNext();
		}
	}
}
void LC_ClientActivityManager::Init(CSVFile::CSVDataLoader* pkLoader, const CSVFile::CSVDataSourceList* pkCSVs)
{
	{
		CSVFile::CF_ActivitySchedule* inst = SafeGetSingleton<CSVFile::CF_ActivitySchedule>();
		if(IS_VALID_RELATED_CSV(inst, pkCSVs))
		{
			UnitSchedule();		
			CSVFile::CF_ActivitySchedule::DataEntryMapExternIterator iter = inst->GetIterator();
			while(iter.HasMoreElements())
			{
				CSVFile::CF_ActivitySchedule::DataEntry* entry = iter.PeekNextValuePtr();

				ACT_ScheduleEntry kDataEntry;
				if (kDataEntry.Init(entry))
				{
					m_kScheduleMap[kDataEntry.m_iID] = kDataEntry;
				}
				iter.MoveNext();
			}
		}
	}
	_InitActivityData<CSVFile::CF_ACTSubordinateReward, CSVFile::CF_ACTSubordinateRewardA>(pkLoader, pkCSVs, Activity::ACT_SCHEDULE_TP_SUBORDINATE);
	_InitActivityData<CSVFile::CF_ACTSubordinateRewardB>(pkLoader, pkCSVs, Activity::ACT_SCHEDULE_TP_SUBORDINATE_B);
	_InitActivityData<CSVFile::CF_ACTChargeProfit, CSVFile::CF_ACTChargeProfitA>(pkLoader, pkCSVs, Activity::ACT_SCHEDULE_TP_CHARGE);
	_InitActivityData<CSVFile::CF_ACTShop, CSVFile::CF_ACTShopA>(pkLoader, pkCSVs, Activity::ACT_SCHEDULE_TP_SHOP);
	_InitActivityData<CSVFile::CF_ACTShopB>(pkLoader, pkCSVs, Activity::ACT_SCHEDULE_TP_SHOP_B);
	_InitActivityData<CSVFile::CF_ACTSubordinateConsume>(pkLoader, pkCSVs, Activity::ACT_SCHEDULE_TP_SUBORDINATE_CONSUME);
	_InitActivityData<CSVFile::CF_ACTShot>(pkLoader, pkCSVs, Activity::ACT_SCHEDULE_TP_SHOT);
	_InitActivityData<CSVFile::CF_ACTLottery>(pkLoader, pkCSVs, Activity::ACT_SCHEDULE_TP_LOTTERY);
	_InitActivityData<CSVFile::CF_HolidayActivity_PinTuReward>(pkLoader, pkCSVs, Activity::ACT_SCHEDULE_TP_PINTU);
	_InitActivityData<CSVFile::CF_HolidayActivity_ChargeContinuous>(pkLoader,pkCSVs,Activity::ACT_SCHEDULE_TP_CHARGE_REBATES); 


	//reload后重新调度活动
	if (GET_SERVER_OPEN_TIME || GET_SERVER_CONFLAT_TIME)
	{
		Schedule(GET_SERVER_OPEN_TIME, GET_SERVER_CONFLAT_TIME);
	}
	_UpdateAllScheduleActiveInfo();
}

void LC_ClientActivityManager::Unit(void)
{
	UnitSchedule();
	ACT_ScheduleDataContainerMap::iterator it = m_DataContainerMap.begin();
	for (; it!=m_DataContainerMap.end(); ++it)
	{
		it->second.Clear();
	}
	m_DataContainerMap.clear();
	m_kSuperSubRankInfoMap.clear();
	m_kHolidayActRankInfoMap.clear();
	m_kSuperSubValueMap.clear();
	m_kSuperSubBValueMap.clear();
	m_mLotteryRecord.clear();
	m_mLotteryDataMap.clear();
	m_mChargeProfitDataMap.clear();
}
void LC_ClientActivityManager::Schedule(uint32_t starttime, uint32_t conflatetime)
{
	//使用服务器Schedule版本
	/*T_PROFILE_TIMER("LC_ClientActivityManager::Schedule");
	ACT_ScheduleEntryMap::iterator it = m_kScheduleMap.begin();
	for (; it!=m_kScheduleMap.end(); ++it)
	{
		it->second.SetUp(starttime, conflatetime);
		it->second.Rechedule(Utility::TIMER_ID_NA);
	}*/
}
bool LC_ClientActivityManager::IsScheduleValid(LC_PlayerBase* pkPlayer, int32_t iSchedule)
{
	IF_A_NA_PLAYER(pkPlayer)
	{
		return false;
	}

	if (iSchedule == -1)
	{
		return true;
	}

	ACT_ScheduleEntry* pkScheduleEntry = GetSchedule(iSchedule);
	if (NULL == pkScheduleEntry)
	{
		return false;
	}
	return pkScheduleEntry->m_bActive && pkScheduleEntry->IsServerValid(GER_SERVER_UNION_ID(), pkPlayer->GetServerID());
}

void GameLogic::LC_ClientActivityManager::UnitSchedule(void)
{
	ACT_ScheduleEntryMap::iterator it = m_kScheduleMap.begin();
	for (; it!=m_kScheduleMap.end(); ++it)
	{
		it->second.Unit();
	}
	m_kScheduleMap.clear();
	m_kShceduleEventIndices.clear();
}

StringType GameLogic::LC_ClientActivityManager::GetScheduleImage(int32_t iScheduleId)
{
	mem::map<int32_t,StringType>::iterator it=ScheduleImageMap.find(iScheduleId);
	if (it!=ScheduleImageMap.end())
	{
		return it->second;
	}
	return "";
}

StringType LC_ClientActivityManager::GetTimeIdImage(int iTimeID)
{
	mem::map<int32_t,StringType>::iterator it=TimeIdImageMap.find(iTimeID);
	return it!=TimeIdImageMap.end() ? it->second : "";
}

int32_t GameLogic::LC_ClientActivityManager::GetActiveSchedule(int32_t ActivityType, int32_t iCatagory, LC_PlayerBase* pkPlayer, bool bFlag)
{
	return _getActiveSchedule(ActivityType, iCatagory, pkPlayer, bFlag, true);
}

int32_t GameLogic::LC_ClientActivityManager::GetActiveScheduleAlways(int32_t ActivityType, int32_t iCatagory, LC_PlayerBase* pkPlayer, bool bFlag)
{
	return _getActiveSchedule(ActivityType, iCatagory, pkPlayer, bFlag, false);
}

StringType GameLogic::LC_ClientActivityManager::GetScheduleList(int32_t ActivityType, int32_t iCatagory, LC_ClientGamePlayer* pkPlayer)
{
	ACT_ScheduleDataCatagory *pkCatagory = GetScheduleDataCatagory(ActivityType, iCatagory);
	if (NULL == pkCatagory)
	{
		return "0#";
	}
	const GameLogic::LC_IDSet& kScheduleIDs = pkCatagory->GetScheduleIDs();
	if (kScheduleIDs.empty())
	{
		return "0#";
	}
	StringStreamType sStream;
	sStream << kScheduleIDs.size() << "#";
	for (GameLogic::LC_IDSet::const_iterator it = kScheduleIDs.begin(); it != kScheduleIDs.end(); ++it)
	{
		sStream << *it << "#";
		if(ActivityType == ACT_SCHEDULE_TP_SHOP_B)
			sStream << (IsTimeIdValid(pkPlayer, *it) ? 1 : 0);
		else
			sStream << (IsScheduleValid(pkPlayer, *it) ? 1 : 0);

		sStream << "#";
	}
	return sStream.str();
}
int32_t GameLogic::LC_ClientActivityManager::GetRewardMaxKey(int32_t ActivityType, int32_t iCatagory, int32_t iSchedule)
{
	ACT_ScheduleDataEntry *pkDataEntry = GetScheduleDataEntry(ActivityType, iCatagory, iSchedule);
	if (NULL == pkDataEntry)
	{
		return 0;
	}
	const GameLogic::LC_IDSet& KeyIDs = pkDataEntry->KeyIDs();
	if (KeyIDs.empty())
	{
		return 0;
	}
	else
	{
		return *(std::max_element(KeyIDs.begin(),KeyIDs.end()));
	}
}

StringType GameLogic::LC_ClientActivityManager::GetRewardKeyIDsInfo(int32_t ActivityType, int32_t iCatagory, int32_t iSchedule)
{
	ACT_ScheduleDataEntry *pkDataEntry = GetScheduleDataEntry(ActivityType, iCatagory, iSchedule);
	if (NULL == pkDataEntry)
	{
		return "0#";
	}
	const ACT_RewardDataEntryMap& kEntrys = pkDataEntry->entrys();
	StringStreamType sStream;
	sStream << kEntrys.size() << "#";
	for (ACT_RewardDataEntryMap::const_iterator it = kEntrys.begin(); it != kEntrys.end(); ++it)
	{
		sStream << it->second.m_iKey << "#";
	}
	return sStream.str();

}

StringType GameLogic::LC_ClientActivityManager::GetRewardKeyIDsInfoEx(int32_t ActivityType, int32_t iCatagory, int32_t iSchedule)
{
	ACT_ScheduleDataEntry *pkDataEntry = GetScheduleDataEntry(ActivityType, iCatagory, iSchedule);
	if (NULL == pkDataEntry)
	{
		return "0#";
	}
	const ACT_RewardDataEntryMap& kEntrys = pkDataEntry->entrys();
	StringStreamType sStream;
	sStream << kEntrys.size() << "#";
	for (ACT_RewardDataEntryMap::const_iterator it = kEntrys.begin(); it != kEntrys.end(); ++it)
	{
		sStream << it->second.m_iKey << "#" << it->second.m_iParentKey << "#" << it->second.m_iCustomA << "#" << it->second.m_iCustomB << "#";
	}
	return sStream.str();

}

StringType GameLogic::LC_ClientActivityManager::GetRewardKeyIDsWithUIRank(int32_t ActivityType, int32_t iCatagory, int32_t iSchedule)
{
	ACT_ScheduleDataEntry *pkDataEntry = GetScheduleDataEntry(ActivityType, iCatagory, iSchedule);
	if (NULL == pkDataEntry)
	{
		return "0#";
	}
	const ACT_RewardDataEntryMap& kEntrys = pkDataEntry->entrys();
	StringStreamType sStream;
	sStream << kEntrys.size() << "#";
	for (ACT_RewardDataEntryMap::const_iterator it = kEntrys.begin(); it != kEntrys.end(); ++it)
	{
		sStream << it->second.m_iKey << "#" << it->second.m_iRank << "#";
	}
	return sStream.str();

}

StringType GameLogic::LC_ClientActivityManager::GetRewardChoiceIDsInfo(int32_t ActivityType, int32_t iCatagory, int32_t iSchedule, int32_t iKey, LC_PlayerBase* pkPlayer)
{
	ACT_RewardDataEntry* pkRewardEntry = GetRewardDataEntry(ActivityType,iCatagory,iSchedule,iKey);
	if (pkRewardEntry == NULL)
	{
		return "0#";
	}
	StringStreamType sStream("");
	LC_IDSet kChoices;
	pkRewardEntry->m_kRewardItems.CheckChoice(pkPlayer->GetWeaponAttribute(), kChoices);
	sStream << kChoices.size() << "#";
	for (LC_IDSet::const_iterator it = kChoices.begin(); it != kChoices.end(); ++it)
	{
		sStream << *it << "#";
	}
	return sStream.str();
}

void GameLogic::LC_ClientActivityManager::_rescheduleCB(Utility::timer_id_type timerID, int32_t iID)
{
	/*static LC_ClientActivityManager* pkActivityMgr = LC_ClientActivityManager::GetSingletonPtr();
	ACT_ScheduleEntry* pkSchedule = pkActivityMgr->GetSchedule(iID);
	if (pkSchedule)
	{
		pkSchedule->Rechedule(timerID);
		pkActivityMgr->AddDirtyFlagRevisionID();
	}*/
}

void GameLogic::LC_ClientActivityManager::_activeCB(Utility::timer_id_type timerID, int32_t iID)
{
	/*static LC_ClientActivityManager* pkActivityMgr = LC_ClientActivityManager::GetSingletonPtr();
	ACT_ScheduleEntry* pkSchedule = pkActivityMgr->GetSchedule(iID);
	if (pkSchedule)
	{
		pkSchedule->Active(timerID);
		pkActivityMgr->AddDirtyFlagRevisionID();
	}*/
}

void GameLogic::LC_ClientActivityManager::_deactiveCB(Utility::timer_id_type timerID, int32_t iID)
{
	/*static LC_ClientActivityManager* pkActivityMgr = LC_ClientActivityManager::GetSingletonPtr();
	ACT_ScheduleEntry* pkSchedule = pkActivityMgr->GetSchedule(iID);
	if (pkSchedule)
	{
		pkSchedule->Deactive(timerID);
		pkActivityMgr->AddDirtyFlagRevisionID();
	}*/
}

StringType GameLogic::LC_ClientActivityManager::GetScheduleIDEventParams(int32_t iScheule)
{
	const ScheduleEventSet& kEventVector = m_kShceduleEventIndices[iScheule];
	if (kEventVector.empty())
	{
		return "";
	}
	StringType sResult = Int2String(kEventVector.size()) + "#";
	for (ScheduleEventSet::const_iterator it = kEventVector.begin(); it != kEventVector.end(); ++it)
	{
		const ScheduleEvent& kEventParams = *it;
		sResult = sResult + Int2String(kEventParams.iActivityType) + "#";
		sResult = sResult + Int2String(kEventParams.iCatagory) + "#";
	}
	return sResult;
}

StringType GameLogic::LC_ClientActivityManager::GetShopPriceInfo(int32_t ActivityType, int32_t iCatagory, int32_t iSchedule, int32_t iKey, bool bOrigin)
{
	ACT_RewardDataEntry* pkRewardEntry = GetRewardDataEntry(ActivityType,iCatagory,iSchedule,iKey);
	if (pkRewardEntry == NULL)
	{
		return "";
	}
	StringStreamType sTemp("");
	const Utility::LC_CashMap* pkCashMap = &pkRewardEntry->m_kCurrentPrices;
	if (bOrigin)
	{
		pkCashMap = &pkRewardEntry->m_kOriginalPrices;
	}
	sTemp << pkCashMap->size() << "#";
	for (Utility::LC_ComMap::const_iterator it = pkCashMap->begin(); it != pkCashMap->end(); ++it)
	{
		sTemp << it->first << "#" << it->second << "#";
	}
	return sTemp.str();
}

StringType GameLogic::LC_ClientActivityManager::GetActScoreInfo(int32_t ActivityType, int32_t iCatagory, int32_t iSchedule, int32_t iKey, bool bOrigin)
{
	ACT_RewardDataEntry* pkRewardEntry = GetRewardDataEntry(ActivityType,iCatagory,iSchedule,iKey);
	if (pkRewardEntry == NULL)
	{
		return "";
	}
	StringStreamType sTemp("");
	for (UT_SIMDataList::const_iterator conit = pkRewardEntry->m_kActScore.begin(); conit != pkRewardEntry->m_kActScore.end(); ++conit)
	{
		const UT_SIMDataInfo& kinfo = *conit;
		sTemp << kinfo.ID() << "|" << kinfo.ParamA() << "|"<< kinfo.ParamB() << "|";
	}
	return sTemp.str();
}

void GameLogic::LC_ClientActivityManager::GetDataFromPB(const msg_define::schedule_info&info)
{
	for (size_t i = 0; i < info.schedules_size(); ++i)
	{
		const msg_define::schedule_entry& kentry = info.schedules(i);
		m_kScheduleActiveMap[kentry.id()] = ScheduleActiveInfo(kentry.id(), kentry.active(), kentry.start(), kentry.end());

		ACT_ScheduleEntry* pkScheduleEntry = GetSchedule(kentry.id());
		if (pkScheduleEntry)
		{
			bool oldActive = pkScheduleEntry->m_bActive;
#ifndef _SHIPPING
			if (oldActive || kentry.active())
			{
				GfxWriteLog(LOG_SYSTEM_FILELINE, LOG_SWITCH_INFO,"schedule info id = [%i], active = [%i], oldactive = [%i]", kentry.id(), kentry.active() ? 1 : 0, oldActive ? 1 : 0);
			}
#endif
			pkScheduleEntry->m_bActive = kentry.active();
			pkScheduleEntry->m_kSchedule.m_iStartTime = kentry.start();
			pkScheduleEntry->m_kSchedule.m_iEndTime = kentry.end();

//			if ((!oldActive && kentry.active())||(oldActive && !kentry.active()))
			if (oldActive != kentry.active())
			{
				LC_GameEvent kGameEvent;
				kGameEvent.SetEventType(GLET_CLIENT_ACTIVITY_ACTIVE_CHANGE);
				kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, kentry.id());
				kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, kentry.active()?1:0);//active
				UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
				pkUIManager->HandlerGameEvent(kGameEvent);	
			}
		}
	}
	AddDirtyFlagRevisionID();
}

void GameLogic::LC_ClientActivityManager::_UpdateAllScheduleActiveInfo()
{
	for (ScheduleActiveInfoMap::const_iterator it = m_kScheduleActiveMap.begin(); it != m_kScheduleActiveMap.end(); ++it)
	{
		const ScheduleActiveInfo& kInfo = it->second;
		ACT_ScheduleEntry* pkScheduleEntry = GetSchedule(kInfo.iScheduleID);
		if (pkScheduleEntry)
		{
			pkScheduleEntry->m_bActive = kInfo.bActive;
			pkScheduleEntry->m_kSchedule.m_iStartTime = kInfo.m_iStartTime;
			pkScheduleEntry->m_kSchedule.m_iEndTime = kInfo.m_iEndTime;
		}
	}
	AddDirtyFlagRevisionID();
}

bool GameLogic::LC_ClientActivityManager::CheckActivityChoice(int32_t ActivityType, int32_t iCatagory, int32_t iSchedule, LC_PlayerBase* pkPlayer)
{
	if (pkPlayer == NULL)
	{
		return false;
	}
	ACT_ScheduleDataEntry *pkDataEntry = GetScheduleDataEntry(ActivityType, iCatagory, iSchedule);
	if (NULL == pkDataEntry)
	{
		return false;
	}
	const ACT_RewardDataEntryMap& kEntrys = pkDataEntry->entrys();
	WeaponType iWeaponType = pkPlayer->GetWeaponAttribute();
	LC_IDSet kIDSet;
	for (ACT_RewardDataEntryMap::const_iterator it = kEntrys.begin(); it != kEntrys.end(); ++it)
	{
		kIDSet.clear();
		const GameLogic::LC_ItemsReward& kReward = it->second.m_kRewardItems;
		kReward.CheckChoice(iWeaponType, kIDSet);
		if (kIDSet.size() >= 2)
		{
			return true;
		}
	}
	return false;
}

int32_t GameLogic::LC_ClientActivityManager::_getActiveSchedule(int32_t ActivityType, int32_t iCatagory, LC_PlayerBase* pkPlayer, bool bFlag /* = false */, bool bCheckValid/* =true */)
{
	ACT_ScheduleDataCatagory *pkCatagory = GetScheduleDataCatagory(ActivityType, iCatagory);
	if (NULL == pkCatagory)
	{
		return 0;
	}
	
	const GameLogic::LC_IDSet& kScheduleIDs = pkCatagory->GetScheduleIDs();
	if (kScheduleIDs.empty())
	{
		return 0;
	}
	
	for (GameLogic::LC_IDSet::const_iterator it = kScheduleIDs.begin(); it != kScheduleIDs.end(); ++it)
	{
		//现在只有进阶榜 ACT_SCHEDULE_TP_SUBORDINATE 是特殊的，有高级类型
		if ( ActivityType != ACT_SCHEDULE_TP_SUBORDINATE && (!bCheckValid || IsScheduleValid(pkPlayer, *it)) && !bFlag )
		{
			return *it;
		}

		ACT_ScheduleDataEntry* pkDataEntry = pkCatagory->GetScheduleData(*it);
		if(pkDataEntry != NULL)
		{
			const ACT_RewardDataEntryMap& dataMap = pkDataEntry->entrys();
			ACT_RewardDataEntryMap::const_iterator iter = dataMap.begin();
			for(; iter!=dataMap.end(); ++iter)
			{
				const ACT_RewardDataEntry& rwdEntry = iter->second;
				if(bFlag == rwdEntry.m_bHighReward 
					&& (!bCheckValid || IsScheduleValid(pkPlayer, *it)))
				{
					return *it;
				}
			}
		}
	}
	return 0;
}

void GameLogic::LC_ClientActivityManager::GetDataFromStruct(const MG_RLT_GetRankSubBInfo& msg)
{
	mem::map<int, PS_SubBInfo>* pkMap = NULL;
	if(msg.m_nType == Charge_Type_3)
	{
		pkMap = &m_kHolidayActRankInfoMap[msg.m_nID];
	}
	else
	{
		pkMap = &m_kSuperSubRankInfoMap[msg.m_nID];
	}

	mem::map<int, PS_SubBInfo>& kMap = *pkMap;

	m_iSubRankListCount = msg.m_nTotalCount;

	kMap.clear();
	for(size_t i=0; i<msg.m_nPlayerInfo.size(); ++i)
	{
		const PS_SubBInfo& subInfo = msg.m_nPlayerInfo[i];
		kMap[subInfo.m_nRankIndex] = subInfo;
	}

	const PS_SubBInfo& kSelfInfo = msg.m_nMyselfInfo;
	m_kSelfSubRankInfo.m_nRankIndex = kSelfInfo.m_nRankIndex;
	m_kSelfSubRankInfo.m_nCharName = kSelfInfo.m_nCharName;
	m_kSelfSubRankInfo.m_nValue = kSelfInfo.m_nValue;

	m_iSubRankFirstCID = msg.m_nFirstCID;
	m_iSubRankFirstUID = msg.m_nFirstCharID;

	AddDirtyFlagRevisionID();
}


void GameLogic::LC_ClientActivityManager::UpdateSubBValue(std::vector<PS_SubBValue>& m_nSubBValue)
{
	for(int i=0;i<m_nSubBValue.size();i++)
	{
		PS_SubBValue& val=m_nSubBValue[i];
		m_kSuperSubBValueMap[val.m_nGroupID]=val.m_nValue;
	}
}

StringType GameLogic::LC_ClientActivityManager::GetSubRankInfo(int iGroupID, int iRank)
{
	SuperSubRankMap::const_iterator iter = m_kSuperSubRankInfoMap.find(iGroupID);
	if(iter == m_kSuperSubRankInfoMap.end())
		return "";

	const mem::map<int, PS_SubBInfo>& subMap = iter->second;
	mem::map<int, PS_SubBInfo>::const_iterator itr = subMap.find(iRank);
	if(iRank < 0 || itr == subMap.end())
		return "";

	return _genSubRankData(iGroupID, itr->second);
}

StringType GameLogic::LC_ClientActivityManager::GetSelfSubRankInfo(int iGroupID)
{
	return _genSubRankData(iGroupID, m_kSelfSubRankInfo);
}

StringType GameLogic::LC_ClientActivityManager::GetSubRankCondition(int iScheduleID, int iType)
{
	CF_RankCondition::DataEntryMapExternIterator iter = CF_RankCondition::GetSingletonPtr()->GetIterator();
	for( ; iter.HasMoreElements(); iter.MoveNext())
	{
		const CF_RankCondition::DataEntry *pkEntry = iter.PeekNextValuePtr();
		if(pkEntry->_iType == iType && pkEntry->_iScheduleID == iScheduleID)
		{
			return CSVDataSourceBase::ReadFiledFromValue(pkEntry->_ConditionInfo);
		}
	}

	return "";
}

StringType GameLogic::LC_ClientActivityManager::GetHolidayACTRankInfo(int iID, int iRank)
{
	SuperSubRankMap::const_iterator iter = m_kHolidayActRankInfoMap.find(iID);
	if(iter == m_kHolidayActRankInfoMap.end())
		return "";

	const mem::map<int, PS_SubBInfo>& subMap = iter->second;
	mem::map<int, PS_SubBInfo>::const_iterator itr = subMap.find(iRank);
	if(iRank < 0 || itr == subMap.end())
		return "";

	return _genHolidayACTRankData(iID, itr->second);
}

StringType GameLogic::LC_ClientActivityManager::GetHolidayACTSelfSubRankInfo(int iID)
{
	return _genHolidayACTRankData(iID, m_kSelfSubRankInfo);
}

int64_t GameLogic::LC_ClientActivityManager::GetSubBValue(int iGroupID)
{
	mem::map<int32_t, int64_t>::const_iterator iter = m_kSuperSubBValueMap.find(iGroupID);
	if(iter != m_kSuperSubBValueMap.end())
		return (*iter).second;

	return -1;
}

StringType GameLogic::LC_ClientActivityManager::_genSubRankData(int iGroupID, const PS_SubBInfo& info)
{
	StringStreamType sStream;
	sStream << info.m_nRankIndex << '#' << info.m_nCharName << '#' << info.m_nValue << '#';

	sStream << _getSubRankReward<CF_ShotActivityLimitRankRewards>(iGroupID, info.m_nRankIndex);
	sStream << '#';
	sStream << info.m_nCharID << '#';
	return sStream.str();
}

StringType GameLogic::LC_ClientActivityManager::_genHolidayACTRankData(int iID, const PS_SubBInfo& info)
{
	StringStreamType sStream;
	sStream << info.m_nRankIndex << '#' << info.m_nCharName << '#' << info.m_nValue << '#';

	const CF_ACTChargeProfitB::DataEntry* pkEntry = SafeGetCSVFileDataEntryPtr<CF_ACTChargeProfitB>(iID);
	T_ASSERT_CRITICAL_S(pkEntry != NULL);

	sStream << _getSubRankReward<CF_ActivityRankReward>(pkEntry->_iType, info.m_nRankIndex);
	sStream << '#';
	sStream << info.m_nCharID << '#';
	return sStream.str();
}

StringType GameLogic::LC_ClientActivityManager::GetSubActiveScheduleInfos(int32_t iCatagory, LC_PlayerBase* pkPlayer)
{

	ACT_ScheduleDataCatagory *pkCatagory = GetScheduleDataCatagory(ACT_SCHEDULE_TP_SUBORDINATE_B, iCatagory);
	if (NULL == pkCatagory)
	{
		return "";
	}
	const GameLogic::LC_IDSet& kScheduleIDs = pkCatagory->GetScheduleIDs();
	if (kScheduleIDs.empty())
	{
		return "";
	}

	StringStreamType sStream;
	StringType sCondition;
	sCondition.reserve(30);

	mem::set<int32_t> kValidScheduleIDs;
	for (GameLogic::LC_IDSet::const_iterator it = kScheduleIDs.begin(); it != kScheduleIDs.end(); ++it)
	{
		ACT_ScheduleDataEntry* pkDataEntry = pkCatagory->GetScheduleData(*it);
		if(pkDataEntry != NULL)
		{
			const ACT_RewardDataEntryMap& dataMap = pkDataEntry->entrys();
			ACT_RewardDataEntryMap::const_iterator iterBeg = dataMap.begin();
			ACT_RewardDataEntryMap::const_iterator iterEnd = dataMap.end();
			for(; iterBeg!=iterEnd; ++iterBeg)
			{
				const ACT_RewardDataEntry& rwdEntry = iterBeg->second;
				if(IsScheduleValid(pkPlayer, rwdEntry.m_iCustomA) 
					&& kValidScheduleIDs.insert(rwdEntry.m_iCustomA).second)
				{
					sCondition.clear();
					FormatDataIFs2String(rwdEntry.m_kRewardCondition, sCondition);
					sStream << rwdEntry.m_iParentKey << '#' << *it << '#' << rwdEntry.m_iCustomA << '#' << sCondition << '|';
				}
			}
		}
	}

	return sStream.str();
}

//寻宝
//=================================================================================================
void GameLogic::LC_ClientActivityManager::SyncLotteryRecord(const MG_SyncLotteryRecord& msg)
{
	std::vector<PS_LotteryRecord>::const_iterator iter = msg.m_nLotteryRecord.begin();
	for( ; iter!=msg.m_nLotteryRecord.end(); ++iter)
	{
		m_mLotteryRecord[iter->m_nType] = *iter;
	}

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_SYNC_LOTTERYRECORD);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void GameLogic::LC_ClientActivityManager::SyncLotteryData(const MG_SC_SyncLottery_Ret& msg)
{
	m_mLotteryDataMap.clear();

	std::vector<Lottery_Base>::const_iterator iter = msg.m_LotteryVec.begin();
	for( ; iter!=msg.m_LotteryVec.end(); ++iter)
	{
		m_mLotteryDataMap[iter->m_Id] = *iter;
	}

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_SYNC_LOTTERY_DATA);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void GameLogic::LC_ClientActivityManager::UpdateLottryData(const MG_SC_Lottery_Draw_Ret& msg)
{
	Lottery_Base& lotteryData = m_mLotteryDataMap[msg.m_Id];
	lotteryData.m_DayCount = msg.m_DayCount;
	lotteryData.m_Free = msg.m_Free;
	lotteryData.m_TotalCount = msg.m_TotalCount;

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_SYNC_LOTTERY_DATA);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void GameLogic::LC_ClientActivityManager::UpdateLottryData(const MG_SC_Lottery_ExtraReward_Ret& msg, bool bStatus)
{
	Lottery_Base& lotteryData = m_mLotteryDataMap[msg.m_Id];
	lotteryData.m_RewardNum = msg.m_Param;
	if(bStatus)
	{
		lotteryData.m_Status.push_back(msg.m_Param);
	}

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_SYNC_CUMULATE_NUM);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

StringType GameLogic::LC_ClientActivityManager::GetLotteryRecordInfo(int iType)
{
	mem::map<int, PS_LotteryRecord>::const_iterator iter = m_mLotteryRecord.find(iType);
	if(iter == m_mLotteryRecord.end())
		return "";

	const PS_LotteryRecord& record = iter->second;

	StringStreamType sStream;
	sStream << record.m_nCount << '#' << record.m_bUsedFree;
	return sStream.str();
}

StringType GameLogic::LC_ClientActivityManager::GetLotteryDataById(int iId)
{
	mem::map<int, Lottery_Base>::const_iterator iter = m_mLotteryDataMap.find(iId);
	if(iter == m_mLotteryDataMap.end())
		return "";

	const Lottery_Base& data = iter->second;

	StringStreamType sStream;
	sStream << data.m_DayCount << '#' << data.m_Free << '#';
	sStream << data.m_RewardNum << '#' << data.m_TotalCount << '#';
	sStream << data.m_Status.size() << '#';

	std::vector<int32_t>::const_iterator itr = data.m_Status.begin();
	for( ; itr!=data.m_Status.end(); ++itr)
	{
		sStream << *itr << '#';
	}
	return sStream.str();
}

StringType LC_ClientActivityManager::GetTreasureHuntInfo()
{
	StringStreamType sStream;
	StringType sItems;
	sItems.reserve(48);

	CF_ACTTreasureHunt::DataEntryMapExternIterator iter = CF_ACTTreasureHunt::GetSingletonPtr()->GetIterator();
	for(; iter.HasMoreElements(); iter.MoveNext())
	{
		const CF_ACTTreasureHunt::DataEntry* pkData = iter.PeekNextValuePtr();
		sStream << pkData->_iID << '#';

		sItems.clear();
		FormatDataIFs2String(pkData->_kPerItems, sItems);

		sStream << pkData->_iSchedule << '#' << pkData->_iScheduleGetReward << '#' << pkData->_iCategory << '#';
		sStream << pkData->_iKey << '#' << pkData->_iPerCharge << '#' << sItems << '#';
		sStream << pkData->_iMaxGamble << '#' << pkData->_iVIPDiscount << '#' << pkData->_iLotteryMaxTimes << '#';

		sItems.clear();
		FormatDataIFs2String(pkData->_sReward, sItems);

		sStream << pkData->_iTimes << '#' << sItems << '#' << pkData->_iCrystalReward << '#';
		sStream << pkData->_kButtonIcon << '#' << pkData->_sCardDesc << '#' << pkData->_sRewardShow << '#';
		sStream << pkData->_iBuddyID << '#' << pkData->_sBigBkg << '#' << pkData->_sCrystal << '#';
		sStream << pkData->_bActivity << '#' << pkData->_sTips<< '#' << pkData->_sRewardInfo << '#' << pkData->_iType << '|';
	}

	return sStream.str();
}

//充值奖励
//=========================================================================================

StringType LC_ClientActivityManager::GetChargeProfitBInfo(int iId, bool bWithRwd)
{
	const CF_ACTChargeProfitB::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_ACTChargeProfitB>(iId);
	if(pkData == NULL)
		return "";

	StringStreamType sStream;
	sStream << pkData->_iSchedule << '#' << pkData->_iType << '#' << pkData->_iCashType << '#';

	mem::vector<int>::const_iterator iter1 = pkData->_sKey.begin();
	for( ; iter1!=pkData->_sKey.end(); ++iter1)
	{
		sStream << *iter1 << '|';
	}

	sStream << '#';

	mem::vector<int>::const_iterator iter2 = pkData->_sTimeLimits.begin();
	for( ; iter2!=pkData->_sTimeLimits.end(); ++iter2)
	{
		sStream << *iter2 << '|';
	}
	
	if(bWithRwd)
	{
		sStream << '#';

		StringType sItems;
		sItems.reserve(48);

		mem::vector<int>::const_iterator iter = pkData->_sReward.begin();
		for( ; iter!=pkData->_sReward.end(); ++iter)
		{
			const CF_ACTChargeProfitBReward::DataEntry* pkRwdData = SafeGetCSVFileDataEntryPtr<CF_ACTChargeProfitBReward>(*iter);
			T_ASSERT_CRITICAL_S(pkRwdData != NULL);

			sItems.clear();
			FormatDataIFs2String(pkRwdData->_kReward, sItems);
			sStream << sItems << '|';
		}
	}
	
	return sStream.str();
}

void LC_ClientActivityManager::SyncChargeProfitData(const MG_SC_SynChargeProfit_Ret& msg)
{
	m_mChargeProfitDataMap.clear();

	std::vector<ChargeProfit_Base>::const_iterator iter = msg.m_ChargeProfitVec.begin();
	for( ; iter!=msg.m_ChargeProfitVec.end(); ++iter)
	{
		m_mChargeProfitDataMap[iter->m_Id] = *iter;
	}

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_SYNC_CHARGEPROFIT_DATA);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void LC_ClientActivityManager::UpdateChargeProfitData(const MG_SC_ChargeProfit_Reward_Ret& msg)
{
	ChargeProfit_Base& chargeProfitData = m_mChargeProfitDataMap[msg.m_Id];
	chargeProfitData.m_Status[msg.m_Param.key] = msg.m_Param.value;

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_SYNC_CHARGEPROFIT_DATA);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

StringType LC_ClientActivityManager::GetChargeProfitData(int iId)
{
	mem::map<int, ChargeProfit_Base>::const_iterator iter = m_mChargeProfitDataMap.find(iId);
	if(iter == m_mChargeProfitDataMap.end())
		return "";

	const ChargeProfit_Base& data = iter->second;

	StringStreamType sStream;
	sStream << data.m_TotalNum << '#' << data.m_Timestamp << '#';
	sStream << data.m_Status.size() << '#';

	std::map<int,int>::const_iterator itr = data.m_Status.begin();
	for( ; itr!=data.m_Status.end(); ++itr)
	{
		sStream << itr->first << '|' << itr->second << '#';
	}

	return sStream.str();
}

int64_t LC_ClientActivityManager::GetTimeIdEndTimes(int32_t iTimeID, uint32_t iRegTime)
{
	CF_ActivityTime::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_ActivityTime>(iTimeID);
	if(pkEntry == NULL)
		return 0;

	switch(pkEntry->_iType)
	{
	case 1:
		{
			int64_t startTime = pkEntry->_iCoolTime.GetTimeStramp(iRegTime);
			return startTime + pkEntry->_iTimeLen*ONE_MINUTE_SECONDS;
		}
	default:
		T_ASSERT_CRITICAL_S(false);
	}
	
	return 0;
}

bool LC_ClientActivityManager::IsTimeIdValid(LC_ClientGamePlayer* pkPlayer, int32_t iTimeID)
{
	CF_ActivityTime::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_ActivityTime>(iTimeID);
	if(pkEntry == NULL)
		return false;

	int64_t curTime = pkPlayer->GetServerTimeVal();
	int64_t startTime = pkEntry->_iCoolTime.GetTimeStramp(pkPlayer->GetRegTime());

	return curTime >= startTime && curTime <= (startTime + pkEntry->_iTimeLen*ONE_MINUTE_SECONDS);
}

int LC_ClientActivityManager::GetActiveTimeId(int32_t ActivityType, int32_t iCatagory, LC_ClientGamePlayer* pkPlayer)
{
	T_ASSERT_CRITICAL_S(ActivityType == ACT_SCHEDULE_TP_SHOP_B);

	ACT_ScheduleDataCatagory *pkCatagory = GetScheduleDataCatagory(ActivityType, iCatagory);
	if (NULL == pkCatagory)
	{
		return 0;
	}
	
	const GameLogic::LC_IDSet& kScheduleIDs = pkCatagory->GetScheduleIDs();
	if (kScheduleIDs.empty())
	{
		return 0;
	}

	for (GameLogic::LC_IDSet::const_iterator it = kScheduleIDs.begin(); it != kScheduleIDs.end(); ++it)
	{
		ACT_ScheduleDataEntry* pkDataEntry = pkCatagory->GetScheduleData(*it);
		if(pkDataEntry != NULL)
		{
			const ACT_RewardDataEntryMap& dataMap = pkDataEntry->entrys();
			ACT_RewardDataEntryMap::const_iterator iter = dataMap.begin();
			for(; iter!=dataMap.end(); ++iter)
			{
				const ACT_RewardDataEntry& rwdEntry = iter->second;
				if(IsTimeIdValid(pkPlayer, *it))
				{
					return *it;
				}
			}
		}
	}
	return 0;
}
