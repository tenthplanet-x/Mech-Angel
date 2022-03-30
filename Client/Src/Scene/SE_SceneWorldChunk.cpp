#include "ClientPCH.h"
#include "BS_ClientWaterMeshManager.h"
#include "FX_ClientFxManager.h"
#include "GD_ClientAsyncModelManager.h"
#include "GD_Material.h"
#include "GD_MaterialManager.h"
#include "GD_ModelContent.h"
#include "MG_MessageBase.h"
#include "MTF_ClientTile.h"
#include "RG_ClientPointLightManager.h"
#include "RG_RenderManager.h"
#include "SE_SceneManager.h"
#include "SE_SceneTerrainInfo.h"
#include "SE_SceneWorldChunk.h"
#include "SE_SceneWorldChunkCluster.h"
#include "SE_SceneWorldMap.h"
#include "SE_SceneWorldTile.h"
#include "SE_Terraincallback.h"
#include "SE_WorldChunkGeometryDataManager.h"
#include "SE_UntouchedModel.h"
#include "LC_ClientLogicManager.h"
#include "LC_ClientNPCManager.h"
#include "BL_AsyncOperationManager.h"
using namespace Render;
using namespace Terrain;
using namespace Utility;
using namespace Data;
using namespace FileSystem;
//------------------------------------------------------
SE_SceneWorldChunk::SE_SceneWorldChunk(int iIndexX,int iIndexY,
									   SE_SceneWorldMap* pkWorldMapParent,
									   SE_SceneWorldTile* pkWorldTileParent ,
									   SE_SceneWorldChunkCluster* pkWorldChunkClusterParent)
: m_iChunkIndexX(iIndexX)
, m_iChunkIndexY(iIndexY)
, m_pkWorldMapParent(pkWorldMapParent)
, m_pkWorldTileParent(pkWorldTileParent)
, m_pkWorldChunkClusterParent(pkWorldChunkClusterParent)
, m_spNiTerrainRootNode(NULL)
, m_spChunkTerrainGeometry(NULL)
, m_spChunkTerrainGeometryData(NULL)
, m_spTerrainAlphaMap(NULL)
, m_bLoadTerrain(false)
, m_bLoadModel(false)
, m_bLoadGrass(false)
, m_bLoadingGrass(false)
, m_bLoadWater(false)
, m_pkGrassCluster(NULL)
, m_pkHeightField(NULL)
, m_kCurrentPlayerPos( NiPoint3::ZERO )
, m_fWaterHeight(0.0f)
, m_fPlayerDistance(0)
#ifdef _ENABLE_PHYSX
, m_pkPhyxActor(NULL)
, m_pkPhysXStream(NULL)
#endif
, m_pucVisibilityData(NULL)
, m_iVisibilityDataWidth(0)
, m_pfHeights(NULL)
, m_pkNormal(NULL)
, m_pfWalkableHeights(NULL)
, m_pkWalkableNormal(NULL)
{
	for ( int i = 0 ; i < 20 ; i++ )
	{
		m_nLocalNPC[i] = -1;
	}
	for(int iMaterialIndex = 0; iMaterialIndex < 4; iMaterialIndex++)
		m_pkTerrainMaterial[iMaterialIndex]	= NULL;

	int iClusterIndexX = m_pkWorldChunkClusterParent->GetChunkClusterIndexX();
	int iClusterIndexY = m_pkWorldChunkClusterParent->GetChunkClusterIndexY();
	int iChunkCountXPerCluster = GfxGetWorldConfig().CFG_WORLD_CHUNK_COUNTX_PER_CLUSTER;
	int iChunkCountYPerCluster = GfxGetWorldConfig().CFG_WORLD_CHUNK_COUNTX_PER_CLUSTER;
	m_iChunkTileIndexX = iClusterIndexX * iChunkCountXPerCluster + m_iChunkIndexX;
	m_iChunkTileIndexY = iClusterIndexY * iChunkCountYPerCluster + m_iChunkIndexY;

	m_iChunkID = GfxGetWorldConfig().ComputeChunkID(
		pkWorldTileParent->GetTileIndexX(),
		pkWorldTileParent->GetTileIndexY(),
		iClusterIndexX,iClusterIndexY,m_iChunkIndexX,m_iChunkIndexY);

	_computeAreaRect();

	FileSystem::WorldMapFile* pkMapFile = pkWorldMapParent->GetWorldMapFile();
	m_kWorldBound.Copy( pkMapFile->GetWorldChunkBoundingVolume( pkWorldTileParent->GetTileIndexX() , 
		pkWorldTileParent->GetTileIndexY() , m_iChunkTileIndexX , m_iChunkTileIndexY ) );
	NiPoint3 kC = m_kWorldBound.GetCenter();
	/*GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING,
		"Init Tile %d %d Chunk %d %d with BB center at [%f %f %f]" , 
		pkWorldTileParent->GetTileIndexX() , 
		pkWorldTileParent->GetTileIndexY() , m_iChunkTileIndexX , m_iChunkTileIndexY ,
		kC.x , kC.y , kC.z );*/
	
	int iChunkVertexCountX = GfxGetWorldConfig().CFG_WORLD_CHUNK_VERTEXCOUNTX;
	int iChunkVertexCountY = GfxGetWorldConfig().CFG_WORLD_CHUNK_VERTEXCOUNTX;
	m_iChunkVertexCount = iChunkVertexCountX * iChunkVertexCountY;

	m_spNiTerrainRootNode = NiNew NiNode;
	m_spNiWaterRootNode	= NiNew NiNode;

	Utility::UT_AABB& rkParentClusterRect = m_pkWorldChunkClusterParent->GetAreaRect();

	m_kChunkCenter = NiPoint3(  (float)(m_kAreaRect.iMinX - rkParentClusterRect.iMinX), 
		(float)(m_kAreaRect.iMinY - rkParentClusterRect.iMinY),
		0.0f );

	m_spNiTerrainRootNode->SetTranslate(m_kChunkCenter);

	m_spNiWaterRootNode->SetTranslate(
        (float)(m_kAreaRect.iMinX - rkParentClusterRect.iMinX),
        (float)(m_kAreaRect.iMinY - rkParentClusterRect.iMinY),
        0.0f);
 
	SetActive(false);

	m_pkHeightField = pkWorldMapParent->GetHeightField();
	m_iHeightFieldX = pkWorldTileParent->GetTileIndexX() * ( iChunkVertexCountX - 1 ) *  GfxGetWorldConfig().CFG_WORLD_CHUNK_COUNTX_PER_TILE + m_iChunkTileIndexX * ( iChunkVertexCountX - 1 );
	m_iHeightFieldY = pkWorldTileParent->GetTileIndexY() * ( iChunkVertexCountY - 1 ) *  GfxGetWorldConfig().CFG_WORLD_CHUNK_COUNTX_PER_TILE + m_iChunkTileIndexY * ( iChunkVertexCountY - 1 );

	m_pkVisibility = pkWorldMapParent->GetVisibilityField();
	m_iVisibleFieldIndexX = pkWorldTileParent->GetTileIndexX() * GfxGetWorldConfig().CFG_WORLD_CHUNK_COUNTX_PER_TILE + m_iChunkTileIndexX;
	m_iVisibleFieldIndexY = pkWorldTileParent->GetTileIndexY() * GfxGetWorldConfig().CFG_WORLD_CHUNK_COUNTX_PER_TILE + m_iChunkTileIndexY;

	int nSamplePerChunk = Terrain::WALKABLE_GRID_SCALE * Terrain::CHUNK_VERTEX_COUNT ;
	m_pkWalkableField = pkWorldMapParent->GetWalkableField();
	m_iWalkableFieldX = pkWorldTileParent->GetTileIndexX() * nSamplePerChunk *  GfxGetWorldConfig().CFG_WORLD_CHUNK_COUNTX_PER_TILE + m_iChunkTileIndexX * nSamplePerChunk; 
	m_iWalkableFieldY = pkWorldTileParent->GetTileIndexY() * nSamplePerChunk *  GfxGetWorldConfig().CFG_WORLD_CHUNK_COUNTX_PER_TILE + m_iChunkTileIndexY * nSamplePerChunk;

#if 0
	NiPoint3* pkChunkAreaVertex = NiNew NiPoint3[4];
	pkChunkAreaVertex[0] = NiPoint3(0.0f,0.0f,1.0f);
	pkChunkAreaVertex[1] = NiPoint3(0.0f,GfxGetWorldConfig().CFG_WORLD_CHUNK_WIDTH,1.0f);
	pkChunkAreaVertex[2] = NiPoint3(GfxGetWorldConfig().CFG_WORLD_CHUNK_WIDTH,GfxGetWorldConfig().CFG_WORLD_CHUNK_WIDTH,1.0f);
	pkChunkAreaVertex[3] = NiPoint3(GfxGetWorldConfig().CFG_WORLD_CHUNK_WIDTH,0.0f,1.0f);
	NiBool* pbLinesFlag = NiAlloc(NiBool,4);

	NiLinesPtr pkAreaLine = NiNew NiLines(4,pkChunkAreaVertex,NULL,NULL,0,NiGeometryData::NBT_METHOD_NONE,pbLinesFlag);

	m_spNiTerrainRootNode->AttachChild(pkAreaLine);
#endif

	m_spNiLoadedWater = NiNew NiNode;

	NiNodePtr spClusterNode = pkWorldChunkClusterParent->GetNiSceneRootNode();
	NiPoint3 kPos = spClusterNode->GetTranslate();
	m_spNiLoadedWater->SetTranslate( kPos );

	for ( int i = 0 ; i < MAX_DYNAMIC_POINT_LIGHT ; i++ )
	{
		m_vPLID[i] = -1;
	}
	m_bLoading = false;

	char buffer[MAX_PATH];
	
	if ( !GfxGetWorldConfig().CFG_WORLD_FILE_DECORATION_BASE.empty() )
	{
		StringType strFileNameBase = GfxGetWorldConfig().CFG_WORLD_FILE_DECORATION_BASE;
		StringType kDDSFilePath = strFileNameBase + StringType("_Chunk\\");
		sprintf_s( buffer , MAX_PATH , "%sBlend_%d_%d_%d_%d.dds" , 
			kDDSFilePath.c_str() , m_pkWorldTileParent->GetTileIndexX(),
			m_pkWorldTileParent->GetTileIndexY(), m_iChunkTileIndexX , 
			m_iChunkTileIndexY );
		m_strPreBlendTexNameDecoration = StringType( buffer );
	}
	else
	{
		m_strPreBlendTexNameDecoration = "";
	}
	
	{
		StringType strFileNameBase = GfxGetWorldConfig().CFG_WORLD_FILE_BASE;
		StringType kDDSFilePath = strFileNameBase + StringType("_Chunk\\");
		sprintf_s( buffer , MAX_PATH , "%sBlend_%d_%d_%d_%d.dds" , 
			kDDSFilePath.c_str() , m_pkWorldTileParent->GetTileIndexX(),
			m_pkWorldTileParent->GetTileIndexY(), m_iChunkTileIndexX , 
			m_iChunkTileIndexY );
		m_strPreBlendTexName = StringType( buffer );
	}
}
//------------------------------------------------------
SE_SceneWorldChunk::~SE_SceneWorldChunk()
{
	T_ASSERT(!m_bLoadTerrain && "Mem Leak");
	T_ASSERT(!m_bLoadModel && "Mem Leak");
	T_ASSERT(!m_bLoadGrass && "Mem Leak");

	m_spNiTerrainRootNode = NULL;
	m_spNiLoadedWater = NULL;
	m_kChunkModelInfo.clear();
}
//------------------------------------------------------
bool SE_SceneWorldChunk::RecvMessage(MG_MessageBase& rkMessage)
{
	MG_MessageHandlerInterface::RecvMessage(rkMessage);
	
	BEGIN_MESSAGE_HANDLE
	MESSAGE_HANDLE(MGT_TIMER_TICK,			_event_TimerTick)
	MESSAGE_HANDLE(MGT_LOGIC_PLAYER_MOVE,	_event_LogicPlayerMove)
	END_MESSAGE_HANDLE
	
	return true;
}
//------------------------------------------------------
bool SE_SceneWorldChunk::LoadTerrain()
{
	T_ASSERT(!m_bLoadTerrain && "Reload World Chunk");
	MTF_ClientTile* pkWorldTileFile = m_pkWorldTileParent->GetWorldTileFile();
	if( pkWorldTileFile == NULL )
		return false;
	//T_PROFILE_TIMER("load terrain geometry");

	_loadTerrainGeometryData(pkWorldTileFile);
	_loadTerrainMaterial(pkWorldTileFile);

	m_spNiTerrainRootNode->UpdateProperties();
	m_spNiTerrainRootNode->Update(0.0f);

	return true;
}

