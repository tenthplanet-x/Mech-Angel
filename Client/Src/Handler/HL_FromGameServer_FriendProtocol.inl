#include "MG_CS_FriendProtocol.h"

void HL_FromGameServer::_handlerRltAddFriend(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_AddFriend kMsg;
	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "添加好友结果协议解析错误 MG_RLT_AddFriend");	
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( !pkGamePlayer )
	{
		return;
	}


	if ( RE_ADD_FRIEND_SUCCESS == kMsg.m_iResultType )
	{
		pkGamePlayer->GetSocietyAsset()->DeleteSociety(ST_TEMP_FRIEND,kMsg.m_lTargetCharID);
		SendLogicInfoMessage(kMsg.m_iResultType, STDSTR2TPSTR(kMsg.m_szCharName));
	}
	else
	{
		if( RE_ADD_FRIEND_FAILED_IS_EXIST == kMsg.m_iResultType )
		{
			SendLogicInfoMessage(kMsg.m_iResultType, STDSTR2TPSTR(kMsg.m_szCharName));
			return;
		}
		SendLogicInfoMessage(kMsg.m_iResultType);
	}
	
}

void HL_FromGameServer::_handlerRltDelFriend(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_DelFriend kMsg;	
	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "删除好友结果协议解析错误 MG_RLT_DelFriend");	
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( !pkGamePlayer )
	{
		return;
	}

	if (RE_DEL_FRIEND_SUCCESS == kMsg.m_iResultType)
	{		
		StringType strName;
		LC_FriendDetailInfo* pkFriendInfo = pkGamePlayer->GetSocietyAsset()->GetFriendInfo1(kMsg.m_lTargetCharID);
		if (pkFriendInfo)
		{
			strName = pkFriendInfo->GetCharName();
		}
		pkGamePlayer->GetSocietyAsset()->DeleteSociety(ST_FRIEND, kMsg.m_lTargetCharID);

		SendLogicInfoMessage(kMsg.m_iResultType, strName);
	}
	else
	{
		SendLogicInfoMessage(kMsg.m_iResultType);
	}
}

void HL_FromGameServer::_handlerRltGetFriendList(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_GetFriendList kMsg;
	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "获取好友列表结果协议解析错误 MG_RLT_GetFriendList");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( !pkGamePlayer )
	{
		return;
	}

	{
		uint16_t uiCount = kMsg.m_rFriendList.m_iCount;

		for (int i = 0; i < uiCount; ++i)
		{
			pkGamePlayer->GetSocietyAsset()->UpdateFriend(kMsg.m_rFriendList.m_akInfos[i]);
		}

		pkGamePlayer->GetSocietyAsset()->Sort(ST_FRIEND);
	}
}

void HL_FromGameServer::_handlerRltAddEnemy(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_AddEnemy kMsg;
	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "添加仇人结果协议解析错误 MG_RLT_AddEnemy");	
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( !pkGamePlayer )
	{
		return;
	}

	if (RE_ADD_ENEMY_SUCCESS == kMsg.m_iResultType)
	{		
		pkGamePlayer->GetSocietyAsset()->UpdateSociety(ST_ENEMY, kMsg.m_kEnemyInfo);
		pkGamePlayer->GetSocietyAsset()->Sort(ST_ENEMY);

		SendLogicInfoMessage(kMsg.m_iResultType, STDSTR2TPSTR(kMsg.m_kEnemyInfo.szCharName));
	}
	else if(RE_FRIEND_UNKOWN_CHAR != kMsg.m_iResultType)
	{
		SendLogicInfoMessage(kMsg.m_iResultType);
	}	
}

