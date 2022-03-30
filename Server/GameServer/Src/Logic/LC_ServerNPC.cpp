#include "CF_MonsterAIFunc.h"
#include "LC_ServerNPC.h"
#include "SM_ServerControllerFactory.h"
#include "PF_ServerPathManager.h"
#include "LC_ServerMap.h"
#include "LC_ServerNPCManager.h"
#include "LC_ServerNPCOwnerList.h"
#include "SM_ServerStateFactory.h"
#include "LC_ServerNPCInteractionManager.h"
#include "LC_ActorSkillTrigger.h"
#include "LC_ServerWorldManager.h"
//#include "CF_SkillExecuteList.h"
#include "CF_AutoFitCharType.h"
#include "LC_AutoFitNPCManager.h"
#include "LC_ServerLogicManager.h"

#include "NW_Helper.h"
#include "SC_ServerScriptInterface.h"
#include "SC_ServerSkillInterface.h"
#include "SK_Factory.h"
#include "CF_SkillExecuteRule.h"
#include "LC_NPCGroupBase.h"
#include "LC_WorldManagerBase.h"
#include "LC_ServerGuildManager.h"
#include "UT_ServerHelper.h"
#include "UT_Redis.h"
#include "GameServerApp.h"
#include "CF_GuildTower.h"

using namespace GameLogic;
using namespace CSVFile;
using namespace StateMachine;
using namespace Utility;
using namespace Protocol;
using namespace Skill;
//--------------------------------------------------------
LC_ServerNPC::LC_ServerNPC()
: LC_NPCBase()
{
	m_stateCount = ST_NPC_STATE_END__;
	m_kStateMap =(SM_StateBase**) T_MALLOC(sizeof(SM_StateBase*)*m_stateCount, Memory::MEMCATEGORY_GENERAL);
	::memset(m_kStateMap, 0, sizeof(SM_StateBase*)*m_stateCount);
	_uninitStateMap();
	SM_StateMachine::Init();
	//Begin 路径数据
	m_kMovePath.Clear();
	m_nNoticeTime = 0;

	m_nUpdateHpTime = 0;

	m_nDropRandIndex = 0;
	m_nEscortNpcState = 0;

	m_kCacheMovePath.Clear();
	//End 路径数据

	//状态控制器
	m_pkSMController = SM_ServerControllerFactory::GetSingletonPtr()->RequestController(SCT_SERVER_NPC);

	ReUse();

	m_kSkillStateMap->SetDataChangerObserver(&m_dataChangeFlag);
}
//--------------------------------------------------------
LC_ServerNPC::~LC_ServerNPC()
{
	if(m_kStateMap)
	{
		T_FREE(m_kStateMap, Memory::MEMCATEGORY_GENERAL);
		m_kStateMap = NULL;
	}
}
//--------------------------------------------------------
void LC_ServerNPC::InitLogicData()
{
	LC_NPCBase::InitLogicData();

	CF_CharType::DataEntry* pkData = this->GetCharTypePtr();
	if ( pkData == NULL )
	{
		return ;
	}
//	T_ASSERT_CRITICAL_S(pkData);

	CF_MonsterAIFunc* pkMonsterAI = CF_MonsterAIFunc::GetSingletonPtr();
	CF_MonsterAIFunc::DataEntry* pkMonsterAIData = pkMonsterAI->GetEntryPtr( pkData->_lAIFunc );
	if ( pkMonsterAIData == NULL )
	{
		return ;
	}
//	T_ASSERT_CRITICAL_S(pkMonsterAIData);
	
	//初始化AI数据
	this->InitAIData(pkData, pkMonsterAIData);

	m_Elite_Level = 0;

	Skill::SK_Factory* pkSkillFactory = Skill::SK_Factory::GetSingletonPtr();

	bool first = true;

	for (int iIndex = 0; iIndex < MAX_ACTOR_SKILL_TRIGGER_COUNT; iIndex ++)
	{
		LC_SkillTriggerEntry* rkEntry = m_kSkillTrigger.GetTriggerByIndex(iIndex);
		if (rkEntry && rkEntry->GetValid() )
		{
			int skill_typeid = rkEntry->GetSkillTypeID();
			if(  IS_SKILL_TYPE(skill_typeid) )
			{
				Skill::SK_SkillExecute * skill = pkSkillFactory->RequestSkillExcute(skill_typeid);
				if(skill)
				{
					  if(first)
					  {
						  first = false;
						  m_SurroundRanger = skill->GetProperty_MaxExecuteDistance();
					  }
					  else if( m_SurroundRanger > skill->GetProperty_MaxExecuteDistance() && skill->GetProperty_MaxExecuteDistance() > 0 )
					  {
						  m_SurroundRanger = skill->GetProperty_MaxExecuteDistance();
					  }
				}
			}
		}
	}

	/*
  if(m_SurroundRanger > 200)
  {
    m_SurroundRanger = m_SurroundRanger * 4 / 5;
  }
	*/
	SM_StateMachine::Start(ST_NPC_BORN, GET_PROCESS_TIME, 0.0f);
}
//--------------------------------------------------------
void AddSkillRule(int rule_id,LC_ServerNPC& trigger)
{
	CF_SkillExecuteRule::DataEntry * entry = SafeGetCSVFileDataEntryPtr<CF_SkillExecuteRule>(rule_id);
	if(entry)
	{
		LC_SkillTriggerEntry pkEntry;
		pkEntry.SetType((unsigned short)entry->_lTriggerType);
		pkEntry.SetSkillTypeID(entry->_lSkillID);
		pkEntry.SetPriority((unsigned short)entry->_lPrio);
		pkEntry.SetCountLimit((unsigned short)entry->_lLimitCount);
		pkEntry.SetLifeTime(entry->_fLifeTime);
		pkEntry.SetData1(entry->_fData1);
		pkEntry.SetData2(entry->_fData2);
		pkEntry.SetData3(entry->_fData3);
		pkEntry.SetValid(true);
		pkEntry.SetShieldSkill(entry->_iShieldSkill == 1);
		trigger.AddSkill(pkEntry , (entry->_iRandomSkill == 1));
	}
}
//--------------------------------------------------------
void LC_ServerNPC::InitAIData(CSVFile::CF_CharType::DataEntry* pkCharData, CSVFile::CF_MonsterAIFunc::DataEntry* pkAIData)
{
	T_ASSERT_CRITICAL_S(pkCharData);
	T_ASSERT_CRITICAL_S(pkAIData);

	LC_NPCBase::InitAIData(pkCharData, pkAIData);

	LC_ServerNPCInteractionManager* pkInteractionManager = (LC_ServerNPCInteractionManager*)LC_ServerNPCInteractionManager::GetSingletonPtr();

	if (pkInteractionManager)
		SetCanInteraction(pkInteractionManager->CanInteraction(this->GetCharType()));

	//初始化控制器
	const StringType& rkScriptName = pkAIData->_kAIControlScriptPath;
	_initSMController(rkScriptName);

	//初始化状态表
	_initStateMap(rkScriptName);

	if(pkAIData->_lMinIdleWhenBorn > 0)
	{
		m_IdelWhenBron = (float)((float)UT_MathBase::RandInRangeInt(pkAIData->_lMinIdleWhenBorn,pkAIData->_lMaxIdleWhenBorn)/10000.0) + this->GetProperty_BornTime();
	}
	else
	{
		m_IdelWhenBron = this->GetProperty_BornTime();
	}
	//初始化AI功能列表
	const StringType& rkAIFuncList = pkAIData->_kAIFuncTypeList;
	if(!rkAIFuncList.empty() && rkAIFuncList != "xx")
		InitAIFuncTypeList(rkAIFuncList);

	//通过脚本初始化技能管理
	m_kSkillTrigger.Reset();
	m_kSkillTrigger.Init(pkCharData, pkAIData);
	if (pkCharData->_lSkillRuleID1)
		AddSkillRule(pkCharData->_lSkillRuleID1,*this);
	if (pkCharData->_lSkillRuleID2)
		AddSkillRule(pkCharData->_lSkillRuleID2,*this);
	if (pkCharData->_lSkillRuleID3)
		AddSkillRule(pkCharData->_lSkillRuleID3,*this);
	if (pkCharData->_lSkillRuleID4)
		AddSkillRule(pkCharData->_lSkillRuleID4,*this);
	if (pkCharData->_lSkillRuleID5)
		AddSkillRule(pkCharData->_lSkillRuleID5,*this);
	if (pkCharData->_lSkillRuleID6)
		AddSkillRule(pkCharData->_lSkillRuleID6,*this);
	if (pkCharData->_lSkillRuleID7)
		AddSkillRule(pkCharData->_lSkillRuleID7,*this);
	if (pkCharData->_lSkillRuleID8)
		AddSkillRule(pkCharData->_lSkillRuleID8,*this);
	if (pkCharData->_lSkillRuleID9)
		AddSkillRule(pkCharData->_lSkillRuleID9,*this);
	if (pkCharData->_lSkillRuleID10)
		AddSkillRule(pkCharData->_lSkillRuleID10,*this);

	SurroundTimeIntelval = pkAIData->_fKeepDistanceTime;
	SurroundTime = pkAIData->_fKeepDistanceTime;

	SetChaseInnerRadius(pkAIData->_lChaseInnerRadius);
	SetChaseOuterRadius(pkAIData->_lChaseOuterRadius);
	SetIsSurrounder(pkAIData->_bIsSurrounder);
	SetSwingAngle(pkAIData->_fSwingAngle);
	SetSwingMaxAngle(pkAIData->_fSwingMaxAngle);
	SetChaseMoveType(ChaseType(pkAIData->_iChaseType));
	SetHPRecoverRate(pkAIData->_iResumeRate);
	SetHPRecoverTimer(pkAIData->_fResumeTime);
	SetSleepProbability(pkAIData->_fSleepProbability);
	SetSleepTimeMin(pkAIData->_fSleepTimeMin);
	SetSleepTimeMax(pkAIData->_fSleepTimeMax);
}
//-----------------------------------------------------------------------------------
void LC_ServerNPC::AddSkill(GameLogic::LC_SkillTriggerEntry& data ,bool AddSkillNameSlot)
{
	SK_SkillExecute* pkSkillExecute = SK_Factory::GetSingletonPtr()->RequestSkillExcute(data.GetSkillTypeID());
	if (pkSkillExecute==NULL)
	{
		return;
	}
	CF_SkillSpecialAttrib::DataEntry* pkSkillData = pkSkillExecute->GetAttrDataEntryPtr();
	CF_SkillExecuteList::DataEntry* pkSkillInfo = pkSkillExecute->GetDataEntryPtr();
	
	if( SPNT_NEGTIVE == pkSkillInfo->_lPosNegType)//被动
	{
		if (pkSkillExecute->HasPassive())
		{
			pkSkillExecute->StartPassive(this);
		}
		else
		{
			ActiveSkillState(pkSkillData->_lSkillConstParameter1,0,this->GetID(),Utility::Int2Float(pkSkillData->_lSkillConstParameter2),Utility::Int2Float(pkSkillData->_lSkillConstParameter3),Utility::Int2Float(pkSkillData->_lSkillConstParameter4),Utility::Int2Float(pkSkillData->_lSkillConstParameter5),Utility::Int2Float(pkSkillData->_lSkillConstParameter6));
		}
	}
	else
	{
		//增加主动技能
		m_kSkillTrigger.AddSkill(data);
	}
	if(AddSkillNameSlot)
	{
		this->AddSkillID(data.GetSkillTypeID());
	}
}

