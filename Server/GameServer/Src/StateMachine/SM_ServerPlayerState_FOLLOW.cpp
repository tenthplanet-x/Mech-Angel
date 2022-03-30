

#include "LC_ServerPlayer.h"
#include "SM_ServerPlayerState_FOLLOW.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "SK_ServerSkillEventManager.h"

#include "PlayerStateAction.h"

using namespace StateMachine;
using namespace CSVFile;
using namespace GameLogic;
using namespace Skill;
//------------------------------------------------------
SM_ServerPlayerState_FOLLOW::SM_ServerPlayerState_FOLLOW()
: SM_PlayerState_FOLLOW()
{
}
//------------------------------------------------------
SM_ServerPlayerState_FOLLOW::~SM_ServerPlayerState_FOLLOW()
{
}
//------------------------------------------------------
bool SM_ServerPlayerState_FOLLOW::CanTransite(SM_StateMachine* pkMachine,SM_StateBase* pkToTransiteState)
{
	return true;
}
//------------------------------------------------------
StateChangeResultType SM_ServerPlayerState_FOLLOW::StateStart(SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime)
{
	SM_PlayerState_FOLLOW::StateStart(pkMachine,fCurrentTime,fDeltaTime);

	return SCRT_SUCCESS;
}
//------------------------------------------------------
StateProcessType SM_ServerPlayerState_FOLLOW::StateProcess(SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime )
{
	SM_PlayerState_FOLLOW::StateProcess(pkMachine,fCurrentTime,fDeltaTime);
	
	//处理跟随
	if (SPT_SUSPEND == _processFollow(pkMachine))
		return SPT_SUSPEND;

	//主动激活
	ProcessPlayerStatePositiveActive(this,pkMachine, fCurrentTime, fDeltaTime);

	return ProcessPlayerStateTestFight(this,pkMachine,fCurrentTime,fDeltaTime);
}
//------------------------------------------------------
StateChangeResultType SM_ServerPlayerState_FOLLOW::StateEnd(SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime )
{
	return SCRT_SUCCESS;
}
//------------------------------------------------------
StateProcessType SM_ServerPlayerState_FOLLOW::_processFollow( StateMachine::SM_StateMachine* pkMachine )
{
	LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)pkMachine;
	//const UT_Vec3Int & kCurrentLocation = pkServerPlayer->GetCurrentLocation();
	
	LC_MapBase* pkMap = pkServerPlayer->GetMap();
	if(pkMap)
	{}
	
	pkMachine->SetNextState(ST_PLAYER_WANDER);
	return SPT_END;
}
