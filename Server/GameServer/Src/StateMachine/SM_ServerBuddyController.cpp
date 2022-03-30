#include "LC_WorldManagerBase.h"
#include "SM_ServerBuddyController.h"
#include "LC_LogicManagerBase.h"
#include "LC_ServerPlayer.h"
#include "LC_FactionManager.h"
#include "NW_Helper.h"
#include "LC_MapBase.h"
#include "SK_ServerSkillEventManager.h"
#include "SK_Factory.h"
#include "Buddy/BuddyInstance.h"
#include "GlobalSettings.h"
#include "UT_PathFindHelp.h"

using namespace StateMachine;
using namespace Skill;
using namespace GameLogic;
using namespace Protocol;
using namespace Utility;
using namespace CSVFile;

//#define		MAX_BUDDY_FOLLOW_DISTANCE			600
//#define		STOP_FOLLOW_DISTANCE				200

#define		STOP_MOVE_DISTANCE					100
#define		SKILL_RUSH_SPEED					1000
#define		FIGHT_UPDATE_TIMER					1.20f
#define		DENY_FIND_PATH_DISTANCE		 100

#define		OWNER_CAST_IDLE_TIME				2.00f
#define		REAL_CAST_SKILL_DISTANCE_RATE		0.8f

//#define		SKILL_DISTANCE_DEBUG_MODE			1
#define		SKILL_DISTANCE_DEBUG_VAL			10

#define		INFINIT_SKILL_DISTANCE				999999
//#define		BUDDY_AI_DEBUG_MODE		1

using namespace Utility;

//----------------------------------------------------------
SM_ServerBuddyController::SM_ServerBuddyController()
	: m_bAllowFightUpdate(true)
{

}
//----------------------------------------------------------
SM_ServerBuddyController::~SM_ServerBuddyController()
{

}
//----------------------------------------------------------
bool SM_ServerBuddyController::UpdateBuddyAction(SM_StateMachine* pkStateMachine, float fCurrentTime,float fDeltaTime)
{
	return true;
}
void SM_ServerBuddyController::Update( SM_StateMachine* pkStateMachine, float fCurrentTime, float fDeltaTime )
{
	m_bAllowFightUpdate = true;
	if (fDeltaTime == 0.00f)
		return;
	LC_ServerPlayer* pkServerPlayer = static_cast<LC_ServerPlayer*>(pkStateMachine);
	//状态更新
	//if (m_eUpdateMachineStateType != SUFT_REPLACE)
	//	_updateMachineState(pkStateMachine, fCurrentTime, fDeltaTime);

	//if(pkServerPlayer->IsInstance() && !pkServerPlayer->GetAIControl())//为真人并且未被控制
	if (pkServerPlayer->GetControlType() == LC_ServerPlayer::eControlType_Player)
	{
		return;
	}

	bool bDenyMove = pkServerPlayer->GetDenyMove();
	if ( bDenyMove )
		return;

	int nBattleUpdate = 0;
	////战斗更新
	pkServerPlayer->RollNextSkill();
	//移动更新
	if (m_eUpdateTransformType != SUFT_REPLACE )
		nBattleUpdate = _updateTransform(pkStateMachine, fCurrentTime, fDeltaTime, nBattleUpdate, nBattleUpdate);

	//战斗更新
	//if (m_eUpdateFightType != SUFT_REPLACE)
	if ( m_bAllowFightUpdate )
		nBattleUpdate = _updateFight(pkStateMachine, fCurrentTime, fDeltaTime, nBattleUpdate);
}
//----------------------------------------------------------
void SM_ServerBuddyController::NotifySkillEvent(SM_StateMachine* pkStateMachine, Skill::SK_SkillEvent* pkSkillEvent)
{
	LC_ServerPlayer* pkPlayer = static_cast<LC_ServerPlayer*>(pkStateMachine);
	LC_ActorHatredList* pkHatredList = pkPlayer->GetHatredList();

	SkillEventTypeEx eEventType = pkSkillEvent->GetEventType();
	object_id_type lSrcActorID = pkSkillEvent->GetSrcActorID();
	object_id_type lDstActorID = pkSkillEvent->GetDstActorID();
	
	switch (eEventType)
	{
	case SETE_ENTER_GUARD:
		{
			attr_value_type lHatredValue = LC_Helper::ComputeHatredValue(1, lSrcActorID, 0);
			LC_ActorHatredEntry kHatredEntry;
			//kHatredEntry.SetTargetID(lSrcActorID);
			kHatredEntry.SetTargetID(lDstActorID);
			kHatredEntry.SetHatredValue(lHatredValue);
			kHatredEntry.SetUpdateTime(GET_PROCESS_TIME);
			pkHatredList->AddHatred(kHatredEntry);
			break;
		}
	case SETE_DAMAGE:
		{
			int lHatredID = 0;
			if (pkPlayer->GetID() == lSrcActorID)
			{
				lHatredID = lDstActorID;
			}
			else if (pkPlayer->GetID() == lDstActorID)
			{
				lHatredID = lSrcActorID;
			}
			int lDamageType = (int)pkSkillEvent->GetParam(SK_DamageEvent::SEP_DAMAGE_TYPE);
			if (SDT_DOT != lDamageType && pkPlayer->GetID() != (uint32_t)lHatredID)
			{
				//这里不需要计算仇恨值，目前玩家仇恨只用于表现进出战斗状态
				LC_ActorHatredEntry kHatredEntry;
				kHatredEntry.SetTargetID(lHatredID);
				kHatredEntry.SetHatredValue(1);
				kHatredEntry.SetUpdateTime(GET_PROCESS_TIME);
				pkHatredList->AddHatred(kHatredEntry);
				if (lDstActorID != pkPlayer->GetID())
					pkPlayer->TrySetDefaultTargetID( lDstActorID );
			}
		}
		break;

	case SETE_ATTACH_STATE:
		{
			//自己给自己添加状态，不做处理
			if (pkPlayer->GetID() == lSrcActorID && lSrcActorID == lDstActorID)
			{
				return;
			}

			int lHatredID = 0;
			if (pkPlayer->GetID() == lSrcActorID)
			{
				lHatredID = lDstActorID;
			}
			else if (pkPlayer->GetID() == lDstActorID)
			{
				lHatredID = lSrcActorID;
			}

			if (pkPlayer->GetID() != (uint32_t)lHatredID)
			{
				int lStateID = (int)pkSkillEvent->GetParam(SK_AttachStateEvent::SEP_STATE_ID);
				CF_SkillStateList* pkStateList = CF_SkillStateList::GetSingletonPtr();
				CF_SkillStateList::DataEntry* pkStateData = pkStateList->GetEntryPtr(lStateID);
				if (NULL == pkStateData)
				{
					return;
				}
				//添加debuff状态
				if (2 == pkStateData->_lBuffType)
				{
					//这里不需要计算仇恨值，目前玩家仇恨只用于表现进出战斗状态
					LC_ActorHatredEntry kHatredEntry;
					kHatredEntry.SetTargetID(lHatredID);
					kHatredEntry.SetHatredValue(1);
					kHatredEntry.SetUpdateTime(GET_PROCESS_TIME);
					pkHatredList->AddHatred(kHatredEntry);
				}
			}
		}
		break;

	default:
		break;
	}
}
//----------------------------------------------------------
bool SM_ServerBuddyController::NotifyStateEnd( SM_StateMachine* pkStateMachine, int iEndState,float fCurrentTime,float fDeltaTime )
{
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkStateMachine;
	if (pkPlayer->GetControlType() == LC_ServerPlayer::eControlType_Player)
		return true;
	if(IS_SKILL_TYPE(iEndState) && !pkPlayer->IsAIControlStand() )
	{
		//pkStateMachine->SetNextState(ST_PLAYER_CHASE);
	}

	return true;
}
//----------------------------------------------------------
void SM_ServerBuddyController::_updateMachineState(StateMachine::SM_StateMachine* pkStateMachine, float fCurrentTime, float fDeltaTime)
{
	LC_ServerPlayer* pkPlayer = static_cast<LC_ServerPlayer*>(pkStateMachine);

	if(pkPlayer->GetDying())
	{
		pkPlayer->SummorFriendTalk(2,fCurrentTime);
		pkPlayer->SetDead(true);
		return;
	}

	//眩晕、睡眠、沉默、近程沉默、远程沉默情况下打断当前技能
	SK_ServerSkillEventManager* pkEventManager = SERVER_GET_SKILL_EVENT_MANAGER();
	if (IS_SKILL_TYPE(pkPlayer->GetCurrentStateType()))
	{
		SK_SkillExecute* pkSkillExecute = (SK_SkillExecute*)pkPlayer->GetCurrentState();
		if ( pkPlayer->GetSkillFlag(SK_DIZZ)
			|| pkPlayer->GetSkillFlag(SK_SLEEP)
			|| (pkPlayer->GetSkillFlag(SK_SILENCE) && pkSkillExecute->GetProperty_DamageType() != 0 && pkPlayer->GetCurrentStateType() != LC_Helper::GetNormalAttackSkill(pkPlayer))
			|| pkPlayer->GetSkillFlag(SK_FREEZE_ANIMATION)
			/*|| (pkPlayer->GetSkillFlag(SK_SILENCE_MELEE) && SRT_MELEE == pkSkillExecute->GetProperty_RangeType())
			|| (pkPlayer->GetSkillFlag(SK_SILENCE_RANGE) && SRT_RANGE == pkSkillExecute->GetProperty_RangeType())*/
			|| pkPlayer->GetSkillFlag(SK_SILENCE_CAREER_SKILL) )	// && pkSkillExecute->GetProperty_CareerType() > CT_NONE && pkSkillExecute->GetProperty_CareerType() < CT_MAX_COUNT) )
		{
			SK_SkillEvent* pkEvent = pkEventManager->AllocEvent();
			if (NULL != pkEvent)
			{
				pkEvent->SetEventType(SETE_INTERRUPT);
				pkEvent->SetSrcActorID(0);
				pkEvent->SetDstActorID(pkPlayer->GetID());
				pkEvent->SetLogicID(0);
				pkEvent->SetParam(SK_InterruptEvent::SEP_BE_INTERRUPT_SKILL_ID, pkPlayer->GetCurrentStateType());
				pkEvent->SetParam(SK_InterruptEvent::SEP_INTERRUPT_TYPE, SKI_EVENT);
				pkEventManager->AddEvent(pkEvent);
			}
		}
	}

	if(pkPlayer->GetDead() && pkPlayer->GetCurrentStateType() != ST_PLAYER_DEAD)
	{ 
		pkPlayer->FixCurrentState(ST_PLAYER_DEAD,fCurrentTime,fDeltaTime);
	}

	//更新仇恨
	_updateHatred(pkStateMachine, fCurrentTime, fDeltaTime);
}
//----------------------------------------------------------
void SM_ServerBuddyController::_updateHatred(SM_StateMachine* pkStateMachine, float fCurrentTime, float fDeltaTime)
{
	LC_ServerPlayer* pkPlayer = static_cast<LC_ServerPlayer*>(pkStateMachine);	

	object_id_type lSelfID = pkPlayer->GetID();

	//仇恨列表
	LC_ActorHatredList* pkHatredList = pkPlayer->GetHatredList();

	LC_WorldManagerBase* pkWorldManager = ENGINE_GET_WORLD_MANAGER();
	if (NULL == pkWorldManager)
	{
		return;
	}

	/*
	*更新仇恨列表
	*/
	LC_ActorHatredEntry* pkHatredEntry = NULL;
	int iHatredCount = pkHatredList->GetHatredCount();
//	bool bAllImmortal = true;
	//仇恨列表的访问索引
	for (int i = 0; i < iHatredCount; i ++)
	{
		pkHatredEntry = pkHatredList->GetSortHatredByIndex(i);

		if (pkHatredEntry && pkHatredEntry->GetValid())
		{
			object_id_type lTargetID = pkHatredEntry->GetTargetID();
			if (IS_PLAYER_CLASS_ID(lTargetID) || IS_NPC_ID(lTargetID))
			{
				LC_ActorBase* pkTarget = (LC_ActorBase*)pkWorldManager->FindObject(lTargetID);
				//目标无效
				if (NULL == pkTarget 
					|| lSelfID == lTargetID
					|| pkTarget->GetDead())
				{
					pkHatredList->RemoveHatred(lTargetID);
					continue;
				}
			}
		}
	}
}
//----------------------------------------------------------
object_id_type SM_ServerBuddyController::_updateTransform(SM_StateMachine* pkStateMachine, float fCurrentTime, float fDeltaTime, bool bCombat, object_id_type nTargetId )
{
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(pkStateMachine);

	float fLastTranslateTime = pkPlayer->GetLastTranslateTime();
	float fDeltaTimeTemp = fCurrentTime - fLastTranslateTime;
	pkPlayer->SetLastTranslateTime(fCurrentTime);

	//_translateAndRotateServerBuddy(pkStateMachine, fCurrentTime,fDeltaTimeTemp);
	return _updateMoveLogic(pkStateMachine, fCurrentTime,fDeltaTimeTemp, bCombat, nTargetId);
	//_updateMoveImpl_Path(pkStateMachine, fCurrentTime,fDeltaTimeTemp);
	//return 1;
}

