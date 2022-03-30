

#include "PlayerStateAction.h"

#include "LC_ServerPlayer.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "SK_ServerSkillEventManager.h"
#include "LC_FactionManager.h"

#include "CF_ActorFilterFunc.h"

using namespace StateMachine;
using namespace Utility;
using namespace GameLogic;
using namespace Skill;
using namespace CSVFile;

void SetPlayerLastIdleTime(StateMachine::SM_StateMachine* pkMachine,float fCurrentTime)
{
	LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)pkMachine;
	pkServerPlayer->SetLastIdleTime(fCurrentTime);
}

StateProcessType ProcessPlayerStateTestFight(SM_StateBase * State , SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime)
{
	//return ProcessPlayerStateTestFight_Orig( State, pkMachine, fCurrentTime, fDeltaTime );
	return ProcessPlayerStateTestFight_AsBuddy( State, pkMachine, fCurrentTime, fDeltaTime );
}

StateProcessType ProcessPlayerStateTestFight_AsBuddy(SM_StateBase * State , SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime)
{
	LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)pkMachine;
	pkServerPlayer->SetNextState(ST_PLAYER_CHASE);
	return SPT_END;
}

StateProcessType ProcessPlayerStateTestFight_Orig(SM_StateBase * State , SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime)
{
	LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)pkMachine;

	LC_ActorSkillList* pkSkillList = pkServerPlayer->GetActorSkillList();
	LC_ActorSkillTrigger* pkSkillTrigger = pkServerPlayer->GetSkillTrigger();

	LC_ActorSkillEntry* pkEntry = pkSkillList->GetRandomSkillEntry();
	if (pkEntry != NULL && pkEntry->GetValid())
	{
		//设置挂起技能时间
		pkEntry->SetStartTime(GET_PROCESS_TIME);
		pkServerPlayer->SetLockedTargetID(pkEntry->GetLockedTargetID());
		ResultType iResult = pkServerPlayer->PendingSkill(pkEntry->GetSkillTypeID(),0);
		if (RE_SKILL_PENDING_SUCCESS != iResult)
		{
			pkSkillList->RemoveFirstSkillEntry();
		}
	}

	//处理Player技能
	SK_SkillExecute* pkPendingSkill = pkServerPlayer->GetPendingSkillExecute();
	if (NULL != pkPendingSkill && !pkServerPlayer->IsAIControlStand() )
	{
		pkServerPlayer->SetNextState(ST_PLAYER_CHASE);
		return SPT_END;
	}

	return SPT_SUSPEND;
}

StateProcessType ProcessPlayerStateTestFollow(SM_StateBase * State , SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime , int Distance)
{
	LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)pkMachine;

	Utility::UT_Vec3Int kCurrentLocation = pkServerPlayer->GetCurrentLocation();

	LC_MapBase* pkMap = pkServerPlayer->GetMap();
	if(pkMap)
	{
		
	}
	return SPT_SUSPEND;
}

StateMachine::StateProcessType ProcessPlayerStateTestTalk(StateMachine::SM_StateBase * State , StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime)
{
	LC_FactionManager* pkFactionManager = LC_FactionManager::GetSingletonPtr();
	if (NULL == pkFactionManager)
	{
		return SPT_SUSPEND;
	}

	LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)pkMachine;

	object_id_type lLockedTargetID = pkServerPlayer->GetLockedTargetID();
	if (IS_PLAYER_CLASS_ID(lLockedTargetID))
	{
		LC_MapBase* pkMap = pkServerPlayer->GetMap();
		if (NULL != pkMap)
		{
			LC_ActorBase* pkLockedTarget = (LC_ActorBase*)pkMap->FindObject(lLockedTargetID);
			if (NULL != pkLockedTarget && !pkFactionManager->GetEnemyRelation(pkServerPlayer, pkLockedTarget))
			{
				pkServerPlayer->SetNextState(ST_PLAYER_TALK);
				return SPT_END;
			}
		}	
	}

	return SPT_SUSPEND;	
}

StateMachine::StateProcessType ProcessPlayerStateTestWander(StateMachine::SM_StateBase * State , StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime)
{
	LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)pkMachine;

	float fProperty_IdleTime = 3;
	float fLastIdleTime = pkServerPlayer->GetLastIdleTime();
	//如果休闲时间足够
	if(fCurrentTime - fLastIdleTime > fProperty_IdleTime)
	{
		pkServerPlayer->SetNextState(ST_PLAYER_WANDER);
		return SPT_END;
	}

	return SPT_SUSPEND;
}
#define			PLAYER_ACTION_SEARCH_RADIUS		3000
void ProcessPlayerStatePositiveActive(StateMachine::SM_StateBase * State , StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime)
{
	LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)pkMachine;

	float fLastPositiveActiveTime = pkServerPlayer->GetLastPositiveActiveTime();
	float fRandDeltaTime = UT_MathBase::RandFloat01();
	if(fCurrentTime - fLastPositiveActiveTime < fRandDeltaTime)
		return;

	pkServerPlayer->SetLastPositiveActiveTime(fCurrentTime);

	UT_Circle kGuidCircle;
	kGuidCircle.kCenterPoint	= pkServerPlayer->GetCurrentLocation();
	kGuidCircle.iRadius			= PLAYER_ACTION_SEARCH_RADIUS;

	CF_ActorFilterFunc::DataEntry* pkFilterData = SafeGetCSVFileDataEntryPtr<CF_ActorFilterFunc>(2,true,true);

	LC_MapBase* pkMap = pkServerPlayer->GetMap();
	if(NULL == pkMap)
		return;

	LC_ActorPtrVector* pkPlayerList = pkMap->LockActorsInCircle(kGuidCircle,pkFilterData);
	if(!pkPlayerList->empty())
	{
		LC_ActorPtrVectorIter iter;
		LC_ActorBase* pkTarget = NULL;

		TRAVP(pkPlayerList, iter)
		{
			pkTarget = *iter;

			//如果发现死亡的玩家，不作处理
			if ( (*iter)->GetID() == pkServerPlayer->GetID() || (*iter)->GetDead() || (*iter)->GetSkillFlag(SK_INVISIBLE) )
				continue;

			if (RE_SKILL_CHECK_VALID_TARGET == LC_Helper::IsValidTarget(pkServerPlayer, pkTarget, pkFilterData))
			{
				//通知仇恨
				SK_ServerSkillEventManager* pkEventManager = SERVER_GET_SKILL_EVENT_MANAGER();
				SK_SkillEvent* pkEvent = pkEventManager->AllocEvent();
				if (NULL != pkEvent)
				{
					pkEvent->SetEventType(SETE_ENTER_GUARD);
					pkEvent->SetSrcActorID(pkTarget->GetID());
					pkEvent->SetDstActorID(pkServerPlayer->GetID());
					pkEvent->SetLogicID(0);
					pkEventManager->AddEvent(pkEvent);
				}
			}
		}
	}
}