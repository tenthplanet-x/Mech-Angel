#include "LC_ClientMailManager.h"
#include "UT_Timer.h"

//------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltMailAsset(NW_Game2ClientSession* pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_MailAsset kMsg;
	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "获取邮件资产结果协议解析错误 MG_RLT_MailAsset");	
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return;
	}

	LC_ClientMailManager* pkClientMailManager = LC_ClientMailManager::GetSingletonPtr();
	if ( NULL == pkClientMailManager )
	{
		return;
	}

	pkClientMailManager->ReceiveMailSimpleInfo(kMsg);
	pkClientMailManager->GetMailAsset()->Sort();
	pkClientMailManager->GetMailAsset()->UpdateNotReadMailCount();

}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltReadMail(NW_Game2ClientSession* pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_ReadMail kMsg;
	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_RLT_ReadMail解析错误");	
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return;
	}

	LC_ClientMailManager* pkClientMailManager = LC_ClientMailManager::GetSingletonPtr();
	if ( NULL == pkClientMailManager )
	{
		return;
	}

	pkClientMailManager->ReceiveMail(kMsg.m_kMailInfo);
	pkClientMailManager->GetMailAsset()->Sort();
}

//------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltWriteMail(NW_Game2ClientSession* pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_WriteMail kMsg;
	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "写信结果协议解析错误 MG_RLT_WriteMail");	
		return;
	}

	LC_ClientMailManager* pkClientMailManager = LC_ClientMailManager::GetSingletonPtr();
	if ( NULL == pkClientMailManager )
	{
		return;
	}
	if (RE_WRITEMAIL_SUCCESS == kMsg.m_iResultType)
	{

		pkClientMailManager->CompleteWriteMail();
		pkClientMailManager->SetCompleteFlag(true);
	}
	else
	{
		pkClientMailManager->SetSendingFlag(false);
	}
	
	/*	LC_ClientMailManager* pkClientMailManager = LC_ClientMailManager::GetSingletonPtr();
		if ( NULL == pkClientMailManager )
		{
			return;
		}

		pkClientMailManager->CompleteWriteMail();
		pkClientMailManager->SetCompleteFlag(true);*/

	SendLogicInfoMessage(kMsg.m_iResultType);
}

void HL_FromGameServer::_handlerRltItemAffix(NW_Game2ClientSession* pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_GetItemAffix kMsg;
	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "取附件结果协议解析错误 MG_RLT_GetItemAffix");	
		return;
	}

	if (RE_GETMAIL_ITEM_AFFIX_SUCCESS == kMsg.m_iResultType)
	{
		// 如果成功的话
		LC_ClientMailManager* pkClientMailManager = LC_ClientMailManager::GetSingletonPtr();
		if (NULL == pkClientMailManager)
		{
			return;
		}

		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_CLIENT_MAIL_GETITEMAFFIX_SUCCESS);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, kMsg.m_iIndex);
		UI_UIManager::GetSingletonPtr()->HandlerGameEvent(kGameEvent);

		pkClientMailManager->GetItemAffix(kMsg.m_iMailID, kMsg.m_iIndex);

		LC_MailAsset* pkMailAsset = pkClientMailManager->GetMailAsset();
		LC_MailInfo* pkMail =pkMailAsset->GetMailByMailID(kMsg.m_iMailID);
		if (NULL != pkMail)
			pkMail->SetReadFlag(true);
		pkMailAsset->UpdateNotReadMailCount();
		pkMailAsset->AddDirtyFlagRevisionID();
	}

	SendLogicInfoMessage(kMsg.m_iResultType);
}

void HL_FromGameServer::_handlerRltCashAffix(NW_Game2ClientSession* pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_GetCashAffix kMsg;
	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "取附件结果协议解析错误 MG_RLT_GetCashAffix");	
		return;
	}

	// 如果成功的话
	LC_ClientMailManager* pkClientMailManager = LC_ClientMailManager::GetSingletonPtr();
	if (NULL == pkClientMailManager)
	{
		return;
	}
	pkClientMailManager->GetCashAffix(kMsg.m_iMailID);

	SendLogicInfoMessage(kMsg.m_iResultType);
}

