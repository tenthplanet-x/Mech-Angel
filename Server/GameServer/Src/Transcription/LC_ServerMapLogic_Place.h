#ifndef _LC_SERVERMAPLOGIC_PLACE_H
#define _LC_SERVERMAPLOGIC_PLACE_H
//����
#include "LC_ServerMapLogicBase.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerMapLogic_PlayBase.h"
#include "LC_ServerMapLogic_ActorManger.h"
#include "CF_TranscriptionRule.h"
#include "UT_CSUnorderedMap.h"
/////////////////////////////////////////////////////////////////////////////
enum FactionType
{
	FACTION_NONE			=   -1,
	FACTION_MIN				=   0,
	FACTION_PLAYER			=	0,
	FACTION_NEUTRAL			=	1,
	FACTION_JUSTICE			=	2,
	FACTION_EVIL			=   3,
	FACTION_RAIDEVIL		=   5,
	FACTION_TARGET			=	6,
	FACTION_RAIDPLAYER		=	7,
	FACTION_NPCPLAYER		=	8,
	FACTION_GUILDGUARD		=	9,
	FACTION_SERVERGUARD		=	10,
	FACTION_DEFENDER		=	11,
	FACTION_DEF_MONSTER		=	12,
	FACTION_ATTACKER		=	13,
	FACTION_ATK_MONSTER		=	14,	
	FACTION_RED				=	15,
	FACTION_GREEN			=	16,
	FACTION_BLUE			=   17,
	FACTION_MAX,

	FACTION_CUSTOM			=	100,
};
/////////////////////////////////////////////////////////////////////////////
#define MONSTER_FACTION_SIZE 3
enum FactionResult
{
	FACTION_RESULT_SUCESS	=   0,
	FACTION_RESULT_FAIL		=	1,
	FACTION_RESULT_DRAW		=	2,
	FACTION_RESULT_NONE		=	3
};
/////////////////////////////////////////////////////////////////////////////
struct LC_ServerMapLogic_Faction
{
	FactionType			m_FactionType;
	int					m_InterId;
	FactionResult		m_Result;
	LC_ServerMapLogic_Faction():m_FactionType(FACTION_PLAYER), m_InterId(0), m_Result(FACTION_RESULT_NONE)
	{}
};

struct LC_ServerMapLogic_Door
{
	LC_ServerMapLogic_Door();
	LC_ServerMapLogic_Door(const LC_ServerMapLogic_Door& other);
	LC_ServerMapLogic_Door& operator =(const LC_ServerMapLogic_Door& other);
	LC_ServerMapLogic_Door(DungeonConfig::DoorEntry& door);
	bool SetDoorState(DoorState state);

	object_id_type		m_ID;
	int					m_TypeId;
	DoorState			m_State;
	UT_Vec3Int			m_Pos;
	int					m_Angel;
	UT_OBB				m_Obb;
};
/////////////////////////////////////////////////////////////////////////////
class LC_ServerMapLogic_Record
{
public:
	LC_ServerMapLogic_Record():m_KillMonsterRecord(0), m_DeadRecord(0), m_starttime(0.0f), m_endtime(0.0f) {}
	void		Init();
	void		UnInit();
	float		getPassTime();
	inline void RecordStarttime(float t) { m_starttime = t; };
	inline void RecordEndtime(float t) { m_endtime = t;};
	inline void RecordMonster(int n) { m_KillMonsterRecord += n;};
	inline void RecordPlayer(int n) { m_DeadRecord += n;};
	int			getScore(int resid, bool& preEnd);
public:
	int					m_KillMonsterRecord;
	int					m_DeadRecord;
	float				m_starttime;
	float				m_endtime;
};
/////////////////////////////////////////////////////////////////////////////
enum PlaceNotifyEvent
{
	PlaceNotifyEvent_Group = 0,
	PlaceNotifyEvent_LogicArea = 1,
	PlaceNotifyEvent_DialogFinish = 2,
};

