
#include "ClientPCH.h"
#include "ClientScriptPCH.h"
#include "LM_LocaleManager.h"
#include "AS_ClientSoundManager.h"
#include "CA_CameraManager.h"
#include "IM_Manager.h"
#include "IP_InputManager.h"
#include "LC_ClientPlaceHolder.h"
#include "LC_PlaceHolderManagerBase.h"
#include "LC_ClientLogicManager.h"
#include "LC_ClientWorldManager.h"
#include "LC_ClientNetPlayerManager.h"
#include "LC_ClientNPCManager.h"
#include "LC_ClientPlaceHolderManager.h"
#include "LC_ClientTask.h"
#include "LC_ClientTaskManager.h"
#include "LC_ClientMap.h"
#include "LC_ClientWorldManager.h"
#include "LC_ClientLogicManager.h"
#include "LC_MiniMap.h"
#include "LC_MapManager.h"
#include "LC_NPCShopManager.h"
#include "LC_NPCTransferManager.h"
#include "LC_ClientActorOperationManager.h"
//#include "LC_ClientExchangeManager.h"
#include "LC_ClientMapTranscriptionManager.h"
#include "LC_ConversationManager.h"
#include "MG_MessageCenter.h"
#include "MG_MessageBase.h"
#include "NW_ClientNetManager.h"
#include "NW_ClientSessionManager.h"
#include "NW_Game2ClientSession.h"
#include "NW_Helper.h"
#include "NW_Login2ClientSession.h"
#include "PF_PathFinder.h"
#include "RG_RenderManager.h"
#include "SC_ClientSkillInterface.h"
#include "SM_GameStageMachine.h"
#include "LC_PlayerFlopManager.h"
#include "UI_UIManager.h"
#include "UI_AvatarManager.h"
#include "UT_LogicInfoMessage.h"
#include "UT_EngineMath.h"
#include "MTF_MemFileStream.h"
#include "SK_ClientSkillEventManager.h"
#include "UT_FileManagerBase.h"
#include "CL_CUIBubble.h"
#include "CF_ShopList.h"
#include "CF_CrossRealmMap.h"
#include "CF_ForcesBattle.h"
#include "SM_WalkTransferManager.h"
#include "SM_ClientGamePlayerController.h"
#include "LC_ClientGuildManager.h"
#include "LC_ClientNewBeeManager.h"
#include "CA_CameraPathAnimation.h"
#include "LC_ClientShopAsset.h"
#include "LC_Helper.h"
#include "UT_ClientGlobalSettings.h"
#include "UT_Pick_Manager.h"
#include "IG_MaterialManager.h"

#ifndef _SHIPPING
#include "DebugFx.h"
#endif

#include "ClientApp.h"
#include "SK_SP.h"
#include "CF_ItemList.h"
#include "CF_TransferArea.h"
#include "CF_WorldObjectDesc.h"
#include "CF_SpawnPointList.h"
#include "CF_SkillLevelUp.h"
#include "CF_WorldMapList.h"
#include "CF_ItemUIAssembleUseFunc.h"
#include "CF_CreateCharParam.h"
#include "CF_Helper.h"
#include "CF_OpenLevel.h"
#include "CF_SocketItemFunc.h"
#include "CF_CutScenes.h"
#include "CF_RankReward.h"
#include "CF_CharType.h"
#include "CF_VIP.h"
#include "CF_DailyLiveness.h"
#include "CF_NPCFunctionType.h"
#include "CF_MapCategory.h"
#include "CF_GuildWarCatapult.h"
#include "ClientTextDefine.h"
#include "CF_ACTShopTimeLimited.h"
#include "CF_TimeLimitedActivity.h"
#include "CF_ItemEffect.h"
#include "CF_ItemEffectFunc.h"
#include "CF_ItemGen.h"
#include "CF_ItemGenEffect.h"
#include "CF_OpenBag.h"
#include "CF_WarehouseCost.h"
#include "CF_PowerUp.h"
#include "CF_PowerUpFunc.h"
#include "CF_TreasurePanel.h"
#include "CF_TreasureChests.h"
#include "CF_Mental.h"
#include "CF_SpecialEquipAvatar.h"
#include "CF_GuildGood.h"
#include "CF_ItemDisassembleUseFunc.h"
#include "CF_VIPProfit.h"
#include "CF_GameNotice.h"
#include "CF_Subordinate.h"
#include "CF_GuildDonate.h"
#include "CF_QQGirls.h"
#include "CF_GuildWarActivity.h"
#include "CF_ActivitySchedule.h"
#include "CF_SecretTreasureReward.h"
#include "CF_SecretTreasureTimes.h"
#include "CF_FashionSuit.h"
#include "LC_SkillBookAsset.h"
#include "CF_YunYingOpenUrlConfig.h"
#include "CF_EffectTrigger.h"
#include "CF_ItemMelt.h"
#include "CF_StarsRune.h"
#include "CF_ACTMonopoly.h"
#include "CF_ACTMonopolyA.h"
#include "CF_SkillSpecialAttrib.h"
#include "CF_Buddy.h"
#include "CF_EquipStar.h"
#include "CF_BuddyUpgrade.h"
#include "CF_BuddyUnlock.h"
#include "CF_StarVeinList.h"
#include "CF_StarVeinGoalList.h"
#include "CF_SkillBookInitAttribute.h"
#include "CF_Pet.h"
#include "CF_SevenDaysReward.h"
#include "CF_SkillBookUnlock.h"
#include "CF_EquipSlotReinforce.h"
#include "CF_GuildLevel.h"
#include "CF_TaskChapters.h"
#include "CF_RuneList.h"
#include "CF_RuneLevelUp.h"
#include "CF_RuneStar.h"
#include "CF_BiographyTask.h"
#include "CF_TaskReachRewards.h"
#include "CF_EquipSlotStar.h"
#include "CF_ShopGroupOpenSetting.h"
#include "CF_VIPProfitDescribe.h"
#include "CF_HookMaps.h"
#include "CF_TestDatabase.h"
#include "CF_PromoteCombat.h"
#include "CF_SoulConnect.h"
#include "CF_EquipUpgrade.h"
#include "CF_EquipCommanderJinglian.h"
#include "CF_EquipCommanderSlotReinforceSuit.h"
#include "CF_EquipCommanderJinglianSuit.h"
#include "CF_BestGuildBossRankRewards.h"
#include "CF_BestGuildBossInfo.h"
#include "CF_BestGuildRobReward.h"
#include "CF_RegularBoss.h"
#include "CF_EquipCommanderSlotMoldingSpirit.h"
#include "CF_EquipCommanderSlotMoldingSpiritSuit.h"
#include "CF_RankOpen.h"
#include "CF_GloryExpedition.h"
#include "GlobalSettings.h"
#include "CF_PromoteLevel.h"
#include "CF_ShotActivityLimitRankRewards.h"
#include "CF_GloryRewardLink.h"
#include "CF_EquipCommanderSuitCollection.h"
#include "CF_ComeBack.h"
#include "CF_ACTTreasureHuntStageReward.h"
#include "CF_BuddyTransform.h"
#include "CF_AchievementTypeBig.h"
#include "CF_AchievementTypeSmall.h"
#include "CF_TreasureItem.h"
#include "CF_GuildGift.h"
#include "CF_GuildCrystal.h"
#include "CF_SpecialBuy.h"
#include "CF_GuildTowerRankReward.h"
#include "CF_ElementDamage.h"
#include "CF_BuddyThemeTaskList.h"
#include "CF_BuddyThemeTaskType.h"
#include "CF_UIOpenLevel.h"
#include "CF_Scheme.h"

#include "CF_BuddyThemeMain.h"
#include "CF_BuddyThemeSecondary.h"
#include "CF_ACTShopA.h"
#include "CF_BuddyThemeBossRankReward.h"
#include "CF_BuddyThemeBossType.h"
#include "CF_Charge.h"
#include "CF_MailInfo.h"
#include "CF_TutorialRewards.h"
#include "CF_ACTChargeProfitType.h"
#include "CF_SubSystemInfo.h"
#include "CF_SkyArenaRankReward.h"
#include "CF_SkyArenaRank.h"
#include "CF_SpecialTask.h"
#include "CF_ExportSoloDuleRobotInfo.h"
#include "CF_Guess.h"
#include "CF_SummitArenaMatchName.h"
#include "CF_SummitArenaRankRewards.h"
#include "CF_YiZhongJiangLinRankReward.h"
#include "CF_YiZhongJiangLinBossInformation.h"
#include "CF_HolidayActivityMain.h"
#include "CF_HolidayActivitySecondary.h"
#include "CF_HolidayActivityTaskType.h"
#include "CF_HolidayActivityTaskList.h"
#include "CF_HolidayActivity_PinTuUnlock.h"
#include "CF_HolidayActivity_PinTuReward.h"
#include "CF_HolidayActivity_ChargeContinuous.h"
#include "SystemDefine.h"

using namespace Base;
using namespace CSVFile;
using namespace Data;
using namespace GameLogic;
using namespace NetWork;
using namespace Protocol;
using namespace Script;
using namespace Skill;
using namespace StateMachine;
using namespace Utility;
using namespace GUISys;

#include "SC_ClientSkillInterface.inl"
#include "CF_CompositionFormula.h"
#include "CF_VIPPrivilegeCard.h"
#include "LC_ClientHelp.h"
#include "md5plusplus.h"
#include <shlobj.h>
#include <shellapi.h>

//---------------------------------------------------
SC_ClientSkillInterface::SC_ClientSkillInterface()
: SC_SkillInterface()
,	m_iCount(0)
,	m_bEquip(false)
,	m_iClothesCount(0)
{
	for (int i = 0; i < 4; ++i)
	{
		m_TmpBackPackEntry[i].Reset();
	}
	m_TmpItemCompositionPackEntry.Reset();

}
//---------------------------------------------------
SC_ClientSkillInterface::~SC_ClientSkillInterface()
{
	for (int i = 0; i < 4; ++i)
	{
		m_TmpBackPackEntry[i].Reset();
	}
	m_TmpItemCompositionPackEntry.Reset();
}
//------------------------------------------------------------
float SC_ClientSkillInterface::GetGameTime()
{
	return GET_PROCESS_TIME;
}
float SC_ClientSkillInterface::GetGameDeltaTime()
{
	return UT_TimeManager::GetSingletonPtr()->GetDeltaTime();
}

StringType SC_ClientSkillInterface::GetCalendarToday(void)
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)LC_ClientLogicManager::GetSingletonPtr();
	LC_ClientGamePlayer* pkPlayer = pkLogicManger->GetOperationPlayer();
	if (NULL == pkPlayer)
		return "";

	time_t tTime = pkPlayer->GetServerTimeVal();
	tm pkTime = *localtime(&tTime);
	return _getCalendarString(1900 + pkTime.tm_year, pkTime.tm_mon + 1, pkTime.tm_mday);
}

StringType SC_ClientSkillInterface::GetTimeStamp(time_type iBeginTime, time_type iEndTime)
{	
	time_t bt = time_t(iBeginTime);
	time_t et = time_t(iEndTime);

	tm kTimeBegin = *localtime(&bt);
	tm kTimeEnd = *localtime(&et);

	kTimeBegin.tm_mon += 1;
	kTimeEnd.tm_mon += 1;

	//本地时间
	time_t t = time_t(UT_TimeManager::GetSingleton().GetCurrentSystemTime());
	tm kTime = *localtime(&t);

	StringStreamType sStream;
	if (kTimeBegin.tm_year != kTimeEnd.tm_year)
	{
		sStream << kTimeBegin.tm_year << CLIENT_TEXT_YEAR << kTimeBegin.tm_mon << CLIENT_TEXT_MONTH << kTimeBegin.tm_mday << CLIENT_TEXT_DAY << " " << setw(2) << setfill('0') << kTimeBegin.tm_hour << ":" << setw(2) << setfill('0') << kTimeBegin.tm_min;
		sStream << "-";
		sStream << kTimeEnd.tm_year << CLIENT_TEXT_YEAR << kTimeEnd.tm_mon << CLIENT_TEXT_MONTH << kTimeEnd.tm_mday << CLIENT_TEXT_DAY << " " << setw(2) << setfill('0') << kTimeEnd.tm_hour << ":" <<setw(2) << setfill('0') << kTimeEnd.tm_min;
	}
	else if (kTimeBegin.tm_mon != kTimeEnd.tm_mon 
		|| kTimeBegin.tm_mday != kTimeEnd.tm_mday)
	{
		if (kTimeBegin.tm_year != kTime.tm_year)
			sStream << kTimeBegin.tm_year << CLIENT_TEXT_YEAR;
		sStream << kTimeBegin.tm_mon << CLIENT_TEXT_MONTH << kTimeBegin.tm_mday << CLIENT_TEXT_DAY << " " << setw(2) << setfill('0') << kTimeBegin.tm_hour << ":" << setw(2) << setfill('0') << kTimeBegin.tm_min;
		sStream << "-";
		sStream << kTimeEnd.tm_mon << CLIENT_TEXT_MONTH << kTimeEnd.tm_mday << CLIENT_TEXT_DAY << " " << setw(2) << setfill('0') << kTimeEnd.tm_hour << ":" << setw(2) << setfill('0') << kTimeEnd.tm_min;
	}
	else
	{
		if (kTimeBegin.tm_year != kTime.tm_year)
			sStream << kTimeBegin.tm_year << CLIENT_TEXT_YEAR;
		sStream << kTimeBegin.tm_mon << CLIENT_TEXT_MONTH << kTimeBegin.tm_mday << CLIENT_TEXT_DAY << " " << setw(2) << setfill('0') << kTimeBegin.tm_hour << ":" << setw(2) << setfill('0') << kTimeBegin.tm_min;
		sStream << "-";
		sStream << setw(2) << setfill('0') << kTimeEnd.tm_hour << ":" <<setw(2) << setfill('0') << kTimeEnd.tm_min;
	}

	return sStream.str();
}

StringType SC_ClientSkillInterface::GetCurrentDayTime()
{
	StringStreamType sStream("");
	time_t t = time_t(UT_TimeManager::GetSingleton().GetCurrentSystemTime());
	tm kTime = *localtime(&t);
	sStream << kTime.tm_hour <<  ":" <<  kTime.tm_min << ":" << kTime.tm_sec ;
	return sStream.str();
}

StringType SC_ClientSkillInterface::GetTimeString(time_type iBeginTime, int iType)
{
	if(iBeginTime < 0)
	{
		GfxWriteLog(LOG_WARNING_FILELINE, LOG_SWITCH_WARNING,"时间错误:时间不能为负数");
		return "";
	}
	time_t bt = time_t(iBeginTime);
	tm kTimeBegin = *localtime(&bt);

	kTimeBegin.tm_mon += 1;

	StringStreamType sStream;
	if (iType == 2)
	{
		time_t t = time_t(UT_TimeManager::GetSingleton().GetCurrentSystemTime());
		tm kTime = *localtime(&t);
		if (kTime.tm_yday != kTimeBegin.tm_yday)
			sStream << kTimeBegin.tm_mon << CLIENT_TEXT_MONTH << kTimeBegin.tm_mday << CLIENT_TEXT_DAY << " " << setw(2) << setfill('0') << kTimeBegin.tm_hour << ":" << setw(2) << setfill('0') << kTimeBegin.tm_min;
		else
			sStream << setw(2) << setfill('0') << kTimeBegin.tm_hour << ":" << setw(2) << setfill('0') << kTimeBegin.tm_min;
	}
	else
	{
		sStream << kTimeBegin.tm_mon << CLIENT_TEXT_MONTH << kTimeBegin.tm_mday << CLIENT_TEXT_DAY << " " << setw(2) << setfill('0') << kTimeBegin.tm_hour << ":" << setw(2) << setfill('0') << kTimeBegin.tm_min;
		sStream << " (" << CLIENT_TEXT_WEEK << CHN_WEEKDAY(kTimeBegin.tm_wday) << ")";
	}

	return sStream.str();
}

int64_t SC_ClientSkillInterface::GetActivityScheduleDeltaTimeStamp(int iScheduleID)
{
	LC_ClientGamePlayer* pkGamePlayer = GetPlayerPtr();
	if(pkGamePlayer == NULL)
		return 0;

	LC_ClientActivityManager *pkActivityManager = LC_ClientActivityManager::GetSingletonPtr();
	return pkActivityManager->GetScheduleEndTimes(iScheduleID) - pkGamePlayer->GetServerTimeVal();
}

// return -1 开启前 0 开启中 1 结束后
int SC_ClientSkillInterface::GetActivityScheduleStat(int iScheduleID)
{
	LC_ClientGamePlayer* pkGamePlayer = GetPlayerPtr();
	if(pkGamePlayer == NULL)
		return 0;

	LC_ClientActivityManager *pkActivityManager = LC_ClientActivityManager::GetSingletonPtr();
	int64_t BeginTime		= pkActivityManager->GetScheduleBeginTimes(iScheduleID);
	int64_t EndTime			= pkActivityManager->GetScheduleEndTimes(iScheduleID);
	int64_t ServerTime		= pkGamePlayer->GetServerTimeVal();
	if(ServerTime<BeginTime)
	{
		return -1;
	}
	else if (ServerTime>EndTime)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

//------------------------------------------------------------
int SC_ClientSkillInterface::GetLoginType()
{
	NW_ClientSessionManager* pkClientSessionManager = NW_ClientSessionManager::GetSingletonPtr();
	if(pkClientSessionManager)
		return pkClientSessionManager->GetLoginType();

	return 0;
}
//------------------------------------------------------------
int SC_ClientSkillInterface::GetGameStage()
{
	NW_ClientSessionManager* pkClientSessionManager = NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup* pkSessionGroup = pkClientSessionManager->GetGameSessionGroup();
	if(NULL == pkSessionGroup)
		return -1;

	return pkSessionGroup->m_pkGameStageMachine->GetCurrentStateType();
}
//------------------------------------------------------------
LC_ClientGamePlayer* SC_ClientSkillInterface::GetPlayerPtr()
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();

	return pkGamePlayer;
}
//------------------------------------------------------------
void SC_ClientSkillInterface::ReturnToCity(GameLogic::LC_Attacker* pkScriptCaller)
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return;

	pkGamePlayer->ReturnToCity();
}
//------------------------------------------------------------
LC_ClientNetPlayer* SC_ClientSkillInterface::GetNetPlayerPtr(unique_id_type lCharID)
{
	LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
	LC_ClientNetPlayer* pkNetPlayer = (LC_ClientNetPlayer*)(pkNetPlayerManager->FindPlayer(lCharID));
	return pkNetPlayer;
}
//------------------------------------------------------------
LC_ClientNetPlayer* SC_ClientSkillInterface::GetNetPlayerPtrByPlayerID(object_id_type lPlayerID)
{
	if(!IS_PLAYER_CLASS_ID(lPlayerID))
		return NULL;

	LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
	LC_ClientNetPlayer* pkNetPlayer = (LC_ClientNetPlayer*)(pkNetPlayerManager->FindPlayer(lPlayerID));
	return pkNetPlayer;
}
//------------------------------------------------------------
int SC_ClientSkillInterface::GetNetPlayerCount()
{
	LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
	if(!pkNetPlayerManager)
		return 0;

	return pkNetPlayerManager->GetNetPlayerCount();
}
//------------------------------------------------------------
LC_ClientNetPlayer* SC_ClientSkillInterface::GetNetPlayerByIndex(int iIndex)
{
	LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
	if(!pkNetPlayerManager)
		return NULL;

	LC_ClientNetPlayer* pkNetPlayer = (LC_ClientNetPlayer*)(pkNetPlayerManager->GetNetPlayerPtrByIndex(iIndex));
	return pkNetPlayer;
}
//------------------------------------------------------------
int SC_ClientSkillInterface::GetNPCCount()
{
	LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
	if(!pkNPCManager)
		return 0;

	return pkNPCManager->GetNPCCount();
}
//------------------------------------------------------------
LC_ClientNPC* SC_ClientSkillInterface::GetNPCByIndex(int iIndex)
{
	LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
	if(!pkNPCManager)
		return NULL;

    return pkNPCManager->GetNPCPtrByIndex(iIndex);
}
//------------------------------------------------------------
LC_ClientNPC* SC_ClientSkillInterface::GetNPCPtr(object_id_type lNPCID)
{
	if(!IS_NPC_ID(lNPCID) && !IS_LOCAL_NPC_ID(lNPCID)/* && !IS_MONSTER_ID(lNPCID)*/)
		return NULL;

	LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
	LC_ClientNPC* pkNPC = static_cast<LC_ClientNPC*>(pkNPCManager->FindNPC(lNPCID));
	return pkNPC;
}
//------------------------------------------------------------
LC_ClientPlaceHolder* SC_ClientSkillInterface::GetChestPtr(object_id_type lPlaceHolderID)
{
	if(!IS_CHEST_ID(lPlaceHolderID))
		return NULL;

	LC_ClientPlaceHolderManager* pkChestManager = CLIENT_GET_PLACEHOLDER_MANAGER();
	LC_ClientPlaceHolder* pkChest = static_cast<LC_ClientPlaceHolder*>(pkChestManager->FindPlaceHolder(lPlaceHolderID));
	return pkChest;
}
//------------------------------------------------------------------------
LC_ClientTask* SC_ClientSkillInterface::GetTaskPtr(int lTaskID)
{
	LC_ClientTask* pkClientTask = NULL;
	LC_ClientTaskManager* pkTaskManager = (LC_ClientTaskManager*)LC_ClientTaskManager::GetSingletonPtr();
	if (pkTaskManager)
	{
		pkClientTask = (LC_ClientTask*)(pkTaskManager->GetTask(lTaskID));
		return pkClientTask;
	}

	return NULL;
}
int SC_ClientSkillInterface::GetTaskBuddyExpBonus(int iTaskID, int iCommanderLevel)
{
	LC_ClientTask* pkTask = GetTaskPtr(iTaskID);
	if(pkTask != NULL)
	{
		CF_EXPList::DataEntry *pkExpData = SafeGetCSVFileDataEntryPtr<CF_EXPList>(iCommanderLevel);
		CF_TaskParam::DataEntry *pkParamData = SafeGetCSVFileDataEntryPtr<CF_TaskParam>(iTaskID);
		if(pkExpData != NULL && pkParamData != NULL)
		{
			return pkTask->GetTaskBuddyBonusEXP() + pkExpData->_lBaseBuddyXP * pkParamData->_fBuddyExp;
		}
	}

	return 0;
}

int SC_ClientSkillInterface::GetTaskGoldBonus(int iTaskID, int iCommanderLevel)
{
	LC_ClientTask* pkTask = GetTaskPtr(iTaskID);
	if(pkTask != NULL)
	{
		CF_EXPList::DataEntry *pkExpData = SafeGetCSVFileDataEntryPtr<CF_EXPList>(iCommanderLevel);
		CF_TaskParam::DataEntry *pkParamData = SafeGetCSVFileDataEntryPtr<CF_TaskParam>(iTaskID);
		if(pkParamData != NULL && pkExpData != NULL)
		{
			return pkTask->GetTaskBonusUnBindGold() + pkExpData->_lBaseCash * pkParamData->_fCash;
		}
	}

	return 0;
}

int SC_ClientSkillInterface::GetTaskCommanderExpBonus(int iTaskID, int iCommanderLevel)
{
	LC_ClientTask* pkTask = GetTaskPtr(iTaskID);
	if(pkTask != NULL)
	{
		CF_EXPList::DataEntry *pkExpData = SafeGetCSVFileDataEntryPtr<CF_EXPList>(iCommanderLevel);
		CF_TaskParam::DataEntry *pkParamData = SafeGetCSVFileDataEntryPtr<CF_TaskParam>(iTaskID);
		if(pkExpData != NULL && pkParamData != NULL)
		{
			return pkTask->GetTaskBonusEXP() + pkExpData->_lBaseXP * pkParamData->_fExp;
		}
	}

	return 0;
}

int SC_ClientSkillInterface::GetTaskGuildPointBonus(int iTaskID, int iCommanderLevel)
{
	LC_ClientTask* pkTask = GetTaskPtr(iTaskID);
	if(pkTask != NULL)
	{
		CF_EXPList::DataEntry *pkExpData = SafeGetCSVFileDataEntryPtr<CF_EXPList>(iCommanderLevel);
		CF_TaskParam::DataEntry *pkParamData = SafeGetCSVFileDataEntryPtr<CF_TaskParam>(iTaskID);
		if(pkExpData != NULL && pkParamData != NULL)
		{
			return pkTask->GetTaskBonusEXP() + pkExpData->_lBaseGuild * pkParamData->_fGuild;
		}
	}

	return 0;
}

void SC_ClientSkillInterface::SetPostTransferLocation(GameLogic::LC_Attacker* pkScriptCaller)
{
	LC_ActorBase* player = (LC_ActorBase*)pkScriptCaller;
	if (player == NULL)
		return;

	UT_Vec3Int targetLocation = player->GetPostTransferLocation();
	Path::PF_PathManager* pkPathMgr = Path::PF_PathManager::GetSingletonPtr();
	targetLocation.x = UT_MathBase::ClampFloat( Int2Float(targetLocation.x) , 0.0f , GfxGetWorldConfig().CFG_WORLD_MAP_WIDTH );
	targetLocation.y = UT_MathBase::ClampFloat( Int2Float(targetLocation.y) , 0.0f , GfxGetWorldConfig().CFG_WORLD_MAP_WIDTH );
	UT_Vec3Int kNewTarget = targetLocation;
	if ( pkPathMgr )
	{
		if ( !pkPathMgr->TestCanMoveTo( targetLocation , false ))
		{
			int lSearchStep = 200;
			int lHorizontalRange;
			int lVerticalRange;
			int lMaxDistance;

			lHorizontalRange = 2000;
			lVerticalRange   = 500;
			lMaxDistance     = 500;

			kNewTarget = pkPathMgr->FindClosestMovablePoint( targetLocation , 
				lSearchStep , 100 , lHorizontalRange , lVerticalRange , lMaxDistance );
			targetLocation = (kNewTarget.x!=0 && kNewTarget.y!=0)? kNewTarget: player->GetCurrentLocation();
		}
	}
	player->SetLockedLocation(targetLocation);
}

//------------------------------------------------------------------------
int SC_ClientSkillInterface::GetNeighborPlayerCount()
{
	LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
	return pkNetPlayerManager->GetNetPlayerCount();
}
//------------------------------------------------------------------------
int SC_ClientSkillInterface::GetTrueNeighborPlayerCount()
{
	LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
	return pkNetPlayerManager->GetTrueNetPlayerCount();
}
//------------------------------------------------------------------------
LC_ClientNetPlayer* SC_ClientSkillInterface::GetNeighborPlayerPtrByIndex(int iIndex)
{
	LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
	return (LC_ClientNetPlayer*)(pkNetPlayerManager->GetNetPlayerPtrByIndex(iIndex));
}
//------------------------------------------------------------
LC_ClientNPC* SC_ClientSkillInterface::FindNPCPtrInCircleByCharType(LC_Attacker* pkCaller,int lCharType,int iRadius)
{
	UT_Circle kCircle;
	kCircle.iRadius = iRadius;
	kCircle.kCenterPoint = pkCaller->GetCurrentLocation();

	LC_MapBase* pkMap = pkCaller->GetMap();
	if(!pkMap)
	{
		GfxWriteLog(LOG_WARNING_FILELINE, LOG_SWITCH_WARNING,"脚本:试图用非法目标位置作区域锁定");
		return 0;
	}

	CF_ActorFilterFunc* pkCSVActorFilterFunc = CF_ActorFilterFunc::GetSingletonPtr();
	CF_ActorFilterFunc::DataEntry* pkData = pkCSVActorFilterFunc->GetEntryPtr(2);
	if(!pkData)
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"找不到目标过滤者");
		return 0;
	}

	//从地图返回区域内的Actor队列
	LC_ActorPtrVector* pkLockResult = pkMap->LockActorsInCircle(kCircle,pkData);
	if (!pkLockResult->empty())
	{
		LC_ActorPtrVectorIter Iter;

		TRAVP(pkLockResult,Iter)
		{
			LC_ActorBase* pkTarget = *Iter;
			if(pkTarget->GetCharType() == lCharType && IS_NPC_ID(pkTarget->GetID()))
				return (LC_ClientNPC*)pkTarget;
		}
	}

	return NULL;
}
bool SC_ClientSkillInterface::FindNPCbylID(object_id_type lID)
{
	LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
	if(NULL == pkNPCManager)
		return 0;
	return pkNPCManager->FindNPC(lID) != NULL;
}
LC_NPCBase* SC_ClientSkillInterface::FindNPCByCharType(object_id_type lCharType)
{
	LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
	if(NULL == pkNPCManager)
		return NULL;
	return pkNPCManager->FindNPCByCharType(lCharType);
}
int SC_ClientSkillInterface::FindNPCByNumberCharType(object_id_type lCharType)
{
	LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
	if(NULL == pkNPCManager)
		return 0;
	return pkNPCManager->FindNPCNumberByCharType(lCharType);
}

int SC_ClientSkillInterface::PathtoTreasure(object_id_type lCharType)
{
	LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
	if(NULL == pkNPCManager)
		return -1;

	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	LC_NPCBase* pkNpc = pkNPCManager->FindNearestNPCByCharType(lCharType, pkGamePlayer);
	if (pkNpc == NULL)
		return -1;
	
	int iTargetType = pkNpc->GetID();
	UT_Vec3Int pos = ((LC_ClientNPC*)pkNpc)->GetCurrentLocation();
	pkGamePlayer->PathToPointLocation(pos.x, pos.y, pos.z, true);
	pkGamePlayer->PendingPathAction(PPAT_COLLECT_ITEM, iTargetType, false, 0);
	return iTargetType;
}
//------------------------------------------------------------
int SC_ClientSkillInterface::PathtoTreasureToy(object_id_type id)
{
	LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
	if(NULL == pkNPCManager)
		return -1;

	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	LC_NPCBase* pkNpc = pkNPCManager->FindNPC(id);
	if (pkNpc == NULL)
		return -1;

	int iTargetType = pkNpc->GetID();
	UT_Vec3Int pos = ((LC_ClientNPC*)pkNpc)->GetCurrentLocation();
	pkGamePlayer->PathToPointLocation(pos.x, pos.y, pos.z, true);
	pkGamePlayer->PendingPathAction(PPAT_TALK_TO_NPC, pkNpc->GetCharType(), false, 0);
	return iTargetType;
}
//------------------------------------------------------------
LC_ClientTaskManager* SC_ClientSkillInterface::GetTaskMgr()
{
	return (LC_ClientTaskManager*)LC_ClientTaskManager::GetSingletonPtr();
}

int SC_ClientSkillInterface::GetTaskCurChapter()
{
	LC_ClientTaskManager* pkManager = (LC_ClientTaskManager*)LC_ClientTaskManager::GetSingletonPtr();
	return pkManager->GetCurChapter();
}

StringType SC_ClientSkillInterface::GetAllChapterInfo()
{
	LC_ClientTaskManager* pkManager = (LC_ClientTaskManager*)LC_ClientTaskManager::GetSingletonPtr();
	return pkManager->GetAllChapterInfo();
}

StringType SC_ClientSkillInterface::GetAllTaskReachInfo()
{
	LC_ClientTaskManager* pkManager = (LC_ClientTaskManager*)LC_ClientTaskManager::GetSingletonPtr();
	return pkManager->GetAllReachInfo();
}

StringType SC_ClientSkillInterface::GetTaskReachName(int iTaskCount)
{
	StringType sResult;
	CF_TaskReachRewards::DataEntryMapExternIterator Iter = CF_TaskReachRewards::GetSingletonPtr()->GetIterator();
	for(; Iter.HasMoreElements(); Iter.MoveNext())
	{
		const CF_TaskReachRewards::DataEntry *pkData = Iter.PeekNextValuePtr();
		if(iTaskCount < pkData->_iTaskCounts)
			return sResult;

		sResult = pkData->_sName;
	}
	
	return sResult;
}

StringType SC_ClientSkillInterface::GetChapterTaskInfo(int iChapterID)
{
	LC_ClientTaskManager* pkManager = (LC_ClientTaskManager*)LC_ClientTaskManager::GetSingletonPtr();
	return pkManager->GetChapterTaskInfo(iChapterID);
}

StringType SC_ClientSkillInterface::GetAllTaskRankInfo()
{
	LC_ClientTaskManager* pkManager = (LC_ClientTaskManager*)LC_ClientTaskManager::GetSingletonPtr();
	return pkManager->GetAllRankInfo();

}

StringType SC_ClientSkillInterface::GetTaskRankInfo(int iRank)
{
	LC_ClientTaskManager* pkManager = (LC_ClientTaskManager*)LC_ClientTaskManager::GetSingletonPtr();
	return pkManager->GetRankInfo(iRank);
}

int SC_ClientSkillInterface::GetCurTaskRank()
{
	LC_ClientTaskManager* pkManager = (LC_ClientTaskManager*)LC_ClientTaskManager::GetSingletonPtr();
	return pkManager->GetCurTaskRank();
}

int64_t SC_ClientSkillInterface::GetTaskChapterRewardEndTime()
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(pkGamePlayer == NULL)
		return 0;

	LC_ClientActivityManager *pkActivityManager = LC_ClientActivityManager::GetSingletonPtr();
	return pkActivityManager->GetScheduleEndTimes(GetGlobalSetting.ScheduleID) - pkGamePlayer->GetServerTimeVal();
}

StringType SC_ClientSkillInterface::GetReachTaskChapterInfo()
{
	CF_TaskList::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_TaskList>(GetGlobalSetting.nReachTaskID);
	if(pkData == NULL)
		return "";

	StringStreamType sStream;
	sStream << pkData->_lParentId << '#' << pkData->_lGroupID;
	return sStream.str();
}

StringType SC_ClientSkillInterface::GetTaskReward(int iTaskId)
{
	CF_TaskList::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_TaskList>(iTaskId);
	if (pkData == NULL)
	{
		return "";
	}
	StringStreamType sStream;
	sStream << pkData->_sRewardItemsA ;
	return sStream.str();
}

//------------------------------------------------------------
object_id_type SC_ClientSkillInterface::GetWatchedTargetID()
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return 0;

	return pkGamePlayer->GetWatchTargetID();
}
//------------------------------------------------------------
object_id_type SC_ClientSkillInterface::GetPreLockedTargetID()
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return 0;

	return pkGamePlayer->GetPreLockedTargetID();
}
//---------------------------------------------------
void SC_ClientSkillInterface::SetLockedTargetID(object_id_type lID)
{
	//这个函数UI用，所以两个目标都要设置
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return ;

	pkGamePlayer->SetLockedTargetID(lID);
	pkGamePlayer->SetPreLockedTargetID(lID);
}
//---------------------------------------------------
void SC_ClientSkillInterface::FreezeLockTargetID(bool flag)
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return ;

	pkGamePlayer->FreezeLocked(flag);
}
//---------------------------------------------------
bool SC_ClientSkillInterface::IsFreezeLockTargetID()
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return false;

	return pkGamePlayer->IsFreezeLocked();	
}
//---------------------------------------------------
void SC_ClientSkillInterface::CleanChangeLockedFlag()
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return ;

	pkGamePlayer->ClearChangeLockedFlag();
}
bool SC_ClientSkillInterface::GetIsWildBoss( int lCharTypeID )
{
	CF_CharType* pkCSVCharType = CF_CharType::GetSingletonPtr();
	const CF_CharType::DataEntry* pkData = pkCSVCharType->GetEntryPtr(lCharTypeID);
	if(!pkData)
		return false;

	return pkData->_bIsWildBoss;
}
bool SC_ClientSkillInterface::GetCharShowRank( int lCharTypeID )
{
	CF_CharType* pkCSVCharType = CF_CharType::GetSingletonPtr();
	const CF_CharType::DataEntry* pkData = pkCSVCharType->GetEntryPtr(lCharTypeID);
	if(!pkData)
		return false;

	return pkData->_bIsShowRank;
	//	return pkData->_kIconPath.c_str();
}
int SC_ClientSkillInterface::GetElementDamageType( int lCharTypeID )
{
	CF_CharType* pkCSVCharType = CF_CharType::GetSingletonPtr();
	const CF_CharType::DataEntry* pkData = pkCSVCharType->GetEntryPtr(lCharTypeID);
	if(!pkData)
		return -1;

	return pkData->_elementdamagetype;
}
//---------------------------------------------------
StringType SC_ClientSkillInterface::GetSkillIconPath(int lSkillID)
{
	CF_SkillExecuteList::DataEntry *entry = LC_Helper::SafeGetSkillExecuteDataEntryPtr(lSkillID);
	if(NULL != entry)
	{
		return entry->_kIconPath;
	}

	return "";
}


StringType SC_ClientSkillInterface::GetSkillUseResult(int lSkillID)
{
	CF_SkillSpecialAttrib::DataEntry *entry = SafeGetCSVFileDataEntryPtr<CF_SkillSpecialAttrib>(lSkillID, false, false);
	if(NULL != entry)
	{
		StringType sResult = entry->_kUseResult;
		//创建角色界面使用，不需要显示具体技能效果
		StringType::size_type nPosStart = sResult.find_first_of("[");
		if(nPosStart != StringType::npos)
		{
			StringType::size_type nPosEnd = sResult.find_first_of("]");
			//]后边那个汉字也要去除
			sResult.erase(nPosStart, nPosEnd - nPosStart + 3);
		}
		return sResult;
	}

	return "";
}
//---------------------------------------------------
int64_t SC_ClientSkillInterface::GetLevelExp(int lLevel)
{
	return LC_Helper::GetLevelUpExp(lLevel);
}
//---------------------------------------------------
int64_t SC_ClientSkillInterface::GetBuddyLevelExp(int lLevel)
{
	int iMaxLevel =  CF_LevelUp::GetSingletonPtr()->GetEntryCount();
	lLevel = UT_MathBase::MaxInt(lLevel, 1);
	lLevel = UT_MathBase::MinInt(lLevel, iMaxLevel);
	
	CF_LevelUp::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_LevelUp>(lLevel);
	return pkData != NULL ? pkData->_fBuddyEXPNeed : 0;
}
//---------------------------------------------------
int SC_ClientSkillInterface::GetChangeServerCash(int lLevel)
{
	return LC_Helper::GetChangeServerCash(lLevel);
}
//------------------------------------------------------------------------------
int SC_ClientSkillInterface::GetAttributeBase(int lLevel, int iAttributeType)
{
	if (iAttributeType < ATT_MAX_HP || iAttributeType >= ATT_MAX_COUNT)
	{
		return 0;
	}

	return LC_Helper::GetAttributeBase(lLevel, (AttributeType)iAttributeType);
}
//------------------------------------------------------------------------------
double SC_ClientSkillInterface::GetAttributeRateBase(int lLevel, int iAttributeType)
{
	if (iAttributeType < ATT_MAX_HP || iAttributeType >= ATT_MAX_COUNT)
	{
		return 1.0f;
	}

	return LC_Helper::GetAttributeRateBase(lLevel, (AttributeType)iAttributeType);
}
//------------------------------------------------------------------------------
double SC_ClientSkillInterface::GetAttributeRate(LC_ActorBase* pkActor, int iAttributeType)
{
	if (NULL == pkActor)
	{
		return 1.0f;
	}

	if (iAttributeType < ATT_MAX_HP || iAttributeType >= ATT_MAX_COUNT)
	{
		return 1.0f;
	}

	return LC_Helper::GetAttributeRate(pkActor, (AttributeType)iAttributeType);
}
//------------------------------------------------------------------------------
double SC_ClientSkillInterface::GetAttributeRateUpgrade(LC_ActorBase* pkActor, int iAttributeType)
{
	if (NULL == pkActor)
	{
		return 0;
	}

	if (iAttributeType < ATT_MAX_HP || iAttributeType >= ATT_MAX_COUNT)
	{
		return 0;
	}

	double fRate = LC_Helper::GetAttributeRate(pkActor, (AttributeType)iAttributeType);
	double fRateBase = LC_Helper::GetAttributeRateBase(pkActor->GetLevel(), (AttributeType)iAttributeType);
	return ((fRate - fRateBase) * 100);
}
//------------------------------------------------------------------------------
double SC_ClientSkillInterface::GetNetAttributeRate(int lPlayerLevel, int lAttribute, int iAttributeType)
{
	double fRate = LC_Helper::ComputeAttributeRate(lPlayerLevel, lAttribute, (AttributeType)iAttributeType);
	return fRate;
}
//------------------------------------------------------------------------------
double SC_ClientSkillInterface::GetNetAttributeRateUpgrade(int lPlayerLevel, int lAttribute, int iAttributeType)
{
	double fRate = LC_Helper::ComputeAttributeRate(lPlayerLevel, lAttribute, (AttributeType)iAttributeType);
	double fRateBase = LC_Helper::ComputeAttributeRate(lPlayerLevel, lAttribute, (AttributeType)iAttributeType);
	return ((fRate - fRateBase) * 100);
}
//------------------------------------------------------------------------------
LC_ShortcutBar*	SC_ClientSkillInterface::GetShortcutBarPtr()
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return NULL;
	return pkGamePlayer->GetShortcutBar();
}
//------------------------------------------------------------------------------
LC_ShopBase* SC_ClientSkillInterface::GetNPCShopPtr(int lNpcCharTypeID,int iShopType)
{
	return LC_NPCShopManager::GetSingletonPtr()->GetNPCShop(lNpcCharTypeID,iShopType);
}
//------------------------------------------------------------------------
LC_TransferBase*	SC_ClientSkillInterface::GetNPCTransferPtr(object_id_type lNPCID)
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return NULL;

	if(!IS_NPC_ID(lNPCID))
		return NULL;

	LC_MapBase* pkMap = pkGamePlayer->GetMap();
	if(!pkMap)
		return NULL;

	LC_NPCBase* pkLockTarget = (LC_NPCBase*)pkMap->FindObject(lNPCID);
	if(!pkLockTarget)
		return NULL;

	return LC_NPCTransferManager::GetSingletonPtr()->GetNPCTransfer(pkLockTarget->GetCharType());
}
//------------------------------------------------------------------------------
LC_SkillCoolDownMap*	SC_ClientSkillInterface::GetSkillCoolDownMap()
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	LC_SkillCoolDownMap* pkSkillCoolDownMap = pkGamePlayer->GetSkillCoolDownMap();
	return pkSkillCoolDownMap;
}
//------------------------------------------------------------------------------
LC_ItemCoolDownMapBase* SC_ClientSkillInterface::GetItemCoolDownMap()
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	LC_ItemCoolDownMapBase* pkItemCoolDownMap = pkGamePlayer->GetControllerItemCoolDownMap();
	return pkItemCoolDownMap;
}
StringType SC_ClientSkillInterface::GetNpcConversationHeadIcon(int lConversationID, int iCurSeq)
{
	int lCharType = GetNPCConversationCharID(lConversationID, iCurSeq);
	if (lCharType == 0)
	{
		LC_ClientGamePlayer* pPlayer = GetPlayerPtr();
		if (pPlayer)
		{
			lCharType = pPlayer->GetCharType();
		}
	}
	if (lCharType > 0)
	{
		CF_CharType* pkCharType = CF_CharType::GetSingletonPtr();
		CF_CharType::DataEntry* pkEntry = pkCharType->GetEntryPtr(lCharType);
		if (!pkEntry)
			return "";
		return pkEntry->_kTalkHeadIcon;
	}
	else
	{
		return "";
	}
}
//------------------------------------------------------------------------
StringType	SC_ClientSkillInterface::GetLogicAreaName(int lMapID, int lAreaID)
{
	LC_LogicMapManager* pkLogicMapManager = LC_LogicMapManager::GetSingletonPtr();
	if (pkLogicMapManager == NULL)
		return "";

	LC_LogicAreaInfo* pkInfo = pkLogicMapManager->GetLogicAreaInfoFromAreaID( lMapID , lAreaID);
	if ( pkInfo != NULL )
	{
		return pkInfo->kAreaName;
	}

	return "";
}
//------------------------------------------------------------------------
UT_Vec3Int& SC_ClientSkillInterface::GetLogicAreaSpawnPos(int lMapID, int lAreaID)
{
	LC_LogicMapManager* pkLogicMapManager = LC_LogicMapManager::GetSingletonPtr();
	T_ASSERT_CRITICAL_S(pkLogicMapManager != NULL);

	LC_LogicAreaInfo* pkInfo = pkLogicMapManager->GetLogicAreaInfoFromAreaID( lMapID , lAreaID);
	T_ASSERT_CRITICAL_S(pkInfo != NULL);

	if ( pkInfo->kSpawnPos.empty() )
	{
		pkInfo->kSpawnPos.push_back(UT_Vec3Int());
	}

	return pkInfo->kSpawnPos.front();
}
//------------------------------------------------------------------------
int SC_ClientSkillInterface::GetItemID( const StringType& kItemName )
{
	CF_ItemList::DataEntryMapExternIterator kIter = CF_ItemList::GetSingletonPtr()->GetIterator();
	for( ; kIter.HasMoreElements(); kIter.MoveNext() )
	{
		const CF_ItemList::DataEntry* pkEntry = kIter.PeekNextValuePtr();
		if (kItemName == pkEntry->_kName)
		{
			return pkEntry->_iID;
		}
	}
	
	return 0;
}

int SC_ClientSkillInterface::GetItemCategoryID(int lItemID)
{
	CF_ItemList *pkItemList = CF_ItemList::GetSingletonPtr();
	CF_ItemList::DataEntry *pkItemData = pkItemList->GetEntryPtr(lItemID);
	if (NULL == pkItemData)
	{
		return -1;
	}
	
	CF_ItemEquipLimitFunc *pkEquipLimitList = CF_ItemEquipLimitFunc::GetSingletonPtr();
	CF_ItemEquipLimitFunc::DataEntry *pkEquipLimit = pkEquipLimitList->GetEntryPtr(pkItemData->_lEquipLimitFuncID);
	if(NULL == pkEquipLimit)
	{
		return -1;
	}

	return pkEquipLimit->_lEquipType;
}
//------------------------------------------------------------------------
int	SC_ClientSkillInterface::GetDakTransferAreaNum()
{
	CF_TransferArea* pkTransferArea = CF_TransferArea::GetSingletonPtr();
	if (!pkTransferArea)
	{
		return 0;
	}

	CF_TransferArea::DataEntryMapExternIterator kIter = pkTransferArea->GetIterator();

	int lDakAreaNum = 0;
	while(kIter.HasMoreElements())
	{
		const CF_TransferArea::DataEntry& kInfo = kIter.PeekNextValue();
		if (kInfo._lType == 0)
		{
			lDakAreaNum++;
		}
		kIter.MoveNext();
	}

	return lDakAreaNum;
}
//------------------------------------------------------------------------
int	SC_ClientSkillInterface::GetTransferMapID(int iIndex)
{
	CF_TransferArea* pkTransferArea = CF_TransferArea::GetSingletonPtr();
	if (!pkTransferArea)
	{
		return 0;
	}

	CF_TransferArea::DataEntryMapExternIterator kIter = pkTransferArea->GetIterator();

	int iTempIndex = 0;
	while(kIter.HasMoreElements())
	{
		const CF_TransferArea::DataEntry& kInfo = kIter.PeekNextValue();
		if (kInfo._lType == 0)
		{
			iTempIndex++;
			if(iTempIndex == iIndex)
				return kInfo._lMapID;
		}
		kIter.MoveNext();
	}

	return 0;
}
//------------------------------------------------------------------------
int	SC_ClientSkillInterface::GetTransferAreaID(int iIndex)
{
	CF_TransferArea* pkTransferArea = CF_TransferArea::GetSingletonPtr();
	if (!pkTransferArea)
	{
		return 0;
	}

	CF_TransferArea::DataEntryMapExternIterator kIter = pkTransferArea->GetIterator();

	int iTempIndex = 0;
	while(kIter.HasMoreElements())
	{
		const CF_TransferArea::DataEntry& kInfo = kIter.PeekNextValue();
		if (kInfo._lType == 0)
		{
			iTempIndex++;
			if(iTempIndex == iIndex)
			{
				LC_LogicMapManager*	pkMapManager = LC_LogicMapManager::GetSingletonPtr();
				if (pkMapManager == NULL)
					return 0;

				LC_LogicAreaInfo* kAreaInfo = pkMapManager->QueryAreaInfo(kInfo._kPos,kInfo._lMapID );
				return kAreaInfo ? kAreaInfo->lLogicAreaID : 0;
			}
		}
		kIter.MoveNext();
	}

	return 0;
}
//------------------------------------------------------------------------
ResultType	SC_ClientSkillInterface::CheckCanUseItem(int lItemID, int iParam)
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return NULL;

	bool bCanUse = false;
	int iPackType = GetWhichPackAutoPut(lItemID);
	int iEntryIndex = -1;
	int iPackCapacity = pkGamePlayer->GetPackAsset()->GetPackSize(iPackType);
	for (int i = 0; i < iPackCapacity; ++i)
	{
		LC_BackPackEntry* pkEntry = pkGamePlayer->GetPackAsset()->GetBackPackEntry(iPackType, i);
		if (pkEntry->GetValid() && !pkEntry->GetLocked() && pkEntry->GetTypeID() == lItemID)
		{
			iEntryIndex = i;
			bCanUse = true;
			break;
		}
	}

	int32_t iCount = 1;
	ResultType iResult = RE_FAIL;
	if (bCanUse)
		iResult = pkGamePlayer->CheckUseItemInPackByIndex(iPackType,iEntryIndex, iCount, iParam);
	else
		iResult = RE_TRANSFER_ITEM_NOT_EXIST;

	if (iResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(iResult);
	}

	return iResult;
}
//------------------------------------------------------------------------
int	SC_ClientSkillInterface::GetLinkInfoIDByLogicID(object_id_type lLogicID)
{
	CF_WorldObjectDesc::DataEntryMapExternIterator kIter = CF_WorldObjectDesc::GetSingletonPtr()->GetIterator();
	for( ; kIter.HasMoreElements(); kIter.MoveNext() )
	{
		const CF_WorldObjectDesc::DataEntry* pkEntry = kIter.PeekNextValuePtr();
		if (lLogicID == pkEntry->_lLogicID)
		{
			return pkEntry->_iID;
		}
	}

	return -1;
}
//------------------------------------------------------------------------
int	SC_ClientSkillInterface::GetLinkInfoID(const StringType& strLinkKeyWord)
{
	CF_WorldObjectDesc::DataEntryMapExternIterator kIter = CF_WorldObjectDesc::GetSingletonPtr()->GetIterator();
	for( ; kIter.HasMoreElements(); kIter.MoveNext() )
	{
		const CF_WorldObjectDesc::DataEntry* pkEntry = kIter.PeekNextValuePtr();
		if (strLinkKeyWord == pkEntry->_kName)
		{
			return pkEntry->_iID;
		}
	}

	return -1;
}
//------------------------------------------------------------------------
StringType SC_ClientSkillInterface::GetLinkKeyWord(int iLinkInfoID)
{
	if (CF_WorldObjectDesc::GetSingletonPtr() == NULL)
		return "";

	CF_WorldObjectDesc::DataEntry* pkEntry = CF_WorldObjectDesc::GetSingletonPtr()->GetEntryPtr(iLinkInfoID);
	if (pkEntry == NULL)
		return "";

	StringStreamType sStream;

	if(pkEntry->_lParam & LINK_PARAM_MAP_POSITION)
	{

		CF_WorldMapList::DataEntry *pkWorldMapEntry = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(pkEntry->_lMapID);
		if (pkWorldMapEntry != NULL && pkWorldMapEntry->_iMapLogicType == MT_NEWBEE && pkWorldMapEntry->_iTributaryMapCount > 1 && pkWorldMapEntry->_AutoCreate == 1)
		{
			sStream << GetMapNameByMapResID(pkEntry->_lMapID) << GET_MSG(SC_CLIENTSKILLINTERFACE_CPP_001) << "(" << GetWorld2RadarPositionInfo(pkEntry->_lMapID,pkEntry->_kLocation.x,pkEntry->_kLocation.y) << ")";
		}
		else
		{
			sStream << GetMapNameByMapResID(pkEntry->_lMapID) << "(" << GetWorld2RadarPositionInfo(pkEntry->_lMapID,pkEntry->_kLocation.x,pkEntry->_kLocation.y) << ")";
		}
	}
	else
	{
		sStream << pkEntry->_kName;
	}

	return sStream.str();
}
//------------------------------------------------------------------------
int	SC_ClientSkillInterface::GetLinkType(int iLinkInfoID)
{
	if (CF_WorldObjectDesc::GetSingletonPtr() == NULL)
		return -1;

	CF_WorldObjectDesc::DataEntry* pkEntry = CF_WorldObjectDesc::GetSingletonPtr()->GetEntryPtr(iLinkInfoID);
	if (pkEntry == NULL)
		return -1;

	return pkEntry->_lType;
}
//------------------------------------------------------------------------
int	SC_ClientSkillInterface::GetLinkObjectTypeID(int iLinkInfoID)
{
	if (CF_WorldObjectDesc::GetSingletonPtr() == NULL)
		return -1;

	CF_WorldObjectDesc::DataEntry* pkEntry = CF_WorldObjectDesc::GetSingletonPtr()->GetEntryPtr(iLinkInfoID);
	if (pkEntry == NULL)
		return -1;

	return pkEntry->_lLogicID;
}
//------------------------------------------------------------------------
UT_Vec3Int& SC_ClientSkillInterface::GetLinkPosition(int iLinkInfoID)
{
	T_ASSERT_CRITICAL_S(CF_WorldObjectDesc::GetSingletonPtr());
	CF_WorldObjectDesc::DataEntry* pkData = CF_WorldObjectDesc::GetSingletonPtr()->GetEntryPtr(iLinkInfoID);
	m_kLocVec.push_back(UT_Vec3Int(0,0,0));
	if (!pkData)
		return m_kLocVec.back();
	
	m_kLocVec.clear();
	LC_Helper::SpliteString2MultiVec3Int(pkData->_kExtLocation, m_kLocVec);
	m_kLocVec.push_back(pkData->_kLocation);
	int iRandomIndex = Utility::UT_MathBase::RandInRangeInt(0, m_kLocVec.size() - 1);
	return m_kLocVec[iRandomIndex];
}
//------------------------------------------------------------------------
int SC_ClientSkillInterface::GetLinkMapResID(int iLinkInfoID)
{
	CF_WorldObjectDesc::DataEntry* pkEntry = CF_WorldObjectDesc::GetSingletonPtr()->GetEntryPtr(iLinkInfoID);
	if (pkEntry == NULL)
		return -1;

	return pkEntry->_lMapID;
}
//------------------------------------------------------------------------
bool SC_ClientSkillInterface::IsSameMapGroup(int iMapResID1, int iMapResID2)
{
	const CF_WorldMapList::DataEntry *pkEntry1 = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(iMapResID1);
	if(pkEntry1 == NULL)
		return false;

	const CF_WorldMapList::DataEntry *pkEntry2 = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(iMapResID2);
	if(pkEntry2 == NULL)
		return false;

	return pkEntry1->_iResourceGroup == pkEntry2->_iResourceGroup;
}
//------------------------------------------------------------------------
bool SC_ClientSkillInterface::IsTravelBoots(int iLinkInfoID)
{
	if (CF_WorldObjectDesc::GetSingletonPtr() == NULL)
		return false;

	CF_WorldObjectDesc::DataEntry* pkEntry = CF_WorldObjectDesc::GetSingletonPtr()->GetEntryPtr(iLinkInfoID);
	if (pkEntry == NULL)
		return false;

	if(pkEntry->_lParam & LINK_PARAM_TRAVEL_BOOTS)
		return true;

	return false;
}


bool SC_ClientSkillInterface::IsNoGainJudge(int iLinkInfoID)
{
	if (CF_WorldObjectDesc::GetSingletonPtr() == NULL)
		return false;

	CF_WorldObjectDesc::DataEntry* pkEntry = CF_WorldObjectDesc::GetSingletonPtr()->GetEntryPtr(iLinkInfoID);
	if (pkEntry == NULL)
		return false;

	if(pkEntry->_lParam & LINK_PARAM_NO_GAIN_JUDGE)
		return true;

	return false;

}


//------------------------------------------------------------------------
StringType	SC_ClientSkillInterface::GetLocationTip(object_id_type lObjID)
{
	CF_WorldObjectDesc::DataEntry* pEntry = CF_WorldObjectDesc::GetSingletonPtr()->GetEntryPtr(lObjID);
	if (pEntry)
	{
		mem::vector<UT_Vec3Int> kLocVec;
		LC_Helper::SpliteString2MultiVec3Int(pEntry->_kExtLocation, kLocVec);
		kLocVec.push_back(pEntry->_kLocation);
		int iRandomIndex = Utility::UT_MathBase::RandInRangeInt(0, kLocVec.size() - 1);

		Utility::UT_Vec3Int pos = kLocVec[iRandomIndex];
		LC_MiniMap*pkMiniMap = LC_MapManager::GetSingletonPtr()->GetMiniMap();
		float fPosx=0.0f,fPosy=0.0f;
		pkMiniMap->CalcWorldPosToMap(pos.x,pos.y,fPosx,fPosy);
		int x = Float2Int(fPosx*500);
		int y = Float2Int(fPosy*500);
		char buffer[64];
		StringType strMapName = " ";
		CF_WorldMapList* pkWorldMapList = CF_WorldMapList::GetSingletonPtr();
		if (pkWorldMapList)
		{
			int lResourceID =  pEntry->_lMapID;
			CF_WorldMapList::DataEntry* pkEntry = pkWorldMapList->GetEntryPtr(lResourceID);
			if (pkEntry)
				strMapName = pkEntry->_kName;
		}

		sprintf(buffer, "%s,%d,%d",strMapName.c_str(),y,x);
		StringType Tip(buffer);
		return Tip;
	}

	return "";
}
//------------------------------------------------------------------------
object_id_type	SC_ClientSkillInterface::GetNPCID(int lCharTypeID)
{
	return LC_Helper::GetNpcIdByCharType(lCharTypeID);
}
//------------------------------------------------------------------------
StringType	SC_ClientSkillInterface::GetNPCName(int lCharTypeID)
{
	CF_CharType* pkCSVCharType = CF_CharType::GetSingletonPtr();
	const CF_CharType::DataEntry* pkData = pkCSVCharType->GetEntryPtr(lCharTypeID);
	if(!pkData||pkData->_kCharName == "NULL")
		return " ";

	return pkData->_kCharName;
}

StringType	SC_ClientSkillInterface::GetNPCNameColor(int lCharTypeID)
{
	CF_CharType* pkCSVCharType = CF_CharType::GetSingletonPtr();
	const CF_CharType::DataEntry* pkData = pkCSVCharType->GetEntryPtr(lCharTypeID);
	if(!pkData)
		return " ";

	return Long2String(pkData->_lNameColor);
}
//------------------------------------------------------------------------
StringType	SC_ClientSkillInterface::GetNPCHeadIcon(int lCharTypeID)
{
	CF_CharType* pkCSVCharType = CF_CharType::GetSingletonPtr();
	const CF_CharType::DataEntry* pkData = pkCSVCharType->GetEntryPtr(lCharTypeID);
	if(!pkData||pkData->_kCharName == "NULL")
		return "";

	return pkData->_kTalkHeadIcon;
}
//------------------------------------------------------------------------
int64_t SC_ClientSkillInterface::GetNPCMaxHp(int lCharTypeID)
{
	CF_CharType* pkCSVCharType = CF_CharType::GetSingletonPtr();
	const CF_CharType::DataEntry* pkData = pkCSVCharType->GetEntryPtr(lCharTypeID);
	if(!pkData)
		return 0;

	return pkData->_lMaxHP;
}
//------------------------------------------------------------------------
int	SC_ClientSkillInterface::GetNPClElite(int lCharTypeID)
{
	CF_CharType* pkCSVCharType = CF_CharType::GetSingletonPtr();
	const CF_CharType::DataEntry* pkData = pkCSVCharType->GetEntryPtr(lCharTypeID);
	if(!pkData||pkData->_kCharName == "NULL")
		return -1;

	return pkData->_lElite;
}
//------------------------------------------------------------------------
void	SC_ClientSkillInterface::PathToObject(object_id_type lObjID, int iTaskID)
{
	CF_WorldObjectDesc::DataEntry* pkEntry = CF_WorldObjectDesc::GetSingletonPtr()->GetEntryPtr(lObjID);
	if (pkEntry == NULL)
		return ;

	int tarMapLogicID = 0;
	if(pkEntry->_lParam & LINK_PARAM_MAIN_MAP)
	{
		LC_ClientNewBeeManager* pkNewBeeManager = LC_ClientNewBeeManager::GetSingletonPtr();
		if (pkNewBeeManager)
		{
			tarMapLogicID = pkNewBeeManager->GetMainLogicIDByMapResID(pkEntry->_lMapID);
		}
	};

	mem::vector<UT_Vec3Int> kLocVec;
	LC_Helper::SpliteString2MultiVec3Int(pkEntry->_kExtLocation, kLocVec);
	kLocVec.push_back(pkEntry->_kLocation);

	
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return;

	//因为传功长老让主角放技能，如果此时响应这些操作，根本无法处理
	if (pkGamePlayer->GetAIControl() )
		return ;

	int iri = pkGamePlayer->GetID() % kLocVec.size();
	Utility::UT_Vec3Int pos = kLocVec[iri];

	LC_MapBase*pkMap = pkGamePlayer->GetMap();
	if (pkMap == NULL)
		return;

	if(pkGamePlayer->GetMapResID() != pkEntry->_lMapID)
	{
 		LC_AcrossMapPath& rPath = pkGamePlayer->GetAcrossMapPath();
		rPath.SetAcrossMapPath(pkGamePlayer->GetMapResID(),pkEntry->_lMapID);
 		if (rPath.GetTransferPosLocation(pkGamePlayer->GetMapResID(),pos) == false)
		{
			SendLogicInfoMessage(RE_FINDPATH_FAIL_NOT_IN_TEMP_MAP);
			return;
		}
		
		pkGamePlayer->PendingPathToLocation(pkEntry->_lMapID,pos);
	}

	SE_SceneManager* pkSceneManager = SE_SceneManager::GetSingletonPtr();
	const SE_SceneTerrainLocalInfo& kInfo = pkSceneManager->GetWalkableInfo((float)pos.x, (float)pos.y);
	pos.z = static_cast < int > ( kInfo._fLocalHeight );

	//LINK_TYPE_MAP_TRANSFER_HOOK 类型不寻路，直接开始打怪
	if(pkEntry->_lType != LINK_TYPE_MAP_TRANSFER_HOOK)
		pkGamePlayer->PathToPoint(pos);

	bool bTarget = tarMapLogicID > 0;
	switch(pkEntry->_lType)
	{
	case LINK_TYPE_NPC_LOCATION:
		{
			pkGamePlayer->PendingPathAction(PPAT_TALK_TO_NPC, pkEntry->_lLogicID, bTarget,tarMapLogicID);
		}
		break;
	case LINK_TYPE_COLLECT_LOCATION:
		{
			if(iTaskID > 0)
				((SM_ClientGamePlayerController*)pkGamePlayer->GetSMController())->GuildPush(pkGamePlayer, iTaskID);
			else
				pkGamePlayer->PendingPathAction(PPAT_TALK_TO_NPC, pkEntry->_lLogicID, bTarget,tarMapLogicID);
		}
		break;
	case LINK_TYPE_MONSTER_LOCATION:
	case LINK_TYPE_MAP_TRANSFER_HOOK:
	case LINK_TYPE_GOLDEN_HOOK:
		{
			pkGamePlayer->PendingPathAction(PPAT_ATTACK_NPC, pkEntry->_lLogicID, bTarget,tarMapLogicID);
		}
		break;
	case LINK_TYPE_PARALLEL_SPACE:
		{
			pkGamePlayer->PendingPathAction(PPAT_TRANSFER, pkEntry->_lLogicID, false, 0);
		}
		break;
	default:
		{
			pkGamePlayer->ClearPathPendingAction();
		}
		break;
	}

	//SendLogicInfoMessage(RE_FINDPATH_START);
}

void	SC_ClientSkillInterface::PathToLocation(int iMapResID, int iX, int iY, int iZ, int tarMapLogicID)
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return;

	LC_MapBase*pkMap = pkGamePlayer->GetMap();
	if (pkMap == NULL)
		return;

	//因为传功长老让主角放技能，如果此时响应这些操作，根本无法处理
	if (pkGamePlayer->GetAIControl() )
		return ;

	LC_ClientNewBeeManager* pkNewBeeManager = LC_ClientNewBeeManager::GetSingletonPtr();
	if (pkNewBeeManager && pkNewBeeManager->IsMapLogicIDIsMapResID(iMapResID,tarMapLogicID))
	{
		pkGamePlayer->PendingPathAction(PPAT_NA,0,true,tarMapLogicID);
	}
	else
	{
		pkGamePlayer->PendingPathAction(PPAT_NA,0,false,0);
	}
	Utility::UT_Vec3Int pos(iX, iY, iZ);
	if(pkGamePlayer->GetMapResID() != iMapResID)
	{
		LC_AcrossMapPath& rPath = pkGamePlayer->GetAcrossMapPath();
		rPath.SetAcrossMapPath(pkGamePlayer->GetMapResID(), iMapResID);
		if (rPath.GetTransferPosLocation(pkGamePlayer->GetMapResID(), pos) == false)
		{
			SendLogicInfoMessage(RE_FINDPATH_FAIL_NOT_IN_TEMP_MAP);
			return;
		}
	}

	SE_SceneManager* pkSceneManager = SE_SceneManager::GetSingletonPtr();
	const SE_SceneTerrainLocalInfo& kInfo = pkSceneManager->GetWalkableInfo((float)pos.x, (float)pos.y);
	pos.z = static_cast < int > ( kInfo._fLocalHeight );

	pkGamePlayer->PathToPoint(pos);
}

bool SC_ClientSkillInterface::WalkToObject(int curMapResID, int tarMapResID, object_id_type lObjID)
{
	CF_WorldObjectDesc::DataEntry* pkEntry = CF_WorldObjectDesc::GetSingletonPtr()->GetEntryPtr(lObjID);
	if (pkEntry != NULL)
	{
		tarMapResID = pkEntry->_lMapID;
	}
		
	int tarMapLogicID = 0;
	if(pkEntry && pkEntry->_lParam & LINK_PARAM_MAIN_MAP)
	{
		LC_ClientNewBeeManager* pkNewBeeManager = LC_ClientNewBeeManager::GetSingletonPtr();
		if (pkNewBeeManager)
		{
			tarMapLogicID = pkNewBeeManager->GetMainLogicIDByMapResID(tarMapResID);
		}
	};

	bool bFlag = false;
	SM_WalkTransferManager::GetSingletonPtr()->PushFinaObj(lObjID);
	SM_WalkTransferManager::GetSingletonPtr()->PushMapLogicID(tarMapLogicID);
	SM_WalkTransferManager::GetSingletonPtr()->Search(curMapResID, tarMapResID);
	int rtMapResID = 0;
	UT_Vec3Int tarPosition;
	int localNPCID = 0;
	
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(pkGamePlayer)
	{
		SM_WalkTransferManager::GetSingletonPtr()->GetNextPt(pkGamePlayer->GetMapResID(), rtMapResID, tarPosition, localNPCID);
		if (curMapResID == rtMapResID)
		{
			pkGamePlayer->PathToPoint(tarPosition);
			if ( localNPCID > 0 )
			{
				pkGamePlayer->PendingPathAction(PPAT_TALK_TO_NPC, localNPCID);
			}
			bFlag = true;
		}
	}
	return bFlag;
}

bool SC_ClientSkillInterface::WalkToLocation(int curMapResID, int tarMapResID, int _x, int _y, int _z, int tarMapLogicID)
{
	bool bFlag = false;
	SM_WalkTransferManager::GetSingletonPtr()->PushFindLocation(UT_Vec3Int(_x,_y,_z));
	SM_WalkTransferManager::GetSingletonPtr()->Search(curMapResID, tarMapResID);
	int rtMapResID = 0;
	UT_Vec3Int tarPosition;
	int localNPCID = 0;

	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	LC_ClientNewBeeManager* pkNewBeeManager = LC_ClientNewBeeManager::GetSingletonPtr();
	if (pkNewBeeManager && pkNewBeeManager->IsMapLogicIDIsMapResID(tarMapResID,tarMapLogicID))
	{
		SM_WalkTransferManager::GetSingletonPtr()->PushMapLogicID(tarMapLogicID);
	}
	else
	{
		SM_WalkTransferManager::GetSingletonPtr()->PushMapLogicID(0);
	}

	if(pkGamePlayer)
	{
		SM_WalkTransferManager::GetSingletonPtr()->GetNextPt(pkGamePlayer->GetMapResID(), rtMapResID, tarPosition, localNPCID);
		if (curMapResID == rtMapResID)
		{
			pkGamePlayer->PathToPoint(tarPosition);
			if ( localNPCID > 0 )
			{
				pkGamePlayer->PendingPathAction(PPAT_TALK_TO_NPC, localNPCID);
			}
			bFlag = true;
		}
	}

	return bFlag;
}

StringType SC_ClientSkillInterface::GetPlayerLocation(void)
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return "";

	const Utility::UT_Vec3Int kLocation = pkGamePlayer->GetVisibleLocation();
	StringStreamType sStream("");
	sStream << kLocation.x << " " << kLocation.y << " " << kLocation.z;
	return sStream.str();
}
//----------------------------------------------------------------------------
static bool isRunInMiniLauncher()	/// 是否由微端启动
{
	#ifdef CLIENT_DLL

		StringType c = ClientCmdlineArg::GetValueSafe(CMD_ARG_CLIENT_TYPE);
		if (c == "pc")		/// 该判断方式由360提供，跳转链接中有该参数
			return true;
		return false;

	#else

		return false;
	#endif
}
static BOOL jzOpenUrl(const char *url)
{
	YunYingPlatform yyp = getYunYingplatform();
	bool bRunInBrowser = !isRunInMiniLauncher();
	int OpenType = bRunInBrowser ? YYOUT_CURRENT : YYOUT_DEFAULT;
	CF_YunYingOpenUrlConfig::DataEntry* pentry = SafeGetCSVFileDataEntryPtr<CF_YunYingOpenUrlConfig>(yyp);
	if (pentry != NULL)
	{
		OpenType = bRunInBrowser ? pentry->_BrowserType : pentry->_MiniLoadType;
	}
	switch (OpenType)
	{
	case YYOUT_CURRENT:
		{
			/// 内网直接走ShellExecute
			#ifndef CLIENT_DLL
				break;
			#endif

			//< current browser, can't be used in Launcher
			char	cmdline[1024] = {0};
			if (bRunInBrowser)		// 浏览器的情况下
			{
				::GetModuleFileName(0, cmdline, 1024);		// 当前进程是否是jlg_sxsy_loader.exe
				::strlwr(cmdline);
				if (strstr(cmdline, "jlg_sxsy_loader.exe") != NULL)
				{
					bool bGot = GetParentProcessModuleFilename(cmdline, 1024);
					if (!bGot)		// failed
						cmdline[0] = 0;
				}
				else
				{
					/// 否则可能是dll模式，当前进程就是浏览器
				}
			}

			/// 有效的浏览器进程
			if (cmdline[0] != 0)
			{
				strcat(cmdline, " \"");
				strcat(cmdline, url);
				strcat(cmdline, "\"");
				RunExe(NULL, SW_SHOWNORMAL, NULL, cmdline);
				return TRUE;
			}
		}
		break;
	case YYOUT_IEXPLORE:
		{
			//< ie
			char	ie_path[1024] = "\"";
			BOOL b2 = ::SHGetSpecialFolderPath(NULL, ie_path+1, CSIDL_PROGRAM_FILES, FALSE);
			if (b2)
			{
				strcat(ie_path, "\\Internet Explorer\\iexplore.exe\"");

				strcat(ie_path, " \"");
				strcat(ie_path, url);
				strcat(ie_path, "\"");
				RunExe(NULL, SW_SHOWNORMAL, NULL, ie_path);
				return TRUE;
			}
		}
		break;
	}

	::ShellExecute(0, "open", url, 0, 0, SW_SHOWNORMAL);
	return TRUE;
}
//----------------------------------------------------------------------------
void SC_ClientSkillInterface::OnGotoUrl(const StringType& rkUrl)
{
	jzOpenUrl(rkUrl.c_str());
}
//----------------------------------------------------------------------------
void SC_ClientSkillInterface::OnshowWebWindow(const StringType& rkUrl, int width, int height)
{
	showWebWindow(rkUrl.c_str(), width, height);
}

void SC_ClientSkillInterface::ShowShunWangWebWindow(const char* rkUrl, const char* rkJumpUrl, int width, int height)
{
	showShunWangWebWindow(rkUrl, rkJumpUrl, width, height);
}

void SC_ClientSkillInterface::OnHideWebWindow()
{
	hideWebWindow();
}

void SC_ClientSkillInterface::OnResetWebWindow(int width, int height, int x, int y)
{
	resetInnerWebWindow(width, height, x, y);
}
//----------------------------------------------------------------------------
bool SC_ClientSkillInterface::ScriptJzCmdlineHasValue(const StringType& argument)
{
	return ClientCmdlineArg::HasValue(argument);
}
//----------------------------------------------------------------------------
StringType SC_ClientSkillInterface::ScriptJzCmdlineGetValue(const StringType& argument)
{
	return ClientCmdlineArg::GetValueSafe(argument);
}
//----------------------------------------------------------------------------
void SC_ClientSkillInterface::OnGotoUserLogin()
{
	NW_ClientSessionManager* pkSessionManager = NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup* pkSessionGroup = pkSessionManager->GetGameSessionGroup();
	pkSessionGroup->m_pkGameStageMachine->SetNextState(ST_GAME_STAGE_LOGIN);
	pkSessionGroup->m_pkGameStageMachine->Update(0.0f,0.0f);

	pkSessionGroup->m_pkLoginSession->Disconnect();
}
//-------------------------------------------------------------

void SC_ClientSkillInterface::OnLoadingToGameStage()
{
	NW_ClientSessionGroup* pkSessionGroup = NW_ClientSessionManager::GetSingleton().GetGameSessionGroup();
    switch (pkSessionGroup->m_pkGameStageMachine->GetPreStateType())
    {
	case ST_GAME_STAGE_START:
    case ST_GAME_STAGE_SEL_CHARACTER:
	case ST_GAME_STAGE_CREATE_CHARACTER:
    case ST_GAME_STAGE_GAME:
        pkSessionGroup->m_pkGameStageMachine->SetNextState(ST_GAME_STAGE_GAME);
        break;
    case ST_GAME_STAGE_QUIT:
		//if(pkSessionGroup->m_pkGameStageMachine->GetChangeServerFlag())
		pkSessionGroup->m_pkGameStageMachine->SetNextState(ST_GAME_STAGE_LOGIN);
		//else
		//	pkSessionGroup->m_pkGameStageMachine->SetNextState(ST_GAME_STAGE_GAME);
		break;
	case ST_GAME_STAGE_CHANGE_SERVER:
		pkSessionGroup->m_pkGameStageMachine->SetNextState(ST_GAME_STAGE_GAME);
		break;
    default:
        break;
    }
	//pkSessionGroup->m_pkGameStageMachine->Update(0.0f, 0.0f);
}
//-------------------------------------------------------------
int SC_ClientSkillInterface::GetPreStateType()
{
	NW_ClientSessionGroup* pkSessionGroup = NW_ClientSessionManager::GetSingleton().GetGameSessionGroup();
	return pkSessionGroup->m_pkGameStageMachine->GetPreStateType();
}
//-------------------------------------------------------------
void SC_ClientSkillInterface::OnQuitLogin()
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	NW_ClientSessionManager* pkSessionManager	= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup* pkSessionGroup		= pkSessionManager->GetGameSessionGroup();
	NW_Login2ClientSession* pkLoginSession		= pkSessionGroup->m_pkLoginSession;

	////向LoginServer发送断开连接消息
// 	MG_LC_Req_Disconnect msg;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_LC_REQ_DISCONNECT, NULL);
	if(bCreateMsg)
	{
		pkLoginSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
		Utility::GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM,"[网络]请求断开连接");
	}

	pkLoginSession->Disconnect();

	//转到Login界面
	pkSessionGroup->m_pkGameStageMachine->SetNextState(ST_GAME_STAGE_LOGIN);
	pkSessionGroup->m_pkGameStageMachine->Update(0.0f,0.0f);
}
//-------------------------------------------------------------
int SC_ClientSkillInterface::GetLastLoginServerID()
{
	return GetUserProfileIntVal("LastLoginServerID");
}
//-------------------------------------------------------------
void SC_ClientSkillInterface::SaveLastLoginServerID(int iServerID)
{
	SetUserProfileIntVal("LastLoginServerID",iServerID);

	NW_ClientSessionManager* pkSessionManager	= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup* pkSessionGroup		= pkSessionManager->GetGameSessionGroup();
	pkSessionGroup->SetCurCitizenShip(iServerID);
}
//-------------------------------------------------------------
void SC_ClientSkillInterface::SaveAutoCommitTeamRequest(int iAuto)
{
	SetUserProfileIntVal("AutoCommitTeamRequest", iAuto);
}

int SC_ClientSkillInterface::GetAutoCommitTeamRequest(void)
{
	return (GetUserProfileIntVal("AutoCommitTeamRequest"));
}
//-------------------------------------------------------------
void SC_ClientSkillInterface::SaveAutoCommitTeamInvite(int iAuto)
{
	SetUserProfileIntVal("AutoCommitTeamInvite", iAuto);
}

int SC_ClientSkillInterface::GetAutoCommitTeamInvite(void)
{
	Utility::UT_ConfigFile kConfig;
	if ( kConfig.Load(CLIENT_USER_PROFILE) )
		return kConfig.GetIntSetting("AutoCommitTeamInvite", "Client", 1);

	::WritePrivateProfileString("Client", "AutoCommitTeamInvite", "1", CLIENT_USER_PROFILE);
	return 1;
}
//-------------------------------------------------------------
unique_id_type SC_ClientSkillInterface::GetLastLoginCharID()
{
	return GetUserProfileInt64Val("LastLoginCharID");
}
//-------------------------------------------------------------
void SC_ClientSkillInterface::SaveLastLoginCharID(unique_id_type iCharID)
{
	SetUserProfileInt64Val("LastLoginCharID",iCharID);
}
//-------------------------------------------------------------
int SC_ClientSkillInterface::GetShowBattleBuddyEquip()
{
	return GetUserProfileInt64Val("ShowBattleBuddyEquip");
}
//-------------------------------------------------------------
void SC_ClientSkillInterface::SaveShowBattleBuddyEquip(int m_iShow)
{
	SetUserProfileInt64Val("ShowBattleBuddyEquip",m_iShow);
}
//-------------------------------------------------------------
int SC_ClientSkillInterface::GetShowBattleBuddyRune()
{
	return GetUserProfileInt64Val("ShowBattleBuddyRune");
}
//-------------------------------------------------------------
void SC_ClientSkillInterface::SaveShowBattleBuddyRune(int m_iShow)
{
	SetUserProfileInt64Val("ShowBattleBuddyRune",m_iShow);
}
//----------------------------------------------------------------------------
void SC_ClientSkillInterface::OnCheckForceLogin()
{
	SendLogicInfoMessage(RE_LOGIN_KICKOUT_ONLINE_USER);
}
//----------------------------------------------------------------------------
void SC_ClientSkillInterface::OnUserLogin(const StringType& strUser, const StringType& strPwd, bool bForceLogin, bool bRemember)
{
	if (strUser == "")
	{
		SendLogicInfoMessage(RE_LOGIN_USERNAME_NULL);
		return;
	}
	if (strUser.length() < 5)
	{
		SendLogicInfoMessage(RE_LOGIN_INVALIDATING_USER);
		return;
	}
	if (strPwd == "")
	{
		SendLogicInfoMessage(RE_LOGIN_PASSWORD_NULL);
		return;
	}
	if (strUser[0] == 'j' && strUser[1] == 'z' && strPwd != "12345")
	{
		SendLogicInfoMessage(RE_LOGIN_USERNAME_PASSWORD_WRONG);
		return;
	}
	
	if(strUser.length() == 10)
	{
		if (strUser[0] == 'm' &&
			strUser[1] == 'k' &&
			strUser[2] == 't' &&
			strUser[3] == 'e' &&
			strUser[4] == 's' && 
			strUser[5] == 't' && 
			strPwd != "12345")
		{
			SendLogicInfoMessage(RE_LOGIN_USERNAME_PASSWORD_WRONG);
			return;
		}

	}

	if (bForceLogin)
	{
#pragma TODO("添加强制登录的判断逻辑")
	}

	NW_ClientSessionManager* pkClientSessionMgr		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup* pkSessionGroup			= pkClientSessionMgr->GetGameSessionGroup();
	NW_Login2ClientSession* pkLoginSession			= pkSessionGroup->m_pkLoginSession;

	pkSessionGroup->m_strUser = strUser;
	pkSessionGroup->m_strPwd  = strPwd;
  
	pkSessionGroup->SetNetState(NLS_WAIT_SIGNATURE);

	//保存进入游戏选中的角色名字

	StringType sName = (bRemember ? pkSessionGroup->m_strUser : "");
	SaveNamePwdToConfig(sName, bRemember ? "1": "0");

	pkLoginSession->FakeAuth();

	GfxWriteLog( LOG_TAG_CLIENT_CRITICAL , LOG_SWITCH_CLIENT_CRITICAL ,
		"[uid:%s]" , strUser.c_str() );	
}
//----------------------------------------------------------------------------
void SC_ClientSkillInterface::SelectLoginZone(int iServerID)
{
	NW_ClientSessionManager* pkSessionManager	= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkSessionGroup		= pkSessionManager->GetGameSessionGroup();
	NW_Login2ClientSession*	 pkLoginSession		= pkSessionGroup->m_pkLoginSession;

	PS_GameSvrInfo* pkServerInfo					= pkSessionManager->GetLoginZoneInfo(iServerID);
	if(NULL == pkServerInfo)
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"选中无效LoginServer");
		return;
	}

	//先断开连接
	pkLoginSession->Disconnect(true);
	 NW_ClientNetManager::GetSingleton().Update();

	//选中路由
	PS_GameSvrInfo kGateWayInfo = pkSessionManager->GetGateWayInfoByUserID(iServerID, pkSessionGroup->m_dwUserID);
	//连接选中的登录服务器
	
	pkLoginSession->SetConnectIPAddress(kGateWayInfo.m_szIP.c_str(),kGateWayInfo.m_iPort);
	GfxWriteLog(LOG_SYSTEM_FILELINE,LOG_SWITCH_SYSTEM,"Connect to gate: %s:%d", kGateWayInfo.m_szIP.c_str(),kGateWayInfo.m_iPort);
	pkLoginSession->TryToConnect();
}
//----------------------------------------------------------------------------
void SC_ClientSkillInterface::OnGotoSelChar()
{
	NW_ClientSessionManager* pkSessionManager	= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup* pkSessionGroup		= pkSessionManager->GetGameSessionGroup();
	if (NULL == pkSessionGroup)
		return;

	pkSessionGroup->m_pkGameStageMachine->SetNextState(ST_GAME_STAGE_SEL_CHARACTER);
	pkSessionGroup->m_pkGameStageMachine->Update(0.0f,0.0f);
}
//----------------------------------------------------------------------------
void SC_ClientSkillInterface::OnGotoCreateChar(void)//int iServerID)
{
	NW_ClientSessionManager* pkSessionManager	= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup* pkSessionGroup		= pkSessionManager->GetGameSessionGroup();
	//NW_Login2ClientSession* m_pkLoginSession	= pkSessionGroup->m_pkLoginSession;
	DWORD dwUserID								= pkSessionGroup->m_dwUserID;
	PS_CharBaseInfoList& rkCharList				= pkSessionGroup->GetCharList();

	//对每个用户可以创建的角色数量进行限制
	if (rkCharList.size() >= MAX_USER_CHAR_COUNT)
	{
		//提示角色数目已满
		SendLogicInfoMessage(RE_LOGIN_CHAR_FULL);
		return;
	}

	pkSessionGroup->m_pkGameStageMachine->SetNextState(ST_GAME_STAGE_CREATE_CHARACTER);
	pkSessionGroup->m_pkGameStageMachine->Update(0.0f,0.0f);
}
//----------------------------------------------------------------------------
int SC_ClientSkillInterface::GetCreateCharServerID()
{
	NW_ClientSessionManager* pkSessionManager	= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup* pkSessionGroup		= pkSessionManager->GetGameSessionGroup();	
	return pkSessionGroup->GetCreateCharServerID();
}
//----------------------------------------------------------------------------
bool SC_ClientSkillInterface::OnCreateChar( const StringType& strCharName, int iCid, 
										   int lHeadEquipID, int lBodyEquipID, int lFootEquipID, 
										   int lWeaponID, int iWeaponType, int lServerID)
{
	NW_ClientSessionManager* pkSessionManager	= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup* pkSessionGroup		= pkSessionManager->GetGameSessionGroup();
	NW_Login2ClientSession* m_pkLoginSession	= pkSessionGroup->m_pkLoginSession;

	if(pkSessionGroup->GetNetState() == NLS_WAIT_ENTER_GAME || pkSessionGroup->GetInGameFlag())
		return false;
	
	if (strCharName.empty())
	{
		SendLogicInfoMessage(RE_LOGIN_CHARNAME_NULL);
		return false;
	}

	std::wstring wText = CodeConvert::Utf8ToWcs(strCharName);
	if (wText.size() > MAX_USER_NAME_CHARACTER)
	{
		SendLogicInfoMessage(RE_LOGIN_CREATE_CHAR_FAIL_NAME_TOO_LONG);
		return false;
	}
	
	TT_TextFilter* pkTextFilter = TT_TextFilter::GetSingletonPtr();
	if (NULL != pkTextFilter)
	{
		pkTextFilter->SetText(strCharName);
		//如果找到非法字符
		if ( pkTextFilter->FindAndFilterForbiddenText(TT_TextFilter::TFT_CHAR_NAME,false,true) )
		{
			SendLogicInfoMessage(RE_LOGIN_CHARNAME_INVALID);
			return false;
		}
	}
	
	if((*strCharName.rbegin()) == '?')
	{
		SendLogicInfoMessage(RE_LOGIN_CHARNAME_INVALID);
		return false;
	}

	DWORD dwUserID								= pkSessionGroup->m_dwUserID;
	PS_CharBaseInfoList& rkCharList				= pkSessionGroup->GetCharList();
	//对每个用户可以创建的角色数量进行限制
	if (rkCharList.size() >= MAX_USER_CHAR_COUNT)
	{
		//提示角色数目已满
		SendLogicInfoMessage(RE_LOGIN_CHAR_FULL);
		return false;
	}

	//int lServerID = pkSessionGroup->GetCreateCharServerID();

	//服务器必须合法
	/*int iServerID = UT_BaseStringUtility::toInt(ClientCmdlineArg::GetValueSafe(CMD_ARG_SID));
	if(0 == iServerID)
	{
		iServerID = lServerID;
		if(NULL == pkSessionManager->GetGameServerInfo(iServerID))
		{
			SendLogicInfoMessage(RE_LOGIN_CREATE_CHAR_FAIL_NEED_SERVER);
			return;
		}
	}*/

	/*if(lServerID  == 10010301)
	{
		SendLogicInfoMessage(RE_LOGIN_CREATE_CHAR_FAIL_SERVER_BUSY);
		return;
	}*/

	MG_LC_Req_CreateChar kCreateCharMsg;
	kCreateCharMsg.m_dwUserID = dwUserID;
	
#pragma TODO("暂时将一些参数置空")
	
	kCreateCharMsg.m_bySex = iCid;
	kCreateCharMsg.m_lSuiteHead = lHeadEquipID;
	kCreateCharMsg.m_lSuiteBody = lBodyEquipID;
	kCreateCharMsg.m_lSuiteFoot = lFootEquipID;
	kCreateCharMsg.m_lWeapon = lWeaponID;
	//kCreateCharMsg.m_lHeadPic = lHeadPicID;
	kCreateCharMsg.m_byWeaponType = iWeaponType;
	
	//检查名字是否过长
	/*int iCopyDstLength = MAX_CHAR_NAME_LENGTH + 1;
	int iCopySrcLength = strCharName.length() + 1;
	if (iCopyDstLength < iCopySrcLength)
	{
		SendLogicInfoMessage(RE_LOGIN_CREATE_CHAR_FAIL_NAME_TOO_LONG);
		return;
	}*/
	TPSTR2STDSTR(strCharName, kCreateCharMsg.m_szCharName);
	
	kCreateCharMsg.m_iCitizenship = lServerID;

	//pkSessionGroup->SetNetState(NLS_WAIT_CREATE_CHAR);
	
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(pkSessionGroup, CRYPT_ALGORITHM_NONE, hMsgHandler, MGPT_LC_REQ_CREATE_CHAR, &kCreateCharMsg);
	if (bCreateMsg)
	{
		m_pkLoginSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
		//打点: 点击创角
		commitPatchProgress(EVENT_CLIENT_CREATE_ROLE, 0);
		GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM, "[网络][发]请求创建角色, %d", MGPT_LC_REQ_CREATE_CHAR);
	}

	return true;
}
//------------------------------------------------------------------------
void SC_ClientSkillInterface::StartCountDown(bool bLeaveGame)
{
	NW_ClientSessionManager* pkSessionManager	= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup* pkSessionGroup		= pkSessionManager->GetGameSessionGroup();
	
	if(!pkSessionGroup)
		return;

	int eGameState = pkSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	if(bLeaveGame)
	{
		PostMessage(g_hWnd, WM_DESTROY, 0, 0);
	}
	else
	{
		RequestResetGame();
	}

	/*if(eGameState != ST_GAME_STAGE_GAME)
		PostMessage(g_hWnd, WM_DESTROY, 0, 0);
	else
	{
		if (bLeaveGame)
			pkSessionGroup->m_pkGameStageMachine->SetCloseGameFlag(bLeaveGame);

		if(!pkSessionGroup->m_pkGameStageMachine->GetStartCountDownFlag())
		{
			pkSessionGroup->m_pkGameStageMachine->SetReadyLeaveGameFlag(true);
			pkSessionGroup->m_pkGameStageMachine->SetStartCountDownFlag(true);
		}
	}*/

}
//------------------------------------------------------------------------
void SC_ClientSkillInterface::CancleCountDown()
{
	NW_ClientSessionManager* pkSessionManager	= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup* pkSessionGroup		= pkSessionManager->GetGameSessionGroup();

	pkSessionGroup->m_pkGameStageMachine->SetCloseGameFlag(false);
	pkSessionGroup->m_pkGameStageMachine->SetStartCountDownFlag(false);
}
//------------------------------------------------------------------------
void SC_ClientSkillInterface::RequestLeaveGame()
{
	RG_RenderManager* pkRenderManager = RG_RenderManager::GetSingletonPtr();
	T_ASSERT(0 != pkRenderManager);

	NW_ClientSessionManager* pkSessionManager	= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup* pkSessionGroup		= pkSessionManager->GetGameSessionGroup();

#ifndef CLIENT_DLL_INTERNAL
	if(!g_dllRunInBrowser)
#endif
	{
		//转到Quit流程
		pkSessionGroup->m_pkGameStageMachine->SetCloseGameFlag(true);
		pkSessionGroup->m_pkGameStageMachine->SetStartCountDownFlag(false);
		pkSessionGroup->m_pkGameStageMachine->SetNextState(ST_GAME_STAGE_QUIT);
	}
}
//------------------------------------------------------------------------
void SC_ClientSkillInterface::RequestResetGame()
{
	NW_ClientSessionManager* pkSessionManager	= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup* pkSessionGroup		= pkSessionManager->GetGameSessionGroup();
	//NW_Game2ClientSession* pkGameSession		= pkSessionGroup->m_pkGameSession;
	//if (!pkGameSession)
	//	return;

	//MG_PACKET_ENCODE_HANDLER hMsgHandler;
	//bool bCreateMsg = false;

	//MG_REQ_ResetGame kMsg;
	//bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_GC_REQ_RESET_GAME, &kMsg);
	//if(bCreateMsg)
	//	pkGameSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	//转到Quit流程
	UI_UIManager::GetSingletonPtr()->SetResetFlag(true);
	pkSessionGroup->m_pkGameStageMachine->SetCloseGameFlag(false);
	pkSessionGroup->m_pkGameStageMachine->SetStartCountDownFlag(false);
	pkSessionGroup->m_pkGameStageMachine->SetNextState(ST_GAME_STAGE_QUIT);
	pkSessionGroup->m_pkGameStageMachine->Update(0.0f,0.0f);
//ToCheck: 转回登陆界面时 清空NetPlayer的连续播放音效
	g_soundMgr.StopPlayerSkillSounds();
}
//------------------------------------------------------------------------
void SC_ClientSkillInterface::RequestReLoginGame()
{
	NW_ClientSessionManager* pkSessionManager	= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup* pkSessionGroup		= pkSessionManager->GetGameSessionGroup();
	NW_Login2ClientSession* pkLoginSession		= pkSessionGroup->m_pkLoginSession;

	bool bInGame = pkSessionGroup->GetInGameFlag();
	bool bIsConnected = pkLoginSession->IsConnected();
	GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI, "SC_ClientSkillInterface::RequestReLoginGame bInGame=[%d] bIsConnected=[%d]", bInGame, bIsConnected);

	if (!bInGame)		return;
	if (bIsConnected)	return;

	GfxWriteLog( LOG_TAG_WARNING , LOG_SWITCH_WARNING ,	__FUNCTION__ );	

	UI_UIManager::GetSingletonPtr()->SetResetFlag(true);
	pkSessionManager->ResetSessionGroup(pkLoginSession);
}

//------------------------------------------------------------------------
void SC_ClientSkillInterface::ResetUINameArrage()
{
	UE_UiEffectManager* pkUIEfm = UE_UiEffectManager::GetSingletonPtr();
	if(pkUIEfm)
		pkUIEfm->ClearNameArrange();
}
//------------------------------------------------------------------------
void SC_ClientSkillInterface::OnLowScoreConfirm(combat_score_type iCurSocre, combat_score_type iNeedScore, int iParam)
{
	SendMessageDialog(RE_DIALOG_CONFIRM_LOW_SCORE, (combat_score_type)iNeedScore,(combat_score_type)iCurSocre, MDT_DIALOG_CONFIRM_LOW_SCORE, iParam, 0, 0);
}
//------------------------------------------------------------------------
int SC_ClientSkillInterface::GetGameNetState()
{
	NW_ClientSessionManager* pkSessionManager	= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup* pkSessionGroup		= pkSessionManager->GetGameSessionGroup();

	return pkSessionGroup->GetNetState();
}
//----------------------------------------------------------------------------
void SC_ClientSkillInterface::OnGotoSelServer()
{
	NW_ClientSessionManager* pkSessionManager	= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup* pkSessionGroup		= pkSessionManager->GetGameSessionGroup();
	//if(pkSessionGroup->GetNetState() == NLS_WAIT_ENTER_GAME)
	//	return;

	//进入选择服务器界面
	pkSessionGroup->m_pkGameStageMachine->SetNextState(ST_GAME_STAGE_SEL_SERVER);
	pkSessionGroup->m_pkGameStageMachine->Update(0.0f,0.0f);
}
//----------------------------------------------------------------------------
void SC_ClientSkillInterface::OnEnterGame(unique_id_type iPlayerID)
{
	NW_ClientSessionManager* pkSessionManager	= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup* pkSessionGroup		= pkSessionManager->GetGameSessionGroup();
	NW_Login2ClientSession* pkLoginSession		= pkSessionGroup->m_pkLoginSession;

	if(pkSessionGroup->GetNetState() == NLS_WAIT_ENTER_GAME)
		return;


	unique_id_impl iCharID = unique_id_impl(iPlayerID);
	int iServerID = iCharID.catagory();
	//用户ID
	DWORD dwUserID					= pkSessionGroup->m_dwUserID;				

	//设置进入游戏的角色
	pkSessionGroup->SetSelectChar(iCharID);
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	pkLogicManager->SetOperationPlayerID(dwUserID);
	pkLogicManager->SetOperationCharID(iCharID);


	StringType sSvrId = ClientCmdlineArg::GetValueSafe(CMD_ARG_SID);
	if (!sSvrId.empty())
		iServerID = UT_BaseStringUtility::toInt(sSvrId);


	pkSessionGroup->SetSelectChar(iCharID);

	MG_LC_Req_Enter_GameServer	reqMsg;
	reqMsg.m_uiCharID	= pkSessionGroup->GetSelectChar();
	reqMsg.m_iServerID = iServerID;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_LC_REQ_ENTER_GAMESERVER, &reqMsg);
	if (bCreateMsg)
	{
		pkLoginSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
		Utility::GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM,"[网络][发]请求进入GameServer:%d, CharID:0x%I64x, msg=%d", iServerID, reqMsg.m_uiCharID, MGPT_LC_REQ_ENTER_GAMESERVER);
	}

	//打点: 点击进入游戏按钮，请求进入游戏
	commitPatchProgress(EVENT_CLIENT_REQ_ENTER_GAME, 0);
	PS_CharViewInfo* pkCharInfo = pkSessionGroup->GetCharacter(pkSessionGroup->GetSelectChar());
	if (pkCharInfo)
		MG_MessageCenter::GetSingleton().SendMessage( CreateMsg_PLAYER_LOGIN(pkCharInfo), true);	//必须true

	//pkSessionGroup->SetNetState(NLS_WAIT_ENTER_GAME);
	UI_UIManager::GetSingletonPtr()->SetLoadingProcess(true);
	pkSessionGroup->m_pkGameStageMachine->SetNextState(ST_GAME_STAGE_LOADING);
	pkSessionGroup->m_pkGameStageMachine->Update(0.0f,0.0f);
}
//----------------------------------------------------------------------------
void SC_ClientSkillInterface::EndVideo(int iVideoType)
{
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->EndVideo(iVideoType);
}
//----------------------------------------------------------------------------
bool SC_ClientSkillInterface::PlayVideo(int iVideoType, const StringType& kVideoName)
{
	EndVideo(iVideoType);

	StringType sStream(CF_DIR_VIDEO);
	sStream += kVideoName;

	StringType sImgSetName("");
	switch(iVideoType)
	{
	case VIDEOTYPE_CHAR_DEMO:
		sImgSetName = "video/char_demo";
		break;
	case VIDEOTYPE_SKILL:
		sImgSetName = "video/skill";
		break;
	case VIDEOTYPE_SKILL_TOOLTIP:
		sImgSetName = "video/skilltips";
		break;
	}
	MG_MessageCenter::GetSingleton().SendMessage( CreateMsg_ADD_PLAY_VIDEO_LOAD_TASK(iVideoType, sImgSetName, sStream, 0), true);	//必须true
	return true;
}
//----------------------------------------------------------------------------
StringType SC_ClientSkillInterface::GetNamePwdFromConfig()
{
	Utility::UT_ConfigFile kConfig;
	if ( kConfig.Load(CLIENT_USER_PROFILE) )
	{
		StringStreamType sStream("");
		sStream << kConfig.GetSetting("Name", "UserAccount", UT_EngineStringUtility::BLANK) << LOGIN_UI_STRING_MARKER;
		sStream << kConfig.GetSetting("RememberName", "UserAccount", "1") << LOGIN_UI_STRING_MARKER;
		return sStream.str();
	}
	return "";
}
//----------------------------------------------------------------------------
void SC_ClientSkillInterface::SaveNamePwdToConfig(const StringType& kName, const StringType& kRemember)
{
	::WritePrivateProfileString( "UserAccount" , "Name" , kName.c_str() , CLIENT_USER_PROFILE );
	::WritePrivateProfileString( "UserAccount" , "RememberName" , kRemember.c_str() , CLIENT_USER_PROFILE );
}
//----------------------------------------------------------------------------
StringType SC_ClientSkillInterface::GetInfoFromConfig(const StringType& strTite, const StringType& strKey)
{
	Utility::UT_ConfigFile kConfig;
	if ( kConfig.Load(CLIENT_USER_PROFILE) )
	{
		StringType kName = kConfig.GetSetting(strKey, strTite, UT_EngineStringUtility::BLANK);
		if (kName == "")
		{
			return "";
		}

		return kName;
	}
	return "";
}
//----------------------------------------------------------------------------
void SC_ClientSkillInterface::SaveInfoToConfig( const StringType& strTite, const StringType& strKey, const StringType& kValue )
{
	::WritePrivateProfileString( strTite.c_str() , strKey.c_str() , kValue.c_str() , CLIENT_USER_PROFILE );
}
//----------------------------------------------------------------------------
bool SC_ClientSkillInterface::GetSuperShopCustomizeTodayPrompt(const StringType& strTitle)
{
	Utility::UT_ConfigFile kConfig;
	if ( kConfig.Load(CLIENT_USER_PROFILE) )
	{
		return kConfig.GetIntSetting(strTitle, "Client", 0) == 0;
	}

	return false;
}
//----------------------------------------------------------------------------
StringType SC_ClientSkillInterface::GetLoginServerIDList(int iZoneID)
{
	NW_ClientSessionManager* pkSessionManager = NW_ClientSessionManager::GetSingletonPtr();
	PS_ServerInfoList& rkServerList = pkSessionManager->GetLoginServerInfoList(iZoneID);

	//将服务器ID组织成字符串的形式返回
	StringStreamType kStream;
	kStream.str("");
	for (PS_ServerInfoListIter iter = rkServerList.begin();iter != rkServerList.end(); ++iter)
	{
		int iServerID = (*iter).m_iServerID;
		kStream << iServerID << LOGIN_UI_STRING_MARKER;
	}
	return kStream.str();
}

//----------------------------------------------------------------------------
StringType SC_ClientSkillInterface::GetLoginZoneIDList()
{
	NW_ClientSessionManager* pkSessionManager = NW_ClientSessionManager::GetSingletonPtr();
	PS_ServerInfoList& rkServerList = pkSessionManager->GetLoginZoneInfoList();

	//将服务器ID组织成字符串的形式返回
	StringStreamType kStream;
	kStream.str("");
	for (PS_ServerInfoListIter iter = rkServerList.begin();iter != rkServerList.end(); ++iter)
	{
		int iServerID = (*iter).m_iServerID;
		kStream << iServerID << LOGIN_UI_STRING_MARKER;
	}
	return kStream.str();
}
//----------------------------------------------------------------------------
StringType SC_ClientSkillInterface::GetLoginServerInfo( int iZoneID, int iIndex )
{
	NW_ClientSessionManager* pkSessionManager	= NW_ClientSessionManager::GetSingletonPtr();
	PS_ServerInfoList& rkServerList = pkSessionManager->GetLoginServerInfoList(iZoneID);

	if(iIndex < 0 || iIndex >= rkServerList.size())
		return "";

	PS_GameSvrInfo& rkServerInfo = rkServerList[iIndex];
	
	//将该LoginServer的信息拼接成字符串，传入脚本
	StringStreamType kStream;
	kStream << rkServerInfo.m_iServerID << "_"
		<< rkServerInfo.m_szName << "_"
		<< rkServerInfo.m_iUserCount << "_"
		<< rkServerInfo.m_iCapacity << "_"
		<< (int)rkServerInfo.m_cStatus << "_"
		<< 0 << "_"
		<< rkServerInfo.m_iWaitCount << "_" 
		<< 0 << "_";	//注意最后必须添加一个"_"，便于lua中解析
	
	return kStream.str();
}
//----------------------------------------------------------------------------
StringType SC_ClientSkillInterface::GetLoginZoneInfo( int iZoneID )
{
	NW_ClientSessionManager* pkSessionManager	= NW_ClientSessionManager::GetSingletonPtr();
	PS_GameSvrInfo* pkServerInfo			= pkSessionManager->GetLoginZoneInfo(iZoneID);
	if(!pkServerInfo)
		return "";

	//将该LoginServer的信息拼接成字符串，传入脚本
	StringStreamType kStream;
	kStream.str("");
	kStream << pkServerInfo->m_iServerID << "_"
		<< pkServerInfo->m_szName << "_"
		<< pkServerInfo->m_iUserCount << "_"
		<< pkServerInfo->m_iCapacity << "_"
		<< (int)pkServerInfo->m_cStatus << "_"
		<< 0 << "_"
		<< pkServerInfo->m_iWaitCount << "_" 
		<< 0 << "_";	//注意最后必须添加一个"_"，便于lua中解析

	return kStream.str();
}
//----------------------------------------------------------------------------
int SC_ClientSkillInterface::GetGameServerCount()
{
	NW_ClientSessionManager* pkSessionManager = NW_ClientSessionManager::GetSingletonPtr();
	if(!pkSessionManager)
		return 0;

	return pkSessionManager->GetGameServerInfoList().size();
}
//----------------------------------------------------------------------------
StringType SC_ClientSkillInterface::GetGameServerList(bool bIncludeOffServer)
{
	NW_ClientSessionManager* pkSessionManager = NW_ClientSessionManager::GetSingletonPtr();
	PS_ServerInfoList& rkServerList = pkSessionManager->GetGameServerInfoList();
	int iLoginType = pkSessionManager->GetLoginType();

	//将服务器ID组织成字符串的形式返回
	StringStreamType kStream;
	kStream.str("");
	for (PS_ServerInfoListIter iter = rkServerList.begin();iter != rkServerList.end(); ++iter)
	{
		PS_GameSvrInfo& rkServerInfo = (*iter);
		
		if(rkServerInfo.m_iType == SERVER_TYPE_GAME )
		{
			int iServerID = rkServerInfo.m_iServerID;
			if(bIncludeOffServer ||  rkServerInfo.m_cStatus == SERVER_STATUS_ON)
			{				
				kStream << iServerID << LOGIN_UI_STRING_MARKER;

				for(std::vector<uint16_t>::const_iterator it = rkServerInfo.m_kSideline.begin(); it != rkServerInfo.m_kSideline.end(); ++it)
				{
					if((*it) != iServerID)
					{
						kStream << (*it) << LOGIN_UI_STRING_MARKER;
					}
				}
			}
		}
	}
	return kStream.str();
}
//----------------------------------------------------------------------------
StringType SC_ClientSkillInterface::GetGameServerInfo( int iServerID )
{
	NW_ClientSessionManager* pkSessionManager = NW_ClientSessionManager::GetSingletonPtr();
	const PS_GameSvrInfo* pkServerInfo		  = pkSessionManager->GetGameServerInfo(iServerID);

	StringStreamType kStream("");
	if(!pkServerInfo)
		return "";

	//将该GameServer的信息拼接成字符串，传入脚本
	kStream << iServerID << "_";
	kStream << pkServerInfo->m_szName;
	if(iServerID != pkServerInfo->m_iServerID)
		kStream << iServerID;
	kStream << "_" << pkServerInfo->m_iUserCount << "_"
		<< pkServerInfo->m_iCapacity << "_"
		<< (int)(pkServerInfo->m_cStatus) << "_"
		<< 0 << "_"
		<< 0 << "_"
		<< pkServerInfo->m_iGroupID << "_";	//注意最后必须添加一个"_"，便于lua中解析

	return kStream.str();
}
//----------------------------------------------------------------------------
StringType SC_ClientSkillInterface::GetGameServerName( int iServerID )
{
	NW_ClientSessionManager* pkSessionManager = NW_ClientSessionManager::GetSingletonPtr();
	const PS_GameSvrInfo* pkServerInfo		  = pkSessionManager->GetGameServerInfo(iServerID);
	if(!pkServerInfo)
	{
		return "";
	}
	return STDSTR2TPSTR(pkServerInfo->m_szName);
}
//----------------------------------------------------------------------------
int SC_ClientSkillInterface::GetCurGameServer()
{
	NW_ClientSessionManager* pkSessionManager	= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup* pkSessionGroup		= pkSessionManager->GetGameSessionGroup();
	return	pkSessionGroup->GetCurGameServer();
}
//----------------------------------------------------------------------------
int SC_ClientSkillInterface::GetCurGameServerNode()
{
	NW_ClientSessionManager* pkSessionManager	= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup* pkSessionGroup		= pkSessionManager->GetGameSessionGroup();
	return	pkSessionGroup->GetCurGameServerNode();
}
//----------------------------------------------------------------------------
int SC_ClientSkillInterface::GetUserCharCount()
{
	return MAX_USER_CHAR_COUNT;
}
//----------------------------------------------------------------------------
StringType SC_ClientSkillInterface::GetCurCharList()
{
	NW_ClientSessionManager* pkSessionManager	= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup* pkSessionGroup		= pkSessionManager->GetGameSessionGroup();

	//获取角色列表
	PS_CharBaseInfoList& rkCharList				= pkSessionGroup->GetCharList();
	if(rkCharList.size() == 0)
		return "";

	//将角色ID组织成字符串的形式返回
	StringStreamType kStringStream;
	for (PS_CharBaseInfoList::const_iterator iter = rkCharList.begin();
		iter != rkCharList.end(); ++iter)
	{
		unique_id_type iCharID = (*iter).m_uiCharID;
		kStringStream << iCharID << LOGIN_UI_STRING_MARKER;
	}
	return kStringStream.str();
}
//----------------------------------------------------------------------------
StringType SC_ClientSkillInterface::GetCharInfo( unique_id_type iCharID )
{
	NW_ClientSessionManager* pkSessionManager	= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup* pkSessionGroup		= pkSessionManager->GetGameSessionGroup();

	//获取角色
	PS_CharViewInfo* pkCharInfo					= pkSessionGroup->GetCharacter(iCharID);
	if (NULL == pkCharInfo)
	{
		return "";
	}

	int iDeleteTime = pkCharInfo->m_iDeleteTime;
	int lRemainTime = 0;
	//如果角色没有被删除
	if (iDeleteTime <= 0)
	{
		lRemainTime = -1;
	}
	else	//如果角色被删除，则计算删除剩余时间
	{
		time_t ttCurTime;
		time(&ttCurTime);
		//删除期限，以天为单位
		lRemainTime = DELETE_CHAR_TIME_LIMIT - ((int)ttCurTime - iDeleteTime) / 3600 / 24;
	}

	int lServerID = pkCharInfo->m_iCitizenship;

	StringStreamType kStringStream;
	kStringStream << GEN_UNIQUE_ID(getYunYingplatform(), lServerID,pkCharInfo->m_uiCharID) << LOGIN_UI_STRING_MARKER
		<< (int)pkCharInfo->m_byCharType << LOGIN_UI_STRING_MARKER
		<< pkCharInfo->m_szCharName << LOGIN_UI_STRING_MARKER 
		<< (int)pkCharInfo->m_byWeapon << LOGIN_UI_STRING_MARKER
		<< (int)pkCharInfo->m_usCharLevel << LOGIN_UI_STRING_MARKER 
		<< LOGIN_UI_STRING_MARKER
		<< lRemainTime << LOGIN_UI_STRING_MARKER
	    << lServerID << LOGIN_UI_STRING_MARKER
		<< pkCharInfo->m_LastMapResID << LOGIN_UI_STRING_MARKER
		<< pkCharInfo->m_curMapResID << LOGIN_UI_STRING_MARKER;

	return kStringStream.str();
}
////----------------------------------------------------------------------------
StringType SC_ClientSkillInterface::GetParamForCreateChar()
{
	StringStreamType kStringStream;
	CF_CreateCharParam::DataEntryMapExternIterator kIter = CF_CreateCharParam::GetSingleton().GetIterator();
	while(kIter.HasMoreElements())
	{
		const CF_CreateCharParam::DataEntry* pkData = kIter.PeekNextValuePtr();
		kStringStream << pkData->_iID << "#" << pkData->_Paixu << "#" << pkData->_Sex << "#";
		kStringStream << pkData->_kHead << "#" << pkData->_kBody << "#" << pkData->_kFoot << "#" << pkData->_kWeapon << "|";
		kIter.MoveNext();
	}
	
	return kStringStream.str();
}
//----------------------------------------------------------------------------
void SC_ClientSkillInterface::RotateChar(  int lCharType,int iAvatarIndex,float fAngle )
{
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	T_ASSERT(NULL != pkUIManager);
	UI_AvatarManager* pkAvatarManager = UI_AvatarManager::GetSingletonPtr();
	if (NULL != pkAvatarManager)
	{
		pkAvatarManager->RotatePlayerAvatar(lCharType,iAvatarIndex,fAngle);
	}
}

void SC_ClientSkillInterface::RotateAvatarModel(long lCharType, int iIndex, int iCameraMode, float fRotAngle)
{
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	T_ASSERT(NULL != pkUIManager);
	UI_AvatarManager* pkAvatarManager = UI_AvatarManager::GetSingletonPtr();
	if (NULL != pkAvatarManager)
	{
		pkAvatarManager->RotateAvatar(lCharType, iIndex, iCameraMode, fRotAngle);
	}
}
void SC_ClientSkillInterface::SetModelRotation(long lCharType, int iIndex, int iCameraMode, float fRotAngleX, float fRotAngleY, float fRotAngleZ)
{
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	T_ASSERT(NULL != pkUIManager);
	UI_AvatarManager* pkAvatarManager = UI_AvatarManager::GetSingletonPtr();
	if (NULL != pkAvatarManager)
	{
		pkAvatarManager->SetAvatarRotation(lCharType, iIndex, iCameraMode, fRotAngleX, fRotAngleY, fRotAngleZ);
	}
}
//----------------------------------------------------------------------------

void SC_ClientSkillInterface::SetShaderConstF(const StringType& strCharName, float v)
{
	NiShaderFactory::UpdateGlobalShaderConstant(strCharName.c_str(), sizeof(float), &v);
}

float SC_ClientSkillInterface::GetShaderConstF(const StringType& strCharName)
{
	const void* pData = NULL;
	unsigned int vSize = 0;
	NiShaderFactory::RetrieveGlobalShaderConstant(strCharName.c_str(), vSize, pData);
	if (pData)
	{
		float v = *((float *)pData);
		return v;
	}
	return 0;
}
//----------------------------------------------------------------------------

bool SC_ClientSkillInterface::UpdateAvatarBloomParams(int lCharType)
{
	CF_Buddy::DataEntry* pkData = CF_Buddy::GetSingleton().GetEntryPtr(lCharType);
	if (pkData)
	{
		const Utility::UT_Vec4Float & kParam = pkData->_kAvatarBloomParam;

		NiShaderFactory::UpdateGlobalShaderConstant("g_fAvatarSharp", sizeof(float), &kParam.x);
		NiShaderFactory::UpdateGlobalShaderConstant("g_fAvatarBlur" , sizeof(float), &kParam.y);
		NiShaderFactory::UpdateGlobalShaderConstant("g_fAvatarExp"  , sizeof(float), &kParam.z);
		NiShaderFactory::UpdateGlobalShaderConstant("g_fAvatarPower", sizeof(float), &kParam.w);

		return true;
	}
	return false;
}

//----------------------------------------------------------------------------
void SC_ClientSkillInterface::DeteachAllAttachFxAnim( GameLogic::LC_ActorBase* pkActor )
{
	if (IS_NPC_ID(pkActor->GetID()))
	{
		LC_ClientNPC* pkNPC = (LC_ClientNPC*)pkActor;
		GD_ClientActorModel* pkVisibleData = IM_Manager::GetSingleton().GetActor(pkNPC->GetID());
		if (pkVisibleData)
		{
			pkVisibleData->DoDetachFxsOnAnim();
		}
	}
}

int SC_ClientSkillInterface::GetMaxValidLevelInVIPProfit(int iType)
{
	CF_VIPProfit::DataEntry *pkVIPProfit = SafeGetCSVFileDataEntryPtr<CF_VIPProfit>(iType);
	if (pkVIPProfit)
	{
		int profits[15]={
			pkVIPProfit->_lVIP1Value,
			pkVIPProfit->_lVIP2Value,
			pkVIPProfit->_lVIP3Value,
			pkVIPProfit->_lVIP4Value,
			pkVIPProfit->_lVIP5Value,
			pkVIPProfit->_lVIP6Value,
			pkVIPProfit->_lVIP7Value,
			pkVIPProfit->_lVIP8Value,
			pkVIPProfit->_lVIP9Value,
			pkVIPProfit->_lVIP10Value,
			pkVIPProfit->_lVIP11Value,
			pkVIPProfit->_lVIP12Value,
			pkVIPProfit->_lVIP13Value,
			pkVIPProfit->_lVIP14Value,
			pkVIPProfit->_lVIP15Value
		};
		int profitValue=profits[14];
		for (int i=0;i<=14;i++)
		{
			if (profitValue == profits[i])
			{
				return i+1;
			}
		}
	}
	return 0;
}

//----------------------------------------------------------------------------
void SC_ClientSkillInterface::SendReqServerListMsg()
{
	NW_ClientSessionManager* pkSessionManager	= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup* pkSessionGroup		= pkSessionManager->GetGameSessionGroup();
	NW_Login2ClientSession* pkLoginSession		= pkSessionGroup->m_pkLoginSession;

	{
		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = false;

		bCreateMsg = CreateMsgPacketHandler(pkSessionGroup, CRYPT_ALGORITHM_NONE, hMsgHandler, MGPT_LC_REQ_SERVER_LIST, NULL);

		if (bCreateMsg)
			pkLoginSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}
//----------------------------------------------------------------------------
bool SC_ClientSkillInterface::WorldMapLoaded()
{

    return true;
}
//----------------------------------------------------------------------------
void SC_ClientSkillInterface::ConfirmAction(int iActionType, int lParam)
{
	NW_ClientSessionManager* pkSessionManager	= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup* pkSessionGroup		= pkSessionManager->GetGameSessionGroup();
	NW_Login2ClientSession* pkLoginSession		= pkSessionGroup->m_pkLoginSession;
	DWORD dwUserID								= pkSessionGroup->m_dwUserID;
	int iCharID = lParam;

	CharManageActionType eActionType = (CharManageActionType)iActionType;
	switch (eActionType)
	{
	case CMAT_DELETE_CHAR:
		{
			MG_LC_Req_DeleteChar kReqDeleteCharMsg;
			kReqDeleteCharMsg.m_dwUserID = dwUserID;
			kReqDeleteCharMsg.m_iCharID = iCharID;

			MG_PACKET_ENCODE_HANDLER hMsgHandler;
			bool bCreateMsg = CreateMsgPacketHandler(pkSessionGroup, CRYPT_ALGORITHM_NONE, hMsgHandler, MGPT_LC_REQ_DELETE_CHAR, &kReqDeleteCharMsg);
			if (bCreateMsg)
				pkLoginSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

			//提示正在请求删除角色
			pkSessionGroup->SetNetState(NLS_WAIT_DELETE_CHAR);
			break;
		}

	case CMAT_RECOVER_CHAR:
		{
			MG_LC_Req_RecoverChar kReqRecoverCharMsg;
			kReqRecoverCharMsg.m_dwUserID = dwUserID;
			kReqRecoverCharMsg.m_iCharID = iCharID;

			MG_PACKET_ENCODE_HANDLER hMsgHander;
			bool bCreateMsg = CreateMsgPacketHandler(pkSessionGroup, CRYPT_ALGORITHM_NONE, hMsgHander, MGPT_LC_REQ_RECOVER_CHAR, &kReqRecoverCharMsg);
			if (bCreateMsg)
				pkLoginSession->Send(hMsgHander.GetBuff(), hMsgHander.GetMsgSize());

			//提示正在恢复角色
			pkSessionGroup->SetNetState(NLS_WAIT_RECOVER_CHAR);
			break;
		}

	default:
		break;
	}
}
//----------------------------------------------------------------------------
void SC_ClientSkillInterface::RequestRespawnOnCurrentLocation(bool bAuto)
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return;

	MG_REQ_RespawnOnCurrentLocation msg;
	msg.m_lPlayerID = pkGamePlayer->GetID();
	msg.m_Auto = bAuto;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_RESPAWN_ON_CURRENT_LOCATION, &msg);
	if (bCreateMsg)
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI, "SC_ClientSkillInterface::RequestRespawnOnCurrentLocation m_lPlayerID=[%d] m_Auto=[%d]", msg.m_lPlayerID, msg.m_Auto);
		pkGamePlayer->GetSession()->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}
//---------------------------------------------------
void SC_ClientSkillInterface::RequestRespawnOnSafeLocation()
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return;

	MG_REQ_RespawnOnSafeLocation msg;
	msg.m_lPlayerID = pkGamePlayer->GetID();

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_RESPAWN_ON_SAFE_LOCATION, &msg);
	if (bCreateMsg)
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI, "SC_ClientSkillInterface::RequestRespawnOnSafeLocation m_lPlayerID=[%d]", msg.m_lPlayerID);
		pkGamePlayer->GetSession()->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}
//---------------------------------------------------
void SC_ClientSkillInterface::RequestRespawnByItem()
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return;
	
	if(pkGamePlayer->CheckUseItemInPackByItemType(RESPAWN_ITEM_TYPE_ID) != RE_SUCCESS)
		return;

	MG_REQ_RespawnByItem msg;
	msg.m_lPlayerID = pkGamePlayer->GetID();

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_RESPAWN_BY_ITEM, &msg);
	if (bCreateMsg)
		pkGamePlayer->GetSession()->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
//---------------------------------------------------
bool SC_ClientSkillInterface::CheckCanRespawnByItem()
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return false;

	if(pkGamePlayer->CheckUseItemInPackByItemType(RESPAWN_ITEM_TYPE_ID) == RE_SUCCESS)
		return true;
	else
		return false;
}
//--------------------------------------------------
bool SC_ClientSkillInterface::CanRespawnOnSafe(int mapResID)
{
	CF_TranscriptionRule* pkTranscriptionRule = CF_TranscriptionRule::GetSingletonPtr();
	CF_TranscriptionRule::DataEntry* pkData = pkTranscriptionRule->GetEntryPtr(mapResID);
	if (!pkData)
		return true;

	return !pkData->_iIsQuitRespawn;
}

StringType SC_ClientSkillInterface::GetEnterMapItemInfo(int mapResID)
{
	LC_ClientGamePlayer* pkGamePlayer = GetPlayerPtr();
	if(pkGamePlayer == NULL)
		return "";

	const CF_TranscriptionRule::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_TranscriptionRule>(mapResID);
	if (pkData != NULL)
	{
		LC_PackAsset* pkPackAsset = pkGamePlayer->GetPackAsset();
		UT_SIMDataList::const_iterator iter = pkData->_kConsumeItems.begin();
		if(iter != pkData->_kConsumeItems.end())
		{
			const UT_SIMDataInfo& info = *iter;
			StringType sItems;
			UT_BaseStringUtility::format(sItems, "%d#%d", info.ID(), info.ParamA());
			return sItems;
		}
	}

	return "";
}

long SC_ClientSkillInterface::GetRespawnProtectTime(int mapResID)
{
	CF_WorldMapList::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(mapResID);
	if(NULL == pkEntry)
		return 0;

	return pkEntry->_iRespawnSafeTime;
}
//---------------------------------------------------
void SC_ClientSkillInterface::RobotRequestRespawnOnCurrentLocation(GameLogic::LC_ActorBase* pkMachineOwner)
{
	LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkMachineOwner;

	if(!pkGamePlayer)
		return;

	MG_REQ_RespawnOnCurrentLocation msg;
	msg.m_lPlayerID = pkGamePlayer->GetID();

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_RESPAWN_ON_CURRENT_LOCATION, &msg);
	if (bCreateMsg)
		pkGamePlayer->GetSession()->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
//---------------------------------------------------
void SC_ClientSkillInterface::Change2NextPos(int nPosX, int nPosY, int nPosZ)
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return;

	Utility::UT_Vec3Int kCurrentLocation;
	kCurrentLocation.x = nPosX;
	kCurrentLocation.y = nPosY;
	kCurrentLocation.z = nPosZ;

	pkGamePlayer->SetCurrentLocation(kCurrentLocation);
	pkGamePlayer->SetTargetLocation(kCurrentLocation);
}
//------------------------------------------------------------------------------
void SC_ClientSkillInterface::StampASpecialBubble(GameLogic::LC_ActorBase* pkActor, const StringType& kBubbleMsg, int lShownTime)
{
	if (!pkActor)
	{
		return;
	}
	/*if (IS_MONSTER_ID(pkActor->GetID()))
	{
		LC_ClientMonster* pkMonster = (LC_ClientMonster*) pkActor;
		pkMonster->ShowMonsterTalkingBubble(kBubbleMsg, lShownTime);
	}
	else if (IS_NPC_ID(pkActor->GetID()))*/
	{
		LC_ClientNPC* pkNPC = (LC_ClientNPC*)pkActor;
		pkNPC->ShowNPCTalkingBubble(kBubbleMsg, lShownTime);
	}
}
//------------------------------------------------------------------------------
void SC_ClientSkillInterface::UpdateUIScreenPosByLogicObject(WT_Widget* pkWidget,uint64_t lLogicObjectID,int iXOffset,int iYOffset,int iWidth,int iHeight)
{
	if(!pkWidget)
		return;

	NiPoint3 ScreenPos(0.0f, 0.0f, 0.0f);
	NiPoint3 kPos;
	bool	 bResult = false;

	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_PlayerBase* pkObject = pkLogicManger->GetOperationPlayer();
	if(!pkObject || pkObject->GetInstance() != lLogicObjectID)
	{
		LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
		pkObject = pkNetPlayerManager->FindPlayer(lLogicObjectID);
	}
	
	if(pkObject != NULL)
	{
		kPos = UT_EngineMath::ConvertVec3IntToNiPoint3(pkObject->GetVisibleLocation());
		
		if(IS_PLAYER_CLASS_ID(lLogicObjectID) || IS_NPC_ID(lLogicObjectID))
		{
			LC_ActorBase* pkActor = (LC_ActorBase*)pkObject;
			kPos.z += Int2Float(pkActor->GetProperty_ShapeHeight());
		}
			
		NiCameraPtr spCamera = NULL;
		NiRenderer* pkRenderer = NiRenderer::GetRenderer();
		const NiRenderTargetGroup* pkRTGroup = pkRenderer->
			GetDefaultRenderTargetGroup();
		GUISys::GUIRect kScreenBounds(0, 0,(float)pkRTGroup->GetWidth(0),(float)pkRTGroup->GetHeight(0));
		if(CA_CameraManager::GetSingletonPtr() != NULL)
		{
			spCamera = CA_CameraManager::GetSingletonPtr()->GetNiCamera();
			bResult = spCamera->WorldPtToScreenPt3(kPos,ScreenPos.x,ScreenPos.y,ScreenPos.z);
			if( bResult )
			{
				ScreenPos.x = ScreenPos.x * kScreenBounds.GetWidth();
				ScreenPos.y = (1 - ScreenPos.y) * kScreenBounds.GetHeight();
			}
		}	
	}
	if(0 == UT_BaseStringUtility::StrCompare(pkWidget->GetRTTIName(),"CL_CUIBubble"))
	{
		CL_CUIBubble* pkBubbleWidget = (CL_CUIBubble*)pkWidget;
		if(iWidth != 0 && iHeight != 0)
			pkBubbleWidget->SetTargetRect(ScreenPos.x + iXOffset, 
				ScreenPos.y + iYOffset - pkWidget->GetHeight() ,iWidth,iHeight  );
	}
}
//---------------------------------------------------
 long SC_ClientSkillInterface::GetSkillUpGradeNeedCash(long lSkillID)
{
	CF_SkillLevelUp::DataEntry *pkSkill = SafeGetCSVFileDataEntryPtr<CF_SkillLevelUp>(lSkillID);
	if(!pkSkill)
		return 0;

	return pkSkill->_lNeedCash;
}

 int SC_ClientSkillInterface::GetSkillUpgradeNeedBuddyStar(long lSkillID)
 {
	 CF_SkillLevelUp::DataEntry *pkSkill = SafeGetCSVFileDataEntryPtr<CF_SkillLevelUp>(lSkillID);
	 if(!pkSkill)
		 return -1;

	 return pkSkill->_level;
 }

bool SC_ClientSkillInterface::GetSkillCanUpGrade(int lSkillID, int iCID)
{
	LC_ClientGamePlayer* pkGamePlayer = GetPlayerPtr();
	if(!pkGamePlayer)
		return false;
	
	LC_SkillAsset* pkSkillAsset = pkGamePlayer->GetBuddySkillAsset(iCID);
	if (!pkSkillAsset)
		return false;


	LC_SkillAssetEntry *pkSkillAssetEntry = pkSkillAsset->FindLearnedSkill(lSkillID,false);
	if (!pkSkillAssetEntry)
		return false;

	LC_PackAsset* pkPackAsset = pkGamePlayer->GetPackAsset();

	int iCurrentSkillID = pkSkillAssetEntry->GetTypeID();
	CF_SkillLevelUp* pkCSVSkillLevelUp = CF_SkillLevelUp::GetSingletonPtr();
	CF_SkillLevelUp::DataEntry* pkData = pkCSVSkillLevelUp->GetEntryPtr(iCurrentSkillID);
	if(!pkData)
		return false;

	if(pkData->_lNextLevelSkillTypeID <= 0)
		return false;

	if(pkData->_lNeedLevel > pkGamePlayer->GetBuddyLevel(iCID))
		return false;

	if (!pkGamePlayer->IsCashEnoughForSkillUpgrade(iCurrentSkillID))
		return false;

	if(!pkGamePlayer->IsSPEnoughForSkillUpgrade(iCurrentSkillID))
		return false;

	if (!pkData->_kNeedItems.empty())
	{
		Utility::UT_SIMDataList::iterator itItem = pkData->_kNeedItems.begin();
		for (; itItem!=pkData->_kNeedItems.end(); ++itItem)
		{
			if (pkGamePlayer->GetCanCostItemCount(itItem->ID()) < itItem->ItemCount())
			{
				return false;
			}
		}
	}
	
	if (!pkData->_kNeedTasks.empty())
	{
		LC_TaskMap* pkTaskMap = pkGamePlayer->GetTaskMap();
		if (pkTaskMap)
		{
			Utility::UT_SSIMDataList::iterator itTask = pkData->_kNeedTasks.begin();
			for (; itTask!=pkData->_kNeedTasks.end(); ++itTask)
			{
				if (!pkTaskMap->IsInFinishedTaskMap(itTask->ID()))
				{
					return false;
				}
			}
		}
	}

	if(pkData->_level > pkGamePlayer->GetBuddyStarLevel(iCID))
		return false;

	return true;
}

bool SC_ClientSkillInterface::GetSkillCanWakeUp(int lSkillID, int iCID)
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return false;

	LC_SkillAsset* pkSkillAsset = pkGamePlayer->GetBuddySkillAsset(iCID);
	if (!pkSkillAsset)
		return false;
	
	LC_PackAsset* pkPackAsset = pkGamePlayer->GetPackAsset();
	if (!pkPackAsset)
		return false;

	// 1.技能需要已解锁
	LC_SkillAssetEntry *pkSkillAssetEntry = pkSkillAsset->FindLearnedSkill(lSkillID,false);
	if (!pkSkillAssetEntry)
		return false;

	if(pkGamePlayer->CheckBuddyActivateSkill(iCID,lSkillID))
		return false;

	//3.背包中拥有此解锁的道具
	int ItemID=GetActiveSkillItemID(lSkillID);
	if (pkGamePlayer->GetCanCostItemCount(ItemID) <= 0 )
	{
		return false;
	}

	return true;
}

int SC_ClientSkillInterface::CheckActivateSkill(GameLogic::LC_Attacker* pkScriptCaller, int lSkillTypeID)
{
	int result = -1;
	if (NULL == pkScriptCaller)
		return result;
	if (!IS_PLAYER_CLASS_ID(pkScriptCaller->GetID()))
		return result;
	
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if (!pkGamePlayer)
		return result;

	LC_ActorBase* player = dynamic_cast<LC_ActorBase*>(pkScriptCaller);
	if (player)
	{
		if (player->GetID() == pkGamePlayer->GetID())
		{
			result = pkGamePlayer->CheckBuddyActivateSkill(pkGamePlayer->GetCharType(), lSkillTypeID);
		}
		else if (pkGamePlayer->IsBuddy(pkScriptCaller->GetID()))
		{
			result = pkGamePlayer->CheckBuddyActivateSkill(player->GetCharType(), lSkillTypeID);
		}
		else
		{
			LC_ClientNetPlayer* netplayer = dynamic_cast<LC_ClientNetPlayer*>(player);
			if (netplayer)
			{
				result = netplayer->CheckActivateSkill(lSkillTypeID);
			}
		}
	}
	
	return result;
}

void SC_ClientSkillInterface::NotifyTreasureSkillInterrupt()
{
	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_NOTIFY_TREASURESKILL_INTERRUPT);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

//---------------------------------------------------
NPCLevelType SC_ClientSkillInterface::GetNPCLevelType(object_id_type lNPCID)
{
	if(!IS_NPC_ID(lNPCID))
		return NLT_NONE;

	LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
	LC_NPCBase* pkNPC = pkNPCManager->FindNPC(lNPCID);
	if(!pkNPC)
		return NLT_NONE;

	return LC_Helper::GetNPCLevelTypeByCharTypeID(pkNPC->GetCharType());
}

NPCLevelType SC_ClientSkillInterface::GetNPCLevelTypeByCharType(int iCharType)
{
	return LC_Helper::GetNPCLevelTypeByCharTypeID(iCharType);
}
//---------------------------------------------------
NPCEliteType SC_ClientSkillInterface::GetNPCEliteType(object_id_type lNpcID)
{
	if(!IS_NPC_ID(lNpcID))
	{
		return ELT_NONE;
	}

	LC_ClientNPCManager *pkNPCManager = CLIENT_GET_NPC_MANAGER();
	LC_NPCBase *pkNPC = pkNPCManager->FindNPC(lNpcID);
	if(NULL == pkNPC)
	{
		return ELT_NONE;
	}

	return (NPCEliteType)(pkNPC->GetEliteLevel());
}
//---------------------------------------------------
void SC_ClientSkillInterface::ChangeLockedTarget()
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return;

	//pkGamePlayer->ManualChangeLockedTarget();
	pkGamePlayer->ManualChangeLockedTargetFunc1();

	/*
	LC_MapBase* pkMap = pkGamePlayer->GetMap();
	if(!pkMap)
	{
		GfxWriteLog(LOG_WARNING_FILELINE, LOG_SWITCH_WARNING,"脚本:试图用非法目标位置作区域锁定");
		return;
	}

	CF_ActorFilterFunc* pkCSVActorFilterFunc = CF_ActorFilterFunc::GetSingletonPtr();
	CF_ActorFilterFunc::DataEntry* pkData = NULL;
	//排除当前锁定目标和自身
	pkData = pkCSVActorFilterFunc->GetEntryPtr(88);
	
	if (NULL == pkData)
	{
		return;
	}

	//从地图返回区域内的Actor队列
	UT_Circle kCircle;
	kCircle.iRadius = 1000;
	kCircle.kCenterPoint = pkGamePlayer->GetCurrentLocation();
	LC_ActorPtrList kLockResult = pkMap->LockActorsInCircle(kCircle,pkData);

	//根据距离对目标进行排序
	UT_Vec3Int kPlayerLocation = pkGamePlayer->GetCurrentLocation();
	kLockResult.sort(lessThan(kPlayerLocation));

	//根据过滤设置,过滤Actor队列
	object_id_vector kFilterResult = LC_Helper::FilterActors(pkGamePlayer,kLockResult,pkData);

	if(kFilterResult.size() == 0)
		return;

	////随机扰动队列顺序
	//{
	//	float fRandom = UT_MathBase::RandFloat01();
	//	_randomTargetVector(kFilterResult,fRandom);
	//}
	//object_id_type lTargetID = kFilterResult[0];
	//object_id_type lCurrentWatchedID =  pkGamePlayer->GetWatchTargetID();
	//if(lCurrentWatchedID == lTargetID)
	//{
	//	if(kFilterResult.size() == 1)
	//		return;
	//	else
	//		lTargetID = kFilterResult[1];
	//}

	//pkGamePlayer->WatchTarget(lTargetID);

	//使用静态变量记录目标列表和更新时间
	static object_id_type s_alEnemyList[SHORT_CUT_ENEMY_COUNT] = {0, 0, 0, 0, 0};
	static int s_iSelectEnemyIndex = 0;
	static int s_lUpdateEnemyListTime = 0;
	_selectWatchedTarget(s_alEnemyList, SHORT_CUT_ENEMY_COUNT, s_iSelectEnemyIndex, s_lUpdateEnemyListTime, kFilterResult);
	object_id_type lSelectTargetID = s_alEnemyList[s_iSelectEnemyIndex];
	if ( (!IS_PLAYER_CLASS_ID(lSelectTargetID) && !IS_NPC_ID(lSelectTargetID)) 
		|| pkGamePlayer->GetLockedTargetID() == lSelectTargetID )
	{
		return;
	}
	pkGamePlayer->SetLockedTargetID(lSelectTargetID);
	*/
}
//---------------------------------------------------
//void SC_ClientSkillInterface::RequestLevelUp()
//{
//	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
//	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
//	if(!pkGamePlayer)
//		return;
//
//	if(pkGamePlayer->GetActionForbid()->IsActionForbid(LAFT_GAME,LAF_GAME_PLAYER_LEVEL_UP))
//	{
//		SendLogicInfoMessage(RE_ACTION_FORBID);
//		return;
//	}
//
//	MG_REQ_LevelUp reqMsg;
//	//reqMsg.m_lPlayerID = pkGamePlayer->GetID();
//
//	MG_PACKET_ENCODE_HANDLER hMsgHandler;
//	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_LEVEL_UP, &reqMsg);
//	if (bCreateMsg)
//		pkGamePlayer->GetSession()->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
//}
//------------------------------------------------------------
const StringType SC_ClientSkillInterface::GetNormalAttackAnimName(GameLogic::LC_Attacker* pkScriptCaller,int iIndex)
{
	char name[64];
	LC_PlayerBase* pkAttacker = (LC_PlayerBase*)pkScriptCaller;

	StringType	a = LC_ClientHelp::GetWeaponedResName(pkAttacker->GetCurrentBandWeapon(), RES_ANM_ACTOR_ATTACK);
	sprintf(name, "%d", iIndex);
	a += name;

	return a;
}
//---------------------------------------------------
void SC_ClientSkillInterface::PlayTargetAnimation(GameLogic::LC_Attacker* pkScriptCaller,int iTargetIndex,const StringType& rkAnimName)
{
	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller,iTargetIndex, kTargetsList);
	LC_ActorPtrListIter IterBe = kTargetsList.begin();
	LC_ActorPtrListIter IterEd = kTargetsList.end();
	LC_ActorBase* pkTarget = NULL;
	object_id_type lTargetID;

	for(; IterBe != IterEd; ++IterBe)
	{
		pkTarget = (*IterBe);
		lTargetID = pkTarget->GetID();
		pkTarget->PlayAnimation(rkAnimName,true);
		//GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"播放动画 %s",rkAnimName.c_str());
	}
}
//---------------------------------------------------
void SC_ClientSkillInterface::PlayTargetAnimationFreq(GameLogic::LC_Attacker* pkScriptCaller,int iTargetIndex,const StringType& rkAnimName, float fSequency)
{
	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller,iTargetIndex, kTargetsList);
	LC_ActorPtrListIter IterBe = kTargetsList.begin();
	LC_ActorPtrListIter IterEd = kTargetsList.end();
	LC_ActorBase* pkTarget = NULL;
	object_id_type lTargetID;

	for(; IterBe != IterEd; ++IterBe)
	{
		pkTarget = (*IterBe);
		lTargetID = pkTarget->GetID();
		pkTarget->PlayAnimation(rkAnimName,true,fSequency);
		//GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"播放动画 %s",rkAnimName.c_str());
	}
}
//---------------------------------------------------
void SC_ClientSkillInterface::PlayTargetWeaponAnimation(GameLogic::LC_Attacker* pkScriptCaller,int iTargetIndex,const StringType& rkAnimName)
{
	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller,iTargetIndex, kTargetsList);
	LC_ActorPtrListIter IterBe = kTargetsList.begin();
	LC_ActorPtrListIter IterEd = kTargetsList.end();
	LC_ActorBase* pkTarget = NULL;
	object_id_type lTargetID;

	for(; IterBe != IterEd; ++IterBe)
	{
		pkTarget = (*IterBe);
		lTargetID = pkTarget->GetID();
		pkTarget->PlayWeaponAnimation(rkAnimName);
	}
}
//---------------------------------------------------
void SC_ClientSkillInterface::PlayManPaiAnimation(GameLogic::LC_Attacker* pkScriptCaller,int iTargetIndex)
{
	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller,iTargetIndex, kTargetsList);
	LC_ActorPtrListIter IterBe = kTargetsList.begin();
	LC_ActorPtrListIter IterEd = kTargetsList.end();
	LC_ActorBase* pkTarget = NULL;
	object_id_type lTargetID;

	for(; IterBe != IterEd; ++IterBe)
	{
		pkTarget = (*IterBe);
		lTargetID = pkTarget->GetID();
		if ( IS_PLAYER_CLASS_ID(lTargetID) )
		{
			LC_PlayerBase* pkPlayer = (LC_PlayerBase*)pkTarget;
			if ( pkPlayer->GetRiderCharType() == 0 )
			{
				if ( pkPlayer->IsArtifactWeapon() )
				{
					if ( pkPlayer->IsArtifactWeaponIndex(GOD_ITEM_4) )
						pkPlayer->PlayAnimation(RES_ANIM_ARTIFACTS4_IDLE_ATK);	
					else if ( pkPlayer->IsArtifactWeaponIndex(GOD_ITEM_5) )
						pkPlayer->PlayAnimation(RES_ANIM_ARTIFACTS5_IDLE_ATK);
					else
						pkPlayer->PlayAnimation(RES_ANIM_ARTIFACTS_IDLE_ATK);
				}
				else
				{
					StringType resName = GameLogic::LC_ClientHelp::GetWeaponedResName(pkPlayer->GetCurrentBandWeapon(), RES_ANM_ACTOR_IDLE_ATTACKREADY);
					pkPlayer->PlayAnimation(resName);
				}
			}
			else
			{
				if ( pkPlayer->IsEquipVehicleWeapon() )
				{
					pkPlayer->PlayAnimation(RES_ANM_ACTOR_RIDE_IDLE_HAND);
				}
				else
				{
					pkPlayer->PlayAnimation(RES_ANM_ACTOR_RIDE_IDLE_MENPAI);
				}
			}
		}
	}
}
//---------------------------------------------------
void SC_ClientSkillInterface::FaceToTarget(GameLogic::LC_Attacker* pkScriptCaller,int iTargetIndex)
{
	object_id_type lCallerID = pkScriptCaller->GetID();

	/*if(!IS_PLAYER_CLASS_ID(lCallerID) && !IS_NPC_ID(lCallerID) && !IS_LOCAL_NPC_ID(lCallerID) )
		return;*/
	
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	/*if (pkScriptCaller != pkGamePlayer)
		return;*/

	LC_ActorBase* pkActorCaller = (LC_ActorBase*)pkScriptCaller;

	UT_Vec3Int kCurrentLocation = pkActorCaller->GetVisibleLocation();
	UT_Vec3Int kTargetLocation;
	if(iTargetIndex == TARGET_LOCATION)
		kTargetLocation = pkActorCaller->GetLockedLocation();
	else
	{
		LC_ActorPtrList kTargetsList;
		_findTarget(pkScriptCaller,iTargetIndex, kTargetsList);
		//T_ASSERT(kTargetsList.size() == 1);
		if(kTargetsList.size() != 1)
			return;
		LC_LogicObject* pkTarget = kTargetsList.front();
		if(!pkTarget)
			return;
		//如果锁定自身
		if (pkTarget->GetID() == lCallerID)
			return;

		kTargetLocation = pkTarget->GetVisibleLocation();
	}
	LC_Helper::FaceActorToLocation(pkActorCaller,kCurrentLocation,kTargetLocation);
}
//---------------------------------------------------
void SC_ClientSkillInterface::SetTargetLocation(GameLogic::LC_Attacker* pkScriptCaller,int iTargetIndex)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	object_id_type lCallerID = pkScriptCaller->GetID();
	if (!IS_PLAYER_CLASS_ID(lCallerID) && !IS_NPC_ID(lCallerID) && !IS_LOCAL_NPC_ID(lCallerID))
	{
		return;
	}

	LC_ActorBase* pkCaller = (LC_ActorBase*)pkScriptCaller;

	UT_Vec3Int kTargetLocation;
	if(iTargetIndex == TARGET_LOCATION)
		kTargetLocation = pkCaller->GetLockedLocation();
	else
	{
		LC_ActorPtrList kTargetsList;
		_findTarget(pkScriptCaller,iTargetIndex, kTargetsList);
		if(kTargetsList.size() != 1)
			return;
		LC_LogicObject* pkTarget = kTargetsList.front();
		if(!pkTarget)
			return;
		//如果锁定自身
		if (pkTarget->GetID() == lCallerID)
			return;

		kTargetLocation = pkTarget->GetVisibleLocation();
	}

	pkCaller->SetTargetLocation(kTargetLocation);
}
//*****
void SC_ClientSkillInterface::SetLockedLocationByDir(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex,int iFaceDistance)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	object_id_type lCallerID = pkScriptCaller->GetID();
	if (!IS_PLAYER_CLASS_ID(lCallerID) && !IS_NPC_ID(lCallerID))
	{
		return;
	}

	LC_ActorBase* pkCaller = (LC_ActorBase*)pkScriptCaller;
	UT_Vec2Float tarDir	  = pkCaller->GetForwardDir();
	tarDir.Unitize();
	UT_Vec3Int kCurrentLocation = pkCaller->GetCurrentLocation();

	UT_Vec3Int finTargetLocation;
	finTargetLocation.x = kCurrentLocation.x + (int)(tarDir.x * iFaceDistance);
	finTargetLocation.y = kCurrentLocation.y + (int)(tarDir.y * iFaceDistance);
	finTargetLocation.z = kCurrentLocation.z;

	Path::PF_PathManager* pkPathMgr = Path::PF_PathManager::GetSingletonPtr();
	UT_Vec3Int kNewTarget = finTargetLocation;
	if (pkPathMgr&&!pkPathMgr->TestCanMoveTo( finTargetLocation , false ))
	{
		int lSearchStep = 200;
		int lHorizontalRange;
		int lVerticalRange;

		lHorizontalRange = 2000;
		lVerticalRange   = 2000;

		kNewTarget = pkPathMgr->FindClosestMovablePoint( finTargetLocation , 
			lSearchStep , 100 , lHorizontalRange , lVerticalRange ,abs(iFaceDistance) );
		finTargetLocation = (kNewTarget.x!=0 && kNewTarget.y!=0)? kNewTarget: pkScriptCaller->GetCurrentLocation();
	}

	pkCaller->SetLockedLocation(finTargetLocation);
}
//*****
void SC_ClientSkillInterface::SetLockedLocationByMouseLocation(GameLogic::LC_Attacker* pkScriptCaller, bool bLockedTarget)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();

	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();

	SM_ClientGamePlayerController* controller = (SM_ClientGamePlayerController*)pkGamePlayer->GetSMController();
	LC_ActorBase * pkTarget = NULL;
	if ( controller->IsTargetAttackable( pkGamePlayer, pkGamePlayer->GetLockedTargetID() ) )
		pkTarget = LC_Helper::GetLockedTarget(pkGamePlayer);
	else
		pkTarget = LC_Helper::GetLockedTarget((LC_ActorBase*)pkScriptCaller);

	if (pkGamePlayer->IsHooking() 
		&& !(pkTarget && bLockedTarget))
		return;

	if (pkScriptCaller != pkGamePlayer)
	{
		if (pkTarget && bLockedTarget)
		{
			UT_Vec3Int kMouseLocation = pkTarget->GetVisibleLocation();
			pkScriptCaller->SetLockedLocation(kMouseLocation);
		}
	}
	else
	{
		const UT_Vec3Int& curLocation = pkGamePlayer->GetCurrentLocation();
		UT_Vec3Int kMouseLocation = pkTarget && bLockedTarget ? pkTarget->GetVisibleLocation() : UT_EngineMath::ConvertNiPoint3ToVec3Int(pkGamePlayer->GetMouseOverLocation());

		Path::PF_PathManager* pkPathMgr = Path::PF_PathManager::GetSingletonPtr();
		UT_Vec3Int kNewTarget = kMouseLocation;
		if (pkPathMgr&&!pkPathMgr->TestCanMoveTo( kMouseLocation , false ))
		{
			int lSearchStep = 200;
			int lHorizontalRange;
			int lVerticalRange;
			int lMaxDistance;

			lHorizontalRange = 2000;
			lVerticalRange   = 500;
			lMaxDistance     = 500;

			kNewTarget = pkPathMgr->FindClosestMovablePoint( kMouseLocation , 
				lSearchStep , 100 , lHorizontalRange , lVerticalRange , lMaxDistance );
			kMouseLocation = (kNewTarget.x!=0 && kNewTarget.y!=0)? kNewTarget: curLocation;
		}

		pkGamePlayer->SetLockedLocation(kMouseLocation);
	}
}

void SC_ClientSkillInterface::SetLockedLocation(GameLogic::LC_Attacker* pkScriptCaller, int iFaceDistance, float fDeltaAngle)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	object_id_type lCallerID = pkScriptCaller->GetID();
	if (!IS_PLAYER_CLASS_ID(lCallerID) && !IS_NPC_ID(lCallerID) && !IS_LOCAL_NPC_ID(lCallerID) )
	{
		return;
	}

	LC_ActorBase* pkCaller = (LC_ActorBase*)pkScriptCaller;

	UT_Vec3Int kCurrentLocation = pkCaller->GetCurrentLocation();
	UT_Vec2Float kCurrentForward = pkCaller->GetForwardDir();

	if (fDeltaAngle != 0)	//从当前方向偏移多少角度
	{
		Utility::UT_Vec2Float newAt = UT_MathBase::ZRot(kCurrentForward, fDeltaAngle);

		kCurrentForward = newAt;
	}

	UT_Vec3Int kTargetLocation;	//在当前位置按当前方向偏移多少距离
	kTargetLocation.x = kCurrentLocation.x + (int)(kCurrentForward.x * iFaceDistance);
	kTargetLocation.y = kCurrentLocation.y + (int)(kCurrentForward.y * iFaceDistance);
	const SE_SceneTerrainLocalInfo& rkTerrainInfo1 = SE_SceneManager::GetSingletonPtr()->GetWalkableInfo(Int2Float(kTargetLocation.x), Int2Float(kTargetLocation.y));
	kTargetLocation.z		= rkTerrainInfo1._fLocalHeight;

	pkCaller->SetLockedLocation(kTargetLocation);
}

//--------------------------------------------------
void SC_ClientSkillInterface::SetTargetLocationValue(GameLogic::LC_Attacker* pkScriptCaller, int posx, int posy, int posz)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	object_id_type lCallerID = pkScriptCaller->GetID();
	if (!IS_PLAYER_CLASS_ID(lCallerID) && !IS_NPC_ID(lCallerID))
	{
		return;
	}

	LC_ActorBase* pkCaller = (LC_ActorBase*)pkScriptCaller;
	UT_Vec3Int kTargetLocation;
	kTargetLocation.x = posx;
	kTargetLocation.y = posy;
	kTargetLocation.z = posz;
	pkCaller->SetTargetLocation(kTargetLocation);
}

void SC_ClientSkillInterface::SetClosestTargetLocation(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	object_id_type lCallerID = pkScriptCaller->GetID();
	if (!IS_PLAYER_CLASS_ID(lCallerID) && !IS_NPC_ID(lCallerID) && !IS_LOCAL_NPC_ID(lCallerID))
	{
		return;
	}

	LC_ActorBase* pkCaller = (LC_ActorBase*)pkScriptCaller;

	UT_Vec3Int kTargetLocation;
	if(iTargetIndex == TARGET_LOCATION)
		kTargetLocation = pkCaller->GetLockedLocation();
	else
	{
		LC_ActorPtrList kTargetsList;
		_findTarget(pkScriptCaller,iTargetIndex, kTargetsList);
		if(kTargetsList.size() != 1)
			return;
		LC_LogicObject* pkTarget = kTargetsList.front();
		if(!pkTarget)
			return;
		//如果锁定自身
		if (pkTarget->GetID() == lCallerID)
			return;

		kTargetLocation = pkTarget->GetVisibleLocation();
	}

	Path::PF_PathManager* pkPathMgr = Path::PF_PathManager::GetSingletonPtr();
	kTargetLocation.x = UT_MathBase::ClampFloat( Int2Float(kTargetLocation.x) , 0.0f , GfxGetWorldConfig().CFG_WORLD_MAP_WIDTH );
	kTargetLocation.y = UT_MathBase::ClampFloat( Int2Float(kTargetLocation.y) , 0.0f , GfxGetWorldConfig().CFG_WORLD_MAP_WIDTH );
	UT_Vec3Int kNewTarget = kTargetLocation;
	if ( pkPathMgr )
	{
		bool bCanMove = pkPathMgr->TestCanMoveTo( kTargetLocation , false );
		if ( !bCanMove )
		{
			int lSearchStep = 200;
			int lHorizontalRange;
			int lVerticalRange;
			int lMaxDistance;

			lHorizontalRange = 2000;
			lVerticalRange   = 500;
			lMaxDistance     = 500;

			kNewTarget = pkPathMgr->FindClosestMovablePoint( kTargetLocation , 
				lSearchStep , 100 , lHorizontalRange , lVerticalRange , lMaxDistance );
			kTargetLocation = (kNewTarget.x!=0 && kNewTarget.y!=0)? kNewTarget: pkScriptCaller->GetCurrentLocation();
		}
	}

	pkCaller->SetTargetLocation(kTargetLocation);
}

//---------------------------------------------------
void SC_ClientSkillInterface::SetAlphaDead(GameLogic::LC_Attacker* pkScriptCaller, bool kFlag)
{
	if (pkScriptCaller && IS_NPC_ID(pkScriptCaller->GetID()) )
	{
		LC_ClientNPC* pkNPC = (LC_ClientNPC*)pkScriptCaller;
		pkNPC->SetAlphaDead(kFlag);
	}
}
//---------------------------------------------------
void SC_ClientSkillInterface::GenerateSkillDamage(GameLogic::LC_Attacker* pkScriptCaller,int iDamageTargetIndex, int lSkillTypeID,int iDamageRandomIndex, int iAttackRateRandomIndex,int iCriticalRateRandomIndex,object_id_type lDamageSourceActorID, int iHPCost, int iMPCost)
{
	LC_ActorPtrList kDamageTargetsList;
	_findTarget(pkScriptCaller,iDamageTargetIndex, kDamageTargetsList);
	if(kDamageTargetsList.size() == 0)
		return;

	LC_ActorBase* pkAttacker = NULL;

	if(pkScriptCaller->GetID() == lDamageSourceActorID)
		pkAttacker = (LC_ActorBase*)pkScriptCaller;
	else
		pkAttacker = _findActor(lDamageSourceActorID);

	if(!pkAttacker)
		return;

	LC_ActorBase*		pkTarget = NULL;
	object_id_type				lTargetID;
	LC_ActorPtrListIter Iter;

	TRAV(kDamageTargetsList,Iter)
	{
		pkTarget = *Iter;
		lTargetID = pkTarget->GetID();
	
		//抛出伤害事件
		SK_ClientSkillEventManager* pkEventManager = CLIENT_GET_SKILL_EVENT_MANAGER();
		SK_SkillEvent* pkSkillEvent = pkEventManager->AllocEvent();
		if (NULL != pkSkillEvent)
		{
			pkSkillEvent->SetEventType(SETE_CLIENT_ATTACK);
			pkSkillEvent->SetSrcActorID(lDamageSourceActorID);
			pkSkillEvent->SetDstActorID(lTargetID);
			pkSkillEvent->SetLogicID(lSkillTypeID);
			pkEventManager->AddEvent(pkSkillEvent);
		}
	}
}
//---------------------------------------------------
void SC_ClientSkillInterface::GenerateSkillDamageMultiple(GameLogic::LC_Attacker* pkScriptCaller,int iDamageTargetIndex, int lSkillTypeID,int iDamageRandomIndex, int iAttackRateRandomIndex,int iCriticalRateRandomIndex,object_id_type lDamageSourceActorID, int iHPCost, int iMPCost, int Multiple, int fPercent)
{
	this->GenerateSkillDamage(pkScriptCaller,iDamageTargetIndex,lSkillTypeID,iDamageRandomIndex,iAttackRateRandomIndex,iCriticalRateRandomIndex,lDamageSourceActorID,iHPCost,iMPCost);
}
//---------------------------------------------------
void SC_ClientSkillInterface::GenerateSkillDamageMainTarget( GameLogic::LC_Attacker* pkScriptCaller,int iDamageTargetIndex, int lSkillTypeID,int iDamageRandomIndex, int iAttackRateRandomIndex,int iCriticalRateRandomIndex,object_id_type lDamageSourceActorID, int iHPCost, int iMPCost,int fPercent )
{
	this->GenerateSkillDamage(pkScriptCaller, iDamageTargetIndex, lSkillTypeID, iDamageRandomIndex, iAttackRateRandomIndex, iCriticalRateRandomIndex, lDamageSourceActorID, iHPCost, iMPCost);
}
//---------------------------------------------------
void SC_ClientSkillInterface::GenerateDamage(GameLogic::LC_Attacker* pkScriptCaller,int iDamageTargetIndex,int lDamageLogicTypeID,int lDamageType,int iDamage,object_id_type lDamageSourceActorID)
{
	LC_ActorPtrList kDamageTargetsList;
	_findTarget(pkScriptCaller,iDamageTargetIndex, kDamageTargetsList);
	if(kDamageTargetsList.size() == 0)
		return;

	LC_ActorBase* pkAttacker = NULL;

	if(pkScriptCaller->GetID() == lDamageSourceActorID)
		pkAttacker = (LC_ActorBase*)pkScriptCaller;
	else
		pkAttacker = _findActor(lDamageSourceActorID);

	if(!pkAttacker)
		return;

	LC_ActorBase*		pkTarget = NULL;
	object_id_type				lTargetID;
	LC_ActorPtrListIter Iter;

	TRAV(kDamageTargetsList,Iter)
	{
		pkTarget = *Iter;
		lTargetID = pkTarget->GetID();

		//抛出伤害事件
		SK_ClientSkillEventManager* pkEventManager = CLIENT_GET_SKILL_EVENT_MANAGER();
		SK_SkillEvent* pkSkillEvent = pkEventManager->AllocEvent();
		if (NULL != pkSkillEvent)
		{
			pkSkillEvent->SetEventType(SETE_CLIENT_ATTACK);
			pkSkillEvent->SetSrcActorID(lDamageSourceActorID);
			pkSkillEvent->SetDstActorID(lTargetID);
			pkSkillEvent->SetLogicID(lDamageLogicTypeID);
			pkEventManager->AddEvent(pkSkillEvent);
		}
	}
}
//---------------------------------------------------
//bool SC_ClientSkillInterface::SitForRecovery(GameLogic::LC_ActorBase* pkMachineOwner)
//{
//	if( pkMachineOwner->GetHP() != pkMachineOwner->GetAttributeMap()->GetAttribute(ATT_MAX_HP)
//		&& pkMachineOwner->GetMP() != pkMachineOwner->GetAttributeMap()->GetAttribute(ATT_MAX_MP) )
//	{
//		//打坐回复HP\MP
//		pkMachineOwner->PendingSkill(21003001,0);	
//		return false;
//	}
//	else 
//	{
//		//打坐结束
//		float fCurrentTime = GET_PROCESS_TIME;
//		pkMachineOwner->BeInterruptCurrentSkill(fCurrentTime);		//test
//		return true;
//	}
//}

//---------------------------------------------------
//void SC_ClientSkillInterface::AttachTargetGfx(GameLogic::LC_Attacker* pkScriptCaller, 
//											  int iTargetIndex, 
//											  int iDummyType, 
//											  const StringType& strFxFileName)
//{
//	//将特效绑定到地点
//	if (TARGET_LOCATION == iTargetIndex)
//	{
//		UT_Vec3Int kTargetLocation = pkScriptCaller->GetLockedLocation();
//
//		//获取目标点Z坐标
//		SE_SceneManager* pkSceneManager = SE_SceneManager::GetSingletonPtr();
//		const SE_SceneTerrainLocalInfo& rkTerrainInfo = 
//			pkSceneManager->GetWalkableInfo(Int2Float(kTargetLocation.x), Int2Float(kTargetLocation.y));
//		kTargetLocation.z = Float2Int(rkTerrainInfo._fLocalHeight);
//
//		FX_ClientFxManager* pkFxManager = FX_ClientFxManager::GetSingletonPtr();
//		int iFxID = pkFxManager->CreateFx(strFxFileName, true);
//		pkFxManager->BindFxToLocation(iFxID, UT_EngineMath::ConvertVec3IntToNiPoint3(kTargetLocation));
//		pkFxManager->PlayFx(iFxID, GET_PROCESS_TIME);
//	}
//	else
//	{
//		T_ASSERT(iDummyType > GD_ClientActorModel::FDT_INVALID && iDummyType < GD_ClientActorModel::FDT_MAX_COUNT);
//
//		LC_ActorPtrList kTargetsList = _findTarget(pkScriptCaller, iTargetIndex);
//		GD_ClientActorModel* pkVisibleData = NULL;
//		LC_ActorPtrListIter IterBe = kTargetsList.begin();
//		LC_ActorPtrListIter IterEd = kTargetsList.end();
//
//		for(; IterBe != IterEd; ++IterBe)
//		{
//			pkVisibleData = IM_Manager::GetSingletonPtr()->GetActor((*IterBe)->GetID());
//			if (pkVisibleData != NULL)
//			{
//				pkVisibleData->AttachFx((GD_ClientActorModel::FxDummyNodeType)iDummyType, 0, true, strFxFileName);
//			}
//		}
//	}
//}
//---------------------------------------------------
void SC_ClientSkillInterface::AttachClientTargetGfx(GameLogic::LC_Attacker* pkScriptCaller, 
											  int iTargetIndex, 
											  int iDummyType, 
											  const StringType& strFxFileName, 
											  bool bStayStatic)
{
	//将特效绑定到地点
	if (TARGET_LOCATION == iTargetIndex)
	{
		UT_Vec3Int kTargetLocation = pkScriptCaller->GetLockedLocation();

		//获取目标点Z坐标
		SE_SceneManager* pkSceneManager = SE_SceneManager::GetSingletonPtr();
		const SE_SceneTerrainLocalInfo& rkTerrainInfo = 
			pkSceneManager->GetWalkableInfo(Int2Float(kTargetLocation.x), Int2Float(kTargetLocation.y));
		kTargetLocation.z = Float2Int(rkTerrainInfo._fLocalHeight);

		FX_ClientFxManager* pkFxManager = FX_ClientFxManager::GetSingletonPtr();
		StringType rp = pkFxManager->findPlatformReplace(strFxFileName);
		int iFxID = pkFxManager->CreateFx(rp, true);
		pkFxManager->BindFxToLocation(iFxID, UT_EngineMath::ConvertVec3IntToNiPoint3(kTargetLocation));
		pkFxManager->PlayFx(iFxID, GET_PROCESS_TIME);
	}
	else if ( TARGET_BULLET_LOCATION == iTargetIndex)
	{
		UT_Vec3Int kTargetLocation = pkScriptCaller->GetBulletLocation();

		//获取目标点Z坐标
		SE_SceneManager* pkSceneManager = SE_SceneManager::GetSingletonPtr();
		const SE_SceneTerrainLocalInfo& rkTerrainInfo = 
			pkSceneManager->GetWalkableInfo(Int2Float(kTargetLocation.x), Int2Float(kTargetLocation.y));
		kTargetLocation.z = Float2Int(rkTerrainInfo._fLocalHeight);

		FX_ClientFxManager* pkFxManager = FX_ClientFxManager::GetSingletonPtr();
		int iFxID = pkFxManager->CreateFx(strFxFileName, true);
		pkFxManager->BindFxToLocation(iFxID, UT_EngineMath::ConvertVec3IntToNiPoint3(kTargetLocation));
		pkFxManager->PlayFx(iFxID, GET_PROCESS_TIME);
	}
	else
	{
		T_ASSERT(iDummyType > GD_ClientActorModel::FDT_INVALID && iDummyType < GD_ClientActorModel::FDT_MAX_COUNT);

		LC_ActorPtrList kTargetsList;
		_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
		GD_ClientActorModel* pkVisibleData = NULL;
		LC_ActorPtrListIter IterBe = kTargetsList.begin();
		LC_ActorPtrListIter IterEd = kTargetsList.end();

		for(; IterBe != IterEd; ++IterBe)
		{
			pkVisibleData = IM_Manager::GetSingletonPtr()->GetActor((*IterBe)->GetID());
			if (pkVisibleData != NULL)
			{
				FX_ClientFxManager* pkFxManager = FX_ClientFxManager::GetSingletonPtr();
				StringType rp = pkFxManager->findPlatformReplace(strFxFileName);
				pkVisibleData->AttachFx((GD_ClientActorModel::FxDummyNodeType)iDummyType, 0, bStayStatic, rp);
			}
		}
	}
}
//
void SC_ClientSkillInterface::AttachClientTargetGfxSacle(GameLogic::LC_Attacker* pkScriptCaller, 
													int iTargetIndex, 
													int iDummyType, 
													const StringType& strFxFileName, 
													bool bStayStatic,float fScale)
{
	NiTransform kform;
	kform.m_Rotate.MakeIdentity();
	kform.m_Translate = NiPoint3::ZERO;
	kform.m_fScale = fScale;

	//将特效绑定到地点
	if (TARGET_LOCATION == iTargetIndex)
	{
		UT_Vec3Int kTargetLocation = pkScriptCaller->GetLockedLocation();

		//获取目标点Z坐标
		SE_SceneManager* pkSceneManager = SE_SceneManager::GetSingletonPtr();
		const SE_SceneTerrainLocalInfo& rkTerrainInfo = 
			pkSceneManager->GetWalkableInfo(Int2Float(kTargetLocation.x), Int2Float(kTargetLocation.y));
		kTargetLocation.z = Float2Int(rkTerrainInfo._fLocalHeight);

		FX_ClientFxManager* pkFxManager = FX_ClientFxManager::GetSingletonPtr();
		StringType rp = pkFxManager->findPlatformReplace(strFxFileName);
		int iFxID = pkFxManager->CreateFx(rp, true);
		pkFxManager->BindFxToLocation(iFxID, UT_EngineMath::ConvertVec3IntToNiPoint3(kTargetLocation), NULL, &kform);
		pkFxManager->PlayFx(iFxID, GET_PROCESS_TIME);
	}
	else if ( TARGET_BULLET_LOCATION == iTargetIndex)
	{
		UT_Vec3Int kTargetLocation = pkScriptCaller->GetBulletLocation();

		//获取目标点Z坐标
		SE_SceneManager* pkSceneManager = SE_SceneManager::GetSingletonPtr();
		const SE_SceneTerrainLocalInfo& rkTerrainInfo = 
			pkSceneManager->GetWalkableInfo(Int2Float(kTargetLocation.x), Int2Float(kTargetLocation.y));
		kTargetLocation.z = Float2Int(rkTerrainInfo._fLocalHeight);

		FX_ClientFxManager* pkFxManager = FX_ClientFxManager::GetSingletonPtr();
		int iFxID = pkFxManager->CreateFx(strFxFileName, true);
		pkFxManager->BindFxToLocation(iFxID, UT_EngineMath::ConvertVec3IntToNiPoint3(kTargetLocation), NULL, &kform);
		pkFxManager->PlayFx(iFxID, GET_PROCESS_TIME);
	}
	else
	{
		T_ASSERT(iDummyType > GD_ClientActorModel::FDT_INVALID && iDummyType < GD_ClientActorModel::FDT_MAX_COUNT);

		LC_ActorPtrList kTargetsList;
		_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
		GD_ClientActorModel* pkVisibleData = NULL;
		LC_ActorPtrListIter IterBe = kTargetsList.begin();
		LC_ActorPtrListIter IterEd = kTargetsList.end();

		for(; IterBe != IterEd; ++IterBe)
		{
			pkVisibleData = IM_Manager::GetSingletonPtr()->GetActor((*IterBe)->GetID());
			if (pkVisibleData != NULL)
			{
				FX_ClientFxManager* pkFxManager = FX_ClientFxManager::GetSingletonPtr();
				StringType rp = pkFxManager->findPlatformReplace(strFxFileName);
				pkVisibleData->AttachFx((GD_ClientActorModel::FxDummyNodeType)iDummyType, 0, bStayStatic, rp, NULL,NULL, fScale);
			}
		}
	}
}
//---------------------------------------------------
void SC_ClientSkillInterface::DetachTargetGfx(GameLogic::LC_Attacker* pkScriptCaller, 
											  int iTargetIndex, 
											  int iDummyType, 
											  const StringType& strFxFileName)
{
	T_ASSERT(iDummyType > GD_ClientActorModel::FDT_INVALID && iDummyType < GD_ClientActorModel::FDT_MAX_COUNT);

	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	LC_ActorPtrListIter IterBe = kTargetsList.begin();
	LC_ActorPtrListIter IterEd = kTargetsList.end();
	GD_ClientActorModel* pkVisibleData = NULL;

	for(; IterBe != IterEd; ++IterBe)
	{
		pkVisibleData = IM_Manager::GetSingletonPtr()->GetActor((*IterBe)->GetID());
		if (pkVisibleData != NULL)
		{
			pkVisibleData->DetachFx((GD_ClientActorModel::FxDummyNodeType)iDummyType, strFxFileName);
		}
	}
}
//------------------------------------------------------------
void SC_ClientSkillInterface::AttachClientTargetBowGfx(GameLogic::LC_Attacker* pkScriptCaller, 
												 int iTargetIndex, 
												 int iDummyType, 
												 const StringType& strFxFileName, 
												 bool bStayStatic, 
												 int eBindMode, 
												 float fScale)
{
	T_ASSERT(iDummyType > GD_ClientActorModel::FDT_INVALID && iDummyType < GD_ClientActorModel::FDT_MAX_COUNT);

	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	GD_ClientActorModel* pkVisibleData = NULL;
	LC_ActorPtrListIter IterBe = kTargetsList.begin();
	LC_ActorPtrListIter IterEd = kTargetsList.end();

	for(; IterBe != IterEd; ++IterBe)
	{
		pkVisibleData = IM_Manager::GetSingletonPtr()->GetActor((*IterBe)->GetID());
		if (pkVisibleData != NULL)
		{
			pkVisibleData->AttachFxToBow((GD_ClientActorModel::FxDummyNodeType)iDummyType, eBindMode, bStayStatic, strFxFileName, NULL, NULL, fScale);
		}
	}
}
//-----------------------------------------------------------
void SC_ClientSkillInterface::AttachClientTargetBowGfxWithDir(GameLogic::LC_Attacker* pkScriptCaller, 
															  int iTargetIndex, int iDummyType, 
															  const StringType& strFxFileName, 
															  bool bStayStatic, int eBindMode, 
															  float xAngle,float yAngle,float zAngle,
															  float posx,float posy,float posz, 
															  float fScale)
{
	T_ASSERT(iDummyType > GD_ClientActorModel::FDT_INVALID && iDummyType < GD_ClientActorModel::FDT_MAX_COUNT);

	NiPoint3 pt(xAngle,yAngle, zAngle);
	NiPoint3 pos(posx,posy,posz);
	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	GD_ClientActorModel* pkVisibleData = NULL;
	LC_ActorPtrListIter IterBe = kTargetsList.begin();
	LC_ActorPtrListIter IterEd = kTargetsList.end();

	for(; IterBe != IterEd; ++IterBe)
	{
		pkVisibleData = IM_Manager::GetSingletonPtr()->GetActor((*IterBe)->GetID());
		if (pkVisibleData != NULL)
		{
			pkVisibleData->AttachFxToBow((GD_ClientActorModel::FxDummyNodeType)iDummyType, eBindMode, bStayStatic, strFxFileName, &pos, &pt, fScale);
		}
	}
}
//-----------------------------------------------------------
void SC_ClientSkillInterface::AttachClientTargetGfxWithDir(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iDummyType, const StringType& strFxFileName, bool bStayStatic, int eBindMode,float dirAngle, float posx,float posy,float posz)
{
	//将特效绑定到地点
	NiPoint3 pt(0.0f,0.0f, dirAngle);
	NiPoint3 pos(posx,posy,posz);
	if (TARGET_LOCATION == iTargetIndex)
	{
		UT_Vec3Int kTargetLocation = pkScriptCaller->GetLockedLocation();

		//获取目标点Z坐标
		SE_SceneManager* pkSceneManager = SE_SceneManager::GetSingletonPtr();
		const SE_SceneTerrainLocalInfo& rkTerrainInfo = 
			pkSceneManager->GetWalkableInfo(Int2Float(kTargetLocation.x), Int2Float(kTargetLocation.y));
		kTargetLocation.z = Float2Int(rkTerrainInfo._fLocalHeight);

		FX_ClientFxManager* pkFxManager = FX_ClientFxManager::GetSingletonPtr();
		int iFxID = pkFxManager->CreateFx(strFxFileName, true);

		NiTransform kform;
		kform.MakeIdentity();
		if ( pt != NiPoint3::ZERO)
		{
			NiMatrix3 kRotation;

			kRotation.MakeXRotation(pt.x);
			kform.m_Rotate = kRotation;
			kRotation.MakeYRotation(pt.y);
			kform.m_Rotate = kRotation * kform.m_Rotate;
			kRotation.MakeZRotation(pt.z);
			kform.m_Rotate = kRotation * kform.m_Rotate;
		}
		
		if ( pos != NiPoint3::ZERO)
		{
			kform.m_Translate = pos;
		}
		
		pkFxManager->BindFxToLocation(iFxID, UT_EngineMath::ConvertVec3IntToNiPoint3(kTargetLocation), NULL, &kform);
		pkFxManager->PlayFx(iFxID, GET_PROCESS_TIME);
	}
	else
	{
		T_ASSERT(iDummyType > GD_ClientActorModel::FDT_INVALID && iDummyType < GD_ClientActorModel::FDT_MAX_COUNT);

		LC_ActorPtrList kTargetsList;
		_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
		GD_ClientActorModel* pkVisibleData = NULL;
		LC_ActorPtrListIter IterBe = kTargetsList.begin();
		LC_ActorPtrListIter IterEd = kTargetsList.end();

		for(; IterBe != IterEd; ++IterBe)
		{
			pkVisibleData = IM_Manager::GetSingletonPtr()->GetActor((*IterBe)->GetID());
			if (pkVisibleData != NULL)
			{
				pkVisibleData->AttachFx((GD_ClientActorModel::FxDummyNodeType)iDummyType, eBindMode, bStayStatic, strFxFileName,&pos,&pt);
			}
		}
	}
}
//------------------------------------------------------------
void SC_ClientSkillInterface::AttachClientTargetGfxWithDirScale(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iDummyType, const StringType& strFxFileName, bool bStayStatic, int eBindMode, float dirAngle, float posx,float posy,float posz, float scale)
{
	//将特效绑定到地点
	NiPoint3 pt(0.0f,0.0f, dirAngle);
	NiPoint3 pos(posx,posy,posz);
	if (TARGET_LOCATION == iTargetIndex)
	{
		UT_Vec3Int kTargetLocation = pkScriptCaller->GetLockedLocation();

		//获取目标点Z坐标
		SE_SceneManager* pkSceneManager = SE_SceneManager::GetSingletonPtr();
		const SE_SceneTerrainLocalInfo& rkTerrainInfo = 
			pkSceneManager->GetWalkableInfo(Int2Float(kTargetLocation.x), Int2Float(kTargetLocation.y));
		kTargetLocation.z = Float2Int(rkTerrainInfo._fLocalHeight);

		FX_ClientFxManager* pkFxManager = FX_ClientFxManager::GetSingletonPtr();
		int iFxID = pkFxManager->CreateFx(strFxFileName, true);

		NiTransform kform;
		kform.MakeIdentity();
		if ( pt != NiPoint3::ZERO)
		{
			NiMatrix3 kRotation;

			kRotation.MakeXRotation(pt.x);
			kform.m_Rotate = kRotation;
			kRotation.MakeYRotation(pt.y);
			kform.m_Rotate = kRotation * kform.m_Rotate;
			kRotation.MakeZRotation(pt.z);
			kform.m_Rotate = kRotation * kform.m_Rotate;
		}

		if ( pos != NiPoint3::ZERO)
		{
			kform.m_Translate = pos;
		}

		kform.m_fScale = scale;

		pkFxManager->BindFxToLocation(iFxID, UT_EngineMath::ConvertVec3IntToNiPoint3(kTargetLocation), NULL, &kform);
		pkFxManager->PlayFx(iFxID, GET_PROCESS_TIME);
	}
	else
	{
		T_ASSERT(iDummyType > GD_ClientActorModel::FDT_INVALID && iDummyType < GD_ClientActorModel::FDT_MAX_COUNT);

		LC_ActorPtrList kTargetsList;
		_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
		GD_ClientActorModel* pkVisibleData = NULL;
		LC_ActorPtrListIter IterBe = kTargetsList.begin();
		LC_ActorPtrListIter IterEd = kTargetsList.end();

		for(; IterBe != IterEd; ++IterBe)
		{
			pkVisibleData = IM_Manager::GetSingletonPtr()->GetActor((*IterBe)->GetID());
			if (pkVisibleData != NULL)
			{
				pkVisibleData->AttachFx((GD_ClientActorModel::FxDummyNodeType)iDummyType, eBindMode, bStayStatic, strFxFileName,&pos,&pt,scale);
			}
		}
	}
}
//----------------------------------------------------------
void SC_ClientSkillInterface::AttachClientTargetGfxWith3DScale(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iDummyType, 
															   const StringType& strFxFileName, bool bStayStatic, int eBindMode, 
															   float xAngle, float yAngle, float zAngle, 
															   float posx, float posy, float posz, float scale)
{
	//将特效绑定到地点
	xAngle *= UT_MathBase::ANGLE2RAD;
	yAngle *= UT_MathBase::ANGLE2RAD;
	zAngle *= UT_MathBase::ANGLE2RAD;
	NiPoint3 pt(xAngle, yAngle, zAngle);
	NiPoint3 pos(posx,posy,posz);
	if (TARGET_LOCATION == iTargetIndex)
	{
		UT_Vec3Int kTargetLocation = pkScriptCaller->GetLockedLocation();

		//获取目标点Z坐标
		SE_SceneManager* pkSceneManager = SE_SceneManager::GetSingletonPtr();
		const SE_SceneTerrainLocalInfo& rkTerrainInfo = 
			pkSceneManager->GetWalkableInfo(Int2Float(kTargetLocation.x), Int2Float(kTargetLocation.y));
		kTargetLocation.z = Float2Int(rkTerrainInfo._fLocalHeight);

		FX_ClientFxManager* pkFxManager = FX_ClientFxManager::GetSingletonPtr();
		int iFxID = pkFxManager->CreateFx(strFxFileName, true);

		NiTransform kform;
		kform.MakeIdentity();
		if ( pt != NiPoint3::ZERO)
		{
			NiMatrix3 kRotation;

			kRotation.MakeXRotation(pt.x);
			kform.m_Rotate = kRotation;
			kRotation.MakeYRotation(pt.y);
			kform.m_Rotate = kRotation * kform.m_Rotate;
			kRotation.MakeZRotation(pt.z);
			kform.m_Rotate = kRotation * kform.m_Rotate;
		}

		if ( pos != NiPoint3::ZERO)
		{
			kform.m_Translate = pos;
		}

		kform.m_fScale = scale;

		UT_Vec2Float callerDir = ( (LC_ActorBase*)pkScriptCaller )->GetForwardDir();
		NiPoint3 direction = UT_EngineMath::ConvertVec3FloatToNiPoint3( UT_MathBase::ConvertVec2To3Float(callerDir));

		pkFxManager->BindFxToLocation(iFxID, UT_EngineMath::ConvertVec3IntToNiPoint3(kTargetLocation), eBindMode == 1? &direction: NULL, &kform);
		pkFxManager->PlayFx(iFxID, GET_PROCESS_TIME);
	}
	else
	{
		T_ASSERT(iDummyType > GD_ClientActorModel::FDT_INVALID && iDummyType < GD_ClientActorModel::FDT_MAX_COUNT);

		LC_ActorPtrList kTargetsList;
		_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
		GD_ClientActorModel* pkVisibleData = NULL;
		LC_ActorPtrListIter IterBe = kTargetsList.begin();
		LC_ActorPtrListIter IterEd = kTargetsList.end();

		for(; IterBe != IterEd; ++IterBe)
		{
			pkVisibleData = IM_Manager::GetSingletonPtr()->GetActor((*IterBe)->GetID());
			if (pkVisibleData != NULL)
			{
				pkVisibleData->AttachFx((GD_ClientActorModel::FxDummyNodeType)iDummyType, eBindMode, bStayStatic, strFxFileName,&pos,&pt,scale);
			}
		}
	}
}
//------------------------------------------------------------
void SC_ClientSkillInterface::AttachClientTargetPairGfx(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iDummyType1,int iPairTargetIndex, int iDummyType2, const StringType& strFxFileName)
{
	UT_Vec3Int kTargetLocation;
	UT_Vec3Int kPairTargetLocation;

	logic_id_type srcID = 0;
	logic_id_type dstID = 0;

	Data::GD_ModelBase* srcMod = NULL;
	Data::GD_ModelBase* dstMod = NULL;
	//找到目标点1
	if (TARGET_LOCATION == iTargetIndex)
	{
		kTargetLocation = pkScriptCaller->GetLockedLocation();
	}
	else
	{
		LC_ActorPtrList kTargetsList;
		_findTarget(pkScriptCaller, iTargetIndex,kTargetsList);
		if (kTargetsList.size() == 1)
		{
			LC_ActorBase* pkActor = kTargetsList.front();
			srcMod = IM_Manager::GetSingleton().GetModel(pkActor->GetID());
			if (srcMod == NULL)
			{
				return ;
			}
			srcID = pkActor->GetID();
			const NiPoint3& kDummyPos = srcMod->GetFxDummyNodePosition((Data::GD_ModelBase::FxDummyNodeType)iDummyType1 );
			kTargetLocation = UT_EngineMath::ConvertNiPoint3ToVec3Int(kDummyPos);
		}
		else
		{
			return ;
		}
	}

	//找到目标点2
	if (TARGET_LOCATION == iPairTargetIndex)
	{
		kPairTargetLocation = pkScriptCaller->GetLockedLocation();
	}
	else
	{
		LC_ActorPtrList kTargetsList;
		_findTarget(pkScriptCaller, iPairTargetIndex, kTargetsList);
		if (kTargetsList.size() == 1)
		{
			LC_ActorBase* pkActor = kTargetsList.front();
			dstMod = IM_Manager::GetSingleton().GetModel(pkActor->GetID());
			if (dstMod == NULL)
			{
				return ;
			}
			dstID = pkActor->GetID();
			const NiPoint3& kDummyPos = dstMod->GetFxDummyNodePosition((Data::GD_ModelBase::FxDummyNodeType)iDummyType2 );
			kPairTargetLocation = UT_EngineMath::ConvertNiPoint3ToVec3Int(kDummyPos);
		}
		else
		{
			return ;
		}
	}

// 	//获取目标点1 Z坐标
// 	SE_SceneManager* pkSceneManager = SE_SceneManager::GetSingletonPtr();
// 	const SE_SceneTerrainLocalInfo& rkTerrainInfo1 = 
// 		pkSceneManager->GetWalkableInfo(Int2Float(kTargetLocation.x), Int2Float(kTargetLocation.y));
// 	kTargetLocation.z = Float2Int(rkTerrainInfo1._fLocalHeight);
// 	//获取目标点2 Z坐标
// 	const SE_SceneTerrainLocalInfo& rkTerrainInfo2 = 
// 		pkSceneManager->GetWalkableInfo(Int2Float(kPairTargetLocation.x), Int2Float(kPairTargetLocation.y));
// 	kPairTargetLocation.z = Float2Int(rkTerrainInfo2._fLocalHeight);

	if ( dstID != 0 && srcID != 0 )
	{
		FX_ClientFxManager* pkFxManager = FX_ClientFxManager::GetSingletonPtr();
		int iFxID = pkFxManager->CreateFx(strFxFileName, true);
		pkFxManager->BindFxToPairLocations(iFxID, UT_EngineMath::ConvertVec3IntToNiPoint3(kTargetLocation), UT_EngineMath::ConvertVec3IntToNiPoint3(kPairTargetLocation));
		pkFxManager->PlayFx(iFxID, GET_PROCESS_TIME);

		if ( srcMod )
		{
			srcMod->AddPairGfxID(iFxID);
		}
		if ( dstMod )
		{
			dstMod->AddPairGfxID(iFxID);
		}
		pkFxManager->RegisterGfxPairActor(iFxID, srcID, dstID);

	}
}
//------------------------------------------------------------
void SC_ClientSkillInterface::AttachClientTargetPairGfxLink(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iDummyType1,int iPairTargetIndex, int iDummyType2, const StringType& strFxFileName)
{
	NiAVObjectPtr kDummyNode1;
	NiAVObjectPtr kDummyNode2;
	int lActorID = 0;
	int lTargetID = 0;

	GD_ClientActorModel* pkSrcModel = NULL;
	GD_ClientActorModel* pkDstModel = NULL;

	//找到目标点1
	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	if (kTargetsList.size() == 1)
	{
		LC_ActorBase* pkActor = kTargetsList.front();
		pkSrcModel = (GD_ClientActorModel*)IM_Manager::GetSingleton().GetModel(pkActor->GetID());
		if (pkSrcModel == NULL)
		{
			return ;
		}
		kDummyNode1 = pkSrcModel->GetFxDummyNode((GD_ClientActorModel::FxDummyNodeType)iDummyType1);
		lActorID = pkActor->GetID();
	}

	else
	{
		return ;
	}

	//找到目标点2
	kTargetsList;
	_findTarget(pkScriptCaller, iPairTargetIndex, kTargetsList);
	if (kTargetsList.size() == 1)
	{
		LC_ActorBase* pkActor = kTargetsList.front();
		pkDstModel = (GD_ClientActorModel*)IM_Manager::GetSingleton().GetModel(pkActor->GetID());
		if (pkDstModel == NULL)
		{
			return ;
		}
		kDummyNode2 = pkDstModel->GetFxDummyNode((GD_ClientActorModel::FxDummyNodeType)iDummyType2);
		lTargetID = pkActor->GetID();
	} 
	else
	{
		return ;
	}

	if ( lTargetID != 0 && lActorID != 0)
	{
		FX_ClientFxManager* pkFxManager = FX_ClientFxManager::GetSingletonPtr();
		int iFxID = pkFxManager->CreateFx(strFxFileName, true);
		pkFxManager->BindFxToPairDummyNode(iFxID, kDummyNode1, false, false, kDummyNode2, false, false);
		pkFxManager->PlayFx(iFxID, GET_PROCESS_TIME);

		if ( pkSrcModel )
		{
			pkSrcModel->AddPairGfxID(iFxID);
		}
		if ( pkDstModel )
		{
			pkDstModel->AddPairGfxID(iFxID);
		}
		pkFxManager->RegisterGfxPairActor(iFxID, lActorID, lTargetID);

	}
}
//------------------------------------------------------------
int SC_ClientSkillInterface::AttachClientTargetSound(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, const StringType& kFileName, int iSoundChannel, int iSoundType)
{
	if (NULL == pkScriptCaller)
	{
		return 0;
	}

	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if(!CheckAttachSkillSound(pkGamePlayer, pkScriptCaller))
		return 0;

	float fControlVolume = 1.0f;
	if (pkScriptCaller != pkGamePlayer && IS_PLAYER_CLASS_ID(pkScriptCaller->GetID()))
	{
		fControlVolume = GetGlobalSetting.fSkillVolumeControl;
	}

	//将音效播放到某个位置（目前此处不可播放循环音效）
	int soundID = 0;
	if ( iSoundType == SK_AttachSoundEvent::SST_2D_SINGLE_SOUND ) 
	{
		soundID = g_soundMgr.Play2DEx(kFileName, (AS_ClientSoundManager::SOUND_CHANNEL_TYPE)iSoundChannel);
	}
	else
	{
		if (TARGET_LOCATION == iTargetIndex)
		{
			UT_Vec3Int kTargetLocation = pkScriptCaller->GetLockedLocation();
			NiPoint3 kPos = UT_EngineMath::ConvertVec3IntToNiPoint3(kTargetLocation);
			const SE_SceneTerrainLocalInfo& crTInfo = 
				SE_SceneManager::GetSingleton().GetWalkableInfo((float)kPos.x, (float)kPos.y, true, false);
			kPos.z = (int)crTInfo._fLocalHeight;
			soundID = g_soundMgr.Play3DEx(kPos, kFileName, (AS_ClientSoundManager::SOUND_CHANNEL_TYPE)iSoundChannel, false, false, fControlVolume);
		}
		else if (TARGET_BULLET_LOCATION == iTargetIndex )
		{
			UT_Vec3Int kTargetLocation = pkScriptCaller->GetBulletLocation();
			NiPoint3 kPos = UT_EngineMath::ConvertVec3IntToNiPoint3(kTargetLocation);
			const SE_SceneTerrainLocalInfo& crTInfo = 
				SE_SceneManager::GetSingleton().GetWalkableInfo((float)kPos.x, (float)kPos.y, true, false);
			kPos.z = (int)crTInfo._fLocalHeight;
			soundID = g_soundMgr.Play3DEx(kPos, kFileName, (AS_ClientSoundManager::SOUND_CHANNEL_TYPE)iSoundChannel, false, false, fControlVolume);
		}
		else
		{
			LC_ActorPtrList kTargetsList;
			_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
			LC_ActorPtrListIter IterBe = kTargetsList.begin();
			LC_ActorPtrListIter IterEd = kTargetsList.end();
			for(; IterBe != IterEd; ++IterBe)
			{
				{
					NiPoint3 kPos = UT_EngineMath::ConvertVec3IntToNiPoint3((*IterBe)->GetCurrentLocation());
					const SE_SceneTerrainLocalInfo& crTInfo = 
						SE_SceneManager::GetSingleton().GetWalkableInfo((float)kPos.x, (float)kPos.y, true, false);
					kPos.z = (int)crTInfo._fLocalHeight;
					soundID = g_soundMgr.Play3DEx(kPos, kFileName, (AS_ClientSoundManager::SOUND_CHANNEL_TYPE)iSoundChannel, false, false, fControlVolume);
				}
			}
		}
	}
	return soundID;
}

int SC_ClientSkillInterface::AttachClientTargetSoundCount(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, const StringType& kFileName, int iSoundChannel, int iSoundType, bool isLoop)
{
	if (NULL == pkScriptCaller)
	{
		return 0;
	}

	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if(!CheckAttachSkillSound(pkGamePlayer, pkScriptCaller))
		return 0;

	float fControlVolume = 1.0f;
	if (pkScriptCaller != pkGamePlayer && IS_PLAYER_CLASS_ID(pkScriptCaller->GetID()))
	{
		fControlVolume = GetGlobalSetting.fSkillVolumeControl;
	}

	//将音效播放到某个位置（目前此处不可播放循环音效）
	int soundID = 0;
	if ( iSoundType == SK_AttachSoundEvent::SST_2D_SINGLE_SOUND ) 
	{
		soundID = g_soundMgr.Play2DEx(kFileName, (AS_ClientSoundManager::SOUND_CHANNEL_TYPE)iSoundChannel);
	}
	else
	{
		if (TARGET_LOCATION == iTargetIndex)
		{
			UT_Vec3Int kTargetLocation = pkScriptCaller->GetLockedLocation();
			NiPoint3 kPos = UT_EngineMath::ConvertVec3IntToNiPoint3(kTargetLocation);
			const SE_SceneTerrainLocalInfo& crTInfo = 
				SE_SceneManager::GetSingleton().GetWalkableInfo((float)kPos.x, (float)kPos.y, true, false);
			kPos.z = (int)crTInfo._fLocalHeight;
			soundID = g_soundMgr.Play3DEx(kPos, kFileName, (AS_ClientSoundManager::SOUND_CHANNEL_TYPE)iSoundChannel, false, isLoop, fControlVolume);
		}
		else if (TARGET_BULLET_LOCATION == iTargetIndex )
		{
			UT_Vec3Int kTargetLocation = pkScriptCaller->GetBulletLocation();
			NiPoint3 kPos = UT_EngineMath::ConvertVec3IntToNiPoint3(kTargetLocation);
			const SE_SceneTerrainLocalInfo& crTInfo = 
				SE_SceneManager::GetSingleton().GetWalkableInfo((float)kPos.x, (float)kPos.y, true, false);
			kPos.z = (int)crTInfo._fLocalHeight;
			soundID = g_soundMgr.Play3DEx(kPos, kFileName, (AS_ClientSoundManager::SOUND_CHANNEL_TYPE)iSoundChannel, false, isLoop, fControlVolume);
		}
		else
		{
			LC_ActorPtrList kTargetsList;
			_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
			LC_ActorPtrListIter IterBe = kTargetsList.begin();
			LC_ActorPtrListIter IterEd = kTargetsList.end();
			for(; IterBe != IterEd; ++IterBe)
			{
				{
					NiPoint3 kPos = UT_EngineMath::ConvertVec3IntToNiPoint3((*IterBe)->GetCurrentLocation());
					const SE_SceneTerrainLocalInfo& crTInfo = 
						SE_SceneManager::GetSingleton().GetWalkableInfo((float)kPos.x, (float)kPos.y, true, false);
					kPos.z = (int)crTInfo._fLocalHeight;
					soundID = g_soundMgr.Play3DEx(kPos, kFileName, (AS_ClientSoundManager::SOUND_CHANNEL_TYPE)iSoundChannel, false, isLoop, fControlVolume);
				}
			}
		}
	}
	return soundID;
}

void SC_ClientSkillInterface::StopClientTargetSound(GameLogic::LC_Attacker* pkScriptCaller, int SoundID, bool isFade, float fadeTime)
{
	if (pkScriptCaller == NULL)
		return;
	
	if (isFade)
		g_soundMgr.StopAudioSourceFade(SoundID, fadeTime);
	else
		g_soundMgr.StopAudioSource(SoundID);
}

void SC_ClientSkillInterface::UpdateClientTargetSoundPosition(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int soundID)
{
	if (NULL == pkScriptCaller)
		return;

	NiPoint3 kPos = NiPoint3::ZERO;
	if (TARGET_LOCATION == iTargetIndex)
	{
		UT_Vec3Int kTargetLocation = pkScriptCaller->GetLockedLocation();
		kPos = UT_EngineMath::ConvertVec3IntToNiPoint3(kTargetLocation);
		const SE_SceneTerrainLocalInfo& crTInfo = 
			SE_SceneManager::GetSingleton().GetWalkableInfo((float)kPos.x, (float)kPos.y, true, false);
		kPos.z = (int)crTInfo._fLocalHeight;
	}
	else if (TARGET_BULLET_LOCATION == iTargetIndex )
	{
		UT_Vec3Int kTargetLocation = pkScriptCaller->GetBulletLocation();
		kPos = UT_EngineMath::ConvertVec3IntToNiPoint3(kTargetLocation);
		const SE_SceneTerrainLocalInfo& crTInfo = 
			SE_SceneManager::GetSingleton().GetWalkableInfo((float)kPos.x, (float)kPos.y, true, false);
		kPos.z = (int)crTInfo._fLocalHeight;
	}
	else
	{
		LC_ActorPtrList kTargetsList;
		_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
		LC_ActorPtrListIter IterBe = kTargetsList.begin();
		LC_ActorPtrListIter IterEd = kTargetsList.end();
		for(; IterBe != IterEd; ++IterBe)
		{
			kPos = UT_EngineMath::ConvertVec3IntToNiPoint3((*IterBe)->GetCurrentLocation());
			const SE_SceneTerrainLocalInfo& crTInfo = 
				SE_SceneManager::GetSingleton().GetWalkableInfo((float)kPos.x, (float)kPos.y, true, false);
			kPos.z = (int)crTInfo._fLocalHeight;
		}
	}

	if (kPos != NiPoint3::ZERO)
		g_soundMgr.UpdateAudioSourcePosition(kPos, soundID);
}

int SC_ClientSkillInterface::AttachClientTargetVoice(GameLogic::LC_Attacker* pkScriptCaller, const StringType& kFileName, int iSoundChannel)
{
	if (NULL == pkScriptCaller)
	{
		return 0;
	}

	LC_PlayerBase* pbPtr = dynamic_cast<LC_PlayerBase*>(pkScriptCaller);
	if (pbPtr == NULL)
	{
		return 0;
	}

	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if(!CheckAttachSkillSound(pkGamePlayer, pkScriptCaller))
		return 0;

	if (pbPtr->IsSpecking())
	{
		return 0;
	}

	NiPoint3 kPos = UT_EngineMath::ConvertVec3IntToNiPoint3(pbPtr->GetCurrentLocation());
	int soundID = g_soundMgr.Play3DEx(kPos, kFileName, (AS_ClientSoundManager::SOUND_CHANNEL_TYPE)iSoundChannel);

	pbPtr->ResetVoice(soundID);
	return soundID;
}

bool SC_ClientSkillInterface::CheckAttachSkillSound(LC_ClientGamePlayer* pkGamePlayer, GameLogic::LC_Attacker* pkScriptCaller)
{
	if(pkScriptCaller == NULL)
		return false;

	if(GetGlobalSetting.iSkillSoundMode == 0
		|| pkGamePlayer == pkScriptCaller)
		return true;

	object_id_type id = pkScriptCaller->GetID();
	if(IS_NPC_ID(id))
	{
		return pkGamePlayer->GetID() == ((LC_ClientNPC*)pkScriptCaller)->GetSummonOwnerID();
	}
	else if(IS_PLAYER_CLASS_ID(id))
	{
		return !pkGamePlayer->IsBuddy(id) && ((LC_ClientNetPlayer*)pkScriptCaller)->IsMainNetPlayer();
	}

	return true;
}

//------------------------------------------------------------
void SC_ClientSkillInterface::AttachClientNPCSoundSeq(const StringType& kFileName)
{
	g_soundMgr.ParseSequenceString(kFileName);
	g_soundMgr.PlaySequenceNPCSound();
}
//------------------------------------------------------------
void SC_ClientSkillInterface::AttachClientGameEvent(int eventID)
{
	LC_GameEvent gevt;
	gevt.SetEventType((GameEventType)eventID);

	UI_UIManager* pkMgr = UI_UIManager::GetSingletonPtr();
	if (pkMgr)
	{
		pkMgr->HandlerGameEvent(gevt);
	}
}
//------------------------------------------------------------
void SC_ClientSkillInterface::OnPlayNPCSound(object_id_type npcID)
{
	g_soundMgr.OnPlayNPCSound(g_settings.SOUND_LOWER_BGM_FOR_NPCSOUND, npcID);
}
//------------------------------------------------------------
void SC_ClientSkillInterface::DetachTargetBowGfx(GameLogic::LC_Attacker* pkScriptCaller, 
											  int iTargetIndex, 
											  int iDummyType, 
											  const StringType& strFxFileName)
{
	T_ASSERT(iDummyType > GD_ClientActorModel::FDT_INVALID && iDummyType < GD_ClientActorModel::FDT_MAX_COUNT);

	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	LC_ActorPtrListIter IterBe = kTargetsList.begin();
	LC_ActorPtrListIter IterEd = kTargetsList.end();
	GD_ClientActorModel* pkVisibleData = NULL;

	for(; IterBe != IterEd; ++IterBe)
	{
		pkVisibleData = IM_Manager::GetSingletonPtr()->GetActor((*IterBe)->GetID());
		if (pkVisibleData != NULL)
		{
			pkVisibleData->DetachFxToBow((GD_ClientActorModel::FxDummyNodeType)iDummyType, strFxFileName);
		}
	}
}
//------------------------------------------------------------
StringType SC_ClientSkillInterface::GetSitRecoverVipGfx(GameLogic::LC_Attacker* pkScriptCaller)
{
	if (NULL == pkScriptCaller || !IS_PLAYER_ID(pkScriptCaller->GetID()))
	{
		return "";
	}
	LC_PlayerBase * player = ((LC_PlayerBase*)pkScriptCaller);
	SK_SP * inst = SK_SP::GetSingletonPtr();
	if (NULL == inst)
	{
		return "";
	}
	SP_Entry entry;
	inst->GetSP_Entry(player->GetSitRecoverLevel(),entry);
	return entry.Gfx;
}

//-----------------------------------------------------------------------------
//>> 发射子弹
struct ShotBulletArg
{
	LC_ClientGamePlayer* pkGamePlayer;		// 只有GamePlayer的子弹才会如此处理，buddy的都不算
	Skill::SK_SkillExecute* pkSkillExecute;		// 哪个技能发射的
	const CSVFile::CF_SkillBulletFunc::DataEntry * pkBullet;		// 子弹配置表
	int				lSkillTypeID;			// 哪个技能发射的
	int				iBulletFuncIdx;			// 该技能中哪一波子弹

	float			fStartTime;				// 发射时间，客户端
	int				iBulletCount;			// 一次射击几颗子弹，散射数
	int				iRequesterID;			// 主角ID
	int				iTargetIndex;			// 子弹飞行锁定的目标，跟踪型子弹
	int				iLocationIndex;			// 子弹射击的对象取得
	object_id_type	iLockedTargetID;		// 非跟踪型子弹的目标；TARGET_NONE情况下朝TARGET_LOCKED射击，用于调整子弹轨迹的pitch angle
	int				iPenetrateCount;		// 子弹穿刺次数，技能脚本中传入
	int				iDummyType;				// 子弹发射时特效绑点
	int				iScatteringAngle;		// 一次发射多颗子弹时，扇形散射角
	int				iMoveDistance;			// 子弹最大飞行距离
	float			fPitchAngle;			// 绕X轴旋转的角度
	UT_Vec3Int		kShotPos;				// 子弹发射位置
	float			fZRotAngle;				// 子弹绕Z轴的旋转角度

	ShotBulletArg()	{
		::memset(this, 0, sizeof(ShotBulletArg));
	}
};

void SC_ClientSkillInterface::ShotBulletToTarget(GameLogic::LC_Attacker* pkScriptCaller,int iTargetIndex,int lSkillTypeID,int iBulletFuncIdx,int iPenetrateCount,int iDummyType)
{
	LC_ActorBase* pkActor = (LC_ActorBase*)pkScriptCaller;
	// 主角发射的子弹；buddy的由服务器驱动
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (pkScriptCaller != pkGamePlayer)
		return;

	// 取技能数据
	SK_Factory* pkSkillFactory = SK_Factory::GetSingletonPtr();
	SK_SkillExecute* pkSkillExecute = pkSkillFactory->RequestSkillExcute(lSkillTypeID);
	if (NULL == pkSkillExecute)
		return;

	UT_Vec2Float dir = pkActor->GetForwardDir();

	// 准备参数
	ShotBulletArg	arg;
	arg.pkGamePlayer	= pkGamePlayer;
	arg.pkSkillExecute  = pkSkillExecute;
	arg.pkBullet		= pkSkillExecute->GetBulletDataEntryPtr(iBulletFuncIdx);
	arg.fStartTime		= GET_PROCESS_TIME;
	arg.iBulletCount	= 1;
	arg.iRequesterID    = pkActor->GetID();
	arg.iTargetIndex	= iTargetIndex;
	arg.iLocationIndex	= TARGET_ME;
	arg.iLockedTargetID = pkActor->GetLockedTargetID();
	arg.lSkillTypeID	= lSkillTypeID;
	arg.iBulletFuncIdx	= iBulletFuncIdx;
	arg.iPenetrateCount = iPenetrateCount;
	arg.iDummyType		= iDummyType;
	arg.iScatteringAngle= 0;
	arg.iMoveDistance   = arg.pkBullet->_lMaxMoveDistance;
	arg.fPitchAngle		= arg.pkBullet->_fPitchAngle;
	arg.kShotPos		= pkActor->GetCurrentLocation();
	arg.fZRotAngle		= UT_MathBase::GetZRotAngle(dir);

	_implShotBullet(pkActor, arg);
}

void SC_ClientSkillInterface::ShotBulletToTargetAngularDeflection(GameLogic::LC_Attacker* pkScriptCaller,int iTargetIndex,int lSkillTypeID,int iBulletFuncIdx,int iPenetrateCount,int iDummyType, float fAngle)
{
	LC_ActorBase* pkActor = (LC_ActorBase*)pkScriptCaller;
	// 主角发射的子弹；buddy的由服务器驱动
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (pkScriptCaller != pkGamePlayer)
		return;

	// 取技能数据
	SK_Factory* pkSkillFactory = SK_Factory::GetSingletonPtr();
	SK_SkillExecute* pkSkillExecute = pkSkillFactory->RequestSkillExcute(lSkillTypeID);
	if (NULL == pkSkillExecute)
		return;

	UT_Vec2Float dir = pkActor->GetForwardDir();
	if (fAngle != 0)
		dir = UT_MathBase::ZRot(dir, fAngle);

	// 准备参数
	ShotBulletArg	arg;
	arg.pkGamePlayer	= pkGamePlayer;
	arg.pkSkillExecute  = pkSkillExecute;
	arg.pkBullet		= pkSkillExecute->GetBulletDataEntryPtr(iBulletFuncIdx);
	arg.fStartTime		= GET_PROCESS_TIME;
	arg.iBulletCount	= 1;
	arg.iRequesterID    = pkActor->GetID();
	arg.iTargetIndex	= iTargetIndex;
	arg.iLocationIndex	= TARGET_ME;
	arg.iLockedTargetID = pkActor->GetLockedTargetID();
	arg.lSkillTypeID	= lSkillTypeID;
	arg.iBulletFuncIdx	= iBulletFuncIdx;
	arg.iPenetrateCount = iPenetrateCount;
	arg.iDummyType		= iDummyType;
	arg.iScatteringAngle= 0;
	arg.iMoveDistance   = arg.pkBullet->_lMaxMoveDistance;
	arg.fPitchAngle		= arg.pkBullet->_fPitchAngle;
	arg.kShotPos		= pkActor->GetCurrentLocation();
	arg.fZRotAngle		= UT_MathBase::GetZRotAngle(dir);

	_implShotBullet(pkActor, arg);
}

void SC_ClientSkillInterface::ShotBulletToTargetFromLocation(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lSkillTypeID, int iBulletFuncIdx, int iPenetrateCount, int iDummyType, int iLocationIndex, float fDeltaAngle, int64_t lBulletID)
{
	LC_ActorBase* pkActor = (LC_ActorBase*)pkScriptCaller;
	// 主角发射的子弹；buddy的由服务器驱动
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (pkScriptCaller != pkGamePlayer)
		return;

	// 取技能数据
	SK_Factory* pkSkillFactory = SK_Factory::GetSingletonPtr();
	SK_SkillExecute* pkSkillExecute = pkSkillFactory->RequestSkillExcute(lSkillTypeID);
	if (NULL == pkSkillExecute)
		return;

	UT_Vec3Int kShotLocation;
	switch(iLocationIndex)
	{
	case TARGET_BULLET_LOCATION:
		{
			kShotLocation = pkScriptCaller->GetBulletLocation();
		}
		break;
	case TARGET_LOCATION:
		{
			kShotLocation = pkScriptCaller->GetLockedLocation();
			const SE_SceneTerrainLocalInfo& crTInfo = SE_SceneManager::GetSingleton().GetWalkableInfo((float)kShotLocation.x, (float)kShotLocation.y, true, false);
			kShotLocation.z = (int)crTInfo._fLocalHeight;
		}
	default:
		{
			LC_ActorPtrList kTargetsList;
			_findTarget(pkScriptCaller,iLocationIndex, kTargetsList);
			if(kTargetsList.size() != 1)
			{
				return;
			}

			LC_ActorBase* pkTarget = kTargetsList.front();
			kShotLocation = pkTarget->GetCurrentLocation();
		}
		break;
	}

	UT_Vec2Float dir = pkActor->GetForwardDir();
	object_id_type objID = 0;
	if (lBulletID > 0)
	{
		long eventID = 0, bulletLocalID = 0;
		_getBulletAndEventID(lBulletID, eventID, bulletLocalID);

		SK_ClientSkillEventManager* pkSkillEventManager = CLIENT_GET_SKILL_EVENT_MANAGER();
		SK_BulletEvent* bulletEvent = (SK_BulletEvent*)(pkSkillEventManager->FindBulletEvent(eventID, bulletLocalID, pkActor->GetID()));
		if (NULL != bulletEvent /*&& bulletEvent->GetParam(SK_BulletEvent::SEP_MOVE_TYPE) == BT_SHOOT*/)
		{
			UT_Vec3Int fromLocation(bulletEvent->GetParam(SK_BulletEvent::SEP_SHOT_LOCATION_X), bulletEvent->GetParam(SK_BulletEvent::SEP_SHOT_LOCATION_Y), bulletEvent->GetParam(SK_BulletEvent::SEP_SHOT_LOCATION_Z));
			UT_Vec3Int targetLocation(bulletEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_X), bulletEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Y), bulletEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Z));
			UT_Vec3Int preBulletDir = targetLocation - fromLocation;
			dir = UT_Vec2Float((float)preBulletDir.x, (float)preBulletDir.y);
			dir.Unitize();
			objID = bulletEvent->GetDstActorID();
		}
	}
	else
	{
		objID = pkActor->GetLockedTargetID();
	}
	dir = UT_MathBase::ZRot(dir, fDeltaAngle);

	// 准备参数
	ShotBulletArg	arg;
	arg.pkGamePlayer	= pkGamePlayer;
	arg.pkSkillExecute  = pkSkillExecute;
	arg.pkBullet		= pkSkillExecute->GetBulletDataEntryPtr(iBulletFuncIdx);
	arg.fStartTime		= GET_PROCESS_TIME;
	arg.iBulletCount	= 1;
	arg.iRequesterID    = pkActor->GetID();
	arg.iTargetIndex	= iTargetIndex;
	arg.iLocationIndex	= iLocationIndex;
	arg.iLockedTargetID = objID;
	arg.lSkillTypeID	= lSkillTypeID;
	arg.iBulletFuncIdx	= iBulletFuncIdx;
	arg.iPenetrateCount = iPenetrateCount;
	arg.iDummyType		= iDummyType;
	arg.iScatteringAngle= 0;
	arg.iMoveDistance   = arg.pkBullet->_lMaxMoveDistance;
	arg.fPitchAngle		= arg.pkBullet->_fPitchAngle;
	arg.kShotPos		= kShotLocation;
	arg.fZRotAngle		= UT_MathBase::GetZRotAngle(dir);

	_implShotBullet(pkActor, arg);
}

void SC_ClientSkillInterface::ShotMultiBulletToTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lSkillTypeID, int iBulletFuncIdx, int iPenetrateCount, int iDummyType, int iScatteringAngle, int iScatteringCnt)
{
	LC_ActorBase* pkActor = (LC_ActorBase*)pkScriptCaller;

	// 主角发射的子弹；buddy的由服务器驱动
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (pkScriptCaller != pkGamePlayer)
		return;

	// 取技能数据
	SK_Factory* pkSkillFactory = SK_Factory::GetSingletonPtr();
	SK_SkillExecute* pkSkillExecute = pkSkillFactory->RequestSkillExcute(lSkillTypeID);
	if (NULL == pkSkillExecute)
		return;

	// 准备参数
	ShotBulletArg	arg;
	arg.pkGamePlayer	= pkGamePlayer;
	arg.pkSkillExecute	= pkSkillExecute;
	arg.pkBullet		= pkSkillExecute->GetBulletDataEntryPtr(iBulletFuncIdx);
	arg.fStartTime		= GET_PROCESS_TIME;
	arg.iBulletCount	= iScatteringCnt;
	arg.iRequesterID    = pkActor->GetID();
	arg.iTargetIndex	= iTargetIndex;
	arg.iLocationIndex	= TARGET_ME;
	arg.iLockedTargetID = pkActor->GetLockedTargetID();
	arg.lSkillTypeID	= lSkillTypeID;
	arg.iBulletFuncIdx	= iBulletFuncIdx;
	arg.iPenetrateCount = iPenetrateCount;
	arg.iDummyType		= iDummyType;
	arg.iScatteringAngle= iScatteringAngle;
	arg.iMoveDistance   = arg.pkBullet->_lMaxMoveDistance;
	arg.fPitchAngle		= arg.pkBullet->_fPitchAngle;
	arg.kShotPos		= pkActor->GetCurrentLocation();
	arg.fZRotAngle		= pkActor->GetForwardDirAngle();

	_implShotBullet(pkActor, arg);
}

void SC_ClientSkillInterface::ShotBulletAroundTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lMinRadius, int lMaxRadius, int lSkillTypeID, int iBulletFuncID, int iPenetrateCount, int iDummyType)
{
	if (pkScriptCaller == NULL || !IS_PLAYER_CLASS_ID(pkScriptCaller->GetID()) && !IS_NPC_ID(pkScriptCaller->GetID()))
		return;
	LC_ActorBase* pCaller = (LC_ActorBase*)pkScriptCaller;

	UT_Vec3Int center;
	switch (iTargetIndex)
	{
	case TARGET_NONE:
		{
			SK_Factory* pSkillFactory = SK_Factory::GetSingletonPtr();
			SK_SkillExecute* pSkillExecute = pSkillFactory->RequestSkillExcute(lSkillTypeID);
			const CF_SkillBulletFunc::DataEntry* pBulletFunc = pSkillExecute->GetBulletDataEntryPtr(iBulletFuncID);

			int distance = pBulletFunc->_lMaxMoveDistance;
			UT_Vec2Float dir = pCaller->GetForwardDir();
			center = UT_MathBase::V3dIncrementAdd(pCaller->GetCurrentLocation(), dir, distance);
		}
		break;
	case TARGET_LOCKED:
		{
			LC_ActorPtrList targetList;
			_findTarget(pCaller, iTargetIndex, targetList);
			if (targetList.size() <= 0)
			{
				return;
			}
			LC_ActorBase* pTarget = targetList.front();

			center = pTarget->GetCurrentLocation();
		}
		break;
	case TARGET_LOCATION:
		center = pCaller->GetLockedLocation();
		break;
	default:
		T_ASSERT(false);
		break;
	}

	UT_Vec3Int lockedLocation = pCaller->GetLockedLocation();
	UT_Vec3Int targetLocation = UT_MathBase::RandInCircleInt(center, lMinRadius, lMaxRadius);
	pCaller->SetLockedLocation(targetLocation);
	ShotBulletToTarget(pCaller, TARGET_LOCATION, lSkillTypeID, iBulletFuncID, iPenetrateCount, iDummyType);

	pCaller->SetLockedLocation(lockedLocation);
}

void SC_ClientSkillInterface::ShotMultiBulletToTargetFromLocation(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lSkillTypeID, int iBulletFuncID, int iPenetrateCount, int iDummyType, int iScatteringAngle, int iScatteringCnt, int iLocationIndex, float fDeltaAngle, int64_t lBulletID)
{
	LC_ActorBase* pkActor = (LC_ActorBase*)pkScriptCaller;

	// 主角发射的子弹；buddy的由服务器驱动
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (pkScriptCaller != pkGamePlayer)
		return;

	// 取技能数据
	SK_Factory* pkSkillFactory = SK_Factory::GetSingletonPtr();
	SK_SkillExecute* pkSkillExecute = pkSkillFactory->RequestSkillExcute(lSkillTypeID);
	if (NULL == pkSkillExecute)
		return;

	UT_Vec3Int kShotLocation;
	switch(iLocationIndex)
	{
	case TARGET_BULLET_LOCATION:
		{
			kShotLocation = pkScriptCaller->GetBulletLocation();
		}
		break;
	case TARGET_LOCATION:
		{
			kShotLocation = pkScriptCaller->GetLockedLocation();
			const SE_SceneTerrainLocalInfo& crTInfo = SE_SceneManager::GetSingleton().GetWalkableInfo((float)kShotLocation.x, (float)kShotLocation.y, true, false);
			kShotLocation.z = (int)crTInfo._fLocalHeight;
		}
	default:
		{
			LC_ActorPtrList kTargetsList;
			_findTarget(pkScriptCaller,iLocationIndex, kTargetsList);
			if(kTargetsList.size() != 1)
			{
				return;
			}

			LC_ActorBase* pkTarget = kTargetsList.front();
			kShotLocation = pkTarget->GetCurrentLocation();
		}
		break;
	}

	UT_Vec2Float dir = pkActor->GetForwardDir();
	object_id_type objID = 0;
	if (lBulletID > 0)
	{
		long eventID = 0, bulletLocalID = 0;
		_getBulletAndEventID(lBulletID, eventID, bulletLocalID);

		SK_ClientSkillEventManager* pkSkillEventManager = CLIENT_GET_SKILL_EVENT_MANAGER();
		SK_BulletEvent* bulletEvent = (SK_BulletEvent*)(pkSkillEventManager->FindBulletEvent(eventID, bulletLocalID, pkActor->GetID()));
		if (NULL != bulletEvent /*&& bulletEvent->GetParam(SK_BulletEvent::SEP_MOVE_TYPE) == BT_SHOOT*/)
		{
			UT_Vec3Int fromLocation(bulletEvent->GetParam(SK_BulletEvent::SEP_SHOT_LOCATION_X), bulletEvent->GetParam(SK_BulletEvent::SEP_SHOT_LOCATION_Y), bulletEvent->GetParam(SK_BulletEvent::SEP_SHOT_LOCATION_Z));
			UT_Vec3Int targetLocation(bulletEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_X), bulletEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Y), bulletEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Z));
			UT_Vec3Int preBulletDir = targetLocation - fromLocation;
			dir = UT_Vec2Float((float)preBulletDir.x, (float)preBulletDir.y);
			dir.Unitize();
			objID = bulletEvent->GetDstActorID();
		}
	}
	else
	{
		objID = pkActor->GetLockedTargetID();
	}

	dir = UT_MathBase::ZRot(dir, fDeltaAngle);

	// 准备参数
	ShotBulletArg	arg;
	arg.pkGamePlayer	= pkGamePlayer;
	arg.pkSkillExecute	= pkSkillExecute;
	arg.pkBullet		= pkSkillExecute->GetBulletDataEntryPtr(iBulletFuncID);
	arg.fStartTime		= GET_PROCESS_TIME;
	arg.iBulletCount	= iScatteringCnt;
	arg.iRequesterID    = pkActor->GetID();
	arg.iTargetIndex	= iTargetIndex;
	arg.iLocationIndex	= iLocationIndex;
	arg.iLockedTargetID = objID;
	arg.lSkillTypeID	= lSkillTypeID;
	arg.iBulletFuncIdx	= iBulletFuncID;
	arg.iPenetrateCount = iPenetrateCount;
	arg.iDummyType		= iDummyType;
	arg.iScatteringAngle= iScatteringAngle;
	arg.iMoveDistance   = arg.pkBullet->_lMaxMoveDistance;
	arg.fPitchAngle		= arg.pkBullet->_fPitchAngle;
	arg.kShotPos		= kShotLocation;
	arg.fZRotAngle		= UT_MathBase::GetZRotAngle(dir);

	// 发射位置偏移
	const CSVFile::CF_SkillBulletFunc::DataEntry * pkBullet = pkSkillExecute->GetBulletDataEntryPtr(iBulletFuncID);
	if (NULL != pkBullet)
	{
		UT_Vec3Float kShotLocalOffset = UT_MathBase::ConvertVec3IntToFloat(pkBullet->_kShotPosLocalOffset);
		UT_Matrix3Float rot;
		rot.MakeZRotation(UT_MathBase::GetZRotAngle(dir));
		UT_Vec3Float kShotWorldOffset = rot * kShotLocalOffset;
		const SE_SceneTerrainLocalInfo& rkTerrainInfo1 = SE_SceneManager::GetSingletonPtr()->GetWalkableInfo(Int2Float(arg.kShotPos.x), Int2Float(arg.kShotPos.y));
		
		if (arg.kShotPos.z - rkTerrainInfo1._fLocalHeight < 10)
		{
			arg.kShotPos  += UT_MathBase::ConvertVec3FloatToInt(kShotWorldOffset);
		}
	}

	_implShotBullet(pkActor, arg);
}


void SC_ClientSkillInterface::ShotMultiBulletToTargetFromMultiLocation(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lMinRadius, int lMaxRadius, int lSkillTypeID, int iBulletFuncID, int iPenetrateCount, int iDummyType, int iScatteringAngle, int iScatteringCnt, int iLocationIndex, float fDeltaAngle, int64_t lBulletID)
{
	LC_ActorBase* pkActor = (LC_ActorBase*)pkScriptCaller;

	// 主角发射的子弹；buddy的由服务器驱动
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (pkScriptCaller != pkGamePlayer)
		return;

	// 取技能数据
	SK_Factory* pkSkillFactory = SK_Factory::GetSingletonPtr();
	SK_SkillExecute* pkSkillExecute = pkSkillFactory->RequestSkillExcute(lSkillTypeID);
	if (NULL == pkSkillExecute)
		return;

	UT_Vec3Int center;
	switch(iLocationIndex)
	{
	case TARGET_NONE:
		{
			SK_Factory* pSkillFactory = SK_Factory::GetSingletonPtr();
			SK_SkillExecute* pSkillExecute = pSkillFactory->RequestSkillExcute(lSkillTypeID);
			const CF_SkillBulletFunc::DataEntry* pBulletFunc = pSkillExecute->GetBulletDataEntryPtr(iBulletFuncID);

			int distance = pBulletFunc->_lMaxMoveDistance;
			UT_Vec2Float dir = pkActor->GetForwardDir();
			center = UT_MathBase::V3dIncrementAdd(pkActor->GetCurrentLocation(), dir, distance);
		}
		break;
	case TARGET_LOCKED:
		{
			LC_ActorPtrList targetList;
			_findTarget(pkActor, iTargetIndex, targetList);
			if (targetList.size() <= 0)
			{
				return;
			}
			LC_ActorBase* pTarget = targetList.front();

			center = pTarget->GetCurrentLocation();
		}
		break;
	case TARGET_LOCATION:
		center = pkActor->GetLockedLocation();
		break;
	case TARGET_BULLET_LOCATION:
		center = pkScriptCaller->GetBulletLocation();
		break;
	default:
		{
			LC_ActorPtrList kTargetsList;
			_findTarget(pkScriptCaller,iLocationIndex, kTargetsList);
			if(kTargetsList.size() != 1)
			{
				return;
			}

			LC_ActorBase* pkTarget = kTargetsList.front();
			pkTarget->GetCurrentLocationWithZ(center);
		}
		break;
	}
	const SE_SceneTerrainLocalInfo& crTInfo = SE_SceneManager::GetSingleton().GetWalkableInfo((float)center.x, (float)center.y, true, false);
	center.z = (int)crTInfo._fLocalHeight;
	UT_Vec3Int kShotLocation = UT_MathBase::RandInCircleInt(center, lMinRadius, lMaxRadius);

	UT_Vec2Float dir = pkActor->GetForwardDir();
	object_id_type objID = 0;
	if (lBulletID > 0)
	{
		long eventID = 0, bulletLocalID = 0;
		_getBulletAndEventID(lBulletID, eventID, bulletLocalID);

		SK_ClientSkillEventManager* pkSkillEventManager = CLIENT_GET_SKILL_EVENT_MANAGER();
		SK_BulletEvent* bulletEvent = (SK_BulletEvent*)(pkSkillEventManager->FindBulletEvent(eventID, bulletLocalID, pkActor->GetID()));
		if (NULL != bulletEvent /*&& bulletEvent->GetParam(SK_BulletEvent::SEP_MOVE_TYPE) == BT_SHOOT*/)
		{
			UT_Vec3Int fromLocation(bulletEvent->GetParam(SK_BulletEvent::SEP_SHOT_LOCATION_X), bulletEvent->GetParam(SK_BulletEvent::SEP_SHOT_LOCATION_Y), bulletEvent->GetParam(SK_BulletEvent::SEP_SHOT_LOCATION_Z));
			UT_Vec3Int targetLocation(bulletEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_X), bulletEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Y), bulletEvent->GetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Z));
			UT_Vec3Int preBulletDir = targetLocation - fromLocation;
			dir = UT_Vec2Float((float)preBulletDir.x, (float)preBulletDir.y);
			dir.Unitize();
			objID = bulletEvent->GetDstActorID();
		}
	}
	else
	{
		objID = pkActor->GetLockedTargetID();
	}

	dir = UT_MathBase::ZRot(dir, fDeltaAngle);

	// 准备参数
	ShotBulletArg	arg;
	arg.pkGamePlayer	= pkGamePlayer;
	arg.pkSkillExecute	= pkSkillExecute;
	arg.pkBullet		= pkSkillExecute->GetBulletDataEntryPtr(iBulletFuncID);
	arg.fStartTime		= GET_PROCESS_TIME;
	arg.iBulletCount	= iScatteringCnt;
	arg.iRequesterID    = pkActor->GetID();
	arg.iTargetIndex	= iTargetIndex;
	arg.iLocationIndex	= iLocationIndex;
	arg.iLockedTargetID = objID;
	arg.lSkillTypeID	= lSkillTypeID;
	arg.iBulletFuncIdx	= iBulletFuncID;
	arg.iPenetrateCount = iPenetrateCount;
	arg.iDummyType		= iDummyType;
	arg.iScatteringAngle= iScatteringAngle;
	arg.iMoveDistance   = arg.pkBullet->_lMaxMoveDistance;
	arg.fPitchAngle		= arg.pkBullet->_fPitchAngle;
	arg.kShotPos		= kShotLocation;
	arg.fZRotAngle		= UT_MathBase::GetZRotAngle(dir);

	// 发射位置偏移
	const CSVFile::CF_SkillBulletFunc::DataEntry * pkBullet = pkSkillExecute->GetBulletDataEntryPtr(iBulletFuncID);
	if (NULL != pkBullet)
	{
		UT_Vec3Float kShotLocalOffset = UT_MathBase::ConvertVec3IntToFloat(pkBullet->_kShotPosLocalOffset);
		UT_Matrix3Float rot;
		rot.MakeZRotation(UT_MathBase::GetZRotAngle(dir));
		UT_Vec3Float kShotWorldOffset = rot * kShotLocalOffset;
		const SE_SceneTerrainLocalInfo& rkTerrainInfo1 = SE_SceneManager::GetSingletonPtr()->GetWalkableInfo(Int2Float(arg.kShotPos.x), Int2Float(arg.kShotPos.y));

		if (arg.kShotPos.z - rkTerrainInfo1._fLocalHeight < 10)
		{
			arg.kShotPos  += UT_MathBase::ConvertVec3FloatToInt(kShotWorldOffset);
		}
	}

	_implShotBullet(pkActor, arg);
}

//-----------------------------------------------------------------------------
void SC_ClientSkillInterface::OpenPlaceHolder(LC_Attacker* pkScriptCaller,int iTargetIndex, bool bNeedOpenFirst)
{
#pragma TODO("类型转换逻辑")
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	object_id_type lGamePlayerID = pkLogicManager->GetOperationPlayerID();
	if(lGamePlayerID != pkScriptCaller->GetID())
		return;

	LC_ClientGamePlayer* pkPlayer = (LC_ClientGamePlayer*)pkScriptCaller;
	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller,iTargetIndex, kTargetsList);
	if(kTargetsList.size() != 1)
	{
		pkPlayer->SetLockedTargetID(0);
		return;
	}

	LC_ActorBase* pkTarget = kTargetsList.front();
	object_id_type lTargetID = pkTarget->GetID();
	if(!IS_CHEST_ID(lTargetID))
		return;

	pkPlayer->GetFlopManager()->RequestPickItem(lTargetID);
}
//-----------------------------------------------------------------------------
void SC_ClientSkillInterface::TalkToNPC(LC_Attacker* pkScriptCaller,int iTargetIndex )
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkMC = pkLogicManger->GetOperationPlayer();

	object_id_type lCallerID = pkScriptCaller->GetID();

	if(pkMC->GetID() != lCallerID)
		return;

	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller,iTargetIndex, kTargetsList);
	T_ASSERT(kTargetsList.size() <= 1);
	if(kTargetsList.size() == 0)
	{
		pkMC->SetLockedTargetID(0);
		return;
	}

	LC_ActorBase* pkTarget = kTargetsList.front();
	if(!pkTarget)
		return;

	if (!IS_NPC_ID(pkTarget->GetID()))
	{
		return;
	}
	
	pkMC->RequestTalkToNPC( pkTarget->GetID() );
}
//------------------------------------------------------------------------
void SC_ClientSkillInterface::CollectItem(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex)
{
#pragma TODO("类型转换逻辑")
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	object_id_type lGamePlayerID = pkLogicManager->GetOperationPlayerID();
	if(lGamePlayerID != pkScriptCaller->GetID())
		return;

	LC_ClientGamePlayer* pkPlayer = (LC_ClientGamePlayer*)pkScriptCaller;

	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller,iTargetIndex, kTargetsList);
	if(kTargetsList.size() != 1)
	{
		pkPlayer->SetLockedTargetID(0);
		return;
	}

	LC_ActorBase* pkTarget = kTargetsList.front();
	object_id_type lTargetID = pkTarget->GetID();
	//	T_ASSERT(IS_CHEST_ID(lTargetID));
	if(lTargetID <= 0)
		return;

	pkPlayer->RequestCollectItem(lTargetID);

}
//------------------------------------------------------------------------
void SC_ClientSkillInterface::StartRevealInvisibleTarget(GameLogic::LC_Attacker *pkScriptCaller, int iTargetIndex, int iCircleRadius, bool bRevealOnce)
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

	if (NULL == pkScriptCaller || pkScriptCaller->GetID() != pkGamePlayer->GetID())
	{
		return;
	}

	UT_Vec3Int kCenterPoint;
	if(iTargetIndex == TARGET_LOCATION)
		kCenterPoint = pkScriptCaller->GetLockedLocation();
	else
	{
		LC_ActorPtrList kTargetsList;
		_findTarget(pkScriptCaller,iTargetIndex, kTargetsList);
		if(kTargetsList.size() != 1)
		{
			GfxWriteLog(LOG_WARNING_FILELINE, LOG_SWITCH_WARNING,"脚本:试图用非法目标位置作区域锁定");
			return;
		}

		LC_ActorBase* pkTarget = kTargetsList.front();
		if(!pkTarget)
		{
			GfxWriteLog(LOG_WARNING_FILELINE, LOG_SWITCH_WARNING,"脚本:试图用非法目标位置作区域锁定");
			return;
		}

		kCenterPoint = pkTarget->GetCurrentLocation();
	}

	pkGamePlayer->SetRevealInvisibilityParams(kCenterPoint, iCircleRadius, bRevealOnce);

}
//------------------------------------------------------------------------
bool	SC_ClientSkillInterface::CanGetTaskItem(int lItemID)
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(pkGamePlayer == NULL)
		return false;

	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	LC_ClientItem*  pkItem = (LC_ClientItem* )pkItemFactory->RequestItem(lItemID);
	if (pkItem == NULL)
		return false;

	if (pkItem->GetUseWeaponTypeLimit() > WT_NONE && pkGamePlayer->GetWeaponAttribute() != pkItem->GetUseWeaponTypeLimit() )
	{
		return false;
	}

	if (pkGamePlayer->GetWeaponAttribute() != (WeaponType)pkItem->GetEquiplWeaponType())
		return false;

	if (pkItem->GetUseCareerLimitID() > 0 && pkItem->GetUseCareerLimitID() != pkGamePlayer->GetCareerType())
	{
		return false;
	}

	return true;

}
//------------------------------------------------------------------------
bool	SC_ClientSkillInterface::IsTaskInCurrentArea(int lTaskID)
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(pkGamePlayer == NULL)
		return false;

	LC_ClientTask* pkClientTask = NULL;
	LC_ClientTaskManager* pkTaskManager = (LC_ClientTaskManager*)LC_ClientTaskManager::GetSingletonPtr();
	if (NULL == pkTaskManager)
		return false;

	pkClientTask = (LC_ClientTask*)(pkTaskManager->GetTask(lTaskID));
	if (NULL == pkClientTask)
		return false;

	LC_MapBase*	pkMap = pkGamePlayer->GetMap();
	if (NULL == pkMap)
		return false;

	LC_LogicAreaInfo* kLogicAreaInfo = LC_LogicMapManager::GetSingletonPtr()->QueryAreaInfo(pkGamePlayer->GetCurrentLocation(),pkGamePlayer->GetMapResID());

	StringType an = kLogicAreaInfo != NULL ? kLogicAreaInfo->kAreaName : "";
	StringType str = pkClientTask->GetTaskSite();
	StringType::size_type stHead = 0;
	StringType::size_type stTail = str.find(" ");
	if (stTail == StringType::npos)
	{
		if (str == an)
			return true;
	}

	while(stTail != StringType::npos) 
	{
		if (str.substr(stHead, stTail - stHead) == an)
			return true;

		stHead = stTail + 1;
		stTail = str.find(" ", stHead);
		if (stTail == StringType::npos)
		{
			if (str.substr(stHead) == an)
				return true;
		}
	}

	return false;

}
//---------------------------------------------------------------------------
int SC_ClientSkillInterface::GetItemEquipLimit(int lItemID)
{
	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	LC_ItemBase*  pkItem = pkItemFactory->RequestItem(lItemID);
	if (pkItem == NULL)
	{
		return -1;
	}
	
	return pkItem->GetEquipTypeLimit();
}

int SC_ClientSkillInterface::GetGetSpecialTrainingCount()
{
	return CF_SpecialTraining::GetSingletonPtr()->GetEntryCount();
}

//---------------------------------------------------------------------------
void SC_ClientSkillInterface::StartDebugConsole(const char* szPath)
{
}
//------------------------------------------------------------------------------
int SC_ClientSkillInterface::GetEnhanceLimitFuncID(int nIndex)
{
	return 0;
}
//------------------------------------------------------------------------------
int SC_ClientSkillInterface::GetEnhanceNeedMaterialID(int nIndex)
{
	return 0;
}
//------------------------------------------------------------------------------
int SC_ClientSkillInterface::GetEnhanceNeedMaterialNum(int nIndex)
{
	return 0;
}
//------------------------------------------------------------------------------
int SC_ClientSkillInterface::GetItemEnhanceLevel(int nIndex)
{
	return 0;
}
//------------------------------------------------------------------------------
int SC_ClientSkillInterface::GetItemEnhanceLevelLimit(int nIndex)
{
	return -1;
}
//------------------------------------------------------------------------------
int SC_ClientSkillInterface::GetEnhanceNeedRuneNum(int nIndex)
{
	return 0;
}
//------------------------------------------------------------------------------
bool SC_ClientSkillInterface::StartIdentifySkill(int iBackPackType,int iBackPackIndex)
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)LC_ClientLogicManager::GetSingletonPtr();
	LC_ClientGamePlayer* pkPlayer = pkLogicManger->GetOperationPlayer();
	if (NULL == pkPlayer)
		return false;

	//启动鉴定技能
	pkPlayer->PendingSkill(SKILL_IDENTIFY_ITEM,0);

	//设置启动技能的参数：信物ID
	pkPlayer->SetSkillStartParameter(0, iBackPackType);
	pkPlayer->SetSkillStartParameter(1, iBackPackIndex);

	return true;
}
//------------------------------------------------------------------------------

StringType SC_ClientSkillInterface::GetProductionTypeName(int lProductionType)
{
	switch(lProductionType)
	{
	case PT_TAILOR:
		return GET_MSG(SC_CLIENTSKILLINTERFACE_CPP_003);
	case PT_BLACKSMITH:
		return GET_MSG(SC_CLIENTSKILLINTERFACE_CPP_004);
	case PT_ORNAMENT:
		return GET_MSG(SC_CLIENTSKILLINTERFACE_CPP_005);
	case PT_PHARMACIST:
		return GET_MSG(SC_CLIENTSKILLINTERFACE_CPP_006);
	default:
		return "";
	}
}
//------------------------------------------------------------------------------
StringType SC_ClientSkillInterface::GetItemEnhanceFuncDescript(int lItemEnhanceNextLevel)
{
	return "";
}
//------------------------------------------------------------------------------
bool SC_ClientSkillInterface::IsEnhanceRune(int lItemTypeID)
{
	return false;
}
//------------------------------------------------------------------------------
bool SC_ClientSkillInterface::CheckNeedUnBindCashDialog(int iItemID, bool bConfirmUseUnBindCash)
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)LC_ClientLogicManager::GetSingletonPtr();
	LC_ClientGamePlayer* pkPlayer = pkLogicManger->GetOperationPlayer();
	if (NULL == pkPlayer)
		return false;

	CF_ItemList* pkCSVItemList = CF_ItemList::GetSingletonPtr();
	CF_ItemList::DataEntry* pkItemData = pkCSVItemList->GetEntryPtr(iItemID);

	if (!pkItemData)
		return false;

	int lNeedCash = 0;

	if (!bConfirmUseUnBindCash)
	{
		if ( pkPlayer->IsNeedUnBindCashItemEnhance(lNeedCash) )
		{
			SendMessageDialog(RE_DIALOG_CONFIRM_USE_UNBINDCASH_NORMAL_ENHANCE, 
				"", "", RE_DIALOG_CONFIRM_USE_UNBINDCASH_NORMAL_ENHANCE, 
				0, 0, 0);

			return false;
		}
	}

	return true;
}
//------------------------------------------------------------------------------
StringType SC_ClientSkillInterface::GetEnhanceEquipLevelProperty(int iEqupSlotIdx, int iEquipReinforceLevel)
{
	LC_ActorAttributeMap attrMap;
	LC_ActorAttributeMap attrNextMap;

	for(int i=iEquipReinforceLevel; i<=(iEquipReinforceLevel+1); ++i)
	{
		const CF_EquipSlotReinforce::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_EquipSlotReinforce>(i);
		if(pkEntry != NULL)
		{
			int iAttribID = 0;
			switch(iEqupSlotIdx)
			{
			case ET_HEAD:	//新武器
				iAttribID = pkEntry->_lAttrFuncID1;
				break;
			case ET_BODY:	//新铠甲
				iAttribID = pkEntry->_lAttrFuncID2;
				break;
			case ET_FOOT:	//新饰品
				iAttribID = pkEntry->_lAttrFuncID4;
				break;
			case ET_NECKLACE:	//新头盔
				iAttribID = pkEntry->_lAttrFuncID3;
				break;
			}
			
			if(iAttribID > 0)
			{
				if(i == iEquipReinforceLevel)
					LC_Helper::AttributeMapByAttrFunc(&attrMap, iAttribID);
				else
					LC_Helper::AttributeMapByAttrFunc(&attrNextMap, iAttribID);
			}
		}
	}

	attrMap.RecomputeAll();
	attrNextMap.RecomputeAll();

	StringStreamType sStream;
	sStream << _getAttribString(&attrMap) << "@" << _getAttribString(&attrNextMap);
	return sStream.str();
}

StringType SC_ClientSkillInterface::GetEnhanceEquipCost(int iEqupSlotIdx, int iEquipReinforceLevel )
{
	const CF_EquipSlotReinforce::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_EquipSlotReinforce>(iEquipReinforceLevel);
	if(pkEntry == NULL)
		return "";

	StringType sItem;
	FormatDataIFs2String(pkEntry->_kItem, sItem);
	return sItem;
}

int SC_ClientSkillInterface::GetEnhanceEquipNeedLevel(int iEquipReinforceLevel )
{
	const CF_EquipSlotReinforce::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_EquipSlotReinforce>(iEquipReinforceLevel);
	if(pkEntry == NULL)
		return 0;

	return pkEntry->_iBuddyLevel;
}

int SC_ClientSkillInterface::GetEnhanceEquipNeedEquipLevel(int iEquipReinforceLevel)
{
	const CF_EquipSlotReinforce::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_EquipSlotReinforce>(iEquipReinforceLevel);
	if(pkEntry == NULL)
		return 0;

	return pkEntry->_iEquipLevel;
}

int SC_ClientSkillInterface::GetEquipSlotMaxLevel()
{
	return CF_EquipSlotReinforce::GetSingletonPtr()->GetEntryCount();
}

int64_t SC_ClientSkillInterface::GetEquipSlotScore(int iEqupSlotIdx, int iEquipReinforceLevel, int iEquipStar)
{
	return LC_Helper::GetEquipSlotScore(iEqupSlotIdx, iEquipReinforceLevel, iEquipStar);
}
//------------------------------------------------------------------------------

int SC_ClientSkillInterface::_getCommanderEquipAttribID(const CF_EquipCommanderSlotReinforce::DataEntry *pkEntry, int iEquipSlotIdx)
{
	switch(iEquipSlotIdx)
	{
	case ET_HEAD:	//新武器
		return pkEntry->_lAttrFuncID1;
	case ET_BODY:	//新铠甲
		return pkEntry->_lAttrFuncID2;
	case ET_FOOT:	//新饰品
		return pkEntry->_lAttrFuncID4;
	case ET_NECKLACE:	//新头盔
		return pkEntry->_lAttrFuncID3;
	case ET_SHIELD:		//新腰带
		return pkEntry->_lAttrFuncID6;
	case ET_BARCER:		//新护手
		return pkEntry->_lAttrFuncID5;
	case ET_GAITER:		//新护腿
		return pkEntry->_lAttrFuncID7;
	case ET_CLOAK:		//新鞋子
		return pkEntry->_lAttrFuncID8;
	default:
		//错误的iEqupSlotIdx
		T_ASSERT_CRITICAL_S(false);
	}

	return 0;
}

StringType SC_ClientSkillInterface::GetCommanderReinforceEquipProperty(int iEqupSlotIdx, int iEquipReinforceLevel)
{
	LC_ActorAttributeMap attrMap;
	LC_ActorAttributeMap attrNextMap;
	const CF_EquipCommanderSlotReinforce::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_EquipCommanderSlotReinforce>(iEquipReinforceLevel);
	if(pkEntry != NULL)
	{
		int iAttribID = _getCommanderEquipAttribID(pkEntry, iEqupSlotIdx);
		LC_Helper::AttributeMapByAttrFunc(&attrMap, iAttribID);
	}
	
	pkEntry = SafeGetCSVFileDataEntryPtr<CF_EquipCommanderSlotReinforce>(++iEquipReinforceLevel);
	if(pkEntry != NULL)
	{
		int iAttribID = _getCommanderEquipAttribID(pkEntry, iEqupSlotIdx);
		LC_Helper::AttributeMapByAttrFunc(&attrNextMap, iAttribID);
	}
	
	attrMap.RecomputeAll();
	attrNextMap.RecomputeAll();

	StringStreamType sStream;
	sStream << _getAttribString(&attrMap) << "@" << _getAttribString(&attrNextMap);
	return sStream.str();
}

StringType SC_ClientSkillInterface::GetCommanderReinforceEquipCost(int iEqupSlotIdx, int iEquipReinforceLevel )
{
	const CF_EquipCommanderSlotReinforce::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_EquipCommanderSlotReinforce>(iEquipReinforceLevel);
	if(pkEntry == NULL)
		return "";

	StringType sItem;
	FormatDataIFs2String(pkEntry->_kItem, sItem);
	return sItem;
}

int SC_ClientSkillInterface::GetCommanderReinforceEquipNeedLevel(int iEquipReinforceLevel )
{
	const CF_EquipCommanderSlotReinforce::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_EquipCommanderSlotReinforce>(iEquipReinforceLevel);
	if(pkEntry == NULL)
		return 0;

	return pkEntry->_iCommanderLevel;
}

int SC_ClientSkillInterface::GetCommanderEquipSlotMaxLevel()
{
	return CF_EquipCommanderSlotReinforce::GetSingletonPtr()->GetEntryCount();
}

StringType SC_ClientSkillInterface::GetCommanderRefineEquipInfo(int iEqupSlotIdx, int iEquipReinforceLevel)
{
	LC_ActorAttributeMap attrMap;
	LC_ActorAttributeMap attrNextMap;
	StringStreamType sCommentStream;

	CF_EquipCommanderJinglian::DataEntryMapExternIterator iter = CF_EquipCommanderJinglian::GetSingletonPtr()->GetIterator();
	for(; iter.HasMoreElements(); iter.MoveNext())
	{
		const CF_EquipCommanderJinglian::DataEntry *pkEntry = iter.PeekNextValuePtr();
		if(pkEntry->_iEquipType == iEqupSlotIdx)
		{
			const int curLevel = pkEntry->_iJinglianLevel;
			if(curLevel == iEquipReinforceLevel)
			{
				const Utility::UT_SIMDataList& attr = pkEntry->Attribute;
				for(Utility::UT_SIMDataList::const_iterator it=attr.begin(); it!=attr.end(); ++it)
				{
					attrMap.ModifyAttributeByPart(it->ParamA(), it->ParamB(), it->ParamC(), true);
				}
			}
			else if(curLevel == (iEquipReinforceLevel + 1))
			{
				const Utility::UT_SIMDataList& attr = pkEntry->Attribute;
				for(Utility::UT_SIMDataList::const_iterator it=attr.begin(); it!=attr.end(); ++it)
				{
					attrNextMap.ModifyAttributeByPart(it->ParamA(), it->ParamB(), it->ParamC(), true);
				}
			}

			if(!pkEntry->_Descript.empty())
				sCommentStream << curLevel << '&' << pkEntry->_Descript << '_';
		}
	}

	attrMap.RecomputeAll();
	attrNextMap.RecomputeAll();

	StringStreamType sStream;
	sStream << _getAttribString(&attrMap) << '@' << _getAttribString(&attrNextMap) << '@' << sCommentStream.str();
	return sStream.str();
}

StringType SC_ClientSkillInterface::GetCommanderRefineEquipCostInfo(int iEqupSlotIdx, int iEquipRefineLevel)
{
	const CF_EquipCommanderJinglian::DataEntry* pkCurEntry = NULL;
	const CF_EquipCommanderJinglian::DataEntry* pkNxtEntry = NULL;
	int iMaxStar = 0;

	CF_EquipCommanderJinglian::DataEntryMapExternIterator kIter = CF_EquipCommanderJinglian::GetSingleton().GetIterator();
	while( kIter.HasMoreElements() )
	{
		const CF_EquipCommanderJinglian::DataEntry* pkData = kIter.PeekNextValuePtr();
		const int iCurLevel = pkData->_iJinglianLevel;
		if (pkData->_iEquipType == iEqupSlotIdx && iCurLevel >= iEquipRefineLevel)
		{
			if(iCurLevel == iEquipRefineLevel)
				pkCurEntry = pkData;
			else if(iCurLevel == (iEquipRefineLevel + 1))
				pkNxtEntry = pkData;

			if(iCurLevel > iMaxStar)
				iMaxStar = iCurLevel;
		}

		kIter.MoveNext();
	}

	StringStreamType sStream;
	if(pkCurEntry != NULL)
	{
		sStream << pkCurEntry->_PropertySpecialChange << '@' << pkCurEntry->_ConsumeMoney << '@';
		sStream << pkCurEntry->_iCommanderLevel << '@' << pkCurEntry->_SuccessRate << '@' << iMaxStar << '@';

		StringType sItem;
		FormatDataIFs2String(pkCurEntry->_kItem, sItem);
		sStream << sItem << '@';
		if(pkNxtEntry != NULL)
			sStream << pkNxtEntry->_PropertySpecialChange << '@';
	}
	return sStream.str();
}

StringType SC_ClientSkillInterface::GetCommanderModingSpiritEquipCostInfo(int iPackType, int iEqupSlotIdx, int iEquipRefineLevel )
{
	const CF_EquipCommanderSlotMoldingSpirit::DataEntry* pkCurEntry = NULL;
	const CF_EquipCommanderSlotMoldingSpirit::DataEntry* pkNxtEntry = NULL;
	int iMaxLevel = 0;

	CF_EquipCommanderSlotMoldingSpirit::DataEntryMapExternIterator kIter = CF_EquipCommanderSlotMoldingSpirit::GetSingleton().GetIterator();
	while( kIter.HasMoreElements() )
	{
		const CF_EquipCommanderSlotMoldingSpirit::DataEntry* pkData = kIter.PeekNextValuePtr();
		const int iCurLevel = pkData->_iLevel;
		if (pkData->iType == iPackType 
			&& pkData->_iEquipType == iEqupSlotIdx
			&& iCurLevel >= iEquipRefineLevel)
		{
			if(iCurLevel == iEquipRefineLevel)
				pkCurEntry = pkData;
			else if(iCurLevel == (iEquipRefineLevel + 1))
				pkNxtEntry = pkData;

			if(iCurLevel > iMaxLevel)
				iMaxLevel = iCurLevel;
		}

		kIter.MoveNext();
	}

	LC_ActorAttributeMap attrMap;

	StringStreamType sStream;
	if(pkCurEntry != NULL)
	{
		sStream << pkCurEntry->_iSuitID << '@' << pkCurEntry->_iEffectLevel << '@';
		sStream << pkCurEntry->_iCommanderLevel << '@' << pkCurEntry->_sSuitTitle << '@';
		sStream << iMaxLevel << '@' << pkCurEntry->_iCommanderEquipLevel << '@';

		LC_Helper::AttributeMapByAttrFunc(&attrMap, pkCurEntry->_lAttrFuncID);
		attrMap.RecomputeAll();
		sStream << _getAttribString(&attrMap);
		sStream << '@';

		StringType sItem;
		FormatDataIFs2String(pkCurEntry->_kItem, sItem);
		sStream << sItem << '@';
		if(pkNxtEntry != NULL)
		{
			sStream << pkNxtEntry->_iSuitID << '@' << pkNxtEntry->_iEffectLevel << '@';
			sStream << pkNxtEntry->_sSuitTitle << '@';

			attrMap.Clear();
			LC_Helper::AttributeMapByAttrFunc(&attrMap, pkNxtEntry->_lAttrFuncID);
			attrMap.RecomputeAll();
			sStream << _getAttribString(&attrMap);
		}
	}
	return sStream.str();
}

StringType SC_ClientSkillInterface::GetModingSpiritSuitInfo()
{
	StringStreamType sStream;

	CF_EquipCommanderSlotMoldingSpiritSuit::DataEntryMapExternIterator itr = CF_EquipCommanderSlotMoldingSpiritSuit::GetSingletonPtr()->GetIterator();
	for( ; itr.HasMoreElements(); itr.MoveNext())
	{
		const CF_EquipCommanderSlotMoldingSpiritSuit::DataEntry* pkData = itr.PeekNextValuePtr();

		sStream << pkData->_iSuitID << '@' << pkData->_kSuitName << '@' << pkData->_sPropDesc << '@';

		const Utility::UT_SIMDataList& attr = pkData->_iSuitProp;
		for(Utility::UT_SIMDataList::const_iterator it=attr.begin(); it!=attr.end(); ++it)
		{
			sStream << it->ID() << '*' << it->ParamA() << '*' << it->ParamB();
			sStream << '&';
		}
		sStream << '_';
	}
	
	return sStream.str();
}

combat_score_type SC_ClientSkillInterface::GetCommanderEquipScore(int iEquipSlotIdx)
{
	LC_ClientGamePlayer* pkGamePlayer = GetPlayerPtr();
	if(pkGamePlayer == NULL)
		return 0;

	LC_ActorAttributeMap attrMap;
	LC_BackPackEntry* pkEntry = pkGamePlayer->GetEquipEntry(CommandPack, iEquipSlotIdx);
	if(pkEntry->GetValid())
	{
		LC_ItemBase* pkItem = LC_ItemFactoryBase::GetSingletonPtr()->RequestItem(pkEntry->GetTypeID());
		if(pkItem != NULL)
			pkItem->ComputerAttribMapModify(&attrMap, NULL);
	}
	
	int iReinforceLevel = pkGamePlayer->GetCommanderEquipReinforceLevel(iEquipSlotIdx);
	if (iReinforceLevel > 0)
	{
		const CF_EquipCommanderSlotReinforce::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_EquipCommanderSlotReinforce>(iReinforceLevel);
		if(pkEntry != NULL)
		{
			int iAttribID = _getCommanderEquipAttribID(pkEntry, iEquipSlotIdx);
			LC_Helper::AttributeMapByAttrFunc(&attrMap, iAttribID);
		}
	}

	int iRefineLevel = pkGamePlayer->GetCommanderEquipRefineLevel(iEquipSlotIdx);
	if(iRefineLevel > 0)
	{
		CF_EquipCommanderJinglian::DataEntryMapExternIterator iter = CF_EquipCommanderJinglian::GetSingletonPtr()->GetIterator();
		for(; iter.HasMoreElements(); iter.MoveNext())
		{
			const CF_EquipCommanderJinglian::DataEntry *pkEntry = iter.PeekNextValuePtr();
			if(pkEntry->_iEquipType == iEquipSlotIdx
				&& pkEntry->_iJinglianLevel == iRefineLevel)
			{
				const Utility::UT_SIMDataList& attr = pkEntry->Attribute;
				for(Utility::UT_SIMDataList::const_iterator it=attr.begin(); it!=attr.end(); ++it)
				{
					attrMap.ModifyAttributeByPart(it->ParamA(), it->ParamB(), it->ParamC(), true);
				}
				break;
			}
		}
	}

	int iModingSpiritLevel = pkGamePlayer->GetCommanderEquipModingSpiritLevel(CommandPack, iEquipSlotIdx);
	if(iModingSpiritLevel > 0)
	{
		CF_EquipCommanderSlotMoldingSpirit::DataEntryMapExternIterator kIter = CF_EquipCommanderSlotMoldingSpirit::GetSingleton().GetIterator();
		for( ; kIter.HasMoreElements(); kIter.MoveNext() )
		{
			const CF_EquipCommanderSlotMoldingSpirit::DataEntry* pkData = kIter.PeekNextValuePtr();
			const int iCurLevel = pkData->_iLevel;
			if (pkData->iType == 100 
				&& pkData->_iEquipType == iEquipSlotIdx
				&& iCurLevel == iModingSpiritLevel)
			{
				LC_Helper::AttributeMapByAttrFunc(&attrMap, pkData->_lAttrFuncID);
				break;
			}
		}
	}

	attrMap.RecomputeAll();
	return LC_Helper::CalcAttributeMapCombatScore(&attrMap, false);
}

//------------------------------------------------------------------------------
StringType SC_ClientSkillInterface::GetBreakEquipDescription(int iEqupSlotIdx, int iEquipBreakLevel)
{
	CF_EquipSlotStar::DataEntryMapExternIterator kIter = CF_EquipSlotStar::GetSingleton().GetIterator();

	const CF_EquipSlotStar::DataEntry* pkCur = NULL;
	int iMaxStar = 0;
	while( kIter.HasMoreElements() )
	{
		const CF_EquipSlotStar::DataEntry* pkData = kIter.PeekNextValuePtr();
		if (pkData->_StarUpType == iEqupSlotIdx && pkData->_StarLevel >= iEquipBreakLevel)
		{
			if(pkData->_StarLevel == iEquipBreakLevel)
				pkCur = pkData;

			if(pkData->_StarLevel > iMaxStar)
				iMaxStar = pkData->_StarLevel;
		}

		kIter.MoveNext();
	}
	
	if(pkCur == NULL)
		return "";

	StringType sItem;
	FormatDataIFs2String(pkCur->_kItem, sItem);

	StringStreamType sStream;
	sStream << pkCur->_ConsumeMoney << '|'<< sItem << '|' << (int)pkCur->_bBreak << '|';
	sStream << (int)pkCur->_StarLevelClient << '|' << pkCur->_iBuddyLevel << '|' << pkCur->_quality << '|' << iMaxStar;
	return sStream.str();
}

int SC_ClientSkillInterface::GetEquipStarQuality(int iStarUpType, int iStarLevel)
{
	CF_EquipSlotStar::DataEntryMapExternIterator kIter = CF_EquipSlotStar::GetSingletonPtr()->GetIterator();
	for(; kIter.HasMoreElements(); kIter.MoveNext())
	{
		const CF_EquipSlotStar::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		if(pkEntry->_StarUpType == iStarUpType && pkEntry->_StarLevel == iStarLevel)
			return pkEntry->_quality;
	}
	return 0;
}
//------------------------------------------------------------------------------
StringType SC_ClientSkillInterface::GetProductionInfo(int lFormulaID)
{
	StringStreamType sStream;
	CF_ProductionFormula* pkFormulaList = CF_ProductionFormula::GetSingletonPtr();
	CF_ProductionFormula::DataEntry* pkFormulaData = pkFormulaList->GetEntryPtr(lFormulaID);
	if (pkFormulaData)
	{
		sStream << pkFormulaData->_iID << "#" << pkFormulaData->_kFormulaName << "#" << pkFormulaData->_lUIPriority << "#"
			<< pkFormulaData->_lCash << "#" << pkFormulaData->_bClientShow << "#"
			<< pkFormulaData->_lUIType << "#" << pkFormulaData->_lUIBigType << "#"
			<< pkFormulaData->_lMadeItemID << "#" << pkFormulaData->_lMadeItemNum << "#"
			<< pkFormulaData->_lMaterial1ID << "#" << pkFormulaData->_lMaterial1Num << "#"
			<< pkFormulaData->_lMaterial2ID << "#" << pkFormulaData->_lMaterial2Num << "#"
			<< pkFormulaData->_lMaterial3ID << "#" << pkFormulaData->_lMaterial3Num << "#"
			<< pkFormulaData->_lMaterial4ID << "#" << pkFormulaData->_lMaterial4Num << "#"
			<< pkFormulaData->_lMaterial5ID << "#" << pkFormulaData->_lMaterial5Num << "#"
			<< pkFormulaData->_lMaterial6ID << "#" << pkFormulaData->_lMaterial6Num << "#";
	}
	return sStream.str();
}
//------------------------------------------------------------------------------
StringType SC_ClientSkillInterface::GetItemName(int lItemID)
{
	CF_ItemList* pkItemList = CF_ItemList::GetSingletonPtr();
	CF_ItemList::DataEntry* pkItemData = pkItemList->GetEntryPtr(lItemID);

	if (pkItemData)
	{
		return pkItemData->_kName;
	}

	return "";
}
//------------------------------------------------------------------------------
int SC_ClientSkillInterface::GetCurLevelMaxMastery(int lProductionLevel)
{
	return 0;
}
//------------------------------------------------------------------------------
int SC_ClientSkillInterface::GetProductionNeedLevel(int lProductionLevel)
{
	return 0;
}
//------------------------------------------------------------------------------
int SC_ClientSkillInterface::GetCanProductionNum(int lFormulaID)
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkMC = pkLogicManger->GetOperationPlayer();

	if (!pkMC)
		return 0;

	CF_ProductionFormula* pkFormulaList = CF_ProductionFormula::GetSingletonPtr();
	CF_ProductionFormula::DataEntry* pkFormulaData = pkFormulaList->GetEntryPtr(lFormulaID);

	if (!pkFormulaData)
		return 0;

	int lCanProductionNum = 100000;
	//检查每一个材料的可制作数量，取最小值
	for (int i = 1; i <= MAX_PRODUCTION_MATERIAL_COUNT; ++i)
	{
		int lNum = 0;
		int lMaterialID =CSVHelper::GetProductionMaterialID(pkFormulaData, i);

		if (lMaterialID != 0)
		{
			if(!IS_ITEM_ID(lMaterialID))
				return 0;

			int lNeedMaterialNum = CSVHelper::GetProductionMaterialNum(pkFormulaData, i);
			int lPlayerMaterialNum = pkMC->GetCanCostItemCount(lMaterialID);

			lNum = lPlayerMaterialNum / lNeedMaterialNum;
			if (lNum == 0)
				return 0;

			if (lNum < lCanProductionNum)
				lCanProductionNum = lNum;
		}
	}

	return lCanProductionNum;
}
//------------------------------------------------------------------------------
int SC_ClientSkillInterface::GetMadeItemGrade(int lItemTypeID)
{
	if(lItemTypeID == 0)
		return ITEM_GRADE_GRAY;

	CF_ItemList* pkCSVItemList = CF_ItemList::GetSingletonPtr();
	CF_ItemList::DataEntry* pkItemaData = pkCSVItemList->GetEntryPtr(lItemTypeID);
	if(pkItemaData != NULL)
		return pkItemaData->_lGrade;

	GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"SC_ClientSkillInterface::GetMadeItemGrade 不存在的道具id = %d", lItemTypeID);
	return ITEM_GRADE_GRAY;
}
//------------------------------------------------------------------------------
void SC_ClientSkillInterface::AddCanLearnFormulaEntry()
{
	return;
}
//------------------------------------------------------------------------------
void SC_ClientSkillInterface::RequestInterruptProduction()
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)LC_ClientLogicManager::GetSingletonPtr();
	LC_ClientGamePlayer* pkPlayer = pkLogicManger->GetOperationPlayer();

	if (!pkPlayer)
		return;

	if (pkPlayer->GetDead())
	{
		SendLogicInfoMessage(RE_PRODUCTION_YOU_ARE_DEAD);
		return;
	}
	int nCurrentStateType = pkPlayer->GetCurrentStateType();
	if (nCurrentStateType == SKILL_PRODUCTION_TAILOR || nCurrentStateType == SKILL_PRODUCTION_BLACKSMITH ||
		nCurrentStateType == SKILL_PRODUCTION_ORNAMENT || nCurrentStateType == SKILL_PRODUCTION_PHARMACIST )
		pkPlayer->InterruptSkill(GET_PROCESS_TIME, nCurrentStateType, SKI_NONE);
}

//------------------------------------------------------------------------------
void SC_ClientSkillInterface::StartProduceSkill( GameLogic::LC_Attacker* pkScriptCaller,GameLogic::LC_ActorOperationEntry* pkOperation )
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)LC_ClientLogicManager::GetSingletonPtr();
	LC_ClientGamePlayer* pkPlayer = pkLogicManger->GetOperationPlayer();

	if (!pkPlayer)
		return;
}
//------------------------------------------------------------------------------
void SC_ClientSkillInterface::LearnProduction(int lProductionType, int lProductionLevel)
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkMC = pkLogicManger->GetOperationPlayer();

	if (!pkMC)
		return;

	pkMC->LearnProduction(lProductionType, lProductionLevel);
}
//------------------------------------------------------------------------------
bool SC_ClientSkillInterface::ForgetProduction()
{
	return true;
	/*
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkMC = pkLogicManger->GetOperationPlayer();

	if (!pkMC)
		return false;

	int lLearnType = pkMC->GetProductionType();
	StringType strProductionName = "";

	switch(lLearnType)
	{
	case PT_TAILOR:
		strProductionName = "裁缝";
		break;
	case PT_BLACKSMITH:
		strProductionName = "铁匠";
		break;
	case PT_ORNAMENT:
		strProductionName = "玉师";
		break;
	case PT_PHARMACIST:
		strProductionName = "药师";
		break;
	default:
		break;
	}

	if (lLearnType != PT_NONE)
	{
		//if (pkMC->GetProductionType() != lProductionType)
		//{
		//	SendLogicInfoMessage(RE_PRODUCTION_NPC_ERROR, strProductionName.c_str());
		//	return false;
		//}

		MG_MessageCenter* pkMessageCenter = MG_MessageCenter::GetSingletonPtr();
		if(!pkMessageCenter)
			return false;

		char kOutputMessage[256];
		sprintf(kOutputMessage,"是否确定要遗忘%s技能",strProductionName.c_str());

		//pkMessageCenter->SendMessage(CreateMsg_LOGIC_SYSTEM_INFO(kOutputMessage,SIT_CONFIRM_FORGET_PRODUCTION,lProductionType,NO_USE_PARAMETER,NO_USE_PARAMETER),true);

		SendMessageDialog(RE_DIALOG_CONFIRM_FORGET_PRODUCTION,
			strProductionName,"",
			MDT_CONFIRM_FORGET_PRODUCTION,lLearnType,NO_USE_PARAMETER,NO_USE_PARAMETER);
		return true;
	}

	return false;
	*/
}
//------------------------------------------------------------------------------
void SC_ClientSkillInterface::UpgradeProduction(int lProductionType)
{
	return;
	/*
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkMC = pkLogicManger->GetOperationPlayer();

	if (!pkMC)
		return;

	pkMC->UpgradeProduction(lProductionType);
	*/
}
//------------------------------------------------------------------------------
bool SC_ClientSkillInterface::LearnBaseFormula(int lFormulaID)
{
	return true;
	/*
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkMC = pkLogicManger->GetOperationPlayer();

	if (!pkMC)
		return false;

	int lPlayerMastery = pkMC->GetCurrentMastery();
	int lPlayerProductionType = pkMC->GetProductionType();

	CF_ProductionFormula* pkCSVFormula = CF_ProductionFormula::GetSingletonPtr();
	CF_ProductionFormula::DataEntry* pkFormulaData = pkCSVFormula->GetEntryPtr(lFormulaID);
	if(!pkFormulaData)
		return false;

	int lMinMastery = pkFormulaData->_lMinMastery;
	int lProductionType = pkFormulaData->_lProductionType;
	bool bIsLearned = pkMC->GetProductionAsset()->IsLearnedProductionFormula(lFormulaID);

	if (lProductionType != lPlayerProductionType)
		return false;

	if (bIsLearned)
	{
		SendLogicInfoMessage(RE_PRODUCTION_LEARN_FORMULA_ALREADY);
		return false;
	}
		

	if (lPlayerMastery < lMinMastery)
	{ 
		SendLogicInfoMessage(RE_LEARN_FORMULA_FAIL, lMinMastery);
		return false;
	}
		
	pkMC->LearnFormula(lFormulaID);
	return true;
	*/
}
//------------------------------------------------------------------------------
bool SC_ClientSkillInterface::IsLearnedProduction()
{
	return false;
	/*
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkMC = pkLogicManger->GetOperationPlayer();

	if (!pkMC)
		return false;

	if (0 == pkMC->GetProductionType())
	{
		return false;
	}

	return true;
	*/
}
//------------------------------------------------------------------------------
bool SC_ClientSkillInterface::SetBitVec(int nIndex, bool bSet)
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkMC = pkLogicManger->GetOperationPlayer();
	if(pkMC == NULL)
		return false;

	bool b = pkMC->SetBitVec(nIndex, bSet);
	return b;
}
//------------------------------------------------------------------------------
bool SC_ClientSkillInterface::IsEquipID(int lItemID)
{
	return IS_EQUIP_ID(lItemID);
}
bool SC_ClientSkillInterface::IsMedicineID(int iItemID)
{
	return IS_MEDICINE_ITEM_ID(iItemID);
}
//------------------------------------------------------------------------------
bool SC_ClientSkillInterface::IsQinggongSkillID(int iSkillID)
{
	return IS_QINGGONG_SKILL_ID(iSkillID);
}
//------------------------------------------------------------------------------
bool SC_ClientSkillInterface::IsGemID(int lItemID)
{
	return IS_GEM_TYPE(lItemID);
}
bool SC_ClientSkillInterface::IsSkillBookID(int lItemID)
{
	return IS_SKILL_BOOK_ID(lItemID);
}
//------------------------------------------------------------------------------
void SC_ClientSkillInterface::TriggerNPCFunc(int nTaskState)
{
	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_CLICK_NPC_FUNC);
	kGameEvent.SetEventParam(EP_CLICK_NPCFUNC_TASKSTATE, nTaskState);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}
//------------------------------------------------------------------------------
bool SC_ClientSkillInterface::IsCanLevelUp(int64_t lEXP)
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkMC = pkLogicManger->GetOperationPlayer();

	if (!pkMC)
		return false;

	CF_LevelUp* pkCSVLevelUp = CF_LevelUp::GetSingletonPtr();
	const CF_LevelUp::DataEntry* pkData = pkCSVLevelUp->GetEntryPtr(pkMC->GetLevel() + 1);
	if (!pkData)
		return false;

	if (pkMC->GetLevel() >= pkCSVLevelUp->GetEntryCount())
		return false;

	return lEXP >= pkData->_lEXPNeed;
}
//------------------------------------------------------------------------------
bool SC_ClientSkillInterface::IsCanEnChantItem(int lItemID)
{
	CF_ItemGen::DataEntry * pkItemGen = SafeGetCSVFileDataEntryPtr<CF_ItemGen>(lItemID);
	return pkItemGen == NULL ? false : !pkItemGen->EnchantEffect.empty();
}
//------------------------------------------------------------------------------
void SC_ClientSkillInterface::InsertNPCFuncEvent()
{
	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_TALK_TO_NPC);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}
//------------------------------------------------------------------------------
UT_Vec4Int SC_ClientSkillInterface::GetLoadingPercent()
{
#ifdef	USE_DOWNLOADER
	//< 地图名
	std::string mapName;
	IDownloader * p = UT_FileManagerBase::GetSingletonPtr()->GetDownloader();
	if(p && p->getUserData(ClientGamePlayerLoadingMapKey, mapName) && !mapName.empty())
	{
		//< metrics
		JZCurlTaskMetrics m;
		p->getReqMetrics(mapName.c_str(), m);
		float process = m.getSizePercent();	// getCountPercent
		int iProcess = process * 100;

		//< 地图下载阶段，有主角、npc模型下载，DownloadMetrics *m未能统计到，只能统计地图的
		//< 因此，显示不了下载速度；只能使用全局下载速度
		DownloadStatistics s;
		p->getWholeStatistics(s);

		//< BUG #18587
		//< a.进度条默认前50%为角色模型加载时间，模型加载完毕后直接跳转到50%开始计算场景加载百分比
		//< b.角色加载进度以恒定速度显示增加，设定时间暂定20s（20s后若未加载完角色进度条卡滞在50%）
		//< Hard Code: 正在第一次下载地图的时候，做进度条模拟，不一定是第一张地图
		static bool  s_1stMapLoading = true;
		if (s_1stMapLoading)
		{
			static float s_1stMapLoading_FakePercent = 0;
			static DWORD s_1stMapLoading_FakePercent_StartTime = ::GetTickCount();
			if (iProcess == 0)	//< 地图还未开始的时候，这时候在下载主角，模拟进度20s，50%
			{
				DWORD elapsed = ::GetTickCount() - s_1stMapLoading_FakePercent_StartTime;
				s_1stMapLoading_FakePercent = elapsed * 0.001f * (0.5f / 20.0f);
				if (s_1stMapLoading_FakePercent > 0.5f)
					s_1stMapLoading_FakePercent = 0.5f;
				iProcess = s_1stMapLoading_FakePercent * 100.0f;
			}
			else
			{
				float v = s_1stMapLoading_FakePercent + (1.0f - s_1stMapLoading_FakePercent) * process;
				iProcess = v * 100.0f;
			}
		}
		if (iProcess >= 99)
			s_1stMapLoading = false;

		return UT_Vec4Int(0, iProcess, (int)m.d_Size, ((int)s.d_Speed)>>10);
		//< 进度到达100%就会隐藏loading界面
		//< 这里的写法，影响到了原先的逻辑，原先是根据UI_UIManager::GetSingleton().GetLoadingPercent()判断的（见下面）
		//< 现在只根据下载进度判断，相对于开发版，更早的从SM_GameStage_LOADING切到了SM_GameStage_GAME，
		//< 由于下载完后资源未能及时加载，可能对一些逻辑产生影响，比如寻路
	}
#endif

	return UT_Vec4Int(1, UI_UIManager::GetSingleton().GetLoadingPercent(), 0, 0);	//< 见SM_GameStage_LOADING::StateProcess
}

StringType SC_ClientSkillInterface::GetDownloadInfo()
{
  #ifdef USE_DOWNLOADER
	IDownloader * p = UT_FileManagerBase::GetSingletonPtr()->GetDownloader();
	if(p)
	{
		StringStreamType sStream("");
		CategoryDownloadStatistics  c;
		CategoryDownloadStatistics::iterator iter;
		p->getCategoryStatistics(c);
		for(iter =c.begin();iter!=c.end();++iter)
		{
		   sStream<<iter->first.c_str()<<"#"<<iter->second.d_Size<<"#"<<iter->second.d_AvgSpeed<<"&";	 
		}
		return sStream.str();
	}	
  #endif
	return "";
}

void SC_ClientSkillInterface::ResetDownloadInfo()
{
#ifdef USE_DOWNLOADER
	IDownloader * p = UT_FileManagerBase::GetSingletonPtr()->GetDownloader();
	if(p)
	{
		p->clearCategoryStatistics();
	}     
#endif
}

float SC_ClientSkillInterface::GetDownloadPercent(const StringType &sKey)
{
#ifdef	USE_DOWNLOADER
	IDownloader *p = UT_FileManagerBase::GetSingletonPtr()->GetDownloader();
	if(p)
	{
		JZCurlTaskMetrics v;

		StringType uiKey = sKey + "_UI";
		p->getReqMetrics(uiKey.c_str(), v);
		return v.getSizePercent();
	}
#endif
	return 0.0f;
}

int	SC_ClientSkillInterface::GetYuanbaoOriginalPrice(int iShopID)
{
	CF_ShopList* pkShopList = CF_ShopList::GetSingletonPtr();
	{
		CF_ShopList::DataEntry* pkEntry = pkShopList->GetEntryPtr(iShopID);
		if (pkEntry)
			return LC_Helper::GetComMapValue(pkEntry->_OriginalPriceList, CT_UNBIND_YUANBAO);
	}
	return 0;
}

int	SC_ClientSkillInterface::GetYuanbaoCurPrice( int iShopID )
{
	CF_ShopList* pkShopList = CF_ShopList::GetSingletonPtr();
	{
		CF_ShopList::DataEntry* pkEntry = pkShopList->GetEntryPtr(iShopID);
		if (pkEntry)
			return LC_Helper::GetComMapValue(pkEntry->_ActualPriceList, CT_UNBIND_YUANBAO);
	}
	return 0;
}

StringType	SC_ClientSkillInterface::GetAccessWayBuyPrice(int iShopID)
{
	CF_ShopList* pkShopList = CF_ShopList::GetSingletonPtr();
	CF_ShopList::DataEntry* pkEntry = pkShopList->GetEntryPtr(iShopID);
	if (pkEntry && pkEntry->_StagePriceList.size() > 0)
	{
		LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
		if (NULL == pkLogicManager)
			return "";
		LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
		if (NULL == pkGamePlayer)
			return "";

		int icount = pkGamePlayer->GetCommonShopAsset().GetAvailableCount(iShopID);
		StringStreamType sStream;
		sStream << icount << "#";
		
		//剩余次数为0则显示最后一个的消耗
		UT_SIMDataList::reverse_iterator it = pkEntry->_StagePriceList.rbegin() + UT_MathBase::MaxInt(icount, 1) - 1;
		if (it != pkEntry->_StagePriceList.rend())
		{
			sStream << it->ID() << "#" << it->ParamA() << "#";
		}

		Utility::LC_ComMap::const_iterator iter = pkEntry->_sCountLimits.begin();
		if(iter!=pkEntry->_sCountLimits.end())
		{
			sStream << iter->first << "#" << iter->second;
		}

		return sStream.str();
	}
	return "";
}

int	SC_ClientSkillInterface::GetItemidInShopList( int iShopID )
{
	CF_ShopList* pkShopList = CF_ShopList::GetSingletonPtr();
	{
		CF_ShopList::DataEntry* pkEntry = pkShopList->GetEntryPtr(iShopID);
		if (pkEntry)
			return pkEntry->_iItemID;
	}
	return 0;
}

StringType	SC_ClientSkillInterface::GetItemInShopList( int iShopID )
{
	StringStreamType stream;
	CF_ShopList* pkShopList = CF_ShopList::GetSingletonPtr();
	{
		CF_ShopList::DataEntry* pkEntry = pkShopList->GetEntryPtr(iShopID);
		if (pkEntry)
			stream << pkEntry->_iItemID << "#" << pkEntry->_iCount << "#";
	}
	return stream.str();
}

void SC_ClientSkillInterface::_selectWatchedTarget( object_id_type* alTargetList, int iTargetListSize, 
												   int& iSelectTargetIndex, int& lUpdateTargetListTime, const object_id_vector& rkSurroundTargetVector )
{
	int lCurrentTime = (int)GET_PROCESS_TIME;
	//如果上次更新目标列表时间已经过去5秒钟，则更新目标列表
	if ( lCurrentTime - lUpdateTargetListTime > 5 )
	{
		//记录更新时间
		lUpdateTargetListTime = lCurrentTime;

		//重置
		for (int i = 0; i < iTargetListSize; i ++)
		{
			alTargetList[i] = 0;
		}
		int iUpdateTargetCount = iTargetListSize < rkSurroundTargetVector.size() 
			? iTargetListSize : rkSurroundTargetVector.size();
		//更新目标列表
		for (int i = 0; i < iUpdateTargetCount; i ++)
		{
			alTargetList[i] = rkSurroundTargetVector[i];
		}
		//选中第一个目标
		iSelectTargetIndex = 0;
	}
	else
	{
		//切换到下一个目标
		if (iSelectTargetIndex + 1 >= iTargetListSize 
			|| alTargetList[iSelectTargetIndex + 1] == 0)
		{
			iSelectTargetIndex = 0;		//重新选中第一个目标
		}
		else
		{
			iSelectTargetIndex ++;
		}
	}

	return;
}

object_id_type SC_ClientSkillInterface::GetLogicIDByCharType(int lTypeID)
{
	if (IS_CHAR_NPC_TYPE(lTypeID))
	{
		LC_NPCManagerBase* pkManager = ENGINE_GET_NPC_MANAGER();
		const LC_NPCIDPtrMap& kMap = pkManager->GetNPCIDMap();
		LC_NPCIDPtrMapConstIter iterBegin = kMap.begin();
		LC_NPCIDPtrMapConstIter iterEnd = kMap.end();

		for ( ;iterBegin != iterEnd; ++iterBegin)
		{
			LC_NPCBase* pkBase = iterBegin->second;
			if (pkBase)
			{
				if (pkBase->GetCharType() == lTypeID)
				{
					return pkBase->GetID();
				}
			}
		}
	}
	
	if (IS_CHEST_TYPE(lTypeID))
	{
		LC_PlaceHolderManagerBase* pkChestManager = ENGINE_GET_CHEST_MANAGER();
		const LC_PlaceHolderIDPtrMap& kChestMap = pkChestManager->GetIDPtrMap();
		LC_PlaceHolderIDPtrMapConstIter chestiterBegin = kChestMap.begin();
		LC_PlaceHolderIDPtrMapConstIter chestiterEnd = kChestMap.end();

		for ( ;chestiterBegin != chestiterEnd; ++chestiterBegin )
		{
			LC_PlaceHolderBase* pkPlaceHolder = chestiterBegin->second;
			if (pkPlaceHolder)
			{
				return pkPlaceHolder->GetID();
			}
		}
	}
	
	return -1;
}

object_id_type SC_ClientSkillInterface::GetChestIDByCharType(int lTypeID)
{
	LC_PlaceHolderManagerBase* pkManager = ENGINE_GET_CHEST_MANAGER();
	const LC_PlaceHolderIDPtrMap& kMap = pkManager->GetIDPtrMap();
	LC_PlaceHolderIDPtrMapConstIter iterBegin = kMap.begin();
	LC_PlaceHolderIDPtrMapConstIter iterEnd = kMap.end();

	for ( ;iterBegin != iterEnd; ++iterBegin)
	{
		LC_PlaceHolderBase* pkBase = iterBegin->second;
		if (pkBase)
		{
			if (pkBase->GetCharType() == lTypeID)
			{
				return pkBase->GetID();
			}
		}
	}
	return -1;
}
//------------------------------------------------------------------------------
void SC_ClientSkillInterface::SwitchShieldVisibility(int iIndex, bool visiable)
{
	IM_Manager* pkImManager = IM_Manager::GetSingletonPtr();
	if (pkImManager != NULL)
	{
		if ( iIndex == IM_Manager::SHIELD_WEATHER )
		{
			g_settings.CFG_ENABLE_WEATHER_EFFECT = visiable;
			RG_RenderManager::GetSingleton().ShieldWeather(visiable);
		}
		else
		{
			pkImManager->SwitchShieldVisibility(iIndex);
			pkImManager->SetShieldVisibility(iIndex, visiable);
		}
	}
}
//--------------------------------------------------------------------------------
int SC_ClientSkillInterface::GetTokenCompositeItemSlot(int lItemID)
{	
	CF_ItemUIAssembleUseFunc* pkItemUIAssembleUseFunc = CF_ItemUIAssembleUseFunc::GetSingletonPtr();
	if (NULL == pkItemUIAssembleUseFunc)
	{
		return -1;
	}

	int lItemSlot = -1;
	CF_ItemUIAssembleUseFunc::DataEntryMapExternIterator kItemUIAssembleIter = pkItemUIAssembleUseFunc->GetIterator();
	while(kItemUIAssembleIter.HasMoreElements())
	{
		CF_ItemUIAssembleUseFunc::DataEntry* pkData = kItemUIAssembleIter.PeekNextValuePtr();
		if(pkData)
		{
			// 在第1物品列找到，则放入slot 0
			if (lItemID == pkData->_lSuiteID1)
			{
				lItemSlot = 0;
				break;
			}
			// 在第2物品列找到，则放入slot 1
			if (lItemID == pkData->_lSuiteID2)
			{
				lItemSlot = 1;
				break;
			}
		}	
		kItemUIAssembleIter.MoveNext();
	}
	// 没找到，说明物品放错
	if (-1 == lItemSlot)
	{
		SendLogicInfoMessage(RE_TOKEN_COMPOSITE_ITEM_ERROR);
	}

	return lItemSlot;
}

int SC_ClientSkillInterface::GetTokenCompositedItemID(int lItemIDSlot0, int lItemIDSlot1)
{
	CF_ItemUIAssembleUseFunc* pkItemUIAssembleUseFunc = CF_ItemUIAssembleUseFunc::GetSingletonPtr();
	if (NULL == pkItemUIAssembleUseFunc)
	{
		return -1;
	}

	CF_ItemUIAssembleUseFunc::DataEntryMapExternIterator kItemUIAssembleIter = pkItemUIAssembleUseFunc->GetIterator();
	while (kItemUIAssembleIter.HasMoreElements())
	{
		CF_ItemUIAssembleUseFunc::DataEntry* pkData = kItemUIAssembleIter.PeekNextValuePtr();
		if (pkData)
		{			
			// slot 0 和 slot 1的物品与表中某一行的对应位置分别匹配，则返回合成的物品ID
			if (lItemIDSlot0 == pkData->_lSuiteID1 && lItemIDSlot1 == pkData->_lSuiteID2)
			{
				return pkData->_lAssembleItemID;
			}
		}
		kItemUIAssembleIter.MoveNext();
	}
	
	return -1;
}

void SC_ClientSkillInterface::RequestInterruptTokenComposite()
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)LC_ClientLogicManager::GetSingletonPtr();
	LC_ClientGamePlayer* pkPlayer = pkLogicManger->GetOperationPlayer();

	if (!pkPlayer)
		return;

	if (pkPlayer->GetDead())
	{
		SendLogicInfoMessage(RE_PRODUCTION_YOU_ARE_DEAD);
		return;
	}
	pkPlayer->InterruptSkill(GET_PROCESS_TIME, SKILL_TOKEN_COMPOSITE, SKI_NONE);
}
//--------------------------------------------------------------------------------
bool SC_ClientSkillInterface::IsIMEOpen()
{
	BOOL bFlag = ImmIsIME( GetKeyboardLayout(0) );
	if (bFlag == TRUE)
		return true;
	else
		return false;
}
//--------------------------------------------------------------------------------
void SC_ClientSkillInterface::ConfirmSendMail(bool bPayFlag, const StringType& kName, const StringType& kCash)
{
	if (bPayFlag)
	{
		SendMessageDialog(RE_DIALOG_CONFIRM_SEND_PAY_MAIL,
			kName, "", RE_DIALOG_CONFIRM_SEND_PAY_MAIL, 0, 0, 0);
	}
	else
	{
		SendMessageDialog(RE_DIALOG_CONFIRM_SEND_MAIL,
			kCash, kName, RE_DIALOG_CONFIRM_SEND_MAIL, 0, 0, 0);
	}	
}
//--------------------------------------------------------------------------------
void SC_ClientSkillInterface::ConfirmExitGuild()
{
	SendMessageDialog(RE_DIALOG_CONFIRM_EXIT_GUILD, "", "", RE_DIALOG_CONFIRM_EXIT_GUILD, 0, 0, 0);
}
void SC_ClientSkillInterface::ConfirmDismissGuild()
{
	SendMessageDialog(RE_DIALOG_CONFIRM_DISMISS_GUILD, "", "", RE_DIALOG_CONFIRM_DISMISS_GUILD, 0, 0, 0);
}
//--------------------------------------------------------------------------------
combat_score_type SC_ClientSkillInterface::GetSkillStateCombatScoreBySkillID(int SkillID)
{
	CF_SkillStateList* pkCSVStateList = CF_SkillStateList::GetSingletonPtr();
	CF_SkillStateList::DataEntry* pkData = pkCSVStateList->GetEntryPtr(SkillID);
	if(!pkData)
	{
		return 0;
	}
	return (combat_score_type)pkData->_lCombatScore;
}

StringType SC_ClientSkillInterface::GetSkillStateUseResult(int iID)
{
	CF_SkillStateList::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_SkillStateList>(iID);
	if(pkEntry != NULL)
		return pkEntry->_kUseResult;

	return "";
}

StringType SC_ClientSkillInterface::GetSevenDaysRewardInfo(int iID)
{
	StringStreamType sStream("");
	CF_SevenDaysReward::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_SevenDaysReward>(iID);
	if(pkEntry != NULL)
		sStream << pkEntry->_sTitle << "#" << pkEntry->_sImage << "#" << pkEntry->_iCharType << "#" << pkEntry->_sIcon << "#" << pkEntry->_Position;

	return sStream.str();
}

void SC_ClientSkillInterface::UseItemInPackByIndex(int iPackType, int iEntryIndex, int iParam, int iCid, int iCount)
{
	LC_ClientGamePlayer* pkPlayer = GetPlayerPtr();
	if (pkPlayer)
	{
		pkPlayer->RequestUseItemInPackByIndex(iPackType, iEntryIndex, iCount, iParam,"", iCid);
	}
}

int SC_ClientSkillInterface::GetItemUseUIOpenFuncID(int iItemID)
{
	CF_ItemList::DataEntry *pkItem = SafeGetCSVFileDataEntryPtr<CF_ItemList>(iItemID);
	if(NULL == pkItem)
		return 0;

	return pkItem->_lItemUseOpenUIFunc;
}

bool SC_ClientSkillInterface::GetItembShowUseIconByItemID(int iItemID)
{
	CF_ItemList::DataEntry *pkItem = SafeGetCSVFileDataEntryPtr<CF_ItemList>(iItemID);
	if(NULL == pkItem)
		return 0;

	return pkItem->_bShowUseIcon;
}

StringType	SC_ClientSkillInterface::GetItemPropertyFuncSkills(int iItemID)
{
	CF_ItemList::DataEntry *pkItem = SafeGetCSVFileDataEntryPtr<CF_ItemList>(iItemID);
	if(NULL == pkItem)
		return "";

	CF_ItemPropertyFunc::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_ItemPropertyFunc>(pkItem->_lPropertyFuncID);
	if(NULL == pkEntry)
		return "";

	return pkEntry->_kSkills;
}

void SC_ClientSkillInterface::EquipItemInPackage(int iEntryIndex, int cID)
{
	LC_ClientGamePlayer* pkPlayer = GetPlayerPtr();
	LC_PackAsset *pkPackAsset = pkPlayer->GetPackAsset();
	LC_BackPackEntry* pkEntry = pkPackAsset->GetBackPackEntry(PACK_TYPE_BASIC, iEntryIndex);
	if(!pkEntry->GetValid())
	{
		return;
	}

	int lItemTypeID = pkEntry->GetTypeID();
	LC_ItemDescriptionEntry* pkItem = LC_ItemFactoryBase::GetSingletonPtr()->GetItemDescriptionEntry(lItemTypeID);
	T_ASSERT_CRITICAL_S(pkItem != NULL);

	int iTarPackType = pkItem->GetEquipPackLimit();
	int iCat = pkItem->GetCategory();
	

	ResultType iUseResult = pkPlayer->CheckEquipItemInPackage(PACK_TYPE_BASIC, iEntryIndex, cID);
	if(iUseResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(iUseResult);
		return;
	}

	pkPlayer->RequestEquipItem(iEntryIndex, cID, iCat);
}
//--------------------------------------------------------------------------------
void SC_ClientSkillInterface::PickItemInChestByIndex(object_id_type lPlaceHolderID, int nIndex)
{
	LC_ClientGamePlayer* pkPlayer = GetPlayerPtr();
	if (pkPlayer)
	{
		LC_PlayerFlopManager* pkFlopManager = pkPlayer->GetFlopManager();
		if (pkFlopManager)
		{
			pkFlopManager->RequestPickItem(lPlaceHolderID);
		}
	}
}
//--------------------------------------------------------------------------------
void SC_ClientSkillInterface::RollButtonDown(int nButtonType, int nRollWndIndex)
{
}
//--------------------------------------------------------------------------------
void SC_ClientSkillInterface::OPItemInTempPack( int iOPType, int nIndex, int iDstPackType, int iDstIndex)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager)
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (NULL == pkGamePlayer)
		return;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_OP_TEMP_PACK reqMsg;
	reqMsg.m_lPlayerID  = pkGamePlayer->GetID();
	reqMsg.m_iOPType	= iOPType;
	reqMsg.m_lSrcIndex  = nIndex;
	reqMsg.m_lDstPackType  = 0;
	reqMsg.m_lDstIndex  = 0;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_OP_TEMP_PACK, &reqMsg);

	if(bCreateMsg)
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
//--------------------------------------------------------------------------------
ResultType SC_ClientSkillInterface::CheckOperationValid(GameLogic::LC_ActorOperationEntry* pkOperation)
{
	LC_ClientGamePlayer* pkPlayer = GetPlayerPtr();
	if(NULL == pkPlayer)
		return RE_FAIL;

	LC_ActorOperationManagerBase* pkActorOperationManager = (LC_ClientActorOperationManager*)LC_ClientActorOperationManager::GetSingletonPtr();
	if(NULL == pkActorOperationManager)
		return RE_FAIL;

	ResultType iResult = pkActorOperationManager->CheckOperationValid(pkOperation,pkPlayer);

	//SendLogicInfoMessage(iResult);
	return iResult;
}
//--------------------------------------------------------------------------------
ResultType SC_ClientSkillInterface::CheckOperationExecute(GameLogic::LC_ActorOperationEntry* pkOperation)
{
	LC_ClientGamePlayer* pkPlayer = GetPlayerPtr();
	if(NULL == pkPlayer)
		return RE_FAIL;

	LC_ActorOperationManagerBase* pkActorOperationManager = LC_ClientActorOperationManager::GetSingletonPtr();
	if(NULL == pkActorOperationManager)
		return RE_FAIL;

	ResultType iResult = pkActorOperationManager->CheckOperationExecute(pkOperation,pkPlayer);

	//SendLogicInfoMessage(iResult);
	return iResult;
}
//--------------------------------------------------------------------------------
ResultType SC_ClientSkillInterface::ExecuteOperation(GameLogic::LC_Attacker* pkScriptCaller,GameLogic::LC_ActorOperationEntry* pkOperation)
{
	LC_ClientGamePlayer* pkPlayer = GetPlayerPtr();
	if (NULL == pkScriptCaller 
		|| NULL == pkPlayer 
		|| pkScriptCaller->GetID() != pkPlayer->GetID())
	{
		return RE_FAIL;
	}

	ResultType iExecuteResult = SC_SkillInterface::ExecuteOperation(pkScriptCaller,pkOperation);
	ResultType iMessageResult = iExecuteResult;
	
	//if(iMessageResult == RE_SUCCESS)
	//{
		//switch (pkOperation->GetOperationType())
		//{
		//case LAOT_TEXT_SAND_COMPOSITION:
		//	iMessageResult = RE_TEXT_SAND_COMPOSITION_SUCCESS;
		//case LAOT_ADD_TEXT_TO_EQUIP:
		//	iMessageResult = RE_TEXT_ADD_TEXT_TO_EQUIP_SUCCESS;
		//case LAOT_REMOVE_TEXT_FROM_EQUIP:
		//	iMessageResult = RE_TEXT_REMOVE_TEXT_FROM_EQUIP_SUCCESS;
		//case LAOT_SAND_TEXT_IN_EQUIP:
		//	iMessageResult = RE_TEXT_SAND_TEXT_IN_EQUIP_SUCCESS;
		//case LAOT_APPEND_ATTRIBUTE_ON_EQUIP:
		//	iMessageResult = RE_ITEM_APPEND_ATTRIBUTE_SUCCESS;
		//default:
		//	iMessageResult = RE_SUCCESS;
		//}
	//}

	switch (iMessageResult)
	{
	//case RE_AMULET_RECAST_FAIL_NOT_ENOUGH_ITEM:
	//case RE_AMULET_LOCK_FAIL_NOT_ENOUGH_ITEM:
	//case RE_AMULET_UNLOCK_FAIL_NOT_ENOUGH_ITEM:
	//	{
	//		int lItemID = pkOperation->GetOperationParameter(LC_AmuletOperation::LOPT_NEED_COST_ITEM_ID);
	//		if (IS_ITEM_ID(lItemID))
	//		{
	//			CF_ItemList* pkItemList = CF_ItemList::GetSingletonPtr();
	//			CF_ItemList::DataEntry* pkItemData = pkItemList->GetEntryPtr(lItemID);
	//			if (NULL != pkItemData)
	//			{
	//				SendLogicInfoMessage(iMessageResult, pkItemData->_kName.c_str());
	//			}
	//		}
	//	}
	//	break;
	/*case RE_COMPOSITION_MATERIAL_NOT_ENOUGH:
		{
			int lYangItemSlot = pkOperation->GetOperationParameter(LC_ItemCompositionOperation::LOPT_ITEM_COMPOSITION_YANG_BACKPACK_INDEX);

			LC_PackAsset* pkPackAsset = pkPlayer->GetPackAsset();
			if(NULL == pkPackAsset)
				return RE_FAIL;

			if (lYangItemSlot >= 0)
			{
				LC_BackPackEntry* pkYangItemEntry = pkPackAsset->GetBackPackEntry(PACK_TYPE_BASIC, lYangItemSlot);
				if (!pkYangItemEntry)
					return RE_FAIL;

				CF_ItemList* pkItemList = CF_ItemList::GetSingletonPtr();
				CF_ItemList::DataEntry* pkItemData = pkItemList->GetEntryPtr(pkYangItemEntry->GetTypeID());

				if (!pkItemData)
					return RE_FAIL;

				CF_CompositionFormula* pkCSVCompositionList = CF_CompositionFormula::GetSingletonPtr();
				CF_CompositionFormula::DataEntry* pkCompositionData = pkCSVCompositionList->GetEntryPtr(pkItemData->_lCompositionFuncID);

				if (!pkCompositionData)
					return RE_FAIL;

				CF_ItemList::DataEntry* pkMaterialData = pkItemList->GetEntryPtr(pkCompositionData->_lNeedMaterialID);
				StringType strItemName = pkMaterialData->_kName;

				SendLogicInfoMessage(iMessageResult, strItemName.c_str());
			}
		}
		break;*/
	case  RE_PRODUCTION_MATERIAL_NOT_ENOUGH:
		{
			int lFormulaID = pkOperation->GetOperationParameter(LC_Production::LOPT_PRODUCTION_FORMULA_ID);

			CF_ProductionFormula* pkCSVFormula = CF_ProductionFormula::GetSingletonPtr();
			CF_ProductionFormula::DataEntry* pkFormulaData = pkCSVFormula->GetEntryPtr(lFormulaID);

			if (!pkFormulaData)
				return RE_FAIL;

			int lMaterialID[6];
			lMaterialID[0] = pkFormulaData->_lMaterial1ID;
			lMaterialID[1] = pkFormulaData->_lMaterial2ID;
			lMaterialID[2] = pkFormulaData->_lMaterial3ID;
			lMaterialID[3] = pkFormulaData->_lMaterial4ID;
			lMaterialID[4] = pkFormulaData->_lMaterial5ID;
			lMaterialID[5] = pkFormulaData->_lMaterial6ID;

			int lMaterialNum[6];
			lMaterialNum[0] = pkFormulaData->_lMaterial1Num;
			lMaterialNum[1] = pkFormulaData->_lMaterial2Num;
			lMaterialNum[2] = pkFormulaData->_lMaterial3Num;
			lMaterialNum[3] = pkFormulaData->_lMaterial4Num;
			lMaterialNum[4] = pkFormulaData->_lMaterial5Num;
			lMaterialNum[5] = pkFormulaData->_lMaterial6Num;

			for (int i = 0; i < 6; ++i)
			{
				if (IS_ITEM_ID(lMaterialID[i]))
				{
					int nPlayerMaterialNum = pkPlayer->GetCanCostItemCount(lMaterialID[i]);

					if (nPlayerMaterialNum < lMaterialNum[i])
					{
						CF_ItemList* pkCSVItemList = CF_ItemList::GetSingletonPtr();
						CF_ItemList::DataEntry* pkItemaData = pkCSVItemList->GetEntryPtr(lMaterialID[i]);

						if (!pkItemaData)
							return RE_FAIL;

						StringType strMaterialName = pkItemaData->_kName;
						SendLogicInfoMessage(RE_PRODUCTION_MATERIAL_NOT_ENOUGH, strMaterialName.c_str());
					}
				}
			}
		}
		break;
	default:
		SendLogicInfoMessage(iMessageResult);
		break;
	}

	return iExecuteResult;
}
//--------------------------------------------------------------------------------
ResultType SC_ClientSkillInterface::SetOperationParameter(GameLogic::LC_Attacker* pkScriptCaller,GameLogic::LC_ActorOperationEntry* pkOperation,int iParameterIndex,int lVal)
{
	LC_ClientGamePlayer* pkPlayer = GetPlayerPtr();
	if (NULL == pkScriptCaller 
		|| NULL == pkPlayer 
		|| pkScriptCaller->GetID() != pkPlayer->GetID())
	{
		return RE_FAIL;
	}

	ResultType iResult = SC_SkillInterface::SetOperationParameter(pkScriptCaller,pkOperation,iParameterIndex,lVal);
	
	if(iResult != RE_SUCCESS)
	{
		switch (iResult)
		{
		case RE_COMPOSITION_TYPE_ERROR:
			{
				/*int lYangItemSlot = pkOperation->GetOperationParameter(LC_ItemCompositionOperation::LOPT_ITEM_COMPOSITION_YANG_BACKPACK_INDEX);
				int lYinItemSlot = pkOperation->GetOperationParameter(LC_ItemCompositionOperation::LOPT_ITEM_COMPOSITION_YIN_BACKPACK_INDEX);
				
				LC_PackAsset* pkPackAsset = pkPlayer->GetPackAsset();
				if(NULL == pkPackAsset)
					return RE_FAIL;

				if (lYangItemSlot < 0)
				{
					LC_BackPackEntry* pkYinItemEntry = pkPackAsset->GetBackPackEntry(PACK_TYPE_BASIC, lYinItemSlot);
					if (!pkYinItemEntry)
						return RE_FAIL;

					CF_ItemList* pkItemList = CF_ItemList::GetSingletonPtr();
					CF_ItemList::DataEntry* pkItemData = pkItemList->GetEntryPtr(pkYinItemEntry->GetTypeID());

					if (!pkItemData)
						return RE_FAIL;

					CF_CompositionFormula* pkCSVCompositionList = CF_CompositionFormula::GetSingletonPtr();
					CF_CompositionFormula::DataEntry* pkCompositionData = pkCSVCompositionList->GetEntryPtr(pkItemData->_lCompositionFuncID);

					if (!pkCompositionData)
						return RE_FAIL;
				
					CF_ItemList::DataEntry* pkYangItemSlotData = pkItemList->GetEntryPtr(pkCompositionData->_lYangItemID);
					StringType strItemName = pkYangItemSlotData->_kName;

					SendLogicInfoMessage(iResult, strItemName.c_str());
					break;
				}
				
				if (lYinItemSlot < 0)
				{
					LC_BackPackEntry* pkYangItemEntry = pkPackAsset->GetBackPackEntry(PACK_TYPE_BASIC, lYangItemSlot);
					if (!pkYangItemEntry)
						return RE_FAIL;

					CF_ItemList* pkItemList = CF_ItemList::GetSingletonPtr();
					CF_ItemList::DataEntry* pkItemData = pkItemList->GetEntryPtr(pkYangItemEntry->GetTypeID());

					if (!pkItemData)
						return RE_FAIL;

					CF_CompositionFormula* pkCSVCompositionList = CF_CompositionFormula::GetSingletonPtr();
					CF_CompositionFormula::DataEntry* pkCompositionData = pkCSVCompositionList->GetEntryPtr(pkItemData->_lCompositionFuncID);

					if (!pkCompositionData)
						return RE_FAIL;

					CF_ItemList::DataEntry* pkYangItemSlotData = pkItemList->GetEntryPtr(pkCompositionData->_lYangItemID);
					StringType strItemName = pkYangItemSlotData->_kName;

					SendLogicInfoMessage(iResult, strItemName.c_str());
					break;
				}*/
			}
			break;
		default:
			SendLogicInfoMessage(iResult);
			break;
		}
	}

	return iResult;
}
//--------------------------------------------------------------------------------
void SC_ClientSkillInterface::ReqSetOperationParameter( int lOperationType, int iParameterIndex,int lVal, int iCid )
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager)
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (NULL == pkGamePlayer)
		return;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_SetOperationParameter reqMsg;
	reqMsg.m_lOperationType  = lOperationType;
	reqMsg.m_lParameterIndex = iParameterIndex;
	reqMsg.m_lParameterVal   = lVal;
	reqMsg.m_nBuddyID = iCid;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SET_ACTOR_OPERATION_PARAMETER, &reqMsg);

	if(bCreateMsg)
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
//--------------------------------------------------------------------------------
void SC_ClientSkillInterface::ReqResetOperationParameter( int lOperationType, bool bClearParameters )
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager)
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (NULL == pkGamePlayer)
		return;


	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_ResetOperation reqMsg;
	reqMsg.m_lOperationType = lOperationType;
	reqMsg.m_bClearParameters = bClearParameters;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_RESET_ACTOR_OPERATION, &reqMsg);

	if(bCreateMsg)
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
//--------------------------------------------------------------------------------
void SC_ClientSkillInterface::SetUIMouseLogicType(GameLogic::LC_Attacker* pkScriptCaller, int iMouseLogicType)
{
	LC_ClientGamePlayer* pkPlayer = GetPlayerPtr();
	if (NULL == pkScriptCaller 
		|| NULL == pkPlayer 
		|| pkScriptCaller->GetID() != pkPlayer->GetID())
	{
		return;
	}

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	if (NULL != pkUIManager && iMouseLogicType > MLMT_ATTACK && iMouseLogicType < MLMT_MAX_COUNT)
	{
		pkUIManager->SetUIMouseLogicType(iMouseLogicType);
	}
}
//--------------------------------------------------------------------------------
void SC_ClientSkillInterface::SendInfoMessage(int lSkillID, int lMsgID)
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

	CF_ItemList* pkItemList = CF_ItemList::GetSingletonPtr();
	if (NULL == pkItemList)
	{
		return;
	}

	switch (lMsgID)
	{
	case RE_SKILL_NOT_ENOUGH_ITEM:
		{
			CF_SkillExecuteList::DataEntry* pkSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(lSkillID);
			if (NULL == pkSkillData)
			{
				return;
			}
			LC_PackAsset* pkPackAsset = pkGamePlayer->GetPackAsset();
			if (NULL == pkPackAsset)
			{
				return;
			}

			for(int i = 0; i < MAX_SKILL_COST_ITEM_COUNT; i++)
			{
				UT_Vec3Int kItemCost = CSVHelper::GetSkillItemCost(pkSkillData, i);
				int lItemTypeID = kItemCost.x;

				if(IS_ITEM_ID(lItemTypeID))
				{
					int iPackType = GetWhichPackAutoPut(lItemTypeID);
					if (pkPackAsset->GetItemCountInPack(iPackType, lItemTypeID) < kItemCost.y)
					{
						CF_ItemList::DataEntry* pkItemData = pkItemList->GetEntryPtr(lItemTypeID);
						if (NULL == pkItemData)
						{
							return;
						}
	
						SendLogicInfoMessage(RE_SKILL_NOT_ENOUGH_ITEM, LC_Helper::GetGradeName(pkItemData->_kName, pkItemData->_lGrade));

						LC_GameEvent kGameEvent;
						kGameEvent.SetEventType(GLET_TOYPET_CATCHFAIL);
						kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, pkItemData->_iID);

						UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
						pkUIManager->HandlerGameEvent(kGameEvent);
					}
				}
			}
		}
		break;
	case RE_ITEM_CANT_USE_INVALID_TARGET:
		SendLogicInfoMessage(lMsgID);
		break;
	default:
		SendLogicInfoMessage(lMsgID);
		break;
	}
}
//--------------------------------------------------------------------------------
void SC_ClientSkillInterface::ShowUIError(const StringType& kString)
{
	SendLogicInfoMessage(RE_TIP_MESSAGE,kString.c_str());
}

//--------------------------------------------------------------------------------
void SC_ClientSkillInterface::ShowMouseTips(const StringType& kString)
{
	SendLogicInfoMessage(RE_MOUSE_TIP,kString.c_str());
}

void SC_ClientSkillInterface::ShowNotifyMessage(const StringType& kString, int iLevel)
{
	SendNotifyMessage(PROMPT_CHANNEL_ACTIVITY_TIP, kString, iLevel);
}

void SC_ClientSkillInterface::ShowNotifyMessageWithType(long iMsgType,const StringType& kString, int iLevel)
{
	SendNotifyMessage(iMsgType, kString, iLevel);
}

void SC_ClientSkillInterface::ShowSystemTip(const StringType& kString)
{
	SendNotifyMessage(PROMPT_CHANNEL_SYSTEM_TIP, kString);
}

void SC_ClientSkillInterface::ShowUIErrorByCode(int iErrorCode)
{
	SendLogicInfoMessage(iErrorCode);
}

void SC_ClientSkillInterface::ShowNoticeMessage(int iID)
{
	CF_GameNotice::DataEntry *entry = SafeGetCSVFileDataEntryPtr<CF_GameNotice>(iID);
	if(entry)
	{
		SendNotifyMessage(entry->_Type, entry->_Text, entry->_Level);
	}
}

void SC_ClientSkillInterface::ShowAttriActive(int kAttribute, int val)
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
	pkGamePlayer->SendMessage( CreateMsg_UE_DELTA_ATTRI(pkGamePlayer->GetID(), kAttribute, val, 0) );
}

//--------------------------------------------------------------------------------
StringType SC_ClientSkillInterface::GetActorHatredList(object_id_type lActorID)
{
	LC_WorldManagerBase* pkWorldManager = ENGINE_GET_WORLD_MANAGER();
	LC_ActorBase* pkActor = (LC_ActorBase*)pkWorldManager->FindObject(lActorID);
	if (NULL == pkActor)
	{
		return "";
	}

	LC_ActorHatredList* pkHatredList = pkActor->GetHatredList();
	if (NULL == pkHatredList)
	{
		return "";
	}

	LC_ActorHatredEntry* pkHaterdEntry = NULL;
	StringStreamType kStrStream;
	for (int i = 0; i < pkHatredList->GetHatredCount(); i ++)
	{
		pkHaterdEntry = pkHatredList->GetSortHatredByIndex(i);
		if (NULL != pkHaterdEntry)
		{
			int lHatredID = pkHaterdEntry->GetTargetID();
			if (IS_PLAYER_CLASS_ID(lHatredID) || IS_NPC_ID(lHatredID))
			{
				kStrStream << lHatredID << "\n";
			}
		}
	}

	return kStrStream.str();
}
//--------------------------------------------------------------------------------
StringType SC_ClientSkillInterface::GetActorSkillList(object_id_type lActorID)
{
	return "";
}
//--------------------------------------------------------------------------------

void SC_ClientSkillInterface::Snapshot(bool bWithUI, bool bTiled)
{
    RG_RenderManager::GetSingleton().Snapshot(bWithUI, bTiled);
}

void SC_ClientSkillInterface::NoticeMessage( int lMSG_ID, int lValue )
{
	LC_ClientTaskManager* pkManager = (LC_ClientTaskManager*)LC_ClientTaskManager::GetSingletonPtr();
	if (pkManager == NULL)
		return;

	LC_TaskNoticeMessageManager& rNotice = pkManager->GetNoticeManager();
	rNotice.NoticeMessage(lMSG_ID,lValue);
}

void SC_ClientSkillInterface::AddNoticeTask( int lMSG_ID, int lLeftTime, int lIntervalTime, int lValue, int lDeltValue )
{
	LC_ClientTaskManager* pkManager = (LC_ClientTaskManager*)LC_ClientTaskManager::GetSingletonPtr();
	if (pkManager == NULL)
		return;

	LC_TaskNoticeMessageManager& rNotice = pkManager->GetNoticeManager();
	rNotice.AddNoticeTask(lMSG_ID,lLeftTime,lIntervalTime,lValue,lDeltValue);

}

void SC_ClientSkillInterface::RemoveNoticeTask( int lMSG_ID )
{
	LC_ClientTaskManager* pkManager = (LC_ClientTaskManager*)LC_ClientTaskManager::GetSingletonPtr();
	if (pkManager == NULL)
		return;

	LC_TaskNoticeMessageManager& rNotice = pkManager->GetNoticeManager();
	rNotice.RemoveNoticeTask(lMSG_ID);
}

void SC_ClientSkillInterface::StartForcibleWeather(float fDensity, float fSpeed, float fLength, float fWidth, float fAlpha, float fWind)
{
    SE_SceneManager* pkSceneManager = SE_SceneManager::GetSingletonPtr();
    if (pkSceneManager != NULL)
    {
        pkSceneManager->StartForcibleWeather(fDensity, fSpeed, fLength, fWidth, fAlpha, fWind);
    }
}

void SC_ClientSkillInterface::StopForcibleWeather(void)
{
    SE_SceneManager* pkSceneManager = SE_SceneManager::GetSingletonPtr();
    if (pkSceneManager != NULL)
    {
        pkSceneManager->StopForcibleWeather();
    }
}

void SC_ClientSkillInterface::StartFullScreenEffect(int eFullScreenEffectType)
{
}

void SC_ClientSkillInterface::StopFullScreenEffect(void)
{
}

void SC_ClientSkillInterface::StopCurrentBGM()
{
	g_soundMgr.StopCurrentBGM();
}

//--------------------------------------------------------------------------------
StringType SC_ClientSkillInterface::GetComputerTime()
{
  	StringType CurrentTime = "";

	LC_ClientGamePlayer* pkPlayer = GetPlayerPtr();
	time_t localTime		=	time(0); 
	if (NULL != pkPlayer)
	{
		localTime = (time_t)pkPlayer->GetServerTimeVal();
	};
	tm pkCurLocalTime	    =	*localtime(&localTime); 

	int day,mon,year,hour,minute,second; 
	second					=	pkCurLocalTime.tm_sec;
	minute					=	pkCurLocalTime.tm_min;
	hour					=   pkCurLocalTime.tm_hour;
	day						=	pkCurLocalTime.tm_mday; 
	mon						=	pkCurLocalTime.tm_mon + 1; 
	year					=	pkCurLocalTime.tm_year + 1900; 	
	
	int nWddkday = LC_Helper::GetWeekdayOfInputYearMonthDay(year, mon, day);

	StringType strYear	=	Int2String(year);
	StringType strMon	=	Int2String(mon);
	StringType strDay	=	Int2String(day);
	StringType strHour	=	Int2String(hour);
	StringType strMin	=	Int2String(minute);

	if (strMon.size() == 1)
		strMon = "0" + strMon;

	if (strDay.size() == 1)
		strDay = "0" + strDay;

	if (strHour.size() == 1)
		strHour = "0" + strHour;

	if (strMin.size() == 1)
		strMin = "0" + strMin;

	switch(nWddkday)
	{
	case 0:
		CurrentTime = strYear + "-" + strMon + "-" + strDay + GET_MSG(SC_CLIENTSKILLINTERFACE_CPP_007) + strHour + ":" + strMin;
		break;
	case 1:
		CurrentTime = strYear + "-" + strMon + "-" + strDay + GET_MSG(SC_CLIENTSKILLINTERFACE_CPP_008) + strHour + ":" + strMin;
		break;
	case 2:
		CurrentTime = strYear + "-" + strMon + "-" + strDay + GET_MSG(SC_CLIENTSKILLINTERFACE_CPP_009) + strHour + ":" + strMin;
		break;
	case 3:
		CurrentTime = strYear + "-" + strMon + "-" + strDay + GET_MSG(SC_CLIENTSKILLINTERFACE_CPP_010) + strHour + ":" + strMin;
		break;
	case 4:
		CurrentTime = strYear + "-" + strMon + "-" + strDay + GET_MSG(SC_CLIENTSKILLINTERFACE_CPP_011) + strHour + ":" + strMin;
		break;
	case 5:
		CurrentTime = strYear + "-" + strMon + "-" + strDay + GET_MSG(SC_CLIENTSKILLINTERFACE_CPP_012) + strHour + ":" + strMin;
		break;
	case 6:
		CurrentTime = strYear + "-" + strMon + "-" + strDay + GET_MSG(SC_CLIENTSKILLINTERFACE_CPP_013) + strHour + ":" + strMin;
		break;
	default:
		break;
	}

	return CurrentTime;
}

void SC_ClientSkillInterface::CookingDish()
{
	return;
}
void SC_ClientSkillInterface::RequstInterruptCooking()
{
	return;
}
bool SC_ClientSkillInterface::CanContinueCookDish()
{
	return false;
}
void SC_ClientSkillInterface::PlaySound(const StringType& strKeyWord, int iChannelType, bool bFadeToGain, int nPlayCount, float fAutoStop)
{
	g_soundMgr.Play2DEx(strKeyWord, 
		(AS_ClientSoundManager::SOUND_CHANNEL_TYPE)iChannelType,
		bFadeToGain,
		nPlayCount,
		fAutoStop);
}

int SC_ClientSkillInterface::GetShowBloodBarType( GameLogic::LC_Attacker* pkScriptCaller )
{
	if (pkScriptCaller == NULL)
		return 0;

	GameLogic::LC_ActorBase* pkActor = (LC_ActorBase*)pkScriptCaller;
	int lCharType = pkActor->GetCharType();

	CF_CharType* pkCSVCharType = CF_CharType::GetSingletonPtr();
	CF_CharType::DataEntry* pkData = pkCSVCharType->GetEntryPtr(lCharType);
	if(pkData)
		return pkData->_lShowFlags & CTSF_BLOOD;
	else
		return 0;
}

StringType SC_ClientSkillInterface::GetItemText1CharFuncDescript(int lTextItemID1,int lLevel)
{
	return "NULL";
}

StringType SC_ClientSkillInterface::GetItemText2CharFuncDescript(int lTextItemID1,int lLevel1,int lTextItemID2,int lLevel2)
{
	return "NULL";
}
//--------------------------------------------------------
StringType SC_ClientSkillInterface::GetItemText4CharFuncDescript(int lTextItemID1,int lLevel1,int lTextItemID2,int lLevel2,int lTextItemID3,int lLevel3,int lTextItemID4,int lLevel4)
{
	return "NULL";
}
//--------------------------------------------------------
StringType SC_ClientSkillInterface::GetAttributeFuncDescript(int lAttributeFuncID)
{
	LC_TooltipDirector* pkToolTipDirector = LC_TooltipDirector::GetSingletonPtr();
	if(NULL == pkToolTipDirector)
		return "NULL";

	return pkToolTipDirector->GetAttributeFuncDescript(lAttributeFuncID);
}
int SC_ClientSkillInterface::GetHookItemCostID(int idx)
{
	return 0;
}
int SC_ClientSkillInterface::GetHookItemCostAmount(int idx)
{
	return 0;
}
int SC_ClientSkillInterface::GetHookItemCostTime()
{
	return 0;
}

StringType SC_ClientSkillInterface::GetMapNameByMapResID(int iMapResID)
{
	CF_WorldMapList* pkList	= CF_WorldMapList::GetSingletonPtr();
	CF_WorldMapList::DataEntry* pkData = pkList->GetEntryPtr(iMapResID);
	if (NULL == pkData)
	{
		return "";
	}
	else
	{
		return pkData->_kName;
	}
}

int SC_ClientSkillInterface::GetMapLineCount(int iMapResID)
{
	CF_WorldMapList::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(iMapResID);
	if(NULL == pkEntry)
		return 0;

	return pkEntry->_lineMapCount;
}

StringType SC_ClientSkillInterface::GetMapEnterConditionInfo(int iMapResID)
{

	CF_WorldMapList::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(iMapResID);
	if(pkData == NULL)
		return "";

	StringStreamType sStream;
	sStream << pkData->_needRelation << '#' << pkData->_iNeedLevel << '#' << pkData->iPreTaskID << '#';
	sStream << pkData->_iVIPLevel << '#' << pkData->_needNewVipLevel << '#';

	mem::vector<int>::const_iterator iter = pkData->_needAchievement.begin();
	for( ; iter!=pkData->_needAchievement.end(); ++iter)
	{
		sStream << *iter << ';';
	}
	
	sStream << '#';
	return sStream.str();
}

int SC_ClientSkillInterface::GetMaxItemEnduranceValue( int lItemID )
{
	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	LC_ClientItem*  pkItem = (LC_ClientItem* )pkItemFactory->RequestItem(lItemID);
	if (pkItem == NULL)
		return 0;

	int lEnduranceFuncID = pkItem->GetEnduranceFuncID();
	if (lEnduranceFuncID <= 0)
		return 0;

	CF_ItemEnduranceFunc* pkFunce = CF_ItemEnduranceFunc::GetSingletonPtr();
	if (pkFunce)
	{
		CF_ItemEnduranceFunc::DataEntry* pkEntry = pkFunce->GetEntryPtr(lEnduranceFuncID);
		if (pkEntry)
			return pkEntry->_lMAXEnduranceValue;
	}

	return 0;
}

//------------------------------------
void SC_ClientSkillInterface::ShowUIFrame(bool bShow)
{
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	T_ASSERT(NULL != pkUIManager);

	pkUIManager->SetCurrentScreenVisibility(bShow);
}
//------------------------------------
void SC_ClientSkillInterface::ShowUIContentFunc(int lItemTypeID)
{
	if (!IS_ITEM_ID(lItemTypeID))
		return;

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_SHOW_UI_CONTENT_FUNC);
	kGameEvent.SetEventParam(EP_SHOW_UI_CONTENT_FUNC_ITEM_ID, lItemTypeID);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}
//------------------------------------
int SC_ClientSkillInterface::GetShowUITypeID(int lItemTypeID, int nIndex)
{
	return 0;
}
//------------------------------------
StringType SC_ClientSkillInterface::GetShowUIIconDesc(int lItemTypeID, int nIndex)
{
	return "";
}
//------------------------------------
int SC_ClientSkillInterface::GetDescriptType(int lTypeID)
{
	if (IS_ITEM_ID(lTypeID))
		return 1;

	if (IS_SKILL_TYPE(lTypeID))
		return 2;

	return 0;
}
//------------------------------------
StringType SC_ClientSkillInterface::GetNPCShopNameByFuncID(int lNPCShopFuncID)
{
	CF_NPCShopList* pkNPCShopList = CF_NPCShopList::GetSingletonPtr();
	CF_NPCShopList::DataEntry* pkNPCShopData = pkNPCShopList->GetEntryPtr(lNPCShopFuncID);
	if (NULL != pkNPCShopData)
	{
		return pkNPCShopData->_kName;
	}
	return "";
}

void SC_ClientSkillInterface::ShowRichUI( GameLogic::LC_Attacker* pkScriptCaller, int iTargetType,int iRichUIID, int bShow )
{
	if (iTargetType == 0)
	{
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_SHOW_RICH_UI);
		kGameEvent.SetEventParam(EP_SHOW_RICH_UI_ID,iRichUIID);
		kGameEvent.SetEventParam(EP_SHOW_RICH_UI_SHOW,bShow);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}

}		

LC_ClientMapTransDataEntry* SC_ClientSkillInterface::GetMapTransDataEntryByMapID(int lMapResID)
{
	LC_ClientMapTranscriptionManager* pkMapTransMgr = LC_ClientMapTranscriptionManager::GetSingletonPtr();
	if(pkMapTransMgr)
		return pkMapTransMgr->GetMapTransDataEntry(lMapResID);
	return NULL;
}

void SC_ClientSkillInterface::StopUICutScenes(void)
{
	if (LC_ClientUICutScenesManager::GetSingletonPtr())
		LC_ClientUICutScenesManager::GetSingletonPtr()->Escape();
}

float SC_ClientSkillInterface::GetUICutScenesPlayTime(int ID)
{
	CSVFile::CF_CutScenes* pkCSVCutScenes = CSVFile::CF_CutScenes::GetSingletonPtr();
	T_ASSERT_CRITICAL_S(pkCSVCutScenes);
	const CSVFile::CF_CutScenes::DataEntry* pkData = pkCSVCutScenes->GetEntryPtr(ID);
	if(!pkData)
		return 0.0f;
	return pkData->_fTime;
}

float SC_ClientSkillInterface::GetUICutScenesTimeStep(int ID)
{
	CSVFile::CF_CutScenes* pkCSVCutScenes = CSVFile::CF_CutScenes::GetSingletonPtr();
	T_ASSERT_CRITICAL_S(pkCSVCutScenes);
	const CSVFile::CF_CutScenes::DataEntry* pkData = pkCSVCutScenes->GetEntryPtr(ID);
	if(!pkData)
		return 0.0f;
	return pkData->_fTimeStep;
}
const StringType& SC_ClientSkillInterface::GetUICutScenesIcon(int ID)
{
	CSVFile::CF_CutScenes* pkCSVCutScenes = CSVFile::CF_CutScenes::GetSingletonPtr();
	T_ASSERT_CRITICAL_S(pkCSVCutScenes);
	const CSVFile::CF_CutScenes::DataEntry* pkData = pkCSVCutScenes->GetEntryPtr(ID);
	T_ASSERT_CRITICAL_S(pkData);
	return pkData->_kResPath;
}

StringType SC_ClientSkillInterface::GetSkillOpenLevelInfo(LC_PlayerBase *pkPlayer)
{
	if(NULL == pkPlayer)
		return "";

	CF_SkillExecuteList *pkCSVSkillExecute = CF_SkillExecuteList::GetSingletonPtr();
	CF_SkillExecuteList::DataEntryMapExternIterator iter = pkCSVSkillExecute->GetIterator();

	int iCommunity = pkPlayer->GetWeaponAttribute();

	StringStreamType sStream("");
	for( ; iter.HasMoreElements(); iter.MoveNext())
	{
		CF_SkillExecuteList::DataEntry *pkData = iter.PeekNextValuePtr();
		int lWeaponType = pkData->_lWeaponType;
		//取第一级的技能ID
		if(iCommunity == lWeaponType && pkData->_lCategory1 != SKILL_TYPE_SUB_ACTOR_GREATKUNGFU && \
			pkData->_lCategory2 != SKILL_TYPE_QINGGONG && pkData->_lCategory2 != SKILL_TYPE_BASE && \
			(pkData->_lCategory2 < SKILL_TYPE_MENTAL_BEIGN || pkData->_lCategory2 > SKILL_TYPE_MENTAL_END) &&\
			GET_MSG(SC_CLIENTSKILLINTERFACE_CPP_014) != pkData->_kDescript)
		{
			int lSkillID = pkData->_iID;

			//绝学技能_lCategory1为1001，不加入要技能界面中
			if(SKILL_TYPE_CONTROL == pkData->_lCategory2 && pkData->_lCategory1 == SKILL_TYPE_SUB_ACTOR_GREATKUNGFU)
			{
				continue;
			}

			int iNeedLevel = 0;
			StringType sLimitStr = "";
			StringType sInfo = GetOpenLevelInfoByKeyID(lSkillID, 1);
			TStringVector kVector = UT_BaseStringUtility::SplitString(sInfo, '#');
			if(kVector.size() > 1)
			{
				iNeedLevel = atoi(kVector[0].c_str());
				sLimitStr = kVector[1];
			}
			else if(kVector.size() > 0)
			{
				iNeedLevel = atoi(kVector[0].c_str());
			}

			int iCurLevel = 1;
			sStream << pkData->_lCategory2 << "#" << pkData->_lCategory1 << "#" << lSkillID << "#" << iNeedLevel << "#"  << iCurLevel << "#" << pkData->_AutoShortcutIdx << "#";
			sStream << sLimitStr << "#" << pkData->_kIconPath << "|";
		}
	}

	return sStream.str();
}

StringType SC_ClientSkillInterface::GetTwToZn(const StringType &sName)
{
#ifdef _TAIWAN
	return LC_Helper::GetZnByTw(sName);
#else
	return sName;
#endif
}

int SC_ClientSkillInterface::GetFirstSkillIDByShortcutSlot(int iSlotID)
{
	LC_ClientGamePlayer* pkPlayer = GetPlayerPtr();
	if (NULL != pkPlayer)
	{
		CF_OpenLevel *pkCSVOpenLevel = CF_OpenLevel::GetSingletonPtr();
		CF_OpenLevel::DataEntryMapExternIterator iter = pkCSVOpenLevel->GetIterator();

		int iCurLevel = pkPlayer->GetLevel();
		int iWeaponType = pkPlayer->GetWeaponAttribute();
		while(iter.HasMoreElements())
		{
			CF_OpenLevel::DataEntry *pkData = iter.PeekNextValuePtr();
			if(iCurLevel < pkData->_CharLevel && iWeaponType == SECT_TO_WEAPONTYPE[pkData->_Sect])
			{
				CF_SkillExecuteList::DataEntry* pkSkillEntry = LC_Helper::SafeGetSkillExecuteDataEntryPtr(pkData->_KeyID);
				
				if (pkSkillEntry && pkSkillEntry->_AutoShortcutIdx==iSlotID)
				{
					return pkData->_KeyID;
				}
			}

			iter.MoveNext();
		}
	}

	return 0;
}

StringType SC_ClientSkillInterface::GetOpenLevelInfoByKeyID(int lSkillID, int iType)
{
	CF_OpenLevel *pkCSVOpenLevel = CF_OpenLevel::GetSingletonPtr();
	CF_OpenLevel::DataEntryMapExternIterator iter = pkCSVOpenLevel->GetIterator();

	while(iter.HasMoreElements())
	{
		CF_OpenLevel::DataEntry *pkData = iter.PeekNextValuePtr();
		if(lSkillID == pkData->_KeyID && iType == pkData->_Type)
		{
			StringStreamType sStream("");
			sStream << pkData->_CharLevel << "#" << pkData->_WidgetDesc << "#" << pkData->_kSubLevels << "#" << pkData->_iID;
			return sStream.str();
		}

		iter.MoveNext();
	}
	return "";
}

StringType SC_ClientSkillInterface::_getCimeliaLevelInOpenLevelBySkillID(int lSkillID)
{
	CF_OpenLevel *pkCSVOpenLevel = CF_OpenLevel::GetSingletonPtr();
	CF_OpenLevel::DataEntryMapExternIterator iter = pkCSVOpenLevel->GetIterator();

	while(iter.HasMoreElements())
	{
		CF_OpenLevel::DataEntry *pkData = iter.PeekNextValuePtr();
		if(lSkillID == pkData->_KeyID)
		{
			return pkData->_kSubLevels;
		}

		iter.MoveNext();
	}
	return "";
}

int SC_ClientSkillInterface::_getAutoShortcutKeyInOpenLevelBySkillID(int lSkillID)
{
	CF_OpenLevel *pkCSVOpenLevel = CF_OpenLevel::GetSingletonPtr();
	CF_OpenLevel::DataEntryMapExternIterator iter = pkCSVOpenLevel->GetIterator();

	while(iter.HasMoreElements())
	{
		CF_OpenLevel::DataEntry *pkData = iter.PeekNextValuePtr();
		if(lSkillID == pkData->_KeyID)
		{
			CF_SkillExecuteList::DataEntry* pkSkillEntry = LC_Helper::SafeGetSkillExecuteDataEntryPtr(lSkillID);
			if (pkSkillEntry)
			{
				return pkSkillEntry->_AutoShortcutIdx;
			}
		}

		iter.MoveNext();
	}
	return 0;
}
//=====================================================================
bool SC_ClientSkillInterface::_isLeapYear(int iYear)
{
	return (!(iYear%4) && iYear%100 || !(iYear%400));
}

StringType SC_ClientSkillInterface::_getCalendarString(int iYear, int iMonth, int iDate)
{
	static int dayCount[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	if(_isLeapYear(iYear))
		//闰年2月为29天
		dayCount[2] = 29;

	StringStreamType sStream;
	int iWeekDay = LC_Helper::GetWeekdayOfInputYearMonthDay(iYear, iMonth, 1);
	//iWeekDay = iWeekDay < 7 ? iWeekDay : 0;	//星期天在最前
	int LastMonth = (iMonth + 10) % 12 + 1; //上个月的月数
	int NextMonth = iMonth % 12 + 1;//这个月的月数
	int LastMonthDayCount = dayCount[LastMonth];//上月的天数
	int MonthDayCount = dayCount[iMonth];//本月的天数
	int NextMonthDayCount = dayCount[NextMonth];//下月的天数
	int LastMonthEndIndexInCalendar = iWeekDay > 0 ? iWeekDay - 1 : 6;//上个月在表里的时间结束Index
	int MonthEndIndexInCalendar = LastMonthEndIndexInCalendar + MonthDayCount;////本月在表里的时间结束Index
	for (int i = 0; i < 42; ++i)//总计有42天的天数
	{
		if (i <= LastMonthEndIndexInCalendar)
		{
			sStream << LastMonth << "|" << (LastMonthDayCount - LastMonthEndIndexInCalendar + i) << "#";
		}
		else if (i <= MonthEndIndexInCalendar)
		{
			sStream << iMonth << "|" << (i - LastMonthEndIndexInCalendar) << "#";
		}
		else
		{
			sStream << NextMonth << "|" << (i - MonthEndIndexInCalendar) << "#";
		}
	}
	return sStream.str();
}

StringType SC_ClientSkillInterface::GetItemEffectValueStr(int TypeID, int iValue)
{
	bool bFloat = false;
	switch(TypeID)
	{
	case e_IEP_ATT_CIR_DAM:
	case e_IEP_ATT_MAX:
	case e_IEP_ATT_DEF:
	case e_IEP_ATT_HP:
	case e_IEP_ATT_SPEED:
	case e_IEP_ATT_ATK_RATE:
	case e_IEP_ATT_CIR_RATE:
	case e_IE_ATT_DMG_RATE:
	case e_IE_ATT_DMG_DERATE:
	case e_IE_ATT_IGNORE_DEF:
	case e_IE_ATT_DMG_REBOUND:
		bFloat = true;
		break;
	default:
		{
			if(TypeID >= e_IEP_ATTR_SUB_TP_BEGIN && TypeID <= e_IEP_ATTR_SUB_TP_END)
			{
				bFloat = true;
			}
		}
		break;
	}

	StringType sResult;
	if(bFloat)
		UT_BaseStringUtility::format(sResult, "%.2f%%", iValue*0.01);
	else
		sResult = Int2String(iValue);
	return sResult;
}

StringType SC_ClientSkillInterface::_makeItemEffectRangeDesc(int TypeID, int iBaseValue, int iBonusValue)
{
	StringType ValueString;
	char buffer[256];

	int iFinal = iBaseValue + iBonusValue;

	switch(TypeID)
	{
	case e_IE_ATT_MAX:
	case e_IE_ATT_DEF:
	case e_IE_ATT_HP:
	case e_IE_ATT_MP:
	case e_IE_ATT_DOGGE:
	case e_IE_ATT_CIR_RATE:
	case e_IE_ATT_HP_REG:
	case e_IE_ATT_MP_REG:
	case e_IE_ATT_SPEED:
	case e_IE_ATT_RES_FREEZ:
	case e_IE_ATT_RES_SLOW:
	case e_IE_ATT_RES_SILENCE:
	case e_IE_ATT_RES_DIZZ:
	case e_IE_ATT_RES_SLEEP:
	case e_IE_ATT_STM:
	case e_IE_ATT_AMM_REG:
	case e_IE_ATT_GOLD_RANGER:
	case e_IE_ATTR_ATK_RATE:
	case e_IE_ATTR_CIR_DEF:
	case e_IE_ATTR_BASE_ATT_MAX:
	case e_IE_ATTR_BASE_DEF:
	case e_IE_ATTR_BASE_HP:
		{
			sprintf_s(buffer, " [ %d-%d ] ", iBaseValue, iFinal);
			break;
		}
	case e_IEP_ATT_CIR_DAM:
	case e_IEP_ATT_MAX:
	case e_IEP_ATT_DEF:
	case e_IEP_ATT_HP:
	case e_IEP_ATT_MP:
	case e_IEP_ATT_SPEED:
	case e_IEP_ATT_AMM:
	case e_IE_ATT_GAIN_EXP:
	case e_IE_ATT_MF_RATE:
	case e_IE_ATT_GF_RATE:
		{
			sprintf_s(buffer, " [ %.0f%%-%.0f%% ] ", (iBaseValue*0.01f+1), iFinal*0.01f);
			break;
		}
	case e_IE_ATTR_SOCKET:
		{
			break;
		}
	default:
		break;
	}
	ValueString = buffer;
	return ValueString;
}

int SC_ClientSkillInterface::_getAttribCount(int iItemID)
{
	int iCount = 0;
	CF_ItemGen::DataEntry *pkItemGen = SafeGetCSVFileDataEntryPtr<CF_ItemGen>(iItemID);
	if(NULL != pkItemGen)
	{		
		if(pkItemGen->GenEffect1 == 0)
			return iCount;

		iCount++;
		if(pkItemGen->GenEffect2 == 0)
			return iCount;

		iCount++;
		if(pkItemGen->GenEffect3 == 0)
			return iCount;

		iCount++;
		if(pkItemGen->GenEffect4 == 0)
			return iCount;

		iCount++;
		if(pkItemGen->GenEffect5 == 0)
			return iCount;

		iCount++;
		if(pkItemGen->GenEffect6 == 0)
			return iCount;

		iCount++;
	}

	return iCount;
}

StringType SC_ClientSkillInterface::_makeAttribString(LC_BackPackEntry *pkEntry, const SortItemEffect &kEffctInfo, bool bRefine)
{
	ItemEffectManage *pkItemEffectMgr = ItemEffectManage::GetSingletonPtr();
	if(NULL == pkItemEffectMgr)
		return "";

	if(NULL == pkEntry)
		return "";

	StringStreamType sStream("");

	Protocol::EquipAttrib pkEquipAttr = kEffctInfo.kEquipAttrib;
	int iAttribIdx = kEffctInfo.lIndex;
	if(pkEquipAttr.m_TypeID > 0)
	{
		ItemEffect *pkEffect = pkItemEffectMgr->FindEffect(pkEquipAttr.m_TypeID);
		if(pkEffect != NULL)
		{
			CF_ItemEffect::DataEntry *entry = SafeGetCSVFileDataEntryPtr<CF_ItemEffect>(pkEquipAttr.m_TypeID);
			if(NULL != entry)
			{
				CF_ItemEffectFunc::DataEntry *funcEntry = SafeGetCSVFileDataEntryPtr<CF_ItemEffectFunc>(entry->_lItemEffectID);
				if(NULL != funcEntry)
				{
					sStream << "1&" << iAttribIdx << "&" << pkEffect->GetDesc(pkEquipAttr.m_Level) << "&" << pkEquipAttr.m_Level;
				}

				bool bPerfect = false;
				CF_ItemGen::DataEntry *pkItemGen = SafeGetCSVFileDataEntryPtr<CF_ItemGen>(pkEntry->GetTypeID());
				if(NULL != pkItemGen)
				{
					sStream << "&" << pkItemGen->MaxEffectLevel;

					int iGenID = 0;
					switch(kEffctInfo.lIndex)
					{
					case 0:
						iGenID = pkItemGen->GenEffect1;
						break;
					case 1:
						iGenID = pkItemGen->GenEffect2;
						break;
					case 2:
						iGenID = pkItemGen->GenEffect3;
						break;
					case 3:
						iGenID = pkItemGen->GenEffect4;
						break;
					case 4:
						iGenID = pkItemGen->GenEffect5;
						break;
					case 5:
						iGenID = pkItemGen->GenEffect6;
						break;
					}

					if(iGenID > 0)
					{
						CF_ItemGenEffect * inst = CF_ItemGenEffect::GetSingletonPtr();
						if(inst)
						{
							CF_ItemGenEffect::DataEntryMapExternIterator iter = inst->GetIterator();
							while(iter.HasMoreElements())
							{
								const CF_ItemGenEffect::DataEntry& entry = iter.Next();
								if(entry._lItemDropGenID == iGenID && entry._lItemEffectID == pkEquipAttr.m_TypeID)
								{
									bPerfect = entry._bPerfect;
									break;
								}
							}
						}
					}
				}

				sStream << "&" << (int)bPerfect;
			}
		}
	}
	else
	{
		sStream << "0&" << iAttribIdx;
		if(bRefine)
			sStream << GET_MSG(SC_CLIENTSKILLINTERFACE_CPP_015);
		else
			sStream << GET_MSG(SC_CLIENTSKILLINTERFACE_CPP_019);
	}

	sStream << "&";

	int iVipLevel = 0;
	CF_VIPProfit::DataEntry *pkVipProfit = SafeGetCSVFileDataEntryPtr<CF_VIPProfit>(kEffctInfo.lVipLevel);
	if(NULL != pkVipProfit)
	{
		iVipLevel = _getRefineAttribVipLimit(pkVipProfit);
	}

	sStream << iVipLevel << "&" << kEffctInfo.sOperatinoInfo << "&" << kEffctInfo.iCustom;

	return sStream.str();
}
//=====================================================================
int SC_ClientSkillInterface::_getRefineAttribVipLimit(const CF_VIPProfit::DataEntry *pkVipProfit)
{
	if(pkVipProfit->_lVIP1Value > 0)
		return 1;

	if(pkVipProfit->_lVIP2Value > 0)
		return 2;

	if(pkVipProfit->_lVIP3Value > 0)
		return 3;

	if(pkVipProfit->_lVIP4Value > 0)
		return 4;

	if(pkVipProfit->_lVIP5Value > 0)
		return 5;

	if(pkVipProfit->_lVIP6Value > 0)
		return 6;

	if(pkVipProfit->_lVIP7Value > 0)
		return 7;

	if(pkVipProfit->_lVIP8Value > 0)
		return 8;

	if(pkVipProfit->_lVIP9Value > 0)
		return 9;

	if(pkVipProfit->_lVIP10Value > 0)
		return 10;

	return 0;
}
//=====================================================================
int SC_ClientSkillInterface::CanEquipOrUseItem(LC_ClientGamePlayer *pkPlayer, int lItemID, int iCid)
{
	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	LC_ClientItem*  pkItem = (LC_ClientItem* )pkItemFactory->RequestItem(lItemID);
	if(pkItem == NULL)
	{
		return ITEM_USE_TYPE_NULL;
	}

	bool bLevel = false;

	if(IS_EQUIP_ID(lItemID))
	{
		LC_SubordinateAsset& kSubAsset = pkPlayer->GetSubordinateAsset();

		const UT_SIMDataList& kEquipSubLmt = pkItem->GetEquipSubLimit();
		UT_SIMDataList::const_iterator itLmt = kEquipSubLmt.begin();
		for (; itLmt!=kEquipSubLmt.end(); ++itLmt)
		{
			LC_SubUpgradeData* pkSubData = kSubAsset.GetSubData(itLmt->ID());
			int32_t iLevel = pkSubData && pkSubData->IsEnabled() ? pkSubData->GetStage() : 0;

			if (iLevel < (int32_t)itLmt->ParamA())
			{
				return ITEM_USE_TYPE_SUB_LEVEL_LOW;
			}
		}

		int iCurLevel = iCid == 0 ? pkPlayer->GetCommanderLevel() : pkPlayer->GetBuddyLevel(iCid);
		bLevel =  iCurLevel >= pkItem->GetEquipLevelLimit();
	}
	else
	{
		if(pkItem->GetUseLimitFuncID() == 0)
		{
			return ITEM_USE_TYPE_CANUSE;
		}

		bLevel = pkPlayer->GetCommanderLevel() >= pkItem->GetUseMinLevelLimit();
	}

	if(!bLevel)
		return ITEM_USE_TYPE_LEVEL_LOW;
	else
		return ITEM_USE_TYPE_CANUSE;
}

bool SC_ClientSkillInterface::CanEquipSubItem(LC_PlayerBase *pkPlayer, int lItemID)
{
	if(pkPlayer == NULL)
		return false;

	CF_ItemList::DataEntry *pkItemData = SafeGetCSVFileDataEntryPtr<CF_ItemList>(lItemID);
	if (NULL == pkItemData)
		return false;

	if(pkItemData->_lCategory1 != ITEM_CATEGORY_SUB_EQUIP)
		return false;

	CF_ItemEquipLimitFunc::DataEntry *pkEquipLimitData = SafeGetCSVFileDataEntryPtr<CF_ItemEquipLimitFunc>(pkItemData->_lEquipLimitFuncID);
	if (NULL == pkEquipLimitData)
		return false;

	LC_SubordinateAsset& kSubAsset = pkPlayer->GetSubordinateAsset();
	UT_SIMDataList::iterator itLmt = pkEquipLimitData->_kSubLimits.begin();
	for (; itLmt!=pkEquipLimitData->_kSubLimits.end(); ++itLmt)
	{
		LC_SubUpgradeData* pkSubData = kSubAsset.GetSubData(itLmt->ID());
		int32_t iLevel = pkSubData && pkSubData->IsEnabled() ? pkSubData->GetStage() : 0;

		if (iLevel < (int32_t)itLmt->ParamA())
		{
			return false;
		}
	}

	return true;
}

bool SC_ClientSkillInterface::CanDecomposition(int lItemID)
{
	if(!IS_EQUIP_ID(lItemID))
	{
		return false;
	}

	CF_ItemList::DataEntry *entry = SafeGetCSVFileDataEntryPtr<CF_ItemList>(lItemID, false, false);
	if(NULL != entry)
	{
		return entry->_lDecompositionFuncID > 0;
	}

	return false;
}

StringType SC_ClientSkillInterface::GetItemDisassembleUseFuncResult(int lItemID, bool bProbability, int32_t iChoice)
{
	CF_ItemList::DataEntry *pkItemInfo = SafeGetCSVFileDataEntryPtr<CF_ItemList>(lItemID, false, false);
	if(NULL != pkItemInfo && !pkItemInfo->_kDisassembleUseFuncIDs.empty())
	{
		CF_ItemDisassembleUseFunc::DataEntry *pkItemDisassembleUseFunc = SafeGetCSVFileDataEntryPtr<CF_ItemDisassembleUseFunc>(pkItemInfo->_kDisassembleUseFuncIDs.front().ID());
		if(NULL != pkItemDisassembleUseFunc)
		{
			return _getRewardInfo(GetPlayerPtr(), pkItemDisassembleUseFunc->_lClientShowDropItems, bProbability, iChoice);
		}
	}

	return "";
}

StringType SC_ClientSkillInterface::GetItemDisassembleMustDropItems(int _iID)
{
	StringStreamType sStream("");
	CF_ItemDisassembleUseFunc::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_ItemDisassembleUseFunc>(_iID);
	if(pkData)
	{
		for (int i=0;i<pkData->_lMustDropItems.size();i++)
		{
			sStream<<pkData->_lMustDropItems[i].GetItemId()<<"#"<<pkData->_lMustDropItems[i].GetItemNum()<<"|";
		}
		
	}
	return sStream.str();
}

StringType	SC_ClientSkillInterface::GetMultiSelectGiftChoices(int lItemID)
{
	CF_ItemList::DataEntry *pkItemInfo = SafeGetCSVFileDataEntryPtr<CF_ItemList>(lItemID, false, false);
	if(NULL != pkItemInfo && !pkItemInfo->_kDisassembleUseFuncIDs.empty())
	{
		CF_ItemDisassembleUseFunc::DataEntry *pkItemDisassembleUseFunc = SafeGetCSVFileDataEntryPtr<CF_ItemDisassembleUseFunc>(pkItemInfo->_kDisassembleUseFuncIDs.front().ID());
		if(NULL != pkItemDisassembleUseFunc)
		{
			return _getChoiceInfo(GetPlayerPtr(), pkItemDisassembleUseFunc->_lClientShowDropItems);
		}
	}

	return "";
}

//获取多开礼包的选择
StringType SC_ClientSkillInterface::GetMultiSelectGiftChoicesCost(int lItemID)
{
	StringType sContent = "";
	CF_ItemList::DataEntry *pkItemInfo = SafeGetCSVFileDataEntryPtr<CF_ItemList>(lItemID, false, false);
	if(NULL != pkItemInfo && !pkItemInfo->_kDisassembleUseFuncIDs.empty())
	{
		CF_ItemDisassembleUseFunc::DataEntry *pkItemDisassembleUseFunc = SafeGetCSVFileDataEntryPtr<CF_ItemDisassembleUseFunc>(pkItemInfo->_kDisassembleUseFuncIDs.front().ID());
		if(NULL != pkItemDisassembleUseFunc)
		{
			Utility::UT_SIMDataList &CostList = pkItemDisassembleUseFunc->_lChoiceCosts;
			for(Utility::UT_SIMDataList::const_iterator it = CostList.begin(); it != CostList.end(); ++it)
			{
				sContent +=  Utility::Int2String(it->ID()) + "_" + Utility::Int2String(it->ParamA()) + "_" +Utility::Int2String(it->ParamB()) + "#";
			}
		}
	}

	return sContent;
}
int SC_ClientSkillInterface::GetItemShowID(int lItemID)
{
	CF_ItemList::DataEntry *pkItemInfo = SafeGetCSVFileDataEntryPtr<CF_ItemList>(lItemID, false, false);
	if(NULL != pkItemInfo)
	{
		LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
		if (!pkLogicManager)
			return lItemID;
		LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
		if (!pkGamePlayer)
			return lItemID;
		int eWeaponType = pkGamePlayer->GetWeaponAttribute();

		mem::vector<StringType> opts;
		mem::vector<StringType> params;
		mem::vector<StringType> weaponTypes;
		LC_Helper::SpliteString2MultiString(pkItemInfo->_lShowItemID, opts, ';');
		for(mem::vector<StringType>::iterator i = opts.begin(); i != opts.end(); ++i)
		{
			params.clear();
			StringType& opt = *i;
			LC_Helper::SpliteString2MultiString(opt,params,':');
			if(params.size() > 1)
			{
				if (eWeaponType == String2Int(params[0]))
				{
					lItemID = String2Int(params[1]);
					break;
				}
			}
		}
	}

	return lItemID;
}

StringType SC_ClientSkillInterface::GetBuddyExpDrugs()
{
	CF_ItemList::DataEntryMapExternIterator iter = CF_ItemList::GetSingletonPtr()->GetIterator();
	StringStreamType sStream("");

	for( ; iter.HasMoreElements(); iter.MoveNext())
	{
		const CF_ItemList::DataEntry *pkEntry = iter.PeekNextValuePtr();
		if(pkEntry->_lCategory2 == ITEM_CATEGORY_CONSUME_EXP_DRUGS)
		{
			sStream << pkEntry->_iID << "#";
		}
	}

	return sStream.str();
}

void SC_ClientSkillInterface::RequestRandomPlayerName(int iSex)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	NW_ClientSessionGroup *pkSessionGroup = NW_ClientSessionManager::GetSingletonPtr()->GetGameSessionGroup();

	MG_LC_Req_RandomCharName reqMsg;
	reqMsg.m_dwUserID = pkSessionGroup->m_dwUserID;
	reqMsg.m_Sex = iSex;
	GfxWriteLog(LOG_TAG_GAME_LOG, LOG_SWITCH_GAME,"请求随机名字，UserID=%d,Sex=%d",reqMsg.m_dwUserID,reqMsg.m_Sex);
	bCreateMsg = CreateMsgPacketHandler(pkSessionGroup, CRYPT_ALGORITHM_NONE, hMsgHandler, MGPT_LC_REQ_RANDOM_CHAR_NAME, &reqMsg);
	if(bCreateMsg)
	{
		pkSessionGroup->m_pkLoginSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}

float SC_ClientSkillInterface::GetArenaDailyRewardParam(int iRank, int iRewardType)
{
	//rank从1开始
	CF_RankReward::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_RankReward>(iRank, false, false);
	if (NULL == pkData)
		return 0;
	else
	{
		if (iRewardType == 1)
			return pkData->_DailyExpParm;
		else if (iRewardType == 2)
			return pkData->_DailyMoneyParm;
		else if(iRewardType == 3)
			return pkData->_DailyCourage;
		else if (iRewardType == 4)
			return pkData->_DailySP;
		return 0;
	}
}

int SC_ClientSkillInterface::GetGuildCreateRequireLevel(void)
{
	GuildCreateConfig* pCreateCfgMgr = GuildCreateConfig::Instance();
	if(pCreateCfgMgr == NULL)
	{
		return 0;
	}
	return pCreateCfgMgr->getLimit()->_iLevel;
}

int SC_ClientSkillInterface::GetGuildCreateRequireMoney(void)
{
	GuildCreateConfig* pCreateCfgMgr = GuildCreateConfig::Instance();
	if(pCreateCfgMgr == NULL)
	{
		return 0;
	}
	return pCreateCfgMgr->getLimit()->_iCash;
}

int SC_ClientSkillInterface::GetGuildCreateRequireYuanbao(void)
{
	GuildCreateConfig* pCreateCfgMgr = GuildCreateConfig::Instance();
	if(pCreateCfgMgr == NULL)
	{
		return 0;
	}
	return pCreateCfgMgr->getLimit()->_iYuanBao;
}

int SC_ClientSkillInterface::GetGuildCreateRequireVip(void)
{
	GuildCreateConfig* pCreateCfgMgr = GuildCreateConfig::Instance();
	if(pCreateCfgMgr == NULL)
	{
		return 0;
	}
	return pCreateCfgMgr->getLimit()->_iVipLvl;
}

StringType SC_ClientSkillInterface::GetBossIdList(void)
{
	CF_CharType::DataEntryMapExternIterator iter = CF_CharType::GetSingletonPtr()->GetIterator();

	StringStreamType sStream("");

	while(iter.HasMoreElements())	
	{
		const CF_CharType::DataEntry* pkEntry = iter.PeekNextValuePtr();
		if (3 == pkEntry->_bBoss)
		{
			sStream << pkEntry->_kCharName << "_" << pkEntry->_iID << "_" << pkEntry->_kTalkHeadIcon << "_\n";
		}

		iter.MoveNext();
	}

	return sStream.str();
}

bool SC_ClientSkillInterface::IsBoss(object_id_type lCharType)
{
	return SC_SkillInterface::IsBoss(lCharType);
}

bool SC_ClientSkillInterface::IsUnbindItem(int iItemID)
{
	CF_ItemList::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_ItemList>(iItemID);
	if(NULL == pkEntry)
	{
		return false;
	}

	return pkEntry->_lUnbindFuncID;
}

bool SC_ClientSkillInterface::CanBeBatchUsed(int iItemID)
{
	CF_ItemList::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_ItemList>(iItemID);
	if(NULL == pkEntry)
	{
		return false;
	}

	return pkEntry->_bBatchUse;
}

int SC_ClientSkillInterface::GetMapShowType(long lMapResID)
{
	CF_WorldMapList* pkWorldMapList = CF_WorldMapList::GetSingletonPtr();
	if (pkWorldMapList)
	{
		CF_WorldMapList::DataEntry* pkData = pkWorldMapList->GetEntryPtr(lMapResID);
		if (pkData)
			return pkData->_iMapShowType;
	}

	return (int)MST_NONE;
}

bool SC_ClientSkillInterface::GetMapAutoTask(int lMapResID)
{
	CF_WorldMapList::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(lMapResID);
	if (pkData != NULL)
		return !pkData->_bNoAutoTask;

	return true;
}

int SC_ClientSkillInterface::GetUIMapShowType(long lMapResID)
{
	CF_WorldMapList* pkWorldMapList = CF_WorldMapList::GetSingletonPtr();
	if (pkWorldMapList)
	{
		CF_WorldMapList::DataEntry* pkData = pkWorldMapList->GetEntryPtr(lMapResID);
		if (pkData)
			return pkData->_iShowUIMap;
	}

	return (int)UIMAP_SHOW_TYPE_NONE;
}

int SC_ClientSkillInterface::GetSchemeGroupID(long lMapResID)
{
	CF_WorldMapList* pkWorldMapList = CF_WorldMapList::GetSingletonPtr();
	if (pkWorldMapList)
	{
		CF_WorldMapList::DataEntry* pkData = pkWorldMapList->GetEntryPtr(lMapResID);
		if (pkData)
			return pkData->_iSchemeID;
	}

	return 0;
}

int64_t SC_ClientSkillInterface::GetHarmQuantity(long lMapResID)
{
	CF_WorldMapList* pkWorldMapList = CF_WorldMapList::GetSingletonPtr();
	if (pkWorldMapList)
	{
		CF_WorldMapList::DataEntry* pkData = pkWorldMapList->GetEntryPtr(lMapResID);
		if (pkData)
			return pkData->_HarmQuantity;
	}

	return 0;
}

int SC_ClientSkillInterface::GetMapBelongStoryID(long lMapResID)
{
	CF_WorldMapList* pkWorldMapList = CF_WorldMapList::GetSingletonPtr();
	if (pkWorldMapList)
	{
		CF_WorldMapList::DataEntry* pkData = pkWorldMapList->GetEntryPtr(lMapResID);
		if (pkData)
			return pkData->_iStoryID;
	}

	return 0;
}

int SC_ClientSkillInterface::GetMapSchemeTeamCount(int iMapResID)
{
	CF_WorldMapList::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(iMapResID);
	if (pkData != NULL)
	{
		CF_Scheme::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_Scheme>(pkData->_iSchemeID);
		if(pkEntry != NULL)
		{
			return pkEntry->_schemeGroupNum;
		}
	}

	return 0;
}

int SC_ClientSkillInterface::GetShapeShiftInfo(GameLogic::LC_Attacker* pkScriptCaller)
{
	if (pkScriptCaller == NULL)
		return -1;
	
	if (IS_PLAYER_CLASS_ID(pkScriptCaller->GetID()))
	{
		LC_PlayerBase* pkPlayer = (LC_PlayerBase*)pkScriptCaller;
		return pkPlayer->GetShapeShiftCharTypeID();
	}
	else if (IS_NPC_ID(pkScriptCaller->GetID()))
	{ 
		LC_ClientNPC* pkNPC = (LC_ClientNPC*)pkScriptCaller;
		return pkNPC->GetShapeShiftCharTypeID();
	}
	return -1;
}

StringType SC_ClientSkillInterface::GetVipAuthorityString(int iVipLevel)
{
	StringStreamType sStream("");
	CF_VIP::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_VIP>(iVipLevel);
	if(NULL != pkEntry)
	{
		sStream<<pkEntry->_sVipDescription;
	}

	return sStream.str();
}

StringType SC_ClientSkillInterface::GetVipNameString(int iVipLevel)
{
	StringStreamType sStream("");
	CF_VIP::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_VIP>(iVipLevel);
	if(NULL != pkEntry)
	{
		sStream<<pkEntry->_sDescrip;
	}

	return sStream.str();
}

StringType SC_ClientSkillInterface::GetVipPrivelegeInfo()
{
	StringStreamType sStream;
	CF_VIPProfitDescribe::DataEntryMapExternIterator kIter = CF_VIPProfitDescribe::GetSingletonPtr()->GetIterator();
	for( ; kIter.HasMoreElements(); kIter.MoveNext() )
	{
		const CF_VIPProfitDescribe::DataEntry *pkData = kIter.PeekNextValuePtr();
		sStream <<GetShowVipPrivelegeInfoByType(pkData->_iID) << "|";
	}

	return sStream.str();
}

StringType SC_ClientSkillInterface::GetVipPrivelegeInfoByType(int iType)
{
	StringStreamType sStream;
	CF_VIPProfit::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_VIPProfit>(iType);
	if(pkData != NULL)
	{
		sStream << Int2String(pkData->_iID) << "#" << pkData->_sDescrip << "#" << pkData->_lVIP0Value << "#";
		sStream << pkData->_lVIP1Value << "#" << pkData->_lVIP2Value << "#";
		sStream << pkData->_lVIP3Value << "#" << pkData->_lVIP4Value << "#";
		sStream << pkData->_lVIP5Value << "#" << pkData->_lVIP6Value << "#";
		sStream << pkData->_lVIP7Value << "#" << pkData->_lVIP8Value << "#";
		sStream << pkData->_lVIP9Value << "#" << pkData->_lVIP10Value << "#";
		sStream << pkData->_lVIP11Value << "#" << pkData->_lVIP12Value << "#";
		sStream << pkData->_lVIP13Value << "#" << pkData->_lVIP14Value << "#";
		sStream << pkData->_lVIP15Value;
	}

	return sStream.str();
}

int SC_ClientSkillInterface::GetVipPrivelegeInfoByTypeAndLevel(int iType,int iLevel)
{
	CF_VIPProfit::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_VIPProfit>(iType);
	switch(iLevel)
	{
	case 0:
		return pkData->_lVIP0Value;
	case 1:
		return pkData->_lVIP1Value;
	case 2:
		return pkData->_lVIP2Value;
	case 3:
		return pkData->_lVIP3Value;
	case 4:
		return pkData->_lVIP4Value;
	case 5:
		return pkData->_lVIP5Value;
	case 6:
		return pkData->_lVIP6Value;
	case 7:
		return pkData->_lVIP7Value;
	case 8:
		return pkData->_lVIP8Value;
	case 9:
		return pkData->_lVIP9Value;
	case 10:
		return pkData->_lVIP10Value;
	case 11:
		return pkData->_lVIP11Value;
	case 12:
		return pkData->_lVIP12Value;
	case 13:
		return pkData->_lVIP13Value;
	case 14:
		return pkData->_lVIP14Value;
	case 15:
		return pkData->_lVIP15Value;
	default:
		return 0;
	}
}

StringType SC_ClientSkillInterface::GetShowVipPrivelegeInfoByType(int iLevel)
{
	StringStreamType sStream("");
	CF_VIPProfitDescribe::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_VIPProfitDescribe>(iLevel);
	if(pkData != NULL)
	{
		sStream << Int2String(pkData->_iID) << "#" << pkData->_sDescrip << "#";
		sStream << pkData->_lVIP1Value << "#" << pkData->_lVIP2Value << "#";
		sStream << pkData->_lVIP3Value << "#" << pkData->_lVIP4Value << "#";
		sStream << pkData->_lVIP5Value << "#" << pkData->_lVIP6Value << "#";
		sStream << pkData->_lVIP7Value << "#" << pkData->_lVIP8Value << "#";
		sStream << pkData->_lVIP9Value << "#" << pkData->_lVIP10Value << "#";
		sStream << pkData->_lVIP11Value << "#" << pkData->_lVIP12Value << "#";
		sStream << pkData->_lVIP13Value << "#" << pkData->_lVIP14Value << "#";
		sStream << pkData->_lVIP15Value << "#" << Int2String(pkData->bTick);
	}

	return sStream.str();
}

StringType SC_ClientSkillInterface::GetFlowerSendInfo(int iID)
{
	StringStreamType sStream("");
	CF_FlowerConfig::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_FlowerConfig>(iID);
	if(pkEntry != NULL)
		sStream << pkEntry->_sDescrip << "#" << pkEntry->_lVipLvl;

	return sStream.str();
}

int SC_ClientSkillInterface::GetMaxGuildSearchNum(void)
{
	return MAX_GUILD_SEARCH_NUM;
}

int SC_ClientSkillInterface::GetLivenessByLevel(int iLevel)
{
	CF_DailyLiveness::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_DailyLiveness>(iLevel);
	if (!pkData)
		return 0;
	return pkData->_lLiveness;
}

StringType SC_ClientSkillInterface::GetChoosePetIds()
{
	StringStreamType sStream;
	const mem::vector<int>& Ids = GetGlobalSetting.petIDs;
	for(size_t i=0; i<Ids.size(); ++i)
	{
		sStream << Int2String(Ids[i]) << '#';
	}

	return sStream.str();
}

StringType SC_ClientSkillInterface::GetIconByNPCFuntionType(int iNPCFunctionType)
{
	CF_NPCFunctionType::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_NPCFunctionType>(iNPCFunctionType);
	if (!pkData)
		return "";
	return pkData->_kIcon;
}

StringType SC_ClientSkillInterface::GetProgressInfo(int iStep)
{

	StringStreamType sStream("");

	CF_MapCategory::DataEntryMapExternIterator kIter = CF_MapCategory::GetSingletonPtr()->GetIterator();
	for( ; kIter.HasMoreElements(); kIter.MoveNext() )
	{
		const CF_MapCategory::DataEntry* pkEntry = kIter.PeekNextValuePtr();
		//step为0时，设成第一项的值
		if(iStep == 0)
			iStep = pkEntry->_lMapClassicID;

		if (iStep == pkEntry->_lMaprResID)
		{
			sStream << pkEntry->_lMapClassicBossID << "#";
			sStream << pkEntry->_kCategoryName << "#";
			sStream << pkEntry->_lChapter << "#";
			sStream << pkEntry->_lMaprResID << "#";

			if(pkEntry->_lMapClassicBossID > 0)
			{
				const CF_CharType::DataEntry *pkCharTypeEntry = SafeGetCSVFileDataEntryPtr<CF_CharType>(pkEntry->_lMapClassicBossID);
				if(NULL != pkCharTypeEntry)
				{				
					sStream << pkCharTypeEntry->_kTalkHeadIcon << "#";
				}
			}

			break;
		}
	}

	return sStream.str();
}

StringType SC_ClientSkillInterface::GetMapCateGoryInfo(void)
{
	StringStreamType sStream("");

	CF_MapCategory::DataEntryMapExternIterator kIter = CF_MapCategory::GetSingletonPtr()->GetIterator();
	for( ; kIter.HasMoreElements(); kIter.MoveNext() )
	{
		const CF_MapCategory::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		if (pkEntry->_lChapter > 0)
		{
			sStream << pkEntry->_lMapClassicBossID << "#";
			sStream << pkEntry->_iRaidType << "#";
			sStream << pkEntry->_lMaprResID << "#";
			sStream << pkEntry->_kName << "#";
			sStream << pkEntry->_lID << "#";
			sStream << pkEntry->_lChapter << "#";
			sStream << pkEntry->_kWorldMapPos << "#";

			//城镇，野外没有最低等级和消耗活力值
			if(pkEntry->_iRaidType != MST_TOWN && pkEntry->_iRaidType != MST_FIELD)
			{
				const CF_TranscriptionRule::DataEntry *pkTransRule = SafeGetCSVFileDataEntryPtr<CF_TranscriptionRule>(pkEntry->_lMaprResID);
				if(NULL != pkTransRule)
				{
					sStream << pkTransRule->_iPlayerMinLevel << "#";
					sStream << pkTransRule->_iStamina << "#";
				}
			}

			sStream << "|";
		}
	}

	return sStream.str();
}

StringType SC_ClientSkillInterface::GetMapRwdInfo(int iMapResID)
{
	StringStreamType sStream;
	const CF_TranscriptionRule::DataEntry *pkTransRule = SafeGetCSVFileDataEntryPtr<CF_TranscriptionRule>(iMapResID);
	if(NULL != pkTransRule)
	{
		sStream << pkTransRule->_kName << "#";
		sStream << pkTransRule->_iStamina << "#";
		sStream << pkTransRule->_kIntro << "#";
		sStream << pkTransRule->_kMonster << "#";
		sStream << "" << "#";
		sStream << pkTransRule->_Money_Quick << "#";
		sStream << "" << "#";
		sStream << pkTransRule->_kReward << "#";
		sStream << pkTransRule->_kRandomReward << "#";
		sStream << pkTransRule->_iPlayerMinLevel << "#";

		sStream << pkTransRule->_CombatMinScore << "#";
		sStream << pkTransRule->_CombatMaxScore << "#";

		StringType sMaterial;
		FormatDataIFs2String(pkTransRule->_Consume_Quick_Money, sMaterial);
		sStream << sMaterial << "#";
	}

	return sStream.str();
}

int SC_ClientSkillInterface::GetShowRaidStage(int iMapResID)
{
	return LC_Helper::GetShowRaidStage(iMapResID);
}

//副本中复活需要替身草人数量
int SC_ClientSkillInterface::GetRespawnItemNum(int iMapResID)
{
	CF_WorldMapList::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(iMapResID);
	if (NULL == pkData)
		return 0;

	return pkData->_iRespawnItemNum;
}

bool SC_ClientSkillInterface::CanScreenPosMoveTo(float x, float y)
{
	UT_Vec3Int pos;
	LC_MapManager::GetSingletonPtr()->GetMiniMap()->CalcMapPosToWorld(y, x, pos.x, pos.y);

	SE_SceneManager* pkSceneManager = SE_SceneManager::GetSingletonPtr();
	const SE_SceneTerrainLocalInfo& kInfo = pkSceneManager->GetWalkableInfo((float)pos.x, (float)pos.y);
	pos.z = static_cast < int > ( kInfo._fLocalHeight );

	Path::PF_PathManager* pkPathMgr = Path::PF_PathManager::GetSingletonPtr();
	if ( pkPathMgr )
	{
		bool bCanMove = pkPathMgr->TestCanMoveTo( pos , false );
		return bCanMove;
	}

	return false;
}
//URLENCODE函数 林培华 2014/4/18

StringType SC_ClientSkillInterface::URLEncode(const StringType &sIn)
{
	return LC_Helper::URLEncode(sIn);
}

StringType SC_ClientSkillInterface::GetCrossRealmMapInfo(int iMapTypeID)
{
	CF_CrossRealmMap::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_CrossRealmMap>(iMapTypeID);
	if(NULL != pkEntry)
	{
		StringStreamType sStream("");
		sStream << pkEntry->_kName << "_" << pkEntry->_kLevel << "_" << pkEntry->_kPlayCount << "_";
		sStream << pkEntry->_kPlayScore << "_" << pkEntry->_kBoss << "_" << pkEntry->_kFirstReward << "_";
		sStream << pkEntry->_kDailyReward << "_" << pkEntry->_kBoss1 << "_" << pkEntry->_kBoss2 << "_";
		sStream << pkEntry->_kBoss3 << "_" << pkEntry->_kBoss4 << "_" << pkEntry->_kCoordinate << "_" << pkEntry->_kNamePre << "_" << pkEntry->_kBossName;
		return sStream.str();
	}

	return "";
}

int	SC_ClientSkillInterface::GetCrossRealmMapTargetType(int iMapTypeID)
{
	CF_CrossRealmMap::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_CrossRealmMap>(iMapTypeID);
	if(NULL !=pkEntry)
	{
		return pkEntry->_kTargetType;
	}
	return -1;
}
unique_id_type SC_ClientSkillInterface::GetKungFuMasterID()
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager)
		return 0;
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if( NULL == pkGamePlayer)
		return 0;

	if ( pkLogicManager->GetGuildWarAssert() == NULL)
		return 0;

	Protocol::PS_PowerGuildInfo* pkPowerGuildInfo = pkLogicManager->GetGuildWarAssert()->GetPowerGuildInfo(pkGamePlayer->GetActiveServer());
	if (NULL == pkPowerGuildInfo)
		return 0;
	return pkPowerGuildInfo->m_AllMember[0].m_CharID;
}

unique_id_type SC_ClientSkillInterface::GetCrossKungFuMasterID()
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager)
		return 0;
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if( NULL == pkGamePlayer)
		return 0;
	if ( pkLogicManager->GetGuildWarAssert() == NULL)
		return 0;

	Protocol::PS_CrossRealm_PowerGuildInfo* pkGlobalGuildInfo = pkLogicManager->GetGuildWarAssert()->GetGlobalPowerGuildInfo(pkLogicManager->GetGuildWarAssert()->GetGroupId());
	if (NULL == pkGlobalGuildInfo)
		return 0;
	return pkGlobalGuildInfo->m_CharID;
}

int	SC_ClientSkillInterface::GetCrossRealWarServerID()
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager)
		return 0;
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if( NULL == pkGamePlayer)
		return 0;
	if ( pkLogicManager->GetGuildWarAssert() == NULL)
		return 0;

	Protocol::PS_CrossRealm_PowerGuildInfo* pkGlobalGuildInfo = pkLogicManager->GetGuildWarAssert()->GetGlobalPowerGuildInfo(pkLogicManager->GetGuildWarAssert()->GetGroupId());
	if (NULL == pkGlobalGuildInfo)
		return 0;
	return pkGlobalGuildInfo->m_ServerId;
}

int SC_ClientSkillInterface::GetKungFuMasterDress(unique_id_type CharID, int prevItemID, int EquipType, bool ignoreFashion)
{
	int equipItemID = prevItemID;
	CF_SpecialEquipAvatar::DataEntry* entry = NULL;

	if (!ignoreFashion && IS_FASHION_ID(prevItemID) )
		return prevItemID;

	if( GetCrossKungFuMasterID() == CharID )
	{
		entry = SafeGetCSVFileDataEntryPtr<CF_SpecialEquipAvatar>(SKF_CROSSGUILDWAR);	
	}
	else if (GetKungFuMasterID() == CharID)
	{
		entry = SafeGetCSVFileDataEntryPtr<CF_SpecialEquipAvatar>(SKF_GUILDWAR);	
	}
	else
	{
		equipItemID = prevItemID;
	}

	if (entry)
	{
		switch (EquipType)
		{
		case 0:
			equipItemID = entry->_lHeadID;
			break;
		case 1:
			equipItemID = entry->_lBodyID;
			break;
		case 2:
			equipItemID = entry->_lFeetID;;
			break;
		}
	}

	return equipItemID;
}

int SC_ClientSkillInterface::GetKungFuMasterDress2(int prevItemID, int EquipType, int index, bool ignoreFashion)
{
	if (!ignoreFashion &&  IS_FASHION_ID(prevItemID) )
		return prevItemID;

 	int equipItemID = prevItemID;
	CF_SpecialEquipAvatar::DataEntry* entry = SafeGetCSVFileDataEntryPtr<CF_SpecialEquipAvatar>(index);
	if(!entry)
	{
		equipItemID = prevItemID;
	}
	if (entry)
	{
		switch (EquipType)
		{
		case 0:
			equipItemID = entry->_lHeadID;
			break;
		case 1:
			equipItemID = entry->_lBodyID;
			break;
		case 2:
			equipItemID = entry->_lFeetID;;
			break;
		}
	}
	
	return equipItemID;
}

guild_id_type SC_ClientSkillInterface::GetPowerGuildID()
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager)
		return 0;
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if( NULL == pkGamePlayer)
		return 0;
	if ( pkLogicManager->GetGuildWarAssert() == NULL)
		return 0;

	Protocol::PS_PowerGuildInfo* pkPowerGuildInfo = pkLogicManager->GetGuildWarAssert()->GetPowerGuildInfo(pkGamePlayer->GetActiveServer());
	if (NULL == pkPowerGuildInfo)
		return 0;
	return pkPowerGuildInfo->m_GuildId;
}

int SC_ClientSkillInterface::GetDirtyFlagRevisionID(int iFlag)
{
	int iRevisionID = 0;

	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	//先判断logicmanger
	iRevisionID = pkLogicManger->GetDirtyFlagRevisionID(iFlag);
	if (iRevisionID > 0)
		return iRevisionID;
	
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if( NULL == pkGamePlayer)
		return iRevisionID;
	
	//再判断主角色身上
	iRevisionID = pkGamePlayer->GetDirtyFlagRevisionID(iFlag);
	if (iRevisionID > 0)
		return iRevisionID;

	switch(iFlag)
	{
	case DIRTY_FLAG_GUILDRAID_INTO:
		{
			iRevisionID = LC_GuildRaidDataManager::GetSingleton().GetDirtyFlagRevisionID();
		}
		break;
	}

	//判断logicmanager
	return iRevisionID;
}

bool SC_ClientSkillInterface::IsDomainMap(int iMapResID)
{
	CF_ForcesBattle::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_ForcesBattle>(iMapResID);
	if (NULL == pkData)
		return  false;

	return true;
}

StringType SC_ClientSkillInterface::GetDomainFlagPos(int iMapResID)
{
	CF_ForcesBattle::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_ForcesBattle>(iMapResID);
	if (NULL == pkData)
		return  "";
	
	StringStreamType sStream("");
	sStream << pkData->_Pos.x << "#";
	sStream << pkData->_Pos.y << "#";

	return sStream.str();
}

int SC_ClientSkillInterface::GetDomainMapResIDByIndex(int iIndex)
{
	CF_ForcesBattle::DataEntryMapExternIterator it = CF_ForcesBattle::GetSingleton().GetIterator();
	if(iIndex <= 0)
		return 0;
	while (it.HasMoreElements())
	{
		const CF_ForcesBattle::DataEntry* pkData = it.PeekNextValuePtr();
		if (pkData->_Index == iIndex)
			return pkData->_MapResID;
		
		it.MoveNext();
	}
	
	return 0;
}

bool SC_ClientSkillInterface::IsDomainActive()
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager )
		return false;

	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if( NULL == pkGamePlayer)
		return false;

	LC_WorldGuildWarAsset* kWorldGuildWarAssert = pkLogicManager->GetGuildWarAssert();
	if ( NULL == kWorldGuildWarAssert )
		return false;

	int64_t serverTime = pkGamePlayer->GetServerTimeVal();
	if(kWorldGuildWarAssert->GetDomainBeginStamp() <= serverTime && kWorldGuildWarAssert->GetDomainEndStamp() >= serverTime)
		return true;
	return false;
}

int SC_ClientSkillInterface::CheckDomainReward(int iMapResID)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager )
		return DOMAIN_REWARD_TYPE_CANT;

	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if( NULL == pkGamePlayer)
		return DOMAIN_REWARD_TYPE_CANT;

	/*if(LC_Helper::CanEnterMap(iMapResID, pkGamePlayer->GetLevel()) != RE_SUCCESS)
		return DOMAIN_REWARD_TYPE_CANT;*/

	LC_WorldGuildWarAsset* kWorldGuildWarAssert = pkLogicManager->GetGuildWarAssert();
	if ( NULL == kWorldGuildWarAssert )
		return DOMAIN_REWARD_TYPE_CANT;

	const DomainPowerMap* pkDomainPowerMap = kWorldGuildWarAssert->GetDomainPowerGuildInfo(pkGamePlayer->GetActiveServer());
	if(NULL == pkDomainPowerMap)
		return DOMAIN_REWARD_TYPE_CANT;

	DomainPowerMap::const_iterator it = pkDomainPowerMap->find(iMapResID);
	if(it == pkDomainPowerMap->end())
		return DOMAIN_REWARD_TYPE_CANT;
	if(it->second.m_GuildId != pkGamePlayer->GetGuildID())
		return DOMAIN_REWARD_TYPE_CANT;

	if (!pkGamePlayer->GetPlayerGuildAssert()->CheckDomainReward(iMapResID,it->second.m_TimeStamp))
		return DOMAIN_REWARD_TYPE_DRAWED;
	
	return DOMAIN_REWARD_TYPE_HAVE;
}

StringType SC_ClientSkillInterface::GetMapDomainInfo(int iMapResID,int iServerID)
{
	StringStreamType sStream("");

	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager )
		return "";

	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if( NULL == pkGamePlayer)
		return "";

	LC_WorldGuildWarAsset* kWorldGuildWarAssert = pkLogicManager->GetGuildWarAssert();
	if ( NULL == kWorldGuildWarAssert )
		return "";

	const DomainPowerMap* pkDomainPowerMap = kWorldGuildWarAssert->GetDomainPowerGuildInfo(iServerID==0?pkGamePlayer->GetActiveServer():iServerID);
	if(NULL == pkDomainPowerMap)
	{
		sStream << 0 << "#";
		sStream << "" << "#";
		sStream << 0 << "#";
		sStream << "" << "#";
		sStream << 0 << "#";
	}
	else
	{
		DomainPowerMap::const_iterator it = pkDomainPowerMap->find(iMapResID);
		if(it == pkDomainPowerMap->end())
		{
			sStream << 0 << "#";
			sStream << "" << "#";
			sStream << 0 << "#";
			sStream << "" << "#";
			sStream << 0 << "#";
		}
		else
		{
			sStream << it->second.m_GuildId << "#";
			sStream << it->second.m_GuildName << "#";
			sStream << it->second.m_CharID << "#";
			sStream << it->second.m_PlayerName << "#";
			sStream << pkGamePlayer->GetServerTimeVal() - it->second.m_TimeStamp << "#";
		}
	}


	CF_ForcesBattle::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_ForcesBattle>(iMapResID);
	if(NULL == pkData)
	{
		sStream << "0#0#0#"; //经验、真气、金币
	}
	else
	{
		/*int iLevel = pkGamePlayer->GetLevel();*/
		int iLevel = pkGamePlayer->GetCommanderLevel();
		sStream << (int)LC_Helper::ExpressionEval(pkData->_Exp, "[PLAYERLV]", iLevel) << "#";	//经验
		sStream << (int)LC_Helper::ExpressionEval(pkData->_Gold, "[PLAYERLV]", iLevel) << "#";	//金币
		sStream << (int)LC_Helper::ExpressionEval(pkData->_Sp, "[PLAYERLV]", iLevel) << "#";	//真气

		Utility::UT_SIMDataList	&RewardItems = pkData->_Rewards;
		for(int i=0; i<RewardItems.size(); ++i)
		{
			int iItemID = RewardItems[i].ID();
			sStream << RewardItems[i].ID() << "#" << RewardItems[i].ItemCount() << "#";
		};
	}
	

	return sStream.str();
}

int64_t SC_ClientSkillInterface::NumberPlus(int64_t iNumber1, int64_t iNumber2)
{
	return iNumber1 + iNumber2;
}

int64_t SC_ClientSkillInterface::GetPlayerCombatScore(LC_ClientGamePlayer *pkPlayer)
{
	if(NULL == pkPlayer)
		return 0;

	return pkPlayer->GetCommanderTotalScore();
}

bool SC_ClientSkillInterface::IsKungFuMasterID(unique_id_type iUserID)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager)
		return false;

	LC_WorldGuildWarAsset* kWorldGuildWarAssert = pkLogicManager->GetGuildWarAssert();
	if ( NULL == kWorldGuildWarAssert )
		return false;

	int iPosition = -1;
	if (NULL == kWorldGuildWarAssert->GetPlayerPowerGuild(iUserID, iPosition))
		return false;
	
	return iPosition == GUILD_MEMBER_MASTER;
}

StringType SC_ClientSkillInterface::GetGuildWarCatapultList(int iPlayerLevel)
{
	StringStreamType sStream("");
	
	bool bHasValue = false;

	CF_GuildWarCatapult::DataEntryMapExternIterator kIter = CF_GuildWarCatapult::GetSingletonPtr()->GetIterator();
	for( ; kIter.HasMoreElements(); kIter.MoveNext() )
	{
		const CF_GuildWarCatapult::DataEntry* pkData = kIter.PeekNextValuePtr();
		if (pkData->_Level > iPlayerLevel)
			break;
		
		if (bHasValue)
		{
			sStream << "#";
		}

		sStream << pkData->_iID << "_";
		sStream << pkData->_Name << "_";
		sStream << pkData->_Cost << "_";
		sStream << pkData->_PowerDesc;

		bHasValue = true;
	}
	
	return sStream.str();
}

bool SC_ClientSkillInterface::IsCrossRealmMap(int iMapResID)
{
	return LC_Helper::IsCrossRealmMap(iMapResID);
}

bool SC_ClientSkillInterface::IsGuildRaidMap(int iMapResID)
{
	return LC_GuildRaidDataManager::GetSingleton().IsGuildRaidMap(iMapResID);
}

StringType SC_ClientSkillInterface::GetCharTalkIcon(int iCharID)
{
	CF_CharType::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_CharType>(iCharID);
	if(NULL != pkEntry)
	{
		return pkEntry->_kTalkHeadIcon;
	}

	return "";
}

uint32_t SC_ClientSkillInterface::GetCharNameColor(LC_PlayerBase* pkWatcher, LC_ActorBase* pkTarget, int iPowerGuildID/* = -1*/)
{
	const UT_Vec3Float kColor = LC_Helper::ComputeActorNameColor(pkWatcher, pkTarget, iPowerGuildID);
	uint32_t iResult = RGB2Int(kColor.x, kColor.y, kColor.z);
	return iResult;
}

int SC_ClientSkillInterface::GetDungeonTime(int iMapResID)
{
	CF_TranscriptionRule::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_TranscriptionRule>(iMapResID);
	if(NULL != pkEntry)
	{
		return pkEntry->_iExistTime;
	}

	return 0;
}

bool SC_ClientSkillInterface::CanEnterMap(int iMapResID)
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(NULL == pkGamePlayer)
		return false;

	int iRetValue = 0;
	return RE_SUCCESS == LC_ClientHelp::CanEnterMap(iMapResID, pkGamePlayer, iRetValue);
}

int SC_ClientSkillInterface::CanEnterMapWithPrompt(int iMapResID)
{
	int iResult = RE_FAIL;

	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(NULL == pkGamePlayer)
		return iResult;

	int iRetValue = 0;
	iResult = LC_ClientHelp::CanEnterMap(iMapResID, pkGamePlayer, iRetValue);

	switch(iResult)
	{
	case RE_CANNOT_TRANSFERMAP_TASK_FAIL:
		{
			LC_ClientTask* pkTask = (LC_ClientTask*)LC_TaskManager::GetSingleton().GetTask(iRetValue);
			if(pkTask != NULL)
				SendLogicInfoMessage(RE_CANNOT_TRANSFERMAP_TASK_FAIL, pkTask->GetTaskGroupID(), pkTask->GetClientTaskGroupID());
		}
		break;
	default:
		SendLogicInfoMessage(iResult, iRetValue);
		break;
	}

	return iResult;
}

int SC_ClientSkillInterface::GetMapNeedLevel(int iMapResID)
{
	CF_WorldMapList::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(iMapResID);
	if(NULL == pkData)
		return 0;
	return pkData->_iNeedLevel;
}

StringType SC_ClientSkillInterface::GetMapPreTaskName(int iMapResID)
{
	CF_WorldMapList::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(iMapResID);
	if (NULL == pkData)
		return "";

	CF_TaskList::DataEntry* pkTaskData = SafeGetCSVFileDataEntryPtr<CF_TaskList>(pkData->iPreTaskID);
	if(NULL == pkTaskData)
		return "";
	return pkTaskData->_kName;
}

StringType SC_ClientSkillInterface::GetHookMapsInfo()
{
	StringStreamType sStream;

	CF_HookMaps::DataEntryMapExternIterator iter = CF_HookMaps::GetSingleton().GetIterator();
	for(; iter.HasMoreElements(); iter.MoveNext())
	{
		const CF_HookMaps::DataEntry *pkEntry = iter.PeekNextValuePtr();
		sStream << pkEntry->_iID << '#' << pkEntry->_iMapID << '#' << pkEntry->_sBossLevel << '#';
		sStream << pkEntry->_sBossNumber << '#' << pkEntry->_sRefreshTime << '#' << pkEntry->_sPic << '#';

		CF_WorldMapList::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(pkEntry->_iMapID);
		if(pkData != NULL)
			sStream << pkData->_needNewVipLevel << '#' << pkData->_iNeedLevel << '#';
		else
			sStream << "0#0#";

		sStream << pkEntry->_Reward;
		sStream << '|';
	}

	return sStream.str();
}

StringType SC_ClientSkillInterface::GetHolidayRankRewardsInfo(int iType, int iParam)
{
	StringStreamType sStream;
	CF_ActivityRankReward::DataEntryMapExternIterator iter = CF_ActivityRankReward::GetSingletonPtr()->GetIterator();
	for( ; iter.HasMoreElements(); iter.MoveNext())
	{
		CF_ActivityRankReward::DataEntry *pkData = iter.PeekNextValuePtr();
		if(pkData->_GroupID == iType && pkData->_iParam == iParam)
		{
			SerializeDataEntryPtr<CF_ActivityRankReward>(pkData, sStream,'#');
			sStream<<"|";
		}
	}
	return sStream.str();
}

StringType SC_ClientSkillInterface::GetGamePlayerSubRewardsInfo(int iGroupId)
{
	StringStreamType sStream;
	CF_ShotActivityLimitRankRewards::DataEntryMapExternIterator iter = CF_ShotActivityLimitRankRewards::GetSingletonPtr()->GetIterator();
	for( ; iter.HasMoreElements(); iter.MoveNext())
	{
		CF_ShotActivityLimitRankRewards::DataEntry *pkData = iter.PeekNextValuePtr();
		if(pkData->_GroupID == iGroupId)
		{
			sStream<<pkData->_iRankStartIndex<<"#"<<pkData->_iRankEndIndex<<"#";
			for(int i=0;i<pkData->_kReward.size();i++)
			{
				sStream<<pkData->_kReward[i].GetItemId()<<":"<<pkData->_kReward[i].GetItemNum()<<";";
			}
			sStream<<"|";
		}
	}
	return sStream.str();
}

combat_score_type SC_ClientSkillInterface::GetCimeliaTotalScore(LC_PlayerBase *pkPlayer, int iType, int iLevel)
{
	return LC_Helper::GetCimeliaTotalScore(pkPlayer, iType, iLevel);
}

combat_score_type SC_ClientSkillInterface::GetCharAttributeCombatScore(long lAttribFunc)
{
	return LC_Helper::GetCharAttrCombatScore(lAttribFunc);
}

//获取坐骑等战斗力不含零时属性
combat_score_type	SC_ClientSkillInterface::GetCimeliaOriginalScore(int iType, int iLevel)
{
	combat_score_type iAttrScore = 0;
	const LC_SubordinateDataEntry *pkEntry = SafeGetSubDataEntry(iType, iLevel);
	if (NULL != pkEntry)
	{
		//初始皮肤属性战力
		CF_SubordinateTransform::DataEntryMapExternIterator kIter = CF_SubordinateTransform::GetSingleton().GetIterator();
		while(kIter.HasMoreElements())
		{
			CF_SubordinateTransform::DataEntry* pkData = kIter.PeekNextValuePtr();
			if (pkData->_iType == iType && pkData->_iSubType == 0 && pkData->_iLevel == iLevel)
			{
				iAttrScore += LC_Helper::GetCharAttrCombatScore(pkData->_iAttrFunc);
				break;
			}
			kIter.MoveNext();
		}
	}

	return iAttrScore;
}

int SC_ClientSkillInterface::GetVehicleCurrentLevel(int iPlyerLevel, int iVehicleLevel)
{
	return LC_Helper::GetCurrentVehicleLevel(iPlyerLevel, iVehicleLevel);
}

combat_score_type SC_ClientSkillInterface::GetVehicleFightingTotalScore(LC_PlayerBase *pkPlayer, int iLevel)
{
	return LC_Helper::GetVehicleFightingTotalScore(pkPlayer, iLevel);
}

combat_score_type SC_ClientSkillInterface::GetEquipTotalScore(LC_PlayerBase *pkPlayer)
{
	return ((LC_ClientGamePlayer*)pkPlayer)->GetCommanderEquipScore();
}

combat_score_type SC_ClientSkillInterface::GetStandardCombatScore(LC_PlayerBase *pkPlayer)
{
	int iLevel = 1;
	if(NULL != pkPlayer)
		iLevel = pkPlayer->GetLevel();

	const CF_LevelUp::DataEntry *pkLvUpInfo = SafeGetCSVFileDataEntryPtr<CF_LevelUp>(iLevel);
	if(NULL != pkLvUpInfo)
		return pkLvUpInfo->_lCombatScore;

	return 0;
}

combat_score_type SC_ClientSkillInterface::GetBuddyLevelUpCombatScoreBonus(int iLevel)
{
	const CF_LevelUp::DataEntry *pkLvUpInfo = SafeGetCSVFileDataEntryPtr<CF_LevelUp>(iLevel);
	if(NULL != pkLvUpInfo)
		return pkLvUpInfo->_lCombatScore;
	return 0;
}

combat_score_type	SC_ClientSkillInterface::GetBuddyOriginalCombatScore(int Cid)
{
	LC_ActorAttributeMap attrMap;
	CF_Buddy::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_Buddy>(Cid);
	attrMap.SetAttributeBase(ATT_MAX_HP,		pkData->_lMaxHP);
	attrMap.SetAttributeBase(ATT_MAX_ATK,		pkData->_lMaxPHA);
	attrMap.SetAttributeBase(ATT_DEF,			pkData->_lPHD);
	attrMap.SetAttributeBase(ATT_ATK_RATE,		pkData->_lATKRateAdd);
	attrMap.SetAttributeBase(ATT_DOG_RATE,		pkData->_lDogRATE);
	attrMap.SetAttributeBase(ATT_CIR_RATE,		pkData->_lCirRATE);
	attrMap.SetAttributeBase(ATT_CIR_DEF,		pkData->_lCirDefAdd);
	attrMap.SetAttributeBase(ATT_DMG_RATE,		pkData->_lDMG_RATE);
	attrMap.SetAttributeBase(ATT_DMG_DERATE,	pkData->_lDMG_DERATE);
	attrMap.SetAttributeBase(ATT_IGNORE_DEF,	pkData->_lIGNORE_DEF);
	attrMap.SetAttributeBase(ATT_DMG_REBOUND,	pkData->_lDMG_REBOUND);
	attrMap.SetAttributeBase(ATT_CRI_DMG_DERATE,pkData->_lCRI_DMG_DERATE);

	attrMap.RecomputeAll();
	return LC_Helper::CalcAttributeMapCombatScore(&attrMap, false);
}

combat_score_type SC_ClientSkillInterface::GetBuddyStarUpCombatScoreBonus(int Cid, int Starlevel, int iLevel)
{
	combat_score_type iScore = 0;
	const CF_LevelUp::DataEntry *pkLvUpInfo = SafeGetCSVFileDataEntryPtr<CF_LevelUp>(iLevel);
	if(NULL != pkLvUpInfo)
		iScore = pkLvUpInfo->_lCombatScore;

	const CF_BuddyUpgrade::DataEntry * pkStarUpInfo = SafeGetCSVFileDataEntryPtr<CF_BuddyUpgrade>(Cid * 100 + Starlevel);
	if(NULL != pkStarUpInfo)
		return iScore * (pkStarUpInfo->_lMaxHP - 1);
	return 0;
}

combat_score_type SC_ClientSkillInterface::GetItemCombatScore(int iItemId)
{
	LC_ActorAttributeMap attrMap;
	LC_Helper::GetItemBaseAttributeMap(iItemId, &attrMap);
	return LC_Helper::GetAttributeMapCombatScoreEx(&attrMap);
}

combat_score_type SC_ClientSkillInterface::GetSkillTotalScore(LC_PlayerBase *pkPlayer)
{
	return LC_Helper::GetSkillTotalScore(pkPlayer);
}

combat_score_type SC_ClientSkillInterface::GetSkillScore(int iSkillID)
{
	CF_SkillSpecialAttrib::DataEntry *pkSkillExecute = SafeGetCSVFileDataEntryPtr<CF_SkillSpecialAttrib>(iSkillID);
	if(NULL == pkSkillExecute)
		return 0;

	return pkSkillExecute->_lCombatScore;
}
combat_score_type SC_ClientSkillInterface::GetEquipReinforceScore(LC_BackPackEntry *pkBackPackEntry,bool bHasNext)
{
	return LC_Helper::GetEquipReinforceCombatScore(pkBackPackEntry,bHasNext);
}
combat_score_type SC_ClientSkillInterface::GetEquipScore(LC_BackPackEntry *pkBackPackEntry)
{
	return LC_Helper::GetEquipCombatScore(pkBackPackEntry);
}
combat_score_type SC_ClientSkillInterface::GetEquipScoreI3(int iItemID, int iEquipReinforceLevel, int iEquipReinforceStar, int iSkillID)
{
	return LC_Helper::GetEquipCombatScore(iItemID, iEquipReinforceLevel, iEquipReinforceStar, iSkillID);
}
combat_score_type SC_ClientSkillInterface::GetEquipScoreByItemID(int iItemID)
{
	return LC_Helper::GetEquipCombatScoreByItemID(iItemID);
}
combat_score_type SC_ClientSkillInterface::GetRuneEquipBaseScore(int iItemID)
{
	LC_ActorAttributeMap attrMap;
	LC_Helper::GetItemBaseAttributeMap(iItemID, &attrMap);
	combat_score_type Score= LC_Helper::GetAttributeMapCombatScoreEx(&attrMap);
	Score+=GetSubordinateTransformCombatScoreByItemId(iItemID);
	return Score;
}

combat_score_type SC_ClientSkillInterface::GetRuneEquipScore(const LC_BackPackEntry *pkItemEntry)
{
	return LC_Helper::GetRuneEquipScore(pkItemEntry);
}

combat_score_type SC_ClientSkillInterface::GetItemSuitTotalScore(int iSuitID, int iPara)
{
	return LC_Helper::GetItemSuitTotalScore(iSuitID, iPara);
}

StringType SC_ClientSkillInterface::GetEffectDesp(int EffectID, int iLevel)
{
	ItemEffectManage *pkItemEffectMgr = ItemEffectManage::GetSingletonPtr();
	
	if (pkItemEffectMgr != NULL)
	{
		ItemEffect *pkEffect = pkItemEffectMgr->FindEffect(EffectID);
		if(NULL != pkEffect)
		{
			return pkEffect->GetDesc(iLevel);
		}
	}
	return "";
}

StringType SC_ClientSkillInterface::GetItemEffectFuncInfo(int iEffectFuncID)
{
	StringStreamType sStream("");
	CF_ItemEffectFunc::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_ItemEffectFunc>(iEffectFuncID);
	if(pkEntry != NULL)
	{
		sStream << pkEntry->_lDesc << "#" << pkEntry->_TooltipIndex;
	}

	return sStream.str();
}

bool SC_ClientSkillInterface::_isItemAttributeValid(int iItemOpCode, int iAttribIdx)
{
	LC_ClientGamePlayer *pkPlayer  = GetPlayerPtr();
	if(!pkPlayer)
		return false;

	LC_ItemFactoryBase *pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	if(!pkItemFactory)
		return false;

	LC_ItemOperationDataEntry* pkOperation = pkItemFactory->RequestItemOperation(iItemOpCode, iAttribIdx);
	if (NULL != pkOperation)
	{
		//if ((pkOperation->_iCustom > 0 && pkPlayer->GetLevel() < pkOperation->_iCustom) && (pkOperation->_iVIPID > 0 && pkPlayer->GetVIPAsset().GetProfitValue(pkOperation->_iVIPID) <= 0))
		if ((pkOperation->_iCustom > 0 && pkPlayer->GetCommanderLevel() < pkOperation->_iCustom) && (pkOperation->_iVIPID > 0 && pkPlayer->GetVIPAsset().GetProfitValue(pkOperation->_iVIPID) <= 0))
		{
			return false;
		}
	}

	return true;
}

StringType SC_ClientSkillInterface::GetBaseEffectInfo(LC_BackPackEntry *pkEntry)
{
	StringStreamType sStream("");

	ItemEffectManage *pkItemEffectMgr = ItemEffectManage::GetSingletonPtr();

	//基本属性
	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	LC_ItemBase *pkItem = pkItemFactory->RequestItem(pkEntry->GetTypeID());
	if(NULL == pkItem)
	{
		return sStream.str();
	}

	for(int i = 0; i < LC_ItemBase::MAX_BASEEQUIP_ATTRIB_COUNT; i++)
	{
		LC_EquipBaseAttrib *pkBaseAttr = pkItem->GetBaseEquip(i);
		if(pkBaseAttr != NULL)
		{
			ItemEffect *pkEffect = pkItemEffectMgr->FindEffect(pkBaseAttr->GetEffectID());
			if(NULL != pkEffect)
			{
				StringType sDesc = pkEffect->GetDesc(pkBaseAttr->GetEffectLevel());
				sStream << sDesc << "|";
			}
		}
	}

	return sStream.str();
}

StringType SC_ClientSkillInterface::GetOpenBagData(int iTargetIndex)
{
	LC_ClientGamePlayer* pkPlayer = GetPlayerPtr();
	if(NULL == pkPlayer)
		return "";

	int iCapacity = pkPlayer->GetPackAsset()->GetPackCapacity(PACK_TYPE_BASIC);
	int iCurIndex = pkPlayer->GetPackAsset()->GetPackSize(PACK_TYPE_BASIC);

	if (iCurIndex > iTargetIndex || iTargetIndex > iCapacity)
		return "";

	int iCost = 0;
	int iTime = 0;
	std::map<int, int> kItems;
	
	CF_OpenBag::DataEntry* pkData = NULL;
	for (int i = iCurIndex; i <= iTargetIndex; i++)
	{
		pkData = SafeGetCSVFileDataEntryPtr<CF_OpenBag>(i);
		if(NULL == pkData)
			continue;

		iTime += pkData->_Time;

		if (pkData->_ShopID > 0)
		{
			CF_ShopList::DataEntry * sentry = SafeGetCSVFileDataEntryPtr<CF_ShopList>(pkData->_ShopID);
			if(NULL != sentry)
				iCost = LC_Helper::GetComMapValue(sentry->_ActualPriceList, CT_UNBIND_YUANBAO) * sentry->_iCount;
		}

		int iItemID = pkData->_ItemID;
		int iItemCount = pkData->_ItemCount;
		if(iItemID > 0 && iItemCount > 0)
		{
			std::map<int ,int>::const_iterator iter = kItems.find(pkData->_ItemID);
			if(iter == kItems.end())
			{
				kItems[iItemID] = 0;
				
			}

			kItems[iItemID] += iItemCount;
		}
	}

	StringStreamType sStream("");
	sStream << iTargetIndex - iCurIndex + 1 << "#";
	sStream << iTime << "#";
	sStream << iCost << "#";
	for(std::map<int ,int>::iterator i=kItems.begin(); i!=kItems.end(); ++i)
	{
		sStream << (*i).first << ":" << (*i).second << "|";
	}

	return sStream.str();
}

int SC_ClientSkillInterface::GetExternWarehouseData(int iCurrentIndex, int iTargetIndex)
{
	LC_ClientGamePlayer* pkPlayer = GetPlayerPtr();
	if(NULL == pkPlayer)
		return 0;

	int iCost = 0;

	CF_WarehouseCost::DataEntry* pkData = NULL;
	for (int i = iCurrentIndex; i <= iTargetIndex; i++)
	{
		pkData = SafeGetCSVFileDataEntryPtr<CF_WarehouseCost>(i);
		if(NULL == pkData)
			continue;

		iCost += pkData->_lCost;
	}

	return iCost;
}

StringType SC_ClientSkillInterface::GetShopItemInfo(int iItemID, int iShopType)
{
	StringStreamType sStream("");
	CF_ShopList::DataEntryMapExternIterator kIter = CF_ShopList::GetSingletonPtr()->GetIterator();
	for( ; kIter.HasMoreElements(); kIter.MoveNext() )
	{
		CF_ShopList::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		if((iShopType == -1 ||pkEntry->_iShopType == iShopType) && pkEntry->_iItemID == iItemID && pkEntry->_iCount > 0)
		{
			sStream << pkEntry->_iID << "#" << LC_Helper::GetComMapValue(pkEntry->_ActualPriceList, CT_UNBIND_YUANBAO) << "#" << LC_Helper::GetComMapValue(pkEntry->_ActualPriceList, CT_BIND_YUANBAO);
			break;
		}
	}

	return sStream.str();
}
int SC_ClientSkillInterface::GetItemUnBindPrice(int iItemID)
{
	CF_ItemList::DataEntry *pkItemInfo = SafeGetCSVFileDataEntryPtr<CF_ItemList>(iItemID);
	if ( pkItemInfo != NULL)
		return pkItemInfo->_lUnBindPrice;
	
	return 0;
}

StringType SC_ClientSkillInterface::GetPowerUpInfo(int iLevel)
{
	StringStreamType sStream("");
	CF_PowerUp::DataEntryMapExternIterator kIter = CF_PowerUp::GetSingletonPtr()->GetIterator();
	for( ; kIter.HasMoreElements(); kIter.MoveNext() )
	{
		const CF_PowerUp::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		sStream << pkEntry->_iID << "#" << pkEntry->_level << "#" << pkEntry->des << "#" << _getCombatScoreStandard(iLevel, pkEntry->_iID) << "#";

		TStringVector kVector = UT_BaseStringUtility::SplitString(pkEntry->_lFuncIDString, ';');
		for(int i=0; i<kVector.size(); ++i)
		{
			int iFuncID = atoi(kVector[i].c_str());
			const CF_PowerUpFunc::DataEntry *pkFunc = SafeGetCSVFileDataEntryPtr<CF_PowerUpFunc>(iFuncID);
			if(NULL != pkFunc)
			{
				sStream << pkFunc->_iID << "@" << pkFunc->_kIconPath << "@" << pkFunc->_kContent << "@" << Bool2Long(pkFunc->_bTrace) << "@";
				sStream << pkFunc->_lOperation << "|";
			}
			else
			{
				sStream << "NULL";
			}
		}

		sStream << "\n";
	}

	return sStream.str();
}

combat_score_type SC_ClientSkillInterface::_getCombatScoreStandard(int iLevel, int iIndex)
{
	const CF_LevelUp::DataEntry *pkLvUpInfo = SafeGetCSVFileDataEntryPtr<CF_LevelUp>(iLevel);
	if(NULL != pkLvUpInfo)
	{
		switch(iIndex)
		{
		case 1:
			{
				int iCount = (int) CF_LevelUp::GetSingletonPtr()->GetEntryCount();
				const CF_LevelUp::DataEntry *pkLvUpInfoMax = SafeGetCSVFileDataEntryPtr<CF_LevelUp>(iCount);
				if(NULL != pkLvUpInfoMax)
					return (combat_score_type)pkLvUpInfoMax->_lCombatScore;
				else
					return (combat_score_type)pkLvUpInfo->_lCombatScore;
			}
			return (combat_score_type)pkLvUpInfo->_lCombatScore;
		case 2:
			return (combat_score_type)pkLvUpInfo->_lEquipScoreStandard;
		case 3:
			return (combat_score_type)pkLvUpInfo->_lSkillScoreStandard;
		case 4:
			return (combat_score_type)pkLvUpInfo->_lVehicleScoreStandard;
		case 5:
			return (combat_score_type)pkLvUpInfo->_lArrowScoreStandard;
		case 6:
			return (combat_score_type)pkLvUpInfo->_lDevaEyeScoreStandard;
		case 7:
			return (combat_score_type)pkLvUpInfo->_lVehicleFightingScoreStandard;
		case 8:
			return (combat_score_type)pkLvUpInfo->_lMentalScoreStandard;
		}
	}

	return 0;
}

bool SC_ClientSkillInterface::IsEquipMostSuitable(int iItemID)
{
	LC_ClientGamePlayer* pkPlayer = GetPlayerPtr();
	if(NULL == pkPlayer)
		return false;
	
	CF_ItemList::DataEntry *pkItemInfo = SafeGetCSVFileDataEntryPtr<CF_ItemList>(iItemID);
	if(NULL == pkItemInfo)
		return false;

	int iPlayerLevel = pkPlayer->GetLevel();
	int iCurLevel = 0;
	CF_ItemList::DataEntryMapExternIterator kIter = CF_ItemList::GetSingletonPtr()->GetIterator();
	for( ; kIter.HasMoreElements(); kIter.MoveNext() )
	{
		const CF_ItemList::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		if(pkItemInfo->_lCategory2 == pkEntry->_lCategory2)
		{
			if(pkEntry->_lLevel <= iPlayerLevel && iCurLevel < pkEntry->_lLevel)
			{
				iCurLevel = pkEntry->_lLevel;
			}
		}
	}

	return pkItemInfo->_lLevel == iCurLevel;
}

StringType SC_ClientSkillInterface::GetCimeliaItemInfo(int iLevel, int iCimeliaType)
{
	StringStreamType sStream("");

	const LC_SubordinateDataEntry *pkEntry = SafeGetSubDataEntry(iCimeliaType, iLevel);
	if(NULL != pkEntry)
	{
		sStream << pkEntry->_iItemTypeID << "#" << pkEntry->_iCount;
	}

	return sStream.str();
}

StringType SC_ClientSkillInterface::GetCimeliaSkillInfo(int iCimeliaType, int iLevel)
{
	StringStreamType sStream("");
	const LC_SubordinateDataEntry *pkEntry = SafeGetSubDataEntry(iCimeliaType, iLevel);
	if(NULL != pkEntry)
	{
		sStream << pkEntry->_kSkillIDs.size() << "|";
		Utility::UT_SIMDataList::const_iterator cit = pkEntry->_kSkillIDs.begin();
		for (; cit!=pkEntry->_kSkillIDs.end(); ++cit)
		{
			sStream << cit->ID() << "#";
		}
		
	}

	int iNxtLevel = iLevel+1;
	LC_SubordinateDataEntry *pkNxtEntry = SafeGetSubDataEntry(iCimeliaType,iNxtLevel);
	if(pkNxtEntry == NULL)
	{
		sStream << sStream << "@0";
	}
	else
	{
		while(true)
		{
			size_t iSize = pkEntry->_kSkillIDs.size();
			if((iCimeliaType == LC_SUB_TP_VEHICLE && iSize > 1) || iSize > 0)
			{
				break;
			}

			pkNxtEntry = SafeGetSubDataEntry(iCimeliaType, ++iNxtLevel);
			if(pkNxtEntry == NULL)
			{
				iNxtLevel = 0;
				break;
			}
		}
		
	}

	sStream << '@' << iNxtLevel;
	return sStream.str();
}

int SC_ClientSkillInterface::GetCimeliaDisplaySkillID(int iCimeliaType, int iLevel)
{
	const LC_SubordinateDataEntry *pkEntry = SafeGetSubDataEntry(iCimeliaType, iLevel);
	if(NULL == pkEntry)
		return 0;

	return pkEntry->_iDisplaySkillID;
}

int SC_ClientSkillInterface::GetCurrentCanLearnSkillCount(LC_PlayerBase *pkPlayer)
{
	if(NULL == pkPlayer)
		return 0;

	LC_SkillAsset *pkSkillAsset = pkPlayer->GetSkillAsset();
	if(NULL == pkSkillAsset)
		return 0;

	CF_SkillExecuteList *pkCSVSkillExecute = CF_SkillExecuteList::GetSingletonPtr();
	CF_SkillExecuteList::DataEntryMapExternIterator iter = pkCSVSkillExecute->GetIterator();

	int iCurLevel = pkPlayer->GetLevel();
	int iCount = 0;

	for( ; iter.HasMoreElements(); iter.MoveNext())
	{
		CF_SkillExecuteList::DataEntry *pkData = iter.PeekNextValuePtr();
		int lWeaponType = pkData->_lWeaponType;
		//取第一级的技能ID
		if((SKILL_TYPE_CONTROL == pkData->_lCategory2 || SKILL_TYPE_PASSIVE == pkData->_lCategory2) && \
			pkData->_lCategory1 != SKILL_TYPE_SUB_ACTOR_GREATKUNGFU)
		{
			int lSkillID = pkData->_iID;
			LC_SkillAssetEntry *pkSkillEntry = pkSkillAsset->FindLearnedSkill(lSkillID);
			if(NULL == pkSkillEntry)
			{
				CF_ItemList::DataEntry *pkSkillBookInfo = SafeGetCSVFileDataEntryPtr<CF_ItemList>(pkData->_lSkillBookID);
				if(NULL != pkSkillBookInfo)
				{
					CF_ItemUseLimitFunc::DataEntry *pkLmtInfo = SafeGetCSVFileDataEntryPtr<CF_ItemUseLimitFunc>(pkSkillBookInfo->_lUseLimitFuncID);
					if(NULL != pkLmtInfo)
					{
						if(iCurLevel >= pkLmtInfo->_lMinLevelLimit)
							iCount++;
					}
				}
			}
		}
	}

	return iCount;
}

int SC_ClientSkillInterface::PlayCameraTrackAnim(int trackIdx)
{
	CA_CameraManager::GetSingleton().SetCameraType(CA_CameraManager::CT_TRACK_ANIM);

	bool b = CA_CameraManager::GetSingleton().PlayTrackAnim(trackIdx);

	return int(b);
}

int SC_ClientSkillInterface::PlayCameraMaxTrackAnim(int lCharType)
{
	CA_CameraManager::GetSingleton().SetCameraType(CA_CameraManager::CT_TRACK_ANIM);

	bool b = CA_CameraManager::GetSingleton().PlayMaxAnim(lCharType,0);

	return int(b);
}

void SC_ClientSkillInterface::StopCameraTrackAnim()
{
	CA_CameraManager::GetSingleton().StopTrackAnim();
}

void SC_ClientSkillInterface::RestoreFollowCamera()
{
	CA_CameraManager::GetSingleton().SetCameraType(CA_CameraManager::CT_DRACONIS);
}

void SC_ClientSkillInterface::MoveCameraToNpcCharType(int lNpcCharTypeID, float go, float back, float stop, bool follow)
{
	object_id_type npcID = GetLogicIDByCharID(lNpcCharTypeID, 0);
	if (npcID > 0)
	{
		if (go <= 0)		go = 0.5f;
		if (back <= 0)		back = 0.5f;
		if (stop <=0)		stop = FLT_MAX;		//无限停留
		CA_CameraManager::GetSingleton().MoveToTarget(npcID, go, back, stop, follow);
	}
}

void SC_ClientSkillInterface::MoveCameraToTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, float go, float back, float stop, bool follow)
{
	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller, iTargetIndex, kTargetsList);
	if (kTargetsList.size() < 1)
		return;

	LC_ActorBase *pkTarget = kTargetsList.front();
// 	if (!IS_NPC_ID(pkTarget->GetID()))
// 		return;

	if (go <= 0)		go = 0.5f;
	if (back <= 0)		back = 0.5f;
	if (stop <=0)		stop = FLT_MAX;		//无限停留
	CA_CameraManager::GetSingleton().MoveToTarget(pkTarget->GetID(), go, back, stop, follow);
}

void SC_ClientSkillInterface::MoveCameraToMC(void)
{
	CA_CameraManager::GetSingleton().MoveToMC();
}

void SC_ClientSkillInterface::MoveCameraToNoUnLock()
{
	CA_CameraManager::GetSingleton().MoveCameraToNoUnLock();
}

StringType SC_ClientSkillInterface::GetAnimModelPath()
{
	return Camera::CA_CameraAnimationManager::GetSingletonPtr()->GetAnimationKeyString();
}

void SC_ClientSkillInterface::SceeenFadeIn(float keepTime, float rampTime)
{
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();

	pkUIManager->ScreenFadeIn(0xff000000, 0x00000000, keepTime, rampTime);
}

void SC_ClientSkillInterface::SceeenFadeOut(float rampTime, float keepTime)
{
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();

	pkUIManager->ScreenFadeOut(0x00000000, 0xff000000, rampTime, keepTime);
}


void SC_ClientSkillInterface::SetModelVisibility( GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, float bAlpha , bool bFlash)
{		 
	LC_ActorPtrList kTargetsList;
	_findTarget(pkScriptCaller,iTargetIndex, kTargetsList);
	LC_ActorPtrListIter IterBe = kTargetsList.begin();
	LC_ActorPtrListIter IterEd = kTargetsList.end();
	LC_ActorBase* pkTarget = NULL;
	object_id_type lTargetID = 0;
	for(; IterBe != IterEd; ++IterBe)
	{
		pkTarget = (*IterBe);
		lTargetID = pkTarget->GetID();

		pkTarget->SetFlashAlpha(bFlash);
		GD_ClientActorModel* visiData = IM_Manager::GetSingleton().GetActor(lTargetID);
		if ( visiData )
		{
			visiData->SetAlpha(bAlpha);
		}
	}
}

void SC_ClientSkillInterface::Crash(void)
{
	T_ASSERT_CRITICAL_S(0);
}

void SC_ClientSkillInterface::EyeExecuteSkill(GameLogic::LC_ActorBase* pkCaller, int lSkillID, object_id_type lTargetID)
{
	if(pkCaller && IS_PLAYER_CLASS_ID(pkCaller->GetID()))
	{
		LC_ClientGamePlayer* pkGamePlayer = (CLIENT_GET_LOGIC_MANAGER())->GetOperationPlayer();
		LC_ClientNPC *pkEye = NULL;
		if(pkGamePlayer->GetID() != pkCaller->GetID() )
		{
			LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
			LC_ClientNetPlayer* netplayer = (LC_ClientNetPlayer*)pkCaller;
			pkEye = (LC_ClientNPC *)pkNPCManager->FindNPC(netplayer->GetEyeCharTypeID());
			
			SK_Factory* pkSkillFactory = SK_Factory::GetSingletonPtr();
			SK_SkillExecute* pkSkill = pkSkillFactory->RequestSkillExcute(lSkillID);
			if (pkSkill && pkEye)
			{
				pkEye->SetTargetLocation(netplayer->GetCurrentLocation());
				pkEye->SetCurrentLocation(netplayer->GetCurrentLocation());
				pkEye->SetVisibleLocation(netplayer->GetCurrentLocation());
				pkEye->SetLockedTargetID(lTargetID);
				LC_ActorBase* acotr = _findActor(lTargetID);
				if( acotr)
					pkEye->SetLockedLocation(acotr->GetCurrentLocation());
				pkEye->ExecuteSkill(GET_PROCESS_TIME, pkSkill, 0);		
			}
		}
		else
		{
			LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
			LC_ClientNPC *pkEye = (LC_ClientNPC *)pkNPCManager->FindNPC(pkGamePlayer->GetEyeCharTypeID());
		
			SK_Factory* pkSkillFactory = SK_Factory::GetSingletonPtr();
			SK_SkillExecute* pkSkill = pkSkillFactory->RequestSkillExcute(lSkillID);
			if (pkSkill && pkEye)
			{
				pkEye->SetTargetLocation(pkCaller->GetCurrentLocation());
				pkEye->SetCurrentLocation(pkCaller->GetCurrentLocation());
				pkEye->SetVisibleLocation(pkCaller->GetCurrentLocation());
				pkEye->SetLockedTargetID(lTargetID);
				LC_ActorBase* acotr = _findActor(lTargetID);
				if( acotr)
					pkEye->SetLockedLocation(acotr->GetCurrentLocation());
				pkEye->ExecuteSkill(GET_PROCESS_TIME, pkSkill, 0);		
			}
		}
	}
}

////////////////////conversation begin//////////////////////////////////////////////////////

int	SC_ClientSkillInterface::GetNPCConversationSequenceCount(int lConversationID) const
{
	LC_ConversationManager* pkConversationMgr = LC_ConversationManager::GetSingletonPtr();
	if(!pkConversationMgr)
		return 0;
	return pkConversationMgr->GetSequenceCount(lConversationID);
}
int SC_ClientSkillInterface::GetNPCConversationCharID(int lConversationID, int iCurSeq) const
{
	LC_ConversationManager* pkConversationMgr = LC_ConversationManager::GetSingletonPtr();
	if(!pkConversationMgr)
		return 0;
	return pkConversationMgr->GetCharID(lConversationID, iCurSeq);
}
const StringType& SC_ClientSkillInterface::GetNPCConversationContent(int lConversationID, int iCurSeq) const
{
	LC_ConversationManager* pkConversationMgr = LC_ConversationManager::GetSingletonPtr();
	T_ASSERT_CRITICAL_S(pkConversationMgr);
	return pkConversationMgr->GetContent(lConversationID, iCurSeq);
}
const StringType& SC_ClientSkillInterface::GetNPCConversationName(int lConversationID) const
{
	LC_ConversationManager* pkConversationMgr = LC_ConversationManager::GetSingletonPtr();
	T_ASSERT_CRITICAL_S(pkConversationMgr);
	return pkConversationMgr->GetName(lConversationID);
}
int	SC_ClientSkillInterface::GetNPCConversationSequenceCount(int lConversationID)
{
	LC_ConversationManager* pkConversationMgr = LC_ConversationManager::GetSingletonPtr();
	if(!pkConversationMgr)
		return 0;
	return pkConversationMgr->GetSequenceCount(lConversationID);
}
int SC_ClientSkillInterface::GetNPCConversationCharID(int lConversationID, int iCurSeq)
{
	LC_ConversationManager* pkConversationMgr = LC_ConversationManager::GetSingletonPtr();
	if(!pkConversationMgr)
		return 0;
	return pkConversationMgr->GetCharID(lConversationID, iCurSeq);
}
const StringType& SC_ClientSkillInterface::GetNPCConversationContent(int lConversationID, int iCurSeq)
{
	LC_ConversationManager* pkConversationMgr = LC_ConversationManager::GetSingletonPtr();
	T_ASSERT_CRITICAL_S(pkConversationMgr);
	return pkConversationMgr->GetContent(lConversationID, iCurSeq);
}
const StringType& SC_ClientSkillInterface::GetNPCConversationName(int lConversationID)
{
	LC_ConversationManager* pkConversationMgr = LC_ConversationManager::GetSingletonPtr();
	T_ASSERT_CRITICAL_S(pkConversationMgr);
	return pkConversationMgr->GetName(lConversationID);
}

int SC_ClientSkillInterface::GetConversationTypeByID(int lConversationID)
{
	LC_ConversationManager* pkConversationMgr = LC_ConversationManager::GetSingletonPtr();
	if(!pkConversationMgr)
		return 0;
	const CSVFile::CF_Conversation::DataEntry* pkData = pkConversationMgr->GetDataEntry(lConversationID, 1);
	if(!pkData)
		return 0;
	return pkData->_iType;
}
float SC_ClientSkillInterface::GetConversationTime(int lConversationID, int iCurSeq)
{
	LC_ConversationManager* pkConversationMgr = LC_ConversationManager::GetSingletonPtr();
	if(!pkConversationMgr)
		return 0;
	const CSVFile::CF_Conversation::DataEntry* pkData = pkConversationMgr->GetDataEntry(lConversationID, iCurSeq);
	if(!pkData)
		return 0;
	return pkData->_fTime;
}

StringType SC_ClientSkillInterface::GetConversationSoundKey(int lConversationID, int iCurSeq)
{
	LC_ConversationManager* pkConversationMgr = LC_ConversationManager::GetSingletonPtr();
	if(!pkConversationMgr)
		return "";

	const CSVFile::CF_Conversation::DataEntry* pkData = pkConversationMgr->GetDataEntry(lConversationID, iCurSeq);
	if(!pkData)
		return "";

	return pkData->_SoundKey;
}

guild_id_type SC_ClientSkillInterface::GetGlobleWarGuildID()
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)LC_ClientLogicManager::GetSingletonPtr();
	if(NULL == pkLogicManger)
		return 0;
	LC_ClientGamePlayer* pkPlayer = pkLogicManger->GetOperationPlayer();
	if(NULL == pkPlayer)
		return 0;
	int groupId = pkPlayer->GetGuildGroup();
	LC_WorldGuildWarAsset* pkWorldGuildAsset = pkLogicManger->GetGuildWarAssert();
	if ( NULL == pkWorldGuildAsset )
		return 0;

	Protocol::PS_CrossRealm_PowerGuildInfo* pkGlobalGuildInfo = pkWorldGuildAsset->GetGlobalPowerGuildInfo(groupId);
	if (NULL == pkGlobalGuildInfo)
		return 0;
	return pkGlobalGuildInfo->m_GuildId;
}

bool SC_ClientSkillInterface::IsDomainWarGuild( guild_id_type guildID )
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)LC_ClientLogicManager::GetSingletonPtr();
	LC_WorldGuildWarAsset* pkWorldGuildAsset = pkLogicManger->GetGuildWarAssert();
	if ( NULL == pkWorldGuildAsset )
		return false;

	const DomainPowerMap* pDomainGuilds = pkWorldGuildAsset->GetCurDomainPowerGuildInfo();
	if(!pDomainGuilds)
	{
		return false;
	}
	for(  DomainPowerMap::const_iterator p = pDomainGuilds->begin(); p != pDomainGuilds->end(); ++p)
	{
		if(p->second.m_GuildId == guildID)
		{
			return true;
		}
	}
	return false;
}

int SC_ClientSkillInterface::GetSystemTitlePeriod(int iTitleId)
{
	CF_SystemTitleList::DataEntry* pkEntry=SafeGetCSVFileDataEntryPtr<CF_SystemTitleList>(iTitleId);
	if (pkEntry)
	{
		return pkEntry->_iPeriod;
	}
	return 0;
}

StringType	SC_ClientSkillInterface::GetGuildClashBossName(int iIndex)
{
	StringType sName = "";
	CF_GuildClash::DataEntry *entry = SafeGetCSVFileDataEntryPtr<CF_GuildClash>(iIndex);
	if (NULL != entry)
	{
		sName = entry->_kCharName;
	}
	return sName; 
}
int	SC_ClientSkillInterface::GetGuildClashBossLocationID(int iIndex)
{
	int id = 0;
	CF_GuildClash::DataEntry *entry = SafeGetCSVFileDataEntryPtr<CF_GuildClash>(iIndex);
	if (NULL != entry)
	{
		id = entry->_kLocation;
	}
	return id; 
}

int SC_ClientSkillInterface::GetNPCLevelByCharType(int lCharType)
{
	CF_CharType::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_CharType>(lCharType);
	if (NULL == pkData)
		return 0;

	return pkData->_lLevel;
}

int SC_ClientSkillInterface::GetRaidBossPublicLinkID(int iChatType)
{
	CF_RegularBoss::DataEntry *pkEntry  = SafeGetCSVFileDataEntryPtr<CF_RegularBoss>(iChatType);
	if (pkEntry)
		return pkEntry->_kLocation;

	return 0;
}

int SC_ClientSkillInterface::GetGuildBossLinkID(int MapID)
{
	CF_GuildTower::DataEntry *pkEntry  = SafeGetCSVFileDataEntryPtr<CF_GuildTower>(MapID);
	if (pkEntry)
		return pkEntry->_iLocation;

	return 0;
}

int SC_ClientSkillInterface::GetSubordinateAttriLevel(int openID, int stage, bool bNext)
{
	CF_Subordinate::DataEntry* pkEntry = SafeGetCSVFileDataEntryPtr<CF_Subordinate>(openID);
	int maxAttriLevel = 0;
	int maxCir = openID + 10;
	while (pkEntry != NULL && pkEntry->_iAttrMaxLevel > 0 && pkEntry->_iStage < stage && openID <= maxCir)
	{
		maxAttriLevel +=pkEntry->_iAttrMaxLevel;
		openID += 1;
		pkEntry = SafeGetCSVFileDataEntryPtr<CF_Subordinate>(openID);
	}

	if (bNext)
	{
		maxAttriLevel +=pkEntry->_iAttrMaxLevel;
	}

	return maxAttriLevel;

};


void SC_ClientSkillInterface::RequestUseGuildTransportItem( int index )
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return;
	if(!pkGamePlayer->IsExistGuild())
	{
		SendLogicInfoMessage(RE_GUILD_TRANSPORT_CANNOT_USE);
		return;
	}
	bool bCreateMsg = false;
	MG_Req_GuildTransport_Create msg;
	msg.m_Index = index;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GUILDTRANSPORT_CREATE, &msg);

	if(bCreateMsg)
		pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

void SC_ClientSkillInterface::RequestGuildWorship( int worshipType, unique_id_type targetID )
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return;

	 MG_Req_Add_GuildGood kMsg;
	 kMsg.m_GoodType = worshipType;
	 kMsg.m_TargetCharID = targetID;

	 bool bCreateMsg = false;
	 MG_PACKET_ENCODE_HANDLER hMsgHandler;
	 bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_ADD_GUILDGOOD, &kMsg);

	 if(bCreateMsg)
		 pkGamePlayer->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	
}

int SC_ClientSkillInterface::GetItemGuildMoneyValue(int itemId)
{
	CF_ItemList* pkCSVItemList = CF_ItemList::GetSingletonPtr();
	if(pkCSVItemList == NULL)
	{
		return 0;
	}
	CF_ItemList::DataEntry* pkCfgData = pkCSVItemList->GetEntryPtr(itemId);
	if (pkCfgData == NULL)
	{
		return 0;
	}
	return pkCfgData->_GuildCashV;
}

int SC_ClientSkillInterface::GetGuildWorshipReward( int worshipType )
{
	CF_GuildGood::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_GuildGood>(worshipType);
	if (pkData)
	{
		return pkData->_GuildMoneyReward;
	}
	return 0;
}

int SC_ClientSkillInterface::GetGuildWorshipCost( int worshipType )
{
	CF_GuildGood::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_GuildGood>(worshipType);
	if (pkData)
	{
		return pkData->_CostMoney + pkData->_CostYB;
	}
	return 0;
}

int SC_ClientSkillInterface::GetGuildWorshipVipLevel( int worshipType )
{
	CF_GuildGood::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_GuildGood>(worshipType);
	if (pkData)
	{
		//return pkData->_VIPLevel;
		return 0;
	}
	return 0;
}

void SC_ClientSkillInterface::ShowMessage( int messagetype, const StringType& para1, const StringType& para2 )
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return;

	//晓杰说可以注释掉
	/*if(messagetype == RE_ITEM_BUY_SHOP_CITY_SUCESS)
	{	
		StringType	kItemName		= "";
		CF_ItemList* pkCSVItemList = CF_ItemList::GetSingletonPtr();
		int lItemID = atoi(para1.c_str());
		CF_ItemList::DataEntry* pkItemData = pkCSVItemList->GetEntryPtr(lItemID);
		if(pkItemData)
		{
			char numstr[100];
			ltoa(lItemID, numstr, 10);
			kItemName = kItemName + "[@" + numstr +"]";
			kItemName = pkItemData->_kName;
		}
		SendLogicInfoMessage(messagetype, kItemName, para2);
		return;
	}
	else if(messagetype == RE_VIGOR_BUY_REQUEST)
	{
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
		int32_t iCurCount = kVipAsset.GetProfitStatValue(pkProfDataEntry->GetID());
		int32_t iCountLimit = pkProfDataEntry->GetValue(pkGamePlayer->GetVIPLevel(), pkGamePlayer->GetVIPAsset().IsFlagSet(LC_VIP_FG_FAKE));

		LC_ShopIDMap kShopIDMap;
		const LC_VIPProfitDefine& kCommonDefine = pkProfDataEntry->GetCommonDefine();
		Utility::UT_SIMDataList::const_iterator cit = kCommonDefine.m_kExtras.begin();
		kShopIDMap[cit->ID()] += (int32_t)iCurCount;

		LC_CommonShopManager* pkCmnShopMgr = LC_ClientShopManager::GetSingletonPtr();
		LC_ShopDataEntry* pkEntry = pkCmnShopMgr->GetShopDataEntry(cit->ID());
		
		int32_t Price = pkEntry->m_kCashMap[CT_UNBIND_YUANBAO] * (iCurCount + 1);
		int32_t MyYuanBao = Utility::String2Int(pkGamePlayer->GetUnBindYuanBaoString());
		StringType kFirst = Utility::Int2String(Price);
		if (Price > MyYuanBao)
		{
			kFirst = para1 + "<color val = 0xffDE1F00>" + kFirst + "<color>";
		}
		else
		{
			kFirst = para1 + "<color val = 0xff00ff00>" + kFirst + "<color>";
		}
		StringType kSecond = Utility::Int2String(iCountLimit - iCurCount) + "/" + Utility::Int2String(iCountLimit);
		if(iCountLimit > iCurCount)
		{
			kSecond = para2 + "<color val = 0xff00ff00>" + kSecond + "<color>";
		}
		else
		{
			kSecond = para2 + "<color val = 0xffDE1F00>" + kSecond + "<color>";
		}
		SendMessageDialog(messagetype,  kFirst, kSecond, messagetype, 0, 0, 0);
		return;
	}*/
	else if(messagetype == RE_PLAYER_ADD_ITEM || messagetype == RE_EQUIP_INLAY_SUCCESS)
	{
		StringType	kItemName = "";
		int lItemID = atoi(para1.c_str());
		int iCount = atoi(para2.c_str()); 
		CF_ItemList* pkCSVItemList = CF_ItemList::GetSingletonPtr();
		CF_ItemList::DataEntry* pkItemData = pkCSVItemList->GetEntryPtr(lItemID);
		if (NULL == pkItemData)
		{
			return;
		}
		kItemName = LC_Helper::GetGradeName(pkItemData->_kName, pkItemData->_lGrade);
		SendLogicInfoMessage(messagetype, kItemName, iCount);
		return;
	}

	SendLogicInfoMessage(messagetype, para1, para2);
}


StringType SC_ClientSkillInterface::GetErrorString(int iErrorId)
{
	StringType sStr("");
	CF_SystemError* pkCSVSystemError = CF_SystemError::GetSingletonPtr();
	if(!pkCSVSystemError)
		return sStr;

	CF_SystemError::DataEntry* pkData = pkCSVSystemError->GetEntryPtr(iErrorId);
	if(!pkData)
		return sStr;
	return pkData->_sErrorMsg;
}

StringType SC_ClientSkillInterface::GetActTimeShopInfoByID(int ID)
{
	LC_ClientGamePlayer* pkGamePlayer = (CLIENT_GET_LOGIC_MANAGER())->GetOperationPlayer();
	if(!pkGamePlayer)
		return "";
	StringStreamType sStream("");
	CF_ACTShopTimeLimited::DataEntry *pkShopData = SafeGetCSVFileDataEntryPtr<CF_ACTShopTimeLimited>(ID);
	if (pkShopData)
	{
		sStream << pkShopData->_iID << "#";
		sStream << pkShopData->_name << "#";
		const Utility::LC_CashMap* pkCashMap = &pkShopData->_kCurrentPrices;
		for (Utility::LC_ComMap::const_iterator it = pkCashMap->begin(); it != pkCashMap->end(); ++it)
		{
			sStream << it->first << ":" << it->second << ";";
		}
		sStream << "#";
		const Utility::UT_SIMDataList* pkItems = &pkShopData->_kItems;
		for (Utility::UT_SIMDataList::const_iterator it = pkItems->begin(); it != pkItems->end(); ++it)
		{
			sStream << it->ID() << ":" << it->ItemCount() << ";";
		}
		sStream << "#";
		sStream << pkShopData->_kIcon1 << "#";
		sStream << pkShopData->_kIcon2 << "#";
		sStream << pkShopData->_sDescSign << "#";
		sStream << pkShopData->_iSchedule << "#";
		sStream << pkShopData->_iType << "#";
		sStream << pkShopData->_iKey << "#";
	}
	return sStream.str();
}

int SC_ClientSkillInterface::GetDistanceFromNpc(LC_ActorBase *pkNpc)
{
	if (!pkNpc)
		return -1;
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return -1;
	int iDistance = UT_MathBase::LineLengthXYInt(pkGamePlayer->GetVisibleLocation(),pkNpc->GetVisibleLocation());
	return iDistance;
}

bool SC_ClientSkillInterface::IsSpecialAffixItemID(int itemID)
{
	return LC_Helper::IsSpecialAffixItemID(itemID);
}

int SC_ClientSkillInterface::IsRunInMiniLauncher(void)
{
	return (int)isRunInMiniLauncher();
}

void SC_ClientSkillInterface::CommitPatchProgress(int iEvent, int iCode)
{
	commitPatchProgress(iEvent, iCode);
};

bool SC_ClientSkillInterface::IsMapCanGuildTransport(int iMapResID)
{
	CF_WorldMapList::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(iMapResID);
	if(NULL == pkEntry)
		return false;

	return pkEntry->_CanGuildTransport == 1;
}

int SC_ClientSkillInterface::GetCurrentNPCTalkTaskFunction()
{
	int searchIndex = -1;
	LC_ClientGamePlayer* pkGamePlayer = (CLIENT_GET_LOGIC_MANAGER())->GetOperationPlayer();
	int npcID = pkGamePlayer->GetLockedNpcID();
	LC_ClientNPCManager* const pkNPCManager = CLIENT_GET_NPC_MANAGER();
	LC_ClientNPC* const pkNPC = (LC_ClientNPC*)pkNPCManager->FindNPC(npcID);
	if (pkNPC)
	{
		int iCount = pkNPC->GetNPCFuncDataCount();
		for (int i = 0 ; i < iCount; i++)
		{
			if (pkNPC->GetNPCFuncDataType(i) == NFT_TASK)
			{
				searchIndex = i;
				break;
			}
		}
	}
	return searchIndex;
}

bool SC_ClientSkillInterface::_sortFunc(const SortItemEffect &v1, const SortItemEffect &v2)
{
	/*return ((v1.lSortID > 0 && v2.lSortID > 0 && v1.lSortID < v2.lSortID)
		|| (v2.lSortID == 0)
		|| (v1.lSortID == v2.lSortID && v1.kEquipAttrib.m_Level < v2.kEquipAttrib.m_Level));*/
	return v2.lSortID == 0;
}

bool SC_ClientSkillInterface::_equipAttribSortFunc(const SortItemEffect &v1, const SortItemEffect &v2)
{
	return v1.kEquipAttrib.m_Level < v2.kEquipAttrib.m_Level;
}

LC_ClientRumorManager* SC_ClientSkillInterface::GetClientRumorManager()
{
	return CLIENT_GET_RUMOR_MANAGER();
}

int SC_ClientSkillInterface::GetVipProfitDefineExtraBeginID(int iID, int iLevel )
{
	LC_VIPCmnManager* pkMgr = LC_VIPCmnManager::GetSingletonPtr();
	LC_VIPProfitDataEntry* pkDataEntry = pkMgr->GetVIPProfitDataEntry(iID);
	if (pkDataEntry)
	{
		const Utility::UT_SIMDataList* pkInfoList = pkDataEntry->GetExtra(iLevel, false);
		if(!pkInfoList->empty())
		{
			return pkInfoList->begin()->ID();
		}
	}
	return 0;
};

StringType SC_ClientSkillInterface::GetWorld2RadarPositionInfo(int iMapResID, int iX, int iY)
{
	CF_WorldMapList::DataEntry *pkMapInfo = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(iMapResID);
	if(NULL == pkMapInfo)
		return "";

	float fMapWidth = Int2Float(LC_Helper::GetWorldMapWidth(iMapResID));
	UT_Vec2Float kBottomRightPosition = UT_Vec2Float(fMapWidth, fMapWidth);
	float fDx = kBottomRightPosition.x;
	float fDy = kBottomRightPosition.y;
	float dAB = sqrt(fDx*fDx + fDy*fDy) * 0.996195f;
	float fXOffset = fDx/2.0f;
	float fYOffset = fDy/2.0f;

	float fXMapClipoffset = pkMapInfo->_kClipArea.x * 1.0f/pkMapInfo->_kClipSize.x;
	float fYMapClipoffset = pkMapInfo->_kClipArea.y * 1.0f/pkMapInfo->_kClipSize.y;
	float fMapUIScaleX = pkMapInfo->_iMapUISize * 1.0f / pkMapInfo->_kClipSize.x;
	float fMapUIScaleY = pkMapInfo->_iMapUISize* 1.0f / pkMapInfo->_kClipSize.y;

	Utility::UT_Vec2Float kPos;
	kPos.x = iX - fXOffset;
	kPos.y = iY - fYOffset;

	Utility::UT_Vec2Float kPosNew;
	kPosNew.x = 0.766044f * kPos.x + 0.642788f * kPos.y + fXOffset;
	kPosNew.y = 0.766044f * kPos.y - 0.642788f * kPos.x + fYOffset;

	UT_Vec2Float kPosA(0.0f, 0.0f);
	UT_Vec2Float kPosB(0.0f, 0.0f);
	kPosA.x = fDx/2.0f - dAB/2.0f;
	kPosA.y = fDy/2.0f - dAB/2.0f;
	kPosB.x = fDx/2.0f + dAB/2.0f;
	kPosB.y = fDy/2.0f + dAB/2.0f;

	float fResultX = (kPosNew.x - kPosA.x) / (kPosB.x - kPosA.x) * fMapUIScaleY - fYMapClipoffset;
	float fResultY = (kPosNew.y - kPosA.y) / (kPosB.y - kPosA.y) * fMapUIScaleX - fXMapClipoffset;
	
	fResultX = fResultX * pkMapInfo->_kClipSize.y;
	fResultY = fResultY * pkMapInfo->_kClipSize.x;

	StringStreamType sStream("");
	sStream << Float2Int(fResultY) << "," << Float2Int(fResultX);
	return sStream.str();
};

bool SC_ClientSkillInterface::IsItemAutoUseByPlayer(long ItemID)
{
	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	if (!pkItemFactory)
	{
		return false;
	}
	LC_ItemBase* pkItem = pkItemFactory->RequestItem(ItemID);
	if(!pkItem)
	{
		return false;
	}
	if (!pkItem->IsAutoUseItem())
	{
		return false;
	}
	LC_ClientGamePlayer *pkPlayer = GetPlayerPtr();
	if (!pkPlayer)
	{
		return false;
	}
	if (!pkItemFactory->CheckItemUsable(pkPlayer, ItemID))
	{
		return false;
	}
	return true;
}

bool SC_ClientSkillInterface::IsItemAutoShowByPlayer(long ItemID)
{
	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	if (!pkItemFactory)
	{
		return false;
	}
	LC_ItemBase* pkItem = pkItemFactory->RequestItem(ItemID);
	if(!pkItem)
	{
		return false;
	}
	if (!pkItem->IsAutoShowItem())
	{
		return false;
	}
	LC_ClientGamePlayer *pkPlayer = GetPlayerPtr();
	if (!pkPlayer)
	{
		return false;
	}
	if (!pkItemFactory->CheckItemUsable(pkPlayer, ItemID))
	{
		return false;
	}
	return true;
}

bool SC_ClientSkillInterface::IsItemTrueAutoUseByPlayer(long ItemID)
{
	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	if (!pkItemFactory)
	{
		return false;
	}
	LC_ItemBase* pkItem = pkItemFactory->RequestItem(ItemID);
	if(!pkItem)
	{
		return false;
	}
	if (!pkItem->IsTrueAutoUseItem())
	{
		return false;
	}
	LC_ClientGamePlayer *pkPlayer = GetPlayerPtr();
	if (!pkPlayer)
	{
		return false;
	}
	if (!pkItemFactory->CheckItemUsable(pkPlayer, ItemID))
	{
		return false;
	}
	return true;
}


int	SC_ClientSkillInterface::GetUseGranuLimitCount(long ItemID)
{
	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	if (!pkItemFactory)
	{
		return 0;
	}
	LC_ItemBase* pkItem = pkItemFactory->RequestItem(ItemID);
	if(!pkItem)
	{
		return 0;
	}
	return pkItem->GetUseGranuLimit();
}
int64_t SC_ClientSkillInterface::GetNextColdDownTimeByCronStr(const StringType& cronStr)
{
	Utility::UT_CronData kCron;

	if(kCron.ParseCronStr(cronStr))
	{
		LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
		LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
		uint64_t iSystemTime = 0;
		if (!pkGamePlayer)
			iSystemTime = GET_SYSTEM_TIME;
		else
			iSystemTime = pkGamePlayer->GetServerTimeVal();
		time_t cur = (time_t)(iSystemTime);
		time_t p = cur;

		time_t NextTime = kCron.GetNextTime(p);

		uint64_t iNextTime = (uint64_t)NextTime;
		return (iNextTime - iSystemTime);
	}
	return -1;
}

void SC_ClientSkillInterface::StopDownloadImageResources(const StringType &sWidgetPath)
{
	StringType uiKey = sWidgetPath + "_UI";
	UT_FileManagerBase::GetSingleton().RemoveDownloadRequest(uiKey.c_str());
}

long	SC_ClientSkillInterface::GetToyPetCharTypeBycID(int32_t cID)
{
	CF_Pet::DataEntry* dataEntry = SafeGetCSVFileDataEntryPtr<CF_Pet>(cID);
	if (dataEntry != NULL)
	{
		return dataEntry->_charType;
	}
	return 0;
}

StringType SC_ClientSkillInterface::GetToyPetInfoByCID(int32_t cID)
{
	CF_Pet::DataEntry* dataEntry = SafeGetCSVFileDataEntryPtr<CF_Pet>(cID);
	if (dataEntry == NULL)
		return "";

	StringStreamType sStream;
	sStream << Int2String(dataEntry->_quality) << '#' << dataEntry->_qualityicon << '#';
	sStream << dataEntry->_PetName << '#'<< dataEntry->_jobicon << '#' << Int2String(dataEntry->_charType) << '#';
	sStream << Int2String(dataEntry->_MaxLevel) << '#' << dataEntry->_simg << '#' << dataEntry->sequence << '#' << dataEntry->_skillid;
	return sStream.str();
}

StringType SC_ClientSkillInterface::GetShopCityGroupInfo(int iGroupID)
{
	CF_ShopGroupOpenSetting::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_ShopGroupOpenSetting>(iGroupID);
	if(pkData == NULL)
		return "";

	StringType sCondition;
	FormatDataIFs2String(pkData->_kBuyConditions, sCondition);
	return sCondition;
}

bool SC_ClientSkillInterface::IsNeedShowFly(int iVal)
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(pkGamePlayer)
	{
		return (IsTravelBoots(iVal) && !pkGamePlayer->IsRedNameInvalidTTT(GetLinkMapResID(iVal)));
	}
	return false;
}

int SC_ClientSkillInterface::GetLinkLevelInfo(int iLinkInfoID)
{
	if (CF_WorldObjectDesc::GetSingletonPtr() == NULL)
		return 0;

	CF_WorldObjectDesc::DataEntry* pkEntry = CF_WorldObjectDesc::GetSingletonPtr()->GetEntryPtr(iLinkInfoID);
	if (pkEntry == NULL)
		return 0;

	if (pkEntry->_lType != 3)
	{
		return 0;
	}
	if (pkEntry->_lLevel > 0 )
		return pkEntry->_lLevel;

	CF_CharType::DataEntry *pkCharTypeEntry = SafeGetCSVFileDataEntryPtr<CF_CharType>(pkEntry->_lLogicID);
	if (pkCharTypeEntry == NULL)
		return 0;

	return pkCharTypeEntry->_lLevel;
}

StringType SC_ClientSkillInterface::GetTransferPosition(int iMapResID)
{
	int iTransferAreaID = 0;
	CF_MapCategory::DataEntryMapExternIterator kIter = CF_MapCategory::GetSingletonPtr()->GetIterator();
	for( ; kIter.HasMoreElements(); kIter.MoveNext() )
	{
		const CF_MapCategory::DataEntry* pkEntry = kIter.PeekNextValuePtr();
		if(pkEntry->_lMaprResID == iMapResID)
		{
			iTransferAreaID = pkEntry->_iTransferAreaID.empty() ? 0 : pkEntry->_iTransferAreaID.front();
			break;
		}
	}

	if(0 == iTransferAreaID)
		return "";

	CF_TransferArea::DataEntry *pkTransferArea = SafeGetCSVFileDataEntryPtr<CF_TransferArea>(iTransferAreaID);
	if(NULL == pkTransferArea)
		return "";

	UT_Vec3Int kTargetLocation;
	size_t sz = pkTransferArea->_kRandomPos.size();
	if (sz > 0)
	{
		const Utility::UT_SIMDataInfo& kPos = pkTransferArea->_kRandomPos[UT_MathBase::RandInRangeInt(0, (int)sz - 1)];
		kTargetLocation = UT_Vec3Int(kPos.X(), kPos.Y(), kPos.Z());
	}
	else
	{
		kTargetLocation = pkTransferArea->_kPos;
	}

	StringStreamType sStream;
	sStream << kTargetLocation.x << " " << kTargetLocation.y << " " << kTargetLocation.z;
	return sStream.str();
}

bool SC_ClientSkillInterface::RaidMapCanRecordPK(int mapResID)
{
	CF_WorldMapList::DataEntry* ety = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(mapResID);
	if (ety)
	{
		return ety->_RecordPK == 1;
	}
	return true;
}

bool SC_ClientSkillInterface::RaidMapCanPk(int mapResID)
{
	CF_WorldMapList::DataEntry* ety = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(mapResID);
	if (ety)
	{
		return ety->_CanPK == 1;
	}
	return true;
}

int SC_ClientSkillInterface::GetWorldObjecDescID(const StringType& info)
{
	return LC_Helper::GetWorldObjecDescID(info);
}

void SC_ClientSkillInterface::SetClientModeCull(GameLogic::LC_ActorBase* pkCaller, bool isCull)
{
	if ( pkCaller )
	{
		GD_ClientActorModel* model = (GD_ClientActorModel*)IM_Manager::GetSingleton().GetModel(pkCaller->GetID());
		if ( model)
		{
			model->SetClientCull(isCull);
		}
	}
}

int SC_ClientSkillInterface::IsPkPunishArea(int mapResID)
{
	LC_ClientMapTranscriptionManager* pkMapTransMgr = LC_ClientMapTranscriptionManager::GetSingletonPtr();
	LC_ClientMapTransDataEntry* entry = pkMapTransMgr->GetMapTransDataEntry(mapResID);
	if ( !entry )
	{
		return 0;
	}

	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if ( !pkGamePlayer )
	{
		return 0;
	}

	//condi1：野外
	int mapLogicType = entry->GetMapLogicType();
	bool isNormalMap = IS_NORMAL_MAP(mapLogicType) && !LC_Helper::IsAllianceMap(mapResID);

	LC_ClientGameStoryManager* GameStoryMgr = pkGamePlayer->GetGameStoryManager();
	if ( !GameStoryMgr) 
	{
		return 0;
	}

	//condi2：不是势力争夺战
	GameStoryInfo* ginfo = GameStoryMgr->GetGameStoryInfo(GAMESTORY_TYPE_ShiLiZhengDuo);
	bool bFlag = false;
	if ( ginfo )
	{
		bFlag = GameStoryMgr->GameStoryCanJoin(ginfo);
	}

	//condi3: 记录pk值
	bool rcPK = RaidMapCanRecordPK(mapResID);

	//condi4：地图能否PK
	bool CanPk = RaidMapCanPk(mapResID);


	//0.不可以PK的区域（canPK=0，安全区）不显示文字
	//1.可以PK，记录PK值（canPK=1，RecordPK=1），显示文字“PK限制区域” 野外势力争夺战另外
	//2.可以PK，不记录PK值（canPK=1，RecordPK=0，，活动副本），显示文字“PK无限制区域”
	//if (pkGamePlayer->IsSafeArea() || !CanPk)
	if (!CanPk)
	{
		return 0;
	}
	else if (rcPK && (!isNormalMap || (isNormalMap && !bFlag)))
	{
		return 1;
	}
	else
	{
		return 2;
	}
}

bool SC_ClientSkillInterface::CanChangePKMode(int mapResID, int iPkState)
{
	CF_WorldMapList::DataEntry* ety = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(mapResID);
	if (ety)
	{
		return std::find(ety->_FixPKMode.begin(), ety->_FixPKMode.end(), iPkState) != ety->_FixPKMode.end();
	}
	return true;
}

int SC_ClientSkillInterface::GetFixPKMode(int mapResID)
{
	CF_WorldMapList::DataEntry* ety = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(mapResID);
	if (ety)
	{
		if (ety->_FixPKMode.size()>0)
		{
			return ety->_FixPKMode[0];
		}
	}
	return 1;
}

int SC_ClientSkillInterface::GetEnterPKMode(int mapResID)
{
	CF_WorldMapList::DataEntry* ety = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(mapResID);
	if (ety)
	{
		return ety->_enterPKMode;
	}
	return 0;
}

StringType SC_ClientSkillInterface::GetBreakEquipLevelProperty(int iEqupSlotIdx, int iEquipStar)
{
	StringStreamType sCommentStream;

	LC_ActorAttributeMap attrMap;
	LC_ActorAttributeMap attrNextMap;
	CF_EquipSlotStar::DataEntryMapExternIterator iter = CF_EquipSlotStar::GetSingletonPtr()->GetIterator();
	for(; iter.HasMoreElements(); iter.MoveNext())
	{
		const CF_EquipSlotStar::DataEntry *pkEntry = iter.PeekNextValuePtr();
		if(pkEntry->_StarUpType == iEqupSlotIdx)
		{

			if(pkEntry->_StarLevel == iEquipStar)
			{
				const Utility::UT_SIMDataList& attr = pkEntry->Attribute;
				for(Utility::UT_SIMDataList::const_iterator it=attr.begin(); it!=attr.end(); ++it)
				{
					attrMap.ModifyAttributeByPart(it->ParamA(), it->ParamB(), it->ParamC(), true);
				}
			}
			else if(pkEntry->_StarLevel == (iEquipStar + 1))
			{
				const Utility::UT_SIMDataList& attr = pkEntry->Attribute;
				for(Utility::UT_SIMDataList::const_iterator it=attr.begin(); it!=attr.end(); ++it)
				{
					attrNextMap.ModifyAttributeByPart(it->ParamA(), it->ParamB(), it->ParamC(), true);
				}
			}

			if(!pkEntry->_Descript.empty())
				sCommentStream << (int)pkEntry->_StarLevel << '&' << (int)pkEntry->_quality << '&' << (int)pkEntry->_StarLevelClient << '&' << pkEntry->_Descript << '_';
		}
	}

	attrMap.RecomputeAll();
	attrNextMap.RecomputeAll();

	StringStreamType sStream;
	sStream << _getAttribString(&attrMap) << '@' << _getAttribString(&attrNextMap) << '@' << sCommentStream.str();
	return sStream.str();
}

int SC_ClientSkillInterface::GetBreakEquipPropertySpecialChange(int iEqupSlotIdx, int iEquipStar)
{
	CF_EquipSlotStar::DataEntryMapExternIterator iter = CF_EquipSlotStar::GetSingletonPtr()->GetIterator();
	for(; iter.HasMoreElements(); iter.MoveNext())
	{
		const CF_EquipSlotStar::DataEntry *pkEntry = iter.PeekNextValuePtr();
		if(pkEntry->_StarUpType == iEqupSlotIdx
			&& pkEntry->_StarLevel == iEquipStar)
		{
			return pkEntry->_PropertySpecialChange;
		}
	}
	return 0;
}

StringType SC_ClientSkillInterface::GetBreakEquipProperty(LC_BackPackEntry* pkEquipEntry, int iEqupSlotIdx, int iEquipStar)
{
	if (pkEquipEntry == NULL)
		return "";
	int itemID = pkEquipEntry->GetTypeID();

	StringStreamType sCommentStream;
	CF_EquipSlotStar::DataEntryMapExternIterator iter = CF_EquipSlotStar::GetSingletonPtr()->GetIterator();
	for(; iter.HasMoreElements(); iter.MoveNext())
	{
		const CF_EquipSlotStar::DataEntry *pkEntry = iter.PeekNextValuePtr();
		if(pkEntry->_StarUpType == iEqupSlotIdx)
		{
			if(!pkEntry->_Descript.empty())
				sCommentStream << (int)pkEntry->_StarLevel << '&' << (int)pkEntry->_quality << '&' << (int)pkEntry->_StarLevelClient << '&' << pkEntry->_Descript << '_';
		}
	}

	StringStreamType sStream;
	sStream << GetItemBaseAttributes(itemID) << '@' << sCommentStream.str();
	return sStream.str();
}

StringType SC_ClientSkillInterface::GetItemBaseAttributes(int iItemID)
{
	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	if(NULL == pkItemFactory)
	{
		return "";
	}

	LC_ItemBase *pkItem = pkItemFactory->RequestItem(iItemID);
	if(NULL == pkItem)
	{
		return "";
	}

	LC_ActorAttributeMap attrMap;
	pkItem->ComputerAttribMapModify(&attrMap, NULL);
	return _getAttribString(&attrMap);
}

bool SC_ClientSkillInterface::CanReturnToCity( GameLogic::LC_Attacker* pkScriptCaller )
{
	if (pkScriptCaller == NULL || !IS_PLAYER_CLASS_ID(pkScriptCaller->GetID()))
	{
		return false;
	}
	//查找回城点
	CF_WorldMapList* pkCSVWorldMapList = CF_WorldMapList::GetSingletonPtr();
	if (pkCSVWorldMapList == NULL)
	{
		return false;
	}
	LC_PlayerBase* pkPlayer = (LC_PlayerBase*)pkScriptCaller;
	int CurMapResID = pkPlayer->GetMapResID();
	CF_WorldMapList::DataEntry* data = pkCSVWorldMapList->GetEntryPtr(CurMapResID);
	if (data == NULL ||  (pkPlayer->GetServerID() != GetCurGameServer() && data->_CanChgLocation <= 0))
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool SC_ClientSkillInterface::IsItemCategoryShowTipAnim(int iCategory2)
{
	CF_ItemCategory::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_ItemCategory>(iCategory2);
	if(pkEntry == NULL)
		return false;

	return pkEntry->_bShowUiAnim;
}

StringType SC_ClientSkillInterface::GetItemAttributeString(int iItemID, int iEquipStar)
{
	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	if(NULL == pkItemFactory)
	{
		return "";
	}

	LC_ItemBase *pkItem = pkItemFactory->RequestItem(iItemID);
	if(NULL == pkItem)
	{
		return "";
	}

	LC_ItemDescriptionEntry* pkItemDesc = pkItem->GetItemDescriptionEntry();
	if(NULL == pkItemDesc)
	{
		return "";
	}

	int iPropChange = 0;
	CF_EquipStar::DataEntryMapExternIterator kIter = CF_EquipStar::GetSingleton().GetIterator();
	while( kIter.HasMoreElements() )
	{
		const CF_EquipStar::DataEntry* pkData = kIter.PeekNextValuePtr();
		if (pkData->_StarUpType == pkItemDesc->GetStarUpType() && pkData->_StarLevel < iEquipStar)
		{
			iPropChange += pkData->_PropertyChange;
		}

		kIter.MoveNext();
	}

	StringStreamType sStream("");
	ItemEffectManage *pkItemEffectMgr = ItemEffectManage::GetSingletonPtr();
	//基本属性
	for(int i = 0; i < LC_ItemBase::MAX_BASEEQUIP_ATTRIB_COUNT; i++)
	{
		LC_EquipBaseAttrib *pkBaseAttr = pkItem->GetBaseEquip(i);
		if(pkBaseAttr != NULL)
		{
			ItemEffect *pkEffect = pkItemEffectMgr->FindEffect(pkBaseAttr->GetEffectID());
			int value = pkEffect->GetValue(pkBaseAttr->GetEffectLevel());
			sStream << pkEffect->m_iEffectFuncID << "#" << Utility::Round2Int(value*(1 + iPropChange*0.0001f));
			//StringType sDesc = pkEffect->GetDesc(pkBaseAttr->GetEffectLevel());
			//sStream << sDesc;
		}
		sStream << "|";
	}
	return sStream.str();
}

//获取当前当前显卡的默认视频等级
long SC_ClientSkillInterface::GetDefaultConfigLevelOfGPU()
{
	return UT_ConfigManager::GetSingleton().GetDefaultConfigLevel();
}

//根据id获取OpenLevel在线时间
int SC_ClientSkillInterface::GetOpenLevelOnlineTimeByID(int iID)
{
	CF_OpenLevel::DataEntry *pkOpenLevel = SafeGetCSVFileDataEntryPtr<CF_OpenLevel>(iID);
	if(pkOpenLevel)
		return pkOpenLevel->_OnlineTime;

	return 0;
}
// 根据openlevel表id获取已在线总时间
uint32_t SC_ClientSkillInterface::GetOpenLvelTotoalTime(int iID)
{
	LC_ClientGamePlayer* pPlayer = GetPlayerPtr();
	if (pPlayer)
	{
		LC_OpenAbilityAsset& OpenAbilityAsset = pPlayer->GetOpenAbilityAsset();
		LC_OpenAbilityEntry* OpenAbilityEntry = OpenAbilityAsset.GetEntyByID(iID);
		if (OpenAbilityEntry)
		{
			uint32_t timestamp = pPlayer->GetServerTimeVal();
			uint32_t uiCum = timestamp - OpenAbilityEntry->GetCumTimestamp();
			if (timestamp == uiCum)
			{
				uiCum = 0;
			}
			return  uiCum + OpenAbilityEntry->GetCumTime();
		}
	}
	return 0;
}

uint32_t SC_ClientSkillInterface::GetCimeliaLevelInOpenLevelByOpenID(int iOpenID,int iSubID)
{
	CF_OpenLevel *pkCSVOpenLevel = CF_OpenLevel::GetSingletonPtr();
	CF_OpenLevel::DataEntryMapExternIterator iter = pkCSVOpenLevel->GetIterator();

	while(iter.HasMoreElements())
	{
		CF_OpenLevel::DataEntry *pkData = iter.PeekNextValuePtr();
		if(iOpenID == pkData->_iID)
		{
			Utility::UT_SIMDataList m_kSubLevels;
			Utility::ParseFmtString2DataIFs(pkData->_kSubLevels, m_kSubLevels);
			Utility::UT_SIMDataList::iterator it = m_kSubLevels.begin();
			for (; it!=m_kSubLevels.end(); ++it)
			{
				if ((int32_t)it->ID() == iSubID)
				{
					return (int32_t)it->ParamA();
				}
			}
		}

		iter.MoveNext();
	}
	return 0;
}

//根据物品id获取批量使用时背包最少格子数目
uint32_t SC_ClientSkillInterface::GetBatchMaxItemCountByID(int iItemID)
{
	CF_ItemList* pkCSVItemList = CF_ItemList::GetSingletonPtr();
	CF_ItemList::DataEntry* pkItemData = pkCSVItemList->GetEntryPtr(iItemID);
	if (NULL == pkItemData)
	{
		return 0;
	}
	if (pkItemData->_kDisassembleUseFuncIDs.empty())
	{
		return 0;
	}
	CF_ItemDisassembleUseFunc::DataEntry *pkItemDisassembleUseFunc = SafeGetCSVFileDataEntryPtr<CF_ItemDisassembleUseFunc>(pkItemData->_kDisassembleUseFuncIDs.front().ID());
	if(NULL == pkItemDisassembleUseFunc)
	{
		return 0;
	}
	return pkItemDisassembleUseFunc->_lBatchMaxItemCount;
}

//获取法宝人物charTypeID
int	SC_ClientSkillInterface::GetDisplayCharTypeID(int iType, int iCharTypeID)
{
	int lCharTypeID = 0;
	CF_Subordinate::DataEntry *pkEntry = LC_Helper::GetCimeliaInfoEntryByCharTypeID(iType, iCharTypeID);
	if(NULL != pkEntry)
		lCharTypeID = pkEntry->_DisplayItemID;

	return lCharTypeID;
}
//获取帮贡信息	
StringType	SC_ClientSkillInterface::GetGuildDonateInfo()
{
	CF_GuildDonate::DataEntryMapExternIterator kIter = CF_GuildDonate::GetSingleton().GetIterator();
	StringStreamType sStream("");
	uint32_t moneyGet = 0;
	uint32_t Get = 0;
	while(kIter.HasMoreElements())
	{
		CF_GuildDonate::DataEntry* pkData = kIter.PeekNextValuePtr();
		if (pkData->_iDonateType == 0)
		{
			moneyGet = pkData->_iGuildMoney;
		}
		else
		{
			Get = pkData->_iGuildMoney;
		}
		kIter.MoveNext();
	}

	sStream<<GET_MSG(SC_CLIENTSKILLINTERFACE_CPP_016)<<moneyGet<<GET_MSG(SC_CLIENTSKILLINTERFACE_CPP_017)<<Get<<GET_MSG(SC_CLIENTSKILLINTERFACE_CPP_018);
	return sStream.str();
}
int	SC_ClientSkillInterface::GetTreasureCostByID(int iID)
{
	LC_TreasureChestsManager* pkManager = LC_TreasureChestsManager::GetSingletonPtr();

	LC_TreasureChestDataEntry* pkChestEntry = pkManager->GetChestEntry(iID);

	if (NULL == pkChestEntry)
	{
		return 0;
	}
	for (LC_ShopIDMap::const_iterator cit = pkChestEntry->m_kShopIDMap.begin(); cit != pkChestEntry->m_kShopIDMap.end(); ++cit)
	{
		return cit->second;
	}
	return 0;
}

//根据技能id获取秘籍id 绝学表
uint32_t	SC_ClientSkillInterface::GetSkillBookID(int iSkillID)
{
	CF_SecretSkillClient::DataEntryMapExternIterator kIter = CF_SecretSkillClient::GetSingleton().GetIterator();
	while(kIter.HasMoreElements())
	{
		CF_SecretSkillClient::DataEntry* pkData = kIter.PeekNextValuePtr();
		if (pkData && pkData->_SkillID == iSkillID)
		{
			return pkData->_SkillBookID;
		}

		kIter.MoveNext();
	}
	return 0;
}
uint32_t SC_ClientSkillInterface::GetSecretTypeByBookID(int iBookID)
{
	CF_SecretSkillClient::DataEntryMapExternIterator kIter = CF_SecretSkillClient::GetSingleton().GetIterator();
	while(kIter.HasMoreElements())
	{
		CF_SecretSkillClient::DataEntry* pkData = kIter.PeekNextValuePtr();
		if (pkData && pkData->_SkillBookID == iBookID)
		{
			return pkData->_Type;
		}

		kIter.MoveNext();
	}
	return 0;
}

//获取美女客服数量
uint32_t	SC_ClientSkillInterface::GetQQGirlsCount()
{
	CF_QQGirls* pkQQGirls = CF_QQGirls::GetSingletonPtr();
	if (NULL == pkQQGirls)
	{
		return 0;
	}
	return pkQQGirls->GetEntryCount();
}

uint32_t	SC_ClientSkillInterface::GetFabaoIdByCharType(int charTypeID)
{
	CF_SubordinateTransform::DataEntryMapExternIterator kIter = CF_SubordinateTransform::GetSingleton().GetIterator();
	while(kIter.HasMoreElements())
	{
		CF_SubordinateTransform::DataEntry* pkData = kIter.PeekNextValuePtr();
		if (pkData->_iCharType == charTypeID)
		{
			return pkData->_iID;
		}
		kIter.MoveNext();
	}
	return -1;
}

uint32_t	SC_ClientSkillInterface::GetCharTypeByFabaoID(int fabaoID)
{
	CF_SubordinateTransform::DataEntry* pkEntry = SafeGetCSVFileDataEntryPtr<CF_SubordinateTransform>(fabaoID);
	if (pkEntry != NULL)
	{
		return pkEntry->_iCharType;
	}
	return 0;
}

int SC_ClientSkillInterface::GetSubTransformSkillID(int iTransformID)
{
	const CF_SubordinateTransform::DataEntry *pkTransfromData = SafeGetCSVFileDataEntryPtr<CF_SubordinateTransform>(iTransformID);
	if(pkTransfromData == NULL)
		return 0;

	return pkTransfromData->_iSkill;
}

int SC_ClientSkillInterface::GetSubTransformType(int iTransformID)
{
	const CF_SubordinateTransform::DataEntry *pkTransfromData = SafeGetCSVFileDataEntryPtr<CF_SubordinateTransform>(iTransformID);
	if(pkTransfromData == NULL)
		return 0;

	return pkTransfromData->_iSubType;
}

uint32_t SC_ClientSkillInterface::GetGuildWarEndTime(int iWarType,int OpenType)
{
	LC_ClientGamePlayer* pPlayer = GetPlayerPtr();
	if (NULL ==  pPlayer)
	{
		return 0;
	}

	uint32_t iBeginTimes =  0;
	int64_t nStartTime = 0;

	if (OpenType == 1) //开服时间
	{
		iBeginTimes = pPlayer->GetServerOpenTime();
	}
	else if (OpenType == 2) //合服时间
	{
		iBeginTimes = pPlayer->GetServerConflateTime();
	}

	nStartTime = (uint64_t)LC_Helper::GetDayTimeStamp(time_t(iBeginTimes), 0, 0, 0);
	int iEndTime = 0;

	CF_GuildWarActivity::DataEntryMapExternIterator kIter = CF_GuildWarActivity::GetSingleton().GetIterator();
	while(kIter.HasMoreElements())
	{
		CF_GuildWarActivity::DataEntry* pkData = kIter.PeekNextValuePtr();
		if (pkData->_iType == iWarType)
		{
			if (OpenType == 1)
			{
				iEndTime = pkData->_endSTime.GetTimeStramp(nStartTime);
			}
			else if (OpenType == 2)
			{
				iEndTime = pkData->_endMTime.GetTimeStramp(nStartTime);
			}

		}
		kIter.MoveNext();
	}
	return iEndTime;
}

//圣火令和屠龙刀
uint32_t	SC_ClientSkillInterface::CheckGuildWarTime(int iWarType,int OpenType)
{
	LC_ClientGamePlayer* pPlayer = GetPlayerPtr();
	if (NULL ==  pPlayer)
	{
		return 0;
	}

	uint32_t iBeginTimes =  0;
	int64_t nStartTime = 0;

	if (OpenType == 1) //开服时间
	{
		iBeginTimes = pPlayer->GetServerOpenTime();
	}
	else if (OpenType == 2) //合服时间
	{
		iBeginTimes = pPlayer->GetServerConflateTime();
	}

	nStartTime = (uint64_t)LC_Helper::GetDayTimeStamp(time_t(iBeginTimes), 0, 0, 0);

	int iCurTime = pPlayer->GetServerTimeVal();

	CF_GuildWarActivity::DataEntryMapExternIterator kIter = CF_GuildWarActivity::GetSingleton().GetIterator();
	while(kIter.HasMoreElements())
	{
		CF_GuildWarActivity::DataEntry* pkData = kIter.PeekNextValuePtr();
		if (pkData->_iType == iWarType)
		{
			if (OpenType == 1)
			{
				int iSBeginTime = pkData->_beginSTime.GetTimeStramp(nStartTime);
				int iSEndTime = pkData->_endSTime.GetTimeStramp(nStartTime);
				if (iCurTime >= iSBeginTime && iCurTime <= iSEndTime)
				{
					return 1;
				}
			}
			else if (OpenType == 2)
			{
				int iSBeginTime = pkData->_beginMTime.GetTimeStramp(nStartTime);
				int iSEndTime = pkData->_endMTime.GetTimeStramp(nStartTime);
				if (iCurTime >= iSBeginTime && iCurTime <= iSEndTime)
				{
					return 1;
				}
			}

		}
		kIter.MoveNext();
	}
	return 0;
}
int	SC_ClientSkillInterface::CheckSecretPoolValueID(int iItemID,int iItemCount,int iCatogory,int iRewardIndex)
{
	CF_SecretTreasureReward::DataEntryMapExternIterator kIter = CF_SecretTreasureReward::GetSingleton().GetIterator();
	while(kIter.HasMoreElements())
	{
		CF_SecretTreasureReward::DataEntry* pkData = kIter.PeekNextValuePtr();
		if (pkData && pkData->_iCatagory == iCatogory && pkData->_iID >= iRewardIndex)
		{			
			Utility::UT_SIMDataList::iterator itIcons = pkData->_kRewardItems.begin();
			for (; itIcons!=pkData->_kRewardItems.end(); ++itIcons)
			{
				if (itIcons->ID() == iItemID && itIcons->ParamA()==iItemCount)
				{
					return pkData->_iID;
				}
			}
		}
		kIter.MoveNext();
	}
	return 0;
}

StringType	SC_ClientSkillInterface::CheckRichManTime(int iChestID,int iTime)
{
	int hasTimes = iTime;
	StringStreamType sStream("");
	CF_SecretTreasureTimes::DataEntryMapExternIterator kIter = CF_SecretTreasureTimes::GetSingleton().GetIterator();
	while(kIter.HasMoreElements())
	{
		CF_SecretTreasureTimes::DataEntry* pkData = kIter.PeekNextValuePtr();
		if (pkData && pkData->_iChestID == iChestID)
		{			
			if (hasTimes < pkData->_iTimes)
			{
				sStream << pkData->_iType<<"#"<<pkData->_iValue;
				break;
			}
			else
			{
				hasTimes -= pkData->_iTimes;
			}
		}
		kIter.MoveNext();
	}
	return sStream.str();
}

int	SC_ClientSkillInterface::GetScretTreasureTotalTimes(int iChestID,int iCurValue)
{
	CF_SecretTreasureTimes::DataEntryMapExternIterator kIter = CF_SecretTreasureTimes::GetSingleton().GetIterator();
	while(kIter.HasMoreElements())
	{
		CF_SecretTreasureTimes::DataEntry* pkData = kIter.PeekNextValuePtr();
		if (pkData && pkData->_iChestID == iChestID)
		{			
			uint32_t uiFactor = iCurValue / (uint32_t)pkData->_iValue;
			return uiFactor>=1 ? (pkData->_iTimes + (uiFactor-1)*pkData->_iAccumExpo) : 0;
		}
		kIter.MoveNext();
	}
	return 0;
}

StringType SC_ClientSkillInterface::GetAchievementInfo(int iID)
{
	StringStreamType sStream("");
	CF_AchievementList::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_AchievementList>(iID);
	if (pkEntry)
	{
		sStream << pkEntry->_sAchievementinfo << "#";
		sStream << pkEntry->_iLinkID << "#";
	}
	return sStream.str();
}

int	SC_ClientSkillInterface::GetScretTimesValue(int iChestID)
{
	CF_SecretTreasureTimes::DataEntryMapExternIterator kIter = CF_SecretTreasureTimes::GetSingleton().GetIterator();
	while(kIter.HasMoreElements())
	{
		CF_SecretTreasureTimes::DataEntry* pkData = kIter.PeekNextValuePtr();
		if (pkData && pkData->_iChestID == iChestID)
		{			
			return pkData->_iValue;
		}
		kIter.MoveNext();
	}
	return 0;
}
int	SC_ClientSkillInterface::GetScretLimitTimes(int iChestID)
{
	CF_SecretTreasureTimes::DataEntryMapExternIterator kIter = CF_SecretTreasureTimes::GetSingleton().GetIterator();
	while(kIter.HasMoreElements())
	{
		CF_SecretTreasureTimes::DataEntry* pkData = kIter.PeekNextValuePtr();
		if (pkData && pkData->_iChestID == iChestID)
		{			
			return pkData->_iLimit;
		}
		kIter.MoveNext();
	}
	return 0;

}

StringType SC_ClientSkillInterface::CheckActivitySchedule(int iCatagory)
{	
	StringStreamType sStream("");

	LC_ClientGamePlayer* pPlayer = GetPlayerPtr();
	if (NULL ==  pPlayer)
	{
		return sStream.str();
	}
	CF_SecretTreasureSchedule::DataEntryMapExternIterator kIter = CF_SecretTreasureSchedule::GetSingleton().GetIterator();
	while(kIter.HasMoreElements())
	{
		CF_SecretTreasureSchedule::DataEntry* pkData = kIter.PeekNextValuePtr();
		if (pkData->_iCatagory == iCatagory)
		{			
			CF_ActivitySchedule& pkActSche = CF_ActivitySchedule::GetSingleton();
			const CF_ActivitySchedule::DataEntry* pkScheData = pkActSche.GetEntryPtr(pkData->_iSchedule);
			if (NULL != pkScheData)
			{
				//if (iCatagory == 0)//丹哥宝库
				//{
					uint32_t endTime = 0;
					uint32_t beginTime = 0;

					bool bCronTime = LC_ClientHelp::CheckCronTimeStamp(pkScheData->_sCron, pkScheData->_iTimeLen, beginTime, endTime);
					if (bCronTime)
					{
						sStream<<pkData->_iSchedule<<"#"<<endTime<<"#"<<pkData->_iCluster<<"#";
						break;
					}
				//}
				//else if (iCatagory == 1 || iCatagory == 2) //大富翁 砸蛋 开服前几天
				//{
					uint32_t iBeginTimes = pPlayer->GetServerOpenTime();
					int64_t nStartTime = (uint64_t)LC_Helper::GetDayTimeStamp(time_t(iBeginTimes), 0, 0, 0);
					CSVFile::StartTime time1 = pkScheData->_StartValidTime;
					CSVFile::StartTime time2 = pkScheData->_StartCoolTime;
					int iSEndTime1 = time1.GetTimeStramp(nStartTime);
					int iSEndTime2 = time2.GetTimeStramp(nStartTime);
					int iCurTime = pPlayer->GetServerTimeVal();
					if ((time1.GetTimeStramp() > 0 && iCurTime <= iSEndTime1 && iCurTime- nStartTime <= pkScheData->_iTimeLen*60) || (time2.GetTimeStramp()>0&&iCurTime > iSEndTime2 && iCurTime-iSEndTime2 < pkScheData->_iTimeLen*60))
					{
						sStream<<pkData->_iSchedule<<"#0#"<<pkData->_iCluster<<"#";
						break;
					}
				//}
			}
		}
		kIter.MoveNext();
	}

	return sStream.str();
}

//< tx对接，通过网页购买物品，显示web窗口
bool SC_ClientSkillInterface::showWebWindowBuyItem( int payitem_id, int payitem_price, int payitem_num, const StringType & goodsmeta, const StringType & goodsurl, const StringType & aux )
{
	LC_ClientGamePlayer* pPlayer = GetPlayerPtr();
	if (!pPlayer)
	{
		Utility::GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM,"(err)showWebWindowBuyItem noplayer");
		return false;
	}

	int bMiniClient = IsRunInMiniLauncher();

	TxVip vt = getTxSubplatformVip();
	if (vt == TSV_YELLOW && !bMiniClient)	//< 微端黄钻充值走大厅接口，网页端保持不变
	{
		if (!ClientCmdlineArg::HasValue(CMD_ARG_UID))			return false;	//< openid
		if (!ClientCmdlineArg::HasValue(CMD_ARG_OPENKEY))		return false;
		if (!ClientCmdlineArg::HasValue(CMD_ARG_PLATFORM_FULL))	return false;
		if (!ClientCmdlineArg::HasValue(CMD_ARG_PFKEY))			return false;
		if (!ClientCmdlineArg::HasValue(CMD_ARG_TX_SID))		return false;
		if (!ClientCmdlineArg::HasValue(CMD_ARG_SID))			return false;

		const StringType & a = ClientCmdlineArg::GetValue(CMD_ARG_UID);		//< openid
		const StringType & b = ClientCmdlineArg::GetValue(CMD_ARG_OPENKEY);
		const StringType & c = ClientCmdlineArg::GetValue(CMD_ARG_PLATFORM_FULL);
		const StringType & d = ClientCmdlineArg::GetValue(CMD_ARG_PFKEY);
		const StringType & e = ClientCmdlineArg::GetValue(CMD_ARG_TX_SID);
		const StringType & e2= ClientCmdlineArg::GetValue(CMD_ARG_SID);

		//////////////////////////////////////////////////////////////////////////
		std::string f = jzUrlEncode(CodeConvert::MbsToUtf8(goodsmeta).c_str());

		StringStreamType usm;
		usm << "http://s"		<< e.c_str() << ".app1104956040.qqopenapp.com/res/recharge.php?";
		usm << "openid="		<< a.c_str();
		usm << "&openkey="		<< b.c_str();
		usm << "&platform="		<< c.c_str();
		usm << "&pfkey="		<< d.c_str();
		usm << "&zoneid="		<< e.c_str();
		usm << "&serverid="		<< e2.c_str();
		usm << "&payitem="		<< payitem_id << "*" << payitem_price << "*" << payitem_num;
		usm << "&goodsmeta="	<< f.c_str();
		usm << "&goodsurl="		<< goodsurl.c_str();
		usm << aux.c_str();

		//////////////////////////////////////////////////////////////////////////

		StringType   str = usm.str();
		const char * url = str.c_str();
		Utility::GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM,"showWebWindowBuyItem %s", url);

		return showWebWindow(url, 690, 600);
	}

	if (vt == TSV_BLUE || (vt == TSV_YELLOW && bMiniClient))
	{
		if (!ClientCmdlineArg::HasValue(CMD_ARG_UID))			return false;	//< openid
		if (!ClientCmdlineArg::HasValue(CMD_ARG_PLATFORM_FULL))	return false;
		if (!ClientCmdlineArg::HasValue(CMD_ARG_TX_SID))		return false;
		if (!ClientCmdlineArg::HasValue(CMD_ARG_SID))			return false;

		const StringType & a = ClientCmdlineArg::GetValue(CMD_ARG_UID);		//< openid
		const StringType & c = ClientCmdlineArg::GetValue(CMD_ARG_PLATFORM_FULL);
		const StringType & e = ClientCmdlineArg::GetValue(CMD_ARG_TX_SID);
		const StringType & e2= ClientCmdlineArg::GetValue(CMD_ARG_SID);

		//////////////////////////////////////////////////////////////////////////
		//std::string f = jzUrlEncode(CodeConvert::MbsToUtf8(goodsmeta).c_str());

		//http://qqgameplatcdn.qq.com/social_hall/app_frame/?appid=1104956040&param=buygoods|platform=qqgame|zoneid=2|title=ceshigoumai|payitem=10001*10*1|goodsmeta=abc*def|goodsurl=http://res.jzwl.mokun.net/debug/tst116.png

		StringStreamType usm;
		usm << "http://qqgameplatcdn.qq.com/social_hall/app_frame/?appid=1104956040&param=buygoods";
		usm << "|platform="		<< c.c_str();
		usm << "|zoneid="		<< e.c_str();
		usm << "|serverid="		<< e2.c_str();
		usm << "|title="		<< "%e5%85%83%e5%ae%9d%e8%b4%ad%e4%b9%b0";	// '元宝购买'
		usm << "|payitem="		<< payitem_id << "*" << payitem_price << "*" << payitem_num;
		usm << "|goodsmeta="	<< payitem_price << "%e5%85%83%e5%ae%9d" << "*" << payitem_price << "%e5%85%83%e5%ae%9d" ;
		usm << "|goodsurl="		<< goodsurl.c_str();
		if (isTxUnion() || (vt == TSV_YELLOW && bMiniClient))	//< 如果是union，附加openkey和pfkey
		{
			const StringType & b = ClientCmdlineArg::GetValue(CMD_ARG_OPENKEY);
			usm << "|openkey="		<< b.c_str();

			const StringType & f = ClientCmdlineArg::GetValue(CMD_ARG_PFKEY);
			usm << "|pfkey="		<< f.c_str();
		}
		usm << aux.c_str();

		//////////////////////////////////////////////////////////////////////////

		StringType   str = usm.str();
		const char * url = str.c_str();
		Utility::GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM,"showWebWindowBuyItem QQGame %s", url);

		return showWebWindow(url,  690, 600);
	}

	return false;
}

bool SC_ClientSkillInterface::showWebWindowYellowVip( int tokentype, const StringType & discountid, bool bYearOrMonth )
{
	TxVip vt = getTxSubplatformVip();
	if (vt == TSV_YELLOW)
	{
		if (!ClientCmdlineArg::HasValue(CMD_ARG_UID))			return false;	//< openid
		if (!ClientCmdlineArg::HasValue(CMD_ARG_OPENKEY))		return false;
		if (!ClientCmdlineArg::HasValue(CMD_ARG_PLATFORM_FULL))	return false;
		if (!ClientCmdlineArg::HasValue(CMD_ARG_PFKEY))			return false;
		if (!ClientCmdlineArg::HasValue(CMD_ARG_TX_SID))		return false;

		const StringType & a = ClientCmdlineArg::GetValue(CMD_ARG_UID);		//< openid
		const StringType & b = ClientCmdlineArg::GetValue(CMD_ARG_OPENKEY);
		const StringType & c = ClientCmdlineArg::GetValue(CMD_ARG_PLATFORM_FULL);
		const StringType & d = ClientCmdlineArg::GetValue(CMD_ARG_PFKEY);
		const StringType & e = ClientCmdlineArg::GetValue(CMD_ARG_TX_SID);
		const StringType & e2= ClientCmdlineArg::GetValue(CMD_ARG_SID);

		//////////////////////////////////////////////////////////////////////////

		const char * paytype = bYearOrMonth ? "&paytime=year" : "";

		StringStreamType usm;
		usm << "http://s"		<< e.c_str() << ".app1104956040.qqopenapp.com/res/yellowvip.php?";
		usm << "openid="		<< a.c_str();
		usm << "&openkey="		<< b.c_str();
		usm << "&platform="		<< c.c_str();
		usm << "&pfkey="		<< d.c_str();
		usm << "&zoneid="		<< e.c_str();
		usm << "&serverid="		<< e2.c_str();
		usm << "&version=v3";
		usm << "&tokentype="	<< tokentype;
		usm << "&discountid="	<< discountid.c_str();
		usm << paytype;

		//////////////////////////////////////////////////////////////////////////

		StringType   str = usm.str();
		const char * url = str.c_str();
		Utility::GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM,"showWebWindowYellowVip %s", url);

		return showWebWindow(url, 690, 600);
	}

	return false;
}

bool SC_ClientSkillInterface::showWebWindowBlueVip( int tokentype, const StringType & discountid, bool bYearOrMonth )
{
	TxVip vt = getTxSubplatformVip();
	if (vt == TSV_BLUE)
	{
		if (!ClientCmdlineArg::HasValue(CMD_ARG_UID))		return false;	//< openid
		if (!ClientCmdlineArg::HasValue(CMD_ARG_OPENKEY))	return false;
		if (!ClientCmdlineArg::HasValue(CMD_ARG_PLATFORM_FULL))	return false;
		if (!ClientCmdlineArg::HasValue(CMD_ARG_TX_SID))		return false;
		if (!ClientCmdlineArg::HasValue(CMD_ARG_SID))		return false;

		const StringType & a = ClientCmdlineArg::GetValue(CMD_ARG_UID);		//< openid
		const StringType & b = ClientCmdlineArg::GetValue(CMD_ARG_OPENKEY);
		const StringType & c = ClientCmdlineArg::GetValue(CMD_ARG_PLATFORM_FULL);
		const StringType & e = ClientCmdlineArg::GetValue(CMD_ARG_TX_SID);
		const StringType & e2= ClientCmdlineArg::GetValue(CMD_ARG_SID);

		//////////////////////////////////////////////////////////////////////////
		// http://qqgameplatcdn.qq.com/social_hall/app_frame/?appid=1104956040&param=bluevipgift|platform=qqgame|zoneid=2|mp_id=UM15121513051626|version=v3|tokentype=1|discountid=UM15121513051626

		StringStreamType usm;
		usm << "http://qqgameplatcdn.qq.com/social_hall/app_frame/?appid=1104956040";
		usm << "&param=bluevip";
		usm << "|platform="		<< c.c_str();
		usm << "|zoneid="		<< e.c_str();
		usm << "|serverid="		<< e2.c_str();
		usm << "|mp_id=UM15121513051626";
		usm << "|version=v3";
		usm << "|tokentype="	<< tokentype;
		usm << "|discountid="	<< discountid.c_str();

		//////////////////////////////////////////////////////////////////////////

		StringType   str = usm.str();
		const char * url = str.c_str();
		Utility::GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM,"showWebWindow_BlueVip %s", url);

		return showWebWindow(url, 690, 600);
	}

	return false;
}
int SC_ClientSkillInterface::GetYunyingPlatform()
{
	return getYunYingplatform();
}

int SC_ClientSkillInterface::GetYunyingPlatformPage()
{
	return getYunYingPlatformPage();
}

StringType SC_ClientSkillInterface::GetCompositionFormulaInfo(int lFormulaID)
{
	StringStreamType sStream("");
	CF_CompositionFormula* pkCSVCompositionList = CF_CompositionFormula::GetSingletonPtr();
	if (pkCSVCompositionList == NULL)
	{
		return sStream.str();
	}
	CF_CompositionFormula::DataEntry* pkCompositionData = pkCSVCompositionList->GetEntryPtr(lFormulaID);
	if (pkCompositionData == NULL)
	{
		return sStream.str();
	}
	UT_SIMDataList &kDataList = pkCompositionData->_kNeedMaterials;
	sStream << pkCompositionData->_iID << "#";
	sStream << pkCompositionData->_lYangItemID << "#";
	sStream << pkCompositionData->_lYinItemID << "#";
	sStream << pkCompositionData->_lCompositionItemID << "#";
	sStream << kDataList.size() << "#";
	for (UT_SIMDataList::iterator it = kDataList.begin(); it != kDataList.end(); ++it)
	{
		sStream << it->ID() << "|" << it->ItemCount() << "|";
	}
	return sStream.str();

}

StringType	SC_ClientSkillInterface::GetDecompositionInfo(int lFormulaID)
{
	StringStreamType sStream("");
	CF_ItemDecompositionFunc* pkCSVDecomposition = CF_ItemDecompositionFunc::GetSingletonPtr();
	if (pkCSVDecomposition == NULL)
	{
		return sStream.str();
	}

	CF_ItemDecompositionFunc::DataEntry* pkDrCompositionData = pkCSVDecomposition->GetEntryPtr(lFormulaID);
	if (pkDrCompositionData == NULL)
	{
		return sStream.str();
	}

	sStream<<LC_Helper::GetComMapValue(pkDrCompositionData->_kConsumeCashMap, CT_UNBIND_YUANBAO)<<"#"<<pkDrCompositionData->_lMaxItemCount<<"#";
	UT_SIMDataList &kDataList = pkDrCompositionData->_lMustDropItems;
	for (UT_SIMDataList::iterator it = kDataList.begin(); it != kDataList.end(); ++it)
	{
		sStream << it->ID() << ":" << it->ItemCount() << ";";
	}
	sStream << "#";
	UT_SIMDataList &kOtherDataList = pkDrCompositionData->_lItem1;
	for (UT_SIMDataList::iterator it = kOtherDataList.begin(); it != kOtherDataList.end(); ++it)
	{
		sStream << it->ID() << ":" << it->ItemCount() << ";";
	}
	sStream << "#";

	return sStream.str();
}

StringType SC_ClientSkillInterface::GetArenaRankReward(int iEntryIndex, bool bSucessed)
{
	
	CF_RankReward* pkRankRewardList = CF_RankReward::GetSingletonPtr();
	CF_RankReward::DataEntry* pkRankRewardData = pkRankRewardList->GetEntryPtr(iEntryIndex);
	if (pkRankRewardData == NULL)
	{
		return "";
	}

	StringStreamType sStream;
	UT_SIMDataList &kDataList = pkRankRewardData->_SucessMaterial;
	if (!bSucessed)
		kDataList = pkRankRewardData->_ErrMaterial;

	sStream << kDataList.size() << '|';
	for (UT_SIMDataList::iterator it = kDataList.begin(); it != kDataList.end(); ++it)
	{
		sStream << it->ID() << '#' << it->ItemCount() << '#';
	}
	
	sStream << '|';

	LC_ClientGamePlayer *pkPlayer = GetPlayerPtr();
	CF_EXPList::DataEntry *pkExpData = SafeGetCSVFileDataEntryPtr<CF_EXPList>(pkPlayer->GetCommanderLevel());
	sStream << SPEC_ITEM_TID_BUDDY_EXP << '|';
	if(bSucessed)
		sStream << pkExpData->_lBaseBuddyXP*pkRankRewardData->_WinExp;
	else
		sStream << pkExpData->_lBaseBuddyXP*pkRankRewardData->_FailExp;

	return sStream.str();

}
int32_t SC_ClientSkillInterface::GetInt32tBy2Int16tUnion(int16_t a, int16_t b)
{
	typedef union {
		int32_t _pack;
		struct{
			int16_t _index;
			int16_t _type;
		};
	} LC_OperationPackParam;

	LC_OperationPackParam kpack;
	kpack._index = a;
	kpack._type	= b;
	return kpack._pack;
}

LC_BackPackEntry* SC_ClientSkillInterface::GetItemCompositionTargetInfo()
{
	return &m_TmpItemCompositionPackEntry;
}

void SC_ClientSkillInterface::SetItemCompositionTargetInfo(LC_BackPackEntry *SrcEntry, int TargetID)
{
	if (!SrcEntry || !SrcEntry->GetValid())
	{
		return;
	}
	LC_PackEntryFactory* pkPackEntryFactory = LC_PackEntryFactory::GetSingletonPtr();
	if(NULL == pkPackEntryFactory)
	{
		return;
	}

	m_TmpItemCompositionPackEntry.Reset();
	m_TmpItemCompositionPackEntry.SetValid(true);
	m_TmpItemCompositionPackEntry.SetTypeID(TargetID);
	m_TmpItemCompositionPackEntry.SetOverlapCount(1);
	m_TmpItemCompositionPackEntry.SetBackPackValueByType(LPSFET_BINDMODE, SrcEntry->GetBackPackValueByType(LPSFET_BINDMODE));
}

void SC_ClientSkillInterface::ResetItemCompositionTargetInfo()
{
	m_TmpItemCompositionPackEntry.Reset();
}

int64_t SC_ClientSkillInterface::GetAuctionSellPrice(int iItemID, int iMoneyType)
{
	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	if (pkItemFactory == NULL)
		return 0;

	LC_ItemBase*  pkItem = pkItemFactory->RequestItem(iItemID);
	if (pkItem == NULL)
		return 0;

	const LC_CashMap& kCashMap = pkItem->GetAuctionSoldCashs();
	if (kCashMap.empty())
		return 0;

	LC_CashMap::const_iterator it = kCashMap.find(iMoneyType);
	if (it != kCashMap.end())
	{
		return it->second;
	}
	else
	{
		return 0;
	}
}

bool SC_ClientSkillInterface::IsEquipReinStar(int iItemID, uint16_t equipStar)
{
	const CF_ItemList::DataEntry *pkItemData = SafeGetCSVFileDataEntryPtr<CF_ItemList>(iItemID);
	if (pkItemData)
	{
		return pkItemData->_InitStar < equipStar;
	}
	return false;
}

int SC_ClientSkillInterface::GetMapIndexByResIDAndLogicID(int lMapResID, int lMapLogicID)
{
	LC_ClientNewBeeManager* pkNewBeeManager = LC_ClientNewBeeManager::GetSingletonPtr();
	if (!pkNewBeeManager)
	{
		return 1;
	}
	return pkNewBeeManager->GetIndexByMapResIDAndMapLogicID(lMapResID,lMapLogicID);
}

bool SC_ClientSkillInterface::IsMapIsNewBeeMap(int lMapResID)
{
	CF_WorldMapList::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(lMapResID);
	if (pkData && ((pkData->_iTributaryMapCount > 1 && pkData->_AutoCreate == 1) || pkData->_bShowLine))
	{
		return true;
	}
	return false;
}

unique_id_type SC_ClientSkillInterface::GetUniqueID(object_id_type playerID, object_id_type ServerID)
{
	unique_id_impl CharID = GEN_UNIQUE_ID(getYunYingplatform(), ServerID, playerID);
	return (unique_id_type)CharID;
}

int SC_ClientSkillInterface::GetDaysInterval(int64_t iBeginTimes, int64_t iEndTimes)
{
	int64_t iBeginDaysTime = LC_Helper::GetDayTimeStamp(time_t(iBeginTimes), 0, 0, 0);
	static int64_t iOneDayInterval = 24 * 60 * 60;
	return (iEndTimes - iBeginDaysTime) / iOneDayInterval;
}

int SC_ClientSkillInterface::GetServerOpenTime(bool bHour)
{
	LC_ClientGamePlayer* pPlayer = GetPlayerPtr();
	if (pPlayer)
	{
		int iBeginTimes = pPlayer->GetServerOpenTime();
		int iEndTimes = pPlayer->GetServerTimeVal();

		int64_t iBeginDaysTime = LC_Helper::GetDayTimeStamp(time_t(iBeginTimes), 0, 0, 0);
		static int64_t iOneDayInterval = 24 * 60 * 60;
		static int64_t iOneHourInterval = 60 * 60;
		if (bHour)
		{
			return (iEndTimes - iBeginDaysTime) / iOneHourInterval;
		}
		else
		{
			return (iEndTimes - iBeginDaysTime) / iOneDayInterval;
		}

	}
	return 0;
}

bool SC_ClientSkillInterface::IsInSelfServer()
{
	LC_ClientGamePlayer *pkPlayer = GetPlayerPtr();
	if (!pkPlayer)
	{
		return true;
	}
	return (GetCurGameServer() == 0) || (GetCurGameServer() == pkPlayer->GetServerID());
	
}

bool SC_ClientSkillInterface::CheckVipPrivilege(int type)
{
	return LC_Helper::GetVipPrivilegeTimesByType(type, GetPlayerPtr()) > 0;
}

StringType SC_ClientSkillInterface::GetVipPrivilegeIcon(int type)
{
	CF_VIPPrivilegeCard* pkVIPProfit = CF_VIPPrivilegeCard::GetSingletonPtr();
	CF_VIPPrivilegeCard::DataEntryMapExternIterator kIter = pkVIPProfit->GetIterator();
	while (kIter.HasMoreElements())
	{
		CF_VIPPrivilegeCard::DataEntry* pkData = kIter.PeekNextValuePtr();
		if (pkData && pkData->_kType == type)
		{
			return pkData->_ItemIcon;
		}
		kIter.MoveNext();
	}
	return "";
}

StringType SC_ClientSkillInterface::GetVipPrivilegeDespTips(int type)
{
	CF_VIPPrivilegeCard* pkVIPProfit = CF_VIPPrivilegeCard::GetSingletonPtr();
	CF_VIPPrivilegeCard::DataEntryMapExternIterator kIter = pkVIPProfit->GetIterator();
	while (kIter.HasMoreElements())
	{
		CF_VIPPrivilegeCard::DataEntry* pkData = kIter.PeekNextValuePtr();
		if (pkData && pkData->_kType == type)
		{
			return pkData->_sDescriptionTips;
		}
		kIter.MoveNext();
	}
	return "";
}

StringType SC_ClientSkillInterface::GetVipPrivilegeDespTitle(int type)
{
	CF_VIPPrivilegeCard* pkVIPProfit = CF_VIPPrivilegeCard::GetSingletonPtr();
	CF_VIPPrivilegeCard::DataEntryMapExternIterator kIter = pkVIPProfit->GetIterator();
	while (kIter.HasMoreElements())
	{
		CF_VIPPrivilegeCard::DataEntry* pkData = kIter.PeekNextValuePtr();
		if (pkData && pkData->_kType == type)
		{
			return pkData->_sDescrip;
		}
		kIter.MoveNext();
	}
	return "";
}

int SC_ClientSkillInterface::GetShopIDShopType(int iShopID)
{
	CF_ShopList* pkShopList = CF_ShopList::GetSingletonPtr();
	if (pkShopList)
	{
		CF_ShopList::DataEntry* pkEntry = pkShopList->GetEntryPtr(iShopID);
		if (pkEntry)
			return pkEntry->_iShopType;
	}
	return -1;
}

StringType SC_ClientSkillInterface::GetEquipUpgradeCompositionInfo(int iFormulaType)
{
	StringStreamType sStream;
	StringType sTransIDs, sDropNums;

	StringType sMaterial;
	sMaterial.reserve(32);

	CF_CompositionFormula::DataEntryMapExternIterator kIter = CF_CompositionFormula::GetSingleton().GetIterator();
	for (;kIter.HasMoreElements();kIter.MoveNext())
	{
		const CF_CompositionFormula::DataEntry *pkData = kIter.PeekNextValuePtr();
		if(pkData->_FormulaType == iFormulaType)
		{
			sMaterial.clear();
			FormatDataIFs2String(pkData->_kNeedMaterials, sMaterial);

			sStream << pkData->_iID << '#' << sMaterial << '#' << pkData->_lYangItemID << '#';

			sTransIDs.clear();
			sDropNums.clear();

			int iDropNum = 0;
			if(!pkData->_kNeedMaterials.empty())
			{
				const uint32_t iItemID = pkData->_kNeedMaterials.front().ID();
				const CF_EquipUpgrade::DataEntry *pkUpgrade = SafeGetCSVFileDataEntryPtr<CF_EquipUpgrade>(iItemID);
				if(pkUpgrade != NULL)
				{
					sTransIDs = pkUpgrade->_sTranscriptionRuleID;
					sDropNums = pkUpgrade->_sItemDropNumber;
				}
			}
			sStream << sTransIDs << '#' << sDropNums  << '#';
			sStream << '|';
		}
	}

	return sStream.str();
}

int SC_ClientSkillInterface::GetNxtEquipUpgradeItemID(int iItemID, int iFormulaType)
{
	CF_CompositionFormula::DataEntryMapExternIterator kIter = CF_CompositionFormula::GetSingleton().GetIterator();
	for (;kIter.HasMoreElements();kIter.MoveNext())
	{
		const CF_CompositionFormula::DataEntry *pkData = kIter.PeekNextValuePtr();
		if(pkData->_lYangItemID == iItemID && pkData->_FormulaType == iFormulaType)
		{
			return pkData->_lCompositionItemID;
		}
	}

	return 0;
}

int32_t SC_ClientSkillInterface::GetItemCompositionFuncIDByType(int iItemID, int iType)
{
	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	if (NULL == pkItemFactory)
	{
		return 0;
	}
	CF_CompositionFormula* pkCSVCompositionList = CF_CompositionFormula::GetSingletonPtr();
	if (pkCSVCompositionList == NULL)
	{
		return 0;
	}
	LC_ItemBase* pkItem = pkItemFactory->RequestItem(iItemID);
	if( pkItem )
	{
		const mem::vector<int32_t>& VecFuncID = pkItem->GetItemDescriptionEntry()->GetCompositionFuncIDs();
		for (mem::vector<int32_t>::const_iterator it = VecFuncID.begin(); it != VecFuncID.end(); ++it)
		{
			CF_CompositionFormula::DataEntry* pkCompositionData = pkCSVCompositionList->GetEntryPtr(*it);
			if (pkCompositionData && pkCompositionData->_FormulaType == iType)
			{
				return *it;
			}
		}
	}
	return 0;
}

int32_t	  SC_ClientSkillInterface::GetItemDeCompositionFuncIDByType(int iItemID)
{
	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	if (NULL == pkItemFactory)
	{
		return 0;
	}
	CF_ItemDecompositionFunc* pkCSVDecomposition = CF_ItemDecompositionFunc::GetSingletonPtr();
	if (pkCSVDecomposition == NULL)
	{
		return 0;
	}
	LC_ItemBase* pkItem = pkItemFactory->RequestItem(iItemID);
	if( pkItem)
	{
		CF_ItemDecompositionFunc::DataEntry* pkDrCompositionData = pkCSVDecomposition->GetEntryPtr(pkItem->GetDecompositionFuncID());
		if (pkDrCompositionData)
		{
			return pkItem->GetDecompositionFuncID();
		}
	}
	return 0;
}

int SC_ClientSkillInterface::GetHookStatePosY()
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)LC_ClientLogicManager::GetSingletonPtr();
	LC_ClientGamePlayer* pkPlayer = pkLogicManger->GetOperationPlayer();
	if (NULL == pkPlayer)
		return 0;

	return UE_UiEffectManager::GetSingleton().GetPlayerHight(pkPlayer->GetID()).y;
}

int SC_ClientSkillInterface::GetYunyingTxSubPlatform()
{
	return getTxSubplatform();
}

int SC_ClientSkillInterface::GetBaseLevelExp(int iLevel)
{
	return LC_Helper::BaseLevelExp(iLevel);
}

int SC_ClientSkillInterface::GetForceBattleMapIndex(int lMapResID)
{
	CF_ForcesBattle::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_ForcesBattle>(lMapResID);
	if (pkData)
	{
		return pkData->_iLineID;
	}
	return -1;
}

int SC_ClientSkillInterface::GetSkillLevel(skill_id_type id)
{
	CF_SkillSpecialAttrib::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_SkillSpecialAttrib>(id);
	if (pkData)
	{
		return pkData->_lCurLevel;
	}
	return 0;
}

int SC_ClientSkillInterface::GetPlayerMaxLevel()
{
	CF_LevelUp* pkCSVLevelUp = CF_LevelUp::GetSingletonPtr();
	if (NULL == pkCSVLevelUp)
	{
		return 200;//这里先设定为200级
	}
	return pkCSVLevelUp->GetEntryCount();
}

int SC_ClientSkillInterface::GetFashionSuitCount()
{
	CF_FashionSuit *pkSuitList = CF_FashionSuit::GetSingletonPtr();
	if(NULL == pkSuitList)
		return 0;

	return pkSuitList->GetEntryCount();
}
double SC_ClientSkillInterface::GetProfileTimeStamp()
{
	static bool bFirst = true;
	static LARGE_INTEGER freq;
	static LARGE_INTEGER initial;

	if (bFirst)
	{
		QueryPerformanceFrequency(&freq);
		QueryPerformanceCounter(&initial);
		bFirst = false;
	}

	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	return ((double)
		(counter.QuadPart - initial.QuadPart) / 
		(double) freq.QuadPart);
}

StringType SC_ClientSkillInterface::DecodePlatformVipInfo(int formatvip)
{
	uint8_t platinfo = static_cast<uint8_t>(formatvip);
	int is_vip = (platinfo >> TENCENT_PF_VIP_IS) & 0x1;
	int vip_level = (platinfo >> TENCENT_PF_VIP_LEVEL) & TENCENT_PF_VIP_LEVEL_MASK;
	int is_year_vip = platinfo >> TENCENT_PF_VIP_YEAR & 0x1;
	int is_high_vip = platinfo >> TENCENT_PF_VIP_HIGH & 0x1;
	int is_expand_vip = platinfo >> TENCENT_PF_VIP_EXPAND & 0x1;
	StringStreamType sStream;

	sStream << is_vip << "#" << vip_level << "#" << is_year_vip << "#" << is_high_vip << "#" << is_expand_vip << "#";
	return sStream.str();
}

StringType SC_ClientSkillInterface::GetProcessName()
{
	char process_path[1024] = {0};
	GetModuleFileName(0, process_path, 1024);
	return StringType(process_path);
	
}

void SC_ClientSkillInterface::LuaShellExecute(const StringType& lpOperation, const StringType& lpFile, const StringType&  lpParameters, const StringType& lpDirectory, int nShowCmd)
{
	ShellExecute(0, lpOperation.c_str(), lpFile.c_str(), lpParameters.c_str(), lpDirectory.c_str(), nShowCmd);
}

int SC_ClientSkillInterface::GetGuildBattleTaskOpenSchedule(int iTabIdx)
{
	CF_BestGuildChapterID::DataEntry* pkEntry = SafeGetCSVFileDataEntryPtr<CF_BestGuildChapterID>(iTabIdx);
	if (pkEntry)
	{
		return pkEntry->_iScheduleID;
	}
	return 0;
}

bool SC_ClientSkillInterface::CheckLocation(int lID,int iDistance)
{
	CF_WorldObjectDesc::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_WorldObjectDesc>(lID);
	if (NULL == pkData)
		return false;
	mem::vector<UT_Vec3Int> kLocVec;
	LC_Helper::SpliteString2MultiVec3Int(pkData->_kExtLocation, kLocVec);
	kLocVec.push_back(pkData->_kLocation);
	UT_Vec3Int &Loc = kLocVec[0];
	LC_ClientGamePlayer* pPlayer = GetPlayerPtr();
	if (NULL ==  pPlayer)
	{
		return false;
	}
	//距离太近就不用飞鞋，走过去
	if(UT_MathBase::LineLengthXYInt(pPlayer->GetCurrentLocation(), Loc) < iDistance )
	{
		return true;
	}
	return false;
}

int SC_ClientSkillInterface::GetGameServerInfoIndex(int iServerID, int iIndex)
{
	NW_ClientSessionManager* pkSessionManager = NW_ClientSessionManager::GetSingletonPtr();
	if(!pkSessionManager)
		return false;

	return pkSessionManager->GetGameServerInfoIndex(iServerID, iIndex);
}

StringType SC_ClientSkillInterface::GetActivityRewardInfo(int32_t ActivityType, int32_t iCatagory, int32_t iSchedule, int32_t ikey)
{
	LC_ClientActivityManager *pkActivityManager = LC_ClientActivityManager::GetSingletonPtr();

	ACT_RewardDataEntry *RewardEntry = pkActivityManager->GetRewardDataEntry(ActivityType, iCatagory, iSchedule, ikey);
	if (RewardEntry == NULL)
	{
		return "";
	}

	const Utility::UT_SIMDataList* pkItems = RewardEntry->m_kRewardItems.GetAllItemList();

	StringStreamType sStream;
	sStream << _getRewardInfo(GetPlayerPtr(), *pkItems) << Int2String(RewardEntry->m_bHighReward);
	return sStream.str();
}

StringType SC_ClientSkillInterface::GetActivityRewardInfoWithChoice(int32_t ActivityType, int32_t iCatagory, int32_t iSchedule, int32_t ikey, bool bProbability, int32_t iChoice)
{
	LC_ClientActivityManager *pkActivityManager = LC_ClientActivityManager::GetSingletonPtr();

	ACT_RewardDataEntry *RewardEntry = pkActivityManager->GetRewardDataEntry(ActivityType, iCatagory, iSchedule, ikey);
	if (RewardEntry == NULL)
	{
		return "";
	}
	const Utility::UT_SIMDataList* pkItems = RewardEntry->m_kRewardItems.GetAllItemList();
	return _getRewardInfo(GetPlayerPtr(), *pkItems, bProbability, iChoice);
}

int SC_ClientSkillInterface::GetGameStoryCurrentMapResID(int32_t iGameStoryID)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if(pkLogicManager == NULL)
		return 0; 

	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if(pkGamePlayer == NULL)
		return 0;

	mem::vector<int32_t> kMapResIDs;
	CF_GameStoryRaidList::DataEntryMapExternIterator kIter = CF_GameStoryRaidList::GetSingletonPtr()->GetIterator();
	for( ; kIter.HasMoreElements(); kIter.MoveNext())
	{
		const CF_GameStoryRaidList::DataEntry *pkData = kIter.PeekNextValuePtr();
		if(pkData->_lBelongStoryID == iGameStoryID)
		{
			kMapResIDs.push_back(pkData->_MapResID);
		}
	}

	std::sort(kMapResIDs.begin(), kMapResIDs.end());

	int iResult = 0;
	for(mem::vector<int32_t>::iterator it = kMapResIDs.begin(); it != kMapResIDs.end(); ++it)
	{
		if(pkGamePlayer->GetRaidScore(*it) == -1)
			break;
		
		iResult = *it;
	}

	return iResult;
}

StringType SC_ClientSkillInterface::GetGameStoryBossInfo(int iMapResID)
{
	CF_GameStoryRaidList::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_GameStoryRaidList>(iMapResID);
	if(pkData == NULL)
		return "";

	CF_CharType::DataEntry *pkCharData = SafeGetCSVFileDataEntryPtr<CF_CharType>(pkData->_lCharType);
	if(pkCharData == NULL)
		return "";

	StringStreamType sStream;
	sStream << pkCharData->_kTalkHeadIcon << '#' << pkCharData->_lLevel << '#' << pkCharData->_kCharName;
	return sStream.str();
}

StringType SC_ClientSkillInterface::_getRewardInfo(LC_ClientGamePlayer* pkGamePlayer, const Utility::UT_SIMDataList &ItemList, bool bProbability, int32_t iChoice)
{
	if (!pkGamePlayer)
		return "";
	int eWeaponType = pkGamePlayer->GetWeaponAttribute();
	StringStreamType sStream("");

	StringType sContent = "";
	int iCount = 0;
	for(Utility::UT_SIMDataList::const_iterator it = ItemList.begin(); it != ItemList.end(); ++it)
	{
		int32_t WItemSection = it->ItemSection();
		if (it->ItemChoice() == iChoice && ((it->ItemProbability() && bProbability) || (!it->ItemProbability() && !bProbability))  && (WItemSection == eWeaponType || WItemSection == WT_NONE || WItemSection == WT_HAND ))
		{
			sContent +=  Utility::Int2String(it->ID()) + "_" + Utility::Int2String(it->ParamA()) + "_" + Utility::Int2String(it->ItemExistenceFunc()) + "_" + Utility::Int2String(it->ItemReinfore()) + "_" + Utility::Int2String(it->ItemBindMode() > 0 ? 1 : 0) + "#";
			iCount++;
		}
	}
	sStream << Utility::Int2String(iCount) << "#" << sContent;

	return sStream.str();
}

StringType SC_ClientSkillInterface::_getChoiceInfo(LC_ClientGamePlayer* pkGamePlayer, const Utility::UT_SIMDataList &ItemList)
{
	if (!pkGamePlayer)
		return "";
	int eWeaponType = pkGamePlayer->GetWeaponAttribute();
	StringStreamType sStream("");

	std::map<int,int> equipMap;
	StringType sContent = "";
	int iCount = 0;
	for(Utility::UT_SIMDataList::const_iterator it = ItemList.begin(); it != ItemList.end(); ++it)
	{
		int32_t iChoice = it->ItemChoice();
		std::map<int,int>::const_iterator iter = equipMap.find(iChoice);
		if(iter == equipMap.end())
		{
			equipMap[iChoice] = iChoice;
			sContent +=  Utility::Int2String(it->ItemChoice()) + "_";
			iCount++;
		}
	}
	sStream << Utility::Int2String(iCount) << "#" << sContent;

	return sStream.str();
}

//秘笈相关
//iPos = -1 表示所有子节点的属性总和
StringType SC_ClientSkillInterface::GetSkillBookAttribs(int iSkillBookID, int iPos)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if(pkLogicManager == NULL)
		return ""; 

	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if(pkGamePlayer == NULL)
		return "";

	LC_SkillBookData* pkData = pkGamePlayer->GetSkillBookAsset().GetSkillBookData(iSkillBookID);

	LC_ActorAttributeMap attrMap;
	if(iPos == -1)
	{
		CF_SkillBookInitAttribute::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_SkillBookInitAttribute>(iSkillBookID);
		if(pkEntry != NULL)
		{
			LC_Helper::AttributeMapByAttrFunc(&attrMap, pkEntry->_iInitAttr);
		}

		if(pkData != NULL)
		{
			mem::vector<LC_SkillBookData::SkillBookData>& skillBookData = pkData->GetSkillBookData();
			mem::vector<LC_SkillBookData::SkillBookData>::const_iterator iter = skillBookData.begin();
			for(; iter != skillBookData.end(); ++iter)
			{
				_getSkillBookAttribs(&attrMap, iSkillBookID, (*iter).m_iPos, (*iter).m_iLevel);
			}
		}
	}
	else
	{
		if(pkData != NULL)
		{
			int iLevel = pkData->GetNodeLevel(iPos);
			_getSkillBookAttribs(&attrMap, iSkillBookID, iPos, iLevel);
		}
	}

	attrMap.RecomputeAll();
	return _getAttribString(&attrMap);
}

StringType SC_ClientSkillInterface::GetSkillBookAttribsByNodeLevel(int iSkillBookID,int iPos, int iLevel)
{
	LC_ActorAttributeMap attrMap;
	_getSkillBookAttribs(&attrMap, iSkillBookID, iPos, iLevel);
	attrMap.RecomputeAll();

	return _getAttribString(&attrMap);
}

StringType SC_ClientSkillInterface::GetSkillBookItemChangeAttribs(int iSkillBookID,int iLevel)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if(pkLogicManager == NULL)
		return "";

	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if(pkGamePlayer == NULL)
		return "";

	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	if(NULL == pkItemFactory)
		return "";

	LC_SkillBookDataEntry* entry = LC_SkillBookManager::GetSingleton().GetSkillBookDataEntry(iSkillBookID,iLevel);
	if (NULL == entry)
	{
		return "";
	}

	StringStreamType sStream("");

	LC_ActorAttributeMap attrMap;
	for(size_t i=0; i<entry->_PropTimes.size(); ++i)
	{
		if(iLevel >= (i+1))
		{
			for(int j=0; j<entry->_PropTimes[i]; ++j)
				LC_Helper::AttributeMapByAttrFunc(&attrMap, entry->_kModifyPropertyID);
		}
	}

	attrMap.RecomputeAll();
	return _getAttribString(&attrMap);
}

int64_t SC_ClientSkillInterface::GetSkillBookCombatScore(int iSkillBookID, int iLevel)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if(pkLogicManager == NULL)
		return 0;

	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if(pkGamePlayer == NULL)
		return 0;

	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	if(NULL == pkItemFactory)
		return 0;

	LC_SkillBookDataEntry* entry = LC_SkillBookManager::GetSingleton().GetSkillBookDataEntry(iSkillBookID,iLevel);
	if (NULL == entry)
	{
		return 0;
	}

	LC_ActorAttributeMap attrMap;
	for(size_t i=0; i<entry->_PropTimes.size(); ++i)
	{
		if(iLevel >= (i+1))
		{
			for(int j=0; j<entry->_PropTimes[i]; ++j)
				LC_Helper::AttributeMapByAttrFunc(&attrMap, entry->_kModifyPropertyID);
		}
	}

	attrMap.RecomputeAll();

	return LC_Helper::GetAttributeMapCombatScore(&attrMap);
}

ResultType SC_ClientSkillInterface::CheckSkillBookUpgrade(int iSkillBookID,int iPos)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if(pkLogicManager == NULL)
		return RE_FAIL;

	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if(pkGamePlayer == NULL)
		return RE_FAIL;

	LC_SkillBookData* pkData = pkGamePlayer->GetSkillBookAsset().GetSkillBookData(iSkillBookID);
	if(NULL == pkData)
		return RE_FAIL;

	int iNextLevel = 0;
	return pkData->CheckUpgrade(pkGamePlayer, iPos, iNextLevel);
}

StringType SC_ClientSkillInterface::GetCimeliaInitAttribInfo(int iType)
{
	int iAttribFuncID = 0;
	CF_SubordinateTransform::DataEntryMapExternIterator kIter = CF_SubordinateTransform::GetSingletonPtr()->GetIterator();
	for( ; kIter.HasMoreElements(); kIter.MoveNext() )
	{
		const CF_SubordinateTransform::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		if(pkEntry->_iType == iType && pkEntry->_iSubType == 0 && pkEntry->_iLevel == 1)
		{
			iAttribFuncID = pkEntry->_iAttrFunc;
			break;
		}
	}

	if(iAttribFuncID > 0)
	{
		LC_ActorAttributeMap attrMap;
		LC_Helper::AttributeMapByAttrFunc(&attrMap, iAttribFuncID);
		attrMap.RecomputeAll();
		return _getAttribString(&attrMap);
	}

	return "";
}

StringType SC_ClientSkillInterface::GetCimeliaTransformAttribInfo(int iTransformID)
{
	CF_SubordinateTransform::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_SubordinateTransform>(iTransformID);
	if(pkData == NULL)
		return "";

	LC_ActorAttributeMap attrMap;
	LC_Helper::AttributeMapByAttrFunc(&attrMap, pkData->_iAttrFunc);
	attrMap.RecomputeAll();
	return _getAttribString(&attrMap);
}

StringType SC_ClientSkillInterface::GetCimeliaTransformElementAttribInfo(int iTransformID)
{
	CF_SubordinateTransform::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_SubordinateTransform>(iTransformID);
	if(pkData == NULL)
		return "";

	UT_SIMDataList::const_iterator iter = pkData->Attribute.begin();
	if(iter == pkData->Attribute.end())
		return "";

	LC_ActorAttributeMap attrMap;

	int iType = iter->ParamA();
	if(iType == 2 || iType == 4)
	{
		attrMap.SetAttributePercentFunc(iter->ParamB(), iter->ParamC());
		attrMap.RecomputeAll();

		return _getPercentAttribString(&attrMap);
	}
	else
	{
		attrMap.SetAttributeValueFunc(iter->ParamB(), iter->ParamC());
		attrMap.RecomputeAll();

		return _getAttribString(&attrMap);
	}
}


int SC_ClientSkillInterface::CheckUseItemOpenSkillBook(GameLogic::LC_ActorBase* pkScriptCaller, int32_t iID)
{
	//服务器端实现
	return 0;
}

combat_score_type SC_ClientSkillInterface::GetSubordinateTransformCombatScoreByItemId(int iItemId)
{
	CF_SubordinateTransform::DataEntryMapExternIterator kIter = CF_SubordinateTransform::GetSingleton().GetIterator();
	for (;kIter.HasMoreElements();kIter.MoveNext())
	{
		const CF_SubordinateTransform::DataEntry *pkData = kIter.PeekNextValuePtr();
		if(pkData->_kItems.size()>0)
		{
			const Utility::UT_SIMDataInfo pInfo=pkData->_kItems[0];
			int itemId=pInfo.ID();
			if(itemId==iItemId)
			{
				LC_ActorAttributeMap attrMap;
				attrMap.Clear();
				LC_Helper::AttributeMapByAttrFunc(&attrMap, pkData->_iAttrFunc);
				attrMap.RecomputeAll();
				combat_score_type AddScore = LC_Helper::CalcAttributeMapCombatScore(&attrMap, false);
				return AddScore;
			}
		}
	}
	return 0;
}

StringType SC_ClientSkillInterface::GetAllSkillBooks()
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if(pkGamePlayer == NULL)
	{
		return "";
	}

	StringStreamType sStream("");
	const SkillBookDataMap& dataMap = pkGamePlayer->GetSkillBookAsset().GetSkillBookDatas();
	SkillBookDataMap::const_iterator it = dataMap.begin();
	for (; it != dataMap.end(); ++it)
	{
		sStream << it->first << "#";
	}

	return sStream.str();
}

StringType SC_ClientSkillInterface::GetEverySkillBooks()
{
	StringStreamType sStream;
	CF_SkillBookInitAttribute::DataEntryMapExternIterator it = CF_SkillBookInitAttribute::GetSingletonPtr()->GetIterator();
	for(; it.HasMoreElements(); it.MoveNext())
	{
		const CF_SkillBookInitAttribute::DataEntry* pkEntry = it.PeekNextValuePtr();
		sStream << pkEntry->_iSkillBookID << '#' << pkEntry->_sName << '#' << pkEntry->_sImg << '#' << pkEntry->_CharTypeID << '#';
		sStream << pkEntry->_kSkills << '#' << pkEntry->_VIPLevel << '#' << (int)pkEntry->_iType << '#' << FormatComMap2String(pkEntry->_Cost, 1) << '#';
		sStream << pkEntry->_UnlockHeroStars << '#' << pkEntry->sequence << '|';
	}

	return sStream.str();
}

StringType SC_ClientSkillInterface::GetSkillBookGridInfo()
{
	StringStreamType sStream;
	CF_SkillBookUnlock::DataEntryMapExternIterator iter = CF_SkillBookUnlock::GetSingletonPtr()->GetIterator();
	for(; iter.HasMoreElements(); iter.MoveNext())
	{
		const CF_SkillBookUnlock::DataEntry *pkEntry = iter.PeekNextValuePtr();
		sStream << pkEntry->_index << "#" << pkEntry->_slotID << "#";
		sStream << pkEntry->_UnlockLevel << "#" << FormatComMap2String(pkEntry->_UnlockCost, 1) << "|";
	}

	return sStream.str();
}

StringType SC_ClientSkillInterface::GetSkillBookNodeInfo(int iID)
{
	return LC_SkillBookManager::GetSingleton().GetSkillBookNodeInfo(iID);
}

StringType SC_ClientSkillInterface::GetSkillBookInfo(int iID)
{
	CF_SkillBookInitAttribute::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_SkillBookInitAttribute>(iID);
	if(pkEntry == NULL)
		return "";
	
	StringStreamType sStream("");
	sStream << pkEntry->_sName << "#" << pkEntry->_sImg << "#" <<pkEntry->_CharTypeID << "#" << pkEntry->_kSkills << "#" << pkEntry->_iInitAttr << "#" << pkEntry->_sStoryLocked << "#" << pkEntry->_sStoryUnlocked;
	return sStream.str();
}

StringType SC_ClientSkillInterface::GetCategorySkills(int ilCategory1)
{
	CF_SkillExecuteList *pkCSVSkillExecute = CF_SkillExecuteList::GetSingletonPtr();
	CF_SkillExecuteList::DataEntryMapExternIterator iter = pkCSVSkillExecute->GetIterator();

	StringStreamType sStream("");
	for( ; iter.HasMoreElements(); iter.MoveNext())
	{
		CF_SkillExecuteList::DataEntry *pkData = iter.PeekNextValuePtr();

		if(pkData->_lCategory1 == SKILL_TYPE_SUB_ACTOR_NINEMENKF)
		{
			sStream<<pkData->_iID<<"#";
		}
	}

	return sStream.str();
}

void	   SC_ClientSkillInterface::ClickOnScreen(int posX,int PosY)
{
	SendMessage(CreateMsg_MOUSE_MSG(MGT_INPUT_MOUSE_LBUTTON_DOWN,posX,PosY,(long)1),true);
}

void SC_ClientSkillInterface::PlayVipLevelupFx(int iVipLevel)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if(pkGamePlayer != NULL)
		SendMessage(CreateMsg_UE_VIP_LEVELUP(pkGamePlayer->GetID(), iVipLevel));
}

void SC_ClientSkillInterface::PlayVipNewLevelupFx(int iVipLevel)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if(pkGamePlayer != NULL)
		SendMessage(CreateMsg_UE_VIP_NEW_LEVELUP(pkGamePlayer->GetID(), iVipLevel));
}

bool SC_ClientSkillInterface::PlayArenaTimerFx()
{
	int iState = GetGameStage();
	if (iState != ST_GAME_STAGE_GAME)
		return false;

	FX_ClientFxManager& rkFxManager = FX_ClientFxManager::GetSingleton();

	int iFxID = rkFxManager.CreateFx(RES_FX_ARENATIMER, true);
	rkFxManager.PlayFx(iFxID, GET_PROCESS_TIME);
	return true;
}

bool SC_ClientSkillInterface::PlaySkyArenaTimerFx(bool bBig)
{
	int iState = GetGameStage();
	if (iState != ST_GAME_STAGE_GAME)
		return false;

	FX_ClientFxManager& rkFxManager = FX_ClientFxManager::GetSingleton();

	int iFxID = 0;
	if (bBig)
	{
		iFxID = rkFxManager.CreateFx(RES_FX_SKYARENATIMER, true);
	}
	else
	{
		iFxID = rkFxManager.CreateFx(RES_FX_SKYARENATIMERSMALL, true);
	}
	rkFxManager.PlayFx(iFxID, GET_PROCESS_TIME);

	return true;
}

StringType SC_ClientSkillInterface::GetGameStoryRaidRewardByMapresID(int iMapResID)
{
	CF_GameStoryRaidList::DataEntryMapExternIterator kIter = CF_GameStoryRaidList::GetSingletonPtr()->GetIterator();
	for( ; kIter.HasMoreElements(); kIter.MoveNext())
	{
		const CF_GameStoryRaidList::DataEntry *pkData = kIter.PeekNextValuePtr();
		if(pkData->_MapResID == iMapResID)
		{
			 return pkData->_Reward;
		}
	}
	return "";
}

void	   SC_ClientSkillInterface::FilterCronTimestampSameDay(const StringType& cronStr, mem::vector<uint32_t>& vecTime)
{
	if (cronStr.empty())
	{
		return;
	}
	Utility::UT_CronData kCron;

	if(kCron.ParseCronStr(cronStr))
	{
		LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
		LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
		uint64_t iSystemTime = 0;
		if (!pkGamePlayer)
			iSystemTime = GET_SYSTEM_TIME;
		else
			iSystemTime = pkGamePlayer->GetServerTimeVal();
		time_t cur = (time_t)(iSystemTime);
		tm *curLT = localtime ( &cur );
		if (iSystemTime != 0)
		{
			curLT->tm_hour = 0;
			curLT->tm_min = 0;
			curLT->tm_sec = 1;
		}
		time_t today = mktime(curLT);
		time_t p = today;

		do 
		{
			time_t t = kCron.GetNextTime(p);

			if (!LC_Helper::CheckIsSameDay((uint32_t)cur, (uint32_t)t))
			{
				break;
			}

			vecTime.push_back((uint32_t)t);
			p = t + 1;

		} while (1);
	}
}

StringType SC_ClientSkillInterface::GetNextActiveGameStoryTimeSlot(long lSchduleID)
{
	StringStreamType sStream(""); 
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager)
	{
		return sStream.str();
	}
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (NULL == pkGamePlayer)
	{
		return sStream.str();
	}	
	CF_ActivitySchedule& pkActSche = CF_ActivitySchedule::GetSingleton();
	const CF_ActivitySchedule::DataEntry* pkData = pkActSche.GetEntryPtr(lSchduleID);
	if (NULL != pkData)
	{
		mem::vector<uint32_t> kCronTime;
		kCronTime.clear();
		FilterCronTimestampSameDay(pkData->_sCron,kCronTime);
		if (!kCronTime.empty())
		{
			for (int j = 0; j < kCronTime.size(); j++)
			{
				uint32_t OpenTime = kCronTime[j];
				uint32_t EndTime = OpenTime + pkData->_iTimeLen * 60;

				int iSystemTime = 0;
				iSystemTime = (int)pkGamePlayer->GetServerTimeVal();
				if (iSystemTime < EndTime)
				{
					char strTemp[MAX_GAMESTORY_TIME_DESCRIPTION_LENGTH + 1];
					time_t bt = time_t(OpenTime);
					time_t et = time_t(EndTime);

					tm bLT = *localtime(&bt);
					tm bET = *localtime(&et);

					sprintf_s(strTemp,sizeof(strTemp),"%d:%02d - %d:%02d",bLT.tm_hour, bLT.tm_min, bET.tm_hour, bET.tm_min);

					sStream<<strTemp;
					break;
				}
			}
		}
	}
	return sStream.str();
}

int32_t SC_ClientSkillInterface::GetServerUnionID()
{
	return GER_SERVER_UNION_ID();
}

void SC_ClientSkillInterface::SetLockLevel(int level)
{
	// UT_ClientGlobalSettings此接口已作废
	/*UT_ClientGlobalSettings* pkClientSetting = UT_ClientGlobalSettings::GetSingletonPtr();
	if ( pkClientSetting)
	{
		pkClientSetting->SetLodLevel((UT_ClientGlobalSettings::LodLevel)level);	
	}*/
}

void SC_ClientSkillInterface::LockLockedLevel( bool IsLocked )
{
	// 有空换一下SC_ClientSkillInterface::LockLockedLevel这个接口的名称
	UT_ClientGlobalSettings* pkClientSetting = UT_ClientGlobalSettings::GetSingletonPtr();
	if ( pkClientSetting)
	{
		pkClientSetting->onEventSimpleMode(IsLocked);
	}
}

StringType SC_ClientSkillInterface::GetPeakArenaLevelByScore(int iScore)
{
	StringType tempDesc = "";
	static CSVFile::CF_ArenaSection* inst = SafeGetSingleton<CSVFile::CF_ArenaSection>();
	CSVFile::CF_ArenaSection::DataEntryMapExternIterator iter = inst->GetIterator();
	while(iter.HasMoreElements())
	{
		CSVFile::CF_ArenaSection::DataEntry* pkData = iter.PeekNextValuePtr();
		if (iScore >= pkData->_iScore)
		{
			tempDesc = pkData->_sDesc;
		}
		else
		{
			break;
		}
		iter.MoveNext();
	}
	return tempDesc;
}
combat_score_type  SC_ClientSkillInterface::GetPeakArenaSectionScore(combat_score_type iScore)
{
	combat_score_type tempScore = 0;
	CSVFile::CF_ArenaSection::DataEntry* pTempData = NULL;

	static CSVFile::CF_ArenaSection* inst = SafeGetSingleton<CSVFile::CF_ArenaSection>();
	CSVFile::CF_ArenaSection::DataEntryMapExternIterator iter = inst->GetIterator();
	while(iter.HasMoreElements())
	{
		CSVFile::CF_ArenaSection::DataEntry* pkData = iter.PeekNextValuePtr();
		if(NULL != pTempData)
		{
			if (iScore < pkData->_iScore)
			{
				break;
			}
			else
			{
				pTempData = pkData;
			}
		}
		if (NULL == pTempData && pkData->_iScore > 0)
		{
			pTempData = pkData;
		}
		iter.MoveNext();
	}
	return pTempData->_iScore;
}

StringType SC_ClientSkillInterface::GetPeakLevelAttr(int level)
{
	CF_LevelUp* pkCSVLevelUp = CF_LevelUp::GetSingletonPtr();
	const CF_LevelUp::DataEntry* pkData = pkCSVLevelUp->GetEntryPtr(level);
	if (!pkData)
	{
		return "";
	}

	StringStreamType sStream("");
	sStream << pkData->_lMaxAtkRate <<"#"<<pkData->_lMaxDefRate<<"#"<<pkData->_lMaxHPRate<<"#"<<pkData->_lMFRate;
	return sStream.str();
}

combat_score_type SC_ClientSkillInterface::GetPercentAttrCombatScore(double hpPercent, double ackPercent, double defPercent)
{
	double fGs = 0;
	fGs =LC_Helper::GetBaseAttri(0,0,0,0,0,0,0,0,0,0,0,0,hpPercent,ackPercent,defPercent,0,0);
	return (combat_score_type)Utility::Round2Int(fGs);
}

StringType SC_ClientSkillInterface::GetStringMd5(const StringType& str)
{
	return STDSTR2TPSTR(getMd5Str(TPSTR2STDSTR(str)));
}

StringType SC_ClientSkillInterface::GetBoneSoulSkillIconAndTip(int iLevel, int iIndex, int iStar)
{
	StringStreamType sStream;

	if(iIndex < LC_BoneSoulAsset::BONE_SOUL_COMMON_SKILL_NUM && iStar == 0)
		iStar = 1;

	CF_BoneSoulSkill::DataEntryMapExternIterator kIter = CF_BoneSoulSkill::GetSingletonPtr()->GetIterator();
	while(kIter.HasMoreElements())
	{
		const CF_BoneSoulSkill::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		if(pkEntry->_iLevel == iLevel && pkEntry->_iIndex == iIndex && pkEntry->_iStar == iStar)
		{
			sStream << pkEntry->_kIcon << "#" << pkEntry->_kTip;
			break;
		}
		kIter.MoveNext();
	}

	return sStream.str();
}

StringType SC_ClientSkillInterface::GetBoneSoulCost(int iLevel, int iCount)
{
	StringStreamType sStream;
	CF_BoneSoulLevelUp::DataEntryMapExternIterator kIter = CF_BoneSoulLevelUp::GetSingletonPtr()->GetIterator();
	while(kIter.HasMoreElements())
	{
		const CF_BoneSoulLevelUp::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		if(pkEntry->_iLevel == iLevel)
		{
			if(iCount < pkEntry->_kCashCost.size())
				sStream << pkEntry->_kCashCost[iCount];
			else
				sStream << "0";
			sStream << "#" << pkEntry->_kCashCost.size() << "#" << pkEntry->_iYuanbaoCost;
			break;
		}
		kIter.MoveNext();
	}

	return sStream.str();
}

bool	SC_ClientSkillInterface::IsItemMelt(int16_t meltType,int lItemID)
{
	CF_ItemMelt* pkItemMelt = CF_ItemMelt::GetSingletonPtr();
	const CF_ItemMelt::DataEntry* pkData = pkItemMelt->GetEntryPtr(lItemID);
	if (NULL != pkData)
	{
		if (pkData->_kTypeLimit.empty()
			|| (pkData->_kTypeLimit[0].IID() != meltType 
			&& pkData->_kTypeLimit[0].ParamA() != meltType 
			&& pkData->_kTypeLimit[0].ParamB() != meltType))
		{
			return false;
		}
		return true;
	}
	return false;
}

int64_t	  SC_ClientSkillInterface::GetDateTimeStamp(int iSecond, int iMinute, int iHour, int iDay, int iMonth, int iYear)
{
	time_t nowTime = time(NULL);
	tm* timeTm = localtime(&nowTime);
	timeTm->tm_hour = iHour;
	timeTm->tm_min = iMinute;
	timeTm->tm_sec = iSecond;
	timeTm->tm_mday = iDay;
	timeTm->tm_mon = iMonth -1;
	timeTm->tm_year = iYear - 1900;
	time_t daytime = mktime(timeTm);
	return (int64_t)daytime;
}


int32_t	SC_ClientSkillInterface::CanStarsRune(int iCategory,int iIndex)
{
	LC_StarsRuneManager* pkStarsRuneMgr = LC_StarsRuneManager::GetSingletonPtr();
	if (NULL == pkStarsRuneMgr)
	{
		return RE_FAIL;
	}
	if (iCategory <= 0)
	{
		return RE_FAIL;
	}
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager)
	{
		return RE_FAIL;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (NULL == pkGamePlayer)
	{
		return RE_FAIL;
	}	

	int32_t iCurLevel = pkGamePlayer->GetStarsRuneAsset().GetStarsRuneLevel(iCategory, iIndex);

	CF_StarsRune::DataEntry* pTemEntry = NULL;
	CF_StarsRune::DataEntryMapExternIterator beg = CF_StarsRune::Instance()->GetIterator();
	while (beg.HasMoreElements())
	{
		CF_StarsRune::DataEntry* pEntry = beg.PeekNextValuePtr();
		if (pEntry != NULL && (int32_t)pEntry->_iCategory == iCategory && pEntry->_iIndex == iIndex && pEntry->_iLevel == iCurLevel+1)
		{
			pTemEntry = pEntry;
			break;
		}
		beg.MoveNext();
	}

	if (NULL == pTemEntry)
	{
		if (0 == iCurLevel)
		{
			return RE_FAIL;
		}
		else
		{
			return 2;
		}
	}

	bool hasItemEnough = true;
	Utility::UT_SIMDataList::iterator iterConsume = pTemEntry->_kConsume.begin();
	for (; iterConsume != pTemEntry->_kConsume.end(); ++iterConsume)
	{
		UT_SIMDataInfo& kItemInfo = *iterConsume;

		int itemID = kItemInfo.IID();
		int needCount = kItemInfo.ItemCount();
		int hasCount = pkGamePlayer->GetCanCostItemCount(itemID);
		if (hasCount < needCount)
		{
			hasItemEnough = false;
			break;
		}
	}
	if (hasItemEnough)
	{
		return RE_SUCCESS;
	}
	else
	{
		return 3;
	}
}

StringType SC_ClientSkillInterface::GetStarsRuneCost(int iCategory,int iIndex)
{
	StringStreamType sStream("");
	LC_StarsRuneManager* pkStarsRuneMgr = LC_StarsRuneManager::GetSingletonPtr();
	if (NULL == pkStarsRuneMgr)
	{
		return sStream.str();
	}
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager)
	{
		return sStream.str();
	}
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (NULL == pkGamePlayer)
	{
		return sStream.str();
	}	

	int32_t iCurLevel = pkGamePlayer->GetStarsRuneAsset().GetStarsRuneLevel(iCategory, iIndex);

	CF_StarsRune::DataEntry* pTemEntry = NULL;
	CF_StarsRune::DataEntryMapExternIterator beg = CF_StarsRune::Instance()->GetIterator();
	while (beg.HasMoreElements())
	{
		CF_StarsRune::DataEntry* pEntry = beg.PeekNextValuePtr();
		if ((int32_t)pEntry->_iCategory == iCategory && pEntry->_iIndex == iIndex && pEntry->_iLevel == iCurLevel+1)
		{
			pTemEntry = pEntry;
			break;
		}
		beg.MoveNext();
	}

	if (NULL == pTemEntry)
	{
		return sStream.str();
	}

	bool hasItemEnough = true;
	Utility::UT_SIMDataList::iterator iterConsume = pTemEntry->_kConsume.begin();
	for (; iterConsume != pTemEntry->_kConsume.end(); ++iterConsume)
	{
		UT_SIMDataInfo& kItemInfo = *iterConsume;

		int itemID = kItemInfo.IID();
		int needCount = kItemInfo.ItemCount();
		sStream<<itemID<<"#"<<needCount<<"#";
	}
	return sStream.str();
}

int	SC_ClientSkillInterface::GetChessMatrixActiveScheduleID()
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager)
	{
		return 0;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (NULL == pkGamePlayer)
	{
		return 0;
	}	
	LC_ClientActivityManager *pkActivityManager = LC_ClientActivityManager::GetSingletonPtr();

	CF_ACTMonopoly::DataEntryMapExternIterator beg = CF_ACTMonopoly::Instance()->GetIterator();
	while (beg.HasMoreElements())
	{
		CF_ACTMonopoly::DataEntry* pEntry = beg.PeekNextValuePtr();
		if (pkActivityManager->IsScheduleValid(pkGamePlayer,pEntry->_iSchedule))
		{
			return pEntry->_iSchedule;
		}
		beg.MoveNext();
	}

	CF_ACTMonopolyA::DataEntryMapExternIterator beg2 = CF_ACTMonopolyA::Instance()->GetIterator();
	while (beg2.HasMoreElements())
	{
		CF_ACTMonopolyA::DataEntry* pEntry = beg2.PeekNextValuePtr();
		if (pkActivityManager->IsScheduleValid(pkGamePlayer,pEntry->_iSchedule))
		{
			return pEntry->_iSchedule;
		}
		beg2.MoveNext();
	}
	return 0;
}

StringType SC_ClientSkillInterface::GetCoreInfo()
{
	StringStreamType sStream("");
	CF_StarVeinList::DataEntryMapExternIterator iter = CF_StarVeinList::GetSingletonPtr()->GetIterator();
	while(iter.HasMoreElements())
	{
		const CF_StarVeinList::DataEntry *pkEntry = iter.PeekNextValuePtr();
		sStream << pkEntry->id << "#";

		if(!pkEntry->StarVeinCost.empty())
		{	
			Utility::UT_SIMDataList::const_iterator iter = pkEntry->StarVeinCost.begin();
			sStream << iter->ID() <<"#" << iter->ParamA() <<"#";
		}
		else
		{
			sStream << "0#0#";
		}

		sStream << pkEntry->PreStarVein << "#" << pkEntry->_Position << "#";
		sStream << pkEntry->Page << "#" << pkEntry->_AttributePosition << "#";
		sStream << pkEntry->MapID << "#" <<pkEntry->_iBossCharType << "|";

		iter.MoveNext();
	}

	return sStream.str();
}

StringType SC_ClientSkillInterface::GetCoreGoalInfo()
{
	StringStreamType sStream;
	CF_StarVeinGoalList::DataEntryMapExternIterator iter = CF_StarVeinGoalList::GetSingletonPtr()->GetIterator();
	while(iter.HasMoreElements())
	{
		const CF_StarVeinGoalList::DataEntry *pkEntry = iter.PeekNextValuePtr();
		sStream << pkEntry->id << "#" << pkEntry->GoalInfo << "#";
		sStream << pkEntry->GoalCondition << "#" << pkEntry->GoalType << "#";
		sStream << pkEntry->PreGoal << "#";

		StringType sItem;
		FormatDataIFs2String(pkEntry->_sReward, sItem);
		sStream<< sItem << "|";

		iter.MoveNext();
	}

	return sStream.str();
}

StringType SC_ClientSkillInterface::GetCoreAttribs(int iID, bool bPercent)
{
	CF_StarVeinList::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_StarVeinList>(iID);
	if(pkData == NULL)
		return "";

	LC_ActorAttributeMap attrMap;

	for(int w=0; w<pkData->Attribute.size(); ++w)
	{
		Utility::UT_SIMDataInfo& info = pkData->Attribute[w];
		if(info.GetFlag() == 1)
		{
			attrMap.ModifyAttributeByPart(info.GetPart(), info.GetAttrID(), info.GetValue(), true);
		}
	}

	attrMap.RecomputeAll();
	if(bPercent)
		return _getPercentAttribString(&attrMap);
	else
		return _getAttribString(&attrMap);
}

StringType SC_ClientSkillInterface::GetCommanderTitleAttribs(int iID, bool bPercent)
{
	CF_CommanderTitle::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_CommanderTitle>(iID);
	if(pkData == NULL)
		return "";

	LC_ActorAttributeMap attrMap;

	for(int w=0; w<pkData->_sAttribute.size(); ++w)
	{
		Utility::UT_SIMDataInfo& info = pkData->_sAttribute[w];
		if(info.GetFlag() == 1)
		{
			attrMap.ModifyAttributeByPart(info.GetPart(), info.GetAttrID(), info.GetValue(), true);
		}
	}

	attrMap.RecomputeAll();
	if(bPercent)
		return _getPercentAttribString(&attrMap);
	else
		return _getAttribString(&attrMap);
}

StringType SC_ClientSkillInterface::GetCommanderTitleName(int iID)
{
	const CF_CommanderTitle::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_CommanderTitle>(iID);
	if(pkData == NULL)
		return "";

	return pkData->_name;
}

bool SC_ClientSkillInterface::IsCommanderTitleCanShow(int iID)
{
	const CF_CommanderTitle::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_CommanderTitle>(iID);
	if(pkData == NULL)
		return false;

	return pkData->_bIsShow;
}

StringType SC_ClientSkillInterface::GetStigmasUnlockInfo(int iID)
{
	LC_ClientGamePlayer* pkGamePlayer = GetPlayerPtr();
	if(pkGamePlayer == NULL)
		return "";

	const CF_SkillBookInitAttribute::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_SkillBookInitAttribute>(iID);
	if(pkData == NULL)
		return "";

	//int iCount = pkGamePlayer->GetSkillBookAsset().GetSkillBookCount();
	//int itemID = GetGlobalSetting.consumeItemID;
	//int itemCount = GetGlobalSetting.cosumeCountInit + iCount*GetGlobalSetting.cosumeCountPlus;

	StringStreamType sStream;
	//sStream << iNeedLevel << '#' << itemID << '#' << itemCount;
	sStream << pkData->_UnlockLevel << '#' << pkData->_UnlockHeroStars << '#' << pkData->_UnlockRegDays;

	return sStream.str();
}

StringType SC_ClientSkillInterface::GetDrawCardRewardDisplay(int iShopType,int iScheduleID)
{
	StringStreamType sStream;
	CF_ACTLottery::DataEntryMapExternIterator iter = CF_ACTLottery::GetSingletonPtr()->GetIterator();
	for(; iter.HasMoreElements(); iter.MoveNext())
	{
		const CF_ACTLottery::DataEntry *pkEntry = iter.PeekNextValuePtr();
		if(pkEntry->_iSchedule==iScheduleID && pkEntry->_iCategory==iShopType)
		{
			sStream << pkEntry->_sRewardShow;
			break;
		}
	}
	return sStream.str();
}

int	SC_ClientSkillInterface::ScriptActionExecute(const StringType& scriptName,int value)
{
	int32_t iCnt = 0;
	SA_RT_TRIGGER_ACTION(int32_t, iCnt, scriptName, value);
	return iCnt;
}

StringType SC_ClientSkillInterface::FindAndFileterForbiddenText(const StringType& sInfo, int eType)
{
	StringType kOutput = sInfo;
	GUISys::getRichTextParse->FilterForbiddenText(kOutput, kOutput, (TT_TextFilter::TextFilterType)eType);
	return kOutput;
}

void SC_ClientSkillInterface::ShowGameUIByKeyName(const StringType& sKeyName)
{
	WT_WidgetManager::GetSingletonPtr()->ShowGameUIByKeyName(sKeyName);
}

void SC_ClientSkillInterface::ShowGameUIByKeyNameAndPath(const StringType& sKeyName, const StringType& sPath)
{
	WT_WidgetManager::GetSingletonPtr()->ShowGameUIByKeyNameAndPath(sKeyName, sPath);
}

void SC_ClientSkillInterface::DestroyGameUI(WT_Widget* pkWgt)
{
	WT_WidgetManager::GetSingletonPtr()->DestroyWidget(pkWgt);
}

int SC_ClientSkillInterface::GetBackPackEmptyEntry(int iPackType)
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(!pkGamePlayer)
		return -1;

	int iEntryIndex = -1;
	pkGamePlayer->GetPackAsset()->GetEmptyPackEntry(iPackType, iEntryIndex);
	return iEntryIndex;
}

StringType SC_ClientSkillInterface::_getBuddyCsvInfo(const CSVFile::CF_Buddy::DataEntry *pkEntry)
{
	StringStreamType sStream("");
	if(pkEntry)
	{
		sStream << pkEntry->_roleName << "#" << pkEntry->_quality << "#" << pkEntry->_qualityicon << "#";
		sStream << Int2String(pkEntry->_ielementdamagetype) << "#" << pkEntry->_jobicon << "#";
		sStream << pkEntry->_simg << "#" << pkEntry->_bigimg << "#";
		sStream << pkEntry->sequence << "#" << pkEntry->requirement << "#" << pkEntry->Consumption << "#" << Int2String(pkEntry->_cID) << "#";
		sStream << Int2String(pkEntry->_MaxLevel) << "#" << pkEntry->_modelweapon << "#" << pkEntry->_skillid << "#";
		sStream << pkEntry->_kUpgradeGiftimg << "#" << pkEntry->_kNewimg <<"#" << Int2String(pkEntry->_InitLevel) << "#";
		sStream << pkEntry->_kUpgradeGiftimgHead << "#" << pkEntry->_kUpgradeGiftimgReturn <<"#" << pkEntry->_iDefaultTransform << "#" << pkEntry->_sModelIcon << "#";
	}

	return sStream.str();
}

StringType SC_ClientSkillInterface::GetBuddyInfo(int iCid)
{
	CF_Buddy::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_Buddy>(iCid);
	return _getBuddyCsvInfo(pkEntry);
}

StringType SC_ClientSkillInterface::GetBuddyInfoList()
{
	StringStreamType sStream("");
	CF_Buddy::DataEntryMapExternIterator kIter = CF_Buddy::GetSingletonPtr()->GetIterator();
	sStream << CF_Buddy::GetSingletonPtr()->GetEntryCount() << "|";
	while(kIter.HasMoreElements())
	{
		CF_Buddy::DataEntry* pkData = kIter.PeekNextValuePtr();
		sStream << _getBuddyCsvInfo(pkData) << "|";
		kIter.MoveNext();
	}

	return sStream.str();
}

StringType SC_ClientSkillInterface::GetBuddyUpgradeInfo(int iCid, int iLevel, int iStar)
{
	if(iLevel == 0)
		return "";

	CF_Buddy::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_Buddy>(iCid);
	if(NULL == pkEntry)
		return "";
	CF_LevelUp::DataEntry *level =  SafeGetCSVFileDataEntryPtr<CF_LevelUp>(iLevel);
	StringStreamType sStream("");
	CF_BuddyUpgrade::DataEntryMapExternIterator kIter = CF_BuddyUpgrade::GetSingletonPtr()->GetIterator();
	while(kIter.HasMoreElements())
	{
		CF_BuddyUpgrade::DataEntry* pkData = kIter.PeekNextValuePtr();
		if(pkData->_cID == iCid && pkData->_level == iStar)
		{
			sStream << pkData->_needitem << "#" <<pkData->_needgold << "#";
			sStream << _getBuddyAttrib(pkEntry->_lMaxHP, level->_lMaxHPAdd, pkData->_lMaxHP) << "#";
			sStream << _getBuddyAttrib(pkEntry->_lMaxPHA, level->_lMaxATKAdd, pkData->_lMaxPHA) << "#";
			sStream << _getBuddyAttrib(pkEntry->_lPHD, level->_lPHDAdd, pkData->_lPHD) << "#";
			sStream << _getBuddyAttrib(pkEntry->_lATKRateAdd, level->_lATKRateAdd, pkData->_lATKRateAdd) << "#";
			sStream << _getBuddyAttrib(pkEntry->_lDogRATE, level->_lDogRateAdd, pkData->_lDogRATE) << "#";
			sStream << _getBuddyAttrib(pkEntry->_lCirRATE, level->_lCirRateAdd, pkData->_lCirRATE) << "#";
			sStream << _getBuddyAttrib(pkEntry->_lCirDefAdd, level->_lCirDefAdd, pkData->_lCirDefAdd) << "#";
			sStream << Int2String(pkData->_iBuddyMaxLevel);
			break;
		}

		kIter.MoveNext();
	}

	return sStream.str();
}

StringType SC_ClientSkillInterface::GetBuddyTalent(int iCid)
{
	CF_Buddy::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_Buddy>(iCid);
	if(NULL == pkEntry)
		return "";

	CF_BuddyUpgrade::DataEntryMapExternIterator kIter = CF_BuddyUpgrade::GetSingletonPtr()->GetIterator();
	while(kIter.HasMoreElements())
	{
		const CF_BuddyUpgrade::DataEntry* pkData = kIter.PeekNextValuePtr();
		if(pkData->_cID == iCid)
		{
			if(pkData->_unlockskill > 0)
			{
				StringStreamType sStream("");
				sStream << (int)pkData->_level << "#" << pkData->_unlockskill;
				return sStream.str();
			}
		}

		kIter.MoveNext();
	}

	return "";
}

int SC_ClientSkillInterface::GetBuddyMaxLevel(int iCid, int iStar)
{
	int iCurMaxLv = 0;
	int iNxtMaxLv = 0;

	CF_BuddyUpgrade::DataEntryMapExternIterator kIter = CF_BuddyUpgrade::GetSingletonPtr()->GetIterator();
	while(kIter.HasMoreElements())
	{
		const CF_BuddyUpgrade::DataEntry* pkData = kIter.PeekNextValuePtr();
		if(pkData->_cID == iCid)
		{
			if(pkData->_level == iStar)
			{
				iCurMaxLv = pkData->_iBuddyMaxLevel;
			}
			else if(pkData->_level > iStar && pkData->_iBuddyMaxLevel > iCurMaxLv)
			{
				iNxtMaxLv = pkData->_level;
				break;
			}
		}

		kIter.MoveNext();
	}

	return iNxtMaxLv*1000 + iCurMaxLv;
}

StringType SC_ClientSkillInterface::GetBuddyAttributeShow(int iCid, int iStar)
{
	CF_BuddyUpgrade::DataEntryMapExternIterator kIter = CF_BuddyUpgrade::GetSingletonPtr()->GetIterator();
	while(kIter.HasMoreElements())
	{
		const CF_BuddyUpgrade::DataEntry* pkData = kIter.PeekNextValuePtr();
		if(pkData->_cID == iCid && pkData->_level == iStar)
		{
			return pkData->Zhanshi;
		}

		kIter.MoveNext();
	}
	return "";
}

int SC_ClientSkillInterface::GetBuddySpareItemID(int iCid, int iStar)
{
	CF_BuddyUpgrade::DataEntryMapExternIterator kIter = CF_BuddyUpgrade::GetSingletonPtr()->GetIterator();
	while(kIter.HasMoreElements())
	{
		const CF_BuddyUpgrade::DataEntry* pkData = kIter.PeekNextValuePtr();
		if(pkData->_cID == iCid && pkData->_level == iStar)
		{
			return pkData->_SpareItems;
		}
		kIter.MoveNext();
	}

	return 0;
}

int SC_ClientSkillInterface::GetActiveSkillItemID(int iSkillID)
{
	LC_ClientItemFactory* pkItemFactory = (LC_ClientItemFactory*)LC_ItemFactoryBase::GetSingletonPtr();
	if(NULL == pkItemFactory)
		return 0;
	
	return pkItemFactory->GetActiveSkillItemID(GET_SKILL_INDEX(iSkillID));
}

StringType SC_ClientSkillInterface::GetBuddyChainOpenInfo()
{
	StringStreamType sStream;

	CF_SoulConnect* pkCsv = CF_SoulConnect::GetSingletonPtr();
	sStream << pkCsv->GetEntryCount() << '|';

	CF_SoulConnect::DataEntryMapExternIterator iter = pkCsv->GetIterator();
	for(; iter.HasMoreElements(); iter.MoveNext())
	{
		const CF_SoulConnect::DataEntry *pkData = iter.PeekNextValuePtr();
		sStream << pkData->_id << '#' << pkData->_iLevel << '#' << pkData->_iTaskID << '#';
		sStream << pkData->_iCommanderTitleLevel << '|';
	}

	return sStream.str();
}

int SC_ClientSkillInterface::GetBuddyGridUnlockLevel(int index)
{
	CF_BuddyUnlock::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_BuddyUnlock>(index);
	if(NULL == pkEntry)
		return 0;

	return pkEntry->_UnlockLevel;
}

StringType SC_ClientSkillInterface::GetGuildLevelRewards()
{
	StringStreamType sStream;
	CF_GuildLevel::DataEntryMapExternIterator iter = CF_GuildLevel::GetSingletonPtr()->GetIterator();
	for(; iter.HasMoreElements(); iter.MoveNext())
	{
		const CF_GuildLevel::DataEntry *pkEntry = iter.PeekNextValuePtr();
		sStream << pkEntry->_sReward << '|';
	}

	return sStream.str();
}

void SC_ClientSkillInterface::PlayFailTaskUeFx()
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(pkGamePlayer == NULL)
		return;

	MG_MessageCenter* pkMessageCenter = MG_MessageCenter::GetSingletonPtr();
	pkMessageCenter->SendMessage(CreateMsg_UE_FAIL_TASK(pkGamePlayer->GetID()));
}

void SC_ClientSkillInterface::PlayTaskChapterOpenFx(int iChapter)
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManger->GetOperationPlayer();
	if(pkGamePlayer == NULL)
		return;

	CF_TaskChapters::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_TaskChapters>(iChapter);
	if(pkEntry == NULL)
		return;

	MG_MessageCenter* pkMessageCenter = MG_MessageCenter::GetSingletonPtr();
	pkMessageCenter->SendMessage(CreateMsg_UE_TASKCHAPTER_OPEN(pkGamePlayer->GetID(), iChapter));
}

StringType SC_ClientSkillInterface::GetRuneInfo(int iItemID)
{
	CF_RuneList::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_RuneList>(iItemID);
	if(pkEntry == NULL)
		return "";

	StringStreamType sStream;
	sStream << pkEntry->_sModelIcon << '#' << pkEntry->_sCardIcon;
	return sStream.str();
}

StringType SC_ClientSkillInterface::GetLevelUpRuneCost(int iLevel)
{
	const CF_RuneLevelUp::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_RuneLevelUp>(iLevel);
	if(pkEntry == NULL)
		return "";

	StringStreamType sStream;
	LC_ComMap::const_iterator iter = pkEntry->_ConsumeMoney.begin();
	for(; iter!=pkEntry->_ConsumeMoney.end(); ++iter)
	{
		sStream << iter->first << '#' << iter->second << '|';
	}

	return sStream.str();
}

int SC_ClientSkillInterface::GetRuneCommanderLevelLimit(int iLevel)
{
	const CF_RuneLevelUp::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_RuneLevelUp>(iLevel);
	if(pkEntry == NULL)
		return 0;

	return pkEntry->_iCommanderLevel;
}

bool SC_ClientSkillInterface::GetIsRuneSkillUp(int iLevel)
{
	CF_RuneLevelUp::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_RuneLevelUp>(iLevel);
	if(pkData == NULL)
		return false;

	return pkData->_bSkillLevelUp;
}

StringType SC_ClientSkillInterface::GetRuneCollectionInfo()
{
	StringStreamType sStream;
	CF_RuneList::DataEntryMapExternIterator iter = CF_RuneList::GetSingletonPtr()->GetIterator();
	for(;iter.HasMoreElements(); iter.MoveNext())
	{
		const CF_RuneList::DataEntry *pkData = iter.PeekNextValuePtr();
		const CF_ItemList::DataEntry *pkItemData = SafeGetCSVFileDataEntryPtr<CF_ItemList>(pkData->_iItemID);
		if(pkItemData != NULL)
		{
			StringType sItem;
			FormatDataIFs2String(pkData->_sReward, sItem);
			sStream << Int2String(pkData->_iItemID) << '#' << pkItemData->_kName << '#';
			sStream << Int2String(pkItemData->_lGrade) << '#'<< pkData->_sModelIcon << '#';
			sStream << sItem << '|';
		}
	}

	return sStream.str();
}

StringType SC_ClientSkillInterface::GetRuneAttributes(int itemID, int iEquipLevel, int iStarLevel)
{
	LC_ActorAttributeMap attrMap;
	LC_Helper::GetRuneAttrMap(itemID, iEquipLevel, iStarLevel, attrMap);

	return _getAttribString(&attrMap);
}

StringType SC_ClientSkillInterface::GetRuneStarInfo(int iStarUpType, int iStarLevel)
{
	CF_RuneStar::DataEntryMapExternIterator kIter = CF_RuneStar::GetSingletonPtr()->GetIterator();
	for(; kIter.HasMoreElements(); kIter.MoveNext())
	{
		const CF_RuneStar::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		if(pkEntry->_StarUpType == iStarUpType && pkEntry->_StarLevel == iStarLevel)
		{
			StringStreamType sStream;
			StringType sItems;
			FormatDataIFs2String(pkEntry->_consumeItems1, sItems);
			sStream << pkEntry->_MaxLevel << '#' << sItems << '#';
			
			sItems.clear();
			FormatDataIFs2String(pkEntry->_consumeItems2, sItems);
			sStream << sItems << '#' << pkEntry->_quality;
			return sStream.str();
		}
	}

	return "";
}

int SC_ClientSkillInterface::GetRuneStarQuality(int iStarUpType, int iStarLevel)
{
	CF_RuneStar::DataEntryMapExternIterator kIter = CF_RuneStar::GetSingletonPtr()->GetIterator();
	for(; kIter.HasMoreElements(); kIter.MoveNext())
	{
		const CF_RuneStar::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		if(pkEntry->_StarUpType == iStarUpType && pkEntry->_StarLevel == iStarLevel)
			return pkEntry->_quality;
	}
	return 0;
}

int SC_ClientSkillInterface::GetRuneEquipNxtLevelOfAffiliatedSkill(int iEquipLevel, int iSkillLevelID)
{
	CF_RuneLevelUp::DataEntryMapExternIterator kIter = CF_RuneLevelUp::GetSingleton().GetIterator();

	int iLevel = 1;
	while( kIter.HasMoreElements() )
	{
		const CF_RuneLevelUp::DataEntry* pkData = kIter.PeekNextValuePtr();
		if (pkData->_bSkillLevelUp)
		{
			++iLevel;
			if(iLevel == iSkillLevelID)
				return pkData->_iLevel;
		}

		kIter.MoveNext();
	}
	return 0;
}

int SC_ClientSkillInterface::GetRuneEquipSkillIDByItemIDAndItemLevel(int itemID, int itemLevel)
{
	CF_RuneLevelUp::DataEntryMapExternIterator kIter = CF_RuneLevelUp::GetSingleton().GetIterator();
	int skillUp = 0;
	int skillID = 0;
	for(; kIter.HasMoreElements(); kIter.MoveNext())
	{
		const CF_RuneLevelUp::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		if(pkEntry->_iLevel > itemLevel)
			break;
		if(pkEntry->_bSkillLevelUp)
			++skillUp;
	}
	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	LC_ItemDescriptionEntry* pDesc = pkItemFactory->GetItemDescriptionEntry(itemID);
	if(pDesc)
	{
		skillID = pDesc->GetAffiliatedSkill() + skillUp;
	}
	return skillID;
}

StringType SC_ClientSkillInterface::GetRuneMaxLevelInfo(int iItemID)
{
	CF_RuneList::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_RuneList>(iItemID);
	T_ASSERT_CRITICAL_S(pkData != NULL);
	
	StringStreamType sStream;
	sStream << pkData->_iMaxStar << '#' << pkData->_iMaxLevel << '#' << pkData->_iMaxSkillID;
	return sStream.str();
}

int SC_ClientSkillInterface::GetBuddyShowTime(int iCID)
{
	CF_Buddy::DataEntry* pkEntry=SafeGetCSVFileDataEntryPtr<CF_Buddy>(iCID);
	if (pkEntry)
	{
		return pkEntry->_iShowTime;
	}
	return 0;
}

StringType SC_ClientSkillInterface::GetCharShowRewards(int iCID)
{
	CF_CharType::DataEntry* pkEntry=SafeGetCSVFileDataEntryPtr<CF_CharType>(iCID);
	if (pkEntry)
	{
		return pkEntry->_sShowRewards;
	}
	return "";
}

int SC_ClientSkillInterface::GetRaidGloryChapterNum()
{
	return CF_GloryExpedition::GetSingletonPtr()->GetEntryCount();
}

int SC_ClientSkillInterface::GetVipNewMaxNum()
{
	return CF_VipNew::GetSingletonPtr()->GetEntryCount();
}

int SC_ClientSkillInterface::GetBestGuildItemLinkID(int id)
{
	CF_BestGuildBossInfo::DataEntry* pkEntry = SafeGetCSVFileDataEntryPtr<CF_BestGuildBossInfo>(id);
	if (pkEntry)
	{
		return pkEntry->_iLinkID;
	}
	return 0;
}

StringType SC_ClientSkillInterface::GetThemeActivityTaskTabNames(int iBuddyThemeTaskType)
{
	StringStreamType sStream;
	CF_BuddyThemeTaskType::DataEntry* pkEntry = SafeGetCSVFileDataEntryPtr<CF_BuddyThemeTaskType>(iBuddyThemeTaskType);
	if (pkEntry)
	{
		TStringVector kProperties=UT_BaseStringUtility::SplitString(pkEntry->_sChapters, ';');	
		for (int i=0;i<kProperties.size();i++)
		{
			int id=atoi(kProperties[i].c_str());
			CF_BuddyThemeTaskList::DataEntry* listEntry=SafeGetCSVFileDataEntryPtr<CF_BuddyThemeTaskList>(id);
			if (listEntry)
			{
				sStream<<listEntry->_kName<<"#";
			}
		}
	}
	return sStream.str();
}



int SC_ClientSkillInterface::GetThemeActivityTaskScheduleId(int iBuddyThemeTaskType)
{
	CF_BuddyThemeTaskType::DataEntry* pkEntry = SafeGetCSVFileDataEntryPtr<CF_BuddyThemeTaskType>(iBuddyThemeTaskType);
	if (pkEntry)
	{
		return pkEntry->_iSchedule;
	}
	return 0;
}

int SC_ClientSkillInterface::GetHolidayActivityTaskScheduleId(int iHolidayActivityTaskType)
{
	CF_HolidayActivityTaskType::DataEntry* pkEntry = SafeGetCSVFileDataEntryPtr<CF_HolidayActivityTaskType>(iHolidayActivityTaskType);
	if (pkEntry)
	{
		return pkEntry->_iSchedule;
	}
	return 0;
}

StringType SC_ClientSkillInterface::GetHolidayActivityTaskTabNames(int iHolidayActivityTaskType)
{
	StringStreamType sStream;
	CF_HolidayActivityTaskType::DataEntry* pkEntry = SafeGetCSVFileDataEntryPtr<CF_HolidayActivityTaskType>(iHolidayActivityTaskType);
	if (pkEntry)
	{
		TStringVector kProperties=UT_BaseStringUtility::SplitString(pkEntry->_sChapters, ';');	
		for (int i=0;i<kProperties.size();i++)
		{
			int id=atoi(kProperties[i].c_str());
			CF_HolidayActivityTaskList::DataEntry* listEntry=SafeGetCSVFileDataEntryPtr<CF_HolidayActivityTaskList>(id);
			if (listEntry)
			{
				sStream<<listEntry->_kName<<"#";
			}
		}
	}
	return sStream.str();
}




StringType SC_ClientSkillInterface::GetCustomCommandList()
{
	LC_CommandParserBase* pkCmdParser=LC_CommandParserBase::GetSingletonPtr();
	StringStreamType sStream;
	if (!pkCmdParser)
	{
		return "";
	}

	return pkCmdParser->GetCommandList();
}

int CalculateDrugScore(int iType,int iAttrMaxScore,int iStage,int Drug0Count,int Drug1Count)
{
	int ItemPropertyFuncID0=50501;
	int ItemPropertyFuncID1=50508;
	switch(iType)
	{
	case 0:
		ItemPropertyFuncID0=50501;
		ItemPropertyFuncID1=50508;
		break;
	case 2:
		ItemPropertyFuncID0=50503;
		ItemPropertyFuncID1=50510;
		break;
	case 8:
		ItemPropertyFuncID0=50502;
		ItemPropertyFuncID1=50509;
		break;
	}


	combat_score_type Drug0Score=0;
	combat_score_type Drug1Score=0;
	//固定属性丹
	CF_ItemPropertyFunc::DataEntry* pkPropertyEntry1=SafeGetCSVFileDataEntryPtr<CF_ItemPropertyFunc>(ItemPropertyFuncID0);
	if (pkPropertyEntry1)
	{

		TStringVector kVector = UT_BaseStringUtility::SplitString(pkPropertyEntry1->_sScriptArgs, ';');
		for (int i=0;i<kVector.size();i++)
		{
			TStringVector kProperties=UT_BaseStringUtility::SplitString(kVector[i], ':');	
			combat_score_type attrID=_atoi64(kProperties[1].c_str());
			combat_score_type attrAbsVal=_atoi64(kProperties[2].c_str());
			Drug0Score+=LC_Helper::CalcSingleAttributeCombatScore((AttributeType)attrID,attrAbsVal,false);
		}
	}

	//百分比属性丹 

	LC_ActorAttributeMap attrMap;
	for(int i=1; i<iStage; ++i)
	{
		LC_SubordinateDataEntry* pkEntry = SafeGetSubDataEntry(iType, i);
		if (NULL != pkEntry)
		{
			for(mem::vector<int>::iterator it1=pkEntry->_sAttrTypes.begin(); it1!=pkEntry->_sAttrTypes.end(); ++it1)
			{
				LC_Helper::GetCimeliaMapByAttrID(&attrMap, pkEntry->_iAttrFuncID, *it1, pkEntry->_iAttrMaxLevel);	
			}
		}
	}
	CF_ItemPropertyFunc::DataEntry* pkPropertyEntry2=SafeGetCSVFileDataEntryPtr<CF_ItemPropertyFunc>(ItemPropertyFuncID1);
	if (pkPropertyEntry2)
	{

		TStringVector kVector = UT_BaseStringUtility::SplitString(pkPropertyEntry2->_sScriptArgs, ';');
		for (int i=0;i<kVector.size();i++)
		{
			TStringVector kProperties=UT_BaseStringUtility::SplitString(kVector[i], ':');	
			combat_score_type attrID=_atoi64(kProperties[1].c_str());
			combat_score_type attrPercent=_atoi64(kProperties[3].c_str());	
			attrMap.AddAttributePercentFunc(attrID,attrPercent);
		}
	}
	attrMap.RecomputeAll();
	Drug1Score = LC_Helper::CalcAttributeMapCombatScore(&attrMap, false);
	Drug1Score=Drug1Score-iAttrMaxScore;
	combat_score_type DrugMaxScore=Drug0Score*Drug0Count+Drug1Score*Drug1Count;
	return DrugMaxScore;
}

void SC_ClientSkillInterface::CalculateBeBetterMaxScores()
{
	//英雄
	CalculateAllHeroLevelMaxScore();
	CalculateAllHeroStarMaxScore();
	CalculateAllHeroSkillMaxScore();
	
	//宝具
	CalculateAllRuneLevelMaxScore();
	CalculateAllRuneStarMaxScore();

	//装备
	CalculateAllEquipLevelMaxScore();
	CalculateAllEquipStarMaxScore();	
	CalculateAllEquipMaxScore();
	//玩具
	CalculateToyPetMaxScore();

	//幻兽
	CalculateVehicleAttrMaxScore();
	CalculateVehicleSkinMaxScore();
	CalculateVehicleEquipMaxScore();
	CalculateVehicleDrugMaxScore();

	//萌宠
	CalculateDevaEyeAttrMaxScore();
	CalculateDevaEyeSkinMaxScore();
	CalculateDevaEyeEquipMaxScore();
	CalculateDevaEyeDrugMaxScore();
	
	//背饰
	CalculateWingAttrMaxScore();
	CalculateWingSkinMaxScore();
	CalculateWingEquipMaxScore();
	CalculateWingDrugMaxScore();

	//指挥官装备
	CalculateCommanderEquipAttrMaxScore();
	CalculateCommanderEquipStrengthenMaxScore();
	CalculateCommanderEquipStarMaxScore();


}

int GetMaxSubordinateStageByCommanderLevel(int iType,int iLevel)
{
	for(int i = 1; i <= 99; ++i)
	{
		LC_SubordinateDataEntry* pkEntry = SafeGetSubDataEntry(iType, i);
		if (!pkEntry)
		{
			return i-1;
		}
		CF_Subordinate::DataEntry* pkSbData = SafeGetCSVFileDataEntryPtr<CF_Subordinate>(pkEntry->_iID);
		if (!pkSbData || iLevel<pkSbData->_iLevel)
		{
			return i;
		}
	}
	return 0;
}

combat_score_type GetSubordinateMaxAttriScoreByStage(int iType,int iStage)
{
	LC_ActorAttributeMap attrMap;

	for(int i = 1; i < iStage; ++i)
	{
		LC_SubordinateDataEntry* pkEntry = SafeGetSubDataEntry(iType, i);
		if (NULL != pkEntry)
		{
			for(mem::vector<int>::iterator it1 = pkEntry->_sAttrTypes.begin(); it1 != pkEntry->_sAttrTypes.end(); ++it1)
			{
				LC_Helper::GetCimeliaMapByAttrID(&attrMap, pkEntry->_iAttrFuncID, *it1, pkEntry->_iAttrMaxLevel);
			}
		}
	}
	attrMap.RecomputeAll();
	return LC_Helper::CalcAttributeMapCombatScore(&attrMap, false);
}

combat_score_type SC_ClientSkillInterface::GetAllHeroLevelMaxScore()
{
	return m_AllHeroLevelScore;
}

int SC_ClientSkillInterface::GetAllHeroCount()
{
	return CF_Buddy::GetSingleton().GetEntryCount();
}


float GetCoefficientByQuality(CF_Buddy::DataEntry* pkEntry,CF_PromoteCombat::DataEntry* pkPromote)
{

	float coefficient = 0;
	if (pkEntry->_quality == "R")
	{
		coefficient = pkPromote->_fCoefficient1;
	}
	else if (pkEntry->_quality == "SR")
	{
		coefficient = pkPromote->_fCoefficient2;
	}
	else if (pkEntry->_quality == "SSR")
	{
		coefficient = pkPromote->_fCoefficient3;
	}
	else if (pkEntry->_quality == "UR")
	{
		coefficient = pkPromote->_fCoefficient4;
	}
	return coefficient;
}

//英雄等级当前战力
combat_score_type SC_ClientSkillInterface::GetOwnedHeroLevelScore()
{
	CF_PromoteCombat::DataEntry* pkPromote = SafeGetCSVFileDataEntryPtr<CF_PromoteCombat>(1);
	LC_ClientGamePlayer* pkPlayer = GetPlayerPtr();
	if(!pkPlayer)
	{
		return 0;
	}
	combat_score_type OwnedHeroLevelScore = 0;
	const buddyInstanceMap& BuddyInstanceMap = pkPlayer->GetBuddyInstanceMap();
	buddyInstanceMap::const_iterator  it = BuddyInstanceMap.begin();
	for (;it != BuddyInstanceMap.end();++it)
	{
		LC_ClientBuddyInstance* pInst = it->second;
		if (!pInst->GetInitial())			//GetInitial为false说明已拥有
		{

			CF_Buddy::DataEntry* pkEntry = SafeGetCSVFileDataEntryPtr<CF_Buddy>(pInst->GetCID());
			float coefficient = GetCoefficientByQuality(pkEntry,pkPromote);
			OwnedHeroLevelScore += coefficient*pInst->GetLevel();
		}
	}	
	return OwnedHeroLevelScore;
}


//英雄星级当前战力
combat_score_type SC_ClientSkillInterface::GetOwnedHeroStarScore()
{
	combat_score_type OwnedHeroStarScore = 0;
	LC_ClientGamePlayer* pkPlayer = GetPlayerPtr();
	CF_PromoteCombat::DataEntry* pkPromote = SafeGetCSVFileDataEntryPtr<CF_PromoteCombat>(2);
	if(pkPlayer)
	{
		const buddyInstanceMap& BuddyInstanceMap = pkPlayer->GetBuddyInstanceMap();
		buddyInstanceMap::const_iterator  it = BuddyInstanceMap.begin();
		for (;it != BuddyInstanceMap.end();++it)
		{
			LC_ClientBuddyInstance* pInst = it->second;
			if (!pInst->GetInitial())			//GetInitial为false说明已拥有
			{

				CF_Buddy::DataEntry* pkEntry = SafeGetCSVFileDataEntryPtr<CF_Buddy>(pInst->GetCID());
				float coefficient = GetCoefficientByQuality(pkEntry,pkPromote);
				OwnedHeroStarScore += coefficient*pInst->GetStarLevel();
			}
		}
	}

	return OwnedHeroStarScore;
}

//英雄技能当前战力
combat_score_type SC_ClientSkillInterface::GetOwnedHeroSkillScore()
{
	LC_ClientGamePlayer* pkGamePlayer = GetPlayerPtr();
	if(!pkGamePlayer)
	{
		return 0;
	}
	combat_score_type OwnedHeroSkillScore = 0;

	CF_Buddy::DataEntryMapExternIterator kIter = CF_Buddy::GetSingleton().GetIterator();
	while(kIter.HasMoreElements())	//循环次数为英雄数量 21
	{
		CF_Buddy::DataEntry* pkData = kIter.PeekNextValuePtr();
		TStringVector kvect = UT_BaseStringUtility::SplitString(pkData->_skillid, ';');
		for (unsigned int i = 0 ; i < kvect.size() ; ++i)	//4个基础技能，循环次数为4
		{
			const StringType& SkillInfo = kvect[i];
			TStringVector SkillID = UT_BaseStringUtility::SplitString(SkillInfo, ':');
			int iSkillID = atoi(SkillID[0].c_str());

			LC_SkillAsset* pkSkillAsset = pkGamePlayer->GetBuddySkillAsset(pkData->_cID);
			if (!pkSkillAsset)
			{
				continue;
			}

			LC_SkillAssetEntry* pkLearnedSkill = pkSkillAsset->FindLearnedSkill(iSkillID);
			if (!pkLearnedSkill)
			{
				continue;
			}

			CF_SkillSpecialAttrib::DataEntry* pkLearnedSkillEntry = SafeGetCSVFileDataEntryPtr<CF_SkillSpecialAttrib>(pkLearnedSkill->GetTypeID());// 18000条数据 大约15次查找
			if (pkLearnedSkillEntry)
			{
				OwnedHeroSkillScore += pkLearnedSkillEntry->_lCombatScore;
			}	
		}
		kIter.MoveNext();
	}
	return OwnedHeroSkillScore;
}

int SC_ClientSkillInterface::GetOwnedHeroCount()
{	
	LC_ClientGamePlayer* pkGamePlayer = GetPlayerPtr();
	if(!pkGamePlayer)
	{
		return 0;
	}
	int OwnedHeroCount = 0;
	const buddyInstanceMap& BuddyInstanceMap = pkGamePlayer->GetBuddyInstanceMap();
	buddyInstanceMap::const_iterator  it = BuddyInstanceMap.begin();
	for (;it != BuddyInstanceMap.end();++it)
	{
		LC_ClientBuddyInstance* pInst = it->second;
		if (!pInst->GetInitial())
		{
			OwnedHeroCount++;
		}		
	}
	return OwnedHeroCount;
}

//等级总战力
void SC_ClientSkillInterface::CalculateAllHeroLevelMaxScore()
{
	LC_ClientGamePlayer* pkGamePlayer = GetPlayerPtr();
	if(!pkGamePlayer)
	{
		return;
	}
	m_AllHeroLevelScore = 0;
	CF_PromoteCombat::DataEntry* pkPromote = SafeGetCSVFileDataEntryPtr<CF_PromoteCombat>(1);
	const buddyInstanceMap& BuddyInstanceMap = pkGamePlayer->GetBuddyInstanceMap();
	buddyInstanceMap::const_iterator it = BuddyInstanceMap.begin();
	int iCommanderLevel = pkGamePlayer->GetCommanderLevel();
	for (;it != BuddyInstanceMap.end();++it)
	{
		LC_ClientBuddyInstance* pInst = it->second;
		if (!pInst->GetInitial())			//GetInitial为false说明已拥有
		{
			CF_Buddy::DataEntry* pkEntry  =  SafeGetCSVFileDataEntryPtr<CF_Buddy>(pInst->GetCID());
			float coefficient = GetCoefficientByQuality(pkEntry,pkPromote);
			m_AllHeroLevelScore += coefficient*iCommanderLevel;
		}
	}	
}

//星级总战力
void SC_ClientSkillInterface::CalculateAllHeroStarMaxScore()
{
	LC_ClientGamePlayer* pkGamePlayer = GetPlayerPtr();
	if(!pkGamePlayer)
	{
		return;
	}
	m_AllHeroStarScore = 0;
	CF_PromoteCombat::DataEntry* pkPromote = SafeGetCSVFileDataEntryPtr<CF_PromoteCombat>(2);
	const buddyInstanceMap& BuddyInstanceMap = pkGamePlayer->GetBuddyInstanceMap();
	buddyInstanceMap::const_iterator it = BuddyInstanceMap.begin();
	int iMaxStar = 25;
	for (;it != BuddyInstanceMap.end();++it)
	{
		LC_ClientBuddyInstance* pInst = it->second;
		if (!pInst->GetInitial())			//GetInitial为false说明已拥有
		{
			CF_Buddy::DataEntry* pkEntry = SafeGetCSVFileDataEntryPtr<CF_Buddy>(pInst->GetCID());
			float coefficient = GetCoefficientByQuality(pkEntry,pkPromote);
			m_AllHeroStarScore += coefficient*iMaxStar;
		}
	}	
}

//技能总战力
void SC_ClientSkillInterface::CalculateAllHeroSkillMaxScore()
{
	LC_ClientGamePlayer* pkGamePlayer = GetPlayerPtr();
	if(!pkGamePlayer)
	{
		return;
	}
	m_AllHeroSkillScore = 0;
	int iCommanderLevel = pkGamePlayer->GetCommanderLevel();
	const buddyInstanceMap& BuddyInstanceMap = pkGamePlayer->GetBuddyInstanceMap();
	buddyInstanceMap::const_iterator it = BuddyInstanceMap.begin();
	for (;it!=BuddyInstanceMap.end();++it)
	{
		LC_ClientBuddyInstance* pInst = it->second;
		if (pInst->GetInitial())			//GetInitial为true说明未拥有
		{
			continue;
		}

		CF_Buddy::DataEntry* pkData  = SafeGetCSVFileDataEntryPtr<CF_Buddy>(pInst->GetCID());
		TStringVector kvect = UT_BaseStringUtility::SplitString(pkData->_skillid, ';');
		for (unsigned int i = 0 ; i < kvect.size() ; ++i)	//循环次数为4
		{
			const StringType& SkillInfo = kvect[i];
			TStringVector SkillID = UT_BaseStringUtility::SplitString(SkillInfo, ':');
			int uiSkillID = atoi(SkillID[0].c_str());
			int iOpenLevel = atoi(SkillID[1].c_str());
			if (iOpenLevel>iCommanderLevel)
			{
				continue;
			}
			CF_SkillSpecialAttrib::DataEntry* pkEntry = SafeGetCSVFileDataEntryPtr<CF_SkillSpecialAttrib>(uiSkillID);// 18000条数据 大约15次查找
			if (!pkEntry)
			{
				continue;	
			}

			/*
			***感觉表的数据字段设置的不是很合理，但是改不动啊
			*/
			int TopSkillID = 0;
			int CurMaxSkillLevel = 1;
			for (int m = 1;m <= pkEntry->_lTopLevel;m++)
			{
				int TopSkillID = (uiSkillID/100)*100+m;
				CF_SkillLevelUp::DataEntry* pkSkillLvUpEntry = SafeGetCSVFileDataEntryPtr<CF_SkillLevelUp>(TopSkillID);// 18000条数据 大约15次查找
				if (pkSkillLvUpEntry == NULL)
				{
					break;
				}
				CurMaxSkillLevel = m;
				if (iCommanderLevel<pkSkillLvUpEntry->_lNeedLevel)
				{
					break;
				}

			}
			TopSkillID = (uiSkillID/100)*100+CurMaxSkillLevel;
			CF_SkillSpecialAttrib::DataEntry* pkTopEntry = SafeGetCSVFileDataEntryPtr<CF_SkillSpecialAttrib>(TopSkillID);// 18000条数据 大约15次查找
			if (pkTopEntry)
			{
				m_AllHeroSkillScore+=pkTopEntry->_lCombatScore;
			}		
		}
	}
}


int SC_ClientSkillInterface::GetAllRuneCount()
{
	return CF_RuneList::GetSingleton().GetEntryCount();
}

//宝具等级当前战力
combat_score_type SC_ClientSkillInterface::GetOwnedRuneLevelScore()
{
	LC_ClientGamePlayer* pkPlayer = GetPlayerPtr();
	if (!pkPlayer)
	{
		return 0;
	}

	CF_PromoteCombat::DataEntry* pkPromote = SafeGetCSVFileDataEntryPtr<CF_PromoteCombat>(3);
	float coefficient[4] = {pkPromote->_fCoefficient1,pkPromote->_fCoefficient2,pkPromote->_fCoefficient3,pkPromote->_fCoefficient4};

	combat_score_type OwnedRuneLevelScore = 0;

	//穿在角色身上的
	const buddyInstanceMap& pEquipedRune = pkPlayer->GetBuddyInstanceMap();
	for (buddyInstanceMap::const_iterator itr = pEquipedRune.begin(); itr!=pEquipedRune.end(); ++itr)
	{
		LC_ClientBuddyInstance* pkInst = itr->second;
		for(size_t i=0; i<GetGlobalSetting.RunePackSize; i++)
		{
			const LC_BackPackEntry* pItemData = pkInst->GetRuneEquipEntry(i);
			CF_RuneList::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_RuneList>(pItemData->GetTypeID());
			if (pkData)
			{
				int iLevel = pItemData->GetEquipLevel();
				int idx = UT_MathBase::ClampInt(pkData->_iGrade-2,0,3);;
				OwnedRuneLevelScore+=coefficient[idx]*iLevel;
			}
		}
	}
	
	//背包里没穿的宝具
	LC_PackAsset* pkPackAsset = pkPlayer->GetPackAsset();
	const LC_BasicPack& pRuneInPack = pkPackAsset->GetBasicPack();
	for (int i = 0;i<pRuneInPack.GetSize();i++)
	{
		const LC_BackPackEntry& pItemData = pRuneInPack.m_aData[i];
		CF_RuneList::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_RuneList>(pItemData.GetTypeID());
		if (pkData)
		{
			int iLevel = pItemData.GetEquipLevel();
			int idx = UT_MathBase::ClampInt(pkData->_iGrade-2,0,3);;
			OwnedRuneLevelScore += coefficient[idx]*iLevel;
		}
	}
	return OwnedRuneLevelScore;
}

//宝具星级当前战力
combat_score_type SC_ClientSkillInterface::GetOwnedRuneStarScore()
{
	LC_ClientGamePlayer* pkPlayer = GetPlayerPtr();
	if (!pkPlayer)
	{
		return 0;
	}

	CF_PromoteCombat::DataEntry* pkPromote = SafeGetCSVFileDataEntryPtr<CF_PromoteCombat>(4);
	float coefficient[4] = {pkPromote->_fCoefficient1,pkPromote->_fCoefficient2,pkPromote->_fCoefficient3,pkPromote->_fCoefficient4};

	combat_score_type OwnedRuneStarScore = 0;
	
	//穿在角色身上的
	const buddyInstanceMap& pEquipedRune = pkPlayer->GetBuddyInstanceMap();
	for (buddyInstanceMap::const_iterator itr = pEquipedRune.begin(); itr!=pEquipedRune.end(); ++itr)
	{
		LC_ClientBuddyInstance* pkInst = itr->second;
		for(size_t i=0; i<GetGlobalSetting.RunePackSize; i++)
		{
			const LC_BackPackEntry* pItemData = pkInst->GetRuneEquipEntry(i);
			CF_RuneList::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_RuneList>(pItemData->GetTypeID());
			if (pkData)
			{
				int iStar = pItemData->GetEquipStar();
				int idx = UT_MathBase::ClampInt(pkData->_iGrade-2,0,3);;
				OwnedRuneStarScore += coefficient[idx]*iStar;
			}
		}
	}

	//背包里没穿的宝具
	LC_PackAsset* pkPackAsset = pkPlayer->GetPackAsset();
	const LC_BasicPack& pRuneInPack = pkPackAsset->GetBasicPack();
	for (int i = 0;i<pRuneInPack.GetSize();i++)
	{
		const LC_BackPackEntry& pItemData = pRuneInPack.m_aData[i];
		CF_RuneList::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_RuneList>(pItemData.GetTypeID());
		if (pkData)
		{
			int iStar = pItemData.GetEquipStar();
			int idx = UT_MathBase::ClampInt(pkData->_iGrade-2,0,3);;
			OwnedRuneStarScore += coefficient[idx]*iStar;
		}
	}
	return OwnedRuneStarScore;
}

int SC_ClientSkillInterface::GetOwnedRuneCount()
{
	LC_ClientGamePlayer* pkPlayer = GetPlayerPtr();
	if (!pkPlayer)
	{
		return 0;
	}

	int iCount  = 0;
	const buddyInstanceMap& pEquipedRune = pkPlayer->GetBuddyInstanceMap();
	for (buddyInstanceMap::const_iterator itr = pEquipedRune.begin(); itr!=pEquipedRune.end(); ++itr)
	{
		LC_ClientBuddyInstance* pkInst = itr->second;
		for(size_t i=0; i<GetGlobalSetting.RunePackSize; i++)
		{
			const LC_BackPackEntry* pItemData = pkInst->GetRuneEquipEntry(i);
			if (pItemData->GetValid())
			{
				iCount++;
			}
		}
	}

	//背包里没穿的宝具
	LC_PackAsset* pkPackAsset = pkPlayer->GetPackAsset();
	const LC_BasicPack& pRuneInPack = pkPackAsset->GetBasicPack();
	for (int i = 0;i<pRuneInPack.GetSize();i++)
	{
		const LC_BackPackEntry& pItemData = pRuneInPack.m_aData[i];
		if (pItemData.GetValid())
		{
			if (CF_RuneList::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_RuneList>(pItemData.GetTypeID()))
			{
				iCount++;
			}
		}
	}
	return iCount;
}

int GetRuneMaxLevelByCommanderLevel(int iMaxLevel,int iCommanderLevel)
{
	int iCount = 0;
	CF_RuneLevelUp::DataEntryMapExternIterator kIter = CF_RuneLevelUp::GetSingleton().GetIterator();
	while(kIter.HasMoreElements())//最差情况300次
	{
		CF_RuneLevelUp::DataEntry* pkData = kIter.PeekNextValuePtr();
		if (iCount>iMaxLevel)
		{
			return iMaxLevel;
		}
		if(pkData->_iCommanderLevel > iCommanderLevel)
		{
			return iCount;
		}
		kIter.MoveNext();
		iCount++;
	}
	return iMaxLevel;
}

//宝具等级总战力(所有宝具之和，已拥有)
void SC_ClientSkillInterface::CalculateAllRuneLevelMaxScore()
{
	LC_ClientGamePlayer* pkGamePlayer = GetPlayerPtr();
	if (!pkGamePlayer)
	{
		return ;
	}

	CF_PromoteCombat::DataEntry* pkPromote = SafeGetCSVFileDataEntryPtr<CF_PromoteCombat>(3);
	float coefficient[4] = {pkPromote->_fCoefficient1,pkPromote->_fCoefficient2,pkPromote->_fCoefficient3,pkPromote->_fCoefficient4};

	m_AllRuneLevelScore = 0;
	int iCommanderLevel = pkGamePlayer->GetCommanderLevel();

	//穿在角色身上的
	const buddyInstanceMap& pEquipedRune = pkGamePlayer->GetBuddyInstanceMap();
	for (buddyInstanceMap::const_iterator itr = pEquipedRune.begin(); itr!=pEquipedRune.end(); ++itr)
	{
		LC_ClientBuddyInstance* pkInst = itr->second;
		for(size_t i=0; i<GetGlobalSetting.RunePackSize; i++)
		{
			const LC_BackPackEntry* pItemData = pkInst->GetRuneEquipEntry(i);
			CF_RuneList::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_RuneList>(pItemData->GetTypeID());
			if (pkData)
			{
				int iLevel = GetRuneMaxLevelByCommanderLevel(pkData->_iMaxLevel,iCommanderLevel);
				int idx = UT_MathBase::ClampInt(pkData->_iGrade-2,0,3);
				m_AllRuneLevelScore += coefficient[idx]*iLevel;
			}
		}
	}

	//背包里没穿的宝具
	LC_PackAsset* pkPackAsset = pkGamePlayer->GetPackAsset();
	const LC_BasicPack& pRuneInPack = pkPackAsset->GetBasicPack();
	for (int i = 0;i<pRuneInPack.GetSize();i++)
	{
		const LC_BackPackEntry& pItemData = pRuneInPack.m_aData[i];
		CF_RuneList::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_RuneList>(pItemData.GetTypeID());
		if (pkData)
		{
			int iLevel = GetRuneMaxLevelByCommanderLevel(pkData->_iMaxLevel,iCommanderLevel);
			int idx = UT_MathBase::ClampInt(pkData->_iGrade-2,0,3);;
			m_AllRuneLevelScore += coefficient[idx]*iLevel;
		}
	}
}

//宝具星级总战力(所有宝具之和，已拥有)
void SC_ClientSkillInterface::CalculateAllRuneStarMaxScore()
{
	LC_ClientGamePlayer* pkGamePlayer = GetPlayerPtr();
	if (!pkGamePlayer)
	{
		return ;
	}
	
	CF_PromoteCombat::DataEntry* pkPromote = SafeGetCSVFileDataEntryPtr<CF_PromoteCombat>(4);
	float coefficient[4] = {pkPromote->_fCoefficient1,pkPromote->_fCoefficient2,pkPromote->_fCoefficient3,pkPromote->_fCoefficient4};
	
	m_AllRuneStarScore = 0;
	
	//穿在角色身上的
	const buddyInstanceMap& pEquipedRune = pkGamePlayer->GetBuddyInstanceMap();
	for (buddyInstanceMap::const_iterator itr = pEquipedRune.begin(); itr!=pEquipedRune.end(); ++itr)
	{
		LC_ClientBuddyInstance* pkInst = itr->second;
		for(size_t i=0; i<GetGlobalSetting.RunePackSize; i++)
		{
			const LC_BackPackEntry* pItemData = pkInst->GetRuneEquipEntry(i);
			CF_RuneList::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_RuneList>(pItemData->GetTypeID());
			if (pkData)
			{
				int iStar = pkData->_iMaxStar;
				
				int idx = UT_MathBase::ClampInt(pkData->_iGrade-2,0,3);

				m_AllRuneStarScore += coefficient[idx]*iStar;
			}
		}
	}

	//背包里没穿的宝具
	LC_PackAsset* pkPackAsset = pkGamePlayer->GetPackAsset();
	const LC_BasicPack& pRuneInPack = pkPackAsset->GetBasicPack();
	for (int i = 0;i<pRuneInPack.GetSize();i++)
	{
		const LC_BackPackEntry& pItemData = pRuneInPack.m_aData[i];
		CF_RuneList::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_RuneList>(pItemData.GetTypeID());
		if (pkData)
		{
			int iStar = pkData->_iMaxStar;
			int idx = UT_MathBase::ClampInt(pkData->_iGrade-2,0,3);;
			m_AllRuneStarScore += coefficient[idx]*iStar;
		}
	}
}



//仅装备当前战力(已拥有英雄之和)
combat_score_type SC_ClientSkillInterface::GetOwnedEquipScore()
{
	LC_ClientGamePlayer* pkPlayer = GetPlayerPtr();
	if (!pkPlayer)
	{
		return 0;
	}

	combat_score_type OwnedHeroEquipedScore = 0;
	const buddyInstanceMap& BuddyInstanceMap = pkPlayer->GetBuddyInstanceMap();
	buddyInstanceMap::const_iterator  it = BuddyInstanceMap.begin();

	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();

	int iSize = GetGlobalSetting.ArtifactPackSize;
	for (;it != BuddyInstanceMap.end();++it)
	{
		LC_ClientBuddyInstance* pInst = it->second;
		if (!pInst->GetInitial())			//GetInitial为false说明已拥有
		{
			for (int i = 0;i<iSize;i++)
			{
				const LC_BackPackEntry* pItemData = pInst->GetEquipEntry(i);
				if(pItemData->GetValid())
				{
					//Slotidx;				武器:0 铠甲:1 头盔:4 饰品:2
					//fCoefficient          武器:1 铠甲:2 头盔:3 饰品:4
					const int iItemId = pItemData->GetTypeID();
					LC_ItemDescriptionEntry* pDesc = pkItemFactory->GetItemDescriptionEntry(iItemId);
					if(pDesc)
					{
						OwnedHeroEquipedScore += GetEquipScoreByItemID(iItemId);
					}
				}
			}
		}
	}	
	return OwnedHeroEquipedScore;
}
//仅装备槽位等级当前战力(已拥有英雄之和)
combat_score_type SC_ClientSkillInterface::GetOwnedEquipLevelScore()
{
	CF_PromoteCombat::DataEntry* pkPromote = SafeGetCSVFileDataEntryPtr<CF_PromoteCombat>(6);

	LC_ClientGamePlayer* pkPlayer = GetPlayerPtr();
	if (!pkPlayer)
	{
		return 0;
	}

	combat_score_type OwnedHeroEquipLevelScore = 0;
	int iSlotSize = GetGlobalSetting.ArtifactPackSize;

	const buddyInstanceMap& BuddyInstanceMap = pkPlayer->GetBuddyInstanceMap();
	buddyInstanceMap::const_iterator  it = BuddyInstanceMap.begin();
	for (;it != BuddyInstanceMap.end();++it)
	{
		LC_ClientBuddyInstance* pInst = it->second;
		if (!pInst->GetInitial())			//GetInitial为false说明已拥有
		{
			float coefficient[4] = {pkPromote->_fCoefficient1,pkPromote->_fCoefficient2,pkPromote->_fCoefficient3,pkPromote->_fCoefficient4};
			for (int i = 0;i<iSlotSize;i++)
			{
				int iLevel = pkPlayer->GetEquipSlotLevel(it->first, i);
				OwnedHeroEquipLevelScore += coefficient[i]*iLevel ;
			}
		}
	}	
	return OwnedHeroEquipLevelScore;
}
//仅装备槽位星级当前战力评分(已拥有英雄之和)
combat_score_type SC_ClientSkillInterface::GetOwnedEquipStarScore()
{
	CF_PromoteCombat::DataEntry* pkPromote = SafeGetCSVFileDataEntryPtr<CF_PromoteCombat>(7);

	LC_ClientGamePlayer* pkPlayer = GetPlayerPtr();
	if (!pkPlayer)
	{
		return 0;
	}

	combat_score_type OwnedHeroEquipStarScore = 0;
	int iSlotSize = GetGlobalSetting.ArtifactPackSize;

	const buddyInstanceMap& BuddyInstanceMap = pkPlayer->GetBuddyInstanceMap();
	buddyInstanceMap::const_iterator  it = BuddyInstanceMap.begin();
	for (;it != BuddyInstanceMap.end();++it)
	{
		LC_ClientBuddyInstance* pInst = it->second;
		if (!pInst->GetInitial())			//GetInitial为false说明已拥有
		{
			float coefficient[4] = {pkPromote->_fCoefficient1,pkPromote->_fCoefficient2,pkPromote->_fCoefficient3,pkPromote->_fCoefficient4};
			for (int i = 0;i<iSlotSize;i++)
			{
				int iLevel = pkPlayer->GetEquipSlotStar(it->first, i);
				OwnedHeroEquipStarScore += coefficient[i]*iLevel ;
			}
		}
	}	
	return OwnedHeroEquipStarScore;
}

//最强装备总值（x已拥有英雄数量）
void SC_ClientSkillInterface::CalculateAllEquipMaxScore()
{
	LC_ClientGamePlayer* pkGamePlayer = GetPlayerPtr();
	if (!pkGamePlayer)
	{
		return;
	}
	m_AllHeroEquipScore = 0;
	int iCommanderLevel = pkGamePlayer->GetCommanderLevel();
	int iCurMaxLevel = 1;
	for (int i = 101;i <= 199;i++)
	{
		CF_ItemEquipLimitFunc::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_ItemEquipLimitFunc>(i);
		if (!pkData)
		{
			break;
		}
		if (iCommanderLevel<pkData->_lLevelLimit)
		{
			break;
		}
		iCurMaxLevel = i-100;
	}
	//最好的四件装备 ：武器，铠甲，头盔，饰品
	int BestEquipID[4] = {11360000+iCurMaxLevel,11361000+iCurMaxLevel,11364000+iCurMaxLevel,11362000+iCurMaxLevel};

	int PerHeroScore = 0;
	for (int i = 0;i<4;i++)
	{
		PerHeroScore += GetEquipScoreByItemID(BestEquipID[i]);
	}
	int iHeroCount = GetOwnedHeroCount();
	m_AllHeroEquipScore = PerHeroScore*iHeroCount;
}

//装备槽位最高等级总值（x已拥有英雄数量）
void SC_ClientSkillInterface::CalculateAllEquipLevelMaxScore()
{
	LC_ClientGamePlayer* pkGamePlayer = GetPlayerPtr();
	if (!pkGamePlayer)
	{
		return;
	}
	CF_PromoteCombat::DataEntry* pkPromote = SafeGetCSVFileDataEntryPtr<CF_PromoteCombat>(6);
	m_AllHeroEquipLevelScore = 0;
	int iHeroCount = GetOwnedHeroCount();
	int iEquipMaxLevel = CF_EquipSlotReinforce::GetSingleton().GetEntryCount();
	int iCurMaxLevel = 0;
	int iCommanderLevel = pkGamePlayer->GetCommanderLevel();
	for (int i = 1;i <= iEquipMaxLevel;i++)
	{
		CF_EquipSlotReinforce::DataEntry* pkEntry = SafeGetCSVFileDataEntryPtr<CF_EquipSlotReinforce>(i);
		if (pkEntry == NULL)
		{
			break;
		}
		if (pkEntry->_iBuddyLevel>iCommanderLevel)
		{
			break;
		}
		iCurMaxLevel = i;
	}
	float coefficient[4] = {pkPromote->_fCoefficient1,pkPromote->_fCoefficient2,pkPromote->_fCoefficient3,pkPromote->_fCoefficient4};
	combat_score_type PerHeroScore = 0;
	for (int i = 0;i<4;i++)
	{
		PerHeroScore += iCurMaxLevel*coefficient[i];
	}
	
	
	m_AllHeroEquipLevelScore = PerHeroScore*iHeroCount;
}

//装备槽位最高星级总值（x已拥有英雄数量）
void SC_ClientSkillInterface::CalculateAllEquipStarMaxScore()
{
	LC_ClientGamePlayer* pkGamePlayer = GetPlayerPtr();
	if (!pkGamePlayer)
	{
		return;
	}
	CF_PromoteCombat::DataEntry* pkPromote = SafeGetCSVFileDataEntryPtr<CF_PromoteCombat>(7);
	int iMaxStar = 29;//不太方便读表，如果后期需要可改成根据表来获取最大星级
	int iCommanderLevel = pkGamePlayer->GetCommanderLevel();
	int iCurMaxStar = 0;
	for (int i = 1;i<=iMaxStar;i++)
	{
		CF_EquipSlotStar::DataEntry* pkEntry = SafeGetCSVFileDataEntryPtr<CF_EquipSlotStar>(i);
		if (pkEntry == NULL)
		{
			break;
		}
		if (pkEntry->_iBuddyLevel>iCommanderLevel)
		{
			break;
		}
		iCurMaxStar = i;
	}
	m_AllHeroEquipStarScore = 0;
	int iHeroCount = GetOwnedHeroCount();
	float coefficient[4] = {pkPromote->_fCoefficient1,pkPromote->_fCoefficient2,pkPromote->_fCoefficient3,pkPromote->_fCoefficient4};
	combat_score_type PerHeroScore = 0;
	for (int i = 0;i < 4;i++)
	{
		PerHeroScore += iCurMaxStar*coefficient[i];
	}
	m_AllHeroEquipStarScore = PerHeroScore*iHeroCount;
}


int SC_ClientSkillInterface::GetAllToyPetCount()
{
	return CF_Pet::GetSingleton().GetEntryCount();
}

//当前已拥有玩具战力总和
combat_score_type SC_ClientSkillInterface::GetOwnedToyPetScore()
{
	LC_ClientToyPetManager* pkToyPetMgr = LC_ClientToyPetManager::GetSingletonPtr();
	return pkToyPetMgr->GetTotalCombateScore();
}

int SC_ClientSkillInterface::GetOwnedToyPetCount()
{
	LC_ClientToyPetManager* pkToyPetMgr = LC_ClientToyPetManager::GetSingletonPtr();
	return pkToyPetMgr->GetCurToyPetCount();
}

//所有玩具战力总和
void SC_ClientSkillInterface::CalculateToyPetMaxScore()
{
	m_AllToyPetScore = 0;
	CF_Pet::DataEntryMapExternIterator kIter = CF_Pet::GetSingleton().GetIterator();
	mem::vector<raw_attr_value_type> kAttributes;
	
	while(kIter.HasMoreElements())
	{
		LC_ClientToyPetManager* pToyMgr = LC_ClientToyPetManager::GetSingletonPtr();
		kAttributes.clear();
		kAttributes.resize(ATT_MAX_COUNT);
		CF_Pet::DataEntry* pkData = kIter.PeekNextValuePtr();
		CF_PetUpgrade::DataEntry *pkEntry = pToyMgr->GetPetUpgradeData(pkData->_cID, pkData->_MaxLevel);
		bool bToyActive = pToyMgr->CheckPetcIDActive(pkData->_cID);
		if(pkEntry != NULL && bToyActive)
		{
			LC_Helper::OnPetLevelUp_Attribute(kAttributes, pkData, pkEntry, true);
			combat_score_type AddScore = LC_Helper::CalcAttributeRawDataCombatScore(kAttributes, false);
			m_AllToyPetScore += AddScore;
		}
		kIter.MoveNext();
	}
}

//幻兽当前属性等级之和
combat_score_type SC_ClientSkillInterface::GetOwnedVehicleAttrScore()
{
	int iType = 0; //幻兽
	LC_ClientGamePlayer* pkPlayer = GetPlayerPtr();
	if (!pkPlayer)
	{
		return 0;
	}
	LC_SubUpgradeData *pkData = pkPlayer->GetSubordinateAsset().GetSubData(iType);
	if(!pkData)
	{
		return 0;
	}
	int iStage = pkPlayer->GetSubordinateStage(iType);;
	m_OwnedVehicleAttrLevelScore = 0;
	LC_ActorAttributeMap attrMap;
	for(int i = 1; i <= iStage; ++i)
	{
		LC_SubordinateDataEntry* pkEntry = SafeGetSubDataEntry(iType, i);
		if (NULL != pkEntry)
		{
			if(i<iStage)
			{
				for(mem::vector<int>::iterator it1 = pkEntry->_sAttrTypes.begin(); it1 != pkEntry->_sAttrTypes.end(); ++it1)
				{
					LC_Helper::GetCimeliaMapByAttrID(&attrMap, pkEntry->_iAttrFuncID, *it1, pkEntry->_iAttrMaxLevel);
				}

			}
			else
			{
				for(mem::map<int, int>::iterator it2 = pkData->m_kAttrLevelRecords.begin(); it2 != pkData->m_kAttrLevelRecords.end(); ++it2)
				{
					LC_Helper::GetCimeliaMapByAttrID(&attrMap, pkEntry->_iAttrFuncID, it2->first, it2->second);
				}
			}
		}
	}
	attrMap.RecomputeAll();
	m_OwnedVehicleAttrLevelScore = LC_Helper::CalcAttributeMapCombatScore(&attrMap, false);
	return m_OwnedVehicleAttrLevelScore;
}
//幻兽已拥有皮肤增加属性战力
combat_score_type SC_ClientSkillInterface::GetOwnedVehicleSkinScore()
{
	LC_ClientGamePlayer* pkPlayer = GetPlayerPtr();
	if(!pkPlayer)
	{
		return 0;
	}
	LC_SubUpgradeData *pkData = pkPlayer->GetSubordinateAsset().GetSubData(0);
	if(!pkData)
	{
		return 0;
	}
	combat_score_type OwnedVehicleSkinScore = 0;
	LC_ActorAttributeMap attrMap;
	LC_SubUpgradeData::TransformMap::iterator it = pkData->m_kTansfroms.begin();
	for(; it != pkData->m_kTansfroms.end(); ++it)
	{
		CF_SubordinateTransform::DataEntry *pkTransfromData = SafeGetCSVFileDataEntryPtr<CF_SubordinateTransform>(it->first);
		if(pkTransfromData != NULL)
		{
			attrMap.Clear();
			LC_Helper::AttributeMapByAttrFunc(&attrMap, pkTransfromData->_iAttrFunc);
			attrMap.RecomputeAll();
			combat_score_type AddScore = LC_Helper::CalcAttributeMapCombatScore(&attrMap, false);
			OwnedVehicleSkinScore += AddScore;
		}
	}
	return OwnedVehicleSkinScore;
}

combat_score_type SC_ClientSkillInterface::_getOwnedCimeliaEquipScore(int iPackType)
{
	LC_ClientGamePlayer* pkPlayer = GetPlayerPtr();
	if(!pkPlayer)
	{
		return 0;
	}

	combat_score_type OwnedVehicleEquipScore = 0;
	EquipVec& equipList = pkPlayer->GetCommander()->GetEquipList(iPackType);
	for (size_t i = 0; i < equipList.size(); ++i)
	{
		LC_BackPackEntry& pkEntry = equipList[i];
		if(pkEntry.GetValid())
		{
			OwnedVehicleEquipScore += GetEquipScoreByItemID(pkEntry.GetTypeID());
		}
	}

	return OwnedVehicleEquipScore;
}

//幻兽
combat_score_type SC_ClientSkillInterface::GetOwnedVehicleDrugScore()
{
	LC_ClientGamePlayer* pkPlayer = GetPlayerPtr();

	if (!pkPlayer)
	{
		return 0;
	}
	int iType = 0;
	int iStage = pkPlayer->GetSubordinateStage(iType);
	combat_score_type OwnedVehicleDrugScore = 0;
	StringType DrugInfo = pkPlayer->GetCimeliaDrugInfo(iType);
	TStringVector kVector = UT_BaseStringUtility::SplitString(DrugInfo, '#');

	combat_score_type VehicleAttrCurLevelMaxScore = GetSubordinateMaxAttriScoreByStage(iType,iStage);

	if (kVector.size() >= 3)
	{
		int curAttrVal = atoi(kVector[0].c_str());
		int curAttrPer = atoi(kVector[2].c_str());
		OwnedVehicleDrugScore = CalculateDrugScore(iType,VehicleAttrCurLevelMaxScore,iStage,curAttrVal,curAttrPer);
	}
	return OwnedVehicleDrugScore;
}

//幻兽属性加成战力最大值
void SC_ClientSkillInterface::CalculateVehicleAttrMaxScore()
{
	LC_ClientGamePlayer* pkGamePlayer = GetPlayerPtr();
	if (!pkGamePlayer)
	{
		return;
	}
	int iCommanderLevel = pkGamePlayer->GetCommanderLevel();
	int iType = 0;
	m_VehicleAttrLevelMaxScore = 0;
	
	int iMaxStage = GetMaxSubordinateStageByCommanderLevel(iType,iCommanderLevel);
	m_VehicleAttrLevelMaxScore = GetSubordinateMaxAttriScoreByStage(iType,iMaxStage);
}

//幻兽皮肤加成战力最大值
void SC_ClientSkillInterface::CalculateVehicleSkinMaxScore()
{
	CF_SubordinateTransform::DataEntryMapExternIterator kIter = CF_SubordinateTransform::GetSingleton().GetIterator();
	LC_ActorAttributeMap m_kAttributes;
	m_VehicleSkinMaxScore = 0;
	while(kIter.HasMoreElements())
	{
		CF_SubordinateTransform::DataEntry* pkData = kIter.PeekNextValuePtr();
		if (pkData->_iType == 0)
		{
			m_kAttributes.Clear();
			LC_Helper::AttributeMapByAttrFunc(&m_kAttributes, pkData->_iAttrFunc);
			m_kAttributes.RecomputeAll();
			combat_score_type AddScore = LC_Helper::CalcAttributeMapCombatScore(&m_kAttributes, false);
			m_VehicleSkinMaxScore += AddScore;

		}
		kIter.MoveNext();
	}
}

//幻兽装备加成战力最大值
void SC_ClientSkillInterface::CalculateVehicleEquipMaxScore()
{
	LC_ClientGamePlayer* pkGamePlayer = GetPlayerPtr();
	if (!pkGamePlayer)
	{
		return;
	}
	int iType = 0; //幻兽
	int iSubStage = pkGamePlayer->GetSubordinateStage(iType);;
	const Utility::GlobalSettings& pSetting = GetGlobalSetting;
	int OpenStage[4] = {pSetting.Vehicle_0,pSetting.Vehicle_1,pSetting.Vehicle_2,pSetting.Vehicle_3};

	//最好的四件装备 ：幻甲，幻魂，幻灵，幻心
	int BestEquipID[4] = {12021050,12022050,12023050,12024050};
	m_VehicleEquipMaxScore = 0;
	combat_score_type PerHeroScore = 0;
	for (int i = 0;i < 4;i++)
	{
		if (iSubStage >= OpenStage[i])
		{
			combat_score_type AddScore = (GetEquipScoreByItemID(BestEquipID[i]));
			m_VehicleEquipMaxScore += AddScore;
		}

	}
}

void SC_ClientSkillInterface::CalculateVehicleDrugMaxScore()
{
	LC_ClientGamePlayer* pkGamePlayer = GetPlayerPtr();
	if (!pkGamePlayer)
	{
		return;
	}
	int iCommanderLevel = pkGamePlayer->GetCommanderLevel();
	int iMaxStage = GetMaxSubordinateStageByCommanderLevel(0,iCommanderLevel);
	int iLoopCount = 0;

	int MaxDrug0 = 0;//潜能单最大数量
	int MaxDrug1 = 0;//幻化单最大数量
	CF_Subordinate::DataEntryMapExternIterator kIter = CF_Subordinate::GetSingleton().GetIterator();
	while(kIter.HasMoreElements())
	{
		CF_Subordinate::DataEntry* pkData = kIter.PeekNextValuePtr();
		if (pkData->_iType == 0)
		{
			
			if (iLoopCount >= iMaxStage)
			{
				break;
			}
			if (pkData->_lMaxAttrPlusVal > MaxDrug0)
			{
				MaxDrug0 = pkData->_lMaxAttrPlusVal;
			}
			if (pkData->_lMaxAttrPlusPercent > MaxDrug1)
			{
				MaxDrug1 = pkData->_lMaxAttrPlusPercent;
			}

			iLoopCount++;
		}
		kIter.MoveNext();
	}
	m_VehicleDrugMaxScore = CalculateDrugScore(0,m_VehicleAttrLevelMaxScore,iMaxStage,MaxDrug0,MaxDrug1);
}

//萌宠属性加成战力当前值
combat_score_type SC_ClientSkillInterface::GetOwnedDevaEyeAttrScore()
{
	int iType = 2; //萌宠
	LC_ClientGamePlayer* pkPlayer = GetPlayerPtr();
	if (!pkPlayer)
	{
		return 0;
	}
	LC_SubUpgradeData *pkData = pkPlayer->GetSubordinateAsset().GetSubData(iType);
	if(!pkData)
	{
		return 0;
	}
	int iStage = pkPlayer->GetSubordinateStage(iType);;
	m_OwnedDevaEyeAttrLevelScore = 0;
	LC_ActorAttributeMap attrMap;
	for(int i = 1; i <= iStage; ++i)
	{
		LC_SubordinateDataEntry* pkEntry = SafeGetSubDataEntry(iType, i);
		if (NULL != pkEntry)
		{
			if(i<iStage)
			{
				for(mem::vector<int>::iterator it1 = pkEntry->_sAttrTypes.begin(); it1!=pkEntry->_sAttrTypes.end(); ++it1)
				{
					LC_Helper::GetCimeliaMapByAttrID(&attrMap, pkEntry->_iAttrFuncID, *it1, pkEntry->_iAttrMaxLevel);
				}
			}
			else
			{
				for(mem::map<int, int>::iterator it2 = pkData->m_kAttrLevelRecords.begin(); it2 != pkData->m_kAttrLevelRecords.end(); ++it2)
				{
					LC_Helper::GetCimeliaMapByAttrID(&attrMap, pkEntry->_iAttrFuncID, it2->first, it2->second);
				}
			}
		}
	}
	attrMap.RecomputeAll();
	m_OwnedDevaEyeAttrLevelScore = LC_Helper::CalcAttributeMapCombatScore(&attrMap, false);
	return m_OwnedDevaEyeAttrLevelScore;
}

//萌宠皮肤加成战力当前值
combat_score_type SC_ClientSkillInterface::GetOwnedDevaEyeSkinScore()
{
	LC_ClientGamePlayer* pkPlayer = GetPlayerPtr();
	if(!pkPlayer)
	{
		return 0;
	}
	LC_SubUpgradeData *pkData = pkPlayer->GetSubordinateAsset().GetSubData(2);
	if(!pkData)
	{
		return 0;
	}
	combat_score_type OwnedDevaEyeSkinScore = 0;
	LC_ActorAttributeMap attrMap;
	LC_SubUpgradeData::TransformMap::iterator it = pkData->m_kTansfroms.begin();
	for(; it != pkData->m_kTansfroms.end(); ++it)
	{
		CF_SubordinateTransform::DataEntry *pkTransfromData = SafeGetCSVFileDataEntryPtr<CF_SubordinateTransform>(it->first);
		if(pkTransfromData != NULL)
		{
			attrMap.Clear();
			LC_Helper::AttributeMapByAttrFunc(&attrMap, pkTransfromData->_iAttrFunc);
			attrMap.RecomputeAll();
			combat_score_type AddScore=LC_Helper::CalcAttributeMapCombatScore(&attrMap, false);
			OwnedDevaEyeSkinScore += AddScore;
		}
	}
	return OwnedDevaEyeSkinScore;
}


combat_score_type SC_ClientSkillInterface::GetOwnedDevaEyeDrugScore()
{
	LC_ClientGamePlayer* pkPlayer = GetPlayerPtr();

	if (!pkPlayer)
	{
		return 0;
	}
	int iType = 2;
	int iStage = pkPlayer->GetSubordinateStage(iType);
	StringType DrugInfo = pkPlayer->GetCimeliaDrugInfo(iType);
	TStringVector kVector = UT_BaseStringUtility::SplitString(DrugInfo, '#');
	combat_score_type OwnedDevaEyeDrugScore = 0;

	combat_score_type DevaEyeAttrCurLevelMaxScore = GetSubordinateMaxAttriScoreByStage(iType,iStage);

	if (kVector.size()>3)
	{
		int curAttrVal = atoi(kVector[0].c_str());
		int curAttrPer = atoi(kVector[2].c_str());
		OwnedDevaEyeDrugScore = CalculateDrugScore(iType,DevaEyeAttrCurLevelMaxScore,iStage,curAttrVal,curAttrPer);
	}
	return OwnedDevaEyeDrugScore;
}

//萌宠属性加成战力最大值
void SC_ClientSkillInterface::CalculateDevaEyeAttrMaxScore()
{
	LC_ClientGamePlayer* pkGamePlayer = GetPlayerPtr();
	if (!pkGamePlayer)
	{
		return;
	}
	int iCommanderLevel = pkGamePlayer->GetCommanderLevel();

	int iType = 2;
	int iMaxStage = GetMaxSubordinateStageByCommanderLevel(iType,iCommanderLevel);
	m_DevaEyeAttrLevelMaxScore = GetSubordinateMaxAttriScoreByStage(iType,iMaxStage);
}

//萌宠皮肤加成战力最大值
void SC_ClientSkillInterface::CalculateDevaEyeSkinMaxScore()
{
	CF_SubordinateTransform::DataEntryMapExternIterator kIter = CF_SubordinateTransform::GetSingleton().GetIterator();
	LC_ActorAttributeMap m_kAttributes;
	m_DevaEyeSkinMaxScore = 0;
	while(kIter.HasMoreElements())
	{
		CF_SubordinateTransform::DataEntry* pkData = kIter.PeekNextValuePtr();
		if (pkData->_iType == 2)
		{
			m_kAttributes.Clear();
			LC_Helper::AttributeMapByAttrFunc(&m_kAttributes, pkData->_iAttrFunc);
			m_kAttributes.RecomputeAll();
			combat_score_type AddScore = LC_Helper::CalcAttributeMapCombatScore(&m_kAttributes, false);
			m_DevaEyeSkinMaxScore += AddScore;

		}
		kIter.MoveNext();
	}
}

//萌宠装备加成战力最大值
void SC_ClientSkillInterface::CalculateDevaEyeEquipMaxScore()
{
	LC_ClientGamePlayer* pkGamePlayer = GetPlayerPtr();
	if (!pkGamePlayer)
	{
		return;
	}
	int iType = 2; //萌宠
	int iSubStage = pkGamePlayer->GetSubordinateStage(iType);;
	const Utility::GlobalSettings& pSetting = GetGlobalSetting;
	int OpenStage[4] = {pSetting.Devaeye_0,pSetting.Devaeye_1,pSetting.Devaeye_2,pSetting.Devaeye_3};

	m_DevaEyeEquipMaxScore = 0;
	//最好的四件装备 ：甲，魂，灵，心
	int BestEquipID[4] = {12041050,12042050,12043050,12044050};
	for (int i = 0;i < 4;i++)
	{
		if (iSubStage >= OpenStage[i])
		{
			combat_score_type AddScore = GetEquipScoreByItemID(BestEquipID[i]);
			m_DevaEyeEquipMaxScore += AddScore;
		}
	}
}



void SC_ClientSkillInterface::CalculateDevaEyeDrugMaxScore()
{
	LC_ClientGamePlayer* pkGamePlayer = GetPlayerPtr();
	if (!pkGamePlayer)
	{
		return;
	}
	int iCommanderLevel = pkGamePlayer->GetCommanderLevel();
	int iMaxStage = GetMaxSubordinateStageByCommanderLevel(2,iCommanderLevel);
	int iLoopCount = 0;

	int MaxDrug0 = 0;//潜能单最大数量
	int MaxDrug1 = 0;//幻化单最大数量
	CF_Subordinate::DataEntryMapExternIterator kIter = CF_Subordinate::GetSingleton().GetIterator();
	while(kIter.HasMoreElements())
	{
		CF_Subordinate::DataEntry* pkData = kIter.PeekNextValuePtr();
		if (pkData->_iType == 2)
		{
			if (iLoopCount >= iMaxStage)
			{
				break;;
			}
			if (pkData->_lMaxAttrPlusVal>MaxDrug0)
			{
				MaxDrug0 = pkData->_lMaxAttrPlusVal;
			}
			if (pkData->_lMaxAttrPlusPercent>MaxDrug1)
			{
				MaxDrug1 = pkData->_lMaxAttrPlusPercent;
			}

			iLoopCount++;
		}
		kIter.MoveNext();
	}
	m_DevaEyeDrugMaxScore = CalculateDrugScore(2,m_DevaEyeAttrLevelMaxScore,iMaxStage,MaxDrug0,MaxDrug1);
}

//背饰属性加成战力当前值
combat_score_type SC_ClientSkillInterface::GetOwnedWingAttrScore()
{
	int iType = 8; //背饰
	LC_ClientGamePlayer* pkPlayer = GetPlayerPtr();
	if (!pkPlayer)
	{
		return 0;
	}
	LC_SubUpgradeData *pkData = pkPlayer->GetSubordinateAsset().GetSubData(iType);
	if(!pkData)
	{
		return 0;
	}
	int iStage = pkPlayer->GetSubordinateStage(iType);;
	m_OwnedWingAttrLevelScore = 0;
	LC_ActorAttributeMap attrMap;
	for(int i = 1; i <= iStage; ++i)
	{
		LC_SubordinateDataEntry* pkEntry = SafeGetSubDataEntry(iType, i);
		if (NULL != pkEntry)
		{
			if(i<iStage)
			{
				for(mem::vector<int>::iterator it1 = pkEntry->_sAttrTypes.begin(); it1 != pkEntry->_sAttrTypes.end(); ++it1)
				{
					LC_Helper::GetCimeliaMapByAttrID(&attrMap, pkEntry->_iAttrFuncID, *it1, pkEntry->_iAttrMaxLevel);
				}
			}
			else
			{
				for(mem::map<int, int>::iterator it2 = pkData->m_kAttrLevelRecords.begin(); it2 != pkData->m_kAttrLevelRecords.end(); ++it2)
				{
					LC_Helper::GetCimeliaMapByAttrID(&attrMap, pkEntry->_iAttrFuncID, it2->first, it2->second);
				}
			}
		}
	}
	attrMap.RecomputeAll();
	m_OwnedWingAttrLevelScore = LC_Helper::CalcAttributeMapCombatScore(&attrMap, false);
	return m_OwnedWingAttrLevelScore;
}

//背饰皮肤加成战力当前值
combat_score_type SC_ClientSkillInterface::GetOwnedWingSkinScore()
{
	LC_ClientGamePlayer* pkPlayer = GetPlayerPtr();
	if(!pkPlayer)
	{
		return 0;
	}
	LC_SubUpgradeData *pkData = pkPlayer->GetSubordinateAsset().GetSubData(8);
	if(!pkData)
	{
		return 0;
	}
	combat_score_type OwnedWingSkinScore = 0;
	LC_ActorAttributeMap attrMap;
	LC_SubUpgradeData::TransformMap::iterator it = pkData->m_kTansfroms.begin();
	for(; it != pkData->m_kTansfroms.end(); ++it)
	{
		CF_SubordinateTransform::DataEntry *pkTransfromData = SafeGetCSVFileDataEntryPtr<CF_SubordinateTransform>(it->first);
		if(pkTransfromData != NULL)
		{
			attrMap.Clear();
			LC_Helper::AttributeMapByAttrFunc(&attrMap, pkTransfromData->_iAttrFunc);
			attrMap.RecomputeAll();
			combat_score_type AddScore = LC_Helper::CalcAttributeMapCombatScore(&attrMap, false);
			OwnedWingSkinScore += AddScore;
		}
	}
	return OwnedWingSkinScore;
}

combat_score_type SC_ClientSkillInterface::GetOwnedWingDrugScore()
{
	LC_ClientGamePlayer* pkPlayer = GetPlayerPtr();

	if (!pkPlayer)
	{
		return 0;
	}
	int iType = 8;
	int iStage = pkPlayer->GetSubordinateStage(iType);
	StringType DrugInfo = pkPlayer->GetCimeliaDrugInfo(iType);
	TStringVector kVector = UT_BaseStringUtility::SplitString(DrugInfo, '#');
	combat_score_type OwnedWingDrugScore = 0;

	int WingAttrCurLevelMaxScore = GetSubordinateMaxAttriScoreByStage(iType,iStage);
	if (kVector.size()>3)
	{
		int curAttrVal = atoi(kVector[0].c_str());
		int curAttrPer = atoi(kVector[2].c_str());
		OwnedWingDrugScore = CalculateDrugScore(iType,WingAttrCurLevelMaxScore,iStage,curAttrVal,curAttrPer);
	}
	return OwnedWingDrugScore;
}

//背饰属性加成战力最大值
void SC_ClientSkillInterface::CalculateWingAttrMaxScore()
{
	LC_ClientGamePlayer* pkGamePlayer = GetPlayerPtr();
	if (!pkGamePlayer)
	{
		return;
	}
	int iCommanderLevel = pkGamePlayer->GetCommanderLevel();

	int iType = 8;
	m_WingAttrLevelMaxScore = 0;
	LC_ActorAttributeMap attrMap;

	int iMaxStage = GetMaxSubordinateStageByCommanderLevel(iType,iCommanderLevel);
	m_WingAttrLevelMaxScore = GetSubordinateMaxAttriScoreByStage(iType,iMaxStage);
}

//背饰皮肤加成战力最大值
void SC_ClientSkillInterface::CalculateWingSkinMaxScore()
{
	CF_SubordinateTransform::DataEntryMapExternIterator kIter = CF_SubordinateTransform::GetSingleton().GetIterator();
	LC_ActorAttributeMap m_kAttributes;
	m_WingSkinMaxScore = 0;
	while(kIter.HasMoreElements())
	{
		CF_SubordinateTransform::DataEntry* pkData = kIter.PeekNextValuePtr();
		if (pkData->_iType == 8)
		{
			m_kAttributes.Clear();
			LC_Helper::AttributeMapByAttrFunc(&m_kAttributes, pkData->_iAttrFunc);
			m_kAttributes.RecomputeAll();
			combat_score_type AddScore = LC_Helper::CalcAttributeMapCombatScore(&m_kAttributes, false);;
			m_WingSkinMaxScore += AddScore;

		}
		kIter.MoveNext();
	}
}

//背饰装备加成战力最大值
void SC_ClientSkillInterface::CalculateWingEquipMaxScore()
{
	LC_ClientGamePlayer* pkGamePlayer = GetPlayerPtr();
	if (!pkGamePlayer)
	{
		return;
	}
	int iType = 8; //背饰
	int iSubStage = pkGamePlayer->GetSubordinateStage(iType);;
	const Utility::GlobalSettings& pSetting = GetGlobalSetting;
	int OpenStage[4] = {pSetting.Wing_0,pSetting.Wing_1,pSetting.Wing_2,pSetting.Wing_3};

	m_WingEquipMaxScore = 0;
	//最好的四件装备 ：甲，魂，灵，心
	int BestEquipID[4] = {12031050,12032050,12033050,12034050};
	for (int i = 0;i < 4;i++)
	{
		if (iSubStage >= OpenStage[i])
		{
			combat_score_type AddScore = GetEquipScoreByItemID(BestEquipID[i]);
			m_WingEquipMaxScore += AddScore;
		}
	}
}

void SC_ClientSkillInterface::CalculateWingDrugMaxScore()
{
	LC_ClientGamePlayer* pkGamePlayer = GetPlayerPtr();
	if (!pkGamePlayer)
	{
		return;
	}
	int iCommanderLevel = pkGamePlayer->GetCommanderLevel();
	int iMaxStage = GetMaxSubordinateStageByCommanderLevel(0,iCommanderLevel);
	int iLoopCount=0;

	int MaxDrug0 = 0;//潜能单最大数量
	int MaxDrug1 = 0;//幻化单最大数量
	CF_Subordinate::DataEntryMapExternIterator kIter = CF_Subordinate::GetSingleton().GetIterator();
	while(kIter.HasMoreElements())
	{
		CF_Subordinate::DataEntry* pkData = kIter.PeekNextValuePtr();
		if (pkData->_iType == 8)
		{
			if (iLoopCount >= iMaxStage)
			{
				break;;
			}
			if (pkData->_lMaxAttrPlusVal>MaxDrug0)
			{
				MaxDrug0 = pkData->_lMaxAttrPlusVal;
			}
			if (pkData->_lMaxAttrPlusPercent>MaxDrug1)
			{
				MaxDrug1 = pkData->_lMaxAttrPlusPercent;
			}
			iLoopCount++;
		}
		kIter.MoveNext();
	}
	m_WingDrugMaxScore = CalculateDrugScore(8,m_WingAttrLevelMaxScore,iMaxStage,MaxDrug0,MaxDrug1);
}

combat_score_type SC_ClientSkillInterface::GetOwnedCommanderEquipAttrScore()
{
	LC_ClientGamePlayer* pkPlayer = GetPlayerPtr();
	if (!pkPlayer)
	{
		return 0;
	}

	LC_ActorAttributeMap attrMap;
	
	const EquipVec& equipList = pkPlayer->GetCommander()->GetEquipList(CommandPack);
	for (size_t i = 0; i < equipList.size(); ++i)
	{
		const LC_BackPackEntry& pkEntry = equipList[i];
		if(pkEntry.GetValid())
		{
			const int iItemID = pkEntry.GetTypeID();
			CF_ItemList::DataEntry *pkEntry=SafeGetCSVFileDataEntryPtr<CF_ItemList>(iItemID);
			if (pkEntry)
			{
				LC_Helper::GetItemBaseAttributeMap(iItemID, &attrMap);
				LC_Helper::GetNonpareilAttrMap(&attrMap,pkEntry->_sNonpareilEffect);
			}
		}
	}

	attrMap.RecomputeAll();
	m_EquipAttrCurScore = LC_Helper::CalcAttributeMapCombatScore(&attrMap, false);
	return m_EquipAttrCurScore;
}

combat_score_type SC_ClientSkillInterface::GetOwnedCommanderEquipStrengthenScore()
{
	LC_ClientGamePlayer* pkPlayer = GetPlayerPtr();
	if (!pkPlayer)
	{
		return 0;
	}

	combat_score_type OwnedScore = 0;

	int64_t iScore = 0;
	int iCurReinforceLv = 999;

	const EquipVec& equiplist = pkPlayer->GetCommander()->GetEquipList(CommandPack);
	for (size_t i = 0; i < equiplist.size(); ++i)
	{
		int iItemID = 0;
		const LC_BackPackEntry& pkEntry = equiplist[i];
		if (pkEntry.GetValid())
		{
			iItemID = pkEntry.GetTypeID();
		}

		int iReinforceLevel = pkPlayer->GetControllerEquipSlotReinforceLevel(i);
		if(iReinforceLevel < iCurReinforceLv)
			iCurReinforceLv = iReinforceLevel ;

		OwnedScore += LC_Helper::GetCommanderEquipSlotScore(i, iReinforceLevel, 0, iItemID);
	}

	LC_ActorAttributeMap attrMap;
	//强化套装
	CF_EquipCommanderSlotReinforceSuit::DataEntryMapExternIterator kIter = CF_EquipCommanderSlotReinforceSuit::GetSingletonPtr()->GetIterator();
	const CF_EquipCommanderSlotReinforceSuit::DataEntry *pkSuitData = NULL;
	for( ; kIter.HasMoreElements(); kIter.MoveNext())
	{
		const CF_EquipCommanderSlotReinforceSuit::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		if(iCurReinforceLv >= pkEntry->_iEquipLevel)
		{
			pkSuitData = pkEntry;
		}
	}
	if(pkSuitData != NULL)
	{
		const Utility::UT_SIMDataList& attr = pkSuitData->Attribute;
		for(Utility::UT_SIMDataList::const_iterator it=attr.begin(); it!=attr.end(); ++it)
		{
			attrMap.ModifyAttributeByPart(it->ParamA(), it->ParamB(), it->ParamC(), true);
		}
	}
	attrMap.RecomputeAll();

	OwnedScore += LC_Helper::CalcAttributeMapCombatScore(&attrMap, false);
	return OwnedScore;
}

combat_score_type SC_ClientSkillInterface::GetOwnedCommanderEquipStarScore()
{
	LC_ClientGamePlayer* pkPlayer = GetPlayerPtr();
	if (!pkPlayer)
	{
		return 0;
	}
	
	const EquipVec& equiplist = pkPlayer->GetCommander()->GetEquipList(CommandPack);
	
	combat_score_type JinglianScore=0;
	int MinJinglianLevel=30;
	int EquipID[COMMANDER_EQUIP_SLOT_SIZE] = {0,0,0,0,0,0,0,0};
	int EquipJinglianLv[COMMANDER_EQUIP_SLOT_SIZE] = {0,0,0,0,0,0,0,0};
	for (size_t i = 0; i < equiplist.size(); ++i)
	{
		const LC_BackPackEntry& pItemData = equiplist[i];
		int JinglianLv=pkPlayer->GetCommanderEquipRefineLevel(i);
		int iTypeId=pItemData.GetTypeID();
		if (iTypeId!=0)
		{
			EquipID[i]=iTypeId;
			EquipJinglianLv[i]=JinglianLv;
		}
		if (MinJinglianLevel>JinglianLv)
		{
			MinJinglianLevel=JinglianLv;
			
		}
	}
	int PropertySpecialChange=0;
	CF_EquipCommanderJinglian::DataEntryMapExternIterator kIter = CF_EquipCommanderJinglian::GetSingletonPtr()->GetIterator();
	for( ; kIter.HasMoreElements(); kIter.MoveNext())
	{
		const CF_EquipCommanderJinglian::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		if(MinJinglianLevel >= pkEntry->_iJinglianLevel)
		{
			PropertySpecialChange = pkEntry->_PropertySpecialChange > PropertySpecialChange ? pkEntry->_PropertySpecialChange : PropertySpecialChange;
		}
		else
		{
			break;
		}
	}

	LC_ActorAttributeMap attrMap;
	LC_ActorAttributeMap attrMap2;
	combat_score_type PercentIncreasedCombat=0;
	for (int i=0;i<COMMANDER_EQUIP_SLOT_SIZE;i++)
	{
		attrMap.Clear();
		LC_Helper::GetItemBaseAttributeMap(EquipID[i],&attrMap);
		attrMap.RecomputeAll();
		combat_score_type OriginCombat=LC_Helper::CalcAttributeMapCombatScore(&attrMap, false);
		LC_Helper::AddAttrmapByPercent(&attrMap,PropertySpecialChange);
		attrMap.RecomputeAll();
		combat_score_type NewCombat=LC_Helper::CalcAttributeMapCombatScore(&attrMap, false);
		PercentIncreasedCombat+=(NewCombat-OriginCombat);

		CF_EquipCommanderJinglian::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_EquipCommanderJinglian>(i*31+EquipJinglianLv[i]+1);
		if (pkEntry)
		{
			LC_Helper::GetItemBaseAttributeMap(EquipID[i],&attrMap2);
			const Utility::UT_SIMDataList& attr = pkEntry->Attribute;
			for(Utility::UT_SIMDataList::const_iterator it=attr.begin(); it!=attr.end(); ++it)
			{
				attrMap2.ModifyAttributeByPart(it->ParamA(), it->ParamB(), it->ParamC(), true);
			}
		}
	}
	
	int JinglianSuitLv=0;
	CF_EquipCommanderJinglianSuit::DataEntryMapExternIterator kIter2 = CF_EquipCommanderJinglianSuit::GetSingletonPtr()->GetIterator();
	for( ; kIter2.HasMoreElements(); kIter2.MoveNext())
	{
		const CF_EquipCommanderJinglianSuit::DataEntry *pkEntry = kIter2.PeekNextValuePtr();
		if(MinJinglianLevel >= pkEntry->_iEquipLevel)
		{
			JinglianSuitLv=pkEntry->_iLevel;
		}
		else
		{
			break;
		}
	}
	CF_EquipCommanderJinglianSuit::DataEntry* JinglianSuitEntry = SafeGetCSVFileDataEntryPtr<CF_EquipCommanderJinglianSuit>(JinglianSuitLv);
	if (JinglianSuitEntry)
	{
		const Utility::UT_SIMDataList& attr = JinglianSuitEntry->Attribute;
		for(Utility::UT_SIMDataList::const_iterator it=attr.begin(); it!=attr.end(); ++it)
		{
			attrMap2.ModifyAttributeByPart(it->ParamA(), it->ParamB(), it->ParamC(), true);
		}
	}
	attrMap2.RecomputeAll();
	combat_score_type JinglianSuitScore=LC_Helper::CalcAttributeMapCombatScore(&attrMap2, false);
	combat_score_type TotalScore=PercentIncreasedCombat+JinglianSuitScore-m_EquipAttrCurScore;
	return TotalScore;
}

void SC_ClientSkillInterface::CalculateCommanderEquipAttrMaxScore()
{
	m_CommanderEquipMaxScore=0;
	LC_ClientGamePlayer* pkPlayer = GetPlayerPtr();
	if (!pkPlayer)
	{
		return;
	}
	int BestEquipID[8] = {0,0,0,0,0,0,0,0};
	int iCurMaxLevel=0;
	
	int iCommanderLevel=pkPlayer->GetCommanderLevel();
	for (int i = 1001;i <= 1099;i++)
	{
		CF_ItemEquipLimitFunc::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_ItemEquipLimitFunc>(i);
		if (!pkData)
		{
			break;
		}
		if (iCommanderLevel<pkData->_lLevelLimit)
		{
			break;
		}
		iCurMaxLevel = i-1000;
	}
	//最好的四件装备 ：武器，铠甲，头盔，饰品
	int iQuality=iCurMaxLevel/2+2;
	iQuality=iQuality<5?iQuality:5;
	for (int i=0;i<8;i++)
	{
		BestEquipID[i]=11390000+i*1000+iQuality*100+iCurMaxLevel;
	}

	LC_ActorAttributeMap attrMap;
	//套装
	for (int i=0;i<8;i++)
	{
		CF_ItemList::DataEntry *pkEntry=SafeGetCSVFileDataEntryPtr<CF_ItemList>(BestEquipID[i]);
		LC_Helper::GetItemBaseAttributeMap(BestEquipID[i],&attrMap);
		LC_Helper::GetNonpareilAttrMap(&attrMap,pkEntry->_sNonpareilEffect);
	}
	attrMap.RecomputeAll();
	m_CommanderEquipMaxScore = LC_Helper::CalcAttributeMapCombatScore(&attrMap, false);
}

void SC_ClientSkillInterface::CalculateCommanderEquipStrengthenMaxScore()
{
	m_CommanderEquipStrengthenMaxScore=0;
	LC_ClientGamePlayer* pkPlayer = GetPlayerPtr();
	if (!pkPlayer)
	{
		return;
	}
	int BestEquipID[8] = {0,0,0,0,0,0,0,0};
	int iCurMaxLevel=0;
	int iCommanderLevel=pkPlayer->GetCommanderLevel();

	for (int i = 1001;i <= 1099;i++)
	{
		CF_ItemEquipLimitFunc::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_ItemEquipLimitFunc>(i);
		if (!pkData)
		{
			break;
		}
		if (iCommanderLevel<pkData->_lLevelLimit)
		{
			break;
		}
		iCurMaxLevel = i-1000;
	}
	//最好的四件装备 ：武器，铠甲，头盔，饰品
	int iQuality=iCurMaxLevel/2+2;
	iQuality=iQuality<5?iQuality:5;
	for (int i=0;i<8;i++)
	{
		BestEquipID[i]=11390000+i*1000+iQuality*100+iCurMaxLevel;
	}

	combat_score_type iScore = 0;
	int iMaxReinforceLevel = 0;
	CF_EquipCommanderSlotReinforce::DataEntryMapExternIterator kIter1 = CF_EquipCommanderSlotReinforce::GetSingletonPtr()->GetIterator();
	for( ; kIter1.HasMoreElements(); kIter1.MoveNext())
	{
		const CF_EquipCommanderSlotReinforce::DataEntry *pkEntry = kIter1.PeekNextValuePtr();
		if (iCommanderLevel>=pkEntry->_iCommanderLevel)
		{
			iMaxReinforceLevel=pkEntry->_iLevel;
		}
		else
		{
			break;
		}
	}
	for (int i = 0; i < COMMANDER_EQUIP_SLOT_SIZE; ++i)
	{
		CF_ItemList::DataEntry *pkEntry=SafeGetCSVFileDataEntryPtr<CF_ItemList>(BestEquipID[i]);
		int iSlotIdx = COMMANDER_EQUIP_SLOT[i];
		if(pkEntry)
		{
			iMaxReinforceLevel=iMaxReinforceLevel < pkEntry->_MaxCommanderEquipReinforceLevel ? iMaxReinforceLevel : pkEntry->_MaxCommanderEquipReinforceLevel;
			m_CommanderEquipStrengthenMaxScore += LC_Helper::GetCommanderEquipSlotScore(iSlotIdx, iMaxReinforceLevel, 0, BestEquipID[i]);
		}
	}
	LC_ActorAttributeMap attrMap;
	//强化套装
	CF_EquipCommanderSlotReinforceSuit::DataEntryMapExternIterator kIter = CF_EquipCommanderSlotReinforceSuit::GetSingletonPtr()->GetIterator();
	const CF_EquipCommanderSlotReinforceSuit::DataEntry *pkSuitData = NULL;
	for( ; kIter.HasMoreElements(); kIter.MoveNext())
	{
		const CF_EquipCommanderSlotReinforceSuit::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		if(iMaxReinforceLevel >= pkEntry->_iEquipLevel)
		{
			pkSuitData = pkEntry;
		}
	}
	if(pkSuitData != NULL)
	{
		const Utility::UT_SIMDataList& attr = pkSuitData->Attribute;
		for(Utility::UT_SIMDataList::const_iterator it=attr.begin(); it!=attr.end(); ++it)
		{
			attrMap.ModifyAttributeByPart(it->ParamA(), it->ParamB(), it->ParamC(), true);
		}
	}
	attrMap.RecomputeAll();

	m_CommanderEquipStrengthenMaxScore += LC_Helper::CalcAttributeMapCombatScore(&attrMap, false);
}

void SC_ClientSkillInterface::CalculateCommanderEquipStarMaxScore()
{

	m_CommanderEquipStarMaxScore=0;
	LC_ClientGamePlayer* pkPlayer = GetPlayerPtr();
	if (!pkPlayer)
	{
		return;
	}
	int BestEquipID[8] = {0,0,0,0,0,0,0,0};
	int iCurMaxLevel=0;
	int iCommanderLevel=pkPlayer->GetCommanderLevel();

	for (int i = 1001;i <= 1099;i++)
	{
		CF_ItemEquipLimitFunc::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_ItemEquipLimitFunc>(i);
		if (!pkData)
		{
			break;
		}
		if (iCommanderLevel<pkData->_lLevelLimit)
		{
			break;
		}
		iCurMaxLevel = i-1000;
	}
	int iQuality=iCurMaxLevel/2+2;
	iQuality=iQuality<5?iQuality:5;
	for (int i=0;i<8;i++)
	{
		BestEquipID[i]=11390000+i*1000+iQuality*100+iCurMaxLevel;
	}

	int iMaxJinglianLv=0;
	int PropertySpecialChange=0;
	
	CF_EquipCommanderJinglian::DataEntryMapExternIterator kIter = CF_EquipCommanderJinglian::GetSingletonPtr()->GetIterator();
	bool bFinished=false;
	for( ; kIter.HasMoreElements(); kIter.MoveNext())
	{
		const CF_EquipCommanderJinglian::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		if(iCommanderLevel >= pkEntry->_iCommanderLevel)
		{
			iMaxJinglianLv=pkEntry->_iJinglianLevel;
			PropertySpecialChange = pkEntry->_PropertySpecialChange > PropertySpecialChange ? pkEntry->_PropertySpecialChange : PropertySpecialChange;
			bFinished=true;
		}
		else if (bFinished)
		{
			iMaxJinglianLv=pkEntry->_iJinglianLevel;
			PropertySpecialChange = pkEntry->_PropertySpecialChange > PropertySpecialChange ? pkEntry->_PropertySpecialChange : PropertySpecialChange;
			break;
		}
	}
	iMaxJinglianLv=iMaxJinglianLv > 30?30:iMaxJinglianLv;



	LC_ActorAttributeMap attrMap;
	LC_ActorAttributeMap attrMap2;
	combat_score_type PercentIncreasedCombat=0;
	for (int i=0;i<8;i++)
	{
		attrMap.Clear();
		LC_Helper::GetItemBaseAttributeMap(BestEquipID[i],&attrMap);
		attrMap.RecomputeAll();
		combat_score_type OriginCombat=LC_Helper::CalcAttributeMapCombatScore(&attrMap, false);
		LC_Helper::AddAttrmapByPercent(&attrMap,PropertySpecialChange);
		attrMap.RecomputeAll();
		combat_score_type NewCombat=LC_Helper::CalcAttributeMapCombatScore(&attrMap, false);
		PercentIncreasedCombat+=(NewCombat-OriginCombat);

		CF_EquipCommanderJinglian::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_EquipCommanderJinglian>(i*31+iMaxJinglianLv+1);
		if (pkEntry)
		{
			LC_Helper::GetItemBaseAttributeMap(BestEquipID[i],&attrMap2);
			const Utility::UT_SIMDataList& attr = pkEntry->Attribute;
			for(Utility::UT_SIMDataList::const_iterator it=attr.begin(); it!=attr.end(); ++it)
			{
				attrMap2.ModifyAttributeByPart(it->ParamA(), it->ParamB(), it->ParamC(), true);
			}
		}
	}

	int iMaxJinglianSuitLv=0;
	CF_EquipCommanderJinglianSuit::DataEntryMapExternIterator kIter2 = CF_EquipCommanderJinglianSuit::GetSingletonPtr()->GetIterator();
	for( ; kIter2.HasMoreElements(); kIter2.MoveNext())
	{
		const CF_EquipCommanderJinglianSuit::DataEntry *pkEntry = kIter2.PeekNextValuePtr();
		if(iMaxJinglianLv >= pkEntry->_iEquipLevel)
		{
			iMaxJinglianSuitLv=pkEntry->_iLevel;
		}
		else
		{
			break;
		}
	}
	CF_EquipCommanderJinglianSuit::DataEntry* JinglianSuitEntry = SafeGetCSVFileDataEntryPtr<CF_EquipCommanderJinglianSuit>(iMaxJinglianSuitLv);
	if (JinglianSuitEntry)
	{
		const Utility::UT_SIMDataList& attr = JinglianSuitEntry->Attribute;
		for(Utility::UT_SIMDataList::const_iterator it=attr.begin(); it!=attr.end(); ++it)
		{
			attrMap2.ModifyAttributeByPart(it->ParamA(), it->ParamB(), it->ParamC(), true);
		}
	}
	attrMap2.RecomputeAll();
	combat_score_type JinglianSuitScore=LC_Helper::CalcAttributeMapCombatScore(&attrMap2, false);
	m_CommanderEquipStarMaxScore=PercentIncreasedCombat+JinglianSuitScore-m_CommanderEquipMaxScore;
}

StringType SC_ClientSkillInterface::GetDebugMaterialListInfo()
{
	return IG_MaterialManager::GetSingletonPtr()->GetDebugMaterialListInfo();
}


int SC_ClientSkillInterface::_getBuddyAttrib(int iBase, int iLevel, int iRate)
{
	return iBase + iLevel*iRate;
}

// 朝锁定目标的dummy点射击，往上抬枪
// 返回偏转角 fPitchAngle 和对应的向量 kShotRay
static float _adjustBulletStraightLineTrajectory(const ShotBulletArg & arg, const NiPoint3 & kShotPos, NiPoint3 & kShotRay)
{
	// 水平轴
	kShotRay.z = 0;
	kShotRay.Unitize();

	// 偏转角
	float fPitchAngle = 0;
	GD_ClientActorModel* pkVisibleData = IM_Manager::GetSingleton().GetActor(arg.iLockedTargetID);
	if (pkVisibleData)
	{
		const CF_SkillExecuteList::DataEntry* pkSkillData = arg.pkSkillExecute->GetDataEntryPtr();
		NiPoint3 kDummyPos = pkVisibleData->GetFxDummyNodePosition((GD_ModelBase::FxDummyNodeType)pkSkillData->_lBeAttackGfxBindDumy);
		NiPoint3 kActualDir  = kDummyPos - kShotPos;
		kActualDir.Unitize();

		// 夹角；这个夹角不是十分准确，因为这两向量不一定在一个垂直平面内
		float dp    = kActualDir.Dot(kShotRay);
		float sign  = UT_MathBase::Sign(kActualDir.z);
		float rad = acosf(dp);
		float fPitchRad = sign * min(rad, 0.5236f);		// 最大偏转角，大约30°
		fPitchAngle = fPitchRad * UT_MathBase::RAD2ANGLE;

		//*
		// 根据夹角重新计算
		float fSin = 0, fCos = 0;
		UT_MathBase::SinCos(fPitchRad, fSin, fCos);
		kShotRay.x = kShotRay.x * fCos;
		kShotRay.y = kShotRay.y * fCos;
		kShotRay.z = fSin;
		/*/
		kShotRay = kActualDir;
		//*/
	}
	return fPitchAngle;
}

static bool _testBulletStraightLineSceneCollide(const NiPoint3 & kShotPos, const NiPoint3 & kShotRay, 
																  Utility::UT_PickResult & kResult)
{
	int iWorldID          = SE_SceneManager::GetSingletonPtr()->GetCurrentMapID();
	UT_Vec3Float kRayPos  = UT_EngineMath::ConvertNiPoint3ToVec3Float(kShotPos);
	UT_Vec3Float kRayLine = UT_EngineMath::ConvertNiPoint3ToVec3Float(kShotRay);

	bool  bCollided = UT_Pick_Manager::GetSingleton().PickWorld(iWorldID, kRayPos, kRayLine, kResult);
	return bCollided;
}

static NiPoint3 _make_shot_ray_by_H_distance(const NiPoint3 & kShotDir, int iShotHDistance)
{
	// kShotDir 是单位向量
	if (kShotDir.z == 0)
	{
		return (kShotDir * iShotHDistance);
	}
	else
	{
		NiPoint2 kHDir(kShotDir.x, kShotDir.y);		// 水平分量
		float fPitchAngleCos = kHDir.Length();
		float fShotDistance = iShotHDistance / fPitchAngleCos;
		NiPoint3 kNewDir = kShotDir * fShotDistance;
		return kNewDir;
	}
}

void SC_ClientSkillInterface::_implShotBullet(GameLogic::LC_ActorBase* pkActor, ShotBulletArg & arg)
{
	T_PROFILE("Skill::SC_ClientSkillInterface::_implShotBullet");
	/*
	子弹运动方式:
		BT_SHOOT	直线型
		BT_CHAIN	链接型
		BT_CAST		投射型（垂直方向抛物线）
		BT_MISSILE	导弹型（BSpline型）

	子弹目标类型:
		TARGET_NONE:		朝向型，非跟踪
		TARGET_LOCATION:	地点型，非跟踪
		else:				目标型，跟踪弹
	*/

	int cnt = UT_MathBase::Min(arg.iBulletCount, MAX_SKILL_BULLET_SCATTERING_COUNT);
	if (cnt <= 0)
		return;

	// 子弹发射位置
	UT_Vec3Int kShotPosI = arg.kShotPos;
	NiPoint3   kShotPosF;
	GD_ClientActorModel* pkVisibleData = IM_Manager::GetSingleton().GetActor(arg.iRequesterID);
	if (pkVisibleData && arg.iLocationIndex == TARGET_ME)
	{
		kShotPosF = pkVisibleData->GetFxDummyNodePosition((GD_ModelBase::FxDummyNodeType)arg.iDummyType);	// 取枪口位置
		kShotPosI = UT_EngineMath::ConvertNiPoint3ToVec3Int(kShotPosF);
		arg.kShotPos = kShotPosI;
	}
	else
	{
		kShotPosF = UT_EngineMath::ConvertVec3IntToNiPoint3(kShotPosI);
	}

	//根据策划需求，如果子弹表填写了目标转换，则在有目标的时候将子弹类型转为TARGET_LOCKED
	if (arg.pkBullet->_bChangeTargetIndex && RE_SKILL_CHECK_VALID_TARGET == LC_Helper::IsValidTarget(pkActor, arg.iLockedTargetID, arg.lSkillTypeID) )
	{
		arg.iTargetIndex = TARGET_LOCKED;
	}

	//////////////////////////////////////////////////////////////////////////
	//发送网络请求
	MG_REQ_Bullet_Shot reqMsg;
	reqMsg.m_iTargetIdx     = arg.iTargetIndex;
	reqMsg.m_lRequesterID	= arg.iRequesterID;
	reqMsg.m_lBulletBeginID = GD_ClientSelfInitiatedBulletID::AllocIDs(cnt);
	reqMsg.m_lSkillTypeID	= arg.lSkillTypeID;
	reqMsg.m_iBulletFuncIdx	= (int8_t)arg.iBulletFuncIdx;
	reqMsg.m_iPenetrateCount= (int8_t)arg.iPenetrateCount;
	reqMsg.m_iDummyType     = (int16_t)arg.iDummyType;

	reqMsg.m_lShotLocationX   = kShotPosI.x;
	reqMsg.m_lShotLocationY   = kShotPosI.y;
	reqMsg.m_lShotLocationZ   = kShotPosI.z;
	reqMsg.m_fShotZRotAngle   = arg.fZRotAngle;
	reqMsg.m_fShotXRotAngle   = arg.fPitchAngle;
	reqMsg.m_iScatteringCount = (uint16_t)cnt;
	reqMsg.m_iScatteringAngle = (uint16_t)arg.iScatteringAngle;	// 朝向型才支持多发散射

	// 角色朝向，单束子弹时的射击方向，默认为水平
	UT_Vec2Float kCurrentForward = UT_MathBase::GetZRotDir(arg.fZRotAngle);
	NiPoint3	 kBaseShotDirH(kCurrentForward.x, kCurrentForward.y, 0);	kBaseShotDirH.Unitize();
	NiPoint3	 kBaseShotDir = kBaseShotDirH;

	// 开始发射
	int iMoveType			= arg.pkBullet->_lMoveType;
	int lMinMoveDistance	= arg.pkBullet->_lMinMoveDistance;
	int lMaxMoveDistance	= arg.pkBullet->_lMaxMoveDistance;
	bool bEnableSceneCollide= arg.pkBullet->_bEnableCollide;
	bool bEnableAdjustPitch = arg.pkBullet->_bEnableAdjustTrajectory;
	// bStraightLine     :  直线型子弹
	// bCanScattering    :	BT_SHOOT/BT_CAST的情况下，TARGET_NONE 才支持多发散射
	// bAdjustTrajectory :  直线朝向型子弹，才对准目标抬高弹道
	// bClampHDistance   :  投掷型需要限定水平投掷距离
	// bTestCollide      :	直线型子弹才预先做场景碰撞检测
	bool bStraightLine		= (iMoveType == BT_SHOOT);
	//bool bCanScattering		= (arg.iTargetIndex == TARGET_NONE) && (iMoveType == BT_SHOOT || iMoveType == BT_CAST);
	bool bAdjustTrajectory	= (arg.iTargetIndex == TARGET_NONE) && (iMoveType == BT_SHOOT || iMoveType == BT_MISSILE) && bEnableAdjustPitch;
	bool bClampHDistance	= iMoveType == BT_CAST;
	bool bTestCollide		= bStraightLine && bEnableSceneCollide;

	

	switch (iMoveType)
	{
	case BT_SHOOT:
	case BT_MISSILE:
	case BT_CAST:
	case BT_HOMINGMISSLE:
	case BT_TERMINAL_HOVER:
		switch (arg.iTargetIndex)
		{
		case TARGET_NONE:	//朝向型
			{
				// 计算pitch angle
				if ( bAdjustTrajectory
				  && arg.iLockedTargetID != 0		// 朝向型直线射击，但是有目标
				  && RE_SKILL_CHECK_VALID_TARGET == LC_Helper::IsValidTarget(pkActor, arg.iLockedTargetID, arg.lSkillTypeID)
					)
				{
					float fPitchAngle = _adjustBulletStraightLineTrajectory(arg, kShotPosF, kBaseShotDir);
					reqMsg.m_fShotXRotAngle = fPitchAngle;
					arg.fPitchAngle		    = fPitchAngle;
				}

				//////////////////////////////////////////////////////////////////////////
				// delta scattering angle
				float fScatteringDeltaAngle = Int2Float(reqMsg.m_iScatteringAngle) / Int2Float(cnt - 1);
				fScatteringDeltaAngle *= UT_MathBase::ANGLE2RAD;

				// 生成客户端子弹，并碰撞计算
				for (int i = 0; i < cnt; ++i)
				{
					// 子弹方向
					NiPoint3 kShotAt = kBaseShotDir;	// pitch已经算在kBaseShotDir中，不用再旋转
					if (cnt > 1)
					{
						NiMatrix3 mat;
						float angle = (i - (cnt-1)/2.0f) * fScatteringDeltaAngle;
						mat.MakeZRotation(angle);
						kShotAt = kBaseShotDir * mat;
					}
					// 子弹射线
					NiPoint3 kShotRay = _make_shot_ray_by_H_distance(kShotAt, arg.iMoveDistance);
					
					if (bClampHDistance)
					{
						UT_Vec3Int kTargetPos = arg.kShotPos + UT_EngineMath::ConvertNiPoint3ToVec3Int(kShotRay);
						kTargetPos = UT_MathBase::MoveAlongIntClampXY(arg.kShotPos, kTargetPos, (float)lMinMoveDistance, (float)lMaxMoveDistance);
						kShotRay   = UT_EngineMath::ConvertVec3IntToNiPoint3(kTargetPos - arg.kShotPos);
						arg.iMoveDistance = UT_MathBase::LineLengthXYInt(arg.kShotPos, kTargetPos);
					}

					// 子弹与场景物件碰撞点
					bool bCollided = false;
					Utility::UT_PickResult kPickResult(0);
					if (bTestCollide)
						bCollided = _testBulletStraightLineSceneCollide(kShotPosF, kShotRay, kPickResult);
					float fCollideT = kPickResult.clampped_t();
					reqMsg.m_fCollidePosParamT.push_back(fCollideT);
					reqMsg.m_iCollideSurfaceMaterial.push_back(kPickResult.m_enmSurfaceMaterial);

					// 根据碰撞点，调整飞行距离
					if (bCollided)
					{
						kShotRay *= fCollideT;
					}

					// 创建客户端本地子弹对象
					this->_createLocalBullet(pkActor, arg, reqMsg.m_lBulletBeginID + i,
						UT_EngineMath::ConvertNiPoint3ToVec3Int(kShotRay), 
						kPickResult);
				}

				// 发消息通知服务器
				MG_PACKET_ENCODE_HANDLER hMsgHandler;
				bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_BULLET_SHOT, &reqMsg);
				if (bCreateMsg)
					arg.pkGamePlayer->GetSession()->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
			}
			break;

		case TARGET_LOCATION:	//地点型
			{
				UT_Vec3Int kTargetPos = pkActor->GetLockedLocation();
				if (bClampHDistance)
					kTargetPos = UT_MathBase::MoveAlongIntClampXY(arg.kShotPos, kTargetPos, (float)lMinMoveDistance, (float)lMaxMoveDistance);
				arg.iMoveDistance = UT_MathBase::LineLengthXYInt(arg.kShotPos, kTargetPos);

				NiPoint3 kShotRay = UT_EngineMath::ConvertVec3IntToNiPoint3(kTargetPos - arg.kShotPos);

				// 子弹与场景物件碰撞点
				bool bCollided = false;
				Utility::UT_PickResult kPickResult(0);
				if (bTestCollide)
					bCollided = _testBulletStraightLineSceneCollide(kShotPosF, kShotRay, kPickResult);
				float fCollideT = kPickResult.clampped_t();
				reqMsg.m_fCollidePosParamT.push_back(fCollideT);
				reqMsg.m_iCollideSurfaceMaterial.push_back(kPickResult.m_enmSurfaceMaterial);
				reqMsg.m_iScatteringCount = 1;
				reqMsg.m_iScatteringAngle = 0;

				// 根据碰撞点，调整飞行距离
				if (bCollided)
				{
					kShotRay *= fCollideT;
				}

				// 创建客户端本地子弹对象
				this->_createLocalBullet(pkActor, arg, reqMsg.m_lBulletBeginID,
					UT_EngineMath::ConvertNiPoint3ToVec3Int(kShotRay),
					kPickResult);

				UT_Vec3Int tgtLocation = arg.kShotPos + UT_EngineMath::ConvertNiPoint3ToVec3Int(kShotRay);
				reqMsg.m_lDestLocationX = tgtLocation.x;
				reqMsg.m_lDestLocationY = tgtLocation.y;
				reqMsg.m_lDestLocationZ = tgtLocation.z;

				// 发消息通知服务器
				MG_PACKET_ENCODE_HANDLER hMsgHandler;
				bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_BULLET_SHOT, &reqMsg);
				if (bCreateMsg)
					arg.pkGamePlayer->GetSession()->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
			}
			break;

		default:				//当做目标型处理
			{
				LC_ActorPtrList kTargetList;
				_findTarget(pkActor, arg.iTargetIndex, kTargetList);
				if (kTargetList.size() <= 0)
				{
					return;
				}

				// 目标型必须服务器发出，朝向型和地点型由客户端先发出
				// 暂未测试
				reqMsg.m_lBulletBeginID = 0;
				reqMsg.m_fCollidePosParamT.push_back(1.0f);
				reqMsg.m_iCollideSurfaceMaterial.push_back(0);
				reqMsg.m_iScatteringCount = 1;
				reqMsg.m_iScatteringAngle = 0;

				// 发消息通知服务器
				MG_PACKET_ENCODE_HANDLER hMsgHandler;
				bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_BULLET_SHOT, &reqMsg);
				if (bCreateMsg)
					arg.pkGamePlayer->GetSession()->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
			}
		}
		break;

	case BT_CHAIN:
		{
			LC_ActorPtrList kTargetList;
			_findTarget(pkActor, arg.iTargetIndex, kTargetList);
			if (kTargetList.size() <= 0)
			{
				return;
			}

			// 目标型必须服务器发出，朝向型和地点型由客户端先发出
			// 暂未测试
			reqMsg.m_lBulletBeginID = 0;
			reqMsg.m_fCollidePosParamT.push_back(1.0f);
			reqMsg.m_iCollideSurfaceMaterial.push_back(0);
			reqMsg.m_iScatteringCount = 1;
			reqMsg.m_iScatteringAngle = 0;

			// 发消息通知服务器
			MG_PACKET_ENCODE_HANDLER hMsgHandler;
			bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_BULLET_SHOT, &reqMsg);
			if (bCreateMsg)
				arg.pkGamePlayer->GetSession()->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
		}
		break;
	}
}

void SC_ClientSkillInterface::_createLocalBullet(GameLogic::LC_ActorBase* pkActor, const ShotBulletArg & arg, 
												  unsigned int iBulletClientID, const UT_Vec3Int & kShotRay, const Utility::UT_PickResult & kPickResult)
{
	// 准备参数
	UT_Vec3Int kTargetPos = arg.kShotPos + kShotRay;
	int iBulletFuncIdx	= arg.iBulletFuncIdx;
	int iMoveType		= arg.pkBullet->_lMoveType;
	int iMoveSpeed		= arg.pkBullet->_lMoveSpeed;
	float fCollideT		= kPickResult.clampped_t();
	int iSurfMat		= kPickResult.m_enmSurfaceMaterial;
	float fMoveTime		= Int2Float(arg.iMoveDistance) / iMoveSpeed * fCollideT;

	if ((iMoveType == BT_MISSILE || iMoveType == BT_CAST || iMoveType == BT_HOMINGMISSLE) 
		&& fMoveTime < arg.pkBullet->_fMinExistTime)
	{
		fMoveTime = arg.pkBullet->_fMinExistTime;
	}

	//TOADD: 读表的子弹停滞时间(二阶段时间)
	if (iMoveType == BT_HOMINGMISSLE || iMoveType == BT_TERMINAL_HOVER)
	{
		fMoveTime += arg.pkBullet->_fBulletHoverTime;
	}

	int iTargetID		= 0;
	if (arg.iTargetIndex != TARGET_NONE && arg.iTargetIndex != TARGET_LOCATION)
	{
		LC_ActorPtrList kTargetsList;
		_findTarget(pkActor,arg.iTargetIndex, kTargetsList);
		if (!kTargetsList.empty())
		{
			LC_ActorBase* pkTarget = kTargetsList.front();
			iTargetID = pkTarget->GetID();
		}
	}

	// 准备事件
	SK_SkillEvent kEvent;
	kEvent.SetEventID(0);
	kEvent.SetEventType(SETE_BULLET);
	kEvent.SetSrcActorID(arg.iRequesterID);
	kEvent.SetDstActorID(iTargetID);
	kEvent.SetLogicID(arg.lSkillTypeID);
	kEvent.SetStartTime(arg.fStartTime);
	kEvent.SetParam(SK_BulletEvent::SEP_MOVE_TYPE, iMoveType);
	kEvent.SetParam(SK_BulletEvent::SEP_ACTION_TYPE, SK_BulletEvent::SBAT_SHOT | SK_BulletEvent::SBSS_FromClient);
	kEvent.SetParam(SK_BulletEvent::SEP_DUMMY_TYPE, arg.iDummyType);
	kEvent.SetParam(SK_BulletEvent::SEP_MOVE_START_TIME, SK_SkillEvent::ParamFloat2LLong(arg.fStartTime));
	kEvent.SetParam(SK_BulletEvent::SEP_MOVE_LIFE_TIME, SK_SkillEvent::ParamFloat2LLong(fMoveTime));
	kEvent.SetParam(SK_BulletEvent::SEP_PITCH_ANGLE, SK_SkillEvent::ParamFloat2LLong(arg.fPitchAngle));
	kEvent.SetParam(SK_BulletEvent::SEP_MOVE_SPEED, iMoveSpeed);
	kEvent.SetParam(SK_BulletEvent::SEP_BULLET_FUNC, iBulletFuncIdx);
	kEvent.SetParam(SK_BulletEvent::SEP_MAX_PENETRATE_COUNT, arg.iPenetrateCount);
	kEvent.SetParam(SK_BulletEvent::SEP_TARGET_LOCATION_X, kTargetPos.x);
	kEvent.SetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Y, kTargetPos.y);
	kEvent.SetParam(SK_BulletEvent::SEP_TARGET_LOCATION_Z, kTargetPos.z);
	kEvent.SetParam(SK_BulletEvent::SEP_SHOT_LOCATION_X, arg.kShotPos.x);
	kEvent.SetParam(SK_BulletEvent::SEP_SHOT_LOCATION_Y, arg.kShotPos.y);
	kEvent.SetParam(SK_BulletEvent::SEP_SHOT_LOCATION_Z, arg.kShotPos.z);
	kEvent.SetParam(SK_BulletEvent::SEP_COLLIDE_PARAM, Skill::SerializeToQuatWord(SK_BulletEvent::CollideParam(fCollideT, iSurfMat)));
	kEvent.SetParam(SK_BulletEvent::SEP_BULLET_LOCAL_ID, iBulletClientID);
	kEvent.SetParam(SK_BulletEvent::SEP_LAST_UPDATE_TIME, SK_SkillEvent::ParamFloat2LLong(arg.fStartTime));

	// 创建客户端子弹
	SK_ClientSkillEventManager* pkSkillEventManager = CLIENT_GET_SKILL_EVENT_MANAGER();
	pkSkillEventManager->RecvMessageFromClient(&kEvent);
}

void SC_ClientSkillInterface::TransferToTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, bool bUseSafeRadius)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	LC_ActorBase* pkPlayer = (LC_ActorBase*)pkScriptCaller;
	
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();

	const UT_Vec3Int& curLocation = pkPlayer->GetCurrentLocation();

	UT_Vec3Int kTargetLocation;
	if (TARGET_LOCATION == iTargetIndex)
	{
		kTargetLocation = pkPlayer->GetLockedLocation();
	}
	else
	{
		LC_ActorPtrList kTargetList;
		_findTarget(pkScriptCaller, iTargetIndex, kTargetList);
		if (kTargetList.size() != 1)
		{
			return;
		}

		LC_LogicObject* pkTarget = kTargetList.front();
		if (NULL == pkTarget)
		{
			return;
		}
		kTargetLocation = pkTarget->GetCurrentLocation();

		if (bUseSafeRadius)
		{
			int iSafeRadius	 = pkPlayer->GetProperty_ShapeRadius();
			iSafeRadius	+= ((LC_ActorBase*)pkTarget)->GetProperty_ShapeRadius();
			kTargetLocation = UT_MathBase::ComputeSafeRadiusTargetLocation(curLocation,kTargetLocation,iSafeRadius);
		}
	}

	if (pkScriptCaller == pkGamePlayer)
	{
		int iDistance = UT_MathBase::LineLengthXYZInt(kTargetLocation, curLocation);
		const UT_Vec2Float& kCurrentForward = pkPlayer->GetTargetForwardDir();
		NiPoint3 kDirH(kCurrentForward.x, kCurrentForward.y, 0);
		kDirH.Unitize();
		NiPoint3 kShotRay = _make_shot_ray_by_H_distance(kDirH, iDistance);

		Utility::UT_PickResult kPickResult(0);
		NiPoint3 kStartPoint = UT_EngineMath::ConvertVec3IntToNiPoint3(curLocation);
		bool bCollided = _testBulletStraightLineSceneCollide(kStartPoint, kShotRay, kPickResult);
		if(bCollided)
		{
			float fCollideT = kPickResult.clampped_t();
			kShotRay *= fCollideT;
			kTargetLocation = UT_EngineMath::ConvertNiPoint3ToVec3Int(kStartPoint + kShotRay);
		}

		Path::PF_PathManager* pkPathMgr = Path::PF_PathManager::GetSingletonPtr();
		kTargetLocation.x = UT_MathBase::ClampFloat( Int2Float(kTargetLocation.x) , 0.0f , GfxGetWorldConfig().CFG_WORLD_MAP_WIDTH );
		kTargetLocation.y = UT_MathBase::ClampFloat( Int2Float(kTargetLocation.y) , 0.0f , GfxGetWorldConfig().CFG_WORLD_MAP_WIDTH );
		UT_Vec3Int kNewTarget = kTargetLocation;
		if ( pkPathMgr )
		{
			if ( !pkPathMgr->TestCanMoveTo( kTargetLocation , false ))
			{
				int lSearchStep = 200;
				int lHorizontalRange;
				int lVerticalRange;
				int lMaxDistance;

				lHorizontalRange = 2000;
				lVerticalRange   = 500;
				lMaxDistance     = 500;

				kNewTarget = pkPathMgr->FindClosestMovablePoint( kTargetLocation , 
					lSearchStep , 100 , lHorizontalRange , lVerticalRange , lMaxDistance );
				kTargetLocation = (kNewTarget.x!=0 && kNewTarget.y!=0)? kNewTarget: pkPlayer->GetCurrentLocation();
			}

			UT_Vec3Int NcurLocation = curLocation;
			NcurLocation.z += 150; 
			UT_Vec3Int NTarLocation = kTargetLocation;
			NTarLocation.z += 150; 
			if(pkPathMgr->TestLineCollision( NcurLocation, NTarLocation))
			{
				kNewTarget = pkPathMgr->FindGetFurthestMovablePoint( curLocation, kTargetLocation);
				kTargetLocation = (kNewTarget.x!=0 && kNewTarget.y!=0)? kNewTarget: curLocation;
			}
		}
	}
 
	pkPlayer->SetCurrentLocation(kTargetLocation);
	pkPlayer->SetVisibleLocation(kTargetLocation);
	pkPlayer->SetTargetLocation(kTargetLocation);
}

void SC_ClientSkillInterface::TransferBackToTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, bool bUseSafeRadius)
{
	if (NULL == pkScriptCaller)
	{
		return;
	}

	LC_ActorBase* pkPlayer = (LC_ActorBase*)pkScriptCaller;

	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();

	const UT_Vec3Int& curLocation = pkPlayer->GetCurrentLocation();

	UT_Vec3Int kTargetLocation;
	if (TARGET_LOCATION == iTargetIndex)
	{
		kTargetLocation = pkPlayer->GetLockedLocation();
	}
	else
	{
		LC_ActorPtrList kTargetList;
		_findTarget(pkScriptCaller, iTargetIndex, kTargetList);
		if (kTargetList.size() != 1)
		{
			return;
		}

		LC_LogicObject* pkTarget = kTargetList.front();
		if (NULL == pkTarget)
		{
			return;
		}
		kTargetLocation = pkTarget->GetCurrentLocation();

		if (bUseSafeRadius)
		{
			int iSafeRadius	 = pkPlayer->GetProperty_ShapeRadius();
			iSafeRadius	+= ((LC_ActorBase*)pkTarget)->GetProperty_ShapeRadius();
			kTargetLocation = UT_MathBase::ComputeSafeRadiusTargetLocation(curLocation,kTargetLocation,iSafeRadius);
		}
	}

	if (pkScriptCaller == pkGamePlayer)
	{
		Path::PF_PathManager* pkPathMgr = Path::PF_PathManager::GetSingletonPtr();
		kTargetLocation.x = UT_MathBase::ClampFloat( Int2Float(kTargetLocation.x) , 0.0f , GfxGetWorldConfig().CFG_WORLD_MAP_WIDTH );
		kTargetLocation.y = UT_MathBase::ClampFloat( Int2Float(kTargetLocation.y) , 0.0f , GfxGetWorldConfig().CFG_WORLD_MAP_WIDTH );
		UT_Vec3Int kNewTarget = kTargetLocation;
		if ( pkPathMgr && !pkPathMgr->TestCanMoveTo( kTargetLocation , false ))
		{
			int lSearchStep = 200;
			int lHorizontalRange;
			int lVerticalRange;
			int lMaxDistance;

			lHorizontalRange = 2000;
			lVerticalRange   = 500;
			lMaxDistance     = 500;

			kNewTarget = pkPathMgr->FindClosestMovablePoint( kTargetLocation , 
				lSearchStep , 100 , lHorizontalRange , lVerticalRange , lMaxDistance );
			kTargetLocation = (kNewTarget.x!=0 && kNewTarget.y!=0)? kNewTarget: pkPlayer->GetCurrentLocation();
		}
	}

	pkPlayer->SetCurrentLocation(kTargetLocation);
	pkPlayer->SetVisibleLocation(kTargetLocation);
	pkPlayer->SetTargetLocation(kTargetLocation);
}

bool SC_ClientSkillInterface::IsNpc(object_id_type iObjId)
{
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)LC_ClientLogicManager::GetSingletonPtr();
	LC_ClientGamePlayer* pkPlayer = pkLogicManger->GetOperationPlayer();
	if (NULL == pkPlayer)
		return false;
	
	LC_ActorBase* pkNpc = GetNPCPtr(iObjId);
	if(NULL == pkNpc)
		return false;

	return  RE_SKILL_CHECK_INVALID_TARGET == LC_Helper::IsValidTarget(pkPlayer, pkNpc);
}

StringType SC_ClientSkillInterface::_getAttribString(LC_ActorAttributeMap *attrMap)
{
	StringStreamType sStream;
	sStream << LC_Helper::CalcAttributeMapCombatScore(attrMap, false) << "|";
	for(int i=0; i<ATT_MAX_COUNT; ++i)
	{
		int iValue = attrMap->GetAttribute(i);
		if(iValue  > 0)
		{
			sStream << Int2String(i) << '#' << Int2String(iValue) << '|';
		}
	}
	return sStream.str();
}

StringType SC_ClientSkillInterface::_getPercentAttribString(LC_ActorAttributeMap *attrMap)
{
	StringStreamType sStream;
	sStream << LC_Helper::GetAttributeMapCombatScore(attrMap) << "|";
	for(int i=0; i<ATT_MAX_COUNT; ++i)
	{
		int iValue = attrMap->GetAttributePercent(i);
		if(iValue  > 0)
		{
			sStream << Int2String(i) << '#' << Int2String(iValue) << '|';
		}
	}
	return sStream.str();
}

void SC_ClientSkillInterface::_getSkillBookAttribs(LC_ActorAttributeMap* attrMap, int iSkillBookID, int iPos, int iLevel)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if(pkLogicManager == NULL)
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if(pkGamePlayer == NULL)
		return;

	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	if(NULL == pkItemFactory)
		return;

	LC_SkillBookDataEntry* entry = SafeGetSkillBookDataEntry(iSkillBookID, iPos);
	if (NULL == entry)
	{
		return;
	}
	
	for(size_t i=0; i<entry->_PropTimes.size(); ++i)
	{
		if(iLevel >= (i+1))
		{
			for(int j=0; j<entry->_PropTimes[i]; ++j)
				LC_Helper::AttributeMapByAttrFunc(attrMap, entry->_kModifyPropertyID);
		}
	}

	attrMap->RecomputeAll();
}

int SC_ClientSkillInterface::GetMapIDByStoryID(int storyID)
{
	CF_WorldMapList::DataEntryMapExternIterator kIter = CF_WorldMapList::GetSingleton().GetIterator();
	for (;kIter.HasMoreElements();kIter.MoveNext())
	{
		CF_WorldMapList::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		if (pkEntry->_iStoryID == storyID)
			return pkEntry->_iID;
	}
	return -1;
}

StringType SC_ClientSkillInterface::GetTestDataBaseByIndex(int index)
{
	StringStreamType sStream("");
	CF_TestDatabase::DataEntryMapExternIterator kIter = CF_TestDatabase::GetSingleton().GetIterator();
	for (;kIter.HasMoreElements();kIter.MoveNext())
	{
		CF_TestDatabase::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		if(pkEntry->_id == index)
		{
			sStream << pkEntry->_topic << "#" << pkEntry->_answer1 << "#" << pkEntry->_answer2 << "#"
					<< pkEntry->_answer3 << "#" << pkEntry->_answer4 << "#" << pkEntry->_rightAnswer << "#";
			break;
		}
	}
	return sStream.str();
}

int SC_ClientSkillInterface::GetDELTodayEndTimeStamp()
{
	LC_ClientGamePlayer* pkGamePlayer = GetPlayerPtr();
	int t = pkGamePlayer->GetServerTimeVal();
	return GameLogic::LC_Helper::GetDayEndTimestamp(t, 1) - t;
}

bool SC_ClientSkillInterface::IsTimestampEndToday(int64_t iTimeStamp)
{
	LC_ClientGamePlayer* pkGamePlayer = GetPlayerPtr();
	int t = pkGamePlayer->GetServerTimeVal();
	return GameLogic::LC_Helper::GetDayEndTimestamp(t, 1) 
				== LC_Helper::GetDayEndTimestamp(iTimeStamp, 1);
}

int SC_ClientSkillInterface::GetDeltaDay(uint32_t timeA, uint32_t timeB)
{
	return LC_Helper::GetDeltaDay(timeA, timeB);
}

int SC_ClientSkillInterface::GetSevendaysDetailCidByIndex(int index)
{
	CF_SevenDaysLoginReward::DataEntryMapExternIterator kIter = CF_SevenDaysLoginReward::GetSingleton().GetIterator();
	int t = 0;
	for (;kIter.HasMoreElements();kIter.MoveNext())
	{
		CF_SevenDaysLoginReward::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		if(pkEntry->_CID > 0 && pkEntry->_bShow)
		{
			++t;
			if (t == index)
				return pkEntry->_CID;
		}
	}
	return 0;
}

int SC_ClientSkillInterface::GetMapLeaveCountDown(int InMapId)
{
	CF_TranscriptionRule::DataEntry* pkEntry=SafeGetCSVFileDataEntryPtr<CF_TranscriptionRule>(InMapId);
	if (pkEntry)
	{
		return pkEntry->_CountDownTime;
	}
	return 0;
}

StringType SC_ClientSkillInterface::GetTreasureAddAward(int iId,int iLevel)
{
	CF_ACTTreasureHuntStageReward::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_ACTTreasureHuntStageReward>(iId);
	if(pkData == NULL)
		return "";

	T_ASSERT_CRITICAL_S(pkData->_Level.size() == pkData->_Award.size());
	
	StringStreamType sStream;
	for(size_t i=0; i<pkData->_Level.size(); ++i)
	{
		int iLv = pkData->_Level[i];
		if(iLevel >= iLv)
		{
			const UT_SIMDataInfo& data = pkData->_Award[i];
			sStream << data.ID() << ':' << data.ParamA() << ';';
		}
	}

	return sStream.str();
}

int SC_ClientSkillInterface::GetEntrustTaskStarLevel(int iTaskId)
{
	CF_EntrustTask::DataEntry* pkEntry=SafeGetCSVFileDataEntryPtr<CF_EntrustTask>(iTaskId);
	if (pkEntry)
	{
		return pkEntry->_iStar;
	}
	Utility::GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM,"Wrong EntrustTaskId: %d",iTaskId);
	return 0;
}

StringType SC_ClientSkillInterface::GetReinforceSuitInfo()
{
	StringStreamType sStream;
	LC_ActorAttributeMap attrMap;

	CF_EquipCommanderSlotReinforceSuit::DataEntryMapExternIterator iter = CF_EquipCommanderSlotReinforceSuit::GetSingletonPtr()->GetIterator();
	for(; iter.HasMoreElements(); iter.MoveNext())
	{
		const CF_EquipCommanderSlotReinforceSuit::DataEntry *pkData = iter.PeekNextValuePtr();

		attrMap.Clear();
		const Utility::UT_SIMDataList& attr = pkData->AttributeRate;
		for(Utility::UT_SIMDataList::const_iterator it=attr.begin(); it!=attr.end(); ++it)
		{
			attrMap.ModifyAttributeByPart(it->ParamA(), it->ParamB(), it->ParamC(), true);
		}
		attrMap.RecomputeAll();

		sStream << pkData->_iEquipLevel << '@' << pkData->_Descript << '@' << _getAttribString(&attrMap) << '_';
	}

	return sStream.str();
}

StringType SC_ClientSkillInterface::GetRefineSuitInfo()
{
	StringStreamType sStream;
	LC_ActorAttributeMap attrMap;

	CF_EquipCommanderJinglianSuit::DataEntryMapExternIterator iter = CF_EquipCommanderJinglianSuit::GetSingletonPtr()->GetIterator();
	for(; iter.HasMoreElements(); iter.MoveNext())
	{
		const CF_EquipCommanderJinglianSuit::DataEntry *pkData = iter.PeekNextValuePtr();
		attrMap.Clear();

		bool bPercent = false;
		const Utility::UT_SIMDataList& attr = pkData->Attribute;
		for(Utility::UT_SIMDataList::const_iterator it=attr.begin(); it!=attr.end(); ++it)
		{
			int iPartIdx = it->ParamA();
			attrMap.ModifyAttributeByPart(iPartIdx, it->ParamB(), it->ParamC(), true);
			if(iPartIdx == 2 || iPartIdx == 4)
				bPercent = true;
		}
		attrMap.RecomputeAll();

		sStream << pkData->_iEquipLevel << '@';
		if(bPercent)
			sStream << _getPercentAttribString(&attrMap) << '_';
		else
			sStream << _getAttribString(&attrMap) << '_';
	}

	return sStream.str();
}

StringType SC_ClientSkillInterface::GetItemNonpareilEffect(int iItemID)
{
	CF_ItemList::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_ItemList>(iItemID);
	if(pkData == NULL || pkData->_sNonpareilEffect.empty())
		return "";

	LC_ActorAttributeMap attrMap;
	LC_Helper::GetNonpareilAttrMap(&attrMap, pkData->_sNonpareilEffect);
	return _getPercentAttribString(&attrMap);
}

StringType SC_ClientSkillInterface::GetItemNonpareilEffectBase(int iItemID)
{
	CF_ItemList::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_ItemList>(iItemID);
	if(pkData == NULL || pkData->_sNonpareilEffectEquipBase.empty())
		return "";

	LC_ActorAttributeMap attrMap;
	LC_Helper::GetNonpareilAttrMap(&attrMap, pkData->_sNonpareilEffectEquipBase);
	return _getPercentAttribString(&attrMap);
}

StringType SC_ClientSkillInterface::GetGuildBattleBossRankReward(int chartypeID)
{
	StringStreamType sStream;
	CF_CharType* pkCSVCharType = CF_CharType::GetSingletonPtr();
	const CF_CharType::DataEntry* pkData = pkCSVCharType->GetEntryPtr(chartypeID);
	if(pkData == NULL || pkData->_iType <= 0)
		return "";
	int itype = pkData->_iType;

	int icount = 0;
	CF_BestGuildBossRankRewards::DataEntryMapExternIterator iter = CF_BestGuildBossRankRewards::GetSingletonPtr()->GetIterator();
	for(; iter.HasMoreElements(); iter.MoveNext())
	{
		const CF_BestGuildBossRankRewards::DataEntry *pkEntry = iter.PeekNextValuePtr();
		if (pkEntry->_iType == itype)
		{
			sStream << pkEntry->_iRankStartIndex << '#';
			StringType sStr;
			FormatDataIFs2String(pkEntry->_kReward, sStr);
			sStream << sStr << "|";
			icount++;
			if (icount >= 4)
				break;
		}
	}
	return sStream.str();
}

StringType SC_ClientSkillInterface::GetGuildBattleBossBaseReward(int chartypeID)
{
	CF_BestGuildBossInfo::DataEntryMapExternIterator iter = CF_BestGuildBossInfo::GetSingletonPtr()->GetIterator();
	for(; iter.HasMoreElements(); iter.MoveNext())
	{
		const CF_BestGuildBossInfo::DataEntry *pkEntry = iter.PeekNextValuePtr();
		if (pkEntry->_lCharType == chartypeID)
			return pkEntry->_kRewardDesc;
	}
	return "";
}

int SC_ClientSkillInterface::GetGuildBattleTowerBuff(int chartypeID)
{
	CF_BestGuildBossInfo::DataEntryMapExternIterator iter = CF_BestGuildBossInfo::GetSingletonPtr()->GetIterator();
	for(; iter.HasMoreElements(); iter.MoveNext())
	{
		const CF_BestGuildBossInfo::DataEntry *pkEntry = iter.PeekNextValuePtr();
		if (pkEntry->_lCharType == chartypeID)
			return pkEntry->_iBuff;
	}
	return 0;
}

int SC_ClientSkillInterface::GetGuildBattleTowerIntegralAdd(int chartypeID)
{
	CF_BestGuildBossInfo::DataEntryMapExternIterator iter = CF_BestGuildBossInfo::GetSingletonPtr()->GetIterator();
	for(; iter.HasMoreElements(); iter.MoveNext())
	{
		const CF_BestGuildBossInfo::DataEntry *pkEntry = iter.PeekNextValuePtr();
		if (pkEntry->_lCharType == chartypeID)
		{
			TStringVector kVector = UT_BaseStringUtility::SplitString(pkEntry->_iScore, ':');
			if (kVector.size() > 0)
			{
				TStringVector::iterator it = kVector.begin();
				return String2Int(it->c_str());
			}
			else
				break;
		}
	}
	return 0;
}

int SC_ClientSkillInterface::GetGuildBattleTowerVipBonusPercent(int chartypeID)
{
	CF_BestGuildBossInfo::DataEntry *pkData=SafeGetCSVFileDataEntryPtr<CF_BestGuildBossInfo>(chartypeID);
	if (pkData)
	{
		return pkData->_iVipRate;
	}
	return 0;
}

StringType SC_ClientSkillInterface::GetGuildBattleBossInfoByType(int iType)
{
	StringStreamType sStream;
	CF_BestGuildBossInfo::DataEntryMapExternIterator iter = CF_BestGuildBossInfo::GetSingletonPtr()->GetIterator();
	for(; iter.HasMoreElements(); iter.MoveNext())
	{
		const CF_BestGuildBossInfo::DataEntry *pkEntry = iter.PeekNextValuePtr();
		if (pkEntry->_lType==iType)
		{
			sStream<<pkEntry->_lCharType<<"#"<<pkEntry->_kPos.x<<"#"<<pkEntry->_kPos.y<<"#"<<pkEntry->_kLocation.x<<"#"<<pkEntry->_kLocation.y<<"#"<<pkEntry->_lType1<<"|";
		}
	}
	return sStream.str();
}

StringType SC_ClientSkillInterface::GetGuildBattleBossInfoByCharType(int iCharType)
{
	StringStreamType sStream;
	CF_BestGuildBossInfo::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_BestGuildBossInfo>(iCharType);
	if (pkEntry)
	{
		sStream<<pkEntry->_kPos.x<<"#"<<pkEntry->_kPos.y<<"#"<<pkEntry->_kLocation.x<<"#"<<pkEntry->_kLocation.y;
	}
	return sStream.str();
}

StringType SC_ClientSkillInterface::GetGuildBattleRobRewardInfo()
{
	StringStreamType sStream;
	CF_BestGuildRobReward::DataEntryMapExternIterator iter = CF_BestGuildRobReward::GetSingletonPtr()->GetIterator();
	for( ; iter.HasMoreElements(); iter.MoveNext())
	{
		const CF_BestGuildRobReward::DataEntry *pkData = iter.PeekNextValuePtr();
		sStream << pkData->_iWinTimes << '#' << pkData->_kReward << '|';
	}

	return sStream.str();
}

int SC_ClientSkillInterface::GetWeekDayByTime(uint32_t itime)
{
	time_t bt = time_t(itime);
	tm bLT = *localtime(&bt);
	return bLT.tm_wday;
}

int SC_ClientSkillInterface::GetSkillDirection(int lSkillID)
{
	CF_SkillExecuteList::DataEntry* pkEntry = LC_Helper::SafeGetSkillExecuteDataEntryPtr(lSkillID);
	if (!pkEntry)
	{
		return -1;
	}
	return pkEntry->_Direction ? pkEntry->_Direction : 0;
}

StringType SC_ClientSkillInterface::GetBestGuildRankStageInfo(int itype)
{
	StringStreamType sStream;
	LC_ClientGamePlayer* pPlayer = GetPlayerPtr();
	int64_t nStartTime = GET_SERVER_OPEN_TIME;
	CF_RankOpen::DataEntryMapExternIterator iter = CF_RankOpen::GetSingletonPtr()->GetIterator();
	for(; iter.HasMoreElements(); iter.MoveNext())
	{
		const CF_RankOpen::DataEntry *pkEntry = iter.PeekNextValuePtr();
		if (pkEntry->_iType == itype)
		{
			time_t uStartTime = 0;
			time_t uEndTime = 0;
			if (pkEntry->_sStartTime.empty())
			{
				CSVFile::StartTime tempStart = pkEntry->_StartTime1;
				uStartTime = tempStart.GetTimeStramp(nStartTime);
			}
			else
			{
				uStartTime = LC_Helper::DateTimeToTimeStamp(pkEntry->_sStartTime.c_str());
			}

			if (pkEntry->_sEndTime.empty())
			{
				CSVFile::StartTime tempStart = pkEntry->_EndTime1;
				uEndTime = tempStart.GetTimeStramp(nStartTime);
			}
			else
			{
				uEndTime = LC_Helper::DateTimeToTimeStamp(pkEntry->_sEndTime.c_str());
			}
			sStream << pkEntry->_iId << "#" << uStartTime << "#" << uEndTime <<"|";
		}
	}
	return sStream.str();
}

int SC_ClientSkillInterface::GetBestGuildGoalCount(int iID)
{
	CF_BestGuildGoal::DataEntry *pkEntry=SafeGetCSVFileDataEntryPtr<CF_BestGuildGoal>(iID);
	if (!pkEntry)
		return -1;
	return pkEntry->_Count;
}

int32_t SC_ClientSkillInterface::GetSevendaysRewardEndTime(int32_t beginTime, int32_t itime)
{
	return beginTime + itime;
}

StringType SC_ClientSkillInterface::GetToyIsLandBossLink(int mapID)
{
	StringStreamType sStream;
	CF_WorldObjectDesc::DataEntryMapExternIterator kIter = CF_WorldObjectDesc::GetSingletonPtr()->GetIterator();
	int icount = 0;
	for( ; kIter.HasMoreElements(); kIter.MoveNext() )
	{
		const CF_WorldObjectDesc::DataEntry* pkEntry = kIter.PeekNextValuePtr();
		if (pkEntry->_lMapID == mapID)
		{
			sStream << pkEntry->_iID << "|" << pkEntry->_kName << "#";
			icount++;
			if (icount >= 5)
				break;
		}
	}
	return sStream.str();
}

StringType SC_ClientSkillInterface::GetExpAccessWayInfo()
{
	StringStreamType sStream;
	CF_PromoteLevel::DataEntryMapExternIterator kIter = CF_PromoteLevel::GetSingletonPtr()->GetIterator();
	for( ; kIter.HasMoreElements(); kIter.MoveNext() )
	{
		const CF_PromoteLevel::DataEntry* pkEntry = kIter.PeekNextValuePtr();
		sStream << pkEntry->_iID << "|" << pkEntry->_lLinkID << "#";
	}
	return sStream.str();
}

int SC_ClientSkillInterface::GetCurrentProcessId()
{
	return ::GetCurrentProcessId();
}

StringType SC_ClientSkillInterface::GetGloryRewardLinkInfo(int iTransRuleID)
{
	const CF_GloryRewardLink::DataEntry *pkEntry = NULL;
	CF_GloryRewardLink::DataEntryMapExternIterator iter = CF_GloryRewardLink::GetSingletonPtr()->GetIterator();
	for(; iter.HasMoreElements(); iter.MoveNext())
	{
		const CF_GloryRewardLink::DataEntry *pkData = iter.PeekNextValuePtr();
		if(pkData->_iTranscriptionRuleID > iTransRuleID)
		{
			break;
		}

		pkEntry = pkData;
	}

	if(pkEntry == NULL)
		return "";

	StringStreamType sStream;
	const CF_WorldObjectDesc::DataEntry *pkLink = SafeGetCSVFileDataEntryPtr<CF_WorldObjectDesc>(pkEntry->_iLink);
	T_ASSERT_CRITICAL_S(pkLink != NULL);

	sStream << pkEntry->_iLink << '#' << pkLink->_kName << '#' << pkEntry->_iItemID;
	return sStream.str();
}

StringType SC_ClientSkillInterface::GetCommanderEquipSuitCollectInfo()
{
	StringStreamType sStream;
	StringType sRwd;
	sRwd.reserve(64);

	LC_ActorAttributeMap attrMap;
	CF_EquipCommanderSuitCollection::DataEntryMapExternIterator iter = CF_EquipCommanderSuitCollection::GetSingletonPtr()->GetIterator();
	for(; iter.HasMoreElements(); iter.MoveNext())
	{
		const CF_EquipCommanderSuitCollection::DataEntry *pkData = iter.PeekNextValuePtr();
		sStream << pkData->ID << '&' << pkData->Name << '&';
		sStream << pkData->EquipShowID1 << '&' << pkData->EquipShowID2 << '&';
		sStream << pkData->EquipShowID3 << '&' << pkData->EquipShowID4 << '&';
		sStream << pkData->EquipShowID5 << '&' << pkData->EquipShowID6 << '&';
		sStream << pkData->EquipShowID7 << '&' << pkData->EquipShowID8 << '&';

		sRwd.clear();
		FormatDataIFs2String(pkData->Reward, sRwd);
		sStream << sRwd << '&' << pkData->LinkID << '&' << pkData->CombatScore << '&' << pkData->PreID << '&';

		attrMap.Clear();
		const Utility::UT_SIMDataList& attr = pkData->Attribute;
		for(Utility::UT_SIMDataList::const_iterator it=attr.begin(); it!=attr.end(); ++it)
		{
			attrMap.ModifyAttributeByPart(it->ParamA(), it->ParamB(), it->ParamC(), true);
		}
		attrMap.RecomputeAll();
		sStream << _getAttribString(&attrMap) << '@';
	}

	return sStream.str();
}

StringType SC_ClientSkillInterface::GetResourceRetrieveInfo()
{
	StringStreamType sStream;
	StringType sLevels;

	CF_ComeBack::DataEntryMapExternIterator iter = CF_ComeBack::GetSingletonPtr()->GetIterator();
	for( ; iter.HasMoreElements(); iter.MoveNext())
	{
		const CF_ComeBack::DataEntry *pkData = iter.PeekNextValuePtr();
		sStream << pkData->_iID << '#' << pkData->_sName << '#' << pkData->_iExp << '#';
		sStream << pkData->_iGold << '#' << pkData->_iYuanneng << '#' << pkData->_iGuild << '#';
		sStream << pkData->_iCourage << '#' << pkData->_iNormalPercent << '#' << pkData->_iSpecialNormal << '#';

		sLevels.clear();
		FormatDataIFs2String(pkData->_sLevel, sLevels);
		sStream << sLevels << '#' << pkData->_kReward << '#';
		sStream << FormatComMap2String(pkData->_iCost, 1) << '@';
	}
	
	return sStream.str();
}

StringType SC_ClientSkillInterface::GetRatioByLevel(int iLevel)
{
	const CF_EXPList::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_EXPList>(iLevel);
	if(pkData == NULL)
		return "";

	StringStreamType sStream;
	sStream << pkData->_lBaseXP << '#' << pkData->_lBaseCash << '#';
	sStream << pkData->_lBaseGuild << '#' << pkData->_lBaseCourage << '#' << pkData->_lBaseBuddyXP;
	return sStream.str();
}

StringType SC_ClientSkillInterface::GetAchievementTypeInfo(int itype)
{
	StringStreamType sStream;
	CF_AchievementList::DataEntryMapExternIterator iter = CF_AchievementList::GetSingletonPtr()->GetIterator();
	for( ; iter.HasMoreElements(); iter.MoveNext())
	{
		CF_AchievementList::DataEntry* pkEntry = iter.PeekNextValuePtr();
		if (pkEntry->_iAchievementType == itype)
		{
			sStream << pkEntry->_lID << '|' << pkEntry->_iSubclass << '#';
		}
	}
	return sStream.str();
}

StringType SC_ClientSkillInterface::GetBuddyTransformInfoByCid(int iCid)
{
	StringStreamType sStream;
	CF_BuddyTransform::DataEntryMapExternIterator iter = CF_BuddyTransform::GetSingletonPtr()->GetIterator();
	for( ; iter.HasMoreElements(); iter.MoveNext())
	{
		CF_BuddyTransform::DataEntry* pkEntry = iter.PeekNextValuePtr();
		if (pkEntry->_buddy == iCid)
		{
			sStream << pkEntry->_iID << '|' << pkEntry->_Name << '|' << pkEntry->_iGrade << '|';
			const Utility::UT_SIMDataList& attr = pkEntry->_kAttribute;
			for(Utility::UT_SIMDataList::const_iterator it=attr.begin(); it!=attr.end(); ++it)
			{
				sStream << it->ParamA() << ':' << it->ParamB() << ':' << it->ParamC() << ';';
			}
			sStream << '|';

			const Utility::UT_SIMDataList& glbAttr = pkEntry->_kAttributeGlobal;
			for(Utility::UT_SIMDataList::const_iterator itr=glbAttr.begin(); itr!=glbAttr.end(); ++itr)
			{
				sStream << itr->ParamA() << ':' << itr->ParamB() << ':' << itr->ParamC() << ';';
			}
			sStream << '|';

			const Utility::UT_SIMDataList& items = pkEntry->_kItems;
			for(Utility::UT_SIMDataList::const_iterator itor=items.begin(); itor!=items.end(); ++itor)
			{
				sStream << itor->ID() << ':' << itor->ParamA() << ';';
			}
			sStream << '|' << pkEntry->_kPath << '|' << pkEntry->_iTime << '|' << pkEntry->_BigIcon << '#';
		}
	}
	return sStream.str();
}

StringType SC_ClientSkillInterface::GetBuddyTransformName(int iID)
{
	CF_BuddyTransform::DataEntry *pkEntry=SafeGetCSVFileDataEntryPtr<CF_BuddyTransform>(iID);
	if (!pkEntry)
		return "";
	return pkEntry->_Name;
}

int SC_ClientSkillInterface::GetBuddyTransformGrade(int iID)
{
	CF_BuddyTransform::DataEntry *pkEntry=SafeGetCSVFileDataEntryPtr<CF_BuddyTransform>(iID);
	if (!pkEntry)
		return 0;
	return pkEntry->_iGrade;
}

int SC_ClientSkillInterface::GetBuddyTransformBuddyId(int iID)
{
	CF_BuddyTransform::DataEntry *pkEntry=SafeGetCSVFileDataEntryPtr<CF_BuddyTransform>(iID);
	if (!pkEntry)
		return 0;
	return pkEntry->_buddy;
}

StringType SC_ClientSkillInterface::GetAchievementBigType()
{
	StringStreamType sStream;
	CF_AchievementTypeBig::DataEntryMapExternIterator iter = CF_AchievementTypeBig::GetSingletonPtr()->GetIterator();
	for( ; iter.HasMoreElements(); iter.MoveNext())
	{
		CF_AchievementTypeBig::DataEntry* pkEntry = iter.PeekNextValuePtr();
		sStream << pkEntry->_iType1 << '|' << pkEntry->_sType2 << '|' << pkEntry->_sName << '#';
	}
	return sStream.str();
}

StringType SC_ClientSkillInterface::GetAchievementSmallTypeName(int iID)
{
	CF_AchievementTypeSmall::DataEntry *pkEntry=SafeGetCSVFileDataEntryPtr<CF_AchievementTypeSmall>(iID);
	if (!pkEntry)
		return 0;
	return pkEntry->_sName;
}

int SC_ClientSkillInterface::GetGuildProfitMaxExp(int ilevel)
{
	CF_GuildGift::DataEntry *pkEntry=SafeGetCSVFileDataEntryPtr<CF_GuildGift>(ilevel);
	if (!pkEntry)
		return 0;
	return pkEntry->_iExp;
}

int SC_ClientSkillInterface::GetGuildCrystalQuality(int ilevel)
{
	CF_GuildGift::DataEntry *pkEntry=SafeGetCSVFileDataEntryPtr<CF_GuildGift>(ilevel);
	if (!pkEntry)
		return 0;
	return pkEntry->_iCrystalQuality;
}

int SC_ClientSkillInterface::GetGuildCrystalMaxExp(int igrade)
{
	CF_GuildCrystal::DataEntry *pkEntry=SafeGetCSVFileDataEntryPtr<CF_GuildCrystal>(igrade);
	if (!pkEntry)
		return 0;
	return pkEntry->_iExp;
}

int SC_ClientSkillInterface::GetGuildProfitMaxLevel()
{
	return CF_GuildGift::GetSingletonPtr()->GetEntryCount();
}

StringType SC_ClientSkillInterface::GetTreasureItemInfo()
{
	StringStreamType sStream;
	CF_TreasureItem::DataEntryMapExternIterator iter = CF_TreasureItem::GetSingletonPtr()->GetIterator();
	for( ; iter.HasMoreElements(); iter.MoveNext())
	{
		const CF_TreasureItem::DataEntry *pkData = iter.PeekNextValuePtr();
		sStream << pkData->_id << '#' << pkData->_iSkillID << '#';
		sStream << pkData->_sIcon << '#' << pkData->_iPriority << '|';
	}
	
	return sStream.str();
}

StringType SC_ClientSkillInterface::GetBuyRareSubInfo()
{
	StringStreamType sStream;
	CF_SpecialBuy::DataEntryMapExternIterator iter = CF_SpecialBuy::GetSingletonPtr()->GetIterator();
	for( ; iter.HasMoreElements(); iter.MoveNext())
	{
		const CF_SpecialBuy::DataEntry *pkData = iter.PeekNextValuePtr();
		
		CF_ShopList::DataEntry* pkEntry=SafeGetCSVFileDataEntryPtr<CF_ShopList>(pkData->_iShopID);
		
		char l='s';
		int Price=0;
		if(pkEntry)
		{
			int StartTicktPrice	= LC_Helper::GetComMapValue(pkEntry->_ActualPriceList,CT_PENTACLE);
			int DiamondPrice	= LC_Helper::GetComMapValue(pkEntry->_ActualPriceList,CT_UNBIND_YUANBAO);
			if (StartTicktPrice>0)
			{
				Price=StartTicktPrice;
				l='s';
			}else if (DiamondPrice>0)
			{
				Price=DiamondPrice;
				l='d';
			}
		}

		sStream << pkData->_id << '|' << pkData->_iShopID << '|' << pkData->_iItemShow << '|' << pkData->_lCharID1 << '|' << pkData->_Angle1 << '|'
			<< pkData->_lCharID2 << '|' << pkData->_Angle2 << '|' << pkData->_sDesIcon<< l << '|' << Price <<'#';
	}

	return sStream.str();
}

StringType SC_ClientSkillInterface::GetMustBuyInfo()
{

	StringStreamType sStream;
	CF_SubSystemInfo::DataEntryMapExternIterator iter = CF_SubSystemInfo::GetSingletonPtr()->GetIterator();
	for( ; iter.HasMoreElements(); iter.MoveNext())
	{
		const CF_SubSystemInfo::DataEntry *pkData = iter.PeekNextValuePtr();
		//
		CF_ShopList::DataEntry* pkEntry=SafeGetCSVFileDataEntryPtr<CF_ShopList>(pkData->_iShopID);

		char l='s';
		int Price=0;
		if(pkEntry)
		{
			int StartTicktPrice	= LC_Helper::GetComMapValue(pkEntry->_ActualPriceList,CT_PENTACLE);
			int DiamondPrice	= LC_Helper::GetComMapValue(pkEntry->_ActualPriceList,CT_UNBIND_YUANBAO);
			if (StartTicktPrice>0)
			{
				Price=StartTicktPrice;
				l='s';
			}else if (DiamondPrice>0)
			{
				Price=DiamondPrice;
				l='d';
			}
		}

		sStream << pkData->_subType << '|' << pkData->_iShopID << '|' << pkData->_iItemShow << '|' << pkData->_lCharID1 << '|' << pkData->_Angle1 << '|'
			<< pkData->_sDesIcon << '|' << Price <<'|'<<pkData->_criticalHitCardBuffID<<'#';

		//sStream << pkData->_subType << '|' << pkData->_iShopID << '|' << pkData->_iItemShow << '|' << pkData->_lCharID1 << '|' << pkData->_Angle1 << '|'
		//	  << pkData->_sDesIcon<< l << '|' << Price <<'#';
	}

	return sStream.str();
}
StringType SC_ClientSkillInterface::GetGuildBossRankReward(int iType)
{
	StringStreamType sStream;
	CF_GuildTowerRankReward::DataEntryMapExternIterator iter = CF_GuildTowerRankReward::GetSingletonPtr()->GetIterator();
	for( ; iter.HasMoreElements(); iter.MoveNext())
	{
		const CF_GuildTowerRankReward::DataEntry *pkData = iter.PeekNextValuePtr();
		if (pkData->_iType == iType)
		{
			sStream << pkData->_iRankStartIndex << '|' << pkData->_iRankEndIndex << '|'<< pkData->_kReward << '#';
		}
		else if (pkData->_iType > iType)
		{
			break;
		}
	}

	return sStream.str();
}

StringType SC_ClientSkillInterface::GetRestraintJob(int iID)
{
	StringStreamType sStream;
	CF_ElementDamage::DataEntryMapExternIterator iter = CF_ElementDamage::GetSingletonPtr()->GetIterator();
	for( ; iter.HasMoreElements(); iter.MoveNext())
	{
		const CF_ElementDamage::DataEntry *pkData = iter.PeekNextValuePtr();
		float damage = 1.0f;
		switch(iID)
		{
		case 0:
			damage = pkData->_fElement0;
			break;
		case 1:
			damage = pkData->_fElement1;
			break;
		case 2:
			damage = pkData->_fElement2;
			break;
		case 3:
			damage = pkData->_fElement3;
			break;
		case 4:
			damage = pkData->_fElement4;
			break;
		}

		if (damage > 1)
			sStream << pkData->_iID << '#';
	}
	return sStream.str();
}

bool SC_ClientSkillInterface::IsScheduleValid(int iScheduleID)
{
	const CF_ActivitySchedule::DataEntry* pkScheData = SafeGetCSVFileDataEntryPtr<CF_ActivitySchedule>(iScheduleID);
	if (NULL == pkScheData)
		return false;

	uint32_t endTime = 0;
	uint32_t beginTime = 0;
	bool bValid=LC_ClientHelp::CheckCronTimeStamp(pkScheData->_sCron, pkScheData->_iTimeLen, beginTime, endTime);
	if (bValid)
	{
		return true;
	}
	else
	{
		LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
		LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
		uint32_t iBeginTimes = pkGamePlayer->GetServerOpenTime();
		int64_t nStartTime = (uint64_t)LC_Helper::GetDayTimeStamp(time_t(iBeginTimes), 0, 0, 0);
		CSVFile::StartTime time1 = pkScheData->_StartValidTime;
		CSVFile::StartTime time2 = pkScheData->_StartCoolTime;
		int iSEndTime1 = time1.GetTimeStramp(nStartTime);
		int iSEndTime2 = time2.GetTimeStramp(nStartTime);
		int iCurTime = pkGamePlayer->GetServerTimeVal();
		return iCurTime>iSEndTime2 && iCurTime<iSEndTime1;
	}
}

StringType SC_ClientSkillInterface::GetGameDir()
{
	char szFile[200];
	_getcwd(szFile, sizeof(szFile));
	StringStreamType sStream(szFile);
	return sStream.str();
}

StringType SC_ClientSkillInterface::GetThemeMainActivityInfo()
{
	StringStreamType sStream;

	CF_BuddyThemeMain::DataEntryMapExternIterator iter = CF_BuddyThemeMain::GetSingletonPtr()->GetIterator();
	for( ; iter.HasMoreElements(); iter.MoveNext())
	{
		const CF_BuddyThemeMain::DataEntry *pkData = iter.PeekNextValuePtr();
		sStream << pkData->_lID << '#'<< pkData->_iSchedule << '#';
		sStream << pkData->_iScheduleShow << '#' << pkData->_sActivity << '#';
		sStream << pkData->_sButton1 << '#' << pkData->_iBuddyTransformID << '#' << pkData->_sTitle << '#';
		sStream    << pkData->_sBkg << '#' << pkData->_sBkgHero<< '#' << pkData->_iBuddyID<< '#' << pkData->_sEnterButton 
			<< '#' << pkData->_iRuneID << '#' << pkData->_sEnterTitle << '#' << pkData->_sEnterBkg<< '#' << pkData->_sEnterButton1 << '|';
	}

	return sStream.str();
}

StringType SC_ClientSkillInterface::GetThemeSubActivityInfo(int iId)
{
	CF_BuddyThemeSecondary::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_BuddyThemeSecondary>(iId);
	if(pkData == NULL)
		return "";

	StringStreamType sStream;
	sStream << pkData->_iType << '#' << pkData->_sActivity << '#';
	sStream << pkData->_sButton << '#' << pkData->_sBkg<< '#' ;
	for (int i=0;i<pkData->_sParam1.size();i++)
	{
		sStream << pkData->_sParam1[i]<<';';
	}
	sStream<<'#' << pkData->_iScheduleShow;
	sStream<<'#' << pkData->_sMainResource;
	sStream<<'#' << pkData->_sLine;
	return sStream.str();
}

int32_t SC_ClientSkillInterface::GetOpenTimeByID(int ID)
{
	int32_t starttime = GET_SERVER_OPEN_TIME;
	CF_UIOpenLevel::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_UIOpenLevel>(ID);
	if(pkData == NULL)
		return 0;
	return (int32_t)pkData->_OpenTime.GetTimeStramp(starttime);
}

int32_t SC_ClientSkillInterface::GetUnLockTimeByID(int ID)
{
	int32_t starttime = GET_SERVER_OPEN_TIME;
	CF_UIOpenLevel::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_UIOpenLevel>(ID);
	if(pkData == NULL)
		return 0;
	return (int32_t)pkData->_UnLockTime.GetTimeStramp(starttime);
}

StringType SC_ClientSkillInterface::GetACTShopAInfo(int iId)
{
	CF_ACTShopA::DataEntry* pkEntry=SafeGetCSVFileDataEntryPtr<CF_ACTShopA>(iId);
	if (!pkEntry)
	{
		return "";
	}

	StringStreamType sStream;
	sStream << pkEntry->_iSchedule << '#' << pkEntry->_kLimits[0].GetItemId() << '#';
	sStream << pkEntry->_iType << '#' << pkEntry->_iKey << '#';
	mem::map<int,int> ResultItemMap;
	MergeRewards(ResultItemMap,pkEntry->_kItems);
	//sStream << pkEntry->_kCurrentPrices << '#' << pkEntry->_kItems;
	for (mem::map<int32_t, int64_t>::iterator it=pkEntry->_kCurrentPrices.begin();it!=pkEntry->_kCurrentPrices.end();++it)
	{
		sStream << it->first <<":"<<it->second<<";";
	}
	sStream <<"#";
	for (mem::map<int, int>::iterator it=ResultItemMap.begin();it!=ResultItemMap.end();++it)
	{
		sStream << it->first <<":"<<it->second<<";";
	}
	sStream <<"#";
	return sStream.str();
}

StringType SC_ClientSkillInterface::GetThemeActivityRaidBossRankRwd(int iActivityId)//此处iActivityId就是mapid
{
	StringStreamType sStream;
	CF_BuddyThemeBossRankReward::DataEntryMapExternIterator iter = CF_BuddyThemeBossRankReward::GetSingletonPtr()->GetIterator();
	for( ; iter.HasMoreElements(); iter.MoveNext())
	{
		const CF_BuddyThemeBossRankReward::DataEntry *pkData = iter.PeekNextValuePtr();
		if (pkData->_iMapID==iActivityId)
		{
			sStream << pkData->_iRankStartIndex << '#' << pkData->_iRankEndIndex << '#' << pkData->_kReward<< '|';
		}
	}
	return sStream.str();
}

StringType SC_ClientSkillInterface::GetThemeRaidBossType(int iActivityId)
{
	StringStreamType sStream;
	CF_BuddyThemeBossType::DataEntry* pkEntry=SafeGetCSVFileDataEntryPtr<CF_BuddyThemeBossType>(iActivityId);
	if (pkEntry)
	{
		sStream<<pkEntry->_iStoryID<<"#"<<pkEntry->_iSchedule<<"#";
		sStream<<pkEntry->_lCharType<<"#";
		sStream<<pkEntry->_sSkills<<"#"<<pkEntry->_sBossDescription<<"#";
		sStream<<pkEntry->_iDamages<<"#"<<pkEntry->_sReward<<"#"<<pkEntry->_iMapID<<"#"<<pkEntry->_sIcon<<"#";
		for (int i=0;i<pkEntry->_sTaskID.size();i++)
		{
			sStream<<pkEntry->_sTaskID[i].GetItemId()<<";";
		}
		sStream<<"#";
	}
	return sStream.str();
}

int SC_ClientSkillInterface::GetThemeRaidBossTypeSchedule(int iActivityId)
{
	CF_BuddyThemeBossType::DataEntry* pkEntry=SafeGetCSVFileDataEntryPtr<CF_BuddyThemeBossType>(iActivityId);
	if (pkEntry)
	{
		return pkEntry->_iSchedule;
	}
	return 0;
}

int SC_ClientSkillInterface::GetChargeRate(int iType)
{
	CF_Charge::DataEntryMapExternIterator iter = CF_Charge::GetSingletonPtr()->GetIterator();
	for( ; iter.HasMoreElements(); iter.MoveNext())
	{
		const CF_Charge::DataEntry *pkData = iter.PeekNextValuePtr();
		if (pkData->_cashType == iType)
		{
			return pkData->_rate;
		}
	}
	return 0;
}

const char* SC_ClientSkillInterface::GetTimeStrByTimeStamp(uint32_t iTimeStamp)
{
	time_t t = iTimeStamp;
	return LC_Helper::TimeStampToString(&t);
}

int SC_ClientSkillInterface::GetRobotScore(uint64_t iId)
{
	CF_ExportSoloDuleRobotInfo::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_ExportSoloDuleRobotInfo>(iId);
	if(pkData == NULL)
		return 0;

	return pkData->_score;
}

int SC_ClientSkillInterface::GetRobotHead(uint64_t iId)
{

	CF_ExportSoloDuleRobotInfo::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_ExportSoloDuleRobotInfo>(iId);
	if(pkData == NULL)
		return 0;

	return pkData->_createBuddyCID;
}

int SC_ClientSkillInterface::GetRobotLevel(uint64_t iId)
{

	CF_ExportSoloDuleRobotInfo::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_ExportSoloDuleRobotInfo>(iId);
	if(pkData == NULL)
		return 0;

	return pkData->_controllerLevel;
}

StringType SC_ClientSkillInterface::GetRobotName(uint64_t iId)
{

	CF_ExportSoloDuleRobotInfo::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_ExportSoloDuleRobotInfo>(iId);
	if(pkData == NULL)
		return 0;

	return pkData->_name;
}

StringType SC_ClientSkillInterface::GetMailText(int iId)
{
	CF_MailInfo::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_MailInfo>(iId);
	if(pkData == NULL)
		return "";

	return pkData->_desc;
}

StringType SC_ClientSkillInterface::GetDecodeScore(uint64_t nInput, bool bTime)
{
	StringStreamType sStream;
	uint32_t iHight = 0;
	uint32_t iLow = 0;
	LC_Helper::DecodeScore(nInput, iHight, iLow);
	if (bTime)
	{
		iLow = LC_Helper::TimeStampReverse(iLow);
	}
	sStream << iHight << "#" << iLow;
	return sStream.str(); 
}

StringType SC_ClientSkillInterface::GetSubCharString(const StringType& sContent, int iBegin, int iEnd)
{
	StdWString wContent = CodeConvert::Utf8ToWcs(sContent);
	return CodeConvert::WcsToUtf8(wContent.substr(iBegin, iEnd));
}

int SC_ClientSkillInterface::GetCharCount(const StringType& sContent)
{
	StdWString wContent = CodeConvert::Utf8ToWcs(sContent);
	return wContent.size();
}

StringType SC_ClientSkillInterface::GetTutorialRewardInfo()
{
	StringStreamType sStream;
	SerializeDataFile<CF_TutorialRewards>(sStream, '#', '|');
	return sStream.str();
}

int SC_ClientSkillInterface::GetChargeProfitCashType(int iType)
{
	CF_ACTChargeProfitType::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_ACTChargeProfitType>(iType);
	if(pkData == NULL)
		return -1;

	return pkData->_Moneytype;
}

StringType SC_ClientSkillInterface::GetSkyArenaRankRewardInfo()
{
	StringStreamType sStream;
	SerializeDataFile<CF_SkyArenaRankReward>(sStream, '#', '|');
	return sStream.str();
}

StringType SC_ClientSkillInterface::GetSkyArenaRankInfo()
{
	StringStreamType sStream;
	SerializeDataFile<CF_SkyArenaRank>(sStream, '#', '|');
	return sStream.str();
}

StringType SC_ClientSkillInterface::GetSpecialTaskInfo()
{
	StringStreamType sStream;
	SerializeDataFile<CF_SpecialTask>(sStream, '#', '|');
	return sStream.str();
}


StringType SC_ClientSkillInterface::GetHolidayMainActivityInfo()
{
	StringStreamType sStream;
	SerializeDataFile<CF_HolidayActivityMain>(sStream, '#', '|');
	return sStream.str();
}

StringType SC_ClientSkillInterface::GetHolidaySubActivityInfo(int iId)
{
	CF_HolidayActivitySecondary::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_HolidayActivitySecondary>(iId);
	if(pkData == NULL)
		return "";

	StringStreamType sStream;
	SerializeDataEntryPtr<CF_HolidayActivitySecondary>(pkData, sStream, '#');
	return sStream.str();
}

StringType SC_ClientSkillInterface::GetGuessCost(int ID)
{
	StringStreamType sStream;

	CF_Guess::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_Guess>(ID);
	T_ASSERT_CRITICAL_S(pkEntry != NULL);

	const Utility::UT_SIMDataList& attr = pkEntry->_guessConsumeCash;
	for(Utility::UT_SIMDataList::const_iterator it=attr.begin(); it!=attr.end(); ++it)
	{
		sStream << it->ID() << '*' << it->ParamA() << '*';
	}

	return sStream.str();
}

StringType SC_ClientSkillInterface::GetGuessProfit(int ID)
{
	CF_Guess::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_Guess>(ID);
	T_ASSERT_CRITICAL_S(pkEntry != NULL);
	return pkEntry->_guessReward;
}

StringType SC_ClientSkillInterface::GetSummitArenaMatchName(int iID)
{
	CF_SummitArenaMatchName::DataEntry* pkEntry = SafeGetCSVFileDataEntryPtr<CF_SummitArenaMatchName>(iID);
	if(pkEntry == NULL)
		return "";

	return pkEntry->_sMatchName;
}

StringType SC_ClientSkillInterface::GetSummitArenaRankReward()
{
	StringStreamType sStream;
	SerializeDataFile<CF_SummitArenaRankRewards>(sStream, '#', '|');
	return sStream.str();
}

bool SC_ClientSkillInterface::IsSoloduleRobotPlayerId(uint64_t uiID)
{
	return  IS_SOLODULE_ROBOT_PLAYERID(uiID);
}

int SC_ClientSkillInterface::GetPreStoryId(int storyId)
{
	CF_GameStory::DataEntry* pkEntry = SafeGetCSVFileDataEntryPtr<CF_GameStory>(storyId);
	if (pkEntry == NULL)
	{
		return 0;
	}
	return pkEntry->_preStoryID;
}

int64_t SC_ClientSkillInterface::GetNextStoryTime(long lStoryID)
{
	CF_GameStory::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_GameStory>(lStoryID);
	if (pkData->_iID == lStoryID)
	{
		LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
		LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
		time_t iSystemTime = (time_t)pkGamePlayer->GetServerTimeVal();
		Utility::UT_CronData kCron;
		kCron.ParseCronStr(pkData->_kCronStr);
		time_t t = kCron.GetNextTime(iSystemTime);
		return (int64_t)t;
	}
	return 0;
}

/* 
类型，0历练 1任务：
任务/历练 id
奖励:
奖励数量:
当前进度：
总进度：
是否被领取:
历练/任务描述
历练:linkID 任务:0;
*/
StringType SC_ClientSkillInterface::GetHAPTTaskInfo()//活动拼图任务（HolidayActivityPinTu HAPT）信息
{

	StringStreamType sStream("");	

	CF_HolidayActivity_PinTuUnlock::DataEntryMapExternIterator Iter = CF_HolidayActivity_PinTuUnlock::GetSingletonPtr()->GetIterator();
	for( ; Iter.HasMoreElements(); Iter.MoveNext())
	{
		const CF_HolidayActivity_PinTuUnlock::DataEntry *pkData = Iter.PeekNextValuePtr();

		if (pkData->_iTaskID > 0)
		{		
			StringType TaskInfoStr;
			GetTaskInfoStringById(pkData->_iTaskID,TaskInfoStr);
			sStream<<TaskInfoStr;
			continue;
		}
		if (pkData->_iAchievementID > 0)
		{
			StringType AchievementInfoStr;
			GetAchievementInfoStringById(pkData->_iAchievementID,AchievementInfoStr);
			sStream<<AchievementInfoStr;
			continue;
		}

	}
	return sStream.str();
}


void SC_ClientSkillInterface::GetTaskInfoStringById(int iTaskID,StringType& OutStr)
{
	StringStreamType sStream("");
	OutStr.clear();
	LC_ClientGamePlayer* pkGamePlayer = GetPlayerPtr();
	

	CF_TaskList::DataEntry *pkTaskData=SafeGetCSVFileDataEntryPtr<CF_TaskList>(iTaskID);
	if(!pkTaskData)
	{
		return;
	}

	LC_TaskMap* pkTaskMap = pkGamePlayer->GetTaskMap();
	if (!pkTaskMap)
	{
		return;
	}
	bool bFinished=pkTaskMap->IsInFinishedTaskMap(iTaskID);
	LC_ProcessingTaskEntry* pkTaskEntry = pkTaskMap->GetProcessingTaskEntryByID(iTaskID);
	TStringVector kvect = UT_BaseStringUtility::SplitString(pkTaskData->_sRewardItemsA, ':');
	LC_TaskManager* pTaskMgr=LC_TaskManager::GetSingletonPtr();
	LC_Task* pTask = pTaskMgr->GetTask(iTaskID);
	LC_Task_Condition* pCondition=pTask->GetFinishConditionNoParams(0);
	int CurCount=0;
	int MaxCount = pCondition->GetMaxCount();
	StringType TraceInfo=pTask->GetTaskTraceInfo(iTaskID);

	if (pkTaskEntry)
	{
		CurCount=pkTaskEntry->GetFinishState1();
	}
	else if(!bFinished)
	{
		pkGamePlayer->ReqAcceptTask(iTaskID);
		CurCount=0;
	}
	else
	{
		CurCount=MaxCount;
	}
	sStream<<"1:"		//类型，0历练 1任务
		<<iTaskID<<":"	//任务/历练 id
		<<kvect[0]<<":"	//奖励:
		<<kvect[1]<<":"	//奖励数量:
		<<CurCount<<":"	//当前进度：
		<<MaxCount<<":"	//总进度：
		<<bFinished<<":"//是否被领取:
		<<TraceInfo<<":"//历练/任务描述
		<<"0"<<"|";		//历练:linkID 任务:0;

	OutStr=sStream.str();
}


void SC_ClientSkillInterface::GetAchievementInfoStringById(int iAchievementID ,StringType& OutStr)
{
	StringStreamType sStream("");
	OutStr.clear();
	LC_ClientGamePlayer* pkGamePlayer = GetPlayerPtr();
	CAchievementAsset& pkAchievementMap = pkGamePlayer->GetAchievementAsset();

	CF_AchievementList::DataEntry *pkAchivementData=SafeGetCSVFileDataEntryPtr<CF_AchievementList>(iAchievementID);
	if(!pkAchivementData)
	{
		return;
	}

	bool bFinished=pkAchievementMap.IsInFinishAchievementRecord(iAchievementID);
	int64_t iMax=pkAchivementData->_iConditionThreshold;
	int64_t iProgress = 0;
	if (bFinished)
	{
		iProgress = iMax;
	}
	else
	{
		iProgress = pkAchievementMap.GetAchievementProcessing(iAchievementID);
	}


	TStringVector kvect = UT_BaseStringUtility::SplitString(pkAchivementData->_sRewardItems, ':');

	sStream<<"0:"				//类型，0历练 1任务：
		<<iAchievementID<<":";	//任务/历练 id
	if(kvect.size() >= 2)
	{
		sStream << kvect[0] << ":"			//奖励:
		<< kvect[1] << ":";			//奖励数量:
	}
	else
	{
		sStream << ":0:";
	}
	sStream <<iProgress<<":"		//当前进度：
		<<iMax<<":"				//总进度：
		<<bFinished<<":"		//是否被领取:
		<<pkAchivementData->_sAchievementinfo<<":"//历练/任务描述
		<<pkAchivementData->_iLinkID<<"|";//历练:linkID 任务:0;
	
	OutStr=sStream.str();
}

StringType SC_ClientSkillInterface::GetHAPTReward()//活动拼图（HolidayActivityPinTu HAPT）奖励
{
	StringStreamType sStream("");	

	CF_HolidayActivity_PinTuReward::DataEntryMapExternIterator Iter = CF_HolidayActivity_PinTuReward::GetSingletonPtr()->GetIterator();
	for( ; Iter.HasMoreElements(); Iter.MoveNext())
	{
		const CF_HolidayActivity_PinTuReward::DataEntry *pkData = Iter.PeekNextValuePtr();
		sStream<<pkData->_iKey << "|"  << pkData->_iType<< "|"<< _getRewardInfo(GetPlayerPtr(),pkData->_kReward)<<";";//需要的拼图，奖励id和奖励数量
	}

	return sStream.str();
} 

StringType SC_ClientSkillInterface::GetContinousChargeInfo() {
	StringStreamType sStream;
	SerializeDataFile<CF_HolidayActivity_ChargeContinuous>(sStream, '#', '|');
	return sStream.str();
}