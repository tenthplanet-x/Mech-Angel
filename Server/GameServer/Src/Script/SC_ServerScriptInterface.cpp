#include "UT_DirConfig.h"
#include "ServerLogicScriptPCH.h"
#include "LS_LuaStateFactory.h"
#include "H_ServerScriptInterface.h"

#include "SC_ServerScriptInterface.inl"

using namespace Memory;
using namespace Script;
using namespace GameLogic;
using namespace Utility;
using namespace CSVFile;
using namespace Skill;
using namespace Protocol;
//---------------------------------------------------
bool SC_ServerScriptInterface::Init()
{
	SC_GameLogicScriptInterface::Init();

	m_pkServerSkillInterface = T_NEW_D SC_ServerSkillInterface;
	m_pkServerDebugInterface = T_NEW_D SC_ServerDebugInterface;

	m_pkYaZhiManager = T_NEW_D CYaZhiManager;

	LS_LuaState* pkState = LS_LuaStateFactory::GetSingleton().GetLogicLuaState();
	UT_DirConfig* pkDirConfig = UT_DirConfig::GetSingletonPtr();
	if(!pkDirConfig)
		return false;

	StringType kScriptServerPCHFileName = pkDirConfig->GetDir(CF_NODE_SCRIPT_SERVER_PCH);
	pkState->DoFile(kScriptServerPCHFileName);
	return true;
}
//---------------------------------------------------
bool SC_ServerScriptInterface::UnInit()
{
	T_SAFE_DELETE(m_pkServerSkillInterface);
	T_SAFE_DELETE(m_pkServerDebugInterface);
	T_SAFE_DELETE(m_pkYaZhiManager);
	return true;
}
//---------------------------------------------------
void SC_ServerScriptInterface::RegisterToLua()
{
	SC_GameLogicScriptInterface::RegisterToLua();

	/* 不能和SC_GameLogicScriptInterface::RegisterToLua注册重复 */
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_ServerTask );
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_ActorAttributeMap );
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_TaskScriptImportParam );
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_ServerTaskMap );
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_ServerPlayer );

	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( SC_ServerScriptInterface );
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( SC_ServerSkillInterface );
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( SC_ServerDebugInterface );

	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_MapLogicBase );
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_ServerMapLogicBase );

}