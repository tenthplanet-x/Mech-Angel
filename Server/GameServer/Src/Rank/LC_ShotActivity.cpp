
#include "LC_ShotActivity.h"
#include "LC_CommonStatAsset.h"
#include "LC_ServerPlayer.h"
#include "LC_RankManager.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ItemFactoryBase.h"

#include "UT_Redis.h"
#include "UT_ServerHelper.h"
#include "GameServerApp.h"
#include "SystemDefine.h"
#include "LC_PackEntryFactory.h"
#include "UT_Timer.h"
#include "RedisScript.h"

#include "ajson_mem.hpp"

#include "redis.pb.h"

#include "MG_Game2Login.h"

#include "ACT_ActivityDataManager.h"
#include "ACT_Shot.h"
#include "ACT_ActivitySchedule.h"
#include "UT_OperateLog.h"
using namespace OPERATELOG;
using namespace GameLogic;
using namespace CSVFile;
using namespace Protocol;

AJSON(ShotActivityData::RankEntryData, (d)(v)(i))
AJSON(ShotActivityData::RankSnapData, (rankid)(timestamp)(rankdata))

#define MAX_SNAP_RETRY_TIMES 30
#define MAX_SNAP_RETRY_INTERVAL 10

ShotActivityData::RankCharIndices ShotActivityData::RankSnapData::kCharIndices;

void ShotActivityData::RankSnapData::SetUpDetail(void)
{
}

void ShotActivityData::RankSnapData::SetUpIndices(void)
{
	kDataIndices.clear();
	RankEntryDataVec::iterator it = rankdata.begin();
	for (; it != rankdata.end(); ++it)
	{
		kDataIndices[it->d] = &*it;
	}
}

inline ShotActivityData::RankEntryData* ShotActivityData::RankSnapData::GetRankDataAcrdIndex(int32_t idx)
{
	if (idx < 0 || idx >= (int32_t)rankdata.size())
	{
		return NULL;
	}
	return &rankdata[idx];
}

