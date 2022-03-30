#include "LC_GuildLadderManager.h"
#include "StringDefine.h"
#include "UT_DirConfig.h"
#include "UT_Log.h"
#include "NW_Helper.h"
#include "LC_ServerMapLogic_Place.h"
#include "LC_GameStory_GuildLadderWar.h"

using namespace Utility;
using namespace GameStory;

bool LC_GuildLadderManager::Init()
{
	StringType sConfigPath(TRANSCRIPTION_CONFIG_DIR);
	sConfigPath += StringType("guild_ladder.json");
	StringType error_string;
	if (!boost::ajson::load_from_file(m_kConfig, sConfigPath.c_str(), error_string))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "[error]: load %s failed, error:%s.\n", sConfigPath.c_str(), error_string.c_str());
		return false;
	}
    return true;
}

void LC_GuildLadderManager::Unit()
{
	m_kMatchMap.clear();
	m_kMatchUIDMap.clear();
	m_kCharID2MatchMap.clear();
	m_kGuildUID2FactionMap.clear();
	m_kGuildUID2TargetServerMap.clear();
}
GuildLadderMatchData* LC_GuildLadderManager::GetMatchData(int32_t iMatchID)
{
	GuildLadderMatchMap::iterator iter = m_kMatchMap.find(iMatchID);
	if (iter == m_kMatchMap.end())
	{
		return NULL;
	}
	return &(iter->second);
}
GuildLadderMatchData* LC_GuildLadderManager::GetMatchDataByGuildUID(unique_id_impl uid)
{
	GuildLadderUID2MatchPtrMap::iterator iter = m_kMatchUIDMap.find(uid);
	if (iter == m_kMatchUIDMap.end())
	{
		return NULL;
	}
	return iter->second;
}
GuildLadderMatchData* LC_GuildLadderManager::GetMatchDataByCharID(unique_id_impl charID)
{
	GuildLadderCharID2MatchPtrMap::iterator iter = m_kCharID2MatchMap.find(charID);
	if (iter == m_kCharID2MatchMap.end())
	{
		return NULL;
	}
	return iter->second;
}
void LC_GuildLadderManager::FillLadderMatchInfo()
{
	m_kMatchMap.clear();
	m_kGuildUID2FactionMap.clear();
	m_kGuildUID2TargetServerMap.clear();
	m_kCharID2MatchMap.clear();
	GuildLadderMatchData& data = m_kMatchMap[1];
	data.m_iMatchID = 1;
	data.m_kGuildUIDs[0] = 1;
	data.m_kGuildUIDs[1] = 2;
	data.m_kGuildUIDs[2] = 0;
	m_kMatchUIDMap[1] = &data;
	m_kMatchUIDMap[2] = m_kMatchUIDMap[1];
	m_kGuildUID2FactionMap[1] = FACTION_RED;
	m_kGuildUID2FactionMap[2] = FACTION_GREEN;
	m_kGuildUID2TargetServerMap[1] = 9999;
	m_kGuildUID2TargetServerMap[2] = 9999;
}

int32_t LC_GuildLadderManager::GetFactionByGuildUID(unique_id_impl& guildUID)
{
	int32_t ret = FACTION_NONE;
	GuildLadderUID2FactionMap::iterator iter = m_kGuildUID2FactionMap.find(guildUID);
	if (iter != m_kGuildUID2FactionMap.end())
	{
		return iter->second;
	}		
	return ret;
}

int32_t LC_GuildLadderManager::GetTargetServerByGuildUID(unique_id_impl& guildUID)
{
	int32_t ret = 0;
	GuildLadderUID2TargetServerMap::iterator iter = m_kGuildUID2TargetServerMap.find(guildUID);
	if (iter != m_kGuildUID2TargetServerMap.end())
	{
		return iter->second;
	}
	return ret;
}