bool SE_SceneWorldChunk::PostLoadTerrain()
{
	//主线程调用
	//地形不产生投影
	if ( m_spChunkTerrainGeometry == NULL )
	{
		m_bLoadTerrain = true;
		return true;
	}
	if ( m_pkVisibility && m_pucVisibilityData != NULL )
	{
		m_pkVisibility->AddChunkLightMap(m_iVisibleFieldIndexX, m_iVisibleFieldIndexY, m_pucVisibilityData, m_iVisibilityDataWidth);
	}
	else
	{
		m_pkVisibility->AddChunkLightMap(m_iVisibleFieldIndexX, m_iVisibleFieldIndexY , NULL , 0 );
	}
	if ( m_pfHeights && m_pkNormal )
	{
		m_pkHeightField->Fill(m_iVisibleFieldIndexX, m_iVisibleFieldIndexY,
			m_iHeightFieldX , m_iHeightFieldY , m_pfHeights , m_pkNormal , Terrain::CHUNK_VERTEX_COUNT + 1 );
	}
	if ( m_pfWalkableHeights && m_pkWalkableNormal )
	{
		m_pkWalkableField->Fill(m_iVisibleFieldIndexX, m_iVisibleFieldIndexY,
			m_iWalkableFieldX , m_iWalkableFieldY , m_pfWalkableHeights , m_pkWalkableNormal , Terrain::WALKABLE_GRID_SCALE * Terrain::CHUNK_VERTEX_COUNT + 1  );
	}
	SE_SceneManager::GetSingleton().AttachToLightEffect(m_spNiTerrainRootNode,false);

	if ( g_settings.LOD_TERRAIN_SHADER > 0 ||GfxGetWorldConfig().CFG_BAKING_TYPE != TS_WorldConfig::NORMAL )
	{
		RG_ClientPointLightManager::GetSingleton().ApplyPointLightsToModel(MAX_DYNAMIC_POINT_LIGHT, m_vPLID, true, m_spNiTerrainRootNode);

		/*for ( int i = 0 ; i < MAX_DYNAMIC_POINT_LIGHT ; i++ )
		{
			if ( m_vPLID[i] != -1 )
			{
				GfxWriteLog( LOG_TAG_CLIENT_CRITICAL , LOG_SWITCH_CLIENT_CRITICAL ,
					"apply pl %d to chunk %d" , m_vPLID[i] , m_iChunkID );
			}
		}*/
	}

	m_spNiTerrainRootNode->UpdateEffects();
#ifdef _ENABLE_PHYSX
	if(g_settings.PHYSX_SWITCH)
	{
		if ( m_pkPhysXStream && m_pkPhysXStream->valid() )
		{
			//加载物理信息必须在主线程中，这样就不会有锁的问题
			m_pkPhyxActor = SE_SceneManager::GetSingleton().AddPhyxCollisionSceneChunk( *m_pkPhysXStream );
		}
	}
	m_pkPhyxActor = NULL; 	
#endif


	m_bLoadTerrain = true;


	//
	LC_ClientNPCManager* pkLocalNpcManager = CLIENT_GET_LOCAL_NPC_MANAGER();
	NiPoint3 kCenter = m_kWorldBound.GetCenter();
	kCenter.z = m_pkWalkableField->GetHeight(kCenter.x , kCenter.y );
	UT_Vec3Int kCPos = UT_EngineMath::ConvertNiPoint3ToVec3Int( kCenter );
	
	if ( pkLocalNpcManager )
	{
#if 1
		for ( int i = 0 ; i < 20 ; i++ )
		{
			if ( i > g_settings.BL_PRESSURE_TEST_MODEL )
			{
				break;
			}
			int nType = UT_MathBase::RandInRangeInt( 0 ,
				g_settings.BL_PRESSURE_TEST_CHARS.size() - 1 );
			m_nLocalNPC[i] = pkLocalNpcManager->CreateLocalNPC( 
				g_settings.BL_PRESSURE_TEST_CHARS[nType] , kCPos );
		}
#endif

        if ( !m_kLocalNPCList.empty() )
        {
            for ( size_t i = 0 ; i < m_kLocalNPCList.size() ; i++ )
            {
#ifndef _MAP_VIEWER_BUILD
                long lS = pkLocalNpcManager->CreateLocalNPC( 
                    m_kLocalNPCList[i] , "LocalNPC" );
#else
                long lS = pkLocalNpcManager->CreateLocalNPC( 
                    m_kLocalNPCList[i] , "LocalNPC", true );
#endif
                m_kLocalNPCCreateID.push_back( lS );
            }
        }
	}


	return true;
}