bool SM_ServerBuddyController::_updateMoveLogic_ForceFollow(StateMachine::SM_StateMachine* pkStateMachine, float fCurrentTime,float fDeltaTime)
{
	bool bResult = false;

	LC_ServerPlayer* pkPlayer = static_cast<LC_ServerPlayer*>(pkStateMachine);

	if (!pkPlayer->GetBuddyAIParam_ForceFollowMode())
		return bResult;

	bResult = true;

	UT_Vec3Int kTargetLocation;
	UT_Vec3Int kCurrentLocation;

	object_id_type nFollowId = pkPlayer->GetFollowTargetObjectId();
	LC_WorldManagerBase* pkWorldManager = ENGINE_GET_WORLD_MANAGER();
	LC_ActorBase* pFollowTarget = (LC_ActorBase*)(pkWorldManager->FindObject(nFollowId));
	if (pFollowTarget == NULL)
		return bResult;

	SM_ServerBuddyController::GetFollowLocation( pkStateMachine, pFollowTarget, kTargetLocation );

	//kTargetLocation = pFollowObj->GetCurrentLocation();
	kCurrentLocation = pkPlayer->GetCurrentLocation();

	int nFollowDistance = UT_MathBase::LineLengthXYInt(pkPlayer->GetCurrentLocation(), pFollowTarget->GetCurrentLocation() );

	bool bDenyFindPath = false;
	float fSpeedRate = 1.00f;
	if (nFollowDistance > GetGlobalSetting.nMaxBuddyCombatFollowDistance)
	{
		bDenyFindPath = true;
		fSpeedRate = 1.5f;
	}
	_updateMoveImpl(pkStateMachine, fCurrentTime, fDeltaTime, nFollowId, kCurrentLocation, kTargetLocation, 0, false, false, true, fSpeedRate);	

	int32_t nSelfDistance = UT_MathBase::LineLengthXYInt( pkPlayer->GetTargetLocation() , pkPlayer->GetCurrentLocation());
	// just force sync stop pos.
	//if( !GetGlobalSetting.BuddyAIDebugMode && !bFollow_Combat && nDistance <= GetGlobalSetting.nMaxBuddyFollowDistance && nSelfDistance <= STOP_MOVE_DISTANCE)
	UT_LinePath& rkMovePath = pkPlayer->GetMovePath();
	//if(nSelfDistance <= STOP_MOVE_DISTANCE && rkMovePath.GetPathPointCount() == 0)
	if(nSelfDistance <= STOP_MOVE_DISTANCE )
	{
		rkMovePath.Clear();
		//pkPlayer->SetCurrentLocation(pkPlayer->GetTargetLocation());
		pkPlayer->StopMove();
		pkPlayer->SetBuddyAIParam_ForceFollowMode(false);
		return true;
	}

	//_updateMoveImpl(pkStateMachine, fCurrentTime, fDeltaTime, nFollowId, kCurrentLocation, kTargetLocation, 0, false);	

	return bResult;
}

bool SM_ServerBuddyController::_doMoveLogic_BeginFollow(StateMachine::SM_StateMachine* pkStateMachine, float fCurrentTime,float fDeltaTime, LC_ActorBase* pFollowTarget)
{
	LC_ServerPlayer* pkPlayer = static_cast<LC_ServerPlayer*>(pkStateMachine);

	if (pkPlayer->GetBuddyAIParam_ForceFollowMode() == true)
		return true;

	pkPlayer->SetBuddyAIParam_ForceFollowMode(true);

	UT_Vec3Int kTargetLocation;
	UT_Vec3Int kCurrentLocation;

	SM_ServerBuddyController::GetFollowLocation( pkStateMachine, pFollowTarget, kTargetLocation );

	//kTargetLocation = pFollowObj->GetCurrentLocation();
	kCurrentLocation = pkPlayer->GetCurrentLocation();

	_updateMoveImpl(pkStateMachine, fCurrentTime, fDeltaTime, pFollowTarget->GetID(), kCurrentLocation, kTargetLocation, 0, false);	
	
	return true;
}

