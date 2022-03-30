#include "ClientPCH.h"

#include "SE_SceneWorldMap.h"
#include "SE_SceneWorldTile.h"
#include "SE_SceneWorldChunkCluster.h"
#include "SE_SceneTerrainInfo.h"
#include "SE_SceneManager.h"
#include "MG_MessageBase.h"
#include "BS_ClientWaterMeshManager.h"
#include "CA_CameraManager.h"
#include "SE_Terraincallback.h"
#include "UT_GlobalSettings.h"
#include "AS_ClientSoundManager.h"
#include "MTF_ClientTile.h"
#include "MTF_ClientMapGlobalInfo.h"
#include "RG_LightProbe.h"

using namespace Render;
using namespace Utility;
using namespace Terrain;
using namespace FileSystem;
//----------------------------------------------------------------
SE_SceneWorldMap::SE_SceneWorldMap()
: WORLD_TILE_COUNTX_PER_MAP(GfxGetWorldConfig().CFG_WORLD_TILE_COUNTX_PER_MAP)
, WORLD_CHUNKCLUSTER_COUNTX_PER_TILE(GfxGetWorldConfig().CFG_WORLD_CHUNKCLUSTER_COUNTX_PER_TILE)
, WORLD_CHUNK_COUNTX_PER_CLUSTER(GfxGetWorldConfig().CFG_WORLD_CHUNK_COUNTX_PER_CLUSTER)
, WORLD_GRID_WIDTH(GfxGetWorldConfig().CFG_WORLD_GRID_WIDTH)
, WORLD_MAP_WIDTH(GfxGetWorldConfig().CFG_WORLD_MAP_WIDTH)
, WORLD_TILE_WIDTH(GfxGetWorldConfig().CFG_WORLD_TILE_WIDTH)

, m_spOutDoorLight(NULL)
, m_spPointLight(NULL)
, m_pkTerrainLightField(NULL)
, m_fTargetDepthFogDensity(0.0f)
, m_fTargetHeightFogDensity(0.0f)
, m_fCurrentDepthFogDensity(0.0f)
, m_fCurrentHeightFogDensity(0.0f)
, m_bDepthFogFadeOut(false)
, m_bDepthFogFadeIn(false)
, m_bHeightFogFadeIn(false)
, m_bHeightFogFadeOut(false)
, m_pkHeightField(NULL)
, m_pkWalkableField(NULL)
, m_pkShadowField(NULL)
, m_pkWaterMeshManager(NULL)
, m_pkWorldMapFile(NULL)
, m_fTransTimeLeft(g_settings.CHARACTER_LIGHT_TRANS_TIME)
, m_fFogDepthTransTimeLeft(m_fTransTimeLeft/2.0f)
, m_fFogHeightTransTimeLeft(m_fTransTimeLeft/2.0f)
, m_pkMapEffect(NULL)
, m_pkMapGlobalInfo(NULL)
, m_fTargetLightMultiplier(0.0f)
, m_fCurrentLightMultiplier(0.0f)
, m_fCurrentWaterHeight(0.0f)
, m_bCompute(false)
, m_bIsCurrentInSceneLightning(false)
, m_fCurrentSceneLightningPower(0.0f)
{
	m_kSceneWorldTileVector.clear();

	_initCharacterParams();
	_initPaticleParams();
	_initFinalEffectParams();

	m_kCurrentAmbientColor   = m_kTargetAmbientColor = NiColor::BLACK;
	m_kCurrentDiffuseColor   = m_kTargetDiffuseColor = NiColor::BLACK;
	m_kCurrentBackLightColor = m_kTargetBackLightColor = NiColor::BLACK;
	m_kCurrentShadowColor    = m_kTargetShadowColor = NiColor::BLACK;

	m_kCurrentFogColor       = m_kTargetFogColor = NiColor::BLACK;
	m_kDepthFogParam         = m_kTargetDepthFogParam = NiColorA::BLACK; 
	m_kHeightFogParam        = m_kTargetHeightFogParam = NiColorA::BLACK;
	m_bWeatherEnable = false;
	m_bBloomEnable = false;

	m_bInitParams = false;

	m_spOutDoorLight = NiNew NiDirectionalLight;
	m_spSceneFxLight = NiNew NiDirectionalLight;
	m_spPointLight   = NiNew NiPointLight;
	m_spSceneFxPointLight = NiNew NiPointLight;

	m_pkMapGlobalInfo = T_NEW_D ClientMapGlobalInfoManager;
	m_pkProbeMgr = T_NEW_D RG_LightProbeManager;
}
//----------------------------------------------------------------
SE_SceneWorldMap::~SE_SceneWorldMap()
{
	m_spSceneFxLight->AssignShadowGenerator(NULL);
	m_spSceneFxPointLight->AssignShadowGenerator( NULL );
	T_SAFE_DELETE( m_pkMapGlobalInfo );
	T_SAFE_DELETE( m_pkProbeMgr );
}
//----------------------------------------------------------------
bool SE_SceneWorldMap::RecvMessage(MG_MessageBase& rkMessage)
{
	MG_MessageHandlerInterface::RecvMessage(rkMessage);

	BEGIN_MESSAGE_HANDLE
	MESSAGE_HANDLE(MGT_LOGIC_PLAYER_MOVE,	_event_LogicPlayerMove)
	MESSAGE_HANDLE(MGT_TIMER_TICK,			_event_TimerTick)
	END_MESSAGE_HANDLE
	return true;
}
//---------------------------------------------------------------------------------
void SE_SceneWorldMap::RefreshMessageHandlerTree()
{
	T_ASSERT( GfxIsMainThread() );

	//unsigned int nActiveCount = 0;
	//unsigned int nDeactiveCount = 0;

	m_kLoadedChunks.clear();

	//GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING , 
	//	"--------------------\nBegin Refreash Active Chunks\n------------------------" );

	for (int iTileIndexX = 0; iTileIndexX < WORLD_TILE_COUNTX_PER_MAP; ++iTileIndexX)
	{
		for (int iTileIndexY = 0; iTileIndexY < WORLD_TILE_COUNTX_PER_MAP; ++iTileIndexY)
		{
			SE_SceneWorldTile* pkWorldTile = GetWorldTile(iTileIndexX, iTileIndexY);
			if (pkWorldTile == NULL)
			{
                continue;
            }

            bool bWorldTileCanSafeUnloadTileFile = true;

			for (int iClusterIndexX = 0; iClusterIndexX < WORLD_CHUNKCLUSTER_COUNTX_PER_TILE; ++iClusterIndexX)
			{
				for (int iClusterIndexY = 0; iClusterIndexY < WORLD_CHUNKCLUSTER_COUNTX_PER_TILE; ++iClusterIndexY)
				{
					SE_SceneWorldChunkCluster* pkCluster = pkWorldTile->GetWorldChunkCluster(iClusterIndexX, iClusterIndexY);
					if (pkCluster == NULL)
					{
                        continue;
                    }
					if ( pkCluster->IsLoading() )
					{
						bWorldTileCanSafeUnloadTileFile = false;
					}

                    bool bIsAnyChunkInClusterLoaded = false;

					for (int iChunkIndexX = 0; iChunkIndexX < WORLD_CHUNK_COUNTX_PER_CLUSTER; ++iChunkIndexX)
					{
						for (int iChunkIndexY = 0; iChunkIndexY < WORLD_CHUNK_COUNTX_PER_CLUSTER; ++iChunkIndexY)
						{

							SE_SceneWorldChunk* pkChunk = pkCluster->GetWorldChunk(iChunkIndexX, iChunkIndexY);
							if (pkChunk == NULL)
							{
                                continue;
							}

							if (pkChunk->IsLoaded())
							{
                                bWorldTileCanSafeUnloadTileFile = false;
                                bIsAnyChunkInClusterLoaded = true;

								pkChunk->SetActive(true);
								//nDeactiveCount++;
								m_kLoadedChunks.push_back( pkChunk );

								//GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING , 
								//	"Add Active Chunk %d" , pkChunk->GetDebugChunkID() );
							}
							else
							{
								pkChunk->SetActive(false);
								//++nActiveCount;
							}
						}
					}

                    pkCluster->SetActive(bIsAnyChunkInClusterLoaded);
				}
			}
            pkWorldTile->SetActive(!bWorldTileCanSafeUnloadTileFile);
			/*if ( bWorldTileCanSafeUnloadTileFile )
			{
				GfxWriteLog( LOG_BACKLOADING_FILELINE , LOG_SWITCH_BACKLOADING ,
					"set tile active false 0x%x in refresh func" , pkWorldTile->GetWorldTileFile() );
			}*/
		}
	}
}
//---------------------------------------------------------------------------------
void SE_SceneWorldMap::GetTerrainVisibleArray( NiCameraPtr spNiCamera, NiCullingProcess& rkCuller, NiVisibleArray& rkVisibleArray )
{
	//这里没有必要再去Cull了，因为ActiveChunk已经用包围盒Cull过一次了
	SE_WorldChunkListIter iter;
	TRAV( m_kActiveChunks , iter )
	{
		if( !(*iter)->IsLoaded() )
			continue;
		NiNodePtr spTerrainRoot = (*iter)->GetNiChunkTerrainRootNode();
		CullNiScene( spNiCamera , spTerrainRoot , rkCuller , rkVisibleArray , false );
	}
}
//---------------------------------------------------------------------------------
void SE_SceneWorldMap::GetModelVisibleArray( NiCameraPtr spNiCamera, 
											NiCullingProcess& rkCuller, 
											NiVisibleArray& rkVisibleArray, 
											bool bFirstInFrame )
{
    SE_WorldChunkList::const_iterator iter;
    TRAV( m_kActiveChunks , iter )
    {
        if( (*iter)->IsLoaded() )
        {
            (*iter)->GetModelVisibleArrary( spNiCamera , rkCuller , rkVisibleArray , false , bFirstInFrame );
        }
    }
}

