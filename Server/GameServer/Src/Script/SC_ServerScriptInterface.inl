	////////////////////////////////////////////////
	//由Perl脚本自动生成的Lua绑定代码，请勿手动修改！//
	////////////////////////////////////////////////
	///////////////////////////////////////////////
	////////LC_ActorAttributeMap Lua Export////////////////////
	///////////////////////////////////////////////
	static int bnd_LuaCreate_LC_ActorAttributeMap(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		GameLogic::LC_ActorAttributeMap* h = NULL;
		if( h == NULL )
		{
			GfxWriteLog(LOG_WARNING_FILELINE, LOG_SWITCH_WARNING, "全局类%s的对象还没有创建就试图在Lua脚本中调用，请检查初始化时序！！", "LC_ActorAttributeMap");
			return 0;
		}
		binder.PushUserType(h, h->GetRTTIName());
		return 1;
	}
	static int bnd_LuaDestroy_LC_ActorAttributeMap(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		//GameLogic::LC_ActorAttributeMap* h = (GameLogic::LC_ActorAttributeMap*) binder.CheckUserType(1, "LC_ActorAttributeMap");
		binder.CheckUserType(1, "LC_ActorAttributeMap");
		return 0;
	}
	static int bnd_LC_ActorAttributeMap_GetAttribute(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		GameLogic::LC_ActorAttributeMap* pkAPI = (GameLogic::LC_ActorAttributeMap*)binder.CheckUserType(1, "LC_ActorAttributeMap");
		Utility::LuaCall(*pkAPI, &GameLogic::LC_ActorAttributeMap::GetAttribute, L, 2);
		return 1;
	}
	static const luaL_reg lib_LC_ActorAttributeMap[] = {
	{"Create", bnd_LuaCreate_LC_ActorAttributeMap},
	{"GetAttribute", bnd_LC_ActorAttributeMap_GetAttribute},
	{NULL, NULL}
	};
	static int luaopen_LC_ActorAttributeMap(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		binder.Init("LC_ActorAttributeMap", 0, lib_LC_ActorAttributeMap, bnd_LuaDestroy_LC_ActorAttributeMap);
		return 1;
	}
	///////////////////////////////////////////////
	////////LC_ServerTask Lua Export////////////////////
	///////////////////////////////////////////////
	static int bnd_LuaCreate_LC_ServerTask(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		LC_ServerTask* h = NULL;
		if( h == NULL )
		{
			GfxWriteLog(LOG_WARNING_FILELINE, LOG_SWITCH_WARNING, "全局类%s的对象还没有创建就试图在Lua脚本中调用，请检查初始化时序！！", "LC_ServerTask");
			return 0;
		}
		binder.PushUserType(h, h->GetRTTIName());
		return 1;
	}
	static int bnd_LuaDestroy_LC_ServerTask(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		//LC_ServerTask* h = (LC_ServerTask*) binder.CheckUserType(1, "LC_ServerTask");
		binder.CheckUserType(1, "LC_ServerTask");
		return 0;
	}
	static const luaL_reg lib_LC_ServerTask[] = {
	{"Create", bnd_LuaCreate_LC_ServerTask},
	{NULL, NULL}
	};
	static int luaopen_LC_ServerTask(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		binder.Init("LC_ServerTask", "LC_Task", lib_LC_ServerTask, bnd_LuaDestroy_LC_ServerTask);
		return 1;
	}
	///////////////////////////////////////////////
	////////LC_TaskScriptImportParam Lua Export////////////////////
	///////////////////////////////////////////////
	static int bnd_LuaCreate_LC_TaskScriptImportParam(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		GameLogic::LC_TaskScriptImportParam* h = NULL;
		if( h == NULL )
		{
			GfxWriteLog(LOG_WARNING_FILELINE, LOG_SWITCH_WARNING, "全局类%s的对象还没有创建就试图在Lua脚本中调用，请检查初始化时序！！", "LC_TaskScriptImportParam");
			return 0;
		}
		binder.PushUserType(h, h->GetRTTIName());
		return 1;
	}
	static int bnd_LuaDestroy_LC_TaskScriptImportParam(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		//GameLogic::LC_TaskScriptImportParam* h = (GameLogic::LC_TaskScriptImportParam*) binder.CheckUserType(1, "LC_TaskScriptImportParam");
		binder.CheckUserType(1, "LC_TaskScriptImportParam");
		return 0;
	}
	static int bnd_LC_TaskScriptImportParam_GetParam(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		GameLogic::LC_TaskScriptImportParam* pkAPI = (GameLogic::LC_TaskScriptImportParam*)binder.CheckUserType(1, "LC_TaskScriptImportParam");
		Utility::LuaCall(*pkAPI, &GameLogic::LC_TaskScriptImportParam::GetParam, L, 2);
		return 1;
	}
	static int bnd_LC_TaskScriptImportParam_SetParam(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		GameLogic::LC_TaskScriptImportParam* pkAPI = (GameLogic::LC_TaskScriptImportParam*)binder.CheckUserType(1, "LC_TaskScriptImportParam");
		Utility::LuaCall(*pkAPI, &GameLogic::LC_TaskScriptImportParam::SetParam, L, 2);
		return 0;
	}		
	static const luaL_reg lib_LC_TaskScriptImportParam[] = {
	{"Create", bnd_LuaCreate_LC_TaskScriptImportParam},
	{"GetParam", bnd_LC_TaskScriptImportParam_GetParam},
	{"SetParam", bnd_LC_TaskScriptImportParam_SetParam},
	{NULL, NULL}
	};
	static int luaopen_LC_TaskScriptImportParam(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		binder.Init("LC_TaskScriptImportParam", "MM_BaseObject", lib_LC_TaskScriptImportParam, bnd_LuaDestroy_LC_TaskScriptImportParam);
		return 1;
	}
	///////////////////////////////////////////////
	////////LC_ServerTaskMap Lua Export////////////////////
	///////////////////////////////////////////////
	static int bnd_LuaCreate_LC_ServerTaskMap(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		LC_ServerTaskMap* h = NULL;
		if( h == NULL )
		{
			GfxWriteLog(LOG_WARNING_FILELINE, LOG_SWITCH_WARNING, "全局类%s的对象还没有创建就试图在Lua脚本中调用，请检查初始化时序！！", "LC_ServerTaskMap");
			return 0;
		}
		binder.PushUserType(h, h->GetRTTIName());
		return 1;
	}
	static int bnd_LuaDestroy_LC_ServerTaskMap(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		//LC_ServerTaskMap* h = (LC_ServerTaskMap*) binder.CheckUserType(1, "LC_ServerTaskMap");
		binder.CheckUserType(1, "LC_ServerTaskMap");
		return 0;
	}
	static const luaL_reg lib_LC_ServerTaskMap[] = {
	{"Create", bnd_LuaCreate_LC_ServerTaskMap},
	{NULL, NULL}
	};
	static int luaopen_LC_ServerTaskMap(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		binder.Init("LC_ServerTaskMap", "LC_TaskMap", lib_LC_ServerTaskMap, bnd_LuaDestroy_LC_ServerTaskMap);
		return 1;
	}
	///////////////////////////////////////////////
	////////LC_ServerPlayer Lua Export////////////////////
	///////////////////////////////////////////////
	static int bnd_LuaCreate_LC_ServerPlayer(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		LC_ServerPlayer* h = NULL;
		if( h == NULL )
		{
			GfxWriteLog(LOG_WARNING_FILELINE, LOG_SWITCH_WARNING, "全局类%s的对象还没有创建就试图在Lua脚本中调用，请检查初始化时序！！", "LC_ServerPlayer");
			return 0;
		}
		binder.PushUserType(h, h->GetRTTIName());
		return 1;
	}
	static int bnd_LuaDestroy_LC_ServerPlayer(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		//LC_ServerPlayer* h = (LC_ServerPlayer*) binder.CheckUserType(1, "LC_ServerPlayer");
		binder.CheckUserType(1, "LC_ServerPlayer");
		return 0;
	}
	static int bnd_LC_ServerPlayer_AddAttributePlus(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		LC_ServerPlayer* pkAPI = (LC_ServerPlayer*)binder.CheckUserType(1, "LC_ServerPlayer");
		Utility::LuaCall(*pkAPI, &LC_ServerPlayer::AddAttributePlus, L, 2);
		return 0;
	}		
	static int bnd_LC_ServerPlayer_IsInGame(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		LC_ServerPlayer* pkAPI = (LC_ServerPlayer*)binder.CheckUserType(1, "LC_ServerPlayer");
		Utility::LuaCall(*pkAPI, &LC_ServerPlayer::IsInGame, L, 2);
		return 1;
	}
	static const luaL_reg lib_LC_ServerPlayer[] = {
	{"Create", bnd_LuaCreate_LC_ServerPlayer},
	{"AddAttributePlus", bnd_LC_ServerPlayer_AddAttributePlus},
	{"IsInGame", bnd_LC_ServerPlayer_IsInGame},
	{NULL, NULL}
	};
	static int luaopen_LC_ServerPlayer(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		binder.Init("LC_ServerPlayer", "LC_PlayerBase", lib_LC_ServerPlayer, bnd_LuaDestroy_LC_ServerPlayer);
		return 1;
	}
	///////////////////////////////////////////////
	////////SC_ServerScriptInterface Lua Export////////////////////
	///////////////////////////////////////////////
	static int bnd_LuaCreate_SC_ServerScriptInterface(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerScriptInterface* h = SC_ServerScriptInterface::GetSingletonPtr();
		if( h == NULL )
		{
			GfxWriteLog(LOG_WARNING_FILELINE, LOG_SWITCH_WARNING, "全局类%s的对象还没有创建就试图在Lua脚本中调用，请检查初始化时序！！", "SC_ServerScriptInterface");
			return 0;
		}
		binder.PushUserType(h, h->GetRTTIName());
		return 1;
	}
	static int bnd_LuaDestroy_SC_ServerScriptInterface(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		//SC_ServerScriptInterface* h = (SC_ServerScriptInterface*) binder.CheckUserType(1, "SC_ServerScriptInterface");
		binder.CheckUserType(1, "SC_ServerScriptInterface");
		return 0;
	}
	static int bnd_SC_ServerScriptInterface_GetSkillInterface(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerScriptInterface* pkAPI = (SC_ServerScriptInterface*)binder.CheckUserType(1, "SC_ServerScriptInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerScriptInterface::GetSkillInterface, L, 2);
		return 1;
	}
	static int bnd_SC_ServerScriptInterface_GetDebugInterface(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerScriptInterface* pkAPI = (SC_ServerScriptInterface*)binder.CheckUserType(1, "SC_ServerScriptInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerScriptInterface::GetDebugInterface, L, 2);
		return 1;
	}
	static const luaL_reg lib_SC_ServerScriptInterface[] = {
	{"Create", bnd_LuaCreate_SC_ServerScriptInterface},
	{"GetSkillInterface", bnd_SC_ServerScriptInterface_GetSkillInterface},
	{"GetDebugInterface", bnd_SC_ServerScriptInterface_GetDebugInterface},
	{NULL, NULL}
	};
	static int luaopen_SC_ServerScriptInterface(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		binder.Init("SC_ServerScriptInterface", 0, lib_SC_ServerScriptInterface, bnd_LuaDestroy_SC_ServerScriptInterface);
		return 1;
	}
	///////////////////////////////////////////////
	////////SC_ServerSkillInterface Lua Export////////////////////
	///////////////////////////////////////////////
	static int bnd_LuaCreate_SC_ServerSkillInterface(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* h = NULL;
		if( h == NULL )
		{
			GfxWriteLog(LOG_WARNING_FILELINE, LOG_SWITCH_WARNING, "全局类%s的对象还没有创建就试图在Lua脚本中调用，请检查初始化时序！！", "SC_ServerSkillInterface");
			return 0;
		}
		binder.PushUserType(h, h->GetRTTIName());
		return 1;
	}
	static int bnd_LuaDestroy_SC_ServerSkillInterface(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		//SC_ServerSkillInterface* h = (SC_ServerSkillInterface*) binder.CheckUserType(1, "SC_ServerSkillInterface");
		binder.CheckUserType(1, "SC_ServerSkillInterface");
		return 0;
	}
	static int bnd_SC_ServerSkillInterface_ToNowID(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::ToNowID, L, 2);
		return 1;
	}
	static int bnd_SC_ServerSkillInterface_IsInCross(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::IsInCross, L, 2);
		return 1;
	}
	static int bnd_SC_ServerSkillInterface_GetLCLockedID(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::GetLCLockedID, L, 2);
		return 1;
	}
	static int bnd_SC_ServerSkillInterface_GuildWarCapapultMsg(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::GuildWarCapapultMsg, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerSkillInterface_GuildWarCapapultAllMsg(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::GuildWarCapapultAllMsg, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerSkillInterface_LockAttribute(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::LockAttribute, L, 2);
		return 1;
	}
	static int bnd_SC_ServerSkillInterface_MapLogicSpawnNPC(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::MapLogicSpawnNPC, L, 2);
		return 1;
	}
	static int bnd_SC_ServerSkillInterface_MapLogicSpawnNPCXYZ(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::MapLogicSpawnNPCXYZ, L, 2);
		return 1;
	}
	static int bnd_SC_ServerSkillInterface_MapLogicSpawnEliteNPC(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::MapLogicSpawnEliteNPC, L, 2);
		return 1;
	}
	static int bnd_SC_ServerSkillInterface_GetCurrentMapSummorCount(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::GetCurrentMapSummorCount, L, 2);
		return 1;
	}
	static int bnd_SC_ServerSkillInterface_NotifySingleTranscriptionData(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::NotifySingleTranscriptionData, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerSkillInterface_DismissTargetVehicle(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::DismissTargetVehicle, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerSkillInterface_NotifyClientShowTips(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::NotifyClientShowTips, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerSkillInterface_NotifyClientShowTipsInMap(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::NotifyClientShowTipsInMap, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerSkillInterface_GodKill(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::GodKill, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerSkillInterface_NotifyCastSkillToGatherTaskNpc(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::NotifyCastSkillToGatherTaskNpc, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerSkillInterface_AddUnbindYuanbao(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::AddUnbindYuanbao, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerSkillInterface_AddYuanZhengPoint(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::AddYuanZhengPoint, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerSkillInterface_ActiveSubTransform(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::ActiveSubTransform, L, 2);
		return 1;
	}
	static int bnd_SC_ServerSkillInterface_ActiveBuddyTransform(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::ActiveBuddyTransform, L, 2);
		return 1;
	}
	static int bnd_SC_ServerSkillInterface_OpenFDeposit(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::OpenFDeposit, L, 2);
		return 1;
	}
	static int bnd_SC_ServerSkillInterface_ItemUseAddBossDropTimes(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::ItemUseAddBossDropTimes, L, 2);
		return 1;
	}
	static int bnd_SC_ServerSkillInterface_ItemUseActivateCommanderTitleID(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::ItemUseActivateCommanderTitleID, L, 2);
		return 1;
	}
	static int bnd_SC_ServerSkillInterface_AddOssRecharge(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::AddOssRecharge, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerSkillInterface_GetEscortStealCount(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::GetEscortStealCount, L, 2);
		return 1;
	}
	static int bnd_SC_ServerSkillInterface_SetNPCAttacker(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::SetNPCAttacker, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerSkillInterface_SetLockedLocationByMouseLocation(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::SetLockedLocationByMouseLocation, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerSkillInterface_NotifyClientShowTipsInGuild(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::NotifyClientShowTipsInGuild, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerSkillInterface_UseItemAddlevel(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::UseItemAddlevel, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerSkillInterface_UseItemAddSubordinateAttr(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::UseItemAddSubordinateAttr, L, 2);
		return 1;
	}
	static int bnd_SC_ServerSkillInterface_UseItemCommitAddSubordinateAttr(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::UseItemCommitAddSubordinateAttr, L, 2);
		return 1;
	}
	static int bnd_SC_ServerSkillInterface_UseItemAddSubordinateBlessPoint(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::UseItemAddSubordinateBlessPoint, L, 2);
		return 1;
	}
	static int bnd_SC_ServerSkillInterface_CheckUseItemOpenSubordinate(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::CheckUseItemOpenSubordinate, L, 2);
		return 1;
	}
	static int bnd_SC_ServerSkillInterface_UseItemAddSubLevel(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::UseItemAddSubLevel, L, 2);
		return 1;
	}
	static int bnd_SC_ServerSkillInterface_UseItemAddSubLevelTo(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::UseItemAddSubLevelTo, L, 2);
		return 1;
	}
	static int bnd_SC_ServerSkillInterface_UseItemOpenSubordinate(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::UseItemOpenSubordinate, L, 2);
		return 1;
	}
	static int bnd_SC_ServerSkillInterface_UseItemAddSnowManScore(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::UseItemAddSnowManScore, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerSkillInterface_GetWorldLevel(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::GetWorldLevel, L, 2);
		return 1;
	}
	static int bnd_SC_ServerSkillInterface_GetGroupWorldLevel(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::GetGroupWorldLevel, L, 2);
		return 1;
	}
	static int bnd_SC_ServerSkillInterface_DoConsumeItems(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::DoConsumeItems, L, 2);
		return 1;
	}
	static int bnd_SC_ServerSkillInterface_DoPayShops(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::DoPayShops, L, 2);
		return 1;
	}
	static int bnd_SC_ServerSkillInterface_SetMapLockEnter(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::SetMapLockEnter, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerSkillInterface_SetWatchTowerState(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::SetWatchTowerState, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerSkillInterface_SetTowerLocation(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::SetTowerLocation, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerSkillInterface_CurrentMapIsRecorderPkValue(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::CurrentMapIsRecorderPkValue, L, 2);
		return 1;
	}
	static int bnd_SC_ServerSkillInterface_ServerPlayerCross(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::ServerPlayerCross, L, 2);
		return 1;
	}
	static int bnd_SC_ServerSkillInterface_SendKingFlagGainInfo(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::SendKingFlagGainInfo, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerSkillInterface_SendFactionResult(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::SendFactionResult, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerSkillInterface_ActivateFashion(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::ActivateFashion, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerSkillInterface_SetShieldBreak(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::SetShieldBreak, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerSkillInterface_CheckUseItemOpenSkillBook(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::CheckUseItemOpenSkillBook, L, 2);
		return 1;
	}
	static int bnd_SC_ServerSkillInterface_UseItemOpenSkillBook(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::UseItemOpenSkillBook, L, 2);
		return 1;
	}
	static int bnd_SC_ServerSkillInterface_UseItemAddActScore(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::UseItemAddActScore, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerSkillInterface_ChooseFactionByGlobalPower(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::ChooseFactionByGlobalPower, L, 2);
		return 1;
	}
	static int bnd_SC_ServerSkillInterface_GetFactionDefServer(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::GetFactionDefServer, L, 2);
		return 1;
	}
	static int bnd_SC_ServerSkillInterface_GetFactionFailBuffOverlap(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::GetFactionFailBuffOverlap, L, 2);
		return 1;
	}
	static int bnd_SC_ServerSkillInterface_AddGemReinforceExp(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::AddGemReinforceExp, L, 2);
		return 1;
	}
	static int bnd_SC_ServerSkillInterface_AddRelateTimeBuff(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::AddRelateTimeBuff, L, 2);
		return 1;
	}
	static int bnd_SC_ServerSkillInterface_AddGSFactionScore(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::AddGSFactionScore, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerSkillInterface_AddGSActorScore(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::AddGSActorScore, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerSkillInterface_GFxWriteGAMELog(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerSkillInterface* pkAPI = (SC_ServerSkillInterface*)binder.CheckUserType(1, "SC_ServerSkillInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerSkillInterface::GFxWriteGAMELog, L, 2);
		return 0;
	}		
	static const luaL_reg lib_SC_ServerSkillInterface[] = {
	{"Create", bnd_LuaCreate_SC_ServerSkillInterface},
	{"ToNowID", bnd_SC_ServerSkillInterface_ToNowID},
	{"IsInCross", bnd_SC_ServerSkillInterface_IsInCross},
	{"GetLCLockedID", bnd_SC_ServerSkillInterface_GetLCLockedID},
	{"GuildWarCapapultMsg", bnd_SC_ServerSkillInterface_GuildWarCapapultMsg},
	{"GuildWarCapapultAllMsg", bnd_SC_ServerSkillInterface_GuildWarCapapultAllMsg},
	{"LockAttribute", bnd_SC_ServerSkillInterface_LockAttribute},
	{"MapLogicSpawnNPC", bnd_SC_ServerSkillInterface_MapLogicSpawnNPC},
	{"MapLogicSpawnNPCXYZ", bnd_SC_ServerSkillInterface_MapLogicSpawnNPCXYZ},
	{"MapLogicSpawnEliteNPC", bnd_SC_ServerSkillInterface_MapLogicSpawnEliteNPC},
	{"GetCurrentMapSummorCount", bnd_SC_ServerSkillInterface_GetCurrentMapSummorCount},
	{"NotifySingleTranscriptionData", bnd_SC_ServerSkillInterface_NotifySingleTranscriptionData},
	{"DismissTargetVehicle", bnd_SC_ServerSkillInterface_DismissTargetVehicle},
	{"NotifyClientShowTips", bnd_SC_ServerSkillInterface_NotifyClientShowTips},
	{"NotifyClientShowTipsInMap", bnd_SC_ServerSkillInterface_NotifyClientShowTipsInMap},
	{"GodKill", bnd_SC_ServerSkillInterface_GodKill},
	{"NotifyCastSkillToGatherTaskNpc", bnd_SC_ServerSkillInterface_NotifyCastSkillToGatherTaskNpc},
	{"AddUnbindYuanbao", bnd_SC_ServerSkillInterface_AddUnbindYuanbao},
	{"AddYuanZhengPoint", bnd_SC_ServerSkillInterface_AddYuanZhengPoint},
	{"ActiveSubTransform", bnd_SC_ServerSkillInterface_ActiveSubTransform},
	{"ActiveBuddyTransform", bnd_SC_ServerSkillInterface_ActiveBuddyTransform},
	{"OpenFDeposit", bnd_SC_ServerSkillInterface_OpenFDeposit},
	{"ItemUseAddBossDropTimes", bnd_SC_ServerSkillInterface_ItemUseAddBossDropTimes},
	{"ItemUseActivateCommanderTitleID", bnd_SC_ServerSkillInterface_ItemUseActivateCommanderTitleID},
	{"AddOssRecharge", bnd_SC_ServerSkillInterface_AddOssRecharge},
	{"GetEscortStealCount", bnd_SC_ServerSkillInterface_GetEscortStealCount},
	{"SetNPCAttacker", bnd_SC_ServerSkillInterface_SetNPCAttacker},
	{"SetLockedLocationByMouseLocation", bnd_SC_ServerSkillInterface_SetLockedLocationByMouseLocation},
	{"NotifyClientShowTipsInGuild", bnd_SC_ServerSkillInterface_NotifyClientShowTipsInGuild},
	{"UseItemAddlevel", bnd_SC_ServerSkillInterface_UseItemAddlevel},
	{"UseItemAddSubordinateAttr", bnd_SC_ServerSkillInterface_UseItemAddSubordinateAttr},
	{"UseItemCommitAddSubordinateAttr", bnd_SC_ServerSkillInterface_UseItemCommitAddSubordinateAttr},
	{"UseItemAddSubordinateBlessPoint", bnd_SC_ServerSkillInterface_UseItemAddSubordinateBlessPoint},
	{"CheckUseItemOpenSubordinate", bnd_SC_ServerSkillInterface_CheckUseItemOpenSubordinate},
	{"UseItemAddSubLevel", bnd_SC_ServerSkillInterface_UseItemAddSubLevel},
	{"UseItemAddSubLevelTo", bnd_SC_ServerSkillInterface_UseItemAddSubLevelTo},
	{"UseItemOpenSubordinate", bnd_SC_ServerSkillInterface_UseItemOpenSubordinate},
	{"UseItemAddSnowManScore", bnd_SC_ServerSkillInterface_UseItemAddSnowManScore},
	{"GetWorldLevel", bnd_SC_ServerSkillInterface_GetWorldLevel},
	{"GetGroupWorldLevel", bnd_SC_ServerSkillInterface_GetGroupWorldLevel},
	{"DoConsumeItems", bnd_SC_ServerSkillInterface_DoConsumeItems},
	{"DoPayShops", bnd_SC_ServerSkillInterface_DoPayShops},
	{"SetMapLockEnter", bnd_SC_ServerSkillInterface_SetMapLockEnter},
	{"SetWatchTowerState", bnd_SC_ServerSkillInterface_SetWatchTowerState},
	{"SetTowerLocation", bnd_SC_ServerSkillInterface_SetTowerLocation},
	{"CurrentMapIsRecorderPkValue", bnd_SC_ServerSkillInterface_CurrentMapIsRecorderPkValue},
	{"ServerPlayerCross", bnd_SC_ServerSkillInterface_ServerPlayerCross},
	{"SendKingFlagGainInfo", bnd_SC_ServerSkillInterface_SendKingFlagGainInfo},
	{"SendFactionResult", bnd_SC_ServerSkillInterface_SendFactionResult},
	{"ActivateFashion", bnd_SC_ServerSkillInterface_ActivateFashion},
	{"SetShieldBreak", bnd_SC_ServerSkillInterface_SetShieldBreak},
	{"CheckUseItemOpenSkillBook", bnd_SC_ServerSkillInterface_CheckUseItemOpenSkillBook},
	{"UseItemOpenSkillBook", bnd_SC_ServerSkillInterface_UseItemOpenSkillBook},
	{"UseItemAddActScore", bnd_SC_ServerSkillInterface_UseItemAddActScore},
	{"ChooseFactionByGlobalPower", bnd_SC_ServerSkillInterface_ChooseFactionByGlobalPower},
	{"GetFactionDefServer", bnd_SC_ServerSkillInterface_GetFactionDefServer},
	{"GetFactionFailBuffOverlap", bnd_SC_ServerSkillInterface_GetFactionFailBuffOverlap},
	{"AddGemReinforceExp", bnd_SC_ServerSkillInterface_AddGemReinforceExp},
	{"AddRelateTimeBuff", bnd_SC_ServerSkillInterface_AddRelateTimeBuff},
	{"AddGSFactionScore", bnd_SC_ServerSkillInterface_AddGSFactionScore},
	{"AddGSActorScore", bnd_SC_ServerSkillInterface_AddGSActorScore},
	{"GFxWriteGAMELog", bnd_SC_ServerSkillInterface_GFxWriteGAMELog},
	{NULL, NULL}
	};
	static int luaopen_SC_ServerSkillInterface(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		binder.Init("SC_ServerSkillInterface", "SC_SkillInterface", lib_SC_ServerSkillInterface, bnd_LuaDestroy_SC_ServerSkillInterface);
		return 1;
	}
	///////////////////////////////////////////////
	////////SC_ServerDebugInterface Lua Export////////////////////
	///////////////////////////////////////////////
	static int bnd_LuaCreate_SC_ServerDebugInterface(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* h = NULL;
		if( h == NULL )
		{
			GfxWriteLog(LOG_WARNING_FILELINE, LOG_SWITCH_WARNING, "全局类%s的对象还没有创建就试图在Lua脚本中调用，请检查初始化时序！！", "SC_ServerDebugInterface");
			return 0;
		}
		binder.PushUserType(h, h->GetRTTIName());
		return 1;
	}
	static int bnd_LuaDestroy_SC_ServerDebugInterface(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		//SC_ServerDebugInterface* h = (SC_ServerDebugInterface*) binder.CheckUserType(1, "SC_ServerDebugInterface");
		binder.CheckUserType(1, "SC_ServerDebugInterface");
		return 0;
	}
	static int bnd_SC_ServerDebugInterface_SendToDebugConsole(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SendToDebugConsole, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_LogError(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::LogError, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_GetCurrentTimestamp(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetCurrentTimestamp, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_GetServerID(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetServerID, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_GetServerStartTime(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetServerStartTime, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_GetBackPackTypeID(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetBackPackTypeID, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_FindOwerType(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::FindOwerType, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_SetState(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SetState, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_GetAttributeMap(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetAttributeMap, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_NPCScript(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::NPCScript, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_GetServerPlayerNum(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetServerPlayerNum, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_GetPlayerID(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetPlayerID, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_GetServerNPCNum(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetServerNPCNum, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_GetMapPlayerCount(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetMapPlayerCount, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_GetNPCID(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetNPCID, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_GetNPCShopFuncID(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetNPCShopFuncID, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_SetListeningFlag(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SetListeningFlag, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_SetHP(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SetHP, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_SetSkillLevelupPoints(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SetSkillLevelupPoints, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_SetOwnerMaxMP(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SetOwnerMaxMP, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_ModifyOwnerMaxMP(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::ModifyOwnerMaxMP, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_SetOwnerMP(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SetOwnerMP, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_ModifyOwnerMP(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::ModifyOwnerMP, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_SyncOwnerMP(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SyncOwnerMP, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_SetMP(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SetMP, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_SetCombatRecorderEnabled(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SetCombatRecorderEnabled, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_FinishCombatRecorder(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::FinishCombatRecorder, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_AddSkill(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::AddSkill, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_RemoveSkill(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::RemoveSkill, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_RemoveAllSkill(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::RemoveAllSkill, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_AddSkillBook(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::AddSkillBook, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_RemoveSkillBook(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::RemoveSkillBook, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_SkillBookNoteSoul(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SkillBookNoteSoul, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_SkillBookUpgrade(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SkillBookUpgrade, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_QueryUserChallenge(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::QueryUserChallenge, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_QueryRankUserInfo(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::QueryRankUserInfo, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_ActivateStarVein(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::ActivateStarVein, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_GetFDRewardTime(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetFDRewardTime, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_GetFDFreeReward(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetFDFreeReward, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_GetFDDisCountReward(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetFDDisCountReward, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_UpgradeSubordinate(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::UpgradeSubordinate, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_BuyBuddyGrowthPackage(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::BuyBuddyGrowthPackage, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_GetBuddyGrowthReward(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetBuddyGrowthReward, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_CollectionActivation(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::CollectionActivation, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_CollectionReward(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::CollectionReward, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_GainOfflineEXP(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GainOfflineEXP, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_LeaveGame(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::LeaveGame, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_Console(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::Console, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_ActivateSkillBook(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::ActivateSkillBook, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_UnlockSkillBookSlot(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::UnlockSkillBookSlot, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_WearSkillBook(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::WearSkillBook, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_ReliveBeforeEnterMap(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::ReliveBeforeEnterMap, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_TakeChapterReward(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::TakeChapterReward, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_BuyItem(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::BuyItem, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_AddItem(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::AddItem, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_GetActor(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetActor, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_ReloadSkillScript(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::ReloadSkillScript, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_AddCash(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::AddCash, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_AddBindCash(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::AddBindCash, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_AddUnBindCash(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::AddUnBindCash, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_AddCouragePoint(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::AddCouragePoint, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_BroadcastMsg(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::BroadcastMsg, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_AddJusticePoint(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::AddJusticePoint, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_AddExp(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::AddExp, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_AddExpToBuddy(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::AddExpToBuddy, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_AddExpToActivateBuddy(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::AddExpToActivateBuddy, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_AddExpToAllBuddy(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::AddExpToAllBuddy, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_SetBaseDataStr(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SetBaseDataStr, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_EchoBaseDataStr(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::EchoBaseDataStr, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_SetBaseDataInt(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SetBaseDataInt, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_EchoBaseDataInt(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::EchoBaseDataInt, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_AddLevel(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::AddLevel, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_GMDead(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GMDead, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_UpgradeBuddyStar(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::UpgradeBuddyStar, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_AddBuddyLevel(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::AddBuddyLevel, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_RegisterCombatEvent(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::RegisterCombatEvent, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_UnregisterCombatEvent(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::UnregisterCombatEvent, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_BuddyLevelUp(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::BuddyLevelUp, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_SwitchLine(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SwitchLine, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_GuildOfflineSolo(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GuildOfflineSolo, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_SetNewVIPLevel(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SetNewVIPLevel, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_SetBuddySchemeData(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SetBuddySchemeData, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_SwitchToBuddyScheme(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SwitchToBuddyScheme, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_ResetBuddyScheme(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::ResetBuddyScheme, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_AddBuddyChain(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::AddBuddyChain, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_DelBuddyChain(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::DelBuddyChain, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_SwitchBuddyChain(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SwitchBuddyChain, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_SetBuddyChainCount(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SetBuddyChainCount, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_SyncBuddyChain(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SyncBuddyChain, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_FireTimerById(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::FireTimerById, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_BindBuddyToPlayer(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::BindBuddyToPlayer, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_SavePlayer(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SavePlayer, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_SaveBuddy(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SaveBuddy, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_SwitchBuddy(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SwitchBuddy, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_FinishRaid(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::FinishRaid, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_StartGuildWar(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::StartGuildWar, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_EndGuildWar(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::EndGuildWar, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_StartCrossRealmGuildWar(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::StartCrossRealmGuildWar, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_EndCrossRealmGuildWar(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::EndCrossRealmGuildWar, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_ClearGuildRaid(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::ClearGuildRaid, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_StartDomainWar(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::StartDomainWar, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_EndDomainWar(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::EndDomainWar, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_SetPowerGuild(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SetPowerGuild, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_AddSkillPoint(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::AddSkillPoint, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_ChangedCareerType(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::ChangedCareerType, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_ChangedWeaponType(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::ChangedWeaponType, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_AssignTask(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::AssignTask, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_AcceptTask(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::AcceptTask, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_FinishTask(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::FinishTask, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_CancelTask(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::CancelTask, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_DeleteTask(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::DeleteTask, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_SpawnNPC(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SpawnNPC, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_SpawnNPCByCount(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SpawnNPCByCount, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_SpawnNPCDir(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SpawnNPCDir, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_TransferToTarget(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::TransferToTarget, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_SetActivateBuddy(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SetActivateBuddy, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_ReliveBuddy(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::ReliveBuddy, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_BuyBuddy(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::BuyBuddy, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_AddPet(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::AddPet, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_AddBot(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::AddBot, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_UpgradeBot(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::UpgradeBot, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_SyncBot(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SyncBot, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_BotBonus(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::BotBonus, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_BotBonusSimple(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::BotBonusSimple, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_BotBonusNormal(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::BotBonusNormal, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_BotBonusTalent(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::BotBonusTalent, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_SubScore(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SubScore, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_ReleaseAllUIOpenLevel(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::ReleaseAllUIOpenLevel, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_ClearChallengeCD(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::ClearChallengeCD, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_EnablePet(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::EnablePet, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_DisablePet(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::DisablePet, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_DecomposePet(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::DecomposePet, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_BuddyDebugMode(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::BuddyDebugMode, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_ActivateSkill(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::ActivateSkill, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_UpgradePetStarLevel(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::UpgradePetStarLevel, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_SyncPet(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SyncPet, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_AddDrop(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::AddDrop, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_SetDenyDead(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SetDenyDead, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_SetDenyBuddyDead(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SetDenyBuddyDead, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_CastSkill(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::CastSkill, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_ActBegin(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::ActBegin, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_ActGet(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::ActGet, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_ActSet(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::ActSet, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_ModifySkillUsableCount(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::ModifySkillUsableCount, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_GetSkillUsableCount(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetSkillUsableCount, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_ModifySkillMaxUsableCount(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::ModifySkillMaxUsableCount, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_GetSkillMaxUsableCount(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetSkillMaxUsableCount, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_RedisBeginData(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::RedisBeginData, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_RedisGetDataAsString(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::RedisGetDataAsString, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_RedisSetDataAsString(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::RedisSetDataAsString, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_RedisReadElement(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::RedisReadElement, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_RedisWriteElement(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::RedisWriteElement, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_RedisReadRange(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::RedisReadRange, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_RedisCreateSnapshot(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::RedisCreateSnapshot, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_BuddyDebug(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::BuddyDebug, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_EquipSlotReinforce(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::EquipSlotReinforce, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_EquipSlotStar(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::EquipSlotStar, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_RecommendFriend(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::RecommendFriend, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_GetBuyMapStepInfo(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetBuyMapStepInfo, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_BuyVIPTimes(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::BuyVIPTimes, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_QuickRaidAdvance(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::QuickRaidAdvance, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_GuildList(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GuildList, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_ExternWarehouse(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::ExternWarehouse, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_EnterTranscript(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::EnterTranscript, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_SetFlopProbability(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SetFlopProbability, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_SummonVehicle(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SummonVehicle, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_DismissVehicle(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::DismissVehicle, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_AddSystemTilte(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::AddSystemTilte, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_RemoveSystemTitle(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::RemoveSystemTitle, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_KickPlayer(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::KickPlayer, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_AddVigor(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::AddVigor, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_SetCurrentLocation(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SetCurrentLocation, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_ChangeCurrentLocation(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::ChangeCurrentLocation, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_SpawnNPCByLocation(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SpawnNPCByLocation, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_SpawnAutoFitNPC(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SpawnAutoFitNPC, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_SpawnGuildNPC(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SpawnGuildNPC, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_SpawnServerNPC(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SpawnServerNPC, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_SetInteractionValue(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SetInteractionValue, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_SetArenaExp(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SetArenaExp, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_SetArenaScore(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SetArenaScore, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_SetArenaTotalData(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SetArenaTotalData, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_SetArenaQuarterData(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SetArenaQuarterData, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_AddPKValue(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::AddPKValue, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_SetEquipEndurance(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SetEquipEndurance, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_AddShortcut(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::AddShortcut, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_DisarmTarget(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::DisarmTarget, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_ConsumeItemByIndex(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::ConsumeItemByIndex, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_ConsumeItemByID(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::ConsumeItemByID, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_SetBow(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SetBow, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_GetVIPInfo(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetVIPInfo, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_SendToPlayerConsole(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SendToPlayerConsole, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_DrawVIPGift(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::DrawVIPGift, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_SignIn(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SignIn, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_ScheduleGameStory(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::ScheduleGameStory, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_EndGameStory(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::EndGameStory, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_GMLogin(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GMLogin, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_OpenAbility(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::OpenAbility, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_FindPlayerByName(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::FindPlayerByName, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_FindPlayerByID(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::FindPlayerByID, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_OpenAllMaps(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::OpenAllMaps, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_GetServerInfo(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetServerInfo, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_Announceinfo(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::Announceinfo, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_ChangeVehicle(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::ChangeVehicle, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_SetCrossRealmServer(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SetCrossRealmServer, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_CrossRaidServerList(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::CrossRaidServerList, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_GetServerConnectTime(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetServerConnectTime, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_Closeserver(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::Closeserver, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_ExportAllRobot(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::ExportAllRobot, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_RefreshSkillCD(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::RefreshSkillCD, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_RefreshSkillIDCD(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::RefreshSkillIDCD, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_SetSkillCD(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SetSkillCD, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_SetSystemTitle(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SetSystemTitle, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_GetAchievement(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetAchievement, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_AddAchievement(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::AddAchievement, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_RemoveAchievement(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::RemoveAchievement, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_SendMailToAll(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SendMailToAll, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_ForbidTalking(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::ForbidTalking, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_AddLoginDays(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::AddLoginDays, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_SetGMCommandResult(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SetGMCommandResult, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_GetOnlineUser(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetOnlineUser, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_GetOnlineUserCount(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetOnlineUserCount, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_GetRoleInfo(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetRoleInfo, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_SendMailToOnline(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SendMailToOnline, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_SendMailToOne(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SendMailToOne, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_SendMailToOneUtf(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SendMailToOneUtf, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_AddItemToAllUtf(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::AddItemToAllUtf, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_AddItemToOneUtf(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::AddItemToOneUtf, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_RechargeMailToOneUtf(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::RechargeMailToOneUtf, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_GetItemList(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetItemList, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_GetConfigList(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetConfigList, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_GetMatchIPCharList(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetMatchIPCharList, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_GMCharLists(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GMCharLists, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_SetGmLevel(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SetGmLevel, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_GetRoleBaseInfo(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetRoleBaseInfo, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_GetRoleVehicleInfo(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetRoleVehicleInfo, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_GetRoleArrowInfo(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetRoleArrowInfo, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_GetRoleDevaeyeInfo(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetRoleDevaeyeInfo, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_GetRoleVehicleFightingInfo(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetRoleVehicleFightingInfo, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_GetRoleSectSkillInfo(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetRoleSectSkillInfo, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_GetRoleMentalSkillInfo(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetRoleMentalSkillInfo, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_GetRoleInfolist(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetRoleInfolist, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_GetRoleInfoBytype(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetRoleInfoBytype, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_GetTaskInfolistBytype(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetTaskInfolistBytype, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_AnnounceinfoUTF8(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::AnnounceinfoUTF8, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_SetFCM(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SetFCM, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_SetSubLevel(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SetSubLevel, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_ClearPack(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::ClearPack, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_GetUnionRegUserCount(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetUnionRegUserCount, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_GetUnionOnLineUserCount(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetUnionOnLineUserCount, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_GetUnionServerRecharge(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetUnionServerRecharge, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_ResetPlayerGuildRaid(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::ResetPlayerGuildRaid, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_SendGM2Server(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SendGM2Server, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_SendGM2TransServer(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SendGM2TransServer, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_SetInternal(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SetInternal, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_SetFreezeAcc(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SetFreezeAcc, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_SetFreezeRol(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SetFreezeRol, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_GetItemListByPage(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetItemListByPage, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_SendMailToOneMoney(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SendMailToOneMoney, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_ReloadCSV(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::ReloadCSV, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_ReloadStoryScript(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::ReloadStoryScript, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_PlatformInfoDirty(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::PlatformInfoDirty, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_SetServerPlatformInfo(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SetServerPlatformInfo, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_PlatformClientMsg(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::PlatformClientMsg, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_PlatformTest(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::PlatformTest, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_SetLoginPlatformInfo(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SetLoginPlatformInfo, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_DetachShield(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::DetachShield, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_AddForbidWord(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::AddForbidWord, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_DelForbidWord(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::DelForbidWord, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_ShowForbidWordList(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::ShowForbidWordList, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_ShowChatMonitorInfo(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::ShowChatMonitorInfo, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_SetChatMonitorLevel(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SetChatMonitorLevel, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_SetSilentGagTime(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SetSilentGagTime, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_DismissGuild(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::DismissGuild, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_GetGuildList(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetGuildList, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_RandomGuildName(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::RandomGuildName, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_ActivateFashion(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::ActivateFashion, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_DeactivateFashion(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::DeactivateFashion, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_AddBonusRole(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::AddBonusRole, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_RemoveBonusRole(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::RemoveBonusRole, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_ResetBonus(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::ResetBonus, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_ClearAreaKeyMonster(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::ClearAreaKeyMonster, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_SendItemToRole(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SendItemToRole, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_DumpScore(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::DumpScore, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_DumpBuddyScore(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::DumpBuddyScore, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_Act360PriviTest(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::Act360PriviTest, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_Set360PriviGMTest(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::Set360PriviGMTest, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_GainDefenderGift(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GainDefenderGift, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_GetResourceAlertConfig(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetResourceAlertConfig, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_UpdateResourceAlertConfig(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::UpdateResourceAlertConfig, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_SetPreActive(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SetPreActive, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_LoadUpdateBulletin(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::LoadUpdateBulletin, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_SendFBReward(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SendFBReward, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_DebugCommand(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::DebugCommand, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_HandleAsset(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::HandleAsset, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_BoneSoulTrain(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::BoneSoulTrain, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_SetGameStoryRealDay(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SetGameStoryRealDay, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_AddMonopolyCount(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::AddMonopolyCount, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_AddRuneCream(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::AddRuneCream, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_RuneUpgrade(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::RuneUpgrade, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_RuneStar(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::RuneStar, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_ResetRune(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::ResetRune, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_EquipRune(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::EquipRune, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_UnEquipRune(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::UnEquipRune, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_AddStarSoul(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::AddStarSoul, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_DecompositionItem(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::DecompositionItem, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_AddPentacle(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::AddPentacle, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_BiographyPartReward(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::BiographyPartReward, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_Charge(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::Charge, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_MainTaskReachReward(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::MainTaskReachReward, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_ResetShopTimes(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::ResetShopTimes, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_GetShopResetTimes(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetShopResetTimes, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_SetDecompositionSetting(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SetDecompositionSetting, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_AddMoneyCourage(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::AddMoneyCourage, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_AddEnergy(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::AddEnergy, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_GetStarVeinGoalReward(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetStarVeinGoalReward, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_ActivateSub(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::ActivateSub, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_CastBuddySkillBySkillIndex(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::CastBuddySkillBySkillIndex, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_TurnPlayer(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::TurnPlayer, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_RuneReward(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::RuneReward, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_GetShopItem(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetShopItem, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_BuyInvestmentCard(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::BuyInvestmentCard, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_AddInvestmentCardReward(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::AddInvestmentCardReward, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_OneKeyEquipRune(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::OneKeyEquipRune, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_OneKeyUnEquipRune(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::OneKeyUnEquipRune, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_RefreshRewardTask(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::RefreshRewardTask, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_SetBuddyRuneSlotState(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SetBuddyRuneSlotState, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_OpenPetAbility(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::OpenPetAbility, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_WriteCharbaseTest(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::WriteCharbaseTest, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_WriteRaidDataTest(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::WriteRaidDataTest, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_SetRankTransferFailed(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SetRankTransferFailed, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_AddRankRedisData(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::AddRankRedisData, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_ClearRankRedisData(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::ClearRankRedisData, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_GetRankRedisData(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetRankRedisData, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_TransferMap(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::TransferMap, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_ReqPlayerDetail(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::ReqPlayerDetail, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_CommitAnswer(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::CommitAnswer, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_GetAnswerRankInfo(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetAnswerRankInfo, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_ExportSoloDuleRobot(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::ExportSoloDuleRobot, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_WheelFightReq(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::WheelFightReq, L, 2);
		return 0;
	}		
	static int bnd_SC_ServerDebugInterface_GetNearbyPlayer(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetNearbyPlayer, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_GetRankSubBInfo(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetRankSubBInfo, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_GetYiZhongQinXiDamageData(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetYiZhongQinXiDamageData, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_GetYiZhongQinXiDetailDamageData(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetYiZhongQinXiDetailDamageData, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_UpgradeCommanderTitle(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::UpgradeCommanderTitle, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_GetChapterRaidStarReward(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetChapterRaidStarReward, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_QuickRaidNew(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::QuickRaidNew, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_TestEquipGradeLevel(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::TestEquipGradeLevel, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_ControllerEquipSlotReinforce(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::ControllerEquipSlotReinforce, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_ControllerEquipSlotRefine(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::ControllerEquipSlotRefine, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_EquipItemInPackage(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::EquipItemInPackage, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_UnEquipItemInEquipSlot(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::UnEquipItemInEquipSlot, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_GetOtherOfflineInfo (lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::GetOtherOfflineInfo , L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_TestGuildFight(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::TestGuildFight, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_TestRobScore(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::TestRobScore, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_TestGuildMemberPointInfo(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::TestGuildMemberPointInfo, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_JumpTask(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::JumpTask, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_TestCombatScore(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::TestCombatScore, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_AddAuction(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::AddAuction, L, 2);
		return 1;
	}
	static int bnd_SC_ServerDebugInterface_SetSkyArenaScore(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		SC_ServerDebugInterface* pkAPI = (SC_ServerDebugInterface*)binder.CheckUserType(1, "SC_ServerDebugInterface");
		Utility::LuaCall(*pkAPI, &SC_ServerDebugInterface::SetSkyArenaScore, L, 2);
		return 1;
	}
	static const luaL_reg lib_SC_ServerDebugInterface[] = {
	{"Create", bnd_LuaCreate_SC_ServerDebugInterface},
	{"SendToDebugConsole", bnd_SC_ServerDebugInterface_SendToDebugConsole},
	{"LogError", bnd_SC_ServerDebugInterface_LogError},
	{"GetCurrentTimestamp", bnd_SC_ServerDebugInterface_GetCurrentTimestamp},
	{"GetServerID", bnd_SC_ServerDebugInterface_GetServerID},
	{"GetServerStartTime", bnd_SC_ServerDebugInterface_GetServerStartTime},
	{"GetBackPackTypeID", bnd_SC_ServerDebugInterface_GetBackPackTypeID},
	{"FindOwerType", bnd_SC_ServerDebugInterface_FindOwerType},
	{"SetState", bnd_SC_ServerDebugInterface_SetState},
	{"GetAttributeMap", bnd_SC_ServerDebugInterface_GetAttributeMap},
	{"NPCScript", bnd_SC_ServerDebugInterface_NPCScript},
	{"GetServerPlayerNum", bnd_SC_ServerDebugInterface_GetServerPlayerNum},
	{"GetPlayerID", bnd_SC_ServerDebugInterface_GetPlayerID},
	{"GetServerNPCNum", bnd_SC_ServerDebugInterface_GetServerNPCNum},
	{"GetMapPlayerCount", bnd_SC_ServerDebugInterface_GetMapPlayerCount},
	{"GetNPCID", bnd_SC_ServerDebugInterface_GetNPCID},
	{"GetNPCShopFuncID", bnd_SC_ServerDebugInterface_GetNPCShopFuncID},
	{"SetListeningFlag", bnd_SC_ServerDebugInterface_SetListeningFlag},
	{"SetHP", bnd_SC_ServerDebugInterface_SetHP},
	{"SetSkillLevelupPoints", bnd_SC_ServerDebugInterface_SetSkillLevelupPoints},
	{"SetOwnerMaxMP", bnd_SC_ServerDebugInterface_SetOwnerMaxMP},
	{"ModifyOwnerMaxMP", bnd_SC_ServerDebugInterface_ModifyOwnerMaxMP},
	{"SetOwnerMP", bnd_SC_ServerDebugInterface_SetOwnerMP},
	{"ModifyOwnerMP", bnd_SC_ServerDebugInterface_ModifyOwnerMP},
	{"SyncOwnerMP", bnd_SC_ServerDebugInterface_SyncOwnerMP},
	{"SetMP", bnd_SC_ServerDebugInterface_SetMP},
	{"SetCombatRecorderEnabled", bnd_SC_ServerDebugInterface_SetCombatRecorderEnabled},
	{"FinishCombatRecorder", bnd_SC_ServerDebugInterface_FinishCombatRecorder},
	{"AddSkill", bnd_SC_ServerDebugInterface_AddSkill},
	{"RemoveSkill", bnd_SC_ServerDebugInterface_RemoveSkill},
	{"RemoveAllSkill", bnd_SC_ServerDebugInterface_RemoveAllSkill},
	{"AddSkillBook", bnd_SC_ServerDebugInterface_AddSkillBook},
	{"RemoveSkillBook", bnd_SC_ServerDebugInterface_RemoveSkillBook},
	{"SkillBookNoteSoul", bnd_SC_ServerDebugInterface_SkillBookNoteSoul},
	{"SkillBookUpgrade", bnd_SC_ServerDebugInterface_SkillBookUpgrade},
	{"QueryUserChallenge", bnd_SC_ServerDebugInterface_QueryUserChallenge},
	{"QueryRankUserInfo", bnd_SC_ServerDebugInterface_QueryRankUserInfo},
	{"ActivateStarVein", bnd_SC_ServerDebugInterface_ActivateStarVein},
	{"GetFDRewardTime", bnd_SC_ServerDebugInterface_GetFDRewardTime},
	{"GetFDFreeReward", bnd_SC_ServerDebugInterface_GetFDFreeReward},
	{"GetFDDisCountReward", bnd_SC_ServerDebugInterface_GetFDDisCountReward},
	{"UpgradeSubordinate", bnd_SC_ServerDebugInterface_UpgradeSubordinate},
	{"BuyBuddyGrowthPackage", bnd_SC_ServerDebugInterface_BuyBuddyGrowthPackage},
	{"GetBuddyGrowthReward", bnd_SC_ServerDebugInterface_GetBuddyGrowthReward},
	{"CollectionActivation", bnd_SC_ServerDebugInterface_CollectionActivation},
	{"CollectionReward", bnd_SC_ServerDebugInterface_CollectionReward},
	{"GainOfflineEXP", bnd_SC_ServerDebugInterface_GainOfflineEXP},
	{"LeaveGame", bnd_SC_ServerDebugInterface_LeaveGame},
	{"Console", bnd_SC_ServerDebugInterface_Console},
	{"ActivateSkillBook", bnd_SC_ServerDebugInterface_ActivateSkillBook},
	{"UnlockSkillBookSlot", bnd_SC_ServerDebugInterface_UnlockSkillBookSlot},
	{"WearSkillBook", bnd_SC_ServerDebugInterface_WearSkillBook},
	{"ReliveBeforeEnterMap", bnd_SC_ServerDebugInterface_ReliveBeforeEnterMap},
	{"TakeChapterReward", bnd_SC_ServerDebugInterface_TakeChapterReward},
	{"BuyItem", bnd_SC_ServerDebugInterface_BuyItem},
	{"AddItem", bnd_SC_ServerDebugInterface_AddItem},
	{"GetActor", bnd_SC_ServerDebugInterface_GetActor},
	{"ReloadSkillScript", bnd_SC_ServerDebugInterface_ReloadSkillScript},
	{"AddCash", bnd_SC_ServerDebugInterface_AddCash},
	{"AddBindCash", bnd_SC_ServerDebugInterface_AddBindCash},
	{"AddUnBindCash", bnd_SC_ServerDebugInterface_AddUnBindCash},
	{"AddCouragePoint", bnd_SC_ServerDebugInterface_AddCouragePoint},
	{"BroadcastMsg", bnd_SC_ServerDebugInterface_BroadcastMsg},
	{"AddJusticePoint", bnd_SC_ServerDebugInterface_AddJusticePoint},
	{"AddExp", bnd_SC_ServerDebugInterface_AddExp},
	{"AddExpToBuddy", bnd_SC_ServerDebugInterface_AddExpToBuddy},
	{"AddExpToActivateBuddy", bnd_SC_ServerDebugInterface_AddExpToActivateBuddy},
	{"AddExpToAllBuddy", bnd_SC_ServerDebugInterface_AddExpToAllBuddy},
	{"SetBaseDataStr", bnd_SC_ServerDebugInterface_SetBaseDataStr},
	{"EchoBaseDataStr", bnd_SC_ServerDebugInterface_EchoBaseDataStr},
	{"SetBaseDataInt", bnd_SC_ServerDebugInterface_SetBaseDataInt},
	{"EchoBaseDataInt", bnd_SC_ServerDebugInterface_EchoBaseDataInt},
	{"AddLevel", bnd_SC_ServerDebugInterface_AddLevel},
	{"GMDead", bnd_SC_ServerDebugInterface_GMDead},
	{"UpgradeBuddyStar", bnd_SC_ServerDebugInterface_UpgradeBuddyStar},
	{"AddBuddyLevel", bnd_SC_ServerDebugInterface_AddBuddyLevel},
	{"RegisterCombatEvent", bnd_SC_ServerDebugInterface_RegisterCombatEvent},
	{"UnregisterCombatEvent", bnd_SC_ServerDebugInterface_UnregisterCombatEvent},
	{"BuddyLevelUp", bnd_SC_ServerDebugInterface_BuddyLevelUp},
	{"SwitchLine", bnd_SC_ServerDebugInterface_SwitchLine},
	{"GuildOfflineSolo", bnd_SC_ServerDebugInterface_GuildOfflineSolo},
	{"SetNewVIPLevel", bnd_SC_ServerDebugInterface_SetNewVIPLevel},
	{"SetBuddySchemeData", bnd_SC_ServerDebugInterface_SetBuddySchemeData},
	{"SwitchToBuddyScheme", bnd_SC_ServerDebugInterface_SwitchToBuddyScheme},
	{"ResetBuddyScheme", bnd_SC_ServerDebugInterface_ResetBuddyScheme},
	{"AddBuddyChain", bnd_SC_ServerDebugInterface_AddBuddyChain},
	{"DelBuddyChain", bnd_SC_ServerDebugInterface_DelBuddyChain},
	{"SwitchBuddyChain", bnd_SC_ServerDebugInterface_SwitchBuddyChain},
	{"SetBuddyChainCount", bnd_SC_ServerDebugInterface_SetBuddyChainCount},
	{"SyncBuddyChain", bnd_SC_ServerDebugInterface_SyncBuddyChain},
	{"FireTimerById", bnd_SC_ServerDebugInterface_FireTimerById},
	{"BindBuddyToPlayer", bnd_SC_ServerDebugInterface_BindBuddyToPlayer},
	{"SavePlayer", bnd_SC_ServerDebugInterface_SavePlayer},
	{"SaveBuddy", bnd_SC_ServerDebugInterface_SaveBuddy},
	{"SwitchBuddy", bnd_SC_ServerDebugInterface_SwitchBuddy},
	{"FinishRaid", bnd_SC_ServerDebugInterface_FinishRaid},
	{"StartGuildWar", bnd_SC_ServerDebugInterface_StartGuildWar},
	{"EndGuildWar", bnd_SC_ServerDebugInterface_EndGuildWar},
	{"StartCrossRealmGuildWar", bnd_SC_ServerDebugInterface_StartCrossRealmGuildWar},
	{"EndCrossRealmGuildWar", bnd_SC_ServerDebugInterface_EndCrossRealmGuildWar},
	{"ClearGuildRaid", bnd_SC_ServerDebugInterface_ClearGuildRaid},
	{"StartDomainWar", bnd_SC_ServerDebugInterface_StartDomainWar},
	{"EndDomainWar", bnd_SC_ServerDebugInterface_EndDomainWar},
	{"SetPowerGuild", bnd_SC_ServerDebugInterface_SetPowerGuild},
	{"AddSkillPoint", bnd_SC_ServerDebugInterface_AddSkillPoint},
	{"ChangedCareerType", bnd_SC_ServerDebugInterface_ChangedCareerType},
	{"ChangedWeaponType", bnd_SC_ServerDebugInterface_ChangedWeaponType},
	{"AssignTask", bnd_SC_ServerDebugInterface_AssignTask},
	{"AcceptTask", bnd_SC_ServerDebugInterface_AcceptTask},
	{"FinishTask", bnd_SC_ServerDebugInterface_FinishTask},
	{"CancelTask", bnd_SC_ServerDebugInterface_CancelTask},
	{"DeleteTask", bnd_SC_ServerDebugInterface_DeleteTask},
	{"SpawnNPC", bnd_SC_ServerDebugInterface_SpawnNPC},
	{"SpawnNPCByCount", bnd_SC_ServerDebugInterface_SpawnNPCByCount},
	{"SpawnNPCDir", bnd_SC_ServerDebugInterface_SpawnNPCDir},
	{"TransferToTarget", bnd_SC_ServerDebugInterface_TransferToTarget},
	{"SetActivateBuddy", bnd_SC_ServerDebugInterface_SetActivateBuddy},
	{"ReliveBuddy", bnd_SC_ServerDebugInterface_ReliveBuddy},
	{"BuyBuddy", bnd_SC_ServerDebugInterface_BuyBuddy},
	{"AddPet", bnd_SC_ServerDebugInterface_AddPet},
	{"AddBot", bnd_SC_ServerDebugInterface_AddBot},
	{"UpgradeBot", bnd_SC_ServerDebugInterface_UpgradeBot},
	{"SyncBot", bnd_SC_ServerDebugInterface_SyncBot},
	{"BotBonus", bnd_SC_ServerDebugInterface_BotBonus},
	{"BotBonusSimple", bnd_SC_ServerDebugInterface_BotBonusSimple},
	{"BotBonusNormal", bnd_SC_ServerDebugInterface_BotBonusNormal},
	{"BotBonusTalent", bnd_SC_ServerDebugInterface_BotBonusTalent},
	{"SubScore", bnd_SC_ServerDebugInterface_SubScore},
	{"ReleaseAllUIOpenLevel", bnd_SC_ServerDebugInterface_ReleaseAllUIOpenLevel},
	{"ClearChallengeCD", bnd_SC_ServerDebugInterface_ClearChallengeCD},
	{"EnablePet", bnd_SC_ServerDebugInterface_EnablePet},
	{"DisablePet", bnd_SC_ServerDebugInterface_DisablePet},
	{"DecomposePet", bnd_SC_ServerDebugInterface_DecomposePet},
	{"BuddyDebugMode", bnd_SC_ServerDebugInterface_BuddyDebugMode},
	{"ActivateSkill", bnd_SC_ServerDebugInterface_ActivateSkill},
	{"UpgradePetStarLevel", bnd_SC_ServerDebugInterface_UpgradePetStarLevel},
	{"SyncPet", bnd_SC_ServerDebugInterface_SyncPet},
	{"AddDrop", bnd_SC_ServerDebugInterface_AddDrop},
	{"SetDenyDead", bnd_SC_ServerDebugInterface_SetDenyDead},
	{"SetDenyBuddyDead", bnd_SC_ServerDebugInterface_SetDenyBuddyDead},
	{"CastSkill", bnd_SC_ServerDebugInterface_CastSkill},
	{"ActBegin", bnd_SC_ServerDebugInterface_ActBegin},
	{"ActGet", bnd_SC_ServerDebugInterface_ActGet},
	{"ActSet", bnd_SC_ServerDebugInterface_ActSet},
	{"ModifySkillUsableCount", bnd_SC_ServerDebugInterface_ModifySkillUsableCount},
	{"GetSkillUsableCount", bnd_SC_ServerDebugInterface_GetSkillUsableCount},
	{"ModifySkillMaxUsableCount", bnd_SC_ServerDebugInterface_ModifySkillMaxUsableCount},
	{"GetSkillMaxUsableCount", bnd_SC_ServerDebugInterface_GetSkillMaxUsableCount},
	{"RedisBeginData", bnd_SC_ServerDebugInterface_RedisBeginData},
	{"RedisGetDataAsString", bnd_SC_ServerDebugInterface_RedisGetDataAsString},
	{"RedisSetDataAsString", bnd_SC_ServerDebugInterface_RedisSetDataAsString},
	{"RedisReadElement", bnd_SC_ServerDebugInterface_RedisReadElement},
	{"RedisWriteElement", bnd_SC_ServerDebugInterface_RedisWriteElement},
	{"RedisReadRange", bnd_SC_ServerDebugInterface_RedisReadRange},
	{"RedisCreateSnapshot", bnd_SC_ServerDebugInterface_RedisCreateSnapshot},
	{"BuddyDebug", bnd_SC_ServerDebugInterface_BuddyDebug},
	{"EquipSlotReinforce", bnd_SC_ServerDebugInterface_EquipSlotReinforce},
	{"EquipSlotStar", bnd_SC_ServerDebugInterface_EquipSlotStar},
	{"RecommendFriend", bnd_SC_ServerDebugInterface_RecommendFriend},
	{"GetBuyMapStepInfo", bnd_SC_ServerDebugInterface_GetBuyMapStepInfo},
	{"BuyVIPTimes", bnd_SC_ServerDebugInterface_BuyVIPTimes},
	{"QuickRaidAdvance", bnd_SC_ServerDebugInterface_QuickRaidAdvance},
	{"GuildList", bnd_SC_ServerDebugInterface_GuildList},
	{"ExternWarehouse", bnd_SC_ServerDebugInterface_ExternWarehouse},
	{"EnterTranscript", bnd_SC_ServerDebugInterface_EnterTranscript},
	{"SetFlopProbability", bnd_SC_ServerDebugInterface_SetFlopProbability},
	{"SummonVehicle", bnd_SC_ServerDebugInterface_SummonVehicle},
	{"DismissVehicle", bnd_SC_ServerDebugInterface_DismissVehicle},
	{"AddSystemTilte", bnd_SC_ServerDebugInterface_AddSystemTilte},
	{"RemoveSystemTitle", bnd_SC_ServerDebugInterface_RemoveSystemTitle},
	{"KickPlayer", bnd_SC_ServerDebugInterface_KickPlayer},
	{"AddVigor", bnd_SC_ServerDebugInterface_AddVigor},
	{"SetCurrentLocation", bnd_SC_ServerDebugInterface_SetCurrentLocation},
	{"ChangeCurrentLocation", bnd_SC_ServerDebugInterface_ChangeCurrentLocation},
	{"SpawnNPCByLocation", bnd_SC_ServerDebugInterface_SpawnNPCByLocation},
	{"SpawnAutoFitNPC", bnd_SC_ServerDebugInterface_SpawnAutoFitNPC},
	{"SpawnGuildNPC", bnd_SC_ServerDebugInterface_SpawnGuildNPC},
	{"SpawnServerNPC", bnd_SC_ServerDebugInterface_SpawnServerNPC},
	{"SetInteractionValue", bnd_SC_ServerDebugInterface_SetInteractionValue},
	{"SetArenaExp", bnd_SC_ServerDebugInterface_SetArenaExp},
	{"SetArenaScore", bnd_SC_ServerDebugInterface_SetArenaScore},
	{"SetArenaTotalData", bnd_SC_ServerDebugInterface_SetArenaTotalData},
	{"SetArenaQuarterData", bnd_SC_ServerDebugInterface_SetArenaQuarterData},
	{"AddPKValue", bnd_SC_ServerDebugInterface_AddPKValue},
	{"SetEquipEndurance", bnd_SC_ServerDebugInterface_SetEquipEndurance},
	{"AddShortcut", bnd_SC_ServerDebugInterface_AddShortcut},
	{"DisarmTarget", bnd_SC_ServerDebugInterface_DisarmTarget},
	{"ConsumeItemByIndex", bnd_SC_ServerDebugInterface_ConsumeItemByIndex},
	{"ConsumeItemByID", bnd_SC_ServerDebugInterface_ConsumeItemByID},
	{"SetBow", bnd_SC_ServerDebugInterface_SetBow},
	{"GetVIPInfo", bnd_SC_ServerDebugInterface_GetVIPInfo},
	{"SendToPlayerConsole", bnd_SC_ServerDebugInterface_SendToPlayerConsole},
	{"DrawVIPGift", bnd_SC_ServerDebugInterface_DrawVIPGift},
	{"SignIn", bnd_SC_ServerDebugInterface_SignIn},
	{"ScheduleGameStory", bnd_SC_ServerDebugInterface_ScheduleGameStory},
	{"EndGameStory", bnd_SC_ServerDebugInterface_EndGameStory},
	{"GMLogin", bnd_SC_ServerDebugInterface_GMLogin},
	{"OpenAbility", bnd_SC_ServerDebugInterface_OpenAbility},
	{"FindPlayerByName", bnd_SC_ServerDebugInterface_FindPlayerByName},
	{"FindPlayerByID", bnd_SC_ServerDebugInterface_FindPlayerByID},
	{"OpenAllMaps", bnd_SC_ServerDebugInterface_OpenAllMaps},
	{"GetServerInfo", bnd_SC_ServerDebugInterface_GetServerInfo},
	{"Announceinfo", bnd_SC_ServerDebugInterface_Announceinfo},
	{"ChangeVehicle", bnd_SC_ServerDebugInterface_ChangeVehicle},
	{"SetCrossRealmServer", bnd_SC_ServerDebugInterface_SetCrossRealmServer},
	{"CrossRaidServerList", bnd_SC_ServerDebugInterface_CrossRaidServerList},
	{"GetServerConnectTime", bnd_SC_ServerDebugInterface_GetServerConnectTime},
	{"Closeserver", bnd_SC_ServerDebugInterface_Closeserver},
	{"ExportAllRobot", bnd_SC_ServerDebugInterface_ExportAllRobot},
	{"RefreshSkillCD", bnd_SC_ServerDebugInterface_RefreshSkillCD},
	{"RefreshSkillIDCD", bnd_SC_ServerDebugInterface_RefreshSkillIDCD},
	{"SetSkillCD", bnd_SC_ServerDebugInterface_SetSkillCD},
	{"SetSystemTitle", bnd_SC_ServerDebugInterface_SetSystemTitle},
	{"GetAchievement", bnd_SC_ServerDebugInterface_GetAchievement},
	{"AddAchievement", bnd_SC_ServerDebugInterface_AddAchievement},
	{"RemoveAchievement", bnd_SC_ServerDebugInterface_RemoveAchievement},
	{"SendMailToAll", bnd_SC_ServerDebugInterface_SendMailToAll},
	{"ForbidTalking", bnd_SC_ServerDebugInterface_ForbidTalking},
	{"AddLoginDays", bnd_SC_ServerDebugInterface_AddLoginDays},
	{"SetGMCommandResult", bnd_SC_ServerDebugInterface_SetGMCommandResult},
	{"GetOnlineUser", bnd_SC_ServerDebugInterface_GetOnlineUser},
	{"GetOnlineUserCount", bnd_SC_ServerDebugInterface_GetOnlineUserCount},
	{"GetRoleInfo", bnd_SC_ServerDebugInterface_GetRoleInfo},
	{"SendMailToOnline", bnd_SC_ServerDebugInterface_SendMailToOnline},
	{"SendMailToOne", bnd_SC_ServerDebugInterface_SendMailToOne},
	{"SendMailToOneUtf", bnd_SC_ServerDebugInterface_SendMailToOneUtf},
	{"AddItemToAllUtf", bnd_SC_ServerDebugInterface_AddItemToAllUtf},
	{"AddItemToOneUtf", bnd_SC_ServerDebugInterface_AddItemToOneUtf},
	{"RechargeMailToOneUtf", bnd_SC_ServerDebugInterface_RechargeMailToOneUtf},
	{"GetItemList", bnd_SC_ServerDebugInterface_GetItemList},
	{"GetConfigList", bnd_SC_ServerDebugInterface_GetConfigList},
	{"GetMatchIPCharList", bnd_SC_ServerDebugInterface_GetMatchIPCharList},
	{"GMCharLists", bnd_SC_ServerDebugInterface_GMCharLists},
	{"SetGmLevel", bnd_SC_ServerDebugInterface_SetGmLevel},
	{"GetRoleBaseInfo", bnd_SC_ServerDebugInterface_GetRoleBaseInfo},
	{"GetRoleVehicleInfo", bnd_SC_ServerDebugInterface_GetRoleVehicleInfo},
	{"GetRoleArrowInfo", bnd_SC_ServerDebugInterface_GetRoleArrowInfo},
	{"GetRoleDevaeyeInfo", bnd_SC_ServerDebugInterface_GetRoleDevaeyeInfo},
	{"GetRoleVehicleFightingInfo", bnd_SC_ServerDebugInterface_GetRoleVehicleFightingInfo},
	{"GetRoleSectSkillInfo", bnd_SC_ServerDebugInterface_GetRoleSectSkillInfo},
	{"GetRoleMentalSkillInfo", bnd_SC_ServerDebugInterface_GetRoleMentalSkillInfo},
	{"GetRoleInfolist", bnd_SC_ServerDebugInterface_GetRoleInfolist},
	{"GetRoleInfoBytype", bnd_SC_ServerDebugInterface_GetRoleInfoBytype},
	{"GetTaskInfolistBytype", bnd_SC_ServerDebugInterface_GetTaskInfolistBytype},
	{"AnnounceinfoUTF8", bnd_SC_ServerDebugInterface_AnnounceinfoUTF8},
	{"SetFCM", bnd_SC_ServerDebugInterface_SetFCM},
	{"SetSubLevel", bnd_SC_ServerDebugInterface_SetSubLevel},
	{"ClearPack", bnd_SC_ServerDebugInterface_ClearPack},
	{"GetUnionRegUserCount", bnd_SC_ServerDebugInterface_GetUnionRegUserCount},
	{"GetUnionOnLineUserCount", bnd_SC_ServerDebugInterface_GetUnionOnLineUserCount},
	{"GetUnionServerRecharge", bnd_SC_ServerDebugInterface_GetUnionServerRecharge},
	{"ResetPlayerGuildRaid", bnd_SC_ServerDebugInterface_ResetPlayerGuildRaid},
	{"SendGM2Server", bnd_SC_ServerDebugInterface_SendGM2Server},
	{"SendGM2TransServer", bnd_SC_ServerDebugInterface_SendGM2TransServer},
	{"SetInternal", bnd_SC_ServerDebugInterface_SetInternal},
	{"SetFreezeAcc", bnd_SC_ServerDebugInterface_SetFreezeAcc},
	{"SetFreezeRol", bnd_SC_ServerDebugInterface_SetFreezeRol},
	{"GetItemListByPage", bnd_SC_ServerDebugInterface_GetItemListByPage},
	{"SendMailToOneMoney", bnd_SC_ServerDebugInterface_SendMailToOneMoney},
	{"ReloadCSV", bnd_SC_ServerDebugInterface_ReloadCSV},
	{"ReloadStoryScript", bnd_SC_ServerDebugInterface_ReloadStoryScript},
	{"PlatformInfoDirty", bnd_SC_ServerDebugInterface_PlatformInfoDirty},
	{"SetServerPlatformInfo", bnd_SC_ServerDebugInterface_SetServerPlatformInfo},
	{"PlatformClientMsg", bnd_SC_ServerDebugInterface_PlatformClientMsg},
	{"PlatformTest", bnd_SC_ServerDebugInterface_PlatformTest},
	{"SetLoginPlatformInfo", bnd_SC_ServerDebugInterface_SetLoginPlatformInfo},
	{"DetachShield", bnd_SC_ServerDebugInterface_DetachShield},
	{"AddForbidWord", bnd_SC_ServerDebugInterface_AddForbidWord},
	{"DelForbidWord", bnd_SC_ServerDebugInterface_DelForbidWord},
	{"ShowForbidWordList", bnd_SC_ServerDebugInterface_ShowForbidWordList},
	{"ShowChatMonitorInfo", bnd_SC_ServerDebugInterface_ShowChatMonitorInfo},
	{"SetChatMonitorLevel", bnd_SC_ServerDebugInterface_SetChatMonitorLevel},
	{"SetSilentGagTime", bnd_SC_ServerDebugInterface_SetSilentGagTime},
	{"DismissGuild", bnd_SC_ServerDebugInterface_DismissGuild},
	{"GetGuildList", bnd_SC_ServerDebugInterface_GetGuildList},
	{"RandomGuildName", bnd_SC_ServerDebugInterface_RandomGuildName},
	{"ActivateFashion", bnd_SC_ServerDebugInterface_ActivateFashion},
	{"DeactivateFashion", bnd_SC_ServerDebugInterface_DeactivateFashion},
	{"AddBonusRole", bnd_SC_ServerDebugInterface_AddBonusRole},
	{"RemoveBonusRole", bnd_SC_ServerDebugInterface_RemoveBonusRole},
	{"ResetBonus", bnd_SC_ServerDebugInterface_ResetBonus},
	{"ClearAreaKeyMonster", bnd_SC_ServerDebugInterface_ClearAreaKeyMonster},
	{"SendItemToRole", bnd_SC_ServerDebugInterface_SendItemToRole},
	{"DumpScore", bnd_SC_ServerDebugInterface_DumpScore},
	{"DumpBuddyScore", bnd_SC_ServerDebugInterface_DumpBuddyScore},
	{"Act360PriviTest", bnd_SC_ServerDebugInterface_Act360PriviTest},
	{"Set360PriviGMTest", bnd_SC_ServerDebugInterface_Set360PriviGMTest},
	{"GainDefenderGift", bnd_SC_ServerDebugInterface_GainDefenderGift},
	{"GetResourceAlertConfig", bnd_SC_ServerDebugInterface_GetResourceAlertConfig},
	{"UpdateResourceAlertConfig", bnd_SC_ServerDebugInterface_UpdateResourceAlertConfig},
	{"SetPreActive", bnd_SC_ServerDebugInterface_SetPreActive},
	{"LoadUpdateBulletin", bnd_SC_ServerDebugInterface_LoadUpdateBulletin},
	{"SendFBReward", bnd_SC_ServerDebugInterface_SendFBReward},
	{"DebugCommand", bnd_SC_ServerDebugInterface_DebugCommand},
	{"HandleAsset", bnd_SC_ServerDebugInterface_HandleAsset},
	{"BoneSoulTrain", bnd_SC_ServerDebugInterface_BoneSoulTrain},
	{"SetGameStoryRealDay", bnd_SC_ServerDebugInterface_SetGameStoryRealDay},
	{"AddMonopolyCount", bnd_SC_ServerDebugInterface_AddMonopolyCount},
	{"AddRuneCream", bnd_SC_ServerDebugInterface_AddRuneCream},
	{"RuneUpgrade", bnd_SC_ServerDebugInterface_RuneUpgrade},
	{"RuneStar", bnd_SC_ServerDebugInterface_RuneStar},
	{"ResetRune", bnd_SC_ServerDebugInterface_ResetRune},
	{"EquipRune", bnd_SC_ServerDebugInterface_EquipRune},
	{"UnEquipRune", bnd_SC_ServerDebugInterface_UnEquipRune},
	{"AddStarSoul", bnd_SC_ServerDebugInterface_AddStarSoul},
	{"DecompositionItem", bnd_SC_ServerDebugInterface_DecompositionItem},
	{"AddPentacle", bnd_SC_ServerDebugInterface_AddPentacle},
	{"BiographyPartReward", bnd_SC_ServerDebugInterface_BiographyPartReward},
	{"Charge", bnd_SC_ServerDebugInterface_Charge},
	{"MainTaskReachReward", bnd_SC_ServerDebugInterface_MainTaskReachReward},
	{"ResetShopTimes", bnd_SC_ServerDebugInterface_ResetShopTimes},
	{"GetShopResetTimes", bnd_SC_ServerDebugInterface_GetShopResetTimes},
	{"SetDecompositionSetting", bnd_SC_ServerDebugInterface_SetDecompositionSetting},
	{"AddMoneyCourage", bnd_SC_ServerDebugInterface_AddMoneyCourage},
	{"AddEnergy", bnd_SC_ServerDebugInterface_AddEnergy},
	{"GetStarVeinGoalReward", bnd_SC_ServerDebugInterface_GetStarVeinGoalReward},
	{"ActivateSub", bnd_SC_ServerDebugInterface_ActivateSub},
	{"CastBuddySkillBySkillIndex", bnd_SC_ServerDebugInterface_CastBuddySkillBySkillIndex},
	{"TurnPlayer", bnd_SC_ServerDebugInterface_TurnPlayer},
	{"RuneReward", bnd_SC_ServerDebugInterface_RuneReward},
	{"GetShopItem", bnd_SC_ServerDebugInterface_GetShopItem},
	{"BuyInvestmentCard", bnd_SC_ServerDebugInterface_BuyInvestmentCard},
	{"AddInvestmentCardReward", bnd_SC_ServerDebugInterface_AddInvestmentCardReward},
	{"OneKeyEquipRune", bnd_SC_ServerDebugInterface_OneKeyEquipRune},
	{"OneKeyUnEquipRune", bnd_SC_ServerDebugInterface_OneKeyUnEquipRune},
	{"RefreshRewardTask", bnd_SC_ServerDebugInterface_RefreshRewardTask},
	{"SetBuddyRuneSlotState", bnd_SC_ServerDebugInterface_SetBuddyRuneSlotState},
	{"OpenPetAbility", bnd_SC_ServerDebugInterface_OpenPetAbility},
	{"WriteCharbaseTest", bnd_SC_ServerDebugInterface_WriteCharbaseTest},
	{"WriteRaidDataTest", bnd_SC_ServerDebugInterface_WriteRaidDataTest},
	{"SetRankTransferFailed", bnd_SC_ServerDebugInterface_SetRankTransferFailed},
	{"AddRankRedisData", bnd_SC_ServerDebugInterface_AddRankRedisData},
	{"ClearRankRedisData", bnd_SC_ServerDebugInterface_ClearRankRedisData},
	{"GetRankRedisData", bnd_SC_ServerDebugInterface_GetRankRedisData},
	{"TransferMap", bnd_SC_ServerDebugInterface_TransferMap},
	{"ReqPlayerDetail", bnd_SC_ServerDebugInterface_ReqPlayerDetail},
	{"CommitAnswer", bnd_SC_ServerDebugInterface_CommitAnswer},
	{"GetAnswerRankInfo", bnd_SC_ServerDebugInterface_GetAnswerRankInfo},
	{"ExportSoloDuleRobot", bnd_SC_ServerDebugInterface_ExportSoloDuleRobot},
	{"WheelFightReq", bnd_SC_ServerDebugInterface_WheelFightReq},
	{"GetNearbyPlayer", bnd_SC_ServerDebugInterface_GetNearbyPlayer},
	{"GetRankSubBInfo", bnd_SC_ServerDebugInterface_GetRankSubBInfo},
	{"GetYiZhongQinXiDamageData", bnd_SC_ServerDebugInterface_GetYiZhongQinXiDamageData},
	{"GetYiZhongQinXiDetailDamageData", bnd_SC_ServerDebugInterface_GetYiZhongQinXiDetailDamageData},
	{"UpgradeCommanderTitle", bnd_SC_ServerDebugInterface_UpgradeCommanderTitle},
	{"GetChapterRaidStarReward", bnd_SC_ServerDebugInterface_GetChapterRaidStarReward},
	{"QuickRaidNew", bnd_SC_ServerDebugInterface_QuickRaidNew},
	{"TestEquipGradeLevel", bnd_SC_ServerDebugInterface_TestEquipGradeLevel},
	{"ControllerEquipSlotReinforce", bnd_SC_ServerDebugInterface_ControllerEquipSlotReinforce},
	{"ControllerEquipSlotRefine", bnd_SC_ServerDebugInterface_ControllerEquipSlotRefine},
	{"EquipItemInPackage", bnd_SC_ServerDebugInterface_EquipItemInPackage},
	{"UnEquipItemInEquipSlot", bnd_SC_ServerDebugInterface_UnEquipItemInEquipSlot},
	{"GetOtherOfflineInfo ", bnd_SC_ServerDebugInterface_GetOtherOfflineInfo },
	{"TestGuildFight", bnd_SC_ServerDebugInterface_TestGuildFight},
	{"TestRobScore", bnd_SC_ServerDebugInterface_TestRobScore},
	{"TestGuildMemberPointInfo", bnd_SC_ServerDebugInterface_TestGuildMemberPointInfo},
	{"JumpTask", bnd_SC_ServerDebugInterface_JumpTask},
	{"TestCombatScore", bnd_SC_ServerDebugInterface_TestCombatScore},
	{"AddAuction", bnd_SC_ServerDebugInterface_AddAuction},
	{"SetSkyArenaScore", bnd_SC_ServerDebugInterface_SetSkyArenaScore},
	{NULL, NULL}
	};
	static int luaopen_SC_ServerDebugInterface(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		binder.Init("SC_ServerDebugInterface", 0, lib_SC_ServerDebugInterface, bnd_LuaDestroy_SC_ServerDebugInterface);
		return 1;
	}
	///////////////////////////////////////////////
	////////LC_MapLogicBase Lua Export////////////////////
	///////////////////////////////////////////////
	static int bnd_LuaCreate_LC_MapLogicBase(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		GameLogic::LC_MapLogicBase* h = NULL;
		if( h == NULL )
		{
			GfxWriteLog(LOG_WARNING_FILELINE, LOG_SWITCH_WARNING, "全局类%s的对象还没有创建就试图在Lua脚本中调用，请检查初始化时序！！", "LC_MapLogicBase");
			return 0;
		}
		binder.PushUserType(h, h->GetRTTIName());
		return 1;
	}
	static int bnd_LuaDestroy_LC_MapLogicBase(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		//GameLogic::LC_MapLogicBase* h = (GameLogic::LC_MapLogicBase*) binder.CheckUserType(1, "LC_MapLogicBase");
		binder.CheckUserType(1, "LC_MapLogicBase");
		return 0;
	}
	static int bnd_LC_MapLogicBase_GetNPCCount(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		GameLogic::LC_MapLogicBase* pkAPI = (GameLogic::LC_MapLogicBase*)binder.CheckUserType(1, "LC_MapLogicBase");
		Utility::LuaCall(*pkAPI, &GameLogic::LC_MapLogicBase::GetNPCCount, L, 2);
		return 1;
	}
	static int bnd_LC_MapLogicBase_GetPlayerCount(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		GameLogic::LC_MapLogicBase* pkAPI = (GameLogic::LC_MapLogicBase*)binder.CheckUserType(1, "LC_MapLogicBase");
		Utility::LuaCall(*pkAPI, &GameLogic::LC_MapLogicBase::GetPlayerCount, L, 2);
		return 1;
	}
	static const luaL_reg lib_LC_MapLogicBase[] = {
	{"Create", bnd_LuaCreate_LC_MapLogicBase},
	{"GetNPCCount", bnd_LC_MapLogicBase_GetNPCCount},
	{"GetPlayerCount", bnd_LC_MapLogicBase_GetPlayerCount},
	{NULL, NULL}
	};
	static int luaopen_LC_MapLogicBase(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		binder.Init("LC_MapLogicBase", 0, lib_LC_MapLogicBase, bnd_LuaDestroy_LC_MapLogicBase);
		return 1;
	}
	///////////////////////////////////////////////
	////////LC_ServerMapLogicBase Lua Export////////////////////
	///////////////////////////////////////////////
	static int bnd_LuaCreate_LC_ServerMapLogicBase(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		LC_ServerMapLogicBase* h = NULL;
		if( h == NULL )
		{
			GfxWriteLog(LOG_WARNING_FILELINE, LOG_SWITCH_WARNING, "全局类%s的对象还没有创建就试图在Lua脚本中调用，请检查初始化时序！！", "LC_ServerMapLogicBase");
			return 0;
		}
		binder.PushUserType(h, h->GetRTTIName());
		return 1;
	}
	static int bnd_LuaDestroy_LC_ServerMapLogicBase(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		//LC_ServerMapLogicBase* h = (LC_ServerMapLogicBase*) binder.CheckUserType(1, "LC_ServerMapLogicBase");
		binder.CheckUserType(1, "LC_ServerMapLogicBase");
		return 0;
	}
	static const luaL_reg lib_LC_ServerMapLogicBase[] = {
	{"Create", bnd_LuaCreate_LC_ServerMapLogicBase},
	{NULL, NULL}
	};
	static int luaopen_LC_ServerMapLogicBase(lua_State* L)
	{
		Utility::LuaBinder binder(L);
		binder.Init("LC_ServerMapLogicBase", "LC_MapLogicBase", lib_LC_ServerMapLogicBase, bnd_LuaDestroy_LC_ServerMapLogicBase);
		return 1;
	}
