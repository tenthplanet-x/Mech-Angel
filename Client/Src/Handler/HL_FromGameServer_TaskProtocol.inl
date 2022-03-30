

//------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltAcceptTask(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

// 	//如果还未进入游戏，则退出
// 	if (eGameStage != ST_GAME_STAGE_GAME)
// 		return;

	MG_RLT_AcceptTask msg;
	if(!msg.Decode(pMsg,wSize))
		return;

	object_id_type lPlayerID = msg.m_lPlayerID;
	int  lTaskID    = msg.m_lTaskID;
	ResultType iResult = msg.m_iResult;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(!pkGamePlayer || pkGamePlayer->GetMainControllerID() != lPlayerID)
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"角色数值同步消息逻辑错误");
		return;
	}

	LC_ClientTaskManager* pkTaskManager = (LC_ClientTaskManager*)LC_ClientTaskManager::GetSingletonPtr();
	if (pkTaskManager == NULL)
		return;

	LC_ClientTask*  pkTask = (LC_ClientTask*)pkTaskManager->GetTask(lTaskID);
	if (pkTask == NULL)
		return;

	//GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MGPT_RLT_ACCEPT_TASK iResult : %d , taskId : %d", msg.m_iResult ,msg.m_lTaskID);
	if (iResult == RE_TASK_START_SUCCESS)
	{
		pkGamePlayer->RltAcceptTask(lTaskID);
		pkGamePlayer->AcceptGuideTask(pkTask);
		pkGamePlayer->SetFinishTaskFlag(false);

		int iTaskType = pkTask->GetTaskType();
		if (iTaskType == STT_MAIN_SCENARIO || iTaskType == STT_DAILY_TASK || iTaskType == STT_GUILD_TASK ||
			iTaskType == STT_GAMESTORY || iTaskType == STT_FRONTIER || iTaskType == STT_REWARD_TASK)
		{
			int nTaskTimeLimit = pkTask->GetTimeLimit();

			LC_GameEvent kGameEvent;
			kGameEvent.SetEventType(GLET_ACCEPT_TASK);
			kGameEvent.SetEventParam(EP_ACCEPT_TASK_PLAYER_ID, lPlayerID);
			kGameEvent.SetEventParam(EP_ACCEPT_TASK_ID, lTaskID);
			kGameEvent.SetEventParam(EP_ACCEPT_TASK_TIME_LIMIT, nTaskTimeLimit);
			kGameEvent.SetEventParam(EP_ACCEPT_TASK_INDEX, pkTask->GetMainIndex());

			UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
			pkUIManager->HandlerGameEvent(kGameEvent);

			//g_soundMgr.Play2DEx(VARIABLE2STRING(SE_Sys_Mission_Accept));

			MG_MessageCenter::GetSingleton().SendMessage( CreateMsg_UICutScenes_Condition(CUCS_TYPE_Task, lTaskID, CUCS_TASK_Accept) );
		}
	}
	else
	{
		if(iResult != RE_ACCEPTTASK_ERORR_2)
			pkGamePlayer->GetClientTaskMap()->AddToCanStartList(lTaskID);

		if ( iResult == RE_EXCEED_MAX_TIMES_PER_DAY)
		{
			SendLogicInfoMessage(RE_TASK_EXCEED_MAX_TIMES_PER_DAY);
		}
		else
			SendLogicInfoMessage(iResult);
	}
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltFinishTask(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

// 	//如果还未进入游戏，则退出
// 	if (eGameStage != ST_GAME_STAGE_GAME)
// 		return;


	MG_RLT_FinishTask msg;
	if(!msg.Decode(pMsg,wSize))
		return;

	GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM,"MGPT_RLT_FINISH_TASK m_iResult=[%d],m_lPlayerID=[%d],m_lTaskID=[%d],m_iBonusItemIndex=[%d],",msg.m_iResult,msg.m_lPlayerID,msg.m_lTaskID,msg.m_iBonusItemIndex);
	object_id_type lPlayerID = msg.m_lPlayerID;
	int  lTaskID    = msg.m_lTaskID;
	int  iItemIndex = msg.m_iBonusItemIndex;
	int  iResult	= msg.m_iResult;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

	if(!pkGamePlayer || pkGamePlayer->GetMainControllerID() != lPlayerID)
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"角色数值同步消息逻辑错误");
		return;
	}

	if (iResult == RE_TASK_FINISH_SUCCESS)
	{
		pkGamePlayer->RltFinishTask(lTaskID,iItemIndex);

		LC_Task* pkTask = LC_ClientTaskManager::GetSingletonPtr()->GetTask(msg.m_lTaskID);
		if (pkTask == NULL)
			return;

		//任务完成音效播放
		if (pkTask->GetTaskType() == STT_GAMESTORY)
		{
			//押镖任务完成音效
			g_soundMgr.Play2DEx(VARIABLE2STRING(SE_Sys_CarriageMission_Complete));		
		}
		else if (pkTask->GetTaskType() == STT_CRUSADE_TOKEN)
		{
			//讨伐任务完成音效		
			g_soundMgr.Play2DEx(VARIABLE2STRING(SE_Sys_CrusadeMission_Complete));	
		}
		else
		{
			g_soundMgr.Play2DEx(VARIABLE2STRING(SE_Sys_Mission_Complete));
		}

		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_FINISH_TASK);
		kGameEvent.SetEventParam(EP_FINISH_TASK_ID, lTaskID);
		kGameEvent.SetEventParam(EP_FINISH_TASK_INDEX, pkTask->GetMainIndex());

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);

		MG_MessageCenter::GetSingleton().SendMessage( CreateMsg_UICutScenes_Condition(CUCS_TYPE_Task, lTaskID, CUCS_TASK_Finished) );
		pkGamePlayer->SetFinishTaskFlag(true);
		pkGamePlayer->CheckCommanderTitleOpen();

		/*if (pkTask->GetTaskType() == STT_DAILY_CHALLENGE)
		{
			SendLogicInfoMessage(RE_DAILY_CHANLLENGE_FINISH_INFO,pkTask->GetTaskGoal().c_str());
		}*/
		
	}
	else
	{
		SendLogicInfoMessage(iResult);
	}
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltCancelTask(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

// 	//如果还未进入游戏，则退出
// 	if (eGameStage != ST_GAME_STAGE_GAME)
// 		return;


	MG_RLT_CancelTask msg;
	if(!msg.Decode(pMsg,wSize))
		return;

	object_id_type lPlayerID = msg.m_lPlayerID;
	int  lTaskID    = msg.m_lTaskID;
	ResultType	iResult = msg.m_iResult;
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(!pkGamePlayer || pkGamePlayer->GetMainControllerID() != lPlayerID)
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"角色数值同步消息逻辑错误");
		return;
	}

	if (iResult == RE_TASK_CANCEL_SUCCESS)
	{
		pkGamePlayer->RltCancelTask(lTaskID);
		g_soundMgr.Play2DEx("UI_CANCEL_TASK", 
			AS_ClientSoundManager::SCT_2D);

		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_CANCEL_TASK);
		kGameEvent.SetEventParam(EP_CANCEL_TASK_ID, lTaskID);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
	else
	{
		LC_Task* pkTask = LC_ClientTaskManager::GetSingletonPtr()->GetTask(lTaskID);
		if (pkTask == NULL)
			return;
		//if (pkTask->GetTaskType() == STT_GAMESTORY && pkTask->IsGuildRelated())
		//{
		//	pkGamePlayer->ReqCancelTask(lTaskID);
		//}
		else if (pkTask->GetTaskType() != STT_INVISIBLE_TASK)
			SendLogicInfoMessage(iResult);
	}

}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltShareTask(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

// 	//如果还未进入游戏，则退出
// 	if (eGameStage != ST_GAME_STAGE_GAME)
// 		return;


	MG_RLT_ShareTask msg;
	if(msg.Decode(pMsg,wSize))
	{
		object_id_type lResponderID	= msg.m_lResponderID;
		int  lTaskID		= msg.m_lTaskID;
		object_id_type lPublisherID	= msg.m_lPublisherID;
		int  iResult		= msg.m_iResult;
		StringType kPublisherName = STDSTR2TPSTR(msg.m_strPublisherName);
		StringType kResponderName = STDSTR2TPSTR(msg.m_strResponderName);

		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

		if(!pkGamePlayer) //|| pkGamePlayer->GetID() != lPlayerID)
		{
			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"角色数值同步消息逻辑错误");
			return;
		}

		MG_MessageCenter* pkMessageCenter = MG_MessageCenter::GetSingletonPtr();
		if(!pkMessageCenter)
			return;

		if (pkGamePlayer->GetID() == lPublisherID)
		{
			if (iResult == RE_SUCCESS)
				SendLogicInfoMessage(RE_TASK_SHARE_SUCCESS);

			if (iResult == RE_TASK_SHARE_NOT_IN_RANGE
				||iResult == RE_TASK_SHARE_TASK_IS_FULL
				||iResult == RE_TASK_SHARE_TASK_IS_PROCESS
				||iResult == RE_TASK_SHARE_TASK_IS_FINISH)
			{
				SendLogicInfoMessage(iResult,kResponderName.c_str());
			}			

			if (iResult == RE_TASK_SHARE_CANT_ACCEPT_TASK)
			{
				LC_Task* pkTask = LC_ClientTaskManager::GetSingletonPtr()->GetTask(msg.m_lTaskID);
				if (pkTask == NULL)
					return;

				SendLogicInfoMessage(RE_TASK_START_FAILED_CANT_ACCEPT_TASK,pkTask->GetTaskName().c_str(),kResponderName.c_str());
			}
		}

		if (pkGamePlayer->GetID() == lResponderID)
		{
			if (iResult == RE_TASK_SHARE_TASK_IS_FULL)
				SendLogicInfoMessage(iResult,CTSTR_YOU);

			if (iResult == RE_TASK_SHARE_CANT_ACCEPT_TASK)
			{
				LC_Task* pkTask = LC_ClientTaskManager::GetSingletonPtr()->GetTask(msg.m_lTaskID);
				if (pkTask == NULL)
					return;

				SendLogicInfoMessage(iResult,kPublisherName.c_str(),pkTask->GetTaskName().c_str());
			}

			if (iResult == RE_SUCCESS)
			{
				LC_ClientTaskMap* pkTaskMap = (LC_ClientTaskMap*)pkGamePlayer->GetTaskMap();
				if (pkTaskMap)
					pkTaskMap->AddRequestShareTask(lTaskID,lPublisherID);
			}
		}
	}
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltConfirmShareTask(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

// 	//如果还未进入游戏，则退出
// 	if (eGameStage != ST_GAME_STAGE_GAME)
// 		return;


	MG_RLT_ConfirmShareTask msg;
	if(msg.Decode(pMsg,wSize))
	{

		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

		if (!pkGamePlayer)
		{
			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"角色数值同步消息逻辑错误");
			return;
		}
		/*if(!pkGamePlayer || pkGamePlayer->GetID() != lPlayerID)
		{
			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"角色数值同步消息逻辑错误");
			return;
		}*/
		//接受任务共享者应答
		if (pkGamePlayer->GetID() == msg.m_lResponderID)
		{
			
		}
		//发起任务共享者应答
		if (pkGamePlayer->GetID() == msg.m_lPublisherID)
		{

			if (msg.m_iConfirmResult == RE_TASK_DENY_SHARE
				||msg.m_iConfirmResult == RE_TASK_ACCEPT_SHARE)
			{
				LC_Task* pkTask = LC_ClientTaskManager::GetSingletonPtr()->GetTask(msg.m_lTaskID);
				if (pkTask == NULL)
					return;

				SendLogicInfoMessage(msg.m_iConfirmResult, STDSTR2TPSTR(msg.m_strResponderName), pkTask->GetTaskName().c_str());
			}
		}

	}
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltBillBoardTaskInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//如果还未进入游戏，则退出
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;

	MG_RLT_BillBoardTaskInfo msg;
	if(msg.Decode(pMsg,wSize))
	{
		object_id_type lPlayerID		= msg.m_lPlayerID;
		int  lBillBoardID   = msg.m_lBillBoardID;

		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

		if(!pkGamePlayer || pkGamePlayer->GetMainControllerID() != lPlayerID)
		{
			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"角色数值同步消息逻辑错误");
			return;
		}

		LC_ClientTaskMap* pkTaskMap = (LC_ClientTaskMap*)pkGamePlayer->GetTaskMap();
		if (pkTaskMap == NULL)
			return;

		BillboardTaskData objBillboardData;
		objBillboardData.m_lBillboardID = lBillBoardID;
		for (int i = 0 ;i < MAX_BILLBORAD_PROCESS_COUNT;i++)
		{
			objBillboardData.m_lTaskID[i] = msg.m_lTaskID[i];
		}

		pkTaskMap->AddBillboardDataToCanStarList(objBillboardData);
		long lNPCID = pkGamePlayer->GetLockedNpcID();
		LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
		LC_ClientNPC* pkNPC = (LC_ClientNPC*)pkNPCManager->FindNPC(lNPCID);
		if(pkNPC)
		{
			if ( pkNPC->GetCharType() == lBillBoardID)
			{
				pkTaskMap->SetShowBillboardFlag(true);
			}
		}

		pkTaskMap->UpdateCanAcceptTask();
		pkGamePlayer->UpdateNearNPCTaskState();
	}
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerReqShareMultiuserTask( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

// 	//如果还未进入游戏，则退出
// 	if (eGameStage != ST_GAME_STAGE_GAME)
// 		return;

	MG_REQ_ShareMultiuserTask msg;
	if(msg.Decode(pMsg,wSize))
	{
		long lPlayerID		= msg.m_lPlayerID;
		long lTaskID		= msg.m_lTaskID;
		long lIndex			= msg.m_lTaskControlDateIndex;

		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
		if(!pkGamePlayer)
		{
			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"ReqShareMultiuserTask逻辑错误");
			return;
		}

		LC_ClientTaskMap* pkTaskMap = (LC_ClientTaskMap*)pkGamePlayer->GetTaskMap();
		if (pkTaskMap)
			pkTaskMap->AddRequestShareTask(lTaskID,lPlayerID,TST_MULTIUSER,lIndex);

		LC_Task* pkTask = LC_ClientTaskManager::GetSingletonPtr()->GetTask(lTaskID);
		if (NULL == pkTask)
			return;

		StringType InfoText = "队友邀请你加入" + pkTask->GetTaskName() + "任务";
		SendLogicInfoMessage(RE_TIP_MESSAGE,InfoText.c_str());
	}
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltJoinInMultiuserTask( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

// 	//如果还未进入游戏，则退出
// 	if (eGameStage != ST_GAME_STAGE_GAME)
// 		return;

	MG_RLT_JoinInMultiuserTask msg;
	if(msg.Decode(pMsg,wSize))
	{
		object_id_type lPlayerID		= msg.m_lPlayerID;
		long lTaskID		= msg.m_lTaskID;
		long iResult		= msg.m_iResult;

		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
		if(!pkGamePlayer || pkGamePlayer->GetMainControllerID() != lPlayerID)
		{
			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"RltJoinInMultiuserTask逻辑错误");
			return;
		}

		if (iResult == RE_SUCCESS)
		{
			pkGamePlayer->RltJoinInTask(lTaskID);

			LC_Task* pkTask = LC_ClientTaskManager::GetSingletonPtr()->GetTask(msg.m_lTaskID);
			if (pkTask == NULL)
				return;

			int nTaskTimeLimit = pkTask->GetTimeLimit();

			if (pkTask->GetTaskType() == STT_MAIN_SCENARIO)
			{
				LC_GameEvent kGameEvent;
				kGameEvent.SetEventType(GLET_ACCEPT_TASK);
				kGameEvent.SetEventParam(EP_ACCEPT_TASK_PLAYER_ID, lPlayerID);
				kGameEvent.SetEventParam(EP_ACCEPT_TASK_ID, lTaskID);
				kGameEvent.SetEventParam(EP_ACCEPT_TASK_TIME_LIMIT, nTaskTimeLimit);
				kGameEvent.SetEventParam(EP_ACCEPT_TASK_INDEX, pkTask->GetMainIndex());
				UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
				pkUIManager->HandlerGameEvent(kGameEvent);

				//g_soundMgr.Play2DEx(VARIABLE2STRING(SE_Sys_Mission_Accept));
			}	
		}
	}
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltTaskAssignInfo( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();


	MG_RLT_AssignTaskInfo msg;
	if(msg.Decode(pMsg,wSize))
	{
		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
		if(pkGamePlayer)
		{
			LC_ClientTaskMap* pkTaskMap = (LC_ClientTaskMap*)pkGamePlayer->GetTaskMap();
			if (pkTaskMap == NULL)
				return;
			
			pkTaskMap->GetAssignTaskInfo()->GetDataFromProtocolStruct(msg);
			pkTaskMap->UpdateAssignTaskRoundInfo();
			pkTaskMap->SetDirtyFlag(DIRTY_FLAG_ASSIGN_TASK,true);
			pkTaskMap->AddDirtyFlagRevisionID(DIRTY_FLAG_ASSIGN_TASK);
			pkTaskMap->UpdateAutoStartList();
			pkGamePlayer->UpdateNearNPCTaskState();
			//pkGamePlayer->SetImmediateTaskStart(true);
			//GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"同步AssignInfo信息时间[%f]",GET_PROCESS_TIME);
			LC_GameEvent kGameEvent;
			kGameEvent.SetEventType(GLET_CLIENT_TASK_ASSIGN_INFO_SYNC);
			UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
			pkUIManager->HandlerGameEvent(kGameEvent);
		}
	}
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltDrawLivenessReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RLT_DrawLivenessReward msg;
	if(!msg.Decode(pMsg, wSize))
		return;

	if (msg.m_uiRslt == RE_SUCCESS)
	{
		StringStreamType sStream("");
		sStream << msg.m_uiLevel << "#";
		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerMsgGameEvent(MGPT_RLT_DRAW_LIVENESS_REWARD, sStream.str());
	}
	else
	{
		SendLogicInfoMessage(msg.m_uiRslt);
	}
}


