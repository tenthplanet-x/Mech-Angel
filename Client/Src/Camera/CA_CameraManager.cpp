#include "ClientPCH.h"

#include "CA_CameraHelpper.h"
#include "CA_CameraManager.h"
#include "CA_FreeCamera.h"
#include "CA_LockCamera.h"
#include "CA_AnimCamera.h"
#include "CA_SelCharCamera.h"
#include "CA_DraconisCamera.h"
#include "CA_TrackAnimCamera.h"
#include "CA_BossShowCamera.h"
#include "CA_BossSeparateCamera.h"
#include "CA_CameraPathAnimation.h"
#include "MG_MessageBase.h"
#include "MG_MessageCenter.h"

#include "LC_ClientLogicManager.h"
#include "UT_CutSceneManager.h"

#include "CF_CharType.h"
#include "CF_BossCamera.h"
#include "CF_BossCameraDetail.h"
#include "LC_ClientNPCManager.h"

#include "CF_CameraCut.h"

using namespace GameLogic;
using namespace Utility;

//---------------------------------------------------

static void FillDataArray(float* pfData, const D3DMATRIX& kMatrix)
{
	pfData[0] = kMatrix._11;
	pfData[1] = kMatrix._12;
	pfData[2] = kMatrix._13;
	pfData[3] = kMatrix._14;
	pfData[4] = kMatrix._21;
	pfData[5] = kMatrix._22;
	pfData[6] = kMatrix._23;
	pfData[7] = kMatrix._24;
	pfData[8] = kMatrix._31;
	pfData[9] = kMatrix._32;
	pfData[10] = kMatrix._33;
	pfData[11] = kMatrix._34;
	pfData[12] = kMatrix._41;
	pfData[13] = kMatrix._42;
	pfData[14] = kMatrix._43;
	pfData[15] = kMatrix._44;
}

//---------------------------------------------------

CA_CameraManager::CA_CameraManager()
: m_iLockCameraLastLevel(-1)
, m_spCamera(NULL)
, m_spReflectCamera(NULL)
, m_spSelCharCamera(NULL)
, m_eCurrentCamera(CT_NA)
{
	::memset(m_pfWorldToCamera, 0, sizeof(m_pfWorldToCamera));
}

CA_CameraManager::~CA_CameraManager()
{
	T_ASSERT_CRITICAL_S(m_spCamera == NULL);
	T_ASSERT_CRITICAL_S(m_spReflectCamera == NULL);
	T_ASSERT_CRITICAL_S(m_spSelCharCamera == NULL);
}

//---------------------------------------------------

bool CA_CameraManager::Init()
{
	m_spCamera = NiNew NiCamera;
	m_spReflectCamera = NiNew NiCamera;
	
	//camera types
	CA_FreeCamera* pkFreeCamera = T_NEW_D CA_FreeCamera(m_spCamera);
    m_kCameraMap[CT_FREE] = pkFreeCamera;

	CA_LockCamera* pkLockCamera = T_NEW_D CA_LockCamera(m_spCamera);
    m_kCameraMap[CT_LOCK] = pkLockCamera;

	CA_AnimCamera* pkAnimCamera = T_NEW_D CA_AnimCamera(m_spCamera);
    m_kCameraMap[CT_ANIM] = pkAnimCamera;

	CA_SelCharCamera* pkSelCamera = T_NEW_D CA_SelCharCamera( m_spCamera );
    m_kCameraMap[CT_SELCHAR] = pkSelCamera;

	CA_DraconisCamera* pkDraconisCamera = T_NEW_D CA_DraconisCamera( m_spCamera );
    m_kCameraMap[CT_DRACONIS] = pkDraconisCamera;

	CA_TrackAnimCamera* pkTrackAnimCamera = T_NEW_D CA_TrackAnimCamera( m_spCamera );
    m_kCameraMap[CT_TRACK_ANIM] = pkTrackAnimCamera;

	CA_BossShowCamera* pkBossShowCamera = T_NEW_D CA_BossShowCamera( m_spCamera );
	m_kCameraMap[CT_DUNGEON_BOSS_SHOW] = pkBossShowCamera;

	m_kSeparateCameraVec.reserve(10);

	return true;
}

