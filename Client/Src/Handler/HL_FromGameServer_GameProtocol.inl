#include "SC_ClientScriptInterface.h"

#include "msg.pb.h"
#include "MG_Game2Group.h"
#include "MG_GameProtocol.h"
#include "MG_PetProtocol.h"
#include "ajson_mem.hpp"

#include "NW_ClientSessionManager.h"
#include "LC_AchievementManager.h"
#include "LC_MiniMap.h"
#include "LC_MapManager.h"

#include "CF_GameStory.h"
#include "CF_LevelUp.h"
#include "CF_ProductionFormula.h"
#include "CF_CrossRealmMap.h"

#include "CL_RichTextParse.h"
#include "LC_ClientNewBeeManager.h"
#include "LC_SecretTreasureAsset.h"
#include "LC_ClientToyPetManager.h"
#include "LC_ClientArenaPlayBackManager.h"
#include "GlobalSettings.h"
#include "CF_FlowerConfig.h"
#include "CF_RegularBoss.h"
#include "CF_GameRestartNotice.h"
#include "CF_ACTTreasureHunt.h"
#include "CF_ItemGen.h"

using namespace boost::ajson;

//------------------------------------------------------------------------
void HL_FromGameServer::_handlerSystemAnnounce(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_SystemAnnounce announceMsg;
	if(!announceMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SystemAnnounce无法解析");
		return;
	}

	SendLogicInfoMessage(RE_SYSTEM_ANNOUNCE, STDSTR2TPSTR(announceMsg.m_szSystemAnnounce));
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerSystemNotifyMsg(NW_Game2ClientSession* pkSession, char* pMsg, uint16_t wSize )
{
	MG_NotifyMsg rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
	{
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if (!pkGamePlayer)
	{
		return;
	}

	//BUG #22101 【功能】【活动相关】低等级玩家不宜过早的收到活动开启相关的公告信息
	CF_TextConfig::DataEntry *pkTextConfig = SafeGetCSVFileDataEntryPtr<CF_TextConfig>(rltMsg.m_Type);
	//if(pkTextConfig && pkTextConfig->_lChannelLevelLimit > pkGamePlayer->GetLevel())
	if(pkTextConfig 
		&& (pkTextConfig->_lChannelLevelLimit > pkGamePlayer->GetCommanderLevel()) 
		|| (pkTextConfig->_iChannelOpenLevelLimit > 0 && !pkGamePlayer->GetOpenAbilityAsset().IsAbilityOpened(pkTextConfig->_iChannelOpenLevelLimit)))
	{
		return;
	}
	StringType kServerInfo("");
	if (rltMsg.m_ServerID > 0 && rltMsg.m_ServerID != pkGamePlayer->GetServerID())
	{
		kServerInfo = "[s" + Utility::Int2String(rltMsg.m_ServerID) + "]";
	}
	else if (rltMsg.m_ServerID >0 && rltMsg.m_ServerID == pkGamePlayer->GetServerID())
	{
		kServerInfo = "[本服]";
	}

	StringType kOutput = STDSTR2TPSTR(rltMsg.m_szSystemAnnounce);
	/*GUISys::getRichTextParse->FilterForbiddenText(kOutput, kOutput, TT_TextFilter::TFT_CHAR_NAME);*/
	SendNotifyMessage(rltMsg.m_Type, kOutput.insert(0,kServerInfo), rltMsg.m_MsgLevel);
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerNotifyYiZhongQinXiNotice(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_NotifyYiZhongQinXiNotice msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_NotifyYiZhongQinXiNotice解析错误");
		return;
	}

	SendLogicInfoMessage(RE_SYSTEM_ANNOUNCE, STDSTR2TPSTR(msg.m_nNotice));
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltUpdateSkill( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//如果还未进入游戏，则退出
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;

	MG_RLT_UpdateSkill msg;
	if (msg.Decode(pMsg, wSize))
	{
		LC_ClientGamePlayer* pkPlayer = pkSession->GetPlayer();
		if(NULL == pkPlayer)
		{
			return;
		}

		bool bSuccess = msg.m_lUpdateSkillResult == RE_SKILL_UPDATE_SUCCESS;
		if(!bSuccess)
		{
			pkPlayer->SetIsAutoUpgradeSkill(false);
		}
		bool bUpgrading = pkPlayer->GetIsAutoUpgradeSkill();
		if (bUpgrading && (pkPlayer->GetCurrentUpgradeSkillID()/SKILL_LEVEL_MASK) == (msg.m_lSkillIDAfterUpdate/SKILL_LEVEL_MASK))
		{
			//pkPlayer->SetCurrentUpgradeSkillID(msg.m_lSkillIDAfterUpdate);
			//pkPlayer->RequestSkillUpGrade(msg.m_lSkillIDAfterUpdate);
			pkPlayer->SetIsDoAutoUpgradeSkill(true);
		}
		else
			pkPlayer->SetIsDoAutoUpgradeSkill(false);

		long lSkillID = msg.m_lSkillIDAfterUpdate;
		CF_SkillSpecialAttrib::DataEntry* pkSkillDetail = SafeGetCSVFileDataEntryPtr<CF_SkillSpecialAttrib>(lSkillID);
		if (NULL == pkSkillDetail)
		{
			return;
		}
		CF_SkillExecuteList::DataEntry* pkSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(lSkillID);
		if (NULL == pkSkillData)
		{
			return;
		}

		if (bSuccess)
		{			
			//注意：从描述中获取技能名字
			StringType kSkillName = pkSkillData->_kName;
			long lCurLevel = pkSkillDetail->_lCurLevel;
			if(pkSkillData->_bPrompt)
			{
				g_soundMgr.Play2DEx(VARIABLE2STRING(SE_Sys_SkillUpgrade));
				if (pkSkillData->_lCategory2 == SKILL_TYPE_JUEXUE)
				{
					int level1 = (lCurLevel-1)/10+1;
					int level2 = lCurLevel + 10 - level1 * 10;
					SendLogicInfoMessage(RE_SECRET_SKILL_UPDATE_SUCCESS, kSkillName.c_str(), level1,level2);
				} 
				/*else if (pkSkillData->_lCategory2 == SKILL_TYPE_SKILLBOOK)
				{
					SendLogicInfoMessage(RE_SKILL_UPDATE_SUCCESS, kSkillName.c_str(), lCurLevel,"级");
				}*/
				else
				{
					bool bBreak = false;
					CF_SkillLevelUp::DataEntry *pkLevelUp = SafeGetCSVFileDataEntryPtr<CF_SkillLevelUp>(lSkillID);
					if(pkLevelUp != NULL )
					{
						CF_SkillLevelUp::DataEntry *pkLastLevelUp = SafeGetCSVFileDataEntryPtr<CF_SkillLevelUp>(pkLevelUp->_lLastLevelSkillTypeID);
						bBreak = pkLastLevelUp != NULL && pkLastLevelUp->_bBreak;
					}
					if(bBreak)
						SendLogicInfoMessage(RE_SKILL_UPDATE_SUCCESS, kSkillName.c_str(), GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_079), lCurLevel);
					else
						SendLogicInfoMessage(RE_SKILL_UPDATE_SUCCESS, kSkillName.c_str(), GET_MSG(LC_EQUIPITEMTOOLTIP_CPP_083), lCurLevel);
				}
			}

			LC_SkillAsset *pkSkillAsset = pkPlayer->GetBuddySkillAsset(msg.m_iBuddyID);
			if(pkSkillAsset)
			{
				LC_SkillAssetEntry *pkEntry = pkSkillAsset->FindLearnedSkill(lSkillID);
				if(pkEntry)
				{
					int32_t iSlotIdx = pkEntry->GetSlotIndex();
					pkSkillAsset->AddSkillToAsset((skill_id_type&)lSkillID, iSlotIdx);
				}
			}

			//增加升级技能事件，用于挂机技能更新
			if(pkSkillData->_bPrompt && (pkSkillData->_lCategory2 == SKILL_TYPE_CONTROL || pkSkillData->_lCategory2 == SKILL_TYPE_SECT))
			{
				CF_Buddy::DataEntry *pkBuddyData = SafeGetCSVFileDataEntryPtr<CF_Buddy>(msg.m_iBuddyID);
				if(pkBuddyData != NULL)
					g_soundMgr.Play2DEx(pkBuddyData->_sSkillLevelUpSound);

				LC_GameEvent kGameEvent;
				kGameEvent.SetEventType(GLET_SKILL_LEVEL_UP);
				kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, pkSkillData->_iID);
				kGameEvent.SetEventParam(EVENT_BASE_PARAM_3, RE_SKILL_UPDATE_SUCCESS);

				UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
				pkUIManager->HandlerGameEvent(kGameEvent);
			}
		}
		else
		{
			switch (msg.m_lUpdateSkillResult)
			{
			case RE_SKILL_UPDATE_NEED_LEVEL:
			case RE_SKILL_UPDATE_NEED_SP:
			case RE_SKILL_UPDATE_NEED_CASH:
			case RE_SKILL_UPDATE_NEED_FINISH_TASK:
			case RE_SKILL_UPDATE_TO_MAX_LEVEL:
			case RE_SKILL_UPDATE_FAIL:
				if(pkSkillData->_bPrompt)
					SendLogicInfoMessage(msg.m_lUpdateSkillResult);
				break;
			case RE_SKILL_UPDATE_NEED_ITEM:
				if((pkSkillData->_lCategory2>=SKILL_TYPE_MENTAL_BEIGN&&pkSkillData->_lCategory2<SKILL_TYPE_MENTAL_END))
					SendLogicInfoMessage(RE_SKILL_MENTAL_UPDATE_NEED_ITEM, pkSkillData->_kName);
				else
				{
					StringType itemName = pkSkillData->_kName;
					CF_SkillLevelUp* pkLevelUp = CF_SkillLevelUp::GetSingletonPtr();
					CF_SkillLevelUp::DataEntry* pkLevelUpData = pkLevelUp->GetEntryPtr(lSkillID);
					if (pkLevelUpData != NULL)
					{
						Utility::UT_SIMDataList::const_iterator it = pkLevelUpData->_kNeedItems.begin();
						for (; it!=pkLevelUpData->_kNeedItems.end(); ++it)
						{
							LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
							LC_ItemBase* pkItemData = pkItemFactory->RequestItem(it->ID());
							if(pkItemData != NULL)
							{
								itemName = pkItemData->GetItemDescriptionEntry()->GetItemName();
								break;
							}
						}
					}
					SendLogicInfoMessage(RE_SKILL_UPDATE_NEED_ITEM, itemName);
				}
				break;
			/*case RE_SKILL_UPDATE_NEED_SKILL_BOOK_LEVEL:
				{
					CF_SkillLevelUp* pkLevelUp = CF_SkillLevelUp::GetSingletonPtr();
					CF_SkillLevelUp::DataEntry* pkLevelUpData = pkLevelUp->GetEntryPtr(lSkillID);
					if (pkLevelUpData != NULL && !pkLevelUpData->_kNeedSkillBookLevels.empty())
					{
						TypeNumList::iterator it = pkLevelUpData->_kNeedSkillBookLevels.begin();
						for (; it != pkLevelUpData->_kNeedSkillBookLevels.end(); ++it)
						{
							int level = int((it->getNum()-1)/6) +1;
							SendLogicInfoMessage(msg.m_lUpdateSkillResult,level);
							break;
						}
					}		
				}		
				break;*/
			default:
				break;
			}

			//失败也下发消息
			LC_GameEvent kGameEvent;
			kGameEvent.SetEventType(GLET_SKILL_LEVEL_UP);
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, pkSkillData->_iID);
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_3, msg.m_lUpdateSkillResult);

			UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
			pkUIManager->HandlerGameEvent(kGameEvent);
		}
	}
}

void HL_FromGameServer::_handlerRltLearnControllerSkill(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RLT_LearnControllerSkill msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_RLT_LearnControllerSkill解析错误");
		return;
	}

	if(msg.m_nResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_nResult);
		return;
	}

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_SKILL_LEVEL_UP);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, msg.m_nSkillID);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

//------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltRespawn(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);

	//如果还未进入游戏，则退出
	if (!pkClientSessionGroup->GetInGameFlag())
		return;

	MG_RLT_Respawn msg;
	if(msg.Decode(pMsg,wSize))
	{
		UT_Vec3Int kLocation;
		object_id_type lPlayerID = msg.m_lPlayerID;
		int  iRespawnType = msg.m_iRespawnType;
		attr_value_type  iHP	   = msg.m_iHP;
		int  iMP	   = msg.m_iMP;

		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

		if(pkGamePlayer)  //本地玩家安全复活
		{
			LC_PlayerBase* pkPlayer = pkGamePlayer->GetBuddyPlayerBasePtr(lPlayerID);
			if (NULL == pkPlayer)
				return;

			pkPlayer->Respawn(iHP,iMP);
			if (pkPlayer != pkGamePlayer)
			{
				((LC_ClientNetPlayer*)pkPlayer)->ShowName();
				((LC_ClientNetPlayer*)pkPlayer)->ShowBlood();
			}

			IM_Manager* pkIMManager = IM_Manager::GetSingletonPtr();
			if (pkIMManager != NULL)
			{
				GD_ActorModel* pkVisibleData = pkIMManager->GetActor(lPlayerID);
				if (pkVisibleData == NULL)
					pkPlayer->CreateVisibleData();
			}
			SendLogicInfoMessage(RE_YOU_RESPAWN);

			if(msg.m_iReduceExp > 0)
				SendLogicInfoMessage(RE_EXP_REDUCE,msg.m_iReduceExp);

			if (msg.m_iReduceBindCash > 0)
			{
				MoneyInfo kMoneyInfo = LC_Money::MoneyNumber2MoneyInfo(msg.m_iReduceBindCash);
				StringStreamType kStrStream;
				kStrStream.clear();
				MoneyType ulMoney = (MoneyType)kMoneyInfo.iGold*10000 + (MoneyType)kMoneyInfo.sSilver*100 + (MoneyType)kMoneyInfo.sCopper;
				if (ulMoney > 0)
					kStrStream << ulMoney;
				//SendLogicInfoMessage(RE_BIND_CASH_REDUCE, kStrStream.str());
			}

			if (pkGamePlayer->IsBuddy(lPlayerID))
			{
				LC_GameEvent kGameEvent;
				kGameEvent.SetEventType(GLET_PLAYER_RESPAWN);
				kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_iResult);
				kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, pkPlayer->GetCharType());

				UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
				pkUIManager->HandlerGameEvent(kGameEvent);

				g_soundMgr.Play2DEx(VARIABLE2STRING(SE_Sys_Player_Rev));

				pkGamePlayer->SetDirtyFlag(DIRTY_FLAG_ACTIVEBUDDYLIST, true);
				pkGamePlayer->AddDirtyFlagRevisionID(DIRTY_FLAG_ACTIVEBUDDYLIST);

				int idx = pkGamePlayer->GetBuddyIndex(lPlayerID);
				pkGamePlayer->ResetRespawnCoolDown(idx);
			}
		}
	}
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltNetPlayerEquipInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	//NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	//NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	//int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	////如果还未进入游戏，则退出
	//if (eGameStage != ST_GAME_STAGE_GAME)
	//	return;


	MG_RLT_NetPlayerEquipInfo msg;
	if(!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"解析网络玩家装备数据错误");
		return;
	}

	unique_id_type lNetPlayerID = msg.m_lNetCharID;

    GfxWriteLog(LOG_SYSTEM_FILELINE, LOG_SWITCH_SYSTEM, 
        "Logic net-player [%s] handlerRltNetPlayerEquipInfo", Int642String(lNetPlayerID).c_str());

	LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
	if(!pkNetPlayerManager)
		return;

	LC_ClientNetPlayer*	pkNetPlayer = (LC_ClientNetPlayer*)(pkNetPlayerManager->FindPlayer(lNetPlayerID));
	if(!pkNetPlayer)
		return;

	for(int i = 0; i < MAX_EQUIP_SLOT_SIZE; i++)
	{
		long lNewItemTypeID = msg.m_alEquipContent[i];
		long lOldItemTypeID = pkNetPlayer->GetCurrentEquipItem(i);
		if (lNewItemTypeID != lOldItemTypeID)
		{
			StringStreamType sStream;
			sStream << "Logic net-player [" << lNetPlayerID << "] handlerRltNetPlayerEquipInfo " << lOldItemTypeID << " -> " << lNewItemTypeID;
            GfxWriteLog(LOG_SYSTEM_FILELINE, LOG_SWITCH_SYSTEM, sStream.str().c_str());

            pkNetPlayer->ChangeAvatar(lOldItemTypeID,lNewItemTypeID);

			pkNetPlayer->SetCurrentEquipItem(i, lNewItemTypeID);
			if (ET_WEAPON == (EquipType)i)
			{
				pkNetPlayer->SetEquipWeapon(lNewItemTypeID);
			}
		}
	}

    pkNetPlayer->ShowVisibleDataAfterAllAvatarChanged();
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltNetPlayerAttribute(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	//NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	//NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	//int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	////如果还未进入游戏，则退出
	//if (eGameStage != ST_GAME_STAGE_GAME)
	//	return;

	MG_RLT_NetPlayerAttribute msg;
	if(!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"解析网络玩家属性数据错误");
		return;
	}

	unique_id_type lNetPlayerID = msg.m_value.charid();
	/*
	int	 iMaxHP			= msg.m_kPlayerAttribute.m_iMaxHP;
	int	 iMaxMP			= msg.m_kPlayerAttribute.m_iMaxMP;
	int  iMAX_PHA		= msg.m_kPlayerAttribute.m_iMAX_ATK;
	int  iMAX_STM		= msg.m_kPlayerAttribute.m_iMAX_STM;
	int  iMAX_AMM		= msg.m_kPlayerAttribute.m_iMAX_AMM;
	int  iDEF			= msg.m_kPlayerAttribute.m_iDEF;
	int  iMF_RATE			= msg.m_kPlayerAttribute.m_iMF_RATE;
	int  iHP_Revocer		= msg.m_kPlayerAttribute.m_iHP_Recover;
	int  iMP_Revocer		= msg.m_kPlayerAttribute.m_iMP_Recover;
	int  iDOG_RATE		= msg.m_kPlayerAttribute.m_iDOG_RATE;
	int  iCIR_RATE		= msg.m_kPlayerAttribute.m_iCIR_RATE;
	int  iCIR_DAM		= msg.m_kPlayerAttribute.m_iCIR_DAM;
	int  iMAX_VIGOR		= msg.m_kPlayerAttribute.m_iMAX_VIGOR;
	int  iMOVE_SPEED	= msg.m_kPlayerAttribute.m_iMOVE_SPEED;
	int	 iCASH_RATE		= msg.m_kPlayerAttribute.m_iCASH_RATE;
	*/
	LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
	if(!pkNetPlayerManager)
		return;

	LC_ClientNetPlayer*	pkNetPlayer = (LC_ClientNetPlayer*)(pkNetPlayerManager->FindPlayer(lNetPlayerID));
	if(!pkNetPlayer)
		return;

	LC_ActorAttributeMap* pkAttributeMap = pkNetPlayer->GetAttributeMap();

	pkAttributeMap->Clear();
	for( int i = 0; i < msg.m_value.attrs_size() ; ++i )
	{
		int attrType = msg.m_value.attrs(i).attr();
		pkAttributeMap->SetAttributeBase(attrType,msg.m_value.attrs(i).value());
		pkAttributeMap->AddAttributePercentFunc(attrType,msg.m_value.attrs(i).percent());
	}
	pkAttributeMap->RecomputeAll();

}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltTransfer(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	commitPatchProgress(EVENT_CLIENT_GC_RLT_TRANSFER, 0);

	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//如果还未进入游戏，则退出
	if(!pkClientSessionGroup->GetInGameFlag())
	{
		GfxWriteLog(LOG_TAG_ERROR, LOG_SWITCH_ERROR,"[网络][收]MG_RLT_Transfer 状态错误，还未进入游戏");
		return;
	}

	MG_RLT_Transfer RltMsg;
	if(!RltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_ERROR, LOG_SWITCH_ERROR,"[网络][收]MG_RLT_Transfer 解析传送数据错误");
		return;
	}
	
	UT_Vec3Int kLocation;
	ResultType iResult  = RltMsg.m_iResult;
	object_id_type lPlayerID		= RltMsg.m_lPlayerID;
	long lMapLogicID    = RltMsg.m_lMapLogicID;
	long lMapResID		= RltMsg.m_lMapResID;
	long lMapLogicType	= RltMsg.m_lMapLogicType;
	int iLineNum		= RltMsg.m_iLineNum;
	kLocation.x			= RltMsg.m_PosX;
	kLocation.y			= RltMsg.m_PosY;
	kLocation.z			= RltMsg.m_PosZ;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
