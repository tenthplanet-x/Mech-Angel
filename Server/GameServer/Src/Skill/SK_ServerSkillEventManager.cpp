#include "GameServerApp.h"

#include "SK_ServerSkillEventManager.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerMap.h"
#include "LC_ServerNPC.h"
#include "LC_NPCGroupBase.h"
#include "LC_ServerDuelFlow.h"
#include "LC_ServerTaskManager.h"
#include "LC_ServerMapLogic_Place.h"
#include "LC_StoryLogic_Manager.h"
#include "SK_Factory.h"
#include "NW_Helper.h"
#include "UT_GameServerLogManager.h"
#include "LC_GameEventManager.h"
#include "oss_define.h"
#include "UT_ServerHelper.h"
#include "SC_ServerSkillInterface.h"
#include "SC_ServerScriptInterface.h"
#include "ACT_ActivitySchedule.h"
#include "CF_BossFirstKill.h"
#include "CF_CharType.h"
#include "MG_Game2Login.h"
#include "gsnode.h"

using namespace Skill;
using namespace GameLogic;
using namespace GameStory;
using namespace Utility;
using namespace Protocol;
using namespace CSVFile;
using namespace StateMachine;

//���������¼��б���ÿ�δ�����¼�����
#define SERVER_MAIN_SKILL_EVENT_PROCESS_COUNT 3000
#define MAX_SERVER_CHAIN_SKILL_EVENT_PROCESS_COUNT 3000
#define SKILL_EVENT_PRE_ALLOC_COUNT	5000

//------------------------------------------------------------
SK_ServerSkillEventManager::SK_ServerSkillEventManager(): m_iAllocEventID(0), m_nProcessingEvent(E_ProcessState_Init)
{
	m_kMainEventList.reserve(SKILL_EVENT_PRE_ALLOC_COUNT);
	m_kChainEventList.reserve(SKILL_EVENT_PRE_ALLOC_COUNT);
	m_kActiveBuddyBulletReqEvents.reserve(SKILL_EVENT_PRE_ALLOC_COUNT);

	const Utility::MemPoolConfigEntry & plc = GetGlobalSetting.poolSkillEvent;
	m_kEventPool.Init(plc.InitialCount, plc.IncCount, "SK_ServerSkillEventManager.m_kEventPool");
	m_kEventPool.EnableAutoDec(plc.AutoDec, plc.AutoDecInterval, plc.AutoDecCount, plc.AutoDecMinCount);
}
//------------------------------------------------------------
SK_ServerSkillEventManager::~SK_ServerSkillEventManager()
{
	this->_clearEventList(m_kMainEventList);
	this->_clearEventList(m_kChainEventList);
	this->_clearEventList(m_kActiveBuddyBulletReqEvents);
}
//------------------------------------------------------------
void SK_ServerSkillEventManager::Update(float fCurrentTime, float fDeltaTime)
{
	//���ڴ����¼�
	m_nProcessingEvent = E_ProcessState_Chain;
	//����һ�������������¼�
	_procEventList(fCurrentTime, m_kChainEventList, MAX_SERVER_CHAIN_SKILL_EVENT_PROCESS_COUNT);

	//��δ����������¼�ת�����¼��б���
	SK_SkillEventListIter iterCur = m_kChainEventList.begin();
	SK_SkillEventListIter iterEnd = m_kChainEventList.end();
	while (iterCur != iterEnd)
	{
		m_kMainEventList.push_back(*iterCur++);		// AddEvent, m_nProcessingEvent == E_ProcessState_Chain
	}
	//ע������¼��ظ�
	m_kChainEventList.clear();

	m_nProcessingEvent = E_ProcessState_Main;
	_procEventList(fCurrentTime, m_kMainEventList, SERVER_MAIN_SKILL_EVENT_PROCESS_COUNT);
	m_nProcessingEvent = E_ProcessState_Done;

	// ����ͻ����ӵ��¼�����
	//�� ÿ֡����_clearEventList�����и����⣬��Ϊǰ�������б��У��¼�����������������ƣ����п���ĳЩ�ӵ��¼���ǰ֡δ������Ȼ����Ӧ�Ŀͻ�������ʱ��ȴ�Ѿ��������Ӷ��ò�����ȷ����
	//*
	this->_updateActiveBuddyBulletReqEvent(fCurrentTime);
	/*/
	this->_clearEventList(m_kActiveBuddyBulletReqEvents);
	//*/

	//< ���¶����
	m_kEventPool.Update(fCurrentTime);
}
//------------------------------------------------------------
SK_SkillEvent* SK_ServerSkillEventManager::AllocEvent()
{
	SK_SkillEvent* pkEvent = m_kEventPool.Alloc();
	if (NULL == pkEvent)
	{
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "SK_ServerSkillEventManager AllocEvent Failed!");
		return NULL;
	}
	static const uint32_t nMaxSkillID = -1;
	m_iAllocEventID = (m_iAllocEventID + 1) % nMaxSkillID;			//�����¼�ID
	_resetEvent(pkEvent);
	pkEvent->SetEventID(m_iAllocEventID);
	pkEvent->SetStartTime(GET_PROCESS_TIME);						//�����¼���ʼʱ��

	return pkEvent;
}
//------------------------------------------------------------
void SK_ServerSkillEventManager::AddEvent(SK_SkillEvent* pkEvent)
{
	if (m_nProcessingEvent == E_ProcessState_Main)
	{
		m_kChainEventList.push_back(pkEvent);
	}
	else
	{
		m_kMainEventList.push_back(pkEvent);
	}
}
//------------------------------------------------------------
void SK_ServerSkillEventManager::ProcImmediateEvent(SK_SkillEvent* pkEvent)
{
	float fCurrentTime = GET_PROCESS_TIME;
	bool bDestroyEvent = true;

	int lEventType = pkEvent->GetEventType();
	switch (lEventType)
	{
		case SETE_KILL:
			{
				_procKillEvent(fCurrentTime, pkEvent, bDestroyEvent);
			}
			break;
	}
	m_kEventPool.Free(pkEvent);
}
//------------------------------------------------------------
void SK_ServerSkillEventManager::SendToClient(LC_ActorBase* pkActor, SK_SkillEvent* pkEvent, SendSkillEventType eSendType)
{
	if (NULL == pkActor || NULL == pkEvent)
	{
		return;
	}

	MG_PB<msg_define::skill_event> kSkillEventMsg;
	pkEvent->SetToProtocol(kSkillEventMsg.m_value);
	switch (eSendType)
	{
		case SSET_SEND:
			{
				//ֻ����player������Ϣ
				if (IS_PLAYER_CLASS_ID(pkActor->GetID()) )
				{
					//LC_ServerPlayer* pPlayer = (LC_ServerPlayer*)pkActor;
					//if ( !pPlayer->IsMainController() && pkEvent->GetEventType() == SETE_BULLET)
					//	return;
					 ((LC_ServerPlayer*)pkActor)->SendMsgToClient(MGPT_RLT_SKILL_EVENT, &kSkillEventMsg);
				}
			}
			break;
		case SSET_BROADCAST_TO_AREA:
			{
				LC_ServerMap* pkMap = (LC_ServerMap*)pkActor->GetMap();
				if (NULL != pkMap)
				{
					LC_ControllerOnlyPlayerFilter f;
					pkMap->BroadCastToArea(pkActor->GetSectorIndex(), MGPT_RLT_SKILL_EVENT, &kSkillEventMsg, &f);
				}
			}
			break;
		default:
			GfxWriteLog(LOG_SKILL_EVENT_FILELINE, LOG_SWITCH_SKILL_EVENT, "Wrong event sending type[%i]", eSendType);
			break;
	}
}

void SK_ServerSkillEventManager::SendMsgToClient(GameLogic::LC_ActorBase* pkActor, uint16_t usMsgType, Protocol::MG_BODY* pkMsg, SendSkillEventType eSendType)
{
	if (NULL == pkActor || NULL == pkMsg)
	{
		return;
	}

	switch (eSendType)
	{
	case SSET_SEND:
		{
			//ֻ����player������Ϣ
			if (IS_PLAYER_CLASS_ID(pkActor->GetID()) )
			{
				((LC_ServerPlayer*)pkActor)->SendMsgToClient(usMsgType, pkMsg);
			}
		}
		break;
	case SSET_BROADCAST_TO_AREA:
		{
			LC_ServerMap* pkMap = (LC_ServerMap*)pkActor->GetMap();
			if (NULL != pkMap)
			{
				LC_ControllerOnlyPlayerFilter f;
				pkMap->BroadCastToArea(pkActor->GetSectorIndex(), usMsgType, pkMsg, &f);
			}
		}
		break;
	default:
		GfxWriteLog(LOG_SKILL_EVENT_FILELINE, LOG_SWITCH_SKILL_EVENT, "Wrong event sending type[%i]", eSendType);
		break;
	}
}