bool CA_CameraManager::UnInit()
{
    for (int i = 0; i < CT_MAX_COUNT; ++i)
    {
        T_SAFE_DELETE(m_kCameraMap[(CAMERA_TYPE)i]);
    }
	m_kCameraMap.clear();

	SeparateCameraVec::iterator iter = m_kSeparateCameraVec.begin();
	for( ; iter!=m_kSeparateCameraVec.end(); ++iter)
	{
		T_SAFE_DELETE(*iter);
	}
	m_kSeparateCameraVec.clear();

	m_spCamera = NULL;
	m_spReflectCamera = NULL;
    m_spSelCharCamera = NULL;
	m_eCurrentCamera = CT_NA;


	return true;
}

//---------------------------------------------------

bool CA_CameraManager::RecvMessage( MG_MessageBase& rkMessage )
{
	MG_MessageHandlerInterface::RecvMessage(rkMessage);

	BEGIN_MESSAGE_HANDLE
#ifdef _MAP_VIEWER_BUILD
	MESSAGE_HANDLE(MGT_INPUT_KEY_9,_event_InputKey9)
#endif
		MESSAGE_HANDLE(MGT_TIMER_TICK,_eventTimerTick)
		MESSAGE_HANDLE(MGT_NPC_BORN_BEGIN,_event_NPCBornBegin)
	END_MESSAGE_HANDLE

	return true;
}

#ifdef _MAP_VIEWER_BUILD
void CA_CameraManager::_event_InputKey9(MG_MessageBase& rkMessage)
{
	if(m_eCurrentCamera != CT_ANIM)
	{
		SetCameraType(CT_ANIM);
	}
	else
	{
		SetCameraType(CT_LOCK);
	}
}
#endif

void	CA_CameraManager::_eventTimerTick(MG_MessageBase& rkMessage)
{
	switch(m_eCurrentCamera)
	{
	case CT_DUNGEON_BOSS_SHOW:
		{
			CA_BossShowCamera* pkBossShowCamera = (CA_BossShowCamera*)(m_kCameraMap[CT_DUNGEON_BOSS_SHOW]);
			if (pkBossShowCamera->isEnd())			//移回来结束，切回原先镜头
			{
				this->UpdateCameraType();
			}
		}
		break;
	case CT_BOSS_SHOW_SEPARATE:
		{
			SeparateCameraVec::iterator iter = m_kSeparateCameraVec.begin();
			if(iter != m_kSeparateCameraVec.end())
			{
				CA_BossSeparateCamera* pkBossShowCamera = *iter;
				if (pkBossShowCamera->isEnd())
				{
					_detachMessageHandler(pkBossShowCamera);

					if(m_kSeparateCameraVec.size() == 1)
					{
						pkBossShowCamera->OnDetach();
					}

					T_SAFE_DELETE(*iter);
					iter = m_kSeparateCameraVec.erase(iter);

					if(iter != m_kSeparateCameraVec.end())
					{
						CA_BossSeparateCamera* pkNewCamera = *iter;
						_attachMessageHandler(pkNewCamera);
						pkNewCamera->go(m_kSeparateCameraVec.size() == 1);
					}
				}
			}
			else
			{
				UpdateCameraType();
			}

		}
		break;
	case CT_TRACK_ANIM:
		{
			CA_TrackAnimCamera* pkTrackAnim = (CA_TrackAnimCamera*)(m_kCameraMap[CT_TRACK_ANIM]);
			if ( pkTrackAnim->IsNeedEnd() )			//移回来结束，切回原先镜头
			{
				this->UpdateCameraType();
			}			
		}
		break;
	}
}