inline ShotActivityData::RankEntryData* ShotActivityData::RankSnapData::GetRankData(object_id_type cid)
{
	RankEntryDataIndices::iterator it = kDataIndices.find(cid);
	return (it != kDataIndices.end()) ? it->second : NULL;
}
void ShotActivityData::RankSnapData::OnPlayerPBDetailResp(const Protocol::PS_User_Detail_PBInfo& info)
{
	kCharIndices[info.m_CharID].cid = info.m_CharID;
	kCharIndices[info.m_CharID].name = info.m_CharName.c_str();
}
void ShotActivityData::RankSnapData::OnPlayerPBDetailResp(const unique_id_impl& charID, const std::string& info)
{
	redis_define::char_sim_info kchar;
	if(kchar.ParseFromString(info))
	{
		kCharIndices[charID].GetDataFromPB(kchar);
	}
}
//////////////////////////////////////////////////////////////////////////////////////////
void LC_ShotActivitySaveEntry::Init(int32_t iServerID, LC_ShotActivityEntry* pkEntry)
{
	m_iServer = iServerID;
	if (NULL == pkEntry)
	{
		return;
	}
	m_kSave.set_id(pkEntry->m_iID);
	static GameServerApp* pkApp = GetGameServerApp();
	std::string sServerPrefix = pkApp->GetRedisServerPredix(iServerID);
	m_sID = boost::lexical_cast<std::string>(pkEntry->m_iID);
	m_sRankID = boost::lexical_cast<std::string>(pkEntry->RankID());
	std::string sScheduleID = boost::lexical_cast<std::string>(pkEntry->ScheduleID());
	m_sSaveKey = sServerPrefix + UT_REDIS_KEY_SHOT_ACTIVITY_ENTRY;
	std::string sRankString;
	switch (pkEntry->RankSettle())
	{
	case LC_RK_STL_TP_GROUP:
		sRankString = UT_REDIS_KEY_SHOT_ACTIVITY_RANK;
		break;
	case LC_RK_STL_TP_REDIS:
		sRankString = UT_REDIS_KEY_SHOT_REDIS_RANK;
		break;
	default:
		break;
	}
	m_sRankCharKey.clear();
	switch(pkEntry->RankFlag())
	{
	case LOCAL_SERVER:
		{
			m_sRankPrefix = sServerPrefix + sRankString + ":" + m_sRankID + ":" + sScheduleID;
			m_sAvailCountsKey = sServerPrefix + UT_REDIS_KEY_AVAIL_COUNTS;
		}
		break;
	case GROUP_SERVER:
		{
			m_sRankPrefix = pkApp->GetRedisGroupPrefix() 
				+ UT_REDIS_KEY_SHOT_ACTIVITY_GROUP_PREFIX + boost::lexical_cast<std::string>(pkApp->GetServerManager()->GetCurGuildGroupId()) + sRankString + ":" + m_sRankID + ":" + sScheduleID;
			m_sAvailCountsKey = pkApp->GetRedisGroupPrefix() + UT_REDIS_KEY_AVAIL_COUNTS;
		}
		break;
	case GLOBAL_SERVER:
		{
			m_sRankPrefix = pkApp->GetRedisGlobalPrefix() + sRankString  + ":" + m_sRankID + ":" + sScheduleID;
			m_sAvailCountsKey = pkApp->GetRedisGlobalPrefix() + UT_REDIS_KEY_AVAIL_COUNTS;
			m_sRankCharKey = pkApp->GetRedisGlobalPrefix() + UT_REDIS_KEY_CHAR_CACHE;
		}
		break;
	default:
		break;
	}
}
std::string LC_ShotActivitySaveEntry::GetRankKey(uint32_t timestamp)
{
	std::stringstream _ss;
	_ss << m_sRankPrefix << ":" << timestamp;
	return _ss.str();
}
std::string LC_ShotActivitySaveEntry::GetCountKey(uint32_t timestamp)
{
	std::stringstream _ss;
	_ss << m_sRankPrefix << ":" << m_sID << ":" << timestamp;
	return _ss.str();
}
std::string LC_ShotActivitySaveEntry::GetRankCharKey(void)
{
	return m_sRankCharKey;
}
void LC_ShotActivitySaveEntry::OnShotAvailResp(uint32_t timestamp, int32_t ret, int32_t count)
{
	if (ret)
	{
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "fail to sync shot entry count", m_sID, m_iServer);
		return;
	}

	if (GetEndTime() != timestamp)
	{
		return;
	}
	SetCount((int32_t)count);
}
void LC_ShotActivitySaveEntry::Update(LC_ShotActivityEntry* pkEntry, bool bActive)
{
	if (NULL == pkEntry)
	{
		return;
	}

	static GameServerApp* pkApp = GetGameServerApp(); 
	if (GetDirtyFlag() && UT_ServerHelper::CheckLocal(GetServer()))
	{
		ASYNC_REDIS_REQUEST_NCB(pkApp->GetRedisChannel(), &redispp::Connection::hset, m_sSaveKey, m_sID, m_kSave.SerializeAsString());
		SetDirtyFlag(false);
	}

	if (bActive && pkEntry->IsCountLimit())
	{
		if (!m_kSyncTimer.IsExpired())
		{
			return;
		}

		static LC_ShotActivityManager* pkShotMgr = LC_ShotActivityManager::GetSingletonPtr();
		int32_t iSphere = FromRankFlagToSphere(getRankFlagByID(pkEntry->RankID()));
		Utility::Redis_Channel* pkChannel = pkApp->GetRedisChannel(iSphere);
		if (NULL == pkChannel)
		{
			return;
		}

		int32_t ret = 0;
		boost::function<void(int32_t, const std::string&)> cb = boost::bind(&LC_ShotActivityManager::OnShotAvailResp, pkShotMgr, GetID(), GetServer(), GetEndTime(), _1, _2);
		ASYNC_REDIS_REQUEST(ret, (*pkChannel), &redispp::Connection::hget, cb, m_sAvailCountsKey, GetCountKey(GetEndTime()));
	}
}
//////////////////////////////////////////////////////////////////////////////////////////
uint32_t LC_ShotActivityEntry::BenchTimestamp(int32_t iServerID) const 
{
	static Activity::ACT_ScheduleManager* pkScheduleMgr = Activity::ACT_ScheduleManager::GetSingletonPtr();
	Activity::ACT_ScheduleEntry* pkSchedule = pkScheduleMgr->GetSchedule(iServerID, ScheduleID());
	return pkSchedule ? uint32_t(pkSchedule->CurrentStartTime() / ONE_SECOND_MILI): 0;
}
//////////////////////////////////////////////////////////////////////////////////////////
uint32_t LC_ShotActivityEntry::ValidTimestamp(int32_t iServerID) const 
{
	static Activity::ACT_ScheduleManager* pkScheduleMgr = Activity::ACT_ScheduleManager::GetSingletonPtr();
	Activity::ACT_ScheduleEntry* pkSchedule = pkScheduleMgr->GetSchedule(iServerID, ScheduleID());
	return pkSchedule ? uint32_t(pkSchedule->CurrentEndTime() / ONE_SECOND_MILI): 0;
}
//////////////////////////////////////////////////////////////////////////////////////////
int32_t LC_ShotActivityEntry::PeekCount(int32_t iServerID) 
{ 
	if (m_pkData->_iCount<=0)
	{
		return 0xFFFF;
	}
	static GameServerApp* pkApp = GetGameServerApp(); 
	static Activity::ACT_ScheduleManager* pkScheduleMgr = Activity::ACT_ScheduleManager::GetSingletonPtr();
	LC_ShotActivitySaveEntry* pkSaveEntry = GetSaveEntry(iServerID);
	Activity::ACT_ScheduleEntry* pkSchedule = pkScheduleMgr->GetSchedule(iServerID, ScheduleID());
	if (pkSaveEntry && pkSchedule && pkSchedule->GetActive())
	{
		std::vector<std::string> keys;
		std::vector<std::string> args;

		keys.push_back(pkSaveEntry->m_sAvailCountsKey);
		keys.push_back(pkSaveEntry->GetCountKey(pkSaveEntry->GetEndTime()));

		args.push_back(boost::lexical_cast<std::string>(m_pkData->_iCount));
		 
		int32_t ret = 0;
		redispp::IntReply avail;
		int32_t iSpere = FromRankFlagToSphere(RankFlag());
		Utility::Redis_Handler* pkRedisHandler = pkApp->GetRedisHandler(iSpere);
		UT_REDIS_CMD_WITH_REPLY(ret, pkRedisHandler, avail, eval, Utility::stg_rsc_avail_get, keys, args);
		if (ret != 0)
		{
			GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "shot peek count failed!", pkRedisHandler->GetLastError());
		}
		else
		{		
			//avail is current count! should do one more
			pkSaveEntry->SetCount(m_pkData->_iCount-(int32_t)avail + 1);
		}
		return avail;
	}	
	return 0;
}
void LC_ShotActivityEntry::RegMailed(int32_t iServerID)
{ 
	LC_ShotActivitySaveEntry* pkEntry = GetSaveEntry(iServerID);
	if (pkEntry)
	{
		pkEntry->SetFlag(redis_define::shot_activity_entry::MAILED, true); 
	}
}
void LC_ShotActivityEntry::ResetMailed(int32_t iServerID) 
{
	LC_ShotActivitySaveEntry* pkEntry = GetSaveEntry(iServerID);
	if (pkEntry)
	{
		pkEntry->SetFlag(redis_define::shot_activity_entry::MAILED, false); 
	}
}
LC_ShotActivitySaveEntry* LC_ShotActivityEntry::GetSaveEntry(int32_t iServerID)
{	
	LC_ShotActivitySaveEntryMap::iterator it = m_kSaves.find(iServerID);
	if (it != m_kSaves.end())
	{
		return &it->second;
	}
	LC_ShotActivitySaveEntry& rkEntry = m_kSaves[iServerID];
	rkEntry.Init(iServerID, this);
	return &rkEntry;
}
void LC_ShotActivityEntry::OnShotAvailResp(int32_t iServerID, uint32_t timestamp, int32_t ret, int32_t count)
{
	LC_ShotActivitySaveEntry* pkSaveEntry = GetSaveEntry(iServerID);
	if (pkSaveEntry)
	{
		pkSaveEntry->OnShotAvailResp(timestamp, ret, count);
	}
}
//////////////////////////////////////////////////////////////////////////////////////////
void LC_ShotActivityRankRedisEntry::OnSync(int32_t ret, std::vector<std::string>& vals)
{

}
void LC_ShotActivityRankRedisEntry::OnRankSize(int32_t ret, int64_t size)
{
	if (ret)
	{
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "fail to sync rank entry", sKey);
		return;
	}
	kData.ranksize = (int)size;
}
void LC_ShotActivityRankRedisEntry::CheckSync(uint32_t timestamp)
{
	static GameServerApp* pkApp = GetGameServerApp(); 
	static LC_ShotActivityManager* pkShotMgr = LC_ShotActivityManager::GetSingletonPtr();
	int32_t iSphere = FromRankFlagToSphere(getRankFlagByID(iRankID));
	Utility::Redis_Channel* pkChannel = pkApp->GetRedisChannel(iSphere);
	if (NULL == pkChannel)
	{
		return;
	}

	if (IsFlagSet(LC_RK_ST_SUC) || IsFlagSet(LC_RK_ST_FAIL))
	{
		return;
	}

	if (iSize<=0)
	{
		return;
	}

	if (timestamp >= uiTimestamp )
	{
		SetFlag(LC_RK_ST_SUC, true);
		pkShotMgr->OnShotFixed(this);
	}
	else if(m_kSyncTimer.IsExpired())
	{
		int32_t ret = 0;
		boost::function<void(int32_t, std::vector<std::string>&)> cbSync = boost::bind(&LC_ShotActivityManager::OnRankSync, pkShotMgr, sKey, _1, _2);
		ASYNC_REDIS_REQUEST(ret, (*pkChannel), &redispp::Connection::zrevrange, cbSync, sKey, 0, iSize, "WITHSCORES");

		boost::function<void(int32_t, int64_t)> cbSize = boost::bind(&LC_ShotActivityManager::OnRankSize, pkShotMgr, sKey, _1, _2);
		ASYNC_REDIS_REQUEST(ret, (*pkChannel), &redispp::Connection::zcount, cbSize, sKey, 0, 0x7FFFFFFFFFFFFFFF);
	}
}
void LC_ShotActivityRankRedisEntry::OnResp(const unique_id_impl& charID, const msg_define::shot_activity_rank_info_req& req, int32_t ret, std::vector<std::string>& vals)
{

}

