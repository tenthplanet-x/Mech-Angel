#include "UT_Func.h"
#include "UT_DirConfig.h"
#include "LC_Server_War_Config.h"
#include "ajson_mem.hpp"
#include "UT_Log.h"
using namespace boost::ajson;
using namespace DungeonConfig;
using namespace Utility;

///////////////////////////////////////////////////////////////////////////////
AJSON_CODE( Utility::UT_Vec3Int,(x)(y)(z))
AJSON_CODE( PosAndLoad,(x)(y)(z)(l)(r)(a))
AJSON_CODE( Utility::UT_Vec2Float,(x)(y))
AJSON_CODE(Event,(Type)(Value1)(Value2)(Value3)(Value4)(Value5))
AJSON_CODE(MonsterWeightEntry,(MonsterIndex)(Rate))

AJSON_CODE(BatchGroup,(PosIndex)(MonsterCombins))
AJSON_CODE(BatchEntry,(Scope)(DelayTime)(Group)(Elite)(StartHandle)(FinishEvent)(FinishHandle)(StatCatagory))
AJSON_CODE(MonsterEntry,(TypeId)(Count))
AJSON_CODE(DoorEntry,(TypeId)(Position)(Angel)(InitState)(CenterPoint)(XAxis)(YAxis)(HalfExtentX)(HalfExtentY))
AJSON_CODE(TargetEntry,(TypeId)(Position)(Angel))
AJSON_CODE(EliteSkillRuleEntry,(SkillRuleID)(Rate))
AJSON_CODE(Dungeon_Config,(EliteMonsterRefleshRate)(EliteSkillRule)(NotifyMsgs)(MonsterGroup)(EliteGroup)(PosGroup)(StartEvent)(StartHandle)
					 (FinishEvent)(ExcludeElite)(IncludeElite)(BatchsNotify)(Batchs)(Doors)(Targets))

AJSON_CODE(PowerEntry,(CharType)(Pos)(Angel)(DurationTime)(Cost)(NaviPos))
AJSON_CODE(Respawn,(Pos)(Radius)(CharType)(Punish))
AJSON_CODE(LogicAreaEntry,(LogicAreaId)(BuffId)(Multiple))
AJSON_CODE(RuneEntry,(RuneId)(Rate))
AJSON_CODE(RuneSet,(NotifyMsg)(PosGroup)(RuneList)(DelayTime)(IntervalTime)(LiveTime))
AJSON_CODE(CatapultEntry,(CharType)(Pos)(Angel)(BulletCost)(Radius)(CruiseTime)(WarnTime)(CDTime)(VaildDistance))
AJSON_CODE(War_Config,(Power)(PowerRespawn)(ChallengeRespawn)(LogicArea)(Rune)(Monster)(Catapults))

AJSON_CODE(NoticeEntry,(TimeSpan)(NoticeStr))
AJSON_CODE(WarTimeEntry,(TimeLen)(StartNotice)(EndNotify)(SuccessNotify)(FailNotify)(GivePowerSuccessNotify)(GivePowerFailNotify)(Notice))
AJSON_CODE(GuildMapEntry,(MapResId)(Rate)(MaxPlayer))
AJSON_CODE(GuildWarManager_Config,(GuildWarTime)(GuildMap))
///////////////////////////////////////////////////////////////////////////////

bool War_Config::Init(const StringType& configFileName)
{
	StringType pkConfigFileName = TRANSCRIPTION_CONFIG_DIR + configFileName;
	pkConfigFileName = Utility::getFilePath(pkConfigFileName);
	StringType errstr;
	bool ret= load_from_file((*this),(char *)pkConfigFileName.c_str(),errstr);
	if(!ret)
	{
		GfxWriteLog(LOG_SYSTEM_FILELINE,LOG_SWITCH_SYSTEM,"load json file error!%s,%s",configFileName.c_str(),errstr.c_str());
	}
	return ret;
}

int War_Config::get_batchcount()
{
	int count=0;
	BatchSetType::iterator beg=this->Monster.Batchs.begin();
	for (;beg!=this->Monster.Batchs.end();++beg)
	{
		count+=(int)beg->Scope.size();
	}
	return count;
}

bool GuildWarManager_Config::Init(const StringType& configFileName)
{
	StringType pkConfigFileName = TRANSCRIPTION_CONFIG_DIR + configFileName;
	pkConfigFileName = Utility::getFilePath(pkConfigFileName);
	StringType errstr;
	bool ret= load_from_file((*this),(char *)pkConfigFileName.c_str(),errstr);
	if(!ret)
	{
		GfxWriteLog(LOG_SYSTEM_FILELINE,LOG_SWITCH_SYSTEM,"load json error!%s,%s",configFileName.c_str(),errstr.c_str());
	}
	return ret;
}
