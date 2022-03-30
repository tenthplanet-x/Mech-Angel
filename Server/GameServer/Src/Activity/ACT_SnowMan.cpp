#include "ACT_SnowMan.h"
#include "ACT_ActivityDataManager.h"
#include "ACT_ActivitySchedule.h"
#include "LC_ServerPlayer.h"
#include "UT_ServerHelper.h"
#include "SC_ScriptAction.h"
#include "GameServerApp.h"
#include "UT_RedisRequest.h"
#include "LC_RumorManager.h"

using namespace CSVFile;
using namespace GameLogic;
using namespace Activity;

ResultType ACT_SnowManDataEntry::DoReward(LC_ServerPlayer* pkPlayer, ACT_ScheduleSaveEntry* pkEntry, GameLogic::LC_RewardItemsExtend& rkExtend)
{
	if (NULL==pkPlayer || NULL==pkEntry)
	{
		return RE_FAIL;
	}

	ResultType uiRslt = RE_SUCCESS;
	do
	{
		Utility::UT_SIMDataList kItems;
		rkExtend.SetChoice(m_kReward.CheckChoiceID(pkPlayer->GetWeaponAttribute(), rkExtend.Choice()));
		m_kReward.Resolve(pkPlayer, kItems, &rkExtend);
		if (!kItems.empty())
		{
			uiRslt = UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, kItems, pkPlayer, SYS_FUNC_TP_ACTIVITY_SNOW_MAN
				, m_iID, m_sMailTitle, m_sMailDesc, m_bMailEnable);
			if (RE_SUCCESS != uiRslt)
			{
				break;
			}
		}
	}while(0);

	return uiRslt;
}


struct SnowManScheduleOper : public Activity::ACT_ScheduleSaveDataOper<ACT_SnowManManager::DataEntryType>
{
	SnowManScheduleOper(ACT_ScheduleManager* schedule, LC_ServerPlayer* player, ACT_ScheduleAsset* asset, ACT_ScheduleSaveActivity* act, ACT_ScheduleSaveCatagory* cat)
		:Activity::ACT_ScheduleSaveDataOper<ACT_SnowManManager::DataEntryType>(schedule, player, asset, act, cat)
		, iAddCount(0)
		, iAddAchieve(0)
		, pkActSnowManMgr(NULL)
	{}
	
	virtual bool operator ()(ACT_SnowManManager::ScheduleDataType* pkSchedule)
	{
		if (pkScheduleMgr->IsScheduleValid(pkPlayer->GetServerID(), pkSchedule->m_iID))
		{
			ACT_ScheduleSaveEntry* pkSaveEntry = pkSaveCatagory->_addScheduleSaveEntry(pkSchedule->m_iID);
			if (NULL != pkSaveEntry)
			{
				int32_t iOldAchieve = pkSaveEntry->GetAchieve();
				pkSaveEntry->AddCount(iAddCount);	
				pkSaveEntry->AddAchieve(iAddAchieve);
				pkSaveCatagory->UpdateDirtyFlag();
				pkSaveActivity->UpdateDirtyFlag();
				pkAsset->UpdateDirtyFlag();

				do
				{
					if (NULL != pkActSnowManMgr)
					{
						ACT_SnowManManager::CatagoryDataType* pkCatagory = pkActSnowManMgr->GetCatagoryData(pkSaveCatagory->ID());						 
						if (NULL == pkCatagory)
						{
							break;
						}
						ACT_SnowManManager::ScheduleDataType* pkScheduleDataType = pkCatagory->GetScheduleData(pkSchedule->m_iID);
						if (NULL == pkScheduleDataType)
						{
							break;
						}	
						const GameLogic::LC_IDVector& kIDVec = pkScheduleDataType->Keys();
						if (kIDVec.empty())
						{
							break;
						}
						ACT_SnowManManager::DataEntryType* pkDataEntryType = pkScheduleDataType->GetDataEntry(kIDVec[0]);
						if (NULL == pkDataEntryType)
						{
							break;
						}
						pkActSnowManMgr->AddGroupScore(pkSchedule->m_iID, pkDataEntryType->m_iGroupSaveIndex, iAddCount, pkPlayer->GetOwnerCharName(), pkPlayer->GetCitizenship(), pkPlayer->GetInstance());
					}
				} while (0);
				return true;
			}
		}
		return false;
	}
	int32_t iAddCount;
	int32_t iAddAchieve;
	ACT_SnowManManager * pkActSnowManMgr;
};

