#include "LC_GameStory_GuildLadderWar.h"
#include "LC_GameStoryDataDefine.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerLogicManager.h"
#include "LC_NPCBase.h"
#include "LC_GuildLadderManager.h"

using namespace GameLogic;
using namespace GameStory;
using namespace Utility;

LC_GameStory_GuildLadderWar::LC_GameStory_GuildLadderWar(int32_t logicID, GameStoryRecord *pkRecord)
:LC_Server_GameStory_Base(logicID, pkRecord)
{

}

LC_GameStory_GuildLadderWar::~LC_GameStory_GuildLadderWar()
{

}

void LC_GameStory_GuildLadderWar::Init()
{
	LC_Server_GameStory_Base::Init();
}
void LC_GameStory_GuildLadderWar::UnInit()
{
	LC_Server_GameStory_Base::UnInit();
}

void LC_GameStory_GuildLadderWar::Update(float fCurrentTime, float fDeltaTime)
{
	LC_Server_GameStory_Base::Update(fCurrentTime, fDeltaTime);
	static LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	GSGuildLadderInfoMap::iterator iter = m_kAllGuildLadderInfo.begin();
	for (; iter != m_kAllGuildLadderInfo.end(); ++iter)
	{
		GameStoryScheduleKey ikey = iter->first;
		LC_ServerMapLogic_Place* pkPlace = pkWorldManager->GetMapLogic(ikey);
		if (NULL == pkPlace)
		{
			continue;
		}
		GSGuildLadderInfo& kLadderInfo = iter->second;
		if (kLadderInfo.IsEnd())
		{
			continue;
		}
		GSGuildLadderOccupationMap::iterator kOccupationIter = kLadderInfo.m_kOccupation.begin();
		for (; kOccupationIter != kLadderInfo.m_kOccupation.end(); ++kOccupationIter)
		{
			GSGuildLadderOccupationInfo& kData = kOccupationIter->second;
			if (FACTION_NONE == kData.m_iFaction)
			{
				continue;
			}
			if (fCurrentTime - kData.m_iOccupateTime > kData.m_iScoreDeltaTime)
			{
				kData.m_iOccupateTime += kData.m_iScoreDeltaTime;

				//add actor score
				GameLogic::LC_ActorPtrList kPlayers;
				pkPlace->GetPlayersInMap(kPlayers);
				GameLogic::LC_ActorPtrList::iterator kActorIter;
				TRAV(kPlayers, kActorIter)
				{
					AddActorScore(ikey, kData.m_iAddActorScore, *kActorIter, NULL);
				}

				//add factionscore
				_addOccupationFactionScore(ikey, kData.m_iFaction, kData.m_iAddFactionScore, kData.m_iIndexFlag);				
				if (_checkFactionScoreToEnd(ikey, kData.m_iFaction))
				{
					continue;
				}
			}
		}
		_syncWarInfo(ikey, NULL);
	}
}

void LC_GameStory_GuildLadderWar::ActorBeKilled(GameLogic::LC_ActorBase* pkKiller, GameLogic::LC_ActorBase*  pkInjured)
{
	LC_Server_GameStory_Base::ActorBeKilled(pkKiller, pkInjured);
}

GSGuildLadderInfo* LC_GameStory_GuildLadderWar::GetGuildLadderInfoPtr(GameStoryScheduleKey iKey)
{
	if (iKey < GS_SCHEDULE_KEY_MAP_LOGIC)
	{
		return NULL;
	}
	return &m_kAllGuildLadderInfo[iKey];
}