// 	if (!pkGamePlayer || pkGamePlayer->GetID() != lPlayerID)
// 	{
// 		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "屏蔽列表消息逻辑错误");
// 		return;
// 	}

	if(!pkGamePlayer)
	{
		GfxWriteLog(LOG_TAG_ERROR, LOG_SWITCH_ERROR,"[网络][收]没有GamePlayer角色对象");
		return;
	}

	GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM,"[网络][收]MG_RLT_Transfer Result=%d PlayerID=%d GamePlayerID=%d ActiveBuddyIndex=%d MapLogicID=%d MapResID=%d MapLogicType=%d iLineNum=%d PosX=%d PosY=%d PosZ=%d",
		iResult, lPlayerID, pkGamePlayer->GetID(), pkGamePlayer->GetCurrentActiveBuddyIndex(), lMapLogicID, lMapResID, lMapLogicType, iLineNum, kLocation.x, kLocation.y, kLocation.z);

	bool IsChangeMap = false;
	bool isPhaseDungeon = false;
	if(pkGamePlayer->GetID() != lPlayerID)	//别人在transfer
	{
		if(IS_PLAYER_CLASS_ID(lPlayerID))
		{
			LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();;
			LC_PlayerBase* pkPlayer = pkNetPlayerManager->FindPlayer(lPlayerID);
			if (NULL == pkPlayer)
			{
				return;
			}
			if(pkGamePlayer->GetMapLogicID()==lMapLogicID)
			{
				pkPlayer->SetCurrentLocation(kLocation);
				pkPlayer->SetTargetLocation(kLocation);
				pkPlayer->SetVisibleLocation(kLocation);
			}
		}
		else if(IS_NPC_ID(lPlayerID))
		{
			LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();;
			LC_NPCBase* pkNPC			      = pkNPCManager->FindNPC(lPlayerID);			
			if(pkNPC && pkNPC->GetMapLogicID()==lMapLogicID)
			{
				pkNPC->SetCurrentLocation(kLocation);
				pkNPC->SetTargetLocation(kLocation);
				pkNPC->SetVisibleLocation(kLocation);
				pkNPC->SetMoveSpeed(0);
			}
		}		
	}
	else
	{
		bool bReHooking = false;
		bool bLoading = false;
		bool bSwitchLine = false;
		if (iResult == RE_SUCCESS)
		{
			if (pkGamePlayer->GetCurrentLineNum() != iLineNum)
			{
				bSwitchLine = true;
				NW_ClientSessionManager* pkSessionManager	= NW_ClientSessionManager::GetSingletonPtr();
				NW_ClientSessionGroup* pkSessionGroup		= pkSessionManager->GetGameSessionGroup();
				int nodeID = pkSessionGroup->GetCurGameServerNode();
				pkGamePlayer->SendRequestInfoByType(MGPT_FALLIANCE_BOSS_INFO_REQ,nodeID,0,0,0);
			}
			pkGamePlayer->SetCurrentLineNum(iLineNum);
			pkGamePlayer->SetIsSwitich(false);
			GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"设置当前分线，curLineNum=%d",iLineNum);
			//同地图近距离切换不loading,如果是跨服传送有可能出现同地图，比如1服8001，传送到2服8001，理论上也需要changemap loading
			CF_WorldMapList::DataEntry* leaveMap = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(pkGamePlayer->GetMapResID());
			CF_WorldMapList::DataEntry* enterMap = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(lMapResID);

			if (leaveMap && enterMap && (IS_RAID_MAP(leaveMap->_iMapLogicType)) && (IS_RAID_MAP(enterMap->_iMapLogicType)) && pkGamePlayer->IsHooking())
			{
				pkGamePlayer->SetHooking(false);
				//出相位副本不开启自动挂机
				if(enterMap->_iShowRaidStage > 0)
					bReHooking = true;
			}

			int dist = UT_MathBase::LineLengthXYInt(pkGamePlayer->GetCurrentLocation(), kLocation);
			if(leaveMap && enterMap && leaveMap->_iResourceGroup == enterMap->_iResourceGroup && !pkGamePlayer->GetPostEnterGameFlag() && (dist<TRANSFER_LOADING_DISTANCE || IS_RAID_MAP(lMapLogicType)))
			{
				if (dist > GetGlobalSetting.iReviseDistance|| (IS_RAID_MAP(lMapLogicType) && enterMap->_iShowRaidStage <= 0))
				{
					pkGamePlayer->SetCurrentLocation(kLocation);
					pkGamePlayer->SetVisibleLocation(kLocation);
				}
				pkGamePlayer->SetTargetLocation(kLocation);

				//< 通知服务器清除 m_clientLoading 标志位
				//pkGamePlayer->NotifyServerAlreadyInGame();
				if (pkGamePlayer->GetMapLogicID() != lMapLogicID||bSwitchLine)
				{
					//ToDo: 更改phase map的相关函数
					pkGamePlayer->RemoveAllBullets();
					pkGamePlayer->ClearAllSkillSound();
					pkGamePlayer->ChangePhaseMap(lMapLogicID,lMapResID,lMapLogicType);
					IsChangeMap = true;

					//出相位副本不开启自动挂机
					if(enterMap->_iShowRaidStage > 0)
						isPhaseDungeon = true;
				}
				else
				{
					IsChangeMap = false;
				}

				if (pkGamePlayer->GetIsSwitchLine())
					pkGamePlayer->ReqMGPTInGame();
			}
			else
			//< obsoleted: 服务器调用了ServerPlayer::ChangeMap，设置了切换地图标志位，如果客户端不切换地图，会导致不发送loaded消息，导致服务器标志位无法清除 m_clientLoading
			{
				//打点: 新老用户进入游戏成功（服务端返回角色进入游戏成功，开始请求资源加载前）
				commitPatchProgress(EVENT_CLIENT_ENTER_GAME_OK, 0);
				
				IsChangeMap = pkGamePlayer->GetMapResID() != lMapResID || pkGamePlayer->GetMapLogicID() != lMapLogicID;

				GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM,"开始切换地图, ResID=%d, Location=(%d, %d, %d)", lMapResID, kLocation.x, kLocation.y, kLocation.z);
				pkGamePlayer->RemoveAllBullets();
				pkGamePlayer->ClearAllSkillSound();
				
				/*if (IsChangeMap || IS_NORMAL_MAP(lMapLogicType) || MT_WATCHTOWER==lMapLogicType)*/
				{
					pkGamePlayer->ChangeMap(lMapLogicID,lMapResID,lMapLogicType,kLocation);
					bLoading = true;
				}
			}

			pkGamePlayer->SetTranToTranTick(0);
			//pkGamePlayer->SetNotifyInGameFlag(0);
			//pkGamePlayer->NotifyServerAlreadyInGame();
			
			pkGamePlayer->SetHookingParam("search", enterMap->_iHookType, 0, true);
		}
		else
		{
			SendLogicInfoMessage(iResult);
			GfxWriteLog(LOG_TAG_ERROR, LOG_SWITCH_ERROR,"[网络][收]MG_RLT_Transfer Fail, %d", iResult);
		}

		if(pkGamePlayer->GetAlreadyTransfer())
		{
			pkGamePlayer->SetAlreadyTransfer(false);
			pkGamePlayer->ClearPathFinding();
		}
		
		//这个是复活挂机的逻辑，复活挂机一定是在本地图，如果跨地图的话，就不需要传送
		if (!IsChangeMap)
		{
			if(pkGamePlayer->HookRespawnFlag())
			{
				pkGamePlayer->SetHooking(false);
				pkGamePlayer->PendingPathToLocation(lMapResID, pkGamePlayer->GetHookGuideLocation(), lMapLogicID);
				pkGamePlayer->PendingPathAction(PPAT_ATTACK_NPC,0,false,0);

				if(!bLoading && !IS_RAID_MAP(lMapLogicType))
					pkGamePlayer->PathToPendingLocation();

				pkGamePlayer->SetHookRespawnPathFlag(true);
				pkGamePlayer->SetHookRespawnFlag(false);
			}
			else if(pkGamePlayer->GetHookRespawnDeadFlag())
			{
				pkGamePlayer->SetHooking(true);
				pkGamePlayer->SetHookRespawnDeadFlag(false);
			}
		}

		if (bReHooking)
		{
			pkGamePlayer->SetHooking(true);
		}

		if (!isPhaseDungeon && IS_RAID_MAP(pkGamePlayer->GetMapLogicType()))
		{
			pkGamePlayer->SetLockedTargetID(0);
			pkGamePlayer->SetSpecialHookCharType(0);
			pkGamePlayer->SetHooking(true);
		}

		//添加一个强制打断,因为有时候收不到服务器的打断,不能指望服务器
		if (IS_SKILL_TYPE(pkGamePlayer->GetCurrentStateType()))
		{
			float fCurrentTime = GET_PROCESS_TIME;
			pkGamePlayer->RequestInterrputSkill(fCurrentTime, pkGamePlayer->GetCurrentStateType(), SKI_FORCE);
			pkGamePlayer->InterruptSkill(fCurrentTime, pkGamePlayer->GetCurrentStateType(), SKI_FORCE);
		}

		/*if ( !IsChangeMap )
		{	
			pkGamePlayer->MotivateActionForFeiXie();
		}*/

		pkGamePlayer->SetAutoPicking(false);
		pkGamePlayer->SetIsSwitchLine(0);
	}
}
//-----------------------------------------------------------------------
void HL_FromGameServer::_handlerRltTransferToLocation(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//如果还未进入游戏，则退出
	if(!pkClientSessionGroup->GetInGameFlag())
		return;

	MG_Rlt_TransferToLocation rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"解析传送数据错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if (NULL == pkGamePlayer || pkGamePlayer != pkSession->GetPlayer())
		return;

	//int iMapLogicType = LC_Helper::GetMapLogicType(rltMsg.m_iMapResID);
	//UT_Vec3Int kLocation(rltMsg.m_iLocX, rltMsg.m_iLocY, rltMsg.m_iLocZ);
	//pkGamePlayer->ChangeMap(rltMsg.m_iMapLogicID, rltMsg.m_iMapResID, iMapLogicType, kLocation);
	//pkGamePlayer->SetHooking(false);
	GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM,"[网络][收]MG_Rlt_TransferToLocation Result=%d ServerID=%d LineNum=%d MapLogicID=%d MapResID=%d PosX=%d PosY=%d PosZ=%d",
		rltMsg.m_uiRslt, rltMsg.m_iServerID, rltMsg.m_iLineNum, rltMsg.m_iMapLogicID, rltMsg.m_iMapResID, rltMsg.m_iLocX, rltMsg.m_iLocY, rltMsg.m_iLocZ);

	if (rltMsg.m_uiRslt == RE_SUCCESS)
	{
		pkGamePlayer->OnMovePathEnd();
	}
	else
	{
		pkGamePlayer->OnReceiveRLTTransferToLocation(rltMsg);
		SendLogicInfoMessage(rltMsg.m_uiRslt);
	}

	pkGamePlayer->SetTranToTranTick(0);
}

//-----------------------------------------------------------------------
void HL_FromGameServer::_handlerRltEnterTranscription(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//如果还未进入游戏，则退出
	if(!pkClientSessionGroup->GetInGameFlag())
		return;


	MG_RLT_ENTER_Transcription RltMsg;
	if(!RltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"解析传送数据错误");
		return;
	}

	ResultType iResult	= RltMsg.m_iResult;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if (!pkGamePlayer)
	{
		return;
	}	

	if (iResult == RE_SUCCESS)
	{			
		//long lTranscriptionType =  RltMsg.m_lTranscriptionType;
	
		////副本为心法时保存特效设置值
		//if(lTranscriptionType == MT_CAREER_SELECTION)
		//{
		//	fShadow_Light_Offeet_Value = g_settings.SHADOW_LIGHT_OFFSET;
		//	g_settings.SHADOW_LIGHT_OFFSET = 7000;
		//}
	
		LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
		if(pkNPCManager)
		{
			if(RltMsg.m_iAutoFitLevel == 1)
			{
				pkNPCManager->SetAutoFitFlag(true);
				pkNPCManager->SetFitLevel(RltMsg.m_iLevelNUM);
			}
			else
			{
				pkNPCManager->SetAutoFitFlag(false);
				pkNPCManager->SetFitLevel(0);
			}
		}		
	}
	else
	{
		pkGamePlayer->OnReceiveRLTTranscriptionInfo(RltMsg);
	}
}

//-----------------------------------------------------------------------
void HL_FromGameServer::_handlerRltQuitTranscription(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//如果还未进入游戏，则退出
	if(!pkClientSessionGroup->GetInGameFlag())
		return;

	MG_RLT_QUIT_Transcription RltMsg;
	if(!RltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"解析传送数据错误");
		return;
	}

	ResultType iResult		= RltMsg.m_iResult;
	object_id_type lPlayerID			= RltMsg.m_lPlayerID;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if (!pkGamePlayer || pkGamePlayer->GetMainControllerID() != lPlayerID)
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "屏蔽列表消息逻辑错误");
		return;
	}	

	if (iResult == RE_SUCCESS && pkGamePlayer->IsPlayBackFlag())
	{
		// 战斗回放 退出观看
		//if (LC_ClientLogicManager::m_fBeginRecord < 0)
		{
			LC_ClientArenaPlayBackManager *pkArenaPlayBackManager = LC_ClientArenaPlayBackManager::GetSingletonPtr();
			pkArenaPlayBackManager->ReUse();
			//LC_ClientLogicManager::m_fBeginRecord = 0;
			pkGamePlayer->ShowAllBuddyVisibleData(true);

			LC_GameEvent kGameEvent;
			kGameEvent.SetEventType(GLET_ARENA_PLAYBACK_QUIT);
			UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
			pkUIManager->HandlerGameEvent(kGameEvent);
		}
	}
	else
		SendLogicInfoMessage(iResult);
}

//-----------------------------------------------------------------------
void HL_FromGameServer::_handlerRltReliveTranscription(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//如果还未进入游戏，则退出
	if(!pkClientSessionGroup->GetInGameFlag())
		return;

	MG_RLT_Relive_BeforeEnterMap RltMsg;
	if(!RltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"解析传送数据错误");
		return;
	}

	ResultType iResult		= RltMsg.m_iResult;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

	if (iResult == RE_SUCCESS)
	{
		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_NOTIFY_RECHARLLENGE_RAID);
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
	else
		SendLogicInfoMessage(iResult);
}
//-------------------------------------------------------------------------------------------------
void HL_FromGameServer::_handlerReceiveTranscriptionTipsInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	MG_NOTIFY_CLIENT_TRANSCIRPTION_TIPS RltMsg;
	if(!RltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"解析传送数据错误");
		return;
	}

	ResultType iResult		= RltMsg.m_iResult;
	object_id_type lPlayerID			= RltMsg.m_lPlayerID;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if (!pkGamePlayer || pkGamePlayer->GetID() != lPlayerID)
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "屏蔽列表消息逻辑错误");
		return;
	}

	SendLogicInfoMessage(iResult);
}

