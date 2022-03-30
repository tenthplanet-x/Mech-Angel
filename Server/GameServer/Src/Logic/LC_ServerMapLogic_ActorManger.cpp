#include "LC_ServerMapLogic_Place.h"
#include "LC_ServerMapLogic_ActorManger.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerNPCManager.h"


LC_ServerMapLogic_ActorManger::LC_ServerMapLogic_ActorManger():
m_parent(NULL),m_id_begin(0)
{
}

LC_ServerMapLogic_ActorManger::~LC_ServerMapLogic_ActorManger()
{
	m_parent = NULL;
}

bool LC_ServerMapLogic_ActorManger::Init(LC_ServerMapLogic_Place* parent)
{
	m_id_begin=0;
	m_logicActorMap.clear();
	m_organizeMap.clear();
	m_parent=parent;
	return true;
}

void LC_ServerMapLogic_ActorManger::UnInit()
{
	m_id_begin=0;
	m_logicActorMap.clear();
	m_organizeMap.clear();
	m_parent=NULL;
}

bool LC_ServerMapLogic_ActorManger::AddActor(object_id_type ActorID,Logic_Actor_State initstate,int actortype,int abnormaltime)
{
	LogicActorMap::iterator actorIt=m_logicActorMap.find(ActorID);
	if(actorIt!=m_logicActorMap.end())
	{
		return false;
	}
	LC_ServerMapLogic_Actor& actor = m_logicActorMap[ActorID];
	actor.m_pOwnerMgr = this;
	actor.ActorID=ActorID;
	actor.Typeid=actortype;
	actor.SetState(initstate);
	if(actor.IsOffline())
	{
		float nowtime = GET_PROCESS_TIME;
		actor.SetTime(nowtime+abnormaltime);
	}
	else
	{
		actor.SetTime(0);
	}
	return true;
}

bool LC_ServerMapLogic_ActorManger::AddActor2Organize(object_id_type ActorID, object_id_type organizeId)
{
	OrganizeMap::iterator organizeIt = m_organizeMap.find(organizeId);
	if(organizeIt == m_organizeMap.end())
	{
		return false;
	}
	if (organizeIt->second.IsIn(ActorID))
	{
		return false;
	}

	LogicActorMap::iterator actorIt=m_logicActorMap.find(ActorID);
	if(actorIt==m_logicActorMap.end())
	{
		return false;
	}
	organizeIt->second.AddActor(ActorID);
	actorIt->second.AddOrganizeId(organizeId);

	Group_Change_Map changemap;
	{
		mem::vector<object_id_type>& temp = changemap[Group_Change_Type_All];
		int curallsize = GetAllActorSizeInOrganize(organizeId);
		temp.push_back(curallsize-1);
		temp.push_back(curallsize);
	}

	if (actorIt->second.CheckIn())
	{
		mem::vector<object_id_type>& temp = changemap[Group_Change_Type_In];
		int curinsize=GetInActorSizeInOrganize(organizeId);
		temp.push_back(curinsize-1);
		temp.push_back(curinsize);
	}

	if (actorIt->second.IsActive())
	{
		mem::vector<object_id_type>& temp = changemap[Group_Change_Type_Active];
		int curactivesize=GetActiveActorSizeInOrganize(organizeId);
		temp.push_back(curactivesize-1);
		temp.push_back(curactivesize);
	}

	m_parent->ActorGroupSizeChange(organizeId,changemap,ActorID);
	return true;
}

LC_ServerMapLogic_Organize* LC_ServerMapLogic_ActorManger::CreateOrganize()
{
	++m_id_begin;
	LC_ServerMapLogic_Organize temp(m_id_begin);
	std::pair<OrganizeMap::iterator, bool> ret=m_organizeMap.insert(std::make_pair(m_id_begin, temp));
	if(ret.second)
	{
		return &(ret.first->second);
	}
	return NULL;
}

