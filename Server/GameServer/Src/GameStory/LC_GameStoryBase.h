#ifndef _SERVER_GAMESTORY_BASE_H
#define _SERVER_GAMESTORY_BASE_H
//活动基类
#include "LC_Define.h"
#include "LC_TranscriptionDataType.h"
#include "LC_GameStoryDataDefine.h"
#include "LC_ServerMapLogic_Place.h"

#include "UT_TimeManager.h"

#include "ajson.hpp"

namespace GameLogic
{
	class LC_PlayerBase;
	class LC_CommonStatEntry;
}

class LC_ServerPlayer;
class LC_ServerNPC;
class LC_ServerMap_Trigger;
class LC_ServerMapLogicBase;
class LC_ServerMapLogic_Place;
typedef mem::vector<LC_ServerMapLogic_Place*> LC_ServerMapLogicPlaceVector;


AJSON(GameStory::GSRTRankSnapEntry, (d)(v)(i)(name))
AJSON(GameStory::GSRTRankSnap, (type)(detail)(version)(info))


namespace Utility
{
	class Redis_Channel;
}

namespace GameStory
{
	class LC_StoryLogic_Manager;

	struct GSServerReward : public GSCommonReward
	{
		typedef mem::map<unique_id_type, StringType> MailPlayerMap;

		GSServerReward(){}
		void Reward(GameLogic::LC_PlayerBase* pkPlayer, int lStroyID,int nLogCode = 0, const vector<int>& nParams = vector<int>());
		void Email(const MailPlayerMap& kPlayers, int nLogCode = 0, const vector<int>& nParams = vector<int>());
		void Email(mem::set<int32_t>& kServerIDs, int nLogCode = 0, const vector<int>& nParams = vector<int>());
	};

	enum GSTriCondType
	{
		GST_COND_FACTION_SCORE = 0,
		GST_COND_NPC_DEAD	   = 1,
	};

	typedef mem::map<GameStoryScheduleKey, object_id_set> KeyMonsterLogicIDsMap;
	class LC_Server_GameStory_Base : public Memory::MM_BaseObject
	{
		friend class LC_GameStory_Manager;
	public:
		LC_Server_GameStory_Base(int32_t logicID, GameStoryRecord* pkRecord);
		virtual			~LC_Server_GameStory_Base(){};

		virtual bool	PostLoaded(void);
		inline bool		IsLoaded(void) const {return m_bLoaded;}

		virtual void	ActivateStory();
		virtual void	DeactivateStory();

		virtual void 	Init();
		virtual void 	UnInit();
		virtual void 	Update(float fCurrentTime, float fDeltaTime);
		virtual void	CheckReset(uint32_t timestamp);

		virtual void	RemovePlayerStateFromCmnStatMap(const unique_id_impl& charID);
		virtual void	AddKillSpecialNPCCount(GameLogic::LC_ActorBase* pkKillerPlayer, int num);

		virtual void	NotifyUserStoryStart(LC_ServerPlayer* pkLoginPlayer);
		void			NotifyTip(GSRTTipEntry& kRTTipEntry, GameLogic::LC_ActorBase* pkKiller = NULL);

		virtual int32_t	NotifyPlayerGameStoryBatch(GameStoryScheduleKey iKey, LC_ServerPlayer* pkServerPlayer = NULL, Utility::timer_id_type iTimerID = Utility::TIMER_ID_NA, GameStoryConfig* pkConfig = NULL);
		virtual void	NotifyRankInfo(GameStoryScheduleKey iKey, LC_ServerPlayer* pkServerPlayer = NULL, bool force = false, GameStoryConfig* pkConfig = NULL);
		virtual void	NotifyActorEncountedStat(LC_ServerPlayer* pkServerPlayer, object_id_type uiID);
		virtual void	NotifyActorEncountedStatByDamageType(LC_ServerPlayer* pkServerPlayer, object_id_type uiID, int damageType);
		virtual uint64_t GetActorDamageData(LC_ServerPlayer* pkServerPlayer, object_id_type uiID, int type);