void SE_SceneWorldMap::GetWalkableModelVisibleArray(NiCameraPtr spNiCamera, NiCullingProcess& rkCuller, 
                                                    NiVisibleArray& rkVisibleArray)
{
	SE_WorldChunkList::const_iterator iter;
	TRAV(m_kActiveChunks, iter)
	{
		if ((*iter)->IsLoaded())
		{
			(*iter)->GetWalkableModelVisibleArray(spNiCamera, rkCuller, rkVisibleArray);
		}
	}
}

void SE_SceneWorldMap::GetTransparentVisibleArray(NiCameraPtr spNiCamera, NiCullingProcess& rkCuller, 
                                                  NiVisibleArray& rkVisibleArray)
{
    SE_WorldChunkList::const_iterator iter;
    TRAV( m_kActiveChunks , iter )
    {
        if( (*iter)->IsLoaded() )
        {
            (*iter)->GetTransparentVisibleArrary( spNiCamera , rkCuller , 
                rkVisibleArray );
        }
    }
}

void SE_SceneWorldMap::GetSceneFxVisibleArray( NiCameraPtr spNiCamera, NiCullingProcess& rkCuller, NiVisibleArray& rkVisibleArray )
{
	SE_WorldChunkList::const_iterator iter;
	TRAV( m_kActiveChunks , iter )
	{
        if( (*iter)->IsLoaded() )
        {
            (*iter)->GetModelVisibleArrary( spNiCamera , rkCuller , rkVisibleArray , true , false );
        }
	}
}
//---------------------------------------------------------------------------------
void SE_SceneWorldMap::GetWaterVisibleArray(NiCameraPtr spNiCamera, 
											NiCullingProcess& rkCuller, 
											NiVisibleArray& rkVisibleArray,
											int nCullChunk )
{
	SE_WorldChunkListIter iter;
	int nDirectChunk = 0;
	TRAV( m_kActiveChunks , iter )
	{
		NiNodePtr spWaterRoot = (*iter)->GetNiChunkWaterRootNode();
		//在渲染的时候离自己最近的N个Chunk不Cull，否则会有水面突然被裁减的问题
		//但是在渲染反射图的时候必须全部Cull，否则会有大量多余的反射计算
		if ( nDirectChunk++ < nCullChunk )
		{
			GfxAddGeometryToSet(spWaterRoot, rkVisibleArray);
		}
		else
		{
			int iVisibleCount = CullNiScene(spNiCamera, spWaterRoot, rkCuller, rkVisibleArray,false);
		}
	}
}
//---------------------------------------------------------------------------------
void SE_SceneWorldMap::GetReflectVisibleArray( NiCameraPtr spNiCamera, NiCullingProcess& rkCuller, NiVisibleArray& rkVisibleArray, bool bTerrain  )
{
	_getReflectObjects( m_kActiveChunks , spNiCamera, rkCuller, rkVisibleArray,bTerrain);
}
//----------------------------------------------------------------
bool SE_SceneWorldMap::InitWorldMap(const StringType& rkWorldName,
									const StringType& rkDecoration,
									int MapResID)
{
	_initCharacterParams();
	_initPaticleParams();
	_initFinalEffectParams();
	if ( rkWorldName.length() == 0 )
	{
		GfxWriteLog( LOG_ERROR_FILELINE , LOG_SWITCH_ERROR ,
			"传入了空的地图名字！");
		T_ASSERT(false);
		return false;
	}
	char cResult[MAX_PATH];
	sprintf_s(cResult,"%s.tmap",rkWorldName.c_str());
	m_kWorldFileName = cResult; 

	bool bResult = false;

    TS_WorldConfig& rkWorldConfig = GfxGetWorldConfig();
	bResult = rkWorldConfig.InitTerrainConfig( m_kWorldFileName , rkDecoration );

	if ( !bResult )
	{
		GfxWriteLog( LOG_TAG_CLIENT_CRITICAL , LOG_SWITCH_CLIENT_CRITICAL ,
			"can't find map file %s" , m_kWorldFileName.c_str() );
		MessageBoxA2W_IDS( IDS_GAMEMAP_CORRUPTED , L"Warning", MB_OK|MB_ICONWARNING);
		T_ASSERT_CRITICAL_S(false);
		return false;
	}
	bResult = false;
    WORLD_TILE_COUNTX_PER_MAP           = rkWorldConfig.CFG_WORLD_TILE_COUNTX_PER_MAP;
    WORLD_CHUNKCLUSTER_COUNTX_PER_TILE  = rkWorldConfig.CFG_WORLD_CHUNKCLUSTER_COUNTX_PER_TILE;
    WORLD_CHUNK_COUNTX_PER_CLUSTER      = rkWorldConfig.CFG_WORLD_CHUNK_COUNTX_PER_CLUSTER;
    WORLD_GRID_WIDTH                    = rkWorldConfig.CFG_WORLD_GRID_WIDTH;
    WORLD_MAP_WIDTH                     = rkWorldConfig.CFG_WORLD_MAP_WIDTH;
    WORLD_TILE_WIDTH                    = rkWorldConfig.CFG_WORLD_TILE_WIDTH;
	/*
		如果rkDecoration不为空即为部分更新的地图类型，需要同时从两个不同的目录中读取资源文件
		优先找rkDecoration目录中的文件，如果找不到再用基础目录中的
	*/
	m_pkMapEffect = T_NEW_D MTF_EffectTile;
	
	if ( !rkDecoration.empty() )
	{
		NiSprintf(cResult, MAX_PATH, "%s_Effect.bin", 
			rkDecoration.c_str());
		bResult = m_pkMapEffect->ReadFromFile( cResult );
	}
	if ( !bResult )
	{
		NiSprintf(cResult, MAX_PATH, "%s_Effect.bin", 
			rkWorldName.c_str());
		bResult = m_pkMapEffect->ReadFromFile( cResult );
		if ( !bResult )
		{
			GfxWriteLog( LOG_ERROR_FILELINE , LOG_SWITCH_ERROR ,
				"无法找到地图效果层文件%s" , cResult );
		}
	}
	m_pkWorldMapFile = T_NEW_D FileSystem::WorldMapFile;
	bResult = false;
	if ( !rkDecoration.empty() )
	{
		sprintf_s(cResult,"%s.tmap",rkDecoration.c_str());
		bResult = m_pkWorldMapFile->ReadFromFile(cResult);
		
	}
	if ( !bResult )
	{
		bResult = m_pkWorldMapFile->ReadFromFile(m_kWorldFileName);
	}
	bResult = false;
	//_Global.bin文件以及弃用，暂时可以屏蔽
	//if ( !rkDecoration.empty() )
	//{
	//	NiSprintf( cResult , MAX_PATH , "%s_Global.bin" , 
	//		rkDecoration.c_str());
	//	m_pkMapGlobalInfo->LoadMap( cResult );
	//	bResult = m_pkMapGlobalInfo->GetFile() != NULL;
	//}
	//if ( !bResult )
	//{
	//	NiSprintf( cResult , MAX_PATH , "%s_Global.bin" , 
	//		rkWorldName.c_str());
	//	m_pkMapGlobalInfo->LoadMap( cResult );
	//}
	g_soundMgr.LoadMap( rkWorldName , rkDecoration, MapResID);
	//初始化光源
	{
		T_PROFILE_TIMER( "初始化LightField" );
		float fSunRotY = ( m_pkWorldMapFile->GetSunDirectionY() / 180.0f ) * NI_PI;
		float fSunRotZ = ( m_pkWorldMapFile->GetSunDirectionZ() / 180.0f )  * NI_PI;
		NiMatrix3 kRotY;
		kRotY.MakeYRotation( fSunRotY );
		NiMatrix3 kRotZ;
		kRotZ.MakeZRotation( fSunRotZ );
	
		m_spOutDoorLight->SetRotate( kRotY * kRotZ );
		m_spOutDoorLight->Update(0);
		m_spSceneFxLight->Update(0.0f);
		m_spSceneFxPointLight->Update( 0.0f );
		
		m_pkTerrainLightField = T_NEW_D Terrain::TS_LightField;
		m_pkTerrainLightField->InitField(rkWorldConfig.CFG_WORLD_CHUNK_COUNTX_PER_MAP,
										 rkWorldConfig.CFG_WORLD_CHUNK_COUNTX_PER_MAP,
										 true);
		m_pkTerrainLightField->Load(*m_pkWorldMapFile);

		m_spOutDoorLight->SetAmbientColor(NiColor::BLACK);
		m_spOutDoorLight->SetSpecularColor(NiColor::WHITE);

		m_spPointLight->SetConstantAttenuation(0.2f);
		m_spPointLight->SetLinearAttenuation(0.001f);
		m_spPointLight->SetQuadraticAttenuation(0.0f);

		m_spSceneFxPointLight->SetAmbientColor( NiColor::WHITE );
		m_spSceneFxPointLight->SetSpecularColor( NiColor::WHITE );
		m_spSceneFxPointLight->SetDiffuseColor( NiColor::WHITE );

		m_spSceneFxPointLight->SetConstantAttenuation(0.0f);
		m_spSceneFxPointLight->SetLinearAttenuation(0.001f);
		m_spSceneFxPointLight->SetQuadraticAttenuation(0.0f);
	}
	//初始化水面
	{
		m_pkWaterMeshManager = T_NEW_D BS_ClientWaterMeshManager;
		m_pkWaterMeshManager->Init();
		m_pkWaterMeshManager->Load(*m_pkWorldMapFile);
		m_fCurrentWaterHeight = 0.0f;
	}
	//初始化高度图管理器
	{
		m_pkHeightField = T_NEW_D SE_CompactHeightField<float>;
		m_pkHeightField->Init( 1 );

		//NiSprintf(cResult, MAX_PATH, "%s_Path\\walkable.tok", rkWorldConfig.CFG_WORLD_FILE_BASE.c_str());

		m_pkWalkableField = T_NEW_D SE_CompactHeightField<short>;
		m_pkWalkableField->Init( Terrain::WALKABLE_GRID_SCALE );
	}
	{
		m_pkShadowField = T_NEW_D SE_ActorShadowField;
		m_pkShadowField->Init();
	}
	//暂时静态加载LightProbe以后考虑动态加载
	//m_pkProbeMgr->Init();
	
	//set tile for later loading
	m_kSceneWorldTileVector.resize(WORLD_TILE_COUNTX_PER_MAP * WORLD_TILE_COUNTX_PER_MAP);
    int iTileIndex = 0;
    for(int iTileIndexY = 0; iTileIndexY < WORLD_TILE_COUNTX_PER_MAP; ++iTileIndexY)
	{
        for(int iTileIndexX = 0; iTileIndexX < WORLD_TILE_COUNTX_PER_MAP ; ++iTileIndexX)
		{
			m_kSceneWorldTileVector[iTileIndex] = T_NEW_D SE_SceneWorldTile(iTileIndexX, iTileIndexY, this);

			_attachMessageHandler(m_kSceneWorldTileVector[iTileIndex]);
			m_kSceneWorldTileVector[iTileIndex]->SetActive(false);

            ++iTileIndex;
		}
	}
	m_kActiveChunks.clear();

    m_bIsCurrentInSceneLightning = false;
    m_fCurrentSceneLightningPower = 1.0f;

	return true;
}
//----------------------------------------------------------------------
void SE_SceneWorldMap::UnInitWorldMap()
{
		for(int iTileIndex = 0; iTileIndex < GfxGetWorldConfig().CFG_WORLD_TILE_COUNT_PER_MAP; iTileIndex++)
		{
			SE_SceneWorldTile* pkTile = m_kSceneWorldTileVector[iTileIndex];
			T_ASSERT(pkTile && "NULL Tile");

			if(pkTile)
			{
				_detachMessageHandler(pkTile);
				T_DELETE_D pkTile;
				m_kSceneWorldTileVector[iTileIndex] = NULL;
			}
		}
		m_kSceneWorldTileVector.clear();
		if ( m_pkWaterMeshManager )
		{
			m_pkWaterMeshManager->UnInit();
			m_pkWaterMeshManager->UnLoad();
		}
		
		T_SAFE_DELETE( m_pkWaterMeshManager );
	
		T_SAFE_DELETE(m_pkTerrainLightField);
		if ( m_pkHeightField )
		{
			m_pkHeightField->UnInit();
		}
		T_SAFE_DELETE( m_pkHeightField );
		if( m_pkWalkableField) 
			m_pkWalkableField->UnInit();
		T_SAFE_DELETE( m_pkWalkableField );
		if ( m_pkShadowField )
		{
			m_pkShadowField->UnInit();
		}
		T_SAFE_DELETE( m_pkShadowField );

		T_SAFE_DELETE( m_pkMapEffect );

		m_kActiveChunks.clear();
		m_kLoadedChunks.clear();

	T_SAFE_DELETE( m_pkWorldMapFile );

	m_pkProbeMgr->ShutDown();

	m_bInitParams = false;
	m_bCompute = true;
}
//----------------------------------------------------------------------
unsigned int SE_SceneWorldMap::PerformGrassInstanceRendering( const NiPoint3& kPlayerPos , NiInstanceTriShapePtr spGrassInstanceTriShape, NiDX9Renderer* pkInstanceRenderer )
{
	unsigned int uiGrassDp = 0;
	SE_WorldChunkListIter iter;
	TRAV( m_kActiveChunks , iter )
	{
		uiGrassDp += (*iter)->PerformGrassInstanceRendering( spGrassInstanceTriShape,pkInstanceRenderer );
	}
	return uiGrassDp;
}
//---------------------------------------------------------------------------------
bool SE_SceneWorldMap::PickMap( NiPick* pkScenePick,const NiPoint3 &origin, const NiPoint3 &dir , const NiPoint3& kPlayerPos )
{
	pkScenePick->ClearResultsArray();
	SE_WorldChunkListIter iter;
	bool bFound = false;
	TRAV( m_kActiveChunks , iter )
	{
		if( (*iter)->PickMap( pkScenePick , origin , dir , kPlayerPos ) )
		{
			bFound = true;
			break;
		}
	}

    //GfxWriteDefaultLog( LOG_INFO_TAG , "Pick Count is %d" , nPickCount );
	if ( !bFound && pkScenePick->GetResults().GetEffectiveSize() == 0 )
	{
		return false;
	}
	else
	{
		return true;
	}
}
bool SE_SceneWorldMap::PickVertical( NiPick* pkScenePick, long lX , long lY , long& rlZ )
{
	int iTileX , iTileY;
	NiPoint3 kCenter( (float)lX , (float)lY , 8000.0f );
	GetWhichTile( iTileX , iTileY , kCenter.x , kCenter.y );
	SE_SceneWorldTile* pkTile = GetWorldTile(iTileX,iTileY);
	if( pkTile && pkTile->GetActive() )
	{
		SE_SceneWorldChunk* pkChunk = pkTile->GetChunkByCenter( kCenter );
		if ( pkChunk && pkChunk->IsLoaded() )
		{
			return pkChunk->PickMap( pkScenePick , kCenter , -NiPoint3::UNIT_Z , NiPoint3::ZERO );
		}
	}
	return false;
}
//---------------------------------------------------------------------------------
bool SE_SceneWorldMap::GetTerrainLocalInfo(NiPick* pkPick, 
										   SE_SceneTerrainLocalInfo& rkLocalInfo, 
										   float fLocationX, float fLocationY)
{
    if (rkLocalInfo._bGetHeight)
    {
        rkLocalInfo._fLocalHeight = m_pkHeightField->GetHeight(fLocationX, fLocationY);
    }

	if (rkLocalInfo._bGetNormal)
	{
        m_pkHeightField->GetNormal(fLocationX, fLocationY, rkLocalInfo._kLocalNormal);
	}

    return true;
}
static inline NiPoint3  XGetTerrainNormal(NiPoint3 &v0,NiPoint3 &v1,NiPoint3 &v2,NiPoint3 &v3,NiPoint3 &v4)
{
	NiPoint3  nor1,nor2,nor3,nor4,nor;

	NiPoint3 v11,v22;
	v11 = (v2 - v0 ) ;
	v22 = ( v1 - v0) ;
	nor1 = v11.Cross(v22);
	v11 = (v3 - v0 ) ;
	v22 = ( v2 - v0) ;
	nor2 = v11.Cross(v22);
	v11 = (v4 - v0 );
	v22 = ( v3 - v0);
	nor3 = v11.Cross(v22);
	v11 = (v1 - v0 );
	v22 = ( v4 - v0);
	nor4 = v11.Cross(v22);


	nor1.Unitize();
	nor2.Unitize();
	nor3.Unitize();
	nor4.Unitize();

	nor = ( nor1 + nor2 + nor3 + nor4 ) / 4.0f;
	nor = - nor;
	nor.Unitize();
	return nor;
}
bool SE_SceneWorldMap::GetWalkableInfo( SE_SceneTerrainLocalInfo& rkLocalInfo, float fLocationX,float fLocationY )
{
	if ( m_pkWalkableField == NULL )
	{
		return false;
	}
	if (rkLocalInfo._bGetHeight)
	{
		rkLocalInfo._fLocalHeight = m_pkWalkableField->GetHeight(fLocationX, fLocationY);
		if ( rkLocalInfo._fLocalHeight == -999999.0f )
		{
			//临时解决的方法，如果高度图导出有问题，就用地形高度，避免掉的太多
			rkLocalInfo._fLocalHeight = m_pkHeightField->GetHeight(fLocationX, fLocationY);
		}
	}
	if ( rkLocalInfo._bGetNormal )
	{
		//test
		//float fHeight[5];
		//int x = (int)( fLocationX * 0.01f ), y = (int)( fLocationY * 0.01f );

		//fHeight[0] = m_pkHeightField->GetHeight( x , y );

		//if ( fHeight[0] == 0.0f )
		//{
		//	rkLocalInfo._kLocalNormal = NiPoint3( 0 , 0 , 1 );
		//}

		//fHeight[4] = fHeight[0];
		//fHeight[3] = fHeight[0];
		//fHeight[2] = fHeight[0];
		//fHeight[1] = fHeight[0];

		//fHeight[1] = m_pkHeightField->GetHeight(x-1,y);
		//fHeight[2] = m_pkHeightField->GetHeight(x,y-1);
		//fHeight[3] = m_pkHeightField->GetHeight(x+1,y);
		//fHeight[4] = m_pkHeightField->GetHeight(x,y+1);


		//NiPoint3 v1,v2,v3,v4,v5;

		//v1.x = GfxGetWorldConfig().CFG_WORLD_GRID_WIDTH;
		//v1.y = GfxGetWorldConfig().CFG_WORLD_GRID_WIDTH;
		//v1.z = fHeight[0];

		//v2.x = 0;
		//v2.y = GfxGetWorldConfig().CFG_WORLD_GRID_WIDTH;
		//v2.z = fHeight[1];

		//v3.x = GfxGetWorldConfig().CFG_WORLD_GRID_WIDTH;
		//v3.y = 0;
		//v3.z = fHeight[2];

		//v4.x = GfxGetWorldConfig().CFG_WORLD_GRID_WIDTH * 2;
		//v4.y = GfxGetWorldConfig().CFG_WORLD_GRID_WIDTH;
		//v4.z = fHeight[3];


		//v5.x = GfxGetWorldConfig().CFG_WORLD_GRID_WIDTH;
		//v5.y = GfxGetWorldConfig().CFG_WORLD_GRID_WIDTH * 2;
		//v5.z = fHeight[4];

		//rkLocalInfo._kLocalNormal = XGetTerrainNormal(v1,v2,v3,v4,v5);
		m_pkWalkableField->GetNormal( fLocationX , fLocationY , rkLocalInfo._kLocalNormal );
	}
	return true;
}
bool SE_SceneWorldMap::GetWalkableInfo( SE_SceneTerrainLocalInfo& rkLocalInfo, float fLocationX,float fLocationY , float fPrevHeight )
{
	if ( m_pkWalkableField == NULL )
	{
		return false;
	}
	if (rkLocalInfo._bGetHeight)
	{
		rkLocalInfo._fLocalHeight = m_pkWalkableField->GetHeight(fLocationX, fLocationY,fPrevHeight);
		if ( rkLocalInfo._fLocalHeight == -999999.0f )
		{
			rkLocalInfo._fLocalHeight = m_pkHeightField->GetHeight(fLocationX, fLocationY);
		}
	}
	if ( rkLocalInfo._bGetNormal )
	{
		m_pkWalkableField->GetNormal( fLocationX , fLocationY , rkLocalInfo._kLocalNormal );
	}
	return true;
}