bool SE_SceneWorldChunk::PreUnLoadTerrain()
{
	if ( m_spChunkTerrainGeometry == NULL )
	{
		m_bLoadTerrain = false;
		return true;
	}
	LC_ClientNPCManager* pkLocalNpcManager = CLIENT_GET_LOCAL_NPC_MANAGER();
	if ( pkLocalNpcManager )
	{
		for ( int i = 0 ; i < 20 ; i++ )
		{
			if ( m_nLocalNPC[i] != -1 )
			{
				pkLocalNpcManager->DestroyLocalNPC( m_nLocalNPC[i] );
			}
		}
		for ( size_t i = 0 ; i< m_kLocalNPCCreateID.size() ; i++ )
		{
			if (m_kLocalNPCCreateID[i] != -1)
			{
				pkLocalNpcManager->DestroyLocalNPC( m_kLocalNPCCreateID[i] );
			}
		}
		m_kLocalNPCCreateID.clear();
	}
    m_pkVisibility->RemoveChunkLightMap(m_iVisibleFieldIndexX, m_iVisibleFieldIndexY);
	m_pkHeightField->Remove( m_iVisibleFieldIndexX, m_iVisibleFieldIndexY );
	m_pkWalkableField->Remove( m_iVisibleFieldIndexX, m_iVisibleFieldIndexY );

	SE_SceneManager::GetSingleton().DetachToLightEffect(m_spNiTerrainRootNode,false);
#ifdef _ENABLE_PHYSX
	if(g_settings.PHYSX_SWITCH)
	{
		if ( m_pkPhyxActor != NULL )
		{
			SE_SceneManager::GetSingleton().RemovePhyxCollisionSceneChunk(m_pkPhyxActor);
		}
		m_pkPhyxActor = NULL;
	}
#endif
	if ( m_spChunkTerrainGeometry )
	{
		m_spNiTerrainRootNode->DetachChild(m_spChunkTerrainGeometry);
	}
	RG_ClientPointLightManager* pkPLManager = RG_ClientPointLightManager::GetSingletonPtr();
	if (pkPLManager != NULL)
	{
		for ( int i = 0 ; i < MAX_DYNAMIC_POINT_LIGHT ; i++ )
		{
			if ( m_vPLID[i] != -1 )
			{
				pkPLManager->ReleasePointLight( m_vPLID[i] );
				/*GfxWriteLog( LOG_TAG_CLIENT_CRITICAL , LOG_SWITCH_CLIENT_CRITICAL ,
					"release pl %d to chunk %d" , m_vPLID[i] , m_iChunkID );*/
			}
		}
	}
	m_bLoadTerrain = false;
	return true;
}

//------------------------------------------------------
bool SE_SceneWorldChunk::UnLoadTerrain()
{
	_unloadTerrainGeometryData();
	_unloadTerrainMaterial();
	return true;
}
//------------------------------------------------------
bool SE_SceneWorldChunk::LoadModels()
{
	//对WorldTile的访问必须都在后台线程里面，否则会有代码重入造成的随机崩溃
	T_ASSERT(!m_bLoadModel && "Reload Model");
	MTF_ClientTile* pkWorldTileFile = m_pkWorldTileParent->GetWorldTileFile();
	if(!pkWorldTileFile)
	{
		return false;
	}

	int iModelCount = pkWorldTileFile->GetChunkModelNum(m_iChunkTileIndexX,m_iChunkTileIndexY);
	for(int iModelIndex = 0; iModelIndex < iModelCount; ++iModelIndex)
	{
		ModelInfo kModelInfo;
		NiTexture::FormatPrefs kFormatePrefs;
		kFormatePrefs.m_eMipMapped = NiTexture::FormatPrefs::NO;

		StringType kResName = pkWorldTileFile->GetChunkModelTemplateName(m_iChunkTileIndexX, m_iChunkTileIndexY, iModelIndex);
		if ( !g_settings.LOD_LOAD_SCENE_FX  )
		{
			StringType::size_type index = kResName.rfind(SCENE_MODEL_GFX);
			StringType::size_type index2 = kResName.rfind(SCENE_MODEL_LIGHT_T);

			if ( index != StringType::npos && index2 == StringType::npos )
				continue;
		}
		kModelInfo.kResName = kResName;
		kModelInfo.iIndex = pkWorldTileFile->GetChunkModelTemplateID(m_iChunkTileIndexX, m_iChunkTileIndexY, iModelIndex);

		kModelInfo.kPos = pkWorldTileFile->GetChunkModelTranslate(m_iChunkTileIndexX,m_iChunkTileIndexY,iModelIndex);
		kModelInfo.kRot = pkWorldTileFile->GetChunkModelRotate(m_iChunkTileIndexX, m_iChunkTileIndexY, iModelIndex);
		kModelInfo.fScale = pkWorldTileFile->GetChunkModelScale(m_iChunkTileIndexX, m_iChunkTileIndexY, iModelIndex);

		kModelInfo.bPick = pkWorldTileFile->GetChunkModelPick( m_iChunkTileIndexX , m_iChunkTileIndexY , iModelIndex );
		kModelInfo.bWalkable = pkWorldTileFile->GetChunkModelWalkable(m_iChunkTileIndexX , m_iChunkTileIndexY , iModelIndex );
		kModelInfo.bRenderReflect = pkWorldTileFile->GetChunkModelRenderReflect(m_iChunkTileIndexX , m_iChunkTileIndexY , iModelIndex );

        pkWorldTileFile->GetChunkModelPointLight( m_iChunkTileIndexX, m_iChunkTileIndexY, iModelIndex , kModelInfo.kPLID );

		int nSize = pkWorldTileFile->GetChunkModelLightMapSize( m_iChunkTileIndexX, m_iChunkTileIndexY, iModelIndex );
		int nDataSize = 0;
		byte* pbPixelData = pkWorldTileFile->GetChunkModelLightMapPixelData( m_iChunkTileIndexX, m_iChunkTileIndexY, iModelIndex , nDataSize );

		if( nDataSize > 0 && pbPixelData && g_settings.LOD_TERRAIN_BLEND_COUNT > 2 )
		{
			NiPixelFormat kFmt = NiPixelFormat::DXT1;
			if( GfxGetWorldConfig().CFG_BAKING_TYPE == TS_WorldConfig::DUNGEON_1 )
			{
				 kFmt = NiPixelFormat::DXT1;
				 NiPixelData* pkData = NiNew NiPixelData( nSize , nSize , kFmt );
				 NiMemcpy( pkData->GetPixels() , nDataSize , pbPixelData , nDataSize );

				 bool bDap = true;
				 kModelInfo.spLightMapTexList.push_back( NiSourceTexture::Create( pkData , kFormatePrefs, &bDap ) );
			}
			else if ( GfxGetWorldConfig().CFG_BAKING_TYPE == TS_WorldConfig::DUNGEON_2 )
			{
				//地宫2.0的物件光照图是DXT5压缩的
				//并且数据段中可能会包含多张贴图
				kFmt = NiPixelFormat::DXT5;
				FileSystem::MemFileStream kStream;
				kStream.LoadFromBuffer( pbPixelData , nDataSize );
				char nCount = kStream.ReadByte();
				mem::vector<int> kSizeArray;
				for ( char i = 0 ; i < nCount ; i++ )
				{
					kSizeArray.push_back( kStream.ReadInt() );
				}
				for ( char i = 0 ; i < nCount ; i++ )
				{
					nSize = (int)sqrtf( kSizeArray[i] );
					NiPixelData* pkData = NiNew NiPixelData( nSize , nSize , kFmt );
					kStream.ReadVoid( pkData->GetPixels() , kSizeArray[i] );

					bool bDap = true;
					kModelInfo.spLightMapTexList.push_back( NiSourceTexture::Create( pkData , kFormatePrefs, &bDap ) );
				}
			}
			else
			{
				T_ASSERT(false);
			}
		}
		else
		{
			//kModelInfo.spLightMapTex = NULL;
		}

		m_kChunkModelInfo.push_back( kModelInfo );
	}
	return true;
}

void SE_SceneWorldChunk::RequestModels()
{
	T_ASSERT(!m_bLoadModel && "Reload Model");
	T_ASSERT( GfxIsMainThread() );
	MTF_ClientTile* pkWorldTileFile = m_pkWorldTileParent->GetWorldTileFile();
	if(!pkWorldTileFile)
	{
		return ;
	}
	_sendModelLoadingMsg(pkWorldTileFile);
	m_bLoadModel = true;
}