StringType	SK_ServerSkillEventManager::GetRTInfo(void)
{
	StringStreamType sStream;
	sStream << "----[SK_ServerSkillEventManager Begin]----\n";
	sStream << m_kEventPool.GetRTInfo();
	sStream << "AllocEventID:[" << m_iAllocEventID << "]\n";
	sStream << "MainEventList:[" << m_kMainEventList.size() << "]\n";
	sStream << "ChainEventList:[" << m_kChainEventList.size() << "]\n";
	sStream << "----[SK_ServerSkillEventManager End]----\n";

	return sStream.str();
}
//------------------------------------------------------------
void SK_ServerSkillEventManager::_procEventList(float fCurrentTime, SK_SkillEventList& rkEventList, int nMaxSize)
{
	bool bDestroyEvent = true;
	int nInternalSize = rkEventList.size();
	if ( nInternalSize >= nMaxSize )
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "SK_ServerSkillEventManager::_procEventList internal: %d input: %d", nInternalSize, nMaxSize );

	SK_SkillEventListIter iterCur = rkEventList.begin();
	SK_SkillEventListIter iterEnd = rkEventList.end();
	while (iterCur != iterEnd && nMaxSize > 0)
	{
		nMaxSize--;
		SK_SkillEvent* pkEvent = *iterCur++;
		if(pkEvent != NULL)
		{
			bDestroyEvent = false;
			_procEvent(fCurrentTime, pkEvent, bDestroyEvent);
			if (bDestroyEvent)
			{
				//�����¼�Ϊ����״̬or�¼���ʱ���������¼�
				m_kEventPool.Free(pkEvent);
			}
			else if (fCurrentTime - pkEvent->GetStartTime() > SKILL_EVENT_UPDATE_DELTA_TIME)
			{
				GfxWriteFmtLog(LOG_SKILL_EVENT_FILELINE, LOG_SWITCH_SKILL_EVENT, "event type timeout!", pkEvent->GetEventID(), pkEvent->GetEventType(), pkEvent->GetSrcActorID(), pkEvent->GetDstActorID(), pkEvent->GetLogicID(), pkEvent->GetParam(0), pkEvent->GetParam(1));
				m_kEventPool.Free(pkEvent);
			}
			else
			{
				//�¼�δ�������ȴ��´δ���
				AddEvent(pkEvent);
			}
		}
	}

	// ���Ѵ���������������� iterCur ��end���൱��clear
	rkEventList.erase(rkEventList.begin(), iterCur);
}
//------------------------------------------------------------
void SK_ServerSkillEventManager::_procEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent)
{
	object_id_type lSrcActorID = pkEvent->GetSrcActorID();
	object_id_type lDstActorID = pkEvent->GetDstActorID();

	if (lSrcActorID > 0)
	{
		if (!IS_PLAYER_CLASS_ID(lSrcActorID) && !IS_NPC_ID(lSrcActorID))
		{
			bDestroyEvent = true;
			return;
		}
	}

	if (lDstActorID > 0)
	{
		if (!IS_PLAYER_CLASS_ID(lDstActorID) && !IS_NPC_ID(lDstActorID))
		{
			bDestroyEvent = true;
			return;
		}
	}

	//return;

	SkillEventTypeEx eEventType = pkEvent->GetEventType();
	switch (eEventType)
	{
		case SETE_COST:
			{
				_procCostEvent(fCurrentTime, pkEvent, bDestroyEvent);
			}
			break;
		case SETE_RECOVER:
			{
				_procRecoverEvent(fCurrentTime, pkEvent, bDestroyEvent);
			}
			break;
		case SETE_DAMAGE:
			{
				_procDamageEvent(fCurrentTime, pkEvent, bDestroyEvent);
			}
			break;
		case SETE_ALL_DAMAGE:
			{
				_procAllDamageEvent(fCurrentTime, pkEvent, bDestroyEvent);
			}
			break;
		case SETE_EXECUTE_SKILL:
			{
				_procExecuteSkillEvent(fCurrentTime, pkEvent, bDestroyEvent);
			}
			break;
		case SETE_INTERRUPT:
			{
				_procInterruptEvent(fCurrentTime, pkEvent, bDestroyEvent);
			}
			break;
		case SETE_ATTACH_STATE:
			{
				_procAttachStateEvent(fCurrentTime, pkEvent, bDestroyEvent);
			}
			break;
		case SETE_DETACH_STATE:
			{
				_procDetachStateEvent(fCurrentTime, pkEvent, bDestroyEvent);
			}
			break;
		case SETE_BULLET:
			{
				_procBulletEvent(fCurrentTime, pkEvent, bDestroyEvent);
			}
			break;
		case SETE_KILL:
			{
				_procKillEvent(fCurrentTime, pkEvent, bDestroyEvent);
			}
			break;
		case SETE_ENTER_GUARD:
			{
				_procEnterGuardEvent(fCurrentTime, pkEvent, bDestroyEvent);
			}
			break;
		case SETE_BROADCAST_ENEMY:
			{
				_procBroadcastEnemyEvent(fCurrentTime, pkEvent, bDestroyEvent);
			}
			break;
		case SETE_ATTACH_GFX:
			{
				_procAttachGfxEvent(fCurrentTime, pkEvent, bDestroyEvent);
			}
			break;
		case SETE_RETREAT:
			{
				_procRetreateEvent(fCurrentTime, pkEvent, bDestroyEvent);
			}
			break;
		case SETE_ATTACH_SOUND:
			{
				_procAttachSoundEvent(fCurrentTime, pkEvent, bDestroyEvent);
			}
			break;
		case SETE_DISPLAY_CHEST:
			{
				_procAttachDisplayChestEvent(fCurrentTime, pkEvent, bDestroyEvent);
			}
			break;
		case SETE_PLAYER_ENTER_MAP:
			{
				_procPlayerEnterMapEvent(fCurrentTime, pkEvent, bDestroyEvent);
			}
			break;
		case SETE_DETACH_GFX:
			{
				_procDetachGfxEvent(fCurrentTime, pkEvent, bDestroyEvent);
			}
			break;
		case SETE_CLIENT_EYE_SKILL:
			{
				_procClientEyeSkillEvent(fCurrentTime, pkEvent, bDestroyEvent);
			}
			break;
		case SETE_SUMMOR_DAMAGE:
			{
				_procSummorDamage(fCurrentTime, pkEvent, bDestroyEvent);
			}
			break;
		case SETE_IMM_PUNCH_MOVE:
			{
				_procImmMoveEvent(fCurrentTime, pkEvent, bDestroyEvent);
			}
			break;
		default:
			break;
	}
}
//------------------------------------------------------------
void SK_ServerSkillEventManager::_procCostEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent)
{
	object_id_type lSrcActorID = pkEvent->GetSrcActorID();
	object_id_type lDstActorID = pkEvent->GetDstActorID();
	int lCostType = (int)pkEvent->GetParam(SK_CostEvent::SEP_COST_TYPE);

	//�¼���������������
	bDestroyEvent = true;

	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ActorBase* pkSrcActor = (LC_ActorBase*)pkWorldManager->FindObject(lSrcActorID);
	LC_ActorBase* pkDstActor = (LC_ActorBase*)pkWorldManager->FindObject(lDstActorID);
	if (NULL == pkDstActor)
	{
		return;
	}

	//������Ʒ������Ҫ���к�������
	if (SK_CostEvent::SCT_ITEM == lCostType)
	{
		return;
	}

	_notifyState(pkSrcActor, pkDstActor, pkEvent);

	//��������������
	int64_t lCostBase = pkEvent->GetParam(SK_CostEvent::SEP_COST_BASE);
	int64_t lModifyValue = pkEvent->GetParam(SK_CostEvent::SEP_COST_MODIFY_VALUE);
	int64_t lModifyPercent = pkEvent->GetParam(SK_CostEvent::SEP_COST_MODIFY_PERCENT);
	double fModifyPercent = SK_CostEvent::ParamPerLL2Double(lModifyPercent);
	int64_t lFinalCost = Double2Int64(lCostBase * (1.0 + fModifyPercent)) + lModifyValue;
	pkEvent->SetParam(SK_CostEvent::SEP_FINAL_COST, lFinalCost);

	//���ûظ���
	switch (lCostType)
	{
		case SK_CostEvent::SCT_HP:
			{
				pkDstActor->SetHP(pkDstActor->GetHP() - lFinalCost);
			}
			break;
		case SK_CostEvent::SCT_MP:
			{
				pkDstActor->SetMP(pkDstActor->GetMP() - lFinalCost);
			}
			break;
		default:
			break;
	}
	_notifyController(pkSrcActor, pkDstActor, pkEvent);
}
//------------------------------------------------------------
void SK_ServerSkillEventManager::_procRecoverEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent)
{
	object_id_type lSrcActorID = pkEvent->GetSrcActorID();
	object_id_type lDstActorID = pkEvent->GetDstActorID();
	int lRecoverType = (int)pkEvent->GetParam(SK_RecoverEvent::SEP_RECOVER_TYPE);

	//�¼���������������
	bDestroyEvent = true;

	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ActorBase* pkSrcActor = (LC_ActorBase*)pkWorldManager->FindObject(lSrcActorID);
	LC_ActorBase* pkDstActor = (LC_ActorBase*)pkWorldManager->FindObject(lDstActorID);
	if (NULL == pkDstActor)
	{
		return;
	}
	LC_ActorAttributeMap* pkAttriMap = pkDstActor->GetFinalAttributeMap();
	if (NULL == pkAttriMap)
	{
		return;
	}

	_notifyState(pkSrcActor, pkDstActor, pkEvent);

	//����ڤ�� debuff
	double valuePercent = pkDstActor->GetStateMutableParameter(31006045,0);

	//�������ջظ���
	int64_t lRecoverBase = pkEvent->GetParam(SK_RecoverEvent::SEP_RECOVER_BASE);
	int64_t lModifyValue = pkEvent->GetParam(SK_RecoverEvent::SEP_RECOVER_MODIFY_VALUE);
	int64_t lModifyPercent = pkEvent->GetParam(SK_RecoverEvent::SEP_RECOVER_MODIFY_PERCENT);
	double fModifyPercent = SK_RecoverEvent::ParamPerLL2Double(lModifyPercent);
	int64_t lFinalRecover = Double2Int64((lRecoverBase * (1.0 + fModifyPercent) + lModifyValue)*(1-valuePercent));

	pkEvent->SetParam(SK_RecoverEvent::SEP_FINAL_RECOVER, lFinalRecover);

	//���ûظ���
	attr_value_type lMaxAttribute = 0;
	attr_value_type lCurrentAttribute = 0;
	switch (lRecoverType)
	{
		case SK_RecoverEvent::SRT_HP:
			{
				lMaxAttribute = pkAttriMap->GetAttribute(ATT_MAX_HP);
				lCurrentAttribute = pkDstActor->GetHP();


				if (lMaxAttribute > lCurrentAttribute)
				{
					pkDstActor->SetHP(pkDstActor->GetHP() + lFinalRecover);
				}
			}
			break;
		case SK_RecoverEvent::SRT_MP:
			{
				lMaxAttribute = pkAttriMap->GetAttribute(ATT_MAX_MP);
				lCurrentAttribute = pkDstActor->GetMP();
				if (lMaxAttribute > lCurrentAttribute)
				{
					pkDstActor->SetMP(pkDstActor->GetMP() + (int32_t)lFinalRecover);
				}
			}
			break;
		case SK_RecoverEvent::SRT_SP:
			{
				lMaxAttribute = 0x7fffffff;
				lCurrentAttribute = ((LC_ServerPlayer*)pkDstActor)->GetSkillLevelupPoints();
			}
			break;
		default:
			break;
	}
	//�Իָ�ֵ��������
	if (lMaxAttribute <= lCurrentAttribute)
	{
		lFinalRecover = 0;
	}
	else if (/*lMaxAttribute > lCurrentAttribute &&*/ lMaxAttribute < lCurrentAttribute + lFinalRecover)
	{
		lFinalRecover = lMaxAttribute - lCurrentAttribute;
	}
	pkEvent->SetParam(SK_RecoverEvent::SEP_FINAL_RECOVER, lFinalRecover);

	_notifyController(pkSrcActor, pkDstActor, pkEvent);

	if (lFinalRecover > 0)
	{
		SendToClient(pkDstActor, pkEvent, SSET_BROADCAST_TO_AREA);

		if(pkSrcActor!=NULL /*&& pkDstActor!=NULL*/)
		{
			if(IS_PLAYER_CLASS_ID(lSrcActorID))
			{
				int buddyID = ((LC_ServerPlayer*)pkSrcActor)->GetBuddyID();
				LC_ActorBase::CombatRecord* pkRecord = pkSrcActor->GetPlayerCombatDataMapByID(buddyID);
				if(NULL != pkRecord)
				{
					pkRecord->m_nID = buddyID;
					pkRecord->m_nHealData += lFinalRecover;
				}
			}

			//������Ϯ�����ֽ�����Ҫ��¼�Թ���ɵ��˺�
			bool tag = UT_ServerHelper::CheckRecordDestActorDamage(pkSrcActor, pkDstActor);
			if(tag && IS_NPC_ID(lDstActorID))
			{
				LC_ActorBase::CombatRecord* pkRecord = pkDstActor->GetMonstarCombatDataMapByID(lSrcActorID);
				if(NULL != pkRecord)
				{
					pkRecord->m_nID = pkSrcActor->GetCharType();
					pkRecord->m_nHealData += lFinalRecover;
				}
			}
		}
	}
}
//------------------------------------------------------------
void SK_ServerSkillEventManager::_procDamageEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent)
{
	object_id_type lSrcActorID = pkEvent->GetSrcActorID();
	object_id_type lDstActorID = pkEvent->GetDstActorID();
	object_id_type lLogicID = pkEvent->GetLogicID();
	int lDamageType = (int)pkEvent->GetParam(SK_DamageEvent::SEP_DAMAGE_TYPE);
	int lResult = (int)pkEvent->GetParam(SK_DamageEvent::SEP_RESULT);

	//�¼���������������
	bDestroyEvent = true;

	//�ж��˺����Ͳ���ȷ���¼�����
	if (lDamageType != SDT_NORMAL
			&& lDamageType != SDT_SKILL
			&& lDamageType != SDT_DOT
			&& lDamageType != SDT_REBOUND
			&& lDamageType != SDT_SHARE
			&& lDamageType != SDT_LC_SKILL)
	{
		return;
	}

	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ActorBase* pkSrcActor = (LC_ActorBase*)pkWorldManager->FindObject(lSrcActorID);
	LC_ActorBase* pkDstActor = (LC_ActorBase*)pkWorldManager->FindObject(lDstActorID);
	if(NULL==pkDstActor || NULL==pkSrcActor)
	{
		return;
	}

	if(pkDstActor->GetDead() || pkDstActor->GetDying())
	{
		return;
	}

	if(pkSrcActor->GetAttackFlag() == false)
	{
		return;
	}

	//�ж���� �Ƿ���Ч
	if (IS_PLAYER_CLASS_ID(lDstActorID) /*&& pkDstActor*/)
	{
		LC_ServerPlayer* pkDstPlayer = (LC_ServerPlayer*)pkDstActor;
		IF_A_NA_PLAYER(pkDstPlayer){ return; }
	}

	//�жϲ�ͬ��ͼ������˺�
	if (pkSrcActor && /*pkDstActor &&*/ pkSrcActor->GetMap()!=pkDstActor->GetMap() )
	{
		return;
	}

	//˫��������ң�������ʽPKģʽΪ������ģʽ��ȴ�����ı��������֤
	if (pkSrcActor && IS_PLAYER_CLASS_ID(lDstActorID) && IS_PLAYER_CLASS_ID(lSrcActorID) )
	{
		LC_FactionManager* pkFactionManager = LC_FactionManager::GetSingletonPtr();
		if (NULL != pkFactionManager)
		{
			Faction_Relation relation = pkFactionManager->GetEnemyRelation(pkSrcActor, pkDstActor);
			LC_ServerPlayer* pkSrcPlayer = (LC_ServerPlayer*)pkSrcActor;
			if (relation == Faction_PKMode && !pkSrcPlayer->GetPKRecorder()->CanPKTarget(pkDstActor) )
			{
				return ;
			}
		}
	}

	//�˺�û�����У��¼�����
	if (SK_DamageEvent::SDR_MISS == lResult && !pkDstActor->GetSkillFlag(SK_ATTACK_FLAG))
	{
		_notifyState(pkSrcActor, pkDstActor, pkEvent);
		_notifyController(pkSrcActor, pkDstActor, pkEvent);
		SendToClient(pkSrcActor, pkEvent, SSET_SEND);
		//������Ϣ�ظ�����
		if (lSrcActorID != lDstActorID)
		{
			SendToClient(pkDstActor, pkEvent, SSET_SEND);
		}
		return;
	}

	pkDstActor->SetAttackerID(lSrcActorID);
	pkDstActor->SetAttackerSkillTypeID(lLogicID);

	/*
	//�����ownerlist���ڹ���������ʱ����ά��
	if (IS_NPC_ID(lDstActorID))
	{
		//���Ŀǰû�й����������
		LC_ServerNPCOwnerList& rkOwnerList = ((LC_ServerNPC*)pkDstActor)->GetNPCOwnerList();
		rkOwnerList.AddOwnerID(lSrcActorID);
	}
	*/

	if(NULL!=pkSrcActor && IS_NPC_ID(lDstActorID) && IS_PLAYER_CLASS_ID(lSrcActorID))
	{
		LC_ServerPlayer* pkSrcOwnerPlayer = ((LC_ServerPlayer*)pkSrcActor)->GetOwnerPlayer();
		object_id_type HitPlayerID = ((LC_ServerNPC*)pkDstActor)->GetHitPlayerID();

		LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(pkServerPlayerManager->FindPlayer(HitPlayerID));

		uint32_t HitTime = ((LC_ServerNPC*)pkDstActor)->GetHitTime();
		uint32_t currentTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();

		//��һ������˺���ָ�ӹٳ���6��û�жԹ�������˺�  �����׻���
		//��һ������˺���ָ�ӹ��Ѿ�������  �����׻���
		if(NULL!=pkSrcOwnerPlayer && (HitPlayerID!=pkSrcOwnerPlayer->GetOwnerObjectID() && currentTime-HitTime>6) || NULL==pkPlayer)
		{
			object_id_type id = pkSrcOwnerPlayer->GetOwnerObjectID();
			((LC_ServerNPC*)pkDstActor)->SetHitPlayer(pkSrcOwnerPlayer->GetOwnerObjectID());
		}

		if(HitPlayerID == pkSrcOwnerPlayer->GetOwnerObjectID())
		{
			((LC_ServerNPC*)pkDstActor)->SetHitTime(currentTime);
		}
		//�������ϴι��������ʱ��

		((LC_ServerNPC*)pkDstActor)->AddDamagePlayer(pkSrcOwnerPlayer->GetOwnerObjectID());
	}

	//Ŀ�괦���޵�״̬���¼�����
	if (pkDstActor->GetSkillFlag(SK_IMMORTAL))										//�޵�
	{
		return;
	}

	if (pkSrcActor && IS_PLAYER_CLASS_ID(pkSrcActor->GetID()) && pkDstActor->GetSkillFlag(SK_IMMORTAL_FOR_PLAYER) )
	{
		return ;
	}

	if (SK_DamageEvent::SDR_JUMP_DODGE == lResult )									//����Ṧʱ��Ϊ�޵�
	{
		SendToClient(pkDstActor, pkEvent, SSET_SEND);
		return;
	}

	_notifyState(pkSrcActor, pkDstActor, pkEvent);

	/*
	*���������˺�
	*/
	int64_t lDamageBase = pkEvent->GetParam(SK_DamageEvent::SEP_DAMAGE_BASE);
	int64_t lModifyValue = pkEvent->GetParam(SK_DamageEvent::SEP_DAMAGE_MODIFY_VALUE);
	int64_t lNShield = pkEvent->GetParam(SK_DamageEvent::SEP_NEUTRALIZE_SHIELD);
	int64_t lModifyPercent = pkEvent->GetParam(SK_DamageEvent::SEP_DAMAGE_MODIFY_PERCENT);
	double fModifyPercent = SK_DamageEvent::ParamPerLL2Double(lModifyPercent);
	int64_t rrt = pkEvent->GetParam(SK_DamageEvent::SEP_RESULT);
	uint64_t lFinalDamage = 0;
	int64_t hpDamage = 0;
	//�����߼�
	if ( rrt == SK_DamageEvent::SDR_SHIELD)
	{
		lFinalDamage = Double2Int64(lDamageBase *  (1.0f + fModifyPercent)); 
		//�����˺��Ǵ��뻤��ֵ
		pkEvent->SetParam(SK_DamageEvent::SEP_FINAL_DAMAGE, lNShield-lModifyValue);
	}
	else
	{
		lFinalDamage = Double2Int64(lDamageBase *  (1.0f + fModifyPercent)) + lModifyValue;
		//�˺�����Ϊ�Ǹ�
		lFinalDamage = UT_MathBase::MaxInt64(0, lFinalDamage);

		//�˺�Ѫ������
		double hpProtect = SK_DamageEvent::ParamPerLL2Double(pkEvent->GetParam(SK_DamageEvent::SEP_DAMAGE_HP_PROTECTED));
		if ( hpProtect > 0 )
		{
			double val = pkDstActor->GetFinalAttributeMap()->GetAttribute(ATT_MAX_HP) * hpProtect;
			lFinalDamage = UT_MathBase::MinInt64((int64_t)val, lFinalDamage);
		}

		int64_t preFinalDamage = lFinalDamage;

		//�����˺�����
		double finDec = SK_DamageEvent::ParamPerLL2Double(pkEvent->GetParam(SK_DamageEvent::SEP_DAMAGE_FINAL_DMG_DE));
		if ( finDec > 0 )
		{
			finDec = UT_MathBase::MinDouble(finDec, 1.0f);
			lFinalDamage = UT_MathBase::MaxInt64(0,lFinalDamage - (int64_t)(lFinalDamage * finDec));
		}

		//�����˺��ӳ�
		double finAd = SK_DamageEvent::ParamPerLL2Double(pkEvent->GetParam(SK_DamageEvent::SEP_DAMAGE_FINAL_DMG_AD));
		if ( finAd > 0 )
		{
			finAd = UT_MathBase::MaxDouble(finAd, 0);
			lFinalDamage = UT_MathBase::MaxInt64(0, lFinalDamage + (int64_t)(lFinalDamage * finAd));
		}

		//�����˺��ظ�
		double finRecover = SK_DamageEvent::ParamPerLL2Double(pkEvent->GetParam(SK_DamageEvent::SEP_DAMAGE_FINAL_RECOVER));
		if ( finRecover > 0)
		{
			SC_ServerSkillInterface* pkSkillInterface =  SC_ServerScriptInterface::GetSingleton().GetSkillInterface();
			if ( pkSkillInterface )
			{
				pkSkillInterface->RecoverTargetHP(pkDstActor, TARGET_ME, lLogicID, Double2Int64(lFinalDamage*finRecover),0, lDstActorID);
			}
		}

		//�����˺��ظ�������ǰ��
		double finRecover2 = SK_DamageEvent::ParamPerLL2Double(pkEvent->GetParam(SK_DamageEvent::SEP_DAMAGE_FINAL_RECOVER_RE));
		if ( finRecover2 > 0)
		{
			SC_ServerSkillInterface* pkSkillInterface =  SC_ServerScriptInterface::GetSingleton().GetSkillInterface();
			if ( pkSkillInterface )
			{
				pkSkillInterface->RecoverTargetHP(pkDstActor, TARGET_ME, lLogicID, Double2Int64(preFinalDamage*finRecover2),0, lDstActorID);
			}
		}

		//�����˺�����
		int64_t rebDmg = 0;
		double finReb = SK_DamageEvent::ParamPerLL2Double(pkEvent->GetParam(SK_DamageEvent::SEP_DAMAGE_FINAL_REBOND));
		if ( finReb > 0 )
		{
			rebDmg = (int64_t)(UT_MathBase::MinInt64(pkDstActor->GetHP(), lFinalDamage) * finReb);
		}
		//�����˺���������ǰ
		double finRebRe = SK_DamageEvent::ParamPerLL2Double(pkEvent->GetParam(SK_DamageEvent::SEP_DAMAGE_FINAL_REBOND_RE));
		if ( finRebRe > 0 )
		{
			rebDmg += (int64_t)(UT_MathBase::MinInt64(pkDstActor->GetHP(), preFinalDamage) * finRebRe);
		}

		//���Է���
		int64_t redound =  (int64_t) pkDstActor->GetFinalAttributeMap()->GetAttribute(ATT_DMG_REBOUND);
		if ( (lDamageType == SDT_SKILL || lDamageType == SDT_NORMAL) && redound > 0 )
		{
			rebDmg +=  (int64_t)(UT_MathBase::MinInt64(pkDstActor->GetHP(), lFinalDamage) * (redound/10000.0));
		}

		//�����˺�����
		SC_ServerSkillInterface* pkSkillInterface =  SC_ServerScriptInterface::GetSingleton().GetSkillInterface();
		if ( pkSkillInterface && rebDmg > 0)
		{
			if ( pkSrcActor )
				pkSkillInterface->GenerateDamage(pkDstActor, pkSrcActor->GetID(), 0, SDT_REBOUND, rebDmg, pkDstActor->GetID());
		}

		//�����˺�����
		double finCombo = SK_DamageEvent::ParamPerLL2Double(pkEvent->GetParam(SK_DamageEvent::SEP_DAMAGE_FINAL_COMBO));
		if ( finCombo > 0 )
		{
			SC_ServerSkillInterface* pkSkillInterface =  SC_ServerScriptInterface::GetSingleton().GetSkillInterface();
			if ( pkSkillInterface && pkSrcActor)
			{
				pkSkillInterface->GenerateDamage(pkSrcActor,pkDstActor->GetID(), 0, SDT_SKILL, (int64_t)(lFinalDamage*finCombo),pkSrcActor->GetID());
			}
		}

		//HPѪ������
		int64_t abor = pkEvent->GetParam(SK_DamageEvent::SEP_DAMAGE_FINAL_AB_SHIELD);
		if ( abor > 0 )
		{
			hpDamage =	abor - lFinalDamage;
			pkEvent->SetParam(SK_DamageEvent::SEP_DAMAGE_FINAL_AB_SHIELD, hpDamage); 
			hpDamage = hpDamage >= 0 ? 0 : -hpDamage;
		}
		else
		{
			hpDamage = lFinalDamage;
		}

		//boss����ǿ�Ƴ���
		if ( IS_NPC_ID(pkDstActor->GetID()) )
		{
			LC_ServerNPC* pkNPC = (LC_ServerNPC*)pkDstActor;
			if ( pkNPC->GetSkillTrigger()->IsShield() )
			{
				attr_value_type maxHP = pkNPC->GetAttributeMap()->GetAttribute(ATT_MAX_HP);
				double rtHP = double(pkDstActor->GetHP() - hpDamage) / (double)maxHP;
				LC_SkillTriggerEntry* fd = pkNPC->GetSkillTrigger()->GetShieldActivate(rtHP);
				if ( fd )
				{
					attr_value_type hprt = attr_value_type(maxHP * (double)(fd->GetData1() - 0.01f));
					hpDamage = pkDstActor->GetHP() - hprt > 0 ? pkDstActor->GetHP() - hprt : 0;
					lFinalDamage = hpDamage;
				}
			}
		}

		//�˺����ܳ�������ʣ��Ѫ������
		pkEvent->SetParam(SK_DamageEvent::SEP_FINAL_DAMAGE, lFinalDamage);
	}
	/*
	*��ͻ��˷�����Ϣ
	*ע�⣺������HPǰ��Ӧ�÷����˺���Ϣ����ΪSetHP�в�����������Ϣ�Ǽ�ʱ��Ϣ���������˺���Ϣ����ͻ���
	*/
	SendToClient(pkSrcActor, pkEvent, SSET_SEND);
	//������Ϣ�ظ�����
	if (lSrcActorID != lDstActorID)
	{
		SendToClient(pkDstActor, pkEvent, SSET_SEND);
	}
	
	//�����������������Ϯ
	if(pkSrcActor!=NULL /*&& pkDstActor!=NULL*/)
	{
		if(IS_PLAYER_CLASS_ID(lSrcActorID))
		{
			int buddyID = ((LC_ServerPlayer*)pkSrcActor)->GetBuddyID();
			LC_ActorBase::CombatRecord* pkRecord = pkSrcActor->GetPlayerCombatDataMapByID(buddyID);
			if(NULL != pkRecord)
			{
				pkRecord->m_nID = buddyID;
				pkRecord->m_nDamageData += lFinalDamage;
			}
		}
		if(IS_PLAYER_CLASS_ID(lDstActorID))
		{
			int buddyID = ((LC_ServerPlayer*)pkDstActor)->GetBuddyID();
			LC_ActorBase::CombatRecord* pkRecord = pkDstActor->GetPlayerCombatDataMapByID(buddyID);
			if(NULL != pkRecord)
			{
				pkRecord->m_nID = buddyID;
				pkRecord->m_nHurtData += lFinalDamage;
			}
		}

		bool tag = UT_ServerHelper::CheckRecordDestActorDamage(pkSrcActor, pkDstActor);
		if(tag)
		{
			if(IS_NPC_ID(lSrcActorID))
			{
				LC_ActorBase::CombatRecord* pkRecord = pkDstActor->GetMonstarCombatDataMapByID(lSrcActorID);
				if(NULL != pkRecord)
				{
					pkRecord->m_nID = pkSrcActor->GetCharType();
					pkRecord->m_nDamageData += lFinalDamage;
				}
			}

			if(IS_NPC_ID(lDstActorID))
			{
				LC_ActorBase::CombatRecord* pkRecord = pkSrcActor->GetMonstarCombatDataMapByID(lDstActorID);
				if(NULL != pkRecord)
				{
					pkRecord->m_nID = pkDstActor->GetCharType();
					pkRecord->m_nHurtData += lFinalDamage;
				}
			}
		}
	}

	_settleState(pkSrcActor, pkDstActor, pkEvent);
	_notifyController(pkSrcActor, pkDstActor, pkEvent);

	int32_t iMapLogicID = (int32_t)pkEvent->GetParam(SK_DamageEvent::SEP_MAP_LOGIC_ID);

	/*
	*����HP
	*/
	attr_value_type iCurrentHP = pkDstActor->GetHP() - hpDamage;

	//buddy��ֵ��˺�ͳ�ƶ�����OwnerPlayer
	if(IS_PLAYER_CLASS_ID(lSrcActorID) && NULL!=pkSrcActor)
	{
		LC_ServerPlayer* pkOwnerPlayer = ((LC_ServerPlayer* )pkSrcActor)->GetOwnerPlayer();
		if(NULL == pkOwnerPlayer)
		{
			object_id_type nOwnerId =  ((LC_ServerPlayer* )pkSrcActor)->GetOwnerPlayerId();
			// log
			return;
		}

		// ����ս��ͳ�����  κ��
		if(!pkDstActor->GetOverDamage())
		{
			int64_t currentHP = pkDstActor->GetHP()>0 ? pkDstActor->GetHP() : 0;
			lFinalDamage = UT_MathBase::MinInt64(currentHP, lFinalDamage);
		}
	}

	if(IS_PLAYER_CLASS_ID(lSrcActorID) && IS_NPC_ID(lDstActorID) && NULL!=pkDstActor)
	{
		LC_ServerPlayer* pkOwnerPlayer = ((LC_ServerPlayer* )pkSrcActor)->GetOwnerPlayer();
		pkDstActor->RecordDamage(pkSrcActor, lFinalDamage);
		if(NULL != pkOwnerPlayer && pkDstActor->GetRecordDamageFlag())
		{
			pkOwnerPlayer->AddCombatRecorderValue(lFinalDamage);
		}
	}
	
	//���������Ҫ�������ɱ��
	LC_ActorBase* pOwner_Src = pkSrcActor;
	LC_ActorBase* pOwner_Dst = pkDstActor;
	if (/*NULL != pkDstActor &&*/ NULL != pkSrcActor)
	{
		LC_StoryLogic_Manager* pkStoryLogicManager = SERVER_GET_STORYLOGIC_MANAGER;
		if (pkStoryLogicManager)
		{
			if(IS_PLAYER_CLASS_ID(lSrcActorID))
			{
				pOwner_Src = ((LC_ServerPlayer*)pkSrcActor)->GetOwnerPlayer();
			}

			if(IS_PLAYER_CLASS_ID(lDstActorID))
			{
				pOwner_Dst = ((LC_ServerPlayer*)pkDstActor)->GetOwnerPlayer();
			}
			
			pkStoryLogicManager->ActorBeDamaged(pOwner_Src, pOwner_Dst, iMapLogicID, lFinalDamage);
		}
	}

	pkDstActor->SetHP(iCurrentHP);

	//����PKֵ
	LC_ServerMapLogic_Place* themaplogic = pkWorldManager->GetMapLogic(iMapLogicID);
	
	if(NULL != themaplogic && themaplogic->GetMapLogicType() == MT_GUILD_MAP)
	{
		if(iCurrentHP>=0 && lFinalDamage != hpDamage)
		{
			GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, " lFinalDamage!=hpDamage! mapID:%d, hpDamage:%llu, lFinalDamage:%d\n", themaplogic->GetMapResID(), hpDamage, lFinalDamage);
			//lFinalDamage = lFinalDamage/0;
		}
	}

	if (themaplogic && IS_PLAYER_CLASS_ID(lSrcActorID) && IS_PLAYER_CLASS_ID(lDstActorID))
	{
		LC_ServerPlayer* srcPlayer = (LC_ServerPlayer*)LC_Helper::GetRealActor(pkSrcActor);
		LC_ServerPlayer* dstPlayer = (LC_ServerPlayer*)LC_Helper::GetRealActor(pkDstActor);
		if (srcPlayer != NULL && dstPlayer != NULL)
		{
			if(themaplogic->GetRecordPK())
			{
				srcPlayer->GetPKRecorder()->AttackPlayer(pkDstActor, dstPlayer->GetPKRecorder());
			}
			//�����ͱ�������ʾ
			//int32_t nSrcSId = UT_ServerHelper::GetActiveId(srcPlayer);
			//int32_t nDstSId = UT_ServerHelper::GetActiveId(dstPlayer);
			int32_t nSrcSId = GSNodeNetwork::Instance().getEntryIdByMergeId(srcPlayer->GetCitizenship());
			int32_t nDstSId = GSNodeNetwork::Instance().getEntryIdByMergeId(dstPlayer->GetCitizenship());
			if(nSrcSId != nDstSId)
			{
				LC_ServerPlayer::SendNoticeOnPlayerAttack(LC_ServerPlayer::E_AttackType_Attack, themaplogic, srcPlayer, dstPlayer, nSrcSId, nDstSId);
			}
		}
	}

	/*
	*���ܴ�ϣ��������سǡ���˼��ܲ���DOT�˺����
	*/
	int iStateType = pkDstActor->GetCurrentStateType();
	if (IS_SKILL_TYPE(iStateType) && lFinalDamage > 0)
	{
		//�жϵ�ǰ�����Ƿ���Ա�dot���
		bool bCanBeInterruptByDot = true;
		//�������سǡ��ɼ������Ա�dot���
		if (iStateType == SKILL_MEDITATION
				|| iStateType == SKILL_RETURN_CITY
				|| iStateType == SKILL_COLLECT)
		{
			bCanBeInterruptByDot = false;
		}

		if (bCanBeInterruptByDot
				|| (lDamageType != SDT_DOT))
		{
			int iSkillStage = pkDstActor->GetSkillStage();
			SK_SkillExecute* pkSkillExecute = (SK_SkillExecute*)pkDstActor->GetCurrentState();
			if (pkSkillExecute && pkSkillExecute->GetProperty_SkillStageCanInterruptByDamage(iSkillStage))
			{
				SK_SkillEvent* pkInterruptEvent = AllocEvent();
				if (NULL != pkInterruptEvent)
				{
					pkInterruptEvent->SetEventType(SETE_INTERRUPT);
					pkInterruptEvent->SetSrcActorID(lSrcActorID);
					pkInterruptEvent->SetDstActorID(lDstActorID);
					pkInterruptEvent->SetLogicID(lLogicID);
					pkInterruptEvent->SetParam(SK_InterruptEvent::SEP_BE_INTERRUPT_SKILL_ID, iStateType);
					pkInterruptEvent->SetParam(SK_InterruptEvent::SEP_INTERRUPT_TYPE, SKI_DAMAGE);
					AddEvent(pkInterruptEvent);
				}
			}
		}
	}
}
//------------------------------------------------------------
void SK_ServerSkillEventManager::_procAllDamageEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent)
{
	object_id_type lSrcActorID = pkEvent->GetSrcActorID();
	int lDamageType = (int)pkEvent->GetParam(SK_DamageEvent::SEP_DAMAGE_TYPE);

	//�¼���������������
	bDestroyEvent = true;

	//�ж��˺����Ͳ���ȷ���¼�����
	if (lDamageType != SDT_NORMAL
			&& lDamageType != SDT_SKILL
			&& lDamageType != SDT_DOT
			&& lDamageType != SDT_REBOUND
			&& lDamageType != SDT_SHARE
			&& lDamageType != SDT_LC_SKILL)
	{
		return;
	}

	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ActorBase* pkSrcActor = (LC_ActorBase*)pkWorldManager->FindObject(lSrcActorID);

	_notifyState(pkSrcActor, NULL, pkEvent);
}
//------------------------------------------------------------
void SK_ServerSkillEventManager::_procExecuteSkillEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent)
{
	object_id_type lSrcActorID = pkEvent->GetSrcActorID();
	object_id_type lDstActorID = pkEvent->GetDstActorID();
	object_id_type lLogicID = pkEvent->GetLogicID();
	int lExecuteResult = (int)pkEvent->GetParam(SK_ExecuteSkillEvent::SEP_RESULT);

	//�¼���������������
	bDestroyEvent = true;

	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ActorBase* pkSrcActor = (LC_ActorBase*)pkWorldManager->FindObject(lSrcActorID);
	LC_ActorBase* pkDstActor = (LC_ActorBase*)pkWorldManager->FindObject(lDstActorID);

	_notifyState(pkSrcActor, pkDstActor, pkEvent);
	_notifyController(pkSrcActor, pkDstActor, pkEvent);

	if (lExecuteResult == SK_ExecuteSkillEvent::SER_SUCCESS)
	{
		if (IS_PLAYER_CLASS_ID(lSrcActorID) && pkSrcActor)
		{
			if (LC_Helper::isInArena(pkSrcActor))
			{

			}
			else
			{
				((LC_ServerPlayer*) pkSrcActor)->CostItemEndurance(false, lLogicID, true);
			}
		}

		if (IS_PLAYER_CLASS_ID(lDstActorID) && pkDstActor)
		{
			if (LC_Helper::isInArena(pkDstActor))
			{

			}
			else
			{
				((LC_ServerPlayer*) pkDstActor)->CostItemEndurance(false, lLogicID, true);
			}
		}
	}
}
//------------------------------------------------------------
void SK_ServerSkillEventManager::_procInterruptEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent)
{
	object_id_type lSrcActorID = pkEvent->GetSrcActorID();
	object_id_type lDstActorID = pkEvent->GetDstActorID();
	int lInterruptSkillID = (int)pkEvent->GetParam(SK_InterruptEvent::SEP_BE_INTERRUPT_SKILL_ID);
	int lInterruptType = (int)pkEvent->GetParam(SK_InterruptEvent::SEP_INTERRUPT_TYPE);
	if (lInterruptType <= SKI_NONE || lInterruptType >= SKI_MAX_COUNT)
	{
		return;
	}
	SkillInterruptType eInterruptType = (SkillInterruptType)lInterruptType;


	//�¼���������������
	bDestroyEvent = true;

	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ActorBase* pkSrcActor = (LC_ActorBase*)pkWorldManager->FindObject(lSrcActorID);
	LC_ActorBase* pkDstActor = (LC_ActorBase*)pkWorldManager->FindObject(lDstActorID);

	if (NULL == pkDstActor)
	{
		return;
	}


	int lCurrentStateType = pkDstActor->GetCurrentStateType();
	//ֻ����¼���ָ���ļ���
	if (lCurrentStateType == lInterruptSkillID && IS_SKILL_TYPE(lCurrentStateType))
	{
		int iSkillStage = pkDstActor->GetSkillStage();
		SK_SkillExecute* pkSkillExecute = (SK_SkillExecute*)pkDstActor->GetCurrentState();
		if (NULL != pkSkillExecute)
		{
			bool bCanInterrupt = false;

			//���ݴ�������ж��Ƿ���Ա����
			switch (lInterruptType)
			{
				case SKI_MOVE:
					{
						bCanInterrupt = pkSkillExecute->GetProperty_SkillStageCanInterruptByMove(iSkillStage);
					}
					break;
				case SKI_CHANGE_TARGET:
					{
						bCanInterrupt = pkSkillExecute->GetProperty_SkillStageCanInterruptByChangeTarget(iSkillStage);
					}
					break;
				case SKI_CHANGE_SKILL:
					{
						bCanInterrupt = pkSkillExecute->GetProperty_SkillStageCanInterruptByChangeSkill(iSkillStage);
					}
					break;
				case SKI_EVENT:
					{
						bCanInterrupt = pkSkillExecute->GetProperty_SkillStageCanInterruptByEvent(iSkillStage);
					}
					break;
				case SKI_DAMAGE:
					{
						bCanInterrupt = pkSkillExecute->GetProperty_SkillStageCanInterruptByDamage(iSkillStage);
					}
					break;
				case SKI_FORCE:
					{
						bCanInterrupt = true;
					}
					break;
				default:
					break;
			}

			if (bCanInterrupt)
			{
//#pragma TODO("Ŀǰʹ��InterruptSkill�����������Ϣ���Ƿ���Ҫ�ĳ���BeInterruptCurrentSkill��")
				pkDstActor->InterruptSkill(GET_PROCESS_TIME, pkDstActor->GetCurrentStateType(), eInterruptType);
			}
		}
	}

	_notifyState(pkSrcActor, pkDstActor, pkEvent);
	_notifyController(pkSrcActor, pkDstActor, pkEvent);
}
//------------------------------------------------------------
void SK_ServerSkillEventManager::_procAttachStateEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent)
{
	object_id_type lSrcActorID = pkEvent->GetSrcActorID();
	object_id_type lDstActorID = pkEvent->GetDstActorID();
	int lStateID = (int)pkEvent->GetParam(SK_AttachStateEvent::SEP_STATE_ID);

	//�¼���������������
	bDestroyEvent = true;

	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ActorBase* pkSrcActor = (LC_ActorBase*)pkWorldManager->FindObject(lSrcActorID);
	LC_ActorBase* pkDstActor = (LC_ActorBase*)pkWorldManager->FindObject(lDstActorID);

	CF_SkillStateList* pkStateList = CF_SkillStateList::GetSingletonPtr();
	CF_SkillStateList::DataEntry* pkStateData = pkStateList->GetEntryPtr(lStateID);
	if (NULL == pkStateData)
	{
		return;
	}

	if (!pkDstActor || !pkSrcActor)
	{
		return;
	}
	_notifyState(pkSrcActor, pkDstActor, pkEvent);
	_notifyController(pkSrcActor, pkDstActor, pkEvent);
	SendToClient(pkDstActor, pkEvent, SSET_BROADCAST_TO_AREA);
}
//------------------------------------------------------------
void SK_ServerSkillEventManager::_procDetachStateEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent)
{
	object_id_type lSrcActorID = pkEvent->GetSrcActorID();
	object_id_type lDstActorID = pkEvent->GetDstActorID();

	//�¼���������������
	bDestroyEvent = true;

	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ActorBase* pkSrcActor = (LC_ActorBase*)pkWorldManager->FindObject(lSrcActorID);
	LC_ActorBase* pkDstActor = (LC_ActorBase*)pkWorldManager->FindObject(lDstActorID);
	if (NULL == pkDstActor)
	{
		return;
	}

	if (pkEvent->GetParam(SK_DetachStateEvent::SEP_DETACH_TYPE) == SK_DetachStateEvent::SDSR_NOTIFY)
	{
		SendToClient(pkDstActor, pkEvent, SSET_SEND);
	}
	else
	{
		SendToClient(pkDstActor, pkEvent, SSET_BROADCAST_TO_AREA);
		_notifyState(pkSrcActor, pkDstActor, pkEvent);
		_notifyController(pkSrcActor, pkDstActor, pkEvent);
	}
}
//------------------------------------------------------------
static bool bullet_update_by_client(SK_SkillEvent * pkEvent)
{
	object_id_type lSrcActorID = pkEvent->GetSrcActorID();
	if (IS_NPC_ID(lSrcActorID))
		return false;

	int32_t  lLocalID = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_BULLET_LOCAL_ID);
	return (lLocalID != 0);
}

