#pragma once

#include "LC_Define.h"
#include "LC_Server_TranscriptionDungeon_Config.h"
#include "LC_ServerMapLogic_PlayBase.h"

using namespace DungeonConfig;
/////////////////////////////////////////////////////////////////
struct LogicAreaUnit
{
	int LogicAreaId;
	int BuffId;
	int Multiple;

	LogicAreaUnit(): LogicAreaId(0), BuffId(0), Multiple(0) {}
};
typedef std::vector<LogicAreaUnit>	LogicAreaArrType;
typedef mem::map<int, LogicAreaUnit*>	LogicAreaMapType;

struct LogicArea_Config: public Logic_ConfigData_Base
{
	LogicAreaArrType LogicArea;
	LogicAreaMapType AreaMap;

	LogicArea_Config(): LogicArea(), AreaMap() {}
	bool Init(const StringType& configFileName);
};
/////////////////////////////////////////////////////////////////
class LC_ServerMapLogic_LogicArea: public LC_ServerMapLogic_PlayBase
{
public:
	DECL_RTTI_NAME(LC_ServerMapLogic_LogicArea)
public:
	LC_ServerMapLogic_LogicArea(): m_Config(NULL) {}
	virtual ~LC_ServerMapLogic_LogicArea() {}
	bool Init(LC_ServerMapLogic_Place* place, LogicArea_Config* config);
	virtual void UnInit();
	virtual bool Start();
	virtual void End();
	virtual bool Update(float fCurrentTime, float fDeltaTime);
	virtual void EventNotify(MapLogicEvent& kEvent);
	virtual void PlayerEventNotify(LC_ServerPlayer* player, MapLogicEvent& kEvent);
	virtual bool CheckEnd() { return false; }
	virtual bool CheckRun() { return true; }
private:
	LogicArea_Config* m_Config;
};