//[changetype,presize,nowsize]
bool LC_ServerMapLogic_ActorManger::SetActorState(object_id_type ActorID,Logic_Actor_State State,int abnormaltime)
{
	LogicActorMap::iterator actorIt = m_logicActorMap.find(ActorID);
	if (actorIt==m_logicActorMap.end())
	{
		return false;
	}

	if (actorIt->second.CheckState(State))
	{
		return false;
	}

	if(State==Logic_Actor_State_OFFLINE)
	{
		float nowtime=GET_PROCESS_TIME;
		actorIt->second.SetTime(nowtime+abnormaltime);
	}
	else
	{
		actorIt->second.SetTime(0);
	}
	bool preactive=actorIt->second.IsActive();
	bool prein=actorIt->second.CheckIn();
	bool nowactive=ActorCheckActive(State);
	bool nowin=ActorCheckIn(State);
	//判断是走是留
	if(prein!=nowin || preactive!=nowactive)
	{
		mem::map<object_id_type, mem::vector<int> > countrecord;
		LC_ServerMapLogic_Actor::OrganizeSet::iterator idbeg=actorIt->second.organizeIds.begin();
		for (;idbeg!=actorIt->second.organizeIds.end();++idbeg)
		{
			LC_ServerMapLogic_Organize* organize=GetOrganize(*idbeg);
			if(organize==NULL)
				continue;

			int curinsize=GetInActorSizeInOrganize(*idbeg);
			int curactivesize=GetActiveActorSizeInOrganize(*idbeg);

			mem::vector<int>& temp = countrecord[organize->Organize_id];
			temp.push_back(curinsize);
			temp.push_back(curactivesize);
		}
		if (countrecord.empty())
		{
			actorIt->second.SetState(State);
			return true;
		}
		
		if ((prein!=nowin && nowin) || (preactive!=nowactive && nowactive))//进入
		{
			actorIt->second.SetState(State);
			mem::map<object_id_type, mem::vector<int> >::iterator beg=countrecord.begin();
			for (;beg!=countrecord.end();++beg)
			{
				Group_Change_Map changemap;
				if (prein!=nowin)
				{
					mem::vector<object_id_type>& temp = changemap[Group_Change_Type_In];
					temp.push_back(beg->second[0]);
					temp.push_back(beg->second[0]+1);
				}
				if (preactive!=nowactive)
				{
					mem::vector<object_id_type>& temp = changemap[Group_Change_Type_Active];
					temp.push_back(beg->second[1]);
					temp.push_back(beg->second[1]+1);
				}
				if (!changemap.empty())
				{
					m_parent->ActorGroupSizeChange(beg->first,changemap,ActorID);
				}
			}
		}
		else//出去
		{
			
			mem::map<object_id_type, mem::vector<int> >::iterator beg=countrecord.begin();
			for (;beg!=countrecord.end();++beg)
			{
				Group_Change_Map changemap;
				if (prein!=nowin)
				{
					mem::vector<object_id_type>& temp = changemap[Group_Change_Type_In];
					temp.push_back(beg->second[0]);
					temp.push_back(beg->second[0]-1);
				}
				if (preactive!=nowactive)
				{
					mem::vector<object_id_type>& temp = changemap[Group_Change_Type_Active];
					temp.push_back(beg->second[1]);
					temp.push_back(beg->second[1]-1);
				}
				if (!changemap.empty())
				{
					m_parent->ActorGroupSizeChange(beg->first,changemap,ActorID);
				}
			}
			actorIt->second.SetState(State);
		}
	}
	else
	{
		actorIt->second.SetState(State);
	}
	
	return true;
}

void LC_ServerMapLogic_ActorManger::DelActor(object_id_type ActorID)
{
	LogicActorMap::iterator actorIt=m_logicActorMap.find(ActorID);
	if (actorIt==m_logicActorMap.end())
	{
		return;
	}

	LC_IDVector delorgs;
	delorgs.insert(delorgs.begin(), actorIt->second.organizeIds.begin(), actorIt->second.organizeIds.end());
	LC_IDVector::iterator beg=delorgs.begin();
	for (;beg!=delorgs.end();++beg)
	{
		DelActorInOrganize(ActorID, *beg);
	}
	m_logicActorMap.erase(actorIt);
}