void SK_ServerSkillEventManager::_procBulletEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent)
{
	object_id_type lDstActorID = pkEvent->GetDstActorID();
	int lMoveType = (int)pkEvent->GetParam(SK_BulletEvent::SEP_MOVE_TYPE);

	//Ĭ�������¼�
	bDestroyEvent = true;

	// �ͻ��˸�����µ��ӵ�
	if (bullet_update_by_client(pkEvent))
	{
		// ��ʱ���ͻ���������ӵ�����������ȴ� N ��
		float fStartTime = SK_BulletEvent::ParamLLong2Float(pkEvent->GetParam(SK_BulletEvent::SEP_MOVE_START_TIME));
		float fMoveTime = SK_BulletEvent::ParamLLong2Float(pkEvent->GetParam(SK_BulletEvent::SEP_MOVE_LIFE_TIME));
		if (fCurrentTime - fStartTime > fMoveTime + 3.0f)
			return;

		// ��Ӧ�ͻ�������
		_procActiveBuddyBulletReqEvent(fCurrentTime, pkEvent, bDestroyEvent);
		return;
	}

	// ������������µ��ӵ�
	switch (lMoveType)
	{
		case BT_SHOOT:
		case BT_MISSILE:
		case BT_HOMINGMISSLE:
		case BT_TERMINAL_HOVER:
			{
				if (IS_PLAYER_CLASS_ID(lDstActorID) || IS_NPC_ID(lDstActorID))
				{
					_procBulletToTarget(fCurrentTime, pkEvent, bDestroyEvent);
				}
				else
				{
					_procBulletToLocation(fCurrentTime, pkEvent, bDestroyEvent);
				}
			}
			break;
		case BT_CHAIN:
			{
				_procBulletToTarget(fCurrentTime, pkEvent, bDestroyEvent);
			}
			break;
		case BT_CAST:
			{
				_procBulletToLocation(fCurrentTime, pkEvent, bDestroyEvent);
			}
			break;
		default:
			break;
	}
}

