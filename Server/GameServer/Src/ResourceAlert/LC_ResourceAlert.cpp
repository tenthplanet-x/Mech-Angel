#include "GameLogicPCH.h"

#include "LC_ResourceAlert.h"
#include "LC_ServerPlayer.h"
#include "UT_TimeManager.h"
#include "oss_define.h"

void LC_ResourceAlert::Init()
{
	uint32_t timestamp = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	if (!LC_Helper::CheckIsSameDay(timestamp, m_iLastUpdateTime))
	{
		DailyReset();
	}
}

void LC_ResourceAlert::Clear()
{
	m_iLastUpdateTime = 0;
	m_kStats.clear();
	m_kRecourds.clear();
}

void LC_ResourceAlert::DailyReset()
{
	if (NULL == m_pkOwner)
	{
		return;
	}
	LC_ResourceAlertManager* pkAlertMgr = LC_ResourceAlertManager::GetSingletonPtr();
	if (NULL == pkAlertMgr)
	{
		return;
	}
	mem::map<int32_t, LC_ResourceStat>::iterator it = m_kStats.begin();
	for (; it != m_kStats.end(); ++it)
	{
		LC_ResourceStat& kStat = it->second;
		int64_t iLimit = pkAlertMgr->GetResourceLimit(
			it->first, m_pkOwner->GetLevel(), m_pkOwner->GetVIPLevel()); 
		if (iLimit < 0)
		{
			continue;
		}
		if (kStat.GetCount() >= (uint64_t)iLimit)
		{
			OnResourceAlert(it->first, iLimit);
		}
	}

	m_iLastUpdateTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	m_kStats.clear();
	m_kRecourds.clear();
}

void LC_ResourceAlert::AddResource(int32_t iTypeID, int32_t iSrcType, uint64_t iCount)
{
	if (NULL == m_pkOwner)
	{
		return;
	}
	LC_ResourceAlertManager* pkAlertMgr = LC_ResourceAlertManager::GetSingletonPtr();
	if (NULL == pkAlertMgr)
	{
		return;
	}
	int64_t iLimit = pkAlertMgr->GetResourceLimit(
		iTypeID, m_pkOwner->GetLevel(), m_pkOwner->GetVIPLevel());
	if (iLimit < 0)
	{
		return;
	}

	LC_ResourceRecord& kRecord = GetRecord(iTypeID, iSrcType);
	kRecord.AddCount(iCount);
	LC_ResourceStat& kStat = m_kStats[iTypeID];
	kStat.AddCount(iCount);
	m_iLastUpdateTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();

	if (kStat.GetCount() >= (uint64_t)iLimit)
	{
		OnResourceAlert(iTypeID, iLimit);
	}
}

LC_ResourceRecord& LC_ResourceAlert::GetRecord(int32_t iTypeID, int32_t iSrcType)
{
	int64_t iUniqueID = SpliceTwoInt32(iTypeID, iSrcType);
	LC_ResourceRecord& kRecord = m_kRecourds[iUniqueID];
	kRecord.SetTypeID(iTypeID);
	kRecord.SetSrcType(iSrcType);
	return kRecord;
}

void LC_ResourceAlert::OnResourceAlert(int32_t iTypeID, int64_t iLimit)
{
	if (NULL == m_pkOwner)
	{
		return;
	}
	LC_ResourceStat& kStat = m_kStats[iTypeID];
	if (kStat.GetCount() <= 0)
	{
		return;
	}
	if (kStat.IsRecord())
	{
		return;
	}
	int64_t iFirstUID = SpliceTwoInt32(iTypeID, 0);
	mem::map<int64_t, LC_ResourceRecord>::iterator it = m_kRecourds.lower_bound(iFirstUID);
	if (it == m_kRecourds.end())
	{
		return;
	}
	StringStreamType ss;
	for (; it != m_kRecourds.end(); ++it)
	{
		LC_ResourceRecord& kRecord = it->second;
		if (kRecord.GetTypeID() != iTypeID)
		{
			break;
		}
		ss << kRecord.GetSrcType() << ':' << kRecord.GetCount() << ';';
	}
	StringType str = ss.str();
	if (str.size() > 0)
	{
		str.resize(str.size() - 1);
	}
	else
	{
		return;
	}
	oss_resource_alert(m_pkOwner, iTypeID, kStat.GetCount(), iLimit, m_iLastUpdateTime, str);
	kStat.OnRecord();
}

