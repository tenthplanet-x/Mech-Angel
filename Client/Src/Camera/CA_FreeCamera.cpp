#include "ClientPCH.h"

#include "CA_FreeCamera.h"
#include "CA_CameraManager.h"
#include "IM_Manager.h"
#include "LC_ClientLogicManager.h"
#include "MG_MessageBase.h"
#include "UT_CutSceneManager.h"
#include "UT_LogicInfoMessage.h"

using namespace Utility;
//---------------------------------------------------------------------------
CA_FreeCamera::CA_FreeCamera( NiCameraPtr pkObject )
: CA_BaseCamera(pkObject)
{
	m_fCameraMoveSpeed = 50.0f;
	m_fCameraRotSpeed = 0.01f;

	m_iLastMousePosX = 0;
	m_iLastMousePosY = 0;
	m_bTrackingMouse = false;
	m_kOrbitPt = NiPoint3::ZERO;

	for(int i = CA_FREE_UP ; i < CA_FREE_MAX; i++ )
	{
		m_UDLR[i] = false;
	}
	m_cspeed = 0;

	m_lastYaw = 140.0f;
	m_lastPitch = 50.0f;
	m_lastDistance = 2800.0f;
	m_lastZ = 0.0f;

	m_lockLocation = NiPoint3::ZERO;

    ResetCamera();
}
//---------------------------------------------------------------------------	
CA_FreeCamera::~CA_FreeCamera()
{
}
//---------------------------------------------------------------------------
bool CA_FreeCamera::RecvMessage( MG_MessageBase& rkMessage )
{
	MG_MessageHandlerInterface::RecvMessage(rkMessage);

	BEGIN_MESSAGE_HANDLE
        MESSAGE_HANDLE(MGT_TIMER_TICK, _eventTimerTick)
		MESSAGE_HANDLE(MGT_INPUT_KEY_UP,_event_InputKeyW)
		MESSAGE_HANDLE(MGT_INPUT_KEY_DOWN,_event_InputKeyS)
		MESSAGE_HANDLE(MGT_INPUT_KEY_LEFT,_event_InputKeyA)
		MESSAGE_HANDLE(MGT_INPUT_KEY_RIGHT,_event_InputKeyD)
		MESSAGE_HANDLE(MGT_INPUT_KEY_PAGE_PRIOIR,_event_InputPgUp)
		MESSAGE_HANDLE(MGT_INPUT_KEY_PAGE_NEXT,_event_InputpgDn)
		MESSAGE_HANDLE(MGT_INPUT_KEY_COMMA, _event_InputComma)
		MESSAGE_HANDLE(MGT_INPUT_KEY_PERIOD,_event_InputPeriod)
	    MESSAGE_HANDLE(MGT_INPUT_MOUSE_MOVE,_event_InputMouseMove)
	    MESSAGE_HANDLE(MGT_INPUT_MOUSE_WHEEL , _event_InputMouseWheel );
	    MESSAGE_HANDLE(MGT_INPUT_MOUSE_MBUTTON_DOWN , _event_InputMouseMButtonDown );
	    MESSAGE_HANDLE(MGT_INPUT_MOUSE_MBUTTON_UP , _event_InputMouseMButtonUp );
	END_MESSAGE_HANDLE
	return true;
}
//-----------------------------------------------------------------
bool CA_FreeCamera::OnAttach()
{
	m_lastYaw = 140.0f;
	m_lastPitch = 50.0f;
	m_lastDistance = 2800.0f;
	m_lastZ = 0.0f;
	NiPoint3 kCameraDir = m_spNiCamera->GetWorldDirection();
	m_lockLocation = m_spNiCamera->GetTranslate() + m_lastDistance * kCameraDir; 

	return true;
}
//-----------------------------------------------------------------
void CA_FreeCamera::_eventTimerTick(MG_MessageBase& rkMessage)
{
	bool kFlag = false;
	for (int i = 0 ; i < CA_FREE_ZDOWN+1; i++)
	{
		if ( m_UDLR[i])
		{
			NiPoint3 dc = _calcuteDelta(i);
			m_lockLocation += dc * m_cspeed;
			kFlag = true;
			m_UDLR[i] = false;
		}
	}

	for (int i = CA_FREE_DIS ; i < CA_FREE_YAW+1; i++)
	{
		if ( m_UDLR[i])
		{
			kFlag = true;
			m_UDLR[i] = false;
		}
	}

	if ( kFlag )
	{
		_reset_camera(30.0f,m_lastYaw,m_lastPitch);
		NiPoint3 kCameraDir = m_spNiCamera->GetWorldDirection();
		m_kLastWorldXlate = m_lockLocation - kCameraDir * m_lastDistance;
		m_spNiCamera->SetTranslate(m_kLastWorldXlate);
		m_spNiCamera->Update(0.0f);
	}
	
}
//-----------------------------------------------------------------
void CA_FreeCamera::_event_InputKeyW(MG_MessageBase& rkMessage)
{
	m_UDLR[CA_FREE_UP] = true;
}
//-----------------------------------------------------------------
void CA_FreeCamera::_event_InputKeyS(MG_MessageBase& rkMessage)
{
	m_UDLR[CA_FREE_DOWN] = true;
}
//-----------------------------------------------------------------
void CA_FreeCamera::_event_InputKeyA(MG_MessageBase& rkMessage)
{
	m_UDLR[CA_FREE_LEFT] = true;
}
//-----------------------------------------------------------------
void CA_FreeCamera::_event_InputKeyD(MG_MessageBase& rkMessage)
{
	m_UDLR[CA_FREE_RIGHT] = true;
}
//-----------------------------------------------------------------
void CA_FreeCamera::_event_InputComma(MG_MessageBase& rkMessage)
{
	m_UDLR[CA_FREE_ZUP] = true;
}
//-----------------------------------------------------------------
void CA_FreeCamera::_event_InputPeriod(MG_MessageBase& rkMessage)
{
	m_UDLR[CA_FREE_ZDOWN] = true;
}
//-----------------------------------------------------------------
void CA_FreeCamera::_event_InputPgUp(MG_MessageBase& rkMessage)
{
	m_cspeed += 10;
	char buf[256];
	sprintf_s(buf,256, "当前镜头速度为%f", m_cspeed);

	StringType st = buf;
	SendLogicInfoMessage(RE_CHAT_NEAR,st);

}
//-----------------------------------------------------------------
void CA_FreeCamera::_event_InputpgDn(MG_MessageBase& rkMessage)
{
	m_cspeed -= 10;
	char buf[256];
	sprintf_s(buf,256, "当前镜头速度为%f", m_cspeed);

	StringType st = buf;
	SendLogicInfoMessage(RE_CHAT_NEAR,st);
}
//-----------------------------------------------------------------
void CA_FreeCamera::_event_InputMouseMove(MG_MessageBase& rkMessage)
{
	POINT Delta;
	Delta.x = rkMessage.m_uiParam1 - m_iLastMousePosX;
	Delta.y = rkMessage.m_uiParam2 - m_iLastMousePosY;
	m_iLastMousePosX = rkMessage.m_uiParam1;
	m_iLastMousePosY = rkMessage.m_uiParam2;

	m_fCameraMoveSpeed = UT_CutSceneManager::GetSingleton().GetFreeCameraMoveSpeed();
	m_fCameraRotSpeed = UT_CutSceneManager::GetSingleton().GetFreeCameraRotateSpeed();

	if( GetAsyncKeyState(MK_RBUTTON) & 0x8000)
	{
		if ( Delta.x < 0 )
		{
			m_UDLR[CA_FREE_LEFT] = true;
		}
		else if ( Delta.x > 0)
		{
			m_UDLR[CA_FREE_RIGHT] = true;
		}

		if ( Delta.y < 0 )
		{
			m_UDLR[CA_FREE_UP] = true;
		}
		else if ( Delta.y > 0)
		{
			m_UDLR[CA_FREE_DOWN] = true;
		}
	}
	if(m_bTrackingMouse)
	{
		m_lastYaw -= Delta.x * 0.3f;
		m_lastPitch -= Delta.y * 0.3f;
		m_UDLR[CA_FREE_YAW] = true;
		//_OnRotateCamera(  Delta , Pos );
	}
}
void CA_FreeCamera::_event_InputMouseWheel(MG_MessageBase &rkMessage)
{
	int iZDelta = GET_WHEEL_DELTA_WPARAM(rkMessage.m_lParam3);
	NiPoint3 kCameraTranslate = m_spNiCamera->GetTranslate();
	NiPoint3 kCameraDir = m_spNiCamera->GetWorldDirection();

	m_lastDistance -= iZDelta * 0.3f;
	m_UDLR[CA_FREE_DIS] = true;
}

