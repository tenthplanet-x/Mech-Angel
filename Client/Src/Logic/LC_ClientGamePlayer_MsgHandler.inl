
//---------------------------------------------------------------------------
void LC_ClientGamePlayer::SendRequestInfoByType(int iType, uint32_t lID, int iParam1, int iParam2, int iParam3)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	switch(iType)
	{
	case MGPT_REQ_RANK_QUERY_CHALLENGE_INFO:
		{
			/*GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"竞技场  发送请求MGPT_REQ_RANK_QUERY_CHALLENGE_INFO，iParam1=%d",iParam1);*/
			MG_REQ_UserChallengeInfo msg;
			/*msg.m_RankID = getRankIDByType(iParam1, LOCAL_SERVER);*/
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg);
		}
		break;
	case MGPT_REQ_BATTLE_INFO:
		{
			/*GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"竞技场  发送请求MGPT_REQ_BATTLE_INFO，iParam1=%d",iParam1);*/
			MG_REQ_BattleInfo msg;
			msg.m_RankID = getRankIDByType(iParam1, LOCAL_SERVER);
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg);
		}
		break;
	case MGPT_REQ_ADD_CHALLENGE_TIME:
		{
			MG_REQ_AddChallengeTimes msg;
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg);
		}
		break;
	case MGPT_REQ_CLEAR_CHALLENGE_CD:
		{
			MG_REQ_ClearChallengeCD msg;
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg);
		}
		break;
	case MGPT_REQ_GET_CHALLENGE_CD:
		{
			/*GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"竞技场  发送请求MGPT_REQ_GET_CHALLENGE_CD");*/
			MG_REQ_GetChallengeCD msg;
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg);
		}
		break;
	case MGPT_REQ_TRANSCRIPTION_BEGIN_DIALOGUE:
		{
			MG_REQ_BEGIN_DIALOGUE msg;
			msg.m_dialogueId = lID;
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg);
		}
		break;
	case MGPT_REQ_TRANSCRIPTION_FINISH_DIALOGUE:
		{
			MG_REQ_FINISH_DIALOGUE msg;
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg);
		}
		break;
	case MGPT_REQ_DAILY_AWARD:
		{
			MG_REQ_Daily_Award msg;
			msg.m_RankID = getRankIDByType(iParam1, LOCAL_SERVER);
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg);
		}
		break;
	case MGPT_RANK_QUERY_BASEINFO_REQ:
		{
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType);
		}
		break;
	case MGPT_REQ_DRAW_LIVENESS_REWARD:
		{
			MG_REQ_DrawLivenessReward msg;
			msg.m_uiType = lID;
			msg.m_uiLevel = iParam1;

			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg);
		}
		break;
	case MGPT_REQ_SETAI:
		{
			MG_SetAI_Req msg;
			msg.m_SetAI = iParam1 > 0;

			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg);
		}
		break;
	case MGPT_REQ_TASK_ONE_KEY_OP: //一键完成每日任务
		{
			MG_REQ_TaskOneKeyOP msg;
			msg.m_iTaskID = lID;
			msg.m_iOPCode = iParam1;
			msg.m_iOPPara = iParam2;

			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg);
		}
		break;

	case MGPT_REQ_TASK_ASSIGN_ONE_KEY_OP://一键完成所有任务
		{
			MG_REQ_TaskAssginOneKeyOP msg;
			msg.m_uiAssignType = iParam1;
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg);
		}
		break;

	case MGPT_REQ_TASK_PHAGOTROPHY_REQ:
	case MGPT_RLT_TASK_PHAGOTROPHY_RESP:
		{
			LC_ClientTaskManager* pkTaskManager = (LC_ClientTaskManager*)LC_ClientTaskManager::GetSingletonPtr();
			if (NULL == pkTaskManager)
				return;
			LC_ClientTaskMap* pkTaskMap = GetClientTaskMap();
			if (NULL == pkTaskMap)
				return;
			ResultType iResultType = pkTaskMap->CanPhagotrophy(pkTaskManager->GetTask(lID), pkTaskManager->GetTask(iParam1));
			if (iResultType != RE_SUCCESS)
			{
				SendLogicInfoMessage(iResultType);
				return;
			}

			MG_REQ_TaskPhagotrophy msg;
			msg.m_lPhagotrophID = lID;
			msg.m_lPhagocytableID = iParam1;
			msg.m_lType = (iType == MGPT_REQ_TASK_PHAGOTROPHY_REQ) ? 0 : 1;

			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_TASK_PHAGOTROPHY_REQ, &msg);
		}
		break;
	case MGPT_REQ_REDUCE_TASK_DIFF_STAR: //降低每日任务难度
		{
			if(lID == INVALID_TASK_ID)
				return;
			MG_REQ_ReduceTaskDiffStar msg;
			msg.m_iTaskID = lID;
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg);
		}
		break;

	case MGPT_REQ_PROMOTE_TASK_REWARD_STAR://提高每日任务奖励
		{
			MG_REQ_PromoteTaskRewardStar msg;
			msg.m_iTaskID = lID;
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg);
		}
		break;

	case MGPT_REQ_TASK_DRAW_ROUND_OFF_REWARD://获取每日任务完成奖励
		{
			MG_REQ_TaskDrawRoundOffReward msg;
			msg.m_uiAssignType = iParam1;
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg); 
		}
		break;
	case MGPT_GREAT_KUNGFU_OPERATION_REQ:
		{
			MG_GreatKungFuOperationREQ msg;
			msg.m_uiID = lID;
			msg.m_uiOPCode = iParam1;
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg);
		}
		break;
	case MGPT_REQ_DRAW_FDEPOSIT_REWARD:
		{
			MG_REQ_Draw_FDepositReward msg;
			msg.m_uiLevel = iParam1;
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg);
		}
		break;
	case MGPT_REQ_OPEN_ABILITY:
		{
			MG_ReqOpenAbility msg;
			msg.m_iID = lID;
			msg.m_nParam = iParam1;
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg);
		}
		break;
	case MGPT_REQ_EXTERN_BASICBAG:
		{
			MG_Common msg;
			msg.set("play_id", m_iID);
			msg.set("bag_slot", iParam1);
			if(iParam2 > 0)
				msg.set("use_item", 1);

			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg);
		}
		break;
	case MGPT_REQ_DOMAINREWARD:
		{
			MG_DomainReward_Req msg;
			msg.m_ResId = lID;
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg);
		}
		break;
	case MGPT_REQ_GUILDRAIDREWARD:
		{
			MG_GuildRaidReward_Req msg;
			msg.m_Page = lID;
			msg.m_Stage = iParam1;
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg);
		}
		break;
	case MGPT_GAMESTORY_KEY_ECOUNTED_STAT_REQ:
		{
			MG_GAMESTORY_ACTOR_ENCOUNTED_STAT_REQ msg;
			msg.m_iStroyID = lID;
			msg.m_uiID = iParam1;
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg);
		}
		break;
	case MGPT_REQ_BUYCOURAGE_ITEM:
		{
			MG_BuyCourageItem_Req msg;
			msg.m_ItemId = lID;
			msg.m_Count = 1;
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg);
		}
		break;
	case MGPT_REQ_TRANSFER_TO_LOCATION:
		{
			CF_WorldObjectDesc::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_WorldObjectDesc>(lID);
			if (NULL == pkData)
				return;

			CF_WorldMapList::DataEntry* pkWorldMapData = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(pkData->_lMapID);
			if (!pkWorldMapData)
				return;

			NW_ClientSessionManager* pkSessionManager	= NW_ClientSessionManager::GetSingletonPtr();
			NW_ClientSessionGroup* pkSessionGroup		= pkSessionManager->GetGameSessionGroup();

			mem::vector<UT_Vec3Int> kLocVec;
			LC_Helper::SpliteString2MultiVec3Int(pkData->_kExtLocation, kLocVec);
			kLocVec.push_back(pkData->_kLocation);
			int iRandomIndex = Utility::UT_MathBase::RandInRangeInt(0, kLocVec.size() - 1);
			UT_Vec3Int &Loc = kLocVec[iRandomIndex];

			iParam3 = GetCurrentLineNum();
			//距离太近就不用飞鞋，走过去
			if((iParam1 == 0 || iParam1 == pkSessionGroup->GetCurGameServer()) && GetMapResID() == pkData->_lMapID && UT_MathBase::LineLengthXYInt(GetCurrentLocation(), Loc) < g_settings.DS_FLY_DIS_JUDGEMENT )
			{
				SC_ClientScriptInterface* pkClientInterface = SC_ClientScriptInterface::GetSingletonPtr();
				SC_ClientSkillInterface* pkInterface = pkClientInterface->GetSkillInterface();

				pkInterface->PathToObject(lID);

				//SendLogicInfoMessage(RE_NEAR_NOT_FLY);
				return ;
			}
			
			//BUG #25489 【BUG】【帮会战】势力战不允许持旗人使用飞鞋
			if (IsSkillStateExist(GuildWarTmpStateID) || IsSkillStateExist(GuildWarTmpStateID_T) || IsSkillStateExist(DomainWarTmpState_ID))
			{
				SendLogicInfoMessage(RE_CANOT_FLY_BY_HOLD_FLAG);
				return;
			}

			if (IsInCrossRealMapTeam() && !IS_NORMAL_MAP(pkWorldMapData->_iMapLogicType))
			{
				SendLogicInfoMessage(RE_IN_CROSS_MAP_TEAM_CAN_ENTER_AREA);
				return;
			}

			/*if ( IsRedNameInvalidTTT(pkData->_lMapID) )
			{
				SendLogicInfoMessage(RE_EVIL_TRANSFER);
				return ;
			}*/

			if(!CheckCanTransferMap(pkData->_lMapID))
				return;

			switch(pkData->_lType)
			{
			case LINK_TYPE_NPC_LOCATION:
			case LINK_TYPE_COLLECT_LOCATION:
				{
					//this->PendingPathAction(PPAT_TALK_TO_NPC, pkData->_lLogicID);
					this->ClearPathPendingAction();
					this->PendingPathActionForFeiXie(PPAT_TALK_TO_NPC, pkData->_lLogicID);
				}
				break;
			case LINK_TYPE_MONSTER_LOCATION:
			case LINK_TYPE_MAP_TRANSFER_HOOK:
				{
					//this->PendingPathAction(PPAT_ATTACK_NPC, pkData->_lLogicID);
					this->ClearPathPendingAction();
					this->PendingPathActionForFeiXie(PPAT_ATTACK_NPC, pkData->_lLogicID);
				}
				break;
			case LINK_TYPE_PARALLEL_SPACE:
				{
					ClearPathPendingAction();
					PendingPathActionForFeiXie(PPAT_TRANSFER, pkData->_lLogicID);
				}
				break;
			default:
				{
					this->ClearPathPendingActionForFeiXie();
				}
				break;
			}

			int tarMapLogicID = 0;
			if(pkData->_lParam & LINK_PARAM_MAIN_MAP)
			{
				LC_ClientNewBeeManager* pkNewBeeManager = LC_ClientNewBeeManager::GetSingletonPtr();
				if (pkNewBeeManager)
				{
					tarMapLogicID = pkNewBeeManager->GetMainLogicIDByMapResID(pkData->_lMapID);
				}
			};

			_resetAutoTask();

			int iServerID = iParam1;
			/*if (iServerID <= 0)
			{
				iServerID = GetServerID();
			}*/
			MG_Req_TransferToLocation msg;
			msg.m_iLocX = Loc.x;
			msg.m_iLocY = Loc.y;
			msg.m_iLocZ = Loc.z;
			msg.m_iMapResID = pkData->_lMapID;
			msg.m_iMapLogicID = tarMapLogicID;
			msg.m_iServerID = iServerID;
			msg.m_iLineNum = iParam3;
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg);

			GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM,"LC_ClientGamePlayer::RequestTransferToLocation2 playerID=%d iMapResID=%d iMapLogicID=%d iServerID=%d iLineNum=%d",
				GetID(), pkData->_lMapID, tarMapLogicID, iServerID, iParam3);

			SetTranToTranTick(GET_PROCESS_TIME);
		}
		break;
	case MGPT_REQ_GUILDWAR_ENTER:
	case MGPT_REQ_GLOBALGUILDWAR_ENTER:
	case MGPT_REQ_GUILDRAIDENTER:
		{
			if (IsInCrossRealMapTeam())
			{
				SendLogicInfoMessage(RE_IN_CROSS_MAP_TEAM_CAN_ENTER_AREA);
				return;
			}
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType);
		}
		break;
	case MGPT_REQ_SET_TITLE:
		{
			MG_REQ_SetTitle msg;
			msg.m_lPlayerID = GetID();
			msg.m_iSetTitleIndex = lID;
			if(lID == 0)
				msg.m_iSetTitleType = TNT_NONE_TITLE;
			else
				msg.m_iSetTitleType = TNT_SYSTEM_TITLE;
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg);
		}
		break;
	case MGPT_SEND_DAILY_CLEAN_REQ:
		{
			MG_Resp_DailyClean msg;
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg);
		}
		break;
	case MGPT_REQ_EXECUTE_ACTOR_OPERATION:
		{
			MG_REQ_ExecuteOperation msg;
			msg.m_lOperationType = lID;
			msg.m_nBuddyID = iParam1;
			msg.m_nIndex = iParam2;
			msg.m_nDescID = iParam3;
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg);
		}
		break;
	case MGPT_REQ_GAIN_OFFLINE_EXP:
		{
			MG_REQ_GainOfflineExp msg;
			msg.m_gainType = iParam1;
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg);
		}
		break;
	case MGPT_REQ_TRANSFER_DETAIL:
		{
			GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MGPT_REQ_TRANSFER_DETAIL m_iResID = %d ", lID);
			MG_REQ_TransferDetail reqMsg;
			reqMsg.m_iResID = lID;
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &reqMsg);
		}
		break;
	case MGPT_FALLIANCE_BOSS_INFO_REQ:
		{
			MG_Req_Alliance_Server_List reqMsg;
			reqMsg.m_nGroupID = lID;
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &reqMsg);
			GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"BOSS信息 MGPT_FALLIANCE_BOSS_INFO_REQ lID=%d", lID);
		}
		break;
	case  MGPT_REQ_SOULREFINE_LVLUP:
		{
			MG_REQ_SoulRefineLvlUp reqMsg;
			reqMsg.iPage = lID;
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &reqMsg);
		}
		break;
	case MGPT_REQ_FASHION_SHOW:
		{
			MG_PB<msg_define::fashion_show_req> reqMsg;
			reqMsg.m_value.set_type(lID);
			reqMsg.m_value.set_id(iParam1);
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &reqMsg);
		}
		break;
	case MGPT_REQ_SHOW_FASHION:
		{
			MG_REQ_ShowFashion reqMsg;
			reqMsg.m_lPlayerID = GetID();
			reqMsg.m_bIsShowFashion = lID > 0;
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &reqMsg);
		}
		break;
	default:
		{
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType);
		}
	}

	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
