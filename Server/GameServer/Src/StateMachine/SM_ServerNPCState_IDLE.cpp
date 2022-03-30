

#include "LC_ServerNPC.h"
#include "SM_ServerNPCState_IDLE.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "LC_PlayerBase.h"
#include "SK_SkillExecute.h"
#include "LC_ServerPlayer.h"
#include "SK_ServerSkillEventManager.h"
#include "LC_FactionManager.h"

using namespace StateMachine;
using namespace CSVFile;
using namespace Skill;
using namespace GameLogic;
//------------------------------------------------------
SM_ServerNPCState_IDLE::SM_ServerNPCState_IDLE()
: SM_NPCState_IDLE()
{
}
//------------------------------------------------------
SM_ServerNPCState_IDLE::~SM_ServerNPCState_IDLE()
{
}
//------------------------------------------------------
bool SM_ServerNPCState_IDLE::CanTransite(SM_StateMachine* pkMachine,SM_StateBase* pkToTransiteState)
{
	return true;
}
//------------------------------------------------------
StateChangeResultType SM_ServerNPCState_IDLE::StateStart(SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime)
{
	SM_NPCState_IDLE::StateStart(pkMachine,fCurrentTime,fDeltaTime);

//#pragma TODO("这个数据应该放在StateMachine还是Actor")
	LC_ActorBase* pkActor = (LC_ActorBase*)pkMachine;
	if (pkActor&&pkActor->GetAIFuncValue(SF_IDLE_LIFE_TIME)==SFT_DEFAULT)
		_setLastIdleTime(pkMachine,fCurrentTime,fDeltaTime);

	return SCRT_SUCCESS;
}
//------------------------------------------------------
StateProcessType SM_ServerNPCState_IDLE::StateProcess(SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime )
{
	SM_NPCState_IDLE::StateProcess(pkMachine,fCurrentTime,fDeltaTime);
	LC_ServerNPC* pkServerNPC = (LC_ServerNPC*)pkMachine;
	//Utility::UT_TimeManager::GetSingletonPtr()->Tick();
	//float time1=GET_PROCESS_TIME;
//	int lCharType = pkServerNPC->GetCharType();
	if(pkServerNPC&&pkServerNPC->GetAIFuncValue(SF_IDLE_CAN_FOLLOW)== SFT_DEFAULT)
	{
		if(SPT_END == _transiteFollowState(pkMachine,fCurrentTime,fDeltaTime))
			return SPT_END;
	}
	//Utility::UT_TimeManager::GetSingletonPtr()->Tick();
	//float time2=GET_PROCESS_TIME;
	if (pkServerNPC&&pkServerNPC->GetAIFuncValue(SF_IDLE_POSITIVE_ACTIVE) == SFT_DEFAULT)
		_positiveActive(pkMachine, fCurrentTime, fDeltaTime);

	//Utility::UT_TimeManager::GetSingletonPtr()->Tick();
	//float time3=GET_PROCESS_TIME;
	//处理技能
	if (pkServerNPC&&pkServerNPC->GetAIFuncValue(SF_IDLE_UPDATE_EXECUTE_SKILL) == SFT_DEFAULT)
	{
		if (SPT_END == _processFight(pkMachine, fCurrentTime, fDeltaTime))
			return SPT_END;
	}
	//Utility::UT_TimeManager::GetSingletonPtr()->Tick();
	//float time4=GET_PROCESS_TIME;
	if (pkServerNPC&&pkServerNPC->GetAIFuncValue(SF_IDLE_LIFE_TIME)== SFT_DEFAULT)
	{
		if (SPT_END == _transiteWanderState(pkMachine, fCurrentTime, fDeltaTime))
			return SPT_END;
	}
	//Utility::UT_TimeManager::GetSingletonPtr()->Tick();
	//float time5=GET_PROCESS_TIME;
	if (pkServerNPC&&pkServerNPC->GetAIFuncValue(SF_IDLE_CAN_TALK)== SFT_DEFAULT)
	{
		if (SPT_END == _transiteTalkState(pkMachine, fCurrentTime, fDeltaTime))
			return SPT_END;
	}
	return SPT_SUSPEND;
}
//------------------------------------------------------
StateChangeResultType SM_ServerNPCState_IDLE::StateEnd(SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime )
{
	return SCRT_SUCCESS;
}
//------------------------------------------------------
StateProcessType SM_ServerNPCState_IDLE::_processFight(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime)
{
	LC_ServerNPC* pkServerNPC = (LC_ServerNPC*)pkMachine;

	LC_ActorSkillList* pkSkillList = pkServerNPC->GetActorSkillList();
	LC_ActorSkillTrigger* pkSkillTrigger = pkServerNPC->GetSkillTrigger();

	LC_ActorSkillEntry* pkEntry = pkSkillList->GetFirstSkillEntry();
	if (pkEntry != NULL && pkEntry->GetValid())
	{
		//设置挂起技能时间
		pkEntry->SetStartTime(GET_PROCESS_TIME);
		pkServerNPC->SetLockedTargetID(pkEntry->GetLockedTargetID());
		ResultType iResult = pkServerNPC->PendingSkill(pkEntry->GetSkillTypeID(),0);
		if (RE_SKILL_PENDING_SUCCESS != iResult)
		{
			pkSkillList->RemoveFirstSkillEntry();
		}
	}

	//处理NPC技能
	SK_SkillExecute* pkPendingSkill = pkServerNPC->GetPendingSkillExecute();
	if (NULL != pkPendingSkill && pkServerNPC->GetDyingSkillID() != pkPendingSkill->GetExecuteID())
	{
		pkServerNPC->SetNextState(ST_NPC_CHASE);
		return SPT_END;
	}

	return SPT_SUSPEND;
}
//------------------------------------------------------
void SM_ServerNPCState_IDLE::_setLastIdleTime( StateMachine::SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime )
{
	LC_ServerNPC* pkServerNPC = (LC_ServerNPC*)pkMachine;
	pkServerNPC->SetLastIdleTime(fCurrentTime);
}
//------------------------------------------------------
void SM_ServerNPCState_IDLE::_positiveActive( StateMachine::SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime )
{
	LC_ServerNPC* pkServerNPC = (LC_ServerNPC*)pkMachine;

	float fLastPositiveActiveTime = pkServerNPC->GetLastPositiveActiveTime();
	float fRandDeltaTime = 0.25f + UT_MathBase::RandFloat01() * 0.75f;
	if(fCurrentTime - fLastPositiveActiveTime < fRandDeltaTime)
		return;

	pkServerNPC->SetLastPositiveActiveTime(fCurrentTime);

	//中立怪没有警戒行为
	if (pkServerNPC->GetFactionID() <= 0)
	{
		return;
	}

	//主动怪行为
	if(pkServerNPC->GetProperty_GuidRadius() > 0)
	{
		UT_Circle kGuidCircle;
		kGuidCircle.kCenterPoint = pkServerNPC->GetCurrentLocation();
		kGuidCircle.iRadius = pkServerNPC->GetProperty_GuidRadius();

		CF_ActorFilterFunc* pkCSVActorFilterFunc = CF_ActorFilterFunc::GetSingletonPtr();
		CF_ActorFilterFunc::DataEntry* pkFilterData = pkCSVActorFilterFunc->GetEntryPtr(NPC_ACTOR_FILTER_INDEX);
		if (NULL == pkFilterData)
		{
			return;
		}

		LC_MapBase* pkMap = pkServerNPC->GetMap();
		if(NULL == pkMap)
			return;

		LC_ActorPtrVector* pkTargetList = pkMap->LockActorsInCircle(kGuidCircle,pkFilterData);
		if (pkTargetList->empty())
		{
			return;
		}

		LC_ActorPtrVector::iterator beg = pkTargetList->begin();
		LC_ActorBase* pkTarget = NULL;
		for (;beg!=pkTargetList->end();++beg)
		{
			pkTarget = *beg;
			if (RE_SKILL_CHECK_VALID_TARGET == LC_Helper::IsValidTarget(pkServerNPC, pkTarget, pkFilterData))
			{
				//通知仇恨
				SK_ServerSkillEventManager* pkEventManager = SERVER_GET_SKILL_EVENT_MANAGER();
				SK_SkillEvent* pkEvent = pkEventManager->AllocEvent();
				if (NULL != pkEvent)
				{
					pkEvent->SetEventType(SETE_ENTER_GUARD);
					pkEvent->SetSrcActorID(pkTarget->GetID());
					pkEvent->SetDstActorID(pkServerNPC->GetID());
					pkEvent->SetLogicID(0);
					pkEventManager->AddEvent(pkEvent);
				}
			}
		}
		//Utility::UT_TimeManager::GetSingletonPtr()->Tick();
		//float time4=GET_PROCESS_TIME;
		//GfxWriteLog(LOG_SYSTEM_FILELINE,LOG_SWITCH_SYSTEM,"怪物名称：%s,%f,%f,%f,%f,%f",pkServerNPC->GetOwnerCharName().c_str(),time2-time1,time3-time2,time4-time3);
	}
}
//------------------------------------------------------
StateProcessType SM_ServerNPCState_IDLE::_transiteWanderState( StateMachine::SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime )
{
	LC_ServerNPC* pkServerNPC = (LC_ServerNPC*)pkMachine;
	
	float fProperty_IdleTime = pkServerNPC->GetProperty_IdleTime();
	float fLastIdleTime = pkServerNPC->GetLastIdleTime();
	//如果休闲时间足够
	if(fCurrentTime - fLastIdleTime > fProperty_IdleTime&&pkServerNPC->GetAIFuncValue(SF_WANDER_CAN_MOVE)==SFT_DEFAULT)
	{
		pkServerNPC->SetNextState(ST_NPC_WANDER);
		return SPT_END;
	}

	return SPT_SUSPEND;
}
//------------------------------------------------------
StateProcessType SM_ServerNPCState_IDLE::_transiteTalkState( StateMachine::SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime )
{
	LC_FactionManager* pkFactionManager = LC_FactionManager::GetSingletonPtr();
	if (NULL == pkFactionManager)
	{
		return SPT_SUSPEND;
	}

	LC_ServerNPC* pkServerNPC = (LC_ServerNPC*)pkMachine;

	object_id_type lLockedTargetID = pkServerNPC->GetLockedTargetID();
	if (IS_PLAYER_ID(lLockedTargetID))
	{
		LC_MapBase* pkMap = pkServerNPC->GetMap();
		if (NULL != pkMap)
		{
			LC_ActorBase* pkLockedTarget = (LC_ActorBase*)pkMap->FindObject(lLockedTargetID);
			if (NULL != pkLockedTarget && !pkFactionManager->GetEnemyRelation(pkServerNPC, pkLockedTarget))
			{
				pkServerNPC->SetNextState(ST_NPC_TALK);
				return SPT_END;
			}
		}	
	}

	return SPT_SUSPEND;
}
//------------------------------------------------------
StateProcessType SM_ServerNPCState_IDLE::_transiteFollowState(StateMachine::SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime)
{
	LC_ServerNPC* pkServerNPC = (LC_ServerNPC*)pkMachine;
	const UT_Vec3Int & kCurrentLocation = pkServerNPC->GetCurrentLocation();

	LC_MapBase* pkMap = pkServerNPC->GetMap();
	if(pkMap)
	{
		object_id_type lFollowedTargetID = pkServerNPC->GetFollowedTargetID();
		if (IS_PLAYER_ID(lFollowedTargetID) || IS_NPC_ID(lFollowedTargetID))
		{
			LC_LogicObject* pkTarget = pkMap->FindObject(lFollowedTargetID);
			if(pkTarget)
			{
				const UT_Vec3Int & kTargetLocation  = pkTarget->GetCurrentLocation();
				int iDistance = UT_MathBase::LineLengthXYInt(kCurrentLocation,kTargetLocation);
				if(iDistance > pkServerNPC->GetProperty_MaxDistanceToFollow())
				{
					pkMachine->SetNextState(ST_NPC_FOLLOW);
					return SPT_END;
				}
			}

			//回收npc
			if (NULL == pkTarget
				|| pkTarget->GetMapLogicID() != pkServerNPC->GetMapLogicID()
				|| pkTarget->GetMapResID() != pkServerNPC->GetMapResID())
			{
				pkServerNPC->SetDying(true);
			}
		}
	}

	return SPT_SUSPEND;
}