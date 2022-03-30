#include "LC_StoryLogic_Manager.h"
#include "LC_ServerMap.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerLogicManager.h"

#include "LC_GameEvent.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerPlayerGroup.h"
#include "CF_WorldMapList.h"
#include "LC_ServerMap.h"

#include "MG_Game2DB.h"
#include "GameServerApp.h"
#include <time.h>

#include "LC_GameStory_WildBoss.h"
#include "LC_GameStory_Transcript.h"
#include "LC_GameStory_Escort.h"
#include "LC_GameStory_FactionWar.h"
#include "LC_GameStory_GuildLadderWar.h"
#include "LC_GameStory_Manager.h"

#include "CF_BossInvade.h"
#include "CF_BossInvadeRankReward.h"
#include "CF_BuddyThemeBossType.h"

using namespace GameLogic;
using namespace GameStory;
using namespace Skill;
using namespace Protocol;
using namespace CSVFile;
using namespace Utility;


int32_t LC_StoryLogic_Manager::m_stBaseLogicID = 0;

//---------------------------------------------
LC_StoryLogic_Manager::LC_StoryLogic_Manager()
{
}

//---------------------------------------------
LC_StoryLogic_Manager::~LC_StoryLogic_Manager(){}

//---------------------------------------------
void LC_StoryLogic_Manager::Init()
{
	m_kETP2CSTPMap[GLET_EXP_CHANGED] = GS_CMN_STAT_TP_EXP;
	m_kETP2CSTPMap[GLET_ADD_CASH] = GS_CMN_STAT_TP_CASH;
	m_kETP2CSTPMap[GLET_SP_ADD] = GS_CMN_STAT_TP_SP;

	m_kUpdateTimer.SetTimer(GET_INTERVAL(CMN_INTER_LAZY));
}

//----------------------------------------------
void LC_StoryLogic_Manager::UnInit()
{
	GameStoryLogicMapIter pkMapIter = m_kStoryLogicMap.begin();
	for(; pkMapIter!= m_kStoryLogicMap.end();++pkMapIter)
	{
		LC_Server_GameStory_Base* pkStoryBase = pkMapIter->second;
		T_SAFE_DELETE (pkStoryBase);
	}

	m_kStoryLogicMap.clear();
}

LC_Server_GameStory_Base* LC_StoryLogic_Manager::GetStoryLogic(int logicID)
{
	GameStoryLogicMapIter it = m_kStoryLogicMap.find(logicID);
	return (it != m_kStoryLogicMap.end()) ? it->second : NULL;
}

LC_Server_GameStory_Base *LC_StoryLogic_Manager::GetLCStoryImpl(GameStoryRecord &record)
{	
	LC_Server_GameStory_Base* pkStoryBase = GetStoryLogic(record.m_iStoryLogicId);
	if (pkStoryBase)
	{
		return pkStoryBase;
	}
	switch (record.m_eContentType)
	{
	case GS_CON_TP_WILD_MOP_UP:
		pkStoryBase = T_NEW_D LC_Server_GameStory_WildBoss(AutoLogicID(), &record);
		break;
	case GS_CON_TP_TANSCRIPTION:
		pkStoryBase = T_NEW_D LC_GameStory_Transcript(AutoLogicID(), &record);
		break;
	case GS_CON_TP_ESCORT:
		pkStoryBase = T_NEW_D LC_GameStory_Escort(AutoLogicID(), &record);
		break;
	case GS_CON_TP_FACTION_WAR:
		pkStoryBase = T_NEW_D LC_GameStory_FactionWar(AutoLogicID(), &record);
		break;
	case  GS_CON_TP_GUILD_LADDER_WAR:
		pkStoryBase = T_NEW_D LC_GameStory_GuildLadderWar(AutoLogicID(), &record);
		break;
	default:
		break;
	}

	if (pkStoryBase)
	{
		m_kStoryLogicMap[pkStoryBase->GetLogicID()] = pkStoryBase;
	}

	return pkStoryBase;
}

GameStoryCommonStatType LC_StoryLogic_Manager::GetCommonStatType(GameEventType eEventType) const
{
	GameStoryETP2CSTPMap::const_iterator cit = m_kETP2CSTPMap.find(eEventType);
	return (cit != m_kETP2CSTPMap.end()) ? cit->second : GS_CMN_STAT_TP_NA;
}

