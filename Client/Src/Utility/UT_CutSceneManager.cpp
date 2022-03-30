#include "ClientPCH.h"
#include "IM_Manager.h"
#include "UT_CutSceneManager.h"

#include "CA_CameraManager.h"

#include "SE_SceneManager.h"

#include "UT_GlobalSettings.h"

using namespace Utility;

void UT_CutSceneManager::Init()
{
#ifndef _SHIPPING
#ifndef _CLIENT_MT
#ifndef _DEBUG
	m_kFbxMgr.Init();
#endif
#endif
#endif
	m_bPlay = false;
	m_fLastTime = 0.0f;
	m_fSkyDomeZ = 0.0f;
	m_bPause = false;
	m_fPlayTime = 0.0f;
	m_kSkyDomeName = "";
	m_bShowGamePlayerModel = true;
	m_bShowNPCModel = true;
	m_bFirstUpdate = false;

	m_bPlayerName = true;
	m_bPlayerBlood = true;
	m_bNPCBlood = true;
	m_bUECost = true;//显示关闭冒血,攻击，被攻击受伤害
	m_bShowUE = true;//关闭所有UE,不显示玩家名称，玩家血条等等
	m_bShowUI = true;//关闭所有UI，游戏中当前的UI界面不显示
	m_bShowSelectionFx = true;
	m_fFreeMoveSpeed = 50.0f;
	m_fFreeRotSpeed = 0.01f;
    m_lFreeCameraFollowPlayerID = 10000001;
    m_bIsPlayerMoveDisabled = false;
    m_kPlayerLocation = NiPoint3::ZERO;

	ReloadConfig();
}

void UT_CutSceneManager::Shutdown()
{
#ifndef _SHIPPING
#ifndef _CLIENT_MT
#ifndef _DEBUG
	m_kFbxMgr.Shutdown();
#endif
#endif
#endif
}

void UT_CutSceneManager::ReloadConfig()
{
	if ( g_settings.CFG_ENABLE_CUTSCENE )
	{
		UT_ConfigFile kConfig;
		kConfig.Load( "Data/Config/CutScene.ini" );

		m_nLoadingWidth = UT_BaseStringUtility::GetIntFromString( kConfig , "Config" , "Chunk" , 1 );
		m_fFarClip = UT_BaseStringUtility::GetFloatFromString( kConfig , "Config" , "Far" , 7000 );
		m_fNearClip = UT_BaseStringUtility::GetFloatFromString( kConfig , "Config" , "Near" , 10 );
		m_fFreeMoveSpeed = UT_BaseStringUtility::GetFloatFromString( kConfig , "Config" , "FreeCameraMoveSpeed" , 50.0f );
		m_fFreeRotSpeed = UT_BaseStringUtility::GetFloatFromString( kConfig , "Config" , "FreeCameraRotateSpeed" , 0.01f );
        m_lFreeCameraFollowPlayerID = UT_BaseStringUtility::GetIntFromString(kConfig, "Config", "FreeCameraFollowPlayerID", 10000001);

		m_kFbxFileName = kConfig.GetSetting( "SaveFbxName" , "Config" , "1.fbx" );
		m_kCameraAnimName = kConfig.GetSetting( "CameraAnimationFile" , "Config" , "1.nif" );
		m_kSkyDomeName = kConfig.GetSetting( "SkyDomeFile" , "Config" , "1.nif" );
		m_fSkyDomeZ = UT_BaseStringUtility::GetFloatFromString( kConfig , "Config" , "SkyDomeZTranslate" , 0.0f );

		m_bPlayerName = UT_BaseStringUtility::GetIntFromString( kConfig, "Config","PlayName" , 1 ) ? true : false;
		m_bPlayerBlood = UT_BaseStringUtility::GetIntFromString( kConfig,  "Config" , "PlayBlood" ,1 ) ? true : false;
		m_bNPCBlood = UT_BaseStringUtility::GetIntFromString( kConfig,  "Config" ,"NPCBlood" , 1 ) ? true : false;
		m_bUECost = UT_BaseStringUtility::GetIntFromString( kConfig, "Config" ,"UECost" ,  1 ) ? true : false;
		m_bShowUE = UT_BaseStringUtility::GetIntFromString( kConfig, "Config" ,"ShowUE" ,  1 ) ? true : false;
		m_bShowUI = UT_BaseStringUtility::GetIntFromString( kConfig, "Config" ,"ShowUI" ,  1 ) ? true : false;
		m_bShowSelectionFx = UT_BaseStringUtility::GetIntFromString(kConfig, "Config" ,"ShowSelectionFx",  1) ? true : false;

		m_bShowGamePlayerModel = UT_BaseStringUtility::GetIntFromString( kConfig, "Config" ,"ShowGamePlayer" ,  1 ) ? true : false;
		m_bShowNPCModel = UT_BaseStringUtility::GetIntFromString(kConfig, "Config" ,"ShowNPC",  1) ? true : false;

		bool bCameraBlock =  UT_BaseStringUtility::GetIntFromString( kConfig, "Config" ,"EnableCameraBlock" ,  1 ) ? true : false;
		if ( !bCameraBlock )
		{
			g_settings.LOD_CAMERA_BLOCK_UPDATE_TIME = 100000;
		}
		else
		{
			g_settings.LOD_CAMERA_BLOCK_UPDATE_TIME = 0.1f;
		}

		g_settings.LOD_SCENE_PREFETCH_WIDTH = m_nLoadingWidth;
		//g_settings.LOD_TERRAIN_CULL_MODE = 1;
		g_settings.LOD_SCENE_MODEL_RENDER_DISTANCE = m_nLoadingWidth * 2 * 1600.0f;

		g_settings.CAMERA_CLIP_FAR = m_fFarClip;
		g_settings.CAMERA_CLIP_NEAR = m_fNearClip;

		float fNewPitchStart = UT_BaseStringUtility::GetFloatFromString( kConfig , "Config" , "FreeCameraFarPitchStart" , 50.0f );
		float fNewPitchEnd = UT_BaseStringUtility::GetFloatFromString( kConfig , "Config" , "FreeCameraFarPitchEnd" , 50.0f );
		float fNewDistance = UT_BaseStringUtility::GetFloatFromString( kConfig , "Config" , "FreeCameraFarDistance" , 2800.0f );
		CA_CameraManager::GetSingleton().SetCameraPitchAndDistance( fNewPitchStart , fNewPitchEnd , fNewDistance );
	}
}

