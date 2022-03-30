#ifndef _SM_SERVER_NPC_STATE_WANDER_H
#define _SM_SERVER_NPC_STATE_WANDER_H

#include "SM_NPCState_WANDER.h"
#include "CF_MovePathList.h"

class SM_ServerNPCState_WANDER : public StateMachine::SM_NPCState_WANDER
{
public:
	SM_ServerNPCState_WANDER();
	virtual ~SM_ServerNPCState_WANDER();

	virtual bool								CanTransite(StateMachine::SM_StateMachine* pkMachine, SM_StateBase* pkToTransiteState);
	virtual StateMachine::StateChangeResultType	StateStart(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);
	virtual StateMachine::StateProcessType		StateProcess(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);
	virtual StateMachine::StateChangeResultType	StateEnd(StateMachine::SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime);

protected:
	void										_initWanderPath(StateMachine::SM_StateMachine* pkMachine, CSVFile::CF_MovePathList::DataEntry* pkCSVData);
	void										_computeWanderLocation(StateMachine::SM_StateMachine* pkMachine);
	void										_positiveActive(StateMachine::SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime);
	StateMachine::StateProcessType				_transiteTalkState(StateMachine::SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime);
	StateMachine::StateProcessType				_processWander(StateMachine::SM_StateMachine* pkMachine , float fCurrentTime );
	StateMachine::StateProcessType				_processFight(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);

};

#endif