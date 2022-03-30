#include "LC_ServerMap_Trigger.h"
#include "LC_ServerMap.h"
#include "CF_ActorFilterFunc.h"
using namespace GameLogic;
using namespace CSVFile;
using namespace Protocol;
using namespace Utility;

LC_ServerMap_Trigger::LC_ServerMap_Trigger(int id,Utility::UT_OBB& obb,TriggerCallBack callback,LC_ServerMap* place,LC_ServerMapLogic_Place* logic):
m_Id(id),m_TriggerType(MapTrigger_Type_Rect),m_State(MapTrigger_State_Ready),m_Obb(obb),
m_Interval(0),m_CallBack(callback),m_TrigTimes(0),m_CurTrigTimes(0),
m_FilterType(TriggerFilter_AllPlayer),m_Parm(),m_CheckTimeStamp(0.0f),m_IntervalTimeStamp(0.0f),
m_InActor(),m_MapLogic(logic),m_Place(place)
{

}

LC_ServerMap_Trigger::LC_ServerMap_Trigger(int id,Utility::UT_Circle& circle,TriggerCallBack callback,LC_ServerMap* place,LC_ServerMapLogic_Place* logic):
m_Id(id),m_TriggerType(MapTrigger_Type_Circle),m_State(MapTrigger_State_Ready),m_Circle(circle),
m_Interval(0),m_CallBack(callback),m_TrigTimes(0),m_CurTrigTimes(0),
m_FilterType(TriggerFilter_AllPlayer),m_Parm(),m_CheckTimeStamp(0.0f),m_IntervalTimeStamp(0.0f),
m_InActor(),m_MapLogic(logic),m_Place(place)
{
	
}

LC_ServerMap_Trigger::~LC_ServerMap_Trigger()
{
	m_Place=NULL;
	m_MapLogic=NULL;
}

void LC_ServerMap_Trigger::Start()
{
	SetState(MapTrigger_State_Active);
}

void LC_ServerMap_Trigger::Stop()
{
	SetState(MapTrigger_State_Ready);
}

void LC_ServerMap_Trigger::Distory()
{
	SetState(MapTrigger_State_Distory);
	m_Place->DelTrigger(GetId());
}

void LC_ServerMap_Trigger::Update(float fCurrentTime,float fDeltaTime)
{
	if (!IsActive())
	{
		return;
	}

	if (fCurrentTime<m_IntervalTimeStamp)
	{
		return;
	}

	if (fCurrentTime<m_CheckTimeStamp)
	{
		return;
	}
	m_CheckTimeStamp=fCurrentTime+0.5f;
	CF_ActorFilterFunc* pkCSVActorFilterFunc = CF_ActorFilterFunc::GetSingletonPtr();
	CF_ActorFilterFunc::DataEntry* pkFilterData = pkCSVActorFilterFunc->GetEntryPtr(0);
	if (NULL == pkFilterData)
	{
		return;
	}
	
	LC_ActorPtrVector* pkTargetList = NULL;
	if (m_TriggerType==MapTrigger_Type_Circle)
	{
		pkTargetList = m_Place->LockActorsInCircle(m_Circle,pkFilterData);
	}
	else if (m_TriggerType==MapTrigger_Type_Rect)
	{
		pkTargetList = m_Place->LockActorsInRect(m_Obb,pkFilterData);
	}
	else
	{
		return;
	}

	if (pkTargetList->empty())
	{
		return;
	}
	LC_ActorPtrVector alltrigactor;
	alltrigactor.reserve(pkTargetList->size());
	//¹ýÂË
	if (m_FilterType==TriggerFilter_AllPlayer)
	{
		LC_ActorPtrVector::iterator beg=pkTargetList->begin();
		for (;beg!=pkTargetList->end();++beg)
		{
			if (IS_PLAYER_CLASS_ID((*beg)->GetID()))
			{
				alltrigactor.push_back((*beg));
			}
		}
	}
	else if (m_FilterType==TriggerFilter_SpecialPlayer)
	{
		LC_ActorPtrVector::iterator beg=pkTargetList->begin();
		for (;beg!=pkTargetList->end();++beg)
		{
			if (IS_PLAYER_CLASS_ID((*beg)->GetID()) && m_Parm.count((*beg)->GetID())>0)
			{
				alltrigactor.push_back((*beg));
			}
		}
	}
	else if (m_FilterType==TriggerFilter_AllMonster)
	{
		LC_ActorPtrVector::iterator beg=pkTargetList->begin();
		for (;beg!=pkTargetList->end();++beg)
		{
			if (IS_NPC_ID((*beg)->GetID()))
			{
				alltrigactor.push_back((*beg));
			}
		}
	}
	else if (m_FilterType==TriggerFilter_SpecialMonsterType)
	{
		LC_ActorPtrVector::iterator beg=pkTargetList->begin();
		for (;beg!=pkTargetList->end();++beg)
		{
			if (IS_NPC_ID((*beg)->GetID()) && m_Parm.count((*beg)->GetCharType())>0)
			{
				alltrigactor.push_back((*beg));
			}
		}
	}
	else if (m_FilterType==TriggerFilter_SpecialMonsterID)
	{
		LC_ActorPtrVector::iterator beg=pkTargetList->begin();
		for (;beg!=pkTargetList->end();++beg)
		{
			if (IS_NPC_ID((*beg)->GetID()) && m_Parm.count((*beg)->GetID())>0)
			{
				alltrigactor.push_back((*beg));
			}
		}
	}
	else
	{
		LC_ActorPtrVector::iterator beg=pkTargetList->begin();
		for (;beg!=pkTargetList->end();++beg)
		{
			alltrigactor.push_back((*beg));
		}
	}
	LC_ActorPtrList result;
	LC_ActorPtrVector::iterator beg=alltrigactor.begin();
	for (;beg!=alltrigactor.end();++beg)
	{
		if(m_InActor.count((*beg)->GetID())<=0)
		{
			result.push_back((*beg));
		}
	}
	m_InActor.clear();
	beg=alltrigactor.begin();
	for (;beg!=alltrigactor.end();++beg)
	{
		m_InActor.insert((*beg)->GetID());
	}
	if (result.empty())
	{
		return;
	}
	m_CallBack(this,result,m_MapLogic);
	if (m_Interval>0)
	{
		m_IntervalTimeStamp=fCurrentTime+m_Interval;
	}
	else if (m_TrigTimes>0)
	{
		++m_CurTrigTimes;
		if (m_CurTrigTimes>=m_TrigTimes)
		{
			m_CurTrigTimes=0;
			SetState(MapTrigger_State_Ready);
		}
	}
}
