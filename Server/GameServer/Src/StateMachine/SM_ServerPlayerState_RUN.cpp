

#include "SM_ServerPlayerState_RUN.h"
#include "LC_ServerPlayer.h"

using namespace StateMachine;
//------------------------------------------------------
SM_ServerPlayerState_RUN::SM_ServerPlayerState_RUN()
: SM_PlayerState_RUN()
{
}
//------------------------------------------------------
SM_ServerPlayerState_RUN::~SM_ServerPlayerState_RUN()
{
}
//------------------------------------------------------
bool SM_ServerPlayerState_RUN::CanTransite(SM_StateMachine* pkMachine, SM_StateBase* pkToTransiteState)
{
	return true;
}
//------------------------------------------------------
StateChangeResultType SM_ServerPlayerState_RUN::StateStart(SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime)
{
	SM_PlayerState_RUN::StateStart(pkMachine,fCurrentTime,fDeltaTime);
	return SCRT_SUCCESS;
}
//------------------------------------------------------
StateProcessType SM_ServerPlayerState_RUN::StateProcess(SM_StateMachine* pkMachine,  float fCurrentTime,float fDeltaTime )
{
	SM_PlayerState_RUN::StateProcess(pkMachine,fCurrentTime,fDeltaTime);
//	LC_ServerPlayer* pkPlayer = static_cast<LC_ServerPlayer*>(pkMachine);
	//int lPendingSkillID = pkPlayer->GetPendingSkillTypeID();
	//ResultType eResult = pkPlayer->ExecutePendingSkillImmediately(fCurrentTime);
	//if (eResult == RE_SKILL_EXECUTE_SUCCESS)
	//{
	//	pkPlayer->SetNextState(lPendingSkillID);
	//	return SPT_END;
	//}
	return SPT_SUSPEND;


	/*SM_PlayerState_RUN::StateProcess(pkMachine,fCurrentTime,fDeltaTime);
	return SPT_SUSPEND;*/
}
//------------------------------------------------------
StateChangeResultType SM_ServerPlayerState_RUN::StateEnd(SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime )
{
	return SCRT_SUCCESS;
}
