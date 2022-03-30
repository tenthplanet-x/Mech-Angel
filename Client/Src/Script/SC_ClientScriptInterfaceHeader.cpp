
namespace Utility
{
	
#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_LogicObject
	void	LuaPushValue(lua_State* L, GameLogic::LC_LogicObject* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_LogicObject*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_LogicObject*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_LogicObject*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_LogicObject*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDUtilityUT_Vec2Int
	void	LuaPushValue(lua_State* L, Utility::UT_Vec2Int* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<Utility::UT_Vec2Int*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	Utility::UT_Vec2Int*	LuaGetValue(Utility::LuaTypeWrapper<Utility::UT_Vec2Int*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<Utility::UT_Vec2Int*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDUtilityUT_Vec3Int
	void	LuaPushValue(lua_State* L, Utility::UT_Vec3Int* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<Utility::UT_Vec3Int*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	Utility::UT_Vec3Int*	LuaGetValue(Utility::LuaTypeWrapper<Utility::UT_Vec3Int*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<Utility::UT_Vec3Int*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDUtilityUT_Vec4Int
	void	LuaPushValue(lua_State* L, Utility::UT_Vec4Int* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<Utility::UT_Vec4Int*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	Utility::UT_Vec4Int*	LuaGetValue(Utility::LuaTypeWrapper<Utility::UT_Vec4Int*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<Utility::UT_Vec4Int*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_Attacker
	void	LuaPushValue(lua_State* L, GameLogic::LC_Attacker* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_Attacker*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_Attacker*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_Attacker*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_Attacker*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDSkillSK_SkillEvent
	void	LuaPushValue(lua_State* L, Skill::SK_SkillEvent* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<Skill::SK_SkillEvent*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	Skill::SK_SkillEvent*	LuaGetValue(Utility::LuaTypeWrapper<Skill::SK_SkillEvent*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<Skill::SK_SkillEvent*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ActorBase
	void	LuaPushValue(lua_State* L, GameLogic::LC_ActorBase* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ActorBase*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_ActorBase*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ActorBase*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_ActorBase*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_PlayerBase
	void	LuaPushValue(lua_State* L, GameLogic::LC_PlayerBase* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_PlayerBase*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_PlayerBase*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_PlayerBase*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_PlayerBase*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_ClientNetPlayer
	void	LuaPushValue(lua_State* L, LC_ClientNetPlayer* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientNetPlayer*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_ClientNetPlayer*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientNetPlayer*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_ClientNetPlayer*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_NPCBase
	void	LuaPushValue(lua_State* L, GameLogic::LC_NPCBase* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_NPCBase*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_NPCBase*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_NPCBase*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_NPCBase*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_ClientNPC
	void	LuaPushValue(lua_State* L, LC_ClientNPC* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientNPC*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_ClientNPC*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientNPC*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_ClientNPC*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_ClientGamePlayer
	void	LuaPushValue(lua_State* L, LC_ClientGamePlayer* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientGamePlayer*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_ClientGamePlayer*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientGamePlayer*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_ClientGamePlayer*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_ClientGameStoryManager
	void	LuaPushValue(lua_State* L, LC_ClientGameStoryManager* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientGameStoryManager*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_ClientGameStoryManager*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientGameStoryManager*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_ClientGameStoryManager*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_ClientOpenAbilityManager
	void	LuaPushValue(lua_State* L, LC_ClientOpenAbilityManager* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientOpenAbilityManager*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_ClientOpenAbilityManager*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientOpenAbilityManager*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_ClientOpenAbilityManager*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_ClientTranscriptionInfoManager
	void	LuaPushValue(lua_State* L, LC_ClientTranscriptionInfoManager* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientTranscriptionInfoManager*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_ClientTranscriptionInfoManager*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientTranscriptionInfoManager*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_ClientTranscriptionInfoManager*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameStoryRaidInfo
	void	LuaPushValue(lua_State* L, GameStoryRaidInfo* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameStoryRaidInfo*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameStoryRaidInfo*	LuaGetValue(Utility::LuaTypeWrapper<GameStoryRaidInfo*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameStoryRaidInfo*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameStoryInfo
	void	LuaPushValue(lua_State* L, GameStoryInfo* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameStoryInfo*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameStoryInfo*	LuaGetValue(Utility::LuaTypeWrapper<GameStoryInfo*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameStoryInfo*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ActorAttributeMap
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

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ActorAttributeAppendMap
	void	LuaPushValue(lua_State* L, GameLogic::LC_ActorAttributeAppendMap* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ActorAttributeAppendMap*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_ActorAttributeAppendMap*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ActorAttributeAppendMap*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_ActorAttributeAppendMap*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_BackPackEntry
	void	LuaPushValue(lua_State* L, GameLogic::LC_BackPackEntry* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_BackPackEntry*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_BackPackEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_BackPackEntry*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_BackPackEntry*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_PackAsset
	void	LuaPushValue(lua_State* L, GameLogic::LC_PackAsset* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_PackAsset*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_PackAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_PackAsset*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_PackAsset*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_GreatKungFuAsset
	void	LuaPushValue(lua_State* L, GameLogic::LC_GreatKungFuAsset* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_GreatKungFuAsset*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_GreatKungFuAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_GreatKungFuAsset*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_GreatKungFuAsset*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ClientAlliance
	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientAlliance* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientAlliance*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_ClientAlliance*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientAlliance*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_ClientAlliance*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ItemDescriptionEntry
	void	LuaPushValue(lua_State* L, GameLogic::LC_ItemDescriptionEntry* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ItemDescriptionEntry*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_ItemDescriptionEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ItemDescriptionEntry*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_ItemDescriptionEntry*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_CrossRealmTeamAsset
	void	LuaPushValue(lua_State* L, GameLogic::LC_CrossRealmTeamAsset* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_CrossRealmTeamAsset*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_CrossRealmTeamAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_CrossRealmTeamAsset*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_CrossRealmTeamAsset*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ItemAppendAttributeFormulaEntry
	void	LuaPushValue(lua_State* L, GameLogic::LC_ItemAppendAttributeFormulaEntry* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ItemAppendAttributeFormulaEntry*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_ItemAppendAttributeFormulaEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ItemAppendAttributeFormulaEntry*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_ItemAppendAttributeFormulaEntry*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ItemUIContentEntry
	void	LuaPushValue(lua_State* L, GameLogic::LC_ItemUIContentEntry* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ItemUIContentEntry*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_ItemUIContentEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ItemUIContentEntry*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_ItemUIContentEntry*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ItemFactoryBase
	void	LuaPushValue(lua_State* L, GameLogic::LC_ItemFactoryBase* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ItemFactoryBase*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_ItemFactoryBase*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ItemFactoryBase*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_ItemFactoryBase*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ItemOperationDataEntry
	void	LuaPushValue(lua_State* L, GameLogic::LC_ItemOperationDataEntry* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ItemOperationDataEntry*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_ItemOperationDataEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ItemOperationDataEntry*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_ItemOperationDataEntry*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_Task_Condition
	void	LuaPushValue(lua_State* L, GameLogic::LC_Task_Condition* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_Task_Condition*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_Task_Condition*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_Task_Condition*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_Task_Condition*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_TaskManager
	void	LuaPushValue(lua_State* L, GameLogic::LC_TaskManager* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_TaskManager*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_TaskManager*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_TaskManager*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_TaskManager*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_ClientTaskManager
	void	LuaPushValue(lua_State* L, LC_ClientTaskManager* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientTaskManager*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_ClientTaskManager*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientTaskManager*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_ClientTaskManager*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ProcessingTaskEntry
	void	LuaPushValue(lua_State* L, GameLogic::LC_ProcessingTaskEntry* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ProcessingTaskEntry*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_ProcessingTaskEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ProcessingTaskEntry*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_ProcessingTaskEntry*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_CanStartTaskMapEntry
	void	LuaPushValue(lua_State* L, LC_CanStartTaskMapEntry* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_CanStartTaskMapEntry*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_CanStartTaskMapEntry*	LuaGetValue(Utility::LuaTypeWrapper<LC_CanStartTaskMapEntry*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_CanStartTaskMapEntry*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_FinishedTaskRecord
	void	LuaPushValue(lua_State* L, GameLogic::LC_FinishedTaskRecord* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_FinishedTaskRecord*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_FinishedTaskRecord*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_FinishedTaskRecord*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_FinishedTaskRecord*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_MainTaskDescriptEntry
	void	LuaPushValue(lua_State* L, GameLogic::LC_MainTaskDescriptEntry* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_MainTaskDescriptEntry*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_MainTaskDescriptEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_MainTaskDescriptEntry*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_MainTaskDescriptEntry*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_MainTask
	void	LuaPushValue(lua_State* L, GameLogic::LC_MainTask* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_MainTask*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_MainTask*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_MainTask*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_MainTask*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_TaskParams
	void	LuaPushValue(lua_State* L, GameLogic::LC_TaskParams* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_TaskParams*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_TaskParams*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_TaskParams*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_TaskParams*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_Task
	void	LuaPushValue(lua_State* L, GameLogic::LC_Task* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_Task*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_Task*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_Task*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_Task*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_DailyLivenessEntry
	void	LuaPushValue(lua_State* L, GameLogic::LC_DailyLivenessEntry* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_DailyLivenessEntry*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_DailyLivenessEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_DailyLivenessEntry*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_DailyLivenessEntry*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_ClientTask
	void	LuaPushValue(lua_State* L, LC_ClientTask* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientTask*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_ClientTask*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientTask*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_ClientTask*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_TaskMap
	void	LuaPushValue(lua_State* L, GameLogic::LC_TaskMap* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_TaskMap*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_TaskMap*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_TaskMap*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_TaskMap*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_AssignTaskInfo
	void	LuaPushValue(lua_State* L, GameLogic::LC_AssignTaskInfo* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_AssignTaskInfo*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_AssignTaskInfo*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_AssignTaskInfo*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_AssignTaskInfo*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_TaskAssignEntry
	void	LuaPushValue(lua_State* L, GameLogic::LC_TaskAssignEntry* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_TaskAssignEntry*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_TaskAssignEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_TaskAssignEntry*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_TaskAssignEntry*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_AssignTaskData
	void	LuaPushValue(lua_State* L, GameLogic::LC_AssignTaskData* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_AssignTaskData*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_AssignTaskData*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_AssignTaskData*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_AssignTaskData*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_ClientTaskMap
	void	LuaPushValue(lua_State* L, LC_ClientTaskMap* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientTaskMap*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_ClientTaskMap*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientTaskMap*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_ClientTaskMap*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_TradeManager
	void	LuaPushValue(lua_State* L, GameLogic::LC_TradeManager* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_TradeManager*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_TradeManager*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_TradeManager*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_TradeManager*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_NPCShopItemEntry
	void	LuaPushValue(lua_State* L, GameLogic::LC_NPCShopItemEntry* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_NPCShopItemEntry*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_NPCShopItemEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_NPCShopItemEntry*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_NPCShopItemEntry*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ShopBase
	void	LuaPushValue(lua_State* L, GameLogic::LC_ShopBase* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ShopBase*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_ShopBase*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ShopBase*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_ShopBase*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_TransferBase
	void	LuaPushValue(lua_State* L, GameLogic::LC_TransferBase* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_TransferBase*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_TransferBase*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_TransferBase*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_TransferBase*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_SkillCoolDownMap
	void	LuaPushValue(lua_State* L, GameLogic::LC_SkillCoolDownMap* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_SkillCoolDownMap*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_SkillCoolDownMap*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_SkillCoolDownMap*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_SkillCoolDownMap*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ItemCoolDownMapBase
	void	LuaPushValue(lua_State* L, GameLogic::LC_ItemCoolDownMapBase* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ItemCoolDownMapBase*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_ItemCoolDownMapBase*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ItemCoolDownMapBase*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_ItemCoolDownMapBase*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_SkillAssetEntry
	void	LuaPushValue(lua_State* L, GameLogic::LC_SkillAssetEntry* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_SkillAssetEntry*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_SkillAssetEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_SkillAssetEntry*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_SkillAssetEntry*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDSkillLC_SkillDescriptionEntry
	void	LuaPushValue(lua_State* L, Skill::LC_SkillDescriptionEntry* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<Skill::LC_SkillDescriptionEntry*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	Skill::LC_SkillDescriptionEntry*	LuaGetValue(Utility::LuaTypeWrapper<Skill::LC_SkillDescriptionEntry*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<Skill::LC_SkillDescriptionEntry*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_SkillAsset
	void	LuaPushValue(lua_State* L, GameLogic::LC_SkillAsset* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_SkillAsset*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_SkillAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_SkillAsset*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_SkillAsset*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ActorStateEntry
	void	LuaPushValue(lua_State* L, GameLogic::LC_ActorStateEntry* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ActorStateEntry*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_ActorStateEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ActorStateEntry*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_ActorStateEntry*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDSkillLC_StateDescriptionEntry
	void	LuaPushValue(lua_State* L, Skill::LC_StateDescriptionEntry* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<Skill::LC_StateDescriptionEntry*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	Skill::LC_StateDescriptionEntry*	LuaGetValue(Utility::LuaTypeWrapper<Skill::LC_StateDescriptionEntry*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<Skill::LC_StateDescriptionEntry*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ActorStateMap
	void	LuaPushValue(lua_State* L, GameLogic::LC_ActorStateMap* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ActorStateMap*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_ActorStateMap*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ActorStateMap*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_ActorStateMap*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ShortcutEntry
	void	LuaPushValue(lua_State* L, GameLogic::LC_ShortcutEntry* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ShortcutEntry*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_ShortcutEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ShortcutEntry*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_ShortcutEntry*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ShortcutBar
	void	LuaPushValue(lua_State* L, GameLogic::LC_ShortcutBar* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ShortcutBar*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_ShortcutBar*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ShortcutBar*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_ShortcutBar*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDTT_TextManager
	void	LuaPushValue(lua_State* L, TT_TextManager* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<TT_TextManager*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	TT_TextManager*	LuaGetValue(Utility::LuaTypeWrapper<TT_TextManager*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<TT_TextManager*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_TooltipManager
	void	LuaPushValue(lua_State* L, LC_TooltipManager* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_TooltipManager*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_TooltipManager*	LuaGetValue(Utility::LuaTypeWrapper<LC_TooltipManager*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_TooltipManager*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_MapManager
	void	LuaPushValue(lua_State* L, LC_MapManager* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_MapManager*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_MapManager*	LuaGetValue(Utility::LuaTypeWrapper<LC_MapManager*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_MapManager*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_MiniMap
	void	LuaPushValue(lua_State* L, LC_MiniMap* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_MiniMap*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_MiniMap*	LuaGetValue(Utility::LuaTypeWrapper<LC_MiniMap*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_MiniMap*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_TownMap
	void	LuaPushValue(lua_State* L, LC_TownMap* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_TownMap*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_TownMap*	LuaGetValue(Utility::LuaTypeWrapper<LC_TownMap*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_TownMap*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_ObjectDescription
	void	LuaPushValue(lua_State* L, LC_ObjectDescription* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ObjectDescription*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_ObjectDescription*	LuaGetValue(Utility::LuaTypeWrapper<LC_ObjectDescription*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_ObjectDescription*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_RadarObjectDescription
	void	LuaPushValue(lua_State* L, LC_RadarObjectDescription* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_RadarObjectDescription*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_RadarObjectDescription*	LuaGetValue(Utility::LuaTypeWrapper<LC_RadarObjectDescription*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_RadarObjectDescription*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_TownIconDescription
	void	LuaPushValue(lua_State* L, LC_TownIconDescription* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_TownIconDescription*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_TownIconDescription*	LuaGetValue(Utility::LuaTypeWrapper<LC_TownIconDescription*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_TownIconDescription*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_PathDesc
	void	LuaPushValue(lua_State* L, LC_PathDesc* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_PathDesc*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_PathDesc*	LuaGetValue(Utility::LuaTypeWrapper<LC_PathDesc*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_PathDesc*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_RadarMap
	void	LuaPushValue(lua_State* L, LC_RadarMap* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_RadarMap*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_RadarMap*	LuaGetValue(Utility::LuaTypeWrapper<LC_RadarMap*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_RadarMap*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_PlayerGroupEntry
	void	LuaPushValue(lua_State* L, GameLogic::LC_PlayerGroupEntry* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_PlayerGroupEntry*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_PlayerGroupEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_PlayerGroupEntry*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_PlayerGroupEntry*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_PlayerGroupBase
	void	LuaPushValue(lua_State* L, GameLogic::LC_PlayerGroupBase* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_PlayerGroupBase*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_PlayerGroupBase*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_PlayerGroupBase*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_PlayerGroupBase*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ShopCityGoodsDescriptionEntry
	void	LuaPushValue(lua_State* L, GameLogic::LC_ShopCityGoodsDescriptionEntry* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ShopCityGoodsDescriptionEntry*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_ShopCityGoodsDescriptionEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ShopCityGoodsDescriptionEntry*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_ShopCityGoodsDescriptionEntry*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ClientShopCity
	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientShopCity* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientShopCity*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_ClientShopCity*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientShopCity*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_ClientShopCity*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_PKRecorderBase
	void	LuaPushValue(lua_State* L, GameLogic::LC_PKRecorderBase* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_PKRecorderBase*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_PKRecorderBase*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_PKRecorderBase*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_PKRecorderBase*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDSC_ClientScriptInterface
	void	LuaPushValue(lua_State* L, SC_ClientScriptInterface* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<SC_ClientScriptInterface*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	SC_ClientScriptInterface*	LuaGetValue(Utility::LuaTypeWrapper<SC_ClientScriptInterface*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<SC_ClientScriptInterface*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDSC_ClientSkillInterface
	void	LuaPushValue(lua_State* L, SC_ClientSkillInterface* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<SC_ClientSkillInterface*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	SC_ClientSkillInterface*	LuaGetValue(Utility::LuaTypeWrapper<SC_ClientSkillInterface*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<SC_ClientSkillInterface*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDSC_ClientDebugInterface
	void	LuaPushValue(lua_State* L, SC_ClientDebugInterface* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<SC_ClientDebugInterface*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	SC_ClientDebugInterface*	LuaGetValue(Utility::LuaTypeWrapper<SC_ClientDebugInterface*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<SC_ClientDebugInterface*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDPathPF_Path
	void	LuaPushValue(lua_State* L, Path::PF_Path* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<Path::PF_Path*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	Path::PF_Path*	LuaGetValue(Utility::LuaTypeWrapper<Path::PF_Path*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<Path::PF_Path*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_SkillInfoEntry
	void	LuaPushValue(lua_State* L, GameLogic::LC_SkillInfoEntry* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_SkillInfoEntry*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_SkillInfoEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_SkillInfoEntry*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_SkillInfoEntry*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_PlayerFlopManager
	void	LuaPushValue(lua_State* L, LC_PlayerFlopManager* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_PlayerFlopManager*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_PlayerFlopManager*	LuaGetValue(Utility::LuaTypeWrapper<LC_PlayerFlopManager*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_PlayerFlopManager*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDUtilityUT_GlobalSettings
	void	LuaPushValue(lua_State* L, Utility::UT_GlobalSettings* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<Utility::UT_GlobalSettings*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	Utility::UT_GlobalSettings*	LuaGetValue(Utility::LuaTypeWrapper<Utility::UT_GlobalSettings*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<Utility::UT_GlobalSettings*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDUtilityGlobalSettings
	void	LuaPushValue(lua_State* L, Utility::GlobalSettings* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<Utility::GlobalSettings*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	Utility::GlobalSettings*	LuaGetValue(Utility::LuaTypeWrapper<Utility::GlobalSettings*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<Utility::GlobalSettings*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_ProductionAssetEntry
	void	LuaPushValue(lua_State* L, LC_ProductionAssetEntry* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ProductionAssetEntry*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_ProductionAssetEntry*	LuaGetValue(Utility::LuaTypeWrapper<LC_ProductionAssetEntry*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_ProductionAssetEntry*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_ClientProduction
	void	LuaPushValue(lua_State* L, LC_ClientProduction* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientProduction*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_ClientProduction*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientProduction*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_ClientProduction*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_ClientDialogManager
	void	LuaPushValue(lua_State* L, LC_ClientDialogManager* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientDialogManager*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_ClientDialogManager*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientDialogManager*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_ClientDialogManager*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_ClientDialogInfo
	void	LuaPushValue(lua_State* L, LC_ClientDialogInfo* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientDialogInfo*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_ClientDialogInfo*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientDialogInfo*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_ClientDialogInfo*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_DialogSet
	void	LuaPushValue(lua_State* L, LC_DialogSet* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_DialogSet*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_DialogSet*	LuaGetValue(Utility::LuaTypeWrapper<LC_DialogSet*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_DialogSet*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_DialogNodeData
	void	LuaPushValue(lua_State* L, LC_DialogNodeData* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_DialogNodeData*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_DialogNodeData*	LuaGetValue(Utility::LuaTypeWrapper<LC_DialogNodeData*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_DialogNodeData*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ClientMailManager
	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientMailManager* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientMailManager*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_ClientMailManager*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientMailManager*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_ClientMailManager*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_MailAsset
	void	LuaPushValue(lua_State* L, GameLogic::LC_MailAsset* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_MailAsset*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_MailAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_MailAsset*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_MailAsset*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_MailInfo
	void	LuaPushValue(lua_State* L, GameLogic::LC_MailInfo* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_MailInfo*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_MailInfo*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_MailInfo*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_MailInfo*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ClientItemAffixInfo
	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientItemAffixInfo* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientItemAffixInfo*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_ClientItemAffixInfo*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientItemAffixInfo*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_ClientItemAffixInfo*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ClientAuctionManager
	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientAuctionManager* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientAuctionManager*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_ClientAuctionManager*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientAuctionManager*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_ClientAuctionManager*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ClientAddItemInfo
	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientAddItemInfo* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientAddItemInfo*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_ClientAddItemInfo*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientAddItemInfo*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_ClientAddItemInfo*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ClientAuctionBuyData
	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientAuctionBuyData* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientAuctionBuyData*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_ClientAuctionBuyData*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientAuctionBuyData*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_ClientAuctionBuyData*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ClientAuctionSellData
	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientAuctionSellData* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientAuctionSellData*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_ClientAuctionSellData*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientAuctionSellData*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_ClientAuctionSellData*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ClientGuildManager
	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientGuildManager* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientGuildManager*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_ClientGuildManager*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientGuildManager*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_ClientGuildManager*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_GuildAsset
	void	LuaPushValue(lua_State* L, GameLogic::LC_GuildAsset* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_GuildAsset*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_GuildAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_GuildAsset*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_GuildAsset*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_GuildInfo
	void	LuaPushValue(lua_State* L, GameLogic::LC_GuildInfo* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_GuildInfo*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_GuildInfo*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_GuildInfo*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_GuildInfo*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ClientGuildMilestoneStruct
	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientGuildMilestoneStruct* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientGuildMilestoneStruct*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_ClientGuildMilestoneStruct*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientGuildMilestoneStruct*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_ClientGuildMilestoneStruct*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ClientGuildSkillInfoStruct
	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientGuildSkillInfoStruct* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientGuildSkillInfoStruct*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_ClientGuildSkillInfoStruct*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientGuildSkillInfoStruct*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_ClientGuildSkillInfoStruct*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ClientGuildSingleSkill
	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientGuildSingleSkill* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientGuildSingleSkill*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_ClientGuildSingleSkill*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientGuildSingleSkill*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_ClientGuildSingleSkill*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_GuildMemberEntry
	void	LuaPushValue(lua_State* L, GameLogic::LC_GuildMemberEntry* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_GuildMemberEntry*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_GuildMemberEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_GuildMemberEntry*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_GuildMemberEntry*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_GuildMemberInfo
	void	LuaPushValue(lua_State* L, GameLogic::LC_GuildMemberInfo* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_GuildMemberInfo*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_GuildMemberInfo*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_GuildMemberInfo*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_GuildMemberInfo*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_GuildApplyEntry
	void	LuaPushValue(lua_State* L, GameLogic::LC_GuildApplyEntry* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_GuildApplyEntry*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_GuildApplyEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_GuildApplyEntry*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_GuildApplyEntry*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_GuildApplyInfo
	void	LuaPushValue(lua_State* L, GameLogic::LC_GuildApplyInfo* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_GuildApplyInfo*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_GuildApplyInfo*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_GuildApplyInfo*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_GuildApplyInfo*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_GuildSearchInfo
	void	LuaPushValue(lua_State* L, GameLogic::LC_GuildSearchInfo* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_GuildSearchInfo*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_GuildSearchInfo*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_GuildSearchInfo*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_GuildSearchInfo*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_TutorialEntry
	void	LuaPushValue(lua_State* L, GameLogic::LC_TutorialEntry* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_TutorialEntry*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_TutorialEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_TutorialEntry*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_TutorialEntry*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_TutorialAsset
	void	LuaPushValue(lua_State* L, GameLogic::LC_TutorialAsset* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_TutorialAsset*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_TutorialAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_TutorialAsset*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_TutorialAsset*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_GameEventBase
	void	LuaPushValue(lua_State* L, GameLogic::LC_GameEventBase* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_GameEventBase*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_GameEventBase*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_GameEventBase*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_GameEventBase*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_GameEvent
	void	LuaPushValue(lua_State* L, GameLogic::LC_GameEvent* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_GameEvent*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_GameEvent*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_GameEvent*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_GameEvent*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_ClientGameEvent
	void	LuaPushValue(lua_State* L, LC_ClientGameEvent* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientGameEvent*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_ClientGameEvent*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientGameEvent*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_ClientGameEvent*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_TaskScriptImportParam
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

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_ClientMapTransDataEntry
	void	LuaPushValue(lua_State* L, LC_ClientMapTransDataEntry* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientMapTransDataEntry*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_ClientMapTransDataEntry*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientMapTransDataEntry*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_ClientMapTransDataEntry*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_ClientSocietyAsset
	void	LuaPushValue(lua_State* L, LC_ClientSocietyAsset* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientSocietyAsset*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_ClientSocietyAsset*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientSocietyAsset*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_ClientSocietyAsset*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ClientWorldLevelAsset
	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientWorldLevelAsset* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientWorldLevelAsset*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_ClientWorldLevelAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientWorldLevelAsset*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_ClientWorldLevelAsset*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_SocietyBasicInfo
	void	LuaPushValue(lua_State* L, GameLogic::LC_SocietyBasicInfo* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_SocietyBasicInfo*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_SocietyBasicInfo*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_SocietyBasicInfo*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_SocietyBasicInfo*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_FriendDetailInfo
	void	LuaPushValue(lua_State* L, GameLogic::LC_FriendDetailInfo* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_FriendDetailInfo*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_FriendDetailInfo*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_FriendDetailInfo*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_FriendDetailInfo*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_ClientFriendMsgBroadStruct
	void	LuaPushValue(lua_State* L, LC_ClientFriendMsgBroadStruct* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientFriendMsgBroadStruct*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_ClientFriendMsgBroadStruct*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientFriendMsgBroadStruct*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_ClientFriendMsgBroadStruct*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_ClientFriendMsgBroad
	void	LuaPushValue(lua_State* L, LC_ClientFriendMsgBroad* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientFriendMsgBroad*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_ClientFriendMsgBroad*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientFriendMsgBroad*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_ClientFriendMsgBroad*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ClientViewPlayerInfo
	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientViewPlayerInfo* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientViewPlayerInfo*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_ClientViewPlayerInfo*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientViewPlayerInfo*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_ClientViewPlayerInfo*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_ClientFriendNick
	void	LuaPushValue(lua_State* L, LC_ClientFriendNick* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientFriendNick*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_ClientFriendNick*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientFriendNick*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_ClientFriendNick*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_FriendNick
	void	LuaPushValue(lua_State* L, LC_FriendNick* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_FriendNick*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_FriendNick*	LuaGetValue(Utility::LuaTypeWrapper<LC_FriendNick*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_FriendNick*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDTT_TextElement
	void	LuaPushValue(lua_State* L, TT_TextElement* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<TT_TextElement*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	TT_TextElement*	LuaGetValue(Utility::LuaTypeWrapper<TT_TextElement*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<TT_TextElement*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_ShortcutKeyAssetEntry
	void	LuaPushValue(lua_State* L, LC_ShortcutKeyAssetEntry* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ShortcutKeyAssetEntry*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_ShortcutKeyAssetEntry*	LuaGetValue(Utility::LuaTypeWrapper<LC_ShortcutKeyAssetEntry*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_ShortcutKeyAssetEntry*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_ShortcutKeyAsset
	void	LuaPushValue(lua_State* L, LC_ShortcutKeyAsset* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ShortcutKeyAsset*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_ShortcutKeyAsset*	LuaGetValue(Utility::LuaTypeWrapper<LC_ShortcutKeyAsset*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_ShortcutKeyAsset*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_ClientArenaPlayerDataEntry
	void	LuaPushValue(lua_State* L, LC_ClientArenaPlayerDataEntry* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientArenaPlayerDataEntry*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_ClientArenaPlayerDataEntry*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientArenaPlayerDataEntry*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_ClientArenaPlayerDataEntry*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_ClientArenaManager
	void	LuaPushValue(lua_State* L, LC_ClientArenaManager* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientArenaManager*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_ClientArenaManager*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientArenaManager*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_ClientArenaManager*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ArenaEntry
	void	LuaPushValue(lua_State* L, GameLogic::LC_ArenaEntry* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ArenaEntry*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_ArenaEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ArenaEntry*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_ArenaEntry*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ArenaAsset
	void	LuaPushValue(lua_State* L, GameLogic::LC_ArenaAsset* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ArenaAsset*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_ArenaAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ArenaAsset*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_ArenaAsset*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_VIPAsset
	void	LuaPushValue(lua_State* L, GameLogic::LC_VIPAsset* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_VIPAsset*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_VIPAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_VIPAsset*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_VIPAsset*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_VIPFDepositEntry
	void	LuaPushValue(lua_State* L, GameLogic::LC_VIPFDepositEntry* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_VIPFDepositEntry*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_VIPFDepositEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_VIPFDepositEntry*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_VIPFDepositEntry*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_SignInAssert
	void	LuaPushValue(lua_State* L, GameLogic::LC_SignInAssert* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_SignInAssert*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_SignInAssert*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_SignInAssert*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_SignInAssert*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_RaidAssert
	void	LuaPushValue(lua_State* L, GameLogic::LC_RaidAssert* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_RaidAssert*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_RaidAssert*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_RaidAssert*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_RaidAssert*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_OpenAbilityAsset
	void	LuaPushValue(lua_State* L, GameLogic::LC_OpenAbilityAsset* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_OpenAbilityAsset*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_OpenAbilityAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_OpenAbilityAsset*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_OpenAbilityAsset*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_SubordinateAsset
	void	LuaPushValue(lua_State* L, GameLogic::LC_SubordinateAsset* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_SubordinateAsset*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_SubordinateAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_SubordinateAsset*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_SubordinateAsset*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_MeltAsset
	void	LuaPushValue(lua_State* L, GameLogic::LC_MeltAsset* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_MeltAsset*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_MeltAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_MeltAsset*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_MeltAsset*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_BoneSoulAsset
	void	LuaPushValue(lua_State* L, GameLogic::LC_BoneSoulAsset* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_BoneSoulAsset*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_BoneSoulAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_BoneSoulAsset*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_BoneSoulAsset*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_StarsRuneAsset
	void	LuaPushValue(lua_State* L, GameLogic::LC_StarsRuneAsset* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_StarsRuneAsset*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_StarsRuneAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_StarsRuneAsset*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_StarsRuneAsset*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_CommonStatRewardDataEntry
	void	LuaPushValue(lua_State* L, GameLogic::LC_CommonStatRewardDataEntry* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_CommonStatRewardDataEntry*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_CommonStatRewardDataEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_CommonStatRewardDataEntry*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_CommonStatRewardDataEntry*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ActorOperationEntry
	void	LuaPushValue(lua_State* L, GameLogic::LC_ActorOperationEntry* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ActorOperationEntry*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_ActorOperationEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ActorOperationEntry*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_ActorOperationEntry*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicGuildWareHousePack
	void	LuaPushValue(lua_State* L, GameLogic::GuildWareHousePack* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::GuildWareHousePack*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::GuildWareHousePack*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::GuildWareHousePack*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::GuildWareHousePack*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ClientGuildHomeMgr
	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientGuildHomeMgr* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientGuildHomeMgr*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_ClientGuildHomeMgr*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientGuildHomeMgr*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_ClientGuildHomeMgr*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDUI_UIManager
	void	LuaPushValue(lua_State* L, UI_UIManager* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<UI_UIManager*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	UI_UIManager*	LuaGetValue(Utility::LuaTypeWrapper<UI_UIManager*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<UI_UIManager*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_ClientArenaBattleChallengeInfoAsset
	void	LuaPushValue(lua_State* L, LC_ClientArenaBattleChallengeInfoAsset* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientArenaBattleChallengeInfoAsset*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_ClientArenaBattleChallengeInfoAsset*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientArenaBattleChallengeInfoAsset*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_ClientArenaBattleChallengeInfoAsset*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_ClientGameEventUserDataBase
	void	LuaPushValue(lua_State* L, LC_ClientGameEventUserDataBase* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientGameEventUserDataBase*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_ClientGameEventUserDataBase*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientGameEventUserDataBase*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_ClientGameEventUserDataBase*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_ClientGameEventUserData_RaidSettle
	void	LuaPushValue(lua_State* L, LC_ClientGameEventUserData_RaidSettle* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientGameEventUserData_RaidSettle*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_ClientGameEventUserData_RaidSettle*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientGameEventUserData_RaidSettle*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_ClientGameEventUserData_RaidSettle*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_ClientGameEventUserData_String
	void	LuaPushValue(lua_State* L, LC_ClientGameEventUserData_String* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientGameEventUserData_String*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_ClientGameEventUserData_String*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientGameEventUserData_String*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_ClientGameEventUserData_String*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_ClientRankUserInfo
	void	LuaPushValue(lua_State* L, LC_ClientRankUserInfo* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientRankUserInfo*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_ClientRankUserInfo*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientRankUserInfo*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_ClientRankUserInfo*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_ClientBattleRecordDataBase
	void	LuaPushValue(lua_State* L, LC_ClientBattleRecordDataBase* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientBattleRecordDataBase*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_ClientBattleRecordDataBase*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientBattleRecordDataBase*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_ClientBattleRecordDataBase*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_ClientGuessDataBase
	void	LuaPushValue(lua_State* L, LC_ClientGuessDataBase* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientGuessDataBase*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_ClientGuessDataBase*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientGuessDataBase*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_ClientGuessDataBase*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_ClientGameEventUserData_ArenaBattle_BattleInfo
	void	LuaPushValue(lua_State* L, LC_ClientGameEventUserData_ArenaBattle_BattleInfo* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientGameEventUserData_ArenaBattle_BattleInfo*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_ClientGameEventUserData_ArenaBattle_BattleInfo*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientGameEventUserData_ArenaBattle_BattleInfo*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_ClientGameEventUserData_ArenaBattle_BattleInfo*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_ClientGameEventUserData_ArenaSettleInfo
	void	LuaPushValue(lua_State* L, LC_ClientGameEventUserData_ArenaSettleInfo* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientGameEventUserData_ArenaSettleInfo*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_ClientGameEventUserData_ArenaSettleInfo*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientGameEventUserData_ArenaSettleInfo*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_ClientGameEventUserData_ArenaSettleInfo*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_ClientGameEventUserData_Common
	void	LuaPushValue(lua_State* L, LC_ClientGameEventUserData_Common* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientGameEventUserData_Common*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_ClientGameEventUserData_Common*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientGameEventUserData_Common*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_ClientGameEventUserData_Common*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_CommonStatAsset
	void	LuaPushValue(lua_State* L, GameLogic::LC_CommonStatAsset* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_CommonStatAsset*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_CommonStatAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_CommonStatAsset*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_CommonStatAsset*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_CommonStatEntry
	void	LuaPushValue(lua_State* L, GameLogic::LC_CommonStatEntry* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_CommonStatEntry*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_CommonStatEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_CommonStatEntry*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_CommonStatEntry*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_TreasureChestsAsset
	void	LuaPushValue(lua_State* L, GameLogic::LC_TreasureChestsAsset* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_TreasureChestsAsset*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_TreasureChestsAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_TreasureChestsAsset*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_TreasureChestsAsset*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDACT_SecretTreasureAsset
	void	LuaPushValue(lua_State* L, ACT_SecretTreasureAsset* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<ACT_SecretTreasureAsset*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	ACT_SecretTreasureAsset*	LuaGetValue(Utility::LuaTypeWrapper<ACT_SecretTreasureAsset*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<ACT_SecretTreasureAsset*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_SkillBookAsset
	void	LuaPushValue(lua_State* L, LC_SkillBookAsset* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_SkillBookAsset*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_SkillBookAsset*	LuaGetValue(Utility::LuaTypeWrapper<LC_SkillBookAsset*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_SkillBookAsset*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_CommonStatManager
	void	LuaPushValue(lua_State* L, GameLogic::LC_CommonStatManager* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_CommonStatManager*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_CommonStatManager*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_CommonStatManager*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_CommonStatManager*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ClientSoulRefineAsset
	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientSoulRefineAsset* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientSoulRefineAsset*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_ClientSoulRefineAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientSoulRefineAsset*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_ClientSoulRefineAsset*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ClientShotActivityManager
	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientShotActivityManager* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientShotActivityManager*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_ClientShotActivityManager*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientShotActivityManager*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_ClientShotActivityManager*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_FashionBagAsset
	void	LuaPushValue(lua_State* L, GameLogic::LC_FashionBagAsset* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_FashionBagAsset*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_FashionBagAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_FashionBagAsset*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_FashionBagAsset*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ClientRumorManager
	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientRumorManager* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientRumorManager*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_ClientRumorManager*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientRumorManager*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_ClientRumorManager*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ClientActivityManager
	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientActivityManager* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientActivityManager*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_ClientActivityManager*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientActivityManager*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_ClientActivityManager*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ClientActivityAsset
	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientActivityAsset* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientActivityAsset*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	GameLogic::LC_ClientActivityAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientActivityAsset*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<GameLogic::LC_ClientActivityAsset*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_AchievementManager
	void	LuaPushValue(lua_State* L, LC_AchievementManager* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_AchievementManager*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_AchievementManager*	LuaGetValue(Utility::LuaTypeWrapper<LC_AchievementManager*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_AchievementManager*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_ClientToyPetManager
	void	LuaPushValue(lua_State* L, LC_ClientToyPetManager* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientToyPetManager*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_ClientToyPetManager*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientToyPetManager*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_ClientToyPetManager*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_ClientCombatRecordManager
	void	LuaPushValue(lua_State* L, LC_ClientCombatRecordManager* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientCombatRecordManager*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_ClientCombatRecordManager*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientCombatRecordManager*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_ClientCombatRecordManager*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_ClientGuildProfitManager
	void	LuaPushValue(lua_State* L, LC_ClientGuildProfitManager* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientGuildProfitManager*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_ClientGuildProfitManager*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientGuildProfitManager*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_ClientGuildProfitManager*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_ClientArenaPlayBackManager
	void	LuaPushValue(lua_State* L, LC_ClientArenaPlayBackManager* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientArenaPlayBackManager*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_ClientArenaPlayBackManager*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientArenaPlayBackManager*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_ClientArenaPlayBackManager*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_ClientReportManager
	void	LuaPushValue(lua_State* L, LC_ClientReportManager* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientReportManager*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_ClientReportManager*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientReportManager*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_ClientReportManager*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_ClientToyPetInstance
	void	LuaPushValue(lua_State* L, LC_ClientToyPetInstance* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientToyPetInstance*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_ClientToyPetInstance*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientToyPetInstance*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_ClientToyPetInstance*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_ClientCollectionManager
	void	LuaPushValue(lua_State* L, LC_ClientCollectionManager* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientCollectionManager*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_ClientCollectionManager*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientCollectionManager*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_ClientCollectionManager*>(*ubox);  
	}
#endif

#ifdef SC_ClientScriptInterface_SCRIPT_HANDLC_ClientBotManager
	void	LuaPushValue(lua_State* L, LC_ClientBotManager* value)
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
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientBotManager*>, lua_State* L, int idx)
	{
		int ntype = lua_type(L, idx);
		return (ntype == LUA_TLIGHTUSERDATA) || (ntype == LUA_TUSERDATA);  
	}
	LC_ClientBotManager*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientBotManager*>, lua_State* L, int idx)
	{ 
		void** ubox = (void**)lua_touserdata(L, idx);
		T_ASSERT(ubox && *ubox);
		return static_cast<LC_ClientBotManager*>(*ubox);  
	}
#endif

}
