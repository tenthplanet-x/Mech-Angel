#include "LM_LocaleManager.h"

#include "GameServerApp.h"
#include "LC_ServerWorldManager.h"
#include "UT_ServerHelper.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerNPC.h"
#include "LC_ServerMapLogicFactory.h"
#include "LC_ServerMapLogic_Place.h"
#include "LC_ServerMapLogic_SoloDulePlay.h"
#include "LC_RankManager.h"
#include "CF_RankReward.h"
#include "LC_Dule_Manager.h"
#include "oss_define.h"
#include "SM_StateBase.h"
#include "../Buddy/BuddyInstance.h"
#include "UT_OperateLog.h"
#include "gsnode.h"

using namespace OPERATELOG;
using namespace GameLogic;
using namespace CSVFile;
using namespace Protocol;
using namespace Utility;

LC_ServerMapLogic_SoloDulePlay::LC_ServerMapLogic_SoloDulePlay():
m_Config(NULL),m_State(SoloDule_State_None),
m_ChallengePlayerID(INVALID_ACTOR_ID),m_BeChallengeCharID(0),m_BeChallengePlayer(NULL),m_BeChallengeIndex(NONE_INDEX),m_SoloPrePareTime(0),m_SoloBattleTime(0),m_TimeStamp(0.0f),m_PlayerBuffInfo(),m_isolofightuniqueId(0)
{

}

LC_ServerMapLogic_SoloDulePlay::~LC_ServerMapLogic_SoloDulePlay()
{
	//UnInit();
}

bool LC_ServerMapLogic_SoloDulePlay::Init(LC_ServerMapLogic_Place* place,LC_Dule_Config* config, GameLogic::SOLO_DULE_TYPE eSoloType)
{
	if (place==NULL || config==NULL)
	{
		return false;
	}
	bool ret=LC_ServerMapLogic_PlayBase::Init(place,PlayType_SoloDule);
	if (!ret)
	{
		return false;
	}
	m_Config=config;
	m_ChallengePlayerID = 0;
	m_BeChallengeCharID=0;
	m_BeChallengePlayer=NULL;
	m_BeChallengeIndex=NONE_INDEX;
	m_SoloPrePareTime=m_Config->PrePareTime;
	m_SoloBattleTime=m_Config->BattleTime;
	m_TimeStamp=0.0f;
	m_PlayerBuffInfo.clear();
	m_State=SoloDule_State_Inited;
	m_eSoloType = eSoloType;
	return true;
}

void LC_ServerMapLogic_SoloDulePlay::UnInit()
{
	End();
	// relase
	mem::vector<int32_t> vecBuddyIDsB;
	IF_A_VALID_PLAYER(m_BeChallengePlayer)
	{
		m_BeChallengePlayer->GetActivateBuddyIDs( vecBuddyIDsB);
		LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
		for (int i = 0; i < vecBuddyIDsB.size(); i++)
		{
			LC_ServerPlayer* pBuddy = m_BeChallengePlayer->GetActivateBuddyByID(vecBuddyIDsB[i]);
			if (pBuddy == NULL)
				continue;
			pkPlayerManager->DestroyPlayer(pBuddy);
		}
	}
	m_ChallengePlayerID = 0;
	m_BeChallengeCharID=0;
	m_BeChallengePlayer=NULL;
	m_BeChallengeIndex=NONE_INDEX;
	m_SoloPrePareTime=0;
	m_SoloBattleTime=0;
	m_TimeStamp=0;
	m_PlayerBuffInfo.clear();
	m_State=SoloDule_State_None;
	m_Config=NULL;
	LC_ServerMapLogic_PlayBase::UnInit();
}

bool LC_ServerMapLogic_SoloDulePlay::Start()
{
	if (m_State!=SoloDule_State_Inited && m_State!=SoloDule_State_End)
	{
		return false;
	}
	m_State=SoloDule_State_Prepare;
	return true;
}

void LC_ServerMapLogic_SoloDulePlay::End()
{
	if (!CheckRun())
	{
		return;
	}
	float fCurrentTime = GET_PROCESS_TIME;
	m_State=SoloDule_State_End;
	// 停止战斗行为
	LC_ServerPlayer* challengePlayer=GetPlace()->GetActorManger().GetPlayer(m_ChallengePlayerID);
	
	IF_A_VALID_PLAYER(challengePlayer)
	{
		mem::vector<int32_t> vecBuddyIDsA;
		challengePlayer->GetActivateBuddyIDs( vecBuddyIDsA);
		for (int i = 0; i < vecBuddyIDsA.size(); i++)
		{
			LC_ServerPlayer* pBuddy = challengePlayer->GetActivateBuddyByID(vecBuddyIDsA[i]);
			if (pBuddy == NULL)
				continue;
			pBuddy->TryInterruptSkill( -1, fCurrentTime, SKI_FORCE);
			pBuddy->TryFinishSkill( -1, fCurrentTime, pBuddy->GetCurrentLocation().x, pBuddy->GetCurrentLocation().y );
			pBuddy->RefreshCurrentOPIndex( i, false );
			pBuddy->GetHatredList()->Reset();
			pBuddy->GetHatredList()->Disable();
		}
	}

	
	IF_A_VALID_PLAYER(m_BeChallengePlayer)
	{
		mem::vector<int32_t> vecBuddyIDsB;
		m_BeChallengePlayer->GetActivateBuddyIDs( vecBuddyIDsB);
		for (int i = 0; i < vecBuddyIDsB.size(); i++)
		{
			LC_ServerPlayer* pBuddy = m_BeChallengePlayer->GetActivateBuddyByID(vecBuddyIDsB[i]);
			if (pBuddy == NULL)
				continue;
			pBuddy->TryInterruptSkill( -1, fCurrentTime, SKI_FORCE);
			pBuddy->TryFinishSkill( -1, fCurrentTime, pBuddy->GetCurrentLocation().x, pBuddy->GetCurrentLocation().y );
			pBuddy->RefreshCurrentOPIndex( i, false );
			pBuddy->GetHatredList()->Reset();
			pBuddy->GetHatredList()->Disable();
		}
	}

	//结算
	Settle();
	//删除对方
	IF_A_VALID_PLAYER(m_BeChallengePlayer)
	{
		GetPlace()->AddActor2Faction(m_BeChallengePlayer, FACTION_EVIL);
		m_BeChallengePlayer->GetActorSkillList()->Reset();
		m_BeChallengePlayer->SetSoloDulePlay(false);
	}
	//LC_ServerPlayer* challengePlayer=GetPlace()->GetActorManger().GetPlayer(m_ChallengePlayerID);
	IF_A_VALID_PLAYER(challengePlayer)
	{
		GetPlace()->AddActor2Faction(challengePlayer,FACTION_PLAYER);
		challengePlayer->GetActorSkillList()->Reset();
		challengePlayer->SetSoloDulePlay(false);
		//challengePlayer->LoadSnapshotData();
	}
	//GetPlace()->ClearPlayer(m_BeChallengePlayer);
	ChallengePlayerActionUnForbid();
}

