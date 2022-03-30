//同步未出站列表
void HL_FromGameServer::_handlerSyncBuddyData(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_SC_BuddyDataSync BuddyDataMsg;
	if(!BuddyDataMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SC_BuddyDataSync解析错误");
		return;
	}
	
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SC_BuddyDataSync解析错误, GamePlayer 不存在");
		return;
	}

	//update InactiveBuddyList
	pkGamePlayer->UpdateInactiveBuddyList(BuddyDataMsg);
}

//同步已出站列表
void HL_FromGameServer::_handlerSyncActiveBuddyData(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_SC_ActivateBuddyDataSync BuddyDataMsg;
	if(!BuddyDataMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SC_ActivateBuddyDataSync解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SC_ActivateBuddyDataSync解析错误, GamePlayer 不存在");
		return;
	}

	if (pkGamePlayer->UpdateActiveBuddyList(BuddyDataMsg))
	{
		//Swap player data with buddy
		pkGamePlayer->SwapBuddyData(BuddyDataMsg.m_nCurrentIndex);
		//Reset Camera
		pkGamePlayer->ResetCamera();
	}

	pkGamePlayer->SetDirtyFlag(DIRTY_FLAG_ACTIVEBUDDYLIST, true);
	pkGamePlayer->AddDirtyFlagRevisionID(DIRTY_FLAG_ACTIVEBUDDYLIST);

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_BUDDY_SCORE_CHANGED);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

//Buddy通用回复
void HL_FromGameServer::_handlerBuddyReply(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_SC_BuddyReply BuddyDataMsg;

	if(!BuddyDataMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SC_BuddyReply解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SC_BuddyReply解析错误, GamePlayer 不存在");
		return;
	}

	uint32_t errCode = BuddyDataMsg.m_nErrorCode;
	ResultType eResult = RE_FAIL;
	int buddyID = 0;
	int mode = 0;
	LC_GameEvent kGameEvent;
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	switch(errCode)
	{
	case RE_BUDDY_ERROR_BUY_OK:
		eResult = errCode;
		buddyID = BuddyDataMsg.m_nBuddyTemplateID;
		mode = BuddyDataMsg.m_nMode;

		kGameEvent.SetEventType(GLET_BUDDY_UNLOCK);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, buddyID);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, mode);
		pkUIManager->HandlerGameEvent(kGameEvent);

		break;
	case RE_BUDDY_ERROR_BUY_NO_TEMPLATE:
		eResult = errCode;
		break;
	case RE_BUDDY_ERROR_BUY_NO_RES:
		eResult = errCode;
		break;
	case RE_BUDDY_ERROR_BUY_GOT:
		eResult = errCode;
		break;
	case RE_BUDDY_ERROR_ACTIVATE_OK:
		eResult = errCode;
		pkGamePlayer->SetActivateBuddyFlag(true);

		//竞技场相关地图中不显示该提示
		if(IS_ARENA_MAP(pkGamePlayer->GetMapLogicType()))
			return;

		break;
	case RE_UNBIND_YUANBAO_NOT_ENOUGH:
		{
			eResult = errCode;
			LC_GameEvent kGameEvent;
			kGameEvent.SetEventType(GLET_PLAYER_RESPAWN);
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, errCode);

			UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
			pkUIManager->HandlerGameEvent(kGameEvent);
		}
		break;
	case RE_BUDDY_ERROR_SWAP_CONTROLLER_COOLDOWN:
		{
			SendLogicInfoMessage(errCode, BuddyDataMsg.m_nSwapCDTime);
		}
		break;
	case RE_BUDDY_ERROR_ACTIVATE_BUDDYID_IN_USE:
	case RE_BUDDY_ERROR_ACTIVATE_BUDDYID_INVALID:
	case RE_BUDDY_ERROR_ACTIVATE_SLOT_INVALID:
	case RE_BUDDY_ERROR_ACTIVATE_NONE_CONTROLLER:
	case RE_BUDDY_ERROR_ACTIVATE_CD_NOT_READY:
	case RE_BUDDY_ERROR_ACTIVATE_MAP_DENY:
	case RE_BUDDY_ERROR_ACTIVATE_MAP_DENY_FIGHT:
	case RE_BUDDY_ERROR_RESPAWN_MAP_DENY:
	case RE_BUDDY_ERROR_RESPAWN_LESS_ITEM:
	case RE_BUDDY_ERROR_DEACTIVATE_OK:
	case RE_BUDDY_ERROR_DEACTIVATE_DEAD:
	case RE_BUDDY_ERROR_DEACTIVATE_NIL_SLOT:
	case RE_BUDDY_SCHEME_ERROR_OK:
	//case RE_BUDDY_SCHEME_ERROR_NOT_MATCH:
	//case RE_BUDDY_SCHEME_ERROR_NO_RES:
	case RE_BUDDY_SCHEME_ERROR_PARAM:
	case RE_BUDDY_SCHEME_ERROR_Q_INVALID:
	case RE_BUDDY_SCHEME_ERROR_EXIST:
	//case RE_BUDDY_SCHEME_ERROR_TYPE:
	case RE_BUDDY_SCHEME_ERROR_NO_INSTANCE:
		eResult = errCode;
		break;
	default:
		break;
	}

	SendLogicInfoMessage(eResult);
}