void LC_GameStory_GuildLadderWar::AddFactionScore(GameStoryScheduleKey iKey, int32_t iType, int32_t iVal, GameLogic::LC_ActorBase* pkKiller, GameLogic::LC_ActorBase* pkInjured)
{
	LC_Server_GameStory_Base::AddFactionScore(iKey, iType, iVal, pkKiller, pkInjured);
	int32_t iFaction = FACTION_NONE;
	if (NULL == pkKiller || NULL == pkInjured)
	{
		return;
	}
	iFaction  = pkKiller->GetFactionID();	
	if (iFaction < FACTION_RED || iFaction > FACTION_BLUE)
	{
		return;
	}
	GSGuildLadderInfo* pkGuildLadderInfo = GetGuildLadderInfoPtr(iKey);
	if (NULL == pkGuildLadderInfo)
	{
		return;
	}
	E_GSGuildLaddderScoreType iScoreType = E_GSGuildLaddderScore_None;
	if (IS_PLAYER_CLONE_ID(pkInjured->GetID()))
	{
		iScoreType = E_GSGuildLaddderScore_Kill;
	}
	else
	{
		if (!IS_NPC_ID(pkInjured->GetID()))
		{
			return;
		}
		CF_CharType::DataEntry* pkEntry = pkInjured->GetCharTypePtr();
		if (NULL == pkEntry)
		{
			return;
		}
		iScoreType = (pkEntry->_lWalkSpeed <= 0 ? E_GSGuildLaddderScore_Gather : E_GSGuildLaddderScore_Kill);
	}
	pkGuildLadderInfo->AddFactionScore(iFaction, iVal, iScoreType, 0);
}
void LC_GameStory_GuildLadderWar::_addOccupationFactionScore(GameStoryScheduleKey iKey, int32_t iFaction, int32_t iVal, int32_t iIndexFlag)
{
	LC_Server_GameStory_Base::AddFactionScore(iKey, iFaction, iVal, NULL, NULL);
	if (iFaction < FACTION_RED || iFaction > FACTION_BLUE)
	{
		return;
	}
	GSGuildLadderInfo* pkGuildLadderInfo = GetGuildLadderInfoPtr(iKey);
	if (NULL == pkGuildLadderInfo)
	{
		return;
	}
	pkGuildLadderInfo->AddFactionScore(iFaction, iVal, E_GSGuildLaddderScore_Occupation, iIndexFlag);
}
int32_t LC_GameStory_GuildLadderWar::GetFactionScore(GameStoryScheduleKey iKey, int32_t iType) 
{
	if (iType < FACTION_RED || iType > FACTION_BLUE)
	{
		return 0;
	}
	GSGuildLadderInfo* pkGuildLadderInfo = GetGuildLadderInfoPtr(iKey);
	if (NULL == pkGuildLadderInfo)
	{
		return 0;
	}
	return pkGuildLadderInfo->GetFactionScore(iType);
}

void LC_GameStory_GuildLadderWar::AddActorScore(GameStoryScheduleKey iKey, int32_t iVal, GameLogic::LC_ActorBase* pkKiller, GameLogic::LC_ActorBase* pkInjured)
{
	if (NULL == pkKiller || IS_PLAYER_CLONE_ID(pkKiller->GetID()))
	{
		return;
	}
	GSGuildLadderInfo* pkGuildLadderInfo = GetGuildLadderInfoPtr(iKey);
	if (NULL == pkGuildLadderInfo)
	{
		return;
	}
	if (!pkKiller->GetAsAControlee().IsValid())
	{
		return;
	}
	const unique_id_impl&  charID = pkKiller->GetControlerPeer().CharID();
	pkGuildLadderInfo->AddActorScore(charID, iVal);
	_syncActorScore(iKey, (GameLogic::LC_PlayerBase*)pkKiller);
}

