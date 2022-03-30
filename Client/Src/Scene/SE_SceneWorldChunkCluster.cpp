#include "ClientPCH.h"
#include "SE_SceneWorldChunkCluster.h"
#include "SE_SceneWorldMap.h"
#include "SE_SceneWorldTile.h"
#include "SE_SceneWorldChunk.h"
#include "SE_SceneManager.h"
#include "SE_SceneTerrainInfo.h"
#include "MG_MessageBase.h"
#include "RG_RenderManager.h"
#include "MTF_WorldTile.h"
using namespace Utility;
using namespace FileSystem;

//----------------------------------------------------
SE_SceneWorldChunkCluster::SE_SceneWorldChunkCluster(int iChunkClusterIndexX, 
													 int iChunkClusterIndexY, 
													 SE_SceneWorldMap* pkWorldMapParent, 
													 SE_SceneWorldTile* pkWorldTileParent)
: WORLD_CHUNK_COUNTX_PER_CLUSTER(GfxGetWorldConfig().CFG_WORLD_CHUNK_COUNTX_PER_CLUSTER)

, m_pkWorldMapParent(pkWorldMapParent)
, m_pkWorldTileParent(pkWorldTileParent)
, m_iChunkClusterIndexX(iChunkClusterIndexX)
, m_iChunkClusterIndexY(iChunkClusterIndexY)
{
	_computeAreaRect();

	//child0 : 地形 child1:模型
	m_spNiSceneRootNode = NiNew NiNode(3);

	m_spNiTerrainRootNode = NiNew NiNode;
	m_spNiModelRootNode = NiNew NiNode;
	m_spNiWaterRootNode = NiNew NiNode;
	m_spNiPathModelRootNode = NiNew NiNode;
	
	m_spNiSceneRootNode->SetAt(0,m_spNiTerrainRootNode);
	m_spNiSceneRootNode->SetAt(1,m_spNiModelRootNode);
	m_spNiSceneRootNode->SetAt(2,m_spNiWaterRootNode);
	//Pick节点，设置为隐藏
	m_spNiPathModelRootNode->SetAppCulled( true );
	m_spNiSceneRootNode->SetAt(3,m_spNiPathModelRootNode);

	m_spNiSceneRootNode->SetTranslate((float)m_kAreaRect.iMinX,(float)m_kAreaRect.iMinY,0.0f);
	m_spNiSceneRootNode->Update(0.0f);

	m_iChunkCountX = WORLD_CHUNK_COUNTX_PER_CLUSTER;
	m_iChunkCountY = WORLD_CHUNK_COUNTX_PER_CLUSTER;

	_initChunks();
}
//----------------------------------------------------
SE_SceneWorldChunkCluster::~SE_SceneWorldChunkCluster()
{
	_uninitChunks();

	m_pkWorldTileParent = NULL;
	m_pkWorldMapParent = NULL;
	m_spNiTerrainRootNode = NULL;
	m_spNiModelRootNode = NULL;
	m_spNiSceneRootNode = NULL;
	m_spNiPathModelRootNode = NULL;
}
//----------------------------------------------------
bool SE_SceneWorldChunkCluster::RecvMessage( MG_MessageBase& rkMessage )
{
	MG_MessageHandlerInterface::RecvMessage(rkMessage);
	return true;
}
//----------------------------------------------------
bool SE_SceneWorldChunkCluster::LoadChunk( int iChunkIndexX,int iChunkIndexY )
{
	SE_SceneWorldChunk* pkWorldChunk = GetWorldChunk(iChunkIndexX,iChunkIndexY);
	T_ASSERT(pkWorldChunk && "NULL Chunk");
	if(!pkWorldChunk)
		return false;

	MTF_ClientTile* pkTileFile = m_pkWorldTileParent->GetWorldTileFile();
	if( pkTileFile == NULL )
		return false;

	//对Tile的访问必须在后台线程
	//T_ASSERT( !GfxIsMainThread() );
	bool bLoad = pkTileFile->LoadChunk( pkWorldChunk->GetChunkTileIndexX() , 
		pkWorldChunk->GetChunkTileIndexY() );
	if ( bLoad )
	{
		pkWorldChunk->LoadTerrain();
		pkWorldChunk->LoadModels();
		pkWorldChunk->LoadWater();
		pkWorldChunk->LoadGrass();
	}
	return true;
}
//----------------------------------------------------
bool SE_SceneWorldChunkCluster::UnLoadChunk(int iChunkIndexX,int iChunkIndexY)
{
	SE_SceneWorldChunk* pkWorldChunk = GetWorldChunk(iChunkIndexX,iChunkIndexY);
	T_ASSERT(pkWorldChunk && "NULL Chunk");
	if(!pkWorldChunk)
		return false;

    pkWorldChunk->UnLoadTerrain();
    pkWorldChunk->UnLoadGrass();
    pkWorldChunk->UnLoadWater();

	MTF_ClientTile* pkTileFile = m_pkWorldTileParent->GetWorldTileFile();
	if ( !pkTileFile )
	{
		GfxWriteLog( LOG_BACKLOADING_FILELINE , LOG_SWITCH_BACKLOADING ,
			"tile file is released!!!" );
	}
	else
	{
		pkTileFile->UnLoadChunk( pkWorldChunk->GetChunkTileIndexX() , 
		pkWorldChunk->GetChunkTileIndexY() ) ;
	}

	return true;
}
void SE_SceneWorldChunkCluster::PostLoadChunk( int iChunkIndexX,int iChunkIndexY )
{
	//Link SceneGraph
	//由于重构了Cull的算法
	//所以在每一个Chunk内部都有一个根节点，在完成加载的PostLoad中完成挂接操作
	//就不需要在这里进行了
	SE_SceneWorldChunk* pkWorldChunk = GetWorldChunk(iChunkIndexX,iChunkIndexY);
	T_ASSERT(pkWorldChunk);

	//先挂接地形再PostLoad，以便正确更新WorldBound
	m_spNiTerrainRootNode->AttachChild(pkWorldChunk->GetNiChunkTerrainRootNode());
	m_spNiTerrainRootNode->Update(0.0f);

	pkWorldChunk->PostLoadTerrain();
	pkWorldChunk->PostLoadWater();
	pkWorldChunk->PostLoadGrass();
	//发送模型加载的异步请求
	pkWorldChunk->RequestModels();

	T_ASSERT( pkWorldChunk->IsLoaded() );

	pkWorldChunk->SetActive( true );

	m_pkWorldMapParent->AddToLoadedArray( pkWorldChunk );
}

