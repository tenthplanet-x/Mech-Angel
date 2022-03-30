#include "MG_BackPackProtocol.h"
#include "MG_PlaceHolderProtocol.h"
#include "MG_SyncProtocol.h"
#include "MG_BotProtocol.h"
#include "IG_ImageManager.h"
#include "LC_GuildRaidDataManager.h"
#include "LC_TitleManager.h"
#include "LC_ClientShopAsset.h"
#include "CF_ItemGen.h"
#include "CF_RuneLevelUp.h"
#include "LC_ClientActivityManager.h"
//**************  Sync skill back pack
void HL_FromGameServer::_handlerSyncSkillBackPack(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	//NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	//NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	//int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	////如果还未进入游戏，则退出
	//if (eGameStage != ST_GAME_STAGE_GAME)
	//	return;
	MG_RLT_SkillBackPack syncMsg;
	if(syncMsg.Decode(pMsg,wSize))
	{
//origin
 	//	object_id_type lPlayerID = syncMsg.m_lPlayerID;
 	//	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
 
 	//	if (!pkGamePlayer || pkGamePlayer->GetID() != lPlayerID)
 	//	{
 	//		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "技能背包消息逻辑错误");
 	//		return;
 	//	}
 
 	//	LC_SkillAsset* pkSkillAsset = pkGamePlayer->GetSkillAsset();
  //		if (NULL == pkSkillAsset)
  //		{
		//	return;
  //		}
		//LC_SkillAssetData* pkSkillAssetData = pkSkillAsset->GetSkillAssetData();
		//if (NULL == pkSkillAssetData)
		//{
		//	return;
		//}
		//pkSkillAssetData->Reset();
		//pkSkillAsset->GetDataFromProtocolStruct(syncMsg.m_kCharSkillInfo);

		//pkSkillAsset->UpdateDirtyFlag();

		////重新生成CD表
		//pkGamePlayer->BuildSkillCoolDownMap();
		//pkGamePlayer->BuildTransformSkillCoolDownMap();
		////获取技能CD
		//LC_SkillCoolDownMap* pkSkillCDMap = pkGamePlayer->GetSkillCoolDownMap();
		//if (NULL == pkSkillCDMap)
		//{
		//	return;
		//}
		//pkSkillCDMap->GetCDFromProtocol(pkSkillAsset, syncMsg.m_kCharSkillInfo);

//new
		LC_SkillAsset* pkSkillAsset = NULL;
		LC_SkillAssetData* pkSkillAssetData = NULL;
		LC_SkillCoolDownMap* pkSkillCDMap = NULL;

		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
		if(pkGamePlayer == NULL)
		{
			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "技能背包消息 没有GamePlayer");
			return;
		}

		//先对BuddyInstance的skillAsset 进行更新
		pkSkillAsset = pkGamePlayer->GetBuddySkillAsset(syncMsg.m_iBuddyID);
		if(NULL == pkSkillAsset)
		{
			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "技能背包消息 Buddy没有skillAsset");
			return;
		}

		pkSkillAssetData = pkSkillAsset->GetSkillAssetData();
		//if (NULL == pkSkillAssetData)
		//{
		//	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "技能背包消息 skillAsset没有SkillAssetData");
		//	return;
		//}
		pkSkillAssetData->Reset();
		pkSkillAsset->GetDataFromProtocolStruct(syncMsg.m_kCharSkillInfo);

		//重新生成CD表
		pkSkillAsset->BuildSkillCoolDownMap();
		//pkPlayer->BuildTransformSkillCoolDownMap();
		//获取技能CD
		pkSkillCDMap = pkSkillAsset->GetSkillCoolDownMap();
		//if (NULL == pkSkillCDMap)
		//{
		//	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "技能背包消息 player没有SkillCoolDownMap");
		//	return;
		//}
		pkSkillCDMap->GetCDFromProtocol(pkSkillAsset, syncMsg.m_kCharSkillInfo);
		//通知UI
		//pkSkillAsset = pkGamePlayer->GetSkillAsset();
		//pkSkillAsset->UpdateDirtyFlag();

		//再寻找对应的PlayerBase进行更新
		//bool bIsInstance = false;
		//bIsInstance = pkGamePlayer->IsBuddyInstance(syncMsg.m_iBuddyID);
		//if(!bIsInstance)
		//{
		//	object_id_type iOID = pkGamePlayer->GetBuddyObjectID(syncMsg.m_iBuddyID);
		//	LC_PlayerBase* pkPlayer = pkGamePlayer->GetBuddyPlayerBasePtr(iOID);

		//	if (NULL == pkPlayer)
		//	{
		//		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "技能背包消息 未找到对应player");
		//		return;
		//	}

		//	pkSkillAsset = pkPlayer->GetSkillAsset();
		//	if (NULL == pkSkillAsset)
		//	{
		//		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "技能背包消息 player没有skillAsset");
		//		return;
		//	}
		//}

		//pkSkillAssetData = pkSkillAsset->GetSkillAssetData();
		//if (NULL == pkSkillAssetData)
		//{
		//	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "技能背包消息 skillAsset没有SkillAssetData");
		//	return;
		//}
		//pkSkillAssetData->Reset();
		//pkSkillAsset->GetDataFromProtocolStruct(syncMsg.m_kCharSkillInfo);

		////重新生成CD表
		//pkSkillAsset->BuildSkillCoolDownMap();
		////pkPlayer->BuildTransformSkillCoolDownMap();
		////获取技能CD
		//pkSkillCDMap = pkSkillAsset->GetSkillCoolDownMap();
		//if (NULL == pkSkillCDMap)
		//{
		//	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "技能背包消息 player没有SkillCoolDownMap");
		//	return;
		//}
		//pkSkillCDMap->GetCDFromProtocol(pkSkillAsset, syncMsg.m_kCharSkillInfo);

		//通知UI
		//pkSkillAsset = pkGamePlayer->GetSkillAsset();
		//pkSkillAsset->UpdateDirtyFlag();
		pkGamePlayer->SetDirtyFlag(DIRTY_FLAG_SKILLASSET, true);
		pkGamePlayer->AddDirtyFlagRevisionID(DIRTY_FLAG_SKILLASSET);
	}
}

//**************** sync skill usable count, max usable count, single skill cd
void HL_FromGameServer::_handlerSkillUsableCount(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_Sync_SkillUsableCount msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_Sync_SkillUsableCount解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_Sync_SkillUsableCount解析错误, 没有 GamePlayer");
		return;
	}

	LC_SkillAsset* skillAsset = pkGamePlayer->GetBuddySkillAsset(msg.m_nBuddyId);
	if (!skillAsset)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_Sync_SkillUsableCount解析错误, skillAsset不存在 buddyID: %d", msg.m_nBuddyId);
		return;
	}

	LC_SkillAssetEntry *skillEntry = skillAsset->FindLearnedSkill(msg.m_nSkillId);
	if(skillEntry)
	{
		skillEntry->SetUsableCount(msg.m_nUsableCount);
		GfxWriteLog(LOG_TAG_SKILL, LOG_SWITCH_SKILL_EVENT,
			"[Skill]Sync Skill Usable Count, buddyID=%d, skillID=%d, usableCount=%d",
			msg.m_nBuddyId, msg.m_nSkillId, msg.m_nUsableCount);
	}
}

void HL_FromGameServer::_handlerSkillMaxUsableCount(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_Sync_SkillMaxUsableCount msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_Sync_SkillMaxUsableCount解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_Sync_SkillMaxUsableCount解析错误, 没有 GamePlayer");
		return;
	}

	LC_SkillAsset* skillAsset = pkGamePlayer->GetBuddySkillAsset(msg.m_nBuddyId);
	if (!skillAsset)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_Sync_SkillMaxUsableCount解析错误, skillAsset不存在 buddyID: %d", msg.m_nBuddyId);
		return;
	}

	LC_SkillAssetEntry *skillEntry = skillAsset->FindLearnedSkill(msg.m_nSkillId, false);
	if(skillEntry)
	{
		skillEntry->SetMaxUsableCount(msg.m_nMaxUsableCount);
		GfxWriteLog(LOG_TAG_SKILL, LOG_SWITCH_SKILL_EVENT,
			"[Skill]Sync Skill MaxUsable Count, buddyID=%d, skillID=%d, maxUsableCount=%d",
			msg.m_nBuddyId, msg.m_nSkillId, msg.m_nMaxUsableCount);
	}
}

void HL_FromGameServer::_handlerDrawCardResp(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncLotteryItem msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SyncLotteryItem解析错误");
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_Sync_SingleSkillCD解析错误, 没有 GamePlayer");
		return;
	}
	pkGamePlayer->RltSyncGamePlayerDrawCardReward(msg);
}

void HL_FromGameServer::_handlerSyncResourceRetrieveState(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncResourceRetrieveState msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SyncResourceRetrieveState解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SyncResourceRetrieveState解析错误, 没有 GamePlayer");
		return;
	}

	pkGamePlayer->SetResourceRetrieve(msg.m_bActiveState);

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_SYNC_RESOURCERETRIEVE_STATE);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerSyncCollectPlayerEquipSuit(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MGPT_SYNC_Collect_Player_Equip_Suit msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MGPT_SYNC_Collect_Player_Equip_Suit解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MGPT_SYNC_Collect_Player_Equip_Suit解析错误, 没有 GamePlayer");
		return;
	}

	pkGamePlayer->SyncCmdEquipSuitCollection(msg.m_mapCollect);

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_SYNC_COMMANDER_EQUIPSUIT_COLLECTION);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerRespCollectPlayerEquipSuit(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MGPT_RESP_Collect_Player_Equip_Suit msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MGPT_RESP_Collect_Player_Equip_Suit解析错误");
		return;
	}

	if(msg.m_nResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_nResult);
		return;
	}

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_RESP_COMMANDER_EQUIPSUIT_COLLECTION);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_uOpType);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handleRltEquipRune(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_EquipRune msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_RLT_EquipRune解析错误");
		return;
	}

	if(msg.m_iResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_iResult);
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_RLT_EquipRune解析错误, 没有 GamePlayer");
		return;
	}
	
	if(msg.m_iSrcBuddyID > 0)
	{
		LC_ClientBuddyInstance* pkSrcInst = pkGamePlayer->GetBuddyInstance(msg.m_iSrcBuddyID);
		if(pkSrcInst != NULL)
		{
			pkSrcInst->UpdateRuneEquip(msg.m_iSrcIndex, msg.m_SrcSlotData);
		}
	}

	LC_ClientBuddyInstance* pkDestInst = pkGamePlayer->GetBuddyInstance(msg.m_iDestBuddyID);
	if(pkDestInst != NULL)
	{
		pkDestInst->UpdateRuneEquip(msg.m_iDestIndex, msg.m_DestSlotData);
	}
	
	pkGamePlayer->OnRuneUpdated();

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_RUNE_EQUIP_SUCCESS);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_iDestBuddyID);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, msg.m_iDestIndex);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handleRltOnekeyEquipRune(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_OneKeyEquipRune msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_RLT_OneKeyEquipRune解析错误");
		return;
	}

	if(msg.m_iResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_iResult);
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_RLT_OneKeyEquipRune解析错误, 没有 GamePlayer");
		return;
	}

	
	LC_ClientBuddyInstance* pkInst = pkGamePlayer->GetBuddyInstance(msg.m_iBuddyID);
	if(pkInst != NULL)
	{
		std::map<int32_t,PS_ItemAssetEntry>::const_iterator iter = msg.m_DestSlotData.begin();
		for( ; iter!=msg.m_DestSlotData.end(); ++iter)
		{
			pkInst->UpdateRuneEquip(iter->first, iter->second);
		}

		pkGamePlayer->OnRuneUpdated();
	}

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_RUNE_EQUIP_SUCCESS);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, -1);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handleRltUnEquipRune(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_UnEquipRune msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_RLT_UnEquipRune解析错误");
		return;
	}

	if(msg.m_iResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_iResult);
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_RLT_EquipRune解析错误, 没有 GamePlayer");
		return;
	}
	
	LC_ClientBuddyInstance* pkInst = pkGamePlayer->GetBuddyInstance(msg.m_iBuddyID);
	if(pkInst != NULL)
	{
		pkInst->ClearRuneEquipEntry(msg.m_iSrcSlotIndex);

		pkGamePlayer->OnRuneUpdated();
	}

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_RUNE_UNEQUIP_SUCCESS);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handleRltOnekeyUnEquipRune(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_OneKeyUnEquipRune msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_RLT_OneKeyUnEquipRune解析错误");
		return;
	}

	if(msg.m_iResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_iResult);
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_RLT_OneKeyEquipRune解析错误, 没有 GamePlayer");
		return;
	}
	
	LC_ClientBuddyInstance* pkInst = pkGamePlayer->GetBuddyInstance(msg.m_iBuddyID);
	if(pkInst != NULL)
	{
		std::set<int32_t>::const_iterator iter = msg.m_SlotIndexVec.begin();
		for( ; iter!=msg.m_SlotIndexVec.end(); ++iter)
		{
			pkInst->ClearRuneEquipEntry(*iter);
		}

		pkGamePlayer->OnRuneUpdated();
	}

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_RUNE_UNEQUIP_SUCCESS);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handleRltLevelUpRune(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_RuneUpgrade msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_RLT_RuneUpgrade解析错误");
		return;
	}
	
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_RLT_RuneUpgrade解析错误, 没有 GamePlayer");
		return;
	}

	LC_PackAsset *pkPackAsset = pkGamePlayer->GetPackAsset();

	LC_ClientBuddyInstance* pkInstance = NULL;
	bool bBag = msg.m_iBuddyID == 0;
	if(!bBag)
	{
		pkInstance = pkGamePlayer->GetBuddyInstance(msg.m_iBuddyID);
		if(pkInstance == NULL)
		{
			GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_RLT_RuneUpgrade解析错误, 没有 Buddy buddyID=%d", msg.m_iBuddyID);
			return;
		}
	}
	
	if(msg.m_iResult == RE_CONTROLLER_LEVEL_NOT_ENOUGH_RUNE)
	{
		LC_BackPackEntry* pkEntry = NULL;
		if(bBag)
			pkEntry = pkPackAsset->GetBackPackEntry(PACK_TYPE_BASIC, msg.m_iSlotIndex);
		else
			pkEntry = pkInstance->GetRuneEquipEntry(msg.m_iSlotIndex);

		T_ASSERT_CRITICAL_S(pkEntry != NULL);

		CF_RuneLevelUp::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_RuneLevelUp>(pkEntry->GetEquipLevel()+1);
		if(pkData != NULL)
		{
			SendLogicInfoMessage(msg.m_iResult, pkData->_iCommanderLevel);
		}
	}
	else
	{
		if(msg.m_iResult == RE_SUCCESS)
		{
			if(!bBag)
			{
				pkInstance->UpdateRuneEquip(msg.m_iSlotIndex, msg.m_SlotData);
				pkGamePlayer->OnRuneUpdated();
			}

			LC_GameEvent kGameEvent;
			kGameEvent.SetEventType(GLET_RUNE_LEVELUP_SUCCESS);
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_iBuddyID);
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, msg.m_iSlotIndex);

			UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
			pkUIManager->HandlerGameEvent(kGameEvent);
		}
		else
		{
			SendLogicInfoMessage(msg.m_iResult);
		}
	}
}

void HL_FromGameServer::_handleRltStarUpRune(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_RuneStar msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_RLT_RuneStar解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_RLT_RuneUpgrade解析错误, 没有 GamePlayer");
		return;
	}

	if(msg.m_iResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_iResult);
	}
	else
	{
		if(msg.m_iBuddyID > 0)
		{
			LC_ClientBuddyInstance* pkInstance = pkGamePlayer->GetBuddyInstance(msg.m_iBuddyID);
			if(pkInstance != NULL)
			{
				pkInstance->UpdateRuneEquip(msg.m_iSlotIndex, msg.m_SlotData);
			}

			pkGamePlayer->OnRuneUpdated();
		}
	}
	
	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_RUNE_STARUP_RLT);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_iResult);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, msg.m_iBuddyID);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_3, msg.m_iSlotIndex);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handleRltResetRune(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_ResetRune msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_RLT_ResetRune解析错误");
		return;
	}

	if(msg.m_iResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_iResult);
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_RLT_RuneUpgrade解析错误, 没有 GamePlayer");
		return;
	}

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_RUNE_RESET_SUCCESS);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_iBuddyID);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, msg.m_iSlotIndex);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);


	if(msg.m_iBuddyID > 0)
	{
		LC_ClientBuddyInstance* pkInstance = pkGamePlayer->GetBuddyInstance(msg.m_iBuddyID);
		if(pkInstance != NULL)
		{
			pkInstance->UpdateRuneEquip(msg.m_iSlotIndex, msg.m_SlotData);

			pkGamePlayer->OnRuneUpdated();
		}
	}
}

void HL_FromGameServer::_handlerSkillSingleCD(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_Sync_SingleSkillCD msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_Sync_SingleSkillCD解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_Sync_SingleSkillCD解析错误, 没有 GamePlayer");
		return;
	}

	LC_SkillAsset* skillAsset = pkGamePlayer->GetBuddySkillAsset(msg.m_nBuddyId);
	if (!skillAsset)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_Sync_SingleSkillCD解析错误, skillAsset不存在 buddyID: %d", msg.m_nBuddyId);
		return;
	}


	LC_SkillCoolDownMap* cooldownMap = skillAsset->GetSkillCoolDownMap();
	//if (cooldownMap)
	{
		cooldownMap->SetSkillCD(msg.m_nSkillId, msg.m_nCDTime/1000);
		GfxWriteLog(LOG_TAG_SKILL, LOG_SWITCH_SKILL_EVENT,
			"[Skill]Sync Skill CD, buddyID=%d, skillID=%d, CDTime=%d",
			msg.m_nBuddyId, msg.m_nSkillId, msg.m_nCDTime);
	}
}

void HL_FromGameServer::_handlerSkillSingleLogicValue(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_Sync_SingleSkillLogicValue msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_Sync_SingleSkillLogicValue解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_Sync_SingleSkillLogicValue解析错误, 没有 GamePlayer");
		return;
	}

	LC_SkillAsset* skillAsset = pkGamePlayer->GetBuddySkillAsset(msg.m_nBuddyId);
	if (!skillAsset)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_Sync_SingleSkillLogicValue解析错误, skillAsset不存在 buddyID: %d", msg.m_nBuddyId);
		return;
	}

	LC_SkillAssetEntry *skillEntry = skillAsset->FindLearnedSkill(msg.m_nSkillId, false);
	if(skillEntry)
	{
		skillEntry->SetLogicValue(1);
		
	}
	pkGamePlayer->SetDirtyFlag(DIRTY_FLAG_SHORTCUT, true);
	pkGamePlayer->AddDirtyFlagRevisionID(DIRTY_FLAG_SHORTCUT);
	GfxWriteLog(LOG_TAG_SKILL, LOG_SWITCH_SKILL_EVENT,
		"[Skill]Sync Skill LogicValue, buddyID=%d, skillID=%d, logicValue=%d",
		msg.m_nBuddyId, msg.m_nSkillId, skillEntry?1:0);
}

void HL_FromGameServer::_handlerSyncOwnerMPInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncCommanderMP msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MGPT_SYNC_OWNER_MP_INFO解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MGPT_SYNC_OWNER_MP_INFO解析错误, 没有 GamePlayer");
		return;
	}
	

	if (NULL != pkGamePlayer->GetCommander())
	{
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_SYNC_COMMANDER_MP);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_nCur);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, msg.m_nMax);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);

		pkGamePlayer->GetCommander()->SetCurMP(msg.m_nCur);
		pkGamePlayer->GetCommander()->SetMaxMP(msg.m_nMax);
	}
	else
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MGPT_SYNC_OWNER_MP_INFO解析错误, 没有 GamePlayerCommander");
		return;
	}

	GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,
		"[Skill]Sync MP Info, cur=%d, max=%d",
		msg.m_nCur, msg.m_nMax);
}

//***** 技能[符文]回复
void HL_FromGameServer::_handlerRltActivateSkill(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_Reply_ActivateSkill msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_Reply_ActivateSkill解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_Reply_ActivateSkill解析错误, 没有 GamePlayer");
		return;
	}

	int32_t errCode = msg.m_nError;
	int32_t errParam = msg.m_nParam;
	int32_t errParam2 = msg.m_nParam2;

	switch(errCode)
	{
	case RE_SKILL_ACTIVATE_SUCCESS:
		{
			const CF_Buddy::DataEntry* pkData  = CF_Buddy::GetSingleton().GetEntryPtr(errParam);
			if (NULL != pkData)
			{
				SendLogicInfoMessage(errCode, pkData->_roleName.c_str());
			}

			LC_GameEvent kGameEvent;
			kGameEvent.SetEventType(GLET_SKILL_AWAKE);
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, errParam);
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, errParam2);

			UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
			pkUIManager->HandlerGameEvent(kGameEvent);
		}
		break;
	case RE_SKILL_ACTIVATE_NO_BUDDY:
		{
			const CF_Buddy::DataEntry* pkData  = CF_Buddy::GetSingleton().GetEntryPtr(errParam);
			if (NULL != pkData)
			{
				SendLogicInfoMessage(errCode, pkData->_roleName.c_str());
			}
		}
		break;
	case RE_SKILL_ACTIVATE_ALREADY_DONE:
		{
			const CF_SkillExecuteList::DataEntry* pkData  = CF_SkillExecuteList::GetSingleton().GetEntryPtr(errParam);
			if (NULL != pkData)
			{
				SendLogicInfoMessage(errCode, pkData->_kName.c_str());
			}
		}
		break;
	case RE_SKILL_ACTIVATE_NO_SKILL:
	case RE_SKILL_ACTIVATE_SYSTEM:
		SendLogicInfoMessage(errCode);
		break;
	default:
		break;
	}
}

//------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncSkillCD( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	// 	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	// 	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	// 	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();
	// 
	// 	//如果还未进入游戏，则退出
	// 	if (eGameStage != ST_GAME_STAGE_GAME)
	// 		return;

