#include "LC_ClientGuildManager.h"
#include "LC_GuildRaidDataManager.h"
#include "CF_GuildGood.h"

void HL_FromGameServer::_handlerRltGuildAsset(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_GuildAsset kMsg;
	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "解析错误 MG_RLT_GuildAsset");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if (!pkGamePlayer)
	{
		return;
	}

	LC_ClientGuildManager* pkGuildManager = LC_ClientGuildManager::GetSingletonPtr();
	if (pkGuildManager)
	{
		pkGuildManager->UpdateGuildInfo(kMsg.m_kGuildInfo);
	}
}

void HL_FromGameServer::_handlerRltGuildMemberInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_GuildMemberInfo kMsg;
	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "解析错误 MG_RLT_GuildMemberInfo");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if (!pkGamePlayer)
	{
		return;
	}

	LC_ClientGuildManager* pkGuildManager = LC_ClientGuildManager::GetSingletonPtr();
	if (pkGuildManager)
	{
		pkGuildManager->UpdateAllGuildMemberInfo(kMsg.m_kMemberInfo);
	}						 
}

void HL_FromGameServer::_handlerRltCreateGuild(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_CreateGuild kMsg;
	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "解析错误 MG_RLT_CreateGuild");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( !pkGamePlayer )
	{
		return;
	}

	if (kMsg.m_uiResult == RE_SUCCESS)
	{
		LC_ClientGuildManager* pkGuildManager = LC_ClientGuildManager::GetSingletonPtr();
		if (pkGuildManager)
		{
			pkGuildManager->UpdateGuildInfo(kMsg.m_GuidInfo);
			pkGuildManager->GetGuildAsset()->GetGuildInfo().AddDirtyFlagRevisionID();
		}

		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_GUILD_CREATE_SUCCESS_EVENT);
		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}

	SendLogicInfoMessage(kMsg.m_uiResult);
}

void HL_FromGameServer::_handlerNotifyAdd2Guild(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_Notify_Add2Guild kMsg;
	if (!kMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "解析错误 MGPT_NOTIFY_ADD2GUILD");
		return;
	}

	LC_ClientGuildManager* pkGuildManager = LC_ClientGuildManager::GetSingletonPtr();
	if (pkGuildManager)
	{
		pkGuildManager->UpdateGuildInfo(kMsg.m_GuidInfo);
	}
	
	SendLogicInfoMessage(RE_TIP_MESSAGE, GET_MSG(LC_GUILD_CONGRATULATION) + STDSTR2TPSTR(kMsg.m_GuidInfo.szName));

	LC_ClientGamePlayer *pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer != NULL)
	{
		SendLogicInfoMessage(RE_JOIN_GUILD_NEW_MEMBER_NOTIFY, pkGamePlayer->GetCommanderName());
	}

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_CLIENT_GUILD_APPLY_SUCCESS);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerNotifyAddMemberInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_Notify_AddMember kMsg;
	if (!kMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "解析错误 MGPT_NOTIFY_ADD_MEMBER_INFO");
		return;
	}
	
	LC_ClientGuildManager* pkGuildManager = LC_ClientGuildManager::GetSingletonPtr();
	if (pkGuildManager)
	{
		pkGuildManager->AddGuildMemberInfo(kMsg.m_MemberInfo);
		SendLogicInfoMessage(RE_JOIN_GUILD_NEW_MEMBER_NOTIFY, STDSTR2TPSTR(kMsg.m_MemberInfo.szCharName));
	}
}

void HL_FromGameServer::_handlerNotifyDelMemberInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_Notify_DelMember kMsg;
	if (!kMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "解析错误 MG_Notify_DelMember");
		return;
	}

	LC_ClientGuildManager* pkGuildManager = LC_ClientGuildManager::GetSingletonPtr();
	if (pkGuildManager)
	{
		pkGuildManager->DelGuildMemberInfo(kMsg.m_MemberId);
	}
}

void HL_FromGameServer::_handlerNotifyUpdateMemberInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_Notify_MemberInfo kMsg;
	if(!kMsg.Decode(pMsg, wSize))
		return;

	LC_ClientGuildManager* pkGuildManager = LC_ClientGuildManager::GetSingletonPtr();
	if (pkGuildManager)
	{
		pkGuildManager->UpdateGuildMemberInfo(kMsg.m_MemberInfo);
	}
}

