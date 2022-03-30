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
	//*�ж�npc�Ƿ��ڵȴ�cd���жϷ�����ÿһ�����ͷŵļ����Ƿ���������ͷ�
	//*������ڲ��������ͷŵļ��ܣ�npc��ʼ׷��Ŀ��
	//*/
	//bool bNPCWaitCD = false;
	//LC_ActorSkillList* pkSkillList = pkNPC->GetActorSkillList();
	//LC_ActorSkillTrigger* pkSkillTrigger = pkNPC->GetSkillTrigger();
	////��ǰ�����б��в����ڼ��ܣ����жϼ��ܴ������д��ͷŵļ���
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
	//				//����һ�������ͷŵļ��ܣ������жϽ�����npc��ʼ׷��Ŀ��
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
	//	//npcԭ�ش���
	//	UT_LinePath& rkMovePath = pkNPC->GetMovePath();
	//	rkMovePath.Clear();
	//	pkNPC->SetTargetLocation(pkNPC->GetCurrentLocation());
	//}

	//ֻ�н���npc����Ҫ����׷��Ŀ��㣬Զ��npc��PrepareExecuteSkillΪ������Ŀ���
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
	SK_SkillExecute* pkPendingSkill = pkServerNPC->GetPendingSkillExecute();
	//�����ǰ�����ܣ��ض�����SkillList�еĵ�һ������
	if (NULL != pkSkillEntry && pkSkillEntry->GetValid() && NULL != pkPendingSkill 
		&& pkSkillEntry->GetSkillTypeID() == pkPendingSkill->GetExecuteID())
	{
		pkServerNPC->AIFillExecuteSkillParam(pkPendingSkill);
		ResultType eResult = pkServerNPC->CheckExecuteSkill(fCurrentTime,pkPendingSkill,0);
		switch (eResult)
		{
			//����̫������ִ�м���
		case RE_SKILL_DISTANCE_NEAR:
			{
				pkServerNPC->ClearPendingSkill();

				//�ͷż��ܳɹ���ɾ�������б�����Ӧ��
				pkSkillTrigger->SetExecuteSkillTime(pkSkillEntry->GetTriggerIndex(), fCurrentTime);
				pkServerNPC->SetLastExecuteSkillTime(fCurrentTime);
				pkSkillList->RemoveFirstSkillEntry();

				return SPT_SUSPEND;
			}
			break;
		case RE_SKILL_DISTANCE_FAR:
			{
				//�������������������ȴ�
				//eResult = pkServerNPC->PrepareExecuteSkill(pkPendingSkill,0);
			}
			break;
		case RE_SKILL_CAN_TAKE_PLACE:
			{
				pkServerNPC->ExecuteSkill(fCurrentTime,pkPendingSkill,0);
				pkServerNPC->ClearPendingSkill();

				//�ͷż��ܳɹ���ɾ�������б�����Ӧ��
				pkSkillTrigger->SetExecuteSkillTime(pkSkillEntry->GetTriggerIndex(), fCurrentTime);
				pkServerNPC->SetLastExecuteSkillTime(fCurrentTime);
				pkSkillList->RemoveFirstSkillEntry();

				return SPT_END;
			}
			break;
		default:
			{
				//����ִ�д���
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

	//���Ŀǰ���뼤���̫Զ
	if(UT_MathBase::LineLengthXYInt(kActiveLocation, kCurrentLocation) > iChaseRadius)
	{
		//ת�볷��״̬
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
		//������Ŀ���������꣬npc�Զ���������λ��
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

	//��鼼���б��еļ��ܣ�ֱ���ҵ�һ����Ч�ļ���
	LC_ActorSkillEntry* pkSkillEntry = NULL;
	LC_SkillTriggerEntry* pkTriggerEntry = NULL;
	SK_SkillExecute* pkPendingSkill = pkNPC->GetPendingSkillExecute();
	while ((pkSkillEntry = pkSkillList->GetFirstSkillEntry()) != NULL && pkSkillEntry->GetValid())
	{
		bool bCheckSuccess = true;

		//����ü��ܻ�δ�����жϸü���cdʱ���Ƿ�����
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
		//���ﱻ���ʽ��1�����˺���2�����䷶Χ�ڴ���Ŀ�ꣻ3�������б��д��ڴ��ͷż���
		pkNPC->SaveActiveInfo();

		int lSkillID = pkEntry->GetSkillTypeID();
		SK_SkillExecute* pkPendingSkill = pkNPC->GetPendingSkillExecute();
		//����ü����Ѿ������򲻷������𣬱����ظ����ù���ʱ��
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

			//���жϹ������ҵľ��룬���������뾶�������ƶ����뾶������֮���Բ��õ�������Ϊ ����*���� ���ھ������⣬��һ����׼ȷ���ƶ����뾶��
			//���������ǽ���ˣ���������뾶�ж� �Ϳ�ʼ�Ӱ��˶� 
			//�߻������Ǵ��㻷���˶�������������clockwise,��������ĳ����Ӱڣ������û������ˣ�������̫�鷳
			if (abs(distance-innerRadius) > 10)
			{
				tgtLocation = enemyLocation + UT_MathBase::ConvertVec3FloatToInt(UT_MathBase::ConvertVec2To3Float( UT_Vec2Float( directionV2.x * innerRadius, directionV2.y * innerRadius)));
				if (!pathmanager->IsPosValid(npc->GetMapResID(), tgtLocation)) //��ֹ�ֱ�����ǽ�ϲ��ܶ�
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

				//������߼��� ��ѡ��ǣ�������о�ѡС�ǣ�����ٲ��У��ͷ�ת�ȴ�ǣ���С�� 
				//�����߼�Ϊ(�� ����):	0/2 = 0 0,		1/2 = 0 1,		2/2 = 1 0,		3/2 = 1 1
				//��С��ͬ��:				����ת��ǣ�	����תС�ǣ�	��ת��ǣ�	��תС��
				//��� 4�ζ�û��ѡ�������� �Ͳ���
				//�߻���TM�������µ����ת���Ƕ����ƣ������߼��Լ�����������
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
						if (zRotAngle < 1.0f)	//��ֹ�������ǶȺ� �Ͳ���ת��
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