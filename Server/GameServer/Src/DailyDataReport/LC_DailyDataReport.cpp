#include "LC_DailyDataReport.h"

#include <time.h>

#include "GameServerApp.h"
#include "LC_RankManager.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerPlayerManager.h"
#include "oss_define.h"
#include "UT_Func.h"

static const std::string BASE_INFO_STR = "base_info";
static const int32_t ONLINE_SCORE_MASK = 100000;
static const int32_t FLUSH_DIRTY_COUNT = 10;

void report_base_info::daily_reset()
{
	recharge_roles.clear();
	active_roles.clear();
}

data_report::data_report()
{
	reset();
}

void data_report::init()
{
	static GameServerApp* pkApp = GetGameServerApp();
	union_id = pkApp->GetServerUnionInfo().union_id;
	server_id = pkApp->GetServerManager()->GetCurServerId(); 
}

void data_report::reset()
{
	union_id = 0;
	server_id = 0;
	start_day = 0;
	conflate_day = 0;
	recharge = 0;
	bonus = 0;
	yb_consume = 0;
	active = 0;
	max_online = 0;
	avg_online = 0;
	recharge_role = 0;
	create_char = 0;
	world_level = 0;
	yb_deposit = 0;
}

void data_report::daily_reset()
{
	recharge = 0;
	bonus = 0;
	yb_consume = 0;
	active = 0;
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (pkPlayerManager)
	{
		max_online = pkPlayerManager->GetPlayerCount();
	}
	else
	{
		max_online = 0;
	}
	avg_online = 0;
	recharge_role = 0;
	create_char = 0;
	yb_deposit = 0;
}

void data_report::get_start_day()
{
	static GameServerApp* pkApp = GetGameServerApp();
	static GameServerManager* pkServerMgr = pkApp->GetServerManager();
	uint32_t startTime = (uint32_t)pkServerMgr->GetServerStartTime();
	uint32_t conflateTime = (uint32_t)pkServerMgr->GetServerConflateTime();
	uint32_t timestamp = (uint32_t)time(NULL);
	start_day = (startTime > 0) ?
		(LC_Helper::GetDeltaDay(startTime, timestamp) + 1) : 0;
	conflate_day = (conflateTime > 0) ?
		(LC_Helper::GetDeltaDay(conflateTime, timestamp) + 1) : 0;
}

void hourly_data_report::hourly_reset()
{
	online = 0;
	online_ip = 0;
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (pkPlayerManager)
	{
		online = pkPlayerManager->GetPlayerCount();
		online_ip = pkPlayerManager->GetOnlineIPCount();
	}
}

std::string getDateString(time_t timestamp)
{
	tm t = *localtime(&timestamp);
	char buf[256] = {0};
	sprintf(buf, "%04d-%02d-%02d", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday);
	return buf;
}

std::string getDateStringWithHour(time_t timestamp)
{
	tm t = *localtime(&timestamp);
	char buf[256] = {0};
	sprintf(buf, "%04d-%02d-%02d_%02d", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour);
	return buf;
}

std::string getHourlyDataField(time_t timestamp)
{
	tm t = *localtime(&timestamp);
	char buf[256] = {0};
	sprintf(buf, "%04d%02d%02d%02d", t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour);
	return buf;
}

int64_t getOnlineRecordScore(uint32_t timestamp, int32_t onlineCount)
{
	timestamp /= 100;
	onlineCount = onlineCount > ONLINE_SCORE_MASK - 1 ?
		ONLINE_SCORE_MASK - 1 : onlineCount;
	return (int64_t)timestamp * ONLINE_SCORE_MASK + onlineCount;
}

void getOnlineRecordFromScore(int64_t onlineScore, int32_t& hour, int32_t& onlineCount)
{
	time_t timer = (time_t)(onlineScore / ONLINE_SCORE_MASK) * 100;
	onlineCount = (int32_t)(onlineScore % ONLINE_SCORE_MASK);
	struct tm t = *localtime(&timer);
	hour = t.tm_hour;
}

