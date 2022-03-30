//��������NPC��״̬������

#ifndef _SM_SERVER_NPC_BASE_CONTROLLER_H
#define _SM_SERVER_NPC_BASE_CONTROLLER_H

#include "SM_ControllerBase.h"
#include "LC_ServerLuaFunction.h"

class SM_ServerNPCBaseController : public StateMachine::SM_ControllerBase
{
public:

public:
	SM_ServerNPCBaseController();
	virtual ~SM_ServerNPCBaseController();

	bool			InitScriptData(const StringType& rkScriptName);

 	virtual void	Update(StateMachine::SM_StateMachine* pkStateMachine, float fCurrentTime, float fDeltaTime);

	virtual void	NotifySkillEvent(StateMachine::SM_StateMachine* pkStateMachine, Skill::SK_SkillEvent* pkSkillEvent);
	virtual void	NotifyGameEvent(StateMachine::SM_StateMachine* pkStateMachine, GameLogic::LC_GameEvent* pkGameEvent);
 	virtual bool	NotifyStateEnd(StateMachine::SM_StateMachine* pkStateMachine, int iEndState,float fCurrentTime,float fDeltaTime);

protected:
	//����·��
	void			_updateMovePath(StateMachine::SM_StateMachine* pkStateMachine, float fCurrentTime, float fDeltaTime);

	//״̬����
	void			_updateMachineState(StateMachine::SM_StateMachine* pkStateMachine, float fCurrentTime, float fDeltaTime);

	//ս������
	void			_updateFight(StateMachine::SM_StateMachine* pkStateMachine, float fCurrentTime, float fDeltaTime);
	object_id_type			_selectTarget(StateMachine::SM_StateMachine* pkStateMachine, float fCurrentTime, float fDeltaTime);

	//�ƶ�����
	void			_updateTransform(StateMachine::SM_StateMachine* pkStateMachine, float fCurrentTime, float fDeltaTime);
	void			_translateAndRotateServerNPC(StateMachine::SM_StateMachine* pkStateMachine, float fCurrentTime,float fDeltaTime);

	void			_notifySkillEvent_EnterGuard(StateMachine::SM_StateMachine* pkStateMachine, Skill::SK_SkillEvent* pkSkillEvent);
	void			_notifySkillEvent_Damage(StateMachine::SM_StateMachine* pkStateMachine, Skill::SK_SkillEvent* pkSkillEvent);
	void			_notifySkillEvent_AttachState(StateMachine::SM_StateMachine* pkStateMachine, Skill::SK_SkillEvent* pkSkillEvent);
	void			_notifySkillEvent_DetachState(StateMachine::SM_StateMachine* pkStateMachine, Skill::SK_SkillEvent* pkSkillEvent);
	void			_notifySkillEvent_Retreat(StateMachine::SM_StateMachine* pkStateMachine, Skill::SK_SkillEvent* pkSkillEvent);

	//Ĭ�Ͽ������������¼�
	void			_controllerHandleSkillEvent(StateMachine::SM_StateMachine* pkStateMachine, Skill::SK_SkillEvent* pkSkillEvent);
	//�������ű��������¼�
	void			_controllerScriptHandleSkillEvent(StateMachine::SM_StateMachine* pkStateMachine, Skill::SK_SkillEvent* pkSkillEvent);
	//״̬Ԫ�ű��������¼�
	void			_stateScriptHandleSkillEvent(StateMachine::SM_StateMachine* pkStateMachine, Skill::SK_SkillEvent* pkSkillEvent);

	//�㲥���
	void			_broadcastHatred(StateMachine::SM_StateMachine* pkStateMachine, int lHatredID);


	StateMachine::StateHandleEventType _getEventHandleType(Skill::SK_SkillEvent* pkSkillEvent);

	//����Functor����Ϊ��
	Utility::LC_ServerLuaVoidFunction* m_pkScriptUpdateMachineStateFunctor;	//״̬����
	Utility::LC_ServerLuaVoidFunction* m_pkScriptUpdateMovePathFunctor;		//·�����£�δʹ��
	Utility::LC_ServerLuaVoidFunction* m_pkScriptUpdateFightFunctor;			//ս������
	Utility::LC_ServerLuaVoidFunction* m_pkScriptUpdateTransformFunctor;		//�ƶ�����

	StateMachine::SM_HandleEventMap		m_kHandleSkillEventMap;			//�����¼��ص�������
	StateMachine::SM_HandleEventMap		m_kHandleGameEventMap;   		//��Ϸ�¼��ص�������
};

#endif