// 	MG_SyncSkillCD syncMsg;
// 	if (!syncMsg.Decode(pMsg, wSize))
// 	{
// 		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"MG_SyncSkillCD 解析错误");
// 		return;
// 	}
// 	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
// 	if(!pkGamePlayer)
// 	{
// 		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"MG_SyncSkillCD 解析错误, GamePlayer有误");
// 		return;
// 	}
// 	LC_SkillAsset* pkSkillAsset = pkGamePlayer->GetSkillAsset();
// 	if (NULL == pkSkillAsset)
// 	{
// 		return;
// 	}
// 
// 	PS_SkillCDInfo& rkSkillCDInfo = syncMsg.m_kSkillCDInfo;
// 	pkGamePlayer->GetSkillCoolDownMap()->GetCDFromProtocol(pkSkillAsset, rkSkillCDInfo);
}

//------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncSkillCDModify(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncSkillCDModify syncMsg;
	if (!syncMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"MG_SyncSkillCDModify 解析错误");
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(!pkGamePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"MG_SyncSkillCDModify 解析错误, GamePlayer有误");
		return;
	}
	
	pkGamePlayer->GetSkillCoolDownMap()->GetCDModifyFromProtocol(syncMsg.m_kSkillCDModify);
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncItemCD( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	// 	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	// 	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	// 	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();
	// 
	// 	//如果还未进入游戏，则退出
	// 	if (eGameStage != ST_GAME_STAGE_GAME)
	// 		return;

	MG_SyncItemCD syncMsg;
	if (!syncMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"MG_SyncItemCD 解析错误");
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(!pkGamePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"MG_SyncItemCD 解析错误, GamePlayer有误");
		return;
	}

	PS_ItemCDInfo& rkItemCDInfo = syncMsg.m_kItemCDInfo;
	pkGamePlayer->GetControllerItemCoolDownMap()->GetCDFromProtocol(pkGamePlayer, rkItemCDInfo);
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncTutorialAsset(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	//NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	//NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	//int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	////如果还未进入游戏，则退出
	//if (eGameStage != ST_GAME_STAGE_GAME)
	//	return;

	MG_SyncTutorialAsset msg;
	if(msg.Decode(pMsg,wSize))
	{
		object_id_type lPlayerID			= msg.m_lPlayerID;

		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

		if(!pkGamePlayer || pkGamePlayer->GetMainControllerID() != lPlayerID)
		{
			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"新手资产同步消息逻辑错误");
			return;
		}

		pkGamePlayer->GetTutorialAsset()->SetDataBuff(msg.m_byTutorialData, MG_TUTORIALDATA_LENGTH);

		pkGamePlayer->GetTutorialAsset()->SetDisplayFlag(true);

		pkGamePlayer->GetTutorialAsset()->AddDirtyFlagRevisionID();

		pkGamePlayer->SetIsCanStartTutorialFlag(true);

		//pkGamePlayer->SetBitVec(0, true);	//设置强制设置视频已播放的标志位，程序运行到这里，前面肯定放过视频了，该函数可重入，不会重复发送消息

		//存在 一战成名，开始旅程的初始UI ， 需要重置进入自动挂机的时间
		if( !pkGamePlayer->GetTutorialAsset()->GetBitVec(0) )
		{
			pkGamePlayer->Reset2GudingFirset();
		}
	}
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncSkillState(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack)
{
	//NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	//NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	//int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	////如果还未进入游戏，则退出
	//if (eGameStage != ST_GAME_STAGE_GAME)
	//	return;


	MG_SyncSkillStates syncMsg;
	if(!syncMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"MG_SyncSkillStates 解析错误");
		return;
	}

	object_id_type lPlayerID = syncMsg.m_value.id();
	if (bArenaPlayBack)
	{
		LC_ClientArenaPlayBackManager *pkArenaPlayBackManager = LC_ClientArenaPlayBackManager::GetSingletonPtr();
		lPlayerID = pkArenaPlayBackManager->GetTemID(lPlayerID);
	}
	LC_PlayerBase* pkPlayer = pkSession->GetPlayer();

	if (!pkPlayer || pkPlayer->GetID() != lPlayerID)
	{
		LC_ClientNetPlayerManager* clientNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
		pkPlayer = clientNetPlayerManager->FindPlayer(lPlayerID);
		if(pkPlayer == NULL)
		{
			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "MG_SyncSkillStates逻辑错误");
			return;
		}
	}

	LC_ActorStateMap* pkActorStateMap = pkPlayer->GetStateMap();
	pkActorStateMap->BuildEffectKeeper(syncMsg.m_value);
	pkPlayer->EndSkillStateGFX();
	pkActorStateMap->GetFromProtocol(syncMsg.m_value);
	pkPlayer->BeginSkillStateGFX();
	pkActorStateMap->ClearEffectKeeper();

	//MG_MessageCenter* pkMsgCenter = MG_MessageCenter::GetSingletonPtr();
	//pkMsgCenter->SendMessage(CreateMsg_UI_UPDATE(DIRTY_FLAG_SKILLSTATE));		//UI测试
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncPlayerSociety(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_SyncPlayerSociety msg;
	if(msg.Decode(pMsg,wSize))
	{
		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
		GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM,"[数据][收] MGPT_SYNC_PLAYER_SOCIETY flag=%d",msg.m_bLoadFromSocietyFlag);

		if(!pkGamePlayer)
		{
			GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM,"[数据][收] MGPT_SYNC_PLAYER_SOCIETY 没有gameplayer");
			return;
		}
		pkGamePlayer->SetCharMood(STDSTR2TPSTR(msg.m_CharMood));
		pkGamePlayer->SetGuildID(msg.m_uiGuildID);
		pkGamePlayer->SetbLoadFromSocietyFlag(msg.m_bLoadFromSocietyFlag);
		//pkGamePlayer->SetGuildPoint(msg.m_GuildPoint);
		pkGamePlayer->SetGuildMoney(msg.m_GuildMoney);
		pkGamePlayer->SetGuildName(STDSTR2TPSTR(msg.m_strGuildName));
		pkGamePlayer->SetActiveServer(msg.m_ActiveServer);
		pkGamePlayer->SetGuildGroup(msg.m_GuildGroup);
		pkGamePlayer->SetGuildAutoAdd(msg.m_GuildAutoAdd);
		pkGamePlayer->SetCouragePoint(msg.m_CouragePoint);
		if(msg.m_uiGuildID == INVALID_GUILD_ID && LC_GuildRaidDataManager::GetSingletonPtr())
		{
			LC_GuildRaidDataManager::GetSingleton().ReUse();
			LC_ClientGuildProfitManager::GetSingleton().ReUse();
		}

		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_SOCIETY_ALREADY);
		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncPlayerProperty(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack)
{
	//NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	//NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	//int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	////如果还未进入游戏，则退出
	//if (eGameStage != ST_GAME_STAGE_GAME)
	//	return;


#pragma TODO("消息调整需要修改")
	LC_ClientArenaPlayBackManager *pkArenaPlayBackManager = LC_ClientArenaPlayBackManager::GetSingletonPtr();
	MG_SyncPlayerProperty msg;
	if(msg.Decode(pMsg,wSize))
	{
		object_id_type lPlayerID	= msg.m_lPlayerID;
		if (bArenaPlayBack)
			lPlayerID				= pkArenaPlayBackManager->GetTemID(lPlayerID);
		int	cID						= msg.m_lBuddyID;
		attr_value_type  iHP		= msg.m_iHP;
		int  iMP					= msg.m_iMP;
		int64_t  iEXP				= msg.m_iEXP;
		int  iLevel					= msg.m_iLevel;
		int64_t llSkillFlag			= msg.m_llSkillFlag;
		//long lActionFlag		= msg.m_lActionFlag;
		//bool bIsShowFashion		= msg.m_bIsShowFashion;

		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

		if(pkGamePlayer == NULL)
		{
			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"角色属性同步消息逻辑错误");
			return;
		}

		//if(pkGamePlayer->GetDirtyFlagRevisionID(LC_ActorBase::DIRTY_FLAG_PROPERTY) <= lRevisionID)
		//{	
/*
			bool bDead = pkGamePlayer->GetDead();
			pkGamePlayer->SetHP(iHP);
			pkGamePlayer->SetMP(iMP);
			//pkGamePlayer->SetLevel(iLevel);
			//pkGamePlayer->SetEXP(iEXP);
			pkGamePlayer->SetAllSkillFlag(llSkillFlag);
			//pkGamePlayer->SetAllActionFlag(lActionFlag);
			pkGamePlayer->SetSkillLevelupPoints(msg.m_SkillLevelupPoint);
			pkGamePlayer->SetSitRecoverLevel(msg.m_SitRecoverLevel);
			pkGamePlayer->SetCurrentSitRecoverSkillPoint(msg.m_CurrentSkillLevelupPoint);
			pkGamePlayer->SetCurrentSitRecoverVipSkillPoint(msg.m_CurrentVipSkillLevelupPoint);
			pkGamePlayer->SetCurrentSitRecoverExp(msg.m_CurrentSitExp);
			pkGamePlayer->SetCurrentSitRecoverExpVip(msg.m_CurrentSitVipExp);
			pkGamePlayer->SetVigor(msg.m_Vigor);
			pkGamePlayer->SetStamina(msg.m_Stamine);
//			pkGamePlayer->SetBowAmount(msg.m_BowAmount);
			
//			pkGamePlayer->SetSummorFriendCharID(msg.m_summorFriendCharID);
			pkGamePlayer->SetPlaceCanPK(msg.m_PlaceCanPk);

			pkGamePlayer->SetFactionID(msg.m_FactionId);
//			pkGamePlayer->SetVehicleBufferSeqData(msg.m_VehicleBufferSeq);
			
			pkGamePlayer->SetRiderCharType(msg.m_lRiderCharType);
			pkGamePlayer->SetRegTime(msg.m_CharCreateTime);
			
			pkGamePlayer->SetServerID(msg.m_iServerID);
			
			
			pkGamePlayer->SetPairSitRecoverID(msg.m_PairSitRecoverID);
			pkGamePlayer->SetJumpDodgePoint(msg.m_JumpDodgePoint);
			
			LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
			if (NULL != pkLogicManager)
			{
				if (pkGamePlayer->GetSkillFlag(SK_FREEZE_ANIMATION) && !pkGamePlayer->GetAnimationFreezed())
				{ 
					pkLogicManager->FreezeAnimation(pkGamePlayer);
				}
				else if (!pkGamePlayer->GetSkillFlag(SK_FREEZE_ANIMATION) && pkGamePlayer->GetAnimationFreezed())
				{
					pkLogicManager->UnFreezeAnimation(pkGamePlayer);
				}
			}

			for(PS_PlatformInfo::iterator it = msg.m_Platform.begin(); it != msg.m_Platform.end(); ++it)
			{
				StringType sType = STDSTR2TPSTR(it->first);
				platform_info* pkDetail = pkGamePlayer->GetPlatformInfo(sType);
				if(!pkDetail)
					pkGamePlayer->AddPlatformInfo(sType, it->second);
				else
					pkDetail->decode_vip(sType, it->second);
			}

			if (bDead != pkGamePlayer->GetDead() && bDead == false)
				pkGamePlayer->SetNPCFunctionType(NFT_NONE);	

			pkGamePlayer->PostPlayerEnterGame();

			pkGamePlayer->SetIsShowFashion(msg.m_bIsShowFashion);
			pkGamePlayer->UpdateSpecialEquipData();
/*/
			LC_PlayerBase* pkPlayer = pkGamePlayer->GetBuddyPlayerBasePtr(lPlayerID);
			if (pkPlayer == NULL)
				return;
			pkPlayer->SetHP(iHP);
			pkPlayer->SetMP(iMP);
			//pkGamePlayer->SetLevel(iLevel);
			//pkGamePlayer->SetEXP(iEXP);
			pkPlayer->SetAllSkillFlag(llSkillFlag);
			//pkGamePlayer->SetAllActionFlag(lActionFlag);
			pkPlayer->SetSkillLevelupPoints(msg.m_SkillLevelupPoint);
			pkPlayer->SetSitRecoverLevel(msg.m_SitRecoverLevel);
			pkPlayer->SetCurrentSitRecoverSkillPoint(msg.m_CurrentSkillLevelupPoint);
			pkPlayer->SetCurrentSitRecoverVipSkillPoint(msg.m_CurrentVipSkillLevelupPoint);
			pkPlayer->SetCurrentSitRecoverExp(msg.m_CurrentSitExp);
			pkPlayer->SetCurrentSitRecoverExpVip(msg.m_CurrentSitVipExp);
			pkPlayer->SetVigor(msg.m_Vigor);
			pkPlayer->SetStamina(msg.m_Stamine);
			pkPlayer->SetCombateScore(msg.m_iCombatScore);
			//			pkGamePlayer->SetBowAmount(msg.m_BowAmount);

			//			pkGamePlayer->SetSummorFriendCharID(msg.m_summorFriendCharID);
			pkPlayer->SetPlaceCanPK(msg.m_PlaceCanPk);

			pkPlayer->SetFactionID(msg.m_FactionId);
			//			pkGamePlayer->SetVehicleBufferSeqData(msg.m_VehicleBufferSeq);

			pkPlayer->SetRiderCharType(msg.m_lRiderCharType);
			pkPlayer->SetServerID(msg.m_iServerID);


			pkPlayer->SetPairSitRecoverID(msg.m_PairSitRecoverID);
			pkPlayer->SetJumpDodgePoint(msg.m_JumpDodgePoint);

			LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
			if (NULL != pkLogicManager)
			{
				if (pkPlayer->GetSkillFlag(SK_FREEZE_ANIMATION) && !pkPlayer->GetAnimationFreezed())
				{ 
					pkLogicManager->FreezeAnimation(pkPlayer);
				}
				else if (!pkPlayer->GetSkillFlag(SK_FREEZE_ANIMATION) && pkPlayer->GetAnimationFreezed())
				{
					pkLogicManager->UnFreezeAnimation(pkPlayer);
				}
			}

			if (pkPlayer == pkGamePlayer)
			{
				for(PS_PlatformInfo::iterator it = msg.m_Platform.begin(); it != msg.m_Platform.end(); ++it)
				{
					StringType sType = STDSTR2TPSTR(it->first);
					platform_info* pkDetail = pkGamePlayer->GetPlatformInfo(sType);
					if(!pkDetail)
						pkGamePlayer->AddPlatformInfo(sType, it->second);
					else
						pkDetail->decode_vip(sType, it->second);
				}

				pkGamePlayer->PostPlayerEnterGame();
				pkGamePlayer->SetIsShowFashion(msg.m_bIsShowFashion);
				pkGamePlayer->UpdateSpecialEquipData();
			}
//*/		
			pkGamePlayer->UpdateDirtyFlag(DIRTY_FLAG_PROPERTY);
		//}
	}
	else
	{
		SendLogicInfoMessage(RE_PLAYER_PROPERTY_MSG_DECODE_FAIL);
	}
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncPlayerAttribute(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	//NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	//NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	//int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	////如果还未进入游戏，则退出
	//if (eGameStage != ST_GAME_STAGE_GAME)
	//	return;
//origin
	//MG_SyncPlayerAttribute msg;
	//if(msg.Decode(pMsg,wSize))
	//{
	//	object_id_type lPlayerID		= msg.m_value.id();
	//	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

	//	if(!pkGamePlayer || pkGamePlayer->GetID() != lPlayerID)
	//	{
	//		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"角色参数同步消息逻辑错误");
	//		return;
	//	}

	//	//if(pkGamePlayer->GetDirtyFlagRevisionID(LC_ActorBase::DIRTY_FLAG_ATTRIBUTE) <= lRevisionID)
	//	{
	//		LC_ActorAttributeMap* pkAttributeMap = pkGamePlayer->GetAttributeMap();
	//		pkAttributeMap->RecomputeAll();

	//		LC_ActorAttributeMap tmpMap = *pkAttributeMap;
	//		pkAttributeMap->Clear();

	//		int iSize = sizeof(ATTRIB_SHOWTYPE_TO_ATTRIBTYPE)/sizeof(int);
	//		bool bInitAttrib = pkGamePlayer->GetIsInitAttribute();
	//		for( int i = 0; i < msg.m_value.attrs_size() ; ++i )
	//		{
	//			int attrType = msg.m_value.attrs(i).attr();
	//			pkAttributeMap->SetAttributeBase (attrType,msg.m_value.attrs(i).value());
	//			pkAttributeMap->AddAttributePercentFunc(attrType,msg.m_value.attrs(i).percent());

	//			if(bInitAttrib && attrType >= 0 && attrType < iSize)
	//			{
	//				pkAttributeMap->RecomputeAttribute(attrType);
	//				int val = int(pkAttributeMap->GetAttribute(attrType) - tmpMap.GetAttribute(attrType));
	//				if(val > 0 && ATTRIB_SHOWTYPE_TO_ATTRIBTYPE[attrType] > 0)
	//				{
	//					pkGamePlayer->SendMessage( CreateMsg_UE_DELTA_ATTRI(pkGamePlayer->GetID(), ATTRIB_SHOWTYPE_TO_ATTRIBTYPE[attrType], val, 0));
	//				}
	//			}
	//		}

	//		pkGamePlayer->SetInitAttribute(true);
	//		pkGamePlayer->GetAttributeAppendMap()->GetDataFromPB( msg.m_value.attr_pluss() );
	//		pkAttributeMap->RecomputeAll();

	//		pkGamePlayer->FixMaxHPMP();

	//		pkGamePlayer->SetCareerType((CareerType)msg.m_value.career());
	//		pkGamePlayer->SetWeaponAttribute((WeaponType)msg.m_value.weapon());
	//		//pkGamePlayer->SetPlayerType(msg.m_lPlayerType);
	//		pkGamePlayer->SetDirtyFlag(DIRTY_FLAG_ATTRIBUTE, true);
	//		pkGamePlayer->AddDirtyFlagRevisionID(DIRTY_FLAG_ATTRIBUTE);
	//	}

	//}
//new
	MG_SyncPlayerAttribute msg;
	if(msg.Decode(pMsg,wSize))
	{
		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
		if(pkGamePlayer == NULL)
		{
			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"MG_SyncPlayerAttribute解析错误, 没有 GamePlayer");
			return;
		}

		LC_ActorAttributeMap* pkAttributeMap = NULL;
		LC_ActorAttributeAppendMap* pkAttributeAppendMap = NULL;

		uint32_t cID = msg.m_value.buddy();
		uint32_t iType = msg.m_value.usynctype();
		int64_t iCombatScore = msg.m_value.ucombatscore();
		int64_t iCommanderEquipScore = msg.m_value.ucommanderequipscore();
		bool bLogin = msg.m_value.login();

		pkGamePlayer->UpdateCommanderEquipScore(iCommanderEquipScore);
		bool bSub = false;
		bool bBuddy = false;
		switch(iType)
		{
		case SPAT_BuddyAllAttr:
			{
				pkAttributeMap = pkGamePlayer->GetBuddyAttributeMap(cID);
				pkAttributeAppendMap = pkGamePlayer->GetBuddyAttributeAppendMap(cID);

				LC_ClientBuddyInstance* pkBuddyInstance = pkGamePlayer->GetBuddyInstance(cID);
				if(pkBuddyInstance != NULL)
				{
					pkBuddyInstance->SetCombateScore(iCombatScore);

					if(!bLogin)
					{
						LC_GameEvent kGameEvent;
						kGameEvent.SetEventType(GLET_BUDDY_SCORE_CHANGED);
						UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
						pkUIManager->HandlerGameEvent(kGameEvent);
					}
				}

				bBuddy = true;
			}
			break;
		case SPAT_CommanderAllAttr:
			{
				pkAttributeMap = pkGamePlayer->GetCommanderAttributeMap();
				pkAttributeAppendMap = pkGamePlayer->GetCommanderAttributeAppendMap();
				pkGamePlayer->CommanderSetTotalCombateScore(iCombatScore);
			}
			break;
		case SPAT_CoreAllAttr:
			{
				pkAttributeMap = pkGamePlayer->GetCommanderChainAttributeMap();
				pkGamePlayer->UpdateChainCombatScore(iCombatScore);
			}
			break;
		case SPAT_VehicleAttr:
		case SPAT_DevaAllAttr:
		case SPAT_TpMuhun:
		case SPAT_TaliSman:
			{
				int iSubTypes[] = {LC_SUB_TP_VEHICLE, LC_SUB_TP_DEVA_EYE, LC_SUB_TP_MUHUN, LC_SUB_TP_TALISMAN};
				LC_SubUpgradeData* pkSubData = pkGamePlayer->GetSubordinateAsset().GetSubData(iSubTypes[iType - SPAT_VehicleAttr]);
				if(pkSubData == NULL)
					return;

				bSub = true;
				pkAttributeMap = pkSubData->GetAttributeMap();
				pkSubData->SetCombatScore(iCombatScore);
			}
			break;
		default:
			T_ASSERT_CRITICAL_S(false);
			break;
		}
		
		if (pkAttributeMap != NULL)
		{
			bool bNotify = !bLogin
							&& (iType == SPAT_CommanderAllAttr 
							|| iType == SPAT_CoreAllAttr
							|| iType == SPAT_BuddyAllAttr);

			StringStreamType sStream;
			
			LC_ActorAttributeMap attrMap;
			attrMap.CopyFrom(pkAttributeMap);

			pkAttributeMap->Clear();
			for( int i = 0; i < msg.m_value.attrs_size() ; ++i )
			{
				int attrType = msg.m_value.attrs(i).attr();

				attr_value_type iOldValue = attrMap.GetAttribute(attrType);

				const msg_define::actor_attribute_entry& attr = msg.m_value.attrs(i);
				pkAttributeMap->SetAttributeBase (attrType, attr.value());
				pkAttributeMap->AddAttributePercentFunc(attrType, attr.percent());
				pkAttributeMap->RecomputeAttribute(attrType);

				if(bNotify)
				{
					attr_value_type iNewValue = pkAttributeMap->GetAttribute(attrType);
					attr_value_type iDelta = iNewValue - iOldValue;
					if(iDelta > 0)
					{
						sStream << attrType << '#' << iDelta << '|';
					}
				}
			}

			if(bNotify && sStream.rdbuf()->in_avail() > 0)
			{
				uint32_t iReason = msg.m_value.reasontype();
				sStream << '@' << iType << '@' << cID << '@' << iReason;

				LC_ClientGameEvent kGameEvent;
				kGameEvent.SetClientEventType(GameLogic::CGET_MSG_ATTRIB_CHANGE);
				LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(sStream.str());
				kGameEvent.SetUserData(&kUserData);

				UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
				pkUIManager->HandlerGameEvent(kGameEvent);
			}
		}

		if(pkAttributeAppendMap != NULL)
		{
			pkAttributeAppendMap->GetDataFromPB( msg.m_value.attr_pluss() );
		} 

//更新出战角色
		//object_id_type oID = msg.m_value.id() > 0 ? msg.m_value.id() : pkGamePlayer->GetBuddyObjectID(cID);

		if(SPAT_BuddyAllAttr == iType)
		{
			object_id_type oID = pkGamePlayer->GetBuddyObjectID(cID);
			LC_PlayerBase* pkPlayer = pkGamePlayer->GetBuddyPlayerBasePtr(oID);

			if (pkPlayer != NULL)
			{
				pkAttributeMap = pkPlayer->GetAttributeMap();
				pkAttributeMap->RecomputeAll();

				LC_ActorAttributeMap tmpMap = *pkAttributeMap;
				pkAttributeMap->Clear();

				int iSize = sizeof(ATTRIB_SHOWTYPE_TO_ATTRIBTYPE)/sizeof(int);
				bool bInitAttrib = pkPlayer->GetIsInitAttribute();
				for( int i = 0; i < msg.m_value.attrs_size() ; ++i )
				{
					int attrType = msg.m_value.attrs(i).attr();
					pkAttributeMap->SetAttributeBase (attrType,msg.m_value.attrs(i).value());
					pkAttributeMap->AddAttributePercentFunc(attrType,msg.m_value.attrs(i).percent());

					/*if(bInitAttrib && attrType >= 0 && attrType < iSize)
					{
						pkAttributeMap->RecomputeAttribute(attrType);
						if (oID == pkGamePlayer->GetID())
						{
							int val = int(pkAttributeMap->GetAttribute(attrType) - tmpMap.GetAttribute(attrType));
							if(val > 0 && ATTRIB_SHOWTYPE_TO_ATTRIBTYPE[attrType] > 0)
							{
								pkGamePlayer->SendMessage( CreateMsg_UE_DELTA_ATTRI(pkGamePlayer->GetID(), ATTRIB_SHOWTYPE_TO_ATTRIBTYPE[attrType], val, 0));
							}
						}
					}*/
				}
				pkPlayer->SetInitAttribute(true);
				pkAttributeMap->RecomputeAll();

				pkPlayer->FixMaxHPMP();
				pkPlayer->SetCareerType((CareerType)msg.m_value.career());
				pkPlayer->SetWeaponAttribute((WeaponType)msg.m_value.weapon());
			}
		}

		//pkGamePlayer->SetPlayerType(msg.m_lPlayerType);

		if(bBuddy)
		{
			pkGamePlayer->UpdateDirtyFlag(DIRTY_FLAG_PROPERTY);
		}
		else if(bSub)
		{
			pkGamePlayer->GetSubordinateAsset().UpdateDirtyFlag();
		}
		
		pkGamePlayer->SetDirtyFlag(DIRTY_FLAG_ATTRIBUTE, true);
		pkGamePlayer->AddDirtyFlagRevisionID(DIRTY_FLAG_ATTRIBUTE);
	}