void LC_ServerMapLogic_ActorManger::DelActorInOrganize(object_id_type ActorID,int orgid)
{
	OrganizeMap::iterator organizeIt=m_organizeMap.find(orgid);
	if(organizeIt==m_organizeMap.end())
		return;

	if (!organizeIt->second.IsIn(ActorID))
	{
		return;
	}

	LogicActorMap::iterator actorIt=m_logicActorMap.find(ActorID);
	if(actorIt==m_logicActorMap.end())
		return;

	Group_Change_Map changemap;
	{
		mem::vector<object_id_type>& temp = changemap[Group_Change_Type_All];
		int allsize=organizeIt->second.GetSize();
		temp.push_back(allsize);
		temp.push_back(allsize-1);
	}

	if (actorIt->second.CheckIn())
	{
		mem::vector<object_id_type>& temp = changemap[Group_Change_Type_In];
		int insize=GetInActorSizeInOrganize(orgid);
		temp.push_back(insize);
		temp.push_back(insize-1);
	}

	if (actorIt->second.IsActive())
	{
		mem::vector<object_id_type>& temp = changemap[Group_Change_Type_Active];
		int activesize=GetActiveActorSizeInOrganize(orgid);
		temp.push_back(activesize);
		temp.push_back(activesize-1);
	}
	
	m_parent->ActorGroupSizeChange(orgid, changemap, ActorID);
	
	organizeIt=m_organizeMap.find(orgid);
	if(organizeIt!=m_organizeMap.end())
	{
		organizeIt->second.DelActor(ActorID);
	}

	actorIt=m_logicActorMap.find(ActorID);
	if(actorIt!=m_logicActorMap.end())
	{
		actorIt->second.DelOrganizeId(orgid);
	}	
}

LC_ServerMapLogic_Actor* LC_ServerMapLogic_ActorManger::GetActor(object_id_type ActorID)
{
	LogicActorMap::iterator it = m_logicActorMap.find(ActorID);
	return (it==m_logicActorMap.end()) ? NULL : &(it->second);
}

LC_ServerMapLogic_Organize* LC_ServerMapLogic_ActorManger::GetOrganize(int OrgId)
{
	OrganizeMap::iterator it = m_organizeMap.find(OrgId);
	return (it==m_organizeMap.end()) ? NULL : &(it->second);
}

const object_id_set* LC_ServerMapLogic_ActorManger::GetAllActorIDsInOrganize(int organizeId)//获取已经在副本中的，包括掉线的
{
	OrganizeMap::iterator it=m_organizeMap.find(organizeId);
	return (it==m_organizeMap.end()) ? NULL : &it->second.ActorId_set;
}

int32_t LC_ServerMapLogic_ActorManger::GetAllActorSizeInOrganize(int organizeId)//获取已经在副本中的，包括掉线的
{
	OrganizeMap::iterator it=m_organizeMap.find(organizeId);
	return (it==m_organizeMap.end()) ? 0 : (int32_t)it->second.ActorId_set.size();
}

object_id_vector LC_ServerMapLogic_ActorManger::GetInActorListInOrganize(int organizeId)//获取已经在副本中的，包括掉线的
{
	object_id_vector ret;
	OrganizeMap::iterator organizeIt=m_organizeMap.find(organizeId);
	if(organizeIt==m_organizeMap.end())
		return ret;

	object_id_set::iterator beg = organizeIt->second.ActorId_set.begin();
	while (beg!=organizeIt->second.ActorId_set.end())
	{
		LC_ServerMapLogic_Actor* actor=GetActor(*beg);
		if(actor && actor->CheckIn())
		{
			ret.push_back(*beg);
		}
		++beg;
	}
	return ret;
}