void HL_FromGameServer::_handlerRltApplyGuild(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_ApplyGuild kMsg;
	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "解析错误 MG_RLT_ApplyGuild");
		return;
	}

	if(kMsg.m_uiResult == RE_GUILD_REFUSE_ADD)
	{
		SendLogicInfoMessage(kMsg.m_uiResult, STDSTR2TPSTR(kMsg.m_guildname));
	}
	else if(kMsg.m_uiResult == RE_SUCCESS)
	{
		SendLogicInfoMessage(RE_GUILD_APPLY_WAIT_CONFIRM);
	}
	else if (kMsg.m_uiResult == RE_NO_GUILD)
	{
		SendLogicInfoMessage(kMsg.m_uiResult);
	}
	else
	{
		SendLogicInfoMessage(kMsg.m_uiResult);
	}
}

void HL_FromGameServer::_handlerNotifyPositionChange(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_Notify_Guild_Position kMsg;
	if(!kMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "解析错误 MG_Notify_Guild_Position");
		return;
	}

	LC_ClientGuildManager* pkGuildManager = LC_ClientGuildManager::GetSingletonPtr();
	if (pkGuildManager)
	{
		for(int i=0; i<kMsg.m_Size; i++)
		{
			pkGuildManager->UpdatePosition(kMsg.m_PosChanges[i]);
		}
	}
}

void HL_FromGameServer::_handlerRltGuildApplyList(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_Guild_Apply kMsg;
	if(!kMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "解析错误 MG_RLT_Guild_Apply");
		return;
	}
	
	// 将申请人员加入到申请列表
	LC_ClientGuildManager* pkGuildManager = LC_ClientGuildManager::GetSingletonPtr();
	//if (pkGuildManager)
	{
		pkGuildManager->UpdateGuildApplyList(kMsg.m_kAllApplyInfo);
	}

	//设置是否自动允许加入
	pkGuildManager->GetGuildAsset()->GetGuildApplyInfoPtr()->SetAutoAdd(kMsg.m_kAllApplyInfo.AutoAdd);

}

void HL_FromGameServer::_handlerRltComirmApplyGuilid(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_ConfirmApplyGuild kMsg;
	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "解析错误 MG_RLT_ConfirmApplyGuild");
		return;
	}

	SendLogicInfoMessage(kMsg.m_Result);

	//判断是否是全部批准
	LC_ClientGuildManager* pkGuildManager = LC_ClientGuildManager::GetSingletonPtr();
	if (pkGuildManager)
	{
		int index = pkGuildManager->GetFirstValidApplyPlayerIndex();
		if(kMsg.m_Result == RE_SUCCESS)
		{
			if(index >= 0)
			{
				pkGuildManager->RequestComfirmApplyAllGuild(index,true);
			}
		}
	}
	
}

void HL_FromGameServer::_handlerRltExitGuild(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_ExitGuild kMsg;
	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "解析错误 MG_RLT_ModifyGuildDeclaration");
		return;
	}

	if (kMsg.m_uiResult == RE_EXIT_GUILD_SUCCESS)
	{
		// 处理离开门派
		LC_ClientGuildManager* pkGuildManager = LC_ClientGuildManager::GetSingletonPtr();
		if (pkGuildManager)
		{
			pkGuildManager->GetGuildAsset()->ReUse();
		}

		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
		if( pkGamePlayer && pkGamePlayer->GetCommanderUID() == kMsg.m_uiCharID )
		{
			pkGamePlayer->SetGuildID(INVALID_GUILD_ID);
			pkGamePlayer->SetGuildName("");
			LC_GuildRaidDataManager::GetSingleton().ReUse();

			LC_GameEvent kGameEvent;
			kGameEvent.SetEventType(GLET_CLIENT_GUILD_EXIT);

			UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
			pkUIManager->HandlerGameEvent(kGameEvent);
		}
	}

	SendLogicInfoMessage(kMsg.m_uiResult);

}

void HL_FromGameServer::_handlerRltModifyGuildDeclaration(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_ModifyGuildDeclaration kMsg;
	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "解析错误 MG_RLT_ModifyGuildDeclaration");
		return;
	}

	/*if (kMsg.m_uiResult == RE_SUCCESS)
	{
		LC_ClientGuildManager* pkGuildManager = LC_ClientGuildManager::GetSingletonPtr();
		if (pkGuildManager)
		{
			pkGuildManager->UpdateDeclaration(kMsg.m_szDeclaration);
		}
	}*/
	
	if(kMsg.m_uiResult == RE_SUCCESS)
	{
		LC_ClientGamePlayer *pkGamePlayer = pkSession->GetPlayer();
		if(NULL != pkGamePlayer && pkGamePlayer->GetCommanderUID() == kMsg.m_uiCharID)
			SendLogicInfoMessage(RE_MODIFY_GUILD_DECLARATION_SUCCESS);
	}
	else
	{
		SendLogicInfoMessage(kMsg.m_uiResult);
	}
}