void LC_ServerNPC::AddSkill(int32_t iSkillRuleID, skill_id_type iSkillID)
{
	CF_SkillExecuteRule::DataEntry * entry = SafeGetCSVFileDataEntryPtr<CF_SkillExecuteRule>(iSkillRuleID);
	if(entry && entry->_lSkillID==iSkillID)
	{
		LC_SkillTriggerEntry pkEntry;
		pkEntry.SetType((unsigned short)entry->_lTriggerType);
		pkEntry.SetSkillTypeID(entry->_lSkillID);
		pkEntry.SetPriority((unsigned short)entry->_lPrio);
		pkEntry.SetCountLimit((unsigned short)entry->_lLimitCount);
		pkEntry.SetLifeTime(entry->_fLifeTime);
		pkEntry.SetData1(entry->_fData1);
		pkEntry.SetData2(entry->_fData2);
		pkEntry.SetData3(entry->_fData3);
		pkEntry.SetValid(true);
		pkEntry.SetShieldSkill(entry->_iShieldSkill == 1);
		AddSkill(pkEntry , (entry->_iRandomSkill == 1));
	}
}
//-----------------------------------------------------------------------------------
void LC_ServerNPC::ReUse()
{
	LC_NPCBase::ReUse();

	//需要锁定目标
	m_bNeedLockTarget		= false;

	m_CombatEventData.ReUse();
	//Begin 路径数据
	m_kMovePath.Clear();
	m_kCacheMovePath.Clear();
	m_bCacheMovePath = false;
	m_nDropRandIndex = 0;

	m_nNoticeTime = 0;
	m_nUpdateHpTime = 0;

	m_nEscortNpcState = 0;

	//归属
	m_kNPCOwnerList.Clear();

	m_kHitPlayerID = 0;
	m_kHitTime = 0;

	//脚本控制
	m_bControledByScript = false;

	m_fLastPositiveActiveTime	= 0.0f;

	m_fLastExecuteSkillTime = 0.0f;

	m_fForceSyncPositionTime = 0.0f;

	m_Elite_Level = 0;

	m_Group_Elite_Level = 0;
	memset(m_resists,0,sizeof(m_resists));

	m_lSummorID = 0;
	m_bUpdateFlag = true;
	ResetChaseData();
}
void LC_ServerNPC::SetDead(bool bIsDead)
{
	LC_ActorBase::SetDead( bIsDead );
	bool bDead = GetDead();
	if ( bDead )
	{
		GetStateMap()->ClearStatesOnDead();
		object_id_type lAttackerID = GetAttackerID();
		LC_WorldManagerBase* pkWorldManager = ENGINE_GET_WORLD_MANAGER();
		LC_ActorBase* pkAttacker = (LC_PlayerBase*)pkWorldManager->FindObject(lAttackerID);
		//设置npc面向攻击者
		if (pkAttacker)
		{
			float r = UT_MathBase::RandInRangeFloat(-GetGlobalSetting.nNpcDeadDirRand,  GetGlobalSetting.nNpcDeadDirRand);
			UT_Vec3Int kIntDir = pkAttacker->GetCurrentLocation() - GetCurrentLocation();
			UT_Vec2Float kDirXY;
			kDirXY.x = (float)kIntDir.x;
			kDirXY.y = (float)kIntDir.y;
			float fDirLength = kDirXY.Unitize();
			float f = UT_MathBase::GetZRotAngle(kDirXY) + r;
			kDirXY = UT_MathBase::GetZRotDir(f);
			fDirLength = kDirXY.Unitize();
			if (fDirLength > UT_MathBase::Epsilon)
			{
				SetForwardDir(kDirXY);
				SetTargetForwardDir(kDirXY);
				//m_dataChangeFlag.GetBroadCastFlag().SetAngle();
				Protocol::MG_SyncNPCsAngleInFOV angleData;
				NPCAngleEntry& kEntry = angleData.m_rEntries[angleData.m_usEntryCount];

				kEntry.m_lID	= GetID();
				kEntry.m_iAngle		= LC_Helper::Angle_Float2Char( GetForwardDirAngle() );
				angleData.m_usEntryCount++;
				LC_ServerMap* pMap  = (LC_ServerMap*)GetMap();
				if ( pMap )
				{
					pMap->BroadCastToArea(GetSectorIndex(), MGPT_SYNC_NPCS_ANGLE_IN_FOV, &angleData);
				}
			}
		}
	}
}
void LC_ServerNPC::RegisterCombatEventData( COMBAT_EVENT_TYPE eEventType, COMBAT_EVENT_KEY_TYPE nKey, COMBAT_EVENT_VALUE_TYPE nValue )
{
	m_CombatEventData.Register(eEventType, nKey, nValue);
}

void LC_ServerNPC::UnregisterCombatEventData( COMBAT_EVENT_TYPE eEventType, COMBAT_EVENT_KEY_TYPE nKey )
{
	m_CombatEventData.Unregister(eEventType, nKey);
}

void LC_ServerNPC::ModifyCombatEventData( COMBAT_EVENT_TYPE eEventType, COMBAT_EVENT_KEY_TYPE nKey, COMBAT_EVENT_VALUE_TYPE nDeltaValue )
{
	m_CombatEventData.Modify(eEventType, nKey, nDeltaValue);
}

bool LC_ServerNPC::GetCombatEventData( COMBAT_EVENT_TYPE eEventType, COMBAT_EVENT_KEY_TYPE nKey, COMBAT_EVENT_VALUE_TYPE& nValue )
{
	return m_CombatEventData.Get(eEventType, nKey, nValue);
}

COMBAT_EVENT_VALUE_TYPE LC_ServerNPC::GetCombatEventData( COMBAT_EVENT_TYPE eEventType, COMBAT_EVENT_KEY_TYPE nKey )
{
	return m_CombatEventData.Get(eEventType, nKey);
}

//-------------------------------------------------------
void LC_ServerNPC::Update( float fCurrentTime,float fDeltaTime )
{
	//注意：防止登录、转服时的时序问题导致地图为空
	if (NULL == m_pkMap)
	{
		return;
	}

	if(m_bForbiddenUpdate)
	{
		return;
	}
	int32_t nOrigCurState = m_iCurrentState;
	int32_t nOrigNextState = m_iNextState;
	int32_t nOrigDefState = m_iDefaultState;
	int32_t nOrigPreState = m_iPreState;

	LC_NPCBase::Update(fCurrentTime,fDeltaTime);
	//更新状态
	m_pkSMController->Update(this, fCurrentTime, fDeltaTime);

	//更新技能状态
	m_kSkillStateMap->Update(fCurrentTime);

	//如果有跟随目标，更新归属
	if(IS_PLAYER_CLASS_ID(GetFollowedTargetID()))
		_updateOwnerByFollowTarget();
	else
	{
		int lMapLogicType = GetMapLogicType();
		// 祖传代码 这里永远进去不。 此模块暂未理清，不好轻易修改。QA 测试有bug 再去动
		if(lMapLogicType < MT_TRANSCRIPTION_BEGIN && lMapLogicType > MT_TRANSCRIPTION_END)
			m_kNPCOwnerList.Update(fCurrentTime,GetMapLogicID(), m_kCurrentLocation,false,true);
		else
			m_kNPCOwnerList.Update(fCurrentTime,GetMapLogicID(), m_kCurrentLocation,false,false);
	}

	//only npc need to do this update for now!
	m_kEncountedStatMgr.Update(this, GET_CURRENT_TIMESTAMP_IN_SECONDS());//更新战斗状态
	m_kHatredList.Update(fCurrentTime, 60.0f);

	SaveDamageToRedis();

	//首杀玩家死亡，重置首杀
	object_id_type HitPlayerID = GetHitPlayerID();
	if(0 != HitPlayerID)
	{
		LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(pkServerPlayerManager->FindPlayer(HitPlayerID));
		if(NULL == pkPlayer)
		{
			SetHitPlayer(0);
		}
	}

	UpdateForAutoRecover(fDeltaTime);
}
//-----------------------------------------------------------
void LC_ServerNPC::SetForwardDir(const Utility::UT_Vec2Float& rkDir)
{
	const Utility::UT_Vec2Float& olddir = GetForwardDir();
	if (olddir.x != rkDir.x ||
		olddir.y != rkDir.y )
	{
		LC_ActorBase::SetForwardDir(rkDir);	
		//FindPathTo(rkTargetLocation);
	}
}
//--------------------------------------------------------
bool LC_ServerNPC::SetTargetLocation(const Utility::UT_Vec3Int& rkTargetLocation)
{
	const Utility::UT_Vec3Int& oldLocation = GetTargetLocation();
	if (oldLocation.x != rkTargetLocation.x ||
		oldLocation.y != rkTargetLocation.y ||
		oldLocation.z != rkTargetLocation.z)
	{
		int32_t nDistance = UT_MathBase::LineLengthXYInt(oldLocation, rkTargetLocation);
		LC_ActorBase::SetTargetLocation(rkTargetLocation);	
		//FindPathTo(rkTargetLocation);
		m_dataChangeFlag.GetBroadCastFlag().SetLocation();
		return true;
	}
	return false;
}
//--------------------------------------------------------
void LC_ServerNPC::SetMoveLineTargetLocation(const Utility::UT_Vec3Int& kLocation)
{
	const Utility::UT_Vec3Int& oldLocation = GetMoveLineTargetLocation();
	if (oldLocation.x != kLocation.x ||
		oldLocation.y != kLocation.y ||
		oldLocation.z != kLocation.z)
	{
		m_dataChangeFlag.GetBroadCastFlag().SetLocation();
		LC_NPCBase::SetMoveLineTargetLocation(kLocation);	
	}
}

bool LC_ServerNPC::GetDenyDead()
{
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerMapLogic_Place* pkMapLogic = NULL;
	pkMapLogic = pkWorldManager->GetMapLogic(GetMapLogicID());
	if(pkMapLogic && pkMapLogic->GetPlaceState() >= PLACESTATE_FAIL)
	{
		return true;
	}

	return LC_ActorBase::GetDenyDead();
}

bool LC_ServerNPC::CheckForceUpdate()
{
	return GetSaveDamageDataFlag() && 0==m_nSaveDamageTime;
}

