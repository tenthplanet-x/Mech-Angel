#include "NW_ServerSession.h"
#include "HL_PacketHandler.h"
#include "MG_TaskProtocol.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "GameServerApp.h"
#include "LC_SequenceManager.h"
#include "LC_ServerTaskManager.h"
#include "LC_ServerTaskMap.h"
#include "LC_ServerTask.h"
#include "MG_Common.h"
#include "LC_GameEventManager.h"

using namespace Protocol;
using namespace GameLogic;

namespace Protocol
{
#define this_source_type SERVER_TYPE_CLIENT

//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_ACCEPT_TASK, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (pkPlayer == NULL)
		return;

	MG_REQ_AcceptTask reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	int lTaskID = reqMsg.m_lTaskID;
	pkPlayer->AcceptTaskOP(lTaskID);
}
//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_FINISH_TASK, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (pkPlayer == NULL)
		return;

	MG_REQ_FinishTask reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer->FinishTaskOP(reqMsg.m_lTaskID);
}
//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_CANCEL_TASK, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (pkPlayer == NULL)
		return;

	MG_REQ_CancelTask reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_CancelTask rltMsg;
	rltMsg.m_lPlayerID = pkPlayer->GetID();
	rltMsg.m_lTaskID = reqMsg.m_lTaskID;
	rltMsg.m_iResult = pkPlayer->CancleTaskOP(reqMsg.m_lTaskID, reqMsg.m_iCancelType);
	pkPlayer->SendMsgToClient(MGPT_RLT_CANCEL_TASK, &rltMsg);
}
//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_SHARE_TASK, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (pkPlayer == NULL)
		return;

	MG_REQ_ShareTask reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_Message errorMsg;
	errorMsg.m_iMessageID = RE_TASK_SHARE_CANT_ACCEPT_TASK;

	pkPlayer->SendMsgToClient(MGPT_MESSAGE, &errorMsg);
}
//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_CONFIRM_SHARE_TASK, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;


	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (pkPlayer == NULL)
		return;

	MG_REQ_ConfirmShareTask reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	int lTaskID    = reqMsg.m_lTaskID;
	object_id_type lPublisherID = reqMsg.m_lPublisherID;

	LC_PlayerGroupBase* pGroup = pkPlayer->GetPlayerGroup();
	if (!pGroup)
	{
		return;
	}
	if (reqMsg.m_iConfirmResult == RE_TASK_ACCEPT_SHARE)
	{
		//发送网络应答给共享任务发布者
		MG_RLT_ConfirmShareTask rltConfirmMsg;
		rltConfirmMsg.m_lResponderID = pkPlayer->GetID();
		rltConfirmMsg.m_lTaskID = lTaskID;
		rltConfirmMsg.m_lPublisherID = lPublisherID;
		rltConfirmMsg.m_iConfirmResult = RE_TASK_ACCEPT_SHARE;
		TPSTR2STDSTR(pkPlayer->GetOwnerCharName(), rltConfirmMsg.m_strResponderName);

		LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
		LC_ServerPlayer* pkPublisherPlayer = (LC_ServerPlayer*)(pkServerPlayerManager->FindPlayer(lPublisherID));
		if (NULL != pkPublisherPlayer)
		{
			pkPublisherPlayer->SendMsgToClient(MGPT_RLT_CONFIRM_SHARE_TASK, &rltConfirmMsg);
		}
	}

	if (reqMsg.m_iConfirmResult == RE_TASK_DENY_SHARE)
	{
		MG_RLT_ConfirmShareTask rltMsg;
		rltMsg.m_lResponderID = pkPlayer->GetID();
		rltMsg.m_lTaskID = lTaskID;
		rltMsg.m_lPublisherID = lPublisherID;
		rltMsg.m_iConfirmResult = RE_TASK_DENY_SHARE;
		TPSTR2STDSTR(pkPlayer->GetOwnerCharName(), rltMsg.m_strResponderName);

		LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
		LC_ServerPlayer* pkPublisherPlayer = (LC_ServerPlayer*)(pkServerPlayerManager->FindPlayer(lPublisherID));
		if (NULL != pkPublisherPlayer)
		{
			pkPublisherPlayer->SendMsgToClient(MGPT_RLT_CONFIRM_SHARE_TASK, &rltMsg);
		}
	}


}
//------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_NOTICE_TASK_EVENT, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;


	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (pkPlayer == NULL)
		return;

	MG_REQ_NoticeTaskEvent reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	//object_id_type lPlayerID  = reqMsg.m_lPlayerID;
	int lTaskID    = reqMsg.m_lTaskID;
	int iTaskEvent = reqMsg.m_iTaskEvent;
	int lParam1	= reqMsg.m_lParam1;
	int lParam2	= reqMsg.m_lParam2;


	//GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "ReqNoticeTaskEvent Begin TaskID[%d]:PlayerID[%d]:TaskEvent[%d]", lTaskID, pkPlayer->GetID(), iTaskEvent);

	LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
	LC_GameEvent* pkGameEvent = pkEventManager->AllocGameEvent();
	if (pkGameEvent == NULL)
		return;

	if (iTaskEvent == GLET_PLAYER_ARRIVE_AREA)
	{
		pkGameEvent->SetEventType(GLET_PLAYER_ARRIVE_AREA);
		pkGameEvent->SetEventParam(EP_ARRIVE_AREA_PLAYER_ID, pkPlayer->GetID());
		pkGameEvent->SetEventParam(EP_ARRIVE_AREA_ID, lParam1);
		pkGameEvent->SetEventParam(EP_ARRIVE_AREA_PARAM, lParam2);
		pkGameEvent->SetEventParam(EP_ARRIVE_AREA_MAP_ID, pkPlayer->GetMapResID());
		pkEventManager->InsertGameEvent(pkGameEvent);
		return;
	}
	else if (iTaskEvent == GLET_TALK_TO_NPC)
	{
		pkGameEvent->SetEventType(GLET_TALK_TO_NPC);
		pkGameEvent->SetEventParam(EP_TALK_TO_NPC_PLAYER_ID, pkPlayer->GetID());
		pkGameEvent->SetEventParam(EP_TALK_TO_NPC_CHAR_TYPE_ID, lParam1);
		pkGameEvent->SetEventParam(EP_TALK_TO_NPC_DIALOG_PARAM, lParam2);
		pkGameEvent->SetEventParam(EP_TALK_TO_NPC_MAPID, pkPlayer->GetMapResID());
		pkEventManager->InsertGameEvent(pkGameEvent);
		return;
	}
	else if (iTaskEvent == GLET_USE_ITEM)
	{
		pkGameEvent->SetEventType(GLET_USE_ITEM);
		pkGameEvent->SetEventParam(EVENT_SEND_TYPE_PARAM, GENT_NONE);
		pkGameEvent->SetEventParam(EP_USE_ITEM_PLAYER_ID, pkPlayer->GetID());
		pkGameEvent->SetEventParam(EP_USE_ITEM_TYPE_ID, lParam1);
		pkGameEvent->SetEventParam(EP_USE_ITEM_MAP_ID, pkPlayer->GetMapResID());
		pkEventManager->InsertGameEvent(pkGameEvent);
		if (lParam1 == REDUCE_TRANSCRIPTION_ENTERTIMES_ITEM)
		{
			pkPlayer->ReduceTranscriptionEnterTimes(lParam2);
		}
		return;
	}

	pkEventManager->FreeGameEvent(pkGameEvent);
}
//------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_BILLBOARD_TASK_INFO, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (pkPlayer == NULL)
		return;

	MG_REQ_InitBillBoardTask reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	int llBillBaordID   = reqMsg.m_lBillBaordID;

	//GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "ReqInitBillBoardTask Begin PlayerID[%d]:BillBaordID[%d]", pkPlayer->GetID(), llBillBaordID);

	LC_TaskMap* pkTaskMap = pkPlayer->GetTaskMap();
	if (pkTaskMap == NULL)
		return;

	ResultType iResult = pkTaskMap->InitBillboardTaskList(llBillBaordID);
	if (iResult == RE_TASK_BILLBOARD_INIT_SUCCESS)
	{
		BillboardTaskData* pkTaskData = pkTaskMap->GetBillboardDataFormCanStarList(llBillBaordID);
		if (pkTaskData)
		{
			MG_RLT_BillBoardTaskInfo rltMsg;
			rltMsg.m_lPlayerID = pkPlayer->GetID();
			rltMsg.m_lBillBoardID = llBillBaordID;
			for (int i = 0; i < MAX_BILLBORAD_PROCESS_COUNT; i++)
			{
				rltMsg.m_lTaskID[i] = pkTaskData->m_lTaskID[i];
			}

			pkPlayer->SendMsgToClient(MGPT_RLT_BILLBOARD_TASK_INFO, &rltMsg);
			return;
		}

		iResult = RE_TASK_SYSTEM_ERROR;
	}

	MG_Message errorMsg;
	errorMsg.m_iMessageID = iResult;

	pkPlayer->SendMsgToClient(MGPT_MESSAGE, &errorMsg);
}
//------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_DRAW_LIVENESS_REWARD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;


	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (pkPlayer == NULL)
		return;

	MG_REQ_DrawLivenessReward reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}


	MG_RLT_DrawLivenessReward rltMsg;
	rltMsg.m_uiLevel = reqMsg.m_uiLevel;
	rltMsg.m_uiType = reqMsg.m_uiType;
	LC_ServerTaskMap* pkTaskMap = (LC_ServerTaskMap*)pkPlayer->GetTaskMap();

	rltMsg.m_uiRslt = pkTaskMap->DrawLevelReward(reqMsg.m_uiType, reqMsg.m_uiLevel);

	pkPlayer->SendMsgToClient(MGPT_RLT_DRAW_LIVENESS_REWARD, &rltMsg);

}
//------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_REDUCE_TASK_DIFF_STAR, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (pkPlayer == NULL)
		return;

	MG_REQ_ReduceTaskDiffStar reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerTaskManager* pkTaskManager = (LC_ServerTaskManager*)LC_TaskManager::GetSingletonPtr();
	if (pkTaskManager == NULL)
		return;

	LC_ServerTask* pkTask = (LC_ServerTask*)pkTaskManager->GetTask(reqMsg.m_iTaskID);
	if (pkTask == NULL)
		return ;
	LC_ServerTaskMap* pkTaskMap = (LC_ServerTaskMap*)pkPlayer->GetTaskMap();
	if (NULL == pkTaskMap)
	{
		return;
	}

	GameLogic::LC_ProcessingTaskEntry*	pkProcessEntry = pkTaskMap->GetProcessingTaskEntryByID(reqMsg.m_iTaskID);

	if (NULL == pkProcessEntry)
	{
		return;
	}


	MG_RLT_ReduceTaskDiffStar rltMsg;
	rltMsg.m_iTaskID = reqMsg.m_iTaskID;
	rltMsg.m_uiRslt = pkTask->ReduceDiffStar(pkPlayer, pkProcessEntry);

	rltMsg.m_uiStar = pkProcessEntry->GetTaskParam().GetDiffiStar();

	pkPlayer->SendMsgToClient(MGPT_RLT_REDUCE_TASK_DIFF_STAR, &rltMsg);

}
//------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_PROMOTE_TASK_REWARD_STAR, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (pkPlayer == NULL)
		return;

	MG_REQ_PromoteTaskRewardStar reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerTaskManager* pkTaskManager = (LC_ServerTaskManager*)LC_TaskManager::GetSingletonPtr();
	if (pkTaskManager == NULL)
		return;

	LC_ServerTask* pkTask = (LC_ServerTask*)pkTaskManager->GetTask(reqMsg.m_iTaskID);
	if (pkTask == NULL)
		return ;
	LC_ServerTaskMap* pkTaskMap = (LC_ServerTaskMap*)pkPlayer->GetTaskMap();
	if (NULL == pkTaskMap)
	{
		return;
	}

	GameLogic::LC_ProcessingTaskEntry*	pkProcessEntry = pkTaskMap->GetProcessingTaskEntryByID(reqMsg.m_iTaskID);
	if (NULL == pkProcessEntry)
	{
		return;
	}

	MG_RLT_PromoteTaskRewardStar rltMsg;
	rltMsg.m_iTaskID = reqMsg.m_iTaskID;
	rltMsg.m_uiRslt = pkTask->PromoteRewardStar(pkPlayer, pkProcessEntry);

	rltMsg.m_uiStar = pkProcessEntry->GetTaskParam().GetRewardStar();

	pkPlayer->SendMsgToClient(MGPT_RLT_PROMOTE_TASK_REWARD_STAR, &rltMsg);

}
//------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_TASK_ONE_KEY_OP, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;


	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (pkPlayer == NULL)
		return;

	MG_REQ_TaskOneKeyOP reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerTaskManager* pkTaskManager = (LC_ServerTaskManager*)LC_TaskManager::GetSingletonPtr();
	if (pkTaskManager == NULL)
		return;

	LC_ServerTask* pkTask = (LC_ServerTask*)pkTaskManager->GetTask(reqMsg.m_iTaskID);
	if (pkTask == NULL)
		return ;
	LC_ServerTaskMap* pkTaskMap = (LC_ServerTaskMap*)pkPlayer->GetTaskMap();
	if (NULL == pkTaskMap)
	{
		return;
	}

	GameLogic::LC_ProcessingTaskEntry*	pkProcessEntry = pkTaskMap->GetProcessingTaskEntryByID(reqMsg.m_iTaskID);
	if (NULL == pkProcessEntry)
	{
		return;
	}

	MG_RLT_TaskOneKeyOP rltMsg;
	rltMsg.m_iTaskID = reqMsg.m_iTaskID;
	rltMsg.m_iOPCode = reqMsg.m_iOPCode;
	rltMsg.m_iOPPara = reqMsg.m_iOPPara;

	switch (reqMsg.m_iOPCode)
	{
		case LC_TOK_OP_QUICK_FINISH:
			{
				rltMsg.m_uiRslt = pkTask->OneKeyOperationQuickFinish(pkPlayer, pkProcessEntry);
			}
			break;
		case LC_TOK_OP_MULTI_FINISH:
			{
				rltMsg.m_uiRslt = pkTask->OneKeyOperationMultiFinish(pkPlayer, pkProcessEntry);
			}
			break;
		case LC_TOK_OP_COMBO_FINISH:
			{
				rltMsg.m_uiRslt = pkTask->OneKeyOperationComboFinish(pkPlayer, pkProcessEntry);
			}
		default:
			break;
	}

	pkPlayer->SendMsgToClient(MGPT_RLT_TASK_ONE_KEY_OP, &rltMsg);

}
//------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_TASK_ASSIGN_ONE_KEY_OP, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;


	MG_REQ_TaskAssginOneKeyOP reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerTaskManager* pkTaskManager = (LC_ServerTaskManager*)LC_TaskManager::GetSingletonPtr();
	if (pkTaskManager == NULL)
		return;

	LC_ServerTaskMap* pkTaskMap = (LC_ServerTaskMap*)pkPlayer->GetTaskMap();
	if (NULL == pkTaskMap)
	{
		return;
	}

	MG_RLT_TaskAssginOneKeyOP rltMsg;
	rltMsg.m_uiAssignType = reqMsg.m_uiAssignType;
	rltMsg.m_uiRslt = pkTaskMap->OneKeyOperation(reqMsg.m_uiAssignType);

	pkPlayer->SendMsgToClient(MGPT_RLT_TASK_ASSIGN_ONE_KEY_OP, &rltMsg);

}
//------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_TASK_DRAW_ROUND_OFF_REWARD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (pkPlayer == NULL)
		return;

	MG_REQ_TaskAssginOneKeyOP reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerTaskManager* pkTaskManager = (LC_ServerTaskManager*)LC_TaskManager::GetSingletonPtr();
	if (pkTaskManager == NULL)
		return;

	LC_ServerTaskMap* pkTaskMap = (LC_ServerTaskMap*)pkPlayer->GetTaskMap();
	if (NULL == pkTaskMap)
	{
		return;
	}

	MG_RLT_TaskAssginOneKeyOP rltMsg;
	rltMsg.m_uiAssignType = reqMsg.m_uiAssignType;
	rltMsg.m_uiRslt = pkTaskMap->DrawAssignRoundOffReward(reqMsg.m_uiAssignType);

	pkPlayer->SendMsgToClient(MGPT_RLT_TASK_DRAW_ROUND_OFF_REWARD, &rltMsg);

}
//------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_TASK_PHAGOTROPHY_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;


	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (pkPlayer == NULL)
		return;

	MG_REQ_TaskPhagotrophy reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerTaskManager* pkTaskManager = (LC_ServerTaskManager*)LC_TaskManager::GetSingletonPtr();
	if (pkTaskManager == NULL)
		return;

	MG_RESP_TaskPhagotrophy respMsg;
	respMsg.m_lPhagotrophID = reqMsg.m_lPhagotrophID;
	respMsg.m_lPhagocytableID = reqMsg.m_lPhagocytableID;
	respMsg.m_lType = reqMsg.m_lType;
	respMsg.m_uiRslt = pkTaskManager->Phagotrophy(pkPlayer, reqMsg.m_lPhagotrophID, reqMsg.m_lPhagocytableID, reqMsg.m_lType, respMsg.m_fPropo);

	pkPlayer->SendMsgToClient(MGPT_RLT_TASK_PHAGOTROPHY_RESP, &respMsg);
}

