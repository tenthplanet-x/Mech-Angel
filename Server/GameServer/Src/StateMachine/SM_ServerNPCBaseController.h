//服务器端NPC的状态控制器

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
	//更新路径
	void			_updateMovePath(StateMachine::SM_StateMachine* pkStateMachine, float fCurrentTime, float fDeltaTime);

	//状态更新
	void			_updateMachineState(StateMachine::SM_StateMachine* pkStateMachine, float fCurrentTime, float fDeltaTime);

	//战斗更新
	void			_updateFight(StateMachine::SM_StateMachine* pkStateMachine, float fCurrentTime, float fDeltaTime);
	object_id_type			_selectTarget(StateMachine::SM_StateMachine* pkStateMachine, float fCurrentTime, float fDeltaTime);

	//移动更新
	void			_updateTransform(StateMachine::SM_StateMachine* pkStateMachine, float fCurrentTime, float fDeltaTime);
	void			_translateAndRotateServerNPC(StateMachine::SM_StateMachine* pkStateMachine, float fCurrentTime,float fDeltaTime);

	void			_notifySkillEvent_EnterGuard(StateMachine::SM_StateMachine* pkStateMachine, Skill::SK_SkillEvent* pkSkillEvent);
	void			_notifySkillEvent_Damage(StateMachine::SM_StateMachine* pkStateMachine, Skill::SK_SkillEvent* pkSkillEvent);
	void			_notifySkillEvent_AttachState(StateMachine::SM_StateMachine* pkStateMachine, Skill::SK_SkillEvent* pkSkillEvent);
	void			_notifySkillEvent_DetachState(StateMachine::SM_StateMachine* pkStateMachine, Skill::SK_SkillEvent* pkSkillEvent);
	void			_notifySkillEvent_Retreat(StateMachine::SM_StateMachine* pkStateMachine, Skill::SK_SkillEvent* pkSkillEvent);

	//默认控制器处理技能事件
	void			_controllerHandleSkillEvent(StateMachine::SM_StateMachine* pkStateMachine, Skill::SK_SkillEvent* pkSkillEvent);
	//控制器脚本处理技能事件
	void			_controllerScriptHandleSkillEvent(StateMachine::SM_StateMachine* pkStateMachine, Skill::SK_SkillEvent* pkSkillEvent);
	//状态元脚本处理技能事件
	void			_stateScriptHandleSkillEvent(StateMachine::SM_StateMachine* pkStateMachine, Skill::SK_SkillEvent* pkSkillEvent);

	//广播仇恨
	void			_broadcastHatred(StateMachine::SM_StateMachine* pkStateMachine, int lHatredID);


	StateMachine::StateHandleEventType _getEventHandleType(Skill::SK_SkillEvent* pkSkillEvent);

	//以下Functor均可为空
	Utility::LC_ServerLuaVoidFunction* m_pkScriptUpdateMachineStateFunctor;	//状态更新
	Utility::LC_ServerLuaVoidFunction* m_pkScriptUpdateMovePathFunctor;		//路径更新，未使用
	Utility::LC_ServerLuaVoidFunction* m_pkScriptUpdateFightFunctor;			//战斗更新
	Utility::LC_ServerLuaVoidFunction* m_pkScriptUpdateTransformFunctor;		//移动更新

	StateMachine::SM_HandleEventMap		m_kHandleSkillEventMap;			//技能事件回调函数表
	StateMachine::SM_HandleEventMap		m_kHandleGameEventMap;   		//游戏事件回调函数表
};

#endif