bool SE_SceneWorldChunk::PreUnLoadModels()
{
	//发送ReleaseAsyncModel的请求
	T_ASSERT( GfxIsMainThread() );
	SE_SceneModelVectorIter iter;
	TRAV( m_kChunkModels , iter )
	{
		GD_ClientAsyncModelManager::GetSingleton().ReleaseModel( *iter );
		/*GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING , 
			"Chunk %d Request UnLoad Model %s" , m_iChunkID , (*iter)->GetResourceName().c_str() );*/
	}
	m_kChunkModels.clear();
	m_bLoadModel = false;
	return true;
}
//------------------------------------------------------
bool SE_SceneWorldChunk::LoadGrass()
{
	T_ASSERT(!m_bLoadGrass && "Reload Grass");
	MTF_ClientTile* pkWorldTileFile = m_pkWorldTileParent->GetWorldTileFile();
	if(!pkWorldTileFile)
		return false;

	_loadGrassData(pkWorldTileFile);
	
	return true;
}
bool SE_SceneWorldChunk::PostLoadGrass()
{
	//在主线程中修改是否渲染草的标记位，不需要加锁
	m_bLoadingGrass = false;
	m_bLoadGrass = true;
	return true;
}
bool SE_SceneWorldChunk::PreUnLoadGrass()
{
	//在主线程中修改是否渲染草的标记位，不需要加锁
	m_bLoadingGrass = true;
	m_bLoadGrass = false;
	return true;
}
//------------------------------------------------------
bool SE_SceneWorldChunk::UnLoadGrass()
{
	_unloadGrassData();
	return true;
}
//------------------------------------------------------
unsigned int SE_SceneWorldChunk::PerformGrassInstanceRendering( NiInstanceTriShapePtr spInstanceTriShape,NiDX9Renderer* pkInstanceRenderer )
{
	if( !m_bLoadingGrass && m_bLoadGrass && m_pkGrassCluster && m_bShowGrass )
	{
		return m_pkGrassCluster->PerformInstanceRendering(spInstanceTriShape,pkInstanceRenderer);
	}
	else
	{
		return 0;
	}
}	
//------------------------------------------------------
bool SE_SceneWorldChunk::LoadWater()
{
	T_ASSERT(!m_bLoadWater && "Reload Water");

	if(m_bLoadWater)
	{
		return false;
	}
	MTF_ClientTile* pkWorldTileFile = m_pkWorldTileParent->GetWorldTileFile();
	if(!pkWorldTileFile)
	{
		return false;
	}
	_loadWaterData(pkWorldTileFile);

	m_spNiWaterRootNode->UpdateEffects();
	m_spNiWaterRootNode->UpdateProperties();
	m_spNiWaterRootNode->Update(0.0f);

	return true;
}
//------------------------------------------------------
bool SE_SceneWorldChunk::PostLoadWater()
{
	SE_SceneManager* pkSceneMgr = SE_SceneManager::GetSingletonPtr();
	pkSceneMgr->AttachToLightEffect(m_spNiWaterRootNode,false);
	//pkSceneMgr->AddUnShadowReceiveNiNode(m_spNiWaterRootNode);
	m_spNiLoadedWater->AttachChild( m_spNiWaterRootNode );
	m_spNiLoadedWater->Update( 0.0f );
	m_bLoadWater = true;
	return true;
}
//------------------------------------------------------
bool SE_SceneWorldChunk::PreUnLoadWater()
{
	SE_SceneManager::GetSingletonPtr()->DetachToLightEffect(m_spNiWaterRootNode,false);
	//SE_SceneManager::GetSingletonPtr()->RemoveUnShadowReceiveNiNode(m_spNiWaterRootNode);
	m_spNiLoadedWater->DetachChild( m_spNiWaterRootNode );
	T_ASSERT(m_bLoadWater && "UnLoad Empty Water");
	m_bLoadWater = false;
	return true;
}
//------------------------------------------------------
bool SE_SceneWorldChunk::UnLoadWater()
{
	_unloadWaterData();
	return true;
}
//------------------------------------------------------
void SE_SceneWorldChunk::_event_TimerTick( MG_MessageBase& rkMessage )
{
	float fCurrentTime = rkMessage.m_fParam1;
	//当Chunk距离玩家比较远的时候就不更新草了
	//可以大大降低Update的数量
	NiPoint3 kCenter = m_spNiTerrainRootNode->GetWorldTranslate();
	kCenter.x += 800;
	kCenter.y += 800;
	kCenter.z = 0;
	NiPoint3 kDiff = kCenter - m_kCurrentPlayerPos;
	m_fPlayerDistance = kDiff.Length();
	NiPoint3 kRange = NiPoint3(1000.0f , 1000.0f , 0.0f );
	//TS_BoundingVolume kPlayerBox( m_kCurrentPlayerPos - kRange ,
	//	m_kCurrentPlayerPos + kRange );
	//kPlayerBox.Update();
	//if( !m_kWorldBound.AABB_intersect( kPlayerBox ) )
	if ( m_fPlayerDistance > g_settings.LOD_SCENE_MODEL_RENDER_DISTANCE )
	{
		m_bShowGrass = false;
	}
	else
	{
		m_bShowGrass = true;
	}
}
//------------------------------------------------------
void SE_SceneWorldChunk::_event_LogicPlayerMove( MG_MessageBase& rkMessage )
{
	float fPlayerLocationX = rkMessage.m_fParam1;
	float fPlayerLocationY = rkMessage.m_fParam2;
	m_kCurrentPlayerPos.x = fPlayerLocationX;
	m_kCurrentPlayerPos.y = fPlayerLocationY;
	//m_kCurrentPlayerPos.z = rkMessage.m_fParam3;
	if(!m_pkGrassCluster)
		return;
	if(!Utility::UT_MathBase::TestPointInRect((int)fPlayerLocationX,(int)fPlayerLocationY,m_kAreaRect))
		return;
	float fGrassGridScale = GfxGetWorldConfig().CFG_WORLD_CHUNK_WIDTH / GfxGetWorldConfig().CFG_PLANTMAPSIZE;

	int iGrassPlanIndexX = (int)((fPlayerLocationX - m_kAreaRect.iMinX) / fGrassGridScale);
	int iGrassPlanIndexY = (int)((fPlayerLocationY - m_kAreaRect.iMinY) / fGrassGridScale);

	for(int iIndexX = iGrassPlanIndexX - 2; iIndexX < iGrassPlanIndexX + 2; iIndexX++)
	{
		for(int iIndexY = iGrassPlanIndexY - 2; iIndexY < iGrassPlanIndexY + 2; iIndexY++)
		{
			T_PROFILE_EX( "SE_SceneWorldChunk::DistortChunkPlant",3.0f );
			m_pkGrassCluster->DistortChunkPlant(iIndexX,iIndexY);
		}
	}
	if ( !m_bLoadingGrass )
	{
		T_PROFILE_EX( "SE_SceneWorldChunk::RefreshChunkPlants",3.0f );
		m_pkGrassCluster->RefreshChunkPlantsClient( true );
	}
}	
//------------------------------------------------------
void SE_SceneWorldChunk::_loadTerrainGeometryData( MTF_ClientTile* pkWorldTileFile )
{
	SE_WorldChunkGeometryDataManager* pkChunkGeometryDataManager = SE_WorldChunkGeometryDataManager::GetSingletonPtr();
	m_spChunkTerrainGeometryData = NiDynamicCast(NiTriStripsData,
		pkChunkGeometryDataManager->RequestFreeChunkGeometryData());

	float* fChunkHeightData = pkWorldTileFile->GetChunkHeights(m_iChunkTileIndexX,m_iChunkTileIndexY);
	NiPoint3* pkChunkNormalData = pkWorldTileFile->GetChunkNormals(m_iChunkTileIndexX,m_iChunkTileIndexY);
	if ( fChunkHeightData == NULL || pkChunkNormalData == NULL )
	{
		GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR ,
			"Terrain Geometry Data Loading Error!!!Chunk ID is %d %d" , 
			m_iChunkTileIndexX , m_iChunkTileIndexY );
		return;
	}

