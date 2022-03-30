#ifndef _GAME_SERVER_APP_H
#define _GAME_SERVER_APP_H

#define GetGameServerApp()		( (GameServerApp*) GameServerApp::GetSingletonPtr() )
#define GetGameServerID()		( GetGameServerApp()->GetServerInfo().m_iServerID )
#define GetGameServerEntryID()	( GetGameServerApp()->GetServerEntryId() )
#define GetGameUnionID()		( GetGameServerApp()->GetServerUnionInfo().union_id)
#define GetGameRegionID()		( GetGameServerApp()->GetServerUnionInfo().group_id)

#ifdef WIN32
	#pragma warning( disable : 4819)
#endif

#include "NW_Helper.h"
#include "MG_ProtocolStruct.h"
#include "MG_CommonMg.h"
#include "NW_MessageRecorder.h"
#include "CF_ServerCSVManager.h"
#include "GameServerManager.h"
#include "ExportRobot.h"
#include "UT_Redis.h"
#include "UT_RedisChannel.h"
#include "NW_NetManagerBase.h"
#include "UT_TimeManager.h"
#include "NW_NetSessionManager.h"
#include "UT_Timer.h"
#include "LC_Define.h"
#include "LC_TextFilterChannel.h"
#include "LC_DailyDataReport.h"
#include "AH_AsyHttpClient.h"
#include "LC_LogicMapFileCache.h"
#include "base64.h"
using namespace Utility;

class NW_ServerSession;

class LC_ServerPlaceHolderCSVManager;
class LC_ServerLogicManager;
class TT_TextManager;
class MC_Message2ClientManager;
class TS_TranManager;
class LC_ServerPackEntryFactory;
class LC_HttpHandler;
class LC_ThirdPlatformReport;
namespace GameLogic
{
	class LC_GuildRaidDataManager;
	class LC_TaskManager;
	class LC_GameEventManager;
}

namespace Skill
{
	class SK_SkillEventManagerBase;
}

struct server_union_info
{
	int					group_id;			//大区id
	StringType			group_name;			//大区名字
	int					union_id;			//联运商id
	StringType			union_name;			//联运商名称
	server_union_info(): group_id(0), union_id(0) {}
};

struct gm_account
{
	StringType user;
	StringType pwd;
	int priority;

	gm_account():priority(INIT_GM_PRIORITY){}
};

struct init_setting
{
	init_setting(): version(0) {}
	uint32_t version;
	mem::vector<std::string> activation_codes;
	mem::vector<gm_account> gm_accounts;
	mem::vector<StringType> cmds;
};

struct redis_info
{
	redis_info(void):port(6379){}
	std::string		ip;
	int				port;
	std::string		auth;
};

struct update_bulletin
{
	update_bulletin() : update_time(0) {}
	uint32_t	update_time;
	StringType	bulletin;
};
AJSON(update_bulletin, (update_time)(bulletin));

struct MultiUsersCluster
{
	MultiUsersCluster(void)
		:iServerType(0), iServerID(0)
	{
		kUsers.reserve(MAX_MULTICAST_USER_COUNT);
	}

	inline void Init(int32_t iSvrType, int32_t iSvrID)
	{
		iServerType = iSvrType;
		iServerID = iSvrID;
	}

	inline void Clear(void)
	{
		kUsers.clear();
	}

	inline bool AddUserReturnFull(const Protocol::PS_UserSocketIndex& user)
	{
		kUsers.push_back(user);
		return kUsers.size() >= MAX_MULTICAST_USER_COUNT;
	}

	inline int32_t ServerType(void) {return iServerType;}
	inline int32_t ServerID(void) {return iServerID;}
	inline const Protocol::PS_UserSocketIndices& Indices(void) const {return kUsers;}
	inline bool IsEmpty(void) const {return kUsers.empty();}

	int32_t iServerType;
	int32_t iServerID;
	Protocol::PS_UserSocketIndices kUsers;
};

class GameServerApp;
struct MulticastMsgHelper
{
	typedef mem::map<unique_id_type, MultiUsersCluster> MultiUsersClusterMap;

	void SetUp(GameServerApp* pkApp, mem::vector<Protocol::PS_ServerInfo>& gatewayInofs);
	MultiUsersCluster* GetUserCluster(int32_t iSvrType, int32_t iSvrID);