void HL_FromGameServer::_handlerRltDelEnemy(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_DelEnemy kMsg;
	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "删除仇人结果协议解析错误 MG_RLT_DelEnemy");	
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( !pkGamePlayer )
	{
		return;
	}

	if (RE_DEL_ENEMY_SUCCESS == kMsg.m_iResultType)
	{		
		StringType strName;
		LC_SocietyBasicInfo* pkEnemy = pkGamePlayer->GetSocietyAsset()->GetEnemy(kMsg.m_lTargetCharID);
		if (pkEnemy)
		{
			strName = pkEnemy->GetCharName();
		}

		pkGamePlayer->GetSocietyAsset()->DeleteSociety(ST_ENEMY, kMsg.m_lTargetCharID);
		SendLogicInfoMessage(kMsg.m_iResultType, strName);
	}
	else
	{
		SendLogicInfoMessage(kMsg.m_iResultType);
	}	
}

void HL_FromGameServer::_handlerRltGetEnemyList(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_GetEnemyList kMsg;
	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "获取仇人列表结果协议解析错误 MG_RLT_GetEnemyList");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if ( !pkGamePlayer )
	{
		return;
	}

	{
		uint16_t uiCount = kMsg.m_rEnemyList.m_iCount;

		for (int i = 0; i < uiCount; ++i)
		{
			pkGamePlayer->GetSocietyAsset()->UpdateSociety(ST_ENEMY, kMsg.m_rEnemyList.m_akInfos[i]);
		}		
		pkGamePlayer->GetSocietyAsset()->Sort(ST_ENEMY);
	}

	////在调试窗口打印仇人列表
	//LC_ClientSocietyAsset* pkSocietyAsset = pkGamePlayer->GetSocietyAsset();
	//if (NULL != pkSocietyAsset)
	//{
	//	int iEnemyCount = pkSocietyAsset->GetCount(ST_ENEMY);
	//	LC_SocietyBasicInfo* pkSocietyEntry = NULL;
	//	for (int i = 0; i < iEnemyCount; i ++)
	//	{
	//		pkSocietyEntry = pkSocietyAsset->GetSocietyInfo(ST_ENEMY, i);
	//		if (NULL != pkSocietyEntry && pkSocietyEntry->GetValid())
	//		{
	//			GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "仇人[%d][%s]", i, pkSocietyEntry->GetCharName().c_str());
	//		}
	//	}
	//}
}

void HL_FromGameServer::_handlerRltAddBlackList(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_AddBlackList kMsg;
	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "添加黑名单结果协议解析错误 MG_RLT_AddBlackList");	
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( !pkGamePlayer )
	{
		return;
	}

	if (RE_ADD_BLACKLIST_SUCCESS == kMsg.m_iResultType)
	{		
		pkGamePlayer->GetSocietyAsset()->UpdateSociety(ST_BLACKLIST, kMsg.m_kBlackListInfo);
		SendLogicInfoMessage(kMsg.m_iResultType, STDSTR2TPSTR(kMsg.m_kBlackListInfo.szCharName));
	}
	else
	{
		SendLogicInfoMessage(kMsg.m_iResultType);
	}	
}

void HL_FromGameServer::_handlerRltDelBlackList(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_DelBlackList kMsg;
	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "删除仇人结果协议解析错误 MG_RLT_DelEnemy");	
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( !pkGamePlayer )
	{
		return;
	}

	if (RE_DEL_BLACKLIST_SUCCESS == kMsg.m_iResultType)
	{		
		StringType strName;
		LC_SocietyBasicInfo* pkInfo = pkGamePlayer->GetSocietyAsset()->GetBlackList(kMsg.m_lTargetCharID);

		if (pkInfo)
		{
			strName = pkInfo->GetCharName();
		}

		pkGamePlayer->GetSocietyAsset()->DeleteSociety(ST_BLACKLIST, kMsg.m_lTargetCharID);

		SendLogicInfoMessage(kMsg.m_iResultType, strName);
	}
	else
	{
		SendLogicInfoMessage(kMsg.m_iResultType);
	}	
}

void HL_FromGameServer::_handlerRltGetBlackList(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_GetBlackList kMsg;
	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "获取黑名单列表结果协议解析错误 MG_RLT_GetBlackList");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if ( !pkGamePlayer )
	{
		return;
	}

	{
		uint16_t uiCount = kMsg.m_rBlackList.m_iCount;

		for (int i = 0; i < uiCount; ++i)
		{
			pkGamePlayer->GetSocietyAsset()->UpdateSociety(ST_BLACKLIST, kMsg.m_rBlackList.m_akInfos[i]);
		}		

		return;
	}
}

