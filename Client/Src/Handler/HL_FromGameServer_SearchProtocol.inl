#include "MG_CS_SearchPlayerProtocol.h"
#include "MG_GroupProtocol.h"
#include "CF_ForcesBattle.h"
#include "LC_ClientGuildWarAsset.h"
#include "CF_CrossRealmMap.h"
#include "LC_ClientGameStoryManager.h"
#include "MG_GameProtocol.h"
#include "LC_ClientNewBeeManager.h"
#include "MG_Game2Alliance.h"
#include "LC_ClientHelp.h"

void HL_FromGameServer::_handlerRltSearchPlayer(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_SearchPlayer kMsg;
	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "处理查询结果协议错误MG_RLT_SearchPlayer");
		return;
	}

	// 将查询结果保存起来
	LC_ClientGamePlayer* pkPlayer = pkSession->GetPlayer();
	if (NULL == pkPlayer)
	{
		return;	
	}

	pkPlayer->GetSearchPlayer()->Clear();

	if ( kMsg.m_currentArraySize > 0)
	{
		for (int i = 0; i < kMsg.m_currentArraySize; ++i)
		{
			pkPlayer->GetSearchPlayer()->Add(kMsg.kPlayerList[i]);
		}
	}
	else
	{
#pragma TODO("注意注意")
		//SendLogicInfoMessage(RE_SEARCH_FAILED_ZERO);
	}
}

void HL_FromGameServer::_handlerRltSearchPlayerDetailInfoTitle(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_SearchPlayer_DetailInfoTitle kMsg;

	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "处理查询详细信息协议错误MG_RLT_SearchPlayer_DetailInfoTitle");
		return;
	}

	LC_ClientGamePlayer* pkPlayer = pkSession->GetPlayer();
	if (NULL == pkPlayer)
	{
		return;	
	}
	LC_ClientViewPlayerInfo* pkViewPlayerInfo = pkPlayer->GetViewPlayerInfo();
	//if (NULL == pkViewPlayerInfo)
	//{
	//	return;
	//}
	//观察的目标不一致
	if (kMsg.m_lTargetCharID != pkViewPlayerInfo->GetCharID())
	{
		return;
	}
	LC_TitleAsset* pkTitleAsset = pkViewPlayerInfo->GetTitleAsset();
	//if (NULL == pkTitleAsset)
	//{
	//	return;
	//}

	if (kMsg.m_iResult == RE_SUCCESS)
	{
		int iCurrentTitleType = kMsg.m_kTitleNameInfo.m_iCurrentTitleType;
		int iCurrentTitleIndex = kMsg.m_kTitleNameInfo.m_iCurrentTitleID;
		pkTitleAsset->SetCurrentTitle(iCurrentTitleType, iCurrentTitleIndex);

		LC_TitleAsset::SystemTitleIDList& rkSysTitleIDList = pkTitleAsset->GetSystemTitleList();
		rkSysTitleIDList.clear();
		std::set<unsigned int>::iterator it = kMsg.m_kTitleNameInfo.m_iSystemTitleName.begin();
		for(; it != kMsg.m_kTitleNameInfo.m_iSystemTitleName.end(); ++it)
			rkSysTitleIDList.insert((*it));
	}
	else
	{
		SendLogicInfoMessage(kMsg.m_iResult);
	}
}

void HL_FromGameServer::_handlerRltSearchPlayerDetailInfoArena(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_SearchPlayer_DetailInfoArena kMsg;

	if ( !kMsg.Decode(pMsg, wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "处理查询详细信息协议错误MG_RLT_SearchPlayer_DetailInfoArena");
		return;
	}

	LC_ClientGamePlayer* pkPlayer = pkSession->GetPlayer();
	if (NULL == pkPlayer)
	{
		return;	
	}
	LC_ClientViewPlayerInfo* pkViewPlayerInfo = pkPlayer->GetViewPlayerInfo();
	//if (NULL == pkViewPlayerInfo)
	//{
	//	return;
	//}
	//观察的目标不一致
	if (kMsg.m_lTargetCharID != pkViewPlayerInfo->GetCharID())
	{
		return;
	}
	LC_ArenaAsset* pkArenaAsset = pkViewPlayerInfo->GetArenaAsset();
	//if (NULL == pkArenaAsset)
	//{
	//	return;
	//}

	if (kMsg.m_iResult == RE_SUCCESS)
	{
		pkArenaAsset->GetDataFromProtocolStruct(kMsg.m_arenaInfo);
	}
	else
	{
		SendLogicInfoMessage(kMsg.m_iResult);
	}
}

