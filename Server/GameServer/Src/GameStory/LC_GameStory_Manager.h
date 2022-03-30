#ifndef __SERVER_GAMESTORY_MANAGER__
#define __SERVER_GAMESTORY_MANAGER__
//Server端活动时间管理器

#include "LC_ActorBase.h"
#include "LC_TranscriptionDataType.h"
#include "LC_GameStoryDataDefine.h"
#include "LC_ServerLogicManager.h"

#include "LC_ServerPlayer.h"
#include "CF_GameStory.h"
#include "LC_ServerTaskManager.h"
#include "LC_GameStoryBase.h"

class LC_ServerPlayer;
class LC_Transcription_Manager;

struct OnSyncTestDatabase : public GameLogic::LC_ActorOperator
{
	OnSyncTestDatabase()
	{
		syncMsg.m_topicID.clear();
		UT_ServerHelper::GetTestDatabase(syncMsg.m_topicID);
	}

	virtual bool operator ()(GameLogic::LC_ActorBase* pkActor)
	{
		if (pkActor && IS_PLAYER_ID(pkActor->GetID()))
		{
			LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkActor;
			LC_ServerPlayer* pkOwnerPlayer = pkPlayer->GetOwnerPlayer();
			if(NULL != pkOwnerPlayer)
			{
				pkPlayer->SendMsgToClient(MGPT_SYNCTESTDATABASE, &syncMsg);
			}
		}
		return true;
	}

	MG_SyncTestDatabase syncMsg;
};

struct AssignEscortTask : public GameLogic::LC_ActorOperator
{
	AssignEscortTask(GameStory::LC_Server_GameStory_Base* story, int serverID) : pkStory(story)	, iServerID(serverID) 
	{

	}

	virtual bool operator ()(GameLogic::LC_ActorBase* pkActor)
	{
		if (pkActor && IS_PLAYER_ID(pkActor->GetID()))
		{
			LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkActor;
			if(/*pkPlayer!=NULL &&*/ iServerID==pkPlayer->GetServerID() && NULL!=pkStory)
			{
				pkStory->AssignEscortTask(pkPlayer);
			}
		}
		return true;
	}

	GameStory::LC_Server_GameStory_Base* pkStory;
	int iServerID;
};

namespace GameStory
{
	struct GameStoryRelate
	{
		GameStoryRelate() : m_iStoryID(0), m_iType(0), m_iRelateDay(0), m_iFirstTime(0), m_iSecondTime(0) {}
		int32_t		m_iStoryID;
		int32_t		m_iType;
		int32_t		m_iRelateDay;
		time_t		m_iFirstTime;
		time_t		m_iSecondTime;
		UT_CronData	m_kCronData;
	};
	typedef mem::map<int32_t, GameStoryRelate> GSRelateMap;
	struct LC_GameStroyReward;
	class LC_GameStory_Manager : public GameStoryCommonManager
	{
	public:
		LC_GameStory_Manager();
		~LC_GameStory_Manager();

		void	Init(CSVFile::CSVDataLoader* pkLoader=NULL, const CSVFile::CSVDataSourceList* pkCSVs=NULL);
		void	UnInit(void);

		void	OnGameServerLoaded(void);
		void	OnSocietyServerConnected(void);

		void Update_Alliance_Boss_Info(std::vector<int>& storyVec);

		void	ScheduleGameStory(int32_t iStoryID, const StringType& cronStr);

		void	Update(float fCurrentTime,float fDeltaTime);

		void	NotifyPlayerGameStoryContent(LC_ServerPlayer* pkServerPlayer, const GSResIDSet* pkStroyIDs=NULL);
		void	NotifyActorEncountedStat(LC_ServerPlayer* pkServerPlayer, int32_t iStoryID, object_id_type uiID);
		void NotifyActorEncountedStatWithoutStoryID(LC_ServerPlayer* pkServerPlayer, object_id_type uiID);
		void NotifyActorEncountedStatByDamageType(LC_ServerPlayer* pkServerPlayer, int32_t iStoryID, object_id_type uiID, int damageType);		

		ResultType IsAllowedEnterTranscription(GameLogic::LC_PlayerBase* pkServerPlayer, int lMapResID, GameLogic::GameStoryEntry* pkStoryEntry);

		ResultType		CheckGameStoryEnterCondition(GameLogic::LC_PlayerBase* pkPlayer, const int32_t iStoryID, uint32_t times=1);
		virtual bool	CheckIsActiveStory(int32_t iStoryID);

		shop_id_type	GetRespawnCostShopID(int32_t iStoryID, int lMapResId);

		void	CheckReset(GameLogic::LC_PlayerBase *pkPlayer);

		int32_t	GetTaskExtraProfit(int32_t iStoryID, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_Task* pkTask, uint32_t timestamp);
		int32_t	GetTaskFailProfit(int32_t iStoryID, GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_Task* pkTask, uint32_t timestamp);