void LC_ShotActivityRankRedisEntry::OnRankIndex(const unique_id_impl& charID, const msg_define::shot_activity_rank_index_req& req, int32_t ret, int64_t index)
{
	if (ret)
	{
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "fail to req rank index", charID, req.key());
		return;
	}

	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(SERVER_GET_PLAYER_MANAGER())->FindPlayer(charID);
	if (NULL == pkPlayer)
	{
		return;
	}

	int32_t ranktype = getRankTypeByID(iRankID);
	MG_PB<msg_define::shot_activity_rank_index_resp> respMsg;
	msg_define::shot_activity_rank_index_resp& resp_val = respMsg.m_value;
	resp_val.mutable_req()->CopyFrom(req);
	resp_val.set_rankid(iRankID);
	resp_val.set_idx((int32_t)index);
	pkPlayer->SendMsgToClient(MGPT_RESP_SHOT_ACTIVITY_RANK_INDEX, &respMsg);
}
void LC_ShotActivityRankRedisEntry::RequestRank(LC_ServerPlayer* pkPlayer, const msg_define::shot_activity_rank_info_req& req)
{
	
}
void LC_ShotActivityRankRedisEntry::RequestRankIndex(LC_ServerPlayer* pkPlayer, const msg_define::shot_activity_rank_index_req& req)
{
	static GameServerApp* pkApp = GetGameServerApp(); 
	static LC_ShotActivityManager* pkShotMgr = LC_ShotActivityManager::GetSingletonPtr();
	int32_t iSphere = FromRankFlagToSphere(getRankFlagByID(iRankID));
	Utility::Redis_Channel* pkChannel = pkApp->GetRedisChannel(iSphere);
	if (NULL == pkChannel)
	{
		return;
	}

	int32_t ret = 0;
	std::string sCharID = boost::lexical_cast<std::string>(pkPlayer->GetInstance());
	boost::function<void(int32_t, int64_t)> cb = boost::bind(&LC_ShotActivityManager::OnRankIndex, pkShotMgr, pkPlayer->GetInstance(), req, _1, _2);
	ASYNC_REDIS_REQUEST(ret, (*pkChannel), &redispp::Connection::zrevrank, cb, sKey, sCharID);
}

