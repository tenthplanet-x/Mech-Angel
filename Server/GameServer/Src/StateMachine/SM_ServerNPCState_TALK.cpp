

#include "SM_ServerNPCState_TALK.h"

#include "LC_ServerNPC.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"

#include "LC_FactionManager.h"

using namespace StateMachine;
using namespace CSVFile;
using namespace Utility;
using namespace GameLogic;
//------------------------------------------------------
SM_ServerNPCState_TALK::SM_ServerNPCState_TALK()
: SM_NPCState_TALK()
{
}
//------------------------------------------------------
SM_ServerNPCState_TALK::~SM_ServerNPCState_TALK()
{
}
//------------------------------------------------------
bool SM_ServerNPCState_TALK::CanTransite(SM_StateMachine* pkMachine, SM_StateBase* pkToTransiteState)
{
	return true;
}
//------------------------------------------------------
StateChangeResultType SM_ServerNPCState_TALK::StateStart(SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime)
{
	SM_NPCState_TALK::StateStart(pkMachine,fCurrentTime,fDeltaTime);
	return SCRT_SUCCESS;
}
//------------------------------------------------------
StateProcessType SM_ServerNPCState_TALK::StateProcess(SM_StateMachine* pkMachine,  float fCurrentTime,float fDeltaTime )
{
	LC_ServerNPC* pkServerNPC = (LC_ServerNPC*)pkMachine;
	object_id_type lLockTargetID = pkServerNPC->GetLockedTargetID();

	if(!IS_PLAYER_ID(lLockTargetID))
	{
		pkServerNPC->SetLockedTargetID(0);
		pkServerNPC->SetNextState(ST_NPC_IDLE);
		return SPT_END;
	}

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(pkPlayerManager->FindPlayer(lLockTargetID));
	if(!pkPlayer)
	{
		pkServerNPC->SetLockedTargetID(0);
		pkServerNPC->SetNextState(ST_NPC_IDLE);
		return SPT_END;
	}

	int iDistance = UT_MathBase::LineLengthXYInt(pkPlayer->GetCurrentLocation(),pkServerNPC->GetCurrentLocation());
	if(iDistance > 500)
	{
		pkServerNPC->SetLockedTargetID(0);
		pkServerNPC->SetNextState(ST_NPC_IDLE);
		return SPT_END;
	}

	//如果是敌对阵营，结束talk状态
	LC_FactionManager* pkFactionManager = LC_FactionManager::GetSingletonPtr();
	if (pkFactionManager->GetEnemyRelation(pkServerNPC, pkPlayer))
	{
		return SPT_END;
	}

	return SPT_SUSPEND;
}
//------------------------------------------------------
StateChangeResultType SM_ServerNPCState_TALK::StateEnd(SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime )
{
	return SCRT_SUCCESS;
}