struct player_gain_exp
{
	LC_ServerPlayer* player;
	int64_t           exp;
};

class BossKillParterAchNotifyOP : public LC_EncountedIDOperator
{
public:
	BossKillParterAchNotifyOP(LC_MapBase* map, int32_t charTypeID)
		:pkMap(map)
	{
		kEvent.SetConditionInfo(charTypeID);
		kEvent.SetNum(1);		
	}

	virtual bool operator ()(object_id_type id)
	{
		if (!IS_PLAYER_ID(id))
		{
			return false;
		}

		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkMap->FindPlayer(id);
		if (NULL == pkPlayer || pkPlayer->CheckOwnerDead())
		{
			return false;
		}

		pkPlayer->UpdateAchievementRecord(LC_ACHIEVEMENT_TYPE_KILL_NPC, kEvent);
		return true;
	}

	LC_MapBase* pkMap;
	AchievementEvent kEvent;
};
//------------------------------------------------------------
void SK_ServerSkillEventManager::_procKillEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent)
{
	object_id_type lSrcActorID	= pkEvent->GetSrcActorID();
	object_id_type lDstActorID	= pkEvent->GetDstActorID();
	object_id_type lLogicID		= pkEvent->GetLogicID();

	//�����¼�
	bDestroyEvent = true;

	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ActorBase* pkSrcActor = (LC_ActorBase*)pkWorldManager->FindObject(lSrcActorID);
	
	if(NULL != pkSrcActor)
	{
		//�����ͷ�����buddy�г����Ĺ�����ͷ�����buddy
		if(IS_NPC_ID(lSrcActorID) && pkSrcActor->GetSummonOwnerID()!=0)
		{			
			lSrcActorID = pkSrcActor->GetSummonOwnerID();
			pkSrcActor = (LC_ActorBase*)pkWorldManager->FindObject(lSrcActorID);
		}
	}

	LC_ActorBase* pkDstActor = (LC_ActorBase*)pkWorldManager->FindObject(lDstActorID);
	if (NULL == pkDstActor)
	{
		GfxWriteLog(LOG_SKILL_EVENT_FILELINE, LOG_SWITCH_SKILL_EVENT, "killevent[%i]error��Recipient not found[%i]", pkEvent->GetEventID(), lDstActorID);
		return;
	}
	LC_MapBase* pkDstMap = pkDstActor->GetMap();
	if (NULL == pkDstMap)
	{
		return;
	}

	LC_GameEventManager* pkGameEventManager = LC_GameEventManager::GetSingletonPtr();

	LC_ServerMapLogic_Place* pkMapLogic = NULL;
	LC_ServerNPC* pkNPCDest = NULL;

	//���ɱ��orNPCɱ�֣����齱���͵��䱦��
	if (IS_NPC_ID(lDstActorID) /*&& NULL != pkDstActor*/)
	{
		pkNPCDest = (LC_ServerNPC*)pkDstActor;

		//��ȡ�׻�����ΪNPC��owner����npc���׻��߻����׻��߲����ߣ���ȡ�˺��������ΪNPC��owner
		object_id_type HitPlayerID = pkNPCDest->ConvertHitPlayerToOwnerList();
		LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(pkServerPlayerManager->FindPlayer(HitPlayerID));
		if(NULL == pkPlayer)
		{
			pkNPCDest->ConvertEncountToOwnerList();
		}

		//���齱���ͱ�������Ϊ��Ϊ�ɹ���������MapLogicͳһ����
		LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
		{
			pkMapLogic = pkWorldManager->GetMapLogic(pkNPCDest->GetMapLogicID());
			if (pkMapLogic && !pkMapLogic->GetAutoChangeMonsterHp())
			{
				pkNPCDest->GetNPCOwnerList().ResolveTotalLevel(pkNPCDest->GetMap());
			}
		}
		
		//buddy�����Ĺ֣�����ָ�ӹٴ����ģ�֪ͨGLET_KILL�¼�
		if (NULL!=pkSrcActor && IS_PLAYER_CLASS_ID(lSrcActorID))
		{
			lSrcActorID = pkSrcActor->GetOwnerObjectID();
		}

		//����ɱ���¼�
		LC_GameEvent* pkGameEvent = pkGameEventManager->AllocGameEvent();
		if (NULL != pkGameEvent)
		{
			//ע�⣬��Ȼ�˴�ɱ������ΪGLET_PLAYER_KILL_NPC����Ҳ�п�����NPCɱ��
			pkGameEvent->SetEventType(GLET_KILL);
			pkGameEvent->SetEventParam(EP_KILL_SRC_ID,		lSrcActorID);
			pkGameEvent->SetEventParam(EP_KILL_DST_CHAR_TYPE_ID,	pkDstActor->GetCharType());
			pkGameEvent->SetEventParam(EP_KILL_DST_ID, lDstActorID);
			pkGameEvent->SetEventParam(EP_KILL_SKILL_ID, lLogicID);
			pkGameEvent->SetEventParam(EP_KILL_ELITE_LVL, pkNPCDest->GetEliteLevel());
			pkGameEvent->SetEventParam(EP_KILL_MAP_ID,		pkDstMap->GetMapLogicID());
			pkGameEvent->SetEventParam(EP_KILL_POS_X,		pkDstActor->GetCurrentLocation().x);
			pkGameEvent->SetEventParam(EP_KILL_POS_Y,		pkDstActor->GetCurrentLocation().y);
			pkGameEvent->SetEventParam(EP_KILL_POS_Z,		pkDstActor->GetCurrentLocation().z);
			pkGameEvent->SetEventParam(EP_KILL_MAP_RES_ID,	pkDstActor->GetMapResID());
			pkGameEvent->SetEventParam(EVENT_EXTEND_PARAM_5,	pkDstActor->GetCharType());
			//һ������ֵ�ڸ����л��õ����벻Ҫɾ��
			pkGameEvent->SetEventParam(EVENT_EXTEND_PARAM_6,	(int)pkNPCDest->GetProperty_ExpRadio());
			pkGameEvent->SetEventParam(EVENT_EXTEND_PARAM_7,	pkNPCDest->GetLevel());
			pkGameEventManager->InsertGameEvent(pkGameEvent);
		}

		//������������Ϊ
		LC_NPCGroupBase* pkNPCGroup = pkNPCDest->GetNPCGroup();
		if (NULL != pkNPCGroup)
		{
			//������������+1
			pkNPCGroup->SetDeathCount(pkNPCGroup->GetDeathCount() + 1);
			//���ó���ʱ��
			LC_NPCGroupSpawnPoint* pkSpawnPoint = pkNPCGroup->GetSpawnPointByMemberID(pkNPCDest->GetGroupMemberID());
			if (NULL != pkSpawnPoint)
			{
				pkSpawnPoint->SetActiveTime(fCurrentTime);
			}
			//ɾ����������
			pkNPCGroup->RemoveByNPCID(pkNPCDest->GetID());
		}
	}
	else if (IS_PLAYER_CLASS_ID(lDstActorID))		//��ұ�ɱ���������ͷ����䱦��
	{
		//���齱���ͱ�������Ϊ��Ϊ�ɹ���������MapLogicͳһ����
		LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
		{
			pkMapLogic	=	pkWorldManager->GetMapLogic(pkDstActor->GetMapLogicID());
			if (pkMapLogic)
			{
				LC_ServerPlayer* pkTargetPlayer = (LC_ServerPlayer*)pkDstActor;
				pkMapLogic->PlayerBeKilled(pkTargetPlayer, lSrcActorID);
				if (pkTargetPlayer->IsClone() && !pkTargetPlayer->IsBuddy())
				{
					pkTargetPlayer->SummorFriendTalk(2, fCurrentTime);
				}
			}
		}
		

		//���������ͷ����ж��˺���Դ
		pkDstActor->SetAttackerID(lSrcActorID);

		//���ɱ�����
		if (IS_PLAYER_CLASS_ID(lSrcActorID))
		{
			//�׳�ɱ���¼�
			LC_GameEvent* pkGameEvent = pkGameEventManager->AllocGameEvent();
			if (NULL != pkGameEvent)
			{
				pkGameEvent->SetEventType(GLET_PLAYER_KILL_PLAYER);
				pkGameEvent->SetEventParam(EP_KILL_PLAYER_PLAYER_ID,	lSrcActorID);
				pkGameEvent->SetEventParam(EP_KILL_PLAYER_CHAR_TYPE_ID, pkDstActor->GetCharType());
				pkGameEvent->SetEventParam(EP_KILL_PLAYER_ID,			lDstActorID);
				pkGameEvent->SetEventParam(EP_KILL_PLAYER_SKILL_ID,		lLogicID);
				pkGameEvent->SetEventParam(EP_KILL_PLAYER_MAP_ID,		pkDstMap->GetMapLogicID());
				pkGameEvent->SetEventParam(EP_KILL_PLAYER_POS_X,		pkDstActor->GetCurrentLocation().x);
				pkGameEvent->SetEventParam(EP_KILL_PLAYER_POS_Y,		pkDstActor->GetCurrentLocation().y);
				pkGameEvent->SetEventParam(EP_KILL_PLAYER_POS_Z,		pkDstActor->GetCurrentLocation().z);
				pkGameEvent->SetEventParam(EVENT_EXTEND_PARAM_5,		pkDstActor->GetCharType());
				pkGameEventManager->InsertGameEvent(pkGameEvent);
			}

			if (pkSrcActor /*&& pkDstActor*/)
			{
				LC_ServerPlayer* pkSrcPlayer = (LC_ServerPlayer*)pkSrcActor;
				LC_ServerPlayer* pkDesPlayer = (LC_ServerPlayer*)pkDstActor;
				oss_role_killed_by_player(pkDesPlayer, pkSrcPlayer, 0);
			}
		}

		if (IS_NPC_ID(lSrcActorID) && pkSrcActor /*&& pkDstActor*/)
		{
			LC_ServerPlayer* pkDesPlayer = (LC_ServerPlayer*)pkDstActor;
			LC_ServerNPC* pkSrcNPC = (LC_ServerNPC*)pkSrcActor;
			oss_role_killed_by_monster(pkDesPlayer, pkSrcNPC);
		}
	}
	SendToClient(pkDstActor, pkEvent, SSET_BROADCAST_TO_AREA);
	_notifyState(pkSrcActor, pkDstActor, pkEvent);
	_notifyController(pkSrcActor, pkDstActor, pkEvent);
	bool drop = true;
	LC_ServerPlayer* pkFirstHitPlayer = NULL;
	if (pkMapLogic && pkNPCDest)
	{
		//���ɱ��npc���ж���ɱ
		if(IS_PLAYER_CLASS_ID(lSrcActorID))
		{
			object_id_type pkFirstHitPlayerID = pkNPCDest->GetHitPlayerID();
			pkFirstHitPlayer = (LC_ServerPlayer*)pkWorldManager->FindObject(pkFirstHitPlayerID);
			if(NULL == pkFirstHitPlayer && pkSrcActor)
			{
				//�Ҳ�����ɱ��ң������������һ����
				pkFirstHitPlayer = ((LC_ServerPlayer*)pkSrcActor)->GetOwnerPlayer();
			}

			if(NULL != pkFirstHitPlayer)
			{
				pkNPCDest->SetKillPlayerID(pkFirstHitPlayer->GetOwnerObjectID());

				CF_WorldMapList::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(pkNPCDest->GetMapResID());
				if(NULL!=pkData && pkData->_saveKillPlayerInfo)
				{
					UT_ServerHelper::UpdateKillPlayerInfo(pkNPCDest->GetMapResID(), pkNPCDest->GetCharType(), pkFirstHitPlayer);
				}

				int charType = pkNPCDest->GetCharType();
				CF_CharType::DataEntry* pkNpcData = pkNPCDest->GetCharTypePtr();
				if(NULL!=pkNpcData && pkNpcData->_bReduceDropType!=0)
				{
					//���������
					//pkFirstHitPlayer�е������ʱ���ŵ���
					int times = pkFirstHitPlayer->GetCurrentRecoverTimes(pkNpcData->_bReduceDropType);
					//2021.2.23 �����޸ģ�������غ�����Ҫ���������Ϊ0��ʾ����ʣ1��ʱ����ɱboss�۳���Ϊ0Ҳ�赯����ʾ
					if(times <= 1)
					{
						UT_ServerHelper::BroadCastRecoverTimesMsg(pkFirstHitPlayer, pkNpcData->_bReduceDropType);
					}

					if(times > 0)
					{
						pkFirstHitPlayer->ReduceCurrentRecoverTimes(pkNpcData->_bReduceDropType);
					}
					else
					{
						drop = false;
					}
				}

				//��ɱ����
				static Activity::ACT_ScheduleManager* pkScheduleMgr = Activity::ACT_ScheduleManager::GetSingletonPtr();
				Activity::ACT_ScheduleEntry* pkSchedule = pkScheduleMgr->GetSchedule(pkFirstHitPlayer->GetServerID(), GetGlobalSetting.BossFirstKillScheduleID);
				if(NULL != pkSchedule && pkScheduleMgr->IsScheduleValid(pkFirstHitPlayer->GetServerID(), GetGlobalSetting.BossFirstKillScheduleID))
				{
					int npcID = pkNPCDest->GetCharType();
					CF_BossFirstKill::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_BossFirstKill>(npcID);
					if(NULL != pkData)
					{
						pkFirstHitPlayer->AddKillBossRecord(npcID);

						//���һ�ɱ��
						std::string result;
						mem::vector<uint64_t> input;
						input.push_back(GetGlobalSetting.BossFirstKillScheduleID);
						input.push_back(npcID);
						bool tag = UT_ServerHelper::RedisGetData_String(UT_REDIS_KEY_BOSS_FIRST_KILL, input, result);
						if(tag)
						{
							BossFirstKillPlayerInfo info;
							if(result == "0")
							{
								//δ��ɱ��
								info.killerName =TPSTR2STDSTR(pkFirstHitPlayer->GetOwnerCharName());
								info.killerLevel = pkFirstHitPlayer->GetCommanderLevel();
								info.killerbornCharType = pkFirstHitPlayer->GetProfilePictureID();
								info.killerCommanderTitleID = pkFirstHitPlayer->GetCommanderTitle();
								info.KillerCommanderTitleFrameID = pkFirstHitPlayer->GetCurrentCommanderTitleFrameID();

								std::stringstream ss;
								boost::ajson::save_to_buff(info, ss);
								std::string data = ss.str().c_str();

								UT_ServerHelper::RedisSetData_String(UT_REDIS_KEY_BOSS_FIRST_KILL, input, data);

								//boss��ɱ��֪ͨ���н��
								MG_SS_NoticeBossFirstKillInfo syncMsg;
								syncMsg.m_nBossID = npcID;
								syncMsg.m_nKillerInfo.m_nPlayerName = info.killerName;
								syncMsg.m_nKillerInfo.m_nBornCharType = info.killerbornCharType;
								syncMsg.m_nKillerInfo.m_nCommanderTitleID = info.killerCommanderTitleID;
								syncMsg.m_nKillerInfo.m_nPlayerLevel = info.killerLevel;
								syncMsg.m_nKillerInfo.m_nCommanderTitleFrameID = info.KillerCommanderTitleFrameID;
								SendMsgToLogin(MGPT_SS_NOTICEBOSSFIRSTKILLINFO_LOGIN, &syncMsg);

								//����ȫ������
								CF_BossFirstKill::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_BossFirstKill>(npcID);
								CF_CharType::DataEntry* pkNPCData = SafeGetCSVFileDataEntryPtr<CF_CharType>(npcID);
								if(NULL!=pkData && NULL!=pkNPCData)
								{
									UT_ServerHelper::_GameNoticeFmort noticeFmort;
									noticeFmort.SetMsgID(40);
									noticeFmort.AddPlayerInfo(pkFirstHitPlayer->GetInstanceUniqueID(), pkFirstHitPlayer->GetOwnerCharName());
									noticeFmort.AddCharType(npcID);
									UT_ServerHelper::BrocastNoticeMsg(noticeFmort);					
								}
							}
						}
					}
				}
			}
		}
		
		if(NULL!=pkFirstHitPlayer)
		{
			CF_CharType::DataEntry* pkData = pkNPCDest->GetCharTypePtr();
			if(NULL != pkData)
			{
				std::string killName = TPSTR2STDSTR(pkFirstHitPlayer->GetOwnerCharName());
				
				if(drop)
				{
					UT_ServerHelper::SetKillBossInfo(pkFirstHitPlayer->GetOwnerCharName(), pkNPCDest->GetCharType(), pkNPCDest->GetMapResID());
					pkMapLogic->DropItemsOnNPCDead(pkNPCDest);
					UT_ServerHelper::ClearKillBossInfo();
				}

				if(pkData->_bIsShowReward)
				{
					MG_SyncDropInfo msg;
					msg.m_nReduceDropType = pkData->_bReduceDropType;
					msg.m_nDropVaild = drop;
					if(drop)
					{
						msg.m_nDropType = DROP_TYPE_KILLNPC;
						msg.m_nHitPlayerName = killName;
						pkFirstHitPlayer->GetDropItemInfo(pkNPCDest->GetCharType(), msg.m_nDropItems);
					}
					pkFirstHitPlayer->SendMsgToClient(MGPT_SYNCDROPINFO, &msg);
				}

				if(pkData->_bIsShowReward)
				{
					MG_SyncDropInfo msg;
					msg.m_nDropVaild = true;
					msg.m_nReduceDropType = pkData->_bReduceDropType;
					msg.m_nDropType = DROP_TYPE_PARTAKE_KILLNPC;
					msg.m_nHitPlayerName = killName;
					pkMapLogic->GetDropItemPartakeKillNpcReward(pkNPCDest, msg.m_nDropItems);

					Utility::UT_SIMDataList items;
					for(std::map<int, int>::iterator it=msg.m_nDropItems.begin(); it!=msg.m_nDropItems.end(); ++it)
					{
						UT_SIMDataInfo data;
						data.SetItemInfo(it->first, it->second, 0);
						items.push_back(data);
					}

					object_id_set idVec;
					pkNPCDest->GetDamageList(idVec);
					CF_CharType::DataEntry* pkNpcData = pkNPCDest->GetCharTypePtr();
					for (object_id_set::iterator it=idVec.begin(); it!=idVec.end(); ++it)
					{
						object_id_type player_id = *it;
						if(player_id == pkFirstHitPlayer->GetOwnerPlayerId())
						{
							continue;
						}

						LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkWorldManager->FindObject(player_id);
						if(NULL!=pkPlayer && NULL!=pkNpcData)
						{
							if(pkPlayer->GetCurrentRecoverTimes(pkNpcData->_bReduceDropType)<=0)
							{
								MG_SyncDropInfo vaildMsg;
								vaildMsg.m_nReduceDropType = pkData->_bReduceDropType;
								vaildMsg.m_nDropVaild = false;
								pkPlayer->SendMsgToClient(MGPT_SYNCDROPINFO, &vaildMsg);
							}
							else
							{
								int iDistance = UT_MathBase::LineLengthXYInt(pkNPCDest->GetCurrentLocation(),pkPlayer->GetCurrentLocation());
								if(iDistance < GetGlobalSetting.PickUpItemDistance)
								{
									pkPlayer->ReduceCurrentRecoverTimes(pkNpcData->_bReduceDropType);

									//���뱳��
									std::stringstream ss;
									ss << pkData->_iID;
									StringType mailTitle = "134;" + STDSTR2TPSTR(ss.str()) + ";";
									UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, items, pkPlayer, SYS_FUNC_TP_PARTAKE_KILLNPC, pkData->_iID, mailTitle, "135;", true);

									pkPlayer->SendMsgToClient(MGPT_SYNCDROPINFO, &msg);
								}
							}
						}
					}
				}
			}
		}

		mem::vector<player_gain_exp> gain_exps;

		object_id_vector&  ids = pkNPCDest->GetNPCOwnerList().GetOwnerIDVector();
		CF_CharType::DataEntry* pkCharData = pkNPCDest->GetCharTypePtr();

		LC_PlayerManagerBase* pkPlayerManager = ENGINE_GET_GAMEPLAYER_MANAGER();
		if (IS_PLAYER_ID(lSrcActorID))
		{
			object_id_type lOwnerID;
			for (int iIndex = 0; iIndex < (int)ids.size(); ++iIndex)
			{
				lOwnerID = ids[iIndex];
				LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(lOwnerID);
				if (pkPlayer != NULL)
				{
					if (pkNPCDest->GetEliteLevel() > 0)
					{
						pkPlayer->AddKillEliteCount();
					}

					player_gain_exp gain;
					gain.player = pkPlayer;
					gain.exp = pkPlayer->GetEXP();
					gain_exps.push_back(gain);
				}
			}

			if(pkCharData->_bBoss)
			{
				BossKillParterAchNotifyOP kAchNotifyOP(pkDstMap, pkNPCDest->GetCharType());
				pkNPCDest->GetEncountedStatMgr().GetStat(LC_ECT_TP_Damage)->ForAllEncounted(LC_ECT_CAT_ACTOR, kAchNotifyOP);
			}

			int32_t iStarLevel = pkCharData->_bBoss ? 0 : pkNPCDest->GetStarLevel();
			if (pkSrcActor && iStarLevel>=0)
			{
				LC_PlayerBase* pkPlayer = (LC_PlayerBase*)pkSrcActor;
				pkPlayer->GetSoulRefineAsset().OnKillMoster(pkNPCDest->GetCharType(), iStarLevel, 1);
			}
		}

		//pkMapLogic->NPCDead(pkNPCDest);
		if (IS_PLAYER_ID(lSrcActorID) && !gain_exps.empty())
		{
			for (mem::vector<player_gain_exp>::iterator i = gain_exps.begin(); i != gain_exps.end(); ++i)
			{
				player_gain_exp& gain = *i;
				oss_role_monster_die(gain.player, pkNPCDest, (int)(gain.player->GetEXP() - gain.exp));
			}
		}
	}

	//���ܻḴ�����  ���Ե�����ͬ����ɺ��� �����߼�
	//���������Ҫ�������ɱ��
	//IS_PLAYER_ID(lSrcActorID) && NULL != pkSrcActor &&
	if (/*NULL != pkDstActor &&*/ NULL != pkSrcActor)
	{
		LC_StoryLogic_Manager* pkStoryLogicManager = SERVER_GET_STORYLOGIC_MANAGER;
		if (pkStoryLogicManager)
		{
			if(IS_PLAYER_CLASS_ID(lSrcActorID))
			{
				pkSrcActor = ((LC_ServerPlayer*)pkSrcActor)->GetOwnerPlayer();
			}

			if(IS_PLAYER_CLASS_ID(lDstActorID))
			{
				pkDstActor = ((LC_ServerPlayer*)pkDstActor)->GetOwnerPlayer();
			}
			pkStoryLogicManager->ActorBeKilled(pkSrcActor, pkDstActor);
		}
	}

	if(pkMapLogic && pkNPCDest)
	{
		pkMapLogic->NPCDead(pkNPCDest);
	}
}
//------------------------------------------------------------
void SK_ServerSkillEventManager::_procEnterGuardEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent)
{
	object_id_type lSrcActorID = pkEvent->GetSrcActorID();
	object_id_type lDstActorID = pkEvent->GetDstActorID();

	//�¼���������������
	bDestroyEvent = true;

	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ActorBase* pkSrcActor = (LC_ActorBase*)pkWorldManager->FindObject(lSrcActorID);
	LC_ActorBase* pkDstActor = (LC_ActorBase*)pkWorldManager->FindObject(lDstActorID);

	if (!pkDstActor || !pkSrcActor)
	{
		return;
	}
	_notifyState(pkSrcActor, pkDstActor, pkEvent);
	_notifyController(pkSrcActor, pkDstActor, pkEvent);
}
//------------------------------------------------------------
void SK_ServerSkillEventManager::_procBroadcastEnemyEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent)
{
	object_id_type lSrcActorID = pkEvent->GetSrcActorID();
	object_id_type lDstActorID = pkEvent->GetDstActorID();

	//�¼���������������
	bDestroyEvent = true;

	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ActorBase* pkSrcActor = (LC_ActorBase*)pkWorldManager->FindObject(lSrcActorID);
	LC_ActorBase* pkDstActor = (LC_ActorBase*)pkWorldManager->FindObject(lDstActorID);

	if (!pkDstActor || !pkSrcActor)
	{
		return;
	}
	_notifyState(pkSrcActor, pkDstActor, pkEvent);
	_notifyController(pkSrcActor, pkDstActor, pkEvent);
}
//------------------------------------------------------------
void SK_ServerSkillEventManager::_procAttachGfxEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent)
{
	object_id_type lSrcActorID = pkEvent->GetSrcActorID();
	object_id_type lDstActorID = pkEvent->GetDstActorID();
	int lTargetLocationX = (int)pkEvent->GetParam(SK_AttachGfxEvent::SEP_TARGET_LOCATION_X);
	int lTargetLocationY = (int)pkEvent->GetParam(SK_AttachGfxEvent::SEP_TARGET_LOCATION_Y);

	//�¼���������������
	bDestroyEvent = true;

	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ActorBase* pkSrcActor = (LC_ActorBase*)pkWorldManager->FindObject(lSrcActorID);
	LC_ActorBase* pkDstActor = (LC_ActorBase*)pkWorldManager->FindObject(lDstActorID);

	if (NULL == pkDstActor && lTargetLocationX <= 0 && lTargetLocationY <= 0)
	{
		return;
	}

	if (NULL != pkDstActor)
	{
		SendToClient(pkDstActor, pkEvent, pkEvent->GetSendOrBroadCast());
	}
	else
	{
		SendToClient(pkSrcActor, pkEvent, pkEvent->GetSendOrBroadCast());
	}
}
//------------------------------------------------------------
void SK_ServerSkillEventManager::_procRetreateEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent)
{
	object_id_type lSrcActorID = pkEvent->GetSrcActorID();

	//�¼���������������
	bDestroyEvent = true;

	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ActorBase* pkSrcActor = (LC_ActorBase*)pkWorldManager->FindObject(lSrcActorID);

	if (NULL == pkSrcActor)
	{
		return;
	}

	if (IS_NPC_ID(pkSrcActor->GetID()))
	{
		LC_NPCGroupBase* pkGroup = ((LC_NPCBase*)pkSrcActor)->GetNPCGroup();
		//����س�
		if (NULL != pkGroup && pkGroup->GetGroupRetreate())
		{
			for (int i = 0; i < MAX_NPC_GROUP_MEMBER_LIST_SIZE; i ++)
			{
				LC_NPCGroupEntry* pkGroupEntry = pkGroup->GetGroupEntryByIndex(i);
				if (NULL != pkGroupEntry && pkGroupEntry->GetValid() && pkGroupEntry->GetNPC() != NULL)
				{
					pkGroupEntry->GetNPC()->NotifySkillEvent(pkEvent);
				}
			}
		}
	}

}
//------------------------------------------------------------
void SK_ServerSkillEventManager::_procAttachSoundEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent)
{
	//int lSrcActorID = pkEvent->GetSrcActorID();
	int lDstActorID = pkEvent->GetDstActorID();
	object_id_type lTargetLocationX = (object_id_type)pkEvent->GetParam(SK_AttachSoundEvent::SEP_TARGET_LOCATION_X);
	object_id_type lTargetLocationY = (object_id_type)pkEvent->GetParam(SK_AttachSoundEvent::SEP_TARGET_LOCATION_Y);
	int lBroadCastType = (int)pkEvent->GetParam(SK_AttachSoundEvent::SEP_SOUND_NAME_BROADCAST_TYPE);

	//�¼���������������
	bDestroyEvent = true;

	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ActorBase* pkDstActor = (LC_ActorBase*)pkWorldManager->FindObject(lDstActorID);

	if (NULL == pkDstActor && lTargetLocationX <= 0 && lTargetLocationY <= 0)
	{
		return;
	}

	if (NULL != pkDstActor && lBroadCastType == SK_AttachSoundEvent::SBT_BROADCAST_TO_SINGLE)
	{
		SendToClient(pkDstActor, pkEvent, SSET_SEND);
	}
	else if (NULL != pkDstActor && lBroadCastType == SK_AttachSoundEvent::SBT_BROADCAST_TO_AREA)
	{
		SendToClient(pkDstActor, pkEvent, SSET_BROADCAST_TO_AREA);
	}
}
//------------------------------------------------------------
void SK_ServerSkillEventManager::_procAttachDisplayChestEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent)
{
	//object_id_type lSrcActorID = pkEvent->GetSrcActorID();
	object_id_type lDstActorID = pkEvent->GetDstActorID();
	int lTargetLocationX = (int)pkEvent->GetParam(SK_AttachGfxEvent::SEP_TARGET_LOCATION_X);
	int lTargetLocationY = (int)pkEvent->GetParam(SK_AttachGfxEvent::SEP_TARGET_LOCATION_Y);

	//�¼���������������
	bDestroyEvent = true;

	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ActorBase* pkDstActor = (LC_ActorBase*)pkWorldManager->FindObject(lDstActorID);

	if (NULL == pkDstActor && lTargetLocationX <= 0 && lTargetLocationY <= 0)
	{
		return;
	}

	//����¼�ֻ�ܵ���
	if (NULL != pkDstActor)
	{
		SendToClient(pkDstActor, pkEvent, SSET_BROADCAST_TO_AREA);
	}
}
//------------------------------------------------------------
void SK_ServerSkillEventManager::_procPlayerEnterMapEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent)
{
	object_id_type lSrcActorID = pkEvent->GetSrcActorID();
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();

	//�¼���������������
	bDestroyEvent = true;

	//srcID��ʾ�����ͼ�����ID
	LC_ActorBase* pkSrcActor = (LC_ActorBase*)pkWorldManager->FindObject(lSrcActorID);
	if (NULL == pkSrcActor)
	{
		return;
	}
	//�ҳ�������ҵ�ͼ�е����й��֪ͨstate
	LC_ServerMapLogic_Place* pkMapLogic = (LC_ServerMapLogic_Place*)pkWorldManager->GetMapLogic(pkSrcActor->GetMapLogicID());
	if (pkMapLogic && LC_Helper::isTranscriptClass(pkMapLogic->GetMapLogicType()))
	{

		LC_ActorPtrList pkNPCList;
		mem::set<int> setA, setB;
		pkMapLogic->GetAllMonster(pkNPCList, setA, setB);
		for (LC_ActorPtrList::iterator iter = pkNPCList.begin(); iter != pkNPCList.end() ; iter++)
		{
			pkEvent->SetSrcActorID((*iter)->GetID());
			pkEvent->SetDstActorID((*iter)->GetID());
			//�����Ǽ����¼�����state���ܴ����������д�������ֵ
			pkEvent->SetLogicID(SKILL_TYPE_MIN + 1);
			pkEvent->SetParam(SK_PlayerEnterMapEvent::SEP_PLAYER_ID, lSrcActorID);
			_notifyState((*iter), NULL, pkEvent);
		}
	}
}
//------------------------------------------------------------
void SK_ServerSkillEventManager::_procDetachGfxEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent)
{
	//object_id_type lSrcActorID = pkEvent->GetSrcActorID();
	object_id_type lDstActorID = pkEvent->GetDstActorID();
	int broadCastType = (int)pkEvent->GetParam(SK_DetachGfxEvent::SEP_BROCAST_TYPE);
	StringType fxName;
	pkEvent->GetParam(SK_DetachGfxEvent::SEP_GFX_NAME_START, fxName);

	//�¼���������������
	bDestroyEvent = true;

	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ActorBase* pkDstActor = (LC_ActorBase*)pkWorldManager->FindObject(lDstActorID);

	if (NULL == pkDstActor || fxName.empty())
	{
		return;
	}
	SendToClient(pkDstActor, pkEvent, (Skill::SendSkillEventType)broadCastType);
}
//-----------------------------------------------------------
void SK_ServerSkillEventManager::_procClientEyeSkillEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent)
{
	//object_id_type lSrcActorID = pkEvent->GetSrcActorID();
	object_id_type lSrcActorID = pkEvent->GetSrcActorID();
	int skillid = (int)pkEvent->GetParam(SK_ClientEyeSkillEvent::SEP_SKILL_ID);

	//�¼���������������
	bDestroyEvent = true;

	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ActorBase* pkSrcActor = (LC_ActorBase*)pkWorldManager->FindObject(lSrcActorID);

	if (NULL == pkSrcActor || skillid <= 0)
	{
		return;
	}
	SendToClient(pkSrcActor, pkEvent, Skill::SSET_BROADCAST_TO_AREA);
}
//------------------------------------------------------------
void Skill::SK_ServerSkillEventManager::_procImmMoveEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent)
{
	bDestroyEvent = true;
	//object_id_type lSrcActorID = pkEvent->GetSrcActorID();
	object_id_type lSrcActorID = pkEvent->GetSrcActorID();
	object_id_type lDstActorID = pkEvent->GetDstActorID();

	//�¼���������������
	bDestroyEvent = true;

	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ActorBase* pkSrcActor = (LC_ActorBase*)pkWorldManager->FindObject(lSrcActorID);
	LC_ActorBase* pkDstActor = (LC_ActorBase*)pkWorldManager->FindObject(lDstActorID);

	if (NULL == pkSrcActor || NULL == pkDstActor)
	{
		return;
	}
	SendToClient(pkSrcActor, pkEvent, Skill::SSET_BROADCAST_TO_AREA);
	//SendToClient(pkDstActor, pkEvent, Skill::SSET_SEND);
}

