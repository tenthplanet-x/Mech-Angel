#include "GameLogicPCH.h"

#include "LC_ServerPlayer.h"
#include "LC_FacebookReward.h"
#include "LC_PackEntryFactory.h"
#include "LC_Define.h"
#include "UT_ServerHelper.h"

using namespace GameLogic;
using namespace CSVFile;

void LC_FacebookRecord::Load(const char_data_define::char_detail_info& info)
{
	m_kDrawCount.clear();
	m_kDrawCountDaily.clear();
	for (int i = 0; i < info.fbrecord_size(); ++i)
	{
		const char_data_define::fbreward_record& kRecord = info.fbrecord(i);
		mem::map<int32_t, int32_t>& kDrawCounts = m_kDrawCount[kRecord.type()];
		kDrawCounts[kRecord.id()] = kRecord.draw_count();
	}
	for (int i = 0; i < info.fbrecord_daily_size(); ++i)
	{
		const char_data_define::fbreward_record& kRecord = info.fbrecord_daily(i);
		mem::map<int32_t, int32_t>& kDrawCounts = m_kDrawCountDaily[kRecord.type()];
		kDrawCounts[kRecord.id()] = kRecord.draw_count();
	}
}

void LC_FacebookRecord::Save(char_data_define::char_detail_info& info)
{
	mem::map<int32_t, mem::map<int32_t, int32_t> >::iterator it = m_kDrawCount.begin();
	for (; it != m_kDrawCount.end(); ++it)
	{
		mem::map<int32_t, int32_t>::iterator sit = it->second.begin();
		for (; sit != it->second.end(); ++sit)
		{
			char_data_define::fbreward_record* kRecord = info.add_fbrecord();
			kRecord->set_type(it->first);
			kRecord->set_id(sit->first);
			kRecord->set_draw_count(sit->second);
		}
	}
	it = m_kDrawCountDaily.begin();
	for (; it != m_kDrawCountDaily.end(); ++it)
	{
		mem::map<int32_t, int32_t>::iterator sit = it->second.begin();
		for (; sit != it->second.end(); ++sit)
		{
			char_data_define::fbreward_record* kRecord = info.add_fbrecord_daily();
			kRecord->set_type(it->first);
			kRecord->set_id(sit->first);
			kRecord->set_draw_count(sit->second);
		}
	}
}

int32_t LC_FacebookRecord::GetDrawCount(int32_t iType, int32_t iID, bool bDaily)
{
	mem::map<int32_t, mem::map<int32_t, int32_t> >* pkTempDrawCount;
	if (!bDaily)
	{
		pkTempDrawCount = &m_kDrawCount;
	}
	else
	{
		pkTempDrawCount = &m_kDrawCountDaily;
	}
	mem::map<int32_t, mem::map<int32_t, int32_t> >::iterator it =
		pkTempDrawCount->find(iType);
	if (it == pkTempDrawCount->end())
	{
		return 0;
	}
	mem::map<int32_t, int32_t>::iterator sit = it->second.find(iID);
	if (sit == it->second.end())
	{
		return 0;
	}
	return sit->second;
}

void LC_FacebookRecord::DrawReward(LC_ServerPlayer* pkPlayer, int32_t iType, int32_t iID, int32_t iCount)
{
	LC_PackEntryFactory* pkPackEntryFactory = LC_PackEntryFactory::GetSingletonPtr();
	if (NULL == pkPackEntryFactory)
	{
		return;
	}
	
	LC_FacebookRewardManager* pkFBRewardMgr = LC_FacebookRewardManager::GetSingletonPtr();
	if (NULL == pkFBRewardMgr)
	{
		return;
	}

	LC_FBRewardEntry* pkEntry = pkFBRewardMgr->GetFBRewardEntry(iType, iID);
	if (NULL == pkEntry)
	{
		return;
	}
	mem::map<int32_t, mem::map<int32_t, int32_t> >* pkTempDrawCount;
	if (pkEntry->_bDaily)
	{
		pkTempDrawCount = &m_kDrawCountDaily;
	}
	else
	{
		pkTempDrawCount = &m_kDrawCount;
	}
	mem::map<int32_t, int32_t>& kDrawCounts = pkTempDrawCount->operator[](iType);
	kDrawCounts[iID] += iCount;

	LC_BackPackEntryList kEntryList;
	Utility::UT_SIMDataList::const_iterator cit = pkEntry->_kRewards.begin();
	for (; cit != pkEntry->_kRewards.end(); ++cit)
	{
		if (pkPackEntryFactory->CreateItemEntry(*cit, CIET_NONE, kEntryList))
		{
			GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "mail item to", pkPlayer->GetInstance(), cit->ToString().c_str());
		}
	}

	if (kEntryList.size() > 0)
	{
		UT_ServerHelper::SendToMail(pkEntry->_sTitle, pkEntry->_sContent, pkPlayer->GetInstance(),
			pkPlayer->GetOwnerCharName(), 0, kEntryList, "3");
	}
}

bool LC_FacebookRecord::CheckCanDraw(int32_t iType, int32_t iID)
{
	LC_FacebookRewardManager* pkFBRewardMgr = LC_FacebookRewardManager::GetSingletonPtr();
	if (NULL == pkFBRewardMgr)
	{
		return false;
	}
	LC_FBRewardEntry* pkEntry = pkFBRewardMgr->GetFBRewardEntry(iType, iID);
	if (NULL == pkEntry)
	{
		return false;
	}
	if (pkEntry->_iCount <= GetDrawCount(iType, iID, pkEntry->_bDaily))
	{
		return false;
	}
	return true;
}

bool LC_FBRewardEntry::Init(CSVFile::CF_FacebookReward::DataEntry* pkEntry)
{
	if (NULL == pkEntry)
	{
		return false;
	}

	_iType = pkEntry->_iType;
	_iID = pkEntry->_iSubID;
	_iCount = pkEntry->_iCount;
	_bDaily = pkEntry->_bDaily;
	_sTitle = pkEntry->_sTitle;
	_sContent = pkEntry->_sContent;
	_kRewards = pkEntry->_kRewards;
	return true;
}

void LC_FacebookRewardManager::Init()
{
	CF_FacebookReward* inst = Utility::SafeGetSingleton<CF_FacebookReward>();
	CF_FacebookReward::DataEntryMapExternIterator i = inst->GetIterator();

	while (i.HasMoreElements())
	{
		CF_FacebookReward::DataEntry* entry = i.PeekNextValuePtr();
		LC_FBRewardEntry lcentry;
		lcentry.Init(entry);
		mem::map<int32_t, LC_FBRewardEntry>& rewards = m_kFBRewardEntryMap[lcentry._iType];
		rewards[lcentry._iID] = lcentry;
		i.MoveNext();
	}
}

void LC_FacebookRewardManager::Unit()
{
	m_kFBRewardEntryMap.clear();
}
