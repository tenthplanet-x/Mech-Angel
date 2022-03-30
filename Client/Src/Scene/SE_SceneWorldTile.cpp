#include "ClientPCH.h"
#include "SE_SceneWorldTile.h"
#include "SE_SceneWorldMap.h"
#include "SE_SceneWorldChunkCluster.h"
#include "SE_SceneTerrainInfo.h"
#include "MG_MessageBase.h"
#include "TS_WorldConfig.h"
#include "MTF_ClientMapGlobalInfo.h"
#include "RG_ClientPointLightManager.h"
//--------------------------------------------
using namespace FileSystem;
//--------------------------------------------
SE_SceneWorldTile::SE_SceneWorldTile(int iIndexX,int iIndexY,
									 SE_SceneWorldMap* pkWorldMapParent)
: m_iTileIndexX(iIndexX)
, m_iTileIndexY(iIndexY)
, m_pkSceneWorldMapParent(pkWorldMapParent)
, m_pkWorldTileFile(NULL)
, m_bFileLoaded(false)
, m_bPostLoaded(false)
{
	_computeAreaRect();

	m_iChunkClusterCountX = GfxGetWorldConfig().CFG_WORLD_CHUNKCLUSTER_COUNTX_PER_TILE;
	m_iChunkClusterCountY = GfxGetWorldConfig().CFG_WORLD_CHUNKCLUSTER_COUNTX_PER_TILE;

	m_pkMapGlobalInfo = pkWorldMapParent->GetMapGlobalInfo();
	m_kWorldChunkClusterVector.resize(m_iChunkClusterCountX * m_iChunkClusterCountY);
	for(int iChunkClusterIndexX = 0; iChunkClusterIndexX < m_iChunkClusterCountX; iChunkClusterIndexX++)
	{
		for(int iChunkClusterIndexY = 0; iChunkClusterIndexY < m_iChunkClusterCountY; iChunkClusterIndexY++)
		{
			int iChunkClusterIndex = iChunkClusterIndexY * m_iChunkClusterCountX + iChunkClusterIndexX;
			m_kWorldChunkClusterVector[iChunkClusterIndex] 
				= T_NEW_D SE_SceneWorldChunkCluster(iChunkClusterIndexX,iChunkClusterIndexY,m_pkSceneWorldMapParent,this);
			m_kWorldBound.Adjust( m_kWorldChunkClusterVector[iChunkClusterIndex]->GetBoundingVolume() );
		
			_attachMessageHandler(m_kWorldChunkClusterVector[iChunkClusterIndex]);
			m_kWorldChunkClusterVector[iChunkClusterIndex]->SetActive(false);
		}
	}
	m_kWorldBound.Update();
}
//--------------------------------------------
SE_SceneWorldTile::~SE_SceneWorldTile()
{
	for(int iChunkClusterIndexX = 0; iChunkClusterIndexX < m_iChunkClusterCountX; iChunkClusterIndexX++)
	{
		for(int iChunkClusterIndexY = 0; iChunkClusterIndexY < m_iChunkClusterCountY; iChunkClusterIndexY++)
		{
			int iChunkClusterIndex = iChunkClusterIndexY * m_iChunkClusterCountY + iChunkClusterIndexX;
			SE_SceneWorldChunkCluster* pkCluster = m_kWorldChunkClusterVector[iChunkClusterIndex]; 
			T_ASSERT(pkCluster && "NULL Cluster");
			if(pkCluster)
			{
				_detachMessageHandler(pkCluster);			
				T_DELETE_D pkCluster;
				m_kWorldChunkClusterVector[iChunkClusterIndex] = NULL;
			}
		}
	}
	if( m_bFileLoaded )
	{
		GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING, 
			"unload tile 0x%x in destructor" , m_pkWorldTileFile );
		UnLoadTileFile();
	}
	m_kWorldChunkClusterVector.clear();
	PreUnLoad();
}
//--------------------------------------------
bool SE_SceneWorldTile::RecvMessage( MG_MessageBase& rkMessage )
{
	MG_MessageHandlerInterface::RecvMessage(rkMessage);
	return true;
}
//--------------------------------------------
SE_SceneWorldChunkCluster* SE_SceneWorldTile::GetWorldChunkCluster( int iChunkClusterIndexX, int iChunkClusterIndexY )
{
	if(iChunkClusterIndexX < 0 || iChunkClusterIndexX >= GfxGetWorldConfig().CFG_WORLD_CHUNKCLUSTER_COUNTX_PER_TILE)
		return NULL;
	
	if(iChunkClusterIndexY < 0 || iChunkClusterIndexY >= GfxGetWorldConfig().CFG_WORLD_CHUNKCLUSTER_COUNTX_PER_TILE)
		return NULL;

	int iChunkClusterIndex = iChunkClusterIndexX + iChunkClusterIndexY * m_iChunkClusterCountX;
	SE_SceneWorldChunkCluster* pkResult = m_kWorldChunkClusterVector[iChunkClusterIndex];
	T_ASSERT(pkResult && "NULL Cluster");
	return pkResult;
}
//--------------------------------------------
bool SE_SceneWorldTile::LoadTileFile()
{
	if(m_bFileLoaded)
		return false;
	
	T_ASSERT(!m_pkWorldTileFile &&  "mem leak");
	_generateTileFileName();
	//如果没有global.bin文件，说明是不需要差异表升级的地图
	ClientMapGlobalInfoManager* pkGlobal = m_pkMapGlobalInfo;
	if ( m_pkMapGlobalInfo->GetFile() == NULL )
	{
		pkGlobal = NULL;
	}
	m_pkWorldTileFile = T_NEW_D MTF_ClientTile(m_iTileIndexX + 
		m_iTileIndexY * Terrain::TS_WorldConfig::GetSingleton().CFG_WORLD_TILE_COUNTX_PER_MAP , pkGlobal );	
	bool bResult = false;
	if ( !m_kDecorationTileFileName.empty() )
	{
		bResult = m_pkWorldTileFile->ReadFromFile( m_kDecorationTileFileName );
	}
	if ( !bResult )
	{
		bResult = m_pkWorldTileFile->ReadFromFile(m_kTileFileName);
	}
	if(!bResult)
	{
		T_DELETE_D m_pkWorldTileFile;
		m_pkWorldTileFile = NULL;
		m_bFileLoaded = false;
		return false;
	}
	const MTF_ClientTile::TilePointLightInfoMap& rkTilePLs = m_pkWorldTileFile->GetTilePointLights();

	for (MTF_ClientTile::TilePointLightInfoMap::const_iterator itr = rkTilePLs.begin();
		itr != rkTilePLs.end(); ++itr)
	{
		MTF_ClientTile::ClientTilePointLightInfo kPL;
		kPL.kPointLightColor = itr->second.kPointLightColor;
		kPL.kPos =  itr->second.kPos;
		kPL.kParam = itr->second.kParam;
		kPL.nCtrlType = itr->second.nCtrlType;
		kPL.kPLICParams = itr->second.kPLICParams;
		m_kTilePLs.insert( std::make_pair(itr->first,kPL ) );
	}
	m_bFileLoaded = true;
	return true;
}
//--------------------------------------------
bool SE_SceneWorldTile::UnLoadTileFile()
{
	if(!m_bFileLoaded)
		return false;
	T_ASSERT(m_pkWorldTileFile && "mem leak");
	GfxWriteLog( LOG_BACKLOADING_FILELINE , LOG_SWITCH_BACKLOADING ,
		"delete tile file 0x%x" , m_pkWorldTileFile );
	T_DELETE_D m_pkWorldTileFile;
	m_pkWorldTileFile = NULL;
	m_bFileLoaded = false;

	//关闭文件
	return true;
}
//--------------------------------------------
bool SE_SceneWorldTile::CanSafeUnLoadTileFile()
{
	//这个函数会在多个线程中被调用
	//Load标记只在主线程中进行设置
	//当Load为True，只可能是完成了加载
	//操作之后开始执行PostLoad
	int iClusterCount = m_kWorldChunkClusterVector.size();
	for(int iClusterIndex = 0; iClusterIndex < iClusterCount; iClusterIndex++)
	{
		SE_SceneWorldChunkCluster* pkCluster = m_kWorldChunkClusterVector[iClusterIndex];
		if( pkCluster->IsLoaded() || pkCluster->IsLoading() )
			return false;
	}
	return true;
}
//--------------------------------------------
MTF_ClientTile* SE_SceneWorldTile::GetWorldTileFile()
{
	return m_pkWorldTileFile;
}
//--------------------------------------------
Utility::UT_AABB& SE_SceneWorldTile::GetAreaRect()
{
	return m_kAreaRect;
}
SE_SceneWorldChunk* SE_SceneWorldTile::GetChunkByCenter( const NiPoint3& kChunkCenter  )
{
	int iClusterIndexX,iClusterIndexY;
	_getWhichChunkCluster(iClusterIndexX,iClusterIndexY,kChunkCenter.x ,kChunkCenter.y  );

	SE_SceneWorldChunkCluster* pkCluster = GetWorldChunkCluster(iClusterIndexX ,iClusterIndexY);
	if(pkCluster)
		return pkCluster->GetChunkByCenter( kChunkCenter );
	return NULL;
}

