#include "ACT_SecretTreasure.h"
#include "ACT_ActivitySchedule.h"

#include "LC_ServerPlayer.h"
#include "LC_RumorManager.h"
#include "LC_CommonStatAsset.h"

#include "UT_ServerHelper.h"

#include "GameServerApp.h"

using namespace Utility;
using namespace Activity;
using namespace CSVFile;

bool ACT_SecretTreasurePoolDataEntry::Init(CSVFile::CF_SecretTreasurePool::DataEntry* pkEntry)
{
	if (NULL == pkEntry)
	{
		return false;
	}
	m_iID = pkEntry->_iID;
	m_iSchedule = pkEntry->_iSchedule;
	m_iCatagory = pkEntry->_iCatagory;
	m_iMinLevel = pkEntry->_iMinLevel;
	m_iMaxLevel = pkEntry->_iMaxLevel;
	m_iRumorType = pkEntry->_lRumorType;
	m_iSelfTimes = pkEntry->_iSelfTimes;
	m_iGlobalTimes = pkEntry->_iGlobalTimes;
	m_kRewardItems.SetUp(pkEntry->_kRewardItems);
	m_kPuts = pkEntry->_kPuts;
	return true;
}

void ACT_SecretTreasurePoolDataEntry::Resolve(LC_ServerPlayer* pkPlayer, Utility::UT_SIMDataList& kRewards, GameLogic::LC_RewardItemsExtend& rkExtend)
{
	if (m_kPuts.empty())
	{
		m_kRewardItems.Resolve(pkPlayer, kRewards, &rkExtend);
	}
	else
	{
		mem::set<ACT_SecretTreasurePutDataEntry*> kPutSet;
		uint32_t uiTotalPro = m_kRewardItems.TotalRandomPro(pkPlayer->GetWeaponAttribute(), &rkExtend);
		static ACT_SecretTreasureManager* pkMgr = ACT_SecretTreasureManager::GetSingletonPtr();
		Utility::UT_SIMDataMap::iterator it = m_kPuts.begin();
		for (; it!=m_kPuts.end(); ++it)
		{
			ACT_SecretTreasurePutDataEntry* pkPut = pkMgr->GetPut(it->first);
			if (NULL == pkPut)
			{
				continue;
			}

			if (!pkPut->GetPut())
			{
				continue;
			}
			kPutSet.insert(pkPut);
			uiTotalPro += pkPut->TotalRandomPro(pkPlayer->GetWeaponAttribute(), &rkExtend);
		}

		if (kPutSet.empty() || uiTotalPro<=0)
		{
			m_kRewardItems.Resolve(pkPlayer, kRewards, &rkExtend);
		}
		else
		{
			GameLogic::LC_RewardItemsExtend kTmpExtend;
			kTmpExtend.SetBind(rkExtend.Bind());
			uint32_t seed = UT_MathBase::RandInRangeInt(1, uiTotalPro-1);
			mem::set<ACT_SecretTreasurePutDataEntry*>::iterator itPut = kPutSet.begin();
			for (; itPut!=kPutSet.end(); ++itPut)
			{
				ACT_SecretTreasurePutDataEntry* pkPut = *itPut;
				if (seed < pkPut->TotalRandomPro(pkPlayer->GetWeaponAttribute(), &rkExtend))
				{
					kTmpExtend.SetSeed(seed);
					pkPut->m_kRewards.Resolve(pkPlayer, kRewards, &kTmpExtend);
					pkPut->ConsumePut();
					return;
				}
				pkPut->ReleasePut();
				seed -= pkPut->TotalRandomPro(pkPlayer->GetWeaponAttribute(), &rkExtend);
			}
			kTmpExtend.SetSeed(seed);
			m_kRewardItems.Resolve(pkPlayer, kRewards, &kTmpExtend);
		}
	}
}

bool ACT_SecretTreasureScheduleDataEntry::Init(CSVFile::CF_SecretTreasureSchedule::DataEntry* pkEntry)
{
	if (NULL == pkEntry)
	{
		return false;
	}
	m_iID = pkEntry->_iID;
	m_iSchedule = pkEntry->_iSchedule;
	m_iCatagory = pkEntry->_iCatagory;
	m_iCluster = pkEntry->_iCluster;
	m_sDesc = pkEntry->_sDesc;
	m_kIcons = pkEntry->_kIcons;
	return true;
}

ACT_SecretTreasurePoolDataEntry* ACT_SecretTreasureScheduleDataEntry::ResolvePool(LC_ServerPlayer* pkPlayer, int32_t iSelfTimes, int32_t iGlobalTimes)
{
	IF_A_NA_PLAYER(pkPlayer)
	{
		return NULL;
	}
	ACT_SecretTreasurePoolDataEntries::iterator it = m_kPools.begin();
	for (; it!=m_kPools.end(); ++it)
	{
		ACT_SecretTreasurePoolDataEntry& rkEntry = *it;
		if (rkEntry.m_iMinLevel>pkPlayer->GetLevel())
		{
			continue;
		}

		if (rkEntry.m_iMaxLevel>0 && rkEntry.m_iMaxLevel<=pkPlayer->GetLevel())
		{
			continue;
		}

		if (rkEntry.m_iGlobalTimes>0 && iGlobalTimes==rkEntry.m_iGlobalTimes)
		{
			return &rkEntry;
		}
		else if (rkEntry.m_iSelfTimes>0 && iSelfTimes%rkEntry.m_iSelfTimes==0)
		{
			return &rkEntry;
		}
	}
	return NULL;
}

bool ACT_SecretTreasurePutDataEntry::Init(CSVFile::CF_SecretTreasurePut::DataEntry* pkEntry)
{
	if (NULL == pkEntry)
	{
		return false;
	}
	m_iID = pkEntry->_iID;
	m_iSchedule = pkEntry->_iSchedule;
	m_sCron = pkEntry->_sCron;
	m_iType = pkEntry->_iType;
	m_iMaxTimes = pkEntry->_iMaxTimes;
	m_kInChargeServers = pkEntry->_InChargeServers;

	m_kRewards.SetUp(pkEntry->_kItems);
	return true;
}

void ACT_SecretTreasurePutDataEntry::Unit(void)
{
	static UT_TimerManager* pkTimerMgr = UT_TimerManager::GetSingletonPtr();
	if (IsTimerIDValid(m_iTimer))
	{
		pkTimerMgr->CancelTimer(m_iTimer);
		m_iTimer = TIMER_ID_NA;
	}
	m_save.Clear();
}