bool LC_DailyDataReport::Init()
{
	GameServerApp* pkApp = GetGameServerApp();
	std::string sKey = pkApp->GetRedisServerPredix() + UT_REDIS_KEY_DAILY_DATA_REPORT;
	Utility::Redis_Handler* pkRedisHandler = pkApp->GetRedisHandler();
	int ret = 0;
	{
		UT_REDIS_CMD_BEGIN(ret, pkRedisHandler);
		redispp::StringReply kReply = pkRedisConn->hget(sKey, BASE_INFO_STR);
		if (kReply.result())	
		{
			std::string reply = kReply;
			std::string errorMsg;
			if (!boost::ajson::load_from_buff(m_kBaseInfo, reply.c_str(), errorMsg))
			{
				return false;
			}
			m_kRechargeRoles.insert(m_kBaseInfo.recharge_roles.begin(),
									m_kBaseInfo.recharge_roles.end());
			m_kActiveRoles.insert(m_kBaseInfo.active_roles.begin(),
								  m_kBaseInfo.active_roles.end());
		}
		UT_REDIS_CMD_END(ret, pkRedisHandler);
	}
	if (ret != 0)
	{
		return false;
	}

	if (m_kBaseInfo.last_sync_time == 0)
	{
		return true;
	}

	time_t curSecond = time(NULL);
	uint32_t curZeroSecond = LC_Helper::GetDayEndTimestamp((uint32_t)curSecond, 0);
	bool bNeedReport = false;
	{
		std::string field;
		if (LC_Helper::GetDayEndTimestamp((uint32_t)m_kBaseInfo.last_sync_time, 0) < curZeroSecond
			&& m_kBaseInfo.report_flag <= 0)
		{
			bNeedReport = true;
			field = getDateString(m_kBaseInfo.last_sync_time);
		}
		else
		{
			field = getDateString(curSecond);
		}
		UT_REDIS_CMD_BEGIN(ret, pkRedisHandler);
		redispp::StringReply kReply = pkRedisConn->hget(sKey, field);
		if (kReply.result())
		{
			std::string reply = kReply;
			std::string errorMsg;
			if (!boost::ajson::load_from_buff(m_kDailyDataRecord, reply.c_str(), errorMsg))
			{
				return false;
			}
		}
		UT_REDIS_CMD_END(ret, pkRedisHandler);
		if (ret != 0)
		{
			return false;
		}
	}
	if (LC_Helper::CheckIsSameDay(m_kBaseInfo.last_online_record_time, (uint64_t)curSecond))
	{
		std::string sOnlineKey = pkApp->GetRedisServerPredix() + UT_REDIS_KEY_ONLINE_RECORD;
		int64_t minScore = getOnlineRecordScore((uint32_t)curZeroSecond, 0);
		int64_t maxScore = getOnlineRecordScore((uint32_t)curSecond, 0);
		UT_REDIS_CMD_BEGIN(ret, pkRedisHandler);
		redispp::MultiBulkEnumerator kReplys = pkRedisConn->zrangebyscore(sOnlineKey, minScore, maxScore);
		int32_t i = 0, hour = 0, iOnlineCount = 0;
		int64_t iOnlineScore = 0;
		std::string reply, member;
		while (kReplys.next(&reply))
		{
			if (i % 2 == 0)
			{
				member = reply;
				continue;
			}
			try {
				iOnlineScore = boost::lexical_cast<int64_t>(reply);
			} catch (...) {
				GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "Cast online score failed! ", reply, sOnlineKey, member);
				continue;
			}
			getOnlineRecordFromScore(iOnlineScore, hour, iOnlineCount);
			m_kOnlineRecord[hour] = iOnlineCount;
		}

		UT_REDIS_CMD_END(ret, pkRedisHandler);
		if (ret != 0)
		{
			return false;
		}
	}
	if (LC_Helper::CheckIsSameHour(m_kBaseInfo.last_hourly_record_time, (uint64_t)curSecond))
	{
		std::string sHourlyKey = pkApp->GetRedisServerPredix() + UT_REDIS_KEY_HOURLY_MAX_ONLINE_IP;
		std::string field = getDateStringWithHour(curSecond);
		redispp::StringReply kReply;
		UT_REDIS_CMD_WITH_REPLY(ret, pkRedisHandler, kReply, hget, sHourlyKey, field);
		if (ret != 0)
		{
			return false;
		}
		if (kReply.result())
		{
			std::string reply = kReply;
			std::string errorMsg;
			if (!boost::ajson::load_from_buff(m_kHourlyDataRecord, reply.c_str(), errorMsg))
			{
				return false;
			}
		}
	}

	m_kDailyDataRecord.init();
	if (bNeedReport)
	{
		ReportData();
		DailyReset();
		struct tm t = *localtime(&curSecond);
		int32_t iCount = OnlineRecord(t.tm_hour);
		AsyncUpdateOnlineRecord((uint32_t)curSecond, iCount);
		FillData();
		AsyncUpdateDataRecord();
		AsyncUpdateBaseInfo();
		CleanDirty();
	}
	return true;
}