bool LC_ServerMapLogic_SoloDulePlay::Update(float fCurrentTime, float fDeltaTime)
{
	if(m_State==SoloDule_State_CountDown)
	{
		if(fCurrentTime>=m_TimeStamp)
		{
			Battle();
		}
	}
	else if(m_State==SoloDule_State_Battle)
	{
		if(fCurrentTime>=m_TimeStamp)
		{
			BattleResult();
		}
	}
	return true;
}

ResultType LC_ServerMapLogic_SoloDulePlay::CheckEnter(LC_ServerPlayer* player)
{
	if (player==NULL)
	{
		return RE_FAIL;
	}
	if (player->GetID() != m_ChallengePlayerID)
	{
		return RE_FAIL;
	}
	return RE_SUCCESS;
}

Utility::UT_Vec3Int	LC_ServerMapLogic_SoloDulePlay::ComputeEnterLocation(LC_ServerPlayer* player,const Utility::UT_Vec3Int& targetlocation)
{
	return m_Config->Factions[0].Position;
}

void LC_ServerMapLogic_SoloDulePlay::EventNotify(MapLogicEvent& kEvent)
{
	
}

void LC_ServerMapLogic_SoloDulePlay::PlayerEventNotify(LC_ServerPlayer* player, MapLogicEvent& kEvent)
{
	if (player==NULL)
	{
		return;
	}
	if (kEvent.Type()==PlayerEvent_Enter)
	{
		GetPlace()->AddActor2Faction(player, FACTION_NEUTRAL);
		ChallengePlayerActionForbid();
		//准备对方玩家
		Prepare();
	}
	else if (kEvent.Type()==PlayerEvent_Leave)
	{
		ChallengePlayerActionUnForbid();
		End();
	}
	else if (kEvent.Type()==PlayerEvent_Death)
	{
		object_id_type nDeadId;
		bool bOwnerDead = CheckOwnerDead(player, nDeadId);
		// No Controller dead.
		if (!bOwnerDead)
			return;

		if (nDeadId == m_ChallengePlayerID)
		{
			GetPlace()->SetFactionResult(FACTION_PLAYER,FACTION_RESULT_FAIL);
			GetPlace()->SetFactionResult(FACTION_EVIL,FACTION_RESULT_SUCESS);
		}
		else
		{
			GetPlace()->SetFactionResult(FACTION_PLAYER,FACTION_RESULT_SUCESS);
			GetPlace()->SetFactionResult(FACTION_EVIL,FACTION_RESULT_FAIL);
		}
		End();
	}
}

bool LC_ServerMapLogic_SoloDulePlay::CheckOwnerDead( LC_ServerPlayer* pPlayer, object_id_type& nDeadPlayerID )
{
	bool bResult = false;

	bResult = pPlayer->CheckOwnerDead();
	if (bResult)
	{
		nDeadPlayerID = pPlayer->GetOwnerPlayerId();
	}

	return bResult;
}

bool LC_ServerMapLogic_SoloDulePlay::CheckEnd()
{
	return m_State==SoloDule_State_End;
}

bool LC_ServerMapLogic_SoloDulePlay::CheckRun()
{
	if (m_State==SoloDule_State_Prepare || m_State==SoloDule_State_CountDown || m_State==SoloDule_State_Battle)
	{
		return true;
	}
	return false;
}

void LC_ServerMapLogic_SoloDulePlay::Settle()
{
	switch ( m_eSoloType )
	{
	case GameLogic::eSoloDuleType_Arena:
		Settle_Arena();
		break;
	case GameLogic::eSoloDuleType_GuildOffline:
		// 异步处理 这里 去db 拿相关扣除分数的限制
		PreSettle_GuildOffline();
		//Settle_GuildOffline();
		break;
	default:
		break;
	}
	return;
}

