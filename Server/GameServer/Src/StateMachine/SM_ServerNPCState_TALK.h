#ifndef _SM_SERVER_NPC_STATE_TAKL_H
#define _SM_SERVER_NPC_STATE_TAKL_H

#include "SM_NPCState_TALK.h"

class SM_ServerNPCState_TALK : public StateMachine::SM_NPCState_TALK
{
public:
	SM_ServerNPCState_TALK();
	virtual ~SM_ServerNPCState_TALK();

	virtual bool								CanTransite(StateMachine::SM_StateMachine* pkMachine, SM_StateBase* pkToTransiteState);
	virtual StateMachine::StateChangeResultType	StateStart(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);
	virtual StateMachine::StateProcessType		StateProcess(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);
	virtual StateMachine::StateChangeResultType	StateEnd(StateMachine::SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime);
};

#endif