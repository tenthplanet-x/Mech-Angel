

#include "SM_ServerPlayerState_TALK.h"

#include "LC_ServerPlayer.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"

#include "LC_FactionManager.h"

using namespace StateMachine;
using namespace CSVFile;
using namespace Utility;
using namespace GameLogic;
//------------------------------------------------------
SM_ServerPlayerState_TALK::SM_ServerPlayerState_TALK()
: SM_PlayerState_TALK()
{
}
//------------------------------------------------------
SM_ServerPlayerState_TALK::~SM_ServerPlayerState_TALK()
{
}
//------------------------------------------------------
bool SM_ServerPlayerState_TALK::CanTransite(SM_StateMachine* pkMachine, SM_StateBase* pkToTransiteState)
{
	return true;
}
//------------------------------------------------------
StateChangeResultType SM_ServerPlayerState_TALK::StateStart(SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime)
{
	SM_PlayerState_TALK::StateStart(pkMachine,fCurrentTime,fDeltaTime);
	return SCRT_SUCCESS;
}
//------------------------------------------------------
StateProcessType SM_ServerPlayerState_TALK::StateProcess(SM_StateMachine* pkMachine,  float fCurrentTime,float fDeltaTime )
{
	LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)pkMachine;
	object_id_type lLockTargetID = pkServerPlayer->GetLockedTargetID();

	if(!IS_PLAYER_CLASS_ID(lLockTargetID))
	{
		pkServerPlayer->SetLockedTargetID(0);
		pkServerPlayer->SetNextState(ST_PLAYER_STAND);
		return SPT_END;
	}

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(pkPlayerManager->FindPlayer(lLockTargetID));
	if(!pkPlayer)
	{
		pkServerPlayer->SetLockedTargetID(0);
		pkServerPlayer->SetNextState(ST_PLAYER_STAND);
		return SPT_END;
	}

	int iDistance = UT_MathBase::LineLengthXYInt(pkPlayer->GetCurrentLocation(),pkServerPlayer->GetCurrentLocation());
	if(iDistance > 500)
	{
		pkServerPlayer->SetLockedTargetID(0);
		pkServerPlayer->SetNextState(ST_PLAYER_STAND);
		return SPT_END;
	}

	//如果是敌对阵营，结束talk状态
	LC_FactionManager* pkFactionManager = LC_FactionManager::GetSingletonPtr();
	if (pkFactionManager->GetEnemyRelation(pkServerPlayer, pkPlayer))
	{
		return SPT_END;
	}

	return SPT_SUSPEND;
}
//------------------------------------------------------
StateChangeResultType SM_ServerPlayerState_TALK::StateEnd(SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime )
{
	return SCRT_SUCCESS;
}