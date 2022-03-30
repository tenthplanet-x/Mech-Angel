

#include "LC_ServerNPC.h"
#include "SM_ServerNPCState_SLEEP.h"

#include "LC_ServerPlayer.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerPlayerManager.h"

using namespace StateMachine;
using namespace GameLogic;
using namespace Skill;
using namespace Utility;
using namespace CSVFile;
//------------------------------------------------------
SM_ServerNPCState_SLEEP::SM_ServerNPCState_SLEEP()
: SM_NPCState_SLEEP()
{
}
//------------------------------------------------------
SM_ServerNPCState_SLEEP::~SM_ServerNPCState_SLEEP()
{
}
//------------------------------------------------------
bool SM_ServerNPCState_SLEEP::CanTransite(SM_StateMachine* pkMachine, SM_StateBase* pkToTransiteState)
{
	return true;
}
//------------------------------------------------------
StateChangeResultType SM_ServerNPCState_SLEEP::StateStart(SM_StateMachine* pkMachine,  float fCurrentTime,float fDeltaTime)
{
	SM_NPCState_SLEEP::StateStart(pkMachine,fCurrentTime,fDeltaTime);
	return SCRT_SUCCESS;
}
//------------------------------------------------------
StateProcessType SM_ServerNPCState_SLEEP::StateProcess(SM_StateMachine* pkMachine,  float fCurrentTime,float fDeltaTime )
{
	LC_ServerNPC* pkNPC = static_cast<LC_ServerNPC*>(pkMachine);

	bool bInSleepState = pkNPC->GetSkillFlag(SK_SLEEP);
	//如果已经解除睡眠状态or当前技能睡眠免疫
	if ( !bInSleepState )
	{
		return SPT_END;

	}
	else	//继续保持睡眠状态
	{
		return SPT_HOLD;
	}	
}
//------------------------------------------------------
StateChangeResultType SM_ServerNPCState_SLEEP::StateEnd(SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime )
{
	return SCRT_SUCCESS;
}