ON_HANDLE(MGPT_REQ_TAKECHAPTERREWARD, this_source_type, false)
{
	/*
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (pkPlayer == NULL)
	{
		return;
	}

	MG_REQ_TakeChapterReward reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_TakeChapterReward respMsg;
	respMsg.m_lChapterID = reqMsg.m_lChapterID;
	respMsg.m_lResult = pkPlayer->TaskChapterRewardOP(reqMsg.m_lChapterID);
	pkPlayer->SendMsgToClient(MGPT_RLT_TAKECHAPTERREWARD, &respMsg);
	if(RE_SUCCESS == respMsg.m_lResult)
	{
		pkPlayer->SyncTaskChapterRecord();
	}
	*/
}

ON_HANDLE(MGPT_REQ_MAINTASKREACHREWARD, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (pkPlayer == NULL)
	{
		return;
	}

	MG_REQ_MainTaskReachReward reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_MainTaskReachReward respMsg;
	respMsg.m_lReachRewardID = reqMsg.m_lReachRewardID;
	respMsg.m_lResult = pkPlayer->MainTaskReachRewardOP(reqMsg.m_lReachRewardID);
	pkPlayer->SendMsgToClient(MGPT_RLT_MAINTASKREACHREWARD, &respMsg);
	if(RE_SUCCESS == respMsg.m_lResult)
	{
		pkPlayer->SyncMainTaskReachRecord();
	}
}


}
