#include "ClientPCH.h"

#include "LC_ClientGameVehicle.h"
#include "LC_ClientGamePlayer.h"
#include "LC_ClientLogicManager.h"

#include "IM_Manager.h"

#include "LC_ClientGamePlayer.h"
#include "LC_ClientGamePlayerManager.h"
#include "LC_ClientLogicManager.h"
#include "LC_VehicleBase.h"

#include "MG_PACKET.h"

#include "NW_Game2ClientSession.h"
#include "NW_Helper.h"

#include "SE_SceneManager.h"
#include "GD_ClientActorModel.h"

#include "SM_ClientControllerFactory.h"
#include "SM_ClientGameVehicleController.h"
#include "SM_ClientVehicleState_IDLE.h"

using namespace Base;
using namespace Data;
using namespace GameLogic;
using namespace Path;
using namespace Protocol;
using namespace StateMachine;
using namespace Utility;
using namespace Base;
using namespace CSVFile;
//------------------------------------------------------------------------------
LC_ClientGameVehicle::LC_ClientGameVehicle()
: LC_VehicleBase()
, m_bMoveStride(false)
, m_bSynFlag(false)
{
	SM_StateMachine::Init();
	SM_StateMachine::Start(ST_VEHICLE_IDLE,0.0f,0.0f);

	m_pkSMController = SM_ClientControllerFactory::GetSingletonPtr()->RequestController(SCT_CLIENT_GAME_VEHICLE);
}

//------------------------------------------------------------------------------
LC_ClientGameVehicle::~LC_ClientGameVehicle()
{
}
//------------------------------------------------------------------------------
void LC_ClientGameVehicle::Update(float fCurrentTime, float fDeltaTime)
{
	LC_VehicleBase::Update(fCurrentTime, fDeltaTime);

	//更新状态机
	m_pkSMController->Update(this, fCurrentTime, fDeltaTime);
	//同步到服务器端
	//_syncToServer(fCurrentTime);[[目前需要让Player去同步服务器]]
}
//------------------------------------------------------------------------------
bool LC_ClientGameVehicle::NotifyStateEnd(int iEndState,float fCurrentTime,float fDeltaTime)
{
	if(m_pkSMController)
		m_pkSMController->NotifyStateEnd(this,iEndState,fCurrentTime,fDeltaTime);

	return true;
}
//------------------------------------------------------------------------------
bool LC_ClientGameVehicle::RecvMessage(MG_MessageBase& rkMessage)
{
	MG_MessageHandlerInterface::RecvMessage(rkMessage);
	if (m_pkSMController->GetControllerType() == SCT_CLIENT_GAME_VEHICLE)
	{
		return ((SM_ClientGameVehicleController*)m_pkSMController)->RecvMessage(this, rkMessage);
	}
	return true;
}