void HL_FromGameServer::_handlerRltKickMemberGuild(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_KickMemberFromGuild kMsg;

	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "解析错误 MG_RLT_KickMemberFromGuild");
		return;
	}

	if (kMsg.m_uiResult != RE_SUCCESS)
	{
		if (kMsg.m_uiResult == RE_GUILD_KICKMEMBER_SUCCESS_BE_KICKED)
		{
			LC_ClientGuildManager* pkGuildManager = LC_ClientGuildManager::GetSingletonPtr();
			if (pkGuildManager)
			{
				pkGuildManager->GetGuildAsset()->ReUse();	
			}
			LC_ClientGamePlayer* pkGamePlayer=pkSession->GetPlayer();
			if (pkGamePlayer)
			{
				pkGamePlayer->SetGuildID(INVALID_GUILD_ID);
			}
			LC_GameEvent kGameEvent;
			kGameEvent.SetEventType(GLET_CLIENT_GUILD_EXIT);

			UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
			pkUIManager->HandlerGameEvent(kGameEvent);
		}

		SendLogicInfoMessage(kMsg.m_uiResult);
	}
}

void HL_FromGameServer::_handlerRltDismissGuild(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_DismissGuild kMsg;
	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "解析错误 MG_RLT_DismissGuild");
		return;
	}

	if (kMsg.m_uiResult == RE_GUILD_DISMISS_SUCCESS)
	{
		LC_ClientGuildManager* pkGuildManager = LC_ClientGuildManager::GetSingletonPtr();
		if (pkGuildManager)
		{
			pkGuildManager->GetGuildAsset()->ReUse();
		}

		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
		if( pkGamePlayer )
		{
			pkGamePlayer->SetGuildID(INVALID_GUILD_ID);
			pkGamePlayer->SetGuildName("");
		}
		LC_GuildRaidDataManager::GetSingleton().ReUse();

		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_CLIENT_GUILD_EXIT);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}

	SendLogicInfoMessage(kMsg.m_uiResult);
}

void HL_FromGameServer::_handlerRltInviteList(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{

}

void HL_FromGameServer::_handlerRltInviteToGuild(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_InviteToGuild kMsg;

	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "解析错误 MG_RLT_InviteToGuild");
		return;
	}

	if (kMsg.m_uiResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(kMsg.m_uiResult, STDSTR2TPSTR(kMsg.m_szBeInvitePlayerName));
	}
}

void HL_FromGameServer::_handlerReqConfirmInviteToGuild(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_REQ_ConfirmInviteToGuild kMsg;

	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "解析错误 MG_REQ_ConfirmInviteToGuild");
		return;
	}

	// 弹出对话框，让玩家确认	   
	
	//SendMessageDialog(RE_DIALOG_CONFIRM_INVITE_TO_GUILD, 
	//	kMsg.m_szInvitePlayerName, kMsg.m_szGuildName, 
	//	RE_DIALOG_CONFIRM_INVITE_TO_GUILD, 
	//	kMsg.m_uiGuildID, kMsg.m_uiInvitePlayerID, 0 );
		

	LC_ClientGameEvent kGameEvent;
	StringStreamType kContentStream;
	kContentStream << kMsg.m_szInvitePlayerName << "#";
	kContentStream << kMsg.m_szGuildName << "#";
	kContentStream << kMsg.m_uiGuildID << "#";
	kContentStream << kMsg.m_uiInviteCharID << "#";
	kContentStream << kMsg.m_uiGuildLevel << "#";
	kContentStream << kMsg.m_uiGuildMemberNum;
	kGameEvent.SetClientEventType(GLET_GUILD_INVITE_EVENT);
	LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(kContentStream.str());
	kGameEvent.SetUserData(&kUserData);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerRltGuildSkill(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	/*MG_RLT_GuildSkillInfo kMsg;

	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "解析错误 MG_RLT_ModifyGuildDeclaration");
		return;
	}

	if (kMsg.m_uiResult == RE_SUCCESS)
	{
		LC_ClientGuildManager* pkGuildManager = LC_ClientGuildManager::GetSingletonPtr();
		if (pkGuildManager)
		{
			pkGuildManager->UpdateGuildSkillInfo(kMsg.m_kSkillInfo);
		}
	}

	SendLogicInfoMessage(kMsg.m_uiResult);*/
}

