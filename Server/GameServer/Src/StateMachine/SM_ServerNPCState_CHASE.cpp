#include "UT_PathFindHelp.h"
#include "SM_ServerNPCState_CHASE.h"
#include "LC_ServerNPC.h"
#include "PF_ServerPathManager.h"

using namespace StateMachine;
using namespace Utility;
using namespace GameLogic;
using namespace Skill;
using namespace CSVFile;

//------------------------------------------------------
SM_ServerNPCState_CHASE::SM_ServerNPCState_CHASE()
: SM_NPCState_CHASE()
{
}
//------------------------------------------------------
SM_ServerNPCState_CHASE::~SM_ServerNPCState_CHASE()
{
}
//------------------------------------------------------
bool SM_ServerNPCState_CHASE::CanTransite(SM_StateMachine* pkMachine,  SM_StateBase* pkToTransiteState)
{
	return true;
}
//------------------------------------------------------
StateChangeResultType SM_ServerNPCState_CHASE::StateStart(SM_StateMachine* pkMachine,  float fCurrentTime,float fDeltaTime)
{
	SM_NPCState_CHASE::StateStart(pkMachine,fCurrentTime,fDeltaTime);
	LC_ServerNPC* pkNPC = static_cast<LC_ServerNPC*>(pkMachine);
	if (NULL != pkNPC)
		pkNPC->ResetChaseTime(fCurrentTime);
// 	if (pkNPC&&pkNPC->GetAIFuncValue(SF_CHASE_RESET_SPAWN_TIME) == SFT_DEFAULT)
// 		pkNPC->SetSurvivalStartTime(fCurrentTime);
	
	return SCRT_SUCCESS;
}
//------------------------------------------------------
StateProcessType SM_ServerNPCState_CHASE::StateProcess(SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime )
{
	SM_NPCState_CHASE::StateProcess(pkMachine,fCurrentTime,fDeltaTime);
	LC_ServerNPC* pkNPC = static_cast<LC_ServerNPC*>(pkMachine);

	if (pkNPC->GetAIFuncValue(SF_CHASE_UPDATE_EXECUTE_SKILL) == SFT_DEFAULT)
	{
		if (SPT_END == _processFight(pkMachine, fCurrentTime, fDeltaTime))
			return SPT_END;
	}

	if(pkNPC->GetAIFuncValue(SF_CHASE_CHECK_RETREAT) == SFT_DEFAULT)
	{
		if (SPT_END == _checkChaseDistanceToRetreat(pkMachine))
			return SPT_END;

		/*if (SPT_END == _checkChaseTargetToRetreat(pkMachine))
			return SPT_END;*/
	}
	

	if(pkNPC->GetAIFuncValue(SF_CHASE_CHECK_FOLLOW) == SFT_DEFAULT)
	{
		if (SPT_END == _checkChaseDistanceToFollow(pkMachine))
			return SPT_END;
	}	


	///*
	//*判断npc是否在等待cd，判断方法：每一个待释放的技能是否可以正常释放
	//*如果存在不能正常释放的技能，npc开始追击目标
	//*/
	//bool bNPCWaitCD = false;
	//LC_ActorSkillList* pkSkillList = pkNPC->GetActorSkillList();
	//LC_ActorSkillTrigger* pkSkillTrigger = pkNPC->GetSkillTrigger();
	////当前技能列表中不存在技能，则判断技能触发器中待释放的技能
	//if (NULL != pkSkillList 
	//	&& NULL != pkSkillList->GetFirstSkillEntry() 
	//	&& !pkSkillList->GetFirstSkillEntry()->GetValid()
	//	&& NULL != pkSkillTrigger)
	//{
	//	SK_Factory* pkSkillFactory = SK_Factory::GetSingletonPtr();
	//	SK_SkillExecute* pkSkill = NULL;

	//	LC_SkillTriggerEntry* pkTriggerEntry = NULL;

	//	LC_SkillTriggerIndexList kSkillList = pkSkillTrigger->GetSkillInCD();
	//	LC_SkillTriggerIndexListIter kSkillIter;
	//	TRAV(kSkillList, kSkillIter)
	//	{
	//		pkTriggerEntry = pkSkillTrigger->GetTriggerByIndex(*kSkillIter);
	//		if (NULL != pkTriggerEntry)
	//		{
	//			pkSkill = pkSkillFactory->RequestSkillExcute(pkTriggerEntry->GetSkillTypeID());
	//			if (RE_SKILL_CAN_TAKE_PLACE == pkNPC->CheckExecuteSkill(fCurrentTime, pkSkill, 0))
	//			{
	//				bNPCWaitCD = true;
	//			}
	//			else
	//			{
	//				//存在一个不能释放的技能，本轮判断结束，npc开始追击目标
	//				bNPCWaitCD = false;
	//				break;
	//			}
	//		}
	//	}
	//}
	//if (!bNPCWaitCD)
	//{
	//	_keepChaseDistance(pkMachine);
	//}
	//else
	//{
	//	//npc原地待命
	//	UT_LinePath& rkMovePath = pkNPC->GetMovePath();
	//	rkMovePath.Clear();
	//	pkNPC->SetTargetLocation(pkNPC->GetCurrentLocation());
	//}

	//只有近程npc才需要设置追击目标点，远程npc由PrepareExecuteSkill为其设置目标点
	/*
	if (!pkNPC->GetProperty_IsLongRange())
	{
		_keepChaseDistance(pkMachine);
	}
	*/
	_keepChaseDistance(pkMachine,fCurrentTime,fDeltaTime);

	if (pkNPC->GetHatredList()->GetHatredCount() <= 0 && pkNPC->GetAdditionTargetID() == 0)
	{
		if(pkNPC->GetAIFuncValue(SF_CHASE_CHECK_RETREAT) == SFT_DEFAULT)
			pkNPC->SetNextState(ST_NPC_RETREAT);
		else
			pkNPC->SetNextState(ST_NPC_IDLE);
	}

	return SPT_SUSPEND;
}
//------------------------------------------------------
StateChangeResultType SM_ServerNPCState_CHASE::StateEnd(SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime )
{
	return SCRT_SUCCESS;
}
//------------------------------------------------------
StateMachine::StateProcessType SM_ServerNPCState_CHASE::_processFight( StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime )
{
	LC_ServerNPC* pkServerNPC = static_cast<LC_ServerNPC*>(pkMachine);
	
	LC_ActorSkillList* pkSkillList = pkServerNPC->GetActorSkillList();
	LC_ActorSkillTrigger* pkSkillTrigger = pkServerNPC->GetSkillTrigger();


	/*
	*以下情况删除SkillList中的技能
	*1、技能挂起失败
	*2、技能挂起后超时
	*3、技能挂起后释放失败
	*4、技能释放成功
	*/


	//检查技能列表中的技能是否有效，注意这部分技能检查工作应该与目标过滤的工作内容互斥
	_checkSkill(pkMachine, fCurrentTime, fDeltaTime);

	//挂起技能
	_pendingSkill(pkMachine, fCurrentTime, fDeltaTime);


	LC_ActorSkillEntry* pkSkillEntry = pkSkillList->GetFirstSkillEntry();
	SK_SkillExecute* pkPendingSkill = pkServerNPC->GetPendingSkillExecute();
	//如果当前挂起技能，必定挂起SkillList中的第一个技能
	if (NULL != pkSkillEntry && pkSkillEntry->GetValid() && NULL != pkPendingSkill 
		&& pkSkillEntry->GetSkillTypeID() == pkPendingSkill->GetExecuteID())
	{
		pkServerNPC->AIFillExecuteSkillParam(pkPendingSkill);
		ResultType eResult = pkServerNPC->CheckExecuteSkill(fCurrentTime,pkPendingSkill,0);
		switch (eResult)
		{
			//距离太近，不执行技能
		case RE_SKILL_DISTANCE_NEAR:
			{
				pkServerNPC->ClearPendingSkill();

				//释放技能成功，删除技能列表中相应项
				pkSkillTrigger->SetExecuteSkillTime(pkSkillEntry->GetTriggerIndex(), fCurrentTime);
				pkServerNPC->SetLastExecuteSkillTime(fCurrentTime);
				pkSkillList->RemoveFirstSkillEntry();

				return SPT_SUSPEND;
			}
			break;
		case RE_SKILL_DISTANCE_FAR:
			{
				//触发条件不够，继续等待
				//eResult = pkServerNPC->PrepareExecuteSkill(pkPendingSkill,0);
			}
			break;
		case RE_SKILL_CAN_TAKE_PLACE:
			{
				pkServerNPC->ExecuteSkill(fCurrentTime,pkPendingSkill,0);
				pkServerNPC->ClearPendingSkill();

				//释放技能成功，删除技能列表中相应项
				pkSkillTrigger->SetExecuteSkillTime(pkSkillEntry->GetTriggerIndex(), fCurrentTime);
				pkServerNPC->SetLastExecuteSkillTime(fCurrentTime);
				pkSkillList->RemoveFirstSkillEntry();

				return SPT_END;
			}
			break;
		default:
			{
				//技能执行错误
				pkServerNPC->ClearPendingSkill();
				pkSkillTrigger->SetExecuteSkillTime(pkSkillEntry->GetTriggerIndex(), fCurrentTime);
				pkServerNPC->SetLastExecuteSkillTime(fCurrentTime);
				pkSkillList->RemoveFirstSkillEntry();

				/*if(pkServerNPC->GetAIFuncValue(SF_CHASE_CHECK_RETREAT) == SFT_DEFAULT)
					pkServerNPC->SetNextState(ST_NPC_RETREAT);
				else
					pkServerNPC->SetNextState(ST_NPC_IDLE);*/
			}
			break;
		}
	}
	else
	{
		if ((NULL == pkSkillEntry || !pkSkillEntry->GetValid()) && NULL != pkPendingSkill)
		{
		}
		if (NULL != pkSkillEntry && pkSkillEntry->GetValid() && NULL != pkPendingSkill && pkSkillEntry->GetSkillTypeID() != pkPendingSkill->GetExecuteID())
		{
		}
	}

	return SPT_SUSPEND;
}
//------------------------------------------------------
StateMachine::StateProcessType SM_ServerNPCState_CHASE::_checkChaseDistanceToRetreat( StateMachine::SM_StateMachine* pkMachine )
{
	LC_ServerNPC* pkNPC = static_cast<LC_ServerNPC*>(pkMachine);
	const UT_Vec3Int & kCurrentLocation = pkNPC->GetCurrentLocation();
	const UT_Vec3Int & kActiveLocation = pkNPC->GetActiveLocation();
	int     iChaseRadius = pkNPC->GetProperty_ChaseRadius();

	//如果目前距离激活点太远
	if(UT_MathBase::LineLengthXYInt(kActiveLocation, kCurrentLocation) > iChaseRadius)
	{
		//转入撤退状态
		pkNPC->SetNextState(ST_NPC_RETREAT);
		pkNPC->ClearPendingSkill();
		return SPT_END;
	}

	return SPT_SUSPEND;
}
//------------------------------------------------------
StateProcessType SM_ServerNPCState_CHASE::_checkChaseTargetToRetreat( StateMachine::SM_StateMachine* pkMachine )
{
	LC_ServerNPC* pkNPC = static_cast<LC_ServerNPC*>(pkMachine);

	if(pkNPC->GetLockedTargetID() == 0)
	{
		pkNPC->SetNextState(ST_NPC_RETREAT);
		return SPT_END;
	}

	return SPT_SUSPEND;
}
//------------------------------------------------------
StateProcessType SM_ServerNPCState_CHASE::_checkChaseDistanceToFollow(StateMachine::SM_StateMachine* pkMachine)
{
	LC_ServerNPC* pkServerNPC = (LC_ServerNPC*)pkMachine;

	LC_MapBase* pkMap = pkServerNPC->GetMap();
	if(pkMap)
	{
		object_id_type lFollowedTargetID = pkServerNPC->GetFollowedTargetID();
		LC_LogicObject* pkTarget = pkMap->FindObject(lFollowedTargetID);
		if(pkTarget)
		{
			const UT_Vec3Int & kCurrentLocation = pkServerNPC->GetCurrentLocation();
			const UT_Vec3Int & kTargetLocation  = pkTarget->GetCurrentLocation();
			int iDistance = UT_MathBase::LineLengthXYInt(kCurrentLocation,kTargetLocation);
			if(iDistance >= pkServerNPC->GetProperty_MaxDistanceToFollow())
			{
				pkMachine->SetNextState(ST_NPC_FOLLOW);
				return SPT_END;
			}
		}
	}

	return SPT_SUSPEND;
}
//------------------------------------------------------
void SM_ServerNPCState_CHASE::_keepChaseDistance( StateMachine::SM_StateMachine* pkMachine, float fCurrentTime, float fDeltaTime)
{
	LC_FactionManager* pkFactionManager = LC_FactionManager::GetSingletonPtr();
	if (NULL == pkFactionManager)
		return;

	LC_ServerNPC* pkNPC = static_cast<LC_ServerNPC*>(pkMachine);
	LC_MapBase* pkMap = pkNPC->GetMap();
	if(pkMap)
	{
		object_id_type lTargetID = pkNPC->GetLockedTargetID();
		LC_LogicObject* pkTarget = pkMap->FindObject(lTargetID);
		//根据与目标的相对坐标，npc自动调整自身位置
		if (NULL != pkTarget && (IS_PLAYER_CLASS_ID(pkTarget->GetID()) || IS_NPC_ID(pkTarget->GetID())))
		{
			bool isSurrounder = pkNPC->GetIsSurrounder();
			if(pkNPC->TryFightSurrounder(GET_PROCESS_TIME,(LC_ActorBase*)pkTarget) && isSurrounder)
			{
				UT_Vec3Int kTargetLocation = ((LC_ActorBase*)pkTarget)->AddFightSurrounder(pkNPC);
				pkNPC->SetTargetLocation(kTargetLocation);
			}
			else if (pkNPC->CheckChaseValid(fCurrentTime))
			{
				_computeRandomMoveLocation(pkMachine, fCurrentTime, fDeltaTime);
			}
		}
	}
}
//------------------------------------------------------
void SM_ServerNPCState_CHASE::_checkSkill(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime)
{
	LC_ServerNPC* pkNPC = (LC_ServerNPC*)pkMachine;
	LC_ActorSkillList* pkSkillList = pkNPC->GetActorSkillList();
	LC_ActorSkillTrigger* pkSkillTrigger = pkNPC->GetSkillTrigger();

	LC_MapBase* pkMap = pkNPC->GetMap();
	if (NULL == pkMap)
		return;

	//检查技能列表中的技能，直至找到一个有效的技能
	LC_ActorSkillEntry* pkSkillEntry = NULL;
	LC_SkillTriggerEntry* pkTriggerEntry = NULL;
	SK_SkillExecute* pkPendingSkill = pkNPC->GetPendingSkillExecute();
	while ((pkSkillEntry = pkSkillList->GetFirstSkillEntry()) != NULL && pkSkillEntry->GetValid())
	{
		bool bCheckSuccess = true;

		//如果该技能还未挂起，判断该技能cd时间是否满足
		if (NULL == pkPendingSkill || pkPendingSkill->GetExecuteID() != pkSkillEntry->GetSkillTypeID())
		{
			pkTriggerEntry = pkSkillTrigger->GetTriggerByIndex(pkSkillEntry->GetTriggerIndex());
			if (NULL != pkTriggerEntry
				&& pkTriggerEntry->GetValid()
				&& fCurrentTime - pkNPC->GetLastExecuteSkillTime() < pkTriggerEntry->GetCDTime())
			{
				bCheckSuccess = false;
			}
		}

		if (bCheckSuccess)
		{
			//检查技能是否超时
			if (pkSkillEntry->GetStartTime() > 0.0f
				&& fCurrentTime - pkSkillEntry->GetStartTime() > pkSkillEntry->GetLifeTime())
			{
				bCheckSuccess = false;

				//释放技能失败事件
				SK_SkillEventManagerBase* pkEventManager = ENGINE_GET_SKILL_EVENT_MANAGER();
				SK_SkillEvent* pkEvent = pkEventManager->AllocEvent();
				if (NULL != pkEvent)
				{
					pkEvent->SetEventType(SETE_EXECUTE_SKILL);
					pkEvent->SetSrcActorID(pkNPC->GetID());
					pkEvent->SetDstActorID(pkSkillEntry->GetLockedTargetID());
					pkEvent->SetLogicID(pkSkillEntry->GetSkillTypeID());
					pkEvent->SetParam(SK_ExecuteSkillEvent::SEP_RESULT, SK_ExecuteSkillEvent::SER_FAIL_TARGET_FAR);
					pkEventManager->AddEvent(pkEvent);
				}
			}
		}

		if (bCheckSuccess)
		{
			//检查目标是否隐身，这部分功能最终应该由目标过滤来处理
			LC_LogicObject* pkActor = pkMap->FindObject(pkSkillEntry->GetLockedTargetID());
			if (NULL != pkActor && (IS_PLAYER_CLASS_ID(pkActor->GetID()) || IS_NPC_ID(pkActor->GetID())))
			{
				if (((LC_ActorBase*)pkActor)->GetSkillFlag(SK_INVISIBLE))
				{
					bCheckSuccess = false;
				}
			}
		}

		if (bCheckSuccess)
		{
			//找到第一个有效技能，本次检查结束
			break;
		}
		else
		{
			//如果当前已经挂起该技能，撤销挂起
			if (NULL != pkPendingSkill && pkPendingSkill->GetExecuteID() == pkSkillEntry->GetSkillTypeID())
			{
				pkNPC->ClearPendingSkill();
			}
			pkSkillList->RemoveFirstSkillEntry();
		}
	}
}
//------------------------------------------------------
void SM_ServerNPCState_CHASE::_pendingSkill(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime)
{
	LC_ServerNPC* pkNPC = (LC_ServerNPC*)pkMachine;

	LC_ActorSkillList* pkSkillList = pkNPC->GetActorSkillList();
	LC_ActorSkillTrigger* pkSkillTrigger = pkNPC->GetSkillTrigger();

	LC_ActorSkillEntry* pkEntry = pkSkillList->GetFirstSkillEntry();
	if (pkEntry != NULL && pkEntry->GetValid())
	{
		//怪物被激活方式：1、被伤害；2、警戒范围内存在目标；3、技能列表中存在待释放技能
		pkNPC->SaveActiveInfo();

		int lSkillID = pkEntry->GetSkillTypeID();
		SK_SkillExecute* pkPendingSkill = pkNPC->GetPendingSkillExecute();
		//如果该技能已经挂起，则不反复挂起，避免重复设置挂起时间
		if (NULL != pkPendingSkill && pkPendingSkill->GetExecuteID() == pkEntry->GetSkillTypeID())
		{
			return;
		}

		bool bPendingSkillSuccess = true;

		ResultType iResult = pkNPC->CheckPendingSkill(lSkillID);
		if (RE_SKILL_NONE_ERROR != iResult)
		{
			bPendingSkillSuccess = false;
		}
		if (bPendingSkillSuccess)
		{
			pkNPC->SetLockedTargetID(pkEntry->GetLockedTargetID());
			ResultType iResult = pkNPC->PendingSkill(pkEntry->GetSkillTypeID(),0);
			if (RE_SKILL_PENDING_SUCCESS != iResult)
			{
				bPendingSkillSuccess = false;
			}
		}
		if (bPendingSkillSuccess)
		{
			//设置挂起技能时间
			pkEntry->SetStartTime(GET_PROCESS_TIME);	
		}
		else
		{
			//设置技能释放时间，防止技能因为释放不成功反复添加到技能列表中
			pkSkillTrigger->SetExecuteSkillTime(pkEntry->GetTriggerIndex(), fCurrentTime);
			pkSkillList->RemoveFirstSkillEntry();
		}
	}
}

