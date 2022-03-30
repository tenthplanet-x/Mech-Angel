#ifndef _LC_SERVERMAPLOGIC_PLACEBASE_H
#define _LC_SERVERMAPLOGIC_PLACEBASE_H
#include "SystemError.h"
#include "LC_ServerPlayer.h"

//Íæ·¨»ùÀà
enum PlayerEventType
{
	PlayerEvent_Enter,
	PlayerEvent_Leave,
	PlayerEvent_OffLine,
	PlayerEvent_Recover,
	PlayerEvent_Death,
	PlayerEvent_ChangeLogicArea
};

enum MapLogicNotifyEvent
{
	MapLogicNotifyEvent_Org = 0,
	MapLogicNotifyEvent_LogicArea = 1,
	MapLogicNotifyEvent_DialogFinish = 2,
	MapLogicNotifyEvent_MonsterCount = 3,
	MapLogicNotifyEvent_Target = 4,
	MapLogicNotifyEvent_PlayerEnter = 5,
	MapLogicNotifyEvent_FinishTask = 6,
};

#define MAX_MAP_LOGIC_EVENT_PARAM_COUNT 5
/////////////////////////////////////////////////////////////////////////////
struct MapLogicEvent
{
	MapLogicEvent(): m_iType(0), m_uiSrcID(INVALID_ACTOR_ID) { memset(m_kParams, 0, sizeof(m_kParams)); }
	MapLogicEvent(int32_t iType, object_id_type uiSrcID = INVALID_ACTOR_ID): m_iType(iType), m_uiSrcID(uiSrcID) { memset(m_kParams, 0, sizeof(m_kParams)); }
	inline void SetType(int32_t  type) { m_iType = type; }

	void SetParam(int32_t idx, int32_t val)
	{
		if (idx < 0 || idx >= MAX_MAP_LOGIC_EVENT_PARAM_COUNT)
		{
			return;
		}
		m_kParams[idx] = val;
	}

	inline int32_t Type(void) const { return m_iType; }
	inline object_id_type SrcID(void) const { return m_uiSrcID; }
	inline int32_t Param(int32_t idx) const { return (idx < 0 || idx >= MAX_MAP_LOGIC_EVENT_PARAM_COUNT) ? 0 : m_kParams[idx]; }

	inline int32_t& Param1(void) { return m_kParams[0]; }
	inline int32_t& Param2(void) { return m_kParams[1]; }
	inline int32_t& Param3(void) { return m_kParams[2]; }
	inline int32_t& Param4(void) { return m_kParams[3]; }
	inline int32_t& Param5(void) { return m_kParams[4]; }

	int32_t m_iType;
	object_id_type m_uiSrcID;
	int32_t m_kParams[MAX_MAP_LOGIC_EVENT_PARAM_COUNT];
};
/////////////////////////////////////////////////////////////////////////////
class LC_ServerMapLogic_Place;
class LC_ServerMapLogic_PlayBase : public ::Memory::MM_BaseObject
{
public:
	DECL_RTTI_NAME(LC_ServerMapLogic_PlayBase)
public:
	LC_ServerMapLogic_PlayBase(): m_Place(NULL), m_PlayType(PlayType_None), m_AutoStart(false) {}

	virtual ~LC_ServerMapLogic_PlayBase() { UnInit(); }

	virtual bool Init(LC_ServerMapLogic_Place* place, PlayType playtype)
	{
		m_Place = place;
		m_PlayType = playtype;
		return true;
	}

	virtual void UnInit()
	{
		m_PlayType = PlayType_None;
		m_Place = NULL;
	}

	virtual bool Start() = 0;
	virtual void End() = 0;
	virtual bool Update(float fCurrentTime, float fDeltaTime) = 0;
	virtual void EventNotify(MapLogicEvent& kEvent)	{}

	virtual bool CheckEnd() = 0;
	virtual bool CheckRun() = 0;
	virtual Utility::UT_Vec3Int	ComputeEnterLocation(LC_ServerPlayer* player, const Utility::UT_Vec3Int& targetlocation) { return Utility::UT_Vec3Int();}

	virtual ResultType CheckEnter(LC_ServerPlayer* player) { return RE_SUCCESS; }
	virtual void PlayerEventNotify(LC_ServerPlayer* player, MapLogicEvent& kEvent) {}
	virtual void Save(Protocol::MG_OUTPUT_CDR& rkStream) {}
	virtual void Load(Protocol::MG_INPUT_CDR& rkStream) {}

	inline void SetPlace(LC_ServerMapLogic_Place* place) { m_Place = place; }
	inline LC_ServerMapLogic_Place* GetPlace() { return m_Place; }
	inline PlayType GetType() { return m_PlayType; }
	inline void SetAutoStart(bool autostart) { m_AutoStart = autostart; }
	inline bool GetAutoStart() { return m_AutoStart; }
private:
	LC_ServerMapLogic_Place*	m_Place;
	PlayType					m_PlayType;
	bool						m_AutoStart;
};
#endif