#include "H_ServerScriptInterface.h"

namespace Utility
{
	
#ifdef SC_ServerScriptInterface_SCRIPT_HANDGameLogicLC_ActorAttributeMap
	void	LuaPushValue(lua_State* L, GameLogic::LC_ActorAttributeMap* value)
	{
		if(value != NULL)
		{
			LuaBinder binder(L);
			binder.PushUserType(value, value->GetRTTIName());
		}
		else
		{
			lua_pushnil(L);
		}
	}
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ActorAttributeMap*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_ActorAttributeMap*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ActorAttributeMap*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_ActorAttributeMap*>(*ubox);  
	}
#endif

#ifdef SC_ServerScriptInterface_SCRIPT_HANDLC_ServerTask
	void	LuaPushValue(lua_State* L, LC_ServerTask* value)
	{
		if(value != NULL)
		{
			LuaBinder binder(L);
			binder.PushUserType(value, value->GetRTTIName());
		}
		else
		{
			lua_pushnil(L);
		}
	}
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ServerTask*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_ServerTask*	LuaGetValue(Utility::LuaTypeWrapper<LC_ServerTask*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_ServerTask*>(*ubox);  
	}
#endif

#ifdef SC_ServerScriptInterface_SCRIPT_HANDGameLogicLC_TaskScriptImportParam
	void	LuaPushValue(lua_State* L, GameLogic::LC_TaskScriptImportParam* value)
	{
		if(value != NULL)
		{
			LuaBinder binder(L);
			binder.PushUserType(value, value->GetRTTIName());
		}
		else
		{
			lua_pushnil(L);
		}
	}
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_TaskScriptImportParam*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_TaskScriptImportParam*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_TaskScriptImportParam*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_TaskScriptImportParam*>(*ubox);  
	}
#endif

#ifdef SC_ServerScriptInterface_SCRIPT_HANDLC_ServerTaskMap
	void	LuaPushValue(lua_State* L, LC_ServerTaskMap* value)
	{
		if(value != NULL)
		{
			LuaBinder binder(L);
			binder.PushUserType(value, value->GetRTTIName());
		}
		else
		{
			lua_pushnil(L);
		}
	}
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ServerTaskMap*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_ServerTaskMap*	LuaGetValue(Utility::LuaTypeWrapper<LC_ServerTaskMap*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_ServerTaskMap*>(*ubox);  
	}
#endif

#ifdef SC_ServerScriptInterface_SCRIPT_HANDLC_ServerPlayer
	void	LuaPushValue(lua_State* L, LC_ServerPlayer* value)
	{
		if(value != NULL)
		{
			LuaBinder binder(L);
			binder.PushUserType(value, value->GetRTTIName());
		}
		else
		{
			lua_pushnil(L);
		}
	}
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ServerPlayer*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_ServerPlayer*	LuaGetValue(Utility::LuaTypeWrapper<LC_ServerPlayer*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_ServerPlayer*>(*ubox);  
	}
#endif

#ifdef SC_ServerScriptInterface_SCRIPT_HANDSC_ServerScriptInterface
	void	LuaPushValue(lua_State* L, SC_ServerScriptInterface* value)
	{
		if(value != NULL)
		{
			LuaBinder binder(L);
			binder.PushUserType(value, value->GetRTTIName());
		}
		else
		{
			lua_pushnil(L);
		}
	}
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<SC_ServerScriptInterface*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	SC_ServerScriptInterface*	LuaGetValue(Utility::LuaTypeWrapper<SC_ServerScriptInterface*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<SC_ServerScriptInterface*>(*ubox);  
	}
#endif

#ifdef SC_ServerScriptInterface_SCRIPT_HANDSC_ServerSkillInterface
	void	LuaPushValue(lua_State* L, SC_ServerSkillInterface* value)
	{
		if(value != NULL)
		{
			LuaBinder binder(L);
			binder.PushUserType(value, value->GetRTTIName());
		}
		else
		{
			lua_pushnil(L);
		}
	}
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<SC_ServerSkillInterface*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	SC_ServerSkillInterface*	LuaGetValue(Utility::LuaTypeWrapper<SC_ServerSkillInterface*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<SC_ServerSkillInterface*>(*ubox);  
	}
#endif

#ifdef SC_ServerScriptInterface_SCRIPT_HANDSC_ServerDebugInterface
	void	LuaPushValue(lua_State* L, SC_ServerDebugInterface* value)
	{
		if(value != NULL)
		{
			LuaBinder binder(L);
			binder.PushUserType(value, value->GetRTTIName());
		}
		else
		{
			lua_pushnil(L);
		}
	}
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<SC_ServerDebugInterface*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	SC_ServerDebugInterface*	LuaGetValue(Utility::LuaTypeWrapper<SC_ServerDebugInterface*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<SC_ServerDebugInterface*>(*ubox);  
	}
#endif

#ifdef SC_ServerScriptInterface_SCRIPT_HANDGameLogicLC_MapLogicBase
	void	LuaPushValue(lua_State* L, GameLogic::LC_MapLogicBase* value)
	{
		if(value != NULL)
		{
			LuaBinder binder(L);
			binder.PushUserType(value, value->GetRTTIName());
		}
		else
		{
			lua_pushnil(L);
		}
	}
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_MapLogicBase*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_MapLogicBase*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_MapLogicBase*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_MapLogicBase*>(*ubox);  
	}
#endif

#ifdef SC_ServerScriptInterface_SCRIPT_HANDLC_ServerMapLogicBase
	void	LuaPushValue(lua_State* L, LC_ServerMapLogicBase* value)
	{
		if(value != NULL)
		{
			LuaBinder binder(L);
			binder.PushUserType(value, value->GetRTTIName());
		}
		else
		{
			lua_pushnil(L);
		}
	}
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ServerMapLogicBase*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_ServerMapLogicBase*	LuaGetValue(Utility::LuaTypeWrapper<LC_ServerMapLogicBase*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_ServerMapLogicBase*>(*ubox);  
	}
#endif

}
