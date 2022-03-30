#ifndef _SM_SERVER_NPC_STATE_FOLLOW_H
#define _SM_SERVER_NPC_STATE_FOLLOW_H

#include "SM_NPCState_FOLLOW.h"

class SM_ServerNPCState_FOLLOW : public StateMachine::SM_NPCState_FOLLOW
{
public:
	SM_ServerNPCState_FOLLOW();
	virtual ~SM_ServerNPCState_FOLLOW();

	virtual bool								CanTransite(StateMachine::SM_StateMachine* pkMachine, SM_StateBase* pkToTransiteState);
	virtual StateMachine::StateChangeResultType	StateStart(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);
	virtual StateMachine::StateProcessType		StateProcess(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);
	virtual StateMachine::StateChangeResultType	StateEnd(StateMachine::SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime);

protected:
	void										_setLastIdleTime(StateMachine::SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime);
	void										_positiveActive(StateMachine::SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime);
	StateMachine::StateProcessType				_processFight(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);
	StateMachine::StateProcessType				_processFollow(StateMachine::SM_StateMachine* pkMachine);
};

#endif