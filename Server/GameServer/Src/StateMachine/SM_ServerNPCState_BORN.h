#ifndef _SM_SERVER_NPC_STATE_BORN_H
#define _SM_SERVER_NPC_STATE_BORN_H

#include "SM_NPCState_BORN.h"

class SM_ServerNPCState_BORN : public StateMachine::SM_NPCState_BORN
{
public:
	SM_ServerNPCState_BORN();
	virtual ~SM_ServerNPCState_BORN();

	virtual bool								CanTransite(StateMachine::SM_StateMachine* pkMachine, SM_StateBase* pkToTransiteState);
	virtual StateMachine::StateChangeResultType	StateStart(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);
	virtual StateMachine::StateProcessType		StateProcess(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);
	virtual StateMachine::StateChangeResultType	StateEnd(StateMachine::SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime);
};

#endif