float SE_SceneWorldMap::GetHeightFieldHeight(int iHeightFieldCoordIndexX, int iHeightFieldCoordIndexY)
{
    if (m_pkWalkableField == NULL)
    {
        return 0.0f;
    }

    float fWakableHeight = m_pkWalkableField->GetHeight(iHeightFieldCoordIndexX * 2 , iHeightFieldCoordIndexY * 2 );

    if (m_pkHeightField == NULL)
    {
        return 0.0f;
    }

 //   float fTerrainHeight = m_pkHeightField->GetHeight(iHeightFieldCoordIndexX, iHeightFieldCoordIndexY);

	//if ( abs( fWakableHeight - fTerrainHeight ) > 10 )
	//{
	//	fWakableHeight = fTerrainHeight;
	//}
	return fWakableHeight;
}
//---------------------------------------------------------------------------------
void SE_SceneWorldMap::UpdateCameraBlockModels( NiPick* pkPick , const Terrain::TS_BoundingVolume& rkPlayerAABB , const NiPoint3& kOrigin,const NiPoint3& kDir )
{
	T_PROFILE_EX("SE_SceneWorldMap::UpdateCameraBlockModels",3.0f);
	
	SE_WorldChunkListIter iter;
	TRAV( m_kActiveChunks , iter )
	{
		if ( !(*iter)->IsLoaded() )
		{
			continue;
		}
		(*iter)->UpdateCameraBlockModels( pkPick , rkPlayerAABB , kOrigin , kDir  );
	}
}
//----------------------------------------------------------------------------
void SE_SceneWorldMap::_event_LogicPlayerMove( MG_MessageBase& rkMessageBase )
{
	//float fPlayerPosX = rkMessageBase.m_fParam1;
	//float fPlayerPosY = rkMessageBase.m_fParam2;
	//float fPlayerPosZ = rkMessageBase.m_fParam3;
	if ( m_pkTerrainLightField == NULL )
	{
		return;
	}

	float fSampleS = rkMessageBase.m_fParam1 / WORLD_MAP_WIDTH;
	float fSampleT = rkMessageBase.m_fParam2 / WORLD_MAP_WIDTH;

	_updateTargetFogParam( fSampleS, fSampleT );
	_updateTargetCharacterParam( fSampleS, fSampleT );
	_updateTargetPointLight( fSampleS, fSampleT );
	_updateTargetPaticleParams(fSampleS, fSampleT);
	_updateTargetFinalEffectParams(fSampleS, fSampleT);
	
	if ( !m_bInitParams )
	{
		m_bInitParams = true;
		m_bCompute = true;

		m_kCurrentAmbientColor   = m_kTargetAmbientColor;
		m_kCurrentDiffuseColor   = m_kTargetDiffuseColor;
		m_kCurrentBackLightColor = m_kTargetBackLightColor ;
		m_kCurrentShadowColor    = m_kTargetShadowColor ;

		m_kCurrentFogColor       = m_kTargetFogColor;
		m_kDepthFogParam         = m_kTargetDepthFogParam ; 
		m_kHeightFogParam        = m_kTargetHeightFogParam;
		m_fCurrentLightMultiplier = m_fTargetLightMultiplier;
	
		m_fCurrentCharAmbientPower		= m_fCharacterAmbientPower;
		m_fCurrentCharSunPower          = m_fCharacterSunPower;
		m_fCurrentCharMatEmmisive		= m_fCharacterMatEmmisive;
		m_fCurrentCharSelfIllumination	= m_fCharacterSelfIllumination;
		m_fCurrentCharSpecularPower		= m_fCharacterSpecularPower;
		m_fCurrentCharSpecularMultiplier= m_fCharacterSpecularMultiplier;
		m_kCurrentCharDiffuseLightColor	= m_kCharacterDiffuseLightColor;
		m_kCurrentCharAmbientColor		= m_kCharacterAmbientColor;
		m_kCurrentCharLightWorldDir		= m_kCharacterLightWorldDir;
		m_kCurrentCharRimLightColor		= m_kCharacterRimLightColor;
		m_fCurrentCharRimLightPower		= m_fCharacterRimLightPower;
		m_fCurrentCharIllumBias			= m_fCharacterIllumBias;

		m_fCurrentSharpenIntensity = m_fTargetSharpenIntensity;
		m_fCurrentSoftLightScope   = m_fTargetSoftLightScope;
		m_fCurrentGammaIntensity   = m_fTargetGammaIntensity;
		m_fCurrentGammaExponent    = m_fTargetGammaExponent;

		m_fCurrentParticleDensity = m_fTargetParticleDensity;
		m_fCurrentParticleSpeed   = m_fTargetParticleSpeed;
		m_fCurrentParticleWidth   = m_fTargetParticleWidth;
		m_fCurrentParticleLength  = m_fTargetParticleLength;
		m_fCurrentParticleAlpha   = m_fTargetParticleAlpha;
		m_fCurrentParticleWind    = m_fTargetParticleWind;

	}
	else
	{
		m_bCompute = !_IsEqual();
	}
}
/*
计算雾效的淡入淡出
*/
static void ComputeFogFadeIn( float& fCurrentDensity , float fTargetDensity , NiColorA& kFogParam , NiColorA kTargetParam , 
							 bool& bFogFadeOut , bool& bFogFadeIn ,  float& fTransTimeLeft, float fDeltaTime )
{
	if ( ( fCurrentDensity != fTargetDensity ) &&  !bFogFadeOut && !bFogFadeIn   )
	{
		//如果需要进行区域切换，并且不在Fade的过程中
		bFogFadeOut = true;
		bFogFadeIn = false;
		//GfxWriteDefaultLog( LOG_INFO_TAG , "start fog fade in/out , current is %f , target is %f" , fCurrentDensity,fTargetDensity );
	}
	if ( bFogFadeOut )
	{
		//雾效只能用Density来做FadeIn/FadeOut
		//先从Current->0做FadeOut
		if ( fCurrentDensity < 0.01f )
		{
			fCurrentDensity = 0.0f;
			bFogFadeIn = true;
			bFogFadeOut = false;
			fTransTimeLeft = g_settings.CHARACTER_LIGHT_TRANS_TIME / 2.0f;
			return;
			//GfxWriteDefaultLog( LOG_INFO_TAG , "end fog fade out start fog fade in, current is %f , target is %f" , fCurrentDensity,fTargetDensity );
		}
		else
		{
			fCurrentDensity -= fCurrentDensity / fTransTimeLeft * fDeltaTime;
			//GfxWriteDefaultLog( LOG_INFO_TAG , "continue fog fade out , current is %f , target is %f" , fCurrentDensity,fTargetDensity );
		}
	}
	if ( bFogFadeIn )
	{
		if ( fCurrentDensity > fTargetDensity )
		{
			bFogFadeIn = false;
			fCurrentDensity = fTargetDensity;
		}
		//再从0->Target做FadeIn
		if ( abs( fCurrentDensity - fTargetDensity ) < 0.01f )
		{
			bFogFadeIn = false;
			fCurrentDensity = fTargetDensity;
			fTransTimeLeft = g_settings.CHARACTER_LIGHT_TRANS_TIME / 2.0f;
			//GfxWriteDefaultLog( LOG_INFO_TAG , "end fog fade in , current is %f , target is %f" , fCurrentDensity,fTargetDensity );
		}
		else
		{
			if ( bFogFadeIn )
			{
				fCurrentDensity += fTargetDensity / fTransTimeLeft * fDeltaTime;
				//GfxWriteDefaultLog( LOG_INFO_TAG , "continue fog fade in , current is %f , target is %f" , fCurrentDensity,fTargetDensity );
			}
		}
		kFogParam.r = kTargetParam.r;
		kFogParam.g = kTargetParam.g;
		kFogParam.a = kTargetParam.a;
	}
	fCurrentDensity = UT_MathBase::ClampFloat( fCurrentDensity , 0.0f , 1.0f );
	kFogParam.b = fCurrentDensity;
}
/*
灯光区域颜色的混合
*/
static void ComputeColorFadeIn( NiColor& kCurrentColor , const NiColor& kTargetColor , const float fTransTimeLeft,  float fDeltaTime )
{
	NiColor kDeltaColor;
	NiPoint3 kDeltaVector;

	kDeltaColor = kTargetColor - kCurrentColor;
	kDeltaVector.x = kDeltaColor.r;
	kDeltaVector.y = kDeltaColor.g;
	kDeltaVector.z = kDeltaColor.b;

	if(kDeltaVector.SqrLength() < 0.001f)
	{
		kCurrentColor = kTargetColor;
	}
	else
	{
		kCurrentColor.r += kDeltaVector.x / fTransTimeLeft * fDeltaTime;
		kCurrentColor.g += kDeltaVector.y / fTransTimeLeft * fDeltaTime;
		kCurrentColor.b += kDeltaVector.z / fTransTimeLeft * fDeltaTime;       
	}
}