void HL_FromGameServer::_handlerRltTaskOneKeyOp(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RLT_TaskOneKeyOP rltMsg;
	
	if(!rltMsg.Decode(pMsg, wSize))
		return;


	//LC_GameEvent kGameEvent;
	//kGameEvent.SetEventType(GLET_CLIENT_DAYLY_TASK);
	//kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, MGPT_RLT_TASK_ONE_KEY_OP);
	//kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, rltMsg.m_iTaskID);

	//UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	//pkUIManager->HandlerGameEvent(kGameEvent);

	LC_ClientTaskManager* pkTaskManager = (LC_ClientTaskManager*)LC_ClientTaskManager::GetSingletonPtr();
	if (pkTaskManager == NULL)
		return;	
	if (rltMsg.m_uiRslt == RE_SUCCESS)
	{
		StringStreamType sStream("");
		sStream << rltMsg.m_iTaskID << "#";
		sStream << rltMsg.m_iOPCode << "#";
		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerMsgGameEvent(MGPT_RLT_TASK_ONE_KEY_OP, sStream.str());
	}
	SendLogicInfoMessage(rltMsg.m_uiRslt);
}

void HL_FromGameServer::_handlerRltTaskAssignOneKeyOp(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RLT_TaskAssginOneKeyOP msg;

	if(!msg.Decode(pMsg, wSize))
		return;

	if (msg.m_uiRslt == RE_SUCCESS)
	{
		StringStreamType sStream("");
		sStream << msg.m_uiAssignType;
		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerMsgGameEvent(MGPT_RLT_TASK_ASSIGN_ONE_KEY_OP, sStream.str());
	}
	SendLogicInfoMessage(msg.m_uiRslt);	
}

