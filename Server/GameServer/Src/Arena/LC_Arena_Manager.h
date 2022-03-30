#ifndef _LC_ARENA_MANAGER_H
#define _LC_ARENA_MANAGER_H

#pragma once

#include "msg.pb.h"

struct ArenaChar
{
	ArenaChar(void):m_CharID(0), m_ObjectID(0), m_FactionID(0), m_KillTimes(0), m_DeadTimes(0)
	{}

	inline const unique_id_impl& CharID(void) {return m_CharID;}
	inline object_id_type ObjectID(void) {return m_ObjectID;}
	inline int32_t FactionID(void) {return m_FactionID;}
	inline int32_t DeadTimes(void) {return m_DeadTimes;}

	unique_id_impl m_CharID;
	object_id_type m_ObjectID;
	int32_t m_FactionID;
	Utility::UT_Vec3Int m_Location;
	int32_t	m_KillTimes;
	int32_t	m_DeadTimes;
};
typedef mem::map<unique_id_type, ArenaChar> ArenaCharMap;

struct Arena_Info :public Memory::MM_BaseObject
{
	Arena_Info(void) : map_logic_id(0), state(0), uptime(GET_PROCESS_TIME){}
	Arena_Info(const msg_define::arena_info& aif, const ::google::protobuf::RepeatedField< ::google::protobuf::uint64 >& chars);
	
	int32_t Type(void) const {return arena_if.type();}
	int64_t ID(void) const {return arena_if.id();}
	int32_t	TargetServer(void) const {return arena_if.target().id();}
	int32_t MapLogicID(void) const {return map_logic_id;}

	inline void AddIn(const unique_id_impl& charid) {inchars.insert(charid);}
	inline bool IsIn(const unique_id_impl& charid) const {return inchars.count(charid) > 0;}
	inline void CharLeave(const unique_id_impl& charid) { inchars.erase(charid);}
	inline const unique_id_set& InChars(void) const {return inchars;}

	msg_define::arena_info		arena_if;
	int32_t						map_logic_id;
	int32_t						state;
	float						uptime;
	unique_id_set				inchars;
};		

class LC_Arena_Manager;
class LC_Arena_Manager:public Memory::MM_BaseObject
{
public:
	typedef mem::map<int64_t, Arena_Info> ArenaInfoMap;
	typedef mem::map<unique_id_type, float> ArenaSignUpMap;
	
	LC_Arena_Manager();
	virtual ~LC_Arena_Manager();
	DECL_RTTI_NAME( LC_Arena_Manager )
	
	bool			init();
	void			fin();

	Arena_Info*		GetArenaInfo(int64_t iArenaID);
	Arena_Info*		GetArenaInfoByCharID(const unique_id_impl& charID);

	Arena_Info*		AddArenaInfo(const msg_define::arena_info& aif, const ::google::protobuf::RepeatedField< ::google::protobuf::uint64 >& chars);
	void			DelArenaInfo(int64_t iArenaID);
	void			SyncArenaInfo(int64_t iArenaID);
	void			SyncArenaInfoToClient(int64_t iArenaID);

	void			ResetArenaOnServer(int32_t iServerID);

	void			CharLeave(LC_ServerPlayer* pkPlayer);
	void			CharEnter(LC_ServerPlayer* pkPlayer);

	inline bool		IsCharSignValid(const unique_id_impl& charID, float currentTime)
	{
		ArenaSignUpMap::iterator it = m_ArenaSignups.find(charID);
		return (it!=m_ArenaSignups.end() && it->second + 80.f > currentTime);
	}
	ResultType		ArenaSign(LC_ServerPlayer* pkPlayer, float currentTime);
	ResultType		ArenaUnsign(const unique_id_impl& charID);
	ResultType		ArenaEnter(int64_t iArenaID, LC_ServerPlayer* pkPlayer, int32_t serverID, user_id_type userID, const unique_id_impl& charID, object_id_type objectID);
	ResultType		ArenaLeave(int64_t iArenaID, LC_ServerPlayer* pkPlayer);
	ResultType		ArenaPrepareTimes(int64_t iArenaID);
	
	ResultType		ArenaCheck(int64_t iArenaID);
	ResultType		ArenaPrepare(int64_t iArenaID);

	ResultType		SetArenaResult(int64_t iArenaID, ArenaCharMap& rkArenaData);

	void			Update(float fCurrentTime);
private:
	ArenaInfoMap	m_ArenaMap;
	ArenaSignUpMap	m_ArenaSignups;
	float			m_LastUpdateTime;
};
#endif