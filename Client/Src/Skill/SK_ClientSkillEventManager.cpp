#include "ClientPCH.h"

#include "SK_ClientSkillEventManager.h"
#include "LC_ClientLogicManager.h"
#include "LC_ClientWorldManager.h"
#include "LC_ClientNetPlayerManager.h"
#include "LC_ClientGamePlayer.h"
#include "LC_ClientNetPlayer.h"
#include "LC_ClientNPC.h"

#include "LC_ClientNPCManager.h"

#include "FX_ClientFxManager.h"

#include "NW_ClientSessionManager.h"
#include "NW_Game2ClientSession.h"
#include "NW_Helper.h"

#include "GD_ClientActorModel.h"
#include "IM_Manager.h"

#include "MG_MessageBase.h"

#include "AS_ClientSoundManager.h"

#include "UT_LogicInfoMessage.h"

#include "GD_ClientBulletManager.h"

#include "SE_SceneManager.h"

#include "CF_ItemAvartarFunc.h"
#include "CF_ItemList.h"

#include "CA_CameraManager.h"
#include "UT_MathBase.h"

#include "UI_UIManager.h"

#include "CF_SoundSequence.h"
#include "CF_GfxList.h"
#include "CF_SkillBulletFuncRandom.h"

#include "LC_ClientPlaceHolderManager.h"
#include "LC_ClientPlaceHolder.h"
#include "GD_ClientChestModel.h"

#include "LC_CommonStatAsset.h"

#include "SC_ClientScriptInterface.h"
#include "CL_RichTextParse.h"
#include "LC_ClientArenaPlayBackManager.h"

using namespace GameLogic;
using namespace Skill;
using namespace Utility;
using namespace Protocol;
using namespace Data;
using namespace CSVFile;
using namespace StateMachine;
//------------------------------------------------------------
SK_ClientSkillEventManager::SK_ClientSkillEventManager()
{
	m_kMainEventList.reserve(64);
	m_kEventPool.Init(MAX_CLIENT_SKILL_EVENT_POOL_SIZE, 50);
}
//------------------------------------------------------------
SK_ClientSkillEventManager::~SK_ClientSkillEventManager()
{
	SK_SkillEventListIter iter;
	for (iter = m_kMainEventList.begin(); iter != m_kMainEventList.end(); ++iter)
	{
		_freeEvent(*iter);
	}
	m_kMainEventList.clear();
}
//------------------------------------------------------------
void SK_ClientSkillEventManager::Update( float fCurrentTime, float fDeltaTime )
{
	T_PROFILE("Update::SK_ClientSkillEventManager");

	int iProcEventCount = m_kMainEventList.size();

	//����һ�����������¼�
	for (int i = 0; i < iProcEventCount; i ++)
	{
		SK_SkillEvent* pkEvent = m_kMainEventList[i];
		bool bDestroyEvent = true;
		_procEvent(fCurrentTime, pkEvent, bDestroyEvent);

		//�����¼�Ϊ����״̬or�¼���ʱ���������¼�
		if (bDestroyEvent)
		{
			_freeEvent(pkEvent);
		}
		else if (fCurrentTime - pkEvent->GetStartTime() > SKILL_EVENT_UPDATE_DELTA_TIME)
		{
			GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "����[%d]�����¼���ʱ", pkEvent->GetEventType());

			_freeEvent(pkEvent);
		}
		else	//�¼�δ�������ȴ��´δ���
		{
			_addEvent(pkEvent);
		}
	}
	if (iProcEventCount > 0)
	{
		SK_SkillEventListIter itBegin = m_kMainEventList.begin();
		m_kMainEventList.erase(itBegin, itBegin + iProcEventCount);
	}
}
//------------------------------------------------------------
SK_SkillEvent* SK_ClientSkillEventManager::AllocEvent()
{
	SK_SkillEvent* pkEvent = m_kEventPool.Alloc();
	if (NULL != pkEvent)
	{
		_resetEvent(pkEvent);
	}
	return pkEvent;
}
//------------------------------------------------------------
void SK_ClientSkillEventManager::AddEvent( SK_SkillEvent* pkEvent )
{
	m_kMainEventList.push_back(pkEvent);
}
//------------------------------------------------------------
void SK_ClientSkillEventManager::ProcImmediateEvent(SK_SkillEvent* pkEvent)
{
	m_kEventPool.Free(pkEvent);
}
//------------------------------------------------------------
void SK_ClientSkillEventManager::RecvMessageFromServer(const msg_define::skill_event& kpbSkillEvent, bool bArenaPlayBack)
{
	SK_SkillEvent* pkEvent = _allocEvent();
	pkEvent->GetFromProtocol(kpbSkillEvent);
	_recvMessageFromServer(pkEvent, bArenaPlayBack);
	_freeEvent(pkEvent);
}

Skill::SK_SkillEvent* Skill::SK_ClientSkillEventManager::FindBulletEvent(long lEventID, long lBulletLocalID, object_id_type lShooterID)
{
	return _findBulletEvent(lEventID,lBulletLocalID,lShooterID);
}
//------------------------------------------------------------
void Skill::SK_ClientSkillEventManager::RecvMessageFromClient(SK_SkillEvent* pkSkillEvent)
{
	_recvMessageFromServer(pkSkillEvent);
}