void HL_FromGameServer::_handlerRltGetFriendStatus(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_GetFriendStatus kMsg;
	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "请求转换好友状态结果协议解析错误 MG_RLT_GetFriendStatus");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if ( !pkGamePlayer )
	{
		return;
	}

	NW_ClientSessionManager* pkSessionManager = NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup* pkSessionGroup = pkSessionManager->GetGameSessionGroup();
	int	iCurrentGameServerID = pkSessionGroup->GetCurGameServer();


	//现在不需要这个了，服务器会发了
	if( kMsg.m_bClearStatus )
		pkGamePlayer->GetSocietyAsset()->SetStatusOffline(kMsg.m_uiSocietyType);
	
	for (int i = 0; i < kMsg.m_currentArraySize; ++i)
	{
		pkGamePlayer->GetSocietyAsset()->SetOnlineStatus(kMsg.m_uiSocietyType, kMsg.m_lOnlinePlayerCharID[i],kMsg.m_lOnlinePlayerDegree[i],kMsg.m_lOnlinePlayerServerID[i]);
		if(iCurrentGameServerID >0 && iCurrentGameServerID != kMsg.m_lOnlinePlayerServerID[i])
		{
			LC_FriendDetailInfo* pFriend = pkGamePlayer->GetSocietyAsset()->GetFriendInfo1(kMsg.m_lOnlinePlayerCharID[i]);
			if( pFriend != NULL )
			{
				pkSessionManager->FindNetCharSimpleInfo( pFriend->GetCharName() );
			}
		}
		
	}
	pkGamePlayer->GetSocietyAsset()->Sort(ST_FRIEND);

}

void HL_FromGameServer::_handlerRltGetFriendDetail(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	// 暂时不处理
	MG_RLT_GetFriendDetail kRltMsg;
	if ( !kRltMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "请求获得玩家额外信息变化数据协议 MG_RLT_GetFriendDetail 解析错误");
		return ;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

	LC_ClientSocietyAsset* pkAsset = pkGamePlayer->GetSocietyAsset();
		
	pkAsset->UpdateFriend(kRltMsg.m_kFriendInfo);

}

void HL_FromGameServer::_handlerRltPlayerDeltaData(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_PlayerDeltaData kMsg;
	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "请求获得玩家变化数据协议 MG_RLT_PlayerDeltaData 解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if ( !pkGamePlayer )
	{
		return;
	}

	if (kMsg.m_iResult == RE_SUCCESS)
	{
		// 更新好友系统
		pkGamePlayer->GetSocietyAsset()->UpdatePlayerData(kMsg.m_lTargetCharID, kMsg.m_kData);
	}
}

void HL_FromGameServer::_handlerRltSyncFriendModifyName(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncModifyCharName Msg;
	if (!Msg.Decode(pMsg,wSize))
	{
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

	LC_ClientSocietyAsset* pkAsset = pkGamePlayer->GetSocietyAsset();
	pkAsset->UpdateSocietyPlayerName(Msg);
}

void HL_FromGameServer::_handlerRltGetSocietyDeltaData(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_SocietyDeltaData kMsg;
	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "请求获得好友变化数据协议 MG_RLT_SocietyDeltaData 解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if ( !pkGamePlayer )
	{
		return;
	}

	for (int i = 0; i < kMsg.m_currentArraySize; ++i)
	{
		pkGamePlayer->GetSocietyAsset()->UpdateSocietyData(kMsg.m_uiSocietyType, kMsg.m_alTargetCharID[i], kMsg.m_kData[i]);
	}
	pkGamePlayer->GetSocietyAsset()->Sort(kMsg.m_uiSocietyType);
}

void HL_FromGameServer::_handlerRltFriendMsgBroad(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_FriendMsgBroad kMsg;
	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "好友广播消息结果协议解析错误 MG_RLT_FriendMsgBroad");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if (NULL == pkGamePlayer)
	{
		return;
	}

	pkGamePlayer->GetFriendMsgBroad()->AddMsg(kMsg.m_lFriendID, kMsg.m_uiMsgType);	
}

