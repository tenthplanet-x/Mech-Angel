//服务器脚本接口
#ifndef _SC_SERVER_SCRIPT_INTERFACE_H
#define _SC_SERVER_SCRIPT_INTERFACE_H

#include "SC_GameLogicScriptInterface.h"

#include "LC_CombatYaZhiManager.h"
class SC_ServerSkillInterface;
class SC_ServerDebugInterface;
class CYaZhiManager;

class SC_ServerScriptInterface : public Script::SC_GameLogicScriptInterface,
								 public Utility::UT_Singleton<SC_ServerScriptInterface>
{
public:
	virtual bool				Init();
	virtual bool				UnInit();
	DECL_RTTI_NAME( SC_ServerScriptInterface )
	virtual void				RegisterToLua();

	SC_ServerSkillInterface*	GetSkillInterface(){return m_pkServerSkillInterface;};
	SC_ServerDebugInterface*	GetDebugInterface(){return m_pkServerDebugInterface;};
	CYaZhiManager* GetYaZhiManager() {return m_pkYaZhiManager;}

protected:
	SC_ServerSkillInterface*	m_pkServerSkillInterface;
	SC_ServerDebugInterface*	m_pkServerDebugInterface;
	CYaZhiManager* m_pkYaZhiManager;
};

#endif