void LC_ServerMapLogic_SoloDulePlay::ChallengePlayerActionForbid()
{
	LC_ServerPlayer* challengePlayer=GetPlace()->GetActorManger().GetPlayer(m_ChallengePlayerID);
	IF_A_NA_PLAYER(challengePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, " LC_ServerMapLogic_SoloDulePlay  ChallengePlayerActionForbid not find player uid :%u \n",m_ChallengePlayerID);
		return;
	}
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, " LC_ServerMapLogic_SoloDulePlay  ChallengePlayerActionForbid  uid :%u \n",m_ChallengePlayerID);
	LC_ActionForbid* pa = challengePlayer->GetActionForbid();
	pa->AddActionForbid(LAFT_GROUP,LAF_GROUP_MAX_COUNT);
	pa->AddActionForbid(LAFT_ITEM,	LAF_ITEM_MAX_COUNT);
	pa->AddActionForbid(LAFT_SKILL,LAF_SKILL_SKILL_ID,SKILL_RETURN_CITY);
	pa->AddActionForbid(LAFT_TRADE,LAF_TRADE_MAX_COUNT);
	//pa->AddActionForbid(LAFT_DUEL, LAF_DUEL_MAX_COUNT);
	pa->AddActionForbid(LAFT_GAME,LAF_GAME_PLAYER_LEVEL_UP);
	pa->AddActionForbid(LAFT_GAME,LAF_GAME_SKILL_LEVEL_UP);
	pa->AddActionForbid(LAFT_EQUIP,LAF_EQUIP_MAX_COUNT);
	pa->AddActionForbid(LAFT_AUCTION,LAF_AUCTION_MAX_COUNT);
	pa->AddActionForbid(LAFT_MAIL,LAF_MAIL_MAX_COUNT);
	pa->AddActionForbid(LAFT_GUILD,LAF_GUILD_MAX_COUNT);
	challengePlayer->SetDenyMove(true);
	//pa->AddActionForbid(LAFT_SHOPCITY,LAF_SHOPCITY_MAX_COUNT);
	m_PlayerBuffInfo.clear();

	challengePlayer->GetStateMap()->RemoveSkillStateInStateMap(GOD_STATE_ID);
	char_data_define::save_skill_state_info info;
	challengePlayer->GetStateMap()->Clear_SaveAdditionalStateToPB(info);
	info.SerializeToString(&m_PlayerBuffInfo);
	challengePlayer->SetAIControl(true);
	challengePlayer->RefreshSMController();
	challengePlayer->GenSkillRandList();
	//LC_ActorAttributeMap* pkAttributeMap = challengePlayer->GetAttributeMap();
	//int lMP = (int)(pkAttributeMap->GetAttribute(ATT_MAX_MP));
	mem::vector<int32_t> vecBuddyIDsA;
	challengePlayer->GetActivateBuddyIDs( vecBuddyIDsA);

	int nCurrentIndex = 0;
	//nCurrentIndex = challengePlayer->GetCurrentActivateBuddyIndex();
	challengePlayer->SetCurrentBuddy(nCurrentIndex);
	for (int i = 0; i < vecBuddyIDsA.size(); i++)
	{
		LC_ServerPlayer* pBuddy = challengePlayer->GetActivateBuddyByID(vecBuddyIDsA[i]);
		if (pBuddy == NULL)
			continue;
		//pBuddy->SetForwardDir( UT_Vec2Float(0.0f,1.0f) );
		pBuddy->SetHP(pBuddy->GetFinalAttributeMap()->GetAttribute(ATT_MAX_HP));
		pBuddy->SetDead(false);
		pBuddy->RefreshCurrentOPIndex( nCurrentIndex );
	}
	
}

void LC_ServerMapLogic_SoloDulePlay::ChallengePlayerActionUnForbid()
{
	LC_ServerPlayer* challengePlayer=GetPlace()->GetActorManger().GetPlayer(m_ChallengePlayerID);
	IF_A_NA_PLAYER(challengePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, " LC_ServerMapLogic_SoloDulePlay  ChallengePlayerActionUnForbid not find player uid :%u \n",m_ChallengePlayerID);
		return;
	}
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, " LC_ServerMapLogic_SoloDulePlay  ChallengePlayerActionUnForbid player uid :%u \n",m_ChallengePlayerID);
	LC_ActionForbid* pa = challengePlayer->GetActionForbid();
	pa->RemoveActionForbid(LAFT_GROUP,LAF_GROUP_MAX_COUNT);
	pa->RemoveActionForbid(LAFT_ITEM,	LAF_ITEM_MAX_COUNT);
	pa->RemoveActionForbid(LAFT_SKILL,LAF_SKILL_SKILL_ID,SKILL_RETURN_CITY);
	pa->RemoveActionForbid(LAFT_TRADE,LAF_TRADE_MAX_COUNT);
	//pa->RemoveActionForbid(LAFT_DUEL, LAF_DUEL_MAX_COUNT);
	pa->RemoveActionForbid(LAFT_GAME,LAF_GAME_PLAYER_LEVEL_UP);
	pa->RemoveActionForbid(LAFT_GAME,LAF_GAME_SKILL_LEVEL_UP);
	pa->RemoveActionForbid(LAFT_EQUIP,LAF_EQUIP_MAX_COUNT);
	pa->RemoveActionForbid(LAFT_AUCTION,LAF_AUCTION_MAX_COUNT);
	pa->RemoveActionForbid(LAFT_MAIL,LAF_MAIL_MAX_COUNT);
	pa->RemoveActionForbid(LAFT_GUILD,LAF_GUILD_MAX_COUNT);
	//pa->RemoveActionForbid(LAFT_SHOPCITY,LAF_SHOPCITY_MAX_COUNT);
	char_data_define::save_skill_state_info info;
	info.ParseFromString(m_PlayerBuffInfo);
	challengePlayer->GetStateMap()->GetSaveStateFromPB(info,false);
	challengePlayer->GetStateMap()->StartSavedSkillState();
	challengePlayer->SyncSkillStatesToClient(SSC_ALL);

	//challengePlayer->SetHP(challengePlayer->GetAttributeMap()->GetAttribute(ATT_MAX_HP));
}

void LC_ServerMapLogic_SoloDulePlay::ReservaDuleInfo(object_id_type challengeid,const unique_id_impl& bechallengeid,int bechallengeindex,int64_t isolofightuniqueId)
{
	m_ChallengePlayerID=challengeid;
	m_BeChallengeCharID=bechallengeid;
	m_BeChallengePlayer=NULL;
	m_BeChallengeIndex=bechallengeindex;
	m_isolofightuniqueId = isolofightuniqueId;
}