void SM_ServerNPCState_CHASE::_computeRandomMoveLocation(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime)
{
	LC_ServerNPC* npc = (LC_ServerNPC*)pkMachine;
	LC_ServerNPC::ChaseType chaseType = npc->GetChaseMoveType();
	if (0 == chaseType)
		return;

	PF_ServerPathManager* pathmanager=PF_ServerPathManager::GetSingletonPtr();
	if (pathmanager == NULL)
		return;

	int	iChaseRadius = npc->GetProperty_ChaseRadius();
	int	innerRadius = npc->GetChaseInnerRadius();
	int outerRadius = npc->GetChaseOuterRadius();
	
	LC_ActorAttributeMap* pkAttributeMap = npc->GetAttributeMap();
	float speed = (float)pkAttributeMap->GetAttribute(ATT_MOVE_SPEED);
	if (speed <= 0)
		speed = 1.0f;

	UT_Vec3Int tgtLocation;
	const UT_Vec3Int &curLocation = npc->GetCurrentLocation();
	const UT_Vec3Int &activeLocation = npc->GetActiveLocation();

	float fSleepRate = npc->GetSleepProbability();
	float fSleepTimeMin = npc->GetSleepTimeMin();
	float fSleepTimeMax = npc->GetSleepTimeMax();

	switch(chaseType)
	{
	
	case 1:		//RANDOM
		if (outerRadius > 0)
		{
			if (UT_MathBase::RandFloat01() > fSleepRate)
			{
				float sleepTime = UT_MathBase::RandInRangeFloat(fSleepTimeMin, fSleepTimeMax);
				npc->SetChaseMoveTime(sleepTime);		
			}
			else if (UT_PathFindHelp::GenRandomPosInRestrictedRing(npc->GetMapResID(), curLocation, innerRadius, outerRadius, activeLocation, iChaseRadius, tgtLocation))
			{
				npc->SetTargetLocation(tgtLocation);
				int distance = UT_MathBase::LineLengthXYInt(tgtLocation, curLocation);
				float moveTime = distance/ speed;
				npc->SetChaseMoveTime(moveTime);
			}
		}
		break;
	case 2:		//SWING
		if (innerRadius > 0)
		{
			object_id_type lTargetID = npc->GetLockedTargetID();
			LC_MapBase* curMap = npc->GetMap();
			if (curMap == NULL)
				return;

			LC_LogicObject* target = curMap->FindObject(lTargetID);
			if (target == NULL)
				return;
			
			const UT_Vec3Int& enemyLocation = target->GetCurrentLocation();
			int distance = UT_MathBase::LineLengthXYInt(enemyLocation, curLocation);

			UT_Vec3Int directionV3 = curLocation - enemyLocation;
			UT_Vec2Float directionV2 = UT_MathBase::ConvertVec2IntToFloat( UT_Vec2Int(directionV3.x, directionV3.y));
			directionV2.Unitize();

			bool bClockwise = false;

			//先判断怪物和玩家的距离，如果不满足半径，就先移动到半径附近，之所以不用等于是因为 方向*距离 存在精度问题，不一定能准确的移动到半径上
			//如果被顶到墙上了，或者满足半径判断 就开始钟摆运动 
			//策划本来是打算环绕运动，所以起名叫clockwise,结果后来改成了钟摆，就懒得换名字了，改起来太麻烦
			if (abs(distance-innerRadius) > 10)
			{
				tgtLocation = enemyLocation + UT_MathBase::ConvertVec3FloatToInt(UT_MathBase::ConvertVec2To3Float( UT_Vec2Float( directionV2.x * innerRadius, directionV2.y * innerRadius)));
				if (!pathmanager->IsPosValid(npc->GetMapResID(), tgtLocation)) //防止怪被顶到墙上不能动
					bClockwise = true;
			}
			else if (UT_MathBase::RandFloat01() > fSleepRate)
			{
				float sleepTime = UT_MathBase::RandInRangeFloat(fSleepTimeMin, fSleepTimeMax);
				npc->SetChaseMoveTime(sleepTime);		
			}
			else
			{
				bClockwise = true;
			}

			if (bClockwise)
			{
				bool isSmallAngle = false;
				bool isReverse	= false;
				float zRotAngle	= 0;
				float minAngle	= GetGlobalSetting.fClockWiseMinAngle;
				float maxAngle	= npc->GetSwingAngle();
				float maxMoveAngle = npc->GetSwingMaxAngle();
				float curMoveAngle = npc->GetSwingCurrentAngle();
				UT_Vec2Float direction;

				//这里的逻辑是 先选大角，如果不行就选小角，如果再不行，就翻转先大角，再小角 
				//所以逻辑为(商 余数):	0/2 = 0 0,		1/2 = 0 1,		2/2 = 1 0,		3/2 = 1 1
				//大小角同向:				不翻转大角，	不翻转小角，	翻转大角，	翻转小角
				//如果 4次都没有选出合理结果 就不动
				//策划又TM加入了新的最大转动角度限制，具体逻辑自己看代码理解吧
				for (int i=0; i<4; ++i)
				{
					isReverse = i/2;
					isSmallAngle = i%2;

					if (isReverse)
					{
						maxAngle = -npc->GetSwingAngle();
					}

					if (isSmallAngle)
					{
						zRotAngle = maxAngle>0? minAngle: -minAngle;
					}
					else
					{
						zRotAngle = maxAngle;
					}

					maxMoveAngle = zRotAngle>0? npc->GetSwingMaxAngle(): -npc->GetSwingMaxAngle();
					if (maxMoveAngle > 0)
					{
						zRotAngle = curMoveAngle + zRotAngle > maxMoveAngle? maxMoveAngle-curMoveAngle: zRotAngle;
						if (zRotAngle < 1.0f)	//防止到达最大角度后 就不再转动
							continue;
					}
					else
					{
						zRotAngle = curMoveAngle + zRotAngle < maxMoveAngle? maxMoveAngle - curMoveAngle: zRotAngle;
						if (zRotAngle > -1.0f)
							continue;
					}

					direction = UT_MathBase::ZRot(directionV2, zRotAngle);
					tgtLocation = enemyLocation + UT_MathBase::ConvertVec3FloatToInt(UT_MathBase::ConvertVec2To3Float( UT_Vec2Float( direction.x * innerRadius, direction.y * innerRadius)));
					if (pathmanager->IsPosValid(npc->GetMapResID(), tgtLocation))
						break;
				}

				if (pathmanager->IsPosValid(npc->GetMapResID(), tgtLocation))
				{
					npc->SetSwingAngle(maxAngle);
					npc->SetSwingCurrentAngle(curMoveAngle + zRotAngle);
				}
				else
				{
					tgtLocation = curLocation;
				}
			}
			
			npc->SetTargetLocation(tgtLocation);
			distance = UT_MathBase::LineLengthXYInt(tgtLocation, curLocation);
			float moveTime = distance/ speed;
			npc->SetChaseMoveTime(moveTime);
		}
		break;
	case 0:	//DO_NOTHING
	default:	//treat as DO_NOTHING
		break;
	}
}