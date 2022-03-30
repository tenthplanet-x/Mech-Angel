#pragma  once
//����ͻ��� session������ login �� game sessions
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
	CIT_ITEM_TYPE_START = 0,		//���ڿ�����Ʒ���͵ķ�Χ
	CIT_HEAD_EQUIP,
	CIT_BODY_EQUIP,
	CIT_FOOT_EQUIP,
	CIT_HEAD_PIC,
	CIT_WEAPON,
	CIT_ITEM_TYPE_END,				//���ڿ�����Ʒ���͵ķ�Χ
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
	// ������cmmcommon.json���õ��û���¼
	LOGINMODE_LIMIT = 0,
	// ����ģʽ��ǿ�Ƶ�¼
	LOGINMODE_NORMAL	=	1,
	// ԭ���ֵ���֤��¼
	LOGINMODE_ONLINE	=	2,
	// 360 ���е�360ģʽ��¼
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


	//��ɫ�б�Ĵ�ȡ
	void								AddCharacter(PS_CharViewInfo& rkCharacter);
	void								DeleteCharacter(const unique_id_impl& iCharID, int iDeleteTime);	//ɾ���û��µ�һ����ɫ����ʼɾ����ʱ
	void								DeleteCharacter(const unique_id_impl& iCharID);					//����ɾ���û��µ�һ����ɫ
	void								ClearCharacters();								//ɾ���û������еĽ�ɫ
	void								RecoverCharacter(const unique_id_impl& iCharID);					//�ָ���ʱɾ�����û���ɫ
	PS_CharViewInfo*					GetCharacter(const unique_id_impl& iCharID);
	PS_CharBaseInfoList&				GetCharList();

	//��ǰʹ�õ�GameServer��ȡ
	int						GetCurGameServer()				{return m_nCurGameServerID;};
	void					SetCurGameServer(int nServerID)	{m_nCurGameServerID = nServerID;};
	int						GetCurGameServerNode()				{return m_nCurGameServerNodeID;};
	void					SetCurGameServerNode(int nEntryID)	{m_nCurGameServerNodeID = nEntryID;};

	//��ǰѡ�еĽ�ɫ��ȡ
	const unique_id_impl&						GetSelectChar()					{return m_nSelectCharID;};
	void					SetSelectChar(const unique_id_impl& nCharID)		{m_nSelectCharID = nCharID;};

	uint32_t				GetUserID()						{return m_dwUserID;};
	void					SetUserID(uint32_t uiUserID)	{m_dwUserID = uiUserID;};

	//������ɫʱ��Ʒ�Ĵ�ȡ
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

	//ˢ���Ŷ���Ϣ
	void					UpdateWaitEnterGame(float fCurrentTime);
	//�ȴ�ת��
	//void					UpdateChangeServer(float fCurrentTime);
	//�Ŷ�������¼
	void					SetLastWaitUserSyncCount(int iUserCount){m_iLastWaitUserCount = iUserCount;};
	void					SetLastWaitUserSyncTime(float fTime){m_fLastWaitUserCountTime = fTime;};
	int						GetLastWaitUserSyncCount(){return m_iLastWaitUserCount;};
	float					GetLastWaitUserSyncTime(){return m_fLastWaitUserCountTime;};
	//ѡ�����
	void					SetCurCitizenShip(int iCitizenShip){m_iCurCitizenShip = iCitizenShip;};
	int						GetCurCitizenShip(){return m_iCurCitizenShip;}
	//��Ϸ�ڱ�־
	void					SetInGameFlag(bool bInGameFlag){m_bInGameFlag = bInGameFlag;};
	bool					GetInGameFlag(){return m_bInGameFlag;};

	//��½IP��¼
	void					SetLastLoginIP(const StringType& rkIP){m_kLastLoginIP = rkIP;};
	void					SetCurLoginIP(const StringType& rkIP){m_kCurLoginIP = rkIP;};
	void					SetLastLoginAddress(const StringType& rkAddress){m_kLastLoginAddress = rkAddress;};
	void					SetCurLoginAddress(const StringType& rkAddress){m_kCurLoginAddress = rkAddress;};

	const StringType &		GetLastLoginIP(){return m_kLastLoginIP;};
	const StringType &		GetCurLoginIP(){return m_kCurLoginIP;};
	const StringType &		GetLastLoginAddress(){return m_kLastLoginAddress;};
	const StringType &		GetCurLoginAddress(){return m_kCurLoginAddress;};

	//�������汾��
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

	//�����ɫ�б�
	PS_CharBaseInfoList				m_kCharBaseInfoList;
	//��ǰ��¼ʹ�õ�GameServer ID
	int								m_nCurGameServerID;		// GS Node entry
	int								m_nCurGameServerNodeID;	// GS Node
	//��ǰ��¼ѡ�еĽ�ɫID
	unique_id_impl					m_nSelectCharID;
	//������ɫ�ķ�����ID
	int								m_iCreateCharServerID;
	//���洴������ʱ��Ҫ����Ʒ
	NW_ItemIDList					m_kHeadEquipList;
	NW_ItemIDList					m_kBodyEquipList;
	NW_ItemIDList					m_kFootEquipList;
	NW_ItemIDList					m_kHeadPicList;
	NW_ItemIDList					m_kWeaponList;

	bool							m_bRequestExitGame;

	//��֤��Ϣ
	Protocol::SignatureInfo			m_kSignatureInfo;
	int16_t							m_sSignatureBuffLength;
	char							m_szSigatureBuff[MAX_AUTH_BUFF_LENGTH];

	//����״��
	NW_NetState						m_eNetState;
	float							m_fLastNetStateTime;

	ResultType						m_eMessage;

	//IP��¼
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
		LT_NORMAL	= 1,	//����ģʽ����������ʱѡ��
		LT_FREE		= 2,	//����ģʽ��������Ϸʱ����ѡ��
		LT_TWO_PORT = 3,	//��2ר�ã� ���ֵ�����ͨ���
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

	//Begin �������б�Ĵ�ȡ
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

	//End �������б�Ĵ�ȡ
	
	//���ûỰ
	bool							ResetSessionGroup(NetWork::NW_NetSessionBase* pkSession);

	
	bool							ActivateGameSession(NW_Login2ClientSession* pkLoginSession, const PS_GameSvrInfo* pkGameServerInfo);
    bool							ExitGameSession(NW_Game2ClientSession* pkClientGameSession);
	
	NW_ClientSessionGroup*			GetSessionGroup(NetWork::NW_NetSessionBase* pkSession);
	NW_ClientSessionGroup*			GetGameSessionGroup();

	NW_NetCharSimpleInfo*			FindNetCharSimpleInfo(const StringType& rkCharName, bool bNeedReverse = false);
	void							AddNetCharSimpleInfo(const StringType& rkCharName,const NW_NetCharSimpleInfo& rkInfo);
	
	
	PS_GameSvrInfo			GetGateWayInfoByUserID(int iLoginServerID,long lUserID);
	PS_GameSvrInfo			GetGateWayInfoByUserID(char *szIP, int iPort, long lUserID);

	//�Ƿ�ʹ��CASƽ̨��¼
	bool							GetUseCAS(){return m_bUseCAS;};

	//serverunion
	void					SetServerUnionID(int iUnion) { m_iServerUnionID = iUnion; }
	int						GetServerUnionID(){return m_iServerUnionID;}

protected:
	//�����Ƽ�������״̬
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

	//������Ե�LoginServer�б�
	PS_ServerInfoList				m_kLoginZoneInfoList;
	mem::map<int, PS_ServerInfoList> m_kLoginServerInfoMap;
	mem::map<int, std::pair<int, int>> m_kLoginServerIDMap;

	//������õ�GameServer�б�
	PS_ServerInfoList				m_kGameServerInfoList;

	//�����¼���̵����� �����Ժ��м�汾�����ã�
	int								m_iLoginType;

	//�����ѯ�������������Ϣ
	NW_NetCharSimpleInfoMap			m_kNetCharSimpleInfoMap;
	
	//�������ѯ�����������Ϣ
	TStringList						m_kWaitProfileNetCharNameList;
	float							m_fLastUpdateProfileTime;
	
	//���·������б�
	float							m_fLastUpdateServerListTime;

	//�Ƿ�ʹ��CASƽ̨
	bool							m_bUseCAS;

	//serverunion
	int								m_iServerUnionID;

private:
	static int _disconnectFunc(int reason, void* param);
};
#define GER_SERVER_UNION_ID	NW_ClientSessionManager::GetSingletonPtr()->GetServerUnionID