void Skill::SK_ServerSkillEventManager::_clearEventList(SK_SkillEventList& rkEventList)
{
	SK_SkillEventListIter iterCur = rkEventList.begin();
	SK_SkillEventListIter iterEnd = rkEventList.end();
	while (iterCur != iterEnd)
	{
		SK_SkillEvent* pkEvent = *iterCur++;
		m_kEventPool.Free(pkEvent);
	}

	rkEventList.clear();
}

void Skill::SK_ServerSkillEventManager::_procSummorDamage( float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent )
{
	object_id_type lSrcActorID = pkEvent->GetSrcActorID();
	int skillid = (int32_t)pkEvent->GetParam(SK_SummorDamage::SEP_SKILL_ID);
	float duration = SK_SkillEvent::ParamLLong2Float(pkEvent->GetParam(SK_SummorDamage::SEP_DURATION));
	float startTime = SK_SkillEvent::ParamLLong2Float(pkEvent->GetParam(SK_SummorDamage::SEP_START_TIME));
	int tickCount = (int32_t)pkEvent->GetParam(SK_SummorDamage::SEP_TICK_COUNT);
	float tickTime = SK_SkillEvent::ParamLLong2Float(pkEvent->GetParam(SK_SummorDamage::SEP_TICK_TIME));

	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ActorBase* pkSrcActor = (LC_ActorBase*)pkWorldManager->FindObject(lSrcActorID);

	if (NULL == pkSrcActor)
	{
		return;
	}

	SK_Factory* pkSkillFactory = SK_Factory::GetSingletonPtr();
	SK_SkillExecute* pkSkillExecute = pkSkillFactory->RequestSkillExcute(skillid);
	for (int i = tickCount+1; startTime + i * tickTime < fCurrentTime ; i++) 
	{
		//ȡ�ͷŰ����ļ���
		if (NULL != pkSkillExecute)
		{
			pkSkillExecute->DelayUpdate(pkSrcActor);
		}
		pkEvent->SetParam(SK_SummorDamage::SEP_TICK_COUNT, i);

		//��ֹ��ѭ��
		if ( i > 10)
			break;
	}

	if ( duration + startTime < fCurrentTime )
	{
		//�¼���������������
		bDestroyEvent = true;
	}
}
//------------------------------------------------------------
void Skill::SK_ServerSkillEventManager::_settleState( GameLogic::LC_ActorBase* pkSrcActor, GameLogic::LC_ActorBase* pkDstActor, SK_SkillEvent* pkEvent )
{
	if (NULL != pkSrcActor)
	{
		LC_ActorStateMap* pkStateMap = pkSrcActor->GetStateMap();
		if (NULL != pkStateMap)
		{
			pkStateMap->SettleSkillEventToStateMap(pkEvent, false);
		}
	}
	if (NULL != pkDstActor)
	{
		LC_ActorStateMap* pkStateMap = pkDstActor->GetStateMap();
		if (NULL != pkStateMap)
		{
			pkStateMap->SettleSkillEventToStateMap(pkEvent, false);
		}
	}
}
//------------------------------------------------------------
void SK_ServerSkillEventManager::_procBulletToTarget(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent)
{
	/*
		_procBulletToTarget���ӵ�����Ŀ�꣬�����в��������κ������������ж����ӵ�����ʱ����������̻��߱�ը�¼�
		����Ǵ��̣���¼��ǰĿ�꣬������Χֻȡһ��Ŀ�꣬��������
	*/
	object_id_type lSrcActorID = pkEvent->GetSrcActorID();
	object_id_type lDstActorID = pkEvent->GetDstActorID();
	int lSkillID = pkEvent->GetLogicID();
	float fStartTime = SK_BulletEvent::ParamLLong2Float(pkEvent->GetParam(SK_BulletEvent::SEP_MOVE_START_TIME));
	float fMoveTime = SK_BulletEvent::ParamLLong2Float(pkEvent->GetParam(SK_BulletEvent::SEP_MOVE_LIFE_TIME));
	int lMoveSpeed = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_MOVE_SPEED);
	int lBulletFuncID = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_BULLET_FUNC);
	int lMaxPenetrateCount = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_MAX_PENETRATE_COUNT);
	int lPenetrateCount =(int32_t) pkEvent->GetParam(SK_BulletEvent::SEP_PENETRATE_COUNT);
	long lEventID = pkEvent->GetEventID();
	long lBulletLocalID = pkEvent->GetParam(SK_BulletEvent::SEP_BULLET_LOCAL_ID);

	//�����¼�
	bDestroyEvent = true;
	SK_Factory* pkSkillFactory = SK_Factory::GetSingletonPtr();
	SK_SkillExecute* pkSkillExecute = pkSkillFactory->RequestSkillExcute(lSkillID);
	if (NULL == pkSkillExecute)
		return;
	/// �ӵ�����
	const SK_SkillExecute::BulletParam & kBP = pkSkillExecute->GetProperty_BulletParam(lBulletFuncID);

	/*
	*�ӵ����η��н���,
	*1���������Ҫ��������Ŀ�꣬�Ҹ�Ŀ������ҵ������ӵ���͸��
	*2��������账������Ŀ�꣬or��һ��Ŀ���Ҳ��������ӵ���ը
	*/
	if (fCurrentTime - fStartTime > fMoveTime)
	{
		//�����ӵ��ͷ���
		LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
		LC_ActorBase* pkSrcActor = (LC_ActorBase*)pkWorldManager->FindObject(lSrcActorID);
		if (NULL == pkSrcActor)
			return;

		//���Ŀ���Ѿ����� ������һ�μ�¼�ĵص㱬ը
		UT_Vec3Int kTargetLocation;
		LC_ActorBase* pkCurrentTarget = (LC_ActorBase*)pkWorldManager->FindObject(lDstActorID);
		if (NULL == pkCurrentTarget)
		{
			kTargetLocation.x = pkEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_X);
			kTargetLocation.y = pkEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Y);
			kTargetLocation.z = pkEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Z);
		}
		else
		{
			kTargetLocation = pkCurrentTarget->GetCurrentLocation();
		}

		//��Ҫ��͸�µ�Ŀ��
		if (lPenetrateCount < lMaxPenetrateCount && NULL != pkCurrentTarget)
		{
			//��¼�������Ŀ�꣬_bulletLockTarget�������ų�����Ŀ��
			if (lPenetrateCount < 0 || lPenetrateCount >= SK_BulletEvent::MAX_BULLET_PENETRATE_COUNT)
				return;
			pkEvent->SetParam(SK_BulletEvent::SEP_PENETRATE_COUNT, lPenetrateCount + 1);
			pkEvent->SetParam(SK_BulletEvent::SEP_TARGET_BEGIN + lPenetrateCount, lDstActorID);
			//�κγ�ԽSEP_TARGET_END�Ĵ�͸Ŀ���趨���ǷǷ��ģ��ᵼ��Խ�縲�Ǻ�������SEP_SHOT_LOCATION_X��SEP_SHOT_LOCATION_Y������MAX_SKILL_EVENT_PARAM_COUNT��������

			int lActorFilterFuncID = pkSkillExecute->GetProperty_LockTargetType();
			int lBulletLockRange = kBP.m_lLockRange;
			int lBulletMaxLockCount = 1;	//ֻ����һ��Ŀ��
			bool bShotBack = kBP.m_bShotBack;

			//�ڵ�ǰĿ����Χ�����µ�Ŀ�ֻ꣬ȡ��һ��
			object_id_vector kTargetVector = _bulletLockTarget(pkSrcActor, (SK_BulletEvent*)pkEvent, kTargetLocation, lBulletLockRange, lActorFilterFuncID, lBulletMaxLockCount, bShotBack);
			int lNextTargetID = !kTargetVector.empty() ? kTargetVector.front() : 0;
			LC_ActorBase* pkNextTarget = lNextTargetID != 0 ? (LC_ActorBase*)pkWorldManager->FindObject(lNextTargetID) : NULL;

			if (NULL != pkNextTarget)
			{
				//�����ӵ������һ��Ŀ����еľ���
				UT_Vec3Int kNextTargetLocation;
				int nRetCode = pkNextTarget->GetCurrentLocationWithZ(kNextTargetLocation);
				int iDistance = UT_MathBase::LineLengthXYInt(kTargetLocation, kNextTargetLocation);
				float fNextMoveTime = Long2Float(iDistance - pkCurrentTarget->GetProperty_ShapeRadius() - pkNextTarget->GetProperty_ShapeRadius()) / Long2Float(UT_MathBase::MaxInt(1, lMoveSpeed));

				pkEvent->SetDstActorID(lNextTargetID);
				pkEvent->SetParam(SK_BulletEvent::SEP_MOVE_START_TIME, SK_BulletEvent::ParamFloat2LLong(fCurrentTime));
				pkEvent->SetParam(SK_BulletEvent::SEP_MOVE_LIFE_TIME, SK_BulletEvent::ParamFloat2LLong(fNextMoveTime));

				//�򱻴���Ŀ��㲥������Ϣ
				_broadcastBulletPenetrate(pkCurrentTarget, (SK_BulletEvent*)pkEvent, kTargetLocation);


				//��¼�ӵ�λ��
				pkSrcActor->SetBulletLocation(kTargetLocation);
				//��͸�������˺���Ŀ��
				pkSkillExecute->BulletPenetrate(lBulletFuncID, pkSrcActor, lDstActorID, lPenetrateCount, _BULLET_UUID(lEventID, lBulletLocalID));

				//�������¼�
				bDestroyEvent = false;
				return;
			}
		}

		//�򱻱�ը�㲥��ը��Ϣ
		_broadcastBulletExplosive(pkCurrentTarget, (SK_BulletEvent*)pkEvent, kTargetLocation, lDstActorID, SK_BulletEvent::SBER_Timeout);

		//��¼�ӵ�λ��
		pkSrcActor->SetBulletLocation(kTargetLocation);
		//��ը����
		pkSkillExecute->BulletExplosive(lBulletFuncID, pkSrcActor, lDstActorID, lPenetrateCount, _BULLET_UUID(lEventID, lBulletLocalID));
	}
	else
	{
		//����Ŀ�����ӵ������ձ�ըλ��
		LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
		LC_ActorBase* pkCurrentTarget = (LC_ActorBase*)pkWorldManager->FindObject(lDstActorID);
		if (NULL != pkCurrentTarget)
		{
			const UT_Vec3Int & kTargetLocation = pkCurrentTarget->GetCurrentLocation();
			pkEvent->SetParam(SK_BulletEvent::SEP_TARGET_LOCATION_X, kTargetLocation.x);
			pkEvent->SetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Y, kTargetLocation.y);
			pkEvent->SetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Z, kTargetLocation.z);
		}

		LC_ActorBase* pkSrcActor = (LC_ActorBase*)pkWorldManager->FindObject(lSrcActorID);
		if (NULL != pkSrcActor)
		{
			int lBulletShapeRadius = kBP.m_lShapeRadius;
			int lBulletMaxLockCount = kBP.m_lMaxLockCount;
			bool bPeneStraight = kBP.m_bPeneStraight;
			bool bProcHalfWay = kBP.m_bPuncture;

			//�ж��Ƿ����ֱ�ߴ���
			bool bProcPath = bPeneStraight && bProcHalfWay && (lBulletMaxLockCount > 0) && (lBulletShapeRadius > 0);

			if (bProcPath)
			{
				UT_Vec3Int kShotLocation;
				kShotLocation.x = (int)pkEvent->GetParam(SK_BulletEvent::SEP_SHOT_LOCATION_X);
				kShotLocation.y = (int)pkEvent->GetParam(SK_BulletEvent::SEP_SHOT_LOCATION_Y);
				kShotLocation.z = (int)pkEvent->GetParam(SK_BulletEvent::SEP_SHOT_LOCATION_Z);

				UT_Vec3Int kTargetLocation;
				kTargetLocation.x = (int)pkEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_X);
				kTargetLocation.y = (int)pkEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Y);
				kTargetLocation.z = (int)pkEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Z);

				// ��ֵ�����ӵ���ǰλ��
				int iMoveType = (int)pkEvent->GetParam(SK_BulletEvent::SEP_MOVE_TYPE);
				float fMoveDistance = lMoveSpeed * (fCurrentTime - fStartTime);
				UT_Vec3Int kBulletLocation;
				switch (iMoveType)
				{
				case BT_TERMINAL_HOVER:
					{
						UT_Vec3Int kRoute = kTargetLocation - kShotLocation;
						float fRouteLength = kRoute.Length();
						kBulletLocation = fMoveDistance > fRouteLength? kTargetLocation: UT_MathBase::MoveAlongInt3D(kShotLocation, kTargetLocation, fMoveDistance);
					}
					break;
				default:
					kBulletLocation = UT_MathBase::MoveAlongInt3D(kShotLocation, kTargetLocation, fMoveDistance + 30);
					break;
				}
				
				//��ȡ����Ŀ�귽ʽ
				int lActorFilterFuncID = pkSkillExecute->GetProperty_LockTargetType();
				bool bShotBack = kBP.m_bShotBack;

				// �ڵ�ǰλ����Χ����Ŀ��
				object_id_vector kPenetrateTargetVector = _bulletLockTarget(pkSrcActor, (SK_BulletEvent*)pkEvent, kBulletLocation, lBulletShapeRadius, lActorFilterFuncID, lBulletMaxLockCount, bShotBack);

				// ��ʼ�ӵ���ǰλ�ô���
				object_id_vector_iter kPenetrateIter;
				TRAV(kPenetrateTargetVector, kPenetrateIter)
				{
					object_id_type lTargetID = *kPenetrateIter;
					LC_ActorBase* pCurrentTarget = (LC_ActorBase*)pkWorldManager->FindObject(lTargetID);
					if (NULL == pCurrentTarget)
					{
						continue;
					}

					//��¼�ӵ�λ��
					pkSrcActor->SetBulletLocation(kBulletLocation);

					//�ӵ���͸
					//��ǰλ�ô���ֻ�ýű������˺������ı��ӵ��켣
					pkSkillExecute->BulletPenetrate(lBulletFuncID, pkSrcActor, lTargetID, lPenetrateCount, _BULLET_UUID(lEventID, lBulletLocalID));	

					//��¼�������Ŀ��
					pkEvent->SetParam(SK_BulletEvent::SEP_TARGET_0, lTargetID);

					//�ͻ����յ�������Ϣ�����pkSkillExecute->BulletPenetrate()��BulletManager->BulletPenetrate()
					//ʵ���ϴ��̺͸ı�켣Ӧ���������¼����ͻ��˿���ʹ�ô������ı�켣��
					_broadcastBulletPenetrate(pCurrentTarget, (SK_BulletEvent*)pkEvent, kBulletLocation);
				}
			}
		}
		//�������¼�
		bDestroyEvent = false;
	}
}
//------------------------------------------------------------
void SK_ServerSkillEventManager::_procBulletToLocation(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent)
{
	/*
		_procBulletToLocation���ӵ�����Ŀ��㣬���bProcHalfwayΪtrue�������ж������κ������������ж����ӵ������¼�������ը
		����Ǵ��̣�������Χ����һ��Ŀ�꣨Ŀ��������SkillBulletFunc.csv�������ã���������̣�������󴩴�����ը
	*/
	object_id_type lSrcActorID = pkEvent->GetSrcActorID();
	int lSkillID = pkEvent->GetLogicID();
	float fStartTime = SK_BulletEvent::ParamLLong2Float(pkEvent->GetParam(SK_BulletEvent::SEP_MOVE_START_TIME));
	float fMoveTime = SK_BulletEvent::ParamLLong2Float(pkEvent->GetParam(SK_BulletEvent::SEP_MOVE_LIFE_TIME));
	int lMoveSpeed = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_MOVE_SPEED);
	int lBulletFuncID = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_BULLET_FUNC);
	int lMaxPenetrateCount = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_MAX_PENETRATE_COUNT);
	int lPenetrateCount = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_PENETRATE_COUNT);
	long lEventID = pkEvent->GetEventID();
	long lBulletLocalID = pkEvent->GetParam(SK_BulletEvent::SEP_BULLET_LOCAL_ID);
	float fLastHoverTime = SK_BulletEvent::ParamLLong2Float(pkEvent->GetParam(SK_BulletEvent::SEP_LAST_UPDATE_TIME));

	//�����¼�
	bDestroyEvent = true;

	//�����ӵ��ͷ���
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ActorBase* pkSrcActor = (LC_ActorBase*)pkWorldManager->FindObject(lSrcActorID);
	if (NULL == pkSrcActor)
		return;

	SK_Factory* pkSkillFactory = SK_Factory::GetSingletonPtr();
	SK_SkillExecute* pkSkillExecute = pkSkillFactory->RequestSkillExcute(lSkillID);
	if (NULL == pkSkillExecute)
		return;

	//��ȡ����Ŀ�귽ʽ
	int lActorFilterFuncID = pkSkillExecute->GetProperty_LockTargetType();
	//�Ƿ����м����
	const SK_SkillExecute::BulletParam & kBP = pkSkillExecute->GetProperty_BulletParam(lBulletFuncID);
	bool bProcHalfway = kBP.m_bPuncture;
	float fTerminalHoverInterval = kBP.m_fTerminalHoverInterval;

	//�Ѿ����е�Ŀ���
	if (fCurrentTime - fStartTime > fMoveTime)
	{
		UT_Vec3Int kTargetLocation;
		kTargetLocation.x = (int)pkEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_X);
		kTargetLocation.y = (int)pkEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Y);
		kTargetLocation.z = (int)pkEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Z);

		//�ӵ���ը
		_broadcastBulletExplosive(pkSrcActor, (SK_BulletEvent*)pkEvent, kTargetLocation, 0, SK_BulletEvent::SBER_Timeout);

		//��¼�ӵ�λ��
		pkSrcActor->SetBulletLocation(kTargetLocation);
		pkSkillExecute->BulletExplosive(lBulletFuncID, pkSrcActor, 0, lPenetrateCount, _BULLET_UUID(lEventID, lBulletLocalID));
	}
	else if (bProcHalfway)	//��Ҫ�����м����
	{
		bool bPeneStraight = kBP.m_bPeneStraight;
		//��ȡ�ӵ�����
		int lMaxLockCount = kBP.m_lMaxLockCount;
		if (lMaxLockCount <= 0)
			return;

		int lBulletShapeRadius = kBP.m_lShapeRadius;
		if (lBulletShapeRadius <= 0)
			return;


		// �ӵ�����λ�ú��յ�λ��
		UT_Vec3Int kShotLocation;
		kShotLocation.x = (int)pkEvent->GetParam(SK_BulletEvent::SEP_SHOT_LOCATION_X);
		kShotLocation.y = (int)pkEvent->GetParam(SK_BulletEvent::SEP_SHOT_LOCATION_Y);
		kShotLocation.z = (int)pkEvent->GetParam(SK_BulletEvent::SEP_SHOT_LOCATION_Z);

		UT_Vec3Int kTargetLocation;
		kTargetLocation.x = (int)pkEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_X);
		kTargetLocation.y = (int)pkEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Y);
		kTargetLocation.z = (int)pkEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Z);

		bool bShotBack = kBP.m_bShotBack;
		bool bRepeatStrike = false;
		int lBulletMaxLockCount = bShotBack? 1: lMaxLockCount;

		// �����ӵ����� ��ֵ�����ӵ���ǰλ��
		int iMoveType = (int)pkEvent->GetParam(SK_BulletEvent::SEP_MOVE_TYPE);
		float fMoveDistance = lMoveSpeed * (fCurrentTime - fStartTime);
		UT_Vec3Int kBulletLocation;		
		switch (iMoveType)
		{
		case BT_TERMINAL_HOVER:
			{
				UT_Vec3Int kRoute = kTargetLocation - kShotLocation;
				float fRouteLength = kRoute.Length();

				if (fMoveDistance >= fRouteLength)
				{
					if (fCurrentTime < fLastHoverTime + fTerminalHoverInterval)
					{
						bDestroyEvent = false;
						return;
					}
					else
					{
						pkEvent->SetParam(SK_BulletEvent::SEP_LAST_UPDATE_TIME, SK_SkillEvent::ParamFloat2LLong(fCurrentTime));
						bRepeatStrike = true;
					}
				}

				kBulletLocation = fMoveDistance > fRouteLength? kTargetLocation: UT_MathBase::MoveAlongInt3D(kShotLocation, kTargetLocation, fMoveDistance);
			}
			break;
		default:
			kBulletLocation = UT_MathBase::MoveAlongInt3D(kShotLocation, kTargetLocation, fMoveDistance + 30);
			break;
		}

		// �ڵ�ǰλ����Χ����Ŀ��
		object_id_vector kPenetrateTargetVector = _bulletLockTarget(pkSrcActor, (SK_BulletEvent*)pkEvent, kBulletLocation, lBulletShapeRadius, lActorFilterFuncID, lBulletMaxLockCount, bShotBack, bRepeatStrike);
		
		// ��ʼ�ӵ���ǰλ�ô���
		object_id_vector_iter kPenetrateIter;
		TRAV(kPenetrateTargetVector, kPenetrateIter)
		{
			object_id_type lTargetID = *kPenetrateIter;
			LC_ActorBase* pkCurrentTarget = (LC_ActorBase*)pkWorldManager->FindObject(lTargetID);
			if (pkCurrentTarget == NULL)
			{
				continue;
			}
			//��͸Ŀ���������㣬�������
			if (lPenetrateCount < lMaxPenetrateCount)
			{
				//��¼�ӵ�λ��
				pkSrcActor->SetBulletLocation(kBulletLocation);
				//�ӵ���͸
				pkSkillExecute->BulletPenetrate(lBulletFuncID, pkSrcActor, lTargetID, lPenetrateCount, _BULLET_UUID(lEventID, lBulletLocalID));	//��ǰλ�ô���ֻ�ýű������˺������ı��ӵ��켣

				//��¼�������Ŀ��

				if (lPenetrateCount<0)
				{
					return;
				}
				lPenetrateCount ++;
				pkEvent->SetParam(SK_BulletEvent::SEP_PENETRATE_COUNT, lPenetrateCount);
				pkEvent->SetParam(SK_BulletEvent::SEP_TARGET_BEGIN + lPenetrateCount - 1, lTargetID);

				//�ͻ����յ�������Ϣ�����pkSkillExecute->BulletPenetrate()��BulletManager->BulletPenetrate()
				//ʵ���ϴ��̺͸ı�켣Ӧ���������¼����ͻ��˿���ʹ�ô������ı�켣��
				_broadcastBulletPenetrate(pkCurrentTarget, (SK_BulletEvent*)pkEvent, kBulletLocation);
			}
			else if (bPeneStraight)
			{
				//��¼�ӵ�λ��
				pkSrcActor->SetBulletLocation(kBulletLocation);

				//�ӵ���͸
				pkSkillExecute->BulletPenetrate(lBulletFuncID, pkSrcActor, lTargetID, lPenetrateCount, _BULLET_UUID(lEventID, lBulletLocalID));	//��ǰλ�ô���ֻ�ýű������˺������ı��ӵ��켣

				lPenetrateCount ++;
				//��¼�������Ŀ��
				pkEvent->SetParam(SK_BulletEvent::SEP_TARGET_0, lTargetID);

				//�ͻ����յ�������Ϣ�����pkSkillExecute->BulletPenetrate()��BulletManager->BulletPenetrate()
				//ʵ���ϴ��̺͸ı�켣Ӧ���������¼����ͻ��˿���ʹ�ô������ı�켣��
				_broadcastBulletPenetrate(pkCurrentTarget, (SK_BulletEvent*)pkEvent, kBulletLocation);
			}
			else	//��͸Ŀ�������㹻����ը
			{
				_broadcastBulletExplosive(pkSrcActor, (SK_BulletEvent*)pkEvent, kBulletLocation, lTargetID, SK_BulletEvent::SBER_PernerateFull);

				//��¼�ӵ�λ��
				pkSrcActor->SetBulletLocation(kBulletLocation);
				pkSkillExecute->BulletExplosive(lBulletFuncID, pkSrcActor, lTargetID, lPenetrateCount, _BULLET_UUID(lEventID, lBulletLocalID));

				//�ӵ��Ѿ���ը��������ʣ���Ŀ��
				return;
			}
		}

		do	//��ʼ��Χ����
		{
			if (kPenetrateTargetVector.empty())	//��Ŀ���˲�������Χ����
				break;
			
			if (bPeneStraight)
				break;

			//��ʼ��Χ��͸
			int lBulletLockRange = kBP.m_lLockRange;
			if (lBulletLockRange == 0)	//������Χ�������lBulletLockRange����Ϊ0
				break;

			//���ҵ�һ�����̵�Ŀ��
			object_id_type lTargetID = kPenetrateTargetVector.front();
			LC_ActorBase* pkCurrentTarget = (LC_ActorBase*)pkWorldManager->FindObject(lTargetID);
			if (pkCurrentTarget == NULL)
				break;

			//ֻ����һ��Ŀ��
			int lBulletMaxLockCount = 1;

			object_id_vector kTargetVector = _bulletLockTarget(pkSrcActor, (SK_BulletEvent*)pkEvent, kBulletLocation, lBulletLockRange, lActorFilterFuncID, lBulletMaxLockCount, bShotBack, bRepeatStrike);
			int lNextTargetID = !kTargetVector.empty() ? kTargetVector.front() : 0;
			LC_ActorBase* pkNextTarget = lNextTargetID != 0 ? (LC_ActorBase*)pkWorldManager->FindObject(lNextTargetID) : NULL;
			if (pkNextTarget == NULL)
				break;

			//�����ӵ������һ��Ŀ����еľ��룬���Ŀ��һ���ǲ�����lBulletShapeRadius��Χ�ڵģ���Ϊ����ڣ�ǰ���б��Ѿ�������ˣ������ӵ��Ѿ����̹��౬ը��
			int iBulletFuncID = (int)pkEvent->GetParam(SK_BulletEvent::SEP_BULLET_FUNC);
			int lMaxMoveDistance = kBP.m_lMaxMoveDistance;
			//const UT_Vec3Int & kNextTargetLocation = pkNextTarget->GetCurrentLocationWithZ();	//pkNextTargetֻ������ȷ����һ�����䷽��
			UT_Vec3Int kNextTargetLocation;
			int nRetCode = pkNextTarget->GetCurrentLocationWithZ(kNextTargetLocation);	//pkNextTargetֻ������ȷ����һ�����䷽��
			UT_Vec3Int kTargetLocation = UT_MathBase::MoveAlongIntXY(kBulletLocation, kNextTargetLocation, Long2Float(lMaxMoveDistance));
			float fNextMoveTime = Long2Float(lMaxMoveDistance) / Long2Float(lMoveSpeed);

			pkEvent->SetParam(SK_BulletEvent::SEP_MOVE_START_TIME, SK_BulletEvent::ParamFloat2LLong(fCurrentTime));
			pkEvent->SetParam(SK_BulletEvent::SEP_MOVE_LIFE_TIME, SK_BulletEvent::ParamFloat2LLong(fNextMoveTime));
			pkEvent->SetParam(SK_BulletEvent::SEP_COLLIDE_PARAM, Skill::SerializeToQuatWord(SK_BulletEvent::CollideParam(1.0f, 0)));	// �޸�Ŀ��ʱδ����ײ��������ײ����

			//�򱻴���Ŀ��㲥�ı�켣��Ϣ
			_broadcastBulletChangeTargetLocation(pkCurrentTarget, (SK_BulletEvent*)pkEvent, kBulletLocation, kTargetLocation);
		}while (0);

		//�������¼����ȴ��´δ���
		bDestroyEvent = false;
	}
	else
	{
		//����Ҫ�����¼�
		bDestroyEvent = false;
	}
}
//------------------------------------------------------------
bool SK_ServerSkillEventManager::ActiveBuddyReqBulletPenetrate(GameLogic::LC_ActorBase* pkActor, const Protocol::MG_REQ_Bullet_Penetrate & req)
{
	SK_SkillEvent * pkEvent = this->AllocEvent();
	pkEvent->SetEventType(SETE_BULLET);
	pkEvent->SetEventID(req.m_lBulletEventID);
	pkEvent->SetLogicID(req.m_lSkillID);
	pkEvent->SetSrcActorID(req.m_lRequesterID);
	pkEvent->SetDstActorID(req.m_lToucherID);
	pkEvent->SetParam(SK_BulletEvent::SEP_ACTION_TYPE, SK_BulletEvent::SBAT_PENETRATE);
	pkEvent->SetParam(SK_BulletEvent::SEP_BULLET_LOCAL_ID, req.m_lBulletLocalID);
	pkEvent->SetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_X, req.m_lBulletLocationX);
	pkEvent->SetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_Y, req.m_lBulletLocationY);
	pkEvent->SetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_Z, req.m_lBulletLocationZ);
	pkEvent->SetParam(SK_BulletEvent::SEP_TARGET_0, req.m_lNextTargetID);

	m_kActiveBuddyBulletReqEvents.push_back(pkEvent);
	return true;
}

