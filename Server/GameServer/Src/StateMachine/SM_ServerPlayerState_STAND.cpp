

#include "SM_ServerPlayerState_STAND.h"

#include "PlayerStateAction.h"

using namespace StateMachine;
using namespace Skill;
using namespace GameLogic;

//------------------------------------------------------
SM_ServerPlayerState_STAND::SM_ServerPlayerState_STAND()
: SM_PlayerState_STAND()
{
}
//------------------------------------------------------
SM_ServerPlayerState_STAND::~SM_ServerPlayerState_STAND()
{
}
//------------------------------------------------------
bool SM_ServerPlayerState_STAND::CanTransite(SM_StateMachine* pkMachine, SM_StateBase* pkToTransiteState)
{
	return true;
}
//------------------------------------------------------
StateChangeResultType SM_ServerPlayerState_STAND::StateStart(SM_StateMachine* pkMachine,  float fCurrentTime,float fDeltaTime)
{
	SM_PlayerState_STAND::StateStart(pkMachine,fCurrentTime,fDeltaTime);

	SetPlayerLastIdleTime(pkMachine,fCurrentTime);

	return SCRT_SUCCESS;
}
//------------------------------------------------------
StateProcessType SM_ServerPlayerState_STAND::StateProcess(SM_StateMachine* pkMachine,  float fCurrentTime,float fDeltaTime )
{
	SM_PlayerState_STAND::StateProcess(pkMachine,fCurrentTime,fDeltaTime);

	if(SPT_END == ProcessPlayerStateTestFollow(this,pkMachine,fCurrentTime,fDeltaTime,150))
		return SPT_END;

	ProcessPlayerStatePositiveActive(this,pkMachine, fCurrentTime, fDeltaTime);

	//处理技能
	if (SPT_END == ProcessPlayerStateTestFight(this,pkMachine,fCurrentTime,fDeltaTime))
		return SPT_END;
	if (SPT_END == ProcessPlayerStateTestWander(this,pkMachine, fCurrentTime, fDeltaTime))
		return SPT_END;
	if (SPT_END == ProcessPlayerStateTestTalk(this,pkMachine, fCurrentTime, fDeltaTime))
		return SPT_END;
	return SPT_SUSPEND;
}
//------------------------------------------------------
StateChangeResultType SM_ServerPlayerState_STAND::StateEnd(SM_StateMachine* pkMachine,  float fCurrentTime,float fDeltaTime )
{
	return SCRT_SUCCESS;
}
