#ifndef LC_SERVER_GUILDWAR_CONFG_H
#define LC_SERVER_GUILDWAR_CONFG_H

#include "StringDefine.h"
#include "LC_Server_TranscriptionDungeon_Config.h"

namespace DungeonConfig
{
	struct PowerEntry
	{
		int		CharType;
		int		Angel;
		int		DurationTime;
		int		Cost;
		Utility::UT_Vec3Int	Pos;
		Utility::UT_Vec3Int	NaviPos;

		PowerEntry(): CharType(0), Angel(0), DurationTime(0), Cost(0), Pos(), NaviPos() {}
	};
	typedef mem::vector<int>	LogicAreaSet;
	//////////////////////////////////////////////////////////////////////////////////////
	struct Respawn
	{
		int		CharType;
		Utility::UT_Vec3Int	Pos;
		int		Radius;
		LogicAreaSet Punish;

		Respawn(): CharType(0), Pos(), Radius(0), Punish() {}
	};
	//////////////////////////////////////////////////////////////////////////////////////
	struct LogicAreaEntry
	{
		int LogicAreaId;
		int BuffId;
		int Multiple;

		LogicAreaEntry(): LogicAreaId(0), BuffId(0), Multiple(0) {}
	};
	typedef mem::vector<LogicAreaEntry>	LogicAreaSetType;
	//////////////////////////////////////////////////////////////////////////////////////
	struct RuneEntry
	{
		int	RuneId;
		int	Rate;
		RuneEntry(): RuneId(0), Rate(0) {}
	};
	typedef mem::vector<mem::vector<RuneEntry> >	RuneSetType;
	//////////////////////////////////////////////////////////////////////////////////////
	struct RuneSet
	{
		std::string NotifyMsg;
		PositionSetType  PosGroup;
		RuneSetType  RuneList;
		float DelayTime;
		float IntervalTime;
		float LiveTime;
		RuneSet(): NotifyMsg(), PosGroup(), RuneList(), DelayTime(0.0), IntervalTime(0.0), LiveTime(0) {}
	};
	//////////////////////////////////////////////////////////////////////////////////////
	struct CatapultEntry
	{
		int CharType;
		PosAndLoad	Pos;
		int	Angel;
		int BulletCost;
		int Radius;
		float CruiseTime;
		float WarnTime;
		float CDTime;
		int VaildDistance;
		CatapultEntry(): CharType(0), Pos(), Angel(0), BulletCost(0), Radius(0), CruiseTime(0.0f), WarnTime(0.0f), CDTime(0.0f), VaildDistance(0) {}
	};
	typedef mem::vector<CatapultEntry >	CatapultSetType;
	//////////////////////////////////////////////////////////////////////////////////////
	struct War_Config: public Logic_ConfigData_Base
	{
		PowerEntry	Power;
		Respawn		PowerRespawn;
		Respawn		ChallengeRespawn;
		LogicAreaSetType LogicArea;
		RuneSet		Rune;
		Dungeon_Config	Monster;
		CatapultSetType Catapults;

		War_Config(): Power(), PowerRespawn(), ChallengeRespawn(), LogicArea(), Rune(), Monster(), Catapults() {}
		bool Init(const StringType& configFileName);
		int get_batchcount();
	};
	//////////////////////////////////////////////////////////////////////////////////////
	struct NoticeEntry
	{
		int		TimeSpan;
		std::string	NoticeStr;
		NoticeEntry(): TimeSpan(0), NoticeStr() {}
	};
	typedef mem::vector<NoticeEntry>	NoticeSetType;
	//////////////////////////////////////////////////////////////////////////////////////
	struct WarTimeEntry
	{
		int TimeLen;
		std::string StartNotice;
		std::string EndNotify;
		std::string SuccessNotify;
		std::string FailNotify;
		std::string GivePowerSuccessNotify;
		std::string GivePowerFailNotify;
		NoticeSetType Notice;
		WarTimeEntry(): TimeLen(0), StartNotice(), EndNotify(), SuccessNotify(), FailNotify(),
						GivePowerSuccessNotify(), GivePowerFailNotify(), Notice() {}
	};
	//////////////////////////////////////////////////////////////////////////////////////
	struct GuildMapEntry
	{
		int	MapResId;
		int	Rate;
		int MaxPlayer;
		GuildMapEntry(): MapResId(0), Rate(0), MaxPlayer(0) {}

		bool IsVaild() { return MapResId != 0; }
	};
	//////////////////////////////////////////////////////////////////////////////////////
	struct GuildWarManager_Config
	{
		WarTimeEntry GuildWarTime;
		GuildMapEntry GuildMap;
		LongSetType DomainMap;
		GuildWarManager_Config(): GuildWarTime(), GuildMap(), DomainMap() {}
		bool Init(const StringType& configFileName);
	};
}

#endif
