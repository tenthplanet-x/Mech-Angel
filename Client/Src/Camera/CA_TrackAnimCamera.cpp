#include "ClientPCH.h"
#include "CA_CameraHelpper.h"
#include "CA_TrackAnimCamera.h"
#include "MG_MessageBase.h"

using namespace Camera;

bool CA_TrackAnimCamera::RecvMessage( MG_MessageBase& rkMessage )
{
	MG_MessageHandlerInterface::RecvMessage(rkMessage);
	BEGIN_MESSAGE_HANDLE
		MESSAGE_HANDLE(MGT_TIMER_TICK,_eventTimerTick)
	END_MESSAGE_HANDLE
	return true;
}


CA_TrackAnimCamera::CA_TrackAnimCamera( NiCameraPtr pkObject )
	:CA_BaseCamera( pkObject )
	,m_pkCurrentPath( NULL )
	,m_kAutoStop(false)
	,m_pkMaxPath(NULL)
{
	
}

CA_TrackAnimCamera::~CA_TrackAnimCamera()
{

}

void CA_TrackAnimCamera::_eventTimerTick( MG_MessageBase& rkMessage )
{
	if ( m_pkCurrentPath )
	{
		float t = NiGetCurrentTimeInSec();
		m_pkCurrentPath->Update(t);

		NiNode*  node = m_pkCurrentPath->GetControllerNode();
		m_spNiCamera->SetRotate(node->GetRotate());
		m_spNiCamera->SetTranslate(node->GetTranslate());

		float fov = m_pkCurrentPath->GetFov();
		this->SetFov(fov);	//update(0) in it
	}
	else if ( m_pkMaxPath )
	{
		float t = NiGetCurrentTimeInSec();
		m_pkMaxPath->Update(t);
		
		NiNode*  node = m_pkMaxPath->GetControllerNode();
		m_spNiCamera->SetRotate(node->GetRotate());
		m_spNiCamera->SetTranslate(node->GetTranslate());
		m_spNiCamera->Update(0.0f);
	}
}

bool  CA_TrackAnimCamera::OnAttach()
{
	bool b = CA_BaseCamera::OnAttach();

	CA_CameraHelp::GamePlayer::SetAIControl(true);

	return b;
}

void  CA_TrackAnimCamera::OnDetach()
{
	CA_CameraHelp::GamePlayer::SetAIControl(false);

	if (m_pkCurrentPath)
	{
		m_pkCurrentPath->SetPlay(false);
		m_pkCurrentPath = NULL;
	}

	if ( m_pkMaxPath )
	{
		m_pkMaxPath->SetPlay(false);
		m_pkMaxPath = NULL;
	}
}

bool  CA_TrackAnimCamera::PlayAnim(int trackIdx)
{
	m_pkCurrentPath = CA_CameraAnimationManager::Instance()->GetAnimationPath(trackIdx);
	if (m_pkCurrentPath)
	{
		m_pkCurrentPath->SetPlay(true);
		return true;
	}

	return false;
}

bool CA_TrackAnimCamera::PlayMaxAnim(int chartpyeid, const NiPoint3& src)
{
	m_pkMaxPath = CA_CameraAnimationManager::GetSingleton().GetMaxAnimationPath(chartpyeid);
	if ( m_pkMaxPath )
	{
		m_pkMaxPath->SetPlay(true);
		m_pkMaxPath->SetSrcPos(src);
		return true;
	}
	return false;	
}

void CA_TrackAnimCamera::StopAnim()
{
	if (m_pkCurrentPath)
	{
		m_pkCurrentPath->SetPlay(false);
	}
}
bool CA_TrackAnimCamera::SetKeyCamera(int trackIdx, int keyIdx, NiPoint3& tp, NiQuaternion& qua, float& fov)
{
	CA_EditorAnimationPath* path = (CA_EditorAnimationPath*)CA_CameraAnimationManager::Instance()->GetAnimationPath(trackIdx);
	if (path)
	{
		const PathKey* key = path->GetKey(keyIdx);
		if (key)
		{
			tp = key->kPos;
			qua = key->kRot;
			fov = path->GetFov();
			return true;
		}
		return false;
	}
	return false;
}

int CA_TrackAnimCamera::GetKeyCameraCount(int trackIdx)
{
	CA_EditorAnimationPath* path = (CA_EditorAnimationPath*)CA_CameraAnimationManager::Instance()->GetAnimationPath(trackIdx);
	if (path)
	{
		return path->GetKeyCount();
	}
	return 0;
}

void CA_TrackAnimCamera::ModifyKey(int trackIdx, int keyIdx, const NiPoint3& tp , const NiQuaternion& qua)
{
	CA_EditorAnimationPath* path = (CA_EditorAnimationPath*)CA_CameraAnimationManager::Instance()->GetAnimationPath(trackIdx);
	if (path)
	{
		path->SetPos(keyIdx, tp);
		path->SetRotate(keyIdx, qua);
	}
}	 

void CA_TrackAnimCamera::AddKey(int trackIdx, int keyIdx, const NiPoint3& tp , const NiQuaternion& qua)
{
	CA_EditorAnimationPath* path = (CA_EditorAnimationPath*)CA_CameraAnimationManager::Instance()->GetAnimationPath(trackIdx);
	if (path)
	{
		path->InsertKey(keyIdx, tp	, qua);
	}
}

void CA_TrackAnimCamera::RemoveKey(int trackIdx, int keyIdx)
{
	CA_EditorAnimationPath* path = (CA_EditorAnimationPath*)CA_CameraAnimationManager::Instance()->GetAnimationPath(trackIdx);
	if (path)
	{
		path->RemoveKey(keyIdx);
	}
}

bool CA_TrackAnimCamera::IsNeedEnd()
{
	if (m_pkCurrentPath)
	{
		return !m_pkCurrentPath->GetPlay() && m_kAutoStop;
	}
	else if ( m_pkMaxPath )
	{
		return !m_pkMaxPath->GetPlay() && m_kAutoStop;
	}
	return true;
}

bool CA_TrackAnimCamera::IsCurrentPlay()
{
	if( m_pkMaxPath )
	{
		return m_pkMaxPath->GetPlay();
	}
	return false;
}