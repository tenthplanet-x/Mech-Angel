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

//技能连锁事件列表中每次处理的事件个数
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
	//正在处理事件
	m_nProcessingEvent = E_ProcessState_Chain;
	//处理一定数量的连锁事件
	_procEventList(fCurrentTime, m_kChainEventList, MAX_SERVER_CHAIN_SKILL_EVENT_PROCESS_COUNT);

	//将未处理的连锁事件转到主事件列表中
	SK_SkillEventListIter iterCur = m_kChainEventList.begin();
	SK_SkillEventListIter iterEnd = m_kChainEventList.end();
	while (iterCur != iterEnd)
	{
		m_kMainEventList.push_back(*iterCur++);		// AddEvent, m_nProcessingEvent == E_ProcessState_Chain
	}
	//注意避免事件重复
	m_kChainEventList.clear();

	m_nProcessingEvent = E_ProcessState_Main;
	_procEventList(fCurrentTime, m_kMainEventList, SERVER_MAIN_SKILL_EVENT_PROCESS_COUNT);
	m_nProcessingEvent = E_ProcessState_Done;

	// 清除客户端子弹事件请求
	//☆ 每帧调用_clearEventList可能有个问题，因为前面两个列表中，事件处理都有最大数量限制，极有可能某些子弹事件当前帧未被处理，然后相应的客户端请求时间却已经被清理，从而得不到正确处理
	//*
	this->_updateActiveBuddyBulletReqEvent(fCurrentTime);
	/*/
	this->_clearEventList(m_kActiveBuddyBulletReqEvents);
	//*/

	//< 更新对象池
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
	m_iAllocEventID = (m_iAllocEventID + 1) % nMaxSkillID;			//分配事件ID
	_resetEvent(pkEvent);
	pkEvent->SetEventID(m_iAllocEventID);
	pkEvent->SetStartTime(GET_PROCESS_TIME);						//设置事件开始时间

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
				//只能向player发送消息
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
			//只能向player发送消息
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
				//设置事件为销毁状态or事件超时，则销毁事件
				m_kEventPool.Free(pkEvent);
			}
			else if (fCurrentTime - pkEvent->GetStartTime() > SKILL_EVENT_UPDATE_DELTA_TIME)
			{
				GfxWriteFmtLog(LOG_SKILL_EVENT_FILELINE, LOG_SWITCH_SKILL_EVENT, "event type timeout!", pkEvent->GetEventID(), pkEvent->GetEventType(), pkEvent->GetSrcActorID(), pkEvent->GetDstActorID(), pkEvent->GetLogicID(), pkEvent->GetParam(0), pkEvent->GetParam(1));
				m_kEventPool.Free(pkEvent);
			}
			else
			{
				//事件未结束，等待下次处理
				AddEvent(pkEvent);
			}
		}
	}

	// 把已处理完的清理掉，如果 iterCur 是end，相当于clear
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

	//事件处理完后进行销毁
	bDestroyEvent = true;

	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ActorBase* pkSrcActor = (LC_ActorBase*)pkWorldManager->FindObject(lSrcActorID);
	LC_ActorBase* pkDstActor = (LC_ActorBase*)pkWorldManager->FindObject(lDstActorID);
	if (NULL == pkDstActor)
	{
		return;
	}

	//消耗物品，不需要进行后续处理
	if (SK_CostEvent::SCT_ITEM == lCostType)
	{
		return;
	}

	_notifyState(pkSrcActor, pkDstActor, pkEvent);

	//计算最终消耗量
	int64_t lCostBase = pkEvent->GetParam(SK_CostEvent::SEP_COST_BASE);
	int64_t lModifyValue = pkEvent->GetParam(SK_CostEvent::SEP_COST_MODIFY_VALUE);
	int64_t lModifyPercent = pkEvent->GetParam(SK_CostEvent::SEP_COST_MODIFY_PERCENT);
	double fModifyPercent = SK_CostEvent::ParamPerLL2Double(lModifyPercent);
	int64_t lFinalCost = Double2Int64(lCostBase * (1.0 + fModifyPercent)) + lModifyValue;
	pkEvent->SetParam(SK_CostEvent::SEP_FINAL_COST, lFinalCost);

	//设置回复量
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

	//事件处理完后进行销毁
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

	//化骨冥掌 debuff
	double valuePercent = pkDstActor->GetStateMutableParameter(31006045,0);

	//计算最终回复量
	int64_t lRecoverBase = pkEvent->GetParam(SK_RecoverEvent::SEP_RECOVER_BASE);
	int64_t lModifyValue = pkEvent->GetParam(SK_RecoverEvent::SEP_RECOVER_MODIFY_VALUE);
	int64_t lModifyPercent = pkEvent->GetParam(SK_RecoverEvent::SEP_RECOVER_MODIFY_PERCENT);
	double fModifyPercent = SK_RecoverEvent::ParamPerLL2Double(lModifyPercent);
	int64_t lFinalRecover = Double2Int64((lRecoverBase * (1.0 + fModifyPercent) + lModifyValue)*(1-valuePercent));

	pkEvent->SetParam(SK_RecoverEvent::SEP_FINAL_RECOVER, lFinalRecover);

	//设置回复量
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
	//对恢复值进行修正
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

			//异种侵袭和异种降临需要记录对怪造成的伤害
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

	//事件处理完后进行销毁
	bDestroyEvent = true;

	//判断伤害类型不正确，事件结束
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

	//判断玩家 是否有效
	if (IS_PLAYER_CLASS_ID(lDstActorID) /*&& pkDstActor*/)
	{
		LC_ServerPlayer* pkDstPlayer = (LC_ServerPlayer*)pkDstActor;
		IF_A_NA_PLAYER(pkDstPlayer){ return; }
	}

	//判断不同地图不造成伤害
	if (pkSrcActor && /*pkDstActor &&*/ pkSrcActor->GetMap()!=pkDstActor->GetMap() )
	{
		return;
	}

	//双方都是玩家，攻击方式PK模式为服务器模式，却攻击的本服玩家验证
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

	//伤害没有命中，事件结束
	if (SK_DamageEvent::SDR_MISS == lResult && !pkDstActor->GetSkillFlag(SK_ATTACK_FLAG))
	{
		_notifyState(pkSrcActor, pkDstActor, pkEvent);
		_notifyController(pkSrcActor, pkDstActor, pkEvent);
		SendToClient(pkSrcActor, pkEvent, SSET_SEND);
		//避免消息重复发送
		if (lSrcActorID != lDstActorID)
		{
			SendToClient(pkDstActor, pkEvent, SSET_SEND);
		}
		return;
	}

	pkDstActor->SetAttackerID(lSrcActorID);
	pkDstActor->SetAttackerSkillTypeID(lLogicID);

	/*
	//怪物的ownerlist，在怪物死亡的时候再维护
	if (IS_NPC_ID(lDstActorID))
	{
		//如果目前没有归属，就添加
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

		//上一个造成伤害的指挥官持续6秒没有对怪物造成伤害  更换首击者
		//上一个造成伤害的指挥官已经不在线  更换首击者
		if(NULL!=pkSrcOwnerPlayer && (HitPlayerID!=pkSrcOwnerPlayer->GetOwnerObjectID() && currentTime-HitTime>6) || NULL==pkPlayer)
		{
			object_id_type id = pkSrcOwnerPlayer->GetOwnerObjectID();
			((LC_ServerNPC*)pkDstActor)->SetHitPlayer(pkSrcOwnerPlayer->GetOwnerObjectID());
		}

		if(HitPlayerID == pkSrcOwnerPlayer->GetOwnerObjectID())
		{
			((LC_ServerNPC*)pkDstActor)->SetHitTime(currentTime);
		}
		//归属者上次攻击怪物的时间

		((LC_ServerNPC*)pkDstActor)->AddDamagePlayer(pkSrcOwnerPlayer->GetOwnerObjectID());
	}

	//目标处于无敌状态，事件结束
	if (pkDstActor->GetSkillFlag(SK_IMMORTAL))										//无敌
	{
		return;
	}

	if (pkSrcActor && IS_PLAYER_CLASS_ID(pkSrcActor->GetID()) && pkDstActor->GetSkillFlag(SK_IMMORTAL_FOR_PLAYER) )
	{
		return ;
	}

	if (SK_DamageEvent::SDR_JUMP_DODGE == lResult )									//玩家轻功时认为无敌
	{
		SendToClient(pkDstActor, pkEvent, SSET_SEND);
		return;
	}

	_notifyState(pkSrcActor, pkDstActor, pkEvent);

	/*
	*计算最终伤害
	*/
	int64_t lDamageBase = pkEvent->GetParam(SK_DamageEvent::SEP_DAMAGE_BASE);
	int64_t lModifyValue = pkEvent->GetParam(SK_DamageEvent::SEP_DAMAGE_MODIFY_VALUE);
	int64_t lNShield = pkEvent->GetParam(SK_DamageEvent::SEP_NEUTRALIZE_SHIELD);
	int64_t lModifyPercent = pkEvent->GetParam(SK_DamageEvent::SEP_DAMAGE_MODIFY_PERCENT);
	double fModifyPercent = SK_DamageEvent::ParamPerLL2Double(lModifyPercent);
	int64_t rrt = pkEvent->GetParam(SK_DamageEvent::SEP_RESULT);
	uint64_t lFinalDamage = 0;
	int64_t hpDamage = 0;
	//护盾逻辑
	if ( rrt == SK_DamageEvent::SDR_SHIELD)
	{
		lFinalDamage = Double2Int64(lDamageBase *  (1.0f + fModifyPercent)); 
		//最终伤害是存入护盾值
		pkEvent->SetParam(SK_DamageEvent::SEP_FINAL_DAMAGE, lNShield-lModifyValue);
	}
	else
	{
		lFinalDamage = Double2Int64(lDamageBase *  (1.0f + fModifyPercent)) + lModifyValue;
		//伤害必须为非负
		lFinalDamage = UT_MathBase::MaxInt64(0, lFinalDamage);

		//伤害血量上限
		double hpProtect = SK_DamageEvent::ParamPerLL2Double(pkEvent->GetParam(SK_DamageEvent::SEP_DAMAGE_HP_PROTECTED));
		if ( hpProtect > 0 )
		{
			double val = pkDstActor->GetFinalAttributeMap()->GetAttribute(ATT_MAX_HP) * hpProtect;
			lFinalDamage = UT_MathBase::MinInt64((int64_t)val, lFinalDamage);
		}

		int64_t preFinalDamage = lFinalDamage;

		//最终伤害减免
		double finDec = SK_DamageEvent::ParamPerLL2Double(pkEvent->GetParam(SK_DamageEvent::SEP_DAMAGE_FINAL_DMG_DE));
		if ( finDec > 0 )
		{
			finDec = UT_MathBase::MinDouble(finDec, 1.0f);
			lFinalDamage = UT_MathBase::MaxInt64(0,lFinalDamage - (int64_t)(lFinalDamage * finDec));
		}

		//最终伤害加成
		double finAd = SK_DamageEvent::ParamPerLL2Double(pkEvent->GetParam(SK_DamageEvent::SEP_DAMAGE_FINAL_DMG_AD));
		if ( finAd > 0 )
		{
			finAd = UT_MathBase::MaxDouble(finAd, 0);
			lFinalDamage = UT_MathBase::MaxInt64(0, lFinalDamage + (int64_t)(lFinalDamage * finAd));
		}

		//最终伤害回复
		double finRecover = SK_DamageEvent::ParamPerLL2Double(pkEvent->GetParam(SK_DamageEvent::SEP_DAMAGE_FINAL_RECOVER));
		if ( finRecover > 0)
		{
			SC_ServerSkillInterface* pkSkillInterface =  SC_ServerScriptInterface::GetSingleton().GetSkillInterface();
			if ( pkSkillInterface )
			{
				pkSkillInterface->RecoverTargetHP(pkDstActor, TARGET_ME, lLogicID, Double2Int64(lFinalDamage*finRecover),0, lDstActorID);
			}
		}

		//最终伤害回复（减免前）
		double finRecover2 = SK_DamageEvent::ParamPerLL2Double(pkEvent->GetParam(SK_DamageEvent::SEP_DAMAGE_FINAL_RECOVER_RE));
		if ( finRecover2 > 0)
		{
			SC_ServerSkillInterface* pkSkillInterface =  SC_ServerScriptInterface::GetSingleton().GetSkillInterface();
			if ( pkSkillInterface )
			{
				pkSkillInterface->RecoverTargetHP(pkDstActor, TARGET_ME, lLogicID, Double2Int64(preFinalDamage*finRecover2),0, lDstActorID);
			}
		}

		//最终伤害反弹
		int64_t rebDmg = 0;
		double finReb = SK_DamageEvent::ParamPerLL2Double(pkEvent->GetParam(SK_DamageEvent::SEP_DAMAGE_FINAL_REBOND));
		if ( finReb > 0 )
		{
			rebDmg = (int64_t)(UT_MathBase::MinInt64(pkDstActor->GetHP(), lFinalDamage) * finReb);
		}
		//最终伤害反弹减免前
		double finRebRe = SK_DamageEvent::ParamPerLL2Double(pkEvent->GetParam(SK_DamageEvent::SEP_DAMAGE_FINAL_REBOND_RE));
		if ( finRebRe > 0 )
		{
			rebDmg += (int64_t)(UT_MathBase::MinInt64(pkDstActor->GetHP(), preFinalDamage) * finRebRe);
		}

		//属性反弹
		int64_t redound =  (int64_t) pkDstActor->GetFinalAttributeMap()->GetAttribute(ATT_DMG_REBOUND);
		if ( (lDamageType == SDT_SKILL || lDamageType == SDT_NORMAL) && redound > 0 )
		{
			rebDmg +=  (int64_t)(UT_MathBase::MinInt64(pkDstActor->GetHP(), lFinalDamage) * (redound/10000.0));
		}

		//最终伤害反弹
		SC_ServerSkillInterface* pkSkillInterface =  SC_ServerScriptInterface::GetSingleton().GetSkillInterface();
		if ( pkSkillInterface && rebDmg > 0)
		{
			if ( pkSrcActor )
				pkSkillInterface->GenerateDamage(pkDstActor, pkSrcActor->GetID(), 0, SDT_REBOUND, rebDmg, pkDstActor->GetID());
		}

		//最终伤害连击
		double finCombo = SK_DamageEvent::ParamPerLL2Double(pkEvent->GetParam(SK_DamageEvent::SEP_DAMAGE_FINAL_COMBO));
		if ( finCombo > 0 )
		{
			SC_ServerSkillInterface* pkSkillInterface =  SC_ServerScriptInterface::GetSingleton().GetSkillInterface();
			if ( pkSkillInterface && pkSrcActor)
			{
				pkSkillInterface->GenerateDamage(pkSrcActor,pkDstActor->GetID(), 0, SDT_SKILL, (int64_t)(lFinalDamage*finCombo),pkSrcActor->GetID());
			}
		}

		//HP血量护盾
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

		//boss护盾强制出盾
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

		//伤害不能超过怪物剩余血量过多
		pkEvent->SetParam(SK_DamageEvent::SEP_FINAL_DAMAGE, lFinalDamage);
	}
	/*
	*向客户端发送消息
	*注意：在设置HP前就应该发送伤害消息，因为SetHP中产生的死亡消息是即时消息，会先于伤害消息到达客户端
	*/
	SendToClient(pkSrcActor, pkEvent, SSET_SEND);
	//避免消息重复发送
	if (lSrcActorID != lDstActorID)
	{
		SendToClient(pkDstActor, pkEvent, SSET_SEND);
	}
	
	//如果现在是在异种侵袭
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
	*设置HP
	*/
	attr_value_type iCurrentHP = pkDstActor->GetHP() - hpDamage;

	//buddy打怪的伤害统计都计入OwnerPlayer
	if(IS_PLAYER_CLASS_ID(lSrcActorID) && NULL!=pkSrcActor)
	{
		LC_ServerPlayer* pkOwnerPlayer = ((LC_ServerPlayer* )pkSrcActor)->GetOwnerPlayer();
		if(NULL == pkOwnerPlayer)
		{
			object_id_type nOwnerId =  ((LC_ServerPlayer* )pkSrcActor)->GetOwnerPlayerId();
			// log
			return;
		}

		// 副本战斗统计入口  魏欣
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
	
	//活动管理器需要处理玩家杀怪
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

	//设置PK值
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
			//攻击和被攻击提示
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
	*技能打断，打坐、回城、骑乘技能不被DOT伤害打断
	*/
	int iStateType = pkDstActor->GetCurrentStateType();
	if (IS_SKILL_TYPE(iStateType) && lFinalDamage > 0)
	{
		//判断当前技能是否可以被dot打断
		bool bCanBeInterruptByDot = true;
		//打坐、回城、采集不可以被dot打断
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

	//事件处理完后进行销毁
	bDestroyEvent = true;

	//判断伤害类型不正确，事件结束
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

	//事件处理完后进行销毁
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


	//事件处理完后进行销毁
	bDestroyEvent = true;

	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ActorBase* pkSrcActor = (LC_ActorBase*)pkWorldManager->FindObject(lSrcActorID);
	LC_ActorBase* pkDstActor = (LC_ActorBase*)pkWorldManager->FindObject(lDstActorID);

	if (NULL == pkDstActor)
	{
		return;
	}


	int lCurrentStateType = pkDstActor->GetCurrentStateType();
	//只打断事件中指定的技能
	if (lCurrentStateType == lInterruptSkillID && IS_SKILL_TYPE(lCurrentStateType))
	{
		int iSkillStage = pkDstActor->GetSkillStage();
		SK_SkillExecute* pkSkillExecute = (SK_SkillExecute*)pkDstActor->GetCurrentState();
		if (NULL != pkSkillExecute)
		{
			bool bCanInterrupt = false;

			//根据打断类型判断是否可以被打断
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
//#pragma TODO("目前使用InterruptSkill来处理被打断消息，是否需要改成用BeInterruptCurrentSkill？")
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

	//事件处理完后进行销毁
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

	//事件处理完后进行销毁
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

	//默认销毁事件
	bDestroyEvent = true;

	// 客户端负责更新的子弹
	if (bullet_update_by_client(pkEvent))
	{
		// 超时，客户端请求的子弹，服务器多等待 N 秒
		float fStartTime = SK_BulletEvent::ParamLLong2Float(pkEvent->GetParam(SK_BulletEvent::SEP_MOVE_START_TIME));
		float fMoveTime = SK_BulletEvent::ParamLLong2Float(pkEvent->GetParam(SK_BulletEvent::SEP_MOVE_LIFE_TIME));
		if (fCurrentTime - fStartTime > fMoveTime + 3.0f)
			return;

		// 响应客户端请求
		_procActiveBuddyBulletReqEvent(fCurrentTime, pkEvent, bDestroyEvent);
		return;
	}

	// 服务器负责更新的子弹
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

	//销毁事件
	bDestroyEvent = true;

	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ActorBase* pkSrcActor = (LC_ActorBase*)pkWorldManager->FindObject(lSrcActorID);
	
	if(NULL != pkSrcActor)
	{
		//技能释放者是buddy招出来的怪物，则释放者是buddy
		if(IS_NPC_ID(lSrcActorID) && pkSrcActor->GetSummonOwnerID()!=0)
		{			
			lSrcActorID = pkSrcActor->GetSummonOwnerID();
			pkSrcActor = (LC_ActorBase*)pkWorldManager->FindObject(lSrcActorID);
		}
	}

	LC_ActorBase* pkDstActor = (LC_ActorBase*)pkWorldManager->FindObject(lDstActorID);
	if (NULL == pkDstActor)
	{
		GfxWriteLog(LOG_SKILL_EVENT_FILELINE, LOG_SWITCH_SKILL_EVENT, "killevent[%i]error，Recipient not found[%i]", pkEvent->GetEventID(), lDstActorID);
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

	//玩家杀怪orNPC杀怪，经验奖励和掉落宝箱
	if (IS_NPC_ID(lDstActorID) /*&& NULL != pkDstActor*/)
	{
		pkNPCDest = (LC_ServerNPC*)pkDstActor;

		//先取首击者作为NPC的owner，若npc无首击者或者首击者不在线，则取伤害最多的玩家为NPC的owner
		object_id_type HitPlayerID = pkNPCDest->ConvertHitPlayerToOwnerList();
		LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(pkServerPlayerManager->FindPlayer(HitPlayerID));
		if(NULL == pkPlayer)
		{
			pkNPCDest->ConvertEncountToOwnerList();
		}

		//经验奖励和宝箱掉落改为改为由怪物所处的MapLogic统一处理
		LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
		{
			pkMapLogic = pkWorldManager->GetMapLogic(pkNPCDest->GetMapLogicID());
			if (pkMapLogic && !pkMapLogic->GetAutoChangeMonsterHp())
			{
				pkNPCDest->GetNPCOwnerList().ResolveTotalLevel(pkNPCDest->GetMap());
			}
		}
		
		//buddy打死的怪，按照指挥官打死的，通知GLET_KILL事件
		if (NULL!=pkSrcActor && IS_PLAYER_CLASS_ID(lSrcActorID))
		{
			lSrcActorID = pkSrcActor->GetOwnerObjectID();
		}

		//发送杀怪事件
		LC_GameEvent* pkGameEvent = pkGameEventManager->AllocGameEvent();
		if (NULL != pkGameEvent)
		{
			//注意，虽然此处杀怪类型为GLET_PLAYER_KILL_NPC，但也有可能是NPC杀怪
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
			//一下两个值在副本中会用到，请不要删掉
			pkGameEvent->SetEventParam(EVENT_EXTEND_PARAM_6,	(int)pkNPCDest->GetProperty_ExpRadio());
			pkGameEvent->SetEventParam(EVENT_EXTEND_PARAM_7,	pkNPCDest->GetLevel());
			pkGameEventManager->InsertGameEvent(pkGameEvent);
		}

		//怪物存在组队行为
		LC_NPCGroupBase* pkNPCGroup = pkNPCDest->GetNPCGroup();
		if (NULL != pkNPCGroup)
		{
			//死亡队友数量+1
			pkNPCGroup->SetDeathCount(pkNPCGroup->GetDeathCount() + 1);
			//设置出生时间
			LC_NPCGroupSpawnPoint* pkSpawnPoint = pkNPCGroup->GetSpawnPointByMemberID(pkNPCDest->GetGroupMemberID());
			if (NULL != pkSpawnPoint)
			{
				pkSpawnPoint->SetActiveTime(fCurrentTime);
			}
			//删除死亡队友
			pkNPCGroup->RemoveByNPCID(pkNPCDest->GetID());
		}
	}
	else if (IS_PLAYER_CLASS_ID(lDstActorID))		//玩家被杀死，死亡惩罚掉落宝箱
	{
		//经验奖励和宝箱掉落改为改为由怪物所处的MapLogic统一处理
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
		

		//用于死亡惩罚，判断伤害来源
		pkDstActor->SetAttackerID(lSrcActorID);

		//玩家杀死玩家
		if (IS_PLAYER_CLASS_ID(lSrcActorID))
		{
			//抛出杀人事件
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
		//玩家杀死npc，判断首杀
		if(IS_PLAYER_CLASS_ID(lSrcActorID))
		{
			object_id_type pkFirstHitPlayerID = pkNPCDest->GetHitPlayerID();
			pkFirstHitPlayer = (LC_ServerPlayer*)pkWorldManager->FindObject(pkFirstHitPlayerID);
			if(NULL == pkFirstHitPlayer && pkSrcActor)
			{
				//找不到首杀玩家，则奖励属于最后一击者
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
					//检测掉落次数
					//pkFirstHitPlayer有掉落次数时，才掉落
					int times = pkFirstHitPlayer->GetCurrentRecoverTimes(pkNpcData->_bReduceDropType);
					//2021.2.23 需求修改：首领禁地和首领要塞掉落次数为0提示：还剩1点时，击杀boss扣除后为0也需弹出提示
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

				//首杀处理
				static Activity::ACT_ScheduleManager* pkScheduleMgr = Activity::ACT_ScheduleManager::GetSingletonPtr();
				Activity::ACT_ScheduleEntry* pkSchedule = pkScheduleMgr->GetSchedule(pkFirstHitPlayer->GetServerID(), GetGlobalSetting.BossFirstKillScheduleID);
				if(NULL != pkSchedule && pkScheduleMgr->IsScheduleValid(pkFirstHitPlayer->GetServerID(), GetGlobalSetting.BossFirstKillScheduleID))
				{
					int npcID = pkNPCDest->GetCharType();
					CF_BossFirstKill::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_BossFirstKill>(npcID);
					if(NULL != pkData)
					{
						pkFirstHitPlayer->AddKillBossRecord(npcID);

						//查找击杀者
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
								//未击杀过
								info.killerName =TPSTR2STDSTR(pkFirstHitPlayer->GetOwnerCharName());
								info.killerLevel = pkFirstHitPlayer->GetCommanderLevel();
								info.killerbornCharType = pkFirstHitPlayer->GetProfilePictureID();
								info.killerCommanderTitleID = pkFirstHitPlayer->GetCommanderTitle();
								info.KillerCommanderTitleFrameID = pkFirstHitPlayer->GetCurrentCommanderTitleFrameID();

								std::stringstream ss;
								boost::ajson::save_to_buff(info, ss);
								std::string data = ss.str().c_str();

								UT_ServerHelper::RedisSetData_String(UT_REDIS_KEY_BOSS_FIRST_KILL, input, data);

								//boss首杀，通知所有结点
								MG_SS_NoticeBossFirstKillInfo syncMsg;
								syncMsg.m_nBossID = npcID;
								syncMsg.m_nKillerInfo.m_nPlayerName = info.killerName;
								syncMsg.m_nKillerInfo.m_nBornCharType = info.killerbornCharType;
								syncMsg.m_nKillerInfo.m_nCommanderTitleID = info.killerCommanderTitleID;
								syncMsg.m_nKillerInfo.m_nPlayerLevel = info.killerLevel;
								syncMsg.m_nKillerInfo.m_nCommanderTitleFrameID = info.KillerCommanderTitleFrameID;
								SendMsgToLogin(MGPT_SS_NOTICEBOSSFIRSTKILLINFO_LOGIN, &syncMsg);

								//发送全服公告
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

									//加入背包
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

	//可能会复活玩家  所以等所有同步完成后再 处理活动逻辑
	//活动管理器需要处理玩家杀怪
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

	//事件处理完后进行销毁
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

	//事件处理完后进行销毁
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

	//事件处理完后进行销毁
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

	//事件处理完后进行销毁
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
		//集体回撤
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

	//事件处理完后进行销毁
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

	//事件处理完后进行销毁
	bDestroyEvent = true;

	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ActorBase* pkDstActor = (LC_ActorBase*)pkWorldManager->FindObject(lDstActorID);

	if (NULL == pkDstActor && lTargetLocationX <= 0 && lTargetLocationY <= 0)
	{
		return;
	}

	//这个事件只能单发
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

	//事件处理完后进行销毁
	bDestroyEvent = true;

	//srcID表示进入地图的玩家ID
	LC_ActorBase* pkSrcActor = (LC_ActorBase*)pkWorldManager->FindObject(lSrcActorID);
	if (NULL == pkSrcActor)
	{
		return;
	}
	//找出进入玩家地图中的所有怪物，通知state
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
			//必须是技能事件才是state才能处理，所以这边写入特殊的值
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

	//事件处理完后进行销毁
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

	//事件处理完后进行销毁
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

	//事件处理完后进行销毁
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
		//取释放暗器的技能
		if (NULL != pkSkillExecute)
		{
			pkSkillExecute->DelayUpdate(pkSrcActor);
		}
		pkEvent->SetParam(SK_SummorDamage::SEP_TICK_COUNT, i);

		//防止死循环
		if ( i > 10)
			break;
	}

	if ( duration + startTime < fCurrentTime )
	{
		//事件处理完后进行销毁
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
		_procBulletToTarget，子弹飞向目标，过程中不与其它任何物体做穿刺判定，子弹飞行时间结束后处理穿刺或者爆炸事件
		如果是穿刺，记录当前目标，并在周围只取一个目标，继续飞行
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

	//销毁事件
	bDestroyEvent = true;
	SK_Factory* pkSkillFactory = SK_Factory::GetSingletonPtr();
	SK_SkillExecute* pkSkillExecute = pkSkillFactory->RequestSkillExcute(lSkillID);
	if (NULL == pkSkillExecute)
		return;
	/// 子弹参数
	const SK_SkillExecute::BulletParam & kBP = pkSkillExecute->GetProperty_BulletParam(lBulletFuncID);

	/*
	*子弹本次飞行结束,
	*1、如果还需要处理其它目标，且该目标可以找到，则子弹穿透，
	*2、如果不需处理其它目标，or下一个目标找不到，则子弹爆炸
	*/
	if (fCurrentTime - fStartTime > fMoveTime)
	{
		//查找子弹释放人
		LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
		LC_ActorBase* pkSrcActor = (LC_ActorBase*)pkWorldManager->FindObject(lSrcActorID);
		if (NULL == pkSrcActor)
			return;

		//如果目标已经死亡 就在上一次记录的地点爆炸
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

		//需要穿透新的目标
		if (lPenetrateCount < lMaxPenetrateCount && NULL != pkCurrentTarget)
		{
			//记录打击过的目标，_bulletLockTarget会用于排斥锁定目标
			if (lPenetrateCount < 0 || lPenetrateCount >= SK_BulletEvent::MAX_BULLET_PENETRATE_COUNT)
				return;
			pkEvent->SetParam(SK_BulletEvent::SEP_PENETRATE_COUNT, lPenetrateCount + 1);
			pkEvent->SetParam(SK_BulletEvent::SEP_TARGET_BEGIN + lPenetrateCount, lDstActorID);
			//任何超越SEP_TARGET_END的穿透目标设定都是非法的，会导致越界覆盖后续参数SEP_SHOT_LOCATION_X和SEP_SHOT_LOCATION_Y，超出MAX_SKILL_EVENT_PARAM_COUNT将被忽略

			int lActorFilterFuncID = pkSkillExecute->GetProperty_LockTargetType();
			int lBulletLockRange = kBP.m_lLockRange;
			int lBulletMaxLockCount = 1;	//只搜索一个目标
			bool bShotBack = kBP.m_bShotBack;

			//在当前目标周围锁定新的目标，只取第一个
			object_id_vector kTargetVector = _bulletLockTarget(pkSrcActor, (SK_BulletEvent*)pkEvent, kTargetLocation, lBulletLockRange, lActorFilterFuncID, lBulletMaxLockCount, bShotBack);
			int lNextTargetID = !kTargetVector.empty() ? kTargetVector.front() : 0;
			LC_ActorBase* pkNextTarget = lNextTargetID != 0 ? (LC_ActorBase*)pkWorldManager->FindObject(lNextTargetID) : NULL;

			if (NULL != pkNextTarget)
			{
				//计算子弹打击下一个目标飞行的距离
				UT_Vec3Int kNextTargetLocation;
				int nRetCode = pkNextTarget->GetCurrentLocationWithZ(kNextTargetLocation);
				int iDistance = UT_MathBase::LineLengthXYInt(kTargetLocation, kNextTargetLocation);
				float fNextMoveTime = Long2Float(iDistance - pkCurrentTarget->GetProperty_ShapeRadius() - pkNextTarget->GetProperty_ShapeRadius()) / Long2Float(UT_MathBase::MaxInt(1, lMoveSpeed));

				pkEvent->SetDstActorID(lNextTargetID);
				pkEvent->SetParam(SK_BulletEvent::SEP_MOVE_START_TIME, SK_BulletEvent::ParamFloat2LLong(fCurrentTime));
				pkEvent->SetParam(SK_BulletEvent::SEP_MOVE_LIFE_TIME, SK_BulletEvent::ParamFloat2LLong(fNextMoveTime));

				//向被穿刺目标广播穿刺消息
				_broadcastBulletPenetrate(pkCurrentTarget, (SK_BulletEvent*)pkEvent, kTargetLocation);


				//记录子弹位置
				pkSrcActor->SetBulletLocation(kTargetLocation);
				//穿透操作，伤害旧目标
				pkSkillExecute->BulletPenetrate(lBulletFuncID, pkSrcActor, lDstActorID, lPenetrateCount, _BULLET_UUID(lEventID, lBulletLocalID));

				//不销毁事件
				bDestroyEvent = false;
				return;
			}
		}

		//向被爆炸广播爆炸消息
		_broadcastBulletExplosive(pkCurrentTarget, (SK_BulletEvent*)pkEvent, kTargetLocation, lDstActorID, SK_BulletEvent::SBER_Timeout);

		//记录子弹位置
		pkSrcActor->SetBulletLocation(kTargetLocation);
		//爆炸操作
		pkSkillExecute->BulletExplosive(lBulletFuncID, pkSrcActor, lDstActorID, lPenetrateCount, _BULLET_UUID(lEventID, lBulletLocalID));
	}
	else
	{
		//更新目标型子弹的最终爆炸位置
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

			//判断是否进行直线穿刺
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

				// 插值计算子弹当前位置
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
				
				//获取过滤目标方式
				int lActorFilterFuncID = pkSkillExecute->GetProperty_LockTargetType();
				bool bShotBack = kBP.m_bShotBack;

				// 在当前位置周围锁定目标
				object_id_vector kPenetrateTargetVector = _bulletLockTarget(pkSrcActor, (SK_BulletEvent*)pkEvent, kBulletLocation, lBulletShapeRadius, lActorFilterFuncID, lBulletMaxLockCount, bShotBack);

				// 开始子弹当前位置穿刺
				object_id_vector_iter kPenetrateIter;
				TRAV(kPenetrateTargetVector, kPenetrateIter)
				{
					object_id_type lTargetID = *kPenetrateIter;
					LC_ActorBase* pCurrentTarget = (LC_ActorBase*)pkWorldManager->FindObject(lTargetID);
					if (NULL == pCurrentTarget)
					{
						continue;
					}

					//记录子弹位置
					pkSrcActor->SetBulletLocation(kBulletLocation);

					//子弹穿透
					//当前位置穿刺只让脚本产生伤害，不改变子弹轨迹
					pkSkillExecute->BulletPenetrate(lBulletFuncID, pkSrcActor, lTargetID, lPenetrateCount, _BULLET_UUID(lEventID, lBulletLocalID));	

					//记录打击过的目标
					pkEvent->SetParam(SK_BulletEvent::SEP_TARGET_0, lTargetID);

					//客户端收到穿刺消息会调用pkSkillExecute->BulletPenetrate()和BulletManager->BulletPenetrate()
					//实际上穿刺和改变轨迹应该是两种事件，客户端可能使用穿刺来改变轨迹了
					_broadcastBulletPenetrate(pCurrentTarget, (SK_BulletEvent*)pkEvent, kBulletLocation);
				}
			}
		}
		//不销毁事件
		bDestroyEvent = false;
	}
}
//------------------------------------------------------------
void SK_ServerSkillEventManager::_procBulletToLocation(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent)
{
	/*
		_procBulletToLocation，子弹飞向目标点，如果bProcHalfway为true，过程中对其它任何物体做穿刺判定，子弹飞行事件结束后爆炸
		如果是穿刺，则在周围锁定一堆目标（目标数量由SkillBulletFunc.csv表中配置），逐个穿刺，超出最大穿刺数则爆炸
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

	//销毁事件
	bDestroyEvent = true;

	//查找子弹释放人
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ActorBase* pkSrcActor = (LC_ActorBase*)pkWorldManager->FindObject(lSrcActorID);
	if (NULL == pkSrcActor)
		return;

	SK_Factory* pkSkillFactory = SK_Factory::GetSingletonPtr();
	SK_SkillExecute* pkSkillExecute = pkSkillFactory->RequestSkillExcute(lSkillID);
	if (NULL == pkSkillExecute)
		return;

	//获取过滤目标方式
	int lActorFilterFuncID = pkSkillExecute->GetProperty_LockTargetType();
	//是否处理中间过程
	const SK_SkillExecute::BulletParam & kBP = pkSkillExecute->GetProperty_BulletParam(lBulletFuncID);
	bool bProcHalfway = kBP.m_bPuncture;
	float fTerminalHoverInterval = kBP.m_fTerminalHoverInterval;

	//已经飞行到目标点
	if (fCurrentTime - fStartTime > fMoveTime)
	{
		UT_Vec3Int kTargetLocation;
		kTargetLocation.x = (int)pkEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_X);
		kTargetLocation.y = (int)pkEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Y);
		kTargetLocation.z = (int)pkEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Z);

		//子弹爆炸
		_broadcastBulletExplosive(pkSrcActor, (SK_BulletEvent*)pkEvent, kTargetLocation, 0, SK_BulletEvent::SBER_Timeout);

		//记录子弹位置
		pkSrcActor->SetBulletLocation(kTargetLocation);
		pkSkillExecute->BulletExplosive(lBulletFuncID, pkSrcActor, 0, lPenetrateCount, _BULLET_UUID(lEventID, lBulletLocalID));
	}
	else if (bProcHalfway)	//需要处理中间过程
	{
		bool bPeneStraight = kBP.m_bPeneStraight;
		//获取子弹属性
		int lMaxLockCount = kBP.m_lMaxLockCount;
		if (lMaxLockCount <= 0)
			return;

		int lBulletShapeRadius = kBP.m_lShapeRadius;
		if (lBulletShapeRadius <= 0)
			return;


		// 子弹发射位置和终点位置
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

		// 根据子弹类型 插值计算子弹当前位置
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

		// 在当前位置周围锁定目标
		object_id_vector kPenetrateTargetVector = _bulletLockTarget(pkSrcActor, (SK_BulletEvent*)pkEvent, kBulletLocation, lBulletShapeRadius, lActorFilterFuncID, lBulletMaxLockCount, bShotBack, bRepeatStrike);
		
		// 开始子弹当前位置穿刺
		object_id_vector_iter kPenetrateIter;
		TRAV(kPenetrateTargetVector, kPenetrateIter)
		{
			object_id_type lTargetID = *kPenetrateIter;
			LC_ActorBase* pkCurrentTarget = (LC_ActorBase*)pkWorldManager->FindObject(lTargetID);
			if (pkCurrentTarget == NULL)
			{
				continue;
			}
			//穿透目标数量不足，逐个穿刺
			if (lPenetrateCount < lMaxPenetrateCount)
			{
				//记录子弹位置
				pkSrcActor->SetBulletLocation(kBulletLocation);
				//子弹穿透
				pkSkillExecute->BulletPenetrate(lBulletFuncID, pkSrcActor, lTargetID, lPenetrateCount, _BULLET_UUID(lEventID, lBulletLocalID));	//当前位置穿刺只让脚本产生伤害，不改变子弹轨迹

				//记录打击过的目标

				if (lPenetrateCount<0)
				{
					return;
				}
				lPenetrateCount ++;
				pkEvent->SetParam(SK_BulletEvent::SEP_PENETRATE_COUNT, lPenetrateCount);
				pkEvent->SetParam(SK_BulletEvent::SEP_TARGET_BEGIN + lPenetrateCount - 1, lTargetID);

				//客户端收到穿刺消息会调用pkSkillExecute->BulletPenetrate()和BulletManager->BulletPenetrate()
				//实际上穿刺和改变轨迹应该是两种事件，客户端可能使用穿刺来改变轨迹了
				_broadcastBulletPenetrate(pkCurrentTarget, (SK_BulletEvent*)pkEvent, kBulletLocation);
			}
			else if (bPeneStraight)
			{
				//记录子弹位置
				pkSrcActor->SetBulletLocation(kBulletLocation);

				//子弹穿透
				pkSkillExecute->BulletPenetrate(lBulletFuncID, pkSrcActor, lTargetID, lPenetrateCount, _BULLET_UUID(lEventID, lBulletLocalID));	//当前位置穿刺只让脚本产生伤害，不改变子弹轨迹

				lPenetrateCount ++;
				//记录打击过的目标
				pkEvent->SetParam(SK_BulletEvent::SEP_TARGET_0, lTargetID);

				//客户端收到穿刺消息会调用pkSkillExecute->BulletPenetrate()和BulletManager->BulletPenetrate()
				//实际上穿刺和改变轨迹应该是两种事件，客户端可能使用穿刺来改变轨迹了
				_broadcastBulletPenetrate(pkCurrentTarget, (SK_BulletEvent*)pkEvent, kBulletLocation);
			}
			else	//穿透目标数量足够，爆炸
			{
				_broadcastBulletExplosive(pkSrcActor, (SK_BulletEvent*)pkEvent, kBulletLocation, lTargetID, SK_BulletEvent::SBER_PernerateFull);

				//记录子弹位置
				pkSrcActor->SetBulletLocation(kBulletLocation);
				pkSkillExecute->BulletExplosive(lBulletFuncID, pkSrcActor, lTargetID, lPenetrateCount, _BULLET_UUID(lEventID, lBulletLocalID));

				//子弹已经爆炸，不处理剩余的目标
				return;
			}
		}

		do	//开始周围穿刺
		{
			if (kPenetrateTargetVector.empty())	//打到目标了才能找周围穿刺
				break;
			
			if (bPeneStraight)
				break;

			//开始周围穿透
			int lBulletLockRange = kBP.m_lLockRange;
			if (lBulletLockRange == 0)	//不想周围穿刺请把lBulletLockRange设置为0
				break;

			//查找第一个穿刺的目标
			object_id_type lTargetID = kPenetrateTargetVector.front();
			LC_ActorBase* pkCurrentTarget = (LC_ActorBase*)pkWorldManager->FindObject(lTargetID);
			if (pkCurrentTarget == NULL)
				break;

			//只搜索一个目标
			int lBulletMaxLockCount = 1;

			object_id_vector kTargetVector = _bulletLockTarget(pkSrcActor, (SK_BulletEvent*)pkEvent, kBulletLocation, lBulletLockRange, lActorFilterFuncID, lBulletMaxLockCount, bShotBack, bRepeatStrike);
			int lNextTargetID = !kTargetVector.empty() ? kTargetVector.front() : 0;
			LC_ActorBase* pkNextTarget = lNextTargetID != 0 ? (LC_ActorBase*)pkWorldManager->FindObject(lNextTargetID) : NULL;
			if (pkNextTarget == NULL)
				break;

			//计算子弹打击下一个目标飞行的距离，这个目标一定是不会在lBulletShapeRadius范围内的，因为如果在，前面列表已经处理过了，或者子弹已经穿刺过多爆炸了
			int iBulletFuncID = (int)pkEvent->GetParam(SK_BulletEvent::SEP_BULLET_FUNC);
			int lMaxMoveDistance = kBP.m_lMaxMoveDistance;
			//const UT_Vec3Int & kNextTargetLocation = pkNextTarget->GetCurrentLocationWithZ();	//pkNextTarget只是用来确定下一个折射方向
			UT_Vec3Int kNextTargetLocation;
			int nRetCode = pkNextTarget->GetCurrentLocationWithZ(kNextTargetLocation);	//pkNextTarget只是用来确定下一个折射方向
			UT_Vec3Int kTargetLocation = UT_MathBase::MoveAlongIntXY(kBulletLocation, kNextTargetLocation, Long2Float(lMaxMoveDistance));
			float fNextMoveTime = Long2Float(lMaxMoveDistance) / Long2Float(lMoveSpeed);

			pkEvent->SetParam(SK_BulletEvent::SEP_MOVE_START_TIME, SK_BulletEvent::ParamFloat2LLong(fCurrentTime));
			pkEvent->SetParam(SK_BulletEvent::SEP_MOVE_LIFE_TIME, SK_BulletEvent::ParamFloat2LLong(fNextMoveTime));
			pkEvent->SetParam(SK_BulletEvent::SEP_COLLIDE_PARAM, Skill::SerializeToQuatWord(SK_BulletEvent::CollideParam(1.0f, 0)));	// 修改目标时未做碰撞，重置碰撞参数

			//向被穿刺目标广播改变轨迹消息
			_broadcastBulletChangeTargetLocation(pkCurrentTarget, (SK_BulletEvent*)pkEvent, kBulletLocation, kTargetLocation);
		}while (0);

		//不销毁事件，等待下次处理
		bDestroyEvent = false;
	}
	else
	{
		//不需要销毁事件
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
TODO: 安全性检查：
	子弹穿刺两次同一个目标
	子弹爆炸两次
*/
void Skill::SK_ServerSkillEventManager::_procActiveBuddyBulletReqEvent(float fCurrentTime, SK_SkillEvent * pkEvent, bool& bDestroyEvent)
{
	// 默认不删除
	bDestroyEvent = false;

	object_id_type lSrcActorID = pkEvent->GetSrcActorID();
	int lSkillID = pkEvent->GetLogicID();
	int lBulletFuncID = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_BULLET_FUNC);
	long lEventID = pkEvent->GetEventID();
	long lBulletLocalID = pkEvent->GetParam(SK_BulletEvent::SEP_BULLET_LOCAL_ID);


	// 目标型or方向型
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

	// TODO: 此处需要优化
	//查找子弹释放人
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

	// 处理该事件所对应的请求
	SK_SkillEventList& rkReqList = m_kActiveBuddyBulletReqEvents;

	// event参数
	object_id_type o1 = pkEvent->GetSrcActorID();
	object_id_type s1 = pkEvent->GetLogicID();
	long  lLocalID1 = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_BULLET_LOCAL_ID);
	object_id_type d1 = pkEvent->GetDstActorID();

	// 迭代过程中容器会变，不能把 rkReqList.end() 抽取出来进行表达式优化，会导致迭代器异常
	for (SK_SkillEventListIter iter = rkReqList.begin(); iter != rkReqList.end(); )
	{
		SK_SkillEvent* pkReq = *iter;

		// 在 rkReqList 中查找与 pkEvent 相关的请求进行处理，可能不止1个
		object_id_type o2 = pkReq->GetSrcActorID();
		if (o1 != o2) {
			++iter;			// pkReq 与 pkEvent 不符，继续下一个
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

		// 响应请求
		int lActionType = (int32_t)pkReq->GetParam(SK_BulletEvent::SEP_ACTION_TYPE);
		switch(lActionType & SK_BulletEvent::SBAT_Mask)
		{
		case SK_BulletEvent::SBAT_PENETRATE:
			{
				// 穿刺目标
				object_id_type lDstActorID = pkReq->GetDstActorID();
				if (lDstActorID == 0)
					break;

				// 已经穿刺过
				if (!bShotBack && is_target_penetrated(pkEvent, lDstActorID))
					break;

				//查找当前目标
				LC_ActorBase* pkCurrentTarget = (LC_ActorBase*)pkWorldManager->FindObject(lDstActorID);
				if (NULL == pkCurrentTarget)
					break;

				int lMaxPenetrateCount = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_MAX_PENETRATE_COUNT);
				int lPenetrateCount =(int32_t) pkEvent->GetParam(SK_BulletEvent::SEP_PENETRATE_COUNT);
				//const UT_Vec3Int & kTargetLocation2 = pkCurrentTarget->GetCurrentLocationWithZ();

				// TODO: 当前目标是否可攻击；目标位置是否合理
				UT_Vec3Int kTargetLocation;
				kTargetLocation.x = (int32_t)pkReq->GetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_X);
				kTargetLocation.y = (int32_t)pkReq->GetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_Y);
				kTargetLocation.z = (int32_t)pkReq->GetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_Z);

				//需要穿透新的目标
				if (lPenetrateCount < lMaxPenetrateCount)
				{
					//记录打击过的目标，_bulletLockTarget会用于排斥锁定目标
					if (lPenetrateCount < 0 || lPenetrateCount >= SK_BulletEvent::MAX_BULLET_PENETRATE_COUNT)
						break;
					pkEvent->SetParam(SK_BulletEvent::SEP_PENETRATE_COUNT, lPenetrateCount + 1);
					pkEvent->SetParam(SK_BulletEvent::SEP_TARGET_BEGIN + lPenetrateCount, lDstActorID);

					//向被穿刺目标广播穿刺消息
					_broadcastBulletPenetrate(pkCurrentTarget, (SK_BulletEvent*)pkEvent, kTargetLocation);

					//记录子弹位置
					pkSrcActor->SetBulletLocation(kTargetLocation);
					//穿透操作，伤害旧目标
					int lBulletFuncID = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_BULLET_FUNC);
					pkSkillExecute->BulletPenetrate(lBulletFuncID, pkSrcActor, lDstActorID, lPenetrateCount, _BULLET_UUID(lEventID, lBulletLocalID));
				}
				else if (bPeneStraight)
				{
					if (lPenetrateCount < 0 || lPenetrateCount >= SK_BulletEvent::MAX_BULLET_PENETRATE_COUNT)
						break;
					pkEvent->SetParam(SK_BulletEvent::SEP_PENETRATE_COUNT, lPenetrateCount);
					pkEvent->SetParam(SK_BulletEvent::SEP_TARGET_0, lDstActorID);

					//向被穿刺目标广播穿刺消息
					_broadcastBulletPenetrate(pkCurrentTarget, (SK_BulletEvent*)pkEvent, kTargetLocation);

					//记录子弹位置
					pkSrcActor->SetBulletLocation(kTargetLocation);
					//穿透操作，伤害旧目标
					int lBulletFuncID = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_BULLET_FUNC);
					pkSkillExecute->BulletPenetrate(lBulletFuncID, pkSrcActor, lDstActorID, lPenetrateCount, _BULLET_UUID(lEventID, lBulletLocalID));
				}

				// 附带转移目标
				object_id_type lOldTargetID  = lDstActorID;
				object_id_type lNextTargetID = (object_id_type)pkReq->GetParam(SK_BulletEvent::SEP_TARGET_0);
				if (bProcToTarget && lNextTargetID != 0)	// 目标型子弹的穿刺，才处理附带转移目标
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
				// 已爆炸，事件可删除
				bDestroyEvent = true;

				int lBulletFuncID = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_BULLET_FUNC);
				int lPenetrateCount = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_PENETRATE_COUNT);
				object_id_type lTargetID = pkReq->GetDstActorID();
/*
				// TODO: 当前目标是否可攻击；目标位置是否合理
				UT_Vec3Int kTargetLocation2;
				kTargetLocation2.x = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_X);
				kTargetLocation2.y = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Y);
				kTargetLocation2.z = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Z);
*/
				UT_Vec3Int kTargetLocation;
				kTargetLocation.x = (int32_t)pkReq->GetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_X);
				kTargetLocation.y = (int32_t)pkReq->GetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_Y);
				kTargetLocation.z = (int32_t)pkReq->GetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_Z);

				//子弹爆炸
				_broadcastBulletExplosive(pkSrcActor, (SK_BulletEvent*)pkEvent, kTargetLocation, lTargetID, (SK_BulletEvent::SK_BulletExplosiveReason)lActionType);

				//记录子弹位置
				pkSrcActor->SetBulletLocation(kTargetLocation);
				pkSkillExecute->BulletExplosive(lBulletFuncID, pkSrcActor, lTargetID, lPenetrateCount, _BULLET_UUID(lEventID, lBulletLocalID));
			}
			break;

		case SK_BulletEvent::SBAT_CHANGE_TARGET_LOCATION:
			{
				// TODO: 子弹位置是否合理；目的地是否在范围内

				int lMoveSpeed = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_MOVE_SPEED);
				int lBulletFuncID = (int32_t)pkEvent->GetParam(SK_BulletEvent::SEP_BULLET_FUNC);

				//开始周围穿透
				const SK_SkillExecute::BulletParam & kBP = pkSkillExecute->GetProperty_BulletParam(lBulletFuncID);
				int lBulletLockRange = kBP.m_lLockRange;
				if (lBulletLockRange == 0)	//不想周围穿刺请把lBulletLockRange设置为0
					break;

				//查找第一个穿刺的目标
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

				//计算子弹打击下一个目标飞行的距离，这个目标一定是不会在lBulletShapeRadius范围内的，因为如果在，前面列表已经处理过了，或者子弹已经穿刺过多爆炸了
				int iBulletFuncID = (int)pkEvent->GetParam(SK_BulletEvent::SEP_BULLET_FUNC);
				int lMaxMoveDistance = pkSkillExecute->GetProperty_BulletMaxMoveDistance(iBulletFuncID);
				const UT_Vec3Int & kNextTargetLocation = pkNextTarget->GetCurrentLocationWithZ();	//pkNextTarget只是用来确定下一个折射方向
				UT_Vec3Int kTargetLocation2 = UT_MathBase::MoveAlongIntXY(kBulletLocation, kNextTargetLocation, Long2Float(lMaxMoveDistance));