bool LC_ShotActivityRankRedisEntry::UpdateChar(LC_ServerPlayer* pkPlayer)
{
	if (sRankCharKey.empty())
	{
		return false;
	}

	static GameServerApp* pkApp = GetGameServerApp(); 
	int32_t iSphere = FromRankFlagToSphere(getRankFlagByID(iRankID));
	Utility::Redis_Channel* pkChannel = pkApp->GetRedisChannel(iSphere);
	if (NULL == pkChannel)
	{
		return false;
	}

	std::string sCharID = boost::lexical_cast<std::string>(pkPlayer->GetInstance()); 
	redis_define::char_sim_info rk_char;
	rk_char.set_cid(pkPlayer->GetInstance());
	rk_char.set_name(pkPlayer->GetOwnerCharName().c_str());
	char_data_define::char_lite_info lite;
	char_data_define::char_base_info detail;
	pkPlayer->PBSaveCharLiteInfo(lite);
	pkPlayer->PBSaveCharBaseInfo(detail);
	rk_char.set_lite(lite.SerializeAsString());
	rk_char.set_detail(detail.SerializeAsString());
	ASYNC_REDIS_REQUEST_NCB((*pkChannel), &redispp::Connection::hset, sRankCharKey, sCharID, rk_char.SerializeAsString());
	return true;
}
//////////////////////////////////////////////////////////////////////////////////////////
ResultType LC_ShotActivityManager::Init(void)
{
	m_kUpdateTimer.SetTimer(GET_INTERVAL(CMN_INTER_LAZY));
	return RE_SUCCESS;
}