void LC_ServerNPC::SaveDamageToRedis()
{
	if(!GetSaveDamageDataFlag())
	{
		return;
	}

	uint32_t currentTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	if(m_nSaveDamageTime==0 || currentTime-m_nSaveDamageTime>=10)
	{
		UpdateDamageToRedis();
		bool tag = UpdateBossCurrentHp();
		if(tag)
		{
			m_nSaveDamageTime = currentTime;
		}		
	}
}

void LC_ServerNPC::UpdateDamageToRedis()
{
	LC_EncountedStatMgr& kStatMgr = GetEncountedStatMgr();
	LC_EncountedStat* pkStat = kStatMgr.GetStat(LC_ECT_TP_Damage);
	if (NULL == pkStat)
	{
		return;
	}

	bool tag = pkStat->GetDirtyFlag();
	if(!tag)
	{
		return;
	}

	BossDamageInfo info;
	pkStat->GetStatInfo(LC_ECT_CAT_ACTOR, info.m_nActorDamageInfo);
	pkStat->GetStatInfo(LC_ECT_CAT_GUILD, info.m_nGuildDamageInfo);
	pkStat->GetPlayerInstance(LC_ECT_CAT_ACTOR, info.m_nPlayerIDToInstanceID);
	kStatMgr.GetPlayerName(info.m_nActorName);
	kStatMgr.GetGuildName(info.m_nGuildName);

	if(info.m_nActorDamageInfo.empty() && info.m_nGuildDamageInfo.empty() && info.m_nPlayerIDToInstanceID.empty() && info.m_nActorName.empty())
	{
		return;
	}

	int charTypeID = GetCharType();
	int mapResID = GetMapResID();
	uint64_t res = LC_Helper::EncodeScore(charTypeID, mapResID);

	std::stringstream ss;
	boost::ajson::save_to_buff(info, ss);
	std::string data = ss.str().c_str();

	mem::vector<uint64_t> key;
	key.push_back(res);

	UT_ServerHelper::RedisSetData_String(UT_REDIS_KEY_BOSS_DAMAGE_INFO, key, data, false, true);

	pkStat->SetDirtyFlag(false);
}

void LC_ServerNPC::UpdateDamageInfoFromRedis(std::string& str)
{
	BossDamageInfo info;
	std::string errorMsg;
	if(!boost::ajson::load_from_buff(info, str.c_str(), errorMsg))
	{
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_INFO, "UpdateDamageInfoFromRedis Failed parse string", errorMsg);
		return;
	}

	LC_EncountedStatMgr& kStatMgr = GetEncountedStatMgr();
	LC_EncountedStat* pkStat = kStatMgr.GetStat(LC_ECT_TP_Damage);
	if (NULL == pkStat)
	{
		return;
	}

	kStatMgr.SetPlayerName(info.m_nActorName);
	kStatMgr.SetGuildName(info.m_nGuildName);
	pkStat->SetStatInfo(LC_ECT_CAT_ACTOR, info.m_nActorDamageInfo);
	pkStat->SetStatInfo(LC_ECT_CAT_GUILD, info.m_nGuildDamageInfo);
	pkStat->SetPlayerInstance(LC_ECT_CAT_ACTOR, info.m_nPlayerIDToInstanceID);
}

bool LC_ServerNPC::UpdateBossCurrentHp()
{
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerMapLogicPlaceVector* pkLogicMaps = pkWorldManager->GetMapLogicByRes(GetGlobalSetting.BestGuildMapID);
	if(NULL==pkLogicMaps)
	{
		return false;
	}

	LC_ServerMapLogicPlaceVector::iterator it=pkLogicMaps->begin();
	LC_ServerMapLogic_Place* pkBestGuildPlace = *it;
	if(NULL == pkBestGuildPlace)
	{
		return false;
	}

	return pkBestGuildPlace->SetMonsterHP(GetCharType(), GetHP(), GetMapResID());
}

int32_t LC_ServerNPC::GetNoticeTime()
{
	return m_nNoticeTime;
}

void LC_ServerNPC::SetNoticeTime(int32_t time)
{
	m_nNoticeTime = time;
}

void LC_ServerNPC::RecordDamage(LC_ActorBase* pkObject, attr_value_type lPara)
{
	LC_ActorBase::RecordDamage(pkObject, lPara);

	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerMapLogic_Place* pkPlace = pkWorldManager->GetMapLogic(GetMapLogicID());
	if(NULL != pkPlace)
	{
		pkPlace->UpdatePlayerDamage(pkObject, lPara);
	}

	if(GetGuildID()>0)
	{
		int time = GET_CURRENT_TIMESTAMP_IN_SECONDS();
		int lastTime = GetNoticeTime();
		if(0!=lastTime && time-lastTime < 30)
		{
			return;
		}

		bool tag = SendNoticeToGuildPlayer(21);
		if(tag)
		{
			SetNoticeTime(time);
		}
	}
	else if(UT_ServerHelper::IsEscortNPC(GetCharType()))
	{
		int time = GET_CURRENT_TIMESTAMP_IN_SECONDS();
		int lastTime = GetNoticeTime();
		if(0!=lastTime && time-lastTime < 10)
		{
			return;
		}

		LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
		if (pkPlayerManager == NULL)
		{
			return;
		}

		object_id_type id = GetSummonOwnerID();
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(id);
		if(NULL != pkPlayer)
		{
			//给玩家发送被攻击公告
			UT_ServerHelper::_GameNoticeFmort noticeFmort;
			noticeFmort.SetMsgID(24);
			UT_ServerHelper::BrocastNoticeMsg(noticeFmort, pkPlayer->GetCitizenship(), pkPlayer);

			bool tag = SetEscortNpcState(EscortNPCState::EscortNPC_2);
			if(tag)
			{
				pkPlayer->SyncEscortNpcState(GetID(), GetCharType(), EscortNPCState::EscortNPC_2);
			}
		}

		SetNoticeTime(time);
	}
}

bool LC_ServerNPC::SendNoticeToGuildPlayer(int noticeID)
{
	int mapLogicID = GetMapLogicID();
	static LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();

	LC_ServerMapLogic_Place* pkCurrentPlace =  (LC_ServerMapLogic_Place*)pkWorldManager->GetMapLogic(mapLogicID);
	if(NULL == pkCurrentPlace)
	{
		return false;
	}

	LC_ServerMapLogicPlaceVector* pkLogicMaps = pkWorldManager->GetMapLogicByRes(GetGlobalSetting.BestGuildMapID);
	if(NULL==pkLogicMaps)
	{
		return false;
	}

	LC_ServerMapLogicPlaceVector::iterator it=pkLogicMaps->begin();
	LC_ServerMapLogic_Place* pkPlace = *it;
	if(NULL == pkPlace)
	{
		return false;
	}

	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if(NULL == pkGuildManager)
	{
		return false;
	}

	int storyID = pkCurrentPlace->GetStoryID();
	int npcID = GetCharType();
	uint64_t guildID = pkPlace->GetBossGuildID(storyID, npcID);
	int townerID = pkPlace->GetTowerID(storyID, npcID);
	if(0 == guildID)
	{
		return false;
	}

	//发送被攻击公告
	UT_ServerHelper::_GameNoticeFmort noticeFmort;
	noticeFmort.SetMsgID(noticeID);
	noticeFmort.AddCharType(townerID);
	UT_ServerHelper::BrocastNoticeMsg(noticeFmort, guildID);

	return true;
}

int32_t LC_ServerNPC::GetRandIndex()
{
	return m_nDropRandIndex;
}

void LC_ServerNPC::UpdateRandIndex(int32_t randIndex)
{
	m_nDropRandIndex = randIndex;
}

void LC_ServerNPC::ClearRandIndex()
{
	m_nDropRandIndex = 0;
}

