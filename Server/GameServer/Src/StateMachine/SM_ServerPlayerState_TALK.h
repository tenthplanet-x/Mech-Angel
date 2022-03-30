#ifndef _SM_SERVER_Player_STATE_TAKL_H
#define _SM_SERVER_Player_STATE_TAKL_H

#include "SM_PlayerState_TALK.h"

class SM_ServerPlayerState_TALK : public StateMachine::SM_PlayerState_TALK
{
public:
	SM_ServerPlayerState_TALK();
	virtual ~SM_ServerPlayerState_TALK();

	virtual bool								CanTransite(StateMachine::SM_StateMachine* pkMachine, SM_StateBase* pkToTransiteState);
	virtual StateMachine::StateChangeResultType	StateStart(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);
	virtual StateMachine::StateProcessType		StateProcess(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);
	virtual StateMachine::StateChangeResultType	StateEnd(StateMachine::SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime);
};

#endif