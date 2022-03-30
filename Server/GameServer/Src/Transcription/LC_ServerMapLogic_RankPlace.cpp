#include "LC_ServerMapLogic_Place.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerMapLogic_RankPlace.h"
#include "LC_DamageRankManager.h"
#include "CF_WorldMapList.h"
#include "GlobalSettings.h"

using namespace GameLogic;
using namespace Utility;
using namespace CSVFile;
using namespace Protocol;
//------------------------------------------------------------------------------
LC_ServerMapLogic_RankPlace::LC_ServerMapLogic_RankPlace() : LC_ServerMapLogic_Place(), m_bSettle(false)
{
}
//------------------------------------------------------------------------------
LC_ServerMapLogic_RankPlace::~LC_ServerMapLogic_RankPlace()
{

}
//------------------------------------------------------------------------------
void LC_ServerMapLogic_RankPlace::Init()
{
	LC_ServerMapLogic_Place::Init();
	Clear();
}
void LC_ServerMapLogic_RankPlace::UnInit()
{
	LC_ServerMapLogic_Place::UnInit();
	Clear();
}
void LC_ServerMapLogic_RankPlace::Clear()
{
	m_bSettle = false;
	SetMapLogicType(MT_REANSCRIPTION_RANK);
}
//----------------------------------------------------------------------------
void LC_ServerMapLogic_RankPlace::SetPlaceState(PlaceLogicState flag)
{
	if (m_PlaceState == flag)
	{
		return;
	}

	if(flag != PLACESTATE_PROCESS)
	{
		SetVaild(false);
	}

	m_PlaceState = flag;
	if (m_PlaceState == PLACESTATE_PROCESS)
	{
		GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "LC_ServerMapLogic_RankPlace SetPlaceState RecordStarttime", GetMapResID(), GET_PROCESS_TIME);
		m_logicRecord.RecordStarttime(GET_PROCESS_TIME);
		if (m_PersistTotalTime <= 0)
		{
			m_TimeStamp = 0.0f;
		}
		else
		{
			float curtime = GET_PROCESS_TIME;
			m_TimeStamp = curtime + (float)(m_PersistTotalTime - m_CountDownTime);
		}
	}
	else if (m_PlaceState == PLACESTATE_PREEND)
	{//倒计时开始到副本结束的时间
		m_TimeStamp = GET_PROCESS_TIME + m_CountDownTime;
		//BroadCastGameEventToClient(EVENT_TYPE_SHUTDOWN_TIME_LEFT, m_CountDownTime);

		int storyID = GetStoryID();
		//怪被打死，提前结束
		Settle(storyID, m_CountDownTime, true);
	}
	else if(m_PlaceState == PLACESTATE_FAIL)
	{
		float curtime = GET_PROCESS_TIME;
		float countDownTime = m_CountDownTime;
		if(m_bIShowRaidStage)
		{
			countDownTime = (float)GetGlobalSetting.failCountDownTime;
		}

		m_TimeStamp = curtime + (float)(countDownTime);
		int resMapID = GetMapResID();
		int mapID = GetMapLogicID();
		int storyID = GetStoryID();
		//GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "LC_ServerMapLogic_RankPlace::SetPlaceState()  m_bIShowRaidStage:[%d], resMapID:[%d], mapId:[%d], time:[%f]\n", m_bIShowRaidStage, resMapID, mapID, curtime);
		//BroadCastGameEventToClient(EVENT_TYPE_FAIL, (int)countDownTime);

		//指挥官死亡    玩家请求退出副本  副本时间到
		Settle(storyID, countDownTime);
	}
	else if (m_PlaceState == PLACESTATE_END)
	{
		m_logicRecord.RecordEndtime(GET_PROCESS_TIME);
		//需要删除队伍共享副本
		if (GetGroupShare() == Share_None)
		{
			return;
		}
		__clearAllGroupShare();
	}
}
//-----------------------------------------------------------------------------------
void LC_ServerMapLogic_RankPlace::Settle(int storyID, float countDownTime, bool success)
{
	if(m_bSettle)
	{
		return;
	}

	m_bSettle = true;

	GameLogic::LC_ActorPtrList players;
	GetAllRealPlayer(players);
	if(players.empty())
	{
		return;
	}

	GameLogic::LC_ActorPtrList::iterator actorBeg = players.begin();
	for(; actorBeg != players.end(); ++actorBeg)
	{
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(*actorBeg);
		if (pkPlayer != NULL)
		{
			pkPlayer->UpdateDamageData(storyID, PlayerDamageUpdateType_GreaterUpdate, countDownTime);		
		}

		if(success)
		{
			int32_t iPassTime = (int32_t)(GetRecord()->getPassTime());
			pkPlayer->GetRaidAsset()->UpdateRaid(pkPlayer, GetMapResID(), 3, iPassTime);
		}
	}
}


/////////////////////////////////////////////////////////////////////
LC_ServerMapLogic_GuildStoryMapPlace::LC_ServerMapLogic_GuildStoryMapPlace() : m_nGuildID(0)
{
}