		virtual void	ActorBeKilled(GameLogic::LC_ActorBase* pkKiller, GameLogic::LC_ActorBase*  pkInjured);
		virtual void	ActorBeDamaged(GameLogic::LC_ActorBase* pkAttacker, GameLogic::LC_ActorBase* pkInjured, int32_t iMapLogicID, attr_value_type dmg);

		virtual void	DoTranscriptionEntered(LC_ServerMapLogic_Place* pkPlace, LC_ServerPlayer* pkPlayer){};
		virtual void	DoTranscriptionFinished(LC_ServerMapLogic_Place* pkPlace, GSTranscriptResult& trans){};

		void	PlayerEnter(LC_ServerPlayer *pkPlayer, int32_t mapID, uint32_t times=1);
		void	PlayerFinish(LC_ServerPlayer* pkOwnerPlayer, int32_t mapID, uint32_t times=1);

		virtual void	OnAcceptTask(GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_Task* pkTask, GameStoryConfig* pkConfig = NULL);
		virtual void	OnFinishTask(GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_Task* pkTask, GameStoryConfig* pkConfig = NULL);
		virtual void	OnCancelTask(GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_Task* pkTask, GameStoryConfig* pkConfig = NULL);

		virtual void	PreMapDestoryed(LC_ServerMapLogicBase* pkMapLogic);
		virtual void	PostMapCreated(LC_ServerMapLogicBase* pkMapLogic, int iAutoFitLevel);
		virtual void	PrePlayerLeaveMap(GameLogic::LC_PlayerBase* pkPlayer, LC_ServerMapLogicBase* pkPlace);
		virtual void	PostPlayerEnterMap(GameLogic::LC_PlayerBase* pkPlayer, LC_ServerMapLogicBase* pkPlace);
		virtual void	PlayerLogicAreaChanged(GameLogic::LC_PlayerBase* pkPlayer, LC_ServerMapLogicBase* pkPlace, int32_t iPre);
		virtual void	NotifyEnterOfflinePlayer(GameLogic::LC_PlayerBase* pkPlayer, LC_ServerMapLogicBase* pkPlace);

		virtual void	ProcessGameEvent(GameLogic::LC_GameEvent* pkEvent, LC_StoryLogic_Manager* pkStoryLogicManager, GameLogic::LC_PlayerBase* pkPlayer);

		virtual void	CheckUpdateTask(LC_ServerPlayer* pkPlayer){};

		void			ProgressChanged(LC_ServerPlayer* pkPlayer, int pro);

		uint32_t GetStartTime();
		int32_t GetEndTime();

		inline bool		IsTimestampIn(int64_t timestamp) {return m_pkCoreRecord ? m_pkCoreRecord->IsTimestampIn(timestamp) : false;}
		inline bool		IsActive(void) {return m_pkCoreRecord ? m_pkCoreRecord->m_bActive : false;};

		inline GameStoryRecord*	GetCoreRecord(void) { return m_pkCoreRecord; }
		inline GameStoryConfig*	GetConfig(void) { return m_pkCoreRecord ? m_pkCoreRecord->GetConfig():NULL;};

		inline int				GetStoryID(void) { return m_pkCoreRecord ? m_pkCoreRecord->m_iStoryID:-1;};
		inline int				GetLogicID(void) { return m_iLogicID;};

		bool GetRepeated();
		void SetRepeated( bool bInput );

		GSRTPlayerCommonStat*	GetPlayerStats(const unique_id_impl& charID, bool bForceExist = false, StringType sName = "");
		GSRTPlayerCommonStat*	GetPlayerStats(GameLogic::LC_PlayerBase* pkPlayer);

		void GetAllPlayerIDs(mem::vector<object_id_type>& playerVec);
		void GetStoryPlayer(GameStoryScheduleKey iKey, mem::vector<object_id_type>& playerVec, GameStoryConfig* pkConfig);

		void					GetCurrentStatReward(const unique_id_impl& charID, GSCommonReward& reward);
		inline const GSRTRankEntry*	GetRTRankEntry(const RankID& kRankID){return m_kRTRankInfo.GetRankEntry(kRankID);}