static void ComputeFloatFadeIn(float& fCurrent, const float fTarget, const float fTransTimeLeft, float fDeltaTime)
{
	float fDelta = fTarget - fCurrent;
	if ( fDelta * fDelta < 0.00001f )
	{
		fCurrent = fTarget;
	}
	else
	{
		fCurrent += fDelta /fTransTimeLeft * fDeltaTime;
	}
}

static void ComputeDirFadeIn(NiPoint3& kCurrent, const NiPoint3& kTarget, const float fTransTimeLeft, float fDeltaTime )
{
	NiPoint3 kDeltaVector;
	kDeltaVector = kTarget - kCurrent;

	if(kDeltaVector.SqrLength() < 0.001f)
	{
		kCurrent = kTarget;
	}
	else
	{
		kDeltaVector.Unitize();
		kCurrent.x += kDeltaVector.x / fTransTimeLeft * fDeltaTime;
		kCurrent.y += kDeltaVector.y / fTransTimeLeft * fDeltaTime;
		kCurrent.z += kDeltaVector.z / fTransTimeLeft * fDeltaTime;
	}
}
//----------------------------------------------------------------------------
void SE_SceneWorldMap::_event_TimerTick( MG_MessageBase& rkMessage )
{
	T_PROFILE("Update::SE_SceneWorldMap");

	/*
	编辑器中为每一个Chunk计算了一个世界坐标的包围盒
	在发送动态加载请求的时候，还是按以前的做法，加载
	当前玩家为中心的6*6的Chunk，这是为了避免因为Chunk加载延迟造成黑块出现
	但是在这里，我们使用当前的相机Frustum来和已经加载的Chunk
	做一次相交测试，把不可见的Chunk排除掉。这样可以保证不会出现黑块
	同时我们也不用把当前加载的Chunk全部做一次Cull和绘制
	这样在获得VisibleArray的时候就不用做Cull了（要测试下特效物件是否会出现问题，比如Billboard）
	*/
	//GfxWriteLog( LOG_WARNING_FILELINE, LOG_SWITCH_WARNING, "===fDepthFogDensity: =%f=, bDepthFogFadeOut: =%d=, bDepthFogFadeIn: =%d=, m_fCurrentHeightFogDensity: =%f=",  
	//m_fCurrentDepthFogDensity, m_bDepthFogFadeOut, m_bDepthFogFadeIn, m_fCurrentHeightFogDensity);

	//RefreshMessageHandlerTree();
	m_kActiveChunks.clear();

    NiCamera* pkCam = CA_CameraManager::GetSingleton().GetNiCamera();
	SE_WorldChunkListIter iter;
	TRAV( m_kLoadedChunks , iter )
	{
		TS_BoundingVolume& rkBound = (*iter)->GetBoundingVolume();
		if ( rkBound.Frustum_intersect( pkCam->GetViewFrustum() , pkCam->GetWorldTransform() ) )
		{
			m_kActiveChunks.push_back( *iter );
		}
	}
	std::stable_sort( m_kActiveChunks.begin() , m_kActiveChunks.end() , ChunkDistanceCmp() );

	float fDeltaTime = rkMessage.m_fParam2;
	if ( !m_bCompute )
	{
		m_fTransTimeLeft = g_settings.CHARACTER_LIGHT_TRANS_TIME;
		m_fFogDepthTransTimeLeft = m_fTransTimeLeft/2.0f;
		m_fFogHeightTransTimeLeft = m_fTransTimeLeft/2.0f;
		return;
	}

	if ( m_fTransTimeLeft < fDeltaTime )
    {
        fDeltaTime = m_fTransTimeLeft;
    }

	ComputeColorFadeIn( m_kCurrentAmbientColor , m_kTargetAmbientColor , m_fTransTimeLeft, fDeltaTime );

	ComputeColorFadeIn( m_kCurrentDiffuseColor , m_kTargetDiffuseColor , m_fTransTimeLeft, fDeltaTime );
	m_spOutDoorLight->SetDiffuseColor(m_kCurrentDiffuseColor);
	
	ComputeColorFadeIn( m_kCurrentBackLightColor , m_kTargetBackLightColor , m_fTransTimeLeft, fDeltaTime );
	ComputeColorFadeIn( m_kCurrentShadowColor , m_kTargetShadowColor , m_fTransTimeLeft, fDeltaTime );

	ComputeColorFadeIn( m_kCurrentFogColor , m_kTargetFogColor , m_fTransTimeLeft, fDeltaTime );

	ComputeFogFadeIn( m_fCurrentDepthFogDensity , m_fTargetDepthFogDensity , 
		m_kDepthFogParam , m_kTargetDepthFogParam , 
		m_bDepthFogFadeOut , m_bDepthFogFadeIn , m_fFogDepthTransTimeLeft, fDeltaTime );

	ComputeFogFadeIn( m_fCurrentHeightFogDensity , m_fTargetHeightFogDensity , 
		m_kHeightFogParam , m_kTargetHeightFogParam , 
		m_bHeightFogFadeOut , m_bHeightFogFadeIn , m_fFogHeightTransTimeLeft, fDeltaTime );

	ComputeFloatFadeIn( m_fCurrentLightMultiplier, m_fTargetLightMultiplier, m_fTransTimeLeft, fDeltaTime );

	_computeCurrentCharacterParam( m_fTransTimeLeft, fDeltaTime );
	_computeCurrentPaticleParams( m_fTransTimeLeft, fDeltaTime );
	_computeCurrentFinalEffectParams( m_fTransTimeLeft, fDeltaTime );

	if ( m_fTransTimeLeft <= fDeltaTime )
	{
		m_fTransTimeLeft = g_settings.CHARACTER_LIGHT_TRANS_TIME;
	}
	else
	{
		m_fTransTimeLeft -= fDeltaTime;
	}
    if ( m_fFogDepthTransTimeLeft <= fDeltaTime )
    {
        m_fFogDepthTransTimeLeft = g_settings.CHARACTER_LIGHT_TRANS_TIME/2.0f;
    }
    else
    {
        m_fFogDepthTransTimeLeft -= fDeltaTime;
    }
    if ( m_fFogHeightTransTimeLeft <= fDeltaTime )
    {
        m_fFogHeightTransTimeLeft = g_settings.CHARACTER_LIGHT_TRANS_TIME/2.0f;
    }
    else
    {
        m_fFogHeightTransTimeLeft -= fDeltaTime;
    }
}
//---------------------------------------------------------------------------------
void SE_SceneWorldMap::GetWhichTile( int& iTileIndexX, int& iTileIndexY, float fPointX, float fPointY )
{
	iTileIndexX = (int)(fPointX / WORLD_TILE_WIDTH);
	iTileIndexY = (int)(fPointY / WORLD_TILE_WIDTH);

    iTileIndexX = min(WORLD_TILE_COUNTX_PER_MAP - 1, max(0, iTileIndexX));		// clamp(0, WORLD_TILE_COUNTX_PER_MAP - 1)
    iTileIndexY = min(WORLD_TILE_COUNTX_PER_MAP - 1, max(0, iTileIndexY));
}

