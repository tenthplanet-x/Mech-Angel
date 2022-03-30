#ifndef _SM_SERVER_NPC_STATE_DIZZ_H
#define _SM_SERVER_NPC_STATE_DIZZ_H

#include "SM_NPCState_DIZZ.h"

class SM_ServerNPCState_DIZZ : public StateMachine::SM_NPCState_DIZZ
{
public:
	SM_ServerNPCState_DIZZ();
	virtual ~SM_ServerNPCState_DIZZ();

	virtual bool								CanTransite(StateMachine::SM_StateMachine* pkMachine, SM_StateBase* pkToTransiteState);
	virtual StateMachine::StateChangeResultType	StateStart(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);
	virtual StateMachine::StateProcessType		StateProcess(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);
	virtual StateMachine::StateChangeResultType	StateEnd(StateMachine::SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime);
};

#endif