void HL_FromGameServer::_handlerRltDonateGuildMaterials(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_DonateGuildMaterials kMsg;

	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "解析错误 MG_RLT_ModifyGuildDeclaration");
		return;
	}

	if (kMsg.m_uiResult != RE_SUCCESS)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "_handlerRltDonateGuildMaterials，CharID : %llu,Result : %u",kMsg.m_uiCharID,kMsg.m_uiResult);
		return;
	}

	SendLogicInfoMessage(kMsg.m_uiResult);
}

void HL_FromGameServer::_handlerRltSearchGuild( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	//MG_RLT_GuildSearch kMsg;
	MG_SS_RLT_GuildList kMsg;
	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "解析错误 MG_SS_RLT_GuildList");
		return;
	}
	LC_ClientGuildManager* pkGuildManager = LC_ClientGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
		return;

	pkGuildManager->UpdateSearchGuildInfo(kMsg.m_kResultInfo,kMsg.m_uiIndex);

	LC_GameEvent kGameEvent;

	kGameEvent.SetEventType(GLET_CLIENT_GUILD_SEARCH_INFO_UPDATE);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1,kMsg.m_allCount);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);

	//pkGuildManager->SetREQSearchFlag(false);
	//SendLogicInfoMessage(kMsg.m_uiResult);
}


void HL_FromGameServer::_handlerRltForbidGuildApply(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_ForbidGuildApply kMsg;

	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "解析错误 MG_RLT_ForbidGuildApply");
		return;
	}

	LC_ClientGuildManager* pkGuildManager = LC_ClientGuildManager::GetSingletonPtr();
	if (kMsg.m_uiResult == RE_SUCCESS)
	{
		LC_ClientGuildManager* pkGuildManager = LC_ClientGuildManager::GetSingletonPtr();
		if (pkGuildManager)
		{
			pkGuildManager->GetGuildAsset()->GetGuildInfo().SetForbidApply(kMsg.m_bIsForbidApply);
			pkGuildManager->GetGuildAsset()->GetGuildInfo().SetDirtyFlag(true);
			pkGuildManager->GetGuildAsset()->GetGuildInfo().AddDirtyFlagRevisionID();
		}		
	}

	SendLogicInfoMessage(kMsg.m_uiResult);
}

void HL_FromGameServer::_handlerRltClearGuildApplyList(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_ClearGuildApplyList kMsg;

	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "解析错误 MG_RLT_ForbidGuildApply");
		return;
	}

	LC_ClientGuildManager* pkGuildManager = LC_ClientGuildManager::GetSingletonPtr();
	if (kMsg.m_uiResult == RE_SUCCESS)
	{
		LC_ClientGuildManager* pkGuildManager = LC_ClientGuildManager::GetSingletonPtr();
		if (pkGuildManager)
		{
			pkGuildManager->GetGuildAsset()->GetGuildApplyInfo().Reset();
		}		
	}

	SendLogicInfoMessage(kMsg.m_uiResult);
}

void HL_FromGameServer::_handlerNotifyAddGuildApply(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_Notify_AddApply kMsg;
	if(!kMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "解析错误 MG_Notify_AddApply");
		return;
	}

	// 将申请人员加入到申请列表
	LC_ClientGuildManager* pkGuildManager = LC_ClientGuildManager::GetSingletonPtr();
	if (pkGuildManager)
	{
		pkGuildManager->AddGuildApply(kMsg.m_Apply);
	}
}

void HL_FromGameServer::_handlerNotifyDelGuildApply(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_Notify_DelApply kMsg;
	if(!kMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "解析错误 MG_Notify_DelApply");
		return;
	}
	
	LC_ClientGuildManager* pkGuildManager = LC_ClientGuildManager::GetSingletonPtr();
	if (pkGuildManager)
	{
		pkGuildManager->DelGuildApply(kMsg.m_TargetId);
	}
}