ResultType LC_ShotActivityManager::InitConfig(CSVFile::CSVDataLoader* pkLoader, const CSVFile::CSVDataSourceList* pkCSVs)
{
	CF_ShotActivity* pkShotActivity = CF_ShotActivity::GetSingletonPtr();
	if (!IS_VALID_RELATED_CSV(pkShotActivity, pkCSVs))
	{
		return RE_FAIL;
	}

	static GameServerManager* pkServerMgr = GetGameServerApp()->GetServerManager();
	if (!pkServerMgr->isLoaded())
	{
		return RE_FAIL;
	}

	Utility::Redis_Handler* pkRedisHandler = GetGameServerApp()->GetRedisHandler();
	if (NULL == pkRedisHandler)
	{
		return RE_FAIL;
	}

	static Activity::ACT_ScheduleManager* pkScheduleMgr = Activity::ACT_ScheduleManager::GetSingletonPtr();

	const msg_define::game_init_info& rkGameInit = pkServerMgr->GetGameInitInfo();

	uint32_t cur_timestamp = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	int ret = 0;
	m_kShotActivityEntryMap.clear();

	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "shot init config");
	CF_ShotActivity::DataEntryMapExternIterator kIter = pkShotActivity->GetIterator();
	while (kIter.HasMoreElements())
	{
		CF_ShotActivity::DataEntry* pkData = kIter.PeekNextValuePtr();
		{
			LC_ShotActivityEntry& kShotEntry = m_kShotActivityEntryMap[pkData->_iID];
			kShotEntry.m_iID = pkData->_iID;
			kShotEntry.m_pkData = pkData;

			//std::string sID = boost::lexical_cast<std::string>(pkData->_iID);

			for (int i=0; i<rkGameInit.time_infos_size(); ++i)
			{
				const msg_define::server_time_info& rkTimeInfo = rkGameInit.time_infos(i);
				Activity::ACT_ScheduleEntry* pkSchedule = pkScheduleMgr->GetSchedule(rkTimeInfo.server_id(), kShotEntry.ScheduleID());
				if (NULL == pkSchedule)
				{
					continue;
				}
				
				LC_ShotActivitySaveEntry* pkServerEntry = kShotEntry.GetSaveEntry(rkTimeInfo.server_id());
				if (pkServerEntry)
				{
					UT_REDIS_CMD_BEGIN(ret, pkRedisHandler);
					redispp::StringReply kEntryRly = pkRedisConn->hget(pkServerEntry->m_sSaveKey, pkServerEntry->m_sID);
					redis_define::shot_activity_entry& kentry = pkServerEntry->m_kSave;
					if (kEntryRly.result() && kentry.ParseFromString(kEntryRly))
					{
						GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "Get Rank Entry PB", pkServerEntry->m_iServer, kShotEntry.m_iID, kShotEntry.RankID()
							, pkServerEntry->GetBencheTime(), pkServerEntry->GetCount(), pkServerEntry->GetFlag());
					}
					else
					{
						GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "Get Rank Entry NA", pkServerEntry->m_iServer, kShotEntry.m_iID, kShotEntry.RankID());
					}
					
					UT_REDIS_CMD_END(ret, pkRedisHandler);
					GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "shot entry", kShotEntry.m_iID, rkTimeInfo.server_id(), pkServerEntry->GetCount(), pkServerEntry->GetBencheTime(), pkServerEntry->GetFlag());
				}
			}
		}
		kIter.MoveNext();
	}
	return RE_SUCCESS;
}

