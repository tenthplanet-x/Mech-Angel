

#include "LC_ServerNPC.h"
#include "SM_ServerNPCState_BORN.h"

using namespace StateMachine;
using namespace CSVFile;
using namespace Skill;
using namespace GameLogic;
//------------------------------------------------------
SM_ServerNPCState_BORN::SM_ServerNPCState_BORN()
: SM_NPCState_BORN()
{
}
//------------------------------------------------------
SM_ServerNPCState_BORN::~SM_ServerNPCState_BORN()
{
}
//------------------------------------------------------
bool SM_ServerNPCState_BORN::CanTransite(SM_StateMachine* pkMachine,SM_StateBase* pkToTransiteState)
{
	return true;
}
//------------------------------------------------------
StateChangeResultType SM_ServerNPCState_BORN::StateStart(SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime)
{
	SM_NPCState_BORN::StateStart(pkMachine,fCurrentTime,fDeltaTime);
	LC_ServerNPC* pkServerNPC = (LC_ServerNPC*)pkMachine;
	pkServerNPC->SetLastIdleTime(fCurrentTime);

	return SCRT_SUCCESS;
}
//------------------------------------------------------
StateProcessType SM_ServerNPCState_BORN::StateProcess(SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime )
{
	SM_NPCState_BORN::StateProcess(pkMachine,fCurrentTime,fDeltaTime);
	LC_ServerNPC* pkServerNPC = (LC_ServerNPC*)pkMachine;
	float fLastIdleTime = pkServerNPC->GetLastIdleTime();
	if(fCurrentTime - fLastIdleTime > pkServerNPC->GetIdleWhenBorn())
	{
		pkServerNPC->SetNextState(ST_NPC_IDLE);
		return SPT_END;
	}
	return SPT_SUSPEND;
}
//------------------------------------------------------
StateChangeResultType SM_ServerNPCState_BORN::StateEnd(SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime )
{
	return SCRT_SUCCESS;
}