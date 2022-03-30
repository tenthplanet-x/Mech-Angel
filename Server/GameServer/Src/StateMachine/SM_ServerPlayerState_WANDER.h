#ifndef _SM_SERVER_Player_STATE_WANDER_H
#define _SM_SERVER_Player_STATE_WANDER_H
#include "CF_MovePathList.h"
#include "SM_PlayerState_WANDER.h"

class SM_ServerPlayerState_WANDER : public StateMachine::SM_PlayerState_WANDER
{
public:
	SM_ServerPlayerState_WANDER();
	virtual ~SM_ServerPlayerState_WANDER();

	virtual bool								CanTransite(StateMachine::SM_StateMachine* pkMachine, SM_StateBase* pkToTransiteState);
	virtual StateMachine::StateChangeResultType	StateStart(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);
	virtual StateMachine::StateProcessType		StateProcess(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);
	virtual StateMachine::StateChangeResultType	StateEnd(StateMachine::SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime);

protected:
	void										_initWanderPath(StateMachine::SM_StateMachine* pkMachine, CSVFile::CF_MovePathList::DataEntry* pkCSVData);
	void										_computeWanderLocation(StateMachine::SM_StateMachine* pkMachine);
	StateMachine::StateProcessType				_processWander(StateMachine::SM_StateMachine* pkMachine);

};

#endif