bool SM_ServerBuddyController::_updateMoveLogic_Skill(StateMachine::SM_StateMachine* pkStateMachine, float fCurrentTime,float fDeltaTime)
{
	bool bResult = false;
	LC_ServerPlayer* pkPlayer = static_cast<LC_ServerPlayer*>(pkStateMachine);

	Utility::UT_Vec3Int src = pkPlayer->GetCurrentLocation();
	Utility::UT_Vec3Int dst = pkPlayer->GetTargetLocation();
	int nSpeed = pkPlayer->GetMoveSpeed();

	int32_t nState = pkPlayer->GetCurrentStateType();

	//if (src != dst && nSpeed > SKILL_RUSH_SPEED)
	if (  (src.x != dst.x || src.y != dst.y)   && nSpeed > SKILL_RUSH_SPEED)
	//if (src.x != dst.x || src.y != dst.y) 
	{
		bResult = true;
		_updateMoveImpl(pkStateMachine, fCurrentTime, fDeltaTime, 0, src, dst, 0, false, true);
	}


	return bResult;
}

bool SM_ServerBuddyController::_updateTryForceDrag(LC_ServerPlayer* pBuddy, LC_ServerPlayer* pCurPlayer, float fCurrentTime,float fDeltaTime)
{
	m_bAllowFightUpdate = true;
	if ( GetGlobalSetting.nEnableBuddyDrag == 0)
		return false;
	if ( pBuddy == NULL || pCurPlayer == NULL )
		return false;

	if (pBuddy->GetAllSkillFlag() & SK_JUMP_NA_FLAGS)
		return false;

	LC_MapBase* pMap = pBuddy->GetMap();
	if ( pMap == NULL )
		return false;

	int32_t nDistance = UT_MathBase::LineLengthXYInt( pBuddy->GetCurrentLocation() , pCurPlayer->GetCurrentLocation());
	if ( nDistance <GetGlobalSetting.nMaxBuddyDragDistance )
		return false;

	Utility::UT_Vec3Int kDstLoc;// = pCurPlayer->GetCurrentLocation();
	const Utility::UT_Vec3Int& kSrcLoc = pBuddy->GetCurrentLocation();
	SM_ServerBuddyController::GetFollowLocation( pBuddy, pCurPlayer, kDstLoc );
	// ClearEverything
	pCurPlayer->GetHatredList()->RemoveHatred(pCurPlayer->GetDefaultTargetID());
	pCurPlayer->SetDefaultTargetID(INVALID_ACTOR_ID);
	pCurPlayer->SetLogicCombatFlag(false);
	pBuddy->ClearCombatStatusAsBuddy();
	pBuddy->TryInterruptSkill( -1, fCurrentTime, SKI_FORCE);
	pBuddy->TryFinishSkill( -1, fCurrentTime, kSrcLoc.x, kSrcLoc.y );
	// DoDrag
	pBuddy->	SetCurrentLocation(kDstLoc);
	pBuddy->SetTargetLocation(kDstLoc);
	pBuddy->ActiveSkillState(GetGlobalSetting.nBuddyDragBuffID, GetGlobalSetting.nBuddyDragBuffTimeInSecond, pBuddy->GetID());

	m_bAllowFightUpdate = false;
	return true;
}