*/
				int lMoveDistance = UT_MathBase::LineLengthXYInt(kBulletLocation, kTargetLocation);
				float fNextMoveTime = Long2Float(lMoveDistance) / Long2Float(lMoveSpeed);

				pkEvent->SetParam(SK_BulletEvent::SEP_MOVE_START_TIME, SK_BulletEvent::ParamFloat2LLong(fCurrentTime));
				pkEvent->SetParam(SK_BulletEvent::SEP_MOVE_LIFE_TIME, SK_BulletEvent::ParamFloat2LLong(fNextMoveTime));
				pkEvent->SetParam(SK_BulletEvent::SEP_COLLIDE_PARAM, Skill::SerializeToQuatWord(SK_BulletEvent::CollideParam(1.0f, 0)));	// 修改目标时未做碰撞，重置碰撞参数

				//向被穿刺目标广播改变轨迹消息
				_broadcastBulletChangeTargetLocation(pkCurrentTarget, (SK_BulletEvent*)pkEvent, kBulletLocation, kTargetLocation);
			}
			break;
		}

		// 删除已处理的请求
		m_kEventPool.Free(pkReq);
		iter = rkReqList.erase(iter);

		// 已经处理过爆炸，后续的req无需再遍历
		if (bDestroyEvent)
			return;
	}
}

