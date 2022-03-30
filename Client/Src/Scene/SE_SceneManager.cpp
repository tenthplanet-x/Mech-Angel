#include "ClientPCH.h"

#include "AS_ClientSoundManager.h"
#include "BL_AsyncOperationManager.h"
#include "CA_CameraManager.h"
#include "GD_ClientSceneModel.h"
#include "GD_DynamicCollisionManager.h"
#include "IM_Manager.h"
#include "LC_ClientLogicManager.h"
#include "LC_ClientLogicAreaManager.h"
#include "MG_MessageBase.h"
#include "MG_MessageCenter.h"
#include "RG_CustomeShadowGenerator.h"
#include "RG_RenderManager.h"
#include "SE_SceneManager.h"
#include "SE_SceneWorldMap.h"
#include "SE_WorldMiniMap.h"
#include "SE_WorldChunkGeometryDataManager.h"
#include "SE_SceneTerrainInfo.h"
#include "SE_TerrainCallback.h"
#include "TS_DynamicWeather.h"
#include "UT_CutSceneManager.h"
#include "CF_MapEffectConfig.h"
#include "CF_WorldMapList.h"
#include "WeatherManager.h"
#include "CA_CameraPathAnimation.h"
#include "CF_CameraCut.h"
#include "UT_Pick_Manager.h"

using namespace CSVFile;
using namespace Data;
using namespace GameLogic;
using namespace Path;
using namespace Terrain;
using namespace Utility;
//------------------------------------------------------
SE_SceneManager::SE_SceneManager()
: GSC_NAME_time("time")
, GSC_NAME_GroundAmbient("g_vGroundAmbient")
, GSC_NAME_AmbientLight("g_vAmbientLight")
, GSC_NAME_DepthFogParam("g_vDepthFogParam")
, GSC_NAME_HeightFogParam("g_vHeightFogParam")
, GSC_NAME_FogColor("g_vFogColor")
, GSC_NAME_DiffuseSunLightColor("g_vDiffuseSunLightColor")
, GSC_NAME_ShadowColor("g_vShadowColor")
, GSC_NAME_LightDir("g_vLightDir")
, GSC_NAME_ShadowIndoor("g_bShadowIndoor")
, GSC_NAME_ActorPlayerLocation("g_vActorPlayerLocation")
, GSC_NAME_SceneObjectSaturationInc("g_fSceneObjectSaturationInc")

, GSC_NAME_Sharp("g_fSharp")
, GSC_NAME_Blur("g_fBlur")
, GSC_NAME_Exp("g_fExp")
, GSC_NAME_Power("g_fPower")

, strCharacterAmbientPower("g_fCharacterAmbientPower")
, strCharacterSunPower("g_fCharacterSunPower")
, strCharacterMatEmmisive("g_fCharacterMatEmmisive")
, strCharacterSelfIllumination("g_fCharacterSelfIllumination")
, strCharacterSpecularPower("g_fCharacterSpecularPower")
, strCharacterSpecularMultiplier("g_fCharacterSpecularMultiplier")
, strCharacterDiffuseLightColor("g_vCharacterDiffuseLightColor")
, strCharacterAmbientColor("g_vCharacterAmbientColor")
, strCharacterLightWorldDir("g_vCharacterLightWorldDir")
, strCharacterRimLightColor("g_vCharacterRimLightColor")
, strCharacterRimLightPower("g_fCharacterRimLightPower")
, strCharacterIllumBias("g_fCharacterIllumBias")
, strPlayerPointLightPos("g_vPlayerPointLightPos")
, strPlayerPointLightColor("g_vPlayerPointLightColor")
, strPlayerPointLightParam("g_vPlayerPointLightParam")
, strPlayerPointLightEnable("g_fPlayerPointLightEnable")

, m_pkSceneWorldMap(NULL)
, m_pkWorldMiniMap(NULL)
, m_pkWorldChunkGeometryDataManager(NULL)

, m_kPreCuller(NULL)

, m_spShadowCullingProcess(NULL)
, m_spCurrentShadowGenerator(NULL)
, m_spDirShadowGenerator(NULL)
, m_pkNiScenePick(NULL)
, m_pkLastPickedNPC(NULL)
, m_spGrassInstanceTriShape(NULL)
, m_bLoadWorldMap(false)
, m_kPathMeshCuller(NULL)
#ifdef _ENABLE_PHYSX
, m_fLastPhyxUpdateTime(0.0f)
#endif
, m_fLastShadowUpdateTime(0.0f)
, m_fLastBlockModelUpdateTime(0.0f)
, m_pkWeatherManager(NULL)
, m_bUseExternalWeather(false)
, m_fLastLightUpdateTime(0.0f)
, m_bUseExternalPointLight(false)
, m_pkSkyBoxModel(NULL)
, HEIGHT_FIELD_GRID_SIZE(0)
, m_pkModelPick(NULL)
, m_bFirstStaticInFrame(false)
, m_bLoadingMap(false)
, m_fLastPhyxUpdateTime(0.0f)
{
	m_bEnableFog = true;
	m_bEnableWater = true;
	m_bEnableGrass = true;
	m_bEnblePathMeshShow = false;
	m_bEnableModelShow = true;
	m_bEnableSceneFxShow = true;
	m_bEnableTerrainShow = true;
	m_bUpdateLightPos = false;
	m_nCurrentMapID = -1;
	m_bFirstTerrainInFrame = true;
    m_bFirstModelInFrame = true;
	m_nCurrentShadowMode = INVALID_SHADOW;
	m_bValidFXWall = false;

	memset(m_iFXWallID, 0, sizeof(m_iFXWallID));

	//Weather
	m_pkWeatherManager = T_NEW_D TS_DynamicWeather;

#ifdef _ENABLE_PHYSX
	m_spPhysScene = NULL;
	m_pkPhysManager = NULL;
	m_pkPhysXLog = T_NEW_E( UT_PhysXLog , Memory::MEMCATEGORY_GENERAL );
	m_pkPhysXAlloc = T_NEW_E( UT_PhysXAllocator , Memory::MEMCATEGORY_GENERAL );
#endif
}
//------------------------------------------------------
SE_SceneManager::~SE_SceneManager()
{
	//由于物理需要最后释放，但是UnInit里面又需要释放地图
#ifdef _ENABLE_PHYSX
	if(g_settings.PHYSX_SWITCH)
		_unintPhysXManager();

 	T_DELETE_E( m_pkPhysXLog , UT_PhysXLog , Memory::MEMCATEGORY_GENERAL );
 	T_DELETE_E( m_pkPhysXAlloc , UT_PhysXAllocator , Memory::MEMCATEGORY_GENERAL );
#endif
}
//------------------------------------------------------
bool SE_SceneManager::Init()
{
	m_bEnableGrass = g_settings.LOD_ENABLE_GRASS_RENDER;

	StringType kGrassConfigFileName = GfxGetDirConfig().GetDir(CF_NODE_GRASS_CONFIG);

	GfxGetWorldConfig().InitShaderConfig();
	GfxGetWorldConfig().InitGrassConfig(kGrassConfigFileName);

	m_pkNiScenePick						= NiNew NiPick;
	m_pkNiScenePick->SetPickType(NiPick::FIND_ALL);
	m_pkNiScenePick->SetSortType(NiPick::SORT);
	m_pkNiScenePick->SetObserveAppCullFlag( false );
	m_pkNiScenePick->SetCoordinateType(NiPick::WORLD_COORDINATES);
	m_pkNiScenePick->SetIntersectType(NiPick::TRIANGLE_INTERSECT);
	m_pkNiScenePick->SetFrontOnly(false);

	m_pkModelPick						= NiNew NiPick;
	m_pkModelPick->SetPickType(NiPick::FIND_ALL);
	m_pkModelPick->SetSortType(NiPick::SORT);
	m_pkModelPick->SetCoordinateType(NiPick::WORLD_COORDINATES);
	m_pkModelPick->SetIntersectType(NiPick::BOUND_INTERSECT);
	m_pkModelPick->SetFrontOnly(false);
	
	if ( g_settings.CFG_ENABLE_WEATHER_EFFECT )
	{
		m_pkWeatherManager->Init();
	}

	m_pkSceneWorldMap = T_NEW_D SE_SceneWorldMap;
#ifdef _ENABLE_PHYSX
	if(g_settings.PHYSX_SWITCH)
		_initPhysXManager();
#endif
	m_bLoadingMap = false;
	_initShadowManager();

	CF_CameraCut::DataEntryMapExternIterator kIter = CF_CameraCut::GetSingletonPtr()->GetIterator();
	for( ; kIter.HasMoreElements(); kIter.MoveNext() )
	{
		const CF_CameraCut::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		Camera::CA_CameraAnimationManager::Instance()->LoadMax(pkEntry->_lID, pkEntry->_kResPath, pkEntry->_fTime, pkEntry->_kZAngle);
	}

	return true;
}
//------------------------------------------------------
bool SE_SceneManager::UnInit()
{
	if ( m_bLoadWorldMap )
	{
		UnLoadWorldMap();
	}
	_uninitShadowManager();
	m_pkWeatherManager->Shutdown();
	T_SAFE_DELETE( m_pkWeatherManager );

	T_SAFE_DELETE( m_pkSceneWorldMap );
	NiDelete m_pkNiScenePick;
	NiDelete m_pkModelPick;

    if (m_pkSkyBoxModel != NULL)
    {
        GD_ClientAsyncModelManager::GetSingleton().ReleaseModel((GD_ModelBase*)m_pkSkyBoxModel);
    }

	return true;
}
//------------------------------------------------------
bool SE_SceneManager::LoadWorldMap( const StringType& rkWorldName , 
								    const StringType& rkDecoration ,
								    long nMapID , long lR )
{ 
	WeatherManager * pkWeather = WeatherManager::Instance();
	if (pkWeather != NULL)
	{
		StringType configWeather = rkWorldName;
		configWeather += ".weather";
		if(!pkWeather->Open(configWeather))
		{
			pkWeather->SetWeather(WT_None);
		}
	}

	m_nCurrentMapID = nMapID;
	m_bLoadingMap = true;
	{
		T_PROFILE_TIMER( "载入地图" );
		{
			{
				T_PROFILE_TIMER( "载入LogicAreaData" );
				LC_ClientLogicMapManager * pkLogicMapMgr = LC_ClientLogicMapManager::GetSingletonPtr();
				pkLogicMapMgr->ClrLogicMapData();		// 只加载当前地图的LogicAreaData，其它的卸载
				pkLogicMapMgr->AddLogicMapData(nMapID);
			}
			{
				T_PROFILE_TIMER( "初始化地图" );
				T_ASSERT(!m_bLoadWorldMap && "Reload WorldMap");

				bool bLoad = m_pkSceneWorldMap->InitWorldMap( rkWorldName , rkDecoration, m_nCurrentMapID);
				T_ASSERT( bLoad );
				_initMapEffectConfig(nMapID);

			}
			{
				_switchShadowMode(OUTDOOR_DIRLIGHT_SHADOW);
			}

			CF_WorldMapList* pkCSVWorldMapList = CF_WorldMapList::GetSingletonPtr();
			if ( pkCSVWorldMapList )
			{
				CF_WorldMapList::DataEntry* pkData = pkCSVWorldMapList->GetEntryPtr(nMapID);
				if ( pkData && pkData->_kViewRadius > 0 )
				{
					g_settings.CAMERA_CLIP_FAR = pkData->_kViewRadius;
				}
				else
				{
					g_settings.CAMERA_CLIP_FAR = g_settings.GetDefaultFloatConfig( "CameraSetting.ClipPlane.Far" , 7000 );
				}

				
				if( CA_CameraManager::GetSingletonPtr() )
				{
                    if (g_settings.CFG_CAMERA_MODE == 1/*CA_CameraManager::CT_DRACONIS*/ 
                        && pkData /*&& pkData->_iInitCameraAngle >= 0*/)  //// @@@@@@依赖Gamelogic，所以屏蔽
                    {
                        //CA_CameraManager::GetSingleton().SetCameraType(CA_CameraManager::CT_DRACONIS);
                        //CA_CameraManager::GetSingleton().SetDraconisCameraOrientation(pkData->_iInitCameraAngle);  //// @@@@@@依赖Gamelogic，所以屏蔽
                    }
					CA_CameraManager::GetSingleton().SetFarClipDistance( g_settings.CAMERA_CLIP_FAR );
				}

				// 子弹碰撞用的场景数据
				if (pkData != NULL)
					UT_Pick_Manager::GetSingleton().LoadWorld(nMapID, pkData->_kSceneObjectCollision);
			}
			
			{
				T_PROFILE_TIMER( "载入寻路片" );
				PF_PathManager* pkPathManager = PF_PathManager::GetSingletonPtr();
				pkPathManager->LoadGroundMesh( rkWorldName , rkDecoration );
			}
			{
				T_PROFILE_TIMER( "初始化地形几何数据缓存" );
				m_pkWorldMiniMap					= T_NEW_D SE_WorldMiniMap(m_pkSceneWorldMap);
				m_pkWorldChunkGeometryDataManager	= T_NEW_D SE_WorldChunkGeometryDataManager(m_pkSceneWorldMap);

				_initGrassInstanceData();
				_attachMessageHandler(m_pkSceneWorldMap);
				_attachMessageHandler(m_pkWorldMiniMap);
				SendMessage( CreateMsg_TOGGLE_BACKLOADING( 1 , lR ) , true );
			}
			{
				Camera::CA_CameraAnimationManager::Instance()->Init();
				Camera::CA_CameraAnimationManager::Instance()->Load();
			}
#ifdef	_ENABLE_PHYSX
			m_kSceneMeshMap.clear();
#endif
			m_bLoadWorldMap = true;
		}
	}

    m_bUseExternalPointLight = false;

    HEIGHT_FIELD_GRID_SIZE = GfxGetWorldConfig().CFG_WORLD_GRID_WIDTH;

    GD_DynamicCollisionManager::GetSingleton().OnMapReloaded(true);
	
	//g_profilingMgr.SetProfileBreak( 125.0f );
	return true;
}
//------------------------------------------------------
bool SE_SceneManager::UnLoadWorldMap()
{
    //切换场景，需要先发消息终止后台加载
    //当场景加载到一半的时候就点进入游戏需要等旧地图已经卸载完了才开始加载新地图
    SendMessage(CreateMsg_TOGGLE_BACKLOADING(0, 0), true);

	if( !m_bLoadWorldMap)
		return false;

	_detachMessageHandler(m_pkWorldMiniMap);
	_detachMessageHandler(m_pkSceneWorldMap);

	// 清除阴影场景；
	ClearShadowGenerator();

	// 清除子弹碰撞用的场景数据
	UT_Pick_Manager::GetSingleton().Clear();

	_uninitGrassInstanceData();

	Camera::CA_CameraAnimationManager::Instance()->Shutdown();

	PF_PathManager::GetSingletonPtr()->UnLoadGroundMesh(  );

	// 这两个pick的result中会保存sceneWorldMap中对象的smartpointer，
	// 所以在sceneWorldMap清空之前，把这两个Pick的result先清空掉。
	if ( m_pkModelPick )
	{
		m_pkModelPick->RemoveTarget();
		m_pkModelPick->DeepClearResultsArray();
	}
	if ( m_pkNiScenePick )
	{
		m_pkNiScenePick->RemoveTarget();
		m_pkNiScenePick->DeepClearResultsArray();
	}

	if(m_pkSceneWorldMap)
	{
		m_pkSceneWorldMap->UnInitWorldMap();
	}
	if( m_pkWorldMiniMap )
	{
		T_DELETE_D m_pkWorldMiniMap;
		m_pkWorldMiniMap = NULL;
	}
	if( m_pkWorldChunkGeometryDataManager )
	{
		T_DELETE_D m_pkWorldChunkGeometryDataManager;
		m_pkWorldChunkGeometryDataManager = NULL;
	}

	m_bLoadWorldMap = false;

	return true;
}
//------------------------------------------------------
bool SE_SceneManager::RecvMessage( MG_MessageBase& rkMessage )
{
	MG_MessageHandlerInterface::RecvMessage(rkMessage);

	BEGIN_MESSAGE_HANDLE
	MESSAGE_HANDLE(MGT_TIMER_TICK,					_event_TimerTick)
	MESSAGE_HANDLE(MGT_LOGIC_PLAYER_MOVE,			_event_LogicPlayerMove)
	MESSAGE_HANDLE(MGT_INPUT_MOUSE_LBUTTON_DOWN,	_event_InputMouseLButtonDown)
	MESSAGE_HANDLE(MGT_INPUT_MOUSE_LBUTTON_DBCLK,	_event_InputMouseLButtonDBClk)
	MESSAGE_HANDLE(MGT_INPUT_MOUSE_LBUTTON_UP,		_event_InputMouseLButtonUp)
	MESSAGE_HANDLE(MGT_INPUT_MOUSE_RBUTTON_DOWN,	_event_InputMouseRButtonDown)
	MESSAGE_HANDLE(MGT_INPUT_MOUSE_RBUTTON_UP,		_event_InputMouseRButtonUp)
	MESSAGE_HANDLE(MGT_INPUT_MOUSE_MOVE,			_event_InputMouseMove)
	END_MESSAGE_HANDLE
	return true;
}