void SE_SceneWorldMap::_getVisibleSet( NiNodePtr spWaterRoot, NiVisibleArray &rkVisibleArray )
{
	unsigned int uiCount = spWaterRoot->GetArrayCount();
	for ( unsigned int i = 0 ; i < uiCount ; i++ )
	{
		NiAVObject* pkChild = spWaterRoot->GetAt( i );
		if ( pkChild && NiDynamicCast( NiGeometry , pkChild ) )
		{
			rkVisibleArray.Add( *( (NiGeometry*)pkChild ) );
		}
	}
}

void SE_SceneWorldMap::_getReflectObjects( const SE_WorldChunkList& rkChunks, NiCameraPtr spNiCamera, NiCullingProcess& rkCuller, NiVisibleArray& rkVisibleArray, bool bTerrain )
{
	//注意这个函数每帧只能被调用一次
	SE_WorldChunkList::const_iterator iter;
	m_fCurrentWaterHeight = 0.0f;
	TRAV( rkChunks , iter )
	{
		if( !(*iter)->IsLoaded() )
			continue;

		if ( !bTerrain )
		{
		/*
			(*iter)->GetModelVisibleArrary( spNiCamera , rkCuller , rkVisibleArray , false , false );
		/*/
			(*iter)->GetReflectVisibleArrary( spNiCamera , rkCuller , rkVisibleArray);
		//*/
		}
		else
		{
			NiNodePtr spTerrainRoot = (*iter)->GetNiChunkTerrainRootNode();
			NiCullScene(spNiCamera, spTerrainRoot, rkCuller, rkVisibleArray,false);
		}
		/*float fChunkWaterHeight = (*iter)->GetChunkWaterHeight();
		if ( fChunkWaterHeight > m_fCurrentWaterHeight )
		{
			m_fCurrentWaterHeight = fChunkWaterHeight;
		}*/
	}
}
void SE_SceneWorldMap::_initCharacterParams()
{
	m_fCharacterAmbientPower = 0.0f;
	m_fCharacterSunPower = 0.0f;
	m_fCharacterMatEmmisive = 0.0f;
	m_fCharacterSelfIllumination = 0.0f;
	m_fCharacterSpecularPower = 0.0f;
	m_fCharacterSpecularMultiplier = 0.0f;
	m_kCharacterDiffuseLightColor= NiColor::BLACK;
	m_kCharacterAmbientColor = NiColor::BLACK;
	m_kCharacterLightWorldDir = NiPoint3::ZERO;
	m_kCharacterRimLightColor= NiColor::BLACK;
	m_fCharacterRimLightPower = 0.0f;
	m_fCharacterIllumBias	  = 0.0f;
	m_kPointLightPos		  = NiPoint3::ZERO;
	m_kPointLightColor		  = NiColor::BLACK;
	m_kPointLightParam		  = NiPoint3::ZERO;
	m_fPointLightEnable		  = 0.0f;

	m_fCurrentCharAmbientPower = 0.0f;
	m_fCurrentCharSunPower = 0.0f;
	m_fCurrentCharMatEmmisive = 0.0f;
	m_fCurrentCharSelfIllumination = 0.0f;
	m_fCurrentCharSpecularPower = 0.0f;
	m_fCurrentCharSpecularMultiplier = 0.0f;
	m_kCurrentCharDiffuseLightColor = NiColor::BLACK;
	m_kCurrentCharAmbientColor = NiColor::BLACK;
	m_kCurrentCharLightWorldDir = NiPoint3::ZERO;
	m_kCurrentCharRimLightColor = NiColor::BLACK;
	m_fCurrentCharRimLightPower = 0.0f;
	m_fCurrentCharIllumBias = 0.0f;
}