//--new
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncGamePlayerInfo(NW_Game2ClientSession* pkSession, char* pMsg, uint16_t wSize)
{
//	MG_Sync_GamePlayerInfo msg;
//	if (!msg.Decode(pMsg, wSize))
//	{
//		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "解析人物移动信息错误");
//		return;
//	}
//  	LC_ClientGamePlayer* pkPlayer = pkSession->GetPlayer();
//  	if(pkPlayer && pkPlayer->GetID() == msg.m_lPlayerID)
//  	{
//  		UT_Vec3Int kCurrentLocation;
//  		UT_Vec3Int kTargetLocation;
//  		kCurrentLocation.x = msg.m_lCurrentLocationX;
//  		kCurrentLocation.y = msg.m_lCurrentLocationY;
//  		kTargetLocation.x = msg.m_lTargetLocationX;
//  		kTargetLocation.y = msg.m_lTargetLocationY;
//  		pkPlayer->SetCurrentLocation(kCurrentLocation);
//  		pkPlayer->SetTargetLocation(kTargetLocation);
//  		pkPlayer->SetMoveSpeed(msg.m_lMoveSpeed);
//  	}
}
//------------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncGamePlayerAIControl(NW_Game2ClientSession* pkSession, char* pMsg, uint16_t wSize)
{
	MG_AIControl msg;
	if (!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_AIControl decode error");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if (NULL == pkGamePlayer)
		return;

	pkGamePlayer->SetAIControl(msg.m_iscontrol);
}
//------------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncGamePlayerAIControlLocation(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	PlayerAILocationEntry msg;
	if(!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SyncPlayerLocation::PlayerLocationEntry解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(!pkGamePlayer)
		return;

	const SE_SceneTerrainLocalInfo& rkTerrainInfo1 = SE_SceneManager::GetSingletonPtr()->GetWalkableInfo(Int2Float(msg.m_lCurrentLocationX), Int2Float(msg.m_lCurrentLocationY));
	const SE_SceneTerrainLocalInfo& rkTerrainInfo2 = SE_SceneManager::GetSingletonPtr()->GetWalkableInfo(Int2Float(msg.m_lTargetLocationX), Int2Float(msg.m_lTargetLocationY));

	UT_Vec2Float kDir = UT_MathBase::GetZRotDir(LC_Helper::Angle_Char2Float(msg.m_chCurrentDirAngle));
	UT_Vec3Int kCurrentLocation(msg.m_lCurrentLocationX,msg.m_lCurrentLocationY,rkTerrainInfo1._fLocalHeight);
	UT_Vec3Int kTargetLocation(msg.m_lTargetLocationX,msg.m_lTargetLocationY,rkTerrainInfo2._fLocalHeight);
	pkGamePlayer->SetCurrentLocation(kCurrentLocation);
	pkGamePlayer->SetTargetLocation(kTargetLocation);
	pkGamePlayer->SetMoveLineTargetLocation(kTargetLocation);
	pkGamePlayer->SetForwardDir(kDir);
	pkGamePlayer->SetTargetForwardDir(kDir);
}
//------------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncPlayerCash(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	//NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	//NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	//int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	////如果还未进入游戏，则退出
	//if (eGameStage != ST_GAME_STAGE_GAME)
	//	return;


	MG_SyncPlayerCash msg;
	if (msg.Decode(pMsg,wSize))
	{
		object_id_type lPlayerID		= msg.m_lPlayerID;			
		MoneyType llBindCash    = msg.m_llBindCash;
		MoneyType llUnBindCash    = msg.m_llUnBindCash;

		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

		if(!pkGamePlayer || pkGamePlayer->GetMainControllerID() != lPlayerID)
		{
			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"角色参数同步消息逻辑错误");
			return;
		}
		
		LC_Money& rkUnbindCash = pkGamePlayer->GetUnBindCash();
		MoneyType unbindDelta = llUnBindCash - rkUnbindCash.GetMoney();
		if(llUnBindCash > 0 && rkUnbindCash.GetMoney() > 0 && unbindDelta > 0)
		{
			LC_GameEvent kGameEvent;
			kGameEvent.SetEventType(GLET_GAIN_MONEY);
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, CT_UNBIND_CASH);
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, unbindDelta);

			UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
			pkUIManager->HandlerGameEvent(kGameEvent);
		}

		pkGamePlayer->SetBindCash(llBindCash);
		pkGamePlayer->SetUnBindCash(llUnBindCash);
	}
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncPlayerYuanBao(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncPlayerYuanBao msg;
	if ( msg.Decode(pMsg, wSize) )
	{
		object_id_type lPlayerID				= msg.m_lPlayerID;		
		MoneyType llBindYuanBao		= msg.m_llBindYuanBao;
		MoneyType llUnBindYuanBao	= msg.m_llUnBindYuanBao;

		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

		if ( !pkGamePlayer || pkGamePlayer->GetMainControllerID() != lPlayerID)
		{
			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "角色参数同步消息逻辑错误");
			return;
		}
		
		LC_Money& rkBindYuanBao = pkGamePlayer->GetBindYuanBao();
		LC_Money& rkUnBindYuanBao = pkGamePlayer->GetUnBindYuanBao();

		MoneyType unbindDelta = llUnBindYuanBao - rkUnBindYuanBao.GetMoney();
		if(llUnBindYuanBao > 0 && rkUnBindYuanBao.GetMoney() > 0 && unbindDelta > 0)
		{
			LC_GameEvent kGameEvent;
			kGameEvent.SetEventType(GLET_GAIN_MONEY);
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, CT_UNBIND_YUANBAO);
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, unbindDelta);

			UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
			pkUIManager->HandlerGameEvent(kGameEvent);
		}

		MoneyType bindDelta = llBindYuanBao - rkBindYuanBao.GetMoney();
		if(llBindYuanBao > 0 && rkBindYuanBao.GetMoney() > 0 && bindDelta > 0)
		{
			LC_GameEvent kGameEvent;
			kGameEvent.SetEventType(GLET_GAIN_MONEY);
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, CT_BIND_YUANBAO);
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, bindDelta);

			UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
			pkUIManager->HandlerGameEvent(kGameEvent);
		}

		rkBindYuanBao.SetMoney(llBindYuanBao);
		rkUnBindYuanBao.SetMoney(llUnBindYuanBao);
	}
}

//------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncPlayerCustomCash(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_SyncCustomCash msg;
	if ( msg.Decode(pMsg, wSize) )
	{
		object_id_type lPlayerID		= msg.m_lPlayerID;		
		MoneyType couragePt	= msg.m_kCustomCash[1];
		MoneyType justicePt	= msg.m_kCustomCash[0];
		MoneyType arenaPt = msg.m_kCustomCash[CT_ARENA_POINT-CT_JUSTICE_PT];
		MoneyType monolyPt = msg.m_kCustomCash[CT_MONOPOLY_POINT-CT_JUSTICE_PT];
		MoneyType starSoul = msg.m_kCustomCash[CT_STAR_SOUL-CT_JUSTICE_PT];
		MoneyType runeCream = msg.m_kCustomCash[CT_RUNE_CREAM-CT_JUSTICE_PT];
		MoneyType buddyExp = msg.m_kCustomCash[CT_BUDDY_EXP-CT_JUSTICE_PT];
		MoneyType starMoney = msg.m_kCustomCash[CT_PENTACLE-CT_JUSTICE_PT];
		MoneyType courageMoney = msg.m_kCustomCash[CT_COURAGE-CT_JUSTICE_PT];
		MoneyType energyMoney = msg.m_kCustomCash[CT_ENERGY-CT_JUSTICE_PT];
		MoneyType trialMoney = msg.m_kCustomCash[CT_TRIAL-CT_JUSTICE_PT];
		MoneyType cubeMoney = msg.m_kCustomCash[CT_MAGIC_CUBE-CT_JUSTICE_PT];
		MoneyType actmeritMoney = msg.m_kCustomCash[CT_ACTIVE_MERIT-CT_JUSTICE_PT];
		MoneyType rewardmedalMoney = msg.m_kCustomCash[CT_REWARD_MEDAL-CT_JUSTICE_PT];
		MoneyType dailymeritMoney = msg.m_kCustomCash[CT_DAILY_MERIT-CT_JUSTICE_PT];
		MoneyType guildmeritMoney = msg.m_kCustomCash[CT_GUILD_MERIT-CT_JUSTICE_PT];
		MoneyType chapterhonourMoney = msg.m_kCustomCash[CT_CHAPTER_HONOUR-CT_JUSTICE_PT];
		MoneyType bossJPMoney = msg.m_kCustomCash[CT_BOSS_JP-CT_JUSTICE_PT];
		MoneyType accountPoint = msg.m_kCustomCash[CT_ACCOUNT_POINT-CT_JUSTICE_PT];
		MoneyType SkinPointMoney = msg.m_kCustomCash[CT_SKIN_POINT-CT_JUSTICE_PT];
		MoneyType RaidGloryMoney = msg.m_kCustomCash[CT_CASH_TYPE_28-CT_JUSTICE_PT];
		MoneyType YijituxiMoney = msg.m_kCustomCash[CT_CASH_TYPE_29-CT_JUSTICE_PT];
		MoneyType EntrustPoint = msg.m_kCustomCash[CT_CASH_TYPE_30-CT_JUSTICE_PT];
		MoneyType SkyArenaPoint = msg.m_kCustomCash[CT_SKYARENA_POINT-CT_JUSTICE_PT];
		MoneyType VehiclePoint = msg.m_kCustomCash[CT_CASH_TYPE_34-CT_JUSTICE_PT];
		MoneyType PetPoint = msg.m_kCustomCash[CT_CASH_TYPE_35-CT_JUSTICE_PT];
		MoneyType BackWingPoint = msg.m_kCustomCash[CT_CASH_TYPE_36-CT_JUSTICE_PT];
		MoneyType holidayMoney = msg.m_kCustomCash[CT_CASH_TYPE_37-CT_JUSTICE_PT];
		

		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

		if ( !pkGamePlayer || pkGamePlayer->GetMainControllerID() != lPlayerID)
		{
			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "角色参数同步消息逻辑错误");
			return;
		}

		LC_Money& rkCouragePt = pkGamePlayer->GetCouragePT();
		LC_Money& rkJusticePt = pkGamePlayer->GetJusticePT();
		LC_Money& rkArenaPt = pkGamePlayer->GetArenaPT();
		LC_Money& rkMonolyPt = pkGamePlayer->GetMonopolyPT();
		LC_Money& rkRuneCream = pkGamePlayer->GetRuneCream();
		LC_Money& rkStarSoul = pkGamePlayer->GetStarSoul();
		LC_Money& rkBuddyExp = pkGamePlayer->GetBuddyExp();
		LC_Money& rkStarMoney = pkGamePlayer->GetPentacle();
		LC_Money& rkCourageMoney = pkGamePlayer->GetMoneyCourage();
		LC_Money& rkEnergyMoney = pkGamePlayer->GetEnergy();
		LC_Money& rkTrialMoney = pkGamePlayer->GetTrial();
		LC_Money& rkCubeMoney = pkGamePlayer->GetMagicCube();
		LC_Money& rkactmeritMoney = pkGamePlayer->GetActiveMerit();
		LC_Money& rkrewardmealMoney = pkGamePlayer->GetRewardMedal();
		LC_Money& rkdailymeritMoney = pkGamePlayer->GetDailyMerit();
		LC_Money& rkguildmeritMoney = pkGamePlayer->GetGuildMerit();
		LC_Money& rkchapterhonourMoney = pkGamePlayer->GetChapterHonour();
		LC_Money& rkbossJPMoney = pkGamePlayer->GetBossJP();
		LC_Money& rkAccountPt = pkGamePlayer->GetAccountPoint();
		LC_Money& rkSkinPoint = pkGamePlayer->GetSkinPoint();
		LC_Money& rkRaidGloryMoney = pkGamePlayer->GetCashType28();
		LC_Money& rkYijituxiMoney = pkGamePlayer->GetCashType29();
		LC_Money& rkEntrustMoney = pkGamePlayer->GetCashType30();
		LC_Money& rkSkyArenaMoney = pkGamePlayer->GetCashType32();
		LC_Money& rkVehicleMoney = pkGamePlayer->GetCashType34();
		LC_Money& rkPetMoney = pkGamePlayer->GetCashType35();
		LC_Money& rkBackWingMoney = pkGamePlayer->GetCashType36();

		LC_Money* pkHolidayMoney = NULL;
		pkGamePlayer->GetCashByCashType(CT_CASH_TYPE_37, &pkHolidayMoney);
		

		MoneyType buddyExpDelta = buddyExp - rkBuddyExp.GetMoney();
		if(buddyExp > 0 && rkBuddyExp.GetMoney() > 0 && buddyExpDelta > 0)
		{
			LC_GameEvent kGameEvent;
			kGameEvent.SetEventType(GLET_GAIN_MONEY);
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, CT_BUDDY_EXP);
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, buddyExpDelta);

			UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
			pkUIManager->HandlerGameEvent(kGameEvent);
		}

		MoneyType runeCreamExpDelta = runeCream - rkRuneCream.GetMoney();
		if(runeCream > 0 && rkRuneCream.GetMoney() > 0 && runeCreamExpDelta > 0)
		{
			LC_GameEvent kGameEvent;
			kGameEvent.SetEventType(GLET_GAIN_MONEY);
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, CT_RUNE_CREAM);
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, runeCreamExpDelta);

			UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
			pkUIManager->HandlerGameEvent(kGameEvent);
		}

		MoneyType bossJPDelta = bossJPMoney - rkbossJPMoney.GetMoney();
		if(bossJPMoney > 0 && rkbossJPMoney.GetMoney() > 0 && bossJPDelta > 0)
		{
			LC_GameEvent kGameEvent;
			kGameEvent.SetEventType(GLET_GAIN_MONEY);
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, CT_BOSS_JP);
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, bossJPDelta);

			UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
			pkUIManager->HandlerGameEvent(kGameEvent);
		}

		MoneyType YijituxiDelta = YijituxiMoney - rkYijituxiMoney.GetMoney();
		if(YijituxiMoney > 0 && rkYijituxiMoney.GetMoney() > 0 && YijituxiDelta > 0)
		{
			LC_GameEvent kGameEvent;
			kGameEvent.SetEventType(GLET_GAIN_MONEY);
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, CT_CASH_TYPE_29);
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, YijituxiDelta);

			UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
			pkUIManager->HandlerGameEvent(kGameEvent);
		}

		rkCouragePt.SetMoney(couragePt);
		rkCouragePt.UpdateDirtyFlag();
		rkJusticePt.SetMoney(justicePt);
		rkJusticePt.UpdateDirtyFlag();
		rkArenaPt.SetMoney(arenaPt);
		rkArenaPt.UpdateDirtyFlag();
		rkRuneCream.SetMoney(runeCream);
		rkRuneCream.UpdateDirtyFlag();
		rkStarSoul.SetMoney(starSoul);
		rkStarSoul.UpdateDirtyFlag();
		rkBuddyExp.SetMoney(buddyExp);
		rkBuddyExp.UpdateDirtyFlag();
		rkStarMoney.SetMoney(starMoney);
		rkStarMoney.UpdateDirtyFlag();
		rkCourageMoney.SetMoney(courageMoney);
		rkCourageMoney.UpdateDirtyFlag();
		rkEnergyMoney.SetMoney(energyMoney);
		rkEnergyMoney.UpdateDirtyFlag();
		rkTrialMoney.SetMoney(trialMoney);
		rkTrialMoney.UpdateDirtyFlag();
		rkCubeMoney.SetMoney(cubeMoney);
		rkCubeMoney.UpdateDirtyFlag();
		rkactmeritMoney.SetMoney(actmeritMoney);
		rkactmeritMoney.UpdateDirtyFlag();
		rkrewardmealMoney.SetMoney(rewardmedalMoney);
		rkrewardmealMoney.UpdateDirtyFlag();
		rkdailymeritMoney.SetMoney(dailymeritMoney);
		rkdailymeritMoney.UpdateDirtyFlag();
		rkguildmeritMoney.SetMoney(guildmeritMoney);
		rkguildmeritMoney.UpdateDirtyFlag();
		rkchapterhonourMoney.SetMoney(chapterhonourMoney);
		rkchapterhonourMoney.UpdateDirtyFlag();
		rkbossJPMoney.SetMoney(bossJPMoney);
		rkbossJPMoney.UpdateDirtyFlag();
		rkAccountPt.SetMoney(accountPoint);
		rkAccountPt.UpdateDirtyFlag();
		rkSkinPoint.SetMoney(SkinPointMoney);
		rkSkinPoint.UpdateDirtyFlag();
		rkRaidGloryMoney.SetMoney(RaidGloryMoney);
		rkRaidGloryMoney.UpdateDirtyFlag();
		rkYijituxiMoney.SetMoney(YijituxiMoney);
		rkYijituxiMoney.UpdateDirtyFlag();
		rkEntrustMoney.SetMoney(EntrustPoint);
		rkEntrustMoney.UpdateDirtyFlag();
		rkSkyArenaMoney.SetMoney(SkyArenaPoint);
		rkSkyArenaMoney.UpdateDirtyFlag();
		rkVehicleMoney.SetMoney(VehiclePoint);
		rkVehicleMoney.UpdateDirtyFlag();
		rkPetMoney.SetMoney(PetPoint);
		rkPetMoney.UpdateDirtyFlag();
		rkBackWingMoney.SetMoney(BackWingPoint);
		rkBackWingMoney.UpdateDirtyFlag();

		pkHolidayMoney->SetMoney(holidayMoney);
		pkHolidayMoney->UpdateDirtyFlag();
	}
}

