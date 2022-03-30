#include "LM_LocaleManager.h"

#include "GameServerApp.h"
#include "LC_ServerWorldManager.h"
#include "UT_ServerHelper.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerNPC.h"
#include "LC_ServerMapLogicFactory.h"
#include "LC_ServerMapLogic_Place.h"
#include "LC_ServerMapLogic_ArenaPlay.h"
#include "LC_RankManager.h"
#include "CF_RankReward.h"
#include "LC_Dule_Manager.h"
#include "oss_define.h"
#include "LC_ServerGameEventManager.h"
using namespace GameLogic;
using namespace CSVFile;
using namespace Protocol;
using namespace Utility;

LC_ServerMapLogic_ArenaPlay::LC_ServerMapLogic_ArenaPlay():
m_Config(NULL),m_State(Arena_State_None), m_ArenaID(0),
m_PrePareTime(0),m_BattleTime(0),m_TimeStamp(0.0f)
{}

LC_ServerMapLogic_ArenaPlay::~LC_ServerMapLogic_ArenaPlay()
{
	UnInit();
}

bool LC_ServerMapLogic_ArenaPlay::Init(LC_ServerMapLogic_Place* place, LC_Dule_Config* config)
{
	if (place==NULL || config==NULL)
	{
		return false;
	}
	bool ret=LC_ServerMapLogic_PlayBase::Init(place, PlayType_Arena);
	if (!ret)
	{
		return false;
	}
	m_Config=config;
	m_PrePareTime=m_Config->PrePareTime;
	m_BattleTime=m_Config->BattleTime;
	m_TimeStamp=0.0f;
	m_State=Arena_State_Inited;
	return true;
}

void LC_ServerMapLogic_ArenaPlay::UnInit()
{
	End();
	ArenaCharMap::iterator it = m_Chars.begin();
	for (; it!=m_Chars.end(); ++it)
	{
		ArenaChar& rkChar = it->second;
		LC_ServerPlayer* pkPlayer=GetPlace()->GetActorManger().GetPlayer(rkChar.ObjectID());
		if (pkPlayer)
		{
			GetPlace()->ClearPlayer(pkPlayer);
		}
	}
	m_Chars.clear();
	m_PrePareTime=0;
	m_BattleTime=0;
	m_TimeStamp=0;
	m_State=Arena_State_None;
	m_Config=NULL;
	LC_ServerMapLogic_PlayBase::UnInit();
}

bool LC_ServerMapLogic_ArenaPlay::Start()
{
	if (m_State!=Arena_State_Inited && m_State!=Arena_State_End)
	{
		return false;
	}
	m_State = Arena_State_Prepare;
	m_TimeStamp = GET_PROCESS_TIME + 30;
	SyncArenaStatus();
	return true;
}

void LC_ServerMapLogic_ArenaPlay::End()
{
	if (!CheckRun())
	{
		return;
	}
	if (Arena_State_End != m_State)
	{
		BattleResult();
		m_State = Arena_State_End;
		m_TimeStamp = GET_PROCESS_TIME + m_PrePareTime + 2;
	}
	//½áËã
	Settle();
	SyncArenaStatus();

	ArenaCharMap::iterator it = m_Chars.begin();
	for (; it!=m_Chars.end(); ++it)
	{
		ArenaChar& rkChar = it->second;
		LC_ServerPlayer* pkPlayer=GetPlace()->GetActorManger().GetPlayer(rkChar.ObjectID());
		if (pkPlayer)
		{
			GetPlace()->AddActor2Faction(pkPlayer, FACTION_NEUTRAL);
		}
	}
}

bool LC_ServerMapLogic_ArenaPlay::Update(float fCurrentTime, float fDeltaTime)
{
	switch(m_State)
	{
	case Arena_State_Prepare:
		if(fCurrentTime>=m_TimeStamp)
		{
			Loading();
		}
		break;
	case Arena_State_Loading:
		if(fCurrentTime>=m_TimeStamp)
		{
			CountDown();
		}
		break;
	case Arena_State_CountDown:
		if(fCurrentTime>=m_TimeStamp)
		{
			Battle();
		}
		break;
	case Arena_State_Battle:
		if(fCurrentTime>=m_TimeStamp)
		{
			End();
		}
		break;
	case Arena_State_End:
		if (fCurrentTime>=m_TimeStamp)
		{
			Reset();
		}
		break;
	default:
		break;
	}
	return true;
}