//切换Buddy回复
void HL_FromGameServer::_handlerSCSetActivateBuddyReply(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_SC_SetActivateBuddyReply BuddyDataMsg;
	if(!BuddyDataMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SC_SetActivateBuddyReply解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SC_SetActivateBuddyReply解析错误, GamePlayer 不存在");
		return;
	}
	
	GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI, "[LHX] _handlerSCSetActivateBuddyReply: ActiveBuddyIndex=%d", BuddyDataMsg.m_nIndex);

	if(pkGamePlayer->CheckTargetActiveBuddyIndex(BuddyDataMsg.m_nIndex))
	{
		pkGamePlayer->SetCurrentActiveBuddyIndex(BuddyDataMsg.m_nIndex);
	}
	
	pkGamePlayer->SwapBuddyData(BuddyDataMsg.m_nIndex);
	pkGamePlayer->ResetCamera();
	pkGamePlayer->ActiveSwapCoolDown(pkGamePlayer->GetPendingSwapIndex(), SWITCH_BUDDY_CD_TIME);
	pkGamePlayer->SetDirtyFlag(DIRTY_FLAG_CURRENTINDEX, true);
	pkGamePlayer->AddDirtyFlagRevisionID(DIRTY_FLAG_CURRENTINDEX);
	pkGamePlayer->SetDirtyFlag(DIRTY_FLAG_ACTIVEBUDDYLIST, true);
	pkGamePlayer->AddDirtyFlagRevisionID(DIRTY_FLAG_ACTIVEBUDDYLIST);
}

//同步Buddy基础数据
void HL_FromGameServer::_handlerSCBuddyBaseDataSync(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_SC_BuddyBaseDataSync BuddyDataMsg;
	if(!BuddyDataMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SC_BuddyBaseDataSync解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SC_BuddyBaseDataSync解析错误, GamePlayer 不存在");
		return;
	}

	int iCid = BuddyDataMsg.m_Data.nBuddyTemplateId;
	int iTransformId = BuddyDataMsg.m_Data.nCurrentTransformID;
	T_ASSERT_CRITICAL_S(iCid > 0 && iTransformId > 0);

	LC_ClientBuddyInstance* pkBuddyInstance = pkGamePlayer->GetBuddyInstance(iCid);
	if (pkBuddyInstance == NULL)
		pkBuddyInstance = pkGamePlayer->CreateNewBuddyInstance(iCid);

	int oldExp = pkBuddyInstance->GetEXP();
	int oldLevel = pkBuddyInstance->GetLevel();
	combat_score_type oldCombateScore = pkBuddyInstance->GetCombateScore();
	int oldTransformID = pkBuddyInstance->GetCurrentTransformID();

	pkBuddyInstance->ParseProtocol(BuddyDataMsg.m_Data);
	bool ActiveChange = pkBuddyInstance->SetActiveTransformRecord(BuddyDataMsg.m_Data.nActiveTransformRecord);
	bool TimeChnage = pkBuddyInstance->SetActiveTransformInvaildTime(BuddyDataMsg.m_Data.m_nBuddyTransformInvaildTime);
	if (oldTransformID != iTransformId)
	{
		if(!pkGamePlayer->IsBuddyInstance(iCid))
		{
			object_id_type buddyObjectID  = pkGamePlayer->GetBuddyObjectID(iCid);
			LC_PlayerBase* pkBuddy = pkGamePlayer->GetBuddyPlayerBasePtr(buddyObjectID);
			if(pkBuddy != NULL)
			{
				pkBuddy->ChangeVisibleData(iTransformId);
			}
		}
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_BUDDYTRANSFORM_CHANGE);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, iCid);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, iTransformId);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
	if (ActiveChange || TimeChnage)
	{
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_BUDDYTRANSFORMLIST_CHANGE);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, iCid);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
	pkGamePlayer->UpdateDirtyFlag(DIRTY_FLAG_PROPERTY);

	if (oldExp != pkBuddyInstance->GetEXP())
	{
		pkGamePlayer->SyncBuddyEXP(iCid);
	}
	
	if (oldLevel != pkBuddyInstance->GetLevel())
	{
		pkGamePlayer->SyncBuddyLevel(iCid);
	}

	if (oldCombateScore != pkBuddyInstance->GetCombateScore())
	{
		pkGamePlayer->SyncBuddyCombate(iCid);
	}

	if (pkBuddyInstance->GetInitial())
	{
		pkBuddyInstance->SetInitial(false);
		return;
	}

	if(oldExp != pkBuddyInstance->GetEXP() || oldLevel != pkBuddyInstance->GetLevel())
	{
		pkGamePlayer->SendBuddyExpAndLevelChange(oldExp,oldLevel,pkBuddyInstance->GetEXP(),pkBuddyInstance->GetLevel(), iCid);
	}
}

