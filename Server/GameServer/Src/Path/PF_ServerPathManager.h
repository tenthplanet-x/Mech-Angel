#pragma once
#include "UT_PathGrid.h"
#include "UT_LinePath.h"
#include "UT_Singleton.h"

class PF_ServerPathManager : public Memory::MM_BaseObject,public Utility::UT_Singleton<PF_ServerPathManager>
{
public:

	PF_ServerPathManager();
	virtual ~PF_ServerPathManager();

	bool                    Load(  );

	bool                    FindPath( 
		int nMapID , 
		Utility::UT_LinePath& rkPath , 
		const Utility::UT_Vec3Int& rkStart , 
		const Utility::UT_Vec3Int& rkEnd
		);
	bool					FindDirectFarawayPoint(int nMapID ,const Utility::UT_Vec3Int& srcpoint,const Utility::UT_Vec3Int& destpoint,int length,Utility::UT_Vec3Int& endpoint);
	bool					FindDirectFarawayPointByDis(int nMapID ,const Utility::UT_Vec3Int& srcpoint,const Utility::UT_Vec3Int& destpoint,int length,Utility::UT_Vec3Int& endpoint);
	bool                    IsPosValid( int nMapID , const Utility::UT_Vec3Int& rkP0 );
	bool					GetAllVaildCell( int nMapID , const Utility::UT_Vec3Int& center, int radius,mem::vector<Utility::UT_Vec2Int>& result);
	bool					RandomVaildPoint( int nMapID , Utility::UT_Vec3Int& center);
	bool					GetCellPoint( int nMapID ,Utility::UT_Vec2Int& cell, Utility::UT_Vec3Int& point);
	long					GetMapSize(int nMapID);
	bool					CheckCellVaild(int nMapID,int x,int y);
protected:
	Utility::PathGridMap m_kPathGrids;
};