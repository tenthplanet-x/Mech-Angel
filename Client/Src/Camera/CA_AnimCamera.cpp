#include "ClientPCH.h"
#include "CA_AnimCamera.h"
#include "MG_MessageBase.h"
#include "UT_CutSceneManager.h"

using namespace Camera;

CA_AnimCamera::CA_AnimCamera( NiCameraPtr pkObject )
:CA_BaseCamera( pkObject )
{
	
}

CA_AnimCamera::~CA_AnimCamera()
{

}

bool CA_AnimCamera::RecvMessage( MG_MessageBase& rkMessage )
{
	MG_MessageHandlerInterface::RecvMessage(rkMessage);
	BEGIN_MESSAGE_HANDLE
		MESSAGE_HANDLE(MGT_TIMER_TICK,_eventTimerTick)
	END_MESSAGE_HANDLE
	return true;
}

void CA_AnimCamera::_eventTimerTick( MG_MessageBase& rkMessage )
{
	if ( UT_CutSceneManager::GetSingletonPtr() && 
		UT_CutSceneManager::GetSingleton().IsPlayerMoveDisabled() )
	{
		UT_CutSceneManager::GetSingleton().Update( NiGetCurrentTimeInSec() );
		NiCameraPtr spAnimCam = UT_CutSceneManager::GetSingleton().GetAnimCamera();
		NiPoint3 kPos = spAnimCam->GetWorldTranslate();
		NiFrustum kFrust = spAnimCam->GetViewFrustum();
		
		NiMatrix3 kRot = spAnimCam->GetWorldRotate(  );

		m_spNiCamera->SetTranslate( kPos );
		m_spNiCamera->SetRotate( kRot );
		m_spNiCamera->SetViewFrustum( kFrust );
		m_spNiCamera->Update( 0.0f );
		//SendMessage( CreateMsg_LOGIC_PLAYER_MOVE( kPos.x , kPos.y , kPos.z ) );
        UT_CutSceneManager::GetSingleton().SetPlayerLocation(kPos);
	}
}