bool SK_ServerSkillEventManager::ActiveBuddyReqBulletExplosive(GameLogic::LC_ActorBase* pkActor, const Protocol::MG_REQ_Bullet_Explosive & req)
{
	SK_SkillEvent * pkEvent = this->AllocEvent();
	pkEvent->SetEventType(SETE_BULLET);
	pkEvent->SetEventID(req.m_lBulletEventID);
	pkEvent->SetLogicID(req.m_lSkillID);
	pkEvent->SetSrcActorID(req.m_lRequesterID);
	pkEvent->SetDstActorID(req.m_lToucherID);
	pkEvent->SetParam(SK_BulletEvent::SEP_ACTION_TYPE, SK_BulletEvent::SBAT_EXPLOSIVE | req.m_iExplosiveReason);
	pkEvent->SetParam(SK_BulletEvent::SEP_BULLET_LOCAL_ID, req.m_lBulletLocalID);
	pkEvent->SetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_X, req.m_lBulletLocationX);
	pkEvent->SetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_Y, req.m_lBulletLocationY);
	pkEvent->SetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_Z, req.m_lBulletLocationZ);

	m_kActiveBuddyBulletReqEvents.push_back(pkEvent);
	return true;
}

bool SK_ServerSkillEventManager::ActiveBuddyReqBulletChangeTargetLocation(GameLogic::LC_ActorBase* pkActor, const Protocol::MG_REQ_Bullet_ChangeTargetLocation & req)
{
	SK_SkillEvent * pkEvent = this->AllocEvent();
	pkEvent->SetEventType(SETE_BULLET);
	pkEvent->SetEventID(req.m_lBulletEventID);
	pkEvent->SetLogicID(req.m_lSkillID);
	pkEvent->SetSrcActorID(req.m_lRequesterID);
	pkEvent->SetDstActorID(req.m_lFromTargetID);
	pkEvent->SetParam(SK_BulletEvent::SEP_ACTION_TYPE, SK_BulletEvent::SBAT_CHANGE_TARGET_LOCATION);
	pkEvent->SetParam(SK_BulletEvent::SEP_BULLET_LOCAL_ID, req.m_lBulletLocalID);
	pkEvent->SetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_X, req.m_lBulletLocationX);
	pkEvent->SetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_Y, req.m_lBulletLocationY);
	pkEvent->SetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_Z, req.m_lBulletLocationZ);
	pkEvent->SetParam(SK_BulletEvent::SEP_TARGET_LOCATION_X, req.m_lTargetLocationX);
	pkEvent->SetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Y, req.m_lTargetLocationY);
	pkEvent->SetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Z, req.m_lTargetLocationZ);
	pkEvent->SetParam(SK_BulletEvent::SEP_TARGET_0, req.m_lToTargetID);

	m_kActiveBuddyBulletReqEvents.push_back(pkEvent);
	return true;
}
//------------------------------------------------------------

