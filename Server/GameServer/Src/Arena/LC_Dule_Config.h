#ifndef LC_DULE_CONFG_H
#define LC_DULE_CONFG_H
#include "LC_Server_TranscriptionDungeon_Config.h"
#include "StringDefine.h"
#include "UT_Vec.h"

namespace DungeonConfig
{
	struct Faction
	{
		long  FactionId;
		Utility::UT_Vec3Int  Position;
		Faction():
		FactionId(0),Position()
		{
		}

	};
	typedef mem::vector<Faction>	FactionVectorType;
	struct LC_Dule_Config:public Logic_ConfigData_Base
	{
		LC_Dule_Config():Factions(){}

		FactionVectorType Factions;
		long PrePareTime;
		long BattleTime;
		long ShowAllInfoTime;
		long ShowTeamInfoTime;
		
		bool Init(const StringType& configFileName);
	};

}


#endif