void HL_FromGameServer::_handlerRltGuildAppoint(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_GuildAppoint kMsg;

	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "解析错误 MG_RLT_GuildAppoint");
		return;
	}

	//LC_ClientGuildManager* pkGuildManager = LC_ClientGuildManager::GetSingletonPtr();
	//if (kMsg.m_uiResult == RE_SUCCESS)
	//{
	//	LC_ClientGuildManager* pkGuildManager = LC_ClientGuildManager::GetSingletonPtr();
	//	if (pkGuildManager)
	//	{
	//		pkGuildManager->GetGuildAsset()->GetGuildApplyInfo().Reset();
	//	}		
	//}
	if (kMsg.m_uiResult == RE_ACTION_FORBID)
	{
		SendLogicInfoMessage(RE_GUILD_APPOINT_FORBID);
	}
	else
	{
		SendLogicInfoMessage(kMsg.m_uiResult);
	}

}

void HL_FromGameServer::_handlerRltUpdateGuildRank(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SS_RLT_GuildRank kMsg;
	if(!kMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "解析错误 MG_SS_RLT_GuildRank");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if (!pkGamePlayer)
	{
		return;
	}

	LC_ClientGuildManager* pkGuildManager = LC_ClientGuildManager::GetSingletonPtr();
	if(kMsg.m_iResult == RE_SUCCESS && pkGuildManager)
	{
		pkGuildManager->UpdateGuildRankInfo(kMsg.m_GuildRankID);
	}
	else
	{
		SendLogicInfoMessage(kMsg.m_iResult);
	}
}


void HL_FromGameServer::_handlerRltLearnGuildSkill(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	//MG_RLT_LearnGuildSkill kMsg;
	//if ( !kMsg.Decode(pMsg, wSize) )
	//{
	//	GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "解析错误 MG_RLT_LearnGuildSkill");
	//	return;
	//}

	//if (kMsg.m_uiResult == RE_SUCCESS)
	//{
	//	SendLogicInfoMessage(RE_GUILD_LEARN_SKILL_SUCCESS);
	//}
	//else
	//{
	//	SendLogicInfoMessage(kMsg.m_uiResult);
	//}
}

void HL_FromGameServer::_handlerRltOnlinePlayerState( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RLT_OnlinePlayerState kRltMsg;
	if ( !kRltMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "解析错误 MG_RLT_OnlinePlayerState");
		return;
	}

	LC_ClientGuildManager* pkClientGuildManager = LC_ClientGuildManager::GetSingletonPtr();
	if (!pkClientGuildManager)
	{
		return;
	}
	for (int i = 0;i != kRltMsg.m_uiCrtSize;++i)
	{
		if (kRltMsg.m_uiOnlineCharID[i])
		{
			if (pkClientGuildManager->GetGuildAsset())
			{
				LC_GuildMemberEntry* pkGuildMemberEntry = pkClientGuildManager->GetGuildAsset()->GetGuildMemberInfo().FindMemeberEntry(kRltMsg.m_uiOnlineCharID[i]);
				if (pkGuildMemberEntry)
				{
					pkGuildMemberEntry->SetOnline(kRltMsg.m_uiOnlineCharServerID[i] > 0);
					pkGuildMemberEntry->SetServerID(kRltMsg.m_uiOnlineCharServerID[i]);
				}
			}
		}
	}
	if (pkClientGuildManager->GetGuildAsset())
	{
		pkClientGuildManager->GetGuildAsset()->GetGuildMemberInfo().SetDirtyFlag(true);
		pkClientGuildManager->GetGuildAsset()->GetGuildMemberInfo().AddDirtyFlagRevisionID();
	}
}

void HL_FromGameServer::_handlerRltUpgradeGuild(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_GuildUpgrade kRltMsg;
	if ( !kRltMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "解析错误 MG_RLT_GuildUpgrade");
		return;
	}
	if (kRltMsg.m_Result == RE_SUCCESS)
	{
		LC_GuildAsset* pkGuildAsset = LC_ClientGuildManager::GetSingletonPtr()->GetGuildAsset();
		if(NULL != pkGuildAsset)
		{
			LC_GuildInfo *pkGuildInfo = pkGuildAsset->GetGuildInfoPtr();
			if(NULL != pkGuildInfo)
			{
				pkGuildInfo->AddDirtyFlagRevisionID();
			}
		}
	}
	SendLogicInfoMessage(kRltMsg.m_Result);
}


void HL_FromGameServer::_handleSyncGuildWarInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_GuildWarAssert syncMsg;
	if(!syncMsg.Decode(pMsg,wSize))
		return;


	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return;
	}

	pkGamePlayer->GetGuildWarAssert()->GetDataFromProtocolStruct(syncMsg);
	pkGamePlayer->GetGuildWarAssert()->SetDirtyFlag(true);
	pkGamePlayer->GetGuildWarAssert()->UpdateDirtyFlag();
}

