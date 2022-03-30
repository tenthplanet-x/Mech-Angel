#pragma once

#include "LC_Define.h"
#include "LC_ResourceRecord.h"
#include "char_data.pb.h"

#include "CF_ResourceAlert.h"
#include "ajson.hpp"

using namespace GameLogic;
class LC_ServerPlayer;
class LC_ResourceStat
{
public:
	LC_ResourceStat() : m_bRecord(false), m_iTotalCount(0) {}

	void		AddCount(uint64_t iVal) { m_iTotalCount += iVal; }
	uint64_t	GetCount() const { return m_iTotalCount; }
	void		OnRecord() { m_bRecord = true; };
	bool		IsRecord() const { return m_bRecord; }

public:
	bool		m_bRecord;
	uint64_t	m_iTotalCount;
};

class LC_ResourceAlert
{
public:
	LC_ResourceAlert() {}

	void				Init();
	void				Clear();
	void				DailyReset();
	void				SetOwner(LC_ServerPlayer* pkPlayer) { m_pkOwner = pkPlayer; }
	LC_ServerPlayer*	GetOwner() { return m_pkOwner; }
	void				AddResource(int32_t iTypeID, int32_t iSrcType, uint64_t iCount);
	LC_ResourceRecord&	GetRecord(int32_t iUniqueID, int32_t iSrcType);
	void				OnResourceAlert(int32_t iTypeID, int64_t iLimit);

	void				Load(const char_data_define::resource_alert& info);
	void				Save(char_data_define::resource_alert& info);

private:
	uint32_t								m_iLastUpdateTime;
	mem::map<int32_t, LC_ResourceStat>		m_kStats;
	mem::map<int64_t, LC_ResourceRecord>	m_kRecourds;

	LC_ServerPlayer*						m_pkOwner;
};

enum ALERT_CONFIG_OPT_TYPE
{
	ALERT_UPDATE = 0,
	ALERT_DELETE = 1,
};

struct alert_config
{
public:
	alert_config()
		: opt_type(0), type_id(0), base_limit(0) , level_coef(0), vip_coef(0) {}

	bool Init(CSVFile::CF_ResourceAlert::DataEntry* pkEntry);

	inline int64_t GetResourceLimit(int32_t iLevel, int32_t iVipLvl)
	{ 
		return base_limit + (int64_t)(iLevel * level_coef) + (int64_t)(iVipLvl * vip_coef); 
	}	

public:
	int32_t		opt_type;
	int32_t		type_id;
	int64_t		base_limit;
	float		level_coef;
	float		vip_coef;
};
AJSON(alert_config, (opt_type)(type_id)(base_limit)(level_coef)(vip_coef));

class LC_ResourceAlertManager : public Memory::MM_BaseObject,
	public Utility::UT_Singleton<LC_ResourceAlertManager>
{
public:
	LC_ResourceAlertManager() {}

	void Init(void);
	void Unit(void);

	int64_t		GetResourceLimit(int32_t iTypeID, int32_t iLevel, int32_t iVipLvl);
	StringType	GetAlertConfigs();
	StringType	UpdateAlertConfig(const StringType& sConfig);

private:
	mem::map<int32_t, alert_config> m_kAlertConfigs;
};
