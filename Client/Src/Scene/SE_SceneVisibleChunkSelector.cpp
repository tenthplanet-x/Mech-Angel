#include "ClientPCH.h"

#include "SE_SceneWorldMap.h"
#include "SE_SceneWorldTile.h"
#include "SE_SceneWorldChunkCluster.h"
#include "SE_SceneWorldChunk.h"
#include "SE_SceneVisibleChunkSelector.h"

#include "CA_CameraManager.h"
#include "UT_RenderHelper.h"

#include "UT_ClientGlobalSettings.h"

using namespace Utility;
using namespace Terrain;
//---------------------------------------------------------------
SE_SceneVisibleChunkSelector::SE_SceneVisibleChunkSelector(SE_SceneWorldMap* pkSelectMap,
														   float fSelectionRadius)
: m_pkSelectMap(pkSelectMap)
, m_fSelectionRadius(fSelectionRadius)
{
}
//---------------------------------------------------------------
SE_SceneVisibleChunkSelector::SE_SceneVisibleChunkSelector()
: m_pkSelectMap(NULL)
, m_fSelectionRadius(0.0f)
{
}
//---------------------------------------------------------------
SE_SceneVisibleChunkSelector::~SE_SceneVisibleChunkSelector()
{
	m_pkSelectMap = NULL;
	m_fSelectionRadius = 0.0f;
}
//---------------------------------------------------------------
void SE_SceneVisibleChunkSelector::SelectVisibleChunks(NiPoint3 kEyeLocation)
{
	T_ASSERT(m_pkSelectMap && "NULL World Map");
	if(!m_pkSelectMap)
		return;
	/*ProfileStopWatch kWatch;
	kWatch.Start();*/

	SE_ChunkIndexPool kResult;

	int nChunkWorldIndexX = Utility::Float2Int( kEyeLocation.x / GfxGetWorldConfig().CFG_WORLD_CHUNK_WIDTH );
	int nChunkWorldIndexY = Utility::Float2Int( kEyeLocation.y / GfxGetWorldConfig().CFG_WORLD_CHUNK_WIDTH );

	int nMaxChunkStep = g_clientsettings.GetLod_ScenePrefetchWidth();

	int nMinXE = nChunkWorldIndexX - nMaxChunkStep - 1;
	int nMinYE = nChunkWorldIndexY - nMaxChunkStep - 1;
	int nMaxXE = nChunkWorldIndexX + nMaxChunkStep + 2;
	int nMaxYE = nChunkWorldIndexY + nMaxChunkStep + 2;

	int iTileX ,iTileY;
	for ( int nX = nMinXE ; nX < nMaxXE ; nX++ )
	{
		for ( int nY = nMinYE ; nY < nMaxYE ; nY++ )
		{
			NiPoint3 kPos = NiPoint3::ZERO;
			kPos.x = nX * GfxGetWorldConfig().CFG_WORLD_CHUNK_WIDTH + 0.5f * GfxGetWorldConfig().CFG_WORLD_CHUNK_WIDTH;
			kPos.y = nY * GfxGetWorldConfig().CFG_WORLD_CHUNK_WIDTH + 0.5f * GfxGetWorldConfig().CFG_WORLD_CHUNK_WIDTH;

			m_pkSelectMap->GetWhichTile( iTileX , iTileY , kPos.x , kPos.y );

			SE_SceneWorldTile* pkTile = m_pkSelectMap->GetWorldTile(iTileX,iTileY);

			if( pkTile )
			{
				SE_ChunkIndex kVisibleChunk;
				kVisibleChunk._tileIndexX = iTileX;
				kVisibleChunk._tileIndexY = iTileY;
				kVisibleChunk._clusterIndexX = 0;
				kVisibleChunk._clusterIndexY = 0;
				kVisibleChunk._chunkIndexX = 0;
				kVisibleChunk._chunkIndexY = 0;

				bool bResult = pkTile->GetChunkByCenter( kPos ,
					kVisibleChunk._clusterIndexX , kVisibleChunk._clusterIndexY,
					kVisibleChunk._chunkIndexX , kVisibleChunk._chunkIndexY);
				int iChunkID = GfxGetWorldConfig().ComputeChunkID(
					iTileX,
					iTileY,
					kVisibleChunk._clusterIndexX,
					kVisibleChunk._clusterIndexY,
					kVisibleChunk._chunkIndexX,
					kVisibleChunk._chunkIndexY );

				if ( bResult  )
				{
					kResult.insert( std::make_pair( iChunkID , kVisibleChunk ) );
				}
			}
		}
	}
	m_pkSelectMap->SetVisibleChunks(kResult);
	/*GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING ,  
		"Visible chunk is %d,Use %f Time to compute" , kResult.size() , kWatch.End() );*/
	if ( g_settings.CFG_ENABLE_BACKLOADING_LOG )
	{
		StringStreamType output;
		SE_ChunkIndexPoolIter iter;
		TRAV( kResult , iter )
		{
			output<<iter->first<<",";
		}
		GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING,
			"Visible chunk is %s" , output.str().c_str() );

	}
}