#ifdef _WOW_MESH
	NiBound& rkBound = m_spChunkTerrainGeometryData->GetBound();

	NiPoint3* pkNiVertices = m_spChunkTerrainGeometryData->GetVertices();
	NiPoint3* pkNiNormals = m_spChunkTerrainGeometryData->GetNormals();

	NiPoint3* pkTemp;

	float* pfData;

	int nWidth = Terrain::CHUNK_VERTEX_COUNT + 1;

	for(int y = 0; y < (Terrain::CHUNK_VERTEX_COUNT + 1); ++y)
	{
		int nCol =  (y % 2) ? WMI_CHUNK_IN_VECTOR_NUM : WMI_CHUNK_OUT_VECTOR_NUM;
		int nDelta =  (y % 2) ? 1 : 0;
		for(int x = 0; x < nCol; ++x)
		{

			pkTemp = pkNiVertices + indexMapBuf(x,y);
			pfData = fChunkHeightData + y * nWidth + ( x*2 + nDelta );
			pkTemp->z =  (*pfData);

			pkTemp = pkNiNormals + indexMapBuf(x,y);
			*pkTemp = *(pkChunkNormalData + y * nWidth + ( x*2 + nDelta)); 

		}
	}

	rkBound.ComputeFromData(WMI_CHUNK_TOTAL_VECTOR,pkNiVertices);

#else

	//初始化高度图数据，提高地形高度查询效率
	m_pfHeights = fChunkHeightData;
	m_pkNormal = pkChunkNormalData;

	int       iVerticesCount = m_spChunkTerrainGeometryData->GetVertexCount();
	T_ASSERT(iVerticesCount == m_iChunkVertexCount && "Chunk Data Error");

	int       iVertexIndex;
	NiBound& rkBound = m_spChunkTerrainGeometryData->GetBound();

	/* 
	reset chunk height. The chunk vertex's x,y position is aready
	set in the Dynamic Geometry Data 
	*/
	{
		NiPoint3* pkNiVertices = m_spChunkTerrainGeometryData->GetVertices();
		{
			for(iVertexIndex = 0; iVertexIndex < m_iChunkVertexCount; iVertexIndex++)	// memcpy
				pkNiVertices[iVertexIndex].z = fChunkHeightData[iVertexIndex];
		}

		//recomputer bound  
		//TODO : Need optimal
		rkBound.ComputeFromData(m_iChunkVertexCount,pkNiVertices);
	}
	//reset normal 
	{
		NiPoint3* pkNiNormals = m_spChunkTerrainGeometryData->GetNormals();
		{
			for(iVertexIndex = 0; iVertexIndex < m_iChunkVertexCount; iVertexIndex++)
				pkNiNormals[iVertexIndex] = pkChunkNormalData[iVertexIndex];
		}
	}
#endif
	m_spChunkTerrainGeometryData->MarkAsChanged(
		NiGeometryData::VERTEX_MASK | 
		NiGeometryData::NORMAL_MASK );

	m_spChunkTerrainGeometry = NiNew NiTriStrips(m_spChunkTerrainGeometryData);

	NiStencilPropertyPtr pkProp = NiNew NiStencilProperty;
	pkProp->SetDrawMode(NiStencilProperty::DRAW_CW);
	m_spChunkTerrainGeometry->AttachProperty(pkProp);

	m_spNiTerrainRootNode->AttachChild(m_spChunkTerrainGeometry);

	m_pfWalkableHeights = pkWorldTileFile->GetChunkWalkableHeights(m_iChunkTileIndexX,m_iChunkTileIndexY);
	m_pkWalkableNormal = pkWorldTileFile->GetChunkWalkableNormals(m_iChunkTileIndexX,m_iChunkTileIndexY);
	if ( m_pfWalkableHeights == NULL || m_pkWalkableNormal == NULL )
	{
		GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR , 
			"Terrain Walkable Data Loading Error!!!Chunk ID is %d %d" , 
			m_iChunkTileIndexX , m_iChunkTileIndexY );
		return;
	}

#ifdef _ENABLE_PHYSX
	if(g_settings.PHYSX_SWITCH)
	{
		T_ASSERT( m_pkPhysXStream == NULL );  
		bool bResult = pkWorldTileFile->GetPhysXStream( m_iChunkTileIndexX , m_iChunkTileIndexY , m_pkPhysXStream  );
		if( !bResult )
		{
			GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING ,
				"无法加载%d %d物理数据" , m_iChunkTileIndexX , m_iChunkTileIndexY );
		}
	}
	m_pkPhyxActor = NULL;
#endif
}

