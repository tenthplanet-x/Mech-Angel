#pragma once

#include "IdDefine.h"
#include "LC_Define.h"
#include "ajson.hpp"

#define PREDAY_ACTIVE_MASK 10000000

struct report_base_info
{
	report_base_info() : last_sync_time(0), last_online_record_time(0), last_hourly_record_time(0), report_flag(0) {}
	void daily_reset();

	int64_t last_sync_time;
	int64_t last_online_record_time;
	int64_t last_hourly_record_time;
	int32_t report_flag;
	mem::vector<unique_id_type>	recharge_roles;
	mem::vector<unique_id_type> active_roles;
};
AJSON(report_base_info, (last_sync_time)(last_online_record_time)
	  (last_hourly_record_time)(report_flag)(recharge_roles)(active_roles));

struct data_report
{
	data_report();
	void init();
	void reset();
	void daily_reset();
	void get_start_day();

	int32_t		union_id;
	int32_t		server_id;
	int32_t		start_day;
	int32_t		conflate_day;
	int64_t		recharge;
	int64_t		bonus;
	int64_t		yb_consume;
	int32_t		active;
	int32_t		max_online;
	int32_t		avg_online;
	int32_t		recharge_role;
	int32_t		create_char;
	int32_t		world_level;
	int64_t		yb_deposit;
};
AJSON(data_report, (union_id)(server_id)(start_day)(conflate_day)
	  (recharge)(bonus)(yb_consume)(active)(max_online)(avg_online)(recharge_role)
	  (create_char)(world_level)(yb_deposit));

struct hourly_data_report
{
	hourly_data_report() : online(0), online_ip(0) {}
	void hourly_reset();

	int32_t		online;		//max online per hour
	int32_t		online_ip;	//max online ip per hour
};
AJSON(hourly_data_report, (online)(online_ip));

std::string getDataReportField(uint32_t timestamp);

class LC_DailyDataReport
{
public:
	static const int32_t SYNC_INTERVAL = 300;
	static const int32_t ONLINE_RECORD_INTERVAL = 3600;
	LC_DailyDataReport() : m_fLastUpdateTime(0), m_iDirtyFlag(0), m_iDailyDirtyFlag(0) {}

	bool Init();
	void Update(float fTime);

	void	AddRecharge(unique_id_type charID, int64_t iVal);
	void	AddBonus(int64_t iVal);
	void	AddYBConsume(int64_t iVal);
	void	SetMaxOnline();
	int32_t	OnlineRecord(int32_t hour);
	int32_t	GetAvgOnline();
	void	DirtyFlag(int32_t iVal = 1) { m_iDirtyFlag += iVal; }
	void	CleanDirty() { m_iDirtyFlag = 0; }

	void	OnRecharge(unique_id_type charID, int32_t iSrcType, int64_t iVal);
	void	OnYBConsume(int64_t iVal);
	void	OnLogin(unique_id_type charID);

	void	DailyReset();
	void	HourlyReset();
	void	ReportData();
	void	FillData();
	void	AsyncUpdateBaseInfo();
	void	AsyncUpdateOnlineRecord(uint32_t timestamp, int32_t iOnlineCount);
	void	AsyncUpdateDataRecord();
	void	AsyncUpdateHourlyDataRecord();
	void	AsyncLoadCreateChar();
	void	AsyncUpdatePredayActive();

private:
	void		on_get_create_char(int32_t err, const std::string& reply);

private:
	float						m_fLastUpdateTime;
	int32_t						m_iDirtyFlag;
	int32_t						m_iDailyDirtyFlag;
	report_base_info			m_kBaseInfo;
	data_report					m_kDailyDataRecord;
	hourly_data_report			m_kHourlyDataRecord;
	unique_id_set				m_kRechargeRoles;
	unique_id_set				m_kActiveRoles;
	mem::map<int32_t, int32_t>	m_kOnlineRecord;
};
