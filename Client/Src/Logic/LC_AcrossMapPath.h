#pragma once
//客户端跨地图信息管理
struct MapExitPointData
{
	int m_iDstMapID;
	int m_iTriggerTransferPointID;
};
typedef mem::vector<MapExitPointData> MapExitPointList;

struct MapExitInfo
{
	int m_iMapID;
	MapExitPointList m_vecMapExitPointList;
};

typedef mem::vector<MapExitInfo> MapExitInfoList;

class LC_AcrossMapPath :	public Memory::MM_BaseObject
{
public:
	LC_AcrossMapPath();
	~LC_AcrossMapPath();
	DECL_RTTI_NAME( LC_AcrossMapPath )

	bool Init();
	bool UnInit();
	void ReUse();

	bool SetAcrossMapPath(int iCurrentMapID ,int iNewMapID);
	void ClearAcrossMapPath();
	bool IsFindingAcrossMap();
	bool GetTransferPosLocation(int iCurrentMapID,Utility::UT_Vec3Int& rPos);
private:
	bool _findAcrossMapPath(int iCurrentMapID ,int iNewMapID);
	bool _findExitPoint(MapExitPointList& rList, int iMapID, int& iTransferPoint);
	bool _isHoldMap(int iMapID);
	void _unHoldMap(int iMapID);
	MapExitInfoList  m_MapExitInfoList;
	mem::vector<int> m_vecAcrossMapPath;
	mem::vector<int> m_vecHoldMapList;
	bool m_bFindingAcrossMapPath;
};