		void			GetPlayers(GSRTPlayerFilter& kRTFilter);

		ResultType		ReqEnterCrossMap(GameLogic::LC_PlayerBase* pkPlayer, int32_t iMapResID);
		ResultType		ReqTaskWaver(GameLogic::LC_PlayerBase* pkPlayer, bool addItem);
		ResultType		ReqShopping(GameLogic::LC_PlayerBase* pkPlayer, shop_id_type iShopID);
		ResultType		ReqTransfer(GameLogic::LC_PlayerBase* pkPlayer, int32_t iAreaIdx);
		ResultType		ReqTaskAppoint(GameLogic::LC_PlayerBase* pkPlayer, int32_t iTaskID);
		ResultType		ReqBuyMapStepTimes(GameLogic::LC_PlayerBase* pkPlayer);
		bool			checkTimesLimit(int32_t shopID, int32_t times);

		ResultType		CanTaskWaver(GameLogic::LC_PlayerBase* pkPlayer/*, GameLogic::LC_ShopIDMap& kShopIDMap*/);
		//ResultType		TaskWaver(GameLogic::LC_PlayerBase* pkPlayer, uint32_t& uiCount);
		ResultType		TaskWaver(GameLogic::LC_PlayerBase* pkPlayer, uint32_t& rlt);
		ResultType		CanTaskAppoint(GameLogic::LC_PlayerBase* pkPlayer, int32_t iTaskID, const TaskData*& pkTaskData);
		ResultType		TaskAppoint(GameLogic::LC_PlayerBase* pkPlayer, int32_t iTaskID, bool checkTaskID=false);
		ResultType		CheckClearSameTaskType(GameLogic::LC_PlayerBase* pkPlayer);


		void AssignEscortTask(GameLogic::LC_PlayerBase* pkPlayer);

		void			ClearAreaKeyMonster(GameStoryScheduleKey iKey, int32_t iAreaIndex);
	
		GameLogic::LC_CommonStatEntry* GetCommonStatEntry(GameLogic::LC_PlayerBase* pkPlayer, GameStoryConfig* pkConfig=NULL);
		void			RecordCommonStat(GameLogic::LC_PlayerBase* pkPlayer, GameStoryCommonStatType eCmnStatType, int iStat, GameStoryConfig* pkConfig = NULL);

		//Faction war
		virtual void	InitFactionScore(GameStoryScheduleKey iKey) {}
		virtual void	AddFactionScore(GameStoryScheduleKey iKey, int32_t iType, int32_t iVal, GameLogic::LC_ActorBase* pkKiller, GameLogic::LC_ActorBase* pkInjured) {};
		virtual void	AddActorScore(GameStoryScheduleKey iKey, int32_t iVal, GameLogic::LC_ActorBase* pkKiller, GameLogic::LC_ActorBase* pkInjured) {};
		virtual int32_t	GetFactionScore(GameStoryScheduleKey iKey, int32_t iType) { return 0; }
		virtual void	SetFactionScoreDirty(GameStoryScheduleKey iKey) {}
		virtual void	CheckSyncFactionScore(float fCurrentTime) {}
		virtual void	SyncFactionScore(GameStoryScheduleKey iKey);
		virtual void	ClearFactionScore(GameStoryScheduleKey iKey) {}
		virtual void	AttackTargetStat(int32_t iType, int32_t iID, int32_t iNum) {}
		virtual bool	CheckAttackTarget() { return false; }
		virtual FactionType GetWinFactionID() const { return FACTION_NONE; }
		virtual	void	OnPlayerInWar(LC_PlayerBase* pkPlayer) {}
		virtual	GSServerReward::MailPlayerMap* GetAttackers() { return NULL; }
		virtual GSServerReward::MailPlayerMap* GetDefenders() { return NULL; }

		virtual void	FactionOccupation(GameStoryScheduleKey iKey, GameLogic::LC_ActorBase* pkPlayer, GameLogic::LC_ActorBase* pkBeOccupied){}

