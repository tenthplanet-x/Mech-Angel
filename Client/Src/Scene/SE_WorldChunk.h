#ifndef _SE_WORLD_CHUNK_H
#define _SE_WORLD_CHUNK_H

#include <NiTriStrips.h>
#include <NiTriShape.h>

#include "MTF_WorldChunk.h"
#include "MTF_WorldTile.h"


//used to seperate modelloading per cluster
typedef std::vector<int>   ModelIDVector;

class SE_WorldTile;
class SE_WorldChunk
{
public:
	SE_WorldChunk(int iIndexX,int iIndexY,SE_WorldTile* pkWorldTileParent);
	virtual ~SE_WorldChunk();

	void           SetChunkGeometryData(NiTriStripsDataPtr pkFreeGeometryData);

	bool           LoadChunkGeometryData(WorldChunkFile* pkChunkFile);

	bool           UnLoadChunkGeometryData();

	NiAVObjectPtr  GetNiContent();

	void           UpdateGeometryData();

	bool           GetTerrainLocalInfo(TerrainLocalInfo& rkLocalInfo,float fLocationX,float fLocationZ);

	bool           IsLoaded();
protected:

	bool     _setChunkGeometryData(Vector4  kBoundingSphere,
		float*   pfHeights = NULL,
		Vector3* pkNormals = NULL,
		Vector3* pkColors = NULL);


	bool    _getChunkGeometryData(float*& pfHeights,Vector3*& pkNormals,Vector3*& pkColors,Vector4& rkBoundingSphere);


	int          m_iChunkIndexX;
	int          m_iChunkIndexY;

	NiNodePtr      m_pkNiNode;
	NiAVObjectPtr  m_pkChunkGeometry;
	NiAVObjectPtr  m_pkChunkLiquad;
	WorldTile*     m_pkWorldTile;

	NiTriStripsDataPtr m_pkChunkGeometryData;
	NiTriStripsDataPtr m_pkChunkLiquadData;

	ChunkDetailLevel m_eCurrentDetailLevel;

	int              m_iChunkMaterialID[4];
	NiTexturePtr     m_akAlphaTextures[3];
	NiTexturePtr     m_pkShadowTexture;

	bool           m_bLoadGeometryData;
	bool           m_bLoadMaterial;
	bool           m_bLoadModels;
	bool           m_bLoadLiquad;

	EntityVector       m_kModelVector;

	static  NiSourceTexturePtr     ms_akLiquadTextures[30];
	int                m_iCurrentLiquadTexture;

public:
	ModelIDVector      m_kModelIDVector; 
};

#endif