void LC_ServerMapLogic_SoloDulePlay::Prepare()
{
	LC_ServerPlayer *bechallengeplayer=GetPlace()->SpawnPlayer(m_BeChallengeCharID, m_Config->Factions[1].Position, PLY_DEF_CLONE_BITS, NULL, LC_ServerMapLogic_SoloDulePlay::SummonPlayerFinish, FACTION_NEUTRAL);
	IF_A_NA_PLAYER(bechallengeplayer)
		return;
	float fCurrent = GET_PROCESS_TIME;

	uint32_t mapID = SOLO_DULE_RES_ID;
	switch (m_eSoloType)
	{
	case eSoloDuleType_Arena:
		{
			mapID = SOLO_DULE_RES_ID;
		}
		break;
	case eSoloDuleType_GuildOffline:
		{
			mapID = GUILD_OFFLINE_SOLO_DULE_RES_ID;
		}
		break;
	default:
		return;
	}

	bechallengeplayer->GetBuddySchemeMgr().SetBeChallangeMapID(mapID);

	m_BeChallengePlayer=bechallengeplayer;
	m_BeChallengePlayer->SetSoloDulePlay(true);
	if(IS_SOLODULE_ROBOT_PLAYERID(m_BeChallengeCharID))
	{
		//是机器人，创建数据
		bechallengeplayer->InitRobotLogicData(m_BeChallengeCharID);
	}
	
	return;
}

void LC_ServerMapLogic_SoloDulePlay::CountDown()
{
	LC_ServerPlayer* challengePlayer=GetPlace()->GetActorManger().GetPlayer(m_ChallengePlayerID);
	LC_ServerPlayer* bechallengePlayer=m_BeChallengePlayer;
	float fCur = GET_PROCESS_TIME;
	IF_A_NA_PLAYER(challengePlayer)
		return;
	IF_A_NA_PLAYER(bechallengePlayer)
		return;
	if(challengePlayer->GetRiderCharType()==0)
	{
		//challengePlayer->GoRider();
	}
	if(bechallengePlayer->GetRiderCharType()==0)
	{
		//bechallengePlayer->GoRider();
	}
	challengePlayer->GetSkillCoolDownMap()->ClearAllSkillCDNoGod();
	bechallengePlayer->GetSkillCoolDownMap()->ClearAllSkillCD();

	UT_Vec2Float rkDir;
	rkDir.x = Int2Float(bechallengePlayer->GetCurrentLocation().x - challengePlayer->GetCurrentLocation().x);
	rkDir.y = Int2Float(bechallengePlayer->GetCurrentLocation().y - challengePlayer->GetCurrentLocation().y);
	rkDir.Unitize();
	//challengePlayer->SetForwardDir( UT_Vec2Float(0.0f,1.0f) );
	mem::vector<int32_t> vecBuddyIDsA;
	challengePlayer->GetActivateBuddyIDs( vecBuddyIDsA);
	mem::vector<int32_t> vecBuddyIDsB;
	bechallengePlayer->GetActivateBuddyIDs( vecBuddyIDsB);
	for (int i = 0; i < vecBuddyIDsA.size(); i++)
	{
		LC_ServerPlayer* pBuddy = challengePlayer->GetActivateBuddyByID(vecBuddyIDsA[i]);
		if (pBuddy == NULL)
			continue;
		//pBuddy->SetForwardDir( rkDir );
		//pBuddy->SetForwardDir( UT_Vec2Float(1.0f,1.6f) );
		pBuddy->SetForwardDirAngle(135);
		pBuddy->SetAIControlNotify(true);
		//pBuddy->RefreshCurrentOPIndex(i);
	}

	rkDir.x=0-rkDir.x;
	rkDir.y=0-rkDir.y;
	//bechallengePlayer->SetForwardDir( rkDir );
	for (int i = 0; i < vecBuddyIDsB.size(); i++)
	{
		LC_ServerPlayer* pBuddy = bechallengePlayer->GetActivateBuddyByID(vecBuddyIDsB[i]);
		if (pBuddy == NULL)
			continue;
		//pBuddy->SetForwardDir( rkDir );
		//pBuddy->SetForwardDir( UT_Vec2Float(0.0f,-1.0f) );
		pBuddy->SetForwardDirAngle(315);
		pBuddy->SetAIControlNotify(true);
		//pBuddy->RefreshCurrentOPIndex(i);
	}

	GetPlace()->NotifyGameEventToClient(challengePlayer,EVENT_TYPE_SOLODULE_PREPARE,m_SoloPrePareTime);
	m_State=SoloDule_State_CountDown;
	m_TimeStamp=GET_PROCESS_TIME+m_SoloPrePareTime+1;//+1是因为客户端有一个"开始战斗界面"，需要1秒后再开始battle
}

void LC_ServerMapLogic_SoloDulePlay::Battle()
{
	LC_ServerPlayer* challengePlayer=GetPlace()->GetActorManger().GetPlayer(m_ChallengePlayerID);
	LC_ServerPlayer* bechallengePlayer=m_BeChallengePlayer;
	
	IF_A_NA_PLAYER(challengePlayer)
		return;
	IF_A_NA_PLAYER(bechallengePlayer)
		return;
	const unique_id_impl& challengePlayerInstanceID = challengePlayer->GetInstance();
	const unique_id_impl& beChallengePlayerInstanceID = bechallengePlayer->GetInstance();

	float fCur = GET_PROCESS_TIME;
	GetPlace()->AddActor2Faction(challengePlayer,FACTION_PLAYER);
	GetPlace()->AddActor2Faction(bechallengePlayer,FACTION_EVIL);
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "<%u>SoloDulePlay: %s challengePlayer c_cid:%d, c_citizenship:%d, c_uid:%d VS beChallengePlayer c_cid:%d, c_citizenship:%d, c_uid:%d", 
		UT_ServerHelper::GetServerId(), __FUNCTION__, challengePlayerInstanceID.detail(), challengePlayerInstanceID.catagory(), challengePlayer->GetUserID(), beChallengePlayerInstanceID.detail(), beChallengePlayerInstanceID.catagory(), bechallengePlayer->GetUserID());

	InitBattle( challengePlayer, bechallengePlayer );

	GetPlace()->NotifyGameEventToClient(challengePlayer,EVENT_TYPE_SOLODULE_BATTLE,m_SoloBattleTime);

	m_State=SoloDule_State_Battle;
	m_TimeStamp=GET_PROCESS_TIME+m_SoloBattleTime;
}

