#include "ClientPCH.h"
#include "GD_ClientBulletManager.h"
#include "IM_Manager.h"
#include "MG_MessageBase.h"
#include "LC_ActorBase.h"

#include "LC_ClientLogicManager.h"
#include "LC_ClientNetPlayerManager.h"
#include "LC_ClientNetPlayer.h"
#include "LC_ClientGamePlayer.h"
#include "LC_ClientNPC.h"
#include "LC_ClientNPCManager.h"

using namespace Data;

//==============================================================================
// class GD_ClientBulletManager
//==============================================================================

GD_ClientBulletManager::GD_ClientBulletManager(void)
{
}

GD_ClientBulletManager::~GD_ClientBulletManager(void)
{
}

bool GD_ClientBulletManager::RecvMessage(MG_MessageBase& rkMessage)
{
	MG_MessageHandlerInterface::RecvMessage(rkMessage);

	BEGIN_MESSAGE_HANDLE
	MESSAGE_HANDLE(MGT_TIMER_TICK, _eventTimerTick)
	END_MESSAGE_HANDLE

	return true;
}

GD_Bullet* GD_ClientBulletManager::_allocBullet(const GD_BulletCreateArg & kArg)
{
    return T_NEW_D GD_ClientBullet(kArg);
}

void GD_ClientBulletManager::_eventTimerTick(MG_MessageBase& rkMessage)
{
    T_PROFILE("Update::GD_ClientBulletManager");

    Update(rkMessage.m_fParam1);
}


//==============================================================================
// class GD_ClientBullet
//==============================================================================

GD_ClientBullet::GD_ClientBullet(const GD_BulletCreateArg & kArg)
: GD_Bullet(kArg)
{
}

GD_ClientBullet::~GD_ClientBullet(void)
{
}

GD_ModelBase* GD_ClientBullet::_doGetModelById(long lId)
{
    T_ASSERT(lId > 0);

    return (GD_ModelBase*)IM_Manager::GetSingleton().GetActor(lId);
}

bool GD_ClientBullet::_refetchTargetCurrentLocation(Target& kTarget)
{
	switch (kTarget.eType)
	{
	case TPT_DYNAMIC_POSITION:
		{
			if (kTarget.eDummyNodeType == GD_ModelBase::FDT_EYE)
			{
				LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
				LC_ClientNetPlayer* pkNetPlayer = (LC_ClientNetPlayer* )pkNetPlayerManager->FindPlayer(kTarget.lId);

				LC_ClientGamePlayer* pkGamePlayer = (CLIENT_GET_LOGIC_MANAGER())->GetOperationPlayer();
				if(pkGamePlayer != NULL &&  pkGamePlayer->GetID() == kTarget.lId)
				{
					object_id_type eyeID = pkGamePlayer->GetEyeCharTypeID();
					//每帧都去判断Eye是否存在
					LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();

					LC_ClientNPC *pkEye = (LC_ClientNPC *)pkNPCManager->FindNPC(eyeID);
					if (pkEye != NULL)	
					{
						GD_ClientActorModel* pkVisibleData = IM_Manager::GetSingleton().GetActor(pkEye->GetID());
						if(pkVisibleData)
						{
							kTarget.kLocation = pkVisibleData->GetFxDummyNodePosition(GD_ModelBase::FDT_HEAD);
						}
						else
						{
							kTarget.kLocation = UT_EngineMath::ConvertVec3IntToNiPoint3(pkEye->GetCurrentLocation());
						}
					}
					else
					{
						return false;
					}
				}
				else if (pkNetPlayer != NULL)
				{
					object_id_type eyeID = pkNetPlayer->GetEyeCharTypeID();
					//每帧都去判断Eye是否存在
					LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
					LC_ClientNPC *pkEye = (LC_ClientNPC *)pkNPCManager->FindNPC(eyeID);
					if (pkEye != NULL)	
					{
						GD_ClientActorModel* pkVisibleData = IM_Manager::GetSingleton().GetActor(pkEye->GetID());
						if(pkVisibleData)
						{
							kTarget.kLocation = pkVisibleData->GetFxDummyNodePosition(GD_ModelBase::FDT_HEAD);
						}
						else
						{
							kTarget.kLocation = UT_EngineMath::ConvertVec3IntToNiPoint3(pkEye->GetCurrentLocation());
						}
					}
					else
					{
						return false;
					}
				}
				else
				{
					return false;
				}
			}
			else
			{
				GD_ModelBase* pkModel = _doGetModelById(kTarget.lId);
				if (pkModel == NULL)
				{
					return false;
				}

				kTarget.kLocation = pkModel->GetFxDummyNodePosition(kTarget.eDummyNodeType);
			}
		}
		break;
	case TPT_FIXED_POSITION:
		break;
	default:
		T_ASSERT(false);
		return false;
	}

	return true;
}


//////////////////////////////////////////////////////////////////////////

GD_ClientSelfInitiatedBulletID::BulletID	GD_ClientSelfInitiatedBulletID::ms_BulletStartID = 1;
GD_ClientSelfInitiatedBulletID::BulletID	GD_ClientSelfInitiatedBulletID::AllocIDs(unsigned int n)
{
	// 如此处理，返回值不可能为0；0留给服务器用；
	// 因此SEP_BULLET_LOCAL_ID参数为0的表示纯服务器子弹
	if (ms_BulletStartID == 0)	++ms_BulletStartID;

	BulletID nextStartID = ms_BulletStartID + n;
	if (nextStartID < ms_BulletStartID)		// 跨越边界
		ms_BulletStartID = 1;	// 从1开始，而不是从0

	// 分配id段
	BulletID startId = ms_BulletStartID;
	ms_BulletStartID = nextStartID;
	return startId;
}
