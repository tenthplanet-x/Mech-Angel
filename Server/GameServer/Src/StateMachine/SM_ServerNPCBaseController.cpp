#include "SM_ServerNPCBaseController.h"
#include "GameLogicScriptPCH.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerNPC.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "LC_FactionManager.h"
#include "LC_GameEventManager.h"
#include "SK_ServerSkillEventManager.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerMapLogic_Place.h"
#include "UT_DirConfig.h"
#include "LS_LuaStateFactory.h"
#include "LC_NPCGroupBase.h"
using namespace StateMachine;
using namespace Skill;
using namespace Utility;
using namespace GameLogic;
using namespace CSVFile;
//----------------------------------------------------------
SM_ServerNPCBaseController::SM_ServerNPCBaseController()
: m_pkScriptUpdateMachineStateFunctor(NULL)
, m_pkScriptUpdateMovePathFunctor(NULL)
, m_pkScriptUpdateFightFunctor(NULL)
, m_pkScriptUpdateTransformFunctor(NULL)
{
	m_kHandleSkillEventMap.clear();
}
//----------------------------------------------------------
SM_ServerNPCBaseController::~SM_ServerNPCBaseController()
{
	T_SAFE_DELETE(m_pkScriptUpdateMovePathFunctor);
	T_SAFE_DELETE(m_pkScriptUpdateMachineStateFunctor);
	T_SAFE_DELETE(m_pkScriptUpdateFightFunctor);
	T_SAFE_DELETE(m_pkScriptUpdateTransformFunctor);

	//清空事件回调列表
	SM_HandleEventMapIter iter;
	TRAV(m_kHandleSkillEventMap, iter)
	{
		SM_StateHandleEventData* pkData = iter->second;
		T_SAFE_DELETE(pkData);
	}
	m_kHandleSkillEventMap.clear();

	TRAV(m_kHandleGameEventMap, iter)
	{
		SM_StateHandleEventData* pkData = iter->second;
		T_SAFE_DELETE(pkData);
	}
	m_kHandleGameEventMap.clear();
}
//------------------------------------------------------------------------
bool SM_ServerNPCBaseController::InitScriptData( const StringType& rkScriptName )
{
	T_ASSERT_CRITICAL_S(rkScriptName.find("xx",0) == std::string::npos);

	LS_LuaState* pkLuaState = LS_LuaStateFactory::GetSingletonPtr()->GetLogicLuaState();
	StringType kTaskScriptPath = GfxGetDirConfig().GetDir(CF_NODE_AI_SCRIPT);
	StringType kScriptFileFullName = kTaskScriptPath + rkScriptName;

	bool bLoadScript = pkLuaState->DoFile(kScriptFileFullName);
	if(!bLoadScript)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"Script file[%s]error",kScriptFileFullName.c_str());
		T_ASSERT_CRITICAL_S(false);
		return false;
	}

	FilePathNamePair kPathNamePair = UT_BaseStringUtility::SplitFileName(rkScriptName);
	StringType kBaseFileName = kPathNamePair.second;
	FilePrePostFixPair kPrePostFixPair = UT_BaseStringUtility::SplitBaseFileName(kBaseFileName);

	TStringVector kFileNameTable = UT_BaseStringUtility::SplitString(kPrePostFixPair.first,'-');
	StringType kTableName = kFileNameTable[0] + "_AI_Controller";

	//检查表是否存在
	T_CHECK_LUAOBJ_EXIST(pkLuaState,kTableName);

	LS_LuaObject* pkLuaObject = NULL;

	bool bReload = false;
	StringType strFuncType = kTableName + ".Update_MachineState_TYPE";
	StringType strFunctor = kTableName + ".Update_MachineState";
	pkLuaObject = pkLuaState->GetLuaObject(strFuncType,bReload);
	if(pkLuaObject)
	{
		m_eUpdateMachineStateType = _getUpdateFuncType(pkLuaObject->ToInt());
		if (m_eUpdateMachineStateType != SUFT_DEFAULT)
		{
			m_pkScriptUpdateMachineStateFunctor = T_NEW_D LC_ServerLuaVoidFunction(pkLuaState,strFunctor.c_str());
			T_ASSERT_CRITICAL_S(m_pkScriptUpdateMachineStateFunctor);
		}
	}

	strFuncType = kTableName + ".Update_MovePath_TYPE";
	strFunctor = kTableName + ".Update_MovePath";
	pkLuaObject = pkLuaState->GetLuaObject(strFuncType,bReload);
	if(pkLuaObject)
	{
		m_eUpdateMovePathType = _getUpdateFuncType(pkLuaObject->ToInt());
		if (m_eUpdateMovePathType != SUFT_DEFAULT)
		{
			m_pkScriptUpdateMovePathFunctor = T_NEW_D LC_ServerLuaVoidFunction(pkLuaState,strFunctor.c_str());
			T_ASSERT_CRITICAL_S(m_pkScriptUpdateMovePathFunctor);
		}
	}

	strFuncType = kTableName + ".Update_Fight_TYPE";
	strFunctor = kTableName + ".Update_Fight";
	pkLuaObject = pkLuaState->GetLuaObject(strFuncType,bReload);
	if(pkLuaObject)
	{
		m_eUpdateFightType = _getUpdateFuncType(pkLuaObject->ToInt());
		if (m_eUpdateFightType != SUFT_DEFAULT)
		{
			m_pkScriptUpdateFightFunctor = T_NEW_D LC_ServerLuaVoidFunction(pkLuaState,strFunctor.c_str());
			T_ASSERT_CRITICAL_S(m_pkScriptUpdateFightFunctor);
		}
	}

	strFuncType = kTableName + ".Update_Transform_TYPE";
	strFunctor = kTableName + ".Update_Transform";
	pkLuaObject = pkLuaState->GetLuaObject(strFuncType,bReload);
	if(pkLuaObject)
	{
		m_eUpdateTransformType = _getUpdateFuncType(pkLuaObject->ToInt());
		if (m_eUpdateTransformType != SUFT_DEFAULT)
		{
			m_pkScriptUpdateTransformFunctor = T_NEW_D LC_ServerLuaVoidFunction(pkLuaState,strFunctor.c_str());
			T_ASSERT_CRITICAL_S(m_pkScriptUpdateTransformFunctor);
		}
	}

	//事件处理回调
	char		cBuffer[255];
	int			iEventCount = 0;
	StringType  kEventCountName = kTableName+ ".EventCount";
	pkLuaObject = pkLuaState->GetLuaObject(kEventCountName,bReload);
	if(pkLuaObject)
		iEventCount = pkLuaObject->ToInt();

	StringType kEventName;
	StringType kEventIDName;
	StringType kEventTypeName;
	StringType kEventFunctorName;
	StringType kEventHandleTypeName;
	StringType kFunctorName;
	for(int iEventIndex = 0; iEventIndex < iEventCount; iEventIndex++)
	{
		sprintf(cBuffer,"Event_%i",iEventIndex);
		kEventName = cBuffer;
		kEventName = kTableName+ "." + kEventName;

		//检查事件是否存在
		T_CHECK_LUAOBJ_EXIST(pkLuaState,kEventName);

		kEventIDName		  = kEventName + ".Event";
		kEventTypeName		  = kEventName + ".EventType";
		kEventFunctorName     = kEventName + ".Functor";
		kEventHandleTypeName  = kEventName + ".HandleType";

		pkLuaObject = pkLuaState->GetLuaObject(kEventIDName,bReload);
		if(pkLuaObject == NULL)
			continue;

		int iEvent = pkLuaObject->ToInt();

		pkLuaObject = pkLuaState->GetLuaObject(kEventTypeName,bReload);
		if(pkLuaObject == NULL)
			continue;

		int iEventType = pkLuaObject->ToInt();

		pkLuaObject = pkLuaState->GetLuaObject(kEventHandleTypeName,bReload);
		if(pkLuaObject == NULL)
			continue;

		StateHandleEventType eHandleType = (StateHandleEventType)pkLuaObject->ToInt();

		Utility::LC_ServerLuaFunction<int>* pkFunctor = NULL;
		SM_StateHandleEventData* pkData = T_NEW_D SM_StateHandleEventData;
		{
			if (eHandleType == SHET_REPLACE||eHandleType == SHET_SUPPLEMENT)
			{
				pkLuaObject = pkLuaState->GetLuaObject(kEventFunctorName,bReload);
				if(pkLuaObject)
				{
					kFunctorName = kTableName + "." + pkLuaObject->ToString();
					pkLuaObject = pkLuaState->GetLuaObject(kFunctorName,bReload);
					if(pkLuaObject)
						pkFunctor = T_NEW_D Utility::LC_ServerLuaFunction<int>(pkLuaState,kFunctorName.c_str());
				}
			}
			pkData->m_pkFunctor = pkFunctor;
			pkData->m_iHandleType = eHandleType;
			if (iEventType == SET_SKILL)
			{
				m_kHandleSkillEventMap[iEvent] = pkData;
			}
			else if (iEventType == SET_GAME)
			{
				m_kHandleGameEventMap[iEvent] = pkData;
			}
			else
			{
				T_ASSERT_CRITICAL_S(false);
				return false;
			}
		}
	}

	return true;
}
//----------------------------------------------------------
void SM_ServerNPCBaseController::Update( SM_StateMachine* pkStateMachine, float fCurrentTime, float fDeltaTime )
{
	LC_ActorBase* pkActor = static_cast<LC_ActorBase*>(pkStateMachine);

	//状态更新
	if (m_eUpdateMovePathType != SUFT_REPLACE)
		_updateMachineState(pkStateMachine, fCurrentTime, fDeltaTime);
	if (m_pkScriptUpdateMovePathFunctor&&m_eUpdateMovePathType != SUFT_DEFAULT)
		(*m_pkScriptUpdateMovePathFunctor)(pkActor, fCurrentTime, fDeltaTime);
		
	//路径更新
// 	if (m_eUpdateMachineStateType != SUFT_REPLACE)
// 		_updateMovePath(pkStateMachine, fCurrentTime, fDeltaTime);
// 	if (m_pkScriptUpdateMachineStateFunctor&&m_eUpdateMachineStateType != SUFT_DEFAULT)
// 		(*m_pkScriptUpdateMachineStateFunctor)(pkActor, fCurrentTime, fDeltaTime);

	//移动更新
	if (m_eUpdateTransformType != SUFT_REPLACE)
		_updateTransform(pkStateMachine, fCurrentTime, fDeltaTime);
	if (m_pkScriptUpdateTransformFunctor&&m_eUpdateTransformType != SUFT_DEFAULT)
		(*m_pkScriptUpdateTransformFunctor)(pkActor, fCurrentTime, fDeltaTime);

	//战斗更新
	if (m_eUpdateFightType != SUFT_REPLACE)
		_updateFight(pkStateMachine, fCurrentTime, fDeltaTime);
	if (m_pkScriptUpdateFightFunctor&&m_eUpdateFightType != SUFT_DEFAULT)
		(*m_pkScriptUpdateFightFunctor)(pkActor, fCurrentTime, fDeltaTime);
}

