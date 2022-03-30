

#include "LC_ServerPlayer.h"
#include "SM_ServerPlayerState_DIZZ.h"

using namespace StateMachine;
using namespace GameLogic;
//------------------------------------------------------
SM_ServerPlayerState_DIZZ::SM_ServerPlayerState_DIZZ()
: SM_PlayerState_DIZZ()
{
}
//------------------------------------------------------
SM_ServerPlayerState_DIZZ::~SM_ServerPlayerState_DIZZ()
{
}
//------------------------------------------------------
bool SM_ServerPlayerState_DIZZ::CanTransite(SM_StateMachine* pkMachine, SM_StateBase* pkToTransiteState)
{
	return true;
}
//------------------------------------------------------
StateChangeResultType SM_ServerPlayerState_DIZZ::StateStart(SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime)
{
	SM_PlayerState_DIZZ::StateStart(pkMachine,fCurrentTime,fDeltaTime);
	return SCRT_SUCCESS;
}
//------------------------------------------------------
StateProcessType SM_ServerPlayerState_DIZZ::StateProcess(SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime )
{
	return SPT_SUSPEND;
}
//------------------------------------------------------
StateChangeResultType SM_ServerPlayerState_DIZZ::StateEnd(SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime )
{
	return SCRT_SUCCESS;
}