void HL_FromGameServer::_handleRltPlayerDetail(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RESP_GetPlayerInfo rltMsg;
	if (!rltMsg.Decode(pMsg, wSize))
	{
		return;
	}

	LC_ClientGamePlayer* pkPlayer = pkSession->GetPlayer();
	if (NULL == pkPlayer)
	{
		return;
	}

	if(rltMsg.m_SystemType == PIS_NORMAL)
	{
		pkPlayer->GetPlayerDetailMgrPtr()->UpdatePlayerInfo(rltMsg);
	}
	else
	{
		//LC_GameEvent kGameEvent;
		StringStreamType sStream("");
		sStream <<rltMsg.m_SystemType<<"|"<<rltMsg.m_nUserInfo.m_CharID<<"|"<<rltMsg.m_nUserInfo.m_sCharName<<"|"<< rltMsg.m_nUserInfo.m_nControllerCombatScore<< "|" <<rltMsg.m_nUserInfo.m_nCommanderTitleID<<"|"<<rltMsg.m_nUserInfo.m_nCommanderTitleFrameID<<"|"<<
			rltMsg.m_nUserInfo.m_nBornType<<"|"<<rltMsg.m_nUserInfo.m_nLevel<<"|"<<rltMsg.m_nUserInfo.m_sGuildName<<"|";
		LC_ClientGameEvent kGameEvent;
		//kGameEvent.SetClientEventType(CGET_SYNC_REWARD);
		LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(sStream.str());
		kGameEvent.SetUserData(&kUserData);

		kGameEvent.SetClientEventType(GLET_RESP_GETCONTROLLERBASEDATA);
		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
}

void HL_FromGameServer::_handlerRltControllerAndCoreCombatScore(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RLT_ControllerAndCoreCombatScore rltMsg;
	if (!rltMsg.Decode(pMsg, wSize))
	{
		return;
	}

	LC_ClientGamePlayer* pkPlayer = pkSession->GetPlayer();
	pkPlayer->SetBuddyCombatComposition(rltMsg.m_nBuddyID, rltMsg.m_nControllerCombatScore, rltMsg.m_nCoreCombatScore);

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_BUDDY_COMBACOMPOSITION);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, rltMsg.m_nBuddyID);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerRltPlayerPKInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RLT_PlayerPkInfo rltMsg;
	if (!rltMsg.Decode(pMsg, wSize))
	{
		return;
	}

	LC_ClientGamePlayer* pkPlayer = pkSession->GetPlayer();
	pkPlayer->SetPlayerPKDetailInfo(rltMsg);

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_PLAYER_COMBATCOMPARE);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, rltMsg.m_nOtherInfo.m_CharID);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handleMsgToLua(uint16_t iMsgType, NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	LC_ClientGamePlayer* pkPlayer = pkSession->GetPlayer();
	if (NULL == pkPlayer)
	{
		return;	
	}

	StringStreamType sStream("");

	switch (iMsgType)
	{
	case MGPT_NOTIFY_POWERGUILD_INFO:
		{
			MG_WarInfo rltMsg;
			if (!rltMsg.Decode(pMsg, wSize))
				return;

			//武林盟主产生
			if (rltMsg.m_PowerGenerate && !rltMsg.m_IsActive && rltMsg.m_PowerGuildId != 0)
			{
				g_soundMgr.Play2DEx(VARIABLE2STRING(SE_Sys_GVG_Champion));
			}
			//抢到圣火令
			if(pkPlayer->GetPowerGuildID() == 0 && rltMsg.m_PowerGuildId != 0 && rltMsg.m_IsActive)
			{
				g_soundMgr.Play2DEx(VARIABLE2STRING(SE_Sys_GVG_Token_Get));
			}
			//圣火令归位
			if(pkPlayer->GetPowerGuildID() != 0 && rltMsg.m_PowerGuildId == 0 && rltMsg.m_IsActive)
			{
				g_soundMgr.Play2DEx(VARIABLE2STRING(SE_Sys_GVG_Token_Return));
			}


			sStream << rltMsg.m_PowerGenerate;
			sStream << "#";
			sStream << LC_ClientHelp::FilterGBKPlayerName(rltMsg.m_PowerGuildPlayerName);
			sStream << "#";

			if (rltMsg.m_IsActive || (rltMsg.m_WarType != WarType_DomainWar))
			{
				sStream << rltMsg.m_PowerGuildX;
				sStream << "#";
				sStream << rltMsg.m_PowerGuildY;
				sStream << "#";

				pkPlayer->SetGuildWarTargetPos(rltMsg.m_PowerGuildX, rltMsg.m_PowerGuildY);
			}
			else
			{
				CF_ForcesBattle::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_ForcesBattle>(pkPlayer->GetMapResID());
				if (NULL != pkData)
				{
					sStream << pkData->_Pos.x;
					sStream << "#";
					sStream << pkData->_Pos.y;
					sStream << "#";

					pkPlayer->SetGuildWarTargetPos(pkData->_Pos.x, pkData->_Pos.y);
				}
				else
				{
					sStream << rltMsg.m_PowerGuildX;
					sStream << "#";
					sStream << rltMsg.m_PowerGuildY;
					sStream << "#";

					pkPlayer->SetGuildWarTargetPos(rltMsg.m_PowerGuildX, rltMsg.m_PowerGuildY);
				}
			}

			sStream << rltMsg.m_PowerGuildTime;
			sStream << "#";
			sStream << rltMsg.m_PowerChar;
			sStream << "#";
			sStream << rltMsg.m_PowerGuildId;
			sStream << "#";
			sStream << LC_ClientHelp::FilterGBKPlayerName(rltMsg.m_PowerGuildName);
			sStream << "#";
			sStream << rltMsg.m_WarType << "#";
			sStream << rltMsg.m_IsActive << "#";

			//if (rltMsg.m_PowerGenerate)
			//{
			//	pkPlayer->SetPowerGuildID(0);
			//	pkPlayer->SetPowerPlayerID(0);
			//}
			//else
			{
				pkPlayer->SetPowerPlayer(rltMsg.m_PowerChar);
				pkPlayer->SetPowerGuildID(rltMsg.m_PowerGuildId);

				if (rltMsg.m_IsActive)
					pkPlayer->SetGuildWarType(rltMsg.m_WarType);
				else
				{
					pkPlayer->SetGuildWarType(WarType_None);
					pkPlayer->SetPowerPlayer(0);
					pkPlayer->SetPowerGuildID(-1);
				}
			}
		}
		break;
	case MGPT_NOTIFY_GUILDPLAYER_INFO:
		{
			MG_WarPlayerInfo rltMsg;
			if (!rltMsg.Decode(pMsg, wSize))
				return;
			
			sStream << rltMsg.m_PowerGuildX;
			sStream << "#";
			sStream << rltMsg.m_PowerGuildY;
			sStream << "#";
			sStream << rltMsg.m_PowerChar;

			pkPlayer->SetGuildWarTargetPos(rltMsg.m_PowerGuildX, rltMsg.m_PowerGuildY);
		}
		break;
	case MG_RLT_GET_FD_FREE_REWARD:
		{
			MG_RLT_GetFDFreeReward rltMsg;
			if (!rltMsg.Decode(pMsg, wSize))
				return;

			if (rltMsg.m_nResult != RE_SUCCESS)
			{
				SendLogicInfoMessage(rltMsg.m_nResult);
			}

		}
		break;
	case MG_RLT_BUY_GROWTH_PACKAGE:
		{
			MG_RLT_BuyGrowthPackage rltMsg;
			if (!rltMsg.Decode(pMsg, wSize))
				return ;
			
			if (rltMsg.m_nResult != RE_SUCCESS)
			{
				SendLogicInfoMessage(rltMsg.m_nResult);
			}

		}
		break;
	case MG_RLT_GET_GROWTH_REWARD:
		{
			MG_RLT_BuyGrowthPackage rltMsg;
			if (!rltMsg.Decode(pMsg, wSize))
				return ;

			if (rltMsg.m_nResult != RE_SUCCESS)
			{
				SendLogicInfoMessage(rltMsg.m_nResult);
			}

		}
		break;
	case MG_RLT_GET_FD_DISCOUNT_REWARD:
		{
			MG_RLT_GetFDDisCountReward rltMsg;
			if (!rltMsg.Decode(pMsg, wSize))
				return;

			if (rltMsg.m_nResult != RE_SUCCESS)
			{
				SendLogicInfoMessage(rltMsg.m_nResult);
			}

		}
		break;
	case MGPT_RESP_GUILDWAR_CATAPULT_FIRE:
		{
			MG_GuildWar_FireCatapult_Resp rltMsg;
			if (!rltMsg.Decode(pMsg, wSize))
				return;
			
			if(rltMsg.m_Result != RE_SUCCESS)
			{
				SendLogicInfoMessage(rltMsg.m_Result, rltMsg.m_Cost/10000);		
				return;
			}

			sStream << rltMsg.m_CruiseTime;
			sStream << "#";
			sStream << rltMsg.m_CatapultId;
			sStream << "#";
			sStream << rltMsg.m_TargetX;
			sStream << "#";
			sStream << rltMsg.m_TargetY;
		}
		break;
	case MGPT_CROSSREALM_MAPLIST_RESP:
		{
			MG_RLT_CrossRealm_MapList rltMsg;
			if(!rltMsg.Decode(pMsg, wSize))
				return;

			for (int i=0; i<rltMsg.m_Size; ++i)
			{
				PS_CrossRealmTeamMap kMap = rltMsg.m_AllMap[i];
				sStream << kMap.m_MapTypeId << "-";
				for(int j=0; j<kMap.m_Size; ++j)
				{
					PS_PassPlayerRecord kRecord = kMap.m_PassRecord[j];
					sStream << LC_ClientHelp::FilterGBKPlayerName(kRecord.m_PlayerName) << "_";
					sStream << kRecord.m_ServerId << "_";
					sStream << kRecord.m_ServerName << "_";
					sStream << kRecord.m_Time << "|";
				}
				sStream << "#";
			}
		}
		break;
	case MGPT_CROSSREALM_TEAMLIST_RESP:
		{
			MG_RESP_CrossRealm_TeamList rltMsg;
			if(!rltMsg.Decode(pMsg, wSize))
				return;

			sStream << rltMsg.m_Size << "#";
			sStream << rltMsg.m_MapTypeId << "#";
			for(int i=0; i<rltMsg.m_Size; ++i)
			{
				PS_TeamAbstract kTeam = rltMsg.m_AllTeam[i];
				sStream << kTeam.m_MasterId << "_";
				sStream << kTeam.m_PlayNum << "_";
				sStream << kTeam.m_ServerId << "_";
				sStream << kTeam.m_ServerName << "_";
				sStream << kTeam.m_TeamId << "_";
				sStream << kTeam.m_TeamName << "_";
				sStream << kTeam.m_TeamState << "_";
				sStream << kTeam.m_TimeStamp << "_";
				sStream << (int)kTeam.m_AutoStart << "_";
				sStream << kTeam.m_TotalScore << "|";
			}
		}
		break;
	case MGPT_CROSSREALM_CREATETEAM_RESP:
		{
			MG_RLT_CrossRealm_CreateTeam rltMsg;
			if(!rltMsg.Decode(pMsg, wSize))
				return;

			if(rltMsg.m_Result != RE_SUCCESS)
			{
				if (rltMsg.m_Result == RE_CROSSREALMMAP_TIME_OUT)
				{
					StringType sMapName("");
					CF_CrossRealmMap::DataEntry *pkDataEntry = SafeGetCSVFileDataEntryPtr<CF_CrossRealmMap>(rltMsg.m_MapTypeId);
					if (pkDataEntry)
					{
						sMapName = pkDataEntry->_kName;
					}
					SendLogicInfoMessage(rltMsg.m_Result, sMapName.c_str());	
				}
				else
				{
					SendLogicInfoMessage(rltMsg.m_Result);		
				}
				return;
			}
			sStream << rltMsg.m_MapTypeId << "#";
			sStream << rltMsg.m_TeamId << "#";
			
			PS_TeamDetail kTeamDetail = rltMsg.m_TeamDetail;
			sStream << kTeamDetail.m_TeamName << "-";
			sStream << kTeamDetail.m_TeamState << "-";
			sStream << kTeamDetail.m_ServerId << "-";
			sStream << kTeamDetail.m_ServerName << "-";
			sStream << kTeamDetail.m_Master << "-";
			sStream << kTeamDetail.m_TimeStamp << "-";
			sStream << kTeamDetail.m_Size << "-";
			sStream << (int)kTeamDetail.m_AutoStart << "-";
			sStream << kTeamDetail.m_TotalScore << "#";

			for(int i=0; i<kTeamDetail.m_Size; ++i)
			{
				PS_TeamPlayerAbstract kTeamPlayer = kTeamDetail.m_AllPlayer[i];
				sStream << kTeamPlayer.m_CharID << "_";
				sStream << LC_ClientHelp::FilterGBKPlayerName(kTeamPlayer.m_PlayerName) << "_";
				sStream << kTeamPlayer.m_Level << "_";
				sStream << kTeamPlayer.m_ServerId << "_";
				sStream << kTeamPlayer.m_ServerName << "_";
				sStream << kTeamPlayer.m_TotalScore << "_";
				sStream << kTeamPlayer.m_CharType << "_";
				sStream << kTeamPlayer.m_HeadEquip << "_";
				sStream << kTeamPlayer.m_BodyEquip << "_";
				sStream << (int)kTeamPlayer.m_Ready << "_";
				sStream << kTeamPlayer.m_WeaponEquip <<"|";
			}
		}
		break;
	case MGPT_CROSSREALM_ADD2TEAM_RESP:
		{
			MG_RESP_CrossRealm_Add2Team rltMsg;
			if(!rltMsg.Decode(pMsg,wSize))
				return;
			
			if(rltMsg.m_Result != RE_SUCCESS)
			{
				if (rltMsg.m_Result == RE_CROSSREALMMAP_TIME_OUT)
				{
					StringType sMapName("");
					CF_CrossRealmMap::DataEntry *pkDataEntry = SafeGetCSVFileDataEntryPtr<CF_CrossRealmMap>(rltMsg.m_MapTypeId);
					if (pkDataEntry)
					{
						sMapName = pkDataEntry->_kName;
					}
					SendLogicInfoMessage(rltMsg.m_Result, sMapName.c_str());	
				}
				else
				{
					SendLogicInfoMessage(rltMsg.m_Result);		
				}
				return;
			}
			sStream << rltMsg.m_MapTypeId << "#";
			sStream << rltMsg.m_TeamId << "#";

			PS_TeamDetail kTeamDetail = rltMsg.m_TeamDetail;
			sStream << kTeamDetail.m_TeamName << "-";
			sStream << kTeamDetail.m_TeamState << "-";
			sStream << kTeamDetail.m_ServerId << "-";
			sStream << kTeamDetail.m_ServerName << "-";
			sStream << kTeamDetail.m_Master << "-";
			sStream << kTeamDetail.m_TimeStamp << "-";
			sStream << kTeamDetail.m_Size << "-";
			sStream << (int)kTeamDetail.m_AutoStart << "-";
			sStream << kTeamDetail.m_TotalScore << "#";

			for(int i=0; i<kTeamDetail.m_Size; ++i)
			{
				PS_TeamPlayerAbstract kTeamPlayer = kTeamDetail.m_AllPlayer[i];
				sStream << kTeamPlayer.m_CharID << "_";
				sStream << LC_ClientHelp::FilterGBKPlayerName(kTeamPlayer.m_PlayerName) << "_";
				sStream << kTeamPlayer.m_Level << "_";
				sStream << kTeamPlayer.m_ServerId << "_";
				sStream << kTeamPlayer.m_ServerName << "_";
				sStream << kTeamPlayer.m_TotalScore << "_";
				sStream << kTeamPlayer.m_CharType << "_";
				sStream << kTeamPlayer.m_HeadEquip << "_";
				sStream << kTeamPlayer.m_BodyEquip << "_";
				sStream << (int)kTeamPlayer.m_Ready << "_";
				sStream << kTeamPlayer.m_WeaponEquip <<"|";
			}
		}
		break;
	case MGPT_NOTIFY_CROSSREALM_INVITE:
		{
			MG_Notify_Invite_CrossRealmTeam rltMsg;
			if(!rltMsg.Decode(pMsg, wSize))
				return;
			StringType playername = STDSTR2TPSTR(rltMsg.m_playerName);
			sStream << rltMsg.m_iMapTypeID << "_";
			sStream << rltMsg.m_iTeamID << "_";
			sStream << playername << "_";
			sStream << rltMsg.m_iInviteCharID << "_";

		}
		break;
	case MGPT_CROSSREALM_INVITEPLAYER_RESP:
		{
			MG_RESP_InvitePlayer_CrossRealmTeam rltMsg;
			if(!rltMsg.Decode(pMsg, wSize))
				return;
			if(rltMsg.m_Result != RE_SUCCESS)
			{
				SendLogicInfoMessage(rltMsg.m_Result);
				return;
			}
			StringType playername = STDSTR2TPSTR(rltMsg.m_iplayerName);
			sStream << rltMsg.m_iBeInviteCharID << "_";
			sStream << rltMsg.m_Result << "_";
			sStream << playername << "_";
		}
		break;
	case MGPT_RLT_GET_BUY_TIMES:
		{
			MG_RLT_GetBuyTimes rltMsg;
			if(!rltMsg.Decode(pMsg, wSize))
				return;
			if(rltMsg.m_iResult != RE_SUCCESS)
			{
				SendLogicInfoMessage(rltMsg.m_iResult);
				return;
			}

			sStream << rltMsg.m_iGameStoryId << "_";
			sStream << rltMsg.m_buyTimes << "_";
			sStream << rltMsg.m_totalTimes << "_";
			sStream << rltMsg.m_costType << "_";
			sStream << rltMsg.m_costCount << "_";
			sStream << rltMsg.m_type << "_";
			pkPlayer->GetGameStoryManager()->ReceiveDataFormServer(rltMsg);
			pkPlayer->GetGameStoryManager()->AddGameStoryRevisionID(ACTIVITY_DIRTY_FLAG_UPDATE_LIST);
		}
		break;
	case MGPT_NOTIFY_CROSSREALM_REFUSE_INVITE:
		{
			MG_Notify_CrossRealm_Refush_Invate rltMsg;
			if(!rltMsg.Decode(pMsg, wSize))
				return;
			StringType playername = STDSTR2TPSTR(rltMsg.m_iplayerName);
			sStream << rltMsg.m_iBeInviteCharID << "_";
			sStream << playername << "_";
		}
		break;
	case MGPT_NOTIFY_CHALLENGE_ENDLESSTOWERSTATE:
		{
			MG_Notify_ChallengeEndLessTowerState rltMsg;
			if(!rltMsg.Decode(pMsg, wSize))
				return;
			sStream << Int2String(rltMsg.m_iStoryID) << "_";
			sStream << Int2String(rltMsg.m_bSuccess) << "_";
			sStream << Int2String(rltMsg.m_iStat);
		}
		break;
	case MGPT_INVITE_ALLPLAYER_CROSSREALMTEAM_RESP:
		{
			MG_RLT_InviteAllPlayer_CrossRealmTeam rltMsg;
			if(!rltMsg.Decode(pMsg, wSize))
				return;
			if(rltMsg.m_Result != RE_SUCCESS)
			{
				SendLogicInfoMessage(rltMsg.m_Result);		
				return;
			}

		}
		break;
	case MGPT_CROSSREALM_LEAVETEAM_RESP:
		{
			MG_RESP_CrossRealm_LeaveTeam rltMsg;
			if(!rltMsg.Decode(pMsg, wSize))
				return;

			if (rltMsg.m_Result == RE_SUCCESS)
			{
				LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
				if (NULL == pkLogicManager)
				{
					return;
				}
				LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
				if (NULL == pkGamePlayer)
				{
					return;
				}
				pkGamePlayer->SetCrossRealMapTeamID(0);
				pkGamePlayer->SetCrossRealTeamMapID(0);
			}
			else
			{
				SendLogicInfoMessage(rltMsg.m_Result);		
				return;
			}

		}
		break;
	case MGPT_CROSSREALM_TEAMINFOEDIT_RESP:
		{
			MG_RESP_CrossRealm_TeamInfoEdit rltMsg;
			if(!rltMsg.Decode(pMsg, wSize))
				return;

			if(rltMsg.m_Result != RE_SUCCESS)
			{
				SendLogicInfoMessage(rltMsg.m_Result);		
				return;
			}

			/*sStream << rltMsg.m_MapTypeId << "#";
			sStream << rltMsg.m_TeamId << "#";*/
		}
		break;
	case MGPT_CROSSREALM_KICKMEMBER_RESP:
		{
			MG_RESP_CrossRealm_Kick_Member rltMsg;
			if(!rltMsg.Decode(pMsg, wSize))
				return;

			if(rltMsg.m_Result != RE_SUCCESS)
			{
				SendLogicInfoMessage(rltMsg.m_Result);
				return;
			}
		}
		break;
	case MGPT_CROSSREALM_READYMEMBER_RESP:
		{
			MG_RESP_CrossRealm_Ready_Member rltMsg;
			if(!rltMsg.Decode(pMsg, wSize))
				return;

			if(rltMsg.m_Result != RE_SUCCESS)
			{
				SendLogicInfoMessage(rltMsg.m_Result);
				return;
			}
		}
		break;
	case MGPT_CROSSREALM_AUTOADD_RESP:
		{
			MG_RESP_CrossRealm_AutoAdd_Team rltMsg;
			if(!rltMsg.Decode(pMsg, wSize))
				return;

			if(rltMsg.m_Result != RE_SUCCESS)
			{
				if (rltMsg.m_Result == RE_CROSSREALMMAP_TIME_OUT)
				{
					StringType sMapName("");
					CF_CrossRealmMap::DataEntry *pkDataEntry = SafeGetCSVFileDataEntryPtr<CF_CrossRealmMap>(rltMsg.m_MapTypeId);
					if (pkDataEntry)
					{
						sMapName = pkDataEntry->_kName;
					}
					SendLogicInfoMessage(rltMsg.m_Result, sMapName.c_str());	
				}
				else
				{
					SendLogicInfoMessage(rltMsg.m_Result);		
				}
				return;
			}
		}
		break;
	case MGPT_CROSSREALM_ADD2TEAM_NOTIFY:
		{
			MG_NOTIFY_CrossRealm_Add2Team rltMsg;
			if(!rltMsg.Decode(pMsg, wSize))
				return;

			sStream << rltMsg.m_MapTypeId << "#";
			sStream << rltMsg.m_TeamId << "#";
			for(int i=0; i<rltMsg.m_Size; ++i)
			{
				PS_TeamPlayerAbstract kTeamPlayer = rltMsg.m_PlayerInfo[i];
				sStream << kTeamPlayer.m_CharID << "_";
				sStream << LC_ClientHelp::FilterGBKPlayerName(kTeamPlayer.m_PlayerName) << "_";
				sStream << kTeamPlayer.m_Level << "_";
				sStream << kTeamPlayer.m_ServerId << "_";
				sStream << kTeamPlayer.m_ServerName << "_";
				sStream << kTeamPlayer.m_TotalScore << "_";
				sStream << kTeamPlayer.m_CharType << "_";
				sStream << kTeamPlayer.m_HeadEquip << "_";
				sStream << kTeamPlayer.m_BodyEquip << "_";
				sStream << (int)kTeamPlayer.m_Ready << "_";
				sStream << kTeamPlayer.m_WeaponEquip <<"|";
			}
		}
		break;
	case MGPT_CROSSREALM_PLAYINFOCHANGE_NOTIFY:
		{
			MG_NOTIFY_CrossRealm_PlayInfoChange rltMsg;
			if(!rltMsg.Decode(pMsg, wSize))
				return;

			sStream << rltMsg.m_MapTypeId << "#";
			sStream << rltMsg.m_TeamId << "#";

			PS_TeamPlayerAbstract kTeamPlayer = rltMsg.m_PlayerInfo;
			sStream << kTeamPlayer.m_CharID << "_";
			sStream << LC_ClientHelp::FilterGBKPlayerName(kTeamPlayer.m_PlayerName) << "_";
			sStream << kTeamPlayer.m_Level << "_";
			sStream << kTeamPlayer.m_ServerId << "_";
			sStream << kTeamPlayer.m_ServerName << "_";
			sStream << kTeamPlayer.m_TotalScore << "_";
			sStream << kTeamPlayer.m_CharType << "_";
			sStream << kTeamPlayer.m_HeadEquip << "_";
			sStream << kTeamPlayer.m_BodyEquip << "_";
			sStream << (int)kTeamPlayer.m_Ready << "_";
			sStream << kTeamPlayer.m_WeaponEquip;
		}
		break;
	case MGPT_CROSSREALM_TEAMINFOCHANGE_NOTIFY:
		{
			MG_NOTIFY_CrossRealm_TeamInfoChange rltMsg;
			if(!rltMsg.Decode(pMsg, wSize))
				return;

			sStream << rltMsg.m_MapTypeId << "#";
			PS_TeamAbstract kTeam = rltMsg.m_TeamInfo;
			sStream << kTeam.m_MasterId << "_";
			sStream << kTeam.m_PlayNum << "_";
			sStream << kTeam.m_ServerId << "_";
			sStream << kTeam.m_ServerName << "_";
			sStream << kTeam.m_TeamId << "_";
			sStream << kTeam.m_TeamName << "_";
			sStream << kTeam.m_TeamState << "_";
			sStream << kTeam.m_TimeStamp << "_";
			sStream << (int)kTeam.m_AutoStart << "_";
			sStream << kTeam.m_TotalScore;
		}
		break;
	case MGPT_CROSSREALM_LEAVETEAM_NOTIFY:
		{
			MG_Notify_CrossRealm_LeaveTeam rltMsg;
			if(!rltMsg.Decode(pMsg, wSize))
				return;

			sStream << rltMsg.m_MapTypeId << "#";
			sStream << rltMsg.m_TeamId << "#";
			sStream << rltMsg.m_CharID << "#";
			sStream << (int)rltMsg.m_beKicked;
		}
		break;
	case MGPT_CROSSREALM_BEKICK_NOTIFY:
		{
			MG_Notify_CrossRealm_BeKick rltMsg;
			if(!rltMsg.Decode(pMsg, wSize))
				return;
		}
		break;
	case MGPT_CROSSREALM_ENTER_RESP:
		{
			MG_RESP_CrossRealm_Enter rltMsg;
			if(!rltMsg.Decode(pMsg, wSize))
				return;
		}
		break;
	case MGPT_CROSSREALM_ENTER_NOTIFY:
		{
			MG_Notify_Team_ReadyEnter rltMsg;
			if(!rltMsg.Decode(pMsg, wSize))
				return;

			sStream << rltMsg.m_MapTypeId << "#";
			sStream << rltMsg.m_TeamId << "#";
			sStream << rltMsg.m_DelayTime << "#";
		}
		break;
	case MGPT_NOTIFY_POWERGUILDEMENY_INFO:
		{
			MG_WarEmenyPlayerInfo rltMsg;
			if (!rltMsg.Decode(pMsg, wSize))
				return;

			LC_MiniMap*pkMiniMap = LC_MapManager::GetSingletonPtr()->GetMiniMap();
			float fPosX;
			float fPosY;
			
			bool bHasValue = false;
			for (int i = 0; i < rltMsg.m_Size; ++i)
			{
				if (pkPlayer->GetPowerPlayer() == rltMsg.m_PlayerInfo[i].m_CharID)
					continue;
				pkMiniMap->CalcWorldPosToMap( Int2Float(rltMsg.m_PlayerInfo[i].m_PowerGuildX), Int2Float(rltMsg.m_PlayerInfo[i].m_PowerGuildY), fPosY, fPosX);

				if (bHasValue)
					sStream << "#";
				sStream << fPosX << "_" << fPosY;
				bHasValue = true;
			}
		}
		break;
	case MGPT_CROSSREALM_PLAYER_TEAMINFO_RESP:
		{
			MG_RLT_PlayerTeamDetail rltMsg;
			if(!rltMsg.Decode(pMsg, wSize))
				return;

			if(rltMsg.m_Result != RE_SUCCESS)
			{
				SendLogicInfoMessage(rltMsg.m_Result);		
				return;
			}

			sStream << rltMsg.m_MapTypeId << "#";
			sStream << rltMsg.m_TeamId << "#";

			PS_TeamDetail kTeamDetail = rltMsg.m_TeamDetail;
			sStream << kTeamDetail.m_TeamName << "-";
			sStream << kTeamDetail.m_TeamState << "-";
			sStream << kTeamDetail.m_ServerId << "-";
			sStream << kTeamDetail.m_ServerName << "-";
			sStream << kTeamDetail.m_Master << "-";
			sStream << kTeamDetail.m_TimeStamp << "-";
			sStream << kTeamDetail.m_Size << "-";
			sStream << (int)kTeamDetail.m_AutoStart << "-";
			sStream << kTeamDetail.m_TotalScore << "#";

			for(int i=0; i<kTeamDetail.m_Size; ++i)
			{
				PS_TeamPlayerAbstract kTeamPlayer = kTeamDetail.m_AllPlayer[i];
				sStream << kTeamPlayer.m_CharID << "_";
				sStream << LC_ClientHelp::FilterGBKPlayerName(kTeamPlayer.m_PlayerName) << "_";
				sStream << kTeamPlayer.m_Level << "_";
				sStream << kTeamPlayer.m_ServerId << "_";
				sStream << kTeamPlayer.m_ServerName << "_";
				sStream << kTeamPlayer.m_TotalScore << "_";
				sStream << kTeamPlayer.m_CharType << "_";
				sStream << kTeamPlayer.m_HeadEquip << "_";
				sStream << kTeamPlayer.m_BodyEquip << "_";
				sStream << (int)kTeamPlayer.m_Ready << "_";
				sStream << kTeamPlayer.m_WeaponEquip <<"|";
			}
		}
		break;
	case MGPT_NOTIFY_WAR_SETTLE:
		{
			MG_Notify_WarSettle rltMsg;
			if (!rltMsg.Decode(pMsg, wSize))
				return;

			sStream << rltMsg.m_WarType << "#";
			sStream << (rltMsg.m_result == true ? 1 : 0) << "#";
			sStream << rltMsg.m_exp << "#";
			sStream << rltMsg.m_money << "#";
			sStream << rltMsg.m_sp << "#";
			sStream << rltMsg.m_guildmoney << "#";
			std::map<uint32_t,uint32_t>::iterator it = rltMsg.m_itemMap.begin();
			for(;it!=rltMsg.m_itemMap.end();++it)
			{
				sStream << it->first<<"-"<<it->second<<"|";
			}
			sStream<<"#";
		}
		break;
	case MGPT_GAMESTORY_KEY_ECOUNTED_STAT_NOTIFY:
		{
			MG_GAMESTORY_ACTOR_ENCOUNTED_STAT_NOTIFY rltMsg;
			if(!rltMsg.Decode(pMsg, wSize))
				return;
			/*LC_ClientGameStoryManager* pkGameStoryManager = CLIENT_GET_GAMESTORY_MANAGER();
			if (pkGameStoryManager)
			{
				pkGameStoryManager->SetTargetPos(rltMsg.m_iLocX,rltMsg.m_iLocY,rltMsg.m_iStroyID);
			}*/
			sStream << 0 << "#";
			sStream << rltMsg.m_uiID << "#";
			sStream << rltMsg.m_uiMapResID << "#";
			sStream << rltMsg.m_iLocX << "#";
			sStream << rltMsg.m_iLocY << "#";
			sStream << 0 << "#";
			sStream << rltMsg.m_iCurHP << "#";
			sStream << rltMsg.m_iMaxHP << "#";
			sStream << "|";
			StringStreamType PlayerStream;
			StringStreamType ServerStream;

			for (std::vector<PS_ActorEcountedStat>::iterator it = rltMsg.m_kStats.begin(); it != rltMsg.m_kStats.end(); ++it)
			{
				StringStreamType *RefStr = &PlayerStream;
				switch (it->m_uiType)
				{
				case LC_ECT_CAT_ACTOR:
					RefStr = &PlayerStream;
					break;
				case LC_ECT_CAT_SERVER:
					RefStr = &ServerStream;
					break;
				default:
					continue;
				}
				std::vector<PS_RankActorDesc>::iterator tmpit = it->m_kStats.begin();
				for (; tmpit != it->m_kStats.end(); ++tmpit)
				{
					*RefStr << (*tmpit).m_uiCharID << "#";
					*RefStr << (*tmpit).m_uiRank + 1 << "#";
					*RefStr << (*tmpit).m_szCharName << "#";
					*RefStr << (*tmpit).m_iStat << "#";
				}
			}
			sStream << PlayerStream.str() << "|" << ServerStream.str() << "|";
		}
		break;
	case MGPT_RESP_TRANSFER_DETAIL:
		{
			MG_RESP_TransferDetail respMsg;
			if (!respMsg.Decode(pMsg,wSize))
				return;
			if (respMsg.m_kDetails.empty())
				return;
			int iMapResID = respMsg.m_kDetails[0].m_iResID;
			sStream << iMapResID << "#";
			sStream << respMsg.m_kDetails.size() << "#";
			/*int curLogicID = pkPlayer->GetMapLogicID();*/
			LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
			int curLogicID = pkGamePlayer->GetMapLogicID();
			NW_ClientSessionManager* pkSessionManager        = NW_ClientSessionManager::GetSingletonPtr();
			NW_ClientSessionGroup* pkSessionGroup                = pkSessionManager->GetGameSessionGroup();
			int nodeID = pkSessionGroup->GetCurGameServerNode();
			for(Protocol::MG_RESP_TransferDetail::PS_MapDetailList::iterator it = respMsg.m_kDetails.begin(); it != respMsg.m_kDetails.end(); ++it)
			{
				sStream << it->m_iLogicID << "|";
				sStream << it->m_iLineNum << "|";
				sStream << it->m_uiPlayerCount << "|";
				sStream << it->m_iGSNodeID << "#";
				if (curLogicID == it->m_iLogicID && nodeID == it->m_iGSNodeID)
				{
					pkPlayer->SetCurrentLineNum(it->m_iLineNum);
				}
			}
			GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"收到地图分线信息，mapID=%d, info=%s， curLogicID=%d, nodeID=%d",iMapResID, sStream.str().c_str(), curLogicID, nodeID);
			LC_ClientNewBeeManager *pkNewBeeManager = LC_ClientNewBeeManager::GetSingletonPtr();
			if (pkNewBeeManager)
			{
				pkNewBeeManager->GetDataFromProtocol(respMsg);
			}
		};
		break;
	case  MGPT_FALLIANCE_BOSS_INFO_RSP:
		{
			MG_Resp_Alliance_Boss_Info respMsg;
			if (!respMsg.Decode(pMsg,wSize))
			{
				GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"BOSS信息 MGPT_FALLIANCE_BOSS_INFO_RSP decode失败");
				return;
			}

			sStream << respMsg.m_rInfoList.size() << "#";
			for(Protocol::SL_Alliance_Boss_Info::iterator it = respMsg.m_rInfoList.begin(); it != respMsg.m_rInfoList.end(); ++it)
			{
				sStream << it->m_nServerID << "|";
				sStream << it->m_rBossList.size() << "|";
				for (std::list<Protocol::ST_Alliance_Boss_StateInfo>::iterator stateit = it->m_rBossList.begin(); stateit != it->m_rBossList.end(); ++stateit)
				{
					sStream << stateit->m_nNpcType << "|" << stateit->m_rLocation.m_iMapID << "|" << stateit->m_rLocation.m_iPosX << "|" << stateit->m_rLocation.m_iPosY << "|"<< stateit->m_nRefreshTime << "|"
							<< stateit->m_nState << "|" << LC_ClientHelp::FilterGBKPlayerName(stateit->m_killName) << "|" << stateit->m_nLineNum << "|" << stateit->m_nDisAppearTime << "|" << stateit->m_nPrepareTime << "|" <<stateit->m_killerID << "|";
				}
				sStream << "#";
				GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"BOSS信息 MGPT_FALLIANCE_BOSS_INFO_RSP，bosslistsize=%d",it->m_rBossList.size());
			}
		}
		_handleSyncBestMapItemInfo(pkSession,pMsg,wSize);
		break;
	case MGPT_FALLIANCE_BOSS_INFO_NOTICE:
	case MGPT_FALLIANCE_BOSS_INFO_NOTICE_NOTIFY:
		{
			MG_Notice_Alliance_Boss_Info respMsg;
			if (!respMsg.Decode(pMsg,wSize))
			{
				GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"BOSS信息 MGPT_FALLIANCE_BOSS_INFO_NOTICE decode失败");
				return;
			}

			sStream << "1#";
			sStream << respMsg.m_nBossInfo.m_nServerID << "|";
			sStream << respMsg.m_nBossInfo.m_rBossList.size() << "|";
			for (std::list<Protocol::ST_Alliance_Boss_StateInfo>::iterator stateit = respMsg.m_nBossInfo.m_rBossList.begin(); stateit != respMsg.m_nBossInfo.m_rBossList.end(); ++stateit)
			{
				sStream << stateit->m_nNpcType << "|" << stateit->m_rLocation.m_iMapID << "|" << stateit->m_rLocation.m_iPosX << "|" << stateit->m_rLocation.m_iPosY << "|"<< stateit->m_nRefreshTime << "|"
						<< stateit->m_nState << "|" << LC_ClientHelp::FilterGBKPlayerName(stateit->m_killName) << "|" << stateit->m_nLineNum << "|" << stateit->m_nDisAppearTime << "|" << stateit->m_nPrepareTime << "|"<<stateit->m_killerID << "|";
			}
			sStream << "#";

			GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"BOSS信息 MGPT_FALLIANCE_BOSS_INFO_NOTICE，bosslistsize=%d",respMsg.m_nBossInfo.m_rBossList.size());
			LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
			pkGamePlayer->UpdateBestGuildMapBossInfo(respMsg.m_nBossInfo);

			LC_GameEvent kGameEvent;
			kGameEvent.SetEventType(GLET_GUILDBATTLE_MAPINFO_CHANGE);

			UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
			pkUIManager->HandlerGameEvent(kGameEvent);
		}
		break;
	case MGPT_GAMESTORY_NOTIFY_FACTION_SETTLE:
		{
			MG_GAMESTORY_FACTION_WAR_SETTLE rltMsg;
			if (!rltMsg.Decode(pMsg,wSize))
				return;
			sStream << (rltMsg.m_bIsWin == true ? 1 : 0) << "#";
			sStream << rltMsg.m_iExp << "#";
			sStream << rltMsg.m_iMoney << "#";
			sStream << rltMsg.m_iSp << "#";
			std::map<uint32_t,uint32_t>::iterator it = rltMsg.m_kItemMap.begin();
			for(;it!=rltMsg.m_kItemMap.end();++it)
			{
				sStream << it->first<<"-"<<it->second<<"|";
			}
			sStream<<"#";
		}
		break;
	default:
		return;
	}

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerMsgGameEvent(iMsgType, sStream.str());
}

void HL_FromGameServer::_handleNotifyCrossRealmTeamFast(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	//MG_GG_Notify_CrossRealmTeamFast rltMsg;

}

//这里包含 地图上的怪物
void HL_FromGameServer::_handleSyncBestMapItemInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_Resp_Alliance_Boss_Info respMsg;
	if (!respMsg.Decode(pMsg,wSize))
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->UpdateBestGuildMapBossInfo(respMsg.m_rInfoList);

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_GUILDBATTLE_MAPINFO_CHANGE);

	UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}