void	CA_CameraManager::_event_NPCBornBegin(MG_MessageBase& rkMessage)	//boss出现时切镜头移过去
{
	if (m_eCurrentCamera != CT_DRACONIS)
		return;

	object_id_type	lNpcID = rkMessage.m_lParam1;
	int				lNpcCharType = rkMessage.m_lParam2;

	CF_CharType::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_CharType>(lNpcCharType);
	if (pkData == NULL)
		return;

	if ( Camera::CA_CameraAnimationManager::GetSingleton().GetAnimationPath((unsigned int)pkData->_fBornCameraShowTime) != NULL)
	{
		CA_CameraManager::GetSingleton().SetCameraType(CT_TRACK_ANIM);
		CA_CameraManager::GetSingleton().PlayTrackAnim((unsigned int)pkData->_fBornCameraShowTime, true); 
		return ;
	}

	if (pkData->_fBornCameraShowTime <= 0)			//见CharType表
		return;

	CF_BossCamera::DataEntry* pkCameraData = SafeGetCSVFileDataEntryPtr<CF_BossCamera>(lNpcCharType);
	if(pkCameraData == NULL)
	{
		return;
	}
	
	m_kSeparateCameraVec.clear();

	mem::vector<int>::const_iterator iter = pkCameraData->_kCameras.begin();
	for( ; iter!=pkCameraData->_kCameras.end(); ++iter)
	{
		const CF_BossCameraDetail::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_BossCameraDetail>(*iter);
		T_ASSERT_CRITICAL_S(pkData != NULL);

		CA_BossSeparateCamera* pkBossShowCamera = T_NEW_D CA_BossSeparateCamera( m_spCamera );
		pkBossShowCamera->setMoveArg(pkData->_fTotalTime, lNpcID);
		pkBossShowCamera->setYaw(pkData->_fHorzAngle);
		pkBossShowCamera->setTargetYaw(pkData->_fTargetHorzAngle);
		pkBossShowCamera->setPitch(pkData->_fCloseUpAngle);
		pkBossShowCamera->setTargetPitch(pkData->_fTargetCloseUpAngle);
		pkBossShowCamera->setRoll(pkData->_fSideAngle);
		pkBossShowCamera->setTargetRoll(pkData->_fTargetSideAngle);
		pkBossShowCamera->setHeight(pkData->_fCloseUpHeight);
		pkBossShowCamera->setTargetHeight(pkData->_fTargetCloseUpHeight);
		pkBossShowCamera->setDistance(pkData->_fCloseUpDistance);
		pkBossShowCamera->setTargetDistance(pkData->_fTargetCloseUpDistance);
		m_kSeparateCameraVec.push_back(pkBossShowCamera);
	}

	if(m_kSeparateCameraVec.size() == 0)
		return;

	SetCameraType(CT_BOSS_SHOW_SEPARATE);

	LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
	LC_ClientNPC* pkNPC = (LC_ClientNPC* )pkNPCManager->FindNPC(lNpcID);
	if(pkNPC)
	{
		if(!pkNPC->IsCreateVisibleData())
		{
			pkNPC->CreateVisibleData();
		}

		pkNPC->PlayScreenFx(pkCameraData->_sFx);
	}

	CA_BossSeparateCamera* pkFirstCamera = m_kSeparateCameraVec[0];
	pkFirstCamera->ShowOther(false);
	pkFirstCamera->go(m_kSeparateCameraVec.size() == 1);
}

//---------------------------------------------------

NiCameraPtr CA_CameraManager::GetNiReflectCamera( float fReflectPlaneHeight , bool bUpdate /*= false */ )
{
	if( bUpdate )
	{
		NiPoint3 kActorPlayerLocation(0.0f, 0.0f, 0.0f);
		GetCurrActorPlayerLocation( kActorPlayerLocation );
		// set the mirror camera's rotation matrix
		NiMatrix3 kRot;
		//Compute Pitch and Yaw
		NiPoint3 kViewDirection = m_spCamera->GetWorldDirection();
		float fPitch = NiASin(kViewDirection.z);
		kViewDirection.z = 0;
		kViewDirection.Unitize();
		float fYaw = NiATan2(kViewDirection.y, kViewDirection.x);
		fYaw *= -1.0f;
		//Invert the pitch here since we will reflect the camera across the plane.
		//Yaw should remain the same
		fPitch = -1.0f * fPitch;
		NiMatrix3 kTemp;
		//Rotate and orient the camera so that Z is up.
		kRot.MakeXRotation( -1.0f * NI_HALF_PI );
		//Pitch around Y which is the current right vector.
		kTemp.MakeYRotation( fPitch );
		kRot = kTemp * kRot;
		//Make the Z rotation here around world Z since we've defined world Z
		//as up for our application.
		kTemp.MakeZRotation(fYaw);
		kRot = kTemp * kRot;

		m_spReflectCamera->SetRotate(kRot);

		NiPoint3 kLoc = m_spCamera->GetWorldTranslate();
		NiPoint3 kWorldDir = m_spCamera->GetWorldDirection();
		//kLoc -= m_spCamera->GetWorldDirection() * 800.0f;
		float fWaterHeight = fReflectPlaneHeight;
		// reflect loc across the plane = L - 2((L - C) DOT N) N
		kLoc.z -= 2.0f * ( kLoc.z - fWaterHeight );
		//把反射相机的位置设置为当前玩家位置沿相机的世界坐标方向12米偏移的地方
		//这样在三个视角下都能正确的把当前可视场景中的物件进行反射
		//否则会出现远视角正常，拉到中视角的时候反射图就被裁掉了
		//kLoc.x = kActorPlayerLocation.x + kWorldDir.x * 1200.0f;
		//kLoc.y = kActorPlayerLocation.y + kWorldDir.x * 1200.0f;
		m_spReflectCamera->SetTranslate(kLoc);
		//GfxWriteLog( LOG_INFO_FILELINE , LOG_SWITCH_INFO , "Ref camera pos %f %f %f" , kLoc.x , kLoc.y , kLoc.z );
		//强行把FOV设置为60，避免出现反射纹理因为相机高度问题出现的纹理错误
		float fVerticalFieldOfViewDegrees = 60;
		float fVerticalFieldOfViewRad = NI_PI / 180.0f * fVerticalFieldOfViewDegrees;
		float fViewPlaneHalfHeight = tanf(fVerticalFieldOfViewRad * 0.5f);
		float fViewPlaneHalfWidth = fViewPlaneHalfHeight * 4.0f/3.0f;

		NiFrustum kFrustum(
			-fViewPlaneHalfWidth, fViewPlaneHalfWidth, 
			fViewPlaneHalfHeight, -fViewPlaneHalfHeight,
			0.0f,150000.0f);
		m_spReflectCamera->SetViewFrustum(kFrustum);

		m_spReflectCamera->Update(0.0f);

		NiPoint3 kRefDir = m_spReflectCamera->GetWorldDirection();

		_update_world_to_camera_matrix( m_spReflectCamera, "WorldToReflect" );

		if ( g_settings.LOD_ENABLE_REFRACT )
		{
			_update_world_to_camera_matrix( m_spCamera, "WorldToRefract" );
		}
	}
	return m_spReflectCamera;
}

