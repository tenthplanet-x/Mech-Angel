

#include "LC_ServerPlayer.h"
#include "SM_ServerPlayerState_BORN.h"
#include "PlayerStateAction.h"


using namespace StateMachine;
using namespace CSVFile;
using namespace Skill;
using namespace GameLogic;
//------------------------------------------------------
SM_ServerPlayerState_BORN::SM_ServerPlayerState_BORN()
: SM_PlayerState_BORN()
{
}
//------------------------------------------------------
SM_ServerPlayerState_BORN::~SM_ServerPlayerState_BORN()
{
}
//------------------------------------------------------
bool SM_ServerPlayerState_BORN::CanTransite(SM_StateMachine* pkMachine,SM_StateBase* pkToTransiteState)
{
	return true;
}
//------------------------------------------------------
StateChangeResultType SM_ServerPlayerState_BORN::StateStart(SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime)
{
	SM_PlayerState_BORN::StateStart(pkMachine,fCurrentTime,fDeltaTime);
	SetPlayerLastIdleTime(pkMachine,fCurrentTime);

	return SCRT_SUCCESS;
}
//------------------------------------------------------
StateProcessType SM_ServerPlayerState_BORN::StateProcess(SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime )
{
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkMachine;
	SM_PlayerState_BORN::StateProcess(pkMachine,fCurrentTime,fDeltaTime);
	float fProperty_IdleTime = 1.5;
	float fLastIdleTime = pkPlayer->GetLastIdleTime();
	if(fCurrentTime - fLastIdleTime > fProperty_IdleTime)
	{
		pkPlayer->SummorFriendTalk(0,fCurrentTime);
		pkPlayer->SetNextState(ST_PLAYER_STAND);
		return SPT_END;
	}
	return SPT_SUSPEND;
}
//------------------------------------------------------
StateChangeResultType SM_ServerPlayerState_BORN::StateEnd(SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime )
{
	return SCRT_SUCCESS;
}