void HL_FromGameServer::_handlerRltFriendOnlineNotice(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_FriendOnlineNotice kMsg;
	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "好友上线通知结果解析错误 MG_RLT_FriendOnlineNotice");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if (NULL == pkGamePlayer)
	{
		return;
	}

	LC_FriendDetailInfo* pkFriendInfo = pkGamePlayer->GetSocietyAsset()->GetFriendInfo1(kMsg.m_lFriendID);
	
	if (pkFriendInfo)
	{
		pkGamePlayer->GetSocietyAsset()->SetOnlineStatus(ST_FRIEND, kMsg.m_lFriendID,0, kMsg.m_lServerID );
		SendLogicInfoMessage(RE_FRIEND_ONLINE_NOTICE, pkFriendInfo->GetCharName().c_str());
		pkGamePlayer->GetSocietyAsset()->Sort( ST_FRIEND);

		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_FRIEND_OLINE_NOTICE);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, unique_id_impl(kMsg.m_lFriendID).detail());
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, unique_id_impl(kMsg.m_lFriendID).catagory());

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}	
}
void HL_FromGameServer::_handlerRltFriendOfflineNotice(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_FriendOfflineNotice kMsg;
	if ( !kMsg.Decode(pMsg, wSize) )
	{
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if (NULL == pkGamePlayer)
	{
		return;
	}

	LC_FriendDetailInfo* pkFriendInfo = pkGamePlayer->GetSocietyAsset()->GetFriendInfo1(kMsg.m_lFriendID);

	if (pkFriendInfo)
	{
		pkGamePlayer->GetSocietyAsset()->SetOfflineStatus(ST_FRIEND, kMsg.m_lFriendID);
		SendLogicInfoMessage(RE_FRIEND_OFFLINE_NOTICE, pkFriendInfo->GetCharName().c_str());
	}	
}
void HL_FromGameServer::_handlerRltAddFiendNick(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_AddFriendNick kMsg;
	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "添加好友外号解析错误 MG_RLT_FriendOnlineNotice");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if (NULL == pkGamePlayer)
	{
		return;
	}

	if (RE_FRIEND_ADD_NICK_SUCCESS == kMsg.m_iResult)
	{
		LC_ClientFriendNick* pkClientFriendNick = pkGamePlayer->GetFriendNick();
		LC_FriendNick* pkFriendNick = pkClientFriendNick->GetFriendNick(kMsg.m_lTargetCharID);
		if (NULL != pkFriendNick)
		{	
			pkFriendNick->AddNick(kMsg.m_lSrcCharID, STDSTR2TPSTR(kMsg.m_szNick));	

			pkClientFriendNick->AddDirtyFlagRevisionID();
		}
	}

	SendLogicInfoMessage(kMsg.m_iResult, STDSTR2TPSTR(kMsg.m_szNick));
}

void HL_FromGameServer::_handlerRltSupportFiendNick(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_SupportFriendNick kMsg;
	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "好友外号支持协议解析错误 MG_RLT_SupportFriendNick");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if (NULL == pkGamePlayer)
	{
		return;
	}

	if (RE_FRIEND_SUPPORT_NICK_SUCCESS == kMsg.m_iResult)
	{
		LC_ClientFriendNick* pkClientFriendNick = pkGamePlayer->GetFriendNick();
		LC_FriendNick* pkFriendNick = pkClientFriendNick->GetFriendNick(kMsg.m_lTargetFriendID);
		if (NULL != pkFriendNick)
		{
			pkFriendNick->Support(pkGamePlayer->GetInstance(), kMsg.m_lIndex);

			pkClientFriendNick->AddDirtyFlagRevisionID();
		}
	}

	SendLogicInfoMessage(kMsg.m_iResult);
}

