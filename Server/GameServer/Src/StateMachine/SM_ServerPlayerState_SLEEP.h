#ifndef _SM_SERVER_PLAYER_STATE_SLEEP_H
#define _SM_SERVER_PLAYER_STATE_SLEEP_H

#include "SM_PlayerState_SLEEP.h"

class SM_ServerPlayerState_SLEEP : public StateMachine::SM_PlayerState_SLEEP
{
public:
	SM_ServerPlayerState_SLEEP();
	virtual ~SM_ServerPlayerState_SLEEP();

	virtual bool								CanTransite(StateMachine::SM_StateMachine* pkMachine, SM_StateBase* pkToTransiteState);
	virtual StateMachine::StateChangeResultType	StateStart(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);
	virtual StateMachine::StateProcessType		StateProcess(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);
	virtual StateMachine::StateChangeResultType	StateEnd(StateMachine::SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime);
};

#endif