void SE_SceneWorldMap::_initPaticleParams()
{
	m_fTargetParticleDensity = 0.0f;
	m_fTargetParticleSpeed = 0.0f;
	m_fTargetParticleWidth = 0.0f;
	m_fTargetParticleLength = 0.0f;
	m_fTargetParticleAlpha = 0.0f;
	m_fTargetParticleWind = 0.0f;

	m_fCurrentParticleDensity = 0.0f;
	m_fCurrentParticleSpeed = 0.0f;
	m_fCurrentParticleWidth = 0.0f;
	m_fCurrentParticleLength = 0.0f;
	m_fCurrentParticleAlpha = 0.0f;
	m_fCurrentParticleWind = 0.0f;

    m_fCurrentRegionSceneLightningInterval = -1.0f;
    m_fCurrentRegionSceneLightningPower = 0.0f;
}

void SE_SceneWorldMap::_initFinalEffectParams()
{
	m_fTargetSharpenIntensity = 0.0f;
	m_fTargetSoftLightScope = 0.0f;
	m_fTargetGammaIntensity = 0.0f;
	m_fTargetGammaExponent = 1.0f;

	m_fCurrentSharpenIntensity = 0.0f;
	m_fCurrentSoftLightScope = 0.0f;
	m_fCurrentGammaIntensity = 0.0f;
	m_fCurrentGammaExponent = 1.0f;
}

