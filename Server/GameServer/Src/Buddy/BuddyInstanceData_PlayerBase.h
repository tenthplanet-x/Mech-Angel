#ifndef __BUDDY_INSTANCE_DATA_PLAYER_BASE_H__
#define __BUDDY_INSTANCE_DATA_PLAYER_BASE_H__

#pragma once

#include "LC_PlayerBase.h"
#include "LC_BuddyEquipPack.h"
#include "LC_RuneEquipPack.h"
#include "GlobalSettings.h"

class LC_ServerPlayer;
class BuddyInstanceData_PlayerBase //: public GameLogic::LC_ServerPlayer
{
public:
	BuddyInstanceData_PlayerBase(void) : m_nBeActived(false),m_nActiveTime(0),m_nDay(0), m_nLevelUpFlag(false)
	{
	}

	~BuddyInstanceData_PlayerBase() 
	{
	}

	void Init(int32_t buddyID)
	{
		m_BuddyEquipPack.Init(GetGlobalSetting.ArtifactPackSize);
		m_RuneEquipPack.Init(buddyID, GetGlobalSetting.RunePackSize);
	}

	void Clear(void){}

	//int LoadFromDB();
	//int LoadFromTemplate(int32_t nTemplateId);

	//const BuddyInstance* Get();
	//int SaveData();
	// DBLogic

public:
	friend class BuddyInstanceData;
	friend class BuddyInstance;
	// from LC_PlayerBase
	CareerType						m_eCareerType;							//职业
	uint32_t						m_uiPlayerFlag;
	LC_SkillAsset		  			m_kSkillAsset;							//技能资产
	LC_ShortcutBar*					m_pkShortcutBar;						//快捷栏数据

	CRuneEquipPack m_RuneEquipPack;//buddy的宝具槽位
	CBuddyEquipPack m_BuddyEquipPack;//buddy纹章信息

	bool	m_nLevelUpFlag;//升级标志

	//成长礼包相关
	bool									m_nBeActived;									//是否激活了成长礼包
	uint64_t								m_nActiveTime;								//成长礼包的激活时间
	int32_t									m_nDay;											//已经领取了n天
};

#endif //__ACT_ACTIVITY_ASSET_H__
