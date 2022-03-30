//------------------------------------------------------------------------
void HL_FromGameServer::_handlerNPCExecuteSkillTypeLocation(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//如果还未进入游戏，则退出
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;

	MG_Rlt_NPCExecuteSkillTypeLocation msg;
	if(!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"MG_Rlt_NPCExecuteSkillTypeLocation 解析错误");
		return;
	}

	//npc是否有效
	long lNPCID				= msg.m_lNPCID;
	if(!IS_NPC_ID(lNPCID))
		return;

	LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
	LC_ClientNPC* pkNPC = (LC_ClientNPC* )pkNPCManager->FindNPC(lNPCID);
	if(!pkNPC || pkNPC->GetDead())
		return;

	/*
	long lSkillID			= msg.m_lSkillExecuteID;
	UT_Vec2Float kForwardDir(msg.m_fForwardDirX,msg.m_fForwardDirY);
	UT_Vec3Int kNPCLocation(msg.m_lCurrentLocationX,msg.m_lCurrentLocationY,0);
	UT_Vec3Int kNPCLockedLocation(msg.m_lLockedLocationX,msg.m_lLockedLocationY,0);

	pkNPC->SetTargetForwardDir(kForwardDir);
	pkNPC->SetCurrentLocation(kNPCLocation);
	pkNPC->SetLockedLocation(kNPCLockedLocation);
	pkNPC->ClearSkillMutableParameters();
	pkNPC->PendingSkill(lSkillID,0);
	/*/
	LC_ClientNPCAction	a;
	a.FromSkillLocation(msg);
	pkNPC->PushAction(a);
	//*/
}