//------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltSetTransferPos(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RLT_SetTransferPos RltMsg;
	if(!RltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"解析设置传送数据错误");
		return;
	}

	object_id_type lPlayerID		 = RltMsg.m_lPlayerID;
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if (!pkGamePlayer || pkGamePlayer->GetID() != lPlayerID)
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "设置传送数据逻辑错误");
		return;
	}

	if (RltMsg.m_iResult == RE_TRANSFER_NPC_BIND_SUCCESS)
	{
		LC_LogicAreaInfo* kLogicAreaInfo = LC_LogicMapManager::GetSingletonPtr()->QueryAreaInfo(pkGamePlayer->GetCurrentLocation(),pkGamePlayer->GetMapResID());
		SendLogicInfoMessage(RltMsg.m_iResult,kLogicAreaInfo != NULL ? kLogicAreaInfo->kAreaName.c_str() : "");
		return;
	}

	SendLogicInfoMessage(RltMsg.m_iResult);
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncNetPlayerTitle(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//如果还未进入游戏，则退出
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;


	MG_SyncNetPlayerTitleName msg;
	if (msg.Decode(pMsg,wSize))
	{
		LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
		LC_ClientNetPlayer*	pkNetPlayer = (LC_ClientNetPlayer*)(pkNetPlayerManager->FindPlayer(msg.m_lPlayerID));
		if(!pkNetPlayer)
			return;

		pkNetPlayer->SetCharTitle(STDSTR2TPSTR(msg.m_strTitle));
		pkNetPlayer->SetCurrentTitleType(msg.m_lTitleType);
		pkNetPlayer->SendMessage( CreateMsg_UE_CHANGE_PLAYER_TITLE(pkNetPlayer->GetID()) );
	}

}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltSetTitle( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//如果还未进入游戏，则退出
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;

	MG_RLT_SetTitle msg;
	if (msg.Decode(pMsg,wSize))
	{
		object_id_type lPlayerID		 = msg.m_lPlayerID;
		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
		if (!pkGamePlayer || pkGamePlayer->GetSelfID() != lPlayerID)
		{
			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "设置称号逻辑错误");
			return;
		}

		SendLogicInfoMessage(msg.m_iResult);
	}
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltAddDIYTitle( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//如果还未进入游戏，则退出
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;


	MG_RLT_AddDIYTitle msg;
	if (msg.Decode(pMsg,wSize))
	{
		object_id_type lPlayerID		 = msg.m_lPlayerID;
		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
		if (!pkGamePlayer || pkGamePlayer->GetSelfID() != lPlayerID)
		{
			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "设置称号逻辑错误");
			return;
		}

		SendLogicInfoMessage(msg.m_iResult);
	}
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltRemoveDIYTitle( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//如果还未进入游戏，则退出
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;


	MG_RLT_RemoveDIYTitle msg;
	if (msg.Decode(pMsg,wSize))
	{
		object_id_type lPlayerID		 = msg.m_lPlayerID;
		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
		if (!pkGamePlayer || pkGamePlayer->GetSelfID() != lPlayerID)
		{
			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "设置称号逻辑错误");
			return;
		}

		SendLogicInfoMessage(msg.m_iResult);
	}
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltModifyDIYTitle( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//如果还未进入游戏，则退出
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;


	MG_RLT_ModifyDIYTitle msg;
	if (msg.Decode(pMsg,wSize))
	{
		object_id_type lPlayerID		 = msg.m_lPlayerID;
		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
		if (!pkGamePlayer || pkGamePlayer->GetSelfID() != lPlayerID)
		{
			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "设置称号逻辑错误");
			return;
		}

		SendLogicInfoMessage(msg.m_iResult);
	}
}
//------------------------------------------------------------------------
//void HL_FromGameServer::_handlerRltItemProduction(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
//{
//	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
//	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
//	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();
//
//	//如果还未进入游戏，则退出
//	if (eGameStage != ST_GAME_STAGE_GAME)
//		return;
//
//	MG_RLT_ItemProduction msg;
//	if (msg.Decode(pMsg,wSize))
//	{
//		long lPlayerID = msg.m_lPlayerID;
//		long lFormulaID = msg.m_lFormulaID;
//		ResultType iResult = msg.m_iResult;
//
//		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
//
//		if(!pkGamePlayer || pkGamePlayer->GetID() != lPlayerID)
//		{
//			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"生产返回消息逻辑错误");
//			return;
//		}
//
//		CF_ProductionFormula* pkCSVFormulaList = CF_ProductionFormula::GetSingletonPtr();
//		CF_ProductionFormula::DataEntry* pkProductionData = pkCSVFormulaList->GetEntryPtr(lFormulaID);
//
//		if (!pkProductionData)
//			return;
//
//		if (iResult == RE_PRODUCTION_SUCCESS)
//		{
//			CF_ItemList* pkCSVItemList = CF_ItemList::GetSingletonPtr();
//			CF_ItemList::DataEntry* pkItemData = pkCSVItemList->GetEntryPtr(pkProductionData->_lMadeItemID);
//
//			if (!pkItemData)
//				return;
//
//			StringType strItemName = pkItemData->_kName;
//			SendLogicInfoMessage(msg.m_iResult, strItemName.c_str());
//
//			LC_GameEvent kGameEvent;
//			kGameEvent.SetEventType(GLET_PRODUCTION_SUCCESS);
//
//			UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
//			pkUIManager->HandlerGameEvent(kGameEvent);
//		}
//		else if (iResult == RE_PRODUCTION_MATERIAL_NOT_ENOUGH)
//		{
//			long lMaterial1ID = pkProductionData->_lMaterial1ID;
//			int	 nPlayerMaterial1Num = 0;
//			if (IS_ITEM_ID(lMaterial1ID))
//			{
//				nPlayerMaterial1Num = pkGamePlayer->GetPackAsset()->GetItemCountInPack(PACK_TYPE_MATERIAL, lMaterial1ID);
//				if (nPlayerMaterial1Num < pkProductionData->_lMaterial1Num)
//				{
//					CF_ItemList* pkCSVItemList = CF_ItemList::GetSingletonPtr();
//					CF_ItemList::DataEntry* pkItemaData = pkCSVItemList->GetEntryPtr(lMaterial1ID);
//
//					StringType strMaterialName = pkItemaData->_kName;
//					SendLogicInfoMessage(RE_PRODUCTION_MATERIAL_NOT_ENOUGH, strMaterialName.c_str());
//					return;
//				}
//			}
//
//			long lMaterial2ID = pkProductionData->_lMaterial2ID;
//			int	 nPlayerMaterial2Num = 0;
//			if (IS_ITEM_ID(lMaterial2ID))
//			{
//				nPlayerMaterial2Num = pkGamePlayer->GetPackAsset()->GetItemCountInPack(PACK_TYPE_MATERIAL, lMaterial2ID);
//				if (nPlayerMaterial2Num < pkProductionData->_lMaterial2Num)
//				{
//					CF_ItemList* pkCSVItemList = CF_ItemList::GetSingletonPtr();
//					CF_ItemList::DataEntry* pkItemaData = pkCSVItemList->GetEntryPtr(lMaterial2ID);
//
//					StringType strMaterialName = pkItemaData->_kName;
//					SendLogicInfoMessage(RE_PRODUCTION_MATERIAL_NOT_ENOUGH, strMaterialName.c_str());
//					return;
//				}
//			}
//
//			long lMaterial3ID = pkProductionData->_lMaterial3ID;
//			int	 nPlayerMaterial3Num = 0;
//			if (IS_ITEM_ID(lMaterial3ID))
//			{
//				nPlayerMaterial3Num = pkGamePlayer->GetPackAsset()->GetItemCountInPack(PACK_TYPE_MATERIAL, lMaterial3ID);
//				if (nPlayerMaterial3Num < pkProductionData->_lMaterial3Num)
//				{
//					CF_ItemList* pkCSVItemList = CF_ItemList::GetSingletonPtr();
//					CF_ItemList::DataEntry* pkItemaData = pkCSVItemList->GetEntryPtr(lMaterial3ID);
//
//					StringType strMaterialName = pkItemaData->_kName;
//					SendLogicInfoMessage(RE_PRODUCTION_MATERIAL_NOT_ENOUGH, strMaterialName.c_str());
//					return;
//				}
//			}
//
//			long lMaterial4ID = pkProductionData->_lMaterial4ID;
//			int	 nPlayerMaterial4Num = 0;
//			if (IS_ITEM_ID(lMaterial4ID))
//			{
//				nPlayerMaterial4Num = pkGamePlayer->GetPackAsset()->GetItemCountInPack(PACK_TYPE_MATERIAL, lMaterial4ID);
//				if (nPlayerMaterial4Num < pkProductionData->_lMaterial4Num)
//				{
//					CF_ItemList* pkCSVItemList = CF_ItemList::GetSingletonPtr();
//					CF_ItemList::DataEntry* pkItemaData = pkCSVItemList->GetEntryPtr(lMaterial4ID);
//
//					StringType strMaterialName = pkItemaData->_kName;
//					SendLogicInfoMessage(RE_PRODUCTION_MATERIAL_NOT_ENOUGH, strMaterialName.c_str());
//					return;
//				}
//			}
//
//			long lMaterial5ID = pkProductionData->_lMaterial5ID;
//			int	 nPlayerMaterial5Num = 0;
//			if (IS_ITEM_ID(lMaterial5ID))
//			{
//				nPlayerMaterial5Num = pkGamePlayer->GetPackAsset()->GetItemCountInPack(PACK_TYPE_MATERIAL, lMaterial5ID);
//				if (nPlayerMaterial5Num < pkProductionData->_lMaterial5Num)
//				{
//					CF_ItemList* pkCSVItemList = CF_ItemList::GetSingletonPtr();
//					CF_ItemList::DataEntry* pkItemaData = pkCSVItemList->GetEntryPtr(lMaterial5ID);
//
//					StringType strMaterialName = pkItemaData->_kName;
//					SendLogicInfoMessage(RE_PRODUCTION_MATERIAL_NOT_ENOUGH, strMaterialName.c_str());
//					return;
//				}
//			}
//
//			long lMaterial6ID = pkProductionData->_lMaterial6ID;
//			int	 nPlayerMaterial6Num = 0;
//			if (IS_ITEM_ID(lMaterial6ID))
//			{
//				nPlayerMaterial6Num = pkGamePlayer->GetPackAsset()->GetItemCountInPack(PACK_TYPE_MATERIAL, lMaterial6ID);
//				if (nPlayerMaterial6Num < pkProductionData->_lMaterial6Num)
//				{
//					CF_ItemList* pkCSVItemList = CF_ItemList::GetSingletonPtr();
//					CF_ItemList::DataEntry* pkItemaData = pkCSVItemList->GetEntryPtr(lMaterial6ID);
//
//					StringType strMaterialName = pkItemaData->_kName;
//					SendLogicInfoMessage(RE_PRODUCTION_MATERIAL_NOT_ENOUGH, strMaterialName.c_str());
//					return;
//				}
//			} 
//		}
//		else
//		{
//			SendLogicInfoMessage(msg.m_iResult);
//
//			int iCurrentState = pkGamePlayer->GetCurrentStateType();
//			if(iCurrentState == 29900003)
//			{
//				pkGamePlayer->InterruptSkill(GET_PROCESS_TIME,iCurrentState,SKI_EVENT);
//			}
//		}
//			
//	}
//}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltLearnProduction(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	return;
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltForgetProduction(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	return;
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltUpgradeProduction(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	return;
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltLearnFormula(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	return;
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerItemProtocol(NW_Game2ClientSession *pkSession, uint32_t iMsgType, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	LC_ClientGamePlayer*	 pkPlayer				= pkSession->GetPlayer();
	if (NULL == pkPlayer)
		return;

	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//如果还未进入游戏，则退出
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;

	long		lItemID = 0;
	long		lItemCount=1;
	StringType strItemName = "";

	MG_MessageCenter* pkMessageCenter = MG_MessageCenter::GetSingletonPtr();
	if(!pkMessageCenter)
		return;

	ResultType eResult = RE_FAIL;

	switch (iMsgType)
	{
	case MGPT_RLT_USE_ITEM_IN_PACKAGE:
		{
			MG_RLT_UseItemInPackage msgUse;
			if(msgUse.Decode(pMsg,wSize))
			{
				UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();

				lItemID = msgUse.m_lItemID;
				lItemCount = msgUse.m_iParam;
				CSVFile::CF_ItemList::DataEntry* pkData =  CSVFile::CF_ItemList::GetSingleton().GetEntryPtr(lItemID);
				if (pkData->_lCategory2 == ITEM_CATEGORY_DRUG)
					g_soundMgr.Play2DEx(VARIABLE2STRING(SE_Sys_TakeFood));
				if (lItemID >= 11311101 && lItemID <= 11311104)		//使用讨伐令的音效
				{
					g_soundMgr.Play2DEx(VARIABLE2STRING(SE_Sys_CrusadeMission_Act));
					
					//显示界面
					LC_GameEvent kGameEvent;
					kGameEvent.SetEventType(GLET_CLIENT_SHOW_CRUSADETASK);
					kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, lItemID);

					pkUIManager->HandlerGameEvent(kGameEvent);
				}
				if (lItemID == 110011005 || lItemID == 110011006)
				{
					//通过宝具石获得宝具
					LC_GameEvent kGameEvent;
					kGameEvent.SetEventType(GLET_SYNC_ADDRUNEBYITEM);
					kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, lItemID);
					pkUIManager->HandlerGameEvent(kGameEvent);
				}
				eResult = RE_ITEM_USE_SUCCESS;
				LC_ClientGamePlayer* pkPlayer = pkSession->GetPlayer();
				pkPlayer->GetControllerItemCoolDownMap()->RefreshItemCD(lItemID);

				LC_GameEvent kGameEvent;
				kGameEvent.SetEventType(GLET_USE_ITEM_IN_PACKAGE);
				kGameEvent.SetEventParam(EP_USE_ITEM_ID, lItemID);

				pkUIManager->HandlerGameEvent(kGameEvent);
			}
		}
		break;
	
	case MGPT_RLT_PACK_SPLIT_ITEM:
		{
			MG_RLT_PACK_SplitItem msgSplit;
			if(msgSplit.Decode(pMsg,wSize))
			{
				lItemID = msgSplit.m_lItemID;
				//eResult = RE_ITEM_SPLIT_SUCCESS;
			}
		}
		break;
	case MGPT_RLT_PACK_SWITCH_ITEM:
		{
			MG_RLT_PACK_SwitchItem msgSwitch;
			if(msgSwitch.Decode(pMsg,wSize))
			{
				if (msgSwitch.m_Result == RE_SUCCESS)
				{
					if(msgSwitch.m_iSrcPackType == msgSwitch.m_iTargetPackType)
					{
						eResult = RE_ITEM_EXCHANGE_INPACKAGE_SUCCESS;
					}	
					else
					{
						if (IS_WAREHOUSE_PACK(msgSwitch.m_iTargetPackType))
						{
							eResult = RE_WAREHOUSE_PUTINTO_SUCESS;
						}

						else if (IS_WAREHOUSE_PACK(msgSwitch.m_iSrcPackType))
						{
							eResult = RE_WAREHOUSE_TAKEOUT_SUCESS;
						}
					}
				}
				else
				{
					if (msgSwitch.m_Result != RE_ITEM_LOCKED)
					{
						//目标格子
						LC_BackPackEntry* pkEntryTemp = pkPlayer->GetPackAsset()->GetBackPackEntry(msgSwitch.m_iTargetPackType, msgSwitch.m_iTargetIndex);
						if ( pkEntryTemp && !(pkEntryTemp->GetValid()) && pkEntryTemp->GetLocked())
						{
							pkEntryTemp->SetLocked(false);
						}
						//源格子
						pkEntryTemp = pkPlayer->GetPackAsset()->GetBackPackEntry(msgSwitch.m_iSrcPackType, msgSwitch.m_iSrcIndex);
						if ( pkEntryTemp && !(pkEntryTemp->GetValid()) && pkEntryTemp->GetLocked())
						{
							pkEntryTemp->SetLocked(false);
						}
					}
					eResult = ResultType(msgSwitch.m_Result);
				}
			}
		}
		break;
	case MGPT_RLT_MERGE_ITEM_IN_PACKAGE:
		{
			MG_RLT_MergeItemInPackage msgMerge;
			if(msgMerge.Decode(pMsg,wSize))
			{
				lItemID = msgMerge.m_lItemID;
				eResult = RE_ITEM_MERGE_SUCCESS;
			}
		}
		break;
	case MGPT_RLT_TIDY_ITEM_IN_PACKAGE:
		{
			MG_RLT_PACK_Tidy msgTidy;
			if(msgTidy.Decode(pMsg,wSize))
			{
				eResult = RE_ITEM_TIDY_SUCCESS;
				g_soundMgr.Play2DEx(VARIABLE2STRING(SE_Sys_Item_Arrange));
			}
		}
		break;
		//BUG #16709 【BUG】【购买道具】购买大量的道具，会出现大量的购买提示的情况
		//许侃说这个只要ADDITEM事件中的内容显示就可以了
	/*case MGPT_RLT_NPCSHOP_BUY:
		{
			MG_RLT_NPCShopBuy msgBuy;
			if(msgBuy.Decode(pMsg,wSize))
			{
				eResult = msgBuy.m_iResultType;
				lItemID = msgBuy.m_lItemID;
				lItemCount = msgBuy.m_lItemCount;
			}
		}
		break;*/
	
	case MGPT_RLT_DESTROY_ITEM_IN_PACKAGE:
		{
			MG_RLT_DestroyItemInPackage msgDestroy;
			if(msgDestroy.Decode(pMsg,wSize))
			{
				lItemID = msgDestroy.m_lItemID;
				eResult = RE_ITEM_DESTORY_SUCCESS;
				g_soundMgr.Play2DEx(VARIABLE2STRING(SE_Sys_Item_Delete));
			}
		}
		break;
	case MGPT_RLT_IDENTIFY_ITEM_IN_PACKAGE:
		{
			MG_RLT_IdentifyItemInPackage msgIdentify;
			if(msgIdentify.Decode(pMsg,wSize))
			{
				lItemID = msgIdentify.m_lItemID;
				eResult = msgIdentify.m_ResultID ? RE_SUCCESS : RE_ITEM_IDENTIFY_SUCCESS;
				
				if (eResult == RE_ITEM_IDENTIFY_SUCCESS)
				{
					g_soundMgr.Play2DEx(VARIABLE2STRING(SE_Sys_Item_Identify));
				}

				LC_GameEvent kGameEvent;
				kGameEvent.SetEventType(GLET_ITEM_IDENTIFY);
				kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, lItemID);
				kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, msgIdentify.m_lPackType);
				kGameEvent.SetEventParam(EVENT_BASE_PARAM_3, msgIdentify.m_lIndex);
				kGameEvent.SetEventParam(EVENT_BASE_PARAM_4, msgIdentify.m_ResultID);

				UI_UIManager::GetSingletonPtr()->HandlerGameEvent(kGameEvent);
			}
		}
		break;
	case MGPT_RLT_EQUIP_VEHICLE_EQCMENT:
		{
			MG_RLT_Equip_VehicleEqcment msgEquip;
			if(msgEquip.Decode(pMsg,wSize))
			{
				lItemID = msgEquip.m_lItemID;
				eResult = RE_ITEM_EQUIP_SUCCESS;
			}
		}
		break;
	case MGPT_RLT_UNEQUIP_VEHICLE_EQCMENT:
		{
			MG_RLT_UnEquip_VehicleEqcment msgUnEquip;
			if(msgUnEquip.Decode(pMsg,wSize))
			{
				lItemID = msgUnEquip.m_lItemID;
				eResult = RE_ITEM_UNEQUIP_SUCCESS;
			}
		}
		break;
	}


	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	LC_ItemBase* pkItem = pkItemFactory->RequestItem(lItemID);
	CF_ItemList::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_ItemList>(lItemID);
	if(pkItem && pkData)
	{
		strItemName = LC_Helper::GetGradeName(pkItem->GetProperty_Name(), pkData->_lGrade);
	}
	else
	{
		strItemName = "未知物品";
	}

	SendLogicInfoMessage(eResult,strItemName,lItemCount);
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltGameEvent(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	////如果还未进入游戏，则退出
	//if (eGameStage != ST_GAME_STAGE_GAME)
	//	return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if (NULL == pkGamePlayer)
		return ;

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();

	MG_PB<msg_define::game_event> pbMsg;
	if (pbMsg.Decode(pMsg,wSize))
	{
		LC_GameEvent kGameEvent;
		kGameEvent.GetFromProtocol(pbMsg.m_value);
		StringType	 kStringParam = STDSTR2TPSTR(pbMsg.m_value.string_param());
		LC_GameEventHandler::HandlerGameEvent(kGameEvent,kStringParam,pkGamePlayer);

		pkUIManager->HandlerGameEvent(kGameEvent);
	}
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncShapeShift(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//如果还未进入游戏，则退出
// 	if (eGameStage != ST_GAME_STAGE_GAME)
// 		return;


	MG_RLT_ShapeShiftInfo RltMsg;
	if(!RltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"解析传送数据错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

	object_id_type lPlayerID= RltMsg.m_lPlayerID;
	bool bShapeShift		= RltMsg.m_bShapeShift != 0; 
	long lShapeShiftTypeID	= RltMsg.m_lShapeShiftTypeID;
	
	if (lPlayerID == pkGamePlayer->GetID())
	{
		if (pkGamePlayer->GetShapeShiftFlag() != bShapeShift)
		{

			pkGamePlayer->SetShapeShiftFlag(bShapeShift);
			pkGamePlayer->SetShapeShiftCharTypeID(lShapeShiftTypeID);
			pkGamePlayer->ChangeVisibleData(lShapeShiftTypeID);
			if (pkGamePlayer->GetShapeShiftCharTypeID() == 0)
			{
				pkGamePlayer->DoUpdateBowAvatar();	
				pkGamePlayer->DoUpdateWingAvatar();
			}
			//这个函数的功能是用于客户端主角变身切换技能栏的相关设置，现在不用了，以后如果需要此功能，则将变身chartypeID和相关逻辑用表对应起来
			//pkGamePlayer->SetShapeShiftInfo(lShapeShiftTypeID);
		}
	}
	else
	{
		LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
		if(!pkNetPlayerManager)
			return;

		LC_ClientNetPlayer*	pkNetPlayer = (LC_ClientNetPlayer*)(pkNetPlayerManager->FindPlayer(lPlayerID));
		if(!pkNetPlayer)
			return;
		
		if (pkNetPlayer->GetShapeShiftFlag() != bShapeShift)
		{
			pkNetPlayer->SetShapeShiftFlag(bShapeShift);
			pkNetPlayer->SetShapeShiftCharTypeID(lShapeShiftTypeID);
			if (bShapeShift == false)
				lShapeShiftTypeID = pkNetPlayer->GetCharType();
			pkNetPlayer->ChangeVisibleData(lShapeShiftTypeID);

			if (pkNetPlayer->GetShapeShiftCharTypeID() == 0)
			{
				pkNetPlayer->DoUpdateBowAvatar();
				pkNetPlayer->DoUpdateWingAvatar();
			}
		}
	}

}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltNPCOwnerShip(NW_Game2ClientSession* pkSession,char* pMsg, uint16_t wSize)
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//如果还未进入游戏，则退出
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;

	MG_RLT_NPC_Ownership rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_REQ_NPC_Ownership解析错误");
		return;
	}
	
	LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
	if(!pkNPCManager)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_REQ_NPC_Ownership错误");
		return;
	}

	LC_ClientNPC* pkNPC = (LC_ClientNPC*)(pkNPCManager->FindNPC(rltMsg.m_lNPCID));
	if(!pkNPC)
		return;

	pkNPC->SetOwnerShip(rltMsg.m_bOwnerShip);
}
//---------------------------------------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltQuitCareerSelect(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//如果还未进入游戏，则退出
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;

	MG_RLT_CAREER_SELECTION RltMsg;
	if(!RltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"解析传送数据错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if (!pkGamePlayer || pkGamePlayer->GetID() != RltMsg.m_lPlayerID)
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "屏蔽列表消息逻辑错误");
		return;
	}

	ResultType iResult   = RltMsg.m_iResult;
	if(iResult == RE_SUCCESS)
	{
        //LC_GameEvent kGameEvent;
        //kGameEvent.SetEventType(GLET_PREPARE_CAREER_STUDY);
        //kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, RltMsg.m_lNPCID);

        //(CLIENT_GET_WORLD_MANAGER())->HandleGameEvent(kGameEvent);
	}
	else
	{
		SendLogicInfoMessage(RltMsg.m_iResult);			
	}
}
//-----------------------------------------------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltNPCUpdate(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//如果还未进入游戏，则退出
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;

	MG_RLT_NPC_UPGRADE RltMsg;
	if(!RltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"解析传送数据错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if (pkGamePlayer == NULL)
		return;


	if(RltMsg.m_iResult == RE_SUCCESS)
	{
		LC_ClientTranscriptionInfoManager* pkTranscriptionInfoManager = CLIENT_GET_TRANSCRIPTION_INFO_MANAGER();
		if(!pkTranscriptionInfoManager)
			return;
		int iIndex = pkTranscriptionInfoManager->GetNPCUpdateIndex(RltMsg.m_lNPCID);
		if (iIndex < 0)
			return;

		pkTranscriptionInfoManager->SetNPCUpdataID(iIndex,RltMsg.m_lNextNPCID);
		pkTranscriptionInfoManager->SetNPCUpdataTypeID(iIndex,RltMsg.m_lNextNPCCharType);
		int iState = pkTranscriptionInfoManager->GetNPCUpdataState(iIndex);
		pkTranscriptionInfoManager->SetNPCUpdataState(iIndex,++iState);
	}
	else
	{
		SendLogicInfoMessage(RltMsg.m_iResult);		
	}

}