void SM_ServerNPCBaseController::NotifySkillEvent( SM_StateMachine* pkStateMachine, Skill::SK_SkillEvent* pkSkillEvent )
{
	if (pkStateMachine == NULL||pkSkillEvent == NULL)
		return;

	StateHandleEventType eStateHandleType = SHET_DEFAULT;
	SM_StateBase* pkState = pkStateMachine->GetCurrentState();
	if (pkState)
		eStateHandleType = pkState->GetEventHandleType(pkSkillEvent);

	StateHandleEventType eControllerHandleType = _getEventHandleType(pkSkillEvent);

	switch (eStateHandleType)
	{
	case SHET_REPLACE:
		{
			//只调用状态元脚本处理事件
			_stateScriptHandleSkillEvent(pkStateMachine, pkSkillEvent);
		}
		break;
	case SHET_SUPPLEMENT:
		{
			//只调用控制器的默认方式处理事件，即控制器脚本与状态元脚本不能同时处理事件
			_controllerHandleSkillEvent(pkStateMachine, pkSkillEvent);
			_stateScriptHandleSkillEvent(pkStateMachine, pkSkillEvent);
		}
		break;
	case SHET_DEFAULT:
		{
			switch (eControllerHandleType)
			{
			case SHET_REPLACE:
				{
					//只调用控制器脚本处理事件
					_controllerScriptHandleSkillEvent(pkStateMachine, pkSkillEvent);
				}
				break;
			case SHET_SUPPLEMENT:
				{
					_controllerHandleSkillEvent(pkStateMachine, pkSkillEvent);
					_controllerScriptHandleSkillEvent(pkStateMachine, pkSkillEvent);
				}
				break;
			case SHET_DEFAULT:
				{
					_controllerHandleSkillEvent(pkStateMachine, pkSkillEvent);
				}
				break;
			case SHET_IGNORE:
			default:
				break;
			}
		}
		break;
	case SHET_IGNORE:
	default:
		break;
	}
}

