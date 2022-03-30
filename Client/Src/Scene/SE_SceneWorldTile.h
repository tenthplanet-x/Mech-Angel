#ifndef _SE_SCENE_WORLD_TILE_H
#define _SE_SCENE_WORLD_TILE_H


#include "MTF_ClientTile.h"
#include "MG_MessageHandlerInterface.h"

class SE_SceneWorldMap;
class SE_SceneWorldChunk;
class SE_SceneWorldChunkCluster;
class SE_SceneTerrainLocalInfo;
namespace FileSystem { class ClientMapGlobalInfoManager; }

class SE_SceneWorldTile : public MG_MessageHandlerInterface,
						  public Memory::MM_BaseObject
{
public:
	SE_SceneWorldTile(int iIndexX,int iIndexY,SE_SceneWorldMap* pkWorldMapParent);
	virtual ~SE_SceneWorldTile();


	virtual bool RecvMessage(MG_MessageBase& rkMessage);

	SE_SceneWorldChunkCluster*   GetWorldChunkCluster(int iChunkClusterIndexX, int iChunkClusterIndexY);
	int   GetTileIndexX(){return m_iTileIndexX;};
	int   GetTileIndexY(){return m_iTileIndexY;};

	bool        LoadTileFile();  //根据标示自己计算文件名
	bool		UnLoadTileFile();
	bool        CanSafeUnLoadTileFile();

	//点光信息要在这里CheckIn
	void        PostLoad();
	void        PreUnLoad();

	void                        RegisterLocalNPC( long lSpawnID , const NiPoint3& rkPos );
	Utility::UT_AABB&		    GetAreaRect();
	Terrain::TS_BoundingVolume& GetBoundingVolume(){ return m_kWorldBound; }
	SE_SceneWorldChunk*         GetChunkByCenter( const NiPoint3& kChunkCenter );
	bool                        GetChunkByCenter( const NiPoint3& kChunkCenter , int& iClusterX , int& iClusterY , int& iChunkX , int& iChunkY );

	FileSystem::MTF_ClientTile* GetWorldTileFile();

protected:
	SE_SceneWorldTile();
	
	void		 _generateTileFileName();
	void		 _getWhichChunkCluster(int& iClusterIndexX, int& iClusterIndexY, float fPointX, float fPointY);
	void		 _computeAreaRect();

	int			m_iTileIndexX;
	int			m_iTileIndexY;
	int         m_iChunkClusterCountX;
	int         m_iChunkClusterCountY;
	
	typedef mem::vector<SE_SceneWorldChunkCluster*>		SE_SceneWorldChunkClusterVector;
	typedef SE_SceneWorldChunkClusterVector::iterator	SE_SceneWorldChunkClusterVectorIter;

	SE_SceneWorldChunkClusterVector						m_kWorldChunkClusterVector;

	SE_SceneWorldMap*		m_pkSceneWorldMapParent;
	Utility::UT_AABB		m_kAreaRect;

	bool					        m_bFileLoaded;
	StringType				        m_kTileFileName;
	StringType                      m_kDecorationTileFileName;
	
	Terrain::TS_BoundingVolume      m_kWorldBound;
	
	FileSystem::MTF_ClientTile*				m_pkWorldTileFile;
	FileSystem::MTF_ClientTile::TilePointLightInfoMap	m_kTilePLs;
	FileSystem::ClientMapGlobalInfoManager* m_pkMapGlobalInfo;

    bool m_bPostLoaded;
};

#endif