object_id_type SM_ServerBuddyController::_updateMoveLogic(StateMachine::SM_StateMachine* pkStateMachine, float fCurrentTime,float fDeltaTime, bool bCombat, object_id_type nTargetId)
{
	LC_ServerPlayer* pkPlayer = static_cast<LC_ServerPlayer*>(pkStateMachine);

	if ( pkPlayer->GetDenyMove() )
		return INVALID_ACTOR_ID;

	LC_ServerPlayer* pCurrentPlayer = pkPlayer->GetCurrentActivateBuddy();
	if ( pCurrentPlayer == NULL )
		return INVALID_ACTOR_ID;
	user_id_type uid1 =  pkPlayer->GetUserID();
	if (pkPlayer->GetDead())
	{
		pkPlayer->SetCurrentLocation( pCurrentPlayer->GetCurrentLocation());
		pkPlayer->SetTargetLocation( pCurrentPlayer->GetCurrentLocation());
		pkPlayer->SetLockedLocation( pCurrentPlayer->GetCurrentLocation());
		//pkPlayer->SyncLocationInfo();
	}

	bool bDragFlag = _updateTryForceDrag(pkPlayer, pCurrentPlayer, fCurrentTime, fDeltaTime );
	if ( bDragFlag )
	{
		return INVALID_ACTOR_ID;
	}

	LC_ServerPlayer* pOwner = pkPlayer->GetOwnerPlayer();
	if (pOwner == NULL)
		return INVALID_ACTOR_ID;

	// 因为技能导致的移动
	if (_updateMoveLogic_Skill(pkStateMachine, fCurrentTime, fDeltaTime))
		return INVALID_ACTOR_ID;

	if (_updateMoveLogic_ForceFollow(pkStateMachine, fCurrentTime, fDeltaTime))
		return INVALID_ACTOR_ID;

	user_id_type uid2 =  pkPlayer->GetUserID();
	LC_ActorBase* pFollowObj = pCurrentPlayer;
	int32_t nOwnerDistance = UT_MathBase::LineLengthXYInt( pCurrentPlayer->GetCurrentLocation() , pkPlayer->GetCurrentLocation());
	bool bFollow_Combat = false;
	object_id_type lSelectTarget = _selectTarget(pkStateMachine, fCurrentTime, fDeltaTime);
	//object_id_type lSelectTarget = _selectTarget_orig(pkStateMachine, fCurrentTime, fDeltaTime);
	int nMaxFollowDistance = 0;
	bool bTryFollow = false;
	if (pCurrentPlayer->GetLogicCombatFlag())
	{
		//nMaxFollowDistance = GetGlobalSetting.nMaxBuddyCombatFollowDistance;
		if (nOwnerDistance > GetGlobalSetting.nMaxBuddyCombatFollowDistance)
		{
			nMaxFollowDistance = GetGlobalSetting.nMaxBuddyCombatFollowDistance;
			bTryFollow = true;
		}
	}
	else
	{
		//nMaxFollowDistance = GetGlobalSetting.nMaxBuddyFollowDistance;
		if (nOwnerDistance > GetGlobalSetting.nMaxBuddyFollowDistance)
		{
			nMaxFollowDistance = GetGlobalSetting.nMaxBuddyFollowDistance;
			bTryFollow = true;
		}
	}
	// 超过最大距离，强制跟随
	//if (nOwnerDistance > GetGlobalSetting.nMaxBuddyFollowDistance)
	//if (nOwnerDistance > nMaxFollowDistance )//&& pCurrentPlayer->GetLogicCombatFlag())
	if (bTryFollow && pCurrentPlayer->GetLogicCombatFlag())
	{
		pkPlayer->TryInterruptSkill( -1, fCurrentTime, SKI_FORCE);
		pkPlayer->TryFinishSkill( -1, fCurrentTime, pkPlayer->GetCurrentLocation().x, pkPlayer->GetCurrentLocation().y );
		//pkPlayer->SetBuddyAIParam_ForceFollow(true);
		_doMoveLogic_BeginFollow(pkStateMachine, fCurrentTime, fDeltaTime, pCurrentPlayer);
		//pCurrentPlayer->SetLogicCombatFlag(false);
		return INVALID_ACTOR_ID;
	}

	user_id_type uid3 =  pkPlayer->GetUserID();
	if (GetGlobalSetting.BuddyAIDebugMode)
		lSelectTarget = pCurrentPlayer->GetID();

	//if (nOwnerDistance <= GetGlobalSetting.nMaxBuddyFollowDistance && lSelectTarget != 0 && 
	//	( !GetGlobalSetting.BuddyAIDebugMode && pCurrentPlayer->GetLogicCombatFlag())
	//	)
	//if (nOwnerDistance <= nMaxFollowDistance && lSelectTarget != 0 && 
	if (lSelectTarget != 0 && 
		( !GetGlobalSetting.BuddyAIDebugMode && pCurrentPlayer->GetLogicCombatFlag() && !pkPlayer->GetBuddyAIParam_ForceFollowMode())
		)
	{
		LC_WorldManagerBase* pkWorldManager = ENGINE_GET_WORLD_MANAGER();
		LC_ActorBase* pTarget = (LC_ActorBase*)(pkWorldManager->FindObject(lSelectTarget));
		if (pTarget)
		{
			int nTargetDistance = UT_MathBase::LineLengthXYInt(pkPlayer->GetCurrentLocation(), pTarget->GetCurrentLocation() );
			if ( nTargetDistance < GetGlobalSetting.nMaxBuddyCombatFollowDistance )
			{
				pFollowObj = pTarget;
				bFollow_Combat = true;
			}
		}
	}

	//if (pCurrentPlayer->GetSkillFlag( SK_FLY ))
	//{
	//	pkPlayer->StopMove();
	//	return INVALID_ACTOR_ID;
	//}

	user_id_type uid4 =  pkPlayer->GetUserID();
	int nFollowChange = UT_MathBase::LineLengthXYInt(pkPlayer->GetFollowLocation(), pFollowObj->GetCurrentLocation() );
	int nSkillDistance = 0;
	LC_SkillAssetEntry* pSkillEntry = pkPlayer->GetNextSkill();
	user_id_type uid5 =  pkPlayer->GetUserID();
	int nReuse1 = pkPlayer->GetReuseCount();
	if ( pSkillEntry )
	{
		CF_SkillExecuteList::DataEntry* pkSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(pSkillEntry->GetTypeID());
		if (pkSkillData)
			nSkillDistance = pkSkillData->_lMaxExecuteDistance * REAL_CAST_SKILL_DISTANCE_RATE;

		user_id_type uid6 =  pkPlayer->GetUserID();
		int nReuse2 = pkPlayer->GetReuseCount();
		if (pkSkillData && pkSkillData->_lDamageType == 0 && pkSkillData->_lOperationType == SOT_LOCATION)
		{
			nSkillDistance = INFINIT_SKILL_DISTANCE;
		}
#if defined SKILL_DISTANCE_DEBUG_MODE
		// Debug mode
		nSkillDistance = SKILL_DISTANCE_DEBUG_VAL;
#endif
	}

	int32_t nSelfDistance = UT_MathBase::LineLengthXYInt( pkPlayer->GetTargetLocation() , pkPlayer->GetCurrentLocation());
	int32_t nDistance = UT_MathBase::LineLengthXYInt( pFollowObj->GetCurrentLocation() , pkPlayer->GetCurrentLocation());

	// just force sync stop pos.
	if( !GetGlobalSetting.BuddyAIDebugMode && !bFollow_Combat && nDistance <= GetGlobalSetting.nStopFollowDistance && nSelfDistance <= STOP_MOVE_DISTANCE)
	{
		if ( pkPlayer->GetMoveSpeed() > 0 || pkPlayer->GetBuddyAIParam_ForceFollowMode() )
		{
			pkPlayer->StopMove();
			//pkPlayer->SetCurrentLocation(pkPlayer->GetTargetLocation());
			pkPlayer->SetBuddyAIParam_ForceFollowMode(false);
			//pkPlayer->SetMoveSpeed(0);
		}
		return INVALID_ACTOR_ID;
	}

	UT_Vec3Int kTargetLocation;
	UT_Vec3Int kCurrentLocation;

	if (GetGlobalSetting.BuddyAIDebugMode)
	{
		SM_ServerBuddyController::GetSkillLocation( pkStateMachine, pFollowObj, kTargetLocation, nSkillDistance );
	}
	else
	{
		if (bFollow_Combat)
		{
			// 直接打
			if (nSkillDistance >= nDistance)
			{
				return lSelectTarget;
			}
			// 追击
			else
			{
				//kTargetLocation = 
				SM_ServerBuddyController::GetSkillLocation( pkStateMachine, pFollowObj, kTargetLocation, nSkillDistance );
			}
		}
		else
		{
			//kTargetLocation = pCurrentPlayer->GetCurrentLocation();
			// Here should call SM_ServerBuddyController::GetFollowLocation
			SM_ServerBuddyController::GetFollowLocation( pkStateMachine, pFollowObj, kTargetLocation );
		}
	}

	//if ( pkPlayer->GetDenyMove() )
	//	return INVALID_ACTOR_ID;

	//kTargetLocation = pFollowObj->GetCurrentLocation();
	kCurrentLocation = pkPlayer->GetCurrentLocation();

	int32_t nDebugDistance = UT_MathBase::LineLengthXYInt( kTargetLocation , kCurrentLocation);

	if ( nDebugDistance < 5 )
		return INVALID_ACTOR_ID;

	_updateMoveImpl(pkStateMachine, fCurrentTime, fDeltaTime, pFollowObj->GetID(), kCurrentLocation, kTargetLocation, nFollowChange, bFollow_Combat);

	return INVALID_ACTOR_ID;
}

int SM_ServerBuddyController::_doMoveImpl_FindPath( LC_ServerPlayer* pPlayer, LC_ServerPlayer* pCurrentPlayer, float fSpeed, float fCurrentTime,float fDeltaTime )
{
	if ( pPlayer == NULL || pCurrentPlayer == NULL)
		return false;

	// input: [end] FollowLocation, buddy's target, not current activate buddy's location.
	const Utility::UT_Vec3Int& nInputPos = pPlayer->GetFollowLocation();
	// output: [begin] pPlayer->SetCurrentLocation();
	Utility::UT_Vec3Int nOutputPos;

	float fMoveDistance = (float)fSpeed * fDeltaTime;

	UT_Vec3Int kTargetLocation;
	UT_Vec3Int kCurrentLocation;// = pPlayer->GetCurrentLocation();

	UT_Vec2Float kMoveDirXY;
	UT_LinePath& rkMovePath = pPlayer->GetMovePath();
	int32_t nDistance = UT_MathBase::LineLengthXYInt(pPlayer->GetCurrentLocation(), pPlayer->GetTargetLocation());
	if (nDistance <= GetGlobalSetting.nMaxBuddyFollowDistance)
		return false;
	bool bIgnoreFindPath = UT_MathBase::EqualVec3IntAs2( pCurrentPlayer->GetCurrentLocation(), pPlayer->GetFollowBaseLocation() );
	if ( bIgnoreFindPath )
		return false;
	bool bEqual = UT_MathBase::EqualVec3IntAs2(pPlayer->GetTargetLocation(), rkMovePath.GetTargetPoint());
	bool bCheckInPath = rkMovePath.CheckInPath(pPlayer->GetCurrentLocation());
	float fStep = rkMovePath.GetMoveStep();
	int nPointCount = rkMovePath.GetPathPointCount();
	//if(rkMovePath.GetPathPointCount()<2 || pPlayer->GetTargetLocation()!=rkMovePath.GetTargetPoint() || rkMovePath.GetMoveStep()>=1.0f ||!rkMovePath.CheckInPath(pPlayer->GetCurrentLocation()))
	if(nPointCount<2 || !bEqual || fStep>=1.0f ||!bCheckInPath)
	{
		int nDistance = UT_MathBase::LineLengthXYInt( pPlayer->GetTargetLocation() , rkMovePath.GetTargetPoint());
		//搜索路径
		rkMovePath.Clear();
		//rkMovePath.SetStartPoint(pPlayer->GetCurrentLocation());
		//rkMovePath.SetTargetPoint(pPlayer->GetTargetLocation());
		rkMovePath.SetStartPoint(pPlayer->GetCurrentLocation());
		rkMovePath.SetTargetPoint(nInputPos);
		pPlayer->FindPathTo(pPlayer->GetTargetLocation());
	}
	int iIndex = -1;
	//if( rkMovePath.MoveAlongPath(fMoveDistance,kCurrentLocation,kTargetLocation,kMoveDirXY,iIndex, true) )
	if( rkMovePath.MoveAlongPath(fMoveDistance,kCurrentLocation,kTargetLocation,kMoveDirXY,iIndex) )
	//if( rkMovePath.MoveAlongPath(fMoveDistance,kCurrentLocation,nInputPos,kMoveDirXY,iIndex) )
	{
		int iLastPathPoint = pPlayer->GetLastPathPointIndex();
		if (iLastPathPoint != iIndex && iIndex >= 0)
			pPlayer->SetLastPathPointIndex(iIndex);
		pPlayer->SetForwardDir( kMoveDirXY );
		pPlayer->SetCurrentLocation( kCurrentLocation );
		pPlayer->SetMoveLineTargetLocation( kTargetLocation );	
	}
	else
	{
		//沿着路点走不成功，直接走
		rkMovePath.Clear();
		//return false;
		//kCurrentLocation = pPlayer->GetCurrentLocation();
		//pPlayer->SetCurrentLocation(kCurrentLocation);
		//pPlayer->SetMoveLineTargetLocation(kCurrentLocation);
	}

	return true;
}

