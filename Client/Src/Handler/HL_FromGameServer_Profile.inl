
void HL_FromGameServer::_handlerRltProfileName(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_Char_Profile kMsg;

	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "CharProfile协议解析错误 MG_RLT_Char_Profile");	
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( !pkGamePlayer )
	{
		return;
	}
	

	// 检查好友系统是否有查询
	NW_NetCharSimpleInfo kSimpleInfo;
	kSimpleInfo.m_iProfileResult = kMsg.m_iResult;
	kSimpleInfo.m_lUserID		 = kMsg.m_uiUserID;
	kSimpleInfo.m_lCharID		 = kMsg.m_uiCharID;
	NW_ClientSessionManager* pkSessionManager = NW_ClientSessionManager::GetSingletonPtr();
	pkSessionManager->AddNetCharSimpleInfo(STDSTR2TPSTR(kMsg.m_szCharName), kSimpleInfo);
		

	if(RE_SUCCESS == kMsg.m_iResult)
	{
		// 好友系统	
		LC_ClientSocietyAsset* pkSocietyAsset = pkGamePlayer->GetSocietyAsset();
		//if(pkSocietyAsset)
		{
			pkSocietyAsset->UpdatePlayerProfile(kMsg);
		}

		if (/*pkSocietyAsset &&*/ pkSocietyAsset->IsExistSearchName(STDSTR2TPSTR(kMsg.m_szCharName)))
		{
			StringType strMsg = pkSocietyAsset->GetAccurateSearchMsg(STDSTR2TPSTR(kMsg.m_szCharName));
			//如果请求的角色信息，是要添加好友的角色信息
			pkGamePlayer->AddFriend(kMsg.m_uiCharID, STDSTR2TPSTR(kMsg.m_szCharName), strMsg);
			
			pkSocietyAsset->EraseAccurateSearchName(STDSTR2TPSTR(kMsg.m_szCharName));
		}

		LC_ClientMailManager* pkMailManager = LC_ClientMailManager::GetSingletonPtr();
		if (pkMailManager)
		{
			if ( pkMailManager->IsExistPendingName(STDSTR2TPSTR(kMsg.m_szCharName)))
			{
				pkMailManager->SendPendingMail();				
				pkMailManager->ErasePendingName(STDSTR2TPSTR(kMsg.m_szCharName));				
			}
		}

		if ( pkGamePlayer->IsSearchingChatName(STDSTR2TPSTR(kMsg.m_szCharName)))
		{
			PrivateChatSaveSt& pst = pkGamePlayer->GetSearchingChatTarget();
			pst.targetID = kMsg.m_uiCharID;
			pkGamePlayer->SetChatTargetID(kMsg.m_uiCharID);
			pkGamePlayer->SendSavePrivateChatMsg();
			pkGamePlayer->EraseSearchingChatName();
		}
	}
	else
	{
		LC_ClientSocietyAsset* pkSocietyAsset = pkGamePlayer->GetSocietyAsset();
		if (/*pkSocietyAsset &&*/ pkSocietyAsset->IsExistSearchName(STDSTR2TPSTR(kMsg.m_szCharName)))
		{			
			pkSocietyAsset->EraseAccurateSearchName(STDSTR2TPSTR(kMsg.m_szCharName));
			SendLogicInfoMessage(RE_ADD_FRIEND_FAILED_TARGET_NOT_EXIST, STDSTR2TPSTR(kMsg.m_szCharName));
			return;
		}		
				
		// 邮件系统更新
		LC_ClientMailManager* pkMailManager = LC_ClientMailManager::GetSingletonPtr();
		if (pkMailManager)
		{
			if ( pkMailManager->IsExistPendingName(STDSTR2TPSTR(kMsg.m_szCharName)))
			{
				pkMailManager->ErasePendingName(STDSTR2TPSTR(kMsg.m_szCharName));				
				SendLogicInfoMessage(RE_WRITEMAIL_FAIL_NAME_IS_NOT_EXIST, STDSTR2TPSTR(kMsg.m_szCharName));
			}
		}

		if ( pkGamePlayer->IsSearchingChatName(STDSTR2TPSTR(kMsg.m_szCharName)))
		{
			SendLogicInfoMessage(RE_CHAT_TARGET_OFFLINE);
			pkGamePlayer->EraseSearchingChatName();
		} 
	}
}