//
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncLotteryConsumeUnbindYuanbao(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_SyncLotteryConsumeUnBindYuanBao msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncLotteryConsumeUnBindYuanBao无法解析");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(!pkGamePlayer)
	{
		return;
	}

	pkGamePlayer->SetLotteryConsumeUnbindyuanbao(msg.m_nLotteryConsumeUnBindYuanBaoCount);

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_SYNC_LOTTERY_CONSUME_UNBINDYUANBAO);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncBasicBackPack(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	//NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	//NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	//int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	////如果还未进入游戏，则退出
	//if (eGameStage != ST_GAME_STAGE_GAME)
	//	return;
 	MG_RLT_BasicPackData msg;
 	if(msg.Decode(pMsg,wSize))
 	{
 		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
 		if(!pkGamePlayer)
 			return;
 		
 		LC_PackAsset* pkPackAsset = pkGamePlayer->GetPackAsset();
 		if (pkPackAsset)
 		{
			const PS_BasicPack& kPack = msg.m_kPack;
			LC_BasicPack& rBasicPack = pkPackAsset->GetBasicPack();
			rBasicPack.GetFromProtocol(kPack);

			if(msg.m_bIsAllPack)
			{
				LC_GameEvent kGameEvent;
				kGameEvent.SetEventType(GLET_BACKPACK_CHANGED);
				kGameEvent.SetEventParam(EP_LEFT_CAPACITY, pkPackAsset->GetPackLeftCapacity(PACK_TYPE_BASIC));

				UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
				pkUIManager->HandlerGameEvent(kGameEvent);
			}
			else
			{
				StringStreamType sStream;

				for (size_t i = 0; i < kPack.m_aData.size(); ++i)
				{
					int32_t index = kPack.m_aData[i].m_Index;
					sStream << index << '#';
				}

				LC_ClientGameEvent kGameEvent;
				kGameEvent.SetClientEventType(GLET_BACKPACK_DELTA_UPDATE);
				LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(sStream.str());
				kGameEvent.SetUserData(&kUserData);

				UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
				pkUIManager->HandlerGameEvent(kGameEvent);
			}
			
			pkPackAsset->SetDirtyFlag(PACK_TYPE_BASIC, true);
			pkPackAsset->AddDirtyFlagRevisionID(PACK_TYPE_BASIC);

			//更新CD表
			pkGamePlayer->BuildItemCoolDownMap();
		}
 	}
 	else
 	{
 		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "基本背包协议解析错误");
 	}
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncRuneBackPack(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RLT_RuneEquipSlotPackData msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "HL_FromGameServer::_handlerSyncRuneBackPack 星魂背包协议解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer == NULL)
		return;

	int iBuddyId = msg.m_nBuddyID;
	LC_ClientBuddyInstance* pkInst = pkGamePlayer->GetBuddyInstance(iBuddyId);
	if(pkInst == NULL)
		return;

	mem::vector<int> kIdxs;
	std::vector<Equip_Base>::const_iterator iter = msg.m_RuneEquip.begin();
	for( size_t i=0 ; i<msg.m_RuneEquip.size(); i++)
	{
		const Equip_Base& info = msg.m_RuneEquip[i];
		pkInst->UpdateRuneEquip(i, info.m_Data);

		if(pkGamePlayer->UpdateRuneBuddySlotDressState(iBuddyId, i, info.m_SuiteId > 0))
			kIdxs.push_back(i);
	}
	
	LC_PackAsset* pkPackAsset = pkGamePlayer->GetPackAsset();
	pkPackAsset->SetDirtyFlag(PACK_TYPE_RUNE_EQUIP_SLOT, true);
	pkPackAsset->AddDirtyFlagRevisionID(PACK_TYPE_RUNE_EQUIP_SLOT);

	//下发格子开启事件
	if(!kIdxs.empty())
	{
		StringStreamType sStream;
		sStream << iBuddyId << '#' << kIdxs.size() << '#';

		for(size_t i=0; i<kIdxs.size(); ++i)
		{
			sStream << kIdxs[i] << '#';
		}

		LC_ClientGameEvent kGameEvent;
		kGameEvent.SetClientEventType(GameLogic::CGET_MSG_ON_BUDDY_RUNESLOT_OPEN);
		LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(sStream.str());
		kGameEvent.SetUserData(&kUserData);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
}

void HL_FromGameServer::_handlerSyncFirstAddRune(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_SyncAddRune msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "HL_FromGameServer::_handlerSyncFirstAddRune 协议解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer == NULL)
		return;

	StringStreamType sStream;
	sStream << msg.m_bIsItemAdd << ";";
	bool bsuccess = false;
	for(size_t i=0; i< msg.m_runeInfo.size(); ++i)
	{
		const PS_AddRuneInfo& runeIno = msg.m_runeInfo[i];
		if(runeIno.m_bFirstAdd)
			pkGamePlayer->SetRuneRecord(runeIno.m_nRuneID, false);

		sStream << runeIno.m_nRuneID << '#'<< runeIno.m_bFirstAdd << '#';
		for(size_t i=0; i<runeIno. m_nDecompositionResult.size(); ++i)
		{
			const PS_ItemInfo& kItemInfo = runeIno. m_nDecompositionResult[i];
			sStream << kItemInfo.m_nItemID << ':' << kItemInfo.m_nItemNum << '|';
		}
		sStream << '@';
		if(runeIno.m_nResult != RE_SUCCESS)
			SendLogicInfoMessage(runeIno.m_nResult);
		else
			bsuccess = true;
	}
	
	if (!bsuccess) //如果都不是成功，就用不发事件
		return;
	LC_ClientGameEvent kCGameEvent;
	kCGameEvent.SetClientEventType(CGET_ADD_RUNE);
	LC_ClientGameEventUserData_String kUserData(sStream.str());
	kCGameEvent.SetUserData(&kUserData);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kCGameEvent);
}

void HL_FromGameServer::_handlerSyncRuneRecord(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_SyncRuneRecord msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "HL_FromGameServer::_handlerSyncRuneRecord 协议解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer == NULL)
		return;

	pkGamePlayer->SyncRuneRecordInfo(msg);
}

void HL_FromGameServer::_handlerNewItemNotify(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_SC_NEW_ITEM_NOTIFY msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "HL_FromGameServer::_handlerNewItemNotify 协议解析错误");
		return;
	}

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_NEWITEM_NOTIFY);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_nItemId);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerRespRuneReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RLT_RuneReward msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "HL_FromGameServer::_handlerRespRuneReward 协议解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer == NULL)
		return;

	if(msg.m_nResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_nResult);
	}
	else
	{
		pkGamePlayer->SetRuneRecord(msg.m_nRewardID, true);

		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_SYNC_RESP_RUNE_REWARD);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_nRewardID);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncTaskBackPack(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	//NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	//NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	//int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	////如果还未进入游戏，则退出
	//if (eGameStage != ST_GAME_STAGE_GAME)
	//	return;

	/*
 	MG_RLT_TaskPackData msg;
 	if(msg.Decode(pMsg,wSize))
 	{
 		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
 		if(!pkGamePlayer)
 			return;
 
 		LC_PackAsset* pkPackAsset = pkGamePlayer->GetPackAsset();
 		if (pkPackAsset)
 		{
 			//pkPackAsset->SetTaskPack( msg.m_kPack );
			
			LC_TaskPack& rTaskPack = pkPackAsset->GetTaskPack();
			//LC_TaskPack kTaskPack;
			rTaskPack.Reset();
			rTaskPack.GetFromProtocol(msg.m_kPack);

			//pkPackAsset->SetTaskPack(kTaskPack);
			pkPackAsset->SetDirtyFlag(PACK_TYPE_TASK, true);
			pkPackAsset->AddDirtyFlagRevisionID(PACK_TYPE_TASK);
			pkGamePlayer->AddDirtyFlagRevisionID(DIRTY_FLAG_SHORTCUT);

			//更新CD表
			pkGamePlayer->BuildItemCoolDownMap();
 		}
 	}
 	else
 	{
 		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "任务背包协议解析错误");		
 	}
	*/
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncCommandEquip(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_SC_SynCommandEquip_Ret msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SC_SynCommandEquip_Ret协议解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(!pkGamePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "No GamePlayer MG_SC_SynCommandEquip_Ret协议解析错误");
		return;
	}

	LC_ClientGamePlayerCommander* pkCommander = pkGamePlayer->GetCommander();
	pkCommander->UpdateEquipList(msg);

	LC_PlayerBase::LC_PlayerEquipSuitPtrMap& kPlayerEquipSuitMap = pkGamePlayer->GetPlayerEquipSuitMap();
	kPlayerEquipSuitMap.clear();

	EquipVec& equipList = pkCommander->GetEquipList(CommandPack);
	for (size_t i = 0; i < equipList.size(); ++i)
	{
		const LC_BackPackEntry& pkEntry = equipList[i];
		if(pkEntry.GetValid())
		{
			CF_ItemGen::DataEntry * entry = SafeGetCSVFileDataEntryPtr<CF_ItemGen>(pkEntry.GetTypeID(),false,false);
			if (entry && !entry->EquipeSetID.empty())
			{
				Utility::UT_SSIMDataListInt16::iterator it = entry->EquipeSetID.begin();
				for (; it!=entry->EquipeSetID.end(); ++it)
				{
					kPlayerEquipSuitMap[it->ID()] += 1;
				}
			}
		}
	}

	LC_PackAsset* pkPackAsset = pkGamePlayer->GetPackAsset();
	pkPackAsset->SetDirtyFlag(PACK_TYPE_EQUIP_SLOT, true);
	pkPackAsset->AddDirtyFlagRevisionID(PACK_TYPE_EQUIP_SLOT);
	
	pkPackAsset->SetDirtyFlag(PACK_TYPE_SUB_EQUIP_SLOT, true);
	pkPackAsset->AddDirtyFlagRevisionID(PACK_TYPE_SUB_EQUIP_SLOT);
}

void HL_FromGameServer::_handlerSyncBuddyEquipSlotPackData(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RLT_BuddyEquipSlotPackData msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_RLT_BuddyEquipSlotPackData协议解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "No GamePlayer MG_RLT_BuddyEquipSlotPackData协议解析错误");
		return;
	}

	LC_ClientBuddyInstance* pkInst = pkGamePlayer->GetBuddyInstance(msg.m_nBuddyID);
	if(pkInst == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "No Buddy cid=[%d] MG_RLT_BuddyEquipSlotPackData协议解析错误", msg.m_nBuddyID);
		return;
	}

	SC_ClientScriptInterface* pkClientInterface = SC_ClientScriptInterface::GetSingletonPtr();
	SC_ClientSkillInterface* pkInterface = pkClientInterface->GetSkillInterface();

	mem::vector<int> equiplevel;
	mem::vector<int> equipGrade;
	equiplevel.resize(GetGlobalSetting.ArtifactPackSize);
	equipGrade.resize(GetGlobalSetting.ArtifactPackSize);

	for(size_t i=0; i<msg.m_EquipData.size(); i++)
	{
		const Equip_Base& equipInfo = msg.m_EquipData[i];
		pkInst->UpdateEquip(i, equipInfo);

		equiplevel[i] = pkInterface->GetEquipStarQuality(i, equipInfo.m_IntensifyLevel);

		int iGrade = ITEM_GRADE_NONE;
		LC_BackPackEntry *pkEntry = pkInst->GetEquipEntry(i);
		if(pkEntry->GetValid())
		{
			iGrade = pkInterface->GetMadeItemGrade(pkEntry->GetTypeID()) - 1;
		}
		equipGrade[i] = iGrade;
	}
	
	pkGamePlayer->SetBuddyEquipGrades(msg.m_nBuddyID, equipGrade);
	pkGamePlayer->SetBuddyEquipLevel(msg.m_nBuddyID, equiplevel);
	pkGamePlayer->OnBuddyEquipUpdated();
}

void HL_FromGameServer::_handlerSyncBuddyEquipSlotData(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RLT_BuddyEquipSlotData msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_RLT_BuddyEquipSlotData协议解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "No GamePlayer MG_RLT_BuddyEquipSlotData协议解析错误");
		return;
	}

	LC_ClientBuddyInstance* pkInst = pkGamePlayer->GetBuddyInstance(msg.m_nBuddyID);
	if(pkInst == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "No Buddy cid=[%d] MG_RLT_BuddyEquipSlotData协议解析错误", msg.m_nBuddyID);
		return;
	}

	pkInst->UpdateEquip(msg.m_nSlotIndex, msg.m_EquipSlotData);
	pkGamePlayer->SetUpBuddyEquipGrade(msg.m_nBuddyID, msg.m_nSlotIndex);
	pkGamePlayer->OnBuddyEquipUpdated();
}

//------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncWarehouseBackPack(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	//NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	//NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	//int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	////如果还未进入游戏，则退出
	//if (eGameStage != ST_GAME_STAGE_GAME)
	//	return;


 	MG_RLT_WarehousePackData msg;
 	if(msg.Decode(pMsg,wSize))
 	{
 		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
 		if(!pkGamePlayer)
 			return;
 
 		LC_PackAsset* pkPackAsset = pkGamePlayer->GetPackAsset();
 		if (pkPackAsset)
 		{
 			//pkPackAsset->SetWarehousePack( msg.m_kPack );
			
			LC_WarehousePack& rWarehousePack = pkPackAsset->GetWarehousePack();
			//LC_WarehousePack kWarehousePack;
			//rWarehousePack.Reset();
			rWarehousePack.GetFromProtocol(msg.m_kPack);
			pkPackAsset->SetDirtyFlag(PACK_TYPE_WAREHOUSE, true);
			pkPackAsset->AddDirtyFlagRevisionID(PACK_TYPE_WAREHOUSE);

			//pkPackAsset->SetWarehousePack(kWarehousePack);
 		}
 	}
 	else
 	{
 		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "任务背包协议解析错误");		
 	}
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncOpenBagAsset(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_OpenBagAssert rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(!pkGamePlayer)
		return;

	pkGamePlayer->GetOpenBagAssert().GetDataFromProtocolStruct(rltMsg);
}