void SE_SceneVisibleChunkSelector::SelectVisibleChunks( const NiPoint2& rkPos , NiCamera* pkCamera )
{
	//根据位置和相机的视椎体进行需要加载的Chunk的判定
	T_ASSERT(m_pkSelectMap && "NULL World Map");
	if(!m_pkSelectMap)
		return;
	ProfileStopWatch kWatch;
	kWatch.Start();

	SE_ChunkIndexPool kResult;

	// 首先把周围5×5的chunk加入到可见队列中
	int nChunkWorldIndexX = Utility::Float2Int( rkPos.x / GfxGetWorldConfig().CFG_WORLD_CHUNK_WIDTH );
	int nChunkWorldIndexY = Utility::Float2Int( rkPos.y / GfxGetWorldConfig().CFG_WORLD_CHUNK_WIDTH );

	int nMaxChunkStep = g_clientsettings.GetLod_ScenePrefetchWidth();

	int nMinXE = nChunkWorldIndexX - nMaxChunkStep - 1;		// 左边界，方括号
	int nMinYE = nChunkWorldIndexY - nMaxChunkStep - 1;
	int nMaxXE = nChunkWorldIndexX + nMaxChunkStep + 2;		// 右边界，圆括号，这个不算，所以是5x5，不是6x6
	int nMaxYE = nChunkWorldIndexY + nMaxChunkStep + 2;

	int iTileX ,iTileY;
	for ( int nX = nMinXE ; nX < nMaxXE ; nX++ )
	{
		for ( int nY = nMinYE ; nY < nMaxYE ; nY++ )
		{
			NiPoint3 kPos = NiPoint3::ZERO;		// Chunk中心点
			kPos.x = nX * GfxGetWorldConfig().CFG_WORLD_CHUNK_WIDTH + 0.5f * GfxGetWorldConfig().CFG_WORLD_CHUNK_WIDTH;
			kPos.y = nY * GfxGetWorldConfig().CFG_WORLD_CHUNK_WIDTH + 0.5f * GfxGetWorldConfig().CFG_WORLD_CHUNK_WIDTH;

			m_pkSelectMap->GetWhichTile( iTileX , iTileY , kPos.x , kPos.y );	// Get Tile Index

			SE_SceneWorldTile* pkTile = m_pkSelectMap->GetWorldTile(iTileX,iTileY);

			if( pkTile )
			{
				SE_ChunkIndex kVisibleChunk;
				kVisibleChunk._tileIndexX = iTileX;
				kVisibleChunk._tileIndexY = iTileY;
				kVisibleChunk._clusterIndexX = 0;
				kVisibleChunk._clusterIndexY = 0;
				kVisibleChunk._chunkIndexX = 0;
				kVisibleChunk._chunkIndexY = 0;

				bool bResult = pkTile->GetChunkByCenter( kPos ,
					kVisibleChunk._clusterIndexX , kVisibleChunk._clusterIndexY,
					kVisibleChunk._chunkIndexX , kVisibleChunk._chunkIndexY);
				int iChunkID = GfxGetWorldConfig().ComputeChunkID(
					iTileX,
					iTileY,
					kVisibleChunk._clusterIndexX,
					kVisibleChunk._clusterIndexY,
					kVisibleChunk._chunkIndexX,
					kVisibleChunk._chunkIndexY );

				if ( bResult  )
				{
					kResult.insert( std::make_pair( iChunkID , kVisibleChunk ) );
				}
			}
		}
	}

	// 再把与Camera Frustum相交的Chunk加入可见队列中
	if ( pkCamera && g_clientsettings.GetLod_SceneAddCameraCull() )
	{
		NiFrustum kFrus = pkCamera->GetViewFrustum();

		int iTileCountXPerMap = GfxGetWorldConfig().CFG_WORLD_TILE_COUNTX_PER_MAP;
		int iTileCountYPerMap = GfxGetWorldConfig().CFG_WORLD_TILE_COUNTX_PER_MAP;
		int iChunkClusterCountXPerTile = GfxGetWorldConfig().CFG_WORLD_CHUNKCLUSTER_COUNTX_PER_TILE;
		int iChunkClusterCountYPerTile = GfxGetWorldConfig().CFG_WORLD_CHUNKCLUSTER_COUNTX_PER_TILE;
		int iChunkCountXPerCluster = GfxGetWorldConfig().CFG_WORLD_CHUNK_COUNTX_PER_CLUSTER;
		int iChunkCountYPerCluster = GfxGetWorldConfig().CFG_WORLD_CHUNK_COUNTX_PER_CLUSTER;

		// 逐层判断是否与Camera视锥有相交，Tile -> ChunkCluster -> Chunk
		// Tile
		for(int iTileIndexX = 0; iTileIndexX < iTileCountXPerMap; iTileIndexX++)
		{
			for(int iTileIndexY = 0; iTileIndexY < iTileCountYPerMap; iTileIndexY++)
			{
				SE_SceneWorldTile* pkWorldTile = m_pkSelectMap->GetWorldTile(iTileIndexX,iTileIndexY);
				if ( pkWorldTile )
				{
					Terrain::TS_BoundingVolume& rkBound = pkWorldTile->GetBoundingVolume();
					//如果地图导出有问题，就跳过加载，但是输出警告
					if ( rkBound.GetCenter() == NiPoint3::ZERO )
					{
						GfxWriteLog( LOG_TAG_ERROR , LOG_SWITCH_ERROR ,
							"地图没有进行完整导出，导致某些块的包围盒没有正确输出！");
						continue;
					}
					if(!rkBound.Frustum_intersect( kFrus , pkCamera->GetWorldTransform() ))
						continue;

					// ChunkCluster
					for(int iChunkClusterIndexX = 0; iChunkClusterIndexX < iChunkClusterCountXPerTile; iChunkClusterIndexX++)
					{
						for(int iChunkClusterIndexY = 0; iChunkClusterIndexY < iChunkClusterCountYPerTile; iChunkClusterIndexY++)
						{
							SE_SceneWorldChunkCluster* pkWorldChunkCluster = pkWorldTile->GetWorldChunkCluster(iChunkClusterIndexX,iChunkClusterIndexY);
							Terrain::TS_BoundingVolume& rkBound = pkWorldChunkCluster->GetBoundingVolume();
							if(!rkBound.Frustum_intersect( kFrus , pkCamera->GetWorldTransform() ))
								continue;

							// Chunk
							for(int iChunkIndexX = 0;iChunkIndexX < iChunkCountXPerCluster; iChunkIndexX++)
							{
								for(int iChunkIndexY = 0; iChunkIndexY < iChunkCountYPerCluster; iChunkIndexY++)
								{
									SE_SceneWorldChunk* pkWorldChunk = pkWorldChunkCluster->GetWorldChunk(iChunkIndexX,iChunkIndexY);
									Terrain::TS_BoundingVolume& rkBound = pkWorldChunk->GetBoundingVolume();
									if(!rkBound.Frustum_intersect( kFrus , pkCamera->GetWorldTransform() ))
										continue;

									SE_ChunkIndex kVisibleChunk;
									kVisibleChunk._tileIndexX = iTileIndexX;
									kVisibleChunk._tileIndexY = iTileIndexY;
									kVisibleChunk._clusterIndexX = iChunkClusterIndexX;
									kVisibleChunk._clusterIndexY = iChunkClusterIndexY;
									kVisibleChunk._chunkIndexX = iChunkIndexX;
									kVisibleChunk._chunkIndexY = iChunkIndexY;

									int iChunkID = GfxGetWorldConfig().ComputeChunkID(iTileIndexX,
										iTileIndexY,
										iChunkClusterIndexX,
										iChunkClusterIndexY,
										iChunkIndexX,
										iChunkIndexY);
									kResult.insert( std::make_pair( iChunkID , kVisibleChunk ) );	// 与之前5x5插入的结果应该会有重复
								}
							}
						}
					}
				}
			}
		}
	}
	m_pkSelectMap->SetVisibleChunks(kResult);
	GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING ,  
		"Visible chunk is %d,Use %f Time to compute" , kResult.size() , kWatch.End() );
}