void ACT_SecretTreasurePutDataEntry::SetUp(void)
{
	static GameServerApp* pkApp = GetGameServerApp();
	static UT_TimerManager* pkTimerMgr = UT_TimerManager::GetSingletonPtr();

	Unit();
	Utility::Redis_Handler* pkRedisHandler = pkApp->GetRedisHandler(m_iType);
	std::stringstream _ss;
	switch(m_iType)
	{
	case ACT_PUT_TP_LOCAL:
		{
			_ss << pkApp->GetRedisServerPredix();
		}
		break;
	case ACT_PUT_TP_GROUP:
		{
			_ss << pkApp->GetRedisGlobalPrefix();
		}
		break;
	case ACT_PUT_TP_UNION:
		{
			_ss << pkApp->GetRedisGlobalPrefix();
		}
		break;
	default:
		return;
		break;
	}
	std::string preKey = _ss.str();
	m_sServerID = boost::lexical_cast<std::string>(pkApp->GetServerManager()->GetCurServerId());
	m_sID = boost::lexical_cast<std::string>(m_iID);
	m_sPutEntryKey = preKey + UT_REDIS_KEY_ACTIVITY_TREASURE_PUT;
	if (pkRedisHandler && !m_sPutEntryKey.empty())
	{
		int ret = 0;
		UT_REDIS_CMD_BEGIN(ret, pkRedisHandler);
		redispp::StringReply kEntryRly = pkRedisConn->hget(m_sPutEntryKey, m_sID);
		redis_define::secret_treasure_put_entry& kentry = m_save;
		kentry.Clear();
		if (kEntryRly.result() && kentry.ParseFromString(kEntryRly))
		{
			GetDataFromPB(kentry);
		}
		UT_REDIS_CMD_END(ret, pkRedisHandler);
	}
	m_sPutPoolKey = m_sPutEntryKey + ":" + m_sID;
	m_sPutOccupyKey = preKey + ":" + UT_REDIS_KEY_ACTIVITY_TREASURE_OCCUPY + ":" + m_sID;

	m_iTimer = pkTimerMgr->SetCron(m_sCron, 60, ACT_SecretTreasureManager::_onCronPut, m_iID);
}

void ACT_SecretTreasurePutDataEntry::CheckReset(uint32_t timestamp)
{
	if (!LC_Helper::CheckIsSameDay(timestamp, m_uiTimestamp))
	{
		m_iTimes = 0;
		m_uiTimestamp = timestamp;
		UpdateDirtyFlag();
	}
}

void ACT_SecretTreasurePutDataEntry::CheckPut(void)
{
	static GameServerApp* pkApp = GetGameServerApp();

	static ACT_ScheduleManager* pkScheduleMgr = ACT_ScheduleManager::GetSingletonPtr();
	if(!pkScheduleMgr->IsScheduleValid(GetGameServerID(), m_iSchedule))
	{
		return;
	}

	if (m_iTimes >= m_iMaxTimes)
	{
		return;
	}

	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "check put", m_iTimes, m_iMaxTimes);
	switch (m_iType)
	{
	case ACT_PUT_TP_LOCAL:
		break;
	case ACT_PUT_TP_GROUP:
	case ACT_PUT_TP_UNION:
		{
			bool  valid = false;
			Utility::UT_SIMDataMap::iterator it = m_kInChargeServers.begin();
			for (; it!=m_kInChargeServers.end(); ++it)
			{
				if(UT_ServerHelper::CheckLocal(it->first))
				{
					valid = true;
					break;
				}
			}
			if (!valid)
			{
				return;
			}
		}
		break;
	default:
		break;
	}

	++m_iTimes;
	UpdateDirtyFlag();

	Utility::Redis_Channel* pkRedisChannel = pkApp->GetRedisChannel(m_iType);
	ASYNC_REDIS_REQUEST_NCB(*pkRedisChannel, &redispp::Connection::incr, m_sPutPoolKey);
}
bool ACT_SecretTreasurePutDataEntry::GetPut(void)
{
	static GameServerApp* pkApp = GetGameServerApp();
	static ACT_ScheduleManager* pkScheduleMgr = ACT_ScheduleManager::GetSingletonPtr();
	if(!pkScheduleMgr->IsScheduleValid(GetGameServerID(), m_iSchedule))
	{
		return false;
	}

	Utility::Redis_Handler* pkRedisHandler = pkApp->GetRedisHandler(m_iType);
	int ret = 0;
	bool got = false;

	UT_REDIS_CMD_BEGIN(ret, pkRedisHandler);
	do 
	{
		redispp::BoolReply kOccupyRply = pkRedisConn->setNX(m_sPutOccupyKey, m_sServerID);	
		if (!kOccupyRply.result())
		{
			std::string sOccupyValue = pkRedisConn->get(m_sPutOccupyKey);
			if(m_sServerID != sOccupyValue)
			{
				break;
			}
		}
		
		//occupy key  timeout 1 seconds
		pkRedisConn->expire(m_sPutOccupyKey, 1);
		
		//check count available?
		int32_t iCur = pkRedisConn->decrBy(m_sPutPoolKey, 0);
		if (iCur <= 0)
		{
			break;
		}
		GfxWriteFmtLog(LOG_INFO_FILELINE,LOG_SWITCH_INFO, "get put", m_sPutPoolKey, iCur);
		got = true;
	} while (0);
	UT_REDIS_CMD_END(ret, pkRedisHandler);

	if (ret)
	{
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "get treasure put error", ret, pkRedisHandler->GetLastError());
	}
	return got;
}
void post_release_put(int32_t err, bool& ret)
{
	if (err)
	{
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "release treasure put error", err, ret);
	}
	else
	{
		GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "release treasure put left count", err, ret);
	}
}
void post_consume_put(int32_t err, int64_t& left)
{
	if (err)
	{
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "consume treasure put error", err, left);
	}
	else
	{
		GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "consume treasure put left count", err, left);
	}
}
void ACT_SecretTreasurePutDataEntry::ReleasePut(void)
{
	static GameServerApp* pkApp = GetGameServerApp();
	Utility::Redis_Channel* pkRedisChannel = pkApp->GetRedisChannel(m_iType);
	int ret = 0;
	int left = 0;
	ASYNC_REDIS_REQUEST(ret, *pkRedisChannel, &redispp::Connection::del, post_release_put, m_sPutOccupyKey);
}
void ACT_SecretTreasurePutDataEntry::ConsumePut(void)
{
	static GameServerApp* pkApp = GetGameServerApp();
	Utility::Redis_Channel* pkRedisChannel = pkApp->GetRedisChannel(m_iType);
	int ret = 0;
	int left = 0;

	ASYNC_REDIS_REQUEST(ret, *pkRedisChannel, &redispp::Connection::decrBy, post_consume_put, m_sPutPoolKey, 1);
}
void ACT_SecretTreasurePutDataEntry::PushSave(void)
{
	static GameServerApp* pkApp = GetGameServerApp();
	Utility::Redis_Channel* pkRedisChannel = pkApp->GetRedisChannel(m_iType);
	SetDataToPB(m_save);
	ASYNC_REDIS_REQUEST_NCB(*pkRedisChannel, &redispp::Connection::hset, m_sPutEntryKey, m_sID, m_save.SerializeAsString());
}
void ACT_SecretTreasurePutDataEntry::Update(float fTime)
{
	if (GetDirtyFlag())
	{
		PushSave();
		SetDirtyFlag(false);
	}
}
void ACT_SecretTreasurePutDataEntry::GetDataFromPB(const redis_define::secret_treasure_put_entry& info)
{
	m_uiTimestamp = info.timestamp();
	m_iTimes = info.times();
}
void ACT_SecretTreasurePutDataEntry::SetDataToPB(redis_define::secret_treasure_put_entry& info)
{
	info.set_id(m_iID);
	info.set_timestamp(m_uiTimestamp);
	info.set_times(m_iTimes);
}

