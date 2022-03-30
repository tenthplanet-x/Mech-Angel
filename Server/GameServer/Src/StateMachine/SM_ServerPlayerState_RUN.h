#ifndef _SM_SERVER_PLAYER_STATE_RUN_H
#define _SM_SERVER_PLAYER_STATE_RUN_H

#include "SM_PlayerState_RUN.h"

class SM_ServerPlayerState_RUN : public StateMachine::SM_PlayerState_RUN
{
public:
	SM_ServerPlayerState_RUN();
	virtual ~SM_ServerPlayerState_RUN();

	virtual bool								CanTransite(StateMachine::SM_StateMachine* pkMachine, SM_StateBase* pkToTransiteState);
	virtual StateMachine::StateChangeResultType	StateStart(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);
	virtual StateMachine::StateProcessType		StateProcess(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);
	virtual StateMachine::StateChangeResultType	StateEnd(StateMachine::SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime);
};

#endif