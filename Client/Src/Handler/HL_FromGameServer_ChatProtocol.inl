
#include "CL_RichTextParse.h"
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltChatRequest(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//�����δ������Ϸ�����˳�
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;


	MG_RLT_RequestChat rltMsg;
	if(rltMsg.Decode(pMsg,wSize))
	{
		if(rltMsg.m_iResult != RE_CHAT_SEND_MSG_SUCCESS)
		{
			SendLogicInfoMessage(rltMsg.m_iResult);
		}
		/*else if (rltMsg.m_iChatChannel == CCT_TRUMPET || rltMsg.m_iChatChannel == CCT_SUPER_TRUMPET)
		{
			if (rltMsg.m_iShowTime > 0 )
				SendLogicInfoMessage(RE_CHAT_TRUMPET_SHOW_TIME,rltMsg.m_iShowTime);
		}*/
	}
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerReceiveChatMsg(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//�����δ������Ϸ�����˳�
	//if (eGameStage != ST_GAME_STAGE_GAME)
	//	return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( pkGamePlayer == NULL )
		return;

	MG_GC_ChatMessage ChatMsg;
	if(!ChatMsg.Decode(pMsg,wSize))
		return;

	if(NULL == GUISys::getRichTextParse)
		return;

	//StringType Text;
	//TT_TextManager::EncodeXMLString(pkTextFilter->GetText(),Text);

	ResultType	iChatResult = RE_FAIL;

	bool IsBlock = false; 
	bool bMultipy = true;
	MG_MessageCenter* pkMessageCenter = MG_MessageCenter::GetSingletonPtr();
	if(!pkMessageCenter)
		return;

	//�������к��������������Ϣ
	LC_ClientSocietyAsset *pkSocietyAsset = pkGamePlayer->GetSocietyAsset();
	//if(pkSocietyAsset)
	{
		LC_SocietyBasicInfo *pkInfo = pkSocietyAsset->GetBlackList(ChatMsg.m_lCharID);
		if(pkInfo)
		{
			return;
		}
	}

	switch(ChatMsg.m_iChatChannel)
	{
	case CCT_PRIVATE:
		if ( ChatMsg.m_lCharID == pkGamePlayer->GetCommanderUID() && ChatMsg.m_strPlayerName == pkGamePlayer->GetCommanderName())  //�����ϢӦ�����������������
		{
			ChatMsg.m_strPlayerName = ChatMsg.m_strTargetName;
		}
		else if (ChatMsg.m_lTargetCharID != pkGamePlayer->GetCommanderUID())
		{
			// ������ʱ�����б�,��ͬ�߲��ü�
			pkGamePlayer->AddTempFriend(ChatMsg.m_lTargetCharID, STDSTR2TPSTR(ChatMsg.m_strPlayerName), -1, 0);		
		}
		else if(NULL == pkSocietyAsset->GetFriendInfo1(ChatMsg.m_lCharID))
		{
			//���Ǻ��ѵ�˽����Ϣ����
			return;
		}
		iChatResult = RE_CHAT_PRIVATE;
		bMultipy = false;
		//strSoundPath = LS_LuaStateFactory::GetSingletonPtr()->GetStringFromLogicState("UI_PRIVATE_CHAT");
		g_soundMgr.Play2DEx(VARIABLE2STRING(SE_Sys_Chat));
		break;
	//��������Ƶ��������
	case CCT_SYSTEM:
		iChatResult = RE_CHAT_SYSTEM;
		break;

	case CCT_GROUP:
		iChatResult = RE_CHAT_GROUP;
		g_soundMgr.Play2DEx(VARIABLE2STRING(SE_Sys_Chat));
		break;
	case CCT_GUILD:	
		{
			iChatResult = RE_CHAT_GUILD;
			g_soundMgr.Play2DEx(VARIABLE2STRING(SE_Sys_Chat));
			LC_ClientSocietyAsset* asset = pkGamePlayer->GetSocietyAsset();
			//if(asset)
			{
				LC_SocietyBasicInfo* info = asset->GetBlackList(ChatMsg.m_lCharID);
				if (info )
				{
					 IsBlock = true;
				}
			}
		}
		break;
	case CCT_NEAR:
		iChatResult = RE_CHAT_NEAR;
		break;
	case CCT_WORLD:
		iChatResult = RE_CHAT_WORLD;
		break;
	case CCT_TRUMPET:
		iChatResult = RE_CHAT_TRUMPET;
		{
			LC_ClientSocietyAsset* asset = pkGamePlayer->GetSocietyAsset();
			//if(asset)
			{
				LC_SocietyBasicInfo* info = asset->GetBlackList(ChatMsg.m_lCharID);
				if (info )
				{
					IsBlock = true;
				}
			}
		}
		break;
	case CCT_SUPER_TRUMPET:
		iChatResult = RE_CHAT_SUPER_TRUMPET;
		{
			LC_ClientSocietyAsset* asset = pkGamePlayer->GetSocietyAsset();
			//if(asset)
			{
				LC_SocietyBasicInfo* info = asset->GetBlackList(ChatMsg.m_lCharID);
				if (info )
				{
					IsBlock = true;
				}
			}
		}
		break;
	//case CCT_SYSTEM:
	//	iChatResult = RE_;
	//	break;
	case CCT_FRIEND:
		iChatResult = RE_CHAT_FRIEND;
		if ( pkGamePlayer->GetInstance() == ChatMsg.m_lCharID )
		{
			LC_GameEvent kGameEvent;
			kGameEvent.SetEventType(GLET_RECEIVE_FRIEND_CHAT);
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_1,unique_id_impl(ChatMsg.m_lCharID).detail());
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_2,unique_id_impl(ChatMsg.m_lCharID).catagory());

			UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
			pkUIManager->HandlerGameEvent(kGameEvent);
		}
		g_soundMgr.Play2DEx(VARIABLE2STRING(SE_Sys_Chat));
		break;
	/*case  CCT_UNION:
		iChatResult = RE_CHAT_FAMILY;
		break;*/
	case  CCT_ANNOUNCE:
		iChatResult = RE_CHAT_ANNOUNCE;
		break;
	case  CCT_ANNOUNCE_ONLY:
		iChatResult = RE_CHAT_ANNOUNCE_ONLY;
		break;
	case CCT_ANNOUNCE_NO_LIMIT:
		iChatResult = RE_CHAT_ANNOUNCE_NO_LIMIT;
		break;
	default:
		//T_ASSERT(false);
		break;
	}

	bool bFilter = false;
	bool bReplaceEnter = false;
	StringType kOutput = STDSTR2TPSTR(ChatMsg.m_strChatContent);

	//BUG #30241 �����ܡ������˶Խӡ�yy����ͻ����ϱ�
	if (unique_id_impl(ChatMsg.m_lCharID).detail() > 0 && unique_id_impl(ChatMsg.m_lCharID).catagory() > 0 && ChatMsg.m_lCharID != pkGamePlayer->GetInstance())
	{
		report2duowan_async_chatmsg(kOutput.c_str(), pkGamePlayer->GetCharName().c_str(), ChatMsg.m_strPlayerName.c_str(), (int)bMultipy, pkGamePlayer->GetMapResID());
	}
	CF_SystemError::DataEntry *pkErrorMsg = SafeGetCSVFileDataEntryPtr<CF_SystemError>(iChatResult);
	if(pkErrorMsg)
	{
		bFilter = true;
		CF_TextConfig::DataEntry *pkTextConfig = SafeGetCSVFileDataEntryPtr<CF_TextConfig>(pkErrorMsg->_lMsgType);
		if(pkTextConfig)
		{
			//BUG #22101 �����ܡ������ء��͵ȼ���Ҳ��˹�����յ��������صĹ�����Ϣ
			//if (pkTextConfig->_lChannelLevelLimit > pkGamePlayer->GetLevel())
			if (pkTextConfig->_lChannelLevelLimit > pkGamePlayer->GetCommanderLevel())
			{
				return;
			}

			if(!pkTextConfig->_bChannelChat_Tab_System)
				bReplaceEnter = true;

			if(pkTextConfig->_bChannelAnnounce || pkTextConfig->_bChannelTip || pkTextConfig->_bChannelChat_Tab_System
				|| (iChatResult == RE_CHAT_WORLD && ChatMsg.m_strChatContent.find("|#@yh|") != std::string::npos))
				//ߺ�Ȳ��������ֹ���
				bFilter = false;
		}
	}

	if(bReplaceEnter)
	{
		GUISys::getRichTextParse->ReplaceAll(kOutput, "\n", "", kOutput);
	}

	if(bFilter)
	{
		GUISys::getRichTextParse->FilterForbiddenText(kOutput, kOutput, TT_TextFilter::TFT_CHAT);
	}

	if ( !IsBlock )
	{
		StringType kOutputName = STDSTR2TPSTR(ChatMsg.m_strPlayerName);
		//GUISys::getRichTextParse->FilterForbiddenText(kOutputName, kOutputName, TT_TextFilter::TFT_CHAR_NAME);
		SendChatMsg(iChatResult, kOutput, kOutputName, LC_Helper::GetSystemTimeString(), ChatMsg.m_lCharID, ChatMsg.m_lTargetCharID, ChatMsg.m_uiUserID, ChatMsg.m_uiGMPriority);
	}
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltUpdateChatChannel(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//�����δ������Ϸ�����˳�
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;


// 	MG_GC_ChatMessage ChatMsg;
// 	if(ChatMsg.Decode(pMsg,wSize))
// 	{
// 
// 	}
}