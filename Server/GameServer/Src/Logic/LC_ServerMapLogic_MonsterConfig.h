/******************************************************
Author:				evan.ding
CreateTime:			2013.7.15   14:20
FileName:			LC_ServerMapLogic_MonsterConfig
FileExt:			.h
Porpose:			Elite Monster Config Data
******************************************************/

#ifndef _LC_SERVER_MAP_LOGIC_MONSTER_CONFIG_H
#define _LC_SERVER_MAP_LOGIC_MONSTER_CONFIG_H
#include "LC_Server_TranscriptionDungeon_Config.h"
#include "LC_Dule_Config.h"
#include "MM_BaseObject.h"
#include "UT_Vec.h"
#include "SystemDefine.h"
#include "LC_Define.h"
#include "LC_Helper.h"
using namespace DungeonConfig;

//地下城刷怪数据定义
struct DungeonConfigData : public ::Memory::MM_BaseObject
{
	DECL_RTTI_NAME( DungeonConfigData )
	struct MonsterData
	{
		MonsterData()
			:m_kEliteProp(&Utility::UT_SIMDataInfo::ParamA)
		{	
			Reset();
		};

		void Reset()
		{
			m_kSpawnLocation			=	Utility::UT_Vec3Int(0,0,0);
			m_iZRotAngle				=	0;
			m_iSpawnCharType			=	0;
			m_iWanderRadius				=	0;
			m_lWanderPathID				=	0;
			m_iNPCType					=	0;
			m_iStage					=	0;
			m_SpawnPointID				= 0;
			m_SpawnContorlType			= 0;
			m_SpawnTime					= 0;
			m_SpawnTriggerValue			= 0;
			m_iAutoFitDeltaLevel		= 0;
			m_iAutoFitMaxLevel			= 0;
			m_iAutoFitMinLevel			= 0;
			m_kEliteProp.Clear();
		}

		MonsterData& clone(const MonsterData& rv)
		{
			this->m_kSpawnLocation		=	rv.m_kSpawnLocation;
			this->m_iZRotAngle			=	rv.m_iZRotAngle;
			this->m_iSpawnCharType		=	rv.m_iSpawnCharType;
			this->m_iWanderRadius		=	rv.m_iWanderRadius;
			this->m_lWanderPathID		=	rv.m_lWanderPathID;
			this->m_iNPCType			=	rv.m_iNPCType;
			this->m_iStage				=	rv.m_iStage;
			this->m_SpawnPointID		= rv.m_SpawnPointID;
			this->m_SpawnContorlType	= rv.m_SpawnContorlType;
			this->m_SpawnTime			= rv.m_SpawnTime;
			this->m_SpawnTriggerValue	= rv.m_SpawnTriggerValue;
			this->m_iAutoFitDeltaLevel	= rv.m_iAutoFitDeltaLevel;
			this->m_iAutoFitMaxLevel	= rv.m_iAutoFitMaxLevel;
			this->m_iAutoFitMinLevel	= rv.m_iAutoFitMinLevel;
			this->m_kEliteProp			= rv.m_kEliteProp;
			return *this;
		}


		MonsterData& operator = (const MonsterData& rv)
		{
			return this->clone(rv);
		}

		Utility::UT_Vec3Int m_kSpawnLocation;
		int				m_iZRotAngle;
		int				m_iSpawnCharType;
		int				m_iWanderRadius;
		int				m_lWanderPathID;	
		int				m_iNPCType;	
		int				m_iStage;
		int				m_SpawnPointID;				//怪物的刷新点ID
		int				m_SpawnContorlType;			//0 立即，1 定时，2 触发
		int				m_SpawnTime;				//定时刷新的周期，单位秒
		int				m_SpawnTriggerValue;		//触发值，如果是触发者，会发送这个值到触发事件，如果是被触发者，收到这个触发事件会刷新出来
		int				m_iAutoFitDeltaLevel;		//与自适应等级的差值，
		int				m_iAutoFitMaxLevel;			//自适应等级最大值
		int				m_iAutoFitMinLevel;			//自适应等级最小值
		GameLogic::LC_RandomSIMDataPool	m_kEliteProp;
	};

