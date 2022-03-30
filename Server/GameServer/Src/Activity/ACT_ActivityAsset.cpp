#include "ACT_ActivityAsset.h"
#include "ACT_ActivitySchedule.h"

#include "LC_Helper.h"
#include "LC_ServerPlayer.h"

using namespace CSVFile;
using namespace GameLogic;
using namespace Activity;

void ACT_ScheduleSaveCommon::Clear(int32_t iBegin, int32_t iEnd)
{
	for (int32_t i=iBegin; i<iEnd; ++i)
	{
		m_kDatas.erase(i);
	}
}
void ACT_ScheduleSaveCommon::ClearEx(int32_t iBegin, int32_t iEnd)
{
	ACT_DataMap::iterator it = m_kDatas.begin();
	for (; it!=m_kDatas.end(); ++it)
	{
		if (it->first>=iBegin && it->first<iEnd)
		{
			it->second = 0;
		}
	}
}
void ACT_ScheduleSaveCommon::GetDataFromPB(const char_data_define::activity_schedule_common& info)
{
	m_iID  = info.id();;
	for (int i=0; i<info.datas_size()/2; ++i)
	{
		m_kDatas[(int32_t)info.datas(i<<1)] = info.datas((i<<1)+1);
	}
}
void ACT_ScheduleSaveCommon::SetDataToPB(char_data_define::activity_schedule_common& info)
{
	info.set_id(m_iID);
	ACT_DataMap::iterator it = m_kDatas.begin();
	for (; it!=m_kDatas.end(); ++it)
	{
		if (it->second)
		{
			info.add_datas(it->first);
			info.add_datas(it->second);
		}
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

void ACT_ScheduleSaveEntry::Clear(uint32_t timestamp)
{
	m_uiTimestamp = timestamp;
	m_kSelf.Clear();
	m_kDetails.clear();
	UpdateDirtyFlag();
}

bool ACT_ScheduleSaveEntry::CheckReset(ACT_ScheduleEntry* pkSchedule, uint32_t timestamp)
{
	if (pkSchedule && pkSchedule->GetActive() && pkSchedule->IsTimestampIn((int64_t)(timestamp)*ONE_SECOND_MILI))
	{
		if (!pkSchedule->IsTimestampIn((int64_t)(m_uiTimestamp)*ONE_SECOND_MILI))
		{
			Clear(timestamp);
			return true;
		}
		else
		{
			do 
			{
				if (LC_Helper::CheckIsSameDay(timestamp, m_uiTimestamp))
				{
					return false;
					break;
				}
				_clearData(LC_ST_TP_DAILY, timestamp);

				if (LC_Helper::CheckIsSameWeek(timestamp, m_uiTimestamp))
				{
					break;
				}
				_clearData(LC_ST_TP_WEEK, timestamp);

				if (LC_Helper::CheckIsSameMonth(timestamp, m_uiTimestamp))
				{
					break;
				}
				_clearData(LC_ST_TP_MONTH, timestamp);
			} while (0);
			return true;
		}
	}
	return false;
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
	UpdateDirtyFlag();
}
void ACT_ScheduleSaveEntry::SetDataToPB(char_data_define::activity_schedule_entry& info, bool all)
{
	info.set_timestamp(m_uiTimestamp);
	m_kSelf.SetDataToPB(*info.mutable_self());
	ACT_ScheduleSaveCommonMap::iterator it = m_kDetails.begin();
	for (; it!=m_kDetails.end(); ++it)
	{
		ACT_ScheduleSaveCommon& rkDetail = it->second;
		char_data_define::activity_schedule_common* pk_detail = info.add_details();
		if (pk_detail)
		{
			rkDetail.SetDataToPB(*pk_detail);
		}
	}
}
ACT_ScheduleSaveCommon* ACT_ScheduleSaveEntry::_addScheduleSaveDetail(int32_t iDetail)
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
void ACT_ScheduleSaveEntry::_clearData(int32_t iStatType, uint32_t timestamp)
{
	m_kSelf.ClearEx(GET_SCHEDULE_KEY(iStatType, ACT_SCHEDULE_BEGIN), GET_SCHEDULE_KEY(iStatType, ACT_SCHEDULE_END));
	int32_t iBegin = GET_SCHEDULE_KEY(iStatType, ACT_SCHEDULE_DETAIL_BEGIN);
	int32_t iEnd = GET_SCHEDULE_KEY(iStatType, ACT_SCHEDULE_DETAIL_END);
	ACT_ScheduleSaveCommonMap::iterator it = m_kDetails.begin();
	for (; it!=m_kDetails.end(); ++it)
	{
		ACT_ScheduleSaveCommon& rkDetail = it->second;
		rkDetail.Clear(iBegin, iEnd);
	}	
	m_uiTimestamp = timestamp;
	UpdateDirtyFlag();
}

void ACT_ScheduleSaveCatagory::ForceDirty(void)
{
	ACT_ScheduleSaveEntryMap::iterator it = m_kSchedules.begin();
	for (; it!=m_kSchedules.end(); ++it)
	{
		ACT_ScheduleSaveEntry& rkScheduleSave = it->second;
		rkScheduleSave.UpdateDirtyFlag();
	}
}

bool ACT_ScheduleSaveCatagory::CheckReset(LC_ServerPlayer* pkPlayer, uint32_t timestamp)
{
	IF_A_NA_PLAYER(pkPlayer){return false;}
	
	bool ret = false;
	static ACT_ScheduleManager* pkMgr = ACT_ScheduleManager::GetSingletonPtr();

	GameLogic::LC_IDVector kUnvalidIDs;
	{
		ACT_ScheduleSaveEntryMap::iterator it = m_kSchedules.begin();
		for (; it!=m_kSchedules.end(); ++it)
		{
			ACT_ScheduleSaveEntry& rkScheduleSave = it->second;
			ACT_ScheduleEntry* pkSchedule = pkMgr->GetSchedule(pkPlayer->GetServerID(), it->first);
			if (pkSchedule)
			{
				if(rkScheduleSave.CheckReset(pkSchedule, timestamp))
				{
					UpdateDirtyFlag();
					ret = true;
				}
			}
			else
			{
				kUnvalidIDs.push_back(it->first);
			}
		}
	}

	{
		GameLogic::LC_IDVector::iterator it = kUnvalidIDs.begin();
		for (; it!=kUnvalidIDs.end(); ++it)
		{
			m_kSchedules.erase(*it);
		}
	}
	return ret;
}
bool ACT_ScheduleSaveCatagory::CheckReset(ACT_ScheduleEntry* pkSchedule, uint32_t timestamp)
{
	if (NULL == pkSchedule)
	{
		return false;
	}
	ACT_ScheduleSaveEntry* pkScheduleSave = GetScheduleSaveEntry(pkSchedule->m_iID);	
	if (pkScheduleSave && pkScheduleSave->CheckReset(pkSchedule, timestamp))
	{
		UpdateDirtyFlag();
		return true;
	}
	return false;
}

void ACT_ScheduleSaveCatagory::ClearScheuleSaveEntry(int32_t iID)
{
	ACT_ScheduleSaveEntryMap::iterator it = m_kSchedules.find(iID);
	if(it != m_kSchedules.end())
	{
		m_kSchedules.erase(it);
	}
}
void ACT_ScheduleSaveCatagory::GetDataFromPB(const char_data_define::activity_schedule_catagory& info)
{
	Clear();
	m_iID = info.id();
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
void ACT_ScheduleSaveCatagory::SetDataToPB(char_data_define::activity_schedule_catagory& info, bool all)
{
	info.set_id(m_iID);
	{
		ACT_ScheduleSaveEntryMap::iterator it = m_kSchedules.begin();
		for (; it!=m_kSchedules.end(); ++it)
		{
			ACT_ScheduleSaveEntry& rkEntry = it->second;
			if (!all && !rkEntry.GetDirtyFlag())
			{
				continue;
			}
			char_data_define::activity_schedule_entry* pkentry = info.add_schedules();
			if (pkentry)
			{
				rkEntry.SetDataToPB(*pkentry, all);
				if (!all)
				{
					rkEntry.SetDirtyFlag(false);
				}
			}
		}
	}
}
ACT_ScheduleSaveEntry* ACT_ScheduleSaveCatagory::_addScheduleSaveEntry(int32_t iID)
{
	ACT_ScheduleSaveEntryMap::iterator it = m_kSchedules.find(iID);
	if (it != m_kSchedules.end())
	{
		return &it->second;
	}

	ACT_ScheduleSaveEntry& rkEntry = m_kSchedules[iID];
	rkEntry.Init(iID, GET_CURRENT_TIMESTAMP_IN_SECONDS());
	return &rkEntry;
}

void ACT_ScheduleSaveActivity::ForceDirty(void)
{
	ACT_ScheduleSaveCatagoryMap::iterator it = m_kCatagorys.begin();
	for (; it!=m_kCatagorys.end(); ++it)
	{
		ACT_ScheduleSaveCatagory& rkSaveCata = it->second;
		rkSaveCata.ForceDirty();
	}
}

bool ACT_ScheduleSaveActivity::CheckReset(LC_ServerPlayer* pkPlayer, uint32_t timestamp)
{
	bool ret = false;
	ACT_ScheduleSaveCatagoryMap::iterator it = m_kCatagorys.begin();
	for (; it!=m_kCatagorys.end(); ++it)
	{
		ACT_ScheduleSaveCatagory& rkSaveCata = it->second;
		if (rkSaveCata.CheckReset(pkPlayer, timestamp))
		{
			UpdateDirtyFlag();
			ret = true; 
		}
	}
	return ret;
}
bool ACT_ScheduleSaveActivity::CheckReset(ACT_ScheduleEntry* pkSchedule, uint32_t timestamp, const GameLogic::LC_IDSet& rkIndices)
{
    bool bResult = false;
	if (pkSchedule)
	{
		GameLogic::LC_IDSet::const_iterator cit = rkIndices.begin();
		for (; cit!=rkIndices.end(); ++cit)
		{
			ACT_ScheduleSaveCatagory* pkSaveCata = GetScheduleSaveCatagory(*cit);
			if (pkSaveCata && pkSaveCata->CheckReset(pkSchedule, timestamp))
			{
				UpdateDirtyFlag();
				bResult = true;
			}
		}
	}
	return bResult;
}

bool ACT_ScheduleSaveActivity::ClearActData(uint32_t nScheduleId, const GameLogic::LC_IDSet& rkIndices)
{
	bool bResult = false;

	GameLogic::LC_IDSet::const_iterator cit = rkIndices.begin();
	for (; cit!=rkIndices.end(); ++cit)
	{
		ACT_ScheduleSaveCatagory* pkSaveCata = GetScheduleSaveCatagory(*cit);
		if (pkSaveCata)
		{
			pkSaveCata->ClearScheuleSaveEntry(nScheduleId);
			UpdateDirtyFlag();
			bResult = true;
		}
	}
	return bResult;
}

void ACT_ScheduleSaveActivity::GetDataFromPB(const char_data_define::activity_schedule_info& info)
{
	Clear();
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
void ACT_ScheduleSaveActivity::SetDataToPB(char_data_define::activity_schedule_info& info, bool all)
{
	info.set_type(m_iType);
	ACT_ScheduleSaveCatagoryMap::iterator it = m_kCatagorys.begin();
	for (; it!=m_kCatagorys.end(); ++it)
	{
		ACT_ScheduleSaveCatagory& rkEntry = it->second;
		if (!all && !rkEntry.GetDirtyFlag())
		{
			continue;
		}
		char_data_define::activity_schedule_catagory* pkentry = info.add_catagorys();
		if (pkentry)
		{
			rkEntry.SetDataToPB(*pkentry, all);
			if (!all)
			{
				rkEntry.SetDirtyFlag(false);
			}
		}
	}
	if (!m_kPersistentInfo.empty())
	{
		ACT_SchedulePersistentInfoMap::iterator iter = m_kPersistentInfo.begin();
		for (; iter != m_kPersistentInfo.end(); ++iter)
		{
			char_data_define::activity_schedule_persistent_info* pkPersistentEntry = info.add_persistent_info();
			if (NULL != pkPersistentEntry)
			{
				pkPersistentEntry->set_id(iter->first);
				pkPersistentEntry->set_value(iter->second);
			}
		}
	}
}

ACT_ScheduleSaveCatagory* ACT_ScheduleSaveActivity::_addScheduleSaveCatagory(int32_t iCatagory)
{
	ACT_ScheduleSaveCatagoryMap::iterator it = m_kCatagorys.find(iCatagory);
	if (it != m_kCatagorys.end())
	{
		return &it->second;
	}

	ACT_ScheduleSaveCatagory& rkEntry = m_kCatagorys[iCatagory];
	rkEntry.m_iID = iCatagory;
	return &rkEntry;
}

void ACT_ScheduleAsset::ForceDirty(void)
{
	ACT_ScheduleSaveActivityMap::iterator it = m_kActivities.begin();
	for (; it!=m_kActivities.end(); ++it)
	{
		ACT_ScheduleSaveActivity& rkActivity = it->second;
		rkActivity.ForceDirty();
	}
}

void ACT_ScheduleAsset::CheckReset(uint32_t timestamp)
{
	ACT_ScheduleSaveActivityMap::iterator it = m_kActivities.begin();
	for (; it!=m_kActivities.end(); ++it)
	{
		ACT_ScheduleSaveActivity& rkActivity = it->second;
		if(rkActivity.CheckReset(m_pkOwner, timestamp))
		{
			UpdateDirtyFlag();
		}
	}
}

void ACT_ScheduleAsset::GetDataFromPB(const char_data_define::char_activity_info& info)
{
	Clear();
	for (int i=0; i<info.activities_size(); ++i)
	{
		const char_data_define::activity_schedule_info& rkentry = info.activities(i);
		ACT_ScheduleSaveActivity* pkEntry = _addScheduleSaveActivity(rkentry.type());
		if (pkEntry)
		{
			pkEntry->GetDataFromPB(rkentry);
		}
	}
	UpdateDirtyFlag();
}
bool ACT_ScheduleAsset::SetDataToPB(char_data_define::char_activity_info& info, bool all)
{
	static size_t _max_pb_msg_sz = (size_t)Protocol::MAX_S2C_MSG_BODY_SIZE / 2;

	ACT_ScheduleSaveActivityMap::iterator it = m_kActivities.begin();
	for (; it!=m_kActivities.end(); ++it)
	{
		ACT_ScheduleSaveActivity& rkEntry = it->second;
		if (!all && !rkEntry.GetDirtyFlag())
		{
			continue;
		}
		char_data_define::activity_schedule_info* pkentry = info.add_activities();
		if (pkentry)
		{
			rkEntry.SetDataToPB(*pkentry, all);
			if (!all)
			{
				rkEntry.SetDirtyFlag(false);
			}

			if (!all && info.ByteSize() >= _max_pb_msg_sz)
			{
				return true;
			}
		}
	}
	return false;
}

ACT_ScheduleSaveActivity* ACT_ScheduleAsset::_addScheduleSaveActivity(int32_t iType)
{
	ACT_ScheduleSaveActivityMap::iterator it = m_kActivities.find(iType);
	if (it != m_kActivities.end())
	{
		return &it->second;
	}

	ACT_ScheduleSaveActivity& rkEntry = m_kActivities[iType];
	rkEntry.Init(iType);
	return &rkEntry;
}
