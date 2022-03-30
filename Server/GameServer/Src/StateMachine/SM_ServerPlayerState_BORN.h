#ifndef _SM_SERVER_PLAYER_STATE_BORN_H
#define _SM_SERVER_PLAYER_STATE_BORN_H

#include "SM_PlayerState_BORN.h"

class SM_ServerPlayerState_BORN : public StateMachine::SM_PlayerState_BORN
{
public:
	SM_ServerPlayerState_BORN();
	virtual ~SM_ServerPlayerState_BORN();

	virtual bool								CanTransite(StateMachine::SM_StateMachine* pkMachine, SM_StateBase* pkToTransiteState);
	virtual StateMachine::StateChangeResultType	StateStart(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);
	virtual StateMachine::StateProcessType		StateProcess(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);
	virtual StateMachine::StateChangeResultType	StateEnd(StateMachine::SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime);
};

#endif