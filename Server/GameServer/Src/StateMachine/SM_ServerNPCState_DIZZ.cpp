

#include "LC_ServerNPC.h"
#include "SM_ServerNPCState_DIZZ.h"

#include "LC_ServerPlayer.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerPlayerManager.h"

using namespace StateMachine;
using namespace GameLogic;
using namespace Skill;
using namespace Utility;
using namespace CSVFile;
//------------------------------------------------------
SM_ServerNPCState_DIZZ::SM_ServerNPCState_DIZZ()
: SM_NPCState_DIZZ()
{
}
//------------------------------------------------------
SM_ServerNPCState_DIZZ::~SM_ServerNPCState_DIZZ()
{
}
//------------------------------------------------------
bool SM_ServerNPCState_DIZZ::CanTransite(SM_StateMachine* pkMachine, SM_StateBase* pkToTransiteState)
{
	return true;
}
//------------------------------------------------------
StateChangeResultType SM_ServerNPCState_DIZZ::StateStart(SM_StateMachine* pkMachine,  float fCurrentTime,float fDeltaTime)
{
	SM_NPCState_DIZZ::StateStart(pkMachine,fCurrentTime,fDeltaTime);
	return SCRT_SUCCESS;
}
//------------------------------------------------------
StateProcessType SM_ServerNPCState_DIZZ::StateProcess(SM_StateMachine* pkMachine,   float fCurrentTime,float fDeltaTime )
{
	LC_ServerNPC* pkNPC = static_cast<LC_ServerNPC*>(pkMachine);
	
	bool bInDizzState = pkNPC->GetSkillFlag(SK_DIZZ);
	//如果已经解除眩晕状态or当前技能眩晕免疫
	if ( !bInDizzState )
	{
		return SPT_END;
	}
	else	//继续保持眩晕状态
	{
		return SPT_HOLD;
	}
}
//------------------------------------------------------
StateChangeResultType SM_ServerNPCState_DIZZ::StateEnd(SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime )
{
	return SCRT_SUCCESS;
}