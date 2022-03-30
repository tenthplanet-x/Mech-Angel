#pragma once

#include "LC_ServerPlayerQueueManager.h"
#include "LC_PlayerBase.h"
#include "UT_TimeManager.h"
#include "MG_Game2Alliance.h"

class LC_ServerPlayer;
class NW_ServerSession;
class LC_ServerWorldManager;
////////////////////////////////////////////////////////////////////////////
enum HoldDataType
{
	HoldDataType_Normal = 0,
	HoldDataType_CrossRealmTeam,		//�������
	HoldDataType_Team,					//���ض���
	HoldDataType_CrossRealmGuildWar,	//����ս
	HoldDataType_CrossRealmRaid,		//�������
	HoldDataType_GuildTransport,		//������
	HoldDataType_FrontierAlliance,		//�߾���ͼ
	HoldDataType_All,
};
////////////////////////////////////////////////////////////////////////////
struct CrossRealmTeam_Player
{
	CrossRealmTeam_Player(): m_charID(), m_playerID(0), m_userID(0), m_SrcServerId(0), m_Enter(false), m_Timestamp(0) {}
	unique_id_impl	m_charID;
	object_id_type  m_playerID;
	user_id_type	m_userID;
	int				m_SrcServerId;
	bool			m_Enter;
	uint64_t		m_Timestamp;
};
////////////////////////////////////////////////////////////////////////////
struct CrossRealmTeamData
{
	CrossRealmTeamData();
	void			dismissLocalTeam();
	void			leaveLocalTeam(LC_ServerPlayer* player);
	void			addLocalTeamMember(LC_ServerPlayer* player);

	int				m_mapID;
	int				m_mapTypeId;
	int				m_GlobalTeamId;
	int				m_LocalTeamId;
	mem::map<object_id_type, CrossRealmTeam_Player> m_PlayerInfo;
};
////////////////////////////////////////////////////////////////////////////
struct LC_UserAccountInfoCache
{
	LC_UserAccountInfoCache(): m_cRoleCount(0), m_uiLoginIP(0),
		m_uiLastLoginIP(0), m_ulClientMAC(0), m_byAdult(0), m_onlineTime(0), m_offlineTime(0),
		 m_bBindPhone(0), m_iLoginTime(0), m_uiCharPropellID(0), m_fTimestamp(0), m_userKey(0)
	{
		m_szAccount[0] = '\0';
	}

	void UpdateUserInfo(LC_ServerPlayer* pkPlayer);

	uint8_t		m_cRoleCount;
	uint32_t	m_uiLoginIP;
	uint32_t	m_uiLastLoginIP;
	uint64_t	m_ulClientMAC;
	char		m_szAccount[MAX_ACCOUNT_NAME_LENGTH + 1];
	int8_t		m_byAdult;
	uint32_t	m_onlineTime;			//��λ��
	uint32_t	m_offlineTime;			//��λ��
	uint8_t		m_bBindPhone;
	int32_t		m_iLoginTime;
	uint64_t	m_uiCharPropellID;
	float		m_fTimestamp;
	int64_t		m_userKey;
	uint16_t m_nLoginPlatform;
	uint16_t m_nLoginPlatformPage;
};
////////////////////////////////////////////////////////////////////////////
class LC_ServerPlayerEnterLeaveManager : public Memory::MM_BaseObject
{
public:
	struct OfflinePlayerStruct
	{
		OfflinePlayerStruct();
		LC_ServerPlayer* GetPlayer();
		object_id_type	m_playerID;
		unique_id_impl	m_charID;
		float			m_maxOfflineTime;
	};

	struct UserToken
	{
		UserToken(void) 
		{
			memset(this, 0, sizeof(UserToken));
		}
		user_id_type	m_userID;
		object_id_type	m_playerID;
		unique_id_impl	m_charID;
		float			m_addTime;
		float			m_holdTime;
		HoldDataType	m_holdType;
		uint64_t		m_Parm1;
		int				m_Parm2;
		int				m_Parm3;
	};

	struct CrossRealmData
	{
		CrossRealmData() 
		{
			memset(this, 0, sizeof(CrossRealmData));
		}
		user_id_type	m_userID;
		object_id_type	m_playerID;
		unique_id_impl	m_charID;
		CrossRealmType	m_CrossRealmType;
		uint64_t				m_Parm1;
		int				m_Parm2;
		int				m_Parm3;
		int				m_MapLogicId;
		int				m_TargetServerID;
	};