ResultType LC_ShotActivityManager::OnGameServerLoaded(void)
{
	if (m_bInited)
	{
		return RE_SUCCESS;
	}

	if(InitConfig() == RE_FAIL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "LC_ShotActivityManager InitConfig Failed!");
		return RE_FAIL;
	}
	m_bInited = true;
	return RE_SUCCESS;
}

void LC_ShotActivityManager::Unit(void)
{
	m_kShotActivityEntryMap.clear();
	
	{
		LC_ShotActivityRankEntryMap::iterator it = m_kShotActivityRankEntryMap.begin();
		for (; it!=m_kShotActivityRankEntryMap.end(); ++it)
		{
			T_SAFE_DELETE(it->second);
		}
		m_kShotActivityRankEntryMap.clear();
	}
	m_bInited = false;
}	

void LC_ShotActivityManager::Update(float fCurrentTime)
{
	if (!m_kUpdateTimer.IsExpired())
	{
		return;
	}

	uint32_t timestamp = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	{
		LC_ShotActivityRankEntryMap::iterator it = m_kShotActivityRankEntryMap.begin();
		for (; it!=m_kShotActivityRankEntryMap.end(); ++it)
		{
			it->second->Update(timestamp);
		}
	}

	{
		static Activity::ACT_ScheduleManager* pkScheduleMgr = Activity::ACT_ScheduleManager::GetSingletonPtr();
		LC_ShotActivityEntryMap::iterator it = m_kShotActivityEntryMap.begin();
		for (; it!=m_kShotActivityEntryMap.end(); ++it)
		{
			LC_ShotActivityEntry& rkEntry = it->second;
			
			LC_ShotActivitySaveEntryMap::iterator itSave = rkEntry.m_kSaves.begin();
			for (; itSave!=rkEntry.m_kSaves.end(); ++itSave)
			{
				LC_ShotActivitySaveEntry& rkSave = itSave->second;
				Activity::ACT_ScheduleEntry* pkSchedule = pkScheduleMgr->GetSchedule(rkSave.GetServer(), rkEntry.ScheduleID());
				if (pkSchedule && pkSchedule->IsScheduleValid())
				{
					uint32_t uiStartTime = (uint32_t)(pkSchedule->CurrentStartTime() / ONE_SECOND_MILI);
					uint32_t uiEndTime = (uint32_t)(pkSchedule->CurrentEndTime() / ONE_SECOND_MILI);
					if (uiStartTime != rkSave.GetBencheTime())
					{
						GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "reset shot entry", it->first, rkSave.GetServer(), rkSave.GetBencheTime(), uiStartTime);
						rkSave.Reset(uiStartTime);
					}
					rkSave.SetEndTime(uiEndTime);

					//if time before, just ignore
					if (timestamp < uiStartTime)
					{
						continue;
					}

					rkSave.Update(&rkEntry, pkSchedule->GetActive());
				}
			}
		}
	}
}