enum PlaceLogicState
{
	PLACESTATE_UNINITED,
	PLACESTATE_PROCESS,
	PLACESTATE_PREEND,
	PLACESTATE_FAIL,
	PLACESTATE_END
};

enum TransPointType
{
	TransPointType_None = 0,
	TransPointType_Random = 1,
	TransPointType_Point = 2
};
struct GuildTransport
{
	uint64_t guildid;
	int32_t transportid;
	unique_id_impl charid;
};
typedef mem::map<unique_id_type, GuildTransport> GuildTransportMap;
/////////////////////////////////////////////////////////////////////////////
class ObjectFromScriptCache
{
public:
	GameLogic::LC_TranscriptionListEntry kEntry;
	FactionType nFactionType;
};
class LC_ServerMapLogic_Place: public LC_ServerMapLogicBase
{
public:
	typedef mem::set<object_id_type> LongSet;
	typedef mem::map<FactionType, LC_ServerMapLogic_Faction> FactionMap;
	typedef mem::map<int, FactionType> InnerMap;
	typedef mem::vector<LC_ServerPlayer*> PlayerPtrVector;
	typedef mem::vector<LC_ServerMapLogic_Door> DoorVector;
	typedef mem::map< object_id_type, bool> DialoguePlayerMap;
	typedef mem::vector<LC_ServerMapLogic_PlayBase*> PlayVector;
	typedef mem::vector<ObjectFromScriptCache> OBJECT_FROM_SCRIPT_CACHE;
	typedef mem::map<int32_t, OBJECT_FROM_SCRIPT_CACHE> OBJECT_FROM_SCRIPT_CACHE_MAP;
	typedef OBJECT_FROM_SCRIPT_CACHE_MAP::iterator OBJECT_FROM_SCRIPT_CACHE_MAP_IT;
public:
	LC_ServerMapLogic_Place();
	virtual ~LC_ServerMapLogic_Place();

	void	SetTributaryAutoRelease(bool b) { m_bTributaryAutoRelease = b; }
	bool	GetTributaryAutoRelease(void) const { return m_bTributaryAutoRelease; }

	virtual int GetDamageNpcLogicID(){return 0;}
	virtual void SetDamageNpcLogicID(int logicID){}
	virtual void SetWatchTowerState(int storyID, int bossMapID, int towerID, int bossID, int type, int vaildTime, int endTimerID){}
	virtual void SetTowerLocation(int storyID, int towerID, int x, int y, int z){}
	virtual bool CheckAddKillControllerReward(int mapID){return false;}
	virtual bool SetMonsterHP(int charType, int64_t hp, int32_t nMapResId){return false;}
	virtual void UpdateWatchTowerFromSaveState(uint64_t id, std::string str){}
	virtual void SetLoadFlag(){}
	virtual void BroadcastTowerInfo_Lite(ST_Single_Tower_info_lite& input) {}
	virtual void UpdateWatchTowerFromRedis() {}
	virtual void BroadcastWatchTowerInfo() {}
	virtual void SyncWatchTowerInfoToPlayer(LC_ServerPlayer * pkPlayer){}
	virtual attr_value_type GetTotalDamage() {return 0;}

	virtual void					UpdatePlayerDamage(LC_ActorBase* pkPlayer, attr_value_type damage) {}
	virtual attr_value_type	GetPlayerDamage(uint64_t playerID){return 0;}

	//��ʼ�����أ���ʼ����ɺ���Ҽ��ɽ���
	virtual void	Init();
	virtual void	UnInit();
	virtual void	Update(float fCurrentTime, float fDeltaTime);
	//�黹����
	virtual void	Finsh();
	virtual void	PreFinsh();

	//�����淨����
	virtual void	AllPlayEnd();

	virtual bool RestartLogic();