void SM_ServerNPCBaseController::NotifyGameEvent( SM_StateMachine* pkStateMachine, GameLogic::LC_GameEvent* pkGameEvent )
{
	if (pkStateMachine == NULL||pkGameEvent == NULL)
		return;

	StateHandleEventType eType = SHET_DEFAULT;
	SM_StateBase* pkState = pkStateMachine->GetCurrentState();
	if (pkState)
		eType = pkState->GetEventHandleType(pkGameEvent);

	if (eType != SHET_REPLACE)
	{
		//默认处理此处添加。。。。。。。。。
	}


	if (eType == SHET_REPLACE || eType == SHET_SUPPLEMENT)
	{
		if (pkState)
			pkState->NotifyGameEvent(pkStateMachine,pkGameEvent);
	}
}
//----------------------------------------------------------
void SM_ServerNPCBaseController::_updateMovePath(SM_StateMachine* pkStateMachine, float fCurrentTime, float fDeltaTime )
{
	
}
//----------------------------------------------------------
void SM_ServerNPCBaseController::_updateTransform(SM_StateMachine* pkStateMachine, float fCurrentTime, float fDeltaTime )
{
	LC_ServerNPC* pkNPC = (LC_ServerNPC*)(pkStateMachine);

	float fLastTranslateTime = pkNPC->GetLastTranslateTime();
	float fDeltaTimeTemp = fCurrentTime - fLastTranslateTime;
	pkNPC->SetLastTranslateTime(fCurrentTime);

	_translateAndRotateServerNPC(pkStateMachine, fCurrentTime,fDeltaTimeTemp);
}
//----------------------------------------------------------
void SM_ServerNPCBaseController::_updateMachineState( SM_StateMachine* pkStateMachine, float fCurrentTime, float fDeltaTime )
{
	LC_ServerNPC* pkNPC = static_cast<LC_ServerNPC*>(pkStateMachine);

	if (pkNPC->GetSkillFlag(SK_DIZZ) && pkNPC->GetCurrentStateType() != ST_NPC_DIZZ)
		pkNPC->FixCurrentState(ST_NPC_DIZZ,fCurrentTime,fDeltaTime);

	if (pkNPC->GetSkillFlag(SK_SLEEP) && pkNPC->GetCurrentStateType() != ST_NPC_SLEEP)
		pkNPC->FixCurrentState(ST_NPC_SLEEP,fCurrentTime,fDeltaTime);

	//眩晕、睡眠、沉默、近程沉默、远程沉默情况下打断当前技能
	SK_ServerSkillEventManager* pkEventManager = SERVER_GET_SKILL_EVENT_MANAGER();
	if (IS_SKILL_TYPE(pkNPC->GetCurrentStateType()))
	{
		SK_SkillExecute* pkSkillExecute = (SK_SkillExecute*)pkNPC->GetCurrentState();
		if ( (pkNPC->GetSkillFlag(SK_SILENCE) && pkSkillExecute->GetProperty_DamageType() != 0)
			|| pkNPC->GetSkillFlag(SK_FREEZE_ANIMATION)
			/*|| (pkNPC->GetSkillFlag(SK_SILENCE_MELEE) && SRT_MELEE == pkSkillExecute->GetProperty_RangeType())
			|| (pkNPC->GetSkillFlag(SK_SILENCE_RANGE) && SRT_RANGE == pkSkillExecute->GetProperty_RangeType())*/ )
		{
			SK_SkillEvent* pkEvent = pkEventManager->AllocEvent();
			if (NULL != pkEvent)
			{
				pkEvent->SetEventType(SETE_INTERRUPT);
				pkEvent->SetSrcActorID(0);
				pkEvent->SetDstActorID(pkNPC->GetID());
				pkEvent->SetLogicID(0);
				pkEvent->SetParam(SK_InterruptEvent::SEP_BE_INTERRUPT_SKILL_ID, pkNPC->GetCurrentStateType());
				pkEvent->SetParam(SK_InterruptEvent::SEP_INTERRUPT_TYPE, SKI_EVENT);
				pkEventManager->AddEvent(pkEvent);
			}
		}
	}

	if(pkNPC->GetDead() && pkNPC->GetCurrentStateType() != ST_NPC_DEAD)
	{
		pkNPC->FixCurrentState(ST_NPC_DEAD,fCurrentTime,fDeltaTime);
	}

	//在垂死状态时释放垂死技能，然后才进入死亡状态
	int lDyingSkillID = pkNPC->GetDyingSkillID();
	int iCurrentStateType = pkNPC->GetCurrentStateType();
	if (pkNPC->GetDying() && IS_SKILL_TYPE(lDyingSkillID) && iCurrentStateType != lDyingSkillID)
	{
		bool bExecuteDyingSkillSuccess = true;

		ResultType iResult = RE_SKILL_NONE_ERROR;
		if(IS_SKILL_TYPE(iCurrentStateType))
		{
			iResult = pkNPC->InterruptSkill(fCurrentTime, iCurrentStateType,  SKI_DAMAGE);
			if (RE_SKILL_NONE_ERROR != iResult)
			{
				bExecuteDyingSkillSuccess = false;
			}
		}

		if (bExecuteDyingSkillSuccess)
		{
			iResult = pkNPC->PendingSkill(lDyingSkillID, 0);
			if (RE_SKILL_PENDING_SUCCESS != iResult)
			{
				bExecuteDyingSkillSuccess = false;
			}
		}

		if (bExecuteDyingSkillSuccess)
		{
			iResult = pkNPC->CheckExecuteSkill(fCurrentTime, pkNPC->GetPendingSkillExecute(), 0);
			if (RE_SKILL_CAN_TAKE_PLACE != iResult)
			{
				bExecuteDyingSkillSuccess = false;
			}
		}
		
		if (bExecuteDyingSkillSuccess)
		{
			iResult = pkNPC->ExecuteSkill(fCurrentTime, pkNPC->GetPendingSkillExecute(), 0);
			if (RE_SKILL_EXECUTE_SUCCESS != iResult)
			{
				bExecuteDyingSkillSuccess = false;
			}
		}

		if (!bExecuteDyingSkillSuccess)
		{
			pkNPC->SetDead(true);
			pkNPC->FixCurrentState(ST_NPC_DEAD,fCurrentTime,fDeltaTime);
		}

		//清空AI技能
		pkNPC->GetActorSkillList()->Reset();
	}
	else if (pkNPC->GetDying() && !IS_SKILL_TYPE(lDyingSkillID))
	{
		pkNPC->SetDying(false);
		pkNPC->SetDead(true);
		pkNPC->FixCurrentState(ST_NPC_DEAD,fCurrentTime,fDeltaTime);
	}
}
//----------------------------------------------------------
void SM_ServerNPCBaseController::_translateAndRotateServerNPC( SM_StateMachine* pkStateMachine, float fCurrentTime,float fDeltaTime )
{
	LC_ServerNPC* pkNPC = static_cast<LC_ServerNPC*>(pkStateMachine);

	//服务器不做高度模拟，抛物线不算的
	int	iMoveSpeed	= pkNPC->GetMoveSpeed();
	int mv0 = iMoveSpeed;
	float fAcc = pkNPC->GetMoveAccSpeed();
	float mv  = iMoveSpeed + fAcc * fDeltaTime;
	if (mv < 0)
		mv = 0;
	pkNPC->SetMoveSpeed((int)mv);

	if(iMoveSpeed <= 0)
		return;
	int curstate=pkNPC->GetCurrentStateType();
	int nMoveDistance = UT_MathBase::LineLengthXYInt( pkNPC->GetTargetLocation() , pkNPC->GetCurrentLocation());
	//if(UT_MathBase::LineLengthXYInt( pkNPC->GetTargetLocation() , pkNPC->GetCurrentLocation()/*m_kMovePath.GetPathPoint(iPathPointCount - 1)*/) <= DISTANCE_ADJUST)
	if(nMoveDistance  <= DISTANCE_ADJUST)
	{
		//这个时候说明怪物是不需要移动的，但是MG_SyncNPCsMoveInFOV发送的m_iTargetLocationX，是当前怪物走的最近点，如果这只怪物在一开始从来没走过，就原地在打，
		//那么SetMoveLineTargetLocation()从来就不会被设置，会下发错误的目标点给客户端，怪物释放技能的时候又会被拉回来，造成怪物滑步
		//所以在怪物不需要移动的时候，设置一次正确的值
		pkNPC->SetMoveLineTargetLocation(pkNPC->GetTargetLocation());
		return;
	}
	float fMoveDistance = (mv0 + mv) * 0.5f * fDeltaTime;

  /*
	//应策划要求，如果NPC没有可用的技能时就停留在原地
	bool holdon = false;
	if( ST_NPC_CHASE == curstate)
	{
		if(pkNPC->GetPendingSkillExecute() == NULL)
		{
			holdon = true;
		}
	}

	if(holdon)
	{
		return;
	}
  */
	UT_Vec3Int kTargetLocation;
	UT_Vec3Int kCurrentLocation;

	UT_Vec2Float kMoveDirXY;
	UT_LinePath& rkMovePath = pkNPC->GetMovePath();
	bool do_find = false;

	if(rkMovePath.GetPathPointCount()<2 || pkNPC->GetTargetLocation()!=rkMovePath.GetTargetPoint() || rkMovePath.GetMoveStep()>=1.0f ||!rkMovePath.CheckInPath(pkNPC->GetCurrentLocation()))
	{
		do_find = true;
		//搜索路径
		rkMovePath.Clear();
		rkMovePath.SetStartPoint(pkNPC->GetCurrentLocation());
		rkMovePath.SetTargetPoint(pkNPC->GetTargetLocation());
		pkNPC->FindPathTo(pkNPC->GetTargetLocation());
	
	}
	int iIndex = -1;
	if( rkMovePath.MoveAlongPath(fMoveDistance,kCurrentLocation,kTargetLocation,kMoveDirXY,iIndex) )
	{
		int iLastPathPoint = pkNPC->GetLastPathPointIndex();
		if (iLastPathPoint != iIndex && iIndex >= 0)
			pkNPC->SetLastPathPointIndex(iIndex);
		if (curstate == ST_NPC_PUNCH_BACK)
			pkNPC->SetForwardDir( -kMoveDirXY );
		else
			pkNPC->SetForwardDir( kMoveDirXY );
		pkNPC->SetCurrentLocation( kCurrentLocation );
		pkNPC->SetMoveLineTargetLocation( kTargetLocation );
	}
	else
	{
		//沿着路点走不成功，直接走
		rkMovePath.Clear();
		kCurrentLocation = pkNPC->GetCurrentLocation();
		pkNPC->SetCurrentLocation(kCurrentLocation);
		pkNPC->SetMoveLineTargetLocation(kCurrentLocation);
	}
}
//----------------------------------------------------------
void SM_ServerNPCBaseController::_updateFight( StateMachine::SM_StateMachine* pkStateMachine, float fCurrentTime, float fDeltaTime )
{
 	LC_ServerNPC* pkNPC = static_cast<LC_ServerNPC*>(pkStateMachine);	
 	if(pkNPC->GetDead() || pkNPC->GetDying() || pkNPC->GetSkillFlag(SK_FREEZE_ANIMATION)) return;

	//技能列表
	LC_ActorSkillList* pkSkillList = pkNPC->GetActorSkillList();

	//技能触发器
	LC_ActorSkillTrigger* pkSkillTrigger = pkNPC->GetSkillTrigger();

	//更新仇恨列表，选取目标
	object_id_type lSelectTarget = _selectTarget(pkStateMachine, fCurrentTime, fDeltaTime);
	if (!IS_PLAYER_CLASS_ID(lSelectTarget) && !IS_NPC_ID(lSelectTarget))
	{
		return;
	}

	//向技能列表中添加技能
	LC_SkillTriggerIndexList kSkillIDList = pkSkillTrigger->GetPendingSkillList(pkNPC, fCurrentTime);
	LC_SkillTriggerIndexListIter iter;
	TRAV(kSkillIDList, iter)
	{
		int iTriggerIndex = *iter;
		LC_SkillTriggerEntry* pkTriggerEntry = pkSkillTrigger->GetTriggerByIndex(iTriggerIndex);
		if (NULL != pkTriggerEntry)
		{
			LC_ActorSkillEntry kSkillEntry;
			kSkillEntry.SetLockedTargetID(lSelectTarget);
			kSkillEntry.SetSkillTypeID(pkTriggerEntry->GetSkillTypeID());
			kSkillEntry.SetTriggerIndex(iTriggerIndex);
			kSkillEntry.SetStartTime(-1.0);	//设置技能没有启动，挂起技能时设置挂起时间
			kSkillEntry.SetLifeTime(pkTriggerEntry->GetLifeTime());
			kSkillEntry.SetPriority(pkTriggerEntry->GetPriority());
			pkSkillList->AddSkillEntry(kSkillEntry);
		}
	}
}
//----------------------------------------------------------
object_id_type SM_ServerNPCBaseController::_selectTarget(StateMachine::SM_StateMachine* pkStateMachine, float fCurrentTime, float fDeltaTime)
{
	LC_ServerNPC* pkNPC = static_cast<LC_ServerNPC*>(pkStateMachine);	
	if(pkNPC->GetDead())
	{
		return 0;
	}

	object_id_type lSelfID = pkNPC->GetID();

	//仇恨列表
	LC_ActorHatredList* pkHatredList = pkNPC->GetHatredList();
	LC_ActorSkillList* pkSkillList = pkNPC->GetActorSkillList();

	LC_WorldManagerBase* pkWorldManager = ENGINE_GET_WORLD_MANAGER();
	if (NULL == pkWorldManager)
	{
		return 0;
	}


	/*
	*更新仇恨列表
	*/
	LC_ActorHatredEntry* pkHatredEntry = NULL;
	int iHatredCount = pkHatredList->GetHatredCount();
	bool bAllImmortal = true;
	//仇恨列表的访问索引
	for (int i = 0; i < iHatredCount; i ++)
	{
		pkHatredEntry = pkHatredList->GetSortHatredByIndex(i);

		if (NULL != pkHatredEntry && pkHatredEntry->GetValid())
		{
			object_id_type lTargetID = pkHatredEntry->GetTargetID();
			if (IS_PLAYER_CLASS_ID(lTargetID) || IS_NPC_ID(lTargetID))
			{
				LC_ActorBase* pkTarget = (LC_ActorBase*)pkWorldManager->FindObject(lTargetID);
				//目标无效
				if (NULL == pkTarget 
					|| lSelfID == lTargetID
					|| pkTarget->GetDead()
					//|| LC_Helper::IsInSafeArea(pkTarget)
					|| pkTarget->GetSkillFlag(SK_INVISIBLE)
				)
				{
					pkHatredList->RemoveHatred(lTargetID);
					//清空技能列表中针对该目标的技能
					pkSkillList->RemoveSkillByTargetID(lTargetID);

					pkNPC->ClearPendingSkill();

					continue;
				}
				if( pkNPC->GetMapLogicID() != pkTarget->GetMapLogicID() )
				{
				  pkHatredList->RemoveHatred(lTargetID);
				  //清空技能列表中针对该目标的技能
				  pkSkillList->RemoveSkillByTargetID(lTargetID);
				  continue;
				}
				if(LC_Helper::IsValidTarget((LC_ActorBase*)pkNPC,pkTarget)!=RE_SKILL_CHECK_VALID_TARGET)
				{
					if(pkHatredList->GetHatredCount() > 1)
					{
						pkHatredList->RemoveHatred(lTargetID);
						//清空技能列表中针对该目标的技能
						pkSkillList->RemoveSkillByTargetID(lTargetID);
					}
					continue;
				}

				//目标无敌
				if (pkTarget->GetSkillFlag(SK_IMMORTAL))
				{
					continue;
				}
				else
				{
					bAllImmortal = false;
				}

				object_id_type lOldTargetID = pkNPC->GetDefaultTargetID();
				if (lOldTargetID != lTargetID)
				{
					//新目标仇恨没有超过现在目标仇恨的1.2倍，不能替换现在的目标
					LC_ActorHatredEntry* pkOldHatredEntry = pkHatredList->GetHatred(lOldTargetID);
					if (NULL != pkOldHatredEntry 
						&& pkHatredEntry->GetHatredValue() < pkOldHatredEntry->GetHatredValue() * (1 + 0.2))
					{
						continue;
					}

					//通过OT方式找到新的目标，必须清空技能队列，否则会继续打击原先的目标
					pkSkillList->Reset();
					pkNPC->ClearPendingSkill();
				}

				//找到合法目标
				break;
			}
		}
	}
	//如果所有目标均无敌，则选择第一个目标
	if (NULL == pkHatredEntry && bAllImmortal)
	{
		pkHatredEntry = pkHatredList->GetSortHatredByIndex(0);
	}


	/*
	*如果给npc指定了特殊目标，则仇恨队列不起作用
	*/
	object_id_type lAdditionTargetID = pkNPC->GetAdditionTargetID();
	if (IS_PLAYER_CLASS_ID(lAdditionTargetID) || IS_NPC_ID(lAdditionTargetID))
	{
		if (pkNPC->GetAdditionTargetDurationTime()==0.0f || (fCurrentTime - pkNPC->GetAdditionTargetBeginTime() < pkNPC->GetAdditionTargetDurationTime()))
		{
			LC_ActorBase* pkAdditionTarget = (LC_ActorBase*)pkWorldManager->FindObject(lAdditionTargetID);
			if (NULL == pkAdditionTarget 
				|| pkAdditionTarget->GetDead()
				|| pkAdditionTarget->GetSkillFlag(SK_INVISIBLE))
			{
				pkNPC->SetAdditionTargetID(0);
				pkNPC->SetAdditionTargetBeginTime(0.0f);
				pkNPC->SetAdditionTargetDurationTime(0.0f);
			}
			if (0 != pkNPC->GetAdditionTargetID())
			{
				return pkNPC->GetAdditionTargetID();
			}
		}
		else
		{
			pkNPC->SetAdditionTargetID(0);
			pkNPC->SetAdditionTargetBeginTime(0.0f);
			pkNPC->SetAdditionTargetDurationTime(0.0f);
		}
	}
	if (NULL != pkHatredEntry)
	{
		object_id_type lSelectTargetID = pkHatredEntry->GetTargetID();
		if (IS_PLAYER_CLASS_ID(lSelectTargetID) || IS_NPC_ID(lSelectTargetID))
		{
			pkNPC->SetDefaultTargetID(lSelectTargetID);
			return lSelectTargetID;
		}
	}

	return 0;
}
//----------------------------------------------------------
StateHandleEventType SM_ServerNPCBaseController::_getEventHandleType(Skill::SK_SkillEvent* pkSkillEvent)
{
	if (pkSkillEvent)
	{
		SM_HandleEventMapIter Iter = m_kHandleSkillEventMap.find(pkSkillEvent->GetEventType());
		if (Iter != m_kHandleSkillEventMap.end())
		{
			SM_StateHandleEventData* pkData = Iter->second;
			if (pkData)
				return (StateHandleEventType)pkData->m_iHandleType;
		}
	}
	return SHET_DEFAULT;
}
//----------------------------------------------------------
bool SM_ServerNPCBaseController::NotifyStateEnd( SM_StateMachine* pkStateMachine, int iEndState,float fCurrentTime,float fDeltaTime )
{
	LC_ServerNPC* pkNPC = (LC_ServerNPC*)pkStateMachine;
	if(IS_SKILL_TYPE(iEndState))
	{
		if (iEndState == pkNPC->GetDyingSkillID())
		{
			pkNPC->SetDying(false);
			pkNPC->SetDead(true);
			pkNPC->SetNextState(ST_NPC_DEAD);
		}
		else
		{
			pkStateMachine->SetNextState(ST_NPC_CHASE);
		}
	}

	return true;
}
//----------------------------------------------------------------------------------------------
void SM_ServerNPCBaseController::_notifySkillEvent_EnterGuard( SM_StateMachine* pkStateMachine, Skill::SK_SkillEvent* pkSkillEvent )
{
 	LC_ServerNPC* pkNPC = static_cast<LC_ServerNPC*>(pkStateMachine);
	LC_ActorHatredList* pkHatredList = pkNPC->GetHatredList();
	
	object_id_type lSrcActorID = pkSkillEvent->GetSrcActorID();

	attr_value_type lHatredValue = LC_Helper::ComputeHatredValue(1, lSrcActorID, 0);
	LC_ActorHatredEntry kHatredEntry;
	kHatredEntry.SetTargetID(lSrcActorID);
	kHatredEntry.SetHatredValue(lHatredValue);
	kHatredEntry.SetUpdateTime(GET_PROCESS_TIME);
	pkHatredList->AddHatred(kHatredEntry, true);

	pkNPC->SaveActiveInfo();

	//两种情况下主动激活广播仇恨：1、idle状态下开启了广播功能；2、wander状态下开启了广播仇恨
	if ( 
		((ST_NPC_IDLE == pkNPC->GetCurrentStateType() && SFT_DEFAULT == pkNPC->GetAIFuncValue(SF_IDLE_POS_ACTIVE_BROADCAST)) ||
		(ST_NPC_WANDER == pkNPC->GetCurrentStateType() && SFT_DEFAULT == pkNPC->GetAIFuncValue(SF_WANDER_POS_ACTIVE_BROADCAST))
		) 
		||(pkNPC->GetEliteLevel() > 0 && pkNPC->GetProperty_BroadcastRadius() > 0 )
		)
	{
		_broadcastHatred(pkStateMachine, lSrcActorID);
	}
}
//----------------------------------------------------------------------------------------------
void SM_ServerNPCBaseController::_notifySkillEvent_Damage(StateMachine::SM_StateMachine* pkStateMachine, Skill::SK_SkillEvent* pkSkillEvent)
{
	LC_ServerNPC* pkNPC = static_cast<LC_ServerNPC*>(pkStateMachine);
	LC_ActorHatredList* pkHatredList = pkNPC->GetHatredList();

	object_id_type lSrcActorID	= pkSkillEvent->GetSrcActorID();
	object_id_type lDestActorID	= pkSkillEvent->GetDstActorID();
	attr_value_type lFinalDamage	= pkSkillEvent->GetParam(SK_DamageEvent::SEP_FINAL_DAMAGE);
	int lSkillTypeID	= pkSkillEvent->GetLogicID();

	if (pkNPC->GetID() == lSrcActorID)
	{
		return;
	}

	//副本中对怪物受到攻击会有特殊逻辑处理，加在这里sky
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	{
		int lMapLogicID = pkNPC->GetMapLogicID();
		LC_ServerMapLogic_Place* pkMapLogic	=	pkWorldManager->GetMapLogic(lMapLogicID);
		if(pkMapLogic)
		{
			LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
			LC_GameEvent* pkGameEvent			= pkEventManager->AllocGameEvent();
			if (pkGameEvent == NULL)
				return;

			pkGameEvent->SetEventType(GLET_NPC_BE_ATTACK);
			pkGameEvent->SetEventParam(EVENT_BASE_PARAM_1,lMapLogicID);
			pkGameEvent->SetEventParam(EVENT_BASE_PARAM_2,lSrcActorID);
			pkGameEvent->SetEventParam(EVENT_BASE_PARAM_3,lDestActorID);
			pkGameEvent->SetEventParam(EVENT_BASE_PARAM_4,lFinalDamage);
			pkMapLogic->ProcessGameEvent(pkGameEvent);
			pkEventManager->FreeGameEvent(pkGameEvent);
		}
	}
	//副本处理逻辑添加完毕

	attr_value_type lHatredValue	= LC_Helper::ComputeHatredValue(UT_MathBase::MaxInt64(1,lFinalDamage), lSrcActorID, lSkillTypeID);
	LC_ActorHatredEntry kHatredEntry;
	kHatredEntry.SetTargetID(lSrcActorID);
	kHatredEntry.SetHatredValue(lHatredValue);
	kHatredEntry.SetUpdateTime(GET_PROCESS_TIME);
	if(pkHatredList->AddHatred(kHatredEntry))
	{
		pkNPC->SaveActiveInfo();
		_broadcastHatred(pkStateMachine, lSrcActorID);
	}
}
//----------------------------------------------------------------------------------------------
void SM_ServerNPCBaseController::_notifySkillEvent_AttachState(StateMachine::SM_StateMachine* pkStateMachine, Skill::SK_SkillEvent* pkSkillEvent)
{
	LC_ServerNPC* pkNPC = static_cast<LC_ServerNPC*>(pkStateMachine);
	LC_ActorHatredList* pkHatredList = pkNPC->GetHatredList();

	object_id_type lSrcActorID		= pkSkillEvent->GetSrcActorID();
	int lSkillTypeID		= pkSkillEvent->GetLogicID();
	int lStateID			= (int)pkSkillEvent->GetParam(SK_AttachStateEvent::SEP_STATE_ID);

	if (pkNPC->GetID() == lSrcActorID)
	{
		return;
	}

	CF_SkillStateList* pkStateList = CF_SkillStateList::GetSingletonPtr();
	CF_SkillStateList::DataEntry* pkStateData = pkStateList->GetEntryPtr(lStateID);
	if (NULL == pkStateData)
		return;

	//添加有害buff
	if (2 == pkStateData->_lBuffType && pkNPC->GetCurrentStateType() != ST_NPC_RETREAT)
	{
		attr_value_type lHatredValue = LC_Helper::ComputeHatredValue(1,lSrcActorID, lSkillTypeID);
		LC_ActorHatredEntry kHatredEntry;
		kHatredEntry.SetTargetID(lSrcActorID);
		kHatredEntry.SetHatredValue(lHatredValue);
		kHatredEntry.SetUpdateTime(GET_PROCESS_TIME);
		pkHatredList->AddHatred(kHatredEntry);

		pkNPC->SaveActiveInfo();

		_broadcastHatred(pkStateMachine, lSrcActorID);
	}
}
//----------------------------------------------------------------------------------------------
void SM_ServerNPCBaseController::_notifySkillEvent_DetachState(StateMachine::SM_StateMachine* pkStateMachine, Skill::SK_SkillEvent* pkSkillEvent)
{
	LC_ServerNPC* pkNPC = static_cast<LC_ServerNPC*>(pkStateMachine);
	LC_ActorHatredList* pkHatredList = pkNPC->GetHatredList();

	object_id_type lSrcActorID		= pkSkillEvent->GetSrcActorID();
	int lSkillTypeID		= pkSkillEvent->GetLogicID();
	int lStateID			= (int)pkSkillEvent->GetParam(SK_AttachStateEvent::SEP_STATE_ID);

	if (pkNPC->GetID() == lSrcActorID)
	{
		return;
	}

	CF_SkillStateList* pkStateList = CF_SkillStateList::GetSingletonPtr();
	CF_SkillStateList::DataEntry* pkStateData = pkStateList->GetEntryPtr(lStateID);
	if (NULL == pkStateData)
		return;

	//驱散有益buff
	if (1 == pkStateData->_lBuffType)
	{
		attr_value_type lHatredValue = LC_Helper::ComputeHatredValue(1,lSrcActorID, lSkillTypeID);
		LC_ActorHatredEntry kHatredEntry;
		kHatredEntry.SetTargetID(lSrcActorID);
		kHatredEntry.SetHatredValue(lHatredValue);
		kHatredEntry.SetUpdateTime(GET_PROCESS_TIME);
		pkHatredList->AddHatred(kHatredEntry);

		pkNPC->SaveActiveInfo();

		_broadcastHatred(pkStateMachine, lSrcActorID);
	}
}
//----------------------------------------------------------------------------------------------
void SM_ServerNPCBaseController::_notifySkillEvent_Retreat(StateMachine::SM_StateMachine* pkStateMachine, Skill::SK_SkillEvent* pkSkillEvent)
{
	LC_ServerNPC* pkNPC = static_cast<LC_ServerNPC*>(pkStateMachine);

	LC_ActorHatredList* pkHatredList = pkNPC->GetHatredList();
	LC_ActorSkillList* pkSkillList = pkNPC->GetActorSkillList();
	LC_NPCGroupBase* pkGroup = pkNPC->GetNPCGroup();

	//NPC集体回撤
	if (pkNPC->GetActive() 
//		&& NULL != pkHatredList 
//		&& NULL != pkSkillList
		&& NULL != pkGroup
		&& pkGroup->GetGroupRetreate())
	{
		pkHatredList->Reset();
		pkSkillList->Reset();
		pkNPC->SetNextState(ST_NPC_RETREAT);
	}
}
//----------------------------------------------------------------------------------------------
void SM_ServerNPCBaseController::_controllerHandleSkillEvent(StateMachine::SM_StateMachine* pkStateMachine, Skill::SK_SkillEvent* pkSkillEvent)
{
	T_ASSERT_CRITICAL_S(pkStateMachine);
	T_ASSERT_CRITICAL_S(pkSkillEvent);

	SkillEventTypeEx eEventType = pkSkillEvent->GetEventType();
	switch (eEventType)
	{
	case SETE_ENTER_GUARD:
		_notifySkillEvent_EnterGuard(pkStateMachine,pkSkillEvent);
		break;
	case SETE_DAMAGE:
		_notifySkillEvent_Damage(pkStateMachine,pkSkillEvent);
		break;
	case SETE_ATTACH_STATE:
		_notifySkillEvent_AttachState(pkStateMachine,pkSkillEvent);
		break;
	case SETE_DETACH_STATE:
		_notifySkillEvent_DetachState(pkStateMachine, pkSkillEvent);
		break;
	case SETE_RETREAT:
		_notifySkillEvent_Retreat(pkStateMachine, pkSkillEvent);
		break;
	default:
		break;
	}
}
//----------------------------------------------------------------------------------------------
void SM_ServerNPCBaseController::_controllerScriptHandleSkillEvent(StateMachine::SM_StateMachine* pkStateMachine, Skill::SK_SkillEvent* pkSkillEvent)
{
	T_ASSERT_CRITICAL_S(pkStateMachine);
	T_ASSERT_CRITICAL_S(pkSkillEvent);

	SM_HandleEventMapIter Iter = m_kHandleSkillEventMap.find(pkSkillEvent->GetEventType());
	if (Iter != m_kHandleSkillEventMap.end())
	{
		SM_StateHandleEventData* pkData = Iter->second;
		if (pkData&&pkData->m_pkFunctor)
			(*pkData->m_pkFunctor)(pkStateMachine,pkSkillEvent);
	}
}
//----------------------------------------------------------------------------------------------
void SM_ServerNPCBaseController::_stateScriptHandleSkillEvent(StateMachine::SM_StateMachine* pkStateMachine, Skill::SK_SkillEvent* pkSkillEvent)
{
	T_ASSERT_CRITICAL_S(pkStateMachine);
	T_ASSERT_CRITICAL_S(pkSkillEvent);

	SM_StateBase* pkState = pkStateMachine->GetCurrentState();
	if (NULL != pkState)
	{
		pkState->NotifySkillEvent(pkStateMachine, pkSkillEvent);
	}
}
//----------------------------------------------------------------------------------------------
void SM_ServerNPCBaseController::_broadcastHatred(StateMachine::SM_StateMachine* pkStateMachine, int lHatredID)
{
	if (NULL == pkStateMachine || (!IS_PLAYER_CLASS_ID(lHatredID) && !IS_NPC_ID(lHatredID)))
	{
		return;
	}

	LC_FactionManager* pkFactionManager = LC_FactionManager::GetSingletonPtr();
	if (NULL == pkFactionManager)
	{
		return;
	}

	LC_ServerNPC* pkNPC = (LC_ServerNPC*)pkStateMachine;

	LC_MapBase* pkMap = pkNPC->GetMap();
	if (NULL == pkMap)
		return;

	//没有广播功能
	if ( (pkNPC->GetProperty_BroadcastID() <= 0 || pkNPC->GetProperty_BroadcastRadius() <= 0) && pkNPC->GetProperty_EliteBroadcastRadius() <= 0)
	{
		return;
	}


	UT_Circle kBroadcastCircle;
	kBroadcastCircle.kCenterPoint = pkNPC->GetCurrentLocation();
	kBroadcastCircle.iRadius = pkNPC->GetProperty_BroadcastRadius();

	CF_ActorFilterFunc* pkFilterList = CF_ActorFilterFunc::GetSingletonPtr();
	//过滤掉玩家和自身
	//注意：LockActorsInCircle方法不能完成过滤的功能，因此需要单独判断是否锁定自身
	CF_ActorFilterFunc::DataEntry* pkFilterData = pkFilterList->GetEntryPtr(6);

	mem::vector<LC_ActorHatredList*> hatred_list;

	LC_ActorHatredEntry kHatredEntry;
	kHatredEntry.SetTargetID(lHatredID);
	kHatredEntry.SetHatredValue(1);
	kHatredEntry.SetUpdateTime(GET_PROCESS_TIME);

	if (NULL != pkFilterData && pkNPC->GetProperty_BroadcastRadius() > 0)
	{
		LC_ActorPtrVector* pkActorList = pkMap->LockActorsInCircle(kBroadcastCircle, pkFilterData);
		if (!pkActorList->empty())
		{
			/*
			*通知给相同广播ID的NPC
			*/
			LC_ActorPtrVectorIter iter;
			LC_ActorBase* pkTarget = NULL;
			LC_ActorHatredList* pkHatredList = NULL;
			TRAVP(pkActorList, iter)
			{
				pkTarget = (*iter);

				if ( IS_NPC_ID(pkTarget->GetID())
					&& pkTarget->GetID() != (uint32_t)lHatredID
					&& pkNPC->GetID() != pkTarget->GetID()
					&& !pkFactionManager->GetEnemyRelation(pkNPC, pkTarget)
					&& pkNPC->GetProperty_BroadcastID() == ((LC_ServerNPC*)pkTarget)->GetProperty_BroadcastID() )
				{
					pkHatredList = pkTarget->GetHatredList();
					hatred_list.push_back(pkHatredList);
				}
			}
		}
	}

	if( pkNPC->GetGroupEliteLevel() > 0)
	{
		if( pkNPC->GetProperty_EliteBroadcastRadius() > 0 )
		{
			LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
			LC_ServerMapLogic_Place* logic=pkWorldManager->GetMapLogic(pkNPC->GetMapLogicID());
			int n = logic->GetEliteCount(pkNPC);
			for(int i = 0; i < n ; ++i)
			{
				LC_ServerNPC * npc = logic->GetInEliteNpc(pkNPC,i);
				if(NULL==npc || npc==pkNPC)
				{
					continue;
				}
				if( UT_MathBase::LineLengthXYInt(npc->GetCurrentLocation(),pkNPC->GetCurrentLocation()) < pkNPC->GetProperty_EliteBroadcastRadius() )
				{
					hatred_list.push_back(npc->GetHatredList());
				}
			}
		}
	}

	for( mem::vector<LC_ActorHatredList*>::iterator i = hatred_list.begin(); i != hatred_list.end(); ++i )
	{
		LC_ActorHatredList * h = *i;
		h->AddHatred(kHatredEntry);
	}

}
//----------------------------------------------------------------------------------------------

