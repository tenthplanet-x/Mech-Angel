#include "ClientPCH.h"
#include "MG_MessageDefine.h"
#include "MG_MessageBase.h"
#include "CA_BossSeparateCamera.h"
#include "CA_CameraHelpper.h"
#include "LC_ClientWorldManager.h"
#include "LC_ClientLogicManager.h"
#include "LC_ClientNPCManager.h"
#include "RG_RenderManager.h"
#include "UI_UIManager.h"

#include "CF_BossCamera.h"

using namespace Camera;

#define BOSS_SHOWCAMERA_FOV 30

CA_BossSeparateCamera::CA_BossSeparateCamera( NiCameraPtr pkObject )
:CA_BaseCamera( pkObject )
, m_enmState(CMS_NA)
{
	m_fStopTime = FLT_MAX;
	m_fTimer = 0.0f;
	m_lTargetID = 0;

	m_startYaw = 0.0f;
	m_lastYaw = 0.0f;
	m_targetYaw = 0.0f;

	m_fStartPitch = 0.0f;
	m_fPitch = 0.0f;
	m_fTargetPitch = 0.0f;

	m_fStartRoll = 0.0f;
	m_fRoll = 0.0f;
	m_fTargetRoll = 0.0f;

	m_StartDistance = 0.0f;
	m_Distance = 0.0f;
	m_TargetDistance = 0.0f;

	m_fStartHeight = 0.0f;
	m_fHeight = 0.0f;
	m_fTargetHeight = 0.0f;

	m_bBackToMc = false;
}

CA_BossSeparateCamera::~CA_BossSeparateCamera()
{

}

bool CA_BossSeparateCamera::RecvMessage( MG_MessageBase& rkMessage )
{
	MG_MessageHandlerInterface::RecvMessage(rkMessage);
	BEGIN_MESSAGE_HANDLE
		MESSAGE_HANDLE(MGT_TIMER_TICK,_eventTimerTick)
	END_MESSAGE_HANDLE
	return true;
}

void CA_BossSeparateCamera::_eventTimerTick( MG_MessageBase& rkMessage )
{
	float fDTime = rkMessage.m_fParam2;
	switch(m_enmState)
	{
	case CMS_STOP_ON_TARGET:
		{
			m_fTimer -= fDTime;
			if (m_fTimer <= 0)
			{
				this->back();
			}
			else
			{
				float delta = (m_targetYaw - m_startYaw) * fDTime / m_fStopTime;
				m_lastYaw += delta;
				
				delta = (m_fTargetPitch - m_fStartPitch) * fDTime / m_fStopTime;
				m_fPitch += delta;

				delta = (m_fTargetRoll - m_fStartRoll) * fDTime / m_fStopTime;
				m_fRoll += delta;
				
				_reset_camera(BOSS_SHOWCAMERA_FOV, m_lastYaw, m_fPitch, m_fRoll);

				LC_ClientWorldManager* pkWorldManager = CLIENT_GET_WORLD_MANAGER();
				LC_ActorBase* pkTarget = (LC_ActorBase*)pkWorldManager->FindObject(m_lTargetID);
				if (pkTarget)
				{
					float deltaDist = (m_TargetDistance- m_StartDistance) * fDTime / m_fStopTime;
					m_Distance += deltaDist;

					deltaDist = (m_fTargetHeight - m_fStartHeight) * fDTime / m_fStopTime;
					m_fHeight += deltaDist;

					float fAngle = pkTarget->GetSpawnZRotAngle();
					const UT_Vec2Float& kCurrentForward = pkTarget->GetForwardDir();
					Utility::UT_Vec2Float newAt = UT_MathBase::ZRot(kCurrentForward, m_lastYaw+fAngle);
					UT_Vec3Int kPos = UT_MathBase::V3dIncrementAdd(UT_Vec3Int(m_targetPos.x, m_targetPos.y, m_targetPos.z), newAt, m_Distance);
					NiPoint3 camPos = NiPoint3(kPos.x, kPos.y, m_fHeight);
					m_spNiCamera->SetTranslate(camPos);
				}
			}
		}
		break;

	case CMS_BACK_TO_MC:
		{
			ShowOther(true);
			m_enmState = CMS_END;
		}
		break;
	}

	m_spNiCamera->Update( 0.0f );
}

void CA_BossSeparateCamera::_releaseScreenFx()
{
	LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
	LC_ClientNPC* pkNPC = (LC_ClientNPC* )pkNPCManager->FindNPC(m_lTargetID);
	if (pkNPC)
	{
		const CF_BossCamera::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_BossCamera>(pkNPC->GetCharType());
		if(pkData)
			pkNPC->StopScreenFx(pkData->_sFx);
	}
}

bool  CA_BossSeparateCamera::OnAttach()
{
	m_fTimer = 0;
	//m_enmState = CMS_NA;
	CA_CameraHelp::GamePlayer::SetAIControl(true);
	return true;
}

void  CA_BossSeparateCamera::OnDetach()
{
	_releaseScreenFx();
	ShowOther(true);

	m_fTimer = 0;
	m_enmState = CMS_END;
	CA_CameraHelp::GamePlayer::SetAIControl(false);
}

void  CA_BossSeparateCamera::go(bool bBackToMc)
{
	m_bBackToMc = bBackToMc;

	LC_ClientWorldManager* pkWorldManager = CLIENT_GET_WORLD_MANAGER();
	LC_ActorBase* pkTarget = (LC_ActorBase*)pkWorldManager->FindObject(m_lTargetID);
	if(pkTarget == NULL)
	{
		back();
		return;
	}

	m_targetPos = UT_EngineMath::ConvertVec3IntToNiPoint3(pkTarget->GetVisibleLocation());

	const UT_Vec2Float& kCurrentForward = pkTarget->GetForwardDir();
	UT_Vec2Float newAt = UT_MathBase::ZRot(kCurrentForward, m_startYaw);

	float fAngle = pkTarget->GetSpawnZRotAngle();
	m_startYaw -= fAngle;
	m_targetYaw -= fAngle;
	m_lastYaw = m_startYaw;
	_reset_camera(BOSS_SHOWCAMERA_FOV, m_startYaw, m_fPitch, m_fStartRoll);
	
	UT_Vec3Int kPos = UT_MathBase::V3dIncrementAdd(UT_Vec3Int(m_targetPos.x, m_targetPos.y, m_targetPos.z), newAt, m_StartDistance);
	m_srcPos = NiPoint3(kPos.x, kPos.y, m_fHeight);
	m_spNiCamera->SetTranslate( m_srcPos );

	m_enmState = CMS_STOP_ON_TARGET;
	m_fTimer = m_fStopTime;
}

void  CA_BossSeparateCamera::back(void)
{
	if(!m_bBackToMc)
	{
		m_enmState = CMS_END;
		return;
	}

	m_enmState = CMS_BACK_TO_MC;

	//主角位置
	NiPoint3 kMCPos = NiPoint3::ZERO;
	CA_CameraHelp::GamePlayer::GetNiPosition(kMCPos);

	//镜头位置
	m_targetPos = m_spNiCamera->GetWorldLocation();
}

void  CA_BossSeparateCamera::ShowOther(bool bShow)
{
	RG_RenderManager::GetSingleton().EnableUIRenderStep(bShow);

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_NOTIFY_BOSS_CLOSEUP);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, !bShow);

	UI_UIManager::GetSingletonPtr()->HandlerGameEvent(kGameEvent);

	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();

	pkGamePlayer->ShowAllBuddyVisibleData(bShow);

}