		virtual void	CheckTriConditions(GameStoryScheduleKey iKey, int iParam1 = 0, int iParam2 = 0, object_id_type iParam3=0);

		const GSRTMonsterIndexCluster* GetGSRTMonsterData(int32_t monsterObjectID);

		void GetNpcAndPlayerInfo(int32_t mapResID, mem::map<int32_t, int32_t>& monsterInfo, LC_PlayerIDPtrMap** pkPlayerIDPtrMap);
	public:
		static bool		_bCareOfEvent(GameLogic::LC_GameEvent* pkEvent);
	protected:
		inline int		_autoEliteIdx(void) { return ++m_lBaseEliteIdx;};

		virtual void	_spawnBatchMonster(GameStoryScheduleKey iKey, int iDetailIndex, GameStoryConfig* pkImplConfig, GameLogic::LC_ActorIDPtrMap* pkActorMap, GSRTTriggerSrcInfo* pkTriSrc, int iAutoFitLevel, GameStoryConfig* pkConfig, int timeID = TIMER_ID_NA, object_id_type iParam=0);

		bool			_checkConditions(GameStoryScheduleKey iKey, const Utility::UT_SIMDataList& kConditions, int iPram1 = 0, int iParam2 = 0);

		void			SyncKeyMonsterHP();
		void			AddKeyMonsterLogicID(GameStoryScheduleKey iKey, object_id_type iLogicID);
		void			NotifyKillNPCTip(GameStoryScheduleKey iKey, GameLogic::LC_ActorBase* pkKiller, GameLogic::LC_ActorBase* pkSufferer, GameStoryConfig* pkConfig);
		void			ReportMosterStateInfo(GameStoryScheduleKey iKey, Utility::timer_id_type iTimerID);
		uint32_t		GetBatchNodeTriTime(GameStoryScheduleKey iKey, int32_t iBatchIdx, Utility::timer_id_type iTimerID);
		uint32_t		GetBatchNodeTriTime(GSRTBatchDetail* pkDetail, Utility::timer_id_type iTimerID);
		uint32_t		GetTriTime(Utility::timer_id_type iTimerID);
	
		uint32_t		GetNextBatchNodeTriTime(GameStoryScheduleKey iKey, int32_t iBatchIdx, Utility::timer_id_type iTimerID);

		GSRTTriggerRecord* GetRTTriggerRecord(const GSRTTriggerIndex& kTriIdx);

		GSRTActorTriggerReocrd*	_getRTActorTriggerRecord(object_id_type iID, GameStoryConfig* pkConfig = NULL);
		GSRTActorTriggerReocrd*	_initRTActorTriggerRecord(GameLogic::LC_ActorBase* pkActor, GameStoryConfig* pkConfig);

		virtual void	_clearRTTriActorRecord(object_id_type iID) {};
	
		void			_doRankReward(GameStoryConfig* pkConfig);
		void			_doRankReward(GameStoryScheduleKey iKey, GameStoryConfig* pkConfig);

		void			_setupRank(void);
		void			_checkresetRank(void);
		bool			_getRankKey(const RankEntry* pkRankEntry, std::string& key);
		void			_updateRankInfo(void);
		void			_getRankInfo(Utility::Redis_Channel& kRedisChannel, const RankEntry* pkDataEntry);

		void			_syncRankInfo(const RankID& kRankID, GSRTRankSnap& kRTRankSnap);

		void			_doBestGuildDamageStatRelatedByPlayerDamage(GameStoryScheduleKey iKey, GameLogic::LC_ActorBase* pkActor, GameStoryConfig* pkConfig);
		
		void			_doDamageStatRelated(GameStoryScheduleKey iKey, GameLogic::LC_ActorBase* pkActor, GameStoryConfig* pkConfig);

		void			_clearSpawnedMonster(object_id_type iID, GameLogic::LC_ActorBase* pkKiller);

		void			_UpdateBestGuildBossStat(GameLogic::LC_ActorBase* pkActor);