void HL_FromGameServer::_handlerGameStoryInfoSync(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if (pkGamePlayer == NULL)
		return;

	MG_GAMESTORY_CONTENT_INFO_SYNC kGameStoryContentInfo;

	if(!kGameStoryContentInfo.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"解析传送数据错误");
		return;
	}	

	LC_ClientGameStoryManager* pkGameStoryManager = CLIENT_GET_GAMESTORY_MANAGER();
	if(!pkGameStoryManager)
		return;
	pkGameStoryManager->ReceiveGameStoryContentInfosync(kGameStoryContentInfo);
	//pkGamePlayer->UpdateNearNPCTaskState();//BUG #33433 【BUG】【运镖】运镖NPC无法对话

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_GAMESTORY_TIMEINFO);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, kGameStoryContentInfo.m_kGameStoryContent[kGameStoryContentInfo.m_iCount - 1].m_uiStoryID);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}
void HL_FromGameServer::_handlerGameStoryBatchInfoSync(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	MG_GAMESTORY_BATCH_INFO_SYNC kGameStoryBatchInfo;

	if(!kGameStoryBatchInfo.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"解析传送数据错误");
		return;
	}	

	if (kGameStoryBatchInfo.m_kGameStoryBatch.m_uiStoryID >= 61 && kGameStoryBatchInfo.m_kGameStoryBatch.m_uiStoryID <= 69)
	{
		StringStreamType sStream;
		for (int i=0;i<kGameStoryBatchInfo.m_kGameStoryBatch.m_kAreaInfos.size();i++)
		{
			const PS_GAMESTORY_KEY_INFO &Info = kGameStoryBatchInfo.m_kGameStoryBatch.m_kAreaInfos[i].m_kKeyInfos[0];
			sStream << "m_iResID:" << Info.m_iResID << " m_iLCID:" << Info.m_iLCID << " m_uiCharID:" << Info.m_uiCharID << " m_szCharName:" << Info.m_szCharName 
				<< " m_nState:" << Info.m_nState << " m_uiTimestamp:" << Info.m_uiTimestamp << " m_nLineNum:" << Info.m_nLineNum << "#";
		}
		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"收到地宫boss信息，m_uiStoryID=%d, info=%s， curLineNum=%d",kGameStoryBatchInfo.m_kGameStoryBatch.m_uiStoryID, sStream.str().c_str(), pkGamePlayer->GetCurrentLineNum());
	}

	LC_ClientGameStoryManager* pkGameStoryManager = CLIENT_GET_GAMESTORY_MANAGER();
	if(!pkGameStoryManager)
		return;
	pkGameStoryManager->ReceiveGameStoryBatchInfosync(kGameStoryBatchInfo);
}
//----------------------------------------------------------------------------------------------------------------------
void HL_FromGameServer::_handlerNPCExecuteSkillFromServer(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_NOTIFY_CLIENT_PLAYE_SKILL_GFX RltMsg;
	if(!RltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"解析传送数据错误");
		return;
	}
	
	LC_ClientLogicManager* pkLogicManager =  CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager)
	{
		GfxWriteLog(LOG_SKILL_EVENT_FILELINE, LOG_SWITCH_SKILL_EVENT, "获取客户端逻辑管理器错误");
		return;
	}
	object_id_type lGamePlayerID = pkLogicManager->GetOperationPlayerID();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (NULL == pkGamePlayer)
	{
		GfxWriteLog(LOG_SKILL_EVENT_FILELINE, LOG_SWITCH_SKILL_EVENT, "获取游戏玩家错误");
		return;
	}

	IM_Manager* pkIMManager =  IM_Manager::GetSingletonPtr();

	long lSrcActorID	=	RltMsg.m_iAttackerID;
	long lLogicID		=	RltMsg.m_iSkillID;

	CF_SkillExecuteList::DataEntry* pkSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(lLogicID);
	if(!pkSkillData)
		return;

	LC_ClientWorldManager* pkWorldManager = CLIENT_GET_WORLD_MANAGER();
	LC_ActorBase* pkSrcActor = (LC_ActorBase*)pkWorldManager->FindObject(lSrcActorID);
	if (NULL == pkSrcActor)
	{
		GfxWriteLog(LOG_SKILL_EVENT_FILELINE, LOG_SWITCH_SKILL_EVENT, "伤害事件错误，找不到发起人[%i]", lSrcActorID);
		return;
	}

	const StringType * pkBeAttackGfx = Utility::RandomChooseValidCellText(&pkSkillData->_kBeAttackGfx, &pkSkillData->_kBeAttackGfx1, &pkSkillData->_kBeAttackGfx2);
	for(int iIndex = 0; iIndex < SKILL_GFX_MAX_COUNT; iIndex++)
	{
		if(RltMsg.m_iBeAttackerIDArray[iIndex] == 0)	continue;

		long lDstActorID = RltMsg.m_iBeAttackerIDArray[iIndex];
		LC_ActorBase* pkDstActor = (LC_ActorBase*)pkWorldManager->FindObject(lDstActorID);
		if (pkDstActor)
		{
			GD_ActorModel* pkVisibleData = pkIMManager->GetActor(lDstActorID);
			if(pkVisibleData && pkBeAttackGfx)
			{
				pkVisibleData->AttachFx(GD_ModelBase::FxDummyNodeType(pkSkillData->_lBeAttackGfxBindDumy),0,true,*pkBeAttackGfx);				
			}			
		}
	}	
}
//----------------------------------------------------------------------------------------------------------------------
 void HL_FromGameServer::_handlerReceiveSeekTreasureFromServer(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
 {
	 NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	 NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	 int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	 Protocol::MG_RLT_SeekTreasure RltMsg;
	 if(!RltMsg.Decode(pMsg,wSize))
	 {
		 GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"解析传送数据错误");
		 return;
	 }
	 LC_ClientTranscriptionInfoManager* pkTranscriptionInfoManager = CLIENT_GET_TRANSCRIPTION_INFO_MANAGER();
	 if(!pkTranscriptionInfoManager)
		 return;
	 pkTranscriptionInfoManager->ReceiveSeekTreasureMsg(RltMsg);
 }
//----------------------------------------------------------------------------------------------------------------------
void HL_FromGameServer::_handlerStoryTipsInfoFromServer(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	Protocol::MG_TIPS_SERVER_TO_CLIENT RltMsg;
	if(!RltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"解析传送数据错误");
		return;
	}
	LC_ClientGameStoryManager* pkTranscriptionInfoManager = CLIENT_GET_GAMESTORY_MANAGER();
	if(!pkTranscriptionInfoManager)
		return;
	pkTranscriptionInfoManager->ReceiveStoryTipsContent(RltMsg);
}
//------------------------------------------------------------------------------------------------------------------------
void HL_FromGameServer::_handlerPerDayTranscriptionInfoFromServer(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	MG_NOTIFY_CLIENT_TRANSCIRPTION_PER_DAY RltMsg;
	if(!RltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"解析传送数据错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if (pkGamePlayer)
	{
		LC_ClientTranscriptionInfoManager* pkTranscriptionInfoManager = CLIENT_GET_TRANSCRIPTION_INFO_MANAGER();
		if(!pkTranscriptionInfoManager)
			return;
		for (int i = 0; i < MAX_TRANSCRIPTION_PER_DAY_NUM;++i)
		{
			if (RltMsg.m_pkInfo[i].m_uiType > 0)
				pkTranscriptionInfoManager->AddTodayCampaignInfo(RltMsg.m_pkInfo[i].m_uiType,RltMsg.m_pkInfo[i].m_iAlreadyFinishedNumPerDay,
				RltMsg.m_pkInfo[i].m_iMaxNumPerDay);
		}
	}

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_TODAY_CAMPAIGN_EVENT);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerRltSaveShortcutKey(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//如果还未进入游戏，则退出
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;

	MG_RLT_SaveShortcutKey msg;

	if (msg.Decode(pMsg,wSize))
	{
		object_id_type lPlayerID = msg.m_lPlayerID;
		ResultType iResult = msg.m_iResult;

		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

		if(!pkGamePlayer || pkGamePlayer->GetSelfID() != lPlayerID)
		{
			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"请求添加物品到强化背包返回消息逻辑错误");
			return;
		}
	}
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerFixItemProtocol( NW_Game2ClientSession *pkSession, uint32_t iMsgType, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//如果还未进入游戏，则退出
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;

	MG_RLT_FixItemEndurance msg;

	if (msg.Decode(pMsg,wSize))
	{
		ResultType iResult = msg.m_iResult;

		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

		if(!pkGamePlayer)
		{
			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"请求返回消息逻辑错误");
			return;
		}

		if (   iResult == RE_SUCCESS
			|| iResult == RE_ITEM_FIX_ENDURANCE_EMPTY )
		{
			g_soundMgr.Play2DEx(VARIABLE2STRING(SE_Sys_Item_Repair));
			iResult = RE_ITEM_FIX_ENDURANCE_SUCCESS;
		}

		SendLogicInfoMessage(iResult);		
	}
}
void HL_FromGameServer::_handlerRlExchange(NW_Game2ClientSession *pkSession, uint32_t iMsgType, char* pMsg, uint16_t wSize )
{
  /*
	MG_RLT_DoExchange msg;

	if (msg.Decode(pMsg,wSize))
	{
		ResultType iResult = msg.m_iResult;
		long lCookResult = msg.m_iCookRewardID;

		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();

		if(!pkGamePlayer)
		{
			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"请求返回消息逻辑错误");
			return;
		}

// 		if (   iResult == RE_SUCCESS
// 			|| iResult == RE_ITEM_FIX_ENDURANCE_EMPTY )
// 		{
// 			iResult = RE_ITEM_FIX_ENDURANCE_SUCCESS;
// 		}
			
		LC_ClientExchangeManager* pkExchangeMgr = (LC_ClientExchangeManager*)LC_ClientExchangeManager::GetSingletonPtr();
		if (NULL == pkExchangeMgr)
			return ;
		LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
		LC_ItemBase* pkItem = pkItemFactory->RequestItem(lCookResult);
		if(pkItem)
		{
			if( iResult == RE_COOK_DISH_SUCCESS || iResult == RE_COOK_DISH_FAIL)
			{
				SendLogicInfoMessage(iResult,pkItem->GetItemDescriptionEntry()->GetItemName().c_str());
			}
		}
		else if( iResult != RE_SUCCESS )
		{
			SendLogicInfoMessage(iResult);
		}
		pkExchangeMgr->SetExchangeResult(iResult);
	}
  */
}

//------------------------------------------------------------------------
//void HL_FromGameServer::_handlerRltHookInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
//
//{
//	MG_RLT_HookInfo msg;
//
//	if (msg.Decode(pMsg,wSize))
//	{
//		long lPlayerID = msg.m_lPlayerID;
//
//		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
//
//		if(!pkGamePlayer || pkGamePlayer->GetID() != lPlayerID)
//		{
//			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"请求添加物品到强化背包返回消息逻辑错误");
//			return;
//		}	
//
//		long lPlayerTotalHookTime = msg.m_lPlayerTotalHookTime;
//		pkGamePlayer->SetActorTotalHookTime(float(lPlayerTotalHookTime));
//
//		long lPlayerCurrentHookTime = msg.m_lPlayerCurrentHookTime;
//		pkGamePlayer->SetActorCurrentHookTime(float(lPlayerCurrentHookTime));
//
//		long lPlayerHookEfficiency = msg.m_lPlayerHookEfficiency;
//		pkGamePlayer->SetActorHookEfficiency(lPlayerHookEfficiency/1000.0f);
//
//		long lPlayerCurrentHookTotalTime = msg.m_lPlayerCurrentHookTotalTime;
//		pkGamePlayer->SetActorCurrentHookTotalTime(float(lPlayerCurrentHookTotalTime));
//
//		LC_GameEvent kGameEvent;
//		kGameEvent.SetEventType(GLET_CLIENT_HOOK_INFO_UPDATE);
//
//		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
//		pkUIManager->HandlerGameEvent(kGameEvent);
//
//		LC_ActionForbid* pkForbid = pkGamePlayer->GetActionForbid();
//		if (pkForbid)
//		{
//			if(lPlayerCurrentHookTime>0)
//			{
//				if(pkGamePlayer->GetHookForbidCount()==0)
//				{
//					CF_LogicAreaFunc::DataEntry* pkLogicAreaFunc = pkGamePlayer->GetLogicAreaFunc();
//					if (NULL != pkLogicAreaFunc)
//					{
//						//挂机-文
//						if (HAT_SAFE == pkLogicAreaFunc->_iID)
//						{
//							pkForbid->AddActionForbid(LAFT_SKILL,LAF_SKILL_EXCLUDE_SKILL_ID,SKILL_HOOK);
//						}
//						//挂机-武
//						else if (HAT_FIGHT == pkLogicAreaFunc->_iID)
//						{
//							pkForbid->AddActionForbid(LAFT_SKILL,LAF_SKILL_EXCLUDE_SKILL_ID,SKILL_HAND_HOOK+pkGamePlayer->GetCurrentBandWeapon()-1);
//						}
//					}
//					//pkForbid->AddActionForbid(LAFT_SKILL,LAF_SKILL_EXCLUDE_SKILL_ID,SKILL_HAND_HOOK+pkGamePlayer->GetCurrentBandWeapon()-1);
//					pkForbid->AddActionForbid(LAFT_DUEL,LAF_DUEL_MAX_COUNT);
//					pkForbid->AddActionForbid(LAFT_EQUIP,LAF_EQUIP_MAX_COUNT);
//					pkForbid->AddActionForbid(LAFT_GROUP,LAF_GROUP_MAX_COUNT);
//					pkForbid->AddActionForbid(LAFT_TRADE,LAF_TRADE_MAX_COUNT);
//					//pkForbid->AddActionForbid(LAFT_ITEM,LAF_ITEM_MAX_COUNT);
//					pkForbid->AddActionForbid(LAFT_ARENA,LAF_ARENA_MAX_COUNT);
//					pkGamePlayer->SetHookForbidCount(1);
//				}
//				
//			}
//			else
//			{
//				if(pkGamePlayer->GetHookForbidCount()==1)
//				{
//					CF_LogicAreaFunc::DataEntry* pkLogicAreaFunc = pkGamePlayer->GetLogicAreaFunc();
//					if (NULL != pkLogicAreaFunc)
//					{
//						//挂机-文
//						if (HAT_SAFE == pkLogicAreaFunc->_iID)
//						{
//							pkForbid->RemoveActionForbid(LAFT_SKILL,LAF_SKILL_EXCLUDE_SKILL_ID,SKILL_HOOK);
//						}
//						//挂机-武
//						else if (HAT_FIGHT == pkLogicAreaFunc->_iID)
//						{
//							pkForbid->RemoveActionForbid(LAFT_SKILL,LAF_SKILL_EXCLUDE_SKILL_ID,SKILL_HAND_HOOK+pkGamePlayer->GetCurrentBandWeapon()-1);
//						}
//					}
//					pkForbid->RemoveActionForbid(LAFT_DUEL,LAF_DUEL_MAX_COUNT);
//					pkForbid->RemoveActionForbid(LAFT_EQUIP,LAF_EQUIP_MAX_COUNT);
//					pkForbid->RemoveActionForbid(LAFT_GROUP,LAF_GROUP_MAX_COUNT);
//					pkForbid->RemoveActionForbid(LAFT_TRADE,LAF_TRADE_MAX_COUNT);
//					//pkForbid->RemoveActionForbid(LAFT_ITEM,LAF_ITEM_MAX_COUNT);
//					pkForbid->RemoveActionForbid(LAFT_ARENA,LAF_ARENA_MAX_COUNT);
//					pkGamePlayer->SetHookForbidCount(0);
//				}		
//			}
//		}
//	}
//}
void HL_FromGameServer::_handlerRltLeavePrison(NW_Game2ClientSession *pkSession, uint32_t iMsgType, char* pMsg, uint16_t wSize )
{

	MG_RLT_LEAVE_PRISON msg;
	if (msg.Decode(pMsg,wSize))
	{
		ResultType iResult = msg.m_iResult;
		SendLogicInfoMessage(iResult);

	}

}
void HL_FromGameServer::_handlerRltReducePkValue(NW_Game2ClientSession *pkSession, uint32_t iMsgType, char* pMsg, uint16_t wSize )
{
	MG_RLT_REDUCE_PKVALUE msg;
	if (msg.Decode(pMsg,wSize))
	{

		ResultType iResult = msg.m_iResult;
		object_id_type lPlayerID = msg.m_lPlayerID;

		if(iResult == RE_SUCCESS )
		{
			LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
			if(!pkGamePlayer || pkGamePlayer->GetSelfID() != lPlayerID)
				return;
			//long lLevel = pkGamePlayer->GetLevel();
			long lLevel = pkGamePlayer->GetCommanderLevel();
			CF_LevelUp* pkCSVLevelUp = CF_LevelUp::GetSingletonPtr();
			const CF_LevelUp::DataEntry* pkData = pkCSVLevelUp->GetEntryPtr(lLevel);
			if(!pkData)
				return ;
			long lReduceValue = pkData->_ReducePKValue.y;
			SendLogicInfoMessage(RE_REDUCE_PKV_SUCCESS,lReduceValue );

		}
		SendLogicInfoMessage(iResult);

	}
}
void HL_FromGameServer::_handlerRltImprisoned(NW_Game2ClientSession *pkSession, uint32_t iMsgType, char* pMsg, uint16_t wSize )
{

	MG_RLT_IMPRISON msg;
	if(msg.Decode(pMsg, wSize))
	{
// 		LC_GameEvent kGameEvent;
// 		kGameEvent.SetEventType(GLET_CLIENT_IMPRISONED);
// 
// 		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
// 		pkUIManager->HandlerGameEvent(kGameEvent);
		ResultType iResult = msg.m_iResult;
		SendLogicInfoMessage(iResult);

	}
}

void HL_FromGameServer::_handlerRltRequestInfo( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RLT_RequestInfo msg;
	if (!msg.Decode(pMsg,wSize))
		return;

	SendMessageDialog(RE_DIALOG_CONFIRM_ENTER_MAP, "", "", 
		RE_DIALOG_CONFIRM_ENTER_MAP, 
		msg.m_iRequestType, msg.m_iID, msg.m_lParam[0] );
}

void HL_FromGameServer::_handlerRltAntiWallow(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_Anti_Wallow rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
		return;

	NW_ClientSessionManager* pkSessionManager = NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup* pkSessionGroup = pkSessionManager->GetSessionGroup(pkSession);
	if(!pkSessionGroup)
	{
		GfxWriteLog(LOG_TAG_ERROR,LOG_SWITCH_ERROR,"找不到SessionGroup,无法拿到签名\n");
		return;
	}

	pkSessionGroup->SetFcm(rltMsg.m_adult);
	pkSessionGroup->SetOnlineTime(rltMsg.m_onlineTime / 60);

	pkSessionGroup->SetCanCalOnlineTime(true);
	pkSessionGroup->NotifyAntiWallowStateToUIInGame();
}

void HL_FromGameServer::_handlerRltFollowPlayer(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_FollowPlayer rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(!pkGamePlayer)
		return;
	
	if(rltMsg.m_iFollowResult == RE_SUCCESS)
		pkGamePlayer->SetFollowedTargetID(rltMsg.m_lTargetPlayerID);
	else
		pkGamePlayer->SetFollowedTargetID(0);
}

void HL_FromGameServer::_handlerRltGMTransfer(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_GM_Transfer rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
		return;

	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return;

	SC_ClientScriptInterface* pkScriptInterface = SC_ClientScriptInterface::GetSingletonPtr();
	
	LC_MapManager* pkMapManager = (LC_MapManager*)(LC_MapManager::GetSingletonPtr());
	LC_MiniMap* pkMiniMap = pkMapManager->GetMiniMap();

	int iPosX = 0;
	int iPosY = 0;
	pkMiniMap->CalcMapPosToWorld(rltMsg.m_uiPosY, rltMsg.m_uiPosX, iPosX, iPosY);

	Utility::UT_Vec3Int kCurrentLocation;
	kCurrentLocation.x = iPosX;
	kCurrentLocation.y = iPosY;
	kCurrentLocation.z = 0;

	char commond[256] = {0};
	sprintf(commond,"setcurrentlocation %d %d %d %d",pkGamePlayer->GetID(),rltMsg.m_iMapResID,iPosX,iPosY);

	pkScriptInterface->GetDebugInterface()->SendDebugCommandToGameServer(commond);
}