	struct ChgPositionInfo
	{
		ChgPositionInfo(void) 
		{
			memset(this, 0, sizeof(ChgPositionInfo));
		}
		unique_id_impl	m_charID;
		int				m_iMapResID;
		bool			m_bPos;
		Utility::UT_Vec3Int m_rTargetPos;
		int32_t			m_nType;
		int32_t			m_nIndex;
	};
	/// ���׼�����ߣ����͹�����׼������
	struct PlayerPrepareSwitchLineArg
	{
		int32_t		m_eInOutType;
		int32_t		m_iMapResID;
		int32_t		m_iMapLogicID;
		float		m_fReqTime;

		PlayerPrepareSwitchLineArg() { 
			m_eInOutType = m_iMapResID = 0; m_iMapLogicID = 0; m_fReqTime = 0;
		}
		PlayerPrepareSwitchLineArg(int32_t eInOutType, int32_t iMapResID, int32_t iMapLogicID) { 
			m_eInOutType = eInOutType;
			m_iMapResID = iMapResID; 
			m_iMapLogicID = iMapLogicID;
			m_fReqTime = 0; }
	};

	typedef mem::list<CrossRealmData> CrossRealmDataList;
	struct CrossRealmDataGroup
	{
		CrossRealmData	m_data;
		CrossRealmDataList m_doList;
	};

	LC_ServerPlayerEnterLeaveManager(void);
	~LC_ServerPlayerEnterLeaveManager(void);

	void	Update(float fCurrentTime, float fDeltaTime);

	//�û�����GameServer
	void	UserEnterServer(user_id_type userID, const unique_id_impl& charID, NW_ServerSession* session);
	//�û��뿪GameServer
	//nError: // ServerKickUserReason Begin // ServerKickUserReason End // SystemError.h
	void	UserLeaveServer(LC_ServerPlayer* player, GameLogic::LC_MapInOutType eInOutType, ResultType nError = RE_SUCCESS,bool bSendlogin = true);

	bool    DestoryPlayerNotSaveDB(LC_ServerPlayer* player, GameLogic::LC_MapInOutType eOutType);
	//�û������뿪GameServer,ת��������Ϸ������
	void	UserJumpServer(LC_ServerPlayer* player, int32_t serverID, GameLogic::LC_MapInOutType eInOutType);
	//���������û�
	void	AllUserLeaveServer();
	//��ҳɹ�������Ϸ
	void	PlayerEnterGame(LC_ServerPlayer* pkPlayer, GameLogic::LC_MapInOutType eInType);

	//���ӿ����������
	void	AddCrossRealmTeamPlayerInfo(mem::vector<CrossRealmTeam_Player>& playerData, int mapid, int maptype, int teamid);
	void	DelCrossRealmTeamPlayerInfo(int teamid, object_id_type playerid, bool notify, bool bClearCross = true);
	void	DelCrossRealmTeamAllPlayerInfo(int teamid, LC_ServerPlayer* player, bool notify);
	void DelCrossRealmTeam(int teamid);
	bool	CrossRealmTeamPlayerEnter(int teamid, LC_ServerPlayer* player);
	bool	CrossRealmTeamPlayerLeave(int teamid, LC_ServerPlayer* player, bool notify = true, bool back = true);
	CrossRealmTeamData* GetCrossRealmTeam(uint64_t teamid);

	//����л�ս
	void	AddCrossRealmGuildWarPlayerInfo(const unique_id_impl& charID, user_id_type userid, int maplogicid);
	void	DelCrossRealmGuildWarPlayerInfo(object_id_type playerid);
	bool	CrossRealmGuildWarPlayerEnter(LC_ServerPlayer* player);
	bool	CrossRealmGuildWarPlayerLeave(LC_ServerPlayer* player, bool back = true);

	//�л��ټ���
	void	AddGuildTransportPlayerInfo(const unique_id_impl& charID, user_id_type userid, uint64_t guildid, int transportid, int maplogicid);
	void	DelGuildTransportPlayerInfo(object_id_type playerid);
	bool	GuildTransportPlayerEnter(LC_ServerPlayer* player);
	bool	GuildTransportPlayerLeave(LC_ServerPlayer* player, bool back = true);

	//�������
	void	AddCrossRealmRaidPlayerInfo(const unique_id_impl& charID, user_id_type userid, int raidid, int maplogicid);
	void	DelCrossRealmRaidPlayerInfo(object_id_type playerid);
	bool	CrossRealmRaidPlayerEnter(LC_ServerPlayer* player);
	bool	CrossRealmRaidPlayerLeave(LC_ServerPlayer* player, bool back = true);