void LC_DailyDataReport::Update(float fTime)
{
	if (fTime < m_fLastUpdateTime + 1.0f)
	{
		return;
	}
	m_fLastUpdateTime = fTime;

	time_t curSecond = time(NULL);
	if (curSecond - m_kBaseInfo.last_sync_time > SYNC_INTERVAL - 5 || m_iDirtyFlag >= 8)
	{
		AsyncLoadCreateChar();
	}
	if (m_iDirtyFlag < FLUSH_DIRTY_COUNT &&
		curSecond / SYNC_INTERVAL == m_kBaseInfo.last_sync_time / SYNC_INTERVAL)
	{
		return;
	}
	struct tm curTm = *localtime(&curSecond);
	struct tm lastTm = *localtime(&m_kBaseInfo.last_sync_time);
	if (m_kBaseInfo.last_sync_time < curSecond && curTm.tm_mday != lastTm.tm_mday)
	{
		ReportData();
		DailyReset();
	}
	struct tm hourlyTm = *localtime(&m_kBaseInfo.last_hourly_record_time);
	if (m_kBaseInfo.last_hourly_record_time < curSecond && curTm.tm_hour != hourlyTm.tm_hour)
	{
		HourlyReset();
	}
	if (m_kBaseInfo.last_online_record_time / ONLINE_RECORD_INTERVAL <
		curSecond / ONLINE_RECORD_INTERVAL)
	{
		struct tm t = *localtime(&curSecond);
		int32_t iCount = OnlineRecord(t.tm_hour);
		AsyncUpdateOnlineRecord((uint32_t)curSecond, iCount);
	}

	FillData();
	AsyncUpdateDataRecord();
	AsyncUpdateBaseInfo();
	CleanDirty();
}

void LC_DailyDataReport::AddRecharge(unique_id_type charID, int64_t iVal)
{
	if (iVal <= 0)
	{
		return;
	}
	m_kDailyDataRecord.recharge += iVal;
	if (m_kRechargeRoles.insert(charID).second)
	{
		m_kBaseInfo.recharge_roles.push_back(charID);
	}
	DirtyFlag(FLUSH_DIRTY_COUNT);
}

void LC_DailyDataReport::AddBonus(int64_t iVal)
{
	m_kDailyDataRecord.bonus += iVal;
	DirtyFlag();
}

void LC_DailyDataReport::AddYBConsume(int64_t iVal)
{
	m_kDailyDataRecord.yb_consume += iVal;
	DirtyFlag();
}

void LC_DailyDataReport::SetMaxOnline()
{
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (!pkPlayerManager)
	{
		return;
	}
	int32_t iOnline = pkPlayerManager->GetPlayerCount();
	if (m_kDailyDataRecord.max_online < iOnline)
	{
		m_kDailyDataRecord.max_online = iOnline;
	}
	DirtyFlag();

	bool bChange = false;
	if (m_kHourlyDataRecord.online < iOnline)
	{
		m_kHourlyDataRecord.online = iOnline;
		bChange = true;
	}
	int32_t iOnlineIP = pkPlayerManager->GetOnlineIPCount();
	if (m_kHourlyDataRecord.online_ip < iOnlineIP)
	{
		m_kHourlyDataRecord.online_ip = iOnlineIP;
		bChange = true;
	}
	if (bChange)
	{
		AsyncUpdateHourlyDataRecord();
	}
}