ResultType LC_ServerMapLogic_ArenaPlay::CheckEnter(LC_ServerPlayer* player)
{
	IF_A_NA_PLAYER (player)
	{
		return RE_FAIL;
	}
	if (m_Chars.count(player->GetInstance()) <= 0)
	{
		return RE_FAIL;
	}
	return RE_SUCCESS;
}
void LC_ServerMapLogic_ArenaPlay::EventNotify(MapLogicEvent& kEvent)
{
	
}

void LC_ServerMapLogic_ArenaPlay::PlayerEventNotify(LC_ServerPlayer* player, MapLogicEvent& kEvent)
{
	IF_A_NA_PLAYER(player)return;
	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "sync arena player event!", m_ArenaID, GetPlace()->GetMapLogicID(), player->GetID(), player->GetInstance(), player->GetMapLogicID(), kEvent.Type(), m_State);
	if (kEvent.Type()==PlayerEvent_Enter)
	{
		ChallengePlayerActionForbid(player);
		player->SetServerViewState(LC_ServerPlayer::SERVER_STATE_INWORLD);
		checkCountdown();
	}
	else if (kEvent.Type()==PlayerEvent_Leave)
	{
		ChallengePlayerActionUnForbid(player);
		ArenaCharMap::iterator it = m_Chars.find(player->GetInstance());
		if (it != m_Chars.end())
		{
			ArenaChar& rkChar = it->second;
			++rkChar.m_DeadTimes;
		}
		End();
	}
	else if (kEvent.Type()==PlayerEvent_Death)
	{
		ArenaCharMap::iterator it = m_Chars.find(player->GetInstance());
		if (it != m_Chars.end())
		{
			ArenaChar& rkChar = it->second;

			++ rkChar.m_DeadTimes;
		}
		End();
	}
}

bool LC_ServerMapLogic_ArenaPlay::CheckEnd()
{
	return m_State==Arena_State_End;
}

bool LC_ServerMapLogic_ArenaPlay::CheckRun()
{
	return (m_State==Arena_State_Prepare || m_State==Arena_State_CountDown || m_State==Arena_State_Loading || m_State==Arena_State_Battle);
}

void LC_ServerMapLogic_ArenaPlay::Settle()
{
	//ÉÏ±¨
	SERVER_GET_ARENA_MANAGER()->SetArenaResult(m_ArenaID, m_Chars);
}

void LC_ServerMapLogic_ArenaPlay::ChallengePlayerActionForbid(LC_ServerPlayer* player)
{
	IF_A_NA_PLAYER(player)return;
	
	LC_ActionForbid* pa = player->GetActionForbid();
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
	//pa->AddActionForbid(LAFT_SHOPCITY,LAF_SHOPCITY_MAX_COUNT);

	LC_ActorAttributeMap* pkAttributeMap = player->GetFinalAttributeMap();
	player->SetDead(false);
	player->SetHP(pkAttributeMap->GetAttribute(ATT_MAX_HP));
}

void LC_ServerMapLogic_ArenaPlay::ChallengePlayerActionUnForbid(LC_ServerPlayer* player)
{
	IF_A_NA_PLAYER(player) return;
	
	LC_ActionForbid* pa = player->GetActionForbid();
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

	LC_ActorAttributeMap* pkAttributeMap = player->GetFinalAttributeMap();
	player->SetHP(pkAttributeMap->GetAttribute(ATT_MAX_HP));
}

void LC_ServerMapLogic_ArenaPlay::ReserveArenaInfo(const Arena_Info& rkArena)
{
	m_ArenaID = rkArena.ID();
	m_Chars.clear();

	const msg_define::arena_info& rk_arena = rkArena.arena_if; 
	for (int i=0; i<rk_arena.factions_size(); ++i)
	{
		Faction& rkFaction = m_Config->Factions[i%m_Config->Factions.size()];
		for (int j=0; j<rk_arena.factions(i).chars_size(); ++j)
		{
			unique_id_impl char_id = rk_arena.factions(i).chars(j);
			m_Chars[char_id].m_CharID = char_id;
			m_Chars[char_id].m_FactionID = rkFaction.FactionId;
			m_Chars[char_id].m_Location = rkFaction.Position;
		}
	}
	Prepare();
}

