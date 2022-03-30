#ifndef _LC_SERVER_MAP_LOGIC_ACTORMANAGER_H
#define _LC_SERVER_MAP_LOGIC_ACTORMANAGER_H
/***************************************************************
	Author:				sunny
	Time:				2013-10-9			
	FileName:			LC_ServerMapLogic_ActorManger
	FileExt:			.h
	Porpose:			场景逻辑角色管理,此处只是提供了一个管理你需要关注的角色的地方，并不是所有的怪都放在这里
****************************************************************/
#include "UT_TimeManager.h"

enum Group_Change_Type
{
	Group_Change_Type_All,
	Group_Change_Type_In,
	Group_Change_Type_Active
};
typedef mem::map<object_id_type,mem::vector<object_id_type> > Group_Change_Map;
//[changetype,presize,nowsize]
enum Logic_Actor_State
{
	// 逻辑没有设置掩码，所以废弃这种定义
	//Logic_Actor_State_ACTIVE=0x0000,
	//Logic_Actor_State_Loading=0x0001,
	//Logic_Actor_State_OFFLINE=0x0002,
	//Logic_Actor_State_Leave=0x0004,
	Logic_Actor_State_ACTIVE=0,
	Logic_Actor_State_Loading,
	Logic_Actor_State_OFFLINE,
	Logic_Actor_State_Leave,
	Logic_Actor_State_Count,
};

struct LC_ServerMapLogic_Organize
{
	int32_t Organize_id;
	object_id_set ActorId_set;
	int32_t m_nCountByState[Logic_Actor_State_Count];

	LC_ServerMapLogic_Organize(int32_t organize_id)
		:Organize_id(organize_id)
		,ActorId_set()
	{
		for ( int i = 0; i < Logic_Actor_State_Count; i++ )
		{
			m_nCountByState[i] = 0;
		}
	};

	LC_ServerMapLogic_Organize(const LC_ServerMapLogic_Organize& othergroup):
	Organize_id(othergroup.Organize_id),ActorId_set(othergroup.ActorId_set)
	{
		for ( int i = 0; i < Logic_Actor_State_Count; i++ )
		{
			m_nCountByState[i] = 0;
		}
	};

	inline int32_t GetSize()
	{
		return (int32_t)ActorId_set.size();
	};

	inline void AddActor(object_id_type ActorID)
	{
		ActorId_set.insert(ActorID);
	};

	inline void DelActor(object_id_type ActorID)
	{
		ActorId_set.erase(ActorID);
	};

	inline bool IsIn(object_id_type ActorID)
	{
		return ActorId_set.count(ActorID) > 0;
	}
private:
	friend class LC_ServerMapLogic_ActorManger;
	int32_t GetCountByState(Logic_Actor_State nState)
	{
		return m_nCountByState[nState];
	}

	void IncCountByState(Logic_Actor_State nState)
	{
		m_nCountByState[nState]++;
	}

	void DecCountByState(Logic_Actor_State nState)
	{
		m_nCountByState[nState]--;
	}
};

static bool ActorCheckActive(Logic_Actor_State actorstate)
{
	return actorstate==Logic_Actor_State_ACTIVE;
}
static bool ActorCheckActiveOrLoading(Logic_Actor_State actorstate)
{
	if (actorstate==Logic_Actor_State_ACTIVE || actorstate==Logic_Actor_State_Loading)
	{
		return true;
	}
	return false;
}
static bool ActorCheckIn(Logic_Actor_State actorstate)
{
	if (actorstate==Logic_Actor_State_ACTIVE || actorstate==Logic_Actor_State_OFFLINE)
	{
		return true;
	}
	return false;
}

struct LC_ServerMapLogic_Actor
{
	typedef mem::set<int32_t> OrganizeSet;

	object_id_type		ActorID;
	Logic_Actor_State	State;//状态
	int32_t				Typeid;
	float				recordTime;
	float				begintime;
	OrganizeSet			organizeIds;
	LC_ServerMapLogic_ActorManger* m_pOwnerMgr;