//------------------------------------------------------
void SE_SceneWorldChunk::_loadTerrainMaterial( MTF_ClientTile* pkWorldTileFile )
{
	T_ASSERT(!m_pkTerrainMaterial[0] && "mem leak");
	T_ASSERT(!m_pkTerrainMaterial[1] && "mem leak");
	T_ASSERT(!m_pkTerrainMaterial[2] && "mem leak");
	T_ASSERT(!m_pkTerrainMaterial[3] && "mem leak");

	GD_MaterialManager* pkMaterialManager = GD_MaterialManager::GetSingletonPtr();
	if(!pkMaterialManager)
		return;
	NiShaderPtr spTerrainShader;
	if ( GfxGetWorldConfig().CFG_BAKING_TYPE != TS_WorldConfig::NORMAL )
	{
		spTerrainShader = GfxGetWorldConfig().GetTerrainIndoorShader();
	}
	else
	{
		spTerrainShader = GfxGetWorldConfig().GetTerrainShader();
	}

	if(!spTerrainShader)
		return;
	if ( !m_spChunkTerrainGeometry)
	{
		return;
	}
	if( g_settings.LOD_SCENE_MIPMAP_SKIPLEVEL > 0 )
	{
		if(NiDX9Renderer::GetRenderer())
			NiDX9Renderer::GetRenderer()->SetMipmapSkipLevel( g_settings.LOD_SCENE_MIPMAP_SKIPLEVEL );

		NiSourceTexture::ms_bForceNotLoadToRendererHint = true;
	}

	m_spChunkTerrainGeometry->SetShader(spTerrainShader);

	NiTexturingPropertyPtr pkTextureProp = NiNew NiTexturingProperty;
	m_spChunkTerrainGeometry->AttachProperty(pkTextureProp);

	int nTerrainBlendNum = g_settings.LOD_TERRAIN_BLEND_COUNT;

	int iShadowMapSize = pkWorldTileFile->GetChunkLightMapSize( m_iChunkTileIndexX , m_iChunkTileIndexY );
	int iShadowMapWidth = (int)(sqrt((float)(iShadowMapSize / 4)));
	unsigned char* pSrc = pkWorldTileFile->GetChunkLightMapData( m_iChunkTileIndexX , m_iChunkTileIndexY );

	GfxWriteLog( LOG_BACKLOADING_FILELINE , LOG_SWITCH_BACKLOADING ,
		"store [%d,%d] lightmap pointer 0x%x from tile 0x%x" , 
		m_iVisibleFieldIndexX , m_iVisibleFieldIndexY ,
		pSrc , pkWorldTileFile );

	m_iVisibilityDataWidth = iShadowMapWidth;
	m_pucVisibilityData = pSrc;

	if ( g_settings.LOD_TERRAIN_SHADER == 0 )
	{
		NiSourceTexturePtr pkBlendTex = NULL;

		if ( g_settings.LOD_USE_CPU_BLEND_TERRAIN )
		{
			/*SE_WorldChunkGeometryDataManager* pkChunkGeometryDataManager = SE_WorldChunkGeometryDataManager::GetSingletonPtr();
			pkBlendTex = pkChunkGeometryDataManager->GetBlendedImage( m_iChunkID , 
				pkWorldTileFile ,m_pkWorldTileParent->GetTileIndexX(),
				m_pkWorldTileParent->GetTileIndexY(), m_iChunkTileIndexX , m_iChunkTileIndexY );*/
		}
		else
		{
//			紫色贴图相关，后续的NiSourceTexture::Create最后一个参数是true，表示完整路径，不需要SetPlatformSpecificSubdirectory，以免影响其它线程
//			NiImageConverter::SetPlatformSpecificSubdirectory("");

			NiTexture::FormatPrefs kFormatePres;
			bool bMipMapped = false;
			kFormatePres.m_eMipMapped = bMipMapped ? NiTexture::FormatPrefs::YES : NiTexture::FormatPrefs::NO;

			NiSourceTexture::SetUsePreloading(true);
			bool bDap = true;

			if ( !m_strPreBlendTexNameDecoration.empty() )
			{
				bool bExists = NiFile::Access( m_strPreBlendTexNameDecoration.c_str() , 
					NiFile::READ_ONLY );
				if ( bExists )
				{
					pkBlendTex = NiSourceTexture::Create(
						m_strPreBlendTexNameDecoration.c_str(), 
						kFormatePres, true , false , true, &bDap); 
				}
			}
			if ( pkBlendTex == NULL )
			{
				pkBlendTex = NiSourceTexture::Create(
					m_strPreBlendTexName.c_str(), 
					kFormatePres, true , false , true, &bDap); 
			}
		}
		NiTexturingProperty::ShaderMap* pkShaderMap0 = NiNew NiTexturingProperty::ShaderMap;
		pkShaderMap0->SetTexture( pkBlendTex );
		pkTextureProp->SetShaderMap(0,pkShaderMap0);

		if ( GfxGetWorldConfig().CFG_BAKING_TYPE != TS_WorldConfig::NORMAL )
		{
			bool bDap = true;
			{
				NiPixelData* pkData = NiNew NiPixelData(iShadowMapWidth, iShadowMapWidth,NiPixelFormat::RGBA32/**ppkPixFmts[NiDX9Renderer::TEX_RGBA8888]*/);
				NiMemcpy( pkData->GetPixels() , iShadowMapSize , pSrc , iShadowMapSize );

				NiTexture::FormatPrefs kFormatePrefs;
				kFormatePrefs.m_ePixelLayout = NiTexture::FormatPrefs::TRUE_COLOR_32;
				kFormatePrefs.m_eMipMapped = NiTexture::FormatPrefs::NO;

				NiSourceTexture* pkAlphaTexture = NiSourceTexture::Create(pkData, kFormatePrefs, &bDap);

				NiTexturingProperty::ShaderMap* pkShaderMap5 = NiNew NiTexturingProperty::ShaderMap;
				pkShaderMap5->SetTexture(pkAlphaTexture);

				pkTextureProp->SetShaderMap(5,pkShaderMap5);
			}
		}
	}
	else
	{
		//layer map
		{
			StringType kMaterialName_0 = pkWorldTileFile->GetChunkTextureLayer(m_iChunkTileIndexX,m_iChunkTileIndexY,0);
			StringType kMaterialName_1;
			StringType kMaterialName_2;
			StringType kMaterialName_3;

			m_pkTerrainMaterial[0] = (GD_Material*)pkMaterialManager->CreateMaterial(
				kMaterialName_0.c_str(),GD_MaterialManager::MT_TILE_SET);
			NiTexturingProperty::ShaderMap* pkShaderMap0 = NiNew NiTexturingProperty::ShaderMap;
			if ( m_pkTerrainMaterial[0] )
			{
				pkShaderMap0->SetTexture(m_pkTerrainMaterial[0]->GetNiTexture());
			}
			pkTextureProp->SetShaderMap(0,pkShaderMap0);


			if ( nTerrainBlendNum > 1 )
			{
				kMaterialName_1 = pkWorldTileFile->GetChunkTextureLayer(m_iChunkTileIndexX,m_iChunkTileIndexY,1);
				m_pkTerrainMaterial[1] = (GD_Material*)pkMaterialManager->CreateMaterial(kMaterialName_1.c_str(),GD_MaterialManager::MT_TILE_SET);
				NiTexturingProperty::ShaderMap* pkShaderMap1 = NiNew NiTexturingProperty::ShaderMap;
				if ( m_pkTerrainMaterial[1] )
				{
					pkShaderMap1->SetTexture(m_pkTerrainMaterial[1]->GetNiTexture());
				}
				pkTextureProp->SetShaderMap(1,pkShaderMap1);
			}
			else
			{
				m_pkTerrainMaterial[1] = NULL;
			}

			if ( nTerrainBlendNum > 2 )
			{
				kMaterialName_2 = pkWorldTileFile->GetChunkTextureLayer(m_iChunkTileIndexX,m_iChunkTileIndexY,2);
				m_pkTerrainMaterial[2] = (GD_Material*)pkMaterialManager->CreateMaterial(kMaterialName_2.c_str(),GD_MaterialManager::MT_TILE_SET);
				NiTexturingProperty::ShaderMap* pkShaderMap2 = NiNew NiTexturingProperty::ShaderMap;
				if ( m_pkTerrainMaterial[2] )
				{
					pkShaderMap2->SetTexture(m_pkTerrainMaterial[2]->GetNiTexture());
				}
				pkTextureProp->SetShaderMap(2,pkShaderMap2);
			}
			else
			{
				m_pkTerrainMaterial[2] = NULL;
			}

			if ( nTerrainBlendNum > 3 )
			{
				kMaterialName_3 = pkWorldTileFile->GetChunkTextureLayer(m_iChunkTileIndexX,m_iChunkTileIndexY,3);

				m_pkTerrainMaterial[3] = (GD_Material*)pkMaterialManager->CreateMaterial(kMaterialName_3.c_str(),GD_MaterialManager::MT_TILE_SET);
				NiTexturingProperty::ShaderMap* pkShaderMap3 = NiNew NiTexturingProperty::ShaderMap;
				if ( m_pkTerrainMaterial[3] )
				{
					pkShaderMap3->SetTexture(m_pkTerrainMaterial[3]->GetNiTexture());
				}
				pkTextureProp->SetShaderMap(3,pkShaderMap3);
			}
			else
			{
				m_pkTerrainMaterial[3] = NULL;
			}
		}

		NiPixelFormat** ppkPixFmts = NiDX9Renderer::GetRenderer()->GetTextureFormatArray(NiDX9Renderer::TEXUSE_TEX);
		//alpha map
		NiTexture::FormatPrefs kFormatePrefs;
		kFormatePrefs.m_ePixelLayout = NiTexture::FormatPrefs::TRUE_COLOR_32;
		kFormatePrefs.m_eMipMapped = NiTexture::FormatPrefs::NO;
		NiSourceTexturePtr spAlphaTex = NULL;
		if ( nTerrainBlendNum > 1 )
		{
			int iAlphaMapSize = pkWorldTileFile->GetChunkAlphaMapSize( m_iChunkTileIndexX , m_iChunkTileIndexY );
			int iAlphaMapWidth = (int)(sqrt((float)(iAlphaMapSize / 4)));
			unsigned char* pSrc = pkWorldTileFile->GetChunkAlphaMapData( m_iChunkTileIndexX , m_iChunkTileIndexY );
			NiPixelData* pkData = NiNew NiPixelData(iAlphaMapWidth, iAlphaMapWidth,NiPixelFormat::RGBA32/**ppkPixFmts[NiDX9Renderer::TEX_RGBA8888]*/);
			NiMemcpy( pkData->GetPixels() , iAlphaMapSize , pSrc , iAlphaMapSize );

			bool bDap = true;
			NiSourceTexture* pkAlphaTexture = NiSourceTexture::Create(pkData,kFormatePrefs, &bDap);

			NiTexturingProperty::ShaderMap* pkShaderMap4 = NiNew NiTexturingProperty::ShaderMap;
			pkShaderMap4->SetTexture(pkAlphaTexture);

			pkTextureProp->SetShaderMap(4,pkShaderMap4);
			spAlphaTex = pkAlphaTexture;
		}

		//light map
		if ( nTerrainBlendNum > 1 )
		{
			bool bDap = true;
			{
				NiPixelData* pkData = NiNew NiPixelData(iShadowMapWidth, iShadowMapWidth,NiPixelFormat::RGBA32/**ppkPixFmts[NiDX9Renderer::TEX_RGBA8888]*/);
				NiMemcpy( pkData->GetPixels() , iShadowMapSize , pSrc , iShadowMapSize );

				NiTexture::FormatPrefs kFormatePrefs;
				kFormatePrefs.m_ePixelLayout = NiTexture::FormatPrefs::TRUE_COLOR_32;
				kFormatePrefs.m_eMipMapped = NiTexture::FormatPrefs::NO;

				NiSourceTexture* pkAlphaTexture = NiSourceTexture::Create(pkData, kFormatePrefs, &bDap);

				NiTexturingProperty::ShaderMap* pkShaderMap5 = NiNew NiTexturingProperty::ShaderMap;
				pkShaderMap5->SetTexture(pkAlphaTexture);

				pkTextureProp->SetShaderMap(5,pkShaderMap5);
			}
		}
	}


	//Point Light
	if ( g_settings.LOD_TERRAIN_SHADER > 0 ||GfxGetWorldConfig().CFG_BAKING_TYPE != TS_WorldConfig::NORMAL )
	{
		pkWorldTileFile->GetChunkPLID( m_iChunkTileIndexX , m_iChunkTileIndexY , m_vPLID );
	}

	if( g_settings.LOD_SCENE_MIPMAP_SKIPLEVEL > 0 )
	{
		if(NiDX9Renderer::GetRenderer())
			NiDX9Renderer::GetRenderer()->SetMipmapSkipLevel( 0 );
		NiSourceTexture::ms_bForceNotLoadToRendererHint = false;
	}
}
//------------------------------------------------------
void SE_SceneWorldChunk::_unloadTerrainGeometryData()
{
	//释放物理数据
#ifdef _ENABLE_PHYSX 
 	T_SAFE_DELETE( m_pkPhysXStream );
#endif
	if ( m_spChunkTerrainGeometryData )
	{
		SE_WorldChunkGeometryDataManager* pkChunkGeometryDataManager = SE_WorldChunkGeometryDataManager::GetSingletonPtr();
		pkChunkGeometryDataManager->RegisterFreeChunkGeometryData(m_spChunkTerrainGeometryData);
	}
	
	m_spChunkTerrainGeometryData = NULL;
	m_spChunkTerrainGeometry = NULL;
}
//------------------------------------------------------
void SE_SceneWorldChunk::_unloadTerrainMaterial()
{
	GD_MaterialManager* pkMaterialManager = GD_MaterialManager::GetSingletonPtr();
	if(!pkMaterialManager)
		return;

	for(int iMaterialIndex = 0; iMaterialIndex < 4; iMaterialIndex++)
	{
		if(m_pkTerrainMaterial[iMaterialIndex])
		{
			pkMaterialManager->ReleaseMaterial(m_pkTerrainMaterial[iMaterialIndex]);
			m_pkTerrainMaterial[iMaterialIndex]	= NULL;
		}
	}
}
//------------------------------------------------------
void SE_SceneWorldChunk::_sendModelLoadingMsg( MTF_ClientTile* pkWorldTileFile )
{
	// 在主线程调用，发送Model加载请求
	if ( !SE_SceneManager::GetSingleton().IsModelShow() )
	{
		return;
	}
	//如果这个时候后台线程已经退出了，就不创建模型了。否则会产生新的后台任务
	if ( BL_AsyncOperationManager::GetSingletonPtr() && 
		 !BL_AsyncOperationManager::GetSingleton().IsLoadingThreadAlive() )
	{
		m_kChunkModelInfo.clear();
		return;
	}
	ChunkModelInfo::iterator iter;
	for( iter = m_kChunkModelInfo.begin() ; iter != m_kChunkModelInfo.end() ; ++iter )
	{
		ModelInfo& kModelInfo = *iter;

		GD_ModelBase* pkAsyncModel = NULL;
		pkAsyncModel = GD_ClientAsyncModelManager::GetSingleton().CreateModel(
			GD_ClientAsyncModelManager::AMT_SCENE,kModelInfo.kResName,kModelInfo.iIndex );

		GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING , 
			"Map:%s,Chunk %d Request Load Model: %s	Pos: %f %f %f" , m_pkWorldMapParent->m_kWorldFileName.c_str(), m_iChunkID , kModelInfo.kResName.c_str() 
			, kModelInfo.kPos.x , kModelInfo.kPos.y , kModelInfo.kPos.z );

        if (pkAsyncModel != NULL)
        {
            pkAsyncModel->SetTranslate(kModelInfo.kPos);
            pkAsyncModel->SetRotate(kModelInfo.kRot);
            pkAsyncModel->SetScale(kModelInfo.fScale );
			pkAsyncModel->UpdateModelContent();
            GD_ClientSceneModel* pkSceneModel = (GD_ClientSceneModel*)pkAsyncModel;
            pkSceneModel->SetExtraModelInfo( kModelInfo );
            m_kChunkModels.push_back( pkSceneModel );
        }
	}
	m_kChunkModelInfo.clear();
}
//------------------------------------------------------
void SE_SceneWorldChunk::_loadGrassData(MTF_ClientTile* pkWorldTileFile)
{
	size_t nGrassSize = pkWorldTileFile->GetChunkGrassStreamSize( m_iChunkTileIndexX , m_iChunkTileIndexY );

	if( nGrassSize > 0 &&  g_settings.LOD_ENABLE_GRASS_RENDER )
	{
		float fStartX = (float)m_kAreaRect.iMinX;
		float fStartY = (float)m_kAreaRect.iMinY;
		m_pkGrassCluster = T_NEW_D Terrain::TS_GrassCluster(fStartX,fStartY,true);
		bool bResult = m_pkGrassCluster->LoadBaked( 
			nGrassSize ,
			pkWorldTileFile->GetChunkGrassStreamData(m_iChunkTileIndexX,m_iChunkTileIndexY) );
		if ( !bResult )
		{
			T_SAFE_DELETE( m_pkGrassCluster );
		}
	}
}
//------------------------------------------------------
void SE_SceneWorldChunk::_unloadGrassData()
{
	if(m_pkGrassCluster)
	{
		T_DELETE_D m_pkGrassCluster;
		m_pkGrassCluster = NULL;
	}
}
//------------------------------------------------------
void SE_SceneWorldChunk::_loadWaterData( MTF_ClientTile* pkWorldTileFile )
{
	BS_ClientWaterMeshManager* pkWaterMeshManager = m_pkWorldMapParent->GetWaterMeshManager();
	if ( pkWaterMeshManager == NULL )
	{
		T_ASSERT(false);
		return;
	}
	mem::vector<int> kWaterTypes;
	pkWorldTileFile->GetChunkWaterTypes( m_iChunkTileIndexX , m_iChunkTileIndexY , kWaterTypes );

	if ( kWaterTypes.size() > 0 )
	{
		for(int iWaterIndex = 0; iWaterIndex < (int)kWaterTypes.size(); iWaterIndex++)
		{
			int iWaterType = kWaterTypes[iWaterIndex];
			WaterParam* pkParams = pkWaterMeshManager->GetWaterTypeParam( iWaterType);
			if (pkParams == NULL)
				continue;
			NiStream kStream;
			pkWorldTileFile->GetChunkWaterStream(  m_iChunkTileIndexX , m_iChunkTileIndexY  , iWaterIndex , kStream );
			if ( kStream.GetObjectCount() == 0 )
			{
				GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR , 
					"Tile %d %d Chunk %d %d Water Baked Data Loading Failed!" ,
					m_pkWorldTileParent->GetTileIndexX(),
					m_pkWorldTileParent->GetTileIndexY(), 
					m_iChunkTileIndexX , m_iChunkTileIndexY );
				continue;
			}
			TS_WaterMeshBase* pkWaterMesh = pkWaterMeshManager->RequestFreeWaterMesh();

			pkWaterMesh->InitWaterParams( *pkParams );
			bool bResult = pkWaterMesh->LoadBaked( kStream );	
			if ( !bResult )
			{
				pkWaterMeshManager->RegisterFreeWaterMesh( pkWaterMesh );
				continue;
			}

			pkWaterMesh->SetReflectMap( RG_RenderManager::GetSingleton().GetWaterReflectRenderTex() );

			if ( pkWaterMesh->GetWaterHeight( false ) > m_fWaterHeight )
			{
				m_fWaterHeight = pkWaterMesh->GetWaterHeight( false );
			}
			m_kWaterMeshMap[iWaterType] = pkWaterMesh;
			m_spNiWaterRootNode->AttachChild(pkWaterMesh->GetNiTriShapePtr());
		}
	}
}
//------------------------------------------------------
void SE_SceneWorldChunk::_unloadWaterData()
{
	m_spNiWaterRootNode->RemoveAllChildren();
	BS_ClientWaterMeshManager* pkWaterMeshManager = m_pkWorldMapParent->GetWaterMeshManager();

	WaterMeshMapIter IterBe = m_kWaterMeshMap.begin();
	WaterMeshMapIter IterEd = m_kWaterMeshMap.end();

	for( ; IterBe != IterEd; ++IterBe)
	{
		TS_WaterMeshBase* pkWaterMesh = (*IterBe).second;
		pkWaterMeshManager->RegisterFreeWaterMesh(pkWaterMesh);
	}

	m_kWaterMeshMap.clear();
	m_fWaterHeight = 0.0f;
}
//------------------------------------------------------
void SE_SceneWorldChunk::_computeAreaRect()
{
	Utility::UT_AABB& rkClusterAreaRect = m_pkWorldChunkClusterParent->GetAreaRect();
	float fChunkBaseX = rkClusterAreaRect.iMinX + m_iChunkIndexX * GfxGetWorldConfig().CFG_WORLD_CHUNK_WIDTH;
	float fChunkBaseY = rkClusterAreaRect.iMinY + m_iChunkIndexY * GfxGetWorldConfig().CFG_WORLD_CHUNK_WIDTH;
	float fChunkTopX = fChunkBaseX + GfxGetWorldConfig().CFG_WORLD_CHUNK_WIDTH;
	float fChunkTopY = fChunkBaseY + GfxGetWorldConfig().CFG_WORLD_CHUNK_WIDTH;

	m_kAreaRect.iMinX = (int)fChunkBaseX;
	m_kAreaRect.iMinY = (int)fChunkBaseY;
	m_kAreaRect.iMaxX = (int)fChunkTopX;
	m_kAreaRect.iMaxY = (int)fChunkTopY;
}
//------------------------------------------------------
bool SE_SceneWorldChunk::PickMap( NiPick* pkScenePick,const NiPoint3 &origin, const NiPoint3 &dir , const NiPoint3& kPlayerPos )
{
	/// 先与Chunk做粗略检测
	float dist = 0;
	if (!m_kWorldBound.Ray_intersect(origin, dir, dist))
		return false;

	//如果Pick到可以寻路的几何片就可以直接返回了，否则还要继续Pick地形的几何数据
	//由于需要选择到最近的几何片，所以Pick成功后还不能直接返回
	SE_SceneModelVectorIter iter;
	bool bResult = false;

	/// pick 物件
	TRAV( m_kChunkModels , iter )
	{
		GD_ClientSceneModel* pkModel = *iter;
		if( pkModel->IsPickableModel() )
		{
			bool bValid = pkModel->SetTargetForPathPick( pkScenePick );
			if ( !bValid )
			{
				continue;
			}
			bResult = pkScenePick->PickObjects(origin,dir,true);
			pkScenePick->SetTarget(NULL);

			/*if ( bResult )
			{
				NiPoint3 kPos = pkModel->GetTranslate();
				GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING , 
					"Pick Model %s Pos %f %f %f",
					pkModel->GetModelFileName().c_str() ,
					kPos.x , kPos.y , kPos.z );
				return true;
			}*/
		}
	}

	/// pick 地面
	pkScenePick->SetTarget( m_spNiTerrainRootNode );
	bResult = pkScenePick->PickObjects(origin,dir,true);
	pkScenePick->SetTarget(NULL);

	return false;
}	