void HL_FromGameServer::_handlerRltPowerGuildAppoint(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_Appoint_Power_RLT rltMsg;
	if (!rltMsg.Decode(pMsg, wSize))
		return;

	SendLogicInfoMessage(rltMsg.m_Result);
}


void HL_FromGameServer::_handleRltGlobalGuildReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_GlobalGuildWarReward_Resp rltMsg;
	if (!rltMsg.Decode(pMsg, wSize))
	{
		return;
	}
	if (rltMsg.m_Result != RE_SUCCESS)
		SendLogicInfoMessage(rltMsg.m_Result);
	else
	{
		CF_ItemList::DataEntry* pkData = NULL;

		StringStreamType sStream("");
		sStream << rltMsg.m_exp << "#";
		sStream << rltMsg.m_sp << "#";
		sStream << rltMsg.m_money << "#";
		sStream << (uint32_t)rltMsg.m_itemlen << "#";
		for (uint32_t i = 0; i < rltMsg.m_itemlen; i++)
		{
			sStream << rltMsg.m_item[i] << "#";
			sStream << rltMsg.m_itemCount[i] << "#";
		}

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerMsgGameEvent(MGPT_RESP_POWERGUILD_REWARD, sStream.str());
	}
}

void HL_FromGameServer::_handleRltDomainReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_DomainReward_Resp rltMsg;
	if (!rltMsg.Decode(pMsg, wSize))
		return ;
	if(rltMsg.m_Result != RE_SUCCESS)
	{
		SendLogicInfoMessage(rltMsg.m_Result);
		return;
	}

	CF_ItemList::DataEntry* pkData = NULL;

	StringStreamType sStream("");
	sStream << rltMsg.m_exp << "#";
	sStream << rltMsg.m_sp << "#";
	sStream << rltMsg.m_money << "#";
	sStream << (uint32_t)rltMsg.m_itemlen << "#";
	for (uint32_t i = 0; i < rltMsg.m_itemlen; i++)
	{
		sStream << rltMsg.m_item[i] << "#";
		sStream << rltMsg.m_itemCount[i] << "#";
	}

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerMsgGameEvent(MGPT_RESP_POWERGUILD_REWARD, sStream.str());
}


void HL_FromGameServer::_handleRltPowerGuildReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_GlobalGuildWarReward_Resp rltMsg;
	if (!rltMsg.Decode(pMsg, wSize))
		return ;
	if(rltMsg.m_Result != RE_SUCCESS)
	{
		SendLogicInfoMessage(rltMsg.m_Result);
		return;
	}

	CF_ItemList::DataEntry* pkData = NULL;

	StringStreamType sStream("");
	sStream << rltMsg.m_exp << "#";
	sStream << rltMsg.m_sp << "#";
	sStream << rltMsg.m_money << "#";
	sStream << (uint32_t)rltMsg.m_itemlen << "#";
	for (uint32_t i = 0; i < rltMsg.m_itemlen; i++)
	{
		sStream << rltMsg.m_item[i] << "#";
		sStream << rltMsg.m_itemCount[i] << "#";
	}

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerMsgGameEvent(MGPT_RESP_POWERGUILD_REWARD, sStream.str());
}

void HL_FromGameServer::_handleNotifyGuildWarCatapultMsg(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_GuildWar_Capapult_Msg_Notify rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return ;

	SC_ClientScriptInterface* pkClientInterface = SC_ClientScriptInterface::GetSingletonPtr();
	SC_ClientSkillInterface* pkInterface = pkClientInterface->GetSkillInterface();

	StringType serverName = pkInterface->GetGameServerName(rltMsg.m_ServerId);


	StringStreamType sStream("");
	sStream << serverName << "#";
	sStream << rltMsg.m_GuildName << "#";
	sStream << rltMsg.m_Name << "#";
	sStream << rltMsg.m_Damage;
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerMsgGameEvent(MGPT_NOTIFY_GUILDWAR_CAPAPULT_MSG, sStream.str());
	
	//SendNotifyMessage(PROMPT_CHANNEL_CHAT_SYSTEM, sStream.str());
}

void HL_FromGameServer::_handlerNotifyGuildRaidInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_Notify_GuildRaid rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return ;

	LC_ClientGamePlayer* pkPlayer = pkSession->GetPlayer();
	if(NULL == pkPlayer)
		return;

	if (rltMsg.m_GuildId != pkPlayer->GetGuildID())
		return;

	LC_GuildRaidDataManager::GetSingleton().GetDataFromProtocolStruct(rltMsg);
}

