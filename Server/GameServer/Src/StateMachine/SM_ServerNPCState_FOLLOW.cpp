

#include "LC_ServerNPC.h"
#include "SM_ServerNPCState_FOLLOW.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "SK_ServerSkillEventManager.h"

using namespace StateMachine;
using namespace CSVFile;
using namespace GameLogic;
using namespace Skill;
//------------------------------------------------------
SM_ServerNPCState_FOLLOW::SM_ServerNPCState_FOLLOW()
: SM_NPCState_FOLLOW()
{
}
//------------------------------------------------------
SM_ServerNPCState_FOLLOW::~SM_ServerNPCState_FOLLOW()
{
}
//------------------------------------------------------
bool SM_ServerNPCState_FOLLOW::CanTransite(SM_StateMachine* pkMachine,SM_StateBase* pkToTransiteState)
{
	return true;
}
//------------------------------------------------------
StateChangeResultType SM_ServerNPCState_FOLLOW::StateStart(SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime)
{
	SM_NPCState_FOLLOW::StateStart(pkMachine,fCurrentTime,fDeltaTime);

	return SCRT_SUCCESS;
}
//------------------------------------------------------
StateProcessType SM_ServerNPCState_FOLLOW::StateProcess(SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime )
{
	SM_NPCState_FOLLOW::StateProcess(pkMachine,fCurrentTime,fDeltaTime);
	
	LC_ServerNPC* pkServerNPC = (LC_ServerNPC*)pkMachine;

	//主动激活
	if (pkServerNPC&&pkServerNPC->GetAIFuncValue(SF_FOLLOW_POSITIVE_ACTIVE) == SFT_DEFAULT)
		_positiveActive(pkMachine, fCurrentTime, fDeltaTime);

	//处理技能
	if (pkServerNPC&&pkServerNPC->GetAIFuncValue(SF_FOLLOW_UPDATE_EXECUTE_SKILL) == SFT_DEFAULT)
	{
		if (SPT_END == _processFight(pkMachine, fCurrentTime, fDeltaTime))
			return SPT_END;
	}

	return _processFollow(pkMachine);
}
//------------------------------------------------------
StateChangeResultType SM_ServerNPCState_FOLLOW::StateEnd(SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime )
{
	return SCRT_SUCCESS;
}
//------------------------------------------------------
StateProcessType SM_ServerNPCState_FOLLOW::_processFollow( StateMachine::SM_StateMachine* pkMachine )
{
	LC_ServerNPC* pkServerNPC = (LC_ServerNPC*)pkMachine;
	const UT_Vec3Int & kCurrentLocation = pkServerNPC->GetCurrentLocation();
	
	LC_MapBase* pkMap = pkServerNPC->GetMap();
	if(pkMap)
	{
		object_id_type lTargetID = pkServerNPC->GetFollowedTargetID();
		if (IS_PLAYER_ID(lTargetID) || IS_NPC_ID(lTargetID))
		{
			LC_LogicObject* pkTarget = pkMap->FindObject(lTargetID);
			if(pkTarget)
			{
				
				//有些跟随NPC是原地不动的
				if(pkServerNPC->GetMoveSpeed() > 0)
				{
					const UT_Vec3Int & kTargetLocation  = pkTarget->GetCurrentLocation();

					int iDistance = UT_MathBase::LineLengthXYInt(kCurrentLocation,kTargetLocation);
					if(iDistance >  pkServerNPC->GetProperty_MinDistanceToFollow())
					{
						UT_Vec3Int realPos = kCurrentLocation - kTargetLocation;
						UT_Vec3Float krr = UT_MathBase::ConvertVec3IntToFloat(kTargetLocation);
						krr = krr + realPos.Normalize() * (float)pkServerNPC->GetProperty_MinDistanceToFollow();
						//跟目标距离过远，瞬移到目标身边
						if (iDistance > 2000 && pkServerNPC->GetAIFuncValue(SF_FOLLOW_CAN_TRANSFER) == SFT_DEFAULT)
						{
							pkServerNPC->GetMovePath().Clear();
							pkServerNPC->SetCurrentLocation(UT_MathBase::ConvertVec3FloatToInt(krr));
							pkServerNPC->SetTargetLocation(UT_MathBase::ConvertVec3FloatToInt(krr));
							pkServerNPC->SetMoveLineTargetLocation(UT_MathBase::ConvertVec3FloatToInt(krr));
						}
						else if (iDistance > 2000 && pkServerNPC->GetAIFuncValue(SF_FOLLOW_CAN_TRANSFER) == SFT_TYPE_1)
						{
							pkServerNPC->SetFollowedTargetID(0);
						}
						else	//跟随目标
						{
							pkServerNPC->SetTargetLocation(UT_MathBase::ConvertVec3FloatToInt(krr));
						}
						return SPT_SUSPEND;
					}
					else
					{
						pkServerNPC->SetTargetLocation(kCurrentLocation);
 					}
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
	
	pkMachine->SetNextState(ST_NPC_IDLE);
	return SPT_END;
}
//------------------------------------------------------
StateProcessType SM_ServerNPCState_FOLLOW::_processFight(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime)
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
	if (NULL != pkPendingSkill && pkServerNPC->GetDyingSkillID() != pkPendingSkill->GetExecuteID() )
	{
		pkServerNPC->SetNextState(ST_NPC_CHASE);
		return SPT_END;
	}

	return SPT_SUSPEND;
}
//------------------------------------------------------
void SM_ServerNPCState_FOLLOW::_positiveActive( StateMachine::SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime )
{
	LC_ServerNPC* pkServerNPC = (LC_ServerNPC*)pkMachine;

	float fLastPositiveActiveTime = pkServerNPC->GetLastPositiveActiveTime();
	float fRandDeltaTime = UT_MathBase::RandFloat01();
	if(fCurrentTime - fLastPositiveActiveTime < fRandDeltaTime)
		return;

	pkServerNPC->SetLastPositiveActiveTime(fCurrentTime);

	if(pkServerNPC->GetProperty_GuidRadius() > 0)
	{
		UT_Circle kGuidCircle;
		kGuidCircle.kCenterPoint	= pkServerNPC->GetCurrentLocation();
		kGuidCircle.iRadius			= pkServerNPC->GetProperty_GuidRadius();

		CF_ActorFilterFunc* pkCSVActorFilterFunc = CF_ActorFilterFunc::GetSingletonPtr();
		CF_ActorFilterFunc::DataEntry* pkFilterData = pkCSVActorFilterFunc->GetEntryPtr(1);
		if (NULL == pkFilterData)
		{
			return;
		}
		
		LC_MapBase* pkMap = pkServerNPC->GetMap();
		if(NULL == pkMap)
			return;

		LC_ActorPtrVector* pkPlayerList = pkMap->LockActorsInCircle(kGuidCircle,pkFilterData);
		if(!pkPlayerList->empty())
		{
			LC_ActorPtrVectorIter iter;
			TRAVP(pkPlayerList, iter)
			{
				//如果发现死亡的玩家，不作处理
				if ( (*iter)->GetID() == pkServerNPC->GetID() || (*iter)->GetDead() || (*iter)->GetSkillFlag(SK_INVISIBLE) )
					continue;

				object_id_type lPlayerID = (*iter)->GetID();

				SK_ServerSkillEventManager* pkEventManager = SERVER_GET_SKILL_EVENT_MANAGER();
				SK_SkillEvent* pkEvent = pkEventManager->AllocEvent();
				if (NULL != pkEvent)
				{
					pkEvent->SetEventType(SETE_ENTER_GUARD);
					pkEvent->SetSrcActorID(lPlayerID);
					pkEvent->SetDstActorID(pkServerNPC->GetID());
					pkEvent->SetLogicID(0);
					pkEventManager->AddEvent(pkEvent);
				}
				break;
			}
		}
	}
}