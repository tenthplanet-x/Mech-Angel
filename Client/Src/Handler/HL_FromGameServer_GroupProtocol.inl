//----------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltCreateGroup(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	////�����δ������Ϸ�����˳�
	//if (eGameStage != ST_GAME_STAGE_GAME)
	//	return;

	LC_ClientGamePlayer* pkPlayer = pkSession->GetPlayer();
	if(!pkPlayer)
		return;

	MG_RLT_CreateGroup rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"�޷�����MG_RLT_CreateGroup");
		return;	
	}

	//��������ɹ�
	if(rltMsg.m_iCreateResult == RE_GROUP_CREATE_SUCCESS)
	{
		long lGroupID = rltMsg.m_lGroupID;
		LC_PlayerGroupManagerBase* pkPlayerGroupManager = LC_PlayerGroupManagerBase::GetSingletonPtr();	
		LC_PlayerGroupBase* pkPlayerGroup = pkPlayerGroupManager->CreatePlayerGroup(lGroupID);
		if(!pkPlayerGroup)
		{		
			GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"MG_RLT_CreateGroup�޷�����,ϵͳ����");
			return;	
		}

		pkPlayerGroup->AddPlayerToMemberList(pkPlayer);
		g_soundMgr.Play2DEx(VARIABLE2STRING(SE_Sys_Team_Join));
		
		pkPlayer->ClearInvitePlayerGroup(); // ��ձ������б�
	}

	SendLogicInfoMessage(rltMsg.m_iCreateResult);
}
//----------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltExitGroup(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();


	MG_RLT_ExitGroup rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"�޷�����MG_RLT_ExitGroup");
		return;
	}

	unique_id_type lExitCharID = rltMsg.m_lExitCharID;
	uint32_t lGroupID = rltMsg.m_lGroupID;	
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
		return;

	LC_PlayerGroupBase* pkPlayerGroup = pkGamePlayer->GetPlayerGroup();
	if(NULL == pkPlayerGroup)
		return;
	
	if(rltMsg.m_iExitResult == RE_GROUP_EXIT_SUCCESS)
	{
		StringType kExitCharName = CTSTR_UNKNOWN_PLAYER;
		if(lExitCharID == pkGamePlayer->GetCommanderUID())
		{
			//if (pkPlayerGroup)
			{
				LC_ClientPlayerGroupManager* pkGroupManager = CLIENT_GET_PLAYER_GROUP_MANAGER();
				pkGroupManager->DestroyPlayerGroup(pkPlayerGroup);

				pkGamePlayer->SetPlayerGroupIDType(INVALID_GROUP_ID, E_PlayerTeamType_Local);
				pkGamePlayer->SetPlayerGroupCheifFlag(false);

				kExitCharName = CTSTR_YOU;
			}
		}
		else
		{
			kExitCharName = STDSTR2TPSTR(rltMsg.m_szExitPlayerName);
		}
		//����˭�뿪����������Ч
		g_soundMgr.Play2DEx(VARIABLE2STRING(SE_Sys_Team_Quit));
		SendLogicInfoMessage(rltMsg.m_iExitResult,kExitCharName.c_str());
	}
	else
	{
		SendLogicInfoMessage(rltMsg.m_iExitResult);
	}	
}
//-------------------------------------------------------
void HL_FromGameServer::_handlerRltKickPlayerFromGroup(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	////�����δ������Ϸ�����˳�
	//if (eGameStage != ST_GAME_STAGE_GAME)
	//	return;
	
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();	
	if(!pkGamePlayer)
		return;

	MG_RLT_KickPlayerFromGroup rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"�޷�����MG_RLT_KickPlayerFromGroup");
		return;
	}

	uint32_t lGroupID		= rltMsg.m_lGroupID;
	unique_id_type lKickCharID	= rltMsg.m_lKickCharID;


	LC_ClientPlayerGroup* pkPlayerGroup = (LC_ClientPlayerGroup*)(pkGamePlayer->GetPlayerGroup());
	if(!pkPlayerGroup || pkPlayerGroup->GetSelfID() != lGroupID)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_RLT_KickPlayerFromGroup�޷�����,��Ҷ��鲻ƥ��");
		return;
	}	
	
	if(rltMsg.m_iKickResult == RE_GROUP_KICK_SUCCESS)
	{
		if ( pkGamePlayer->GetPlayerGroupCheifFlag() )
		{
			//�ӳ�T�˲�����Ч
			g_soundMgr.Play2DEx(VARIABLE2STRING(SE_Sys_Team_Quit));
			SendLogicInfoMessage(RE_GROUP_KICK_SUCCESS_CHEIF_HINT, rltMsg.m_szKickPlayerName.c_str());
		}
		else
		{
			StringType kKickCharName = CTSTR_UNKNOWN_PLAYER;
			if(lKickCharID == pkGamePlayer->GetCommanderUID())
			{
				LC_ClientPlayerGroupManager* pkGroupManager = CLIENT_GET_PLAYER_GROUP_MANAGER();
				pkGroupManager->DestroyPlayerGroup(pkPlayerGroup);

				pkGamePlayer->SetPlayerGroupIDType(INVALID_GROUP_ID, E_PlayerTeamType_Local);
				pkGamePlayer->SetPlayerGroupCheifFlag(false);

				kKickCharName = CTSTR_YOU;
			}
			else
			{
				kKickCharName = STDSTR2TPSTR(rltMsg.m_szKickPlayerName);
			}
			//����˭��T�ˣ���������Ч
			g_soundMgr.Play2DEx(VARIABLE2STRING(SE_Sys_Team_Quit));
			SendLogicInfoMessage(rltMsg.m_iKickResult,kKickCharName.c_str());
		}		
	}
	else
	{
		SendLogicInfoMessage(rltMsg.m_iKickResult);
	}
	
}
//----------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltDismissGroup(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{	
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	////�����δ������Ϸ�����˳�
	//if (eGameStage != ST_GAME_STAGE_GAME)
	//	return;
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();	
	if(!pkGamePlayer)
	{
		return;
	}


	MG_RLT_DismissGroup rltMsg;	
	if(!rltMsg.Decode(pMsg,wSize))	
	{		
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"�޷�����MG_RLT_DismissGroup");		
		return;	
	}		

	long lGroupID = rltMsg.m_lGroupID;	
	LC_PlayerGroupBase* pkPlayerGroup = pkGamePlayer->GetPlayerGroup();	
	if(!pkPlayerGroup || pkPlayerGroup->GetSelfID() != lGroupID)	
	{		
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"�޷�����MG_RLT_DismissGroup��ϵͳ����");		
		return;	
	}	
	
	if(rltMsg.m_iDisimissResult == RE_GROUP_DISMISS_SUCCESS)
	{
		
		pkPlayerGroup->Dismiss();	
		LC_ClientPlayerGroupManager* pkPlayerGroupManager = CLIENT_GET_PLAYER_GROUP_MANAGER();
		pkPlayerGroupManager->DestroyPlayerGroup(pkPlayerGroup);

		//if (pkGamePlayer)
		{
			pkGamePlayer->SetPlayerGroupCheifFlag(false);
			pkGamePlayer->SetPlayerGroupIDType(INVALID_GROUP_ID, E_PlayerTeamType_Local);
		}
		//��ɢ���鲥����Ч
		g_soundMgr.Play2DEx(VARIABLE2STRING(SE_Sys_Team_Quit));
	}

	SendLogicInfoMessage(rltMsg.m_iDisimissResult);
}
//----------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltJoinGroup(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	////�����δ������Ϸ�����˳�
	//if (eGameStage != ST_GAME_STAGE_GAME)
	//	return;
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();	
	if(!pkGamePlayer)
	{
		return;
	}

	MG_RLT_JoinGroup rltMsg;	
	if(!rltMsg.Decode(pMsg,wSize))	
	{		
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"�޷�����MG_RLT_JoinGroup");		
		return;	
	}		

	uint32_t		lGroupID		= rltMsg.m_lGroupID;
	unique_id_type		lJoinCharID	= rltMsg.m_lJoinCharID;
	
	StringType kJoinCharName;
	
	if(lJoinCharID == pkGamePlayer->GetCommanderUID())
	{
		kJoinCharName = GET_MSG(CLIENTTEXTDEFINE_H_007);
		if(rltMsg.m_iJoinResult == RE_GROUP_JOIN_SUCCESS)
		{
			LC_ClientPlayerGroupManager* pkPlayerGroupManager = CLIENT_GET_PLAYER_GROUP_MANAGER();	
			LC_PlayerGroupBase* pkPlayerGroup = pkPlayerGroupManager->CreatePlayerGroup(lGroupID);	
			if(!pkPlayerGroup)	
			{		
				GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"MG_RLT_JoinGroup�޷�����,ϵͳ����");		
				return;	
			}		
			pkPlayerGroup->AddPlayerToMemberList(pkGamePlayer);

			pkGamePlayer->ClearInvitePlayerGroup(); // ��ձ������б�

			LC_GameEvent kGameEvent;
			kGameEvent.SetEventType(GLET_JOIN_GROUP);

			UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
			pkUIManager->HandlerGameEvent(kGameEvent);
		}
		else
		{
			//����ͬ����������������
			g_soundMgr.Play2DEx(VARIABLE2STRING(SE_Sys_Team_Join));
			SendLogicInfoMessage(rltMsg.m_iJoinResult, STDSTR2TPSTR(rltMsg.m_strCheifName));
			return;
		}
	}
	else
	{
		kJoinCharName = STDSTR2TPSTR(rltMsg.m_strJoinPlayerName);
	}
	
	if (kJoinCharName.size() > 0)
	{
		//����Ǳ��������������飬�Լ�ͬ���Լ���ӵĻ����Ͳ����Ž��������Ч����Ϊ֮ǰ����ͬ������ӵ�ʱ�����Ѿ�����һ�������Ч��
		//ֻ��Ӳ������!!!
		if (kJoinCharName != CTSTR_YOU)
		{	
			//�Լ�ͬ���������Ӳ��Ų�����Ч
			g_soundMgr.Play2DEx(VARIABLE2STRING(SE_Sys_Team_Join));
		}
		SendLogicInfoMessage(rltMsg.m_iJoinResult,kJoinCharName.c_str(), STDSTR2TPSTR(rltMsg.m_strCheifName));
	}
	else
	{
		SendLogicInfoMessage(rltMsg.m_iJoinResult, STDSTR2TPSTR(rltMsg.m_strCheifName));
	}
}
//----------------------------------------------------------------------------
void HL_FromGameServer::_handlerReqConfirmGroupJoin(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{	
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	////�����δ������Ϸ�����˳�
	//if (eGameStage != ST_GAME_STAGE_GAME)
	//	return;
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();	
	if(!pkGamePlayer)
		return;

	MG_REQ_ConfirmGroupJoin reqMsg;	
	if(!reqMsg.Decode(pMsg,wSize))	
	{		
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"�޷�����MG_REQ_ConfirmGroupJoin");		
		return;	
	}		

	long lGroupID			= reqMsg.m_lGroupID;
	unique_id_impl lJoinCharID		= reqMsg.m_lJoinTargetCharID;

	LC_PlayerGroupBase* pkPlayerGroup = pkGamePlayer->GetPlayerGroup();
	if(!pkPlayerGroup)	
	{		
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"�޷�����MG_REQ_ConfirmGroupJoin,������ж���");		
		return;	
	}	

	if(pkPlayerGroup->GetGroupCheifID() != pkGamePlayer->GetMainControllerID())
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"�޷�����MG_REQ_ConfirmGroupJoin,��Ҳ��Ƕӳ�");		
		return;	
	}

	LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
	LC_PlayerBase* pkNetPlayer = pkNetPlayerManager->FindPlayer(lJoinCharID);
	if (pkNetPlayer)
	{
		pkPlayerGroup->AddPlayerToCandidateList(pkNetPlayer);
		pkGamePlayer->SetTeamHintFlag(true);
		//GfxWriteLog(LOG_TAG_GAME_TIP,LOG_SWITCH_GAME,GFxSystemInfo(RE_PLAYER_GROUP_RECEIVE_JOIN).c_str(),pkNetPlayer->GetCharName().c_str());
	}	

	//�����¼���lua
	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_CONFIRM_GROUP_JOIN);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, lJoinCharID.detail());
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, lJoinCharID.catagory());

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}
//----------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltInviteToGroup(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	////�����δ������Ϸ�����˳�
	//if (eGameStage != ST_GAME_STAGE_GAME)
	//	return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();	
	if(!pkGamePlayer)
		return;


	MG_RLT_InviteToGroup rltMsg;	
	if(!rltMsg.Decode(pMsg,wSize))	
	{		
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"�޷�����MG_RLT_InviteToGroup");		
		return;	
	}		

	unique_id_type lInviteCharID		= rltMsg.m_lInviteCharID;
	uint32_t lGroupID				= rltMsg.m_lGroupID;
	unique_id_type lBeInviteCharID		= rltMsg.m_lBeInviteCharID;
	int	 iInviteResult			= rltMsg.m_iInviteResult;

	SendLogicInfoMessage(rltMsg.m_iInviteResult, STDSTR2TPSTR(rltMsg.m_strBeInvitePlayerName));

	/*LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();	
	switch (iInviteResult)
	{
	case RE_PLAYER_GROUP_ACCEPT_INVITE:
		{
			if(lBeInvitePlayerID != pkGamePlayer->GetID())
			{
				GfxWriteLog(LOG_TAG_GAME_TIP,LOG_SWITCH_GAME,GFxSystemInfo(RE_PLAYER_GROUP_ACCEPT_INVITE).c_str(),rltMsg.m_kBeInvitePlayerName.c_str());
				return;
			}
			else
			{
				LC_ClientPlayerGroupManager* pkPlayerGroupManager = CLIENT_GET_PLAYER_GROUP_MANAGER();	
				LC_PlayerGroupBase* pkPlayerGroup = pkPlayerGroupManager->CreatePlayerGroup(lGroupID);	
				if(!pkPlayerGroup)	
				{		
					GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"MG_RLT_InviteToGroup�޷�����,ϵͳ����");		
					return;	
				}		
				pkPlayerGroup->AddPlayerToMemberList(pkGamePlayer);
				GfxWriteLog(LOG_TAG_GAME_TIP,LOG_SWITCH_GAME,GFxSystemInfo(RE_PLAYER_GROUP_ACCEPT_INVITE).c_str(),"��");
			}
		}
		break;
	case RE_PLAYER_GROUP_DENY_INVITE:
		{
			if(lInvitePlayerID != pkGamePlayer->GetID())
			{
				GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"�޷�����MG_RLT_InviteToGroup,��Ӧ���յ���Ϣ");
				return;
			}

			GfxWriteLog(LOG_TAG_GAME_TIP,LOG_SWITCH_GAME,GFxSystemInfo(RE_PLAYER_GROUP_DENY_INVITE).c_str(),rltMsg.m_kBeInvitePlayerName.c_str());
		}
		break;
	default:
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_RLT_InviteToGroup�޷�����,���ؽ������");
	}*/
}
//----------------------------------------------------------------------------
void HL_FromGameServer::_handlerReqConfirmInviteToGroup(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{	
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	////�����δ������Ϸ�����˳�
	//if (eGameStage != ST_GAME_STAGE_GAME)
	//	return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();	
	if(!pkGamePlayer)
		return;

	MG_REQ_ConfirmInviteToGroup reqMsg;	
	if(!reqMsg.Decode(pMsg,wSize))	
	{		
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"�޷�����MG_REQ_ConfirmInviteToGroup");		
		return;	
	}		

	unique_id_type lInviteCharID	= reqMsg.m_lInviteCharID;
	uint32_t lGroupID					= reqMsg.m_lGroupID;
	unique_id_type lBeInviteCharID			= reqMsg.m_lBeInviteCharID;

	LC_PlayerGroupBase* pkPlayerGroup = pkGamePlayer->GetPlayerGroup();	
	if(!pkPlayerGroup || pkPlayerGroup->GetSelfID() != lGroupID)
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"�޷�����MG_REQ_ConfirmInviteToGroup,������ݲ�ƥ��");
		return;	
	}

	object_id_type lGroupCheifID = pkPlayerGroup->GetGroupCheifID();
	if(lGroupCheifID != pkGamePlayer->GetMainControllerID())
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"�޷�����MG_REQ_ConfirmInviteToGroup,���Ƕӳ�");
		return;	
	}

	//����ϵͳ��Ϣ,����Ի���
	SendMessageDialog(RE_DIALOG_CONFIRM_INVITE_TO_GROUP, STDSTR2TPSTR(reqMsg.m_strInvitePlayerName), STDSTR2TPSTR(reqMsg.m_strBeInvitePlayerName),
		MDT_CONFIRM_INVITE_TO_GROUP,lInviteCharID,lGroupID,lBeInviteCharID);
}
//----------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltConfirmInviteToGroup(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{	
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	////�����δ������Ϸ�����˳�
	//if (eGameStage != ST_GAME_STAGE_GAME)
	//	return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();	
	if(!pkGamePlayer)
		return;

	MG_RLT_ConfirmInviteToGroup rltMsg;	
	if(!rltMsg.Decode(pMsg,wSize))	
	{		
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"�޷�����MG_RLT_ConfirmInviteToGroup");		
		return;	
	}		

	unique_id_type lInviteCharID			= rltMsg.m_lInviteCharID;
	uint32_t lGroupID					= rltMsg.m_lGroupID;
	unique_id_type lBeInviteCharID			= rltMsg.m_lBeInviteCharID;
	StringType kInvitePlayerName    = STDSTR2TPSTR(rltMsg.m_strInvitePlayerName);
	StringType kBeInvitePlayerName  = STDSTR2TPSTR(rltMsg.m_strBeInvitePlayerName);

	if(lInviteCharID == pkGamePlayer->GetCommanderUID())
		kInvitePlayerName = CTSTR_YOU;

	if(lBeInviteCharID == pkGamePlayer->GetCommanderUID())
		kBeInvitePlayerName = CTSTR_YOU;

	//SendLogicInfoMessage(rltMsg.m_iConfirmResult,kChiefPlayerName.c_str(),kInvitePlayerName.c_str(),kBeInvitePlayerName.c_str());
	SendLogicInfoMessage(rltMsg.m_iConfirmResult, kBeInvitePlayerName.c_str());
}
//----------------------------------------------------------------------------
void HL_FromGameServer::_handlerReqConfirmBeInviteToGroup(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{	
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();


	MG_REQ_ConfirmBeInviteToGroup reqMsg;	
	if(!reqMsg.Decode(pMsg,wSize))	
	{		
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"�޷�����MG_REQ_ConfirmBeInviteToGroup");		
		return;	
	}		

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();	
	if(!pkGamePlayer)
		return;

	ResultType iResult			= reqMsg.m_iResultType;
	unique_id_impl lInviteCharID		= reqMsg.m_lInviteCharID;
	uint32_t lGroupID				= reqMsg.m_lGroupID;
	unique_id_type lBeInviteCharID		= reqMsg.m_lBeInviteCharID;

	if (lGroupID == INVALID_GROUP_ID)
	{
		lGroupID = PLAYER_GROUP_ID_MAX + unique_id_impl(lInviteCharID).detail();
	}

	if(iResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(iResult, STDSTR2TPSTR(reqMsg.m_strInvitePlayerName));
		return;
	}
	
	if(pkGamePlayer->GetPlayerGroup())
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"�޷�����MG_REQ_ConfirmBeInviteToGroup,���ж���");
		return;	
	}

	if(lBeInviteCharID != pkGamePlayer->GetCommanderUID())
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"�޷�����MG_REQ_ConfirmBeInviteToGroup,���Ǳ��������");
		return;	
	}

	pkGamePlayer->AddInvitePlayerGroup(lGroupID, lInviteCharID, reqMsg.m_GroupRaidVaild, reqMsg.m_Combin, reqMsg.m_GroupRaidResId);
	pkGamePlayer->SetTeamHintFlag(true);

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_CONFIRM_BEINVITE_TO_GROUP);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, lInviteCharID.detail());
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, lInviteCharID.catagory());
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_3, lGroupID);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);

	//GfxWriteLog(LOG_TAG_GAME_TIP,LOG_SWITCH_GAME,GFxSystemInfo(RE_PLAYER_GROUP_RECEIVE_INVITE).c_str(),reqMsg.m_kInvitePlayerName.c_str());
}
//----------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltAppointmentCheif(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{	
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	////�����δ������Ϸ�����˳�
	//if (eGameStage != ST_GAME_STAGE_GAME)
	//	return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();	
	if(!pkGamePlayer)
		return;

	MG_RLT_AppointmentCheif rltMsg;	
	if(!rltMsg.Decode(pMsg,wSize))	
	{		
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"�޷�����MG_RLT_AppointmentCheif");		
		return;	
	}		

	unique_id_type lRequestCharID			= rltMsg.m_lRequestCharID;
	uint32_t lGroupID					= rltMsg.m_lGroupID;
	unique_id_type lBeAppointmentCharID		= rltMsg.m_lBeAppointmentCharID;
	ResultType	iAppointmentResult  = rltMsg.m_iAppointmentResult;

	if(!pkGamePlayer->GetPlayerGroup() || pkGamePlayer->GetPlayerGroup()->GetID() != lGroupID)
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"�޷�����MG_RLT_AppointmentCheif,������Ϣ����");
		return;	
	}

	if(lBeAppointmentCharID == pkGamePlayer->GetCommanderUID())
	{
		SendLogicInfoMessage(iAppointmentResult,CTSTR_YOU);
	}
	else
	{
		LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
		LC_PlayerBase* pkNetPlayer = pkNetPlayerManager->FindPlayer(lBeAppointmentCharID);
	
		if(pkNetPlayer)
			SendLogicInfoMessage(iAppointmentResult,pkNetPlayer->GetCharName().c_str());
		//else
			//SendLogicInfoMessage(RE_REPOINT_ERROR);
	}
}

