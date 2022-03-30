#pragma  once
//管理客户端 session，包括 login 和 game sessions
#include "MG_MessageHandlerInterface.h"
#include "LC_PackStruct.h"

class SM_GameStageMachine;
class NW_ClientSessionFactory;
class NW_Login2ClientSession;
class NW_Game2ClientSession;

struct PS_CharViewInfo : public Protocol::PS_CharBaseInfoLite
{
	int                                     m_LastMapResID;
	int										m_curMapResID;
	int										m_powerGuild;
	int										m_crossPowerGuild;
	byte									m_Tutorial[MG_TUTORIALDATA_LENGTH];

	PS_CharViewInfo():PS_CharBaseInfoLite()
	{
		m_LastMapResID = 0;
		m_curMapResID = 0;
		m_powerGuild = 0;
		m_crossPowerGuild = 0;
		memset(m_Tutorial, 0, sizeof(m_Tutorial));
	}

	PS_CharViewInfo(const PS_CharViewInfo& rv)
	{
		m_uiCharID			= rv.m_uiCharID;
		m_byCharType		= rv.m_byCharType;
		memcpy(m_szCharName,rv.m_szCharName,sizeof(m_szCharName));
		m_byCareer			= rv.m_byCareer;
		m_byWeapon			= rv.m_byWeapon;
		m_usCharLevel		= rv.m_usCharLevel;
		m_llCharExp			= rv.m_llCharExp;
		m_llYuanBaoPiao		= rv.m_llYuanBaoPiao;
		m_llCash			= rv.m_llCash;
		//m_llBindCash		= rv.m_llBindCash;
		m_byDeleteFlag		= rv.m_byDeleteFlag;
		m_iDeleteTime		= rv.m_iDeleteTime;
		m_iCitizenship		= rv.m_iCitizenship;
		m_iRegTime			= rv.m_iRegTime;
		m_iLastOfflineTime	= rv.m_iLastOfflineTime;
		m_LastMapResID = rv.m_LastMapResID;
		m_curMapResID		= rv.m_curMapResID;
		m_powerGuild		= rv.m_powerGuild;
		m_crossPowerGuild	= rv.m_crossPowerGuild;
		memcpy(m_Tutorial,rv.m_Tutorial,sizeof(m_Tutorial));
	}
	PS_CharViewInfo& operator = (const Protocol::PS_CharBaseInfoLite& rv)
	{
		m_uiCharID			= rv.m_uiCharID;
		m_byCharType		= rv.m_byCharType;
		memcpy(m_szCharName,rv.m_szCharName,sizeof(m_szCharName));
		m_byCareer			= rv.m_byCareer;
		m_byWeapon			= rv.m_byWeapon;
		m_usCharLevel		= rv.m_usCharLevel;
		m_llCharExp			= rv.m_llCharExp;
		m_llYuanBaoPiao		= rv.m_llYuanBaoPiao;
		m_llCash			= rv.m_llCash;
		//m_llBindCash		= rv.m_llBindCash;
		m_byDeleteFlag		= rv.m_byDeleteFlag;
		m_iDeleteTime		= rv.m_iDeleteTime;
		m_iCitizenship		= rv.m_iCitizenship;
		m_iRegTime			= rv.m_iRegTime;
		m_iLastOfflineTime	= rv.m_iLastOfflineTime;
		return *this;
	}
	PS_CharViewInfo& operator = (const PS_CharViewInfo& rv)
	{
		m_uiCharID			= rv.m_uiCharID;
		m_byCharType		= rv.m_byCharType;
		memcpy(m_szCharName,rv.m_szCharName,sizeof(m_szCharName));
		m_byCareer			= rv.m_byCareer;
		m_byWeapon			= rv.m_byWeapon;
		m_usCharLevel		= rv.m_usCharLevel;
		m_llCharExp			= rv.m_llCharExp;
		m_llYuanBaoPiao		= rv.m_llYuanBaoPiao;
		m_llCash			= rv.m_llCash;
		//m_llBindCash		= rv.m_llBindCash;
		m_byDeleteFlag		= rv.m_byDeleteFlag;
		m_iDeleteTime		= rv.m_iDeleteTime;
		m_iCitizenship		= rv.m_iCitizenship;
		m_iRegTime			= rv.m_iRegTime;
		m_iLastOfflineTime	= rv.m_iLastOfflineTime;
		m_LastMapResID		= rv.m_LastMapResID;
		m_curMapResID		= rv.m_curMapResID;
		m_powerGuild		= rv.m_powerGuild;
		m_crossPowerGuild	= rv.m_crossPowerGuild;
		memcpy(m_Tutorial,rv.m_Tutorial,sizeof(m_Tutorial));
		return *this;
	}
};

