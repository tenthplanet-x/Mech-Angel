/********************************************************************
	created:	2014/06/13
	filename: 	GameServerManager.h
	file path:	GameServer\Src
	file base:	GameServerApp
	file ext:	h
	
	purpose:主要管理服务器与服务器之间的事务
*********************************************************************/
#ifndef _GAME_SERVER_MANAGER_H
#define _GAME_SERVER_MANAGER_H
#include "ServerCommDef.h"

#include "MG_Game2Login.h"
#include "NetSessionType.h"

class GameServerManager;
struct CrossGetTopLevel
{
	CrossGetTopLevel(): m_iCrossLevel(0){}
	virtual void invoke(int32_t err, std::vector<std::string>& infos);

	int32_t GetServerLevel(int32_t iCitizenship)
	{
		mem::map<int32_t, int32_t>::iterator it = m_kServerLevel.find(iCitizenship);
		return (it!=m_kServerLevel.end()) ? it->second : 0;
	}

	void SetServerLevel(int32_t iCitizenship, int32_t iLevel)
	{
		m_kServerLevel[iCitizenship] = iLevel;
	}

	void ComputeCrossLevel(GameServerManager* pkServerMgr);

	mem::map<int32_t, int32_t> m_kServerLevel;
	int32_t m_iCrossLevel;
};

struct CrossTop10AvgPower
{
	CrossTop10AvgPower() : m_iTopServer(0), m_iLastZeroTime(0), m_iSyncTopServerTime(0), m_fLastSyncTime(0.0) {}

	int32_t	m_iTopServer;
	time_t	m_iLastZeroTime;
	time_t	m_iSyncTopServerTime;
	float	m_fLastSyncTime;
};

class GameServerManager:public  Memory::MM_BaseObject
{
public:
	typedef mem::vector<int32_t>	GroupServerList;
public:
	GameServerManager();
	~GameServerManager();

	bool Init(int curserverid,int CurServerType);
	void Uinit();
	void Clear();
	void Update(float fCurrTime);
	
	Protocol::PS_ServerSimInfo* GetActive(int serverid);
	int GetActiveId(int serverid);
	int GetActiveGroupId(int serverid);
	bool GetServerInfo(int serverid, StringType& name);
	inline GroupServerList& GetCurGroupServers() { return m_CurServerIDs; }
	int32_t	GetGroupServerIDIndex( int32_t nServerID = 0);
	inline int32_t GetCurGroupServerIDIndex() { return m_CurGroupIndex; }

	inline bool IsSameServer(int32_t iServerIDA, int32_t iServerIDB) { return (iServerIDA==iServerIDB) ? true : GetActive(iServerIDA) == GetActive(iServerIDB);}

	bool SetGameInitInfo(const msg_define::game_init_info& kInit);
	bool ResetGameInitInfo(const msg_define::game_init_info& kInit);
	const msg_define::game_init_info& GetGameInitInfo(void) {return m_GameInit;}
	bool IsServerOpened(void);
	const msg_define::server_time_info* GetServerTimeInfo(int32_t iServerID);

	// 获取开服当天00:00的时间戳
	uint32_t GetServerStartTimeAbsolute();
	//获得活动开始的时间戳
	uint32_t GetActivityStartTime(int32_t nStarCoolTime);
	// 计算相关活动的信息
	bool GetServerStartTimeAbsoluteIntervalLoopInfo(const std::vector<uint32_t>& nLoopTimer, int32_t nStarCoolTime, uint32_t& startTime, uint32_t& nRemainTime, uint32_t& index);


	bool LoadServerListJson(uint32_t nUpdateTime = 0);
	void GameServerLoadSuccess(void);
	void GameServerLoadFail(void);

	void OnGroupServerReady(void);
	void OnGroupServerLost(void);

	void ConnectAllServer(void);
	void CloseAllServer(void);

	inline bool isLoaded(void){ return m_Loaded;};

	int64_t GetServerStartTime(void);
	int64_t GetServerConflateTime(void);
	int32_t GetEntryServerStartTime();
	int64_t GetEntryServerConflateTime();