	BOOL Begin(uint16_t msgType, GameLogic::LC_PlayerOperator* filter);
	BOOL Encode(Protocol::MG_BODY* pkMsgBody);
	BOOL AddUserIndices(LC_ServerPlayer* pkPlayer);
	BOOL AddUserIndices(Protocol::PS_UserSocketIndex& rkUserIndex);
	BOOL End(void);


	GameServerApp* app;
	uint16_t usMsgType;
	MultiUsersClusterMap userIndicesCache;
	Protocol::MG_MultiCastMessage multiCastMessage;

	GameLogic::LC_PlayerOperator* pkFilter;
};

enum E_LocalServerState
{
	E_LocalServerState_Local = 0,	//本服
	E_LocalServerState_Error,		//错误
	E_LocalServerState_Cross,		//跨服
};




class GameServerApp : public NetWork::NW_NetManagerBase
{
public:
	GameServerApp();
	~GameServerApp();

	virtual bool							Init();
	bool									InitLess();

	virtual void							UnInit();

	virtual void							Update();
	virtual void							Save2Exit();
	void									StopGameLogic();

	inline MG_SERVER_TYPES					GetServerType(void){return MG_SERVER_TYPES(m_kServerInfo.m_iType);}
	inline bool								IsSpecialGameServer(void)
	{
		return MG_SERVER_TYPES(m_kServerInfo.m_iType) != SERVER_TYPE_GAME;
	}

	inline int32_t							GetServerEntryId(void) const { return m_iServerEntryId; }
	inline void								SetServerEntryId(int32_t sid) { m_iServerEntryId = sid; }
	inline const Protocol::PS_ServerInfo& 	GetServerInfo(void) const { return m_kServerInfo; }
	inline server_union_info&				GetServerUnionInfo(void) { return m_server_union_info_; }
	inline mem::vector<Protocol::PS_ServerInfo>& GetGatewayInfo(void) { return m_gatewayInfos; }
	inline const Protocol::PS_ServerInfo& 	GetGatewayInfo(size_t index) { return m_gatewayInfos[index]; }
	inline mem::vector<Protocol::PS_ServerInfo>& GetGameServerInfos(void) { return m_GameServerInfos; }
	void									SetGameServerInfos(const mem::vector<Protocol::PS_ServerInfo>& gameServerInfos);
	E_LocalServerState						CheckLocalServer(int iServerID);
	inline int								GetClientVersionLimit(void) { return m_iClientVersionLimit; };
	inline bool								IsSameServer(int32_t iServerIDA, int32_t iServerIDB)
	{
		return GetServerManager()->IsSameServer(iServerIDA, iServerIDB);
	}

	NW_ServerSession*						GetSession(int server_type, int32_t iID = 0);
	BOOL									IsServerConnect(int server_type);
	BOOL									LoadServerConfig(void);

	Protocol::MG_PACKET_ENCODE_HANDLER&		GetPacketHandler(void){ return m_kPakcetHandler; }

	BOOL									SendMsgToServerType(Protocol::SS_MG_HEAD& rkMsgHead, Protocol::MG_BODY* pkMsgBody=NULL);
	BOOL									SendMsgToServerType(Protocol::SS_MG_HEAD& rkMsgHead, const char* msg_body, uint16_t body_size);
	BOOL									SendMsgToServerType(MG_SERVER_TYPES server_type, int32_t iID , uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody = NULL, uint32_t uiUserID = 0, const unique_id_impl& charID = 0, int32_t iSequence = 0, int32_t iReserved = 0, uint16_t nRouteFlag=0);
	BOOL									SendMsgToServerType(MG_SERVER_TYPES server_type, int32_t iID , uint16_t usMsgType, const char* pMsg, uint16_t nSize, uint32_t uiUserID = 0, const unique_id_impl& charID = 0, int32_t iSequence = 0, int32_t iReserved = 0, uint16_t nRouteFlag=0);

	BOOL									SendMsgToServerByGateway(int32_t iID, uint8_t serverType, uint32_t serverID, uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody = NULL, uint32_t uiUserID = 0, const unique_id_impl& charID = 0, int32_t iSequence = 0);
	BOOL									SendMsgToClientByGateway(int32_t iID, uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody = NULL, uint32_t uiUserID = 0, const unique_id_impl& charID = 0, int32_t iSequence = 0);
	BOOL									SendMsgToClientByGateway(int32_t iID, uint16_t usMsgType, const char* pMsg, uint16_t nSize, uint32_t uiUserID = 0, const unique_id_impl& uiCharID = 0, int32_t iSequence = 0);
	// 只为做战斗录制用
	BOOL									SendMsgFightTranscribeRecord(int32_t iID, uint16_t usMsgType,Protocol::MG_BODY* pkMsgBody = NULL, uint32_t uiUserID = 0, const unique_id_impl& charID = 0, int32_t iSequence = 0);
	

