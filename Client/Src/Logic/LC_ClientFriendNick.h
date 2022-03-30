#pragma once
//�ͻ����ǳƹ���
#include "LC_FriendNick.h"

class LC_ClientFriendNick
{
public:	
	DECL_RTTI_NAME( LC_ClientFriendNick );

	typedef mem::map<long, LC_FriendNick> mapFriendNick;

	void ReUse();

public:
	long  GetDirtyFlagRevisionID();
	void  ClearDirtyFlagRevisionID();
	void  AddDirtyFlagRevisionID();

public:
	LC_FriendNick* GetFriendNick(long lFriendID);

private:
	mapFriendNick	m_kFriendNick;

	long			m_lDirtyFlagRevisionID;
};