void HL_FromGameServer::_landlerRltTempPackOP(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_OP_TEMP_PACK rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(!pkGamePlayer)
		return;

	if(rltMsg.m_iResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(RE_CHEST_FULL);
	}
}
void HL_FromGameServer::_handlerRltEnterSacredWeapon(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
}
void HL_FromGameServer::_handlerRltQuitSacredWeapon(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
}

void HL_FromGameServer::_handlerEnhanceItemEnd(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	ResultType iResult = RE_FAIL;
	MG_RLT_EnhanceItemInPackage rltMsg;
	if (rltMsg.Decode(pMsg, wSize))
	{
		iResult = rltMsg.m_Result;
	}

	SendLogicInfoMessage(iResult);
	
	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_ENHANCE_END);

	EnhanceResult result = (RE_ENHANCE_SUCCESS == iResult ? ER_SUCCESS : ER_FAILED);
	kGameEvent.SetEventParam(EP_ENHANCE_END_RESULT, result);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerDecompositeItemEnd(NW_Game2ClientSession *pkSession, char *pMsg, uint16_t wSize)
{
	ResultType iResult = RE_FAIL;
	MG_RLT_DecompositItemInPackage rltMsg;
	if (rltMsg.Decode(pMsg, wSize))
	{
		iResult = rltMsg.m_Result;
	}

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_DECOMPOSITION_ITEM_END);
	kGameEvent.SetEventParam(EP_DECOMPOSITION_ITEM_RESULT, iResult);

	UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);

	if (RE_SUCCESS == iResult)
		g_soundMgr.Play2DEx(VARIABLE2STRING(SE_Sys_Item_Split));
	
	SendLogicInfoMessage(iResult);
}

void HL_FromGameServer::_handlerUnSocketItemEnd(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	ResultType iResult = RE_FAIL;
	MG_RLT_UnSocketItemInPackage rltMsg;
	if(rltMsg.Decode(pMsg, wSize))
	{
		iResult = rltMsg.m_Result;
	}

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_UNSOCKET_ITEM_END);
	kGameEvent.SetEventParam(EP_UNSOCKET_ITEM_RESULT, iResult);

	UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);

	SendLogicInfoMessage(iResult);
	g_soundMgr.Play2DEx(VARIABLE2STRING(SE_Sys_Diamond_Detach));
}

void HL_FromGameServer::_handlerItemUnbind(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	ResultType iResult = RE_FAIL;
	 MG_RLT_UnbindItem rltMsg;
	if(rltMsg.Decode(pMsg, wSize))
	{
		iResult = rltMsg.m_iResult;
	}

	SendLogicInfoMessage(iResult);
}

void HL_FromGameServer::_handlerItemRefine(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_RefineItemInPackage rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	if(RE_SUCCESS == rltMsg.m_Result)
	{
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_ITEM_REFINE_SUCCESS);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);

		g_soundMgr.Play2DEx(VARIABLE2STRING(SE_Sys_Item_Refine));
	}

	SendLogicInfoMessage(rltMsg.m_Result);
}

void HL_FromGameServer::_handlerItemRefineApply(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_Common rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	int iResult = get_value(rltMsg, "result", RE_FAIL);
	if(RE_SUCCESS == iResult)
	{
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_ITEM_REFINE_APPLY_SUCCESS);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);		
	}

	SendLogicInfoMessage(iResult);
}

void HL_FromGameServer::_handlerRltExternBasicBag(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_Common rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	int iResult = get_value(rltMsg, "result", RE_FAIL);
	if (RE_SUCCESS == iResult)
	{
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_CLIENT_EXTERN_BASIC_BAG);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, 0);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);		
	}

	SendLogicInfoMessage(iResult);
}

void HL_FromGameServer::_handlerNotifyAutoOpenBasicBag(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_Common rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	int slot = get_value(rltMsg, "slot", 0);
	if (slot > 0)
	{
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_CLIENT_EXTERN_BASIC_BAG);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, slot);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);		
	}
}

void HL_FromGameServer::_handlerNotifyAddtoenermy(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_NOTIFY_ADDTOENEMY rltMsg;
	if (rltMsg.Decode(pMsg, wSize))
	{
		object_id_type lPlayerID = rltMsg.m_lPlayerID;
		unique_id_impl lTargetPlayerID = rltMsg.m_lTargetCharID;

		LC_ClientGamePlayer *pkGamePlayer = pkSession->GetPlayer();
		if(NULL == pkGamePlayer || pkGamePlayer->GetSelfID() != lPlayerID)
		{
			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "角色数值同步消息逻辑错误");
			return;
		}
		
		LC_ClientLogicManager* pkPlayerManager = CLIENT_GET_LOGIC_MANAGER();
		LC_PlayerBase *pkTargetPlayer = pkPlayerManager->FindPlayer(lTargetPlayerID);
		if(NULL == pkTargetPlayer)
		{
			return;
		}

		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_NOTIFY_ADD_ENERMY);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, lTargetPlayerID.detail());
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, lTargetPlayerID.catagory());

		UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
}

void HL_FromGameServer::_handlerSyncRaidEnterMapRecord(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncRaidEnterMapRecord msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncRaidEnterMapRecord解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncRaidEnterMapRecord解析错误 GamePlayer == NULL");
		return;
	}

	bool bNew = msg.m_bFirstEnter;
	std::vector<KeyValue_Base>::const_iterator iter = msg.m_EnterMapRecord.begin();
	for( ; iter!=msg.m_EnterMapRecord.end(); ++iter)
	{
		const KeyValue_Base& info = *iter;
		const int iMapResID = info.key;
		pkGamePlayer->SetMapEnterRecord(iMapResID, info.value);

		if(bNew)
		{
			LC_GameEvent kGameEvent;
			kGameEvent.SetEventType(GLET_ENTER_NEWMAP_NOTIFY);
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, iMapResID);
			UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
			pkUIManager->HandlerGameEvent(kGameEvent);
		}
	}
}

void HL_FromGameServer::_handlerSyncIDAsset(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncIDAsset msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncIDAsset解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncIDAsset解析错误 GamePlayer == NULL");
		return;
	}

	std::vector<PB_KeyValueVec>::const_iterator iter = msg.m_IDVec.begin();
	for( ; iter!=msg.m_IDVec.end(); ++iter)
	{
		const PB_KeyValueVec& info = *iter;
		if(info.m_nKey == IDType_1)
		{
			pkGamePlayer->SyncCanEnterMapIDSet(info.m_nValue);
			break;
		}
	}
}

void HL_FromGameServer::_handlerRltAddID(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RESP_AddID msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncIDAsset解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncIDAsset解析错误 GamePlayer == NULL");
		return;
	}

	if(msg.m_nResult == RE_SUCCESS
		&& msg.m_nType == IDType_1
		&& msg.m_nValue > 0)
	{
		pkGamePlayer->AddCanEnterMapID(msg.m_nValue);
		
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_CANENTER_MAPID_ADD);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_nValue);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
}

void HL_FromGameServer::_handleRltStatOperation( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_PB<msg_define::common_stat_operation_resp> resqMsg;
	if (!resqMsg.Decode(pMsg,wSize))
		return;
	if(resqMsg.m_value.result()!=RE_SUCCESS)
	{
		if (RE_ITEM_PACKAGE_FULL == resqMsg.m_value.result())
		{
			SendLogicInfoMessage(RE_ITEM_REWARD_PACKAGE_FULL);
		}
		else if (RE_UNBIND_YUANBAO_NOT_ENOUGH == resqMsg.m_value.result())
		{
			if(resqMsg.m_value.op_code() == LC_CMN_STAT_OP_REPLEVY)
			{
				SendLogicInfoMessage(RE_PURCHASE_YUANBAO_NOTENOUGH);
			}
		}
		else
		{
			SendLogicInfoMessage(resqMsg.m_value.result());
		}

		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_CLIENT_STAT_FAIL);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_1,resqMsg.m_value.type());
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_2,resqMsg.m_value.op_code());
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_3,resqMsg.m_value.op_para());
		UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
	else
	{
		StringStreamType sStream("");
		sStream << resqMsg.m_value.type() << "#" << resqMsg.m_value.op_code() << "#" << resqMsg.m_value.op_para() << "#";
		for (uint32_t i = 0; i < resqMsg.m_value.fvalues_size(); ++i)
		{
			sStream << resqMsg.m_value.fvalues(i) << "|";
		}
		sStream << "#";
		for (uint32_t i = 0; i < resqMsg.m_value.ivalues_size(); ++i)
		{
			sStream << resqMsg.m_value.ivalues(i) << "|";
		}
		sStream << "#";
		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerMsgGameEvent(MGPT_COMMON_STAT_OPERATION_RESP, sStream.str());
	}
}
void HL_FromGameServer::_handleRltActivityCommonOperation( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_PB<msg_define::activity_common_op_resp> resqMsg;
	if (!resqMsg.Decode(pMsg,wSize))
		return;

	const uint32_t iResult = resqMsg.m_value.rslt();
	SendLogicInfoMessage(iResult);

	int iActivityType = resqMsg.m_value.req().type();
	int iOpCode = resqMsg.m_value.req().code();
	int iCategory = resqMsg.m_value.req().catagory();
	int iSchedule = resqMsg.m_value.req().schedule();
	GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI, "HL_FromGameServer::_handleRltActivityCommonOperation result=%d iActivityType=%d iOpCode=%d iCategory=%d iSchedule=%d", 
						iResult, iActivityType, iOpCode, iCategory, iSchedule);

	if (iActivityType == ACT_SCHEDULE_TP_MONOPOLY || iActivityType == ACT_SCHEDULE_TP_SUBORDINATE_CONSUME)
	{
		StringStreamType sStream("");
		
		int moveStep = 0;
		if (resqMsg.m_value.rslt() == RE_SUCCESS)
		{
			moveStep = resqMsg.m_value.values(0);
		}		

		sStream << iResult <<"#"<<iOpCode<<"#"<<moveStep<<"#"<<iActivityType<<"#";
		LC_ClientGameEvent kGameEvent;
		kGameEvent.SetClientEventType(GameLogic::GameEventType(MGPT_ACTIVITY_COMMON_OP_RESP));
		LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(sStream.str());
		kGameEvent.SetUserData(&kUserData);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);	
	}
	else if (iActivityType == ACT_SCHEDULE_TP_CLIMB_TOWER )
	{
		StringStreamType sStream("");
		ResultType res = resqMsg.m_value.rslt();
		sStream << res<<"#" << resqMsg.m_value.req().type() << "#" << resqMsg.m_value.req().catagory() << "#" << resqMsg.m_value.req().schedule() << "#" << resqMsg.m_value.req().code() << "#";		
		for (int i=0;i<resqMsg.m_value.values_size();++i)
		{
			sStream<<resqMsg.m_value.values(i)<<"|";
		}

		LC_ClientGameEvent kGameEvent;
		kGameEvent.SetClientEventType(GameLogic::GameEventType(MGPT_ACTIVITY_COMMON_OP_RESP));
		LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(sStream.str());
		kGameEvent.SetUserData(&kUserData);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);	
	}
	else if (iActivityType == ACT_SCHEDULE_TP_SNOW_MAN || iActivityType == ACT_SCHEDULE_TP_REFRSH_SHOP)
	{
		StringStreamType sStream("");
		ResultType res = resqMsg.m_value.rslt();
		int opCode = resqMsg.m_value.req().code();
		int key = resqMsg.m_value.req().params(ACT_COMMON_OP_PARAM_KEY);

		sStream << res<<"#" << resqMsg.m_value.req().type() << "#" << resqMsg.m_value.req().catagory() << "#" << resqMsg.m_value.req().schedule() << "#" << resqMsg.m_value.req().code() << "#" <<key<<"#";		
		for (int i=0;i<resqMsg.m_value.values_size();++i)
		{
			sStream<<resqMsg.m_value.values(i)<<"|";
		}

		LC_ClientGameEvent kGameEvent;
		kGameEvent.SetClientEventType(GameLogic::GameEventType(MGPT_ACTIVITY_COMMON_OP_RESP));
		LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(sStream.str());
		kGameEvent.SetUserData(&kUserData);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);	
	}
	//else if(iActivityType == ACT_SCHEDULE_TP_LOTTERY)
	//{
	//	StringStreamType sStream("");
	//	if (resqMsg.m_value.req().code()==ACT_COMMON_TREASURE_ADD)
	//	{
	//		if (resqMsg.m_value.rslt()!=0)
	//		{
	//			SendLogicInfoMessage(resqMsg.m_value.rslt());
	//			return;
	//		}
	//		else
	//		{
	//			//itemid:itemcount:bind
	//			for (int i=0;i<resqMsg.m_value.values_size();++i)
	//			{
	//				if (i%3 == 0)
	//				{
	//					sStream<<resqMsg.m_value.values(i)<<":";
	//				}
	//				else if(i%3 == 1)
	//				{
	//					sStream<<resqMsg.m_value.values(i)<<";";
	//				}
	//			}
	//			LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	//			pkGamePlayer->UpdateDrawCardCumulateReward(sStream.str());
	//			LC_GameEvent kGameEvent2;
	//			kGameEvent2.SetEventType(GLET_RLT_CUMULATEREWARD);
	//			UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	//			pkUIManager->HandlerGameEvent(kGameEvent2);
	//		}
	//	}
	//	else
	//	{
	//		
	//		ResultType res = resqMsg.m_value.rslt();
	//		int opCode = resqMsg.m_value.req().code();
	//		int key = resqMsg.m_value.req().params(ACT_COMMON_OP_PARAM_KEY);

	//		sStream << res<<"#" << resqMsg.m_value.req().type() << "#" << resqMsg.m_value.req().catagory() << "#" << resqMsg.m_value.req().schedule() << "#" << resqMsg.m_value.req().code() << "#" <<key<<"#";		
	//		for (int i=0;i<resqMsg.m_value.values_size();++i)
	//		{
	//			sStream<<resqMsg.m_value.values(i)<<"|";
	//		}

	//		LC_ClientGameEvent kGameEvent;
	//		kGameEvent.SetClientEventType(GameLogic::GameEventType(MGPT_ACTIVITY_COMMON_OP_RESP));
	//		LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(sStream.str());
	//		kGameEvent.SetUserData(&kUserData);

	//		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	//		pkUIManager->HandlerGameEvent(kGameEvent);	
	//	}
	//}
	else
	{
		//if (resqMsg.m_value.rslt() == RE_SUCCESS)
		//{
			StringStreamType sStream("");
			sStream << resqMsg.m_value.req().type() << "#" << resqMsg.m_value.req().catagory() << "#" << resqMsg.m_value.req().schedule() << "#" << resqMsg.m_value.req().code() << "#";
			for (uint32_t i = 0; i < resqMsg.m_value.req().params_size(); ++i)
			{
				sStream << resqMsg.m_value.req().params(i) << "|";
			}
			sStream << "#";
			UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
			pkUIManager->HandlerMsgGameEvent(MGPT_ACTIVITY_COMMON_OP_RESP, sStream.str());
		//}
	}

}
void HL_FromGameServer::_handleRltActivityScheduleInfo( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_PB<msg_define::schedule_info> resqMsg;
	if (!resqMsg.Decode(pMsg,wSize))
		return;

	LC_ClientActivityManager::GetSingletonPtr()->GetDataFromPB(resqMsg.m_value);
}
void HL_FromGameServer::_handleRltUseActivationCode( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_PB<msg_define::use_activation_code_resp> resqMsg;
	if (!resqMsg.Decode(pMsg,wSize))
		return;
	if(resqMsg.m_value.rslt() != RE_SUCCESS)
	{
		SendLogicInfoMessage(resqMsg.m_value.rslt());
	}
	else
	{
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_CLIENT_USE_ACTIVATION_CODE_SUCESS);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, resqMsg.m_value.type());

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
}

void HL_FromGameServer::_handleRltUseActivationCount( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_PB<msg_define::activation_code_use_count_resp> resqMsg;
	if (!resqMsg.Decode(pMsg,wSize))
		return;

	LC_ClientGamePlayer *pkGamePlayer = pkSession->GetPlayer();
	if (pkGamePlayer == NULL)
	{
		return;
	}
	pkGamePlayer->SetUseActivationCount(resqMsg.m_value.type(),resqMsg.m_value.count());

}

void HL_FromGameServer::_handleRltChestsOperation( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_TreasureChestsOperationRESP resqMsg;
	if (!resqMsg.Decode(pMsg,wSize))
		return;
	if(resqMsg.m_Result!= RE_SUCCESS)
	{
		if(resqMsg.m_Result == RE_UNBIND_YUANBAO_NOT_ENOUGH)
		{
			SendLogicInfoMessage(RE_YUANBAO_NOT_ENOUGH_IN_TREASURE);
		}
		else
		{
			SendLogicInfoMessage(resqMsg.m_Result);
		}

		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_TREASURE_OPERATION);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_1,resqMsg.m_Result);
		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}else if(resqMsg.m_iID == 1)
	{
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_TREASURE_FIRST_SUCCESS);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}else
	{
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_TREASURE_YUANBAO_SUCCESS);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
}

//全服记录
void HL_FromGameServer::_handleRltChestsLogs( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_TreasureChestsLogsNotify resqMsg;
	if (!resqMsg.Decode(pMsg,wSize))
		return;

	LC_TreasureChestsManager* pkTreasureChestsManager = LC_TreasureChestsManager::GetSingletonPtr();
	
	pkTreasureChestsManager->GetDataFromProtocolStruct(resqMsg);
	
	int iLogSize = resqMsg.m_kTreasureLogs.size();

	if(iLogSize > 0) //客户端主动请求返回
	{
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_TREASURE_CHESTS_LOGS);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}

	if(resqMsg.m_iBoradcast != 0 && iLogSize > 0)
	{
		for(int i=0; i<iLogSize; ++i)
		{
			StringType kItemName = "";
			char numstr[100];
			ltoa(resqMsg.m_kTreasureLogs[i].m_iTypeID, numstr, 10);
			kItemName = kItemName + "[@" + numstr +"]";
			StringType kCharName = "";
			kCharName = kCharName + "<color val = 0xff00ff00>[<link val = 1>" + STDSTR2TPSTR(resqMsg.m_kTreasureLogs[i].m_sCharName) + "</link>]</color>";
			SendLogicInfoMessage(RE_TERASURE_LOG_NOTIFY, kCharName,
				kItemName, resqMsg.m_kTreasureLogs[i].m_iCount);
		}
	}
}
void HL_FromGameServer::_handleRltSecretTreasureOperation(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_PB<msg_define::activity_secret_treasure_op_resp> respMsg;
	if (!respMsg.Decode(pMsg,wSize))
		return;

	StringStreamType sStream("");
	ResultType res = respMsg.m_value.rslt();
	int opCode = respMsg.m_value.req().code();
	sStream << res <<"#"<<opCode<<"#";
	if(res!= RE_SUCCESS)
	{
		SendLogicInfoMessage(res);
	}
	else if(opCode == Activity::ACT_SECRET_TREASURE_OP_OPEN) 
	{
		int chestID = respMsg.m_value.values(0);
		int count = respMsg.m_value.values(1);

		sStream<<chestID<<"#"<<count<<"#";

		for (int i=0;i<count;++i)
		{
			sStream<<respMsg.m_value.values(2*(i+1))<<"#"<<respMsg.m_value.values(2*(i+1)+1)<<"#";
		}
	}
	else if (opCode == Activity::ACT_SECRET_TREASURE_OP_SHOP)
	{
		int shopid = respMsg.m_value.values(0);
		sStream << shopid<<"#";
	}
	LC_ClientGameEvent kGameEvent;
	kGameEvent.SetClientEventType(GameLogic::GameEventType(MGPT_ACTIVITY_SECRET_TREASURE_OP_RESP));
	LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(sStream.str());
	kGameEvent.SetUserData(&kUserData);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);	
}
void HL_FromGameServer::_handleRltChessMatrixOperation(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_PB<msg_define::activity_common_op_resp> respMsg;
	if (!respMsg.Decode(pMsg,wSize))
		return;

	StringStreamType sStream("");
	ResultType res = respMsg.m_value.rslt();
	int opCode = respMsg.m_value.req().code();
	int moveStep = respMsg.m_value.values(0);

	sStream << res <<"#"<<opCode<<"#"<<moveStep<<"#";
	if(res!= RE_SUCCESS)
	{
		SendLogicInfoMessage(res);
	}
	LC_ClientGameEvent kGameEvent;
	kGameEvent.SetClientEventType(GameLogic::GameEventType(MGPT_ACTIVITY_COMMON_OP_RESP));
	LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(sStream.str());
	kGameEvent.SetUserData(&kUserData);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);	
}