//---------------------------------------------------

bool CA_CameraManager::SetCameraType( CAMERA_TYPE eCameraType )
{
	if(m_eCurrentCamera == eCameraType)
		return true;

	if(eCameraType <= CT_NA || eCameraType >= CT_MAX_COUNT)
		return false;

	CA_BaseCamera* pkOldCamera = NULL;
	if(m_eCurrentCamera != CT_BOSS_SHOW_SEPARATE)
	{
		pkOldCamera = m_kCameraMap[m_eCurrentCamera];
	}
	else if(!m_kSeparateCameraVec.empty())
	{
		pkOldCamera = m_kSeparateCameraVec[0];
	}

	
	CA_BaseCamera* pkNewCamera = NULL;
	if(eCameraType == CT_BOSS_SHOW_SEPARATE && !m_kSeparateCameraVec.empty())
	{
		pkNewCamera = m_kSeparateCameraVec[0];
	}
	else
	{
		pkNewCamera = m_kCameraMap[eCameraType];
	}

	//detatch old
	if (pkOldCamera != NULL)
	{
		_detachMessageHandler(pkOldCamera);
		pkOldCamera->OnDetach();
	}

	//attach new
	bool b = false;
	if (pkNewCamera != NULL)
	{
		_attachMessageHandler(pkNewCamera);
		b = pkNewCamera->OnAttach();
	}
	m_eCurrentCamera = eCameraType;

    if (UT_CutSceneManager::GetSingletonPtr() != NULL)
    {
        UT_CutSceneManager::GetSingleton().SetPlayerMoveDisabled(eCameraType == CT_FREE);
    }

	return b;
}

void CA_CameraManager::UpdateCameraType()
{
	if(g_settings.CFG_CAMERA_MODE == 1)
	{
		SetCameraType(CT_DRACONIS);
	}
	else if(g_settings.CFG_CAMERA_MODE == 0)
	{
		SetCameraType(CT_LOCK);
	}
}

void CA_CameraManager::ResetAllCamera()
{
	if ( m_eCurrentCamera == CT_BOSS_SHOW_SEPARATE )
	{
		SeparateCameraVec::iterator iter = m_kSeparateCameraVec.begin();
		if(!m_kSeparateCameraVec.empty())
		{
			CA_BossSeparateCamera* pkCamera = m_kSeparateCameraVec[0];
			_detachMessageHandler(pkCamera);
			pkCamera->OnDetach();

			for( ; iter!=m_kSeparateCameraVec.end(); ++iter)
			{
				T_SAFE_DELETE(*iter);
			}
			m_kSeparateCameraVec.clear();
		}
	}
	else if (m_kCameraMap[m_eCurrentCamera] != NULL)
	{
		m_kCameraMap[m_eCurrentCamera]->ResetCamera();
		MG_MessageCenter::GetSingleton().SendMessage( CreateMsg_CAMERA_RESET() );
	}
}

