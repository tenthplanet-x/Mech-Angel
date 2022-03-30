#include "ClientPCH.h"
#include "CA_CameraHelpper.h"
#include "CA_LockCamera.h"
#include "MG_MessageBase.h"

using namespace Utility;
//----------------------------------------------------------------
CA_LockCamera::CA_LockCamera( NiCameraPtr pkObject)
: CA_BaseCamera(pkObject)
{
	m_kLastLookAtLocation = NiPoint3::ZERO;
	m_kCurrentLookAtLocation = NiPoint3::ZERO;

	m_fMoveVelocity = 300.0f;
	m_fMoveAccel = 3500.0f;

	m_iActiveLevel = 0;

	m_bInTransitionProc = false;
	m_fTransitionStart = 0.0f;
	m_iPrevLevel = -1;
	m_fCurrentOffsetTotal = 0.0f;

	m_fPitch[0]		= g_settings.CAMERA_FAR_PITCH;
	m_fYaw[0]		= g_settings.CAMERA_FAR_YAW;
	m_fDistance[0]	= g_settings.CAMERA_FAR_DISTANCE;
	m_fFov[0]		= g_settings.CAMERA_FAR_FOV;
	m_fCharOutlineWidth[0]= g_settings.CAMERA_FAR_CHAR_OUTLINE_WIDTH;

	m_fPitch[1]		= g_settings.CAMERA_MED_PITCH;
	m_fYaw[1]		= g_settings.CAMERA_MED_YAW;
	m_fDistance[1]	= g_settings.CAMERA_MED_DISTANCE;
	m_fFov[1]		= g_settings.CAMERA_MED_FOV;
	m_fCharOutlineWidth[1]= g_settings.CAMERA_MED_CHAR_OUTLINE_WIDTH;


	m_fPitch[2]		= g_settings.CAMERA_NEAR_PITCH;
	m_fYaw[2]		= g_settings.CAMERA_NEAR_YAW;
	m_fDistance[2]	= g_settings.CAMERA_NEAR_DISTANCE;
	m_fFov[2]		= g_settings.CAMERA_NEAR_FOV;
	m_fCharOutlineWidth[2]= g_settings.CAMERA_NEAR_CHAR_OUTLINE_WIDTH;

	m_fFarCameraHeightBias	= g_settings.CAMERA_TRANSITION_FAR_HEIGHT_BIAS;
	m_fNearCameraHeightBias	= g_settings.CAMERA_TRANSITION_NEAR_HEIGHT_BIAS;
	m_fSmoothTotalTime	= g_settings.CAMERA_TRANSITION_TOTAL_TIME;
}
//----------------------------------------------------------------
CA_LockCamera::~CA_LockCamera()
{
}
//----------------------------------------------------------------
void CA_LockCamera::ResetCamera()
{
	if ( m_spNiCamera!= NULL )
	{
		if (CA_CameraHelp::GamePlayer::GetNiPosition(m_kCurrentLookAtLocation))
        {
            m_kLastLookAtLocation = m_kCurrentLookAtLocation;
        }

        _reset_camera( m_fFov[m_iActiveLevel] , m_fYaw[m_iActiveLevel] , m_fPitch[m_iActiveLevel] );
        _compute_camera_translate( m_fDistance[m_iActiveLevel] , 0.0f , 0.0f );

		CA_BaseCamera::ResetCamera();
	}
}
//----------------------------------------------------------------
bool CA_LockCamera::RecvMessage( MG_MessageBase& rkMessage )
{
	CA_BaseCamera::RecvMessage(rkMessage);

	BEGIN_MESSAGE_HANDLE
	MESSAGE_HANDLE(MGT_TIMER_TICK,			_event_timerTick)
	MESSAGE_HANDLE(MGT_LOGIC_PLAYER_MOVE,	_event_LogicPlayerMove)
	MESSAGE_HANDLE(MGT_INPUT_MOUSE_WHEEL,	_event_InputMouseWheel)
	END_MESSAGE_HANDLE
	return true;
}
//----------------------------------------------------------------
void CA_LockCamera::_event_LogicPlayerMove( MG_MessageBase& rkMessage )
{
	if(!m_spNiCamera)
		return;
	
	m_kLastLookAtLocation = m_kCurrentLookAtLocation;
	m_kCurrentLookAtLocation.x = rkMessage.m_fParam1;
	m_kCurrentLookAtLocation.y = rkMessage.m_fParam2;
	m_kCurrentLookAtLocation.z = rkMessage.m_fParam3;

	NiPoint3 kCameraDir = m_spNiCamera->GetWorldDirection();
	float fDistance = m_fDistance[m_iActiveLevel];
	float fFarHeight = 0.0f;
	float fNearHeight = 0.0f;
	
	if ( m_bInTransitionProc )
	{
		fDistance = m_kDistanceSmoother.GetSmoothValue(); 
		fNearHeight = m_kNearHeightSmoother.GetSmoothValue();
		fFarHeight = m_kFarHeightSmoother.GetSmoothValue();
	}
	_compute_camera_translate( fDistance , fNearHeight , fFarHeight );
}
//----------------------------------------------------------------
void CA_LockCamera::_event_InputMouseWheel(MG_MessageBase& rkMessage)
{
	if(rkMessage.m_bMsgHandled)
		return;
	
	if ( m_bInTransitionProc )
	{
		return;
	}
	int iZDelta = GET_WHEEL_DELTA_WPARAM(rkMessage.m_lParam3);
	
	m_iPrevLevel = m_iActiveLevel;

	if(iZDelta > 0)
	{
		m_iActiveLevel = UT_MathBase::MinInt(2,m_iActiveLevel + 1);
	}
	else
	{
		m_iActiveLevel = UT_MathBase::MaxInt(0,m_iActiveLevel - 1);
	}
	if ( m_iActiveLevel == m_iPrevLevel )
	{
		return;
	}
	m_bInTransitionProc = true;
	m_fTransitionStart = GET_PROCESS_TIME;
	m_kDistanceSmoother.Init( m_fDistance[m_iPrevLevel] , m_fDistance[m_iActiveLevel] , m_fSmoothTotalTime , 0 , 0 , true );
	m_kYawSmoother.Init( m_fYaw[m_iPrevLevel] , m_fYaw[m_iActiveLevel] , m_fSmoothTotalTime , 0 , 0 , true );
	m_kPitchSmoother.Init( m_fPitch[m_iPrevLevel] , m_fPitch[m_iActiveLevel] , m_fSmoothTotalTime , 0 , 0, true );
	m_kFovSmoother.Init( m_fFov[m_iPrevLevel] , m_fFov[m_iActiveLevel] , m_fSmoothTotalTime  , 0 , 0, true);
	m_kNearHeightSmoother.Init( 0 ,abs( m_fNearCameraHeightBias- m_fFarCameraHeightBias ) , m_fSmoothTotalTime , 0 , 0 , true);
	m_kFarHeightSmoother.Init( 0 , m_fFarCameraHeightBias , m_fSmoothTotalTime , 0 , 0 , true);
}
//----------------------------------------------------------------
void CA_LockCamera::_event_timerTick( MG_MessageBase& rkMessage )
{
	T_PROFILE("Update::LockCamera");
	if( !m_bInTransitionProc )
    {
        m_spNiCamera->SetRotate(m_kRotation);
        return;
    }

    float fCurrentTime = rkMessage.m_fParam1;
	float fDeltaTime = rkMessage.m_fParam2;
	float fTotalTime = fCurrentTime - m_fTransitionStart;
	float fOldDistance = m_kDistanceSmoother.Update( fDeltaTime );
	m_kYawSmoother.Update( fDeltaTime );
	m_kPitchSmoother.Update( fDeltaTime );
	m_kFovSmoother.Update( fDeltaTime );
	m_kNearHeightSmoother.Update( fDeltaTime );
	m_kFarHeightSmoother.Update( fDeltaTime );
	float fDistance = m_kDistanceSmoother.GetSmoothValue(); 
	float fFov = m_kFovSmoother.GetSmoothValue();
	float fYaw = m_kYawSmoother.GetSmoothValue();
	float fPitch = m_kPitchSmoother.GetSmoothValue();
	float fNearHeight = m_kNearHeightSmoother.GetSmoothValue();
	float fFarHeight = m_kFarHeightSmoother.GetSmoothValue();

	if ( fTotalTime > m_fSmoothTotalTime )
	{
		m_bInTransitionProc = false;
	}
#ifdef _MAP_VIEWER_BUILD
	/*GfxWriteLog( LOG_INFO_FILELINE , LOG_SWITCH_INFO ,
		"Time is %f , Distance is %f , Delta is %f , From is %f , To is %f" ,
		fTotalTime ,  fDistance , fOldDistance , m_fDistance[m_iPrevLevel] ,
		m_fDistance[m_iActiveLevel] );*/
#endif
	_reset_camera( fFov , fYaw , fPitch );
	_compute_camera_translate(fDistance,fNearHeight , fFarHeight );
}
//----------------------------------------------------------------
void CA_LockCamera::_compute_camera_translate( float fDistance , float fNearHeight , float fFarHeight )
{
	NiPoint3 kCurrentLocation = m_spNiCamera->GetWorldLocation();
	NiShaderFactory::UpdateGlobalShaderConstant("eyePosition",sizeof(NiPoint3),&kCurrentLocation);

	NiPoint3 kCameraDir = m_spNiCamera->GetWorldDirection();
	m_kListenerLocation = m_kCurrentLookAtLocation - kCameraDir * fDistance;
	
	NiPoint3 kTargetLocation = m_kCurrentLookAtLocation - kCameraDir * fDistance;

	if( m_bInTransitionProc )
	{
		if ( m_iActiveLevel == 2 && m_iPrevLevel == 1 )//From Med to Near -H1->0->+H2
		{
			kTargetLocation.z += m_fFarCameraHeightBias + fNearHeight;
			m_fCurrentOffsetTotal = m_fFarCameraHeightBias + fNearHeight;

			//GfxWriteLog( LOG_INFO_TAG , "From Med to Near Camera Pos %f %f %f , Height Delta is %f %f" , kTargetLocation.x , kTargetLocation.y , kTargetLocation.z ,fNearHeight );
		}
		if( m_iActiveLevel == 1 && m_iPrevLevel == 2 )//From Near to Med
		{
			kTargetLocation.z += m_fNearCameraHeightBias - fNearHeight;
			m_fCurrentOffsetTotal = m_fNearCameraHeightBias - fNearHeight;
		}
		if ( m_iActiveLevel == 1 && m_iPrevLevel == 0 )//From Far to Med
		{
			kTargetLocation.z += m_fFarCameraHeightBias;
			m_fCurrentOffsetTotal = m_fFarCameraHeightBias;
		}
		if ( m_iActiveLevel == 0 && m_iPrevLevel == 1 )//From Med to Far
		{
			kTargetLocation.z += m_fFarCameraHeightBias;
			m_fCurrentOffsetTotal = m_fFarCameraHeightBias;
		}
		//GfxWriteLog( LOG_INFO_TAG , "Camera Pos %f %f %f , Height Delta is %f %f" , kTargetLocation.x , kTargetLocation.y , kTargetLocation.z ,fNearHeight , fFarHeight );
	}
	else
	{
		if ( m_iActiveLevel == 2 )//Near
		{
			kTargetLocation.z += m_fNearCameraHeightBias;
			m_fCurrentOffsetTotal = m_fNearCameraHeightBias;
		}
		else
		{
			kTargetLocation.z += m_fFarCameraHeightBias;
			m_fCurrentOffsetTotal = m_fFarCameraHeightBias;
		}
	}
	m_spNiCamera->SetTranslate(kTargetLocation);
	m_spNiCamera->Update(0.0f);
}