void SE_SceneWorldChunk::RegisterLocalNPC(long lSpawnID)
{
	mem::vector<long>::iterator iter = std::find(m_kLocalNPCList.begin(), m_kLocalNPCList.end(), lSpawnID);
	if (iter == m_kLocalNPCList.end())
	{
		m_kLocalNPCList.push_back(lSpawnID);
	}


    if (m_bLoadTerrain)
    {
        LC_ClientNPCManager* pkLocalNpcManager = CLIENT_GET_LOCAL_NPC_MANAGER();
#ifndef _MAP_VIEWER_BUILD
        long lS = pkLocalNpcManager->CreateLocalNPC(lSpawnID, "LocalNPC");
#else
        long lS = pkLocalNpcManager->CreateLocalNPC(lSpawnID, "LocalNPC", true);
#endif
    }
}

bool SE_SceneWorldChunk::UpdateCameraBlockModels( NiPick* pkPick , const Terrain::TS_BoundingVolume& rkPlayerAABB,const NiPoint3& kOrigin,const NiPoint3& kDir )
{
	if ( !m_bLoadModel )
	{
		return false;
	}

	/// 优先与整个Chunk做碰撞，快速过滤
	if (!m_kWorldBound.AABB_intersect(rkPlayerAABB))
		return false;

	UT_TimeManager* pkTimeManager = UT_TimeManager::GetSingletonPtr();
	float fDeltaTime = pkTimeManager->GetDeltaTime();
	float fAlphaDelat = fDeltaTime * 1.0f;

	int iStaitcModelCount = m_kChunkModels.size();
	for(int iModelIndex = 0; iModelIndex < iStaitcModelCount; iModelIndex++)
	{
		GD_ClientSceneModel* pkWorldModel = m_kChunkModels[iModelIndex];

		//Terrain::TS_BoundingVolume& rkAABB = pkWorldModel->GetBoundingBox();

		bool bIntersect = false;
		//可以行走的就不要透明了
		if( !pkWorldModel->IsWalkableModel() && 
			 pkWorldModel->SetTargetForNiPick( pkPick ) )
		{
			if( pkPick->PickObjects( kOrigin , kDir ) )
				bIntersect = true;
		}
		if ( bIntersect )
		{
			pkWorldModel->IncrementalChangeAlpha(-fAlphaDelat,0.15f,1.0f);
			pkWorldModel->UpdateAlphaBlend(  );
		}
		else
		{
			pkWorldModel->IncrementalChangeAlpha( fAlphaDelat,0.15f,1.0f);
			pkWorldModel->UpdateAlphaBlend(  );
		}
	}
	return true;
}

