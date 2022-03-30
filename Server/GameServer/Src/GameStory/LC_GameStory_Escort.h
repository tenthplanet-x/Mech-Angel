#ifndef LC_GAMESTORY_ESCORT_H
#define LC_GAMESTORY_ESCORT_H

#pragma once

#include "LC_GameEvent.h"
#include "LC_ServerPlayer.h"
#include "LC_ActorBase.h"
#include "LC_TranscriptionDataDefine.h"
#include "LC_TranscriptionDataType.h"
#include "LC_GameStoryDataDefine.h"
#include "LC_GameStoryBase.h"
#include "LC_ServerMapLogic_Place.h"

namespace GameStory
{

	class LC_GameStory_Escort : public LC_Server_GameStory_Base
	{
	public:
		LC_GameStory_Escort(int32_t logicID, GameStoryRecord *pkRecord);
		virtual			~LC_GameStory_Escort();

		virtual void	Init();
		virtual void	UnInit();
		virtual void	ActorBeKilled(GameLogic::LC_ActorBase* pkKiller, GameLogic::LC_ActorBase*  pkInjured);
		virtual void	OnAcceptTask(GameLogic::LC_PlayerBase *pkPlayer, GameLogic::LC_Task* pkTask, GameStoryConfig* pkConfig = NULL);
		virtual void	CheckUpdateTask(LC_ServerPlayer* pkPlayer);
		virtual int32_t	NotifyPlayerGameStoryBatch(GameStoryScheduleKey iKey, LC_ServerPlayer* pkServerPlayer=NULL, Utility::timer_id_type iTimerID=Utility::TIMER_ID_NA, GameStoryConfig* pkConfig = NULL);
	protected:
		virtual void	_onTriggerHit(LC_ServerMap_Trigger* pkTrigger, GameLogic::LC_ActorPtrList& kActors, LC_ServerMapLogic_Place* pkMapLogic);
		virtual void	_clearRTTriActorRecord(object_id_type iID);
	private:

	protected:
		object_id_set m_kEscortSet;
	};
}
#endif