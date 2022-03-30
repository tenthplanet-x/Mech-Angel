#ifndef _SM_SERVER_NPC_STATE_RETREAT_H
#define _SM_SERVER_NPC_STATE_RETREAT_H

#include "SM_NPCState_RETREAT.h"

class SM_ServerNPCState_RETREAT : public StateMachine::SM_NPCState_RETREAT
{
public:
	SM_ServerNPCState_RETREAT();
	virtual ~SM_ServerNPCState_RETREAT();

	virtual bool								CanTransite(StateMachine::SM_StateMachine* pkMachine, SM_StateBase* pkToTransiteState);
	virtual StateMachine::StateChangeResultType	StateStart(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);
	virtual StateMachine::StateProcessType		StateProcess(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);
	virtual StateMachine::StateChangeResultType	StateEnd(StateMachine::SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime);
	virtual StateMachine::StateHandleEventType	GetDefaultEventHandleType(Skill::SK_SkillEvent* pkSkillEvent);
protected:
	void				_addImmortalState(StateMachine::SM_StateMachine* pkMachinee);
	void				_removeImmortalState(StateMachine::SM_StateMachine* pkMachine);
	void				_recoverHP(StateMachine::SM_StateMachine* pkMachine);
	void				_recoverMP(StateMachine::SM_StateMachine* pkMachine);
	//只留下出生自带的状态，清除其余状态
	void				_removeStateExceptBornState(StateMachine::SM_StateMachine* pkMachine);
};

#endif