	bool	IsInCrossRealmIndex(object_id_type playerid);
	bool	IsInCrossRealmIndex(const unique_id_impl& id);
	bool	CheckCrossRealmIndex(object_id_type playerid, CrossRealmDataGroup& data);
	bool	CheckCrossRealmIndex(object_id_type playerid, CrossRealmData& data);
	bool	CheckCrossRealmType(object_id_type playerid, CrossRealmType& crossrealmtype);
	bool	AddCrossRealmIndex(const unique_id_impl& charID, user_id_type userid, CrossRealmType crossrealmtype, int maplogicid, uint64_t parm1 = 0, int parm2 = 0, int parm3 = 0);
	void	SetCrossRealmTargetServer(object_id_type playerid, int serverId);
	void	ClearCrossRealmIndex(object_id_type playerid);
	void	ClearCrossRealmIndex(const unique_id_type& charID);

	void	PlayerTryEnterCrossRealm(LC_ServerPlayer* player, int targetserver);
	bool	PlayerTryLeaveCrossRealm(LC_ServerPlayer* player,bool notify=true,bool back=true);
	void	CrossRealmEnd(CrossRealmType crossrealmtype, uint64_t parm1 = 0, int parm2 = 0, int parm3 = 0);
	
	//�߾���ͼ
	ResultType	PlayerTryEnterAlliance(LC_ServerPlayer* player, int32_t nServerID, int32_t nAreaID, bool bSendEnter = false);
	bool	AddFrontierAlliancePlayerInfo(const unique_id_impl& charID, user_id_type userID, int32_t nAreaID);
	bool	DelFrontierAlliancePlayerInfo(object_id_type playerID);
	void	ClearFrontierAlliancePlayerInfo(object_id_type playerID);
	bool	FrontierAlliancePlayerEnter(LC_ServerPlayer* player);
	bool	FrontierAlliancePlayerLeave(LC_ServerPlayer* player, bool back = true);

	uint32_t	RequstCrossChangeLocation(LC_ServerPlayer* pkPlayer, int nServerID, int iTargetMapResID, const Utility::UT_Vec3Int& rkTargetLocation);
	uint32_t	RequstCrossChangeLocation(LC_ServerPlayer* pkPlayer, int nServerID, int iTargetMapResID, int32_t nType, int32_t nIndex);
	uint32_t	DealCrossChangeLocationRequst(const unique_id_impl& charID, user_id_type userID, const Protocol::MG_Req_SS_Alliance_Back_Server& reqInfo);
	uint32_t	StartCrossChangeLocation(LC_ServerPlayer* pkPlayer, int nTargetServer);
	ResultType	TransferToServerByType(LC_ServerPlayer* pkPlayer, int nMapRes, int32_t nType, int32_t nIndex, bool bForceTribuMap = false, int32_t nLineIndex = 0);

	void	KickOffCrossRealmPlayer(object_id_type playerid);
	void	DelCrossRealmPlayer(object_id_type playerid, bool notify = true);

	//�û�������ȡ
	void	LoadDataFromDB(LC_ServerPlayer* player, bool success);
	void	LoadDataFromLogin(LC_ServerPlayer* player);

	inline LC_UserAccountInfoCache& GetUserInfoCache(const unique_id_impl& charID) { return m_kUserAccoutInfoCacheMap[charID]; }

	//�û�ռλ
	void	AddPlayerHolder(const unique_id_impl& charID, user_id_type userid, float holdTime, HoldDataType holdtype = HoldDataType_Normal, uint64_t parm1 = 0, int parm2 = 0, int parm3 = 0);
	void	UpdatePlayerHolder(const unique_id_impl& charID, user_id_type userid, float holdTime, HoldDataType holdtype);
	void	UpdatePlayerHolder(object_id_type playerID, float holdTime, HoldDataType holdtype);
	UserToken* GetPlayerHold(object_id_type playerID);
	void	ClearPlayerHold(object_id_type playerID, HoldDataType holdtype = HoldDataType_All);
	void	ClearPlayerHold(const unique_id_type& charID, HoldDataType holdtype = HoldDataType_All);
	int		GetHoldCount(HoldDataType holdtype);

	void	AddCharPositionHold(const unique_id_impl& charID, int nMapRes, const Utility::UT_Vec3Int& rPos);
	void	AddCharPositionHold(const unique_id_impl& charID, int nMapRes, int32_t nType, int32_t nIndex);