int32_t LC_ServerMapLogic_ActorManger::GetInActorSizeInOrganize(int organizeId)
{
	OrganizeMap::iterator organizeIt=m_organizeMap.find(organizeId);
	if(organizeIt==m_organizeMap.end())
		return 0;

	//Logic_Actor_State_ACTIVE || actorstate==Logic_Actor_State_OFFLINE)
	int32_t nResult1 = organizeIt->second.GetCountByState(Logic_Actor_State_ACTIVE);
	int32_t nResult2 = organizeIt->second.GetCountByState(Logic_Actor_State_OFFLINE);
	int32_t nResult3 = nResult1 + nResult2;
	return nResult3;
	/*
	int32_t count=nResult3;
	object_id_set::iterator beg=organizeIt->second.ActorId_set.begin();
	while (beg!=organizeIt->second.ActorId_set.end())
	{
		LC_ServerMapLogic_Actor* actor=GetActor(*beg);
		if(actor && actor->CheckIn())
		{
			++count;
		}
		++beg;
	}
	if ( nResult3 != count )
	{
		printf( "counter error\n" );
	}
	return count;
	*/
}

object_id_vector LC_ServerMapLogic_ActorManger::GetActiveActorListInOrganize(int organizeId)
{
	object_id_vector ret;
	OrganizeMap::iterator organizeIt=m_organizeMap.find(organizeId);
	if(organizeIt==m_organizeMap.end())
		return ret;
	object_id_set::iterator beg=organizeIt->second.ActorId_set.begin();
	while (beg!=organizeIt->second.ActorId_set.end())
	{
		LC_ServerMapLogic_Actor* actor=GetActor(*beg);
		if(actor && actor->IsActive())
		{
			ret.push_back(*beg);
		}
		++beg;
	}
	return ret;
}
int32_t LC_ServerMapLogic_ActorManger::GetActiveActorSizeInOrganize(int organizeId)
{
	OrganizeMap::iterator organizeIt=m_organizeMap.find(organizeId);
	if(organizeIt==m_organizeMap.end())
		return 0;

	int32_t nResult = organizeIt->second.GetCountByState(Logic_Actor_State_ACTIVE);
	return nResult;
/*
	int32_t count=0;
	object_id_set::iterator beg=organizeIt->second.ActorId_set.begin();
	while (beg!=organizeIt->second.ActorId_set.end())
	{
		LC_ServerMapLogic_Actor* actor=GetActor(*beg);
		if(actor && actor->IsActive())
		{
			++count;
		}
		++beg;
	}
	return count;
*/
}

LC_ServerMapLogic_Organize* LC_ServerMapLogic_ActorManger::OrganizeActor(const object_id_vector& ActorIds)
{
	if(ActorIds.empty())
	{
		return NULL;
	}
	++m_id_begin;
	LC_ServerMapLogic_Organize temp(m_id_begin);
	std::pair<OrganizeMap::iterator, bool> ret=m_organizeMap.insert(std::make_pair(m_id_begin, temp));
	if (!ret.second)
	{
		return NULL;
	}
	LC_ServerMapLogic_Organize& rTemp = ret.first->second;
	object_id_vector::const_iterator idbeg=ActorIds.begin();
	for (;idbeg!=ActorIds.end();++idbeg)
	{
		LC_ServerMapLogic_Actor* actor=GetActor(*idbeg);
		if(actor==NULL)
		{
			continue;
		}
		rTemp.AddActor(*idbeg);
		actor->AddOrganizeId(m_id_begin);
	}
	if(rTemp.GetSize()<=0)
	{
		m_organizeMap.erase(ret.first);
		return NULL;
	}
	/*int curallsize=rTemp.GetSize();
	int cursize=GetInActorSizeInOrganize(id_begin);
	m_parent->ActorGroupSizeChange(id_begin,grouptype,0,cursize,0,curallsize,0);*/
	return &(rTemp);
}