	//�Ƿ��һ���˽���ż���
	void			SetActiveFirstEnter(bool active);
	//�Ƿ��������˶��뿪���Զ��ͷ�
	inline void		SetAutoFinishAllPlayerLeave(bool autofinish) { m_AutoFinish = autofinish;}
	inline void		SetAutoFinishPlayEnd(bool autofinish) { m_AutoFinishPlayEnd = autofinish;}
	inline bool		GetAutoFinishPlayEnd() { return m_AutoFinishPlayEnd; }
	//ʱ��
	void			SetPersistTime(int persisttime);
	void			SetCountDownTime(int countdown);
	virtual int	GetLeftTime();

	virtual void SetControllerState(bool dead);
	virtual bool GetControllerState();

	//��ȡ�����ø���״̬
	virtual void			SetPlaceState(PlaceLogicState flag);
	inline PlaceLogicState	GetPlaceState() {return m_PlaceState;};
	inline bool		IsProcess() {return m_PlaceState == PLACESTATE_PROCESS || m_PlaceState == PLACESTATE_PREEND;}

	virtual Utility::UT_Vec3Int	ComputeEnterLocation(GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_MapInOutType eInOutType, const Utility::UT_Vec3Int& rkTargetLocation);
	//�������Ƿ���Խ���
	virtual ResultType	CheckEnterMap(object_id_type playerID, int level, GameLogic::LC_MapInOutType eInOutType);
	//�������Ƿ���Խ���
	virtual ResultType	CheckEnterMap(GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_MapInOutType eInOutType);
	//�������Ƿ�����뿪
	virtual ResultType	CheckLeaveMap(GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_MapInOutType eInOutType, float fOfflineTime);
	//�����ͼ

	virtual void		NotifyEnterOfflinePlayer(GameLogic::LC_PlayerBase* pkPlayer);
	virtual ResultType	RequestEnterMap(GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_MapInOutType eInOutType, const Utility::UT_Vec3Int& rkTargetLocation);
	////�뿪��ͼ
	virtual ResultType	RequestLeaveMap(GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_MapInOutType eInOutType, CSVFile::CF_WorldMapList::DataEntry* pkDataEntry);
	virtual ResultType	PlayerBeforeQuit(GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_MapInOutType eInOutType, bool bBack);
	ResultType	SetPlayerQuitState(LC_ServerPlayer* pkPlayer);
	virtual uint64_t GetBossGuildID(int storyID, int npcID){return 0;}
	virtual int GetTowerID(int storyID, int npcID){return 0;}
	//��������뿪
	virtual void		AllPlayerLeave();
	virtual void		PlayerEnterEvent(LC_ServerPlayer* player, int32_t playersize);
	//����
	virtual void		LetPlayerLeave(LC_ServerPlayer* pkPlayer);
	virtual void		LetAllPlayerLeave();
	virtual void		ProcessGameEvent(GameLogic::LC_GameEvent* pkEvent);
	//�޳����ж���
	void				ClearAllObjectsInMapWhenQuit();
	bool				CheckPosVaild(int x, int y);
	bool				RandomPos(Utility::UT_Vec3Int& pos);

	virtual void		UpdateTaskBatchPlay(int taskID);
	virtual void		ActorGroupSizeChange(object_id_type organizeId, Group_Change_Map& changemap, object_id_type changeid);
	virtual void		PlayerChangeLogicArea(LC_ServerPlayer* player, int prearea, int curarea, int preAreaType, int curAreaType);
	virtual void		PlayerBeKilled(LC_ServerPlayer* pkServerPlayer, object_id_type lSrcActorID);
	virtual void		NPCDead(LC_ServerNPC* pkServerNPC);
	virtual void		DropItemsOnNPCDead(LC_ServerNPC* pkServerNPC);
	virtual bool		ComputeAutoFitLevel(bool changeAtOnce = false);
	void				AdjustMonster(int presize, int cursize);

	void GetDropItemPartakeKillNpcReward(LC_ServerNPC* pkServerNPC, std::map<int, int>& dropItems);