void Skill::SK_ClientSkillEventManager::DeleteAllBulletEvent()
{
	SK_SkillEventList::iterator iter = m_kMainEventList.begin();
	while (iter != m_kMainEventList.end())
	{
		if ((*iter)->GetEventType() == SETE_BULLET)
		{
			iter = m_kMainEventList.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

//------------------------------------------------------------
void SK_ClientSkillEventManager::_recvMessageFromServer(SK_SkillEvent* pkEvent, bool bArenaPlayBack)
{
	T_PROFILE("Skill::SK_ClientSkillEventManager::_recvMessageFromServer");
	switch (pkEvent->GetEventType())
	{
	case SETE_RECOVER:
		_recvRecoverMessage(pkEvent, bArenaPlayBack);
		break;
	case SETE_DAMAGE:
		_recvDamageMessage(pkEvent, bArenaPlayBack);
		break;
	case SETE_ATTACH_STATE:
		_recvAttachStateMessage(pkEvent, bArenaPlayBack);
		break;
	case SETE_DETACH_STATE:
		_recvDetachStateMessage(pkEvent, bArenaPlayBack);
		break;
	case SETE_BULLET:
		{
			long lActionType = ((int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_ACTION_TYPE)) & SK_BulletEvent::SBAT_Mask;
			if (SK_BulletEvent::SBAT_SHOT == lActionType)
			{
				T_PROFILE("Skill::SK_ClientSkillEventManager::_recvBulletShotMessage");
				_recvBulletShotMessage(pkEvent, bArenaPlayBack);
			}
			else if (SK_BulletEvent::SBAT_CHANGE_TARGET_LOCATION == lActionType)
			{
				T_PROFILE("Skill::SK_ClientSkillEventManager::_recvBulletChangeTargetLocationMessage");
				_recvBulletChangeTargetLocationMessage(pkEvent, bArenaPlayBack);
			}
			else if (SK_BulletEvent::SBAT_PENETRATE == lActionType)
			{
				T_PROFILE("Skill::SK_ClientSkillEventManager::_recvBulletPenetrateMessage");
				_recvBulletPenetrateMessage(pkEvent, bArenaPlayBack);
			}
			else if (SK_BulletEvent::SBAT_EXPLOSIVE == lActionType)
			{
				T_PROFILE("Skill::SK_ClientSkillEventManager::_recvBulletExplosiveMessage");
				_recvBulletExplosiveMessage(pkEvent, bArenaPlayBack);
			}
			else
			{
				GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "���ܵ����ӵ��¼���������[%i]����", lActionType);
			}
		}
		break;
	case SETE_KILL:
		_recvKillMessage(pkEvent, bArenaPlayBack);
		break;
	case SETE_ATTACH_GFX:
		_recvAttachGfxMessage(pkEvent, bArenaPlayBack);
		break;
	case SETE_ATTACH_SOUND:
		_recvAttachSoundMessage(pkEvent, bArenaPlayBack);
		break;
	case SETE_DISPLAY_CHEST:
		_recvAttachDisplayChestMessage(pkEvent, bArenaPlayBack);
		break;
	case SETE_DETACH_GFX:
		_recvDetachGfxMessage(pkEvent, bArenaPlayBack);
		break;
	case SETE_CLIENT_EYE_SKILL:
		_procClientEyeSkillEvent(0, pkEvent, bArenaPlayBack);
		break;
	case SETE_SUMMOR_DAMAGE:
		_recvSummorDamageEvent(pkEvent);
		break;
	case SETE_IMM_PUNCH_MOVE:
		_recvImmunePunch(pkEvent, bArenaPlayBack);
		break;
	default:
		break;
	}
}
//------------------------------------------------------------
void SK_ClientSkillEventManager::_recvRecoverMessage(SK_SkillEvent* pkEvent, bool bArenaPlayBack)
{
	LC_ClientLogicManager* pkLogicManager =  CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager)
	{
		GfxWriteLog(LOG_SKILL_EVENT_FILELINE, LOG_SWITCH_SKILL_EVENT, "��ȡ�ͻ����߼�����������");
		return;
	}
	object_id_type lGamePlayerID = pkLogicManager->GetInputControlActorID();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (NULL == pkGamePlayer)
	{
		GfxWriteLog(LOG_SKILL_EVENT_FILELINE, LOG_SWITCH_SKILL_EVENT, "��ȡ��Ϸ��Ҵ���");
		return;
	}

	object_id_type lSrcActorID = pkEvent->GetSrcActorID();
	object_id_type lDstActorID = pkEvent->GetDstActorID();
	if (bArenaPlayBack)
	{
		LC_ClientArenaPlayBackManager *pkArenaPlayBackManager = LC_ClientArenaPlayBackManager::GetSingletonPtr();
		lSrcActorID = pkArenaPlayBackManager->GetTemID(lSrcActorID);
		lDstActorID = pkArenaPlayBackManager->GetTemID(lDstActorID);
	}
	long lLogicID = pkEvent->GetLogicID();
	int64_t lItemID = pkEvent->GetParam(SK_RecoverEvent::SEP_ITEM_ID);
	long lRecoverType = (int32_t)pkEvent->GetParam(SK_RecoverEvent::SEP_RECOVER_TYPE);
	int64_t lFinalRecover = pkEvent->GetParam(SK_RecoverEvent::SEP_FINAL_RECOVER);

	LC_ClientWorldManager* pkWorldManager = CLIENT_GET_WORLD_MANAGER();
	LC_ActorBase* pkSrcActor = (LC_ActorBase*)pkWorldManager->FindObject(lSrcActorID);
	LC_ActorBase* pkDstActor = (LC_ActorBase*)pkWorldManager->FindObject(lDstActorID);
	if (NULL == pkSrcActor)
	{
		GfxWriteLog(LOG_SKILL_EVENT_FILELINE, LOG_SWITCH_SKILL_EVENT, "�ظ��¼������Ҳ���������[%i]", lSrcActorID);
		return;
	}
	if (NULL == pkDstActor)
	{
		GfxWriteLog(LOG_SKILL_EVENT_FILELINE, LOG_SWITCH_SKILL_EVENT, "�ظ��¼������Ҳ���������[%i]", lDstActorID);
		return;
	}

	if(lFinalRecover > 0 && IS_COMBATRECORD_MAP(pkGamePlayer->GetMapLogicType()))
	{
		pkGamePlayer->SendMessage(CreateMsg_CombatRecord_Heal(lSrcActorID, UT_MathBase::MinInt64(pkDstActor->GetAttributeMap()->GetAttribute(ATT_MAX_HP) - pkDstActor->GetHP(), lFinalRecover)));
	}

	CA_CameraManager* pkCameraManager = CA_CameraManager::GetSingletonPtr();
	if (NULL == pkCameraManager)
	{
		return;
	}
	NiCameraPtr pkCamera = pkCameraManager->GetNiCamera();
	if (NULL == pkCamera)
	{
		return;
	}

	if (lGamePlayerID == lSrcActorID || lGamePlayerID == lDstActorID  )
	{
		if(!pkGamePlayer->GetDead())
		{
			int iRecoverUEDir = 0;
			//if (NULL != pkDstActor)
			{
				NiMatrix3 kRot(pkCamera->GetWorldRightVector(), pkCamera->GetWorldDirection(), pkCamera->GetWorldUpVector());
				NiPoint3 kPos;
				kPos.x = pkDstActor->GetForwardDir().x;
				kPos.y = pkDstActor->GetForwardDir().y;
				kPos.z = 0.0f;
				kPos.Unitize();
				kPos = kRot * kPos;
				if (kPos.x >= 0)
				{
					iRecoverUEDir = 0;
				}
				else
				{
					iRecoverUEDir = 1;
				}
			}
			//1����Ѫ��Ч�ķ������ﳯ��ķ�����
			//2��������Ч�ķ������ﳯ��
			if (SK_RecoverEvent::SRT_MP == lRecoverType)
			{
				iRecoverUEDir = 1 - iRecoverUEDir;
			}

			//��Ч
			switch (lRecoverType)
			{
			case SK_RecoverEvent::SRT_HP:
				if (lFinalRecover > 0)
				{
					pkGamePlayer->SendMessage( CreateMsg_UE_DELTA_HP(lDstActorID, lFinalRecover, iRecoverUEDir) );
				}
				break;
			case SK_RecoverEvent::SRT_MP:
				if (lFinalRecover > 0)
				{
					pkGamePlayer->SendMessage( CreateMsg_UE_DELTA_MP(lDstActorID, lFinalRecover, iRecoverUEDir) );
				}
				break;
			case SK_RecoverEvent::SRT_SP:
				if (lFinalRecover > 0)
				{
					pkGamePlayer->SendMessage( CreateMsg_UE_DELTA_SP(lDstActorID, lFinalRecover, iRecoverUEDir) );
				}
				break;
			default:
				break;
			}
		}
	}

	//ս����ʾ��Ϣ
#ifndef _SHIPPING
	bool bSelfInfo = false;
	StringType kSrcName = pkSrcActor->GetCharName();
	StringType kDstName = pkDstActor->GetCharName();
	if (lSrcActorID == lGamePlayerID)
	{
		kSrcName = CTSTR_YOU2;
		bSelfInfo = true;
	}
	if (lDstActorID == lGamePlayerID)
	{
		kDstName = CTSTR_YOU2;
		bSelfInfo = true;
	}
	if (IS_SKILL_TYPE(lLogicID))
	{
		CF_SkillExecuteList::DataEntry* pkSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(lLogicID);
		if (NULL != pkSkillData)
		{
			StringType kLogicName = pkSkillData->_kName;
			switch (lRecoverType)
			{
			case SK_RecoverEvent::SRT_HP:
				//if (bSelfInfo)SetMessageToAuxiliaryTab();
				SendLogicInfoMessage(RE_FIGHT_SKILL_HP_RECOVER, kSrcName.c_str(), kDstName.c_str(), kLogicName.c_str(), lFinalRecover);
				break;
			case SK_RecoverEvent::SRT_MP:
				//if (bSelfInfo)SetMessageToAuxiliaryTab();
				SendLogicInfoMessage(RE_FIGHT_SKILL_MP_RECOVER, kSrcName.c_str(), kDstName.c_str(), kLogicName.c_str(), lFinalRecover);
				break;
			default:
				break;
			}
		}
	}
	if (IS_STATE_TYPE(lLogicID))
	{
		CF_SkillStateList* pkStateList = CF_SkillStateList::GetSingletonPtr();
		CF_SkillStateList::DataEntry* pkStateData = pkStateList->GetEntryPtr(lLogicID);
		if (NULL != pkStateData)
		{
			StringType kLogicName = pkStateData->_kName;
			switch (lRecoverType)
			{
			case SK_RecoverEvent::SRT_HP:
				//if (bSelfInfo)SetMessageToAuxiliaryTab();
				SendLogicInfoMessage(RE_FIGHT_STATE_HP_RECOVER, kSrcName.c_str(), kLogicName.c_str(), kDstName.c_str(), lFinalRecover);
				break;
			case SK_RecoverEvent::SRT_MP:
				//if (bSelfInfo)SetMessageToAuxiliaryTab();
				SendLogicInfoMessage(RE_FIGHT_STATE_MP_RECOVER, kSrcName.c_str(), kLogicName.c_str(), kDstName.c_str(), lFinalRecover);
				break;
			default:
				break;
			}
		}
	}
#endif
	_notifyState(pkSrcActor, pkDstActor, pkEvent);
}
//------------------------------------------------------------
void SK_ClientSkillEventManager::_recvDamageMessage(SK_SkillEvent* pkEvent, bool bArenaPlayBack)
{
	LC_ClientLogicManager* pkLogicManager =  CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager)
	{
		GfxWriteLog(LOG_SKILL_EVENT_FILELINE, LOG_SWITCH_SKILL_EVENT, "��ȡ�ͻ����߼�����������");
		return;
	}
	object_id_type lGamePlayerID = pkLogicManager->GetInputControlActorID();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (NULL == pkGamePlayer)
	{
		GfxWriteLog(LOG_SKILL_EVENT_FILELINE, LOG_SWITCH_SKILL_EVENT, "��ȡ��Ϸ��Ҵ���");
		return;
	}

	LC_ClientArenaPlayBackManager *pkArenaPlayBackManager = LC_ClientArenaPlayBackManager::GetSingletonPtr();
	object_id_type lSrcActorID = pkEvent->GetSrcActorID();
	object_id_type lDstActorID = pkEvent->GetDstActorID();
	if (bArenaPlayBack)
	{
		lSrcActorID = pkArenaPlayBackManager->GetTemID(lSrcActorID);
		lDstActorID = pkArenaPlayBackManager->GetTemID(lDstActorID);
	}
	long lLogicID = pkEvent->GetLogicID();
	long lDamageType = (int32_t)pkEvent->GetParam(SK_DamageEvent::SEP_DAMAGE_TYPE);
	long lDamageResult = (int32_t)pkEvent->GetParam(SK_DamageEvent::SEP_RESULT);
	int64_t lFinalDamage = pkEvent->GetParam(SK_DamageEvent::SEP_FINAL_DAMAGE);

	LC_ClientWorldManager* pkWorldManager = CLIENT_GET_WORLD_MANAGER();
	LC_ActorBase* pkSrcActor = (LC_ActorBase*)pkWorldManager->FindObject(lSrcActorID);
	LC_ActorBase* pkDstActor = (LC_ActorBase*)pkWorldManager->FindObject(lDstActorID);

	if (NULL == pkDstActor)
	{
		GfxWriteLog(LOG_SKILL_EVENT_FILELINE, LOG_SWITCH_SKILL_EVENT, "�˺��¼������Ҳ���������[%i]", lDstActorID);
		return;
	}
	if (pkDstActor->GetDead())
	{
		return;
	}

	bool bDstNpc = IS_NPC_ID(pkDstActor->GetID());
	bool bHit = lDamageResult != SK_DamageEvent::SDR_JUMP_DODGE && lDamageResult != SK_DamageEvent::SDR_MISS && lDamageResult != SK_DamageEvent::SDR_SHIELD;
	if(bHit && lFinalDamage > 0)
	{
		if(IS_COMBATRECORD_MAP(pkGamePlayer->GetMapLogicType()))
			pkGamePlayer->SendMessage(CreateMsg_CombatRecord_Damage(lSrcActorID, lDstActorID, UT_MathBase::MinInt64(pkDstActor->GetHP(), lFinalDamage)));

		//NPC����һ����������buddy���У�������ʾ��������Ч
		//�˺������Ա���ɱ����ʾ��Ч
		if(bDstNpc && lFinalDamage < pkDstActor->GetHP()
			&& (pkSrcActor == pkGamePlayer || pkGamePlayer->IsBuddy(pkSrcActor->GetID())))
		{
			LC_ClientNPC* pkNpc = (LC_ClientNPC*)pkDstActor;
			pkNpc->AttachBeHitFx();
		}
	}


	//�ͻ��˼ٵ�Ѫ��ֻ��Ϊ��Ѫ��������ʾ��ʱ�������Ѫ������������ͬ��������
	//���Ѫ����Сֵ��1������״̬����״̬����Ҫ�ȷ�����ͬ�����ͻ��˲�������Ѫ���Լ���
	if( bDstNpc && bHit)
	{
		int64_t hpDamage = 0;
		int64_t abShied = pkEvent->GetParam(SK_DamageEvent::SEP_DAMAGE_FINAL_AB_SHIELD);
		if (abShied > 0)
		{
			hpDamage = abShied - lFinalDamage;
			hpDamage = hpDamage >= 0 ? 0 : -hpDamage;
		}
		else
		{
			hpDamage = lFinalDamage;
		}
		CF_CharType::DataEntry* sd = CF_CharType::GetSingletonPtr()->GetEntryPtr(pkDstActor->GetCharType() );
		attr_value_type deltaHP = sd->_bBoss ? UT_MathBase::MaxInt64(pkDstActor->GetHP()-lFinalDamage,1) : pkDstActor->GetHP()-lFinalDamage;
		pkDstActor->SetHP(deltaHP);
	}

	CA_CameraManager* pkCameraManager = CA_CameraManager::GetSingletonPtr();
	if (NULL == pkCameraManager)
	{
		return;
	}
	NiCameraPtr pkCamera = pkCameraManager->GetNiCamera();
	if (NULL == pkCamera)
	{
		return;
	}
	
	CF_SkillExecuteList::DataEntry* pkSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(lLogicID);

	//�ܵ�������ҵĹ���
	if (IS_PLAYER_CLASS_ID(lSrcActorID) && NULL != pkSrcActor && (bArenaPlayBack || (lGamePlayerID == lDstActorID && lDstActorID != lSrcActorID)))
	{
		_playBeAttackScreenGfx(pkSrcActor);
	}

	//�Ṧ����ֻҪ�ܵ��˺�
	if (pkGamePlayer->GetMapResID() == QINGGONG_TRIAL_MAPID && lDamageResult != SK_DamageEvent::SDR_JUMP_DODGE)
	{
		_playBeAttackScreenGfx(pkSrcActor);
	}

	if (bArenaPlayBack || lGamePlayerID == lSrcActorID || lGamePlayerID == lDstActorID)
	{

		//�����˺�������Ч����
		int iDamageUEDir = 1;
		if (NULL != pkSrcActor /*&& NULL != pkDstActor*/)
		{
			//�˺�Ʈ�֣��Լ��ܵ����˺����£������ܵ����˺�����

			NiPoint3 kAttackerWorldPos = UT_EngineMath::ConvertVec3IntToNiPoint3(pkSrcActor->GetVisibleLocation());
			float fAttackerScreenX = 0.0f;
			float fAttackerScreenY = 0.0f;
			pkCamera->WorldPtToScreenPt(kAttackerWorldPos, fAttackerScreenX, fAttackerScreenY);
			NiPoint3 kTargetWorldPos = UT_EngineMath::ConvertVec3IntToNiPoint3(pkDstActor->GetVisibleLocation());
			float fTargetScreenX = 0.0f;
			float fTargetScreenY = 0.0f;
			pkCamera->WorldPtToScreenPt(kTargetWorldPos, fTargetScreenX, fTargetScreenY);

			float fDamagePosScreenX = fAttackerScreenX - fTargetScreenX;
			float fDamagePosScreenY = fAttackerScreenY - fTargetScreenY;
			if (fDamagePosScreenX >= 0)
			{
				iDamageUEDir = 1;				//����
			}
			else
			{
				iDamageUEDir = 4;			  //����
			}
		}
		
		//�˺����ֵ����ͣ�������ɫ����Ч����
		int iDamageUEType = 0;
		switch (lDamageType)
		{
		case SDT_NORMAL:	//��ͨ�����ð�ɫ��ʾ
			iDamageUEType =  0;
			break;
		case SDT_SKILL:		//���ܹ����û�ɫ��ʾ
		case SDT_LC_SKILL:
			//��Щ���ܵ���State�����ģ�����State���洫�����SDT_SKILL�˺�������Ҫ�ж��Ƿ�����Skill�������Ƿ����
			//���������ֲ���Ҫ��ɫ��
// 			if (pkSkillData && pkSkillData->_lCategory2 == SKILL_TYPE_ARROW)	//���ڼ���������ɫ
// 				iDamageUEType = 2;
// 			else
// 			{
				iDamageUEType = 0;
				if(lDamageResult == SK_DamageEvent::SDR_CRITICAL)
				{
					iDamageUEType = 1;
				}
// 			}
			break;
		case SDT_DOT:		//DOT�˺�
		case SDT_REBOUND:	//�˺�����
			iDamageUEType = 0;
			break;
		case SDT_SHARE:		//���������˺�
			iDamageUEType =  -1;
			break;
		}


		//�����˺���Դ
		pkDstActor->SetAttackerID(lSrcActorID);
		pkDstActor->SetAttackerSkillTypeID(lLogicID);

		float resultDamage = 1.0f;
		int damageResultType = DamageElementType_1;
		if (NULL != pkSrcActor /*&& NULL != pkDstActor*/)
		{
			DamageElementType srcDamageType = pkSrcActor->GetDamageElementType();
			DamageElementType dstDamageType = pkDstActor->GetDamageElementType();
			resultDamage = LC_Helper::GetDamageElementResult(srcDamageType, dstDamageType);
		}
		if (resultDamage > 1.0f)
			damageResultType = DamageElementType_2;
		else if (resultDamage < 1.f)
			damageResultType = DamageElementType_3;

		//����ðѪ��ð����Ч
		if (IS_PLAYER_CLASS_ID(lDstActorID))
		{
			switch (lDamageResult)
			{
			case SK_DamageEvent::SDR_MISS:
				pkGamePlayer->SendMessage( CreateMsg_UE_PLAYER_MISS(lDstActorID) );
				break;
			case SK_DamageEvent::SDR_JUMP_DODGE:
				pkGamePlayer->SendMessage( CreateMsg_UE_PLAYER_JUMP_DODGE(lDstActorID, pkGamePlayer->GetMapResID()));
				break;
			case SK_DamageEvent::SDR_HIT:
				if (lFinalDamage > 0)
				{
					//�Լ��ܵ��˺�ð��Ϊ��ɫ
					if ((bArenaPlayBack && pkArenaPlayBackManager->CheckPlayerSideByID(lDstActorID)) || lGamePlayerID == lDstActorID)
					{
						//5������Ļ���£�����ר��
						pkGamePlayer->SendMessage( CreateMsg_UE_PLAYER_HP_NORMAL_COST(lDstActorID, lFinalDamage, 5, pkGamePlayer->GetMapResID(), damageResultType) );
					}
					else
					{
						pkGamePlayer->SendMessage( CreateMsg_UE_MONSTER_HP_NORMAL_COST(lDstActorID, lFinalDamage, iDamageUEDir, iDamageUEType, damageResultType) );
					}
				}
				break;
			case SK_DamageEvent::SDR_CRITICAL:
				if (lFinalDamage > 0)
				{
					//�Լ��ܵ��˺�ð��Ϊ��ɫ
					if ((bArenaPlayBack && pkArenaPlayBackManager->CheckPlayerSideByID(lDstActorID)) || lGamePlayerID == lDstActorID)
					{
						pkGamePlayer->SendMessage( CreateMsg_UE_PLAYER_HP_CRITICAL_COST(lDstActorID, lFinalDamage, iDamageUEDir, damageResultType));
					}
					else
					{
						pkGamePlayer->SendMessage( CreateMsg_UE_MONSTER_HP_CRITICAL_COST(lDstActorID, lFinalDamage, iDamageUEDir, iDamageUEType, damageResultType));
					}
				}
				break;
			}
		}
		else if (IS_NPC_ID(lDstActorID))
		{
			switch (lDamageResult)
			{
			case SK_DamageEvent::SDR_MISS:
			case SK_DamageEvent::SDR_JUMP_DODGE:
				pkGamePlayer->SendMessage( CreateMsg_UE_MONSTER_MISS(lDstActorID) );
				break;
			case SK_DamageEvent::SDR_HIT:
				if (lFinalDamage > 0 && iDamageUEType >= 0)	//���ﹲ�������˺�
				{
					pkGamePlayer->SendMessage( CreateMsg_UE_MONSTER_HP_NORMAL_COST(lDstActorID, lFinalDamage, iDamageUEDir, iDamageUEType, damageResultType) );
				}
				break;
			case SK_DamageEvent::SDR_CRITICAL:
				if (lFinalDamage > 0 && iDamageUEType >= 0)	//���ﹲ�������˺�
				{
					pkGamePlayer->SendMessage( CreateMsg_UE_MONSTER_HP_CRITICAL_COST(lDstActorID, lFinalDamage, iDamageUEDir, iDamageUEType, damageResultType));
				}
				break; 
			case SK_DamageEvent::SDR_SHIELD:
				{
					int64_t ShieldDamage = pkEvent->GetParam(SK_DamageEvent::SEP_DAMAGE_BASE);
					int64_t CurShield = pkEvent->GetParam(SK_DamageEvent::SEP_FINAL_DAMAGE);
					int64_t MaxShield = pkEvent->GetParam(SK_DamageEvent::SEP_NEUTRALIZE_SHIELD);
					if (ShieldDamage > 0 && iDamageUEType >= 0)	//���ﹲ�������˺�
					{
						pkGamePlayer->SendMessage( CreateMsg_UE_MONSTER_HP_NORMAL_COST(lDstActorID, ShieldDamage, iDamageUEDir, iDamageUEType, damageResultType) );
					}
					((LC_ClientNPC *)pkDstActor)->SetBossShield(CurShield);
					((LC_ClientNPC *)pkDstActor)->SetBossMaxShield(MaxShield);
				}
				break;
			}

			((LC_ClientNPC *)pkDstActor)->SetHitHilight();
		}
	}

	bool bIsDstBuddy = pkGamePlayer->IsBuddy(lDstActorID);

	//�����ܵ�������buddy�ܵ�����
	if ( (lGamePlayerID == lDstActorID || bIsDstBuddy) )
	{
		/*
		*���û�������κ�Ŀ�꣬���ŷ��𹥻���
		*/
		LC_MapBase* pkMap = pkGamePlayer->GetMap();
		
		long lLockedTargetID = pkGamePlayer->GetLockedTargetID();
		if ( lLockedTargetID == 0 || NULL == pkMap->FindObject(lLockedTargetID))
		{
			pkGamePlayer->WatchTarget(lSrcActorID);
		}

		//�ܵ�������ҹ������������ڹһ�ʱ���л���ȫ�幥��ģʽ������Ŀ���Ϊ�����ߡ�
		if(IS_PLAYER_CLASS_ID(lSrcActorID)
			&& pkGamePlayer->IsHooking()
			&& pkGamePlayer->GetAssistAsset().IsFlagSet(AAFB_PKSETTING) )
		{
			pkGamePlayer->SetHookingTargetID(lSrcActorID);

			int pkMode=SC_ClientScriptInterface::GetSingletonPtr()->GetSkillInterface()->GetFixPKMode(pkGamePlayer->GetMapResID());
			if (pkMode == PKM_MIN || pkMode == PKM_ALL)
			{
				LC_PKRecorderBase* pkRecorder = pkGamePlayer->GetPKRecorder();
				if(pkRecorder->GetPKMode() != PKM_ALL)
				{
					pkRecorder->SetPKMode(PKM_ALL);
					pkGamePlayer->RequestModifyPKSetting();
				}
			}
		}

		if(IS_PLAYER_CLASS_ID(lSrcActorID))
		{
			LC_ClientNetPlayer* pNetPlayer=(LC_ClientNetPlayer*)pkSrcActor;
			if (pNetPlayer)
			{
				pkGamePlayer->AddEnemyPlayer(pNetPlayer->GetCommanderUID());
			}
		}
	}

	if (bIsDstBuddy && pkSkillData != NULL)
	{
		UT_Vec2Float kDir(0, 0);

		if (pkSrcActor != NULL)
			kDir = pkSrcActor->GetForwardDir();

		NiPoint3 pkRotate = NiPoint3::ZERO;
		pkRotate.z = NiATan2(kDir.y, kDir.x);

		IM_Manager* pkIMManager =  IM_Manager::GetSingletonPtr();
		GD_ActorModel* pkVisibleData = pkIMManager->GetActor(lDstActorID);
		if (pkVisibleData != NULL)
		{
			const StringType * pkBeAttackGfx = Utility::RandomChooseValidCellText(&pkSkillData->_kBeAttackGfx, &pkSkillData->_kBeAttackGfx1, &pkSkillData->_kBeAttackGfx2);
			if (pkBeAttackGfx != NULL)
				pkVisibleData->AttachFx(GD_ModelBase::FxDummyNodeType(pkSkillData->_lBeAttackGfxBindDumy),0,true,*pkBeAttackGfx, NULL, &pkRotate);
		}
	}

	//����ǹ�����
	if ( pkGamePlayer->IsBuddy(lSrcActorID) && pkSkillData != NULL && pkSrcActor != NULL /*&& pkDstActor != NULL*/)
	{
		IM_Manager* pkIMManager =  IM_Manager::GetSingletonPtr();

		GD_ActorModel* pkVisibleData = pkIMManager->GetActor(lDstActorID);
		if(NULL == pkVisibleData)
		{
			return;
		}

		NiPoint3 pkRotate = NiPoint3::ZERO;
		Utility::UT_Vec3Int diff3 = pkDstActor->GetVisibleLocation() - pkSrcActor->GetVisibleLocation();
		pkRotate.z = NiATan2(-Int2Float(diff3.y), Int2Float(diff3.x));

		const StringType * pkBeAttackGfx = Utility::RandomChooseValidCellText(&pkSkillData->_kBeAttackGfx, &pkSkillData->_kBeAttackGfx1, &pkSkillData->_kBeAttackGfx2);
		if (pkBeAttackGfx != NULL)
		{
			GD_ModelBase::PendingAttachedFx arg(GD_ModelBase::FxDummyNodeType(pkSkillData->_lBeAttackGfxBindDumy),0,true,*pkBeAttackGfx);
			arg.SetPRS(NULL, &pkRotate, 1.0f);
			arg.bDisableCameraShakeFx = bool(lGamePlayerID != lSrcActorID);
			pkVisibleData->AttachFx(arg);
		}

		int dstState = pkDstActor->GetCurrentStateType();
		bool stateCheck = (ST_NPC_IDLE == dstState || ST_NPC_WANDER == dstState || ST_NPC_CHASE == dstState);
		if (	!IS_SKILL_TYPE(pkDstActor->GetCurrentStateType())
			&&	IS_NPC_ID(lDstActorID)
			&&  !pkDstActor->GetDead()
			/*&&	((LC_ClientNPC *)pkDstActor)->IsUnderAttackCoolDown()*/
			&&	!((LC_ClientNPC *)pkDstActor)->IsStillUnderAttack()
			&&	stateCheck
			/*&& !LC_Helper::IsBossCharType(pkDstActor->GetCharType())*/)
		{
			((LC_ClientNPC *)pkDstActor)->StartUnderAttackTimers();
			//((LC_ClientNPC *)pkDstActor)->PlayAnimation(RES_ANM_ACTOR_UNDERATTACK);
			pkVisibleData->ActivateAnimation(RES_ANM_ACTOR_UNDERATTACK);
		}
	}

	//�������Ǳ�������Ч
	if (SK_DamageEvent::SDR_CRITICAL == lDamageResult )
	{
		if ((bArenaPlayBack && pkArenaPlayBackManager->CheckPlayerSideByID(lDstActorID)) || lGamePlayerID == lDstActorID)
		{
			const CF_Buddy::DataEntry* pkData  = CF_Buddy::GetSingleton().GetEntryPtr(pkGamePlayer->GetCharType());

			if (pkData != NULL && !pkData->_strKeyName.empty())
			{
				AS_ClientSoundManager* pkSoundManager = AS_ClientSoundManager::GetSingletonPtr();
				NiPoint3 kPos = UT_EngineMath::ConvertVec3IntToNiPoint3( pkDstActor->GetVisibleLocation() );
				pkSoundManager->Play3DEx(kPos , pkData->_strKeyName , 
					AS_ClientSoundManager::SCT_PLAYER_SKILL_EVENT );
			}
		}
		else
		{
			//TODO NPC�ܱ�����Ч
		}
	}

	if (NULL == pkSrcActor/* || NULL == pkDstActor*/)
	{
		return;
	}

	if ( (SK_DamageEvent::SDR_MISS != lDamageResult && SK_DamageEvent::SDR_JUMP_DODGE != lDamageResult)  
		&& (lDamageType == SDT_NORMAL || lDamageType == SDT_SKILL || lDamageType == SDT_LC_SKILL ) )
	{
		if (pkSkillData && SC_ClientSkillInterface::CheckAttachSkillSound(pkGamePlayer, pkSrcActor))
		{
			_playHitSound(pkSrcActor, pkDstActor, pkSkillData);
		}
	}


	StringType kDamageType = DAMAGE_TYPE_UNKNOWN;
	switch (lDamageType)
	{
	case SDT_NORMAL:
		kDamageType = DAMAGE_TYPE_NORMAL;
		break;
	case SDT_SKILL:
	case SDT_LC_SKILL:
		kDamageType = DAMAGE_TYPE_SKILL;
		break;
	case SDT_DOT:
		kDamageType = DAMAGE_TYPE_DOT;
		break;
	case SDT_REBOUND:
		kDamageType = DAMAGE_TYPE_REBOUND;
		break;
	case SDT_SHARE:
		kDamageType = DAMAGE_TYPE_SHARE;
		break;
	}
	
	/*
	*ս����ʾ��Ϣ
	*/
#ifndef _SHIPPING
	StringType kSrcName = pkSrcActor->GetCharName();
	StringType kDstName = pkDstActor->GetCharName();
	if (IS_PLAYER_CLASS_ID(lSrcActorID))
	{
		kSrcName = ((LC_PlayerBase*)pkSrcActor)->GetCommanderName() + "-" + kSrcName;
	}
	if (IS_PLAYER_CLASS_ID(lDstActorID))
	{
		kDstName = ((LC_PlayerBase*)pkDstActor)->GetCommanderName() + "-" + kDstName;
	}

	bool bSelfInfo = false;
	if ((bArenaPlayBack && pkArenaPlayBackManager->CheckPlayerSideByID(lSrcActorID)) || lGamePlayerID == lSrcActorID)
	{
		kSrcName = CTSTR_YOU2;
		bSelfInfo = true;
	}
	if ((bArenaPlayBack && pkArenaPlayBackManager->CheckPlayerSideByID(lDstActorID)) || lGamePlayerID == lDstActorID)
	{
		kDstName = CTSTR_YOU2;
		bSelfInfo = true;
	}

	if (!IS_SKILL_TYPE(lLogicID) && !IS_STATE_TYPE(lLogicID))
	{
		switch (lDamageResult)
		{
		case SK_DamageEvent::SDR_HIT:
			if (lFinalDamage > 0)
			{
				//if (bSelfInfo)SetMessageToAuxiliaryTab();
				SendLogicInfoMessage(RE_FIGHT_DAMAGE, kSrcName.c_str(), kDstName.c_str(), lFinalDamage, kDamageType.c_str());
			}
			break;
		case SK_DamageEvent::SDR_CRITICAL:
			if (lFinalDamage > 0)
			{
				//if (bSelfInfo)SetMessageToAuxiliaryTab();
				SendLogicInfoMessage(RE_FIGHT_CRITICAL_DAMAGE, kSrcName.c_str(), kDstName.c_str(), lFinalDamage, kDamageType.c_str());
			}
			break;
		default:
			break;
		}
	}
	else
	{
		StringType kLogicName = "";
		
		if (IS_SKILL_TYPE(lLogicID) && NULL != pkSkillData)
		{
			kLogicName = pkSkillData->_kName;
			switch (lDamageResult)
			{
			case SK_DamageEvent::SDR_MISS:
			case SK_DamageEvent::SDR_JUMP_DODGE:
				//if (bSelfInfo)SetMessageToAuxiliaryTab();
				if (lSrcActorID != lGamePlayerID)
				{
					SendLogicInfoMessage(RE_FIGHT_SKILL_MISS, kSrcName.c_str(), kDstName.c_str(), kLogicName.c_str());
				}
				break;
			case SK_DamageEvent::SDR_HIT:
				if (lFinalDamage > 0)
				{
					//if (bSelfInfo)SetMessageToAuxiliaryTab();
					SendLogicInfoMessage(RE_FIGHT_SKILL_DAMAGE, kSrcName.c_str(), kDstName.c_str(), kLogicName.c_str(), lFinalDamage, kDamageType.c_str());
				}
				break;
			case SK_DamageEvent::SDR_CRITICAL:
				if (lFinalDamage > 0)
				{
					//if (bSelfInfo)SetMessageToAuxiliaryTab();
					SendLogicInfoMessage(RE_FIGHT_SKILL_CRITICAL_DAMAGE, kSrcName.c_str(), kDstName.c_str(), kLogicName.c_str(), lFinalDamage, kDamageType.c_str());
				}
				break;
			default:
				break;
			}
		}
		if (IS_STATE_TYPE(lLogicID))
		{
			CF_SkillStateList* pkStateList = CF_SkillStateList::GetSingletonPtr();
			CF_SkillStateList::DataEntry* pkStateData = pkStateList->GetEntryPtr(lLogicID);
			if (NULL != pkStateData)
			{
				kLogicName = pkStateData->_kName;
				//if (bSelfInfo)SetMessageToAuxiliaryTab();
				SendLogicInfoMessage(RE_FIGHT_STATE_DAMAGE, kSrcName.c_str(), kLogicName.c_str(), kDstName.c_str(), lFinalDamage, kDamageType.c_str());
			}
		}
	}


	if (lGamePlayerID == lSrcActorID)
	{
		/*
		*������ʾ��Ϣ
		*/
		switch (lDamageResult)
		{
		case SK_DamageEvent::SDR_MISS:
		case SK_DamageEvent::SDR_JUMP_DODGE:
			{
				if (NULL != pkSkillData)
				{
					StringType kSkillName = pkSkillData->_kName;
					if (LC_Helper::GetNormalAttackSkill(pkGamePlayer) != lLogicID)
					{
						kSkillName = kSkillName + CTSTR_SKILL;
					}
					//if (bSelfInfo)SetMessageToAuxiliaryTab();
					SendLogicInfoMessage(RE_SKILL_MISS, kSkillName.c_str());
				}
			}
			break;
		default:
// 			if (IS_NPC_ID(lDstActorID) && lDamageType != SDT_DOT)
// 				pkGamePlayer->SummorCombo(1, GET_PROCESS_TIME);
			break;
		}
	}
#endif

	_notifyState(pkSrcActor, pkDstActor, pkEvent);
}
//------------------------------------------------------------
void SK_ClientSkillEventManager::_recvAttachStateMessage(SK_SkillEvent* pkEvent, bool bArenaPlayBack)
{
	long lSrcActorID = pkEvent->GetSrcActorID();
	long lDstActorID = pkEvent->GetDstActorID();
	if (bArenaPlayBack)
	{
		LC_ClientArenaPlayBackManager *pkArenaPlayBackManager = LC_ClientArenaPlayBackManager::GetSingletonPtr();
		lSrcActorID = pkArenaPlayBackManager->GetTemID(lSrcActorID);
		lDstActorID = pkArenaPlayBackManager->GetTemID(lDstActorID);
	}
	long lLogicID = pkEvent->GetLogicID();
	object_id_type lAttachStateID = (object_id_type)pkEvent->GetParam(SK_AttachStateEvent::SEP_STATE_ID);
	long lAttachResult = (int32_t)pkEvent->GetParam(SK_AttachStateEvent::SEP_RESULT);

	LC_ClientLogicManager* pkLogicManager =  CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager)
	{
		GfxWriteLog(LOG_SKILL_EVENT_FILELINE, LOG_SWITCH_SKILL_EVENT, "��ȡ�ͻ����߼�����������");
		return;
	}
	object_id_type lGamePlayerID = pkLogicManager->GetInputControlActorID();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();

	LC_ClientWorldManager* pkWorldManager = CLIENT_GET_WORLD_MANAGER();
	LC_ActorBase* pkSrcActor = (LC_ActorBase*)pkWorldManager->FindObject(lSrcActorID);
	LC_ActorBase* pkDstActor = (LC_ActorBase*)pkWorldManager->FindObject(lDstActorID);
	if (NULL == pkSrcActor)
	{
		GfxWriteLog(LOG_SKILL_EVENT_FILELINE, LOG_SWITCH_SKILL_EVENT, "����״̬�¼������Ҳ���������[%i]", lSrcActorID);
		return;
	}
	if (NULL == pkDstActor)
	{
		GfxWriteLog(LOG_SKILL_EVENT_FILELINE, LOG_SWITCH_SKILL_EVENT, "����״̬�¼������Ҳ���������[%i]", lDstActorID);
		return;
	}
	if (pkDstActor->GetDead() || pkDstActor->GetDying())
	{
		return;
	}

	CF_SkillStateList* pkStateList = CF_SkillStateList::GetSingletonPtr();
	if (!IS_STATE_TYPE(lAttachStateID))
	{
		GfxWriteLog(LOG_SKILL_EVENT_FILELINE, LOG_SWITCH_SKILL_EVENT, "����״̬�¼������Ҳ�������״̬[%i]", lAttachStateID);
		return;
	}
	CF_SkillStateList::DataEntry* pkStateData = pkStateList->GetEntryPtr(lAttachStateID);
	if (NULL == pkStateData)
	{
		GfxWriteLog(LOG_SKILL_EVENT_FILELINE, LOG_SWITCH_SKILL_EVENT, "����״̬�¼������Ҳ�������״̬[%i]", lAttachStateID);
		return;
	}

	//�����debuff��������Ļ��Ч
	if (2 == pkStateData->_lBuffType && IS_PLAYER_CLASS_ID(lSrcActorID) && lDstActorID == lGamePlayerID  && lDstActorID != lSrcActorID)
	{
		_playBeAttackScreenGfx(pkSrcActor);
	}

	StringType kAttachStateName = pkStateData->_kName;
	switch (lAttachResult)
	{
	case SK_AttachStateEvent::SASR_SKILLFLAG_EXCLUDE:
	case SK_AttachStateEvent::SASR_STATE_EXCLUDE:
		{
			//modify by 
			if (lDstActorID == lGamePlayerID)
			{
				SendLogicInfoMessage(RE_SKILL_STATE_ADD_FAIL_EXCLUDE, kAttachStateName.c_str());

				//���⴦��������ʾ
				if (pkStateData->_kExcluteTipsData.x != 0 )
				{
					//״̬ȡ������ӣ��ų����⴦������������UI��ʾ
					StringStreamType sStream;
					sStream << kAttachStateName<<"#"<<" "<<"#"<<pkStateData->_kExcluteTipsData.x;
					UI_UIManager::GetSingletonPtr()->HandlerMsgGameEvent(GLET_CLIENT_COURAGE_EXTRATIPS, sStream.str());
				}
			}
			
			if (NULL != pkGamePlayer && /*pkDstActor != NULL &&*/ pkDstActor->CheckStateFxTime(302))
			{
				//if (pkGamePlayer->IsBuddy(lSrcActorID)
				//	|| pkGamePlayer->IsBuddy(lDstActorID)
				//	|| pkGamePlayer->IsNpcBelongToBuddy(pkSrcActor))
				//{
					pkGamePlayer->SendMessage( CreateMsg_UE_PLAYER_STATE_CHANGE(lDstActorID, 302) );
				//}
			}
		}
		break;
	case SK_AttachStateEvent::SASR_STATE_NEUTRA:
		{
			if (lDstActorID == lGamePlayerID)
			{
				long lNeutraStateID = (int32_t)pkEvent->GetParam(SK_AttachStateEvent::SEP_OLD_STATE_0);
				if (!IS_STATE_TYPE(lNeutraStateID))
				{
					GfxWriteLog(LOG_SKILL_EVENT_FILELINE, LOG_SWITCH_SKILL_EVENT, "����״̬�¼������Ҳ����к͵ļ���״̬[%i]", lNeutraStateID);
					return;
				}
				pkStateData = pkStateList->GetEntryPtr(lNeutraStateID);
				if (NULL == pkStateData)
				{
					GfxWriteLog(LOG_SKILL_EVENT_FILELINE, LOG_SWITCH_SKILL_EVENT, "����״̬�¼������Ҳ����к͵ļ���״̬[%i]", lNeutraStateID);
					return;
				}
				StringType kNeutraStateName = pkStateData->_kName;
				SendLogicInfoMessage(RE_SKILL_STATE_ADD_FAIL_NEUTRA, kAttachStateName.c_str(), kNeutraStateName.c_str());\
			}
		}
		break;
	case SK_AttachStateEvent::SASR_STATE_REPLACE:
		{
			if (lDstActorID == lGamePlayerID)
			{
				int iParamBegin = SK_AttachStateEvent::SEP_OLD_STATE_BEGIN;
				int iParamEnd = SK_AttachStateEvent::SEP_OLD_STATE_END;
				StringType kReplaceStateName = "";
				for (int i = iParamBegin + 1; i <= iParamEnd; i++)
				{
					long lReplaceStateID = (int32_t)pkEvent->GetParam(i);
					if (!IS_STATE_TYPE(lReplaceStateID))
					{
						continue;
					}
					CF_SkillStateList::DataEntry* pkReplaceStateData = pkStateList->GetEntryPtr(lReplaceStateID);
					if (NULL == pkReplaceStateData)
					{
						continue;
					}
					kReplaceStateName = kReplaceStateName + pkReplaceStateData->_kName + ",";
				}
				if ("" == kReplaceStateName)
				{
					GfxWriteLog(LOG_SKILL_EVENT_FILELINE, LOG_SWITCH_SKILL_EVENT, "����״̬�¼������Ҳ�����[%i]�滻�ļ���״̬", lAttachStateID);
					return;
				}
				//ȡ�����Ķ���
				kReplaceStateName = kReplaceStateName.substr(0, kReplaceStateName.length() - 1);

				//modify by lxm 2014.07.05 buff��ȡ��Ч��,����ʾ��22����(ս����Ϣ)
				//SendLogicInfoMessage(RE_SKILL_STATE_ADD_REPLACE, kAttachStateName.c_str(), kReplaceStateName.c_str());
				
				//���⴦��������ʾ
				if (pkStateData->_kReplaceTipsData.x != 0 )
				{
					StringStreamType sStream;
					sStream << kAttachStateName<<"#"<<kReplaceStateName<<"#"<<pkStateData->_kReplaceTipsData.x;
					UI_UIManager::GetSingletonPtr()->HandlerMsgGameEvent(GLET_CLIENT_COURAGE_EXTRATIPS, sStream.str());

				}
			}
		}
		break;
	case SK_AttachStateEvent::SASR_SUCCESS:
		{
			if (lDstActorID == lGamePlayerID)
			{
				bool bSelfInfo = false;
				StringType kDstName = pkDstActor->GetCharName();
				
				kDstName = CTSTR_YOU2;
				bSelfInfo = true;

				/*switch (lAttachStateID)
				{
				case STATE_STATE_RECOVER_HP:
				case STATE_INDUCE_RECOVER_HP:
					SendLogicInfoMessage(RE_SKILL_STATE_ADD_RECOVER_HP, kDstName.c_str());
					break;
				case STATE_STATE_RECOVER_MP:
				case STATE_INDUCE_RECOVER_MP:
					SendLogicInfoMessage(RE_SKILL_STATE_ADD_RECOVER_MP, kDstName.c_str());
					break;
				default:
					SendLogicInfoMessage(RE_FIGHT_ATTACH_STATE, kDstName.c_str(), kAttachStateName.c_str());
					break;
				}*/
				//if (bSelfInfo)SetMessageToAuxiliaryTab();
				
				//modify by lxm ���buff�ɹ�������ʾ��22����(ս����Ϣ)2014.07.05
				//SendLogicInfoMessage(RE_FIGHT_ATTACH_STATE, kDstName.c_str(), kAttachStateName.c_str());

				//���⴦��������ʾ
				if (pkStateData->_kNormalAttachTipsData.x != 0 )
				{
					StringStreamType sStream;
					sStream << kAttachStateName<<"#"<<" "<<"#"<<pkStateData->_kNormalAttachTipsData.x;
					UI_UIManager::GetSingletonPtr()->HandlerMsgGameEvent(GLET_CLIENT_COURAGE_EXTRATIPS, sStream.str());
				}

				LC_GameEvent kGameEvent;
				kGameEvent.SetEventType(GLET_CLIENT_ADDSTATE_TOUI);
				kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, lAttachStateID);
				UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
				pkUIManager->HandlerGameEvent(kGameEvent);
			}

			if (NULL != pkGamePlayer && /*pkDstActor != NULL &&*/ pkDstActor->CheckStateFxTime(pkStateData->_lCustomType1))
			{
				//if (pkGamePlayer->IsBuddy(lSrcActorID)
				//	|| pkGamePlayer->IsBuddy(lDstActorID)
				//	|| pkGamePlayer->IsNpcBelongToBuddy(pkSrcActor))
				//{
					pkGamePlayer->SendMessage( CreateMsg_UE_PLAYER_STATE_CHANGE(lDstActorID, pkStateData->_lCustomType1) );
				//}
			}
		}
		break;
	default:
		break;
	}

	_notifyState(pkSrcActor, pkDstActor, pkEvent);
}
//------------------------------------------------------------
void SK_ClientSkillEventManager::_recvDetachStateMessage(SK_SkillEvent* pkEvent, bool bArenaPlayBack)
{
	long lSrcActorID = pkEvent->GetSrcActorID();
	long lDstActorID = pkEvent->GetDstActorID();
	if (bArenaPlayBack)
	{
		LC_ClientArenaPlayBackManager *pkArenaPlayBackManager = LC_ClientArenaPlayBackManager::GetSingletonPtr();
		lSrcActorID = pkArenaPlayBackManager->GetTemID(lSrcActorID);
		lDstActorID = pkArenaPlayBackManager->GetTemID(lDstActorID);
	}
	long lLogicID = pkEvent->GetLogicID();
	long lDetachType = (int32_t)pkEvent->GetParam(SK_DetachStateEvent::SEP_DETACH_TYPE);
	long lStateID = (int32_t)pkEvent->GetParam(SK_DetachStateEvent::SEP_STATE_ID);
	long lDetachCount = (int32_t)pkEvent->GetParam(SK_DetachStateEvent::SEP_DETACH_COUNT);

	LC_ClientLogicManager* pkLogicManager =  CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager)
	{
		GfxWriteLog(LOG_SKILL_EVENT_FILELINE, LOG_SWITCH_SKILL_EVENT, "��ȡ�ͻ����߼�����������");
		return;
	}
	object_id_type lGamePlayerID = pkLogicManager->GetInputControlActorID();
	
	LC_ClientWorldManager* pkWorldManager = CLIENT_GET_WORLD_MANAGER();
	LC_ActorBase* pkSrcActor = (LC_ActorBase*)pkWorldManager->FindObject(lSrcActorID);
	LC_ActorBase* pkDstActor = (LC_ActorBase*)pkWorldManager->FindObject(lDstActorID);
	if (NULL == pkSrcActor)
	{
		GfxWriteLog(LOG_SKILL_EVENT_FILELINE, LOG_SWITCH_SKILL_EVENT, "����״̬�¼������Ҳ���������[%i]", lSrcActorID);
		return;
	}
	if (NULL == pkDstActor)
	{
		GfxWriteLog(LOG_SKILL_EVENT_FILELINE, LOG_SWITCH_SKILL_EVENT, "����״̬�¼������Ҳ���������[%i]", lDstActorID);
		return;
	}

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_CLIENT_DETACHSTATE_TOUI);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, lStateID);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);


	// modify by lxm buff����ɢ����ʾ����ʾ����22����(ս����Ϣ)��
	
	//ս����ʾ��Ϣ