void UT_CutSceneManager::SaveFbx()
{
#ifndef _SHIPPING
#ifndef _CLIENT_MT
#ifndef _DEBUG
	ReloadConfig();
	NiCameraPtr spCamera = NiNew NiCamera;
	NiPoint3 m_kCenter = NiPoint3(  0 , 0 , 10000 );
	NiPoint3 m_kDir = -NiPoint3::UNIT_Z; 
	NiPoint3 m_kUp = NiPoint3::UNIT_Y;
	NiPoint3 m_kRight = m_kDir.UnitCross(m_kUp);
	m_kDir.Unitize();
	m_kUp.Unitize();
	m_kRight.Unitize();
	NiMatrix3 kMatrix( m_kDir , m_kUp , m_kRight );
	spCamera->SetRotate( kMatrix );
	NiRect<float> kPort( 0.0f , 1.0f , 1.0f , 0.0f );
	spCamera->SetViewPort( kPort );
	spCamera->SetTranslate( m_kCenter );
	spCamera->Update( 0.0f );
	NiFrustum kFru( 0 , 768000 , 768000 , 0 , -1000000.0f , 10000000.0f , true );
	spCamera->SetViewFrustum(kFru);
	spCamera->Update(0.0f);

    m_kCuller.SetFrustumFromCamera(spCamera);

	NiVisibleArray kGeometrys;
	SE_SceneManager::GetSingleton().MarkVisibleArrayDirty();
	//地形	
	SE_SceneManager::GetSingleton().GetTerrainVisibleArrary( spCamera , m_kCuller, kGeometrys );
	unsigned int uiQuantity = kGeometrys.GetCount();
	unsigned int i;
	char buffer[128];
	for (i = 0; i < uiQuantity; i++)
	{
		NiGeometry& rkGeometry = kGeometrys.GetAt(i);
		NiSprintf( buffer , 128 , "%s%d" , "WorldChunk" , i );
		m_kFbxMgr.AddSceneNode( &rkGeometry , buffer , true , true );
	}
	kGeometrys.RemoveAll();
	SE_SceneManager::GetSingleton().MarkVisibleArrayDirty();
	//场景物件
	SE_SceneManager::GetSingleton().GetModelVisibleArray( spCamera , m_kCuller, kGeometrys );

	uiQuantity = kGeometrys.GetCount();
	for (i = 0; i < uiQuantity; i++)
	{
		NiGeometry& rkGeometry = kGeometrys.GetAt(i);
		NiSprintf( buffer , 128 , "%s%d" , "Model" , i );
		m_kFbxMgr.AddSceneNode( &rkGeometry , buffer , true , false );
	}
	kGeometrys.RemoveAll();
	IM_Manager::GetSingleton().GetVisibleArray( spCamera , m_kCuller, kGeometrys );

	uiQuantity = kGeometrys.GetCount();
	for (i = 0; i < uiQuantity; i++)
	{
		NiGeometry& rkGeometry = kGeometrys.GetAt(i);
		NiSprintf( buffer , 128 , "%s%d" , "Actor" , i );
		m_kFbxMgr.AddSceneNode( &rkGeometry , buffer , true , false );
	}
	//Camera
	NiCameraPtr spCurrentCamera = CA_CameraManager::GetSingleton().GetNiCamera();
	NiPoint3 kFocus = NiPoint3::ZERO;
	CA_CameraManager::GetSingleton().GetCurrActorPlayerLocation( kFocus );
	float fVerticalFov = CA_CameraManager::GetSingleton().GetLockCameraFov();
	kFocus.z -= CA_CameraManager::GetSingleton().GetLockCameraOffset();
	m_kFbxMgr.AddCamera( spCurrentCamera , kFocus , fVerticalFov );
	m_kFbxMgr.Save( m_kFbxFileName.c_str() );
	m_kFbxMgr.Clear();

	spCamera = NULL;
#endif
#endif
#endif
}