void HL_FromGameServer::_handlerCreateBuddyNetPlayer(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_CreateBuddyNetplayer msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_CreateBuddyNetplayer解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_CreateBuddyNetplayer解析错误, GamePlayer 不存在");
		return;
	}
	
	//切分线时，非Q角色ID会变
	//更新非Q为主控时，主控的ID。
	if(pkGamePlayer->GetCharType() == msg.m_iBuddyId)
	{
		pkGamePlayer->ChangeID(msg.m_iPlayerId);
	}
}

void HL_FromGameServer::_handlerSyncSingleBuddySchemeData(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	/*MG_SC_SingleBuddyActivateSchemeData rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SC_SingleBuddyActivateSchemeData解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SC_SingleBuddyActivateSchemeData解析错误, GamePlayer 不存在");
		return;
	}

	pkGamePlayer->SyncSchemeData(rltMsg.m_Data);
	pkGamePlayer->UpdateDirtyFlag(DIRTY_FLAG_BUDDY_SCHEME_LIST);*/
}

void HL_FromGameServer::_handlerSyncMultiBuddySchemeData(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	/*MG_SC_MultiBuddyActivateSchemeData rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SC_MultiBuddyActivateSchemeData解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SC_MultiBuddyActivateSchemeData解析错误, GamePlayer 不存在");
		return;
	}

	pkGamePlayer->SyncAllSchemeData(rltMsg);
	pkGamePlayer->UpdateDirtyFlag(DIRTY_FLAG_BUDDY_SCHEME_LIST);*/
}

void HL_FromGameServer::_handlerSyncBuddyActivateSchemeType(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_SC_BuddyActivateSchemeType rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SC_BuddyActivateSchemeType解析错误");
		return;
	}


	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SC_BuddyActivateSchemeType解析错误, GamePlayer 不存在");
		return;
	}

	pkGamePlayer->SetSchemeType(rltMsg.m_nCurrentType);
	pkGamePlayer->UpdateDirtyFlag(DIRTY_FLAG_ACTIVEBUDDYLIST);
}

void HL_FromGameServer::_handlerBuddySchemeReply(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_SC_BuddySchemeReply rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SC_BuddySchemeReply解析错误");
		return;
	}

	if(rltMsg.m_nError != RE_SUCCESS)
		SendLogicInfoMessage(rltMsg.m_nError);
}



//同步玩家级别信息
void HL_FromGameServer::_handlerSYNCPlayerControllerProperty(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncPlayerControllerProperty BuddyDataMsg;
	if(!BuddyDataMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SyncPlayerControllerProperty解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SyncPlayerControllerProperty解析错误, 没有GamePlayer");
		return;
	}

	LC_ClientGamePlayerCommander* pkCommander = pkGamePlayer->GetCommander();
	if(pkCommander == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SyncPlayerControllerProperty解析错误, Gameplayer 没有 commander");
		return;
	}
	
	if (!pkCommander->isInited())
	{
		pkCommander->ParseProtocol(BuddyDataMsg);
		//pkGamePlayer->CommanderCheckLevelUp();
	}

	pkGamePlayer->SetGMLevel(BuddyDataMsg.m_GMLevel);

	pkCommander->UpdatePropertys(BuddyDataMsg);

	if (pkCommander->CheckEXP(BuddyDataMsg.m_iEXP))
		pkGamePlayer->CommanderSetEXP(BuddyDataMsg.m_iEXP);

	if (pkCommander->CheckLevel(BuddyDataMsg.m_iLevel))
		pkGamePlayer->CommanderLevelUp(BuddyDataMsg.m_iLevel);

	pkGamePlayer->AcceptTaskAfterLogin();

	//if (pkCommander->CheckTotalCombateScore(BuddyDataMsg.m_iTotalCombatScore))
	//	pkGamePlayer->CommanderSetTotalCombateScore(BuddyDataMsg.m_iTotalCombatScore);

	pkCommander->SetTaskHonorPoint(BuddyDataMsg.m_iTaskHonorPoint);

	uint32_t iOldFinishCount = pkCommander->GetMainTaskFinishCount();
	uint32_t iNewFinishCount = BuddyDataMsg.m_iMainTaskFinishCount;
	if( iOldFinishCount != iNewFinishCount)
	{
		pkCommander->SetMainTaskFinishCount(iNewFinishCount);

		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_MAINTASK_COUNT_ADD);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, iOldFinishCount);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, iNewFinishCount);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
	
	pkGamePlayer->CheckCommanderTitleOpen();
	pkGamePlayer->UpdateDirtyFlag(DIRTY_FLAG_PROPERTY);
}