		void			_doRegularBossDamageReward(GameLogic::LC_ActorBase* pkActor);
		void			_doEscortRobbedReward(GameLogic::LC_ActorBase* pkActor, GameLogic::LC_ActorBase* pkKiller);
		

		virtual void 	_initMapSetting(GameStoryConfig* pkConfig);
		virtual void 	_unitMapSetting(GameStoryConfig* pkConfig);

		virtual void	_updateCrossMapping(void);

		virtual void	_createCrossMapping(int32_t iMapResID);
		virtual void	_destroyCrossMapping(int32_t iMapResID);


		virtual void	_setupBatchSchedule(GameStoryConfig* pkConfig);
		virtual void	_resetBatchSchedule(GameStoryConfig* pkConfig);

		virtual bool  _tryTriggerNextBatch(GameStoryScheduleKey iKey, GameStoryConfig* pkConfig, int batchIndex, uint32_t endTime, int iSeq, mem::vector<int>& batchVec);
		virtual void	_initBatchScheduleByBatchIndex(GameStoryScheduleKey iKey, GameStoryConfig* pkConfig, mem::map<uint64_t, uint64_t>& batchIndexRecord, int iSeq = 0);
		virtual void	_initBatchSchedule(GameStoryScheduleKey iKey, GameStoryConfig* pkConfig, bool limitNotice=false, int iSeq = 0, Utility::timer_id_type timerID = Utility::TIMER_ID_NA, LC_ServerPlayer* pServerPlayer = NULL);
		virtual void	_clearBatchSchedule(GameStoryScheduleKey iKey, GameStoryConfig* pkConfig);
		virtual void	_nextBatchSchedule(GameStoryScheduleKey iKey, int iDetailIdx, Utility::timer_id_type timerID, GameStoryConfig* pkConfig, bool limitNotice=false, LC_ServerPlayer* pServerPlayer = NULL);

		bool			_activeBatchSchedule(GameStoryScheduleKey iKey, int iSelfAdaptingLevel, GameStoryConfig* pkConfig);
		void			_addBatchSchedule(GameStoryScheduleKey iKey, int iSelfAdaptingLevel, GameStoryConfig* pkConfig, bool limitNotice=false);

		void			_setupTrigger(GameStoryScheduleKey iKey, GameStoryConfig* pkConfig);
		void			_cleanupTrigger(GameStoryScheduleKey iKey);

		GameStoryScheduleKey _peekRTBatchScheduleKey(void); 
		GSRTBatchSchedule* _getRTBatchSchedule(GameStoryScheduleKey iKey);
		void			_getRTScheduleMaps(GameStoryScheduleKey iKey, int32_t iMapResId, LC_ServerMapLogicPlaceVector& kPlaces, GameStoryConfig* pkConfig);
		void			_getRTScheduleActors(GameStoryScheduleKey iKey, GameLogic::LC_ActorPtrList& kActors, GameStoryConfig* pkConfig);
		GameStoryScheduleKey _filterProperRTScheduleKey(LC_ServerMapLogicBase* pkMapLogic, GameStoryConfig* pkConfig);

		bool			_isRTScheduleClear(GameStoryScheduleKey iKey, GameStoryConfig* pkConfig);

		void			_processCommonStatEvent(GameLogic::LC_GameEvent* pkEvent, LC_StoryLogic_Manager* pkStoryLogicManager, GameLogic::LC_PlayerBase* pkPlayer);
		void			_resetCommonStat(GameLogic::LC_PlayerBase* pkPlayer, uint32_t timestamp, GameLogic::LC_CmnStatCheckResetHandler& handler, GameStoryConfig* pkConfig=NULL);
		void			_resetAllCommonStat(GameLogic::LC_PlayerBase* pkPlayer, GameStoryConfig* pkConfig=NULL);

		void			_checkTask(GameLogic::LC_PlayerBase* pkPlayer, GameStoryConfig* pkConfig=NULL);

		void			_setupActorFilter(GSRTPlayerFilter& kRTActorFilter);