	//�����Ӧ�ӿ�
	bool				CheckPlayerActive(object_id_type playerid);
	bool				CheckPlayerActiveOrLoading(object_id_type playerid);
	bool				CheckPlayerIn(object_id_type playerid);
	bool				CheckPlayerPreIn(object_id_type playerid);
	void				GetAllRealPlayer(GameLogic::LC_ActorPtrList& playerlist);
	void				GetAllRealPlayer(unique_id_vector& playerlist);
	int					GetAllRealPlayerSize();
	int					GetAllRealActivePlayerSize();
	void				GetAllPlayer(GameLogic::LC_ActorPtrList& playerlist);
	LC_ServerPlayer*	SpawnPlayer(const unique_id_impl& charID, const Utility::UT_Vec3Int& pos, uint32_t bits, LC_ServerPlayer* owner = NULL, on_summonfinish_handler finishhandle = NULL, FactionType faction = FACTION_NONE);
	LC_ServerPlayer*	SpawnBuddyPlayer( object_id_type playerid, const unique_id_impl& charID, const Utility::UT_Vec3Int& pos, uint32_t bits, LC_ServerPlayer* owner = NULL, on_summonfinish_handler finishhandle = NULL, FactionType faction = FACTION_NONE);
	void				ClearPlayer(LC_ServerPlayer* player);

	int	GetAllControllerPlayerSize();
	LC_PlayerIDPtrMap*	GetControllerPlayerMapPtr();


	//NPC��Ӧ�ӿ�
	//ˢ��Ӣ��
	GameLogic::LC_ActorPtrList SpawnEliteNPC(GameLogic::LC_TranscriptionListEntry& entry, int index, DungeonConfig::EliteSkillRuleSetType& skillrule_list, int autofitlevel = -1, FactionType faction = FACTION_NONE, int32_t iElite=0);
	bool				isElite(int innerid);
	bool				isInElite(LC_ServerNPC* pkServerNPC);
	int					GetEliteCount(LC_ServerNPC* pkServerNPC);
	LC_ServerNPC*		GetInEliteNpc(LC_ServerNPC* pkServerNPC, int index);
	//ˢ��ͨ��
	LC_ServerNPC*		SpawnNPC(GameLogic::LC_TranscriptionListEntry& transEntry, int autofitlevel = -1, FactionType faction = FACTION_NONE);
	//ע�������NPC��ָ���еİ��������ĶԻ�NPC�������ٻ��ı�����
	void				GetAllNPC(GameLogic::LC_ActorPtrList& pkNPCList, mem::set<int>& npctype, mem::set<int>& factiontype);
	void				ClearAllNPC(mem::set<int>& npctype, mem::set<int>& factiontype);
	void				ClearNPC(LC_ServerNPC* npc);
	int					GetAllNPCSize(mem::set<int>& npctype, mem::set<int>& factiontype);
	//�����monster��ָ��ͳ�����ϵĿ��Թ�����NPC
	void				GetAllMonster(GameLogic::LC_ActorPtrList& pkNPCList, mem::set<int>& npctype, mem::set<int>& factiontype);
	void				ClearAllMonster(mem::set<int>& npctype, mem::set<int>& factiontype);
	void				ClearAllMonster();
	int					GetAllMonsterSize(mem::set<int>& npctype, mem::set<int>& factiontype);
	bool				CheckMonster(FactionType faction);

	void GetAllMonster(GameLogic::LC_ActorPtrList& pkNPCList);

	//�ŵ��߼�
	bool				SpanDoor(DungeonConfig::DoorEntry& door);
	LC_ServerMapLogic_Door* GetDoorById(int doorid);
	mem::vector<LC_ServerMapLogic_Door*> GetAllDoor();
	bool				SetDoorState(int index, DoorState state);
	int32_t				GetAllDoorSize();
	LC_ServerMapLogic_Door* GetDoorByIndex(int index);
	void				ClearAllDoor();

	//�Ի�NPC
	void				SpawnDialogNPC(GameLogic::LC_TranscriptionListEntry& transEntry);
	void				DialogueDelayOut();
	void				ClientFinishDialogue(LC_ServerPlayer* player);
	void				ClientBeginDialogue(LC_ServerPlayer* player, int dialogueid);
	bool				CheckDialogueBegin();
	bool				BeginDialogue(int dialogueid);

