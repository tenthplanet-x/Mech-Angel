#ifndef _SM_SERVER_PLAYER_STATE_DIZZ_H
#define _SM_SERVER_PLAYER_STATE_DIZZ_H

#include "SM_PlayerState_DIZZ.h"

class SM_ServerPlayerState_DIZZ : public StateMachine::SM_PlayerState_DIZZ
{
public:
	SM_ServerPlayerState_DIZZ();
	virtual ~SM_ServerPlayerState_DIZZ();

	virtual bool								CanTransite(StateMachine::SM_StateMachine* pkMachine, SM_StateBase* pkToTransiteState);
	virtual StateMachine::StateChangeResultType	StateStart(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);
	virtual StateMachine::StateProcessType		StateProcess(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);
	virtual StateMachine::StateChangeResultType	StateEnd(StateMachine::SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime);
};

#endif