void SE_SceneManager::PreCullModels(void)
{
    T_PROFILE( "SE_SceneManager::PreCullModels"  );

    if (m_pkSceneWorldMap && m_bEnableModelShow)
    {
		NiCameraPtr spCamera = CA_CameraManager::GetSingleton().GetNiCamera();
        if (m_kSceneModelCache.GetCount() == 0)
        {
            m_pkSceneWorldMap->GetModelVisibleArray(spCamera, m_kPreCuller, m_kSceneModelCache, true);
        }
        if (m_kSceneWalkableModelCache.GetCount() == 0)
        {
            m_pkSceneWorldMap->GetWalkableModelVisibleArray(spCamera, m_kPreCuller, m_kSceneWalkableModelCache);
        }
        if (m_kSceneTransparentModelCache.GetCount() == 0)
        {
            m_pkSceneWorldMap->GetTransparentVisibleArray(spCamera, m_kPreCuller, m_kSceneTransparentModelCache);
        }
        m_bFirstModelInFrame = false;
    }
}

void SE_SceneManager::GetModelVisibleArray(NiGeometryList& rkCasterList)
{
    for (unsigned int i = 0; i < m_kSceneModelCache.GetCount(); ++i)
    {
        rkCasterList.AddTail(&m_kSceneModelCache.GetAt(i));
    }
    for (unsigned int i = 0; i < m_kSceneWalkableModelCache.GetCount(); ++i)
    {
        rkCasterList.AddTail(&m_kSceneWalkableModelCache.GetAt(i));
    }
    for (unsigned int i = 0; i < m_kSceneTransparentModelCache.GetCount(); ++i)
    {
        rkCasterList.AddTail(&m_kSceneTransparentModelCache.GetAt(i));
    }
}