void HL_FromGameServer::_handlerRltGetFriendNick(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_GetFriendNick kMsg;

	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "获得好友外号协议解析错误 MG_RLT_GetFriendNick");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if (NULL == pkGamePlayer)
	{
		return;
	}

	LC_ClientFriendNick* pkClientFriendNick = pkGamePlayer->GetFriendNick();
	LC_FriendNick* pkFriendNick = pkClientFriendNick->GetFriendNick(kMsg.m_lTargetFriendID);

	if (NULL != pkFriendNick)
	{
		pkClientFriendNick->AddDirtyFlagRevisionID();

		for (int i = 0; i < MAX_FRIEND_NICK_NUM; ++i)
		{
			LC_FriendNick::stNickInfo* pkNickInfo = pkFriendNick->GetNickInfo(i);
			if (pkNickInfo)
			{
				pkNickInfo->iCount = kMsg.m_kNickData[i].iCount;
				STRNCPY(pkNickInfo->szNick, kMsg.m_kNickData[i].szNick.c_str(), kMsg.m_kNickData[i].szNick.length()+1);
			}
		}		
	}
}

void HL_FromGameServer::_handlerRltBeAddFriend(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_BeAddFriend kMsg;

	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "请求被加为好友协议 MG_RLT_BeAddFriend");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if (NULL == pkGamePlayer)
	{
		return;
	}
	
	SendLogicInfoMessage(kMsg.m_iResult, STDSTR2TPSTR(kMsg.m_szCharName));
}

void HL_FromGameServer::_handReqConfirmBeAddFriend(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_REQ_Confirm_BeAddFriend kMsg;

	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "请求被加为好友协议 MG_REQ_Confirm_BeAddFriend");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if (NULL == pkGamePlayer)
	{
		return;
	}

	MG_MessageCenter* pkMessageCenter = MG_MessageCenter::GetSingletonPtr();
	if(!pkMessageCenter)
		return;

	//char kOutputMessage[256];
	//sprintf(kOutputMessage,"%s 请求加为好友, 是否同意?",);