void CA_LockCamera::_on_active_level_changed(int newLv)
{
	// ÐÞ¸Ä½ÇÉ«shaderÃè±ß¿í¶È
	int lv = UT_MathBase::ClampInt(newLv, 0, _countof(m_fCharOutlineWidth) - 1);
	float g_fOutlineWidth = m_fCharOutlineWidth[lv];
	NiFixedString strCharacterOutlineWidth = "g_fOutlineWidth";
	NiShaderFactory::UpdateGlobalShaderConstant(strCharacterOutlineWidth, sizeof(float), &g_fOutlineWidth);
}

float CA_LockCamera::GetHeightOffset( void )
{
	return m_fCurrentOffsetTotal;
	/*switch(m_iActiveLevel)
	{
	case 0:
		return m_fFarCameraHeightBias;
		break;
	case 1:
		return m_fFarCameraHeightBias + m_fFarCameraHeightBias;
		break;
	case 2:
		return m_fNearCameraHeightBias + m_fFarCameraHeightBias + m_fFarCameraHeightBias;
		break;
	}
	return 0;*/
}

float CA_LockCamera::GetScale( void )
{
    NiFrustum kFrustum = m_spNiCamera->GetViewFrustum();
	float fTmB = kFrustum.m_fTop - kFrustum.m_fBottom;
	float fResult = 2.0f / fTmB;
	fResult = 1 / ( fResult / 1.778341f ); 
	return (1 - fResult);
}

void CA_LockCamera::SetPitch( int iLevel , float fPitch )
{
	m_fPitch[iLevel] = fPitch;
}

void CA_LockCamera::SetDistance( int iLevel , float fDistance )
{
	m_fDistance[iLevel] = fDistance;
}