void HL_FromGameServer::_handleSyncSecretTreasureInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_PB<char_data_define::secret_treasure_info> syncMsg;
	if (!syncMsg.Decode(pMsg,wSize))
		return;


	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return;
	}

	ACT_SecretTreasureAsset& asset = pkGamePlayer->GetSecretTreasureAsset();
	asset.Clear();
	asset.GetDataFromPB(syncMsg.m_value);

	StringStreamType sStream("");
	LC_ClientGameEvent kGameEvent;
	kGameEvent.SetClientEventType(GameLogic::GameEventType(MGPT_SYNC_SECRET_TREASURE_INFO));
	LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(sStream.str());
	kGameEvent.SetUserData(&kUserData);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);		
}

void HL_FromGameServer::_handleSmeltBack(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_PB<msg_define::item_melt_operation_resp> respMsg;
	if (!respMsg.Decode(pMsg,wSize))
		return;

	StringStreamType sStream("");
	ResultType res = respMsg.m_value.result();
	int32_t meltType = respMsg.m_value.melt_type();

	if(res!= RE_SUCCESS)
	{
		SendLogicInfoMessage(res);
	}

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_SMELTER_OPERATION);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1,res);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2,meltType);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handleStarsRuneBack(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_PB<msg_define::stars_rune_operation_resp> respMsg;
	if (!respMsg.Decode(pMsg,wSize))
		return;

	StringStreamType sStream("");
	ResultType res = respMsg.m_value.result();
	int32_t catagory = respMsg.m_value.category();
	int32_t index = respMsg.m_value.index();
	int32_t level = respMsg.m_value.level();
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return;
	}
	if(res!= RE_SUCCESS)
	{
		SendLogicInfoMessage(res);
	}
	else
	{
		pkGamePlayer->GetStarsRuneAsset().UpdateStarsRuneLevel(catagory,index,level);
	}
	pkGamePlayer->UpdateDirtyFlag(DIRTY_FLAG_STARS_RUNE);

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_STARSRUNE_OPERATION);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1,res);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2,catagory);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_3,index);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_4,level);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handleRealTimeComBatRecorder(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SC_RealtimeCombatRecorder respMsg;
	if (!respMsg.Decode(pMsg,wSize))
		return;

	StringStreamType sStream;
	sStream << respMsg.m_nDamageValue << "#" << respMsg.m_nGuildDamageValue;
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerMsgGameEvent(MGPT_SYNC_REALTIME_COMBAT_RECORDER, sStream.str());
}

void HL_FromGameServer::_handlerRltUpgradeMiJi(NW_Game2ClientSession *pkSession, char *pMsg, uint16_t wSize)
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);

	MG_RLT_SkillBookUpgrade rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "客户端解析服务器返回的秘笈系统进阶消息失败");
		return;
	}

	LC_ClientGamePlayer *pkPlayer = pkSession->GetPlayer();
	if(NULL == pkPlayer)
	{
		return;
	}

	if(rltMsg.m_lResult == RE_CONTROLLER_LEVEL_NOT_ENOUGH_SKILLBOOK)
	{
		CF_SkillBookLevelUp::DataEntryMapExternIterator iter = CF_SkillBookLevelUp::GetSingletonPtr()->GetIterator();
		for(; iter.HasMoreElements(); iter.MoveNext())
		{
			const CF_SkillBookLevelUp::DataEntry *pkData = iter.PeekNextValuePtr();
			if(pkData->_iSkillBookID == rltMsg.m_iID && pkData->_iPosition == rltMsg.m_iPos)
			{
				SendLogicInfoMessage(rltMsg.m_lResult, pkData->_iLevel);
				break;
			}
		}
	}
	else
	{
		SendLogicInfoMessage(rltMsg.m_lResult);
	}

	pkPlayer->AddDirtyFlagRevisionID(DIRTY_FLAG_SKILL_BOOK);
	pkPlayer->OnRltUpgradeMiji(rltMsg.m_lResult);
}

void HL_FromGameServer::_handleSyncSkillBookInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncSkillBookInfo syncMsg;
	if (!syncMsg.Decode(pMsg,wSize))
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return;
	}

	LC_SkillBookAsset& pkSkillBookAsset = pkGamePlayer->GetSkillBookAsset();
	pkSkillBookAsset.Load(syncMsg);
	pkGamePlayer->AddDirtyFlagRevisionID(DIRTY_FLAG_SKILL_BOOK);
	pkGamePlayer->OnRltUpgradeMiji(RE_FAIL);
}

void HL_FromGameServer::_handleSyncSkillBookSlotInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncSkillBookSlotInfo syncMsg;
	if (!syncMsg.Decode(pMsg,wSize))
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return;
	}

	pkGamePlayer->SyncSkillBookSlotInfo(syncMsg);

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_SKILLBOOK_SLOT_SYNC);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handleOpenNewSkillBook(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncOpenNewSkillBook syncMsg;
	if (!syncMsg.Decode(pMsg,wSize))
		return;

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_SKILLBOOK_OPEN);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, syncMsg.m_nSkillBookID);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handleRltActivateSkillBook(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_ActivateSkillBook rltMsg;
	if (!rltMsg.Decode(pMsg,wSize))
		return;

	if(rltMsg.m_nResult == RE_SUCCESS)
	{
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_CLIENT_ACTIVE_SKILLBOOK_SUCCESS);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, rltMsg.m_nSkillBookID);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
	else
	{
		SendLogicInfoMessage(rltMsg.m_nResult);
	}
}

void HL_FromGameServer::_handleRltUnlockSkillBook(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_Unlock_SkillBookSlot rltMsg;
	if (!rltMsg.Decode(pMsg,wSize))
		return;

	SendLogicInfoMessage(rltMsg.m_nResult);
}

void HL_FromGameServer::_handleRltWearSkillBook(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_WearSkillBook rltMsg;
	if (!rltMsg.Decode(pMsg,wSize))
		return;

	SendLogicInfoMessage(rltMsg.m_nResult);
}

void HL_FromGameServer::_handleRespEnterCrossMap( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_GAMESTORY_ENTER_CROSS_MAP_RESP respMsg;
	if (!respMsg.Decode(pMsg,wSize))
		return;
	if(respMsg.m_uiRslt != RE_SUCCESS)
	{
		 SendLogicInfoMessage(respMsg.m_uiRslt);
	}
}

void HL_FromGameServer::_handleRespGameStoryOperation( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_GAMESTORY_OPERATION_RESP respMsg;
	if (!respMsg.Decode(pMsg,wSize))
		return;
	if(respMsg.m_uiRslt != RE_SUCCESS)
	{
		SendLogicInfoMessage(respMsg.m_uiRslt);
	}

	StringStreamType sStream("");
	LC_ClientGameEvent kGameEvent;
	kGameEvent.SetClientEventType(GameLogic::GameEventType(MGPT_GAMESTORY_OPERATION_RESP));
	sStream << respMsg.m_iStoryID << "#";
	sStream << respMsg.m_iOPCode << "#";
	sStream << respMsg.m_strRslt << "#";
	LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(sStream.str());
	kGameEvent.SetUserData(&kUserData);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);	
	
}

void HL_FromGameServer::_handleRespSkyArenaTime(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_GAMESTORY_RELATE_TIME_RESP respMsg;
	if (!respMsg.Decode(pMsg,wSize))
		return;

	StringStreamType sStream("");
	LC_ClientGameEvent kGameEvent;
	kGameEvent.SetClientEventType(GameLogic::GameEventType(MGPT_GAMESTORY_RELATE_TIME_RESP));
	sStream << respMsg.m_iStoryID << "#";
	sStream << respMsg.m_iRealTime << "#";
	LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(sStream.str());
	kGameEvent.SetUserData(&kUserData);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);	
}

void HL_FromGameServer::_handleRespFactionDefender(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_GAMESTORY_FACTION_DEFENDER_RESP respMsg;
	if (!respMsg.Decode(pMsg,wSize))
		return;

	StringStreamType sStream("");
	LC_ClientGameEvent kGameEvent;
	kGameEvent.SetClientEventType(GameLogic::GameEventType(MGPT_GAMESTORY_FACTION_DEFENDER_RESP));
	sStream << respMsg.m_iDefender;
	LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(sStream.str());
	kGameEvent.SetUserData(&kUserData);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);	
}
void HL_FromGameServer::_handleRltBuyVigor( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_Rlt_BuyVigor respMsg;
	if (!respMsg.Decode(pMsg,wSize))
		return;
	if(respMsg.m_uiRslt != RE_SUCCESS)
	{
		SendLogicInfoMessage(respMsg.m_uiRslt);
	}
	/*else
	{
		LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
		LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();

		LC_VIPAsset& kVipAsset = pkGamePlayer->GetVIPAsset();
		LC_VIPCmnManager* pkVIPMgr = LC_VIPCmnManager::GetSingletonPtr();
		LC_VIPProfitDataEntryPtrs& kProfitEntryPtrs = pkVIPMgr->GetVIPProfitDataEntryPtrs(LC_VIP_PROF_TP_COMMON, LC_VIP_PROF_KEY_CMN_VIGOR_BUY);
		LC_VIPProfitDataEntry* pkProfDataEntry = NULL;
		LC_VIPProfitDataEntryPtrs::iterator itProf = kProfitEntryPtrs.begin();
		for (; itProf!=kProfitEntryPtrs.end(); ++itProf)
		{
			pkProfDataEntry = *itProf;
			if (pkProfDataEntry)
			{
				break;
			}
		}
		if (NULL == pkProfDataEntry)
		{
			return;
		}

		kVipAsset.SetProfitStatValue(pkProfDataEntry->GetID(), kVipAsset.GetProfitStatValue(pkProfDataEntry->GetID())+1);

		SendLogicInfoMessage(RE_VIGOR_BUY_SUCCESS,respMsg.m_iDelta);
	}*/
}

void HL_FromGameServer::_handleRltClearPKBuff(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RespClearPK respMsg;
	if (!respMsg.Decode(pMsg,wSize))
		return;

	SendLogicInfoMessage(respMsg.m_Result);
}
void HL_FromGameServer::_handleCrossRealMapTeamOperationResp(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RESP_CrossRealmTeamOperation respMsg;
	if (!respMsg.Decode(pMsg,wSize))
		return;
	
	if (respMsg.m_iOPRslt == RE_CROSS_TEAM_OPERATION_CDING)
	{
		switch(respMsg.m_iOPCode)
		{
		case CrossRealmTeamOPType_Broad:
			SendLogicInfoMessage(RE_CROSS_TEAM_OPERATION_CDING);
			break;
		case CrossRealmTeamOPType_Score:
			SendLogicInfoMessage(RE_CROSS_TEAM_OPERATION_CDING_SCORE);
			break;
		}
	}
	else
	{
		SendLogicInfoMessage(respMsg.m_iOPRslt);
	}
	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_CLIENT_CROSSREALMAP_OPERATION);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1,respMsg.m_iOPRslt);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2,respMsg.m_iOPPara);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_3,respMsg.m_iOPCode);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handleCrossRealMapTeamInfoNotify(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_GG_NOTIFY_TeamInfoChange respMsg;
	if (!respMsg.Decode(pMsg,wSize))
		return;
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
		return;
	NW_ClientSessionManager* pkSessionManager = NW_ClientSessionManager::GetSingletonPtr();
	if (NULL == pkSessionManager)
		return;
	CF_CrossRealmMap::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_CrossRealmMap>(respMsg.m_MapTypeId);
	//if (NULL != pkEntry && pkGamePlayer->GetLevel() < pkEntry->_kLevel) //等级不够的无法看到广播
	if (NULL != pkEntry && pkGamePlayer->GetCommanderLevel() < pkEntry->_kLevel)
	{
		return;
	}

	//BUG #26526 【功能】【副本系统】组队副本吆喝不要显示服务器名
	//const PS_ServerInfo* pkServerInfo		  = pkSessionManager->GetGameServerInfo(respMsg.m_TeamInfo.m_ServerId);
	//StringType strNameInfo("");
	//if (pkServerInfo)
	//{
	//	strNameInfo = strNameInfo + "<color val = 0xfffc13>" + STDSTR2TPSTR(pkServerInfo->m_szName) + "." + STDSTR2TPSTR(respMsg.m_TeamInfo.m_TeamName) + "</color>";
	//}
	//else
	//{
	//	strNameInfo = strNameInfo + "<color val = 0xfffc13>" + STDSTR2TPSTR(respMsg.m_TeamInfo.m_ServerName) + "." + STDSTR2TPSTR(respMsg.m_TeamInfo.m_TeamName) + "</color>";
	//}
	StringType strNameInfo("");
	strNameInfo = strNameInfo + "<color val = 0xfffc13>" + STDSTR2TPSTR(respMsg.m_TeamInfo.m_TeamName) + "</color>";
	strNameInfo = "<link val = N" + Int642String(respMsg.m_TeamInfo.m_MasterId) + "#" + STDSTR2TPSTR(respMsg.m_TeamInfo.m_TeamName) + "#SERVERID" + Int2String(respMsg.m_TeamInfo.m_ServerId) + ">" + strNameInfo + "</link>";
	StringType strMapName("");
	if(NULL != pkEntry)
	{
		 strMapName = "<color val = 0xfffc13>【" + pkEntry->_kName + "】</color>";
	}
	StringType strScore("");
	combat_score_type myScore = LC_Helper::GetTotalScore(pkGamePlayer);
	if (respMsg.m_TeamInfo.m_TotalScore > 0)
	{
		if (myScore >= respMsg.m_TeamInfo.m_TotalScore)
		{
			strScore = "要求战斗力：<color val = 0xff00ff00>" + Int2String(respMsg.m_TeamInfo.m_TotalScore) + "</color>，";
		}
		else
		{
			strScore = "要求战斗力：<color val = 0xffDE1F00>" + Int2String(respMsg.m_TeamInfo.m_TotalScore) + "</color>，";
		}

	}
	StringType strJoin("");
	strJoin = "<color val = 0xff00ff00>立即加入</color>";
	strJoin = "<link val = R" + Int2String(respMsg.m_MapTypeId) + "#" + Int2String(respMsg.m_TeamInfo.m_TeamId) + "#>" + strJoin + "</link>";
	SendLogicInfoMessage(RE_CROSS_MAP_BROAD_INFO,strNameInfo.c_str(),strMapName.c_str(),strScore.c_str(),strJoin.c_str());
}

void HL_FromGameServer::_handlerSyncAchievementInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_ERROR,"MG_SyncAchievementRecord no gameplayer！");
		return;
	}
	////如果还未进入游戏，则退出
	//if (eGameStage != ST_GAME_STAGE_GAME)
	//	return;

	MG_SyncAchievementInfo msg;
	if(!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE , LOG_SWITCH_ERROR,"MG_SyncAchievementInfo解析错误！");
		return;
	}
	pkGamePlayer->GetAchievementAsset().GetDataFromProtocolStruct(msg);
}

void HL_FromGameServer::_handlerSyncFinishAchievement(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_SyncFinishAchievement msg;
	if(!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE , LOG_SWITCH_ERROR,"MG_SyncFinishAchievement解析错误！");
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE , LOG_SWITCH_ERROR,"MG_SyncFinishAchievement  no gameplayer！");
		return;
	}
	pkGamePlayer->GetAchievementAsset().SyncFinishAchievement(msg.m_nAchievementID);
}

void HL_FromGameServer::_handlerSyncAchievementRecord(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	/*MG_SyncUnfinishAchievementRecord msg;
	if(!msg.Decode(pMsg,wSize))
		return;
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
		return;*/
	//pkGamePlayer->GetachievementAsset()->SyncUnFinishAchievementRecord(msg);
	MG_SyncAchievementRecord msg;
	if(!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE , LOG_SWITCH_ERROR,"MG_SyncAchievementRecord解析错误！");
		return;
	}
		
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_ERROR,"MG_SyncAchievementRecord no gameplayer！");
		return;
	}
		
	pkGamePlayer->GetAchievementAsset().SyncAchievementRecord(msg);
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_ERROR,"MG_SyncAchievementRecord groupId = %d,value = %d",msg.m_nGroupID,msg.m_nValue);
}

void HL_FromGameServer::_handlerRltAchievementReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RLT_AchievementReward msg;
	if(!msg.Decode(pMsg, wSize))
		return;

	GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM, "MGPT_RLT_ACHIEVEMENT_REWARD m_iResult =  %d ,id = %d",msg.m_iResult,msg.m_nAchievementID);
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
		return;
	SendLogicInfoMessage(msg.m_iResult);
	if (msg.m_iResult==RE_SUCCESS)
	{
		pkGamePlayer->GetAchievementAsset().SyncFinishAchievement(msg.m_nAchievementID);

		LC_GameEvent GameEvent;
		GameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_nAchievementID);
		GameEvent.SetEventType(GLET_SYNC_NEWACHIEVEMENTRECEIVE);
		UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(GameEvent);
	}
	
}

void HL_FromGameServer::_handlerRltAchievementPointReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RLT_AchievementPointReward msg;
	if(!msg.Decode(pMsg, wSize))
		return;

	SendLogicInfoMessage(msg.m_iResult);

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
		return;
	pkGamePlayer->GetAchievementAsset().SyncAddAchievementRewardID(msg);
}

void HL_FromGameServer::_handlerRltSevenDayPointReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_GetSevenDayReward msg;
	if(!msg.Decode(pMsg, wSize))
		return;

	SendLogicInfoMessage(msg.m_nResult);

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
		return;
	pkGamePlayer->GetAchievementAsset().SyncAddSevenDayRewardID(msg);
}


void HL_FromGameServer::_handleItemInPackageCommonOperationResp(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RESP_ItemInPackageCommonOperation respMsg;
	if (!respMsg.Decode(pMsg,wSize))
		return;

	int iResult = respMsg.m_iOPRslt;
	if(respMsg.m_iOPCode == LC_ITEM_OP_CODE_ATTR_LEVEL_UP && iResult == RE_ITEM_OP_FAIL)
	{
		iResult = RE_ITEM_OP_ADD_FAIL;
	}

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_CLIENT_ITEM_IN_PACKAGE_OPERATION);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1,respMsg.m_iPackType);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2,respMsg.m_iEntryIndex);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_3,respMsg.m_iOPCode);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_4,iResult);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_5,respMsg.m_kOPPara[0]);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
	if (iResult != RE_ITEM_OP_ADD_FAIL && iResult != RE_ITEM_OP_FAIL)
	{
		SendLogicInfoMessage(iResult);
	}
}

struct FlowerLog
{
	unique_id_type uid;
	int key;
	int send;
	StringType name;
	FlowerLog():uid(0),key(0),send(0),name("")
	{}
};

AJSON(FlowerLog, (uid)(key)(send)(name));

void HL_FromGameServer::_handlerRespDalyCleanData(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_Resp_DailyClean respMsg;
	if (!respMsg.Decode(pMsg,wSize))
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
		return;

	pkGamePlayer->GetDailyCleanDataFormServer(respMsg.m_type, respMsg.m_dailyMap);

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GameLogic::GameEventType(GLET_CLIENT_DAILY_CLEAN_DATA));
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerRespCommonStatBaseOpt(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_PB<msg_define::common_stat_basic_operation_resp> resqMsg;
	if (!resqMsg.Decode(pMsg,wSize))
		return;
	if(resqMsg.m_value.result()==RE_SUCCESS)
	{

		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_CLIENT_BASSCOMMON_OPT_SUCCESS);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
	else
	{
		SendLogicInfoMessage(resqMsg.m_value.result());
	}
}

void HL_FromGameServer::_handleRltExecuteActorOperation(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RLT_ExecuteOperation resqMsg;
	if (!resqMsg.Decode(pMsg,wSize))
		return;
		
	LC_GameEvent kGameEvent;
		
	kGameEvent.SetEventType(GLET_CLIENT_ACTOR_OPERATION);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1,resqMsg.m_iResult);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2,resqMsg.m_lOperationType);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();	
	pkUIManager->HandlerGameEvent(kGameEvent);
	SendLogicInfoMessage(resqMsg.m_iResult);
}