void LC_GameStory_GuildLadderWar::FactionOccupation(GameStoryScheduleKey iKey, GameLogic::LC_ActorBase* pkPlayer, GameLogic::LC_ActorBase* pkBeOccupied)
{
	if (NULL == pkPlayer || NULL == pkBeOccupied)
	{
		return;
	}
	GSGuildLadderInfo* pkGuildLadderInfo = GetGuildLadderInfoPtr(iKey);
	if (NULL == pkGuildLadderInfo)
	{
		return;
	}
	object_id_type lTargetID = pkBeOccupied->GetID();
	if (!IS_NPC_ID(lTargetID))
	{
		return;
	}

	GameStoryConfig* pkConfig = GetConfig();
	if (NULL == pkConfig)
	{
		return;
	}
	NPCRewardInfo& kNPCReward = pkConfig->m_kNPCRewardInfo;
	int32_t iCharTypeID = ((GameLogic::LC_NPCBase*)pkBeOccupied)->GetCharType();
	const NPCReward* pkRewardData = kNPCReward.GetNPCReward(iCharTypeID);
	if (NULL == pkRewardData)
	{
		return;
	}
	if (pkRewardData->kOccupyInfo.size() < 4)
	{
		return;
	}
	int32_t iIndexFlag = pkRewardData->kOccupyInfo[0];
	if (iIndexFlag <= 0)
	{
		return;
	}
	GSGuildLadderOccupationMap& kOccupationMap = pkGuildLadderInfo->m_kOccupation;
	GSGuildLadderOccupationInfo& kOccupationData = kOccupationMap[iIndexFlag];

	//repeated occupation
	if (kOccupationData.m_iFaction == pkPlayer->GetFactionID())
	{
		return;
	}

	kOccupationData.m_iFaction = pkPlayer->GetFactionID();
	kOccupationData.m_iOccupateTime = GET_PROCESS_TIME;
	kOccupationData.m_iIndexFlag = pkRewardData->kOccupyInfo[0];
	kOccupationData.m_iScoreDeltaTime = pkRewardData->kOccupyInfo[1];
	kOccupationData.m_iAddFactionScore = pkRewardData->kOccupyInfo[2];
	kOccupationData.m_iAddActorScore = pkRewardData->kOccupyInfo[3];

	pkGuildLadderInfo->SetWarInfoDirty(true);
}

BOOL LC_GameStory_GuildLadderWar::_checkFactionScoreToEnd(GameStoryScheduleKey ikey, int32_t iFaction)
{
	GameStoryConfig* pkConfig = GetConfig();
	if (NULL == pkConfig)
	{
		return false;
	}
	int32_t iFactionScoreToEnd = pkConfig->m_kBatchSechedule.kConfig.GetFactionScoreToEnd();
	if (iFactionScoreToEnd > 0)
	{
		if (GetFactionScore(ikey, iFaction) >= iFactionScoreToEnd)
		{
			_clearBatchSchedule(ikey, pkConfig);
			return true;
		}
	}
	return false;
}

void LC_GameStory_GuildLadderWar::_onDeactived(void)
{
	static LC_GuildLadderManager* pkGuildLadderMgr = LC_GuildLadderManager::GetSingletonPtr();
	if (NULL == pkGuildLadderMgr)
	{
		return;
	}
	pkGuildLadderMgr->GuildLadderWarAllSettle();
}

void LC_GameStory_GuildLadderWar::PostPlayerEnterMap(GameLogic::LC_PlayerBase* pkPlayer, LC_ServerMapLogicBase* pkPlace)
{
	LC_Server_GameStory_Base::PostPlayerEnterMap(pkPlayer, pkPlace);
	if (NULL == pkPlayer || NULL == pkPlace)
	{
		return;
	}
	GameStoryScheduleKey ikey = (GameStoryScheduleKey)pkPlace->GetMapLogicID();
	_syncWarInfo(ikey, pkPlayer);
	_syncActorScore(ikey, pkPlayer);
}

void LC_GameStory_GuildLadderWar::_clearBatchSchedule(GameStoryScheduleKey iKey, GameStoryConfig* pkConfig)
{
	LC_Server_GameStory_Base::_clearBatchSchedule(iKey, pkConfig);	
	_guildLadderWarSettle(iKey);
}