//----------------------------------------------------------------------------
void HL_FromGameServer::_handlerNotifyGroupRaid(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	////�����δ������Ϸ�����˳�
	//if (eGameStage != ST_GAME_STAGE_GAME)
	//	return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();	
	if(!pkGamePlayer)
		return;

	MG_NOTIFY_GroupRaid rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"�޷�����MG_NOTIFY_GroupRaid");
		return;
	}

	LC_PlayerGroupBase* pkGroup = pkGamePlayer->GetPlayerGroup();
	if (NULL == pkGroup)
	{
		return;
	}

	if (rltMsg.m_Vaild)
		pkGroup->SetRaidPortal(rltMsg.m_LogicId, rltMsg.m_Resid, rltMsg.m_LogicType, (Share_Type)(rltMsg.m_Shartype), (Exclusive_Type)(rltMsg.m_Excludetype));
	else
		pkGroup->ClearRaidPortal();
}
//----------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltModityQuality(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	////�����δ������Ϸ�����˳�
	//if (eGameStage != ST_GAME_STAGE_GAME)
	//	return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();	
	if(!pkGamePlayer)
		return;

	MG_RLT_ModityQuality rltMsg;

	if(!rltMsg.Decode(pMsg,wSize))	
	{		
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"�޷�����MG_RLT_AppointmentCheif");		
		return;	
	}

	ResultType iResultType = rltMsg.m_iResult;

	if(iResultType == RE_GROUP_MODIFY_QUALITY_SUCCESS)
	{
		StringType strQulity;
		switch (rltMsg.m_iItemGrade)
		{
		case ITEM_GRADE_NONE:
			strQulity = "����ʰȡ";
			break;	
		case ITEM_GRADE_WHITE:
			strQulity = "��ɫװ��";
			break;
		case ITEM_GRADE_BLUE:
			strQulity = "��ɫװ��";	
			break;
		case ITEM_GRADE_GOLDEN:
			strQulity = "��ɫʰȡ";
			break;	
		case ITEM_GRADE_DARKGOLDEN:
			strQulity = "����װ��";
			break;
		case ITEM_GRADE_GREEN:
			strQulity = "��ɫװ��";	
			break;
		case ITEM_GRADE_RED:
			strQulity = "��ɫװ��";	
			break;
		default:
			;
		}

		SendLogicInfoMessage(iResultType, strQulity.c_str());
		SendLogicInfoMessage(RE_GROUP_MODIFY_ROLL_WEAPON_TYPE);

		LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
		//if(pkGamePlayer)
		{
			LC_PlayerGroupBase* pkGroup = pkGamePlayer->GetPlayerGroup();
			if ( pkGroup )
			{
				pkGroup->SetItemGrade(rltMsg.m_iItemGrade);
			}
		}		
	}
	else
	{
		SendLogicInfoMessage(iResultType);
	}
}
//----------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltForbidJoin(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	////�����δ������Ϸ�����˳�
	//if (eGameStage != ST_GAME_STAGE_GAME)
	//	return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();	
	if(!pkGamePlayer)
		return;

	MG_RLT_ForbidJion rltMsg;

	if(!rltMsg.Decode(pMsg,wSize))	
	{		
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"�޷�����MG_RLT_ForbidJion");		
		return;	
	}

	ResultType iResultType = rltMsg.m_iResult;

	if(iResultType == RE_GROUP_FROBID_JOIN_SUCCESS)
	{
		LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
		LC_PlayerGroupBase* pkGroup = pkGamePlayer->GetPlayerGroup();
		if ( pkGroup )
		{
			pkGroup->SetForbidJoinFlag(rltMsg.m_bForbidJoinFlag);
		}

		if (true == rltMsg.m_bForbidJoinFlag)
		{
			SendLogicInfoMessage(RE_GROUP_FROBID_JOIN_SUCCESS_OK);
		}
		else
		{
			SendLogicInfoMessage(RE_GROUP_FROBID_JOIN_SUCCESS_CANCEL);
		}
	}
	else
	{
		SendLogicInfoMessage(iResultType);
	}
}
//----------------------------------------------------------------------------