int32_t LC_DailyDataReport::OnlineRecord(int32_t hour)
{
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (!pkPlayerManager)
	{
		return 0;
	}
	int32_t iOnlineCount = pkPlayerManager->GetPlayerCount();
	m_kOnlineRecord[hour] = iOnlineCount;
	return iOnlineCount;
}

int32_t LC_DailyDataReport::GetAvgOnline()
{
	int32_t total = 0, num = 0;
	mem::map<int32_t, int32_t>::iterator it = m_kOnlineRecord.begin();
	for (; it != m_kOnlineRecord.end(); ++it)
	{
		total += it->second;
		++num;
	}
	num = (num < 1) ? 1 : num;
	return total / num;
}

void LC_DailyDataReport::OnRecharge(unique_id_type charID, int32_t iSrcType, int64_t iVal)
{
	if (iSrcType == SYS_FUNC_TP_GM)
	{
		AddBonus(iVal);
	}
	else if (iSrcType == SYS_FUNC_TP_CHARGE || iSrcType == SYS_FUNC_TP_RECHARGE)
	{
		AddRecharge(charID, iVal);
	}
}

void LC_DailyDataReport::OnYBConsume(int64_t iVal)
{
	AddYBConsume(iVal);
}

void LC_DailyDataReport::OnLogin(unique_id_type charID)
{
	SetMaxOnline();
	if (m_kActiveRoles.insert(charID).second)
	{
		m_kBaseInfo.active_roles.push_back(charID);
	}
}

void LC_DailyDataReport::DailyReset()
{
	AsyncUpdatePredayActive();
	m_kBaseInfo.report_flag = 0;
	m_kBaseInfo.daily_reset();
	m_kDailyDataRecord.daily_reset();
	m_kRechargeRoles.clear();
	m_kActiveRoles.clear();
	m_kOnlineRecord.clear();
	LC_ServerPlayerManager* pkPlayerMgr = SERVER_GET_PLAYER_MANAGER();
	if (pkPlayerMgr)
	{
		pkPlayerMgr->GetAllPlayerCharID(m_kActiveRoles);
		m_kBaseInfo.active_roles.insert(m_kBaseInfo.active_roles.end(),
			m_kActiveRoles.begin(), m_kActiveRoles.end());
	}
	DirtyFlag();
}

void LC_DailyDataReport::HourlyReset()
{
	m_kHourlyDataRecord.hourly_reset();
	AsyncUpdateHourlyDataRecord();
}

void LC_DailyDataReport::ReportData()
{
	oss_daily_data_report(m_kDailyDataRecord);
	m_kBaseInfo.report_flag = 1;
	DirtyFlag();
}

void LC_DailyDataReport::FillData()
{
	static GameServerApp* pkApp = GetGameServerApp();
	static GameServerManager* pkServerMgr = pkApp->GetServerManager();

	data_report& record = m_kDailyDataRecord;
	record.get_start_day();

	int32_t tempActive = (int32_t)m_kActiveRoles.size();
	record.active = UT_MathBase::MaxInt(tempActive, record.active);

	int32_t tempAvgOnline = GetAvgOnline();
	record.avg_online = UT_MathBase::MaxInt(tempAvgOnline, record.avg_online);

	int32_t tempRechargeRole = (int32_t)m_kRechargeRoles.size();
	record.recharge_role = UT_MathBase::MaxInt(tempRechargeRole, record.recharge_role);

	record.world_level = pkServerMgr->GetCrossServerLevel(record.server_id);

	record.yb_deposit = record.recharge + record.bonus - record.yb_consume;
}

void LC_DailyDataReport::AsyncUpdateBaseInfo()
{
	static GameServerApp* pkApp = GetGameServerApp();
	std::string sKey = pkApp->GetRedisServerPredix() + UT_REDIS_KEY_DAILY_DATA_REPORT;
	std::stringstream ss;
	boost::ajson::save_to_buff(m_kBaseInfo, ss);
	std::string data = ss.str().c_str();
	ASYNC_REDIS_REQUEST_NCB(pkApp->GetRedisChannel(), &redispp::Connection::hset, sKey, BASE_INFO_STR, data);
}