	//���͵�
	void				SetTransPoint(TransPointType transtype, int x = 0, int y = 0);

	//��Ӫbegin---------------------------------------------------------------------------------------------------------------------------
	bool				CreateFaction(FactionType FactionId);
	bool				AddActor2Faction(GameLogic::LC_ActorBase* actor, FactionType FactionId);
	bool				DelActorInFaction(object_id_type actorId, FactionType FactionId, bool reset = true);
	bool				DelFaction(FactionType FactionId);
	inline bool			IsFaction(int orgId) {return _InFaction_InerId(orgId);}
	const object_id_set* GetAllActorListInFaction(FactionType FactionId);
	int32_t				GetAllActorSizeInFaction(FactionType FactionId);
	object_id_vector	GetInActorListInFaction(FactionType FactionId);
	int					GetInActorSizeInFaction(FactionType FactionId);
	FactionType			GetFactionId_InerId(int innerid);
	int					GetInerId_FactionId(FactionType Factionid);
	FactionResult		GetFactionResult(FactionType FactionId);
	void				SetFactionResult(FactionType FactionId, FactionResult result);
	FactionType			GetActorFaction(GameLogic::LC_ActorBase* actor);
	mem::vector<FactionType> GetAllFactionId();
	//��Ӫend---------------------------------------------------------------------------------------------------------------------------

	//���Ƴ��ط����Լ������ų�
	inline void			SetGroupShare(Share_Type share) {m_GroupShare = share;};
	inline Share_Type	GetGroupShare(void) {return m_GroupShare;};
	inline void			SetExclusive(Exclusive_Type exclusive) {m_Exclusive = exclusive;};
	inline Exclusive_Type GetExclusive() {return m_Exclusive;};
	ResultType			CheckExclusive(LC_ServerPlayer* pkServerPlayer);
	inline void			AddGroup(int groupid){m_AllGroup.insert(groupid);}

	inline bool			IsCheckRule(void) {return m_CheckRule;};
	//����
	inline void			SetCostVigor(int cost) { m_CostVigor = cost;}
	inline int			GetCostVigor(void) { return m_CostVigor; }

	//����Ӧ
	void				SetAutoFit(bool autofit, AutoFitType fittype = AUTOFIT_TYPE_NORMAL);
	void				GetAutoFit(bool& autofit, AutoFitType& fittype, int& fitlevel);
	inline void			SetAutoChangeMonsterHp(bool autochange) { m_AutoChangeMonsterHP = autochange;}
	inline bool			GetAutoChangeMonsterHp() { return m_AutoChangeMonsterHP; }
	inline bool			IsPlayerFull(void) { return m_MaxPlayer <= 0 ? false : (GetControllerPlayerCount() >= m_MaxPlayer); }
	inline int32_t		GetMaxPlayer() { return m_MaxPlayer; }
	inline void			SetMonsterLevel(int32_t level) { m_MonsterLevel = level; }
	inline int32_t		GetMonsterLevel() { return m_MonsterLevel;}

	//����߼�
	void				SetCrossRealm(bool crossrealm, CrossRealmType crossrealmtype, int crossrealmid = 0);
	bool			IsCrossRealm()	;
	void				GetCrossRealm(CrossRealmType& crossrealmtype, int& crossrealmid);

	//PK����
	void				SetCanPK(bool pk);
	inline bool			GetCanPK(void) { return m_CanPK;}

	//�淨���
	bool				AddPlay2And(LC_ServerMapLogic_PlayBase* play, bool start = true);
	bool				AddPlay2Or(LC_ServerMapLogic_PlayBase* play, bool start = true);
	void				DelPlay(PlayType playtype);
	LC_ServerMapLogic_PlayBase* GetPlay(PlayType playtype);
	void				StartPlay(PlayType playtype = PlayType_None);
	void				EndPlay(PlayType playtype = PlayType_None);
	void				EndAllPlay();
	void				ClearAllPlay();
	bool				CheckAllPlayEnd();
	//�����淨
	void				BeginReadPlay(int& index);
	bool				ReadOnePlay(PlayVector::iterator& it, int& index);
	//�淨��update
	void				UpdateAllPlay(float fCurrentTime, float fDeltaTime);

