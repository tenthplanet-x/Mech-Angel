#pragma once

#include "LC_ClientFriendMsgBroadStruct.h"

class LC_ClientFriendMsgBroad
{
public:
	LC_ClientFriendMsgBroad();
	~LC_ClientFriendMsgBroad();

	DECL_RTTI_NAME( LC_ClientFriendMsgBroad );

	void ReUse();
	
	void AddMsg(long lFriendID, int iMsgType);
	void Delete(long lFriendID);
	void DeleteMsg(long lFriendID, int iIndex);

	int GetMsgCount(long lFriendID);
	LC_ClientFriendMsgBroadStruct* GetMsg(long lFriendID, int iIndex);

private:
	mapFriendIDMsgBroad		m_kFriendIDMsgBroadMap;
};