//#define			MULTI_THREAT_MODE		1


#define MULTI_THREAT_MODE

void LC_ServerMapLogic_SoloDulePlay::InitHatred( LC_ServerPlayer* pPlayerAController, LC_ServerPlayer* pPlayerBController )
{
	float update_time = GET_PROCESS_TIME;
	update_time += m_Config->BattleTime;
	LC_ServerPlayer* pCurrentA = pPlayerAController->GetOwnerPlayer();
	LC_ServerPlayer* pCurrentB = pPlayerBController->GetOwnerPlayer();
	IF_A_NA_PLAYER(pCurrentA)
		return;
	IF_A_NA_PLAYER(pCurrentB)
		return;
	std::vector<LC_ServerPlayer*> vecBuddyPtrPlayerA;	
	pCurrentA->GetAllBuddyPtrPlayer( vecBuddyPtrPlayerA);
	std::vector<LC_ServerPlayer*> vecBuddyPtrPlayerB;	
	pCurrentB->GetAllBuddyPtrPlayer( vecBuddyPtrPlayerB);
	for (int i = 0; i < vecBuddyPtrPlayerB.size(); i++)
	{
		LC_ServerPlayer* pBuddyB = vecBuddyPtrPlayerB[i];
		IF_A_NA_PLAYER(pBuddyB)
			continue;
		uint32_t id =  pBuddyB->GetID();
		for (int k = 0; k < vecBuddyPtrPlayerA.size(); k++)
		{
			LC_ServerPlayer* pBuddyA = vecBuddyPtrPlayerA[k];
			IF_A_NA_PLAYER(pBuddyA)
				continue;
			pBuddyA->GetHatredList()->Reset();
			pBuddyA->GetHatredList()->AddHatredOnly(id,10,update_time);
			pBuddyA->SetDefaultTargetID( id );
			pBuddyA->SetSelectTargetID(id );
			pBuddyA->SetDead(false);
			pBuddyA->SetDying(false);
			
			attr_value_type nMaxHP = pBuddyA->GetFinalAttributeMap()->GetAttribute(ATT_MAX_HP);
			pBuddyA->SetHP(nMaxHP);
			pBuddyA->SetSoloDulePlay(true);
		}
	}

}

//#define			MULTI_THREAT_MODE		1
void LC_ServerMapLogic_SoloDulePlay::InitBattle( LC_ServerPlayer* pPlayerAController, LC_ServerPlayer* pPlayerBController )
{
	InitHatred(pPlayerAController,pPlayerBController );
	InitHatred(pPlayerBController,pPlayerAController );
	pPlayerAController->SetDenyMove(false);
	pPlayerBController->SetDenyMove(false);
}