	//������
	LC_ServerMap_Trigger* AddCircleTrigger(Utility::UT_Circle& circle, LC_ServerMap_Trigger::TriggerCallBack callback = LC_ServerMap_Trigger::TriggerCallBack());
	LC_ServerMap_Trigger* AddRectTrigger(Utility::UT_OBB& obb, LC_ServerMap_Trigger::TriggerCallBack callback = LC_ServerMap_Trigger::TriggerCallBack());
	LC_ServerMap_Trigger* GetTrigger(int id);
	void				  DelTrigger(int id);

	//��ʼһ���߼�,�Ա��ڸ�����������
	void				Start();

	inline LC_ServerMapLogic_Record* GetRecord() { return &m_logicRecord; }
	inline LC_ServerMapLogic_ActorManger& GetActorManger() { return m_ActorManger;}

	//������
	void				AddGuildTransport(const unique_id_impl& charid, uint64_t guildid, int transportid);
	bool				CheckGuildTransport(LC_ServerPlayer* player = NULL);
	inline void			DelGuildTransport(const unique_id_impl& charid) { m_GuildTransport.erase(charid); }

	inline bool			IsEnterLocked(void) { return m_bEnterLocked;}
	inline void			SetEnterLocked(bool b) { m_bEnterLocked = b;}

	inline bool			GetCreateCross() { return m_bCreateCross; }
	inline void			SetCreateCross(bool bCross) { m_bCreateCross = bCross;}

	/// ����
	inline void			SetTributaryMaxPlayer(int iCount) { m_iTributaryMaxPlayer = iCount;}
	inline void			setTributaryIndex(int32_t nIndex) { m_iTributaryIndex = nIndex; }
	inline int32_t		getTributaryIndex() { return m_iTributaryIndex; }
	int					GetTributaryFreePlayerCount();		/// ��ȡʣ����Խ������ʵ�����

	bool IsShowRaidStage() {return m_bIShowRaidStage;}

	//������Դ�Ƿ���Ч
	void SetVaild(bool value);
	bool GetVaild();

	virtual void SettleGuildMapBoss(int bossID) {}
	virtual void ForceUpdateDamageData(LC_ServerPlayer* pkPlayer, bool syncClient){}
	virtual void SetGuildID(uint64_t guildID) {}
	virtual uint64_t GetGuildID() {return 0;}
	virtual int32_t GetPlaceInvaildTime() {return 0;}
	virtual void SetPlaceInvaildTime(int32_t time) {}
public:
	static bool			_bCareOfEvent(GameLogic::LC_GameEvent* pkEvent);

protected:
	//�¼�����
	void				_receivedClientAlreadyInGame(GameLogic::LC_GameEvent* pkEvent);
	void				_kickoffPlayerFromGroup(GameLogic::LC_GameEvent* pkEvent);
	void				_groupDismiss(GameLogic::LC_GameEvent* pkEvent);

	//��Ӫ
	bool				_AddFaction(FactionType FactionId, int innerid);
	bool				_DelFaction(FactionType FactionId);
	bool				_InFaction_InerId(int innerid);
	bool				_InFaction_FactionId(FactionType Factionid);

	//�Ի�npc
	bool				__checkdialoguefinish();
	void				__enddialogue();

	//������鹲����Ϣ
	void				__clearAllGroupShare();
	void				__checkGroupMemberIn(int groupid, int exceptid, int& count);

	//����
	void				_addCourageStateWhenEliteMonsterDie(LC_ServerNPC* npc);
	void				_dropItem(LC_ServerNPC* pkServerNPC);