//玩家buddy升星回复
void HL_FromGameServer::_handlerRLTBuddyStarUpgrade(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_BuddyStarUpgrade BuddyDataMsg;
	if(!BuddyDataMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_CS_BuddyStarUpgrade解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_CS_BuddyStarUpgrade解析错误, 没有 GamePlayer");
		return;
	}

	uint32_t errCode = BuddyDataMsg.m_nErrorCode;
	ResultType eResult = RE_FAIL;
	//金币消耗是成功后显示的，所以获取到的星级是升级后的星级
	long cost = pkGamePlayer->GetBuddyUpgradeStarCost(BuddyDataMsg.m_nBuddyID);

	switch(errCode)
	{
	case RE_BUDDY_ERROR_STAR_OK:
		{
			eResult = errCode;
			SendLogicInfoMessage(eResult, cost);

			CF_Buddy::DataEntry *pkBuddyData = SafeGetCSVFileDataEntryPtr<CF_Buddy>(BuddyDataMsg.m_nBuddyID);
			if(pkBuddyData != NULL)
				g_soundMgr.Play2DEx(pkBuddyData->_sStarSound);
		}
		break;
	case RE_BUDDY_ERROR_STAR_NO_RESOURCE:
		{
			//道具消耗是失败后显示的，获取到的星级是升级前的星级
			int itemID = pkGamePlayer->GetBuddyUpgradeItemID(BuddyDataMsg.m_nBuddyID, false);
			LC_ItemBase* pkItem = LC_ItemFactoryBase::GetSingletonPtr()->RequestItem(itemID);
			T_ASSERT_CRITICAL_S(pkItem != NULL);

			LC_ItemDescriptionEntry* pkItemDescript = pkItem->GetItemDescriptionEntry();
			StringType sItemName = LC_Helper::GetGradeName(pkItemDescript->GetItemName(), pkItemDescript->GetGrade());
			
			eResult = errCode;
			SendLogicInfoMessage(eResult, sItemName);
		}
		break;
	case RE_BUDDY_ERROR_STAR_NO_LEVEL:
	case RE_UNBIND_CASH_NOT_ENOUGH:
		eResult = errCode;
		SendLogicInfoMessage(eResult);
		break;
	default:
		SendLogicInfoMessage(eResult);
		break;
	}

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerMsgGameEvent(MGPT_RLT_BUDDY_STAR_UPGRADE, Int2String(errCode));
}

//玩家buddy升级回复
void HL_FromGameServer::_handlerRltBuddyManualLevelUp(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_BuddyManualLevelUp rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_RLT_BuddyManualLevelUp解析错误");
		return;
	}

	if(rltMsg.m_nErrorCode != RE_SUCCESS)
		SendLogicInfoMessage(rltMsg.m_nErrorCode);
}

//玩家解锁的出战格子数量
void HL_FromGameServer::_handlerRltActivateBuddyCount(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SC_ActivateBuddyCount BuddyDataMsg;
	if(!BuddyDataMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SC_ActivateBuddyCount解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SC_ActivateBuddyCount解析错误, 没有 GamePlayer");
		return;
	}

	int oldSlotNum = pkGamePlayer->GetUnlockSlotNum();
	int unlockSlotNum = BuddyDataMsg.m_nCount;
	pkGamePlayer->SetUnlockSlotNum(unlockSlotNum);

	if (oldSlotNum != unlockSlotNum)
	{
		pkGamePlayer->AccumulateUnlockSlotInit();

		pkGamePlayer->SetDirtyFlag(DIRTY_FLAG_ACTIVEBUDDYLIST, true);
		pkGamePlayer->AddDirtyFlagRevisionID(DIRTY_FLAG_ACTIVEBUDDYLIST);
	}
}