void ACT_SnowManManager::SetUp(void)
{
	ACT_ScheduleDataActivityBase::SetUp();
	m_kUpdateTimer.SetTimer(GET_INTERVAL(CMN_INTER_INERT));
}
void ACT_SnowManManager::Update(float fCurrentTime)
{
	if (!m_kUpdateTimer.IsExpired())
	{
		return;
	}
	mem::map<SnowManGroupScoreKey, int>::iterator iter = m_kGroupScoreMap.begin();
	for (; iter != m_kGroupScoreMap.end(); ++iter)
	{
		_requestGroupScore(iter->first.iSchedule, iter->first.iType);
	}
}

void ACT_SnowManManager::_requestGroupScore(int iSchedule, int iType)
{
	ResultType rslt = RE_SUCCESS;
	GameServerApp* pkApp = GetGameServerApp();
	std::string sKey = pkApp->GetRedisPrefix(INF_SPH_TP_GROUP) + UT_REDIS_KEY_ACTIVITY_SNOWMAN_SCORE;
	sKey += "_";
	sKey += TPSTR2STDSTR(Utility::Int2String(iSchedule));
	sKey += "_";
	sKey += TPSTR2STDSTR(Utility::Int2String(iType));
	int32_t iCurGroupId = pkApp->GetServerManager()->GetCurGroupId();
	if (iCurGroupId <= 0)
	{
		return;
	}
	boost::function< void(int32_t, const std::string&) > cb = boost::bind(&ACT_SnowManManager::_onGetActSnowManGroupScore, this, _1, _2, iSchedule, iType);
	//std::string strGroupID = TPSTR2STDSTR(Utility::Int2String(iCurGroupId));
	ASYNC_REDIS_REQUEST(rslt, pkApp->GetRedisChannel(), &redispp::Connection::hget, cb, sKey, TPSTR2STDSTR(Utility::Int2String(iCurGroupId)));
}

int ACT_SnowManManager::_getGroupScore(int iSchedule, int iType)
{
	if (iSchedule <= 0 || iType <= 0)
	{
		return -1;
	}
	SnowManGroupScoreKey kScoreKey;
	kScoreKey.iSchedule = iSchedule;
	kScoreKey.iType = iType;
	mem::map<SnowManGroupScoreKey, int>::iterator iter = m_kGroupScoreMap.find(kScoreKey);
	if (iter != m_kGroupScoreMap.end())
	{
		return iter->second;
	}
	m_kGroupScoreMap[kScoreKey] = -1;
	return -1;
}

void ACT_SnowManManager::_onGetActSnowManGroupScore(int32_t err, const std::string& strScoreInfo, int iSchedule, int iType)
{
	if (RE_SUCCESS != err)
	{
		GfxWriteFmtLog(LOG_WARNING_FILELINE, LOG_SWITCH_WARNING, "_onGetActSnowManGroupScore failed!");
		return;
	}
	SnowManGroupScoreKey kScoreKey;
	kScoreKey.iSchedule = iSchedule;
	kScoreKey.iType = iType;
	m_kGroupScoreMap[kScoreKey] = Utility::String2Int(STDSTR2TPSTR(strScoreInfo));
}

