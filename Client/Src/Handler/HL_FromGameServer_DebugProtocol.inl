
//------------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltDebugCommandLine(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RLT_DebugCommandLine kRlt;
	if (!kRlt.Decode(pMsg, wSize))
	{
		return;
	}

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_RLT_DEBUG_COMMANDLINE, &kRlt);

	if (bCreateMsg && NW_ClientSessionManager::GetSingletonPtr() != NULL )
	{
		NW_ClientSessionManager* pkManager = NW_ClientSessionManager::GetSingletonPtr();
		NetWork::NW_NetSessionBase* pkSession = pkManager->GetDebugSession();
		if ( pkSession != NULL )
		{
			pkSession->Send(hMsgHandler.GetBuff(),hMsgHandler.GetMsgSize());
		}
		SendLogicInfoMessage(RE_CHAT_NEAR,kRlt.m_strMsg.c_str());
		GfxWriteLog( LOG_TAG_GAME_LOG , LOG_SWITCH_LUABIND ,"lua debug info [%s]" , kRlt.m_strMsg.c_str());
	}
}
//------------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltDebugActorAIInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RLT_DebugActorAIInfo kRlt;
	if (!kRlt.Decode(pMsg, wSize))
	{
		return;
	}

	object_id_type lPlayerID = kRlt.m_lPlayerID;
	object_id_type lActorID = kRlt.m_lTargetID;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if (NULL == pkGamePlayer || pkGamePlayer->GetID() != lPlayerID)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"请求添加物品到口诀镶嵌返回消息逻辑错误");
		return;
	}

	if (!IS_NPC_ID(lActorID))
	{
		return;
	}
	LC_WorldManagerBase* pkWorldManager = ENGINE_GET_WORLD_MANAGER();
	if (NULL == pkWorldManager)
	{
		return;
	}
	LC_ActorBase* pkActor = (LC_ActorBase*)pkWorldManager->FindObject(lActorID);
	if (NULL == pkActor)
	{
		return;
	}
	LC_ActorHatredList* pkHatredList = pkActor->GetHatredList();
	//if (NULL == pkHatredList)
	//{
	//	return;
	//}

	pkHatredList->Reset();
	LC_ActorHatredEntry kHatredEntry;
	for (int i = 0; i < MAX_ACTOR_HATRED_COUNT; i ++)
	{
		long lHatredID = kRlt.m_alHatredList[i];
		if (IS_NPC_ID(lHatredID) || IS_PLAYER_CLASS_ID(lHatredID))
		{
			kHatredEntry.SetTargetID(lHatredID);
			kHatredEntry.SetHatredValue(0);
			kHatredEntry.SetUpdateTime(0.0f);
			pkHatredList->AddHatred(kHatredEntry);
		}
	}
}