//对已出战角色更新出战
void HL_FromGameServer::_handlerRefreshPlayerBindBuddy(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack)
{
	MG_RefreshPlayerBindBuddy BuddyDataMsg;
	if(!BuddyDataMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_RefreshPlayerBindBuddy解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_RefreshPlayerBindBuddy解析错误, 没有 GamePlayer");
		return;
	}
	
	GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI, "[LHX] _handlerRefreshPlayerBindBuddy: chartype=%d, iTransformID=%d", BuddyDataMsg.m_nBuddyInfo.m_nBuddyID, BuddyDataMsg.m_nBuddyInfo.m_nBuddyTransformID);

	const PS_Buddy_Info_BuddyCS& buddyInfo = BuddyDataMsg.m_nBuddyInfo;
	const int iCid = buddyInfo.m_nBuddyID;

	LC_ClientArenaPlayBackManager *pkArenaPlayBackManager = LC_ClientArenaPlayBackManager::GetSingletonPtr();
	object_id_type lID = BuddyDataMsg.m_lPlayerID;
	if (bArenaPlayBack)
		lID = pkArenaPlayBackManager->GetTemID(lID);

	LC_PlayerBase* pkPlayer = NULL;
	if (lID == pkGamePlayer->GetID())
		pkPlayer = pkGamePlayer;
	else
	{
		pkPlayer = pkGamePlayer->GetBuddyPlayerBasePtr(lID);
		if (!pkPlayer)
		{
			for (int i=0;i<3;++i)
			{
				object_id_type lID = pkGamePlayer->GetActiveBuddy(i)->nObjectId;
			}
		}
		else
		{
			pkPlayer->SetSkillActivateAll(buddyInfo.m_bActivateAllSkill);
			pkPlayer->SetBuddyStarLevel(buddyInfo.m_nBuddyStarLevel);
			pkPlayer->SetActiveRuneSlot(buddyInfo.m_bActivateAllSkill);
			pkPlayer->SetActiveEquipSlot(BuddyDataMsg.m_bEquipSlotStarLevel);
			pkPlayer->SetAllEquipReinforceLevel(buddyInfo.m_nAllEquipSlotReinforceLevel);
			
			LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
			SC_ClientScriptInterface* pkClientInterface = SC_ClientScriptInterface::GetSingletonPtr();
			SC_ClientSkillInterface* pkInterface = pkClientInterface->GetSkillInterface();

			mem::vector<int> runelevel;
			runelevel.resize(4);

			std::map<int32_t,PS_RuneItemInfo_BuddyCS>::const_iterator iter = buddyInfo.m_nBuddyRuneInfo.begin();
			for ( ; iter!=buddyInfo.m_nBuddyRuneInfo.end(); ++iter)
			{
				const PS_RuneItemInfo_BuddyCS& runeInfo = iter->second;
				if (runeInfo.m_nRuneLevel >= 0)
				{
					LC_ItemBase* pkItem = pkItemFactory->RequestItem(runeInfo.m_nRuneItemID);
					if (pkItem != NULL)
					{
						int runequality = pkInterface->GetRuneStarQuality(pkItem->GetItemDescriptionEntry()->GetStarUpType(), runeInfo.m_nRuneLevel);
						runelevel[iter->first] = runequality;
					}
				}
			}
			pkPlayer->SetRuneStarLevel(runelevel);

			mem::vector<int>equiplevel;
			equiplevel.resize(EQUIPSHOWORDERSIZE);
			mem::vector<int> equipIds;
			equipIds.resize(EQUIPSHOWORDERSIZE);

			std::map<int32_t,PS_EquipItemInfo_BuddyCS>::const_iterator iter2 = buddyInfo.m_nEquipSlotInfo.begin();
			for ( ; iter2!=buddyInfo.m_nEquipSlotInfo.end(); ++iter2)
			{
				const PS_EquipItemInfo_BuddyCS& equipInfo = iter2->second;
				const int iEquipIdx = iter2->first;
				equiplevel[iEquipIdx] = pkInterface->GetEquipStarQuality(iEquipIdx, equipInfo.m_nEquipSlotStarLevel);
				equipIds[iEquipIdx] = pkInterface->GetMadeItemGrade(equipInfo.m_nEquipItemID) - 1;
			}
			pkPlayer->SetEquipStarLevel(equiplevel);
			pkPlayer->SetEquipItemIDs(equipIds);
		}
	}

	if (pkPlayer != NULL)
	{
		pkPlayer->SetCharType(iCid);
		pkPlayer->SetCharName(pkPlayer->GetCharNameByID());

		int iTransformID = buddyInfo.m_nBuddyTransformID;
		if(iTransformID != pkPlayer->GetTransformID())
		{
			pkPlayer->SetTransformID(iTransformID);
			pkPlayer->ChangeVisibleData(iTransformID);
		}
		
		pkPlayer->InitElementDamageType();

		if (pkPlayer == pkGamePlayer)
			pkGamePlayer->DoUpdateWingAvatar();

		pkGamePlayer->SetAllOwner(pkGamePlayer->GetCharType(), pkGamePlayer);
		pkGamePlayer->AttachEffectToActivateBuddy(lID);
		pkGamePlayer->ShowGroundFx();
		pkGamePlayer->SwapTitles(NULL);
		pkGamePlayer->AddDirtyFlagRevisionID(DIRTY_FLAG_SHORTCUT);
	}
	else
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI, "[LHX] _handlerRefreshPlayerBindBuddy: pkPlayer==NULL");
}