void Skill::SK_ServerSkillEventManager::_updateActiveBuddyBulletReqEvent(float fCurrentTime)
{
	/// 删除过期事件
	/// 如果 Update 中子弹事件处理能力ok，m_kActiveBuddyBulletReqEvents 应该每帧都会被清空，不会存在残留
	SK_SkillEventList::iterator iterLast = m_kActiveBuddyBulletReqEvents.end();
	SK_SkillEventList::iterator iterEnd = m_kActiveBuddyBulletReqEvents.end();
	for (SK_SkillEventList::iterator iter = m_kActiveBuddyBulletReqEvents.begin(); iter != iterLast; )
	{
		SK_SkillEvent* pkReq = *iter;
		if (fCurrentTime - pkReq->GetStartTime() > 1.0f)
		{
			m_kEventPool.Free(pkReq);
			*iter = *(--iterLast);		// 把最后的元素覆盖过来，容器遍历过程中不改变大小
		}
		else
		{
			++iter;
		}
	}
	/// 最后统一删除
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

	//搜索目标的范围
	UT_Circle kCircle;
	kCircle.iRadius = iCircleRadius;
	kCircle.kCenterPoint = rkCircleCenter;

	//从地图返回区域内的Actor队列
	LC_ActorPtrVector* pkLockTarget = pkMap->LockActorsInCircle(kCircle, pkFilterData);
	if (pkLockTarget->empty())
	{
		return object_id_vector();
	}
	//根据过滤设置,过滤Actor队列
	object_id_vector kFilterTarget = LC_Helper::FilterActorsSortDist(pkAttacker, *pkLockTarget, pkFilterData);

	//已打击目标
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

	//排除已打击的目标
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

	//排除多余的目标
	while (UInt2Int((unsigned int)kFinalTarget.size()) > iMaxLockCount)
	{
		kFinalTarget.pop_back();
	}
	return kFinalTarget;
}
//------------------------------------------------------------
void SK_ServerSkillEventManager::_broadcastBulletChangeTargetLocation(LC_ActorBase* pkBroadcastCenter, SK_BulletEvent* pkEvent, const Utility::UT_Vec3Int& rkBulletLocation, const Utility::UT_Vec3Int& rkTargetLocation)
{
	//设置子弹当前位置，通知到客户端用于执行技能的子弹接口
	pkEvent->SetParam(SK_BulletEvent::SEP_TARGET_LOCATION_X, rkTargetLocation.x);
	pkEvent->SetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Y, rkTargetLocation.y);
	pkEvent->SetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Z, rkTargetLocation.z);
	pkEvent->SetParam(SK_BulletEvent::SEP_SHOT_LOCATION_X, rkBulletLocation.x);
	pkEvent->SetParam(SK_BulletEvent::SEP_SHOT_LOCATION_Y, rkBulletLocation.y);
	pkEvent->SetParam(SK_BulletEvent::SEP_SHOT_LOCATION_Z, rkBulletLocation.z);
	//广播客户端改变轨迹消息
	pkEvent->SetParam(SK_BulletEvent::SEP_ACTION_TYPE, SK_BulletEvent::SBAT_CHANGE_TARGET_LOCATION);
	SendToClient(pkBroadcastCenter, pkEvent, SSET_BROADCAST_TO_AREA);
}
//------------------------------------------------------------
void SK_ServerSkillEventManager::_broadcastBulletPenetrate(LC_ActorBase* pkBroadcastCenter, SK_BulletEvent* pkEvent, const UT_Vec3Int& rkBulletLocation)
{
	//设置子弹当前位置，通知到客户端用于执行技能的子弹接口
	pkEvent->SetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_X, rkBulletLocation.x);
	pkEvent->SetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_Y, rkBulletLocation.y);
	pkEvent->SetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_Z, rkBulletLocation.z);
	//广播客户端穿透消息
	pkEvent->SetParam(SK_BulletEvent::SEP_ACTION_TYPE, SK_BulletEvent::SBAT_PENETRATE);
	SendToClient(pkBroadcastCenter, pkEvent, SSET_BROADCAST_TO_AREA);
}
//------------------------------------------------------------
void SK_ServerSkillEventManager::_broadcastBulletExplosive(LC_ActorBase* pkBroadcastCenter, SK_BulletEvent* pkEvent, const UT_Vec3Int& rkBulletLocation, long lTargetID, SK_BulletEvent::SK_BulletExplosiveReason enmReason)
{
	//设置子弹当前位置，通知到客户端用于执行技能的子弹接口
	pkEvent->SetDstActorID(lTargetID);
	pkEvent->SetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_X, rkBulletLocation.x);
	pkEvent->SetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_Y, rkBulletLocation.y);
	pkEvent->SetParam(SK_BulletEvent::SEP_CURRENT_LOCATION_Z, rkBulletLocation.z);
	//广播客户端爆炸消息
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