LC_ServerMapLogic_Organize* LC_ServerMapLogic_ActorManger::OrganizeActorbyType(int ActorType)
{
	++m_id_begin;
	LC_ServerMapLogic_Organize temp(m_id_begin);
	std::pair<OrganizeMap::iterator, bool> ret=m_organizeMap.insert(std::make_pair(m_id_begin, temp));
	if (!ret.second)
	{
		return NULL;
	}
	LC_ServerMapLogic_Organize& rTemp = ret.first->second;
	LogicActorMap::iterator idbeg=m_logicActorMap.begin();
	for (;idbeg!=m_logicActorMap.end();++idbeg)
	{
		if(idbeg->second.Typeid==ActorType)
		{
			rTemp.AddActor(idbeg->first);
			idbeg->second.AddOrganizeId(m_id_begin);
		}
		
	}
	if(rTemp.GetSize()<=0)
	{
		m_organizeMap.erase(ret.first);
		return NULL;
	}
	/*int curallsize=rTemp.GetSize();
	int cursize=GetInActorSizeInOrganize(m_id_begin);
	if(curallsize!=0 || cursize!=0)
		m_parent->ActorGroupSizeChange(m_id_begin,grouptype,0,cursize,0,curallsize,0);*/
	return &(rTemp);
}

void LC_ServerMapLogic_ActorManger::UnOrganizeActor(int orgid)
{
	OrganizeMap::iterator groupit=m_organizeMap.find(orgid);
	if(groupit==m_organizeMap.end())
		return;
	object_id_set::iterator npcbeg=groupit->second.ActorId_set.begin();
	for (;npcbeg!=groupit->second.ActorId_set.end();++npcbeg)
	{
		LC_ServerMapLogic_Actor* actor=GetActor(*npcbeg);
		if(actor==NULL)
			continue;
		actor->DelOrganizeId(orgid);
		
	}
	/*if(preallsize!=0 || presize!=0)
		m_parent->ActorGroupSizeChange(orgid,groupit->second.Organize_type,presize,0,preallsize,0,0);*/
	m_organizeMap.erase(groupit);
}

LC_ServerNPC* LC_ServerMapLogic_ActorManger::GetNpc(object_id_type npcid)
{
	static LC_ServerNPCManager* pkServerNPCManager = SERVER_GET_NPC_MANAGER();
	return (IS_NPC_ID(npcid)/*&&pkServerNPCManager*/) ? (LC_ServerNPC*)(pkServerNPCManager->FindNPC(npcid)) : NULL;
}

LC_ServerPlayer* LC_ServerMapLogic_ActorManger::GetPlayer(object_id_type playerid)
{
	static LC_ServerPlayerManager* pkServerplayerManager = SERVER_GET_PLAYER_MANAGER();
	return (IS_PLAYER_CLASS_ID(playerid)&&pkServerplayerManager) ? (LC_ServerPlayer*)(pkServerplayerManager->FindPlayer(playerid)) : NULL;
}

LC_IDVector LC_ServerMapLogic_ActorManger::GetActorOrganize(object_id_type npcid)
{
	LC_IDVector idlist;
	LC_ServerMapLogic_Actor* actor=GetActor(npcid);
	if(actor==NULL)
		return idlist;
	if(actor->organizeIds.empty())
	{
		return idlist;
	}
	LC_ServerMapLogic_Actor::OrganizeSet::iterator orgbeg=actor->organizeIds.begin();
	for (;orgbeg!=actor->organizeIds.end();++orgbeg)
	{
		OrganizeMap::iterator groupit=m_organizeMap.find(*orgbeg);
		if(groupit==m_organizeMap.end())
			continue;
		idlist.push_back(groupit->second.Organize_id);
	}
	return idlist;
}


object_id_vector LC_ServerMapLogic_ActorManger::GetPlayerList()
{
	object_id_vector ret;
	LogicActorMap::iterator it=m_logicActorMap.begin();
	for (;it!=m_logicActorMap.end();++it)
	{
		if(IS_PLAYER_CLASS_ID(it->second.ActorID))
		{
			ret.push_back(it->second.ActorID);
		}
	}
	return ret;
}

