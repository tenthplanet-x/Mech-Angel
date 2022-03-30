#include "MC_BaseDataDirtyFlag.h"
#include "LC_ServerPlayer.h"
#include "LC_PlayerBase.h"
using namespace GameLogic;

MC_BaseDataDirtyFlag::MC_BaseDataDirtyFlag(void): m_player(NULL)
{
	ClearAll();
}

MC_BaseDataDirtyFlag::~MC_BaseDataDirtyFlag(void)
{
}

void MC_BaseDataDirtyFlag::SetPlayer( LC_ServerPlayer* player )
{
	m_player = player;
}

LC_ServerPlayer* MC_BaseDataDirtyFlag::GetPlayer()
{
	return m_player;
}

void MC_BaseDataDirtyFlag::ClearAll()
{
	m_moveSpeed = 0;
	//m_rDirtyMap.clear();
	m_rDirtyMap.reset();
}

void MC_BaseDataDirtyFlag::SetMoveSpeed()
{
	if (_isBroadCastFlag(LC_PlayerBase::E_DirtyFlag_MoveSpeed) == false && m_player)
	{
		m_moveSpeed = m_player->GetMoveSpeed();
	}
	_setBroadCastFlag(LC_PlayerBase::E_DirtyFlag_MoveSpeed);
}

void MC_BaseDataDirtyFlag::ClearSpeed()
{
	_clearBroadCastFlag(LC_PlayerBase::E_DirtyFlag_MoveSpeed);
	if (m_player)
	{
		m_moveSpeed = m_player->GetMoveSpeed();
	}
}

bool MC_BaseDataDirtyFlag::IsMoveSpeedChange()const
{
	bool change = false;
	if (NULL == m_player)
	{
		change = _isBroadCastFlag(LC_PlayerBase::E_DirtyFlag_MoveSpeed);
	}
	else
	{
		change = _isBroadCastFlag(LC_PlayerBase::E_DirtyFlag_MoveSpeed) & (m_moveSpeed != m_player->GetMoveSpeed());
	}
	return change;
}
////////////////////////////////////////////////////////////////////////
void MC_BaseDataDirtyFlag::SetBroadCastFlag(int nFlag)
{
	switch(nFlag)
	{
	case LC_PlayerBase::E_DirtyFlag_MoveSpeed:
		{
			SetMoveSpeed();
		}
		break;
	default:
		{
			_setBroadCastFlag(nFlag);
		}
		break;
	}
}

void MC_BaseDataDirtyFlag::ClearBroadCastFlag(int nFlag)
{
	switch(nFlag)
	{
	case LC_PlayerBase::E_DirtyFlag_MoveSpeed:
		{
			ClearSpeed();
		}
		break;
	default:
		{
			_clearBroadCastFlag(nFlag);
		}
		break;
	}
}

bool MC_BaseDataDirtyFlag::IsBroadCastFlag(int nFlag) const
{
	switch(nFlag)
	{
	case LC_PlayerBase::E_DirtyFlag_MoveSpeed:
		{
			return IsMoveSpeedChange();
		}
		break;
	default:
		{
			return _isBroadCastFlag(nFlag);
		}
		break;
	}
	return false;
}

void MC_BaseDataDirtyFlag::SetBroadCastFlagForce(int nFlag)
{
	switch(nFlag)
	{
	case LC_PlayerBase::E_DirtyFlag_MoveSpeed:
		{
			SetMoveSpeed();
		}
		break;
	default:
		{
			_setBroadCastFlagForce(nFlag);
		}
		break;
	}
}

void MC_BaseDataDirtyFlag::_setBroadCastFlag(int nFlag)
{
	//m_player为空，说明当前是怪的dirtyFlag
	if(NULL == m_player)
	{
		m_rDirtyMap[nFlag] = true;
		return;
	}

	if (nFlag < LC_PlayerBase::E_DirtyFlag_Count && m_gnAllBuddy[nFlag] == 1)
	{
		mem::vector<int32_t> vecBuddyIDs;
		LC_ServerPlayer* pkOwnerPlayer = m_player->GetOwnerPlayer();
		if(NULL == pkOwnerPlayer)
		{
			return;
		}

		pkOwnerPlayer->GetActivateBuddyIDs( vecBuddyIDs);
		for (int i = 0; i < vecBuddyIDs.size(); i++)
		{
			LC_ServerPlayer* pBuddy = pkOwnerPlayer->GetActivateBuddyByID(vecBuddyIDs[i]);
			if(NULL != pBuddy)
			{
				pBuddy->SetBroadCastFlagForce(nFlag);
			}
		}
	}
	else
	{
		m_rDirtyMap[nFlag] = true;
	}
}

void MC_BaseDataDirtyFlag::_setBroadCastFlagForce(int nFlag)
{
	//m_rDirtyMap[nFlag] = true;
	if ( nFlag >= LC_PlayerBase::E_DirtyFlag_Count )
		return;
	m_rDirtyMap.set( nFlag );
}

void MC_BaseDataDirtyFlag::_clearBroadCastFlag(int nFlag)
{
	//m_rDirtyMap[nFlag] = false;
	if ( nFlag >= LC_PlayerBase::E_DirtyFlag_Count )
		return;
	m_rDirtyMap.reset( nFlag );
}

bool MC_BaseDataDirtyFlag::_isBroadCastFlag(int nFlag) const
{
	if ( nFlag >= LC_PlayerBase::E_DirtyFlag_Count )
		return false;
	//std::map<int, bool>::const_iterator iter = m_rDirtyMap.find(nFlag);
	//if(iter == m_rDirtyMap.end())
	//{
	//	return false;
	//}
	//return iter->second;
	return m_rDirtyMap[nFlag];
}