void LC_ShotActivityManager::OnShotAvailResp(int32_t iID, int32_t iServerID, uint32_t timestamp, int32_t ret, const std::string& count)
{
	LC_ShotActivityEntry* pkShotEntry = GetShotActivityEntry(iID);
	if (pkShotEntry)
	{
		pkShotEntry->OnShotAvailResp(iServerID, timestamp, ret, atol(count.c_str()));
	}
}

void LC_ShotActivityManager::OnCharInfo(unique_id_impl charID, std::string sRankKey, int32_t err, const std::string& sInfo)
{
	if (err || sInfo.empty())
	{
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "fail to req char info", charID, sRankKey, sInfo.length());
		return;
	}

	LC_ShotActivityRankEntryBase* pkRankEntry =  GetShotActivityRankEntry(sRankKey);
	if (pkRankEntry)
	{
		pkRankEntry->kData.OnPlayerPBDetailResp(charID, sInfo);
	}
}

void LC_ShotActivityManager::OnRankResp(unique_id_impl charID, msg_define::shot_activity_rank_info_req rank_req, int32_t ret, std::vector<std::string>& vals)
{
	LC_ShotActivityRankRedisEntry* pkRankEntry = dynamic_cast<LC_ShotActivityRankRedisEntry*>(GetShotActivityRankEntry(rank_req.key()));
	if (pkRankEntry)
	{
		pkRankEntry->OnResp(charID, rank_req, ret, vals);
	}
}

void LC_ShotActivityManager::OnRankIndex(unique_id_impl charID, msg_define::shot_activity_rank_index_req rank_idx_req, int32_t ret, int64_t idx)
{
	LC_ShotActivityRankRedisEntry* pkRankEntry = dynamic_cast<LC_ShotActivityRankRedisEntry*>(GetShotActivityRankEntry(rank_idx_req.key()));
	if (pkRankEntry)
	{
		pkRankEntry->OnRankIndex(charID, rank_idx_req, ret, idx);
	}
}

void LC_ShotActivityManager::OnRankSync(std::string sKey, int32_t ret, std::vector<std::string>& vals)
{
}

void LC_ShotActivityManager::OnRankSize(std::string sKey, int32_t ret, int64_t size)
{
	LC_ShotActivityRankRedisEntry* pkRankEntry = dynamic_cast<LC_ShotActivityRankRedisEntry*>(GetShotActivityRankEntry(sKey));
	if (pkRankEntry)
	{
		pkRankEntry->OnRankSize(ret, size);
	}
}