LC_ServerMapLogic_GuildStoryMapPlace::~LC_ServerMapLogic_GuildStoryMapPlace()
{

}

void LC_ServerMapLogic_GuildStoryMapPlace::Init()
{
	LC_ServerMapLogic_Place::Init();
	Clear();
}

void LC_ServerMapLogic_GuildStoryMapPlace::UnInit()
{
	LC_ServerMapLogic_Place::UnInit();
	Clear();
}

void LC_ServerMapLogic_GuildStoryMapPlace::Clear()
{
	m_nGuildID = 0;
	SetMapLogicType(MT_GUILD_MAP2);
}

void LC_ServerMapLogic_GuildStoryMapPlace::SetPlaceState(PlaceLogicState flag)
{
	if (m_PlaceState == flag)
	{
		return;
	}

	if(flag != PLACESTATE_PROCESS)
	{
		SetVaild(false);
	}

	m_PlaceState = flag;
	if (m_PlaceState == PLACESTATE_PROCESS)
	{
		m_logicRecord.RecordStarttime(GET_PROCESS_TIME);
		if (m_PersistTotalTime <= 0)
		{
			m_TimeStamp = 0.0f;
		}
		else
		{
			float curtime = GET_PROCESS_TIME;
			m_TimeStamp = curtime + (float)(m_PersistTotalTime - m_CountDownTime);
		}
	}
	else if (m_PlaceState == PLACESTATE_PREEND)
	{
		//倒计时开始到副本结束的时间
		m_TimeStamp = GET_PROCESS_TIME + m_CountDownTime;

		//怪被打死，提前结束
		UpdateAllDamageData();
	}
	else if(m_PlaceState == PLACESTATE_FAIL)
	{
		m_TimeStamp = GET_PROCESS_TIME + (float)m_CountDownTime;
		UpdateAllDamageData();
	}
	else if (m_PlaceState == PLACESTATE_END)
	{
		m_logicRecord.RecordEndtime(GET_PROCESS_TIME);
		UpdateAllDamageData();
	}
}

void LC_ServerMapLogic_GuildStoryMapPlace::ForceUpdateDamageData(LC_ServerPlayer* pkPlayer, bool syncClient)
{
	if(NULL==pkPlayer || INVALID_GUILD_ID==pkPlayer->GetGuildID() || !pkPlayer->IsMainController())
	{
		return;
	}

	pkPlayer->UpdateDamageData(GetStoryID(), PlayerDamageUpdateType_IncrementalUpdate, 0, syncClient);
}

void LC_ServerMapLogic_GuildStoryMapPlace::SetGuildID(uint64_t guildID)
{
	m_nGuildID = guildID;
}

uint64_t LC_ServerMapLogic_GuildStoryMapPlace::GetGuildID()
{
	return m_nGuildID;
}

attr_value_type LC_ServerMapLogic_GuildStoryMapPlace::GetTotalDamage()
{
	CDamageRankManager* pkMgr = CDamageRankManager::GetSingletonPtr();
	return pkMgr->GetGuildTotalDamage(GetMapResID(), m_nGuildID);
}

void LC_ServerMapLogic_GuildStoryMapPlace::UpdatePlayerDamage(LC_ActorBase* pkPlayer, attr_value_type damage)
{
	if(NULL==pkPlayer || !IS_PLAYER_CLASS_ID(pkPlayer->GetID()))
	{
		return;
	}

	LC_ServerPlayer* pkOwnerPlayer = ((LC_ServerPlayer*)pkPlayer)->GetOwnerPlayer();
	if(NULL == pkOwnerPlayer || 0==pkOwnerPlayer->GetGuildID())
	{
		return;
	}

	CDamageRankManager* pkMgr = CDamageRankManager::GetSingletonPtr();
	pkMgr->UpdatePlayerDamage(GetMapResID(), m_nGuildID, pkOwnerPlayer->GetInstance(), TPSTR2STDSTR(pkOwnerPlayer->GetCharName()), damage);
}

attr_value_type LC_ServerMapLogic_GuildStoryMapPlace::GetPlayerDamage(uint64_t playerID)
{
	CDamageRankManager* pkMgr = CDamageRankManager::GetSingletonPtr();
	return pkMgr->GetPlayerDamage(GetMapResID(), m_nGuildID, playerID);
}

void LC_ServerMapLogic_GuildStoryMapPlace::UpdateAllDamageData()
{
	GameLogic::LC_ActorPtrList players;
	GetAllRealPlayer(players);
	if(players.empty())
	{
		return;
	}

	GameLogic::LC_ActorPtrList::iterator actorBeg = players.begin();
	for(; actorBeg != players.end(); ++actorBeg)
	{
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(*actorBeg);
		if (pkPlayer != NULL)
		{
			pkPlayer->UpdateDamageData(GetStoryID(), PlayerDamageUpdateType_IncrementalUpdate,0, true);
		}
	}
}