void HL_FromGameServer::_handlerRltReduceTaskDiffStar(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RLT_ReduceTaskDiffStar msg;

	if(!msg.Decode(pMsg, wSize))
		return;

	ResultType iResult = msg.m_uiRslt;
	int32_t iTaskID = msg.m_iTaskID;
	int32_t iDiffStar = msg.m_uiStar;
	if (iResult == RE_SUCCESS)
	{
		StringStreamType sStream("");
		sStream << iTaskID << "#" << iDiffStar << "#";
		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerMsgGameEvent(MGPT_RLT_REDUCE_TASK_DIFF_STAR, sStream.str());
	}
	else
	{
		SendLogicInfoMessage(msg.m_uiRslt);	
	}
}

void HL_FromGameServer::_handlerRltPromoteTaskRewardStar(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RLT_PromoteTaskRewardStar msg;

	if(!msg.Decode(pMsg, wSize))
		return;

	ResultType iResult = msg.m_uiRslt;
	int32_t iTaskID = msg.m_iTaskID;
	int32_t iDiffStar = msg.m_uiStar;
	if (iResult == RE_SUCCESS)
	{
		StringStreamType sStream("");
		sStream << iTaskID << "#" << iDiffStar << "#";
		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerMsgGameEvent(MGPT_RLT_PROMOTE_TASK_REWARD_STAR, sStream.str());
	}
	else
	{
		SendLogicInfoMessage(msg.m_uiRslt);	
	}	
}