		void			_addTips(GameStoryScheduleKey iKey, uint32_t uiBenchMark, const TipSetting& kTipSetting, GameLogic::LC_ActorBase* pkCoreActor, int32_t iID, GSRTTipSchedule::HandlerType* pkHandler = NULL);
		void			_clearTips(GameStoryScheduleKey iKey);
		void			_clearTips(void);
	protected:
		//call back
		static void		_initBatchCB(Utility::timer_id_type iTimerID, LC_Server_GameStory_Base* pkStory, GameStoryScheduleKey iKey, int iSeq);
		static void		_triggerBatchCB(Utility::timer_id_type iTimerID, LC_Server_GameStory_Base* pkStory, GameStoryScheduleKey iKey, int iDetailIndex, int iAutoFitLevel);
		static void		_triggerBatchCB_CF(Utility::timer_id_type iTimerID, LC_Server_GameStory_Base* pkStory, GameStoryScheduleKey iKey, int iDetailIndex, int iAutoFitLevel, GameStoryConfig* pkConfig, bool limitNotice=false, object_id_type iParam3=0, bool setEndTimer=true, int64_t vaildTime=0);
		static void 	_endBatchEndCB(Utility::timer_id_type timerID, LC_Server_GameStory_Base* pkStory, GameStoryScheduleKey iKey, int iDetailIndex, GameLogic::LC_ActorBase* pkTriActor);
		static void 	_endBatchEndCB_CF(Utility::timer_id_type timerID, LC_Server_GameStory_Base* pkStory, GameStoryScheduleKey iKey, int iDetailIndex, GameLogic::LC_ActorBase* pkTriActor, GameStoryConfig* pkConfig);
		//end call back

		//redis call back, do not use stroy ptr ,use story id or logic id recommanded 
		static void		_syncRankInfoCB(int32_t iStoryLogicID, const RankID& kRankID, int32_t err, const std::string& val);

		void			_onCrossTransCreated(int64_t raidid, uint32_t serverid, int32_t resId);
		void			_onCrossTransFailed(int64_t raidid, uint32_t serverid, int32_t resId);
		void			_onCrossTransDestroyed(int64_t raidid, uint32_t serverid, int32_t resId);

		virtual void	_onActived(void);
		virtual void	_onDeactived(void);

		virtual void	_onTriggerHit(LC_ServerMap_Trigger*, GameLogic::LC_ActorPtrList&, LC_ServerMapLogic_Place*) {};
		bool RestartMapLogicByRes( int32_t nMapResId );

		ResultType BuyVIPProfitTimes(LC_PlayerBase* pkOwnerPlayer, uint32_t times);
		ResultType BuyVIPPrivilegeCardTimes(LC_PlayerBase* pkOwnerPlayer, uint32_t times);
	protected:
		int							m_iLogicID;
		GameStoryRecord*			m_pkCoreRecord;
		GSRTMapTriggerRecordMap		m_kRTMapTriggerRecordMap;		//maplogicid --> triggerid -->recorddata
		GSRTActorTriggerReocrdMap	m_kRTActorTriggerReocrdMap;		//actorlcid --> actorrecorddata
		int							m_lBaseEliteIdx;
		EliteGroupMap				m_kEliteGrpMap;
		GSRTMonsterRecordMap		m_kRTMonsterRecord;
		GSRTBatchScheduleMap		m_kRTBatchScheduleMap;
		GSRTCrossTranscriptionMap	m_kRTCrossTranscriptionMap;
		float						m_fLastUpdateCrossTime;
		GSRTTipScheduleList			m_kRTTipSchedules;
		GSRTRankInfo				m_kRTRankInfo;
		float						m_fLastSyncHPTime;
		KeyMonsterLogicIDsMap		m_kKeyMonsterLogicIDsMap;		//maplogicid --> key monster logicids
	private:
		GSRTPlayerCmnStatMap		m_kPlayerCmnStatMap;
		Utility::UT_Timer			m_kSnapTimer;

		bool						m_bLoaded;

		bool m_bRepeated;
	};
}

#endif