//--------------------------------------------------------
void LC_ServerNPC::FindPathTo( const Utility::UT_Vec3Int& rkTarget )
{
	Utility::UT_LinePath& rMovePath = GetMovePath();
	int iPathPointCount = rMovePath.GetPathPointCount();
	if(iPathPointCount == 0 || 
		UT_MathBase::LineLengthXYInt( rkTarget , m_kCurrentLocation/*m_kMovePath.GetPathPoint(iPathPointCount - 1)*/) > DISTANCE_ADJUST)
	{
		if (PF_ServerPathManager::GetSingletonPtr() )
		{
			LC_MapBase* pkMap = GetMap();
			if(!pkMap)
				return;

			int nMapResID = GetMapResID();
			bool bFindResult = PF_ServerPathManager::GetSingletonPtr()->FindPath( nMapResID , rMovePath, m_kCurrentLocation , rkTarget);

			if(!bFindResult)
			{
				rMovePath.Clear();
				SetCurrentLocation(GetCurrentLocation());
				SetMoveLineTargetLocation(GetCurrentLocation());
			}
		}
	}
}
//--------------------------------------------------------
void LC_ServerNPC::CacheCurrentPath(const Utility::UT_Vec3Int&	rkCacheLocation)
{
	if(m_bCacheMovePath)
		return;

	m_bCacheMovePath = true;

	m_kCacheMovePath.Clone(m_kMovePath);
	
	if(m_kCacheMovePath.GetLinePathType() == UT_LinePath::LPT_FLIP)
	{
		if(m_kCacheMovePath.GetMoveStep() >= 1.0f)
		{
			m_kCacheMovePath.ReloadPathPointData();
			m_kCacheMovePath.GeneratePath();
		}
	}

	m_kCacheLocation = rkCacheLocation;
}
//--------------------------------------------------------
void LC_ServerNPC::RecoverCurrentPath()
{
	if(!m_bCacheMovePath)
		return;

	m_bCacheMovePath = false;

	m_kMovePath.Clone(m_kCacheMovePath);

	m_kCacheMovePath.Clear(); 
}
//--------------------------------------------------------
bool LC_ServerNPC::NotifyStateEnd( int iEndState,float fCurrentTime,float fDeltaTime )
{
	LC_NPCBase::NotifyStateEnd(iEndState, fCurrentTime, fDeltaTime);
	return m_pkSMController->NotifyStateEnd(this, iEndState, fCurrentTime, fDeltaTime);
}
//----------------------------------------------------------
void LC_ServerNPC::NotifySkillEvent(SK_SkillEvent* pkSkillEvent)
{
	if (NULL == pkSkillEvent)
	{
		//T_ASSERT_CRITICAL_S(false);
		return;
	}

	LC_NPCBase::NotifySkillEvent(pkSkillEvent);

	if(GetID() != pkSkillEvent->GetSrcActorID())
	{
		//通知控制器
		m_pkSMController->NotifySkillEvent(this, pkSkillEvent);
	}
	
	//通知npc队伍
	if (IS_NPC_GROUP_ID(GetGroupID()) && GetGroupMemberID() != 0 && GetNPCGroup() != NULL)
	{
		GetNPCGroup()->NotifySkillEvent(pkSkillEvent);
	}
}
//----------------------------------------------------------
ResultType LC_ServerNPC::CheckPendingSkill(int lSkillTypeID)
{
	CF_SkillExecuteList::DataEntry* pkSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(lSkillTypeID);
	if (NULL == pkSkillData)
	{
		return RE_SKILL_SYSTEM_ERROR;
	}

	SK_SkillEventManagerBase* pkEventManager = ENGINE_GET_SKILL_EVENT_MANAGER();
	if (NULL == pkEventManager)
	{
		return RE_SKILL_SYSTEM_ERROR;
	}

	//沉默状态释放技能失败
	if ( GetSkillFlag(SK_SILENCE)
		/*|| GetSkillFlag(SK_SILENCE_MELEE) && SRT_MELEE == pkSkillData->_lRangeType
		|| GetSkillFlag(SK_SILENCE_RANGE) && SRT_RANGE == pkSkillData->_lRangeType*/ )
	{
		SK_SkillEvent* pkEvent = pkEventManager->AllocEvent();
		if (NULL != pkEvent)
		{
			pkEvent->SetEventType(SETE_EXECUTE_SKILL);
			pkEvent->SetSrcActorID(GetID());
			pkEvent->SetDstActorID(GetLockedTargetID());
			pkEvent->SetLogicID(lSkillTypeID);
			pkEvent->SetParam(SK_ExecuteSkillEvent::SEP_RESULT, SK_ExecuteSkillEvent::SER_FAIL_SILENCE);
			pkEventManager->AddEvent(pkEvent);
		}

		return RE_SKILL_CHECK_SKILLFLAG_OVERDO;
	}

	//mp不足释放技能失败
	int32_t iDefaultMPCost = LC_Helper::GetSkillDefaultMPCost(this, pkSkillData);
	int32_t iAdditionMPCost = LC_Helper::GetSkillAdditionMPCost(this, pkSkillData);
	if (GetMP() < iDefaultMPCost + iAdditionMPCost)
	{
		SK_SkillEvent* pkEvent = pkEventManager->AllocEvent();
		if (NULL != pkEvent)
		{
			pkEvent->SetEventType(SETE_EXECUTE_SKILL);
			pkEvent->SetSrcActorID(GetID());
			pkEvent->SetDstActorID(GetLockedTargetID());
			pkEvent->SetLogicID(lSkillTypeID);
			pkEvent->SetParam(SK_ExecuteSkillEvent::SEP_RESULT, SK_ExecuteSkillEvent::SER_FAIL_LACK_MP);
			pkEventManager->AddEvent(pkEvent);
		}

		return RE_SKILL_NOT_ENOUGH_MP;
	}
	return RE_SKILL_NONE_ERROR;
}
//----------------------------------------------------------
void LC_ServerNPC::AIFillExecuteSkillParam(Skill::SK_SkillExecute* pkSkill)
{
	if (NULL == pkSkill)
	{
		return;
	}
	int iOperationType	= pkSkill->GetProperty_OperationType();
	switch (iOperationType)
	{
	case SOT_TARGET: //目标型
		{
			object_id_type lTargetID = this->GetLockedTargetID();
			LC_LogicObject* pkTarget = this->GetMap()->FindObject(lTargetID);
			if(pkTarget) 
			{
				const UT_Vec3Int & kCurrentLocation = this->GetCurrentLocation();
				LC_Helper::FaceActorToLocation(this, kCurrentLocation, pkTarget->GetCurrentLocation());
			}
		}
		break;

	case SOT_DIR:	//朝向型
		{
			object_id_type lTargetID = this->GetLockedTargetID();
			LC_LogicObject* pkTarget = this->GetMap()->FindObject(lTargetID);
			const UT_Vec3Int & kCurrentLocation = this->GetCurrentLocation();
			const UT_Vec3Int & kTargetLocation  = (NULL != pkTarget) ? pkTarget->GetCurrentLocation() : this->GetLockedLocation();
			if(NULL != pkTarget)	this->SetLockedLocation(kTargetLocation);	//跟踪目标，将用来设置TargetLocation
			LC_Helper::FaceActorToLocation(this, kCurrentLocation, kTargetLocation);
		}
		break;

	case SOT_LOCATION: //地点型
		{
			//以地面位置设为目标点
			object_id_type lTargetID = this->GetLockedTargetID();
			LC_LogicObject* pkTarget = this->GetMap()->FindObject(lTargetID);
			//UT_Vec3Int kCurrentLocation = this->GetCurrentLocation();
			const UT_Vec3Int & kTargetLocation  = (NULL != pkTarget) ? pkTarget->GetCurrentLocation() : this->GetLockedLocation();
			if(NULL != pkTarget)	this->SetLockedLocation(kTargetLocation);	//跟踪目标，将用来设置TargetLocation
		}
		break;
	default:
		break;
	}

}
//----------------------------------------------------------
ResultType LC_ServerNPC::CheckExecuteSkill(float fCurrentTime,Skill::SK_SkillExecute* pkSkill,int lItemID)
{
	if (NULL == pkSkill)
	{
		return RE_SKILL_SYSTEM_ERROR;
	}

	int lSkillType = pkSkill->GetExecuteID();
	CF_SkillExecuteList::DataEntry* pkSkillData = pkSkill->GetDataEntryPtr();
	if (NULL == pkSkillData)
		return RE_SKILL_SYSTEM_ERROR;

	//判断触发条件
	{
		int iOperationType			 = pkSkill->GetProperty_OperationType();
		int iLockTargetType			 = pkSkill->GetProperty_LockTargetType();
		int iSkillExecuteMinDistance = pkSkill->GetProperty_MinExecuteDistance();
		int iSkillExecuteMaxDistance = pkSkill->GetProperty_MaxExecuteDistance();
		int	 lExecuteDistanceFunc	 = pkSkill->GetProperty_ExecuteDistanceFunc();

		const UT_Vec3Int & kCurrentLocation = GetCurrentLocation();
		UT_Vec3Int kTargetLocation = kCurrentLocation;
		ResultType iResult = 0;

		switch (iOperationType)
		{
		case SOT_TARGET: //目标型
			{
				object_id_type lTargetID = 0;
				LC_LogicObject* pkTarget = NULL;
				CF_ActorFilterFunc* pkCSVActorFilterFunc = CF_ActorFilterFunc::GetSingletonPtr();
				CF_ActorFilterFunc::DataEntry* pkData = pkCSVActorFilterFunc->GetEntryPtr(iLockTargetType);
				if (NULL == pkData)
					return RE_SKILL_SYSTEM_ERROR;

				lTargetID = GetLockedTargetID();
				LC_MapBase* pkMap = GetMap();
				if(!pkMap)
					return RE_SKILL_LOST_TARGET;

				pkTarget = pkMap->FindObject(lTargetID);

				if(!pkTarget || LC_Helper::IsValidTarget(this,lTargetID,pkData) != RE_SKILL_CHECK_VALID_TARGET)
					iResult = RE_SKILL_LOST_TARGET;
				else
				{
					iResult = RE_SKILL_NONE_ERROR;					
					kTargetLocation  = pkTarget->GetCurrentLocation();
					int iSafeRadius	 = GetProperty_ShapeRadius();
					if(IS_PLAYER_CLASS_ID(lTargetID) || IS_NPC_ID(lTargetID))
					{
						iSafeRadius	+= ((LC_ActorBase*)pkTarget)->GetProperty_ShapeRadius();
					}
					
					kTargetLocation = UT_MathBase::ComputeSafeRadiusTargetLocation(kCurrentLocation,kTargetLocation,iSafeRadius);

					int iDistance = UT_MathBase::LineLengthXYInt(kCurrentLocation,kTargetLocation);
					if (iDistance < iSkillExecuteMinDistance && lExecuteDistanceFunc == SEDF_CLAMP)
					{
						iResult = RE_SKILL_DISTANCE_NEAR;
					}
					else if(iDistance > iSkillExecuteMaxDistance)
					{					
						iResult = RE_SKILL_DISTANCE_FAR;
					}
					else
					{
						/*
						if (iSkillExecuteMaxDistance > 100)
						{
							LC_SectorBase * sect = GetMap()->GetSector(GetSectorIndex());
							LC_NPCIDPtrMap& npcs = sect->GetNPCIDPtrMap();
							LC_ServerNPCManager * NPCManager = SERVER_GET_NPC_MANAGER();
							for( LC_NPCIDPtrMap::iterator i = npcs.begin(); i != npcs.end(); ++i )
							{
								if(i->first != GetID())
								{
									LC_ServerNPC * nearnpc = (LC_ServerNPC*)NPCManager->FindNPC(i->first);
									if(NULL != nearnpc)
									{
										if(IS_SKILL_TYPE(nearnpc->GetCurrentStateType()))
										{
											iDistance = UT_MathBase::LineLengthXYInt(GetCurrentLocation(),nearnpc->GetCurrentLocation());
											if(iDistance < (80+iSafeRadius)/2)
											{
												iResult = RE_SKILL_DISTANCE_FAR;
												break;
											}
										}										
									}
								}
							}
						}
						*/
					}
				}
			}
			break;

		case SOT_DIR:	//朝向型
			{
				if (lExecuteDistanceFunc != SEDF_CHECK)	//不检查距离则不做距离检测
				{
					iResult = RE_SKILL_NONE_ERROR;
				}
				else
				{
					object_id_type lTargetID = this->GetLockedTargetID();
					LC_LogicObject* pkTarget = this->GetMap()->FindObject(lTargetID);
					//pkTarget will be NULL when lTargetID == 0

					iResult = RE_SKILL_NONE_ERROR;

					const UT_Vec3Int & kCurrentLocation = this->GetCurrentLocation();
					UT_Vec3Int kTargetLocation  = (NULL != pkTarget) ? pkTarget->GetCurrentLocation() : this->GetLockedLocation();
					if(NULL != pkTarget)	this->SetLockedLocation(kTargetLocation);	//跟踪目标，将用来设置TargetLocation

					int iSafeRadius	 = this->GetProperty_ShapeRadius();
					if((IS_PLAYER_CLASS_ID(lTargetID) || IS_NPC_ID(lTargetID)) && NULL != pkTarget)
						iSafeRadius	+= ((LC_ActorBase*)pkTarget)->GetProperty_ShapeRadius();
					kTargetLocation = UT_MathBase::ComputeSafeRadiusTargetLocation(kCurrentLocation,kTargetLocation,iSafeRadius);

					int iDistance = UT_MathBase::LineLengthXYInt(kCurrentLocation,kTargetLocation);
					if(iDistance > iSkillExecuteMaxDistance)
						iResult = RE_SKILL_DISTANCE_FAR;
				}
			}
			break;

		case SOT_LOCATION: //地点型
			{
				//以地面位置设为目标点
				iResult = RE_SKILL_NONE_ERROR;
				kTargetLocation = GetLockedLocation();
				int iCurrentDistance = UT_MathBase::LineLengthXYInt(kCurrentLocation,kTargetLocation);
				if(iCurrentDistance > iSkillExecuteMaxDistance && lExecuteDistanceFunc == SEDF_CHECK)
					iResult = RE_SKILL_DISTANCE_FAR;
			}
			break;

		default:
			break;
		}

		switch (iResult)
		{
		case RE_SKILL_LOST_TARGET:
			break;
		case RE_SKILL_CHECK_INVALID_TARGET:
			break;
		case RE_SKILL_DISTANCE_TOO_FAR:
			break;
		}

		if(iResult != RE_SKILL_NONE_ERROR)
			return iResult;
	}

	return LC_ActorBase::CheckExecuteSkill(fCurrentTime,pkSkill,lItemID);

	////通知释放技能消息
	//NotifySkillEvent(SET_EXECUTE_PENDING_SKILL, GetID(), m_iPendingSkillExecuteType, Float2Long(fCurrentTime), 0);
}
//----------------------------------------------------------
ResultType LC_ServerNPC::PrepareExecuteSkill(Skill::SK_SkillExecute* pkSkill,int lItemID)
{
	if (NULL == pkSkill)
	{
		return RE_SKILL_SYSTEM_ERROR;
	}

	int iOperationType = pkSkill->GetProperty_OperationType();
//	int iLockTargetType = pkSkill->GetProperty_LockTargetType();
//	int iSkillExecuteMinDistance = pkSkill->GetProperty_MinExecuteDistance();
//	int iSkillExecuteMaxDistance = pkSkill->GetProperty_MaxExecuteDistance();
//	bool bExecuteOnCorpus = pkSkill->GetProperty_ExecuteOnCorpus();
//	bool bNeedTestPath = pkSkill->GetProperty_NeedTestPath();
//	bool bExecuteImmediately = pkSkill->GetProperty_ExecuteImmediately();

	const UT_Vec3Int & kCurrentLocation = GetCurrentLocation();

	switch (iOperationType)
	{
	case SOT_TARGET: //目标型
		{
			object_id_type lTargetID = 0;
			LC_LogicObject* pkTarget = NULL;

			lTargetID = GetLockedTargetID();
			LC_MapBase* pkMap = GetMap();
			if(!pkMap)
				return RE_SKILL_LOST_TARGET;

			pkTarget = pkMap->FindObject(lTargetID);

			if(!pkTarget)
			{
				return RE_SKILL_LOST_TARGET;
			}
			else
			{
				UT_Vec3Int kTargetLocation  = pkTarget->GetCurrentLocation();
				int iSafeRadius	 = GetProperty_ShapeRadius();
				if(IS_PLAYER_CLASS_ID(lTargetID) || IS_NPC_ID(lTargetID))
				{
					iSafeRadius	+= ((LC_ActorBase*)pkTarget)->GetProperty_ShapeRadius();
				}

				kTargetLocation = UT_MathBase::ComputeSafeRadiusTargetLocation(kCurrentLocation,kTargetLocation,iSafeRadius);
				SetTargetLocation(kTargetLocation);
				LC_Helper::FaceActorToLocation(this, GetCurrentLocation(), kTargetLocation);
			}
		}
		break;

	case SOT_DIR: //朝向型
		{
			//以地面位置设为目标点
			const UT_Vec3Int & kTargetLocation = GetLockedLocation();

			//可触发技能,设置目标点
			SetTargetLocation(kTargetLocation);

			LC_Helper::FaceActorToLocation(this, GetCurrentLocation(), kTargetLocation);
		}
		break;

	case SOT_LOCATION: //地点型
		{
			//以地面位置设为目标点
			const UT_Vec3Int & kTargetLocation = GetLockedLocation();

			//可触发技能,设置目标点
			SetTargetLocation(kTargetLocation);
		}
		break;

	default:
		break;
	}

	return RE_SKILL_NONE_ERROR;
}
//----------------------------------------------------------
ResultType LC_ServerNPC::ExecuteSkill(int skillid)
{
	SK_Factory* pkSkillFactory = SK_Factory::GetSingletonPtr();
	SK_SkillExecute* pkSkill = pkSkillFactory->RequestSkillExcute(skillid);

	if(!pkSkill)
	{
		return RE_SKILL_SYSTEM_ERROR;
	}
	return ExecuteSkill(GET_PROCESS_TIME, pkSkill, 0);
}
//------------------------------------------------------------
ResultType LC_ServerNPC::ExecuteSkill(float fCurrentTime,Skill::SK_SkillExecute* pkSkill,int lItemID)
{
	ResultType iResult = LC_ActorBase::ExecuteSkill(fCurrentTime,pkSkill,lItemID);
	if(RE_SKILL_EXECUTE_SUCCESS != iResult)
	{
		return iResult;
	}

	int lSkillType = pkSkill->GetExecuteID();
	CF_SkillExecuteList::DataEntry* pkSkillData = pkSkill->GetDataEntryPtr();
	if(NULL == pkSkillData)
	{
		return RE_SKILL_SYSTEM_ERROR;
	}

	//互斥骑乘状态
	if(GetRiderCharType() > 0)
	{
		if(pkSkillData->_bMutexWithRider)
		{
			SC_ServerSkillInterface* pkSkillInterface = SC_ServerScriptInterface::GetSingleton().GetSkillInterface();
			pkSkillInterface->DetachTargetState(this,TARGET_ME,SMT_KIND,99,0,0);
		}
	}

	//扣除资源
	int32_t iDefaultMPCost = LC_Helper::GetSkillDefaultMPCost(this, pkSkillData);
	int32_t iAdditionMPCost = LC_Helper::GetSkillAdditionMPCost(this, pkSkillData);
	SetMP(GetMP() - iDefaultMPCost - iAdditionMPCost);

	SetTargetLocation(GetCurrentLocation());
	SetMoveLineTargetLocation(GetCurrentLocation());

	//重丢骰子 
	RecomputeRandNums();

	//发送网络消息
	switch (pkSkill->GetProperty_OperationType())
	{
	case SOT_TARGET:
		{
			MG_Rlt_NPCExecuteSkillTypeTarget msg;
			msg.m_lNPCID			= GetID();
			msg.m_lSkillExecuteID	= lSkillType;
			msg.m_lLockedTargetID	= GetLockedTargetID();
			msg.m_iForwardDirAngle	= (int)GetForwardDirAngle();
			msg.m_lCurrentLocationX = GetCurrentLocation().x;
			msg.m_lCurrentLocationY = GetCurrentLocation().y;
			GetRandNums(msg.m_afRandNums);
			msg.m_fServerTime		= fCurrentTime;

			LC_ServerMap* pkServerMap = (LC_ServerMap*)m_pkMap;
			pkServerMap->BroadCastToArea(GetSectorIndex(), MGPT_RLT_NPC_EXECUTE_SKILL_TYPE_TARGET, &msg);
		}
		break;

	case SOT_DIR:
		{
			MG_Rlt_NPCExecuteSkillTypeDir msg;
			msg.m_lNPCID			= GetID();
			msg.m_lSkillExecuteID	= lSkillType;
			msg.m_lLockedTargetID	= GetLockedTargetID();
			msg.m_lCurrentLocationX = GetCurrentLocation().x;
			msg.m_lCurrentLocationY = GetCurrentLocation().y;
			msg.m_lLockedLocationX  = GetLockedLocation().x;
			msg.m_lLockedLocationY  = GetLockedLocation().y;
			msg.m_iForwardDirAngle	= (int)GetForwardDirAngle();
			GetRandNums(msg.m_afRandNums);
			msg.m_fServerTime		= fCurrentTime;

			LC_ServerMap* pkServerMap = (LC_ServerMap*)m_pkMap;
			pkServerMap->BroadCastToArea(GetSectorIndex(), MGPT_RLT_NPC_EXECUTE_SKILL_TYPE_DIR, &msg);
		}
		break;

	case SOT_LOCATION:
		{				
			MG_Rlt_NPCExecuteSkillTypeLocation msg;
			msg.m_lNPCID			= GetID();
			msg.m_lSkillExecuteID	= lSkillType;
			msg.m_iForwardDirAngle	= (int)GetForwardDirAngle();
			msg.m_lCurrentLocationX = GetCurrentLocation().x;
			msg.m_lCurrentLocationY = GetCurrentLocation().y;
			msg.m_lLockedLocationX	= GetLockedLocation().x;
			msg.m_lLockedLocationY	= GetLockedLocation().y;
			msg.m_fServerTime		= fCurrentTime;

			LC_ServerMap* pkServerMap = (LC_ServerMap*)m_pkMap;
			pkServerMap->BroadCastToArea(GetSectorIndex(), MGPT_RLT_NPC_EXECUTE_SKILL_TYPE_LOCATION, &msg);
		}
		break;

	default:
		break;
	}

	return iResult;
}
//---------------------------------------------------------------------------------------
ResultType LC_ServerNPC::InterruptSkill(float fCurrentTime,int lSkillTypeID,SkillInterruptType eInterruptType)
{
	ResultType eInterruptResult = LC_ActorBase::InterruptSkill(fCurrentTime,lSkillTypeID,eInterruptType);
	if(RE_SKILL_NONE_ERROR != eInterruptResult)
	{
		return eInterruptResult;
	}

	//通知客户端
//	bool bInterruptError = false;

	MG_Rlt_NPCInterruptSkill rltMsg;
	rltMsg.m_lRequesterID		= m_iID;
	rltMsg.m_lSkillExecuteID	= lSkillTypeID;
	rltMsg.m_usInterruptType	= eInterruptType;
	rltMsg.m_iResultType		= RE_SKILL_NONE_ERROR;

	//通知给周围的玩家
	SectorIndex dwSectorIndex = GetSectorIndex();
	LC_ServerMap* pkServerMap = (LC_ServerMap*)GetMap();
	pkServerMap->BroadCastToArea(dwSectorIndex, MGPT_RLT_NPC_INTERRUPT_SKILL, &rltMsg);

	return RE_SKILL_NONE_ERROR;
}
//----------------------------------------------------------
void LC_ServerNPC::SaveActiveInfo()
{
	//如果怪物已经被激活
	if (GetActive())
		return;

	if (GetCurrentLocation().x==0 &&  GetCurrentLocation().y==0)
	{
		return;
	}
	int lCurrentTime = (int)GET_PROCESS_TIME;
	SetActiveLocation(GetCurrentLocation());//激活地点
	SetActive(true);						//被激活
	SetActiveTime(lCurrentTime);			//激活时间	
}
//----------------------------------------------------------
void LC_ServerNPC::ResetActiveInfo()
{
	SetActive(false);
}