// 	if (IS_STATE_TYPE(lStateID))
// 	{
// 		CF_SkillStateList* pkStateList = CF_SkillStateList::GetSingletonPtr();
// 		CF_SkillStateList::DataEntry* pkStateData = pkStateList->GetEntryPtr(lStateID);
// 		if (NULL != pkStateData)
// 		{
// 			bool bSelfInfo = false;
// 			StringType kSrcName = "";
// 			StringType kDstName = "";
// 			if (lSrcActorID == lGamePlayerID)
// 			{
// 				kSrcName = CTSTR_YOU2;
// 				bSelfInfo = true;
// 			}
// 			else
// 			{
// 				kSrcName = pkSrcActor->GetCharName();
// 			}
// 			if (lDstActorID == lGamePlayerID)
// 			{
// 				kDstName = CTSTR_YOU2;
// 				bSelfInfo = true;
// 			}
// 			else
// 			{
// 				kDstName = pkDstActor->GetCharName();
// 			}
// 
// 			/*switch (lLogicID)
// 			{
// 			case STATE_STATE_RECOVER_HP:
// 			case STATE_INDUCE_RECOVER_HP:
// 				SendLogicInfoMessage(RE_SKILL_STATE_REMOVE_RECOVER_HP, kDstName.c_str());
// 				break;
// 			case STATE_STATE_RECOVER_MP:
// 			case STATE_INDUCE_RECOVER_MP:
// 				SendLogicInfoMessage(RE_SKILL_STATE_REMOVE_RECOVER_MP, kDstName.c_str());
// 				break;
// 			default:
// 				SendLogicInfoMessage(RE_FIGHT_DETACH_STATE, kDstName.c_str(), pkStateData->_kName.c_str(), kSrcName.c_str());
// 				break;
// 			}*/
// 
// 			//if (bSelfInfo)SetMessageToAuxiliaryTab();
// 			//�һ����⴦��
// 			if(SKILL_STATE_HOOK == lStateID)
// 				SendLogicInfoMessage(RE_HOOK_STATE_INTERRUPT, kDstName.c_str());
// 			else
// 				SendLogicInfoMessage(RE_FIGHT_DETACH_STATE, kDstName.c_str(), pkStateData->_kName.c_str(), kSrcName.c_str());
// 		}
// 	}

	_notifyState(pkSrcActor, pkDstActor, pkEvent);
}
//------------------------------------------------------------
static void init_bullet_arg_with_skill(GD_BulletCreateArg & kArg_, Skill::SK_SkillExecute* pkSkillExecute, int lBulletFuncID_, float & fPitchAngle)
{
	const CSVFile::CF_SkillBulletFunc::DataEntry * pkBulletData = pkSkillExecute->GetBulletDataEntryPtr(lBulletFuncID_);

	// visible data
	//long kExplosiveFxID		= pkSkillExecute->GetProperty_BulletExplosiveFX(lBulletFuncID_);
	long kDropWarningFxID	= pkSkillExecute->GetProperty_BulletDropWarningFX();
	long kLinkFxID			= pkSkillExecute->GetProperty_BulletPenetrateLinkFX();

	//CF_GfxList::DataEntry *pGfxEntry1 = CF_GfxList::GetSingletonPtr()->GetEntryPtr(kExplosiveFxID);
	CF_GfxList::DataEntry *pGfxEntry2 = CF_GfxList::GetSingletonPtr()->GetEntryPtr(kDropWarningFxID);
	CF_GfxList::DataEntry *pGfxEntry3 = CF_GfxList::GetSingletonPtr()->GetEntryPtr(kLinkFxID);

	kArg_.strFlyingFxName		= pkSkillExecute->GetProperty_BulletMoveFXPath(lBulletFuncID_);
	kArg_.strExplosionFxName	= pkSkillExecute->GetProperty_BulletExplosiveFX(lBulletFuncID_);
	kArg_.strDropWarningFxName	= pGfxEntry2->_kName;
	kArg_.strLinkFxName			= pGfxEntry3->_kName;	// "C_��ѧ_��������_����1.gfx";
	kArg_.bLinkTargetsWithFx	= pkBulletData->_bLinkTargetsWithFx;

	// missile data
	kArg_.fRollAngle			= pkBulletData->_fRollAngle;
	kArg_.fCtrlPercentage		= pkBulletData->_fTurningPercentage;
	kArg_.fTurningHeight		= pkBulletData->_fTurningHeight;
	kArg_.fHoverTime			= pkBulletData->_fBulletHoverTime;

	if (pkBulletData->_bRandomParam) 
	{
		const CF_SkillBulletFuncRandom::DataEntry * bulletRandomEntry =  CF_SkillBulletFuncRandom::GetSingletonPtr()->GetEntryPtr(pkBulletData->_iID);
		if (bulletRandomEntry) 
		{
			kArg_.fRollAngle		= UT_MathBase::RandInRangeFloat(bulletRandomEntry->_fRollAngleMin, bulletRandomEntry->_fRollAngleMax);
			fPitchAngle				= UT_MathBase::RandInRangeFloat(bulletRandomEntry->_fPitchAngleMin, bulletRandomEntry->_fPitchAngleMax);
			kArg_.fCtrlPercentage	= UT_MathBase::RandInRangeFloat(bulletRandomEntry->_fTurningPercentageMin, bulletRandomEntry->_fTurningPercentageMax);
			kArg_.fTurningHeight	= UT_MathBase::RandInRangeInt(bulletRandomEntry->_fTurningHeightMin, bulletRandomEntry->_fTurningHeightMax);
		}
	}
}
//------------------------------------------------------------
static bool is_gameplayer_id(object_id_type lAttackerID, LC_ClientGamePlayer* pkGamePlayer)
{
	return (pkGamePlayer->GetID() == lAttackerID);
}