	LC_ServerMapLogic_Actor()
		: ActorID(0),
		State(Logic_Actor_State_Loading),
		Typeid(-1),
		recordTime(0.0),
		begintime(0.0f),
		m_pOwnerMgr(NULL)
	{

	};
	inline bool IsActive()
	{
		return ActorCheckActive(State);
	};
	inline bool IsActiveOrLoading()
	{
		return ActorCheckActiveOrLoading(State);
	};
	inline bool IsLoading()
	{
		return State==Logic_Actor_State_Loading;
	};
	inline bool IsOffline()
	{
		return State==Logic_Actor_State_OFFLINE;
	};
	inline bool IsOfflineTimeOut()
	{
		return true;//temp for db back
		return GET_PROCESS_TIME>=recordTime;
	};
	inline bool IsLeave()
	{
		return State==Logic_Actor_State_Leave;
	};
	void SetState(Logic_Actor_State state);

	inline bool CheckState(Logic_Actor_State state)
	{
		return state==State;
	};

	inline bool CheckIn()
	{
		return ActorCheckIn(State);
	};

	inline void SetTime(float time)
	{
		recordTime=time;
	};

	void AddOrganizeId(object_id_type Id);

	void DelOrganizeId(object_id_type Id);

	inline float GetDuringTime()
	{
		return (begintime<=1.0f) ? 0 : (GET_PROCESS_TIME-begintime);
	}

	void IncCount();
	void DecCount();
	void IncCount( int32_t nOrgId );
	void DecCount( int32_t nOrgId );
};

typedef mem::hash_map<object_id_type, LC_ServerMapLogic_Actor>	LogicActorMap;
typedef mem::hash_map<object_id_type, LC_ServerMapLogic_Organize> OrganizeMap;
typedef mem::vector<LC_ServerMapLogic_Organize*> OrganizeVector;
typedef Utility::UT_SimOperator<bool, LC_ServerMapLogic_Actor*> LC_LogicActorOperator;

//对角色进行管理
class LC_ServerMapLogic_Place;
class LC_ServerNPC;


class LC_ServerMapLogic_ActorManger
{
public:

	LC_ServerMapLogic_ActorManger();
	virtual ~LC_ServerMapLogic_ActorManger();
	bool Init(LC_ServerMapLogic_Place* parent);
	void UnInit();
	LC_ServerMapLogic_Organize* CreateOrganize();
	bool AddActor2Organize(object_id_type ActorID, object_id_type organizeId);
	bool AddActor(object_id_type ActorID, Logic_Actor_State initstate, int actortype=-1, int abnormaltime=0);
	bool SetActorState(object_id_type ActorID, Logic_Actor_State State, int abnormaltime=0);
	void DelActor(object_id_type ActorID);
	void DelActorInOrganize(object_id_type ActorID, int orgid);
	LC_ServerMapLogic_Actor * GetActor(object_id_type ActorID);
	LC_ServerMapLogic_Organize* GetOrganize(int OrgId);
	int32_t GetAllActorSizeInOrganize(int organizeId);
	const object_id_set* GetAllActorIDsInOrganize(int organizeId);//获取所有的
	object_id_vector GetInActorListInOrganize(int organizeId);//获取已经在副本中的，包括掉线的
	int32_t GetInActorSizeInOrganize(int organizeId);
	object_id_vector GetActiveActorListInOrganize(int organizeId);//获取已经在副本中的，包括掉线的
	int32_t GetActiveActorSizeInOrganize(int organizeId);
	
	LC_ServerMapLogic_Organize* OrganizeActor(const object_id_vector& ActorIds);
	LC_ServerMapLogic_Organize* OrganizeActorbyType(int ActorType);
	void UnOrganizeActor(int orgid);
	LC_ServerNPC* GetNpc(object_id_type npcid);
	LC_ServerPlayer* GetPlayer(object_id_type playerid);
	GameLogic::LC_IDVector GetActorOrganize(object_id_type npcid);
	object_id_vector GetPlayerList();
	object_id_vector GetNpcList();
	int32_t GetAllActorSize(){return (int32_t)m_logicActorMap.size();};
	LC_ServerMapLogic_Actor * GetActorInOrganize(object_id_type ActorID, int OrgId);
	
	int32_t GetCountByState(int nOrganizeId, Logic_Actor_State nState);
	bool IncCountByState(int nOrganizeId, Logic_Actor_State nState);
	bool DecCountByState(int nOrganizeId, Logic_Actor_State nState);
private:
	LC_ServerMapLogic_Place*	m_parent;
	LogicActorMap				m_logicActorMap;
	OrganizeMap					m_organizeMap;


	int32_t						m_id_begin;
};
#endif