void SE_SceneWorldChunkCluster::PreUnLoadChunk( int iChunkIndexX,int iChunkIndexY )
{
	//在主线程中执行
	SE_SceneWorldChunk* pkWorldChunk = GetWorldChunk(iChunkIndexX,iChunkIndexY);
	T_ASSERT(pkWorldChunk && pkWorldChunk->IsLoaded() );

	pkWorldChunk->PreUnLoadTerrain();
	m_spNiTerrainRootNode->DetachChild(pkWorldChunk->GetNiChunkTerrainRootNode());
	pkWorldChunk->PreUnLoadWater();
	pkWorldChunk->PreUnLoadGrass();
	pkWorldChunk->PreUnLoadModels();

	pkWorldChunk->SetActive( false );
	m_pkWorldMapParent->RemoveFromLoadedArray( pkWorldChunk );
}
//----------------------------------------------------
bool SE_SceneWorldChunkCluster::IsLoaded()
{
    for (SE_SceneWorldChunkVector::iterator itr = m_kWorldChunkVector.begin(); itr != m_kWorldChunkVector.end(); ++itr)
    {
        SE_SceneWorldChunk* pkChunk = (*itr);
        if (pkChunk->IsTerrainLoaded() || pkChunk->IsModelsLoaded() || pkChunk->IsGrassLoaded() ||pkChunk->IsWaterLoaded())
        {
            return true;
        }
    }
    return false;
}