void LC_GameStory_GuildLadderWar::_guildLadderWarSettle(GameStoryScheduleKey iKey)
{
	GSGuildLadderInfo* pkGuildLadderInfo = GetGuildLadderInfoPtr(iKey);
	if (NULL == pkGuildLadderInfo)
	{
		return;
	}
	if (pkGuildLadderInfo->IsEnd())
	{
		return;
	}
	pkGuildLadderInfo->SetEnd();

	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerMapLogic_Place* logic = (LC_ServerMapLogic_Place*)pkWorldManager->GetMapLogic(iKey);
	if (logic != NULL)
	{
		logic->EndAllPlay();
		logic->Finsh();

		//to clear all clone player
		LC_ActorPtrList kActors;
		logic->GetPlayersInMap(kActors);
		LC_ActorPtrList::iterator itActor = kActors.begin();
		for (; itActor != kActors.end(); ++itActor)
		{
			logic->ClearPlayer((LC_ServerPlayer*)*itActor);
		}
	}
	
}

void LC_GameStory_GuildLadderWar::_syncWarInfo(GameStoryScheduleKey iKey, GameLogic::LC_PlayerBase* pkPlayer)
{
	GSGuildLadderInfo* pkGuildLadderInfo = GetGuildLadderInfoPtr(iKey);
	if (NULL == pkGuildLadderInfo)
	{
		return;
	}
	if (!pkGuildLadderInfo->GetWarInfoDirty())
	{
		return;
	}
	static LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerMapLogic_Place* pkPlace = pkWorldManager->GetMapLogic(iKey);
	if (NULL == pkPlace)
	{
		return;
	}

	pkGuildLadderInfo->SetWarInfoDirty(false);
	MG_PB<msg_define::guild_ladder_war_info> kSyncMsg;
	msg_define::guild_ladder_war_info& msg = kSyncMsg.m_value;
	GSGuildLadderFactionScoreMap::iterator kFactionScoreIter;
	TRAV(pkGuildLadderInfo->m_kFactionScore, kFactionScoreIter)
	{
		msg_define::guild_ladder_war_faction_score* pEntry = msg.add_faction_score();
		if (NULL == pEntry)
		{
			continue;
		}
		int32_t iFaction = kFactionScoreIter->first;
		int32_t iScore = kFactionScoreIter->second.m_iTotalScore;
		pEntry->set_faction(iFaction);
		pEntry->set_score(iScore);
	}

	GSGuildLadderOccupationMap::iterator kOccupationIter;
	TRAV(pkGuildLadderInfo->m_kOccupation, kOccupationIter)
	{
		msg_define::guild_ladder_occupation* pEntry = msg.add_faction_occupation();
		if (NULL == pEntry)
		{
			continue;
		}
		int32_t iIndexFlag = kOccupationIter->first;
		int32_t iFaction = kOccupationIter->second.m_iFaction;
		pEntry->set_index(iIndexFlag);
		pEntry->set_faction(iFaction);
	}
	if (NULL != pkPlayer)
	{
		((LC_ServerPlayer*)pkPlayer)->SendMsgToClient(MGPT_SYNC_GUILD_LADDER_WAR_INFO, &kSyncMsg);
	}
	else
	{
		pkPlace->BroadcastMsgToClient(MGPT_SYNC_GUILD_LADDER_WAR_INFO, &kSyncMsg);
	}	
}

void LC_GameStory_GuildLadderWar::_syncActorScore(GameStoryScheduleKey ikey, GameLogic::LC_PlayerBase* pkPlayer)
{
	if (NULL == pkPlayer)
	{
		return;
	}
	GSGuildLadderInfo* pkGuildLadderInfo = GetGuildLadderInfoPtr(ikey);
	if (NULL == pkGuildLadderInfo)
	{
		return;
	}
	int32_t iActorScore = pkGuildLadderInfo->GetActorScore(pkPlayer->GetInstance());
	MG_PB<msg_define::guild_ladder_actor_score> kSyncMsg;
	kSyncMsg.m_value.set_actor_score(iActorScore);
	((LC_ServerPlayer*)(pkPlayer))->SendMsgToClient(MGPT_SYNC_GUILD_LADDER_ACTOR_SCORE, &kSyncMsg);
}