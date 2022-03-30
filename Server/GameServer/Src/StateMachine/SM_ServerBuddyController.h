//服务器端伙伴的状态控制器

#ifndef _SM_SERVER_BUDDY_CONTROLLER_H
#define _SM_SERVER_BUDDY_CONTROLLER_H

#include "SM_ControllerBase.h"
#include "UT_Vec.h"

using namespace Utility;
using namespace GameLogic;

class GameLogic::LC_ActorBase;
class LC_ServerPlayer;

namespace StateMachine
{
	class SM_ServerBuddyController : public SM_ControllerBase
	{
	public:
		SM_ServerBuddyController();
		virtual ~SM_ServerBuddyController();

		virtual void Update(SM_StateMachine* pkStateMachine, float fCurrentTime, float fDeltaTime);
		virtual void NotifySkillEvent(SM_StateMachine* pkStateMachine, Skill::SK_SkillEvent* pkSkillEvent);
		virtual bool NotifyStateEnd(SM_StateMachine* pkStateMachine, int iEndState,float fCurrentTime,float fDeltaTime);
	private:
		bool UpdateBuddyAction(SM_StateMachine* pkStateMachine,float fCurrentTime,float fDeltaTime);

	private:
		void _updateHatred(SM_StateMachine* pkStateMachine, float fCurrentTime, float fDeltaTime);
		//状态更新
		void			_updateMachineState(StateMachine::SM_StateMachine* pkStateMachine, float fCurrentTime, float fDeltaTime);

		//战斗更新
		void			_checkSkill(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);
		void			_pendingSkill(StateMachine::SM_StateMachine* pkMachine, float fCurrentTime,float fDeltaTime);
		StateMachine::StateProcessType _updateCastSkill(StateMachine::SM_StateMachine* pkStateMachine, float fCurrentTime, float fDeltaTime, object_id_type nTargetId);
		int			_updateFight(StateMachine::SM_StateMachine* pkStateMachine, float fCurrentTime, float fDeltaTime, object_id_type nTargetId);
		void			_updateFight_orig(StateMachine::SM_StateMachine* pkStateMachine, float fCurrentTime, float fDeltaTime);
		object_id_type			_selectTarget(StateMachine::SM_StateMachine* pkStateMachine, float fCurrentTime, float fDeltaTime);
		object_id_type			_selectTarget_orig(StateMachine::SM_StateMachine* pkStateMachine, float fCurrentTime, float fDeltaTime);

		//bool _checkFightDistance();

		//移动更新
		object_id_type	_updateTransform(StateMachine::SM_StateMachine* pkStateMachine, float fCurrentTime, float fDeltaTime, bool bCombat, object_id_type nTargetId);
		void			_updateMoveImpl_Path(StateMachine::SM_StateMachine* pkStateMachine, float fCurrentTime,float fDeltaTime);
		bool _updateMoveLogic_Skill(StateMachine::SM_StateMachine* pkStateMachine, float fCurrentTime,float fDeltaTime);
		bool _updateMoveLogic_ForceFollow(StateMachine::SM_StateMachine* pkStateMachine, float fCurrentTime,float fDeltaTime);
		bool _doMoveLogic_BeginFollow(StateMachine::SM_StateMachine* pkStateMachine, float fCurrentTime,float fDeltaTime, LC_ActorBase* pFollowTarget);
		object_id_type	_updateMoveLogic(StateMachine::SM_StateMachine* pkStateMachine, float fCurrentTime,float fDeltaTime, bool bCombat, object_id_type nTargetId);
		int	_updateMoveImpl(StateMachine::SM_StateMachine* pkStateMachine, float fCurrentTime,float fDeltaTime, object_id_type nFollowTargetId,
				UT_Vec3Int& kCurrentLocation, UT_Vec3Int& kTargetLocation, int nFollowChange, bool bCombat, bool bForce = false, bool bDenyFindPath = false, float fSpeedRate = 1.00f);

		int _doMoveImpl_FindPath( LC_ServerPlayer* pPlayer,  LC_ServerPlayer* pCurrentPlayer, float fSpeed, float fCurrentTime,float fDeltaTime );

		bool _updateTryForceDrag(LC_ServerPlayer* pBuddy, LC_ServerPlayer* pCurPlayer, float fCurrentTime,float fDeltaTime);
	private:
		// Position calculate funcs
		static int GetFollowLocation( StateMachine::SM_StateMachine* pkStateMachine, LC_ActorBase* pFollowTarget, Utility::UT_Vec3Int& rkResult);
		static int GetSkillLocation( StateMachine::SM_StateMachine* pkStateMachine, LC_ActorBase* pFollowTarget, Utility::UT_Vec3Int& rkResult, int32_t nSkillDistance );
	private:
		bool m_bAllowFightUpdate;
	};
}

#endif