//------------------------------------------------------------------------------
void LC_ClientGameVehicle::CreateVisibleData()
{
	T_ASSERT(m_lCharType > 0 && m_iID > 0 && "error type");

	const SE_SceneTerrainLocalInfo& rkTerrainInfo = SE_SceneManager::GetSingletonPtr()->GetTerrainLocalInfo(
		Int2Float(m_kCurrentLocation.x), Int2Float(m_kCurrentLocation.y));
	m_kCurrentLocation.z = Float2Int(rkTerrainInfo._fLocalHeight);

	NiPoint3 kPos = UT_EngineMath::ConvertVec3IntToNiPoint3(m_kCurrentLocation);
	GD_ClientActorModel* pkVisibleData = (GD_ClientActorModel*)IM_Manager::GetSingleton().CreateInteractModel(m_lCharType, m_iID, false,kPos);
	if(pkVisibleData)
	{
		//骑乘不能点击
		pkVisibleData->SetPickFlag(Base::BS_IMInterface::PM_NONE);
	}

	SetVisibleLocation(m_kCurrentLocation);

    m_bIsCreateVisibleData = true;
}
//------------------------------------------------------------------------------
void LC_ClientGameVehicle::DestoryVisibleData()
{
	IM_Manager* pkInteractManager = IM_Manager::GetSingletonPtr();
	pkInteractManager->DestroyInteractModel(m_iID);
	m_bIsCreateVisibleData = false;
}
//------------------------------------------------------------------------------
void LC_ClientGameVehicle::PlayAnimation(const StringType& rkAnim,bool bForce /* = false */,float fSequency /* = 1.0f */,bool bAutoAdjustFrequency)
{
	IM_Manager* pkIMManager = IM_Manager::GetSingletonPtr();
	if(!pkIMManager)
		return;

	GD_ClientActorModel* pkVisibleData = pkIMManager->GetActor(m_iID);
	if(pkVisibleData)	
	{
		pkVisibleData->PlayAnimation(rkAnim, bForce, fSequency,bAutoAdjustFrequency);
	}
}
//------------------------------------------------------------------------------
void LC_ClientGameVehicle::PlayIdleAnimation(void)
{
    IM_Manager* pkIMManager = IM_Manager::GetSingletonPtr();
    if(!pkIMManager)
        return;

    GD_ClientActorModel* pkVisibleData = pkIMManager->GetActor(m_iID);
    if(pkVisibleData)
    {
        pkVisibleData->PlayRandomIdleAnimation();
    }
}
//------------------------------------------------------------------------------
void LC_ClientGameVehicle::SetTargetLocation( const Utility::UT_Vec3Int& rkTarget )
{
	LC_ActorBase::SetTargetLocation( rkTarget );

	int iPathPointCount = m_kMovePath.GetPathPointCount();
	bool bNeedFind = false;
	if ( iPathPointCount > 1 )
	{
		bNeedFind = ( UT_EngineMath::LineLengthXYInt( rkTarget , m_kMovePath.GetPathPoint(iPathPointCount - 1)) > 20 );
	}
	else if ( iPathPointCount == 0 || iPathPointCount == 1 )
	{
		bNeedFind = true;
	}

	if( bNeedFind )
	{
		if (PF_PathManager::GetSingletonPtr() && GfxGetWorldConfig().CFG_ENABLE_PATH_FINDING )
		{
			/*
			注意，这里寻路的目标点和实际生成路径的目标点有可能是不一样的
			所以，是否需要改成把Target当成引用传入进行修改？
			*/
			bool bForceAvenue = false;
			if ( g_settings.LOD_PATH_QUERY_STEP > 1000 )
			{
				bForceAvenue = true;
			}
			PF_PathManager::GetSingletonPtr()->FindPath(m_kMovePath,m_kCurrentLocation,rkTarget , bForceAvenue , true );
		}
		else
		{
			m_kMovePath.AllContentClear();
			m_kMovePath.AddPathPoints(m_kCurrentLocation );
			m_kMovePath.AddPathPoints(rkTarget);
			m_kMovePath.GeneratePath();
		}
	}
}