bool SE_SceneWorldTile::GetChunkByCenter( const NiPoint3& kChunkCenter , int& iClusterX , int& iClusterY , int& iChunkX , int& iChunkY )
{
	_getWhichChunkCluster(iClusterX,iClusterY,kChunkCenter.x ,kChunkCenter.y  );
	SE_SceneWorldChunkCluster* pkCluster = GetWorldChunkCluster(iClusterX ,iClusterY);
	if(pkCluster)
	{
		pkCluster->GetWhichChunk( iChunkX , iChunkY , kChunkCenter.x , kChunkCenter.y );
		return true;
	}
	return false;
}
void SE_SceneWorldTile::RegisterLocalNPC( long lSpawnID , const NiPoint3& rkPos )
{
	int iClusterX , iClusterY;
	_getWhichChunkCluster(iClusterX,iClusterY,rkPos.x ,rkPos.y );
	SE_SceneWorldChunkCluster* pkCluster = GetWorldChunkCluster(iClusterX ,iClusterY);
	if(pkCluster)
	{
		pkCluster->RegisterLocalNPC( lSpawnID , rkPos );
	}
	else
	{
		GfxWriteLog( LOG_TAG_ERROR , LOG_SWITCH_ERROR ,
			"本地NPC坐标有误！[%d]:[%f %f %f]" , 
			lSpawnID , rkPos.x , rkPos.y  , rkPos.z );
	}
}
//--------------------------------------------
void SE_SceneWorldTile::_generateTileFileName()
{
	StringType& rkWorldName = GfxGetWorldConfig().CFG_WORLD_FILE_BASE;
	char cResult[256];
	int iPseudoIndexX,iPseudoIndexY;
	GfxGetWorldConfig().GetPseudoIndexFromRealIndex(m_iTileIndexX,m_iTileIndexY,&iPseudoIndexX,&iPseudoIndexY);

	sprintf_s(cResult,"%s_%d_%d.tile",rkWorldName.c_str(),iPseudoIndexX,iPseudoIndexY);
	m_kTileFileName = cResult;

	if ( !GfxGetWorldConfig().CFG_WORLD_FILE_DECORATION_BASE.empty() )
	{
		sprintf_s(cResult,"%s_%d_%d.tile",
			GfxGetWorldConfig().CFG_WORLD_FILE_DECORATION_BASE.c_str(),
			iPseudoIndexX,iPseudoIndexY);
		m_kDecorationTileFileName = cResult;
	}
	else
	{
		m_kDecorationTileFileName = "";
	}
}
//--------------------------------------------
void SE_SceneWorldTile::_getWhichChunkCluster( int& iClusterIndexX, int& iClusterIndexY, float fPointX, float fPointY )
{
	float fOffSetX = fPointX - m_kAreaRect.iMinX;
	float fOffSetY = fPointY - m_kAreaRect.iMinY;

	iClusterIndexX = (int)(fOffSetX / GfxGetWorldConfig().CFG_WORLD_CHUNKCLUSTER_WIDTH);
	iClusterIndexY = (int)(fOffSetY / GfxGetWorldConfig().CFG_WORLD_CHUNKCLUSTER_WIDTH);

	// Clamp(0, m_iChunkClusterCountX - 1)
	if(iClusterIndexX < 0)
		iClusterIndexX = 0;
	if(iClusterIndexX >= m_iChunkClusterCountX)
		iClusterIndexX = m_iChunkClusterCountX - 1;

	// Clamp(0, m_iChunkClusterCountY - 1)
	if(iClusterIndexY < 0)
		iClusterIndexY = 0;
	if(iClusterIndexY >= m_iChunkClusterCountY)
		iClusterIndexY = m_iChunkClusterCountY - 1;
}
//--------------------------------------------
void SE_SceneWorldTile::_computeAreaRect()
{
	float fTileBaseX = m_iTileIndexX * GfxGetWorldConfig().CFG_WORLD_TILE_WIDTH;
	float fTileBaseY = m_iTileIndexY * GfxGetWorldConfig().CFG_WORLD_TILE_WIDTH;
	float fTileTopX = fTileBaseX + GfxGetWorldConfig().CFG_WORLD_TILE_WIDTH;
	float fTileTopY = fTileBaseY + GfxGetWorldConfig().CFG_WORLD_TILE_WIDTH;

	m_kAreaRect.iMinX = (int)fTileBaseX;
	m_kAreaRect.iMinY = (int)fTileBaseY;
	m_kAreaRect.iMaxX = (int)fTileTopX;
	m_kAreaRect.iMaxY = (int)fTileTopY;
}