void HL_FromGameServer::_handleEquipAdvance_Ret(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SC_EquipAdvance_Ret resqMsg;
	if (!resqMsg.Decode(pMsg,wSize))
		return;


	if (resqMsg.m_Type==2)
	{
		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
		if(!pkGamePlayer)
		{
			GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "No GamePlayer MG_SC_SynCommandEquip_Ret协议解析错误");
			return;
		}
		int idx=resqMsg.m_Index;
		LC_ClientGamePlayerCommander* pkCommander = pkGamePlayer->GetCommander();
		EquipVec& equipList = pkCommander->GetEquipList(CommandPack);
		if (equipList.size()>idx)
		{
			equipList[idx].GetFromProtocol(resqMsg.m_Data);
		}

		LC_PlayerBase::LC_PlayerEquipSuitPtrMap& kPlayerEquipSuitMap = pkGamePlayer->GetPlayerEquipSuitMap();
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

	LC_GameEvent kGameEvent;

	kGameEvent.SetEventType(GLET_CLIENT_ACTOR_OPERATION);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1,resqMsg.result);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2,LAOT_ITEM_COMPOSITION);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();	
	pkUIManager->HandlerGameEvent(kGameEvent);
	SendLogicInfoMessage(resqMsg.result);
}

void HL_FromGameServer::_handleSubEquipAdvance_Ret(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_SubEquipAdvance resqMsg;
	if (!resqMsg.Decode(pMsg,wSize))
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(!pkGamePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "No GamePlayer MG_SC_SynCommandEquip_Ret协议解析错误");
		return;
	}
	int idx=resqMsg.m_nSlotIndex;
	LC_ClientGamePlayerCommander* pkCommander = pkGamePlayer->GetCommander();
	int packType[9]={3,0,4,0,0,0,0,0,2};
	if (resqMsg.m_nSubType>8)
	{
		return;
	}
	EquipVec& equipList = pkCommander->GetEquipList(packType[resqMsg.m_nSubType]);
	if (equipList.size()>idx)
	{
		equipList[idx].GetFromProtocol(resqMsg.m_Data);
	}

	LC_PackAsset* pkPackAsset = pkGamePlayer->GetPackAsset();
	pkPackAsset->SetDirtyFlag(PACK_TYPE_EQUIP_SLOT, true);
	pkPackAsset->AddDirtyFlagRevisionID(PACK_TYPE_EQUIP_SLOT);

	pkPackAsset->SetDirtyFlag(PACK_TYPE_SUB_EQUIP_SLOT, true);
	pkPackAsset->AddDirtyFlagRevisionID(PACK_TYPE_SUB_EQUIP_SLOT);

	LC_GameEvent kGameEvent;

	kGameEvent.SetEventType(GLET_CLIENT_ACTOR_OPERATION);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1,resqMsg.m_nResult);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2,LAOT_ITEM_COMPOSITION);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();	
	pkUIManager->HandlerGameEvent(kGameEvent);
	SendLogicInfoMessage(resqMsg.m_nResult);
}

//-------------------------------------------------------------------
void HL_FromGameServer::_handleChangeCharName(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_PB<msg_define::change_name_resp> resqMsg;
	if (!resqMsg.Decode(pMsg,wSize))
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(resqMsg.m_value.result() == RE_SUCCESS &&  pkGamePlayer != NULL)
	{
		pkGamePlayer->SetPlayerCommanderName(STDSTR2TPSTR(resqMsg.m_value.char_name()));
		pkGamePlayer->UpdateDirtyFlag(DIRTY_FLAG_PROPERTY);
		LC_ClientSocietyAsset* pkAsset=pkGamePlayer->GetSocietyAsset();
		//if (pkAsset)
		{
			pkAsset->AddDirtyFlagRevisionID(ST_FRIEND_MODIFY_NAME);//改名后私聊界面刷新自己的名字
		}
	}

	SendLogicInfoMessage(resqMsg.m_value.result());
}

//-------------------------------------------------------------------
void HL_FromGameServer::_handleRltGamePlayerDailyActivityReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_Rlt_ReceiveDailyActivityReward resqMsg;
	if (!resqMsg.Decode(pMsg,wSize))
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if (resqMsg.m_nResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(resqMsg.m_nResult);
	}

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_DAILY_ACTIVITY_REWARD);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, resqMsg.m_nResult);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
	
}
//-------------------------------------------------------------------
void HL_FromGameServer::_handlerBuddyEquipReinforceLevel(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_Equip_New_Reinforce msg;
	if(!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_RLT_Equip_New_Reinforce解析错误");
		return;
	}

	if (msg.m_Result != 0)
	{
		SendLogicInfoMessage(msg.m_Result);
	}

	//给UI发消息
	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_BUDDY_EQUIP_REINFORCE_LEVEL);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_Result);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}
//-------------------------------------------------------------------
void HL_FromGameServer::_handlerBuddyEquipReinforceStar(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_EquipSlot_Star msg;
	if(!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_RLT_Equip_Star解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "No GamePlayer MG_RLT_EquipSlot_Star协议解析错误");
		return;
	}

	if (msg.m_Result != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_Result);
	}
	else
	{
		LC_ClientBuddyInstance* pkInst = pkGamePlayer->GetBuddyInstance(msg.m_iBuddyId);
		if(pkInst != NULL)
		{
			pkInst->UpdateEquipStarLevel(msg.m_iEquipSlotIndex, msg.m_iNewStarLevel);
			pkGamePlayer->OnBuddyEquipUpdated();
		}
	}

	//给UI发消息
	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_BUDDY_EQUIP_REINFORCE_STAR);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_Result);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}
//-------------------------------------------------------------------
void HL_FromGameServer::_handlerBuddyReinforceEquipSlot(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_Reinforce_EquipSlot msg;
	if(!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_RLT_Reinforce_EquipSlot解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	int iLastLevel = 0;
	if(pkGamePlayer == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "No GamePlayer MG_RLT_Reinforce_EquipSlot协议解析错误");
		return;
	}

	if (msg.m_Result != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_Result);
	}
	else
	{
		
		LC_ClientBuddyInstance* pkInst = pkGamePlayer->GetBuddyInstance(msg.m_iBuddyId);
		if(pkInst != NULL)
		{
			iLastLevel=pkInst->GetEquipIntensifyLevel(msg.m_iSlotID);
			pkInst->UpdateEquipLevel(msg.m_iSlotID, msg.m_iNewLevel);
			pkGamePlayer->OnBuddyEquipUpdated();
		}
	}

	//给UI发消息
	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_BUDDY_REINFORCE_EQUIPSLOT);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_Result);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, msg.m_iBuddyId);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_3, msg.m_iSlotID);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_4, msg.m_iReinforeType);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_5, iLastLevel);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}
//**********************************************************
void HL_FromGameServer::_handlerReduceCash(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_Rlt_ReduceCash msg;
	if(!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_Rlt_ReduceCash解析错误");
		return;
	}
	uint32_t cashNum = msg.m_iConsumeCashNum;
	ResultType errCode = msg.m_iCode;
	SendLogicInfoMessage(errCode, cashNum);
}

void HL_FromGameServer::_handlerSyncActivateStarVein(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncActivateStarVein msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncActivateStarVein解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer != NULL)
	{
		pkGamePlayer->SyncActiveStarVeinIds(msg);
	}
}

void HL_FromGameServer::_handlerSyncActivateStarVeinGoal(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncStarVeinGoalRecord msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncStarVeinGoldRecord解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(pkGamePlayer != NULL)
	{
		pkGamePlayer->SyncActiveStarVeinGoalIds(msg);
	}
}

void HL_FromGameServer::_handlerRltGetStarVeinGoalReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_GetStarVeinReward rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_RLT_GetStarVeinReward解析错误");
		return;
	}

	if(rltMsg.m_nResult != RE_SUCCESS)
		SendLogicInfoMessage(rltMsg.m_nResult);
}

void HL_FromGameServer::_handlerRltActivateStarVein(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_Rlt_ActivateStarVein msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_Rlt_ActivateStarVein解析错误");
		return;
	}

	if(msg.m_nResult != RE_SUCCESS)
		SendLogicInfoMessage(msg.m_nResult);

	//给UI发消息
	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_STARVEIN_ACTIVE_RESULT);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_nResult);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, msg.m_nStarVeinID);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);	
}


//玩具
void HL_FromGameServer::_handlerPetBaseDataSync(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SC_PetBaseDataSync msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SC_PetBaseDataSync解析错误");
		return;
	}

	LC_ClientToyPetManager::GetSingletonPtr()->GetDataFromProtocolStruct(msg);
}

void HL_FromGameServer::_handlerPetCaptureResult(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SC_PetCaptureResult rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SC_PetCaptureResult解析错误");
		return;
	}

	LC_ClientToyPetManager::GetSingletonPtr()->GetDataFromProtocolStruct(rltMsg);
	SendLogicInfoMessage(rltMsg.m_nRetCode);

	if(rltMsg.m_nRetCode == RE_SUCCESS)
	{
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_TOYPET_CAPTURE_SUCCESS);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, rltMsg.m_Data.nIndex);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, rltMsg.m_nMode);
		
		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
}

void HL_FromGameServer::_handlerPetStarUpgradeResp(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_PetStarUpgrade rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_RLT_BuddyStarUpgrade解析错误");
		return;
	}

	LC_ClientToyPetManager::GetSingletonPtr()->GetDataFromProtocolStruct(rltMsg);
	SendLogicInfoMessage(rltMsg.m_nErrorCode);
}

void HL_FromGameServer::_handlerPetDecomposeResp(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SC_PetDecompose rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SC_PetDecompose解析错误");
		return;
	}

	LC_ClientToyPetManager::GetSingletonPtr()->GetDataFromProtocolStruct(rltMsg);
	SendLogicInfoMessage(rltMsg.m_nErrorCode);
}

void HL_FromGameServer::_handlerPetStatusSync(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SC_PetStatusSync rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SC_PetStatusSync解析错误");
		return;
	}
	
	LC_ClientToyPetManager::GetSingletonPtr()->GetDataFromProtocolStruct(rltMsg);
}

void HL_FromGameServer::_handlerPetSyncSkillAsset(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SC_SyncPetSkill rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SC_SyncPetSkill解析错误");
		return;
	}

	LC_ClientToyPetManager::GetSingletonPtr()->GetSkillDataFromProtocolStruct(rltMsg);
}

void HL_FromGameServer::_handlerPetEnableResp(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SC_PetOp_EnableRep rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SC_PetOp_EnableRep解析错误");
		return;
	}
	
	LC_ClientToyPetManager::GetSingletonPtr()->GetDataFromProtocolStruct(rltMsg);
	SendLogicInfoMessage(rltMsg.m_nError);
}

void HL_FromGameServer::_handlerPetDisableResp(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SC_PetOp_DisableRep rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SC_PetOp_DisableRep解析错误");
		return;
	}

	LC_ClientToyPetManager::GetSingletonPtr()->GetDataFromProtocolStruct(rltMsg);
	SendLogicInfoMessage(rltMsg.m_nError);
}

void HL_FromGameServer::_handerSCAddTimer(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SC_AddTimer msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SC_AddTimer解析错误");
		return;
	}

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_ADD_TIMER);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_nTimerId);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handleRltGetRankSubBInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_GetRankSubBInfo rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_RLT_GetRankSubBInfo解析错误");
		return;
	}

	if(rltMsg.m_nResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(rltMsg.m_nResult);
	}
	else
	{
		LC_ClientActivityManager::GetSingletonPtr()->GetDataFromStruct(rltMsg);
	}
}

void HL_FromGameServer::_handleSyncSubInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	/*MG_SyncSubBInfo syncMsg;
	if(!syncMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncSubBInfo解析错误");
		return;
	}

	LC_ClientActivityManager::GetSingletonPtr()->GetDataFromStruct(syncMsg);*/
}

void HL_FromGameServer::_handleSyncSubbValue(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	/*MG_SyncSubBValue syncMsg;
	if(!syncMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncSubBValue解析错误");
		return;
	}

	LC_ClientActivityManager::GetSingletonPtr()->GetDataFromStruct(syncMsg);*/
}

void HL_FromGameServer::_handleSyncActivityFinish(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncActivityFinish syncMsg;
	if(!syncMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncActivityFinish解析错误");
		return;
	}

	StringStreamType sStream;
	sStream << syncMsg.m_nStoryID << "#" << syncMsg.m_nRankIndex << "#" << syncMsg.m_nNum;
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerMsgGameEvent(MGPT_SYNCACTIVITYFINISH, sStream.str());
}

void HL_FromGameServer::_handleRltSubEquipModingSpirit(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_SubEquipSlotMoldingSpirit rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_RLT_SubEquipSlotMoldingSpirit解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_RLT_SubEquipSlotMoldingSpirit解析错误 GamePlayer == NULL");
		return;
	}

	if(rltMsg.m_nResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(rltMsg.m_nResult);
		return;
	}

	pkGamePlayer->GetCommander()->UpdateSubEquip(rltMsg);

	LC_PackAsset* pkPackAsset = pkGamePlayer->GetPackAsset();
	pkPackAsset->SetDirtyFlag(PACK_TYPE_SUB_EQUIP_SLOT, true);
	pkPackAsset->AddDirtyFlagRevisionID(PACK_TYPE_SUB_EQUIP_SLOT);

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_COMMANDER_MODINGSPIRIT_SUCCESS);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, rltMsg.m_nSlotIndex);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, rltMsg.m_nSubType);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handleRltBuddyTransformOP(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_BuddyTransformOP rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_RLT_BuddyTransformOP解析错误");
		return;
	}

	if (rltMsg.m_nResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(rltMsg.m_nResult);
	}
	else
	{
		if (rltMsg.m_nOpType == BuddyTransform_Set)
		{
			LC_GameEvent kGameEvent;
			kGameEvent.SetEventType(GLET_BUDDYTRANSFORM_CHANGE);
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, rltMsg.m_nBuddyID);
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, rltMsg.m_nTransformID);

			UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
			pkUIManager->HandlerGameEvent(kGameEvent);
		}
		else if(rltMsg.m_nOpType == BuddyTransform_Active)
		{
			LC_GameEvent kGameEvent;
			kGameEvent.SetEventType(GLET_BUDDYTRANSFORM_ACTIVE);
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, rltMsg.m_nBuddyID);
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, rltMsg.m_nTransformID);

			UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
			pkUIManager->HandlerGameEvent(kGameEvent);
		}
	}
}

void HL_FromGameServer::_handleSyncDropInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncDropInfo rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncDropInfo解析错误");
		return;
	}

	if (rltMsg.m_nDropVaild)
	{
		StringStreamType sStream;
		sStream << rltMsg.m_nDropType << "#" << rltMsg.m_nHitPlayerName << "#";
		std::map<int32_t,int32_t>::iterator itor = rltMsg.m_nDropItems.begin();
		for (;itor!=rltMsg.m_nDropItems.end();++itor)
		{
			sStream << itor->first << ";" << itor->second << "|";
		}
		sStream << "#";
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"_handleSyncDropInfo， str=%s", sStream.str());
		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerMsgGameEvent(MGPT_SYNCDROPINFO, sStream.str());
	}

	if (rltMsg.m_nReduceDropType == RecoverType_BossDrop)
	{
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_SYNC_RAIDBOSS_NODROPTIMES);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
}

void HL_FromGameServer::_handleSyncRegularBossDrop(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncRegularBossDrop rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncRegularBossDrop解析错误");
		return;
	}

	StringStreamType sStream;
	sStream << rltMsg.m_nDropType << "#" << rltMsg.m_nHitPlayerName << "#";
	CF_RegularBoss::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_RegularBoss>(rltMsg.m_nCharTypeID);
	if (!pkEntry)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncRegularBossDrop信息错误，不存在的chartypeID：%d", rltMsg.m_nCharTypeID);
		return;
	}
	UT_SIMDataList& _reward = pkEntry->_kReward1;
	if (rltMsg.m_nDropType == RegularBossDrop_2)
		_reward = pkEntry->_kReward3;
	else if (rltMsg.m_nDropType == RegularBossDrop_3)
		_reward = pkEntry->_kReward2;

	Utility::UT_SIMDataList::const_iterator it = _reward.begin();
	for (; it!=_reward.end(); ++it)
	{
		sStream << it->ID() << ";" << it->ParamA() << "|";
	}
	sStream << "#";
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerMsgGameEvent(MGPT_SYNCDROPINFO, sStream.str());
}

void HL_FromGameServer::_handlerRltGetTreasurePos(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_GetTreasurePos msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_RLT_GetTreasurePos解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if (!pkGamePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"No GamePlayer MG_RLT_GetTreasurePos解析错误");
		return;
	}

	if(msg.m_nResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_nResult);
		return;
	}

	pkGamePlayer->RltGetTreasurePos(msg);

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_SYNC_TREASURE_POS);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerSyncReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncReward msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncReward解析错误");
		return;
	}

	if(msg.m_nResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_nResult);
	}

	StringStreamType sStream;
	sStream << msg.m_nResult << '#' << msg.m_nType << '#' << msg.m_nReward ;

	LC_ClientGameEvent kGameEvent;
	kGameEvent.SetClientEventType(CGET_SYNC_REWARD);
	LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(sStream.str());
	kGameEvent.SetUserData(&kUserData);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerRltGetGameRestartReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_GetGameRestartReward msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_RLT_GetGameRestartReward解析错误");
		return;
	}

	if(msg.m_nResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_nResult);
	}

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_RLT_GAMERESTART_REWARD);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_nResult);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerSyncGameRestartNotice(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncGameRestartNotice msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncGameRestartNotice解析错误");
		return;
	}

	int iId = msg.m_nNoticeID;
	CF_GameRestartNotice::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_GameRestartNotice>(iId);
	if(pkData == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncGameRestartNotice error noticeID=%d", iId);
		return;
	}
	
	StringStreamType sStream;

	StringType sItems;
	FormatDataIFs2String(pkData->_reward, sItems);
	sStream << iId << '#' << (int)msg.m_nTag << '#' << pkData->_noticeInfo << '#' << sItems;

	LC_ClientGameEvent kGameEvent;
	kGameEvent.SetClientEventType(CGET_SYNC_GAMERESTART_NOTICE);
	LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(sStream.str());
	kGameEvent.SetUserData(&kUserData);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerLotteryDrawRet(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_SC_Lottery_Draw_Ret msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SC_Lottery_Draw_Ret解析错误");
		return;
	}

	if(msg.result != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.result);
		return;
	}

	LC_ClientActivityManager::GetSingletonPtr()->UpdateLottryData(msg);

	LC_ClientGameEvent kGameEvent;
	kGameEvent.SetClientEventType(GLET_NOTIFY_DRAWCARD_REWARD);

	StringStreamType sStream;

	std::vector<KeyValue_Base>::const_iterator iter = msg.m_ItemVec.begin();
	for( ; iter!=msg.m_ItemVec.end(); ++iter)
	{
		const KeyValue_Base& info = *iter;
		sStream << info.key << ':' << info.value << ';';
	}

	LC_ClientGameEventUserData_String kUserData(sStream.str());
	kGameEvent.SetUserData(&kUserData);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerLotteryExtranRewardRet(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_SC_Lottery_ExtraReward_Ret msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SC_Lottery_ExtraReward_Ret解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SC_Lottery_ExtraReward_Ret解析错误 GamePlayer == NULL");
		return;
	}

	CF_ACTTreasureHunt::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_ACTTreasureHunt>(msg.m_Id);
	if(pkData == NULL)
	{
		return;
	}

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_RLT_CUMULATEREWARD);

	if(msg.result != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.result);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, 1);
	}
	else
	{
		LC_ClientActivityManager::GetSingletonPtr()->UpdateLottryData(msg, pkData->_iCategory == 2);

		StringStreamType sStream;
		std::vector<KeyValue_Base>::const_iterator iter = msg.m_ItemVec.begin();
		for( ; iter!=msg.m_ItemVec.end(); ++iter)
		{
			const KeyValue_Base& info = *iter;
			sStream << info.key << ':' << info.value << ';';
		}
		pkGamePlayer->UpdateDrawCardCumulateReward(sStream.str());
	}

	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, pkData->_iCategory);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerChargeProfit_RewardRet(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_SC_ChargeProfit_Reward_Ret msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SC_ChargeProfit_Reward_Ret解析错误");
		return;
	}

	if(msg.result != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.result);
		return;
	}

	LC_ClientActivityManager::GetSingletonPtr()->UpdateChargeProfitData(msg);
}

