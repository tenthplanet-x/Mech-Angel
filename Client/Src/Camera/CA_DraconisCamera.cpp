#include "ClientPCH.h"
#include "CA_DraconisCamera.h"
#include "CA_CameraHelpper.h"
#include "MG_MessageBase.h"
#include "MG_MessageCenter.h"

using namespace Utility;

//---------------------------------------------------------------------------	
CA_DraconisCamera::CA_DraconisCamera( NiCameraPtr pkObject )
: CA_LockCamera(pkObject),
  m_fDistanceMin(1800),
  m_fDistanceMax(2500),
  m_fDraconisDistance(2000),
  m_fDraconisFov(0),
  m_fDraconisPitch(0),
  m_fDraconisYaw(0),
  m_fAngleZ(140.f)
{
	m_fCameraMoveSpeed = 50.0f;
	m_fCameraRotSpeed = 0.01f;
	m_iLastMousePosX = 0;
	m_iLastMousePosY = 0;
	m_bTrackingMouse = false;
	m_kOrbitPt = NiPoint3::ZERO;

	m_fDraconisFov		= g_settings.CAMERA_DRACONIS_FOV;
	m_fDraconisPitch	= g_settings.CAMERA_DRACONIS_PITCH;
	m_fDraconisYaw		= g_settings.CAMERA_DRACONIS_YAW;
	m_fDraconisDistance	= g_settings.CAMERA_DRACONIS_DISTANCE;

	m_fDistanceMin		= g_settings.CAMERA_DRACONIS_DISTANCEMIN;
	m_fDistanceMax		= g_settings.CAMERA_DRACONIS_DISTANCEMAX;
	m_fAngleZ = m_fYaw[m_iActiveLevel];

	m_fFarPitchStart = m_fFarPitchEnd = m_fPitch[0];
	m_bShift = false;

	m_fRotateJudge = 0.0f;
}

CA_DraconisCamera::~CA_DraconisCamera()
{
}
//---------------------------------------------------------------------------	
bool CA_DraconisCamera::RecvMessage( MG_MessageBase& rkMessage )
{
	MG_MessageHandlerInterface::RecvMessage(rkMessage);

	BEGIN_MESSAGE_HANDLE
		MESSAGE_HANDLE(MGT_TIMER_TICK,_event_timerTick)
		MESSAGE_HANDLE(MGT_LOGIC_PLAYER_MOVE,	_event_LogicPlayerMove)
		MESSAGE_HANDLE(MGT_INPUT_MOUSE_MOVE,_event_InputMouseMove)
		MESSAGE_HANDLE(MGT_INPUT_MOUSE_WHEEL , _event_InputMouseWheel )
		MESSAGE_HANDLE(MGT_INPUT_MOUSE_MBUTTON_DOWN, _event_InputRMouseDown)
		MESSAGE_HANDLE(MGT_INPUT_MOUSE_MBUTTON_UP, _event_InputRMouseUp)
		MESSAGE_HANDLE(MGT_INPUT_KEY_ALT_V, _event_InputKeyAlt_VDown)
	END_MESSAGE_HANDLE
	return true;
}
void CA_DraconisCamera::_event_timerTick( MG_MessageBase& rkMessage )
{
	if( !m_bInTransitionProc )
    {
        //m_spNiCamera->SetRotate(m_kRotation);
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
		_on_active_level_changed(m_iActiveLevel);
	}
	_reset_camera( fFov , m_fAngleZ , fPitch );
	_compute_camera_translate(fDistance,fNearHeight , fFarHeight );
}
//---------------------------------------------------------------------------	
void CA_DraconisCamera::_event_InputMouseWheel( MG_MessageBase& rkMessage )
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
	m_kNearHeightSmoother.Init( 0 , m_fNearCameraHeightBias - m_fFarCameraHeightBias , m_fSmoothTotalTime , 0 , 0 , true);
	m_kFarHeightSmoother.Init( 0 , m_fFarCameraHeightBias , m_fSmoothTotalTime , 0 , 0 , true);

	SendMessage( CreateMsg_UE_SHOW_PLAYER_HEADALL(m_iActiveLevel));
}


void CA_DraconisCamera::ResetCamera()
{
    m_fAngleZ = m_fYaw[m_iActiveLevel];
	if ( m_spNiCamera!= NULL )
	{
		if (CA_CameraHelp::GamePlayer::GetNiPosition(m_kCurrentLookAtLocation))
        {
            m_kLastLookAtLocation = m_kCurrentLookAtLocation;
        }

		_reset_camera(m_fFov[m_iActiveLevel], m_fAngleZ, m_fPitch[m_iActiveLevel]);
        _compute_camera_translate( m_fDistance[m_iActiveLevel] , 0.0f , 0.0f );
		_on_active_level_changed(m_iActiveLevel);

		CA_BaseCamera::ResetCamera();
	}
}