void ACT_SecretTreasureCatagoryDataEntry::SetUp(void)
{
	static GameServerApp* pkApp = GetGameServerApp();

	Utility::Redis_Handler* pkRedisHandler = pkApp->GetRedisHandler();
	m_sCatagory = boost::lexical_cast<std::string>(m_iCatagory);
	m_sSaveKey = pkApp->GetRedisServerPredix() + UT_REDIS_KEY_ACTIVITY_TREASURE_CATAGORY;
	
	if (pkRedisHandler && !m_sSaveKey.empty())
	{
		int ret = 0;
		UT_REDIS_CMD_BEGIN(ret, pkRedisHandler);
		redispp::StringReply kEntryRly = pkRedisConn->hget(m_sSaveKey, m_sCatagory);
		redis_define::secret_treasure_cat_entry& kentry = m_save;
		kentry.Clear();
		if (kEntryRly.result() && kentry.ParseFromString(kEntryRly))
		{
			GetDataFromPB(kentry);
		}
		UT_REDIS_CMD_END(ret, pkRedisHandler);
	}
}
void ACT_SecretTreasureCatagoryDataEntry::CheckReset(uint32_t timestamp)
{
	if (!LC_Helper::CheckIsSameDay(timestamp, m_uiTimestamp))
	{
		m_iTimes = 0;
		m_uiTimestamp = timestamp;
		UpdateDirtyFlag();
	}
}
void ACT_SecretTreasureCatagoryDataEntry::PushSave()
{
	static GameServerApp* pkApp = GetGameServerApp();
	Utility::Redis_Channel& rkRedisChannel = pkApp->GetRedisChannel();
	SetDataToPB(m_save);
	ASYNC_REDIS_REQUEST_NCB(rkRedisChannel, &redispp::Connection::hset, m_sSaveKey, m_sCatagory, m_save.SerializeAsString())
}
void ACT_SecretTreasureCatagoryDataEntry::Update(float fTime)
{
	if (GetDirtyFlag())
	{
		PushSave();
		SetDirtyFlag(false);
	}
}
void ACT_SecretTreasureCatagoryDataEntry::GetDataFromPB(const redis_define::secret_treasure_cat_entry& info)
{
	m_iCatagory = info.cat();
	m_uiTimestamp = info.timestamp();
	m_iTimes = info.times();
}
void ACT_SecretTreasureCatagoryDataEntry::SetDataToPB(redis_define::secret_treasure_cat_entry& info)
{
	info.set_cat(m_iCatagory);
	info.set_timestamp(m_uiTimestamp);
	info.set_times(m_iTimes);
}

bool ACT_SecretTreasureChestDataEntry::Init(CSVFile::CF_SecretTreasureChests::DataEntry* pkEntry)
{
	if (NULL == pkEntry)
	{
		return false;
	}

	m_iID = pkEntry->_iID;
	m_iCatagory = pkEntry->_iCatagory;
	m_iTimesLimit = pkEntry->_iTimesLimit;
	m_iGold = pkEntry->_iGold;
	m_kItems = pkEntry->_kItems;
	{
		Utility::UT_SIMDataList::iterator it = pkEntry->_kShopIDs.begin();
		for (; it!=pkEntry->_kShopIDs.end(); ++it)
		{
			m_kShopIDMap[it->ID()] += it->ShopCount();
		}
	}
	m_iRandomTimes = pkEntry->_lRandomTimes;
	m_iBind = pkEntry->_lBind;
	m_iScore = pkEntry->_lScore;
	m_iContribute = pkEntry->_iContribute;
	m_iPackType = pkEntry->_iPackType;
	m_sMailTitle = pkEntry->_sMailTitle;
	m_sMailDesc = pkEntry->_sMailDesc;
	return true;
}

bool ACT_SecretTreasureShopDataEntry::Init(CSVFile::CF_SecretTreasureShop::DataEntry* pkEntry)
{
	if (NULL == pkEntry)
	{
		return false;
	}

	m_iID = pkEntry->_iID;
	m_iCatagory = pkEntry->_iCatagory;
	m_iSchedule = pkEntry->_iSchedule;
	m_iScore = pkEntry->_iScore;
	m_iTimes = pkEntry->_iTimes;
	m_kItems = pkEntry->_kItems;
	m_kLimits = pkEntry->_kLimits;
	return true;
}