	BOOL									MulticastMsgByGame(int32_t iID, uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody = NULL);
	BOOL									MulticastMsgToGateway(int32_t iID, uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody = NULL);

	BOOL									MulticastMsgToGateway(LC_PlayerPtrList& playerList, uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody = NULL, GameLogic::LC_PlayerOperator* pkFilter=NULL);
	BOOL									MulticastMsgToGateway(const GameLogic::LC_PlayerIDPtrMap& playerList, uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody = NULL, GameLogic::LC_PlayerOperator* pkFilter=NULL);
	BOOL									MulticastMsgToGateway(mem::vector<const GameLogic::LC_PlayerIDPtrMap*>& playerListVec, uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody = NULL, GameLogic::LC_PlayerOperator* pkFilter=NULL);
	BOOL									BroadCastMsgToClient(uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody = NULL);

	NW_ServerSessionMap						GetServerSession(NetSessionType sessiontype);
	void SetNodeType( int32_t nType );
	int32_t GetNodeType();
	bool IsNodeType_Global();
	inline bool								IsBranchNode(void) {return m_bBranchNode;}
	inline int								GetServerAcceptCount(void) { return m_iGameServerAcceptCount;}
	inline bool								GetUseNewPlayerMap(void) {return m_bUseNewPlayerMap;};
	inline int								GetNewPlayerCountPerMap(void) {return m_iNewPlayerCountPerMap;};
	inline GameServerManager*				GetServerManager(void) {return &m_ServerManager;};
	inline bool								GetAllowDebug(void) { return m_allow_gm_cmd; };
	inline bool								Get360PriviGMTest(void) { return m_360_privi_gm_test; }
	inline void								Set360PriviGMTest(bool val) { m_360_privi_gm_test = val; }
	inline Utility::Redis_Handler*			GetGlobalRedisHandler(void) { return &m_kGlobalRedisHandler; }
	inline Utility::Redis_Channel&			GetGlobalRedisChannel(void) { return m_kGlobalRedisChannel; }
	inline Utility::Redis_Handler*			GetRedisHandler(void) { return &m_kRedisHandler; }
	inline Utility::Redis_Channel&			GetRedisChannel(void) { return m_kRedisChannel; }
	inline const std::string&				GetRedisGlobalPrefix(void) { return m_sRedisGlobalPrefix; }
	inline const std::string&				GetRedisGroupPrefix(void) { return m_sRedisGroupPrefix; }
	inline const std::string&				GetRedisServerPredix(void) { return m_sRedisServerPrefix; }
	inline Utility::Redis_Channel&			GetMonitorRedisChannel(void) { return m_kMonitorRedisChannel; }

	const std::string&				GetRedisEntryServerPredix(void);
	void SetRedisEntryServerPredix(int entryServerID);

	bool GetInitEntryServeState();

	inline const std::string&				GetServerListPrefix(void) { return m_strServerListPrefix; }

	std::string								GetRedisServerPredix(int32_t iServerID);
	const std::string&						GetRedisPrefix(int32_t iSphereType)	;	
	Utility::Redis_Handler*					GetRedisHandler(int32_t iSphereType);		
	Utility::Redis_Channel*					GetRedisChannel(int32_t iSphereType);					

	inline ResultType						SubscribeChannel(const std::string& channel, const RedisChannelHandler& _handler) { return m_kRedisChannel.FakeSubscribe(channel, _handler); }

	inline redis_info&						GetGlobalRedisInfo() { return m_kGlobalRedis; }

	inline TextFilterChannel&				GetTextFilterChannel(void) { return m_kTextFilterChannel; }
	inline AsyHttpClient&                   GetAsyHttpClient(void) { return m_kAsyHttpClient; }
    inline LC_HttpHandler*					GetHttpHandler(void) { return m_kHttpHandler; }
	inline LC_DailyDataReport&				GetDailyDataReport(void) { return m_kDailyDataReport; }

