////////////////////////////////////////////////
//由Perl脚本自动生成的Lua绑定代码，请勿手动修改！//
//这些函数实现Lua调用和返回时对象的自动压栈
////////////////////////////////////////////////
#include "LS_LuaRegister.h"
#include "MM_BaseObject.h"
	
namespace GameLogic
{
	class LC_ActorAttributeMap;
};
class LC_ServerTask;
namespace GameLogic
{
	class LC_TaskScriptImportParam;
};
class LC_ServerTaskMap;
class LC_ServerPlayer;
class SC_ServerScriptInterface;
class SC_ServerSkillInterface;
class SC_ServerDebugInterface;
namespace GameLogic
{
	class LC_MapLogicBase;
};
class LC_ServerMapLogicBase;
namespace Utility
{
	
#ifndef H_SCRIPT_HANDGameLogicLC_ActorAttributeMap
#define H_SCRIPT_HANDGameLogicLC_ActorAttributeMap
#define SC_ServerScriptInterface_SCRIPT_HANDGameLogicLC_ActorAttributeMap
	void	LuaPushValue(lua_State* L, GameLogic::LC_ActorAttributeMap* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ActorAttributeMap*>, lua_State* L, int idx);
	GameLogic::LC_ActorAttributeMap*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ActorAttributeMap*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_ServerTask
#define H_SCRIPT_HANDLC_ServerTask
#define SC_ServerScriptInterface_SCRIPT_HANDLC_ServerTask
	void	LuaPushValue(lua_State* L, LC_ServerTask* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ServerTask*>, lua_State* L, int idx);
	LC_ServerTask*	LuaGetValue(Utility::LuaTypeWrapper<LC_ServerTask*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_TaskScriptImportParam
#define H_SCRIPT_HANDGameLogicLC_TaskScriptImportParam
#define SC_ServerScriptInterface_SCRIPT_HANDGameLogicLC_TaskScriptImportParam
	void	LuaPushValue(lua_State* L, GameLogic::LC_TaskScriptImportParam* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_TaskScriptImportParam*>, lua_State* L, int idx);
	GameLogic::LC_TaskScriptImportParam*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_TaskScriptImportParam*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_ServerTaskMap
#define H_SCRIPT_HANDLC_ServerTaskMap
#define SC_ServerScriptInterface_SCRIPT_HANDLC_ServerTaskMap
	void	LuaPushValue(lua_State* L, LC_ServerTaskMap* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ServerTaskMap*>, lua_State* L, int idx);
	LC_ServerTaskMap*	LuaGetValue(Utility::LuaTypeWrapper<LC_ServerTaskMap*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_ServerPlayer
#define H_SCRIPT_HANDLC_ServerPlayer
#define SC_ServerScriptInterface_SCRIPT_HANDLC_ServerPlayer
	void	LuaPushValue(lua_State* L, LC_ServerPlayer* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ServerPlayer*>, lua_State* L, int idx);
	LC_ServerPlayer*	LuaGetValue(Utility::LuaTypeWrapper<LC_ServerPlayer*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDSC_ServerScriptInterface
#define H_SCRIPT_HANDSC_ServerScriptInterface
#define SC_ServerScriptInterface_SCRIPT_HANDSC_ServerScriptInterface
	void	LuaPushValue(lua_State* L, SC_ServerScriptInterface* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<SC_ServerScriptInterface*>, lua_State* L, int idx);
	SC_ServerScriptInterface*	LuaGetValue(Utility::LuaTypeWrapper<SC_ServerScriptInterface*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDSC_ServerSkillInterface
#define H_SCRIPT_HANDSC_ServerSkillInterface
#define SC_ServerScriptInterface_SCRIPT_HANDSC_ServerSkillInterface
	void	LuaPushValue(lua_State* L, SC_ServerSkillInterface* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<SC_ServerSkillInterface*>, lua_State* L, int idx);
	SC_ServerSkillInterface*	LuaGetValue(Utility::LuaTypeWrapper<SC_ServerSkillInterface*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDSC_ServerDebugInterface
#define H_SCRIPT_HANDSC_ServerDebugInterface
#define SC_ServerScriptInterface_SCRIPT_HANDSC_ServerDebugInterface
	void	LuaPushValue(lua_State* L, SC_ServerDebugInterface* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<SC_ServerDebugInterface*>, lua_State* L, int idx);
	SC_ServerDebugInterface*	LuaGetValue(Utility::LuaTypeWrapper<SC_ServerDebugInterface*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_MapLogicBase
#define H_SCRIPT_HANDGameLogicLC_MapLogicBase
#define SC_ServerScriptInterface_SCRIPT_HANDGameLogicLC_MapLogicBase
	void	LuaPushValue(lua_State* L, GameLogic::LC_MapLogicBase* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_MapLogicBase*>, lua_State* L, int idx);
	GameLogic::LC_MapLogicBase*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_MapLogicBase*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_ServerMapLogicBase
#define H_SCRIPT_HANDLC_ServerMapLogicBase
#define SC_ServerScriptInterface_SCRIPT_HANDLC_ServerMapLogicBase
	void	LuaPushValue(lua_State* L, LC_ServerMapLogicBase* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ServerMapLogicBase*>, lua_State* L, int idx);
	LC_ServerMapLogicBase*	LuaGetValue(Utility::LuaTypeWrapper<LC_ServerMapLogicBase*>, lua_State* L, int idx);
#endif

}