void HL_FromGameServer::_handlerSyncProcessingTask(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	//NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	//NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	//int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	////如果还未进入游戏，则退出
	//if (eGameStage != ST_GAME_STAGE_GAME)
	//	return;


	MG_SyncProcessingTaskMap msg;
	if(msg.Decode(pMsg,wSize))
	{
		object_id_type lPlayerID		= msg.m_lPlayerID;

		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
		if(!pkGamePlayer)
			return;

		if(pkGamePlayer->GetMainControllerID() != lPlayerID)
		{
			return;
		}

		LC_ClientTaskManager* pkTaskManager = (LC_ClientTaskManager*)LC_ClientTaskManager::GetSingletonPtr();
		if (pkTaskManager == NULL)
			return;
		LC_TaskMap* pkTaskMap = pkGamePlayer->GetTaskMap();
		if (pkTaskMap == NULL)
			return;
		LC_ClientTaskMap* pkClientTaskMap = (LC_ClientTaskMap*)pkTaskMap;
		//pkTaskMap->Clear();
		pkClientTaskMap->SetDirtyFlag(DIRTY_FLAG_TASK_DELTARECORD,true);
		pkClientTaskMap->AddDirtyFlagRevisionID(DIRTY_FLAG_TASK_DELTARECORD);
		pkClientTaskMap->SetDirtyFlag(DIRTY_FLAG_PROCESSINGTASK,true);
		pkClientTaskMap->AddDirtyFlagRevisionID(DIRTY_FLAG_PROCESSINGTASK);
		
		LC_ProcessingTaskEntryMap& kProcessEntryMap = pkTaskMap->GetProcessingTaskEntryMap();
		kProcessEntryMap.clear();
		for(int iIndex = 0; iIndex < msg.m_kCharProcessingTaskInfo.m_szProcessingTasks.size(); iIndex++)
		{
			PS_ProcessingTaskNode& node = msg.m_kCharProcessingTaskInfo.m_szProcessingTasks[iIndex];

			if(node.m_iTaskID<=0) {continue;}

			LC_ProcessingTaskEntry*  pkEntry = pkTaskMap->AddTaskToPorcessingMap(node.m_iTaskID);
			if (pkEntry)
			{
				pkEntry->GetDataFromProtocolStruct(node);
				//失败的任务申请取消任务，并发送事件到Lua
				if (pkEntry->GetTaskFail())
				{
					pkGamePlayer->ReqCancelTask(pkEntry->GetTaskID());
					pkTaskMap->RemoveTaskProcessing(pkEntry->GetTaskID());
				}
			}
		}
		//重置任务类型map
		pkTaskMap->SetUp();
		//同步每日挑战任务数据
		pkClientTaskMap->GetLivenessEntry(STT_DAILY_CHALLENGE).GetDataFromProtocolStruct(msg.m_kDailyLiveness);
		//更新主线任务进度
		pkClientTaskMap->UpdateMainTaskTraceIndex();
		//GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"同步ProcessingTask信息时间[%f]",GET_PROCESS_TIME);
		pkGamePlayer->UpdateNearNPCTaskState();

		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_CLIENT_TASK_DATA_UPDATE);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncFinishedTask(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	//NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	//NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	//int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	////如果还未进入游戏，则退出
	//if (eGameStage != ST_GAME_STAGE_GAME)
	//	return;


	MG_SyncFinishedTaskMap msg;
	if(msg.Decode(pMsg,wSize))
	{
		object_id_type lPlayerID		= msg.m_lPlayerID;
		GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM,"[数据][收]MGPT_SYNC_FINISHED_TASK msg.m_lPlayerID=%d",msg.m_lPlayerID);

		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
		if(!pkGamePlayer)
			return;
		GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM,"[数据][收]MGPT_SYNC_FINISHED_TASK GamePlayerID=%d",pkGamePlayer->GetID());
		if(pkGamePlayer->GetMainControllerID() != lPlayerID)
		{
			return;
		}

		LC_TaskMap* pkTaskMap = pkGamePlayer->GetTaskMap();
		if (NULL == pkTaskMap)
			return;

		LC_ClientTaskMap* pkClientTaskMap = (LC_ClientTaskMap*)pkTaskMap;
		LC_FinishedTaskRecord* pkRecord = pkClientTaskMap->GetFinishedTaskRecord();
		if (NULL == pkRecord)
			return;

		pkRecord->GetDataFromProtocolStruct(msg.m_stCharFinishedTaskRecord);
		pkClientTaskMap->UpdateCanAcceptTask();
		//更新主线任务进度
		pkClientTaskMap->UpdateMainTaskTraceIndex();
		//pkGamePlayer->SetImmediateTaskStart(true);
		//GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"同步FinishedTask信息时间[%f]",GET_PROCESS_TIME);
		pkClientTaskMap->SetDirtyFlag(DIRTY_FLAG_TASK_DELTARECORD,true);
		pkClientTaskMap->AddDirtyFlagRevisionID(DIRTY_FLAG_TASK_DELTARECORD);
		pkClientTaskMap->SetDirtyFlag(DIRTY_FLAG_FINISHEDTASK,true);
		pkClientTaskMap->AddDirtyFlagRevisionID(DIRTY_FLAG_FINISHEDTASK);
		pkGamePlayer->UpdateNearNPCTaskState();
		pkGamePlayer->UpdateNearNPCDynamicCollision();
		pkGamePlayer->CheckCommanderTitleOpen();

		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_SYNC_TASKFINISH_DATA);
		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncFailTask( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_SyncFailTaskInfo msg;
	if(msg.Decode(pMsg,wSize))
	{
		object_id_type lPlayerID		= msg.m_lPlayerID;

		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
		if(!pkGamePlayer)
			return;

		if(pkGamePlayer->GetMainControllerID() != lPlayerID)
		{
			return;
		}

		LC_TaskMap* pkTaskMap = pkGamePlayer->GetTaskMap();
		if (NULL == pkTaskMap)
			return;

		LC_FailTaskEntryMap& kFailEntryMap = pkTaskMap->GetFailTaskEntryMap();
		kFailEntryMap.clear();

		LC_ClientTaskMap* pkClientTaskMap = (LC_ClientTaskMap*)pkTaskMap;
		for (size_t i = 0; i < msg.m_stCharFailTaskInfo.m_szFailTasks.size(); ++i)
		{
			PS_FailTaskNode& node = msg.m_stCharFailTaskInfo.m_szFailTasks[i];
			if(node.m_iTaskID<=0)
			{
				continue;
			}
			pkTaskMap->RecordTaskFail(node.m_iTaskID, node.m_iTaskIndex);
		}

		pkClientTaskMap->SetDirtyFlag(DIRTY_FLAG_FAIL_TASK,true);
		pkClientTaskMap->AddDirtyFlagRevisionID(DIRTY_FLAG_FAIL_TASK);
	}
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncNPCMoveInFOV(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	//NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	//NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	//int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	////如果还未进入游戏，则退出
	//if (eGameStage != ST_GAME_STAGE_GAME)
	//	return;


	MG_SyncNPCsMoveInFOV msg;
	if(msg.Decode(pMsg,wSize))
	{
		LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
		pkNPCManager->SyncNPCMoveInFOV(msg);
	}
}

void HL_FromGameServer::_handlerSyncNPCStateFOV(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_SyncNPCsStateInFOV msg;
	if(msg.Decode(pMsg,wSize))
	{
		LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
		pkNPCManager->SyncNPCStateInFOV(msg);
	}
}

void HL_FromGameServer::_handlerSyncNPCAngleFOV(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_SyncNPCsAngleInFOV msg;
	if(msg.Decode(pMsg,wSize))
	{
		LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
		pkNPCManager->SyncNPCAngleInFOV(msg);
	}
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncNPCPropertyInFOV(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack)
{
	//NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	//NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	//int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	////如果还未进入游戏，则退出
	//if (eGameStage != ST_GAME_STAGE_GAME)
	//	return;


	MG_SyncNPCsPropertyInFOV msg;
	if(msg.Decode(pMsg,wSize))
	{
		LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
		pkNPCManager->SyncNPCPropertyInFOV(msg, bArenaPlayBack);
	}
}

//*** npc refresh level
void HL_FromGameServer::_handlerRefreshNPCLevel(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RefreshNPCLevel msg;
	if (msg.Decode(pMsg,wSize))
	{
		LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
		pkNPCManager->SyncNPCLevelInFOV(msg);
	}
}

//------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncNPCSkillStateInFOV(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	//NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	//NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	//int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	////如果还未进入游戏，则退出
	//if (eGameStage != ST_GAME_STAGE_GAME)
	//	return;

	MG_SyncNPCsSkillStateInFOV msg;
	if(msg.Decode(pMsg,wSize))
	{
		LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
		pkNPCManager->SyncNPCSkillStateInFOV(msg);
	}
}

void HL_FromGameServer::_handlerSyncNPCInteractionInFOV( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_SyncNPCsInteractionValueInFOV msg;
	if(msg.Decode(pMsg,wSize))
	{
		LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
		//pkNPCManager->SyncNPCInteractionInFOV(msg);
	}
}
//------------------------------------------------------------------------
/*void HL_FromGameServer::_handlerSyncNPCFactionInFOV( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_SyncNPCFactionInFOV msg;
	if(msg.Decode(pMsg,wSize))
	{
		LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
		pkNPCManager->SyncNPCFactionInFOV(msg);
	}
}*/
//------------------------------------------------------------------------
/*void HL_FromGameServer::_handlerSyncPlayerFactionInFOV( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_SyncPlayerFactionInFOV msg;
	if(msg.Decode(pMsg,wSize))
	{
		LC_ClientNetPlayerManager* pkManager = CLIENT_GET_NETPLAYER_MANAGER();
		pkManager->SyncPlayerFactionInFOV(msg);
	}
}*/
//------------------------------------------------------------------------
//void HL_FromGameServer::_handlerSyncPlayerInfoInFOV(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
//{
//	//NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
//	//NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
//	//int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();
//
//	////如果还未进入游戏，则退出
//	//if (eGameStage != ST_GAME_STAGE_GAME)
//	//	return;
//
//
//	MG_SyncPlayersInfoInFOV msg;
//	if(!msg.Decode(pMsg,wSize))
//	{
//		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncPlayersInfoInFOV解析错误");
//		return;
//	}
//
//	LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
//	pkNetPlayerManager->SyncPlayerInfoInFOV(msg);
//}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncPlayerPropertyInFOV(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack)
{
	//NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	//NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	//int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	////如果还未进入游戏，则退出
	//if (eGameStage != ST_GAME_STAGE_GAME)
	//	return;

	MG_SyncPlayersPropertyInFOV msg;
	if(!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncPlayersPropertyInFOV解析错误");
		return;
	}

	LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
	pkNetPlayerManager->SyncPlayerPropertyInFOV(msg, bArenaPlayBack);
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncPlayerFastPropertyInFOV(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack)
{
	MG_SyncPlayersFastPropertyInFOV msg;
	if(!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncPlayersPropertyInFOV解析错误");
		return;
	}

	LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
	pkNetPlayerManager->SyncPlayerFastPropertyInFOV(msg, bArenaPlayBack);
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncPlayerExtPropertyInFOV(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_SyncPlayersExtPropertyInFOV msg;
	if(!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncPlayersPropertyInFOV解析错误");
		return;
	}

	LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
	pkNetPlayerManager->SyncPlayerExpPropertyInFOV(msg);
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncPlayerSocietyInFOV(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack)
{
	//NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	//NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	//int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	////如果还未进入游戏，则退出
	//if (eGameStage != ST_GAME_STAGE_GAME)
	//	return;


	MG_SyncPlayersSocietyInFOV msg;
	if(!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncPlayersSocietyInFOV解析错误");
		return;
	}

	LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
	pkNetPlayerManager->SyncPlayerSocietyInFOV(msg, bArenaPlayBack);
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncPlayerEquipInFOV(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	//NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	//NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	//int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	////如果还未进入游戏，则退出
	//if (eGameStage != ST_GAME_STAGE_GAME)
	//	return;


	MG_SyncPlayersEquipInFOV msg;
	if(!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncPlayersEquipInFOV解析错误");
		return;
	}

	LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
	pkNetPlayerManager->SyncPlayerEquipInFOV(msg);
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncPlayerSkillStateInFOV(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack)
{
	//NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	//NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	//int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	////如果还未进入游戏，则退出
	//if (eGameStage != ST_GAME_STAGE_GAME)
	//	return;


	MG_SyncPlayersSkillStateInFOV msg;
	if(!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncPlayersSkillStateInFOV解析错误, size=%d", wSize);
		return;
	}

	LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
	pkNetPlayerManager->SyncPlayerSkillStateInFOV(msg, bArenaPlayBack);
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncPlayerSubordinateInFOV(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack)
{
	//NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	//NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	//int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	////如果还未进入游戏，则退出
	//if (eGameStage != ST_GAME_STAGE_GAME)
	//	return;


	MG_SyncSubordinateInfoInFOV msg;
	if(!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncSubordinateInfoInFOV解析错误");
		return;
	}

	LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
	pkNetPlayerManager->SyncPlayerSubordinateInFOV(msg, bArenaPlayBack);
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncChestList(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	//NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	//NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	//int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	////如果还未进入游戏，则退出
	//if (eGameStage != ST_GAME_STAGE_GAME)
	//	return;


	MG_SyncPlaceHoldersInFOV msg;
	if(msg.Decode(pMsg,wSize))
	{
		LC_ClientPlaceHolderManager* pkChestManager = CLIENT_GET_PLACEHOLDER_MANAGER();
		pkChestManager->RefreshPlaceHolderList(msg);
	}
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncChestStateList(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncPlayerGroupMemberList(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	//NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	//NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	//int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	////如果还未进入游戏，则退出
	//if (eGameStage != ST_GAME_STAGE_GAME)
	//	return;


	MG_SyncPlayerGroupMemberList syncMsg;
	if(!syncMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"无法解析MG_SyncPlayerGroupMemberList");
		return;
	}

	long lGroupID = syncMsg.m_lGroupID;
	
	LC_PlayerBase* pkPlayer = pkSession->GetPlayer();
	if(NULL == pkPlayer)
		return;
	
	LC_ClientPlayerGroupManager* pkGroupManager = CLIENT_GET_PLAYER_GROUP_MANAGER();
	if (lGroupID == INVALID_GROUP_ID)
	{
		if( syncMsg.m_akMemberEntries.size() > 0 )
			//lGroupID = PLAYER_GROUP_ID_MAX + syncMsg.m_akMemberEntries[0].m_lPlayerID;
			lGroupID = PLAYER_GROUP_ID_MAX + unique_id_impl(syncMsg.m_akMemberEntries[0].m_uiCharID).detail();
	}
	LC_ClientPlayerGroup* pkGroup = (LC_ClientPlayerGroup*)(pkGroupManager->FindPlayerGroupByGroupID(lGroupID));

	if(!pkGroup)
	{
		LC_PlayerGroupBase* pkGroupNew = pkGroupManager->CreatePlayerGroup(lGroupID);	
		pkGroup = static_cast <LC_ClientPlayerGroup*> (pkGroupNew);
	}

	if(NULL == pkGroup)
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR, "无法创建队伍");
		return;	
	}

	pkGroup->SyncMemberList(syncMsg);

	// 将队友加入到临时好友
	LC_ClientGamePlayer* pkClientPlayer = static_cast < LC_ClientGamePlayer* > (pkPlayer);
	//if (!pkClientPlayer)
	//{
	//	return;
	//}
	
	LC_PlayerGroupEntry* pkMyEntry = pkGroup->FindGroupEntryInMemberList(pkClientPlayer->GetCommanderUID());
	if(pkMyEntry)
	{
		//我在队伍中
		pkClientPlayer->SetPlayerGroupIDType(pkGroup->GetID(), E_PlayerTeamType_Local);
		pkClientPlayer->SetPlayerGroupCheifFlag(pkMyEntry->GetCheif());
		
		//队友都是临时好友
		for(int i = 0; i < MAX_PLAYER_GROUP_MEMBER_LIST_SIZE; ++i)
		{
			LC_PlayerGroupEntry* pkEntry = pkGroup->GetGroupEntryInMemberList(i);
			if ( pkEntry && pkEntry->GetValid() && pkEntry->GetPlayerID() != pkClientPlayer->GetID() )
			{
				pkClientPlayer->AddTempFriend(pkEntry->GetCharID(), pkEntry->GetName(), pkEntry->GetCareerType(), pkEntry->GetLevel());
			}
		}
	}
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncPlayerGroupCandidateList(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_SyncPlayerGroupCandidateList syncMsg;
	if (!syncMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "无法解析MG_SyncPlayerCandidateList");
		return;
	}

	LC_PlayerBase* pkPlayer = pkSession->GetPlayer();
	if (pkPlayer == NULL)
	{
		return;
	}
	LC_ClientPlayerGroup* pkGroup = (LC_ClientPlayerGroup*)(pkPlayer->GetPlayerGroup());

	if (pkGroup && pkGroup->GetGroupCheifID() == pkPlayer->GetOwnerObjectID())
	{
		pkGroup->SyncCandidateList(syncMsg);
	}	
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncPKState(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	//NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	//NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	//int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	////如果还未进入游戏，则退出
	//if (eGameStage != ST_GAME_STAGE_GAME)
	//	return;


	MG_SyncPKMode syncMsg;
	if(!syncMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "SyncPKState解析错误");
		return;
	}

	LC_PlayerBase* pkPlayer = pkSession->GetPlayer();
	if(!pkPlayer)
		return;

	if(pkPlayer->GetSelfID() != syncMsg.m_lPlayerID)
	{
		LC_ClientNetPlayerManager* clientNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
		pkPlayer = clientNetPlayerManager->FindPlayer(syncMsg.m_lPlayerID);

		if(pkPlayer == NULL)
		{
			GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"SyncPKState无法处理，系统错误");
			return;
		}
	}

	LC_PKRecorderBase* pkPKRecorder = pkPlayer->GetPKRecorder();
	if(pkPKRecorder == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"SyncPKState无法处理，不存在");
		return;
	}

	//原先的PK状态
	PKStateType eOldStateType = pkPKRecorder->GetPKStateType();
	long lOldPKValue = pkPKRecorder->GetPKValue();
	pkPKRecorder->SetPKMode(syncMsg.m_modeindex);

	/*pkPKRecorder->SetCommunityProtected(syncMsg.m_bCommunityProtected);
	pkPKRecorder->SetFriendProtected(syncMsg.m_bFriendProtected);
	pkPKRecorder->SetWhiteNameProtected(syncMsg.m_bWhiteNameProtected);
	pkPKRecorder->SetRedNameProtected(syncMsg.m_bRedNameProtected);
	pkPKRecorder->SetPKStateType((PKStateType)syncMsg.m_iPKState);
	pkPKRecorder->SetInEvilState(syncMsg.m_bIsInEvilState);
	pkPKRecorder->SetPKValue(syncMsg.m_lPKValue);*/

	//显示提示信息
	/*PKStateType eCurrentStateType = pkPKRecorder->GetPKStateType();
	long lCurrentPKValue = pkPKRecorder->GetPKValue();

	if (eOldStateType == PST_WHITE && eCurrentStateType == PST_YELLOW)
	{
		SetMessageToAuxiliaryTab();
		SendLogicInfoMessage(RE_PK_YOU_BECOME_YELLOW);
	}
	else if (eOldStateType == PST_YELLOW && eCurrentStateType == PST_RED)
	{
		SetMessageToAuxiliaryTab();
		SendLogicInfoMessage(RE_PK_YOU_BECOME_RED);
	}

	if (lOldPKValue != lCurrentPKValue)
	{
		SetMessageToAuxiliaryTab();
		SendLogicInfoMessage(RE_PK_CURRENT_PKVALUE, lCurrentPKValue);
	}*/
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncPKValue(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RTL_GetPKValue pkValueMsg;
	if(!pkValueMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "SyncPKState解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(!pkGamePlayer)
		return;

	if ( pkGamePlayer->GetCommanderMainID() == pkValueMsg.m_lPlayerID)
	{
		LC_PKRecorderBase* pkRC = pkGamePlayer->GetPKRecorder();
		pkRC->SetPKValue(pkValueMsg.m_lPkValue);
	}
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncFightState(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	//NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	//NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	//int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	////如果还未进入游戏，则退出
	//if (eGameStage != ST_GAME_STAGE_GAME)
	//	return;


	MG_SyncFightState syncMsg;
	if(!syncMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SyncFightState解析错误");
		return;
	}

	LC_PlayerBase* pkPlayer = pkSession->GetPlayer();
	if(!pkPlayer)
		return;

	if(pkPlayer->GetID() != syncMsg.m_lPlayerID)
	{
		LC_ClientNetPlayerManager* clientNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
		pkPlayer = clientNetPlayerManager->FindPlayer(syncMsg.m_lPlayerID);
		if(pkPlayer == NULL)
		{
			GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncFightState无法处理，系统错误找不到ID为%d的netplayer", syncMsg.m_lPlayerID);
			return;
		}
	}

	/*LC_PlayerHatred* pkHatred = (LC_PlayerHatred*)pkGamePlayer->GetHatred();
	if (NULL == pkHatred)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"SyncPKState无法处理，系统错误");
		return;
	}

	bool bPVP = pkHatred->GetPVPFightState();
	bool bPVE = pkHatred->GetPVEFightState();
	pkHatred->SetPVPFightState(syncMsg.m_bPVPFightState);
	pkHatred->SetPVEFightState(syncMsg.m_bPVEFightState);

	if ((bPVP != pkHatred->GetPVPFightState() && bPVP == false)
		||(bPVE != pkHatred->GetPVEFightState() && bPVE == false))
	{
		pkGamePlayer->SetNPCFunctionType(NFT_NONE);
	}*/

	LC_PKRecorderBase* pkPKRecorder = pkPlayer->GetPKRecorder();
	if (NULL == pkPKRecorder)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"SyncPKState无法处理，系统错误");
		return;
	}

	//原先战斗状态
	bool bOldPVP = pkPKRecorder->GetPVPFightState();
	bool bOldPVE = pkPKRecorder->GetPVEFightState();
	bool bOldFightState = bOldPVP || bOldPVE;

	//新的战斗状态
	bool bPVP = syncMsg.m_bPVPFightState;
	bool bPVE = syncMsg.m_bPVEFightState;
	bool bFightState = bPVP || bPVE;
	pkPKRecorder->SetFightState(bPVP, bPVE);

	//int iGfxID = pkGamePlayer->GetPVPGfxID();
	//FX_ClientFxManager* pkFxManager = FX_ClientFxManager::GetSingletonPtr();
	////进入pvp战斗
	//if (!bOldPVP && bPVP)
	//{
	//	if (iGfxID <= 0)
	//	{
	//		iGfxID = pkFxManager->CreateFx("W_屏幕提示_泛红1.gfx", true);
	//		pkFxManager->PlayFx(iGfxID, GET_PROCESS_TIME);
	//		pkGamePlayer->SetPVPGfxID(iGfxID);
	//	}
	//}
	////退出pvp战斗
	//if (bOldPVP && !bPVP)
	//{
	//	if (iGfxID > 0)
	//	{
	//		pkFxManager->StopFx(iGfxID);
	//		pkFxManager->ReleaseFx(iGfxID);
	//		pkGamePlayer->SetPVPGfxID(0);
	//	}
	//}
	
	//如果进入战斗状态
	if (!bOldFightState && bFightState)
	{
		pkPlayer->SetNPCFunctionType(NFT_NONE);
	}

	//UI提示，进入退出战斗状态
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (NULL != pkLogicManager)
	{
		pkLogicManager->UpdatePlayerFightState(pkPlayer);
	}
}
//-------------------------------------------------------
void HL_FromGameServer::_handlerSyncShortcutBar(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	//NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	//NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	//int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	////如果还未进入游戏，则退出
	//if (eGameStage != ST_GAME_STAGE_GAME)
	//	return;

	MG_SyncShortcutBar syncMsg;
	if(!syncMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SyncShortcutBar解析错误");
		return;
	}
//origin
	/*LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(!pkGamePlayer)
		return;

	if(pkGamePlayer->GetID() != syncMsg.m_lPlayerID)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncShortcutBar无法处理，系统错误");
		return;
	}*/
//new
	
	int cID = syncMsg.m_nBuddyID;
	if (cID == 0)
	{
		return;
	}

	LC_ShortcutBar* pkShortcutBar = NULL;
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if (pkGamePlayer == NULL)
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "技能背包消息错误, 没有 Gameplayer");
		return;
	}

//更新buddyInstance
	pkShortcutBar = pkGamePlayer->GetBuddyShortcutBar(cID);
	if (pkShortcutBar != NULL)
	{
		pkShortcutBar->Clear();
		pkShortcutBar->GetDataFromProtocolStruct(syncMsg.m_akShortcutSlots);
	}
	
//更新出战角色
	object_id_type oID = pkGamePlayer->GetBuddyObjectID(cID);
	LC_PlayerBase* pkPlayer = pkGamePlayer->GetBuddyPlayerBasePtr(oID);

	if (pkPlayer != NULL)
	{
		//pkShortcutBar = pkPlayer->GetShortcutBar();
		//if (pkShortcutBar != NULL)
		//{
		//	pkShortcutBar->Clear();
		//	pkShortcutBar->GetDataFromProtocolStruct(syncMsg.m_akShortcutSlots);
		//}
		pkPlayer->SetDirtyFlag(DIRTY_FLAG_SHORTCUT,true);
		pkPlayer->AddDirtyFlagRevisionID(DIRTY_FLAG_SHORTCUT);
	}
	
//--new
}
//-------------------------------------------------------
void HL_FromGameServer::_handlerSyncGamePlayerMove(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	//NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	//NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	//int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	////如果还未进入游戏，则退出
	//if (eGameStage != ST_GAME_STAGE_GAME)
	//	return;


	MG_Sync_GamePlayerMove syncMsg;
	if(!syncMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_Sync_GamePlayerMove解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer == NULL || syncMsg.m_iPlayerID != pkGamePlayer->GetID())
		return;

	UT_Vec3Int kCurrentLocation(syncMsg.m_lCurrentLocationX,syncMsg.m_lCurrentLocationY,0);
	UT_Vec3Int kTargetLocation(syncMsg.m_lCurrentLocationX,syncMsg.m_lCurrentLocationY,0);
	pkGamePlayer->SetCurrentLocation(kCurrentLocation);
	pkGamePlayer->SetTargetLocation(kTargetLocation);
	pkGamePlayer->SetMoveLineTargetLocation(kTargetLocation);
	pkGamePlayer->GetMovePath().Clear();
}
//-------------------------------------------------------
void HL_FromGameServer::_handlerSyncGamePlayerJump(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack)
{
	MG_Rlt_PlayerJump	jmpMsg;
	if(!jmpMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_Rlt_PlayerJump解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(!pkGamePlayer)
		return;

	if (!IS_PLAYER_JUMP_STATE(jmpMsg.m_usJumpStage))
		return;

	object_id_type lRequesterID		= jmpMsg.m_lRequesterID;
	if (bArenaPlayBack)
	{
		LC_ClientArenaPlayBackManager *pkArenaPlayBackManager = LC_ClientArenaPlayBackManager::GetSingletonPtr();
		lRequesterID = pkArenaPlayBackManager->GetTemID(lRequesterID);
	}
	if(lRequesterID != pkGamePlayer->GetID())
	{
		//网络玩家挂起技能
		LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
		LC_ClientNetPlayer* pkNetPlayer = (LC_ClientNetPlayer* )pkNetPlayerManager->FindPlayer(lRequesterID);
		if(pkNetPlayer && !pkNetPlayer->GetDead())
		{
			UT_Vec3Int kCurrentLocation;
			UT_Vec3Int kTargetLocation;
			kCurrentLocation.x = jmpMsg.m_lCurrentLocationX;
			kCurrentLocation.y = jmpMsg.m_lCurrentLocationY;
			kCurrentLocation.z = jmpMsg.m_lCurrentLocationZ;
			kTargetLocation.x  = jmpMsg.m_lTargetLocationX;
			kTargetLocation.y  = jmpMsg.m_lTargetLocationY;
			kTargetLocation.z  = jmpMsg.m_lTargetLocationZ;

			LC_ActorJumpArg	arg;
			arg.kTargetLocation = kTargetLocation;
			arg.fStartTime = GET_PROCESS_TIME;
			pkNetPlayer->SetPendingJumpArg(arg);
			pkNetPlayer->SetVerticalOffset((float)jmpMsg.m_iVerticalOffset);
			pkNetPlayer->SetCurrentLocation(kCurrentLocation);

			StateMachine::StateType st = (StateMachine::StateType)jmpMsg.m_usJumpStage;
			if (RE_SKILL_NONE_ERROR == pkNetPlayer->CheckPendingJump(st)
				&& RE_SKILL_PENDING_SUCCESS == pkNetPlayer->PendingJump(st, kTargetLocation))
			{
				pkNetPlayer->SetNextState(st);
				pkNetPlayer->SetjumpType(jmpMsg.m_usJumpType);
			}
		}
	}
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncDefaultTransferPos( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_SyncDefaultTransferPos syncMsg;
	if (!syncMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SyncLogicAreaTrace解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(!pkGamePlayer)
		return;

	pkGamePlayer->SetDefaultTransferPos(syncMsg.m_iDefaultTransferMapID
		,UT_Vec3Int(syncMsg.m_iDefaultTransferPosX,syncMsg.m_iDefaultTransferPosY,syncMsg.m_iDefaultTransferPosZ));
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncSummorFriend(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	
}

//------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncTitleInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	//NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	//NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	//int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	////如果还未进入游戏，则退出
	//if (eGameStage != ST_GAME_STAGE_GAME)
	//	return;
#pragma TODO("消息调整需要修改")
 	MG_SyncTitleNameInfo syncMsg;
 	if (!syncMsg.Decode(pMsg,wSize))
 	{
 		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SyncTitleNameInfo解析错误");
 		return;
 	}
 
 	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
 	if(!pkGamePlayer||syncMsg.m_lPlayerID != pkGamePlayer->GetMainControllerID())
 		return;
 
	LC_TitleAsset* pkTitleAsset = pkGamePlayer->GetTitleAsset();
	//if (pkTitleAsset == NULL)
	//	return;

	bool bChangeTitle = false;
	int iLastID = pkTitleAsset->GetCurrentTitle();
	int iNewID = syncMsg.m_kTitleNameInfo.m_iCurrentTitleID;

	if (iLastID != iNewID)
	{
		bChangeTitle = true;
	}

	//同步数据
	pkTitleAsset->GetDataFromProtocolStruct(syncMsg);
	if(LC_TitleManager::GetSingletonPtr())
		LC_TitleManager::GetSingleton().SortTitleList(pkGamePlayer);
 
	if (bChangeTitle)
	{
		pkGamePlayer->SendMessage( CreateMsg_UE_CHANGE_PLAYER_TITLE(pkGamePlayer->GetID()) );
		
		StringType lastTitleRes = LC_TitleManager::GetSingletonPtr()->GetTitleRes(iLastID);
		StringType newTitleRes = LC_TitleManager::GetSingletonPtr()->GetTitleRes(iNewID);
		IM_Manager* pkIMManager = IM_Manager::GetSingletonPtr();

		if((lastTitleRes.find(".gfx") != StringType::npos) || (newTitleRes.find(".gfx") != StringType::npos))
		{
			GD_ActorModel* pkVisibleData = pkIMManager->GetActor(pkGamePlayer->GetID());
			if (NULL != pkVisibleData)
			{
				if(!lastTitleRes.empty())
					pkVisibleData->DetachFx(GD_ModelBase::FDT_HTC, lastTitleRes, false);
				if(!newTitleRes.empty())
					pkVisibleData->AttachFx(GD_ModelBase::FDT_HTC, FileSystem::MTF_ModelData::AFBM_TRANSLATE_ROTATE,  false, newTitleRes);
			}
		}
	}
}

void HL_FromGameServer::_handlerSyncPlayersLocationChange(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack)
{
	MG_SyncPlayerLocation playerLocationMsg;
	if(!playerLocationMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncPlayerLocation解析错误");
		return;
	}
	LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
	pkNetPlayerManager->SyncPlayerLocationInFov(playerLocationMsg, bArenaPlayBack);
}

void HL_FromGameServer::_handlerSyncPlayersAngleChange(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack)
{
	MG_SyncPlayerAngle playerAngleMsg;
	if(!playerAngleMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncPlayerLocation解析错误");
		return;
	}
	LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
	pkNetPlayerManager->SyncPlayerAngleInFov(playerAngleMsg, bArenaPlayBack);
}

void HL_FromGameServer::_handlerSyncPlayersEnter(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack)
{
	MG_SyncPlayerEnter playEnterMsg;
	if(!playEnterMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncPlayerEnter解析错误");
		return;
	}
	LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
	pkNetPlayerManager->SyncPlayerEnterInFov(playEnterMsg, bArenaPlayBack);
}

void HL_FromGameServer::_handleSyncbuddyRuneStarLevel(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncBuddyRuneStarLevelInfo buddyLevelInfo;
	if(!buddyLevelInfo.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncPlayerEnter解析错误");
		return;
	}
	LC_ClientGamePlayer* playerManager = pkSession->GetPlayer();
	playerManager->SetSkillActivateAll(buddyLevelInfo.m_nBuddyID,buddyLevelInfo.m_bActivateAllSkill);

	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	SC_ClientScriptInterface* pkClientInterface = SC_ClientScriptInterface::GetSingletonPtr();
	SC_ClientSkillInterface* pkInterface = pkClientInterface->GetSkillInterface();

	mem::vector<int> runelevel;
	runelevel.resize(4);
	std::map<int,PS_RuneItemInfo_BuddyCS>::iterator iter = buddyLevelInfo.m_nBuddyRuneStarLevelInfo.begin();
	T_ASSERT_CRITICAL_S(buddyLevelInfo.m_nBuddyRuneStarLevelInfo.size() <= 4);
	for (;iter!=buddyLevelInfo.m_nBuddyRuneStarLevelInfo.end();++iter)
	{
		LC_ItemBase* pkItem = pkItemFactory->RequestItem(iter->second.m_nRuneItemID);
		int level = iter->second.m_nRuneStarLevel;
		int quality = pkInterface->GetRuneStarQuality(pkItem->GetItemDescriptionEntry()->GetStarUpType(),level);
		runelevel[iter->first] = quality;
	}
	playerManager->SetBuddyRuneLevel(buddyLevelInfo.m_nBuddyID,runelevel);
}

void HL_FromGameServer::_handlerSyncPlayersLeave(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack)
{
	MG_SyncPlayerLeave playLeaveMsg;
	if(!playLeaveMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncPlayerLeave解析错误");
		return;
	}
	LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
	pkNetPlayerManager->SyncPlayerLeaveInFov(playLeaveMsg, bArenaPlayBack);
}

void HL_FromGameServer::_handlerSyncPlayersStateChange(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack)
{
	MG_SyncPlayerStateChange msg;
	if(!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncPlayerStateChange解析错误");
		return;
	}
	LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
	pkNetPlayerManager->SyncPlayerStateChangeInFov(msg, bArenaPlayBack);

}

void HL_FromGameServer::_handlerSyncPlayersPVEStateChange(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack)
{
	MG_SyncPlayerPVEStateChange msg;
	if(!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncPlayerPVEStateChange解析错误");
		return;
	}
	LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
	pkNetPlayerManager->SyncPlayerPVEStateChangeInFov(msg, bArenaPlayBack);

}

void HL_FromGameServer::_handlerSyncPlayersHPMPChange(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack)
{
	MG_SyncPlayerHPMPChange msg;
	if(!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncPlayerHPMPChange解析错误");
		return;
	}
	LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
	pkNetPlayerManager->SyncPlayerHPMPChangeInFov(msg, bArenaPlayBack);

}

void HL_FromGameServer::_handlerToyPetSwitchEnabledIndex(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SC_PetOp_SwitchEnableIndex msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SC_PetOp_SwitchEnableIndex解析错误");
		return;
	}

	LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
	pkNetPlayerManager->SyncToyPetEnabledIndexInFov(msg);
}

void HL_FromGameServer::_handlerSyncNpcsEnter(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack)
{
	MG_SyncNpcEnter msg;
	if(msg.Decode(pMsg,wSize))
	{
		LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
		pkNPCManager->SyncNPCEnterInFOV(msg, bArenaPlayBack);
	}

}
void HL_FromGameServer::_handlerSyncNpcsLeave(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncNpcLeave msg;
	if(msg.Decode(pMsg,wSize))
	{
		LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
		pkNPCManager->SyncNPCLeaveInFOV(msg);
	}

}

void HL_FromGameServer::_handlerSyncPlayerExecuteSkillTypeTarget(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack)
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//如果还未进入游戏，则退出
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(!pkGamePlayer)
		return;

	MG_Sync_PlayerExecuteSkillTypeTarget msg;
	if(!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_Sync_PlayerExecuteSkillTypeTarget解析错误");
		return;
	}


	//响应服务器的通知，驱动技能
	object_id_type lRequesterID		= msg.m_lReqeusterID;
	if (bArenaPlayBack)
	{
		LC_ClientArenaPlayBackManager *pkArenaPlayBackManager = LC_ClientArenaPlayBackManager::GetSingletonPtr();
		lRequesterID = pkArenaPlayBackManager->GetTemID(lRequesterID);
	}
	object_id_type lLockTargetID		= msg.m_lLockTargetID;
	int  iSkillExecuteID	= msg.m_lSkillExecuteID;
	//网络玩家挂起技能
	LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
	LC_ClientNetPlayer* pkNetPlayer = (LC_ClientNetPlayer* )pkNetPlayerManager->FindPlayer(lRequesterID);
	if(pkNetPlayer && false == pkNetPlayer->GetDead())
	{
		pkNetPlayer->SetLockedTargetID(lLockTargetID);
		pkNetPlayer->PendingSkill(iSkillExecuteID,0);
	}
}
//-------------------------------------------------------
void HL_FromGameServer::_handlerSyncArenaInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_PB<char_data_define::char_global_arena_info> syncMsg;

	if(!syncMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MGPT_SYNC_ARENA_INFO解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(!pkGamePlayer)
		return;

	LC_ArenaAsset*	pkArenaAsset =	pkGamePlayer->GetArenaAsset();
	//if (pkArenaAsset)
	{
		pkArenaAsset->GetDataFromPB(syncMsg.m_value);
	}

	pkGamePlayer->SetDirtyFlag(DIRTY_FLAG_ARENAASSET,true);
	pkGamePlayer->AddDirtyFlagRevisionID(DIRTY_FLAG_ARENAASSET);
}
//-------------------------------------------------------
void HL_FromGameServer::_handlerSyncActorOperationAsset(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( !pkGamePlayer )
		return;

	MG_GC_SyncActorOperationAsset syncMsg;
	if(!syncMsg.Decode(pMsg,wSize))
		return;

	long lOperationType = syncMsg.m_kOperationEntry.m_lOperationType;

	LC_ActorOperationEntry* pkEntry = pkGamePlayer->GetActorOperationEntry(lOperationType);
	if(NULL == pkEntry)
		return;

	pkEntry->GetDataFromProtocolStruct(syncMsg.m_kOperationEntry);
}
//-------------------------------------------------------
void HL_FromGameServer::_handlerSyncTalent(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
}

void HL_FromGameServer::_handlerSyncSupremacy(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
}

void HL_FromGameServer::_handlerSyncBerserk(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
}

void HL_FromGameServer::_handlerSyncShortcutKey(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncShortcutKeyAsset syncMsg;
	if(syncMsg.Decode(pMsg,wSize))
	{
		object_id_type lPlayerID = syncMsg.m_lPlayerID;
		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

		if (!pkGamePlayer || pkGamePlayer->GetID() != lPlayerID)
		{
			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "快捷键背包消息逻辑错误");
			return;
		}

		LC_ShortcutKeyAsset* pkShortcutKeyAsset = pkGamePlayer->GetShortcutKeyAsset();
		//if (NULL == pkShortcutKeyAsset)
		//{
		//	return;
		//}

		if (!syncMsg.m_kShortcutKeyInfo.m_akShortcutKeyAsset.empty())
		{
			pkShortcutKeyAsset->GetDataFromProtocolStruct(syncMsg.m_kShortcutKeyInfo);
			pkShortcutKeyAsset->SetLoadedServerShortcutKey(true);
		}
	}
}

/*void HL_FromGameServer::_handlerSyncPlayerFaction(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( !pkGamePlayer )
		return;

	MG_GC_SyncPlayerFaction kMsg;
	if( !kMsg.Decode(pMsg,wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_GC_SyncPlayerFaction解析错误");
		return;
	}
	pkGamePlayer->SetFactionID(kMsg.m_lfactionId);

	LC_PlayerFaction* pkFaction = pkGamePlayer->GetFactionInfo();
	if (NULL == pkFaction)
	{
		return;
	}
	unsigned long ulRelation1 = kMsg.m_lEnemyRelation1;
	unsigned long ulRelation2 = kMsg.m_lEnemyRelation2;
	unsigned long ulRelation3 = kMsg.m_lEnemyRelation3;
	unsigned long ulRelation4 = kMsg.m_lEnemyRelation4;
	pkFaction->SetAllEnemyRelation(ulRelation1, ulRelation2, ulRelation3, ulRelation4);
}*/

void HL_FromGameServer::_handlerSyncActionForbidType( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( !pkGamePlayer )
		return;

	MG_GC_SyncActionForbidInfo kMsg;
	if( !kMsg.Decode(pMsg,wSize) )
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_GC_SyncPlayerFaction解析错误");
		return;
	}

	LC_ActionForbid* pkActionForbid = pkGamePlayer->GetActionForbid();
	//if (NULL == pkActionForbid)
	//	return;
	pkActionForbid->GetDataFromProtocolStruct(kMsg.m_byActionForbidInfo);
}

void HL_FromGameServer::_handlerRltSyncPing(CS_MG_HEAD& rkHeader,NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( !pkGamePlayer )
		return;
	MG_GC_RLT_SYNC_PING rltMsg;
	if (rltMsg.Decode(pMsg,wSize))
	{
		pkGamePlayer->RltPing(rkHeader.m_usFlag,rltMsg.m_token,rltMsg.m_uiServerTime);
	}
}

//------------------------------------------------------------------------
void HL_FromGameServer::_handleSyncPortal(NW_Game2ClientSession* pkSession, char* pMsg, uint16_t wSize)
{
	MG_PortalAssert rltMsg;
	if (!rltMsg.Decode(pMsg, wSize))
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer)
	{
		pkGamePlayer->UpdatePortalData(rltMsg);
	}
}
void HL_FromGameServer::_handleSyncCorssRealmTeam(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_CrossRealmTeamAssert rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;
	
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
		return;

	pkGamePlayer->GetCrossRealmTeamAssert()->GetDataFromProtocolStruct(rltMsg);
	pkGamePlayer->GetGameStoryManager()->ReceiveCrossRealmTeamAssetSync(pkGamePlayer->GetCrossRealmTeamAssert());
}
void HL_FromGameServer::_handleSyncGreatKungFu(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncGreatKungfuInfo syncMsg;
	if (!syncMsg.Decode(pMsg, wSize))
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return;
	}
	pkGamePlayer->GetGreatKungFuAsset().GetDataFromProtocolStruct(syncMsg);
	pkGamePlayer->GetGreatKungFuAsset().SetDirtyFlag(true);
	pkGamePlayer->GetGreatKungFuAsset().AddDirtyFlagRevisionID();
}
void HL_FromGameServer::_handleSyncOpenAbilityInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncOpenAbilityInfo syncMsg;
	if (!syncMsg.Decode(pMsg, wSize))
		return;

	GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM, "进阶线开启======收到开启消息 _handleSyncOpenAbilityInfo");
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return;
	}
	LC_OpenAbilityAsset& OpenAbilityAsset = pkGamePlayer->GetOpenAbilityAsset();
	bool iFlag = OpenAbilityAsset.IsOpenAbilityInit();
	pkGamePlayer->GetOpenAbilityManager()->SetFirstTimeLoginFlag(iFlag);
	OpenAbilityAsset.GetDataFromProtocolStruct(syncMsg);
	OpenAbilityAsset.SetDirtyFlag(true);
	OpenAbilityAsset.AddDirtyFlagRevisionID();
	pkGamePlayer->GetOpenAbilityManager()->ReceiveOpenAbilityInfosync(pkGamePlayer->GetOpenAbilityAsset());
	pkGamePlayer->GetSkillAsset()->GenSkillBookSkill();
}
void HL_FromGameServer::_handleSyncDomainInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncPlayerGuildInfo syncMsg;
	if(!syncMsg.Decode(pMsg, wSize))
		return ;
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
		return;

	pkGamePlayer->GetPlayerGuildAssert()->GetDataFromProtocolStruct(syncMsg);
	pkGamePlayer->GetPlayerGuildAssert()->UpdateDirtyFlag();
	LC_GuildRaidDataManager::GetSingleton().UpdateDirtyFlag();
}

