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
	WeaponType							m_eWeaponAttribute;			//��ɫ�������ͺ͵�ǰ��������
	StringType							m_kCharName_gb;				//��ǰ����
	StringType							m_kCharName_utf8;		//utf-8����
	long								m_lPunchFlyDistance;	//�����ɾ���			//���ˡ���������
	float								m_fPunchFlyTime;		//������ʱ��
	float								m_fPunchStayTime;		//���˺�ֱʱ��
	SkillMoveActorPositionType			m_ePunchMoveType;		//��������				//���ˡ����ɵ�ǰ����
	float								m_fPunchMoveTime;		//����ʱ��
	Utility::UT_Vec3Int					m_kPunchMovePos;		//����Ŀ���
	int									m_lRiderCharType;		//��������
	int									m_lLevel;				//�ȼ�
	int									m_lMaxLevel;
	int64_t								m_lEXP;					//����
	float								m_fEXPPlus;
	attr_value_type						m_lHP;					//����ֵ
	attr_value_type						m_lOldHP;
	float								m_fChangeTimeHP;
	attr_value_type						m_lMP;					//��Ϣֵ

	int									m_lShapeShiftCharTypeID;
	LC_ActorStateMap*					m_kSkillStateMap;					//Buff״̬����
	// ���棬�����ָ�
	// 1 ���־û�
	// 2 
	mem::map<int, StateCacheData> m_kStateCache;

};

#endif //__ACT_ACTIVITY_ASSET_H__