int SM_ServerBuddyController::_updateMoveImpl(StateMachine::SM_StateMachine* pkStateMachine, float fCurrentTime,float fDeltaTime, 
							object_id_type nFollowTargetId, UT_Vec3Int& kCurrentLocation, UT_Vec3Int& kTargetLocation, int nFollowChange, bool bCombat, bool bForce, bool bDenyFindPath, float fSpeedRate)
{
	UT_Vec3Int kOrigTarget = kTargetLocation;
	LC_ServerPlayer* pkPlayer = static_cast<LC_ServerPlayer*>(pkStateMachine);
	if ( pkPlayer->GetAllSkillFlag() & SK_JUMP_NA_FLAGS )
		return false;
	if (!bForce)
	{
		int nCurStat = pkPlayer->GetCurrentStateType();
		if (IS_SKILL_TYPE(nCurStat))
		{
			return false;
		}
		// 只要当前的FollowLocation和跟随对象的距离大于STOP_FOLLOW_DISTANCE，就继续追
		//if ( nFollowChange >= 2 * GetGlobalSetting.nStopFollowDistance )
		{
			//int32_t nXOffset = UT_MathBase::RandInRangeInt(-STOP_FOLLOW_DISTANCE, STOP_FOLLOW_DISTANCE);
			//int32_t nYOffset = UT_MathBase::RandInRangeInt(-STOP_FOLLOW_DISTANCE, STOP_FOLLOW_DISTANCE);
			//kTargetLocation.x += nXOffset;
			//kTargetLocation.y += nYOffset;
			pkPlayer->SetFollowLocation(kTargetLocation, bCombat, nFollowTargetId);
		}
		//else
		{
			kTargetLocation.x = pkPlayer->GetFollowLocation().x;
			kTargetLocation.y = pkPlayer->GetFollowLocation().y;
		}
	}
	else
	{
		//pkPlayer->SetFollowLocation(kTargetLocation, bCombat, nFollowTargetId);
		//kTargetLocation.x = pkPlayer->GetFollowLocation().x;
		//kTargetLocation.y = pkPlayer->GetFollowLocation().y;
	}

	LC_ServerPlayer* pCurrentOpPlayer = pkPlayer->GetCurrentActivateBuddy();
	if (pCurrentOpPlayer == NULL)
		return 0;
	int	iMoveSpeed	= pkPlayer->GetMoveSpeed();
	if ( iMoveSpeed == 0 )
	{
		int nSpeed = pCurrentOpPlayer->GetFinalAttributeMap()->GetAttribute(ATT_MOVE_SPEED);
		nSpeed *= fSpeedRate;
		pkPlayer->SetMoveSpeed(nSpeed);
		iMoveSpeed	= pkPlayer->GetMoveSpeed();
	}
	bool bSkillMove = false;
	if ( iMoveSpeed >= SKILL_RUSH_SPEED )
	{
		bSkillMove = true;
	}
	
	int bWithPath = _doMoveImpl_FindPath(pkPlayer, pCurrentOpPlayer, iMoveSpeed, fCurrentTime, fDeltaTime);
	UT_Vec3Int newSrc;
	if (bDenyFindPath || bSkillMove)
		bWithPath = false;
	//bWithPath = false;
	float fMoveDistance = iMoveSpeed * fDeltaTime;
	if (!bWithPath)
	{
		newSrc = UT_MathBase::MoveAlongInt(kCurrentLocation, kTargetLocation, fMoveDistance);
	}
	else
		newSrc = pkPlayer->GetCurrentLocation();
	int nCurStat = pkPlayer->GetCurrentStateType();
	int nDistance = UT_MathBase::LineLengthXYInt( kTargetLocation , newSrc);
	// Stop cast while move, buddy only
	if ( !bSkillMove )
	{
		pkPlayer->TryInterruptSkill( -1, fCurrentTime, SKI_FORCE);
		pkPlayer->TryFinishSkill( -1, fCurrentTime, pkPlayer->GetCurrentLocation().x, pkPlayer->GetCurrentLocation().y );
	}
	// Stop cast while move, buddy only
#if 1	// With pathfind result.
	MG_Sync_GamePlayerMove syncMsg;
	syncMsg.m_iPlayerID = pkPlayer->GetID();;
	syncMsg.m_fClientTime = GET_PROCESS_TIME;
	syncMsg.m_lCurrentLocationX = newSrc.x;
	syncMsg.m_lCurrentLocationY = newSrc.y;
	syncMsg.m_lCurrentLocationZ = newSrc.z;
	syncMsg.m_lMoveSpeed = iMoveSpeed;
	syncMsg.m_lMapID = pkPlayer->GetMapLogicID();
	syncMsg.m_lTargetLocationX = kTargetLocation.x;
	syncMsg.m_lTargetLocationY = kTargetLocation.y;
	syncMsg.m_nActionMode = PLAYER_DEFAULT_MOVE_ACTION_MODE;
	if (!bWithPath )
	{
		if ( pkPlayer->GetCurrentLocation().x - kTargetLocation.x != 0 || pkPlayer->GetCurrentLocation().y - kTargetLocation.y != 0 )
		{
			UT_Vec2Float kMoveDirXY;
			kMoveDirXY.x = Int2Float(kTargetLocation.x - pkPlayer->GetCurrentLocation().x);
			kMoveDirXY.y = Int2Float(kTargetLocation.y - pkPlayer->GetCurrentLocation().y);
			kMoveDirXY.Unitize();
			pkPlayer->SetForwardDir( kMoveDirXY );
		}
	}
	syncMsg.m_sCurrentDirAngle = pkPlayer->GetForwardDirAngle();
#endif
#if 0
	MG_Sync_GamePlayerMove syncMsg;
	syncMsg.m_iPlayerID = pkPlayer->GetID();;
	syncMsg.m_fClientTime = GET_PROCESS_TIME;
	syncMsg.m_lCurrentLocationX = newSrc.x;
	syncMsg.m_lCurrentLocationY = newSrc.y;
	syncMsg.m_lCurrentLocationZ = newSrc.z;
	syncMsg.m_lMoveSpeed = iMoveSpeed;
	syncMsg.m_lMapID = pkPlayer->GetMapLogicID();
	syncMsg.m_lTargetLocationX = kTargetLocation.x;
	syncMsg.m_lTargetLocationY = kTargetLocation.y;
	UT_Vec2Float kMoveDirXY;
	kMoveDirXY.x = Int2Float(pkPlayer->GetCurrentLocation().x - kTargetLocation.x);
	kMoveDirXY.y = Int2Float(pkPlayer->GetCurrentLocation().y - kTargetLocation.y);
	kMoveDirXY.Unitize();
	pkPlayer->SetForwardDir( kMoveDirXY );
	syncMsg.m_sCurrentDirAngle = pkPlayer->GetForwardDirAngle();;
#endif
	pkPlayer->BuddyOperation_Move(syncMsg, fCurrentTime, true);

	return 0;
}
//----------------------------------------------------------
void SM_ServerBuddyController::_updateMoveImpl_Path( SM_StateMachine* pkStateMachine, float fCurrentTime,float fDeltaTime )
{
	LC_ServerPlayer* pkPlayer = static_cast<LC_ServerPlayer*>(pkStateMachine);

	if (pkPlayer->GetDead())
		pkPlayer->SetTargetLocation( pkPlayer->GetCurrentLocation());

	LC_ServerPlayer* pCurrentPlayer = pkPlayer->GetCurrentActivateBuddy();
	if (pCurrentPlayer == NULL)
		return;

	int	iMoveSpeed	= pkPlayer->GetMoveSpeed();
	if(iMoveSpeed <= 0)
		return;
	
	if(UT_MathBase::LineLengthXYInt( pkPlayer->GetTargetLocation() , pkPlayer->GetCurrentLocation()/*m_kMovePath.GetPathPoint(iPathPointCount - 1)*/) <= DISTANCE_ADJUST)
	{
		return;
	}
	float fMoveDistance = (float)iMoveSpeed * fDeltaTime;

	UT_Vec3Int kTargetLocation;
	UT_Vec3Int kCurrentLocation;

	UT_Vec2Float kMoveDirXY;
	UT_LinePath& rkMovePath = pkPlayer->GetMovePath();
	if(rkMovePath.GetPathPointCount()<2 || pkPlayer->GetTargetLocation()!=rkMovePath.GetTargetPoint() || rkMovePath.GetMoveStep()>=1.0f ||!rkMovePath.CheckInPath(pkPlayer->GetCurrentLocation()))
	{
		//搜索路径
		rkMovePath.Clear();
		rkMovePath.SetStartPoint(pkPlayer->GetCurrentLocation());
		rkMovePath.SetTargetPoint(pkPlayer->GetTargetLocation());
		pkPlayer->FindPathTo(pkPlayer->GetTargetLocation());
	}
	int iIndex = -1;
	if( rkMovePath.MoveAlongPath(fMoveDistance,kCurrentLocation,kTargetLocation,kMoveDirXY,iIndex) )
	{
		int iLastPathPoint = pkPlayer->GetLastPathPointIndex();
		if (iLastPathPoint != iIndex && iIndex >= 0)
			pkPlayer->SetLastPathPointIndex(iIndex);
		pkPlayer->SetForwardDir( kMoveDirXY );
		pkPlayer->SetCurrentLocation( kCurrentLocation );
		pkPlayer->SetMoveLineTargetLocation( kTargetLocation );	
	}
	else
	{
		//沿着路点走不成功，直接走
		rkMovePath.Clear();
		kCurrentLocation = pkPlayer->GetCurrentLocation();
		pkPlayer->SetCurrentLocation(kCurrentLocation);
		pkPlayer->SetMoveLineTargetLocation(kCurrentLocation);
	}
}
//----------------------------------------------------------
StateMachine::StateProcessType SM_ServerBuddyController::_updateCastSkill(StateMachine::SM_StateMachine* pkStateMachine, float fCurrentTime, float fDeltaTime, object_id_type nTargetId)
{
	static int gsRunning = 1;
	if ( gsRunning == 0 )
		return SPT_SUSPEND;
	LC_ServerPlayer* pkPlayer = static_cast<LC_ServerPlayer*>(pkStateMachine);
	if(pkPlayer->GetDead() || pkPlayer->GetDying())
		return SPT_SUSPEND;

	LC_ServerPlayer* pCurrentPlayer = pkPlayer->GetCurrentActivateBuddy();
	if (pCurrentPlayer == NULL)
		return SPT_SUSPEND;
	int32_t nBuddyId = pkPlayer->GetBuddyID();
	//SKILL_TYPE_SECT  LC_SkillAssetEntry* RandomChooseSkill
	BuddyInstance* pBuddy = pkPlayer->GetAssignedBuddyInstance();
	if ( pBuddy == NULL )
		return SPT_SUSPEND;
	// try to stop
	float fLastCastTime = pCurrentPlayer->GetLastCastSkillTime();
	//if ( fCurrentTime - fLastCastTime > OWNER_CAST_IDLE_TIME && nTargetId == INVALID_ACTOR_ID)
	//if ( fCurrentTime - fLastCastTime > OWNER_CAST_IDLE_TIME)
//#if !defined BUDDY_AI_DEBUG_MODE
	if (!GetGlobalSetting.BuddyAIDebugMode)
	{
		if ( !pCurrentPlayer->GetAIControl() && ( pCurrentPlayer->GetLogicCombatFlag() == false || pkPlayer->GetBuddyAIParam_ForceFollowMode() ) )
		{
			if (pkPlayer->GetBuddyAIParam_ForceFollowMode())
			{
				pkPlayer->TryInterruptSkill( -1, fCurrentTime, SKI_FORCE);
				pkPlayer->TryFinishSkill( -1, fCurrentTime, pkPlayer->GetCurrentLocation().x, pkPlayer->GetCurrentLocation().y );
			}
			return SPT_SUSPEND;
		}
	}
//#endif
	//LC_SkillAssetEntry* pSkillEntry = pBuddy->GetSkillAsset()->GetSkillAssetData()->RandomChooseSkill((skill_cat_type)SKILL_TYPE_SECT);
	LC_SkillAssetEntry* pSkillEntry = pkPlayer->GetNextSkill();
	//LC_SkillAssetEntry* pSkillEntry = pBuddy->GetSkillAsset()->GetSkillAssetData()->GetSkillAssetEntry((skill_cat_type)SKILL_TYPE_SECT, (int32_t)0);
	if (pSkillEntry == NULL)
	{
		pkPlayer->RollNextSkill();
		pSkillEntry = pkPlayer->GetNextSkill();
	}
	if (pSkillEntry == NULL)
		return SPT_SUSPEND;
	int32_t nSkillId = pSkillEntry->GetTypeID();
	if (nSkillId == 0)
		return SPT_SUSPEND;

	// CD Test
	//if (nBuddyId == 3)
	//	nSkillId = 22401001;

	// CDChecking
	SK_SkillExecute* pSkill = SK_Factory::GetSingletonPtr()->RequestSkillExcute(nSkillId);
	if (pSkill == NULL)
		return SPT_SUSPEND;
	ResultType iResult = pkPlayer->CheckExecuteSkill(fCurrentTime, pSkill, 0);
	if (iResult != RE_SKILL_CAN_TAKE_PLACE)
	{
		// Reselect ID
	}

	LC_WorldManagerBase* pkWorldManager = ENGINE_GET_WORLD_MANAGER();
	LC_ActorBase* pTarget = (LC_ActorBase*)(pkWorldManager->FindObject(nTargetId));
	if (pTarget == NULL )
	{
		if (pkPlayer->IsInBaseSkill())
		{
			pkPlayer->TryInterruptSkill( -1, fCurrentTime, SKI_FORCE);
			pkPlayer->TryFinishSkill( -1, fCurrentTime, pkPlayer->GetCurrentLocation().x, pkPlayer->GetCurrentLocation().y );
			pkPlayer->RollNextSkill();
		}
		return SPT_SUSPEND;
	}
	UT_Vec2Float kMoveDirXY;
	kMoveDirXY.x = Int2Float( pTarget->GetCurrentLocation().x - pkPlayer->GetCurrentLocation().x );
	kMoveDirXY.y = Int2Float( pTarget->GetCurrentLocation().y - pkPlayer->GetCurrentLocation().y );
	kMoveDirXY.Unitize();
	//int16_t nForwardAngle = UT_MathBase::GetZRotAngle(kMoveDirXY);
	int16_t nForwardAngle = UT_MathBase::GetZRotAngle(kMoveDirXY);

	int32_t nTargetX = pTarget->GetCurrentLocation().x;
	int32_t nTargetY = pTarget->GetCurrentLocation().y;
	int32_t nLockX = pkPlayer->GetCurrentLocation().x;
	int32_t nLockY = pkPlayer->GetCurrentLocation().y;
	int nRetCode = false;
	UT_Vec3Int rkTarget;
	nRetCode = pTarget->GetCurrentLocationWithZ(rkTarget);
	UT_Vec3Int rkLocked;
	nRetCode = pkPlayer->GetCurrentLocationWithZ(rkLocked);

	CF_SkillExecuteList::DataEntry* pkSkillData = pSkill->GetDataEntryPtr();
	if (pkSkillData == NULL)
		return SPT_SUSPEND;

	int nSkillDistance = pkSkillData->_lMaxExecuteDistance;
	if ( pkSkillData->_lDamageType == 0 && pkSkillData->_lOperationType == SOT_LOCATION )
		nSkillDistance = INFINIT_SKILL_DISTANCE;
#if defined SKILL_DISTANCE_DEBUG_MODE
	nSkillDistance = SKILL_DISTANCE_DEBUG_VAL;
#endif

	// Check distance again
	int32_t nCurDistance = UT_MathBase::LineLengthXYInt( pTarget->GetCurrentLocation() , pkPlayer->GetCurrentLocation());
	if (nCurDistance > nSkillDistance)
		return SPT_SUSPEND;

	int nOPType = pkSkillData->_lOperationType;
	int nCurState = pkPlayer->GetCurrentStateType();
	bool bCasting = false;
	//if (nCurState == nSkillId)
	//if (IS_SKILL_TYPE(nCurState))
	//{
	//	bCasting = true;
	//	return SPT_SUSPEND; 
	//}
	// Fill param
	switch ( nOPType )
	{
	case SOT_TARGET:
		break;
	case SOT_DIR:
		{
			if ((!bCasting || (pkSkillData->_bToMouseDirection)) && pkSkillData->_lDamageType != 0)
			{
				nLockX = nTargetX;
				nLockY = nTargetY;
				rkLocked = rkTarget;
				UT_Vec2Float kDir;
				kDir.x = Int2Float( nLockX - pkPlayer->GetCurrentLocation().x );
				kDir.y = Int2Float( nLockY - pkPlayer->GetCurrentLocation().y );
				kDir.Unitize();
				nForwardAngle = UT_MathBase::GetZRotAngle(kDir);
				pkPlayer->SetLockedTargetID(nTargetId);
			}
			if (nTargetId)
			{
				rkLocked = rkTarget;
			}
			rkTarget.x = 0;
			rkTarget.y = 0;
			rkTarget.z = 0;
		}
		break;
	case SOT_LOCATION:
		{
			if (pkSkillData->_lDamageType != 0)
			{
				nLockX = nTargetX;
				nLockY = nTargetY;
				rkLocked = rkTarget;
			}
		}
		break;
	default:
		return SPT_SUSPEND;
		break;
	}

	//if (!bCasting)
	//pkPlayer->StopMove();
	pkPlayer->SetForwardDirAngle(nForwardAngle);

	if (IS_SKILL_TYPE(nCurState))
	{
		bCasting = true;
		return SPT_SUSPEND; 
	}

	if (!bCasting)
	{
		pkPlayer->CastSkill( nSkillId, GET_PROCESS_TIME, 0, nTargetId, rkTarget, rkLocked, rkLocked, nForwardAngle );
		pkPlayer->ClearNextSkill();
	}
	return SPT_SUSPEND;
}

