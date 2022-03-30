	////////////////////////////////////////////////
	//由Perl脚本自动生成的Lua绑定代码，请勿手动修改！//
	//这些函数实现Lua调用和返回时对象的自动压栈
	////////////////////////////////////////////////
	#include "LS_LuaRegister.h"
#ifndef H_SCRIPT_HANDGameLogicLC_LogicObject
#define H_SCRIPT_HANDGameLogicLC_LogicObject

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_LogicObject* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_LogicObject*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_LogicObject*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_LogicObject*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_LogicObject*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_LogicObject& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_LogicObject* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_LogicObject*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_LogicObject*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_LogicObject*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_LogicObject*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_LogicObject*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDUtilityUT_Vec2Int
#define H_SCRIPT_HANDUtilityUT_Vec2Int

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, Utility::UT_Vec2Int* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<Utility::UT_Vec2Int*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline Utility::UT_Vec2Int*	LuaGetValue(Utility::LuaTypeWrapper<Utility::UT_Vec2Int*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<Utility::UT_Vec2Int*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, Utility::UT_Vec2Int& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const Utility::UT_Vec2Int* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<Utility::UT_Vec2Int*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const Utility::UT_Vec2Int*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline Utility::UT_Vec2Int*	LuaGetValue(Utility::LuaTypeWrapper<const Utility::UT_Vec2Int*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<Utility::UT_Vec2Int*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDUtilityUT_Vec3Int
#define H_SCRIPT_HANDUtilityUT_Vec3Int

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, Utility::UT_Vec3Int* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<Utility::UT_Vec3Int*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline Utility::UT_Vec3Int*	LuaGetValue(Utility::LuaTypeWrapper<Utility::UT_Vec3Int*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<Utility::UT_Vec3Int*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, Utility::UT_Vec3Int& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const Utility::UT_Vec3Int* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<Utility::UT_Vec3Int*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const Utility::UT_Vec3Int*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline Utility::UT_Vec3Int*	LuaGetValue(Utility::LuaTypeWrapper<const Utility::UT_Vec3Int*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<Utility::UT_Vec3Int*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDUtilityUT_Vec4Int
#define H_SCRIPT_HANDUtilityUT_Vec4Int

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, Utility::UT_Vec4Int* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<Utility::UT_Vec4Int*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline Utility::UT_Vec4Int*	LuaGetValue(Utility::LuaTypeWrapper<Utility::UT_Vec4Int*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<Utility::UT_Vec4Int*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, Utility::UT_Vec4Int& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const Utility::UT_Vec4Int* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<Utility::UT_Vec4Int*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const Utility::UT_Vec4Int*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline Utility::UT_Vec4Int*	LuaGetValue(Utility::LuaTypeWrapper<const Utility::UT_Vec4Int*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<Utility::UT_Vec4Int*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_Attacker
#define H_SCRIPT_HANDGameLogicLC_Attacker

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_Attacker* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_Attacker*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_Attacker*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_Attacker*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_Attacker*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_Attacker& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_Attacker* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_Attacker*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_Attacker*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_Attacker*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_Attacker*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_Attacker*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDSkillSK_SkillEvent
#define H_SCRIPT_HANDSkillSK_SkillEvent

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, Skill::SK_SkillEvent* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<Skill::SK_SkillEvent*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline Skill::SK_SkillEvent*	LuaGetValue(Utility::LuaTypeWrapper<Skill::SK_SkillEvent*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<Skill::SK_SkillEvent*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, Skill::SK_SkillEvent& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const Skill::SK_SkillEvent* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<Skill::SK_SkillEvent*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const Skill::SK_SkillEvent*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline Skill::SK_SkillEvent*	LuaGetValue(Utility::LuaTypeWrapper<const Skill::SK_SkillEvent*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<Skill::SK_SkillEvent*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_ActorBase
#define H_SCRIPT_HANDGameLogicLC_ActorBase

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ActorBase* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ActorBase*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ActorBase*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ActorBase*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ActorBase*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ActorBase& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_ActorBase* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_ActorBase*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_ActorBase*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ActorBase*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_ActorBase*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ActorBase*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_PlayerBase
#define H_SCRIPT_HANDGameLogicLC_PlayerBase

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_PlayerBase* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_PlayerBase*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_PlayerBase*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_PlayerBase*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_PlayerBase*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_PlayerBase& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_PlayerBase* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_PlayerBase*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_PlayerBase*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_PlayerBase*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_PlayerBase*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_PlayerBase*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDLC_ClientNetPlayer
#define H_SCRIPT_HANDLC_ClientNetPlayer

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientNetPlayer* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientNetPlayer*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientNetPlayer*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientNetPlayer*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientNetPlayer*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientNetPlayer& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const LC_ClientNetPlayer* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<LC_ClientNetPlayer*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const LC_ClientNetPlayer*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientNetPlayer*	LuaGetValue(Utility::LuaTypeWrapper<const LC_ClientNetPlayer*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientNetPlayer*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_NPCBase
#define H_SCRIPT_HANDGameLogicLC_NPCBase

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_NPCBase* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_NPCBase*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_NPCBase*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_NPCBase*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_NPCBase*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_NPCBase& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_NPCBase* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_NPCBase*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_NPCBase*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_NPCBase*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_NPCBase*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_NPCBase*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDLC_ClientNPC
#define H_SCRIPT_HANDLC_ClientNPC

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientNPC* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientNPC*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientNPC*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientNPC*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientNPC*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientNPC& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const LC_ClientNPC* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<LC_ClientNPC*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const LC_ClientNPC*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientNPC*	LuaGetValue(Utility::LuaTypeWrapper<const LC_ClientNPC*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientNPC*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDLC_ClientGamePlayer
#define H_SCRIPT_HANDLC_ClientGamePlayer

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientGamePlayer* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientGamePlayer*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientGamePlayer*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientGamePlayer*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientGamePlayer*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientGamePlayer& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const LC_ClientGamePlayer* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<LC_ClientGamePlayer*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const LC_ClientGamePlayer*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientGamePlayer*	LuaGetValue(Utility::LuaTypeWrapper<const LC_ClientGamePlayer*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientGamePlayer*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDLC_ClientGameStoryManager
#define H_SCRIPT_HANDLC_ClientGameStoryManager

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientGameStoryManager* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientGameStoryManager*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientGameStoryManager*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientGameStoryManager*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientGameStoryManager*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientGameStoryManager& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const LC_ClientGameStoryManager* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<LC_ClientGameStoryManager*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const LC_ClientGameStoryManager*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientGameStoryManager*	LuaGetValue(Utility::LuaTypeWrapper<const LC_ClientGameStoryManager*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientGameStoryManager*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDLC_ClientOpenAbilityManager
#define H_SCRIPT_HANDLC_ClientOpenAbilityManager

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientOpenAbilityManager* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientOpenAbilityManager*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientOpenAbilityManager*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientOpenAbilityManager*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientOpenAbilityManager*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientOpenAbilityManager& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const LC_ClientOpenAbilityManager* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<LC_ClientOpenAbilityManager*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const LC_ClientOpenAbilityManager*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientOpenAbilityManager*	LuaGetValue(Utility::LuaTypeWrapper<const LC_ClientOpenAbilityManager*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientOpenAbilityManager*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDLC_ClientTranscriptionInfoManager
#define H_SCRIPT_HANDLC_ClientTranscriptionInfoManager

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientTranscriptionInfoManager* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientTranscriptionInfoManager*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientTranscriptionInfoManager*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientTranscriptionInfoManager*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientTranscriptionInfoManager*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientTranscriptionInfoManager& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const LC_ClientTranscriptionInfoManager* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<LC_ClientTranscriptionInfoManager*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const LC_ClientTranscriptionInfoManager*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientTranscriptionInfoManager*	LuaGetValue(Utility::LuaTypeWrapper<const LC_ClientTranscriptionInfoManager*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientTranscriptionInfoManager*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameStoryRaidInfo
#define H_SCRIPT_HANDGameStoryRaidInfo

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameStoryRaidInfo* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameStoryRaidInfo*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameStoryRaidInfo*	LuaGetValue(Utility::LuaTypeWrapper<GameStoryRaidInfo*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameStoryRaidInfo*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameStoryRaidInfo& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameStoryRaidInfo* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameStoryRaidInfo*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameStoryRaidInfo*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameStoryRaidInfo*	LuaGetValue(Utility::LuaTypeWrapper<const GameStoryRaidInfo*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameStoryRaidInfo*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameStoryInfo
#define H_SCRIPT_HANDGameStoryInfo

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameStoryInfo* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameStoryInfo*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameStoryInfo*	LuaGetValue(Utility::LuaTypeWrapper<GameStoryInfo*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameStoryInfo*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameStoryInfo& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameStoryInfo* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameStoryInfo*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameStoryInfo*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameStoryInfo*	LuaGetValue(Utility::LuaTypeWrapper<const GameStoryInfo*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameStoryInfo*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_ActorAttributeMap
#define H_SCRIPT_HANDGameLogicLC_ActorAttributeMap

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ActorAttributeMap* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ActorAttributeMap*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ActorAttributeMap*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ActorAttributeMap*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ActorAttributeMap*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ActorAttributeMap& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_ActorAttributeMap* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_ActorAttributeMap*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_ActorAttributeMap*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ActorAttributeMap*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_ActorAttributeMap*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ActorAttributeMap*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_ActorAttributeAppendMap
#define H_SCRIPT_HANDGameLogicLC_ActorAttributeAppendMap

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ActorAttributeAppendMap* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ActorAttributeAppendMap*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ActorAttributeAppendMap*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ActorAttributeAppendMap*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ActorAttributeAppendMap*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ActorAttributeAppendMap& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_ActorAttributeAppendMap* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_ActorAttributeAppendMap*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_ActorAttributeAppendMap*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ActorAttributeAppendMap*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_ActorAttributeAppendMap*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ActorAttributeAppendMap*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_BackPackEntry
#define H_SCRIPT_HANDGameLogicLC_BackPackEntry

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_BackPackEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_BackPackEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_BackPackEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_BackPackEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_BackPackEntry*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_BackPackEntry& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_BackPackEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_BackPackEntry*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_BackPackEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_BackPackEntry*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_BackPackEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_BackPackEntry*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_PackAsset
#define H_SCRIPT_HANDGameLogicLC_PackAsset

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_PackAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_PackAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_PackAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_PackAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_PackAsset*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_PackAsset& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_PackAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_PackAsset*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_PackAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_PackAsset*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_PackAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_PackAsset*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_GreatKungFuAsset
#define H_SCRIPT_HANDGameLogicLC_GreatKungFuAsset

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_GreatKungFuAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_GreatKungFuAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_GreatKungFuAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_GreatKungFuAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_GreatKungFuAsset*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_GreatKungFuAsset& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_GreatKungFuAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_GreatKungFuAsset*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_GreatKungFuAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_GreatKungFuAsset*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_GreatKungFuAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_GreatKungFuAsset*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_ClientAlliance
#define H_SCRIPT_HANDGameLogicLC_ClientAlliance

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientAlliance* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientAlliance*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ClientAlliance*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientAlliance*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ClientAlliance*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientAlliance& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_ClientAlliance* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_ClientAlliance*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_ClientAlliance*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ClientAlliance*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_ClientAlliance*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ClientAlliance*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_ItemDescriptionEntry
#define H_SCRIPT_HANDGameLogicLC_ItemDescriptionEntry

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ItemDescriptionEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ItemDescriptionEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ItemDescriptionEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ItemDescriptionEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ItemDescriptionEntry*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ItemDescriptionEntry& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_ItemDescriptionEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_ItemDescriptionEntry*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_ItemDescriptionEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ItemDescriptionEntry*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_ItemDescriptionEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ItemDescriptionEntry*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_CrossRealmTeamAsset
#define H_SCRIPT_HANDGameLogicLC_CrossRealmTeamAsset

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_CrossRealmTeamAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_CrossRealmTeamAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_CrossRealmTeamAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_CrossRealmTeamAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_CrossRealmTeamAsset*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_CrossRealmTeamAsset& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_CrossRealmTeamAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_CrossRealmTeamAsset*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_CrossRealmTeamAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_CrossRealmTeamAsset*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_CrossRealmTeamAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_CrossRealmTeamAsset*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_ItemAppendAttributeFormulaEntry
#define H_SCRIPT_HANDGameLogicLC_ItemAppendAttributeFormulaEntry

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ItemAppendAttributeFormulaEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ItemAppendAttributeFormulaEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ItemAppendAttributeFormulaEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ItemAppendAttributeFormulaEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ItemAppendAttributeFormulaEntry*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ItemAppendAttributeFormulaEntry& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_ItemAppendAttributeFormulaEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_ItemAppendAttributeFormulaEntry*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_ItemAppendAttributeFormulaEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ItemAppendAttributeFormulaEntry*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_ItemAppendAttributeFormulaEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ItemAppendAttributeFormulaEntry*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_ItemUIContentEntry
#define H_SCRIPT_HANDGameLogicLC_ItemUIContentEntry

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ItemUIContentEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ItemUIContentEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ItemUIContentEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ItemUIContentEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ItemUIContentEntry*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ItemUIContentEntry& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_ItemUIContentEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_ItemUIContentEntry*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_ItemUIContentEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ItemUIContentEntry*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_ItemUIContentEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ItemUIContentEntry*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_ItemFactoryBase
#define H_SCRIPT_HANDGameLogicLC_ItemFactoryBase

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ItemFactoryBase* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ItemFactoryBase*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ItemFactoryBase*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ItemFactoryBase*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ItemFactoryBase*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ItemFactoryBase& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_ItemFactoryBase* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_ItemFactoryBase*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_ItemFactoryBase*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ItemFactoryBase*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_ItemFactoryBase*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ItemFactoryBase*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_ItemOperationDataEntry
#define H_SCRIPT_HANDGameLogicLC_ItemOperationDataEntry

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ItemOperationDataEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ItemOperationDataEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ItemOperationDataEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ItemOperationDataEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ItemOperationDataEntry*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ItemOperationDataEntry& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_ItemOperationDataEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_ItemOperationDataEntry*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_ItemOperationDataEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ItemOperationDataEntry*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_ItemOperationDataEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ItemOperationDataEntry*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_Task_Condition
#define H_SCRIPT_HANDGameLogicLC_Task_Condition

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_Task_Condition* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_Task_Condition*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_Task_Condition*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_Task_Condition*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_Task_Condition*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_Task_Condition& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_Task_Condition* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_Task_Condition*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_Task_Condition*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_Task_Condition*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_Task_Condition*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_Task_Condition*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_TaskManager
#define H_SCRIPT_HANDGameLogicLC_TaskManager

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_TaskManager* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_TaskManager*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_TaskManager*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_TaskManager*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_TaskManager*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_TaskManager& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_TaskManager* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_TaskManager*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_TaskManager*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_TaskManager*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_TaskManager*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_TaskManager*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_ProcessingTaskEntry
#define H_SCRIPT_HANDGameLogicLC_ProcessingTaskEntry

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ProcessingTaskEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ProcessingTaskEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ProcessingTaskEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ProcessingTaskEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ProcessingTaskEntry*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ProcessingTaskEntry& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_ProcessingTaskEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_ProcessingTaskEntry*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_ProcessingTaskEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ProcessingTaskEntry*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_ProcessingTaskEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ProcessingTaskEntry*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDLC_CanStartTaskMapEntry
#define H_SCRIPT_HANDLC_CanStartTaskMapEntry

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_CanStartTaskMapEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_CanStartTaskMapEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_CanStartTaskMapEntry*	LuaGetValue(Utility::LuaTypeWrapper<LC_CanStartTaskMapEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_CanStartTaskMapEntry*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_CanStartTaskMapEntry& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const LC_CanStartTaskMapEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<LC_CanStartTaskMapEntry*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const LC_CanStartTaskMapEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_CanStartTaskMapEntry*	LuaGetValue(Utility::LuaTypeWrapper<const LC_CanStartTaskMapEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_CanStartTaskMapEntry*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_FinishedTaskRecord
#define H_SCRIPT_HANDGameLogicLC_FinishedTaskRecord

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_FinishedTaskRecord* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_FinishedTaskRecord*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_FinishedTaskRecord*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_FinishedTaskRecord*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_FinishedTaskRecord*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_FinishedTaskRecord& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_FinishedTaskRecord* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_FinishedTaskRecord*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_FinishedTaskRecord*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_FinishedTaskRecord*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_FinishedTaskRecord*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_FinishedTaskRecord*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_MainTaskDescriptEntry
#define H_SCRIPT_HANDGameLogicLC_MainTaskDescriptEntry

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_MainTaskDescriptEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_MainTaskDescriptEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_MainTaskDescriptEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_MainTaskDescriptEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_MainTaskDescriptEntry*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_MainTaskDescriptEntry& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_MainTaskDescriptEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_MainTaskDescriptEntry*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_MainTaskDescriptEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_MainTaskDescriptEntry*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_MainTaskDescriptEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_MainTaskDescriptEntry*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_MainTask
#define H_SCRIPT_HANDGameLogicLC_MainTask

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_MainTask* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_MainTask*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_MainTask*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_MainTask*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_MainTask*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_MainTask& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_MainTask* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_MainTask*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_MainTask*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_MainTask*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_MainTask*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_MainTask*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_TaskParams
#define H_SCRIPT_HANDGameLogicLC_TaskParams

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_TaskParams* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_TaskParams*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_TaskParams*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_TaskParams*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_TaskParams*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_TaskParams& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_TaskParams* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_TaskParams*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_TaskParams*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_TaskParams*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_TaskParams*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_TaskParams*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_Task
#define H_SCRIPT_HANDGameLogicLC_Task

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_Task* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_Task*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_Task*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_Task*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_Task*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_Task& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_Task* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_Task*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_Task*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_Task*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_Task*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_Task*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_DailyLivenessEntry
#define H_SCRIPT_HANDGameLogicLC_DailyLivenessEntry

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_DailyLivenessEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_DailyLivenessEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_DailyLivenessEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_DailyLivenessEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_DailyLivenessEntry*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_DailyLivenessEntry& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_DailyLivenessEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_DailyLivenessEntry*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_DailyLivenessEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_DailyLivenessEntry*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_DailyLivenessEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_DailyLivenessEntry*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDLC_ClientTask
#define H_SCRIPT_HANDLC_ClientTask

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientTask* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientTask*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientTask*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientTask*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientTask*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientTask& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const LC_ClientTask* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<LC_ClientTask*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const LC_ClientTask*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientTask*	LuaGetValue(Utility::LuaTypeWrapper<const LC_ClientTask*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientTask*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_TaskMap
#define H_SCRIPT_HANDGameLogicLC_TaskMap

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_TaskMap* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_TaskMap*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_TaskMap*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_TaskMap*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_TaskMap*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_TaskMap& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_TaskMap* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_TaskMap*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_TaskMap*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_TaskMap*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_TaskMap*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_TaskMap*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_AssignTaskInfo
#define H_SCRIPT_HANDGameLogicLC_AssignTaskInfo

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_AssignTaskInfo* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_AssignTaskInfo*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_AssignTaskInfo*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_AssignTaskInfo*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_AssignTaskInfo*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_AssignTaskInfo& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_AssignTaskInfo* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_AssignTaskInfo*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_AssignTaskInfo*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_AssignTaskInfo*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_AssignTaskInfo*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_AssignTaskInfo*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_TaskAssignEntry
#define H_SCRIPT_HANDGameLogicLC_TaskAssignEntry

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_TaskAssignEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_TaskAssignEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_TaskAssignEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_TaskAssignEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_TaskAssignEntry*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_TaskAssignEntry& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_TaskAssignEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_TaskAssignEntry*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_TaskAssignEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_TaskAssignEntry*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_TaskAssignEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_TaskAssignEntry*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_AssignTaskData
#define H_SCRIPT_HANDGameLogicLC_AssignTaskData

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_AssignTaskData* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_AssignTaskData*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_AssignTaskData*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_AssignTaskData*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_AssignTaskData*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_AssignTaskData& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_AssignTaskData* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_AssignTaskData*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_AssignTaskData*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_AssignTaskData*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_AssignTaskData*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_AssignTaskData*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDLC_ClientTaskMap
#define H_SCRIPT_HANDLC_ClientTaskMap

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientTaskMap* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientTaskMap*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientTaskMap*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientTaskMap*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientTaskMap*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientTaskMap& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const LC_ClientTaskMap* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<LC_ClientTaskMap*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const LC_ClientTaskMap*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientTaskMap*	LuaGetValue(Utility::LuaTypeWrapper<const LC_ClientTaskMap*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientTaskMap*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_TradeManager
#define H_SCRIPT_HANDGameLogicLC_TradeManager

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_TradeManager* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_TradeManager*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_TradeManager*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_TradeManager*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_TradeManager*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_TradeManager& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_TradeManager* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_TradeManager*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_TradeManager*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_TradeManager*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_TradeManager*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_TradeManager*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_NPCShopItemEntry
#define H_SCRIPT_HANDGameLogicLC_NPCShopItemEntry

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_NPCShopItemEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_NPCShopItemEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_NPCShopItemEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_NPCShopItemEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_NPCShopItemEntry*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_NPCShopItemEntry& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_NPCShopItemEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_NPCShopItemEntry*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_NPCShopItemEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_NPCShopItemEntry*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_NPCShopItemEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_NPCShopItemEntry*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_ShopBase
#define H_SCRIPT_HANDGameLogicLC_ShopBase

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ShopBase* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ShopBase*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ShopBase*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ShopBase*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ShopBase*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ShopBase& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_ShopBase* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_ShopBase*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_ShopBase*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ShopBase*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_ShopBase*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ShopBase*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_TransferBase
#define H_SCRIPT_HANDGameLogicLC_TransferBase

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_TransferBase* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_TransferBase*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_TransferBase*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_TransferBase*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_TransferBase*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_TransferBase& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_TransferBase* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_TransferBase*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_TransferBase*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_TransferBase*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_TransferBase*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_TransferBase*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_SkillCoolDownMap
#define H_SCRIPT_HANDGameLogicLC_SkillCoolDownMap

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_SkillCoolDownMap* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_SkillCoolDownMap*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_SkillCoolDownMap*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_SkillCoolDownMap*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_SkillCoolDownMap*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_SkillCoolDownMap& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_SkillCoolDownMap* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_SkillCoolDownMap*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_SkillCoolDownMap*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_SkillCoolDownMap*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_SkillCoolDownMap*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_SkillCoolDownMap*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_ItemCoolDownMapBase
#define H_SCRIPT_HANDGameLogicLC_ItemCoolDownMapBase

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ItemCoolDownMapBase* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ItemCoolDownMapBase*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ItemCoolDownMapBase*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ItemCoolDownMapBase*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ItemCoolDownMapBase*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ItemCoolDownMapBase& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_ItemCoolDownMapBase* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_ItemCoolDownMapBase*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_ItemCoolDownMapBase*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ItemCoolDownMapBase*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_ItemCoolDownMapBase*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ItemCoolDownMapBase*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_SkillAssetEntry
#define H_SCRIPT_HANDGameLogicLC_SkillAssetEntry

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_SkillAssetEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_SkillAssetEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_SkillAssetEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_SkillAssetEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_SkillAssetEntry*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_SkillAssetEntry& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_SkillAssetEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_SkillAssetEntry*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_SkillAssetEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_SkillAssetEntry*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_SkillAssetEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_SkillAssetEntry*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDSkillLC_SkillDescriptionEntry
#define H_SCRIPT_HANDSkillLC_SkillDescriptionEntry

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, Skill::LC_SkillDescriptionEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<Skill::LC_SkillDescriptionEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline Skill::LC_SkillDescriptionEntry*	LuaGetValue(Utility::LuaTypeWrapper<Skill::LC_SkillDescriptionEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<Skill::LC_SkillDescriptionEntry*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, Skill::LC_SkillDescriptionEntry& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const Skill::LC_SkillDescriptionEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<Skill::LC_SkillDescriptionEntry*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const Skill::LC_SkillDescriptionEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline Skill::LC_SkillDescriptionEntry*	LuaGetValue(Utility::LuaTypeWrapper<const Skill::LC_SkillDescriptionEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<Skill::LC_SkillDescriptionEntry*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_SkillAsset
#define H_SCRIPT_HANDGameLogicLC_SkillAsset

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_SkillAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_SkillAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_SkillAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_SkillAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_SkillAsset*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_SkillAsset& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_SkillAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_SkillAsset*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_SkillAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_SkillAsset*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_SkillAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_SkillAsset*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_ActorStateEntry
#define H_SCRIPT_HANDGameLogicLC_ActorStateEntry

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ActorStateEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ActorStateEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ActorStateEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ActorStateEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ActorStateEntry*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ActorStateEntry& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_ActorStateEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_ActorStateEntry*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_ActorStateEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ActorStateEntry*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_ActorStateEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ActorStateEntry*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDSkillLC_StateDescriptionEntry
#define H_SCRIPT_HANDSkillLC_StateDescriptionEntry

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, Skill::LC_StateDescriptionEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<Skill::LC_StateDescriptionEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline Skill::LC_StateDescriptionEntry*	LuaGetValue(Utility::LuaTypeWrapper<Skill::LC_StateDescriptionEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<Skill::LC_StateDescriptionEntry*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, Skill::LC_StateDescriptionEntry& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const Skill::LC_StateDescriptionEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<Skill::LC_StateDescriptionEntry*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const Skill::LC_StateDescriptionEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline Skill::LC_StateDescriptionEntry*	LuaGetValue(Utility::LuaTypeWrapper<const Skill::LC_StateDescriptionEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<Skill::LC_StateDescriptionEntry*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_ActorStateMap
#define H_SCRIPT_HANDGameLogicLC_ActorStateMap

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ActorStateMap* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ActorStateMap*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ActorStateMap*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ActorStateMap*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ActorStateMap*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ActorStateMap& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_ActorStateMap* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_ActorStateMap*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_ActorStateMap*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ActorStateMap*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_ActorStateMap*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ActorStateMap*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_ShortcutEntry
#define H_SCRIPT_HANDGameLogicLC_ShortcutEntry

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ShortcutEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ShortcutEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ShortcutEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ShortcutEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ShortcutEntry*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ShortcutEntry& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_ShortcutEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_ShortcutEntry*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_ShortcutEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ShortcutEntry*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_ShortcutEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ShortcutEntry*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_ShortcutBar
#define H_SCRIPT_HANDGameLogicLC_ShortcutBar

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ShortcutBar* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ShortcutBar*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ShortcutBar*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ShortcutBar*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ShortcutBar*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ShortcutBar& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_ShortcutBar* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_ShortcutBar*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_ShortcutBar*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ShortcutBar*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_ShortcutBar*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ShortcutBar*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDTT_TextManager
#define H_SCRIPT_HANDTT_TextManager

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, TT_TextManager* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<TT_TextManager*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline TT_TextManager*	LuaGetValue(Utility::LuaTypeWrapper<TT_TextManager*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<TT_TextManager*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, TT_TextManager& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const TT_TextManager* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<TT_TextManager*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const TT_TextManager*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline TT_TextManager*	LuaGetValue(Utility::LuaTypeWrapper<const TT_TextManager*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<TT_TextManager*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDLC_TooltipManager
#define H_SCRIPT_HANDLC_TooltipManager

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_TooltipManager* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_TooltipManager*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_TooltipManager*	LuaGetValue(Utility::LuaTypeWrapper<LC_TooltipManager*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_TooltipManager*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_TooltipManager& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const LC_TooltipManager* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<LC_TooltipManager*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const LC_TooltipManager*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_TooltipManager*	LuaGetValue(Utility::LuaTypeWrapper<const LC_TooltipManager*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_TooltipManager*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDLC_MapManager
#define H_SCRIPT_HANDLC_MapManager

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_MapManager* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_MapManager*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_MapManager*	LuaGetValue(Utility::LuaTypeWrapper<LC_MapManager*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_MapManager*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_MapManager& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const LC_MapManager* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<LC_MapManager*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const LC_MapManager*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_MapManager*	LuaGetValue(Utility::LuaTypeWrapper<const LC_MapManager*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_MapManager*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDLC_MiniMap
#define H_SCRIPT_HANDLC_MiniMap

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_MiniMap* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_MiniMap*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_MiniMap*	LuaGetValue(Utility::LuaTypeWrapper<LC_MiniMap*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_MiniMap*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_MiniMap& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const LC_MiniMap* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<LC_MiniMap*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const LC_MiniMap*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_MiniMap*	LuaGetValue(Utility::LuaTypeWrapper<const LC_MiniMap*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_MiniMap*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDLC_TownMap
#define H_SCRIPT_HANDLC_TownMap

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_TownMap* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_TownMap*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_TownMap*	LuaGetValue(Utility::LuaTypeWrapper<LC_TownMap*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_TownMap*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_TownMap& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const LC_TownMap* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<LC_TownMap*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const LC_TownMap*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_TownMap*	LuaGetValue(Utility::LuaTypeWrapper<const LC_TownMap*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_TownMap*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDLC_ObjectDescription
#define H_SCRIPT_HANDLC_ObjectDescription

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ObjectDescription* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ObjectDescription*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ObjectDescription*	LuaGetValue(Utility::LuaTypeWrapper<LC_ObjectDescription*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ObjectDescription*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ObjectDescription& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const LC_ObjectDescription* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<LC_ObjectDescription*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const LC_ObjectDescription*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ObjectDescription*	LuaGetValue(Utility::LuaTypeWrapper<const LC_ObjectDescription*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ObjectDescription*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDLC_RadarObjectDescription
#define H_SCRIPT_HANDLC_RadarObjectDescription

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_RadarObjectDescription* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_RadarObjectDescription*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_RadarObjectDescription*	LuaGetValue(Utility::LuaTypeWrapper<LC_RadarObjectDescription*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_RadarObjectDescription*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_RadarObjectDescription& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const LC_RadarObjectDescription* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<LC_RadarObjectDescription*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const LC_RadarObjectDescription*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_RadarObjectDescription*	LuaGetValue(Utility::LuaTypeWrapper<const LC_RadarObjectDescription*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_RadarObjectDescription*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDLC_TownIconDescription
#define H_SCRIPT_HANDLC_TownIconDescription

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_TownIconDescription* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_TownIconDescription*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_TownIconDescription*	LuaGetValue(Utility::LuaTypeWrapper<LC_TownIconDescription*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_TownIconDescription*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_TownIconDescription& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const LC_TownIconDescription* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<LC_TownIconDescription*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const LC_TownIconDescription*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_TownIconDescription*	LuaGetValue(Utility::LuaTypeWrapper<const LC_TownIconDescription*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_TownIconDescription*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDLC_PathDesc
#define H_SCRIPT_HANDLC_PathDesc

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_PathDesc* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_PathDesc*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_PathDesc*	LuaGetValue(Utility::LuaTypeWrapper<LC_PathDesc*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_PathDesc*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_PathDesc& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const LC_PathDesc* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<LC_PathDesc*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const LC_PathDesc*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_PathDesc*	LuaGetValue(Utility::LuaTypeWrapper<const LC_PathDesc*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_PathDesc*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDLC_RadarMap
#define H_SCRIPT_HANDLC_RadarMap

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_RadarMap* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_RadarMap*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_RadarMap*	LuaGetValue(Utility::LuaTypeWrapper<LC_RadarMap*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_RadarMap*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_RadarMap& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const LC_RadarMap* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<LC_RadarMap*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const LC_RadarMap*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_RadarMap*	LuaGetValue(Utility::LuaTypeWrapper<const LC_RadarMap*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_RadarMap*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_PlayerGroupEntry
#define H_SCRIPT_HANDGameLogicLC_PlayerGroupEntry

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_PlayerGroupEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_PlayerGroupEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_PlayerGroupEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_PlayerGroupEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_PlayerGroupEntry*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_PlayerGroupEntry& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_PlayerGroupEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_PlayerGroupEntry*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_PlayerGroupEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_PlayerGroupEntry*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_PlayerGroupEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_PlayerGroupEntry*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_PlayerGroupBase
#define H_SCRIPT_HANDGameLogicLC_PlayerGroupBase

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_PlayerGroupBase* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_PlayerGroupBase*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_PlayerGroupBase*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_PlayerGroupBase*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_PlayerGroupBase*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_PlayerGroupBase& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_PlayerGroupBase* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_PlayerGroupBase*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_PlayerGroupBase*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_PlayerGroupBase*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_PlayerGroupBase*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_PlayerGroupBase*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_ShopCityGoodsDescriptionEntry
#define H_SCRIPT_HANDGameLogicLC_ShopCityGoodsDescriptionEntry

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ShopCityGoodsDescriptionEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ShopCityGoodsDescriptionEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ShopCityGoodsDescriptionEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ShopCityGoodsDescriptionEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ShopCityGoodsDescriptionEntry*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ShopCityGoodsDescriptionEntry& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_ShopCityGoodsDescriptionEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_ShopCityGoodsDescriptionEntry*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_ShopCityGoodsDescriptionEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ShopCityGoodsDescriptionEntry*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_ShopCityGoodsDescriptionEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ShopCityGoodsDescriptionEntry*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_ClientShopCity
#define H_SCRIPT_HANDGameLogicLC_ClientShopCity

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientShopCity* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientShopCity*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ClientShopCity*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientShopCity*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ClientShopCity*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientShopCity& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_ClientShopCity* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_ClientShopCity*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_ClientShopCity*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ClientShopCity*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_ClientShopCity*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ClientShopCity*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_PKRecorderBase
#define H_SCRIPT_HANDGameLogicLC_PKRecorderBase

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_PKRecorderBase* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_PKRecorderBase*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_PKRecorderBase*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_PKRecorderBase*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_PKRecorderBase*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_PKRecorderBase& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_PKRecorderBase* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_PKRecorderBase*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_PKRecorderBase*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_PKRecorderBase*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_PKRecorderBase*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_PKRecorderBase*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDSC_ClientScriptInterface
#define H_SCRIPT_HANDSC_ClientScriptInterface

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, SC_ClientScriptInterface* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<SC_ClientScriptInterface*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline SC_ClientScriptInterface*	LuaGetValue(Utility::LuaTypeWrapper<SC_ClientScriptInterface*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<SC_ClientScriptInterface*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, SC_ClientScriptInterface& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const SC_ClientScriptInterface* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<SC_ClientScriptInterface*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const SC_ClientScriptInterface*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline SC_ClientScriptInterface*	LuaGetValue(Utility::LuaTypeWrapper<const SC_ClientScriptInterface*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<SC_ClientScriptInterface*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDSC_ClientSkillInterface
#define H_SCRIPT_HANDSC_ClientSkillInterface

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, SC_ClientSkillInterface* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<SC_ClientSkillInterface*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline SC_ClientSkillInterface*	LuaGetValue(Utility::LuaTypeWrapper<SC_ClientSkillInterface*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<SC_ClientSkillInterface*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, SC_ClientSkillInterface& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const SC_ClientSkillInterface* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<SC_ClientSkillInterface*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const SC_ClientSkillInterface*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline SC_ClientSkillInterface*	LuaGetValue(Utility::LuaTypeWrapper<const SC_ClientSkillInterface*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<SC_ClientSkillInterface*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDSC_ClientDebugInterface
#define H_SCRIPT_HANDSC_ClientDebugInterface

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, SC_ClientDebugInterface* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<SC_ClientDebugInterface*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline SC_ClientDebugInterface*	LuaGetValue(Utility::LuaTypeWrapper<SC_ClientDebugInterface*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<SC_ClientDebugInterface*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, SC_ClientDebugInterface& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const SC_ClientDebugInterface* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<SC_ClientDebugInterface*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const SC_ClientDebugInterface*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline SC_ClientDebugInterface*	LuaGetValue(Utility::LuaTypeWrapper<const SC_ClientDebugInterface*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<SC_ClientDebugInterface*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDPathPF_Path
#define H_SCRIPT_HANDPathPF_Path

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, Path::PF_Path* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<Path::PF_Path*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline Path::PF_Path*	LuaGetValue(Utility::LuaTypeWrapper<Path::PF_Path*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<Path::PF_Path*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, Path::PF_Path& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const Path::PF_Path* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<Path::PF_Path*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const Path::PF_Path*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline Path::PF_Path*	LuaGetValue(Utility::LuaTypeWrapper<const Path::PF_Path*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<Path::PF_Path*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_SkillInfoEntry
#define H_SCRIPT_HANDGameLogicLC_SkillInfoEntry

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_SkillInfoEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_SkillInfoEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_SkillInfoEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_SkillInfoEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_SkillInfoEntry*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_SkillInfoEntry& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_SkillInfoEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_SkillInfoEntry*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_SkillInfoEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_SkillInfoEntry*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_SkillInfoEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_SkillInfoEntry*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDLC_PlayerFlopManager
#define H_SCRIPT_HANDLC_PlayerFlopManager

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_PlayerFlopManager* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_PlayerFlopManager*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_PlayerFlopManager*	LuaGetValue(Utility::LuaTypeWrapper<LC_PlayerFlopManager*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_PlayerFlopManager*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_PlayerFlopManager& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const LC_PlayerFlopManager* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<LC_PlayerFlopManager*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const LC_PlayerFlopManager*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_PlayerFlopManager*	LuaGetValue(Utility::LuaTypeWrapper<const LC_PlayerFlopManager*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_PlayerFlopManager*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDUtilityUT_GlobalSettings
#define H_SCRIPT_HANDUtilityUT_GlobalSettings

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, Utility::UT_GlobalSettings* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<Utility::UT_GlobalSettings*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline Utility::UT_GlobalSettings*	LuaGetValue(Utility::LuaTypeWrapper<Utility::UT_GlobalSettings*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<Utility::UT_GlobalSettings*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, Utility::UT_GlobalSettings& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const Utility::UT_GlobalSettings* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<Utility::UT_GlobalSettings*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const Utility::UT_GlobalSettings*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline Utility::UT_GlobalSettings*	LuaGetValue(Utility::LuaTypeWrapper<const Utility::UT_GlobalSettings*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<Utility::UT_GlobalSettings*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDLC_ProductionAssetEntry
#define H_SCRIPT_HANDLC_ProductionAssetEntry

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ProductionAssetEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ProductionAssetEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ProductionAssetEntry*	LuaGetValue(Utility::LuaTypeWrapper<LC_ProductionAssetEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ProductionAssetEntry*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ProductionAssetEntry& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const LC_ProductionAssetEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<LC_ProductionAssetEntry*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const LC_ProductionAssetEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ProductionAssetEntry*	LuaGetValue(Utility::LuaTypeWrapper<const LC_ProductionAssetEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ProductionAssetEntry*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDLC_ClientProduction
#define H_SCRIPT_HANDLC_ClientProduction

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientProduction* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientProduction*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientProduction*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientProduction*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientProduction*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientProduction& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const LC_ClientProduction* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<LC_ClientProduction*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const LC_ClientProduction*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientProduction*	LuaGetValue(Utility::LuaTypeWrapper<const LC_ClientProduction*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientProduction*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDLC_ClientDialogManager
#define H_SCRIPT_HANDLC_ClientDialogManager

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientDialogManager* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientDialogManager*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientDialogManager*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientDialogManager*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientDialogManager*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientDialogManager& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const LC_ClientDialogManager* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<LC_ClientDialogManager*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const LC_ClientDialogManager*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientDialogManager*	LuaGetValue(Utility::LuaTypeWrapper<const LC_ClientDialogManager*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientDialogManager*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDLC_ClientDialogInfo
#define H_SCRIPT_HANDLC_ClientDialogInfo

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientDialogInfo* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientDialogInfo*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientDialogInfo*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientDialogInfo*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientDialogInfo*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientDialogInfo& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const LC_ClientDialogInfo* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<LC_ClientDialogInfo*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const LC_ClientDialogInfo*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientDialogInfo*	LuaGetValue(Utility::LuaTypeWrapper<const LC_ClientDialogInfo*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientDialogInfo*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDLC_DialogSet
#define H_SCRIPT_HANDLC_DialogSet

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_DialogSet* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_DialogSet*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_DialogSet*	LuaGetValue(Utility::LuaTypeWrapper<LC_DialogSet*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_DialogSet*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_DialogSet& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const LC_DialogSet* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<LC_DialogSet*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const LC_DialogSet*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_DialogSet*	LuaGetValue(Utility::LuaTypeWrapper<const LC_DialogSet*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_DialogSet*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDLC_DialogNodeData
#define H_SCRIPT_HANDLC_DialogNodeData

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_DialogNodeData* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_DialogNodeData*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_DialogNodeData*	LuaGetValue(Utility::LuaTypeWrapper<LC_DialogNodeData*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_DialogNodeData*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_DialogNodeData& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const LC_DialogNodeData* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<LC_DialogNodeData*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const LC_DialogNodeData*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_DialogNodeData*	LuaGetValue(Utility::LuaTypeWrapper<const LC_DialogNodeData*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_DialogNodeData*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_ClientMailManager
#define H_SCRIPT_HANDGameLogicLC_ClientMailManager

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientMailManager* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientMailManager*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ClientMailManager*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientMailManager*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ClientMailManager*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientMailManager& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_ClientMailManager* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_ClientMailManager*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_ClientMailManager*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ClientMailManager*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_ClientMailManager*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ClientMailManager*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_MailAsset
#define H_SCRIPT_HANDGameLogicLC_MailAsset

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_MailAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_MailAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_MailAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_MailAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_MailAsset*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_MailAsset& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_MailAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_MailAsset*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_MailAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_MailAsset*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_MailAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_MailAsset*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_MailInfo
#define H_SCRIPT_HANDGameLogicLC_MailInfo

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_MailInfo* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_MailInfo*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_MailInfo*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_MailInfo*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_MailInfo*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_MailInfo& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_MailInfo* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_MailInfo*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_MailInfo*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_MailInfo*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_MailInfo*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_MailInfo*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_ClientItemAffixInfo
#define H_SCRIPT_HANDGameLogicLC_ClientItemAffixInfo

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientItemAffixInfo* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientItemAffixInfo*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ClientItemAffixInfo*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientItemAffixInfo*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ClientItemAffixInfo*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientItemAffixInfo& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_ClientItemAffixInfo* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_ClientItemAffixInfo*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_ClientItemAffixInfo*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ClientItemAffixInfo*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_ClientItemAffixInfo*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ClientItemAffixInfo*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_ClientAuctionManager
#define H_SCRIPT_HANDGameLogicLC_ClientAuctionManager

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientAuctionManager* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientAuctionManager*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ClientAuctionManager*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientAuctionManager*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ClientAuctionManager*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientAuctionManager& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_ClientAuctionManager* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_ClientAuctionManager*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_ClientAuctionManager*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ClientAuctionManager*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_ClientAuctionManager*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ClientAuctionManager*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_ClientAddItemInfo
#define H_SCRIPT_HANDGameLogicLC_ClientAddItemInfo

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientAddItemInfo* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientAddItemInfo*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ClientAddItemInfo*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientAddItemInfo*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ClientAddItemInfo*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientAddItemInfo& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_ClientAddItemInfo* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_ClientAddItemInfo*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_ClientAddItemInfo*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ClientAddItemInfo*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_ClientAddItemInfo*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ClientAddItemInfo*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_ClientAuctionBuyData
#define H_SCRIPT_HANDGameLogicLC_ClientAuctionBuyData

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientAuctionBuyData* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientAuctionBuyData*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ClientAuctionBuyData*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientAuctionBuyData*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ClientAuctionBuyData*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientAuctionBuyData& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_ClientAuctionBuyData* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_ClientAuctionBuyData*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_ClientAuctionBuyData*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ClientAuctionBuyData*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_ClientAuctionBuyData*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ClientAuctionBuyData*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_ClientAuctionSellData
#define H_SCRIPT_HANDGameLogicLC_ClientAuctionSellData

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientAuctionSellData* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientAuctionSellData*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ClientAuctionSellData*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientAuctionSellData*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ClientAuctionSellData*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientAuctionSellData& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_ClientAuctionSellData* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_ClientAuctionSellData*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_ClientAuctionSellData*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ClientAuctionSellData*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_ClientAuctionSellData*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ClientAuctionSellData*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_ClientGuildManager
#define H_SCRIPT_HANDGameLogicLC_ClientGuildManager

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientGuildManager* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientGuildManager*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ClientGuildManager*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientGuildManager*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ClientGuildManager*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientGuildManager& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_ClientGuildManager* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_ClientGuildManager*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_ClientGuildManager*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ClientGuildManager*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_ClientGuildManager*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ClientGuildManager*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_GuildAsset
#define H_SCRIPT_HANDGameLogicLC_GuildAsset

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_GuildAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_GuildAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_GuildAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_GuildAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_GuildAsset*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_GuildAsset& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_GuildAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_GuildAsset*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_GuildAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_GuildAsset*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_GuildAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_GuildAsset*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_GuildInfo
#define H_SCRIPT_HANDGameLogicLC_GuildInfo

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_GuildInfo* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_GuildInfo*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_GuildInfo*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_GuildInfo*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_GuildInfo*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_GuildInfo& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_GuildInfo* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_GuildInfo*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_GuildInfo*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_GuildInfo*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_GuildInfo*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_GuildInfo*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_ClientGuildMilestoneStruct
#define H_SCRIPT_HANDGameLogicLC_ClientGuildMilestoneStruct

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientGuildMilestoneStruct* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientGuildMilestoneStruct*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ClientGuildMilestoneStruct*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientGuildMilestoneStruct*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ClientGuildMilestoneStruct*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientGuildMilestoneStruct& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_ClientGuildMilestoneStruct* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_ClientGuildMilestoneStruct*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_ClientGuildMilestoneStruct*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ClientGuildMilestoneStruct*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_ClientGuildMilestoneStruct*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ClientGuildMilestoneStruct*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_ClientGuildSkillInfoStruct
#define H_SCRIPT_HANDGameLogicLC_ClientGuildSkillInfoStruct

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientGuildSkillInfoStruct* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientGuildSkillInfoStruct*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ClientGuildSkillInfoStruct*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientGuildSkillInfoStruct*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ClientGuildSkillInfoStruct*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientGuildSkillInfoStruct& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_ClientGuildSkillInfoStruct* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_ClientGuildSkillInfoStruct*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_ClientGuildSkillInfoStruct*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ClientGuildSkillInfoStruct*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_ClientGuildSkillInfoStruct*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ClientGuildSkillInfoStruct*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_ClientGuildSingleSkill
#define H_SCRIPT_HANDGameLogicLC_ClientGuildSingleSkill

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientGuildSingleSkill* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientGuildSingleSkill*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ClientGuildSingleSkill*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientGuildSingleSkill*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ClientGuildSingleSkill*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientGuildSingleSkill& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_ClientGuildSingleSkill* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_ClientGuildSingleSkill*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_ClientGuildSingleSkill*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ClientGuildSingleSkill*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_ClientGuildSingleSkill*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ClientGuildSingleSkill*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_GuildMemberEntry
#define H_SCRIPT_HANDGameLogicLC_GuildMemberEntry

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_GuildMemberEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_GuildMemberEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_GuildMemberEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_GuildMemberEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_GuildMemberEntry*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_GuildMemberEntry& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_GuildMemberEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_GuildMemberEntry*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_GuildMemberEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_GuildMemberEntry*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_GuildMemberEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_GuildMemberEntry*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_GuildMemberInfo
#define H_SCRIPT_HANDGameLogicLC_GuildMemberInfo

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_GuildMemberInfo* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_GuildMemberInfo*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_GuildMemberInfo*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_GuildMemberInfo*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_GuildMemberInfo*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_GuildMemberInfo& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_GuildMemberInfo* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_GuildMemberInfo*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_GuildMemberInfo*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_GuildMemberInfo*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_GuildMemberInfo*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_GuildMemberInfo*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_GuildApplyEntry
#define H_SCRIPT_HANDGameLogicLC_GuildApplyEntry

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_GuildApplyEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_GuildApplyEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_GuildApplyEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_GuildApplyEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_GuildApplyEntry*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_GuildApplyEntry& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_GuildApplyEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_GuildApplyEntry*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_GuildApplyEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_GuildApplyEntry*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_GuildApplyEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_GuildApplyEntry*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_GuildApplyInfo
#define H_SCRIPT_HANDGameLogicLC_GuildApplyInfo

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_GuildApplyInfo* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_GuildApplyInfo*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_GuildApplyInfo*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_GuildApplyInfo*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_GuildApplyInfo*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_GuildApplyInfo& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_GuildApplyInfo* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_GuildApplyInfo*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_GuildApplyInfo*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_GuildApplyInfo*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_GuildApplyInfo*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_GuildApplyInfo*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_GuildSearchInfo
#define H_SCRIPT_HANDGameLogicLC_GuildSearchInfo

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_GuildSearchInfo* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_GuildSearchInfo*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_GuildSearchInfo*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_GuildSearchInfo*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_GuildSearchInfo*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_GuildSearchInfo& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_GuildSearchInfo* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_GuildSearchInfo*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_GuildSearchInfo*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_GuildSearchInfo*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_GuildSearchInfo*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_GuildSearchInfo*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_TutorialEntry
#define H_SCRIPT_HANDGameLogicLC_TutorialEntry

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_TutorialEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_TutorialEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_TutorialEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_TutorialEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_TutorialEntry*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_TutorialEntry& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_TutorialEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_TutorialEntry*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_TutorialEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_TutorialEntry*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_TutorialEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_TutorialEntry*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_TutorialAsset
#define H_SCRIPT_HANDGameLogicLC_TutorialAsset

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_TutorialAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_TutorialAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_TutorialAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_TutorialAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_TutorialAsset*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_TutorialAsset& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_TutorialAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_TutorialAsset*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_TutorialAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_TutorialAsset*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_TutorialAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_TutorialAsset*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_GameEventBase
#define H_SCRIPT_HANDGameLogicLC_GameEventBase

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_GameEventBase* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_GameEventBase*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_GameEventBase*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_GameEventBase*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_GameEventBase*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_GameEventBase& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_GameEventBase* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_GameEventBase*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_GameEventBase*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_GameEventBase*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_GameEventBase*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_GameEventBase*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_GameEvent
#define H_SCRIPT_HANDGameLogicLC_GameEvent

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_GameEvent* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_GameEvent*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_GameEvent*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_GameEvent*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_GameEvent*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_GameEvent& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_GameEvent* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_GameEvent*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_GameEvent*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_GameEvent*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_GameEvent*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_GameEvent*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDLC_ClientGameEvent
#define H_SCRIPT_HANDLC_ClientGameEvent

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientGameEvent* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientGameEvent*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientGameEvent*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientGameEvent*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientGameEvent*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientGameEvent& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const LC_ClientGameEvent* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<LC_ClientGameEvent*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const LC_ClientGameEvent*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientGameEvent*	LuaGetValue(Utility::LuaTypeWrapper<const LC_ClientGameEvent*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientGameEvent*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_TaskScriptImportParam
#define H_SCRIPT_HANDGameLogicLC_TaskScriptImportParam

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_TaskScriptImportParam* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_TaskScriptImportParam*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_TaskScriptImportParam*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_TaskScriptImportParam*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_TaskScriptImportParam*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_TaskScriptImportParam& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_TaskScriptImportParam* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_TaskScriptImportParam*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_TaskScriptImportParam*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_TaskScriptImportParam*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_TaskScriptImportParam*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_TaskScriptImportParam*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDLC_ClientMapTransDataEntry
#define H_SCRIPT_HANDLC_ClientMapTransDataEntry

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientMapTransDataEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientMapTransDataEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientMapTransDataEntry*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientMapTransDataEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientMapTransDataEntry*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientMapTransDataEntry& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const LC_ClientMapTransDataEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<LC_ClientMapTransDataEntry*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const LC_ClientMapTransDataEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientMapTransDataEntry*	LuaGetValue(Utility::LuaTypeWrapper<const LC_ClientMapTransDataEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientMapTransDataEntry*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDLC_ClientSocietyAsset
#define H_SCRIPT_HANDLC_ClientSocietyAsset

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientSocietyAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientSocietyAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientSocietyAsset*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientSocietyAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientSocietyAsset*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientSocietyAsset& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const LC_ClientSocietyAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<LC_ClientSocietyAsset*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const LC_ClientSocietyAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientSocietyAsset*	LuaGetValue(Utility::LuaTypeWrapper<const LC_ClientSocietyAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientSocietyAsset*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_ClientWorldLevelAsset
#define H_SCRIPT_HANDGameLogicLC_ClientWorldLevelAsset

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientWorldLevelAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientWorldLevelAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ClientWorldLevelAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientWorldLevelAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ClientWorldLevelAsset*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientWorldLevelAsset& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_ClientWorldLevelAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_ClientWorldLevelAsset*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_ClientWorldLevelAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ClientWorldLevelAsset*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_ClientWorldLevelAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ClientWorldLevelAsset*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_SocietyBasicInfo
#define H_SCRIPT_HANDGameLogicLC_SocietyBasicInfo

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_SocietyBasicInfo* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_SocietyBasicInfo*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_SocietyBasicInfo*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_SocietyBasicInfo*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_SocietyBasicInfo*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_SocietyBasicInfo& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_SocietyBasicInfo* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_SocietyBasicInfo*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_SocietyBasicInfo*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_SocietyBasicInfo*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_SocietyBasicInfo*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_SocietyBasicInfo*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_FriendDetailInfo
#define H_SCRIPT_HANDGameLogicLC_FriendDetailInfo

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_FriendDetailInfo* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_FriendDetailInfo*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_FriendDetailInfo*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_FriendDetailInfo*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_FriendDetailInfo*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_FriendDetailInfo& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_FriendDetailInfo* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_FriendDetailInfo*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_FriendDetailInfo*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_FriendDetailInfo*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_FriendDetailInfo*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_FriendDetailInfo*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDLC_ClientFriendMsgBroadStruct
#define H_SCRIPT_HANDLC_ClientFriendMsgBroadStruct

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientFriendMsgBroadStruct* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientFriendMsgBroadStruct*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientFriendMsgBroadStruct*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientFriendMsgBroadStruct*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientFriendMsgBroadStruct*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientFriendMsgBroadStruct& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const LC_ClientFriendMsgBroadStruct* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<LC_ClientFriendMsgBroadStruct*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const LC_ClientFriendMsgBroadStruct*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientFriendMsgBroadStruct*	LuaGetValue(Utility::LuaTypeWrapper<const LC_ClientFriendMsgBroadStruct*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientFriendMsgBroadStruct*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDLC_ClientFriendMsgBroad
#define H_SCRIPT_HANDLC_ClientFriendMsgBroad

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientFriendMsgBroad* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientFriendMsgBroad*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientFriendMsgBroad*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientFriendMsgBroad*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientFriendMsgBroad*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientFriendMsgBroad& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const LC_ClientFriendMsgBroad* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<LC_ClientFriendMsgBroad*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const LC_ClientFriendMsgBroad*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientFriendMsgBroad*	LuaGetValue(Utility::LuaTypeWrapper<const LC_ClientFriendMsgBroad*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientFriendMsgBroad*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_ClientViewPlayerInfo
#define H_SCRIPT_HANDGameLogicLC_ClientViewPlayerInfo

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientViewPlayerInfo* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientViewPlayerInfo*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ClientViewPlayerInfo*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientViewPlayerInfo*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ClientViewPlayerInfo*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientViewPlayerInfo& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_ClientViewPlayerInfo* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_ClientViewPlayerInfo*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_ClientViewPlayerInfo*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ClientViewPlayerInfo*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_ClientViewPlayerInfo*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ClientViewPlayerInfo*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDLC_ClientFriendNick
#define H_SCRIPT_HANDLC_ClientFriendNick

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientFriendNick* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientFriendNick*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientFriendNick*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientFriendNick*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientFriendNick*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientFriendNick& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const LC_ClientFriendNick* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<LC_ClientFriendNick*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const LC_ClientFriendNick*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientFriendNick*	LuaGetValue(Utility::LuaTypeWrapper<const LC_ClientFriendNick*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientFriendNick*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDLC_FriendNick
#define H_SCRIPT_HANDLC_FriendNick

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_FriendNick* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_FriendNick*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_FriendNick*	LuaGetValue(Utility::LuaTypeWrapper<LC_FriendNick*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_FriendNick*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_FriendNick& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const LC_FriendNick* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<LC_FriendNick*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const LC_FriendNick*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_FriendNick*	LuaGetValue(Utility::LuaTypeWrapper<const LC_FriendNick*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_FriendNick*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDTT_TextElement
#define H_SCRIPT_HANDTT_TextElement

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, TT_TextElement* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<TT_TextElement*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline TT_TextElement*	LuaGetValue(Utility::LuaTypeWrapper<TT_TextElement*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<TT_TextElement*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, TT_TextElement& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const TT_TextElement* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<TT_TextElement*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const TT_TextElement*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline TT_TextElement*	LuaGetValue(Utility::LuaTypeWrapper<const TT_TextElement*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<TT_TextElement*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDLC_ShortcutKeyAssetEntry
#define H_SCRIPT_HANDLC_ShortcutKeyAssetEntry

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ShortcutKeyAssetEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ShortcutKeyAssetEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ShortcutKeyAssetEntry*	LuaGetValue(Utility::LuaTypeWrapper<LC_ShortcutKeyAssetEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ShortcutKeyAssetEntry*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ShortcutKeyAssetEntry& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const LC_ShortcutKeyAssetEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<LC_ShortcutKeyAssetEntry*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const LC_ShortcutKeyAssetEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ShortcutKeyAssetEntry*	LuaGetValue(Utility::LuaTypeWrapper<const LC_ShortcutKeyAssetEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ShortcutKeyAssetEntry*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDLC_ShortcutKeyAsset
#define H_SCRIPT_HANDLC_ShortcutKeyAsset

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ShortcutKeyAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ShortcutKeyAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ShortcutKeyAsset*	LuaGetValue(Utility::LuaTypeWrapper<LC_ShortcutKeyAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ShortcutKeyAsset*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ShortcutKeyAsset& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const LC_ShortcutKeyAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<LC_ShortcutKeyAsset*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const LC_ShortcutKeyAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ShortcutKeyAsset*	LuaGetValue(Utility::LuaTypeWrapper<const LC_ShortcutKeyAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ShortcutKeyAsset*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDLC_ClientArenaPlayerDataEntry
#define H_SCRIPT_HANDLC_ClientArenaPlayerDataEntry

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientArenaPlayerDataEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientArenaPlayerDataEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientArenaPlayerDataEntry*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientArenaPlayerDataEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientArenaPlayerDataEntry*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientArenaPlayerDataEntry& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const LC_ClientArenaPlayerDataEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<LC_ClientArenaPlayerDataEntry*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const LC_ClientArenaPlayerDataEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientArenaPlayerDataEntry*	LuaGetValue(Utility::LuaTypeWrapper<const LC_ClientArenaPlayerDataEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientArenaPlayerDataEntry*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDLC_ClientArenaManager
#define H_SCRIPT_HANDLC_ClientArenaManager

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientArenaManager* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientArenaManager*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientArenaManager*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientArenaManager*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientArenaManager*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientArenaManager& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const LC_ClientArenaManager* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<LC_ClientArenaManager*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const LC_ClientArenaManager*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientArenaManager*	LuaGetValue(Utility::LuaTypeWrapper<const LC_ClientArenaManager*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientArenaManager*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_ArenaEntry
#define H_SCRIPT_HANDGameLogicLC_ArenaEntry

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ArenaEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ArenaEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ArenaEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ArenaEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ArenaEntry*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ArenaEntry& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_ArenaEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_ArenaEntry*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_ArenaEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ArenaEntry*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_ArenaEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ArenaEntry*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_ArenaAsset
#define H_SCRIPT_HANDGameLogicLC_ArenaAsset

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ArenaAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ArenaAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ArenaAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ArenaAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ArenaAsset*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ArenaAsset& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_ArenaAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_ArenaAsset*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_ArenaAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ArenaAsset*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_ArenaAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ArenaAsset*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_VIPAsset
#define H_SCRIPT_HANDGameLogicLC_VIPAsset

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_VIPAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_VIPAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_VIPAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_VIPAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_VIPAsset*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_VIPAsset& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_VIPAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_VIPAsset*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_VIPAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_VIPAsset*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_VIPAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_VIPAsset*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_VIPFDepositEntry
#define H_SCRIPT_HANDGameLogicLC_VIPFDepositEntry

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_VIPFDepositEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_VIPFDepositEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_VIPFDepositEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_VIPFDepositEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_VIPFDepositEntry*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_VIPFDepositEntry& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_VIPFDepositEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_VIPFDepositEntry*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_VIPFDepositEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_VIPFDepositEntry*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_VIPFDepositEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_VIPFDepositEntry*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_SignInAssert
#define H_SCRIPT_HANDGameLogicLC_SignInAssert

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_SignInAssert* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_SignInAssert*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_SignInAssert*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_SignInAssert*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_SignInAssert*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_SignInAssert& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_SignInAssert* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_SignInAssert*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_SignInAssert*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_SignInAssert*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_SignInAssert*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_SignInAssert*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_RaidAssert
#define H_SCRIPT_HANDGameLogicLC_RaidAssert

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_RaidAssert* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_RaidAssert*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_RaidAssert*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_RaidAssert*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_RaidAssert*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_RaidAssert& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_RaidAssert* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_RaidAssert*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_RaidAssert*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_RaidAssert*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_RaidAssert*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_RaidAssert*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_PromoterAsset
#define H_SCRIPT_HANDGameLogicLC_PromoterAsset

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_PromoterAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_PromoterAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_PromoterAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_PromoterAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_PromoterAsset*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_PromoterAsset& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_PromoterAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_PromoterAsset*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_PromoterAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_PromoterAsset*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_PromoterAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_PromoterAsset*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_OpenAbilityAsset
#define H_SCRIPT_HANDGameLogicLC_OpenAbilityAsset

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_OpenAbilityAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_OpenAbilityAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_OpenAbilityAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_OpenAbilityAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_OpenAbilityAsset*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_OpenAbilityAsset& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_OpenAbilityAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_OpenAbilityAsset*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_OpenAbilityAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_OpenAbilityAsset*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_OpenAbilityAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_OpenAbilityAsset*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_SubordinateAsset
#define H_SCRIPT_HANDGameLogicLC_SubordinateAsset

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_SubordinateAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_SubordinateAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_SubordinateAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_SubordinateAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_SubordinateAsset*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_SubordinateAsset& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_SubordinateAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_SubordinateAsset*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_SubordinateAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_SubordinateAsset*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_SubordinateAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_SubordinateAsset*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_MeltAsset
#define H_SCRIPT_HANDGameLogicLC_MeltAsset

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_MeltAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_MeltAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_MeltAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_MeltAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_MeltAsset*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_MeltAsset& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_MeltAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_MeltAsset*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_MeltAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_MeltAsset*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_MeltAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_MeltAsset*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_BoneSoulAsset
#define H_SCRIPT_HANDGameLogicLC_BoneSoulAsset

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_BoneSoulAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_BoneSoulAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_BoneSoulAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_BoneSoulAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_BoneSoulAsset*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_BoneSoulAsset& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_BoneSoulAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_BoneSoulAsset*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_BoneSoulAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_BoneSoulAsset*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_BoneSoulAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_BoneSoulAsset*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_StarsRuneAsset
#define H_SCRIPT_HANDGameLogicLC_StarsRuneAsset

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_StarsRuneAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_StarsRuneAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_StarsRuneAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_StarsRuneAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_StarsRuneAsset*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_StarsRuneAsset& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_StarsRuneAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_StarsRuneAsset*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_StarsRuneAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_StarsRuneAsset*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_StarsRuneAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_StarsRuneAsset*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_CharPromoteDataEntry
#define H_SCRIPT_HANDGameLogicLC_CharPromoteDataEntry

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_CharPromoteDataEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_CharPromoteDataEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_CharPromoteDataEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_CharPromoteDataEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_CharPromoteDataEntry*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_CharPromoteDataEntry& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_CharPromoteDataEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_CharPromoteDataEntry*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_CharPromoteDataEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_CharPromoteDataEntry*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_CharPromoteDataEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_CharPromoteDataEntry*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_CommonStatRewardDataEntry
#define H_SCRIPT_HANDGameLogicLC_CommonStatRewardDataEntry

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_CommonStatRewardDataEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_CommonStatRewardDataEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_CommonStatRewardDataEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_CommonStatRewardDataEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_CommonStatRewardDataEntry*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_CommonStatRewardDataEntry& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_CommonStatRewardDataEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_CommonStatRewardDataEntry*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_CommonStatRewardDataEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_CommonStatRewardDataEntry*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_CommonStatRewardDataEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_CommonStatRewardDataEntry*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_ActorOperationEntry
#define H_SCRIPT_HANDGameLogicLC_ActorOperationEntry

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ActorOperationEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ActorOperationEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ActorOperationEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ActorOperationEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ActorOperationEntry*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ActorOperationEntry& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_ActorOperationEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_ActorOperationEntry*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_ActorOperationEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ActorOperationEntry*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_ActorOperationEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ActorOperationEntry*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicGuildWareHousePack
#define H_SCRIPT_HANDGameLogicGuildWareHousePack

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::GuildWareHousePack* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::GuildWareHousePack*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::GuildWareHousePack*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::GuildWareHousePack*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::GuildWareHousePack*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::GuildWareHousePack& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::GuildWareHousePack* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::GuildWareHousePack*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::GuildWareHousePack*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::GuildWareHousePack*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::GuildWareHousePack*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::GuildWareHousePack*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_ClientGuildHomeMgr
#define H_SCRIPT_HANDGameLogicLC_ClientGuildHomeMgr

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientGuildHomeMgr* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientGuildHomeMgr*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ClientGuildHomeMgr*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientGuildHomeMgr*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ClientGuildHomeMgr*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientGuildHomeMgr& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_ClientGuildHomeMgr* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_ClientGuildHomeMgr*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_ClientGuildHomeMgr*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ClientGuildHomeMgr*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_ClientGuildHomeMgr*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ClientGuildHomeMgr*>(*ubox);
		}
	}

//#ifndef H_SCRIPT_HANDGameLogicLC_SafeSystemAsset
//#define H_SCRIPT_HANDGameLogicLC_SafeSystemAsset
//
//	namespace Utility
//	{
//		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_SafeSystemAsset* value)
//		{
//			if(value != NULL)
//			{
//				LuaBinder binder(L);
//				binder.PushUserType(value , value->GetRTTIName());
//			}
//			else
//			{
//				lua_pushnil(L);
//			}
//		}
//		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_SafeSystemAsset*>, lua_State* L, int idx)
//		{
//			int ntype = lua_type(L, idx);
//			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
//		}
//		inline GameLogic::LC_SafeSystemAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_SafeSystemAsset*>, lua_State* L, int idx)
//		{
//			void** ubox = (void**)lua_touserdata(L, idx);
//			T_ASSERT(ubox && *ubox);
//			return static_cast<GameLogic::LC_SafeSystemAsset*>(*ubox);
//		}
//	}
//#endif
//	namespace Utility
//	{
//		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_SafeSystemAsset& value)
//		{
//			LuaBinder binder(L);
//			binder.PushUserType(&value, value.GetRTTIName());
//		}
//		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_SafeSystemAsset* value)
//		{
//			if(value != NULL)
//			{
//				LuaBinder binder(L);
//				binder.PushUserType(const_cast<GameLogic::LC_SafeSystemAsset*>(value) , value->GetRTTIName());
//			}
//			else
//			{
//				lua_pushnil(L);
//			}
//		}
//		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_SafeSystemAsset*>, lua_State* L, int idx)
//		{
//			int ntype = lua_type(L, idx);
//			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
//		}
//		inline GameLogic::LC_SafeSystemAsset*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_SafeSystemAsset*>, lua_State* L, int idx)
//		{
//			void** ubox = (void**)lua_touserdata(L, idx);
//			T_ASSERT(ubox && *ubox);
//			return static_cast<GameLogic::LC_SafeSystemAsset*>(*ubox);
//		}
//	}

#ifndef H_SCRIPT_HANDUI_UIManager
#define H_SCRIPT_HANDUI_UIManager

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, UI_UIManager* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<UI_UIManager*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline UI_UIManager*	LuaGetValue(Utility::LuaTypeWrapper<UI_UIManager*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<UI_UIManager*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, UI_UIManager& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const UI_UIManager* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<UI_UIManager*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const UI_UIManager*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline UI_UIManager*	LuaGetValue(Utility::LuaTypeWrapper<const UI_UIManager*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<UI_UIManager*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDLC_ClientArenaBattleChallengeInfoAsset
#define H_SCRIPT_HANDLC_ClientArenaBattleChallengeInfoAsset

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientArenaBattleChallengeInfoAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientArenaBattleChallengeInfoAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientArenaBattleChallengeInfoAsset*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientArenaBattleChallengeInfoAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientArenaBattleChallengeInfoAsset*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientArenaBattleChallengeInfoAsset& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const LC_ClientArenaBattleChallengeInfoAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<LC_ClientArenaBattleChallengeInfoAsset*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const LC_ClientArenaBattleChallengeInfoAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientArenaBattleChallengeInfoAsset*	LuaGetValue(Utility::LuaTypeWrapper<const LC_ClientArenaBattleChallengeInfoAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientArenaBattleChallengeInfoAsset*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDLC_ClientGameEventUserDataBase
#define H_SCRIPT_HANDLC_ClientGameEventUserDataBase

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientGameEventUserDataBase* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientGameEventUserDataBase*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientGameEventUserDataBase*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientGameEventUserDataBase*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientGameEventUserDataBase*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientGameEventUserDataBase& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const LC_ClientGameEventUserDataBase* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<LC_ClientGameEventUserDataBase*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const LC_ClientGameEventUserDataBase*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientGameEventUserDataBase*	LuaGetValue(Utility::LuaTypeWrapper<const LC_ClientGameEventUserDataBase*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientGameEventUserDataBase*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDLC_ClientGameEventUserData_RaidSettle
#define H_SCRIPT_HANDLC_ClientGameEventUserData_RaidSettle

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientGameEventUserData_RaidSettle* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientGameEventUserData_RaidSettle*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientGameEventUserData_RaidSettle*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientGameEventUserData_RaidSettle*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientGameEventUserData_RaidSettle*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientGameEventUserData_RaidSettle& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const LC_ClientGameEventUserData_RaidSettle* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<LC_ClientGameEventUserData_RaidSettle*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const LC_ClientGameEventUserData_RaidSettle*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientGameEventUserData_RaidSettle*	LuaGetValue(Utility::LuaTypeWrapper<const LC_ClientGameEventUserData_RaidSettle*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientGameEventUserData_RaidSettle*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDLC_ClientGameEventUserData_String
#define H_SCRIPT_HANDLC_ClientGameEventUserData_String

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientGameEventUserData_String* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientGameEventUserData_String*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientGameEventUserData_String*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientGameEventUserData_String*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientGameEventUserData_String*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientGameEventUserData_String& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const LC_ClientGameEventUserData_String* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<LC_ClientGameEventUserData_String*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const LC_ClientGameEventUserData_String*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientGameEventUserData_String*	LuaGetValue(Utility::LuaTypeWrapper<const LC_ClientGameEventUserData_String*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientGameEventUserData_String*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDLC_ClientRankUserInfo
#define H_SCRIPT_HANDLC_ClientRankUserInfo

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientRankUserInfo* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientRankUserInfo*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientRankUserInfo*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientRankUserInfo*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientRankUserInfo*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientRankUserInfo& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const LC_ClientRankUserInfo* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<LC_ClientRankUserInfo*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const LC_ClientRankUserInfo*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientRankUserInfo*	LuaGetValue(Utility::LuaTypeWrapper<const LC_ClientRankUserInfo*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientRankUserInfo*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDLC_ClientGameEventUserData_ArenaBattle_BattleInfo
#define H_SCRIPT_HANDLC_ClientGameEventUserData_ArenaBattle_BattleInfo

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientGameEventUserData_ArenaBattle_BattleInfo* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientGameEventUserData_ArenaBattle_BattleInfo*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientGameEventUserData_ArenaBattle_BattleInfo*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientGameEventUserData_ArenaBattle_BattleInfo*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientGameEventUserData_ArenaBattle_BattleInfo*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientGameEventUserData_ArenaBattle_BattleInfo& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const LC_ClientGameEventUserData_ArenaBattle_BattleInfo* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<LC_ClientGameEventUserData_ArenaBattle_BattleInfo*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const LC_ClientGameEventUserData_ArenaBattle_BattleInfo*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientGameEventUserData_ArenaBattle_BattleInfo*	LuaGetValue(Utility::LuaTypeWrapper<const LC_ClientGameEventUserData_ArenaBattle_BattleInfo*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientGameEventUserData_ArenaBattle_BattleInfo*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDLC_ClientGameEventUserData_ArenaSettleInfo
#define H_SCRIPT_HANDLC_ClientGameEventUserData_ArenaSettleInfo

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientGameEventUserData_ArenaSettleInfo* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientGameEventUserData_ArenaSettleInfo*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientGameEventUserData_ArenaSettleInfo*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientGameEventUserData_ArenaSettleInfo*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientGameEventUserData_ArenaSettleInfo*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientGameEventUserData_ArenaSettleInfo& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const LC_ClientGameEventUserData_ArenaSettleInfo* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<LC_ClientGameEventUserData_ArenaSettleInfo*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const LC_ClientGameEventUserData_ArenaSettleInfo*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientGameEventUserData_ArenaSettleInfo*	LuaGetValue(Utility::LuaTypeWrapper<const LC_ClientGameEventUserData_ArenaSettleInfo*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientGameEventUserData_ArenaSettleInfo*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDLC_ClientGameEventUserData_Common
#define H_SCRIPT_HANDLC_ClientGameEventUserData_Common

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientGameEventUserData_Common* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientGameEventUserData_Common*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientGameEventUserData_Common*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientGameEventUserData_Common*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientGameEventUserData_Common*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_ClientGameEventUserData_Common& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const LC_ClientGameEventUserData_Common* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<LC_ClientGameEventUserData_Common*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const LC_ClientGameEventUserData_Common*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_ClientGameEventUserData_Common*	LuaGetValue(Utility::LuaTypeWrapper<const LC_ClientGameEventUserData_Common*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_ClientGameEventUserData_Common*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_CommonStatAsset
#define H_SCRIPT_HANDGameLogicLC_CommonStatAsset

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_CommonStatAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_CommonStatAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_CommonStatAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_CommonStatAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_CommonStatAsset*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_CommonStatAsset& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_CommonStatAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_CommonStatAsset*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_CommonStatAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_CommonStatAsset*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_CommonStatAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_CommonStatAsset*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_CommonStatEntry
#define H_SCRIPT_HANDGameLogicLC_CommonStatEntry

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_CommonStatEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_CommonStatEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_CommonStatEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_CommonStatEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_CommonStatEntry*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_CommonStatEntry& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_CommonStatEntry* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_CommonStatEntry*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_CommonStatEntry*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_CommonStatEntry*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_CommonStatEntry*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_CommonStatEntry*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_TreasureChestsAsset
#define H_SCRIPT_HANDGameLogicLC_TreasureChestsAsset

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_TreasureChestsAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_TreasureChestsAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_TreasureChestsAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_TreasureChestsAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_TreasureChestsAsset*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_TreasureChestsAsset& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_TreasureChestsAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_TreasureChestsAsset*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_TreasureChestsAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_TreasureChestsAsset*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_TreasureChestsAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_TreasureChestsAsset*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDACT_SecretTreasureAsset
#define H_SCRIPT_HANDACT_SecretTreasureAsset

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, ACT_SecretTreasureAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<ACT_SecretTreasureAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline ACT_SecretTreasureAsset*	LuaGetValue(Utility::LuaTypeWrapper<ACT_SecretTreasureAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<ACT_SecretTreasureAsset*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, ACT_SecretTreasureAsset& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const ACT_SecretTreasureAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<ACT_SecretTreasureAsset*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const ACT_SecretTreasureAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline ACT_SecretTreasureAsset*	LuaGetValue(Utility::LuaTypeWrapper<const ACT_SecretTreasureAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<ACT_SecretTreasureAsset*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDLC_SkillBookAsset
#define H_SCRIPT_HANDLC_SkillBookAsset

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_SkillBookAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_SkillBookAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_SkillBookAsset*	LuaGetValue(Utility::LuaTypeWrapper<LC_SkillBookAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_SkillBookAsset*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, LC_SkillBookAsset& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const LC_SkillBookAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<LC_SkillBookAsset*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const LC_SkillBookAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline LC_SkillBookAsset*	LuaGetValue(Utility::LuaTypeWrapper<const LC_SkillBookAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<LC_SkillBookAsset*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_CommonStatManager
#define H_SCRIPT_HANDGameLogicLC_CommonStatManager

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_CommonStatManager* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_CommonStatManager*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_CommonStatManager*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_CommonStatManager*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_CommonStatManager*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_CommonStatManager& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_CommonStatManager* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_CommonStatManager*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_CommonStatManager*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_CommonStatManager*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_CommonStatManager*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_CommonStatManager*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_ClientSoulRefineManager
#define H_SCRIPT_HANDGameLogicLC_ClientSoulRefineManager

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientSoulRefineManager* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientSoulRefineManager*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ClientSoulRefineManager*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientSoulRefineManager*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ClientSoulRefineManager*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientSoulRefineManager& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_ClientSoulRefineManager* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_ClientSoulRefineManager*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_ClientSoulRefineManager*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ClientSoulRefineManager*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_ClientSoulRefineManager*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ClientSoulRefineManager*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_ClientSoulRefineAsset
#define H_SCRIPT_HANDGameLogicLC_ClientSoulRefineAsset

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientSoulRefineAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientSoulRefineAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ClientSoulRefineAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientSoulRefineAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ClientSoulRefineAsset*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientSoulRefineAsset& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_ClientSoulRefineAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_ClientSoulRefineAsset*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_ClientSoulRefineAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ClientSoulRefineAsset*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_ClientSoulRefineAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ClientSoulRefineAsset*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_ClientShotActivityManager
#define H_SCRIPT_HANDGameLogicLC_ClientShotActivityManager

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientShotActivityManager* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientShotActivityManager*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ClientShotActivityManager*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientShotActivityManager*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ClientShotActivityManager*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientShotActivityManager& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_ClientShotActivityManager* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_ClientShotActivityManager*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_ClientShotActivityManager*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ClientShotActivityManager*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_ClientShotActivityManager*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ClientShotActivityManager*>(*ubox);
		}
	}


#ifndef H_SCRIPT_HANDGameLogicLC_FashionBagAsset
#define H_SCRIPT_HANDGameLogicLC_FashionBagAsset

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_FashionBagAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_FashionBagAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_FashionBagAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_FashionBagAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_FashionBagAsset*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_FashionBagAsset& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_FashionBagAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_FashionBagAsset*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_FashionBagAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_FashionBagAsset*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_FashionBagAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_FashionBagAsset*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_ClientRumorManager
#define H_SCRIPT_HANDGameLogicLC_ClientRumorManager

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientRumorManager* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientRumorManager*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ClientRumorManager*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientRumorManager*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ClientRumorManager*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientRumorManager& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_ClientRumorManager* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_ClientRumorManager*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_ClientRumorManager*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ClientRumorManager*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_ClientRumorManager*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ClientRumorManager*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_ClientActivityManager
#define H_SCRIPT_HANDGameLogicLC_ClientActivityManager

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientActivityManager* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientActivityManager*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ClientActivityManager*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientActivityManager*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ClientActivityManager*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientActivityManager& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_ClientActivityManager* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_ClientActivityManager*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_ClientActivityManager*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ClientActivityManager*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_ClientActivityManager*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ClientActivityManager*>(*ubox);
		}
	}

#ifndef H_SCRIPT_HANDGameLogicLC_ClientActivityAsset
#define H_SCRIPT_HANDGameLogicLC_ClientActivityAsset

	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientActivityAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(value , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientActivityAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ClientActivityAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientActivityAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ClientActivityAsset*>(*ubox);
		}
	}
#endif
	namespace Utility
	{
		inline	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientActivityAsset& value)
		{
			LuaBinder binder(L);
			binder.PushUserType(&value, value.GetRTTIName());
		}
		inline	void	LuaPushValue(lua_State* L, const GameLogic::LC_ClientActivityAsset* value)
		{
			if(value != NULL)
			{
				LuaBinder binder(L);
				binder.PushUserType(const_cast<GameLogic::LC_ClientActivityAsset*>(value) , value->GetRTTIName());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		inline	bool	LuaTypeCheck(Utility::LuaTypeWrapper<const GameLogic::LC_ClientActivityAsset*>, lua_State* L, int idx)
		{
			int ntype = lua_type(L, idx);
			return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);
		}
		inline GameLogic::LC_ClientActivityAsset*	LuaGetValue(Utility::LuaTypeWrapper<const GameLogic::LC_ClientActivityAsset*>, lua_State* L, int idx)
		{
			void** ubox = (void**)lua_touserdata(L, idx);
			T_ASSERT(ubox && *ubox);
			return static_cast<GameLogic::LC_ClientActivityAsset*>(*ubox);
		}
	}

