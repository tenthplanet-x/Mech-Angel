#pragma once

#include "LC_PlayerBase.h"
#include <bitset>

using namespace GameLogic;

class LC_ServerPlayer;
class MC_BaseDataDirtyFlag
{
public:
	MC_BaseDataDirtyFlag(void);
	~MC_BaseDataDirtyFlag(void);

	void SetPlayer(LC_ServerPlayer* player);
	LC_ServerPlayer* GetPlayer();

	void ClearAll();

	inline void SetStateType() { SetBroadCastFlag(LC_PlayerBase::E_DirtyFlag_StateType); }
	inline void SetPVEStateType() { SetBroadCastFlag(LC_PlayerBase::E_DirtyFlag_PVEStateType); }
	inline void SetHPMP() { SetBroadCastFlag(LC_PlayerBase::E_DirtyFlag_HPMPChg); }
	inline void SetLocation() { SetBroadCastFlag(LC_PlayerBase::E_DirtyFlag_Location); }
	inline void SetProperty() { SetBroadCastFlag(LC_PlayerBase::E_DirtyFlag_Property); }
	inline void SetControllerProperty() { SetBroadCastFlag(LC_PlayerBase::E_DirtyFlag_ControllerProperty); }
	inline void SetEquip() { SetBroadCastFlag(LC_PlayerBase::E_DirtyFlag_Equip); }
	inline void SetSkillState() { SetBroadCastFlag(LC_PlayerBase::E_DirtyFlag_SkillState); }
	inline void SetInteractionValue() { SetBroadCastFlag(LC_PlayerBase::E_DirtyFlag_InterAction); }
	inline void SetSubordinate() { SetBroadCastFlag(LC_PlayerBase::E_DirtyFlag_Subordinate); }
	inline void SetSocity() { SetBroadCastFlag(LC_PlayerBase::E_DirtyFlag_Socity); }
	inline void SetFastProperty() { SetBroadCastFlag(LC_PlayerBase::E_DirtyFlag_PropertyFast); }
	inline void SetExtProperty() { SetBroadCastFlag(LC_PlayerBase::E_DirtyFlag_PropertyExt); }
	inline void SetAngle() { SetBroadCastFlag(LC_PlayerBase::E_DirtyFlag_Angle); }

	void SetMoveSpeed();
	void ClearSpeed();
	bool IsMoveSpeedChange()const;

	void SetBroadCastFlag(int nFlag);
	void SetBroadCastFlagForce(int nFlag);
	void ClearBroadCastFlag(int nFlag);
	bool IsBroadCastFlag(int nFlag) const;
private:
	void _setBroadCastFlagForce(int nFlag);
	void _setBroadCastFlag(int nFlag);
	void _clearBroadCastFlag(int nFlag);
	bool _isBroadCastFlag(int nFlag) const;
private:
	//std::map<int, bool>	m_rDirtyMap;
	std::bitset<LC_PlayerBase::E_DirtyFlag_Count> m_rDirtyMap;
	int					m_moveSpeed;
	LC_ServerPlayer*	m_player;
};

static int m_gnAllBuddy[LC_PlayerBase::E_DirtyFlag_Count] =
{
	0,//E_DirtyFlag_MoveSpeed = 0,
	0,//E_DirtyFlag_StateType,
	1,//E_DirtyFlag_PVEStateType,
	0,//E_DirtyFlag_HPMPChg,
	0,//E_DirtyFlag_Location,
	1,//E_DirtyFlag_Property,
	0,//E_DirtyFlag_Equip,
	0,//E_DirtyFlag_SkillState,
	0,//E_DirtyFlag_InterAction,
	1,//E_DirtyFlag_Subordinate,
	1,//E_DirtyFlag_Socity,
	1,//E_DirtyFlag_PropertyFast,
	1,//E_DirtyFlag_PropertyExt,
	1,//E_DirtyFlag_ControllerProperty,
	1,//E_DirtyFlag_Angle,
};