void HL_FromGameServer::_handlerRefreshPlayerStarLevel(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RefreshBuddyStarLevel BuddyDataMsg;
	if(!BuddyDataMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_RefreshPlayerBindBuddy解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_RefreshPlayerBindBuddy解析错误, 没有 GamePlayer");
		return;
	}
	LC_PlayerBase* pkPlayer = NULL;
	pkPlayer = pkGamePlayer->GetBuddyPlayerBasePtr(BuddyDataMsg.m_lPlayerID);
	if (NULL != pkPlayer)
	{
		pkPlayer->SetBuddyStarLevel(BuddyDataMsg.m_nStarLevel);
	}
}

void HL_FromGameServer::_handleSyncRefreshBuddyEquipSlotLevel(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RefreshBuddyEquipSlotLevelCount BuddyDataMsg;
	if(!BuddyDataMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_RefreshPlayerBindBuddy解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	LC_PlayerBase* pkPlayer = NULL;
	pkPlayer = pkGamePlayer->GetBuddyPlayerBasePtr(BuddyDataMsg.m_lPlayerID);
	if (NULL != pkPlayer)
	{
		pkPlayer->SetAllEquipReinforceLevel(BuddyDataMsg.m_nEquipSlotLevelCount);
	}
}

void HL_FromGameServer::_handlerRefreshPlayerRuneLevel(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RefreshBuddyRuneLevel BuddyDataMsg;
	if(!BuddyDataMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_RefreshPlayerBindBuddy解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_RefreshPlayerBindBuddy解析错误, 没有 GamePlayer");
		return;
	}
	LC_PlayerBase* pkPlayer = NULL;
	pkPlayer = pkGamePlayer->GetBuddyPlayerBasePtr(BuddyDataMsg.m_lPlayerID);
	if (NULL != pkPlayer)
	{
		LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
		SC_ClientScriptInterface* pkClientInterface = SC_ClientScriptInterface::GetSingletonPtr();
		SC_ClientSkillInterface* pkInterface = pkClientInterface->GetSkillInterface();

		if(pkGamePlayer == pkPlayer || pkGamePlayer->IsBuddy(pkPlayer->GetID()))
		{
			mem::vector<int> runelevel = pkGamePlayer->GetBuddyRuneLevel(BuddyDataMsg.m_lBuddyId);
			if (BuddyDataMsg.m_nRuneLevel < 0)
				runelevel[BuddyDataMsg.m_nPosIndex] = 0;
			else
			{
				LC_ItemBase* pkItem = pkItemFactory->RequestItem(BuddyDataMsg.m_lRuneItemId);
				if (pkItem != NULL)
				{
					int runequality = pkInterface->GetRuneStarQuality(pkItem->GetItemDescriptionEntry()->GetStarUpType(),BuddyDataMsg.m_nRuneLevel);
					runelevel[BuddyDataMsg.m_nPosIndex] = runequality;
				}
				else
					runelevel[BuddyDataMsg.m_nPosIndex] = 0;
			}
			pkGamePlayer->SetBuddyRuneLevel(BuddyDataMsg.m_lBuddyId,runelevel);
		}
		else
		{
			mem::vector<int> runelevel =pkPlayer->GetRuneStarLevel();
			if (runelevel.empty())
			{
				return;
			}

			if (BuddyDataMsg.m_nRuneLevel < 0)
				runelevel[BuddyDataMsg.m_nPosIndex] = 0;
			else
			{
				LC_ItemBase* pkItem = pkItemFactory->RequestItem(BuddyDataMsg.m_lRuneItemId);
				if (pkItem != NULL)
				{
					int runequality = pkInterface->GetRuneStarQuality(pkItem->GetItemDescriptionEntry()->GetStarUpType(),BuddyDataMsg.m_nRuneLevel);
					runelevel[BuddyDataMsg.m_nPosIndex] = runequality;
				}
				else
					runelevel[BuddyDataMsg.m_nPosIndex] = 0;
			}
			pkPlayer->SetRuneStarLevel(runelevel);
		}
	}
}

void HL_FromGameServer::_handlerRefreshBuddyEquipSlotStarlevel(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RefreshBuddyEquipSlotStarLevel BuddyDataMsg;
	if(!BuddyDataMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_RefreshPlayerBindBuddy解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_RefreshPlayerBindBuddy解析错误, 没有 GamePlayer");
		return;
	}
	LC_PlayerBase* pkPlayer = NULL;
	pkPlayer = pkGamePlayer->GetBuddyPlayerBasePtr(BuddyDataMsg.m_lPlayerID);
	if (NULL != pkPlayer)
	{
		mem::vector<int> equiplevel =pkPlayer->GetEquipStarLevel();
		if (equiplevel.empty())
		{
			return;
		}
		LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
		SC_ClientScriptInterface* pkClientInterface = SC_ClientScriptInterface::GetSingletonPtr();
		SC_ClientSkillInterface* pkInterface = pkClientInterface->GetSkillInterface();

		if (BuddyDataMsg.m_nEquipSlotStarLevel < 0)
			for (int i=0;i<EQUIPSHOWORDERSIZE;i++)
			{
				if (EQUIPSHOWORDER[i] == BuddyDataMsg.m_nEquipSlotIndex)
					equiplevel[i] = 0;
			}
		else
		{
			for (int i=0;i<EQUIPSHOWORDERSIZE;i++)
			{
				if (EQUIPSHOWORDER[i] == BuddyDataMsg.m_nEquipSlotIndex)
					equiplevel[i] = pkInterface->GetEquipStarQuality(EQUIPSHOWORDER[i], BuddyDataMsg.m_nEquipSlotStarLevel);
			}
		}
		pkPlayer->SetEquipStarLevel(equiplevel);
	}
}

void HL_FromGameServer::_handlerRefreshBuddyEquipItem(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RefreshBuddyEquipItem msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_RefreshBuddyEquipItem解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_RefreshBuddyEquipItem解析错误, 没有 GamePlayer");
		return;
	}

	LC_PlayerBase* pkPlayer = pkGamePlayer->GetBuddyPlayerBasePtr(msg.m_lPlayerID);
	if (NULL != pkPlayer)
	{
		mem::vector<int> equipIds = pkPlayer->GetEquipItemIDs();
		if (equipIds.empty())
		{
			return;
		}
		
		SC_ClientScriptInterface* pkClientInterface = SC_ClientScriptInterface::GetSingletonPtr();
		SC_ClientSkillInterface* pkInterface = pkClientInterface->GetSkillInterface();

		if (msg.m_nEquipItem < 0)
		{
			for (int i=0;i<EQUIPSHOWORDERSIZE;i++)
			{
				if (EQUIPSHOWORDER[i] == msg.m_nEquipSlotIndex)
				{
					equipIds[i] = 0;
					break;
				}
			}
		}
		else
		{
			for (int i=0;i<EQUIPSHOWORDERSIZE;i++)
			{
				if (EQUIPSHOWORDER[i] == msg.m_nEquipSlotIndex)
				{
					equipIds[i] = pkInterface->GetMadeItemGrade(msg.m_nEquipItem) - 1;
					break;
				}
			}
		}
		pkPlayer->SetEquipItemIDs(equipIds);
	}
}

