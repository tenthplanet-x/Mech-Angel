#pragma once

class LC_ClientFriendMsgBroadStruct
{
public:
	DECL_RTTI_NAME( LC_ClientFriendMsgBroadStruct );

	MEMBER_STRING(Time);
	MEMBER_STRING(Title);
	MEMBER_STRING(Content);
};

typedef mem::vector<LC_ClientFriendMsgBroadStruct> vecClientFriendMsgBroad;

typedef mem::map<long, vecClientFriendMsgBroad> mapFriendIDMsgBroad;