	inline int GetCurServerId(void){ return m_CurServerId;};
	inline int GetCurServerType(void){ return m_CurServerType;}
	inline int GetCurGuildGroupId(void){ return GetActiveGroupId(m_CurServerId);}
	inline Protocol::PS_ServerSimInfo* GetCurServerInfo(void){ return GetActive(m_CurServerId);}

	//服务器与服务器之间协议
	void SendMsg2Server(NetSessionType session_type,uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody, uint32_t uiUserID = 0, const unique_id_impl& uiCharID = 0, int32_t iSequence = 0,int32_t iReserved = 0);
	void SendServerInfo2AllServer();
	void ReportServerRealm(NetSessionType nSession);
	void ReportServerRealm(NetSessionType* pArr, int nSize);
	void ReportServerInfo(NetSessionType* pArr, int nSize);
	void RequestServerAllianceInfo();

	//公会服务器
	void RegisterCrossRealmGuildWar();
	void SocietyServerConnect();
	void SocietyServerDisConnect();
	
	//group服务器
	void RegisterCrossRealmTeamMap();
	void InitRankRelated();

	//DBServer
	void DBServerConnect();
	void DBServerDisConnect();

	//TradeServer
	void TradeServerConnect();
	void TradeServerDisConnect();

	//ChatServer
	void ChatServerConnect();
	void ChatServerDisConnect();

	//GlobalServer
	void GlobalServerConnect();
	void GlobalServerDisConnect();

	//GameServer
	void GameServerConnect(int32_t iID);
	void GameServerDisConnect(int32_t iID);

	void RegisterServerOnLineUser(NetSessionType nType);

	void SetServerIdFromArgv( int32_t nInput );
	int32_t GetServerIdFromArgv();

	//request cross level
	int GetCorssLevel(void);
	void SyncCrossServer(void);
	int32_t GetCrossAverageLevel(void){return m_kCrossLevel.m_iCrossLevel;}
	int32_t GetCrossServerLevel(int32_t iCitizenship){return m_kCrossLevel.GetServerLevel(iCitizenship);}
	int32_t GetGroupServerTopLevel();
	int32_t GetCurGroupId() const { return m_CurGroupId; }

	//top10 avg power
	int32_t GetTopAvgPowerServer() { return m_kCrossAvgPower.m_iTopServer; }
protected:
	void _doInit(void);
	void _CopyData(Protocol::PS_ServerSimInfo& rNode, const Js_ActiveServerInfo& rData);
	Protocol::PS_ServerSimInfo* _updateActive(const Js_ActiveServerInfo& kActive);
	inline void _addIndex(int32_t serverid, Protocol::PS_ServerSimInfo* pkActive) { m_Server2Active[serverid] = pkActive->m_iServerID;}
	inline void _delIndex(int32_t serverid) { m_Server2Active.erase(serverid);}
	bool _getServerRealm(msg_define::server_realm& rk_realm);
private:
	int32_t m_ResetGameInitInfoFlag;
	bool	m_Loaded;
	msg_define::game_init_info m_GameInit;
	int64_t m_ServerJsonUpdateTime;
	int32_t m_CurServerId;
	int32_t m_CurServerType;
	int32_t m_CurGroupId;
	int32_t m_CurGroupIndex;
	int32_t m_nServerIdFromArgv;

	bool m_bUpdateTaskHonorPointInfo;

	typedef mem::map<int32_t, Protocol::PS_ServerSimInfo> AllServerInfoMap;
	typedef mem::map<int32_t, int32_t> AllServerInfoIndices;

	AllServerInfoMap m_AllServerInfo;
	AllServerInfoIndices m_Server2Active;
	GroupServerList m_CurServerIDs;
	mem::vector<NetSessionType> m_AllServerType;
	float m_fLastUpdateTime;

	CrossGetTopLevel	m_kCrossLevel;
	Utility::UT_Timer	m_kCrossTimer;
	CrossTop10AvgPower	m_kCrossAvgPower;
};

#endif
