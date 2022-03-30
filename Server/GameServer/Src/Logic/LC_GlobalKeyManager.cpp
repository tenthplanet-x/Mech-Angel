#include "LC_GlobalKeyManager.h"

#include "GameServerApp.h"
#include "UT_Log.h"

#include "boost/lexical_cast.hpp"
#include "boost/function.hpp"

#include "LC_ServerChestCSVManager.h"

using namespace GameLogic;

struct ItemChestOP
{
	ItemChestOP(LC_GlobalKeyManager* mgr):pkMgr(mgr){}

	void operator ()(gen_item_pack& pack)
	{
		if (pkMgr && !pack.global_key.empty())
		{
			pkMgr->AddGlobalKey(LC_GLB_KEY_TP_NUM, pack.global_key.c_str());
		}
	}
	
	LC_GlobalKeyManager* pkMgr;
};

bool LC_GlobalKeyManager::Init()
{
	m_sKeyPrefix = GetGameServerApp()->GetRedisGroupPrefix() + UT_REDIS_KEY_GLOBAL_VALUES;
	
	//LC_ServerChestCSVManager* pkMgr = LC_ServerChestCSVManager::GetSingletonPtr();
	//ItemChestOP kOP(this);
	//pkMgr->ForEach(kOP);

	return true;
}

bool LC_GlobalKeyManager::Unit()
{
	return true;
}

void LC_GlobalKeyManager::Update(float fCurrentTime)
{
	if (fCurrentTime < m_fLastTimeStamp + 30.0f)
	{
		return;
	}

	int32_t iRet = 0;
	Utility::Redis_Channel& channel = GetGameServerApp()->GetRedisChannel();
	boost::function<void(int32_t, std::vector<std::string>&)> cb = boost::bind(&LC_GlobalKeyManager::_updateGlobalKeyValues, this, _1, _2);

	ASYNC_REDIS_REQUEST(iRet, channel, &redispp::Connection::hgetAll, cb, m_sKeyPrefix);

	m_fLastTimeStamp = fCurrentTime;
}
ResultType LC_GlobalKeyManager::GlobalKeyHSet(const std::string& sKey, const std::string& val)
{
	LC_GlobalKeyEntry* pkEntry = _getGlobalKey(sKey);
	if (NULL == pkEntry)
	{
		return RE_FAIL;
	}
	switch (pkEntry->m_iType)
	{
	case LC_GLB_KEY_TP_NUM:
		{
			pkEntry->m_iValue = atoll(val.c_str()); 
		}
		break;
	default:
		break;
	}
	pkEntry->m_sValue = val;

	Utility::Redis_Channel& channel = GetGameServerApp()->GetRedisChannel();
	ResultType rslt = RE_SUCCESS;
	Utility::redis_request_base* req = Utility::new_redis_request("hset", &redispp::Connection::hset, m_sKeyPrefix, sKey, val);

	if(req)	{ 
		rslt = channel.async_req(req);
	} else {
		rslt = RE_FAIL;
	};

	return rslt;
}

ResultType LC_GlobalKeyManager::GlobalKeyHIncreBy(const std::string& sKey, int32_t val)
{
	LC_GlobalKeyEntry* pkEntry = _getGlobalKey(sKey);
	if (NULL == pkEntry)
	{
		return RE_FAIL;
	}
	switch (pkEntry->m_iType)
	{
	case LC_GLB_KEY_TP_NUM:
		break;
	default:
		return RE_FAIL;
		break;
	}

	ResultType rslt = RE_SUCCESS;
	pkEntry->m_iValue += val;
	Utility::Redis_Channel& channel = GetGameServerApp()->GetRedisChannel();

	Utility::redis_request_base* req = Utility::new_redis_request("hincrBy", &redispp::Connection::hincrBy, m_sKeyPrefix, sKey, val);

	if(req)	{ 
		rslt = channel.async_req(req);
	} else {
		rslt = RE_FAIL;
	};

	return rslt;
}
void LC_GlobalKeyManager::_updateGlobalKeyValues(int32_t err, std::vector<std::string>& keyinfos)
{
	if (err)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "Failed to _updateGlobalKeyValues!");
		return;
	}

	Utility::Redis_Channel& channel = GetGameServerApp()->GetRedisChannel();
	for (size_t i=0; i+1<keyinfos.size(); )
	{
		const std::string & key = keyinfos[i++];
		const std::string & val = keyinfos[i++];

		LC_GlobalKeyEntry* pkEntry = _getGlobalKey(key);
		if (NULL == pkEntry)
		{
			continue;
		}
		pkEntry->m_sValue = val;

		switch(pkEntry->m_iType)
		{
		case LC_GLB_KEY_TP_NUM:
			{
				try  
				{   
					pkEntry->m_iValue = boost::lexical_cast<int64_t>(pkEntry->m_sValue);   
				}   
				catch(boost::bad_lexical_cast& e)   
				{   
					GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "Failed to convert GlobalKeyValues ! Err:[%s] Key:[%s] Type:[%d] Val:[%s]", e.what(), key.c_str(), pkEntry->m_iType, val.c_str());
					std::string local_val = boost::lexical_cast<std::string>(pkEntry->m_iValue);
					ASYNC_REDIS_REQUEST_NCB(channel, &redispp::Connection::hset, m_sKeyPrefix, key, local_val);
					pkEntry->m_sValue = local_val;
				}  
			}
			break;
		default:
			break;
		}
	}
}

void LC_GlobalKeyManager::_updateGlobalKey(const std::string& key, HGetCallBackType cb, int32_t err, const std::string& val)
{
	if (err)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "Failed to _updateGlobalKey!%s", key.c_str());
		cb(err, val);
		return;
	}

	Utility::Redis_Channel& channel = GetGameServerApp()->GetRedisChannel();

	LC_GlobalKeyEntry* pkEntry = _getGlobalKey(key);
	if (NULL == pkEntry)
	{
		cb(RE_FAIL, val);
		return;
	}

	pkEntry->m_sValue = val;

	switch(pkEntry->m_iType)
	{
	case LC_GLB_KEY_TP_NUM:
		{
			try  
			{   
				pkEntry->m_iValue = boost::lexical_cast<int64_t>(pkEntry->m_sValue);   
			}   
			catch(boost::bad_lexical_cast& e)   
			{   
				GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "Failed to convert _updateGlobalKey ! Err:[%s] Key:[%s] Type:[%d] Val:[%s]", e.what(), key.c_str(), pkEntry->m_iType, val.c_str());
				std::string local_val = boost::lexical_cast<std::string>(pkEntry->m_iValue);
				ASYNC_REDIS_REQUEST_NCB(channel, &redispp::Connection::hset, m_sKeyPrefix, key, local_val);
				pkEntry->m_sValue = local_val;
			}  
		}
		break;
	default:
		break;
	}
	cb(err, val);
}