void HL_FromGameServer::_handlerActivateBuddyRuneSlot(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncBuddyRuneSlotState rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SyncBuddyRuneSlotState解析错误");
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SyncBuddyRuneSlotState解析错误, 没有 GamePlayer");
		return;
	}
	LC_PlayerBase* pkPlayer = NULL;
	pkPlayer = pkGamePlayer->GetBuddyPlayerBasePtr(rltMsg.m_lPlayerID);
	if (NULL != pkPlayer)
	{
		pkPlayer->SetActiveRuneSlot(rltMsg.m_nBuddyRuneSlotState);
		if(pkGamePlayer->GetMainControllerID() == rltMsg.m_lPlayerID)
		{
			pkGamePlayer->SetRuneBuddySlotOpen(rltMsg.m_nBuddyRuneSlotState);

			LC_PackAsset* pkPackAsset = pkGamePlayer->GetPackAsset();
			pkPackAsset->SetDirtyFlag(PACK_TYPE_RUNE_EQUIP_SLOT, true);
			pkPackAsset->AddDirtyFlagRevisionID(PACK_TYPE_RUNE_EQUIP_SLOT);
		}
	}
}

void HL_FromGameServer::_handlerBudyyEquipSlotState(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RefreshBuddyEquipSlotState rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_RefreshBuddyEquipSlotState解析错误");
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_RefreshBuddyEquipSlotState解析错误, 没有 GamePlayer");
		return;
	}
	LC_PlayerBase* pkPlayer = NULL;
	pkPlayer = pkGamePlayer->GetBuddyPlayerBasePtr(rltMsg.m_lPlayerID);
	if (NULL != pkPlayer)	
	{
		pkPlayer->SetActiveEquipSlot(rltMsg.m_nEquipSlotState);
	}
}