void LC_DailyDataReport::AsyncUpdateOnlineRecord(uint32_t timestamp, int32_t iOnlineCount)
{
	static GameServerApp* pkApp = GetGameServerApp();
	std::string sKey = pkApp->GetRedisServerPredix() + UT_REDIS_KEY_ONLINE_RECORD;
	std::string member = getDateStringWithHour((time_t)timestamp);
	int64_t iOnlineScore = getOnlineRecordScore(timestamp, iOnlineCount);
	ASYNC_REDIS_REQUEST_NCB(pkApp->GetRedisChannel(), &redispp::Connection::zadd, sKey, iOnlineScore, member);
	m_kBaseInfo.last_online_record_time = timestamp;
}

void LC_DailyDataReport::AsyncUpdateDataRecord()
{
	time_t timestamp = time(NULL);
	static GameServerApp* pkApp = GetGameServerApp();
	std::string sKey = pkApp->GetRedisServerPredix() + UT_REDIS_KEY_DAILY_DATA_REPORT;
	std::string field = getDateString(timestamp);
	std::stringstream ss;
	boost::ajson::save_to_buff(m_kDailyDataRecord, ss);
	std::string data = ss.str().c_str();
	ASYNC_REDIS_REQUEST_NCB(pkApp->GetRedisChannel(), &redispp::Connection::hset, sKey, field, data);
	m_kBaseInfo.last_sync_time = timestamp;
}

void LC_DailyDataReport::AsyncUpdateHourlyDataRecord()
{
	time_t timestamp = time(NULL);
	static GameServerApp* pkApp = GetGameServerApp();
	std::string sKey = pkApp->GetRedisServerPredix() + UT_REDIS_KEY_HOURLY_MAX_ONLINE_IP;
	std::string field = getHourlyDataField(timestamp);
	std::stringstream ss;
	boost::ajson::save_to_buff(m_kHourlyDataRecord, ss);
	std::string data = ss.str().c_str();
	ASYNC_REDIS_REQUEST_NCB(pkApp->GetRedisChannel(), &redispp::Connection::hset, sKey, field, data);
	m_kBaseInfo.last_hourly_record_time = timestamp;
}

void LC_DailyDataReport::AsyncLoadCreateChar()
{
	static GameServerApp* pkApp = GetGameServerApp();
	std::string sKey = pkApp->GetRedisServerPredix() + PLATFORM_STAT_CREATE_CHAR;
	Utility::Redis_Channel& channel = pkApp->GetRedisChannel();
	time_t timestamp = time(NULL);
	std::string field = getDateString(timestamp);
	boost::function<void (int32_t, const std::string&)> _cb = boost::bind(&LC_DailyDataReport::on_get_create_char, this, _1, _2);
	ResultType rslt = RE_SUCCESS;
	ASYNC_REDIS_REQUEST(rslt, channel, &redispp::Connection::hget, _cb, sKey, field);
}

void LC_DailyDataReport::AsyncUpdatePredayActive()
{
	static GameServerApp* pkApp = GetGameServerApp();
	pkApp->SetSyncActiveFlag(false);
	std::string sKey = pkApp->GetRedisGroupPrefix() + UT_REDIS_KEY_PREDAY_ACTIVE;
	Utility::Redis_Channel& channel = pkApp->GetRedisChannel();
	uint32_t timestamp = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	std::string field = boost::lexical_cast<std::string>(GetGameServerID());
	int64_t value = (int64_t)timestamp * PREDAY_ACTIVE_MASK + m_kDailyDataRecord.active;
	std::string valueStr = boost::lexical_cast<std::string>(value);
	ASYNC_REDIS_REQUEST_NCB(channel, &redispp::Connection::hset, sKey, field, valueStr);
}

void LC_DailyDataReport::on_get_create_char(int32_t err, const std::string& reply)
{
	if (RE_SUCCESS != err)
	{
		return;
	}
	m_kDailyDataRecord.create_char = atoi(reply.c_str());
}
