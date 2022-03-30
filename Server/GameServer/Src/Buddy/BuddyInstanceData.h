#ifndef __BUDDY_INSTANCE_DATA_H__
#define __BUDDY_INSTANCE_DATA_H__

#pragma once

#include "LC_DirtyFlag.h"
#include "LC_Define.h"

#include "char_data.pb.h"

#include "BuddyInstanceData_Attacker.h"
#include "BuddyInstanceData_ActorBase.h"
#include "BuddyInstanceData_PlayerBase.h"
#include "BuddyInstanceData_ServerPlayer.h"

class BuddyInstanceData_Base
{
public:
	BuddyInstanceData_Base()
		: m_nStarLevel(0)
		, m_nTotalActivatedSkillCount(0)
		, m_bAttrSync(false)
		, m_bAttrDirty(false)
		, m_nStarLevel_Orig(-1)
		, m_nLevel_Orig(-1)
		, m_bLockAssignedPlayerId(false)
		, m_nCurrentTransformID(0)
	{
		m_nActiveTransformRecord.clear();
		m_nInvaildTransformTime.clear();
		m_ActivateSkillID.clear();
		m_UpgradeSkillID.clear();
	}
	~BuddyInstanceData_Base()
	{
		m_nActiveTransformRecord.clear();
		m_nInvaildTransformTime.clear();
		m_ActivateSkillID.clear();
		m_UpgradeSkillID.clear();
	}

public:
	int32_t m_nStarLevel;
	int32_t m_nTotalActivatedSkillCount;
	bool m_bAttrSync;
	bool m_bAttrDirty;
	int32_t m_nStarLevel_Orig;
	int32_t m_nLevel_Orig;
	bool m_bLockAssignedPlayerId;

	int32_t m_nCurrentTransformID;//buddy现在使用的皮肤id
	mem::vector<int32_t> m_nActiveTransformRecord;//buddy现在激活的皮肤id
	mem::vector<BuddyTransformInvaildTime> m_nInvaildTransformTime;//皮肤的到期时间（皮肤id,到期时间）

	mem::vector<int32_t> m_ActivateSkillID;//觉醒的技能id
	mem::set<int32_t> m_UpgradeSkillID;//玩家申请升级的技能id
};

class LC_ServerPlayer;
class BuddyInstanceData //: public GameLogic::LC_ServerPlayer
{
public:
	BuddyInstanceData(void){}
	~BuddyInstanceData(){};

	void Init(int32_t buddyID)
	{
		m_Data_Attacker.Init();
		m_Data_ActorBase.Init();
		m_Data_PlayerBase.Init(buddyID);
		m_Data_ServerPlayer.Init();
	}
	void Clear(void){}

	//int LoadFromDB();
	//int LoadFromTemplate(int32_t nTemplateId);

	//const BuddyInstance* Get();
	//int SaveData();
	// DBLogic
	BuddyInstanceData_Base * GetBuddyInstanceData_Base(){return &m_Data_Base;}
	BuddyInstanceData_PlayerBase * GetBuddyInstanceData_PlayerBase(){return &m_Data_PlayerBase;}
protected:
	friend class BuddyInstance;
	// for base data
	BuddyInstanceData_Base m_Data_Base;
	// from LC_Attacker
	BuddyInstanceData_Attacker m_Data_Attacker;
	// from LC_ActorBase
	BuddyInstanceData_ActorBase m_Data_ActorBase;
	// from LC_PlayerBase
	BuddyInstanceData_PlayerBase m_Data_PlayerBase;
	// from LC_ServerPlayer
	BuddyInstanceData_ServerPlayer m_Data_ServerPlayer;
};

#endif //__ACT_ACTIVITY_ASSET_H__