void CA_CameraManager::OnRendererResize()
{
	CA_BaseCamera * pkCamera = m_kCameraMap[m_eCurrentCamera];
	if (pkCamera == NULL)
		return;

	NiRenderer* pkRenderer = NiRenderer::GetRenderer();
	Ni2DBuffer* pkBackbuffer = pkRenderer->GetDefaultBackBuffer();
	if (pkBackbuffer == NULL)	return;

	unsigned int nWidth  = pkBackbuffer->GetWidth();
	unsigned int nHeight = pkBackbuffer->GetHeight();
	if (nWidth == 0 || nHeight == 0)	return;

	float fAspectRadio = (float)nWidth / (float)nHeight;
	pkCamera->OnRendererResize(fAspectRadio);
}

//---------------------------------------------------

void CA_CameraManager::_update_world_to_camera_matrix( NiCameraPtr spCamera, const NiFixedString& rkKey )
{
	D3DXMATRIX kMatrix, kMatrixT;
	const float* aafWorldToRC = spCamera->GetWorldToCameraMatrix();
	NiMemcpy(&kMatrix, aafWorldToRC, sizeof(float) * 16);

	D3DXMatrixTranspose(&kMatrixT, &kMatrix);
	FillDataArray( m_pfWorldToCamera, kMatrixT);

	NiShaderFactory::UpdateGlobalShaderConstant(rkKey, sizeof(float)*16, m_pfWorldToCamera);
}

//---------------------------------------------------

float CA_CameraManager::GetLockCameraOffset()
{
	CA_BaseCamera *pkBaseCamera = m_kCameraMap[CT_LOCK];
	CA_LockCamera *pkLockCamera = static_cast<CA_LockCamera*>(pkBaseCamera);
	if(pkLockCamera)
	{
		return pkLockCamera->GetHeightOffset();
	}
	return 0.0f;
}

float CA_CameraManager::GetLockCameraFov()
{
	CA_BaseCamera *pkBaseCamera = m_kCameraMap[CT_LOCK];
	CA_LockCamera *pkLockCamera = static_cast<CA_LockCamera*>(pkBaseCamera);
	if(pkLockCamera)
	{
		return pkLockCamera->GetFov();
	}
	return 60.0f;
}
float CA_CameraManager::GetLockCameraScale()
{
	CA_BaseCamera *pkBaseCamera = m_kCameraMap[CT_LOCK];
	CA_LockCamera *pkLockCamera = static_cast<CA_LockCamera*>(pkBaseCamera);
	if(pkLockCamera)
	{
		return pkLockCamera->GetScale();
	}
	return 1.0f;
}

void CA_CameraManager::SetSelCharNiCamera(NiCameraPtr spSelCharNiCamera)
{
    if(spSelCharNiCamera == NULL)
		return;

    m_spSelCharCamera = spSelCharNiCamera;
    m_kCameraMap[CT_SELCHAR]->SetNiCamera(m_spSelCharCamera);
}

void CA_CameraManager::SetFarClipDistance( float fDistance )
{
	if( m_spCamera && fDistance >= 6000 )
	{
		NiFrustum kFru = m_spCamera->GetViewFrustum();
		kFru.m_fFar = fDistance;
		m_spCamera->SetViewFrustum( kFru );
	}
}

void CA_CameraManager::SetLockCameraPitch( float fPitch )
{
	if ( m_eCurrentCamera == CT_LOCK)
	{
		CA_LockCamera* pkLockCamera = (CA_LockCamera*)m_kCameraMap[m_eCurrentCamera];
		pkLockCamera->SetPitch( 0 , fPitch );
		pkLockCamera->ResetCamera();
	}
}

void CA_CameraManager::SetLockCameraDistance( float fDistance )
{
	if ( m_eCurrentCamera == CT_LOCK )
	{
		CA_LockCamera* pkLockCamera = (CA_LockCamera*)m_kCameraMap[m_eCurrentCamera];
		pkLockCamera->SetDistance( 0 , fDistance );
		pkLockCamera->ResetCamera();
	}
}

