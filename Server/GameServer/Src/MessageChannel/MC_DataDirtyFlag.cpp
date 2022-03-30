#include "MC_DataDirtyFlag.h"

using namespace GameLogic;

MC_DataDirtyFlag::MC_DataDirtyFlag(void)
{
}

MC_DataDirtyFlag::~MC_DataDirtyFlag(void)
{
}

MC_UniCastDataDirtyFlag& MC_DataDirtyFlag::GetUniCastFlag()
{
	return m_uniCastDataDirtyFlag;
}

MC_BroadCastDataDirtyFlag& MC_DataDirtyFlag::GetBroadCastFlag()
{
	return m_broadCastDataDirtyFlag;
}

void MC_DataDirtyFlag::InitFlagForAllBuddy()
{
	for (int i = 0; i < LC_PlayerBase::E_DirtyFlag_Count; i++)
	{
		if (m_gnAllBuddy[i] == 1)
		{
			m_broadCastDataDirtyFlag.SetBroadCastFlagForce(i);
			m_uniCastDataDirtyFlag.SetBroadCastFlagForce(i);
		}
	}
	
}

void MC_DataDirtyFlag::SetPlayer( LC_ServerPlayer* player )
{
	m_broadCastDataDirtyFlag.SetPlayer(player);
	m_uniCastDataDirtyFlag.SetPlayer(player);
}

void MC_DataDirtyFlag::Update( GameLogic::DirtyFlag flag )
{
	switch (flag)
	{
	case DIRTY_FLAG_FIGHT_STATE:
		m_broadCastDataDirtyFlag.SetPVEStateType();
		break;
	case DIRTY_FLAG_PLAYERS_ENTER_IN_FOV:
		m_uniCastDataDirtyFlag.SetPlayerEnterOrLeave();
		break;
	case DIRTY_FLAG_PLAYERS_LEAVE_IN_FOV:
		m_uniCastDataDirtyFlag.SetPlayerEnterOrLeave();
		break;
	case DIRTY_FLAG_NPC_ENTER_IN_FOV:
		m_uniCastDataDirtyFlag.SetNpcEnterOrLeave();
		break;
	case DIRTY_FLAG_NPC_LEAVE_IN_FOV:
		m_uniCastDataDirtyFlag.SetNpcEnterOrLeave();
		break;
	case DIRTY_FLAG_SKILLSTATE:
		m_broadCastDataDirtyFlag.SetSkillState();
		break;
	default:
		break;
	}
}

void MC_DataDirtyFlag::SetMoveSpeed()
{
	m_broadCastDataDirtyFlag.SetMoveSpeed();
}

void MC_DataDirtyFlag::SetStateType()
{
	m_broadCastDataDirtyFlag.SetStateType();
}

void MC_DataDirtyFlag::SetPVEState()
{
	m_broadCastDataDirtyFlag.SetPVEStateType();
}
void MC_DataDirtyFlag::SetHPMP()
{
	m_broadCastDataDirtyFlag.SetHPMP();
}

void MC_DataDirtyFlag::SetLocation()
{
	m_broadCastDataDirtyFlag.SetLocation();
}

void MC_DataDirtyFlag::SetPlayerEnterOrLeave()
{
	m_uniCastDataDirtyFlag.SetPlayerEnterOrLeave();
}

void MC_DataDirtyFlag::SetInteractionValue()
{
	m_broadCastDataDirtyFlag.SetInteractionValue();
}