void HL_FromGameServer::_handlerResetSingleBuddyRespawnTimer(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SC_ResetSingleBuddyRespawnTimer rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SC_ResetSingleBuddyRespawnTimer解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SC_ResetSingleBuddyRespawnTimer解析错误, 没有 GamePlayer");
		return;
	}

	pkGamePlayer->ResetActiveBuddyRespawnTimeList(rltMsg);
}

void HL_FromGameServer::_handlerResetAllBuddyRespawnTimer(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SC_ResetAllBuddyRespawnTimer rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SC_ResetAllBuddyRespawnTimer解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SC_ResetSingleBuddyRespawnTimer解析错误, 没有 GamePlayer");
		return;
	}

	pkGamePlayer->ResetAllActiveBuddyRespawnTimeList();
}

void HL_FromGameServer::_handlerSetBuddyRespawnTimer(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SC_SetBuddyRespawnTimer rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SC_SetBuddyRespawnTimer解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SC_ResetSingleBuddyRespawnTimer解析错误, 没有 GamePlayer");
		return;
	}

	pkGamePlayer->UpdateActiveBuddyRespawnTimeList(rltMsg);
}


void HL_FromGameServer::_handlerBuddyChainCountSync(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SC_BuddyChainCountSync msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SC_BuddyChainCountSync解析错误");
		return;
	}

	//msg.m_nCount表示开启格子的索引
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->SetBuddyChainCid(msg.m_nCount, 0);

	LC_GameEvent GameEvent;
	GameEvent.SetEventType(GLET_SYNC_BUDDYCHAIN_INFO);
	GameEvent.SetEventParam(EVENT_BASE_PARAM_1, -1);
	UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(GameEvent);
}

void HL_FromGameServer::_handlerBuddyChainSync(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SC_BuddyChainSync msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SC_BuddyChainSync解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->SyncBuddyChain(msg);
}

void HL_FromGameServer::_handlerBuddyChainOpResult(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SC_BuddyChainOperationResult msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SC_BuddyChainOperationResult解析错误");
		return;
	}
	
	if(msg.m_nError != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_nError);
	}
}

void HL_FromGameServer::_handlerAddBuddyChainSync(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SC_AddBuddyChainSync msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SC_AddBuddyChainSync解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->UpdateBuddyChain(msg.m_nIndex, msg.m_nBuddyID);
}

void HL_FromGameServer::_handlerDelBuddyChainSync(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SC_DelBuddyChainSync msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SC_DelBuddyChainSync解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->UpdateBuddyChain(msg.m_nIndex, 0);
}

void HL_FromGameServer::_handlerSyncChainFlag(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncChainFlag msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SyncChainFlag解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->SetChainFlag(msg.m_nChainFlag);

	LC_GameEvent GameEvent;
	GameEvent.SetEventType(GLET_SYNC_BUDDYCHAIN_FLAG);

	UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(GameEvent);
}

void HL_FromGameServer::_handlerRltSetChainFlag(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_SetChainFlag msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_RLT_SetChainFlag解析错误");
		return;
	}

	if(msg.m_nResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_nResult);
		return;
	}

	LC_GameEvent GameEvent;
	GameEvent.SetEventType(GLET_SET_CHAINFLAG_SUCCESS);
	GameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_nIndex);

	UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(GameEvent);
}

void HL_FromGameServer::_handleRltBuddyEquipLv(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MGPT_RLT_EquipSlotLv msg;
	if (!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MGPT_RLT_EquipSlotLv解析错误");
		return;
	}

	SendLogicInfoMessage(msg.m_nResult);

	if(msg.m_nResult == RE_EQUIP_UPGRADE_SUCCESS)
	{
		LC_GameEvent GameEvent;
		GameEvent.SetEventType(GLET_RLT_EQUIPUPGRADE_SUCCESS);
		UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(GameEvent);
	}
}

void HL_FromGameServer::_handlerSyncRegularBossGroupInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncRegularBossGroupInfo msg;
	if (!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncRegularBossGroupInfo解析错误");
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->SetRegularBossGroupID(msg.m_nRegularBossGroupID);
	pkGamePlayer->SetRegularBossGroupCDTime(msg.m_nRegularBossGroupCDTime);

	LC_GameEvent GameEvent;
	GameEvent.SetEventType(GLET_RLT_REGULARBOSSGROUPIDCHANGE);
	UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(GameEvent);
}

void HL_FromGameServer::_handlerSyncBossKillerInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncBossKillerInfo msg;
	if (!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncBossKillerInfo解析错误");
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncBossKillerInfo错误, GamePlayer 不存在");
		return;
	}
	pkGamePlayer->SetBossKillerInfo(msg);
}