	inline std::string						GetRedisGMReqChannel(void) { return m_sRedisServerPrefix + UT_REDIS_KEY_GM_CMD_REQS; }		//server为单位
	inline std::string						GetRedisGMRespChannel(void) { return m_sRedisServerPrefix + UT_REDIS_KEY_GM_CMD_RESPS;}		//server为单位
	inline std::string						GetRedisGMSortChannel(void) { return m_sRedisGroupPrefix + UT_REDIS_KEY_GM_CMD_SORT; }		//group为单位
	inline std::string						GetRedisHttpReqChannel(void) { return m_sRedisGroupPrefix + UT_REDIS_KEY_HTTP_REQS; }		//group为单位
	inline std::string						GetRedisHttpRespChannel(void) { return m_sRedisServerPrefix + UT_REDIS_KEY_HTTP_RESPS; }	//server为单位
	inline std::string						GetRumorDirtyChannel(void) { return m_sRedisServerPrefix + UT_REDIS_KEY_RUMOR_DIRTY; }		//server为单位

	inline std::string						GetGamePlayerOnlineSnapshot(void) { return m_sRedisGroupPrefix + UT_REDIS_KEY_PLAYER_ONLINE_SNAPSHOT; }	//玩家在线快照数据 索引依据玩家账号
	inline std::string						GetGameAccountIDsKey(void) { return m_sRedisGroupPrefix + PLATFORM_AUTH_GAME_ACCOUNTCHARID; }	//group为单位，考虑跨服
	inline std::string						GetGameUserInfoKey(void) { return m_sRedisGroupPrefix + PLATFORM_AUTH_GAME_USERINFOKEY; }	//group为单位，考虑跨服
	inline std::string						GetGameNameInfoKey(void) { return m_sRedisGroupPrefix + PLATFORM_AUTH_GAME_NAMEINFOKEY; }	//group为单位，考虑跨服
	inline std::string						GetGameChar2UserKey(void) { return m_sRedisGroupPrefix + PLATFORM_AUTH_GAME_CHAR2USERKEY; }	//group为单位，考虑跨服
	inline std::string						GetGamePlatformInfoKey(void) { return m_sRedisGroupPrefix + PLATFORM_AUTH_GAME_PLATFORMINFOKEY; }	//group为单位，考虑跨服
	inline std::string						GetGamePlatformLoginKey(void) { return m_sRedisGroupPrefix + PLATFORM_AUTH_GAME_USERLOGINKEY; }	//group为单位，考虑跨服
	inline std::string						GetGameSurviveCheckKey(void) { return m_sRedisGroupPrefix + SERVER_SURVIVE_CHECK_KEY; }		//group为单位，考虑多服
	inline std::string						GetGameHttpPublishKey(void) { return m_sRedisServerPrefix + PLATFORM_HTTP_PUBLISH_GAME_KEY; }	//http广播，server为单位

	inline std::string						GetGameGlobalShopKey(void) { return m_sRedisServerPrefix + PLATFORM_AUTH_GAME_GLOBALSHOP; }	//server为单位

	template <typename REDIS_DEF>
	inline ResultType						PublishChannel(const std::string& channel, int32_t msg_type, int32_t msg_id, const REDIS_DEF& msg)
	{
		return m_kRedisChannel.Publish(channel, msg_type, msg_id, msg);
	}

	template <typename JSON_DEF>
	inline ResultType						PublishChannel(const std::string& channel, const JSON_DEF& msg)
	{
		return m_kRedisChannel.Publish(channel, msg);
	}

	inline ResultType						PublishMonitor(const std::string& channel, const std::string& msg)
	{
		return m_kMonitorRedisChannel.Publish(channel, msg);
	}

	inline void								AddBonusRole(unique_id_type charID)
	{
		m_kBonusRoleSet.insert(charID);
	}
	inline void								RemoveBonusRole(unique_id_type charID)
	{
		m_kBonusRoleSet.erase(charID);
	}
	inline void								ClearBonusRoles()
	{
		m_kBonusRoleSet.clear();
	}
	inline bool								IsBonusRole(unique_id_type charID)
	{
		unique_id_set::iterator it = m_kBonusRoleSet.find(charID);
		return it != m_kBonusRoleSet.end();
	}
	inline void								SetPreDayActive(int32_t active)
	{
		m_iPredayCrossActive -= m_iPredayActive;
		m_iPredayCrossActive = m_iPredayCrossActive < 0 ? 0 : m_iPredayCrossActive;
		m_iPredayActive = active < 0 ? 0 : active;
		m_iPredayCrossActive += m_iPredayActive;
	}
	inline int32_t							GetPreDayActive() const { return m_iPredayActive; }
	inline int32_t							GetPreDayCrossActive() const { return m_iPredayCrossActive; }