void SK_ClientSkillEventManager::_recvBulletShotMessage(SK_SkillEvent* pkEvent, bool bArenaPlayBack)
{
	// ��������ӿڵ�4���ӵ���
	//	  �ͻ��˷���1�֣�
	//		1. GamePlayer�ģ�lBulletLocalID != 0, lEventID == 0
	//	  ����������3�֣�
	//		1. NPC�ģ� lBulletLocalID == 0��lEventID����ʲ�Ϊ0
	//		2. NetPlayer�ģ� lBulletLocalID != 0��lEventID����ʲ�Ϊ0
	//		3. GamePlayer�ģ�������Ӧ�ͻ�����Ϣ�� lBulletLocalID != 0��lEventID����ʲ�Ϊ0
	//	  ��������ӵ����л�GamePlayer�����������ʱ������

	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (!pkGamePlayer)
		return;

	long lEventID = pkEvent->GetEventID();
	object_id_type lAttackerID = pkEvent->GetSrcActorID();
	object_id_type lDstActorID = pkEvent->GetDstActorID();
	if (bArenaPlayBack)
	{
		LC_ClientArenaPlayBackManager *pkArenaPlayBackManager = LC_ClientArenaPlayBackManager::GetSingletonPtr();
		lAttackerID = pkArenaPlayBackManager->GetTemID(lAttackerID);
		lDstActorID = pkArenaPlayBackManager->GetTemID(lDstActorID);
	}
	long lSkillID = pkEvent->GetLogicID();
	long lMoveType = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_MOVE_TYPE);
	long lDummyType = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_DUMMY_TYPE);
	long lBulletFuncID = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_BULLET_FUNC);
	float fMoveLifeTime = SK_BulletEvent::ParamLLong2Float(pkEvent->GetParam(SK_BulletEvent::SEP_MOVE_LIFE_TIME));
	float fPitchAngle = SK_BulletEvent::ParamLLong2Float(pkEvent->GetParam(SK_BulletEvent::SEP_PITCH_ANGLE));
	long  lBulletLocalID = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_BULLET_LOCAL_ID);
	bool bShotFromClient = (((int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_ACTION_TYPE)) & SK_BulletEvent::SBSS_Mask) == SK_BulletEvent::SBSS_FromClient;
	
	LC_ClientWorldManager* pkWorldManager = CLIENT_GET_WORLD_MANAGER();
	LC_ActorBase* pkAttacker = (LC_ActorBase*)pkWorldManager->FindObject(lAttackerID);
	if (NULL == pkAttacker)
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "�Ҳ����ͷ��ӵ��Ľ�ɫ[%d]", lAttackerID);
		return;
	}

	SK_Factory* pkSkillFactory = SK_Factory::GetSingletonPtr();
	SK_SkillExecute* pkSkillExecute = pkSkillFactory->RequestSkillExcute(lSkillID);
	if (NULL == pkSkillExecute)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "��ȡ�ͷ��ӵ��ļ���[%d]����", lSkillID);
		return;
	}

	if(pkSkillExecute->GetProperty_IsLCSkill())
	{
		LC_PlayerBase* pkPlayer = (LC_PlayerBase*)pkWorldManager->FindObject(lAttackerID);
		if(pkPlayer)
		{
			object_id_type lPlayerID = lAttackerID;
			lAttackerID = pkPlayer->GetLastlcId();
			LC_ActorBase* pkAttacker = (LC_ActorBase*)pkWorldManager->FindObject(lAttackerID);
			if (NULL == pkAttacker)
			{
				GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "�Ҳ����ͷ��ӵ�����߽�ɫ ��ɫ[%d] ���[%d]", lPlayerID, lAttackerID);
				return;
			}
		}
	}	
	

	/*LC_ActorBase* pkTarget = (LC_ActorBase*)pkWorldManager->FindObject(lTargetID);
	if (NULL == pkTarget)
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "�Ҳ����ӵ���Ŀ��[%d]", lTargetID);
		return;
	}*/


	

	//�����ӵ�����
	float fCurrentTime = GET_PROCESS_TIME;
	pkEvent->SetParam(SK_BulletEvent::SEP_MOVE_START_TIME, SK_BulletEvent::ParamFloat2LLong(fCurrentTime));
	pkEvent->SetParam(SK_BulletEvent::SEP_MOVE_LIFE_TIME, SK_BulletEvent::ParamFloat2LLong(fMoveLifeTime));
	pkEvent->SetParam(SK_BulletEvent::SEP_LAST_UPDATE_TIME, SK_BulletEvent::ParamFloat2LLong(fCurrentTime));
	
	//��ӵ��¼��б�
	if (   ((lBulletLocalID == 0)			// NPC��
		|| bShotFromClient					// GamePlayer�ģ�LocalShot
		|| !is_gameplayer_id(lAttackerID, pkGamePlayer))	// NetPlayer��
		&& NULL == _findBulletEvent(lEventID, lBulletLocalID, lAttackerID)
		)
	{
		if (!bShotFromClient)
		{
			_modifyBulletZValue(pkEvent);
		}
		SK_SkillEvent* pkBulletShotEvent = _allocEvent();
		pkBulletShotEvent->Clone(pkEvent);
		_addEvent(pkBulletShotEvent);
	}
	else	// GamePlayer��ServerShot��Response
	{
		SK_SkillEvent* pkBulletShotEvent = _findBulletEvent(0, lBulletLocalID, lAttackerID);
		if (pkBulletShotEvent != NULL)
		{
			pkBulletShotEvent->SetEventID(lEventID);
			GfxWriteLog(LOG_TAG_SKILL_EVENT, LOG_SWITCH_SKILL_EVENT, "[Skill Bullet] Shot ack, lBulletLocalID=%d, lEventID=%d, Shooter=%d", 
				lBulletLocalID, lEventID, lAttackerID);
		}
		else
		{
			// ��Ӧ�ı����ӵ��Ѿ���ɾ����������VerifyBack����֮ǰ���ͱ����ȱ�ը��
			if (lBulletLocalID < GD_ClientSelfInitiatedBulletID::Current())
			{
				GfxWriteLog(LOG_TAG_SKILL_EVENT, LOG_SWITCH_SKILL_EVENT, "[Skill Bullet] Shot ack failed, bullet destroyed, ignore this msg, lBulletLocalID=%d, lEventID=%d, Shooter=%d", 
					lBulletLocalID, lEventID, lAttackerID);
				return;
			}
		}
	}


	GD_ClientBulletManager* pkBulletManager = GD_ClientBulletManager::GetSingletonPtr();
	if (NULL != pkBulletManager)
	{
		T_ASSERT_CRITICAL_S (lDummyType >= GD_ModelBase::FDT_INVALID && lDummyType < GD_ModelBase::FDT_MAX_COUNT);
		BulletType eBulletType = (BulletType)lMoveType;

		GD_BulletCreateArg kBCArg(eBulletType);
		kBCArg.SetShooter(lAttackerID, lDummyType);
		kBCArg.SetTime(fMoveLifeTime, fCurrentTime);
		init_bullet_arg_with_skill(kBCArg, pkSkillExecute, lBulletFuncID, fPitchAngle);
		kBCArg.bDisableCameraShakeOnExplosion = IS_PLAYER_CLASS_ID(lAttackerID) && (!is_gameplayer_id(lAttackerID, pkGamePlayer));	// buddy��NetPlayer�ر�����
		
		UT_Vec3Int kShotLocation;
		kShotLocation.x = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_SHOT_LOCATION_X);
		kShotLocation.y = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_SHOT_LOCATION_Y);
		kShotLocation.z = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_SHOT_LOCATION_Z);
		kBCArg.SetShootLocation(UT_EngineMath::ConvertVec3IntToNiPoint3(kShotLocation));

		switch (eBulletType)
		{
		case BT_MISSILE:
		case BT_HOMINGMISSLE:
			{
				kBCArg.SetPitchAngle(fPitchAngle);
			}//no break here
		case BT_TERMINAL_HOVER:
		case BT_SHOOT:
			{
				if (IS_PLAYER_CLASS_ID(lDstActorID) || IS_NPC_ID(lDstActorID))
				{
					kBCArg.SetTarget(lDstActorID);
					pkBulletManager->ShootBullet(BULLET_UUID(lEventID, lBulletLocalID), kBCArg);
				}
				else
				{
					UT_Vec3Int kTargetLocation;
					kTargetLocation.x = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_X);
					kTargetLocation.y = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Y);
					kTargetLocation.z = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Z);
					//��������ӵ����ʱ ǿ��ת��
					if (!is_gameplayer_id(lAttackerID, pkGamePlayer))
					{
						LC_Helper::FaceActorToLocation(pkAttacker, pkAttacker->GetCurrentLocation(),kTargetLocation);
					}

					/// ������ֱ����
					bool bModifyTargetZWithShotLocation = bool(eBulletType == BT_SHOOT &&  lBulletLocalID == 0);		// from buddy, from npc
					kBCArg.SetTarget(UT_EngineMath::ConvertVec3IntToNiPoint3(kTargetLocation), bModifyTargetZWithShotLocation);
					pkBulletManager->ShootBullet(BULLET_UUID(lEventID, lBulletLocalID), kBCArg);

				}
			}
			break;
		case BT_CHAIN:
			{
				kBCArg.SetTarget(lDstActorID);
				pkBulletManager->ShootBullet(BULLET_UUID(lEventID, lBulletLocalID), kBCArg);
			}
			break;
		case BT_CAST:
			{
				UT_Vec3Int kTargetLocation;
				kTargetLocation.x = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_X);
				kTargetLocation.y = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Y);
				//kTargetLocation.z = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Z);

				//��ȡĿ���߶�
				SE_SceneManager* pkSceneManager = SE_SceneManager::GetSingletonPtr();
				const SE_SceneTerrainLocalInfo& rkTerrainInfo = pkSceneManager->GetWalkableInfo(Int2Float(kTargetLocation.x), Int2Float(kTargetLocation.y));
				kTargetLocation.z = Float2Int(rkTerrainInfo._fLocalHeight);

				kBCArg.SetPitchAngle(fPitchAngle);
				kBCArg.SetTarget(UT_EngineMath::ConvertVec3IntToNiPoint3(kTargetLocation));
				pkBulletManager->ShootBullet(BULLET_UUID(lEventID, lBulletLocalID),kBCArg);
			}
			break;
		default:
			break;
		}
	}
}
//------------------------------------------------------------

void Skill::SK_ClientSkillEventManager::RecvShotBulletMergedFromServer(const Protocol::MG_SyncBulletInfo & rkMsg, bool bArenaPlayBack)
{
	SK_SkillEvent* pkEvent = _allocEvent();
	for (int i = 0; i < rkMsg.m_nEventCount; ++i)
	{
		pkEvent->GetFromProtocol(rkMsg, (unsigned int)i);
		_recvMessageFromServer(pkEvent, bArenaPlayBack);
	}
	_freeEvent(pkEvent);
}

