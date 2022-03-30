#include "LM_LocaleManager.h"
#include "GameServerApp.h"
#include "SC_ServerSkillInterface.h"
#include "SC_ServerScriptInterface.h"
#include "LC_ServerPlayer.h"
#include "UT_GameServerLogManager.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerTaskManager.h"
#include "SK_ServerSkillEventManager.h"
#include "LC_GameStory_Manager.h"
#include "LC_ServerMap.h"
#include "LC_ServerNPC.h"
#include "LC_ServerNPCManager.h"
#include "LC_ServerSector.h"
#include "PF_ServerPathManager.h"
#include "LC_ServerGuildManager.h"
#include "CF_LevelUp.h"
#include "CF_SkillCDModifyFunc.h"
#include "CF_WorldMapList.h"
#include "CF_GameStoryMessage.h"
#include "UT_PathFindHelp.h"
#include "LC_ServerMapLogic_WarPlay.h"
#include "LC_ServerMapLogic_Place.h"
#include "UT_Timer.h"
#include <boost/algorithm/string/replace.hpp>
#include "LC_NPCSummorManager.h"
#include "LC_GameStoryDataDefine.h"
#include "LC_SequenceManager.h"
#include "../Utility/oss_define.h"
#include "SK_Factory.h"
#include "SK_SP.h"
#include "UT_Log.h"
#include "LC_PackEntryFactory.h"
#include "MG_GM2Login.h"
#include "MG_TaskProtocol.h"
#include "MG_Common.h"
#include "LC_GameEventManager.h"
#include "CF_Faction.h"
#include "CF_SystemError.h"
#include "CF_ElementDamage.h"
#include "CF_YaZhiLevel.h"
#include "CF_CommanderTitle.h"
#include "LC_CommandParserBase.h"
#include "LC_RankManager.h"
#include "LC_VIPAsset.h"
#include "LC_ServerAllianceManager.h"
#include "LC_ShortcutBar.h"
#include "ACT_ActivityDataManager.h"
#include "ACT_SnowMan.h"
#include "LC_StoryLogic_Manager.h"
#include "LC_GuildLadderManager.h"
#include "UT_Pick_Manager.h"
#include "GlobalSettings.h"
#include "SC_ServerDebugInterface.h"
#include "../Buddy/BuddyInstance.h"
#include "oss_360_define.h"
#include "MG_VehicleProtocol.h"
#include "LC_Helper.h"
#include "CF_BuddyTransform.h"
#include "UT_OperateLog.h"
#include "CF_Charge.h"
#include "LC_ServerPlayerRecoverTimes.h"

using namespace OPERATELOG;
using namespace Script;
using namespace GameLogic;
using namespace Utility;
using namespace Skill;
using namespace CSVFile;
using namespace StateMachine;
using namespace Protocol;
using namespace GameStory;

// 司令官压制数据快照
static bool gs_bCommanderLevelFuck = false;
//typedef mem::hash_map<int, float> CommanderLevelFuckType;
typedef mem::vector<float> CommanderLevelFuckType;
static CommanderLevelFuckType gs_CommanderLevelFuckData;

static void CommanderLoadLevelFuckData()
{
	if (gs_bCommanderLevelFuck)
		return;

	CF_CommanderTitle::DataEntryMapExternIterator kIt = CF_CommanderTitle::GetSingletonPtr()->GetIterator();
	for(; kIt.HasMoreElements(); kIt.MoveNext())
	{
		CF_CommanderTitle::DataEntry* pkData = kIt.PeekNextValuePtr();
		//gs_CommanderLevelFuckData[pkData->_lID] = 1 + pkData->_fDamageRate;
		gs_CommanderLevelFuckData.push_back(1 + pkData->_fDamageRate);
	}

	gs_bCommanderLevelFuck = true;
}

static float FetchCommanderLevelFuckData(int nSrc, int nDst)
{
	if ( nSrc <= nDst )
		return 1.00f;
	if ( nSrc <= 0 || nDst <= 0 )
		return 1.00f;
	if ( nSrc  > gs_CommanderLevelFuckData.size() )
		return 1.00f;
	return gs_CommanderLevelFuckData[nSrc - 1];
	//CommanderLevelFuckType::iterator it = gs_CommanderLevelFuckData.find( nSrc );
	//if ( it == gs_CommanderLevelFuckData.end() )
	//	return 1.00f;
	//return it->second;
}

static float FetchCommanderLevelFuckData( LC_ActorBase* pSrc, LC_ActorBase* pDst )
{
	CommanderLoadLevelFuckData();
	int nSrcTitle = -1;
	int nDstTitle = -1;
	if ( pSrc->GetType() == LOT_PLAYER )
		nSrcTitle = ((LC_ServerPlayer*)pSrc)->GetCommanderTitle();

	if ( pDst->GetType() == LOT_PLAYER )
		nDstTitle = ((LC_ServerPlayer*)pDst)->GetCommanderTitle();

	return FetchCommanderLevelFuckData( nSrcTitle, nDstTitle );
}

struct YaZhiInfo
{
	float m_pvpYaZhi;
	float m_pveYaZhi;
};

// 等级压制数据快照
static bool gs_bLevelFuck = false;
typedef mem::vector<YaZhiInfo> LevelFuckType;
static LevelFuckType gs_LevelFuckData;
static int gs_nMinDeltaLevel = 0;
static int gs_nMaxDeltaLevel = 0;

static void LoadLevelFuckData()
{
	if (gs_bLevelFuck)
		return;

	CF_YaZhiLevel* pConfig = CF_YaZhiLevel::GetSingletonPtr();
	mem::hash_map<int, YaZhiInfo> mapOrigData;

	CF_YaZhiLevel::DataEntryMapExternIterator kIt = CF_YaZhiLevel::GetSingletonPtr()->GetIterator();
	for(; kIt.HasMoreElements(); kIt.MoveNext())
	{
		CF_YaZhiLevel::DataEntry* pkData = kIt.PeekNextValuePtr();
		if (pkData->_iLvmin < gs_nMinDeltaLevel )
			gs_nMinDeltaLevel = pkData->_iLvmin;
		if ( pkData->_iLvmax > gs_nMaxDeltaLevel )
			gs_nMaxDeltaLevel = pkData->_iLvmax;

		for ( int iLv = pkData->_iLvmin; iLv <= pkData->_iLvmax; iLv++  )
		{
			YaZhiInfo data;
			data.m_pvpYaZhi = pkData->_fYaZhi_PVP;
			data.m_pveYaZhi = pkData->_fYaZhi_PVE;
			mapOrigData[iLv] = data;
		}
	}
	int nSize = gs_nMaxDeltaLevel - gs_nMinDeltaLevel + 1;

	YaZhiInfo info;
	gs_LevelFuckData.resize(nSize, info);

	mem::hash_map<int, YaZhiInfo>::iterator itOrig = mapOrigData.begin();
	for ( ;itOrig != mapOrigData.end(); ++itOrig )
	{
		gs_LevelFuckData[itOrig->first - gs_nMinDeltaLevel] = itOrig->second;
	}

	gs_bLevelFuck = true;
}

static float FetchLevelFuckData(int nDeltaValue, int type)
{
	if ( nDeltaValue < gs_nMinDeltaLevel )
		nDeltaValue = gs_nMinDeltaLevel;
	if ( nDeltaValue > gs_nMaxDeltaLevel )
		nDeltaValue = gs_nMaxDeltaLevel;
	int32_t nIndex = nDeltaValue - gs_nMinDeltaLevel;

	switch(type)
	{
	case YaZhiType_PVP:
		return gs_LevelFuckData[ nIndex ].m_pvpYaZhi;
	case YaZhiType_PVE:
		return gs_LevelFuckData[ nIndex ].m_pveYaZhi;
	}

	return 1.0;
}

static int GetYaZhiType(LC_ActorBase* pSrc, LC_ActorBase* pDst)
{
	int yaZhiType = YaZhiType_None;
	if(IS_PLAYER_CLASS_ID(pSrc->GetID()) && IS_PLAYER_CLASS_ID(pDst->GetID()))
	{
		yaZhiType = YaZhiType_PVP;
	}
	else if(IS_PLAYER_CLASS_ID(pSrc->GetID()) || IS_PLAYER_CLASS_ID(pDst->GetID()))
	{
		yaZhiType = YaZhiType_PVE;
	}

	return yaZhiType;
}

static float FetchLevelFuckData( LC_ActorBase* pSrc, LC_ActorBase* pDst )
{
	LoadLevelFuckData();
	int nSrcLevel = 0;
	int nDstLevel = 0;
	if ( pSrc->GetType() == LOT_PLAYER )
		nSrcLevel = ((LC_ServerPlayer*)pSrc)->GetBuddyLevel();
	else 
		nSrcLevel = pSrc->GetLevel();
	if ( pDst->GetType() == LOT_PLAYER )
		nDstLevel = ((LC_ServerPlayer*)pDst)->GetBuddyLevel();
	else 
		nDstLevel = pDst->GetLevel();

	int yaZhiType = GetYaZhiType(pSrc, pDst);
	float fResult = FetchLevelFuckData(nSrcLevel - nDstLevel, yaZhiType);
	if (pDst->GetType() == LOT_PLAYER && pSrc->GetType() == LOT_PLAYER)
	{
		// 尧辉: PVP最终伤害* 0.3
		fResult *= GetGlobalSetting.fPVPDamageRate;
	}
	return fResult;
}


// 元素伤害数据快照
static bool gs_bElementDamageType = false;
static float gs_fElementDamageType[eDamageElementType_Count][eDamageElementType_Count];

static void LoadElementDamageData()
{
	if (gs_bElementDamageType == true)
		return;

	// Load
	CF_ElementDamage* pConfig = CF_ElementDamage::GetSingletonPtr();

	for ( int i = 0; i < eDamageElementType_Count; i++ )
	{
		CF_ElementDamage::DataEntry* pData = pConfig->GetEntryPtr(i);
		gs_fElementDamageType[i][eDamageElementType_0] = pData->_fElement0;
		gs_fElementDamageType[i][eDamageElementType_1] = pData->_fElement1;
		gs_fElementDamageType[i][eDamageElementType_2] = pData->_fElement2;
		gs_fElementDamageType[i][eDamageElementType_3] = pData->_fElement3;
		gs_fElementDamageType[i][eDamageElementType_4] = pData->_fElement4;
	}

	gs_bElementDamageType = true;
}

static float FetchElementDamageData( DamageElementType eSrc, DamageElementType eDst )
{
	LoadElementDamageData();
	return gs_fElementDamageType[eSrc][eDst];
}

static float GetCombatYaZhiValue(LC_ActorBase* pSrc, LC_ActorBase* pDst)
{
	int yaZhiType = GetYaZhiType(pSrc, pDst);
	int64_t srcCombatScore = pSrc->GetYaZhiCombatScore();
	int64_t destCombatScore = pDst->GetYaZhiCombatScore();
	float value = SC_ServerScriptInterface::GetSingletonPtr()->GetYaZhiManager()->FetchCombatYaZhiData(srcCombatScore, destCombatScore, yaZhiType);
	return value;
}

//---------------------------------------------------
struct PlayerYuanbao
{
	StringType				order_id;
	unique_id_type			uid;
	unique_id_type			role_id;
	StringType				role_name;
	int						skey;
	int						coins;
};
AJSON(PlayerYuanbao, (order_id)(uid)(role_id)(role_name)(skey)(coins));
//---------------------------------------------------
SC_ServerSkillInterface::SC_ServerSkillInterface()
	: SC_SkillInterface()
{
}
//---------------------------------------------------
SC_ServerSkillInterface::~SC_ServerSkillInterface()
{
}
//---------------------------------------------------
unique_id_type SC_ServerSkillInterface::ToNowID(unique_id_type sid)
{
	static platform_id_type _st_pid = (platform_id_type)get_platform_tp_by_union_id(GetGameUnionID());
	return NOW_UNIQUE_ID(_st_pid, sid);
}
//---------------------------------------------------
bool SC_ServerSkillInterface::IsInCross(GameLogic::LC_Attacker* pkScriptCaller)
{
	if ( pkScriptCaller != NULL && IS_PLAYER_ID(pkScriptCaller->GetID()) )
	{
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkScriptCaller;
		return UT_ServerHelper::IsInCross(pkPlayer);
	}
	return false;
}
//---------------------------------------------------
object_id_type SC_ServerSkillInterface::GetLCLockedID(GameLogic::LC_Attacker* pkScriptCaller)
{
	if ( pkScriptCaller && IS_PLAYER_CLASS_ID(pkScriptCaller->GetID()) )
	{
		LC_ServerPlayer* sd = (LC_ServerPlayer*)pkScriptCaller;
		return sd->GetLCLockedID();
	}
	return 0;
}
//---------------------------------------------------
void SC_ServerSkillInterface::FaceToTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	LC_ActorBase* pkActor = (LC_ActorBase*)pkScriptCaller;
	bool bLockMouse = pkActor->GetLockMouseLocation();
	pkActor->SetLockMouseLocation(false);
	static bool bDebug = true;
	if ( bLockMouse && bDebug )
	{
		if ( pkActor->IsCurrentActivateBuddy() )
			return;
		//pkActor->IsCurrentActivateBuddy();
	}

	UT_Vec3Int kCurrentLocation = pkActor->GetCurrentLocation();
	UT_Vec3Int kTargetLocation;

	if (iTargetIndex == TARGET_LOCATION)
	{
		kTargetLocation = pkActor->GetLockedLocation();
	}
	else
	{
		LC_ActorPtrList kTargetsList;
		_findTarget((LC_Attacker*)pkActor, iTargetIndex, kTargetsList);
		if (kTargetsList.empty())
		{
			return;
		}
		LC_ActorBase* pkTarget = kTargetsList.front();
		if (!pkTarget)
		{
			return;
		}
		kTargetLocation = pkTarget->GetCurrentLocation();
	}
	LC_Helper::FaceActorToLocation(pkActor, kCurrentLocation, kTargetLocation);
	float fAngle = pkActor->GetForwardDirAngle();
	pkActor->SetForwardDir(pkActor->GetTargetForwardDir());
}
//---------------------------------------------------
void SC_ServerSkillInterface::BackToTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}
	LC_ActorBase* pkActor = (LC_ActorBase*)pkScriptCaller;
	UT_Vec3Int kCurrentLocation = pkActor->GetCurrentLocation();
	UT_Vec3Int kTargetLocation;

	if (iTargetIndex == TARGET_LOCATION)
	{
		kTargetLocation = pkActor->GetLockedLocation();
	}
	else
	{
		LC_ActorPtrList kTargetsList;
		_findTarget((LC_Attacker*)pkActor, iTargetIndex, kTargetsList);
		if (kTargetsList.empty())
		{
			return;
		}
		LC_ActorBase* pkTarget = kTargetsList.front();
		if (!pkTarget)
		{
			return;
		}
		kTargetLocation = pkTarget->GetCurrentLocation();
	}
	LC_Helper::BackActorToLocation(pkActor, kCurrentLocation, kTargetLocation);
	pkActor->SetForwardDir(pkActor->GetTargetForwardDir());
}
//------------------------------------------------------------
void SC_ServerSkillInterface::SetTargetLocation(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	object_id_type lCallerID = pkScriptCaller->GetID();
	if (!IS_PLAYER_CLASS_ID(lCallerID) && !IS_NPC_ID(lCallerID))
	{
		return;
	}

	LC_ActorBase* pkActor = (LC_ActorBase*)pkScriptCaller;

	UT_Vec3Int kTargetLocation;
	if (iTargetIndex == TARGET_LOCATION)
	{
		kTargetLocation = pkActor->GetLockedLocation();
	}
	else
	{
		LC_ActorPtrList kTargetsList;
		_findTarget(pkActor, iTargetIndex, kTargetsList);
		if (kTargetsList.empty())
		{
			return;
		}
		LC_LogicObject* pkTarget = kTargetsList.front();
		if (!pkTarget)
		{
			return;
		}
		//如果锁定自身
		if (pkTarget->GetID() == lCallerID)
		{
			return;
		}
		kTargetLocation = pkTarget->GetCurrentLocation();
	}
	pkActor->SetTargetLocation(kTargetLocation);
}

//------------------------------------------------------------
void SC_ServerSkillInterface::SetLockedLocationByDir(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex,int iFaceDistance)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	object_id_type lCallerID = pkScriptCaller->GetID();
	if (!IS_PLAYER_CLASS_ID(lCallerID) && !IS_NPC_ID(lCallerID))
	{
		return;
	}

	LC_ActorBase* pkCaller = (LC_ActorBase*)pkScriptCaller;

	UT_Vec3Int kTargetLocation;
	if (TARGET_LOCATION == iTargetIndex)
	{
		kTargetLocation = pkScriptCaller->GetLockedLocation();
	}
	else
	{
		LC_ActorPtrList kTargetList;
		_findTarget(pkScriptCaller, iTargetIndex, kTargetList);
		if (kTargetList.size() != 1)
		{
			return;
		}
		LC_ActorBase* pkTarget = kTargetList.front();
		if(!pkTarget)
		{
			return;
		}
		kTargetLocation = pkTarget->GetCurrentLocation();
	}

	UT_Vec3Int kCurrentLocation = pkCaller->GetCurrentLocation();

	UT_Vec2Float tarDir;
	tarDir.x = (float)kTargetLocation.x - (float)kCurrentLocation.x;
	tarDir.y = (float)kTargetLocation.y - (float)kCurrentLocation.y;
	tarDir.Unitize();

	UT_Vec3Int finTargetLocation;
	finTargetLocation.x = kCurrentLocation.x + (int)(tarDir.x * iFaceDistance);
	finTargetLocation.y = kCurrentLocation.y + (int)(tarDir.y * iFaceDistance);

	PF_ServerPathManager* pfm = SafeGetSingleton<PF_ServerPathManager>();
	if (pfm)
	{
		int MapResID = pkScriptCaller->GetMapResID();
		UT_Vec3Int fvec;
		bool ret = pfm->FindDirectFarawayPoint(MapResID, pkScriptCaller->GetCurrentLocation(), finTargetLocation, UT_MathBase::LineLengthXYInt(finTargetLocation, pkScriptCaller->GetCurrentLocation()), fvec);
		if (ret)
		{
			finTargetLocation = fvec;
		}
	}

	pkCaller->SetLockedLocation(finTargetLocation);
}
//------------------------------------------------------------
void SC_ServerSkillInterface::SetLockedLocationByMouseLocation(GameLogic::LC_Attacker* pkScriptCaller, bool bLockedTarget)
{
	if (pkScriptCaller == NULL)
	{
		return;
	}

	LC_ActorBase* pActor = static_cast<LC_ActorBase*>(pkScriptCaller);
	LC_ActorBase* pkTarget = LC_Helper::GetLockedTarget(pActor);

	// simulate client logic:
	//UT_Vec3Int kMouseLocation = pkTarget && bLockedTarget ? pkTarget->GetVisibleLocation() : UT_EngineMath::ConvertNiPoint3ToVec3Int(pkGamePlayer->GetMouseOverLocation());
	if (bLockedTarget && pkTarget)
	{
			pActor->SetLockedLocation(pkTarget->GetCurrentLocation());
	}
	else
	{
		pActor->SetLockedLocation(pActor->GetMouseLocation());
	}

	pActor->SetLockMouseLocation(true);
}

void SC_ServerSkillInterface::RegisterCombatEventData(GameLogic::LC_Attacker* pkScriptCaller,  int eEventType, int nKey, int nValue )
{
	//server impl
	LC_ActorBase* pkActor = (LC_ActorBase*)pkScriptCaller;
	pkActor->RegisterCombatEventData((COMBAT_EVENT_TYPE)eEventType, nKey, nValue);
}

void SC_ServerSkillInterface::UnregisterCombatEventData(GameLogic::LC_Attacker* pkScriptCaller,  int eEventType, int nKey )
{
	// server impl
	LC_ActorBase* pkActor = (LC_ActorBase*)pkScriptCaller;
	pkActor->UnregisterCombatEventData((COMBAT_EVENT_TYPE)eEventType, nKey);
}

void SC_ServerSkillInterface::RegisterCombatEventPassiveCrit(GameLogic::LC_Attacker* pkScriptCaller,  GameLogic::LC_Attacker* pkAttacker, int nValue )
{
	if ( pkScriptCaller == NULL || pkAttacker == NULL )
		return;

	LC_ActorBase* pkSrcActor = (LC_ActorBase*)pkAttacker;
	LC_ActorBase* pkDstActor = (LC_ActorBase*)pkScriptCaller;

	pkDstActor->RegisterCombatEventData(eCombatEvent_Passive_Crit, pkSrcActor->GetOwnerObjectID(), nValue);
}

void SC_ServerSkillInterface::UnregisterCombatEventPassiveCrit(GameLogic::LC_Attacker* pkScriptCaller,  GameLogic::LC_Attacker* pkAttacker )
{
	if ( pkScriptCaller == NULL || pkAttacker == NULL )
		return;

	LC_ActorBase* pkSrcActor = (LC_ActorBase*)pkAttacker;
	LC_ActorBase* pkDstActor = (LC_ActorBase*)pkScriptCaller;

	pkDstActor->UnregisterCombatEventData(eCombatEvent_Passive_Crit, pkSrcActor->GetOwnerObjectID());
}

void SC_ServerSkillInterface::SetTargetLocationValue(GameLogic::LC_Attacker* pkScriptCaller, int posx, int posy, int posz)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	object_id_type lCallerID = pkScriptCaller->GetID();
	if (!IS_PLAYER_CLASS_ID(lCallerID) && !IS_NPC_ID(lCallerID))
	{
		return;
	}

	LC_ActorBase* pkActor = (LC_ActorBase*)pkScriptCaller;

	UT_Vec3Int kTargetLocation;
	kTargetLocation.x = posx;
	kTargetLocation.y = posy;
	kTargetLocation.z = posz;

	pkActor->SetTargetLocation(kTargetLocation);
}
bool SC_ServerSkillInterface::ModifyVIPGrowthValue(GameLogic::LC_Attacker* pkScriptCaller,int iTargetIndex,int64_t iDeltaValue)
{
	if (pkScriptCaller == NULL || !IS_PLAYER_CLASS_ID(pkScriptCaller->GetID()))
		return false;

	int oldLevel = 0;
	int currentLevel = 0;

	LC_ServerPlayer* pCaller = (LC_ServerPlayer*)pkScriptCaller;
	{
		pCaller = pCaller->GetOwnerPlayer();
		if(NULL != pCaller)
		{
			oldLevel = pCaller->GetVIPAsset().GetVIPLevel();
			pCaller->GetVIPAsset().ModifyCreditGrowthValue(iDeltaValue);
			currentLevel = pCaller->GetVIPAsset().GetVIPLevel();

			if(currentLevel > oldLevel)
			{
				MG_Notify_VIPLevel_Change msg;
				msg.m_nOldVIPLevel = oldLevel;
				msg.m_nCurrentVIPLevel = currentLevel;
				pCaller->SendMsgToClient(MG_NOTIFY_VIPLEVEL_CHANGE, &msg);
			}
			return true;
		}
	}

	
	return false;
}

void SC_ServerSkillInterface::ModifyTargetAttributeByValue(GameLogic::LC_Attacker* pkScriptCaller,int iTargetIndex,int iAttributeType,int64_t iDeltaValue,bool bRevert)
{
	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller,iTargetIndex, kTargetsList);
	LC_ActorPtrListIter IterBe = kTargetsList.begin();
	LC_ActorPtrListIter IterEd = kTargetsList.end();
	LC_ActorBase* pkTarget = NULL;
	for(; IterBe != IterEd; ++IterBe)
	{
		pkTarget = (*IterBe);
		_modifyTargetAttributeByValue(pkTarget,iAttributeType,iDeltaValue,bRevert, false);
		GameLogic::LC_ActorBase* pActorBase = (GameLogic::LC_ActorBase*)pkTarget;
		if(NULL != pActorBase)
		{
			if ( false == IS_PLAYER_CLASS_ID(pActorBase->GetID()))
				continue;
			LC_ServerPlayer* pPlayer = (LC_ServerPlayer*)pActorBase;
			IF_A_VALID_PLAYER(pPlayer)
				pPlayer->SyncBuddyAttribute();
		}
	}
}

void SC_ServerSkillInterface::ModifyTargetAttributeByPercent(GameLogic::LC_Attacker* pkScriptCaller,int iTargetIndex,int iAttributeType,double fDeltaPercent,bool bRevert)
{
	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller,iTargetIndex, kTargetsList);
	LC_ActorPtrListIter IterBe = kTargetsList.begin();
	LC_ActorPtrListIter IterEd = kTargetsList.end();
	LC_ActorBase* pkTarget = NULL;
	for(; IterBe != IterEd; ++IterBe)
	{
		pkTarget = (*IterBe);
		_modifyTargetAttributeByPercent(pkTarget,iAttributeType,fDeltaPercent,bRevert, false);
		GameLogic::LC_ActorBase* pActorBase = (GameLogic::LC_ActorBase*)pkTarget;
		if(NULL != pActorBase)
		{
			if ( false == IS_PLAYER_CLASS_ID(pActorBase->GetID()))
				continue;
			LC_ServerPlayer* pPlayer = (LC_ServerPlayer*)pActorBase;
			IF_A_VALID_PLAYER(pPlayer)
				pPlayer->SyncBuddyAttribute();
		}
	}
}
//------------------------------------------------------------
void SC_ServerSkillInterface::SetTargetVehicleType(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lVehicleType)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	if (pkScriptCaller->GetType() != LOT_PLAYER && pkScriptCaller->GetType() != LOT_NPC)
	{
		return;
	}

	LC_ActorPtrList kTargetsList;
	_findTarget((LC_Attacker*)pkScriptCaller, iTargetIndex, kTargetsList);
	LC_ActorPtrListIter IterBe = kTargetsList.begin();
	LC_ActorPtrListIter IterEd = kTargetsList.end();

	LC_ActorBase* pkTarget = NULL;
	object_id_type lTargetID;
	for (; IterBe != IterEd; ++IterBe)
	{
		pkTarget = (*IterBe);
		lTargetID = pkTarget->GetID();

		if (IS_PLAYER_CLASS_ID(lTargetID))
		{
			LC_ServerPlayer* pkPlayer = ((LC_ServerPlayer*)pkTarget)->GetOwnerPlayer();
			if(NULL == pkPlayer)
				return;

			if (pkPlayer->GetRiderCharType() > 0)
			{
				//if ( pkPlayer->GetRiderCharType() != lVehicleType )
				pkPlayer->DismissVehicle();
				//pkPlayer->SummonVehicle();
			}
			//else
			if ( lVehicleType > 0 )
			{
				pkPlayer->SummonVehicle();
			}
		}
		else if (IS_NPC_ID(lTargetID))
		{
			LC_ServerNPC* pkNPC = (LC_ServerNPC*)pkTarget;
			pkNPC->SetRiderCharType(lVehicleType);
		}
	}
}
//------------------------------------------------------------
void SC_ServerSkillInterface::NotifyCastSkillToAchievement(GameLogic::LC_Attacker* pkScriptCaller, int achievementType)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}
	LC_ServerPlayer* pkActor = (LC_ServerPlayer*)pkScriptCaller;

	pkActor->NotifyCastSkillToAchievement(achievementType);
}

void SC_ServerSkillInterface::NotifyEventToGoal(GameLogic::LC_Attacker* pkScriptCaller, int goalType, int param)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}
	LC_ServerPlayer* pkActor = (LC_ServerPlayer*)pkScriptCaller;

	pkActor->NotifyEventToGoal(goalType, param);
}
//-------------------------------------------------------------------------------------
void SC_ServerSkillInterface::NotifyCastSkillToGatherTaskNpc(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex)
{
	if (NULL == pkScriptCaller)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "SC_ServerSkillInterfaceNotifyCastSkillToGatherTaskNpc r1");
		return;
	}
	LC_ServerPlayer* pkActor = (LC_ServerPlayer*)pkScriptCaller;

	int npcID = 0;
	if (iTargetIndex != TARGET_LOCKED)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "SC_ServerSkillInterfaceNotifyCastSkillToGatherTaskNpc r2 iTargetIndex:%d, c_uid:%d", iTargetIndex, pkActor->GetUserID());
		return;
	}

	LC_ActorPtrList kTargetsList;
	_findTarget((LC_Attacker*)pkActor, iTargetIndex, kTargetsList);
	if (kTargetsList.empty())
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "SC_ServerSkillInterfaceNotifyCastSkillToGatherTaskNpc r3 c_uid:%d", pkActor->GetUserID());
		return;
	}

	LC_ActorBase* pkTarget = kTargetsList.front();
	if (!pkTarget)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "SC_ServerSkillInterfaceNotifyCastSkillToGatherTaskNpc r4 c_uid:%d", pkActor->GetUserID());
		return;
	}
	
	npcID = pkTarget->GetCharType();
	pkActor->NotifyCastSkillToGatherTaskNpc(npcID);
}
//------------------------------------------------------------
void SC_ServerSkillInterface::SetEventParam(Skill::SK_SkillEvent* pkEvent, int iParamIndex, int64_t lParam)
{
	if (NULL == pkEvent)
	{
		return;
	}
	if (iParamIndex < 0 || iParamIndex >= MAX_SKILL_EVENT_PARAM_COUNT)
	{
		return;
	}
	pkEvent->SetParam(iParamIndex, lParam);
}
//------------------------------------------------------------
void SC_ServerSkillInterface::ShotBulletAroundTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lMinRadius, int lMaxRadius, int lSkillTypeID, int iBulletFuncID, int iPenetrateCount, int iDummyType)
{
	if (!IS_PLAYER_CLASS_ID(pkScriptCaller->GetID()) && !IS_NPC_ID(pkScriptCaller->GetID()))
		return;
	LC_ActorBase* pCaller = (LC_ActorBase*)pkScriptCaller;

	UT_Vec3Int center;
	switch (iTargetIndex)
	{
	case TARGET_NONE:
		{
			SK_Factory* pSkillFactory = SK_Factory::GetSingletonPtr();
			SK_SkillExecute* pSkillExecute = pSkillFactory->RequestSkillExcute(lSkillTypeID);
			if (pSkillExecute == NULL)
				return;
			const CF_SkillBulletFunc::DataEntry* pBulletFunc = pSkillExecute->GetBulletDataEntryPtr(iBulletFuncID);
			if (pBulletFunc == NULL)
				return;

			int distance = pBulletFunc->_lMaxMoveDistance;
			UT_Vec2Float dir = pCaller->GetForwardDir();
			center = UT_MathBase::V3dIncrementAdd(pCaller->GetCurrentLocation(), dir, distance);
		}
		break;
	case TARGET_LOCKED:
		{
			LC_ActorPtrList targetList;
			_findTarget(pCaller, iTargetIndex, targetList);
			if (targetList.size() <= 0)
			{
				return;
			}
			LC_ActorBase* pTarget = targetList.front();
			if (pTarget == NULL)
				return;

			center = pTarget->GetCurrentLocation();
		}
		break;
	case TARGET_LOCATION:
		center = pCaller->GetLockedLocation();
		break;
	default:
		return;
	}

	UT_Vec3Int preLockedLocation = pCaller->GetLockedLocation();

	UT_Vec3Int targetLocation = UT_MathBase::RandInCircleInt(center, lMinRadius, lMaxRadius);
	pCaller->SetLockedLocation(targetLocation);
	ShotBulletToTarget(pCaller, TARGET_LOCATION, lSkillTypeID, iBulletFuncID, iPenetrateCount, iDummyType);

	pCaller->SetLockedLocation(preLockedLocation);
	//if (NULL == pkScriptCaller
	//		&& !IS_PLAYER_CLASS_ID(pkScriptCaller->GetID())
	//		&& !IS_NPC_ID(pkScriptCaller->GetID()))
	//{
	//	return;
	//}

	////查找目标
	//LC_ActorPtrList& kTargetList = _findTarget(pkScriptCaller, iTargetIndex);
	//if (kTargetList.size() <= 0)
	//{
	//	return;
	//}
	//LC_ActorBase* pkTarget = kTargetList.front();	//目标
	//UT_Vec3Int  kTargetLocation = pkTarget->GetCurrentLocation();	//目标位置
	//UT_Vec3Int  kAroundLocation = UT_MathBase::RandInCircleInt(kTargetLocation, lMinRadius, lMaxRadius);	//lMinRadius, lMaxRadius范围内周围随机一个位置

	//LC_ActorBase* pkActor = (LC_ActorBase*)pkScriptCaller;
	//UT_Vec3Int  kLockedLocation = pkActor->GetLockedLocation();		//保存pkScriptCaller的LockedLocation
	//pkActor->SetLockedLocation(kAroundLocation);	//设置新的LockedLocation给ShotBulletToTarget用，因为参数是TARGET_LOCATION
	//ShotBulletToTarget(pkScriptCaller, TARGET_LOCATION, lSkillTypeID, iBulletFuncID, iPenetrateCount, iDummyType);
	//pkActor->SetLockedLocation(kLockedLocation);	//恢复pkScriptCaller的LockedLocation
}
//------------------------------------------------------------
static SK_SkillExecute* _verify_skill_bullet(int lSkillTypeID, int iBulletFuncID, const CSVFile::CF_SkillBulletFunc::DataEntry * & pkBulletDataPtr)
{
	SK_Factory* pkSkillFactory = SK_Factory::GetSingletonPtr();
	SK_SkillExecute* pkSkillExecute = pkSkillFactory->RequestSkillExcute(lSkillTypeID);
	if (NULL == pkSkillExecute)
		return NULL;

	const CSVFile::CF_SkillBulletFunc::DataEntry * pkBullet = pkSkillExecute->GetBulletDataEntryPtr(iBulletFuncID);
	if (NULL == pkBullet)
		return NULL;

	if (pkBullet->_lMoveSpeed <= 0)
		return NULL;

	if (pkBullet->_lShapeRadius <= 0)
		return NULL;

	if (pkBullet->_lMoveType < BT_SHOOT || pkBullet->_lMoveType >= BT_MAX_COUNT)
		return NULL;

	pkBulletDataPtr = pkBullet;

	return pkSkillExecute;
}
//------------------------------------------------------------
static bool _is_active_buddy(LC_Attacker* pkScriptCaller)
{
	if ( pkScriptCaller->GetType() == LOT_PLAYER )
	{
		LC_ServerPlayer* pServerPlayer= (LC_ServerPlayer*)pkScriptCaller;
		//if (pServerPlayer->GetControlType() == LC_ServerPlayer::eControlType_Player && pServerPlayer->GetAIControl() == false)
		//{
		//	return true;
		//}

		if ( pServerPlayer->GetControlType() == LC_ServerPlayer::eControlType_Buddy )
			return false;

		if ( pServerPlayer->GetControlType() == LC_ServerPlayer::eControlType_Player && IS_PLAYER_CLONE_ID(pServerPlayer->GetOwnerPlayerId()))
			return false;

		return true;
	}
	return false;
}
//------------------------------------------------------------
// 客户端子弹的碰撞值在 [0~1]，服务器的默认比1大一点点，几乎不影响子弹的距离计算
static const float c_fDefaultCollidePosParamT = 1.0001f;

void SC_ServerSkillInterface::ShotBulletToTarget(LC_Attacker* pkScriptCaller, int iTargetIndex, int lSkillTypeID, int iBulletFuncID, int iPenetrateCount, int iDummyType)
{
	// 检测调用者
	if ( !IS_PLAYER_CLASS_ID(pkScriptCaller->GetID())
	  && !IS_NPC_ID(pkScriptCaller->GetID()))
	{
		return;
	}


	// 检测子弹
	const CSVFile::CF_SkillBulletFunc::DataEntry *	pkBullet = NULL;
	SK_SkillExecute* pkSkillExecute = _verify_skill_bullet(lSkillTypeID, iBulletFuncID, pkBullet);
	if (NULL == pkSkillExecute)
		return;

	// 排除掉主角(ActiveBuddy)发射的子弹，由客户端创建
	// weixin commented:
	// Must not be a LC_Skill
	bool bLC = pkSkillExecute->GetProperty_IsLCSkill();
	if ( _is_active_buddy(pkScriptCaller) && !pkSkillExecute->GetProperty_IsLCSkill())
		return;

	LC_ActorBase * pkActor = (LC_ActorBase*)pkScriptCaller;

	UT_Vec2Float dir = pkActor->GetForwardDir();

	// 准备调用
	ShotBulletArg arg;
	arg.pkSkillExecute	= pkSkillExecute;
	arg.pkBullet		= pkBullet;
	arg.lSkillTypeID	= lSkillTypeID;
	arg.iBulletFuncID	= iBulletFuncID;
	arg.iPenetrateCount = iPenetrateCount;
	arg.iDummyType		= iDummyType;
	arg.fStartTime      = GET_PROCESS_TIME;
	arg.fMoveTime		= 0;
	//arg.kShotLocation   = pkScriptCaller->GetCurrentLocationWithZ();
	int nRetCode = pkScriptCaller->GetCurrentLocationWithZ(arg.kShotLocation );
	arg.kShotDir        = UT_MathBase::ConvertVec2To3Float(dir);
	arg.fPitchAngle		= pkBullet->_fPitchAngle;

	// 发射位置偏移
	UT_Vec3Float kShotLocalOffset = UT_MathBase::ConvertVec3IntToFloat(pkBullet->_kShotPosLocalOffset);
	UT_Matrix3Float rot;
	rot.MakeZRotation(pkActor->GetForwardDirAngle());
	UT_Vec3Float kShotWorldOffset = rot * kShotLocalOffset;
	arg.kShotLocation  += UT_MathBase::ConvertVec3FloatToInt(kShotWorldOffset);

	arg.iClientLocalID	  = 0;
	arg.fCollidePosParamT = c_fDefaultCollidePosParamT;
	arg.iCollideSurfaceMaterial = 0;
	arg.kDestLocation.x = pkScriptCaller->GetLockedLocation().x;
	arg.kDestLocation.y = pkScriptCaller->GetLockedLocation().y;
	arg.kDestLocation.z = pkScriptCaller->GetLockedLocation().z;
	_implShotBullet(pkScriptCaller, iTargetIndex, arg);
}

void SC_ServerSkillInterface::ShotBulletToTargetAngularDeflection(LC_Attacker* pkScriptCaller, int iTargetIndex, int lSkillTypeID, int iBulletFuncID, int iPenetrateCount, int iDummyType, float fAngle)
{
	// 检测调用者
	if ( !IS_PLAYER_CLASS_ID(pkScriptCaller->GetID())
		&& !IS_NPC_ID(pkScriptCaller->GetID()))
	{
		return;
	}


	// 检测子弹
	const CSVFile::CF_SkillBulletFunc::DataEntry *	pkBullet = NULL;
	SK_SkillExecute* pkSkillExecute = _verify_skill_bullet(lSkillTypeID, iBulletFuncID, pkBullet);
	if (NULL == pkSkillExecute)
		return;

	// 排除掉主角(ActiveBuddy)发射的子弹，由客户端创建
	// weixin commented:
	// Must not be a LC_Skill
	bool bLC = pkSkillExecute->GetProperty_IsLCSkill();
	if ( _is_active_buddy(pkScriptCaller) && !pkSkillExecute->GetProperty_IsLCSkill())
		return;

	LC_ActorBase * pkActor = (LC_ActorBase*)pkScriptCaller;

	UT_Vec2Float dir = pkActor->GetForwardDir();
	if (fAngle != 0)
		dir = UT_MathBase::ZRot(dir, fAngle);

	// 准备调用
	ShotBulletArg arg;
	arg.pkSkillExecute	= pkSkillExecute;
	arg.pkBullet		= pkBullet;
	arg.lSkillTypeID	= lSkillTypeID;
	arg.iBulletFuncID	= iBulletFuncID;
	arg.iPenetrateCount = iPenetrateCount;
	arg.iDummyType		= iDummyType;
	arg.fStartTime      = GET_PROCESS_TIME;
	arg.fMoveTime		= 0;
	//arg.kShotLocation   = pkScriptCaller->GetCurrentLocationWithZ();
	int nRetCode = pkScriptCaller->GetCurrentLocationWithZ(arg.kShotLocation );
	arg.kShotDir        = UT_MathBase::ConvertVec2To3Float(dir);
	arg.fPitchAngle		= pkBullet->_fPitchAngle;

	// 发射位置偏移
	UT_Vec3Float kShotLocalOffset = UT_MathBase::ConvertVec3IntToFloat(pkBullet->_kShotPosLocalOffset);
	UT_Matrix3Float rot;
	rot.MakeZRotation(pkActor->GetForwardDirAngle());
	UT_Vec3Float kShotWorldOffset = rot * kShotLocalOffset;
	arg.kShotLocation  += UT_MathBase::ConvertVec3FloatToInt(kShotWorldOffset);

	arg.iClientLocalID	  = 0;
	arg.fCollidePosParamT = c_fDefaultCollidePosParamT;
	arg.iCollideSurfaceMaterial = 0;
	_implShotBullet(pkScriptCaller, iTargetIndex, arg);
}

void SC_ServerSkillInterface::ShotBulletToTargetFromLocation(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lSkillTypeID, int iBulletFuncID, int iPenetrateCount, int iDummyType, int iLocationIndex, float fDeltaAngle, int64_t lBulletID)
{
	// 检测调用者
	if (   !IS_PLAYER_CLASS_ID(pkScriptCaller->GetID())
		&& !IS_NPC_ID(pkScriptCaller->GetID()))
	{
		return;
	}

	// 检测子弹
	const CSVFile::CF_SkillBulletFunc::DataEntry *	pkBullet = NULL;
	SK_SkillExecute* pkSkillExecute = _verify_skill_bullet(lSkillTypeID, iBulletFuncID, pkBullet);
	if (NULL == pkSkillExecute)
		return;

	// 排除掉主角(ActiveBuddy)发射的子弹，由客户端创建
	// weixin commented:
	// Must not be a LC_Skill
	bool bLC = pkSkillExecute->GetProperty_IsLCSkill();
	if ( _is_active_buddy(pkScriptCaller) && !pkSkillExecute->GetProperty_IsLCSkill())
		return;

	UT_Vec3Int kShotLocation;
	switch(iLocationIndex)
	{
	case TARGET_BULLET_LOCATION:
		{
			kShotLocation = pkScriptCaller->GetBulletLocation();
		}
		break;
	case TARGET_LOCATION:
		{
			kShotLocation = pkScriptCaller->GetLockedLocation();
		}
	default:
		{
			LC_ActorPtrList kTargetsList;
			_findTarget(pkScriptCaller,iLocationIndex, kTargetsList);
			if(kTargetsList.size() != 1)
			{
				return;
			}

			LC_ActorBase* pkTarget = kTargetsList.front();
			pkTarget->GetCurrentLocationWithZ(kShotLocation);
		}
		break;
	}
	
	LC_ActorBase * pkActor = (LC_ActorBase*)pkScriptCaller;
	UT_Vec2Float shotDir = pkActor->GetForwardDir();
	if (lBulletID > 0)
	{
		long eventID = 0, bulletLocalID = 0;
		_getBulletAndEventID(lBulletID,eventID,bulletLocalID);

		SK_ServerSkillEventManager* pserverSkillEventManager = SERVER_GET_SKILL_EVENT_MANAGER();
		SK_BulletEvent* bulletEvent = (SK_BulletEvent*)(pserverSkillEventManager->FindEvent(eventID));
		if (NULL != bulletEvent /*&& bulletEvent->GetParam(SK_BulletEvent::SEP_MOVE_TYPE) == BT_SHOOT*/)
		{
			UT_Vec3Int fromLocation(bulletEvent->GetParam(SK_BulletEvent::SEP_SHOT_LOCATION_X), bulletEvent->GetParam(SK_BulletEvent::SEP_SHOT_LOCATION_Y), bulletEvent->GetParam(SK_BulletEvent::SEP_SHOT_LOCATION_Z));
			UT_Vec3Int targetLocation(bulletEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_X), bulletEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Y), bulletEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Z));
			UT_Vec3Int preBulletDir = targetLocation - fromLocation;
			shotDir = UT_Vec2Float((float)preBulletDir.x, (float)preBulletDir.y);
			shotDir.Unitize();
		}
	}

	// 准备调用
	ShotBulletArg arg;
	arg.pkSkillExecute	= pkSkillExecute;
	arg.pkBullet		= pkBullet;
	arg.lSkillTypeID	= lSkillTypeID;
	arg.iBulletFuncID	= iBulletFuncID;
	arg.iPenetrateCount = iPenetrateCount;
	arg.iDummyType		= iDummyType;
	arg.fStartTime      = GET_PROCESS_TIME;
	arg.fMoveTime		= 0;
	arg.kShotLocation	= kShotLocation;
	arg.fPitchAngle		= pkBullet->_fPitchAngle;
	shotDir = UT_MathBase::ZRot(shotDir, fDeltaAngle);
	arg.kShotDir = UT_MathBase::ConvertVec2To3Float(shotDir);

	// 发射位置偏移
	UT_Vec3Float kShotLocalOffset = UT_MathBase::ConvertVec3IntToFloat(pkBullet->_kShotPosLocalOffset);
	UT_Matrix3Float rot;
	rot.MakeZRotation(UT_MathBase::GetZRotAngle(shotDir));
	UT_Vec3Float kShotWorldOffset = rot * kShotLocalOffset;
	arg.kShotLocation  += UT_MathBase::ConvertVec3FloatToInt(kShotWorldOffset);

	arg.iClientLocalID	  = 0;
	arg.fCollidePosParamT = c_fDefaultCollidePosParamT;
	arg.iCollideSurfaceMaterial = 0;
	_implShotBullet(pkScriptCaller, iTargetIndex, arg);
}

void SC_ServerSkillInterface::ShotMultiBulletToTarget(LC_Attacker* pkScriptCaller, int iTargetIndex, int lSkillTypeID, int iBulletFuncID, int iPenetrateCount, int iDummyType, int iScatteringAngle, int iScatteringCnt)
{
	//return;
	// 单束子弹请用 ShotBulletToTarget
	if (iScatteringCnt <= 1)
	{
		ShotBulletToTarget(pkScriptCaller, iTargetIndex, lSkillTypeID, iBulletFuncID, iPenetrateCount, iDummyType);
		return;
	}

	// 检测调用者
	if (   !IS_PLAYER_CLASS_ID(pkScriptCaller->GetID())
		&& !IS_NPC_ID(pkScriptCaller->GetID()))
	{
		return;
	}

	// 检测子弹
	const CSVFile::CF_SkillBulletFunc::DataEntry *	pkBullet = NULL;
	SK_SkillExecute* pkSkillExecute = _verify_skill_bullet(lSkillTypeID, iBulletFuncID, pkBullet);
	if (NULL == pkSkillExecute)
		return;

	// 排除掉主角(ActiveBuddy)发射的子弹，由客户端创建
	// weixin commented:
	// Must not be a LC_Skill
	if ( _is_active_buddy(pkScriptCaller) && !pkSkillExecute->GetProperty_IsLCSkill())
		return;

	LC_ActorBase * pkActor = (LC_ActorBase*)pkScriptCaller;

	if(!GetGlobalSetting.nBulletSetting)
	{
		// 准备调用
		ShotBulletArg arg;
		arg.pkSkillExecute	= pkSkillExecute;
		arg.pkBullet		= pkBullet;
		arg.lSkillTypeID	= lSkillTypeID;
		arg.iBulletFuncID	= iBulletFuncID;
		arg.iPenetrateCount = iPenetrateCount;
		arg.iDummyType		= iDummyType;
		arg.fStartTime      = GET_PROCESS_TIME;
		arg.fMoveTime		= 0;
		//arg.kShotLocation   = pkScriptCaller->GetCurrentLocationWithZ();
		int nRetCode  = pkScriptCaller->GetCurrentLocationWithZ(arg.kShotLocation);
		arg.fPitchAngle		= pkBullet->_fPitchAngle;

		// 发射位置偏移
		UT_Vec3Float kShotLocalOffset = UT_MathBase::ConvertVec3IntToFloat(pkBullet->_kShotPosLocalOffset);
		UT_Matrix3Float rot;
		rot.MakeZRotation(pkActor->GetForwardDirAngle());
		UT_Vec3Float kShotWorldOffset = rot * kShotLocalOffset;
		arg.kShotLocation  += UT_MathBase::ConvertVec3FloatToInt(kShotWorldOffset);

		UT_Vec2Float kCurrentForward = pkActor->GetForwardDir();

		// 朝不同方向进行射击
		float fScatteringDeltaAngle = Int2Float(iScatteringAngle) / Int2Float(iScatteringCnt - 1);
		for (int i = 0; i < iScatteringCnt; ++i)
		{
			Utility::UT_Vec2Float newAt = UT_MathBase::ZRot(kCurrentForward, (i - (iScatteringCnt-1)/2.0f) * fScatteringDeltaAngle);
			arg.kShotDir = UT_MathBase::ConvertVec2To3Float(newAt);

			arg.iClientLocalID	  = 0;
			arg.fCollidePosParamT = c_fDefaultCollidePosParamT;
			arg.iCollideSurfaceMaterial = 0;
			_implShotBullet(pkScriptCaller, iTargetIndex, arg);
		}
	}
	else
	{
		mem::vector<ShotBulletArg> argVector;
		// 准备调用
		ShotBulletArg arg;
		arg.pkSkillExecute	= pkSkillExecute;
		arg.pkBullet		= pkBullet;
		arg.lSkillTypeID	= lSkillTypeID;
		arg.iBulletFuncID	= iBulletFuncID;
		arg.iPenetrateCount = iPenetrateCount;
		arg.iDummyType		= iDummyType;
		arg.fStartTime      = GET_PROCESS_TIME;
		arg.fMoveTime		= 0;
		//arg.kShotLocation   = pkScriptCaller->GetCurrentLocationWithZ();
		int nRetCode  = pkScriptCaller->GetCurrentLocationWithZ(arg.kShotLocation);
		arg.fPitchAngle		= pkBullet->_fPitchAngle;

		// 发射位置偏移
		UT_Vec3Float kShotLocalOffset = UT_MathBase::ConvertVec3IntToFloat(pkBullet->_kShotPosLocalOffset);
		UT_Matrix3Float rot;
		rot.MakeZRotation(pkActor->GetForwardDirAngle());
		UT_Vec3Float kShotWorldOffset = rot * kShotLocalOffset;
		arg.kShotLocation  += UT_MathBase::ConvertVec3FloatToInt(kShotWorldOffset);

		UT_Vec2Float kCurrentForward = pkActor->GetForwardDir();

		// 朝不同方向进行射击
		float fScatteringDeltaAngle = Int2Float(iScatteringAngle) / Int2Float(iScatteringCnt - 1);
		for (int i = 0; i < iScatteringCnt; ++i)
		{
			Utility::UT_Vec2Float newAt = UT_MathBase::ZRot(kCurrentForward, (i - (iScatteringCnt-1)/2.0f) * fScatteringDeltaAngle);
			arg.kShotDir = UT_MathBase::ConvertVec2To3Float(newAt);

			arg.iClientLocalID	  = 0;
			arg.fCollidePosParamT = c_fDefaultCollidePosParamT;
			arg.iCollideSurfaceMaterial = 0;

			argVector.push_back(arg);
			//_implShotBullet(pkScriptCaller, iTargetIndex, arg);
		}
		_implShotMultiBullet(pkScriptCaller, iTargetIndex, argVector);
	}
}

void SC_ServerSkillInterface::ShotMultiBulletToTargetFromLocation(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lSkillTypeID, int iBulletFuncID, int iPenetrateCount, int iDummyType, int iScatteringAngle, int iScatteringCnt, int iLocationIndex, float fDeltaAngle, int64_t lBulletID)
{
	if (iScatteringCnt == 1)
	{
		ShotBulletToTargetFromLocation(pkScriptCaller, iTargetIndex, lSkillTypeID, iBulletFuncID, iPenetrateCount, iDummyType, iLocationIndex, fDeltaAngle, lBulletID);
		return;
	}

	// 检测调用者
	if (   !IS_PLAYER_CLASS_ID(pkScriptCaller->GetID())
		&& !IS_NPC_ID(pkScriptCaller->GetID()))
	{
		return;
	}

	// 检测子弹
	const CSVFile::CF_SkillBulletFunc::DataEntry *	pkBullet = NULL;
	SK_SkillExecute* pkSkillExecute = _verify_skill_bullet(lSkillTypeID, iBulletFuncID, pkBullet);
	if (NULL == pkSkillExecute)
		return;

	// 排除掉主角(ActiveBuddy)发射的子弹，由客户端创建
	// weixin commented:
	// Must not be a LC_Skill
	if ( _is_active_buddy(pkScriptCaller) && !pkSkillExecute->GetProperty_IsLCSkill())
		return;

	UT_Vec3Int kShotLocation;
	switch(iLocationIndex)
	{
	case TARGET_BULLET_LOCATION:
		{
			kShotLocation = pkScriptCaller->GetBulletLocation();
		}
		break;
	case TARGET_LOCATION:
		{
			kShotLocation = pkScriptCaller->GetLockedLocation();
		}
	default:
		{
			LC_ActorPtrList kTargetsList;
			_findTarget(pkScriptCaller,iLocationIndex, kTargetsList);
			if(kTargetsList.size() != 1)
			{
				return;
			}

			LC_ActorBase* pkTarget = kTargetsList.front();
			pkTarget->GetCurrentLocationWithZ(kShotLocation);
		}
		break;
	}

	LC_ActorBase * pkActor = (LC_ActorBase*)pkScriptCaller;
	UT_Vec2Float shotDir = pkActor->GetForwardDir();
	if (lBulletID > 0)
	{
		long eventID = 0, bulletLocalID = 0;
		_getBulletAndEventID(lBulletID,eventID,bulletLocalID);

		SK_ServerSkillEventManager* pserverSkillEventManager = SERVER_GET_SKILL_EVENT_MANAGER();
		SK_BulletEvent* bulletEvent = (SK_BulletEvent*)(pserverSkillEventManager->FindEvent(eventID));
		if (NULL != bulletEvent /*&& bulletEvent->GetParam(SK_BulletEvent::SEP_MOVE_TYPE) == BT_SHOOT*/)
		{
			UT_Vec3Int fromLocation(bulletEvent->GetParam(SK_BulletEvent::SEP_SHOT_LOCATION_X), bulletEvent->GetParam(SK_BulletEvent::SEP_SHOT_LOCATION_Y), bulletEvent->GetParam(SK_BulletEvent::SEP_SHOT_LOCATION_Z));
			UT_Vec3Int targetLocation(bulletEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_X), bulletEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Y), bulletEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Z));
			UT_Vec3Int preBulletDir = targetLocation - fromLocation;
			shotDir = UT_Vec2Float((float)preBulletDir.x, (float)preBulletDir.y);
			shotDir.Unitize();
		}
	}

	if(!GetGlobalSetting.nBulletSetting)
	{
		// 准备调用
		ShotBulletArg arg;
		arg.pkSkillExecute	= pkSkillExecute;
		arg.pkBullet		= pkBullet;
		arg.lSkillTypeID	= lSkillTypeID;
		arg.iBulletFuncID	= iBulletFuncID;
		arg.iPenetrateCount = iPenetrateCount;
		arg.iDummyType		= iDummyType;
		arg.fStartTime      = GET_PROCESS_TIME;
		arg.fMoveTime		= 0;
		arg.fPitchAngle		= pkBullet->_fPitchAngle;
		arg.kShotLocation	= kShotLocation;

		// 发射位置偏移
		UT_Vec3Float kShotLocalOffset = UT_MathBase::ConvertVec3IntToFloat(pkBullet->_kShotPosLocalOffset);
		UT_Matrix3Float rot;
		shotDir = UT_MathBase::ZRot(shotDir, fDeltaAngle);
		rot.MakeZRotation(UT_MathBase::GetZRotAngle(shotDir));
		UT_Vec3Float kShotWorldOffset = rot * kShotLocalOffset;
		arg.kShotLocation  += UT_MathBase::ConvertVec3FloatToInt(kShotWorldOffset);

		// 朝不同方向进行射击
		float fScatteringDeltaAngle = Int2Float(iScatteringAngle) / Int2Float(iScatteringCnt - 1);
		for (int i = 0; i < iScatteringCnt; ++i)
		{
			Utility::UT_Vec2Float newAt = UT_MathBase::ZRot(shotDir, (i - (iScatteringCnt-1)/2.0f) * fScatteringDeltaAngle);
			arg.kShotDir = UT_MathBase::ConvertVec2To3Float(newAt);

			arg.iClientLocalID	  = 0;
			arg.fCollidePosParamT = c_fDefaultCollidePosParamT;
			arg.iCollideSurfaceMaterial = 0;
			_implShotBullet(pkScriptCaller, iTargetIndex, arg);
		}
	}
	else
	{
		mem::vector<ShotBulletArg> argVector;
		// 准备调用
		ShotBulletArg arg;
		arg.pkSkillExecute	= pkSkillExecute;
		arg.pkBullet		= pkBullet;
		arg.lSkillTypeID	= lSkillTypeID;
		arg.iBulletFuncID	= iBulletFuncID;
		arg.iPenetrateCount = iPenetrateCount;
		arg.iDummyType		= iDummyType;
		arg.fStartTime      = GET_PROCESS_TIME;
		arg.fMoveTime		= 0;
		arg.fPitchAngle		= pkBullet->_fPitchAngle;
		arg.kShotLocation	= kShotLocation;

		// 发射位置偏移
		UT_Vec3Float kShotLocalOffset = UT_MathBase::ConvertVec3IntToFloat(pkBullet->_kShotPosLocalOffset);
		UT_Matrix3Float rot;
		shotDir = UT_MathBase::ZRot(shotDir, fDeltaAngle);
		rot.MakeZRotation(UT_MathBase::GetZRotAngle(shotDir));
		UT_Vec3Float kShotWorldOffset = rot * kShotLocalOffset;
		arg.kShotLocation  += UT_MathBase::ConvertVec3FloatToInt(kShotWorldOffset);

		// 朝不同方向进行射击
		float fScatteringDeltaAngle = Int2Float(iScatteringAngle) / Int2Float(iScatteringCnt - 1);
		for (int i = 0; i < iScatteringCnt; ++i)
		{
			Utility::UT_Vec2Float newAt = UT_MathBase::ZRot(shotDir, (i - (iScatteringCnt-1)/2.0f) * fScatteringDeltaAngle);
			arg.kShotDir = UT_MathBase::ConvertVec2To3Float(newAt);

			arg.iClientLocalID	  = 0;
			arg.fCollidePosParamT = c_fDefaultCollidePosParamT;
			arg.iCollideSurfaceMaterial = 0;

			argVector.push_back(arg);
			//_implShotBullet(pkScriptCaller, iTargetIndex, arg);
		}
		_implShotMultiBullet(pkScriptCaller, iTargetIndex, argVector);
	}
}

void SC_ServerSkillInterface::ShotMultiBulletToTargetFromMultiLocation(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lMinRadius, int lMaxRadius, int lSkillTypeID, int iBulletFuncID, int iPenetrateCount, int iDummyType, int iScatteringAngle, int iScatteringCnt, int iLocationIndex, float fDeltaAngle, int64_t lBulletID)
{
	if (iScatteringCnt == 1)
	{
		ShotBulletToTargetFromLocation(pkScriptCaller, iTargetIndex, lSkillTypeID, iBulletFuncID, iPenetrateCount, iDummyType, iLocationIndex, fDeltaAngle, lBulletID);
		return;
	}

	// 检测调用者
	if (   !IS_PLAYER_CLASS_ID(pkScriptCaller->GetID())
		&& !IS_NPC_ID(pkScriptCaller->GetID()))
	{
		return;
	}

	// 检测子弹
	const CSVFile::CF_SkillBulletFunc::DataEntry *	pkBullet = NULL;
	SK_SkillExecute* pkSkillExecute = _verify_skill_bullet(lSkillTypeID, iBulletFuncID, pkBullet);
	if (NULL == pkSkillExecute)
		return;

	// 排除掉主角(ActiveBuddy)发射的子弹，由客户端创建
	// weixin commented:
	// Must not be a LC_Skill
	if ( _is_active_buddy(pkScriptCaller) && !pkSkillExecute->GetProperty_IsLCSkill())
		return;

	LC_ActorBase * pkActor = (LC_ActorBase*)pkScriptCaller;

	UT_Vec3Int center;
	switch(iLocationIndex)
	{
	case TARGET_LOCATION:
		center = pkActor->GetLockedLocation();
		break;
	case TARGET_BULLET_LOCATION:
		center = pkScriptCaller->GetBulletLocation();
		break;
	default:
		{
			LC_ActorPtrList kTargetsList;
			_findTarget(pkScriptCaller,iLocationIndex, kTargetsList);
			if(kTargetsList.size() != 1)
			{
				return;
			}

			LC_ActorBase* pkTarget = kTargetsList.front();
			pkTarget->GetCurrentLocationWithZ(center);
		}
		break;
	}
	UT_Vec3Int kShotLocation = UT_MathBase::RandInCircleInt(center, lMinRadius, lMaxRadius);

	UT_Vec2Float shotDir = pkActor->GetForwardDir();
	if (lBulletID > 0)
	{
		long eventID = 0, bulletLocalID = 0;
		_getBulletAndEventID(lBulletID,eventID,bulletLocalID);

		SK_ServerSkillEventManager* pserverSkillEventManager = SERVER_GET_SKILL_EVENT_MANAGER();
		SK_BulletEvent* bulletEvent = (SK_BulletEvent*)(pserverSkillEventManager->FindEvent(eventID));
		if (NULL != bulletEvent /*&& bulletEvent->GetParam(SK_BulletEvent::SEP_MOVE_TYPE) == BT_SHOOT*/)
		{
			UT_Vec3Int fromLocation(bulletEvent->GetParam(SK_BulletEvent::SEP_SHOT_LOCATION_X), bulletEvent->GetParam(SK_BulletEvent::SEP_SHOT_LOCATION_Y), bulletEvent->GetParam(SK_BulletEvent::SEP_SHOT_LOCATION_Z));
			UT_Vec3Int targetLocation(bulletEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_X), bulletEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Y), bulletEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Z));
			UT_Vec3Int preBulletDir = targetLocation - fromLocation;
			shotDir = UT_Vec2Float((float)preBulletDir.x, (float)preBulletDir.y);
			shotDir.Unitize();
		}
	}

	if(!GetGlobalSetting.nBulletSetting)
	{
		// 准备调用
		ShotBulletArg arg;
		arg.pkSkillExecute	= pkSkillExecute;
		arg.pkBullet		= pkBullet;
		arg.lSkillTypeID	= lSkillTypeID;
		arg.iBulletFuncID	= iBulletFuncID;
		arg.iPenetrateCount = iPenetrateCount;
		arg.iDummyType		= iDummyType;
		arg.fStartTime      = GET_PROCESS_TIME;
		arg.fMoveTime		= 0;
		arg.fPitchAngle		= pkBullet->_fPitchAngle;
		arg.kShotLocation	= kShotLocation;

		// 发射位置偏移
		UT_Vec3Float kShotLocalOffset = UT_MathBase::ConvertVec3IntToFloat(pkBullet->_kShotPosLocalOffset);
		UT_Matrix3Float rot;
		shotDir = UT_MathBase::ZRot(shotDir, fDeltaAngle);
		rot.MakeZRotation(UT_MathBase::GetZRotAngle(shotDir));
		UT_Vec3Float kShotWorldOffset = rot * kShotLocalOffset;
		arg.kShotLocation  += UT_MathBase::ConvertVec3FloatToInt(kShotWorldOffset);

		// 朝不同方向进行射击
		float fScatteringDeltaAngle = Int2Float(iScatteringAngle) / Int2Float(iScatteringCnt - 1);
		for (int i = 0; i < iScatteringCnt; ++i)
		{
			Utility::UT_Vec2Float newAt = UT_MathBase::ZRot(shotDir, (i - (iScatteringCnt-1)/2.0f) * fScatteringDeltaAngle);
			arg.kShotDir = UT_MathBase::ConvertVec2To3Float(newAt);

			arg.iClientLocalID	  = 0;
			arg.fCollidePosParamT = c_fDefaultCollidePosParamT;
			arg.iCollideSurfaceMaterial = 0;
			_implShotBullet(pkScriptCaller, iTargetIndex, arg);
		}
	}
	else
	{
		mem::vector<ShotBulletArg> argVector;
		// 准备调用
		ShotBulletArg arg;
		arg.pkSkillExecute	= pkSkillExecute;
		arg.pkBullet		= pkBullet;
		arg.lSkillTypeID	= lSkillTypeID;
		arg.iBulletFuncID	= iBulletFuncID;
		arg.iPenetrateCount = iPenetrateCount;
		arg.iDummyType		= iDummyType;
		arg.fStartTime      = GET_PROCESS_TIME;
		arg.fMoveTime		= 0;
		arg.fPitchAngle		= pkBullet->_fPitchAngle;
		arg.kShotLocation	= kShotLocation;

		// 发射位置偏移
		UT_Vec3Float kShotLocalOffset = UT_MathBase::ConvertVec3IntToFloat(pkBullet->_kShotPosLocalOffset);
		UT_Matrix3Float rot;
		shotDir = UT_MathBase::ZRot(shotDir, fDeltaAngle);
		rot.MakeZRotation(UT_MathBase::GetZRotAngle(shotDir));
		UT_Vec3Float kShotWorldOffset = rot * kShotLocalOffset;
		arg.kShotLocation  += UT_MathBase::ConvertVec3FloatToInt(kShotWorldOffset);

		// 朝不同方向进行射击
		float fScatteringDeltaAngle = Int2Float(iScatteringAngle) / Int2Float(iScatteringCnt - 1);
		for (int i = 0; i < iScatteringCnt; ++i)
		{
			Utility::UT_Vec2Float newAt = UT_MathBase::ZRot(shotDir, (i - (iScatteringCnt-1)/2.0f) * fScatteringDeltaAngle);
			arg.kShotDir = UT_MathBase::ConvertVec2To3Float(newAt);

			arg.iClientLocalID	  = 0;
			arg.fCollidePosParamT = c_fDefaultCollidePosParamT;
			arg.iCollideSurfaceMaterial = 0;

			argVector.push_back(arg);
			//_implShotBullet(pkScriptCaller, iTargetIndex, arg);
		}
		_implShotMultiBullet(pkScriptCaller, iTargetIndex, argVector);
	}
}

//------------------------------------------------------------
static bool _testBulletStraightLineSceneCollide(GameLogic::LC_Attacker* pkScriptCaller, 
												const UT_Vec3Float & kShotPos, const UT_Vec3Float & kShotRay, 
												Utility::UT_PickResult & kPickResult)
{
	int iWorldID          = pkScriptCaller->GetMapResID();

	bool  bCollided = UT_Pick_Manager::GetSingleton().PickWorld(iWorldID, kShotPos, kShotRay, kPickResult);
	return bCollided;
}

//------------------------------------------------------------
void SC_ServerSkillInterface::_implShotBullet(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, ShotBulletArg & arg)
{
	int iMoveType		= arg.pkBullet->_lMoveType;
	float fMoveSpeed	= Long2Float(arg.pkBullet->_lMoveSpeed);
	bool bServerTestCollide	= (iMoveType == BT_SHOOT) && arg.pkBullet->_bEnableCollide && (arg.fCollidePosParamT > 1.0f);		// 服务器控制发射的子弹，服务器做碰撞检测（buddy、npc，非ActiveBuddy）

	if (arg.pkBullet->_bChangeTargetIndex)
	{
		object_id_type targetID = pkScriptCaller->GetLockedTargetID();
		LC_MapBase* pMapBase = pkScriptCaller->GetMap();
		LC_ActorBase* pTarget = (LC_ActorBase*)pMapBase->FindObject(targetID);
		if (pTarget)
			iTargetIndex = TARGET_LOCKED;
	}
	switch (iMoveType)
	{
		case BT_HOMINGMISSLE:
		case BT_SHOOT:
		case BT_MISSILE:
		case BT_TERMINAL_HOVER:
			{
				//根据目标锁定方式确定子弹运动
				switch (iTargetIndex)
				{
					case TARGET_NONE:		//朝向型
						{
							int lMaxMoveDistance = arg.pkBullet->_lMaxMoveDistance;

							//计算目标点的坐标
							const UT_Vec3Float & kForwardDir = arg.kShotDir;
							const UT_Vec3Int & kAttackerLocation = arg.kShotLocation;
							UT_Vec3Int kTargetLocation = UT_MathBase::V3dIncrementAdd(kAttackerLocation, kForwardDir, (float)lMaxMoveDistance);

							// 服务器做碰撞检测
							Utility::UT_PickResult kPickResult(0);
							if (bServerTestCollide)
							{
								UT_Vec3Float kShotRay = kForwardDir * Long2Float(lMaxMoveDistance);
								bool bCollided = _testBulletStraightLineSceneCollide(pkScriptCaller, 
									UT_MathBase::ConvertVec3IntToFloat(kAttackerLocation), kShotRay, 
									kPickResult);
								arg.fCollidePosParamT = kPickResult.clampped_t();
								arg.iCollideSurfaceMaterial = kPickResult.m_enmSurfaceMaterial;
							}
							float fParamT = arg.fCollidePosParamT;

							UT_Vec3Int kNewTargetLocation = UT_MathBase::V3dLerp(kAttackerLocation, kTargetLocation, fParamT);
							lMaxMoveDistance = (int)(lMaxMoveDistance * fParamT);
							arg.fMoveTime = Long2Float(lMaxMoveDistance) / fMoveSpeed;

							if ((iMoveType == BT_MISSILE || iMoveType == BT_CAST || iMoveType == BT_HOMINGMISSLE) 
								&& arg.fMoveTime < arg.pkBullet->_fMinExistTime)
							{
								arg.fMoveTime = arg.pkBullet->_fMinExistTime;
							}

							if (iMoveType == BT_HOMINGMISSLE || iMoveType == BT_TERMINAL_HOVER)
								arg.fMoveTime += arg.pkBullet->_fBulletHoverTime;

							_implshotBulletToLocation((LC_ActorBase*)pkScriptCaller, kNewTargetLocation, arg);
						}
						break;
					case TARGET_LOCATION:	//地点型
						{
							const UT_Vec3Int & kAttackerLocation = arg.kShotLocation;
							const UT_Vec3Int & kTargetLocation = arg.bClientBullet? arg.kDestLocation: pkScriptCaller->GetLockedLocation();
	
							// 服务器做碰撞检测
							Utility::UT_PickResult kPickResult(0);
							if (bServerTestCollide)
							{
								UT_Vec3Float kShotRay = UT_MathBase::ConvertVec3IntToFloat(kTargetLocation - kAttackerLocation);
								bool bCollided = _testBulletStraightLineSceneCollide(pkScriptCaller, 
									UT_MathBase::ConvertVec3IntToFloat(kAttackerLocation), kShotRay, 
									kPickResult);
								arg.fCollidePosParamT = kPickResult.clampped_t();
								arg.iCollideSurfaceMaterial = kPickResult.m_enmSurfaceMaterial;
							}
							float fParamT = arg.fCollidePosParamT;

							UT_Vec3Int kNewTargetLocation = UT_MathBase::V3dLerp(kAttackerLocation, kTargetLocation, fParamT);
							int lMoveDistance = UT_MathBase::LineLengthXYInt(kAttackerLocation, kNewTargetLocation);
							arg.fMoveTime = Long2Float(lMoveDistance) / fMoveSpeed;
							if ((iMoveType == BT_MISSILE || iMoveType == BT_CAST || iMoveType == BT_HOMINGMISSLE) 
								&& arg.fMoveTime < arg.pkBullet->_fMinExistTime)
							{
								arg.fMoveTime = arg.pkBullet->_fMinExistTime;
							}

							if (iMoveType == BT_HOMINGMISSLE || iMoveType == BT_TERMINAL_HOVER)
								arg.fMoveTime += arg.pkBullet->_fBulletHoverTime;
							
							_implshotBulletToLocation((LC_ActorBase*)pkScriptCaller, kNewTargetLocation, arg);
						}
						break;
					default:				//当做目标型处理
						{
							LC_ActorPtrListIter iter;
							LC_ActorPtrList kTargetList;
							_findTarget(pkScriptCaller, iTargetIndex, kTargetList);
							for (iter = kTargetList.begin(); iter != kTargetList.end(); ++iter)
							{
								LC_ActorBase* pkTarget = *iter;

								//计算子弹飞行时间
								const UT_Vec3Int & kAttackerLocation = arg.kShotLocation;
								UT_Vec3Int kTargetLocation;
								int nRetCode = pkTarget->GetCurrentLocationWithZ(kTargetLocation);

								// 服务器做碰撞检测
								Utility::UT_PickResult kPickResult(0);
								if (bServerTestCollide)
								{
									UT_Vec3Float kShotRay = UT_MathBase::ConvertVec3IntToFloat(kTargetLocation - kAttackerLocation);
									bool bCollided = _testBulletStraightLineSceneCollide(pkScriptCaller, 
										UT_MathBase::ConvertVec3IntToFloat(kAttackerLocation), kShotRay, 
										kPickResult);
									arg.fCollidePosParamT = kPickResult.clampped_t();
									arg.iCollideSurfaceMaterial = kPickResult.m_enmSurfaceMaterial;
								}
								float fParamT = arg.fCollidePosParamT;

								if (fParamT < 1.0f)	// 有碰撞，退化为Location
								{
									UT_Vec3Int kNewTargetLocation = UT_MathBase::V3dLerp(kAttackerLocation, kTargetLocation, fParamT);
									int lMoveDistance = UT_MathBase::LineLengthXYInt(kAttackerLocation, kNewTargetLocation);
									arg.fMoveTime = Long2Float(lMoveDistance) / fMoveSpeed;
									if ((iMoveType == BT_MISSILE || iMoveType == BT_CAST || iMoveType == BT_HOMINGMISSLE) 
										&& arg.fMoveTime < arg.pkBullet->_fMinExistTime)
									{
										arg.fMoveTime = arg.pkBullet->_fMinExistTime;
									}

									if (iMoveType == BT_HOMINGMISSLE || iMoveType == BT_TERMINAL_HOVER)
										arg.fMoveTime += arg.pkBullet->_fBulletHoverTime;

									_implshotBulletToLocation((LC_ActorBase*)pkScriptCaller, kNewTargetLocation, arg);
								}
								else
								{
									int lMoveDistance = UT_MathBase::LineLengthXYInt(kAttackerLocation, kTargetLocation);
									arg.fMoveTime = Long2Float(lMoveDistance) / fMoveSpeed;
									if ((iMoveType == BT_MISSILE || iMoveType == BT_CAST || iMoveType == BT_HOMINGMISSLE) 
										&& arg.fMoveTime < arg.pkBullet->_fMinExistTime)
									{
										arg.fMoveTime = arg.pkBullet->_fMinExistTime;
									}

									if (iMoveType == BT_HOMINGMISSLE || iMoveType == BT_TERMINAL_HOVER)
										arg.fMoveTime += arg.pkBullet->_fBulletHoverTime;
									
									_implShotBulletToTarget((LC_ActorBase*)pkScriptCaller, pkTarget->GetID(), kTargetLocation, arg);
								}
							}
						}
						break;
				}

			}
			break;

		case BT_CHAIN:
			{
				LC_ActorPtrListIter iter;
				LC_ActorPtrList kTargetList;
				_findTarget(pkScriptCaller, iTargetIndex, kTargetList);
				for (iter = kTargetList.begin(); iter != kTargetList.end(); ++iter)
				{
					LC_ActorBase* pkTarget = *iter;

					//计算子弹飞行时间
					const UT_Vec3Int & kAttackerLocation = arg.kShotLocation;
					//const UT_Vec3Int & kTargetLocation = pkTarget->GetCurrentLocationWithZ();
					UT_Vec3Int kTargetLocation;
					int nRetCode = pkTarget->GetCurrentLocationWithZ(kTargetLocation);
					int lMoveDistance = UT_MathBase::LineLengthXYInt(kAttackerLocation, kTargetLocation);
					arg.fMoveTime = Long2Float(lMoveDistance - pkTarget->GetProperty_ShapeRadius()) / fMoveSpeed;

					_implShotBulletToTarget((LC_ActorBase*)pkScriptCaller, pkTarget->GetID(), kTargetLocation, arg);
				}
			}
			break;

		case BT_CAST:
			{
				const UT_Vec3Int & kAttackerLocation = arg.kShotLocation;

				int lMinMoveDistance = arg.pkBullet->_lMinMoveDistance;
				int lMaxMoveDistance = arg.pkBullet->_lMaxMoveDistance;
				//根据目标锁定方式确定子弹运动
				switch (iTargetIndex)
				{
					case TARGET_NONE:		//朝向型
						{
							const UT_Vec3Float & kForwardDir = arg.kShotDir;
							UT_Vec3Int kTargetLocation = UT_MathBase::V3dIncrementAdd(kAttackerLocation, kForwardDir, (float)lMaxMoveDistance);

							//投掷型子弹Clamp子弹移动距离，确定最终的目标点
							kTargetLocation = UT_MathBase::MoveAlongIntClampXY(kAttackerLocation, kTargetLocation, (float)lMinMoveDistance, (float)lMaxMoveDistance);
							int lMoveDistance = UT_MathBase::LineLengthXYInt(kAttackerLocation, kTargetLocation);
							arg.fMoveTime = Long2Float(lMoveDistance) / fMoveSpeed;

							if ((iMoveType == BT_MISSILE || iMoveType == BT_CAST || iMoveType == BT_HOMINGMISSLE) 
								&& arg.fMoveTime < arg.pkBullet->_fMinExistTime)
							{
								arg.fMoveTime = arg.pkBullet->_fMinExistTime;
							}

							_implshotBulletToLocation((LC_ActorBase*)pkScriptCaller, kTargetLocation, arg);
						}
						break;
					case TARGET_LOCATION:	//地点型
						{
							//UT_Vec3Int kTargetLocation = pkScriptCaller->GetLockedLocation();
							UT_Vec3Int kTargetLocation = arg.kDestLocation;
							kTargetLocation.z = 0;

							//投掷型子弹Clamp子弹移动距离，确定最终的目标点
							kTargetLocation = UT_MathBase::MoveAlongIntClampXY(kAttackerLocation, kTargetLocation, (float)lMinMoveDistance, (float)lMaxMoveDistance);
							int lMoveDistance = UT_MathBase::LineLengthXYInt(kAttackerLocation, kTargetLocation);
							arg.fMoveTime = Long2Float(lMoveDistance) / fMoveSpeed;

							if ((iMoveType == BT_MISSILE || iMoveType == BT_CAST || iMoveType == BT_HOMINGMISSLE) 
								&& arg.fMoveTime < arg.pkBullet->_fMinExistTime)
							{
								arg.fMoveTime = arg.pkBullet->_fMinExistTime;
							}

							_implshotBulletToLocation((LC_ActorBase*)pkScriptCaller, kTargetLocation, arg);
						}
						break;
					default:				//当做目标型处理
						{
							LC_ActorPtrListIter iter;
							LC_ActorPtrList kTargetList;
							_findTarget(pkScriptCaller, iTargetIndex, kTargetList);
							for (iter = kTargetList.begin(); iter != kTargetList.end(); ++iter)
							{
								LC_ActorBase* pkTarget = *iter;
								//UT_Vec3Int kTargetLocation = pkTarget->GetCurrentLocationWithZ();
								UT_Vec3Int kTargetLocation;
								int nRetCode = pkTarget->GetCurrentLocationWithZ(kTargetLocation);

								//投掷型子弹Clamp子弹移动距离，确定最终的目标点
								kTargetLocation = UT_MathBase::MoveAlongIntClampXY(kAttackerLocation, kTargetLocation, (float)lMinMoveDistance, (float)lMaxMoveDistance);
								int lMoveDistance = UT_MathBase::LineLengthXYInt(kAttackerLocation, kTargetLocation);
								arg.fMoveTime = Long2Float(lMoveDistance) / fMoveSpeed;

								if ((iMoveType == BT_MISSILE || iMoveType == BT_CAST || iMoveType == BT_HOMINGMISSLE) 
									&& arg.fMoveTime < arg.pkBullet->_fMinExistTime)
								{
									arg.fMoveTime = arg.pkBullet->_fMinExistTime;
								}

								_implshotBulletToLocation((LC_ActorBase*)pkScriptCaller, kTargetLocation, arg);
							}
						}
						break;
				}
			}
			break;
		default:
			break;
	}
}
//------------------------------------------------------------
void SC_ServerSkillInterface::_implShotMultiBullet(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, mem::vector<ShotBulletArg>& argVec)
{
	if(argVec.empty())
	{
		return;
	}

	ShotBulletArg& arg = argVec[0];
	int iMoveType		= arg.pkBullet->_lMoveType;
	float fMoveSpeed	= Long2Float(arg.pkBullet->_lMoveSpeed);
	bool bServerTestCollide	= (iMoveType == BT_SHOOT) && arg.pkBullet->_bEnableCollide && (arg.fCollidePosParamT > 1.0f);		// 服务器控制发射的子弹，服务器做碰撞检测（buddy、npc，非ActiveBuddy）

	if (arg.pkBullet->_bChangeTargetIndex)
	{
		object_id_type targetID = pkScriptCaller->GetLockedTargetID();
		LC_MapBase* pMapBase = pkScriptCaller->GetMap();
		LC_ActorBase* pTarget = (LC_ActorBase*)pMapBase->FindObject(targetID);
		if (pTarget)
			iTargetIndex = TARGET_LOCKED;
	}

	switch (iMoveType)
	{
	case BT_HOMINGMISSLE:
	case BT_SHOOT:
	case BT_MISSILE:
	case BT_TERMINAL_HOVER:
		{
			//根据目标锁定方式确定子弹运动
			switch (iTargetIndex)
			{
			case TARGET_NONE:		//朝向型
				{
					//int lMaxMoveDistance = arg.pkBullet->_lMaxMoveDistance;

					mem::vector<UT_Vec3Int> NewTargetLocations;
					NewTargetLocations.reserve(argVec.size());
					for(int i=0; i<argVec.size(); ++i)
					{
						int lMaxMoveDistance = arg.pkBullet->_lMaxMoveDistance;

						ShotBulletArg& argInfo = argVec[i];
						//计算目标点的坐标
						const UT_Vec3Float & kForwardDir = argInfo.kShotDir;
						const UT_Vec3Int & kAttackerLocation = argInfo.kShotLocation;
						UT_Vec3Int kTargetLocation = UT_MathBase::V3dIncrementAdd(kAttackerLocation, kForwardDir, (float)lMaxMoveDistance);

						// 服务器做碰撞检测
						Utility::UT_PickResult kPickResult(0);
						if (bServerTestCollide)
						{
							UT_Vec3Float kShotRay = kForwardDir * Long2Float(lMaxMoveDistance);
							bool bCollided = _testBulletStraightLineSceneCollide(pkScriptCaller, 
								UT_MathBase::ConvertVec3IntToFloat(kAttackerLocation), kShotRay, kPickResult);
							argInfo.fCollidePosParamT = kPickResult.clampped_t();
							argInfo.iCollideSurfaceMaterial = kPickResult.m_enmSurfaceMaterial;
						}
						float fParamT = argInfo.fCollidePosParamT;

						UT_Vec3Int kNewTargetLocation = UT_MathBase::V3dLerp(kAttackerLocation, kTargetLocation, fParamT);
						lMaxMoveDistance = (int)(lMaxMoveDistance * fParamT);
						argInfo.fMoveTime = Long2Float(lMaxMoveDistance) / fMoveSpeed;

						if ((iMoveType == BT_MISSILE || iMoveType == BT_CAST || iMoveType == BT_HOMINGMISSLE) 
							&& argInfo.fMoveTime < argInfo.pkBullet->_fMinExistTime)
						{
							argInfo.fMoveTime = argInfo.pkBullet->_fMinExistTime;
						}

						if (iMoveType == BT_HOMINGMISSLE || iMoveType == BT_TERMINAL_HOVER)
						{
							argInfo.fMoveTime += argInfo.pkBullet->_fBulletHoverTime;
						}

						NewTargetLocations.push_back(kNewTargetLocation);
					}

					_implShotMultiBulletToLocation((LC_ActorBase*)pkScriptCaller, NewTargetLocations, argVec);
				}
				break;
			case TARGET_LOCATION:	//地点型
				{
					mem::vector<UT_Vec3Int> NewTargetLocations;
					NewTargetLocations.reserve(argVec.size());
					for(int i=0; i<argVec.size(); ++i)
					{
						ShotBulletArg& argInfo = argVec[i];
						const UT_Vec3Int & kAttackerLocation = argInfo.kShotLocation;
						const UT_Vec3Int & kTargetLocation = argInfo.bClientBullet? argInfo.kDestLocation: pkScriptCaller->GetLockedLocation();

						// 服务器做碰撞检测
						Utility::UT_PickResult kPickResult(0);
						if (bServerTestCollide)
						{
							UT_Vec3Float kShotRay = UT_MathBase::ConvertVec3IntToFloat(kTargetLocation - kAttackerLocation);
							bool bCollided = _testBulletStraightLineSceneCollide(pkScriptCaller, 
								UT_MathBase::ConvertVec3IntToFloat(kAttackerLocation), kShotRay, kPickResult);
							argInfo.fCollidePosParamT = kPickResult.clampped_t();
							argInfo.iCollideSurfaceMaterial = kPickResult.m_enmSurfaceMaterial;
						}
						float fParamT = argInfo.fCollidePosParamT;

						UT_Vec3Int kNewTargetLocation = UT_MathBase::V3dLerp(kAttackerLocation, kTargetLocation, fParamT);
						int lMoveDistance = UT_MathBase::LineLengthXYInt(kAttackerLocation, kNewTargetLocation);
						argInfo.fMoveTime = Long2Float(lMoveDistance) / fMoveSpeed;
						if ((iMoveType == BT_MISSILE || iMoveType == BT_CAST || iMoveType == BT_HOMINGMISSLE) 
							&& argInfo.fMoveTime < argInfo.pkBullet->_fMinExistTime)
						{
							argInfo.fMoveTime = argInfo.pkBullet->_fMinExistTime;
						}

						if (iMoveType == BT_HOMINGMISSLE || iMoveType == BT_TERMINAL_HOVER)
						{
							argInfo.fMoveTime += argInfo.pkBullet->_fBulletHoverTime;
						}

						NewTargetLocations.push_back(kNewTargetLocation);
					}

					_implShotMultiBulletToLocation((LC_ActorBase*)pkScriptCaller, NewTargetLocations, argVec);
				}
				break;
			default:				//当做目标型处理
				{
					LC_ActorPtrListIter iter;
					LC_ActorPtrList kTargetList;
					_findTarget(pkScriptCaller, iTargetIndex, kTargetList);
					for (iter = kTargetList.begin(); iter != kTargetList.end(); ++iter)
					{
						LC_ActorBase* pkTarget = *iter;

						mem::vector<ShotBulletArg> argVec1;
						mem::vector<ShotBulletArg> argVec2;

						mem::vector<UT_Vec3Int> NewTargetLocations;

						mem::vector<UT_Vec3Int> TargetLocations2;

						argVec1.reserve(argVec.size());
						argVec2.reserve(argVec.size());
						NewTargetLocations.reserve(argVec.size());
						TargetLocations2.reserve(argVec.size());

						for(int i=0; i<argVec.size(); ++i)
						{
							ShotBulletArg& argInfo = argVec[i];

							//计算子弹飞行时间
							const UT_Vec3Int & kAttackerLocation = argInfo.kShotLocation;
							UT_Vec3Int kTargetLocation;
							int nRetCode = pkTarget->GetCurrentLocationWithZ(kTargetLocation);

							// 服务器做碰撞检测
							Utility::UT_PickResult kPickResult(0);
							if (bServerTestCollide)
							{
								UT_Vec3Float kShotRay = UT_MathBase::ConvertVec3IntToFloat(kTargetLocation - kAttackerLocation);
								bool bCollided = _testBulletStraightLineSceneCollide(pkScriptCaller, 
									UT_MathBase::ConvertVec3IntToFloat(kAttackerLocation), kShotRay, kPickResult);
								argInfo.fCollidePosParamT = kPickResult.clampped_t();
								argInfo.iCollideSurfaceMaterial = kPickResult.m_enmSurfaceMaterial;
							}
							float fParamT = argInfo.fCollidePosParamT;

							if (fParamT < 1.0f)	// 有碰撞，退化为Location
							{
								UT_Vec3Int kNewTargetLocation = UT_MathBase::V3dLerp(kAttackerLocation, kTargetLocation, fParamT);
								int lMoveDistance = UT_MathBase::LineLengthXYInt(kAttackerLocation, kNewTargetLocation);
								argInfo.fMoveTime = Long2Float(lMoveDistance) / fMoveSpeed;
								if ((iMoveType == BT_MISSILE || iMoveType == BT_CAST || iMoveType == BT_HOMINGMISSLE) 
									&& argInfo.fMoveTime < argInfo.pkBullet->_fMinExistTime)
								{
									argInfo.fMoveTime = argInfo.pkBullet->_fMinExistTime;
								}

								if (iMoveType == BT_HOMINGMISSLE || iMoveType == BT_TERMINAL_HOVER)
								{
									argInfo.fMoveTime += argInfo.pkBullet->_fBulletHoverTime;
								}

								NewTargetLocations.push_back(kNewTargetLocation);
								argVec1.push_back(argInfo);
							}
							else
							{
								int lMoveDistance = UT_MathBase::LineLengthXYInt(kAttackerLocation, kTargetLocation);
								argInfo.fMoveTime = Long2Float(lMoveDistance) / fMoveSpeed;
								if ((iMoveType == BT_MISSILE || iMoveType == BT_CAST || iMoveType == BT_HOMINGMISSLE) 
									&& argInfo.fMoveTime < argInfo.pkBullet->_fMinExistTime)
								{
									argInfo.fMoveTime = argInfo.pkBullet->_fMinExistTime;
								}

								if (iMoveType == BT_HOMINGMISSLE || iMoveType == BT_TERMINAL_HOVER)
								{
									argInfo.fMoveTime += argInfo.pkBullet->_fBulletHoverTime;
								}

								TargetLocations2.push_back(kTargetLocation);		
								argVec2.push_back(argInfo);
							}
						}

						if(!argVec1.empty())
						{
							_implShotMultiBulletToLocation((LC_ActorBase*)pkScriptCaller, NewTargetLocations, argVec1);
						}

						if(!argVec2.empty())
						{
							_implShotMultiBulletToTarget((LC_ActorBase*)pkScriptCaller, pkTarget->GetID(), TargetLocations2, argVec2);
						}
					}
				}
				break;
			}

		}
		break;

	case BT_CHAIN:
		{
			LC_ActorPtrListIter iter;
			LC_ActorPtrList kTargetList;
			_findTarget(pkScriptCaller, iTargetIndex, kTargetList);
			for (iter = kTargetList.begin(); iter != kTargetList.end(); ++iter)
			{
				LC_ActorBase* pkTarget = *iter;

				mem::vector<UT_Vec3Int> TargetLocations;
				TargetLocations.reserve(argVec.size());
				for(int i=0; i<argVec.size(); ++i)
				{
					ShotBulletArg& argInfo = argVec[i];

					//计算子弹飞行时间
					const UT_Vec3Int & kAttackerLocation = argInfo.kShotLocation;
					//const UT_Vec3Int & kTargetLocation = pkTarget->GetCurrentLocationWithZ();
					UT_Vec3Int kTargetLocation;
					int nRetCode = pkTarget->GetCurrentLocationWithZ(kTargetLocation);
					int lMoveDistance = UT_MathBase::LineLengthXYInt(kAttackerLocation, kTargetLocation);
					argInfo.fMoveTime = Long2Float(lMoveDistance - pkTarget->GetProperty_ShapeRadius()) / fMoveSpeed;

					TargetLocations.push_back(kTargetLocation);
				}
				_implShotMultiBulletToTarget((LC_ActorBase*)pkScriptCaller, pkTarget->GetID(), TargetLocations, argVec);
			}
		}
		break;

	case BT_CAST:
		{
			const UT_Vec3Int & kAttackerLocation = arg.kShotLocation;

			int lMinMoveDistance = arg.pkBullet->_lMinMoveDistance;
			int lMaxMoveDistance = arg.pkBullet->_lMaxMoveDistance;
			//根据目标锁定方式确定子弹运动
			switch (iTargetIndex)
			{
			case TARGET_NONE:		//朝向型
				{
					mem::vector<UT_Vec3Int> TargetLocations;
					TargetLocations.reserve(argVec.size());
					for(int i=0; i<argVec.size(); ++i)
					{
						ShotBulletArg& argInfo = argVec[i];

						const UT_Vec3Float & kForwardDir = argInfo.kShotDir;
						UT_Vec3Int kTargetLocation = UT_MathBase::V3dIncrementAdd(kAttackerLocation, kForwardDir, (float)lMaxMoveDistance);

						//投掷型子弹Clamp子弹移动距离，确定最终的目标点
						kTargetLocation = UT_MathBase::MoveAlongIntClampXY(kAttackerLocation, kTargetLocation, (float)lMinMoveDistance, (float)lMaxMoveDistance);
						int lMoveDistance = UT_MathBase::LineLengthXYInt(kAttackerLocation, kTargetLocation);
						argInfo.fMoveTime = Long2Float(lMoveDistance) / fMoveSpeed;

						if ((iMoveType == BT_MISSILE || iMoveType == BT_CAST || iMoveType == BT_HOMINGMISSLE) 
							&& argInfo.fMoveTime < argInfo.pkBullet->_fMinExistTime)
						{
							argInfo.fMoveTime = argInfo.pkBullet->_fMinExistTime;
						}

						TargetLocations.push_back(kTargetLocation);
					}

					_implShotMultiBulletToLocation((LC_ActorBase*)pkScriptCaller, TargetLocations, argVec);
				}
				break;
			case TARGET_LOCATION:	//地点型
				{
					UT_Vec3Int kTargetLocation = pkScriptCaller->GetLockedLocation();

					mem::vector<UT_Vec3Int> TargetLocations;
					TargetLocations.reserve(argVec.size());
					for(int i=0; i<argVec.size(); ++i)
					{
						ShotBulletArg& argInfo = argVec[i];

						//投掷型子弹Clamp子弹移动距离，确定最终的目标点
						kTargetLocation = UT_MathBase::MoveAlongIntClampXY(kAttackerLocation, kTargetLocation, (float)lMinMoveDistance, (float)lMaxMoveDistance);
						int lMoveDistance = UT_MathBase::LineLengthXYInt(kAttackerLocation, kTargetLocation);
						argInfo.fMoveTime = Long2Float(lMoveDistance) / fMoveSpeed;

						if ((iMoveType == BT_MISSILE || iMoveType == BT_CAST || iMoveType == BT_HOMINGMISSLE) 
							&& argInfo.fMoveTime < argInfo.pkBullet->_fMinExistTime)
						{
							argInfo.fMoveTime = argInfo.pkBullet->_fMinExistTime;
						}

						TargetLocations.push_back(kTargetLocation);
					}

					_implShotMultiBulletToLocation((LC_ActorBase*)pkScriptCaller, TargetLocations, argVec);
				}
				break;
			default:				//当做目标型处理
				{
					LC_ActorPtrListIter iter;
					LC_ActorPtrList kTargetList;
					_findTarget(pkScriptCaller, iTargetIndex, kTargetList);
					for (iter = kTargetList.begin(); iter != kTargetList.end(); ++iter)
					{
						LC_ActorBase* pkTarget = *iter;
						//UT_Vec3Int kTargetLocation = pkTarget->GetCurrentLocationWithZ();
						mem::vector<UT_Vec3Int> TargetLocations;
						TargetLocations.reserve(argVec.size());
						for(int i=0; i<argVec.size(); ++i)
						{
							ShotBulletArg& argInfo = argVec[i];

							UT_Vec3Int kTargetLocation;
							int nRetCode = pkTarget->GetCurrentLocationWithZ(kTargetLocation);

							//投掷型子弹Clamp子弹移动距离，确定最终的目标点
							kTargetLocation = UT_MathBase::MoveAlongIntClampXY(kAttackerLocation, kTargetLocation, (float)lMinMoveDistance, (float)lMaxMoveDistance);
							int lMoveDistance = UT_MathBase::LineLengthXYInt(kAttackerLocation, kTargetLocation);
							argInfo.fMoveTime = Long2Float(lMoveDistance) / fMoveSpeed;

							if ((iMoveType == BT_MISSILE || iMoveType == BT_CAST || iMoveType == BT_HOMINGMISSLE) 
								&& argInfo.fMoveTime < argInfo.pkBullet->_fMinExistTime)
							{
								argInfo.fMoveTime = argInfo.pkBullet->_fMinExistTime;
							}

							TargetLocations.push_back(kTargetLocation);
						}

						_implShotMultiBulletToLocation((LC_ActorBase*)pkScriptCaller, TargetLocations, argVec);
					}
				}
				break;
			}
		}
		break;
	default:
		break;
	}
}

//------------------------------------------------------------
bool SC_ServerSkillInterface::ActiveBuddyReqBulletShot(GameLogic::LC_Attacker* pkScriptCaller, const MG_REQ_Bullet_Shot & req)
{
	int lSkillTypeID = req.m_lSkillTypeID;
	int iBulletFuncID= req.m_iBulletFuncIdx;
	int iTargetIndex = req.m_iTargetIdx;

	// 检测子弹
	const CSVFile::CF_SkillBulletFunc::DataEntry *	pkBullet = NULL;
	SK_SkillExecute* pkSkillExecute = _verify_skill_bullet(lSkillTypeID, iBulletFuncID, pkBullet);
	if (NULL == pkSkillExecute)
		return false;

	// 准备调用
	SC_ServerSkillInterface::ShotBulletArg arg;
	arg.pkSkillExecute	= pkSkillExecute;
	arg.pkBullet		= pkBullet;
	arg.lSkillTypeID	= req.m_lSkillTypeID;
	arg.iBulletFuncID	= req.m_iBulletFuncIdx;
	arg.iPenetrateCount = req.m_iPenetrateCount;
	arg.iDummyType		= req.m_iDummyType;
	arg.fStartTime      = GET_PROCESS_TIME;
	arg.fMoveTime		= 0;
	arg.kShotLocation   = UT_Vec3Int(req.m_lShotLocationX, req.m_lShotLocationY, req.m_lShotLocationZ);
	arg.kDestLocation	= UT_Vec3Int(req.m_lDestLocationX, req.m_lDestLocationY, req.m_lDestLocationZ);
	arg.bClientBullet	= true;
	arg.fPitchAngle		= req.m_fShotXRotAngle;

	// 朝不同方向进行射击
	int iScatteringAngle = req.m_iScatteringAngle;
	int iScatteringCnt   = req.m_iScatteringCount;
	// T_ASSERT(iScatteringCnt == req.m_fCollidePosParamT.size());
	// T_ASSERT(iScatteringCnt == req.m_iCollideSurfaceMaterial.size());
	float fScatteringDeltaAngle = iScatteringCnt == 1 ? 0 : Int2Float(iScatteringAngle) / Int2Float(iScatteringCnt - 1);
	for (int i = 0; i < iScatteringCnt; ++i)
	{
		float fRotZ = req.m_fShotZRotAngle + (i - (iScatteringCnt-1)/2.0f) * fScatteringDeltaAngle;
		//arg.kShotDir = UT_MathBase::GetXZRotDir(fRotZ, req.m_fShotXRotAngle);
		arg.kShotDir = UT_MathBase::GetXZRotDir(fRotZ, -1);

		arg.iClientLocalID	  = req.m_lBulletBeginID + i;
		arg.fCollidePosParamT = req.m_fCollidePosParamT[i];
		arg.iCollideSurfaceMaterial = req.m_iCollideSurfaceMaterial[i];
		_implShotBullet(pkScriptCaller, iTargetIndex, arg);
	}
	return true;
}

void SC_ServerSkillInterface::ShotBulletToTargetFromState(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lSkillTypeID, int iBulletFuncID, int iPenetrateCount, int iDummyType)
{
	// 检测调用者
	if (   !IS_PLAYER_CLASS_ID(pkScriptCaller->GetID())
		&& !IS_NPC_ID(pkScriptCaller->GetID()))
	{
		return;
	}


	// 检测子弹
	const CSVFile::CF_SkillBulletFunc::DataEntry *	pkBullet = NULL;
	SK_SkillExecute* pkSkillExecute = _verify_skill_bullet(lSkillTypeID, iBulletFuncID, pkBullet);
	if (NULL == pkSkillExecute)
		return;

	LC_ActorBase * pkActor = (LC_ActorBase*)pkScriptCaller;

	// 准备调用
	ShotBulletArg arg;
	arg.pkSkillExecute	= pkSkillExecute;
	arg.pkBullet		= pkBullet;
	arg.lSkillTypeID	= lSkillTypeID;
	arg.iBulletFuncID	= iBulletFuncID;
	arg.iPenetrateCount = iPenetrateCount;
	arg.iDummyType		= iDummyType;
	arg.fStartTime      = GET_PROCESS_TIME;
	arg.fMoveTime		= 0;
	//arg.kShotLocation   = pkScriptCaller->GetCurrentLocationWithZ();
	int nRetCode = pkScriptCaller->GetCurrentLocationWithZ(arg.kShotLocation );
	arg.kShotDir        = UT_MathBase::ConvertVec2To3Float(pkActor->GetForwardDir());
	arg.fPitchAngle		= pkBullet->_fPitchAngle;

	// 发射位置偏移
	UT_Vec3Float kShotLocalOffset = UT_MathBase::ConvertVec3IntToFloat(pkBullet->_kShotPosLocalOffset);
	UT_Matrix3Float rot;
	rot.MakeZRotation(pkActor->GetForwardDirAngle());
	UT_Vec3Float kShotWorldOffset = rot * kShotLocalOffset;
	arg.kShotLocation  += UT_MathBase::ConvertVec3FloatToInt(kShotWorldOffset);

	arg.iClientLocalID	  = 0;
	arg.fCollidePosParamT = c_fDefaultCollidePosParamT;
	arg.iCollideSurfaceMaterial = 0;
	_implShotBullet(pkScriptCaller, iTargetIndex, arg);
}

//------------------------------------------------------------
void SC_ServerSkillInterface::GenerateSkillDamage(GameLogic::LC_Attacker* pkScriptCaller, int iDamageTargetIndex, int lSkillTypeID, int iDamageRandomIndex, int iAttackRateRandomIndex, int iCriticalRateRandomIndex, object_id_type lDamageSourceActorID, int iHPCost, int iMPCost)
{
	_generateSkillDamageMultiple(pkScriptCaller, iDamageTargetIndex, lSkillTypeID, iDamageRandomIndex, iAttackRateRandomIndex, iCriticalRateRandomIndex, lDamageSourceActorID, iHPCost, iMPCost, 10000, 0,true);
}

void SC_ServerSkillInterface::GenerateSkillDamageMainTarget(GameLogic::LC_Attacker* pkScriptCaller, int iDamageTargetIndex, int lSkillTypeID, int iDamageRandomIndex, int iAttackRateRandomIndex, int iCriticalRateRandomIndex, object_id_type lDamageSourceActorID, int iHPCost, int iMPCost, int iPercent)
{
	_generateSkillDamageMultiple(pkScriptCaller, iDamageTargetIndex, lSkillTypeID, iDamageRandomIndex, iAttackRateRandomIndex, iCriticalRateRandomIndex, lDamageSourceActorID, iHPCost, iMPCost, 10000, iPercent,true);
}

void SC_ServerSkillInterface::GenerateSkillDamageMainTargetMulTiple( GameLogic::LC_Attacker* pkScriptCaller, int iDamageTargetIndex, int lSkillTypeID, int iDamageRandomIndex, int iAttackRateRandomIndex, int iCriticalRateRandomIndex, object_id_type lDamageSourceActorID, int iHPCost, int iMPCost, int Multiple , int iPercent )
{
	_generateSkillDamageMultiple(pkScriptCaller, iDamageTargetIndex, lSkillTypeID, iDamageRandomIndex, iAttackRateRandomIndex, iCriticalRateRandomIndex, lDamageSourceActorID, iHPCost, iMPCost, Multiple, iPercent,true);
}

void SC_ServerSkillInterface::GenerateSkillDamageNoScore( GameLogic::LC_Attacker* pkScriptCaller, int iDamageTargetIndex, int lSkillTypeID, int iDamageRandomIndex, int iAttackRateRandomIndex, int iCriticalRateRandomIndex, object_id_type lDamageSourceActorID, int iHPCost, int iMPCost)
{
	_generateSkillDamageMultiple(pkScriptCaller, iDamageTargetIndex, lSkillTypeID, iDamageRandomIndex, iAttackRateRandomIndex, iCriticalRateRandomIndex, lDamageSourceActorID, iHPCost, iMPCost, 10000, 0, false);
}

static int GetVehicleLevel(LC_ActorBase* actor)
{
	if (actor && IS_PLAYER_CLASS_ID(actor->GetID()))
	{
		LC_ServerPlayer* player = (LC_ServerPlayer*)actor;
		return player->GetVehicleLevel();
	}
	return 0;
}

void SC_ServerSkillInterface::GenerateSkillDamageMultiple(GameLogic::LC_Attacker* pkScriptCaller, int iDamageTargetIndex,	int lSkillTypeID, int iDamageRandomIndex, int iAttackRateRandomIndex, int iCriticalRateRandomIndex, object_id_type lDamageSourceActorID, int iHPCost, int iMPCost, int Multiple, int iPercent)
{
	_generateSkillDamageMultiple(pkScriptCaller, iDamageTargetIndex,  lSkillTypeID ,  iDamageRandomIndex, iAttackRateRandomIndex, iCriticalRateRandomIndex,lDamageSourceActorID, iHPCost,iMPCost, Multiple, iPercent,true);
}

void SC_ServerSkillInterface::GenerateSkillDamageMultipleWithPunchFlag(GameLogic::LC_Attacker* pkScriptCaller, int iDamageTargetIndex, int lSkillTypeID, int iDamageRandomIndex, int iAttackRateRandomIndex, int iCriticalRateRandomIndex, object_id_type lDamageSourceActorID, int iHPCost, int iMPCost, int Multiple, int iPercent, bool bDoPunch)
{
	_generateSkillDamageMultiple(pkScriptCaller, iDamageTargetIndex,  lSkillTypeID ,  iDamageRandomIndex, iAttackRateRandomIndex, iCriticalRateRandomIndex,lDamageSourceActorID, iHPCost,iMPCost, Multiple, iPercent, true, bDoPunch);
}

//------------------------------------------------------------
void SC_ServerSkillInterface::GenerateDamage(GameLogic::LC_Attacker* pkScriptCaller, int iDamageTargetIndex, int lDamageLogicTypeID, int lDamageType, attr_value_type iDamage, object_id_type lDamageSourceActorID)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	LC_ActorPtrList kDamageTargetsList;
	_findTarget(pkScriptCaller, iDamageTargetIndex, kDamageTargetsList);
	if (kDamageTargetsList.size() == 0)
	{
		return;
	}
	LC_Attacker* pkAttacker = NULL;
	if (pkScriptCaller->GetID() == lDamageSourceActorID)
	{
		pkAttacker = (LC_ActorBase*)pkScriptCaller;
	}
	else
	{
		LC_ActorPtrList actorList;
		_findTarget(pkScriptCaller, lDamageSourceActorID, actorList);
		if (actorList.size() > 0)
		{
			pkAttacker = actorList.front();
		}
	}
	LC_TargetMap* pkTargetMap = NULL;
	if (pkAttacker)
	{
		pkTargetMap = pkAttacker->GetTargetMap();
		pkTargetMap->ClearHitState();
	}
	else
	{
		return ;
	}
	LC_ActorBase* pkTarget = NULL;
	LC_ActorAttributeMap* pkTargetAttributeMap = NULL;
	object_id_type		  lTargetID;
	LC_ActorPtrListIter Iter;
	int   iTargetLevel;
	double fTargetHPCostRate;

	int iHitCount = 0;
	int64_t iDamgeCount = 0;

	TRAV(kDamageTargetsList, Iter)
	{
		pkTarget = *Iter;
		lTargetID = pkTarget->GetID();

		pkTargetAttributeMap = pkTarget->GetFinalAttributeMap();
		if (!pkTargetAttributeMap)
		{
			continue;
		}
		iTargetLevel = pkTarget->GetLevel();

		//获取消耗率
		fTargetHPCostRate = LC_Helper::GetAttributeRate(pkTarget, ATT_HP_COST_RATE);
		//真实伤害，所以这个伤害不能小于1点
		iDamage  = UT_MathBase::MaxInt64(1, Double2Long(fTargetHPCostRate * iDamage + 0.5f));

		//如果这个时候目标跳闪那就跳闪状态
		int hitState = SK_DamageEvent::SDR_HIT;
		bool bIsHit = true;
		if (IS_PLAYER_CLASS_ID(lTargetID) && pkTarget->IsJumpImmortal())
		{
			hitState = SK_DamageEvent::SDR_JUMP_DODGE;
			bIsHit = false;
		}

		if (hitState == SK_DamageEvent::SDR_HIT || hitState == SK_DamageEvent::SDR_CRITICAL)
		{
			if (lDamageType != SDT_DOT)
			{
				iHitCount++;
			}
			iDamgeCount += iDamage;
		}

		if (pkTargetMap)
		{
			pkTargetMap->PushTargetsHitState(lTargetID, bIsHit);
		}

		// 元素伤害
		iDamage *= FetchElementDamageData(pkAttacker->GetDamageElementType(), pkTarget->GetDamageElementType());
		iDamage *= FetchLevelFuckData((LC_ActorBase*)pkAttacker, (LC_ActorBase*)pkTarget);
		iDamage *= GetCombatYaZhiValue((LC_ActorBase*)pkAttacker, (LC_ActorBase*)pkTarget);
		iDamage *= FetchCommanderLevelFuckData((LC_ActorBase*)pkAttacker, (LC_ActorBase*)pkTarget);
		iDamage *= pkAttacker->GetFinalDamageRate();
		if ( pkTarget->GetStaticInputDamage() > 0 )
			iDamage = pkTarget->GetStaticInputDamage();
		//抛出伤害事件
		NotifyClientSete_Damage(pkAttacker->GetID(),lTargetID,lDamageLogicTypeID,lDamageType,hitState,iDamage,pkTarget->GetMapResID(),pkTarget->GetMapLogicID());
	}
}
//------------------------------------------------------------
void SC_ServerSkillInterface::GenerateDamage2(GameLogic::LC_Attacker* pkScriptCaller, int iDamageTargetIndex, int lDamageLogicTypeID, int lDamageType, attr_value_type iDamage, object_id_type lDamageSourceActorID)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	LC_ActorPtrList kDamageTargetsList;
	_findTarget(pkScriptCaller, iDamageTargetIndex, kDamageTargetsList);
	if (kDamageTargetsList.size() == 0)
	{
		return;
	}

	LC_Attacker* pkAttacker = NULL;
	if (pkScriptCaller->GetID() == lDamageSourceActorID)
	{
		pkAttacker = (LC_ActorBase*)pkScriptCaller;
	}
	else
	{
		pkAttacker = _findActor(lDamageSourceActorID);
	}
	if ( NULL == pkAttacker )
		return;

	LC_TargetMap*pkTargetMap = pkAttacker->GetTargetMap();
	if( NULL != pkTargetMap)
		pkTargetMap->ClearHitState();

	LC_ActorBase* pkTarget = NULL;
	LC_ActorAttributeMap* pkTargetAttributeMap = NULL;
	object_id_type		  lTargetID;
	LC_ActorPtrListIter Iter;
	int   iTargetLevel;

	TRAV(kDamageTargetsList, Iter)
	{
		pkTarget = *Iter;
		lTargetID = pkTarget->GetID();

		if ( IS_PLAYER_CLASS_ID(pkTarget->GetID()) )
		{
			LC_PlayerBase* pkPlayer = (LC_PlayerBase*)pkTarget;
			IF_A_NA_PLAYER( pkPlayer)
				continue ;
		}
		if ( pkTarget->GetMap() != pkAttacker->GetMap() )
			continue;


		pkTargetAttributeMap = pkTarget->GetFinalAttributeMap();
		if (!pkTargetAttributeMap)
		{
			continue;
		}
		iTargetLevel = pkTarget->GetLevel();

		//真实伤害，所以这个伤害不能小于1点
		iDamage = UT_MathBase::MaxInt64(1, iDamage);

		//如果这个时候目标跳闪那就跳闪状态
		int hitState = SK_DamageEvent::SDR_HIT;
		bool bIsHit = true;
		if (IS_PLAYER_CLASS_ID(lTargetID) && pkTarget->IsJumpImmortal())
		{
			hitState = SK_DamageEvent::SDR_JUMP_DODGE;
			bIsHit = false;
		}

		if (pkTargetMap)
		{
			pkTargetMap->PushTargetsHitState(lTargetID, bIsHit);
		}
		
		
		// 元素伤害
		iDamage *= FetchElementDamageData(pkAttacker->GetDamageElementType(), pkTarget->GetDamageElementType());
		iDamage *= FetchLevelFuckData((LC_ActorBase*)pkAttacker, (LC_ActorBase*)pkTarget);
		iDamage *= GetCombatYaZhiValue((LC_ActorBase*)pkAttacker, (LC_ActorBase*)pkTarget);
		iDamage *= FetchCommanderLevelFuckData((LC_ActorBase*)pkAttacker, (LC_ActorBase*)pkTarget);
		iDamage *= pkAttacker->GetFinalDamageRate();
		if ( pkTarget->GetStaticInputDamage() > 0 )
			iDamage = pkTarget->GetStaticInputDamage();
		//抛出伤害事件
		NotifyClientSete_Damage(lDamageSourceActorID,lTargetID,lDamageLogicTypeID,lDamageType,hitState,iDamage,pkTarget->GetMapResID(),pkTarget->GetMapLogicID());
	}
}
//------------------------------------------------------------
void SC_ServerSkillInterface::GenerateEliteDamage(GameLogic::LC_Attacker* pkScriptCaller, int iDamageTargetIndex, int lDamageLogicTypeID, int lDamageType, attr_value_type iDamage, object_id_type lDamageSourceActorID)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	LC_ActorPtrList kDamageTargetsList;
	_findTarget(pkScriptCaller, iDamageTargetIndex, kDamageTargetsList);
	if (kDamageTargetsList.size() == 0)
	{
		return;
	}

	LC_Attacker* pkAttacker = NULL;
	if (pkScriptCaller->GetID() == lDamageSourceActorID)
	{
		pkAttacker = (LC_ActorBase*)pkScriptCaller;
	}
	else
	{
		LC_ActorPtrList actorList;
		_findTarget(pkScriptCaller, lDamageSourceActorID, actorList);
		if (actorList.size() > 0)
		{
			pkAttacker = actorList.front();
		}
	}
	if(NULL == pkAttacker)
		return;
	LC_TargetMap* pkTargetMap = pkAttacker->GetTargetMap();
	if( NULL != pkTargetMap)
		pkTargetMap->ClearHitState();
	

	LC_ActorBase* pkTarget = NULL;
	LC_ActorAttributeMap* pkTargetAttributeMap = NULL;
	object_id_type		  lTargetID;
	LC_ActorPtrListIter Iter;
	int   iTargetLevel;
	double fTargetHPCostRate;

	TRAV(kDamageTargetsList, Iter)
	{
		pkTarget = *Iter;
		lTargetID = pkTarget->GetID();
		
		if ( IS_PLAYER_CLASS_ID(pkTarget->GetID()) )
		{
			LC_PlayerBase* pkPlayer = (LC_PlayerBase*)pkTarget;
			IF_A_NA_PLAYER( pkPlayer)
				continue ;
		}
		if ( pkTarget->GetMap() != pkAttacker->GetMap() )
			continue;

		pkTargetAttributeMap = pkTarget->GetFinalAttributeMap();
		if (!pkTargetAttributeMap)
		{
			continue;
		}
		iTargetLevel = pkTarget->GetLevel();
		attr_value_type iDmg = iDamage;
		//获取消耗率
		fTargetHPCostRate = LC_Helper::GetAttributeRate(pkTarget, ATT_HP_COST_RATE);
		//真实伤害，所以这个伤害不能小于1点
		//精英怪的技能需要计算防御力，所以特殊处理
		int64_t iDef = pkTargetAttributeMap->GetAttribute(ATT_DEF);
		double predamage = 0.0;
		if(iDmg + iDef != 0)
			predamage = UT_MathBase::MaxDouble(double(iDmg*iDmg / (iDmg + iDef)), iDmg * 0.05);
		iDmg  = UT_MathBase::MaxInt(1, Double2Long(fTargetHPCostRate * predamage + 0.5f));

		//如果这个时候目标跳闪那就跳闪状态
		int hitState = SK_DamageEvent::SDR_HIT;
		bool bIsHit = true;
		if (IS_PLAYER_CLASS_ID(lTargetID) && pkTarget->IsJumpImmortal())
		{
			hitState = SK_DamageEvent::SDR_JUMP_DODGE;
			bIsHit = false;
		}

		if (pkTargetMap)
		{
			pkTargetMap->PushTargetsHitState(lTargetID, bIsHit);
		}
		// 元素伤害
		iDmg *= FetchElementDamageData(pkAttacker->GetDamageElementType(), pkTarget->GetDamageElementType());
		iDmg *= FetchLevelFuckData((LC_ActorBase*)pkAttacker, (LC_ActorBase*)pkTarget);
		iDmg *= GetCombatYaZhiValue((LC_ActorBase*)pkAttacker, (LC_ActorBase*)pkTarget);
		iDmg *= FetchCommanderLevelFuckData((LC_ActorBase*)pkAttacker, (LC_ActorBase*)pkTarget);
		iDmg *= pkAttacker->GetFinalDamageRate();
		if ( pkTarget->GetStaticInputDamage() > 0 )
			iDmg = pkTarget->GetStaticInputDamage();
		//抛出伤害事件
		NotifyClientSete_Damage(pkAttacker->GetID(),lTargetID,lDamageLogicTypeID,lDamageType,hitState,iDmg,pkTarget->GetMapResID(),pkTarget->GetMapLogicID());
	}
}

void SC_ServerSkillInterface::DetachStateAndGenerateDamage(GameLogic::LC_Attacker* pActor, int targetIndex, int sourceIndex, int stateID, int skillID, int damage, int damageType, bool pure)
{
	if (NULL == pActor)
		return;

	LC_ActorBase * pSourcer = NULL;
	LC_ActorPtrList sourcePtrList;
	_findTarget(pActor, sourceIndex, sourcePtrList);
	if (sourcePtrList.size() != 1)
		return;
	pSourcer = sourcePtrList.front();

	LC_ActorPtrList targetPtrList;
	_findTarget(pActor, targetIndex, targetPtrList);
	if (targetPtrList.size() == 0)
		return;

	LC_ActorPtrListIter iter = targetPtrList.begin();
	int overlapIndex = 0;
	LC_ActorBase * pTarget = NULL;
	LC_ActorStateMap * pkStateMap = NULL;
	LC_ActorStateEntry * pkStateEntry = NULL;
	int basicDamage = 0;
	for(;iter != targetPtrList.end(); ++iter)
	{
		if (!CheckHaveSkillState(*iter, stateID))
			continue;
		
		if (NULL == *iter)
			continue;

		overlapIndex = 0;
		basicDamage = damage;
		pTarget = *iter;
		pkStateMap = pTarget->GetStateMap();
		pkStateEntry = pkStateMap->GetStateEntryByID(stateID);
		if (pkStateEntry)
		{
			overlapIndex = pkStateEntry->GetOverlapCount();
			pkStateMap->RemoveSkillStateInStateMap(stateID);				
			_handleDetachStateEvent(pSourcer->GetID(), pTarget->GetID(), 0, stateID, overlapIndex);
		}

		if (overlapIndex > 0)
		{
			basicDamage *= overlapIndex;
		}

		if (pure)
			GenerateDamage(pActor, pTarget->GetID(), skillID, damageType, basicDamage, pActor->GetID());
		else
			GenerateEliteDamage(pActor, pTarget->GetID(), skillID, damageType, basicDamage, pActor->GetID());
	}
}

//------------------------------------------------------------
void SC_ServerSkillInterface::GenerateDamageByHPPercent(GameLogic::LC_Attacker* pkScriptCaller, int iDamageTargetIndex, int lDamageLogicTypeID, int lDamageType, int iHPType, double fHPPercent, object_id_type lDamageSourceActorID)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	if (iHPType <= HPT_NONE || iHPType > HPT_CURRENT_HP || fHPPercent < 0.0f)
	{
		return;
	}

	CF_LevelUp* pkLevelUpList = CF_LevelUp::GetSingletonPtr();

	LC_ActorPtrList kDamageTargetsList;
	_findTarget(pkScriptCaller, iDamageTargetIndex, kDamageTargetsList);
	if (kDamageTargetsList.size() == 0)
	{
		return;
	}
	LC_Attacker* pkAttacker = NULL;
	if (pkScriptCaller->GetID() == lDamageSourceActorID)
	{
		pkAttacker = (LC_ActorBase*)pkScriptCaller;
	}
	else
	{
		pkAttacker = _findActor(lDamageSourceActorID);
	}

	if ( NULL == pkAttacker)
		return ;

	LC_ActorBase* pkTarget = NULL;
	LC_ActorAttributeMap* pkTargetAttributeMap = NULL;
	object_id_type		  lTargetID;
	LC_ActorPtrListIter Iter;
	int   iTargetLevel;

	TRAV(kDamageTargetsList, Iter)
	{
		pkTarget = *Iter;
		lTargetID = pkTarget->GetID();

		if ( IS_PLAYER_CLASS_ID(pkTarget->GetID()) )
		{
			LC_PlayerBase* pkPlayer = (LC_PlayerBase*)pkTarget;
			IF_A_NA_PLAYER( pkPlayer)
				continue ;
		}
		if ( pkTarget->GetMap() != pkAttacker->GetMap() )
			continue;


		pkTargetAttributeMap = pkTarget->GetFinalAttributeMap();
		if (!pkTargetAttributeMap)
		{
			continue;
		}
		iTargetLevel = pkTarget->GetLevel();

		attr_value_type iDamage = 0;
		switch (iHPType)
		{
			case HPT_LEVEL_HP:
				{
					CF_LevelUp::DataEntry* pkLevelUpData = pkLevelUpList->GetEntryPtr(pkTarget->GetLevel());
					if (NULL != pkLevelUpData)
					{
						iDamage = Double2Int64(pkLevelUpData->_lMaxHPAdd * fHPPercent);
					}
				}
				break;
			case HPT_MAX_HP:
				{
					LC_ActorAttributeMap* pkAttributeMap = pkTarget->GetFinalAttributeMap();
					if (NULL != pkAttributeMap)
					{
						iDamage = Double2Int64(pkAttributeMap->GetAttribute(ATT_MAX_HP) * fHPPercent);
					}
				}
				break;
			case HPT_CURRENT_HP:
				{
					iDamage = Double2Int64(pkTarget->GetHP() * fHPPercent);
				}
				break;
			default:
				break;
		}
		//抛出伤害事件
		NotifyClientSete_Damage(lDamageSourceActorID,lTargetID,lDamageLogicTypeID,lDamageType,SK_DamageEvent::SDR_HIT,iDamage,pkTarget->GetMapResID(),pkTarget->GetMapLogicID());
	}
}

void SC_ServerSkillInterface::GenerateDamageByAttributePercent(GameLogic::LC_Attacker* pCaller, int iTargetIndex, int iSkillID, int iDamageType, int iAttributeType, float fAttrPercent, int iSourcerIndex, bool bCountDef)
{
	if (NULL == pCaller)
	{
		return;
	}
	
	if (iAttributeType < ATT_MAX_HP || iAttributeType >= ATT_MAX_COUNT)
	{
		return;
	}

	LC_ActorPtrList actorList;
	_findTarget(pCaller, iSourcerIndex, actorList);
	if (actorList.size() <= 0)
	{
		return;
	}

	LC_ActorBase* pSourcer = actorList.front();
	LC_ActorAttributeMap* pAttributeMap = pSourcer->GetFinalAttributeMap();
	if (pAttributeMap == NULL)
	{
		return;
	}

	int dmg = pAttributeMap->GetAttribute(iAttributeType) * fAttrPercent;
	if (bCountDef)
	{
		GenerateEliteDamage(pCaller, iTargetIndex, iSkillID, iDamageType, dmg, iSourcerIndex);
	}
	else
	{
		GenerateDamage(pCaller, iTargetIndex, iSkillID, iDamageType, dmg, iSourcerIndex);
	}
}

//------------------------------------------------------------
void SC_ServerSkillInterface::CostTargetAbsHP(GameLogic::LC_ActorBase* pkScriptCaller, int iTargetIndex, int lSourceTypeID, int iCostValue)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	LC_ActorPtrListIter IterBe = kTargetsList.begin();
	LC_ActorPtrListIter IterEd = kTargetsList.end();
	LC_ActorBase* pkTarget = NULL;
	object_id_type lCallerID = pkScriptCaller->GetID();
	object_id_type lTargetID = 0;
	for (; IterBe != IterEd; ++IterBe)
	{
		pkTarget = (*IterBe);
		lTargetID = pkTarget->GetID();

		double fCostRate = LC_Helper::GetAttributeRate(pkTarget, ATT_HP_COST_RATE);
		iCostValue = Double2Long(iCostValue * fCostRate);

		_handleCostEvent(lCallerID, lTargetID, lSourceTypeID, SK_CostEvent::SCT_HP, iCostValue, 0, 0);
	}
}
//------------------------------------------------------------
void SC_ServerSkillInterface::CostTargetDefaultPercentHP(GameLogic::LC_ActorBase* pkScriptCaller, int iTargetIndex, int lSourceTypeID, float fCostPercent)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	CF_LevelUp* pkLevelUpList = CF_LevelUp::GetSingletonPtr();
	CF_LevelUp::DataEntry* pkLevelUpData = NULL;

	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	LC_ActorPtrListIter IterBe = kTargetsList.begin();
	LC_ActorPtrListIter IterEd = kTargetsList.end();
	LC_ActorBase* pkTarget = NULL;
	object_id_type lCallerID = pkScriptCaller->GetID();
	object_id_type lTargetID = 0;
	for (; IterBe != IterEd; ++IterBe)
	{
		pkTarget = (*IterBe);
		lTargetID = pkTarget->GetID();

		pkLevelUpData = pkLevelUpList->GetEntryPtr(pkTarget->GetLevel());
		if (NULL != pkLevelUpData)
		{
			double fCostRate = LC_Helper::GetAttributeRate(pkTarget, ATT_HP_COST_RATE);
			int32_t iCostValue = Double2Long(pkLevelUpData->_lMaxHPAdd * fCostRate * fCostPercent);

			_handleCostEvent(lCallerID, lTargetID, lSourceTypeID, SK_CostEvent::SCT_HP, iCostValue, 0, 0);
		}
	}
}
//------------------------------------------------------------
void SC_ServerSkillInterface::CostTargetAdditionPercentHP(GameLogic::LC_ActorBase* pkScriptCaller, int iTargetIndex, int lSourceTypeID, float fCostPercent)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	LC_ActorPtrListIter IterBe = kTargetsList.begin();
	LC_ActorPtrListIter IterEd = kTargetsList.end();
	LC_ActorBase* pkTarget = NULL;
	object_id_type lCallerID = pkScriptCaller->GetID();
	object_id_type lTargetID = 0;
	LC_ActorAttributeMap* pkAttributeMap = NULL;
	for (; IterBe != IterEd; ++IterBe)
	{
		pkTarget = (*IterBe);
		lTargetID = pkTarget->GetID();

		pkAttributeMap = pkTarget->GetFinalAttributeMap();
		if (NULL != pkAttributeMap)
		{
			attr_value_type iMaxHP = pkAttributeMap->GetAttribute(ATT_MAX_HP);
			double fCostRate = LC_Helper::GetAttributeRate(pkTarget, ATT_HP_COST_RATE);
			int iCostValue = Double2Long(iMaxHP * fCostRate * fCostPercent);

			_handleCostEvent(lCallerID, lTargetID, lSourceTypeID, SK_CostEvent::SCT_HP, iCostValue, 0, 0);
		}
	}
}
//------------------------------------------------------------
void SC_ServerSkillInterface::CostTargetAbsMP(GameLogic::LC_ActorBase* pkScriptCaller, int iTargetIndex, int lSourceTypeID, int iCostValue)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	LC_ActorPtrListIter IterBe = kTargetsList.begin();
	LC_ActorPtrListIter IterEd = kTargetsList.end();
	LC_ActorBase* pkTarget = NULL;
	object_id_type lCallerID = pkScriptCaller->GetID();
	object_id_type lTargetID = 0;
	for (; IterBe != IterEd; ++IterBe)
	{
		pkTarget = (*IterBe);
		lTargetID = pkTarget->GetID();

		double fCostRate = LC_Helper::GetAttributeRate(pkTarget, ATT_MP_COST_RATE);
		iCostValue = Double2Long(iCostValue * fCostRate);

		_handleCostEvent(lCallerID, lTargetID, lSourceTypeID, SK_CostEvent::SCT_MP, iCostValue, 0, 0);
	}
}
//------------------------------------------------------------
void SC_ServerSkillInterface::CostTargetDefaultPercentMP(GameLogic::LC_ActorBase* pkScriptCaller, int iTargetIndex, int lSourceTypeID, float fCostPercent)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	CF_LevelUp* pkLevelUpList = CF_LevelUp::GetSingletonPtr();
	CF_LevelUp::DataEntry* pkLevelUpData = NULL;

	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	LC_ActorPtrListIter IterBe = kTargetsList.begin();
	LC_ActorPtrListIter IterEd = kTargetsList.end();
	LC_ActorBase* pkTarget = NULL;
	object_id_type lCallerID = pkScriptCaller->GetID();
	object_id_type lTargetID = 0;
	for (; IterBe != IterEd; ++IterBe)
	{
		pkTarget = (*IterBe);
		lTargetID = pkTarget->GetID();

		pkLevelUpData = pkLevelUpList->GetEntryPtr(pkTarget->GetLevel());
		if (NULL != pkLevelUpData)
		{
			double fCostRate = LC_Helper::GetAttributeRate(pkTarget, ATT_MP_COST_RATE);
			int iCostValue = Double2Long(pkLevelUpData->_lMaxMPAdd * fCostRate * fCostPercent);

			_handleCostEvent(lCallerID, lTargetID, lSourceTypeID, SK_CostEvent::SCT_MP, iCostValue, 0, 0);
		}
	}
}
//------------------------------------------------------------
void SC_ServerSkillInterface::CostTargetAdditionPercentMP(GameLogic::LC_ActorBase* pkScriptCaller, int iTargetIndex, int lSourceTypeID, float fCostPercent)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	LC_ActorPtrListIter IterBe = kTargetsList.begin();
	LC_ActorPtrListIter IterEd = kTargetsList.end();
	LC_ActorBase* pkTarget = NULL;
	object_id_type lCallerID = pkScriptCaller->GetID();
	object_id_type lTargetID = 0;
	LC_ActorAttributeMap* pkAttributeMap = NULL;
	for (; IterBe != IterEd; ++IterBe)
	{
		pkTarget = (*IterBe);
		lTargetID = pkTarget->GetID();

		pkAttributeMap = pkTarget->GetFinalAttributeMap();
		if (NULL != pkAttributeMap)
		{
			attr_value_type iMaxMP = pkAttributeMap->GetAttribute(ATT_MAX_MP);
			double fCostRate = LC_Helper::GetAttributeRate(pkTarget, ATT_MP_COST_RATE);
			int iCostValue = Double2Long(iMaxMP * fCostRate * fCostPercent);

			_handleCostEvent(lCallerID, lTargetID, lSourceTypeID, SK_CostEvent::SCT_MP, iCostValue, 0, 0);
		}
	}
}
//------------------------------------------------------------
void SC_ServerSkillInterface::CostTargetSkillResource(GameLogic::LC_ActorBase* pkScriptCaller, int iTargetIndex, int lSkillTypeID)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	CF_SkillExecuteList* pkSkillList = CF_SkillExecuteList::GetSingletonPtr();
	CF_SkillExecuteList::DataEntry* pkSkillData = pkSkillList->GetEntryPtr(GET_SKILL_INDEX(lSkillTypeID));
	if (NULL == pkSkillData)
	{
		return;
	}

	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	LC_ActorPtrListIter iter;
	LC_ActorBase* pkTarget = NULL;
	TRAV(kTargetsList, iter)
	{
		pkTarget = *iter;
		if (IS_PLAYER_CLASS_ID(pkTarget->GetID()))
		{
			LC_Helper::CostSkillResource((LC_PlayerBase*)pkTarget, pkSkillData);
		}
	}
}
//激活技能
int SC_ServerSkillInterface::ActivateSkill(GameLogic::LC_Attacker* pkScriptCaller, int nBuddyId, int lSkillTypeID)
{
	int nResult = -1;
	if (pkScriptCaller == NULL)
		return nResult;
	if (pkScriptCaller->GetType() != LOT_PLAYER)
		return nResult;
	LC_ServerPlayer* pPlayer = (LC_ServerPlayer*)pkScriptCaller;
	int nErr = 0;
	nResult = pPlayer->ActivateSkill(nBuddyId, lSkillTypeID, nErr);

	return nResult;
}

//获取技能激活状态
int SC_ServerSkillInterface::CheckActivateSkill(GameLogic::LC_Attacker* pkScriptCaller, int lSkillTypeID)
{
	int nResult = -1;
	if (pkScriptCaller == NULL)
		return nResult;
	if (pkScriptCaller->GetType() != LOT_PLAYER)
		return nResult;
	LC_ServerPlayer* pPlayer = (LC_ServerPlayer*)pkScriptCaller;
	BuddyInstance* pBuddy = pPlayer->GetAssignedBuddyInstance();
	if (pBuddy == NULL)
		return nResult;
	nResult = pBuddy->CheckActivateSkill(lSkillTypeID);

	return nResult;
}
//------------------------------------------------------------
void SC_ServerSkillInterface::LearnSkill(GameLogic::LC_Attacker* pkScriptCaller, int lSkillTypeID)
{
	if (pkScriptCaller == NULL)
	{
		return;
	}
	object_id_type lCallerID = pkScriptCaller->GetID();
	if (!IS_PLAYER_CLASS_ID(lCallerID))
	{
		return;
	}

	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkScriptCaller;
	int iSlotIndex = -1;
	pkPlayer->LearnSkill(lSkillTypeID, iSlotIndex);
}

void SC_ServerSkillInterface::SetSkillUsableCount(GameLogic::LC_Attacker* pAttacker, int targetIndex, int skillID, int usableCount)
{
	LC_ActorPtrList targetList;
	_findTarget(pAttacker, targetIndex, targetList);
	LC_ActorPtrListIter iter = targetList.begin();
	LC_ActorPtrListIter iterEnd = targetList.end();
	LC_ActorBase* player = NULL;
	SC_ServerDebugInterface* pkDebugInterface = SC_ServerScriptInterface::GetSingletonPtr()->GetDebugInterface();
	if (NULL == pkDebugInterface)
		return;

	for (; iter != iterEnd; ++iter)
	{
		player = *iter;
		pkDebugInterface->ModifySkillUsableCount(player->GetID(), player->GetCharType(), skillID, usableCount);
	}
}

//------------------------------------------------------------
void SC_ServerSkillInterface::LearnFormula(GameLogic::LC_Attacker* pkScriptCaller, int lFoumulaID)
{
	return;
}
//------------------------------------------------------------------------------
void SC_ServerSkillInterface::NotifyClientEyeSkill(GameLogic::LC_Attacker* pkScriptCaller, int skillID)
{
	//抛出伤害事件
	SK_ServerSkillEventManager* pkEventManager = SERVER_GET_SKILL_EVENT_MANAGER();
	SK_SkillEvent* pkSkillEvent = pkEventManager->AllocEvent();
	if (NULL != pkSkillEvent && pkScriptCaller)
	{
		pkSkillEvent->SetEventType(SETE_CLIENT_EYE_SKILL);
		pkSkillEvent->SetSrcActorID(pkScriptCaller->GetID());
		pkSkillEvent->SetDstActorID(pkScriptCaller->GetLockedTargetID());
		pkSkillEvent->SetLogicID(skillID);
		pkSkillEvent->SetParam(SK_ClientEyeSkillEvent::SEP_SKILL_ID, skillID);
		pkEventManager->AddEvent(pkSkillEvent);
	}
}

void SC_ServerSkillInterface::AttachGfx(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iDummyType, const StringType& strFxFileName, bool bStayStatic, bool bApplyDirection, object_id_type lSrcActorID, int sendType, int iLCGfx, int iBindMode)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	AttachTargetGfxImp(pkScriptCaller, iTargetIndex, iDummyType, strFxFileName, bStayStatic, bApplyDirection, lSrcActorID, sendType, iLCGfx, iBindMode);
}

//------------------------------------------------------------------------------
void SC_ServerSkillInterface::LearnVehicle(GameLogic::LC_Attacker* pkScripterCaller, int lVehicleSkillID)
{
	return;
}
//------------------------------------------------------------
void SC_ServerSkillInterface::AttachTargetGfx(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iDummyType, const StringType& strFxFileName, bool bStayStatic)
{
	if(NULL == pkScriptCaller)
	{
		return;
	}
	AttachTargetGfxEx(pkScriptCaller, iTargetIndex, iDummyType, strFxFileName, bStayStatic, false, pkScriptCaller->GetID() , SSET_BROADCAST_TO_AREA);
}
//------------------------------------------------------------
void SC_ServerSkillInterface::AttachTargetGfxSingleSend(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iDummyType, const StringType& strFxFileName, bool bStayStatic)
{
	if(NULL == pkScriptCaller)
	{
		return;
	}
	AttachTargetGfxEx(pkScriptCaller, iTargetIndex, iDummyType, strFxFileName, bStayStatic, false, pkScriptCaller->GetID(), SSET_SEND);
}
//------------------------------------------------------------
void SC_ServerSkillInterface::AttachTargetGfxEx(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iDummyType, const StringType& strFxFileName, bool bStayStatic, bool bApplyDirection, object_id_type lSrcActorID, int sendType)
{
	if (NULL == pkScriptCaller || INVALID_ACTOR_ID == lSrcActorID)
	{
		return;
	}
	
	AttachTargetGfxImp(pkScriptCaller, iTargetIndex, iDummyType, strFxFileName, bStayStatic, bApplyDirection, lSrcActorID, sendType, 0);
}

//-----------------------------------------------------------
void SC_ServerSkillInterface::AttachTargetGfxImp(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iDummyType, const StringType& strFxFileName, bool bStayStatic, bool bApplyDirection, object_id_type lSrcActorID, int sendType, int iLCGfx, int iBindMode)
{
	if (NULL == pkScriptCaller || INVALID_ACTOR_ID == lSrcActorID)
	{
		return;
	}

	int iMaxGfxNameLength = (MAX_SKILL_EVENT_PARAM_COUNT - SK_AttachGfxEvent::SEP_GFX_NAME_START) * sizeof(int);
	if ((int)strFxFileName.length() >= iMaxGfxNameLength)
	{
		return;
	}

	//抛出附加特效事件
	SK_ServerSkillEventManager* pkEventManager = SERVER_GET_SKILL_EVENT_MANAGER();
	SK_SkillEvent* pkSkillEvent = NULL;

	//是否保持静止
	int lStayStatic = bStayStatic ? 1 : 0;
	//是否需要方向
	int lApplyDirection = bApplyDirection ? 1 : 0;

	//将特效绑定到地点
	if (TARGET_LOCATION == iTargetIndex)
	{
		pkSkillEvent = pkEventManager->AllocEvent();
		if (NULL != pkSkillEvent)
		{
			//T_ASSERT_CRITICAL_S(lSrcActorID != 0);	//if lSrcActorID is 0, event won't be processed
			pkSkillEvent->SetEventType(SETE_ATTACH_GFX);
			pkSkillEvent->SetSrcActorID(lSrcActorID);
			pkSkillEvent->SetDstActorID(0);
			pkSkillEvent->SetLogicID(0);

			UT_Vec3Int kTargetLocation = pkScriptCaller->GetLockedLocation();
			pkSkillEvent->SetParam(SK_AttachGfxEvent::SEP_TARGET_LOCATION_X, kTargetLocation.x);
			pkSkillEvent->SetParam(SK_AttachGfxEvent::SEP_TARGET_LOCATION_Y, kTargetLocation.y);
			pkSkillEvent->SetParam(SK_AttachGfxEvent::SEP_DUMMY_TYPE, iDummyType);
			pkSkillEvent->SetParam(SK_AttachGfxEvent::SEP_STAY_STATIC, lStayStatic);
			pkSkillEvent->SetParam(SK_AttachGfxEvent::SEP_APPLY_DIRECTION, lApplyDirection);
			pkSkillEvent->SetParam(SK_AttachGfxEvent::SEP_GFX_NAME_START, strFxFileName);
			pkSkillEvent->SetParam(SK_AttachGfxEvent::SEP_GFX_LC, iLCGfx);
			pkSkillEvent->SetParam(SK_AttachGfxEvent::SEP_BIND_MODE, iBindMode);
			pkSkillEvent->SetSendOrBroadCast((SendSkillEventType)sendType);
			pkEventManager->AddEvent(pkSkillEvent);
		}
	}
	else
	{
		LC_ActorPtrList kTargetsList;
		_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
		LC_ActorPtrListIter IterBe = kTargetsList.begin();
		LC_ActorPtrListIter IterEd = kTargetsList.end();

		for (; IterBe != IterEd; ++IterBe)
		{
			pkSkillEvent = pkEventManager->AllocEvent();
			if (NULL != pkSkillEvent && (*IterBe))
			{
				pkSkillEvent->SetEventType(SETE_ATTACH_GFX);
				pkSkillEvent->SetSrcActorID(lSrcActorID);
				pkSkillEvent->SetDstActorID((*IterBe)->GetID());
				pkSkillEvent->SetLogicID(0);
				pkSkillEvent->SetParam(SK_AttachGfxEvent::SEP_DUMMY_TYPE, iDummyType);
				pkSkillEvent->SetParam(SK_AttachGfxEvent::SEP_STAY_STATIC, lStayStatic);
				pkSkillEvent->SetParam(SK_AttachGfxEvent::SEP_APPLY_DIRECTION, lApplyDirection);
				pkSkillEvent->SetParam(SK_AttachGfxEvent::SEP_GFX_NAME_START, strFxFileName);
				pkSkillEvent->SetParam(SK_AttachGfxEvent::SEP_GFX_LC, iLCGfx);
				pkSkillEvent->SetParam(SK_AttachGfxEvent::SEP_BIND_MODE, iBindMode);
				pkSkillEvent->SetSendOrBroadCast((SendSkillEventType)sendType);
				pkEventManager->AddEvent(pkSkillEvent);
			}
		}
	}
}

void SC_ServerSkillInterface::AttachTargetLCGfx(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iDummyType, const StringType& strFxFileName, bool bStayStatic, bool bApplyDirection, object_id_type lSrcActorID, int sendType, int iLCGfx)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	AttachTargetGfxImp(pkScriptCaller, iTargetIndex, iDummyType, strFxFileName, bStayStatic, bApplyDirection, lSrcActorID, sendType, iLCGfx);
}

//------------------------------------------------------------
void SC_ServerSkillInterface::AttachTargetDisplayChest(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iItemTypeID, object_id_type lSrcActorID, int dummyType, int flyType)
{
	if (NULL == pkScriptCaller || INVALID_ACTOR_ID == lSrcActorID)
	{
		return;
	}

	//抛出附加特效事件
	SK_ServerSkillEventManager* pkEventManager = SERVER_GET_SKILL_EVENT_MANAGER();
	SK_SkillEvent* pkSkillEvent = NULL;

	//将特效绑定到地点
	if (TARGET_LOCATION == iTargetIndex)
	{
		pkSkillEvent = pkEventManager->AllocEvent();
		if (NULL != pkSkillEvent)
		{
			//T_ASSERT_CRITICAL_S(lSrcActorID != 0);	//if lSrcActorID is 0, event won't be processed
			pkSkillEvent->SetEventType(SETE_DISPLAY_CHEST);
			pkSkillEvent->SetSrcActorID(lSrcActorID);
			pkSkillEvent->SetDstActorID(lSrcActorID);
			pkSkillEvent->SetLogicID(0);

			UT_Vec3Int kTargetLocation = pkScriptCaller->GetLockedLocation();
			pkSkillEvent->SetParam(SK_DisplayChestEvent::SEP_TARGET_LOCATION_X, kTargetLocation.x);
			pkSkillEvent->SetParam(SK_DisplayChestEvent::SEP_TARGET_LOCATION_Y, kTargetLocation.y);
			pkSkillEvent->SetParam(SK_DisplayChestEvent::SEP_DISPLAY_CHEST_ID, iItemTypeID);
			pkSkillEvent->SetParam(SK_DisplayChestEvent::SEP_DUMMY_TYPE, dummyType);
			pkSkillEvent->SetParam(SK_DisplayChestEvent::SEP_FLY_TYPE, flyType);
			pkEventManager->AddEvent(pkSkillEvent);
		}
	}
	else
	{
		LC_ActorPtrList kTargetsList;
		_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
		LC_ActorPtrListIter IterBe = kTargetsList.begin();
		LC_ActorPtrListIter IterEd = kTargetsList.end();

		for (; IterBe != IterEd; ++IterBe)
		{
			pkSkillEvent = pkEventManager->AllocEvent();
			if (NULL != pkSkillEvent && (*IterBe))
			{
				pkSkillEvent->SetEventType(SETE_DISPLAY_CHEST);
				pkSkillEvent->SetSrcActorID(lSrcActorID);
				pkSkillEvent->SetDstActorID((*IterBe)->GetID());
				pkSkillEvent->SetLogicID(0);
				pkSkillEvent->SetParam(SK_DisplayChestEvent::SEP_TARGET_LOCATION_X, (*IterBe)->GetCurrentLocation().x);
				pkSkillEvent->SetParam(SK_DisplayChestEvent::SEP_TARGET_LOCATION_Y, (*IterBe)->GetCurrentLocation().y);
				pkSkillEvent->SetParam(SK_DisplayChestEvent::SEP_DISPLAY_CHEST_ID, iItemTypeID);
				pkSkillEvent->SetParam(SK_DisplayChestEvent::SEP_DUMMY_TYPE, dummyType);
				pkSkillEvent->SetParam(SK_DisplayChestEvent::SEP_FLY_TYPE, flyType);
				pkEventManager->AddEvent(pkSkillEvent);
			}
		}
	}
}
//------------------------------------------------------------
void SC_ServerSkillInterface::DetachTargetGfxServer(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iDummyType, const StringType& strFxFileName, int broadCastType)
{
	if (!IS_PLAYER_CLASS_ID(pkScriptCaller->GetID()))
	{
		return;
	}

	//抛出附加特效事件
	SK_ServerSkillEventManager* pkEventManager = SERVER_GET_SKILL_EVENT_MANAGER();
	SK_SkillEvent* pkSkillEvent = NULL;

	//将特效绑定到地点
	if (TARGET_LOCATION != iTargetIndex)
	{
		LC_ActorPtrList kTargetsList;
		_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
		LC_ActorPtrListIter IterBe = kTargetsList.begin();
		LC_ActorPtrListIter IterEd = kTargetsList.end();

		for (; IterBe != IterEd; ++IterBe)
		{
			pkSkillEvent = pkEventManager->AllocEvent();
			if (NULL != pkSkillEvent && (*IterBe))
			{
				pkSkillEvent->SetEventType(SETE_DETACH_GFX);
				pkSkillEvent->SetSrcActorID((*IterBe)->GetID());
				pkSkillEvent->SetDstActorID((*IterBe)->GetID());
				pkSkillEvent->SetLogicID(0);
				pkSkillEvent->SetParam(SK_DetachGfxEvent::SEP_DUMMY_TYPE, iDummyType);
				pkSkillEvent->SetParam(SK_DetachGfxEvent::SEP_BROCAST_TYPE, broadCastType);
				pkSkillEvent->SetParam(SK_DetachGfxEvent::SEP_GFX_NAME_START, strFxFileName);
				pkEventManager->AddEvent(pkSkillEvent);
			}
		}
	}
}
//------------------------------------------------------------
void SC_ServerSkillInterface::AttachTargetSound(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, const StringType& kFileName, object_id_type lSrcActorID, object_id_type lLogicID, int iSoundChannel, int iSoundType, int iBroadCastType)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	if (iSoundType < SK_AttachSoundEvent::SST_2D_SINGLE_SOUND || iSoundType > SK_AttachSoundEvent::SST_2D_NPCSOUND_SEQ)
	{
		return;
	}

	int iMaxNameLength = (MAX_SKILL_EVENT_PARAM_COUNT - SK_AttachSoundEvent::SEP_SOUND_NAME_START) * sizeof(int);
	if ((int)kFileName.length() >= iMaxNameLength)
	{
		return;
	}

	//抛出附加特效事件
	SK_ServerSkillEventManager* pkEventManager = SERVER_GET_SKILL_EVENT_MANAGER();
	SK_SkillEvent* pkSkillEvent = NULL;

	//将特效绑定到地点
	if (TARGET_LOCATION == iTargetIndex)
	{
		pkSkillEvent = pkEventManager->AllocEvent();
		if (NULL != pkSkillEvent)
		{
			pkSkillEvent->SetEventType(SETE_ATTACH_SOUND);
			pkSkillEvent->SetSrcActorID(lSrcActorID);
			pkSkillEvent->SetDstActorID(0);
			pkSkillEvent->SetLogicID(lLogicID);

			UT_Vec3Int kTargetLocation = pkScriptCaller->GetLockedLocation();
			pkSkillEvent->SetParam(SK_AttachSoundEvent::SEP_TARGET_LOCATION_X, kTargetLocation.x);
			pkSkillEvent->SetParam(SK_AttachSoundEvent::SEP_TARGET_LOCATION_Y, kTargetLocation.y);
			pkSkillEvent->SetParam(SK_AttachSoundEvent::SEP_SOUND_CHANNEL_TYPE, iSoundChannel);
			pkSkillEvent->SetParam(SK_AttachSoundEvent::SEP_SOUND_TYPE, iSoundType);
			pkSkillEvent->SetParam(SK_AttachSoundEvent::SEP_SOUND_NAME_BROADCAST_TYPE, iBroadCastType);
			pkSkillEvent->SetParam(SK_AttachSoundEvent::SEP_SOUND_NAME_START, kFileName);
			pkEventManager->AddEvent(pkSkillEvent);
		}
	}
	else
	{
		LC_ActorPtrList kTargetsList;
		_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
		LC_ActorPtrListIter IterBe = kTargetsList.begin();
		LC_ActorPtrListIter IterEd = kTargetsList.end();

		for (; IterBe != IterEd; ++IterBe)
		{
			pkSkillEvent = pkEventManager->AllocEvent();
			if (NULL != pkSkillEvent && (*IterBe))
			{
				pkSkillEvent->SetEventType(SETE_ATTACH_SOUND);
				pkSkillEvent->SetSrcActorID(lSrcActorID);
				pkSkillEvent->SetDstActorID((*IterBe)->GetID());
				pkSkillEvent->SetLogicID(lLogicID);
				pkSkillEvent->SetParam(SK_AttachSoundEvent::SEP_TARGET_LOCATION_X, (*IterBe)->GetCurrentLocation().x);
				pkSkillEvent->SetParam(SK_AttachSoundEvent::SEP_TARGET_LOCATION_Y, (*IterBe)->GetCurrentLocation().y);
				pkSkillEvent->SetParam(SK_AttachSoundEvent::SEP_SOUND_CHANNEL_TYPE, iSoundChannel);
				pkSkillEvent->SetParam(SK_AttachSoundEvent::SEP_SOUND_TYPE, iSoundType);
				pkSkillEvent->SetParam(SK_AttachSoundEvent::SEP_SOUND_NAME_BROADCAST_TYPE, iBroadCastType);
				pkSkillEvent->SetParam(SK_AttachSoundEvent::SEP_SOUND_NAME_START, kFileName);
				pkEventManager->AddEvent(pkSkillEvent);
			}
		}
	}
}
//------------------------------------------------------------
void SC_ServerSkillInterface::RespawnTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int64_t iHP, double fHPPercent, int64_t iMP, double fMPPercent)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	object_id_type lCallerID = pkScriptCaller->GetID();
	if (!IS_PLAYER_ID(lCallerID))
	{
		return;
	}

	LC_PlayerBase* pkActor = (LC_PlayerBase*)pkScriptCaller;
	StringType kCallerName = pkActor->GetOwnerCharName();

	MG_REQ_SkillRespawn reqMsg;
	reqMsg.m_lRespawnSourcePlayerID = lCallerID;
	TPSTR2STDSTR(kCallerName, reqMsg.m_strRespawnSourcePlayerName);

	LC_ActorPtrList kTargetsList;
	_findTarget(pkActor, iTargetIndex, kTargetsList);
	LC_ActorPtrListIter IterBe = kTargetsList.begin();
	LC_ActorPtrListIter IterEd = kTargetsList.end();

	LC_ActorBase* pkTarget = NULL;
	object_id_type lTargetID;
	for (; IterBe != IterEd; ++IterBe)
	{
		pkTarget = (*IterBe);
		if (NULL == pkTarget)
		{
			continue;
		}
		lTargetID = pkTarget->GetID();
		if (!IS_PLAYER_ID(lTargetID))
		{
			continue;
		}

		LC_ActorAttributeMap* pkAttributeMap = pkTarget->GetFinalAttributeMap();
		attr_value_type lMaxHP = pkAttributeMap->GetAttribute(ATT_MAX_HP);
		attr_value_type lMaxMP = pkAttributeMap->GetAttribute(ATT_MAX_MP);

		int64_t lHPRecover = iHP + Double2Int64(lMaxHP * fHPPercent);
		int64_t lMPRecover = iMP + Double2Int64(lMaxMP * fMPPercent);

		//发送复活请求
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkTarget;
		pkPlayer->SetSkillRespawnSourcePlayerID(lCallerID);
		pkPlayer->SetSkillRespawnHP(lHPRecover);
		pkPlayer->SetSkillRespawnMP((int32_t)lMPRecover);
		pkPlayer->SetSkillFlag(SK_SKILL_RESPAWN, true);

		pkPlayer->SendMsgToClient(MGPT_REQ_SKILL_RESPAWN, &reqMsg);
	}
}
//------------------------------------------------------------
bool SC_ServerSkillInterface::IsAttackHit(GameLogic::LC_Attacker* pkScriptCaller, int iDamageTargetIndex, int lSkillTypeID, int iAttackRateRandomIndex)
{
	if (NULL == pkScriptCaller)
	{
		return false;
	}

	object_id_type lAttackerID = pkScriptCaller->GetID();
	if (!IS_PLAYER_CLASS_ID(lAttackerID) && !IS_NPC_ID(lAttackerID))
	{
		return false;
	}

	LC_ActorPtrList kDamageTargetsList;
	_findTarget(pkScriptCaller, iDamageTargetIndex, kDamageTargetsList);
	if (kDamageTargetsList.size() == 0)
	{
		return false;
	}
	LC_ActorBase* pkAttacker = (LC_ActorBase*)pkScriptCaller;
	LC_ActorBase* pkTarget = kDamageTargetsList.front();
	LC_ActorAttributeMap* pkScriptCallerAttributeMap = pkAttacker->GetFinalAttributeMap();
	LC_ActorAttributeMap* pkTargetAttributeMap = pkTarget->GetFinalAttributeMap();
	if (!pkScriptCallerAttributeMap || !pkTargetAttributeMap)
	{
		return false;
	}
	//攻击者必然命中
	bool bAttackerAttackFlag = pkAttacker->GetSkillFlag(SK_ATTACK_FLAG);
	if (bAttackerAttackFlag)
	{
		return true;
	}

	LC_TargetMap* pkTargetMap = pkScriptCaller->GetTargetMap();
	LC_TargetsHitStateMap& pkTargetHitMap = pkTargetMap->GetCurrentTargetsHitState();

	LC_TargetsHitStateMap::iterator findIter = pkTargetHitMap.find(pkTarget->GetID());
	if (findIter != pkTargetHitMap.end())
	{
		return (findIter->second);
	}
	else
	{
		return false;
	}
	return true;
}
//------------------------------------------------------------
bool SC_ServerSkillInterface::IsAttackCritical(GameLogic::LC_Attacker* pkScriptCaller, int iDamageTargetIndex, int lSkillTypeID, int iAttackRateRandomIndex, int iCriticalRateRandomIndex)
{
	if (NULL == pkScriptCaller)
	{
		return false;
	}

	//判断是否命中
	if (!IsAttackHit(pkScriptCaller, iDamageTargetIndex, lSkillTypeID, iAttackRateRandomIndex))
	{
		return false;
	}

	object_id_type lAttackerID = pkScriptCaller->GetID();
	if (!IS_PLAYER_CLASS_ID(lAttackerID) && !IS_NPC_ID(lAttackerID))
	{
		return false;
	}

	LC_ActorPtrList kDamageTargetsList;
	_findTarget(pkScriptCaller, iDamageTargetIndex, kDamageTargetsList);
	if (kDamageTargetsList.size() == 0)
	{
		return false;
	}
	LC_ActorBase* pkAttacker = (LC_ActorBase*)pkScriptCaller;
	LC_ActorBase* pkTarget = kDamageTargetsList.front();
	LC_ActorAttributeMap* pkScriptCallerAttributeMap = pkAttacker->GetFinalAttributeMap();
	LC_ActorAttributeMap* pkTargetAttributeMap = pkTarget->GetFinalAttributeMap();
	if (!pkScriptCallerAttributeMap || !pkTargetAttributeMap)
	{
		return false;
	}
	//攻击者必然暴击
	bool bAttackerCriticalFlag = pkAttacker->GetSkillFlag(SK_CRITICAL_FLAG);
	if (bAttackerCriticalFlag)
	{
		return true;
	}
	//攻击者暴击率
	int iAttackerCritical = (int)pkScriptCallerAttributeMap->GetAttributeValue(ATT_CIR_RATE);
	int iAttackerCriticalRate = (int)pkScriptCallerAttributeMap->GetAttributePercent(ATT_CIR_RATE);
	//攻击者等级
	int iAttackerLevel = pkAttacker->GetLevel();

	CF_SkillSpecialAttrib::DataEntry* pkSkillData = SafeGetCSVFileDataEntryPtr<CF_SkillSpecialAttrib>(lSkillTypeID);
	if (!pkSkillData)
	{
		return false;
	}

	//技能暴击率附加
	double fSkillCriticalRate = pkSkillData->_fCriticalRate;
	//被攻击者等级
	int iTargetLevel = pkTarget->GetLevel();
	//暴击骰子
	float fCriticalRateRandom = pkAttacker->GetRandNum(iCriticalRateRandomIndex);
	int iAttackerCriticalDef = (int)pkTargetAttributeMap->GetAttribute(ATT_CIR_DEF);

	//判断是否暴击
	if (!_judgeCriticalAttack(iAttackerCritical, iAttackerCriticalRate, iAttackerCriticalDef, iAttackerLevel, iTargetLevel, fSkillCriticalRate, fCriticalRateRandom, pkAttacker, pkTarget))
	{
		return false;
	}
	return true;
}
//------------------------------------------------------------
int64_t SC_ServerSkillInterface::ComputeDamageBase(GameLogic::LC_Attacker* pkScriptCaller, int iDamageTargetIndex, int lSkillTypeID, int iDamageRandomIndex, int iCriticalRateRandomIndex, int iHPCost, int iMPCost)
{
	if (NULL == pkScriptCaller)
	{
		return 0;
	}

	object_id_type lAttackerID = pkScriptCaller->GetID();
	if (!IS_PLAYER_CLASS_ID(lAttackerID) && !IS_NPC_ID(lAttackerID))
	{
		return 0;
	}

	LC_ActorBase* pkAttacker = (LC_ActorBase*)pkScriptCaller;
	LC_ActorAttributeMap* pkScriptCallerAttributeMap = pkAttacker->GetFinalAttributeMap();
	if (!pkScriptCallerAttributeMap)
	{
		return 0;
	}
	LC_ActorPtrList kDamageTargetsList;
	_findTarget(pkScriptCaller, iDamageTargetIndex, kDamageTargetsList);
	if (kDamageTargetsList.size() == 0)
	{
		return 0;
	}
	LC_ActorBase* pkTarget = kDamageTargetsList.front();
	LC_ActorAttributeMap* pkTargetAttributeMap = pkTarget->GetFinalAttributeMap();
	if (!pkTargetAttributeMap)
	{
		return 0;
	}

	/*
	*攻击者属性
	*/
	//攻击者等级
	int64_t iAttackerLevel = pkAttacker->GetLevel();
	//攻击者MP上限
	//	int iAttackerMaxMP = pkScriptCallerAttributeMap->GetAttribute(ATT_MAX_MP);
	//攻击者攻击
	int64_t iAttackerMaxPHA = pkScriptCallerAttributeMap->GetAttribute(ATT_MAX_ATK);
	//攻击者必然暴击
	bool bAttackerCriticalFlag = pkAttacker->GetSkillFlag(SK_CRITICAL_FLAG);
	//攻击者暴击率
	int64_t iAttackerCritical = pkScriptCallerAttributeMap->GetAttributeValue(ATT_CIR_RATE);
	int64_t iAttackerCriticalRate = pkScriptCallerAttributeMap->GetAttributePercent(ATT_CIR_RATE);
	//攻击者暴击伤害
	int64_t iAttackerCriticalDamage = pkScriptCallerAttributeMap->GetAttribute(ATT_CIR_DAM);
	/*
	*被攻击者属性
	*/
	//被攻击者等级
	int64_t iTargetLevel = pkTarget->GetLevel();
	//被攻击者防御
	int64_t iTargetPHD = pkTargetAttributeMap->GetAttribute(ATT_DEF);
	//目标躲避率
	int64_t iTargetDog = pkTargetAttributeMap->GetAttributeValue(ATT_DOG_RATE);
	int64_t iTargetDogRate = pkTargetAttributeMap->GetAttributePercent(ATT_DOG_RATE);

	CF_SkillExecuteList::DataEntry* pkSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(lSkillTypeID);
	if (!pkSkillData)
	{
		return 0;
	}
	CF_SkillSpecialAttrib::DataEntry* pkSkillAttrib = SafeGetCSVFileDataEntryPtr<CF_SkillSpecialAttrib>(lSkillTypeID);
	if (!pkSkillAttrib)
	{
		return 0;
	}

	//技能内息消耗
	if (iMPCost <= 0)
	{
		int32_t iDefaultMPCost = LC_Helper::GetSkillDefaultMPCost(pkAttacker, pkSkillData);
		int32_t iAdditionMPCost = LC_Helper::GetSkillAdditionMPCost(pkAttacker, pkSkillData);
		iMPCost = iDefaultMPCost + iAdditionMPCost;
	}

	/*
	*技能属性
	*/
	//技能暴击率附加
	double fSkillCriticalRate = pkSkillAttrib->_fCriticalRate;
	//技能伤害系数
	double fSkillDamageRate = pkSkillAttrib->_fDamageRate;
	//技能暴击加成
	int64_t   iSkillCriticalDamageAdd = pkSkillAttrib->_lCriticalDamageAdd;
	//伤害力骰子
	//float fDamageRandom = pkAttacker->GetRandNum(iDamageRandomIndex);
	//暴击骰子
	double fCriticalRateRandom = pkAttacker->GetRandNum(iCriticalRateRandomIndex);


	//伤害类型
	int64_t iAttackerDamage = 0;
	int lSkillDamageType = pkSkillData->_lDamageType;
	switch (lSkillDamageType)
	{
		case SDT_NORMAL:
		case SDT_SKILL:
			{
				iAttackerDamage = iAttackerMaxPHA;
			}
			break;
		default:
			return 0;
	}

	//计算防御力
	attr_value_type iTargetDefence = 0;
	switch (lSkillDamageType)
	{
		case SDT_NORMAL:
		case SDT_SKILL:
			{
				iTargetDefence = pkAttacker->GetSkillFlag(SK_IGNORE_TARGET_PHD) ? 0 : iTargetPHD;
			}
			break;
		default:
			{
				iTargetDefence = 0;
			}
			break;
	}

	//计算基本伤害
	attr_value_type iSkillAttackRate = attr_value_type(pkSkillAttrib->_fAttackRate) + pkScriptCallerAttributeMap->GetAttribute(ATT_ATK_RATE);
	double fAttackRate = _compureAttackRate(iTargetDog, iTargetDogRate, 0, 0, iSkillAttackRate);

	double append = _computeMinAck(pkTarget->GetID(), pkAttacker->GetID());
	double iFinalDamage = _computeDamageBase(iAttackerDamage, iTargetDefence, iMPCost, fSkillDamageRate, fAttackRate - 1,append);

	bool bCirtical = false;
	if (!bAttackerCriticalFlag)
	{
		bCirtical = _judgeCriticalAttack(iAttackerCritical, iAttackerCriticalRate, 0, iAttackerLevel, iTargetLevel, fSkillCriticalRate, fCriticalRateRandom, pkAttacker, pkTarget);
	}
	else
	{
		bCirtical = true;
	}

	if (bCirtical)
	{
		iFinalDamage = Double2Int64(iFinalDamage * 1.5f + (iSkillCriticalDamageAdd + iAttackerCriticalDamage) * fSkillDamageRate / 3.0f) ;
	}
	return iFinalDamage;
}
//------------------------------------------------------------
int SC_ServerSkillInterface::AttachSkillState(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lStateTypeID, int lStateLifeTime, float fPara0 /* = 0.0f */, float fPara1 /* = 0.0f */, float fPara2 /* = 0.0f */, float fPara3 /* = 0.0f */, float fPara4 /* = 0.0f */)
{
	if (NULL == pkScriptCaller)
	{
		return RE_FAIL;
	}

	object_id_type lAttackerID = pkScriptCaller->GetID();
	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	LC_ActorPtrListIter IterBe = kTargetsList.begin();
	LC_ActorPtrListIter IterEd = kTargetsList.end();
	LC_ActorBase* pkTarget = NULL;
	object_id_type lTargetID;
	ResultType eResult = RE_SKILL_STATE_ADD_SUCCESS;
	for (; IterBe != IterEd; ++IterBe)
	{
		pkTarget = (*IterBe);
		if (NULL == pkTarget)
		{
			continue;
		}
		lTargetID = pkTarget->GetID();

		if (!IS_PLAYER_CLASS_ID(lTargetID) && !IS_NPC_ID(lTargetID))
		{
			continue;
		}

		ResultType eTmpResult = pkTarget->ActiveSkillState(lStateTypeID, lStateLifeTime, lAttackerID, fPara0, fPara1, fPara2, fPara3, fPara4);
		if (RE_SKILL_STATE_ADD_SUCCESS != eTmpResult)
		{
			eResult = eTmpResult;
		}
	}
	return eResult;
}
void SC_ServerSkillInterface::ModifyTargetControllerAttributeByValue(GameLogic::LC_Attacker* pkScriptCaller,int iTargetIndex,int iAttributeType,int64_t iDeltaValue,bool bRevert, bool computerCombatScore, int32_t reasonType)
{
	// 系统专用接口，iTargetIndex无需判断  就是给指挥官自身添加
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkScriptCaller;
	if(NULL == pkPlayer )
		return;
	if(bRevert)
		LC_Helper::RemoveValueToPartABCDbyType(1,iAttributeType,iDeltaValue,*(pkPlayer->GetMainControllerBuffAttrbuteMap()));
	else
		LC_Helper::AddValueToPartABCDbyType(1,iAttributeType,iDeltaValue,*(pkPlayer->GetMainControllerBuffAttrbuteMap()));
	
	bool login = false;
	if(reasonType != SPART_None)
	{
		login = pkPlayer->GetBuffLoginFlag();
	}
	pkPlayer->GetCommanderAttrMgr().AddCommanderAllAttributeMap(pkPlayer, true, login, reasonType);
}

int SC_ServerSkillInterface::AttachSkillStateToController(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lStateTypeID, int lStateLifeTime, float fPara0, float fPara1, float fPara2, float fPara3, float fPara4)
{
	if (NULL == pkScriptCaller)
	{
		return RE_FAIL;
	}

	object_id_type lAttackerID = pkScriptCaller->GetID();
	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	LC_ActorPtrListIter IterBe = kTargetsList.begin();
	LC_ActorPtrListIter IterEd = kTargetsList.end();
	LC_ActorBase* pkTarget = NULL;
	object_id_type lTargetID;
	ResultType eResult = RE_SKILL_STATE_ADD_SUCCESS;
	for (; IterBe != IterEd; ++IterBe)
	{
		pkTarget = (*IterBe);
		if (NULL == pkTarget)
		{
			continue;
		}
		lTargetID = pkTarget->GetID();

		if (!IS_PLAYER_CLASS_ID(lTargetID))
		{
			continue;
		}

		if (pkTarget->GetType() != LOT_PLAYER)
			continue;
		LC_ServerPlayer* pkTargetPlayer = (LC_ServerPlayer*)pkTarget;
		ResultType eTmpResult = pkTargetPlayer->ActiveSkillStateToAllBuddyInstance(lStateTypeID, lStateLifeTime, lAttackerID, fPara0, fPara1, fPara2, fPara3, fPara4);
		if (RE_SKILL_STATE_ADD_SUCCESS != eTmpResult)
		{
			eResult = eTmpResult;
		}
		else
		{
			pkTargetPlayer->SyncSkillStateToActivateBuddyInstance();
		}
	}
	return eResult;
}

//------------------------------------------------------------
void SC_ServerSkillInterface::AttachHitedTargetSkillState(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lStateTypeID, int lStateLifeTime, float fPara0 /* = 0.0f */, float fPara1 /* = 0.0f */, float fPara2 /* = 0.0f */, float fPara3 /* = 0.0f */, float fPara4 /* = 0.0f */)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	object_id_type lAttackerID = pkScriptCaller->GetID();
	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	LC_ActorPtrListIter IterBe = kTargetsList.begin();
	LC_ActorPtrListIter IterEd = kTargetsList.end();
	LC_ActorBase* pkTarget = NULL;
	object_id_type lTargetID;
	for (; IterBe != IterEd; ++IterBe)
	{
		pkTarget = (*IterBe);
		if (NULL == pkTarget)
		{
			continue;
		}
		lTargetID = pkTarget->GetID();

		if (!IS_PLAYER_CLASS_ID(lTargetID) && !IS_NPC_ID(lTargetID))
		{
			continue;
		}
		ResultType eResult = false;
		//要命中才能上这个buff，
		if (SC_ServerSkillInterface::IsAttackHit(pkScriptCaller, pkTarget->GetID(), 0, 0))
		{
			eResult = pkTarget->ActiveSkillState(lStateTypeID, lStateLifeTime, lAttackerID, fPara0, fPara1, fPara2, fPara3, fPara4);
		}
		if (RE_SKILL_STATE_ADD_FAIL_FULL == eResult)
		{
		}
	}
}
//------------------------------------------------------------
int SC_ServerSkillInterface::DetachTargetState(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iBuffType, int iKind, int iLevel, int iMaxStateNum)
{
	if (NULL == pkScriptCaller)
	{
		return 0;
	}

	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);

	object_id_type lAttackerID = pkScriptCaller->GetID();
	LC_ActorBase* pkTarget = NULL;
	LC_ActorPtrListIter Iter;
	//	LC_ActorStateMap* pkStateMap = NULL;
	object_id_type lTargetID = 0;
	// 	int iTargetCount = (int)kTargetsList.size();
	int iTargetCount = 0;

	//驱散单个状态
	if (IS_STATE_TYPE(iKind))
	{
		TRAV(kTargetsList, Iter)
		{
			iTargetCount++;
			pkTarget = *Iter;
			if (NULL == pkTarget)
			{
				continue;
			}
			lTargetID = pkTarget->GetID();
			LC_ActorStateMap* pkStateMap = pkTarget->GetStateMap();
			LC_ActorStateEntry* pkStateEntry = pkStateMap->GetStateEntryByID(iKind);
			if (pkStateEntry)
			{
				int iDetachOverlapCount = pkStateEntry->GetOverlapCount();
				pkStateMap->RemoveSkillStateInStateMap(iKind);				
				
				_handleDetachStateEvent(lAttackerID, lTargetID, 0, iKind, iDetachOverlapCount);
				
				//返回状态重叠层数
				int nTotalCount = 1;
				if ( iTargetIndex == TARGET_ALLBUDDY )
					nTotalCount = kTargetsList.size();
				if (iTargetCount == nTotalCount)
				{
					return iDetachOverlapCount;
				}
			}
		}
	}
	else	//	驱散一批状态
	{
		iTargetCount = 0;
		UT_Vec3Int kMutexType(iBuffType, iKind, iLevel);
		TRAV(kTargetsList, Iter)
		{
			iTargetCount++;
			pkTarget = *Iter;
			if (NULL == pkTarget)
			{
				continue;
			}
			lTargetID = pkTarget->GetID();

			int iClearStateNum = pkTarget->DeactiveSkillState(iBuffType, iKind, iLevel, iMaxStateNum);
			if (iClearStateNum > 0)
			{
				_handleDetachStateEvent(lAttackerID, lTargetID, 0, iKind, iClearStateNum);
			}

			int nTotalCount = 1;
			if ( iTargetIndex == TARGET_ALLBUDDY )
				nTotalCount = kTargetsList.size();

			if (iTargetCount == nTotalCount)
			{
				return iClearStateNum;
			}
		}
	}
	////返回目标数量
	//return iTargetCount;
	return 0;
}
int SC_ServerSkillInterface::DetachTargetStateToController(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iBuffType, int iKind, int iLevel, int iMaxStateNum)
{
	if (NULL == pkScriptCaller)
	{
		return 0;
	}

	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);

	object_id_type lAttackerID = pkScriptCaller->GetID();
	LC_ActorBase* pkTarget = NULL;
	LC_ActorPtrListIter Iter;
	//	LC_ActorStateMap* pkStateMap = NULL;
	object_id_type lTargetID = 0;
	// 	int iTargetCount = (int)kTargetsList.size();
	int iTargetCount = 0;

	//驱散单个状态
	if (IS_STATE_TYPE(iKind))
	{
		TRAV(kTargetsList, Iter)
		{
			iTargetCount++;
			pkTarget = *Iter;
			if (NULL == pkTarget)
			{
				continue;
			}
			lTargetID = pkTarget->GetID();
			if (pkTarget->GetType() != LOT_PLAYER)
				continue;
			LC_ServerPlayer* pTargetPlayer = (LC_ServerPlayer*)pkTarget;
			LC_ActorStateMap* pkStateMap = pTargetPlayer->GetControllerStateMap();
			LC_ActorStateEntry* pkStateEntry = pkStateMap->GetStateEntryByID(iKind);
			if (pkStateEntry)
			{
				int iDetachOverlapCount = pkStateEntry->GetOverlapCount();
				pkStateMap->RemoveSkillStateInStateMap(iKind);
				pTargetPlayer->SyncSkillStatesToClient(SSC_ALL);
				_handleDetachStateEvent(lAttackerID, lTargetID, 0, iKind, iDetachOverlapCount);

				//返回状态重叠层数
				if (iTargetCount == 1)
				{
					return iDetachOverlapCount;
				}
			}
		}
	}
	else	//	驱散一批状态
	{
		iTargetCount = 0;
		UT_Vec3Int kMutexType(iBuffType, iKind, iLevel);
		TRAV(kTargetsList, Iter)
		{
			iTargetCount++;
			pkTarget = *Iter;
			if (NULL == pkTarget)
			{
				continue;
			}
			lTargetID = pkTarget->GetID();
			if (pkTarget->GetType() != LOT_PLAYER)
				continue;
			LC_ServerPlayer* pTargetPlayer = (LC_ServerPlayer*)pkTarget;

			//int iClearStateNum = pkTarget->DeactiveSkillState(iBuffType, iKind, iLevel, iMaxStateNum);
			int iClearStateNum = pTargetPlayer->DeactiveSkillStateToAllBuddyInstance(iBuffType, iKind, iLevel, iMaxStateNum);
			if (iClearStateNum > 0)
			{
				_handleDetachStateEvent(lAttackerID, lTargetID, 0, iKind, iClearStateNum);
				pTargetPlayer->SyncSkillStatesToClient(SSC_ALL);
			}
			if (iTargetCount == 1)
			{
				return iClearStateNum;
			}
		}
	}
	////返回目标数量
	//return iTargetCount;
	return 0;
}
//------------------------------------------------------------
void SC_ServerSkillInterface::NotifyClientDetachTargetState(int attachID, int dstID, int stateID)
{
	if (IS_PLAYER_CLASS_ID(dstID))
	{
		SK_SkillEventManagerBase* pkEventManager = ENGINE_GET_SKILL_EVENT_MANAGER();
		SK_SkillEvent* pkEvent = pkEventManager->AllocEvent();
		if (NULL != pkEvent)
		{
			pkEvent->SetEventType(SETE_DETACH_STATE);
			pkEvent->SetSrcActorID(attachID);
			pkEvent->SetDstActorID(dstID);
			pkEvent->SetLogicID(stateID);
			pkEvent->SetParam(SK_DetachStateEvent::SEP_DETACH_TYPE, SK_DetachStateEvent::SDSR_NOTIFY);
			pkEvent->SetParam(SK_DetachStateEvent::SEP_STATE_ID, stateID);
			pkEvent->SetParam(SK_DetachStateEvent::SEP_DETACH_COUNT, 1);
			pkEventManager->AddEvent(pkEvent);
		}
	}
}
//------------------------------------------------------------
struct CountStateOPer : public GameLogic::LC_ActorStateOperator
{
	CountStateOPer(int iX, int iY, int iZ)
		: kMutexType(iX, iY, iZ)
		, iCount(0)
	{}
	virtual bool operator()(GameLogic::LC_ActorStateEntry* pkEntry)
	{
		if (pkEntry && pkEntry->GetValid()
				&& LC_Helper::CheckStateMutexType(pkEntry->GetTypeID(), kMutexType))
		{
			++iCount;
			return true;
		}
		return false;
	}
	UT_Vec3Int kMutexType;
	int32_t iCount;
};
int SC_ServerSkillInterface::GetTargetStateCount(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iBuffType, int iKind, int iLevel)
{
	if (NULL == pkScriptCaller)
	{
		return 0;
	}

	LC_ActorPtrList kTargetList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetList);
	if (kTargetList.size() != 1)
	{
		return 0;
	}

	//统计目标身上的state数量
	//int iTargetStateCount = 0;

	LC_ActorBase* pkTarget = NULL;
	LC_ActorPtrListIter iter;
	//LC_ActorStateMap* pkStateMap = NULL;
	//LC_ActorStateEntry* pkStateEntry = NULL;
	CountStateOPer oper(iBuffType, iKind, iLevel);
	TRAV(kTargetList, iter)
	{
		pkTarget = *iter;
		pkTarget->GetStateMap()->ForAllState(&oper);
	}
	return oper.iCount;
}
//------------------------------------------------------------
struct OverlapCountStateOPer : public GameLogic::LC_ActorStateOperator
{
	OverlapCountStateOPer(int iX, int iY, int iZ)
		: kMutexType(iX, iY, iZ)
		, iCount(0)
	{}
	virtual bool operator()(GameLogic::LC_ActorStateEntry* pkEntry)
	{
		if (pkEntry && pkEntry->GetValid()
				&& LC_Helper::CheckStateMutexType(pkEntry->GetTypeID(), kMutexType))
		{
			iCount = pkEntry->GetOverlapCount();
			return true;
		}
		return false;
	}
	UT_Vec3Int kMutexType;
	int32_t iCount;
};
int SC_ServerSkillInterface::GetTargetStateOverlapCount(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iBuffType, int iKind, int iLevel)
{
	if (NULL == pkScriptCaller)
	{
		return 0;
	}

	LC_ActorPtrList kTargetList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetList);
	if (kTargetList.size() != 1)
	{
		return 0;
	}

	LC_ActorBase* pkTarget = NULL;
	LC_ActorPtrListIter iter;
	//LC_ActorStateMap* pkStateMap = NULL;
	//LC_ActorStateEntry* pkStateEntry = NULL;
	OverlapCountStateOPer oper(iBuffType, iKind, iLevel);
	TRAV(kTargetList, iter)
	{
		pkTarget = *iter;
		pkTarget->GetStateMap()->UtilAState(&oper);
	}
	return oper.iCount;
}
//------------------------------------------------------------
void SC_ServerSkillInterface::DirtySkillState(GameLogic::LC_Attacker* pkScriptCaller, int iStateID)
{
	if ( pkScriptCaller && IS_PLAYER_CLASS_ID(pkScriptCaller->GetID()) )
	{
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkScriptCaller;
		pkPlayer->GetStateMap()->SaveDirtyInfo(iStateID, ASM_DIRTY_ADD);
	}
}
//------------------------------------------------------------
int SC_ServerSkillInterface::TranslateState(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iDestIndex, int iBuffType, int iKind, int iLevel)
{
	if (NULL == pkScriptCaller)
	{
		return 0;
	}

	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	if (kTargetsList.empty())
	{
		return 0;
	}

	LC_ActorPtrList kDestsList;
	_findTarget(pkScriptCaller, iDestIndex, kDestsList);
	if (kDestsList.empty())
	{
		return 0;
	}

	object_id_type lSrcID = 0;
	if (/*NULL != pkScriptCaller &&*/ (IS_PLAYER_CLASS_ID(pkScriptCaller->GetID()) || IsNPCID(pkScriptCaller->GetID())))
	{
		lSrcID = pkScriptCaller->GetID();
	}

	LC_ActorBase* pkTarget = kTargetsList.front();
	LC_ActorBase* pkDest = kDestsList.front();
	object_id_type lTargetID = pkTarget->GetID();
	object_id_type lDestID = pkDest->GetID();
	//如果状态转移入和转移出是同一个目标，则退出
	if (lTargetID == lDestID)
	{
		return 0;
	}

	LC_ActorStateMap* pkStateMap = pkTarget->GetStateMap();
	FilerStateOPer oper(iBuffType, iKind, iLevel);
	pkStateMap->ForAllState(&oper);

	skill_id_type lStateTypeID = 0;
	if (!oper.kStates.empty())
	{
		LC_SkillIDSet::iterator it = oper.kStates.begin();
		for (; it != oper.kStates.end(); ++it)
		{
			lStateTypeID = *it;
			LC_ActorStateEntry* pkEntry = pkStateMap->GetStateEntryByID(lStateTypeID);
			if (pkEntry)
			{
				pkDest->ActiveSkillState(pkEntry->GetTypeID(), pkEntry->GetFirstAttachStateLifeTime(),
										 lSrcID,
										 pkEntry->GetFirstAttachStatePara0(),
										 pkEntry->GetFirstAttachStatePara1(),
										 pkEntry->GetFirstAttachStatePara2(),
										 pkEntry->GetFirstAttachStatePara3(),
										 pkEntry->GetFirstAttachStatePara4());


				pkStateMap->RemoveSkillStateInStateMap(pkEntry->GetTypeID());
			}
		}
	}
	return lStateTypeID;
}
//------------------------------------------------------------
void SC_ServerSkillInterface::SetTargetSkillCD(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lSkillTypeID, float fCDTime)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	LC_ActorPtrListIter Iter;
	LC_ActorBase* pkTarget;
	LC_PlayerBase* pkPlayer = NULL;
	TRAV(kTargetsList, Iter)
	{
		pkTarget = *Iter;
		if (IS_PLAYER_CLASS_ID(pkTarget->GetID()))
		{
			pkPlayer = (LC_PlayerBase*)pkTarget;

			CF_SkillExecuteList::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_SkillExecuteList>(lSkillTypeID);
			if(NULL == pkData)
			{
				return;
			}

			pkPlayer->GetSkillCoolDownMap()->SetSkillCD(lSkillTypeID, fCDTime);
			pkPlayer->UpdateDirtyFlag(DIRTY_FLAG_SKILL_CD);
		}
	}
}

void SC_ServerSkillInterface::ResetTargetSkillCD(GameLogic::LC_Attacker* pkScriptCaller,int iTargetIndex,int lSkillTypeID,float fCDTime)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	LC_ActorPtrListIter Iter;
	LC_ActorBase* pkTarget;
	LC_PlayerBase* pkPlayer = NULL;
	TRAV(kTargetsList, Iter)
	{
		pkTarget = *Iter;
		if (IS_PLAYER_CLASS_ID(pkTarget->GetID()))
		{
			pkPlayer = (LC_PlayerBase*)pkTarget;

			CF_SkillExecuteList::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_SkillExecuteList>(lSkillTypeID);
			if(NULL == pkData)
			{
				return;
			}

			pkPlayer->GetSkillCoolDownMap()->ResetSkillCD(lSkillTypeID, fCDTime);
			pkPlayer->UpdateDirtyFlag(DIRTY_FLAG_SKILL_CD);
		}
	}
}

void SC_ServerSkillInterface::SetTargetSkillCDOnce(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lSkillTypeID, float fCDTime)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	float fDebugNow = GET_PROCESS_TIME;
	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	LC_ActorPtrListIter Iter;
	LC_ActorBase* pkTarget;
	LC_PlayerBase* pkPlayer = NULL;
	TRAV(kTargetsList, Iter)
	{
		pkTarget = *Iter;
		if (IS_PLAYER_CLASS_ID(pkTarget->GetID()))
		{
			pkPlayer = (LC_PlayerBase*)pkTarget;
			pkPlayer->GetSkillCoolDownMap()->SetSkillCDOnce(lSkillTypeID, fCDTime);
			pkPlayer->GetSkillCoolDownMap()->RefreshSkillCD(lSkillTypeID, false, false);
			pkPlayer->UpdateDirtyFlag(DIRTY_FLAG_SKILL_CD);
		}
	}
}

//------------------------------------------------------------
void SC_ServerSkillInterface::SetTargetSkillTypeCD(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iCDType, int iTypeID, float fCDTime)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	LC_ActorPtrListIter Iter;
	LC_ActorBase* pkTarget;
	LC_PlayerBase* pkPlayer = NULL;
	TRAV(kTargetsList, Iter)
	{
		pkTarget = *Iter;
		if (IS_PLAYER_CLASS_ID(pkTarget->GetID()))
		{
			pkPlayer = (LC_PlayerBase*)pkTarget;
			pkPlayer->GetSkillCoolDownMap()->SetSkillTypeCD(iCDType, iTypeID, fCDTime);
		}
	}
}
//------------------------------------------------------------
void SC_ServerSkillInterface::ClearTargetSkillCD(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lSkillTypeID)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	LC_ActorPtrListIter Iter;
	LC_ActorBase* pkTarget;
	LC_PlayerBase* pkPlayer = NULL;
	TRAV(kTargetsList, Iter)
	{
		pkTarget = *Iter;
		if (IS_PLAYER_CLASS_ID(pkTarget->GetID()))
		{
			pkPlayer = (LC_PlayerBase*)pkTarget;
			CF_SkillExecuteList::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_SkillExecuteList>(lSkillTypeID);
			if(NULL == pkData)
			{
				return;
			}

			pkPlayer->GetSkillCoolDownMap()->ClearSkillCD(lSkillTypeID);
		}
	}
}
//------------------------------------------------------------
void SC_ServerSkillInterface::ClearTargetSkillTypeCD(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iCDType, int iTypeID)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	LC_ActorPtrListIter Iter;
	LC_ActorBase* pkTarget;
	LC_PlayerBase* pkPlayer = NULL;
	TRAV(kTargetsList, Iter)
	{
		pkTarget = *Iter;
		if (IS_PLAYER_CLASS_ID(pkTarget->GetID()))
		{
			pkPlayer = (LC_PlayerBase*)pkTarget;
			pkPlayer->GetSkillCoolDownMap()->ClearSkillTypeCD(iCDType, iTypeID);
		}
	}
}
//------------------------------------------------------------
void SC_ServerSkillInterface::ClearTargetGlobalCD(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	LC_ActorPtrListIter Iter;
	LC_ActorBase* pkTarget;
	LC_PlayerBase* pkPlayer = NULL;
	TRAV(kTargetsList, Iter)
	{
		pkTarget = *Iter;
		if (IS_PLAYER_CLASS_ID(pkTarget->GetID()))
		{
			pkPlayer = (LC_PlayerBase*)pkTarget;
			pkPlayer->GetSkillCoolDownMap()->ClearGlobalCoolDownLevel();
		}
	}
}
//------------------------------------------------------------
void SC_ServerSkillInterface::ModifyTargetSkillCD(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iCDModifyFuncID, bool bRevertModify)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	CF_SkillCDModifyFunc* pkCDModifyFuncList = CF_SkillCDModifyFunc::GetSingletonPtr();
	CF_SkillCDModifyFunc::DataEntry* pkCDModifyFuncData = pkCDModifyFuncList->GetEntryPtr(iCDModifyFuncID);
	if (NULL == pkCDModifyFuncData)
	{
		return;
	}

	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	LC_ActorPtrListIter Iter;
	LC_ActorBase* pkTarget;
	LC_PlayerBase* pkPlayer = NULL;
	LC_SkillCoolDownMap* pkSkillCDMap = NULL;
	TRAV(kTargetsList, Iter)
	{
		pkTarget = *Iter;
		if (IS_PLAYER_CLASS_ID(pkTarget->GetID()))
		{
			pkPlayer = (LC_PlayerBase*)pkTarget;
			pkSkillCDMap = pkPlayer->GetSkillCoolDownMap();
			if (NULL != pkSkillCDMap)
			{
				//添加技能CD修改
				if (!bRevertModify)
				{
					pkSkillCDMap->AddCDModifyFunc(iCDModifyFuncID);
				}
				else	//删除技能CD修改
				{
					pkSkillCDMap->RemoveCDModifyFunc(iCDModifyFuncID);
				}
			}
		}
	}
}
//------------------------------------------------------------
float SC_ServerSkillInterface::GetTargetSkillCD( GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lSkillTypeID )
{
	if (NULL == pkScriptCaller)
	{
		return 0;
	}

	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	LC_ActorPtrListIter Iter;
	LC_ActorBase* pkTarget;
	LC_PlayerBase* pkPlayer = NULL;
	TRAV(kTargetsList, Iter)
	{
		pkTarget = *Iter;
		if (IS_PLAYER_CLASS_ID(pkTarget->GetID()))
		{
			pkPlayer = (LC_PlayerBase*)pkTarget;

			CF_SkillExecuteList::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_SkillExecuteList>(lSkillTypeID);
			if(NULL == pkData)
			{
				return 0;
			}

			return pkPlayer->GetSkillCoolDownMap()->GetSkillCurrentCDTime(lSkillTypeID);
		}
	}
	return 0;
}
//------------------------------------------------------------
int SC_ServerSkillInterface::GetShortBarGodSkill( GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int ShortBarIndex )
{
	if ( NULL == pkScriptCaller )
		return 0;

	if ( IS_PLAYER_CLASS_ID( pkScriptCaller->GetID() ) )
	{
		LC_ServerPlayer* pPlayer = (LC_ServerPlayer*)pkScriptCaller;
		LC_ShortcutEntry* pce = pPlayer->GetShortcutBar()->GetShortcutEntry(3, ShortBarIndex);
		if ( pce )
		{
			return pce->GetTypeID();
		}
	}
	return 0;
}
//------------------------------------------------------------
void SC_ServerSkillInterface::InterruptTargetCurrentSkill(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	LC_ActorPtrListIter Iter;
	LC_ActorBase* pkTarget;
	ResultType iInterruptCheckResult;
	object_id_type lTargetID;
	object_id_type lCallerID;
	int lSkillID;
	LC_ActorBase* pkScriptCallerActor = NULL;

	if (IS_PLAYER_CLASS_ID(pkScriptCaller->GetID()) || IS_NPC_ID(pkScriptCaller->GetID()))
	{
		pkScriptCallerActor = (LC_ActorBase*)pkScriptCaller;
	}
	else
	{
		return;
	}
	lCallerID = pkScriptCallerActor->GetID();

	TRAV(kTargetsList, Iter)
	{
		pkTarget = *Iter;
		lTargetID = pkTarget->GetID();

		if (!IS_PLAYER_CLASS_ID(lTargetID) && !IS_NPC_ID(lTargetID))
		{
			continue;
		}
		lSkillID = pkTarget->GetCurrentStateType();
		if (!IS_SKILL_TYPE(lSkillID))
		{
			continue;
		}
		iInterruptCheckResult = pkTarget->CheckInterruptSkill(lSkillID, SKI_FORCE);
		if (iInterruptCheckResult == RE_SKILL_NONE_ERROR)
		{
			/*pkTarget->InterruptSkill(pkTarget->GetLifeTime(),lSkillID,SKI_EVENT);
			pkTarget->NotifySkillEvent(SET_BE_INTERRUPT,lCallerID,lSkillID,NO_USE_PARAMETER,NO_USE_PARAMETER);
			pkScriptCallerActor->NotifySkillEvent(SET_INTERRUPT,lTargetID,lSkillID,NO_USE_PARAMETER,NO_USE_PARAMETER);*/

			SK_ServerSkillEventManager* pkEventManager = SERVER_GET_SKILL_EVENT_MANAGER();
			SK_SkillEvent* pkEvent = pkEventManager->AllocEvent();
			if (NULL != pkEvent)
			{
				pkEvent->SetEventType(SETE_INTERRUPT);
				pkEvent->SetSrcActorID(lCallerID);
				pkEvent->SetDstActorID(lTargetID);
				pkEvent->SetLogicID(0);
				pkEvent->SetParam(SK_InterruptEvent::SEP_BE_INTERRUPT_SKILL_ID, lSkillID);
				pkEvent->SetParam(SK_InterruptEvent::SEP_INTERRUPT_TYPE, SKI_FORCE);
				pkEventManager->AddEvent(pkEvent);
			}
		}
	}
}
//------------------------------------------------------------
void SC_ServerSkillInterface::ReturnToCity(GameLogic::LC_Attacker* pkScriptCaller, bool needadd)
{
	//临时添加,应该由服务器同步给客户端
	if (pkScriptCaller == NULL || !IS_PLAYER_CLASS_ID(pkScriptCaller->GetID()))
	{
		return;
	}

	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkScriptCaller;

	//查找回城点
	int CurMapResID = pkPlayer->GetMapResID();
	Utility::UT_Vec3Int CurPos = pkPlayer->GetCurrentLocation();
	int CurAreaId = pkPlayer->GetLogicAreaId();

	CF_WorldMapList* pkCSVWorldMapList = CF_WorldMapList::GetSingletonPtr();
	CF_WorldMapList::DataEntry* data = pkCSVWorldMapList->GetEntryPtr(CurMapResID);
	if (data == NULL)
	{
		return;
	}
	Utility::UT_Vec3Int CityPos = data->_iReturnBackPos;
	int CityMapResID = data->_iReturnBackResId;

	LC_ServerWorldManager* pkServerWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerPlayer* pOwner = pkPlayer->GetOwnerPlayer();
	if ( pOwner == NULL )
		return;

	ResultType ret = pkServerWorldManager->RequestChangePlayerLocation(pOwner, LMIOT_RETURN_CITY, 0, CityMapResID, CityPos);
	if (ret != RE_SUCCESS)
	{
		return;
	}
	
	if (needadd)
	{
		//增加传送门
		pOwner->GetPortalAsset()->AddPortal(44111100, CityPos, CityMapResID, CurPos, CurMapResID, CurAreaId, pOwner->GetID());
	}
	return;
}

bool SC_ServerSkillInterface::CanReturnToCity(GameLogic::LC_Attacker* pkScriptCaller)
{
	if (pkScriptCaller == NULL || !IS_PLAYER_CLASS_ID(pkScriptCaller->GetID()))
	{
		return false;
	}
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkScriptCaller;
	//查找回城点
	CF_WorldMapList* pkCSVWorldMapList = CF_WorldMapList::GetSingletonPtr();
	int CurMapResID = pkPlayer->GetMapResID();
	CF_WorldMapList::DataEntry* data = pkCSVWorldMapList->GetEntryPtr(CurMapResID);
	if (data == NULL)
	{
		return false;
	}
	CF_WorldMapList::DataEntry* pTargetMap = pkCSVWorldMapList->GetEntryPtr(data->_iReturnBackResId);
	if (pTargetMap == NULL || (pTargetMap->_CanChgLocation <= 0 && UT_ServerHelper::IsInCross(pkPlayer) == true))
	{
		return false;
	}
	return true;
}

bool SC_ServerSkillInterface::ServerPlayerCross(GameLogic::LC_Attacker* pkScriptCaller)
{
	if ( pkScriptCaller && IS_PLAYER_ID(pkScriptCaller->GetID())) 
	{
		return UT_ServerHelper::IsInCross((LC_ServerPlayer*)pkScriptCaller);
	}
	else
	{
		return true;
	}
}

void SC_ServerSkillInterface::AddItemToPack(GameLogic::LC_Attacker* pkScriptCaller, int iItemID, int iItemNum, int iReason)
{
	if (pkScriptCaller == NULL || !IS_PLAYER_ID(pkScriptCaller->GetID()))
	{
		return;
	}
	LC_PackEntryFactory* pkPackEntryFactory = LC_PackEntryFactory::GetSingletonPtr();
	if (pkPackEntryFactory == NULL)
	{
		return;
	}
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkScriptCaller;
	LC_BackPackEntryList kEntryList;
	kEntryList.clear();

	PackOperateInfoList objOPList;
	objOPList.clear();

	pkPackEntryFactory->CreateItemEntry(iItemID, iItemNum, CIET_NONE, kEntryList);
	for (int i = 0 ; i < (int)kEntryList.size() ; ++i)
	{
		PackOperateInfo objData;
		objData.Reset();
		objData.m_iOPType = OPT_ADD_ITEM;
		objData.m_bOPEntry = true;
		objData.m_bMerger  = true;
		objData.m_kPackEntry.Clone(kEntryList[i]);
		objOPList.push_back(objData);
	}

	ResultType iResult = pkPlayer->GetPackAsset()->ExecutePackOP(objOPList, true);
	if (iResult == RE_SUCCESS)
	{
		pkPlayer->SyncBackPackToClient(PACK_TYPE_BASIC);
		PackOPRecordSet* record_set = pkPlayer->GetPackAsset()->GetRecordSet();
		pkPlayer->HandlePackOPRecord(record_set, iReason, 0);
	}
}

void SC_ServerSkillInterface::AddItemToPackByDrop(GameLogic::LC_Attacker* pkScriptCaller, int targetIndex)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, targetIndex, kTargetsList);
	if (kTargetsList.size() == 0)
		return;

	LC_ActorBase* pkTarget = kTargetsList.front();
	if (!IS_NPC_ID(pkTarget->GetID()))
		return;
	
	CF_CharType::DataEntry* pkEntry = pkTarget->GetCharTypePtr();
	if ( pkEntry == NULL )
		return;

	SC_ServerDebugInterface* pkDebugInterface = SC_ServerScriptInterface::GetSingletonPtr()->GetDebugInterface();
	if (NULL == pkDebugInterface)
		return;

	int dropID = pkEntry->_DisassembleID;
	pkDebugInterface->AddDrop(pkScriptCaller->GetID(), dropID);
}

//------------------------------------------------------------------------------------------
bool SC_ServerSkillInterface::DeleteItemInPack(GameLogic::LC_Attacker* pkScriptCaller, int iItemID, int iItemNum, int iReason)
{
	if (pkScriptCaller == NULL || !IS_PLAYER_ID(pkScriptCaller->GetID()))
	{
		return false;
	}
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkScriptCaller;

	PackOperateInfoList objOPList;
	objOPList.clear();

	PackOperateInfo objData;
	objData.Reset();

	objData.m_iOPType    = OPT_DELETE_ITEM_BY_ID;
	objData.m_bOPEntry   = false;
	objData.m_lItemID    = iItemID;
	objData.m_lCount     = iItemNum;
	objOPList.push_back(objData);

	if (LC_Helper::DoItemOPs(pkPlayer, objOPList, iReason, 0) == RE_SUCCESS)
	{
		return true;
	}
	return false;
}
//------------------------------------------------------------------------------------------
int SC_ServerSkillInterface::ActiveSpawnPoint(GameLogic::LC_Attacker* pkScriptCaller, int lSpawnPointID)
{
	if (pkScriptCaller == NULL)
	{
		return 0;
	}
	LC_MapBase* pkMap = pkScriptCaller->GetMap();
	if (pkMap == NULL)
	{
		return 0;
	}
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerLogicManager* pkLogicManager = SERVER_GET_LOGIC_MANAGER();
	GameLogic::LC_SpawnManagerBase* pkManager = pkLogicManager->GetSpawnManager();
	if (pkManager == NULL)
	{
		return 0;
	}
	return pkManager->CreateNPCOnSpawnPoint(pkMap, lSpawnPointID);
}
//------------------------------------------------------------------------------------------
object_id_type SC_ServerSkillInterface::SpawnNPCByPointID(GameLogic::LC_Attacker* pkScriptCaller, int lSpawnPointID, int lCharTypeID, int lSurvivalTime)
{
	if (pkScriptCaller == NULL)
	{
		return 0;
	}
	LC_MapBase* pkMap = pkScriptCaller->GetMap();
	if (pkMap == NULL)
	{
		return 0;
	}
	LC_ServerLogicManager* pkLogicManager = SERVER_GET_LOGIC_MANAGER();
	GameLogic::LC_SpawnManagerBase* pkManager = pkLogicManager->GetSpawnManager();
	if (NULL == pkManager)
	{
		return 0;
	}
	//脚本刷出的NPC，必须设生存时间，否则最大一个小时
	if (lSurvivalTime <= 0)
	{
		lSurvivalTime = 3600;
	}

	CF_CharType::DataEntry* pkEntry = SafeGetCSVFileDataEntryPtr<CF_CharType>(lCharTypeID);
	if ( pkEntry == NULL )
		return 0;

	return pkManager->CreateNPCOnSpawnPoint(pkMap, lSpawnPointID, lCharTypeID, lSurvivalTime);
}
//------------------------------------------------------------------------------------------
object_id_type SC_ServerSkillInterface::SpawnNPCNearObject(GameLogic::LC_Attacker* pkScriptCaller, object_id_type nObjectId,
														   int lRadius, int lCharTypeID,
														   int iFictionType, int lSurvialTime)
{
	if (NULL == pkScriptCaller)
	{
		return INVALID_ACTOR_ID;
	}

	LC_ActorBase* pkActor = (LC_ActorBase*)pkScriptCaller;

	LC_MapBase* pkMap = pkActor->GetMap();
	if (NULL == pkMap)
	{
		return INVALID_ACTOR_ID;
	}

	LC_ActorBase* pkTargetObj =  (LC_ActorBase*)pkMap->FindObject(nObjectId);
	if ( pkTargetObj == NULL )
	{
		return INVALID_ACTOR_ID;
	}

	if ( pkTargetObj->GetDead() )
		return INVALID_ACTOR_ID;

	UT_Vec3Int kTargetLocation;
	kTargetLocation = pkTargetObj->GetCurrentLocation();

	LC_ServerLogicManager* pkLogicManager = SERVER_GET_LOGIC_MANAGER();
	GameLogic::LC_SpawnManagerBase* pkManager = pkLogicManager->GetSpawnManager();
	if (NULL == pkManager)
	{
		return 0;
	}

	CF_CharType::DataEntry* pkEntry = SafeGetCSVFileDataEntryPtr<CF_CharType>(lCharTypeID);
	if ( pkEntry == NULL )
		return 0;

	//脚本刷出的NPC，必须设生存时间，否则最大一个小时
	if (lSurvialTime <= 0)
	{
		lSurvialTime = 3600;
	}

	float forwardAngle = pkActor->GetForwardDirAngle();
	object_id_type npcid = pkManager->CreateNPCInWorld(pkMap, kTargetLocation, lCharTypeID, lSurvialTime, 0, Float2Int(forwardAngle));
	LC_ServerNPC* npc = (LC_ServerNPC*) pkMap->FindObject(npcid);
	if (npc /*&& pkActor*/)
	{
		npc->SetSummonOwnerID(pkActor->GetID());
		npc->SetForwardDir(pkActor->GetForwardDir());
	}
	return npcid;
}

//------------------------------------------------------------------------------------------
object_id_type SC_ServerSkillInterface::SpawnNPCNearTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex,
		int lRadius, int lCharTypeID,
		int iFictionType, int lSurvialTime)
{
	if (NULL == pkScriptCaller)
	{
		return INVALID_ACTOR_ID;
	}

	LC_ActorBase* pkActor = (LC_ActorBase*)pkScriptCaller;

	LC_MapBase* pkMap = pkActor->GetMap();
	if (NULL == pkMap)
	{
		return 0;
	}
	UT_Vec3Int kTargetLocation;

	if (iTargetIndex == TARGET_LOCATION)
	{
		kTargetLocation = pkActor->GetLockedLocation();
	}
	else if (iTargetIndex == TARGET_BULLET_LOCATION)
	{
		kTargetLocation = pkActor->GetBulletLocation();
	}
	else
	{
		LC_ActorPtrList kTargetsList;
		_findTarget(pkActor, iTargetIndex, kTargetsList);
		if (kTargetsList.empty())
		{
			return 0;
		}
		LC_ActorBase* pkTarget = kTargetsList.front();
		kTargetLocation = pkTarget->GetCurrentLocation();
		if ( pkTarget->GetDead() )
			return 0;
		if ( IS_SKILL_TYPE(pkActor->GetCurrentStateType()) )
		{
			int32_t nDistance = UT_MathBase::LineLengthXYInt(pkTarget->GetCurrentLocation(), pkActor->GetCurrentLocation() );
			CF_SkillExecuteList::DataEntry* pkSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(pkActor->GetCurrentStateType());
			if ( pkSkillData == NULL )
				return 0;
			if ( nDistance > pkSkillData->_lMaxExecuteDistance )
				return 0;
		}
	}

	LC_ServerLogicManager* pkLogicManager = SERVER_GET_LOGIC_MANAGER();
	GameLogic::LC_SpawnManagerBase* pkManager = pkLogicManager->GetSpawnManager();
	if (NULL == pkManager)
	{
		return 0;
	}

	CF_CharType::DataEntry* pkEntry = SafeGetCSVFileDataEntryPtr<CF_CharType>(lCharTypeID);
	if ( pkEntry == NULL )
		return 0;

	//脚本刷出的NPC，必须设生存时间，否则最大一个小时
	if (lSurvialTime <= 0)
	{
		lSurvialTime = 3600;
	}

	float forwardAngle = pkActor->GetForwardDirAngle();
	object_id_type npcid = pkManager->CreateNPCInWorld(pkMap, kTargetLocation, lCharTypeID, lSurvialTime, 0, Float2Int(forwardAngle));
	LC_ServerNPC* npc = (LC_ServerNPC*) pkMap->FindObject(npcid);
	if (npc /*&& pkActor*/)
	{
		npc->SetSummonOwnerID(pkActor->GetID());
		npc->SetForwardDir(pkActor->GetForwardDir());
	}
	return npcid;
}
//------------------------------------------------------------------------------------------
object_id_type SC_ServerSkillInterface::SpawnNPCAroundTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex,
		int lMinRadius, int lMaxRadius, int lCharTypeID,
		int iFictionType, int lSurvialTime)
{
	if (NULL == pkScriptCaller)
	{
		return INVALID_ACTOR_ID;
	}

	LC_ActorBase* pkActor = (LC_ActorBase*)pkScriptCaller;

	LC_MapBase* pkMap = pkActor->GetMap();
	if (pkMap == NULL)
	{
		return 0;
	}
	UT_Vec3Int kTargetLocation;

	if (iTargetIndex == TARGET_LOCATION)
	{
		kTargetLocation = pkActor->GetLockedLocation();
	}
	else if (iTargetIndex == TARGET_BULLET_LOCATION)
	{
		kTargetLocation = pkActor->GetBulletLocation();
	}
	else
	{
		LC_ActorPtrList kTargetsList;
		_findTarget(pkActor, iTargetIndex, kTargetsList);
		if (kTargetsList.empty())
		{
			return 0;
		}
		LC_ActorBase* pkTarget = kTargetsList.front();
		kTargetLocation = pkTarget->GetCurrentLocation();
		if ( IS_SKILL_TYPE(pkActor->GetCurrentStateType()) )
		{
			int32_t nDistance = UT_MathBase::LineLengthXYInt(pkTarget->GetCurrentLocation(), pkActor->GetCurrentLocation() );
			CF_SkillExecuteList::DataEntry* pkSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(pkActor->GetCurrentStateType());
			if ( pkSkillData == NULL )
				return 0;
			if ( nDistance > pkSkillData->_lMaxExecuteDistance )
				return 0;
		}
	}

	//UT_Vec3Int  aroundLocation = UT_MathBase::RandInCircleInt(kTargetLocation, lMinRadius, lMaxRadius);	//lMinRadius, lMaxRadius范围内周围随机一个位置
	UT_Vec3Int  aroundLocation;
	//UT_PathFindHelp::GenRandomPosInCircle2(pkMap->GetMapResID(), kTargetLocation, lMaxRadius, aroundLocation);
	UT_PathFindHelp::GenRandomPosInCircle(pkMap->GetMapResID(), kTargetLocation, lMinRadius, lMaxRadius, aroundLocation);
	UT_Vec3Int dir3 = aroundLocation - kTargetLocation;

	LC_ServerLogicManager* pkLogicManager = SERVER_GET_LOGIC_MANAGER();
	GameLogic::LC_SpawnManagerBase* pkManager = pkLogicManager->GetSpawnManager();
	if (NULL == pkManager)
	{
		return 0;
	}
	
	CF_CharType::DataEntry* pkEntry = SafeGetCSVFileDataEntryPtr<CF_CharType>(lCharTypeID);
	if ( pkEntry == NULL )
		return 0;

	float forwardAngle = pkActor->GetForwardDirAngle();
	object_id_type npcid = pkManager->CreateNPCInWorld(pkMap, aroundLocation, lCharTypeID, lSurvialTime, 0, Float2Int(forwardAngle));
	LC_ServerNPC* npc = (LC_ServerNPC*) pkMap->FindObject(npcid);
	if (NULL == npc)
	{
		return 0;
	}
	////召唤物方向背对召唤者
	//Utility::UT_Vec2Float dir2;
	//dir2.x = (float)dir3.x;
	//dir2.y = (float)dir3.y;
	//if (dir2.Unitize() > 1e-3f)
	//	npc->SetForwardDir(dir2);
	//else
	npc->SetForwardDir(pkActor->GetForwardDir());

	return npcid;
}
//------------------------------------------------------------------------------------------
object_id_type SC_ServerSkillInterface::SpawnNPCForwardTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int forwardDis, float angle, int lCharTypeID, int iFictionType, int lSurvialTime)
{
	if (NULL == pkScriptCaller)
	{
		return INVALID_ACTOR_ID;
	}

	LC_ActorBase* pkActor = (LC_ActorBase*)pkScriptCaller;

	LC_MapBase* pkMap = pkActor->GetMap();
	if (pkMap == NULL)
	{
		return 0;
	}
	UT_Vec3Int kTargetLocation;

	if (iTargetIndex == TARGET_LOCATION)
	{
		kTargetLocation = pkActor->GetLockedLocation();
	}
	else if (iTargetIndex == TARGET_BULLET_LOCATION)
	{
		kTargetLocation = pkActor->GetBulletLocation();
	}
	else
	{
		LC_ActorPtrList kTargetsList;
		_findTarget(pkActor, iTargetIndex, kTargetsList);

		if (kTargetsList.empty())
		{
			return 0;
		}
		LC_ActorBase* pkTarget = kTargetsList.front();
		kTargetLocation = pkTarget->GetCurrentLocation();
	}


	UT_Vec2Float dir = pkActor->GetForwardDir();
	dir.Unitize();

	kTargetLocation.x = kTargetLocation.x + (int)(dir.x * forwardDis);
	kTargetLocation.y = kTargetLocation.y + (int)(dir.y * forwardDis);

	LC_ServerLogicManager* pkLogicManager = SERVER_GET_LOGIC_MANAGER();
	GameLogic::LC_SpawnManagerBase* pkManager = pkLogicManager->GetSpawnManager();
	if (NULL == pkManager)
	{
		return 0;
	}
	
	CF_CharType::DataEntry* pkEntry = SafeGetCSVFileDataEntryPtr<CF_CharType>(lCharTypeID);
	if ( pkEntry == NULL )
		return 0;

	//脚本刷出的NPC，必须设生存时间，否则最大一个小时
	if (lSurvialTime <= 0)
	{
		lSurvialTime = 3600;
	}

	UT_Vec2Float forwardDir = UT_MathBase::ZRot(pkActor->GetForwardDir(), angle);
	int zRotAngle = Float2Int(UT_MathBase::GetZRotAngle(forwardDir));
	object_id_type npcid = pkManager->CreateNPCInWorld(pkMap, kTargetLocation, lCharTypeID, lSurvialTime, 0, zRotAngle);
	LC_ServerNPC* npc = (LC_ServerNPC*) pkMap->FindObject(npcid);
	if (npc)
	{
		npc->SetForwardDir(pkActor->GetForwardDir());
	}

	return npcid;
}
//------------------------------------------------------------------------------------------
void SC_ServerSkillInterface::SetNPCSummonOwnerID(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, object_id_type lSummonOwnerID)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	for (LC_ActorPtrList::iterator it = kTargetsList.begin(); it != kTargetsList.end(); ++it)
	{
		LC_ActorBase* pkTarget = *it;
		if (IS_NPC_ID(pkTarget->GetID()))
		{
			LC_NPCBase* pkNPC  = (LC_NPCBase*)pkTarget;
			pkNPC->SetSummonOwnerID(lSummonOwnerID);

			if(IS_PLAYER_ID(lSummonOwnerID))
			{
				LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(ENGINE_GET_GAMEPLAYER_MANAGER()->FindPlayer(lSummonOwnerID));
				if(NULL != pkPlayer)
				{
					pkNPC->SetSummonOwnerLevel(pkPlayer->GetLevel());
					pkNPC->SetSummonOwnerName(pkPlayer->GetOwnerCharName());
				}
			}

			if(IS_NPC_ID(pkScriptCaller->GetID()))
			{
				int npcID = pkTarget->GetID();
				LC_NPCBase* pkOwnerNpc = (LC_NPCBase*)pkScriptCaller;
				pkOwnerNpc->AddEpigoneID(pkNPC->GetID());
			}
		}		
	}
}
void SC_ServerSkillInterface::SetNPCFromSummonOwnerAttr(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, object_id_type lSummonOwnerID)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	for (LC_ActorPtrList::iterator it = kTargetsList.begin(); it != kTargetsList.end(); ++it)
	{
		LC_ActorBase* pkTarget = *it;
		if (IS_NPC_ID(pkTarget->GetID()))
		{
			LC_NPCBase* pkNPC  = (LC_NPCBase*)pkTarget;
			pkNPC->SetSummonOwnerID(lSummonOwnerID);

			if(IS_PLAYER_ID(lSummonOwnerID))
			{
				LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(ENGINE_GET_GAMEPLAYER_MANAGER()->FindPlayer(lSummonOwnerID));
				if(NULL != pkPlayer)
				{
					pkNPC->SetSummonOwnerLevel(pkPlayer->GetLevel());
					pkNPC->SetSummonOwnerName(pkPlayer->GetOwnerCharName());
					pkNPC->GetFinalAttributeMap()->CopyFrom(pkPlayer->GetFinalAttributeMap());
				}
			}

			if(IS_NPC_ID(pkScriptCaller->GetID()))
			{
				int npcID = pkTarget->GetID();
				LC_NPCBase* pkOwnerNpc = (LC_NPCBase*)pkScriptCaller;
				pkOwnerNpc->AddEpigoneID(pkNPC->GetID());
			}
		}		
	}
}
void SC_ServerSkillInterface::SetEscortNpcState(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int EscortNpcState)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	for (LC_ActorPtrList::iterator it = kTargetsList.begin(); it != kTargetsList.end(); ++it)
	{
		LC_ActorBase* pkTarget = *it;
		if (IS_NPC_ID(pkTarget->GetID()))
		{
			LC_NPCBase* pkNPC  = (LC_NPCBase*)pkTarget;
			bool tag = pkNPC->SetEscortNpcState(EscortNpcState);
			if(tag)
			{
				object_id_type lSummonOwnerID = pkNPC->GetSummonOwnerID();
				if(IS_PLAYER_ID(lSummonOwnerID))
				{
					LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(ENGINE_GET_GAMEPLAYER_MANAGER()->FindPlayer(lSummonOwnerID));
					if(NULL != pkPlayer)
					{
						pkPlayer->SyncEscortNpcState(pkTarget->GetID(), pkTarget->GetCharType(), EscortNpcState);
					}
				}
			}
		}
	}
}
//------------------------------------------------------------------------------------------
void SC_ServerSkillInterface::SetNPCSummorID(GameLogic::LC_Attacker* pkScriptCaller, object_id_type lSummonID)
{
	if (pkScriptCaller && IS_NPC_ID(pkScriptCaller->GetID()))
	{
		LC_ServerNPC* pkNPC = (LC_ServerNPC*)pkScriptCaller;
		pkNPC->SetSummorID(lSummonID);
	}
}
//------------------------------------------------------------------------------------------
void SC_ServerSkillInterface::FinishTask(GameLogic::LC_Attacker* pkScriptCaller, int lTaskID)
{
	if (pkScriptCaller == NULL || !IS_PLAYER_ID(pkScriptCaller->GetID()))
	{
		return;
	}
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkScriptCaller;

	LC_TaskManager* pkTaskManager = LC_TaskManager::GetSingletonPtr();
	LC_Task* pkTask = pkTaskManager->GetTask(lTaskID);
	if (pkTask == NULL)
	{
		return ;
	}
	ResultType iFinishResult = pkPlayer->FinishTask(lTaskID, 10000);
	switch (iFinishResult)
	{
		case RE_TASK_FINISH_SUCCESS:
		case RE_TASK_BILLBOARD_FINISH_SUCCESS:
			{
				MG_RLT_FinishTask rltMsg;
				rltMsg.m_lPlayerID = pkPlayer->GetID();
				rltMsg.m_lTaskID = lTaskID;

				pkPlayer->SendMsgToClient(MGPT_RLT_FINISH_TASK, &rltMsg);

				if (RE_TASK_BILLBOARD_FINISH_SUCCESS == iFinishResult)
				{
					LC_TaskMap* pkTaskMap = pkPlayer->GetTaskMap();
					if (pkTaskMap == NULL)
						return;

					BillboardTaskData* pkTaskData = pkTaskMap->GetBillboardDataFormCanStarList(pkTask->GetAcceptNPCID());
					if (pkTaskData)
					{
						MG_RLT_BillBoardTaskInfo billboradMsg;
						billboradMsg.m_lPlayerID = pkPlayer->GetID();
						billboradMsg.m_lBillBoardID = pkTaskData->m_lBillboardID;
						for (int i = 0; i < MAX_BILLBORAD_PROCESS_COUNT; i++)
						{
							billboradMsg.m_lTaskID[i] = pkTaskData->m_lTaskID[i];
						}
						pkPlayer->SendMsgToClient(MGPT_RLT_BILLBOARD_TASK_INFO, &billboradMsg);
					}
				}
			}
			break;
		case RE_TASK_ALREADY_FINISH:
			break;
		default:
			{
				MG_Message errorMsg;
				errorMsg.m_iMessageID = iFinishResult;

				pkPlayer->SendMsgToClient(MGPT_MESSAGE, &errorMsg);
			}
	}
}

//------------------------------------------------------------------------
void SC_ServerSkillInterface::CancelTask(GameLogic::LC_Attacker* pkScriptCaller, int lTaskID)
{
	if (pkScriptCaller == NULL || !IS_PLAYER_ID(pkScriptCaller->GetID()))
	{
		return;
	}
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkScriptCaller;
	LC_TaskManager* pkTaskManager = LC_TaskManager::GetSingletonPtr();
	if (pkTaskManager == NULL)
	{
		return;
	}
	LC_Task* pkTask = pkTaskManager->GetTask(lTaskID);
	if (pkTask == NULL)
	{
		return;
	}
	MG_RLT_CancelTask rltMsg;
	rltMsg.m_lPlayerID = pkPlayer->GetID();
	rltMsg.m_lTaskID = lTaskID;

	ResultType iCancelResult = pkPlayer->CancelTask(lTaskID);
	rltMsg.m_iResult = iCancelResult;

	switch (iCancelResult)
	{
		case RE_TASK_CANCEL_SUCCESS:
		case RE_TASK_BILLBOARD_CANCEL_SUCCESS:
			{
				pkPlayer->SendMsgToClient(MGPT_RLT_CANCEL_TASK, &rltMsg);
				if (RE_TASK_BILLBOARD_CANCEL_SUCCESS == iCancelResult)
				{
					LC_TaskMap* pkTaskMap = pkPlayer->GetTaskMap();
					if (pkTaskMap == NULL)
					{
						return;
					}
					BillboardTaskData* pkTaskData = pkTaskMap->GetBillboardDataFormCanStarList(pkTask->GetAcceptNPCID());
					if (pkTaskData)
					{
						MG_RLT_BillBoardTaskInfo billboradMsg;
						billboradMsg.m_lPlayerID = pkPlayer->GetID();
						billboradMsg.m_lBillBoardID = pkTaskData->m_lBillboardID;
						for (int i = 0; i < MAX_BILLBORAD_PROCESS_COUNT; i++)
						{
							billboradMsg.m_lTaskID[i] = pkTaskData->m_lTaskID[i];
						}
						pkPlayer->SendMsgToClient(MGPT_RLT_BILLBOARD_TASK_INFO, &billboradMsg);
					}
				}
			}
			break;
		default:
			pkPlayer->SendMsgToClient(MGPT_RLT_CANCEL_TASK, &rltMsg);
	}
}
//-------------------------------------------------------------------------------------
void SC_ServerSkillInterface::DestineTaskTimeEvent(GameLogic::LC_Attacker* pkScriptCaller, int lTaskID, int lTime, bool bNoticeClient)
{
	
}
//-------------------------------------------------------------------------------------
void SC_ServerSkillInterface::SetTaskFail(GameLogic::LC_Attacker* pkScriptCaller, int lTaskID, int iState)
{
	if (pkScriptCaller == NULL || !IS_PLAYER_ID(pkScriptCaller->GetID()))
	{
		return;
	}
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkScriptCaller;
	LC_TaskMap* pkTaskMap = pkPlayer->GetTaskMap();
	if (pkTaskMap == NULL)
	{
		return;
	}
	LC_ProcessingTaskEntry* pkTaskEntry = NULL;
	pkTaskEntry = pkTaskMap->GetProcessingTaskEntryByID(lTaskID);
	if (pkTaskEntry)
	{
		bool bFlag = iState > 0;
		if (bFlag != pkTaskEntry->GetTaskFail())
		{
			pkTaskEntry->SetTaskFail(bFlag);
			pkPlayer->UpdateDirtyFlag(DIRTY_FLAG_PROCESSINGTASK);
		}
	}
	else
	{
		LC_TaskManager* pkTaskManager = LC_TaskManager::GetSingletonPtr();
		if (pkTaskManager == NULL)
		{
			return;
		}
		LC_Task* pkTask = pkTaskManager->GetTask(lTaskID);
		if (pkTask == NULL)
		{
			return;
		}
		pkTaskMap->RecordTaskFail(lTaskID, pkTask->GetTaskIndex());
	}
}
//-------------------------------------------------------------------------------------
void SC_ServerSkillInterface::ControlMapLogicDoorState(GameLogic::LC_Attacker* pkScriptCaller, int iDoorIndex, int iState)
{
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "ControlMapLogicDoorState %d  %d ", iDoorIndex, iState);
	if (NULL == pkScriptCaller)
	{
		return;
	}
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerMapLogic_Place* ptr = pkWorldManager->GetMapLogic(pkScriptCaller->GetMapLogicID());
	if (ptr)
	{
		LC_ServerMapLogic_Door* door = ptr->GetDoorByIndex(iDoorIndex);
		if (door)
		{
			door->SetDoorState((DoorState)iState);
		}
	}
}
//-------------------------------------------------------------------------------------
void SC_ServerSkillInterface::RecoverTargetHP(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lSourceTypeID, int64_t lRecoverHP, float fHPRecoverPercent, object_id_type lSrcActorID)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	int iSrcID = lSrcActorID;
	LC_ActorPtrList srcList;
	_findTarget(pkScriptCaller, iSrcID, srcList);
	if (srcList.size() > 0)
	{
		iSrcID = srcList.front()->GetID();
	}

	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	LC_ActorPtrListIter IterBe = kTargetsList.begin();
	LC_ActorPtrListIter IterEd = kTargetsList.end();
	LC_ActorBase* pkTarget = NULL;
	//	object_id_type lCallerID = pkScriptCaller->GetID();
	object_id_type lTargetID = 0;
	for (; IterBe != IterEd; ++IterBe)
	{
		pkTarget = (*IterBe);
		lTargetID = pkTarget->GetID();

		LC_ActorAttributeMap* pkAttributeMap = pkTarget->GetFinalAttributeMap();
		attr_value_type lMaxHP = pkAttributeMap->GetAttribute(ATT_MAX_HP);

		//获取恢复比率
		double fAttriRate = LC_Helper::GetAttributeRate(pkTarget, ATT_HP_RECOVER_RATE);
		if (pkTarget->GetSkillFlag(SK_CURSE))
		{
			fAttriRate *= 0.5;
		}
		//int lHPRecover = lRecoverHP + Float2Long(lMaxHP * fHPRecoverPercent * fAttriRate);
		int64_t lHPRecover = Double2Int64((lRecoverHP + lMaxHP * fHPRecoverPercent) * fAttriRate);

		SK_SkillEventManagerBase* pkEventManager = ENGINE_GET_SKILL_EVENT_MANAGER();
		SK_SkillEvent* pkEvent = pkEventManager->AllocEvent();
		if (NULL != pkEvent)
		{
			pkEvent->SetEventType(SETE_RECOVER);
			pkEvent->SetSrcActorID(iSrcID);
			pkEvent->SetDstActorID(lTargetID);
			pkEvent->SetLogicID(lSourceTypeID);
			pkEvent->SetParam(SK_RecoverEvent::SEP_ITEM_ID, 0);
			pkEvent->SetParam(SK_RecoverEvent::SEP_RECOVER_TYPE, SK_RecoverEvent::SRT_HP);
			pkEvent->SetParam(SK_RecoverEvent::SEP_RECOVER_BASE, lHPRecover);
			pkEventManager->AddEvent(pkEvent);
		}
	}
}
//-------------------------------------------------------------------------------------
void SC_ServerSkillInterface::RecoverTargetMP(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lSourceTypeID, attr_value_type lRecoverMP, float fMPRecoverPercent, object_id_type lSrcActorID)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	if (0 == lSrcActorID)
	{
		lSrcActorID = pkScriptCaller->GetID();
	}

	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	LC_ActorPtrListIter IterBe = kTargetsList.begin();
	LC_ActorPtrListIter IterEd = kTargetsList.end();
	LC_ActorBase* pkTarget = NULL;
	//	int lCallerID = pkScriptCaller->GetID();
	object_id_type lTargetID = 0;
	for (; IterBe != IterEd; ++IterBe)
	{
		pkTarget = (*IterBe);
		lTargetID = pkTarget->GetID();

		LC_ActorAttributeMap* pkAttributeMap = pkTarget->GetFinalAttributeMap();
		attr_value_type lMaxMP = pkAttributeMap->GetAttribute(ATT_MAX_MP);

		//获取恢复比率
		double fAttriRate = LC_Helper::GetAttributeRate(pkTarget, ATT_MP_RECOVER_RATE);
		//int lMPRecover = lRecoverMP + Float2Long(lMaxMP * fMPRecoverPercent * fAttriRate);
		attr_value_type lMPRecover = Double2Int64((lRecoverMP + lMaxMP * fMPRecoverPercent) * fAttriRate);

		SK_SkillEventManagerBase* pkEventManager = ENGINE_GET_SKILL_EVENT_MANAGER();
		SK_SkillEvent* pkEvent = pkEventManager->AllocEvent();
		if (NULL != pkEvent)
		{
			pkEvent->SetEventType(SETE_RECOVER);
			pkEvent->SetSrcActorID(lSrcActorID);
			pkEvent->SetDstActorID(lTargetID);
			pkEvent->SetLogicID(lSourceTypeID);
			pkEvent->SetParam(SK_RecoverEvent::SEP_ITEM_ID, 0);
			pkEvent->SetParam(SK_RecoverEvent::SEP_RECOVER_TYPE, SK_RecoverEvent::SRT_MP);
			pkEvent->SetParam(SK_RecoverEvent::SEP_RECOVER_BASE, lMPRecover);
			pkEventManager->AddEvent(pkEvent);
		}
	}
}
//------------------------------------------------------------
void SC_ServerSkillInterface::SetTargetHP(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int64_t lHP)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	LC_ActorPtrListIter iter;
	TRAV(kTargetsList, iter)
	{
		LC_ActorBase* pkTarget = *iter;

		pkTarget->SetHP(lHP);
	}
}

void SC_ServerSkillInterface::AddEscortTimes(object_id_type playerID)
{
	if (IS_PLAYER_ID(playerID))
	{
		LC_ServerPlayer* pkSummorPlayer = (LC_ServerPlayer*)(ENGINE_GET_GAMEPLAYER_MANAGER()->FindPlayer(playerID));
		if(NULL != pkSummorPlayer)
		{
			pkSummorPlayer->AddEscortTimes();
		}
	}
}
void SC_ServerSkillInterface::UpdateEscortRecord(GameLogic::LC_Attacker* pkScriptCaller)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	LC_ActorBase* pkTarget = (LC_ActorBase*)pkScriptCaller;
	LC_ServerPlayer* pkSummorPlayer = (LC_ServerPlayer*)(ENGINE_GET_GAMEPLAYER_MANAGER()->FindPlayer(pkTarget->GetSummonOwnerID()));
	if(NULL != pkSummorPlayer)
	{
		pkSummorPlayer->AddEscortRecord(ESCORT_ROBB_SUCCESS, pkTarget->GetCharType(), pkSummorPlayer->GetOwnerCharName());
	}
}
//------------------------------------------------------------
void SC_ServerSkillInterface::KillNpcAnyhow(GameLogic::LC_Attacker* pkScriptCaller, int iTargetID)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	if (!IS_NPC_ID(iTargetID))
	{
		return;
	}
	LC_ServerNPCManager* pkNPCManager = SERVER_GET_NPC_MANAGER();
	LC_ServerNPC* pkNPC = (LC_ServerNPC*)pkNPCManager->FindNPC(iTargetID);
	if (pkNPC)
	{
		pkNPC->SetHP(0);
	}
}
//-------------------------------------------------------------------------------------
bool SC_ServerSkillInterface::CostTargetItem(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lSourceTypeID, int lItemTypeID, int lItemCount)
{
	if (NULL == pkScriptCaller)
	{
		return false;
	}

	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	//LC_ActorPtrListIter IterBe = kTargetsList.begin();
	//LC_ActorPtrListIter IterEd = kTargetsList.end();
	LC_ActorBase* pkTarget = NULL;
	object_id_type lCallerID = pkScriptCaller->GetID();
	object_id_type lTargetID = 0;
	if (kTargetsList.empty())
	{
		return false;
	}
	pkTarget = kTargetsList.front();
	lTargetID = pkTarget->GetID();
	if (!IS_PLAYER_ID(lTargetID))
	{
		return false;
	}

	LC_PlayerBase* pkPlayer = (LC_PlayerBase*)pkTarget;
	ResultType eCostResult = pkPlayer->ConsumeItemsInPackageByID(lItemTypeID, lItemCount);
	if (eCostResult != RE_SUCCESS)
	{
		return false;
	}
	else
	{
		_handleCostEvent(lCallerID, lTargetID, lSourceTypeID, SK_CostEvent::SCT_ITEM, 0, lItemTypeID, lItemCount);
	}
	return true;
}
int SC_ServerSkillInterface::RandomTreasureEvent(GameLogic::LC_Attacker* pkScriptCaller, int itemID)
{
	LC_PlayerBase* pkPlayer = (LC_PlayerBase *)pkScriptCaller;
	if(NULL == pkPlayer)
	{
		return 0;
	}

	return pkPlayer->RandomTreasureEvent(itemID);
}
int SC_ServerSkillInterface::GetTreasureEventType(GameLogic::LC_Attacker* pkScriptCaller, int eventIndex)
{
	LC_PlayerBase* pkPlayer = (LC_PlayerBase *)pkScriptCaller;
	if(NULL == pkPlayer)
	{
		return 0;
	}

	return pkPlayer->GetTreasureEventType(eventIndex);
}
int SC_ServerSkillInterface::CastTreasureEventAddItem(GameLogic::LC_Attacker* pkScriptCaller, int eventIndex)
{
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer *)pkScriptCaller;
	if(NULL == pkPlayer)
	{
		return 0;
	}

	return pkPlayer->CastTreasureEvent_AddItem(eventIndex);
}
int SC_ServerSkillInterface::CastTreasureEventSwapnNPC(GameLogic::LC_Attacker* pkScriptCaller, int eventIndex)
{
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer *)pkScriptCaller;
	if(NULL == pkPlayer)
	{
		return 0;
	}

	return pkPlayer->CastTreasureEvent_SwapnNPC(eventIndex);
}
int SC_ServerSkillInterface::CastTreasureEventTransferMap(GameLogic::LC_Attacker* pkScriptCaller, int eventIndex)
{
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer *)pkScriptCaller;
	if(NULL == pkPlayer)
	{
		return 0;
	}

	return pkPlayer->CastTreasureEvent_TransferMap(eventIndex);
}
//-------------------------------------------------------------------------------------
int SC_ServerSkillInterface::CostItemByID(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, object_id_type lSrcID, object_id_type lLogicID, int lItemID, int iItemCount, int iAction, int lPara0, int lPara1, int lPara2, int lPara3, int lPara4)
{
	if (NULL == pkScriptCaller)
	{
		return RE_FAIL;
	}
	if (!IS_ITEM_ID(lItemID) || iItemCount <= 0)
	{
		return RE_FAIL;
	}
	//非法操作
	if (iAction < SCIA_NONE || iAction >= SCIA_MAX_COUNT)
	{
		return RE_FAIL;
	}

	LC_ActorPtrList kTargetList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetList);
	//只能对一个目标扣除物品
	if (1 != kTargetList.size())
	{
		return RE_FAIL;
	}
	LC_ActorBase* pkTarget = kTargetList.front();
	if (NULL == pkTarget || !IsPlayerID(pkTarget->GetID()))
	{
		return RE_FAIL;
	}
	LC_ServerPlayer* pkTargetPlayer = (LC_ServerPlayer*)pkTarget;
	LC_PackAsset* pkPackAsset = pkTargetPlayer->GetPackAsset();
	if (NULL == pkPackAsset)
	{
		return RE_FAIL;
	}

	//物品是否可以被消耗
	ResultType iCheckResult = pkTargetPlayer->CheckConsumeItemByID(lItemID, iItemCount);
	if (RE_SUCCESS != iCheckResult)
	{
		MG_Message kErrorMsg;
		kErrorMsg.m_iMessageID = RE_ITEM_CANT_USE;
		pkTargetPlayer->SendMsgToClient(MGPT_MESSAGE, &kErrorMsg);
		return iCheckResult;
	}

	switch (iAction)
	{
		case SCIA_NONE:		//光删除物品无后续操作
			{
				int iResult = pkTargetPlayer->ConsumeItemsInPackageByID(lItemID, iItemCount);
				//扣除物品成功
				if (RE_SUCCESS == iResult)
				{
					//扣除物品消息
					_handleCostEvent(lSrcID, pkTarget->GetID(), lLogicID, SK_CostEvent::SCT_ITEM, 0, lItemID, iItemCount);
				}
				else
				{
					MG_Message kErrorMsg;
					kErrorMsg.m_iMessageID = RE_ITEM_CANT_USE;
					pkTargetPlayer->SendMsgToClient(MGPT_MESSAGE, &kErrorMsg);
					return iResult;
				}
			}
			break;
		case SCIA_ADD_NEW_ITEM:
			{
				int lAddItemID = lPara0;
				int lAddItemCount = lPara1;
				int lAddItemReason = lPara2;
				if (!IS_ITEM_ID(lAddItemID) || lAddItemCount <= 0)
				{
					return RE_FAIL;
				}

				LC_PackEntryFactory* pkPackEntryFactory = LC_PackEntryFactory::GetSingletonPtr();
				if (NULL == pkPackEntryFactory)
				{
					return RE_FAIL;
				}
				LC_BackPackEntryList kPackEntryList;
				pkPackEntryFactory->CreateItemEntry(lAddItemID, lAddItemCount, CIET_NONE, kPackEntryList);
				int iPackEntryCount = (int)kPackEntryList.size();

				PackOperateInfoList kPackOpList;
				PackOperateInfo kPackOpEntry;
				for (int i = 0; i < iPackEntryCount; ++i)
				{
					kPackOpEntry.Reset();
					kPackOpEntry.m_iOPType = OPT_ADD_ITEM;
					kPackOpEntry.m_bOPEntry = true;
					kPackOpEntry.m_bMerger = true;
					kPackOpEntry.m_kPackEntry.Clone(kPackEntryList[i]);
					kPackOpList.push_back(kPackOpEntry);
				}

				int iResult = pkPackAsset->ExecutePackOP(kPackOpList, true);
				if (RE_SUCCESS == iResult)
				{
					pkTargetPlayer->SyncBackPackToClient(PACK_TYPE_BASIC);
					iResult = pkTargetPlayer->ConsumeItemsInPackageByID(lItemID, iItemCount);
					if (RE_SUCCESS == iResult)
					{
						//添加物品消息
						PackOPRecordSet* record_set = pkPackAsset->GetRecordSet();
						pkTargetPlayer->HandlePackOPRecord(record_set, lAddItemReason, 0);
					}
					else
					{
						MG_Message kErrorMsg;
						kErrorMsg.m_iMessageID = RE_ITEM_CANT_USE;
						pkTargetPlayer->SendMsgToClient(MGPT_MESSAGE, &kErrorMsg);
						return iResult;
					}
				}
				else
				{
					MG_Message kErrorMsg;
					kErrorMsg.m_iMessageID = RE_ITEM_USE_FAIL_CANT_ADD_NEW_ITEM;
					kErrorMsg.m_iParameter1 = lItemID;
					kErrorMsg.m_iParameter2 = lAddItemID;
					pkTargetPlayer->SendMsgToClient(MGPT_MESSAGE, &kErrorMsg);
					return iResult;
				}
			}
			break;
		case SCIA_LEARN_SKILL:
			{
				int lLearnSkillID = lPara0;
				//int lThreshold = lPara1;
				if (IS_SKILL_TYPE(lLearnSkillID))
				{
					bool bLearnSuccess = false;
					int iSlotIndex = -1;
					bLearnSuccess = pkTargetPlayer->LearnSkill(lLearnSkillID, iSlotIndex);
					if (bLearnSuccess)
					{
						int iResult = pkTargetPlayer->ConsumeItemsInPackageByID(lItemID, iItemCount);
						//扣除物品成功
						if (RE_SUCCESS == iResult)
						{
							//扣除物品消息
							_handleCostEvent(lSrcID, pkTarget->GetID(), lLogicID, SK_CostEvent::SCT_ITEM, 0, lItemID, iItemCount);
						}
						else
						{
							MG_Message kErrorMsg;
							kErrorMsg.m_iMessageID = RE_ITEM_CANT_USE;
							pkTargetPlayer->SendMsgToClient(MGPT_MESSAGE, &kErrorMsg);
							return iResult;
						}
					}
				}
			}
			break;
		case SCIA_SPAWN_NPC:
			{
			}
			break;
		case SCIA_ADD_BindYuanBao:
			{
				//			MoneyType llCurrentBindYuanBao = pkTargetPlayer->GetBindYuanBao().GetMoney();
				MoneyType llAddBindYuanBao = lPara0;
				if (!IS_VALID_CHAR_MONEY(llAddBindYuanBao))
				{
					const unique_id_impl& instance = pkTargetPlayer->GetInstance();
					GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "add[%d]Bind Yuanbao error-1, c_cid:%d, c_citizenship:%d, c_uid:%d", llAddBindYuanBao, instance.detail(), instance.catagory(), pkTargetPlayer->GetUserID());
					return RE_FAIL;
				}
				//pkTargetPlayer->GetBindYuanBao().Add(llAddBindYuanBao);
				bool bAddCashSuccess = pkTargetPlayer->AddCash(llAddBindYuanBao, CT_BIND_YUANBAO, SYS_FUNC_TP_ITEM_USE, lItemID);

				if (bAddCashSuccess)
				{
					int iResult = pkTargetPlayer->ConsumeItemsInPackageByID(lItemID, iItemCount);
					//扣除物品成功
					if (RE_SUCCESS == iResult)
					{
						//扣除物品消息
						_handleCostEvent(lSrcID, pkTarget->GetID(), lLogicID, SK_CostEvent::SCT_ITEM, 0, lItemID, iItemCount);
					}
					else
					{
						MG_Message kErrorMsg;
						kErrorMsg.m_iMessageID = RE_ITEM_CANT_USE;
						pkTargetPlayer->SendMsgToClient(MGPT_MESSAGE, &kErrorMsg);
						return iResult;
					}
				}
				else
				{
					const unique_id_impl& instance = pkTargetPlayer->GetInstance();
					GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "add[%d]Bind Yuanbao error-2, c_cid:%d, c_citizenship:%d, c_uid:%d", llAddBindYuanBao, instance.detail(), instance.catagory(), pkTargetPlayer->GetUserID());
				}
			}
			break;
		default:
			break;
	}
	return RE_SUCCESS;
}
//-------------------------------------------------------------------------------------
int SC_ServerSkillInterface::CostItemByIDAddSkillState(GameLogic::LC_Attacker* pkScriptCaller, int iCostTargetIndex, int iAddStateTargetIndex, object_id_type lSrcID, object_id_type lLogicID, int lItemID, int iItemCount, int lSkillStateID, int lStateTime, float fPara0, float fPara1, float fPara2, float fPara3, float fPara4)
{
	if (NULL == pkScriptCaller)
	{
		return RE_FAIL;
	}
	if (!IS_ITEM_ID(lItemID) || iItemCount <= 0)
	{
		return RE_FAIL;
	}

	LC_ActorPtrList kCostTargetList;
	_findTarget(pkScriptCaller, iCostTargetIndex, kCostTargetList);
	//只能对一个目标扣除物品
	if (1 != kCostTargetList.size())
	{
		return RE_FAIL;
	}
	LC_ActorBase* pkCostTarget = kCostTargetList.front();
	if (NULL == pkCostTarget || !IsPlayerID(pkCostTarget->GetID()))
	{
		return RE_FAIL;
	}
	LC_ServerPlayer* pkCostTargetPlayer = (LC_ServerPlayer*)pkCostTarget;
	/*LC_PackAsset* pkPackAsset = pkCostTargetPlayer->GetPackAsset();
	if (NULL == pkPackAsset)
	{
		return RE_FAIL;
	}*/

	LC_ActorPtrList kAddStateTargetList;
	_findTarget(pkScriptCaller, iAddStateTargetIndex, kAddStateTargetList);
	//不存在需要添加状态的目标
	if (kAddStateTargetList.empty())
	{
		return RE_FAIL;
	}

	//物品是否可以被消耗
	ResultType iCheckResult = pkCostTargetPlayer->CheckConsumeItemByID(lItemID, iItemCount);
	if (RE_SUCCESS != iCheckResult)
	{
		return iCheckResult;
	}

	//只对一个目标添加状态，需要判断状态是否添加成功
	if (1 == kAddStateTargetList.size())
	{
		LC_ActorBase* pkTarget = kAddStateTargetList.front();
		if (NULL != pkTarget)
		{
			int iResult = pkTarget->ActiveSkillState(lSkillStateID, lStateTime, lSrcID, fPara0, fPara1, fPara2, fPara3, fPara4);
			if (RE_SKILL_STATE_ADD_SUCCESS != iResult)
			{
				//注意：其它添加状态失败消息会跟技能事件送达客户端，
				//唯有RE_SKILL_STATE_ADD_FAIL_FULL因为设计问题不能送达客户端
				if (RE_SKILL_STATE_ADD_FAIL_FULL == iResult)
				{
					MG_Message kErrorMsg;
					kErrorMsg.m_iMessageID = RE_SKILL_STATE_ADD_FAIL_EXCLUDE;
					pkCostTargetPlayer->SendMsgToClient(MGPT_MESSAGE, &kErrorMsg);
				}
				return iResult;
			}

			iResult = pkCostTargetPlayer->ConsumeItemsInPackageByID(lItemID, iItemCount);
			//扣除物品成功
			if (RE_SUCCESS == iResult)
			{
				//扣除物品消息
				_handleCostEvent(lSrcID, pkCostTarget->GetID(), lLogicID, SK_CostEvent::SCT_ITEM, 0, lItemID, iItemCount);
			}
			else
			{
				MG_Message kErrorMsg;
				kErrorMsg.m_iMessageID = RE_ITEM_CANT_USE;
				pkCostTargetPlayer->SendMsgToClient(MGPT_MESSAGE, &kErrorMsg);
				return RE_FAIL;
			}
		}
	}
	//对一批目标添加状态
	else
	{
		LC_ActorPtrListIter iter;
		LC_ActorBase* pkTarget = NULL;
		TRAV(kAddStateTargetList, iter)
		{
			pkTarget = *iter;
			pkTarget->ActiveSkillState(lSkillStateID, lStateTime, lSrcID, fPara0, fPara1, fPara2, fPara3, fPara4);
		}
	}
	return RE_SUCCESS;
}
int SC_ServerSkillInterface::CostItemByIDAddSkillStateToController(GameLogic::LC_Attacker* pkScriptCaller, int iCostTargetIndex, int iAddStateTargetIndex, object_id_type lSrcID, object_id_type lLogicID, int lItemID, int iItemCount, int lSkillStateID, int lStateTime, float fPara0, float fPara1, float fPara2, float fPara3, float fPara4)
{
	if (NULL == pkScriptCaller)
	{
		return RE_FAIL;
	}
	if (!IS_ITEM_ID(lItemID) || iItemCount <= 0)
	{
		return RE_FAIL;
	}

	LC_ActorPtrList kCostTargetList;
	_findTarget(pkScriptCaller, iCostTargetIndex, kCostTargetList);
	//只能对一个目标扣除物品
	if (1 != kCostTargetList.size())
	{
		return RE_FAIL;
	}
	LC_ActorBase* pkCostTarget = kCostTargetList.front();
	if (NULL == pkCostTarget || !IsPlayerID(pkCostTarget->GetID()))
	{
		return RE_FAIL;
	}
	LC_ServerPlayer* pkCostTargetPlayer = (LC_ServerPlayer*)pkCostTarget;
	/*LC_PackAsset* pkPackAsset = pkCostTargetPlayer->GetPackAsset();
	if (NULL == pkPackAsset)
	{
		return RE_FAIL;
	}*/

	LC_ActorPtrList kAddStateTargetList;
	_findTarget(pkScriptCaller, iAddStateTargetIndex, kAddStateTargetList);
	//不存在需要添加状态的目标
	if (kAddStateTargetList.empty())
	{
		return RE_FAIL;
	}

	//物品是否可以被消耗
	ResultType iCheckResult = pkCostTargetPlayer->CheckConsumeItemByID(lItemID, iItemCount);
	if (RE_SUCCESS != iCheckResult)
	{
		return iCheckResult;
	}

	//只对一个目标添加状态，需要判断状态是否添加成功
	if (1 == kAddStateTargetList.size())
	{
		LC_ActorBase* pkTarget = kAddStateTargetList.front();
		// 这个返回值，是按照原逻辑的
		if (pkTarget == NULL)
			return RE_SUCCESS;
		if (pkTarget->GetType() != LOT_PLAYER)
			return RE_FAIL;
		LC_ServerPlayer* pTargetPlayer = (LC_ServerPlayer*)pkTarget;
		int iResult = pTargetPlayer->ActiveSkillStateToAllBuddyInstance(lSkillStateID, lStateTime, lSrcID, fPara0, fPara1, fPara2, fPara3, fPara4);
		if (RE_SKILL_STATE_ADD_SUCCESS != iResult)
		{
			//注意：其它添加状态失败消息会跟技能事件送达客户端，
			//唯有RE_SKILL_STATE_ADD_FAIL_FULL因为设计问题不能送达客户端
			if (RE_SKILL_STATE_ADD_FAIL_FULL == iResult)
			{
				MG_Message kErrorMsg;
				kErrorMsg.m_iMessageID = RE_SKILL_STATE_ADD_FAIL_EXCLUDE;
				pkCostTargetPlayer->SendMsgToClient(MGPT_MESSAGE, &kErrorMsg);
			}
			return iResult;
		}

		iResult = pkCostTargetPlayer->ConsumeItemsInPackageByID(lItemID, iItemCount);
		//扣除物品成功
		if (RE_SUCCESS == iResult)
		{
			//扣除物品消息
			_handleCostEvent(lSrcID, pkCostTarget->GetID(), lLogicID, SK_CostEvent::SCT_ITEM, 0, lItemID, iItemCount);
		}
		else
		{
			MG_Message kErrorMsg;
			kErrorMsg.m_iMessageID = RE_ITEM_CANT_USE;
			pkCostTargetPlayer->SendMsgToClient(MGPT_MESSAGE, &kErrorMsg);
			return RE_FAIL;
		}
	}
	//对一批目标添加状态
	else
	{
		LC_ActorPtrListIter iter;
		LC_ActorBase* pkTarget = NULL;
		TRAV(kAddStateTargetList, iter)
		{
			pkTarget = *iter;
			if (pkTarget->GetType() != LOT_PLAYER)
				continue;

			LC_ServerPlayer* pTargetPlayer = (LC_ServerPlayer*)pkTarget;
			pTargetPlayer->ActiveSkillStateToAllBuddyInstance(lSkillStateID, lStateTime, lSrcID, fPara0, fPara1, fPara2, fPara3, fPara4);
		}
	}
	return RE_SUCCESS;
}
//-------------------------------------------------------------------------------------
void SC_ServerSkillInterface::SetTaskFinishState(GameLogic::LC_Attacker* pkScriptCaller, int lTaskID, int iIndex, int lState)
{
	if (pkScriptCaller == NULL || !IS_PLAYER_ID(pkScriptCaller->GetID()))
	{
		return;
	}
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkScriptCaller;
	LC_TaskMap* pkTaskMap = pkPlayer->GetTaskMap();
	if (pkTaskMap == NULL)
	{
		return;
	}
	LC_ProcessingTaskEntry* pkTaskEntry = NULL;
	pkTaskEntry = pkTaskMap->GetProcessingTaskEntryByID(lTaskID);
	if (pkTaskEntry == NULL)
	{
		return;
	}
	int iOldValue = LC_Helper::GetTaskFinishState(pkTaskEntry, iIndex - 1);
	if (lState != iOldValue)
	{
		LC_Helper::SetTaskFinishState(pkTaskEntry, iIndex - 1, lState);
		pkPlayer->UpdateDirtyFlag(DIRTY_FLAG_PROCESSINGTASK);
	}
}

//-------------------------------------------------------------------------------------
void SC_ServerSkillInterface::SetTaskTraceIndex(GameLogic::LC_Attacker* pkScriptCaller, int lTaskID, int iIndex)
{
	if (pkScriptCaller == NULL || !IS_PLAYER_ID(pkScriptCaller->GetID()))
	{
		return;
	}
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkScriptCaller;
	LC_TaskMap* pkTaskMap = pkPlayer->GetTaskMap();
	if (pkTaskMap == NULL)
	{
		return;
	}
	LC_ProcessingTaskEntry* pkTaskEntry = NULL;
	pkTaskEntry = pkTaskMap->GetProcessingTaskEntryByID(lTaskID);
	if (pkTaskEntry == NULL)
	{
		return;
	}
	pkTaskEntry->SetTaskTraceIndex(iIndex);
	pkPlayer->UpdateDirtyFlag(DIRTY_FLAG_PROCESSINGTASK);
}
//-------------------------------------------------------------------------------------
void SC_ServerSkillInterface::ShowStoryMessage(int lMsgID, const StringType& rkMsgKey, int iTargetType, int iTargetID, int iShowType, int lParam)
{
	CF_GameStoryMessage* pkStoryMessage = CF_GameStoryMessage::GetSingletonPtr();
	if (lMsgID <= 0)
	{
		CF_GameStoryMessage::DataEntryMapExternIterator kIter = pkStoryMessage->GetIterator();
		while (kIter.HasMoreElements())
		{
			CF_GameStoryMessage::DataEntry* pkData = kIter.PeekNextValuePtr();
			if (rkMsgKey == pkData->_sMsgKey)
			{
				lMsgID = pkData->_iID;
			}
			kIter.MoveNext();
		}
	}

	if (lMsgID > 0)
	{
		LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
		LC_GameEvent* pkGameEvent = pkEventManager->AllocGameEvent();
		if (pkGameEvent == NULL)
		{
			return;
		}
		pkGameEvent->SetEventType(GLET_GAME_STORY_MSG);
		pkGameEvent->SetEventParam(EVENT_SEND_TYPE_PARAM, iTargetType);
		pkGameEvent->SetEventParam(EP_STORY_MSG_TARGET_TYPE, iTargetType);
		pkGameEvent->SetEventParam(EP_STORY_MSG_TARGET_ID, iTargetID);
		pkGameEvent->SetEventParam(EP_STORY_MSG_ID, lMsgID);
		pkGameEvent->SetEventParam(EP_STORY_MSG_SHOW_TYPE, iShowType);
		pkGameEvent->SetEventParam(EP_STORY_MSG_SHOW_PARAM, lParam);
		pkEventManager->InsertGameEvent(pkGameEvent);
	}
}
//-------------------------------------------------------------------------------------
void SC_ServerSkillInterface::ShowStoryMessageByString(int lMsgID, const StringType& rkMsgKey, int iTargetType, int iTargetID, int iShowType, int lParam, const StringType& strParam)
{
	CF_GameStoryMessage* pkStoryMessage = CF_GameStoryMessage::GetSingletonPtr();
	LC_PlayerManagerBase* pkPlayerManager = ENGINE_GET_GAMEPLAYER_MANAGER();
	if (pkPlayerManager == NULL)
	{
		return;
	}
	if (lMsgID <= 0)
	{
		CF_GameStoryMessage::DataEntryMapExternIterator kIter = pkStoryMessage->GetIterator();
		while (kIter.HasMoreElements())
		{
			CF_GameStoryMessage::DataEntry* pkData = kIter.PeekNextValuePtr();
			if (rkMsgKey == pkData->_sMsgKey)
			{
				lMsgID = pkData->_iID;
				break;
			}
			kIter.MoveNext();
		}
	}

	if (lMsgID > 0)
	{

		LC_GameEvent obj;

		obj.SetEventType(GLET_GAME_STORY_MSG);
		obj.SetEventParam(EVENT_SEND_TYPE_PARAM, iTargetType);
		obj.SetEventParam(EP_STORY_MSG_TARGET_TYPE, iTargetType);
		obj.SetEventParam(EP_STORY_MSG_TARGET_ID, iTargetID);
		obj.SetEventParam(EP_STORY_MSG_ID, lMsgID);
		obj.SetEventParam(EP_STORY_MSG_SHOW_TYPE, iShowType);
		obj.SetEventParam(EP_STORY_MSG_SHOW_PARAM, lParam);

		MG_PB<msg_define::game_event> EventMsg;
		msg_define::game_event& msg_val = EventMsg.m_value;
		obj.SetToProtocol(msg_val);
		//std::string strName;
		//TPSTR2STDSTR(strParam,strName);
		msg_val.set_string_param(TPSTR2STDSTR(strParam));

		if (iTargetType == MST_PRIVATE)
		{
			LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer((object_id_type)iTargetID);
			if (pkPlayer)
				pkPlayer->SendMsgToClient(MGPT_RLT_GAME_EVENT, &EventMsg);
		}
		else if (iTargetType == MST_GROUP)
		{

		}
		else if (iTargetType == MST_NEAR)
		{
			LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
			LC_ActorBase* pkSrcActor = (LC_ActorBase*)pkWorldManager->FindObject(iTargetID);
			if (NULL != pkSrcActor)
			{
				LC_ServerMap* pkMap = (LC_ServerMap*)pkSrcActor->GetMap();
				if (NULL != pkMap)
				{
					pkMap->BroadCastToArea(pkSrcActor->GetSectorIndex(), MGPT_RLT_GAME_EVENT, &EventMsg);
				}
			}
		}
	}
}
//-------------------------------------------------------------------------------------
void SC_ServerSkillInterface::SetCareerType(GameLogic::LC_ActorBase* pkActor, int iCareerType)
{
	if (NULL == pkActor)
	{
		return;
	}
	object_id_type lID = pkActor->GetID();
	if (IS_PLAYER_ID(lID))
	{
		((LC_PlayerBase*)pkActor)->SetCareerType(CareerType(iCareerType));
		((LC_PlayerBase*)pkActor)->UpdateDirtyFlag(DIRTY_FLAG_ATTRIBUTE);
	}
}


void SC_ServerSkillInterface::SetFactionID(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iFactionID)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	//检查阵营信息是否存在
	CF_Faction* pkFactionList = CF_Faction::GetSingletonPtr();
	CF_Faction::DataEntry* pkFactionData = pkFactionList->GetEntryPtr(iFactionID);
	if (NULL == pkFactionData)
	{
		return;
	}

	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	LC_ActorPtrListIter iter;
	LC_ActorBase* pkTarget = NULL;
	TRAV(kTargetsList, iter)
	{
		pkTarget = *iter;
		//modify lxm 因为成昆有一个离间主角的技能，所以这边在判断ID的时候，去掉IS_NPC_ID判断
		if (NULL == pkTarget /*|| !IS_NPC_ID(pkTarget->GetID())*/)
		{
			continue;
		}
		pkTarget->SetFactionID(iFactionID);
	}
}

bool SC_ServerSkillInterface::ChangeActorAIFunc(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iAIFuncID)
{
	if (NULL == pkScriptCaller)
	{
		return false;
	}

	CF_MonsterAIFunc* pkAIFuncList = CF_MonsterAIFunc::GetSingletonPtr();
	CF_MonsterAIFunc::DataEntry* pkAIFuncData = pkAIFuncList->GetEntryPtr(iAIFuncID);
	if (NULL == pkAIFuncData)
	{
		return false;
	}

	//获取仇恨源
	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	LC_ActorPtrListIter iter;
	LC_ActorBase* pkTarget = NULL;
	//	LC_ActorHatredList* pkHatredList = NULL;
	LC_ActorHatredEntry kHatredEntry;
	CF_CharType::DataEntry* pkCharData = NULL;
	TRAV(kTargetsList, iter)
	{
		pkTarget = *iter;
		if (NULL == pkTarget || !IS_NPC_ID(pkTarget->GetID()))
		{
			continue;
		}
		pkCharData = pkTarget->GetCharTypePtr();
		if (NULL == pkCharData)
		{
			continue;
		}
		((LC_ServerNPC*)pkTarget)->InitAIData(pkCharData, pkAIFuncData);
	}
	return true;
}

//----------------------------------------------------------------------------------------
int SC_ServerSkillInterface::GetSortHatredByIndex(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iHatredIndex, bool bExcludeCurrentHatred)
{
	if (NULL == pkScriptCaller)
	{
		return 0;
	}

	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	if (kTargetsList.empty())
	{
		return 0;
	}

	LC_ActorBase* pkTarget = kTargetsList.front();
	if (NULL == pkTarget)
	{
		return 0;
	}

	LC_ActorHatredList* pkHatredList = pkTarget->GetHatredList();
	if (bExcludeCurrentHatred)
	{
		//获取当前锁定目标在仇恨列表中的位置
		int lCurrentHatredID = pkTarget->GetLockedTargetID();
		int iCurrentHatredIndex = pkHatredList->GetSortHatredIndex(lCurrentHatredID);
		if (iCurrentHatredIndex >= 0 && iCurrentHatredIndex <= iHatredIndex)
		{
			LC_ActorHatredEntry* pkEntry = pkHatredList->GetSortHatredByIndex(iHatredIndex + 1);
			if (NULL != pkEntry)
			{
				return pkEntry->GetTargetID();
			}
		}
		return 0;
	}
	else
	{
		LC_ActorHatredEntry* pkEntry = pkHatredList->GetSortHatredByIndex(iHatredIndex);
		if (NULL != pkEntry)
		{
			return pkEntry->GetTargetID();
		}
		return 0;
	}
}
//----------------------------------------------------------------------------------------
attr_value_type SC_ServerSkillInterface::GetHatredValueByIndex(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iHatredIndex, bool bExcludeCurrentHatred)
{
	if (NULL == pkScriptCaller)
	{
		return 0;
	}

	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	if (kTargetsList.empty())
	{
		return 0;
	}

	LC_ActorBase* pkTarget = kTargetsList.front();
	if (NULL == pkTarget)
	{
		return 0;
	}

	LC_ActorHatredList* pkHatredList = pkTarget->GetHatredList();

	if (bExcludeCurrentHatred)
	{
		//获取当前锁定目标在仇恨列表中的位置
		int lCurrentHatredID = pkTarget->GetLockedTargetID();
		int iCurrentHatredIndex = pkHatredList->GetSortHatredIndex(lCurrentHatredID);
		if (iCurrentHatredIndex >= 0 && iCurrentHatredIndex <= iHatredIndex)
		{
			LC_ActorHatredEntry* pkEntry = pkHatredList->GetSortHatredByIndex(iHatredIndex + 1);
			if (NULL != pkEntry)
			{
				return pkEntry->GetHatredValue();
			}
		}
		return 0;
	}
	else
	{
		LC_ActorHatredEntry* pkEntry = pkHatredList->GetSortHatredByIndex(iHatredIndex);
		if (NULL != pkEntry)
		{
			return pkEntry->GetHatredValue();
		}
		return 0;
	}
}
//----------------------------------------------------------------------------------------
attr_value_type SC_ServerSkillInterface::GetHatredValueByID(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lHatredID)
{
	if (NULL == pkScriptCaller)
	{
		return 0;
	}

	//获取仇恨列表源
	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	if (kTargetsList.empty())
	{
		return 0;
	}
	LC_ActorBase* pkTarget = kTargetsList.front();
	if (NULL == pkTarget)
	{
		return 0;
	}

	//获取仇恨目标
	LC_ActorPtrList kHatredPtrList;
	_findTarget(pkTarget, lHatredID, kHatredPtrList);
	if (kHatredPtrList.size() != 1)
	{
		return 0;
	}
	LC_ActorBase* pkHatred = kHatredPtrList.front();
	if (NULL == pkHatred)
	{
		return 0;
	}

	LC_ActorHatredList* pkHatredList = pkTarget->GetHatredList();
	LC_ActorHatredEntry* pkEntry = pkHatredList->GetHatred(pkHatred->GetID());
	if (NULL != pkEntry)
	{
		return pkEntry->GetHatredValue();
	}
	return 0;
}
//----------------------------------------------------------------------------------------
int	SC_ServerSkillInterface::GetHatredCount(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex)
{
	if (NULL == pkScriptCaller)
	{
		return 0;
	}

	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	if (kTargetsList.empty())
	{
		return 0;
	}

	LC_ActorBase* pkTarget = kTargetsList.front();
	if (NULL == pkTarget)
	{
		return 0;
	}

	LC_ActorHatredList* pkHatredList = pkTarget->GetHatredList();
	return pkHatredList->GetHatredCount();
}
//----------------------------------------------------------------------------------------
void SC_ServerSkillInterface::SetHatredValueByIndex(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iHatredIndex, int lHatredValue, bool bExcludeCurrentHatred)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	if (kTargetsList.empty())
	{
		return;
	}

	LC_ActorBase* pkTarget = kTargetsList.front();
	if (NULL == pkTarget)
	{
		return;
	}

	LC_ActorHatredList* pkHatredList = pkTarget->GetHatredList();

	if (bExcludeCurrentHatred)
	{
		//获取当前锁定目标在仇恨列表中的位置
		int lCurrentHatredID = pkTarget->GetLockedTargetID();
		int iCurrentHatredIndex = pkHatredList->GetSortHatredIndex(lCurrentHatredID);
		if (iCurrentHatredIndex >= 0 && iCurrentHatredIndex <= iHatredIndex)
		{
			LC_ActorHatredEntry* pkEntry = pkHatredList->GetSortHatredByIndex(iHatredIndex + 1);
			if (NULL != pkEntry)
			{
				pkEntry->SetHatredValue(lHatredValue);
			}
		}
	}
	else
	{
		LC_ActorHatredEntry* pkEntry = pkHatredList->GetSortHatredByIndex(iHatredIndex);
		if (NULL != pkEntry)
		{
			pkEntry->SetHatredValue(lHatredValue);
		}
	}
}
//----------------------------------------------------------------------------------------
void SC_ServerSkillInterface::SetHatredValueByID(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lHatredID, int lHatredValue)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	//获取仇恨源
	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	if (kTargetsList.empty())
	{
		return;
	}
	LC_ActorBase* pkTarget = kTargetsList.front();
	if (NULL == pkTarget)
	{
		return;
	}

	//获取仇恨目标
	LC_ActorPtrList kHatredPtrList;
	_findTarget(pkTarget, lHatredID, kHatredPtrList);
	if (kHatredPtrList.size() != 1)
	{
		return;
	}
	LC_ActorBase* pkHatred = kHatredPtrList.front();
	if (NULL == pkHatred)
	{
		return;
	}

	LC_ActorHatredList* pkHatredList = pkTarget->GetHatredList();
	LC_ActorHatredEntry* pkEntry = pkHatredList->GetHatred(pkHatred->GetID());
	if (NULL != pkEntry)
	{
		pkEntry->SetHatredValue(lHatredValue);
	}
}
//----------------------------------------------------------------------------------------
void SC_ServerSkillInterface::AddHatred(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lHatredID, int lHatredValue)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	if (!IS_PLAYER_CLASS_ID(lHatredID) && !IS_NPC_ID(lHatredID))
	{
		return;
	}

	//获取仇恨源
	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	LC_ActorPtrListIter iter;
	LC_ActorBase* pkTarget = NULL;
	LC_ActorHatredList* pkHatredList = NULL;
	LC_ActorHatredEntry kHatredEntry;
	TRAV(kTargetsList, iter)
	{
		pkTarget = *iter;
		if (NULL == pkTarget)
		{
			continue;
		}

		pkHatredList = pkTarget->GetHatredList();

		kHatredEntry.SetTargetID(lHatredID);
		kHatredEntry.SetHatredValue(lHatredValue);
		kHatredEntry.SetUpdateTime(GET_PROCESS_TIME);
		pkHatredList->AddHatred(kHatredEntry);
	}
}
//----------------------------------------------------------------------------------------
void SC_ServerSkillInterface::RemoveHatredByIndex(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iHatredIndex, bool bExcludeCurrentHatred)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	if (kTargetsList.empty())
	{
		return;
	}

	LC_ActorBase* pkTarget = kTargetsList.front();
	if (NULL == pkTarget)
	{
		return;
	}

	LC_ActorHatredList* pkHatredList = pkTarget->GetHatredList();

	if (bExcludeCurrentHatred)
	{
		//获取当前锁定目标在仇恨列表中的位置
		int lCurrentHatredID = pkTarget->GetLockedTargetID();
		int iCurrentHatredIndex = pkHatredList->GetSortHatredIndex(lCurrentHatredID);
		if (iCurrentHatredIndex >= 0 && iCurrentHatredIndex <= iHatredIndex)
		{
			pkHatredList->RemoveHatred(lCurrentHatredID);
		}
	}
	else
	{
		pkHatredList->RemoveHatred(pkTarget->GetID());
	}
}
//----------------------------------------------------------------------------------------
void SC_ServerSkillInterface::RemoveHatred(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lHatredID)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	//获取仇恨源
	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	if (kTargetsList.empty())
	{
		return;
	}
	LC_ActorBase* pkTarget = kTargetsList.front();
	if (NULL == pkTarget)
	{
		return;
	}

	//获取仇恨目标
	LC_ActorPtrList kHatredPtrList;
	_findTarget(pkTarget, lHatredID, kHatredPtrList);
	if (kHatredPtrList.size() != 1)
	{
		return;
	}
	LC_ActorBase* pkHatred = kHatredPtrList.front();
	if (NULL == pkHatred)
	{
		return;
	}

	LC_ActorHatredList* pkHatredList = pkTarget->GetHatredList();
	pkHatredList->RemoveHatred(pkHatred->GetID());
}
//----------------------------------------------------------------------------------------
void SC_ServerSkillInterface::ClearHatred(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	if (kTargetsList.empty())
	{
		return;
	}

	LC_ActorBase* pkTarget = kTargetsList.front();
	if (NULL == pkTarget)
	{
		return;
	}

	LC_ActorHatredList* pkHatredList = pkTarget->GetHatredList();

	pkHatredList->ResetHatredValue();
	//pkHatredList->Reset();
}
//----------------------------------------------------------------------------------------
void SC_ServerSkillInterface::AddHatredSkill(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lHatredID, int lSkillID, int iPriority, float fSkillLifeTime)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	if (!IS_SKILL_TYPE(lSkillID))
	{
		return;
	}

	//获取仇恨源
	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	LC_ActorPtrListIter iter;
	LC_ActorBase* pkTarget = NULL;
	LC_ActorSkillList* pkSkillList = NULL;
	LC_ActorSkillEntry kSkillEntry;
	TRAV(kTargetsList, iter)
	{
		pkTarget = *iter;
		if (NULL == pkTarget)
		{
			continue;
		}

		pkSkillList = pkTarget->GetActorSkillList();

		kSkillEntry.SetLockedTargetID(lHatredID);
		kSkillEntry.SetSkillTypeID(lSkillID);
		kSkillEntry.SetTriggerIndex(-1);
		kSkillEntry.SetStartTime(-1.0);		//设置技能没有启动，挂起技能时设置挂起时间
		kSkillEntry.SetLifeTime(fSkillLifeTime);
		kSkillEntry.SetPriority(iPriority);
		pkSkillList->AddSkillEntry(kSkillEntry);
	}
}
//----------------------------------------------------------------------------------------
void SC_ServerSkillInterface::ClearHatredSkillList(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	if (kTargetsList.empty())
	{
		return;
	}

	LC_ActorBase* pkTarget = kTargetsList.front();
	if (NULL == pkTarget)
	{
		return;
	}

	LC_ActorSkillList* pkSkillList = pkTarget->GetActorSkillList();
	pkSkillList->Reset();
}
//----------------------------------------------------------------------------------------
void SC_ServerSkillInterface::ProcessCloneMonsterAfterSpawn(GameLogic::LC_Attacker* pkScriptCaller, int iSourceIndex, int iTargetIndex)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}
	//get source
	LC_ActorPtrList kSrcList;
	_findTarget(pkScriptCaller, iSourceIndex, kSrcList);
	if (kSrcList.size() != 1)
	{
		return;
	}
	LC_ActorBase* pkSrcMonster = kSrcList.front();
	if (NULL == pkSrcMonster)
	{
		return;
	}
	if (!IS_NPC_ID(pkSrcMonster->GetID()))
	{
		return;
	}
	//get target
	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	if (kTargetsList.empty())
	{
		return;
	}
	LC_ActorBase* pkCloneMonster = kTargetsList.front();
	if (NULL == pkCloneMonster)
	{
		return;
	}
	if (!IS_NPC_ID(pkCloneMonster->GetID()))
	{
		return;
	}

	//开始处理
	//仇恨列表
	LC_ActorHatredList* pkSrcHatredlist = pkSrcMonster->GetHatredList();
	LC_ActorHatredList* pkDstHatredlist = pkCloneMonster->GetHatredList();
	LC_ActorHatredEntry* pkEntry = pkSrcHatredlist->GetSortHatredByIndex(0);
	if (NULL != pkEntry)
	{
		LC_ActorHatredEntry kHatredEntry = *pkEntry;
		kHatredEntry.SetUpdateTime(GET_PROCESS_TIME);
		pkDstHatredlist->AddHatred(kHatredEntry);
	}

	//hp
	attr_value_type	hp = pkSrcMonster->GetHP();
	pkCloneMonster->SetHP(hp);

	//精英怪等级
	LC_NPCBase* pkSrcNPC = dynamic_cast<LC_NPCBase*>(pkSrcMonster);
	LC_NPCBase* pkCloneNPC = dynamic_cast<LC_NPCBase*>(pkCloneMonster);
	pkCloneNPC->SetEliteLevel(pkSrcNPC->GetEliteLevel());

	//克隆物
	pkCloneNPC->SetProperty_IsClone(true);

	//技能
	LC_ActorSkillTrigger* pkSkillList = pkCloneMonster->GetSkillTrigger();
	pkSkillList->ResetExceptFirst();
	pkCloneMonster->SetDyingSkillID(0);
}
//----------------------------------------------------------------------------------------
object_id_type SC_ServerSkillInterface::SelectLockedTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex)
{
	if (NULL == pkScriptCaller)
	{
		return 0;
	}

	LC_ActorPtrList kSrcList;
	_findTarget(pkScriptCaller, iTargetIndex, kSrcList);
	if (kSrcList.size() != 1)
	{
		return 0;
	}
	LC_ActorBase* pkSrcActor = kSrcList.front();
	if (NULL == pkSrcActor)
	{
		return 0;
	}

	//获取锁定的所有目标
	LC_ActorPtrList kTargetList;
	_findTarget(pkSrcActor, TARGET_ALL, kTargetList);
	LC_ActorPtrListIter iter;
	LC_ActorBase* pkTarget = NULL;
	object_id_type lSelectTarget = 0;
	TRAV(kTargetList, iter)
	{
		pkTarget = *iter;
		if ( pkTarget->GetHP() > 0 )
		{
			lSelectTarget = pkTarget->GetID();
		}
	}
	return lSelectTarget;
}
//----------------------------------------------------------------------------------------
void SC_ServerSkillInterface::AddAdditionHatred(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lHatredID, float fDurationTime)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	if (!IS_PLAYER_CLASS_ID(lHatredID) && !IS_NPC_ID(lHatredID))
	{
		return;
	}

	//获取仇恨源
	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	LC_ActorPtrListIter iter;
	LC_ActorBase* pkTarget = NULL;
	LC_ActorSkillList* pkSkillList = NULL;
	TRAV(kTargetsList, iter)
	{
		pkTarget = *iter;
		if (NULL == pkTarget)
		{
			continue;
		}
		pkTarget->SetAdditionTargetID(lHatredID);
		pkTarget->SetAdditionTargetBeginTime(GET_PROCESS_TIME);
		pkTarget->SetAdditionTargetDurationTime(fDurationTime);

		pkSkillList = pkTarget->GetActorSkillList();
		pkSkillList->Reset();
	}
}
//-------------------------------------------------------------------------------------
void SC_ServerSkillInterface::ClearAdditionHatred(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	//获取仇恨源
	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	LC_ActorPtrListIter iter;
	LC_ActorBase* pkTarget = NULL;
	LC_ActorSkillList* pkSkillList = NULL;
	TRAV(kTargetsList, iter)
	{
		pkTarget = *iter;
		if (NULL == pkTarget)
		{
			continue;
		}
		pkTarget->SetAdditionTargetID(0);
		pkTarget->SetAdditionTargetBeginTime(0.0f);
		pkTarget->SetAdditionTargetDurationTime(0.0f);

		pkSkillList = pkTarget->GetActorSkillList();
		pkSkillList->Reset();
	}
}
//-------------------------------------------------------------------------------------
void SC_ServerSkillInterface::_handleCostEvent(object_id_type lSrcActorID, object_id_type lDstActorID, object_id_type lLogicID, SK_CostEvent::SK_CostType eCostType, int iCostValue, int lItemID, int iItemCount)
{
	SK_SkillEventManagerBase* pkEventManager = ENGINE_GET_SKILL_EVENT_MANAGER();
	SK_SkillEvent* pkEvent = pkEventManager->AllocEvent();
	if (NULL != pkEvent)
	{
		pkEvent->SetEventType(SETE_COST);
		pkEvent->SetSrcActorID(lSrcActorID);
		pkEvent->SetDstActorID(lDstActorID);
		pkEvent->SetLogicID(lLogicID);
		pkEvent->SetParam(SK_CostEvent::SEP_COST_TYPE, eCostType);
		switch (eCostType)
		{
			case SK_CostEvent::SCT_HP:
			case SK_CostEvent::SCT_MP:
				{
					pkEvent->SetParam(SK_CostEvent::SEP_COST_BASE, iCostValue);
					pkEvent->SetParam(SK_CostEvent::SEP_ITEM_ID, 0);
					pkEvent->SetParam(SK_CostEvent::SEP_ITEM_COUNT, 0);
				}
				break;
			case SK_CostEvent::SCT_ITEM:
				{
					pkEvent->SetParam(SK_CostEvent::SEP_COST_BASE, 0);
					pkEvent->SetParam(SK_CostEvent::SEP_ITEM_ID, lItemID);
					pkEvent->SetParam(SK_CostEvent::SEP_ITEM_COUNT, iItemCount);
				}
				break;
		}
		pkEventManager->AddEvent(pkEvent);
	}
}
//-------------------------------------------------------------------------------------
void SC_ServerSkillInterface::_handleDetachStateEvent(object_id_type lSrcActorID, object_id_type lDstActorID, object_id_type lLogicID, int lStateType, int iDetachCount)
{
	SK_SkillEventManagerBase* pkEventManager = ENGINE_GET_SKILL_EVENT_MANAGER();
	SK_SkillEvent* pkEvent = pkEventManager->AllocEvent();
	if (NULL != pkEvent)
	{
		pkEvent->SetEventType(SETE_DETACH_STATE);
		pkEvent->SetSrcActorID(lSrcActorID);
		pkEvent->SetDstActorID(lDstActorID);
		pkEvent->SetLogicID(lLogicID);
		if (IS_STATE_TYPE(lStateType))
		{
			//驱散一个状态
			pkEvent->SetParam(SK_DetachStateEvent::SEP_DETACH_TYPE, SK_DetachStateEvent::SDSR_SINGLE);
			pkEvent->SetParam(SK_DetachStateEvent::SEP_STATE_ID, lStateType);
			//驱散单个状态的层数
			pkEvent->SetParam(SK_DetachStateEvent::SEP_DETACH_COUNT, iDetachCount);
		}
		else
		{
			//驱散一类状态
			pkEvent->SetParam(SK_DetachStateEvent::SEP_DETACH_TYPE, SK_DetachStateEvent::SDSR_BATCH);
			pkEvent->SetParam(SK_DetachStateEvent::SEP_STATE_ID, 0);
			//驱散状态的数量
			pkEvent->SetParam(SK_DetachStateEvent::SEP_DETACH_COUNT, iDetachCount);
		}
		pkEventManager->AddEvent(pkEvent);
	}
}
////--------------------------------------------------------------------------------------------
//void SC_ServerSkillInterface::NotifyCareerSlectionSucceed(GameLogic::LC_ActorBase* pkMachineOwner,bool flag)
//{
//	LC_ServerPlayer* pkPlayer =   static_cast<LC_ServerPlayer*>(pkMachineOwner);
//	if(pkPlayer)
//	{
//		//pkPlayer->SetCareerSelectedSucceed(pkPlayer->GetCareerSelectedType(),flag);
//	}
//}
//--------------------------------------------------------------------------------------------
void SC_ServerSkillInterface::CostTargetBerserk(GameLogic::LC_ActorBase* pkScriptCaller, int iTargetIndex, int lSourceTypeID, int iCostValue)
{
}
//------------------------------------------------------------
void SC_ServerSkillInterface::RecoverTargetBerserk(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lSourceTypeID, int lRecoverValue)
{
}
//-------------------------------------------------------------------------------------
void SC_ServerSkillInterface::SetBerserkTimer(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, float fTimer)
{
}
//-------------------------------------------------------------------------------------
void SC_ServerSkillInterface::ResetBerserkTimer(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex)
{
}
//-------------------------------------------------------------------------------------
void SC_ServerSkillInterface::SetBerserkProbability(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, float fProbability)
{
}
//-------------------------------------------------------------------------------------
void SC_ServerSkillInterface::ResetBerserkProbability(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex)
{
}
//-------------------------------------------------------------------------------------
void SC_ServerSkillInterface::SetMaxBerserkPoint(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iMaxBerserk)
{
}
//-------------------------------------------------------------------------------------
void SC_ServerSkillInterface::_implShotBulletToTarget(GameLogic::LC_ActorBase* pkAttacker, object_id_type lTargetID, const Utility::UT_Vec3Int& rkTargetLocation, const ShotBulletArg & arg)
{
	// unpack
	int lSkillID = arg.lSkillTypeID;
	int iMoveType = arg.pkBullet->_lMoveType;
	int iDummyType = arg.iDummyType;
	int iPenetrateCount = UT_MathBase::ClampInt(arg.iPenetrateCount, 0, SK_BulletEvent::MAX_BULLET_PENETRATE_COUNT);
	float fStartTime = arg.fStartTime;
	float fMoveTime = arg.fMoveTime;
	float fPitchAngle = arg.fPitchAngle;
	int iMoveSpeed = arg.pkBullet->_lMoveSpeed;
	int iBulletFuncID = arg.iBulletFuncID;
	int iClientLocalID = arg.iClientLocalID;

	SK_ServerSkillEventManager* pkSkillEventManager = SERVER_GET_SKILL_EVENT_MANAGER();
	SK_SkillEvent* pkEvent = pkSkillEventManager->AllocEvent();
	if (NULL != pkEvent)
	{
		pkEvent->SetEventType(SETE_BULLET);
		pkEvent->SetSrcActorID(pkAttacker->GetID());
		pkEvent->SetDstActorID(lTargetID);
		pkEvent->SetLogicID(lSkillID);
		pkEvent->SetParam(SK_BulletEvent::SEP_MOVE_TYPE, iMoveType);
		pkEvent->SetParam(SK_BulletEvent::SEP_ACTION_TYPE, SK_BulletEvent::SBAT_SHOT | SK_BulletEvent::SBSS_FromServer);
		pkEvent->SetParam(SK_BulletEvent::SEP_DUMMY_TYPE, iDummyType);
		pkEvent->SetParam(SK_BulletEvent::SEP_MOVE_START_TIME, SK_SkillEvent::ParamFloat2LLong(fStartTime));
		pkEvent->SetParam(SK_BulletEvent::SEP_MOVE_LIFE_TIME, SK_SkillEvent::ParamFloat2LLong(fMoveTime));
		pkEvent->SetParam(SK_BulletEvent::SEP_PITCH_ANGLE, SK_SkillEvent::ParamFloat2LLong(fPitchAngle));
		pkEvent->SetParam(SK_BulletEvent::SEP_MOVE_SPEED, iMoveSpeed);
		pkEvent->SetParam(SK_BulletEvent::SEP_BULLET_FUNC, iBulletFuncID);
		pkEvent->SetParam(SK_BulletEvent::SEP_MAX_PENETRATE_COUNT, iPenetrateCount);
		pkEvent->SetParam(SK_BulletEvent::SEP_COLLIDE_PARAM, Skill::SerializeToQuatWord(SK_BulletEvent::CollideParam(arg.fCollidePosParamT, arg.iCollideSurfaceMaterial)));
		pkEvent->SetParam(SK_BulletEvent::SEP_BULLET_LOCAL_ID, iClientLocalID);
		pkEvent->SetParam(SK_BulletEvent::SEP_SHOT_LOCATION_X, arg.kShotLocation.x);
		pkEvent->SetParam(SK_BulletEvent::SEP_SHOT_LOCATION_Y, arg.kShotLocation.y);
		pkEvent->SetParam(SK_BulletEvent::SEP_SHOT_LOCATION_Z, arg.kShotLocation.z);
		pkEvent->SetParam(SK_BulletEvent::SEP_TARGET_LOCATION_X, rkTargetLocation.x);
		pkEvent->SetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Y, rkTargetLocation.y);
		pkEvent->SetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Z, rkTargetLocation.z);
		pkEvent->SetParam(SK_BulletEvent::SEP_LAST_UPDATE_TIME, SK_SkillEvent::ParamFloat2LLong(fStartTime));
		pkSkillEventManager->AddEvent(pkEvent);
	}
	//向客户端广播消息
	pkSkillEventManager->SendToClient(pkAttacker, pkEvent, SSET_BROADCAST_TO_AREA);
}

//--------------------------------------------------------------------------------------------
void SC_ServerSkillInterface::_implshotBulletToLocation(GameLogic::LC_ActorBase* pkAttacker, const Utility::UT_Vec3Int& rkTargetLocation, const ShotBulletArg & arg)
{
	// unpack
	int lSkillID = arg.lSkillTypeID;
	int iMoveType = arg.pkBullet->_lMoveType;
	int iDummyType = arg.iDummyType;
	int iPenetrateCount = UT_MathBase::ClampInt(arg.iPenetrateCount, 0, SK_BulletEvent::MAX_BULLET_PENETRATE_COUNT);
	float fStartTime = arg.fStartTime;
	float fMoveTime = arg.fMoveTime;
	float fPitchAngle = arg.fPitchAngle;
	int iMoveSpeed = arg.pkBullet->_lMoveSpeed;
	int iBulletFuncID = arg.iBulletFuncID;
	int iClientLocalID = arg.iClientLocalID;

	SK_ServerSkillEventManager* pkSkillEventManager = SERVER_GET_SKILL_EVENT_MANAGER();
	SK_SkillEvent* pkEvent = pkSkillEventManager->AllocEvent();
	if (NULL != pkEvent)
	{
		pkEvent->SetEventType(SETE_BULLET);
		pkEvent->SetSrcActorID(pkAttacker->GetID());
		pkEvent->SetDstActorID(0);
		pkEvent->SetLogicID(lSkillID);
		pkEvent->SetParam(SK_BulletEvent::SEP_MOVE_TYPE, iMoveType);
		pkEvent->SetParam(SK_BulletEvent::SEP_ACTION_TYPE, SK_BulletEvent::SBAT_SHOT | SK_BulletEvent::SBSS_FromServer);
		pkEvent->SetParam(SK_BulletEvent::SEP_DUMMY_TYPE, iDummyType);
		pkEvent->SetParam(SK_BulletEvent::SEP_MOVE_START_TIME, SK_SkillEvent::ParamFloat2LLong(fStartTime));
		pkEvent->SetParam(SK_BulletEvent::SEP_MOVE_LIFE_TIME, SK_SkillEvent::ParamFloat2LLong(fMoveTime));
		pkEvent->SetParam(SK_BulletEvent::SEP_PITCH_ANGLE, SK_SkillEvent::ParamFloat2LLong(fPitchAngle));
		pkEvent->SetParam(SK_BulletEvent::SEP_MOVE_SPEED, iMoveSpeed);
		pkEvent->SetParam(SK_BulletEvent::SEP_BULLET_FUNC, iBulletFuncID);
		pkEvent->SetParam(SK_BulletEvent::SEP_MAX_PENETRATE_COUNT, iPenetrateCount);
		pkEvent->SetParam(SK_BulletEvent::SEP_TARGET_LOCATION_X, rkTargetLocation.x);
		pkEvent->SetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Y, rkTargetLocation.y);
		pkEvent->SetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Z, rkTargetLocation.z);
		pkEvent->SetParam(SK_BulletEvent::SEP_SHOT_LOCATION_X, arg.kShotLocation.x);
		pkEvent->SetParam(SK_BulletEvent::SEP_SHOT_LOCATION_Y, arg.kShotLocation.y);
		pkEvent->SetParam(SK_BulletEvent::SEP_SHOT_LOCATION_Z, arg.kShotLocation.z);
		pkEvent->SetParam(SK_BulletEvent::SEP_COLLIDE_PARAM, Skill::SerializeToQuatWord(SK_BulletEvent::CollideParam(arg.fCollidePosParamT, arg.iCollideSurfaceMaterial)));
		pkEvent->SetParam(SK_BulletEvent::SEP_BULLET_LOCAL_ID, iClientLocalID);
		pkEvent->SetParam(SK_BulletEvent::SEP_LAST_UPDATE_TIME, SK_SkillEvent::ParamFloat2LLong(fStartTime));
		pkSkillEventManager->AddEvent(pkEvent);
	}
	//向客户端广播消息
	pkSkillEventManager->SendToClient(pkAttacker, pkEvent, SSET_BROADCAST_TO_AREA);
}

void SC_ServerSkillInterface::_implShotMultiBulletToTarget(GameLogic::LC_ActorBase* pkAttacker, object_id_type lTargetID, const mem::vector<Utility::UT_Vec3Int>& rkTargetLocation, const mem::vector<ShotBulletArg>& argVec)
{
	if(rkTargetLocation.size()!=argVec.size())
	{
		return;
	}

	MG_SyncBulletInfo sync;
	const ShotBulletArg& argInfo = argVec[0];

	sync.m_nSrcActorID = pkAttacker->GetID();
	sync.m_nDestActorID = lTargetID;
	sync.m_nSkillID = argInfo.lSkillTypeID;
	sync.m_nMoveType = argInfo.pkBullet->_lMoveType;
	sync.m_nActionType = SK_BulletEvent::SBAT_SHOT | SK_BulletEvent::SBSS_FromServer;
	sync.m_nDummyType = argInfo.iDummyType;
	sync.m_nStartTime = SK_SkillEvent::ParamFloat2LLong(argInfo.fStartTime);
	sync.m_nPitchAngle = SK_SkillEvent::ParamFloat2LLong(argInfo.fPitchAngle);
	sync.m_nMoveSpeed = argInfo.pkBullet->_lMoveSpeed;
	sync.m_nButtleFuncID = argInfo.iBulletFuncID;
	sync.m_nPenetrateCount = UT_MathBase::ClampInt(argInfo.iPenetrateCount, 0, SK_BulletEvent::MAX_BULLET_PENETRATE_COUNT);
	sync.m_nClientLocalID = argInfo.iClientLocalID;
	sync.m_nLastUpdateTime = SK_SkillEvent::ParamFloat2LLong(argInfo.fStartTime);

	sync.m_nAttackerLocation.pos_x = argInfo.kShotLocation.x;
	sync.m_nAttackerLocation.pos_y = argInfo.kShotLocation.y;
	sync.m_nAttackerLocation.pos_z = argInfo.kShotLocation.z;

	SK_ServerSkillEventManager* pkSkillEventManager = SERVER_GET_SKILL_EVENT_MANAGER();
	int eventIDStart = 0;
	for(int i=0; i<argVec.size(); ++i)
	{
		const ShotBulletArg& arg = argVec[i];
		// unpack
		int lSkillID = arg.lSkillTypeID;
		int iMoveType = arg.pkBullet->_lMoveType;
		int iDummyType = arg.iDummyType;
		int iPenetrateCount = UT_MathBase::ClampInt(arg.iPenetrateCount, 0, SK_BulletEvent::MAX_BULLET_PENETRATE_COUNT);
		float fStartTime = arg.fStartTime;
		float fMoveTime = arg.fMoveTime;
		float fPitchAngle = arg.fPitchAngle;
		int iMoveSpeed = arg.pkBullet->_lMoveSpeed;
		int iBulletFuncID = arg.iBulletFuncID;
		int iClientLocalID = arg.iClientLocalID;

		SK_SkillEvent* pkEvent = pkSkillEventManager->AllocEvent();
		if (NULL != pkEvent)
		{
			pkEvent->SetEventType(SETE_BULLET);
			pkEvent->SetSrcActorID(pkAttacker->GetID());
			pkEvent->SetDstActorID(lTargetID);
			pkEvent->SetLogicID(lSkillID);
			pkEvent->SetParam(SK_BulletEvent::SEP_MOVE_TYPE, iMoveType);
			pkEvent->SetParam(SK_BulletEvent::SEP_ACTION_TYPE, SK_BulletEvent::SBAT_SHOT | SK_BulletEvent::SBSS_FromServer);
			pkEvent->SetParam(SK_BulletEvent::SEP_DUMMY_TYPE, iDummyType);
			pkEvent->SetParam(SK_BulletEvent::SEP_MOVE_START_TIME, SK_SkillEvent::ParamFloat2LLong(fStartTime));
			pkEvent->SetParam(SK_BulletEvent::SEP_MOVE_LIFE_TIME, SK_SkillEvent::ParamFloat2LLong(fMoveTime));
			pkEvent->SetParam(SK_BulletEvent::SEP_PITCH_ANGLE, SK_SkillEvent::ParamFloat2LLong(fPitchAngle));
			pkEvent->SetParam(SK_BulletEvent::SEP_MOVE_SPEED, iMoveSpeed);
			pkEvent->SetParam(SK_BulletEvent::SEP_BULLET_FUNC, iBulletFuncID);
			pkEvent->SetParam(SK_BulletEvent::SEP_MAX_PENETRATE_COUNT, iPenetrateCount);
			pkEvent->SetParam(SK_BulletEvent::SEP_COLLIDE_PARAM, Skill::SerializeToQuatWord(SK_BulletEvent::CollideParam(arg.fCollidePosParamT, arg.iCollideSurfaceMaterial)));
			pkEvent->SetParam(SK_BulletEvent::SEP_BULLET_LOCAL_ID, iClientLocalID);
			pkEvent->SetParam(SK_BulletEvent::SEP_SHOT_LOCATION_X, argInfo.kShotLocation.x);
			pkEvent->SetParam(SK_BulletEvent::SEP_SHOT_LOCATION_Y, argInfo.kShotLocation.y);
			pkEvent->SetParam(SK_BulletEvent::SEP_SHOT_LOCATION_Z, argInfo.kShotLocation.z);
			pkEvent->SetParam(SK_BulletEvent::SEP_TARGET_LOCATION_X, rkTargetLocation[i].x);
			pkEvent->SetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Y, rkTargetLocation[i].y);
			pkEvent->SetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Z, rkTargetLocation[i].z);
			pkEvent->SetParam(SK_BulletEvent::SEP_LAST_UPDATE_TIME, SK_SkillEvent::ParamFloat2LLong(fStartTime));
			pkSkillEventManager->AddEvent(pkEvent);

			if(i == 0)
			{
				eventIDStart = pkEvent->GetEventID();
			}

			sync.m_nLifeTime.push_back(fMoveTime);
			sync.m_nCollideParam.push_back(Skill::SerializeToQuatWord(SK_BulletEvent::CollideParam(arg.fCollidePosParamT, arg.iCollideSurfaceMaterial)));

			PS_PosIndex targetPos;
			targetPos.pos_x = rkTargetLocation[i].x;
			targetPos.pos_y = rkTargetLocation[i].y;
			targetPos.pos_z = rkTargetLocation[i].z;
			sync.m_nTargetLocations.push_back(targetPos);
		}

		sync.m_nEventIDStart = eventIDStart;
		sync.m_nEventCount = argVec.size();
	}

	//向客户端广播消息
	pkSkillEventManager->SendMsgToClient(pkAttacker, MGPT_SYNCBULLETINFO, &sync, SSET_BROADCAST_TO_AREA);
}

void SC_ServerSkillInterface::_implShotMultiBulletToLocation(GameLogic::LC_ActorBase* pkAttacker, const mem::vector<Utility::UT_Vec3Int>& rkTargetLocation, const mem::vector<ShotBulletArg>& argVec)
{
	if(rkTargetLocation.size()!=argVec.size())
	{
		return;
	}

	float fStartTime = GET_PROCESS_TIME;

	MG_SyncBulletInfo sync;
	const ShotBulletArg& argInfo = argVec[0];

	sync.m_nSrcActorID = pkAttacker->GetID();
	sync.m_nDestActorID = 0;
	sync.m_nSkillID = argInfo.lSkillTypeID;
	sync.m_nMoveType = argInfo.pkBullet->_lMoveType;
	sync.m_nActionType = SK_BulletEvent::SBAT_SHOT | SK_BulletEvent::SBSS_FromServer;
	sync.m_nDummyType = argInfo.iDummyType;
	sync.m_nStartTime = fStartTime;
	sync.m_nPitchAngle = SK_SkillEvent::ParamFloat2LLong(argInfo.fPitchAngle);
	sync.m_nMoveSpeed = argInfo.pkBullet->_lMoveSpeed;
	sync.m_nButtleFuncID = argInfo.iBulletFuncID;
	sync.m_nPenetrateCount = UT_MathBase::ClampInt(argInfo.iPenetrateCount, 0, SK_BulletEvent::MAX_BULLET_PENETRATE_COUNT);
	sync.m_nClientLocalID = argInfo.iClientLocalID;
	sync.m_nLastUpdateTime = SK_SkillEvent::ParamFloat2LLong(argInfo.fStartTime);

	sync.m_nAttackerLocation.pos_x = argInfo.kShotLocation.x;
	sync.m_nAttackerLocation.pos_y = argInfo.kShotLocation.y;
	sync.m_nAttackerLocation.pos_z = argInfo.kShotLocation.z;

	SK_ServerSkillEventManager* pkSkillEventManager = SERVER_GET_SKILL_EVENT_MANAGER();
	int eventIDStart = 0;
	for(int i=0; i<argVec.size(); ++i)
	{
		const ShotBulletArg arg = argVec[i];
		// unpack
		int lSkillID = arg.lSkillTypeID;
		int iMoveType = arg.pkBullet->_lMoveType;
		int iDummyType = arg.iDummyType;
		int iPenetrateCount = UT_MathBase::ClampInt(arg.iPenetrateCount, 0, SK_BulletEvent::MAX_BULLET_PENETRATE_COUNT);
		float fMoveTime = arg.fMoveTime;
		float fPitchAngle = arg.fPitchAngle;
		int iMoveSpeed = arg.pkBullet->_lMoveSpeed;
		int iBulletFuncID = arg.iBulletFuncID;
		int iClientLocalID = arg.iClientLocalID;

		SK_SkillEvent* pkEvent = pkSkillEventManager->AllocEvent();
		if (NULL != pkEvent)
		{
			pkEvent->SetEventType(SETE_BULLET);
			pkEvent->SetSrcActorID(pkAttacker->GetID());
			pkEvent->SetDstActorID(0);
			pkEvent->SetLogicID(lSkillID);
			pkEvent->SetParam(SK_BulletEvent::SEP_MOVE_TYPE, iMoveType);
			pkEvent->SetParam(SK_BulletEvent::SEP_ACTION_TYPE, SK_BulletEvent::SBAT_SHOT | SK_BulletEvent::SBSS_FromServer);
			pkEvent->SetParam(SK_BulletEvent::SEP_DUMMY_TYPE, iDummyType);
			pkEvent->SetParam(SK_BulletEvent::SEP_MOVE_START_TIME, SK_SkillEvent::ParamFloat2LLong(fStartTime));
			pkEvent->SetParam(SK_BulletEvent::SEP_MOVE_LIFE_TIME, SK_SkillEvent::ParamFloat2LLong(fMoveTime));
			pkEvent->SetParam(SK_BulletEvent::SEP_PITCH_ANGLE, SK_SkillEvent::ParamFloat2LLong(fPitchAngle));
			pkEvent->SetParam(SK_BulletEvent::SEP_MOVE_SPEED, iMoveSpeed);
			pkEvent->SetParam(SK_BulletEvent::SEP_BULLET_FUNC, iBulletFuncID);
			pkEvent->SetParam(SK_BulletEvent::SEP_MAX_PENETRATE_COUNT, iPenetrateCount);
			pkEvent->SetParam(SK_BulletEvent::SEP_TARGET_LOCATION_X, rkTargetLocation[i].x);
			pkEvent->SetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Y, rkTargetLocation[i].y);
			pkEvent->SetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Z, rkTargetLocation[i].z);
			pkEvent->SetParam(SK_BulletEvent::SEP_SHOT_LOCATION_X, argInfo.kShotLocation.x);
			pkEvent->SetParam(SK_BulletEvent::SEP_SHOT_LOCATION_Y, argInfo.kShotLocation.y);
			pkEvent->SetParam(SK_BulletEvent::SEP_SHOT_LOCATION_Z, argInfo.kShotLocation.z);
			pkEvent->SetParam(SK_BulletEvent::SEP_COLLIDE_PARAM, Skill::SerializeToQuatWord(SK_BulletEvent::CollideParam(arg.fCollidePosParamT, arg.iCollideSurfaceMaterial)));
			pkEvent->SetParam(SK_BulletEvent::SEP_BULLET_LOCAL_ID, iClientLocalID);
			pkEvent->SetParam(SK_BulletEvent::SEP_LAST_UPDATE_TIME, SK_SkillEvent::ParamFloat2LLong(fStartTime));
			pkSkillEventManager->AddEvent(pkEvent);

			if(i == 0)
			{
				eventIDStart = pkEvent->GetEventID();
			}

			sync.m_nLifeTime.push_back(fMoveTime);
			sync.m_nCollideParam.push_back(Skill::SerializeToQuatWord(SK_BulletEvent::CollideParam(arg.fCollidePosParamT, arg.iCollideSurfaceMaterial)));

			PS_PosIndex targetPos;
			targetPos.pos_x = rkTargetLocation[i].x;
			targetPos.pos_y = rkTargetLocation[i].y;
			targetPos.pos_z = rkTargetLocation[i].z;
			sync.m_nTargetLocations.push_back(targetPos);
		}

		sync.m_nEventIDStart = eventIDStart;
		sync.m_nEventCount = argVec.size();
	}

	//向客户端广播消息
	pkSkillEventManager->SendMsgToClient(pkAttacker, MGPT_SYNCBULLETINFO, &sync, SSET_BROADCAST_TO_AREA);
}
//--------------------------------------------------------------------------------------------
bool SC_ServerSkillInterface::ReqEnterTranscription(GameLogic::LC_Attacker* pkScriptCaller, int iTranscriptionType)
{
	if (pkScriptCaller == NULL || !IS_PLAYER_ID(pkScriptCaller->GetID()))
	{
		return false;
	}
	LC_ServerPlayer* pkPlayer = static_cast<LC_ServerPlayer*>(pkScriptCaller);
	LC_ServerWorldManager* pkServerWorldManager = SERVER_GET_WORLD_MANAGER();
	pkServerWorldManager->RequestChangePlayerLocation(pkPlayer, LMIOT_TRANSFER, 0, 0, UT_Vec3Int());
	return false;
}
//--------------------------------------------------------------------------------------------------------------------------------------------
static bool AsyncReqQuitTranscription(Utility::timer_id_type timeid, object_id_type id, int32_t iMapLogicID)
{
	SC_ServerSkillInterface* pkSkillInterface =  SC_ServerScriptInterface::GetSingleton().GetSkillInterface();
	if (pkSkillInterface && IS_PLAYER_ID(id))
	{
		GameLogic::LC_Attacker*	pkScriptCaller =  pkSkillInterface->GetActor(id);
		if (pkScriptCaller && pkScriptCaller->GetMapLogicID() == iMapLogicID)
		{
			return pkSkillInterface->ReqQuitTranscription(pkScriptCaller, 0);
		}
	}
	return false;
}
//请求退出副本
bool SC_ServerSkillInterface::ReqQuitTranscription(GameLogic::LC_Attacker* pkScriptCaller, int32_t iDelay/*=0秒数*/)
{
	if (NULL == pkScriptCaller)
	{
		return false;
	}

	if (!IS_PLAYER_ID(pkScriptCaller->GetID()))
	{
		return false;
	}

	LC_ServerPlayer* pkPlayer = static_cast<LC_ServerPlayer*>(pkScriptCaller);

	if (iDelay <= 0)
	{
		//退出副本到普通地图(新手地图),无法确定具体的地图ID，地图类型和坐标
		LC_ServerWorldManager* pkServerWorldManager = SERVER_GET_WORLD_MANAGER();
		pkServerWorldManager->RequestChangePlayerLocation(pkPlayer, LMIOT_QUIT_TRANSCRIPTION, 0, 1, UT_Vec3Int());
	}
	else
	{
		Utility::UT_TimerManager* pkTimerMgr = Utility::UT_TimerManager::GetSingletonPtr();
		pkTimerMgr->DelayCall(iDelay * ONE_SECOND_MILI, &AsyncReqQuitTranscription, (bool(*)(Utility::timer_id_type, object_id_type, int32_t))NULL, pkScriptCaller->GetID(), pkScriptCaller->GetMapLogicID());
	}
	return true;
}
void SC_ServerSkillInterface::AddGuildPresent(GameLogic::LC_Attacker* pkScriptCaller, int32_t itemID, int32_t way, int32_t count)
{
	if (pkScriptCaller == NULL || pkScriptCaller->GetType() != LOT_PLAYER)
	{
		return ;
	}
	LC_ServerPlayer* pkPlayer = static_cast<LC_ServerPlayer*>(pkScriptCaller);
	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (pkPlayer == NULL)
	{
		return;
	}

	for(int i=0; i<count; ++i)
	{
		pkPlayer->AddGuildPresent(itemID, (int8_t)way);
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------
void SC_ServerSkillInterface::AddSystemTitle(GameLogic::LC_Attacker* pkScriptCaller, int lID)
{
	if (pkScriptCaller == NULL || pkScriptCaller->GetType() != LOT_PLAYER)
	{
		return;
	}
	LC_ServerPlayer* pkPlayer = static_cast<LC_ServerPlayer*>(pkScriptCaller);
	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (pkPlayer == NULL)
		return;
	LC_TitleAsset* pkTitleAsset = pkPlayer->GetTitleAsset();
	{
		pkPlayer->AddSystemTitle(lID);
		LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
		LC_GameEvent* pkGameEvent = pkEventManager->AllocGameEvent();
		if (pkGameEvent == NULL)
		{
			return;
		}
		pkGameEvent->SetEventType(GLET_ADD_SYSTEM_TITlE);
		pkGameEvent->SetEventParam(EVENT_SEND_TYPE_PARAM, GENT_SELF);
		pkGameEvent->SetEventParam(EP_ADD_SYSTEM_TITLE_PLAYER_ID, pkPlayer->GetID());
		pkGameEvent->SetEventParam(EP_ADD_SYSTEM_TITLE_ID, lID);
		pkEventManager->InsertGameEvent(pkGameEvent);
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------
void SC_ServerSkillInterface::RemoveSystemTitle(GameLogic::LC_Attacker* pkScriptCaller, int lID)
{
	if (pkScriptCaller == NULL || pkScriptCaller->GetType() != LOT_PLAYER)
	{
		return;
	}
	LC_ServerPlayer* pkPlayer = static_cast<LC_ServerPlayer*>(pkScriptCaller);
	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (pkPlayer == NULL)
		return;
	LC_TitleAsset* pkTitleAsset = pkPlayer->GetTitleAsset();
//	if (pkTitleAsset)
	{
		pkPlayer->RemoveSystemTitle(lID, GET_CURRENT_TIMESTAMP_IN_SECONDS());
		LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
		LC_GameEvent* pkGameEvent = pkEventManager->AllocGameEvent();
		if (pkGameEvent == NULL)
		{
			return;
		}
		pkGameEvent->SetEventType(GLET_REMOVE_SYSTEM_TITlE);
		pkGameEvent->SetEventParam(EVENT_SEND_TYPE_PARAM, GENT_SELF);
		pkGameEvent->SetEventParam(EP_REMOVE_SYSTEM_TITLE_PLAYER_ID, pkPlayer->GetID());
		pkGameEvent->SetEventParam(EP_REMOVE_SYSTEM_TITLE_ID, lID);
		pkEventManager->InsertGameEvent(pkGameEvent);
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------
void SC_ServerSkillInterface::SetSystemTitle(GameLogic::LC_Attacker* pkScriptCaller, int lID)
{
	if (pkScriptCaller == NULL || pkScriptCaller->GetType() != LOT_PLAYER)
	{
		return;
	}
	LC_ServerPlayer* pkPlayer = static_cast<LC_ServerPlayer*>(pkScriptCaller);
	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (pkPlayer == NULL)
		return;
	LC_TitleAsset* pkTitleAsset = pkPlayer->GetTitleAsset();
//	if (pkTitleAsset)
	{
		pkPlayer->SetSystemTitle(lID);
		LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
		LC_GameEvent* pkGameEvent = pkEventManager->AllocGameEvent();
		if (pkGameEvent == NULL)
		{
			return;
		}
		pkGameEvent->SetEventType(GLET_CHANGE_SYSTEM_TITLE);
		pkGameEvent->SetEventParam(EVENT_SEND_TYPE_PARAM, GENT_SELF);
		pkGameEvent->SetEventParam(EP_CHANGE_SYSTEM_TITlE_PLAYER_ID, pkPlayer->GetID());
		pkGameEvent->SetEventParam(EP_CHANGE_SYSTEM_TITlE_ID, lID);
		pkEventManager->InsertGameEvent(pkGameEvent);
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------
bool SC_ServerSkillInterface::DisarmTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lSkillTypeID, int iRandomIndex, int lUnDisarmStateID, float fDisarmDuration)
{
	return false;
}
//--------------------------------------------------------------------------------------------------------------------------------------------
void SC_ServerSkillInterface::UnDisarmTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex)
{
	return;
}
//--------------------------------------------------------------------------------------------------------------------------------------------
void SC_ServerSkillInterface::NoticeSkillExecuteEvent(object_id_type lPlayerID, int lSkillID, object_id_type lTargetID)
{
	LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
	LC_GameEvent* pkGameEvent = pkEventManager->AllocGameEvent();
	if (pkGameEvent == NULL)
	{
		return;
	}
	pkGameEvent->SetEventType(GLET_SKILL_EXECUTE);
	pkGameEvent->SetEventParam(EP_SKILL_SKILL_EXECUTE_PLAYER_ID, lPlayerID);
	pkGameEvent->SetEventParam(EP_SKILL_SKILL_EXECUTE_SKILL_ID, lSkillID);
	pkGameEvent->SetEventParam(EP_SKILL_SKILL_EXECUTE_TARGET_ID, lTargetID);
	pkEventManager->InsertGameEvent(pkGameEvent);
}

void SC_ServerSkillInterface::NoticeConvoyNPCEvent(object_id_type lPlayerID, object_id_type lNPCID, int lTaskID)
{
	LC_PlayerManagerBase* pkPlayerManager = ENGINE_GET_GAMEPLAYER_MANAGER();
	if (pkPlayerManager == NULL)
	{
		return;
	}
	LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
	LC_GameEvent* pkGameEvent			= pkEventManager->AllocGameEvent();
	if (pkGameEvent)
	{
		pkGameEvent->SetEventType(GLET_CONVOY_NPC);
		pkGameEvent->SetEventParam(EVENT_SEND_TYPE_PARAM, GENT_SELF);
		pkGameEvent->SetEventParam(EP_CONVOY_NPC_PLAYER_ID, lPlayerID);
		pkGameEvent->SetEventParam(EP_CONVOY_NPC_ID, lNPCID);
		pkGameEvent->SetEventParam(EP_CONVOY_NPC_TASK_ID, lTaskID);

		if (IS_PLAYER_ID(lPlayerID))
		{
			LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(lPlayerID);
			if (pkPlayer)
			{
				pkPlayer->SendGameEvent(pkGameEvent);
			}
		}
		pkEventManager->FreeGameEvent(pkGameEvent);
	}
}

void SC_ServerSkillInterface::NoticeTaskGameEvent(object_id_type lPlayerID, int lTaskID, int lTaskEventType, int lParam1, int lParam2)
{
	LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
	LC_GameEvent* pkGameEvent = pkEventManager->AllocGameEvent();
	if (pkGameEvent == NULL)
	{
		return;
	}
	pkGameEvent->SetEventType(GLET_TASK_GAME_EVENT);
	pkGameEvent->SetEventParam(EVENT_SEND_TYPE_PARAM, GENT_SELF);
	pkGameEvent->SetEventParam(EP_TASK_GAME_PLAYER_ID, lPlayerID);
	pkGameEvent->SetEventParam(EP_TASK_GAME_TASK_ID, lTaskID);
	pkGameEvent->SetEventParam(EP_TASK_GAME_EVENT_TYPE, lTaskEventType);
	pkGameEvent->SetEventParam(EP_TASK_GAME_PARAM_1, lParam1);
	pkGameEvent->SetEventParam(EP_TASK_GAME_PARAM_2, lParam2);
	pkEventManager->InsertGameEvent(pkGameEvent);
}

void SC_ServerSkillInterface::AddExp(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int64_t iExp)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}
	LC_ActorPtrList kTargetList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetList);
	LC_ActorPtrListIter iter;
	LC_ActorBase* pkTarget = NULL;
	TRAV(kTargetList, iter)
	{
		pkTarget = *iter;
		pkTarget->AddEXP(iExp, SYS_FUNC_TP_GM, 0);
	}
}

void SC_ServerSkillInterface::RecoverExp(GameLogic::LC_Attacker* pkScriptCaller, float multiply)
{
	GetSitRecoverExpIncrement(pkScriptCaller, multiply);
}

void SC_ServerSkillInterface::RecoverSkillPoint(GameLogic::LC_Attacker* pkScriptCaller, float multiply)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}
	LC_PlayerBase* player = (LC_PlayerBase*)pkScriptCaller;
	int lRecoverRate = (int)player->GetFinalAttributeMap()->GetAttribute(ATT_SP_RECOVER_RATE);
	if (lRecoverRate <= 0)
	{
		return;
	}
	SK_SP* inst = SK_SP::GetSingletonPtr();
	if (NULL == inst)
	{
		return;
	}
	SP_Entry entry;
	inst->GetSP_Entry(player->GetSitRecoverLevel(), entry);
	int sp_base = SP_RECOVER_BASE_VALUE;

	int RecoverPart1 = (int)((entry.AddRate + lRecoverRate) * 0.0001f) * sp_base;
	int VipValue = (int)(sp_base * player->GetVIPAsset().GetZazenSPPlus());
	//计算额外加成
	attr_value_type lRecoverValueAdd = attr_value_type(RecoverPart1 * multiply + 0.5f);
	int VipValueadd = int(VipValue * multiply + 0.5f);
	//int TotalValue = lRecoverValueAdd+VipValueadd;
	attr_value_type totalDelta = player->GetSkillLevelupPoints();
	player->AddSkillLevelupPointsWithVip(lRecoverValueAdd, VipValueadd);
	totalDelta = player->GetSkillLevelupPoints() - totalDelta;
	player->AddTotalSkillLevelupPoints(totalDelta);

	// 	SK_SkillEventManagerBase* pkEventManager = ENGINE_GET_SKILL_EVENT_MANAGER();
	// 	SK_SkillEvent* pkEvent = pkEventManager->AllocEvent();
	// 	if (NULL != pkEvent)
	// 	{
	// 		pkEvent->SetEventType(SETE_RECOVER);
	// 		pkEvent->SetSrcActorID(player->GetID());
	// 		pkEvent->SetDstActorID(player->GetID());
	// 		pkEvent->SetLogicID(SKILL_MEDITATION);
	// 		pkEvent->SetParam(SK_RecoverEvent::SEP_ITEM_ID, 0);
	// 		pkEvent->SetParam(SK_RecoverEvent::SEP_RECOVER_TYPE, SK_RecoverEvent::SRT_SP);
	// 		pkEvent->SetParam(SK_RecoverEvent::SEP_RECOVER_BASE, totalDelta);
	// 		pkEventManager->AddEvent(pkEvent);
	// 	}
}

int SC_ServerSkillInterface::GetSitRecoverExpIncrement(GameLogic::LC_Attacker* pkScriptCaller, float multiply)
{
	if (NULL == pkScriptCaller)
	{
		return 0;
	}
	if (!IS_PLAYER_CLASS_ID(pkScriptCaller->GetID()))
	{
		return 0;
	}
	LC_PlayerBase* pkPlayer = static_cast<LC_PlayerBase*>(pkScriptCaller);
	int lRecoverRate = (int)pkPlayer->GetFinalAttributeMap()->GetAttribute(ATT_SP_RECOVER_RATE);
	if (lRecoverRate <= 0)
	{
		return 0;
	}
	float fInc = (float)pkPlayer->GetVIPAsset().GetZazenExpPlus();
	int iTargetLevel = pkPlayer->GetLevel();
	float expBase = 5.0f * iTargetLevel;
	int expAll = int(expBase * (fInc + lRecoverRate * 0.0001f) * multiply + 0.5f);
	pkPlayer->AddSitExpWithVip(int(expBase * lRecoverRate * 0.0001f * multiply + 0.5f), int(expBase * fInc * multiply + 0.5f));
	return expAll;
}

void SC_ServerSkillInterface::AddTargetSkillPoint(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, attr_value_type iSkillPoint)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	LC_ActorPtrList kTargetList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetList);
	LC_ActorPtrListIter iter;
	TRAV(kTargetList, iter)
	{
		if (IS_PLAYER_CLASS_ID((*iter)->GetID()))
		{
			LC_PlayerBase* pkPlayer  = (LC_PlayerBase*)(*iter);
			pkPlayer->AddSkillLevelupPoints(iSkillPoint, SYS_FUNC_TP_NONE, 0);

			SK_SkillEventManagerBase* pkEventManager = ENGINE_GET_SKILL_EVENT_MANAGER();
			SK_SkillEvent* pkEvent = pkEventManager->AllocEvent();
			if (NULL != pkEvent)
			{
				pkEvent->SetEventType(SETE_RECOVER);
				pkEvent->SetSrcActorID(pkPlayer->GetID());
				pkEvent->SetDstActorID(pkPlayer->GetID());
				pkEvent->SetLogicID(SKILL_MEDITATION);
				pkEvent->SetParam(SK_RecoverEvent::SEP_ITEM_ID, 0);
				pkEvent->SetParam(SK_RecoverEvent::SEP_RECOVER_TYPE, SK_RecoverEvent::SRT_SP);
				pkEvent->SetParam(SK_RecoverEvent::SEP_RECOVER_BASE, iSkillPoint);
				pkEventManager->AddEvent(pkEvent);
			}
		}
	}
}

void SC_ServerSkillInterface::AddTargetSkillPointLevel(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, float fParam1)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	LC_ActorPtrList kTargetList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetList);
	LC_ActorPtrListIter iter;
	TRAV(kTargetList, iter)
	{
		if (IS_PLAYER_CLASS_ID((*iter)->GetID()))
		{
			LC_PlayerBase* pkPlayer  = (LC_PlayerBase*)(*iter);
			attr_value_type skillPoint = Double2Int64(fParam1 * LC_Helper::BaseLevelSP(pkPlayer->GetLevel()) + 0.5f);
			pkPlayer->AddSkillLevelupPoints(skillPoint, SYS_FUNC_TP_NONE, 0);

			SK_SkillEventManagerBase* pkEventManager = ENGINE_GET_SKILL_EVENT_MANAGER();
			SK_SkillEvent* pkEvent = pkEventManager->AllocEvent();
			if (NULL != pkEvent)
			{
				pkEvent->SetEventType(SETE_RECOVER);
				pkEvent->SetSrcActorID(pkPlayer->GetID());
				pkEvent->SetDstActorID(pkPlayer->GetID());
				pkEvent->SetLogicID(SKILL_MEDITATION);
				pkEvent->SetParam(SK_RecoverEvent::SEP_ITEM_ID, 0);
				pkEvent->SetParam(SK_RecoverEvent::SEP_RECOVER_TYPE, SK_RecoverEvent::SRT_SP);
				pkEvent->SetParam(SK_RecoverEvent::SEP_RECOVER_BASE, skillPoint);
				pkEventManager->AddEvent(pkEvent);
			}
		}
	}
}

void SC_ServerSkillInterface::AddTargetCashLevel(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, float fParam1)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	LC_ActorPtrList kTargetList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetList);
	LC_ActorPtrListIter iter;
	LC_PlayerBase* pkTarget = NULL;
	TRAV(kTargetList, iter)
	{
		pkTarget = (LC_PlayerBase*)*iter;
		MoneyType mt = MoneyType(fParam1 * LC_Helper::BaseLevelCash(pkTarget->GetLevel()) + 0.5f);
		pkTarget->AddCash(mt, CT_UNBIND_CASH, SYS_FUNC_TP_GM, -1);
	}
}

void SC_ServerSkillInterface::AddCash(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, MoneyType iCash, int iCashType, int iSrcType, int iSrcID)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	LC_ActorPtrList kTargetList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetList);
	LC_ActorPtrListIter iter;
	LC_PlayerBase* pkTarget = NULL;
	TRAV(kTargetList, iter)
	{
		pkTarget = (LC_PlayerBase*)*iter;
		pkTarget->AddCash(iCash, (CashType)iCashType, iSrcType, iSrcID);
	}
}

void SC_ServerSkillInterface::AddTargetExpRelatedLevel(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, float param1)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	LC_ActorPtrList kTargetList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetList);
	LC_ActorPtrListIter iter;
	LC_ActorBase* pkTarget = NULL;
	TRAV(kTargetList, iter)
	{
		pkTarget = *iter;
		pkTarget->AddEXP(Double2Int64(param1 * LC_Helper::BaseLevelExp(pkTarget->GetLevel()) + 0.5f), SYS_FUNC_TP_GM, 0);
	}
}

void SC_ServerSkillInterface::CheckSkillFlagAndInterrupt(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iSkillFlagIndex, object_id_type lLogicID)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	if (iSkillFlagIndex < 1 || iSkillFlagIndex > 63)
	{
		return;
	}
	int64_t llFlag = 1;
	llFlag = (llFlag << (iSkillFlagIndex - 1));

	SK_ServerSkillEventManager* pkEventManager = SERVER_GET_SKILL_EVENT_MANAGER();
	if (NULL == pkEventManager)
	{
		return;
	}

	LC_ActorPtrList kTargetList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetList);
	LC_ActorPtrListIter iter;
	LC_ActorBase* pkTarget = NULL;
	SK_SkillEvent* pkEvent = NULL;
	TRAV(kTargetList, iter)
	{
		pkTarget = *iter;
		if (pkTarget->GetSkillFlag(llFlag) && IS_SKILL_TYPE(pkTarget->GetCurrentStateType()))
		{
			pkEvent = pkEventManager->AllocEvent();
			if (NULL != pkEvent)
			{
				pkEvent->SetEventType(SETE_INTERRUPT);
				pkEvent->SetSrcActorID(pkScriptCaller->GetID());
				pkEvent->SetDstActorID(pkTarget->GetID());
				pkEvent->SetLogicID(lLogicID);
				pkEvent->SetParam(SK_InterruptEvent::SEP_BE_INTERRUPT_SKILL_ID, pkTarget->GetCurrentStateType());
				//注意这里使用伤害方式，目标被动的打断技能
				pkEvent->SetParam(SK_InterruptEvent::SEP_INTERRUPT_TYPE, SKI_EVENT);
				pkEventManager->AddEvent(pkEvent);
			}
		}
	}
}

int SC_ServerSkillInterface::GetCurrentSystemTime()
{
	return (int)GET_PROCESS_TIME;
}

void SC_ServerSkillInterface::GroupTranstfer(GameLogic::LC_Attacker* pkScriptCaller, int iResMapID, int lX, int lY)
{
	if (pkScriptCaller == NULL || !IS_PLAYER_ID(pkScriptCaller->GetID()))
	{
		return;
	}
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkScriptCaller;

	UT_Vec3Int kPos;
	kPos.x = lX;
	kPos.y = lY;

	LC_PlayerGroupBase*	pkGroup = pkPlayer->GetPlayerGroup();
	if (NULL == pkGroup)
	{
		return;
	}
	LC_ServerWorldManager* pkServerWorldManager = SERVER_GET_WORLD_MANAGER();
	for (int i = 0; i < MAX_PLAYER_GROUP_MEMBER_LIST_SIZE; ++i)
	{
		LC_PlayerGroupEntry* pkMember = pkGroup->GetGroupEntryInMemberList(i);
		if (pkMember == NULL)
		{
			continue;
		}
		if (pkMember->GetValid() == false)
		{
			continue;
		}
		LC_ServerPlayer* pkNetPlayer = (LC_ServerPlayer*)pkMember->GetPlayer();
		if (pkNetPlayer == NULL)
		{
			continue;
		}
		pkServerWorldManager->RequestChangePlayerLocation(pkNetPlayer, LMIOT_NPC_TRANSFER, 0, iResMapID, kPos);
	}
}

bool SC_ServerSkillInterface::_judgeNormalAttack(attr_value_type iDog, attr_value_type iDogRate, int64_t iAttackerVehicleLevel, int64_t iTargetVehicleLevel, attr_value_type iAttackRate, double fRandom)
{
	//计算命中率
	double fFinalAttackRate = _compureAttackRate(iDog, iDogRate, iAttackerVehicleLevel, iTargetVehicleLevel, iAttackRate);
	if (fRandom < fFinalAttackRate)
	{
		return true;
	}
	else
	{
		return false;
	}
}
//------------------------------------------------------------
static inline bool over_power(int iAttackerVehicleLevel, int iTargetVehicleLevel)
{
	bool is_over_power = false;
	if (iTargetVehicleLevel > 11)
	{
		is_over_power = iTargetVehicleLevel > iAttackerVehicleLevel;
	}
	return is_over_power;
}

double SC_ServerSkillInterface::_compureAttackRate(attr_value_type iDog, attr_value_type iDogRate, int64_t iAttackerVehicleLevel, int64_t iTargetVehicleLevel, attr_value_type iAttackRate)
{
#if 0
	//暂时屏蔽坐骑压制
	//由于该功能的背景过强不敢删除此代码
// 	bool is_over_power = over_power(iAttackerVehicleLevel, iTargetVehicleLevel);
 	double fAttackRateBase = 0.0;
// 	if (is_over_power)
// 	{
// 		fAttackRateBase = 0.5;
// 	}
// 	else
// 	{
		fAttackRateBase = UT_MathBase::MaxDouble((iDog - iAttackRate) * 0.0001 + 0.05, 0.05) +  iDogRate * 0.0001;
		if (fAttackRateBase < 0.05f)
		{
			fAttackRateBase = 0.05f;
		}
		if (fAttackRateBase > 0.5)
		{
			fAttackRateBase = 0.5;
		}
// 	}
 	return 1.0f - fAttackRateBase;
// 	return iAttackRate;
#endif
	// 尧辉的需求：
	//几率换算公式
	//B闪避几率 B闪避率=max(（B闪避-A命中）/B闪避,0.05) = X
	//A命中几率 A命中率=1-B闪避率 (x) = y
	//roll  y
	double fTargetDogRate = UT_MathBase::MaxDouble((iDog - iAttackRate) /UT_MathBase::MaxDouble(iDog, 1.0) , 0.05);// +  iDogRate * 0.0001;
	float fReuslt = UT_MathBase::MaxDouble(1.0 - fTargetDogRate, 0.5 );
	return fReuslt;
}

//------------------------------------------------------------
bool SC_ServerSkillInterface::_judgeCriticalAttack(attr_value_type iCritical, attr_value_type iCriticalRate, attr_value_type iCriticalDef, int64_t iAttackerLevel, int64_t iTargetLevel, double fCriticalRateAdd, double fRandom, LC_ActorBase* pSrc, LC_ActorBase* pDst)
{
	//计算暴击率
	double fFinalCriticalRate = _computeCriticalRate(iCritical, iCriticalRate, iCriticalDef, iAttackerLevel, iTargetLevel, fCriticalRateAdd, pSrc, pDst);
	if (fRandom < (1.0f - fFinalCriticalRate))
	{
		return false;
	}
	else
	{
		return true;
	}
}
//------------------------------------------------------------
double SC_ServerSkillInterface::_computeCriticalRate(attr_value_type iCritical, attr_value_type iCriticalRate, attr_value_type iCriticalDef, int64_t iAttackerVehicleLevel, int64_t iTargetVehicleLevel, double fCriticalRateAdd, LC_ActorBase* pSrc, LC_ActorBase* pDst)
{
#if 0
	//现在不能压制了，而且压制也写错了，需求传入的应该是坐骑等级，但是代码传入的是任务等级
// 	bool is_over_power = over_power(iAttackerVehicleLevel, iTargetVehicleLevel);
 	double fCriticalRateBase = 0.0;
// 	if (is_over_power)
// 	{
// 		fCriticalRateBase = 0.5;
// 	}
// 	else
// 	{
		fCriticalRateBase = UT_MathBase::MaxDouble((iCritical + fCriticalRateAdd - iCriticalDef) * 0.0001f + 0.05f, 0.05f) + iCriticalRate * 0.0001f;
		if (fCriticalRateBase < 0.05f)
		{
			fCriticalRateBase = 0.05f;
		}
		if (fCriticalRateBase > 0.5)
		{
			fCriticalRateBase = 0.5;
		}
// 	}
	return fCriticalRateBase;
#endif
	// 尧辉需求：
	//A暴击几率 A暴击率=max(（A暴击-B韧性）/A暴击,0.05)  = X
	//20210720 需求修正为: 
	//A暴击几率 A暴击率=max(（A暴击 + A对B额外暴击 -B韧性）/A暴击,0.05)  = X
	//roll X
	if ( pSrc && pDst )
	{
		object_id_type nSrcId = pSrc->GetOwnerObjectID();
		int nCombatEvent_Crit = pDst->GetCombatEventData(eCombatEvent_Passive_Crit, nSrcId );
		iCritical += nCombatEvent_Crit;
	}
	double dResult = UT_MathBase::MaxDouble((iCritical - iCriticalDef) / UT_MathBase::MaxDouble(iCritical, 1.0) , 0.05);// + iCriticalRate * 0.0001f;

	//	魏欣(俺也一样) 7-21 15:02:43
	//Revision: 37769
	//Author: weixin
	//Date: 2021年1月18日 22:34:52
	//Message:
	//	命中率暴击几率修改公式
	//		----
	//		你记不记得这个日子附近，提过暴击率保底的需求？
	//
	//		魏欣(俺也一样) 7-21 15:02:51
	//		fResult = UT_MathBase::MaxDouble( fResult, 0.6f );
	//	至少保证0.6这样的
	//
	//		尧辉 7-21 17:57:56
	//		没有啊
	//
	//		尧辉 7-21 17:59:12
	//		不可能提60%的保底呀
	//
	//		魏欣(俺也一样) 7-21 17:59:27
	//		我也觉得奇怪，那我去掉了
	//float fResult = UT_MathBase::MaxDouble( dResult, 0.6 );

	return dResult;
}
//------------------------------------------------------------
double SC_ServerSkillInterface::_computeDamageBase(double iDamage, attr_value_type iDef, int32_t iMPCost, double fDamageRate, double fOverAttackRate, double appendRate)
{
	// ATT_MAX_ATK ,
	//不论攻防差异如何，mp产生的伤害必定存在
	 // iDef = B防御*(1 -A无视防御%)
	//double DamageBase = UT_MathBase::MaxDouble(0, double(iDamage - iDef));
	//A攻击*A攻击/[A攻击+B防御]
	if( 0 == (iDamage + iDef) )
		return 0.0;
	double DamageBase = 0;
	DamageBase = iDamage*iDamage / (iDamage + iDef);
	double damage = UT_MathBase::MaxDouble(DamageBase, iDamage * appendRate);
	damage = damage * UT_MathBase::MaxDouble(0.0f, fDamageRate);
	damage = UT_MathBase::MaxDouble(0, damage);
	return damage;
}

double SC_ServerSkillInterface::_computeMinAck(object_id_type lTargetID, object_id_type lAttackerID)
{
	double append = 0.05;
	if ( IS_PLAYER_CLASS_ID(lTargetID) && IS_PLAYER_CLASS_ID(lAttackerID) )
	{
		append = 0.35;
	}
	return append;
}

int64_t SC_ServerSkillInterface::_computeScoreDamgeBase(uint64_t iAScore, uint64_t iBScore, attr_value_type bMaxHP, double skillaset)
{
	if(0 == iAScore)
	{
		return 0;
	}

	double dS = (double)iBScore / (double)iAScore;
	if ( dS > 20 )
	{
		return 0;
	}
	else
	{
		return int64_t(UT_MathBase::PowDouble(dS, 0.35)* 0.01f *bMaxHP*skillaset);
	}
}

void SC_ServerSkillInterface::AutoFitLevelReInit(GameLogic::LC_Attacker* pkScriptCaller, int iLevel)
{
	if (pkScriptCaller == NULL)
	{
		return;
	}
	if (!IS_NPC_ID(pkScriptCaller->GetID()))
	{
		return;
	}
	LC_ServerNPC* pkNPC = (LC_ServerNPC*)pkScriptCaller;
	pkNPC->AutoFitLevelReInit(iLevel);
}

void SC_ServerSkillInterface::AddActionForbid(GameLogic::LC_Attacker* pkScriptCaller, int iForbidType, int lParam1 /*= 0*/, int lParam2 /*= 0*/)
{
	if (pkScriptCaller == NULL || !IS_PLAYER_CLASS_ID(pkScriptCaller->GetID()))
	{
		return;
	}
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkScriptCaller;
	LC_ActionForbid* pkForbid = pkPlayer->GetActionForbid();
	pkForbid->AddActionForbid(iForbidType, lParam1, lParam2);
}

void SC_ServerSkillInterface::RemoveActionForbid(GameLogic::LC_Attacker* pkScriptCaller, int iForbidType, int lParam1 /*= 0*/, int lParam2 /*= 0*/)
{
	if (pkScriptCaller == NULL || !IS_PLAYER_CLASS_ID(pkScriptCaller->GetID()))
	{
		return;
	}
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkScriptCaller;
	LC_ActionForbid* pkForbid = pkPlayer->GetActionForbid();
//	if (pkForbid)
	{
		pkForbid->RemoveActionForbid(iForbidType, lParam1, lParam2);
	}
}

void SC_ServerSkillInterface::SetSurvivalTime(GameLogic::LC_Attacker* pkScriptCaller, int iSurvivalTime)
{
	if (pkScriptCaller == NULL)
	{
		return;
	}
	if (!IS_NPC_ID(pkScriptCaller->GetID()))
	{
		return;
	}
	LC_ServerNPC* pkNPC = (LC_ServerNPC*)pkScriptCaller;
	pkNPC->SetSurvivalTime(iSurvivalTime);
	pkNPC->SetSurvivalStartTime(GET_PROCESS_TIME);
}

void SC_ServerSkillInterface::SetTargetSurvivalTime(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iSurvivalTime)
{
	if (pkScriptCaller == NULL)
	{
		return;
	}
	LC_ActorPtrList kTargetList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetList);
	LC_ActorPtrListIter iter;
	LC_ActorBase* pkTarget = NULL;
	TRAV(kTargetList, iter)
	{
		pkTarget = *iter;
		if (!IS_NPC_ID(pkTarget->GetID()))
		{
			continue;
		}
		LC_ServerNPC* pkNPC = (LC_ServerNPC*)pkTarget;
		pkNPC->SetSurvivalTime(iSurvivalTime);
		pkNPC->SetSurvivalStartTime(GET_PROCESS_TIME);
	}
}

int SC_ServerSkillInterface::IsSuvivalDead(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex)
{
	if (NULL == pkScriptCaller)
	{
		return false;
	}

	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	if (kTargetsList.empty())
	{
		return false;
	}

	LC_ActorBase* pkTarget = kTargetsList.front();
	if (NULL == pkTarget)
	{
		return false;
	}

	if (IS_NPC_ID(pkTarget->GetID()))
	{
		LC_NPCBase* pkNPC = (LC_NPCBase*)pkTarget;
		return int(pkNPC->GetSurvivalTime() - GET_PROCESS_TIME + pkNPC->GetSurvivalStartTime());
	}
	return INT_MAX;
}

int SC_ServerSkillInterface::GetAIFuncValue(GameLogic::LC_Attacker* pkScriptCaller, int iAIFuncType)
{
	if (pkScriptCaller == NULL || !IS_NPC_ID(pkScriptCaller->GetID()))
	{
		return	-1;
	}
	LC_ActorBase* pkActor = (LC_ActorBase*)pkScriptCaller;
	return	pkActor->GetAIFuncValue(SF_WANDER_CAN_MOVE);
}

void SC_ServerSkillInterface::SetShapeShiftInfo(GameLogic::LC_Attacker* pkScriptCaller, int lCharTypeID)
{
	if (pkScriptCaller == NULL)
	{
		return;
	}
	if (IS_PLAYER_CLASS_ID(pkScriptCaller->GetID()))
	{
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkScriptCaller;
		pkPlayer->SetShapeShiftInfo(lCharTypeID);
	}
	else if (IS_NPC_ID(pkScriptCaller->GetID()))
	{
		LC_ServerNPC* pkNPC = (LC_ServerNPC*)pkScriptCaller;
		pkNPC->SetShapeShiftInfo(lCharTypeID);
	}
}

int SC_ServerSkillInterface::GetShapeShiftInfo(GameLogic::LC_Attacker* pkScriptCaller)
{
	if (pkScriptCaller == NULL)
	{
		return -1;
	}
	if (IS_PLAYER_CLASS_ID(pkScriptCaller->GetID()))
	{
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkScriptCaller;
		return pkPlayer->GetShapeShiftCharTypeID();
	}
	else if (IS_NPC_ID(pkScriptCaller->GetID()))
	{
		LC_ServerNPC* pkNPC = (LC_ServerNPC*)pkScriptCaller;
		return pkNPC->GetShapeShiftCharTypeID();
	}
	return -1;
}
void SC_ServerSkillInterface::AddTotalHookTime(GameLogic::LC_Attacker* pkScriptCaller, int addHookTime)
{
	return;
}
void SC_ServerSkillInterface::NotifyHookInfoUpdate(GameLogic::LC_Attacker* pkScriptCaller)
{
	return;
}
//-----------------------------------------------------------------------------------------
void SC_ServerSkillInterface::AddOwnerID(GameLogic::LC_Attacker* pkScriptCaller, object_id_type lOwnerID, bool bConsiderGroupMember)
{
	if (!pkScriptCaller || !IS_NPC_ID(pkScriptCaller->GetID()))
	{
		return;
	}
	LC_ServerNPC* pkServerNPC = static_cast<LC_ServerNPC*>(pkScriptCaller);
	LC_ServerNPCOwnerList& rkOwnerList = pkServerNPC->GetNPCOwnerList();
	rkOwnerList.AddOwnerID(lOwnerID, bConsiderGroupMember);
	rkOwnerList.SetAvoidUpdateFlag(true);
}
//-----------------------------------------------------------------------------------------
bool SC_ServerSkillInterface::SetNPCFixedOwnerListFlag(GameLogic::LC_Attacker* pkScriptCaller, bool bFlag)
{
	if (!pkScriptCaller || !IS_NPC_ID(pkScriptCaller->GetID()))
	{
		return false;
	}
	LC_ServerNPC* pkServerNPC = static_cast<LC_ServerNPC*>(pkScriptCaller);
	pkServerNPC->SetFixedOwnerListFlag(bFlag);
	return true;
}
//--------------------------------------------------------------------------------------------------------
void SC_ServerSkillInterface::NPCExecuteSkill(GameLogic::LC_ActorBase* pkActor, int lSkillID)
{
	if (!pkActor || !IS_NPC_ID(pkActor->GetID()))
	{
		return;
	}
	LC_ServerNPC* pkServerNPC = static_cast<LC_ServerNPC*>(pkActor);

	pkServerNPC->ExecuteSkill(lSkillID);
	//将NPC释放技能作为事件广播出去，目前副本中处理
	int lMapLogicID	=	pkServerNPC->GetMapLogicID();
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	{
		LC_ServerMapLogic_Place* pkMapLogic	=	pkWorldManager->GetMapLogic(lMapLogicID);
		if (pkMapLogic && pkMapLogic->GetMapLogicType() > MT_TRANSCRIPTION_BEGIN && pkMapLogic->GetMapLogicType() < MT_TRANSCRIPTION_END)
		{
			LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
			LC_GameEvent* pkGameEvent			= pkEventManager->AllocGameEvent();
			if (pkGameEvent)
			{
				pkGameEvent->SetEventType(GLET_NPC_EXECUTE_SKILL);
				pkGameEvent->SetEventParam(EVENT_BASE_PARAM_1, pkServerNPC->GetMapLogicID());
				pkGameEvent->SetEventParam(EVENT_BASE_PARAM_2, pkServerNPC->GetID());
				pkGameEvent->SetEventParam(EVENT_BASE_PARAM_3, lSkillID);
				pkEventManager->InsertGameEvent(pkGameEvent);
			}
		}
	}
	//广播完毕
}

void SC_ServerSkillInterface::NPCExecuteSkillEx(GameLogic::LC_ActorBase* pkActor, int lSkillID, object_id_type lTargetLocked)
{
	if (!pkActor || !IS_NPC_ID(pkActor->GetID()))
	{
		return;
	}
	LC_ServerNPC* pkServerNPC = static_cast<LC_ServerNPC*>(pkActor);

	pkServerNPC->SetLockedTargetID(lTargetLocked);
	pkServerNPC->ExecuteSkill(lSkillID);
	//将NPC释放技能作为事件广播出去，目前副本中处理
	int lMapLogicID	=	pkServerNPC->GetMapLogicID();
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	{
		LC_ServerMapLogic_Place* pkMapLogic	=	pkWorldManager->GetMapLogic(lMapLogicID);
		if (pkMapLogic && pkMapLogic->GetMapLogicType() > MT_TRANSCRIPTION_BEGIN && pkMapLogic->GetMapLogicType() < MT_TRANSCRIPTION_END)
		{
			LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
			LC_GameEvent* pkGameEvent			= pkEventManager->AllocGameEvent();
			if (pkGameEvent)
			{
				pkGameEvent->SetEventType(GLET_NPC_EXECUTE_SKILL);
				pkGameEvent->SetEventParam(EVENT_BASE_PARAM_1, pkServerNPC->GetMapLogicID());
				pkGameEvent->SetEventParam(EVENT_BASE_PARAM_2, pkServerNPC->GetID());
				pkGameEvent->SetEventParam(EVENT_BASE_PARAM_3, lSkillID);
				pkEventManager->InsertGameEvent(pkGameEvent);
			}
		}
	}
	//广播完毕
}

//-----------------------------------------------------------------------------------------
void SC_ServerSkillInterface::ServerPlayerExecuteSkill(GameLogic::LC_ActorBase* pkActor, int lSkillID)
{
	if (NULL == pkActor)
	{
		return;
	}

	if (IS_PLAYER_CLASS_ID(pkActor->GetID()))
	{
		SK_Factory* pkSkillFactory = SK_Factory::GetSingletonPtr();
		SK_SkillExecute* pkSkill = pkSkillFactory->RequestSkillExcute(lSkillID);
		if (NULL == pkSkill)
		{
			return;
		}
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkActor;
		pkPlayer->ClearPendingSkill();
		pkPlayer->ExecuteSkill(GET_PROCESS_TIME, pkSkill , 0);
	}
}
//-----------------------------------------------------------------------------------------
void SC_ServerSkillInterface::RltPlayerExecutePairSit(GameLogic::LC_ActorBase* pkActor)
{
	if (NULL == pkActor)
	{
		return;
	}

	if (IS_PLAYER_ID(pkActor->GetID()))
	{
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkActor;

		MG_Common sucMsg;
		sucMsg.set("result", RE_SUCCESS);
		sucMsg.set("other_id", pkPlayer->GetPairSitRecoverID());
		pkPlayer->SendMsgToClient(MGPT_RLT_PAIR_SITRECOVER, &sucMsg);
	}
}

//-----------------------------------------------------------------------------------------
void SC_ServerSkillInterface::FreezeAnimation(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	LC_ActorPtrList kTargetList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetList);
	if (kTargetList.size() == 0)
	{
		return;
	}
	LC_ActorPtrListIter iter;
	TRAV(kTargetList, iter)
	{
		LC_ActorBase* pkTarget = *iter;
		if (NULL != pkTarget)
		{
			//冻结动作
			pkTarget->SetSkillFlag(SK_FREEZE_ANIMATION, true);

			//屏蔽物品使用
			if (IS_PLAYER_CLASS_ID(pkTarget->GetID()))
			{
				LC_PlayerBase* pkPlayerTarget = (LC_PlayerBase*)pkTarget;
				if (!pkPlayerTarget->GetAnimationFreezed())
				{
					LC_ActionForbid* pkActionForbid = pkPlayerTarget->GetActionForbid();
					pkActionForbid->AddActionForbid(LAFT_ITEM, LAF_ITEM_MAX_COUNT);
					pkPlayerTarget->SetAnimationFreezed(true);
				}
			}
		}
	}
}
//-----------------------------------------------------------------------------------------
void SC_ServerSkillInterface::UnFreezeAnimation(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	LC_ActorPtrList kTargetList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetList);
	if (kTargetList.size() == 0)
	{
		return;
	}
	LC_ActorPtrListIter iter;
	TRAV(kTargetList, iter)
	{
		LC_ActorBase* pkTarget = *iter;
		if (NULL != pkTarget)
		{
			//解冻动作
			pkTarget->SetSkillFlag(SK_FREEZE_ANIMATION, false);

			//解除物品使用屏蔽
			if (IS_PLAYER_CLASS_ID(pkTarget->GetID()))
			{
				LC_PlayerBase* pkPlayerTarget = (LC_PlayerBase*)pkTarget;
				if (pkPlayerTarget->GetAnimationFreezed())
				{
					LC_ActionForbid* pkActionForbid = pkPlayerTarget->GetActionForbid();
					pkActionForbid->RemoveActionForbid(LAFT_ITEM, LAF_ITEM_MAX_COUNT);
					pkPlayerTarget->SetAnimationFreezed(false);
				}
			}
		}
	}
}

void SC_ServerSkillInterface::ShowRichUI(GameLogic::LC_Attacker* pkScriptCaller, int iTargetType, int iRichUIID, int bShow)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	if (iTargetType == SRUIT_SERVER) //临时
	{
		LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
		LC_GameEvent* pkGameEvent			= pkEventManager->AllocGameEvent();
		if (pkGameEvent)
		{
			pkGameEvent->SetEventType(GLET_SHOW_RICH_UI);
			pkGameEvent->SetEventParam(EVENT_SEND_TYPE_PARAM, GENT_SELF);
			pkGameEvent->SetEventParam(EP_SHOW_RICH_UI_ID, iRichUIID);
			pkGameEvent->SetEventParam(EP_SHOW_RICH_UI_SHOW, bShow);

			if (/*pkScriptCaller &&*/ IS_PLAYER_ID(pkScriptCaller->GetID()))
			{
				LC_PlayerBase* pkPlayer = (LC_PlayerBase*)pkScriptCaller;
				pkPlayer->SendGameEvent(pkGameEvent);
			}
			pkEventManager->FreeGameEvent(pkGameEvent);
		}
	}
}
//-----------------------------------------------------------------------------------------
void SC_ServerSkillInterface::_findTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, GameLogic::LC_ActorPtrList& kResult)
{
	//LC_ActorPtrList& kResult;
	kResult.clear();
	if (NULL == pkScriptCaller)
	{
		return;
	}

	if (iTargetIndex == TARGET_OWNER)
	{
		LC_MapBase* pkMap = pkScriptCaller->GetMap();
		if (NULL == pkMap)
		{
			return;
		}

		if (IS_NPC_ID(pkScriptCaller->GetID()))
		{
			LC_ServerNPCOwnerList& rkOwnerList = ((LC_ServerNPC*)pkScriptCaller)->GetNPCOwnerList();
			int iOwerCount = rkOwnerList.GetOwnerCount();
			for (int i = 0; i < iOwerCount; ++i)
			{
				object_id_type lOwnerID = rkOwnerList.GetOwnerByIndex(i);
				if (IS_PLAYER_CLASS_ID(lOwnerID) || IS_NPC_ID(lOwnerID))
				{
					LC_ActorBase* pkOwner = (LC_ActorBase*)pkMap->FindObject(lOwnerID);
					if (NULL != pkOwner)
					{
						kResult.push_back(pkOwner);
					}
				}
			}
		}
		return;
	}
	else
	{
		return SC_SkillInterface::_findTarget(pkScriptCaller, iTargetIndex, kResult);
	}
}

//-----------------------------------------------------------------------------------------
void SC_ServerSkillInterface::SetCourage(GameLogic::LC_Attacker* pkScriptCaller , bool Value)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkScriptCaller;
	pkPlayer->SetCourage(Value);
}

//获取NPC所在组成员数量
int SC_ServerSkillInterface::GetGroupNpcSize(GameLogic::LC_Attacker* pkScriptCaller)
{
	if (NULL == pkScriptCaller)
	{
		return 0;
	}
	if (!IS_NPC_ID(pkScriptCaller->GetID()))
	{
		return 0;
	}
	int logicid = pkScriptCaller->GetMapLogicID();
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerMapLogic_Place* logic = pkWorldManager->GetMapLogic(logicid);
	if (logic == NULL)
	{
		return 0;
	}
	return logic->GetEliteCount((LC_ServerNPC*)pkScriptCaller);
}

//根据索引获取组内成员
object_id_type SC_ServerSkillInterface::GetGroupNpcIDByIndex(GameLogic::LC_Attacker* pkScriptCaller, int index)
{
	if (NULL == pkScriptCaller)
	{
		return 0;
	}
	if (!IS_NPC_ID(pkScriptCaller->GetID()))
	{
		return 0;
	}
	if (index < 0)
	{
		return 0;
	}
	int logicid = pkScriptCaller->GetMapLogicID();
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerMapLogic_Place* logic = pkWorldManager->GetMapLogic(logicid);
	if (logic == NULL)
	{
		return 0;
	}
	return (logic->GetInEliteNpc((LC_ServerNPC*)pkScriptCaller, index))->GetID();
}

bool SC_ServerSkillInterface::SetNextPosInCircle(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex,/*int posx,int posy,*/int innerRadius, int outRadius, float step, bool clockwise)
{
	if (NULL == pkScriptCaller)
	{
		return false;
	}
	Utility::UT_Vec3Int nextpos;

	//通过TargetIndex的枚举得到目标点的坐标

	LC_ActorBase* pkActor = (LC_ActorBase*)pkScriptCaller;
	UT_Vec3Int kTargetLocation;

	if (iTargetIndex == TARGET_LOCATION)
	{
		kTargetLocation = pkActor->GetLockedLocation();
	}
	else
	{
		LC_ActorPtrList kTargetsList;
		_findTarget((LC_Attacker*)pkActor, iTargetIndex, kTargetsList);
		if (kTargetsList.empty())
		{
			return false;
		}
		LC_ActorBase* pkTarget = kTargetsList.front();
		if (!pkTarget)
		{
			return false;
		}
		kTargetLocation = pkTarget->GetCurrentLocation();
	}

	bool ret = UT_PathFindHelp::GetNextPosInCircle((LC_ActorBase*)pkActor, kTargetLocation.x, kTargetLocation.y, innerRadius, outRadius, step, clockwise, nextpos);
	if (!ret)
	{
		return false;
	}
	((LC_ActorBase*)pkActor)->SetTargetLocation(nextpos);
	return true;
}

void SC_ServerSkillInterface::TransferToTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, bool bUseSafeRadius)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	UT_Vec3Int kTargetLocation;
	if (TARGET_LOCATION == iTargetIndex)
	{
		kTargetLocation = pkScriptCaller->GetLockedLocation();
	}
	else
	{
		LC_ActorPtrList kTargetList;
		_findTarget(pkScriptCaller, iTargetIndex, kTargetList);
		if (kTargetList.size() != 1)
		{
			return;
		}

		LC_LogicObject* pkTarget = kTargetList.front();
		if (NULL == pkTarget)
		{
			return;
		}
		kTargetLocation = pkTarget->GetCurrentLocation();
		if (bUseSafeRadius)
		{
			int iSafeRadius	 = ((LC_ActorBase*)pkScriptCaller)->GetProperty_ShapeRadius();
			iSafeRadius	+= ((LC_ActorBase*)pkTarget)->GetProperty_ShapeRadius();
			kTargetLocation = UT_MathBase::ComputeSafeRadiusTargetLocation(pkScriptCaller->GetCurrentLocation(), kTargetLocation, iSafeRadius);
		}
	}
/*
	PF_ServerPathManager* pathmanager=PF_ServerPathManager::GetSingletonPtr();
	int nMapResId = pkScriptCaller->GetMapResID();
	bool bPosValid = pathmanager->IsPosValid(nMapResId,kTargetLocation);
	if (!bPosValid)
	{
		Utility::UT_LinePath rMovePath;
		bool bFindResult = pathmanager->FindPath(nMapResId , rMovePath, pkScriptCaller->GetCurrentLocation() , kTargetLocation);
		int iPathPointCount = rMovePath.GetPathPointCount();
		kTargetLocation = rMovePath.GetPathPoint( iPathPointCount - 1 );
	}
*/

	PF_ServerPathManager* pfm = SafeGetSingleton<PF_ServerPathManager>();
	if (pfm)
	{

		int MapResID = pkScriptCaller->GetMapResID();
		UT_Vec3Int fvec;
		bool ret = pfm->FindDirectFarawayPoint(MapResID, pkScriptCaller->GetCurrentLocation(), kTargetLocation, UT_MathBase::LineLengthXYInt(kTargetLocation, pkScriptCaller->GetCurrentLocation()), fvec);
		if (ret)
		{
			kTargetLocation = fvec;
		}
	}

	//UT_Vec3Int cur = pkScriptCaller->GetCurrentLocation();
#if 1
	pkScriptCaller->SetCurrentLocation(kTargetLocation);
	pkScriptCaller->SetVisibleLocation(kTargetLocation);
	((LC_ActorBase*)pkScriptCaller)->SetTargetLocation(kTargetLocation);
#endif
}

void SC_ServerSkillInterface::TransferBackToTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, bool bUseSafeRadius)
{
	//客户端的实现不一样
	TransferToTarget(pkScriptCaller, iTargetIndex, bUseSafeRadius);
}

void SC_ServerSkillInterface::TransferTargetToTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex0, int iTargetIndex1, bool bUseSafeRadius)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}
	UT_Vec3Int kTargetLocation;
	if (TARGET_LOCATION == iTargetIndex1)
	{
		kTargetLocation = pkScriptCaller->GetLockedLocation();
	}
	else
	{
		LC_ActorPtrList kTargetList;
		_findTarget(pkScriptCaller, iTargetIndex1, kTargetList);
		if (kTargetList.size() != 1)
		{
			return;
		}

		LC_LogicObject* pkTarget = kTargetList.front();
		if (NULL == pkTarget)
		{
			return;
		}
		kTargetLocation = pkTarget->GetCurrentLocation();
		if (bUseSafeRadius)
		{
			int iSafeRadius	 = ((LC_ActorBase*)pkScriptCaller)->GetProperty_ShapeRadius();
			iSafeRadius	+= ((LC_ActorBase*)pkTarget)->GetProperty_ShapeRadius();
			kTargetLocation = UT_MathBase::ComputeSafeRadiusTargetLocation(pkScriptCaller->GetCurrentLocation(), kTargetLocation, iSafeRadius);
		}
	}

	//找到目标中的第一个
	LC_TargetMap* pkTargetMap = pkScriptCaller->GetTargetMap();
	LC_MapBase* pkMapBase = pkScriptCaller->GetMap();
	object_id_vector kTargetIDVector = pkTargetMap->GetCurrentTargets();
	int iTargetCount = (int)kTargetIDVector.size();

	if (iTargetIndex0 == TARGET_0 && iTargetCount > 0)
	{
		int lTargetID = kTargetIDVector[0];
		LC_ActorBase* obj = (LC_ActorBase*)(pkMapBase->FindObject(lTargetID));
		if (obj)
		{
			if (IS_PLAYER_CLASS_ID(lTargetID))
			{
				LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(obj);
				MG_Sync_GamePlayerMove rltMsg;
				rltMsg.m_iPlayerID = pkPlayer->GetID();
				rltMsg.m_lMapID = pkMapBase->GetMapLogicID();
				//				rltMsg.m_lPlayerID = pkPlayer->GetID();
				rltMsg.m_lCurrentLocationX = kTargetLocation.x;
				rltMsg.m_lCurrentLocationY = kTargetLocation.y;
				rltMsg.m_lTargetLocationX  = kTargetLocation.x;
				rltMsg.m_lTargetLocationY  = kTargetLocation.y;
				pkPlayer->SendMsgToClient(MGPT_SYNC_GAME_PLAYER_MOVE, &rltMsg);
			}
		}
	}
	else
	{
		LC_ActorPtrList kTargetList;
		_findTarget(pkScriptCaller, iTargetIndex0, kTargetList);
		for (LC_ActorPtrList::iterator itr = kTargetList.begin(); itr != kTargetList.end(); ++itr)
		{
			int id = (*itr)->GetID();
			if (IS_PLAYER_CLASS_ID(id))
			{
				LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(*itr);
				MG_Sync_GamePlayerMove rltMsg;
				rltMsg.m_iPlayerID = pkPlayer->GetID();
				rltMsg.m_lMapID = pkMapBase->GetMapLogicID();
				//				rltMsg.m_lPlayerID = pkPlayer->GetID();
				rltMsg.m_lCurrentLocationX = kTargetLocation.x;
				rltMsg.m_lCurrentLocationY = kTargetLocation.y;
				rltMsg.m_lTargetLocationX  = kTargetLocation.x;
				rltMsg.m_lTargetLocationY  = kTargetLocation.y;
				pkPlayer->SendMsgToClient(MGPT_SYNC_GAME_PLAYER_MOVE, &rltMsg);
			}
		}
	}
}

void SC_ServerSkillInterface::TransferToXYZ(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int x, int y, int z)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	UT_Vec3Int kTargetLocation(x, y, z);
	LC_ActorPtrList kTargetList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetList);
	for (LC_ActorPtrList::iterator itr = kTargetList.begin(); itr != kTargetList.end(); ++itr)
	{
		int id = (*itr)->GetID();
		if (IS_PLAYER_CLASS_ID(id))
		{
			LC_MapBase* pkMapBase = pkScriptCaller->GetMap();
			LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(*itr);
			MG_Sync_GamePlayerMove rltMsg;
			rltMsg.m_iPlayerID = pkPlayer->GetID();
			rltMsg.m_lMapID = pkMapBase->GetMapLogicID();
			//				rltMsg.m_lPlayerID = pkPlayer->GetID();
			rltMsg.m_lCurrentLocationX = kTargetLocation.x;
			rltMsg.m_lCurrentLocationY = kTargetLocation.y;
			rltMsg.m_lTargetLocationX  = kTargetLocation.x;
			rltMsg.m_lTargetLocationY  = kTargetLocation.y;
			pkPlayer->SendMsgToClient(MGPT_SYNC_GAME_PLAYER_MOVE, &rltMsg);


			MG_SyncPlayerLocation syncMsg;
			syncMsg.m_usEntryCount = 0;
			PlayerLocationEntry eeyy;
			eeyy.m_lPlayerID = pkPlayer->GetID();
			eeyy.m_lCurrentLocationX =  kTargetLocation.x;
			eeyy.m_lCurrentLocationY = kTargetLocation.y;
			eeyy.m_lTargetLocationX =  kTargetLocation.x;
			eeyy.m_lTargetLocationY = kTargetLocation.y;
			//syncMsg.m_lMoveSpeed	   = (uint16_t)GetAttributeMap()->GetAttribute(ATT_MOVE_SPEED);
			syncMsg.m_rEntries[syncMsg.m_usEntryCount] = eeyy;
			syncMsg.m_usEntryCount++;

			pkPlayer->BroadCastToArea(MGPT_SYNC_PLAYERS_LOCATION_IN_FOV, &syncMsg);
		}
	}
}
#if 0
void SC_ServerSkillInterface::SetPostTransferLocation(GameLogic::LC_Attacker* pkScriptCaller)
{
	LC_ActorBase* player = (LC_ActorBase*)pkScriptCaller;
	if (player == NULL)
		return;

	UT_Vec3Int targetLocation = player->GetPostTransferLocation();

	PF_ServerPathManager* pathmanager=PF_ServerPathManager::GetSingletonPtr();
	int nMapResId = pkScriptCaller->GetMapResID();
	bool bPosValid = pathmanager->IsPosValid(nMapResId,targetLocation);
	if (!bPosValid)
	{
		Utility::UT_LinePath rMovePath;
		bool bFindResult = pathmanager->FindPath(nMapResId , rMovePath, pkScriptCaller->GetCurrentLocation() , targetLocation);
		int iPathPointCount = rMovePath.GetPathPointCount();
		targetLocation = rMovePath.GetPathPoint( iPathPointCount - 1 );
	}

	player->SetLockedLocation(targetLocation);
}
#endif
//***************
void SC_ServerSkillInterface::PunchBackTarget(GameLogic::LC_ActorBase* pkScriptCaller, int iTargetIndex, int iCenterTarget, int iPunchDistance, float fPunchTime)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}
	//传送目标
	LC_ActorPtrList kTransferList;
	_findTarget(pkScriptCaller, iCenterTarget, kTransferList);
	if (kTransferList.size() != 1)
	{
		return ;
	}
	//服务器实现
	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	LC_ActorPtrListIter IterBe = kTargetsList.begin();
	LC_ActorPtrListIter IterEd = kTargetsList.end();
	LC_ActorBase* pkTarget = NULL;
	//	object_id_type lCallerID = pkScriptCaller->GetID();
	object_id_type lTargetID = 0;
	for (; IterBe != IterEd; ++IterBe)
	{
		pkTarget = (*IterBe);
		lTargetID = pkTarget->GetID();

		if (pkTarget->GetSkillFlag(SK_IMM_PUNCH_BACK))	//目标身上是否有免疫击退标志
		{
			pkTarget->OnImmuneMove(SETE_IMM_PUNCH_MOVE,pkScriptCaller->GetID());
			continue;
		}
		if (IS_NPC_ID(lTargetID) && pkTarget->GetCurrentStateType() == ST_NPC_PUNCH_BACK)	//已经在击退状态
		{
			continue;
		}
		if (IS_PLAYER_CLASS_ID(lTargetID) && pkTarget->GetCurrentStateType() == ST_PLAYER_PUNCH_BACK)	//已经在击退状态
		{
			continue;
		}

		LC_ActorBase* center = *(kTransferList.begin());
		if (center)
		{
			UT_Vec3Int dis =  pkTarget->GetCurrentLocation() - center->GetCurrentLocation();
			dis.z = 0;
			UT_Vec3Float dir = dis.Normalize();
			dir.x = dir.x * iPunchDistance;
			dir.y = dir.y * iPunchDistance;
			UT_Vec3Int punchPos(pkTarget->GetCurrentLocation());
			punchPos.x += (int)dir.x;
			punchPos.y += (int)dir.y;
			
			PF_ServerPathManager* pfm = SafeGetSingleton<PF_ServerPathManager>();
			int MapResID = pkTarget->GetMapResID();
			UT_Vec3Int fvec;
			bool ret = pfm->FindDirectFarawayPoint(MapResID, pkTarget->GetCurrentLocation(), punchPos, UT_MathBase::LineLengthXYInt(punchPos, pkTarget->GetCurrentLocation()), fvec);
			if (ret)
			{
				pkTarget->PunchMove(SMAPT_PUNCHBACK, fvec, fPunchTime);
				AttachSkillState(pkScriptCaller, pkTarget->GetID(), 39122101, 1, 0,0,0,0,0);
			}
		}
	}
}

void SC_ServerSkillInterface::PunchBackTargetToPoint(GameLogic::LC_ActorBase* pkScriptCaller, int iTargetIndex, int iCenterTarget, int iPunchDistance, float fPunchTime)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}
	//传送目标
	LC_ActorPtrList kTransferList;
	_findTarget(pkScriptCaller, iCenterTarget, kTransferList);
	if (kTransferList.size() != 1)
	{
		return ;
	}
	//服务器实现
	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	LC_ActorPtrListIter IterBe = kTargetsList.begin();
	LC_ActorPtrListIter IterEd = kTargetsList.end();
	LC_ActorBase* pkTarget = NULL;
	//	object_id_type lCallerID = pkScriptCaller->GetID();
	object_id_type lTargetID = 0;
	float punchSpeed = iPunchDistance / fPunchTime;
	for (; IterBe != IterEd; ++IterBe)
	{
		pkTarget = (*IterBe);
		lTargetID = pkTarget->GetID();

		if (pkTarget->GetSkillFlag(SK_IMM_PUNCH_BACK))	//目标身上是否有免疫击退标志
		{
			pkTarget->OnImmuneMove(SETE_IMM_PUNCH_MOVE,pkScriptCaller->GetID());
			continue;
		}
		if (IS_NPC_ID(lTargetID) && pkTarget->GetCurrentStateType() == ST_NPC_PUNCH_BACK)	//已经在击退状态
		{
			continue;
		}
		if (IS_PLAYER_CLASS_ID(lTargetID) && pkTarget->GetCurrentStateType() == ST_PLAYER_PUNCH_BACK)	//已经在击退状态
		{
			continue;
		}
		
		LC_ActorBase* center = *(kTransferList.begin());
		if (center)
		{
			int punchDistance = UT_MathBase::LineLengthXYInt(center->GetTargetLocation(), pkTarget->GetCurrentLocation());
			float punchTime = punchDistance / punchSpeed;
			UT_Vec3Int dis =  center->GetTargetLocation() - center->GetCurrentLocation();
			UT_Vec3Float dir = dis.Normalize();
			dir.x = dir.x * punchDistance;
			dir.y = dir.y * punchDistance;
			UT_Vec3Int punchPos(pkTarget->GetCurrentLocation());
			punchPos.x += (int)dir.x;
			punchPos.y += (int)dir.y;
			
			PF_ServerPathManager* pfm = SafeGetSingleton<PF_ServerPathManager>();
			int MapResID = pkTarget->GetMapResID();
			UT_Vec3Int fvec;
			bool ret = pfm->FindDirectFarawayPoint(MapResID, pkTarget->GetCurrentLocation(), punchPos, punchDistance, fvec);
			if (ret)
			{
				pkTarget->PunchMove(SMAPT_PUNCHBACK, fvec, punchTime);
				AttachSkillState(pkScriptCaller, pkTarget->GetID(), 39122101, 1, 0,0,0,0,0);
			}
		}
	}
}

void SC_ServerSkillInterface::PunchBackTargetBySide(GameLogic::LC_ActorBase* pkScriptCaller, int iTargetIndex, int iCenterTarget, int iPunchDistance, float fPunchTime)
{
	if (NULL == pkScriptCaller)
		return;

	LC_ActorPtrList kTransferList;
	_findTarget(pkScriptCaller, iCenterTarget, kTransferList);
	if (kTransferList.size() != 1)
		return;

	LC_ActorBase* center = kTransferList.front();
	if (center == NULL)
		return;

	//记录当前的朝向，角度，位置
	float curZRotAngle = center->GetForwardDirAngle();
	UT_Vec2Float curForwardDir = center->GetForwardDir();
	UT_Vec3Int curLocation = center->GetCurrentLocation();

	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	LC_ActorPtrListIter IterBe = kTargetsList.begin();
	LC_ActorPtrListIter IterEd = kTargetsList.end();
	LC_ActorBase* pkTarget = NULL;
	object_id_type lTargetID = 0;
	for (; IterBe != IterEd; ++IterBe)
	{
		pkTarget = (*IterBe);
		lTargetID = pkTarget->GetID();

		if (pkTarget->GetSkillFlag(SK_IMM_PUNCH_BACK))	//目标身上是否有免疫击退标志
		{
			pkTarget->OnImmuneMove(SETE_IMM_PUNCH_MOVE,pkScriptCaller->GetID());
			continue;
		}
		if (IS_NPC_ID(lTargetID) && pkTarget->GetCurrentStateType() == ST_NPC_PUNCH_BACK)	//已经在击退状态
		{
			continue;
		}
		if (IS_PLAYER_CLASS_ID(lTargetID) && pkTarget->GetCurrentStateType() == ST_PLAYER_PUNCH_BACK)	//已经在击退状态
		{
			continue;
		}

		//记录转向目标后的朝向
		UT_Vec3Int targetLocation = pkTarget->GetCurrentLocation();
		UT_Vec3Int faceToForwardDir = targetLocation - curLocation;
		UT_Vec2Float targetForwardDir(Int2Float(faceToForwardDir.x), Int2Float(faceToForwardDir.y));
		float targetZRotAngle = UT_MathBase::GetZRotAngle(targetForwardDir);
		
		//判断击飞方向
		//这里涉及到 ZRot角度是0度附近时的问题，两侧都比0大，所以需要根据差值判断在左侧还是右侧,同理359度
		bool bRightSide = true;
		if (targetZRotAngle < curZRotAngle)
		{
			bRightSide = true;
			if (curZRotAngle - targetZRotAngle > 270.0f)
				bRightSide = false;
		}
		else
		{
			bRightSide = false;
			if (targetZRotAngle - curZRotAngle > 270.0f)
				bRightSide = true;
		}

		//根据左侧还是右侧计算击飞朝向和预期击飞位置
		float punchZRotAngle = 0;
		if (bRightSide)
			punchZRotAngle = -90.0f;
		else
			punchZRotAngle = 90.0f;
		UT_Vec2Float punchDir = UT_MathBase::ZRot(curForwardDir, punchZRotAngle);
		UT_Vec3Int punchPos = UT_MathBase::V3dIncrementAdd(pkTarget->GetCurrentLocation(), punchDir, iPunchDistance);

		PF_ServerPathManager* pfm = SafeGetSingleton<PF_ServerPathManager>();
		int MapResID = pkTarget->GetMapResID();
		UT_Vec3Int fvec;
		bool ret = pfm->FindDirectFarawayPoint(MapResID, pkTarget->GetCurrentLocation(), punchPos, UT_MathBase::LineLengthXYInt(punchPos, pkTarget->GetCurrentLocation()), fvec);
		if (ret)
		{
			pkTarget->PunchMove(SMAPT_PUNCHBACK, fvec, fPunchTime);
			AttachSkillState(pkScriptCaller, pkTarget->GetID(), 39122101, 1, 0,0,0,0,0);
		}
	}
}

void SC_ServerSkillInterface::DoTransferToPos(GameLogic::LC_ActorBase* pkScriptCaller, int iTargetIndex, int iTransferTarget, float fPunchTime)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}
	//源
	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	UT_Vec3Int pos;

	//传送目标
	if (iTransferTarget != TARGET_LOCATION)
	{
		LC_ActorPtrList kTransferList;
		_findTarget(pkScriptCaller, iTransferTarget, kTransferList);
		if (kTransferList.size() != 1)
		{
			return ;
		}
		LC_ActorPtrListIter transfer = kTransferList.begin();
		pos = (*transfer)->GetCurrentLocation();
	}
	else
	{
		pos = pkScriptCaller->GetLockedLocation();
	}
	LC_ActorPtrListIter IterBe = kTargetsList.begin();
	LC_ActorPtrListIter IterEd = kTargetsList.end();
	LC_ActorBase* pkTarget = NULL;
	//	object_id_type lCallerID = pkScriptCaller->GetID();
	object_id_type lTargetID = 0;
	for (; IterBe != IterEd; ++IterBe)
	{
		pkTarget = (*IterBe);
		lTargetID = pkTarget->GetID();
		//开始击退
		PF_ServerPathManager* pfm = SafeGetSingleton<PF_ServerPathManager>();
		int MapResID = pkTarget->GetMapResID();
		UT_Vec3Int fvec;
		bool ret = pfm->FindDirectFarawayPoint(MapResID, pkTarget->GetCurrentLocation(), pos, UT_MathBase::LineLengthXYInt(pos, pkTarget->GetCurrentLocation()), fvec);
		if (ret)
		{
			pkTarget->PunchMove(SMAPT_PASSIVEMOVE, fvec, fPunchTime);
			AttachSkillState(pkScriptCaller, pkTarget->GetID(), 39122101, 1, 0,0,0,0,0);
		}
	}
}

void SC_ServerSkillInterface::DoTransferToCircle(GameLogic::LC_ActorBase* pkScriptCaller, int iTargetIndex, int iTransferTarget, float fPunchTime, int dis)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}
	//源
	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	UT_Vec3Int pos;

	//传送目标
	if (iTransferTarget != TARGET_LOCATION)
	{
		LC_ActorPtrList kTransferList;
		_findTarget(pkScriptCaller, iTransferTarget, kTransferList);
		if (kTransferList.size() != 1)
		{
			return ;
		}
		LC_ActorPtrListIter transfer = kTransferList.begin();
		pos = (*transfer)->GetCurrentLocation();
	}
	else
	{
		pos = pkScriptCaller->GetLockedLocation();
	}
	LC_ActorPtrListIter IterBe = kTargetsList.begin();
	LC_ActorPtrListIter IterEd = kTargetsList.end();
	LC_ActorBase* pkTarget = NULL;
	//	object_id_type lCallerID = pkScriptCaller->GetID();
	object_id_type lTargetID = 0;
	for (; IterBe != IterEd; ++IterBe)
	{
		pkTarget = (*IterBe);
		lTargetID = pkTarget->GetID();
		//开始击退
		if (pkTarget->GetSkillFlag(SK_IMM_PUNCH_BACK))	//目标身上是否有免疫击退标志
		{
			pkTarget->OnImmuneMove(SETE_IMM_PUNCH_MOVE,pkScriptCaller->GetID());
			continue;
		}
		else if (IS_NPC_ID(lTargetID) && pkTarget->GetCurrentStateType() == ST_NPC_PUNCH_BACK)	//已经在击退状态
		{
			continue;
		}
		else if (IS_PLAYER_CLASS_ID(lTargetID) && pkTarget->GetCurrentStateType() == ST_PLAYER_PUNCH_BACK)	//已经在击退状态
		{
			continue;
		}

		PF_ServerPathManager* pfm = SafeGetSingleton<PF_ServerPathManager>();
		int MapResID = pkTarget->GetMapResID();
		UT_Vec3Int fvec;

		int len = UT_MathBase::LineLengthXYInt(pos, pkTarget->GetCurrentLocation());
		if(len > dis)
		{
			len = dis;
		}

		bool ret = pfm->FindDirectFarawayPointByDis(MapResID, pkTarget->GetCurrentLocation(), pos, len, fvec);
		if (ret)
		{
			pkTarget->PunchMove(SMAPT_PUNCHBACK, fvec, fPunchTime);
			AttachSkillState(pkScriptCaller, pkTarget->GetID(), 39122101, 1, 0,0,0,0,0);
		}
	}
}

void SC_ServerSkillInterface::UpdateAIcontrolPos(GameLogic::LC_ActorBase* pkScriptCaller)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, TARGET_LOCKED, kTargetsList);
	if (kTargetsList.empty())
	{
		return;
	}
	if (IS_PLAYER_ID(pkScriptCaller->GetID()))
	{
		LC_PlayerBase* player = (LC_PlayerBase*)pkScriptCaller;
		if (!player->GetAIControl())
		{
			return;
		}
	}
	int iRadius = (pkScriptCaller->GetProperty_ShapeRadius() + kTargetsList.front()->GetProperty_ShapeRadius()) /*>> 1*/;
	UT_Vec3Int res_pos;
	bool ret = UT_PathFindHelp::GenNearByPosInCircle(pkScriptCaller->GetMapResID(), pkScriptCaller->GetCurrentLocation(), kTargetsList.front()->GetCurrentLocation(), iRadius, res_pos);
	if (!ret)
	{
		res_pos = kTargetsList.front()->GetCurrentLocation();
	}
	pkScriptCaller->SetTargetLocation(res_pos);
}

void SC_ServerSkillInterface::UpdateAIcontrolDir(GameLogic::LC_ActorBase* pkScriptCaller)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}
	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, TARGET_LOCKED, kTargetsList);
	if (kTargetsList.empty())
	{
		return;
	}
	if (IS_PLAYER_ID(pkScriptCaller->GetID()))
	{
		LC_PlayerBase* player = (LC_PlayerBase*)pkScriptCaller;
		if (!player->GetAIControl())
		{
			return;
		}
	}
	//设置朝向
	UT_Vec2Float rkDir;
	rkDir.x = Int2Float(kTargetsList.front()->GetCurrentLocation().x - pkScriptCaller->GetCurrentLocation().x);
	rkDir.y = Int2Float(kTargetsList.front()->GetCurrentLocation().y - pkScriptCaller->GetCurrentLocation().y);
	rkDir.Unitize();
	pkScriptCaller->SetForwardDir(rkDir);
	pkScriptCaller->SetLockedLocation(kTargetsList.front()->GetCurrentLocation());
}

bool SC_ServerSkillInterface::CheckGuildAssert(GameLogic::LC_ActorBase* pkScriptCaller, int asserttype, int assertcount)
{
	if (NULL == pkScriptCaller)
	{
		return false;
	}
	if (!IS_PLAYER_ID(pkScriptCaller->GetID()))
	{
		return false;
	}
	LC_ServerPlayer* player = (LC_ServerPlayer*)pkScriptCaller;
	uint64_t guildid = player->GetGuildID();
	if (guildid == INVALID_GUILD_ID)
	{
		return false;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	LC_ServerGuildInfo* guildinfo = pkGuildManager->GetGuildInfoByID(guildid);
	if (guildinfo == NULL)
	{
		return false;
	}
	mem::vector<PS_CostEntry> entry;
	PS_CostEntry temp;
	temp.m_ItemType = asserttype;
	temp.m_ItemCount = assertcount;
	entry.push_back(temp);
	ResultType ret = guildinfo->CheckCost(entry);
	if (ret != RE_SUCCESS)
	{
		return false;
	}
	return true;
}

bool SC_ServerSkillInterface::CheckGivenPower(GameLogic::LC_ActorBase* pkScriptCaller)
{
	if (NULL == pkScriptCaller)
	{
		return false;
	}
	if (!IS_PLAYER_CLASS_ID(pkScriptCaller->GetID()))
	{
		return false;
	}

	pkScriptCaller = ((LC_ServerPlayer*)pkScriptCaller)->GetOwnerPlayer();
	if(NULL == pkScriptCaller)
		return false;

	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerMapLogic_Place* maplogic = (LC_ServerMapLogic_Place*)pkWorldManager->GetMapLogic(pkScriptCaller->GetMapLogicID());
	if (maplogic == NULL)
	{
		return false;
	}
	LC_ServerMapLogic_WarPlay* play = (LC_ServerMapLogic_WarPlay*)maplogic->GetPlay(PlayType_War);
	if (play == NULL)
	{
		return false;
	}
	if (play->CheckGivenPower((GameLogic::LC_PlayerBase*)pkScriptCaller) != RE_SUCCESS)
	{
		return false;
	}
	return true;
}

void SC_ServerSkillInterface::GivenPowerGuild(GameLogic::LC_ActorBase* pkScriptCaller)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}
	if (!IS_PLAYER_CLASS_ID(pkScriptCaller->GetID()))
	{
		return;
	}
	LC_ServerPlayer* player = (LC_ServerPlayer*)pkScriptCaller;
	player = player->GetOwnerPlayer();
	if(player == NULL)
	{
		return;
	}

	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerMapLogic_Place* maplogic = (LC_ServerMapLogic_Place*)pkWorldManager->GetMapLogic(player->GetMapLogicID());
	if (maplogic == NULL)
	{
		return;
	}
	LC_ServerMapLogic_WarPlay* play = (LC_ServerMapLogic_WarPlay*)maplogic->GetPlay(PlayType_War);
	if (play == NULL)
	{
		return;
	}
	play->PreGivenPower(player);

}

//0 有人控制不是你，1 你以控制，2 无人控制但你不满足控制条件，3 无人控制，你满足控制条件
int SC_ServerSkillInterface::CheckControlCatapult(GameLogic::LC_ActorBase* pkScriptCaller)
{
	if (NULL == pkScriptCaller)
	{
		return 2;
	}
	if (!IS_PLAYER_ID(pkScriptCaller->GetID()))
	{
		return 2;
	}
	LC_ServerPlayer* player = (LC_ServerPlayer*)pkScriptCaller;
	int targetid = player->GetLockedTargetID();

	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerMapLogic_Place* maplogic = (LC_ServerMapLogic_Place*)pkWorldManager->GetMapLogic(pkScriptCaller->GetMapLogicID());
	if (maplogic == NULL)
	{
		return 2;
	}
	LC_ServerMapLogic_WarPlay* play = (LC_ServerMapLogic_WarPlay*)maplogic->GetPlay(PlayType_War);
	if (play == NULL)
	{
		return 2;
	}
	WarCatapult* catapult = play->GetCatapult(targetid);
	if (catapult == NULL)
	{
		return 2;
	}
	return catapult->CheckCanControl(player);
}

void SC_ServerSkillInterface::ControlCatapult(GameLogic::LC_ActorBase* pkScriptCaller)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}
	if (!IS_PLAYER_ID(pkScriptCaller->GetID()))
	{
		return;
	}
	LC_ServerPlayer* player = (LC_ServerPlayer*)pkScriptCaller;
	int targetid = player->GetLockedTargetID();
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerMapLogic_Place* maplogic = (LC_ServerMapLogic_Place*)pkWorldManager->GetMapLogic(pkScriptCaller->GetMapLogicID());
	if (maplogic == NULL)
	{
		return;
	}
	LC_ServerMapLogic_WarPlay* play = (LC_ServerMapLogic_WarPlay*)maplogic->GetPlay(PlayType_War);
	if (play == NULL)
	{
		return;
	}
	WarCatapult* catapult = play->GetCatapult(targetid);
	if (catapult == NULL)
	{
		return;
	}
	catapult->Control(player);
}

bool SC_ServerSkillInterface::CheckGuildLadderOccupation(GameLogic::LC_ActorBase* pkCaller)
{
	return true;
}
void SC_ServerSkillInterface::GuildLadderOccupation(GameLogic::LC_ActorBase* pkCaller, int iTargetIndex)
{
	if (NULL == pkCaller)
	{
		return;
	}
	if (!IS_PLAYER_CLASS_ID(pkCaller->GetID()))
	{
		return;
	}
	LC_StoryLogic_Manager* pkStoryLogicManager = SERVER_GET_STORYLOGIC_MANAGER;
	if (NULL == pkStoryLogicManager)
	{
		return;
	}
	object_id_type targetid = pkCaller->GetLockedTargetID();
	LC_ActorPtrList kTargetsList;
	_findTarget(pkCaller, iTargetIndex, kTargetsList);
	LC_ActorPtrListIter Iter;
	GameLogic::LC_ActorBase* pkTarget;
	TRAV(kTargetsList, Iter)
	{
		pkTarget = *Iter;
		pkStoryLogicManager->FactionOccupation(pkCaller, pkTarget);
	}
}

int SC_ServerSkillInterface::GetGuildLadderFaction(GameLogic::LC_ActorBase* pkCaller)
{
	int32_t ret = FACTION_NONE;
	if (NULL == pkCaller)
	{
		return ret;
	}
	object_id_type iID = pkCaller->GetID();
	if (!IS_PLAYER_CLONE_ID(iID))
	{
		return ret;
	}
	unique_id_impl uid = 1;
	LC_GuildLadderManager* pkGuildLadderMgr = LC_GuildLadderManager::GetSingletonPtr();
	if (NULL == pkGuildLadderMgr)
	{
		return ret;
	}
	return pkGuildLadderMgr->GetFactionByGuildUID(uid);
}

void SC_ServerSkillInterface::SetTargetPlayerAIControl(GameLogic::LC_ActorBase* pkScriptCaller, int iTargetIndex, bool bFlag)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}
	//服务器实现
	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	for (LC_ActorPtrList::iterator itr = kTargetsList.begin(); itr != kTargetsList.end(); ++itr)
	{
		LC_ActorBase* pkActor = (*itr);
		if (IS_PLAYER_ID(pkActor->GetID()))
		{
			LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkActor;
			pkPlayer->SetAIControl(bFlag);
			pkPlayer->SetAIControlStand(bFlag);
		}
	}
}

void SC_ServerSkillInterface::SendNotifyStrToClientPlayer(GameLogic::LC_ActorBase* pkScriptCaller, const StringType& str, int msgType, int level)
{
	UT_ServerHelper::_GameNoticeFmort noticeFmort;
	bool res = UT_ServerHelper::GetGameNoticeFromString(str, noticeFmort);
	if(!res)
	{
		return;
	}
	noticeFmort.SetChannelType(msgType);

	//服务器实现
	if (pkScriptCaller)
	{
		if (IS_PLAYER_ID(pkScriptCaller->GetID()))
		{
			LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)pkScriptCaller;			
			UT_ServerHelper::BrocastNoticeMsg(noticeFmort);
		}
		else if (IS_PLAYER_ID(pkScriptCaller->GetFollowedTargetID()))
		{
			object_id_type playID = pkScriptCaller->GetFollowedTargetID();
			LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)GetActor(playID);
			UT_ServerHelper::BrocastNoticeMsg(noticeFmort);
		}
	}
}

void SC_ServerSkillInterface::SendNotifyErrorIDStrToClientPlayer(GameLogic::LC_ActorBase* pkScriptCaller, int systemErrorID, int level)
{
	//服务器实现
	CSVFile::CF_SystemError::DataEntry* entry = CSVFile::SafeGetCSVFileDataEntryPtr<CSVFile::CF_SystemError>(systemErrorID);
	if (!entry)
	{
		return ;
	}

	if (pkScriptCaller)
	{
		if (IS_PLAYER_ID(pkScriptCaller->GetID()))
		{
			LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)pkScriptCaller;
			pkServerPlayer->SendNotifyStrToClient(entry->_sErrorMsg, (PromptChannel)entry->_lMsgType, (PromptLevel)level);
		}
		else if (IS_PLAYER_ID(pkScriptCaller->GetFollowedTargetID()))
		{
			object_id_type playID = pkScriptCaller->GetFollowedTargetID();
			LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)GetActor(playID);
			pkServerPlayer->SendNotifyStrToClient(entry->_sErrorMsg, (PromptChannel)entry->_lMsgType, (PromptLevel)level);
		}
	}
}

void SC_ServerSkillInterface::AddGuildWarAutoExp(GameLogic::LC_ActorBase* pkScriptCaller, attr_value_type iExpValue)
{
	if (pkScriptCaller && IS_PLAYER_ID(pkScriptCaller->GetID()))
	{
		LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)pkScriptCaller;
		LC_GuildWarAssert* gwAssert = pkServerPlayer->GetGuildWarAssert();
		if (gwAssert)
		{
			gwAssert->AddExp(iExpValue);
		}
	}
}

void SC_ServerSkillInterface::GuildWarCapapultMsg(GameLogic::LC_ActorBase* pkScriptCaller, GameLogic::LC_ActorBase* pkTarget, int damage)
{
	if (pkScriptCaller != NULL && IS_PLAYER_ID(pkScriptCaller->GetID()) && pkTarget != NULL && IS_PLAYER_ID(pkTarget->GetID()))
	{
		LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)pkTarget;
		MG_GuildWar_Capapult_Msg_Notify msg;
		msg.m_CharID = pkServerPlayer->GetInstance();
		TPSTR2STDSTR(pkServerPlayer->GetOwnerCharName(), msg.m_Name);
		msg.m_GuildId = pkServerPlayer->GetGuildID();
		TPSTR2STDSTR(pkServerPlayer->GetGuildName(), msg.m_GuildName);
		msg.m_ServerId = pkServerPlayer->GetCitizenship();
		msg.m_Damage = damage;
		((LC_ServerPlayer*)pkScriptCaller)->SendMsgToClient(MGPT_NOTIFY_GUILDWAR_CAPAPULT_MSG, &msg);
	}
}

void SC_ServerSkillInterface::GuildWarCapapultAllMsg(GameLogic::LC_ActorBase* pkScriptCaller, int count, int damage)
{
	if (pkScriptCaller != NULL && IS_PLAYER_ID(pkScriptCaller->GetID()))
	{
		LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)pkScriptCaller;

		UT_ServerHelper::_GameNoticeFmort noticeFmort;
		noticeFmort.SetMsgID(47);
		noticeFmort.AddString(pkServerPlayer->GetGuildName());
		noticeFmort.AddString(pkServerPlayer->GetOwnerCharName());
		noticeFmort.AddNumber(count);
		noticeFmort.AddNumber(damage);
		UT_ServerHelper::BrocastNoticeMsg(noticeFmort);
	}
}

bool SC_ServerSkillInterface::LockAttribute(GameLogic::LC_ActorBase* pkScriptCaller, int iAttributeType, bool bLock, int iValue)
{
	LC_ActorAttributeMap* pkAttributeMap = pkScriptCaller->GetAttributeMap();
	return pkAttributeMap->LockAttribute(iAttributeType, bLock, iValue);
}

int SC_ServerSkillInterface::LockRobberInCircle(GameLogic::LC_Attacker* pkScriptCaller, int iSrcIndex, int lActorFilterFuncID, bool bExclusive, int iCircleRadius, int iMaxLockCount, int iRandomIndex, object_id_type lFileterPlayerID)
{
	if (NULL == pkScriptCaller)
	{
		return 0;
	}
	if (!IS_PLAYER_CLASS_ID(pkScriptCaller->GetID()) && !IS_NPC_ID(pkScriptCaller->GetID()))
	{
		return 0;
	}

	LC_ActorBase* pkFilterPlayer = NULL;
	if (pkScriptCaller->GetID() == lFileterPlayerID)
	{
		pkFilterPlayer = (LC_ActorBase*)pkScriptCaller;
	}
	else
	{
		pkFilterPlayer = _findActor(lFileterPlayerID);
	}
	if (!pkFilterPlayer)
	{
		return 0;
	}

	UT_Circle kCircle;
	kCircle.iRadius = iCircleRadius;

	if (iSrcIndex == TARGET_LOCATION)
	{
		kCircle.kCenterPoint = pkScriptCaller->GetLockedLocation();
	}
	else if (iSrcIndex == TARGET_BULLET_LOCATION)
	{
		kCircle.kCenterPoint = pkScriptCaller->GetBulletLocation();
	}
	else
	{
		LC_ActorPtrList kTargetsList;
		_findTarget(pkScriptCaller, iSrcIndex, kTargetsList);
		if (kTargetsList.empty())
		{
			GfxWriteLog(LOG_WARNING_FILELINE, LOG_SWITCH_WARNING, "Script: attempt to zone lock with illegal target location");
			LC_TargetMap* pkTargetMap = pkScriptCaller->GetTargetMap();
			pkTargetMap->GetCurrentTargets().clear();
			return 0;
		}

		LC_ActorBase* pkTarget = kTargetsList.front();
		if (!pkTarget)
		{
			GfxWriteLog(LOG_WARNING_FILELINE, LOG_SWITCH_WARNING, "Script: attempt to zone lock with illegal target location");
			LC_TargetMap* pkTargetMap = pkScriptCaller->GetTargetMap();
			pkTargetMap->GetCurrentTargets().clear();
			return 0;
		}
		kCircle.kCenterPoint = pkTarget->GetCurrentLocation();
	}

	LC_MapBase* pkMap = pkScriptCaller->GetMap();
	if (!pkMap)
	{
		GfxWriteLog(LOG_WARNING_FILELINE, LOG_SWITCH_WARNING, "Script: attempt to zone lock with illegal target location");
		LC_TargetMap* pkTargetMap = pkScriptCaller->GetTargetMap();
		pkTargetMap->GetCurrentTargets().clear();
		return 0;
	}

	CF_ActorFilterFunc* pkCSVActorFilterFunc = CF_ActorFilterFunc::GetSingletonPtr();
	CF_ActorFilterFunc::DataEntry* pkData = pkCSVActorFilterFunc->GetEntryPtr(lActorFilterFuncID);
	if (!pkData)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "Target filter not found%d", lActorFilterFuncID);
		LC_TargetMap* pkTargetMap = pkScriptCaller->GetTargetMap();
		pkTargetMap->GetCurrentTargets().clear();
		return 0;
	}

	//从地图返回区域内的Actor队列
	LC_ActorPtrVector* pkLockResult = pkMap->LockActorsInCircle(kCircle, pkData);
	if (pkLockResult->empty())
	{
		LC_TargetMap* pkTargetMap = pkScriptCaller->GetTargetMap();
		pkTargetMap->GetCurrentTargets().clear();
		return 0;
	}

	LC_ActorPtrVectorIter iter = pkLockResult->begin();
	while (iter != pkLockResult->end())
	{
		LC_ActorBase* pkActor = (*iter);
		if (pkScriptCaller->GetID() != pkActor->GetAdditionTargetID())
		{
			iter = pkLockResult->erase(iter);
		}
		else
		{
			++iter;
		}
	}

	//根据过滤设置,过滤Actor队列
	object_id_vector kFilterResult = LC_Helper::FilterActorsSortDist(pkFilterPlayer, *pkLockResult, pkData);

	LC_TargetMap* pkTargetMap = pkScriptCaller->GetTargetMap();

	//与以前锁定的对象互斥，就是不再锁定以前锁定过的对象，只锁定新的
	if (bExclusive)
	{
		pkTargetMap->ExclusiveTargets(kFilterResult);	//从kFilterResult中排除pkTargetMap已锁定的对象
	}
	//删除掉多余的对象
	while (UInt2Int((unsigned int)kFilterResult.size()) > iMaxLockCount)
	{
		kFilterResult.pop_back();
	}
	//将结果添加到目标表中
	pkTargetMap->PushTargets(kFilterResult);
	int iLockCount = (int)kFilterResult.size();
	return iLockCount;
}

object_id_type SC_ServerSkillInterface::MapLogicSpawnNPC(GameLogic::LC_ActorBase* pkScriptCaller, int iTargetIndex, int minRadius, int maxRadius, int lCharTypeID)
{
	if (NULL == pkScriptCaller)
	{
		return 0;
	}
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerMapLogic_Place* ptr = pkWorldManager->GetMapLogic(pkScriptCaller->GetMapLogicID());
	if (ptr)
	{
		UT_Vec3Int  aroundLocation;
		if (UT_PathFindHelp::GenRandomPosInCircle(ptr->GetMapResID(), pkScriptCaller->GetCurrentLocation(), minRadius, maxRadius, aroundLocation))
		{
			LC_TranscriptionListEntry transEntry;
			transEntry.m_iMapID				= pkScriptCaller->GetMapLogicID();
			transEntry.m_kSpawnLocation		= aroundLocation;
			transEntry.m_iSpawnCharType		= lCharTypeID;
			transEntry.m_iZRotAngle			= (int)pkScriptCaller->GetForwardDirAngle();
			LC_ServerNPC* pkServerNPC = ptr->SpawnNPC(transEntry, -1, FACTION_EVIL);
			if (pkServerNPC)
			{
				return pkServerNPC->GetID();
			}
		}
	}
	return 0;
}

object_id_type SC_ServerSkillInterface::MapLogicSpawnNPCXYZ(GameLogic::LC_ActorBase* pkScriptCaller, int x, int y, int z, int lCharTypeID)
{
	if (NULL == pkScriptCaller)
	{
		return 0;
	}
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerMapLogic_Place* ptr = pkWorldManager->GetMapLogic(pkScriptCaller->GetMapLogicID());
	if (ptr)
	{
		UT_Vec3Int  aroundLocation;
		if (UT_PathFindHelp::GenRandomPosInCircle(ptr->GetMapResID(), UT_Vec3Int(x, y, z), 0, 0, aroundLocation))
		{
			LC_TranscriptionListEntry transEntry;
			transEntry.m_iMapID				= pkScriptCaller->GetMapLogicID();
			transEntry.m_kSpawnLocation		= aroundLocation;
			transEntry.m_iSpawnCharType		= lCharTypeID;
			transEntry.m_iZRotAngle			= (int)pkScriptCaller->GetForwardDirAngle();
			LC_ServerNPC* pkServerNPC = ptr->SpawnNPC(transEntry, -1, FACTION_EVIL);
			if (pkServerNPC)
			{
				return pkServerNPC->GetID();
			}
		}
	}
	return 0;
}

object_id_type SC_ServerSkillInterface::MapLogicSpawnEliteNPC(GameLogic::LC_ActorBase* pkScriptCaller, int iTargetIndex, int minRadius, int maxRadius, int lCharTypeID, int rule1, int rule2, int rule3)
{
	if (NULL == pkScriptCaller)
	{
		return 0;
	}
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerMapLogic_Place* ptr = pkWorldManager->GetMapLogic(pkScriptCaller->GetMapLogicID());
	if (ptr)
	{
		UT_Vec3Int  aroundLocation;
		if (UT_PathFindHelp::GenRandomPosInCircle(ptr->GetMapResID(), pkScriptCaller->GetCurrentLocation(), minRadius, maxRadius, aroundLocation))
		{
			LC_TranscriptionListEntry transEntry;
			transEntry.m_iMapID				= pkScriptCaller->GetMapLogicID();
			transEntry.m_kSpawnLocation		= aroundLocation;
			transEntry.m_iSpawnCharType		= lCharTypeID;
			transEntry.m_iZRotAngle			= (int)pkScriptCaller->GetForwardDirAngle();
			EliteSkillRuleSetType d;
			EliteSkillRuleEntry ee;
			ee.SkillRuleID = rule1;
			ee.Rate = 5000;
			d.push_back(ee);
			ee.SkillRuleID = rule2;
			ee.Rate = 5000;
			d.push_back(ee);
			ee.SkillRuleID = rule3;
			ee.Rate = 5000;
			d.push_back(ee);
			ptr->SpawnEliteNPC(transEntry, 0, d, -1, FACTION_EVIL);
			return 0;
		}
	}
	return 0;
}

void SC_ServerSkillInterface::SetNPCSurvivalTime(GameLogic::LC_ActorBase* pkScriptCaller, int iTargetIndex, float dTime)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}
	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	for (LC_ActorPtrList::iterator itr = kTargetsList.begin(); itr != kTargetsList.end(); ++itr)
	{
		LC_ActorBase* pkActor = (*itr);
		if (IS_NPC_ID(pkActor->GetID()))
		{
			LC_NPCBase* pkNPC = (LC_NPCBase*)pkActor;
			pkNPC->SetSurvivalTime((int)dTime);
		}
	}
	return ;
}

int SC_ServerSkillInterface::GetCurrentMapSummorCount(GameLogic::LC_ActorBase* pkScriptCaller)
{
	if (NULL == pkScriptCaller)
	{
		return 0;
	}
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	int count = 0;
	LC_ServerMapLogic_Place* ptr = pkWorldManager->GetMapLogic(pkScriptCaller->GetMapLogicID());
	if (ptr)
	{
		GameLogic::LC_ActorPtrList pkNPCList;
		mem::set<int> setA, setB;
		ptr->GetAllMonster(pkNPCList, setA, setB);
		GameLogic::LC_ActorPtrList::iterator iter = pkNPCList.begin();
		for (; iter != pkNPCList.end() ; iter++)
		{
			if ((*iter)->GetSummonOwnerID() == (int)pkScriptCaller->GetID())
			{
				count++;
			}
		}
	}
	return count;
}

void SC_ServerSkillInterface::NotifyTranscriptionData(GameLogic::LC_ActorBase* pkScriptCaller, int kType, int param1, int param2)
{
	if (NULL == pkScriptCaller)
	{
		return ;
	}
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerMapLogic_Place* ptr = pkWorldManager->GetMapLogic(pkScriptCaller->GetMapLogicID());
	if (ptr)
	{
		ptr->BroadCastGameEventToClient(TranscriptionEventType(kType), param1, param2);
	}
}

void SC_ServerSkillInterface::NotifySingleTranscriptionData(GameLogic::LC_ActorBase* pkScriptCaller, GameLogic::LC_ActorBase* pkPlayer, int kType, int param1, int param2)
{
	if (NULL == pkScriptCaller || pkPlayer == NULL)
	{
		return ;
	}
	if (!IS_PLAYER_ID(pkPlayer->GetID()))
	{
		return ;
	}
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerMapLogic_Place* ptr = pkWorldManager->GetMapLogic(pkScriptCaller->GetMapLogicID());
	if (ptr)
	{
		LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)pkPlayer;
		ptr->NotifyGameEventToClient(pkServerPlayer, TranscriptionEventType(kType), param1, param2);
	}
}

void SC_ServerSkillInterface::DismissTargetVehicle(GameLogic::LC_ActorBase* pkScriptCaller, int iTargetIndex)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}
	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	LC_ActorPtrListIter Iter;
	LC_ActorBase* pkTarget;
	TRAV(kTargetsList, Iter)
	{
		pkTarget = *Iter;
		if (IS_PLAYER_CLASS_ID(pkTarget->GetID()))
		{
			LC_ServerPlayer* pkServerPLayer = (LC_ServerPlayer*)pkTarget;
			pkServerPLayer->DismissVehicle();
		}
	}
}
void SC_ServerSkillInterface::NotifyClientShowTips(const StringType& strContent, uint16_t nFlag)
{
	if (strContent.empty())
	{
		return;
	}

	UT_ServerHelper::_GameNoticeFmort noticeFmort;
	bool res = UT_ServerHelper::GetGameNoticeFromString(strContent, noticeFmort);
	if(!res)
	{
		return;
	}

	UT_ServerHelper::BrocastNoticeMsg(noticeFmort);
}

void SC_ServerSkillInterface::NotifyClientShowTipsInMap(const StringType& strContent, int32_t iMapLogicID, int32_t iMapResID)
{
	if (strContent.empty())
	{
		return;
	}

	UT_ServerHelper::_GameNoticeFmort noticeFmort;
	bool res = UT_ServerHelper::GetGameNoticeFromString(strContent, noticeFmort);
	if(!res)
	{
		return;
	}

	noticeFmort.SetChannelType(PROMPT_CHANNEL_ANNOUNCE);
	UT_ServerHelper::BrocastNoticeMsg(noticeFmort);
}


void SC_ServerSkillInterface::NotifyClientShowTipsInGuild(uint64_t iGuildID, int iType, const StringType& strContent)
{
	if (strContent.empty())
	{
		return;
	}

	UT_ServerHelper::_GameNoticeFmort noticeFmort;
	bool res = UT_ServerHelper::GetGameNoticeFromString(strContent, noticeFmort);
	if(!res)
	{
		return;
	}
	
	LC_ServerGuildManager* pkGuildMgr = LC_ServerGuildManager::GetSingletonPtr();
	LC_ServerGuildInfo* pkGuildInfo = pkGuildMgr->GetGuildInfoByID(iGuildID);
	if (NULL == pkGuildInfo)
	{
		return;
	}

	noticeFmort.SetChannelType(iType);
	UT_ServerHelper::BrocastNoticeMsg(noticeFmort);
}

void SC_ServerSkillInterface::GodKill(GameLogic::LC_ActorBase* pkScriptCaller, int iTargetType, int lSkillTypeID)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}
	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetType, kTargetsList);
	LC_ActorPtrListIter Iter;
	LC_ActorBase* pkTarget;
	TRAV(kTargetsList, Iter)
	{
		pkTarget = *Iter;
		pkTarget->SetAttackerID(pkScriptCaller->GetID());
		pkTarget->SetAttackerSkillTypeID(lSkillTypeID);
		pkTarget->SetHP(0);
	}
}

void SC_ServerSkillInterface::AddOssRecharge(unique_id_type charID, int lYuanbao, int iSrcType, const StringType& orderid)
{
	oss_recharge(charID, lYuanbao, orderid.c_str(), iSrcType);
	//oss_360_log_recharge( iYuanbao, iYuanbao * YUANBAO_RMB_RATE,  );
	GfxWriteFmtLog(LOG_BILL_FILELINE, LOG_SWITCH_BILL, "AddUnbindYuanbao", "mail", "", charID, lYuanbao, orderid, 0);
}

void SC_ServerSkillInterface::AddUnbindYuanbao(GameLogic::LC_ActorBase* pkScriptCaller, int iTargetType, MoneyType lYuanbao, int iSrcType, const StringType& orderid)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetType, kTargetsList);
	LC_ActorPtrListIter Iter;
	LC_ServerPlayer* pkTarget;

	StringStreamType infolist;
	mem::vector<PlayerYuanbao>	YuanBaoList;
	TRAV(kTargetsList, Iter)
	{
		if (IS_PLAYER_ID((*Iter)->GetID()))
		{
			pkTarget = (LC_ServerPlayer*)(*Iter);
			if (pkTarget == NULL)
			{
				continue;
			}

			MoneyType llDelta = 0;
			
			CF_Charge::DataEntryMapExternIterator kIter = CF_Charge::GetSingletonPtr()->GetIterator();
			for(; kIter.HasMoreElements(); kIter.MoveNext())
			{
				CF_Charge::DataEntry* pkEntry = kIter.PeekNextValuePtr();
				MoneyType money = pkEntry->_rate * lYuanbao;
				pkTarget->AddCash(money, (CashType)pkEntry->_cashType, iSrcType, lYuanbao);
				if(pkEntry->_mainMoney)
					oss_360_log_recharge( money, lYuanbao,pkTarget->GetCashNumByCashType((CashType)pkEntry->_cashType), orderid.c_str(), "", pkTarget );
				GfxWriteFmtLog(LOG_BILL_FILELINE, LOG_SWITCH_BILL, "AddUnbindYuanbao", "yuanbao", pkTarget->GetUserName().c_str(), pkTarget->GetID(), pkTarget->GetInstanceCitizen(), pkTarget->GetInstanceCharID(), lYuanbao, orderid);
			}
			

		/*	bool brel = pkTarget->AddCash(lYuanbao, CT_UNBIND_YUANBAO, iSrcType, 0, 0, true, &llDelta);
			if (brel && IS_CHARGE_STATISTICAL(iSrcType))
			{
				oss_recharge(pkTarget, (int32_t)llDelta, orderid.c_str(), pkTarget->GetVIPLevel(), pkTarget->GetVIPCredit(), iSrcType);
			
				
				GfxWriteFmtLog(LOG_BILL_FILELINE, LOG_SWITCH_BILL, "AddUnbindYuanbao", "yuanbao", pkTarget->GetUserName().c_str(), pkTarget->GetID(), pkTarget->GetInstanceCitizen(), pkTarget->GetInstanceCharID(), lYuanbao, orderid, brel);
			}*/

			PlayerYuanbao rNode;
			rNode.uid = pkTarget->GetUserID();
			rNode.order_id = orderid;
			rNode.role_id = pkTarget->GetInstance();
			rNode.role_name = pkTarget->GetOwnerCharName();
			rNode.skey = GetGameServerApp()->GetServerInfo().m_iServerID;
			rNode.coins = (int)pkTarget->GetUnBindYuanBao().GetMoney();

			YuanBaoList.push_back(rNode);
		}
	}
	boost::ajson::save_to_buff(YuanBaoList, infolist);
	if (!YuanBaoList.empty())
	{
		Utility::LC_CommandParserBase::GetSingletonPtr()->SetResult(RE_SUCCESS, "SUCCESS", infolist.str());
	}
	else
	{
		Utility::LC_CommandParserBase::GetSingletonPtr()->SetResult(-3, "SUCCESS", infolist.str());
	}
}

void SC_ServerSkillInterface::AddYuanZhengPoint(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, MoneyType iCash, int iCashType, int iSrcType, int iSrcID)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	LC_ActorPtrList kTargetList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetList);
	LC_ActorPtrListIter iter;
	LC_ServerPlayer* pkTarget = NULL;
	TRAV(kTargetList, iter)
	{
		pkTarget = (LC_ServerPlayer*)*iter;
		pkTarget->GetYuanZhengPointAsset().AddTimes(pkTarget, iCash);
	}
}

int SC_ServerSkillInterface::ActiveSubTransform(GameLogic::LC_ActorBase* pkScriptCaller, int iTargetType, int transFormID, int setAddExpFlag)
{
	if (NULL == pkScriptCaller)
	{
		return RE_FAIL;
	}

	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetType, kTargetsList);
	LC_ActorPtrListIter Iter;
	LC_ServerPlayer* pkTarget;
	TRAV(kTargetsList, Iter)
	{
		if (IS_PLAYER_ID((*Iter)->GetID()))
		{
			pkTarget = (LC_ServerPlayer*)(*Iter);
			if (pkTarget == NULL)
			{
				continue;
			}

			CF_SubordinateTransform::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_SubordinateTransform>(transFormID);
			if(NULL == pkData)
			{
				return RE_FAIL;
			}
		
			ResultType res = pkTarget->AddSubTransform(pkData->_iType, transFormID, true);
			if(RE_SUCCESS == res)
			{
				MG_RLT_SubordinateOperation rltMsg;
				rltMsg.m_lPlayerID = pkTarget->GetID();
				rltMsg.m_iSubType = pkData->_iType;
				rltMsg.m_iOPCode = LC_SUB_OP_ADD_TRANSFORM;
				rltMsg.m_iOPParam = transFormID;
				rltMsg.m_iOPRslt = res;
				pkTarget->SendMsgToClient(MGPT_RLT_SUBORDINATE_OPERATION, &rltMsg);
			}
			else if(RE_SUBTRANSFORM_ERROR_1 == res)
			{
				res = pkTarget->AddSubTransformItem(transFormID);
			}
			return res;
		}
	}
	return RE_FAIL;
}

int SC_ServerSkillInterface::ActiveBuddyTransform(GameLogic::LC_ActorBase* pkScriptCaller, int iTargetType, int transFormID, int buddyID)
{
	if (NULL == pkScriptCaller)
	{
		return RE_FAIL;
	}

	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetType, kTargetsList);
	LC_ActorPtrListIter Iter;
	LC_ServerPlayer* pkTarget;
	TRAV(kTargetsList, Iter)
	{
		if (IS_PLAYER_ID((*Iter)->GetID()))
		{
			pkTarget = (LC_ServerPlayer*)(*Iter);
			if (pkTarget == NULL)
			{
				continue;
			}

			CF_BuddyTransform::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_BuddyTransform>(transFormID);
			if(NULL == pkData)
			{
				return RE_FAIL;
			}

			ResultType res = pkTarget->ActiveBuddyTransform(buddyID, transFormID, true);
			if(RE_BUDDYTRANSFORM_ERROR_1 == res)
			{
				return pkTarget->AddBuddyTransformItem(transFormID);
			}

			return res;
		}
	}
	return RE_FAIL;
}

bool SC_ServerSkillInterface::OpenFDeposit(GameLogic::LC_ActorBase* pkScriptCaller)
{
	if (pkScriptCaller == NULL || !IS_PLAYER_CLASS_ID(pkScriptCaller->GetID()))
	{
		return false;
	}

	LC_ServerPlayer* pkPlayer = static_cast<LC_ServerPlayer*>(pkScriptCaller);

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (NULL == pkPlayer)
	{
		return false;
	}

	pkPlayer->OpenFDeposit(true);
	pkPlayer->SyncVIPInfo();

	return true;
}

bool SC_ServerSkillInterface::ItemUseAddBossDropTimes(GameLogic::LC_ActorBase* pkScriptCaller, int type, int times, int consumeCount)
{
	if (pkScriptCaller == NULL || !IS_PLAYER_CLASS_ID(pkScriptCaller->GetID()))
	{
		return false;
	}

	LC_ServerPlayer* pkPlayer = static_cast<LC_ServerPlayer*>(pkScriptCaller);

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (NULL == pkPlayer)
	{
		return false;
	}

	int addTimes = times*consumeCount;
	if(addTimes <= 0)
	{
		return false;
	}

	CRecoverTimes* pkRecoverAsset = pkPlayer->GetPKRecoverAssetByType(type);
	if(NULL == pkRecoverAsset)
	{
		return false;
	}

	int32_t dropTime= pkRecoverAsset->AddTimes(pkPlayer, addTimes, false);
	return dropTime>0 ? true : false;
}

bool SC_ServerSkillInterface::ItemUseActivateCommanderTitleID(GameLogic::LC_ActorBase* pkScriptCaller, int id)
{
	if (pkScriptCaller == NULL || !IS_PLAYER_CLASS_ID(pkScriptCaller->GetID()))
	{
		return false;
	}

	LC_ServerPlayer* pkPlayer = static_cast<LC_ServerPlayer*>(pkScriptCaller);

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (NULL == pkPlayer)
	{
		return false;
	}

	return pkPlayer->ActivateCommanderTitleFrameID(id);
}

int SC_ServerSkillInterface::GetSummorIDCount(GameLogic::LC_ActorBase* pkScriptCaller)
{
	int iCount = 0;
	LC_NPCSummorManager* mgr = LC_NPCSummorManager::GetSingletonPtr();
	if (mgr && pkScriptCaller)
	{
		LC_NPCSummorElement* pkElem = mgr->GetNPCSummorInfo(pkScriptCaller->GetCharType());
		if (pkElem)
		{
			iCount = (int)pkElem->GetSummorCharTypeIDVec().size();
		}
	}
	return iCount;
}

object_id_type SC_ServerSkillInterface::GetSummorIDByIndex(GameLogic::LC_ActorBase* pkScriptCaller, int iIndex)
{
	object_id_type iID = 0;
	LC_NPCSummorManager* mgr = LC_NPCSummorManager::GetSingletonPtr();
	if (mgr && pkScriptCaller)
	{
		LC_NPCSummorElement* pkElem = mgr->GetNPCSummorInfo(pkScriptCaller->GetCharType());
		if (pkElem)
		{
			if (iIndex >= 0 && iIndex < (int)pkElem->GetSummorCharTypeIDVec().size())
			{
				iID = pkElem->GetSummorCharTypeIDVec().at(iIndex);
			}
		}
	}
	return iID;
}

int	SC_ServerSkillInterface::GetEscortStealCount(GameLogic::LC_ActorBase* pkScriptCaller)
{
	return 0;
}

void SC_ServerSkillInterface::SetNPCAttacker(GameLogic::LC_ActorBase* pkScriptCaller, int attackerID)
{
	if (pkScriptCaller && IS_NPC_ID(pkScriptCaller->GetID()))
	{
		pkScriptCaller->SetAttackerID(attackerID);
	}
}

void SC_ServerSkillInterface::CopyHitedInfo(GameLogic::LC_Attacker* pkScriptCaller, GameLogic::LC_Attacker* dstActor)
{
	if (pkScriptCaller && dstActor && IS_NPC_ID(pkScriptCaller->GetID()) && IS_NPC_ID(dstActor->GetID()))
	{
		LC_ServerNPC* pkSrcNPC = (LC_ServerNPC*)pkScriptCaller;
		LC_ServerNPC* pkDstNPC = (LC_ServerNPC*)dstActor;

		LC_TargetMap* pkTargetMapSrc = pkSrcNPC->GetTargetMap();
		LC_TargetMap* pkTargetMapDst = pkDstNPC->GetTargetMap();

		LC_TargetsHitStateMap& hitMapSrc = pkTargetMapSrc->GetCurrentTargetsHitState();
		LC_TargetsHitStateMap& hitmapDst = pkTargetMapDst->GetCurrentTargetsHitState();

		hitmapDst.clear();
		for (LC_TargetsHitStateMap::iterator itr = hitMapSrc.begin() ; itr != hitMapSrc.end(); itr++)
		{
			hitmapDst[itr->first] = itr->second;
		}
	}
}

void SC_ServerSkillInterface::ClearHitedInfo(GameLogic::LC_ActorBase* pkScriptCaller)
{
	if (pkScriptCaller && IS_NPC_ID(pkScriptCaller->GetID()))
	{
		LC_TargetMap* pkTargetMap = pkScriptCaller->GetTargetMap();
		pkTargetMap->ClearHitState();
	}
}

void SC_ServerSkillInterface::CreatePortal(GameLogic::LC_ActorBase* pkScriptCaller, int iTargetIndex, int iMapResID, int doorCharTypeID, int transferID)
{
	if (pkScriptCaller)
	{
		LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
		LC_ServerMapLogic_Place* pkPlace = pkWorldManager->GetMapLogic(pkScriptCaller->GetMapLogicID());
		if (pkPlace)
		{
			UT_Vec3Int kTargetLocation;
			if (iTargetIndex == TARGET_LOCATION)
			{
				kTargetLocation = pkScriptCaller->GetLockedLocation();
			}
			else
			{
				LC_ActorPtrList kTargetsList;
				_findTarget((LC_Attacker*)pkScriptCaller, iTargetIndex, kTargetsList);
				if (kTargetsList.empty())
					return;
				LC_ActorBase* pkTarget = kTargetsList.front();
				if (!pkTarget)
					return;
				kTargetLocation = pkTarget->GetCurrentLocation();
			}
			pkPlace->BroadCastGameEventToClient(EVENT_TYPE_GENPORTAL, doorCharTypeID, iMapResID, kTargetLocation.x, kTargetLocation.y, kTargetLocation.z, transferID);
		}
	}
}

void SC_ServerSkillInterface::CreatePortalXYZ(GameLogic::LC_ActorBase* pkScriptCaller, int x, int y, int z, int iMapResID, int doorCharTypeID, int transferID)
{
	if (pkScriptCaller)
	{
		LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
		LC_ServerMapLogic_Place* pkPlace = pkWorldManager->GetMapLogic(pkScriptCaller->GetMapLogicID());
		if (pkPlace)
		{
			pkPlace->BroadCastGameEventToClient(EVENT_TYPE_GENPORTAL, doorCharTypeID, iMapResID, x, y, z, transferID);
		}
	}
}

int SC_ServerSkillInterface::GetWorldLevel(int32_t iCitizenship)
{
	GameServerManager* pkServerMgr = GetGameServerApp()->GetServerManager();
	Protocol::PS_ServerSimInfo* pkActive = pkServerMgr->GetActive(iCitizenship);
	if(pkActive == NULL)
	{
		return 0;
	}
	if (pkActive->m_iServerID==pkServerMgr->GetCurServerId())
	{
		return 0;
	}
	else
	{
		return pkServerMgr->GetCrossServerLevel(pkActive->m_iServerID);
	}
}

int SC_ServerSkillInterface::GetGroupWorldLevel()
{
	GameServerManager* pkServerMgr = GetGameServerApp()->GetServerManager();
	return pkServerMgr->GetGroupServerTopLevel();
}

void SC_ServerSkillInterface::SetDecompositionItem(GameLogic::LC_ActorBase* pkScriptCaller, bool flag)
{
	if (pkScriptCaller && IS_PLAYER_CLASS_ID(pkScriptCaller->GetID()))
	{
		LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)pkScriptCaller;
		pkServerPlayer = pkServerPlayer->GetOwnerPlayer();
		if(NULL == pkServerPlayer)
		{
			return;
		}
	
		if(flag)
		{
			pkServerPlayer->CheckDecompositionItem();
		}
		else
		{
			//关闭自动分解特权的时候，将玩家的自动分解状态设置为false
			pkServerPlayer->SetAutoDecomposition(false);
			pkServerPlayer->SyncDecompositionSetting();
		}
	}
}

void SC_ServerSkillInterface::VipEnjoy(GameLogic::LC_ActorBase* pkScriptCaller, int kType, bool flag)
{
	if (pkScriptCaller && IS_PLAYER_CLASS_ID(pkScriptCaller->GetID()))
	{
		LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)pkScriptCaller;
		pkServerPlayer = pkServerPlayer->GetOwnerPlayer();
		if(NULL != pkServerPlayer)
		{
			LC_VIPAsset& vipAsset = pkServerPlayer->GetVIPAsset();
			vipAsset.SetFlag(kType, flag);
		}
	}
}

int64_t SC_ServerSkillInterface::RecalculateCombatScore(GameLogic::LC_ActorBase* pkScriptCaller, int kType,int64_t oldScore, bool flag)
{
	if (pkScriptCaller && IS_PLAYER_CLASS_ID(pkScriptCaller->GetID()))
	{
		LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)pkScriptCaller;
	
		pkServerPlayer = pkServerPlayer->GetOwnerPlayer();
		if(NULL == pkServerPlayer)
			return -1;
	}
	return 0;
}

void SC_ServerSkillInterface::UseItemAddlevel(GameLogic::LC_ActorBase* pkScriptCaller, int ilevel)
{
	if (pkScriptCaller == NULL || !IS_PLAYER_ID(pkScriptCaller->GetID()))
	{
		return;
	}
	LC_ServerPlayer* pkPlayer = static_cast<LC_ServerPlayer*>(pkScriptCaller);
	{
		int lCurLevelValue = pkPlayer->GetLevel();
		int lLevel = lCurLevelValue + ilevel;
		static CF_LevelUp* pkCSVLevelUp = CF_LevelUp::GetSingletonPtr();
		if (lLevel < pkCSVLevelUp->GetCurrentMaxID())
		{
			pkPlayer->SetLevel(lLevel);
			oss_role_lvup(pkPlayer, SYS_FUNC_TP_GM);
			oss_360_log_user_upgrade(pkPlayer, SYS_FUNC_TP_GM);
		}
	}
}

int SC_ServerSkillInterface::CheckUseItemOpenSkillBook(GameLogic::LC_ActorBase* pkScriptCaller, int32_t iID)
{
	if (pkScriptCaller == NULL || !IS_PLAYER_CLASS_ID(pkScriptCaller->GetID()))
	{
		return RE_FAIL;
	}
	LC_ServerPlayer* pkPlayer = static_cast<LC_ServerPlayer*>(pkScriptCaller);
	return pkPlayer->GetSkillBookAsset().CheckOpenSkillBook(iID);
}

int SC_ServerSkillInterface::UseItemOpenSkillBook(GameLogic::LC_ActorBase* pkScriptCaller, int32_t iID)
{
	if (pkScriptCaller == NULL || !IS_PLAYER_CLASS_ID(pkScriptCaller->GetID()))
	{
		return RE_FAIL;
	}

	LC_ServerPlayer* pkPlayer = static_cast<LC_ServerPlayer*>(pkScriptCaller);
	if(NULL == pkPlayer)
	{
		return RE_FAIL;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if(NULL == pkPlayer)
		return RE_FAIL;

	ResultType res = pkPlayer->GetSkillBookAsset().OpenSkillBook(iID);
	if(RE_SUCCESS == res)
	{
		pkPlayer->SyncOpenNewSkillBook(iID);
		pkPlayer->SyncSkillBookInfo();
	}

	return res;
}

int SC_ServerSkillInterface::UseItemAddSubordinateAttr(GameLogic::LC_ActorBase* pkScriptCaller, int32_t iSubType, int32_t iAttributeType, int32_t iVal, int32_t iPercent)
{
	if (pkScriptCaller == NULL || pkScriptCaller->GetType() != LOT_PLAYER)
		return RE_FAIL;
	LC_ServerPlayer* pkPlayer = static_cast<LC_ServerPlayer*>(pkScriptCaller);
	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (NULL == pkPlayer)
		return RE_FAIL;
	LC_SubordinateAsset& kSubordinateAsset = pkPlayer->GetSubordinateAsset();
	int ret = kSubordinateAsset.AddAttributePlus(iSubType, iAttributeType, iVal, iPercent);
	if( RE_SUCCESS == ret )
	{
		int32_t reasonType = SPART_None;
		switch(iSubType)
		{
		case LC_SUB_TP_VEHICLE:
			reasonType = SPART_Sub_Vehicle_2;
			break;
		case LC_SUB_TP_DEVA_EYE:
			reasonType = SPART_Sub_Devaeye_2;
			break;
		case LC_SUB_TP_MUHUN:
			reasonType = SPART_Sub_Wing_2;
			break;
		}
		int nLogCode = 0;
		vector<uint64_t> nParams;
		nParams.clear();
		nParams.push_back(iAttributeType);
		nParams.push_back(iVal);
		switch(iSubType)
		{
		case LC_SUB_TP_VEHICLE:
			nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, MOUNTS_OPERATE, OPERATE_LOGTYPE3);
			break;
		case LC_SUB_TP_DEVA_EYE:
			nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, PETS_OPERATE, OPERATE_LOGTYPE3);
			break;
		case LC_SUB_TP_MUHUN:
			nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, BEISHI_OPERATE, OPERATE_LOGTYPE3);
			break;
		}
		RecordOperateLog(pkPlayer,BASE_OPERATE,nLogCode,nParams);

		pkPlayer->GetCommanderAttrMgr().RefreshAllSubAttributeMap(pkPlayer, false, reasonType, iSubType);
	}
	return ret;
}

int SC_ServerSkillInterface::UseItemCommitAddSubordinateAttr(GameLogic::LC_ActorBase* pkScriptCaller, int32_t iSubType, int32_t iValCount, int32_t iPerCount)
{
	if (pkScriptCaller == NULL || !IS_PLAYER_ID(pkScriptCaller->GetID()))
		return RE_FAIL;
	LC_PlayerBase* pkPlayer = static_cast<LC_PlayerBase*>(pkScriptCaller);
	LC_SubordinateAsset& kSubordinateAsset = pkPlayer->GetSubordinateAsset();
	return kSubordinateAsset.CommitAddAttributePlus(iSubType, (uint16_t)iValCount, (uint16_t)iPerCount);
}

int SC_ServerSkillInterface::UseItemAddSubordinateBlessPoint(GameLogic::LC_ActorBase* pkScriptCaller, int32_t iSubType, int32_t iVal, int32_t iPercent)
{
	if (pkScriptCaller == NULL || !IS_PLAYER_ID(pkScriptCaller->GetID()))
		return RE_FAIL;
	LC_PlayerBase* pkPlayer = static_cast<LC_PlayerBase*>(pkScriptCaller);
	LC_SubordinateAsset& kSubordinateAsset = pkPlayer->GetSubordinateAsset();
	LC_SubUpgradeData* pkSubData = kSubordinateAsset.GetSubData(iSubType);
	if (NULL == pkSubData)
		return RE_FAIL;
	const LC_SubordinateDataEntry* entry = SafeGetSubDataEntry(iSubType, pkSubData->GetStage());
	if (NULL == entry)
		return RE_FAIL;

	uint32_t timestamp = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	kSubordinateAsset.UpdateDirtyFlag();
	return RE_SUCCESS;
}

int SC_ServerSkillInterface::CheckUseItemOpenSubordinate(GameLogic::LC_ActorBase* pkScriptCaller, int32_t iSubType, int32_t iLevel)
{
	if (NULL == pkScriptCaller || !IS_PLAYER_ID(pkScriptCaller->GetID()))
		return RE_FAIL;
	LC_PlayerBase* pkPlayer = static_cast<LC_PlayerBase*>(pkScriptCaller);
	LC_SubordinateAsset& kSubordinateAsset = pkPlayer->GetSubordinateAsset();
	if (iLevel <= kSubordinateAsset.GetSubStage(iSubType))
		return RE_SUB_SET_LEVEL_FAIL;
	return RE_SUCCESS;
}

int SC_ServerSkillInterface::UseItemOpenSubordinate(GameLogic::LC_ActorBase* pkScriptCaller, int32_t iSubType, int32_t iLevel)
{
	if (NULL == pkScriptCaller || !IS_PLAYER_ID(pkScriptCaller->GetID()))
		return RE_FAIL;
	LC_ServerPlayer* pkPlayer = static_cast<LC_ServerPlayer*>(pkScriptCaller);
	LC_SubordinateAsset& kSubordinateAsset = pkPlayer->GetSubordinateAsset();
	int32_t iPreLevel = kSubordinateAsset.GetSubStage(iSubType);
	kSubordinateAsset.GMSetLevel(iSubType, iLevel);
	int32_t iCurLevel = kSubordinateAsset.GetSubStage(iSubType);
	if (iPreLevel < iCurLevel)
	{
		const LC_SubordinateDataEntry* entry = SafeGetSubDataEntry(iSubType, iCurLevel);
		oss_role_vehicle_up(pkPlayer, LC_PAY_WITH_GM, iSubType, iPreLevel, iCurLevel, 0,
			NULL, RE_UPGRAD_SUB_SUCCESS, 0, 0, entry ? entry->PeekASkill() : 0);
		if (LC_SUB_TP_VEHICLE == iSubType)
		{
			pkPlayer->OnVehicleUpgrade(iPreLevel);
		}
	}
	return RE_SUCCESS;
}

int SC_ServerSkillInterface::UseItemAddSubLevel(GameLogic::LC_ActorBase* pkScriptCaller, int32_t iItemID, int32_t iSubType, int32_t iLevelLimit, int32_t iLevel, const StringType& sRewards)
{
	if (NULL == pkScriptCaller || !IS_PLAYER_ID(pkScriptCaller->GetID()))
		return RE_FAIL;

	LC_ServerPlayer* pkPlayer = static_cast<LC_ServerPlayer*>(pkScriptCaller);
	LC_SubordinateAsset& kSubordinateAsset = pkPlayer->GetSubordinateAsset();
	LC_SubUpgradeData* pkSub = kSubordinateAsset.GetSubData(iSubType);
	if (NULL == pkSub)
	{
		return RE_FAIL;
	}
	int32_t iPreStage= pkSub->GetStage();
	if (!pkSub->IsEnabled() || iPreStage <= 0)
	{
		return RE_FAIL;
	}
	if (iPreStage < iLevelLimit)
	{
		pkPlayer->UpgradeSubStage(iSubType, iPreStage+1);
		if (iPreStage < pkSub->GetStage())
		{
			const LC_SubordinateDataEntry* entry = SafeGetSubDataEntry(iSubType, pkSub->GetStage());
			oss_role_vehicle_up(pkPlayer, LC_PAY_WITH_GM, iSubType, iPreStage, pkSub->GetStage(), 0,
				NULL, RE_UPGRAD_SUB_SUCCESS, 0, 0, entry ? entry->PeekASkill() : 0);
			if (LC_SUB_TP_VEHICLE == iSubType)
			{
				pkPlayer->OnVehicleUpgrade(iPreStage);
			}
		}
	}
	else
	{
		UT_SIMDataList kRewardItems;
		Utility::ParseFmtString2DataIFs(sRewards, kRewardItems);
		if (!kRewardItems.empty())
		{
			UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, kRewardItems, pkPlayer, SYS_FUNC_TP_ITEM_USE, iItemID, "132;", "133;", true);
		}
	}
	return RE_SUCCESS;
}

int SC_ServerSkillInterface::UseItemAddSubLevelTo(GameLogic::LC_ActorBase* pkScriptCaller, int32_t iItemID, int32_t iSubType, int32_t iLevelLimit, const StringType& sRewards)
{
	if (NULL == pkScriptCaller || !IS_PLAYER_ID(pkScriptCaller->GetID()))
	{
		return RE_FAIL;
	}
	LC_ServerPlayer* pkPlayer = static_cast<LC_ServerPlayer*>(pkScriptCaller);
	LC_SubordinateAsset& kSubordinateAsset = pkPlayer->GetSubordinateAsset();
	LC_SubUpgradeData* pkSub = kSubordinateAsset.GetSubData(iSubType);
	if (NULL == pkSub)
	{
		return RE_FAIL;
	}
	int32_t iPreStage = pkSub->GetStage();
	if (!pkSub->IsEnabled() || iPreStage <= 0)
	{
		return RE_FAIL;
	}
	if (iPreStage < iLevelLimit)
	{
		pkPlayer->UpgradeSubStage(iSubType, iLevelLimit);
		if (iPreStage < pkSub->GetStage())
		{
			const LC_SubordinateDataEntry* entry = SafeGetSubDataEntry(iSubType, pkSub->GetStage());
			oss_role_vehicle_up(pkPlayer, LC_PAY_WITH_GM, iSubType, iPreStage, pkSub->GetStage(), 0,
				NULL, RE_UPGRAD_SUB_SUCCESS, 0, 0, entry ? entry->PeekASkill() : 0);
			if (LC_SUB_TP_VEHICLE == iSubType)
			{
				pkPlayer->OnVehicleUpgrade(iPreStage);
			}
		}
	}
	else
	{
		UT_SIMDataList kRewardItems;
		Utility::ParseFmtString2DataIFs(sRewards, kRewardItems);
		if (!kRewardItems.empty())
		{
			UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, kRewardItems, pkPlayer, SYS_FUNC_TP_ITEM_USE, iItemID, "132;", "133;", true);
		}
	}
	return RE_SUCCESS;
}

void SC_ServerSkillInterface::UseItemAddActScore(GameLogic::LC_ActorBase* pkScriptCaller, int32_t iType, int32_t iVal, const StringType sCatagory)
{
	if (NULL == pkScriptCaller || !IS_PLAYER_ID(pkScriptCaller->GetID()))
	{
		return;
	}
	LC_ServerPlayer* pkPlayer = static_cast<LC_ServerPlayer*>(pkScriptCaller);
	mem::vector<StringType> strList;	
	mem::vector<int32_t> kCatagorys;
	Utility::tokenize(sCatagory, strList, "|");
	size_t nSize = strList.size();
	for (size_t i = 0; i < nSize; ++i)
	{
		int32_t iCatagory = 0;
		__Get_Token_Value(iCatagory, strList, nSize, i, int32_t, atoi);
		kCatagorys.push_back(iCatagory);
	}
	
	static Activity::ACT_ScheduleDataManager* pkACTDataMgr = Activity::ACT_ScheduleDataManager::GetSingletonPtr();
	pkACTDataMgr->AddScore(pkPlayer, iType, iVal, kCatagorys);
}

void SC_ServerSkillInterface::UseItemAddSnowManScore(GameLogic::LC_ActorBase* pkScriptCaller, int32_t iScore, int32_t iAchieve, const StringType sCatagory)
{
	if (NULL == pkScriptCaller || !IS_PLAYER_ID(pkScriptCaller->GetID()))
	{
		return;
	}
	LC_ServerPlayer* pkPlayer = static_cast<LC_ServerPlayer*>(pkScriptCaller);

	mem::vector<StringType> strList;
	mem::vector<int32_t> kCatagorys;
	Utility::tokenize(sCatagory, strList, "|");
	size_t nSize = strList.size();
	for (size_t i = 0; i < nSize; ++i)
	{
		int32_t iCatagory = 0;
		__Get_Token_Value(iCatagory, strList, nSize, i, int32_t, atoi);
		kCatagorys.push_back(iCatagory);
	}

	static Activity::ACT_ScheduleDataManager* pkACTDataMgr = Activity::ACT_ScheduleDataManager::GetSingletonPtr();
	Activity::ACT_SnowManManager* pkActivity = (Activity::ACT_SnowManManager*)pkACTDataMgr->GetActivity(Activity::ACT_SCHEDULE_TP_SNOW_MAN);
	if (pkActivity)
	{
		pkActivity->AddScore(pkPlayer, iScore, iAchieve, kCatagorys);
	}		
}

ResultType SC_ServerSkillInterface::AddGemReinforceExp(GameLogic::LC_ActorBase* pkScriptCaller, int32_t iType, int32_t iExp)
{
	if (NULL == pkScriptCaller)
	{
		return RE_FAIL;
	}
	LC_ServerPlayer* pkPlayer = static_cast<LC_ServerPlayer*>(pkScriptCaller);
	LC_GemReinforceData* pkGemReinData = pkPlayer->GetGemReinforceAsset().GetGemReinforceData((EquipType)iType);
	if (NULL == pkGemReinData)
	{
		return RE_FAIL;
	}
	return pkGemReinData->AddExp(pkPlayer, iExp);
}

ResultType SC_ServerSkillInterface::DoConsumeItems(GameLogic::LC_PlayerBase* pkPlayer, const StringType& items, int32_t reason)
{
	if (NULL == pkPlayer)
	{
		return RE_FAIL;
	}
	if (!items.empty())
	{
		Utility::UT_SIMDataList kItems;
		Utility::ParseFmtString2DataIFs(items, kItems);

		PackOperateInfoList kConsumeOPs;
		LC_Helper::BuildConsumeItemOPs(kItems, kConsumeOPs);
		return LC_Helper::DoItemOPs(pkPlayer, kConsumeOPs, reason, 0);
	}
	return RE_SUCCESS;
}

ResultType SC_ServerSkillInterface::DoPayShops(GameLogic::LC_PlayerBase* pkPlayer, const StringType& shops, int32_t srctype, int32_t srcid)
{
	if (NULL == pkPlayer)
	{
		return RE_FAIL;
	}

	if (!shops.empty())
	{
		Utility::UT_SIMDataList kShops;
		Utility::ParseFmtString2DataIFs(shops, kShops);

		LC_ShopIDMap kShopIDs;
		Utility::UT_SIMDataList::iterator it = kShops.begin();
		for (; it != kShops.end(); ++it)
		{
			kShopIDs[it->ID()] += it->ShopCount();
		}
		return LC_Helper::PayForShop(pkPlayer, kShopIDs, srctype, srcid);
	}
	return RE_SUCCESS;
}

void SC_ServerSkillInterface::SetMapLockEnter(int32_t iMapLogicID, int32_t iResID, bool flag)
{
	LC_ServerWorldManager* pkWorldMgr = SERVER_GET_WORLD_MANAGER();
	if (iMapLogicID >= MAP_LOGIC_ID_BASE)
	{
		LC_ServerMapLogic_Place* pkPlace = pkWorldMgr->GetMapLogic(iMapLogicID);
		if (pkPlace && pkPlace->GetMapResID() == iResID)
		{
			pkPlace->SetEnterLocked(flag);
		}
	}
	else
	{
		LC_ServerMapLogicPlaceVector* pkPlaces = pkWorldMgr->GetMapLogicByRes(iResID);
		if (pkPlaces)
		{
			LC_ServerMapLogicPlaceVector::iterator it = pkPlaces->begin();
			for (; it != pkPlaces->end(); ++it)
			{
				if (*it)
				{
					(*it)->SetEnterLocked(flag);
				}
			}
		}
	}
}
void SC_ServerSkillInterface::SetWatchTowerState(int storyID, int32_t towerMapID, int32_t bossMapID, int towerID, int bossID, int state, int vaildTime, int endTimerID)
{
	LC_ServerWorldManager* pkWorldMgr = SERVER_GET_WORLD_MANAGER();
	if(towerMapID != GetGlobalSetting.BestGuildMapID)
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "SC_ServerSkillInterface::SetWatchTowerState serverid:%d, storyID:%d, towerMapID:%d, bossMapID:%d, towerID:%d, bossID:%d, state:%d return 1\n", GetGameServerID(), storyID, towerMapID, bossMapID, towerID, bossID, state);
		return;
	}

	LC_ServerMapLogicPlaceVector* pkPlaces = pkWorldMgr->GetMapLogicByRes(towerMapID);
	if (NULL == pkPlaces)
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "SC_ServerSkillInterface::SetWatchTowerState serverid:%d, storyID:%d, towerMapID:%d, bossMapID:%d, towerID:%d, bossID:%d, state:%d return 2\n", GetGameServerID(), storyID, towerMapID, bossMapID, towerID, bossID, state);
		return;
	}

	LC_ServerMapLogicPlaceVector::iterator it = pkPlaces->begin();
	LC_ServerMapLogic_Place* pkPlace = *it;
	if(NULL != pkPlace)
	{
		pkPlace->SetWatchTowerState(storyID, bossMapID, towerID, bossID, state, vaildTime, endTimerID);
	}
}

void SC_ServerSkillInterface::SetTowerLocation(int storyID, int32_t mapID, int32_t towerID, int x, int y, int z)
{
	LC_ServerWorldManager* pkWorldMgr = SERVER_GET_WORLD_MANAGER();
	if(mapID != GetGlobalSetting.BestGuildMapID)
	{
		return;
	}

	LC_ServerMapLogicPlaceVector* pkPlaces = pkWorldMgr->GetMapLogicByRes(mapID);
	if (NULL == pkPlaces)
	{
		return;
	}

	LC_ServerMapLogicPlaceVector::iterator it = pkPlaces->begin();
	LC_ServerMapLogic_Place* pkPlace = *it;
	if(NULL != pkPlace)
	{
		pkPlace->SetTowerLocation(storyID, towerID, x, y, z);
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------
bool SC_ServerSkillInterface::CurrentMapIsRecorderPkValue(GameLogic::LC_Attacker* pkCaller)
{
	if (pkCaller)
	{
		LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
		LC_ServerMapLogic_Place* themaplogic = pkWorldManager->GetMapLogic(pkCaller->GetMapLogicID());
		if (themaplogic == NULL)
		{
			return false;
		}
		return themaplogic->GetRecordPK();
	}
	return false;
}
//-------------------------------------------------------------------------------------
void SC_ServerSkillInterface::AddCouragePoint(GameLogic::LC_ActorBase* pkCaller, attr_value_type iPoint)
{
	if ( pkCaller && IS_PLAYER_ID(pkCaller->GetID()) )
	{
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkCaller;
		pkPlayer->AddCouragePoint(iPoint, SYS_FUNC_TP_GM, 0);
	}
}
//-------------------------------------------------------------------------------------
void SC_ServerSkillInterface::AddBreakRewardItem(GameLogic::LC_ActorBase* pkCaller, int iItemID, int iStateID, float per, int iMax, int dis)
{
	LC_EncountedStatMgr& kStatMgr = pkCaller->GetEncountedStatMgr();
	LC_EncountedStat* pkStat = kStatMgr.GetStat(LC_ECT_TP_Damage);
	if (NULL == pkStat)
	{
		return;
	}

	LC_EncountedStatContainer* pkStatCon = pkStat->GetEncountedStat(LC_ECT_CAT_ACTOR);
	if (NULL == pkStatCon)
	{
		return ;
	}
	const uint64_object_id_vector& rkList = pkStatCon->GetSortedIDs();
	object_id_type theID = 0;
	uint64_object_id_vector::const_iterator itID = rkList.begin();
	LC_WorldManagerBase* pkWorldManager = ENGINE_GET_WORLD_MANAGER();
	if ( NULL == pkWorldManager )
	{
		return ;
	}

	int iCount = 0;
	int64_t maxDis = dis*dis;
	for (; itID != rkList.end(); ++itID)
	{
		theID = (int32_t)*itID;
		if ( IS_PLAYER_CLASS_ID( theID ) && UT_MathBase::RandFloat01() < per && iCount <= iMax)
		{
			LC_PlayerBase* pkObj = (LC_PlayerBase*)pkWorldManager->FindObject(theID);
			if ( pkObj )
			{
				if ( UT_MathBase::LinePowXYInt(pkObj->GetCurrentLocation(), pkCaller->GetCurrentLocation()) < maxDis )
				{
					Utility::UT_SIMDataList kRewardItems;
					Utility::UT_SIMDataInfo kItem;
					kItem.SetItemInfo(iItemID, 1, TRUE);
					kRewardItems.push_back(kItem);
					UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, kRewardItems,(LC_PlayerBase*)pkObj, SYS_FUNC_TP_STATE, iStateID, "200;", "200;", true);
					iCount++;
				}
			}
		}
		
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------
double SC_ServerSkillInterface::GetBossShieldPercent()
{
	GameServerApp* pkApp = GetGameServerApp();
	return pkApp->GetBossShieldRate();
}
//--------------------------------------------------------------------------------------------------------------------------------------------
double SC_ServerSkillInterface::GetCrossBossShieldPercent()
{
	GameServerApp* pkApp = GetGameServerApp();
	return pkApp->GetCrossBossShieldRate();
}
//--------------------------------------------------------------------------------------------------------------------------------------------
void SC_ServerSkillInterface::SendKingFlagGainInfo(int nServerID, uint64_t nTime)
{
	LC_ServerAllianceManager* pAllianceManager = LC_ServerAllianceManager::GetSingletonPtr();
	if(pAllianceManager != NULL)
	{
		pAllianceManager->SendKingFlagGainInfo(nServerID, nTime);
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------
void SC_ServerSkillInterface::SendFactionResult(int32_t iStatus)
{
	LC_ServerAllianceManager* pAllianceManager = LC_ServerAllianceManager::GetSingletonPtr();
	if (pAllianceManager != NULL)
	{
		uint64_t iTime = (uint64_t)time(NULL);
		pAllianceManager->SendFactionResult(iTime, iStatus);
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------
void SC_ServerSkillInterface::ActivateFashion(GameLogic::LC_Attacker* pkScriptCaller, int lID)
{
	if (pkScriptCaller == NULL || !IS_PLAYER_ID(pkScriptCaller->GetID()))
	{
		return;
	}
	LC_ServerPlayer* pkPlayer = static_cast<LC_ServerPlayer*>(pkScriptCaller);
	LC_FashionBagAsset& pkFashionBag = pkPlayer->GetFashionBagAsset();

	ResultType ret = pkFashionBag.ActivateFashion((uint32_t)lID);
	if ( ret == RE_SUCCESS )
	{
		pkPlayer->GetMessageDirtyFlag().GetBroadCastFlag().SetEquip();
	}
}
//--------------------------------------------------------------------------------
void SC_ServerSkillInterface::SetShieldBreak(GameLogic::LC_ActorBase* pkCaller)
{
	if ( pkCaller && IS_NPC_ID(pkCaller->GetID()) )
	{
		LC_ServerNPC* pkNPC = (LC_ServerNPC*)pkCaller;
		if ( pkNPC->GetSkillTrigger()->GetLastShieldIndex() < MAX_ACTOR_SKILL_TRIGGER_COUNT && pkNPC->GetSkillTrigger()->GetLastShieldIndex() > -1 )
		{
			LC_SkillTriggerEntry* et = pkNPC->GetSkillTrigger()->GetTriggerByIndex(pkNPC->GetSkillTrigger()->GetLastShieldIndex());
			if ( et )
				et->SetValid(false);
			pkNPC->GetSkillTrigger()->SetLastShieldIndex(-1);
		}
	}
}

void SC_ServerSkillInterface::DelaySkillEvent( GameLogic::LC_ActorBase* pkCaller, int skillID, int duration, int startTime, int perTime)
{
	if ( NULL == pkCaller )
		return ;

	if ( startTime < GET_PROCESS_TIME )
		return ;

	SK_ServerSkillEventManager* pkEventManager = SERVER_GET_SKILL_EVENT_MANAGER();
	SK_SkillEvent* pkSkillEvent = pkEventManager->AllocEvent();
	if (NULL != pkSkillEvent)
	{
		pkSkillEvent->SetEventType(SETE_SUMMOR_DAMAGE);
		pkSkillEvent->SetSrcActorID(pkCaller->GetID());
		pkSkillEvent->SetDstActorID(0);
		pkSkillEvent->SetLogicID(skillID);
		pkSkillEvent->SetParam(SK_SummorDamage::SEP_SKILL_ID, skillID);
		pkSkillEvent->SetParam(SK_SummorDamage::SEP_DURATION, duration);
		pkSkillEvent->SetParam(SK_SummorDamage::SEP_START_TIME, startTime);
		pkSkillEvent->SetParam(SK_SummorDamage::SEP_TICK_TIME, perTime);
		pkSkillEvent->SetParam(SK_SummorDamage::SEP_TICK_COUNT,0);
		pkCaller->SetDelayLocation(pkCaller->GetCurrentLocation());
		pkEventManager->AddEvent(pkSkillEvent);	

		pkEventManager->SendToClient(pkCaller, pkSkillEvent, Skill::SSET_BROADCAST_TO_AREA);
	}
}

int32_t SC_ServerSkillInterface::ChooseFactionByGlobalPower(LC_ActorBase* pkCaller)
{
	if (!IS_PLAYER_ID(pkCaller->GetID()) )
	{
		return FACTION_PLAYER;
	}
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkCaller;
	if (NULL == pkPlayer)
	{
		return FACTION_PLAYER;
	}
	int32_t iDefServerId = UT_ServerHelper::GetFactionDefServer();
	if (INVALID_SERVER_ID == iDefServerId)
	{
		return FACTION_PLAYER;
	}
	if (GetGameServerApp()->IsSameServer(iDefServerId, pkPlayer->GetCitizenship()))
	{
		return FACTION_DEFENDER;
	}
	else
	{
		return FACTION_ATTACKER;
	}
	return FACTION_PLAYER;
}

int32_t SC_ServerSkillInterface::GetFactionDefServer()
{
	return UT_ServerHelper::GetFactionDefServer();
}

int32_t SC_ServerSkillInterface::GetFactionFailBuffOverlap(GameLogic::LC_ActorBase* pkCaller)
{
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkCaller;
	if (NULL == pkPlayer)
	{
		return 0;
	}
	LC_ServerAllianceManager* pkAllianceMgr = LC_ServerAllianceManager::GetSingletonPtr();
	if (NULL == pkAllianceMgr)
	{
		return 0;
	}
	return pkAllianceMgr->GetFactionFailBuffOverlap(pkPlayer);
}

int32_t SC_ServerSkillInterface::AddRelateTimeBuff(GameLogic::LC_ActorBase* pkCaller, int32_t iStoryID, int32_t iBuffID)
{
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkCaller;
	if (NULL == pkPlayer)
	{
		return RE_FAIL;
	}

	GameStory::LC_GameStory_Manager* pkGSMgr = SERVER_GET_GAMESTORY_MANAGER;
	if (NULL == pkGSMgr)
	{
		return RE_FAIL;
	}

	time_t iCurTime = time(NULL);
	time_t iRelateTime = pkGSMgr->GetRelateTime(iStoryID);
	if (iRelateTime == 0 || iRelateTime < iCurTime)
	{
		return RE_FAIL;
	}
	time_t iBuffTime = iRelateTime - iCurTime;
	iBuffTime = (iBuffTime > ONE_WEEK_SECONDS) ? ONE_WEEK_SECONDS : iBuffTime;
	pkPlayer->ActiveSkillState(iBuffID, (int)iBuffTime, pkPlayer->GetID());
	return RE_SUCCESS;
}


void SC_ServerSkillInterface::AddGSFactionScore(GameLogic::LC_ActorBase* pkKiller, GameLogic::LC_ActorBase* pkInjured, int32_t iType, int32_t iVal)
{
	static GameStory::LC_StoryLogic_Manager* pkGSMgr = SERVER_GET_STORYLOGIC_MANAGER;
	pkGSMgr->AddFactionScore(pkKiller, pkInjured, pkKiller->GetMapLogicID(), iType, iVal);
}
void SC_ServerSkillInterface::AddGSActorScore(GameLogic::LC_ActorBase* pkKiller, GameLogic::LC_ActorBase* pkInjured, int32_t iVal)
{
	static GameStory::LC_StoryLogic_Manager* pkGSMgr = SERVER_GET_STORYLOGIC_MANAGER;
	pkGSMgr->AddActorScore(pkKiller, pkInjured, pkKiller->GetMapLogicID(), iVal);
}

// 是否要检查击退状态重入，也可以让策划去配表
static bool should_check_punchback_reenter(long lSkillID)
{
	static int s_blacklist[] = {22161};

	int  iSkillCategoty = lSkillID / 1000;
	for (int i = 0; i < sizeof(s_blacklist)/sizeof(s_blacklist[0]); ++i)
	{
		if (s_blacklist[i] == iSkillCategoty)
			return false;
	}
	return true;
}

void SC_ServerSkillInterface::_doDmagePunch( int64_t iFinalDamage, LC_ActorBase* pkTarget, CF_SkillExecuteList::DataEntry* pkSkillData, object_id_type lTargetID, LC_ActorBase* pkAttacker )
{
	//处理击退
	do
	{
		if (iFinalDamage <= 0)	//无伤害
		{
			break;
		}

		if (pkTarget->GetDead())	//死了，不击退
		{
			break;
		}
		//if (pkTarget->GetSkillFlag(SK_PUNCH_NA_FLAGS))	//目标身上是否有免疫击退标志
		//{
		//	break;
		//}
		if (pkTarget->GetSkillFlag(SK_IMM_PUNCH_BACK))	//目标身上是否有免疫击退标志
		{
			pkTarget->OnImmuneMove(SETE_IMM_PUNCH_MOVE,pkAttacker->GetID());
			break;
		}
		if (pkTarget->GetRiderCharType() > 0)			//目标骑马
		{
			break;
		}
		if (pkSkillData->_HitMoveType <= SMAPT_NONE)	//技能没有击退效果
		{
			break;
		}
		if (should_check_punchback_reenter(pkSkillData->_iID))	//击退状态重入检查
		{
		if (IS_NPC_ID(lTargetID) && (pkTarget->GetCurrentStateType() == ST_NPC_PUNCH_BACK || pkTarget->GetCurrentStateType() == ST_NPC_PASSIVE_MOVE))	//已经在击退状态
		{
			break;
		}
		if (IS_PLAYER_CLASS_ID(lTargetID) && (pkTarget->GetCurrentStateType() == ST_PLAYER_PUNCH_BACK || ST_PLAYER_PASSIVE_MOVE == pkTarget->GetCurrentStateType()))	//已经在击退状态
		{
			break;
		}
		}
		//判断击退位移和时间
		UT_Vec3Int kTargetPos;
		int lMoveDistance = pkSkillData->_lHitMoveRange;
		float fMoveTime = pkSkillData->_fHitMoveTime;
		if (pkSkillData->_HitMoveType == SMAPT_HITFLY)	//如果是击飞，则取NPC属性；如果是击退，则取技能属性
		{
			float u = UT_MathBase::RandFloat01();
			if (u > pkSkillData->_fHitMovePercent)		//如果击飞几率不满足，则取消击飞
			{
				break;
			}
			//lMoveDistance = pkTarget->GetProperty_PunchFlyDistance();
			//fMoveTime = pkTarget->GetProperty_PunchFlyTime();
		}

		if (lMoveDistance <= 0 || fMoveTime <= 0)		//击退位移和时间都满足的，才做击退
		{
			break;
		}
		//中断技能
		int lSkillID = pkTarget->GetCurrentStateType();
		if (IS_SKILL_TYPE(lSkillID))
		{
			pkTarget->InterruptSkill(GET_PROCESS_TIME, lSkillID, SKI_FORCE);	//这里会切换到默认状态
		}

		//开始击退
		PF_ServerPathManager* pfm = SafeGetSingleton<PF_ServerPathManager>();
		int MapResID = pkAttacker->GetMapResID();
		const UT_Vec3Int & kSrcLocation = pkTarget->GetCurrentLocation();
		UT_Vec3Int dis = kSrcLocation - pkAttacker->GetCurrentLocation();
		UT_Vec2Float dir((float)dis.x, (float)dis.y);
		if (dis.x == 0 && dis.y == 0)	// 如果两人重叠，就用攻击者的方向作为被击退方向
			dir = pkAttacker->GetForwardDir();
		dir.Unitize();
		UT_Vec3Int dst;
		dst.x = (int)(kSrcLocation.x + dir.x * lMoveDistance);
		dst.y = (int)(kSrcLocation.y + dir.y * lMoveDistance);
		dst.z = kSrcLocation.z;
		bool ret = pfm->FindDirectFarawayPoint(MapResID, kSrcLocation, dst, lMoveDistance, kTargetPos);
		if (ret)
		{
			pkTarget->ClearPendingSkill();
			pkTarget->SetLastTranslateTime(GET_PROCESS_TIME);	//这句话蛮关键的，NPC Update的时候会用GetLastTranslateTime计算deltaTime，这里为的是让deltaTime为0，就不会产生位移，也不会在当前帧发送同步消息下去了
			pkTarget->PunchMove((SkillMoveActorPositionType)pkSkillData->_HitMoveType, kTargetPos, fMoveTime);
			AttachSkillState(pkAttacker, pkTarget->GetID(), 39122101, 1, 0,0,0,0,0);
		}
	} while (0);
}

void SC_ServerSkillInterface::_generateSkillDamageMultiple( GameLogic::LC_Attacker* pkScriptCaller ,  int iDamageTargetIndex, int lSkillTypeID,  int iDamageRandomIndex , int iAttackRateRandomIndex, int iCriticalRateRandomIndex,  object_id_type lDamageSourceActorID, int iHPCost, int iMPCost, int Multiple, int iPercent, int bUseScore, bool bDoPunch /* =true */)
{
	if (NULL == pkScriptCaller)
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR," _generateSkillDamageMultiple error 0: pkScriptCaller is nullptr." );
		return;
	}
	bool bLog22003001 = false;
	if ( pkScriptCaller->GetCurrentStateType() / 10 == 2200300 )
		bLog22003001 = true;

	if ( bLog22003001 )
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"skill_22003001: _generateSkillDamageMultiple begin" );
	bool bNpc = false;
	if(IS_NPC_ID(pkScriptCaller->GetID()))
		bNpc = true;
	LC_ActorBase* pkAttacker = NULL;
	if (pkScriptCaller->GetID() == lDamageSourceActorID)
	{
		pkAttacker = (LC_ActorBase*)pkScriptCaller;
	}
	else
	{
		pkAttacker = _findActor(lDamageSourceActorID);
	}
	if (!pkAttacker)
	{
		GfxWriteLog(LOG_WARNING_FILELINE, LOG_TAG_WARNING, "Unable to find the source of damage[%i]", lDamageSourceActorID);

		if ( bLog22003001 )
			GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"skill_22003001: _generateSkillDamageMultiple error 1: InvalidAttacker: %d.", lDamageSourceActorID );

		return;
	}

	LC_TargetMap* pkTargetMap = pkAttacker->GetTargetMap();
	pkTargetMap->ClearHitState();

	LC_ActorPtrList kDamageTargetsList;
	_findTarget(pkScriptCaller, iDamageTargetIndex, kDamageTargetsList);
	//LC_Helper::_checkTargetValid()
	LC_ActorPtrList::iterator itDst = kDamageTargetsList.begin();
	uint64_t nSrcGuildId = pkAttacker->GetGuildID();
	for ( ; itDst != kDamageTargetsList.end(); )
	{
		bool tag = true;
		LC_ActorBase* pActorIt = *itDst;
		//源和目的都是人
		if(IS_PLAYER_CLASS_ID(pkAttacker->GetID()) && IS_PLAYER_CLASS_ID(pActorIt->GetID()))
		{
			//竞技场中 同公会的玩家可以互相攻击
			if(pkAttacker->GetMapLogicType()==MT_ARENA_SOLO || pActorIt->GetMapLogicType()==MT_ARENA_SOLO)
			{
				tag = false;
			}
			else
			{
				//不在竞技场中，攻击者是全体模式，可以攻击同公会的玩家
				PKMode pkSrcPKMode = ((LC_PlayerBase*)pkAttacker)->GetPKMode();
				if(pkSrcPKMode == PKM_ALL)
				{
					tag = false;
				}
			}
		}

		if(true == tag)
		{
			if ( nSrcGuildId != 0 && pActorIt->GetGuildID() == nSrcGuildId )
			{
				itDst = kDamageTargetsList.erase( itDst );
			}
			else
			{
				++itDst;
			}
		}		
		else
		{
			++itDst;
		}
	}
	if (kDamageTargetsList.size() == 0)
	{
		if ( bLog22003001 )
			GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"skill_22003001: _generateSkillDamageMultiple error 2: kDamageTargetsList is empty." );

		return;
	}
	//攻击者等级
	int iAttackerLevel =  pkAttacker->GetLevel();
	LC_ActorAttributeMap* pkScriptCallerAttributeMap = NULL;
	if(bNpc)
	{
		pkScriptCallerAttributeMap = pkAttacker->GetFinalAttributeMap();
		iAttackerLevel = pkAttacker->GetSummonOwnerLevel();
	}
	else
	{
		LC_PlayerBase* pkAttackerPlayer1 = (LC_PlayerBase*)pkAttacker;
		IF_A_NA_PLAYER( pkAttackerPlayer1)
		{
			if ( bLog22003001 )
				GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"skill_22003001: _generateSkillDamageMultiple error 3: IF_A_NA_PLAYER is invalid: %d.", pkAttackerPlayer1->IsValid() );
			return ;
		}
		LC_ServerPlayer* pkAttackerPlayer = (LC_ServerPlayer*)pkAttackerPlayer1;
		pkScriptCallerAttributeMap = pkAttackerPlayer->GetFinalAttributeMap();
	}
	if (NULL == pkScriptCallerAttributeMap)
	{
		if ( bLog22003001 )
			GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"skill_22003001: _generateSkillDamageMultiple error 4: pkScriptCallerAttributeMap is nullptr." );

		return;
	}
	//攻击者暴击率
	//attr_value_type iAttackerCritical = pkScriptCallerAttributeMap->GetAttributeValue(ATT_CIR_RATE);
	attr_value_type iAttackerCritical = pkScriptCallerAttributeMap->GetAttribute(ATT_CIR_RATE);
	attr_value_type iAttackerCriticalRate = pkScriptCallerAttributeMap->GetAttributePercent(ATT_CIR_RATE);
	//攻击者攻击
	attr_value_type iAttackerMaxPHA = pkScriptCallerAttributeMap->GetAttribute(ATT_MAX_ATK);
	//攻击者暴击伤害
	attr_value_type iAttackerCriticalDamage = 10000;
	

	CF_SkillExecuteList::DataEntry* pkSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(lSkillTypeID);
	if (!pkSkillData)
	{
		if ( bLog22003001 )
			GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"skill_22003001: _generateSkillDamageMultiple error 5: Invalid skill: %d.", lSkillTypeID );
		return;
	}

	// 宠物技能计算逻辑
	bool bLCSkillLogic = false;
	PetDamageMode ePetDmgMode = PET_DAMAGE_MODE_COUNT;
	if (pkSkillData->_lCategory2 == SKILL_TYPE_LC)
	{
		bLCSkillLogic = true;
		ePetDmgMode = static_cast<PetDamageMode>(pkSkillData->_lPetDamageMode);
	}

	CF_SkillSpecialAttrib::DataEntry* pkSkillAttrib = SafeGetCSVFileDataEntryPtr<CF_SkillSpecialAttrib>(lSkillTypeID);
	if (!pkSkillAttrib)
	{
		if ( bLog22003001 )
			GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"skill_22003001: _generateSkillDamageMultiple error 6: pkSkillAttrib not found: %d.", lSkillTypeID );

		return;
	}
	//技能暴击率附加
	double fSkillCriticalRate = pkSkillAttrib->_fCriticalRate;

	//技能内息消耗
	if (iMPCost <= 0)
	{
		int32_t iDefaultMPCost = LC_Helper::GetSkillDefaultMPCost(pkAttacker, pkSkillData);
		int32_t iAdditionMPCost = LC_Helper::GetSkillAdditionMPCost(pkAttacker, pkSkillData);
		iMPCost = iDefaultMPCost + iAdditionMPCost;
	}

	//技能伤害系数
	double fSkillDamageRate = pkSkillAttrib->_fDamageRate;
	//技能暴击加成
	//int   iSkillCriticalDamageAdd = pkSkillData->_lCriticalDamageAdd;

	//被攻击者躲避率
	attr_value_type	iTargetDog = 0;
	attr_value_type iTargetDogRate = 0;
	//被攻击者等级
	int64_t iTargetLevel = 0;
	//被攻击者防御
	attr_value_type iTargetPHD = 0;

	//最终伤害
	double   fFinalDamage = 0.0;

	LC_ActorBase* pkTarget = NULL;
	LC_ActorAttributeMap* pkTargetAttributeMap = NULL;
	object_id_type		  lTargetID;
	LC_ActorPtrListIter Iter;
	bool  bCirtical;
	attr_value_type	  iTargetDefence;
	//float fTargetHPCostRate;

	//伤害力骰子
	double fDamageRandom = 0;
	//命中骰子
	double fAttackRateRandom = 0;
	//暴击骰子
	double fCriticalRateRandom = 0;
	//计算伤害力
	int64_t iAttackerDamage = 0;
	bool bNeedCalcHitCount = false;

	//伤害类型
	int lSkillDamageType = pkSkillData->_lDamageType;
	switch (lSkillDamageType)
	{
	case SDT_NORMAL:
	case SDT_SKILL: //外攻
		iAttackerDamage = iAttackerMaxPHA;
		bNeedCalcHitCount = true;
		break;
	case SDT_LC_SKILL:
		iAttackerDamage = GetSubordinateAttribute(pkAttacker, ATT_MAX_ATK , LC_SUB_TP_SPIRIT_PET);
		bNeedCalcHitCount = true;
		break;
	default:
		{
			GfxWriteLog(LOG_TAG_SKILL, LOG_SWITCH_ERROR, "skill[%i]injury type error", lSkillTypeID);
			if ( bLog22003001 )
				GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"skill_22003001: _generateSkillDamageMultiple error 7: lSkillDamageType invalid: %d.", lSkillDamageType );
		}
		return;
	}

	if (NULL == pkAttacker->GetMap())
	{
		if ( bLog22003001 )
			GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"skill_22003001: _generateSkillDamageMultiple error 8: pkAttacker map is nullptr" );

		return;
	}

	Utility::GlobalSettings & rkGls = GetGlobalSetting;

	int iHitCount = 0;	//击中怪物计数；只统计普通伤害和技能伤害两种。
	//int64_t iDamageCount = 0;	//本次攻击所造成的伤害统计
	int targetListCount = 0;

	bool bCalcuteMain = false;
	int nTotalSize = kDamageTargetsList.size();
	if ( bLog22003001 )
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"skill_22003001: _generateSkillDamageMultiple before TRAV size: %d", nTotalSize );

	int nHitCount = 0;

	TRAV(kDamageTargetsList, Iter)
	{
		//对于每次攻击动作重新选择随即数
		iDamageRandomIndex = UT_MathBase::RandInRangeInt(0, MAX_ATTACKER_RANDNUM_COUNT - 1);
		//iAttackRateRandomIndex = UT_MathBase::RandInRangeInt(0, MAX_ATTACKER_RANDNUM_COUNT - 1);
		iAttackRateRandomIndex = targetListCount % MAX_ATTACKER_RANDNUM_COUNT;
		iCriticalRateRandomIndex = UT_MathBase::RandInRangeInt(0, MAX_ATTACKER_RANDNUM_COUNT - 1);

		targetListCount++;

		//伤害力骰子
		fDamageRandom = pkAttacker->GetRandNum(iDamageRandomIndex);
		//命中骰子
		fAttackRateRandom = pkAttacker->GetRandNum(iAttackRateRandomIndex);
		//暴击骰子
		fCriticalRateRandom = pkAttacker->GetRandNum(iCriticalRateRandomIndex);

		bCirtical = false;
		pkTarget = *Iter;
		if (pkTarget->GetDead())
		{
			if ( bLog22003001 )
				GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"skill_22003001: _generateSkillDamageMultiple continue 0: pkTarget is dead" );

			continue;
		}

		// internal flag
		// 0: none; 1: player 2: fake-player
		int nSoloFlag = 0;

		// 这段逻辑，用于调试期间，让对方的buddy有和主操不一样的战斗结果
		bool bIsBuddy = false;
		if ( IS_PLAYER_CLASS_ID(pkTarget->GetID()) )
		{
			LC_PlayerBase* pkPlayer = (LC_PlayerBase*)pkTarget;
			IF_A_NA_PLAYER( pkPlayer)
			{
				if ( bLog22003001 )
					GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"skill_22003001: _generateSkillDamageMultiple continue 1: pkPlayer is invalid: %d", pkPlayer->IsValid());

				continue ;
			}
			LC_ServerPlayer* pServerPlayer = (LC_ServerPlayer*)pkPlayer;
			if (pServerPlayer->GetControlType() != LC_ServerPlayer::eControlType_Player)
				bIsBuddy = true;
		}
		if ( pkTarget->GetMap() != pkAttacker->GetMap() )
		{
			if ( bLog22003001 )
				GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"skill_22003001: _generateSkillDamageMultiple continue 2: diff map: target: 0x%X, attack: 0x%X ", pkTarget->GetMap(), pkAttacker->GetMap());

			continue;
		}
		//-- 伤害调试模式
		//	-- 0 关闭，即正常逻辑
		//	-- 1 挑战方失败
		//	-- 2 被挑战方失败
		//	-- 3 超时退出
		if (pkAttacker->GetType() == LOT_PLAYER && rkGls.nSoloPlayDamageDebugMode)
		{
			LC_ServerPlayer* pSrcPlayer = (LC_ServerPlayer*)pkAttacker;
			{
				LC_ServerPlayer* pOwner = pSrcPlayer->GetOwnerPlayer();
				if (pOwner)
				{
					bool bSoloPlayMode = pOwner->GetSoloDulePlay();
					if (bSoloPlayMode)
					{
						if (pOwner->GetID() > 1000000)
						{
							// internal flag
							// 0: none; 1: player 2: fake-player
							nSoloFlag = 1;
						}
						else
						{
							nSoloFlag = 2;
						}
					}
				}
			}
		}

		lTargetID = pkTarget->GetID();
		if ( IS_PLAYER_CLASS_ID(lTargetID))
		{
			LC_ServerPlayer* pTargetPlayer = (LC_ServerPlayer*)pkTarget;
			pkTargetAttributeMap = pTargetPlayer->GetFinalAttributeMap();
		}
		else
			pkTargetAttributeMap = pkTarget->GetFinalAttributeMap();
		if (!pkTargetAttributeMap)
		{
			if ( bLog22003001 )
				GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"skill_22003001: _generateSkillDamageMultiple continue 3: pkTargetAttributeMap is nullptr" );

			continue;
		}
		//iTargetDog = pkTargetAttributeMap->GetAttributeValue(ATT_DOG_RATE);
		iTargetDog = pkTargetAttributeMap->GetAttribute(ATT_DOG_RATE);
		iTargetDogRate = pkTargetAttributeMap->GetAttributePercent(ATT_DOG_RATE);
		iTargetLevel = (int64_t)pkTarget->GetLevel();

		iTargetPHD = pkTargetAttributeMap->GetAttribute(ATT_DEF);

		//打印技能验证信息
		//UT_GameServerLogManager* pkLogManager = UT_GameServerLogManager::GetSingletonPtr();
		//pkLogManager->SKA_LogRemark("Skill_process_begin");
		//pkLogManager->SKA_LogActorInfo(pkAttacker, "Attacker_info_before_skill", "Attacker_info_before_");
		//pkLogManager->SKA_LogActorInfo(pkTarget, "Target_info_before_skill", "Target_info_before_");

		bool bHit = false;
		int missType = SK_DamageEvent::SDR_MISS;
		bool bAttackerAttackFlag = pkAttacker->GetSkillFlag(SK_ATTACK_FLAG);
		double fFinalAttackerRate = 0;
		int64_t iSkillAttackRate = int64_t(pkSkillAttrib->_fAttackRate) + pkScriptCallerAttributeMap->GetAttribute(ATT_ATK_RATE);

		//如果这个时候目标跳闪那就跳闪状态
		if (IS_PLAYER_CLASS_ID(lTargetID) && pkTarget->IsJumpImmortal())
		{
			bHit = false;
			missType = SK_DamageEvent::SDR_JUMP_DODGE;
		}
		else if (bAttackerAttackFlag)
		{
			bHit = true;
		}
		else if (pkTarget->GetSkillFlag(SK_IMM_DODOGE))
		{
			bHit = true;
		}
		else
		{
			// weixin commented: hit calculate logic here.
			fFinalAttackerRate = _compureAttackRate(iTargetDog, iTargetDogRate, 0, 0, iSkillAttackRate);
			bHit = (fAttackRateRandom < fFinalAttackerRate);
		}
		pkTargetMap->PushTargetsHitState(lTargetID, bHit);
		if (bLCSkillLogic)
			bHit = true;
		//判断是否命中
		if (!bHit)
		{
			/*
			//打印技能验证信息
			pkLogManager->SKA_LogSkillInfo(pkAttacker, pkTarget, pkSkillData, fFinalAttackerRate, 0, bHit, false, 0, 0, iMPCost, 0, "Skill_info_miss");
			pkLogManager->SKA_LogActorInfo(pkAttacker, "Attack_into_after_skill", "Attacker_info_after_");
			pkLogManager->SKA_LogActorInfo(pkTarget, "Target_info_after_skill", "Target_info_after_");
			*/

			if (pkScriptCaller->IsListening())
				GfxWriteLog(LOG_TAG_SKILL, LOG_SWITCH_INFO, "[%i]miss[%i]", lDamageSourceActorID, lTargetID);
	
			//抛出伤害事件
			NotifyClientSete_Damage(lDamageSourceActorID,lTargetID,lSkillTypeID,lSkillDamageType,missType,0,pkTarget->GetMapResID(),pkTarget->GetMapLogicID());
		

			if ( bLog22003001 )
				GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"skill_22003001: _generateSkillDamageMultiple continue 4: bHit is miss: %d type: %d param: [iTargetDog: %d - iTargetDogRate: %d - iSkillAttackRate: %d] result: fFinalAttackerRate: %f - fAttackRateRandom: %f input iAttackRateRandomIndex: %d"
					, bHit, missType, iTargetDog, iTargetDogRate, iSkillAttackRate, fFinalAttackerRate, fAttackRateRandom, iAttackRateRandomIndex );
			//fFinalAttackerRate = _compureAttackRate(iTargetDog, iTargetDogRate, 0, 0, iSkillAttackRate);
			//bHit = (fAttackRateRandom < fFinalAttackerRate);
			continue;
		}

		if ( bLog22003001 )
			GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"skill_22003001: _generateSkillDamageMultiple TRAV hit %d", ++nHitCount );

		bool bAttackIgnCriticalFlag = pkAttacker->GetSkillFlag(SK_IGNORE_CRITICAL);
		bool bTargetIgnGetCriticalFlag = pkTarget->GetSkillFlag(SK_IGNORE_GET_CRITICAL);
		bool bAttackGetCriticalFlag = pkAttacker->GetSkillFlag(SK_IGNORE_GET_CRITICAL);
		int iAttackerCriticalDef = (int)pkTargetAttributeMap->GetAttribute(ATT_CIR_DEF);
		
		int critAdd = 0;
		//玄冥神掌秘籍攻击冰冻目标暴击加成
		//LC_ActorStateEntry* stateEntry = pkAttacker->GetStateMap()->GetStateEntryByID(XM_STATE_ID);
		//if( stateEntry )
		//	critAdd = stateEntry->GetModify(pkAttacker, pkTarget);

		////神器暴击加成
		//stateEntry = pkAttacker->GetStateMap()->GetStateEntryByID(GOD_STATE_ID);
		//if( stateEntry )
		//	critAdd += stateEntry->GetModify(pkAttacker, pkTarget);

		////命骨暴击几率降低
		//stateEntry = pkTarget->GetStateMap()->GetStateEntryByID(BONESOUL_STATE_ID);
		//if( stateEntry )
		//	critAdd += stateEntry->GetModify(pkAttacker, pkTarget);

		if (bAttackIgnCriticalFlag)
		{
			bCirtical = false;
		}
		else if ( bTargetIgnGetCriticalFlag || bAttackGetCriticalFlag)
		{
			bCirtical = false;
		}
		else if (pkAttacker->GetSkillFlag(SK_CRITICAL_FLAG))
		{
			bCirtical = true;
		}
		else
		{
			// weixin commented: crit calculate logic here.
			bCirtical = _judgeCriticalAttack(iAttackerCritical, iAttackerCriticalRate + critAdd, iAttackerCriticalDef, iAttackerLevel, iTargetLevel, fSkillCriticalRate, fCriticalRateRandom, pkAttacker, pkTarget);
		}

		//计算防御力
		switch (lSkillDamageType)
		{
		case SDT_NORMAL:
		case SDT_SKILL:
		case SDT_LC_SKILL:
			{
				//是否忽略对方的防御
				iTargetDefence = pkAttacker->GetSkillFlag(SK_IGNORE_TARGET_PHD) ? 0 : iTargetPHD;
			}
			break;
		default:
			{
				iTargetDefence = 0;
			}
			break;
		}

		// B防御*(1 -A无视防御%)
		iTargetDefence = iTargetDefence*(1 - (attr_value_type)(pkScriptCallerAttributeMap->GetAttribute(ATT_IGNORE_DEF) * 0.0001f));
		// 保底伤害 pvp 是 iAttackerDamage * 0.35 pve iAttackerDamage* 0.05
		double append = _computeMinAck(pkTarget->GetID(), pkAttacker->GetID());
		fFinalDamage = _computeDamageBase(iAttackerDamage, iTargetDefence, iMPCost, fSkillDamageRate, fFinalAttackerRate - 1, append);
		//获取HP消耗率，根据HP消耗率修正伤害值
		// 技能固定 伤害
		fFinalDamage += pkSkillAttrib->_lDamageAdd;
		//  A技能生命伤害万分比系数 *A生命伤害万分比 * B最大生命值
		int64_t iAttackerHpHurt = pkScriptCallerAttributeMap->GetAttribute(ATT_HP_HURT);
		double fHpHurtRate = pkSkillAttrib->_HpHurtRate;
		int64_t iTargetMaxHp = pkTargetAttributeMap->GetAttribute(ATT_MAX_HP);
		double fHpHurt = fHpHurtRate * iAttackerHpHurt * 0.0001f * iTargetMaxHp;
		fFinalDamage += fHpHurt;
		if (IS_NPC_ID(lTargetID))
		{
			fFinalDamage += pkSkillAttrib->_lMonsterDamageAdd;
		}
		double fTargetHPCostRate = LC_Helper::GetAttributeRate(pkTarget, ATT_HP_COST_RATE);

		fFinalDamage  = fTargetHPCostRate * fFinalDamage * 0.0001f * (double)Multiple;
		if (bLCSkillLogic)
			bCirtical = false;

		if (bCirtical)
		{
			iAttackerCriticalDamage = pkScriptCallerAttributeMap->GetAttribute(ATT_CIR_DAM);
			//被攻击这暴击伤害降低
			int64_t iCriticalDamageDerate = pkTargetAttributeMap->GetAttribute(ATT_CRI_DMG_DERATE);

			// weixin commented: damage about crit calculate logic here.
			// 尧辉需求：
			// A打出的暴击伤害=伤害*（1+max（A暴击伤害-B暴击减伤，0））
			fFinalDamage =  fFinalDamage * (1.0 + UT_MathBase::MaxDouble((iAttackerCriticalDamage - iCriticalDamageDerate) * 0.0001f, 0));
		}
		// 1+A伤害加成% - B伤害减免%
		float fSrcRate = LC_Helper::GetAttributeRate(pkAttacker, ATT_DMG_RATE);
		float fDstRate = LC_Helper::GetAttributeRate(pkTarget, ATT_DMG_DERATE);
		double fDamageRate_1 = UT_MathBase::MaxDouble(1 + fSrcRate - fDstRate, 0);
		fFinalDamage = fFinalDamage * fDamageRate_1;
		// pvp 指挥官战力 修正伤害值
		uint64_t iTargetScore = pkTarget ->GetControllerCombatScore();
		if ( IS_PLAYER_CLASS_ID(pkAttacker->GetID()) && IS_PLAYER_CLASS_ID(pkTarget->GetID()) && bUseScore)
		{
			LC_ActorAttributeMap* pkAttributeMap = pkTarget->GetFinalAttributeMap();
			attr_value_type lMaxHP = pkAttributeMap->GetAttribute(ATT_MAX_HP);
			fFinalDamage += _computeScoreDamgeBase( pkAttacker->GetControllerCombatScore() , iTargetScore, lMaxHP, fSkillDamageRate);
		}

		//AOE伤害，非锁定目标伤害衰减
		if (IS_PLAYER_CLASS_ID(pkTarget->GetID()) && iPercent > 0)
		{
			if (pkAttacker->GetLockedTargetID() != 0 && pkTarget->GetID() != pkAttacker->GetLockedTargetID())
			{
				fFinalDamage = (fFinalDamage * iPercent * 0.0001f);
			}
			else if (pkAttacker->GetLockedTargetID() == 0 && bCalcuteMain == false)
			{
				bCalcuteMain = true;
			}
			else if (pkAttacker->GetLockedTargetID() == 0 && bCalcuteMain == true)
			{
				fFinalDamage = (fFinalDamage * iPercent  * 0.0001f);
			}
		}
		
		fFinalDamage = UT_MathBase::MaxDouble(1, fFinalDamage);
		//iDamageCount += iFinalDamage;

		if (pkScriptCaller->IsListening())
		{
			GfxWriteFmtLog(LOG_SKILL_FILELINE, LOG_SWITCH_INFO, lDamageSourceActorID, "damage to", lTargetID, fFinalDamage);
		}
	
		// jjc debug: lost
		//if ( pkAttacker->GetID() >800000 || bIsBuddy) 
		//iFinalDamage = 1;
		// Damage Debug Impl
		if (
			(nSoloFlag == 1 && rkGls.nSoloPlayDamageDebugMode == 1) ||
			(nSoloFlag == 2 && rkGls.nSoloPlayDamageDebugMode == 2) ||
			(rkGls.nSoloPlayDamageDebugMode == 3)
			)
			fFinalDamage = 1.0;
		if (bLCSkillLogic && pkScriptCaller->GetType() == LOT_PLAYER)
		{
			LC_ServerPlayer* pPlayer = (LC_ServerPlayer*)pkScriptCaller;
			fFinalDamage = pPlayer->GetPetSkillDamageByMode(static_cast<Skill::PetDamageMode>(pkSkillData->_lPetDamageMode)) * fSkillDamageRate;
		}
		// 元素伤害
		fFinalDamage *= FetchElementDamageData(pkAttacker->GetDamageElementType(), pkTarget->GetDamageElementType());
		// 等级压制
		fFinalDamage *= FetchLevelFuckData((LC_ActorBase*)pkAttacker, (LC_ActorBase*)pkTarget);
		// 战力压制
		fFinalDamage *= GetCombatYaZhiValue((LC_ActorBase*)pkAttacker, (LC_ActorBase*)pkTarget);
		// 指挥官 头衔等级压制
		fFinalDamage *= FetchCommanderLevelFuckData((LC_ActorBase*)pkAttacker, (LC_ActorBase*)pkTarget);
		if (rkGls.nDamageValueDebugMode == 1)
			fFinalDamage = rkGls.nStaticDamageValue;

		fFinalDamage *= pkAttacker->GetFinalDamageRate();

		if ( pkTarget->GetStaticInputDamage() > 0 )
			fFinalDamage = pkTarget->GetStaticInputDamage();
		int lDamageResult = bCirtical ? SK_DamageEvent::SDR_CRITICAL : SK_DamageEvent::SDR_HIT;
		//抛出伤害事件
		if( true == NotifyClientSete_Damage(lDamageSourceActorID,lTargetID,lSkillTypeID,lSkillDamageType,lDamageResult,fFinalDamage,pkTarget->GetMapResID(),pkTarget->GetMapLogicID()))
		{
			//连击统计
			if (bNeedCalcHitCount && IS_NPC_ID(lTargetID))
			{
				iHitCount += 1;
			}
		}

		//打印技能验证信息
		//float fFinalCriticalRate = _computeCriticalRate(iAttackerCriticalRate,0, iAttackerLevel, iTargetLevel,fSkillCriticalRate);
		//暴击必然命中
		//bHit = bCirtical ? true : false;
		//StringType kRemark = bCirtical ? "Skill_info_critical" : "Skill_info_hit";
		/*
		pkLogManager->SKA_LogSkillInfo(pkAttacker, pkTarget, pkSkillData, fFinalAttackerRate, fFinalCriticalRate, bHit, bCirtical, iAttackerDamage, iTargetDefence, iMPCost, iFinalDamage, kRemark);
		pkLogManager->SKA_LogActorInfo(pkAttacker, "Attack_into_after_skill", "Attacker_info_after_");
		pkLogManager->SKA_LogActorInfo(pkTarget, "Target_info_after_skill", "Target_info_after_");
		*/
		
		if (bDoPunch)
			_doDmagePunch(fFinalDamage, pkTarget, pkSkillData, lTargetID, pkAttacker);
	}
	//暂时屏蔽连击功能
	// 	if (IS_PLAYER_CLASS_ID(lDamageSourceActorID))
	// 	{
	// 		LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)pkAttacker;
	// 		pkServerPlayer->SummorCombo(iHitCount, fCurrentTime);
	// 	}

	//重置必命中标志
	//pkAttacker->SetSkillFlag(SK_ATTACK_FLAG, false);
	//重置必暴击标志
	//pkAttacker->SetSkillFlag(SK_CRITICAL_FLAG, false);
	//每次攻击都重新随机随机数
	if ( bLog22003001 )
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"skill_22003001: _generateSkillDamageMultiple end total: %d / %d.", nHitCount, nTotalSize );

	pkAttacker->RecomputeRandNums();
}

void SC_ServerSkillInterface::AddPlayerBoneSoul( GameLogic::LC_ActorBase* caller, int skillID )
{
	if ( caller && IS_PLAYER_CLASS_ID(caller->GetID()) )
	{
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)caller;
		pkPlayer->AddBoneSoul(skillID);
	}
}

int SC_ServerSkillInterface::GetPlayerBoneSoul( GameLogic::LC_ActorBase* caller,int iIndex )
{
	if ( caller && IS_PLAYER_CLASS_ID(caller->GetID()) )
	{
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)caller;
		return pkPlayer->GetBoneSoul(iIndex);
	}
	return 0;
}													  

void SC_ServerSkillInterface::RemovePlayerBoneSoul( GameLogic::LC_ActorBase* caller, int skillID )
{
	if ( caller && IS_PLAYER_CLASS_ID(caller->GetID()) )
	{
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)caller;
		return pkPlayer->RemoveBoneSoul(skillID);
	}
}

void SC_ServerSkillInterface::GFxWriteGAMELog(const char * str)
{
	GfxWriteLog(LOG_INFO_FILELINE, LOG_TAG_GAME_LOG,"[GAME] %s",str);
}

bool SC_ServerSkillInterface::NotifyClientSete_Damage(uint32_t SrcActorID,uint32_t DstActorID,uint32_t LogicID,uint32_t lSkillDamageType,uint32_t lDamageResult,uint32_t iFinalDamage,uint32_t MapResID,uint32_t MapLogicID)
{
	SK_ServerSkillEventManager* pkEventManager = SERVER_GET_SKILL_EVENT_MANAGER();
	SK_SkillEvent* pkSkillEvent = pkEventManager->AllocEvent();
	if (NULL == pkSkillEvent)
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_TAG_INFO," NotifyClientSete_Damage  AllocEvent error ！！！\n");
		return false;
	}
	pkSkillEvent->SetEventType(SETE_DAMAGE);
	pkSkillEvent->SetSrcActorID(SrcActorID);
	pkSkillEvent->SetDstActorID(DstActorID);
	pkSkillEvent->SetLogicID(LogicID);
	pkSkillEvent->SetParam(SK_DamageEvent::SEP_DAMAGE_TYPE, lSkillDamageType);
	pkSkillEvent->SetParam(SK_DamageEvent::SEP_RESULT, lDamageResult);
	pkSkillEvent->SetParam(SK_DamageEvent::SEP_DAMAGE_BASE, iFinalDamage);
	pkSkillEvent->SetParam(SK_DamageEvent::SEP_MAP_RES_ID, MapResID);
	pkSkillEvent->SetParam(SK_DamageEvent::SEP_MAP_LOGIC_ID,MapLogicID);
	pkEventManager->AddEvent(pkSkillEvent);
	return true;
}

double SC_ServerSkillInterface::GetBonFirPartyMapBuffStateMutableParameter(GameLogic::LC_Attacker* pkScriptCaller)
{
	object_id_type lCallerID = pkScriptCaller->GetID();
	if(!IS_PLAYER_CLASS_ID(lCallerID) && !IS_NPC_ID(lCallerID))
	{
		return 0.0f;
	}
	LC_ActorBase* pkCaller = (LC_ActorBase*)pkScriptCaller;
	uint64_t nGuildId = pkCaller->GetGuildID();
	return (double)(UT_ServerHelper::GetGuildMapStateMutableParameter(nGuildId) / 10000.0);
}