void CA_CameraManager::SetCameraPitchAndDistance( float fPStart , float fPEnd , float fDistance )
{
	if ( m_eCurrentCamera == CT_DRACONIS )
	{
		CA_DraconisCamera* pkLockCamera = (CA_DraconisCamera*)m_kCameraMap[m_eCurrentCamera];
		pkLockCamera->SetPitchRange( fPStart , fPEnd );
		pkLockCamera->SetDistance( 0 , fDistance );
		pkLockCamera->ResetCamera();
	}
}

//---------------------------------------------------

bool CA_CameraManager::GetCurrActorPlayerLocation( NiPoint3& rkActorPlayerLocation )
{
	if (CA_CameraHelp::GamePlayer::GetNiPosition(rkActorPlayerLocation))
		return true;

	//如果当前玩家没有创建就用一个假的玩家位置
	rkActorPlayerLocation = m_spCamera->GetWorldLocation();
	//rkActorPlayerLocation.z += 200.0f;
	return false;
}

NiPoint3 CA_CameraManager::GetListenerLocation()
{
	NiPoint3 td = NiPoint3::ZERO;
	if ( m_eCurrentCamera == CT_DRACONIS )
	{
		CA_DraconisCamera* pkLockCamera = (CA_DraconisCamera*)m_kCameraMap[m_eCurrentCamera];
		td = pkLockCamera->GetListenerLocation();
	}
	return td;
}

void CA_CameraManager::MoveCameraToNoUnLock()
{	
	CA_BossShowCamera* pkCamera = (CA_BossShowCamera*)m_kCameraMap[CT_DUNGEON_BOSS_SHOW];
	pkCamera->backLockPlayer();
}

//---------------------------------------------------

bool CA_CameraManager::PlayTrackAnim(int trackIdx, bool AutoStop /*= false*/)
{
	if ( m_eCurrentCamera == CT_TRACK_ANIM )
	{
		CA_TrackAnimCamera* pkCamera = (CA_TrackAnimCamera*)m_kCameraMap[m_eCurrentCamera];
		pkCamera->SetAutoStop(AutoStop);
		return pkCamera->PlayAnim(trackIdx);;
	}

	return false;
}

//---------------------------------------------------

bool CA_CameraManager::PlayMaxAnim(int chartpyeid, int npcid)
{
	if ( m_eCurrentCamera == CT_TRACK_ANIM )
	{
		CA_TrackAnimCamera* pkCamera = (CA_TrackAnimCamera*)m_kCameraMap[m_eCurrentCamera];
		if ( !pkCamera->IsCurrentPlay() )
		{
			pkCamera->SetAutoStop(true);
				LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
			LC_ClientNPC* pkNPC = (LC_ClientNPC* )pkNPCManager->FindNPC(npcid);
			NiPoint3 sd = NiPoint3::ZERO;
			if ( pkNPC )
			{
				sd = UT_EngineMath::ConvertVec3IntToNiPoint3(pkNPC->GetCurrentLocation());
			}
			return pkCamera->PlayMaxAnim(chartpyeid, sd);
		}
		return false;
	}
	return false;
}

//---------------------------------------------------
void CA_CameraManager::StopTrackAnim()
{
	if ( m_eCurrentCamera == CT_TRACK_ANIM )
	{
		CA_TrackAnimCamera* pkCamera = (CA_TrackAnimCamera*)m_kCameraMap[m_eCurrentCamera];
		pkCamera->StopAnim();
	}
}
//---------------------------------------------------

void CA_CameraManager::SetCameraKey(int trackIdx, int keyIdx)
{
	if (m_eCurrentCamera == CT_FREE)
	{
		CA_TrackAnimCamera* pkCamera = (CA_TrackAnimCamera*)m_kCameraMap[CT_TRACK_ANIM];
		CA_FreeCamera* curCamera = (CA_FreeCamera*)m_kCameraMap[CT_FREE];
		NiPoint3 tp = NiPoint3::ZERO;
		NiQuaternion qua = NiQuaternion::IDENTITY;
		float fov;
		if ( pkCamera->SetKeyCamera(trackIdx, keyIdx, tp , qua, fov) )
		{
			curCamera->MoveRotateTo(tp, qua, fov);
		}
	}
}