int SM_ServerBuddyController::_updateFight( StateMachine::SM_StateMachine* pkStateMachine, float fCurrentTime, float fDeltaTime, object_id_type nTargetId )
{
	LC_ServerPlayer* pkPlayer = static_cast<LC_ServerPlayer*>(pkStateMachine);	
	if(pkPlayer->GetDead() || pkPlayer->GetDying() || !pkPlayer->GetHatredList()->CheckEnabled()) 
		return false;

	//bool bResult = pkPlayer->CheckBuddyControllerUpdate_Fight(fCurrentTime, FIGHT_UPDATE_TIMER);
	//if (bResult == false)
	//	return false;

	//pkPlayer->SetBuddyControllerUpdate_Fight(fCurrentTime);

	LC_SkillAsset* pSkillAsset = pkPlayer->GetSkillAsset();
	if (pSkillAsset == NULL)
		return false;
	//技能列表
	LC_ActorSkillList* pkSkillList = pkPlayer->GetActorSkillList();
	//技能触发器
	LC_ActorSkillTrigger* pkSkillTrigger = pkPlayer->GetSkillTrigger();

	LC_ServerPlayer* pOwnerPlayer = pkPlayer->GetOwnerPlayer();
	if (pOwnerPlayer == NULL)
		return false;

	//object_id_type lSelectTarget = _selectTarget(pkStateMachine, fCurrentTime, fDeltaTime);
	////object_id_type lSelectTarget = _selectTarget_orig(pkStateMachine, fCurrentTime, fDeltaTime);
	//if (lSelectTarget == 0)
	//	return false;
	object_id_type lSelectTarget = pkPlayer->GetDefaultTargetID();//nTargetId;//pkPlayer->GetFollowTargetObjectId();
	if (!IS_PLAYER_CLASS_ID(lSelectTarget) && !IS_NPC_ID(lSelectTarget))
	{
		return false;
	}

	_updateCastSkill(pkStateMachine, fCurrentTime, fDeltaTime, lSelectTarget);
	return lSelectTarget;
}

