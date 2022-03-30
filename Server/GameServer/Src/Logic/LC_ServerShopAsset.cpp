#include "LC_ServerShopAsset.h"
#include "UT_RedisChannel.h"
#include "GameServerApp.h"
#include "UT_Timer.h"
#include "UT_ServerHelper.h"

LC_ServerShopManager::LC_ServerShopManager():LC_CommonShopManager(), m_kGlobalShopAsset(), m_lastTimeStramp(0), m_lastDirtyTime(0), m_noticeIdList()
{
	
}

LC_ServerShopManager::~LC_ServerShopManager()
{

}

bool LC_ServerShopManager::Init(CSVFile::CSVDataLoader* pkLoader, const CSVFile::CSVDataSourceList* pkCSVs)
{
	if(!LC_CommonShopManager::Init(pkLoader, pkCSVs))
	{
		return false;
	}
	m_kGlobalShopAsset.SetGlobal(true);
	m_lastDirtyTime = m_lastTimeStramp = (uint32_t)GET_PROCESS_TIME;

	m_noticeIdList.clear();
	for(LC_ShopDataEntryMap::iterator iter = m_kShopDataEntryMap.begin(); iter != m_kShopDataEntryMap.end(); iter++)
	{
		LC_ShopDataEntry& rEntry = iter->second;
		if(rEntry.m_nNoticeID > 0)
		{
			m_noticeIdList.push_back(rEntry.m_iID);
		}
	}
	return true;
}

bool LC_ServerShopManager::Unit()
{
	LC_CommonShopManager::Unit();
	if(m_kGlobalShopAsset.GetDirtyFlag() == true)
	{
		saveGlobalLimitFromRedis();
	}
	m_kGlobalShopAsset.Clear();
	return true;
}

bool LC_ServerShopManager::loadGlobalLimitFromRedis()
{
	int rslt = 0;
	std::string info;
	Utility::Redis_Handler* pChannel = GetGameServerApp()->GetRedisHandler();
	UT_REDIS_CMD_WITH_REPLY(rslt, pChannel, info, hget, GetGameServerApp()->GetGameGlobalShopKey(), "buyassert");

	//std::string err_msg;
	if (rslt == 0 && info.empty() == false)
	{
		char_data_define::char_common_shop_info protoInfo;
		protoInfo.ParseFromString(info);
		m_kGlobalShopAsset.GetDataFromPB(protoInfo);
		m_kGlobalShopAsset.SetGlobal(true);
	}
	m_lastDirtyTime = m_lastTimeStramp = (uint32_t)GET_PROCESS_TIME;
	CheckReset(GET_CURRENT_TIMESTAMP_IN_SECONDS());
	return true;
}

void onsaveGlobalInfo(int32_t err, bool& ret)
{
	if (err)
	{
		GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "onsaveGlobalInfo", err, ret);
	}
}

bool LC_ServerShopManager::saveGlobalLimitFromRedis()
{
	if(m_kGlobalShopAsset.GetDirtyFlag() == false)
	{
		return false;
	}
	char_data_define::char_common_shop_info protoInfo;
	m_kGlobalShopAsset.SetDataToPB(protoInfo);
	std::string saveInfo = "";
	protoInfo.SerializeToString(&saveInfo);

	ResultType rslt = RE_SUCCESS;
	Utility::Redis_Channel& channel = GetGameServerApp()->GetRedisChannel();
	ASYNC_REDIS_REQUEST(rslt, channel, &redispp::Connection::hset, onsaveGlobalInfo, GetGameServerApp()->GetGameGlobalShopKey(), "buyassert", saveInfo);
	
	if(rslt == RE_SUCCESS)
	{
		m_kGlobalShopAsset.ClearDirtyFlag();
		return true;
	}
	return false;
}

void LC_ServerShopManager::CheckReset(uint32_t timestamp)
{
	m_kGlobalShopAsset.CheckReset(timestamp);
}