void LC_ServerNPC::SetUpdateFlag(bool bUpdateFlag)
{
	LC_LogicObject::SetUpdateFlag(bUpdateFlag);

	if (bUpdateFlag)
		OnEnableUpdateFlag();
	else
		OnDisableUpdateFlag();
}

int LC_ServerNPC::OnEnableUpdateFlag()
{
	LC_ServerNPCManager* pkNPCManager = SERVER_GET_NPC_MANAGER();

	pkNPCManager->OnEnableNPCUpdateFlag(this);
	SetLastTranslateTime(GET_PROCESS_TIME);

	return true;
}

int LC_ServerNPC::OnDisableUpdateFlag()
{
	LC_ServerNPCManager* pkNPCManager = SERVER_GET_NPC_MANAGER();
	pkNPCManager->OnDisableNPCUpdateFlag(this);
	return true;
}

//----------------------------------------------------------
void LC_ServerNPC::PunchMove(SkillMoveActorPositionType Type,const Utility::UT_Vec3Int & kTargetPoint,float fPunchTime)
{
	LC_NPCBase::PunchMove(Type, kTargetPoint, fPunchTime);

	const UT_Vec3Int & kCurrentPoint = this->GetCurrentLocation();
	SetTargetLocation(kTargetPoint);
	SetMoveLineTargetLocation(kTargetPoint);
	m_dataChangeFlag.GetBroadCastFlag().SetLocation();
	GetMovePath().Clear();

	////广播消息
	MG_MoveActorPosition	nMsg;
	nMsg.m_lActorID				= m_iID;
	nMsg.m_lMoveType			= Type;
	nMsg.m_lCurrentLocationX	= kCurrentPoint.x;
	nMsg.m_lCurrentLocationY	= kCurrentPoint.y;
	nMsg.m_lTargetLocationX		= kTargetPoint.x;
	nMsg.m_lTargetLocationY		= kTargetPoint.y;
	nMsg.m_fMoveTime			= fPunchTime;
	nMsg.m_fServerTime			= GET_PROCESS_TIME;

	if(m_pkMap)
	{
		((LC_ServerMap*)m_pkMap)->BroadCastToArea(GetSectorIndex(),MGPT_PUNCH_MOVE_NPC, &nMsg);
	}

	if ( Type == SMAPT_PASSIVEMOVE )
	{
		this->FixCurrentState(ST_NPC_PASSIVE_MOVE, GET_PROCESS_TIME, 0);
	}
	else if ( Type == SMAPT_PUNCHBACK)
	{
		this->FixCurrentState(ST_NPC_PUNCH_BACK, GET_PROCESS_TIME, 0);
	}
	else
	{
		this->FixCurrentState(ST_NPC_PASSIVE_MOVE, GET_PROCESS_TIME, 0);
	}
}
//------------------------------------------------------------------------
void LC_ServerNPC::ModifySMController( const StringType& rkControllerName )
{
	if (rkControllerName.empty())
	{	
		m_pkSMController = SM_ServerControllerFactory::GetSingletonPtr()->RequestController(SCT_SERVER_NPC);
		m_bControledByScript = false;
	}
	else
	{
		SM_ControllerBase* pkController = SM_ServerControllerFactory::GetSingletonPtr()->RequestController(rkControllerName);
		if (pkController)
		{
			m_pkSMController = pkController;
			m_bControledByScript = true;
		}
	}
}