void SE_SceneVisibleChunkSelector::SelectVisibleChunks( const NiPoint3& kEyeLocation , bool bBB /*= false */ )
{
	T_ASSERT(m_pkSelectMap && "NULL World Map");
	if(!m_pkSelectMap)
		return;
	ProfileStopWatch kWatch;
	kWatch.Start();

	SE_ChunkIndexPool kResult;

	TS_BoundingVolume kAABB;
	NiPoint3 kMax , kMin;
	kMin.x = (kEyeLocation.x - m_fSelectionRadius);
	kMax.x = (kEyeLocation.x + m_fSelectionRadius);
	kMin.y = (kEyeLocation.y - m_fSelectionRadius);
	kMax.y = (kEyeLocation.y + m_fSelectionRadius);
	kMin.z = -500;
	kMax.z = 1500;
	kAABB.SetMax( kMax );
	kAABB.SetMin( kMin );
	kAABB.Update();
	kAABB.UpdateCorner();

	int iTileCountXPerMap = GfxGetWorldConfig().CFG_WORLD_TILE_COUNTX_PER_MAP;
	int iTileCountYPerMap = GfxGetWorldConfig().CFG_WORLD_TILE_COUNTX_PER_MAP;
	int iChunkClusterCountXPerTile = GfxGetWorldConfig().CFG_WORLD_CHUNKCLUSTER_COUNTX_PER_TILE;
	int iChunkClusterCountYPerTile = GfxGetWorldConfig().CFG_WORLD_CHUNKCLUSTER_COUNTX_PER_TILE;
	int iChunkCountXPerCluster = GfxGetWorldConfig().CFG_WORLD_CHUNK_COUNTX_PER_CLUSTER;
	int iChunkCountYPerCluster = GfxGetWorldConfig().CFG_WORLD_CHUNK_COUNTX_PER_CLUSTER;


	for(int iTileIndexX = 0; iTileIndexX < iTileCountXPerMap; iTileIndexX++)
	{
		for(int iTileIndexY = 0; iTileIndexY < iTileCountYPerMap; iTileIndexY++)
		{
			SE_SceneWorldTile* pkWorldTile = m_pkSelectMap->GetWorldTile(iTileIndexX,iTileIndexY);
			Terrain::TS_BoundingVolume& rkBound = pkWorldTile->GetBoundingVolume();
			if(!rkBound.AABB_intersect( kAABB ))
				continue;

			for(int iChunkClusterIndexX = 0; iChunkClusterIndexX < iChunkClusterCountXPerTile; iChunkClusterIndexX++)
			{
				for(int iChunkClusterIndexY = 0; iChunkClusterIndexY < iChunkClusterCountYPerTile; iChunkClusterIndexY++)
				{
					SE_SceneWorldChunkCluster* pkWorldChunkCluster = pkWorldTile->GetWorldChunkCluster(iChunkClusterIndexX,iChunkClusterIndexY);
					Terrain::TS_BoundingVolume& rkBound = pkWorldChunkCluster->GetBoundingVolume();
					if(!rkBound.AABB_intersect( kAABB ))
						continue;
					for(int iChunkIndexX = 0;iChunkIndexX < iChunkCountXPerCluster; iChunkIndexX++)
					{
						for(int iChunkIndexY = 0; iChunkIndexY < iChunkCountYPerCluster; iChunkIndexY++)
						{
							SE_SceneWorldChunk* pkWorldChunk = pkWorldChunkCluster->GetWorldChunk(iChunkIndexX,iChunkIndexY);
							Terrain::TS_BoundingVolume& rkBound = pkWorldChunk->GetBoundingVolume();
							if(!rkBound.AABB_intersect( kAABB ))
								continue;

							SE_ChunkIndex kVisibleChunk;
							kVisibleChunk._tileIndexX = iTileIndexX;
							kVisibleChunk._tileIndexY = iTileIndexY;
							kVisibleChunk._clusterIndexX = iChunkClusterIndexX;
							kVisibleChunk._clusterIndexY = iChunkClusterIndexY;
							kVisibleChunk._chunkIndexX = iChunkIndexX;
							kVisibleChunk._chunkIndexY = iChunkIndexY;

							int iChunkID = GfxGetWorldConfig().ComputeChunkID(iTileIndexX,
								iTileIndexY,
								iChunkClusterIndexX,
								iChunkClusterIndexY,
								iChunkIndexX,
								iChunkIndexY);
							kResult.insert( std::make_pair( iChunkID , kVisibleChunk ) );
						}
					}
				}
			}
		}
	}
	m_pkSelectMap->SetVisibleChunks(kResult);
	GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING ,  
		"Visible chunk is %d,Use %f Time to compute" , kResult.size() , kWatch.End() );
}
void SE_SceneVisibleChunkSelector::SetSelectionRadius( float fR )
{
	if ( fR < 1.0f )
	{
		return;
	}
	GfxWriteLog( LOG_BACKLOADING_FILELINE , Utility::LOG_SWITCH_BACKLOADING , "改变后台加载的半径为%f" , fR );
	m_fSelectionRadius = fR;
}