

#include "LC_ServerPlayer.h"
#include "SM_ServerPlayerState_SLEEP.h"

using namespace StateMachine;
using namespace GameLogic;
//------------------------------------------------------
SM_ServerPlayerState_SLEEP::SM_ServerPlayerState_SLEEP()
: SM_PlayerState_SLEEP()
{
}
//------------------------------------------------------
SM_ServerPlayerState_SLEEP::~SM_ServerPlayerState_SLEEP()
{
}
//------------------------------------------------------
bool SM_ServerPlayerState_SLEEP::CanTransite(SM_StateMachine* pkMachine, SM_StateBase* pkToTransiteState)
{
	return true;
}
//------------------------------------------------------
StateChangeResultType SM_ServerPlayerState_SLEEP::StateStart(SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime)
{
	SM_PlayerState_SLEEP::StateStart(pkMachine,fCurrentTime,fDeltaTime);
	return SCRT_SUCCESS;
}
//------------------------------------------------------
StateProcessType SM_ServerPlayerState_SLEEP::StateProcess(SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime )
{
	return SPT_SUSPEND;
}
//------------------------------------------------------
StateChangeResultType SM_ServerPlayerState_SLEEP::StateEnd(SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime )
{
	return SCRT_SUCCESS;
}
