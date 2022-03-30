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
	CareerType						m_eCareerType;							//ְҵ
	uint32_t						m_uiPlayerFlag;
	LC_SkillAsset		  			m_kSkillAsset;							//�����ʲ�
	LC_ShortcutBar*					m_pkShortcutBar;						//���������

	CRuneEquipPack m_RuneEquipPack;//buddy�ı��߲�λ
	CBuddyEquipPack m_BuddyEquipPack;//buddy������Ϣ

	bool	m_nLevelUpFlag;//������־

	//�ɳ�������
	bool									m_nBeActived;									//�Ƿ񼤻��˳ɳ����
	uint64_t								m_nActiveTime;								//�ɳ�����ļ���ʱ��
	int32_t									m_nDay;											//�Ѿ���ȡ��n��
};

#endif //__ACT_ACTIVITY_ASSET_H__