struct PS_GameSvrInfo : public Protocol::PS_ServerInfo
{
	std::vector<uint16_t>           m_kSideline;
	int								m_iGroupID;

	uint32_t						m_iServerOpenTime;
	uint32_t						m_iServerConflateTime;
	PS_GameSvrInfo():PS_ServerInfo()
	{
		m_iServerID = 0;
		m_szIP = "";
		m_iPort = 0;
		m_iUserCount = 0;
		m_iCapacity = 0;
		m_iGroupID = 0;
		m_iType = 0;
		m_szName = "";
		m_iWaitCount = 0;
		m_cStatus = 0;
		m_kSideline.clear();
		m_iServerOpenTime = 0;
		m_iServerConflateTime = 0;
	}
};

typedef mem::vector<PS_GameSvrInfo>		PS_ServerInfoList;
typedef PS_ServerInfoList::iterator					PS_ServerInfoListIter;
typedef mem::vector<PS_CharViewInfo>		PS_CharBaseInfoList;

enum NW_CharItemType
{
	CIT_ITEM_TYPE_START = 0,		//用于控制物品类型的范围
	CIT_HEAD_EQUIP,
	CIT_BODY_EQUIP,
	CIT_FOOT_EQUIP,
	CIT_HEAD_PIC,
	CIT_WEAPON,
	CIT_ITEM_TYPE_END,				//用于控制物品类型的范围
};
typedef mem::vector<long>							NW_ItemIDList;


enum NW_NetState
{
	NLS_NONE,
	NLS_WAIT_SIGNATURE,
	NLS_FAIL_SIGNATURE,
	NLS_WAIT_VERIFY,
	NLS_FAIL_VERIFY,
	NLS_WAIT_LOGIN,
	NLS_FAIL_LOGIN,
	NLS_WAIT_SERVER_LIST,
	NLS_FAIL_SERVER_LIST,
	NLS_WAIT_GAME_SERVER,
	NLS_FAIL_GAME_SERVER,
	NLS_WAIT_CHAR_LIST,
	NLS_FAIL_CHAR_LIST,
	NLS_WAIT_CREATE_CHAR,
	NLS_FAIL_CREATE_CHAR,
	NLS_WAIT_DELETE_CHAR,
	NLS_FAIL_DELETE_CHAR,
	NLS_WAIT_RECOVER_CHAR,
	NLS_FAIL_RECOVER_CHAR,
	NLS_WAIT_ENTER_GAME,
	NLS_HANGING_ENTER_GAME,
	NLS_FAIL_ENTER_GAME,
	NLS_WAIT_CHANGE_SERVER,
	NLS_FAIL_CHANGE_SERVER,
	NLS_MAX_COUNT,
};

enum NW_LoginMode
{
	// 仅允许cmmcommon.json配置的用户登录
	LOGINMODE_LIMIT = 0,
	// 开发模式，强制登录
	LOGINMODE_NORMAL	=	1,
	// 原武林的认证登录
	LOGINMODE_ONLINE	=	2,
	// 360 已有的360模式登录
	LOGINMODE_360	=	3,
};

class NW_ClientSessionGroup : public Memory::MM_BaseObject
{
public:
	NW_ClientSessionGroup();
	~NW_ClientSessionGroup();

	void								UpdateNetState(float fCurrentTime);
	void								SetNetState(int iNetState);
	int									GetNetState();


	//角色列表的存取
	void								AddCharacter(PS_CharViewInfo& rkCharacter);
	void								DeleteCharacter(const unique_id_impl& iCharID, int iDeleteTime);	//删除用户下的一个角色，开始删除计时
	void								DeleteCharacter(const unique_id_impl& iCharID);					//永久删除用户下的一个角色
	void								ClearCharacters();								//删除用户下所有的角色
	void								RecoverCharacter(const unique_id_impl& iCharID);					//恢复暂时删除的用户角色
	PS_CharViewInfo*					GetCharacter(const unique_id_impl& iCharID);
	PS_CharBaseInfoList&				GetCharList();