//------------------------------------------------------------
void SK_ClientSkillEventManager::_recvBulletChangeTargetLocationMessage(SK_SkillEvent* pkEvent, bool bArenaPlayBack)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (!pkGamePlayer)
		return;

	UT_Vec3Int kTargetLocation, kShotLocation;

	long lBulletID = pkEvent->GetEventID();
	object_id_type lSrcActorID = pkEvent->GetSrcActorID();
	object_id_type lDstActorID = pkEvent->GetDstActorID();
	if (bArenaPlayBack)
	{
		LC_ClientArenaPlayBackManager *pkArenaPlayBackManager = LC_ClientArenaPlayBackManager::GetSingletonPtr();
		lSrcActorID = pkArenaPlayBackManager->GetTemID(lSrcActorID);
		lDstActorID = pkArenaPlayBackManager->GetTemID(lDstActorID);
	}
	kTargetLocation.x = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_X);
	kTargetLocation.y = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Y);
	kTargetLocation.z = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Z);
	kShotLocation.x   = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_SHOT_LOCATION_X);
	kShotLocation.y   = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_SHOT_LOCATION_Y);
	kShotLocation.z   = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_SHOT_LOCATION_Z);
	int64_t kCollide  = pkEvent->GetParam(SK_BulletEvent::SEP_COLLIDE_PARAM);
	float fMoveLifeTime = SK_BulletEvent::ParamLLong2Float(pkEvent->GetParam(SK_BulletEvent::SEP_MOVE_LIFE_TIME));	//�漰�˵�λת��
	long  lBulletLocalID = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_BULLET_LOCAL_ID);

	float fCurrentTime = GET_PROCESS_TIME;

	//�����¼�
	SK_SkillEvent* pkBulletEvent = _findBulletEvent(lBulletID, lBulletLocalID, lSrcActorID);
	if (NULL == pkBulletEvent)
	{
		GfxWriteLog(LOG_TAG_SKILL_EVENT, LOG_SWITCH_SKILL_EVENT, "[Skill Bullet] ChangeTargetLocation failed, cannot find bullet, lBulletID=%d, lBulletLocalID=%d, t=%f", lBulletID, lBulletLocalID, fCurrentTime);
		return;
	}

	// �����������Ӧ���Ѿ�Change����
	if (is_gameplayer_id(lSrcActorID, pkGamePlayer))
	{
		GfxWriteLog(LOG_TAG_SKILL_EVENT, LOG_SWITCH_SKILL_EVENT, "[Skill Bullet] ChangeTargetLocation ack, lBulletID=%d, lBulletLocalID=%d, t=%f", lBulletID, lBulletLocalID, fCurrentTime);
		return;
	}

	//�޸��ӵ��¼��Ĳ���
	pkBulletEvent->SetDstActorID(lDstActorID);
	pkBulletEvent->SetParam(SK_BulletEvent::SEP_MOVE_START_TIME, SK_BulletEvent::ParamFloat2LLong(fCurrentTime));
	pkBulletEvent->SetParam(SK_BulletEvent::SEP_MOVE_LIFE_TIME, SK_BulletEvent::ParamFloat2LLong(fMoveLifeTime));

	pkBulletEvent->SetParam(SK_BulletEvent::SEP_TARGET_LOCATION_X, kTargetLocation.x);
	pkBulletEvent->SetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Y, kTargetLocation.y);
	pkBulletEvent->SetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Z, kTargetLocation.z);
	pkBulletEvent->SetParam(SK_BulletEvent::SEP_SHOT_LOCATION_X, kShotLocation.x);
	pkBulletEvent->SetParam(SK_BulletEvent::SEP_SHOT_LOCATION_Y, kShotLocation.y);
	pkBulletEvent->SetParam(SK_BulletEvent::SEP_SHOT_LOCATION_Z, kShotLocation.z);

	pkBulletEvent->SetParam(SK_BulletEvent::SEP_COLLIDE_PARAM, kCollide);

	GD_BulletManager::GetSingleton().BulletChangeTargetLocation(BULLET_UUID(lBulletID,lBulletLocalID), 
		UT_EngineMath::ConvertVec3IntToNiPoint3(kShotLocation), 
		UT_EngineMath::ConvertVec3IntToNiPoint3(kTargetLocation), 
		fMoveLifeTime, fCurrentTime);
}
//------------------------------------------------------------
void SK_ClientSkillEventManager::_recvBulletPenetrateMessage(SK_SkillEvent* pkEvent, bool bArenaPlayBack)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (!pkGamePlayer)
		return;

	UT_Vec3Int kBulletLocation;

	object_id_type lSrcActorID = pkEvent->GetSrcActorID();
	object_id_type lDstActorID = pkEvent->GetDstActorID();
	if (bArenaPlayBack)
	{
		LC_ClientArenaPlayBackManager *pkArenaPlayBackManager = LC_ClientArenaPlayBackManager::GetSingletonPtr();
		lSrcActorID = pkArenaPlayBackManager->GetTemID(lSrcActorID);
		lDstActorID = pkArenaPlayBackManager->GetTemID(lDstActorID);
	}
	long lBulletID = pkEvent->GetEventID();
	long lSkillID = pkEvent->GetLogicID();
	kBulletLocation.x = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_X);
	kBulletLocation.y = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_Y);
	kBulletLocation.z = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_Z);
	long lBulletFuncID = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_BULLET_FUNC);
	float fMoveLifeTime = SK_BulletEvent::ParamLLong2Float(pkEvent->GetParam(SK_BulletEvent::SEP_MOVE_LIFE_TIME));
	long  lBulletLocalID = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_BULLET_LOCAL_ID);
	// ���̶���id
	int lPenetrateCount = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_PENETRATE_COUNT);
	long lPenetrateActorID = (long)pkEvent->GetParam(SK_BulletEvent::SEP_TARGET_BEGIN + lPenetrateCount - 1);

	float fCurrentTime = GET_PROCESS_TIME;

	//�����¼�
	SK_SkillEvent* pkBulletEvent = _findBulletEvent(lBulletID, lBulletLocalID, lSrcActorID);
	if (NULL == pkBulletEvent)
	{
		GfxWriteLog(LOG_TAG_SKILL_EVENT, LOG_SWITCH_SKILL_EVENT, "[Skill Bullet] Penetrate failed, cannot find bullet, lBulletID=%d, lBulletLocalID=%d, t=%f", lBulletID, lBulletLocalID, fCurrentTime);
		return;
	}

	// �����������Ӧ���Ѿ����̹���
	if (is_gameplayer_id(lSrcActorID, pkGamePlayer) && lBulletLocalID > 0)
	{
		GfxWriteLog(LOG_TAG_SKILL_EVENT, LOG_SWITCH_SKILL_EVENT, "[Skill Bullet] Penetrate ack, lBulletID=%d, lBulletLocalID=%d, t=%f", lBulletID, lBulletLocalID, fCurrentTime);
		return;
	}

	//�޸��ӵ��¼��Ĳ���

	// ��Ŀ���͵Ĵ�͸��lTargetID == 0��Ӧ�ò���Ҫ�������� SEP_MOVE_START_TIME
	pkBulletEvent->SetParam(SK_BulletEvent::SEP_MOVE_START_TIME, SK_BulletEvent::ParamFloat2LLong(fCurrentTime));
	pkBulletEvent->SetParam(SK_BulletEvent::SEP_MOVE_LIFE_TIME, SK_BulletEvent::ParamFloat2LLong(fMoveLifeTime));

	// ���ô���λ��
	pkBulletEvent->SetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_X, kBulletLocation.x);
	pkBulletEvent->SetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_Y, kBulletLocation.y);
	pkBulletEvent->SetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_Z, kBulletLocation.z);

	// ���ô���Ŀ��
	pkBulletEvent->SetParam(SK_BulletEvent::SEP_PENETRATE_COUNT, lPenetrateCount);

	SK_Factory* pkSkillFactory = SK_Factory::GetSingletonPtr();
	SK_SkillExecute* pkSkillExecute = pkSkillFactory->RequestSkillExcute(lSkillID);
	if (NULL != pkSkillExecute && pkSkillExecute->GetProperty_BulletParam(lBulletFuncID).m_bPeneStraight)
	{
		pkBulletEvent->SetParam(SK_BulletEvent::SEP_TARGET_0, lPenetrateActorID);
		lDstActorID = 0;
	}
	else
	{
		pkBulletEvent->SetParam(SK_BulletEvent::SEP_TARGET_BEGIN + lPenetrateCount - 1, lPenetrateActorID);
		pkBulletEvent->SetDstActorID(lDstActorID);
	}

	LC_ClientWorldManager* pkWorldManager = CLIENT_GET_WORLD_MANAGER();
	LC_ActorBase* pkSrcActor = (LC_ActorBase*)pkWorldManager->FindObject(lSrcActorID);

	if (NULL != pkSkillExecute && NULL != pkSrcActor)
	{
		pkSrcActor->SetBulletLocation(kBulletLocation);
		pkSkillExecute->BulletPenetrate(lBulletFuncID, pkSrcActor, lPenetrateActorID, 0, BULLET_UUID(lBulletID,lBulletLocalID));
	}

	GD_BulletManager::GetSingleton().BulletPenetrate(BULLET_UUID(lBulletID,lBulletLocalID), lPenetrateActorID, lDstActorID, fMoveLifeTime, fCurrentTime);
}
//------------------------------------------------------------
void SK_ClientSkillEventManager::_recvBulletExplosiveMessage(SK_SkillEvent* pkEvent, bool bArenaPlayBack)
{
	object_id_type lSrcActorID = pkEvent->GetSrcActorID();
	if (bArenaPlayBack)
	{
		LC_ClientArenaPlayBackManager *pkArenaPlayBackManager = LC_ClientArenaPlayBackManager::GetSingletonPtr();
		lSrcActorID = pkArenaPlayBackManager->GetTemID(lSrcActorID);
	}
	long lBulletID = pkEvent->GetEventID();
	long lBulletLocalID = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_BULLET_LOCAL_ID);
	
	//�����¼�
	SK_SkillEvent* pkBulletEvent = _findBulletEvent(lBulletID, lBulletLocalID, lSrcActorID);
	if (NULL == pkBulletEvent)
	{
		GfxWriteLog(LOG_TAG_SKILL_EVENT, LOG_SWITCH_SKILL_EVENT, "[Skill Bullet] Exploded failed, cannot find bullet, lBulletID=%d, lBulletLocalID=%d, t=%f", lBulletID, lBulletLocalID, GET_PROCESS_TIME);
		return;
	}
	//�ӵ���ը��ע��ʹ�� pkEvent������������
	_bulletExplosive(pkEvent, bArenaPlayBack);
	//ɾ���ӵ��¼�
	_removeEvent(pkBulletEvent);
	_freeEvent(pkBulletEvent);
}
//------------------------------------------------------------
void SK_ClientSkillEventManager::_recvKillMessage(SK_SkillEvent* pkEvent, bool bArenaPlayBack)
{
	LC_ClientLogicManager* pkLogicManager =  CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager)
	{
		GfxWriteLog(LOG_SKILL_EVENT_FILELINE, LOG_SWITCH_SKILL_EVENT, "��ȡ�ͻ����߼�����������");
		return;
	}
	object_id_type lGamePlayerID = pkLogicManager->GetInputControlActorID();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (NULL == pkGamePlayer)
	{
		GfxWriteLog(LOG_SKILL_EVENT_FILELINE, LOG_SWITCH_SKILL_EVENT, "��ȡ��Ϸ��Ҵ���");
		return;
	}


	object_id_type lSrcActorID = pkEvent->GetSrcActorID();
	object_id_type lDstActorID = pkEvent->GetDstActorID();
	if (bArenaPlayBack)
	{
		LC_ClientArenaPlayBackManager *pkArenaPlayBackManager = LC_ClientArenaPlayBackManager::GetSingletonPtr();
		lSrcActorID = pkArenaPlayBackManager->GetTemID(lSrcActorID);
		lDstActorID = pkArenaPlayBackManager->GetTemID(lDstActorID);
	}

	LC_ClientWorldManager* pkWorldManager = CLIENT_GET_WORLD_MANAGER();
	LC_ActorBase* pkSrcActor = (LC_ActorBase*)pkWorldManager->FindObject(lSrcActorID);
	LC_ActorBase* pkDstActor = (LC_ActorBase*)pkWorldManager->FindObject(lDstActorID);
	if (NULL == pkSrcActor)
	{
		GfxWriteLog(LOG_SKILL_EVENT_FILELINE, LOG_SWITCH_SKILL_EVENT, "ɱ���¼������Ҳ���������[%i]", lSrcActorID);
		return;
	}
	if (NULL == pkDstActor)
	{
		GfxWriteLog(LOG_SKILL_EVENT_FILELINE, LOG_SWITCH_SKILL_EVENT, "ɱ���¼������Ҳ���������[%i]", lDstActorID);
		return;
	}

	//���ɱ���ֻ������
	bool bIsSrcBuddy = pkGamePlayer->IsBuddy(lSrcActorID);
	bool bIsDstBuddy = pkGamePlayer->IsBuddy(lDstActorID);


	if(IS_NPC_ID(lDstActorID))
		return;

	//ս����ʾ��Ϣ
	StringType kSrcName = "";
	StringType kDstName = "";
	bool bSelfInfo = false;
	if (bIsSrcBuddy)
	{
		bSelfInfo = true;
		if (lSrcActorID == lGamePlayerID)
		{
			kSrcName = CTSTR_YOU2;
			
		}
		else
		{
			kSrcName = CTSTR_YOU3 + pkSrcActor->GetCharName();
		}
	}
	else
	{
		if(IS_PLAYER_CLASS_ID(lSrcActorID))
		{
			LC_ClientNetPlayer* pNetPlayer= (LC_ClientNetPlayer*)pkSrcActor;
			//if(pNetPlayer)
			{
				kSrcName = pNetPlayer->GetCommanderName();
			}
		}
		else
		{
			kSrcName = pkSrcActor->GetCharName();
		}
	}
	
	if (bIsDstBuddy)
	{
		bSelfInfo = true;
		if (lDstActorID == lGamePlayerID)
		{
			kDstName = CTSTR_YOU2;
		}
		else
		{
			kDstName = CTSTR_YOU3 + pkDstActor->GetCharName();
		}
	}
	else
	{
		if(IS_PLAYER_CLASS_ID(lDstActorID))
		{
			LC_ClientNetPlayer* pNetPlayer= (LC_ClientNetPlayer*)pkDstActor;
			//if(pNetPlayer)
			{
				if( lDstActorID==pNetPlayer->GetCommanderUID())
				{
					kDstName = pNetPlayer->GetCommanderName();
				}
				else
				{
					kDstName=pNetPlayer->GetCommanderName()+GET_MSG(LC_CLIENTNPCMANAGER_CPP_001)+pNetPlayer->GetCharName();
				}
			}
			//��ɱ����	
		}
		else
		{
			kDstName=pkDstActor->GetCharName();
		}
		
	}

	if (IS_PLAYER_CLASS_ID(lDstActorID))
	{
		if (bSelfInfo)
		{
			SetMessageToAuxiliaryTab();
		}
	}

	//StringType kOutputSrc, kOutputDst;
	//GUISys::getRichTextParse->FilterForbiddenText(kSrcName, kOutputSrc, TT_TextFilter::TFT_CHAR_NAME);
	//GUISys::getRichTextParse->FilterForbiddenText(kDstName, kOutputDst, TT_TextFilter::TFT_CHAR_NAME);
	//PK��ʾ��Ϣ����ɱ���
	if (bIsSrcBuddy && IS_PLAYER_CLASS_ID(lDstActorID))
	{
		SetMessageToAuxiliaryTab();
		SendLogicInfoMessage(RE_PK_YOU_KILL_TARGET, kDstName.c_str());
	}
	else if (bIsDstBuddy)
	{
		SendLogicInfoMessage(RE_FIGHT_KILL,kDstName.c_str(), kSrcName.c_str());
	}

	// ɱ�����	  ���ڲ���Ҫɱ����ң��Ƴ��������������
	/*bool bIsRevenge = false;
	if (lSrcActorID == lGamePlayerID && IS_PLAYER_CLASS_ID(lDstActorID))
	{
		//����ھ�������,�޳����߼�
		if(pkGamePlayer->GetMapLogicType() <= MT_ARENA_BEGIN || pkGamePlayer->GetMapLogicType() >= MT_ARENA_END)
		{
			if ( pkGamePlayer->GetSocietyAsset()->GetEnemy(lDstActorID) )
			{
				bIsRevenge = true;
				pkGamePlayer->DeleteEnemy(lDstActorID);
			}
		}
	}*/
	
	//��ɱ�˹������ڱ����Ͳ���ʾ������if(IS_NPC_ID(lDstActorID))return��Ҳû��ϵ
	if(bIsSrcBuddy)
	{
		LC_ClientGameEvent kGameEvent;
		kGameEvent.SetClientEventType(GameLogic::CGET_PLAYER_KILL_SOMETHING);

		StringStreamType sStream("");
		sStream << pkDstActor->GetCharName() << "#" << lDstActorID;
		if(IS_PLAYER_CLASS_ID(lDstActorID))
			sStream << "#1";
		else
			sStream << "#0";

		if(pkGamePlayer->IsHooking())
			sStream << "#1";
		else
			sStream << "#0";
		
		sStream<<"#"<<kSrcName;
		sStream<<"#"<<kDstName;
		LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(sStream.str());
		kGameEvent.SetUserData(&kUserData);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}

	// ���Ǳ�ɱ��
	if((pkGamePlayer->GetMapLogicType() <= MT_ARENA_BEGIN || pkGamePlayer->GetMapLogicType() >= MT_ARENA_END) && bIsDstBuddy)
	{
		LC_ClientGameEvent kGameEvent;
		kGameEvent.SetClientEventType(GameLogic::CGET_PLAYER_BE_KILLED);

		StringStreamType sStream("");
		sStream << kSrcName <<"#"<<lSrcActorID;
		if(IS_PLAYER_CLASS_ID(lSrcActorID))
			sStream << "#1";
		else
			sStream << "#0";

		if(pkGamePlayer->IsHooking())
			sStream << "#1";
		else
			sStream << "#0";

		sStream<<"#"<<kSrcName;
		sStream<<"#"<<kDstName;
		LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(sStream.str());
		kGameEvent.SetUserData(&kUserData);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
	//if (IS_PLAYER_CLASS_ID(lSrcActorID) && lDstActorID == lGamePlayerID)
	//{
		//�������͸��������,�޳����߼�
		/*if(pkGamePlayer->GetMapLogicType() != MT_ARENA_TEST
			&& pkGamePlayer->GetMapLogicType() != MT_ARENA_33TURN
			&& pkGamePlayer->GetMapLogicType() != MT_ARENA_55TURN
			&& pkGamePlayer->GetMapLogicType() != MT_ARENA_55FREE
			&& !bIsRevenge
			&& NULL != pkGamePlayer->GetPKRecorder()
			&& PST_WHITE == pkGamePlayer->GetPKRecorder()->GetPKStateType())
		{
// 			if ( pkGamePlayer->GetSocietyAsset()->GetEnemy(lSrcActorID) == NULL && !pkGamePlayer->IsFriend(lSrcActorID))
// 			{
// 				pkGamePlayer->AddEnemy(lSrcActorID, pkSrcActor->GetCharName());
// 			}
			pkGamePlayer->SocietyCheckPkEvent( lSrcActorID, pkSrcActor->GetCharName(), lDstActorID, pkDstActor->GetCharName());
		}*/
	//}

	//��������������Ч (���ڲ���Ҫ�ˣ������������ϼ�����Ч)
	//if (lDstActorID == lGamePlayerID)
	//{
	//	StringType kSoundFileDefine = "";
	//	switch (pkGamePlayer->GetCharType())
	//	{
	//	case 1:		//��
	//		//kSoundFileDefine = UT_MathBase::RandFloat11() > 0 ? "UI_PLAYER_DEAD_MAN_1" : "UI_PLAYER_DEAD_MAN_2";
	//		kSoundFileDefine = "Voice_Player_M_Death";
	//		break;
	//	case 2:		//Ů
	//		//kSoundFileDefine = UT_MathBase::RandFloat11() > 0 ? "UI_PLAYER_DEAD_WOMAN_1" : "UI_PLAYER_DEAD_WOMAN_2";
	//		kSoundFileDefine = "Voice_Player_F_Death";
	//		break;
	//	default:
	//		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, 
	//			"��ɫ����[%d]��������ֻ��Ϊ�л���Ů", pkGamePlayer->GetCharType());
	//		break;
	//	}
	//	//LS_LuaStateFactory* pkLuaFactory = LS_LuaStateFactory::GetSingletonPtr();
	//	//StringType kSoundFilePath = pkLuaFactory->GetStringFromLogicState(kSoundFileDefine);
	//	if ("" != kSoundFileDefine)
	//	{
	//		AS_ClientSoundManager* pkSoundManager = AS_ClientSoundManager::GetSingletonPtr();
	//		NiPoint3 kPos = UT_EngineMath::ConvertVec3IntToNiPoint3( pkSrcActor->GetVisibleLocation() );
	//		pkSoundManager->Play3DEx(kPos , kSoundFileDefine , 
	//			AS_ClientSoundManager::SCT_PLAYER_SKILL_EVENT );
	//		//pkSoundManager->Play3D( kPos , kSoundFilePath , 
	//		//	AS_ClientSoundManager::SCT_PLAYER_SKILL_EVENT );
	//	}
	//}

	
	_notifyState(pkSrcActor, pkDstActor, pkEvent);
}
//------------------------------------------------------------
void SK_ClientSkillEventManager::_recvAttachGfxMessage(SK_SkillEvent* pkEvent, bool bArenaPlayBack)
{
	long lSrcActorID = pkEvent->GetSrcActorID();
	long lDstActorID = pkEvent->GetDstActorID();
	if (bArenaPlayBack)
	{
		LC_ClientArenaPlayBackManager *pkArenaPlayBackManager = LC_ClientArenaPlayBackManager::GetSingletonPtr();
		lSrcActorID = pkArenaPlayBackManager->GetTemID(lSrcActorID);
		lDstActorID = pkArenaPlayBackManager->GetTemID(lDstActorID);
	}
	long lTargetLocationX = (int32_t)pkEvent->GetParam(SK_AttachGfxEvent::SEP_TARGET_LOCATION_X);
	long lTargetLocationY = (int32_t)pkEvent->GetParam(SK_AttachGfxEvent::SEP_TARGET_LOCATION_Y);
	long lDummyType = (int32_t)pkEvent->GetParam(SK_AttachGfxEvent::SEP_DUMMY_TYPE);
	long lStayStatic = (int32_t)pkEvent->GetParam(SK_AttachGfxEvent::SEP_STAY_STATIC);
	bool bStayStatic = (lStayStatic == 1) ? true : false;
	long lApplyDirection = (int32_t)pkEvent->GetParam(SK_AttachGfxEvent::SEP_APPLY_DIRECTION);
	bool bApplyDirection = (lApplyDirection == 1) ? true : false;
	int iLCGfx = (int32_t)pkEvent->GetParam(SK_AttachGfxEvent::SEP_GFX_LC);
	int iBindMode = (int32_t)pkEvent->GetParam(SK_AttachGfxEvent::SEP_BIND_MODE);

	//��ȡ
	StringType kGfxName = "";
	pkEvent->GetParam(SK_AttachGfxEvent::SEP_GFX_NAME_START, kGfxName);

	LC_ClientWorldManager* pkWorldManager = CLIENT_GET_WORLD_MANAGER();
	LC_ActorBase* pkSrcActor = (LC_ActorBase*)pkWorldManager->FindObject(lSrcActorID);
	LC_ActorBase* pkDstActor = (LC_ActorBase*)pkWorldManager->FindObject(lDstActorID);

	switch (iLCGfx)
	{
	case 1:
		{
			LC_ClientLogicManager* pLogicManager =  CLIENT_GET_LOGIC_MANAGER();
			LC_ClientGamePlayer* pGamePlayer= pLogicManager->GetOperationPlayer();
			if (NULL != pGamePlayer)
			{
				lDstActorID = pGamePlayer->GetEyeCharTypeID();
				pkDstActor = (LC_ActorBase*)pkWorldManager->FindObject(lDstActorID);
			}

			GD_ClientActorModel* pLCModel = (GD_ClientActorModel*)
				IM_Manager::GetSingleton().GetModel(lDstActorID);

			if (NULL != pLCModel)
			{
				pLCModel->PlayAnimation("����1");
			}
		}
		break;
	default:
		break;
	}

	if (NULL == pkDstActor && (lTargetLocationX <= 0 || lTargetLocationY <= 0))
	{
		//GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "������Ч�����¼������Ҳ�����Ŀ��");
		return;
	}


	//������Ч����
	NiPoint3 kRotate = NiPoint3::ZERO;
	NiPoint3 kDirection = NiPoint3::ZERO;
	if (bApplyDirection)
	{
		if (NULL != pkSrcActor && NULL != pkDstActor)
		{
			UT_Vec3Float kDir = UT_MathBase::ConvertVec3IntToFloat(pkDstActor->GetVisibleLocation() - pkSrcActor->GetVisibleLocation());
			kRotate.z = NiATan2(kDir.y, kDir.x);
			kDirection = UT_EngineMath::ConvertVec3FloatToNiPoint3(kDir);
		}
		else
		{
			//GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "������Ч�����¼����󣬲��ܸ��ӷ�������Ч");
			bApplyDirection = false;
		}
	}

	//����Ч��Ŀ������
	if (NULL != pkDstActor)
	{
		if (lDummyType <= GD_ClientActorModel::FDT_INVALID && lDummyType >= GD_ClientActorModel::FDT_MAX_COUNT)
		{
			GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "������Ч�����¼�������Ч����Ч�󶨵�[%d]", lDummyType);
			return;
		}
		
		GD_ClientActorModel* pkVisibleData = IM_Manager::GetSingletonPtr()->GetActor(lDstActorID);
		if (pkVisibleData != NULL)
		{
			NiPoint3* pkRotate = NULL;
			if (bApplyDirection)
			{
				pkRotate = &kRotate;
			}
			FX_ClientFxManager* pkFxManager = FX_ClientFxManager::GetSingletonPtr();
			StringType rp = pkFxManager->findPlatformReplace(kGfxName);
			pkVisibleData->AttachFx((GD_ClientActorModel::FxDummyNodeType)lDummyType, iBindMode, bStayStatic, rp, NULL, pkRotate);
		}
	}
	else	//����Ч���ص�
	{
		UT_Vec3Int kTargetLocation;
		kTargetLocation.x = lTargetLocationX;
		kTargetLocation.y = lTargetLocationY;

		//��ȡĿ���Z����
		SE_SceneManager* pkSceneManager = SE_SceneManager::GetSingletonPtr();
		const SE_SceneTerrainLocalInfo& rkTerrainInfo = 
			pkSceneManager->GetWalkableInfo(Int2Float(kTargetLocation.x), Int2Float(kTargetLocation.y));
		kTargetLocation.z = Float2Int(rkTerrainInfo._fLocalHeight);

		NiPoint3* pkDirection = NULL;
		if (bApplyDirection)
		{
			pkDirection = &kDirection;
		}
		FX_ClientFxManager* pkFxManager = FX_ClientFxManager::GetSingletonPtr();
		StringType rp = pkFxManager->findPlatformReplace(kGfxName);
		int iFxID = pkFxManager->CreateFx(rp, true);
		pkFxManager->BindFxToLocation(iFxID, UT_EngineMath::ConvertVec3IntToNiPoint3(kTargetLocation), pkDirection);
		pkFxManager->PlayFx(iFxID, GET_PROCESS_TIME);
	}

	_notifyState(pkSrcActor, pkDstActor, pkEvent);
}
//------------------------------------------------------------
void SK_ClientSkillEventManager::_recvAttachSoundMessage(SK_SkillEvent* pkEvent, bool bArenaPlayBack)
{
	long lSrcActorID = pkEvent->GetSrcActorID();
	long lDstActorID = pkEvent->GetDstActorID();
	if (bArenaPlayBack)
	{
		LC_ClientArenaPlayBackManager *pkArenaPlayBackManager = LC_ClientArenaPlayBackManager::GetSingletonPtr();
		lSrcActorID = pkArenaPlayBackManager->GetTemID(lSrcActorID);
		lDstActorID = pkArenaPlayBackManager->GetTemID(lDstActorID);
	}
	long lTargetLocationX = (int32_t)pkEvent->GetParam(SK_AttachSoundEvent::SEP_TARGET_LOCATION_X);
	long lTargetLocationY = (int32_t)pkEvent->GetParam(SK_AttachSoundEvent::SEP_TARGET_LOCATION_Y);
	int iSoundChannel = (int32_t)pkEvent->GetParam(SK_AttachSoundEvent::SEP_SOUND_CHANNEL_TYPE);
	//���������Ĳ��ŷ�ʽ�ɱ���еĲ���ָ��
	long lSoundType = (int32_t)pkEvent->GetParam(SK_AttachSoundEvent::SEP_SOUND_TYPE);

	if (iSoundChannel < AS_ClientSoundManager::SCT_2D || iSoundChannel >= AS_ClientSoundManager::SCT_MAX)
	{
		return;
	}

// 	if (lSoundType != SK_AttachSoundEvent::SST_2D && lSoundType != SK_AttachSoundEvent::SST_3D)
// 	{
// 		return;
// 	}

	//��ȡ
	StringType kSoundName = "";
	pkEvent->GetParam(SK_AttachSoundEvent::SEP_SOUND_NAME_START, kSoundName);

	LC_ClientWorldManager* pkWorldManager = CLIENT_GET_WORLD_MANAGER();
	LC_ActorBase* pkSrcActor = (LC_ActorBase*)pkWorldManager->FindObject(lSrcActorID);
	LC_ActorBase* pkDstActor = (LC_ActorBase*)pkWorldManager->FindObject(lDstActorID);

	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if(!SC_ClientSkillInterface::CheckAttachSkillSound(pkGamePlayer, pkSrcActor))
		return;

	//if (NULL == pkDstActor && (lTargetLocationX <= 0 || lTargetLocationY <= 0))
	//{
		//GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "������Ч�����¼������Ҳ�����Ŀ��");
	//	return;
	//}

	AS_ClientSoundManager* pkSoundManager = AS_ClientSoundManager::GetSingletonPtr();
	
	int soundType = SK_AttachSoundEvent::SST_2D_SINGLE_SOUND;
	if ( lSoundType == SK_AttachSoundEvent::SST_2D_NPCSOUND_SEQ)
	{
		soundType = lSoundType;
	}
	else
	{
		soundType =  pkSoundManager->IsAudioFile2d(kSoundName) ? 1 : 2;
	}
	
	if (SK_AttachSoundEvent::SST_2D_SINGLE_SOUND == soundType)
	{
		pkSoundManager->Play2DEx(kSoundName, (AS_ClientSoundManager::SOUND_CHANNEL_TYPE)iSoundChannel);
	}
	else if (SK_AttachSoundEvent::SST_3D_SINGLE_SOUND == soundType)
	{
		//��������Ŀ������
		NiPoint3 kPlayerSoundPos;
		if (NULL != pkDstActor)
		{
			kPlayerSoundPos = UT_EngineMath::ConvertVec3IntToNiPoint3(pkDstActor->GetVisibleLocation());

		}
		else	//���������ص�
		{
			UT_Vec3Int kTargetLocation;
			kTargetLocation.x = lTargetLocationX;
			kTargetLocation.y = lTargetLocationY;

			kPlayerSoundPos = UT_EngineMath::ConvertVec3IntToNiPoint3(kTargetLocation);
		}		

		//pkSoundManager->Play3D(kPos, kSoundName, AS_ClientSoundManager::SCT_PLAYER_SKILL_EVENT);
		pkSoundManager->Play3DEx(kPlayerSoundPos, kSoundName, (AS_ClientSoundManager::SOUND_CHANNEL_TYPE)iSoundChannel);
	}
	else if(SK_AttachSoundEvent::SST_2D_NPCSOUND_SEQ == soundType)
	{
		CF_SoundSequence::DataEntry* pkEntry = SafeGetCSVFileDataEntryPtr<CF_SoundSequence>(atoi(kSoundName.c_str()));
		if(pkEntry)
		{
			pkSoundManager->ParseSequenceString(pkEntry->_strSeqName);
			pkSoundManager->PlaySequenceNPCSound();
		}
	}

	_notifyState(pkSrcActor, pkDstActor, pkEvent);
}
//------------------------------------------------------------
void SK_ClientSkillEventManager::_recvAttachDisplayChestMessage(SK_SkillEvent* pkEvent, bool bArenaPlayBack)
{
	long lSrcActorID = pkEvent->GetSrcActorID();
	long lDstActorID = pkEvent->GetDstActorID();
	if (bArenaPlayBack)
	{
		LC_ClientArenaPlayBackManager *pkArenaPlayBackManager = LC_ClientArenaPlayBackManager::GetSingletonPtr();
		lSrcActorID = pkArenaPlayBackManager->GetTemID(lSrcActorID);
		lDstActorID = pkArenaPlayBackManager->GetTemID(lDstActorID);
	}
 	long lTargetLocationX = (int32_t)pkEvent->GetParam(SK_DisplayChestEvent::SEP_TARGET_LOCATION_X);
 	long lTargetLocationY = (int32_t)pkEvent->GetParam(SK_DisplayChestEvent::SEP_TARGET_LOCATION_Y);
	long lItemType = (int32_t)pkEvent->GetParam(SK_DisplayChestEvent::SEP_DISPLAY_CHEST_ID);
	int	dummyType = (int32_t)pkEvent->GetParam(SK_DisplayChestEvent::SEP_DUMMY_TYPE);
	int flyType = (int32_t)pkEvent->GetParam(SK_DisplayChestEvent::SEP_FLY_TYPE);

	LC_ClientWorldManager* pkWorldManager = CLIENT_GET_WORLD_MANAGER();
	LC_ActorBase* pkSrcActor = (LC_ActorBase*)pkWorldManager->FindObject(lSrcActorID);
	LC_ActorBase* pkDstActor = (LC_ActorBase*)pkWorldManager->FindObject(lDstActorID);

	if (NULL == pkDstActor && (lTargetLocationX <= 0 || lTargetLocationY <= 0))
	{
		//GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "������Ч�����¼������Ҳ�����Ŀ��");
		return;
	}

	//�����ͻ��˱��ֵ�����
	if (NULL != pkDstActor)
	{
		if (IS_DISPLAY_CHEST_ID(lItemType))
		{
			GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "���ӿͻ��˱��ֵ����＼���¼�������Ч��ID[%d]", lItemType);
			return;
		}

		UT_Vec3Int kTargetLocation;
		kTargetLocation.x = lTargetLocationX;
		kTargetLocation.y = lTargetLocationY;

		//��ȡĿ���Z����
		SE_SceneManager* pkSceneManager = SE_SceneManager::GetSingletonPtr();
		const SE_SceneTerrainLocalInfo& rkTerrainInfo = 
			pkSceneManager->GetWalkableInfo(Int2Float(kTargetLocation.x), Int2Float(kTargetLocation.y));
		kTargetLocation.z = Float2Int(rkTerrainInfo._fLocalHeight);

		//����ǹ��ڽڵ��ϵģ��ͺ���֮ǰ������
		GD_ClientActorModel* pkVisibleData = IM_Manager::GetSingletonPtr()->GetActor(lDstActorID);
		if (dummyType != GD_ModelBase::FDT_INVALID && pkVisibleData)
		{
			NiPoint3 sp = pkVisibleData->GetFxDummyNodePosition((GD_ModelBase::FxDummyNodeType)dummyType);
			kTargetLocation = UT_EngineMath::ConvertNiPoint3ToVec3Int(sp);
		}

		_createDisplayHolder(kTargetLocation, lItemType, lSrcActorID, (GD_ClientChestModel::ChestModelFlyType)flyType);
	}

	//Ŀǰ�¼����ڲ���֪ͨstate
	//_notifyState(pkSrcActor, pkDstActor, pkEvent);
}
//------------------------------------------------------------
void SK_ClientSkillEventManager::_recvDetachGfxMessage(SK_SkillEvent* pkEvent, bool bArenaPlayBack)
{
	long lSrcActorID = pkEvent->GetSrcActorID();
	long lDstActorID = pkEvent->GetDstActorID();
	if (bArenaPlayBack)
	{
		LC_ClientArenaPlayBackManager *pkArenaPlayBackManager = LC_ClientArenaPlayBackManager::GetSingletonPtr();
		lSrcActorID = pkArenaPlayBackManager->GetTemID(lSrcActorID);
		lDstActorID = pkArenaPlayBackManager->GetTemID(lDstActorID);
	}
	long lDummyType = (int32_t)pkEvent->GetParam(SK_DetachGfxEvent::SEP_DUMMY_TYPE);
	StringType fxName;
	pkEvent->GetParam(SK_DetachGfxEvent::SEP_GFX_NAME_START, fxName);

	LC_ClientWorldManager* pkWorldManager = CLIENT_GET_WORLD_MANAGER();
	LC_ActorBase* pkSrcActor = (LC_ActorBase*)pkWorldManager->FindObject(lSrcActorID);
	LC_ActorBase* pkDstActor = (LC_ActorBase*)pkWorldManager->FindObject(lDstActorID);

	//ȡ�� ����Ч��Ŀ������
	if (NULL != pkDstActor)
	{
		if (lDummyType <= GD_ClientActorModel::FDT_INVALID && lDummyType >= GD_ClientActorModel::FDT_MAX_COUNT)
		{
			GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "������Ч�����¼�������Ч����Ч�󶨵�[%d]", lDummyType);
			return;
		}
		GD_ClientActorModel* pkVisibleData = IM_Manager::GetSingletonPtr()->GetActor(lDstActorID);
		if ( pkVisibleData )
			pkVisibleData->DetachFx((GD_ClientActorModel::FxDummyNodeType)lDummyType, fxName);
	}
	//�����֪ͨ
	//_notifyState(pkSrcActor, pkDstActor, pkEvent);
}