static bool is_target_penetrated(SK_SkillEvent* pkEvent, object_id_type lTargetID)
{
	int32_t lPenetrateCount = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_PENETRATE_COUNT);
	for(int i = 0; i < lPenetrateCount; ++i)
	{
		object_id_type pid = (object_id_type)pkEvent->GetParam(SK_BulletEvent::SEP_TARGET_BEGIN + i);
		if (pid == lTargetID)
			return true;
	}
	return false;
}

Skill::SK_SkillEvent* Skill::SK_ServerSkillEventManager::FindEvent(int eventID)
{
	SK_SkillEvent* pEvent = NULL;
	SK_SkillEventListIter mIter = m_kMainEventList.begin();
	for (; mIter != m_kMainEventList.end(); ++mIter)
	{
		pEvent = *mIter;
		if (pEvent->GetEventID() == eventID)
			return pEvent;
	}

	SK_SkillEventListIter cIter = m_kChainEventList.begin();
	for (; cIter != m_kChainEventList.end(); ++cIter)
	{
		pEvent = *cIter;
		if (pEvent->GetEventID() == eventID)
			return pEvent;
	}

	return NULL;
}

/*
TODO: ��ȫ�Լ�飺
	�ӵ���������ͬһ��Ŀ��
	�ӵ���ը����
*/
void Skill::SK_ServerSkillEventManager::_procActiveBuddyBulletReqEvent(float fCurrentTime, SK_SkillEvent * pkEvent, bool& bDestroyEvent)
{
	// Ĭ�ϲ�ɾ��
	bDestroyEvent = false;

	object_id_type lSrcActorID = pkEvent->GetSrcActorID();
	int lSkillID = pkEvent->GetLogicID();
	int lBulletFuncID = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_BULLET_FUNC);
	long lEventID = pkEvent->GetEventID();
	long lBulletLocalID = pkEvent->GetParam(SK_BulletEvent::SEP_BULLET_LOCAL_ID);


	// Ŀ����or������
	bool bProcToTarget = false;
	int lMoveType = (int)pkEvent->GetParam(SK_BulletEvent::SEP_MOVE_TYPE);
	switch(lMoveType)
	{
	case BT_SHOOT:
	case BT_MISSILE:
	case BT_HOMINGMISSLE:
		{
			object_id_type lDstActorID = pkEvent->GetDstActorID();
			bProcToTarget = IS_PLAYER_CLASS_ID(lDstActorID) || IS_NPC_ID(lDstActorID);
		}
		break;
	case BT_CHAIN:
		bProcToTarget = true;
		break;
	}

	// TODO: �˴���Ҫ�Ż�
	//�����ӵ��ͷ���
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ActorBase* pkSrcActor = (LC_ActorBase*)pkWorldManager->FindObject(lSrcActorID);
	if (NULL == pkSrcActor)
		return;

	SK_Factory* pkSkillFactory = SK_Factory::GetSingletonPtr();
	SK_SkillExecute* pkSkillExecute = pkSkillFactory->RequestSkillExcute(lSkillID);
	if (NULL == pkSkillExecute)
		return;

	const SK_SkillExecute::BulletParam & kBP = pkSkillExecute->GetProperty_BulletParam(lBulletFuncID);
	bool bShotBack = kBP.m_bShotBack;
	bool bPeneStraight = kBP.m_bPeneStraight;

	// ������¼�����Ӧ������
	SK_SkillEventList& rkReqList = m_kActiveBuddyBulletReqEvents;

	// event����
	object_id_type o1 = pkEvent->GetSrcActorID();
	object_id_type s1 = pkEvent->GetLogicID();
	long  lLocalID1 = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_BULLET_LOCAL_ID);
	object_id_type d1 = pkEvent->GetDstActorID();

	// ����������������䣬���ܰ� rkReqList.end() ��ȡ�������б��ʽ�Ż����ᵼ�µ������쳣
	for (SK_SkillEventListIter iter = rkReqList.begin(); iter != rkReqList.end(); )
	{
		SK_SkillEvent* pkReq = *iter;

		// �� rkReqList �в����� pkEvent ��ص�������д������ܲ�ֹ1��
		object_id_type o2 = pkReq->GetSrcActorID();
		if (o1 != o2) {
			++iter;			// pkReq �� pkEvent ������������һ��
			continue;
		}

		object_id_type s2 = pkReq->GetLogicID();
		if (s1 != s2) {
			++iter;
			continue;
		}

		long  lLocalID2 = (int32_t)pkReq->GetParam(SK_BulletEvent::SEP_BULLET_LOCAL_ID);
		if (lLocalID1 != lLocalID2) {
			++iter;
			continue;
		}

		if (bProcToTarget)
		{
			object_id_type d2 = pkReq->GetDstActorID();
			if (d1 != d2) {
				++iter;
				continue;
			}
		}

		// ��Ӧ����
		int lActionType = (int32_t)pkReq->GetParam(SK_BulletEvent::SEP_ACTION_TYPE);
		switch(lActionType & SK_BulletEvent::SBAT_Mask)
		{
		case SK_BulletEvent::SBAT_PENETRATE:
			{
				// ����Ŀ��
				object_id_type lDstActorID = pkReq->GetDstActorID();
				if (lDstActorID == 0)
					break;

				// �Ѿ����̹�
				if (!bShotBack && is_target_penetrated(pkEvent, lDstActorID))
					break;

				//���ҵ�ǰĿ��
				LC_ActorBase* pkCurrentTarget = (LC_ActorBase*)pkWorldManager->FindObject(lDstActorID);
				if (NULL == pkCurrentTarget)
					break;

				int lMaxPenetrateCount = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_MAX_PENETRATE_COUNT);
				int lPenetrateCount =(int32_t) pkEvent->GetParam(SK_BulletEvent::SEP_PENETRATE_COUNT);
				//const UT_Vec3Int & kTargetLocation2 = pkCurrentTarget->GetCurrentLocationWithZ();

				// TODO: ��ǰĿ���Ƿ�ɹ�����Ŀ��λ���Ƿ����
				UT_Vec3Int kTargetLocation;
				kTargetLocation.x = (int32_t)pkReq->GetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_X);
				kTargetLocation.y = (int32_t)pkReq->GetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_Y);
				kTargetLocation.z = (int32_t)pkReq->GetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_Z);

				//��Ҫ��͸�µ�Ŀ��
				if (lPenetrateCount < lMaxPenetrateCount)
				{
					//��¼�������Ŀ�꣬_bulletLockTarget�������ų�����Ŀ��
					if (lPenetrateCount < 0 || lPenetrateCount >= SK_BulletEvent::MAX_BULLET_PENETRATE_COUNT)
						break;
					pkEvent->SetParam(SK_BulletEvent::SEP_PENETRATE_COUNT, lPenetrateCount + 1);
					pkEvent->SetParam(SK_BulletEvent::SEP_TARGET_BEGIN + lPenetrateCount, lDstActorID);

					//�򱻴���Ŀ��㲥������Ϣ
					_broadcastBulletPenetrate(pkCurrentTarget, (SK_BulletEvent*)pkEvent, kTargetLocation);

					//��¼�ӵ�λ��
					pkSrcActor->SetBulletLocation(kTargetLocation);
					//��͸�������˺���Ŀ��
					int lBulletFuncID = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_BULLET_FUNC);
					pkSkillExecute->BulletPenetrate(lBulletFuncID, pkSrcActor, lDstActorID, lPenetrateCount, _BULLET_UUID(lEventID, lBulletLocalID));
				}
				else if (bPeneStraight)
				{
					if (lPenetrateCount < 0 || lPenetrateCount >= SK_BulletEvent::MAX_BULLET_PENETRATE_COUNT)
						break;
					pkEvent->SetParam(SK_BulletEvent::SEP_PENETRATE_COUNT, lPenetrateCount);
					pkEvent->SetParam(SK_BulletEvent::SEP_TARGET_0, lDstActorID);

					//�򱻴���Ŀ��㲥������Ϣ
					_broadcastBulletPenetrate(pkCurrentTarget, (SK_BulletEvent*)pkEvent, kTargetLocation);

					//��¼�ӵ�λ��
					pkSrcActor->SetBulletLocation(kTargetLocation);
					//��͸�������˺���Ŀ��
					int lBulletFuncID = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_BULLET_FUNC);
					pkSkillExecute->BulletPenetrate(lBulletFuncID, pkSrcActor, lDstActorID, lPenetrateCount, _BULLET_UUID(lEventID, lBulletLocalID));
				}

				// ����ת��Ŀ��
				object_id_type lOldTargetID  = lDstActorID;
				object_id_type lNextTargetID = (object_id_type)pkReq->GetParam(SK_BulletEvent::SEP_TARGET_0);
				if (bProcToTarget && lNextTargetID != 0)	// Ŀ�����ӵ��Ĵ��̣��Ŵ�����ת��Ŀ��
				{
					LC_ActorBase* pkNextTarget =  (LC_ActorBase*)pkWorldManager->FindObject(lNextTargetID);
					if (pkNextTarget != NULL)
					{
						int lMoveSpeed = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_MOVE_SPEED);
						//const UT_Vec3Int & kNextTargetLocation = pkNextTarget->GetCurrentLocationWithZ();
						UT_Vec3Int kNextTargetLocation;
						int nRetCode = pkNextTarget->GetCurrentLocationWithZ(kNextTargetLocation);
						int iDistance = UT_MathBase::LineLengthXYInt(kTargetLocation, kNextTargetLocation);
						float fNextMoveTime = Long2Float(iDistance - pkCurrentTarget->GetProperty_ShapeRadius() - pkNextTarget->GetProperty_ShapeRadius()) / Long2Float(UT_MathBase::MaxInt(1, lMoveSpeed));

						pkEvent->SetDstActorID(lNextTargetID);
						pkEvent->SetParam(SK_BulletEvent::SEP_MOVE_START_TIME, SK_BulletEvent::ParamFloat2LLong(fCurrentTime));
						pkEvent->SetParam(SK_BulletEvent::SEP_MOVE_LIFE_TIME, SK_BulletEvent::ParamFloat2LLong(fNextMoveTime));
					}
				}

			}
			break;

		case SK_BulletEvent::SBAT_EXPLOSIVE:
			{
				// �ѱ�ը���¼���ɾ��
				bDestroyEvent = true;

				int lBulletFuncID = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_BULLET_FUNC);
				int lPenetrateCount = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_PENETRATE_COUNT);
				object_id_type lTargetID = pkReq->GetDstActorID();
