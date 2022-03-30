
#include "PF_ServerPathManager.h"
#include "UT_PathFindHelp.h"

UT_PathFindHelp::UT_PathFindHelp()
{

}

UT_PathFindHelp::~UT_PathFindHelp()
{

}

bool UT_PathFindHelp::GetNextPosInCircle(GameLogic::LC_ActorBase* pkCaller,int orginx,int orginy,int innerRadius,int outRadius,float step,bool clockwise,Utility::UT_Vec3Int& nextpos)
{
	Utility::UT_Vec3Int originPos(orginx,orginy,0);
	Utility::UT_Vec3Int curlocation=pkCaller->GetCurrentLocation();
	nextpos=curlocation;
	if (innerRadius==outRadius)
	{
		return false;
	}
	if (step<=0.0f)
	{
		return false;
	}
	PF_ServerPathManager* pathmanager=PF_ServerPathManager::GetSingletonPtr();
	if (pathmanager==NULL)
	{
		return false;
	}
	int steplength=(int)(((float)pkCaller->GetMoveSpeed())*step);
	if (steplength<=0)
	{
		return false;
	}
	//获取所在地图
	int resid=pkCaller->GetMapResID();
	//随机半径
	int radius=UT_MathBase::RandInRangeInt(innerRadius,outRadius);
	if (radius<=20)
	{
		radius=20;
	}
	int circlelength=(int)(2*UT_MathBase::PI*radius);
	//算出当前角度
	Utility::UT_Vec3Int currentvect=curlocation-originPos;
	Utility::UT_Vec2Int currentvect2(currentvect.x,currentvect.y);

	float currentangel=0.0f;

	int length=(int)UT_MathBase::Sqrt((float)(currentvect.x*currentvect.x+currentvect.y*currentvect.y));
	if (length<=0)
	{
		int ranangel=UT_MathBase::RandInRangeInt(0,360);
		currentangel=(float)ranangel*2*UT_MathBase::PI/(float)360;

	}
	else
	{
		float x=(float)currentvect.x;
		if (x<0)
		{
			x=0.0f-x;
		}
		float cosvalue=x/length;
		currentangel=UT_MathBase::ACos(cosvalue);
		if (currentvect.x>=0 && currentvect.y>=0)
		{
			currentangel+=0;
		}
		else if (currentvect.x>=0 && currentvect.y<=0)
		{
			currentangel=0.0f-currentangel;
		}
		else if (currentvect.x<=0 && currentvect.y>=0)
		{
			currentangel=UT_MathBase::PI-currentangel;
		}
		else
		{
			currentangel=UT_MathBase::PI+currentangel;
		}
	}

	float angel=steplength*2*UT_MathBase::PI/circlelength;
	float deltaangel=angel/2;
	float targetangel=0.0f;
	if (clockwise)
	{
		targetangel=currentangel-angel;
	}
	else
	{
		targetangel=currentangel+angel;
	}

	//检查是否可达
	int targetx=(int)((float)radius*UT_MathBase::Cos(targetangel));
	int targety=(int)((float)radius*UT_MathBase::Sin(targetangel));

	nextpos.x=targetx+originPos.x;
	nextpos.y=targety+originPos.y;
	if (pathmanager->IsPosValid(resid,nextpos))
	{
		return true;
	}

	//向内找一次
	float theangel=0.0f;
	if (clockwise)
	{
		theangel=targetangel+deltaangel;
	}
	else
	{
		theangel=targetangel-deltaangel;
	}
	targetx=(int)((float)radius*UT_MathBase::Cos(theangel));
	targety=(int)((float)radius*UT_MathBase::Sin(theangel));
	nextpos.x=targetx+originPos.x;
	nextpos.y=targety+originPos.y;
	if (pathmanager->IsPosValid(resid,nextpos))
	{
		return true;
	}
	//向外找5次
	int maxtime=10;
	int i=0;
	while(i<maxtime)
	{
		if (clockwise)
		{
			theangel=targetangel-(i+1)*deltaangel;
		}
		else
		{
			theangel=targetangel+(i+1)*deltaangel;
		}
		targetx=(int)((float)radius*UT_MathBase::Cos(theangel));
		targety=(int)((float)radius*UT_MathBase::Sin(theangel));
		nextpos.x=targetx+originPos.x;
		nextpos.y=targety+originPos.y;
		if (pathmanager->IsPosValid(resid,nextpos))
		{
			return true;
		}
		++i;
	}
	//随机10次
	i=0;
	while(i<maxtime)
	{
		//随机角度
		int ranangel=UT_MathBase::RandInRangeInt(0,360);
		float ranangelf=(float)ranangel*2*UT_MathBase::PI/(float)360;
		float theangel=0.0f;
		if (clockwise)
		{
			theangel=targetangel-ranangelf;
		}
		else
		{
			theangel=targetangel+ranangelf;
		}
		targetx=(int)((float)radius*UT_MathBase::Cos(theangel));
		targety=(int)((float)radius*UT_MathBase::Sin(theangel));
		nextpos.x=targetx+originPos.x;
		nextpos.y=targety+originPos.y;
		if (pathmanager->IsPosValid(resid,nextpos))
		{
			return true;
		}
		++i;
	}
	nextpos=curlocation;
	return false;
}

