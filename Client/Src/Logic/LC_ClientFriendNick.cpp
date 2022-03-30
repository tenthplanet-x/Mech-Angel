#include "ClientPCH.h"

#include "LC_ClientFriendNick.h"

void LC_ClientFriendNick::ReUse()
{
	m_kFriendNick.clear();
	ClearDirtyFlagRevisionID();
}

long LC_ClientFriendNick::GetDirtyFlagRevisionID()
{
	return m_lDirtyFlagRevisionID;
}

void LC_ClientFriendNick::ClearDirtyFlagRevisionID()
{
	m_lDirtyFlagRevisionID = 0;
}

void LC_ClientFriendNick::AddDirtyFlagRevisionID()
{
	++m_lDirtyFlagRevisionID;
}

LC_FriendNick* LC_ClientFriendNick::GetFriendNick(long lFriendID)
{
	mapFriendNick::iterator it = m_kFriendNick.find(lFriendID);
	if (it == m_kFriendNick.end())
	{
		LC_FriendNick kFriendNick;
		m_kFriendNick[lFriendID] = kFriendNick;
	}

	it = m_kFriendNick.find(lFriendID);
	if (it != m_kFriendNick.end())
	{
		LC_FriendNick& kFriendNick = it->second;
		return &kFriendNick;
	}

	return NULL;
}