void ACT_SecretTreasureCluserEntry::GetDataFromPB(const char_data_define::secret_treasure_cluster& info)
{
	m_iID = info.id();
	m_iScore = info.score();
	m_iTimes = info.times();
}
void ACT_SecretTreasureCluserEntry::SetDataToPB(char_data_define::secret_treasure_cluster& info)
{
	info.set_id(m_iID);
	info.set_score(m_iScore);
	info.set_times(m_iTimes);
}
void ACT_TreasureLog::GetDataFromPB(const char_data_define::secret_treasure_log& info)
{
	m_iTypeID = info.type_id();
	m_iCount = info.count();
}
void ACT_TreasureLog::SetDataToPB(char_data_define::secret_treasure_log& info)
{
	info.set_type_id(m_iTypeID);
	info.set_count(m_iCount);
}
void ACT_SecretTreasureChestEntry::GetDataFromPB(const char_data_define::secret_treasure_chest& info)
{
	m_iID = info.id();
	m_iTimes = info.times();
}
void ACT_SecretTreasureChestEntry::SetDataToPB(char_data_define::secret_treasure_chest& info)
{
	info.set_id(m_iID);
	info.set_times(m_iTimes);
}
void ACT_SecretTreasureShopEntry::GetDataFromPB(const char_data_define::secret_treasure_shop& info)
{
	m_kTimes.clear();
	m_iID = info.id();
	for (int i=0; i<info.stats_size(); ++i)
	{
		m_kTimes[info.stats(i).key()] = info.stats(i).value();
	}
}
void ACT_SecretTreasureShopEntry::SetDataToPB(char_data_define::secret_treasure_shop& info)
{
	info.set_id(m_iID);
	TimesMap::iterator it = m_kTimes.begin();
	for (; it!=m_kTimes.end(); ++it)
	{
		char_data_define::stat_pair* pkentry = info.add_stats();
		pkentry->set_key(it->first);
		pkentry->set_value(it->second);
	}
}
int32_t ACT_SecretTreasureCatagoryEntry::GetTimes(void)
{
	int32_t iTimes = 0;
	ACT_SecretTreasureChestEntryMap::iterator it = m_kChests.begin();
	for (; it!=m_kChests.end(); ++it)
	{
		iTimes += it->second.m_iTimes;
	}
	return iTimes;
}
void ACT_SecretTreasureCatagoryEntry::GetDataFromPB(const char_data_define::secret_treasure_catagory& info)
{
	m_iCatagory = info.cat();
	static ACT_SecretTreasureManager* pkDataMgr = ACT_SecretTreasureManager::GetSingletonPtr();
	ACT_SecretTreasureCatagoryDataEntry* pkCatagoryDataEntry = pkDataMgr->GetCatagory(m_iCatagory);
	if (NULL == pkCatagoryDataEntry)
	{
		return;
	}
	{
		m_kClusters.clear();
		for (int i=0; i<info.clusters_size(); ++i)
		{
			const char_data_define::secret_treasure_cluster& rkentry = info.clusters(i);
			if (!pkCatagoryDataEntry->IsValidCluster(rkentry.id()))
			{
				continue;
			}
			ACT_SecretTreasureCluserEntry* pkCluster = _addClusterEntry(rkentry.id());
			if (pkCluster)
			{
				pkCluster->GetDataFromPB(rkentry);
			}
		}
	}	
	{
		m_kLogs.clear();
		for (int i=0; i<info.logs_size(); ++i)
		{
			ACT_TreasureLog kLog;
			kLog.GetDataFromPB(info.logs(i));

			m_kLogs.push_back(kLog);
		}
	}
	{
		m_kChests.clear();
		for (int i=0; i<info.chests_size(); ++i)
		{
			const char_data_define::secret_treasure_chest& rkentry = info.chests(i);
			ACT_SecretTreasureChestEntry* pkChest = _addChestEntry(rkentry.id());
			if (pkChest)
			{
				pkChest->GetDataFromPB(info.chests(i));
			}
		}
	}
	{
		m_kShops.clear();
		for (int i=0; i<info.shops_size(); ++i)
		{
			const char_data_define::secret_treasure_shop& rkentry = info.shops(i);
			ACT_SecretTreasureShopEntry* pkShop = _addShopEntry(rkentry.id());
			if (pkShop)
			{
				pkShop->GetDataFromPB(info.shops(i));
			}
		}
	}
}
void ACT_SecretTreasureCatagoryEntry::SetDataToPB(char_data_define::secret_treasure_catagory& info)
{
	info.set_cat(m_iCatagory);
	{
		ACT_SecretTreasureCluserEntryMap::iterator it = m_kClusters.begin();
		for (; it!=m_kClusters.end(); ++it)
		{
			char_data_define::secret_treasure_cluster* pkentry = info.add_clusters();
			if (pkentry)
			{
				it->second.SetDataToPB(*pkentry);
			}
		}
	}
	{
		ACT_TreasureLogList::iterator itLog = m_kLogs.begin();
		for(; itLog!=m_kLogs.end(); ++itLog)
		{
			char_data_define::secret_treasure_log* pklog = info.add_logs();
			itLog->SetDataToPB(*pklog);
		}
	}
	{
		ACT_SecretTreasureChestEntryMap::iterator itChest = m_kChests.begin();
		for(; itChest!=m_kChests.end(); ++itChest)
		{
			char_data_define::secret_treasure_chest* pkChest = info.add_chests();
			itChest->second.SetDataToPB(*pkChest);
		}
	}
	{
		ACT_SecretTreasureShopEntryMap::iterator itShop = m_kShops.begin();
		for(; itShop!=m_kShops.end(); ++itShop)
		{
			char_data_define::secret_treasure_shop* pkShop = info.add_shops();
			itShop->second.SetDataToPB(*pkShop);
		}
	}
}
void ACT_SecretTreasureCatagoryEntry::RegLog(const Utility::UT_SIMDataInfo& kSimDataInfo)
{
	ACT_TreasureLog kLog;
	kLog.m_iTypeID = kSimDataInfo.ID();
	kLog.m_iCount = kSimDataInfo.ItemCount();
	m_kLogs.push_back(kLog);

	if ((int32_t)m_kLogs.size() > MAX_TREASURE_LOG_COUNT)
	{
		m_kLogs.pop_front();
	}

	UpdateDirtyFlag();
}
ACT_SecretTreasureCluserEntry* ACT_SecretTreasureCatagoryEntry::_addClusterEntry(int32_t iClusterID)
{
	ACT_SecretTreasureCluserEntryMap::iterator it = m_kClusters.find(iClusterID);
	if (it != m_kClusters.end())
	{
		return &it->second;
	}

	if (iClusterID < 0)
	{
		return NULL;
	}

	ACT_SecretTreasureCluserEntry& rkEntry = m_kClusters[iClusterID];
	rkEntry.m_iID = iClusterID;
	return &rkEntry;
}
ACT_SecretTreasureChestEntry* ACT_SecretTreasureCatagoryEntry::_addChestEntry(int32_t iChestID)
{
	ACT_SecretTreasureChestEntryMap::iterator it = m_kChests.find(iChestID);
	if (it != m_kChests.end())
	{
		return &it->second;
	}

	ACT_SecretTreasureChestEntry& rkEntry = m_kChests[iChestID];
	rkEntry.m_iID = iChestID;
	return &rkEntry;
}
ACT_SecretTreasureShopEntry* ACT_SecretTreasureCatagoryEntry::_addShopEntry(int32_t iShopID)
{
	ACT_SecretTreasureShopEntryMap::iterator it = m_kShops.find(iShopID);
	if (it != m_kShops.end())
	{
		return &it->second;
	}

	ACT_SecretTreasureShopEntry& rkEntry = m_kShops[iShopID];
	rkEntry.m_iID = iShopID;
	return &rkEntry;
}
void ACT_SecretTreasureAsset::CheckReset(uint32_t timestamp)
{
	if (!LC_Helper::CheckIsSameDay(m_uiTimestamp, timestamp))
	{
		{
			ACT_SecretTreasureCatagoryEntryMap::iterator it = m_kCatagorys.begin();
			for (; it!=m_kCatagorys.end(); ++it)
			{
				it->second.m_kChests.clear();
				it->second.m_kShops.clear();
			}
		}
		m_uiTimestamp = timestamp;
		UpdateDirtyFlag();
	}
}
void ACT_SecretTreasureAsset::GetDataFromPB(const char_data_define::secret_treasure_info& info)
{
	m_uiTimestamp = info.timestamp();
	for (int i=0; i<info.cats_size(); ++i)
	{
		const char_data_define::secret_treasure_catagory& rkentry = info.cats(i);
		m_kCatagorys[rkentry.cat()].GetDataFromPB(rkentry);
	}
}
void ACT_SecretTreasureAsset::SetDataToPB(char_data_define::secret_treasure_info& info)
{
	info.set_timestamp(m_uiTimestamp);
	ACT_SecretTreasureCatagoryEntryMap::iterator it = m_kCatagorys.begin();
	for (; it!=m_kCatagorys.end(); ++it)
	{
		char_data_define::secret_treasure_catagory* pkentry = info.add_cats();
		if (pkentry)
		{
			it->second.SetDataToPB(*pkentry);
		}
	}
}
ACT_SecretTreasureCatagoryEntry* ACT_SecretTreasureAsset::_addCatagoryEntry(int32_t iCatagoryID)
{
	ACT_SecretTreasureCatagoryEntryMap::iterator it = m_kCatagorys.find(iCatagoryID);
	if (it != m_kCatagorys.end())
	{
		return &it->second;
	}
	
	ACT_SecretTreasureCatagoryEntry& rkEntry = m_kCatagorys[iCatagoryID];
	rkEntry.m_iCatagory = iCatagoryID;
	return &rkEntry;
}
void ACT_SecretTreasureManager::Init(void)
{
	m_kUpdateTimer.SetTimer(GET_INTERVAL(CMN_INTER_LAZY));
}
ResultType ACT_SecretTreasureManager::InitConfig(CSVFile::CSVDataLoader* pkLoader, const CSVFile::CSVDataSourceList* pkCSVs)
{
	Utility::Redis_Handler* pkRedisHandler = GetGameServerApp()->GetRedisHandler();
	if (NULL == pkRedisHandler)
	{
		return RE_FAIL;
	}

	uint32_t timestamp = GET_CURRENT_TIMESTAMP_IN_SECONDS();

	bool bScheduleReinit = false;
	{
		CSVFile::CF_SecretTreasureSchedule* inst = SafeGetSingleton<CSVFile::CF_SecretTreasureSchedule>();
		if(IS_VALID_RELATED_CSV(inst, pkCSVs))
		{
			bScheduleReinit = true;
			m_kCatagorys.clear();
			CSVFile::CF_SecretTreasureSchedule::DataEntryMapExternIterator iter = inst->GetIterator();
			while(iter.HasMoreElements())
			{
				CSVFile::CF_SecretTreasureSchedule::DataEntry* entry = iter.PeekNextValuePtr();
				ACT_SecretTreasureCatagoryDataEntry* pkCatagory = _addCatagory(entry->_iCatagory);
				
				if (pkCatagory)
				{
					ACT_SecretTreasureScheduleDataEntry kDataEntry;
					if (kDataEntry.Init(entry))
					{
						pkCatagory->m_kSchedules[kDataEntry.m_iSchedule] = kDataEntry;
						pkCatagory->m_kClusters.insert(kDataEntry.m_iCluster);
					}
				}
				iter.MoveNext();
			}

			ACT_SecretTreasureCatagoryDataEntryMap::iterator it = m_kCatagorys.begin();
			for (; it!=m_kCatagorys.end(); ++it)
			{
				it->second.SetUp();
				it->second.CheckReset(timestamp);
			}
		}
	}

	{
		CSVFile::CF_SecretTreasurePool* inst = SafeGetSingleton<CSVFile::CF_SecretTreasurePool>();
		if(bScheduleReinit || IS_VALID_RELATED_CSV(inst, pkCSVs))
		{
			ACT_SecretTreasureCatagoryDataEntryMap::iterator it = m_kCatagorys.begin();
			for (; it!=m_kCatagorys.end(); ++it)
			{
				it->second.ClearPools();
			}
			
			CSVFile::CF_SecretTreasurePool::DataEntryMapExternIterator iter = inst->GetIterator();
			while(iter.HasMoreElements())
			{
				CSVFile::CF_SecretTreasurePool::DataEntry* entry = iter.PeekNextValuePtr();
				ACT_SecretTreasureScheduleDataEntry* pkSchedule = GetSchedule(entry->_iCatagory, entry->_iSchedule);
				if (pkSchedule )
				{
					ACT_SecretTreasurePoolDataEntry kDataEntry;
					if (kDataEntry.Init(entry))
					{
						pkSchedule->AddPoolEntry(kDataEntry);
					}
				}
				iter.MoveNext();
			}
		}
	}

	{
		CSVFile::CF_SecretTreasurePut* inst = SafeGetSingleton<CSVFile::CF_SecretTreasurePut>();
		if(IS_VALID_RELATED_CSV(inst, pkCSVs))
		{
			ClearPuts();
			CSVFile::CF_SecretTreasurePut::DataEntryMapExternIterator iter = inst->GetIterator();
			while(iter.HasMoreElements())
			{
				CSVFile::CF_SecretTreasurePut::DataEntry* entry = iter.PeekNextValuePtr();
				
				ACT_SecretTreasurePutDataEntry kDataEntry;
				if (kDataEntry.Init(entry))
				{
					m_kPuts[kDataEntry.m_iID] = kDataEntry;
					m_kPuts[kDataEntry.m_iID].SetUp();
					m_kPuts[kDataEntry.m_iID].CheckReset(timestamp);
				}
				iter.MoveNext();
			}
		}
	}

	{
		CSVFile::CF_SecretTreasureChests* inst = SafeGetSingleton<CSVFile::CF_SecretTreasureChests>();
		if(IS_VALID_RELATED_CSV(inst, pkCSVs))
		{
			m_kChests.clear();
			CSVFile::CF_SecretTreasureChests::DataEntryMapExternIterator iter = inst->GetIterator();
			while(iter.HasMoreElements())
			{
				CSVFile::CF_SecretTreasureChests::DataEntry* entry = iter.PeekNextValuePtr();

				ACT_SecretTreasureChestDataEntry kDataEntry;
				if (kDataEntry.Init(entry))
				{
					m_kChests[kDataEntry.m_iID]= kDataEntry;
				}
				iter.MoveNext();
			}
		}
	}

	{
		CSVFile::CF_SecretTreasureShop* inst = SafeGetSingleton<CSVFile::CF_SecretTreasureShop>();
		if (IS_VALID_RELATED_CSV(inst, pkCSVs))
		{
			
			CSVFile::CF_SecretTreasureShop::DataEntryMapExternIterator iter = inst->GetIterator();
			while(iter.HasMoreElements())
			{
				CSVFile::CF_SecretTreasureShop::DataEntry* entry = iter.PeekNextValuePtr();

				ACT_SecretTreasureShopDataEntry kDataEntry;
				if (kDataEntry.Init(entry))
				{
					m_kShops[kDataEntry.m_iID] = kDataEntry;
				}
				iter.MoveNext();
			}
		}
	}
	return RE_SUCCESS;
}
void ACT_SecretTreasureManager::Unit(void)
{
	m_kCatagorys.clear();
	m_kChests.clear();
	m_kShops.clear();

	ClearPuts();
}
ResultType ACT_SecretTreasureManager::OnGameServerLoaded(void)
{
	if (m_bInited)
	{
		return RE_SUCCESS;
	}

	if (!GetGameServerApp()->GetServerManager()->isLoaded())
	{
		return RE_FAIL;
	}
	if(RE_SUCCESS != InitConfig())
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "ACT_SecretTreasureManager InitConfig Failed!");
		return RE_FAIL;
	}
	m_bInited = true;
	return RE_SUCCESS;
}
void ACT_SecretTreasureManager::CheckReset(uint32_t timestamp)
{
	{
		ACT_SecretTreasureCatagoryDataEntryMap::iterator it = m_kCatagorys.begin();
		for (; it!=m_kCatagorys.end(); ++it)
		{
			ACT_SecretTreasureCatagoryDataEntry& rkCatagory = it->second;
			rkCatagory.CheckReset(timestamp);
		}
	}

	{
		ACT_SecretTreasurePutDataEntryMap::iterator it = m_kPuts.begin();
		for (; it!=m_kPuts.end(); ++it)
		{
			ACT_SecretTreasurePutDataEntry& rkPut = it->second;
			rkPut.CheckReset(timestamp);
		}
	}
}
void ACT_SecretTreasureManager::Update(float fTime)
{
	if (!m_kUpdateTimer.IsExpired())
	{
		return;
	}

	{
		ACT_SecretTreasureCatagoryDataEntryMap::iterator it = m_kCatagorys.begin();
		for (; it!=m_kCatagorys.end(); ++it)
		{
			ACT_SecretTreasureCatagoryDataEntry& rkCatagory = it->second;
			rkCatagory.Update(fTime);
		}
	}

	{
		ACT_SecretTreasurePutDataEntryMap::iterator it = m_kPuts.begin();
		for (; it!=m_kPuts.end(); ++it)
		{
			it->second.Update(fTime);
		}
	}
}
int32_t ACT_SecretTreasureManager::GetExtraTimes(LC_ServerPlayer* pkPlayer, int32_t iChestID)
{
	if (NULL == pkPlayer)
	{
		return 0;
	}

	int32_t iTimes = 0;
	CSVFile::CF_SecretTreasureTimes* inst = SafeGetSingleton<CSVFile::CF_SecretTreasureTimes>();	
	CSVFile::CF_SecretTreasureTimes::DataEntryMapExternIterator iter = inst->GetIterator();
	while(iter.HasMoreElements())
	{
		CSVFile::CF_SecretTreasureTimes::DataEntry* entry = iter.PeekNextValuePtr();
		if (entry && entry->_iChestID==iChestID)
		{
			switch(entry->_iType)
			{
			case ACT_TIMES_TP_ONLINE:
				if (pkPlayer->GetCommonStatAsset().GetCommonStatBasic(LC_ST_TP_DAILY)->GetOnlineTime(GET_CURRENT_TIMESTAMP_IN_SECONDS()) >= (uint32_t)entry->_iValue)
				{
					iTimes += entry->_iTimes;
				}
				break;
			case ACT_TIMES_TP_LIVENESS:
				if (pkPlayer->GetCommonStatAsset().GetCommonStatBasic(LC_ST_TP_DAILY)->GetLiveness() >= (uint32_t)entry->_iValue)
				{
					iTimes += entry->_iTimes;
				}
				break;
			case ACT_TIMES_TP_RECHARGE:
				{
					uint32_t uiFactor = pkPlayer->GetCommonStatAsset().GetCommonStatBasic(LC_ST_TP_DAILY)->GetRecharge() / (uint32_t)entry->_iValue;
					int32_t iTempTimes = (uiFactor>=1 ? (entry->_iTimes + (uiFactor-1)*entry->_iAccumExpo) : 0);
					iTempTimes = (entry->_iLimit > 0 && iTempTimes > entry->_iLimit) ? entry->_iLimit : iTempTimes;
					iTimes += iTempTimes;
				}
				break;
			case ACT_TIMES_TP_CONSUME:
				{
					uint32_t uiFactor = pkPlayer->GetCommonStatAsset().GetCommonStatBasic(LC_ST_TP_DAILY)->GetConsume() / (uint32_t)entry->_iValue;
					int32_t iTempTimes = (uiFactor>=1 ? (entry->_iTimes + (uiFactor-1)*entry->_iAccumExpo) : 0);
					iTempTimes = (entry->_iLimit > 0 && iTempTimes > entry->_iLimit) ? entry->_iLimit : iTempTimes;
					iTimes += iTempTimes;
				}
				break;
			default:
				break;
			}
		}
		iter.MoveNext();
	}
	return iTimes;
}
ResultType ACT_SecretTreasureManager::CheckTreasureOpen(LC_ServerPlayer* pkPlayer, int32_t iChestID, int32_t iScheduleID)
{
	IF_A_NA_PLAYER(pkPlayer)
	{
		return RE_FAIL;
	}

	ACT_SecretTreasureChestDataEntry* pkChest = GetChest(iChestID);
	if (NULL == pkChest)
	{
		return RE_FAIL;
	}

	static ACT_SecretTreasureManager* pkMgr = ACT_SecretTreasureManager::GetSingletonPtr();
	static ACT_ScheduleManager* pkScheduleMgr = ACT_ScheduleManager::GetSingletonPtr();
	if(!pkScheduleMgr->IsScheduleValid(pkPlayer, iScheduleID))
	{
		return RE_FAIL;
	}

	ACT_SecretTreasureScheduleDataEntry* pkSchedule = GetSchedule(pkChest->m_iCatagory, iScheduleID);
	if (NULL == pkSchedule)
	{
		return RE_FAIL;
	}

	ACT_SecretTreasureAsset& rkAsset = pkPlayer->GetSecreteTreasureAsset();	
	ACT_SecretTreasureCatagoryEntry* pkCatagoryEntry = rkAsset._addCatagoryEntry(pkChest->m_iCatagory);
	if (NULL==pkCatagoryEntry)
	{
		return RE_FAIL;
	}

	ACT_SecretTreasureChestEntry* pkChestEntry = pkCatagoryEntry->_addChestEntry(pkChest->m_iID);
	if (NULL==pkChestEntry)
	{
		return RE_FAIL;
	}

	if (pkChest->m_iTimesLimit>=0 && pkChestEntry->m_iTimes>=pkChest->m_iTimesLimit + pkMgr->GetExtraTimes(pkPlayer, iChestID))
	{
		return RE_FAIL;
	}

	if (pkChest->m_iGold > 0)
	{
		if (pkPlayer->GetUnBindCash().GetMoney() < pkChest->m_iGold)
		{
			return RE_TREASURE_CHESTS_GOLD_LACK;
		}
	}

	LC_PackAsset* pkPackAsset = pkPlayer->GetPackAsset();
	if (!pkChest->m_kItems.empty())
	{
		PackOperateInfoList kConsumeOPs;
		LC_Helper::BuildConsumeItemOPs(pkChest->m_kItems, kConsumeOPs);
		ResultType rslt = LC_Helper::CheckItemOPs(pkPackAsset, kConsumeOPs);
		if (RE_SUCCESS != rslt)
		{
			return RE_TREASURE_CHESTS_ITEMS_LACK;
		}
	}

	if (!pkChest->m_kShopIDMap.empty())
	{
		ResultType rslt = LC_Helper::IsShopAvailable(pkPlayer, pkChest->m_kShopIDMap);

		if (RE_SUCCESS != rslt)
		{
			return rslt;
		}
	}

	return RE_SUCCESS;
}