bool UT_PathFindHelp::GenRandomPosInCircle(int res_id, const UT_Vec3Int& rkCircleCenter,int iInnerCircleRadius,int iOuterCircleRadius , UT_Vec3Int& new_pos)
{
	PF_ServerPathManager* pathmanager=PF_ServerPathManager::GetSingletonPtr();
	if (pathmanager==NULL)
	{
		return false;
	}

	UT_Vec3Int pos;
	for(int i = 0; i < 10; ++i)
	{
		pos = UT_MathBase::RandInCircleInt(rkCircleCenter,iInnerCircleRadius,iOuterCircleRadius);
		if (pathmanager->IsPosValid(res_id,pos))
		{
			new_pos = pos;
			return true;
		}
	}
	return false;
}

bool UT_PathFindHelp::GenRandomPosInCircle2(int res_id , const UT_Vec3Int& rkCircleCenter,int CircleRadius,UT_Vec3Int& new_pos)
{
	PF_ServerPathManager* pathmanager=PF_ServerPathManager::GetSingletonPtr();
	if (pathmanager==NULL)
	{
		return false;
	}
	mem::vector<Utility::UT_Vec2Int> result;
	bool ret=pathmanager->GetAllVaildCell(res_id,rkCircleCenter,CircleRadius,result);
	if (!ret || result.empty())
	{
		return false;
	}
	int r=UT_MathBase::RandInRangeInt(0, (int)result.size()-1);
	ret=pathmanager->GetCellPoint(res_id,result[r],new_pos);
	return ret;
}

bool UT_PathFindHelp::GenNearByPosInCircle(int res_id , const UT_Vec3Int& rkCurrentLocation , const UT_Vec3Int& rkTargetLocation ,int CircleRadius,UT_Vec3Int& new_pos)
{
	new_pos = UT_MathBase::ComputeNearByTargetLocation(rkCurrentLocation,rkTargetLocation,CircleRadius);
	PF_ServerPathManager* pathmanager=PF_ServerPathManager::GetSingletonPtr();
	if (pathmanager==NULL)
	{
		return false;
	}
	if(!pathmanager->IsPosValid(res_id,new_pos))
	{
		new_pos = rkTargetLocation;
	}
	return true;
}

bool UT_PathFindHelp::GenRandomPosInRestrictedRing(int res_id, const UT_Vec3Int& rkCircleCenter, int iInnerCircleRadius, int iOuterCircleRadius, 
												   const UT_Vec3Int& restrictedCenter, int restrictedRadius, UT_Vec3Int& new_pos)
{
	PF_ServerPathManager* pathmanager=PF_ServerPathManager::GetSingletonPtr();
	if (pathmanager==NULL)
		return false;

	UT_Vec3Int pos;
	int distance = 0;
	for(int i = 0; i < 10; ++i)
	{
		pos = UT_MathBase::RandInCircleInt(rkCircleCenter, iInnerCircleRadius, iOuterCircleRadius);
		if (pathmanager->IsPosValid(res_id, pos))
		{
			distance = UT_MathBase::LineLengthXYInt(pos, restrictedCenter);
			if (distance < restrictedRadius)
			{
				new_pos = pos;
				return true;
			}
		}
	}
	return false;
}
