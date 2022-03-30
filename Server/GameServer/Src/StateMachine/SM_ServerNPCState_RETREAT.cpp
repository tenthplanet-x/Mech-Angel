#include "SM_ServerNPCState_RETREAT.h"
#include "LC_ServerNPC.h"
#include "LC_WorldManagerBase.h"
#include "SK_ServerSkillEventManager.h"
#include "LC_NPCGroupBase.h"
#include "LC_LogicManagerBase.h"
#define RETREAT_OVERTIME	8.0f

using namespace Utility;
using namespace GameLogic;
using namespace StateMachine;
using namespace Skill;
//------------------------------------------------------
SM_ServerNPCState_RETREAT::SM_ServerNPCState_RETREAT()
: SM_NPCState_RETREAT()
{
}
//------------------------------------------------------
SM_ServerNPCState_RETREAT::~SM_ServerNPCState_RETREAT()
{
}
//------------------------------------------------------
bool SM_ServerNPCState_RETREAT::CanTransite(StateMachine::SM_StateMachine* pkMachine, SM_StateBase* pkToTransiteState)
{
	return SM_NPCState_RETREAT::CanTransite(pkMachine,pkToTransiteState);
}
//------------------------------------------------------
StateChangeResultType SM_ServerNPCState_RETREAT::StateStart( StateMachine::SM_StateMachine* pkMachine,  float fCurrentTime,float fDeltaTime )
{
	SM_NPCState_RETREAT::StateStart(pkMachine,fCurrentTime,fDeltaTime);
	LC_ServerNPC* pkNPC = (LC_ServerNPC*)pkMachine;

	if (pkNPC->GetAIFuncValue(SF_RETREAT_IMMORTAL) == SFT_DEFAULT)
		_addImmortalState(pkMachine);

	if (pkNPC->GetHPRecoverRate() > 0)
		pkNPC->SetAutoRecover(true);
	
	pkNPC->GetMovePath().Clear();
	
	pkNPC->SetTargetLocation(pkNPC->GetActiveLocation());
	pkNPC->GetNPCOwnerList().Clear();
	pkNPC->ResetActiveInfo();
	pkNPC->SetLockedTargetID(0);
	pkNPC->ClearPendingSkill();
	pkNPC->SetShapeShiftInfo(0);
	pkNPC->BeginRetreatAccumulate();	//回撤计时

	//如果存在召唤物，则在回撤的时候干掉他
	
	if(pkNPC->GetSummorID() != 0)
	{
		LC_WorldManagerBase* pkWorldManager = ENGINE_GET_WORLD_MANAGER();
		LC_ActorBase* pkActor = (LC_ActorBase*)(pkWorldManager->FindObject(pkNPC->GetSummorID()));
		if(pkActor != NULL)
		{
			pkActor->SetAttackerID(0);
			pkActor->SetHP(0);
		}
	}

	//只留下出生自带的状态，清除其余状态
	_removeStateExceptBornState(pkMachine);

	//NPC存在组队行为
	LC_NPCGroupBase* pkGroup = pkNPC->GetNPCGroup();
	if (NULL != pkGroup)
	{
		//回撤刷新怪物
		LC_NPCGroupSpawnType eSpawnType = pkGroup->GetGroupSpawnType();
		if (NGST_GROUP_RESPAWN == eSpawnType || NGST_GROUP_DEAD_OVER == eSpawnType)
		{
			pkGroup->SetSpawnActive(true);
		}

		//集体回撤
		if (pkGroup->GetGroupRetreate())
		{
			SK_ServerSkillEventManager* pkEventManager = SERVER_GET_SKILL_EVENT_MANAGER();
			SK_SkillEvent* pkEvent = pkEventManager->AllocEvent();
			if (NULL != pkEvent)
			{
				pkEvent->SetEventType(SETE_RETREAT);
				pkEvent->SetSrcActorID(pkNPC->GetID());
				pkEvent->SetDstActorID(0);
				pkEvent->SetLogicID(0);
				pkEventManager->AddEvent(pkEvent);
			}
		}
	}

	return SCRT_SUCCESS;
}
//------------------------------------------------------
StateProcessType SM_ServerNPCState_RETREAT::StateProcess(SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime )
{
	SM_NPCState_RETREAT::StateProcess(pkMachine,fCurrentTime,fDeltaTime);
	
	LC_ServerNPC* pkNPC = (LC_ServerNPC*)pkMachine;
	pkNPC->StepRetreatAccumulate(fDeltaTime);

	const UT_Vec3Int & kCurrentLocation = pkNPC->GetCurrentLocation();
	const UT_Vec3Int & kTargetLocation = pkNPC->GetActiveLocation();
	int iDistance = UT_MathBase::LineLengthXYInt(kCurrentLocation,kTargetLocation);
	if(iDistance <= DISTANCE_ADJUST)
	{
		pkMachine->SetNextState(ST_NPC_IDLE);
		return SPT_END; 
	}
	else if ( pkNPC->GetRetreatAccumulate() > RETREAT_OVERTIME)
	{
		pkMachine->SetNextState(ST_NPC_IDLE);
		if ( pkNPC->GetActiveLocation().x != 0 && pkNPC->GetActiveLocation().y != 0 )
		{
			pkNPC->SetCurrentLocation(pkNPC->GetActiveLocation());
			pkNPC->SetTargetLocation(pkNPC->GetActiveLocation());
		}
		return SPT_END; 
	}
	else
		return SPT_SUSPEND;
}
//------------------------------------------------------
StateChangeResultType SM_ServerNPCState_RETREAT::StateEnd( SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime )
{
	LC_ServerNPC* pkNPC = (LC_ServerNPC*)pkMachine;

	pkNPC->GetMovePath().Clear();
	pkNPC->GetNPCOwnerList().Clear();
	pkNPC->ResetActiveInfo();
	pkNPC->SetLockedTargetID(0);
	pkNPC->ClearPendingSkill();
	pkNPC->EndRetreatAccumulate();		//回撤计时
	pkNPC->ResetHPRecoverTimer();
	
	//清空技能队列
	LC_ActorSkillList* pkActorSkillList = pkNPC->GetActorSkillList();
	pkActorSkillList->Reset();

	//清空伤害统计
	if(pkNPC->DenyClearEncountedStat() == 0)
	{
		pkNPC->ClearEncountedStat(LC_ECT_TP_Damage);
	}
		
	//清空仇恨数据
	LC_ActorHatredList* pkHatredList = pkNPC->GetHatredList();
	pkHatredList->Reset();
	if (pkNPC->GetHPRecoverRate() > 0)
		pkNPC->SetAutoRecover(true);
	//重置技能触发器
	LC_ActorSkillTrigger* pkSkillTrigger = pkNPC->GetSkillTrigger();
	pkSkillTrigger->ResetExecuteInfo();

	//清空技能AI数据
	pkNPC->ClearAIMutableParameters();

	if (pkNPC->GetAIFuncValue(SF_RETREAT_IMMORTAL) == SFT_DEFAULT)
		_removeImmortalState(pkMachine);

	//恢复生命值和内息
	if (pkNPC->GetAIFuncValue(SF_RETREAT_RECOVER_HP) == SFT_DEFAULT && pkNPC->GetHPRecoverRate() == 0 && !pkNPC->GetDenyAutoRecover())
		_recoverHP(pkMachine);

	if (pkNPC->GetAIFuncValue(SF_RETREAT_RECOVER_MP) == SFT_DEFAULT && !pkNPC->GetDenyAutoRecover())
		_recoverMP(pkMachine);

	//只留下出生自带的状态，清除其余状态
	_removeStateExceptBornState(pkMachine);

	SM_NPCState_RETREAT::StateEnd(pkMachine, fCurrentTime, fDeltaTime);
	return SCRT_SUCCESS;
}
//--------------------------------------------------------------
void SM_ServerNPCState_RETREAT::_addImmortalState( StateMachine::SM_StateMachine* pkMachine)
{
	LC_ServerNPC* pkNPC = (LC_ServerNPC*)pkMachine;
	pkNPC->SetSkillFlag(SK_IMMORTAL, true);		//添加无敌状态
}
//--------------------------------------------------------------
void SM_ServerNPCState_RETREAT::_removeImmortalState(StateMachine::SM_StateMachine* pkMachine)
{
	LC_ServerNPC* pkNPC = (LC_ServerNPC*)pkMachine;
	pkNPC->SetSkillFlag(SK_IMMORTAL, false);	//删除无敌状态
}
//--------------------------------------------------------------
void SM_ServerNPCState_RETREAT::_recoverHP( StateMachine::SM_StateMachine* pkMachine )
{
	LC_ServerNPC* pkNPC = (LC_ServerNPC*)pkMachine;
	pkNPC->SetHP(pkNPC->GetAttributeMap()->GetAttribute(ATT_MAX_HP));
}
//--------------------------------------------------------------
void SM_ServerNPCState_RETREAT::_recoverMP( StateMachine::SM_StateMachine* pkMachine )
{
	LC_ServerNPC* pkNPC = (LC_ServerNPC*)pkMachine;
	pkNPC->SetMP((int32_t)pkNPC->GetAttributeMap()->GetAttribute(ATT_MAX_MP));
}
//--------------------------------------------------------------
StateMachine::StateHandleEventType SM_ServerNPCState_RETREAT::GetDefaultEventHandleType( Skill::SK_SkillEvent* pkSkillEvent )
{
	Skill::SkillEventTypeEx eEventType = pkSkillEvent->GetEventType();

	if (eEventType == Skill::SETE_DAMAGE ||
		eEventType == Skill::SETE_ATTACH_STATE ||
		eEventType == Skill::SETE_BROADCAST_ENEMY ||
		eEventType == Skill::SETE_ENTER_GUARD)
		return SHET_IGNORE; 

	return SHET_DEFAULT;
}
//------------------------------------------------------
void SM_ServerNPCState_RETREAT::_removeStateExceptBornState(StateMachine::SM_StateMachine* pkMachine)
{
	LC_ServerNPC* pkNPC = (LC_ServerNPC*)pkMachine;
	LC_ActorStateMap* pkStateMap = pkNPC->GetStateMap();
	if (NULL != pkStateMap)
	{
		FilerStateOPerNot oper(SMT_KIND, 5, 0);
		pkStateMap->ForAllState(&oper);
		for(LC_SkillIDSet::iterator ite = oper.kStates.begin(); ite != oper.kStates.end(); ite++)
		{
			pkStateMap->RemoveSkillStateInStateMap(*ite);
		}
	}
}
//------------------------------------------------------
