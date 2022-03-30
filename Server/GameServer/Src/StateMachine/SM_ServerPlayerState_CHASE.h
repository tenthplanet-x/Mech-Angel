#ifndef _SM_SERVER_Player_STATE_CHASE_H
#define _SM_SERVER_Player_STATE_CHASE_H

#include "SM_PlayerState_CHASE.h"

class SM_ServerPlayerState_CHASE : public StateMachine::SM_PlayerState_CHASE
{
public:
	SM_ServerPlayerState_CHASE();
	virtual ~SM_ServerPlayerState_CHASE();

	virtual bool								CanTransite(StateMachine::SM_StateMachine* pkMachine, SM_StateBase* pkToTransiteState);
	virtual StateMachine::StateChangeResultType	StateStart(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);
	virtual StateMachine::StateProcessType		StateProcess(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);
	virtual StateMachine::StateChangeResultType	StateEnd(StateMachine::SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime);
protected:
	StateMachine::StateProcessType				_processFight(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime, int32_t & nAppendixResult);
	StateMachine::StateProcessType _processFightAsBuddy(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime, int32_t & nAppendixResult);
	void										_keepChaseDistance(StateMachine::SM_StateMachine* pkMachine, int32_t nDistance);
	void										_checkSkill(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);
	void										_pendingSkill(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);
};



#endif