void HL_FromGameServer::_handlerRltTaskDrawRoundOffReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RLT_TaskDrawRoundOffReward msg;

	if(!msg.Decode(pMsg, wSize))
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
		return;

	pkGamePlayer->SetFinishTaskFlag(true);
	SendLogicInfoMessage(msg.m_uiRslt);	
}

void HL_FromGameServer::_handlerRltTaskPhagotrophy(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	LC_ClientGamePlayer* pkPlayer = pkSession->GetPlayer();
	if(NULL == pkPlayer)
		return;
	LC_TaskMap* pkTaskMap = pkPlayer->GetTaskMap();
	if(NULL == pkTaskMap)
		return;

	MG_RESP_TaskPhagotrophy msg;
	if(!msg.Decode(pMsg, wSize))
		return;
	if (msg.m_uiRslt == RE_SUCCESS)
	{
		LC_Task* pkTask = LC_ClientTaskManager::GetSingleton().GetTask(msg.m_lPhagocytableID);
		LC_ProcessingTaskEntry* pkEntry = pkTaskMap->GetProcessingTaskEntryByID(msg.m_lPhagocytableID);
		if (pkTask && pkEntry)
		{
			/*int iExp = pkTask->ComputeExpBonus(pkEntry, pkPlayer, false);
			int iGold = pkTask->ComputeUnBindGoldBonus(pkEntry, pkPlayer, false);
			int iSp = pkTask->ComputeSpBonus(pkEntry, pkPlayer, false);
			SendLogicInfoMessage(RE_TASK_PHAGOTROPHY_SUCCESS, GUISys::Convert::NumToString(iExp*msg.m_fPropo), GUISys::Convert::NumToString(iGold*msg.m_fPropo), GUISys::Convert::NumToString(iSp*msg.m_fPropo));*/
			int Propo = Float2Int(msg.m_fPropo * 100);
			//StringType PropoStr = Int2String(Propo) + "%";
			SendLogicInfoMessage(RE_TASK_PHAGOTROPHY_SUCCESS, Propo);

			g_soundMgr.Play2DEx(VARIABLE2STRING(SE_Sys_CrusadeMission_Merge));
		}
		else
			SendLogicInfoMessage(msg.m_uiRslt);
	}
	else
		SendLogicInfoMessage(msg.m_uiRslt);
}