void Skill::SK_ClientSkillEventManager::_recvImmunePunch(SK_SkillEvent* pkEvent, bool bArenaPlayBack)
{
	long lSrcActorID = pkEvent->GetSrcActorID();
	long lDstActorID = pkEvent->GetDstActorID();
	if (bArenaPlayBack)
	{
		LC_ClientArenaPlayBackManager *pkArenaPlayBackManager = LC_ClientArenaPlayBackManager::GetSingletonPtr();
		lSrcActorID = pkArenaPlayBackManager->GetTemID(lSrcActorID);
		lDstActorID = pkArenaPlayBackManager->GetTemID(lDstActorID);
	}

	LC_ClientLogicManager* pkLogicManager =  CLIENT_GET_LOGIC_MANAGER();
	object_id_type lGamePlayerID = pkLogicManager->GetInputControlActorID();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (NULL == pkGamePlayer)
	{
		GfxWriteLog(LOG_SKILL_EVENT_FILELINE, LOG_SWITCH_SKILL_EVENT, "_recvImmunePunch��ȡ��Ϸ��Ҵ���");
		return;
	}

	LC_ClientWorldManager* pkWorldManager = CLIENT_GET_WORLD_MANAGER();
	LC_ActorBase* pkSrcActor = (LC_ActorBase*)pkWorldManager->FindObject(lSrcActorID);
	LC_ActorBase* pkDstActor = (LC_ActorBase*)pkWorldManager->FindObject(lDstActorID);

	if (NULL == pkDstActor)
	{
		GfxWriteLog(LOG_SKILL_EVENT_FILELINE, LOG_SWITCH_SKILL_EVENT, "�˺��¼������Ҳ���������[%i]", lDstActorID);
		return;
	}

	if (pkDstActor->GetDead())
	{
		return;
	}

	/*if (pkGamePlayer->IsBuddy(lSrcActorID)
		|| pkGamePlayer->IsBuddy(lDstActorID)
		|| pkGamePlayer->IsNpcBelongToBuddy(pkSrcActor))
	{*/
		if (/*pkGamePlayer != NULL && pkDstActor != NULL &&*/ pkDstActor->CheckStateFxTime(302))
		{
			pkGamePlayer->SendMessage( CreateMsg_UE_PLAYER_STATE_CHANGE(lDstActorID, 302) );
		}
	//}
}

void Skill::SK_ClientSkillEventManager::_recvSummorDamageEvent( SK_SkillEvent* pkEvent)
{
	//��ӵ��¼��б�
	SK_SkillEvent* pkSummorDamage = _allocEvent();
	pkSummorDamage->Clone(pkEvent);
	pkSummorDamage->SetParam(SK_SummorDamage::SEP_START_TIME, SK_SkillEvent::ParamFloat2LLong(GET_PROCESS_TIME));
	_addEvent(pkSummorDamage);
}

void Skill::SK_ClientSkillEventManager::_procSummorEvent( float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent )
{
	object_id_type lSrcActorID = pkEvent->GetSrcActorID();
	float startTime = SK_SkillEvent::ParamLLong2Float(pkEvent->GetParam(SK_SummorDamage::SEP_START_TIME));
	float duration = SK_SkillEvent::ParamLLong2Float(pkEvent->GetParam(SK_SummorDamage::SEP_DURATION));
	float gfxTick = SK_SkillEvent::ParamLLong2Float(pkEvent->GetParam(SK_SummorDamage::SEP_GFX_TICK));
	int  gfxID = pkEvent->GetParam(SK_SummorDamage::SEP_GFX_ID);
	bDestroyEvent = false;

	LC_ClientWorldManager* pkWorldManager = CLIENT_GET_WORLD_MANAGER();
	LC_ActorBase* pkSrcActor = (LC_ActorBase*)pkWorldManager->FindObject(lSrcActorID);

	if ( pkSrcActor != NULL)
	{
		FX_ClientFxManager* pkFxManager = FX_ClientFxManager::GetSingletonPtr();

		if (gfxID == 0 && fCurrentTime > gfxTick + 0.4f )
		{
			gfxID = pkFxManager->CreateFx("N_����4_��������1.gfx",true);

			UT_Vec3Int kTargetLocation = pkSrcActor->GetCurrentLocation();
			//��ȡĿ���Z����
			SE_SceneManager* pkSceneManager = SE_SceneManager::GetSingletonPtr();
			const SE_SceneTerrainLocalInfo& rkTerrainInfo = 
				pkSceneManager->GetWalkableInfo(Int2Float(kTargetLocation.x), Int2Float(kTargetLocation.y));
			kTargetLocation.z = Float2Int(rkTerrainInfo._fLocalHeight);

			pkFxManager->BindFxToLocation(gfxID, UT_EngineMath::ConvertVec3IntToNiPoint3(kTargetLocation));
			pkFxManager->PlayFx(gfxID, fCurrentTime);
			pkEvent->SetParam(SK_SummorDamage::SEP_GFX_TICK, SK_SkillEvent::ParamFloat2LLong(fCurrentTime));
		}

		if ( pkSrcActor->GetMoveSpeed() == 0 )
		{
			pkEvent->SetParam(SK_SummorDamage::SEP_GFX_ID, gfxID);
		}
		else
		{
			pkEvent->SetParam(SK_SummorDamage::SEP_GFX_ID, 0);
		}
	}
	
	if ( duration + startTime < fCurrentTime )
	{
		bDestroyEvent = true;
	}
}

//------------------------------------------------------------
void SK_ClientSkillEventManager::_procEvent( float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent )
{
	SkillEventTypeEx eEventType = pkEvent->GetEventType();
	switch (eEventType)
	{
	case SETE_BULLET:
		_procBulletEvent(fCurrentTime, pkEvent, bDestroyEvent);
		break;
	case SETE_CLIENT_ATTACK:
		_procClientAttackEvent(fCurrentTime, pkEvent, bDestroyEvent);
		break;
	case SETE_SUMMOR_DAMAGE:
		_procSummorEvent(fCurrentTime, pkEvent, bDestroyEvent);
		break;
	default:
		break;
	}
}
//------------------------------------------------------------
static void _prepareLocalBulletExplosive(SK_SkillEvent* pkEvent, SK_BulletEvent::SK_BulletExplosiveReason enmReason, const UT_Vec3Int * pkExplosivePos = NULL)
{
	UT_Vec3Int kBulletLocation;
	if (pkExplosivePos == NULL)
	{
		kBulletLocation.x = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_X);
		kBulletLocation.y = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Y);
		kBulletLocation.z = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Z);
	}
	else
		kBulletLocation = *pkExplosivePos;

	pkEvent->SetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_X, kBulletLocation.x);
	pkEvent->SetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_Y, kBulletLocation.y);
	pkEvent->SetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_Z, kBulletLocation.z);

	pkEvent->SetParam(SK_BulletEvent::SEP_ACTION_TYPE, SK_BulletEvent::SBAT_EXPLOSIVE | enmReason);
}
//------------------------------------------------------------
static float get_bullet_extra_lifetime(SK_SkillEvent* pkEvent)
{
	int lActionType = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_ACTION_TYPE);
	bool bShotFromClient = (lActionType & SK_BulletEvent::SBSS_Mask) == SK_BulletEvent::SBSS_FromClient;
	if (bShotFromClient)		// ���ط�����ӵ������ȴ�
		return 0.0f;

	int lMaxPenetrateCount = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_MAX_PENETRATE_COUNT);
	if (lMaxPenetrateCount <= 0)		// ����Ҫ���̵��ӵ������ȴ�
		return 0.0f;

	object_id_type lSrcActorID = pkEvent->GetSrcActorID();
	LC_ClientWorldManager* pkWorldManager = CLIENT_GET_WORLD_MANAGER();
	LC_ActorBase* pkAttacker = (LC_ActorBase*)pkWorldManager->FindObject(lSrcActorID);
	if (NULL == pkAttacker)		// ������û�ˣ����ȴ��������ٻ��﷢���ӵ������
		return 0.0f;

	return 1.0f;
}

void SK_ClientSkillEventManager::_procBulletEvent( float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent )
{
	long lEventID = pkEvent->GetEventID();
	object_id_type lDstActorID = pkEvent->GetDstActorID();
	float fStartTime = SK_BulletEvent::ParamLLong2Float(pkEvent->GetParam(SK_BulletEvent::SEP_MOVE_START_TIME));
	float fLifeTime = SK_BulletEvent::ParamLLong2Float(pkEvent->GetParam(SK_BulletEvent::SEP_MOVE_LIFE_TIME));
	long lBulletLocalID = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_BULLET_LOCAL_ID);

	//Ĭ�������¼�
	bDestroyEvent = true;

	// ֻ����ActiveBuddy���ӵ��������ӵ���enemy����ײ
	int lActionType = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_ACTION_TYPE);
	bool bShotFromClient = (lActionType & SK_BulletEvent::SBSS_Mask) == SK_BulletEvent::SBSS_FromClient;
	int lMoveType = (int)pkEvent->GetParam(SK_BulletEvent::SEP_MOVE_TYPE);
	if (bShotFromClient)
	{
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
		}

		if (bDestroyEvent)
			return;
	}

	//����Ŀ�����ӵ���Ŀ���Ƿ����
	//if (IS_PLAYER_CLASS_ID(lDstActorID) || IS_NPC_ID(lDstActorID))
	//{
	//	LC_ClientWorldManager* pkWorldManager = CLIENT_GET_WORLD_MANAGER();
	//	LC_ActorBase* pkTarget = (LC_ActorBase*)pkWorldManager->FindObject(lDstActorID);
	//	if ((NULL == pkTarget || pkTarget->GetDead()) && lMoveType != BT_HOMINGMISSLE)
	//	{
	//		GfxWriteLog(LOG_TAG_SKILL_EVENT, LOG_SWITCH_SKILL_EVENT, "[Skill Bullet] exploded (TargetDestroy by client), lBulletLocalID=%d, lEventID=%d, time=%f", 
	//			lBulletLocalID, lEventID, fCurrentTime);

	//		_prepareLocalBulletExplosive(pkEvent, SK_BulletEvent::SBER_TargetDestroy);
	//		_bulletExplosive(pkEvent);
	//		return;
	//	}
	//}

	float fLifeTimeOverflow = fCurrentTime - fStartTime - fLifeTime;
	if (fLifeTimeOverflow > 0 && fLifeTimeOverflow > get_bullet_extra_lifetime(pkEvent) || (fCurrentTime - pkEvent->GetStartTime() > SKILL_EVENT_UPDATE_DELTA_TIME))
	{
		GfxWriteLog(LOG_TAG_SKILL_EVENT, LOG_SWITCH_SKILL_EVENT, "[Skill Bullet] exploded (Timeout by client), lBulletLocalID=%d, lEventID=%d, time=%f", 
			lBulletLocalID, lEventID, fCurrentTime);

		_prepareLocalBulletExplosive(pkEvent, SK_BulletEvent::SBER_Timeout);
		_bulletExplosive(pkEvent);
		bDestroyEvent = true;
		return;
	}

	//�������¼�
	bDestroyEvent = false;
}
//------------------------------------------------------------
void SK_ClientSkillEventManager::_bulletExplosive( SK_SkillEvent* pkEvent, bool bArenaPlayBack)
{
	UT_Vec3Int kBulletLocation, kShotLocation, kTargetLocation;

	// �¼�����
	long lEventID = pkEvent->GetEventID();
	object_id_type lSrcActorID = pkEvent->GetSrcActorID();
	object_id_type lDstActorID = pkEvent->GetDstActorID();
	if (bArenaPlayBack)
	{
		LC_ClientArenaPlayBackManager *pkArenaPlayBackManager = LC_ClientArenaPlayBackManager::GetSingletonPtr();
		lSrcActorID = pkArenaPlayBackManager->GetTemID(lSrcActorID);
		lDstActorID = pkArenaPlayBackManager->GetTemID(lDstActorID);
	}
	long lSkillID = pkEvent->GetLogicID();
	kBulletLocation.x = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_X);
	kBulletLocation.y = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_Y);
	kBulletLocation.z = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_Z);
	kTargetLocation.x = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_X);
	kTargetLocation.y = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Y);
	kTargetLocation.z = kBulletLocation.z;
	kShotLocation.x = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_SHOT_LOCATION_X);
	kShotLocation.y = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_SHOT_LOCATION_Y);
	kShotLocation.z = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_SHOT_LOCATION_Z);
	long lBulletFuncID = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_BULLET_FUNC);
	long lBulletLocalID = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_BULLET_LOCAL_ID);
	SK_BulletEvent::CollideParam kCollide = Skill::SerializeFromQuatWord<SK_BulletEvent::CollideParam>(pkEvent->GetParam(SK_BulletEvent::SEP_COLLIDE_PARAM));
	int iExplosiveType = ((int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_ACTION_TYPE)) & SK_BulletEvent::SBER_Mask;

	// ���Ҷ���
	LC_ClientWorldManager* pkWorldManager = CLIENT_GET_WORLD_MANAGER();
	LC_ActorBase* pkSrcActor = (LC_ActorBase*)pkWorldManager->FindObject(lSrcActorID);
	LC_ActorBase* pkDstActor = lDstActorID ? (LC_ActorBase*)pkWorldManager->FindObject(lDstActorID) : NULL;
	if (NULL != pkDstActor) 
	{
		kBulletLocation = pkDstActor->GetCurrentLocation();
		kBulletLocation.z += 120;
	}

	// ���ü��ܵ��ӵ���ը����
	SK_SkillExecute* pkSkillExecute = SK_Factory::GetSingletonPtr()->RequestSkillExcute(lSkillID);
	if (NULL != pkSkillExecute && NULL != pkSrcActor)
	{
		pkSrcActor->SetBulletLocation(kBulletLocation);
		pkSkillExecute->BulletExplosive(lBulletFuncID, pkSrcActor, lDstActorID, 0, BULLET_UUID(lEventID, lBulletLocalID));
	}

	// �ӵ���ըλ�ã���ը��Ч��ת����
	NiPoint3 kRotate = UT_EngineMath::ConvertVec3IntToNiPoint3(kShotLocation - kTargetLocation);
	kRotate.z = 0;
	kRotate.Unitize();

	//�������û�취�ˣ���������Ҳ�����ڣ������ӵ�ֻ��ǿ��һ������û����!!! ��Ȼ��Ƭ�����ⲻ�ܽ��
	if (kRotate.x == 0 && kRotate.y == 0)
	{
		if (pkSrcActor)
		{
			kRotate = UT_EngineMath::ConvertVec3FloatToNiPoint3(UT_MathBase::ConvertVec2To3Float(pkSrcActor->GetForwardDir()));
		}
		else if (kBulletLocation.x != kShotLocation.x && kBulletLocation.y != kShotLocation.y)
		{
			kRotate = UT_EngineMath::ConvertVec3IntToNiPoint3(kShotLocation - kBulletLocation);
		}
		else
		{
			kRotate = NiPoint3(1,1,0);
		}
		kRotate.z = 0;
		kRotate.Unitize();
	}

	// ֪ͨBulletManager�����ӵ�
	GD_Bullet * pkBullet = GD_BulletManager::GetSingleton().FindBullet(BULLET_UUID(lEventID, lBulletLocalID));
	if (!pkBullet)
	{
		GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "_bulletExplosive�Ҳ����ӵ� eventID:%d",lEventID);
		return;
	}
	else
	{
		GfxWriteLog( LOG_TAG_SKILL, LOG_SWITCH_SKILL_EVENT, "_bulletExplosive�����ӵ� eventID:%d",lEventID);
	}

	StringType strExplodeFxName = pkBullet->GetExplodeFxName();
	NiPoint3 kNiBulletLocation = UT_EngineMath::ConvertVec3IntToNiPoint3(kBulletLocation);
	GD_BulletManager::GetSingleton().BulletExplode(BULLET_UUID(lEventID, lBulletLocalID), iExplosiveType, kNiBulletLocation, &kRotate);

	// ֱ���ʹ���ײ��
	if (iExplosiveType == SK_BulletEvent::SBER_Timeout 
		&& kCollide.t < 1.0f)
	{
		// ��Ч�����˱�ը�����ܻ�
		if (!UT_BaseStringUtility::isValidConfig(strExplodeFxName))
		{
			const CF_SkillExecuteList::DataEntry* pkSkillData = pkSkillExecute->GetDataEntryPtr();
			const StringType * pkBeAttackGfx = Utility::RandomChooseValidCellText(&pkSkillData->_kBeAttackGfx, &pkSkillData->_kBeAttackGfx1, &pkSkillData->_kBeAttackGfx2);
			if (pkBeAttackGfx != NULL)
			{
				FX_ClientFxManager* pkFxManager = FX_ClientFxManager::GetSingletonPtr();
				int iFxID = pkFxManager->CreateFx(*pkBeAttackGfx, true);

				UT_Vec2Float fxDirection(kRotate.x, kRotate.y);
				fxDirection	= UT_MathBase::ZRot(fxDirection, 90);
				kRotate = UT_EngineMath::ConvertVec3FloatToNiPoint3( UT_MathBase::ConvertVec2To3Float(fxDirection) );
				pkFxManager->BindFxToLocation(iFxID, kNiBulletLocation, &kRotate);
				pkFxManager->PlayFx(iFxID, GET_PROCESS_TIME);
			}
		}

		// ��Ч
		LC_ClientLogicManager* pkLogicManager =  CLIENT_GET_LOGIC_MANAGER();
		LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();

		if (SC_ClientSkillInterface::CheckAttachSkillSound(pkGamePlayer, pkSrcActor))
		{
			int iSurfaceMaterialIdx = kCollide.surface_material;
			StringType strHitSoundFrom("bullet");
			StringType strHitSoundTo;
			UT_BaseStringUtility::format(strHitSoundTo, "%d", iSurfaceMaterialIdx);
			AS_ClientSoundManager* pkSoundManager = AS_ClientSoundManager::GetSingletonPtr();
			pkSoundManager->PlayHittedSound( kNiBulletLocation, strHitSoundFrom, strHitSoundTo );
		}
	}
}
//------------------------------------------------------------

