#ifndef LC_SERVER_TRANSCRIPTIONDUNGEON_CONFG_H
#define LC_SERVER_TRANSCRIPTIONDUNGEON_CONFG_H

#include "StringDefine.h"
#include "MM_BaseObject.h"
#include "UT_Vec.h"
#include <string>

namespace DungeonConfig
{
	struct Logic_ConfigData_Base  : public ::Memory::MM_BaseObject
	{
		DECL_RTTI_NAME(Logic_ConfigData_Base)
		int configflag;
	};

	struct Event
	{
		int	Type;
		int	Value1;
		int	Value2;
		int	Value3;
		int	Value4;
		int	Value5;

		Event(): Type(0), Value1(0), Value2(0), Value3(0), Value4(0), Value5(0) {}
		Event(const Event& other): Type(other.Type), Value1(other.Value1), Value2(other.Value2), Value3(other.Value3), Value4(other.Value4), Value5(other.Value5) {}
		
		Event& operator = (const Event& rv)
		{
			this->Type = rv.Type;
			this->Value1 = rv.Value1;
			this->Value2 = rv.Value2;
			this->Value3 = rv.Value3;
			this->Value4 = rv.Value4;
			this->Value5 = rv.Value5;
			return *this;
		}
		
		void reset()
		{
			this->Type = 0;
			this->Value1 = 0;
			this->Value2 = 0;
			this->Value3 = 0;
			this->Value4 = 0;
			this->Value5 = 0;
		}
	};
	////////////////////////////////////////////////////////////////////////
	struct MonsterWeightEntry
	{
		int  MonsterIndex;
		int  Rate;

		MonsterWeightEntry():MonsterIndex(0), Rate(0) {}
	};
	////////////////////////////////////////////////////////////////////////
	struct EliteWeightEntry
	{
		int  EliteIndex;
		int  Rate;

		EliteWeightEntry():EliteIndex(0), Rate(0) {}
	};
	typedef mem::vector<std::string>	NotifyMsgVector;
	////////////////////////////////////////////////////////////////////////
	struct PosAndLoad
	{
		int  x;
		int  y;
		int  z;
		int  l;
		int  r;
		int  a;

		PosAndLoad(): x(0), y(0), z(0), l(0), r(0), a(0) {}
	};
	////////////////////////////////////////////////////////////////////////
	typedef mem::vector<PosAndLoad>	PositionSetType;
	typedef mem::vector<int> LongSetType;
	typedef mem::vector<EliteWeightEntry> EliteWeightSetType;
	typedef mem::vector<MonsterWeightEntry> MonsteWeightSetType;
	////////////////////////////////////////////////////////////////////////
	struct BatchGroup
	{
		LongSetType PosIndex;
		MonsteWeightSetType MonsterCombins;

		BatchGroup(): PosIndex(), MonsterCombins() {}
	};
	typedef mem::vector<BatchGroup>	BatchGroupSetType;
	////////////////////////////////////////////////////////////////////////
	struct EventListener
	{
		Event Concern;
		Event Action;
	};
	typedef mem::vector<EventListener> EventListenerList;
	////////////////////////////////////////////////////////////////////////
	struct BatchEntry
	{
		LongSetType	Scope;
		float DelayTime;
		BatchGroupSetType Group;
		EliteWeightSetType Elite;
		Event StartHandle;
		Event FinishEvent;
		Event FinishHandle;
		EventListenerList Listeners;
		int32_t StatCatagory;

		BatchEntry(): Scope(), DelayTime(0.0), Group(), Elite(), StartHandle(), FinishEvent(), FinishHandle() ,StatCatagory(0){}
	};
	typedef mem::vector<BatchEntry> BatchSetType;
	////////////////////////////////////////////////////////////////////////
	struct EliteRefleshWeightEntry
	{
		int RefleshSize;
		int	Rate;

		EliteRefleshWeightEntry(): RefleshSize(0), Rate(0) {}
	};
	typedef mem::vector<EliteRefleshWeightEntry> EliteRefleshWeightSetType;
	////////////////////////////////////////////////////////////////////////
	struct MonsterEntry
	{
		int	TypeId;
		int	Count;

		MonsterEntry(): TypeId(0), Count(0) {}
	};
	typedef mem::vector< mem::vector<MonsterEntry> > MonsterGroupSetType;
	////////////////////////////////////////////////////////////////////////
	struct DoorEntry
	{
		int	TypeId;
		Utility::UT_Vec3Int	Position;
		int	Angel;
		Utility::UT_Vec3Int	CenterPoint;
		Utility::UT_Vec2Float XAxis;
		Utility::UT_Vec2Float YAxis;
		int	HalfExtentX;
		int	HalfExtentY;
		int	InitState;

		DoorEntry(): TypeId(0), Angel(0), HalfExtentX(0), HalfExtentY(0), InitState(0) {}
	};
	typedef mem::vector<DoorEntry> DoorSetType;
	////////////////////////////////////////////////////////////////////////
	struct TargetEntry
	{
		int	TypeId;
		int	Angel;
		Utility::UT_Vec3Int	Position;

		TargetEntry(): TypeId(0), Angel(0) {}
	};
	typedef mem::vector<TargetEntry> TargetSetType;
	////////////////////////////////////////////////////////////////////////
	struct EliteSkillRuleEntry
	{
		int	SkillRuleID;
		int	Rate;

		EliteSkillRuleEntry(): SkillRuleID(0), Rate(0) {}
	};
	typedef mem::vector<EliteSkillRuleEntry> EliteSkillRuleSetType;
	////////////////////////////////////////////////////////////////////////
	struct Dungeon_Config: public Logic_ConfigData_Base
	{
		EliteRefleshWeightSetType EliteMonsterRefleshRate;
		EliteSkillRuleSetType EliteSkillRule;
		NotifyMsgVector	NotifyMsgs;
		MonsterGroupSetType MonsterGroup;
		LongSetType	EliteGroup;
		PositionSetType	PosGroup;
		Event StartEvent;
		Event StartHandle;
		Event FinishEvent;
		LongSetType	ExcludeElite;
		LongSetType	IncludeElite;
		LongSetType	BatchsNotify;
		BatchSetType Batchs;
		DoorSetType	Doors;
		TargetSetType Targets;

		Dungeon_Config(): EliteMonsterRefleshRate(), EliteSkillRule(), NotifyMsgs(), MonsterGroup(), EliteGroup(), PosGroup(), StartEvent(), StartHandle(), 
							FinishEvent(), ExcludeElite(), IncludeElite(), BatchsNotify(), Batchs(), Doors(), Targets() {}
		bool Init(const StringType& configFileName);
		int get_batchcount();
	};

}


#endif