//--------------------------------------------------------------------------
void LC_StoryLogic_Manager::Update(float fCurrentTime,float fDeltaTime)
{
	if (!m_kUpdateTimer.IsExpired())
	{
		return;
	}
	GameStoryLogicMapIter itLogic = m_kStoryLogicMap.begin();
	for(; itLogic!= m_kStoryLogicMap.end();++itLogic)
	{
		LC_Server_GameStory_Base* pkStoryBase = itLogic->second;
		{
			pkStoryBase->Update(fCurrentTime, fDeltaTime);
		}
	}	
}

void LC_StoryLogic_Manager::CheckReset(uint32_t timestamp)
{
	GameStoryLogicMapIter itLogic = m_kStoryLogicMap.begin();
	for(; itLogic!= m_kStoryLogicMap.end();++itLogic)
	{
		LC_Server_GameStory_Base* pkStoryBase = itLogic->second;
		{
			pkStoryBase->CheckReset(timestamp);
		}
	}
}

BOOL LC_StoryLogic_Manager::ActivateStory(GameStoryRecord &record, bool bRepeated)
{
	GameStoryLogicMapIter itLogic = m_kStoryLogicMap.find(record.m_iStoryLogicId);

	if(itLogic != m_kStoryLogicMap.end())
	{
		LC_Server_GameStory_Base* pkStoryBase = itLogic->second;
		{
			pkStoryBase->SetRepeated(bRepeated);
			pkStoryBase->ActivateStory();	

			return TRUE;
		}
	}
	
	return FALSE;
}

BOOL LC_StoryLogic_Manager::DeactiveStory(int logicID)
{
	GameStoryLogicMapIter itLogic = m_kStoryLogicMap.find(logicID);
	if(itLogic != m_kStoryLogicMap.end())
	{
		LC_Server_GameStory_Base* pkStoryBase = itLogic->second;
		{
			pkStoryBase->DeactivateStory();
		}
		return TRUE;
	}
	return FALSE;
}

void LC_StoryLogic_Manager::CheckUpdateTask(LC_ServerPlayer *pkPlayer)
{
	GameStoryLogicMapIter it = m_kStoryLogicMap.begin();

	for (; it!=m_kStoryLogicMap.end(); ++it)
	{
		it->second->CheckUpdateTask(pkPlayer);
	}
}

//-------------------------------------------------------------------------------
void LC_StoryLogic_Manager::ProcessGameEvent(GameLogic::LC_GameEvent* pkEvent)
{
	if (LC_Server_GameStory_Base::_bCareOfEvent(pkEvent) == false)
	{
		return;
	}
	object_id_type lID = (object_id_type)pkEvent->GetEventParam(EVENT_BASE_PARAM_1);
	LC_PlayerManagerBase* pkPlayerManager = ENGINE_GET_GAMEPLAYER_MANAGER();

	LC_PlayerBase* pkPlayer = pkPlayerManager->FindPlayer(lID);
	if (NULL == pkPlayer)
	{
		return;
	}
	GameStoryLogicMapIter iter = m_kStoryLogicMap.begin();
	for(; iter != m_kStoryLogicMap.end(); ++iter)
	{
		LC_Server_GameStory_Base* pkBase = iter->second;
		if(pkBase->IsActive())
		{
			pkBase->ProcessGameEvent(pkEvent, this, pkPlayer);
		}
	}
}