void SE_SceneManager::GetTerrainVisibleArrary(NiCameraPtr spNiCamera, NiCullingProcess& rkCuller, NiVisibleArray& rkVisibleArray)
{
	T_PROFILE( "Cull::Terrain" );
	if(m_pkSceneWorldMap && m_bEnableTerrainShow )
	{	
		if (  m_bFirstTerrainInFrame )
		{
			m_bFirstTerrainInFrame = false;
			m_pkSceneWorldMap->GetTerrainVisibleArray(spNiCamera, rkCuller, m_kTerrainVisibleCache);
			
		}
		for ( unsigned int i = 0 ; i < m_kTerrainVisibleCache.GetCount() ; ++i )
		{
			rkVisibleArray.Add( m_kTerrainVisibleCache.GetAt( i ) );
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void SE_SceneManager::GetInfiniteModel( NiCameraPtr spNiCamera , NiVisibleArray& rkVisible )
{
	if (m_pkSkyBoxModel != NULL)
	{
		NiCullingProcess kCullingProcess(NULL);
		m_pkSkyBoxModel->Cull( spNiCamera , kCullingProcess , rkVisible);
	}

}
//------------------------------------------------------
void SE_SceneManager::GetModelVisibleArray(NiCameraPtr spNiCamera, NiCullingProcess& rkCuller, 
										   NiVisibleArray& rkVisibleArray)
{
	T_PROFILE("Cull::SceneModel");
	
	if (m_pkSceneWorldMap && m_bEnableModelShow)
	{
		if ( m_bFirstModelInFrame )
		{
			m_pkSceneWorldMap->GetModelVisibleArray(spNiCamera, rkCuller, m_kSceneModelCache, true);
            m_pkSceneWorldMap->GetWalkableModelVisibleArray(spNiCamera, rkCuller, m_kSceneWalkableModelCache);
		}

		for ( unsigned int i = 0 ; i < m_kSceneModelCache.GetCount() ; ++i )
		{
			rkVisibleArray.Add( m_kSceneModelCache.GetAt( i ) );
		}
        for (unsigned int i = 0; i < m_kSceneWalkableModelCache.GetCount(); ++i)
        {
            rkVisibleArray.Add(m_kSceneWalkableModelCache.GetAt(i));
        }
	}
}

void SE_SceneManager::GetShadowCasters(NiCameraPtr spNiCamera, NiCullingProcess& rkCuller, NiGeometryList& rkVisibleArray)
{

	m_kTempVisibles.RemoveAll();
	m_pkSceneWorldMap->GetModelVisibleArray(spNiCamera, rkCuller, m_kTempVisibles, true);
	m_pkSceneWorldMap->GetWalkableModelVisibleArray(spNiCamera, rkCuller, m_kTempVisibles);
	m_pkSceneWorldMap->GetTransparentVisibleArray(spNiCamera, rkCuller, m_kTempVisibles);
	for (unsigned int i = 0; i < m_kTempVisibles.GetCount(); i++)
	{
		rkVisibleArray.AddTail(&m_kTempVisibles.GetAt(i));
	}
}

void SE_SceneManager::GetWalkableModelVisibleArray(NiCameraPtr spNiCamera, 
                                                   NiCullingProcess& rkCuller, 
                                                   NiVisibleArray& rkVisibleArray)
{
    T_PROFILE("Cull::SceneModel");

    if (m_pkSceneWorldMap && m_bEnableModelShow)
    {
        T_ASSERT(!m_bFirstModelInFrame);
        if (m_bFirstModelInFrame)
        {
            m_pkSceneWorldMap->GetWalkableModelVisibleArray(spNiCamera, rkCuller, m_kSceneWalkableModelCache);
        }

        for (unsigned int i = 0; i < m_kSceneWalkableModelCache.GetCount(); ++i)
        {
            rkVisibleArray.Add(m_kSceneWalkableModelCache.GetAt(i));
        }
    }
}

void SE_SceneManager::GetSceneTransparentVisibleArray( NiCameraPtr spNiCamera, NiCullingProcess& rkCuller, NiVisibleArray& rkVisibleArray )
{
    T_PROFILE("Cull::SceneModel");

	if (m_pkSceneWorldMap && m_bEnableModelShow )
	{
        if ( m_bFirstModelInFrame )
        {
            m_pkSceneWorldMap->GetTransparentVisibleArray(spNiCamera, rkCuller, m_kSceneTransparentModelCache);
        }

        for ( unsigned int i = 0 ; i < m_kSceneTransparentModelCache.GetCount() ; ++i )
        {
            rkVisibleArray.Add( m_kSceneTransparentModelCache.GetAt( i ) );
        }
	}
}
//------------------------------------------------------
void SE_SceneManager::GetWaterVisibleArray(NiCameraPtr spNiCamera, 
										   NiCullingProcess& rkCuller, 
										   NiVisibleArray& rkVisibleArray,
										   int nCullChunk )
{
	T_PROFILE( "Cull::Water" );
	if(m_pkSceneWorldMap && m_bEnableWater )
	{
		//T_PROFILE_TIMER("Cull Water Visible Array");
		m_pkSceneWorldMap->GetWaterVisibleArray(spNiCamera, rkCuller, rkVisibleArray,nCullChunk);
	}
}
//------------------------------------------------------
void SE_SceneManager::GetSceneFxVisibleArray(NiCameraPtr spNiCamera, NiCullingProcess& rkCuller, NiVisibleArray& rkVisibleArray)
{
	T_PROFILE( "Cull::SceneFx" );
	if(m_pkSceneWorldMap && m_bEnableSceneFxShow )
	{
		//T_PROFILE_TIMER("Cull SceneFx Visible Array");
		m_pkSceneWorldMap->GetSceneFxVisibleArray(spNiCamera, rkCuller, rkVisibleArray);
	}
}

//------------------------------------------------------
void SE_SceneManager::GetReflectVisibleArray( NiCameraPtr spNiCamera, NiCullingProcess& rkCuller, NiVisibleArray& rkVisibleArray,bool bTerrain )
{
	T_PROFILE("Cull::Reflect");
	if(m_pkSceneWorldMap  && m_bEnableWater )
	{	
		m_pkSceneWorldMap->GetReflectVisibleArray(spNiCamera,rkCuller, rkVisibleArray,bTerrain);
	}
}
void SE_SceneManager::GetPathMeshVisibleArray( NiCameraPtr spNiCamera, NiVisibleArray& rkVisibleArray )
{
	T_PROFILE("Cull::PathMesh");
	PF_PathManager* pkPathMgr = PF_PathManager::GetSingletonPtr();
	if( m_bEnblePathMeshShow )
	{	
		if ( pkPathMgr )
		{
			pkPathMgr->LoadPathMeshRenderable();
			//pkPathMgr->SetPathMeshWireFrame_client(m_bRender_test);
			NiCullScene( spNiCamera , pkPathMgr->GetPathMeshRenderable() , m_kPathMeshCuller , rkVisibleArray );
		}
	}
}

//----------------------------------------------------------------------
void SE_SceneManager::AttachToLightEffect( NiNodePtr spAffectNode , bool bSceneFx )
{
	T_ASSERT(GfxIsMainThread());

	if(m_pkSceneWorldMap == NULL) return;

	if ( bSceneFx )
	{
		NiLightPtr spFxLight = NULL;
		if (m_nCurrentShadowMode == OUTDOOR_DIRLIGHT_SHADOW)
		{
			spFxLight = m_pkSceneWorldMap->GetSceneFxLight();
		}
		if ( spFxLight )
		{
			spFxLight->AttachAffectedNode( spAffectNode );
			spFxLight->Update( 0.0f );
		}
	}
	else
	{
		NiLightPtr spLight = NULL;
		if (m_nCurrentShadowMode == OUTDOOR_DIRLIGHT_SHADOW)
		{
			spLight = m_pkSceneWorldMap->GetOutDoorLight();
		}
		
		if (spLight && m_spCurrentShadowGenerator)
		{
			spLight->AttachAffectedNode(spAffectNode);
			m_spCurrentShadowGenerator->SetRenderViewsDirty(true);
		}
	}
}
//----------------------------------------------------------------------
void SE_SceneManager::DetachToLightEffect(NiNodePtr spUnAffectNode , bool bSceneFx )
{
	T_ASSERT(GfxIsMainThread());
	
	if (m_pkSceneWorldMap == NULL) return;

	if ( bSceneFx )
	{
		NiLightPtr spFxLight = NULL;
		if (m_nCurrentShadowMode == OUTDOOR_DIRLIGHT_SHADOW)
		{
			spFxLight = m_pkSceneWorldMap->GetSceneFxLight();
		}
		if ( spFxLight )
		{
			spFxLight->DetachAffectedNode( spUnAffectNode );
			spFxLight->Update( 0.0f );
		}
	}
	else
	{
		NiLightPtr spLight = NULL;
		if (m_nCurrentShadowMode == OUTDOOR_DIRLIGHT_SHADOW)
		{
			spLight = m_pkSceneWorldMap->GetOutDoorLight();
		}
		if (spLight && m_spCurrentShadowGenerator)
		{
			spLight->DetachAffectedNode(spUnAffectNode);
			m_spCurrentShadowGenerator->SetRenderViewsDirty(true);
		}
	}
}
//------------------------------------------------------
//void SE_SceneManager::AddUnShadowReceiveNiNode( NiNodePtr spUnShadowReceiveNiNode )
//{
//	T_ASSERT(GfxIsMainThread());
//	if(m_spShadowGenerator)
//		m_spShadowGenerator->AttachUnaffectedReceiverNode(spUnShadowReceiveNiNode);
//}
////------------------------------------------------------
//void SE_SceneManager::RemoveUnShadowReceiveNiNode( NiNodePtr spUnShadowReceiveNiNode )
//{
//	T_ASSERT(GfxIsMainThread());
//	if(m_spShadowGenerator)
//		m_spShadowGenerator->DetachUnaffectedReceiverNode(spUnShadowReceiveNiNode);
//}
//------------------------------------------------------
void SE_SceneManager::ClearShadowGenerator()
{
	T_ASSERT(GfxIsMainThread());
	if(m_spCurrentShadowGenerator)
	{
		m_spCurrentShadowGenerator->DetachAllUnaffectedCasterNodes();
		m_spCurrentShadowGenerator->DetachAllUnaffectedReceiverNodes();

		/* 
			在销毁场景物件前先用hardCode强制把shadowView清空一下，
		    如果找到更优雅的办法时候再替换。

			Add by xyd
		*/

		{
			NiShadowMap* shadowMap = NULL;
			Ni3DRenderView* pkShadowView = NULL;

			unsigned int shadowMapCount = 
				m_spCurrentShadowGenerator->GetShadowMapCount();

			for ( unsigned int i = 0; i < shadowMapCount; ++i ) 
			{
				shadowMap = m_spCurrentShadowGenerator->GetOwnedShadowMap(i);

				if ( shadowMap )
				{
					pkShadowView = NiDynamicCast(Ni3DRenderView,
						shadowMap->GetRenderClick()->GetRenderViews().GetHead());

					if ( pkShadowView )
						pkShadowView->RemoveAllScenes();
				}
			}
		}

		/*
			m_spCurrentShadowGenerator中的DynamicEffect其实就是outDoorLight或者是fxLight，
			之后的代码会释放这两个DynamicEffect中的AffectNode和unAffectNode，不需要由m_spCurrentShadowGenerator来释放。

			Add by xyd
		*/
	}

	NiLightPtr spLight = NULL;
	if (m_nCurrentShadowMode == OUTDOOR_DIRLIGHT_SHADOW)
	{
		spLight = m_pkSceneWorldMap->GetOutDoorLight();
	}
	if (spLight)
	{
		spLight->DetachAllAffectedNodes();
		spLight->DetachAllUnaffectedNodes();
	}

	NiLightPtr spFxLight = m_pkSceneWorldMap->GetSceneFxLight();
	if ( spFxLight )
	{
		spFxLight->DetachAllAffectedNodes();
		spFxLight->DetachAllUnaffectedNodes();
	}
}
//------------------------------------------------------
unsigned int SE_SceneManager::PerformGrassInstanceRendering()
{
	if(m_pkSceneWorldMap  && m_bEnableGrass )
	{
		return m_pkSceneWorldMap->PerformGrassInstanceRendering( m_kCurrentPlayerPos , m_spGrassInstanceTriShape,NiDX9Renderer::GetRenderer());
	}
	else
	{
		return 0;
	}
}
//------------------------------------------------------
const SE_SceneTerrainLocalInfo& SE_SceneManager::GetTerrainLocalInfo(float fLocationX,
                                                                     float fLocationY,
                                                                     bool bGetHeight /* = true */,
                                                                     bool bGetNoraml /* = false */)
{
	m_kTerrainLocalInfo._bGetHeight = bGetHeight;
	m_kTerrainLocalInfo._bGetNormal = bGetNoraml;
	if (m_bLoadWorldMap && m_pkSceneWorldMap)
	{
		m_pkSceneWorldMap->GetTerrainLocalInfo(m_pkNiScenePick, m_kTerrainLocalInfo, fLocationX, fLocationY);
	}

    return m_kTerrainLocalInfo;
}

const SE_SceneTerrainLocalInfo& SE_SceneManager::GetWalkableInfo(float fLocationX,
																	 float fLocationY,
																	 bool bGetHeight /* = true */,
																	 bool bGetNoraml /* = false */)
{
	m_kTerrainLocalInfo._bGetHeight = bGetHeight;
	m_kTerrainLocalInfo._bGetNormal = bGetNoraml;
	if (m_pkSceneWorldMap)
	{
		m_pkSceneWorldMap->GetWalkableInfo(m_kTerrainLocalInfo, fLocationX, fLocationY);
	}

	return m_kTerrainLocalInfo;
}
const SE_SceneTerrainLocalInfo& SE_SceneManager::GetWalkableInfo(float fLocationX,
																 float fLocationY,
																 float fPrevHeight,
																 bool bGetHeight /* = true */,
																 bool bGetNoraml /* = false */)
{
	m_kTerrainLocalInfo._bGetHeight = bGetHeight;
	m_kTerrainLocalInfo._bGetNormal = bGetNoraml;
	if (m_pkSceneWorldMap)
	{
		m_pkSceneWorldMap->GetWalkableInfo(m_kTerrainLocalInfo, fLocationX, fLocationY,fPrevHeight);
	}

	return m_kTerrainLocalInfo;
}

float SE_SceneManager::GetHeightFieldHeight(float fHeightFieldCoordX, float fHeightFieldCoordY)
{
    if (m_pkSceneWorldMap == NULL)
    {
        return 0.0f;
    }

    return m_pkSceneWorldMap->GetHeightFieldHeight((int)(fHeightFieldCoordX / HEIGHT_FIELD_GRID_SIZE), (int)(fHeightFieldCoordY / HEIGHT_FIELD_GRID_SIZE));
}
//------------------------------------------------------
void SE_SceneManager::_event_TimerTick(MG_MessageBase& rkMessage)
{
	T_PROFILE( "Update::SE_SceneManager" );

	float fTime = rkMessage.m_fParam1;
	float fDeltaTime = rkMessage.m_fParam2;
	
	if(m_bLoadWorldMap)
	{
		NiColorA kDepthFogParam = m_pkSceneWorldMap->GetDepthFogParam();
		NiColorA kHeightFogParam = m_pkSceneWorldMap->GetHeightFogParam();
		float fFarView = 7000.0f;
		NiCameraPtr spCamera = CA_CameraManager::GetSingleton().GetNiCamera();
		if ( spCamera )
		{
			fFarView = spCamera->GetViewFrustum().m_fFar;
		}
#if 0
		//对深度雾做一些处理来保证视野外的物件都是看不到的
		if ( kDepthFogParam.r > fFarView - 500.0f || kDepthFogParam.r < 100.0f )
		{
			kDepthFogParam.r = fFarView - 500.0f;
		}
		kDepthFogParam.g = fFarView;
		kDepthFogParam.b = 1.0f;
		if ( kDepthFogParam.g - kDepthFogParam.r != 0.0f )
		{
			kDepthFogParam.a = 1.0f / ( kDepthFogParam.g - kDepthFogParam.r );
		}
		else
		{
			kDepthFogParam.a = 1.0f;
		}
#endif
		if ( !m_bEnableFog )
		{
			kDepthFogParam.b = 0.0f;
			kHeightFogParam.b = 0.0f;
		}

        // 场景闪电参数
        bool bSceneLightning = false;
        float fSceneLightningPower = 1.0f;
        float fBackLightPower = 1.0f;
        if (m_pkWeatherManager != NULL)
        {
            fSceneLightningPower = m_pkWeatherManager->GetCurrentSceneLightningPower();
            if (fSceneLightningPower > 0.0f)
            {
                bSceneLightning = true;
                fBackLightPower = 0.0f;
            }
            else
            {
                fSceneLightningPower = 1.0f;
            }
        }
        m_pkSceneWorldMap->SetCurrentSceneLightningPower(bSceneLightning, fSceneLightningPower);

        const NiColor& kAmbientColor = m_pkSceneWorldMap->GetAmbientColor() / fSceneLightningPower;
        const NiColor& kFogColor = m_pkSceneWorldMap->GetFogColor();

        const NiColor& kShadowColor = m_pkSceneWorldMap->GetShadowColor() / fSceneLightningPower;
		const NiColor& rkSunColor = m_pkSceneWorldMap->GetOutDoorLight()->GetDiffuseColor();
        NiColorA kSunFinalColor = bSceneLightning 
            ? NiColorA::WHITE * fSceneLightningPower 
            : NiColorA(rkSunColor.r, rkSunColor.g, rkSunColor.b, 1.0f) * m_pkSceneWorldMap->GetLightMultiplier();

		NiPoint3 kActorPlayerLocation;
		CA_CameraManager::GetSingleton().GetCurrActorPlayerLocation( kActorPlayerLocation );

        UpdateSceneShaderGlobalParams(
            fTime, m_pkSceneWorldMap->GetBackLightColor() * fBackLightPower, NiColorA(kAmbientColor.r, kAmbientColor.g, kAmbientColor.b, 1.0f), 
            kDepthFogParam, kHeightFogParam, kFogColor, 
            kSunFinalColor, NiColorA(kShadowColor.r, kShadowColor.g, kShadowColor.b, 1.0f), 
            m_pkSceneWorldMap->GetOutDoorLight()->GetWorldDirection(), (m_nCurrentShadowMode == INDOOR_POINTLIGHT_SHADOW), 
            kActorPlayerLocation);
		if( RG_RenderManager::GetSingletonPtr() )
		{
			RG_RenderManager::GetSingleton().SetSceneBackgroundColor( kFogColor );
		}

        UpdateCharacterShaderGlobalParams(
            m_pkSceneWorldMap->GetCharacterAmbientPower(), m_pkSceneWorldMap->GetCharacterSunPower() * fSceneLightningPower, 
            m_pkSceneWorldMap->GetCharacterMatEmmisive() * fBackLightPower, m_pkSceneWorldMap->GetCharacterSelfIllumination() * fBackLightPower, 
            m_pkSceneWorldMap->GetCharacterSpecularPower(), m_pkSceneWorldMap->GetCharacterSpecularMultiplier(), 
            m_pkSceneWorldMap->GetCharacterDiffuseLightColor(), m_pkSceneWorldMap->GetCharacterAmbientColor(), 
            /*bSceneLightning ? */m_pkSceneWorldMap->GetOutDoorLight()->GetWorldDirection() /*: m_pkSceneWorldMap->GetCharacterLightWorldDir()*/, 
            m_pkSceneWorldMap->GetCharacterRimLightColor(), m_pkSceneWorldMap->GetCharacterRimLightPower(), 
            m_pkSceneWorldMap->GetCharacterIllumBias());

        if (!m_bUseExternalPointLight)
        {
            UpdatePlayerPointLightShaderGlobalParams(m_pkSceneWorldMap->GetPointLightEnable(), 
                m_pkSceneWorldMap->GetPointLightColor(), m_pkSceneWorldMap->GetPointLightPos(), m_pkSceneWorldMap->GetPointLightParam());
        }

		m_pkWorldMiniMap->Update();
#ifdef _ENABLE_PHYSX
		//if(fTime > m_fLastPhyxUpdateTime + 0.03f)
		if(g_settings.PHYSX_SWITCH)
			UpdatePhyx(fTime,true);
#endif

		if ( g_settings.CFG_ENABLE_WEATHER_EFFECT && m_pkWeatherManager )
		{
            if (!m_bUseExternalWeather)
            {
                m_kWeatherParam.fDensity = m_pkSceneWorldMap->GetParticleDensity();
                m_kWeatherParam.fSpeed = m_pkSceneWorldMap->GetParticleSpeed();
                m_kWeatherParam.fLength = m_pkSceneWorldMap->GetParticleLength();
                m_kWeatherParam.fWidth = m_pkSceneWorldMap->GetParticleWidth();
                m_kWeatherParam.fAlpha = m_pkSceneWorldMap->GetParticleAlpha();
                m_kWeatherParam.fWind = m_pkSceneWorldMap->GetParticleWind();
            }
			
			m_pkWeatherManager->SetParameter( m_kWeatherParam , 0.0f );
			//暂时用一张纹理，避免运行时在主线程加载纹理
			//m_pkWeatherManager->SetTexture( m_pkSceneWorldMap->GetWeatherTexturePath( ) );

			m_pkWeatherManager->Update( fTime, fDeltaTime );
		}

		bool bBloomEnable = m_pkSceneWorldMap->GetBloomEnable();
		float fSharp = 1.0f;
		float fBlur = 0.0f;
		float fExp = 1.0f;
		float fPower = 1.0f;
		if ( g_settings.CFG_ENABLE_POST_PROCESS && bBloomEnable )
		{
			fSharp  = m_pkSceneWorldMap->GetBloomSharp();
			fBlur   = m_pkSceneWorldMap->GetBloomBlur();
			fExp    = m_pkSceneWorldMap->GetBloomGammaExp();
			fPower  = m_pkSceneWorldMap->GetBloomGammaIntensity();
		}
		UpdatePostProcessShaderGlobalParams(fSharp, fBlur, fExp, fPower);

		float fShadowUpdate = g_settings.LOD_SHADOWMAP_UPDATE_FREQ;
		float fLightUpdate  = g_settings.LOD_SHADOWMAP_LIGHT_UPDATE;
		//if(fTime > m_fLastShadowUpdateTime + fShadowUpdate )
		if( m_spCurrentShadowGenerator )
		{
			m_spCurrentShadowGenerator->MarkShadowMapsDirty();
			m_fLastShadowUpdateTime = fTime;
			
            m_spShadowCullingProcess->SetFrustumFromCamera( CA_CameraManager::GetSingleton().GetNiCamera() );
			m_spShadowCullingProcess->SetCullingRange(
				m_kCurrentPlayerPos , g_settings.LOD_SHADOW_RENDER_DISTANCE );
		}
		if(fTime > m_fLastLightUpdateTime + fLightUpdate )
		{
			m_fLastLightUpdateTime = fTime;
			m_bUpdateLightPos = true;
		}
		else
		{
			m_bUpdateLightPos =false;
		}
	}
	if (m_pkSkyBoxModel != NULL)
	{
		m_pkSkyBoxModel->EntirelyUpdate(fTime);
	}
}
//------------------------------------------------------
void SE_SceneManager::_event_InputMouseLButtonDown( MG_MessageBase& rkMessage )
{
	if(TEST_MESSAGE_HANDLED(rkMessage))
		return;

	long iMousePosX = rkMessage.m_lParam1;
	long iMousePosY = rkMessage.m_lParam2;

	NiPoint3	kPickPos;
	bool bPick = this->PickScene(iMousePosX, iMousePosY, &kPickPos);
	if(bPick)
	{
		SE_SceneTerrainLocalInfo rkWalkableInfo = GetWalkableInfo( kPickPos.x , kPickPos.y , true );
		if ( rkWalkableInfo._fLocalHeight > kPickPos.z )
		{
			kPickPos.z = rkWalkableInfo._fLocalHeight;
		}
		SendMessage(CreateMsg_SCENE_L_BUTTON_PICK_TERRAIN(kPickPos.x,kPickPos.y,kPickPos.z, rkMessage.m_lParam3));
		MARK_MESSAGE_HANDLED(rkMessage);
	}
}
//------------------------------------------------------
void SE_SceneManager::_event_InputMouseLButtonDBClk( MG_MessageBase& rkMessage )
{
	if(TEST_MESSAGE_HANDLED(rkMessage))
		return;

	long iMousePosX = rkMessage.m_lParam1;
	long iMousePosY = rkMessage.m_lParam2;

	NiPoint3	kPickPos;
	bool bPick = this->PickScene(iMousePosX, iMousePosY, &kPickPos);
	if(bPick)
	{
		SE_SceneTerrainLocalInfo rkWalkableInfo = GetWalkableInfo( kPickPos.x , kPickPos.y , true );
		if ( rkWalkableInfo._fLocalHeight > kPickPos.z )
		{
			kPickPos.z = rkWalkableInfo._fLocalHeight;
		}
		SendMessage(CreateMsg_SCENE_L_BUTTON_DOUBLE_PICK_TERRAIN(kPickPos.x,kPickPos.y,kPickPos.z, rkMessage.m_lParam3));
		MARK_MESSAGE_HANDLED(rkMessage);
	}
}
//------------------------------------------------------
void SE_SceneManager::_event_InputMouseLButtonUp( MG_MessageBase& rkMessage )
{
	if(TEST_MESSAGE_HANDLED(rkMessage))
		return;

	long iMousePosX = rkMessage.m_lParam1;
	long iMousePosY = rkMessage.m_lParam2;

	NiPoint3 kPickPos;
	bool bPick = this->PickScene(iMousePosX,iMousePosY, &kPickPos);
	if(bPick)
	{
		SendMessage(CreateMsg_SCENE_L_BUTTON_RELEASE_TERRAIN(kPickPos.x,kPickPos.y,kPickPos.z, rkMessage.m_lParam3));
		MARK_MESSAGE_HANDLED(rkMessage);
	}
}
//------------------------------------------------------
void SE_SceneManager::_event_InputMouseRButtonDown( MG_MessageBase& rkMessage )
{
	if(TEST_MESSAGE_HANDLED(rkMessage))
		return;

	long iMousePosX = rkMessage.m_lParam1;
	long iMousePosY = rkMessage.m_lParam2;

	NiPoint3 kPickPos;
	bool bPick = this->PickScene(iMousePosX,iMousePosY, &kPickPos);
	if(bPick)
	{
		SE_SceneTerrainLocalInfo rkWalkableInfo = GetWalkableInfo( kPickPos.x , kPickPos.y , true );
		if ( rkWalkableInfo._fLocalHeight > kPickPos.z )
		{
			kPickPos.z = rkWalkableInfo._fLocalHeight;
		}

		SendMessage(CreateMsg_SCENE_R_BUTTON_PICK_TERRAIN(kPickPos.x,kPickPos.y,kPickPos.z, rkMessage.m_lParam3));
		MARK_MESSAGE_HANDLED(rkMessage);
	}
}
//------------------------------------------------------
void SE_SceneManager::_event_InputMouseRButtonUp( MG_MessageBase& rkMessage )
{
	if(TEST_MESSAGE_HANDLED(rkMessage))
		return;

	long iMousePosX = rkMessage.m_lParam1;
	long iMousePosY = rkMessage.m_lParam2;

	NiPoint3 kPickPos;
	bool bPick = this->PickScene(iMousePosX,iMousePosY, &kPickPos);
	if(bPick)
	{
		SendMessage(CreateMsg_SCENE_R_BUTTON_RELEASE_TERRAIN(kPickPos.x,kPickPos.y,kPickPos.z, rkMessage.m_lParam3));
		MARK_MESSAGE_HANDLED(rkMessage);
	}
}
//------------------------------------------------------
void SE_SceneManager::_event_InputMouseMove( MG_MessageBase& rkMessage )
{
	if(TEST_MESSAGE_HANDLED(rkMessage))
	{
		return;
	}

	//间隔一定时间pick
	//static float s_fLastPickTime = 0;
	//float fCurTime = GET_PROCESS_TIME;
	//if (fCurTime - s_fLastPickTime < 0.07f)
	//{
	//	return;
	//}
	//s_fLastPickTime = fCurTime;

	long iMousePosX = rkMessage.m_lParam1;
	long iMousePosY = rkMessage.m_lParam2;

	NiPoint3	kPickPos;
	bool bPick = this->PickScene(iMousePosX, iMousePosY, &kPickPos);
	if(bPick)
	{
		SendMessage(CreateMsg_SCENE_MOUSE_OVER(kPickPos.x,kPickPos.y,kPickPos.z, rkMessage.m_lParam3), true);
        MARK_MESSAGE_HANDLED(rkMessage);
	}
}
//------------------------------------------------------
void SE_SceneManager::_event_LogicPlayerMove( MG_MessageBase& rkMessage )
{
	NiPoint3 kPlayerPosition(rkMessage.m_fParam1, rkMessage.m_fParam2, rkMessage.m_fParam3);
    bool bPlayerMoved = (kPlayerPosition != m_kCurrentPlayerPos);

	m_kCurrentPlayerPos = kPlayerPosition;
	
	NiDirectionalLightPtr spSunLight = m_pkSceneWorldMap->GetOutDoorLight();
	NiDirectionalLightPtr spFxLight = m_pkSceneWorldMap->GetSceneFxLight();
	NiPointLightPtr spFxPL = m_pkSceneWorldMap->GetSceneFxPointLight();
	NiPointLightPtr spPointLight = m_pkSceneWorldMap->GetPointLight();
	if (spPointLight && m_bUpdateLightPos)
	{
		NiPoint3 pos = kPlayerPosition + m_pkSceneWorldMap->GetPointLightPos();
		spPointLight->SetTranslate(pos);
		spFxPL->SetTranslate(pos );
		spPointLight->Update(0.0f);
	}

	if (spSunLight && m_bUpdateLightPos )
	{
		const NiPoint3& kLightDir = spSunLight->GetWorldDirection();
		NiPoint3 kLightPos = NiPoint3::ZERO;
		if ( m_nCurrentMapID == 10 )
		{
			 kLightPos = kPlayerPosition - kLightDir * g_settings.SHADOW_LIGHT_OFFSET_LARGE;
		}
		else
		{
			 kLightPos = kPlayerPosition - kLightDir * g_settings.SHADOW_LIGHT_OFFSET;
		}
		spSunLight->SetTranslate( kLightPos );
		//#pragma NOTE("如果加入Update的线程池，那么这里的Update要删掉")
		spSunLight->Update(0.0f);
	}
	if (  spFxLight )
	{
		spFxLight->Update( 0.0f );
	}
	if ( spFxPL )
	{
		spFxPL->Update( 0.0f );
	}

	//set scene opacity
    NiCameraPtr spCurrentCamera = CA_CameraManager::GetSingleton().GetNiCamera();
    const NiPoint3& kCurrentCameraPos = spCurrentCamera->GetTranslate();
    float fCurrentTime = NiGetCurrentTimeInSec();
 /*   if (kCurrentCameraPos != m_kLastCameraPos
        && fCurrentTime > m_fLastBlockModelUpdateTime + g_settings.LOD_CAMERA_BLOCK_UPDATE_TIME)*/
    {
        m_kLastCameraPos = kCurrentCameraPos;
        m_fLastBlockModelUpdateTime = fCurrentTime;

	    NiPoint3 kPickRayOrigin(kPlayerPosition.x, kPlayerPosition.y, kPlayerPosition.z + 175.0f),
            kPickRayDir = spCurrentCamera->GetWorldDirection() * -1.0f;

	    NiPoint3 kMax , kMin;
	    float fDis = g_settings.LOD_SCENE_MODEL_TRANSPARENT_DISTANCE;
	    kMin.x = (m_kCurrentPlayerPos.x - fDis);
	    kMax.x = (m_kCurrentPlayerPos.x + fDis);
	    kMin.y = (m_kCurrentPlayerPos.y - fDis);
	    kMax.y = (m_kCurrentPlayerPos.y + fDis);
	    kMin.z =  m_kCurrentPlayerPos.z - fDis;
	    kMax.z =  m_kCurrentPlayerPos.z + fDis;
	    m_kPlayerAABB.SetMax( kMax );
	    m_kPlayerAABB.SetMin( kMin );
	    m_kPlayerAABB.Update();
	    m_kPlayerAABB.UpdateCorner();

	    m_pkSceneWorldMap->UpdateCameraBlockModels( m_pkModelPick , m_kPlayerAABB , kPickRayOrigin , kPickRayDir );
	}

    if (bPlayerMoved)
    {
	    if (m_pkSkyBoxModel != NULL && UT_CutSceneManager::GetSingletonPtr() )
	    {
		    m_pkSkyBoxModel->SetTranslate( m_kCurrentPlayerPos + 
			    NiPoint3( 0 , 0 , UT_CutSceneManager::GetSingleton().GetSkyDomeZOffset() ) );
	    }

        // 场景闪电
        if (m_pkWeatherManager != NULL)
        {
            float fSLInterval = -1.0f, fSLPower = 0.0f;
            m_pkSceneWorldMap->GetCurrentRegionSceneLightningParams(fSLInterval, fSLPower);
            m_pkWeatherManager->SetSceneLightningParams(fSLInterval, fSLPower);
        }
    }
}
//------------------------------------------------------
void SE_SceneManager::_switchShadowMode(int nMode)
{
	if (m_nCurrentShadowMode == nMode) return;
	
	m_nCurrentShadowMode = nMode;

	if (m_pkSceneWorldMap == NULL) return;

	if (m_spCurrentShadowGenerator != NULL)
	{
		ClearShadowGenerator();
		NiShadowManager::DeleteShadowGenerator(m_spCurrentShadowGenerator);
	}

	T_ASSERT(m_spDirShadowGenerator != NULL);
	if (m_nCurrentShadowMode == OUTDOOR_DIRLIGHT_SHADOW)
	{
		m_spDirShadowGenerator->SetActive( true );
		m_spCurrentShadowGenerator = m_spDirShadowGenerator;
	}
	else
	{
		T_ASSERT(false);
		m_spCurrentShadowGenerator = m_spDirShadowGenerator;
	}
	NiShadowManager::AddShadowGenerator(m_spCurrentShadowGenerator);

	UpdateShadowManager();

	// !!!hack!!! re-light the player when map reloaded for fixing the main actor shadow problem
	// main actor will not be reloaded when map changes
	GD_ClientActorModel* pkPlayerModel = IM_Manager::GetSingleton().GetActor((CLIENT_GET_LOGIC_MANAGER())->GetOperationPlayerID());
	if (pkPlayerModel != NULL)
	{
		pkPlayerModel->ReattachToLightEffect();
	}
	if (m_nCurrentShadowMode == OUTDOOR_DIRLIGHT_SHADOW)
	{
		NiDirectionalLightPtr spFxLight = m_pkSceneWorldMap->GetSceneFxLight();
		spFxLight->AssignShadowGenerator( m_spFxLightShadowGenerator ); 
	}
}
//------------------------------------------------------
void SE_SceneManager::_initShadowManager()
{
	if (m_pkSceneWorldMap == NULL)
    {
        return;
    }

	NiCameraPtr spCamera = CA_CameraManager::GetSingleton().GetNiCamera();
	NiLightPtr spSunLight = m_pkSceneWorldMap->GetOutDoorLight();
	NiLightPtr spPointLight = m_pkSceneWorldMap->GetPointLight();
	NiLightPtr spFxLight = m_pkSceneWorldMap->GetSceneFxLight();

	m_spShadowCullingProcess = NiNew UT_FakeCullingProcess(&m_kShadowVisibleArray);
	m_spShadowCullingProcess->SetFrustumFromCamera( spCamera );
	// Initialize shadow click generator and active it.
	m_spShadowClickGenerator = NiNew RG_CustomeShadowClickGenerator;
	m_spShadowClickGenerator->SetName( "RG_CustomeShadowClickGenerator" );
	NiShadowManager::RegisterShadowClickGenerator( m_spShadowClickGenerator );
	bool bResult = NiShadowManager::SetActiveShadowClickGenerator(	"RG_CustomeShadowClickGenerator");
	NiShadowManager::SetSceneCamera(spCamera);
	NiShadowManager::SetCullingProcess(m_spShadowCullingProcess);
	NiShadowManager::SetMaxActiveShadowGenerators(30);

	m_spDirShadowGenerator = NiNew RG_CustomeShadowGenerator(spSunLight);
	m_spFxLightShadowGenerator = NiNew NiShadowGenerator( spFxLight );
	
	NiShadowTechnique* pkShadowTechnique = NiShadowManager::GetActiveShadowTechnique( g_settings.SHADOW_TECH );
	m_spDirShadowGenerator->SetShadowTechnique(pkShadowTechnique);
	m_spDirShadowGenerator->SetSizeHint(32);

	m_kCurrentPlayerPos = NiPoint3::ZERO;
    m_kLastCameraPos = NiPoint3::ZERO;

	m_spFxLightShadowGenerator->SetSMTextureAssignmentCallbackFunc(
		NiShadowGenerator::SingleShadowMapAssignmentCallback);
	m_spFxLightShadowGenerator->SetSMTextureRetrievalCallbackFunc(
		NiShadowGenerator::SingleShadowMapRetrievalCallback);
	m_spFxLightShadowGenerator->SetOwnedSMCallbackFunc(
		NiShadowGenerator::SingleShadowMapGetOwnedCallback);
	m_spFxLightShadowGenerator->SetSMCountCallbackFunc(
		NiShadowGenerator::SingleShadowMapCountCallback);
	m_spFxLightShadowGenerator->SetSMConstantAssignmentCallbackFunc(NULL);
	m_spFxLightShadowGenerator->SetSMConstantRetrievalCallbackFunc(
		NiShadowGenerator::DefaultGetShaderConstantCallback);
	m_spFxLightShadowGenerator->SetExtraDataCleanupCallbackFunc(
		NiShadowGenerator::SingleShadowMapCleanupCallback);
}
//------------------------------------------------------
void SE_SceneManager::UpdateShadowManager()
{
    if (m_spCurrentShadowGenerator != NULL)
    {
	    NiShadowTechnique* pkShadowTechnique = NiShadowManager::GetActiveShadowTechnique( g_settings.SHADOW_TECH );

	    m_spCurrentShadowGenerator->SetShadowTechnique(pkShadowTechnique);

	    m_spCurrentShadowGenerator->SetSizeHint(g_settings.SHADOW_SIZE);
	    //直接使用灯光的位置，不自动计算
	    m_spCurrentShadowGenerator->SetAutoCalcDirLightFrusturmPosition(false);
	    //不自动计算灯光视椎体宽度
	    m_spCurrentShadowGenerator->SetAutoCalcDirLightFrusturmWidth(false);
	    m_spCurrentShadowGenerator->SetAutoCalcNearClippingDist(false);
	    m_spCurrentShadowGenerator->SetAutoCalcFarClippingDist(false);
	    m_spCurrentShadowGenerator->SetRenderBackfaces(true);
	    m_spCurrentShadowGenerator->SetDepthBias( 0.001f );
	    //暂时为南疆做特殊设置，以后改到表里面去
		//暂时为新手副本阴影做特殊设置，以后改到表里面去 -- ayu
	    if ( m_nCurrentMapID == 10 )
	    {
		    m_spCurrentShadowGenerator->SetNearClippingDist( g_settings.SHADOW_FRUSTUM_PARAM_LARGE.x );
		    m_spCurrentShadowGenerator->SetFarClippingDist( g_settings.SHADOW_FRUSTUM_PARAM_LARGE.y );
		    m_spCurrentShadowGenerator->SetDirLightFrusturmWidth( g_settings.SHADOW_FRUSTUM_PARAM_LARGE.z );
	    }
	    else
	    {
		    m_spCurrentShadowGenerator->SetNearClippingDist( g_settings.SHADOW_FRUSTUM_PARAM.x );
		    m_spCurrentShadowGenerator->SetFarClippingDist( g_settings.SHADOW_FRUSTUM_PARAM.y );
		    m_spCurrentShadowGenerator->SetDirLightFrusturmWidth( g_settings.SHADOW_FRUSTUM_PARAM.z );
	    }
    }
}

//------------------------------------------------------
void SE_SceneManager::_uninitShadowManager()
{
	m_spCurrentShadowGenerator = NULL;
	m_spDirShadowGenerator = NULL;
	m_spFxLightShadowGenerator = NULL;
}
//----------------------------------------------------------------------
void SE_SceneManager::_initGrassInstanceData()
{
#ifdef _DX9
	int nSize = 256;
#else
	int nSize = GfxGetWorldConfig().CFG_PLANTMAPSIZE *  GfxGetWorldConfig().CFG_PLANTMAPSIZE;
#endif

	NiPoint3* pkVertex = NiNew NiPoint3[nSize * 4];		// 一个矩形4个顶点
	NiPoint2* pkTexC = NiNew NiPoint2[nSize * 4 * 2];	// 4个顶点UV，正反面
	NIASSERT(pkVertex);
	NIASSERT(pkTexC);
	unsigned short* pusConnect = (unsigned short*)NiMalloc(		// 顶点索引
		sizeof(unsigned short) * (nSize * 2 * 3));
	NIASSERT(pusConnect);

	//Temp Init 
	for(int i = 0; i < nSize; i++)
	{
		// 顶点坐标
		pkVertex[i*4] = NiPoint3(-0.5,0,0);
		pkVertex[i*4 + 1] = NiPoint3(-0.5,0,1);
		pkVertex[i*4 + 2] = NiPoint3(0.5,0,1);
		pkVertex[i*4 + 3] = NiPoint3(0.5,0,0);

		//int kk = rand() % 4;

		// 正面UV坐标
		pkTexC[i*4] = NiPoint2(0,1);
		pkTexC[i*4 + 1] = NiPoint2(0,0);
		pkTexC[i*4 + 2] = NiPoint2(1,0);
		pkTexC[i*4 + 3] = NiPoint2(1,1);

		// 反面UV坐标
		pkTexC[i*4 + nSize * 4] = NiPoint2((float)i,0.0f);   //Index And ....
		pkTexC[i*4 + 1 + nSize * 4] = NiPoint2((float)i,1.0f);
		pkTexC[i*4 + 2 + nSize * 4] = NiPoint2((float)i,1.0f);
		pkTexC[i*4 + 3 + nSize * 4] = NiPoint2((float)i,0.0f);

		// Triangle Index，Triangle List
		pusConnect[i * 6] = i*4;
		pusConnect[i * 6 + 1] = i*4 + 1;
		pusConnect[i * 6 + 2] = i*4 + 2;

		pusConnect[i * 6 + 3] = i*4 + 2;
		pusConnect[i * 6 + 4] = i*4 + 3;
		pusConnect[i * 6 + 5] = i*4;
	}

	m_spGrassInstanceTriShape = NiNew NiInstanceTriShape(nSize * 4,
		pkVertex,NULL,NULL,pkTexC,2,NiGeometryData::NBT_METHOD_NONE,
		nSize * 2,pusConnect);
	m_spGrassInstanceTriShape->SetShader(GfxGetWorldConfig().GetGrassShader());

	NiTexturingPropertyPtr spTextureProperty = NiNew NiTexturingProperty;
	m_spGrassInstanceTriShape->AttachProperty(spTextureProperty);

	m_spGrassRoot = NiNew NiNode;
	m_spGrassRoot->AttachChild(m_spGrassInstanceTriShape);

	AttachToLightEffect(m_spGrassRoot,false);

	m_spGrassRoot->Update(0.0f);
	m_spGrassRoot->UpdateEffects();
	m_spGrassRoot->UpdateProperties();
}
//------------------------------------------------------
void SE_SceneManager::_uninitGrassInstanceData()
{
	DetachToLightEffect(m_spGrassRoot,false);
	m_spGrassInstanceTriShape = NULL;
	m_spGrassRoot =NULL;
}
//------------------------------------------------------
#ifdef _ENABLE_PHYSX 
void SE_SceneManager::_initPhysXManager()
{
	m_pkPhysManager = NiPhysXManager::GetPhysXManager();
#ifdef _SHIPPING
	if (!m_pkPhysManager->Initialize( NULL , m_pkPhysXLog ))
#else
	if (!m_pkPhysManager->Initialize( m_pkPhysXAlloc , m_pkPhysXLog ))
#endif
	{
		//T_ASSERT(false && "Phyx Init Error");
		GfxWriteLog( LOG_TAG_CLIENT_CRITICAL , LOG_SWITCH_CLIENT_CRITICAL ,"can't init physx engine" );
		g_settings.PHYSX_SWITCH = false;
		return;
	}
	m_spPhysScene = NiNew NiPhysXScene();

	m_pkPhysManager->WaitSDKLock();
	m_pkPhysManager->m_pkPhysXSDK->setParameter(NX_SKIN_WIDTH, 0.05f);
//#ifndef _SHIPPING
//	m_pkPhysManager->m_pkPhysXSDK->getFoundationSDK().getRemoteDebugger()->connect ("localhost", 5425);
//#endif

	NxSceneDesc kSceneDesc;
	kSceneDesc.gravity = NxVec3(0.0f,0.0f, -3560.0f);
	kSceneDesc.simType = NX_SIMULATION_SW;

	NxScene* pkNxScene = m_pkPhysManager->m_pkPhysXSDK->createScene(kSceneDesc);
	if( pkNxScene == NULL)
	{
		GfxWriteLog( LOG_TAG_CLIENT_CRITICAL , LOG_SWITCH_CLIENT_CRITICAL ,
			"physx scene init failed" );
		g_settings.PHYSX_SWITCH = false;
		m_spPhysScene = NULL;
		m_pkPhysManager->Shutdown();
		m_pkPhysManager = NULL;
		return;
	}

	m_spPhysScene->SetScaleFactor(1.0f);
	m_spPhysScene->SetPhysXScene(pkNxScene);

	NxMaterial* pkDefaultMaterial = pkNxScene->getMaterialFromIndex(0);
	pkDefaultMaterial->setRestitution(0.0f);
	pkDefaultMaterial->setStaticFriction(0.3f);
	pkDefaultMaterial->setDynamicFriction(0.3f);

	pkNxScene->setGroupCollisionFlag(1,1,false);

	m_pkPhysManager->ReleaseSDKLock();
	
	m_spPhysScene->SetDoFixedStep(true);
	m_spPhysScene->SetTimestep(1.0f / 100.0f);
	m_spPhysScene->SetNumSubSteps(2);
	m_spPhysScene->SetMaxSubSteps(16);

	m_spPhysScene->SetUpdateDest(true);
	m_spPhysScene->SetUpdateSrc(true);
	m_spPhysScene->UpdateDestinations(0.0f, true);
	m_spPhysScene->UpdateSources(0.0f, true);
	m_spPhysScene->Simulate( 0.0 , true );
}
//------------------------------------------------------
void SE_SceneManager::_unintPhysXManager()
{
	if (NULL != m_spPhysScene)
	{
		m_spPhysScene->FetchResults(NI_INFINITY, true);
		m_spPhysScene = NULL;
	}

	if (NULL != m_pkPhysManager)
	{
		m_pkPhysManager->Shutdown();
		m_pkPhysManager = NULL;
	}
}
#endif
//------------------------------------------------------
bool SE_SceneManager::PickVerticalHeight( long lX , long lY , long& rlZ )
{
	//只需要Pick当前点所在的Chunk
	T_PROFILE("SE_SceneManager::PickVerticalHeight");
	rlZ = 0.0f;

	return (m_pkSceneWorldMap != NULL) && m_pkSceneWorldMap->PickVertical( m_pkNiScenePick , lX , lY , rlZ );
}
//------------------------------------------------------
#ifdef _ENABLE_PHYSX
bool SE_SceneManager::AddPhyxProp(NiPhysXPropPtr spProp)
{
    _beginPhysxLock();
	if ( !g_settings.PHYSX_SWITCH)
	{
		 _endPhysxLock();
		return false;
	}
    {
        m_spPhysScene->AddProp(spProp);

        NiPhysXPropDesc* pkPropDesc = spProp->GetSnapshot();
        int nActors = pkPropDesc->GetActorCount();

        for (int i = 0; i < nActors; ++i)
        {
            NiPhysXActorDesc* pkActorDesc = pkPropDesc->GetActorAt(i);
            NxActor* pkNxActor = pkActorDesc->GetActor();

            if (pkNxActor == NULL)
            {
                continue;
            }

            NxU32 nbShapes = pkNxActor->getNbShapes();
            NxShape*const* apkNxShapes = pkNxActor->getShapes();

            while (nbShapes--)
            {
                apkNxShapes[nbShapes]->setGroup(1);
            }
        }
    }
    _endPhysxLock();
	return true;
}
//------------------------------------------------------
void SE_SceneManager::RemovePhyxProp( NiPhysXPropPtr spProp )
{
	_beginPhysxLock();
	if ( !g_settings.PHYSX_SWITCH )
	{
		_endPhysxLock();
		return;
	}
    {
        m_spPhysScene->RemoveProp(spProp);
    }
	_endPhysxLock();
}
//------------------------------------------------------
void SE_SceneManager::AddPhyxCollisionSphere(const NiPoint3& rkCenter, float fRadius)
{
	_beginPhysxLock();
	if ( !g_settings.PHYSX_SWITCH )
	{
		_endPhysxLock();
		return;
	}
	{
		NxSphereShapeDesc sphereDesc;
		sphereDesc.radius = fRadius;

        NxActorDesc actorDesc;
		actorDesc.shapes.pushBack(&sphereDesc);
		actorDesc.density       = 10.0f;
		actorDesc.globalPose.t  = NxVec3(rkCenter.x, rkCenter.y, rkCenter.z);

		m_spPhysScene->GetPhysXScene()->createActor(actorDesc);
	}
	_endPhysxLock();
}
//------------------------------------------------------
NxActor* SE_SceneManager:: AddPhyxCollisionSceneChunk(const Utility::UT_PhysXMemStream& rkMemStream )
{
	//T_PROFILE_TIMER("create physx pkNxActor");
	if ( !g_settings.PHYSX_SWITCH )
	{
		return NULL;
	}
	while(!m_spPhysScene->GetPhysXScene()->isWritable())
	{
		Sleep(1);
	}
	NxActor* pkActor = NULL;
	m_pkPhysManager->WaitSDKLock();
	NxScene* pkNxScene = m_spPhysScene->GetPhysXScene();

	NxTriangleMesh* pkSceneMesh = NULL ;

	pkSceneMesh = m_pkPhysManager->m_pkPhysXSDK->createTriangleMesh(rkMemStream);
	if ( pkSceneMesh )
	{
		NxTriangleMeshShapeDesc bunnyShapeDesc;    
		bunnyShapeDesc.meshData= pkSceneMesh; 
		NxActorDesc actorDesc;    
		actorDesc.shapes.pushBack(&bunnyShapeDesc);   
		actorDesc.density= 1.0f;    
		pkActor = pkNxScene->createActor(actorDesc);   

		bool bResult = m_kSceneMeshMap.insert( std::make_pair( pkActor , pkSceneMesh ) ).second;
		T_ASSERT( bResult );
	}

	m_pkPhysManager->ReleaseSDKLock();
	return pkActor;
}
//------------------------------------------------------
void SE_SceneManager::RemovePhyxCollisionSceneChunk( NxActor* pkActor )
{
	_beginPhysxLock();
	if ( !g_settings.PHYSX_SWITCH )
	{
		_endPhysxLock();
		return;
	}
    {
        m_spPhysScene->GetPhysXScene()->releaseActor(*pkActor);
		NxSceneMeshMapIter iter = m_kSceneMeshMap.find( pkActor );
		if ( iter != m_kSceneMeshMap.end() )
		{
			m_pkPhysManager->m_pkPhysXSDK->releaseTriangleMesh( *( iter->second ) );
			m_kSceneMeshMap.erase(iter);
		}
    }
	_endPhysxLock();
}
#endif 
//------------------------------------------------------
MTF_EffectTile::AreaEffectType SE_SceneManager::GetLocationAreaEffect(const Utility::UT_Vec3Int& kLocation)
{
	if ( m_pkSceneWorldMap && m_bLoadWorldMap )
	{
		return m_pkSceneWorldMap->GetAreaEffectType( kLocation );
	}
	return MTF_EffectTile::AET_UNKNOWN;
}

void SE_SceneManager::SetPlayerPointLightParams(const NiColor& kColor, const NiPoint3& kPos, const NiPoint3& kParam)
{
    T_ASSERT(m_bUseExternalPointLight);

    UpdatePlayerPointLightShaderGlobalParams(1.0f, kColor, kPos, kParam);
}

void SE_SceneManager::ReloadSkyBox(void)
{
    if (m_pkSkyBoxModel != NULL)
    {
        GD_ClientAsyncModelManager::GetSingleton().ReleaseModel((GD_ModelBase*)m_pkSkyBoxModel);
    }
	if ( UT_CutSceneManager::GetSingletonPtr() && !UT_CutSceneManager::GetSingleton().GetSkyDomeName().empty() )
	{
		m_pkSkyBoxModel = (GD_ClientSceneModel*)GD_ClientAsyncModelManager::GetSingleton().CreateModel(
			GD_ClientAsyncModelManager::AMT_SCENE, UT_CutSceneManager::GetSingleton().GetSkyDomeName() );
	}
	else
	{
		m_pkSkyBoxModel = NULL;
	}
}

void SE_SceneManager::UpdateSceneShaderGlobalParams(float fTime, const NiColor& kGroundAmbient, const NiColorA& kAmbientLight, 
                                                    const NiColorA& kDepthFogParam, const NiColorA& kHeightFogParam, const NiColor& kFogColor, 
                                                    const NiColorA& kDiffuseSunLightColor, const NiColorA& kShadowColor, const NiPoint3& kLightDir, bool bShadowIndoor, 
                                                    const NiPoint3& kActorPlayerLocation)
{
    NiShaderFactory::UpdateGlobalShaderConstant(GSC_NAME_time,                  sizeof(float),      &fTime);
    NiShaderFactory::UpdateGlobalShaderConstant(GSC_NAME_GroundAmbient,         sizeof(NiColor),    &kGroundAmbient);
    NiShaderFactory::UpdateGlobalShaderConstant(GSC_NAME_AmbientLight,          sizeof(NiColorA),   &kAmbientLight);
    NiShaderFactory::UpdateGlobalShaderConstant(GSC_NAME_DepthFogParam,         sizeof(NiColorA),   &kDepthFogParam);
    NiShaderFactory::UpdateGlobalShaderConstant(GSC_NAME_HeightFogParam,        sizeof(NiColorA),   &kHeightFogParam);
    NiShaderFactory::UpdateGlobalShaderConstant(GSC_NAME_FogColor,              sizeof(NiColor),    &kFogColor);
    NiShaderFactory::UpdateGlobalShaderConstant(GSC_NAME_DiffuseSunLightColor,  sizeof(NiColorA),   &kDiffuseSunLightColor);
    NiShaderFactory::UpdateGlobalShaderConstant(GSC_NAME_ShadowColor,           sizeof(NiColorA),   &kShadowColor);
    NiShaderFactory::UpdateGlobalShaderConstant(GSC_NAME_LightDir,              sizeof(NiPoint3),   &kLightDir);
    NiShaderFactory::UpdateGlobalShaderConstant(GSC_NAME_ShadowIndoor,          sizeof(bool),       &bShadowIndoor);
    NiShaderFactory::UpdateGlobalShaderConstant(GSC_NAME_ActorPlayerLocation,   sizeof(NiPoint3),   &kActorPlayerLocation);
}

void SE_SceneManager::UpdateCharacterShaderGlobalParams(float fAmbientPower, float fSunPower, float fMatEmmisive, 
                                                        float fSelfIllumination, float fSpecularPower, float fSpecularMultiplier, 
                                                        const NiColor& kDiffuseLightColor, const NiColor& kAmbientColor, const NiPoint3& kLightWorldDir, 
                                                        const NiColor& kRimLightColor, float fRimLightPower, float fIllumBias)
{
    NiShaderFactory::UpdateGlobalShaderConstant(strCharacterAmbientPower,       sizeof(float),      &fAmbientPower);
    NiShaderFactory::UpdateGlobalShaderConstant(strCharacterSunPower,           sizeof(float),      &fSunPower);
    NiShaderFactory::UpdateGlobalShaderConstant(strCharacterMatEmmisive,        sizeof(float),      &fMatEmmisive);
    NiShaderFactory::UpdateGlobalShaderConstant(strCharacterSelfIllumination,   sizeof(float),      &fSelfIllumination);
    NiShaderFactory::UpdateGlobalShaderConstant(strCharacterSpecularPower,      sizeof(float),      &fSpecularPower);
    NiShaderFactory::UpdateGlobalShaderConstant(strCharacterSpecularMultiplier, sizeof(float),      &fSpecularMultiplier);
    NiShaderFactory::UpdateGlobalShaderConstant(strCharacterDiffuseLightColor,  sizeof(NiColor),    &kDiffuseLightColor);
    NiShaderFactory::UpdateGlobalShaderConstant(strCharacterAmbientColor,       sizeof(NiColor),    &kAmbientColor);
    NiShaderFactory::UpdateGlobalShaderConstant(strCharacterLightWorldDir,      sizeof(NiPoint3),   &kLightWorldDir);
    NiShaderFactory::UpdateGlobalShaderConstant(strCharacterRimLightColor,      sizeof(NiColor),    &kRimLightColor);
    NiShaderFactory::UpdateGlobalShaderConstant(strCharacterRimLightPower,      sizeof(float),      &fRimLightPower);
    NiShaderFactory::UpdateGlobalShaderConstant(strCharacterIllumBias,          sizeof(float),      &fIllumBias);
}

void SE_SceneManager::GetCharacterShaderGlobalParams(float& fAmbientPower, float& fSunPower, float& fMatEmmisive, 
									float& fSelfIllumination, float& fSpecularPower, float& fSpecularMultiplier, 
									NiColor& kDiffuseLightColor, NiColor& kAmbientColor, NiPoint3& kLightWorldDir, 
									NiColor& kRimLightColor, float& fRimLightPower, float& fIllumBias)

{	
	const void* gsEntityData = NULL;
	unsigned int gsEntryDataSize = 0;

	fAmbientPower = 0.0f;
	NiShaderFactory::RetrieveGlobalShaderConstant(strCharacterAmbientPower, gsEntryDataSize, gsEntityData);
	if ( gsEntryDataSize >= sizeof(float)) fAmbientPower = *((float*)gsEntityData);

	fSunPower = 0.0f;
	NiShaderFactory::RetrieveGlobalShaderConstant(strCharacterSunPower,gsEntryDataSize, gsEntityData);
	if ( gsEntryDataSize >= sizeof(float)) fSunPower = *((float*)gsEntityData);

	fMatEmmisive = 0.0f;
	NiShaderFactory::RetrieveGlobalShaderConstant(strCharacterMatEmmisive, gsEntryDataSize, gsEntityData);
	if ( gsEntryDataSize >= sizeof(float)) fMatEmmisive = *((float*)gsEntityData);

	fSelfIllumination = 0.0f;
	NiShaderFactory::RetrieveGlobalShaderConstant(strCharacterSelfIllumination, gsEntryDataSize, gsEntityData);
	if ( gsEntryDataSize >= sizeof(float)) fSelfIllumination = *((float*)gsEntityData);

	fSpecularPower = 0.0f;
	NiShaderFactory::RetrieveGlobalShaderConstant(strCharacterSpecularPower, gsEntryDataSize, gsEntityData);
	if ( gsEntryDataSize >= sizeof(float)) fSpecularPower = *((float*)gsEntityData);

	fSpecularMultiplier = 0.0f;
	NiShaderFactory::RetrieveGlobalShaderConstant(strCharacterSpecularMultiplier, gsEntryDataSize, gsEntityData);
	if ( gsEntryDataSize >= sizeof(float)) fSpecularMultiplier = *((float*)gsEntityData);

	kDiffuseLightColor = NiColor::BLACK;
	NiShaderFactory::RetrieveGlobalShaderConstant(strCharacterDiffuseLightColor, gsEntryDataSize, gsEntityData);
	if ( gsEntryDataSize >= sizeof(NiColor) ) kDiffuseLightColor = *((NiColor*)gsEntityData);

	kAmbientColor = NiColor::BLACK;
	NiShaderFactory::RetrieveGlobalShaderConstant(strCharacterAmbientColor, gsEntryDataSize, gsEntityData);
	if ( gsEntryDataSize >= sizeof(NiColor)) kAmbientColor = *((NiColor*)gsEntityData);

	kLightWorldDir = NiPoint3(0.0f, 0.0f, 0.0f);
	NiShaderFactory::RetrieveGlobalShaderConstant(strCharacterLightWorldDir, gsEntryDataSize, gsEntityData);
	if ( gsEntryDataSize >= sizeof(NiPoint3)) kLightWorldDir = *((NiPoint3*)gsEntityData);

	kRimLightColor = NiColor::BLACK;
	NiShaderFactory::RetrieveGlobalShaderConstant(strCharacterRimLightColor, gsEntryDataSize, gsEntityData);
	if ( gsEntryDataSize >= sizeof(NiColor)) kRimLightColor = *((NiColor*)gsEntityData);

	fRimLightPower = 0.0f;
	NiShaderFactory::RetrieveGlobalShaderConstant(strCharacterRimLightPower, gsEntryDataSize, gsEntityData);
	if ( gsEntryDataSize >= sizeof(float)) fRimLightPower = *((float*)gsEntityData);

	fIllumBias = 0.0f;
	NiShaderFactory::RetrieveGlobalShaderConstant(strCharacterIllumBias, gsEntryDataSize, gsEntityData);
	if ( gsEntryDataSize >= sizeof(float)) fIllumBias = *((float*)gsEntityData);
}

void SE_SceneManager::UpdatePlayerPointLightShaderGlobalParams(float fPLEnable, 
                                                               const NiColor& kPLColor, const NiPoint3& kPLPos, const NiPoint3& kPLParam)
{
    NiShaderFactory::UpdateGlobalShaderConstant(strPlayerPointLightEnable, sizeof(float), &fPLEnable);
    if (fPLEnable != 0.0f)
    {
        NiShaderFactory::UpdateGlobalShaderConstant(strPlayerPointLightPos,     sizeof(NiPoint3),   &kPLPos);
        NiShaderFactory::UpdateGlobalShaderConstant(strPlayerPointLightColor,   sizeof(NiColor),    &kPLColor);

        float vPLParam[4];
        vPLParam[0] = kPLParam.x;
        vPLParam[1] = kPLParam.y;
        vPLParam[2] = kPLParam.z;
        vPLParam[3] = kPLParam.y - kPLParam.x;
        NiShaderFactory::UpdateGlobalShaderConstant(strPlayerPointLightParam,   sizeof(float)*4,    vPLParam);
    }
}

void SE_SceneManager::UpdatePostProcessShaderGlobalParams(float fSharp, float fBlur, float fExp, float fPower)
{
    NiShaderFactory::UpdateGlobalShaderConstant(GSC_NAME_Sharp, sizeof(float),  &fSharp);
    NiShaderFactory::UpdateGlobalShaderConstant(GSC_NAME_Blur,  sizeof(float),  &fBlur);
    NiShaderFactory::UpdateGlobalShaderConstant(GSC_NAME_Exp,   sizeof(float),  &fExp);
    NiShaderFactory::UpdateGlobalShaderConstant(GSC_NAME_Power, sizeof(float),  &fPower);
}
#ifdef _ENABLE_PHYSX
void SE_SceneManager::_beginPhysxLock()
{	
    while (m_spPhysScene && !m_spPhysScene->GetPhysXScene()->isWritable())
    {
        Sleep(1);
    }
	if ( m_pkPhysManager )
	{
		m_pkPhysManager->WaitSDKLock();
	}
}	
//------------------------------------------------------
void SE_SceneManager::_endPhysxLock()
{
	if ( m_pkPhysManager )
	{
		m_pkPhysManager->ReleaseSDKLock();
	}
}
//------------------------------------------------------------------------------
void SE_SceneManager::UpdatePhyx(float fCurrentTime,bool bForce)
{
	T_PROFILE_EX("Update::Phyx",3.0f);
	if ( !g_settings.PHYSX_SWITCH )
	{
		return; 
	}
	BeginPhyxUpdateLock();
	bool bFinish = m_spPhysScene->FetchResults( fCurrentTime);
	if ( bFinish )
	{
		m_spPhysScene->UpdateDestinations( fCurrentTime , bForce );
		m_spPhysScene->Simulate(fCurrentTime,bForce);
	}
	EndPhyxUpdateLock();

	m_fLastPhyxUpdateTime = fCurrentTime;
}
//------------------------------------------------------------------------------
void SE_SceneManager::UpdatePhyxSource(float fCurrentTime)
{
	if ( m_spPhysScene )
	{
		m_spPhysScene->UpdateSources(fCurrentTime,true);
	}
}
#endif



void SE_SceneManager::SetFogShow( bool bShow )
{
	m_bEnableFog = bShow;
}

void SE_SceneManager::SetWaterShow( bool bShow )
{
	m_bEnableWater = bShow;
}

void SE_SceneManager::SetModelShow( bool bShow )
{
	m_bEnableModelShow = bShow;
}

void SE_SceneManager::SetShadowShow( bool bShow )
{
//这个函数的 true 或者 false 会影响翅膀是不是歪的， 如果为true 就是正的， 如果为false 翅膀就会歪90度
	NiShadowManager::SetActive( bShow , false );
}

void SE_SceneManager::SetGrassShow( bool bShow )
{
	m_bEnableGrass = bShow;
}
#ifdef _ENABLE_PHYSX
void SE_SceneManager::BeginPhyxUpdateLock()
{
	m_kPhyxUpdateLock.Lock();
}

void SE_SceneManager::EndPhyxUpdateLock()
{
	m_kPhyxUpdateLock.Unlock();
}
#endif

NiColor SE_SceneManager::GetCurrentShadowColor(void)
{
    if (m_pkSceneWorldMap == NULL)
    {
		return NiColor::WHITE;
    }

    return m_pkSceneWorldMap->GetShadowColor();
}

float SE_SceneManager::QueryVisibilityMap( const NiPoint3& kPos )
{
	if( !m_bLoadWorldMap)
		return 1.0f;
    SE_ActorShadowField* pkVisibilityField = NULL;
	if(m_pkSceneWorldMap != NULL && (pkVisibilityField = m_pkSceneWorldMap->GetVisibilityField()) != NULL)
    {
        return pkVisibilityField->GetShadowAmount(kPos.x, kPos.y);
    }
	else
    {
        return 1.0f;
    }
}

float SE_SceneManager::GetCurrentWaterHeight()
{
	if ( m_pkSceneWorldMap == NULL )
	{
		return 0.0f;
	}
	return m_pkSceneWorldMap->GetCurrentWaterHeight( m_kCurrentPlayerPos );
}


void SE_SceneManager::MarkVisibleArrayDirty()
{
	m_bFirstTerrainInFrame = true;
    m_bFirstModelInFrame = true;

    m_kSceneTransparentModelCache.RemoveAll();
    m_kSceneWalkableModelCache.RemoveAll();
	m_kSceneModelCache.RemoveAll();
	m_kTerrainVisibleCache.RemoveAll();
}

void SE_SceneManager::StartForcibleWeather(float fDensity, float fSpeed, float fLength, float fWidth, float fAlpha, float fWind)
{
    m_kWeatherParam.fDensity = fDensity;
    m_kWeatherParam.fSpeed = fSpeed;
    m_kWeatherParam.fLength = fLength;
    m_kWeatherParam.fWidth = fWidth;
    m_kWeatherParam.fAlpha = fAlpha;
    m_kWeatherParam.fWind = fWind;
    //switch (eForcibleWeatherType)
    //{
    //case FWT_LIGHT_RAIN:
    //	break;
    //case FWT_MODERATE_RAIN:
    //    break;
    //case FWT_HEAVY_RAIN:
    //    break;
    //case FWT_LIGHT_SNOW:
    //    break;
    //case FWT_MODERATE_SNOW:
    //    break;
    //case FWT_HEAVY_SNOW:
    //    break;
    //default:
    //    return;
    //}
    m_bUseExternalWeather = true;
}

void SE_SceneManager::StopForcibleWeather(void)
{
    m_bUseExternalWeather = false;
}

void SE_SceneManager::SetTerrainShow( bool bShow )
{
	m_bEnableTerrainShow = bShow;
}

void SE_SceneManager::SetSceneFxShow( bool bShow )
{
	m_bEnableSceneFxShow = bShow;
}

void SE_SceneManager::SetFxWallShow( bool bShow )
{
	m_bValidFXWall = bShow;
}

void SE_SceneManager::SetFxWallID( int iIndex, int iID )
{
	if (iIndex < 0 || iIndex >= MAX_FX_WALL_NUM)
		return;

	m_iFXWallID[iIndex] = iID;
}

int SE_SceneManager::GetFxWallID( int iIndex )
{
	if (iIndex < 0 || iIndex >= MAX_FX_WALL_NUM)
		return 0;

	return m_iFXWallID[iIndex];
}

bool SE_SceneManager::PickScene( const NiPoint3& rkPos , const NiPoint3& rkDir , NiPoint3 * pkPickPos )
{
	//出于效率的考虑，在pick的时候，传入当前玩家的位置
	//可以认为Pick的范围是不会超过当前玩家位置一定范围的区域
	T_PROFILE_EX("SE_SceneManager::PickScene1",1.0f);

	if ( m_pkSceneWorldMap == NULL )
	{
		return false;
	}
	bool bPick = m_pkSceneWorldMap->PickMap(m_pkNiScenePick,rkPos,rkDir,m_kCurrentPlayerPos);
	if(bPick && pkPickPos != NULL)
	{
		*pkPickPos = m_pkNiScenePick->GetResults().GetAt(0)->GetIntersection();
	}
	return bPick;
}
bool SE_SceneManager::PickScene( long lWx, long lWy, NiPoint3 * pkPickPos )
{
	T_PROFILE_EX("SE_SceneManager::PickScene2",1.0f);

	if ( m_pkSceneWorldMap == NULL )
	{
		return false;
	}

	bool bPick = false;
	NiPoint3 kPickRayOrigin,kPickRayDir;
	NiCameraPtr spCurrentCamera = CA_CameraManager::GetSingleton().GetNiCamera();
	bool bRay = spCurrentCamera->WindowPointToRay( lWx, lWy ,kPickRayOrigin, kPickRayDir );	// kPickRayOrigin is cameraPos
	if (bRay)
	{
		float fPickHeight = m_kCurrentPlayerPos.z + 300.0f;
		if(UT_MathBase::Abs(kPickRayDir.z) > 0.0f && fPickHeight < kPickRayOrigin.z)
		{
			float fPickerMoveDistance = (fPickHeight - kPickRayOrigin.z) / kPickRayDir.z;		//为防止PickRay检测到镜头和人物中间的对象,将PickRayOrigin沿着PickRay前移到人物头顶某高度
			NiPoint3 kNearPickOrigin = kPickRayOrigin + kPickRayDir * fPickerMoveDistance;		//此点称为kNearPickOrigin，再做检测
			bPick = m_pkSceneWorldMap->PickMap(m_pkNiScenePick,kNearPickOrigin,kPickRayDir,m_kCurrentPlayerPos);
		}

		if (!bPick)
			bPick = m_pkSceneWorldMap->PickMap(m_pkNiScenePick,kPickRayOrigin,kPickRayDir,m_kCurrentPlayerPos);

		if(bPick && pkPickPos != NULL)
		{
			*pkPickPos = m_pkNiScenePick->GetResults().GetAt(0)->GetIntersection();
		}
	}

	return bPick;
}

void SE_SceneManager::RegisterLocalNpcToLocation(long lLocalNpcID, const Utility::UT_Vec3Int& kLocation)
{
	if ( m_pkSceneWorldMap && m_bLoadWorldMap )
	{
		m_pkSceneWorldMap->RegisterLocalNPC( lLocalNpcID , 
			Utility::UT_EngineMath::ConvertVec3IntToNiPoint3( kLocation ) );
	}
}

void SE_SceneManager::_initMapEffectConfig( long nMapID )
{
	CF_MapEffectConfig* pkMapEffectCfg = CF_MapEffectConfig::GetSingletonPtr();
	CF_MapEffectConfig::DataEntry* pkEffectCfg = pkMapEffectCfg->GetEntryPtr( nMapID );

	// 水面渲染
	bool bMapEnableWater = true;
	if ( pkEffectCfg )
	{
		bMapEnableWater = pkEffectCfg->_bEnableWater;
	}
	m_bEnableWater = bMapEnableWater;

	// 物理
	int nMaxPhsxActor = 5;
	if ( pkEffectCfg )
	{
		nMaxPhsxActor = pkEffectCfg->_ePhysXType;

	}
	g_settings.LOD_MAX_PHYSX_ACTOR = nMaxPhsxActor;

	// 反射
	static bool s_b_reflect = g_settings.LOD_ENABLE_REFLECT;
	if ( pkEffectCfg )
	{
		g_settings.LOD_ENABLE_REFLECT = pkEffectCfg->_bEnableReflect;
	}
	else
	{
		g_settings.LOD_ENABLE_REFLECT = s_b_reflect;
	}

	// 阴影
	bool bShadow = g_settings.CFG_ENABLE_SCENE_OBJECT_SHADOW;
	if ( bShadow )
	{
		SetShadowShow( true );
	}
	else
	{
		SetShadowShow( false );
	}

    // 场景闪电
    if (m_pkWeatherManager != NULL)
    {
        m_pkWeatherManager->SetSceneLightningParams(-1.0f, 0.0f);
    }
	// 场景物件饱和度调整
	int iSaturationInc = pkEffectCfg ? pkEffectCfg->_iSceneObjectSaturationAdjust : 0;
	float fSaturationInc = iSaturationInc / 100.0f;
	NiShaderFactory::UpdateGlobalShaderConstant(GSC_NAME_SceneObjectSaturationInc, sizeof(float), &fSaturationInc);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifdef _ENABLE_PHYSX
void UT_PhysXLog::reportError( NxErrorCode code, const char *message, const char *file, int line )
{
#ifndef _SHIPPING
	char* pcErrorText = "";
	switch (code)
	{
	case NXE_NO_ERROR:
		pcErrorText = "No Error";
		break;
	case NXE_INVALID_PARAMETER:
		pcErrorText = "Invalid Parameter";
		break;
	case NXE_INVALID_OPERATION:
		pcErrorText = "Invalid Operation";
		break;
	case NXE_OUT_OF_MEMORY:
		pcErrorText = "Out of Memory";
		break;
	case NXE_INTERNAL_ERROR:
		pcErrorText = "Internal Error";
		break;
	case NXE_ASSERTION:
		pcErrorText = "Assertion";
		break;
	case NXE_DB_INFO:
		pcErrorText = "Debug Info";
		break;
	case NXE_DB_WARNING:
		pcErrorText = "Debug Warning";
		break;
	case NXE_DB_PRINT:
		pcErrorText = "Debug Print";
		break;
	}
	unsigned short usLength = strlen(pcErrorText) + strlen(message)
		+ strlen(file) + 40;
	char* pcString = (char*)NiMalloc(usLength);

	NiSprintf(pcString, usLength, "PhysX Error: %s: %s in %s line %d\n",
		pcErrorText, message, file, line);


	GfxWriteLog( LOG_ERROR_FILELINE , LOG_SWITCH_ERROR ,
		"%s" , pcString );

	NiFree(pcString);
#endif
}

NxAssertResponse UT_PhysXLog::reportAssertViolation( const char *message, const char *file, int line )
{
	GfxWriteLog( LOG_ERROR_FILELINE , LOG_SWITCH_ERROR ,
		" PhysX Assert Violation:%s , File %s,Line %d" , message , file , line );
#ifndef _SHIPPING
	T_ASSERT(false);
#endif
	 return NX_AR_CONTINUE;
}

void UT_PhysXLog::print( const char *message )
{
#ifndef _SHIPPING
	GfxWriteLog( LOG_WARNING_FILELINE , LOG_SWITCH_ERROR ,
		"%s" , message );
#endif
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------
void* UT_PhysXAllocator::malloc(NxU32 size)
{
	return Memory::BaseAllocPolicy::allocateBytes(size, false,
		__FILE__,__LINE__,__FUNCTION__ );
}
//---------------------------------------------------------------------------
void* UT_PhysXAllocator::mallocDEBUG(NxU32 size,
									const char* fileName, int line)
{
	return Memory::BaseAllocPolicy::allocateBytes(size, false,
		fileName,line,__FUNCTION__ );
}    
//---------------------------------------------------------------------------
void* UT_PhysXAllocator::realloc(void* ptr, NxU32 nsize)
{
	return Memory::BaseAllocPolicy::reallocBytes(ptr, nsize ,
		false, __FILE__,__LINE__,__FUNCTION__ );
}   
//---------------------------------------------------------------------------
void UT_PhysXAllocator::free(void* ptr)
{
	Memory::BaseAllocPolicy::deallocateBytes(ptr);
}
#endif