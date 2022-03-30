#ifndef _SM_SERVER_PLAYER_STATE_STAND_H
#define _SM_SERVER_PLAYER_STATE_STAND_H

#include "SM_PlayerState_STAND.h"

class SM_ServerPlayerState_STAND : public StateMachine::SM_PlayerState_STAND
{
public:
	SM_ServerPlayerState_STAND();
	virtual ~SM_ServerPlayerState_STAND();

	virtual bool								CanTransite(StateMachine::SM_StateMachine* pkMachine, SM_StateBase* pkToTransiteState);
	virtual StateMachine::StateChangeResultType	StateStart(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);
	virtual StateMachine::StateProcessType		StateProcess(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);
	virtual StateMachine::StateChangeResultType	StateEnd(StateMachine::SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime);
};

#endif