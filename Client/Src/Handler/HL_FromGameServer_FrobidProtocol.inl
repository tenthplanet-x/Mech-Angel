

void HL_FromGameServer::_handlerRltForbidList(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//�����δ������Ϸ�����˳�
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;


	MG_RLT_ForbidList msg;

	if ( msg.Decode(pMsg,wSize) )
	{
		object_id_type lPlayerID = msg.m_lPlayerID;
		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

		if (!pkGamePlayer || pkGamePlayer->GetID() != lPlayerID)
		{
			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "�����б���Ϣ�߼�����");
			return;
		}

		LC_ForbidList*	pkForbidList = pkGamePlayer->GetForbidList();
		pkForbidList->SetAllForidInfo(msg.m_kAllForbidInfo);
	}
}