ResultType LC_ServerShopManager::CheckShop(LC_PlayerBase* pkPlayer, const LC_ShopIDMap& kShopIDMap)
{
	//全局商店  暂作限制
	if (m_kGlobalShopAsset.IsGlobalLimit(kShopIDMap) && UT_ServerHelper::IsInCross((LC_ServerPlayer*)pkPlayer))
	{
		return RE_ON_CROSS_SERVER;
	}
	return m_kGlobalShopAsset.CheckShop(pkPlayer, kShopIDMap);
}

void LC_ServerShopManager::RegShopping(const LC_ShopIDMap& kShopIDMap)
{
	m_kGlobalShopAsset.RegShopping(kShopIDMap);
}

LC_CommonShopStat* LC_ServerShopManager::GetShopStat(shop_id_type iShopID, int32_t iStatType)
{
	LC_CommonShopEntry* pkCommonShopEntry = m_kGlobalShopAsset.GetShopEntry(iShopID);
	if (NULL == pkCommonShopEntry)
	{
		return NULL;
	}
	return pkCommonShopEntry->GetStat(iStatType);
}

bool LC_ServerShopManager::IsTimeViable(uint64_t nNowTime, uint64_t nStartTime, uint64_t nMergeTime, LC_ShopDataEntry* pEntry)
{
	if(pEntry == NULL)
	{
		return false;
	}
	return LC_Helper::IsCommTimeViable(nNowTime, nStartTime, nMergeTime, pEntry->m_beginTime, pEntry->m_endTime, 
											pEntry->m_sColdTime, pEntry->m_sBeginTime, pEntry->m_sEndTime, 
											pEntry->m_mColdTime, pEntry->m_mBeginTime, pEntry->m_mEndTime);
}

void LC_ServerShopManager::Tick(uint32_t ntimeStramp)
{
	if(m_lastTimeStramp <= 0)
	{
		return;
	}

	if(ntimeStramp >= m_lastTimeStramp + 2)			//2秒更新一次
	{
		m_lastTimeStramp = ntimeStramp;

		if(m_kGlobalShopAsset.GetDirtyFlag() == true)
		{
			saveGlobalLimitFromRedis();
			m_lastDirtyTime = ntimeStramp;
		}

		uint64_t nNowTime = (uint64_t)GET_CURRENT_TIMESTAMP_IN_SECONDS();
		uint64_t nStartTime = (uint64_t)LC_Helper::GetDayTimeStamp(GetGameServerApp()->GetServerManager()->GetServerStartTime(), 0, 0, 0);
		uint64_t nMergeTime = (uint64_t)LC_Helper::GetDayTimeStamp(GetGameServerApp()->GetServerManager()->GetServerConflateTime(), 0, 0, 0);

		mem::set<int32_t> rNoticeSet;
		for(mem::list<int32_t>::iterator iter = m_noticeIdList.begin(); iter != m_noticeIdList.end(); )
		{
			int32_t nNoticeId = *iter;
			LC_ShopDataEntryMap::iterator itEntry = m_kShopDataEntryMap.find(nNoticeId);
			if(itEntry != m_kShopDataEntryMap.end())
			{
				LC_ShopDataEntry& rEntry = itEntry->second;
				if(IsTimeViable(nNowTime, nStartTime, nMergeTime, &rEntry) == true)
				{
					rNoticeSet.insert(rEntry.m_nNoticeID);
					m_noticeIdList.erase(iter++);
					continue;
				}
			}
			iter++;
		}
		
		UT_ServerHelper::_GameNoticeFmort noticeFmort;
		for(mem::set<int32_t>::iterator iter = rNoticeSet.begin(); iter != rNoticeSet.end(); iter ++)
		{
			noticeFmort.SetMsgID(*iter);
			UT_ServerHelper::BrocastNoticeMsg(noticeFmort);
		}
	}
}