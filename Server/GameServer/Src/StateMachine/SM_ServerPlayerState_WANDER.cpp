
#include "CF_MovePathList.h"
#include "SM_ServerPlayerState_WANDER.h"
#include "LC_ServerPlayer.h"
#include "PlayerStateAction.h"
#include "LC_MapBase.h"
#include "CF_Helper.h"

using namespace StateMachine;
using namespace Utility;
using namespace CSVFile;
using namespace Skill;
using namespace GameLogic;
//------------------------------------------------------
SM_ServerPlayerState_WANDER::SM_ServerPlayerState_WANDER()
: SM_PlayerState_WANDER()
{
}
//------------------------------------------------------
SM_ServerPlayerState_WANDER::~SM_ServerPlayerState_WANDER()
{
}
//------------------------------------------------------
bool SM_ServerPlayerState_WANDER::CanTransite(StateMachine::SM_StateMachine* pkMachine,  SM_StateBase* pkToTransiteState)
{
	return SM_PlayerState_WANDER::CanTransite(pkMachine,pkToTransiteState);
}
//------------------------------------------------------
StateChangeResultType SM_ServerPlayerState_WANDER::StateStart( StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime )
{
	SM_PlayerState_WANDER::StateStart(pkMachine,fCurrentTime,fDeltaTime);
	_computeWanderLocation(pkMachine);
	return SCRT_SUCCESS;
}
//------------------------------------------------------
StateProcessType SM_ServerPlayerState_WANDER::StateProcess(SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime )
{
	SM_PlayerState_WANDER::StateProcess(pkMachine,fCurrentTime,fDeltaTime);

	if (SPT_END == ProcessPlayerStateTestFollow(this,pkMachine,fCurrentTime,fDeltaTime,150))
		return SPT_END;

	//主动激活
	ProcessPlayerStatePositiveActive(this,pkMachine, fCurrentTime, fDeltaTime);

	//处理技能
	if (SPT_END == ProcessPlayerStateTestFight(this,pkMachine, fCurrentTime, fDeltaTime))
		return SPT_END;
	if (SPT_END == ProcessPlayerStateTestTalk(this,pkMachine, fCurrentTime, fDeltaTime))
	return SPT_END;

	return _processWander(pkMachine);
}
//------------------------------------------------------
StateChangeResultType SM_ServerPlayerState_WANDER::StateEnd( SM_StateMachine* pkMachine,float fCurrentTime,float fDeltaTime )
{
	LC_ServerPlayer* pkServerPlayer = static_cast<LC_ServerPlayer*>(pkMachine);
	
	pkServerPlayer->CacheCurrentPath(pkServerPlayer->GetCurrentLocation());
	
	return SCRT_SUCCESS;
}
//------------------------------------------------------
void SM_ServerPlayerState_WANDER::_initWanderPath(SM_StateMachine* pkMachine,CSVFile::CF_MovePathList::DataEntry* pkCSVData)
{
	LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)pkMachine;
	if (pkServerPlayer == NULL)
		return;

	UT_LinePath& rkPath = pkServerPlayer->GetMovePath();

	rkPath.Clear();
	//rkPath.SetLinePathType(UT_LinePath::LPT_FLIP);
	rkPath.SetLinePathType((UT_LinePath::LinePathType)pkCSVData->_iPathType);

	int nPathPointCount = pkCSVData->_iPathPointCount;
	
	for (int nIndex = 0; nIndex < nPathPointCount; ++nIndex)
	{
		Utility::UT_Vec3Int point= CSVHelper::GetMovePathPoint(pkCSVData,nIndex);
		if (nIndex==0)
		{
			rkPath.SetStartPoint(point);
		}
		else if (nIndex==nPathPointCount-1)
		{
			rkPath.SetTargetPoint(point);
		}
		rkPath.AddPathPoint(point );
	}

	rkPath.GeneratePath();
}
//------------------------------------------------------
void SM_ServerPlayerState_WANDER::_computeWanderLocation( StateMachine::SM_StateMachine* pkMachine )
{
	//LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)pkMachine;
}	
//------------------------------------------------------
StateProcessType SM_ServerPlayerState_WANDER::_processWander( StateMachine::SM_StateMachine* pkMachine )
{
	LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)pkMachine;

	bool bMoveEndPath = false;

	UT_LinePath& rkPath = pkServerPlayer->GetMovePath();
	if (rkPath.GetPathPointCount())
	{
		const UT_Vec3Int & kCurrentLocation = pkServerPlayer->GetCurrentLocation();
		const UT_Vec3Int & kTargetLocation = pkServerPlayer->GetTargetLocation();
		int nDistance = UT_MathBase::LineLengthXYInt(kCurrentLocation, kTargetLocation);
		if (nDistance <= DISTANCE_ADJUST)
			bMoveEndPath = true;
	}
	else
	{
		bMoveEndPath = true;
	}

	if(bMoveEndPath)
	{
		pkServerPlayer->SetNextState(ST_PLAYER_STAND);
		return SPT_END;
	}
	else
		return SPT_SUSPEND;
}
//------------------------------------------------------