void HL_FromGameServer::_handleSyncGMInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncGMInfo syncMsg;
	if (!syncMsg.Decode(pMsg, wSize))
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return;
	}

	StringType sUser;
	sUser.append(syncMsg.m_sUser.begin(), syncMsg.m_sUser.end());
	pkGamePlayer->SetGMInfo(sUser, syncMsg.m_uiPriority);
}
void HL_FromGameServer::_handleSyncSubordinateInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncSubordinateInfo syncMsg;
	if (!syncMsg.Decode(pMsg, wSize))
		return;

	GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM, "进阶线开启======收到皮肤资产同步 _handleSyncSubordinateInfo");
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return;
	}

	LC_SubordinateAsset& kSubAsset = pkGamePlayer->GetSubordinateAsset();
	kSubAsset.GetDataFromProtocolStruct(syncMsg);
	pkGamePlayer->SetCimeliaAutoPicking(kSubAsset.IsAutoPicking());
	pkGamePlayer->AddDirtyFlagRevisionID(DIRTY_FLAG_SUBORDINATE_INFO);
}

void HL_FromGameServer::_handleSyncCommonStat(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncCommonStatInfo syncMsg;
	if (!syncMsg.Decode(pMsg, wSize))
		return;
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return;
	}
	pkGamePlayer->GetCommonStatAsset().GetDataFromProtocolStruct(syncMsg);
	pkGamePlayer->GetCommonStatAsset().UpdateDirtyFlag();

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_COMMON_STAT_INFO);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
	
}

//自己log

void HL_FromGameServer::_handleSyncTreasureChestsInfo( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_SyncTreasureChestsInfo syncMsg;
	if (!syncMsg.Decode(pMsg,wSize))
		return;
	
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return;
	}
	pkGamePlayer->GetTreasureChestsAsset().GetDataFromProtocolStruct(syncMsg);
	pkGamePlayer->GetTreasureChestsAsset().UpdateDirtyFlag();	
}

void HL_FromGameServer::_handleSyncGameStoryAsset( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_GAMESTORY_ASSET_INFO_SYNC syncMsg;
	if (!syncMsg.Decode(pMsg,wSize))
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return;
	}
	
	LC_GameStoryAsset gameStoryAsset = pkGamePlayer->GetGameStoryAsset();	

	pkGamePlayer->GetGameStoryAsset().GetDataFromProtocolStruct(syncMsg);
	pkGamePlayer->GetTreasureChestsAsset().UpdateDirtyFlag();	
}
 
void HL_FromGameServer::_hanldeSyncCommonShopInfo( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	 MG_SyncShopInfo syncMsg;
	 if (!syncMsg.Decode(pMsg,wSize))
		 return;

	 LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	 if( NULL == pkGamePlayer )
	 {
		 return;
	 }

	 if(syncMsg.m_bSyncSelf)
		pkGamePlayer->GetCommonShopAsset().GetDataFromProtocolStruct(syncMsg.m_kPlayerShopInfo);
	 if(syncMsg.m_bSyncGlobal)
		 LC_ClientShopManager::GetSingletonPtr()->GetCommonShopAsset()->GetDataFromProtocolStruct(syncMsg.m_kGlobalShopInfo);

	 pkGamePlayer->GetCommonShopAsset().UpdateDirtyFlag();		
}

void HL_FromGameServer::_handlerSyncRumorInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncRumorLogsInfo syncMsg;
	if(!syncMsg.Decode(pMsg, wSize))
		return;
	CLIENT_GET_RUMOR_MANAGER()->GetDataFromProtocolStruct(syncMsg);
}

void HL_FromGameServer::_handlerSyncGameStoryRankInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_GAMESTORY_RANK_INFO_SYNC syncMsg;
	if(!syncMsg.Decode(pMsg, wSize))
		return;

	StringStreamType sStream("");
	int iSize = syncMsg.kRanks.size();
	for(int i=0; i<iSize; ++i)
	{
		sStream << syncMsg.kRanks[i].m_iRankType << "|";
		
		int actorCount = syncMsg.kRanks[i].m_kActors.size();
		for(int j=0; j<actorCount; ++j)
		{
			sStream << syncMsg.kRanks[i].m_kActors[j].m_uiRank << "_";
			sStream << syncMsg.kRanks[i].m_kActors[j].m_szCharName << "_";
			sStream << syncMsg.kRanks[i].m_kActors[j].m_iStat << "&";
		}
	
		sStream << "#";
	}
	
	LC_ClientGameEvent kGameEvent;
	kGameEvent.SetClientEventType(GameLogic::CGET_SYN_GAMESTORY_RANKINFO);

	LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(sStream.str());
	kGameEvent.SetUserData(&kUserData);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
	
}

void HL_FromGameServer::_handlerSyncSkyArenaScore(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_GAMESTORY_FACTION_SCORE_SYNC syncMsg;
	if(!syncMsg.Decode(pMsg, wSize))
		return;

	StringStreamType sStream("");
	for (int32_t i = 0; i < GS_FACTION_SCORE_MAX; ++i)
	{
		sStream << i << "#" << syncMsg.m_kFactionScores[i]<<"|";
	}
	LC_ClientGameEvent kGameEvent;
	kGameEvent.SetClientEventType(GameLogic::CGET_SKYARENA_SCORE_INFO);

	LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(sStream.str());
	kGameEvent.SetUserData(&kUserData);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);

}
void HL_FromGameServer::_handlerSyncSkyArenaMonsterHp(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_GAMESTORY_KEY_MONSTER_HP_SYNC syncMsg;
	if(!syncMsg.Decode(pMsg, wSize))
		return;

	StringStreamType sStream("");

	sStream << syncMsg.m_kKeyMonsterHPs.size() << "#";
	for(size_t i = 0; i < syncMsg.m_kKeyMonsterHPs.size(); ++i)
	{
		GAMESTORY_KEY_MONSTER_HP &kHpInfo = syncMsg.m_kKeyMonsterHPs[i];
		sStream << kHpInfo.m_iLogicID << "#" << kHpInfo.m_iCharType << "#" << kHpInfo.m_iCurHP << "#" << kHpInfo.m_iMaxHP << "#";
	}
	LC_ClientGameEvent kGameEvent;
	kGameEvent.SetClientEventType(GameLogic::CGET_SKYARENA_MONSTERHP_INFO);

	LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(sStream.str());
	kGameEvent.SetUserData(&kUserData);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);

}
void HL_FromGameServer::_handlerSyncPlatformMessage(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_PlatformMessage msg;
	if(!msg.Decode(pMsg, wSize))
		return;

	YunYingPlatform p = getYunYingplatform(msg.m_platform.c_str());
	if (YYP_TX == p)
	{
		if (msg.m_type == "rechargeend" || msg.m_type == "yellowvipend" 
		 || msg.m_type == "bluebuygoods" || msg.m_type == "bluevipgift"
		 )
		{
			if (msg.m_content == "close" || msg.m_content == "success")
			{
				hideWebWindow();
			}
		}
	}
}

void HL_FromGameServer::_handleRltAlliance(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_Resp_Alliance_Server_List msg;
	if(!msg.Decode(pMsg, wSize))
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( pkGamePlayer)
	{
		pkGamePlayer->RltAlliance(msg.m_nGroupID, msg.m_ServerList);

		LC_ClientGameEvent kGameEvent;
		kGameEvent.SetClientEventType(CGET_RLT_ALLIANCE);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);		
	}
}

void HL_FromGameServer::_handleRltEnterAlliance( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_Resp_Alliance_Enter_CrossMap msg;
	if(!msg.Decode(pMsg, wSize))
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( pkGamePlayer)
	{
		pkGamePlayer->RltEnterAlliance(msg.m_nResult);
	}
	SendLogicInfoMessage(msg.m_nResult);
}

void HL_FromGameServer::_handleRltAllianceWinInfo( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_SS_GUILD_WAR_WIN_INFO msg;
	if ( !msg.Decode(pMsg, wSize)) 
		return ;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( pkGamePlayer)
	{
		pkGamePlayer->GetAlliance()->RltAllianceWinInfo(msg);

		LC_ClientGameEvent kGameEvent;
		kGameEvent.SetClientEventType(CGET_RLT_ALLIANCE_GUILDE_WIN);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);	
	}
}

void HL_FromGameServer::_handleRltAllianceUnitInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_Resp_Alliance_Unit_Info msg;
	if (!msg.Decode(pMsg, wSize))
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer)
	{
		LC_ClientAlliance* pkAlliance = pkGamePlayer->GetAlliance();
		//if(pkAlliance)
		{
			pkAlliance->RltAllianceUnitInfo(msg);
		}
	}
}