	struct AutoSpawnMonsterData
	{
		AutoSpawnMonsterData()
			:_lastActiveTime(0)
			,_lastUpdateTime(0)
			,_coolDownTime(0)
			,_bWaitActive(true)
			,_spawnData(0)
		{}

		object_id_set _objectLogicIDs;
		int32_t _lastActiveTime;
		int32_t _lastUpdateTime;
		int32_t _coolDownTime;
		bool _bWaitActive;
		MonsterData * _spawnData;


		AutoSpawnMonsterData(const AutoSpawnMonsterData& rv)
		{
			this->clone(rv);
		}

		AutoSpawnMonsterData& clone(const AutoSpawnMonsterData& rv)
		{
			this->_objectLogicIDs = rv._objectLogicIDs;
			this->_lastActiveTime = rv._lastActiveTime;
			this->_lastUpdateTime = rv._lastUpdateTime;
			this->_coolDownTime = rv._coolDownTime;
			this->_bWaitActive = rv._bWaitActive;
			this->_spawnData = rv._spawnData;
			return *this;
		}

		AutoSpawnMonsterData& operator = (const AutoSpawnMonsterData& rv)
		{
			return this->clone(rv);
		}
	};

	struct EliteMonsterType
	{
		int		m_NPCCharType;			//怪的ID
		int		m_Elite;						//怪能提升到精英等级

		EliteMonsterType()
			:m_NPCCharType(0),m_Elite(0)
		{}

		EliteMonsterType(const EliteMonsterType& rv)
		{
			clone(rv);
		}

		EliteMonsterType& operator = (const EliteMonsterType& rv)
		{
			return clone(rv);
		}

		EliteMonsterType& clone(const EliteMonsterType& rv)
		{
			this->m_NPCCharType = rv.m_NPCCharType;
			this->m_Elite = rv.m_Elite;
			return *this;
		}
	};

	DungeonConfigData()
	{
		m_fStartTime = 0;
	}

	DungeonConfigData(const DungeonConfigData& rv)
	{
		clone(rv);
	}

	DungeonConfigData& operator = (const DungeonConfigData& rv)
	{
		return clone(rv);
	}

	DungeonConfigData& clone(const DungeonConfigData& rv)
	{
		this->m_MonsterMap = rv.m_MonsterMap;
		this->m_MonsterGroupList = rv.m_MonsterGroupList;
		this->m_AutoSpawnMonsterList = rv.m_AutoSpawnMonsterList;
		this->m_EliteMonsterTypeList = rv.m_EliteMonsterTypeList;
		this->m_EliteGroupMin = rv.m_EliteGroupMin;
		this->m_EliteGroupMax = rv.m_EliteGroupMax;
		this->m_fStartTime = rv.m_fStartTime;
		return *this;
	}

	void ReadConfigFile(const StringType& configFileName,const StringType& kTableName);

	void ReadConfigCF(int MapLogicType);

	typedef mem::vector<MonsterData *> MonsterGroupType;
	typedef mem::vector<MonsterGroupType> MonsterGroupListType;

	typedef mem::vector<MonsterData>	MonsterDataList;
	typedef MonsterDataList::iterator	MonsterDataListIter;

	typedef mem::map<int,AutoSpawnMonsterData>	AutoSpawnMonsterDataMap;
	typedef AutoSpawnMonsterDataMap::iterator		AutoSpawnMonsterDataMapIter;

	//地图刷怪的数据
	MonsterDataList				m_MonsterMap;
	//分组后的刷怪数据
	MonsterGroupListType	m_MonsterGroupList;

	AutoSpawnMonsterDataMap m_AutoSpawnMonsterList;

	typedef mem::vector<EliteMonsterType> EliteMonsterTypeListType;

	//地图符合变成精英怪的数据
	EliteMonsterTypeListType m_EliteMonsterTypeList;

	//地图出现精英怪的下限
	int	m_EliteGroupMin;
	//地图出现精英怪的上限
	int	m_EliteGroupMax;

	int GetEliteNumber();