NiPoint3 CA_DraconisCamera::GetListenerLocation()
{
	return m_kListenerLocation;
}

void CA_DraconisCamera::_event_InputMouseMove( MG_MessageBase& rkMessage )
{
	if(TEST_MESSAGE_HANDLED(rkMessage))
	{
		return;
	}
	POINT Delta;
	Delta.x = rkMessage.m_uiParam1 - m_iLastMousePosX;
	Delta.y = rkMessage.m_uiParam2 - m_iLastMousePosY;
	m_iLastMousePosX = rkMessage.m_uiParam1;
	m_iLastMousePosY = rkMessage.m_uiParam2;


	if (m_fRotateJudge != 0.0f && GET_PROCESS_TIME - m_fRotateJudge > 0.1f)
	{
		m_bTrackingMouse = true;
	}

	if(m_bTrackingMouse)
	{
		POINT Pos;
		Pos.x = rkMessage.m_uiParam1;
		Pos.y = rkMessage.m_uiParam2;

		//if ( ( GetAsyncKeyState(VK_LSHIFT) & 0x8000 ) && m_iActiveLevel == 0 )
		//{
		//	m_fPitch[m_iActiveLevel] = m_fPitch[m_iActiveLevel] + (float)Delta.y * 0.005f * ( m_fFarPitchEnd - m_fFarPitchStart );
		//	m_fPitch[m_iActiveLevel] = Utility::UT_MathBase::ClampFloat( m_fPitch[m_iActiveLevel] , m_fFarPitchStart , m_fFarPitchEnd );
		//	_reset_camera(m_fFov[m_iActiveLevel],m_fAngleZ,m_fPitch[m_iActiveLevel] );
		//	
		//	MG_MessageCenter::GetSingleton().SendMessage( CreateMsg_CAMERA_ROT( 
		//		m_fAngleZ , m_fPitch[m_iActiveLevel] ) );

		//}
		//else
		{
			RECT   rt;   
			POINT  tPoint;
			GetCursorPos(&tPoint); 
			GetWindowRect(GfxGetCurrentWndHandle(),&rt);
			if(( tPoint.x > rt.left) && (tPoint.y > rt.top) && (tPoint.x < rt.right) && (tPoint.y < rt.bottom) && (rkMessage.m_lParam3 & MK_MBUTTON))		//鼠标指针在主窗口内
			{
				_OnRotateCamera(  Delta , Pos );
			}
			else
			{
				m_bTrackingMouse = false;
			}
		}
	}
}

void CA_DraconisCamera::_event_InputMouseMButtonUp( MG_MessageBase& rkMessage )
{
	m_bTrackingMouse  = false;
}

void CA_DraconisCamera::_OnRotateCamera( const POINT& kDelta,const POINT& kPos )
{
	if (GetAsyncKeyState(VK_LMENU) & 0x8000) 
	{
	}
	else
	{
		m_fAngleZ -= kDelta.x * 0.3f;
	}

    _reset_camera(m_fFov[m_iActiveLevel], m_fAngleZ, m_fPitch[m_iActiveLevel]);

	 MG_MessageCenter::GetSingleton().SendMessage( CreateMsg_CAMERA_ROTATE( 
		 m_fAngleZ , m_fPitch[m_iActiveLevel] ) );
}

void CA_DraconisCamera::_event_LogicPlayerMove( MG_MessageBase& rkMessage )
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
//---------------------------------------------------------------------------
void CA_DraconisCamera::_event_InputRMouseDown( MG_MessageBase& rkMessage )
{
	if(TEST_MESSAGE_HANDLED(rkMessage))
	{
		return;
	}

	m_fRotateJudge = GET_PROCESS_TIME;
}
void CA_DraconisCamera::_event_InputRMouseUp( MG_MessageBase& rkMessage )
{
	m_bTrackingMouse  = false;

	m_fRotateJudge = 0.0f;
}

void CA_DraconisCamera::_event_InputKeyAlt_VDown( MG_MessageBase& rkMessage )
{
	ResetCamera();
}

void CA_DraconisCamera::SetPitchRange( float fStart , float fEnd )
{
	m_fFarPitchStart = fStart;
	m_fFarPitchEnd   = fEnd;
	SetPitch( 0 , fStart );
}