object_id_vector LC_ServerMapLogic_ActorManger::GetNpcList()
{
	object_id_vector ret;
	LogicActorMap::iterator it=m_logicActorMap.begin();
	for (;it!=m_logicActorMap.end();++it)
	{
		if(IS_NPC_ID(it->second.ActorID))
		{
			ret.push_back(it->second.ActorID);
		}
	}
	return ret;
}

LC_ServerMapLogic_Actor * LC_ServerMapLogic_ActorManger::GetActorInOrganize(object_id_type ActorID, int OrgId)
{
	LC_ServerMapLogic_Organize* org = GetOrganize(OrgId);
	if(org==NULL)
		return NULL;
	if (org->ActorId_set.count(ActorID)<=0)
	{
		return NULL;
	}
	return GetActor(ActorID);
}
//---------------------------------------------------------------------------
int32_t LC_ServerMapLogic_ActorManger::GetCountByState(int nOrganizeId, Logic_Actor_State nState)
{
	if ( nState >= Logic_Actor_State_Count )
		return 0;

	OrganizeMap::iterator it = m_organizeMap.find( nOrganizeId );
	if ( it == m_organizeMap.end() )
		return 0;

	return it->second.GetCountByState(nState);
}

bool LC_ServerMapLogic_ActorManger::IncCountByState(int nOrganizeId, Logic_Actor_State nState)
{
	if ( nState >= Logic_Actor_State_Count )
		return false;

	OrganizeMap::iterator it = m_organizeMap.find( nOrganizeId );
	if ( it == m_organizeMap.end() )
		return false;

	it->second.IncCountByState( nState );
	return true;
}

bool LC_ServerMapLogic_ActorManger::DecCountByState(int nOrganizeId, Logic_Actor_State nState)
{
	if ( nState >= Logic_Actor_State_Count )
		return false;

	OrganizeMap::iterator it = m_organizeMap.find( nOrganizeId );
	if ( it == m_organizeMap.end() )
		return false;

	it->second.DecCountByState( nState );
	return true;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/

void LC_ServerMapLogic_Actor::IncCount( int nOrgId )
{
	m_pOwnerMgr->IncCountByState( nOrgId, State );
}

void LC_ServerMapLogic_Actor::DecCount(int nOrgId)
{
	m_pOwnerMgr->DecCountByState( nOrgId, State );
}

void LC_ServerMapLogic_Actor::IncCount()
{
	OrganizeSet::iterator it =  organizeIds.begin();
	for ( ;it != organizeIds.end(); ++it )
	{
		m_pOwnerMgr->IncCountByState( *it, State );
	}
}

void LC_ServerMapLogic_Actor::DecCount()
{
	OrganizeSet::iterator it =  organizeIds.begin();
	for ( ;it != organizeIds.end(); ++it )
	{
		m_pOwnerMgr->DecCountByState( *it, State );
	}
}

void LC_ServerMapLogic_Actor::SetState(Logic_Actor_State state)
{
	if (state==State)
	{
		return;
	}

	if ( m_pOwnerMgr == NULL )
		return;

	bool prein=CheckIn();
	DecCount();
	//m_pOwnerMgr->DecCountByState(State);
	State = state;
	IncCount();
	//m_pOwnerMgr->IncCountByState(State);
	bool nowin=CheckIn();
	if (prein!=nowin)
	{
		if (prein)
		{
			begintime=0.0f;
		}
		else
		{
			begintime=GET_PROCESS_TIME;
		}
	}
};

void LC_ServerMapLogic_Actor::AddOrganizeId(object_id_type Id)
{
	organizeIds.insert(Id);
	IncCount(Id);
};

void LC_ServerMapLogic_Actor::DelOrganizeId(object_id_type Id)
{
	organizeIds.erase(Id);
	DecCount(Id);
}