	void									ReloadCsv(const StringType& rkCSVFile, const StringType& sVersion);
	void									SetCustomResVersion(const std::string& ver);

	inline const init_setting&				InitSetting(void){return m_initSetting;}
	inline const StringType&				GetConfigPath(void) {return m_strConfigPath;}
	inline void								SetConfigPath(const char* pPath){m_strConfigPath = pPath;}
	inline const StringType&				GetLogPath(void) {return m_strLogPath;}
	inline void								SetLogPath(const char* pPath){m_strLogPath = pPath;} 

	inline const std::string&				GetAct360PriviUrl(void) { return m_strAct360PriviUrl; }

	void SetServerId( int32_t nServerId );

	void									SetSyncActiveFlag(bool val) { m_bSyncActiveSucceed = val; }
	double									GetBossShieldRate();
	double									GetCrossBossShieldRate();

	StringType								GetUpdateBulletin() const { return m_kUpdateBulletin.bulletin; }
	void									LoadUpdateBulletin();
	void									SetUpdateBulletin(uint32_t timestamp, const StringType& sBulletin);
	bool									CheckBulletinUpdate(uint32_t timestamp) const { return timestamp < m_kUpdateBulletin.update_time; }
	uint32_t								GetBulletinTime() const { return m_kUpdateBulletin.update_time; }

	inline MulticastMsgHelper&				GetMulticastMsgHelper(void) {return m_multiMsgHelper;}
	void									AddGameServer(NW_ServerSession* pkGame);
	NW_ServerSession*						RegisterGameServer(Protocol::PS_ServerInfo& rkServer);
	
protected:
	void									InitMessage2Client(void);
	void									UninitMessageChannel(void);
	void									Message2Client(float currentTime);

	void									SetMsgHead(uint16_t usMsgType, uint32_t uiUserID, const unique_id_impl& charID, int32_t iSequence, uint8_t cDstType, int32_t iDstID, int32_t iReserved = 0);
	void									SetMsgHeadToClient(uint16_t usMsgType, uint32_t uiUserID, const unique_id_impl& charID, int32_t iSequence, uint8_t cDstType, int32_t iDstID);
	void									SetMulticastMsgHead(uint16_t usMsgType);
	void									SetBroadcastMsgHead(uint16_t usMsgType);
	void									publishSurviveByRedis(void);
	void									SyncPredayActive(float currentTime);

	//组包工具
	Protocol::MG_PACKET_ENCODE_HANDLER		m_kPakcetHandler;
	Protocol::SS_MG_HEAD					m_kMsgHead;
	
	//模拟 发给client 头 组包工具
	Protocol::MG_PACKET_ENCODE_HANDLER		m_kPakcetHandler_1;
	Protocol::SS_MG_HEAD					m_kMsgHead_1;

	//事务管理
	TS_TranManager*							m_pkTranManager;

	//文字管理
	TT_TextManager*							m_pkTextManager;

	//任务
	GameLogic::LC_TaskManager*				m_pkTaskManager;

	//负责创建物品资产
	LC_ServerPackEntryFactory*				m_pkPackEntryFactory;

	LC_ServerLogicManager*					m_pkLogicManager;
	
	Utility::UT_TimeManager*				m_pkTimeManager;
	Utility::UT_TimerManager*				m_pkTimerMgr;

	float									m_fLastLogicUpdateTime;

	float									m_fGameEventUpdateTime;
	float									m_fSkillEventUpdateTime;
	float									m_fTaskUpdateTime;

	GameLogic::LC_GameEventManager*			m_pkGameEventManager;			//系统事件管理器
	Skill::SK_SkillEventManagerBase*		m_pkSkillEventManager;			//技能事件管理器
	GameLogic::LC_GuildRaidDataManager*		m_pkGuildRaidDataManager;		//公会副本活动id管理器

	LC_ThirdPlatformReport*					m_pkThirdPlatFormReport;
	//------------------------ DBServer logic begin -------------------------------
	FileSystem::LC_LogicMapFileCache* m_pMapResourceCache;
	//与DBServer的连接
	NetWork::NW_NetSessionManager*			m_pkNetSessionManager;
	NW_SIOSessionManager*					m_pkSIOManager;

	NW_ServerSession*						m_pkDBServerSession;
	NW_ServerSession*					    m_pkLoginServerSession;
	NW_ServerSession*					    m_pkGroupServerSession;
	NW_ServerSession*					    m_pkMailServerSession;
	NW_ServerSession*					    m_pkChatServerSession;
	NW_ServerSession*					    m_pkTradeServerSession;
	NW_ServerSession*						m_pkSocietyServerSession;
	NW_ServerSession*						m_pkGlobalServerSession;

