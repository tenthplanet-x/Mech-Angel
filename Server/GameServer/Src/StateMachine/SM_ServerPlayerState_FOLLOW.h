#ifndef _SM_SERVER_Player_STATE_FOLLOW_H
#define _SM_SERVER_Player_STATE_FOLLOW_H

#include "SM_PlayerState_FOLLOW.h"

class SM_ServerPlayerState_FOLLOW : public StateMachine::SM_PlayerState_FOLLOW
{
public:
	SM_ServerPlayerState_FOLLOW();
	virtual ~SM_ServerPlayerState_FOLLOW();

	virtual bool								CanTransite(StateMachine::SM_StateMachine* pkMachine, SM_StateBase* pkToTransiteState);
	virtual StateMachine::StateChangeResultType	StateStart(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);
	virtual StateMachine::StateProcessType		StateProcess(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);
	virtual StateMachine::StateChangeResultType	StateEnd(StateMachine::SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime);

protected:
	void										_setLastIdleTime(StateMachine::SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime);
	StateMachine::StateProcessType				_processFollow(StateMachine::SM_StateMachine* pkMachine);
};

#endif