//------------------------------------------------------------------------
void LC_ServerNPC::SetMoveSpeed(int fMoveSpeed)
{
	int oldSpeed = GetMoveSpeed();
	if (oldSpeed != fMoveSpeed)
	{
		m_dataChangeFlag.SetMoveSpeed();
	}
	LC_ActorBase::SetMoveSpeed(fMoveSpeed);
}
//------------------------------------------------------------------------
void LC_ServerNPC::SetWalkSpeedChange()
{
	m_dataChangeFlag.SetMoveSpeed();
}
//------------------------------------------------------------------------
void LC_ServerNPC::SetHP( attr_value_type  lHP )
{
	attr_value_type newHp = lHP;
	if(GetSaveDamageDataFlag() && lHP<=0)
	{
		CF_CharType::DataEntry* pkData = GetCharTypePtr();
		if(NULL == pkData)
		{
			GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "LC_ServerNPC::SetHP serverid:%d, monsterid:%d, lHP:%lld r1", GetGameServerID(), GetCharType(), lHP);
			return;
		}

		bool tag = false;
		LC_EncountedStatMgr& kStatMgr = GetEncountedStatMgr();
		LC_EncountedStat* pkStat = kStatMgr.GetStat(LC_ECT_TP_Damage);
		if(NULL == pkStat)
		{
			tag = true;
			newHp = pkData->_lMaxHP;
			GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "LC_ServerNPC::SetHP serverid:%d, monsterid:%d, lHP:%lld, newHp:%lld r2", GetGameServerID(), GetCharType(), lHP, newHp);
		}
		else
		{
			const LC_EncountedStatEntry* rkGuildEntry = pkStat->GetTopEntry(LC_ECT_CAT_GUILD);
			if (NULL == rkGuildEntry)
			{				
				//没有公会统计数据，怪物回满血，清除统计数据，所有玩家重新击杀
				tag = true;
				newHp = pkData->_lMaxHP;
				GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "LC_ServerNPC::SetHP serverid:%d, monsterid:%d, lHP:%lld, newHp:%lld r3", GetGameServerID(), GetCharType(), lHP, newHp);
			}
			else
			{
				GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "LC_ServerNPC::SetHP serverid:%d, monsterid:%d, lHP:%lld r4", GetGameServerID(), GetCharType(), lHP);
			}
		}

		if(true == tag)
		{
			GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "LC_ServerNPC::SetHP serverid:%d, monsterid:%d, lHP:%lld Reset kStatMgr r5", GetGameServerID(), GetCharType(), lHP);
			kStatMgr.Reset();
		}
	}

	attr_value_type oldHP = GetHP();
	LC_ActorBase::SetHP(newHp);
	if (oldHP != newHp)
	{
		m_dataChangeFlag.SetHPMP();

		CheckAddGuildMapBossReward(false, oldHP);
	}
}
//------------------------------------------------------------------------
void LC_ServerNPC::SetMP(attr_value_type  lMP)
{
	attr_value_type oldMP = GetMP();
	if (oldMP != lMP)
	{
		m_dataChangeFlag.SetHPMP();
	}
	LC_ActorBase::SetMP(lMP);
}
//------------------------------------------------------------------------
void LC_ServerNPC::SetFactionID(unsigned short usFactionID)
{
	if (GetFactionID() != usFactionID)
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "npcSetFactionID oldFactionID:%d, newFactionID:%d, mapResID:%d, mapLogicID:%d", GetFactionID(), usFactionID, GetMapResID(), GetMapLogicID());
		m_dataChangeFlag.GetBroadCastFlag().SetHPMP();
		LC_NPCBase::SetFactionID(usFactionID);
	}
}

bool LC_ServerNPC::SetEscortNpcState(int state)
{
	//受攻击状态->其他状态 有10sCD时间
	if(m_nEscortNpcState==EscortNPCState::EscortNPC_2 && state!=m_nEscortNpcState)
	{
		int time = GET_CURRENT_TIMESTAMP_IN_SECONDS();
		int lastTime = GetNoticeTime();
		if(0!=lastTime && time-lastTime < 10)
		{
			return false;
		}
	}

	if(m_nEscortNpcState != state)
	{
		m_nEscortNpcState = state;
		return true;
	}

	return false;
}
//------------------------------------------------------------------------
MC_DataDirtyFlag& LC_ServerNPC::GetMessageDirtyFlag()
{
	return m_dataChangeFlag;
}
//------------------------------------------------------------------------
void LC_ServerNPC::SetCurrentLocation( const Utility::UT_Vec3Int& rkLocation )
{
	const Utility::UT_Vec3Int& oldLocation = GetCurrentLocation();
	if (oldLocation.x != rkLocation.x ||
		oldLocation.y != rkLocation.y ||
		oldLocation.z != rkLocation.z)
	{
		int nDistance = UT_MathBase::LineLengthXYInt(rkLocation, oldLocation);
		//if ( nDistance > 190 && oldLocation.x != 1 )
		//{
		//	printf( "aSDFASDFSF\N" );
		//}
		m_dataChangeFlag.GetBroadCastFlag().SetLocation();
	}
	LC_ActorBase::SetCurrentLocation(rkLocation);
}
//----------------------------------------------------------------------------
void LC_ServerNPC::SetCurrentLocation( int iX,int iY,int iZ )
{
	const Utility::UT_Vec3Int& oldLocation = GetCurrentLocation();
	if (oldLocation.x != iX ||
		oldLocation.y != iY ||
		oldLocation.z != iZ )
	{
		m_dataChangeFlag.GetBroadCastFlag().SetLocation();
	}
	LC_ActorBase::SetCurrentLocation(iX,iY,iZ);
}
//----------------------------------------------------------------------------
int LC_ServerNPC::GetCurrentLocationWithZ( Utility::UT_Vec3Int& location )
{
	//Utility::UT_Vec3Int(m_kCurrentLocation.x, m_kCurrentLocation.y, m_kSpawnLocation.z);
	location = m_kCurrentLocation;
	location.z = m_kSpawnLocation.z;
	return true;
}
//----------------------------------------------------------------------------
void LC_ServerNPC::SetCurrentStateType( int iCurrentState )
{
	if (GetCurrentStateType() != iCurrentState)
	{
		LC_NPCBase::SetCurrentStateType(iCurrentState);
		m_dataChangeFlag.GetBroadCastFlag().SetStateType();
		// NPC移动优化
		//m_dataChangeFlag.SetLocation();
		m_dataChangeFlag.GetBroadCastFlag().SetLocation();
	}
}
//----------------------------------------------------------------------------
void LC_ServerNPC::UpdateDirtyFlag( int iFlag)
{
	LC_NPCBase::UpdateDirtyFlag(iFlag);
}
//----------------------------------------------------------------------------
void LC_ServerNPC::SetDirtyFlag( int iFlag,bool bFlag )
{
	LC_NPCBase::SetDirtyFlag(iFlag,bFlag);
}
//----------------------------------------------------------------------------
void LC_ServerNPC::ClearDirtyFlag()
{
	LC_NPCBase::ClearDirtyFlag();
}
//----------------------------------------------------------------------------
void LC_ServerNPC::_initSMController(const StringType& rkScriptName)
{
	SM_ControllerFactory* pkCtrlFactory = SM_ControllerFactory::GetSingletonPtr();

	if(!rkScriptName.empty() && rkScriptName != "xx")
	{
		SM_ControllerBase* AIController = pkCtrlFactory->RequestController(rkScriptName);
		if(AIController)
		{
			m_pkSMController = AIController;
			m_bControledByScript = true;
			return;
		}	
	}

	m_pkSMController = pkCtrlFactory->RequestController(SCT_SERVER_NPC);
	m_bControledByScript = false;
}
//-----------------------------------------------------------------------------------
void LC_ServerNPC::_initStateMap(const StringType& rkScriptName)
{
	SM_ServerStateFactory* pkStateFactory = (SM_ServerStateFactory* )SM_ServerStateFactory::GetSingletonPtr();

	_uninitStateMap();
	m_kStateMap[ST_DEFAULT]			= pkStateFactory->RequestState(ST_DEFAULT);

	bool bScriptAI = (!rkScriptName.empty() && rkScriptName != "xx");
	FilePrePostFixPair kPrePostFixPair = UT_BaseStringUtility::SplitBaseFileName(rkScriptName);
	TStringVector kFileNameTable = UT_BaseStringUtility::SplitString(kPrePostFixPair.first,'-');
	const StringType & kTableName = kFileNameTable[0];

	for(int i = ST_NPC_BORN;i < ST_NPC_STATE_END__;i++)
	{
		if (bScriptAI)
		{
			StringType strStateName = kTableName + "_AI_State."+ pkStateFactory->GetScriptStateName(i);
			SM_StateBase* pkState = ((SM_StateFactory*)pkStateFactory)->RequestState(strStateName);
			if (pkState)
			{
				m_kStateMap[i] = pkState;
				continue;
			}
		}

		m_kStateMap[i] = pkStateFactory->RequestState(i);
	}
}
//---------------------------------------------------------
void LC_ServerNPC::_uninitStateMap()
{
	for( int i = 0 ; i < m_stateCount; ++i)
	{
		m_kStateMap[i] = NULL;
	}
}