void LC_ServerMapLogic_SoloDulePlay::InitBattle_Orig( LC_ServerPlayer* challengePlayer, LC_ServerPlayer* bechallengePlayer )
{
	float fCurrent = GET_PROCESS_TIME;

	mem::vector<int32_t> vecBuddyIDsA;
	challengePlayer->GetActivateBuddyIDs( vecBuddyIDsA);
	mem::vector<int32_t> vecBuddyIDsB;
	bechallengePlayer->GetActivateBuddyIDs( vecBuddyIDsB);

	LC_ServerPlayer* pPlayerAController = challengePlayer->GetOwnerPlayer();
	LC_ServerPlayer* pPlayerBController = bechallengePlayer->GetOwnerPlayer();
	//////////////////////////////////////////////////////////////////////////
	/// 仇恨
	LC_ActorHatredEntry kHatredEntry;
	kHatredEntry.SetHatredValue(10);
	kHatredEntry.SetUpdateTime(GET_PROCESS_TIME);
	for (int i = 0; i < vecBuddyIDsB.size(); i++)
	{
		LC_ServerPlayer* pOther = pPlayerBController->GetActivateBuddyByID(vecBuddyIDsB[i]);
		if (pOther == NULL)
			continue;

		kHatredEntry.SetTargetID(pOther->GetID());
#if defined MULTI_THREAT_MODE
		for (int j = 0; j < vecBuddyIDsA.size(); j++)
		{
			LC_ServerPlayer* pBuddy = pPlayerAController->GetActivateBuddyByID(vecBuddyIDsA[j]);
			if (pBuddy == NULL)
				continue;
			pBuddy->GetHatredList()->AddHatred(kHatredEntry);
			pBuddy->SetDefaultTargetID( pOther->GetID() );
			pBuddy->SetSelectTargetID( pOther->GetID() );
		}
#else
		pCurrentA->GetHatredList()->AddHatred(kHatredEntry);
#endif
	}

	//////////////////////////////////////////////////////////////////////////

	for (int i = 0; i < vecBuddyIDsA.size(); i++)
	{
		LC_ServerPlayer* pOther = pPlayerAController->GetActivateBuddyByID(vecBuddyIDsA[i]);
		if (pOther == NULL)
			continue;
		// 竞技场bug临时测试代码
		//if (i == 0)
		//	pOther->SetHP(1);

		kHatredEntry.SetTargetID(pOther->GetID());
#if defined MULTI_THREAT_MODE
		for (int j = 0; j < vecBuddyIDsB.size(); j++)
		{
			LC_ServerPlayer* pBuddy = pPlayerBController->GetActivateBuddyByID(vecBuddyIDsB[j]);
			if (pBuddy == NULL)
				continue;
			pBuddy->GetHatredList()->AddHatred(kHatredEntry);
			pBuddy->SetDefaultTargetID( pOther->GetID() );
			pBuddy->SetSelectTargetID( pOther->GetID() );
		}
#else
		pCurrentB->GetHatredList()->AddHatred(kHatredEntry);
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	//给两人增加仇恨
	LC_ServerPlayer* pCurrentA = challengePlayer->GetCurrentActivateBuddy();
	LC_ServerPlayer* pCurrentB = bechallengePlayer->GetCurrentActivateBuddy();
	kHatredEntry.SetTargetID(pCurrentB->GetID());
	kHatredEntry.SetHatredValue(10);
	kHatredEntry.SetUpdateTime(GET_PROCESS_TIME);
	pCurrentA->GetHatredList()->AddHatred(kHatredEntry);

	challengePlayer->SetSoloDulePlay(true);
	challengePlayer->SetDenyMove(false);

	kHatredEntry.SetTargetID(pCurrentA->GetID());
	pCurrentB->GetHatredList()->AddHatred(kHatredEntry);
	pCurrentB->SetDead(false);
	pCurrentB->SetDying(false);

	bechallengePlayer->SetSoloDulePlay(true);
	bechallengePlayer->SetDenyMove(false);
}

void LC_ServerMapLogic_SoloDulePlay::Settle_Arena()
{
	//先获取 挑战者、被挑战者信息
	LC_ServerPlayer* challengePlayer=GetPlace()->GetActorManger().GetPlayer(m_ChallengePlayerID);
	LC_ServerPlayer* bechallengePlayer=m_BeChallengePlayer;

	IF_A_NA_PLAYER(challengePlayer)
		return;
	IF_A_NA_PLAYER(bechallengePlayer)
		return;
	LC_RankManager* rank_manager=SERVER_GET_RANK_MANAGER();
	LC_Dule_Manager* dule_Manager = SERVER_GET_DULE_MANAGER();
	Protocol::PS_Rank_User_Info baseinfo;
	rank_manager->FillPlayerData(bechallengePlayer, baseinfo);

	//发奖、结算消息、上报
	//发放奖励
	CF_RankReward* pkCSVRankList = CF_RankReward::GetSingletonPtr();
	bool result=false;
	int resultint=0;
	attr_value_type exp=0;
	MoneyType money=0;
	attr_value_type sp=0;
	attr_value_type courage=0;
	UT_SIMDataList materials;
	int challengeindex=NONE_INDEX;
	int entryIndex = NONE_INDEX;
	int expItemCount = 0;

	int32_t level = challengePlayer->GetLevel();
	CF_EXPList::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_EXPList>(level);
	if(NULL == pkData)
	{
		return;
	}

	if (GetPlace()->GetFactionResult(FACTION_PLAYER)==FACTION_RESULT_SUCESS)
	{
		//（等级*5+20）*系数
		int rankindex=NONE_INDEX;
		rankindex=challengePlayer->GetArenaBattleAssert()->GetLocalIndex();
		challengeindex=rankindex;
		if (rankindex==NONE_INDEX)
		{
			rankindex=m_BeChallengeIndex;
		}
		else if (rankindex>m_BeChallengeIndex)
		{
			rankindex=m_BeChallengeIndex;
		}

		CF_RankReward::DataEntry* pkEntry = NULL;
		if (rankindex==NONE_INDEX)
		{
			pkEntry=pkCSVRankList->GetEntryPtr(1001);
		}
		else
		{
			pkEntry=pkCSVRankList->GetEntryPtr(rankindex+1);
		}
		if (pkEntry==NULL)
		{
			pkEntry=pkCSVRankList->GetEntryPtr(1001);
		}

		if (pkEntry!=NULL)
		{
			entryIndex=pkEntry->_Rank;
			result=true;
			resultint=1;

			//经验 金币等奖励去除上述公式，只用系数*explist中对应指挥官等级的数值
			exp = pkData->_lBaseBuddyXP * pkEntry->_SucessExpParm;
			money = pkData->_lBaseCash * pkEntry->_SucessMoneyParm;
			sp=(attr_value_type)pkEntry->_SucessSP;
			courage=(attr_value_type)pkEntry->_SucessCourage;
			materials = pkEntry->_SucessMaterial;
			expItemCount = pkData->_lBaseBuddyXP * pkEntry->_WinExp;
		}
	}
	else
	{
		int rankindex = NONE_INDEX;
		rankindex=challengePlayer->GetArenaBattleAssert()->GetLocalIndex();
		challengeindex=rankindex;
		CF_RankReward::DataEntry* pkEntry = NULL;
		if (rankindex==NONE_INDEX)
		{
			pkEntry=pkCSVRankList->GetEntryPtr(1001);
		}
		else
		{
			pkEntry=pkCSVRankList->GetEntryPtr(rankindex+1);
		}
		if (pkEntry==NULL)
		{
			pkEntry=pkCSVRankList->GetEntryPtr(1001);
		}
		if (pkEntry!=NULL)
		{
			entryIndex=pkEntry->_Rank;
			result=false;
			resultint=0;

			exp = pkData->_lBaseBuddyXP * pkEntry->_ErrExpParm;
			money = pkData->_lBaseCash * pkEntry->_ErrMoneyParm;
			sp=(attr_value_type)pkEntry->_ErrSP;
			courage=(attr_value_type)pkEntry->_ErrCourage;
			materials = pkEntry->_ErrMaterial;
			expItemCount = pkData->_lBaseBuddyXP * pkEntry->_FailExp;
		}
	}
	//竞技场挑战--埋点日志
	vector<int> nParams;
	nParams.clear();
	nParams.push_back(result);
	int nLogCode = LC_Helper::SerializeOperateLogCode(FUBEN_LOGTYPE, FUBEN11_OPERATE, OPERATE_LOGTYPE1);
	//发奖
	challengePlayer->AddCash(money, CT_UNBIND_CASH, SYS_FUNC_TP_RANK_SOLODULE, bechallengePlayer->GetID(), 0, true, NULL, nLogCode, nParams);
	challengePlayer->AddExpWithGamePlay(exp, SYS_FUNC_TP_RANK_SOLODULE, bechallengePlayer->GetID());
	RecordCashOperateLog(challengePlayer, nLogCode, GET_OPERATE, CT_USER_EXP, exp, -1, nParams);
	challengePlayer->AddSkillLevelupPoints(sp, SYS_FUNC_TP_RANK_SOLODULE, bechallengePlayer->GetID());
	challengePlayer->AddCouragePoint(courage, SYS_FUNC_TP_RANK_SOLODULE, bechallengePlayer->GetID(), nLogCode, nParams);
	if (materials.size() > 0) {
		UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, materials, challengePlayer, SYS_FUNC_TP_RANK_SOLODULE,
			0, "190;", "191;", true, false, false, nLogCode, nParams);
	}

	//给玩家的能源道具
	if (expItemCount > 0) {
		Utility::UT_SIMDataList kItems;
		Utility::UT_SIMDataInfo kItem;
		kItem.SetItemInfo(SPEC_ITEM_TID_BUDDY_EXP, expItemCount, 0);
		kItems.push_back(kItem);
		UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, kItems, challengePlayer, SYS_FUNC_TP_RANK_SOLODULE,
			0, "190;", "191;", true, false, false, nLogCode, nParams);
	}

	GetPlace()->NotifyGameEventToClient(challengePlayer,EVENT_TYPE_SOLODULE_SETTLE,exp,money,resultint,sp,courage, entryIndex);

	GetPlace()->NotifyGameEventToClient(challengePlayer,EVENT_TYPE_FIGHT_RECORD_END,m_isolofightuniqueId,challengePlayer->GetUserID());

	UpLoadSoloDulePlayInfoToGroup(challengePlayer,bechallengePlayer,resultint);
	
	const unique_id_impl& instance = challengePlayer->GetInstance();
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "EVENT_TYPE_SOLODULE_SETTLE result:%d, serverid:%d, c_cid:%d, c_citizenship:%d, c_uid:%d, MapResId:%d, MapLogicID:%d\n", 
		result, GetGameServerID(), instance.detail(), instance.catagory(), challengePlayer->GetUserID(), challengePlayer->GetMapResID(), challengePlayer->GetMapLogicID());
	oss_role_arena(challengePlayer,challengeindex,bechallengePlayer,m_BeChallengeIndex,result?1:0,exp,money);
	//上报
	dule_Manager->GetSoloDuleManager(GameLogic::eSoloDuleType_Arena)->SetChallengeResult_Arena(challengePlayer, bechallengePlayer->GetInstance(), baseinfo,result);
	// 恢复SM_Controller
	mem::vector<int32_t> vecBuddyIDsA;
	challengePlayer->GetActivateBuddyIDs( vecBuddyIDsA);
	challengePlayer->SetAIControl(false);
	challengePlayer->SetDenyMove(false);
	for (int i = 0; i < vecBuddyIDsA.size(); i++)
	{
		LC_ServerPlayer* pBuddy = challengePlayer->GetActivateBuddyByID(vecBuddyIDsA[i]);
		if (pBuddy == NULL)
			continue;
		pBuddy->SetSMController( StateMachine::SCT_SERVER_BUDDY );
	}

}