void CA_FreeCamera::_event_InputMouseMButtonDown( MG_MessageBase& rkMessage )
{
	m_bTrackingMouse = true;
}

void CA_FreeCamera::_event_InputMouseMButtonUp( MG_MessageBase& rkMessage )
{
	m_bTrackingMouse  = false;
}

void CA_FreeCamera::_OnMouseMoveCamera( const POINT& kDelta , POINT kPos )
{
	int dx,dy;
	dx = kDelta.x;
	dy = kDelta.y;

	if((dx)&&(dy))
	{
		if(abs(dx) > abs(dy))
		{
			dy = 0;
		}
		else
		{
			dx = 0;
		}
	}

	NiPoint3 kNewTranslation;
	float fPanSpeedX, fPanSpeedY;
	NiFrustum kFrustum = m_spNiCamera->GetViewFrustum();


	fPanSpeedX = (float)(kDelta.x)*1.8f;
	fPanSpeedY = (float)(kDelta.y)*1.8f;

	const NiPoint3 kCamTranslate = m_spNiCamera->GetTranslate();
	const NiMatrix3 kCamRotate = m_spNiCamera->GetRotate();

	kNewTranslation = UT_ViewMath::Pan(fPanSpeedX, fPanSpeedY,
		m_spNiCamera->GetTranslate(), m_spNiCamera->GetRotate());

    _moveCamera(kNewTranslation);

	return;
	
}