// 	SendMessageDialog(RE_DIALOG_CONFIRM_BE_ADD_FRIND,kMsg.m_szCharName,"",
// 		MDT_CONFIRM_BEADDFRIEND,kMsg.m_lPlayerID,NO_USE_PARAMETER,NO_USE_PARAMETER);

	StringType kCharName = STDSTR2TPSTR(kMsg.m_szCharName);
	pkGamePlayer->GetSocietyAsset()->AddReqAddFriendCharInfo( kMsg.m_lTargetCharID, kCharName, STDSTR2TPSTR(kMsg.m_szMsg), kMsg.m_uiWeaponType, kMsg.m_uiLevel, kMsg.m_BornCharType);

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_CONFIRM_BEADD_FRIEND);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, unique_id_impl(kMsg.m_lTargetCharID).detail());
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, unique_id_impl(kMsg.m_lTargetCharID).catagory());

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerRltChangeSocietyInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	//MG_RLT_Change_Mood kMsg;
	MG_RLT_Change_SocietyInfo kMsg;

	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "请求心情、状态改变信息错误 MGPT_RLT_CHANGE_SOCIETY_INFO");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if (NULL == pkGamePlayer)
	{
		return;
	}

	MG_MessageCenter* pkMessageCenter = MG_MessageCenter::GetSingletonPtr();
	if(!pkMessageCenter)
		return;

	// 查询社交信息的心情改变
	LC_ClientSocietyAsset* pPlayerSocietyAsset = pkGamePlayer->GetSocietyAsset();
	
	//if(NULL == pPlayerSocietyAsset)
	//{ return; }

	bool nFlag = false;

	//查询好友
	//LC_FriendInfo* pPlayerFriendInfo = pPlayerSocietyAsset->GetFriendInfo();
	//if ( !nFlag )
	for(int i = 0 ; i < pPlayerSocietyAsset->GetFriendCount(); i++)
	{
		LC_FriendDetailInfo* pSocietyDetailInfo = pPlayerSocietyAsset->GetFriendInfo(i);
		if ( pSocietyDetailInfo )
		{
			if (pSocietyDetailInfo->GetCharID() == kMsg.m_lCharID)
			{
				if (kMsg.m_cChangeType == SICT_Mood)
				{
					pSocietyDetailInfo->SetCharMood(STDSTR2TPSTR(kMsg.m_szValue));
					pkGamePlayer->GetSocietyAsset()->SetDirtyFlag(ST_FRIEND, true);
					pkGamePlayer->GetSocietyAsset()->AddDirtyFlagRevisionID(ST_FRIEND);
					nFlag = true;
					break;
				}
				else if (kMsg.m_cChangeType == SICT_Status)
				{
					pSocietyDetailInfo->GetBasic()->SetStatus(kMsg.m_Value);
					pkGamePlayer->GetSocietyAsset()->SetDirtyFlag(ST_FRIEND, true);
					pkGamePlayer->GetSocietyAsset()->AddDirtyFlagRevisionID(ST_FRIEND);
					nFlag = true;
					break;
				}
			}
		}
	}

	//查询周围的玩家
	if (!nFlag)
	{
		for(int i = 0 ; i < pPlayerSocietyAsset->GetCount(ST_TEMP_FRIEND); i++)
		{
			LC_SocietyBasicInfo* pSocietyDetailInfo = pPlayerSocietyAsset->GetSocietyInfo(ST_TEMP_FRIEND, i);
			if ( pSocietyDetailInfo )
			{
				if (pSocietyDetailInfo->GetCharID() == kMsg.m_lCharID)
				{
					if (kMsg.m_cChangeType == SICT_Mood)
					{
						pkGamePlayer->GetSocietyAsset()->SetDirtyFlag(ST_TEMP_FRIEND, true);
						pkGamePlayer->GetSocietyAsset()->AddDirtyFlagRevisionID(ST_TEMP_FRIEND);
						nFlag = true;
						break;
					}
					else if (kMsg.m_cChangeType == SICT_Status)
					{
						pSocietyDetailInfo->SetStatus(kMsg.m_Value);
						pkGamePlayer->GetSocietyAsset()->SetDirtyFlag(ST_TEMP_FRIEND, true);
						pkGamePlayer->GetSocietyAsset()->AddDirtyFlagRevisionID(ST_TEMP_FRIEND);
						nFlag = true;
						break;
					}
				}
			}
		}	
	}

	//查询仇人
	if ( !nFlag )
	{	
		for(int i = 0 ; i < pPlayerSocietyAsset->GetCount(ST_ENEMY); i++)
		{
			LC_SocietyBasicInfo* pSocietyDetailInfo = pPlayerSocietyAsset->GetSocietyInfo(ST_ENEMY, i);
			if ( pSocietyDetailInfo )
			{
				if (pSocietyDetailInfo->GetCharID() == kMsg.m_lCharID)
				{
					if (kMsg.m_cChangeType == SICT_Mood)
					{
						pkGamePlayer->GetSocietyAsset()->SetDirtyFlag(ST_ENEMY, true);
						pkGamePlayer->GetSocietyAsset()->AddDirtyFlagRevisionID(ST_ENEMY);
						nFlag = true;
						break;
					}
					else if (kMsg.m_cChangeType == SICT_Status)
					{
						pSocietyDetailInfo->SetStatus(kMsg.m_Value);
						pkGamePlayer->GetSocietyAsset()->SetDirtyFlag(ST_ENEMY, true);
						pkGamePlayer->GetSocietyAsset()->AddDirtyFlagRevisionID(ST_ENEMY);
						nFlag = true;
						break;
					}
				}
			}
		}	
	}
	
 	//查询黑名单