	void Init()
	{
		void merger_();
		void process_auto_spawn(); 
	}
	float m_fStartTime;
private:
	void process_auto_spawn(); 
	void merger_();
};

namespace GameLogic
{
	class LC_SkillTriggerEntry;
}

typedef mem::vector<GameLogic::LC_SkillTriggerEntry *> EliteExternSkillListType;
typedef mem::map<int,GameLogic::LC_SkillTriggerEntry *> EliteExternSkillMapType;

typedef mem::vector<Utility::UT_Vec3Int> PostionLisTypet;

struct EliteMonsterParam
{
	//int m_SkillCount;
	int m_GroupCount;
	int m_MinorCount;

	EliteExternSkillMapType m_SkillList;
	PostionLisTypet	m_PostionList;

	EliteMonsterParam()
		:m_GroupCount(0),m_MinorCount(0)
	{

	}

	EliteMonsterParam(const EliteMonsterParam& rv)
	{
		clone(rv);
	}

	EliteMonsterParam& clone(const EliteMonsterParam& rv)
	{
		//this->m_SkillCount = rv.m_SkillCount;
		this->m_GroupCount = rv.m_GroupCount;
		this->m_SkillList = rv.m_SkillList;
		this->m_MinorCount = rv.m_MinorCount;
		return *this;
	}

	EliteMonsterParam& operator = (const EliteMonsterParam& rv)
	{
		return clone(rv);
	}

	int RandomSkill(EliteExternSkillListType& list);

};

class EliteMonsterParamManager
{
public:
	EliteMonsterParamManager();
	~EliteMonsterParamManager();
	
	int GetMaxSkillCount();
	void Init();
	EliteMonsterParam * GetEliteMonsterParam(int Elite_Level);
	GameLogic::LC_SkillTriggerEntry * GetSkillTrigger(int level,int Rule_Id);

private:
	EliteMonsterParam m_Params[GameLogic::ELT_STAR_MID_MAX];
};


//地下城刷怪数据配置管理，使用cache避免反复读写
class DungeonConfigDataManager : public Memory::MM_BaseObject, public Utility::UT_Singleton<DungeonConfigDataManager>
{
public:
	DungeonConfigDataManager();
	~DungeonConfigDataManager();

	void Init();
	DungeonConfigData * GetConfigData(int MapLogicID);
	Logic_ConfigData_Base * GetConfigDataEx(int MapID);
	EliteMonsterParamManager& GetParamManager(){return m_ParamManger;}
private:
	typedef mem::map<int,DungeonConfigData *> DataMap;
	DataMap m_DataMap;
	typedef mem::map<int,Logic_ConfigData_Base *> Dungeon_Config_DataMap;
	Dungeon_Config_DataMap m_Config_DataMap;
	EliteMonsterParamManager m_ParamManger;
};

DungeonConfigDataManager * Default_DungeonConfigDataManager();

struct MonsterStaus
{
	object_id_type	m_ID;
	int		m_alive;
	MonsterStaus()
		:m_ID(0),m_alive(0)
	{}

	MonsterStaus(const MonsterStaus& rv)
	{
		this->m_ID = rv.m_ID;
		this->m_alive = rv.m_alive;
	}
};

struct MonsterGroupType : public ::Memory::MM_BaseObject
{
	DECL_RTTI_NAME( MonsterGroupType )

	typedef mem::vector<MonsterStaus> ListType;
	int MonsterGroupIndex;
	ListType MonsterStatusList;
	MonsterGroupType()
		:MonsterGroupIndex(0)
	{}

	MonsterGroupType(const MonsterGroupType& rv)
		:MonsterGroupIndex(rv.MonsterGroupIndex)
		,MonsterStatusList(rv.MonsterStatusList)
	{}
};

//精英怪组队管理模块，用于判定一组精英怪是否全部死亡
struct EliteMonsterGroupManager
{
	typedef mem::vector<MonsterGroupType *> MonsterGroupTypeListType;
	typedef mem::map<int,MonsterGroupType*> MonsterGroupTypeMapType;

	typedef mem::vector<int>	AllocGroupIndexList;

	~EliteMonsterGroupManager()
	{
	}