void LC_ServerMapLogic_ArenaPlay::Prepare()
{
	ArenaCharMap::iterator it = m_Chars.begin();
	for (; it!=m_Chars.end(); ++it)
	{
		ArenaChar& rkChar = it->second;
		if (rkChar.m_ObjectID)
		{
			continue;
		}
		LC_ServerPlayer* spawnPlayer = GetPlace()->SpawnPlayer(rkChar.m_CharID, rkChar.m_Location, PLY_DEF_PROXY_BITS, NULL, LC_ServerMapLogic_ArenaPlay::SummonPlayerFinish, FACTION_NEUTRAL);
		if(NULL == spawnPlayer)
			continue;
		rkChar.m_ObjectID = spawnPlayer->GetID();
	}
}

void LC_ServerMapLogic_ArenaPlay::Loading()
{
	m_State = Arena_State_Loading;
	m_TimeStamp = GET_PROCESS_TIME + m_PrePareTime + 2;
}

void LC_ServerMapLogic_ArenaPlay::CountDown()
{
	LC_ServerMapLogic_ActorManger& rkActorMgr = GetPlace()->GetActorManger();
	ArenaCharMap::iterator it = m_Chars.begin();
	for (; it!=m_Chars.end(); ++it)
	{
		ArenaChar& rkChar = it->second;
		LC_ServerPlayer* pkPlayer = rkActorMgr.GetPlayer(rkChar.ObjectID());
		if(NULL == pkPlayer)
			continue;

		GetPlace()->NotifyGameEventToClient(pkPlayer, EVENT_TYPE_ARENA_PREPARE, m_PrePareTime);
	}
	
	m_State = Arena_State_CountDown;
	m_TimeStamp = GET_PROCESS_TIME + m_PrePareTime + 2;
	SyncArenaStatus();
}

void LC_ServerMapLogic_ArenaPlay::Battle()
{
	LC_ServerMapLogic_ActorManger& rkActorMgr = GetPlace()->GetActorManger();
	ArenaCharMap::iterator it = m_Chars.begin();
	for (int i=0; it!=m_Chars.end(); ++it, ++i)
	{
		ArenaChar& rkChar = it->second;
		LC_ServerPlayer* pkPlayer = rkActorMgr.GetPlayer(rkChar.ObjectID());
		if(NULL == pkPlayer)
			continue;

		GetPlace()->AddActor2Faction(pkPlayer, (FactionType)rkChar.FactionID());

		pkPlayer->SetHP(pkPlayer->GetFinalAttributeMap()->GetAttribute(ATT_MAX_HP));
		
		GetPlace()->NotifyGameEventToClient(pkPlayer, EVENT_TYPE_ARENA_BATTLE, m_BattleTime);
	}

	m_State = Arena_State_Battle;
	m_TimeStamp = GET_PROCESS_TIME + m_BattleTime + 2;	
	SyncArenaStatus();
}

void LC_ServerMapLogic_ArenaPlay::Reset()
{
	ArenaCharMap::iterator it = m_Chars.begin();
	for (; it!=m_Chars.end(); ++it)
	{
		ArenaChar& rkChar = it->second;
		LC_ServerPlayer* pkPlayer=GetPlace()->GetActorManger().GetPlayer(rkChar.ObjectID());
		if (pkPlayer)
		{
			GetPlace()->AddActor2Faction(pkPlayer, FACTION_NEUTRAL);
			pkPlayer->GetAsAControlee().Reset();
		}
	}
}