void ACT_SnowManManager::AddScore(LC_ServerPlayer* pkPlayer, int32_t val, int32_t iAchieve, const mem::vector<int32_t>& kCatagorys)
{
	ACT_ScheduleManager* pkScheduleMgr = ACT_ScheduleManager::GetSingletonPtr();
	if (NULL == pkScheduleMgr)
	{
		return;
	}
	Activity::ACT_ScheduleAsset& rkAsset = pkPlayer->GetACTScheduleAsset();
	Activity::ACT_ScheduleSaveActivity*	pkSaveActivity = rkAsset._addScheduleSaveActivity(GetType());
	if (NULL==pkSaveActivity)
	{
		return;
	}
	SnowManScheduleOper kOper(pkScheduleMgr, pkPlayer, &rkAsset, pkSaveActivity, NULL);
	kOper.iAddCount = val;
	kOper.iAddAchieve = iAchieve;
	kOper.pkPlayer = pkPlayer;
	kOper.pkActSnowManMgr = this;
	if (kCatagorys.empty())
	{
		CatagoryDataMap::iterator it = CatagoryDatas().begin();
		for (; it!=CatagoryDatas().end(); ++it)
		{
			kOper.pkSaveCatagory = pkSaveActivity->_addScheduleSaveCatagory(it->first);
			if (kOper.pkSaveCatagory)
			{
				it->second.ForAll(&kOper);
			}
		}
	}
	else
	{
		mem::vector<int32_t>::const_iterator cit = kCatagorys.begin();
		for (; cit!=kCatagorys.end(); ++cit)
		{
			CatagoryDataType* pkCatagory = GetCatagoryData(*cit);
			if (NULL == pkCatagory)
			{
				continue;
			}

			kOper.pkSaveCatagory = pkSaveActivity->_addScheduleSaveCatagory(*cit);
			if (kOper.pkSaveCatagory)
			{
				pkCatagory->ForAll(&kOper);
			}
		}
	}
}

void ACT_SnowManManager::AddGroupScore(int iSchedule, int iType, int32_t val, StringType charName, int citizen, unique_id_impl charID)
{
	ResultType rslt = RE_SUCCESS;
	static GameServerApp* pkApp = GetGameServerApp();
	std::string sKey = pkApp->GetRedisPrefix(INF_SPH_TP_GROUP) + UT_REDIS_KEY_ACTIVITY_SNOWMAN_SCORE;
	sKey += "_";
	sKey += TPSTR2STDSTR(Utility::Int2String(iSchedule));
	sKey += "_";
	sKey += TPSTR2STDSTR(Utility::Int2String(iType));
	int32_t iCurGroupId = pkApp->GetServerManager()->GetCurGroupId();
	if (iCurGroupId <= 0)
	{
		return;
	}
	boost::function< void(int32_t, const int64_t&) > cb = boost::bind(&ACT_SnowManManager::_onAddActSnowManGroupScore, this, _1, _2, iSchedule, iType, charName, citizen, charID);
	//std::string strGroupID = TPSTR2STDSTR(Utility::Int2String(iCurGroupId));
	ASYNC_REDIS_REQUEST(rslt, pkApp->GetRedisChannel(), &redispp::Connection::hincrBy, cb, sKey, TPSTR2STDSTR(Utility::Int2String(iCurGroupId)), val);
}

void ACT_SnowManManager::_onAddActSnowManGroupScore(int32_t err, const int64_t& iScore, int iSchedule, int iType, StringType charName, int citizen, unique_id_impl charID)
{
	if (RE_SUCCESS != err)
	{
		GfxWriteFmtLog(LOG_WARNING_FILELINE, LOG_SWITCH_WARNING, "_onAddActSnowManGroupScore failed!");
		return;
	}
	int iFinal = (int32_t)Utility::UT_MathBase::MinInt64(0xFFFFFFFF, iScore);
	SnowManGroupScoreKey key;
	key.iSchedule = iSchedule;
	key.iType = iType;
	int iOldVal = m_kGroupScoreMap[key];
	m_kGroupScoreMap[key] = iFinal;
}