void HL_FromGameServer::_handlerRltGuildRaidReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_GuildRaidReward_Resp rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;
	if(rltMsg.m_Result != RE_SUCCESS)
	{
		SendLogicInfoMessage(rltMsg.m_Result);
	}
	else
	{
		StringStreamType sStream("");
		sStream << rltMsg.m_Page << "#" << rltMsg.m_Stage << "#";
		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerMsgGameEvent(MGPT_RESP_GUILDRAIDREWARD, sStream.str());
	}
}

void HL_FromGameServer::_handlerRltRecruitGuildMember(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RLT_RecruitGuildMember rltMsg;
	if (!rltMsg.Decode(pMsg, wSize))
		return ;

	if(rltMsg.m_uiResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(rltMsg.m_uiResult);
	}
}

void HL_FromGameServer::_handlerRltGetGuildReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RLT_GuildReward rltMsg;
	if (!rltMsg.Decode(pMsg, wSize))
		return ;

	if(rltMsg.m_uiResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(rltMsg.m_uiResult);
	}
}


void HL_FromGameServer::_handlerRespAddGuildGood(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_Resp_Add_GuildGood rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;
	if(rltMsg.m_Result != RE_SUCCESS)
	{
		SendLogicInfoMessage(rltMsg.m_Result);
	}
	else
	{
		CF_GuildGood::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_GuildGood>(rltMsg.m_GoodType);
		if (pkData)
		{
			SendMessageDialog(RE_GUILF_WORSHIP_SUCCESS, pkData->_GuildMoneyReward, 0, RE_GUILF_WORSHIP_SUCCESS, 0, 0, 0);
		}
	}
}

//帮会仓库操作
void HL_FromGameServer::_handlerRespGuildItemOpt(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_Resp_GuildItemPack_Opt rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;
	if(rltMsg.m_Result != RE_SUCCESS)
	{
		SendLogicInfoMessage(rltMsg.m_Result);
	}

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_GUILD_ITEM_OPT);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1,rltMsg.m_Result);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2,rltMsg.m_OptType);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

//帮会召集令
void HL_FromGameServer::_handleGuildTransportAdd( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_Notify_GuildTransport notifyMsg;
	if (!notifyMsg.Decode(pMsg,wSize))
		return;

	LC_ClientLogicManager* pkLogicManager = (LC_ClientLogicManager*)LC_ClientLogicManager::GetSingletonPtr();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	pkGamePlayer->SetGuildTransportStamp(notifyMsg.m_Timestamp);
	pkGamePlayer->SetGuildTransportResId(notifyMsg.m_TargetResid);
	if(notifyMsg.m_Transed == false)
	{
		LC_ClientGameEvent kGameEvent;
		kGameEvent.SetClientEventType(GameLogic::CGET_GUILD_TRANSPORT);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		StringStreamType sStream;
		sStream<<notifyMsg.m_CharName<<"#"<<notifyMsg.m_TransportId;
		LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(sStream.str());
		kGameEvent.SetUserData(&kUserData);
		pkUIManager->HandlerGameEvent(kGameEvent);
		//SendMessageDialog(RE_GUILD_TRANSPORT, notifyMsg.m_PlayerName.c_str(), "", RE_GUILD_TRANSPORT, notifyMsg.m_TransportId, 0, 0);
		SendLogicInfoMessage(RE_GUILD_TRANSPORT_MSG,notifyMsg.m_CharName.c_str());

		CF_SystemError* pkCSVSystemError = CF_SystemError::GetSingletonPtr();
		if(!pkCSVSystemError)
			return;

		CF_SystemError::DataEntry* pkData = pkCSVSystemError->GetEntryPtr(RE_GUILD_TRANSPORT_CHANNEL);
		if(!pkData)
			return;
		char strbuffer[512];
		sprintf(strbuffer,pkData->_sErrorMsg.c_str(),pkGamePlayer->GetCharName().c_str());
		//SendLogicInfoMessage(RE_GUILD_TRANSPORT_CHANNEL,pkGamePlayer->GetCharName(),"","","","",GUILD_NOTIFICATION);
		SendChatMsg(RE_GUILD_TRANSPORT_CHANNEL,strbuffer,"","",GUILD_NOTIFICATION,0);
	}
	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_GUILD_TRANSPORT_CD);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handleRespGuildTransportCreat( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_Resp_GuildTransport_Create respMsg;
	if (!respMsg.Decode(pMsg,wSize))
		return;
	LC_ClientLogicManager* pkLogicManager = (LC_ClientLogicManager*)LC_ClientLogicManager::GetSingletonPtr();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if(respMsg.m_Result != RE_SUCCESS)
	{
		if (respMsg.m_Result == RE_TRANSPORT_HAS_EXIST)
		{
			int64_t timeInterval = pkGamePlayer->GetGuildTransportStamp() - pkGamePlayer->GetServerTimeVal();
			timeInterval = timeInterval < 0 ? 0 : timeInterval;
			SendLogicInfoMessage(respMsg.m_Result,timeInterval);
		}
		else
		{
			SendLogicInfoMessage(respMsg.m_Result);
		}
	}
	else
	{
		CF_SystemError* pkCSVSystemError = CF_SystemError::GetSingletonPtr();
		if(!pkCSVSystemError)
			return;

		CF_SystemError::DataEntry* pkData = pkCSVSystemError->GetEntryPtr(RE_GUILD_TRANSPORT_CHANNEL);
		if(!pkData)
			return;
		char strbuffer[512];
		sprintf(strbuffer,pkData->_sErrorMsg.c_str(),pkGamePlayer->GetCharName().c_str());
		//SendLogicInfoMessage(RE_GUILD_TRANSPORT_CHANNEL,pkGamePlayer->GetCharName(),"","","","",GUILD_NOTIFICATION);
		SendChatMsg(RE_GUILD_TRANSPORT_CHANNEL,strbuffer,"","",GUILD_NOTIFICATION,0);

		SendLogicInfoMessage(RE_GUILD_TRANSPORT_MSG,pkGamePlayer->GetCharName().c_str());
	}

}