//----------------------------------------------------
NiNodePtr SE_SceneWorldChunkCluster::GetNiSceneRootNode()
{
	return m_spNiSceneRootNode;
}
//----------------------------------------------------
Utility::UT_AABB& SE_SceneWorldChunkCluster::GetAreaRect()
{
	return m_kAreaRect;
}
SE_SceneWorldChunk* SE_SceneWorldChunkCluster::GetChunkByCenter( const NiPoint3& kChunkCenter )
{
	T_ASSERT( GfxIsMainThread() );
	if(!IsLoaded())
		return NULL;
	int iChunkIndeX,iChunkIndeY;
	GetWhichChunk( iChunkIndeX , iChunkIndeY , kChunkCenter.x , kChunkCenter.y );

	SE_SceneWorldChunk* pkChunk = GetWorldChunk( iChunkIndeX , iChunkIndeY );
	if ( !pkChunk->IsLoaded() )
	{
		return NULL;
	}
	return pkChunk;
}
void SE_SceneWorldChunkCluster::RegisterLocalNPC( long lSpawnPointID , const NiPoint3& kPos )
{
	int iChunkIndeX,iChunkIndeY;
	GetWhichChunk( iChunkIndeX , iChunkIndeY , kPos.x , kPos.y );
	SE_SceneWorldChunk* pkChunk = GetWorldChunk( iChunkIndeX , iChunkIndeY );
	if ( pkChunk )
	{
		pkChunk->RegisterLocalNPC( lSpawnPointID );
	}
	else
	{
		GfxWriteLog( LOG_TAG_ERROR , LOG_SWITCH_ERROR ,
			"本地NPC坐标有误！[%d]:[%f %f %f]" , 
			lSpawnPointID , kPos.x , kPos.y  , kPos.z );
	}
}
//----------------------------------------------------
int SE_SceneWorldChunkCluster::GetTerrainVisibleArray( NiCameraPtr spNiCamera,NiCullingProcess& rkCullingProcess, NiVisibleArray& rkVisibleArray )
{
	int iVisibleCount = NiCullScene(spNiCamera, m_spNiTerrainRootNode, rkCullingProcess, rkVisibleArray,false);
	return iVisibleCount;
}
void SE_SceneWorldChunkCluster::_initChunks()
{
	m_kWorldChunkVector.clear();
	
	int iChunkCount = m_iChunkCountX * m_iChunkCountY;
	m_kWorldChunkVector.resize(iChunkCount);

	for(int iChunkIndexX = 0; iChunkIndexX < m_iChunkCountX; iChunkIndexX++)
	{
		for(int iChunkIndexY = 0; iChunkIndexY < m_iChunkCountY; iChunkIndexY++)
		{
			int iChunkIndex = iChunkIndexY * m_iChunkCountX + iChunkIndexX;
			m_kWorldChunkVector[iChunkIndex] 
				= T_NEW_D SE_SceneWorldChunk(iChunkIndexX,iChunkIndexY,m_pkWorldMapParent,m_pkWorldTileParent,this);
			m_kWorldBound.Adjust( m_kWorldChunkVector[iChunkIndex]->GetBoundingVolume() );

			_attachMessageHandler(m_kWorldChunkVector[iChunkIndex]);
			m_kWorldChunkVector[iChunkIndex]->SetActive(false);
		}
	}
	m_kWorldBound.Update();
}
//----------------------------------------------------
void SE_SceneWorldChunkCluster::_uninitChunks()
{
	for(int iChunkIndexX = 0; iChunkIndexX < m_iChunkCountX; iChunkIndexX++)
	{
		for(int iChunkIndexY = 0; iChunkIndexY < m_iChunkCountY; iChunkIndexY++)
		{
			int iChunkIndex = iChunkIndexY * m_iChunkCountX + iChunkIndexX;
			SE_SceneWorldChunk* pkChunk = m_kWorldChunkVector[iChunkIndex];
			T_ASSERT(pkChunk && "NULL Chunk");
			if(pkChunk)
			{
				_detachMessageHandler(pkChunk);
				if( pkChunk->IsTerrainLoaded() )
				{
					GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING,
						"uninit unload chunk %d" , pkChunk->GetDebugChunkID() );
					pkChunk->PreUnLoadTerrain();
					pkChunk->UnLoadTerrain();
				}
				if( pkChunk->IsModelsLoaded() )
				{
					pkChunk->PreUnLoadModels();
				}
				if( pkChunk->IsGrassLoaded() )
				{
					pkChunk->PreUnLoadGrass();
					pkChunk->UnLoadGrass();
				}
				if( pkChunk->IsWaterLoaded() )
				{
					pkChunk->PreUnLoadWater();
					pkChunk->UnLoadWater();
				}
				T_DELETE_D pkChunk;
				m_kWorldChunkVector[iChunkIndex] = NULL;
			}
		}
	}
	m_kWorldChunkVector.clear();
}
//--------------------------------------------------------------------------------
void SE_SceneWorldChunkCluster::GetWhichChunk( int& iChunkIndexX, int& iChunkIndexY, float fPointX, float fPointY )
{
	float fOffSetX = fPointX - m_kAreaRect.iMinX;
	float fOffSetY = fPointY - m_kAreaRect.iMinY;

	iChunkIndexX = (int)(fOffSetX / GfxGetWorldConfig().CFG_WORLD_CHUNK_WIDTH);
	iChunkIndexY = (int)(fOffSetY / GfxGetWorldConfig().CFG_WORLD_CHUNK_WIDTH);

	// Clamp(0, m_iChunkCountX - 1)
	if(iChunkIndexX < 0)
		iChunkIndexX = 0;
	if(iChunkIndexX >= m_iChunkCountX)
		iChunkIndexX = m_iChunkCountX - 1;

	// Clamp(0, m_iChunkCountY - 1)
	if(iChunkIndexY < 0)
		iChunkIndexY = 0;
	if(iChunkIndexY >= m_iChunkCountY)
		iChunkIndexY = m_iChunkCountY - 1;
}
//----------------------------------------------------
void SE_SceneWorldChunkCluster::_computeAreaRect()
{
	Utility::UT_AABB& rkTileAreaRect = m_pkWorldTileParent->GetAreaRect();
	float fClusterBaseX = rkTileAreaRect.iMinX + m_iChunkClusterIndexX * GfxGetWorldConfig().CFG_WORLD_CHUNKCLUSTER_WIDTH;
	float fClusterBaseY = rkTileAreaRect.iMinY + m_iChunkClusterIndexY * GfxGetWorldConfig().CFG_WORLD_CHUNKCLUSTER_WIDTH;
	float fClusterTopX = fClusterBaseX + GfxGetWorldConfig().CFG_WORLD_CHUNKCLUSTER_WIDTH;
	float fClusterTopY = fClusterBaseY + GfxGetWorldConfig().CFG_WORLD_CHUNKCLUSTER_WIDTH;

	m_kAreaRect.iMinX = (int)fClusterBaseX;
	m_kAreaRect.iMinY = (int)fClusterBaseY;
	m_kAreaRect.iMaxX = (int)fClusterTopX;
	m_kAreaRect.iMaxY = (int)fClusterTopY;
}

void SE_SceneWorldChunkCluster::SetLoadingFlag( int nX , int nY , bool bLoading )
{
	T_ASSERT( GfxIsMainThread() );
	SE_SceneWorldChunk* pkWorldChunk = GetWorldChunk(nX,nY);
	T_ASSERT(pkWorldChunk);
	pkWorldChunk->SetLoadingFlag( bLoading );
}
bool SE_SceneWorldChunkCluster::IsLoading(  )
{
	T_ASSERT( GfxIsMainThread() );
	for (SE_SceneWorldChunkVector::iterator itr = m_kWorldChunkVector.begin(); itr != m_kWorldChunkVector.end(); ++itr)
	{
		SE_SceneWorldChunk* pkChunk = (*itr);
		if (pkChunk->IsLoading() )
			return true;
	}
	return false;
}