LC_ShotActivityEntry* LC_ShotActivityManager::GetShotActivityEntry(int32_t iID)
{
	LC_ShotActivityEntryMap::iterator it = m_kShotActivityEntryMap.find(iID);
	return (it != m_kShotActivityEntryMap.end()) ? &it->second : NULL;
}

void LC_ShotActivityManager::OnRankInfoReq(LC_ServerPlayer* pkPlayer, const msg_define::shot_activity_rank_info_req& rank_req)
{
	LC_ShotActivityRankRedisEntry* pkRankEntry = dynamic_cast<LC_ShotActivityRankRedisEntry*>(GetShotActivityRankEntry(rank_req.key()));
	if (pkRankEntry)
	{
		pkRankEntry->RequestRank(pkPlayer, rank_req);
	}
}

void LC_ShotActivityManager::OnRankIndexReq(LC_ServerPlayer* pkPlayer, const msg_define::shot_activity_rank_index_req& rank_idx_req)
{
	LC_ShotActivityRankRedisEntry* pkRankEntry = dynamic_cast<LC_ShotActivityRankRedisEntry*>(GetShotActivityRankEntry(rank_idx_req.key()));
	if (pkRankEntry)
	{
		pkRankEntry->RequestRankIndex(pkPlayer, rank_idx_req);
	}
}

void LC_ShotActivityManager::OnPlayerPBDetailResp(const Protocol::PS_User_Detail_PBInfo& info)
{
	LC_ShotActivityRankEntryMap::iterator it = m_kShotActivityRankEntryMap.begin();
	for (; it != m_kShotActivityRankEntryMap.end(); ++it)
	{
		it->second->kData.OnPlayerPBDetailResp(info);
	}
}
void LC_ShotActivityManager::OnShotFixed(LC_ShotActivityRankEntryBase* pkShotRank)
{
	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "on shot fixed", pkShotRank?pkShotRank->sKey:"", pkShotRank?pkShotRank->uiFlag:0);
	if (NULL == pkShotRank || !pkShotRank->IsFlagSet(LC_RK_ST_SUC))
	{
		return;
	}

	static Activity::ACT_ScheduleDataManager* pkACTDataMgr = Activity::ACT_ScheduleDataManager::GetSingletonPtr();
	static Activity::ACT_ShotManager* pkActivity = (Activity::ACT_ShotManager*)pkACTDataMgr->GetActivity(Activity::ACT_SCHEDULE_TP_SHOT);
	if (NULL == pkActivity)
	{
		return;
	}

	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	if (NULL == pkItemFactory)
	{
		return;
	}

	LC_PackEntryFactory* pkPackEntryFactory = LC_PackEntryFactory::GetSingletonPtr();
	if (NULL == pkPackEntryFactory)
	{
		return;
	}

	int32_t iServerID = GetGameServerID();
	if (!pkShotRank->IsRelatedServer(iServerID))
	{
		GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "on shot fixed not releted server", iServerID);
		return;
	}

	pkShotRank->ResetRankOff();

	LC_ShotActivityEntryMap::iterator it = m_kShotActivityEntryMap.begin();
	for (; it != m_kShotActivityEntryMap.end(); ++it)
	{
		LC_ShotActivityEntry& kShotEntry = it->second;

		if (!pkShotRank->IsSnapFor(kShotEntry))
		{
			continue;
		}

		Activity::ACT_ShotDataEntry* pkDataEntry = pkActivity->GetDataEntry(kShotEntry.RankID(), kShotEntry.ScheduleID(), kShotEntry.ACTKey());
		if (NULL == pkDataEntry)
		{
			continue;
		}

		LC_ShotActivitySaveEntry* pkSaveShotEntry = kShotEntry.GetSaveEntry(iServerID);
		if (NULL == pkSaveShotEntry)
		{
			continue;
		}

		GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "on shot fixed", kShotEntry.m_iID, pkSaveShotEntry->GetEndTime(), pkShotRank->uiTimestamp, kShotEntry.IsMailed(iServerID), kShotEntry.IsMailReward());
	}
}