void HL_FromGameServer::_handlerRltUntreadMail(NW_Game2ClientSession* pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_UntreadMail kMsg;
	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "退信结果协议解析错误 MG_RLT_UntreadMail");	
		return;
	}

	if (RE_SUCCESS == kMsg.m_iResultType)
	{
		LC_ClientMailManager* pkClientMailManager = LC_ClientMailManager::GetSingletonPtr();
		if (NULL == pkClientMailManager)
		{
			return;
		}
		pkClientMailManager->DeleteMail(kMsg.m_lMailID);
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_CLIENT_MAIL_DELETE_SUCCESS);
		UI_UIManager::GetSingletonPtr()->HandlerGameEvent(kGameEvent);
	}

	SendLogicInfoMessage(kMsg.m_iResultType);
}

void HL_FromGameServer::_handlerRltDeleteMail(NW_Game2ClientSession* pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_DeleteMail kMsg;
	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "删除信件结果协议解析错误 MG_RLT_DeleteMail");	
		return;
	}

	if (RE_SUCCESS == kMsg.m_iResultType)
	{
		LC_ClientMailManager* pkClientMailManager = LC_ClientMailManager::GetSingletonPtr();
		if (NULL == pkClientMailManager)
		{
			return;			
		}
		pkClientMailManager->DeleteMail(kMsg.m_lMailID);

		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_CLIENT_MAIL_DELETE_SUCCESS);
		UI_UIManager::GetSingletonPtr()->HandlerGameEvent(kGameEvent);
		SendLogicInfoMessage(RE_DELETEMAIL_SUCCESS);
	}
	else
	{
		SendLogicInfoMessage(kMsg.m_iResultType);
	}	
}

void HL_FromGameServer::_handlerRltAddItemToMailAffix(NW_Game2ClientSession* pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_AddItemToMailAffix kMsg;

	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "添加信件附件结果协议解析错误 MG_RLT_AddItemToMailAffix");	
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return;
	}

	if (RE_SUCCESS == kMsg.m_iResultType)
	{
		LC_ClientMailManager* pkClientMailManager = LC_ClientMailManager::GetSingletonPtr();
		if (NULL == pkClientMailManager)
		{
			return;
		}
		pkClientMailManager->GetClientItemAffixInfo()->AddItemAffix(
			kMsg.m_iTargetPos, kMsg.m_iPackType, kMsg.m_iItemIndex);

		LC_BackPackEntry* pkEntry = pkGamePlayer->GetPackAsset()->GetBackPackEntry(kMsg.m_iPackType, kMsg.m_iItemIndex);
		if (NULL == pkEntry)
		{
			return;
		}
		pkEntry->SetLocked(true);
		pkGamePlayer->GetPackAsset()->AddDirtyFlagRevisionID(kMsg.m_iPackType);
	}

	SendLogicInfoMessage(kMsg.m_iResultType);
}

void HL_FromGameServer::_handlerRltDelItemFromMailAffix(NW_Game2ClientSession* pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_DeletItemFromMailAffix kMsg;

	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "删除信件附件结果协议解析错误 MG_RLT_AddItemToMailAffix");	
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return;
	}

	if (RE_SUCCESS == kMsg.m_iResultType)
	{
		LC_ClientMailManager* pkClientMailManager = LC_ClientMailManager::GetSingletonPtr();
		if (NULL == pkClientMailManager)
		{
			return;
		}
		pkClientMailManager->GetClientItemAffixInfo()->DeleletItemAffix(kMsg.m_iTargetPos);

		LC_BackPackEntry* pkEntry = pkGamePlayer->GetPackAsset()->GetBackPackEntry(kMsg.m_iPackType, kMsg.m_iItemIndex);
		if (NULL == pkEntry)
		{
			return;
		}
		pkEntry->SetLocked(false);
		pkGamePlayer->GetPackAsset()->AddDirtyFlagRevisionID(kMsg.m_iPackType);
	}
	SendLogicInfoMessage(kMsg.m_iResultType);
}

