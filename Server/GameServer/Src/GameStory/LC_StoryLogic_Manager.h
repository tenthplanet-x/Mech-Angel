#ifndef __SERVER_STORYLOGIC_MANAGER__
#define __SERVER_STORYLOGIC_MANAGER__
//Server端活动逻辑管理器

#include "LC_GameStoryBase.h"
#include "LC_ActorBase.h"
#include "LC_TranscriptionDataType.h"
#include "UT_TimeManager.h"

namespace GameLogic{
	class LC_PlayerBase;
}

class LC_ServerPlayer;

namespace GameStory
{
	class LC_Server_GameStory_Base;


	class LC_StoryLogic_Manager : public Memory::MM_BaseObject
	{
	public:
		LC_StoryLogic_Manager();
		~LC_StoryLogic_Manager();

		void	Init();
		void	UnInit();
		void	Update(float fCurrentTime,float fDeltaTime);

		void	CheckReset(uint32_t timestamp);

		LC_Server_GameStory_Base *GetLCStoryImpl(GameStoryRecord &record);

		BOOL	ActivateStory(GameStoryRecord &record, bool bRepeated = false);
		BOOL	DeactiveStory(int logicID);
		
		void	CheckUpdateTask(LC_ServerPlayer *pkPlayer);

		LC_Server_GameStory_Base* GetStoryLogic(int logicID);

		virtual void ProcessGameEvent(GameLogic::LC_GameEvent* pkEvent);

		/// 事件响应函数
		void	PostEnterGame(GameLogic::LC_PlayerBase *pkPlayer);

		//pkKiller或者pkInjured 如果是玩家，则一定是主控
		void    ActorBeKilled(GameLogic::LC_ActorBase* pkKiller, GameLogic::LC_ActorBase*  pkInjured);
		void	ActorBeDamaged(GameLogic::LC_ActorBase* pkAttacker, GameLogic::LC_ActorBase* pkInjured, int32_t iMapLogicID, attr_value_type dmg);

		void	AddFactionScore(GameLogic::LC_ActorBase* pkKiller, GameLogic::LC_ActorBase* pkInjured, int32_t iMapLogicID, int32_t iType, int32_t iVal);
		void	AddActorScore(GameLogic::LC_ActorBase* pkKiller, GameLogic::LC_ActorBase* pkInjured, int32_t iMapLogicID, int32_t iVal);
		void	FactionOccupation(GameLogic::LC_ActorBase* pkPlayer, GameLogic::LC_ActorBase* pkBeOccupied);

		void	TranscriptionEntered(LC_ServerMapLogic_Place* pkPlace, LC_ServerPlayer *pkPlayer);
		void	TranscriptionFinished(LC_ServerMapLogic_Place* pkPlace, GSTranscriptResult &trans);

		void	OnAcceptTask(GameLogic::LC_PlayerBase *pkPlayer, GameLogic::LC_Task* pkTask);
		void	OnFinishTask(GameLogic::LC_PlayerBase *pkPlayer, GameLogic::LC_Task* pkTask);
		void	OnCancelTask(GameLogic::LC_PlayerBase *pkPlayer, GameLogic::LC_Task* pkTask);

		void	PreMapDestoryed(LC_ServerMapLogicBase* pkMapLogic);
		void	PostMapCreated(LC_ServerMapLogicBase* pkMapLogic, int iAutoFitLevel);
		void	PrePlayerLeaveMap(GameLogic::LC_PlayerBase *pkPlayer, LC_ServerMapLogicBase* pkPlace);
		void	PostPlayerEnterMap(GameLogic::LC_PlayerBase *pkPlayer, LC_ServerMapLogicBase* pkPlace);
		void	PlayerLogicAreaChanged(GameLogic::LC_PlayerBase* pkPlayer, LC_ServerMapLogicBase* pkPlace, int32_t iPre);
		void	NotifyEnterOfflinePlayer(GameLogic::LC_PlayerBase *pkPlayer, LC_ServerMapLogicBase* pkPlace);

		GameStoryCommonStatType GetCommonStatType(GameLogic::GameEventType eEventType) const;
	protected:
		static inline int32_t AutoLogicID(void) { return ++m_stBaseLogicID;}

	protected:
		typedef mem::map<int,LC_Server_GameStory_Base*>	GameStoryLogicMap;
		typedef GameStoryLogicMap::iterator	GameStoryLogicMapIter;

		GameStoryLogicMap		m_kStoryLogicMap;
		GameStoryETP2CSTPMap	m_kETP2CSTPMap;

		static int32_t			m_stBaseLogicID;		/// 自增id
		Utility::UT_Timer		m_kUpdateTimer;
	};

}
#endif