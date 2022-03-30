#ifndef _SM_SERVER_NPC_STATE_IDLE_H
#define _SM_SERVER_NPC_STATE_IDLE_H

#include "SM_NPCState_IDLE.h"

class SM_ServerNPCState_IDLE : public StateMachine::SM_NPCState_IDLE
{
public:
	SM_ServerNPCState_IDLE();
	virtual ~SM_ServerNPCState_IDLE();

	virtual bool								CanTransite(StateMachine::SM_StateMachine* pkMachine, SM_StateBase* pkToTransiteState);
	virtual StateMachine::StateChangeResultType	StateStart(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);
	virtual StateMachine::StateProcessType		StateProcess(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);
	virtual StateMachine::StateChangeResultType	StateEnd(StateMachine::SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime);

protected:
	void										_setLastIdleTime(StateMachine::SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime);
	void										_positiveActive(StateMachine::SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime);
	StateMachine::StateProcessType				_transiteWanderState(StateMachine::SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime);
	StateMachine::StateProcessType				_transiteTalkState(StateMachine::SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime);
	StateMachine::StateProcessType				_transiteFollowState(StateMachine::SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime);
	StateMachine::StateProcessType				_processFight(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);
};

#endif