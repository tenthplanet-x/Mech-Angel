

#include "SM_ServerPlayerState_CHASE.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "LC_FactionManager.h"
#include "PlayerStateAction.h"
#include "LC_SkillAssetStruct.h"
#include "LC_ServerWorldManager.h"
#include "SK_Factory.h"

#define			MIN_PLAYER_DISTANCE		50

using namespace StateMachine;
using namespace Utility;
using namespace GameLogic;
using namespace Skill;
using namespace CSVFile;

//------------------------------------------------------
SM_ServerPlayerState_CHASE::SM_ServerPlayerState_CHASE()
: SM_PlayerState_CHASE()
{
}
//------------------------------------------------------
SM_ServerPlayerState_CHASE::~SM_ServerPlayerState_CHASE()
{
}
//------------------------------------------------------
bool SM_ServerPlayerState_CHASE::CanTransite(SM_StateMachine* pkMachine,  SM_StateBase* pkToTransiteState)
{
	return true;
}
//------------------------------------------------------
StateChangeResultType SM_ServerPlayerState_CHASE::StateStart(SM_StateMachine* pkMachine,  float fCurrentTime,float fDeltaTime)
{
	SM_PlayerState_CHASE::StateStart(pkMachine,fCurrentTime,fDeltaTime);

	return SCRT_SUCCESS;
}
//------------------------------------------------------
StateProcessType SM_ServerPlayerState_CHASE::StateProcess(SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime )
{
	SM_PlayerState_CHASE::StateProcess(pkMachine,fCurrentTime,fDeltaTime);
	
	if (SPT_END == ProcessPlayerStateTestFollow(this,pkMachine,fCurrentTime,fDeltaTime,950))
		return SPT_END;

	//������
	int32_t nAppendixResult = 0;
	//if (SPT_END == _processFight(pkMachine,fCurrentTime,fDeltaTime, nAppendixResult))
	if (SPT_END == _processFightAsBuddy(pkMachine,fCurrentTime,fDeltaTime, nAppendixResult))
		return SPT_END;

	_keepChaseDistance(pkMachine, nAppendixResult);

	return SPT_SUSPEND;
}
//------------------------------------------------------
StateChangeResultType SM_ServerPlayerState_CHASE::StateEnd(SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime )
{
	return SCRT_SUCCESS;
}
//------------------------------------------------------
StateMachine::StateProcessType SM_ServerPlayerState_CHASE::_processFightAsBuddy(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime, int32_t & nAppendixResult)
{
	nAppendixResult = -1;
	LC_ServerPlayer* pkServerPlayer = static_cast<LC_ServerPlayer*>(pkMachine);
	float fPlayerTime = pkServerPlayer->GetLastExecuteSkillTime();
	float fDelta = fCurrentTime - fPlayerTime;
	if (pkServerPlayer->GetDead() || fDelta  < 1.0f || !pkServerPlayer->GetSoloDulePlay())
		return SPT_SUSPEND;
	
	LC_ActorHatredList* pkHatredList = pkServerPlayer->GetHatredList();

	LC_WorldManagerBase* pkWorldManager = ENGINE_GET_WORLD_MANAGER();
	if (NULL == pkWorldManager)
	{
		return SPT_SUSPEND;
	}
	object_id_type nTargetId = pkServerPlayer->GetDefaultTargetID();
	LC_ActorBase* pTarget = (LC_ActorBase*)(pkWorldManager->FindObject(nTargetId));
	if (pTarget == NULL)
		return SPT_SUSPEND;

	UT_Vec2Float kMoveDirXY;
	kMoveDirXY.x = Int2Float( pTarget->GetCurrentLocation().x - pkServerPlayer->GetCurrentLocation().x );
	kMoveDirXY.y = Int2Float( pTarget->GetCurrentLocation().y - pkServerPlayer->GetCurrentLocation().y );
	kMoveDirXY.Unitize();
	int16_t nForwardAngle = UT_MathBase::GetZRotAngle(kMoveDirXY);

	int32_t nTargetX = pTarget->GetCurrentLocation().x;
	int32_t nTargetY = pTarget->GetCurrentLocation().y;
	int32_t nLockX = pkServerPlayer->GetCurrentLocation().x;
	int32_t nLockY = pkServerPlayer->GetCurrentLocation().y;

	int nRetCode = false;
	UT_Vec3Int rkTarget;
	nRetCode = pTarget->GetCurrentLocationWithZ(rkTarget);
	UT_Vec3Int rkLocked;
	nRetCode = pkServerPlayer->GetCurrentLocationWithZ(rkLocked);

	float fLastCastTime = pkServerPlayer->GetLastCastSkillTime();
	pkServerPlayer->RollNextSkill();
	LC_SkillAssetEntry* pSkillEntry = pkServerPlayer->GetNextSkill();

	int nCurState = pkServerPlayer->GetCurrentStateType();
	bool bCasting = false;
	if ( IS_SKILL_TYPE(nCurState) )
	{
		bCasting = true;
		return SPT_SUSPEND;
	}
	if ( pSkillEntry == NULL)
	{
		if (pkHatredList->GetHatredCount() <= 0)
		{
			pkServerPlayer->SetNextState(ST_PLAYER_STAND);
		}
		return SPT_SUSPEND;
	}
	//nRetCode = pTarget->GetCurrentLocationWithZ(rkTarget);
	//nRetCode = pkPlayer->GetCurrentLocationWithZ(rkLocked);
	uint32_t nSkillId = pSkillEntry->GetTypeID();
	SK_SkillExecute* pSkill = SK_Factory::GetSingletonPtr()->RequestSkillExcute(nSkillId);
	if (pSkill == NULL)
		return SPT_SUSPEND;
	CF_SkillExecuteList::DataEntry* pkSkillData = pSkill->GetDataEntryPtr();
	if (pkSkillData == NULL)
		return SPT_SUSPEND;

	int32_t nSkillDistance = pSkill->GetProperty_MaxExecuteDistance();
	// Distance checking.

	if (SOT_LOCATION == pkSkillData->_lOperationType)
	{
		pkServerPlayer->SetLockedLocation(pTarget->GetCurrentLocation());
	}
	ResultType iResult = pkServerPlayer->CheckExecuteSkill(fCurrentTime, pSkill, 0);
	if (iResult != RE_SKILL_CAN_TAKE_PLACE)
	{
		if ( iResult == RE_SKILL_DISTANCE_FAR)
			nAppendixResult = nSkillDistance;
		return SPT_SUSPEND;
	}

	int32_t nCurDistance = UT_MathBase::LineLengthXYInt( pTarget->GetCurrentLocation() , pkServerPlayer->GetCurrentLocation());
	if (nCurDistance > nSkillDistance)
	{
		nAppendixResult = nSkillDistance;
		return SPT_SUSPEND;
	}

	int nOPType = pkSkillData->_lOperationType;
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
				kDir.x = Int2Float( nLockX - pkServerPlayer->GetCurrentLocation().x );
				kDir.y = Int2Float( nLockY - pkServerPlayer->GetCurrentLocation().y );
				kDir.Unitize();
				nForwardAngle = UT_MathBase::GetZRotAngle(kDir);
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

	pkServerPlayer->SetForwardDirAngle(nForwardAngle);
	pkServerPlayer->SetAIControlNotify(true);

	pkServerPlayer->CastSkill( nSkillId, GET_PROCESS_TIME, 0, nTargetId, rkTarget, rkLocked, rkLocked, nForwardAngle );
	pkServerPlayer->ClearNextSkill();
	return SPT_SUSPEND;
}