// 	if ( !nFlag )
// 	{	
// 		for(int i = 0 ; i < pPlayerSocietyAsset->GetCount(ST_BLACKLIST); i++)
// 		{
// 			LC_SocietyBasicInfo* pFriendDetailInfo = pPlayerSocietyAsset->GetSocietyInfo(ST_BLACKLIST, i);
// 			if ( pFriendDetailInfo )
// 			{
// 				if (!pFriendDetailInfo->GetValid() || pFriendDetailInfo->GetCharID() == kMsg.m_lPlayerID)
// 				{
// 					pFriendDetailInfo->SetCharMood(kMsg.m_szNewMood);
// 						
// 					nFlag = true;
// 					break;
// 				}
// 			}
// 		}	
// 	}
}

void HL_FromGameServer::_handlerRltSyncSummorPool(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	
}

void HL_FromGameServer::_handlerRltGetPlayerNearBy(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_GetPlayerNearby kMsg;
	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "获取局域网玩家列表结果协议解析错误 MG_RLT_GetFriendList");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( !pkGamePlayer )
	{
		return;
	}

	{
		uint32_t uiCount = kMsg.m_kPlayerList.size();

		pkGamePlayer->GetSocietyAsset()->ClearSociety(ST_NEARBY_PLAYER);
		for (uint32_t i = 0; i < uiCount; ++i)
		{
			pkGamePlayer->GetSocietyAsset()->UpdateSociety(ST_NEARBY_PLAYER, kMsg.m_kPlayerList.at(i));
		}
		pkGamePlayer->GetSocietyAsset()->Sort(ST_NEARBY_PLAYER);
	}
}

void HL_FromGameServer::_handlerRltSocietyDynamicInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncSocietyDynmicInfo kMsg;
	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "获取社会信息动态同步协议解析错误 MG_RLT_GetFriendList");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( !pkGamePlayer )
	{
		return;
	}

	{
		uint32_t uiCount = kMsg.m_kPlayerList.size();

		for (uint32_t i = 0; i < uiCount; ++i)
		{
			pkGamePlayer->GetSocietyAsset()->UpdateSyncSocietyDynmicInfo(kMsg.m_kPlayerList.at(i));
		}

		//只有这两个类型有去排序
		pkGamePlayer->GetSocietyAsset()->Sort(ST_TEMP_FRIEND);
		pkGamePlayer->GetSocietyAsset()->Sort(ST_FRIEND);
	}
}

void HL_FromGameServer::_handlerRltSingleFriendInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_Single_Friend_Info kMsg;
	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "获取社会信息动态同步协议解析错误 MG_RLT_GetFriendList");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( !pkGamePlayer )
	{
		return;
	}
	kMsg.m_kFriendInfo.kBasicInfo.uiCharID = kMsg.m_uiCharID;
	pkGamePlayer->GetSocietyAsset()->UpdateFriend(kMsg.m_kFriendInfo);
	pkGamePlayer->GetSocietyAsset()->Sort(ST_FRIEND);
}

void HL_FromGameServer::_handlerRltRecommendFriend(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_Recommend_Friend rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_RLT_Recommend_Friend解析错误");
		return;
	}

	if(rltMsg.m_iResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(rltMsg.m_iResult);
	}

	StringStreamType sStream("");
	sStream << rltMsg.m_RecommendFriendInfos.size() << "@";

	std::vector<PS_PlayerBasicInfo>::const_iterator it = rltMsg.m_RecommendFriendInfos.begin();
	for (;it != rltMsg.m_RecommendFriendInfos.end(); ++it)
	{
		StringType sName = STDSTR2TPSTR(it->szCharName);
		sStream << it->uiCharID << "#" << it->iLevel << "#" << Int2String(it->iBornCharType) << "#" << it->iScore << "#" << sName << "|";
	}

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerMsgGameEvent(MGPT_RLT_FRIEND_RECOMMEND_FRIEND, sStream.str());
}

void HL_FromGameServer::_handlerRltAddAllRecommendFriends(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_AddRecommendFriend rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_RLT_AddFriend解析错误");
		return;
	}

	if(rltMsg.m_iResultType != RE_SUCCESS)
	{
		SendLogicInfoMessage(rltMsg.m_iResultType);
	}
}