	NW_ServerSessionMap						m_kGame2GatewaySessions;
	NW_ServerSessionMap						m_kGame2GameSessions;

	float									m_fLastDBServerSessionUpdateTime;
	float									m_fLastLoginServerSessionUpdateTime;
	float									m_fLastGMLogicUpdateTime;
	float									m_fLastGroupServerSessionUpdateTime;
	float									m_fLastNetDataOutputSaveTime;

	//本身Ip和对client的监听port
	int32_t									m_iServerEntryId;	// GS所属群，比如2服
	Protocol::PS_ServerInfo					m_kServerInfo;
	Protocol::PS_ServerInfo					m_kLoginServerInfo;
	Protocol::PS_ServerInfo					m_kDBServerInfo;
	Protocol::PS_ServerInfo					m_kGroupServerInfo;
	Protocol::PS_ServerInfo					m_kMailServerInfo;
	Protocol::PS_ServerInfo					m_kChatServerInfo;
	Protocol::PS_ServerInfo					m_kTradeServerInfo;
	Protocol::PS_ServerInfo					m_kSocietyServerInfo;
	Protocol::PS_ServerInfo					m_kGlobalServerInfo;

	mem::vector<Protocol::PS_ServerInfo>	m_gatewayInfos;
	mem::vector<Protocol::PS_ServerInfo>	m_GameServerInfos;
	int										m_iGameServerTimeOut;
	int										m_iGameServerAcceptCount;
	int										m_iClientVersionLimit;

	mem::vector<redis_info>					m_kLocalRediss;
	mem::vector<redis_info>					m_kMonitorRediss;
	redis_info								m_kGlobalRedis;

	//message 2 client
	MC_Message2ClientManager*				m_message2Client;

	NW_MessageRecorder						m_messageRecorder;
	MulticastMsgHelper						m_multiMsgHelper;

	LC_DailyDataReport						m_kDailyDataReport;

	bool									m_bBranchNode;
	//新手分流配置
	bool									m_bUseNewPlayerMap;
	int										m_iNewPlayerCountPerMap;

	bool									m_allow_gm_cmd;
	bool									m_360_privi_gm_test;

	server_union_info						m_server_union_info_;

	GameServerManager						m_ServerManager;
	ExportRobot*							m_ExportRobot;

	Utility::Redis_Handler					m_kGlobalRedisHandler;
	Utility::Redis_Channel					m_kGlobalRedisChannel;
	Utility::Redis_Handler					m_kRedisHandler;
	Utility::Redis_Channel					m_kRedisChannel;
	Utility::Redis_Channel					m_kMonitorRedisChannel;
	std::string								m_sRedisGlobalPrefix;
	std::string								m_sRedisGroupPrefix;
	std::string								m_sRedisServerPrefix;

	bool m_bInitEntryServer;
	std::string								m_sRedisEntryServerPrefix;

	TextFilterChannel						m_kTextFilterChannel;
	AsyHttpClient                           m_kAsyHttpClient;
	LC_HttpHandler*							m_kHttpHandler;

	unique_id_set							m_kBonusRoleSet;

	init_setting							m_initSetting;

	StringType								m_strConfigPath;
	StringType								m_strLogPath;
	StringType								m_strLocalePath;

	std::string								m_strAct360PriviUrl;
	std::string								m_strServerListPrefix;

	std::string								m_sSurviveCheckChanel;
	float									m_fSurviveCheckTime;
	uint32_t								m_uServerPid;

	bool									m_bSyncActiveSucceed;	//活跃值同步完成
	int32_t									m_iPredayActive;		//前一天活跃
	int32_t									m_iPredayCrossActive;	//前一天三联服总活跃

	update_bulletin							m_kUpdateBulletin;		//更新公告
	int32_t  m_nNodeType;

	// bot export operation, tools mode.
	bool m_bToolMode;
	bool m_bExitByToolMode;
	int32_t m_nBotOPCode;
public:
	void SetToolMode( bool bInput );
	bool GetToolMode();

	void SetExitByToolMode(bool bInput);
	bool GetExitByToolMode();

	void SetBotOPCode( int32_t nCode );
	int32_t GetBotOPCode();

	void TryExecuteBotOP();
};

extern void OpenSTDStream();
extern void CloseSTDStream();

#endif