void SE_SceneWorldChunk::GetModelVisibleArrary(NiCameraPtr pkNiCamera , 
											   NiCullingProcess& rkCuller , 
											   NiVisibleArray& rkVisibleArray , 
											   bool bSceneFx, bool bFirstInFrame )
{
	//底层保证后台加载未完成的模型是不会执行Cull的
	
    SE_SceneModelVectorIter iter;
	TRAV( m_kChunkModels , iter )
	{
		GD_ClientSceneModel* pkModel = (*iter);

		NiPoint3 kModelPos = pkModel->GetTranslate();
		kModelPos.z = 0.0f;

		// Trick : 对于比较大的物件，不管距离远近，都渲染
		Terrain::TS_BoundingVolume& rkAABB = pkModel->GetBoundingBox();
		NiPoint3 kMax = rkAABB.GetMax();
		NiPoint3 kMin = rkAABB.GetMin();
		NiPoint3 kR = kMax - kMin;
		float fMaxLength = kR.x > kR.y ? kR.x : kR.y;

		float fDistance = ( m_kCurrentPlayerPos - kModelPos ).Length();
        bool bPreCulled = false;

		//强制把可以行走的模型设置成不可预裁剪的
		if ( !pkModel->IsWalkableModel() )
        {
            bPreCulled = (fDistance - fMaxLength * 0.5f) > g_settings.LOD_SCENE_MODEL_RENDER_DISTANCE;
        }
        
        // Cull 之前先关闭场景模型的 update 标志
        // 避免由于 pre-cull 跳过了实际 cull 操作导致视野外的模型仍然被 update 的问题
		if ( bSceneFx && pkModel->IsSceneEffectModel() )
		{
            if (bPreCulled)
            {
                pkModel->SetIsPreCulled(true);
            }
            else
            {
                pkModel->SetIsPreCulled(false);

                pkModel->Cull( pkNiCamera , rkCuller , rkVisibleArray);
            }
		}
        else if ( !bSceneFx && !pkModel->IsSceneEffectModel() )
		{
			//bobby for cull bug
			/// 重新启用if这几行代码，之前被注释掉，原因如上一行所述；同时修改了前面 bPreCulled 的判断方法，可能不会再有 cull bug
             if (bPreCulled)
             {
                 pkModel->SetIsPreCulled(true);
             }
             else
            {
                pkModel->SetIsPreCulled(false);

				bool bWalked = false;
                if (pkModel->IsWalkableModel())
                {
                    if (bFirstInFrame)
                    {
						bWalked = true;
                        m_kWalkableModels.push_back(pkModel);
                    }
                }
                if ( pkModel->IsTransparent())
                {
                    if (bFirstInFrame)
                    {
                        m_kTransparent.push_back( pkModel );
                    }
                }
                else
				if (!bWalked)
                {
                    pkModel->Cull( pkNiCamera , rkCuller , rkVisibleArray );
                }
            }
		}
	}
}

void SE_SceneWorldChunk::GetWalkableModelVisibleArray(NiCameraPtr pkNiCamera, NiCullingProcess& rkCuller, NiVisibleArray& rkVisibleArray)
{
    if (!m_kWalkableModels.empty())
    {
        for (SE_SceneModelVectorIter itr = m_kWalkableModels.begin(); itr != m_kWalkableModels.end(); ++itr)
        {
            GD_ClientSceneModel* pkModel = (*itr);
            if (!pkModel->IsCulled())
            {
                pkModel->Cull(pkNiCamera, rkCuller, rkVisibleArray);
            }
        }
        m_kWalkableModels.clear();
    }
}

void SE_SceneWorldChunk::GetTransparentVisibleArrary( NiCameraPtr pkNiCamera, NiCullingProcess& rkCuller , NiVisibleArray& rkVisibleArray )
{
	if ( !m_kTransparent.empty() )
	{
		SE_SceneModelVectorIter iter;
		TRAV( m_kTransparent , iter )
		{
			(*iter)->Cull( pkNiCamera , rkCuller , rkVisibleArray );
		}
		m_kTransparent.clear();
	}
}

void SE_SceneWorldChunk::GetReflectVisibleArrary(NiCameraPtr pkNiCamera, NiCullingProcess& rkCuller , NiVisibleArray& rkVisibleArray)
{
	SE_SceneModelVectorIter iter;
	TRAV( m_kChunkModels , iter )
	{
		GD_ClientSceneModel* pkModel = (*iter);

		if (pkModel->IsSceneEffectModel())
			continue;

		if (!pkModel->IsRenderReflect())
			continue;

		pkModel->Cull( pkNiCamera , rkCuller , rkVisibleArray );
	}
}