bool SE_SceneWorldMap::_IsEqual()
{
	bool bEqual = _IsColorEqual( m_kTargetAmbientColor, m_kCurrentAmbientColor );
    if (!bEqual)
    {
        return false;
    }
	bEqual = _IsColorEqual( m_kTargetDiffuseColor, m_kCurrentDiffuseColor );
    if (!bEqual)
    {
        return false;
    }
	bEqual = _IsColorEqual( m_kTargetBackLightColor, m_kCurrentBackLightColor );
    if (!bEqual)
    {
        return false;
    }
	bEqual = _IsColorEqual( m_kTargetShadowColor, m_kCurrentShadowColor );
    if (!bEqual)
    {
        return false;
    }
	bEqual = _IsColorEqual( m_kTargetFogColor, m_kCurrentFogColor );
    if (!bEqual)
    {
        return false;
    }
	bEqual = (m_fTargetDepthFogDensity - m_fCurrentDepthFogDensity) < 0.00001f;
    if (!bEqual)
    {
        return false;
    }

	bEqual =		   abs( m_kTargetDepthFogParam.r - m_kDepthFogParam.r ) < 0.00001f
					&& abs( m_kTargetDepthFogParam.g - m_kDepthFogParam.g ) < 0.00001f
					&& abs( m_kTargetDepthFogParam.b - m_kDepthFogParam.b ) < 0.00001f
					&& abs( m_kTargetDepthFogParam.a - m_kDepthFogParam.a ) < 0.00001f;
	bEqual = bEqual && ( m_bDepthFogFadeOut == m_bDepthFogFadeIn );
	bEqual = bEqual && abs(m_fTargetHeightFogDensity - m_fCurrentHeightFogDensity) < 0.00001f;

	if (!bEqual)
    {
        return false;
    }

	bEqual = (	           abs( m_fCharacterAmbientPower		- m_fCurrentCharAmbientPower ) < 0.00001f
						&& abs( m_fCharacterSunPower			- m_fCurrentCharSunPower ) < 0.00001f
						&& abs( m_fCharacterMatEmmisive		- m_fCurrentCharMatEmmisive ) < 0.00001f
						&& abs( m_fCharacterSelfIllumination	- m_fCurrentCharSelfIllumination ) < 0.00001f
						&& abs( m_fCharacterSpecularPower		- m_fCurrentCharSpecularPower  ) < 0.00001f
						&& abs( m_fCharacterSpecularMultiplier	- m_fCurrentCharSpecularMultiplier ) < 0.00001f
						&& abs( m_fCharacterRimLightPower		- m_fCurrentCharRimLightPower  ) < 0.00001f
						&& abs( m_fCharacterIllumBias			- m_fCurrentCharIllumBias ) < 0.00001f );
	bEqual = bEqual && _IsColorEqual( m_kCharacterDiffuseLightColor, m_kCurrentCharDiffuseLightColor );
    if (!bEqual)
    {
        return false;
    }
	bEqual = _IsColorEqual( m_kCharacterAmbientColor, m_kCurrentCharAmbientColor );
    if (!bEqual)
    {
        return false;
    }
	NiPoint3 kPos = m_kCharacterLightWorldDir - m_kCurrentCharLightWorldDir;
	bEqual = kPos.SqrLength() < 0.001f;
    if (!bEqual)
    {
        return false;
    }

	bEqual = _IsColorEqual( m_kCharacterRimLightColor, m_kCurrentCharRimLightColor );
	if (!bEqual)
    {
        return false;
    }

	bEqual = (             abs( m_fCurrentParticleDensity - m_fTargetParticleDensity ) < 0.00001f
						&& abs( m_fCurrentParticleSpeed - m_fTargetParticleSpeed ) < 0.00001f
						&& abs( m_fCurrentParticleWidth - m_fTargetParticleWidth ) < 0.00001f
						&& abs( m_fCurrentParticleLength - m_fTargetParticleLength ) < 0.00001f
						&& abs( m_fCurrentParticleAlpha - m_fTargetParticleAlpha ) < 0.00001f
						&& abs( m_fCurrentParticleWind - m_fTargetParticleWind ) < 0.00001f );

	bEqual = (             abs( m_fCurrentSharpenIntensity - m_fTargetSharpenIntensity ) < 0.00001f
						&& abs( m_fCurrentSoftLightScope - m_fTargetSoftLightScope ) < 0.00001f
						&& abs( m_fCurrentGammaIntensity - m_fTargetGammaIntensity ) < 0.00001f
						&& abs( m_fCurrentGammaExponent - m_fTargetGammaExponent ) < 0.00001f );

   return bEqual;
}

void SE_SceneWorldMap::_updateTargetFogParam( float fSampleS, float fSampleT )
{
	LightRegion* pkRegion = m_pkTerrainLightField->GetRegion( fSampleS , fSampleT );
	if ( pkRegion )
	{
		m_kTargetAmbientColor		= pkRegion->kAmbient;
		m_kTargetDiffuseColor		= pkRegion->kSun;
		m_kTargetBackLightColor		= pkRegion->kBack;
		m_kTargetShadowColor		= pkRegion->kShadowColor;
		m_kTargetFogColor			= pkRegion->kHeightFogColor;
		m_fTargetLightMultiplier	= pkRegion->fLightMultiplier;

		if ( !m_bHeightFogFadeIn && !m_bHeightFogFadeOut )
		{
			m_kTargetHeightFogParam.r = pkRegion->fHeightFogParam[0];
			m_kTargetHeightFogParam.g = pkRegion->fHeightFogParam[1];
			m_kTargetHeightFogParam.b = pkRegion->fHeightFogParam[2];
			if ( fabs( m_kTargetHeightFogParam.g - m_kTargetHeightFogParam.b ) > 0.000005 )
			{
				m_kTargetHeightFogParam.a = 1.0f / ( m_kTargetHeightFogParam.g - m_kTargetHeightFogParam.r );
			}
			else
			{
				m_kTargetHeightFogParam.a = 0.0f;
			}
			m_fTargetHeightFogDensity = m_kTargetHeightFogParam.b;
            //GfxWriteLog( LOG_TAG_ERROR , LOG_SWITCH_ERROR ,
            //    "Height fog , density target: %f" , m_fTargetHeightFogDensity );
		}

		if ( !m_bDepthFogFadeOut && !m_bDepthFogFadeIn )
		{
			m_kTargetDepthFogParam.r = pkRegion->fDepthFogParam[0];
			m_kTargetDepthFogParam.g = pkRegion->fDepthFogParam[1];
			m_kTargetDepthFogParam.b = pkRegion->fDepthFogParam[2];
			if ( fabs( m_kTargetDepthFogParam.g - m_kTargetDepthFogParam.b ) > 0.000005 )
			{
				m_kTargetDepthFogParam.a = 1.0f / ( m_kTargetDepthFogParam.g - m_kTargetDepthFogParam.r );
			}
			else
			{
				m_kTargetDepthFogParam.a = 0.0f;
			}
			m_fTargetDepthFogDensity = m_kTargetDepthFogParam.b;
            //GfxWriteLog( LOG_TAG_ERROR , LOG_SWITCH_ERROR ,
            //    "Depth fog , density target: %f" , m_fTargetDepthFogDensity );
		}
	}
}

void SE_SceneWorldMap::_updateTargetCharacterParam( float fSampleS, float fSampleT )
{
	LightRegion* pkRegion = m_pkTerrainLightField->GetRegion( fSampleS , fSampleT );
	if ( pkRegion )
	{
		m_fCharacterAmbientPower		= pkRegion->fCharacterAmbientPower;
		m_fCharacterSunPower			= pkRegion->fCharacterSunPower;
		m_fCharacterMatEmmisive			= pkRegion->fCharacterMatEmmisive;
		m_fCharacterSelfIllumination	= pkRegion->fCharacterSelfIllumination;
		m_fCharacterSpecularPower		= pkRegion->fCharacterSpecularPower;
		m_fCharacterSpecularMultiplier	= pkRegion->fCharacterSpecularMultiplier;
		m_kCharacterDiffuseLightColor	= pkRegion->vCharacterDiffuseLightColor;
		m_kCharacterAmbientColor		= pkRegion->vCharacterAmbientColor;
		m_kCharacterLightWorldDir		= pkRegion->vCharacterLightWorldDir;
		m_kCharacterRimLightColor		= pkRegion->vCharacterRimLightColor;
		m_fCharacterRimLightPower		= pkRegion->fCharacterRimLightPower;
		m_fCharacterIllumBias			= pkRegion->fCharacterIllumBias;
	}
}

void SE_SceneWorldMap::_updateTargetPointLight( float fSampleS, float fSampleT )
{
	LightRegion* pkRegion = m_pkTerrainLightField->GetRegion( fSampleS , fSampleT );
	if ( pkRegion )
	{
		m_kPointLightColor		 =  pkRegion->vPointLightColor;
		m_kPointLightParam		 =  pkRegion->vPointLightParam;
		m_kPointLightPos		 =	pkRegion->vPointLightPos;
		m_fPointLightEnable		 =  pkRegion->fPointLightEnable;
	}
}

void SE_SceneWorldMap::_updateTargetPaticleParams( float fSampleS, float fSampleT )
{
	LightRegion* pkRegion = m_pkTerrainLightField->GetRegion( fSampleS , fSampleT );
	if ( pkRegion )
	{
		m_fTargetParticleDensity = pkRegion->fParticleDensity;
		m_fTargetParticleSpeed =pkRegion->fParticleSpeed;
		m_fTargetParticleWidth = pkRegion->fParticleWidth;
		m_fTargetParticleLength = pkRegion->fParticleLength;
		m_fTargetParticleAlpha = pkRegion->fParticleAlpha;
		m_fTargetParticleWind = pkRegion->fParticleWind;
		m_bWeatherEnable = pkRegion->bParticleActive;
		m_kCurrentWeatherTex = pkRegion->strWeatherTexturePath;

        m_fCurrentRegionSceneLightningInterval = pkRegion->fDummyParams1;
        m_fCurrentRegionSceneLightningPower = pkRegion->fDummyParams2;
	}
}

