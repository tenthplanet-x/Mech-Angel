/********************************************************************
	created:	2014/07/15
	filename: 	LC_ServerMap_Trigger.h
	file ext:	h
	
	purpose:	
*********************************************************************/
#ifndef _LC_SERVER_MAP_TRIGGER_H
#define _LC_SERVER_MAP_TRIGGER_H
#include "LC_Define.h"
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include "MM_BaseObject.h"
#include "UT_Shape.h"
enum MapTrigger_Type
{
	MapTrigger_Type_Circle=0,
	MapTrigger_Type_Rect=1
};

enum MapTrigger_State
{
	MapTrigger_State_Ready=0,
	MapTrigger_State_Active=1,
	MapTrigger_State_Distory=2,
};

enum TriggerFilter_Type
{
	TriggerFilter_AllPlayer=0,
	TriggerFilter_SpecialPlayer=1,
	TriggerFilter_AllMonster=2,
	TriggerFilter_SpecialMonsterType=3,
	TriggerFilter_SpecialMonsterID=4,
	TriggerFilter_All=5,
};
class LC_ServerMap;
class LC_ServerMapLogic_Place;
class LC_ServerMap_Trigger : public  Memory::MM_BaseObject
{
public:
	DECL_RTTI_NAME( LC_ServerMap_Trigger )
		typedef boost::function<void(LC_ServerMap_Trigger*,GameLogic::LC_ActorPtrList&,LC_ServerMapLogic_Place*)> TriggerCallBack;
public:
	LC_ServerMap_Trigger(int id,Utility::UT_OBB& obb,TriggerCallBack callback,LC_ServerMap* place,LC_ServerMapLogic_Place* logic);
	LC_ServerMap_Trigger(int id,Utility::UT_Circle& circle,TriggerCallBack callback,LC_ServerMap* place,LC_ServerMapLogic_Place* logic);
	virtual ~LC_ServerMap_Trigger();
	virtual void Start();
	virtual void Stop();
	virtual void Distory();
	virtual void Update(float fCurrentTime,float fDeltaTime);

	MapTrigger_Type GetTriggerType()
	{
		return m_TriggerType;
	}

	Utility::UT_OBB GetOBB()
	{
		return m_Obb;
	}

	Utility::UT_Circle GetCircle()
	{
		return m_Circle;
	}

	int GetId()
	{
		return m_Id;
	}

	void SetAllFilter()
	{
		m_FilterType=TriggerFilter_All;
		m_Parm.clear();
	}
	void SetAllPlayerFilter()
	{
		m_FilterType=TriggerFilter_AllPlayer;
		m_Parm.clear();
	}
	void SetSpecialPlayerFilter(mem::set<int>& specialplayerid)
	{
		m_FilterType=TriggerFilter_SpecialPlayer;
		m_Parm=specialplayerid;
	}
	void SetAllMonsterFilter()
	{
		m_FilterType=TriggerFilter_AllMonster;
		m_Parm.clear();
	}
	void SetSpecialMonsterTypeFilter(mem::set<int>& specialmonstertype)
	{
		m_FilterType=TriggerFilter_SpecialMonsterType;
		m_Parm=specialmonstertype;
	}
	void SetSpecialMonsterIDFilter(mem::set<int>& specialmonsterID)
	{
		m_FilterType=TriggerFilter_SpecialMonsterID;
		m_Parm=specialmonsterID;
	}

	void SetTrigTime(int times)
	{
		if (times<0)
		{
			times=0;
		}
		m_TrigTimes=times;
	}
	int GetTrigTime()
	{
		return m_TrigTimes;
	}
	void ClearTrigTime()
	{
		SetTrigTime(0);
	}

	void SetInterval(int interval)
	{
		m_Interval=interval;
	}
	int GetInterval()
	{
		return m_Interval;
	}

	void SetState(MapTrigger_State state)
	{
		m_State=state;
	}
	bool IsDistory()
	{
		return m_State==MapTrigger_State_Distory;
	}
	bool IsActive()
	{
		return m_State==MapTrigger_State_Active;
	}
private:
	int m_Id;
	MapTrigger_Type m_TriggerType;
	MapTrigger_State m_State;
	Utility::UT_OBB m_Obb;
	Utility::UT_Circle m_Circle;
	int m_Interval;
	TriggerCallBack m_CallBack;
	int m_TrigTimes;
	int m_CurTrigTimes;

	//¹ýÂË
	TriggerFilter_Type m_FilterType;
	mem::set<int> m_Parm;

	float m_CheckTimeStamp;
	float m_IntervalTimeStamp;
	mem::set<object_id_type> m_InActor; 
	LC_ServerMapLogic_Place* m_MapLogic;
	LC_ServerMap* m_Place;
};



#endif