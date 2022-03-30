#include "LC_Rumor.h"
#include "MG_ChatProtocol.h"
#include "NW_Helper.h"
#include "LC_ItemFactoryBase.h"
#include "LC_ItemBase.h"
#include "SK_Define.h"
#include "LC_RumorManager.h"

#include "GameServerApp.h"
#include "UT_ServerHelper.h"

GameLogic::LC_Rumor::LC_Rumor(): m_iRumorType(NoRumor), m_iTriggerType(NoTriggerType) {}

void GameLogic::LC_Rumor::SetUP(void)
{
	{
		LC_RumorDataEntryMap::iterator it = m_kRumerEntryMap.begin();
		for (; it != m_kRumerEntryMap.end(); ++it)
		{
			const LC_RumorKey& kKey = it->first;
			m_kKeyIndices[kKey[0]].push_back(kKey);
		}
	}
	{
		RumorKeyIndices::iterator it = m_kKeyIndices.begin();
		for (; it != m_kKeyIndices.end(); ++it)
		{
			std::sort(it->second.begin(), it->second.end());
		}
	}
}

void GameLogic::LC_Rumor::SetRumorType(RumorType type)
{
	m_iRumorType = type;
}


void GameLogic::LC_Rumor::SetChatChannel(int iChatChannel)
{
	m_iChatChannel = iChatChannel;
}

void GameLogic::LC_Rumor::SetTriggerType(RumorTriggerType type)
{
	m_iTriggerType = type;
}

const GameLogic::LC_RumorDataEntry* GameLogic::LC_Rumor::GetRumerEntry(int32_t iKey, int32_t iDetail)
{
	LC_RumorKeyVector& kKeys = m_kKeyIndices[iKey];
	if (kKeys.empty())
	{
		return NULL;
	}

	LC_RumorKey* pkKey = NULL;
	LC_RumorKeyVector::iterator it = kKeys.begin();
	for (; it != kKeys.end(); ++it)
	{
		if (it->operator[](1) <= iDetail)
		{
			pkKey = &*it;
		}
		else
		{
			break;
		}
	}
	return pkKey ? GetRumerEntry(*pkKey) : NULL;
}
const LC_RumorDataEntry* GameLogic::LC_Rumor::GetRumerEntryBetween(int32_t iKey, int32_t iDetailMin, int32_t iDetailMax)
{
	LC_RumorKeyVector& kKeys = m_kKeyIndices[iKey];
	if (kKeys.empty())
	{
		return NULL;
	}

	LC_RumorKey* pkKey = NULL;
	LC_RumorKeyVector::iterator it = kKeys.begin();
	for (; it != kKeys.end(); ++it)
	{
		int val = it->operator[](1);
		if (val > iDetailMax)
		{
			break;
		}
		if (val > iDetailMin /*&& val <= iDetailMax*/)
		{
			pkKey = &*it;
		}
	}
	return pkKey ? GetRumerEntry(*pkKey) : NULL;
}
uint32_t GameLogic::LC_Rumor::GetRumorLimit(int32_t iGrade)
{
	if (iGrade<0 || iGrade>=INF_SPH_TP_MAX)
	{
		return 0;
	}
	return m_kRecord.uiMaxCount[iGrade];
}

int  GameLogic::LC_Rumor::GetRumorID(const LC_RumorKey& key)
{
	char buff[1024] = {0};
	const LC_RumorDataEntry* pkRumerEntry = GetRumerEntry(key);
	if (NULL != pkRumerEntry)
	{
		return pkRumerEntry->noticeID;
	}

	return 0;
}

void GameLogic::LC_RumorLog::SetDataToProtoclStruct(Protocol::PS_RumorLog& rkStruct)
{
	rkStruct.m_uiTimestamp = m_uiTimestamp;
	rkStruct.m_sInfo = m_sInfo;
}

void GameLogic::LC_RumorLog::GetDataFromProtocolStruct(Protocol::PS_RumorLog& rkStruct)
{
	m_uiTimestamp = rkStruct.m_uiTimestamp;
	m_sInfo = rkStruct.m_sInfo;
}

bool GameLogic::LC_RumorLogInfo::AddLog(const LC_RumorLog& kLog)
{
	if (!m_uiLimit)
	{
		return false;
	}

	LC_RumorLogList::iterator it = m_kRumors.begin();
	while (it != m_kRumors.end())
	{
		if (kLog == *it)
		{
			return false;
		}
		if (!(kLog < *it))
		{
			m_kRumors.insert(it, kLog);
			break;
		}
		else
		{
			++it;
		}
	}

	if (it == m_kRumors.end() && m_kRumors.size() >= m_uiLimit)
	{
		return false;
	}
	else if (it == m_kRumors.end())
	{
		m_kRumors.push_back(kLog);
	}
	else if (m_kRumors.size() > m_uiLimit)
	{
		m_kRumors.resize(m_uiLimit);
	}
	UpdateDirtyFlag();
	return true;
}