//--------------------------------------------------------------------------------
void LC_StoryLogic_Manager::PostEnterGame(GameLogic::LC_PlayerBase *pkPlayer)
{
	LC_ServerPlayer* pkOwnerPlayer = ((LC_ServerPlayer*)pkPlayer)->GetOwnerPlayer();
	if(NULL == pkOwnerPlayer)
	{
		return;
	}

	int32_t enterMode = pkOwnerPlayer->GetEnterServerMode();
	bool enter = bool(enterMode == eEnterServerMode_Normal);
	const unique_id_impl& instance = pkOwnerPlayer->GetInstance();

	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "PostEnterGame serverid:%d, c_cid:%d, c_citizenship:%d, c_uid:%d, id:%d, enterMode:%d, enter:%d", 
		GetGameServerID(), instance.detail(), instance.catagory(), pkOwnerPlayer->GetUserID(), pkPlayer->GetID(), enterMode, enter);

	GameStoryLogicMapIter iter = m_kStoryLogicMap.begin();
	for(; iter != m_kStoryLogicMap.end(); ++iter)
	{
		LC_Server_GameStory_Base* pkBase = iter->second;
		if(pkBase->IsActive())
		{
			if(enter)
			{
				//真正进入游戏的时候，才发活动公告
				pkBase->NotifyUserStoryStart((LC_ServerPlayer*)pkPlayer);
			}
			
			pkBase->NotifyPlayerGameStoryBatch(GS_SCHEDULE_KEY_COMMON, (LC_ServerPlayer*)pkPlayer);
		}
	}

}
//-------------------------------------------------------------------------------
void LC_StoryLogic_Manager::ActorBeKilled(GameLogic::LC_ActorBase* pkKiller, GameLogic::LC_ActorBase*  pkInjured)
{
	GameStoryLogicMapIter iter = m_kStoryLogicMap.begin();
	for(; iter != m_kStoryLogicMap.end(); ++iter)
	{
		LC_Server_GameStory_Base* pkBase = iter->second;
		if(pkBase->IsActive())
		{
			pkBase->ActorBeKilled(pkKiller, pkInjured);
		}
	}
}
//-------------------------------------------------------------------------------
void LC_StoryLogic_Manager::ActorBeDamaged(GameLogic::LC_ActorBase* pkAttacker, GameLogic::LC_ActorBase* pkInjured, int32_t iMapLogicID, attr_value_type dmg)
{
	GameStoryLogicMapIter iter = m_kStoryLogicMap.begin();
	for(; iter != m_kStoryLogicMap.end(); ++iter)
	{
		LC_Server_GameStory_Base* pkBase = iter->second;
		if(pkBase->IsActive())
		{
			pkBase->ActorBeDamaged(pkAttacker, pkInjured, iMapLogicID, dmg);
		}
	}
}
void LC_StoryLogic_Manager::AddFactionScore(GameLogic::LC_ActorBase* pkKiller, GameLogic::LC_ActorBase* pkInjured, int32_t iMapLogicID, int32_t iType , int32_t iVal)
{
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();

	GameStoryLogicMapIter iter = m_kStoryLogicMap.begin();
	for (; iter != m_kStoryLogicMap.end(); ++iter)
	{
		LC_Server_GameStory_Base* pkBase = iter->second;
		if (pkBase->IsActive())
		{
			LC_ServerMapLogic_Place* pkPlace = pkWorldManager->GetMapLogic(iMapLogicID);
			if (NULL == pkPlace)
			{
				continue;
			}
			GameStoryConfig* pkConfig = pkBase->GetConfig();
			if (NULL == pkConfig)
			{
				continue;
			}
			int32_t iMapResID = pkPlace->GetMapResID();
			const MapSetting* pkMapSetting = pkConfig->m_kMapCtrlSetting.GetMapSetting(iMapResID);
			if (NULL == pkMapSetting)
			{
				continue;
			}
			pkBase->AddFactionScore((GameStoryScheduleKey)iMapLogicID, iType, iVal, pkKiller, pkInjured);
		}
	}
}
void LC_StoryLogic_Manager::AddActorScore(GameLogic::LC_ActorBase* pkKiller, GameLogic::LC_ActorBase* pkInjured, int32_t iMapLogicID, int32_t iVal)
{
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();

	GameStoryLogicMapIter iter = m_kStoryLogicMap.begin();
	for (; iter != m_kStoryLogicMap.end(); ++iter)
	{
		LC_Server_GameStory_Base* pkBase = iter->second;
		if (pkBase->IsActive())
		{
			LC_ServerMapLogic_Place* pkPlace = pkWorldManager->GetMapLogic(iMapLogicID);
			if (NULL == pkPlace)
			{
				continue;
			}
			GameStoryConfig* pkConfig = pkBase->GetConfig();
			if (NULL == pkConfig)
			{
				continue;
			}
			int32_t iMapResID = pkPlace->GetMapResID();
			const MapSetting* pkMapSetting = pkConfig->m_kMapCtrlSetting.GetMapSetting(iMapResID);
			if (NULL == pkMapSetting)
			{
				continue;
			}
			pkBase->AddActorScore((GameStoryScheduleKey)iMapLogicID, iVal, pkKiller, pkInjured);
		}
	}
}