void LC_GuildLadderManager::GuildLadderWarSettle(GameStory::GameStoryScheduleKey iKey, GameStory::GSGuildLadderInfo* pkGSGuildLadderInfo)
{
	if (NULL == pkGSGuildLadderInfo)
	{
		return;
	}
	GuildLadderLogic2MatchIDMap::iterator iter = m_kLogic2MatchMap.find(iKey);
	if (iter == m_kLogic2MatchMap.end())
	{
		return;
	}
	int32_t iMatchID = iter->second;
	if (iMatchID <= 0)
	{
		return;
	}
	GuildLadderMatchData* pkMatchData = GetMatchData(iMatchID);
	if (NULL == pkMatchData)
	{
		return;
	}
	if (pkMatchData->IsEnd())
	{
		return;
	}
	pkMatchData->SetEnd();
	
	//report to global server
	MG_PB<msg_define::guild_ladder_settle_req> kReportGlobalMsg;
	msg_define::guild_ladder_settle_req& kSettleMsg = kReportGlobalMsg.m_value;
	kSettleMsg.set_match_id(iMatchID);
	for (int i = 0; i < E_GUILD_LADDER_MAX_FACTIONS; ++i)
	{
		unique_id_type lGuildUID = pkMatchData->m_kGuildUIDs[i].data();
		if (lGuildUID <= 0)
		{
			break;
		}	
		msg_define::guild_ladder_settle_score* pkFactionScoreEntry = kSettleMsg.add_faction_score();
		if (NULL == pkFactionScoreEntry)
		{
			continue;
		}
		pkFactionScoreEntry->set_guild_uid(lGuildUID);
		
		int32_t iFaction = FACTION_RED + i;		
		GSGuildLadderFactionScoreRecord& rkScoreRecord = pkGSGuildLadderInfo->m_kFactionScore[iFaction];
		pkFactionScoreEntry->set_total_score(rkScoreRecord.m_iTotalScore);
		pkFactionScoreEntry->set_gather_score(rkScoreRecord.m_iGatherScore);
		GSGuildLadderOccupationRecordMap::iterator iter;
		TRAV(rkScoreRecord.m_kOccupationScore, iter)
		{
			int32_t iFlagIndex = iter->first;
			int32_t iOccupationScore = iter->second;
			msg_define::ladder_occupy_score* pkOccupyEntry = pkFactionScoreEntry->add_occupy_score();
			if (NULL == pkFactionScoreEntry)
			{
				continue;
			}
			pkOccupyEntry->set_index(iFlagIndex);
			pkOccupyEntry->set_score(iOccupationScore);
		}
		
	}
	SendMsgToGlobal(MGPT_SS_GLOBAL_GUILD_LADDER_SETTLE_REQ, &kReportGlobalMsg, 0, 0);

	//report actor score to gameserver for reward
	GSGuildLadderActorScoreMap::iterator ActorScoreIter;
	TRAV(pkGSGuildLadderInfo->m_kActorScore, ActorScoreIter)
	{
		unique_id_type lCharID = ActorScoreIter->first.data();
		int32_t iActorScore = ActorScoreIter->second;
	}
}

void LC_GuildLadderManager::GuildLadderWarAllSettle()
{
	GuildLadderMatchMap::iterator iter;
	TRAV(m_kMatchMap, iter)
	{
		GuildLadderMatchData& rkMatchData = iter->second;
		if (!rkMatchData.IsEnd())
		{
			GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "Guild Ladder has non-reported match !");
		}
	}
	Unit();
}

void LC_GuildLadderManager::OnSpawPlayerFinished(LC_ServerPlayer* pkPlayer, GuildLadderMatchData* pkMatchData)
{
	if (NULL == pkMatchData || !IS_PLAYER_CLONE_ID(pkPlayer->GetID()))
	{
		return;
	}
	const unique_id_impl& charID = pkPlayer->GetInstance();
	m_kCharID2MatchMap[charID] = pkMatchData;
}

//Íæ¼ÒÏÂÏß£¬É¾³ý¾µÏñ
void LC_GuildLadderManager::CharLeave(LC_ServerPlayer* pkPlayer)
{
	IF_A_NA_PLAYER(pkPlayer) return;
	int32_t iTargetServerID = pkPlayer->GetAsAControler().OppositePeer().ServerID();
	if (iTargetServerID <= 0)
	{
		return;
	}
	SendMsgToGame(iTargetServerID, MGPT_G2G_GUILD_LADDER_LEAVE, NULL, pkPlayer->GetUserID(), pkPlayer->GetInstance());
}