void HL_FromGameServer::_handlerRltPayForMail(NW_Game2ClientSession* pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_PayForMail kMsg;
	if ( !kMsg.Decode(pMsg, wSize) )
	{		
		return;
	}

	if (RE_SUCCESS == kMsg.m_iResultType)
	{
		LC_ClientMailManager* pkClientMailManager = LC_ClientMailManager::GetSingletonPtr();
		if (NULL == pkClientMailManager)
		{
			return;			
		}
		LC_MailAsset* pkMailAsset = pkClientMailManager->GetMailAsset();
		if (NULL == pkMailAsset)
		{
			return;				
		}
		pkMailAsset->PayForMail(kMsg.m_uiMailID);
	}
	else
	{
		SendLogicInfoMessage(kMsg.m_iResultType);
	}	
}

void HL_FromGameServer::_handlerRltIsExistNewMail(NW_Game2ClientSession* pkSession, char* pMsg, uint16_t wSize)
{	
	MG_RLT_IsExitNewMail kMsg;
	if ( !kMsg.Decode(pMsg, wSize) )
	{		
		return;
	}

	LC_ClientMailManager* pkClientMailManager = LC_ClientMailManager::GetSingletonPtr();
	if (NULL == pkClientMailManager)
	{
		return;
	}
	LC_MailAsset* pkMailAsset = pkClientMailManager->GetMailAsset();
	if (NULL == pkMailAsset)
	{
		return;
	}
	//BUG #27208 【优化】【邮件】游戏中的邮件要推送角标时，打开邮箱已经有邮件了，不要玩家等待
	//全局邮件再经过一定的延迟之后才进行相关的操作
	if (kMsg.m_globalmail)
	{
		static int iTimerID = 0;
		UT_TimerManager *pkTimerMgr = UT_TimerManager::GetSingletonPtr();
		if (iTimerID > 0)
		{
			pkTimerMgr->CancelTimer(iTimerID);
			iTimerID = 0;
		}
		int iDelay = UT_MathBase::RandInRangeInt(5, 10);
		iTimerID = pkTimerMgr->DelayCall((int64_t)iDelay * 1000, LC_ClientMailManager::PostGetGlobalMail, (void (*)(Utility::timer_id_type))NULL);
		return;
	}
	pkMailAsset->SetNotReadMailCount(kMsg.m_notreadCount);
	pkClientMailManager->SetMailHintFlag(true);
	//g_soundMgr.Play2DEx(VARIABLE2STRING(SE_Sys_Mail));
	if (kMsg.m_iCount >= MAX_WARNING_MAIL_NUM)
	{
		pkClientMailManager->SetMailboxFullFlag(true);
		if(!kMsg.m_bHint)
			SendLogicInfoMessage(RE_NEWMAIL_FAIL_FULL);
	}
	else
	{
		pkClientMailManager->SetMailboxFullFlag(false);
		if(!kMsg.m_bHint)
			SendLogicInfoMessage(RE_NEWMAIL_SUCCESS);
	}

	// 正在读取邮件，不更新邮件内容
	if (!pkClientMailManager->GetIsReadingMailFlag())
	{
		pkClientMailManager->RequestMailAsset();
	}

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_CLIENT_NEWMAIL_NOTIFY);
	UI_UIManager::GetSingletonPtr()->HandlerGameEvent(kGameEvent);

	/*if ( !kMsg.m_bHint )
	{
		SendLogicInfoMessage(RE_NEWMAIL_SUCCESS);
	}	*/
}