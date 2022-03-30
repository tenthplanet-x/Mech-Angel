#include "UT_DirConfig.h"
#include "UT_Func.h"
#include "LC_Server_TranscriptionDungeon_Config.h"
#include "ajson_mem.hpp"
#include "UT_Log.h"
using namespace boost::ajson;
using namespace DungeonConfig;
using namespace Utility;

AJSON(Utility::UT_Vec3Int, (x)(y)(z))
AJSON_CODE(PosAndLoad, (x)(y)(z)(l)(r)(a))
AJSON(Utility::UT_Vec2Float, (x)(y))
AJSON_CODE(Event, (Type)(Value1)(Value2)(Value3)(Value4)(Value5))
AJSON_CODE(MonsterWeightEntry, (MonsterIndex)(Rate))

AJSON_CODE(EliteWeightEntry, (EliteIndex)(Rate))
AJSON_CODE(BatchGroup, (PosIndex)(MonsterCombins))
AJSON_CODE(EventListener, (Concern)(Action))

AJSON_CODE(BatchEntry, (Scope)(DelayTime)(Group)(Elite)(StartHandle)(FinishEvent)(FinishHandle)(Listeners)(StatCatagory))
AJSON_CODE(EliteRefleshWeightEntry, (RefleshSize)(Rate))
AJSON_CODE(MonsterEntry, (TypeId)(Count))
AJSON_CODE(DoorEntry, (TypeId)(Position)(Angel)(InitState)(CenterPoint)(XAxis)(YAxis)(HalfExtentX)(HalfExtentY))
AJSON_CODE(TargetEntry, (TypeId)(Position)(Angel))
AJSON_CODE(EliteSkillRuleEntry, (SkillRuleID)(Rate))
AJSON_CODE(Dungeon_Config, (EliteMonsterRefleshRate)(EliteSkillRule)(NotifyMsgs)(MonsterGroup)(EliteGroup)(PosGroup)(StartEvent)(StartHandle)(FinishEvent)(ExcludeElite)(IncludeElite)(BatchsNotify)(Batchs)(Doors)(Targets))


bool Dungeon_Config::Init(const StringType& configFileName)
{
	StringType pkConfigFileName = TRANSCRIPTION_CONFIG_DIR + configFileName;
	pkConfigFileName = Utility::getFilePath(pkConfigFileName);
	StringType errstr;
	bool ret = load_from_file((*this), (char*)pkConfigFileName.c_str(), errstr);
	if (!ret)
	{
		GfxWriteLog(LOG_SYSTEM_FILELINE, LOG_SWITCH_SYSTEM, "load json file error!%s,%s", configFileName.c_str(), errstr.c_str());
	}
	return ret;
}
int Dungeon_Config::get_batchcount()
{
	int count = 0;
	BatchSetType::iterator beg = this->Batchs.begin();
	for (; beg != this->Batchs.end(); ++beg)
	{
		count += (int)beg->Scope.size();
	}
	return count;
}