void UT_CutSceneManager::Play()
{
	ReloadConfig();
	m_spCurrentAnimCam = NULL;
	m_spAnimCamNode = NULL;
	NiStream kStream;
	bool bResult = false;
	do 
	{
		if (kStream.Load( m_kCameraAnimName.c_str() ) == false)
		{
			break;
		}
		NiNodePtr spTemp = NiDynamicCast(NiNode, kStream.GetObjectAt(0));
		if ( spTemp == NULL )
		{
			break;
		}
		m_spAnimCamNode = spTemp;
		_recursiveFindCameras( spTemp );
		if ( m_spCurrentAnimCam == NULL )
		{
			break;
		}
		bResult = true;
	} while (0);
	if ( !bResult )
	{
		GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR , 
			"无法加载%s,相机动画不可用！" , m_kCameraAnimName.c_str() );
		return;
	}
	Ni2DBuffer* pkBackbuffer = NiRenderer::GetRenderer()->GetDefaultBackBuffer();
	m_spCurrentAnimCam->AdjustAspectRatio((float)pkBackbuffer->GetWidth() / (float)pkBackbuffer->GetHeight());
	CA_CameraManager::GetSingleton().SetCameraType( CA_CameraManager::CT_ANIM );
	NiFrustum kFrust = m_spCurrentAnimCam->GetViewFrustum();
	kFrust.m_fFar = m_fFarClip;
	kFrust.m_fNear = m_fNearClip;
	m_spCurrentAnimCam->SetViewFrustum( kFrust );

	m_bPlay = true;
	m_fLastTime = 0.0f;
	m_fPlayTime = 0.0f;
	m_bFirstUpdate = true;
	m_bPause = false;	
}

void UT_CutSceneManager::Pause( bool bEnable )
{
	m_bPause = bEnable;
	if ( m_bPause )
	{
		m_bPlay = false;
	}
	else
	{
		float fCurrentTime = NiGetCurrentTimeInSec();
		m_fLastTime = fCurrentTime;
		m_bPlay = true;
	}
}

void UT_CutSceneManager::Stop()
{
	m_bPlay = false;
}

void UT_CutSceneManager::_recursiveFindCameras( NiAVObject* pkSceneGraph )
{
	if (NiIsKindOf(NiCamera, pkSceneGraph))
	{
		m_spCurrentAnimCam = NiDynamicCast( NiCamera , pkSceneGraph );
	}
	else if (NiIsKindOf(NiNode, pkSceneGraph))
	{
		NiNode* pkNode = (NiNode*)pkSceneGraph;
		NiTimeController* pkController = pkNode->GetControllers();
		while (pkController != NULL)
		{
			if (pkController->GetEndKeyTime() > m_fEndTime)
			{
				m_fEndTime = pkController->GetEndKeyTime();
			}
			pkController = pkController->GetNext();
		}
		for (unsigned int i = 0; i < pkNode->GetArrayCount(); i++)
		{
			NiAVObject* pkChild = pkNode->GetAt(i);
			if (pkChild)
				_recursiveFindCameras(pkChild);
		}
	}
}

void UT_CutSceneManager::Update( float fTime )
{
	if ( m_bPlay && m_spCurrentAnimCam )
	{
		NiPoint3 kPos;
		if ( m_fPlayTime == 0.0f && m_bFirstUpdate )
		{
			//first update
			m_fLastTime = fTime;
			m_bFirstUpdate = false;
		}
		m_fPlayTime = m_fPlayTime + ( fTime - m_fLastTime );
		m_fLastTime = fTime;
		if ( m_fPlayTime > m_fEndTime )
		{
			m_bPlay = false;
			m_fPlayTime = 0.0f;
			return;
		}
		m_spAnimCamNode->Update( m_fPlayTime );
	}
}

bool UT_CutSceneManager::ShowPlayerName()
{
	return m_bPlayerName;
}

bool UT_CutSceneManager::ShowPlayerBlood()
{
	return m_bPlayerBlood;
}