StateMachine::StateProcessType SM_ServerPlayerState_CHASE::_processFight( StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime, int32_t & nAppendixResult )
{
	LC_ServerPlayer* pkServerPlayer = static_cast<LC_ServerPlayer*>(pkMachine);
	float fPlayerTime = pkServerPlayer->GetLastExecuteSkillTime();
	float fDelta = fCurrentTime - fPlayerTime;
	if (pkServerPlayer->GetDead() || fDelta  < 1.0f)
		return SPT_SUSPEND;
	
	LC_ActorHatredList* pkHatredList = pkServerPlayer->GetHatredList();
	LC_ActorSkillList* pkSkillList = pkServerPlayer->GetActorSkillList();
	LC_ActorSkillTrigger* pkSkillTrigger = pkServerPlayer->GetSkillTrigger();

	/*
	*�������ɾ��SkillList�еļ���
	*1�����ܹ���ʧ��
	*2�����ܹ����ʱ
	*3�����ܹ�����ͷ�ʧ��
	*4�������ͷųɹ�
	*/


	//��鼼���б��еļ����Ƿ���Ч��ע���ⲿ�ּ��ܼ�鹤��Ӧ����Ŀ����˵Ĺ������ݻ���
	_checkSkill(pkMachine, fCurrentTime, fDeltaTime);

	//������
	_pendingSkill(pkMachine, fCurrentTime, fDeltaTime);

	LC_ActorSkillEntry* pkSkillEntry = pkSkillList->GetFirstSkillEntry();
	//SK_SkillExecute* pkPendingSkill = pkServerPlayer->GetPendingSkillExecute();
	// AsBuddy Logic
	pkServerPlayer->RollNextSkill();
	LC_SkillAssetEntry* pSkillEntry = pkServerPlayer->GetNextSkill();
	uint32_t nSkillId = pSkillEntry->GetTypeID();
	SK_SkillExecute* pkPendingSkill = SK_Factory::GetSingletonPtr()->RequestSkillExcute(nSkillId);
	if (pkPendingSkill == NULL)
		return SPT_SUSPEND;
	
	//�����ǰ�����ܣ��ض�����SkillList�еĵ�һ������
	//if (NULL != pkSkillEntry && pkSkillEntry->GetValid() && NULL != pkPendingSkill 
	//	&& pkSkillEntry->GetSkillTypeID() == pkPendingSkill->GetExecuteID())
	if (pkPendingSkill)
	{
		pkServerPlayer->AIFillExecuteSkillParam(pkPendingSkill);
		ResultType eResult = pkServerPlayer->CheckExecuteSkill(fCurrentTime,pkPendingSkill,0);
		switch (eResult)
		{
		case RE_SKILL_DISTANCE_FAR:
			{
				nAppendixResult = pkPendingSkill->GetProperty_MaxExecuteDistance();
				//�������������������ȴ�
				//eResult = pkServerPlayer->PrepareExecuteSkill(pkPendingSkill,0);
			}
			break;
		case RE_SKILL_CAN_TAKE_PLACE:
			{
				/*LC_MapBase* pkMapBase = pkServerPlayer->GetMap();


				object_id_type targetid =  pkServerPlayer->GetLockedTargetID();
				LC_ActorBase * pkTarget = (LC_ActorBase*)(pkMapBase->FindObject(targetid));
				if(NULL == pkTarget)
				{
					return SPT_END;
				}
				LC_Helper::FaceActorToLocation(pkServerPlayer,pkServerPlayer->GetCurrentLocation(),pkTarget->GetCurrentLocation());*/

				//pkServerPlayer->SetForwardDir(pkServerPlayer->GetTargetForwardDir());

				pkServerPlayer->ExecuteSkill(fCurrentTime,pkPendingSkill,0);
				pkServerPlayer->ClearPendingSkill();
				pkServerPlayer->SetTargetLocation(pkServerPlayer->GetCurrentLocation());

				//�ͷż��ܳɹ���ɾ�������б�����Ӧ��
				//pkSkillTrigger->SetExecuteSkillTime(pkSkillEntry->GetTriggerIndex(), fCurrentTime);
				//pkServerPlayer->SetLastExecuteSkillTime(fCurrentTime);
				//pkSkillList->RemoveFirstSkillEntry();
				//pkServerPlayer->SetTargetLocation(pkServerPlayer->GetLockedLocation());

				return SPT_END;
			}
			break;
		default:
			{
				//����ִ�д���
				pkServerPlayer->ClearPendingSkill();
				pkSkillTrigger->SetExecuteSkillTime(pkSkillEntry->GetTriggerIndex(), fCurrentTime);
				pkServerPlayer->SetLastExecuteSkillTime(fCurrentTime);
				pkSkillList->RemoveFirstSkillEntry();

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

		if (pkHatredList->GetHatredCount() <= 0)
		{
			pkServerPlayer->SetNextState(ST_PLAYER_STAND);
		}
	}

	return SPT_SUSPEND;
}
//------------------------------------------------------
void SM_ServerPlayerState_CHASE::_keepChaseDistance( StateMachine::SM_StateMachine* pkMachine, int32_t nDistance )
{
	LC_ServerPlayer* pkPlayer = static_cast<LC_ServerPlayer*>(pkMachine);
	LC_FactionManager* pkFactionManager = LC_FactionManager::GetSingletonPtr();
	if (NULL == pkFactionManager)
		return;

	if ( nDistance <= 0 )
		return;
	nDistance *= 0.8;
	if (nDistance < MIN_PLAYER_DISTANCE)
		nDistance = MIN_PLAYER_DISTANCE;
	
	LC_MapBase* pkMap = pkPlayer->GetMap();
	if(pkMap)
	{
		object_id_type lTargetID = pkPlayer->GetLockedTargetID();
		LC_LogicObject* pkTarget = pkMap->FindObject(lTargetID);
		//������Ŀ���������꣬Player�Զ���������λ��
		if (NULL != pkTarget && (IS_PLAYER_CLASS_ID(pkTarget->GetID()) || IS_NPC_ID(pkTarget->GetID())))
		{
			UT_Vec3Int dis = pkPlayer->GetCurrentLocation() - pkTarget->GetCurrentLocation();
			UT_Vec2Float dt((float)dis.x, (float)dis.y);
			dt.Unitize();
			dt = dt * 250;
			UT_Vec3Int pp(pkTarget->GetCurrentLocation().x + (int)dt.x, pkTarget->GetCurrentLocation().y + (int)dt.y, pkTarget->GetCurrentLocation().z);
			UT_Vec3Int newSrc;
			newSrc = UT_MathBase::MoveAlongInt(pkTarget->GetCurrentLocation(), pkPlayer->GetCurrentLocation(), nDistance);
			pkPlayer->SetTargetLocation(newSrc);
			//pkPlayer->SetTargetLocation(pp);

			UT_Vec2Float rkDir;
			rkDir.x = Int2Float(pkTarget->GetCurrentLocation().x - pkPlayer->GetCurrentLocation().x);
			rkDir.y = Int2Float(pkTarget->GetCurrentLocation().y - pkPlayer->GetCurrentLocation().y);
			rkDir.Unitize();
			pkPlayer->SetForwardDir( rkDir );
			pkPlayer->SetAIControlNotify(true);
			//pkPlayer->SetLockedLocation(pkTarget->GetCurrentLocation());

// 		  if(pkPlayer->TryFightSurrounder(GET_PROCESS_TIME,(LC_ActorBase*)pkTarget))
// 		  {
// 				UT_Vec3Int kTargetLocation = ((LC_ActorBase*)pkTarget)->AddFightSurrounder(pkPlayer);
// 				pkPlayer->SetTargetLocation(kTargetLocation);
// 		  }
		}
	}
}
//------------------------------------------------------
void SM_ServerPlayerState_CHASE::_checkSkill(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime)
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

	//��鼼���б��еļ��ܣ�ֱ���ҵ�һ����Ч�ļ���
	LC_ActorSkillEntry* pkSkillEntry = NULL;
	LC_SkillTriggerEntry* pkTriggerEntry = NULL;
	SK_SkillExecute* pkPendingSkill = pkPlayer->GetPendingSkillExecute();
	while ((pkSkillEntry = pkSkillList->GetFirstSkillEntry()) != NULL && pkSkillEntry->GetValid())
	{
		bool bCheckSuccess = true;

		//����ü��ܻ�δ�����жϸü���cdʱ���Ƿ�����
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
			//��鼼���Ƿ�ʱ
			if (pkSkillEntry->GetStartTime() > 0.0f
				&& fCurrentTime - pkSkillEntry->GetStartTime() > pkSkillEntry->GetLifeTime())
			{
				bCheckSuccess = false;

				//�ͷż���ʧ���¼�
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
			//���Ŀ���Ƿ������ⲿ�ֹ�������Ӧ����Ŀ�����������
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
			//�ҵ���һ����Ч���ܣ����μ�����
			break;
		}
		else
		{
			//�����ǰ�Ѿ�����ü��ܣ���������
			if (NULL != pkPendingSkill && pkPendingSkill->GetExecuteID() == pkSkillEntry->GetSkillTypeID())
			{
				pkPlayer->ClearPendingSkill();
			}
			pkSkillList->RemoveFirstSkillEntry();
		}
	}
	
}
//------------------------------------------------------
void SM_ServerPlayerState_CHASE::_pendingSkill(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime)
{
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkMachine;
	
	if ( pkPlayer->GetPendingSkillExecute() != NULL)
		return ;

	LC_ActorSkillList* pkSkillList = pkPlayer->GetActorSkillList();

	LC_ActorSkillTrigger* pkSkillTrigger = pkPlayer->GetSkillTrigger();

	LC_ActorSkillEntry* pkEntry = pkSkillList->GetRandomSkillEntry();
	if (pkEntry != NULL && pkEntry->GetValid())
	{
		//���ﱻ���ʽ��1�����˺���2�����䷶Χ�ڴ���Ŀ�ꣻ3�������б��д��ڴ��ͷż���
		if (!pkPlayer->GetActive())
		{
			int lCurrentTime = Float2Long(GET_PROCESS_TIME);
			pkPlayer->SetActive(true);						//������
			pkPlayer->SetActiveTime(lCurrentTime);			//����ʱ��
		}
		int lSkillID = pkEntry->GetSkillTypeID();
		SK_SkillExecute* pkPendingSkill = pkPlayer->GetPendingSkillExecute();
		//����ü����Ѿ������򲻷������𣬱����ظ����ù���ʱ��
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
			//���ù�����ʱ��
			pkEntry->SetStartTime(GET_PROCESS_TIME);	
		}
		else
		{
			//���ü����ͷ�ʱ�䣬��ֹ������Ϊ�ͷŲ��ɹ�������ӵ������б���
			pkSkillTrigger->SetExecuteSkillTime(pkEntry->GetTriggerIndex(), fCurrentTime);
			pkSkillList->RemoveFirstSkillEntry();
		}
	}
}
//------------------------------------------------------