	//当前使用的GameServer存取
	int						GetCurGameServer()				{return m_nCurGameServerID;};
	void					SetCurGameServer(int nServerID)	{m_nCurGameServerID = nServerID;};
	int						GetCurGameServerNode()				{return m_nCurGameServerNodeID;};
	void					SetCurGameServerNode(int nEntryID)	{m_nCurGameServerNodeID = nEntryID;};

	//当前选中的角色存取
	const unique_id_impl&						GetSelectChar()					{return m_nSelectCharID;};
	void					SetSelectChar(const unique_id_impl& nCharID)		{m_nSelectCharID = nCharID;};

	uint32_t				GetUserID()						{return m_dwUserID;};
	void					SetUserID(uint32_t uiUserID)	{m_dwUserID = uiUserID;};

	//创建角色时物品的存取
	NW_ItemIDList*			GetCreateCharItemList(NW_CharItemType kItemType);
	void					AddCreateCharItem(NW_CharItemType kItemType, long lItemID);
	void					DeleteCreateItem(NW_CharItemType kItemType);
	int						GetCreateCharServerID(){return m_iCreateCharServerID;};
	void					SetCreateCharServerID(int iServerID){m_iCreateCharServerID = iServerID;};

	void					SetMessage(ResultType eMessage)	{m_eMessage = eMessage;};

	GameLogic::AnitWallowState			GetPlayerWallowState();
	void					CheckAntiWallowState(float fCurrentTime);

	void					NotifyAntiWallowStateToGameServer();

	void					NotifyAntiWallowStateToUIInGame();
	void					NotifyHealthinessStateToUIInGame();
	void					NotifyWearinessStateToUIInGame();
	void					NotifyWallowStateToUIInGame();

	void					NotifyAntiWallowStateToUIInLogin();
	void					NotifyHealthinessStateToUIInLogin();
	void					NotifyWearinessStateToUIInLogin();
	void					NotifyWallowStateToUIInLogin();

	//刷新排队信息
	void					UpdateWaitEnterGame(float fCurrentTime);
	//等待转服
	//void					UpdateChangeServer(float fCurrentTime);
	//排队人数记录
	void					SetLastWaitUserSyncCount(int iUserCount){m_iLastWaitUserCount = iUserCount;};
	void					SetLastWaitUserSyncTime(float fTime){m_fLastWaitUserCountTime = fTime;};
	int						GetLastWaitUserSyncCount(){return m_iLastWaitUserCount;};
	float					GetLastWaitUserSyncTime(){return m_fLastWaitUserCountTime;};
	//选择国家
	void					SetCurCitizenShip(int iCitizenShip){m_iCurCitizenShip = iCitizenShip;};
	int						GetCurCitizenShip(){return m_iCurCitizenShip;}
	//游戏内标志
	void					SetInGameFlag(bool bInGameFlag){m_bInGameFlag = bInGameFlag;};
	bool					GetInGameFlag(){return m_bInGameFlag;};

	//登陆IP记录
	void					SetLastLoginIP(const StringType& rkIP){m_kLastLoginIP = rkIP;};
	void					SetCurLoginIP(const StringType& rkIP){m_kCurLoginIP = rkIP;};
	void					SetLastLoginAddress(const StringType& rkAddress){m_kLastLoginAddress = rkAddress;};
	void					SetCurLoginAddress(const StringType& rkAddress){m_kCurLoginAddress = rkAddress;};

	const StringType &		GetLastLoginIP(){return m_kLastLoginIP;};
	const StringType &		GetCurLoginIP(){return m_kCurLoginIP;};
	const StringType &		GetLastLoginAddress(){return m_kLastLoginAddress;};
	const StringType &		GetCurLoginAddress(){return m_kCurLoginAddress;};

	//服务器版本号
	void					SetServerGameVersion(const StringType& version) { m_ServerGameVersion = version;}
	void					SetServerResVersion(const StringType& version) { m_ServerResVersion = version;}
	const StringType&		GetServerGameVersion() { return m_ServerGameVersion; }
	const StringType&		GetServerResVersion() { return m_ServerResVersion; }