void HL_FromGameServer::_handleRltAllianceUnitRwd(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_Resp_RewardGain msg;
	if (!msg.Decode(pMsg, wSize))
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer)
	{
		LC_ClientAlliance* pkAlliance = pkGamePlayer->GetAlliance();
		//if(pkAlliance)
		{
			pkAlliance->RltUnitReward(msg.m_type, msg.m_rewardMap);
		}
	}
}

void HL_FromGameServer::_handleRltDrawAllianceUnitRwd(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_Resp_Alliance_Unit_Gain msg;
	if (!msg.Decode(pMsg, wSize))
		return;

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GlET_RLT_DRAW_UNIT_RWD);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);

	SendLogicInfoMessage(msg.m_nResult);
}

void HL_FromGameServer::_handleSoulRefineAssetInfo( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RTL_SoulRefineInfo msg;
	if ( !msg.Decode(pMsg, wSize)) 
		return ;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( pkGamePlayer)
	{
		pkGamePlayer->GetClientSoulRefineAsset()->GetDataFromProtocolStruct(msg.soulInfo);
		pkGamePlayer->GetSoulRefineAsset().Load(msg.soulInfo);
	}
}


void HL_FromGameServer::_handleSoulRefine( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RLT_SoulRefine msg;
	if ( !msg.Decode(pMsg, wSize)) 
		return ;

	SendLogicInfoMessage(msg.iResult);

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_CLIENT_SOUL_REFINE);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.iResult);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, msg.iPage);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_3, msg.iSoul);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}


void HL_FromGameServer::_handleSoulRefineLevelUp( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RTL_SoulRefineLvlUp msg;
	if ( !msg.Decode(pMsg, wSize)) 
		return ;

	SendLogicInfoMessage(msg.iResult);
}

void HL_FromGameServer::_handleFashionAssetInfo( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RTL_FashionInfo msg;
	if(!msg.Decode(pMsg, wSize))
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(!pkGamePlayer)
		return;

	pkGamePlayer->GetFashionBagAsset().Load(msg);
}

void HL_FromGameServer::_handleFashionShow(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_PB<msg_define::fashion_show_resp> msg;
	if (!msg.Decode(pMsg,wSize))
		return;	

	SendLogicInfoMessage(msg.m_value.rslt());
}

void HL_FromGameServer::_handleShowFashion(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_ShowFashion msg;
	if(!msg.Decode(pMsg, wSize))
		return;

	SendLogicInfoMessage(msg.m_iResult);
}


void HL_FromGameServer::_handleSyncActivityInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_PB<char_data_define::char_activity_info> rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return;
	}

	pkGamePlayer->GetClientActivityAsset()->GetDataFromPB(rltMsg.m_value);
}

void HL_FromGameServer::_handleSyncUpdateBulletinInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncUpdateBulletin rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	StringStreamType sStr;
	sStr << Int2String(rltMsg.m_bIsNew ? 1 : 0) << CodeConvert::Utf8ToMbs(rltMsg.m_sUpdateBulletin.c_str());
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerMsgGameEvent(MGPT_SYNC_UPDATE_BULLETIN_INFO, sStr.str());
}

void HL_FromGameServer::_handleSyncHolydeed(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncHolyDeed rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;
	
	StringStreamType sStream;
	std::map<int32_t,int32_t>::iterator iter = rltMsg.m_nHolyDeedRecord.begin();
	LC_ClientGamePlayer *pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return ;
	}
	for(; iter!=rltMsg.m_nHolyDeedRecord.end(); ++iter)
	{
		pkGamePlayer->SetHolydeedUserCount(iter->first,iter->second);
	}
}

void HL_FromGameServer::_handleSyncBossDropinfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncBossDropInfo rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	LC_ClientGamePlayer *pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->SetBossDropInfo(rltMsg);
	
	StringStreamType sStream;
	sStream << rltMsg.m_nType << "#" << rltMsg.m_nDropTimes << "#" << rltMsg.m_nNextRevertTime << "#" << rltMsg.m_nEndRevertTime;
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerMsgGameEvent(MGPT_SYNCBOSSDROPINFO, sStream.str());
}

void HL_FromGameServer::_handleSyncTestDataBase(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncTestDatabase rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;
	GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"收到题目索引，size=%d",rltMsg.m_topicID.size());
	LC_ClientGamePlayer* playerManager = pkSession->GetPlayer();
	playerManager->SetTestDataBase(rltMsg.m_topicID);
}

void HL_FromGameServer::_handleSyncAnswerInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncAnswerInfo rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	StringStreamType sStream;
	sStream << rltMsg.m_nAnswerAccPoint << "#";
	for (int i=0;i<rltMsg.m_nAnswerRewardID.size();++i)
	{
		sStream << rltMsg.m_nAnswerRewardID[i] << "|";
	}
	sStream << "#";
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerMsgGameEvent(MGPT_SYNCANSWERINFO, sStream.str());
}

void HL_FromGameServer::_handleSyncInvestmentCardRecord(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncInvestmentCardRecord rltMsg;
	if (!rltMsg.Decode(pMsg, wSize))
		return;

	LC_ClientGamePlayer *pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return ;
	}
	int len = rltMsg.m_nInvestmentTypeID.size();
	pkGamePlayer->ReSetInvestmentType();
	for (int i=0;i<len;i++)
	{
		pkGamePlayer->SetInvestmentType(rltMsg.m_nInvestmentTypeID[i]);
	}
}

void HL_FromGameServer::_handleSyncInvestmentCardRewardRecord(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncInvestmentCardRewardRecord rltMsg;
	if (!rltMsg.Decode(pMsg, wSize))
		return;

	LC_ClientGamePlayer *pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return ;
	}
	pkGamePlayer->ReSetInvestmentFreeReward();
	int len = rltMsg.m_nFreeRewardAchievementID.size();
	for (int i=0;i<len;i++)
	{
		pkGamePlayer->SetInvestmentFreeReward(rltMsg.m_nFreeRewardAchievementID[i]);
	}
	len = rltMsg.m_nHighRewardAchievementID.size();
	for (int i=0;i<len;i++)
	{
		pkGamePlayer->SetInvestmentHighReward(rltMsg.m_nHighRewardAchievementID[i]);
	}
}

void HL_FromGameServer::_handleSyncRefreshRewardTaskTimes(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncRefreshRewardTaskTimes rltMsg;
	if (!rltMsg.Decode(pMsg, wSize))
		return;

	LC_ClientGamePlayer *pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->SetOfferTaskRefreshTimes(rltMsg.m_nTimes);
	StringStreamType sStream;
	sStream << rltMsg.m_nTimes;
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerMsgGameEvent(MGPT_SYNCREFRESHREWARDTASKTIMES, sStream.str());
}

void HL_FromGameServer::_handleSyncOfferTaskAcceptTimes(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncAcceptRewardTaskTimes rltMsg;
	if (!rltMsg.Decode(pMsg, wSize))
		return;

	LC_ClientGamePlayer *pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->SetOfferTaskAcceptTimes(rltMsg.m_nAcceptRewardTaskTimes);
	StringStreamType sStream;
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerMsgGameEvent(MGPT_SYNCACCEPTREWARDTASKTIMES, sStream.str());
}

void HL_FromGameServer::_handleSyncAddnewHolydeed(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_NotifyAddNewHolyDeed rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	StringStreamType sStream;
	sStream << rltMsg.m_nHolyDeedID;
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerMsgGameEvent(MGPT_NOTIFY_ADDNEW_HOLYDEED, sStream.str());
}

void HL_FromGameServer::_handleSyncPlayerControl( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_PB<msg_define::char_control_info> rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	msg_define::char_control_info& rk_control = rltMsg.m_value;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return ;
	}

	object_id_type oldID = pkGamePlayer->GetID();
	pkGamePlayer->ResetContronlID();
	
	if ( rk_control.has_self() )
	{
		if ( rk_control.mutable_self()->objectid() == pkGamePlayer->GetSelfID() )
		{
			for(int i = 0 ; i < rk_control.controls_size(); i++)
			{
				const msg_define::control_entry& entry = rk_control.controls(i);
				if ( entry.objectid() > 0 )
				{
					IM_Manager::GetSingletonPtr()->ModifyPlayerModeID(oldID, entry.objectid());
					IM_Manager::GetSingletonPtr()->ModifyRiderModeID(oldID, entry.objectid());
					IM_Manager::GetSingletonPtr()->ModifyFabaoModeID(oldID, entry.objectid());
					IM_Manager::GetSingletonPtr()->ModifyAnQiModeID(oldID, entry.objectid());
					pkGamePlayer->AddControlID(entry.objectid());
					
					LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
					pkLogicManager->SetInputControlActorID(entry.objectid(), false);

					if ( pkGamePlayer->GetDead() )
					{
						pkGamePlayer->SaveDeadTimeForC(pkGamePlayer->GetDeadTime());

						//如果是死亡状态下进入PVP，控制者肯定是活着的
						attr_value_type maxhp = pkGamePlayer->GetAttributeMap()->GetAttribute(ATT_MAX_HP);
						pkGamePlayer->Respawn(maxhp,0);
					}

					if ( IS_SKILL_TYPE(pkGamePlayer->GetCurrentStateType()) )
					{
						pkGamePlayer->InterruptSkill(GET_PROCESS_TIME,pkGamePlayer->GetCurrentStateType(),SKI_FORCE);
					}

					if ( pkGamePlayer->GetShapeShiftFlag() )
					{
						pkGamePlayer->SetShapeShiftFlag(false);
						pkGamePlayer->SetShapeShiftCharTypeID(0);
						pkGamePlayer->ChangeVisibleData(0);
						pkGamePlayer->DoUpdateBowAvatar();	
						pkGamePlayer->DoUpdateWingAvatar();
					}
				}
				else
				{
					LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
					pkLogicManager->SetInputControlActorID(pkGamePlayer->GetID(), false);
					IM_Manager::GetSingletonPtr()->ModifyPlayerModeID(oldID, pkGamePlayer->GetID());
					IM_Manager::GetSingletonPtr()->ModifyRiderModeID(oldID, pkGamePlayer->GetID());
					IM_Manager::GetSingletonPtr()->ModifyFabaoModeID(oldID, pkGamePlayer->GetID());
					IM_Manager::GetSingletonPtr()->ModifyAnQiModeID(oldID, pkGamePlayer->GetID());

					if ( pkGamePlayer->GetSaveDeadTimeForC() > 0 )
					{
						pkGamePlayer->SetDead(true);
						pkGamePlayer->SetDeadTime(pkGamePlayer->GetSaveDeadTimeForC());
					}
					else
					{
						//只是控制者死了，本身没死，先吧血量复原，等待服务器同步
						attr_value_type maxhp = pkGamePlayer->GetAttributeMap()->GetAttribute(ATT_MAX_HP);
						pkGamePlayer->Respawn(maxhp,0);
					}

					if ( pkGamePlayer->GetShapeShiftFlag() )
					{
						pkGamePlayer->SetShapeShiftFlag(false);
						pkGamePlayer->SetShapeShiftCharTypeID(0);
						pkGamePlayer->ChangeVisibleData(0);
						pkGamePlayer->DoUpdateBowAvatar();	
						pkGamePlayer->DoUpdateWingAvatar();
					}

				}
			}
		}
	}
}

//************************************************************************
void HL_FromGameServer::_handleRltGamePlayerDailyActivity(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncDailyActivity rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return ;
	}
	pkGamePlayer->RltSyncGamePlayerDailyActivity(rltMsg);

}
//************************************************************************
void HL_FromGameServer::_handlerRltCollection(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RLT_CollectionActivation kRlt;
	if (!kRlt.Decode(pMsg, wSize))
	{
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();	
	if( NULL == pkGamePlayer )
	{
		return ;
	}
	pkGamePlayer->RltUpgradCollection(kRlt);

}
//************************************************************************
void HL_FromGameServer::_handlerRltCollectionSuit(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RLT_CollectionReward kRlt;
	if (!kRlt.Decode(pMsg, wSize))
	{
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();	
	if( NULL == pkGamePlayer )
	{
		return ;
	}
	pkGamePlayer->RltUpgradCollectionSuit(kRlt);
}
//************************************************************************
void HL_FromGameServer::_handlerSyncACTSubInfoEnd(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_SyncPlayerACTSubInfo kRlt;
	if (!kRlt.Decode(pMsg, wSize))
	{
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();	
	if( NULL == pkGamePlayer )
	{
		return ;
	}
	pkGamePlayer->SyncACTSubInfoEnd(kRlt);
}
//************************************************************************
void HL_FromGameServer::_handleRltCharacterUpgradGift(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncBuddyGrowthPackage rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return ;
	}
	pkGamePlayer->RltSyncCharacterUpgradGiftData(rltMsg);

}
//************************************************************************
void HL_FromGameServer::_handleRltGamePlayerDailyCharge(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncFDDay rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return ;
	}
	pkGamePlayer->RltSyncGamePlayerDailyCharge(rltMsg);


	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_DAILY_CHARGE_RECORD);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);

}
//************************************************************************
void HL_FromGameServer::_handleRltGamePlayerDailyChargeTime(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_RewardTime rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return ;
	}

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_DAILY_CHARGE_TIMER);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, rltMsg.m_nResult);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, rltMsg.m_time);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);

	//pkGamePlayer->RltSyncGamePlayerDailyChargeTime(rltMsg);

}
//************************************************************************
void HL_FromGameServer::_handleRltGamePlayerDailyChargeRecord(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncFDRewardRecord rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return ;
	}
	pkGamePlayer->RltSyncGamePlayerDailyChargeRecord(rltMsg);

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_DAILY_CHARGE_RECORD);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);

}
//************************************************************************
void HL_FromGameServer::_handleRltGamePlayerCollectionRecord(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncCollectionInfo rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return ;
	}
	pkGamePlayer->RltSyncGamePlayerCollection(rltMsg);

	/*LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_DAILY_CHARGE_RECORD);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);*/

}
//************************************************************************
void HL_FromGameServer::_handleRltGamePlayerBossFirstKill(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncBossFirstKillInfo rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return ;
	}
	pkGamePlayer->RltSyncGamePlayerBossFirstKill(rltMsg);

}

void HL_FromGameServer::_handleSyncKillBossReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncKillBossReward msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncKillBossReward解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->SyncFirstKillBossReward(msg);
}

void HL_FromGameServer::_handleRltKillBossReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_KillBossReward msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_RLT_KillBossReward解析错误");
		return;
	}

	if(msg.m_nResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_nResult);
		return;
	}
	
	if(msg.m_bServerReward)
	{
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_SYNC_BOSSFIRSTKILL_PLAYERINFO);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_nNpcCharID);
		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
}

void HL_FromGameServer::_handleSyncServerFirstSkillBossReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncServerFirstKillBossReward msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncServerFirstKillBossReward解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->RltSyncServerBossFirstKillRecord(msg);
}

void HL_FromGameServer::_handleSyncSevenDayFinishTime(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncSevenDayFinishTime msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncSevenDayFinishTime解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->SetSevenDaysActivityEndTime(msg.m_nSevenDayEndTime1);
	pkGamePlayer->SetSevenDaysRewardEndTime(msg.m_nSevenDayEndTime2);
}

void HL_FromGameServer::_handleSyncFirstDayLoginRewardTime(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncFirstDayLoginRewardTime msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncFirstDayLoginRewardTime解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->SetFirstDayLoginRewardTime(msg.m_nFirstDayLoginRewardTime);

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_SYNC_FIRSTDAYLOGINREWARDTIME);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_nFirstDayLoginRewardTime);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handleSyncCommanderTitleRecord(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncCommanderTitleRecord msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncCommanderTitleRecord解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->SetCommanderActiviteFrame(msg);

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_SYNC_COMMANDER_HEADORFRAMELIST_CHANGE);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handleSyncProFilePictureRecord(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncProfilePictureRecord msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncProfilePictureRecord解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer  == NULL)
	{
		return;
	}

	pkGamePlayer->SetCommanderActiviteHead(msg);

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_SYNC_COMMANDER_HEADORFRAMELIST_CHANGE);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handleRltSetProfilePictureFlag(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_SetProfilePictureFlag msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_RLT_SetProfilePictureFlag解析错误");
		return;
	}

	if (msg.m_nResult != RE_SUCCESS)
		SendLogicInfoMessage(msg.m_nResult);
}

void HL_FromGameServer::_handleRltSetCommanderTitleFlag(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_SetCommanderTitleFlag msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_RLT_SetCommanderTitleFlag解析错误");
		return;
	}

	if (msg.m_nResult != RE_SUCCESS)
		SendLogicInfoMessage(msg.m_nResult);
}

//************************************************************************
void HL_FromGameServer::_handleRltGamePlayerSuperShopCompete(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncACTActiveRewardInfo rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return ;
	}
	pkGamePlayer->RltSyncGamePlayerCompeteReward(rltMsg);

}

void HL_FromGameServer::_handleSyncActActiveRewardInfoB(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncACTActiveRewardInfoB syncMsg;
	if(!syncMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncACTActiveRewardInfoB解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return ;
	}

	pkGamePlayer->RltSyncGamePlayerSubReward(syncMsg);
}
//************************************************************************
void HL_FromGameServer::_handleRltSyncBotData(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SC_BotDataSync rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return ;
	}
	pkGamePlayer->RltSyncBotData(rltMsg);

}
//************************************************************************
void HL_FromGameServer::_handleRltBotProfit(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SC_BotRewardResult rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return ;
	}
	pkGamePlayer->RltBotProfit(rltMsg);

}
//************************************************************************
void HL_FromGameServer::_handleSyncBotProfit(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SC_BotBonusCacheSync rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return ;
	}
	pkGamePlayer->RltSyncBotProfitData(rltMsg);

}
//************************************************************************
void HL_FromGameServer::_handleSyncBotBgeinTime(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SC_BotDataSync_BeginTime rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return ;
	}
	pkGamePlayer->RltSyncBotBeginTime(rltMsg);

}
//************************************************************************
void HL_FromGameServer::_handleRltDeCompositionEquips(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_DecompositionItem rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return ;
	}
	pkGamePlayer->RltDeCompositionEquips(rltMsg);

}
//************************************************************************
void HL_FromGameServer::_handleRltDeCompositionEquipsSetting(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_DecompositionSetting rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return ;
	}
	pkGamePlayer->RltDecompositionSetting(rltMsg);

}
void HL_FromGameServer::_handleRltSetAutoDecompositionItem(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_SetAutoDecompositionItem rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	if (rltMsg.m_nResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(rltMsg.m_nResult);
		return;
	}

	StringStreamType sStream("");
	sStream << rltMsg.m_nAutoDecomposition;
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerMsgGameEvent(MGPT_RLT_SETAUTODECOMPOSITIONITEM, sStream.str());
}
//************************************************************************
void HL_FromGameServer::_handleRltBiographyReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_BiographyPartReward rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return ;
	}
	pkGamePlayer->RltBiographyReward(rltMsg);
}
//************************************************************************
void HL_FromGameServer::_handleSyncBiographyChapters(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncBiographyPartReward rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return ;
	}
	pkGamePlayer->SyncBiographyChapters(rltMsg);
}
//************************************************************************
void HL_FromGameServer::_handleSyncBiography(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncBiography rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return ;
	}
	
	pkGamePlayer->SyncBiography(rltMsg);

		//LC_GameEvent kGameEvent;
		//kGameEvent.SetEventType(GLET_PLAYER_DEAD);
		//kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_CurRespawnTime);
		//kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, msg.m_SafeRespawnTime);

		//UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		//pkUIManager->HandlerGameEvent(kGameEvent);
}
//************************************************************************
void HL_FromGameServer::_handleSyncTimeLimitShop(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncPlayerTimeLimitedActivity rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return ;
	}
	pkGamePlayer->SyncTimeLimitShop(rltMsg);
}
//************************************************************************
void HL_FromGameServer::_handleRltTimeLimitShop(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncTimeLimitedActivityState rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return ;
	}
	pkGamePlayer->RltTimeLimitShop(rltMsg);
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncGameplayerStarMoney(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncPlayerPentacle rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return ;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return ;
	}
	
	//星币增加
	MoneyType oldValue = pkGamePlayer->GetPentacle().GetMoney();
	MoneyType newValue = rltMsg.m_lPentacle;
	MoneyType delta = newValue - oldValue;
	if(newValue > 0 && oldValue > 0 && delta > 0)
	{
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_GAIN_MONEY);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, CT_PENTACLE);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, delta);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}

	pkGamePlayer->SyncGamePlayerStarMoney(rltMsg);
}
//************************************************************************
void HL_FromGameServer::_handleRltSyncActiveBotData(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SC_BotActivateResult rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return ;
	}
	pkGamePlayer->RltActiveBot(rltMsg);

}
//************************************************************************
void HL_FromGameServer::_handleRltSyncUpgradBotData(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SC_BotUpgradeResult rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return ;
	}
	pkGamePlayer->RltUpgradBot(rltMsg);

}
//************************************************************************
void HL_FromGameServer::_handlerRltNPCDead(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack)
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//如果还未进入游戏，则退出
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;

	MG_RLT_NPCDead msg;
	if(!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"怪物死亡消息解析错误");
		return;
	}

	LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
	pkNPCManager->RltNPCDead(msg, bArenaPlayBack);
}