		static void ActiveStory(Utility::timer_id_type timerID, int32_t storyID, bool bLoop = false, bool bRepeat = false);
		static void DeactiveStory(Utility::timer_id_type timerID, int32_t storyID);

		GameStoryRecord* GetStoryRecord(int32_t iStoryID);

		void	AyncDeactiveStory(int32_t storyID);

		const GSResIDSet& GetAllStoryIDs(void) {return m_kStoryIDs;}; 

		bool	GetStatReward(int32_t iStoryID, unique_id_type charid, GSCommonReward& kReward);

		void	ClearAreaKeyMonster(int32_t iStoryID, GameStoryScheduleKey iKey, int32_t iAreaIndex);
		//op begin
		ResultType	ReqEnterCrossMap(GameLogic::LC_PlayerBase *pkPlayer, int32_t iStoryID, int32_t iMapResID);
		ResultType	ReqTaskWaver(GameLogic::LC_PlayerBase *pkPlayer, int32_t iStoryID, bool addItem);
		ResultType	ReqShopping(GameLogic::LC_PlayerBase *pkPlayer, int32_t iStoryID, shop_id_type iShopID);
		ResultType	ReqTransfer(GameLogic::LC_PlayerBase *pkPlayer, int32_t iStoryID, int32_t iAreaIdx);
		ResultType	ReqTaskAppoint(GameLogic::LC_PlayerBase *pkPlayer, int32_t iStoryID, int32_t iTaskID);
		ResultType	ReqBuyTimes(GameLogic::LC_PlayerBase *pkPlayer, int32_t iStoryID, int32_t buyType, uint32_t times);
		ResultType	ReqBuyMapStepTimes(GameLogic::LC_PlayerBase *pkPlayer, int32_t iStoryID);
		//op end
		const GSRTRankEntry* QueryRankSnap(int32_t iStoryID, const RankID& kRankID);

		GSRTPlayerCommonStat* GetPlayerCmnStats(GameLogic::LC_PlayerBase *pkPlayer, int32_t iStoryID);
		void RemovePlayerCmnStats(GameLogic::LC_PlayerBase *pkPlayer);

		void	LoadRelateTime();
		void	OnCrossWarTimeChange(time_t iPreTime, time_t iNextTime);
		time_t	GetRelateTime(int32_t iStoryID) const;
		void	SetRealDay(int32_t iStoryID, int32_t iDay);

		void	_resetStoryData(GameLogic::LC_PlayerBase *pkPlayer, int iStoryID, int64_t timestamp);

		int32_t GetStoryIdByReplevy( int nInput );

		LC_Server_GameStory_Base* GetGameStoryBasePtrByStoryID(int32_t storyID);

		void ScheduleGameStory(int32_t iStoryID, Utility::timer_id_type timerID=Utility::TIMER_ID_NA);

		int32_t GetNextStoryIdInGroup( int32_t nGroupId, int32_t nStoryId );
		bool IsFirstStoryIdInGroup( int32_t nGroupId, int32_t nStoryId );

		bool StoryIsActive(int32_t storyID);
		uint32_t GetStoryStartTime(int32_t storyID);
		int32_t GetStoryEndTime(int32_t storyID);
		int32_t GetKillSpecialMonsterCount(LC_ServerPlayer *pkPlayer, int32_t storyID);

		void ScheduleLoopGameStory();

		static void	BroadCastQz(int32_t iStoryID);
	protected:
		void TryAddRecordGroup(int32_t nGroupId, int32_t nStoryId);
		bool IsGroupBeginMap( int32_t nGroupId, int32_t nStoryId );
		void InitAllRecorderGroupSchedData();

		void InitSingleRecorderGroupSchedData( int32_t nGroupId,  StoryVector& data );

		bool	_checkReset(GameLogic::GameStoryEntry *pkStoryEntry, int64_t timestamp);

		static bool _checkRelateCondition(int32_t iStoryID);
		
		void	_generateStoryRecordMap();
		GameStoryRecord* _addStoryRecord(const CSVFile::CF_GameStory::DataEntry *pkEntry);

		void	_addRelateStory(int32_t iStoryID, int32_t iRelateType, int32_t iRelateDay, StringType sCronInfo);

		void	_broadcastAllGameStoryContentToClient(const GSResIDSet* pkStroyIDs=NULL);
		
		void	_scheduleGameStory(void);
		void	_scheduleGameStory(int32_t iStoryID, Utility::timer_id_type timerID=Utility::TIMER_ID_NA, bool bLoop = false);

		inline static void _scheduleGameStoryCB(Utility::timer_id_type timerID, int32_t iStoryID) { SERVER_GET_GAMESTORY_MANAGER->_scheduleGameStory(iStoryID, timerID); }

		StoryRecordMap	m_kStoryRecordMap;
		StoryRecordBeginMap m_kStoryGroupBegin;
		GSResIDSet		m_kStoryIDs;
		GSRelateMap		m_kRelateMap;
		mem::map<int32_t, int32_t> m_ReplevyMap;
		StoryGroup m_LoopStoryGroup;
	};
}

#endif