void LC_StoryLogic_Manager::FactionOccupation(GameLogic::LC_ActorBase* pkPlayer, GameLogic::LC_ActorBase* pkBeOccupied)
{
	pkPlayer = ((LC_ServerPlayer*)pkPlayer)->GetOwnerPlayer();
	if (NULL == pkPlayer || NULL == pkBeOccupied)
	{
		return;
	}
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();

	GameStoryLogicMapIter iter = m_kStoryLogicMap.begin();
	for (; iter != m_kStoryLogicMap.end(); ++iter)
	{
		LC_Server_GameStory_Base* pkBase = iter->second;
		if (pkBase->IsActive())
		{
			LC_ServerMapLogic_Place* pkPlace = pkWorldManager->GetMapLogic(pkPlayer->GetMapLogicID());
			if (NULL == pkPlace)
			{
				continue;
			}
			GameStoryConfig* pkConfig = pkBase->GetConfig();
			if (NULL == pkConfig)
			{
				continue;
			}
			int32_t iMapResID = pkPlace->GetMapResID();
			const MapSetting* pkMapSetting = pkConfig->m_kMapCtrlSetting.GetMapSetting(iMapResID);
			if (NULL == pkMapSetting)
			{
				continue;
			}
			pkBase->FactionOccupation((GameStoryScheduleKey)pkPlayer->GetMapLogicID(), pkPlayer, pkBeOccupied);
		}
	}
}

void LC_StoryLogic_Manager::TranscriptionEntered(LC_ServerMapLogic_Place* pkPlace, LC_ServerPlayer *pkPlayer)
{
	int32_t storyID = pkPlace->GetStoryID();

	LC_GameStory_Manager *pkGSMgr = SERVER_GET_GAMESTORY_MANAGER;
	LC_Server_GameStory_Base* pkStory = pkGSMgr->GetGameStoryBasePtrByStoryID(storyID);
	if(NULL == pkStory || !pkStory->IsActive())
	{
		return;
	}

	pkStory->DoTranscriptionEntered(pkPlace, pkPlayer);
}
void LC_StoryLogic_Manager::TranscriptionFinished(LC_ServerMapLogic_Place* pkPlace, GSTranscriptResult &trans)
{
	int32_t storyID = pkPlace->GetStoryID();

	LC_GameStory_Manager *pkGSMgr = SERVER_GET_GAMESTORY_MANAGER;
	LC_Server_GameStory_Base* pkStory = pkGSMgr->GetGameStoryBasePtrByStoryID(storyID);
	if(NULL == pkStory || !pkStory->IsActive())
	{
		return;
	}

	pkStory->DoTranscriptionFinished(pkPlace, trans);
}

void LC_StoryLogic_Manager::OnAcceptTask(GameLogic::LC_PlayerBase *pkPlayer, GameLogic::LC_Task* pkTask)
{
	GameStoryLogicMapIter iter = m_kStoryLogicMap.begin();
	for(; iter != m_kStoryLogicMap.end(); ++iter)
	{
		LC_Server_GameStory_Base* pkBase = iter->second;
		if(pkBase->IsActive())
		{
			pkBase->OnAcceptTask(pkPlayer, pkTask);
		}
	}
}
void LC_StoryLogic_Manager::OnFinishTask(GameLogic::LC_PlayerBase *pkPlayer, GameLogic::LC_Task* pkTask)
{
	GameStoryLogicMapIter iter = m_kStoryLogicMap.begin();
	for(; iter != m_kStoryLogicMap.end(); ++iter)
	{
		LC_Server_GameStory_Base* pkBase = iter->second;
		if(pkBase->IsActive())
		{
			pkBase->OnFinishTask(pkPlayer, pkTask);
		}
	}
}
void LC_StoryLogic_Manager::OnCancelTask(GameLogic::LC_PlayerBase *pkPlayer, GameLogic::LC_Task* pkTask)
{
	GameStoryLogicMapIter iter = m_kStoryLogicMap.begin();
	for(; iter != m_kStoryLogicMap.end(); ++iter)
	{
		LC_Server_GameStory_Base* pkBase = iter->second;
		if(pkBase->IsActive())
		{
			pkBase->OnCancelTask(pkPlayer, pkTask);
		}
	}
}