/*
				// TODO: ��ǰĿ���Ƿ�ɹ�����Ŀ��λ���Ƿ����
				UT_Vec3Int kTargetLocation2;
				kTargetLocation2.x = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_X);
				kTargetLocation2.y = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Y);
				kTargetLocation2.z = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Z);
*/
				UT_Vec3Int kTargetLocation;
				kTargetLocation.x = (int32_t)pkReq->GetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_X);
				kTargetLocation.y = (int32_t)pkReq->GetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_Y);
				kTargetLocation.z = (int32_t)pkReq->GetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_Z);

				//�ӵ���ը
				_broadcastBulletExplosive(pkSrcActor, (SK_BulletEvent*)pkEvent, kTargetLocation, lTargetID, (SK_BulletEvent::SK_BulletExplosiveReason)lActionType);

				//��¼�ӵ�λ��
				pkSrcActor->SetBulletLocation(kTargetLocation);
				pkSkillExecute->BulletExplosive(lBulletFuncID, pkSrcActor, lTargetID, lPenetrateCount, _BULLET_UUID(lEventID, lBulletLocalID));
			}
			break;

		case SK_BulletEvent::SBAT_CHANGE_TARGET_LOCATION:
			{
				// TODO: �ӵ�λ���Ƿ����Ŀ�ĵ��Ƿ��ڷ�Χ��

				int lMoveSpeed = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_MOVE_SPEED);
				int lBulletFuncID = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_BULLET_FUNC);

				//��ʼ��Χ��͸
				const SK_SkillExecute::BulletParam & kBP = pkSkillExecute->GetProperty_BulletParam(lBulletFuncID);
				int lBulletLockRange = kBP.m_lLockRange;
				if (lBulletLockRange == 0)	//������Χ�������lBulletLockRange����Ϊ0
					break;

				//���ҵ�һ�����̵�Ŀ��
				object_id_type lTargetID = (object_id_type)pkReq->GetDstActorID();
				LC_ActorBase* pkCurrentTarget = (LC_ActorBase*)pkWorldManager->FindObject(lTargetID);
				if (pkCurrentTarget == NULL)
					break;

				UT_Vec3Int kBulletLocation;
				kBulletLocation.x = (int32_t)pkReq->GetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_X);
				kBulletLocation.y = (int32_t)pkReq->GetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_Y);
				kBulletLocation.z = (int32_t)pkReq->GetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_Z);

				UT_Vec3Int kTargetLocation;
				kTargetLocation.x = (int32_t)pkReq->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_X);
				kTargetLocation.y = (int32_t)pkReq->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Y);
				kTargetLocation.z = (int32_t)pkReq->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Z);
/*
				object_id_type lNextTargetID = (object_id_type)pkReq->GetParam(SK_BulletEvent::SEP_TARGET_0);
				LC_ActorBase* pkNextTarget = lNextTargetID != 0 ? (LC_ActorBase*)pkWorldManager->FindObject(lNextTargetID) : NULL;
				if (pkNextTarget == NULL)
					break;

				//�����ӵ������һ��Ŀ����еľ��룬���Ŀ��һ���ǲ�����lBulletShapeRadius��Χ�ڵģ���Ϊ����ڣ�ǰ���б��Ѿ�������ˣ������ӵ��Ѿ����̹��౬ը��
				int iBulletFuncID = (int)pkEvent->GetParam(SK_BulletEvent::SEP_BULLET_FUNC);
				int lMaxMoveDistance = pkSkillExecute->GetProperty_BulletMaxMoveDistance(iBulletFuncID);
				const UT_Vec3Int & kNextTargetLocation = pkNextTarget->GetCurrentLocationWithZ();	//pkNextTargetֻ������ȷ����һ�����䷽��
				UT_Vec3Int kTargetLocation2 = UT_MathBase::MoveAlongIntXY(kBulletLocation, kNextTargetLocation, Long2Float(lMaxMoveDistance));
*/
				int lMoveDistance = UT_MathBase::LineLengthXYInt(kBulletLocation, kTargetLocation);
				float fNextMoveTime = Long2Float(lMoveDistance) / Long2Float(lMoveSpeed);

				pkEvent->SetParam(SK_BulletEvent::SEP_MOVE_START_TIME, SK_BulletEvent::ParamFloat2LLong(fCurrentTime));
				pkEvent->SetParam(SK_BulletEvent::SEP_MOVE_LIFE_TIME, SK_BulletEvent::ParamFloat2LLong(fNextMoveTime));
				pkEvent->SetParam(SK_BulletEvent::SEP_COLLIDE_PARAM, Skill::SerializeToQuatWord(SK_BulletEvent::CollideParam(1.0f, 0)));	// �޸�Ŀ��ʱδ����ײ��������ײ����

				//�򱻴���Ŀ��㲥�ı�켣��Ϣ
				_broadcastBulletChangeTargetLocation(pkCurrentTarget, (SK_BulletEvent*)pkEvent, kBulletLocation, kTargetLocation);
			}
			break;
		}

		// ɾ���Ѵ��������
		m_kEventPool.Free(pkReq);
		iter = rkReqList.erase(iter);

		// �Ѿ��������ը��������req�����ٱ���
		if (bDestroyEvent)
			return;
	}
}

void Skill::SK_ServerSkillEventManager::_updateActiveBuddyBulletReqEvent(float fCurrentTime)
{
	/// ɾ�������¼�
	/// ��� Update ���ӵ��¼���������ok��m_kActiveBuddyBulletReqEvents Ӧ��ÿ֡���ᱻ��գ�������ڲ���
	SK_SkillEventList::iterator iterLast = m_kActiveBuddyBulletReqEvents.end();
	SK_SkillEventList::iterator iterEnd = m_kActiveBuddyBulletReqEvents.end();
	for (SK_SkillEventList::iterator iter = m_kActiveBuddyBulletReqEvents.begin(); iter != iterLast; )
	{
		SK_SkillEvent* pkReq = *iter;
		if (fCurrentTime - pkReq->GetStartTime() > 1.0f)
		{
			m_kEventPool.Free(pkReq);
			*iter = *(--iterLast);		// ������Ԫ�ظ��ǹ������������������в��ı��С
		}
		else
		{
			++iter;
		}
	}
	/// ���ͳһɾ��
	if (iterLast != iterEnd)
		m_kActiveBuddyBulletReqEvents.erase(iterLast, iterEnd);
}

int64_t Skill::SK_ServerSkillEventManager::_BULLET_UUID(long lEventID, long lLocalID)
{
	int64_t v = lLocalID;
	v <<= 32;
	v += lEventID;
	return v;
}

//------------------------------------------------------------
object_id_vector SK_ServerSkillEventManager::_bulletLockTarget(LC_ActorBase* pkAttacker, SK_BulletEvent* pkBulletEvent, const UT_Vec3Int& rkCircleCenter, 
															   int iCircleRadius, int lActorFilterFuncID, int iMaxLockCount, bool bShotBack, bool bRepeatStrike)
{
	LC_MapBase* pkMap = pkAttacker->GetMap();
	if (NULL == pkMap)
	{
		return object_id_vector();
	}

	CF_ActorFilterFunc* pkFilterList = CF_ActorFilterFunc::GetSingletonPtr();
	CF_ActorFilterFunc::DataEntry* pkFilterData = pkFilterList->GetEntryPtr(lActorFilterFuncID);
	if (NULL == pkFilterData)
	{
		return object_id_vector();
	}

	//����Ŀ��ķ�Χ
	UT_Circle kCircle;
	kCircle.iRadius = iCircleRadius;
	kCircle.kCenterPoint = rkCircleCenter;

	//�ӵ�ͼ���������ڵ�Actor����
	LC_ActorPtrVector* pkLockTarget = pkMap->LockActorsInCircle(kCircle, pkFilterData);
	if (pkLockTarget->empty())
	{
		return object_id_vector();
	}
	//���ݹ�������,����Actor����
	object_id_vector kFilterTarget = LC_Helper::FilterActorsSortDist(pkAttacker, *pkLockTarget, pkFilterData);

	//�Ѵ��Ŀ��
	int iTargetRecordBegin = SK_BulletEvent::SEP_TARGET_BEGIN + 0;
	int iTargetRecordEnd = SK_BulletEvent::SEP_TARGET_END;
	object_id_vector kExcludeTarget;
	for (int i = iTargetRecordBegin; i < iTargetRecordEnd; i ++)
	{
		object_id_type lTarget = (object_id_type)pkBulletEvent->GetParam(i);
		if (0 != lTarget)
		{
			kExcludeTarget.push_back(lTarget);
		}
	}

	//�ų��Ѵ����Ŀ��
	object_id_vector kFinalTarget;
	object_id_vector_iter iter;
	if (!bShotBack) 
	{
		for (iter = kFilterTarget.begin(); iter != kFilterTarget.end(); ++iter)
		{
			object_id_type lNextTarget = *iter;
			if (std::find(kExcludeTarget.begin(), kExcludeTarget.end(), lNextTarget) == kExcludeTarget.end() || bRepeatStrike)
			{
				kFinalTarget.push_back(lNextTarget);
			}
		}
	}
	else
	{
		object_id_type lLastTarget = 0;
		if (kExcludeTarget.size() > 0)
		{
			lLastTarget = *--kExcludeTarget.end();
		}
		for (iter = kFilterTarget.begin(); iter != kFilterTarget.end(); ++iter)
		{
			object_id_type lNextTarget = *iter;
			if (lLastTarget != lNextTarget && !bRepeatStrike || bRepeatStrike)
			{
				kFinalTarget.push_back(lNextTarget);
				lLastTarget = lNextTarget;
			}
		}
	}

	//�ų������Ŀ��
	while (UInt2Int((unsigned int)kFinalTarget.size()) > iMaxLockCount)
	{
		kFinalTarget.pop_back();
	}
	return kFinalTarget;
}
//------------------------------------------------------------
void SK_ServerSkillEventManager::_broadcastBulletChangeTargetLocation(LC_ActorBase* pkBroadcastCenter, SK_BulletEvent* pkEvent, const Utility::UT_Vec3Int& rkBulletLocation, const Utility::UT_Vec3Int& rkTargetLocation)
{
	//�����ӵ���ǰλ�ã�֪ͨ���ͻ�������ִ�м��ܵ��ӵ��ӿ�
	pkEvent->SetParam(SK_BulletEvent::SEP_TARGET_LOCATION_X, rkTargetLocation.x);
	pkEvent->SetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Y, rkTargetLocation.y);
	pkEvent->SetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Z, rkTargetLocation.z);
	pkEvent->SetParam(SK_BulletEvent::SEP_SHOT_LOCATION_X, rkBulletLocation.x);
	pkEvent->SetParam(SK_BulletEvent::SEP_SHOT_LOCATION_Y, rkBulletLocation.y);
	pkEvent->SetParam(SK_BulletEvent::SEP_SHOT_LOCATION_Z, rkBulletLocation.z);
	//�㲥�ͻ��˸ı�켣��Ϣ
	pkEvent->SetParam(SK_BulletEvent::SEP_ACTION_TYPE, SK_BulletEvent::SBAT_CHANGE_TARGET_LOCATION);
	SendToClient(pkBroadcastCenter, pkEvent, SSET_BROADCAST_TO_AREA);
}
//------------------------------------------------------------
void SK_ServerSkillEventManager::_broadcastBulletPenetrate(LC_ActorBase* pkBroadcastCenter, SK_BulletEvent* pkEvent, const UT_Vec3Int& rkBulletLocation)
{
	//�����ӵ���ǰλ�ã�֪ͨ���ͻ�������ִ�м��ܵ��ӵ��ӿ�
	pkEvent->SetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_X, rkBulletLocation.x);
	pkEvent->SetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_Y, rkBulletLocation.y);
	pkEvent->SetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_Z, rkBulletLocation.z);
	//�㲥�ͻ��˴�͸��Ϣ
	pkEvent->SetParam(SK_BulletEvent::SEP_ACTION_TYPE, SK_BulletEvent::SBAT_PENETRATE);
	SendToClient(pkBroadcastCenter, pkEvent, SSET_BROADCAST_TO_AREA);
}
//------------------------------------------------------------
void SK_ServerSkillEventManager::_broadcastBulletExplosive(LC_ActorBase* pkBroadcastCenter, SK_BulletEvent* pkEvent, const UT_Vec3Int& rkBulletLocation, long lTargetID, SK_BulletEvent::SK_BulletExplosiveReason enmReason)
{
	//�����ӵ���ǰλ�ã�֪ͨ���ͻ�������ִ�м��ܵ��ӵ��ӿ�
	pkEvent->SetDstActorID(lTargetID);
	pkEvent->SetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_X, rkBulletLocation.x);
	pkEvent->SetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_Y, rkBulletLocation.y);
	pkEvent->SetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_Z, rkBulletLocation.z);
	//�㲥�ͻ��˱�ը��Ϣ
	pkEvent->SetParam(SK_BulletEvent::SEP_ACTION_TYPE, SK_BulletEvent::SBAT_EXPLOSIVE | enmReason);
	SendToClient(pkBroadcastCenter, pkEvent, SSET_BROADCAST_TO_AREA);
}
//------------------------------------------------------------
void SK_ServerSkillEventManager::_notifyState(LC_ActorBase* pkSrcActor, LC_ActorBase* pkDstActor, SK_SkillEvent* pkEvent)
{
	// event src and dst is same
	if ( pkSrcActor == pkDstActor && pkDstActor != NULL )
	{
		LC_ActorStateMap* pkStateMap = pkSrcActor->GetStateMap();
		if (NULL != pkStateMap)
		{
			pkStateMap->NotifySkillEventToStateMap(pkEvent, false);
		}
		LC_ActorStateMap* pkControllerStateMap = pkSrcActor->GetControllerStateMap();
		if (NULL != pkControllerStateMap)
		{
			pkControllerStateMap->NotifySkillEventToStateMap(pkEvent, false, pkSrcActor);
		}
	}
	else
	{
		if (NULL != pkSrcActor)
		{
			LC_ActorStateMap* pkStateMap = pkSrcActor->GetStateMap();
			if (NULL != pkStateMap)
			{
				pkStateMap->NotifySkillEventToStateMap(pkEvent, false);
			}
			LC_ActorStateMap* pkControllerStateMap = pkSrcActor->GetControllerStateMap();
			if (NULL != pkControllerStateMap)
			{
				pkControllerStateMap->NotifySkillEventToStateMap(pkEvent, false, pkSrcActor);
			}
		}
		if (NULL != pkDstActor)
		{
			LC_ActorStateMap* pkStateMap = pkDstActor->GetStateMap();
			if (NULL != pkStateMap)
			{
				pkStateMap->NotifySkillEventToStateMap(pkEvent, false);
			}
			LC_ActorStateMap* pkControllerStateMap = pkDstActor->GetControllerStateMap();
			if (NULL != pkControllerStateMap)
			{
				pkControllerStateMap->NotifySkillEventToStateMap(pkEvent, false, pkDstActor);
			}
		}
	}
}
//------------------------------------------------------------
void SK_ServerSkillEventManager::_notifyController(LC_ActorBase* pkSrcActor, LC_ActorBase* pkDstActor, SK_SkillEvent* pkEvent)
{
	if (NULL != pkSrcActor)
	{
		pkSrcActor->NotifySkillEvent(pkEvent);
	}
	if (NULL != pkDstActor)
	{
		pkDstActor->NotifySkillEvent(pkEvent);
	}
}
//------------------------------------------------------------