ResultType ACT_SecretTreasureManager::CheckTreasureShop(LC_ServerPlayer* pkPlayer, int32_t iShopID)
{
	IF_A_NA_PLAYER(pkPlayer)
	{
		return RE_FAIL;
	}

	ACT_SecretTreasureShopDataEntry* pkShop = GetShop(iShopID);
	if (NULL == pkShop)
	{
		return RE_FAIL;
	}

	static ACT_ScheduleManager* pkScheduleMgr = ACT_ScheduleManager::GetSingletonPtr();
	if(!pkScheduleMgr->IsScheduleValid(pkPlayer, pkShop->m_iSchedule))
	{
		return RE_FAIL;
	}

	ACT_SecretTreasureScheduleDataEntry* pkSchedule = GetSchedule(pkShop->m_iCatagory, pkShop->m_iSchedule);
	if (NULL == pkSchedule)
	{
		return RE_FAIL;
	}

	ACT_SecretTreasureAsset& rkAsset = pkPlayer->GetSecreteTreasureAsset();
	ACT_SecretTreasureCatagoryEntry* pkCatagoryEntry = rkAsset._addCatagoryEntry(pkShop->m_iCatagory);
	if (NULL==pkCatagoryEntry)
	{
		return RE_FAIL;
	}

	ACT_SecretTreasureCluserEntry* pkClusterEntry = pkCatagoryEntry->_addClusterEntry(pkSchedule->m_iCluster);
	if (NULL == pkClusterEntry)
	{
		return RE_FAIL;
	}

	ACT_SecretTreasureShopEntry* pkShopEntry = pkCatagoryEntry->_addShopEntry(iShopID);
	if (NULL == pkShopEntry)
	{
		return RE_FAIL;
	}

	if (pkClusterEntry->m_iScore < pkShop->m_iScore)
	{
		return RE_FAIL;
	}

	if (pkCatagoryEntry->GetTimes() < pkShop->m_iTimes)
	{
		return RE_FAIL;
	}

	{
		Utility::UT_SIMDataMap::iterator it = pkShop->m_kLimits.begin();
		for (; it!=pkShop->m_kLimits.end(); ++it)
		{
			if (pkShopEntry->m_kTimes[LC_ST_TP_DAILY] >= it->second.ParamA())
			{
				return RE_FAIL;
			}
		}
	}

	PackOperateInfoList kItemOPs;
	LC_Helper::BuildAddItemOPs(pkShop->m_kItems, kItemOPs, SYS_FUNC_TP_ACTIVITY_TREASURE_SHOP, iShopID);
	return LC_Helper::CheckItemOPs(pkPlayer->GetPackAsset(), kItemOPs);
}