//---------------------------------------------------------
StateMachine::SM_StateBase* LC_ServerNPC::_findState(int iStateType)
{
	if(IS_SKILL_TYPE(iStateType))
	{
		Skill::SK_Factory* skf = Skill::SK_Factory::GetSingletonPtr();
		return skf->RequestSkillExcute(iStateType);		//SkillExecute也是状态的一种
	}
	else
	{
		return (0<=iStateType&&iStateType<m_stateCount) ? m_kStateMap[iStateType] : NULL;
	}
}

//-----------------------------------------------------------------------------------
void LC_ServerNPC::_updateOwnerByFollowTarget()
{
	m_kNPCOwnerList.ClearOwnerList();
	m_kNPCOwnerList.AddOwnerID(GetFollowedTargetID(), m_kNPCOwnerList.GetAddOwnerConsiderGroupMember());
}

object_id_type LC_ServerNPC::GetHitPlayerID()
{
	return m_kHitPlayerID;
}

void LC_ServerNPC::SetHitPlayer(object_id_type id)
{
	if(m_kHitPlayerID == id)
		return;

	m_kHitPlayerID = id;
	m_dataChangeFlag.GetBroadCastFlag().SetHPMP();

	if(0 == id)
	{
		SetHitTime(0);
	}
}

object_id_type LC_ServerNPC::GetKillPlayerID()
{
	return m_kKillPlayerID;
}

void LC_ServerNPC::SetKillPlayerID(object_id_type id)
{
	if(m_kKillPlayerID == id)
		return;

	m_kKillPlayerID = id;
}

uint32_t LC_ServerNPC::GetHitTime()
{
	return m_kHitTime;
}

void LC_ServerNPC::SetHitTime(uint32_t time)
{
	if(m_kHitTime == time)
		return;

	m_kHitTime = time;
}

void LC_ServerNPC::GetDamageList(object_id_set& id)
{
	m_kNPCOwnerList.GetDamageList(id);
}

void LC_ServerNPC::AddDamagePlayer(object_id_type id)
{
	m_kNPCOwnerList.AddDamageID(id);
}

void LC_ServerNPC::ConvertEncountToOwnerList()
{
	const LC_EncountedStatEntry* pkTop = m_kEncountedStatMgr.GetTopGroupDamageList(this, 10000);
	m_kNPCOwnerList.Clear();
	if (pkTop && pkTop->IsKeyValid())
	{
		m_kNPCOwnerList.AddOwnerList(pkTop->KeyIDs());
	}
}

object_id_type LC_ServerNPC::ConvertHitPlayerToOwnerList()
{
	if(0 == m_kHitPlayerID)
	{
		//GfxWriteFmtLog(LOG_WARNING_FILELINE, LOG_SWITCH_WARNING, "ConvertEncountToOwnerList empty!", GetCharType(), GetID());
		return m_kHitPlayerID;
	}

	object_id_set ownerIDs;
	LC_WorldManagerBase* pkWorldManager = ENGINE_GET_WORLD_MANAGER();
	LC_PlayerBase* pkHitPlayer = (LC_PlayerBase*)pkWorldManager->FindObject(m_kHitPlayerID);
	if (NULL == pkHitPlayer)
	{
		return m_kHitPlayerID;
	}

	ownerIDs.insert(m_kHitPlayerID);

	int validDis = 10000 * 10000;
	Utility::UT_Vec3Int kCenter = pkHitPlayer->GetCurrentLocation();
	LC_PlayerGroupBase* pkGroup = pkHitPlayer->GetPlayerGroup();
	if (pkGroup)
	{
		for (int i = 0; i < MAX_PLAYER_GROUP_MEMBER_LIST_SIZE; ++i)
		{
			LC_PlayerGroupEntry* pkEntry = pkGroup->GetGroupEntryInMemberList(i);
			if (pkEntry && pkEntry->GetValid())
			{
				LC_PlayerBase* pkPlayer = pkEntry->GetPlayer();
				if (pkPlayer)
				{
					//不加自己   不加不同地图的队友
					if (pkPlayer->GetID()==pkHitPlayer->GetID() || pkPlayer->GetMapResID() != pkHitPlayer->GetMapResID())
					{
						continue;
					}
					Utility::UT_Vec3Int dif = pkPlayer->GetCurrentLocation() - kCenter;
					if (dif.x * dif.x + dif.y * dif.y <= validDis)
					{
						ownerIDs.insert(pkPlayer->GetID());
					}
				}
			}
		}
	}

	m_kNPCOwnerList.AddOwnerList(ownerIDs);
	return m_kHitPlayerID;
}

//-----------------------------------------------------------------------------------
void LC_ServerNPC::AutoFitLevelReInit(int iLevel)
{
	AutoFitNPCManager * inst = AutoFitNPCManager::GetSingletonPtr();
	inst->AutoFitLevel(GetCharType(), iLevel);
	CF_AutoFitCharType::DataEntry* pkData = inst->FindData(GetCharType(),iLevel);

	if(!pkData)
	{
		return;
	}
	int prelevel=GetAutoFitLevel();
	CF_AutoFitCharType::DataEntry* pkpreData = inst->FindData(GetCharType(),prelevel);
	m_lLevel					= pkData->_lLevel;
	m_bCacheTargetLocation	= false;

	//注意：策划在怪物出生状态中可能修改怪物属性，这些属性应该保存，因此这里不能调用m_kAttributeMap.Clear()
	//m_kAttributeMap.Clear();
	m_kAttributeMap.SetAttributeBase(ATT_MAX_HP,		pkData->_lMaxHP);
	m_kAttributeMap.SetAttributeBase(ATT_MAX_MP,		pkData->_lMaxMP);
	m_kAttributeMap.SetAttributeBase(ATT_MAX_ATK,		pkData->_lMaxPHA);
	m_kAttributeMap.SetAttributeBase(ATT_DEF,			pkData->_lPHD);
	m_kAttributeMap.SetAttributeBase(ATT_DOG_RATE,		pkData->_lDogRATE);
	m_kAttributeMap.SetAttributeBase(ATT_CIR_RATE,		pkData->_lCirRATE);
	m_kAttributeMap.SetAttributeBase(ATT_CIR_DAM,		pkData->_lCirDAM);
	//m_kAttributeMap.SetAttributeBase(ATT_MOVE_SPEED,	pkData->_lMoveSPEED);
	//m_kAttributeMap.SetAttributeBase(ATT_WALK_SPEED,	pkData->_lWalkSpeed);
	//这里指的是比率的改变
	m_kAttributeMap.SetAttributeBase(ATT_MP_COST_RATE, 0);
	m_kAttributeMap.SetAttributeBase(ATT_HP_COST_RATE, 0);
	m_kAttributeMap.SetAttributeBase(ATT_MP_RECOVER_RATE, 0);
	m_kAttributeMap.SetAttributeBase(ATT_HP_RECOVER_RATE, 0);
	//这里指的是百分比*100（存成整数）
	m_kAttributeMap.SetAttributeBase(ATT_EXP_RATE, 10000);
	m_kAttributeMap.SetAttributeBase(ATT_CASH_RATE, 10000);

	m_kAttributeMap.SetAttributeBase(ATT_DMG_RATE, 0);
	m_kAttributeMap.SetAttributeBase(ATT_DMG_DERATE, 0);
	m_kAttributeMap.SetAttributeBase(ATT_RES_FREEZ,	pkData->_lResFreez);
	m_kAttributeMap.SetAttributeBase(ATT_RES_SLOW,	pkData->_lResSlow);
	m_kAttributeMap.SetAttributeBase(ATT_RES_SILENCE,	pkData->_lResSilence);
	m_kAttributeMap.SetAttributeBase(ATT_RES_DIZZ,	pkData->_lResDizz);
	m_kAttributeMap.SetAttributeBase(ATT_RES_SLEEP,	pkData->_lResSleep);

	m_kAttributeMap.SetAttributeBase(ATT_ATK_RATE,		pkData->_lATKRateAdd);
	m_kAttributeMap.SetAttributeBase(ATT_CIR_DEF,		pkData->_lCirDefAdd);

	m_kAttributeMap.RecomputeAll();

	m_resists[0] = pkData->_lNPCRes1;
	m_resists[1] = pkData->_lNPCRes2;
	m_resists[2] = pkData->_lNPCRes3;
	m_resists[3] = pkData->_lNPCRes4;
	m_resists[4] = pkData->_lNPCRes5;
	m_resists[5] = pkData->_lNPCRes6;
	m_resists[6] = pkData->_lNPCRes7;
	m_resists[7] = pkData->_lNPCRes8;
	m_resists[8] = pkData->_lNPCRes9;
	m_resists[9] = pkData->_lNPCRes10;
	m_resists[10] = pkData->_lNPCRes11;
	m_resists[11] = pkData->_lNPCRes12;
	m_resists[12] = pkData->_lNPCRes13;
	m_resists[13] = pkData->_lNPCRes14;
	m_resists[14] = pkData->_lNPCRes15;

	//重新计算HP、mp
	if (pkpreData==NULL)
	{
		SetHP(pkData->_lMaxHP);
		SetMP(pkData->_lMaxMP);
	}
	else
	{
		int64_t hp=(int64_t)((double)GetHP()*((double)pkData->_lMaxHP/(double)pkpreData->_lMaxHP));
		if (hp<1000)
		{
			GfxWriteLog(LOG_ITEM_FILELINE,LOG_SWITCH_ITEM,"AutoFitLevelReInit :hp:%d，GetHP：%d，pkData->_lMaxHP：%ld,pkpreData->_lMaxHP:%ld",hp,GetHP(),pkData->_lMaxHP,pkpreData->_lMaxHP);
		}
		SetHP(hp);
		int mp=(int)((double)GetMP()*((double)pkData->_lMaxMP/(double)pkpreData->_lMaxMP));
		SetMP(mp);
	}
	SetAutoFitLevel(iLevel);
}