void LC_ServerMapLogic_SoloDulePlay::PreSettle_GuildOffline()
{
	//先获取 挑战者、被挑战者信息
	LC_ServerPlayer* challengePlayer=GetPlace()->GetActorManger().GetPlayer(m_ChallengePlayerID);
	LC_ServerPlayer* bechallengePlayer=m_BeChallengePlayer;
	if (NULL==challengePlayer || NULL==bechallengePlayer)
		return;
	// 带上win_id lose_id 去db 结算分数。异步回来 调用 Settle_GuildOffline 
	// 被挑战者 赢了， 向db 请求 改写 被挑战者分数 直接调用 Settle_GuildOffline
	SingleFightRecord singleFightRecordInfo;
	singleFightRecordInfo.m_nTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	challengePlayer->PackSingleSideStruct(singleFightRecordInfo.m_AttackInfo);
	bechallengePlayer->PackSingleSideStruct(singleFightRecordInfo.m_DefenceInfo);
	challengePlayer->SetBestGuildSettleAccountPoint(0);
	bechallengePlayer->SetBestGuildSettleAccountPoint(0);
	std::vector<uint64_t> vecId;
	if(bechallengePlayer->CheckOwnerDead())
	{
		vecId.push_back(challengePlayer->GetInstance());
		vecId.push_back(bechallengePlayer->GetInstance());
		UT_ServerHelper::gCacheGuildDefendMap[challengePlayer->GetInstance()] = singleFightRecordInfo;
	}
	else
	{
		challengePlayer->SetLastAccountPointRankIndexByTypeAndStoryId(GuildFightRankType::GFRT_PERSONAL,ROIT_Total);
		challengePlayer->SetLastAccountPointRankIndexByTypeAndStoryId(GuildFightRankType::GFRT_GUILD,ROIT_Total);
		challengePlayer->SetCurrentAccountPointRankIndexByTypeAndStoryId(GuildFightRankType::GFRT_PERSONAL,ROIT_Total);
		challengePlayer->SetCurrentAccountPointRankIndexByTypeAndStoryId(GuildFightRankType::GFRT_GUILD,ROIT_Total);
		singleFightRecordInfo.m_nScore = 0;

		MG_SS_AddGuildDefendLog msg;
		msg.m_serverID = GSNodeNetwork::Instance().getEntryIdByMergeId(GetGameServerID());
		msg.m_charID = bechallengePlayer->GetInstance();
		msg.m_logRecord = singleFightRecordInfo;
		SendMsgToSociety(MGPT_SS_ADDGUILDDEFENDLOG, &msg);
		UT_ServerHelper::BroadcastBeFightedToPlayer(bechallengePlayer->GetInstance());
		Settle_GuildOffline();
		return;
		/*vecId.push_back(bechallengePlayer->GetInstance());
		vecId.push_back(challengePlayer->GetInstance());*/
	}
	challengePlayer->SetLastAccountPointRankIndexByTypeAndStoryId(GuildFightRankType::GFRT_PERSONAL,ROIT_Total);
	UT_ServerHelper::ReqPlayerOfflineToDb(OfflinePlayerFromDBType::OPFDT_GUILD_FIGHT_SETTLE_ACCOUNTS,vecId,0,0,0,0,0);
}