	NW_Login2ClientSession*		m_pkLoginSession;
	NW_Game2ClientSession*      m_pkGameSession;

	StringType                  m_strUser;
	StringType                  m_strPwd;
	StringType                  m_strSessionKey;
	DWORD	                    m_dwUserID;
	long						m_lCharID;
	SM_GameStageMachine*	    m_pkGameStageMachine;

	//管理角色列表
	PS_CharBaseInfoList				m_kCharBaseInfoList;
	//当前登录使用的GameServer ID
	int								m_nCurGameServerID;		// GS Node entry
	int								m_nCurGameServerNodeID;	// GS Node
	//当前登录选中的角色ID
	unique_id_impl					m_nSelectCharID;
	//创建角色的服务器ID
	int								m_iCreateCharServerID;
	//保存创建人物时需要的物品
	NW_ItemIDList					m_kHeadEquipList;
	NW_ItemIDList					m_kBodyEquipList;
	NW_ItemIDList					m_kFootEquipList;
	NW_ItemIDList					m_kHeadPicList;
	NW_ItemIDList					m_kWeaponList;

	bool							m_bRequestExitGame;

	//验证信息
	Protocol::SignatureInfo			m_kSignatureInfo;
	int16_t							m_sSignatureBuffLength;
	char							m_szSigatureBuff[MAX_AUTH_BUFF_LENGTH];

	//网络状况
	NW_NetState						m_eNetState;
	float							m_fLastNetStateTime;

	ResultType						m_eMessage;

	//IP记录
	StringType						m_kLastLoginIP;
	StringType						m_kCurLoginIP;
	StringType						m_kLastLoginAddress;
	StringType						m_kCurLoginAddress;

	StringType						m_ServerGameVersion;
	StringType						m_ServerResVersion;

	void			SetFcm(uint8_t isFcm);
	bool			IsFcm();
	uint16_t		GetOnlineTime()const;
	void			SetOnlineTime(uint16_t time);
	bool			IsCalOnlineTime();
	void			SetCanCalOnlineTime(bool flag);

	bool			m_isFcm;
	uint16_t		m_onlineTime;
	bool			m_isOnline;
	float			m_lastCalOnlineTime;
	float			m_fLastCheckEnterGameTime;
	//float			m_fLastUpdateChangeServerTime;
	int				m_iLastWaitUserCount;
	float			m_fLastWaitUserCountTime;
	int				m_iCurCitizenShip;
	bool			m_bInGameFlag;
};

typedef mem::vector<NW_ClientSessionGroup*>     NW_ClientSessionGroupList;
typedef NW_ClientSessionGroupList::iterator		NW_ClientSessionGroupListIter;
typedef mem::map<DWORD, NW_ClientSessionGroup*> NW_ClientSessionGroupMap;
typedef NW_ClientSessionGroupMap::iterator		NW_ClientSessionGroupMapIter;


struct NW_NetCharSimpleInfo
{
	ResultType	m_iProfileResult;
	user_id_type		m_lUserID;
	unique_id_impl	m_lCharID;
};

typedef mem::map<StringType,NW_NetCharSimpleInfo>	NW_NetCharSimpleInfoMap;
typedef NW_NetCharSimpleInfoMap::iterator			NW_NetCharSimpleInfoMapIter;