void CA_FreeCamera::_OnRotateCamera( const POINT& kDelta,const POINT& kPos )
{
	NiPoint2 kRotDegree = NiPoint2::ZERO;

	if (GetAsyncKeyState(VK_LMENU) & 0x8000) 
	{
		kRotDegree.y = ( float )kDelta.y * m_fCameraRotSpeed;	
	}
	else
	{
		kRotDegree.x = (float)kDelta.x * m_fCameraRotSpeed;	
	}
	NiPoint3 kNewLocation = NiPoint3::ZERO;
	NiMatrix3 kNewRotation = NiMatrix3::IDENTITY;
	NiPoint3 kUpAxis = NiPoint3::UNIT_Z;
	UT_ViewMath::Orbit( kRotDegree.x , kRotDegree.y, 
		m_spNiCamera->GetTranslate(), m_spNiCamera->GetRotate(), 
		m_kOrbitPt, kUpAxis, kNewLocation, kNewRotation );
	/*GfxWriteLog( LOG_TAG_INFO , LOG_SWITCH_INFO ,
		"rot camera at [%f %f %f] to [%f %f %f]" ,
		m_kOrbitPt.x , m_kOrbitPt.y , m_kOrbitPt.z ,
		kNewLocation.x , kNewLocation.y , kNewLocation.z );*/
	m_spNiCamera->SetRotate(kNewRotation);
	m_spNiCamera->Update(0.0f);

    _moveCamera(kNewLocation);
}

void CA_FreeCamera::ResetCamera()
{
    m_kRelativeXlate = NiPoint3::ZERO;
    m_kLastWorldXlate = NiPoint3::ZERO;

	CA_BaseCamera::ResetCamera();
}

NiPoint3 CA_FreeCamera::GetLockPos()
{
	NiPoint3 sd = m_lockLocation;
	sd.z += m_lastZ;
	return sd;
}

void CA_FreeCamera::MoveRotateTo(const NiPoint3& tp, const NiQuaternion& qua, float fov)
{
	m_spNiCamera->SetRotate(qua);
	m_spNiCamera->Update(0.0f);
	_moveCamera(tp);

	SetFov(fov);
}

void CA_FreeCamera::_moveCamera(const NiPoint3& kCameraWorldXlate)
{
    object_id_type lFreeCameraFollowPlayerID = UT_CutSceneManager::GetSingleton().GetFreeCameraFollowPlayerID();
    if (lFreeCameraFollowPlayerID != ((LC_ClientLogicManager&)LC_ClientLogicManager::GetSingleton()).GetOperationPlayerID())
    {
        GD_ClientActorModel* pkActorModel = IM_Manager::GetSingleton().GetActor(lFreeCameraFollowPlayerID);
        if (pkActorModel != NULL)
        {
            m_kRelativeXlate = kCameraWorldXlate - pkActorModel->GetTranslate();
        }
        else
        {
            m_kRelativeXlate = NiPoint3::ZERO;
            m_kLastWorldXlate = kCameraWorldXlate;
        }
    }
    else
    {
        m_kRelativeXlate = NiPoint3::ZERO;
        m_kLastWorldXlate = kCameraWorldXlate;
    }
}

NiPoint3 CA_FreeCamera::_calcuteDelta(int iIndex)
{
	switch ( iIndex)
	{
	case CA_FREE_UP:
		{		
			NiPoint3 kCameraTranslate = m_spNiCamera->GetTranslate();
			NiPoint3 kCameraRightDir = m_spNiCamera->GetWorldRightVector();
			NiPoint3 kCameraUp = NiPoint3::UNIT_Z;
			NiPoint3 kCameraDir = kCameraRightDir.Cross( kCameraUp );
			kCameraDir.Unitize();
			
			return -1.0f *kCameraDir;   
		}
		break;
	case CA_FREE_DOWN:
		{
			NiPoint3 kCameraTranslate = m_spNiCamera->GetTranslate();
			NiPoint3 kCameraRightDir = m_spNiCamera->GetWorldRightVector();
			NiPoint3 kCameraUp = NiPoint3::UNIT_Z;
			NiPoint3 kCameraDir = kCameraRightDir.Cross( kCameraUp );
			kCameraDir.Unitize();

			return kCameraDir;
		}
		break;
	case CA_FREE_LEFT:
		{
			NiPoint3 kCameraTranslate = m_spNiCamera->GetTranslate();
			NiPoint3 kCameraRightDir = m_spNiCamera->GetWorldRightVector();

			return -1.0f * kCameraRightDir;
		}
		break;
	case CA_FREE_RIGHT:
		{
			NiPoint3 kCameraTranslate = m_spNiCamera->GetTranslate();
			NiPoint3 kCameraRightDir = m_spNiCamera->GetWorldRightVector();

			return kCameraRightDir;
		}
		break;
	case CA_FREE_ZUP:
		{
			m_lastZ += m_cspeed;
			return NiPoint3::UNIT_Z;
		}
		break;
	case CA_FREE_ZDOWN:
		{
			m_lastZ	-= m_cspeed;
			return NiPoint3::UNIT_Z * -1.0f;
		}
		break;
	default:
		{
			return NiPoint3::ZERO;
		}
		break;
	}
}