	void Reset()
	{
		for( MonsterGroupTypeListType::iterator i = m_AllGroup.begin(); i != m_AllGroup.end(); ++i)
		{
			T_SAFE_DELETE(*i);
		}
		m_AllGroup.clear();
		m_MonsterMap.clear();
		m_AllocIndexList.clear();
	}

	void Init(int EliteGroupCount , int MonsterGroupCount)
	{
		if (0 == MonsterGroupCount || 0 == EliteGroupCount)
		{
			return;
		}
		int group_count = (EliteGroupCount+MonsterGroupCount-1)/MonsterGroupCount;
		m_AllocIndexList.reserve(group_count*MonsterGroupCount);
		for( int i = 0; i < group_count; ++i )
		{
			for( int j =0; j < MonsterGroupCount; ++j)
			{
				m_AllocIndexList.push_back(j);
			}
		}
	}

	int AllocEliteGroupIndex()
	{
		int value = m_AllocIndexList.back();
		m_AllocIndexList.pop_back();
		return value;
	}

	void RandomAllocIndex();

	void AddGroup(const MonsterGroupType& group)
	{
		MonsterGroupType * the_group = T_NEW_D MonsterGroupType(group);
		m_AllGroup.push_back(the_group);
		for(MonsterGroupType::ListType::iterator i = the_group->MonsterStatusList.begin(); i != the_group->MonsterStatusList.end(); ++i)
		{
			m_MonsterMap[i->m_ID] = the_group;
		}
	}
	//return value:
	//0 Group alive
	//1 Group dead
	//2 No Group Monster Dead
	int MonsterDeadWithGroup(object_id_type MonsterNPCID)
	{
		MonsterGroupTypeMapType::iterator iter = m_MonsterMap.find(MonsterNPCID);
		if(iter == m_MonsterMap.end())
		{
			return 2;
		}
		MonsterGroupType * group = iter->second;
		int alive_count = 0;
		bool found = false;
		for(MonsterGroupType::ListType::iterator i = group->MonsterStatusList.begin(); i != group->MonsterStatusList.end(); ++i)
		{
			if(i->m_ID == MonsterNPCID)
			{
				i->m_alive = 0;
				found = true;
			}
			if(i->m_alive > 0 )
			{
				++alive_count;
			}
			if(alive_count > 0 && found)
			{
				return 0;
			}
		}
		for(MonsterGroupType::ListType::iterator i = group->MonsterStatusList.begin(); i != group->MonsterStatusList.end(); ++i)
		{
			m_MonsterMap.erase(i->m_ID);
		}
		MonsterGroupTypeListType::iterator fiter = std::find(m_AllGroup.begin(),m_AllGroup.end(),group);
		if( fiter != m_AllGroup.end() )
		{
			m_AllGroup.erase(fiter);
		}
		m_AllocIndexList.push_back(group->MonsterGroupIndex);
		T_SAFE_DELETE(group);
		return 1;
	}

	std::size_t GroupNumber()
	{
		return m_MonsterMap.size();
	}

	MonsterGroupTypeListType m_AllGroup;
	MonsterGroupTypeMapType  m_MonsterMap;
	AllocGroupIndexList m_AllocIndexList;
};

class LC_ServerNPC;
namespace GameLogic
{
	struct LC_TranscriptionListEntry;
}

void Random_Skill_List(EliteMonsterParam * param , EliteExternSkillMapType& skill_list);

void Upgrade_Elite_Monster(LC_ServerNPC * npc ,int Elite_Level  , bool IsMinor , EliteMonsterParam * param ,const EliteExternSkillListType& skill_list,int skillcount);

void Fill_Spawn_Monster_Data(const DungeonConfigData::MonsterData& monster_data,GameLogic::LC_TranscriptionListEntry& entry);

DungeonConfigData::EliteMonsterType Random_Elite_Monster(const DungeonConfigData::EliteMonsterTypeListType& list);

void GetRandomPositionOfTheReferencePos(const Utility::UT_Vec3Int& pkReferencePos,mem::vector<Utility::UT_Vec3Int>& posVec , EliteMonsterParam * Param);

#endif