void LC_ServerNPC::SetSkillFlag(int64_t llFlag,bool bFlag)
{
	if (GetSkillFlag(llFlag) != bFlag)
	{
		m_dataChangeFlag.GetBroadCastFlag().SetHPMP();
		LC_NPCBase::SetSkillFlag(llFlag, bFlag);
	}
}

void LC_ServerNPC::SetAllSkillFlag(int64_t llSkillFlag)
{
	if (GetAllSkillFlag() != llSkillFlag)
	{
		m_dataChangeFlag.GetBroadCastFlag().SetHPMP();
		LC_NPCBase::SetAllSkillFlag(llSkillFlag);
	}
}

void LC_ServerNPC::SetInteractionValue( int lValue )
{
	int oldValue = GetInteractionValue();
	if (oldValue != lValue)
	{
		m_dataChangeFlag.SetInteractionValue();
		LC_NPCBase::SetInterationValue(lValue);
	}
}

void LC_ServerNPC::SetSummorID(object_id_type lID)
{
	m_lSummorID = lID;
}

void LC_ServerNPC::SetSummonOwnerID(object_id_type lID)
{
	m_dataChangeFlag.SetHPMP();
	LC_NPCBase::SetSummonOwnerID(lID);
}
void LC_ServerNPC::SetLevel(int lLevel, bool bInit)
{
	// 1 set value
	LC_ActorBase::SetLevel(lLevel);
	// 2 SetAutoFitLevel
	//SetAutoFitLevel(lLevel);
	// 3 broadcast
	return;
	/*
	if ( bInit )
		return;
	LC_ServerMap* pkServerMap = (LC_ServerMap*)m_pkMap;
	MG_RefreshNPCLevel msg;
	msg.m_nObjId = GetID();
	msg.m_nLevel = GetLevel();
	pkServerMap->BroadCastToArea(GetSectorIndex(), MGPT_REFRESH_NPC_LEVEL, &msg);
	*/
}
//-----------------------------------------------------
void LC_ServerNPC::SetFixedOwnerListFlag(bool bFlag)
{
	m_kNPCOwnerList.SetAvoidUpdateFlag(bFlag);
}

void LC_ServerNPC::SetShapeShiftInfo(int lNewShapeShiftTypeID)
{
	int lOldShapeShiftTypeID = GetShapeShiftCharTypeID();

	if (lNewShapeShiftTypeID == lOldShapeShiftTypeID)
		return;

	bool bNewShift = false;
	if (lNewShapeShiftTypeID > 0 && lNewShapeShiftTypeID != GetCharType())
		bNewShift = true;
	else
		lNewShapeShiftTypeID = 0;

	SetShapeShiftFlag(bNewShift);
	SetShapeShiftCharTypeID(lNewShapeShiftTypeID);
	
	m_dataChangeFlag.GetBroadCastFlag().SetHPMP();
}

void LC_ServerNPC::BeginRetreatAccumulate()
{
	m_kRetreatTimeAccumulate = 0;
}

void LC_ServerNPC::StepRetreatAccumulate(float fDeltaTime)
{
	m_kRetreatTimeAccumulate += fDeltaTime;
}

void LC_ServerNPC::EndRetreatAccumulate()
{
	m_kRetreatTimeAccumulate = 0;
}
//------------------------------------------------------------------------------------------
void LC_ServerNPC::SetRiderCharType(int lCharType)
{
	LC_NPCBase::SetRiderCharType(lCharType);
	m_dataChangeFlag.SetHPMP();
}

void LC_ServerNPC::CheckAddGuildMapBossReward(bool force, int64_t oldHp)
{
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerMapLogic_Place* pkPlace = pkWorldManager->GetMapLogic(GetMapLogicID());
	if (pkPlace == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "CheckAddGuildMapBossReward-1 mapResID:%d, mapLogicID:%d, bossID:%d, force:%d", GetMapResID(), GetMapLogicID(), GetCharType(), force);
		return;
	}

	int64_t totalHP = GetCharTypePtr()->_lMaxHP;
	int64_t currentHP = GetHP() < 0 ? 0 : GetHP();
	int bossCharType = GetCharType();
	int mapID = pkPlace->GetMapResID();
	uint64_t guildID = pkPlace->GetGuildID();

	if(force)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "CheckAddGuildMapBossReward-force mapID:%d, bossID:%d, force:%d\n", GetMapResID(), GetCharType(), force);
	}

	if(pkPlace->GetMapLogicType()!=MT_GUILD_MAP || 0==guildID)
	{
		if(pkPlace->GetMapLogicType()==MT_GUILD_MAP)
		{
			GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "CheckAddGuildMapBossReward-2 mapID:%d, bossID:%d, totalHP:%lld, oldDeltaHP:%lld, newDeltaHp:%lld, force:%d, guildID:%d\n", mapID, bossCharType, totalHP, totalHP-oldHp, totalHP-currentHP, force, guildID);
		}
		return;
	}

	mem::vector<int> rewardIndex;
	ResultType res = UT_ServerHelper::CheckAddGuildMapBossReward(mapID, bossCharType, oldHp, currentHP, guildID, rewardIndex);
	if(RE_GUILDMAP_ERROR_17!=res && RE_SUCCESS!=res)
	{
		return;
	}

	bool updateHP = force;
	if(RE_SUCCESS == res)
	{
		UT_ServerHelper::SendGuildMapBossReward(mapID, guildID, rewardIndex);
		updateHP = true;
	}

	//通知社交服修改剩余血量
	int32_t currentTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	if(updateHP || (0==m_nUpdateHpTime||currentTime>=m_nUpdateHpTime) )
	{
		MG_GG_ModifyGuildBossHp msg;
		msg.m_nMapID = mapID;
		msg.m_nGuildID = guildID;
		msg.m_nBossID = bossCharType;
		msg.m_nHP = currentHP;
		SendMsgToSociety(MGPT_GG_MODIFYGUILDBOSSHP, &msg);

		if(force)
		{
			GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "CheckAddGuildMapBossReward-4 SyncHpToSociety mapID:%d, bossID:%d, totalHP:%lld, newDeltaHp:%lld, force:%d, guildID:%d\n", mapID, msg.m_nBossID, totalHP, totalHP-currentHP, force, guildID);
		}
		
		//5秒更新一次血量
		m_nUpdateHpTime = currentTime + 5;
	}
}

//*****	ChaseData relevant functions BEGIN
void LC_ServerNPC::ResetChaseData()
{
	ResetChaseTime();
	SetChaseMoveType(NOTHING);
	SetChaseInnerRadius(0);
	SetChaseOuterRadius(0);
	SetIsSurrounder(true);
	SetSwingAngle(0);
	SetSwingMaxAngle(0);
	SetSwingCurrentAngle(0);
	SetHPRecoverRate(0);
	SetHPRecoverTimer(0.00f);
	ResetHPRecoverTimer();
}

bool LC_ServerNPC::CheckChaseValid(float currTime)
{
	if (currTime < m_kChaseData.startTime + m_kChaseData.moveTime)
		return false;

	ResetChaseTime(currTime);
	return true;
}

void LC_ServerNPC::ResetChaseTime(float currTime /*= 0*/, float moveTime /*= 0*/)
{
	m_kChaseData.startTime = currTime;
	SetChaseMoveTime(moveTime);
}

void LC_ServerNPC::SetChaseMoveTime(float moveTime)
{
	m_kChaseData.moveTime = moveTime;
}

void LC_ServerNPC::SetChaseMoveType(ChaseType moveType)
{
	m_kChaseData.moveType = moveType;
}

void LC_ServerNPC::SetChaseInnerRadius(int distance)
{
	m_kChaseData.innerRadius = distance;
}

void LC_ServerNPC::SetChaseOuterRadius(int distance)
{
	m_kChaseData.outerRadius = distance;
}

void LC_ServerNPC::SetIsSurrounder(bool isSurrounder)
{
	m_kChaseData.isSurrounder = isSurrounder;
}

void LC_ServerNPC::SetSwingAngle(float swingAngle)
{
	m_kChaseData.swingAngle = swingAngle;
}

void LC_ServerNPC::SetSwingMaxAngle(float swingMaxAngle)
{
	m_kChaseData.swingMaxAngle = swingMaxAngle;
}

void LC_ServerNPC::SetSwingCurrentAngle(float swingCurrentAngle)
{
	m_kChaseData.swingCurrentAngle = swingCurrentAngle;
}

void LC_ServerNPC::SetSleepProbability(float probability)
{
	m_kChaseData.fSleepProbability = probability;
}

void LC_ServerNPC::SetSleepTimeMin(float minTime)
{
	m_kChaseData.fSleepTimeMin = minTime;
}

void LC_ServerNPC::SetSleepTimeMax(float maxTime)
{
	m_kChaseData.fSleepTimeMax = maxTime;
}

void LC_ServerNPC::SetHPRecoverTimer(float fInput)
{
	m_kChaseData.fHPRecoverTimer = fInput;
}

float LC_ServerNPC::GetHPRecoverTimer()
{
	return m_kChaseData.fHPRecoverTimer;
}

void LC_ServerNPC::SetHPRecoverRate(int nInput)
{
	m_kChaseData.nHPRecoverRate = nInput;
}

int LC_ServerNPC::GetHPRecoverRate()
{
	return m_kChaseData.nHPRecoverRate;
}

int	 LC_ServerNPC::UpdateHPRecoverTimer(float fInput)
{
	int nResult = false;
	m_kChaseData.fHPRecoverTimer_RT += fInput;
	if ( m_kChaseData.fHPRecoverTimer_RT >= m_kChaseData.fHPRecoverTimer )
	{
		m_kChaseData.fHPRecoverTimer_RT -= m_kChaseData.fHPRecoverTimer;
		nResult = true;
	}
	return nResult;
}

void LC_ServerNPC::SetAutoRecover(bool bFlag)
{
	m_kChaseData.bAutoRecover = bFlag;
}

bool LC_ServerNPC::GetAutoRecover()
{
	return m_kChaseData.bAutoRecover;
}

void LC_ServerNPC::UpdateForAutoRecover(float fDelta)
{
	if (m_kChaseData.bAutoRecover == false)
		return;

	if ( GetHatredList()->GetHatredCount() > 0 )
	{
		SetAutoRecover(false);
		return;
	}

	if (UpdateHPRecoverTimer(fDelta))
	{
		attr_value_type nMaxHP = GetAttributeMap()->GetAttribute(ATT_MAX_HP);
		float fRate = (float)(GetHPRecoverRate()) / (float)100;
		attr_value_type nRecoverHP = nMaxHP * fRate;
		nRecoverHP += GetHP();
		SetHP(nRecoverHP);
		if (nRecoverHP >= nMaxHP )
			SetAutoRecover(false);
	}
}

void LC_ServerNPC::ResetHPRecoverTimer()
{
	m_kChaseData.fHPRecoverTimer_RT = 0.00f;
}

//*****	ChaseData relevant functions END