class NW_ClientSessionManager : public NetWork::NW_NetSessionManager,
							    public MG_MessageHandlerInterface
{
public:
	enum LoginType
	{
		LT_NORMAL	= 1,	//正常模式，创建人物时选服
		LT_FREE		= 2,	//调试模式，进入游戏时可以选服
		LT_TWO_PORT = 3,	//封2专用， 区分电信网通入口
	};

  	NW_ClientSessionManager(void);
	virtual ~NW_ClientSessionManager(void);

    static NW_ClientSessionManager& GetSingleton(void)
    {
        return *(static_cast<NW_ClientSessionManager*>(NW_NetSessionManager::GetSingletonPtr()));
    }
	static NW_ClientSessionManager* GetSingletonPtr(void)
	{
		return static_cast<NW_ClientSessionManager*>(NW_NetSessionManager::GetSingletonPtr());
	}

	bool									Init();
	bool									UnInit();

	virtual bool							RecvMessage(MG_MessageBase& rkMessage);

	//Begin 服务器列表的存取
	void									LoadServerList();
	void									AddLoginServerInfo(int iZoneID, const PS_GameSvrInfo& rkServerInfo);
	void									AddLoginZoneInfo(const PS_GameSvrInfo &rkServerInfo);
	PS_ServerInfoList&						GetLoginServerInfoList(int iZoneID);
	PS_ServerInfoList&						GetLoginZoneInfoList(){return m_kLoginZoneInfoList;}
	PS_GameSvrInfo*							GetLoginServerInfo(int iServerID);
	PS_GameSvrInfo*							GetLoginZoneInfo(int iZoneID);
	void									ClearLoginServerInfo();	
	void									ClearLoginServerStateData();
	void									ClearLoginServerCharData();

	void									AddGameServerInfo(PS_GameSvrInfo& rkServerInfo);
	PS_ServerInfoList&						GetGameServerInfoList();
	PS_GameSvrInfo*							GetGameServerInfo(int iServerID);
	void									ClearGameServerInfo();

	int										GetGameServerInfoIndex(int iServerID,int iIndex);

	int										GetLoginType(){return m_iLoginType;};

	//End 服务器列表的存取
	
	//重置会话
	bool							ResetSessionGroup(NetWork::NW_NetSessionBase* pkSession);

	
	bool							ActivateGameSession(NW_Login2ClientSession* pkLoginSession, const PS_GameSvrInfo* pkGameServerInfo);
    bool							ExitGameSession(NW_Game2ClientSession* pkClientGameSession);
	
	NW_ClientSessionGroup*			GetSessionGroup(NetWork::NW_NetSessionBase* pkSession);
	NW_ClientSessionGroup*			GetGameSessionGroup();

	NW_NetCharSimpleInfo*			FindNetCharSimpleInfo(const StringType& rkCharName, bool bNeedReverse = false);
	void							AddNetCharSimpleInfo(const StringType& rkCharName,const NW_NetCharSimpleInfo& rkInfo);
	
	
	PS_GameSvrInfo			GetGateWayInfoByUserID(int iLoginServerID,long lUserID);
	PS_GameSvrInfo			GetGateWayInfoByUserID(char *szIP, int iPort, long lUserID);

	//是否使用CAS平台登录
	bool							GetUseCAS(){return m_bUseCAS;};

	//serverunion
	void					SetServerUnionID(int iUnion) { m_iServerUnionID = iUnion; }
	int						GetServerUnionID(){return m_iServerUnionID;}

protected:
	//更新推荐服务器状态
	void							_updateRecommendServerInfo(const PS_GameSvrInfo& rkInfo);
	void							_addWaitProfileNetChar(const StringType& rkCharName, bool bNeedReverse);
	void							_updateWaitProfileNetCharList(float fCurrentTime);
	void							_updateServerList(float fCurrentTime);

    void							_eventTimerTick(MG_MessageBase& rkMessage);
	NW_ClientSessionGroup*			_createSessionGroup(void);

    NW_ClientSessionFactory*		m_pkClientSessionFactory;

    StringType						m_strIP;
    int								m_iPort;

    NW_ClientSessionGroupList		m_kClientSessionGroupList;
    NW_ClientSessionGroupMap		m_kClientSessionGroupMap;

	//管理可以的LoginServer列表
	PS_ServerInfoList				m_kLoginZoneInfoList;
	mem::map<int, PS_ServerInfoList> m_kLoginServerInfoMap;
	mem::map<int, std::pair<int, int>> m_kLoginServerIDMap;

	//管理可用的GameServer列表
	PS_ServerInfoList				m_kGameServerInfoList;

	//管理登录流程的类型 （调试和中间版本测试用）
	int								m_iLoginType;

	//管理查询到的网络玩家信息
	NW_NetCharSimpleInfoMap			m_kNetCharSimpleInfoMap;
	
	//管理待查询的网络玩家信息
	TStringList						m_kWaitProfileNetCharNameList;
	float							m_fLastUpdateProfileTime;
	
	//更新服务器列表
	float							m_fLastUpdateServerListTime;

	//是否使用CAS平台
	bool							m_bUseCAS;

	//serverunion
	int								m_iServerUnionID;

private:
	static int _disconnectFunc(int reason, void* param);
};
#define GER_SERVER_UNION_ID	NW_ClientSessionManager::GetSingletonPtr()->GetServerUnionID