//------------------------------------------------------------------------
void HL_FromGameServer::_handlerNPCExecuteSkillTypeTarget(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//如果还未进入游戏，则退出
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;

	MG_Rlt_NPCExecuteSkillTypeTarget msg;
	if(!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"MG_Rlt_NPCExecuteSkillTypeTarget 解析错误");
		return;
	}

	//npc是否有效
	long lNPCID				= msg.m_lNPCID;
	if(!IS_NPC_ID(lNPCID))
		return;

	LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
	LC_ClientNPC* pkNPC = (LC_ClientNPC* )pkNPCManager->FindNPC(lNPCID);
	if(!pkNPC || pkNPC->GetDead())
		return;

	/*
	long lLockTargetID		= msg.m_lLockedTargetID;
	long lSkillID			= msg.m_lSkillExecuteID;
	float* afRandNums		= msg.m_afRandNums;
	UT_Vec2Float kForwardDir(msg.m_fForwardDirX,msg.m_fForwardDirY);
	UT_Vec3Int kNPCLocation(msg.m_lCurrentLocationX,msg.m_lCurrentLocationY,0);

	pkNPC->SetTargetForwardDir(kForwardDir);
	pkNPC->SetCurrentLocation(kNPCLocation);
	//pkNPC->SetVisibleLocation(kNPCLocation);
	pkNPC->SetLockedTargetID(lLockTargetID);
	pkNPC->ClearSkillMutableParameters();
	pkNPC->SetRandNums(afRandNums);
	pkNPC->PendingSkill(lSkillID,0);
	/*/
	LC_ClientNPCAction	a;
	a.FromSkillTarget(msg);
	pkNPC->PushAction(a);
	//*/
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerNPCExecuteSkillTypeDir(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//如果还未进入游戏，则退出
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;

	MG_Rlt_NPCExecuteSkillTypeDir msg;
	if(!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"MG_Rlt_NPCExecuteSkillTypeDir 解析错误");
		return;
	}

	//npc是否有效
	long lNPCID				= msg.m_lNPCID;
	if(!IS_NPC_ID(lNPCID))
		return;

	LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
	LC_ClientNPC* pkNPC = (LC_ClientNPC* )pkNPCManager->FindNPC(lNPCID);
	if(!pkNPC || pkNPC->GetDead())
		return;

	/*
	long	lSkillID			= msg.m_lSkillExecuteID;
	float* afRandNums			= msg.m_afRandNums;
	UT_Vec2Float kForwardDir(msg.m_fForwardDirX,msg.m_fForwardDirY);
	UT_Vec3Int kCurrentLocation(msg.m_lCurrentLocationX,msg.m_lCurrentLocationY,0);
	UT_Vec3Int kLockedLocation(msg.m_lLockedLocationX,msg.m_lLockedLocationY,0);

	pkNPC->SetTargetForwardDir(kForwardDir);
	pkNPC->SetCurrentLocation(kCurrentLocation);
	pkNPC->SetLockedLocation(kLockedLocation);
	pkNPC->SetLockedTargetID(msg.m_lLockedTargetID);
	pkNPC->ClearSkillMutableParameters();
	pkNPC->SetRandNums(afRandNums);
	pkNPC->PendingSkill(lSkillID,0);
	/*/
	LC_ClientNPCAction	a;
	a.FromSkillDir(msg);
	pkNPC->PushAction(a);
	//*/
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerNPCInterruptSkill(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//如果还未进入游戏，则退出
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(!pkGamePlayer)
		return;

	MG_Rlt_NPCInterruptSkill rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_Rlt_NPCInterruptSkill解析错误");
		return;
	}

	if(rltMsg.m_iResultType != RE_SKILL_NONE_ERROR)
	{
		if (rltMsg.m_iResultType != RE_SKILL_SYSTEM_ERROR)
		{
			SendLogicInfoMessage(rltMsg.m_iResultType);
		}
		return;
	}

	long	lRequesterID		= rltMsg.m_lRequesterID;
	int		iSkillExecuteID		= rltMsg.m_lSkillExecuteID;
	uint8_t	usInterruptType		= rltMsg.m_usInterruptType;

	//怪物中断技能
	LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
	LC_ClientNPC* pkNPC = (LC_ClientNPC* )pkNPCManager->FindNPC(lRequesterID);
	if(pkNPC && !pkNPC->GetDead())
	{
		pkNPC->InterruptSkill(GET_PROCESS_TIME,iSkillExecuteID,(SkillInterruptType)usInterruptType);
	}
}
//------------------------------------------------------------------------
static void logPlayerExecuteSkill(float fCurrentTime, object_id_type who_, long lLockTarget, SK_SkillExecute * pkSkill)
{
	GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM,
		"[Skill]Execute, time=%.3f, player=%d, target=%d, skill_id=%d, op_type=%d, FrameID=%d",
		fCurrentTime, who_, lLockTarget,
		pkSkill->GetExecuteID(), pkSkill->GetProperty_OperationType(),
		UT_TimeManager::GetSingleton().CurrentFrameNo()
		);
}
static void logPlayerInterrupSkill(float fCurrentTime, object_id_type who_, int iSkillExecuteID)
{
	GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM, 
		"[Skill]Interrupt, time=%.3f, player=%d, skill_id=%d, FrameID=%d",
		fCurrentTime, who_, iSkillExecuteID,
		UT_TimeManager::GetSingleton().CurrentFrameNo()
		);
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerPlayerExecuteSkillTypeTarget(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack)
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//如果还未进入游戏，则退出
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(!pkGamePlayer)
		return;

	MG_Rlt_PlayerExecuteSkillTypeTarget msg;
	if(!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_Rlt_PlayerExecuteSkillTypeTarget解析错误");
		return;
	}
	
	object_id_type lRequesterID		= msg.m_lRequesterID;
	if (bArenaPlayBack)
	{
		LC_ClientArenaPlayBackManager *pkArenaPlayBackManager = LC_ClientArenaPlayBackManager::GetSingletonPtr();
		lRequesterID = pkArenaPlayBackManager->GetTemID(lRequesterID);
	}
	int  iSkillExecuteID	= msg.m_lSkillExecuteID;
	if(msg.m_iResultType != RE_SKILL_NONE_ERROR)
	{
		if(lRequesterID == pkGamePlayer->GetID())
			pkGamePlayer->OnRequestExecuteSkillFailed(iSkillExecuteID);

		if(msg.m_iResultType == RE_SKILL_NOT_ENOUGH_ITEM)
		{
			SC_ClientScriptInterface::GetSingletonPtr()->GetSkillInterface()->SendInfoMessage(iSkillExecuteID, RE_SKILL_NOT_ENOUGH_ITEM);
		}
		else if(lRequesterID == pkGamePlayer->GetID())
			SendLogicInfoMessage(msg.m_iResultType);

		return;
	}

	SK_SkillExecute* pkSkill = SK_Factory::GetSingletonPtr()->RequestSkillExcute(iSkillExecuteID);
	if ( !pkSkill )
		return ;
	
	//响应服务器的通知，驱动技能
	long lLockTargetID		= msg.m_lLockTargetID;
	float* afRandNums		= msg.m_afRandNums;
	float fCurrentTime		= GET_PROCESS_TIME;
	
	if(lRequesterID == pkGamePlayer->GetID())
	{
		//如果在寻路状态 忽略拾取道具技能，避免打断当前寻路状态。
		bool bIgnore = SKILL_PICK_CHEST_LEFT == iSkillExecuteID && pkGamePlayer->IsPathFinding();
		if(bIgnore)
			return;

		logPlayerExecuteSkill(fCurrentTime, lRequesterID, lLockTargetID, pkSkill);
		if ( pkSkill->GetProperty_IsLCSkill() )
		{
			pkGamePlayer->ExecuteLCSkillTargetImmediately(fCurrentTime, pkSkill, lLockTargetID);
		}
		else
		{
			if (pkSkill->GetProperty_ExecutePriority() && !pkGamePlayer->GetAIControl())
			{
				pkGamePlayer->_doUnlockRequestExecuteSkill(true, iSkillExecuteID);
			}
			else if (!pkSkill->GetProperty_ExecutePriority() || pkGamePlayer->GetAIControl())
			{
				//主玩家直接启动技能
				pkGamePlayer->SetLockedTargetID(lLockTargetID);
				pkGamePlayer->SetRandNums(afRandNums);
				pkGamePlayer->ClearPendingSkill();

				pkGamePlayer->PrepareExecuteSkill(pkSkill,0);
				pkGamePlayer->ExecuteSkill(fCurrentTime,pkSkill,0);
			}
		}
	}
	else
	{
		//网络玩家挂起技能
		LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
		LC_ClientNetPlayer* pkNetPlayer = (LC_ClientNetPlayer* )pkNetPlayerManager->FindPlayer(lRequesterID);
		if(pkNetPlayer && false == pkNetPlayer->GetDead())
		{
			if ( pkSkill->GetProperty_IsLCSkill() )
			{
				pkNetPlayer->ExecuteLCTargetSkill(fCurrentTime, pkSkill, lLockTargetID);
			}
			else
			{
				pkNetPlayer->SetLockedTargetID(lLockTargetID);
				pkNetPlayer->SetRandNums(afRandNums);
				pkNetPlayer->PendingSkill(iSkillExecuteID,0);
				pkNetPlayer->AddActivateSkill(iSkillExecuteID, msg.m_nLogicValue);
			}
		}
	}
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerPlayerExecuteSkillTypeDir(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack)
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);

	//如果还未进入游戏，则退出
	if(!pkClientSessionGroup->GetInGameFlag())
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(!pkGamePlayer)
		return;

	MG_Rlt_PlayerExecuteSkillTypeDir msg;
	if(!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_Rlt_PlayerExecuteSkillTypeDir解析错误");
		return;
	}

	object_id_type lRequesterID		= msg.m_lRequesterID;
	if (bArenaPlayBack)
	{
		LC_ClientArenaPlayBackManager *pkArenaPlayBackManager = LC_ClientArenaPlayBackManager::GetSingletonPtr();
		lRequesterID = pkArenaPlayBackManager->GetTemID(lRequesterID);
	}
	int  iSkillExecuteID	= msg.m_lSkillExecuteID;
	if(msg.m_iResultType != RE_SKILL_NONE_ERROR)
	{
		if(lRequesterID == pkGamePlayer->GetID())
		{
			pkGamePlayer->OnRequestExecuteSkillFailed(iSkillExecuteID);

			if(msg.m_iResultType == RE_SKILL_NOT_ENOUGH_ITEM)
			{
				SC_ClientScriptInterface::GetSingletonPtr()->GetSkillInterface()->SendInfoMessage(iSkillExecuteID, RE_SKILL_NOT_ENOUGH_ITEM);
			}
			else if (msg.m_iResultType != RE_SKILL_SYSTEM_ERROR && lRequesterID == pkGamePlayer->GetID())
			{
				SendLogicInfoMessage(msg.m_iResultType);
			}
			return;
		}
		return;
	}

	SK_SkillExecute* pkSkill = SK_Factory::GetSingletonPtr()->RequestSkillExcute(iSkillExecuteID);
	if ( !pkSkill )
		return ;

	long lLockTargetID		= msg.m_lLockedTargetID;
	UT_Vec2Float sd			= UT_MathBase::GetZRotDir(msg.m_iForwardDirAngle);
	float fForwardDir_X		= sd.x;
	float fForwardDir_Y		= sd.y;
	const SE_SceneTerrainLocalInfo& rkTerrainInfo1 = SE_SceneManager::GetSingletonPtr()->GetWalkableInfo(Int2Float(msg.m_lLockedLocationX), Int2Float(msg.m_lLockedLocationY));
	UT_Vec3Int kLockedLocation(msg.m_lLockedLocationX,msg.m_lLockedLocationY,rkTerrainInfo1._fLocalHeight);
	float* afRandNums		= msg.m_afRandNums;
	float fCurrentTime		= GET_PROCESS_TIME;

	if(lRequesterID == pkGamePlayer->GetID())
	{
		logPlayerExecuteSkill(fCurrentTime, lRequesterID, lLockTargetID, pkSkill);
		if ( pkSkill->GetProperty_IsLCSkill() )
		{
			pkGamePlayer->ExecuteLCSkillImmediately(fCurrentTime, pkSkill, lLockTargetID, kLockedLocation, msg.m_iForwardDirAngle);
		}
		//方向型瞬发技能
		else if (SOT_DIR == pkSkill->GetProperty_OperationType() && pkSkill->GetProperty_ExecuteImmediately())
		{
			//这里未设置SetLockedLocation，目前不会有问题，因为没有方向型瞬发技能，以后万一有需要注意，ayu
			pkGamePlayer->ExecuteSkillImmediately(fCurrentTime, pkSkill, 0);
		}
		else
		{
			if ( pkSkill->GetProperty_ExecutePriority() && !pkGamePlayer->GetAIControl())
			{
				pkGamePlayer->_doUnlockRequestExecuteSkill(true, iSkillExecuteID);
			}
			else if (!pkSkill->GetProperty_ExecutePriority() || pkGamePlayer->GetAIControl())
			{
				//主玩家直接启动技能
				pkGamePlayer->SetLockedLocation(kLockedLocation);
				pkGamePlayer->SetLockedTargetID(lLockTargetID);
				pkGamePlayer->SetTargetForwardDir(UT_Vec2Float(fForwardDir_X,fForwardDir_Y));
				pkGamePlayer->SetRandNums(afRandNums);
				pkGamePlayer->ClearPendingSkill();

				pkGamePlayer->PrepareExecuteSkill(pkSkill,0);
				pkGamePlayer->ExecuteSkill(fCurrentTime,pkSkill,0);
			}
		}
	}
	else
	{
		//网络玩家挂起技能
		LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
		LC_ClientNetPlayer* pkNetPlayer = (LC_ClientNetPlayer* )pkNetPlayerManager->FindPlayer(lRequesterID);
		if(pkNetPlayer && false == pkNetPlayer->GetDead())
		{
			if ( pkSkill->GetProperty_IsLCSkill() )
			{
				pkNetPlayer->ExecuteLCSkill(fCurrentTime, pkSkill, lLockTargetID, kLockedLocation, msg.m_iForwardDirAngle);
			}
			else
			{	
				pkNetPlayer->SetForwardDirAngle(msg.m_iForwardDirAngle);
				pkNetPlayer->SetLockedLocation(kLockedLocation);
				pkNetPlayer->SetLockedTargetID(lLockTargetID);
				pkNetPlayer->SetTargetForwardDir(UT_Vec2Float(fForwardDir_X,fForwardDir_Y));
				pkNetPlayer->SetRandNums(afRandNums);
				pkNetPlayer->PendingSkill(iSkillExecuteID,0);
				pkNetPlayer->AddActivateSkill(iSkillExecuteID, msg.m_nLogicValue);
			}
		}
	}
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerPlayerExecuteSkillTypeLocation(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack)
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//如果还未进入游戏，则退出
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(!pkGamePlayer)
		return;
	
	MG_Rlt_PlayerExecuteSkillTypeLocation msg;
	if(!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_Rlt_PlayerExecuteSkillTypeLocation解析错误");
		return;
	}

	object_id_type lRequesterID		= msg.m_lRequesterID;
	if (bArenaPlayBack)
	{
		LC_ClientArenaPlayBackManager *pkArenaPlayBackManager = LC_ClientArenaPlayBackManager::GetSingletonPtr();
		lRequesterID = pkArenaPlayBackManager->GetTemID(lRequesterID);
	}
	int  iSkillExecuteID	= msg.m_lSkillExecuteID;
	if(msg.m_iResultType != RE_SKILL_NONE_ERROR)
	{
		if(lRequesterID == pkGamePlayer->GetID())
			pkGamePlayer->OnRequestExecuteSkillFailed(iSkillExecuteID);

		if(msg.m_iResultType == RE_SKILL_NOT_ENOUGH_ITEM)
		{
			SC_ClientScriptInterface::GetSingletonPtr()->GetSkillInterface()->SendInfoMessage(iSkillExecuteID, RE_SKILL_NOT_ENOUGH_ITEM);
		}
		else if (msg.m_iResultType != RE_SKILL_SYSTEM_ERROR && lRequesterID == pkGamePlayer->GetID())
		{
			SendLogicInfoMessage(msg.m_iResultType);
		}
		return;
	}

	SK_SkillExecute* pkSkill = SK_Factory::GetSingletonPtr()->RequestSkillExcute(iSkillExecuteID);
	if (pkSkill == NULL)
		return;

	const SE_SceneTerrainLocalInfo& rkTerrainInfo1 = SE_SceneManager::GetSingletonPtr()->GetWalkableInfo(Int2Float(msg.m_lLockedLocationX), Int2Float(msg.m_lLockedLocationY));
	UT_Vec3Int kLockedLocation(msg.m_lLockedLocationX,msg.m_lLockedLocationY,rkTerrainInfo1._fLocalHeight);
	float* afRandNums		= msg.m_afRandNums;
	float fCurrentTime		= GET_PROCESS_TIME;

	if(lRequesterID == pkGamePlayer->GetID())
	{
		logPlayerExecuteSkill(fCurrentTime, lRequesterID, 0, pkSkill);
		if (pkSkill->GetProperty_ExecutePriority() && !pkGamePlayer->GetAIControl())
		{
			pkGamePlayer->_doUnlockRequestExecuteSkill(true, iSkillExecuteID);
		}
		else if (!pkSkill->GetProperty_ExecutePriority() || pkGamePlayer->GetAIControl())
		{
			//主玩家直接启动技能
			pkGamePlayer->SetLockedLocation(kLockedLocation);
			pkGamePlayer->SetTargetLocation(kLockedLocation);
			pkGamePlayer->SetRandNums(afRandNums);
			pkGamePlayer->ClearPendingSkill();

			pkGamePlayer->PrepareExecuteSkill(pkSkill,0);
			pkGamePlayer->ExecuteSkill(fCurrentTime,pkSkill,0);
		}
	}
	else
	{
		//网络玩家挂起技能
		LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
		LC_ClientNetPlayer* pkNetPlayer = (LC_ClientNetPlayer* )pkNetPlayerManager->FindPlayer(lRequesterID);
		if(pkNetPlayer && false == pkNetPlayer->GetDead())
		{
			pkNetPlayer->SetLockedLocation(kLockedLocation);
			//pkNetPlayer->SetTargetLocation(kLockedLocation);
			pkNetPlayer->SetRandNums(afRandNums);
			pkNetPlayer->PendingSkill(iSkillExecuteID,0);
			pkNetPlayer->AddActivateSkill(iSkillExecuteID, msg.m_nLogicValue);
		}
	}
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerPlayerInterruptSkill(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack)
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//如果还未进入游戏，则退出
	//if (eGameStage != ST_GAME_STAGE_GAME)
	//	return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(!pkGamePlayer)
		return;

	MG_Rlt_PlayerInterruptSkill rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_Rlt_PlayerInterruptSkill解析错误");
		return;
	}

	if(rltMsg.m_iResultType != RE_SKILL_NONE_ERROR)
	{
		if (rltMsg.m_iResultType != RE_SKILL_SYSTEM_ERROR
			&& rltMsg.m_usInterruptType != SKI_CHANGE_TARGET	//因为切换目标导致原有技能中断
			&& rltMsg.m_usInterruptType != SKI_CHANGE_SKILL)	//因为切换技能导致原有技能中断
		{
			SendLogicInfoMessage(rltMsg.m_iResultType);
		}
		return;
	}

	object_id_type	lRequesterID		= rltMsg.m_lRequesterID;
	if (bArenaPlayBack)
	{
		LC_ClientArenaPlayBackManager *pkArenaPlayBackManager = LC_ClientArenaPlayBackManager::GetSingletonPtr();
		lRequesterID = pkArenaPlayBackManager->GetTemID(lRequesterID);
	}
	int		iSkillExecuteID		= rltMsg.m_lSkillExecuteID;
	uint8_t	usInterruptType		= rltMsg.m_usInterruptType;
	float	fCurrentTime		= GET_PROCESS_TIME;

	logPlayerInterrupSkill(fCurrentTime, lRequesterID, iSkillExecuteID);
	if(lRequesterID == pkGamePlayer->GetID())
	{
		//主玩家被动中断技能
		if(usInterruptType == SKI_EVENT || usInterruptType == SKI_DAMAGE || usInterruptType == SKI_FORCE)
		{
			pkGamePlayer->InterruptSkill(fCurrentTime,iSkillExecuteID,(SkillInterruptType)usInterruptType);
			////挂机特殊处理
			//if(iSkillExecuteID == SKILL_HOOK||(iSkillExecuteID>=SKILL_HAND_HOOK&&iSkillExecuteID<=SKILL_DOUBLE_KNIFE_HOOK))
			//{
			//	SendLogicInfoMessage(RE_REQUESET_HOOK_INTERRUPT);
			//}
			//else
			{
				//显示提示信息
				if (usInterruptType != SKI_FORCE && iSkillExecuteID != SKILL_CATCH)		//Tricks: SKI_FORCE类型不做显示提醒，ayu，SKI_FORCE主要用于客户端主动取消和服务器强制取消
					SendLogicInfoMessage(RE_SKILL_BE_INTERRUPT);
			}
		}
	}
	else
	{
		//网络玩家中断技能
		LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
		LC_ClientNetPlayer* pkNetPlayer = (LC_ClientNetPlayer* )pkNetPlayerManager->FindPlayer(lRequesterID);
		if(pkNetPlayer && false == pkNetPlayer->GetDead())
		{
			pkNetPlayer->InterruptSkill(fCurrentTime,iSkillExecuteID,(SkillInterruptType)usInterruptType);
		}
	}
}
//------------------------------------------------------------------------
//void HL_FromGameServer::_handlerRltSkillEvent(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
//{
//	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
//	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
//	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();
//
//	//如果还未进入游戏，则退出
//	if (eGameStage != ST_GAME_STAGE_GAME)
//		return;
//
//
//	MG_RLT_SkillEvent msg;
//	if(!msg.Decode(pMsg,wSize))
//	{
//		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_RLT_SkillEvent解析错误");
//		return;
//	}
//
//	LC_ClientGamePlayer* pkPlayer = pkSession->GetPlayer();
//	if(pkPlayer)
//		pkPlayer->RecvSkillEventFromGameServer(msg);
//}
//----------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltSkillEvent(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack)
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	MG_PB<msg_define::skill_event> pbMsg;
	if (!pbMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_RLT_SkillEvent解析错误");
		return;
	}
	msg_define::skill_event& kentry = pbMsg.m_value; 
	//如果还未进入游戏，则退出
	if (eGameStage != ST_GAME_STAGE_GAME && kentry.event_type() != SETE_DETACH_STATE)
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(!pkGamePlayer)
		return;

	SK_ClientSkillEventManager* pkSkillEventManager = CLIENT_GET_SKILL_EVENT_MANAGER();
	pkSkillEventManager->RecvMessageFromServer(kentry, bArenaPlayBack);
}
//----------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltSkillEventEx(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack)
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	MG_PB<msg_define::skill_event> pbMsg;
	if (!pbMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_RLT_SkillEventEx解析错误");
		return;
	}

	msg_define::skill_event& kentry = pbMsg.m_value; 
	//如果还未进入游戏，则退出
	if (eGameStage != ST_GAME_STAGE_GAME && kentry.event_type() != SETE_DETACH_GFX)
		return;

	
	SK_ClientSkillEventManager* pkSkillEventManager = CLIENT_GET_SKILL_EVENT_MANAGER();
	pkSkillEventManager->RecvMessageFromServer(kentry, bArenaPlayBack);
}
//----------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltSkillEventMerged(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack)
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	MG_SyncBulletInfo kMsg;
	if (!kMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncBulletInfo 解析错误");
		return;
	}

	//如果还未进入游戏，则退出
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;

	SK_ClientSkillEventManager* pkSkillEventManager = CLIENT_GET_SKILL_EVENT_MANAGER();
	pkSkillEventManager->RecvShotBulletMergedFromServer(kMsg, bArenaPlayBack);
}
//----------------------------------------------------------------------------
void HL_FromGameServer::_handlerReqSkillRespawn(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//如果还未进入游戏，则退出
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;


	MG_REQ_SkillRespawn reqMsg;	
	if(!reqMsg.Decode(pMsg,wSize))	
	{		
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"无法解析MG_REQ_SkillRespawn");		
		return;	
	}		

	long lRequestID = reqMsg.m_lRespawnSourcePlayerID;
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	//如果对方已经复活，则不弹出提示
	if (NULL == pkGamePlayer || (!pkGamePlayer->GetDead() && pkGamePlayer->GetID() != lRequestID))
	{
		return;
	}

	//发送系统消息,激活对话框
	MG_MessageCenter* pkMessageCenter = MG_MessageCenter::GetSingletonPtr();
	if(!pkMessageCenter)
		return;


	SendMessageDialog(RE_DIALOG_CONFIRM_BE_SKILL_RESPWAN, STDSTR2TPSTR(reqMsg.m_strRespawnSourcePlayerName), "",
		MDT_CONFIRM_SKILL_RESPAWN, reqMsg.m_lRespawnSourcePlayerID, NO_USE_PARAMETER, NO_USE_PARAMETER);

	//char kOutputMessage[256];
	//sprintf(kOutputMessage,"%s 请求复活你，是否同意?",);
	//pkMessageCenter->SendMessage(CreateMsg_LOGIC_SYSTEM_INFO(kOutputMessage,SIT_CONFIRM_SKILL_RESPAWN,reqMsg.m_lRespawnSourcePlayerID,NO_USE_PARAMETER,NO_USE_PARAMETER),true);
}
//----------------------------------------------------------------------------
void HL_FromGameServer::_handlerRespawnBroadcastLocation(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//如果还未进入游戏，则退出
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;

	MG_RespawnBroadcastLocation kMsg;	
	if(!kMsg.Decode(pMsg,wSize))	
	{		
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"无法解析MG_RespawnBroadcastLocation");		
		return;	
	}		

	object_id_type lPlayerID = kMsg.m_lPlayerID;
	int iLocationX = kMsg.m_iLocationX;
	int iLocationY = kMsg.m_iLocationY;
	
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if (NULL == pkGamePlayer || pkGamePlayer->GetID() == lPlayerID)
	{
		return;
	}

	LC_ClientNetPlayerManager* pkPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
	LC_PlayerBase* pkPlayer = pkPlayerManager->FindPlayer(lPlayerID);
	if (NULL != pkPlayer)
	{
		SE_SceneManager* pkSceneManager = SE_SceneManager::GetSingletonPtr();
		const SE_SceneTerrainLocalInfo& rkTerrainInfo = pkSceneManager->GetWalkableInfo(Int2Float(iLocationX), Int2Float(iLocationY));
		UT_Vec3Int kLocation;
		kLocation.x = iLocationX;
		kLocation.y = iLocationY;
		kLocation.z = Float2Int(rkTerrainInfo._fLocalHeight);
		pkPlayer->SetCurrentLocation(kLocation);
		pkPlayer->SetVisibleLocation(kLocation);
		pkPlayer->SetTargetLocation(kLocation);
	}
}
//----------------------------------------------------------------------------
void HL_FromGameServer::_handlerPunchMovePlayer(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack)
{
	MG_MoveActorPosition kMsg;

	if(!kMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"无法解析MG_MoveActorPosition");
		return;
	}

	SE_SceneManager* pkSceneManager = SE_SceneManager::GetSingletonPtr();
	const SE_SceneTerrainLocalInfo& rkTerrainInfo1 = pkSceneManager->GetWalkableInfo(Int2Float(kMsg.m_lTargetLocationX), Int2Float(kMsg.m_lTargetLocationY));
	UT_Vec3Int kTargetPos(kMsg.m_lTargetLocationX,kMsg.m_lTargetLocationY, Float2Int(rkTerrainInfo1._fLocalHeight));

	object_id_type lPlayerID = kMsg.m_lActorID;
	if (bArenaPlayBack)
	{
		LC_ClientArenaPlayBackManager *pkArenaPlayBackManager = LC_ClientArenaPlayBackManager::GetSingletonPtr();
		lPlayerID = pkArenaPlayBackManager->GetTemID(lPlayerID);
	}
	LC_PlayerBase* pkPlayer = NULL;
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if (NULL == pkGamePlayer || pkGamePlayer->GetID() == lPlayerID)
	{
		if (pkGamePlayer != NULL)
		{
			pkPlayer = pkGamePlayer;
			pkPlayer->PunchMove((SkillMoveActorPositionType)kMsg.m_lMoveType, kTargetPos,kMsg.m_fMoveTime);
		}
	}
	else
	{
		LC_ClientNetPlayerManager* pkPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
		pkPlayer = pkPlayerManager->FindPlayer(lPlayerID);
		if (NULL != pkPlayer)
		{
			const SE_SceneTerrainLocalInfo& rkTerrainInfo2 = pkSceneManager->GetWalkableInfo(Int2Float(kMsg.m_lCurrentLocationX), Int2Float(kMsg.m_lCurrentLocationY));
			UT_Vec3Int kCurrentPos(kMsg.m_lCurrentLocationX,kMsg.m_lCurrentLocationY,Float2Int(rkTerrainInfo2._fLocalHeight));

			pkPlayer->SetCurrentLocation(kCurrentPos);
			pkPlayer->SetVisibleLocation(kCurrentPos);
			pkPlayer->PunchMove((SkillMoveActorPositionType)kMsg.m_lMoveType, kTargetPos,kMsg.m_fMoveTime);
		}
	}

	int lStateType = 0;
	switch(kMsg.m_lMoveType)
	{
	case SMAPT_PUNCHBACK:
		lStateType = 207;
		break;
	case SMAPT_HITFLY:
		lStateType = 208;
		break;
	default:
		return;
		break;
	}
	if (pkGamePlayer != NULL && NULL != pkPlayer && pkPlayer->CheckStateFxTime(lStateType))
	{
		pkGamePlayer->SendMessage( CreateMsg_UE_PLAYER_STATE_CHANGE(lPlayerID, lStateType) );
	}	
}
//----------------------------------------------------------------------------
void HL_FromGameServer::_handlerPunchMoveNpc(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_MoveActorPosition kMsg;

	if(!kMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"无法解析MG_MoveActorPosition");
		return;
	}

	LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
	if(!pkNPCManager)
		return;

	LC_ClientNPC* pkNPC = (LC_ClientNPC*)pkNPCManager->FindNPC(kMsg.m_lActorID);
	if(!pkNPC)
		return;

	SE_SceneManager* pkSceneManager = SE_SceneManager::GetSingletonPtr();
	const SE_SceneTerrainLocalInfo& rkTerrainInfo1 = pkSceneManager->GetWalkableInfo(Int2Float(kMsg.m_lTargetLocationX), Int2Float(kMsg.m_lTargetLocationY));
	const SE_SceneTerrainLocalInfo& rkTerrainInfo2 = pkSceneManager->GetWalkableInfo(Int2Float(kMsg.m_lCurrentLocationX), Int2Float(kMsg.m_lCurrentLocationY));

	UT_Vec3Int kTargetPos(kMsg.m_lTargetLocationX,kMsg.m_lTargetLocationY, Float2Int(rkTerrainInfo1._fLocalHeight));
	UT_Vec3Int kCurrentPos(kMsg.m_lCurrentLocationX,kMsg.m_lCurrentLocationY,Float2Int(rkTerrainInfo2._fLocalHeight));
	pkNPC->SetCurrentLocation(kCurrentPos);
	pkNPC->SetVisibleLocation(kCurrentPos);
	pkNPC->SetActionBaseTime(kMsg.m_fServerTime);
	pkNPC->PunchMove((SkillMoveActorPositionType)kMsg.m_lMoveType, kTargetPos,kMsg.m_fMoveTime);

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	int lStateType = 0;
	switch(kMsg.m_lMoveType)
	{
	case SMAPT_PUNCHBACK:
		lStateType = 207;
		break;
	case SMAPT_HITFLY:
		lStateType = 208;
		break;
	default:
		return;
		break;
	}
	if (pkGamePlayer != NULL && pkNPC->CheckStateFxTime(lStateType))
	{
		pkGamePlayer->SendMessage( CreateMsg_UE_PLAYER_STATE_CHANGE(kMsg.m_lActorID, lStateType) );
	}	
}
//----------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltDeactiveSkillState(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_DeactiveSkillState rltMsg;

	if(!rltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"MG_RLT_DeactiveSkillState解析错误");		
		return;
	}
	
	CF_SkillStateList* pkCSVStateList = CF_SkillStateList::GetSingletonPtr();
	if(!pkCSVStateList)
		return;

	CF_SkillStateList::DataEntry* pkData = pkCSVStateList->GetEntryPtr(rltMsg.m_lSkillStateTypeID);
	if(!pkData)
		return;

	SendLogicInfoMessage(rltMsg.m_lResult,pkData->_kName.c_str());

}
//----------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltTalkToNPC(NW_Game2ClientSession* pkSession,char* pMsg,uint16_t wSize)
{
	MG_RLT_TalkToNPC rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"MG_RLT_TalkToNPC解析错误");		
		return;
	}

	object_id_type lPlayerID = rltMsg.m_lPlayerID;
	object_id_type lNPCID = rltMsg.m_lNPCID;
	bool bCheckNPCOwnerShipSuccess = rltMsg.m_bCheckNPCOwerShipSuccess;

	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (NULL == pkGamePlayer || pkGamePlayer->GetID() != lPlayerID)
	{
		return;
	}

	if (bCheckNPCOwnerShipSuccess)
	{
		pkGamePlayer->TalkToNPC(lNPCID);
	}
	else
	{
		SendLogicInfoMessage(RE_TIP_MESSAGE, "您没有权限");
	}
}
//----------------------------------------------------------------------------