void SE_SceneWorldTile::PostLoad()
{
    if (m_bPostLoaded)
    {
        return;
    }

    m_bPostLoaded = true;

	if ( RG_ClientPointLightManager::GetSingletonPtr() && 
		( g_settings.LOD_TERRAIN_SHADER > 0 ||
		GfxGetWorldConfig().CFG_BAKING_TYPE != Terrain::TS_WorldConfig::NORMAL) )
	{
		RG_ClientPointLightManager& rkPLManager = RG_ClientPointLightManager::GetSingleton();

		for (MTF_ClientTile::TilePointLightInfoMap::const_iterator itr = m_kTilePLs.begin();
			itr != m_kTilePLs.end(); ++itr)
		{
			rkPLManager.CheckInScenePointLightInfo(itr->first, 
				itr->second.kPointLightColor, itr->second.kPos, itr->second.kParam,
				itr->second.nCtrlType, itr->second.kPLICParams );
		}
		rkPLManager.CreateCheckedInScenePointLights();
	}
}

void SE_SceneWorldTile::PreUnLoad()
{
	if ( RG_ClientPointLightManager::GetSingletonPtr()&&
		( g_settings.LOD_TERRAIN_SHADER > 0|| GfxGetWorldConfig().CFG_BAKING_TYPE != Terrain::TS_WorldConfig::NORMAL ) )
	{
		RG_ClientPointLightManager& rkPLManager = RG_ClientPointLightManager::GetSingleton();
		for (MTF_ClientTile::TilePointLightInfoMap::const_iterator itr = m_kTilePLs.begin();
			itr != m_kTilePLs.end(); ++itr)
		{
			rkPLManager.ReleasePointLight( itr->first );
		}
	}
}