void SE_SceneWorldMap::_updateTargetFinalEffectParams( float fSampleS, float fSampleT )
{
	LightRegion* pkRegion = m_pkTerrainLightField->GetRegion( fSampleS , fSampleT );
	if ( pkRegion )
	{
		m_fTargetSharpenIntensity = pkRegion->fSharpenIntensity;
        m_fTargetSoftLightScope = pkRegion->fSoftLightScope * (m_bIsCurrentInSceneLightning ? m_fCurrentSceneLightningPower : 1.0f);
		m_fTargetGammaIntensity = pkRegion->fGammaIntensity;
		m_fTargetGammaExponent = pkRegion->fGammaExponent;
		m_bBloomEnable = pkRegion->bFinalEffectActive;
	}
}

void SE_SceneWorldMap::_computeCurrentCharacterParam( float fLeftTime, float fDeltaTime )
{
	ComputeFloatFadeIn( m_fCurrentCharAmbientPower, m_fCharacterAmbientPower, m_fTransTimeLeft, fDeltaTime );
	ComputeFloatFadeIn( m_fCurrentCharSunPower, m_fCharacterSunPower, m_fTransTimeLeft, fDeltaTime );
	ComputeFloatFadeIn( m_fCurrentCharMatEmmisive, m_fCharacterMatEmmisive, m_fTransTimeLeft, fDeltaTime );
	ComputeFloatFadeIn( m_fCurrentCharSelfIllumination, m_fCharacterSelfIllumination, m_fTransTimeLeft, fDeltaTime );
	ComputeFloatFadeIn( m_fCurrentCharSpecularPower, m_fCharacterSpecularPower, m_fTransTimeLeft, fDeltaTime );
	ComputeFloatFadeIn( m_fCurrentCharSpecularMultiplier, m_fCharacterSpecularMultiplier, m_fTransTimeLeft, fDeltaTime );
	ComputeColorFadeIn( m_kCurrentCharDiffuseLightColor, m_kCharacterDiffuseLightColor, m_fTransTimeLeft, fDeltaTime );
	ComputeColorFadeIn( m_kCurrentCharAmbientColor, m_kCharacterAmbientColor, m_fTransTimeLeft, fDeltaTime );
	ComputeDirFadeIn( m_kCurrentCharLightWorldDir, m_kCharacterLightWorldDir, m_fTransTimeLeft, fDeltaTime );
	ComputeColorFadeIn( m_kCurrentCharRimLightColor, m_kCharacterRimLightColor, m_fTransTimeLeft, fDeltaTime );
	ComputeFloatFadeIn( m_fCurrentCharRimLightPower, m_fCharacterRimLightPower, m_fTransTimeLeft, fDeltaTime );
	ComputeFloatFadeIn( m_fCurrentCharIllumBias, m_fCharacterIllumBias, m_fTransTimeLeft, fDeltaTime );
}

void SE_SceneWorldMap::_computeCurrentPaticleParams( float fLeftTime, float fDeltaTime )
{
	ComputeFloatFadeIn( m_fCurrentParticleDensity, m_fTargetParticleDensity, m_fTransTimeLeft, fDeltaTime );
	ComputeFloatFadeIn( m_fCurrentParticleSpeed, m_fTargetParticleSpeed, m_fTransTimeLeft, fDeltaTime );
	ComputeFloatFadeIn( m_fCurrentParticleWidth, m_fTargetParticleWidth, m_fTransTimeLeft, fDeltaTime );
	ComputeFloatFadeIn( m_fCurrentParticleLength, m_fTargetParticleLength, m_fTransTimeLeft, fDeltaTime );
	ComputeFloatFadeIn( m_fCurrentParticleAlpha, m_fTargetParticleAlpha, m_fTransTimeLeft, fDeltaTime );
	ComputeFloatFadeIn( m_fCurrentParticleWind, m_fTargetParticleWind, m_fTransTimeLeft, fDeltaTime );
}

void SE_SceneWorldMap::_computeCurrentFinalEffectParams( float fLeftTime, float fDeltaTime )
{
	m_fCurrentSharpenIntensity = m_fTargetSharpenIntensity;
	//ComputeFloatFadeIn( m_fCurrentSharpenIntensity, m_fTargetSharpenIntensity, m_fTransTimeLeft, fDeltaTime );
	ComputeFloatFadeIn( m_fCurrentSoftLightScope, m_fTargetSoftLightScope, m_fTransTimeLeft, fDeltaTime );
	m_fCurrentGammaIntensity = m_fTargetGammaIntensity;
	m_fCurrentGammaExponent = m_fTargetGammaExponent;
	//GfxWriteLog( LOG_INFO_FILELINE , LOG_SWITCH_INFO ,
	//	"Post Effect Sharp %f, Blur %f , Exp %f , Power %f" ,
	//	m_fCurrentSharpenIntensity , m_fCurrentSoftLightScope , m_fCurrentGammaExponent , m_fCurrentGammaIntensity );
	//ComputeFloatFadeIn( m_fCurrentGammaIntensity, m_fTargetGammaIntensity, m_fTransTimeLeft, fDeltaTime );
	//ComputeFloatFadeIn( m_fCurrentGammaExponent, m_fTargetGammaExponent, m_fTransTimeLeft, fDeltaTime );
}

float SE_SceneWorldMap::GetCurrentWaterHeight( const NiPoint3& rkPlayer )
{
	m_fCurrentWaterHeight = 999999.0f;
	int nX = Utility::Float2Int( rkPlayer.x / WORLD_GRID_WIDTH );
	int nY = Utility::Float2Int( rkPlayer.y / WORLD_GRID_WIDTH );

	/// get当前的tile
	SE_SceneWorldTile* pTile = GetWorldTile(0 , 0);
	if (pTile)
	{
		SE_SceneWorldChunk* pChunk =   pTile-> GetChunkByCenter( rkPlayer );
		if (pChunk)
		{
			m_fCurrentWaterHeight =  pChunk->GetChunkWaterHeight();
			return m_fCurrentWaterHeight;
		}
	}

	if ( m_pkMapEffect == NULL )
	{
		return 0.0f;
	}
	int nCount = 0;
	for(int iIndexX = nX - 10; iIndexX < nX + 10; iIndexX+=4)
	{
		for(int iIndexY = nY - 10; iIndexY < nY + 10; iIndexY+=4)
		{
			float fHeight = 0;//m_pkMapEffect->GetWaterHeight( iIndexX , iIndexY );
			if( m_fCurrentWaterHeight > fHeight )
			{
				m_fCurrentWaterHeight = fHeight;
			}
			//nCount++;
		}
	}
	//m_fCurrentWaterHeight /= nCount;
	m_fCurrentWaterHeight = rkPlayer.z - 500.0f;
	return m_fCurrentWaterHeight;
}

void SE_SceneWorldMap::AddToLoadedArray( SE_SceneWorldChunk* pkChunk )
{
	m_kLoadedChunks.push_back( pkChunk );
}

void SE_SceneWorldMap::RemoveFromLoadedArray( SE_SceneWorldChunk* pkChunk )
{
	SE_WorldChunkList::iterator iter;
	TRAV( m_kLoadedChunks , iter )
	{
		if ( pkChunk == *iter )
		{
			m_kLoadedChunks.erase( iter );
			return;
		}
	}
}

void SE_SceneWorldMap::RegisterLocalNPC(long lSpawnPointID , 
										const NiPoint3& rkPos )
{
	int iTileX , iTileY;
	GetWhichTile( iTileX , iTileY , rkPos.x , rkPos.y );
	SE_SceneWorldTile* pkTile = GetWorldTile(iTileX,iTileY);
	if ( pkTile )
	{
		pkTile->RegisterLocalNPC( lSpawnPointID , rkPos );
	}
	else
	{
		GfxWriteLog( LOG_TAG_ERROR , LOG_SWITCH_ERROR ,
			"本地NPC坐标有误！[%d]:[%f %f %f]" , 
			lSpawnPointID , rkPos.x , rkPos.y  , rkPos.z );
	}
}

void SE_SceneWorldMap::GetCurrentRegionSceneLightningParams(float& fSLInterval, float& fSLPower)
{
    fSLInterval = m_fCurrentRegionSceneLightningInterval;
    fSLPower = m_fCurrentRegionSceneLightningPower;
}
