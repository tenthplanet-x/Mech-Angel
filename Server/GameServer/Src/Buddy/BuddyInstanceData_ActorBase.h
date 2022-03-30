#ifndef __BUDDY_INSTANCE_DATA_ACTOR_BASE_H__
#define __BUDDY_INSTANCE_DATA_ACTOR_BASE_H__

#pragma once

#include "LC_DirtyFlag.h"
#include "LC_Define.h"

#include "char_data.pb.h"
#include "LC_Attacker.h"
#include "LC_TargetMap.h"
#include "LC_ActorAttributeMap.h"
#include "LC_ActorStateMap.h"
#include "LC_PlayerGroupBase.h"
#include "LC_ActorHatredList.h"
#include "LC_ActorSkillList.h"
#include "LC_ActorSkillTrigger.h"

#include "SK_State.h"
#include "SK_SkillExecute.h"

#include "SM_ControllerBase.h"
#include "LC_Money.h"
#include "LC_DirtyFlag.h"
#include "UT_MathBase.h"
#include "UT_Helper.h"

#include "BuddyInstanceData.h"

#include <map>

class LC_ServerPlayer;

using namespace GameLogic;

class StateCacheData
{
public:
	int lStateTypeID;
	object_id_type lStateAttacherID;
	int nPara0;
	int nPara1;
	int nPara2;
	int nPara3; 
	int nPara4;
};

class BuddyInstanceData_ActorBase //: public GameLogic::LC_ServerPlayer
{
public:
	BuddyInstanceData_ActorBase(void)
		: m_lRiderCharType(0)
		, m_lLevel(1)
		, m_lMaxLevel( 9527 )
		, m_lEXP( 0 )
		, m_fEXPPlus(0)
		, m_lHP( 10 )
		, m_lOldHP( 10 )
		, m_fChangeTimeHP(0)
		, m_lMP( 0 )
		, m_lShapeShiftCharTypeID( 0 )
	{
	}
	~BuddyInstanceData_ActorBase() {};

	void Init(){}
	void Clear(void){}

	//int LoadFromDB();
	//int LoadFromTemplate(int32_t nTemplateId);

	//const BuddyInstance* Get();
	//int SaveData();
	// DBLogic
protected:
	friend class BuddyInstanceData;
	friend class BuddyInstance;
	// from ActorBase
	WeaponType							m_eWeaponAttribute;			//角色武器类型和当前武器类型
	StringType							m_kCharName_gb;				//当前名字
	StringType							m_kCharName_utf8;		//utf-8名字
	long								m_lPunchFlyDistance;	//被击飞距离			//击退、击飞属性
	float								m_fPunchFlyTime;		//被击飞时间
	float								m_fPunchStayTime;		//击退后僵直时间
	SkillMoveActorPositionType			m_ePunchMoveType;		//击退类型				//击退、击飞当前参数
	float								m_fPunchMoveTime;		//击退时间
	Utility::UT_Vec3Int					m_kPunchMovePos;		//击退目标点
	int									m_lRiderCharType;		//坐骑数据
	int									m_lLevel;				//等级
	int									m_lMaxLevel;
	int64_t								m_lEXP;					//经验
	float								m_fEXPPlus;
	attr_value_type						m_lHP;					//生命值
	attr_value_type						m_lOldHP;
	float								m_fChangeTimeHP;
	attr_value_type						m_lMP;					//内息值

	int									m_lShapeShiftCharTypeID;
	LC_ActorStateMap*					m_kSkillStateMap;					//Buff状态管理
	// 缓存，用来恢复
	// 1 不持久化
	// 2 
	mem::map<int, StateCacheData> m_kStateCache;

};

#endif //__ACT_ACTIVITY_ASSET_H__