void HL_FromGameServer::_handlerSyncTaskTypeData(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncTaskTypeData msg;
	if (!msg.Decode(pMsg, wSize))
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if (NULL == pkGamePlayer)
		return;

	if (pkGamePlayer->GetMainControllerID() != msg.m_lPlayerID)
		return;

	LC_ClientTaskMap* pkTaskMap = (LC_ClientTaskMap*)pkGamePlayer->GetTaskMap();
	if (pkTaskMap == NULL)
		return;
	//GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"同步TaskTypeData信息时间[%f]",GET_PROCESS_TIME);
	pkTaskMap->GetDataFromProtocolStruct(msg.m_kTaskTypeDataInfo);
	pkTaskMap->UpdateAutoStartList();
	pkGamePlayer->UpdateNearNPCTaskState();
	//pkGamePlayer->SetImmediateTaskStart(true);

}

void HL_FromGameServer::_handlerSyncTaskRank(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncTaskRank msg;
	if (!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SyncTaskRank解析错误");
		return;
	}

	LC_ClientTaskManager* pkTaskManager = (LC_ClientTaskManager*)LC_ClientTaskManager::GetSingletonPtr();
	pkTaskManager->SetCurTaskRank(msg.m_nCurrentTaskRank);
	pkTaskManager->SetTaskChangeState(msg.m_nChangeState);
	pkTaskManager->UpdateDirtyFlag();
}

