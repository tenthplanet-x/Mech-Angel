#ifndef _SM_SERVER_NPC_STATE_CHASE_H
#define _SM_SERVER_NPC_STATE_CHASE_H

#include "SM_NPCState_CHASE.h"

class SM_ServerNPCState_CHASE : public StateMachine::SM_NPCState_CHASE
{
public:
	SM_ServerNPCState_CHASE();
	virtual ~SM_ServerNPCState_CHASE();

	virtual bool								CanTransite(StateMachine::SM_StateMachine* pkMachine, SM_StateBase* pkToTransiteState);
	virtual StateMachine::StateChangeResultType	StateStart(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);
	virtual StateMachine::StateProcessType		StateProcess(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);
	virtual StateMachine::StateChangeResultType	StateEnd(StateMachine::SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime);
protected:
	StateMachine::StateProcessType				_processFight(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);
	StateMachine::StateProcessType				_checkChaseDistanceToRetreat(StateMachine::SM_StateMachine* pkMachine);
	StateMachine::StateProcessType				_checkChaseTargetToRetreat(StateMachine::SM_StateMachine* pkMachine);
	StateMachine::StateProcessType				_checkChaseDistanceToFollow(StateMachine::SM_StateMachine* pkMachine);
	void										_keepChaseDistance(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime, float fDeltaTime);
	void										_checkSkill(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);
	void										_pendingSkill(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);
	void										_computeRandomMoveLocation(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);
};



#endif