//����������ҵ�״̬������

#ifndef _SM_SERVER_PLAYER_CONTROLLER_H
#define _SM_SERVER_PLAYER_CONTROLLER_H

#include "SM_ControllerBase.h"

namespace StateMachine
{
	class SM_ServerPlayerController : public SM_ControllerBase
	{
	public:
		SM_ServerPlayerController();
		virtual ~SM_ServerPlayerController();

		virtual void Update(SM_StateMachine* pkStateMachine, float fCurrentTime, float fDeltaTime);
		virtual void NotifySkillEvent(SM_StateMachine* pkStateMachine, Skill::SK_SkillEvent* pkSkillEvent);
		virtual bool NotifyStateEnd(SM_StateMachine* pkStateMachine, int iEndState,float fCurrentTime,float fDeltaTime);

	private:
		void _updateHatred(SM_StateMachine* pkStateMachine, float fCurrentTime, float fDeltaTime);
		//״̬����
		void			_updateMachineState(StateMachine::SM_StateMachine* pkStateMachine, float fCurrentTime, float fDeltaTime);

		//ս������
		void			_updateFight(StateMachine::SM_StateMachine* pkStateMachine, float fCurrentTime, float fDeltaTime);
		object_id_type			_selectTarget(StateMachine::SM_StateMachine* pkStateMachine, float fCurrentTime, float fDeltaTime);

		//�ƶ�����
		void			_updateTransform(StateMachine::SM_StateMachine* pkStateMachine, float fCurrentTime, float fDeltaTime);
		void			_translateAndRotateServerPlayer(StateMachine::SM_StateMachine* pkStateMachine, float fCurrentTime,float fDeltaTime);

	};
}

#endif