	bool AddSwitchMapLinePlayer( user_id_type nUserId, const PlayerPrepareSwitchLineArg & arg );	/// login֪ͨ��������׼�����߹���
	void DelSwitchMapLinePlayer( user_id_type nUserId );
	bool CheckSwitchMapLinePlayer( user_id_type nUserId );	/// ����Ƿ���������ҵ�¼
	void UpdateSwitchMapLinePlayer( float fDeltaTime );		/// ��������������󣬿����Ƿ����

	void AddOnlinePlayer( unique_id_type nCharId );
	void DelOnlinePlayer( unique_id_type nCharId );
	bool CheckOnlinePlayer( unique_id_type nCharId );

	void	ClearCharPositionHold(const unique_id_impl& charID);
	ChgPositionInfo* GetCharPositionHold(const unique_id_impl& charID);

	object_id_type QueryPlayerID(const unique_id_impl& charID);
	unique_id_type QueryCharID(object_id_type playerID);

	void SyncCrossData(const unique_id_impl& charID);

	//����������״��
	bool	IsServerFull();
	int		GetServerPlayerCount();
	int		GetServerRealPlayerCount();
	int		GetServerOnlinePlayerCount();
	int		GetWaitingCount();

	LC_ServerPlayer* ClonePlayer(LC_ServerPlayer* Owner, user_id_type userID, object_id_type objectID, const unique_id_impl& charID, uint32_t bits, bool isRobot);
	ResultType TrySwitchMapLine( LC_ServerPlayer* pkPlayer, int32_t nMapResId, int32_t nMapLogicId, int32_t nLineIndex );
private:
	ResultType _checkSwitchMapLine( LC_ServerPlayer* pkPlayer, int32_t nMapResId, int32_t nMapLogicId, bool bForceTribuMap );
	ResultType _trySwitchMapLine( LC_ServerPlayer* pkPlayer, int32_t nMapResId, int32_t nMapLogicId, int32_t nLineIndex );

	bool	_doUserLeaveImpl(LC_ServerPlayer* player, GameLogic::LC_MapInOutType eOutType, ResultType nError = RE_SUCCESS);
	void    _userLeaveServer(LC_ServerPlayer* player);				//�û��˳���Ϸ
	void	_userResetServer(LC_ServerPlayer* player,bool bSendlogin);				//�û�����ѡ�˽���
	void	_userJumpServer(LC_ServerPlayer* player);				//�û�����
	void	_userOffLineServer(LC_ServerPlayer* player, bool bForceLeave);		//�û�����
	void	_userKickOffServer(LC_ServerPlayer* player, ResultType nError = RE_SUCCESS);			//�û�����
	void	_userBeKickedOffServer(LC_ServerPlayer* player, ResultType nError = RE_SUCCESS);		//�û���login��������
	void	_userBeKickedException(LC_ServerPlayer* player);		//�û���Ϊ�߼��쳣���߳�
	void	_userCleanUpServer(LC_ServerPlayer* player);			//�û��������߼���������
	void    _userQuitArenaServer(LC_ServerPlayer* player);			//�û��˳�������
	void	_userGiveUpArenaServer(LC_ServerPlayer* player);		//�û��жϾ�����
	void    _userDelayKickoffServer(LC_ServerPlayer* player);		//�û��ڸ��¹����б�������
	LC_ServerPlayer* _findOfflinePlayer(const unique_id_impl& charID);		//�������
	void	_addOfflinePlayer(const unique_id_impl& charID, LC_ServerPlayer* player);
	void	_removeOfflinePlayer(const unique_id_impl& charID);
	void	_checkOfflinePlayer(float fCurrentTime, float fDeltaTime);

	void	_checkHolder(float fCurrentTime, float fDeltaTime);		//ռλ���
	bool	_holderPlayerEnter(LC_ServerPlayer* player);

	bool	_addWaitingPlayer(LC_ServerPlayer* player);				//�Ŷ����
	void	_checkWaiting(float fCurrentTime, float fDeltaTime);

	void	_pullDataFromLogin(LC_ServerPlayer* player);			//��Login��ȡ�˻�����
	void	_pullDataFromLoginSuccess(LC_ServerPlayer* player);
	void	_checkPullLoginData(float fCurrentTime, float fDeltaTime);

	void	_pullDataFromDB(LC_ServerPlayer* player);				//��DB��ȡ��ɫ����
	void	_pullDataFromDBSuccess(LC_ServerPlayer* player);
	void	_updatePullDataFromDB(float fCurrentTime, float fDeltaTime);

	void	_checkPlayerHeartbeat(float fCurrentTime, float fDeltaTime);	//�������

	bool	_enterOffLinePlayer(user_id_type userID, const unique_id_impl& charID, NW_ServerSession* session);		//��ҽ��뷽ʽ
	bool	_enterOnlinePlayer(user_id_type userID, const unique_id_impl& charID, NW_ServerSession* session);
	bool	_enterHolderPlayer(user_id_type userID, const unique_id_impl& charID, NW_ServerSession* session);
	bool	_enterNormalPlayer(user_id_type userID, const unique_id_impl& charID, NW_ServerSession* session);
	bool	_enterBuddyPlayer(user_id_type userID, const unique_id_impl& charID, LC_ServerPlayer* pOwner);

	void	_notifyOtherServerUserOn(user_id_type userID, const unique_id_impl& charID, int32_t gatewayId, int32_t realmid);	//����֪ͨ���״̬
	void	_notifyOtherServerUserOff(user_id_type userID, const unique_id_impl& charID, int32_t gatewayId);
	void	_notifyOtherServerUserOffWithoutLogin(user_id_type userID, const unique_id_impl& charID, int32_t gatewayId);
public:	
	void	_notifyOtherServerUserReset(user_id_type userID, const unique_id_impl& charID, int32_t gatewayId);
private:
	void	_notifyOtherServerUserJump(user_id_type userID, const unique_id_impl& charID, int32_t gatewayId);


	void	_removePlayerFromServerWorld(LC_ServerPlayer* player, GameLogic::LC_MapInOutType eOutType);		//��Ҵ��������뿪
	bool	_checkServerIDAndTransServer(LC_ServerPlayer* player);
	void	_checkCrossRealmTeamPlayerList(float fCurrentTime, float fDeltaTime);
	void	_checkDelayKickoffPlayerList(float fCurrentTime, float fDeltaTime);

private:

	typedef mem::map<object_id_type, UserToken>					UserPlaceHolder;
	typedef mem::list<UserToken>								PullDataWaitingQueue;
	typedef mem::map<object_id_type, bool>						UserPullLoginMap;
	typedef mem::map<uint64_t, CrossRealmTeamData>		CrossRealmTeamPlayDataMap;
	typedef mem::map<object_id_type, CrossRealmDataGroup>		CrossRealmPlayIndexMap;

	typedef mem::map<object_id_type, PlayerPrepareSwitchLineArg> SWITCH_MAP_LINE_PLAYER_MAP;
	typedef mem::set<unique_id_type> ONLINE_PLAYER_SNAPSHOT;

	typedef mem::map<unique_id_type, LC_UserAccountInfoCache>	UserAccoutInfoCacheMap;
	typedef mem::map<unique_id_type, OfflinePlayerStruct>		OfflinePlayerMap;
	typedef mem::map<unique_id_type, object_id_type>			CharID2PlayerIDIndices;
	typedef mem::map<object_id_type, unique_id_type>			PlayerID2CharIDIndices;

	typedef mem::map<unique_id_type, ChgPositionInfo>			CharPositionHoldMap;
private:
	OfflinePlayerMap				m_offlinePlayers;
	UserPlaceHolder					m_placeHolders;
	UserPlaceHolder					m_pullLoginDataHolders;
	PullDataWaitingQueue			m_pullDBDataQueue;
	UserPullLoginMap				m_loginInUsers;
	LC_ServerPlayerQueueManager		m_waitingQueues;
	float							m_lastCheckOfflineTime;
	float							m_lastCheckPullLoginDataTime;
	float							m_lastCheckPullDataTime;
	float							m_lastCheckHoldTime;
	float							m_lastCheckWaitingTime;
	float							m_lastCheckHeartBeatTime;
	float							m_lastCheckAllowEnterPlayerTime;
	float							m_lastCheckDelayKickoffPlayerTime;

	CrossRealmTeamPlayDataMap		m_CrossRealmTeamPlayInfoMap;

	SWITCH_MAP_LINE_PLAYER_MAP m_SwitchMapLinePlayerSet;
	ONLINE_PLAYER_SNAPSHOT m_OnlinePlayerSnapshot;

	CrossRealmPlayIndexMap			m_CrossRealmPlayIndex;
	object_id_vector				m_kDelayKickOffPlayerIDList;
	UserAccoutInfoCacheMap			m_kUserAccoutInfoCacheMap;
	Utility::UT_Timer				m_kUpdateTimer;
	
	CharID2PlayerIDIndices			m_kCharID2PlayerIDIndices;
	PlayerID2CharIDIndices			m_kPlayerID2CharIDIndices;
	CharPositionHoldMap				m_kCharIDPositionHold;
	static const int				m_pullDataPerTimeCount = 50;
	static const size_t				m_allowQueueLength = 400;
};

