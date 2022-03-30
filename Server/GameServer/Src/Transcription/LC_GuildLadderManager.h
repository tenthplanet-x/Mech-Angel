//服务器帮派天梯赛管理

#pragma once
#include "MM_BaseObject.h"
#include "GuildCommDef.h"
#include "LC_GameStoryDataDefine.h"
#include "LC_GameStory_GuildLadderWar.h"
enum 
{
	E_GUILD_LADDER_MAX_FACTIONS = 3,
};

struct GuildLadderMatchData
{
	GuildLadderMatchData() :m_iMatchID(0), m_iMapLogicID(0), m_iTargetServerID(0), m_bEnd(false)
	{		
		memset(m_kGuildUIDs, 0, sizeof(m_kGuildUIDs));
		memset(m_kServerIDs, 0, sizeof(m_kServerIDs));
	}
	void	SetEnd(){ m_bEnd = true; }
	bool	IsEnd()const{ return m_bEnd; }
	int32_t	GetFaction(unique_id_impl& uid)const
	{
		for (int i = 0; i < sizeof(m_kGuildUIDs) / sizeof(m_kGuildUIDs[0]); ++i)
		{
			if (m_kGuildUIDs[i].data() > 0 && m_kGuildUIDs[i] == uid)
			{
				return FACTION_RED + i;
			}
		}
		return FACTION_NONE;
	}
	int32_t			m_iMatchID;
	unique_id_impl	m_kGuildUIDs[E_GUILD_LADDER_MAX_FACTIONS];
	int32_t			m_kServerIDs[E_GUILD_LADDER_MAX_FACTIONS];
	int32_t			m_iMapLogicID;
	int32_t			m_iTargetServerID;
	bool			m_bEnd;
};
typedef mem::map<int32_t, GuildLadderMatchData> GuildLadderMatchMap;
typedef mem::map<unique_id_impl, GuildLadderMatchData*> GuildLadderUID2MatchPtrMap;
typedef mem::map<unique_id_impl, GuildLadderMatchData*> GuildLadderCharID2MatchPtrMap;
typedef mem::map<int32_t, int32_t> GuildLadderLogic2MatchIDMap;
typedef mem::map<unique_id_impl, int32_t> GuildLadderUID2FactionMap;
typedef mem::map<unique_id_impl, int32_t> GuildLadderUID2TargetServerMap;

class LC_GuildLadderManager : public Memory::MM_BaseObject, public Utility::UT_Singleton<LC_GuildLadderManager>
{
public:
	LC_GuildLadderManager() {}
	~LC_GuildLadderManager() {}

	bool		Init();
	void        Unit();
	void		Update(float fCurrentTime, float fDeltaTime);
    int32_t     GetSignUpLevelLimit() const { return m_kConfig.level_limit; }
    int32_t     GetSignUpNeedMoney() const { return m_kConfig.need_money; }
    bool        IsInSignUpTime() const { return m_kConfig.IsInSignUpTime(); }
	int32_t		GetMapResID() const { return m_kConfig.map_id; }

	GuildLadderMatchData* GetMatchData(int32_t iMatchID);
	GuildLadderMatchData* GetMatchDataByGuildUID(unique_id_impl uid);
	GuildLadderMatchData*	GetMatchDataByCharID(unique_id_impl charID);
	void		FillLadderMatchInfo();
	int32_t		GetMatchSize()const { return (int32_t)m_kMatchMap.size(); }
	int32_t		GetFactionByGuildUID(unique_id_impl& guildUID);
	int32_t		GetTargetServerByGuildUID(unique_id_impl& guildUID);
	void		GuildLadderWarSettle(GameStory::GameStoryScheduleKey iKey, GameStory::GSGuildLadderInfo* pkGSGuildLadderInfo);
	void		GuildLadderWarAllSettle();
	const		ladder_config& GetConfig() const { return m_kConfig; }
	void		OnSpawPlayerFinished(LC_ServerPlayer* pkPlayer, GuildLadderMatchData* pkMatchData);
	
	void		CharLeave(LC_ServerPlayer* pkPlayer);
private:
    ladder_config		m_kConfig;
	GuildLadderMatchMap	m_kMatchMap;
	GuildLadderUID2MatchPtrMap m_kMatchUIDMap;
	GuildLadderCharID2MatchPtrMap m_kCharID2MatchMap;
	GuildLadderLogic2MatchIDMap m_kLogic2MatchMap;
	GuildLadderUID2FactionMap m_kGuildUID2FactionMap;
	GuildLadderUID2TargetServerMap m_kGuildUID2TargetServerMap;
};