void CA_CameraManager::ModifyCameraKey(int trackIdx, int keyIdx)
{
	if (m_eCurrentCamera == CT_FREE)
	{
		CA_TrackAnimCamera* pkCamera = (CA_TrackAnimCamera*)m_kCameraMap[CT_TRACK_ANIM];
		CA_FreeCamera* curCamera = (CA_FreeCamera*)m_kCameraMap[CT_FREE];
		NiPoint3 tp = curCamera->GetNiCamera()->GetTranslate();
		NiQuaternion qua;
		curCamera->GetNiCamera()->GetRotate(qua);
		pkCamera->ModifyKey(trackIdx, keyIdx, tp, qua);
	}
}

//---------------------------------------------------   

void CA_CameraManager::AddCameraKey(int trackIdx, int keyIdx)
{
	if ( m_eCurrentCamera == CT_FREE)
	{
		CA_TrackAnimCamera* pkCamera = (CA_TrackAnimCamera*)m_kCameraMap[CT_TRACK_ANIM];
		CA_FreeCamera* curCamera = (CA_FreeCamera*)m_kCameraMap[CT_FREE];
		NiPoint3 tp = curCamera->GetNiCamera()->GetTranslate();
		NiQuaternion qua;
		curCamera->GetNiCamera()->GetRotate(qua);
		pkCamera->AddKey(trackIdx, keyIdx, tp, qua);
	}
}

void CA_CameraManager::RemoveCameraKey(int trackIdx, int keyIdx)
{
	if ( m_eCurrentCamera == CT_FREE )
	{
		CA_TrackAnimCamera* pkCamera = (CA_TrackAnimCamera*)m_kCameraMap[CT_TRACK_ANIM];
		pkCamera->RemoveKey(trackIdx, keyIdx);
	}
}

//---------------------------------------------------

//---------------------------------------------------
int CA_CameraManager::GetCameraKeyCount(int trackIdx)
{
	if (m_eCurrentCamera == CT_FREE)
	{
		CA_TrackAnimCamera* pkCamera = (CA_TrackAnimCamera*)m_kCameraMap[m_eCurrentCamera];
		return pkCamera->GetKeyCameraCount(trackIdx);
	}
	return 0;
}

//---------------------------------------------------

void CA_CameraManager::MoveToTarget(int lTargetID, float go, float back, float stop, bool follow)
{
	bool kFlag = false;
	float yyaw = 0.0f;
	float ddisn = 0.0f;
	if ( m_eCurrentCamera == CT_DRACONIS)
	{
		CA_DraconisCamera* pkDrac = (CA_DraconisCamera*)(m_kCameraMap[CT_DRACONIS]);
		yyaw = pkDrac->GetYaw();
		ddisn = pkDrac->GetDistance();
		kFlag = true;
	}
	//开始
	this->SetCameraType(CT_DUNGEON_BOSS_SHOW);

	CA_BossShowCamera* pkBossShowCamera = (CA_BossShowCamera*)(m_kCameraMap[CT_DUNGEON_BOSS_SHOW]);
	pkBossShowCamera->setMoveArg(go, back, stop, follow);
	pkBossShowCamera->go(lTargetID);
	if ( kFlag )
	{	
		pkBossShowCamera->setYaw(yyaw);
		pkBossShowCamera->setDistance(ddisn);
	}
	else
	{	
		pkBossShowCamera->setYaw(140.0f);
		pkBossShowCamera->setDistance(2800.0f);
	}
}

void CA_CameraManager::MoveToMC(void)
{
	if(m_eCurrentCamera == CT_DUNGEON_BOSS_SHOW)
	{
		CA_BossShowCamera* pkBossShowCamera = (CA_BossShowCamera*)(m_kCameraMap[CT_DUNGEON_BOSS_SHOW]);
		pkBossShowCamera->back();
	}
}


NiPoint3 CA_CameraManager::GetFreeCameraLoc()
{
	if ( m_eCurrentCamera == CT_FREE )
	{
		CA_FreeCamera* pkFreeCamera = (CA_FreeCamera*)(m_kCameraMap[CT_FREE]);
		return pkFreeCamera->GetLockPos();
	}
	return NiPoint3::ZERO;
}

void CA_CameraManager::ResetPlayerCamera()
{
//TODO: May need to check lock_camera as well
	if ( m_eCurrentCamera == CT_DRACONIS )
	{
		CA_DraconisCamera* pkLockCamera = (CA_DraconisCamera*)m_kCameraMap[m_eCurrentCamera];
		pkLockCamera->ResetCamera();
	}
}

