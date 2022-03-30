
#include "SM_ServerNPCState_WANDER.h"
#include "LC_ServerNPC.h"
#include "SK_SkillExecute.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "SK_ServerSkillEventManager.h"

#include "CF_Helper.h"
#include "UT_PathFindHelp.h"

using namespace StateMachine;
using namespace Utility;
using namespace CSVFile;
using namespace Skill;
using namespace GameLogic;
//------------------------------------------------------
SM_ServerNPCState_WANDER::SM_ServerNPCState_WANDER()
: SM_NPCState_WANDER()
{
}
//------------------------------------------------------
SM_ServerNPCState_WANDER::~SM_ServerNPCState_WANDER()
{
}
//------------------------------------------------------
bool SM_ServerNPCState_WANDER::CanTransite(StateMachine::SM_StateMachine* pkMachine,  SM_StateBase* pkToTransiteState)
{
	return SM_NPCState_WANDER::CanTransite(pkMachine,pkToTransiteState);
}
//------------------------------------------------------
StateChangeResultType SM_ServerNPCState_WANDER::StateStart( StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime )
{
	SM_NPCState_WANDER::StateStart(pkMachine,fCurrentTime,fDeltaTime);
	LC_ServerNPC* pkServerNPC = (LC_ServerNPC*)pkMachine;
	int lWanderPathID = pkServerNPC->GetMovePathID();
	
	pkServerNPC->SetLastIdleTime(fCurrentTime);

	if(lWanderPathID == 0)
		_computeWanderLocation(pkMachine);
	else
	{
		bool bNeedPath = true;
		if(pkServerNPC->GetAIFuncValue(SF_WANDER_CACHE_PATH)== SFT_DEFAULT)
		{
			if (pkServerNPC->IsCachedMovePath())
			{
				const Utility::UT_Vec3Int& rkTargetLocation = pkServerNPC->GetCacheLocation();
				pkServerNPC->SetTargetLocation(rkTargetLocation);
				bNeedPath = false;
			}
		}

		if(bNeedPath)
		{
			CF_MovePathList* pkCSVMovePathList = CF_MovePathList::GetSingletonPtr();
			CF_MovePathList::DataEntry* pkMoveData = pkCSVMovePathList->GetEntryPtr(lWanderPathID);
			if(!pkMoveData)
			{
				return SCRT_SUCCESS;
			}

			_initWanderPath(pkMachine,pkMoveData);
		}
	}

	return SCRT_SUCCESS;
}
//------------------------------------------------------
StateProcessType SM_ServerNPCState_WANDER::StateProcess(SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime )
{
	SM_NPCState_WANDER::StateProcess(pkMachine,fCurrentTime,fDeltaTime);
	LC_ServerNPC* pkServerNPC = static_cast<LC_ServerNPC*>(pkMachine);
// 	if (!pkNPC->GetCanMove())
// 	{
// 		pkNPC->SetNextState(ST_NPC_IDLE);
// 		pkNPC->SetChangedCurrentLocation(true);
// 		return SPT_END;
// 	}
//	int lCharType = pkServerNPC->GetCharType();
	//主动激活
	if (pkServerNPC&&pkServerNPC->GetAIFuncValue(SF_WANDER_POSTIVE_ACTIVE) == SFT_DEFAULT)
		_positiveActive(pkMachine, fCurrentTime, fDeltaTime);

	//处理技能
	if (pkServerNPC&&pkServerNPC->GetAIFuncValue(SF_WANDER_UPDATE_EXECUTE_SKILL) == SFT_DEFAULT)
	{
		if (SPT_END == _processFight(pkMachine, fCurrentTime, fDeltaTime))
			return SPT_END;
	}

	if (pkServerNPC&&pkServerNPC->GetAIFuncValue(SF_WANDER_CAN_TALK)== SFT_DEFAULT)
	{
		if (SPT_END == _transiteTalkState(pkMachine, fCurrentTime, fDeltaTime))
		return SPT_END;
	}

	return _processWander(pkMachine,fCurrentTime);
}
//------------------------------------------------------
StateChangeResultType SM_ServerNPCState_WANDER::StateEnd( SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime )
{
	LC_ServerNPC* pkServerNPC = static_cast<LC_ServerNPC*>(pkMachine);
	
	if(pkServerNPC->GetAIFuncValue(SF_WANDER_CACHE_PATH)== SFT_DEFAULT)
		pkServerNPC->CacheCurrentPath(pkServerNPC->GetCurrentLocation());
	
	return SCRT_SUCCESS;
}
//------------------------------------------------------
void SM_ServerNPCState_WANDER::_initWanderPath(SM_StateMachine* pkMachine,CSVFile::CF_MovePathList::DataEntry* pkCSVData)
{
	LC_ServerNPC* pkServerNPC = (LC_ServerNPC*)pkMachine;
	if (pkServerNPC == NULL)
		return;

	UT_LinePath& rkPath = pkServerNPC->GetMovePath();

	rkPath.Clear();
	//rkPath.SetLinePathType(UT_LinePath::LPT_FLIP);
	rkPath.SetLinePathType((UT_LinePath::LinePathType)pkCSVData->_iPathType);

	int nPathPointCount = pkCSVData->_iPathPointCount;

	for (int nIndex = 0; nIndex < nPathPointCount; ++nIndex)
	{
		Utility::UT_Vec3Int point= CSVHelper::GetMovePathPoint(pkCSVData,nIndex);
		if (nIndex==0)
		{
			if (UT_MathBase::LineLengthXYInt(point, pkServerNPC->GetCurrentLocation()) >= 10)
			{
				rkPath.SetStartPoint(pkServerNPC->GetCurrentLocation());
				rkPath.AddPathPoint(pkServerNPC->GetCurrentLocation());
			}
			else
			{
				rkPath.SetStartPoint(point);
			}
		}
		else if (nIndex==nPathPointCount-1)
		{
			rkPath.SetTargetPoint(point);
		}
		rkPath.AddPathPoint(point );
	}
	rkPath.GeneratePath();
	pkServerNPC->SetTargetLocation(rkPath.GetTargetPoint());
}
//------------------------------------------------------
void SM_ServerNPCState_WANDER::_computeWanderLocation( StateMachine::SM_StateMachine* pkMachine )
{
	LC_ServerNPC* pkServerNPC = (LC_ServerNPC*)pkMachine;

	const UT_Vec3Int & kNPCSpawnLocation = pkServerNPC->GetSpawnLocation();
	int iWanderRaidus = pkServerNPC->GetProperty_WanderRadius();
	//UT_Vec3Int kCurrentLocation = pkServerNPC->GetCurrentLocation();

	UT_Vec3Int kWanderLocation;

	if (iWanderRaidus > 0)
	{
		//UT_PathFindHelp::GetNextPosInCircle(pkServerNPC,kNPCSpawnLocation.x,kNPCSpawnLocation.y,iWanderRaidus / 2,iWanderRaidus,0.5f + UT_MathBase::RandFloat01()*0.5f,true,kWanderLocation);
		if(UT_PathFindHelp::GenRandomPosInCircle(pkServerNPC->GetMapResID(), kNPCSpawnLocation,iWanderRaidus / 2,iWanderRaidus,kWanderLocation))
		{
			pkServerNPC->SetTargetLocation(kWanderLocation);
		}
	}
}	
//------------------------------------------------------
StateProcessType SM_ServerNPCState_WANDER::_processWander( StateMachine::SM_StateMachine* pkMachine , float fCurrentTime )
{
	LC_ServerNPC* pkServerNPC = (LC_ServerNPC*)pkMachine;

	int lWanderPathID = pkServerNPC->GetMovePathID();

	if (pkServerNPC->GetAIFuncValue(SF_WANDER_CAN_MOVE) != SFT_DEFAULT)
	{
		pkServerNPC->SetNextState(ST_NPC_IDLE);
	
		return SPT_END;
	}

	if( 0 == lWanderPathID && fCurrentTime - pkServerNPC->GetLastIdleTime() > 5.0f)
	{
		return SPT_END;
	}

	bool bMoveEndPath = false;

	UT_LinePath& rkPath = pkServerNPC->GetMovePath();
	if (rkPath.GetPathPointCount())
	{
		if ( rkPath.GetMoveStep() >= 1.0f )
			bMoveEndPath = true;
	}
	
	{
		const UT_Vec3Int & kCurrentLocation = pkServerNPC->GetCurrentLocation();
		const UT_Vec3Int & kTargetLocation = pkServerNPC->GetTargetLocation();
		int nDistance = UT_MathBase::LineLengthXYInt(kCurrentLocation, kTargetLocation);
		if (nDistance < DISTANCE_ADJUST)
		{
			pkServerNPC->SetCurrentLocation(kCurrentLocation);
			rkPath.Clear();
			bMoveEndPath = true;
		}
	}

	if(bMoveEndPath)
	{
		if (pkServerNPC->GetAIFuncValue(SF_WANDER_CACHE_PATH)== SFT_DEFAULT && pkServerNPC->IsCachedMovePath())
		{
			pkServerNPC->RecoverCurrentPath();
			return SPT_SUSPEND;
		}
		else
			return SPT_END;
	}
	else
		return SPT_SUSPEND;
}
//------------------------------------------------------
StateProcessType SM_ServerNPCState_WANDER::_processFight(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime)
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
void SM_ServerNPCState_WANDER::_positiveActive( StateMachine::SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime )
{
	LC_ServerNPC* pkServerNPC = (LC_ServerNPC*)pkMachine;

	//中立怪没有警戒行为
	if (pkServerNPC->GetFactionID() <= 0)
	{
		return;
	}

	float fLastPositiveActiveTime = pkServerNPC->GetLastPositiveActiveTime();
	float fRandDeltaTime = 0.25f + UT_MathBase::RandFloat01() * 0.75f;
	if(fCurrentTime - fLastPositiveActiveTime < fRandDeltaTime)
		return;

	pkServerNPC->SetLastPositiveActiveTime(fCurrentTime);

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
		if (!pkTargetList->empty())
		{
			LC_ActorPtrVectorIter iter;
			LC_ActorBase* pkTarget = NULL;
			TRAVP(pkTargetList, iter)
			{
				pkTarget = *iter;
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
		}
	}
}
//------------------------------------------------------
StateMachine::StateProcessType SM_ServerNPCState_WANDER::_transiteTalkState( StateMachine::SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime )
{
	LC_ServerNPC* pkNPC = (LC_ServerNPC*)pkMachine;
	//存在对话的玩家
	if (IS_PLAYER_ID(pkNPC->GetLockedTargetID()))
	{
		pkNPC->SetNextState(ST_NPC_TALK);
		return SPT_END;
	}

	return SPT_SUSPEND;
}
//------------------------------------------------------
