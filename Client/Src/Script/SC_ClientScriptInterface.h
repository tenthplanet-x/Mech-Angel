//客户端脚本接口
#ifndef _SC_CLIENT_SCRIPT_INTERFACE_H
#define _SC_CLIENT_SCRIPT_INTERFACE_H

#include "SC_ClientSkillInterface.h"
#include "SC_ClientDebugInterface.h"

class SC_ClientScriptInterface : public Script::SC_GameLogicScriptInterface,
								 public Utility::UT_Singleton<SC_ClientScriptInterface>
{
public:
	virtual bool				Init();
	virtual bool				UnInit();
	DECL_RTTI_NAME( SC_ClientScriptInterface )
	virtual void				RegisterToLua();


	SC_ClientSkillInterface*	GetSkillInterface(){return m_pkClientSkillInterface;};
	SC_ClientDebugInterface*	GetDebugInterface(){return m_pkClientDebugInterface;};

protected:
	SC_ClientSkillInterface*	m_pkClientSkillInterface;
	SC_ClientDebugInterface*	m_pkClientDebugInterface;
};

#endif