void HL_FromGameServer::_handleRespGuildTransportEnter( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_Resp_Enter_GuildTransport respMsg;
	if (!respMsg.Decode(pMsg,wSize))
		return;
	LC_ClientLogicManager* pkLogicManager = (LC_ClientLogicManager*)LC_ClientLogicManager::GetSingletonPtr();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if(respMsg.m_Result != RE_SUCCESS)
	{
		CF_WorldMapList* pkWorldMapList = CF_WorldMapList::GetSingletonPtr();
		CF_WorldMapList::DataEntry* pkData = pkWorldMapList->GetEntryPtr(pkGamePlayer->GetGuildTransportResId());

		if(!pkData)
			return;
		switch(respMsg.m_Result)
		{
		case RE_TRANS_LEVEL_LOW:
			SendLogicInfoMessage(RE_TRANS_LEVEL_LOW,pkData->_iNeedLevel);
			break;
		case RE_TRANSPORT_REACH_MAX: 
			SendMessageDialog(respMsg.m_Result, 0,0,0, respMsg.m_Result, 0,0,0,0,0);
			break;
		}
	}
}

void HL_FromGameServer::_handleGuildTransportDel( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_Notify_GuildTransport_Del respMsg;
	if (!respMsg.Decode(pMsg,wSize))
		return;
	LC_ClientLogicManager* pkLogicManager = (LC_ClientLogicManager*)LC_ClientLogicManager::GetSingletonPtr();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	int64_t iTimeStamp = pkGamePlayer->GetGuildTransportStamp();
	if (iTimeStamp > 0)
	{
		pkGamePlayer->SetGuildTransportStamp(0);
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_GUILD_TRANSPORT_DEL);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
}

void HL_FromGameServer::_handleGuildRewardState( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{

	MG_SyncGuildRewardState respMsg;
	if(!respMsg.Decode(pMsg, wSize))
		return;

	LC_ClientLogicManager* pkLogicManager = (LC_ClientLogicManager*)LC_ClientLogicManager::GetSingletonPtr();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if(NULL == pkGamePlayer)
		return;

	pkGamePlayer->SetGuildRewardIsValid(respMsg.m_bGuildRewardIsVaild);
	LC_GuildAsset* pkGuildAsset = LC_ClientGuildManager::GetSingletonPtr()->GetGuildAsset();
	if(NULL != pkGuildAsset)
	{
		LC_GuildInfo *pkGuildInfo = pkGuildAsset->GetGuildInfoPtr();
		if(NULL != pkGuildInfo)
		{
			pkGuildInfo->AddDirtyFlagRevisionID();
		}
	}
}