ResultType ACT_SecretTreasureManager::DoOpen(LC_ServerPlayer* pkPlayer, int32_t iChestID, int32_t iScheduleID, Utility::UT_SIMDataList& kRewards)
{
	IF_A_NA_PLAYER(pkPlayer)
	{
		return RE_FAIL;
	}

	ACT_SecretTreasureChestDataEntry* pkChest = GetChest(iChestID);
	if (NULL == pkChest)
	{
		return RE_FAIL;
	}

	ACT_SecretTreasureScheduleDataEntry* pkSchedule = GetSchedule(pkChest->m_iCatagory, iScheduleID);
	if (NULL == pkSchedule)
	{
		return RE_FAIL;
	}

	ACT_SecretTreasureCatagoryDataEntry* pkCatagory = GetCatagory(pkChest->m_iCatagory);
	if (NULL == pkCatagory)
	{
		return RE_FAIL;
	}

	ACT_SecretTreasureAsset& rkAsset = pkPlayer->GetSecreteTreasureAsset();
	ACT_SecretTreasureCatagoryEntry* pkCatagoryEntry = rkAsset._addCatagoryEntry(pkChest->m_iCatagory);
	if (NULL==pkCatagoryEntry)
	{
		return RE_FAIL;
	}

	ACT_SecretTreasureCluserEntry* pkClusterEntry = pkCatagoryEntry->_addClusterEntry(pkSchedule->m_iCluster);
	if (NULL == pkClusterEntry)
	{
		return RE_FAIL;
	}

	ACT_SecretTreasureChestEntry* pkChestEntry = pkCatagoryEntry->_addChestEntry(pkChest->m_iID);
	if (NULL==pkChestEntry)
	{
		return RE_FAIL;
	}

	ResultType rslt = RE_SUCCESS;
	do 
	{
		if (!pkChest->m_kItems.empty())
		{
			PackOperateInfoList objOPList;
			LC_Helper::BuildConsumeItemOPs(pkChest->m_kItems, objOPList);
			rslt = LC_Helper::DoItemOPs(pkPlayer, objOPList, SYS_FUNC_TP_ACTIVITY_TREASURE_OPEN, pkChest->m_iCatagory);
			if (RE_SUCCESS != rslt)
			{
				break;
			}
		}

		if (!pkChest->m_kShopIDMap.empty())
		{
			LC_Helper::PayForShop(pkPlayer, pkChest->m_kShopIDMap, SYS_FUNC_TP_ACTIVITY_TREASURE_OPEN, pkChest->m_iCatagory);
		}

		if (pkChest->m_iGold > 0)
		{
			pkPlayer->ReduceCash(pkChest->m_iGold, CT_UNBIND_CASH, SYS_FUNC_TP_ACTIVITY_TREASURE_OPEN, pkChest->m_iCatagory);
		}

		GameLogic::LC_RewardItemsExtend kExtend;
		kExtend.SetBind(pkChest->m_iBind > 0);
		LC_RumorManager* pkRumorMgr = LC_RumorManager::GetSingletonPtr();
	
		for (int32_t i=0; i<pkChest->m_iRandomTimes; ++i)
		{
			++pkChestEntry->m_iTimes;
			ACT_SecretTreasurePoolDataEntry* pkPool = pkSchedule->ResolvePool(pkPlayer, pkCatagoryEntry->GetTimes(), ++pkCatagory->m_iTimes);
			if (NULL == pkPool)
			{
				continue;
			}

			size_t cur_idx = kRewards.size();
			pkPool->Resolve(pkPlayer, kRewards, kExtend);
			if (kRewards.empty())
			{
				GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "open treasure", iChestID, pkChestEntry->m_iTimes, pkCatagory->m_iTimes, pkPool->m_iID);
			}
			pkCatagory->UpdateDirtyFlag();
		}

		UT_ServerHelper::HandleBonusItems(pkChest->m_iPackType, kRewards, pkPlayer, SYS_FUNC_TP_ACTIVITY_TREASURE_OPEN, iChestID, pkChest->m_sMailTitle, pkChest->m_sMailDesc);

		if (pkChest->m_iScore > 0)
		{
			pkClusterEntry->m_iScore += pkChest->m_iScore;
		}
		rkAsset.UpdateDirtyFlag();

	} while (0);
	return RE_SUCCESS;
}
ResultType ACT_SecretTreasureManager::DoShop(LC_ServerPlayer* pkPlayer, int32_t iShopID)
{
	IF_A_NA_PLAYER(pkPlayer)
	{
		return RE_FAIL;
	}

	ACT_SecretTreasureShopDataEntry* pkShop = GetShop(iShopID);
	if (NULL == pkShop)
	{
		return RE_FAIL;
	}

	static ACT_ScheduleManager* pkScheduleMgr = ACT_ScheduleManager::GetSingletonPtr();
	if(!pkScheduleMgr->IsScheduleValid(pkPlayer, pkShop->m_iSchedule))
	{
		return RE_FAIL;
	}

	ACT_SecretTreasureScheduleDataEntry* pkSchedule = GetSchedule(pkShop->m_iCatagory, pkShop->m_iSchedule);
	if (NULL == pkSchedule)
	{
		return RE_FAIL;
	}

	ACT_SecretTreasureAsset& rkAsset = pkPlayer->GetSecreteTreasureAsset();
	ACT_SecretTreasureCatagoryEntry* pkCatagoryEntry = rkAsset._addCatagoryEntry(pkShop->m_iCatagory);
	if (NULL==pkCatagoryEntry)
	{
		return RE_FAIL;
	}
	ACT_SecretTreasureCluserEntry* pkClusterEntry = pkCatagoryEntry->_addClusterEntry(pkSchedule->m_iCluster);
	if (NULL == pkClusterEntry)
	{
		return RE_FAIL;
	}
	ACT_SecretTreasureShopEntry* pkShopEntry = pkCatagoryEntry->_addShopEntry(iShopID);
	if (NULL == pkShopEntry)
	{
		return RE_FAIL;
	}

	{
		Utility::UT_SIMDataMap::iterator it = pkShop->m_kLimits.begin();
		for (; it!=pkShop->m_kLimits.end(); ++it)
		{
			pkShopEntry->m_kTimes[LC_ST_TP_DAILY] += 1;
		}
	}

	pkClusterEntry->m_iScore = UT_MathBase::MaxInt(0, pkClusterEntry->m_iScore-pkShop->m_iScore);
	rkAsset.UpdateDirtyFlag();
	PackOperateInfoList kItemOPs;
	LC_Helper::BuildAddItemOPs(pkShop->m_kItems, kItemOPs, SYS_FUNC_TP_ACTIVITY_TREASURE_SHOP, iShopID);
	return LC_Helper::DoItemOPs(pkPlayer, kItemOPs, SYS_FUNC_TP_ACTIVITY_TREASURE_SHOP, iShopID);
}
void ACT_SecretTreasureManager::_onCronPut(Utility::timer_id_type timerID, int32_t iPut)
{
	static ACT_SecretTreasureManager* pkMgr = ACT_SecretTreasureManager::GetSingletonPtr();
	ACT_SecretTreasurePutDataEntry* pkPut = pkMgr->GetPut(iPut);
	if (NULL == pkPut)
	{
		return;
	}

	pkPut->CheckPut();
}