void LC_StoryLogic_Manager::PreMapDestoryed(LC_ServerMapLogicBase* pkMapLogic)
{
	GameStoryLogicMapIter iter = m_kStoryLogicMap.begin();
	for(; iter != m_kStoryLogicMap.end(); ++iter)
	{
		LC_Server_GameStory_Base* pkBase = iter->second;
		if(pkBase->IsActive())
		{
			pkBase->PreMapDestoryed(pkMapLogic);
		}
	}
}
void LC_StoryLogic_Manager::PostMapCreated(LC_ServerMapLogicBase* pkMapLogic, int iAutoFitLevel)
{
	if(NULL == pkMapLogic)
	{
		return;
	}

	LC_GameStory_Manager *pkGSMgr = SERVER_GET_GAMESTORY_MANAGER;
	LC_Server_GameStory_Base* pkBase = pkGSMgr->GetGameStoryBasePtrByStoryID(pkMapLogic->GetStoryID());
	if(NULL != pkBase && pkBase->IsActive())
	{
		pkBase->PostMapCreated(pkMapLogic, iAutoFitLevel);
	}
}
void LC_StoryLogic_Manager::PrePlayerLeaveMap(GameLogic::LC_PlayerBase *pkPlayer, LC_ServerMapLogicBase* pkPlace)
{
	GameStoryLogicMapIter iter = m_kStoryLogicMap.begin();
	for(; iter != m_kStoryLogicMap.end(); ++iter)
	{
		LC_Server_GameStory_Base* pkBase = iter->second;
		if(pkBase->IsActive())
		{
			pkBase->PrePlayerLeaveMap(pkPlayer, pkPlace);
		}
	}
}
void LC_StoryLogic_Manager::PostPlayerEnterMap(GameLogic::LC_PlayerBase *pkPlayer, LC_ServerMapLogicBase* pkPlace)
{
	if(NULL == pkPlace)
	{
		return;
	}

	GameStoryLogicMapIter iter = m_kStoryLogicMap.begin();
	for(; iter != m_kStoryLogicMap.end(); ++iter)
	{
		LC_Server_GameStory_Base* pkBase = iter->second;
		if(pkBase->IsActive())
		{
			pkBase->PostPlayerEnterMap(pkPlayer, pkPlace);
		}
		else if(pkPlace->GetMapLogicType() == MT_GUILD_MAP)
		{
			pkBase->PostPlayerEnterMap(pkPlayer, pkPlace);
		}
	}
}
void LC_StoryLogic_Manager::PlayerLogicAreaChanged(GameLogic::LC_PlayerBase* pkPlayer, LC_ServerMapLogicBase* pkPlace, int32_t iPre)
{
	GameStoryLogicMapIter iter = m_kStoryLogicMap.begin();
	for(; iter != m_kStoryLogicMap.end(); ++iter)
	{
		LC_Server_GameStory_Base* pkBase = iter->second;
		if(pkBase->IsActive())
		{
			pkBase->PlayerLogicAreaChanged(pkPlayer, pkPlace, iPre);
		}
	}
}
void LC_StoryLogic_Manager::NotifyEnterOfflinePlayer(GameLogic::LC_PlayerBase *pkPlayer, LC_ServerMapLogicBase* pkPlace)
{
	GameStoryLogicMapIter iter = m_kStoryLogicMap.begin();
	for(; iter != m_kStoryLogicMap.end(); ++iter)
	{
		LC_Server_GameStory_Base* pkBase = iter->second;
		if(pkBase->IsActive())
		{
			pkBase->NotifyEnterOfflinePlayer(pkPlayer, pkPlace);
		}
	}
}