void SM_ServerBuddyController::_updateFight_orig( StateMachine::SM_StateMachine* pkStateMachine, float fCurrentTime, float fDeltaTime )
{
	LC_ServerPlayer* pkPlayer = static_cast<LC_ServerPlayer*>(pkStateMachine);	
	if(pkPlayer->GetDead() || pkPlayer->GetDying()) return;

	//技能列表
	LC_ActorSkillList* pkSkillList = pkPlayer->GetActorSkillList();

	//技能触发器
	LC_ActorSkillTrigger* pkSkillTrigger = pkPlayer->GetSkillTrigger();

	//更新仇恨列表，选取目标
	object_id_type lSelectTarget = _selectTarget(pkStateMachine, fCurrentTime, fDeltaTime);
	//object_id_type lSelectTarget = _selectTarget_orig(pkStateMachine, fCurrentTime, fDeltaTime);
	if (!IS_PLAYER_CLASS_ID(lSelectTarget) && !IS_NPC_ID(lSelectTarget))
	{
		return;
	}

	//向技能列表中添加技能
	LC_SkillTriggerIndexList kSkillIDList = pkSkillTrigger->GetPendingSkillList(pkPlayer, fCurrentTime);
	
	LC_SkillTriggerIndexListIter iter;
	TRAV(kSkillIDList, iter)
	{
		int iTriggerIndex = *iter;
		LC_SkillTriggerEntry* pkTriggerEntry = pkSkillTrigger->GetTriggerByIndex(iTriggerIndex);
		bool isHandSkill = pkTriggerEntry->GetSkillTypeID() == SKILL_MAN_NORMAL_ATTACK_HAND || pkTriggerEntry->GetSkillTypeID() == SKILL_WOMAN_NORMAL_ATTACK_HAND;
		bool handAtk = pkPlayer->GetSkillFlag(SK_DISARM) ? isHandSkill : !isHandSkill;
			
		if (/*NULL != pkTriggerEntry && */handAtk)
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
object_id_type SM_ServerBuddyController::_selectTarget(StateMachine::SM_StateMachine* pkStateMachine, float fCurrentTime, float fDeltaTime)
{
	LC_ServerPlayer* pkPlayer = static_cast<LC_ServerPlayer*>(pkStateMachine);	
	if(pkPlayer->GetDead())
	{
		return 0;
	}

	if (!pkPlayer->GetHatredList()->CheckEnabled())
		return 0;

	LC_ServerPlayer* pCurrentPlayer = pkPlayer->GetCurrentActivateBuddy();
	if (pCurrentPlayer == NULL)
		return 0;

	object_id_type lSelfID = pkPlayer->GetID();

	LC_ActorHatredList* pkHatredList = pCurrentPlayer->GetHatredList();

	// for ai debug
	object_id_type lSelectTargetID = INVALID_ACTOR_ID;
	object_id_type nOrigTargetID = pkPlayer->GetDefaultTargetID();
	object_id_type nCurrentDefaultTarget = pCurrentPlayer->GetDefaultTargetID();
	if (nCurrentDefaultTarget != pCurrentPlayer->GetID())
		lSelectTargetID = nCurrentDefaultTarget;
	if (IS_PLAYER_CLASS_ID(lSelectTargetID) || IS_NPC_ID(lSelectTargetID))
	{
		pkPlayer->SetDefaultTargetID(lSelectTargetID);
		//return lSelectTargetID;
	}
	else
	{
		lSelectTargetID = pkPlayer->GetDefaultTargetID();
		//return lSelectTargetID;
	}
	if (GetGlobalSetting.BuddyAIDebugMode)
	{
		lSelectTargetID = pCurrentPlayer->GetID();
		pkPlayer->SetDefaultTargetID(lSelectTargetID);
	}
#if 1
	if (!IS_PLAYER_CLASS_ID(lSelectTargetID) && !IS_NPC_ID(lSelectTargetID))
	{
		LC_ActorHatredEntry* pkHatredEntry = NULL;
		pkHatredEntry = pkHatredList->GetSortHatredByIndex(0);
		if (NULL != pkHatredEntry)
		{
			lSelectTargetID = pkHatredEntry->GetTargetID();
			int64_t nValue = pkHatredEntry->GetHatredValue();
			if (IS_PLAYER_CLASS_ID(lSelectTargetID) || IS_NPC_ID(lSelectTargetID))
			{
				pkPlayer->SetDefaultTargetID(lSelectTargetID);
				goto Exit0;
				//return lSelectTargetID;
			}
		}
	}

#endif
Exit0:
	if ( nOrigTargetID !=  lSelectTargetID && nOrigTargetID != INVALID_ACTOR_ID && lSelectTargetID != INVALID_ACTOR_ID && pkPlayer->IsInBaseSkill() )
	{
		pkPlayer->TryInterruptSkill( -1, fCurrentTime, SKI_FORCE);
		pkPlayer->TryFinishSkill( -1, fCurrentTime, pkPlayer->GetCurrentLocation().x, pkPlayer->GetCurrentLocation().y );
		pkPlayer->RollNextSkill();
	}
	return lSelectTargetID;
}

object_id_type SM_ServerBuddyController::_selectTarget_orig(StateMachine::SM_StateMachine* pkStateMachine, float fCurrentTime, float fDeltaTime)
{
	LC_ServerPlayer* pkPlayer = static_cast<LC_ServerPlayer*>(pkStateMachine);	
	if(pkPlayer->GetDead())
	{
		return 0;
	}

	LC_ServerPlayer* pOwnerPlayer = pkPlayer->GetOwnerPlayer();
	if (pOwnerPlayer == NULL)
		return 0;

	object_id_type lSelfID = pkPlayer->GetID();

	//仇恨列表
	LC_ActorHatredList* pkHatredList = pkPlayer->GetHatredList();

	LC_ActorSkillList* pkSkillList = pkPlayer->GetActorSkillList();

	LC_WorldManagerBase* pkWorldManager = ENGINE_GET_WORLD_MANAGER();
	if (NULL == pkWorldManager)
	{
		return 0;
	}

	LC_FactionManager* pkFactionManager = LC_FactionManager::GetSingletonPtr();
	if (NULL == pkFactionManager)
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
					|| RE_SKILL_CHECK_VALID_TARGET!=LC_Helper::IsValidTarget((LC_ActorBase*)pkStateMachine,pkTarget))
				{
					pkHatredList->RemoveHatred(lTargetID);
					//清空技能列表中针对该目标的技能
					pkSkillList->RemoveSkillByTargetID(lTargetID);
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

				object_id_type lOldTargetID = pkPlayer->GetDefaultTargetID();
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
					pkPlayer->ClearPendingSkill();
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
	object_id_type lAdditionTargetID = pkPlayer->GetAdditionTargetID();
	if ( (IS_PLAYER_CLASS_ID(lAdditionTargetID) || IS_NPC_ID(lAdditionTargetID))
		&& fCurrentTime - pkPlayer->GetAdditionTargetBeginTime() < pkPlayer->GetAdditionTargetDurationTime() )
	{
		LC_ActorBase* pkAdditionTarget = (LC_ActorBase*)pkWorldManager->FindObject(lAdditionTargetID);
		if (NULL == pkAdditionTarget 
			|| pkAdditionTarget->GetDead()
			//|| LC_Helper::IsInSafeArea(pkAdditionTarget)
			|| pkAdditionTarget->GetSkillFlag(SK_INVISIBLE))
		{
			pkPlayer->SetAdditionTargetID(0);
			pkPlayer->SetAdditionTargetBeginTime(0.0f);
			pkPlayer->SetAdditionTargetDurationTime(0.0f);
		}

		if (0 != pkPlayer->GetAdditionTargetID())
		{
			return pkPlayer->GetAdditionTargetID();
		}
	}


	if (NULL != pkHatredEntry)
	{
		object_id_type lSelectTargetID = pkHatredEntry->GetTargetID();
		if (IS_PLAYER_CLASS_ID(lSelectTargetID) || IS_NPC_ID(lSelectTargetID))
		{
			pkPlayer->SetDefaultTargetID(lSelectTargetID);
			return lSelectTargetID;
		}
	}

	return 0;
}

void SM_ServerBuddyController::_checkSkill(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime)
{	
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkMachine;
	if ( pkPlayer->GetPendingSkillExecute() != NULL)
		return ;

	LC_ActorSkillList* pkSkillList = pkPlayer->GetActorSkillList();
	LC_ActorSkillTrigger* pkSkillTrigger = pkPlayer->GetSkillTrigger();
	LC_ActorHatredList* pkHatredList = pkPlayer->GetHatredList();

	LC_MapBase* pkMap = pkPlayer->GetMap();
	if (NULL == pkMap)
		return;

	//检查技能列表中的技能，直至找到一个有效的技能
	LC_ActorSkillEntry* pkSkillEntry = NULL;
	LC_SkillTriggerEntry* pkTriggerEntry = NULL;
	SK_SkillExecute* pkPendingSkill = pkPlayer->GetPendingSkillExecute();
	while ((pkSkillEntry = pkSkillList->GetFirstSkillEntry()) != NULL && pkSkillEntry->GetValid())
	{
		bool bCheckSuccess = true;

		//如果该技能还未挂起，判断该技能cd时间是否满足
		if (NULL == pkPendingSkill || pkPendingSkill->GetExecuteID() != pkSkillEntry->GetSkillTypeID())
		{
			pkTriggerEntry = pkSkillTrigger->GetTriggerByIndex(pkSkillEntry->GetTriggerIndex());
			if (NULL != pkTriggerEntry
				&& pkTriggerEntry->GetValid()
				&& fCurrentTime - pkPlayer->GetLastExecuteSkillTime() < pkTriggerEntry->GetCDTime())
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
					pkEvent->SetSrcActorID(pkPlayer->GetID());
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
				pkPlayer->ClearPendingSkill();
			}
			pkSkillList->RemoveFirstSkillEntry();
		}
	}

}
//------------------------------------------------------
void SM_ServerBuddyController::_pendingSkill(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime)
{
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkMachine;

	if ( pkPlayer->GetPendingSkillExecute() != NULL)
		return ;

	LC_ActorSkillList* pkSkillList = pkPlayer->GetActorSkillList();
	LC_ActorSkillTrigger* pkSkillTrigger = pkPlayer->GetSkillTrigger();

	LC_ActorSkillEntry* pkEntry = pkSkillList->GetRandomSkillEntry();
	if (pkEntry != NULL && pkEntry->GetValid())
	{
		//怪物被激活方式：1、被伤害；2、警戒范围内存在目标；3、技能列表中存在待释放技能
		if (!pkPlayer->GetActive())
		{
			int lCurrentTime = Float2Long(GET_PROCESS_TIME);
			pkPlayer->SetActive(true);						//被激活
			pkPlayer->SetActiveTime(lCurrentTime);			//激活时间
		}
		int lSkillID = pkEntry->GetSkillTypeID();
		SK_SkillExecute* pkPendingSkill = pkPlayer->GetPendingSkillExecute();
		//如果该技能已经挂起，则不反复挂起，避免重复设置挂起时间
		if (NULL != pkPendingSkill && pkPendingSkill->GetExecuteID() == pkEntry->GetSkillTypeID())
		{
			return;
		}

		bool bPendingSkillSuccess = true;

		ResultType iResult = pkPlayer->CheckPendingSkill(lSkillID);
		if (RE_SKILL_NONE_ERROR != iResult)
		{
			bPendingSkillSuccess = false;
		}
		if (bPendingSkillSuccess)
		{
			pkPlayer->SetLockedTargetID(pkEntry->GetLockedTargetID());
			ResultType iResult = pkPlayer->PendingSkill(pkEntry->GetSkillTypeID(),0);
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

int SM_ServerBuddyController::GetFollowLocation( StateMachine::SM_StateMachine* pkStateMachine, LC_ActorBase* pFollowTarget , Utility::UT_Vec3Int& rkResult )
{
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkStateMachine;
	if (pFollowTarget == NULL)
		return false;
	
	bool bRetCode = pFollowTarget->CalcAdvanceFollowLocation(pkPlayer, GetGlobalSetting.nStopFollowDistance, rkResult);
	if (bRetCode)
		return true;
	
	// cast skill to enemy
	return true;
}


int SM_ServerBuddyController::GetSkillLocation( StateMachine::SM_StateMachine* pkStateMachine, LC_ActorBase* pFollowTarget , Utility::UT_Vec3Int& rkResult, int32_t nSkillDistance )
{
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkStateMachine;
	if (pFollowTarget == NULL)
		return false;

	rkResult = UT_MathBase::MoveAlongInt(pFollowTarget->GetCurrentLocation(), pkPlayer->GetCurrentLocation(), nSkillDistance);
	// cast skill to enemy
	return true;
}