void LC_ServerMapLogic_SoloDulePlay::Settle_GuildOffline()
{
	//先获取 挑战者、被挑战者信息
	LC_ServerPlayer* challengePlayer=GetPlace()->GetActorManger().GetPlayer(m_ChallengePlayerID);
	LC_ServerPlayer* bechallengePlayer=m_BeChallengePlayer;
	
	IF_A_NA_PLAYER(challengePlayer)
		return;
	IF_A_NA_PLAYER(bechallengePlayer)
		return;
	bool result = false;
	if (GetPlace()->GetFactionResult(FACTION_PLAYER)==FACTION_RESULT_SUCESS)
	{
		result = true;
	}

	LC_Dule_Manager* dule_Manager = SERVER_GET_DULE_MANAGER();
	dule_Manager->GetSoloDuleManager(GameLogic::eSoloDuleType_GuildOffline)->SetChallengeResult_GuildOffline(challengePlayer, bechallengePlayer->GetInstance(), bechallengePlayer->GetGuildID(), bechallengePlayer->GetOwnerCharName(), result);
	
	// 恢复SM_Controllerw
	mem::vector<int32_t> vecBuddyIDsA;
	challengePlayer->GetActivateBuddyIDs( vecBuddyIDsA);
	challengePlayer->SetAIControl(false);
	challengePlayer->SetDenyMove(false);
	for (int i = 0; i < vecBuddyIDsA.size(); i++)
	{
		LC_ServerPlayer* pBuddy = challengePlayer->GetActivateBuddyByID(vecBuddyIDsA[i]);
		if (pBuddy == NULL)
			continue;
		pBuddy->SetSMController( StateMachine::SCT_SERVER_BUDDY );
	}
}


void LC_ServerMapLogic_SoloDulePlay::BattleResult()
{
	LC_ServerPlayer* challengePlayer=GetPlace()->GetActorManger().GetPlayer(m_ChallengePlayerID);
	LC_ServerPlayer* bechallengePlayer=m_BeChallengePlayer;

	//在规定时间内未挑战成功，则挑战失败
	GetPlace()->SetFactionResult(FACTION_PLAYER,FACTION_RESULT_FAIL);
	GetPlace()->SetFactionResult(FACTION_EVIL,FACTION_RESULT_SUCESS);
	End();
	return;
}

void LC_ServerMapLogic_SoloDulePlay::UpLoadSoloDulePlayInfoToGroup(LC_ServerPlayer* challengePlayer,LC_ServerPlayer* bechallengePlayer,int resultint)
{
	LC_Dule_Manager* dule_Manager = SERVER_GET_DULE_MANAGER();
	LC_Solo_Duel_Manager* pResult =dule_Manager->GetSoloDuleManager(GameLogic::eSoloDuleType_Arena);
	if(NULL == pResult)
	{
		return ;
	}
	Solo_Duel_Info* pDuelinfo = pResult->GetSoloDuelInfo(challengePlayer->GetInstance());
	if(NULL == pDuelinfo)
	{
		return ;
	}
	
	
	LC_RankManager* rank_manager=SERVER_GET_RANK_MANAGER();
	
	MG_GG_REQ_SoloFightReslut reqMsg;
	rank_manager->FillRankPlayerBaseData(challengePlayer, reqMsg.m_solo_result.m_FightData.m_nLeftUserBaseData);
	rank_manager->FillRankPlayerBaseData(bechallengePlayer, reqMsg.m_solo_result.m_FightData.m_nRightUserBaseData);



	BaseBattlefieldReport singleReportInfo;
	mem::vector<int> challengeBuddyIDVec;
	challengePlayer->GetBuddySchemeMgr().GetBuddySchemeData(challengePlayer, SOLO_DULE_RES_ID, 1, SchemeType_Offensive, challengeBuddyIDVec);
	challengePlayer->PackBaseBattlefieldBuddyDataToVec(challengeBuddyIDVec,singleReportInfo.m_nLeftBuddyData.m_nBuddyData);
	singleReportInfo.m_nLeftBuddyData.m_nResult = resultint?true:false;
	
	mem::vector<int> bechallengeBuddyIDVec;
	bechallengePlayer->GetBuddySchemeMgr().GetBuddySchemeData(bechallengePlayer, SOLO_DULE_RES_ID, 1, SchemeType_Defence, bechallengeBuddyIDVec);
	bechallengePlayer->PackBaseBattlefieldBuddyDataToVec(bechallengeBuddyIDVec,singleReportInfo.m_nRightBuddyData.m_nBuddyData);
	singleReportInfo.m_nRightBuddyData.m_nResult = resultint?false:true;
	singleReportInfo.m_nRepalyId = m_isolofightuniqueId; //本场战斗回放id
	reqMsg.m_solo_result.m_FightBuddyData.push_back(singleReportInfo);
	reqMsg.m_solo_result.m_nResult = resultint?(challengePlayer->GetInstance()):(bechallengePlayer->GetInstance());
	reqMsg.m_solo_result.m_nUniqueId = m_isolofightuniqueId;// 总战斗id group 生成 
	reqMsg.m_solo_result.m_nAreanType = Arena_Type_Challenge;// 结果 0：Arena_Type_Challenge  1：Arena_Type_Firmament 2:Arena_Type_Peak
	
	SendMsgToGroup(MGPT_GG_SoloFightResult, &reqMsg, challengePlayer->GetUserID(), challengePlayer->GetInstance());
	
}

void LC_ServerMapLogic_SoloDulePlay::SummonPlayerFinish(LC_ServerPlayer* player)
{
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerMapLogic_Place* place=(LC_ServerMapLogic_Place*)pkWorldManager->GetMapLogic(player->GetMapLogicID());
	if (place==NULL)
	{
		return;
	}
	LC_ServerMapLogic_SoloDulePlay* play=(LC_ServerMapLogic_SoloDulePlay*)place->GetPlay(PlayType_SoloDule);
	if (play==NULL)
	{
		return;
	}
	char_data_define::save_skill_state_info info;
	player->GetStateMap()->Clear_SaveAdditionalStateToPB(info);
	play->CountDown();
}

