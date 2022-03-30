
void HL_FromGameServer::_handlerChestShowContent(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
}


void HL_FromGameServer::_handlerChestPickUp(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();
	
	//如果还未进入游戏，则退出
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;


	MG_RLT_PickUpPlaceHolder msg;
	if (msg.Decode(pMsg, wSize))
	{
		LC_ClientGamePlayer* pkPlayer = pkSession->GetPlayer();
		pkPlayer->GetFlopManager()->OnRecvChestPickUp(msg);
	}
}

void HL_FromGameServer::_handlerChestItemRoll(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
}

void HL_FromGameServer::_handlerChestOPResult(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
}