	//������ͼ�����¼�����
	void				_onPlayerEnterMap(GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_MapInOutType eInOutType);
	void				_onPlayerLeaveMap(GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_MapInOutType eInOutType);
//private:
protected:
	bool				m_controllerState;
	bool				m_bTributaryAutoRelease;// ��չ�����Զ����գ�Ĭ��false
	PlaceLogicState		m_PlaceState;
	float				m_TimeStamp;
	int					m_PersistTotalTime;		//���ʱ�䣬0��������
	int					m_CountDownTime;		//��󵹼�ʱ֪ͨ��ʱ��  ʧ�ܺ󣬵ȴ�m_CountDownTime�󣬽���Ҵ��ͳ�����
	int					m_PlayerVaildTime;//��ҵ���Чʱ��
	bool				m_AutoFinish;			//�������ߣ��Ƿ����
	bool				m_FirstEnterActive;
	bool				m_bIShowRaidStage;//�Ƿ�����λ����

	int					m_AllPlayerOrg;			//������ҵ���֯
	int					m_AllMonsterOrg;		//��ͳ������֯

	mem::set<int>		m_AllGroup;				//���������Ϣ

	Share_Type			m_GroupShare;			//���Ƹ��������Լ������ų�
	Exclusive_Type		m_Exclusive;

	FactionMap			m_FactionContain;		//������Ӫ
	InnerMap			m_inerId2Faction;		//�ڲ�ID����ӪIDӳ��

	LC_ServerMapLogic_Record m_logicRecord;		//�������ݼ�¼�����ڽ���
	
	TransPointType		m_TransType;			//�������͵�
	Utility::UT_Vec3Int m_TransPort;

	DoorVector			m_DoorVector;			//��

	//�Ի�npc
	object_id_type		m_dialogNpcId;
	DialoguePlayerMap	m_dialoguePlayerMap;
	bool				m_bdialogue;

	int					m_CostVigor;			//����

	bool				m_CheckRule;			//���ع���
	CSVFile::CF_TranscriptionRule::DataEntry* m_Rule;
	
	bool				m_Autofit;				//����Ӧ
	AutoFitType			m_AutofitType;
	int					m_AutofitLevel;
	
	bool				m_AutoChangeMonsterHP;	//����Ѫ������

	int					m_MaxPlayer;			//�������

	EliteMap			m_Elites;				//��Ӣ��
	int					m_MonsterLevel;			//ָ������ȼ�

	//�������
	bool				m_CrossRealm;
	CrossRealmType		m_CrossRealmType;
	int					m_CrossRealmId;

	bool				m_CanPK;

	//�淨����
	PlayVector			m_AndPlays;
	PlayVector			m_OrPlays;
	bool				m_AllPlayEnd;

	bool				m_AutoFinishPlayEnd;
	bool				m_Start;
	LC_ServerMapLogic_ActorManger m_ActorManger;

	//������
	GuildTransportMap	m_GuildTransport;

	bool				m_bEnterLocked;
	int					m_iTributaryMaxPlayer;//������������������(�Զ��������)
	bool				m_bCreateCross;

	int32_t				m_iTributaryIndex;
	bool m_bVaild;//������Դ�Ƿ���Ч
};

/*
typedef mem::hash_map<int32_t, LC_ServerMapLogic_Place*>		LC_ServerMapLogicIDPtrMap;
/*/
typedef Utility::CSUnorderedMap<int32_t, LC_ServerMapLogic_Place*>		LC_ServerMapLogicIDPtrMap;
//*/
typedef LC_ServerMapLogicIDPtrMap::iterator				LC_ServerMapLogicIDPtrMapIter;
typedef mem::vector<LC_ServerMapLogic_Place*>			LC_ServerMapLogicPlaceVector;
typedef mem::map<int32_t, LC_ServerMapLogicPlaceVector> LC_ServerMapMapLogicIndices;
typedef mem::set<int32_t>								LC_ServerMapResIDSet;
typedef mem::map<int32_t, int32_t> LC_ServerMapTributaryRecords;
#endif
