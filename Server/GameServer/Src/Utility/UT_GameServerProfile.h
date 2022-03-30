#ifndef _UT_GAME_SREVER_PROFILE_H
#define _UT_GAME_SERVER_PROFILE_H

#include "MM_BaseObject.h"
#include "StringDefine.h"

class UT_GameServerProfile : public Memory::MM_BaseObject
{
public:
	UT_GameServerProfile(const StringType& rkNodeName,float fTolerantTime);
	~UT_GameServerProfile();

protected:
	uint64_t	m_uiStartTime;
	uint64_t	m_uiTolerantTime;
	StringType	m_kNodeName;
};

//#define UT_SERVER_PROFILE(_profile, _name, _time) UT_GameServerProfile _profile(_name, _time)
#define UT_SERVER_PROFILE(_profile, _name, _time) //

#endif