static bool is_target_penetrated(SK_SkillEvent* pkEvent, object_id_type lTargetID)
{
	long lPenetrateCount = pkEvent->GetParam(SK_BulletEvent::SEP_PENETRATE_COUNT);
	for(int i = 0; i < lPenetrateCount; ++i)
	{
		long pid = pkEvent->GetParam(SK_BulletEvent::SEP_TARGET_BEGIN + i);
		if (pid == lTargetID)
			return true;
	}
	return false;
}

void Skill::SK_ClientSkillEventManager::_procBulletToTarget(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent)
{
	/*
		_procBulletToTarget���ӵ�����Ŀ�꣬�����в��������κ������������ж����ӵ�����ʱ����������̻��߱�ը�¼�
		����Ǵ��̣���¼��ǰĿ�꣬������Χֻȡһ��Ŀ�꣬��������
	*/
	object_id_type lSrcActorID = pkEvent->GetSrcActorID();
	object_id_type lDstActorID = pkEvent->GetDstActorID();
	long lBulletID = pkEvent->GetEventID();
	int lSkillID = pkEvent->GetLogicID();
	float fStartTime = SK_BulletEvent::ParamLLong2Float(pkEvent->GetParam(SK_BulletEvent::SEP_MOVE_START_TIME));
	float fMoveTime = SK_BulletEvent::ParamLLong2Float(pkEvent->GetParam(SK_BulletEvent::SEP_MOVE_LIFE_TIME));
	int lMoveSpeed = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_MOVE_SPEED);
	int lBulletFuncID = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_BULLET_FUNC);
	int lMaxPenetrateCount = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_MAX_PENETRATE_COUNT);
	int lPenetrateCount =(int32_t) pkEvent->GetParam(SK_BulletEvent::SEP_PENETRATE_COUNT);
	int lBulletLocalID =(int32_t) pkEvent->GetParam(SK_BulletEvent::SEP_BULLET_LOCAL_ID);

	//�����¼�
	bDestroyEvent = true;
	/*
	*�ӵ����η��н���,
	*1���������Ҫ��������Ŀ�꣬�Ҹ�Ŀ������ҵ������ӵ���͸��
	*2��������账������Ŀ�꣬or��һ��Ŀ���Ҳ��������ӵ���ը
	*/
	if (fCurrentTime - fStartTime > fMoveTime)
	{
		//�����ӵ��ͷ���
		LC_ClientWorldManager* pkWorldManager = CLIENT_GET_WORLD_MANAGER();
		LC_ActorBase* pkSrcActor = (LC_ActorBase*)pkWorldManager->FindObject(lSrcActorID);
		if (NULL == pkSrcActor)
		{
			bDestroyEvent = false;
			return;
		}
		//���ҵ�ǰĿ��
		
		SK_Factory* pkSkillFactory = SK_Factory::GetSingletonPtr();
		SK_SkillExecute* pkSkillExecute = pkSkillFactory->RequestSkillExcute(lSkillID);
		if (NULL == pkSkillExecute)
		{
			bDestroyEvent = false;
			return;
		}

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
			T_ASSERT_CRITICAL_S(lPenetrateCount >= 0 && lPenetrateCount < SK_BulletEvent::MAX_BULLET_PENETRATE_COUNT);
			pkEvent->SetParam(SK_BulletEvent::SEP_PENETRATE_COUNT, lPenetrateCount + 1);
			pkEvent->SetParam(SK_BulletEvent::SEP_TARGET_BEGIN + lPenetrateCount, lDstActorID);
			//�κγ�ԽSEP_TARGET_END�Ĵ�͸Ŀ���趨���ǷǷ��ģ��ᵼ��Խ�縲�Ǻ�������SEP_SHOT_LOCATION_X��SEP_SHOT_LOCATION_Y������MAX_SKILL_EVENT_PARAM_COUNT��������

			int lActorFilterFuncID = pkSkillExecute->GetProperty_LockTargetType();
			const SK_SkillExecute::BulletParam & kBP = pkSkillExecute->GetProperty_BulletParam(lBulletFuncID);
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
				const UT_Vec3Int & kNextTargetLocation = pkNextTarget->GetCurrentLocation();
				int iDistance = UT_MathBase::LineLengthXYInt(kTargetLocation, kNextTargetLocation);
				float fNextMoveTime = Long2Float(iDistance - pkCurrentTarget->GetProperty_ShapeRadius() - pkNextTarget->GetProperty_ShapeRadius()) / Long2Float(UT_MathBase::MaxInt(1, lMoveSpeed));

				pkEvent->SetDstActorID(lNextTargetID);
				pkEvent->SetParam(SK_BulletEvent::SEP_MOVE_START_TIME, SK_BulletEvent::ParamFloat2LLong(fCurrentTime));
				pkEvent->SetParam(SK_BulletEvent::SEP_MOVE_LIFE_TIME, SK_BulletEvent::ParamFloat2LLong(fNextMoveTime));

				//�򱻴���Ŀ��㲥������Ϣ
				_requestBulletPenetrate(pkCurrentTarget, (SK_BulletEvent*)pkEvent, kTargetLocation, lDstActorID, lNextTargetID);

				//��¼�ӵ�λ��
				pkSrcActor->SetBulletLocation(kTargetLocation);
				//��͸�������˺���Ŀ��
				pkSkillExecute->BulletPenetrate(lBulletFuncID, pkSrcActor, lDstActorID, lPenetrateCount, BULLET_UUID(lBulletID, lBulletLocalID));

				GD_BulletManager::GetSingleton().BulletPenetrate(BULLET_UUID(lBulletID,lBulletLocalID), lDstActorID, lNextTargetID, fNextMoveTime, fCurrentTime);

				//�������¼�
				bDestroyEvent = false;
				return;
			}
		}

		//�򱻱�ը�㲥��ը��Ϣ
		_requestBulletExplosive(pkCurrentTarget, (SK_BulletEvent*)pkEvent, kTargetLocation, lDstActorID, SK_BulletEvent::SBER_Timeout);

		//��¼�ӵ�λ��
		pkSrcActor->SetBulletLocation(kTargetLocation);
		//��ը����
		pkSkillExecute->BulletExplosive(lBulletFuncID, pkSrcActor, lDstActorID, lPenetrateCount, BULLET_UUID(lBulletID, lBulletLocalID));
	}
	else
	{
		//�������¼�
		LC_ClientWorldManager* pkWorldManager = CLIENT_GET_WORLD_MANAGER();
		LC_ActorBase* pkSrcActor = (LC_ActorBase*)pkWorldManager->FindObject(lSrcActorID);
		if (NULL != pkSrcActor)
		{
			//���ҵ�ǰĿ��
			LC_ActorBase* pkCurrentTarget = (LC_ActorBase*)pkWorldManager->FindObject(lDstActorID);
			if (NULL != pkCurrentTarget)
			{
				const UT_Vec3Int & kTargetLocation = pkCurrentTarget->GetCurrentLocation();
				pkEvent->SetParam(SK_BulletEvent::SEP_TARGET_LOCATION_X, kTargetLocation.x);
				pkEvent->SetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Y, kTargetLocation.y);
				pkEvent->SetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Z, kTargetLocation.z);
			}
		}

		bDestroyEvent = false;
	}
}

void Skill::SK_ClientSkillEventManager::_procBulletToLocation(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent)
{
	/*
		_procBulletToLocation���ӵ�����Ŀ��㣬���bProcHalfwayΪtrue�������ж������κ������������ж����ӵ������¼�������ը
		����Ǵ��̣�������Χ����һ��Ŀ�꣨Ŀ��������SkillBulletFunc.csv�������ã���������̣�������󴩴�����ը
	*/
	object_id_type lSrcActorID = pkEvent->GetSrcActorID();
	int lBulletID = pkEvent->GetEventID();
	int lSkillID = pkEvent->GetLogicID();
	float fStartTime = SK_BulletEvent::ParamLLong2Float(pkEvent->GetParam(SK_BulletEvent::SEP_MOVE_START_TIME));
	float fMoveTime = SK_BulletEvent::ParamLLong2Float(pkEvent->GetParam(SK_BulletEvent::SEP_MOVE_LIFE_TIME));
	int lMoveSpeed = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_MOVE_SPEED);
	int lBulletFuncID = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_BULLET_FUNC);
	int lMaxPenetrateCount = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_MAX_PENETRATE_COUNT);
	int lPenetrateCount = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_PENETRATE_COUNT);
	int lBulletLocalID = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_BULLET_LOCAL_ID);
	float fLastHoverTime = SK_BulletEvent::ParamLLong2Float(pkEvent->GetParam(SK_BulletEvent::SEP_LAST_UPDATE_TIME));

	//�����¼�
	bDestroyEvent = true;

	//�����ӵ��ͷ���
	LC_ClientWorldManager* pkWorldManager = CLIENT_GET_WORLD_MANAGER();
	LC_ActorBase* pkSrcActor = (LC_ActorBase*)pkWorldManager->FindObject(lSrcActorID);
	if (NULL == pkSrcActor)
	{
		bDestroyEvent = false;
		return;
	}

	SK_Factory* pkSkillFactory = SK_Factory::GetSingletonPtr();
	SK_SkillExecute* pkSkillExecute = pkSkillFactory->RequestSkillExcute(lSkillID);
	if (NULL == pkSkillExecute)
	{
		bDestroyEvent = false;
		return;
	}

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
		_requestBulletExplosive(pkSrcActor, (SK_BulletEvent*)pkEvent, kTargetLocation, 0, SK_BulletEvent::SBER_Timeout);

		//��¼�ӵ�λ��
		_prepareLocalBulletExplosive(pkEvent, SK_BulletEvent::SBER_Timeout);
		_bulletExplosive(pkEvent);
		return;
	}
	else if (bProcHalfway)	//��Ҫ�����м����
	{
		bool bPeneStraight = kBP.m_bPeneStraight;
		//��ȡ�ӵ�����
		int lMaxLockCount = kBP.m_lMaxLockCount;
		if (lMaxLockCount <= 0)
		{
			bDestroyEvent = false;
			return;
		}

		int lBulletShapeRadius = kBP.m_lShapeRadius;
		if (lBulletShapeRadius <= 0)
		{
			bDestroyEvent = false;
			return;
		}


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
				pkSkillExecute->BulletPenetrate(lBulletFuncID, pkSrcActor, lTargetID, lPenetrateCount, BULLET_UUID(lBulletID, lBulletLocalID));	//��ǰλ�ô���ֻ�ýű������˺������ı��ӵ��켣

				//��¼�������Ŀ��

				if (lPenetrateCount<0)
				{
					bDestroyEvent = false;
					return;
				}
				lPenetrateCount ++;
				pkEvent->SetParam(SK_BulletEvent::SEP_PENETRATE_COUNT, lPenetrateCount);
				pkEvent->SetParam(SK_BulletEvent::SEP_TARGET_BEGIN + lPenetrateCount - 1, lTargetID);

				//�ͻ����յ�������Ϣ�����pkSkillExecute->BulletPenetrate()��BulletManager->BulletPenetrate()
				//ʵ���ϴ��̺͸ı�켣Ӧ���������¼����ͻ��˿���ʹ�ô������ı�켣��
				_requestBulletPenetrate(pkCurrentTarget, (SK_BulletEvent*)pkEvent, kBulletLocation, lTargetID, 0);

				GD_BulletManager::GetSingleton().BulletPenetrate(BULLET_UUID(lBulletID,lBulletLocalID), lTargetID, 0, fMoveTime, fCurrentTime);	// �˴�����timeûɶ��
			}
			else if (bPeneStraight)
			{
				//��¼�ӵ�λ��
				pkSrcActor->SetBulletLocation(kBulletLocation);
				//�ӵ���͸
				pkSkillExecute->BulletPenetrate(lBulletFuncID, pkSrcActor, lTargetID, lPenetrateCount, BULLET_UUID(lBulletID, lBulletLocalID));	//��ǰλ�ô���ֻ�ýű������˺������ı��ӵ��켣

				lPenetrateCount ++;
				pkEvent->SetParam(SK_BulletEvent::SEP_PENETRATE_COUNT, lPenetrateCount);
				pkEvent->SetParam(SK_BulletEvent::SEP_TARGET_0, lTargetID);

				//�ͻ����յ�������Ϣ�����pkSkillExecute->BulletPenetrate()��BulletManager->BulletPenetrate()
				//ʵ���ϴ��̺͸ı�켣Ӧ���������¼����ͻ��˿���ʹ�ô������ı�켣��
				_requestBulletPenetrate(pkCurrentTarget, (SK_BulletEvent*)pkEvent, kBulletLocation, lTargetID, 0);

				GD_BulletManager::GetSingleton().BulletPenetrate(BULLET_UUID(lBulletID,lBulletLocalID), lTargetID, 0, fMoveTime, fCurrentTime);	// �˴�����timeûɶ��
			}
			else	//��͸Ŀ�������㹻����ը
			{
				_requestBulletExplosive(pkSrcActor, (SK_BulletEvent*)pkEvent, kBulletLocation, lTargetID, SK_BulletEvent::SBER_PernerateFull);

				_prepareLocalBulletExplosive(pkEvent, SK_BulletEvent::SBER_PernerateFull, &kBulletLocation);

				pkEvent->SetDstActorID(lTargetID);
				_bulletExplosive(pkEvent);

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
			int lMaxMoveDistance = kBP.m_lMaxMoveDistance;
			const UT_Vec3Int & kNextTargetLocation = pkNextTarget->GetCurrentLocation();	//pkNextTargetֻ������ȷ����һ�����䷽��
			UT_Vec3Int kTargetLocation = UT_MathBase::MoveAlongIntXY(kBulletLocation, kNextTargetLocation, Long2Float(lMaxMoveDistance));
			float fNextMoveTime = Long2Float(lMaxMoveDistance) / Long2Float(lMoveSpeed);

			pkEvent->SetParam(SK_BulletEvent::SEP_MOVE_START_TIME, SK_BulletEvent::ParamFloat2LLong(fCurrentTime));
			pkEvent->SetParam(SK_BulletEvent::SEP_MOVE_LIFE_TIME, SK_BulletEvent::ParamFloat2LLong(fNextMoveTime));
			pkEvent->SetParam(SK_BulletEvent::SEP_COLLIDE_PARAM, Skill::SerializeToQuatWord(SK_BulletEvent::CollideParam(1.0f, 0)));	// �޸�Ŀ��ʱδ����ײ��������ײ����

			// ����������󣬸ı�켣��Ϣ
			_requestBulletChangeTargetLocation(pkCurrentTarget, (SK_BulletEvent*)pkEvent, kBulletLocation, kTargetLocation, lTargetID, lNextTargetID);

			GD_BulletManager::GetSingleton().BulletChangeTargetLocation(BULLET_UUID(lBulletID,lBulletLocalID), 
				UT_EngineMath::ConvertVec3IntToNiPoint3(kBulletLocation), 
				UT_EngineMath::ConvertVec3IntToNiPoint3(kTargetLocation), 
				fNextMoveTime, fCurrentTime);
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

object_id_vector Skill::SK_ClientSkillEventManager::_bulletLockTarget(GameLogic::LC_ActorBase* pkAttacker, SK_BulletEvent* pkBulletEvent, const Utility::UT_Vec3Int& rkCircleCenter, 
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

void Skill::SK_ClientSkillEventManager::_requestBulletChangeTargetLocation(GameLogic::LC_ActorBase* pkBroadcastCenter, SK_BulletEvent* pkEvent, const Utility::UT_Vec3Int& rkBulletLocation, const Utility::UT_Vec3Int& rkTargetLocation, long lTargetID, long lNextTargetID)
{
	MG_REQ_Bullet_ChangeTargetLocation req;
	req.m_lRequesterID		= pkEvent->GetSrcActorID();
	req.m_lSkillID			= pkEvent->GetLogicID();
	req.m_lBulletEventID	= pkEvent->GetEventID();
	req.m_lBulletLocalID	= (uint32_t)pkEvent->GetParam(SK_BulletEvent::SEP_BULLET_LOCAL_ID);
	req.m_lBulletLocationX	= rkBulletLocation.x;
	req.m_lBulletLocationY	= rkBulletLocation.y;
	req.m_lBulletLocationZ	= rkBulletLocation.z;
	req.m_lTargetLocationX	= rkTargetLocation.x;
	req.m_lTargetLocationY	= rkTargetLocation.y;
	req.m_lTargetLocationZ	= rkTargetLocation.z;
	req.m_lFromTargetID		= lTargetID;
	req.m_lToTargetID		= lNextTargetID;

	pkEvent->SetParam(SK_BulletEvent::SEP_TARGET_LOCATION_X, rkTargetLocation.x);
	pkEvent->SetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Y, rkTargetLocation.y);
	pkEvent->SetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Z, rkTargetLocation.z);
	pkEvent->SetParam(SK_BulletEvent::SEP_SHOT_LOCATION_X, rkBulletLocation.x);
	pkEvent->SetParam(SK_BulletEvent::SEP_SHOT_LOCATION_Y, rkBulletLocation.y);
	pkEvent->SetParam(SK_BulletEvent::SEP_SHOT_LOCATION_Z, rkBulletLocation.z);

	// ����Ϣ֪ͨ������
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_BULLET_CHANGE_TARGET_LOCATION, &req);
	if (bCreateMsg)
	{
		LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
		LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();

		pkGamePlayer->GetSession()->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}

	GfxWriteLog(LOG_TAG_SKILL_EVENT, LOG_SWITCH_SKILL_EVENT, "[Skill Bullet] req ChangeTargetLocation, lBulletID=%d, lBulletLocalID=%d, t=%f", req.m_lBulletEventID, req.m_lBulletLocalID, GET_PROCESS_TIME);
}

void Skill::SK_ClientSkillEventManager::_requestBulletPenetrate(GameLogic::LC_ActorBase* pkBroadcastCenter, SK_BulletEvent* pkEvent, const Utility::UT_Vec3Int& rkBulletLocation, long lTargetID, long lNextTargetID)
{
	MG_REQ_Bullet_Penetrate req;
	req.m_lRequesterID		= pkEvent->GetSrcActorID();
	req.m_lSkillID			= pkEvent->GetLogicID();
	req.m_lBulletEventID	= pkEvent->GetEventID();
	req.m_lBulletLocalID	= (uint32_t)pkEvent->GetParam(SK_BulletEvent::SEP_BULLET_LOCAL_ID);
	req.m_lToucherID		= lTargetID;
	req.m_lNextTargetID		= lNextTargetID;
	req.m_lBulletLocationX	= rkBulletLocation.x;
	req.m_lBulletLocationY	= rkBulletLocation.y;
	req.m_lBulletLocationZ	= rkBulletLocation.z;

	pkEvent->SetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_X, rkBulletLocation.x);
	pkEvent->SetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_Y, rkBulletLocation.y);
	pkEvent->SetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_Z, rkBulletLocation.z);

	// ����Ϣ֪ͨ������
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_BULLET_PENETRATE, &req);
	if (bCreateMsg)
	{
		LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
		LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();

		pkGamePlayer->GetSession()->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}

	GfxWriteLog(LOG_TAG_SKILL_EVENT, LOG_SWITCH_SKILL_EVENT, "[Skill Bullet] req Penetrate, lBulletID=%d, lBulletLocalID=%d, lPenetrateTargetID=%d, t=%f", req.m_lBulletEventID, req.m_lBulletLocalID, lTargetID, GET_PROCESS_TIME);
}

void Skill::SK_ClientSkillEventManager::_requestBulletExplosive(GameLogic::LC_ActorBase* pkBroadcastCenter, SK_BulletEvent* pkEvent, const Utility::UT_Vec3Int& rkBulletLocation, long lTargetID, SK_BulletEvent::SK_BulletExplosiveReason enmReason)
{
	MG_REQ_Bullet_Explosive req;
	req.m_lRequesterID		= pkEvent->GetSrcActorID();
	req.m_lSkillID			= pkEvent->GetLogicID();
	req.m_lBulletEventID	= pkEvent->GetEventID();
	req.m_lBulletLocalID	= (uint32_t)pkEvent->GetParam(SK_BulletEvent::SEP_BULLET_LOCAL_ID);
	req.m_lToucherID		= lTargetID;
	req.m_iExplosiveReason  = enmReason;
	req.m_lBulletLocationX	= rkBulletLocation.x;
	req.m_lBulletLocationY	= rkBulletLocation.y;
	req.m_lBulletLocationZ	= rkBulletLocation.z;

	pkEvent->SetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_X, rkBulletLocation.x);
	pkEvent->SetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_Y, rkBulletLocation.y);
	pkEvent->SetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_Z, rkBulletLocation.z);

	// ����Ϣ֪ͨ������
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_BULLET_EXPLOSIVE, &req);
	{
		LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
		LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();

		pkGamePlayer->GetSession()->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}

	GfxWriteLog(LOG_TAG_SKILL_EVENT, LOG_SWITCH_SKILL_EVENT, "[Skill Bullet] req Explosive, lBulletID=%d, lBulletLocalID=%d, lPenetrateTargetID=%d, reason=0x%x, t=%f", req.m_lBulletEventID, req.m_lBulletLocalID, lTargetID, enmReason, GET_PROCESS_TIME);
}

