#ifndef _SM_SERVER_NPC_STATE_SLEEP_H
#define _SM_SERVER_NPC_STATE_SLEEP_H

#include "SM_NPCState_SLEEP.h"

class SM_ServerNPCState_SLEEP : public StateMachine::SM_NPCState_SLEEP
{
public:
	SM_ServerNPCState_SLEEP();
	virtual ~SM_ServerNPCState_SLEEP();

	virtual bool								CanTransite(StateMachine::SM_StateMachine* pkMachine, SM_StateBase* pkToTransiteState);
	virtual StateMachine::StateChangeResultType	StateStart(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);
	virtual StateMachine::StateProcessType		StateProcess(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);
	virtual StateMachine::StateChangeResultType	StateEnd(StateMachine::SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime);
};

#endif