
#include "MC_UniCastDataDirtyFlag.h"
#include "LC_ServerPlayer.h"
#include "LC_ActorFOVMapBase.h"

MC_UniCastDataDirtyFlag::MC_UniCastDataDirtyFlag(void)
{
	m_playerEnterOrLeave = false;
	m_npcEnterOrLeave = false;
	m_sysnAll = false;
}

MC_UniCastDataDirtyFlag::~MC_UniCastDataDirtyFlag(void)
{
}

void MC_UniCastDataDirtyFlag::SetPlayerEnterOrLeave()
{
	m_playerEnterOrLeave = true;
}
void MC_UniCastDataDirtyFlag::SetNpcEnterOrLeave()
{
	m_npcEnterOrLeave = true;
}
void MC_UniCastDataDirtyFlag::SetSysnAll()
{
	m_sysnAll = true;
}
void MC_UniCastDataDirtyFlag::ClearPlayerEnterOrLeave()
{
	m_playerEnterOrLeave = false;
	GetPlayer()->GetFOVMap()->UpdateLastPlayersIDInFov();
}
void MC_UniCastDataDirtyFlag::ClearNpcEnterOrLeave()
{
	m_npcEnterOrLeave = false;
	GetPlayer()->GetFOVMap()->UpdateLastNPCIDInFOV();
}

void MC_UniCastDataDirtyFlag::ClearSysnAll()
{
	m_sysnAll = false;
}

bool MC_UniCastDataDirtyFlag::IsPlayerEnterOrLeave()
{
	return m_playerEnterOrLeave;
}

bool MC_UniCastDataDirtyFlag::IsNpcEnterOrLeave()
{
	return m_npcEnterOrLeave;
}

bool MC_UniCastDataDirtyFlag::IsSysnAll()
{
	return m_sysnAll;
}