void Skill::SK_ClientSkillEventManager::_modifyBulletZValue(SK_SkillEvent* pkEvent)
{
	UT_Vec3Int kTargetLocation;
	kTargetLocation.x = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_X);
	kTargetLocation.y = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Y);
	kTargetLocation.z = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Z);

	const SE_SceneTerrainLocalInfo& rkTerrainInfo1 = SE_SceneManager::GetSingletonPtr()->GetWalkableInfo(Int2Float(kTargetLocation.x), Int2Float(kTargetLocation.y));
	int32_t height = rkTerrainInfo1._fLocalHeight;

	/*if (height > kTargetLocation.z + 300 || height < kTargetLocation.z-300)*/
	if ((height > kTargetLocation.z + 300 || height < kTargetLocation.z-300))
	{
		long skillID = pkEvent->GetLogicID();
		SK_Factory* pkSkillFactory = SK_Factory::GetSingletonPtr();
		SK_SkillExecute* pSkillExecute = pkSkillFactory->RequestSkillExcute(skillID);
		if (NULL != pSkillExecute)
		{
			int32_t bulletFuncID = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_BULLET_FUNC);
			const CSVFile::CF_SkillBulletFunc::DataEntry * pBullet = pSkillExecute->GetBulletDataEntryPtr(bulletFuncID);
			if (pBullet)
			{
				UT_Vec3Float kShotLocalOffset = UT_MathBase::ConvertVec3IntToFloat(pBullet->_kShotPosLocalOffset);
				height += kShotLocalOffset.z;
			}
		}

		pkEvent->SetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Z, height);
	}
}

//------------------------------------------------------------
void SK_ClientSkillEventManager::_procClientAttackEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent)
{
	bDestroyEvent = true;
	
	LC_ClientLogicManager* pkLogicManager =  CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager)
	{
		GfxWriteLog(LOG_SKILL_EVENT_FILELINE, LOG_SWITCH_SKILL_EVENT, "��ȡ�ͻ����߼�����������");
		return;
	}
	object_id_type lGamePlayerID = pkLogicManager->GetInputControlActorID();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (NULL == pkGamePlayer)
	{
		GfxWriteLog(LOG_SKILL_EVENT_FILELINE, LOG_SWITCH_SKILL_EVENT, "��ȡ��Ϸ��Ҵ���");
		return;
	}

	object_id_type lSrcActorID = pkEvent->GetSrcActorID();
	object_id_type lDstActorID = pkEvent->GetDstActorID();
	long lLogicID = pkEvent->GetLogicID();
	long lDamageType = (int32_t)pkEvent->GetParam(SK_DamageEvent::SEP_DAMAGE_TYPE);
	long lDamageResult = (int32_t)pkEvent->GetParam(SK_DamageEvent::SEP_RESULT);
	int64_t lFinalDamage = pkEvent->GetParam(SK_DamageEvent::SEP_FINAL_DAMAGE);

	LC_ClientWorldManager* pkWorldManager = CLIENT_GET_WORLD_MANAGER();
	LC_ActorBase* pkSrcActor = (LC_ActorBase*)pkWorldManager->FindObject(lSrcActorID);
	LC_ActorBase* pkDstActor = (LC_ActorBase*)pkWorldManager->FindObject(lDstActorID);
	if (NULL == pkSrcActor)
	{
		GfxWriteLog(LOG_SKILL_EVENT_FILELINE, LOG_SWITCH_SKILL_EVENT, "�˺��¼������Ҳ���������[%i]", lSrcActorID);
		return;
	}
	if (NULL == pkDstActor)
	{
		GfxWriteLog(LOG_SKILL_EVENT_FILELINE, LOG_SWITCH_SKILL_EVENT, "�˺��¼������Ҳ���������[%i]", lDstActorID);
		return;
	}
	
	CF_SkillExecuteList::DataEntry* pkSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(lLogicID);	
	/*
	*���ű�����Ч
	*/
	IM_Manager* pkIMManager =  IM_Manager::GetSingletonPtr();

	GD_ActorModel* pkVisibleData = pkIMManager->GetActor(lDstActorID);
	if(NULL == pkVisibleData)
	{
		return;
	}

	/*
	*���ű�������
	*/
	//��ҷǼ���״̬�²��ű�������
	if (IS_PLAYER_CLASS_ID(lDstActorID) && !pkDstActor->GetDead()
        && !IS_SKILL_TYPE(pkDstActor->GetCurrentStateType()) 
		&& (pkDstActor->GetCurrentStateType() != ST_PLAYER_RUN)
		&& (pkDstActor->GetCurrentStateType() != ST_PLAYER_PUNCH_BACK
		&& (pkDstActor->GetCurrentStateType() != ST_PLAYER_PASSIVE_MOVE))
		)
	{	
 		//if(pkDstActor->GetRiderCharType() == 0)
 		//	pkVisibleData->ActivateAnimation(LC_Helper::GetPlayerBeAttackAnimName((LC_PlayerBase*)pkDstActor),0,0.05f,1.0f);
 		//else
 		//    pkVisibleData->ActivateAnimation(RES_ANM_ACTOR_VEHICLE_UNDERATTACK,0,0.05f,1.0f);
	}
	else if (IS_NPC_ID(lDstActorID) && !pkDstActor->GetDead())
	{
		//NPC�Ǽ���״̬���ںϱ�������
 		//if (!IS_SKILL_TYPE(pkDstActor->GetCurrentStateType()) )
 		//{
 		//	if (pkDstActor->GetCurrentStateType() != ST_NPC_PUNCH_BACK)		//jz, ayu������״̬���ܲ����ܻ�����������ֻѡһ����Ϊ���˶������߶�λ��
 		//		pkVisibleData->ActivateAnimation(RES_ANM_ACTOR_UNDERATTACK,0,0.05f,1.0f);
 		//}
 		//else
 		//{
 		//	if (NULL != pkSkillData 
 		//		&& pkSkillData->_fBeAttackInterruptPoint > 0 
 		//		&& GET_PROCESS_TIME - pkDstActor->GetCurrentStateStartTime() >= pkSkillData->_fBeAttackInterruptPoint)
 		//	{
 		//		pkVisibleData->PlayAnimation(RES_ANM_ACTOR_UNDERATTACK);
 		//	}
 		//}
	}
	
	if (lGamePlayerID != lDstActorID)
		pkVisibleData->Flash(0.3f,0.1f,0.3f);

	/*
	*���ű�����Ч����Ч
	*/
	if (NULL != pkSkillData)
	{
		//��Чת��ʹ��Ŀ���͹��������Է���
		/*const Utility::UT_Vec2Float& kDir = pkSrcActor->GetForwardDir();
		NiPoint3 pkRotate = NiPoint3::ZERO;
		pkRotate.z = NiATan2(kDir.y, kDir.x);*/

        // �������ڵ���/�������²���������Ч
       /* if (pkGamePlayer->GetMapLogicType() > MT_TRANSCRIPTION_BEGIN && pkGamePlayer->GetMapLogicType() < MT_TRANSCRIPTION_END
            && g_settings.CFG_DISABLE_ATTACKED_FX)
        {
        }
        else
        {*/

            //pkVisibleData->AttachFx(GD_ModelBase::FxDummyNodeType(pkSkillData->_lBeAttackGfxBindDumy),0,true,pkSkillData->_kBeAttackGfx, NULL, &pkRotate);

			//��Чת��
// 			NiPoint3 pkRotate = NiPoint3::ZERO;
// 			Utility::UT_Vec3Int diff3 = pkDstActor->GetVisibleLocation() - pkSrcActor->GetVisibleLocation();
// 			pkRotate.z = NiATan2(-Int2Float(diff3.y), Int2Float(diff3.x));
// 
// 			if (!pkSkillData->_kBeAttackGfx.empty() && pkSkillData->_kBeAttackGfx != "NULL")
// 			{
// 				pkVisibleData->AttachFx(GD_ModelBase::FxDummyNodeType(pkSkillData->_lBeAttackGfxBindDumy),0,true,pkSkillData->_kBeAttackGfx, NULL, &pkRotate);
// 			}
        //}
		LC_ClientGamePlayer* pkGamePlayer = (CLIENT_GET_LOGIC_MANAGER())->GetOperationPlayer();
		if ( pkSrcActor->GetID() != pkGamePlayer->GetID() 
			&& pkDstActor->GetID() != pkGamePlayer->GetID() )
		{
			//���Ŵ����Ч
			_playHitSound(pkSrcActor, pkDstActor, pkSkillData);
		}
	}
}
//------------------------------------------------------------
void SK_ClientSkillEventManager::_procClientEyeSkillEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool bArenaPlayBack)
{
	LC_ClientLogicManager* pkLogicManager =  CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager)
	{
		GfxWriteLog(LOG_SKILL_EVENT_FILELINE, LOG_SWITCH_SKILL_EVENT, "��ȡ�ͻ����߼�����������");
		return;
	}
	object_id_type lGamePlayerID = pkLogicManager->GetInputControlActorID();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (NULL == pkGamePlayer)
	{
		GfxWriteLog(LOG_SKILL_EVENT_FILELINE, LOG_SWITCH_SKILL_EVENT, "��ȡ��Ϸ��Ҵ���");
		return;
	}

	object_id_type lSrcActorID = pkEvent->GetSrcActorID();
	object_id_type lDstActorID = pkEvent->GetDstActorID();
	if (bArenaPlayBack)
	{
		LC_ClientArenaPlayBackManager *pkArenaPlayBackManager = LC_ClientArenaPlayBackManager::GetSingletonPtr();
		lSrcActorID = pkArenaPlayBackManager->GetTemID(lSrcActorID);
		lDstActorID = pkArenaPlayBackManager->GetTemID(lDstActorID);
	}
	long skillID = (int32_t)pkEvent->GetParam(SK_ClientEyeSkillEvent::SEP_SKILL_ID);
	

	LC_ActorBase* caller = NULL;
	if ( lSrcActorID == lGamePlayerID )
	{
		caller = pkGamePlayer; 
	}
	else
	{
		LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
		caller = pkNetPlayerManager->FindPlayer(lSrcActorID);
	}

	SC_ClientScriptInterface* pkClientInterface = SC_ClientScriptInterface::GetSingletonPtr();
	SC_ClientSkillInterface* pkInterface = pkClientInterface->GetSkillInterface();

	pkInterface->EyeExecuteSkill(caller, skillID, lDstActorID);

}
//------------------------------------------------------------
void SK_ClientSkillEventManager::_notifyState(LC_ActorBase* pkSrcActor, LC_ActorBase* pkDstActor, SK_SkillEvent* pkEvent)
{
	/*if (NULL != pkSrcActor)
	{
		LC_ActorStateMap* pkStateMap = pkSrcActor->GetStateMap();
		if (NULL != pkStateMap)
		{
			pkStateMap->NotifySkillEventToStateMap(pkEvent,true);
		}
	}
	if (NULL != pkDstActor)
	{
		LC_ActorStateMap* pkStateMap = pkDstActor->GetStateMap();
		if (NULL != pkStateMap)
		{
			pkStateMap->NotifySkillEventToStateMap(pkEvent,true);
		}
	}*/
}
//------------------------------------------------------------
void SK_ClientSkillEventManager::_playHitSound( LC_ActorBase* pkAttacker, LC_ActorBase* pkTarget, CF_SkillExecuteList::DataEntry* pkSkillData )
{
	CF_ItemList* pkItemList = CF_ItemList::GetSingletonPtr();

	if (NULL == pkAttacker || NULL == pkTarget || NULL == pkSkillData)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "���ò�����Ч�ӿڴ���");
		return;
	}
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "��ȡ�ͻ����߼�����������");
		return;
	}
	object_id_type lGamePlayerID = pkLogicManager->GetInputControlActorID();


	StringType kAttackerMaterial = "NULL";
	StringType kTargetMaterial = "NULL";

	//�����"xx"�Ͳ����ű�����Ч
	if (kAttackerMaterial == "xx")
		return ;

	/*
	*��ȡ�����߲���
	*/
	if ( "NULL" != pkSkillData->_kMaterial )
	{
		kAttackerMaterial = pkSkillData->_kMaterial;
	}
	else
	{
		object_id_type lAttackerID = pkAttacker->GetID();
		if (IS_PLAYER_CLASS_ID(lAttackerID))
		{
			long lWeaponID = 0;
			if (lGamePlayerID == lAttackerID)
			{
				LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkAttacker;
				lWeaponID = pkGamePlayer->GetCurrentEquipItem(ET_WEAPON);
			}
			else
			{
				LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
				LC_ClientNetPlayer* pkNetPlayer = (LC_ClientNetPlayer*)pkNetPlayerManager->FindPlayer(lAttackerID);
				if (NULL == pkNetPlayer)
				{
					return;
				}

				lWeaponID = pkNetPlayer->GetCurrentEquipItem(ET_WEAPON);
			}
			if (IS_ITEM_ID(lWeaponID))
			{
				CF_ItemList::DataEntry* pkItemData = pkItemList->GetEntryPtr(lWeaponID);
				if (NULL != pkItemData )
				{
					CF_ItemAvartarFunc* pkItemAvartar = CF_ItemAvartarFunc::GetSingletonPtr();
					CF_ItemAvartarFunc::DataEntry* pkItemAvartarData = pkItemAvartar->GetEntryPtr( pkItemData->_lAvartarFuncID );
					if ( 0 == pkItemAvartarData )
					{
						GfxWriteLog( LOG_TAG_ERROR, LOG_SWITCH_ERROR, "ItemList.csv gets foreign data ERROR: In ItemAvartarFunc.csv key==%ld  is not found!", pkItemData->_lAvartarFuncID );
					}
					else if ( "NULL" != pkItemAvartarData->_kHitted )
					{
						kAttackerMaterial = pkItemAvartarData->_kHitted;
					}
				}
			}
			else
			{
				kAttackerMaterial = "fist";
			}
		}
	}


	/*
	*��ȡĿ�����
	*/
	object_id_type lTargetID = pkTarget->GetID();
	if (IS_NPC_ID(lTargetID))
	{
		CF_CharType* pkCharList = CF_CharType::GetSingletonPtr();
		CF_CharType::DataEntry* pkCharData = pkCharList->GetEntryPtr(pkTarget->GetCharType());
		if (NULL == pkCharData)
		{
			GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "���ܻ�ȡ��ɫ[%d]��CharType��Ϣ", lTargetID);
			return;
		}
		if ("NULL" != pkCharData->_kHitted)
		{
			kTargetMaterial = pkCharData->_kHitted;
		}
	}
	else if (IS_PLAYER_CLASS_ID(lTargetID))
	{
		long lBodyEquipID = 0;
		if (lGamePlayerID == lTargetID)
		{
			LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkTarget;
			lBodyEquipID = pkGamePlayer->GetCurrentEquipItem(ET_BODY);
		}
		else
		{
			LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
			LC_ClientNetPlayer* pkNetPlayer = (LC_ClientNetPlayer*)pkNetPlayerManager->FindPlayer(lTargetID);
			if (NULL == pkNetPlayer)
			{
				return;
			}

			lBodyEquipID = pkNetPlayer->GetCurrentEquipItem(ET_BODY);
		}
		if (IS_ITEM_ID(lBodyEquipID))
		{
			CF_ItemList::DataEntry* pkItemData = pkItemList->GetEntryPtr(lBodyEquipID);
			if ( NULL != pkItemData )
			{
				CF_ItemAvartarFunc* pkItemAvartar = CF_ItemAvartarFunc::GetSingletonPtr();
				CF_ItemAvartarFunc::DataEntry* pkItemAvartarData = pkItemAvartar->GetEntryPtr( pkItemData->_lAvartarFuncID );
				if ( 0 == pkItemAvartarData )
				{
					GfxWriteLog( LOG_TAG_ERROR, LOG_SWITCH_ERROR, "ItemList.csv gets foreign data ERROR: In ItemAvartarFunc.csv key==%ld  is not found!", pkItemData->_lAvartarFuncID );
				}
				else if ( "NULL" != pkItemAvartarData->_kHitted )
				{
					kTargetMaterial = pkItemAvartarData->_kHitted;
				}
			}
		}
	}

	AS_ClientSoundManager* pkSoundManager = AS_ClientSoundManager::GetSingletonPtr();
	pkSoundManager->PlayHittedSound( 
		UT_EngineMath::ConvertVec3IntToNiPoint3(
		pkTarget->GetVisibleLocation()), kAttackerMaterial, kTargetMaterial );
}
//------------------------------------------------------------
void SK_ClientSkillEventManager::_playBeAttackScreenGfx(LC_ActorBase* pkAttacker)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (NULL == pkGamePlayer)
	{
		return;
	}

	if (NULL == pkAttacker || !IS_PLAYER_CLASS_ID(pkAttacker->GetID()))
	{
		if (pkGamePlayer->GetMapResID() != QINGGONG_TRIAL_MAPID)  //2601 �Ṧ�����ĵ�ͼid
			return;
	}
	LC_PlayerBase* pkAttackerPlayer = (LC_PlayerBase*)pkAttacker;

	//����������
	if (pkGamePlayer->GetMapLogicType() > MT_ARENA_BEGIN && pkGamePlayer->GetMapLogicType() < MT_ARENA_END)
	{
		return;
	}

	//���ھ���״̬
	//if (LC_PlayerDuel_InSameDuel(pkGamePlayer->GetDuelState(), pkAttackerPlayer->GetDuelState()))
	//{
	//	return;
	//}

	FX_ClientFxManager* pkFxManager = FX_ClientFxManager::GetSingletonPtr();

	static int iGfxID = 0;
	bool bPlayGfx = true;
	if (iGfxID > 0 && pkFxManager->IsFxExists(iGfxID))
	{
		bPlayGfx = false;
	}
	if (bPlayGfx)
	{
		iGfxID = pkFxManager->CreateFx(RES_FX_UNDER_ATTACK,true);
		pkFxManager->PlayFx(iGfxID, GET_PROCESS_TIME);
	}
}
void SK_ClientSkillEventManager::_createDisplayHolder(const UT_Vec3Int& kLocation, int lItemTypeID, int FlyToID, int flyType)
{
	LC_ClientPlaceHolderManager* pkHoldMgr = CLIENT_GET_PLACEHOLDER_MANAGER();
	LC_ClientWorldManager* pkWorldManager = CLIENT_GET_WORLD_MANAGER();
	if(pkHoldMgr && pkWorldManager)
	{
		LC_ClientPlaceHolder* pkPlaceHolder = (LC_ClientPlaceHolder*)pkHoldMgr->CreateDisplayPlaceHolder();
		if(pkPlaceHolder)
		{
			pkPlaceHolder->SetCurrentLocation(kLocation);
			pkPlaceHolder->SetVisibleLocation(kLocation);
			pkPlaceHolder->SetSpawnZRotAngle(0);
			pkPlaceHolder->SetItemTypeID(lItemTypeID);
			pkPlaceHolder->SetItemIdentify(0);
			pkPlaceHolder->SetFromLocationPos(kLocation);
			pkPlaceHolder->SetLifeStartTime(10.0f);
			pkPlaceHolder->GetReward().m_eType = LC_Reward::RT_NONE;
			pkPlaceHolder->GetReward().m_Value = 0;
			if (!pkPlaceHolder->IsCreateVisibleData())
			{
				T_ASSERT2(GfxIsMainThread() == true, "pkPlaceHolder->CreateVisibleData() ֻ�������߳��б����ã�");
				pkPlaceHolder->CreateDisplayVisibleData(true, flyType, FlyToID);
			}

			pkWorldManager->AddObjectToMap(pkWorldManager->GetCurrentMapID(),pkPlaceHolder);

			pkPlaceHolder->SetVisualState();
			pkPlaceHolder->SetCanDelete(false);
		}
	}
}
//------------------------------------------------------------
SK_SkillEvent* SK_ClientSkillEventManager::_allocEvent()
{
	SK_SkillEvent* pkEvent = m_kEventPool.Alloc();
	if (NULL != pkEvent)
	{
		_resetEvent(pkEvent);
		pkEvent->SetStartTime(GET_PROCESS_TIME);
	}
	return pkEvent;
}
//------------------------------------------------------------
void SK_ClientSkillEventManager::_addEvent( SK_SkillEvent* pkEvent )
{
	m_kMainEventList.push_back(pkEvent);
}
//------------------------------------------------------------
SK_SkillEvent* SK_ClientSkillEventManager::_findBulletEvent( long lEventID, long lBulletLocalID, object_id_type lShooterID )
{
	SK_SkillEventListIter iter;
	for (iter = m_kMainEventList.begin(); iter != m_kMainEventList.end(); ++iter)
	{
		SK_SkillEvent * pkEvent = *iter;
		if (pkEvent->GetEventType() != SETE_BULLET)
			continue;

		if (pkEvent->GetSrcActorID() != lShooterID)
			continue;

		if (pkEvent->GetEventID() != lEventID)
			continue;

		long  lLocalID = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_BULLET_LOCAL_ID);
		if (lLocalID == lBulletLocalID)
			return pkEvent;
	}
	return NULL;
}
//------------------------------------------------------------
void SK_ClientSkillEventManager::_removeEvent( SK_SkillEvent* pkEvent )
{
	SK_SkillEventListIter iter;
	for (iter = m_kMainEventList.begin(); iter != m_kMainEventList.end(); ++iter)
	{
		if (*iter == pkEvent)
		{
			m_kMainEventList.erase(iter);
			return;
		}
	}
}
//------------------------------------------------------------
void SK_ClientSkillEventManager::_freeEvent( SK_SkillEvent* pkEvent )
{
	m_kEventPool.Free(pkEvent);
}
//------------------------------------------------------------