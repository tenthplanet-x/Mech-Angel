#include "ClientPCH.h"

#include "LC_AuctionStruct.h"

using namespace GameLogic;

//////////////////////////////////////////////////////////////////////////
void LC_ClientAddItemInfo::Reset()
{
	m_kItemAffix.m_byValid = 0;

	ClearDirtyFlag();
	ClearDirtyFlagRevisionID();
}


bool LC_ClientAddItemInfo::AddItemAffix(int iPackType, int iIndex)
{
	m_kItemAffix.m_byValid = 1;
	m_kItemAffix.m_byPackType = (uint8_t) iPackType;
	m_kItemAffix.m_usItemIndex = (uint16_t) iIndex;
	SetDirtyFlag(true);
	AddDirtyFlagRevisionID();

	return true;
}

bool LC_ClientAddItemInfo::DeleletItemAffix()
{
	m_kItemAffix.m_byValid = 0;
	SetDirtyFlag(true);
	AddDirtyFlagRevisionID();

	return true;
}

bool LC_ClientAddItemInfo::IsValid()
{
	return m_kItemAffix.m_byValid != 0;
}

int  LC_ClientAddItemInfo::GetPackType()
{
	return m_kItemAffix.m_byPackType;
}

int	 LC_ClientAddItemInfo::GetIndex()
{	
	return m_kItemAffix.m_usItemIndex;
}

void LC_ClientAddItemInfo::SetDirtyFlag(bool bFlag)
{
	m_bDirtyFlag = bFlag;
}

bool LC_ClientAddItemInfo::GetDirtyFlag()
{
	return m_bDirtyFlag;
}

void LC_ClientAddItemInfo::ClearDirtyFlag()
{
	m_bDirtyFlag = false;
}

void LC_ClientAddItemInfo::AddDirtyFlagRevisionID()
{
	++m_lDirtyFlagRevisionID;
}

long LC_ClientAddItemInfo::GetDirtyFlagRevisionID()
{
	return m_lDirtyFlagRevisionID;
}

void LC_ClientAddItemInfo::SetDirtyFlagRevisionID(long lRevisionID)
{
	m_lDirtyFlagRevisionID = lRevisionID;
}

void LC_ClientAddItemInfo::ClearDirtyFlagRevisionID()
{
	m_lDirtyFlagRevisionID = 0;
}