ResultType ACT_SnowManManager::DrawReward(LC_ServerPlayer* pkPlayer, int32_t iCatagory, int32_t iSchedule, int32_t iKey, GameLogic::LC_RewardItemsExtend& rkExtend, const GameLogic::LC_IDVector* pkExtraParams, int& count)
{
	if (NULL==pkPlayer)
	{
		return RE_FAIL;
	}

	DataEntryType* pkRewardDataEntry = GetDataEntry(iCatagory, iSchedule, iKey);
	if (NULL == pkRewardDataEntry)
	{
		return RE_ACTIVITY_BAD_PARAMS;
	}
	static ACT_ScheduleManager* pkScheduleMgr = ACT_ScheduleManager::GetSingletonPtr();
	if (!pkScheduleMgr->IsScheduleValid(pkPlayer->GetServerID(), iSchedule))
	{
		return RE_ACTIVITY_NOT_VALID;
	}

	ACT_ScheduleSaveActivity* pkSaveActivity = pkPlayer->GetACTScheduleAsset()._addScheduleSaveActivity(GetType());
	if (NULL == pkSaveActivity)
	{
		return RE_FAIL;
	}

	ACT_ScheduleSaveCatagory* pkSaveCatagory = pkSaveActivity->_addScheduleSaveCatagory(iCatagory);
	if (NULL == pkSaveCatagory)
	{
		return RE_FAIL;
	}

	ACT_ScheduleSaveEntry* pkSaveSchedule = pkSaveCatagory->_addScheduleSaveEntry(iSchedule);
	if (NULL == pkSaveSchedule)
	{
		return RE_FAIL;
	}

	if (pkPlayer->GetLevel() < pkRewardDataEntry->m_iLevelLimit)
	{
		return RE_ACTIVITY_CONDITION_FAIL;
	}

	if (1 == pkRewardDataEntry->m_iCondType)
	{
		if (iKey > (int32_t)pkSaveSchedule->GetCount())
		{
			return RE_ACTIVITY_CONDITION_FAIL;
		}
	}
	else
	{
		int iGroupScore = _getGroupScore(iSchedule, iCatagory);
		if (iGroupScore < 0)
		{
			return RE_FAIL;
		}
		if (iKey > iGroupScore)
		{
			return RE_ACTIVITY_CONDITION_FAIL;
		}
	}
	if (pkSaveSchedule->IsRewardReg(pkRewardDataEntry->m_iSaveIndex))
	{
		return RE_ACTIVITY_REWARDED;
	}

	ResultType rslt = pkRewardDataEntry->DoReward(pkPlayer, pkSaveSchedule, rkExtend);
	if (RE_SUCCESS == rslt)
	{
		pkSaveSchedule->RegReward(pkRewardDataEntry->m_iSaveIndex);
		pkSaveCatagory->UpdateDirtyFlag();
		pkSaveActivity->UpdateDirtyFlag();
		pkPlayer->GetACTScheduleAsset().UpdateDirtyFlag();
	}
	return rslt;
}

ResultType ACT_SnowManManager::DoUseItem(LC_ServerPlayer* pkPlayer, mem::vector<int32_t>&kParams)
{
	for (size_t i = 0; i < kParams.size()/2; ++i)
	{
		int iItemId = kParams[i * 2];
		int iCount = kParams[i * 2 + 1];
		if (iItemId <= 0 || iCount <= 0)
		{
			return RE_FAIL;
		}
		if (RE_SUCCESS != pkPlayer->UseItemInPackByItemType(iItemId, iCount))
		{
			return RE_FAIL;
		}
	}
	return RE_SUCCESS;
}

int32_t ACT_SnowManManager::QueryGroupScore(LC_ServerPlayer* pkPlayer, int iSchedule, int iCatagory)
{
	CatagoryDataType* pkCatagory = GetCatagoryData(iCatagory);
	if (NULL == pkCatagory)
	{
		return -1;
	}

	ScheduleDataType* pkScheduleDataType = pkCatagory->GetScheduleData(iSchedule);
	if (NULL == pkScheduleDataType)
	{
		return -1;
	}
	static ACT_ScheduleManager* pkScheduleMgr = ACT_ScheduleManager::GetSingletonPtr();
	if (!pkScheduleMgr->IsScheduleValid(pkPlayer->GetServerID(), iSchedule))
	{
		return -1;
	}
	const GameLogic::LC_IDVector& kIDVec = pkScheduleDataType->Keys();
	if (kIDVec.empty())
	{
		return -1;
	}
	DataEntryType* pkDataEntryType = pkScheduleDataType->GetDataEntry(kIDVec[0]);
	if (NULL == pkDataEntryType)
	{
		return -1;
	}
	return _getGroupScore(iSchedule, pkDataEntryType->m_iGroupSaveIndex);
}