//************************************************************************
void HL_FromGameServer::_handlerRltPlayerDead(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack)
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//如果还未进入游戏，则退出
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;

	MG_RLT_PlayerDead msg;
	if(!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_RLT_PlayerDead解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_RLT_PlayerDead逻辑错误");
		return;
	}

	object_id_type lID = msg.m_lPlayerID;
	if (bArenaPlayBack)
	{
		LC_ClientArenaPlayBackManager *pkArenaPlayBackManager = LC_ClientArenaPlayBackManager::GetSingletonPtr();
		lID = pkArenaPlayBackManager->GetTemID(lID);
	}
	LC_PlayerBase* pkPlayer = pkGamePlayer->GetBuddyPlayerBasePtr(lID);
	if (pkPlayer != NULL)
	{
		pkPlayer->SetHP(0);
		pkPlayer->SetDead(true);
		pkPlayer->SetDeadAnimationPlay(3);

		pkGamePlayer->ResetSwapCoolDownByCID(pkPlayer->GetCharType());

		GfxWriteLog(LOG_TAG_SKILL, LOG_SWITCH_SKILL_EVENT,
			"[Player Dead], playerID=%d, buddyID=%d",
			msg.m_lPlayerID, pkPlayer->GetCharType());
	}

	pkGamePlayer->SetDirtyFlag(DIRTY_FLAG_ACTIVEBUDDYLIST, true);
	pkGamePlayer->AddDirtyFlagRevisionID(DIRTY_FLAG_ACTIVEBUDDYLIST);
	//pkGamePlayer->SetRespawnPunishLevel(msg.m_lRespawnPunishLevel);

	//这里判定如果当前角色已死亡 并且 所有BUDDY均已死亡就向UI发出复活事件
	//if(msg.m_lPlayerID == pkGamePlayer->GetID() && pkGamePlayer->IsAllBuddyDead())
	//{
	//	LC_GameEvent kGameEvent;
	//	kGameEvent.SetEventType(GLET_PLAYER_DEAD);
	//	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_CurRespawnTime);
	//	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, msg.m_SafeRespawnTime);

	//	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	//	pkUIManager->HandlerGameEvent(kGameEvent);
	//}	
}

void HL_FromGameServer::_handlerRltUserDead(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RLT_UserDead msg;
	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_RLT_UserDead解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(!pkGamePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_RLT_UserDead逻辑错误, 没有GamePlayer");
		return;
	}

	pkGamePlayer->SetNPCFunctionType(NFT_NONE);
	//挂机普通复活后 自动寻路状态下死亡
	if(pkGamePlayer->GetHookRespawnPathFlag())
	{
		pkGamePlayer->SetHookRespawnPathFlag(false);
		pkGamePlayer->SetHookRespawnDeadFlag(true);
	}

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_PLAYER_DEAD);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_CurRespawnTime);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, msg.m_SafeRespawnTime);

	LC_GameEvent kGameEvent2;
	kGameEvent2.SetEventType(GLET_SYNC_IGNORE_UI);
	kGameEvent2.SetEventParam(EVENT_BASE_PARAM_1, false);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent2);
	pkUIManager->HandlerGameEvent(kGameEvent);

	GfxWriteLog(LOG_TAG_SKILL, LOG_SWITCH_SKILL_EVENT, "[UserDead] Receive User Dead");
}

//*********************************** SyncNetPlayerCommander BEGIN
void HL_FromGameServer::_handlerSYNCPlayerControllersPropertyInFOV(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack)
{
	MG_SyncPlayerControllersPropertyInFOV msg;
	if(!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncPlayerControllersPropertyInFOV解析错误");
		return;
	}

	LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
	if(pkNetPlayerManager == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncPlayerControllersPropertyInFOV解析错误, 没有netplayer_manager");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(!pkGamePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncPlayerControllersPropertyInFOV逻辑错误, 没有GamePlayer");
		return;
	}

	LC_ClientArenaPlayBackManager *pkArenaPlayBackManager = LC_ClientArenaPlayBackManager::GetSingletonPtr();
	LC_ClientNetPlayer* pkPlayer = NULL;
	for(int i=0; i<msg.m_usEntryCount; ++i)
	{
		object_id_type lID = msg.m_rEntries[i].m_lSelfID;
		if (bArenaPlayBack)
		{
			if (!pkArenaPlayBackManager->IsPlayerInList(lID))
				continue;
			lID = pkArenaPlayBackManager->GetTemID(lID);
		}
		else if (lID == pkGamePlayer->GetID())
			continue;

		pkPlayer = (LC_ClientNetPlayer*)pkNetPlayerManager->FindPlayer(lID);
		if(pkPlayer != NULL)
		{
			LC_ClientNetPlayerCommander* commander = pkPlayer->GetCommander();
			if(commander != NULL)
				commander->Construct(msg.m_rEntries[i], bArenaPlayBack);
			else
				GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncPlayerControllersPropertyInFOV解析错误, ID为 %i 的netplayer没有commmander", msg.m_rEntries[i].m_lControllerID);
		}
		else
		{
			GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncPlayerControllersPropertyInFOV解析错误, 没有ID为 %i 的netplayer", lID);
		}
	}
}

void HL_FromGameServer::_handlerNetPlayerActiveBuddy(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack)
{
	MG_RefreshPlayerMainInfo msg;
	if(!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_RefreshPlayerMainInfo解析错误");
		return;
	}

	LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
	if(pkNetPlayerManager == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_RefreshPlayerMainInfo解析错误, 没有netplayer_manager");
		return;
	}

	LC_ClientArenaPlayBackManager *pkArenaPlayBackManager = LC_ClientArenaPlayBackManager::GetSingletonPtr();
	size_t num = msg.m_lActivePlayerIDs.size();
	LC_ClientNetPlayer* player = NULL;
	for(int i=0; i<num; ++i)
	{
		object_id_type lID = msg.m_lActivePlayerIDs[i];
		if (bArenaPlayBack)
			lID = pkArenaPlayBackManager->GetTemID(lID);
		player = (LC_ClientNetPlayer*)pkNetPlayerManager->FindPlayer(lID);
		if (NULL != player) {
			LC_ClientNetPlayerCommander* commander = player->GetCommander();
			if (commander != NULL) {
				if (bArenaPlayBack)
					commander->m_lCurrentPlayerID = pkArenaPlayBackManager->GetTemID(msg.m_lCurrentPlayerID);
				else
					commander->m_lCurrentPlayerID = msg.m_lCurrentPlayerID;
				commander->m_GuildName = msg.m_GuildName.c_str();
				commander->m_TitleID = msg.m_TitleID;
				commander->m_iUiFlag = msg.m_iUiFlag;
				commander->m_VipLevel = msg.m_VipLevel;
			}
			player->SendMessage( CreateMsg_UE_CHANGE_PLAYER_TITLE(player->GetID()) );
		}
	}
}
//************************************** SyncNetPlayerCommander END

void HL_FromGameServer::_handleSyncSingleCombatRecorder(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SC_SingleCombatRecorder msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SC_SingleCombatRecorder解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(!pkGamePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SC_SingleCombatRecorder逻辑错误, 没有GamePlayer");
		return;
	}
	
	const int iOldRank = pkGamePlayer->GetSingleCombatRank(msg.m_nKey);
	pkGamePlayer->UpdateSingleCombatData(msg);

	GfxWriteLog(LOG_TAG_GAME_LOG, LOG_SWITCH_GAME,"[RaidMap]HL_FromGameServer::_handleSyncSingleCombatRecorder rank=%d damage=%I64d oldRank=%d type=%d param=%d", 
		pkGamePlayer->GetSingleCombatRank(msg.m_nKey), msg.m_nDamageValue, iOldRank, msg.m_nType, msg.m_nParam);

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_SYNC_SINGLECOMBAT_RECORDER);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, pkGamePlayer->GetSingleCombatRank(msg.m_nKey));
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, msg.m_nDamageValue);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_3, iOldRank);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_4, msg.m_nType);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_5, msg.m_nParam);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handleRltSetCurrentCommanderTitleID(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_SetCurrentCommanderTitleID msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_RLT_SetCurrentCommanderTitleID解析错误");
		return;
	}
	if (msg.m_nResult == RE_SUCCESS)
	{
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_SYNC_COMMANDER_FRAME_CHANGESUCCESS);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_nCurrentCommanderTitleID);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
	else
		SendLogicInfoMessage(msg.m_nResult);
}

void HL_FromGameServer::_handleRltSetProfilePicture(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_SetProfilePicture msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_RLT_SetProfilePicture解析错误");
		return;
	}
	if (msg.m_nResult == RE_SUCCESS)
	{
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_SYNC_COMMANDER_HEAD_CHANGESUCCESS);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_nProfilePictureID);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
	else
		SendLogicInfoMessage(msg.m_nResult);
}

void HL_FromGameServer::_handleSyncCollectBossJP(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncCollectBossJP msg;
	if (!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncCollectBossJP解析错误");
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->SetCollectBossJPCount(msg.m_nCollectBossJP);
}

void HL_FromGameServer::_handleSyncBuddyEquipLv(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncEquipSlotLv msg;
	if (!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncEquipSlotLv解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->SetBuddyEquipShowLevel(msg.m_nBuddyId, msg.m_nLv);

	LC_PackAsset* pkPackAsset = pkGamePlayer->GetPackAsset();
	pkPackAsset->SetDirtyFlag(PACK_TYPE_EQUIP_SLOT, true);
	pkPackAsset->AddDirtyFlagRevisionID(PACK_TYPE_EQUIP_SLOT);
}

void HL_FromGameServer::_handleRltSetCommanderEquipSuitLevel(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_SetEquipSuitLevel msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_RLT_SetEquipSuitLevel解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if (!pkGamePlayer)
	{
		return;
	}

	if(msg.m_nResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_nResult);
	}
	else
	{
		pkGamePlayer->GetCommander()->SetEquipSuitLevel(msg.m_nEquipSuitLevel);

		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_SYNC_COMMANDER_EQUIOP_SUITLEVEL);
		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
}

void HL_FromGameServer::_handleSyncCommanderEquipSuitLevel(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncEquipSuitLevel msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncEquipSuitLevel解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if (!pkGamePlayer)
	{
		return;
	}

	pkGamePlayer->GetCommander()->SetEquipSuitLevel(msg.m_nEquipSuitLevel);

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_SYNC_COMMANDER_EQUIOP_SUITLEVEL);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerGuildBattleTowerInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_Notify_Guild_Fight_Tower_Info msg;
	if (!msg.Decode(pMsg,wSize))
	{
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(!pkGamePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_Notify_Guild_Fight_Tower_Info逻辑错误, 没有GamePlayer");
		return;
	}
	pkGamePlayer->SetGuildBattleTowerState(msg.m_TowerVec);
	
	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_GUILDBATTLE_TOWERINFO_CHANGE);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerGuildBattleTowerInfoLife(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_Notify_Guild_Fight_Tower_Info_lite msg;
	if (!msg.Decode(pMsg,wSize))
	{
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(!pkGamePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_Notify_Guild_Fight_Tower_Info_lite逻辑错误, 没有GamePlayer");
		return;
	}
	pkGamePlayer->SetGuildBattleTowerState(msg.towerData);

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_GUILDBATTLE_TOWERINFO_CHANGE);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerGuildBattleTowerRankList(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_GAMESTORY_ACTOR_ENCOUNTED_STAT_NOTIFY_EX msg;
	if (!msg.Decode(pMsg,wSize))
	{
		return;
	}

	StringStreamType sStream;
	sStream << msg.m_iStroyID << "#" << msg.m_nDamageType << "#";
	sStream << msg.m_kSelfStats.m_uiRank << "|" << msg.m_kSelfStats.m_szCharName << "|" << msg.m_kSelfStats.m_uiCharID << "|" << msg.m_kSelfStats.m_iStat << "#";
	for (int i=0;i<msg.m_kStats.size();++i)
		sStream << msg.m_kStats[i].m_uiRank << ";" << msg.m_kStats[i].m_szCharName << ";" << msg.m_kStats[i].m_uiCharID << ";" << msg.m_kStats[i].m_iStat << "|";
	sStream << "#";

	LC_ClientGameEvent kCGameEvent;
	kCGameEvent.SetClientEventType(CGET_BESTGUILD_TOWER_RANKLIST);
	LC_ClientGameEventUserData_String kUserData(sStream.str());
	kCGameEvent.SetUserData(&kUserData);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kCGameEvent);
}

void HL_FromGameServer::_handleRltGuildFightRankInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_GuildFightRankInfo msg;
	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_RLT_GuildFightRankInfo解析错误");
		return;
	}

	GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"收到公会战排行榜信息，m_nResult=%d, m_nType1=%d, m_nStroyId=%d, m_nTotalCount=%d, ",msg.m_nResult, msg.m_nType1, msg.m_nStroyId, msg.m_nTotalCount);
	if (msg.m_nResult == RE_SUCCESS)
	{
		StringStreamType sStream;
		sStream << msg.m_nType1 << "#" << msg.m_nStroyId << "#" << msg.m_nTotalCount << "#";
		sStream << msg.m_nMyselfInfo.m_nRankIndex << "|" << msg.m_nMyselfInfo.m_nGuildName << "|";
		sStream << msg.m_nMyselfInfo.m_nPlayerName << "|" << msg.m_nMyselfInfo.m_nTitleId << "|";
		sStream << msg.m_nMyselfInfo.m_nPoint << "|" << msg.m_nMyselfInfo.m_nGuild << "#";
		for (int i=0;i<msg.m_nPlayerInfo.size();++i)
		{
			const PS_RANK_STRUCT& info = msg.m_nPlayerInfo[i];
			sStream << info.m_nRankIndex << ";" <<info.m_nGuildName << ";" << info.m_nPlayerName << ";";
			sStream << info.m_nTitleId << ";" << info.m_nPoint << ";" << info.m_nGuild << ";" << info.m_uGuildMasterId << "|";
		}
		sStream << "#";
		std::map<int32_t,int32_t>::iterator iter = msg.m_nMyHistoryInfo.begin();
		for (; iter!=msg.m_nMyHistoryInfo.end();++iter)
			sStream << iter->first << ":" << iter->second << ";";
		sStream << "#";

		LC_ClientGameEvent kCGameEvent;
		kCGameEvent.SetClientEventType(CGET_BESTGUILD_RANKLIST);
		LC_ClientGameEventUserData_String kUserData(sStream.str());
		kCGameEvent.SetUserData(&kUserData);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kCGameEvent);
	}
}

void HL_FromGameServer::_handleRespGuildBattleFightRecord(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RESP_FightRecord msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_RESP_FightRecord解析错误");
		return;
	}

	StringStreamType sStream;
	sStream << msg.m_nBeFighted << '&';

	std::vector<SingleFightRecord>::const_iterator iter = msg.m_vecRecord.begin();
	for( ; iter!=msg.m_vecRecord.end(); ++iter)
	{
		const SingleFightRecord& record = *iter;
		sStream << record.m_nTime << '#' << record.m_nScore << '#';
		
		const CommanderBaseInfo& cmdIf = record.m_AttackInfo.m_CommanderBaseInfo;
		sStream << cmdIf.m_nControllerLevel << '#' << STDSTR2TPSTR(cmdIf.m_nCharName ) << '#' << cmdIf.m_nCommanderTitleID << '#';
		sStream << cmdIf.m_nCommanderTitleFrameID << '#' << cmdIf.m_nBornType << '#' << cmdIf.m_uCharId << '#';
		
		const std::vector<PS_Buddy>& buddys = record.m_AttackInfo.m_nVecBuddyInfo;
		std::vector<PS_Buddy>::const_iterator buddyItr = buddys.begin();
		for( ; buddyItr!=buddys.end(); ++buddyItr)
		{
			const PS_Buddy& info = *buddyItr;
			const PS_Buddy_Info_BuddyCS& buddyInfo = info.m_nBuddyData;
			sStream <<  buddyInfo.m_nBuddyID << '@' << info.m_nBuddyIndex << '@';
			sStream  << info.m_nDamageData << '@' << info.m_nHealData << '@' << info.m_nHurtData << '@';
			sStream << buddyInfo.m_nBuddyLevel << '@' << buddyInfo.m_nBuddyStarLevel << '@' << buddyInfo.m_bActivateAllSkill << '@';
			sStream << buddyInfo.m_nAllEquipSlotReinforceLevel << '@';

			std::map<int32_t,PS_RuneItemInfo_BuddyCS>::const_iterator itor = buddyInfo.m_nBuddyRuneInfo.begin();
			for (;itor!=buddyInfo.m_nBuddyRuneInfo.end();++itor)
			{
				const PS_RuneItemInfo_BuddyCS& info = itor->second;
				sStream << itor->first << ';' << info.m_nRuneStarLevel << ';' << info.m_nRuneItemID << ';';
			}
			sStream << "@";

			std::map<int32_t,PS_EquipItemInfo_BuddyCS>::const_iterator itr2 = buddyInfo.m_nEquipSlotInfo.begin();
			for (;itr2!=buddyInfo.m_nEquipSlotInfo.end();++itr2)
			{
				sStream << itr2->first << ';' << itr2->second.m_nEquipItemID << ';';
			}
			sStream << '|';
		}

		sStream << '#';

		const CommanderBaseInfo& defCmdIf = record.m_DefenceInfo.m_CommanderBaseInfo;
		sStream << defCmdIf.m_nControllerLevel << '#' << STDSTR2TPSTR(defCmdIf.m_nCharName ) << '#'<< defCmdIf.m_nCommanderTitleID << '#';
		sStream << defCmdIf.m_nCommanderTitleFrameID << '#' << defCmdIf.m_nBornType << '#' << defCmdIf.m_uCharId << '#';

		const std::vector<PS_Buddy>& selfBuddys = record.m_DefenceInfo.m_nVecBuddyInfo;
		std::vector<PS_Buddy>::const_iterator buddyItr2 = selfBuddys.begin();
		for( ; buddyItr2!=selfBuddys.end(); ++buddyItr2)
		{
			const PS_Buddy& info = *buddyItr2;
			const PS_Buddy_Info_BuddyCS& buddyInfo = info.m_nBuddyData;
			
			sStream <<  buddyInfo.m_nBuddyID << '@' << info.m_nBuddyIndex << '@';
			sStream  << info.m_nDamageData << '@' << info.m_nHealData << '@' << info.m_nHurtData << '@';
			sStream << buddyInfo.m_nBuddyLevel << '@' << buddyInfo.m_nBuddyStarLevel << '@' << buddyInfo.m_bActivateAllSkill << '@';
			sStream << buddyInfo.m_nAllEquipSlotReinforceLevel << '@';

			std::map<int32_t,PS_RuneItemInfo_BuddyCS>::const_iterator itor = buddyInfo.m_nBuddyRuneInfo.begin();
			for (;itor!=buddyInfo.m_nBuddyRuneInfo.end();++itor)
			{
				const PS_RuneItemInfo_BuddyCS& info = itor->second;
				sStream << itor->first << ';' << info.m_nRuneStarLevel << ';' << info.m_nRuneItemID << ';';
			}
			sStream << "@";

			std::map<int32_t,PS_EquipItemInfo_BuddyCS>::const_iterator itr2 = buddyInfo.m_nEquipSlotInfo.begin();
			for (;itr2!=buddyInfo.m_nEquipSlotInfo.end();++itr2)
			{
				sStream << itr2->first << ';' << itr2->second.m_nEquipItemID << ';';
			}
			sStream << '|';
		}

		sStream << '&';
	}
	
	LC_ClientGameEvent kCGameEvent;
	kCGameEvent.SetClientEventType(CGET_GUILDBATTLE_RESP_FIGHTRECORD);
	LC_ClientGameEventUserData_String kUserData(sStream.str());
	kCGameEvent.SetUserData(&kUserData);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kCGameEvent);
}

void HL_FromGameServer::_handlerNoticeBeDefighted(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_NoticeBeDefighted msg;
	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_NoticeBeDefighted解析错误");
		return;
	}
	
	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_NOTICE_BEFIGHTED);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_nBeDefighted);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handleRltGetRegularBossKillerInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_GetRegularBossKillerInfo msg;
	if (!msg.Decode(pMsg,wSize))
	{
		return;
	}

	StringStreamType sStream;
	sStream << msg.m_nCharTypeID << "#" << msg.m_nMapID << "#" << msg.m_nPlayerName << "#" << msg.m_nGuildName << "#";

	LC_ClientGameEvent kCGameEvent;
	kCGameEvent.SetClientEventType(CGET_REGULARBOSS_KILLER_INFO);
	LC_ClientGameEventUserData_String kUserData(sStream.str());
	kCGameEvent.SetUserData(&kUserData);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kCGameEvent);
}

void HL_FromGameServer::_handlerSyncResourceRetrieveRecord(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncResourceRetrieverRecord msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_SyncResourceRetrieverRecord解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(!pkGamePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncResourceRetrieverRecord逻辑错误, 没有GamePlayer");
		return;
	}

	pkGamePlayer->SyncResourceRetrieveRecord(msg);

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_SYNC_RESOURCERETRIEVE_RECORD);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerRltGetResourceRetrieveReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_GetResourceRetrieverReward msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_RLT_GetResourceRetrieverReward解析错误");
		return;
	}

	if(msg.m_nResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_nResult);
	}
}

void HL_FromGameServer::_handlerRefreshBuddyTransformID(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RefreshBuddyTransformID msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_RefreshBuddyTransformID解析错误");
		return;
	}
	
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_RefreshBuddyTransformID解析错误 GamePlayer == NULL");
		return;
	}

	if(msg.m_lPlayerID == pkGamePlayer->GetID())
	{
		return;
	}

	GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI, "[LHX] _handlerRefreshBuddyTransformID: iTransformID=%d", msg.m_nBuddyCurrentTransformID);

	LC_ClientNetPlayer* pkPlayer = (LC_ClientNetPlayer*)CLIENT_GET_NETPLAYER_MANAGER()->FindPlayer(msg.m_lPlayerID);
	if(pkPlayer != NULL)
	{
		int iTransformID = msg.m_nBuddyCurrentTransformID;
		if(iTransformID != pkPlayer->GetTransformID())
		{
			pkPlayer->SetTransformID(iTransformID);
			pkPlayer->ChangeVisibleData(iTransformID);
		}

	}
}

void HL_FromGameServer::_handlerSyncLotteryRecord(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_SyncLotteryRecord msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncLotteryRecord解析错误");
		return;
	}

	LC_ClientActivityManager::GetSingletonPtr()->SyncLotteryRecord(msg);
}

void HL_FromGameServer::_handlerSyncLotteryRet(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_SC_SyncLottery_Ret msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SC_SyncLottery_Ret解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SC_SyncLottery_Ret解析错误 GamePlayer == NULL");
		return;
	}

	pkGamePlayer->SetCommanderLotteryCount(msg.m_TotalNum);
	LC_ClientActivityManager::GetSingletonPtr()->SyncLotteryData(msg);
}

void HL_FromGameServer::_handlerSyncChargeProfit(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_SC_SynChargeProfit_Ret msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SC_SynChargeProfit_Ret解析错误");
		return;
	}

	LC_ClientActivityManager::GetSingletonPtr()->SyncChargeProfitData(msg);
}