//------------------------------------------------------------------------------
/*void LC_ClientGameVehicle::_syncToServer(float fCurrentTime)
{
#pragma TODO("该处有逻辑错误")
	if (fCurrentTime - m_fLastSyncTime > .2f)
	{
		m_fLastSyncTime = fCurrentTime;

		MG_PACKET_HANDLER hMsgHandler;
		bool bCreateMsg = false;

		MG_Sync_GameVehicleMove syncMsg;
		syncMsg.m_fClientTime = fCurrentTime;
		syncMsg.m_lCurrentLocationX = m_kCurrentLocation.x;
		syncMsg.m_lCurrentLocationY = m_kCurrentLocation.y;
		syncMsg.m_lTargetLocationX  = m_kTargetLocation.x;
		syncMsg.m_lTargetLocationY	= m_kTargetLocation.y;
		syncMsg.m_lMoveSpeed		= m_lMoveSpeed;
		syncMsg.m_lVehicleState		= m_iCurrentState;
		syncMsg.m_lVehicleID		= m_iID;
		bCreateMsg = CreateMsgPacketHandler(hMsgHandler,SERVER_TYPE_GAME, 0, MGPT_SYNC_GAME_VEHICLE_MOVE, &syncMsg);

		if (bCreateMsg)
		{
			LC_ClientGamePlayerManager* pkPlayerManager = CLIENT_GET_GAMEPLAYER_MANAGER();
			LC_ClientGamePlayer* pkPlayer = (LC_ClientGamePlayer*)pkPlayerManager->FindPlayer(GetOwnerID());
			if (pkPlayer)
			{
				pkPlayer->GetSession()->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
			}
		}	

	}
}*/
//------------------------------------------------------------------------------
UT_Vec3Int LC_ClientGameVehicle::GetVisibleSeatLocation(void)
{
    if (!IsCreateVisibleData())
    {
        return GetCurrentLocation();
    }

    GD_ClientActorModel* pkVehicleModel = IM_Manager::GetSingleton().GetActor(m_iID);
	if(!pkVehicleModel)
		return UT_Vec3Int(0,0,0);

    NiPoint3 kVisibleSeatLocation = pkVehicleModel->GetSeatLocation(GD_ActorModel::ST_MAJOR);
    return UT_Vec3Int((int)kVisibleSeatLocation.x, (int)kVisibleSeatLocation.y, (int)kVisibleSeatLocation.z);
}
//------------------------------------------------------------------------------
StateType LC_ClientGameVehicle::FindMoveState()
{
	PF_Path& rkMovePath = GetMovePath();
	float fPathMoveStep = rkMovePath.GetMoveStep();
	int	 iMovePathPointCount = rkMovePath.GetPathPointCount();


	LC_ClientGamePlayerManager* pkPlayerManager = CLIENT_GET_GAMEPLAYER_MANAGER();
	LC_ClientGamePlayer* pkPlayer = (LC_ClientGamePlayer*)pkPlayerManager->FindPlayer(GetOwnerID());
	if(pkPlayer && (pkPlayer->GetSkillFlag(SK_DIZZ) || pkPlayer->GetSkillFlag(SK_SLEEP)))
		return ST_VEHICLE_IDLE;


	if(fPathMoveStep > 0.99f || iMovePathPointCount <= 1)
		return ST_VEHICLE_IDLE;
	else 
		return ST_VEHICLE_RUN;
}
//------------------------------------------------------------------------------
ResultType LC_ClientGameVehicle::CheckExecuteSkill(float fCurrentTime,Skill::SK_SkillExecute* pkSkill,long lItemID)
{
	long lSkillType = pkSkill->GetExecuteID();
	CF_SkillExecuteList* pkSkillList = CF_SkillExecuteList::GetSingletonPtr();
	CF_SkillExecuteList::DataEntry* pkSkillData = pkSkillList->GetEntryPtr(lSkillType);
	if (NULL == pkSkillData)
		return RE_SKILL_SYSTEM_ERROR;

	//判断触发条件
	{
		int iOperationType			 = pkSkill->GetProperty_OperationType();
		int iLockTargetType			 = pkSkill->GetProperty_LockTargetType();
		int iSkillExecuteMinDistance = pkSkill->GetProperty_MinExecuteDistance();		
		int iSkillExecuteMaxDistance = pkSkill->GetProperty_MaxExecuteDistance();
		bool bExecuteOnCorpus		 = pkSkill->GetProperty_ExecuteOnCorpus();
		bool bNeedTestPath			 = pkSkill->GetProperty_NeedTestPath();
		bool bExecuteImmediately	 = pkSkill->GetProperty_ExecuteImmediately();

		UT_Vec3Int kCurrentLocation = GetVisibleLocation();
		UT_Vec3Int kTargetLocation = kCurrentLocation;
		ResultType iResult;

		switch (iOperationType)
		{
		case 0: //目标型
			{
				long lTargetID = 0;
				LC_LogicObject* pkTarget = NULL;

				lTargetID = GetLockedTargetID();
				LC_MapBase* pkMap = GetMap();
				if(!pkMap)
					return RE_SKILL_LOST_TARGET;
				pkTarget = pkMap->FindObject(lTargetID);

				if(!pkTarget)
					iResult = RE_SKILL_LOST_TARGET;
				else
				{
					iResult = RE_SKILL_NONE_ERROR;					
					kTargetLocation  = pkTarget->GetVisibleLocation();
					int iSafeRadius	 = GetProperty_ShapeRadius();
					if(IS_PLAYER_CLASS_ID(lTargetID) || /*IS_MONSTER_ID(lTargetID) ||*/ IS_NPC_ID(lTargetID))
						iSafeRadius	+= ((LC_ActorBase*)pkTarget)->GetProperty_ShapeRadius();
					kTargetLocation = UT_EngineMath::ComputeSafeRadiusTargetLocation(kCurrentLocation,kTargetLocation,iSafeRadius);

					int iDistance = UT_MathBase::LineLengthXYInt(kCurrentLocation,kTargetLocation);
					if(iDistance > iSkillExecuteMaxDistance)
						iResult = RE_SKILL_DISTANCE_FAR;
				}
			}
			break;
		case 1: //朝向型
			{
				iResult = RE_SKILL_NONE_ERROR;
			}
			break;
		case 2: //地点型
			{
				//以地面位置设为目标点
				iResult = RE_SKILL_NONE_ERROR;

				//以地面位置设为目标点
				kTargetLocation = GetLockedLocation();

				int iCurrentDistance = UT_MathBase::LineLengthXYInt(kCurrentLocation,kTargetLocation);
				if(iCurrentDistance > iSkillExecuteMaxDistance && !bExecuteImmediately)
					iResult = RE_SKILL_DISTANCE_FAR;
			}
		default:
			break;
		}

		if(iResult != RE_SKILL_NONE_ERROR)
			return iResult;
	}

	return LC_ActorBase::CheckExecuteSkill(fCurrentTime,pkSkill,lItemID);
}
//-------------------------------------------------------
ResultType LC_ClientGameVehicle::PrepareExecuteSkill(Skill::SK_SkillExecute* pkSkill,long lItemID)
{
	int iOperationType				= pkSkill->GetProperty_OperationType();
	int iLockTargetType				= pkSkill->GetProperty_LockTargetType();
	int iSkillExecuteMinDistance	= pkSkill->GetProperty_MinExecuteDistance();		
	int iSkillExecuteMaxDistance	= pkSkill->GetProperty_MaxExecuteDistance();
	bool bExecuteOnCorpus			= pkSkill->GetProperty_ExecuteOnCorpus();
	bool bNeedTestPath				= pkSkill->GetProperty_NeedTestPath();
	bool bExecuteImmediately		= pkSkill->GetProperty_ExecuteImmediately();

	UT_Vec3Int kCurrentLocation		= GetVisibleLocation();
	UT_Vec3Int kTargetLocation		= kCurrentLocation;
	int iSafeRadius = 0;

	switch (iOperationType)
	{
	case 0: //目标型
		{
			long lTargetID = 0;
			LC_LogicObject* pkTarget = NULL;

			lTargetID = GetLockedTargetID();
			LC_MapBase* pkMap = GetMap();
			if(!pkMap)
				return RE_SKILL_LOST_TARGET;
		
			pkTarget = pkMap->FindObject(lTargetID);

			if(!pkTarget)
				return RE_SKILL_LOST_TARGET;
			else
			{
				kTargetLocation  = pkTarget->GetVisibleLocation();
				if (pkSkill->GetExecuteID() != SKILL_TALK_TO_NPC)
				{
					int iSafeRadius	 = GetProperty_ShapeRadius();
					if(IS_PLAYER_CLASS_ID(lTargetID) || /*IS_MONSTER_ID(lTargetID) ||*/ IS_NPC_ID(lTargetID))
						iSafeRadius	+= ((LC_ActorBase*)pkTarget)->GetProperty_ShapeRadius();
					kTargetLocation = UT_EngineMath::ComputeSafeRadiusTargetLocation(kCurrentLocation,kTargetLocation,iSafeRadius);
				}
				SetTargetLocation(kTargetLocation);
			}
		}
		break;
	case 1: //朝向型
		{
			//以地面位置设为目标点
			UT_Vec2Float	 kForwardDirXY	  = GetForwardDir();
			UT_Vec2Float	 kMaxMoveLocation = kForwardDirXY * Long2Float(iSkillExecuteMaxDistance);

			kTargetLocation = kCurrentLocation;
			kTargetLocation.x += (int)kMaxMoveLocation.x;
			kTargetLocation.y += (int)kMaxMoveLocation.y;

			//可触发技能,设置目标点
			SetTargetLocation(kTargetLocation);
			SetLockedLocation(kTargetLocation);
		}
		break;
	case 2: //地点型
		{
			kTargetLocation = GetLockedLocation();
			SetTargetLocation(kTargetLocation);
		}
		break;
	}

	return RE_SKILL_NONE_ERROR;
}
//------------------------------------------------------------------------
ResultType LC_ClientGameVehicle::ExecuteSkill(float fCurrentTime,Skill::SK_SkillExecute* pkSkill,long lItemID)
{
	ResultType iExecuteResult = LC_ActorBase::ExecuteSkill(fCurrentTime,pkSkill,lItemID);

	switch (iExecuteResult)
	{
	case RE_SKILL_EXECUTE_SUCCESS:
		break;
	default:
		break;
	}

	return iExecuteResult;
}