void LC_ResourceAlert::Load(const char_data_define::resource_alert& info)
{
	if (NULL == m_pkOwner)
	{
		return;
	}
	LC_ResourceAlertManager* pkAlertMgr = LC_ResourceAlertManager::GetSingletonPtr();
	if (NULL == pkAlertMgr)
	{
		return;
	}

	m_kRecourds.clear();
	m_kStats.clear();
	m_iLastUpdateTime = info.last_update_time();
	for (int i = 0; i < info.records_size(); ++i)
	{
		LC_ResourceRecord kRecord;
		kRecord.Load(info.records(i));
		int64_t iLimit = pkAlertMgr->GetResourceLimit(kRecord.GetTypeID(), m_pkOwner->GetLevel(), m_pkOwner->GetVIPLevel());
		if (iLimit < 0)
		{
			continue;
		}
		m_kRecourds[kRecord.GetUniqueID()] = kRecord;
		LC_ResourceStat& kStat = m_kStats[kRecord.GetTypeID()];
		kStat.AddCount(kRecord.GetCount());
		if (kStat.GetCount() >= (uint64_t)iLimit)
		{
			kStat.OnRecord();
		}
	}
}

void LC_ResourceAlert::Save(char_data_define::resource_alert& info)
{
	info.set_last_update_time(m_iLastUpdateTime);
	mem::map<int64_t, LC_ResourceRecord>::iterator it = m_kRecourds.begin();
	for (; it != m_kRecourds.end(); ++it)
	{
		char_data_define::resource_record* record = info.add_records();
		it->second.Save(*record);
	}
}

bool alert_config::Init(CSVFile::CF_ResourceAlert::DataEntry* pkEntry)
{
	if (NULL == pkEntry)
	{
		return false;
	}

	type_id = pkEntry->_iTypeID;
	base_limit = pkEntry->_iBaseLimit;
	level_coef = pkEntry->_fLevelCoef;
	vip_coef = pkEntry->_fVipCoef;
	return true;
}

void LC_ResourceAlertManager::Init()
{
	CF_ResourceAlert* inst = Utility::SafeGetSingleton<CF_ResourceAlert>();
	CF_ResourceAlert::DataEntryMapExternIterator i = inst->GetIterator();
	while (i.HasMoreElements())
	{
		CF_ResourceAlert::DataEntry* entry = i.PeekNextValuePtr();

		alert_config kEntry;
		if (kEntry.Init(entry))
		{
			m_kAlertConfigs[kEntry.type_id] = kEntry;
		}
		i.MoveNext();
	}
}

void LC_ResourceAlertManager::Unit()
{
	m_kAlertConfigs.clear();
}

int64_t LC_ResourceAlertManager::GetResourceLimit(int32_t iTypeID, int32_t iLevel, int32_t iVipLvl)
{
	mem::map<int32_t, alert_config>::iterator it = m_kAlertConfigs.find(iTypeID);
	if (it == m_kAlertConfigs.end())
	{
		return -1;
	}
	return it->second.GetResourceLimit(iLevel, iVipLvl);
}

struct alert_config_vec
{
	mem::vector<alert_config> configs;
};
AJSON(alert_config_vec, (configs));

StringType LC_ResourceAlertManager::GetAlertConfigs()
{
	if (m_kAlertConfigs.size() <= 0)
	{
		return "[]";
	}
	alert_config_vec alert_configs;
	mem::map<int32_t, alert_config>::iterator it = m_kAlertConfigs.begin();
	for (; it != m_kAlertConfigs.end(); ++it)
	{
		alert_configs.configs.push_back(it->second);
	}
	StringStreamType ss;
	boost::ajson::save_to_buff_(alert_configs, ss);

	return ss.str();
}

StringType LC_ResourceAlertManager::UpdateAlertConfig(const StringType& sConfig)
{
	alert_config_vec alert_configs;
	StringType sErrorMsg;
	if (!boost::ajson::load_from_buff(alert_configs, sConfig.c_str(), sErrorMsg))
	{
		return sErrorMsg;
	}
	for (size_t i = 0; i < alert_configs.configs.size(); ++i)
	{
		alert_config& config = alert_configs.configs.at(i);
		if (config.opt_type == ALERT_UPDATE)
		{
			m_kAlertConfigs[config.type_id] = config;
		}
		else if (config.opt_type == ALERT_DELETE)
		{
			m_kAlertConfigs.erase(config.type_id);
		}
	}
	return "";
}
