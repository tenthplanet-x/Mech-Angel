#include "UT_DirConfig.h"
#include "ajson_mem.hpp"
#include "LC_Dule_Config.h"
#include "UT_Func.h"
#include "UT_Log.h"
using namespace boost::ajson;
using namespace DungeonConfig;
using namespace Utility;
AJSON_CODE( Utility::UT_Vec3Int,(x)(y)(z))
AJSON_CODE( Faction,(FactionId)(Position))
AJSON_CODE( LC_Dule_Config,(Factions)(PrePareTime)(BattleTime)(ShowAllInfoTime)(ShowTeamInfoTime))

bool LC_Dule_Config::Init(const StringType& configFileName)
{
	StringType pkConfigFileName = TRANSCRIPTION_CONFIG_DIR + configFileName;
	pkConfigFileName = Utility::getFilePath(pkConfigFileName);
	StringType errstr;
	bool ret= load_from_file((*this),(char *)pkConfigFileName.c_str(),errstr);
	if(!ret)
	{
		GfxWriteLog(LOG_SYSTEM_FILELINE,LOG_SWITCH_SYSTEM,"load LC_Dule_Config json file error! fileName:%s, err:%s",configFileName.c_str(),errstr.c_str());
	}
	return ret;
}