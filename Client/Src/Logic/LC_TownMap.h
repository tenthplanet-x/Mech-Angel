#pragma once
//城镇提供的只是坐标转换，城镇还是小地图的一部分
#include "LC_ObjectDescription.h"

class LC_TownMap : public Memory::MM_BaseObject
{
public:
	LC_TownMap();
	~LC_TownMap();

	DECL_RTTI_NAME( LC_TownMap )

	long	EnterTown(long lTownID);

	float	TranToTownPosX(float fMiniMap);
	float	TranToTownPosY(float fMiniMap);
	float   TranToMiniMapPosX(float fTown);
	float   TranToMiniMapPosY(float fTown);

	StringType GetTownName();
	long       GetTownID();

	int	TestIsTown(const StringType& strAreaName);

public:
	long	GetMapTownNum(long lMapID);
	LC_TownIconDescription* GetTownIconInfo(long lMapID, long lIndex);

private:
	bool	_isInTownMap(float fX, float fY);

protected:
	float	m_fLeft;		// 城镇位置
	float	m_fTop;			//
	float	m_fWidth;
	float	m_fHeight;

	long	m_lTownID;				

	LC_TownIconDescription m_kTownIconDesc; // 临时变量，lua访问
};