void HL_FromGameServer::_handlerSyncTaskChapterRecord(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncTaskChapterRecord msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SyncTaskChapterRecord解析错误");
		return;
	}

	LC_ClientTaskManager* pkTaskManager = (LC_ClientTaskManager*)LC_ClientTaskManager::GetSingletonPtr();
	pkTaskManager->SyncChapterRecord(msg);
	pkTaskManager->UpdateDirtyFlag();
}

void HL_FromGameServer::_handlerSyncMainTaskReachRecord(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncMainTaskReachReward msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SyncMainTaskReachReward解析错误");
		return;
	}

	LC_ClientTaskManager* pkTaskManager = (LC_ClientTaskManager*)LC_ClientTaskManager::GetSingletonPtr();
	pkTaskManager->SyncReachRecord(msg);
	pkTaskManager->UpdateDirtyFlag();
}

void HL_FromGameServer::_handlerRltTaskChapterReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_TakeChapterReward rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_RLT_TakeChapterReward解析错误");
		return;
	}

	if(rltMsg.m_lResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(rltMsg.m_lResult);
	}
	else
	{
		SC_ClientScriptInterface* pkClientInterface = SC_ClientScriptInterface::GetSingletonPtr();
		SC_ClientSkillInterface* pkInterface = pkClientInterface->GetSkillInterface();
		pkInterface->PlayTaskChapterOpenFx(++rltMsg.m_lChapterID);
	}
}

void HL_FromGameServer::_handlerRltTaskReachReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_MainTaskReachReward rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_RLT_MainTaskReachReward解析错误");
		return;
	}

	if(rltMsg.m_lResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(rltMsg.m_lResult);
	}
}

void HL_FromGameServer::_handlerSyncNextRefreshRewardTaskTime(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncNextRefreshRewardTaskTime rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SyncNextRefreshRewardTaskTime解析错误");
		return;
	}

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_REWARDTASK_NEXT_REFRESHTIME_NOTIFY);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, rltMsg.m_nNextRefreshTime);
	
	UI_UIManager::GetSingletonPtr()->HandlerGameEvent(kGameEvent);
}
