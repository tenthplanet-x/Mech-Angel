#include "LC_WorldManagerBase.h"
#include "SM_ServerPlayerController.h"
#include "LC_LogicManagerBase.h"
#include "LC_ServerPlayer.h"
#include "LC_FactionManager.h"
#include "NW_Helper.h"
#include "LC_MapBase.h"
#include "SK_ServerSkillEventManager.h"
#include "../Buddy/BuddyInstance.h"

using namespace StateMachine;
using namespace Skill;
using namespace GameLogic;
using namespace Protocol;
using namespace Utility;
using namespace CSVFile;
//----------------------------------------------------------
SM_ServerPlayerController::SM_ServerPlayerController()
{

}
//----------------------------------------------------------
SM_ServerPlayerController::~SM_ServerPlayerController()
{

}
//----------------------------------------------------------
void SM_ServerPlayerController::Update( SM_StateMachine* pkStateMachine, float fCurrentTime, float fDeltaTime )
{
	LC_ServerPlayer* pkServerPlayer = static_cast<LC_ServerPlayer*>(pkStateMachine);
	//״̬����
	if (m_eUpdateMachineStateType != SUFT_REPLACE)
		_updateMachineState(pkStateMachine, fCurrentTime, fDeltaTime);

	if(pkServerPlayer->IsInstance() && !pkServerPlayer->GetAIControl())//Ϊ���˲���δ������
	//if (pkServerPlayer->GetControlType() == LC_ServerPlayer::eControlType_Player)
	{
		return;
	}
	//·������
	// 	if (m_eUpdateMachineStateType != SUFT_REPLACE)
	// 		_updateMovePath(pkStateMachine, fCurrentTime, fDeltaTime);
	// 	if (m_pkScriptUpdateMachineStateFunctor&&m_eUpdateMachineStateType != SUFT_DEFAULT)
	// 		(*m_pkScriptUpdateMachineStateFunctor)(pkActor, fCurrentTime, fDeltaTime);

	//�ƶ�����
	if (m_eUpdateTransformType != SUFT_REPLACE)
		_updateTransform(pkStateMachine, fCurrentTime, fDeltaTime);

	//ս������
	if (m_eUpdateFightType != SUFT_REPLACE)
		_updateFight(pkStateMachine, fCurrentTime, fDeltaTime);

}
//----------------------------------------------------------
void SM_ServerPlayerController::NotifySkillEvent(SM_StateMachine* pkStateMachine, Skill::SK_SkillEvent* pkSkillEvent)
{
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkStateMachine;
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
			kHatredEntry.SetTargetID(lSrcActorID);
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
			int lDmg = (int)pkSkillEvent->GetParam(SK_DamageEvent::SEP_DAMAGE_BASE);
			int nSkillId = pkSkillEvent->GetLogicID();
			if (SDT_DOT != lDamageType && pkPlayer->GetID() != (uint32_t)lHatredID)
			{
				//���ﲻ��Ҫ������ֵ��Ŀǰ��ҳ��ֻ���ڱ��ֽ���ս��״̬
				LC_ActorHatredEntry kHatredEntry;
				kHatredEntry.SetTargetID(lHatredID);
				kHatredEntry.SetHatredValue(1);
				kHatredEntry.SetUpdateTime(GET_PROCESS_TIME);
				pkHatredList->AddHatred(kHatredEntry);
				//object_id_type nOrigTargetID = pkPlayer->GetDefaultTargetID();
				//if ( nOrigTargetID != lDstActorID )
				//{
				//	printf( "check diff\n" );
				//}
				if ( lDstActorID !=  pkPlayer->GetID())
					pkPlayer->TrySetDefaultTargetID( lDstActorID );
			}
		}
		break;

	case SETE_ATTACH_STATE:
		{
			//�Լ����Լ����״̬����������
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
				//���debuff״̬
				if (2 == pkStateData->_lBuffType)
				{
					//���ﲻ��Ҫ������ֵ��Ŀǰ��ҳ��ֻ���ڱ��ֽ���ս��״̬
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
bool SM_ServerPlayerController::NotifyStateEnd( SM_StateMachine* pkStateMachine, int iEndState,float fCurrentTime,float fDeltaTime )
{
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkStateMachine;
	if(IS_SKILL_TYPE(iEndState) && !pkPlayer->IsAIControlStand() )
	{
		pkStateMachine->SetNextState(ST_PLAYER_CHASE);
	}

	return true;
}
//----------------------------------------------------------
void SM_ServerPlayerController::_updateMachineState(StateMachine::SM_StateMachine* pkStateMachine, float fCurrentTime, float fDeltaTime)
{
	LC_ServerPlayer* pkPlayer = static_cast<LC_ServerPlayer*>(pkStateMachine);

	if(pkPlayer->GetDying())
	{
		pkPlayer->SummorFriendTalk(2,fCurrentTime);
		pkPlayer->SetDead(true);
		return;
	}

	//ѣ�Ρ�˯�ߡ���Ĭ�����̳�Ĭ��Զ�̳�Ĭ����´�ϵ�ǰ����
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

	//���³��
	_updateHatred(pkStateMachine, fCurrentTime, fDeltaTime);
}
//----------------------------------------------------------
void SM_ServerPlayerController::_updateHatred(SM_StateMachine* pkStateMachine, float fCurrentTime, float fDeltaTime)
{
	LC_ServerPlayer* pkPlayer = static_cast<LC_ServerPlayer*>(pkStateMachine);	

	object_id_type lSelfID = pkPlayer->GetID();

	//����б�
	LC_ActorHatredList* pkHatredList = pkPlayer->GetHatredList();

	LC_WorldManagerBase* pkWorldManager = ENGINE_GET_WORLD_MANAGER();
	if (NULL == pkWorldManager)
	{
		return;
	}

	/*
	*���³���б�
	*/
	LC_ActorHatredEntry* pkHatredEntry = NULL;
	int iHatredCount = pkHatredList->GetHatredCount();
	//����б�ķ�������
	for (int i = 0; i < iHatredCount; i ++)
	{
		pkHatredEntry = pkHatredList->GetSortHatredByIndex(i);

		if (pkHatredEntry && pkHatredEntry->GetValid())
		{
			object_id_type lTargetID = pkHatredEntry->GetTargetID();
			if (IS_PLAYER_CLASS_ID(lTargetID) || IS_NPC_ID(lTargetID))
			{
				LC_ActorBase* pkTarget = (LC_ActorBase*)pkWorldManager->FindObject(lTargetID);
				//Ŀ����Ч
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
void SM_ServerPlayerController::_updateTransform(SM_StateMachine* pkStateMachine, float fCurrentTime, float fDeltaTime )
{
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(pkStateMachine);

	float fLastTranslateTime = pkPlayer->GetLastTranslateTime();
	float fDeltaTimeTemp = fCurrentTime - fLastTranslateTime;
	pkPlayer->SetLastTranslateTime(fCurrentTime);

	_translateAndRotateServerPlayer(pkStateMachine, fCurrentTime,fDeltaTimeTemp);
}

//----------------------------------------------------------
void SM_ServerPlayerController::_translateAndRotateServerPlayer( SM_StateMachine* pkStateMachine, float fCurrentTime,float fDeltaTime )
{
	LC_ServerPlayer* pkPlayer = static_cast<LC_ServerPlayer*>(pkStateMachine);

	if (	pkPlayer->GetDead() )
		pkPlayer->SetTargetLocation( pkPlayer->GetCurrentLocation());

	int32_t nState = pkPlayer->GetCurrentStateType();
	int	iMoveSpeed	= pkPlayer->GetMoveSpeed();
	//iMoveSpeed = CHAR_MOVE_SPEED;
	if(iMoveSpeed <= 0)
		return;

	//UT_Vec2Float kMoveDirXYTarget;
	//kMoveDirXYTarget.x = Int2Float( pkPlayer->GetTargetLocation().x - pkPlayer->GetCurrentLocation().x );
	//kMoveDirXYTarget.y = Int2Float( pkPlayer->GetTargetLocation().y - pkPlayer->GetCurrentLocation().y );
	//kMoveDirXYTarget.Unitize();
	//int16_t nForwardAngle = UT_MathBase::GetZRotAngle(kMoveDirXYTarget);
	//pkPlayer->SetForwardDirAngle(nForwardAngle);

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
		//����·��
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
		//����·���߲��ɹ���ֱ����
		rkMovePath.Clear();
		kCurrentLocation = pkPlayer->GetCurrentLocation();
		pkPlayer->SetCurrentLocation(kCurrentLocation);
		pkPlayer->SetMoveLineTargetLocation(kCurrentLocation);
	}
}
//----------------------------------------------------------
void SM_ServerPlayerController::_updateFight( StateMachine::SM_StateMachine* pkStateMachine, float fCurrentTime, float fDeltaTime )
{
	LC_ServerPlayer* pkPlayer = static_cast<LC_ServerPlayer*>(pkStateMachine);	
	if(pkPlayer->GetDead() || pkPlayer->GetDying())
		return;

	//�����б�
	LC_ActorSkillList* pkSkillList = pkPlayer->GetActorSkillList();

	//���ܴ�����
	LC_ActorSkillTrigger* pkSkillTrigger = pkPlayer->GetSkillTrigger();

	//���³���б�ѡȡĿ��
	object_id_type lSelectTarget = _selectTarget(pkStateMachine, fCurrentTime, fDeltaTime);
	if (!IS_PLAYER_CLASS_ID(lSelectTarget) && !IS_NPC_ID(lSelectTarget))
	{
		return;
	}
	LC_WorldManagerBase* pkWorldManager = ENGINE_GET_WORLD_MANAGER();
	if (NULL == pkWorldManager)
	{
		return;
	}

	pkPlayer->SetDefaultTargetID(lSelectTarget);
	//if ( pkPlayer->GetLockedTargetID() == INVALID_ACTOR_ID )
	if ( pkPlayer->GetLockedTargetID() != lSelectTarget )
		pkPlayer->SetLockedTargetID(lSelectTarget);

	LC_ActorBase* pTarget = (LC_ActorBase*)pkWorldManager->FindObject(lSelectTarget);
	if ( pTarget == NULL )
		return;


	int nCurState = pkPlayer->GetCurrentStateType();
	BuddyInstance* pBuddy = pkPlayer->GetAssignedBuddyInstance();
	if ( IS_SKILL_TYPE(nCurState) && pBuddy)
	{
		LC_SkillAssetEntry* pFirstSkillEntry = pBuddy->GetSkillAsset()->GetSkillAssetData()->GetSkillAssetEntry((skill_cat_type)SKILL_TYPE_SECT, 0);
		if ( pFirstSkillEntry && pFirstSkillEntry->GetTypeID() == nCurState )
		{
			// Rotate to target
			UT_Vec2Float kMoveDirXY;
			kMoveDirXY.x = Int2Float( pTarget->GetCurrentLocation().x - pkPlayer->GetCurrentLocation().x );
			kMoveDirXY.y = Int2Float( pTarget->GetCurrentLocation().y - pkPlayer->GetCurrentLocation().y );
			kMoveDirXY.Unitize();
			//int16_t nForwardAngle = UT_MathBase::GetZRotAngle(kMoveDirXY);
			int16_t nForwardAngle = UT_MathBase::GetZRotAngle(kMoveDirXY);
			pkPlayer->SetForwardDirAngle(nForwardAngle);
			pkPlayer->SetAIControlNotify(true);
		}
	}

	
	// ���´��������

	//�����б�����Ӽ���
	LC_SkillTriggerIndexList kSkillIDList = pkSkillTrigger->GetPendingSkillList(pkPlayer, fCurrentTime);

	//pkServerPlayer->RollNextSkill();
	//LC_SkillAssetEntry* pSkillEntry = pkServerPlayer->GetNextSkill();

	LC_SkillTriggerIndexListIter iter;
	TRAV(kSkillIDList, iter)
	{
		int iTriggerIndex = *iter;
		LC_SkillTriggerEntry* pkTriggerEntry = pkSkillTrigger->GetTriggerByIndex(iTriggerIndex);
		int nSkillId = pkTriggerEntry->GetSkillTypeID();
		bool isHandSkill = nSkillId == SKILL_MAN_NORMAL_ATTACK_HAND || nSkillId == SKILL_WOMAN_NORMAL_ATTACK_HAND;
		bool bSKDisarm = pkPlayer->GetSkillFlag(SK_DISARM);
		bool handAtk =  bSKDisarm ? isHandSkill : !isHandSkill;
			
		//if (NULL != pkTriggerEntry && handAtk)
		//if (NULL != pkTriggerEntry)
		{
			LC_ActorSkillEntry kSkillEntry;
			kSkillEntry.SetLockedTargetID(lSelectTarget);
			kSkillEntry.SetSkillTypeID(pkTriggerEntry->GetSkillTypeID());
			kSkillEntry.SetTriggerIndex(iTriggerIndex);
			kSkillEntry.SetStartTime(-1.0);	//���ü���û��������������ʱ���ù���ʱ��
			kSkillEntry.SetLifeTime(pkTriggerEntry->GetLifeTime());
			kSkillEntry.SetPriority(pkTriggerEntry->GetPriority());
			pkSkillList->AddSkillEntry(kSkillEntry);	
		}
	}
}
//----------------------------------------------------------
object_id_type SM_ServerPlayerController::_selectTarget(StateMachine::SM_StateMachine* pkStateMachine, float fCurrentTime, float fDeltaTime)
{
	LC_ServerPlayer* pkPlayer = static_cast<LC_ServerPlayer*>(pkStateMachine);	
	if(pkPlayer->GetDead())
		return 0;
	LC_WorldManagerBase* pkWorldManager = ENGINE_GET_WORLD_MANAGER();
	if (NULL == pkWorldManager)
		return 0;
	LC_FactionManager* pkFactionManager = LC_FactionManager::GetSingletonPtr();
	if (NULL == pkFactionManager)
		return 0;
	object_id_type lSelfID = pkPlayer->GetID();
	//����б�
	LC_ActorHatredList* pkHatredList = pkPlayer->GetHatredList();
	LC_ActorSkillList* pkSkillList = pkPlayer->GetActorSkillList();

	/*
	*���³���б�
	*/
	LC_ActorHatredEntry* pkHatredEntry = NULL;
	int iHatredCount = pkHatredList->GetHatredCount();
	bool bAllImmortal = true;
	//����б�ķ�������
	for (int i = 0; i < iHatredCount; i ++)
	{
		pkHatredEntry = pkHatredList->GetSortHatredByIndex(i);
		if (NULL == pkHatredEntry || pkHatredEntry->GetValid())
			continue;
		object_id_type lTargetID = pkHatredEntry->GetTargetID();

		if (	false == IS_PLAYER_CLASS_ID(lTargetID) 
			&&	false == IS_NPC_ID(lTargetID))
			continue;
		LC_ActorBase* pkTarget = (LC_ActorBase*)pkWorldManager->FindObject(lTargetID);
		//Ŀ����Ч
		if (NULL == pkTarget 
			|| lSelfID == lTargetID
			|| pkTarget->GetDead()
			//|| LC_Helper::IsInSafeArea(pkTarget)
			|| pkTarget->GetSkillFlag(SK_INVISIBLE)
			|| RE_SKILL_CHECK_VALID_TARGET!=LC_Helper::IsValidTarget((LC_ActorBase*)pkStateMachine,pkTarget))
		{
			pkHatredList->RemoveHatred(lTargetID);
			//��ռ����б�����Ը�Ŀ��ļ���
			pkSkillList->RemoveSkillByTargetID(lTargetID);
			continue;
		}
		//Ŀ���޵�
		if (pkTarget->GetSkillFlag(SK_IMMORTAL))
			continue;
		else
			bAllImmortal = false;
		object_id_type lOldTargetID = pkPlayer->GetDefaultTargetID();
		if (lOldTargetID != lTargetID)
		{
			//��Ŀ����û�г�������Ŀ���޵�1.2���������滻���ڵ�Ŀ��
			LC_ActorHatredEntry* pkOldHatredEntry = pkHatredList->GetHatred(lOldTargetID);
			if (NULL != pkOldHatredEntry 
				&& pkHatredEntry->GetHatredValue() < pkOldHatredEntry->GetHatredValue() * (1 + 0.2))
				continue;
			//ͨ��OT��ʽ�ҵ��µ�Ŀ�꣬������ռ��ܶ��У������������ԭ�ȵ�Ŀ��
			pkSkillList->Reset();
			pkPlayer->ClearPendingSkill();
		}
		//�ҵ��Ϸ�Ŀ��
		break;
	}
	//�������Ŀ����޵У���ѡ���һ��Ŀ��
	if (NULL == pkHatredEntry && bAllImmortal)
		pkHatredEntry = pkHatredList->GetSortHatredByIndex(0);


	/*
	*�����npcָ��������Ŀ�꣬���޶��в�������
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
