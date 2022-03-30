#pragma once

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include "MG_Game2Guild.h"
#include "LC_Define.h"

typedef boost::function<void(int64_t raidid, uint32_t serverid, int32_t resId)> RaidCallBack;

struct LC_CrossRealmRaidInfo
{
	int64_t m_RaidId;
	uint32_t m_RaidServerId;
	int32_t m_ResId;
	int64_t m_TimeStamp;
	RaidCallBack m_CreatedCallBack;
	RaidCallBack m_CreateFailedCallBack;
	RaidCallBack m_RaidDestoryCallBack;
	LC_CrossRealmRaidInfo():m_RaidId(0), m_RaidServerId(0), m_ResId(0), m_TimeStamp(0), m_CreatedCallBack(), m_CreateFailedCallBack(), m_RaidDestoryCallBack()
	{

	}
};

struct LC_CrossRealmPlaceRaidInfo
{
	int64_t m_RaidId;
	bool m_Destory;
	int m_LogicId;
	LC_CrossRealmPlaceRaidInfo():m_RaidId(0), m_Destory(false), m_LogicId(0) {}
};

class LC_CrossRealmRaidManager: public  Memory::MM_BaseObject, public Utility::UT_Singleton< LC_CrossRealmRaidManager >
{
public:
	LC_CrossRealmRaidManager();
	virtual ~LC_CrossRealmRaidManager();
	void Clear();
	bool Init();
	void UnInit();
	//创建跨服副本,FALSE表示全局
	int CreateCrossRealmRaid(int resid, RaidCallBack createdcallback, RaidCallBack failedcallback, RaidCallBack destorycallback);
	void CreateCrossRealmRaidResp(ResultType result, int64_t raidid, uint32_t serverid, int32_t nResId);
	void DestoryCrossRealmRaid(int iMapResID);
	void DestoryCrossRealmRaidGameNotify(int64_t nRaidID, int iMapResID, int nServerID);
	ResultType EnterCrossRealmRaid(LC_ServerPlayer* player, int iMapResID);
	void EnterCrossRealmRaidResp(LC_ServerPlayer* player, ResultType result, uint32_t targetserverid);

	ResultType PrepareCrossRealmRaid(int64_t raidid, int resid, int toplevel);
	void CrossRealmRaidEndNotify(int logicid);
	ResultType PreEnterCrossRealmRaid_Place(int64_t raidid, const unique_id_impl& charID, user_id_type userid);
	ResultType EnterCrossRealmRaid_Place(int raidid, LC_ServerPlayer* player);
	void SocietyServerShutDown();
	LC_CrossRealmPlaceRaidInfo* GetCrossRealmPlaceRaid(int64_t nRaidId);
private:
	void __SendDestoryCrossRealmRaid(int64_t raidid, int iMapResID);
private:
	mem::map<int, LC_CrossRealmRaidInfo> m_CrossRealmRaid;
	mem::map<int64_t, LC_CrossRealmPlaceRaidInfo> m_CrossRealmPlaceRaid;
	mem::map<int, int64_t> m_LogicId2RaidId;
};