void HL_FromGameServer::_handlerSCEquipCommand_Ret(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_SC_EquipCommand_Ret msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SC_EquipCommand_Ret解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SC_EquipCommand_Ret解析错误 GamePlayer == NULL");
		return;
	}

	if(msg.result != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.result);
		return;
	}

	pkGamePlayer->GetCommander()->UpdateCommanderEquip(msg);

	LC_PackAsset* pkPackAsset = pkGamePlayer->GetPackAsset();
	pkPackAsset->SetDirtyFlag(PACK_TYPE_EQUIP_SLOT, true);
	pkPackAsset->AddDirtyFlagRevisionID(PACK_TYPE_EQUIP_SLOT);
}

void HL_FromGameServer::_handlerSCIntensifyCommandEquipRet(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_SC_IntensifyCommandEquip_Ret msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SC_IntensifyCommandEquip_Ret解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SC_IntensifyCommandEquip_Ret解析错误 GamePlayer == NULL");
		return;
	}

	int iLastLevel = 0;
	if(msg.result != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.result);
	}
	else
	{
		iLastLevel = pkGamePlayer->GetCommanderEquipReinforceLevel(msg.m_EquipIndex);
		pkGamePlayer->GetCommander()->UpdateCommanderEquip(msg);

		LC_PackAsset* pkPackAsset = pkGamePlayer->GetPackAsset();
		pkPackAsset->SetDirtyFlag(PACK_TYPE_EQUIP_SLOT, true);
		pkPackAsset->AddDirtyFlagRevisionID(PACK_TYPE_EQUIP_SLOT);
	}

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_COMMANDER_REINFORCE_RESULT);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.result);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, msg.m_EquipIndex);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_3, iLastLevel);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerSCRefineCommandEquipRet(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_SC_RefineCommandEquip_Ret msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SC_RefineCommandEquip_Ret解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SC_RefineCommandEquip_Ret解析错误 GamePlayer == NULL");
		return;
	}

	if(msg.result != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.result);
	}
	else
	{
		pkGamePlayer->GetCommander()->UpdateCommanderEquip(msg);

		LC_PackAsset* pkPackAsset = pkGamePlayer->GetPackAsset();
		pkPackAsset->SetDirtyFlag(PACK_TYPE_EQUIP_SLOT, true);
		pkPackAsset->AddDirtyFlagRevisionID(PACK_TYPE_EQUIP_SLOT);
	}

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_COMMANDER_REFINE_RESULT);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.result);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, msg.m_EquipIndex);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerSCSoulCommandEquipRet(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_SC_SoulCommandEquip_Ret msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SC_SoulCommandEquip_Ret解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SC_SoulCommandEquip_Ret解析错误 GamePlayer == NULL");
		return;
	}

	if(msg.result != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.result);
	}
	else
	{
		pkGamePlayer->GetCommander()->UpdateCommanderEquip(msg);

		LC_PackAsset* pkPackAsset = pkGamePlayer->GetPackAsset();
		pkPackAsset->SetDirtyFlag(PACK_TYPE_EQUIP_SLOT, true);
		pkPackAsset->AddDirtyFlagRevisionID(PACK_TYPE_EQUIP_SLOT);
	}

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_COMMANDER_MODINGSPIRIT_SUCCESS);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_EquipIndex); 
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, -1);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerRltEquipSubItem(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_Rlt_EquipSubItem msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_Rlt_EquipSubItem解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_Rlt_EquipSubItem解析错误 GamePlayer == NULL");
		return;
	}

	if(msg.m_nResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_nResult);
		return;
	}

	pkGamePlayer->GetCommander()->UpdateSubEquip(msg);

	LC_PackAsset* pkPackAsset = pkGamePlayer->GetPackAsset();
	pkPackAsset->SetDirtyFlag(PACK_TYPE_SUB_EQUIP_SLOT, true);
	pkPackAsset->AddDirtyFlagRevisionID(PACK_TYPE_SUB_EQUIP_SLOT);
}

void HL_FromGameServer::_handlerRltUnEquipSubItem(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_Rlt_UnEquipSubItem msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_Rlt_UnEquipSubItem解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_Rlt_UnEquipSubItem解析错误 GamePlayer == NULL");
		return;
	}

	if(msg.m_nResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_nResult);
		return;
	}

	pkGamePlayer->GetCommander()->UpdateSubEquip(msg);

	LC_PackAsset* pkPackAsset = pkGamePlayer->GetPackAsset();
	pkPackAsset->SetDirtyFlag(PACK_TYPE_SUB_EQUIP_SLOT, true);
	pkPackAsset->AddDirtyFlagRevisionID(PACK_TYPE_SUB_EQUIP_SLOT);
}

void HL_FromGameServer::_handlerRltAddNoviceTutorial(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RLT_AddNoviceTutorial msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_RLT_AddNoviceTutorial解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_RLT_AddNoviceTutorial解析错误 GamePlayer == NULL");
		return;
	}

	if(msg.m_nResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_nResult);
		return;
	}
	int tutorialId=msg.m_nNoviceTutorialID; 
	pkGamePlayer->SetTutorialProgress(tutorialId); 
}

void HL_FromGameServer::_handlerRltGetNoviceTutorialReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RLT_GetNoviceTutorialReward msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_RLT_GetNoviceTutorialReward解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_RLT_GetNoviceTutorialReward解析错误 GamePlayer == NULL");
		return;
	}

	if(msg.m_nResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_nResult);
	}
	else {

		int getRewardId=msg.m_nNoviceTutorialID; 
		pkGamePlayer->AddRewardRecord(getRewardId);
	}
	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_RLT_GETTUTORIALREWARD);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_nResult); 

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerSyncNoviceTutorialInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_SyncNoviceTutorialInfo msg ;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncNoviceTutorialInfo解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncNoviceTutorialInfo解析错误 GamePlayer == NULL");
		return;
	}
	pkGamePlayer->SyncNoviceTutorialInfo(msg.m_vGetRewardNoviceTutorial,msg.m_vNoviceTutorialRecord);
}

void HL_FromGameServer::_handlerRltGetSkyArenaBattleInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_Resp_SkyArenaBattlePlays msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_Resp_SkyArenaBattlePlays解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_Resp_SkyArenaBattlePlays解析错误 GamePlayer == NULL");
		return;
	}

	if(msg.m_Result != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_Result);
		return;
	}

	pkGamePlayer->SetSkyArenaPoint(msg.m_nMyScore);
	pkGamePlayer->SetSkyArenaHonorPoint(msg.n_HonorValue);
	pkGamePlayer->GetArenaBattleChallengeInfoAsset()->UpdateShortInfo(msg.m_battlePlays);

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_RESP_SKYARENABATTLEPLAYS);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_nBattleTimes);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, msg.m_nRefreshTimes);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_3, msg.m_nChangeScore);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_4, msg.m_nIsNewSeason);
	
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerResp_SkyArenaCheckBattleStage(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_Resp_SkyArenaCheckBattleStage msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_Resp_SkyArenaCheckBattleStage解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_Resp_SkyArenaCheckBattleStage解析错误 GamePlayer == NULL");
		return;
	}

	//检查成功则不刷新列表
	if(msg.m_Result != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_Result);
		pkGamePlayer->GetArenaBattleChallengeInfoAsset()->UpdateShortInfo(msg.m_battlePlays);
	}
	
	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_RESP_SKYARENA_CHECKBATTLESTAGE);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_Result);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerNotifySkyArenaTimeData(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_NOTIFY_SkyArenaTimeData msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_NOTIFY_SkyArenaTimeData解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_NOTIFY_SkyArenaTimeData解析错误 GamePlayer == NULL");
		return;
	}

	int iLastSeason = pkGamePlayer->GetSkyArenaSeason();

	const RankTimeData& timeData = msg.m_timeData;
	pkGamePlayer->SetGroupID(timeData.nGroupId);
	pkGamePlayer->SetSkyArenaSeason(timeData.nSeasonId);
	pkGamePlayer->SetSkyArenaCloseTime(timeData.nCloseTime);
	pkGamePlayer->SetSkyArenaNxtTime(timeData.nNextTime);

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_NOTIFY_SKYARENA_TIMEDATA);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, iLastSeason);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerNotifySkyArenaGetHonor(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_Notify_SkyArenaGetHonor msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_Notify_SkyArenaGetHonor解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_Notify_SkyArenaGetHonor解析错误 GamePlayer == NULL");
		return;
	}

	pkGamePlayer->SetSkyArenaHonorPoint(msg.nAddValue);

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_NOTIFY_SKYARENA_GETHONOR);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}




void HL_FromGameServer::_handlerResp_SkyArenaGradeAward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize) {
	MG_Resp_SkyArenaGradeAward  msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_Resp_SkyArenaGradeAward解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_Resp_SkyArenaGradeAward解析错误 GamePlayer == NULL");
		return;
	}

	if(msg.m_nResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_nResult);
	}
	else {
		int info=msg.m_nGradeBits ;
		pkGamePlayer->SetSkyArenaRankRewardAchievedId(info);
	}
	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_RESP_GETSKYARENA_GRADEAWARD);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_nResult); 
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerResp_SkyArenaMyScore(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize) {
	MG_Resp_SkyArenaMyScore  msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_Resp_SkyArenaMyScore解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_Resp_SkyArenaMyScore解析错误 GamePlayer == NULL");
		return;
	}
		pkGamePlayer->SetSkyArenaRankRewardAchievedId(msg.m_nGradeBits) ; 
		pkGamePlayer->SetSkyArenaPoint(msg.m_nMyScore) ; 

		pkGamePlayer->SetSkyArenaMaxPoint(msg.m_nMyTopScore); 
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_RESP_REFRESH_MAXRANK);
		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
}

/////////////////////////////////
void HL_FromGameServer::_handler_SkyArenaChallengeResult(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_CS_SkyArena_Challenge_Result msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_CS_SkyArena_Challenge_Resul解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_Resp_SkyArenaGradeAward解析错误 GamePlayer == NULL");
		return;
	}

	if(msg.m_iResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_iResult);
		if (msg.m_iResult == RE_SKY_REWARD_TIMEOUT_ERROR)
			pkGamePlayer->RequestLeaveArena(PlayType_Sky, QSMOT_None);
		return;
	}
	
	pkGamePlayer->SetSkyArenaSettleReward(msg.m_soloAward);
	pkGamePlayer->SetSkyArenaSettleReportList(msg.m_ReportList);

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_MGPT_CHALLEBGERESULT);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_sucess);			//结果0失败1成功
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, msg.m_RankIndex);		//当前排名
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_3, msg.m_ChangeIndex);	//排名变化
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_4, msg.m_Score);			//当前积分
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_5, msg.m_changeScore);	//积分变化
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_6, LC_Helper::EncodeScore(msg.m_leftWinNum, msg.m_rightWinNum));		//比分 高32左 低32右
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_7, msg.m_nSkyHonorErrorCode);	//代币已满的错误码

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

///////////////////////////////////////////////

//巅峰竞技场时间回复
void HL_FromGameServer::_handlerRespSummitTimeData(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize) {
	MG_NOTIFY_SummitArenaTimeData  msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_NOTIFY_SummitArenaTimeData解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_NOTIFY_SummitArenaTimeData解析错误 GamePlayer == NULL");
		return;
	}
	pkGamePlayer->GetSummitArenaTimeStamp(msg.m_timeData) ;  

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_SYNC_SUMMITTIMEDATA);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

//得到界面信息
void HL_FromGameServer::_handlerRespSummitArenaInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize){

	MG_Resp_SummitArenaInfo  msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_Resp_SummitArenaInfo解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_Resp_SummitArenaInfo解析错误 GamePlayer == NULL");
		return;
	}
	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_RESP_GETSUMMITARENA_INFO);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.nLastRankid); 
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, msg.nMyRankid); 
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}


//巅峰竞技场小组战况 
void HL_FromGameServer::_handlerRespSummitArenaGroupStage(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize) {
	MG_Resp_SummitArenaGroupStage  msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_Resp_SummitArenaGroupStage解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_Resp_SummitArenaGroupStage解析错误 GamePlayer == NULL");
		return;
	}
	pkGamePlayer->GetArenaBattleChallengeInfoAsset()->UpdateDetailInfo(msg); 
	if(msg.m_Result != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_Result);
	}
	else {
		pkGamePlayer->SetAllGroupData(msg.groupDatas);
		pkGamePlayer->GetSummitArenaTime(msg.m_timeData); 
	}
	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_RESP_GETSUMMITARENA_GROUPSTAGE);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_Result); 
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, msg.nTotalGroupId); 
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_3, msg.nBeginGroupId); 
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_4, msg.nEndGroupId); 
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

//本组战斗 
void HL_FromGameServer::_handlerRespSummitArenaMyGroupStage(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize) {

	MG_Resp_SummitArenaMyGroupStage  msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_Resp_SummitArenaMyGroupStage解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_Resp_SummitArenaMyGroupStage解析错误 GamePlayer == NULL");
		return;
	}
	pkGamePlayer->GetArenaBattleChallengeInfoAsset()->UpdateDetailInfo(msg);
	if(msg.m_Result != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_Result);
	}
	else {
		pkGamePlayer->SetMyGroupData(msg.groupDatas) ;
		pkGamePlayer->GetSummitArenaTime(msg.m_timeData); 
	}
	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_RESP_GETSUMMITARENA_MYGROUPSTAGE);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_Result); 
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);

}

//淘汰赛 巅峰竞技场
void HL_FromGameServer::_handlerRespSummitArenaAgainstList(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize) {
	MG_Resp_SummitArenaAgainstList  msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_Resp_SummitArenaAgainstList解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_Resp_SummitArenaAgainstList解析错误 GamePlayer == NULL");
		return;
	}
	pkGamePlayer->GetArenaBattleChallengeInfoAsset()->UpdateDetailInfo(msg);
	if(msg.m_Result != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_Result);
	}
	else {
		if(msg.m_AgainstLists.size()<=0) {
	        GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"请求淘汰赛数据,返回数据为空") ;
			return ; 
		}
		else if(msg.m_AgainstLists.size()>7) {
			GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"请求淘汰赛数据,返回数据大于最大7组,,解析错误") ;
			return; 
		}
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"请求淘汰赛数据,数据长度为 size=%d",msg.m_AgainstLists.size()) ;
		pkGamePlayer->SetAgainstGroup(msg.m_AgainstLists,msg.nGroupId); 
		pkGamePlayer->GetSummitArenaTime(msg.m_timeData); 
	}
	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_RESP_GETSUMMITARENA_AGAINST);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_Result); 
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, msg.nZoneId); 
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_3, msg.nGroupId); 
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
} 

//巅峰红点 
void HL_FromGameServer::_handlerRespSummitRedPoint(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize) {
	MG_NOTIFY_SummitArenaRedPoint  msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_NOTIFY_SummitArenaRedPoint解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_NOTIFY_SummitArenaRedPoint解析错误 GamePlayer == NULL");
		return;
	}

		pkGamePlayer->SetCanEnterSummitArena(msg.nRedPoint); 
}

//参赛列表
void HL_FromGameServer::_handerRespSummitArenaGroupStageEntryList(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize) {

	MG_Resp_SummitArenaGroupStageEntryList msg ; 
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_Resp_SummitArenaGroupStageEntryList解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_Resp_SummitArenaGroupStageEntryList解析错误 GamePlayer == NULL");
		return;
	}
	pkGamePlayer->GetArenaBattleChallengeInfoAsset()->UpdateDetailInfo(msg) ;
	pkGamePlayer->SetEntryList(msg.m_EntryList); 
	pkGamePlayer->GetSummitArenaTime(msg.m_timeData); 
	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_RESP_GETSUMMITARENA_ENTRYLIST);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1,msg.m_EntryList.nGroupId) ; 
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2,msg.m_EntryList.groupdata.size());
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_3,msg.m_bEnd);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}



void HL_FromGameServer::_handlerRespSummitTopType(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize) {
	MG_Resp_SummitArenaMyTopHonorMsg  msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_NOTIFY_SummitArenaRedPoint解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_NOTIFY_SummitArenaRedPoint解析错误 GamePlayer == NULL");
		return;
	}
	if(msg.m_Result != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_Result);
		return ; 
	}
	pkGamePlayer->SetMyTopType(msg.nHonorTopId); 
}


void HL_FromGameServer::_handlerSyncUseItemUpgradeSub(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_SyncUseItemUpgradeSub msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncUseItemUpgradeSub解析错误");
		return;
	}

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_SYNC_USEITEM_UPGRADESUB);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_nSubType);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, msg.m_nSubCurrentStage);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_3, msg.m_nPreCombatScore);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_4, msg.m_nCurCombatScore);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

//极品白拿 
void HL_FromGameServer::_handlerGetBuyNoPayItem(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize) {
	MG_RLT_BuyNonpareilItem msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_RLT_BuyNonpareilItem解析错误");
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_RLT_BuyNonpareilItem解析错误 GamePlayer == NULL");
		return;
	}

	if(msg.m_nResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_nResult);
	}
	else {
		pkGamePlayer->UpdateBuyNoPayList(msg.m_nID,0) ;       
}
	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_GET_BUYNOPAY_ITEM);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_nResult);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, msg.m_nID);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}
void HL_FromGameServer::_handlderSyncBuyNoPayReturnInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize){
	MG_SyncNonpareilReturnInfo msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncNonpareilReturnInfo解析错误");
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_SyncNonpareilReturnInfo解析错误 GamePlayer == NULL");
		return;
	}
	pkGamePlayer->SetBuyNoPayList(msg.m_NonpareilReturnInfo) ; 
}

void HL_FromGameServer::_handlderGetBuyNoPayReturn(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize) {
	MG_RLT_GetNonpareilReturn msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_RLT_GetNonpareilReturn解析错误");
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_RLT_GetNonpareilReturn解析错误 GamePlayer == NULL");
		return;
	}
	if(msg.m_nResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_nResult);
	}
	else {
		pkGamePlayer->UpdateBuyNoPayReturn(msg.m_nID,1); 
	}
	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_GET_BUYNOPAY_RETURN);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_nResult);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, msg.m_nID);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}


//篝火回复剩余次数
void HL_FromGameServer::_handlerBonFirePartyLeftBuyItemTimes(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize) {
	MG_Resp_BonFireParty_LeftBuyItemTimes msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_Resp_BonFireParty_LeftBuyItemTimes解析错误");
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_Resp_BonFireParty_LeftBuyItemTimes解析错误 GamePlayer == NULL");
		return;
	}
	if(msg.m_nResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_nResult);
	}
	else {
		pkGamePlayer->SetLeftTimes(msg.m_nLeftTimes); 
		pkGamePlayer->SetRate(msg.m_nRate); 
	}
	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_GET_BONFIRE_PARTY_LEFTTIME);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_nResult);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, msg.m_nLeftTimes);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_3, msg.m_nRate);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

//篝火购买
void HL_FromGameServer::_handlerBonFirePartyBuyInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize) {
	MG_Resp_BonFireParty_BuyItem msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_Resp_BonFireParty_BuyItem解析错误");
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_Resp_BonFireParty_BuyItem解析错误 GamePlayer == NULL");
		return;
	}
	if(msg.m_nResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_nResult);
	}
	else {
		pkGamePlayer->SetBuyTimes(msg.m_nBuyCount);
		pkGamePlayer->SetRate(msg.m_nRate); 
	}
	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_GET_BONFIRE_PARTY_BUYINFO);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_nResult);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, msg.m_nBuyCount);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_3, msg.m_nRate);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}


//巅峰竞技场是否发送邀请函
void HL_FromGameServer::_handlerRespSummitInvitationValid(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize) {
	MG_NOTIFY_ActivateSummitArena  msg;
	if(!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_NOTIFY_ActivateSummitArena解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_NOTIFY_ActivateSummitArena解析错误 GamePlayer == NULL");
		return;
	}
	pkGamePlayer->GetSummitArenaInvitationValid(msg.m_nType,msg.m_Vaild) ;  

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_SYNC_SUMMITARENAINVITATION);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}