//---------------------------------------------------------------------------
void LC_ClientGamePlayer::SendRequestInfoByTypeWithStringPram(int iType, const StringType& kParam)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	switch(iType)
	{
	case MGPT_REQ_GUILDWAR_CATAPULT_FIRE:	//公会战发射炮弹
		{
			MG_GuildWar_FireCatapult_Req msg;
			TStringVector kStringVector = UT_BaseStringUtility::SplitString(kParam, '#');
			msg.m_CatapultId = atoi(kStringVector[0].c_str());
			msg.m_TargetX = atoi(kStringVector[1].c_str());
			msg.m_TargetY = atoi(kStringVector[2].c_str());
			msg.m_CatapultLevel = atoi(kStringVector[3].c_str());

			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg); 
		}
		break;
	case MGPT_CROSSREALM_MAPLIST_REQ:
		{
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, NULL); 
		}
		break;
	case MGPT_CROSSREALM_TEAMLIST_REQ:
		{
			MG_REQ_CrossRealm_TeamList msg;
			TStringVector kStringVector = UT_BaseStringUtility::SplitString(kParam, '#');
			msg.m_MapTypeId = atoi(kStringVector[0].c_str());

			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg);
		}
		break;
	case MGPT_CROSSREALM_TEAMDETAIL_REQ	:
		{
			MG_REQ_CrossRealm_TeamDetail msg;
			TStringVector kStringVector = UT_BaseStringUtility::SplitString(kParam, '#');
			msg.m_MapTypeId = atoi(kStringVector[0].c_str());
			msg.m_TeamId = atoi(kStringVector[1].c_str());

			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg);
		}
		break;
	case MGPT_CROSSREALM_ADD2TEAM_REQ:
		{
			MG_REQ_CrossRealm_Add2Team msg;
			TStringVector kStringVector = UT_BaseStringUtility::SplitString(kParam, '#');
			msg.m_MapTypeId = atoi(kStringVector[0].c_str());
			msg.m_TeamId = atoi(kStringVector[1].c_str());

			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg);
		}
		break;
	case MGPT_CROSSREALM_CREATETEAM_REQ:
		{
			MG_REQ_CrossRealm_CreateTeam msg;
			TStringVector kStringVector = UT_BaseStringUtility::SplitString(kParam, '#');
			msg.m_MapTypeId = atoi(kStringVector[0].c_str());
			msg.m_AutoStart = Long2Bool(atoi(kStringVector[1].c_str()));
			msg.m_TotalScore = _atoi64(kStringVector[2].c_str());

			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg);
		}
		break;
	case MGPT_CROSSREALM_LEAVETEAM_REQ:
		{
			MG_REQ_CrossRealm_Add2Team msg;
			TStringVector kStringVector = UT_BaseStringUtility::SplitString(kParam, '#');
			msg.m_MapTypeId = atoi(kStringVector[0].c_str());
			msg.m_TeamId = atoi(kStringVector[1].c_str());

			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg);
		}
		break;
	case MGPT_CROSSREALM_TEAMINFOEDIT_REQ:
		{
			MG_REQ_CrossRealm_TeamInfoEdit msg;
			TStringVector kStringVector = UT_BaseStringUtility::SplitString(kParam, '#');
			msg.m_MapTypeId = atoi(kStringVector[0].c_str());
			msg.m_AutoStart = Long2Bool(atoi(kStringVector[1].c_str()));
			msg.m_TeamId = atoi(kStringVector[2].c_str());
			msg.m_TotalScore = _atoi64(kStringVector[3].c_str());

			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg);
		}
		break;
	case MGPT_CROSSREALM_KICKMEMBER_REQ:
		{
			MG_REQ_CrossRealm_Kick_Member msg;
			TStringVector kStringVector = UT_BaseStringUtility::SplitString(kParam, '#');
			msg.m_MapTypeId = atoi(kStringVector[0].c_str());
			msg.m_TeamId = atoi(kStringVector[1].c_str());
			msg.m_TargetCharID = _atoi64(kStringVector[2].c_str());

			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg);
		}
		break;
	case MGPT_CROSSREALM_READYMEMBER_REQ:
		{
			MG_REQ_CrossRealm_Ready_Member msg;
			TStringVector kStringVector = UT_BaseStringUtility::SplitString(kParam, '#');
			msg.m_MapTypeId = atoi(kStringVector[0].c_str());
			msg.m_TeamId = atoi(kStringVector[1].c_str());
			msg.m_Ready = Long2Bool(atoi(kStringVector[2].c_str()));

			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg);
		}
		break;
	case MGPT_CROSSREALM_ENTER_REQ:
		{
			MG_REQ_CrossRealm_Enter msg;
			TStringVector kStringVector = UT_BaseStringUtility::SplitString(kParam, '#');
			msg.m_MapTypeId = atoi(kStringVector[0].c_str());
			msg.m_TeamId = atoi(kStringVector[1].c_str());
		
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg);
		}
		break;
	case MGPT_CROSSREALM_AUTOADD_REQ:
		{
			MG_REQ_CrossRealm_AutoAdd_Team msg;
			TStringVector kStringVector = UT_BaseStringUtility::SplitString(kParam, '#');
			msg.m_MapTypeId = atoi(kStringVector[0].c_str());
			//msg.m_PlayerInfo = 

			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg);
		}
		break;
	case MGPT_CROSSREALM_INVITEPLAYER_REQ:
		{
			MG_REQ_InvitePlayer_CrossRealmTeam msg;
			TStringVector kStringVector = UT_BaseStringUtility::SplitString(kParam, '#');
			msg.m_iBeInviteCharID = _atoi64(kStringVector[0].c_str());
			msg.m_iMapTypeID = atoi(kStringVector[1].c_str());
			msg.m_iTeamID = atoi(kStringVector[2].c_str());
			msg.m_inviteType = atoi(kStringVector[3].c_str());
			msg.m_iScore = _atoi64(kStringVector[4].c_str());
			
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg);

		}
		break;
	case MGPT_CROSSREALM_REFUSE_INVITE:
		{
			MG_CrossRealm_Refush_Invate msg;
			TStringVector kStringVector = UT_BaseStringUtility::SplitString(kParam, '#');
			msg.m_iInviteCharID = _atoi64(kStringVector[0].c_str());

			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg);

		}
		break;
	case MGPT_REQ_GET_BUY_TIMES:
		{
			MG_REQ_GetBuyTimes msg;
			TStringVector kStringVector = UT_BaseStringUtility::SplitString(kParam, '#');
			msg.m_iGameStoryId = atoi(kStringVector[0].c_str());

			if(kStringVector.size() > 1)
				msg.m_iBuyType = atoi(kStringVector[1].c_str());
			else
				msg.m_iBuyType = Buy_Raid_Type_VIPProfit;

			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg);
		}
		break;
	case MGPT_REQ_BUY_TIMES:
		{
			MG_REQ_BuyTimes msg;
			TStringVector kStringVector = UT_BaseStringUtility::SplitString(kParam, '#');
			size_t iSize = kStringVector.size();
			T_ASSERT_CRITICAL_S(iSize >= 2);

			msg.m_iGameStoryId = atoi(kStringVector[0].c_str());
			msg.m_iTimes = atoi(kStringVector[1].c_str());
			
			if (iSize == 3)
			{
				msg.m_iBuyType = atoi(kStringVector[2].c_str());
			}
			else
			{
				msg.m_iBuyType = Buy_Raid_Type_VIPProfit;
			}

			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg);
		}
		break;
	case MGPT_INVITE_ALLPLAYER_CROSSREALMTEAM_REQ:
		{
			MG_REQ_InviteAllPlayer_CrossRealmTeam msg;
			TStringVector kStringVector = UT_BaseStringUtility::SplitString(kParam, '#');
			msg.m_inviteType = atoi(kStringVector[0].c_str());
			msg.m_iMapTypeID = atoi(kStringVector[1].c_str());
			msg.m_iTeamID = atoi(kStringVector[2].c_str());
			msg.m_iScore = _atoi64(kStringVector[3].c_str());
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg);
		}
		break;
	case MGPT_CROSSREALM_PLAYER_TEAMINFO_REQ:
		{
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, NULL);
		}
		break;
	case MGPT_GAMESTORY_OPERATION_REQ:
		{
			MG_GAMESTORY_OPERATION_REQ msg;
			TStringVector kStringVector = UT_BaseStringUtility::SplitString(kParam, '#');
			msg.m_iStoryID = atoi(kStringVector[0].c_str());
			msg.m_iOPCode = atoi(kStringVector[1].c_str());
			msg.m_kOPPara[0] = atoi(kStringVector[2].c_str());

			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg);
		}
		break;
	case MGPT_GAMESTORY_RELATE_TIME_REQ:
		{
			MG_GAMESTORY_RELATE_TIME_REQ msg;
			TStringVector kStringVector = UT_BaseStringUtility::SplitString(kParam, '#');
			msg.m_iStoryID = atoi(kStringVector[0].c_str());

			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg);
		}
		break;
	case MGPT_CROSSREALM_TEAM_OPERATION_REQ:
		{
			MG_REQ_CrossRealmTeamOperation msg;
			TStringVector kStringVector = UT_BaseStringUtility::SplitString(kParam, '#');
			msg.m_iMapTypeID = atoi(kStringVector[0].c_str());
			msg.m_iTeamID = atoi(kStringVector[1].c_str());
			msg.m_iOPCode = atoi(kStringVector[2].c_str());
			msg.m_iOPPara = atoi(kStringVector[3].c_str());

			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg);

		}
		break;
	case MGPT_REQ_RANK_CHALLENGE:
		{	
			if (IsInCrossRealMapTeam())
			{
				SendLogicInfoMessage(RE_IN_CROSS_MAP_TEAM_CAN_ENTER_AREA);
				return;
			}
			TStringVector kStringVector = UT_BaseStringUtility::SplitString(kParam, '#');
			unique_id_type iCharID = _atoi64(kStringVector[0].c_str());
			int rank = atoi(kStringVector[1].c_str());
			int type = atoi(kStringVector[2].c_str());

			MG_REQ_Challenge msg;
			msg.m_beChallengeCharID = iCharID;
			msg.m_BeChallengeIndex = rank;
			msg.m_ChallengeType = type;
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg);
		}
		break;
	case MGPT_REQ_POWERGUILD_APPOINT:
		{
			MG_Appoint_Power_REQ msg;
			TStringVector kStringVector = UT_BaseStringUtility::SplitString(kParam, '#');
			unique_id_type iCharID = _atoi64(kStringVector[0].c_str());
			int iParam1 = atoi(kStringVector[1].c_str());
			msg.m_TargetCharID = iCharID;
			msg.m_Position = iParam1;
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg);
		}
		break;
	case MGPT_REQ_SOULREFINE:
		{
			MG_REQ_SoulRefine msg;
			TStringVector kStringVector = UT_BaseStringUtility::SplitString(kParam, '#');
			if (kStringVector.size() >= 1)
			{
				msg.iPage = atoi(kStringVector[0].c_str());
				for (int i = 1; i < (kStringVector.size() + 1) / 2; ++ i)
				{
					msg.kSouls.insert(make_pair(atoi(kStringVector[2*i-1].c_str()),atoi(kStringVector[2*i].c_str())));
				}
			}
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &msg);
		}
		break;
	case MGPT_REQ_SHOT_ACTIVITY_RANK_INDEX:
		{

			MG_PB<msg_define::shot_activity_rank_index_req> kReqMsg;
			kReqMsg.m_value.set_key(TPSTR2STDSTR(kParam));
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType, &kReqMsg);
		}
		break;
	default:
		{
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, iType);
		}
	}
	if (bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
//------------------------------------------------------------------------------
void LC_ClientGamePlayer::SendRequestRaidBossSpeciesRankInfo(int StoryID, int MinIndex, int MaxIndex)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_GetRankYiZhongQinXi msg;
	msg.m_nStoryID = StoryID;
	msg.m_nMinIndex = MinIndex;
	msg.m_nMaxIndex = MaxIndex;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GETRANKYIZHONGQINXI, &msg);

	if (bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
//------------------------------------------------------------------------------
void LC_ClientGamePlayer::RequestRankInfo(int iRankID, int iBegin, int iEnd, int serverID, int areaID)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_RankInfo msg;
	msg.m_RankID = iRankID;
	msg.m_Begin = iBegin;
	msg.m_End = iEnd;
	msg.m_ServerID = serverID;
	msg.m_nType = 0;  //iType:排行榜类型， 1：竞技场 0：普通排行榜
	msg.m_nGroupId = areaID;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_RANK_INFO, &msg);

	GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"排行榜  发送请求MGPT_REQ_RANK_INFO，m_RankID=%d, m_Begin=%d, m_End=%d",iRankID, iBegin, iEnd);

	if (bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
//------------------------------------------------------------------------------
void LC_ClientGamePlayer::RequestPlayerBuddyRankInfo(int iType, uint64_t iCharID, int serverID)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_GetRankPlayerBuddyInfo msg;
	msg.m_BuddyType = iType;
	msg.m_CharID = iCharID;
	msg.m_ServerID = serverID;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GETRANK_PLAYERBUDDY_INFO, &msg);

	if (bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
//------------------------------------------------------------------------------
void LC_ClientGamePlayer::RequestBuddyRankInfo(int cID, uint64_t iCharID, int serverID)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_GetRankBuddyInfo msg;
	msg.m_BuddyID = cID;
	msg.m_CharID = iCharID;
	msg.m_ServerID = serverID;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GETRANK_BUDDY_INFO, &msg);

	if (bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
//------------------------------------------------------------------------------
void LC_ClientGamePlayer::RequestToyPetRankInfo(uint64_t iCharID, int serverID)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_GetRankPetInfo msg;
	msg.m_CharID = iCharID;
	msg.m_ServerID = serverID;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GETRANK_PET_INFO, &msg);

	if (bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
//------------------------------------------------------------------------------
void LC_ClientGamePlayer::RequestSubRankInfo(int iType, uint64_t iCharID, int serverID)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_GetRankSubInfo msg;
	msg.m_SubRankType = iType;
	msg.m_CharID = iCharID;
	msg.m_ServerID = serverID;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GETRANK_SUB_INFO, &msg);

	if (bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
//------------------------------------------------------------------------------
void LC_ClientGamePlayer::RequestRankTopnPlayerInfo(const StringType& kParam, int iTop, int iType)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_RankTopNPlayerInfo msg;
	TStringVector kStringVector = UT_BaseStringUtility::SplitString(kParam, '#');
	if (kStringVector.size()<=0)
		return;

	for(int i=0; i<kStringVector.size(); ++i)
		msg.m_RankIDVec.push_back(atoi(kStringVector[i].c_str()));


	msg.m_TopN = iTop;
	msg.m_BuddyType = iType;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_RANK_TOPN_PLAYER_INFO, &msg);

	if (bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
//------------------------------------------------------------------------------
void LC_ClientGamePlayer::RequestBuddyRankIndex(uint64_t iCharID)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_GetBuddyRankIndex msg;
	msg.m_CharID = iCharID;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GETBUDDYRANKINDEX, &msg);

	if (bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
//---------------------------------------------------------------------------

void LC_ClientGamePlayer::RequestPlayerDetail(unique_id_type iUid, int iSystemType)
{
	if (iUid == 0)
		return;

	MG_REQ_GetPlayerInfo msg;
	msg.m_CharID = iUid;
	msg.m_SystemType = iSystemType;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_PLAYER_DETAIL, &msg);
	if(bCreateMsg)
		SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

void LC_ClientGamePlayer::SendRequestSameLocationDifLine(int MapID, int ServerID, int Line)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;
	MG_Req_TransferToLocation msg;
	const Utility::UT_Vec3Int& Loc = GetVisibleLocation();
	msg.m_iLocX = Loc.x;
	msg.m_iLocY = Loc.y;
	msg.m_iLocZ = Loc.z;
	msg.m_iMapResID = MapID;
	LC_ClientNewBeeManager* pkNewBeeManager = LC_ClientNewBeeManager::GetSingletonPtr();
	msg.m_iMapLogicID = pkNewBeeManager->GetMainLogicIDByMapResID(MapID);
	msg.m_iServerID = ServerID;
	msg.m_iLineNum = Line;

	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_TRANSFER_TO_LOCATION, &msg);
	if (bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

//------------------------------------------------------------------------------
void LC_ClientGamePlayer::SendRequestCommitAnswer(int topicIndex, int choiceIndex)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_CommitAnswer msg;
	msg.m_nTopicIndex = topicIndex;
	msg.m_nChoice = choiceIndex;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_COMMITANSWER, &msg);

	if (bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
//------------------------------------------------------------------------------
void LC_ClientGamePlayer::SendRequestAnswerRankInfo(int MinIndex, int MaxIndex,int rankType)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_AnswerRankInfo msg;
	msg.m_nMinIndex = MinIndex;
	msg.m_nMaxIndex = MaxIndex;
	msg.m_nRankType=rankType ; 
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_ANSWERRANKINFO, &msg);

	if (bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
//------------------------------------------------------------------------------
void LC_ClientGamePlayer::SendRequestYiJiTuXiRankInfo(int MinIndex, int MaxIndex, bool bFlag)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_YiJiTuXiRankInfo msg;
	msg.m_bFlag = bFlag;
	msg.m_nMinIndex = MinIndex;
	msg.m_nMaxIndex = MaxIndex;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_YIJITUXIRANKINFO, &msg);

	if (bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}