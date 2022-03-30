/********************************************************************
	created:	2013/12/04
	filename: 	UT_PathFindHelp.h
	file path:	Src\Utility
	file base:	UT_PathFindHelp
	file ext:	h
	author:		sunny
	
	purpose:	
*********************************************************************/
#ifndef _UT_PATHFINDHELP_H
#define _UT_PATHFINDHELP_H

#include "MM_BaseObject.h"
#include "LC_ActorBase.h"
#include "UT_Vec.h"
using namespace Utility;

class UT_PathFindHelp : public Memory::MM_BaseObject
{
public:
	UT_PathFindHelp();
	~UT_PathFindHelp();
	static bool GetNextPosInCircle(GameLogic::LC_ActorBase* pkCaller,int orginx,int orginy,int innerRadius,int outRadius,float step,bool clockwise,Utility::UT_Vec3Int& nextpos);
	static bool	GenRandomPosInCircle(int res_id , const UT_Vec3Int& rkCircleCenter,int iInnerCircleRadius,int iOuterCircleRadius , UT_Vec3Int& new_pos);
	static bool GenRandomPosInCircle2(int res_id , const UT_Vec3Int& rkCircleCenter,int CircleRadius,UT_Vec3Int& new_pos);//此函数遍历周围的格子，如果有点一定会返回
	static bool GenNearByPosInCircle(int res_id , const UT_Vec3Int& rkCurrentLocation , const UT_Vec3Int& rkTargetLocation ,int CircleRadius,UT_Vec3Int& new_pos);//此函数遍历周围的格子，如果有点一定会返回
	static bool GenRandomPosInRestrictedRing(int res_id, const UT_Vec3Int& rkCircleCenter, int iInnerCircleRadius, int iOuterCircleRadius, const UT_Vec3Int& restrictedCenter, int restrictedRadius, UT_Vec3Int& new_pos);
};


#endif