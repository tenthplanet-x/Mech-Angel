#ifndef _SE_SCENE_WORLD_CHUNK_CLUSTER_H
#define _SE_SCENE_WORLD_CHUNK_CLUSTER_H

#include "MG_MessageHandlerInterface.h"
#include "TS_BoundingVolume.h"

class SE_SceneWorldMap;
class SE_SceneWorldTile;
class SE_SceneWorldChunk;
class SE_SceneTerrainLocalInfo;

class SE_SceneWorldChunkCluster : public MG_MessageHandlerInterface,
								  public Memory::MM_BaseObject
{
public:
	SE_SceneWorldChunkCluster(int iChunkClusterIndexX,
							  int iChunkClusterIndexY,
							  SE_SceneWorldMap*  pkWorldMapParent,
							  SE_SceneWorldTile* pkWorldTileParent);


	virtual ~SE_SceneWorldChunkCluster();


	virtual bool        RecvMessage(MG_MessageBase& rkMessage);

	SE_SceneWorldChunk* GetWorldChunk(int iChunkIndexX,int iChunkIndexY)
    {
        if (iChunkIndexX < 0 || iChunkIndexX >= WORLD_CHUNK_COUNTX_PER_CLUSTER
            || iChunkIndexY < 0 || iChunkIndexY >= WORLD_CHUNK_COUNTX_PER_CLUSTER)
        {
            return NULL;
        }
        return m_kWorldChunkVector[iChunkIndexY * m_iChunkCountX + iChunkIndexX];
    }
	int GetChunkClusterIndexX() { return m_iChunkClusterIndexX; }
	int GetChunkClusterIndexY() { return m_iChunkClusterIndexY; }
	SE_SceneWorldChunk* GetChunkByCenter( const NiPoint3& kChunkCenter );

	void RegisterLocalNPC( long lSpawnPointID , const NiPoint3& kPos );

	bool LoadChunk(int iChunkIndexX,int iChunkIndexY);
	void PostLoadChunk(int iChunkIndexX,int iChunkIndexY);
	void PreUnLoadChunk(int iChunkIndexX,int iChunkIndexY);
	bool UnLoadChunk(int iChunkIndexX,int iChunkIndexY);
	bool IsLoaded();
	void SetLoadingFlag(  int nX ,int nY,bool bLoading );
	bool IsLoading();

	NiNodePtr			 GetNiSceneRootNode();
	Utility::UT_AABB&    GetAreaRect();
	Terrain::TS_BoundingVolume& GetBoundingVolume(){ return m_kWorldBound; }
	int			                GetTerrainVisibleArray(NiCameraPtr spNiCamera,NiCullingProcess& rkCullingProcess, NiVisibleArray& rkVisibleArray);

	void		                GetWhichChunk(int& iChunkIndexX, int& iChunkIndexY,float fPointX, float fPointY);
protected:
	void		_initChunks();
	void		_uninitChunks();
	
	void		_computeAreaRect();

	Utility::UT_AABB		m_kAreaRect;

	typedef mem::vector<SE_SceneWorldChunk*>		SE_SceneWorldChunkVector;
	typedef SE_SceneWorldChunkVector::iterator		SE_SceneWorldChunkVectorIter;

    const int WORLD_CHUNK_COUNTX_PER_CLUSTER;

	SE_SceneWorldChunkVector						m_kWorldChunkVector;

	SE_SceneWorldMap*  m_pkWorldMapParent;
	SE_SceneWorldTile* m_pkWorldTileParent;

	int				   m_iChunkClusterIndexX;
	int				   m_iChunkClusterIndexY;
	int				   m_iChunkCountX;
	int				   m_iChunkCountY;

	NiNodePtr          m_spNiSceneRootNode;
	NiNodePtr		   m_spNiTerrainRootNode;
	NiNodePtr		   m_spNiModelRootNode;
	NiNodePtr		   m_spNiWaterRootNode;
	NiNodePtr          m_spNiPathModelRootNode;

	Terrain::TS_BoundingVolume      m_kWorldBound;
};

#endif