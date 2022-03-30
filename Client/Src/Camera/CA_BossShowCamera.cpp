#include "ClientPCH.h"
#include "MG_MessageDefine.h"
#include "MG_MessageBase.h"
#include "CA_BossShowCamera.h"
#include "CA_CameraHelpper.h"
#include "LC_ClientWorldManager.h"
#include "LC_ClientLogicManager.h"

using namespace Camera;

CA_BossShowCamera::CA_BossShowCamera( NiCameraPtr pkObject )
:CA_BaseCamera( pkObject )
, m_enmState(CMS_NA)
{
	m_fTimer = 0;
	m_lTargetID = 0;
	m_lastYaw = 0;
	m_caDistance = 0;

	this->setMoveArg(0.5f, 0.5f);
	m_backLockPlayer = false;
}

CA_BossShowCamera::~CA_BossShowCamera()
{

}

bool CA_BossShowCamera::RecvMessage( MG_MessageBase& rkMessage )
{
	MG_MessageHandlerInterface::RecvMessage(rkMessage);
	BEGIN_MESSAGE_HANDLE
		MESSAGE_HANDLE(MGT_TIMER_TICK,_eventTimerTick)
	END_MESSAGE_HANDLE
	return true;
}

void CA_BossShowCamera::_eventTimerTick( MG_MessageBase& rkMessage )
{
	float fDTime = rkMessage.m_fParam2;
	switch(m_enmState)
	{
	case CMS_MOVE_TO_TARGET:
		{
			m_fTimer -= fDTime;
			if (m_fTimer <= 0)
			{
 				_reset_camera(30, 140.0, 50);
				
				NiPoint3 kPos = m_dstPos - m_spNiCamera->GetWorldDirection() * m_caDistance; /*m_relOffset*/;
				m_spNiCamera->SetTranslate( kPos );

				m_enmState = CMS_STOP_ON_TARGET;
				m_fTimer = m_fStopTime;
			}
			else
			{
				if ( m_lastYaw != 140.0f )
				{
					float delta = 140.0f - m_lastYaw;
					delta *= 1.0f - m_fTimer/m_fGoTime;
					_reset_camera(30, m_lastYaw + delta, 50);
				}
				NiPoint3 kDiff = m_dstPos - m_srcPos;
				kDiff *= 1.0f - m_fTimer/m_fGoTime;
				NiPoint3 kPos = m_srcPos + kDiff - m_spNiCamera->GetWorldDirection() * m_caDistance; /*m_relOffset*/;
				m_spNiCamera->SetTranslate( kPos );
			}
		}
		break;

	case CMS_STOP_ON_TARGET:
		{
			m_fTimer -= fDTime;
			if (m_fTimer <= 0)
			{
				this->back();
			}
			else
			{
				if (m_bFollow)
				{
					LC_ClientWorldManager* pkWorldManager = CLIENT_GET_WORLD_MANAGER();
					LC_LogicObject* pkTarget = pkWorldManager->FindObject(m_lTargetID);
					if (pkTarget)
					{
						NiPoint3 kTargetPos = UT_EngineMath::ConvertVec3IntToNiPoint3(pkTarget->GetVisibleLocation());
						NiPoint3 kPos = kTargetPos - m_spNiCamera->GetWorldDirection() * m_caDistance;/*m_relOffset*/;
						m_spNiCamera->SetTranslate( kPos );
					}
				}
			}
		}
		break;

	case CMS_BACK_TO_MC:
		{
			m_fTimer -= fDTime;
			if (m_fTimer <= 0)
			{
// 				_reset_camera(30, m_lastYaw, 50);

				NiPoint3 kPos = m_dstPos - m_spNiCamera->GetWorldDirection() * m_caDistance;/*m_relOffset*/;
				m_spNiCamera->SetTranslate( kPos );

				m_enmState = CMS_END;
			}
			else
			{
// 				if ( m_lastYaw != 140.0f )
// 				{
// 					float delta = m_lastYaw - 140.0f;
// 					delta *= 1.0f - m_fTimer/m_fGoTime;
// 					_reset_camera(30, 140.0f + delta, 50);
// 				}
				NiPoint3 kDiff = m_dstPos - m_srcPos;
				kDiff *= 1.0f - m_fTimer/m_fGoTime;
				NiPoint3 kPos = m_srcPos + kDiff - m_spNiCamera->GetWorldDirection() * m_caDistance;/* m_relOffse*/;
				m_spNiCamera->SetTranslate( kPos );
			}
		}
		break;
	}

	m_spNiCamera->Update( 0.0f );
}

bool  CA_BossShowCamera::OnAttach()
{
	m_fTimer = 0;
	m_enmState = CMS_NA;

	if ( !m_backLockPlayer ) 
	{
		CA_CameraHelp::GamePlayer::SetAIControl(true);
	}
	return true;
}

void  CA_BossShowCamera::OnDetach()
{
	m_fTimer = 0;
	m_enmState = CMS_END;

	if ( !m_backLockPlayer ) 
	{
		CA_CameraHelp::GamePlayer::SetAIControl(false);
	}
	m_backLockPlayer = false;
}		

void CA_BossShowCamera::backLockPlayer()
{
	m_backLockPlayer = true;
}

void  CA_BossShowCamera::go(object_id_type lTargetID)
{
	m_fTimer = m_fGoTime;
	m_enmState = CMS_MOVE_TO_TARGET;
	m_lTargetID = lTargetID;

	//主角位置
	NiPoint3 kMCPos = NiPoint3::ZERO;
	CA_CameraHelp::GamePlayer::GetNiPosition(kMCPos);

	//目标位置
	LC_ClientWorldManager* pkWorldManager = CLIENT_GET_WORLD_MANAGER();
	LC_LogicObject* pkTarget = pkWorldManager->FindObject(lTargetID);
	NiPoint3 kTargetPos = UT_EngineMath::ConvertVec3IntToNiPoint3(pkTarget->GetVisibleLocation());

	//镜头位置
	NiPoint3 kCamPos = m_spNiCamera->GetWorldLocation();

	//计算偏移值
	m_relOffset = kCamPos - kMCPos;

	m_srcPos = kMCPos;
	m_dstPos = kTargetPos;
}

void  CA_BossShowCamera::go(const NiPoint3 & kTargetPos)
{
	m_fTimer = m_fGoTime;
	m_enmState = CMS_MOVE_TO_TARGET;
	m_lTargetID = 0;
	m_bFollow = false;

	//主角位置
	NiPoint3 kMCPos = NiPoint3::ZERO;
	CA_CameraHelp::GamePlayer::GetNiPosition(kMCPos);

	//镜头位置
	NiPoint3 kCamPos = m_spNiCamera->GetWorldLocation();

	//计算偏移值
	m_relOffset = kCamPos - kMCPos;

	m_srcPos = kMCPos;
	m_dstPos = kTargetPos;
}

void  CA_BossShowCamera::back(void)
{
	m_fTimer = m_fBackTime;
	m_enmState = CMS_BACK_TO_MC;

	//主角位置
	NiPoint3 kMCPos = NiPoint3::ZERO;
	CA_CameraHelp::GamePlayer::GetNiPosition(kMCPos);

	//镜头位置
	NiPoint3 kCamPos = m_spNiCamera->GetWorldLocation();

	m_srcPos = m_dstPos;//kCamPos - m_relOffset;
	m_dstPos = kMCPos;
}
