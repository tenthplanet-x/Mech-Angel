//------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltSetAssistInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//如果还未进入游戏，则退出
	//if (eGameStage != ST_GAME_STAGE_GAME)
	//	return;
	
	MG_Assist_Info_Set_Rlt rltMsg;
	if(rltMsg.Decode(pMsg,wSize))
	{
		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
		if( pkGamePlayer == NULL )
			return;

		pkGamePlayer->GetAssistAsset().GetDataFromProtocolStruct(rltMsg.m_AssistInfo);

		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_CLIENT_ASSIST_INFO_SET);
		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);

		LC_ShortcutBar *pkShortCutBar = pkGamePlayer->GetShortcutBar();
		if(NULL == pkShortCutBar)
		{
			return;
		}

		if(RE_SUCCESS == rltMsg.m_uiRslt)
		{
			pkShortCutBar->ShortcutPackage(1, SHORTCUT_INDEX_ITEM, rltMsg.m_AssistInfo.m_iHPTypeID, PACK_TYPE_BASIC);
			pkShortCutBar->ShortcutPackage(2, SHORTCUT_INDEX_ITEM, rltMsg.m_AssistInfo.m_iHPTypeID, PACK_TYPE_BASIC);
		}
		else
		{
			pkShortCutBar->ShortcutRemove(1, SHORTCUT_INDEX_ITEM);
			pkShortCutBar->ShortcutRemove(2, SHORTCUT_INDEX_ITEM);
		}
	}
}
void HL_FromGameServer::_handlerRltGetHookingGuideLocation(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//如果还未进入游戏，则退出
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;


	MG_Hooking_Guide_Location_Rlt rltMsg;
	if(rltMsg.Decode(pMsg,wSize))
	{
		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
		if( pkGamePlayer == NULL )
			return;
		
		if (rltMsg.m_bFind)
		{
			pkGamePlayer->SetHookGuideLocation(UT_Vec3Int(rltMsg.m_iX, rltMsg.m_iY, rltMsg.m_iZ));
		}
		else
		{
			pkGamePlayer->SetHookGuideLocation(pkGamePlayer->GetHookLocation());
		}

	}
}