void LC_ServerMapLogic_ArenaPlay::BattleResult()
{
	LC_ServerMapLogic_ActorManger& rkActorMgr = GetPlace()->GetActorManger();

	attr_value_type max_hp = 0;
	attr_value_type min_dead = 0xFFFF;
	int max_faction = FACTION_NONE;
	ArenaCharMap::iterator it = m_Chars.begin();
	for (int i=0; it!=m_Chars.end(); ++it, ++i)
	{
		ArenaChar& rkChar = it->second;
		LC_ServerPlayer* pkPlayer = rkActorMgr.GetPlayer(rkChar.ObjectID());
		if(NULL==pkPlayer)
		{
			GetPlace()->SetFactionResult((FactionType)rkChar.FactionID(), FACTION_RESULT_FAIL);
			continue;
		}

		if (rkChar.m_DeadTimes==min_dead ? pkPlayer->GetHP() > max_hp : rkChar.m_DeadTimes<min_dead)
		{
			if (max_faction != FACTION_NONE)
			{
				GetPlace()->SetFactionResult((FactionType)max_faction, FACTION_RESULT_FAIL);
			}
			max_faction = rkChar.FactionID();
			max_hp = pkPlayer->GetHP();
			min_dead = rkChar.DeadTimes();
		}
		else
		{
			GetPlace()->SetFactionResult((FactionType)rkChar.FactionID(), FACTION_RESULT_FAIL);
		}
	}
	if (max_faction!=FACTION_NONE)
	{
		GetPlace()->SetFactionResult((FactionType)max_faction, FACTION_RESULT_SUCESS);
	}
}

LC_ServerPlayer* LC_ServerMapLogic_ArenaPlay::FindPlayer(const unique_id_impl& charID)
{
	LC_ServerMapLogic_ActorManger& rkActorMgr = GetPlace()->GetActorManger();
	ArenaCharMap::iterator it = m_Chars.find(charID);
	return (it!=m_Chars.end()) ? rkActorMgr.GetPlayer(it->second.ObjectID()) : NULL;
}

void LC_ServerMapLogic_ArenaPlay::SyncArenaStatus()
{
	static LC_Arena_Manager* pkArenaMgr = SERVER_GET_ARENA_MANAGER();
	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "sync arena status!", m_ArenaID, m_State);
	pkArenaMgr->SyncArenaInfo(m_ArenaID);
}
void LC_ServerMapLogic_ArenaPlay::checkLoaded(void)
{
	if (m_State != Arena_State_Prepare)
	{
		return;
	}
	LC_ServerMapLogic_ActorManger& rkActorMgr = GetPlace()->GetActorManger();
	ArenaCharMap::iterator it = m_Chars.begin();
	for (int i=0; it!=m_Chars.end(); ++it, ++i)
	{
		ArenaChar& rkChar = it->second;
		LC_ServerPlayer* pkPlayer = rkActorMgr.GetPlayer(rkChar.ObjectID());
		if(NULL == pkPlayer)
			continue;

		if (!pkPlayer->GetLoadDataFromDBFlag())
		{
			return;
		}
	}
	Loading();
}
void LC_ServerMapLogic_ArenaPlay::checkCountdown(void)
{
	if (m_State != Arena_State_Loading)
	{
		return;
	}
	LC_ServerMapLogic_ActorManger& rkActorMgr = GetPlace()->GetActorManger();
	ArenaCharMap::iterator it = m_Chars.begin();
	for (int i=0; it!=m_Chars.end(); ++it, ++i)
	{
		ArenaChar& rkChar = it->second;
		LC_ServerPlayer* pkPlayer = rkActorMgr.GetPlayer(rkChar.ObjectID());
		if(NULL == pkPlayer)
			continue;

		if (!pkPlayer->IsInGame())
		{
			return;
		}
	}
	CountDown();
}
void LC_ServerMapLogic_ArenaPlay::SummonPlayerFinish(LC_ServerPlayer* player)
{
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerMapLogic_Place* place=(LC_ServerMapLogic_Place*)pkWorldManager->GetMapLogic(player->GetMapLogicID());
	if (place==NULL)
	{
		return;
	}
	LC_ServerMapLogic_ArenaPlay* play=(LC_ServerMapLogic_ArenaPlay*)place->GetPlay(PlayType_Arena);
	if (play==NULL)
	{
		return;
	}
	play->checkLoaded();
}

