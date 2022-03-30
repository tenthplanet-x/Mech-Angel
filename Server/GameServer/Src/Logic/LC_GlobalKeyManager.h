#ifndef __LC_GLOBAL_KEY_MANAGER_H__
#define __LC_GLOBAL_KEY_MANAGER_H__

#pragma once

#include "MM_BaseObject.h"
#include "UT_RedisChannel.h"
#include "GameServerApp.h"

enum LC_GlobalKeyType
{
	LC_GLB_KEY_TP_NUM = 0,
	LC_GLB_KEY_TP_STRING,
};
////////////////////////////////////////////////////////////////
class LC_GlobalKeyEntry
{
public:
	LC_GlobalKeyEntry(void): m_iType(LC_GLB_KEY_TP_NUM), m_iValue(0) {}

	int32_t		m_iType;
	std::string m_sKey;
	std::string m_sValue;	
	int64_t		m_iValue;
};
typedef mem::map<std::string, LC_GlobalKeyEntry> LC_GlobalKeyEntryMap;
////////////////////////////////////////////////////////////////
class LC_GlobalKeyManager  : public Memory::MM_BaseObject, public Utility::UT_Singleton<LC_GlobalKeyManager>
{
public:
	typedef boost::function<void (int32_t, const std::string&)> HGetCallBackType;

	LC_GlobalKeyManager(void): m_fLastTimeStamp(0.0f) {}

	bool Init(void);
	bool Unit(void);

	void Update(float fCurrentTime);

	inline void AddGlobalKey(int32_t iType, const std::string& sKey)
	{
		LC_GlobalKeyEntry& rEntry = m_kKeys[sKey];
		rEntry.m_iType = iType;
		rEntry.m_sKey = sKey;
	}

	inline const LC_GlobalKeyEntry* GetGlobalKey(const std::string& sKey) { return _getGlobalKey(sKey);}

	ResultType GlobalKeyHGet(const std::string& sKey, HGetCallBackType cb)
	{
		LC_GlobalKeyEntry* pkEntry = _getGlobalKey(sKey);
		if (NULL == pkEntry)
		{
			return RE_FAIL;
		}
		ResultType rslt = RE_SUCCESS;
		Utility::Redis_Channel& channel = GetGameServerApp()->GetRedisChannel();
		HGetCallBackType _cb = boost::bind(&LC_GlobalKeyManager::_updateGlobalKey, this, sKey, cb, _1, _2);
		ASYNC_REDIS_REQUEST(rslt, channel, &redispp::Connection::hget, _cb, m_sKeyPrefix, sKey);
		return rslt;
	}

	ResultType GlobalKeyHSet(const std::string& sKey, const std::string& val);
	ResultType GlobalKeyHIncreBy(const std::string& sKey, int32_t val);
private:
	void _updateGlobalKeyValues(int32_t err, std::vector<std::string>& keyinfos);
	void _updateGlobalKey(const std::string& key, HGetCallBackType cb, int32_t err, const std::string& val);

	LC_GlobalKeyEntry* _getGlobalKey(const std::string& sKey)
	{
		LC_GlobalKeyEntryMap::iterator it = m_kKeys.find(sKey);
		if (it != m_kKeys.end())
		{
			return &it->second;
		}
		return NULL;
	}
protected:
	LC_GlobalKeyEntryMap	m_kKeys;
	std::string				m_sKeyPrefix;
	float					m_fLastTimeStamp;
};



#endif 