#ifndef _SE_SCENE_TERRAIN_INFO_H
#define _SE_SCENE_TERRAIN_INFO_H

class  SE_SceneTerrainLocalInfo
{
public:
	SE_SceneTerrainLocalInfo() 
		: _bGetNormal(false)
		, _bGetHeight(false)
		, _fLocalHeight(0.0f)
	{};

	bool     _bGetNormal;
	bool     _bGetHeight;
	float    _fLocalHeight;
	NiPoint3 _kLocalNormal;
};

#endif