void GameLogic::LC_RumorLogInfo::SetDataToProtoclStruct(Protocol::PS_RumorLogInfo& rkStruct, bool all)
{
	rkStruct.m_iType = (uint16_t)m_iType;
	LC_RumorLogList::iterator it = m_kRumors.begin();
	for (; it != m_kRumors.end(); ++it)
	{
		if (all || it->GetDirtyFlag())
		{
			Protocol::PS_RumorLog psLog;
			it->SetDataToProtoclStruct(psLog);
			rkStruct.m_kRumors.push_back(psLog);
			it->ClearDirtyFlag();
		}
	}
}

void GameLogic::LC_RumorLogInfo::GetDataFromProtocolStruct(Protocol::PS_RumorLogInfo& rkStruct)
{
	Protocol::PS_RumorLogInfo::PS_RumorLogList::iterator it = rkStruct.m_kRumors.begin();
	for (; it != rkStruct.m_kRumors.end(); ++it)
	{
		LC_RumorLog kLog;
		kLog.GetDataFromProtocolStruct(*it);
		AddLog(kLog);
	}
}

void GameLogic::LC_RumorLogInfo::OnSync(int32_t err, std::vector<std::string>& infos)
{
	if (err)
	{
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "fail to get rumor rang", m_iType);
		return;
	}

	static GameLogic::LC_RumorManager* pkRumorManager = GameLogic::LC_RumorManager::GetSingletonPtr();
	LC_Rumor* pkRumor = pkRumorManager->GetRumorByType(m_iType);
	if (NULL == pkRumor)
	{
		return;
	}
	
	for (size_t i = 0; i + 1 < infos.size();)
	{
		LC_RumorLog kLog;
		kLog.m_sInfo = infos[i++];
		kLog.m_uiTimestamp = (uint32_t)atoll(infos[i++].c_str());
		kLog.UpdateDirtyFlag();
		if(AddLog(kLog))
		{
			switch(m_iType)
			{
			case CommonStat:
				//UT_ServerHelper::BrocastMsgStrServer(STDSTR2TPSTR(kLog.m_sInfo), (PromptChannel)pkRumor->GetChatChannel(),  PROMPT_LEVEL_LOW);
				break;
			default:
				break;
			}
		}
	}
}

void GameLogic::LC_RumorLogInfo::SyncRumor(void)
{
	static GameServerApp* pkApp = GetGameServerApp();
	Utility::Redis_Channel* pkRedisChannel = pkApp->GetRedisChannel(m_iGrade);
	boost::function<void(int32_t, std::vector<std::string>&)> cb = boost::bind(&GameLogic::LC_RumorLogInfo::OnSync, this, _1, _2);
	std::string skey = pkApp->GetRedisPrefix(m_iGrade) + ":" + boost::lexical_cast<std::string>(m_iType);
	int ret = 0;
	ASYNC_REDIS_REQUEST(ret, (*pkRedisChannel), &redispp::Connection::zrevrange, cb, skey, 0, m_uiLimit, "WITHSCORES");
}
// 
// bool GameLogic::LC_RumorLogInfo::GetDataFromPB(const redis_define::rumor_info& info, GameLogic::LC_Rumor* pkRumer)
// {
// 	m_iType = info.type();
// 	bool flag = false;
// 	for (int i = 0; i < info.entries_size(); ++i)
// 	{
// 		const redis_define::rumor_entry& kentry = info.entries(i);
// 		LC_RumorLog kLog;
// 		kLog.GetDataFromPB(kentry);
// 		kLog.UpdateDirtyFlag();
// 		flag = AddLog(kLog, pkRumer) || flag;
// 	}
// 	return flag;
// }
// 
// void GameLogic::LC_RumorLogInfo::SetDataToPB(redis_define::rumor_info& info)
// {
// 	info.set_type(m_iType);
// 	LC_RumorLogList::iterator it = m_kRumors.begin();
// 	for (; it != m_kRumors.end(); ++it)
// 	{
// 		redis_define::rumor_entry* pkentry = info.add_entries();
// 		it->SetDataToPB(*pkentry);
// 	}
// }
