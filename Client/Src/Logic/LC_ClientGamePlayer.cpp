#include "ClientPCH.h"
#include "LM_LocaleManager.h"
#include "IM_Manager.h"
#include "BM_Manager.h"
#include "CL_RichTextParse.h"
#include "GlobalSettings.h"

#include "LC_ClientGamePlayer.h"
#include "LC_ClientShortcutBar.h"
#include "LC_ClientItem.h"
#include "LC_ClientItemFactory.h"
#include "LC_ClientNetPlayer.h"
#include "LC_ClientNetPlayerManager.h"
#include "LC_ClientGamePlayerManager.h"
#include "LC_ClientNPCManager.h"
#include "LC_ClientNPC.h"
#include "LC_ClientPlayerGroup.h"
#include "LC_ClientPlayerGroupManager.h"
#include "LC_PlayerFlopManager.h"
#include "LC_ClientTask.h"
#include "LC_ClientTaskMap.h"
#include "LC_ClientMap.h"
#include "LC_ClientWorldManager.h"
#include "LC_ClientLogicManager.h"
#include "LC_ClientLogicAreaManager.h"
#include "LC_NPCShopManager.h"
#include "LC_NPCTransferManager.h"
#include "LC_GameEventHandler.h"
#include "LC_FactionManager.h"
#include "LC_ClientMailManager.h"
#include "LC_ClientGuildManager.h"
#include "LC_ClientAuctionManager.h"
#include "LC_ClientProduction.h"
//#include "LC_ClientCurrencyTradeManager.h"
#include "LC_ClientMapTranscriptionManager.h"
#include "LC_TitleManager.h"
#include "LC_ClientToyPetManager.h"
#include "LC_ClientCollectionManager.h"
#include "LC_ClientBotManager.h"
#include "LC_ClientCombatRecordManager.h"
#include "LC_ClientGuildProfitManager.h"
#include "LC_ClientArenaPlayBackManager.h"
#include "LC_ClientReportManager.h"
#include "MG_MessageBase.h"

#include "CA_CameraPathAnimation.h"
#include "CA_CameraManager.h"
//#include "CA_CameraManager.h"
#include "CF_LevelUp.h"

//#include "SM_ClientPlayerState_STAND.h"

#include "SM_ClientPlayerState_JUMP.h"
#include "SM_ClientGamePlayerController.h"
#include "SM_ClientControllerFactory.h"

#include "SK_SkillExecute.h"
#include "SK_Factory.h"

#include "GD_ClientActorModel.h"

#include "UT_LogicInfoMessage.h"

#include "NW_Game2ClientSession.h"
#include "NW_Helper.h"
#include "MG_VehicleProtocol.h"
#include "MG_GameProtocol.h"

#include "NW_ClientSessionManager.h"
#include "SM_GameStageMachine.h"
#include "UI_UIManager.h"
#include "MG_CS_FriendProtocol.h"
#include "MG_CS_SearchPlayerProtocol.h"
#include "MG_ItemProtocol.h"
#include "LC_ClientDialogManager.h"

#include "AS_ClientSoundManager.h"
#include "LC_MapManager.h"
#include "IP_InputManager.h"

#include "LC_TranscriptionDataType.h"

#include "RG_RenderManager.h"
#include "UT_CutSceneManager.h"
#include "TM_ClientTimeManager.h"

#include "MG_ShortcutKeyProtocol.h"
#include "LC_ClientNPCInteractionManager.h"

#include "SC_ClientScriptInterface.h"
#include "SC_ClientSkillInterface.h"

#include "SK_ClientSkillEventManager.h"
#include "GD_BulletManager.h"

#include "msg.pb.h"

#include <algorithm>
#include "MG_CS_QuanThanhTemple.h"
#include "LC_ClientArenaManager.h"

//#include "LC_ClientSafeSystemAsset.h"
#include "UE_RenderManager.h"
#include "UI_UIManager.h"

#include "WT_WidgetManager.h"
#include "CL_CUIDragItem.h"
#include "AS_ClientSoundManager.h"
#include "LC_ClientGuildWarAsset.h"
#include "MG_Game2Group.h"
#include "MG_AssistProtocol.h"
#include "MG_ItemProtocol.h"
#include "BL_AsyncOperationManager.h"
#include "LC_ClientPlaceHolderManager.h"
#include "SM_WalkTransferManager.h"
#include "LC_ClientNewBeeManager.h"
#include "LC_ClientHelp.h"
#include "../../../Tool/Downloader/Downloader/IDownloader.h"
#include "ClientConfigManager.h"
#include "boost/lexical_cast.hpp"

#include "CF_CommanderTitle.h"
#include "CF_Buddy.h"
#include "CF_MapEffectConfig.h"
#include "CF_NPCSoundList.h"
#include "CF_Helper.h"
#include "CF_ItemList.h"
#include "CF_WorldMapList.h"
#include "CF_TranscriptionRule.h"
#include "CF_SkillLevelUp.h"
#include "CF_GameStory.h"
#include "CF_AreaEffect.h"
#include "CF_WildPrinces.h"
#include "CF_TranscriptionSettleGrade.h"
#include "CF_Subordinate.h"
#include "CF_NPCAvatar.h"
#include "CF_SocketItemFunc.h"
#include "CF_CharAttributeFunc.h"
#include "CF_SubordinateTransform.h"
#include "CF_ChatCooldown.h"
#include "CF_SmartHook.h"
#include "CF_BuddyUpgrade.h"
#include "CF_StarVeinList.h"
#include "CF_InvestmentPlan.h"
#include "CF_GloryExpedition.h"
#include "CF_GloryExpeditionDailyRewards.h"
#include "CF_SpecialTraining.h"
#include "CF_YiJiTuXiStageReward.h"
#include "CF_YiJiXunBaoStageReward.h"
#include "CF_PetIslandStageReward.h"
#include "CF_BestGuild.h"
#include "CF_RuneList.h"
#include "CF_BestGuildBossInfo.h"
#include "CF_BestGuildTask.h"
#include "LC_AchievementManager.h"
#include "CF_UIOpenLevel.h"
#include "CF_BestGuildChapterID.h"
#include "CF_EntrustTask.h"
#include "CF_BuddyThemeTaskList.h"
#include "CF_BuddyThemeTaskType.h"
#include "CF_BuddyThemeBossType.h"
#include "CF_VipNew.h"
#include "CF_TreasureEvent.h"
#include "CF_HolidayActivityTaskType.h"
#include "CF_HolidayActivityTaskList.h"
#include "CF_HolidayActivity_PinTuUnlock.h"
#include "CF_SpecialTask.h"

using namespace Base;
using namespace StateMachine;
using namespace CSVFile;
using namespace Protocol;
using namespace GameLogic;
using namespace Utility;
using namespace Skill;
using namespace Path;
using namespace Data;
using namespace Script;
using namespace Memory;

#include "LC_ClientGamePlayer.inl"
#include "LC_ClientGamePlayer_MsgHandler.inl"


#include "UT_FileManagerBase.h"
#include "BL_AsyncOperation.h"
#include "BL_AsyncOperationManager.h"

#include "LC_PlaceHolderManagerBase.h"

#include "MG_PB.h"
#include "UT_Curl.h"

#include "msg.pb.h"
#include <queue>

#define DBG_USE_3D_COORD_AXIS_GFX 0
#if		DBG_USE_3D_COORD_AXIS_GFX
	#undef		RES_FX_CHOOSE_TARGET_YELLOW
	#define 	RES_FX_CHOOSE_TARGET_YELLOW		"坐标轴.gfx"
#endif


#define REQ_TRANSFER_INTERVAL 1.0f
#define NEED_TARGET_INTERVAL	1.0f
#define TIME_REMAINDER_INTERVAL 43200
//#define TIME_REMAINDER_INTERVAL 60  //TEST
#define TIME_REMAINDER_INTERVAL_HOUR 12

bool LC_ClientGamePlayer::TryReadMap(int lMapResID)
{
#ifdef USE_DOWNLOADER
	IDownloader* downloader = UT_FileManagerBase::GetSingletonPtr()->GetDownloader();
	if (downloader)
	{
		CF_WorldMapList::DataEntry* pkData = CF_WorldMapList::GetSingletonPtr()->GetEntryPtr(lMapResID);

		StringType rkWorldMapName = pkData->_kFilePath + ".tmap";

		//打点: 开始第二次下载（新手场景、主角资源、UI, 20M）
		commitPatchProgress(EVENT_CLIENT_DOWNLOAD_1st_SCENE, 0);

		if (UT_FileManagerBase::GetSingletonPtr()->AccessFile(rkWorldMapName.c_str()))
		{
			return true;
		}

		//从外部置downloader的状态，因为不能在recv message中不能立即开始下载，得等异步加载完成，同时得阻止SC_ClientSkillInterface::GetLoadingPercent()跳入后续加载过程。
		//downloader->setMapDownload(true);
		downloader->setUserData(ClientGamePlayerLoadingMapKey, rkWorldMapName.c_str());

		MG_MessageCenter::GetSingletonPtr()->SendMessage( CreateMsg_ADD_TMAP_LOAD_TASK(rkWorldMapName), true);	//必须true

		return false;
	}
#endif

	return true;
}

void	LC_ClientGamePlayer::tagMapLoadInfo::set(int iMapLogicID,int lMapResID,int lMapLogicType,const Utility::UT_Vec3Int& rkLocation)
{
	m_iMapLogicIDSave   = iMapLogicID;
	m_lMapResIDSave     = lMapResID;
	m_lMapLogicTypeSave = lMapLogicType;
	m_rkLocationSave	= rkLocation;
}

//-------------------------------------------------------
LC_ClientGamePlayer::LC_ClientGamePlayer()
: LC_PlayerBase()
, m_lUserID(0)
, m_pkClientLocalSession(NULL)
, m_fLastSyncMoveTime(0.0f)
, m_lLastMoveSpeed(0)
, m_LastDirAngle(0)
, m_fLastSyncUIOperationTime(0.0f)
, m_eMouseLogicModeType(MLMT_NORMAL)
, m_fLastAutoUpgradeSkillTime(0.0f)
, m_bAutoUpgradeSkill(false)
, m_bDoAutoUpgradeSkill(false)
, m_iCurrentUpgradeSkillID(0)
, m_lLastLockTargetID(0)
, m_lLastLockTargetNeedGfx(0)
, m_lWatchTargetID(0)
, m_lLastWatchTargetID(0)
, m_lMouseOverActorID(0)
, m_lLastMouseOverActorID(0)
, m_lMouseOverInteractiveBtnActorID(0)
, m_lLastMouseOverInteractiveBtnActorID(0)
, m_lPreLockedTargetID(0)
, m_pPreLockedTargetModel(0)
, m_bIsFreezeLock(0)
, m_bWatchTargetFactionChange(false)
, m_fAutoSearchEnemyCDTime(0.0f)
, m_fLastSceneMouseOverTime(0.0f)
, m_bLastSceneMouseOver(false)
, m_bMoveStride(false)
, m_bWASDStride(false)
, m_lChatTargetID(0)
, m_strCharTargetName("")
, m_fLastReqFriendStatusTime(0.0f)
, m_iCurrentChatChannel(CCT_NEAR)
, m_bNpcTask(false)
, m_bNeedLockTradeTarget(false)
, m_bNeedLockLocation(false)
, m_bNeedLockTarget(false)
, m_fLastPickTerrainTime(-1.0f)
, m_fLastPrepareMoveStrideTime(-1.0f)
, m_pkFlopManager(NULL)
, m_sItemTipMessage("")
, m_bTeamHint(false)
, m_bKeyTabPressed(false)
, m_lSaveWatchTargetID(0)
, m_lSaveLockedTargetID(0)
, m_pkClientProduction(NULL)
, m_bRequestLogicTrigger(false)
, m_pkWaitPendingSkill(NULL)
, m_lWaitPendingSkillItemType(0)
, m_bArriveFlag(true)
, m_iLockLocationFXID(0)
, m_iLockLocationFxType(0)
, m_iPendingPathMapResID(0)
, m_iPendingPathMapLogicID(0)
, m_fLastLogicTriggerTime(0.0f)
, m_bMoveStraightFlag(false)
, m_lMoveStraightParam(-1)
, m_bResourceRetrieve(false)
, m_lSelectedNPCIDInCareerLearn(0)
, m_bIsCanStartTutorial(false)
, m_currentGameServerPing(0)
, m_currentGateWayPing(0)
, m_currentSocketPing(0)
, m_token(0)
//, m_bAutoFindPathFlag(false)
, m_fLastCheckPendingTalkNPCTime(0)
, m_bWaitingShortcutKey(false)
, m_pkScriptControllerFunctor(NULL)
, m_fLastRequestOwnerShipTime(0)
, m_iNewPlayerLevel(0)
, m_bPendingToSetVisibleDataEnableFakeShadow(false)
, m_iPVPGfxID(0)
, m_bEnableAfkToMeditation(true)
, m_bEnableToGuding(false)
, m_bTutorialStop(false)
, m_fGudingTimer(FLT_MAX)
, m_fStandToHookTimer(0)
, m_bEnableStandToHook(true)
, m_fInvalidTime(0.0f)
, m_kInvalidPos(0,0,0)
, m_fLastInvalidTarget(0)
, m_kSpecialHookCharType(0)
, m_bHooking(FALSE)
, m_lHookTime(0)
, m_iHookingTargetID(0)
, m_bStopHookOnTargetDead(false)
, m_fAssistCDTime(0)
, m_nDailyActivity(0)
, m_fLastAttackerTime(0)
, m_fTranscriptionCDTimer1(0)
, m_fTranscriptionCDTimer2(0)
, m_bTranscriptionFromReturnToCity(false)
, m_bDropItemTipsShowing(false)
//, m_lastWeaponItemID(0)
//, m_lastBowLevel(0)
//, m_lastWingCharType(0)
//, m_lastEyeCharType(0)
//, m_iEyeNPCID(0)
, m_iCrossRealMapTeamID(0)
, m_iComboEffectData(0)
, m_summorFriendCharID(0)
, m_bForbbidenKey(false)
, m_lPVPTargetID(0)
, m_iPowerGuildId(0)
, m_iGuildWarType(0)
, m_fRespawnTimer(-1.0f)
, m_eRespawnFadeState(RG_RenderManager::SGFIOS_INACTIVE)
, m_lGuildTransportStamp(0)
, m_iSkyArenaPoint(0)
, m_iSkyArenaHonorPoint(0)
, m_iLastSkyArenaPoint(0)
, m_iSkyArenaSeason(0)
, m_iSkyArenaCloseTime(0)
, m_iSkyArenaNxtTime(0)
, m_iTutorialProgress(0)
, m_bImmediateTaskStart(false)
//, m_bInitAttribute(false)
, m_uiLastOffLineTime(0)
, m_bCimeliaAutoPicking(false)
, m_bIntelligentTask(true)
, m_bPostEnterGameFlag(true)
, m_bRuneBuddySlotOpen(false)
, m_bRuneSubSlotOpen(false)
, m_bFirstEnterGame(false)
, m_bGuildRewardIsVaild(false)
, m_uiCurrentActiveBuddyIndex(-1)
, m_iUnlockSlotNum(0)
, m_iUnlockSlotInit(0)
, m_dayFlag(0)
, m_bActivateBuddyFlag(false)
, m_fAutoPathSoundStartTime(0)
, m_iLotteryConsumeUnbindyuanbao(0)
, m_iCurVipNewLevel(999)
,m_iSevenDaysEndTime1(0)
,m_iSevenDaysEndTime2(0)
,m_iOfferTaskRefreshTimes(0)
,m_iCollectBossJP(0)
,m_iChainFlag(0)
,m_iRegularBossGroupID(0)
,m_iRegularBossGroupCDTime(0)
,m_bSwitchLine(0)
,m_bIsSwitich(false)
,m_iGloryDailRewardDrawed(0)
,m_BestGuildFinishedTask(0)
,m_bCommanderTitleOpen(false)
,m_processPathFlag(false)
,m_iResourceRetrieveLevelRecord(0)
,m_FirstChargeTime(0)
,m_iNewAchievementStarNum(0)
,m_bGuildProfitShowName(false)
,m_iGuildProfitLevel(0)
,m_iGuildProfitLevelExp(0)
,m_iGuildProfitCrystalLevel(0)
,m_iGuildProfitCrystalLevelExp(0)
,m_iAnimIndex(-1)
,m_PickActorID(0)
,m_iFinishMainTaskID(0)
,m_bhookRespawnBuyFlag(false)
,m_iSkyArenaRankRewardId(0) 
,m_iMaxScore(0) 
,m_iStartTime(0)
,m_iEndTime(0)
,m_stageType(0)
,m_showTime(0)
,m_battleStTime(0)
,m_myRank (0)
,m_zoneId (0)
,m_stageSetEndTime(0)
,m_stageStTime(0)
,m_stageEndTime(0)
,m_nTotalGroupId(0)
,m_summitRedPoint(false)
,m_isValid(false) 
,m_invitedType(0)
,m_myTopType(0)
,m_leftTimes(0)
,m_buyTimes(0) 
,m_Rate(0)
,m_bAcceptTaskLogin(false)
,m_iCurrentLineNum(-1)
{
	m_bCheckPendingRepeatedSkill = true;	//local player才有效
	SM_StateMachine::Init();

	//状态控制器
	m_pkSMController = SM_ClientControllerFactory::GetSingletonPtr()->RequestController(SCT_CLIENT_GAME_PLAYER);

	m_pkShortcurtBar = T_NEW_D LC_ClientShortcutBar((LC_PlayerBase*)this);
	//m_kTaskMap.SetOwner(this);
	m_pkFlopManager = T_NEW_D LC_PlayerFlopManager(this);

	//Begin 路径数据
	m_kMovePath.Clear();
	//End 路径数据

	m_pkClientProduction = T_NEW_D LC_ClientProduction();
	//_initChestInfo();
	m_pkTaskMap			  = T_NEW_D LC_ClientTaskMap;
	m_pkTaskMap->SetOwner(this);

	//m_pkSafeSystemAsset = T_NEW_D LC_ClientSafeSystemAsset;
	//m_pkSafeSystemAsset->SetOwner(this);

	m_pkGuildwarAssert = T_NEW_D LC_ClientGuildWarAsset;
	m_pkCommander = T_NEW_D LC_ClientGamePlayerCommander;
	
	m_kClientSocietyAsset.SetOwner(this);
	
	ReUse();

	_initMuhunInfo();

	ResetLastEquipData();
	m_vActiveBuddyList.reserve(MAX_ACTIVE_BUDDY_NUM);
	m_vInactiveBuddyList.reserve(MAX_BUDDY_COUNT);
	m_kTimeRemainState.clear();
	m_BestGuildTowerInfo.clear();
	m_BestGuildFortressInfo.clear();
	m_BestGuildRoyalInfo.clear();
	m_ActiveCodeUseCount.clear();
	m_kFirstDailyRecord.clear();
	m_ActiveStarVeinIDs.clear();
	m_ActiveStarVeinGoalIDs.clear();
	m_mBuddyCombat.clear();
	m_mBestGuildGoalInfo.clear();
	m_kBossFirstKill.clear();
	m_mBossFirstKillRewardPlayerInfo.clear();
	m_kServerBossFirstKillRecord.clear();
	m_mBossFirstKillRecord.clear();
	m_kSuperShopCompete.clear();
	m_kSuperShopSub.clear();
	m_kCharacterUpgradGift.clear();
	m_HolydeedUserCount.clear();
	v_InvestmentType.clear();
	v_InvestmentFreeReward.clear();
	v_InvestmentHighReward.clear();
	m_vBuddyChainVec.resize(MAX_BUDDY_COUNT);
	m_nDailyActivityRewardID.reserve(MAX_DAILYACTIVITY_COUNT);
	for(int i=0; i<MAX_ACTIVE_BUDDY_NUM; ++i)
		m_kSkillBookSlotData[i].clear();
	m_nBiographyChapters.reserve(50);
	m_mSchemeInfoMap.clear();
	m_mCommanderEquipSuitCollection.clear();
	m_mResourceRetrieveRecord.clear();
	m_mResourceRetrieveMapRecord.clear();
	m_kRuneRecordMap.clear();
	m_mBossKillerInfo.clear();
	m_mapActivate.clear();
	m_nMyInfo.clear();
	m_nOtherInfo.clear();
	m_VipNewRewardRecord.clear();
	m_vRaidBossSpeciesRankThree.resize(3);
	m_vRaidBossSpeciesRankList.resize(5);
	m_kRuneBuddySlotDressStateMap.clear();
	m_mCommanderFrame.clear();
	m_mCommanderHead.clear();
	m_vNewAchievementRewardCord.clear();
	m_vGuildProfitCrystal.clear();
	m_mGuildBossActivity.clear();
	m_sGuildBossActivityFinishMap.clear();
	m_vGuildBossActivityRankThree.clear();
	m_vGuildBossActivityRankList.clear();
	m_kSingleCombatData.clear();
	m_kRaidBossDropData.clear();
	
}
//-------------------------------------------------------
LC_ClientGamePlayer::~LC_ClientGamePlayer()
{
	ClearDirtyFlag();
	ClearDirtyFlagRevisionID();

	T_SAFE_DELETE( m_pkShortcurtBar );
	T_SAFE_DELETE(m_pkClientProduction);
	T_SAFE_DELETE(m_pkFlopManager);
	T_SAFE_DELETE(m_pkCommander);
	if (m_pkClientLocalSession)
	{
		m_pkClientLocalSession->SetPlayer(NULL);
	}

	ResetLastEquipData();
	m_kTaskState.clear();
	m_kPlayerEquipSubSuitPtrMap.clear();
	m_vActiveBuddyList.clear();
	m_BestGuildTowerInfo.clear();
	m_BestGuildFortressInfo.clear();
	m_BestGuildRoyalInfo.clear();
	m_vInactiveBuddyList.clear();
	m_mBuddyCombat.clear();
	m_kSuperShopCompete.clear();
	m_kSuperShopSub.clear();
	m_kBossFirstKill.clear();
	m_mBestGuildGoalInfo.clear();
	m_mBossFirstKillRewardPlayerInfo.clear();
	m_kServerBossFirstKillRecord.clear();
	m_mBossFirstKillRecord.clear();
	m_nDailyActivityRewardID.clear();
	m_mBuddyInstanceMapItr itr = m_mBuddyInstanceMap.begin();
	for(; itr != m_mBuddyInstanceMap.end(); ++itr)
	{
		T_SAFE_DELETE(itr->second);
		itr->second = NULL;
	}
	m_mBuddyInstanceMap.clear();
	m_mCommanderEquipSuitCollection.clear();
	m_mResourceRetrieveRecord.clear();
	m_mResourceRetrieveMapRecord.clear();
	m_nBiographyChapters.clear();
	m_mSchemeInfoMap.clear();
	m_kRuneRecordMap.clear();
	m_mBossKillerInfo.clear();
	m_mCommanderFrame.clear();
	m_mCommanderHead.clear();
	m_vServerPlayerBossFlagInfo.clear();
}
//------------------------------------------------------------------------
void LC_ClientGamePlayer::InitLogicData()
{
	LC_PlayerBase::InitLogicData();
	if (m_pkTaskMap)m_pkTaskMap->Clear();

	m_lHP = -1;
	m_lMP = 0;
	m_lEXP = -1;
	m_SkillLevelupPoints = -1;


	m_iPendingPathMapResID = 0;
	m_iPendingPathMapLogicID = 0;
	m_kPendingPathPos.x = 0;
	m_kPendingPathPos.y = 0;
	m_kPendingPathPos.z = 0;
	m_kPendingVal = 0;
	m_lSelectedNPCIDInCareerLearn = 0;

	LoadControlScript(false,false);

	((LC_ClientShortcutBar*)GetShortcutBar())->ClearBackShortcutBar();
	((LC_ClientShortcutBar*)GetShortcutBar())->SetBackShortActive( false);

	_initProductionData();

	//_initFisrstChargeData();
}
//-------------------------------------------------------
bool LC_ClientGamePlayer::RecvMessage( MG_MessageBase& rkMessage )
{
#ifdef USE_DOWNLOADER
	if (rkMessage.m_eMsgType == MGT_TIMER_TICK)
	{
		if (d_MapLoadInfo.isLoading())
		{
			if (d_MapLoadInfo.isLoaded())
			{
				//必须先设置标志位，再读地图，否则
				//UT_FileManagerBase::GetSingletonPtr()->GetDownloader()->setMapDownload(false);
				IDownloader* downloader = UT_FileManagerBase::GetSingletonPtr()->GetDownloader();
				downloader->setUserData(ClientGamePlayerLoadingMapKey, "");

				d_MapLoadInfo.endLoad();

				ChangeMapImp(d_MapLoadInfo.m_iMapLogicIDSave, d_MapLoadInfo.m_lMapResIDSave, d_MapLoadInfo.m_lMapLogicTypeSave, d_MapLoadInfo.m_rkLocationSave);
			}
		}
	}
#endif

	if (m_pkSMController->GetControllerType() == SCT_CLIENT_GAME_PLAYER)
	{
		return ((SM_ClientGamePlayerController*)m_pkSMController)->RecvMessage(this, rkMessage);
	}
	return false;
}
//-------------------------------------------------------
void LC_ClientGamePlayer::SendToServer(char *pMsg, uint16_t wSize)
{
	if (m_pkClientLocalSession)
		m_pkClientLocalSession->Send(pMsg,wSize);
}
//------------------------------------------------------------------------
void LC_ClientGamePlayer::OnDisconnect()
{
	SetMouseLogicModeType(MLMT_NORMAL);
	LC_WorldManagerBase* pkWorldManager = LC_LogicManagerBase::GetSingletonPtr()->GetWorldManager();
	pkWorldManager->RemoveObjectFromMap(this);
	//SendMessage(CreateMsg_LOGIC_SYSTEM_INFO("你已经与服务器断开连接",SIT_CONFIRM_DISCONNET_SERVER),true);
}
//-------------------------------------------------------
void LC_ClientGamePlayer::SetHP( attr_value_type iHP )
{
	attr_value_type iOld_hp = GetHP();
	attr_value_type lDeltaHP = iHP - iOld_hp;

	if(lDeltaHP != 0 )
		SendMessage( CreateMsg_UE_UPDATE_PLAYER_BLOOD( GetID(), 0) );

	LC_PlayerBase::SetHP(iHP);

	//bool bShowLowHPGfx = false;
	attr_value_type iMAXHP = GetAttributeMap()->GetAttribute(ATT_MAX_HP);
	attr_value_type iUpper = iMAXHP * 0.2f;
	if ((iHP > 0 && iHP < iUpper) && (iOld_hp >= iUpper || iOld_hp <= 0))
	{
		CF_Buddy::DataEntry *pkBuddyData = SafeGetCSVFileDataEntryPtr<CF_Buddy>(this->GetCharType());
		if(pkBuddyData != NULL)
		{
			int iSoundID = g_soundMgr.Play3DEx(UT_EngineMath::ConvertVec3IntToNiPoint3(GetCurrentLocation()), pkBuddyData->_sHurtSound, AS_ClientSoundManager::SCT_PLAYER_SKILL);
			ResetVoice(iSoundID);
		}
	}

	//血量过低警示特效
	/*IM_Manager* pkIMManager = IM_Manager::GetSingletonPtr();
	if(pkIMManager)
	{
		GD_ActorModel* pkVisibleData = pkIMManager->GetActor(GetID());
		if(pkVisibleData)
		{
			if (bShowLowHPGfx)
				pkVisibleData->AttachFx(GD_ModelBase::FDT_INVALID, FileSystem::MTF_ModelData::AFBM_TRANSLATE_ROTATE, true, RES_FX_LOW_HP_WARNING);
			else if (iHP >= iUpper || iHP <= iLow)
				pkVisibleData->DetachFx(GD_ModelBase::FDT_INVALID, RES_FX_LOW_HP_WARNING, true);
		}
	}*/
}
//--------------------------------------------------------
void LC_ClientGamePlayer::SetSkillLevelupPoints(int64_t Value)
{
	attr_value_type iOld_Sp = GetSkillLevelupPoints();
	attr_value_type lDeltaSP = Value - iOld_Sp;

	LC_PlayerBase::SetSkillLevelupPoints(Value);

	if (lDeltaSP > 0 && iOld_Sp >= 0)
	{
		SendLogicInfoMessage(RE_ADD_SP, lDeltaSP);
	}
	if (lDeltaSP < 0 && iOld_Sp >= 0)
	{
		SendLogicInfoMessage(RE_DEL_SP, -lDeltaSP);
	}
}
//--------------------------------------------------------
void LC_ClientGamePlayer::SetMP(int iMP)
{
	int lDeltaMP = iMP - GetMP();
	if (lDeltaMP != 0) 
		SendMessage( CreateMsg_UE_UPDATE_PLAYER_BLOOD( GetID(), 0 ) );


	LC_ActorBase::SetMP(iMP);
}
//--------------------------------------------------------
void LC_ClientGamePlayer::SetEXP( int64_t iEXP )
{
	//MG_PACKET_ENCODE_HANDLER hMsgHandler;
	//bool bCreateMsg = false;

	//int64_t lDeltaExp = iEXP - GetEXP();
	//if (lDeltaExp > 0 && m_lEXP >= 0)
	//{
	//	SendMessage( CreateMsg_UE_DELTA_EXP( GetID(),lDeltaExp) );
	//	SendLogicInfoMessage(RE_EXP_ADD, lDeltaExp);
	//}

	//LC_ActorBase::SetEXP(iEXP);
	m_lEXP = iEXP;

	/*if (lDeltaExp != 0)
	{
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_EXP_CHANGED);
		kGameEvent.SetEventParam(EP_EXP_CHANGED_EXP_VALUE, iEXP);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}*/
	//检查是否可以升级
	/*CF_LevelUp* pkCSVLevelUp = CF_LevelUp::GetSingletonPtr();
	int iMaxLevel = pkCSVLevelUp->GetEntryCount();
	if(m_lLevel < iMaxLevel)
	{
		const CF_LevelUp::DataEntry* pkData = pkCSVLevelUp->GetEntryPtr(m_lLevel + 1);
		if(pkData && m_lEXP >= pkData->_lEXPNeed)
		{
			MG_REQ_LevelUp reqMsg;
			reqMsg.m_lPlayerID = GetID();
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_LEVEL_UP, &reqMsg);
			if(bCreateMsg)
				m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
		}
	}*/
}
//--------------------------------------------------------
void LC_ClientGamePlayer::SetLevel(int lLevel)
{
	LC_ClientGameStoryManager* pkGameStoryManager = CLIENT_GET_GAMESTORY_MANAGER();
	bool bNeedSortGameStory = false;
	if(pkGameStoryManager&&lLevel!=GetLevel())
	{
		bNeedSortGameStory = true;
	}

	int iOldLevel = GetLevel();
	int64_t iOldExp = GetEXP();
	LC_PlayerBase::SetLevel(lLevel);
	if(bNeedSortGameStory)
	{
		pkGameStoryManager->AddGameStoryRevisionID(ACTIVITY_DIRTY_FLAG_UPDATE_LIST);
	}
}
//--------------------------------------------------
void LC_ClientGamePlayer::CreateVisibleData()
{
	T_ASSERT(this->GetCharType() > 0 && "error type");
	NiPoint3 kPos = UT_EngineMath::ConvertVec3IntToNiPoint3( m_kCurrentLocation );

	if ( UT_CutSceneManager::GetSingletonPtr() &&
		UT_CutSceneManager::GetSingleton().ShowGamePlayer() == false )
	{
		return;
	}

	GD_ClientActorModel* pkVisibleData = (GD_ClientActorModel*)
	 		IM_Manager::GetSingleton().CreateInteractPlayerModel(this->GetCharType(),GetID(),true,GetWeaponAttribute(),kPos, false);

	if(pkVisibleData)
	{
		//主角不能点击
		pkVisibleData->SetPickFlag(BS_IMInterface::PM_RIGHT);

		_initAreaEffectInfo(*pkVisibleData,false);

		if (m_bPendingToSetVisibleDataEnableFakeShadow)
		{
			pkVisibleData->EnableFakeShadow(true);
			m_bPendingToSetVisibleDataEnableFakeShadow = false;
		}

		float fParamList[4] = {0.0f};
		pkVisibleData->SetFresnelParameter(fParamList);
		pkVisibleData->SetFxAutoScale(m_fFxAutoScale);	

		pkVisibleData->ChangeClothing(GD_ClothingModel::CT_HEAD, "");
		pkVisibleData->ChangeClothing(GD_ClothingModel::CT_BODY, "");
		pkVisibleData->ChangeClothing(GD_ClothingModel::CT_FEET, "");
		pkVisibleData->ChangeBow(GD_WeaponModel::WST_RH, "");

		//pkVisibleData->m_bHideBeforeSuitApplied = true;
		//pkVisibleData->m_bAllClothingChanged = true;
		int eBindMode = !DBG_USE_3D_COORD_AXIS_GFX;
		pkVisibleData->AttachFxExt(GD_ClientActorModel::FDT_DYNAMIC_GROUND, eBindMode, false, RES_FX_CHOOSE_TARGET_YELLOW, NULL, NULL, -1.0f);
	}
	SetVisibleLocation(m_kCurrentLocation);

	//名字
	SendMessage( CreateMsg_UE_SHOW_PLAYER_NAME(GetID()) );
	SendMessage( CreateMsg_UE_SHOW_PLAYER_BLOOD(GetID()) );

	m_bIsCreateVisibleData = true;
}
//--------------------------------------------------
void LC_ClientGamePlayer::DestoryVisibleData()
{
	if ( UT_CutSceneManager::GetSingletonPtr() &&
		UT_CutSceneManager::GetSingleton().ShowGamePlayer() == false )
	{
		return;
	}

	IM_Manager::GetSingleton().DestroyInteractModel(GetID());
	m_bIsCreateVisibleData = false;

	DestroyBallVisibleData();

	DestroySuiteNPC();

	DestroyAnQi();

	SendMessage( CreateMsg_UE_HIDE_PLAYER_NAME(GetID()) );
	SendMessage( CreateMsg_UE_HIDE_PLAYER_BLOOD(GetID()) );
}
//--------------------------------------------------
void LC_ClientGamePlayer::CreateRiderVisibleData()
{
	if ( UT_CutSceneManager::GetSingletonPtr() &&
		UT_CutSceneManager::GetSingleton().ShowGamePlayer() == false )
	{
		return;
	}
	T_ASSERT(this->GetCharType() > 0 && "error type");

	object_id_type jc = GetCimeliaCharTypeID(LC_SUB_TP_VEHICLE,GetVehicleLevel());
	//修正模型高度
	CF_CharType::DataEntry* pkData = CF_CharType::GetSingletonPtr()->GetEntryPtr(jc);
	if(pkData)
	{
		m_iProperty_ShapeHeight = pkData->_iPickAreaHeight;
		SendLogicInfoMessage(RE_VEHICLE_SUMMON_SUCCESS, pkData->_kCharName);

		NiPoint3 kPos = UT_EngineMath::ConvertVec3IntToNiPoint3( m_kCurrentLocation );
		IM_Manager* pkInteractManager = IM_Manager::GetSingletonPtr();

		if(pkInteractManager)
		{
			GD_ClientActorModel* pkRiderVisibleData = (GD_ClientActorModel*)
				pkInteractManager->CreateInteractRiderModel(jc, GetID(), kPos);

			if(pkRiderVisibleData)
			{
				pkRiderVisibleData->SetPickFlag(BS_IMInterface::PM_NONE);
				pkRiderVisibleData->SetFxAutoScale(pkData->_fFxAutoScale);
				_initAreaEffectInfo(*pkRiderVisibleData, true);
			}
			SetVisibleLocation(m_kCurrentLocation);

//			这个函数是在SM_ClientGamePlayerController::_translateGamePlayer里面设置的，所以创建的时候并不设置
// 			GD_ClientActorModel* pkNetPlayer = IM_Manager::GetSingletonPtr()->GetActor(GetID());
// 			if ( pkNetPlayer )
// 				pkNetPlayer->SetVehicleModel(pkInteractManager);	

			LC_ClientNPCManager* pkmgr = CLIENT_GET_NPC_MANAGER(); 
			LC_ClientNPC* su = (LC_ClientNPC*)pkmgr->FindNPC(m_lastLCID);
			_updateSuitFollow(su);
		}
	}
}
//--------------------------------------------------
void LC_ClientGamePlayer::DestoryRiderVisibleData()
{
	if ( UT_CutSceneManager::GetSingletonPtr() &&
		UT_CutSceneManager::GetSingleton().ShowGamePlayer() == false )
	{
		return;
	}
	if (GetRiderCharType() > 0)
		SendLogicInfoMessage(RE_VEHICLE_DISMISS_SUCCESS);

	IM_Manager::GetSingletonPtr()->DestroyRiderInteractModel(GetID());
	m_lastVehicleCharType =0;

	//修正模型高度
	CF_Buddy::DataEntry* pkData = CF_Buddy::GetSingletonPtr()->GetEntryPtr(GetCharType());
	if(pkData)
	{
		m_iProperty_ShapeHeight = pkData->_iPickAreaHeight;	
	}

	GD_ClientActorModel* pkNetPlayer = IM_Manager::GetSingletonPtr()->GetActor(GetID());
	if ( pkNetPlayer )
		pkNetPlayer->SetVehicleModel(NULL);
}
//---------------------------------------------------------------------------
void LC_ClientGamePlayer::CreateBallVisibleData()
{
	int jd_id = m_lastJCCharType;

	NiPoint3 kPos = UT_EngineMath::ConvertVec3IntToNiPoint3( m_kCurrentLocation );
	IM_Manager* pkInteractManager = IM_Manager::GetSingletonPtr();

	if(pkInteractManager)
	{
		GD_ClientActorModel* pkRiderVisibleData = (GD_ClientActorModel*)
			pkInteractManager->CreateInteractFabaoModel(jd_id,GetID(),kPos);

		pkRiderVisibleData->SetPickFlag(BS_IMInterface::PM_NONE);
		pkRiderVisibleData->SetTranslate(kPos);
	}
}
//---------------------------------------------------------------------------
void LC_ClientGamePlayer::DestroyBallVisibleData()
{
	IM_Manager* pkInteractManager = IM_Manager::GetSingletonPtr();
	if(pkInteractManager)
	{
		pkInteractManager->DestroyFabaoInteractModel(GetID());
	}
}
//---------------------------------------------------------------------------
void LC_ClientGamePlayer::CreateAnQi()
{
	NiPoint3 kPos = UT_EngineMath::ConvertVec3IntToNiPoint3( m_kCurrentLocation );
	IM_Manager* pkInteractManager = IM_Manager::GetSingletonPtr();

	if(pkInteractManager)
	{
		GD_ClientActorModel* pkRiderVisibleData = (GD_ClientActorModel*)
			pkInteractManager->CreateInteractAnQiModel(m_lastAnQiCharType,GetID(),kPos);

		pkRiderVisibleData->SetPickFlag(BS_IMInterface::PM_NONE);
		pkRiderVisibleData->SetTranslate(kPos);
	}
}
void LC_ClientGamePlayer::DestroyAnQi()
{
	IM_Manager::GetSingletonPtr()->DestroyAnqiInteractModel(GetID());
}
//---------------------------------------------------------------------------
void LC_ClientGamePlayer::CreateSuiteNPC()
{	
	if ( m_lastLCCharType > 0)
	{
		LC_ClientNPCManager* pkmgr = CLIENT_GET_NPC_MANAGER(); 
		m_lastLCID = pkmgr->CreateEyeNPC(m_lastLCCharType, this, false);
		LC_ClientNPC* su = (LC_ClientNPC*)pkmgr->FindNPC(m_lastLCID);
		su->SetNextState(ST_NONE);
		su->SetSMController(SCT_AI);
		su->SetFollowedTargetID(GetID());
		_updateSuitFollow(su);
	}
}
//---------------------------------------------------------------------------
void LC_ClientGamePlayer::DestroySuiteNPC()
{
	LC_ClientNPCManager* pkmgr = CLIENT_GET_NPC_MANAGER(); 
	if ( IS_LOCAL_NPC_ID(m_lastLCID) )
	{
		pkmgr->DestroyLocalNPC(m_lastLCID);
		m_lastLCID = 0;
	}
}
//--------------------------------------------------------------------------
void LC_ClientGamePlayer::CreateSuitWeapon()
{
	GD_ClientActorModel* sd = (GD_ClientActorModel*)IM_Manager::GetSingleton().GetActor(m_lastLCID);
	if ( sd)
	{
		LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
		if ( pkItemFactory )
		{
			LC_ClientItem *pkItem = (LC_ClientItem*)pkItemFactory->RequestItem(m_lastLCWeaponType);
			if(pkItem)
			{
				pkItem->OperationAvatarFunc(MALE_TYPE, sd, NULL, GET_PROCESS_TIME);
			}
			else
			{
				sd->ChangeWeapon(GD_WeaponModel::WST_LRH, "", true);
			}
		}
	}
}
//---------------------------------------------------------------------------
void LC_ClientGamePlayer::ChangeVehicleAvatar(int lNewEquipID)
{
	IM_Manager* pkIMManager = IM_Manager::GetSingletonPtr();
	GD_ClientActorModel* pkVisibleData = pkIMManager->GetRiderActor(GetID());
	if( !pkVisibleData )
		return;

	if(lNewEquipID > 0)
	{
		LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
		LC_ClientItem*  pkItem = (LC_ClientItem* )pkItemFactory->RequestItem(lNewEquipID);
		if(!pkItem)
		{
			//GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"试图装备无效物件, %d", lNewEquipID);
			return ;
		}
		pkItem->OperationAvatarFunc(GetCharType(), pkVisibleData,NULL,GET_PROCESS_TIME);
	}
	else
	{
		pkVisibleData->ChangeExtraClothing("");
	}
}
//------------------------------------------------------------------------
void LC_ClientGamePlayer::ChangeVisibleData(int lCharTypeID)
{
	if ( UT_CutSceneManager::GetSingletonPtr() &&
		UT_CutSceneManager::GetSingleton().ShowGamePlayer() == false )
	{
		return;
	}

	if (IS_CHANGE_SKILL_NPC_TYPE_ID(lCharTypeID))
		return;

	IM_Manager* pkInteractManager = IM_Manager::GetSingletonPtr();
	if(pkInteractManager)
	{
		pkInteractManager->DestroyInteractModel(GetID());
		NiPoint3 kPos = UT_EngineMath::ConvertVec3IntToNiPoint3( m_kCurrentLocation );

		if (lCharTypeID == 0)
		{
			lCharTypeID = GetCurrentTransform(this->GetCharType());
		}

		SetTransformID(lCharTypeID);
		GD_ClientActorModel* pkVisibleData = (GD_ClientActorModel*)pkInteractManager->CreateInteractPlayerModel(lCharTypeID,GetID(),true,GetWeaponAttribute(),kPos);
		if(pkVisibleData)
		{
			//主角不能点击
			pkVisibleData->SetPickFlag(BS_IMInterface::PM_RIGHT);
			_initAreaEffectInfo(*pkVisibleData,false);
			{
				//设置爱好装备

				//pkVisibleData->ShowAfterSuitApplied();
			}
			pkVisibleData->SetFxAutoScale(m_fFxAutoScale);
		}
	}

	SetVisibleLocation(m_kCurrentLocation);

}

void LC_ClientGamePlayer::ShowAllBuddyVisibleData(bool bShow)
{
	IM_Manager* pkIMManager = IM_Manager::GetSingletonPtr();

	for (int i=0; i<m_vActiveBuddyList.size(); ++i)
	{
		const LC_PlayerBase* pActor = GetBuddyPlayerBasePtr(m_vActiveBuddyList[i].nObjectId);
		if (pActor)
		{
			GD_ActorModel* pkVisibleData = pkIMManager->GetActor(pActor->GetID());
			if(pkVisibleData)
			{
				pkVisibleData->SetVisibility(bShow);
				if (bShow)
				{
					SendMessage( CreateMsg_UE_SHOW_PLAYER_NAME(pActor->GetID()));
					SendMessage( CreateMsg_UE_SHOW_PLAYER_BLOOD(pActor->GetID()));
				}
				else
				{
					SendMessage( CreateMsg_UE_HIDE_PLAYER_NAME(pActor->GetID()));
					SendMessage( CreateMsg_UE_HIDE_PLAYER_BLOOD(pActor->GetID()));
				}
			}
		}
	}

	GD_ClientActorModel* pkVehicleVisibleData = pkIMManager->GetRiderActor(GetID());
	if(pkVehicleVisibleData)
		pkVehicleVisibleData->SetVisibility(bShow);

	if(m_lastLCID > 0)
	{
		GD_ActorModel* pkVisibleData = pkIMManager->GetActor(m_lastLCID);
		if(pkVisibleData)
		{
			pkVisibleData->SetVisibility(bShow);
			if (bShow)
			{
				SendMessage( CreateMsg_UE_SHOW_NPC_NAME(m_lastLCID));
				SendMessage( CreateMsg_UE_SHOW_TASK(m_lastLCID));
			}
			else
			{
				SendMessage( CreateMsg_UE_HIDE_NPC_NAME(m_lastLCID));
				SendMessage( CreateMsg_UE_HIDE_TASK(m_lastLCID));
			}
		}
	}

	if(m_iEyeNPCID > 0)
	{
		GD_ActorModel* pkVisibleData = pkIMManager->GetActor(m_iEyeNPCID);
		if(pkVisibleData)
			pkVisibleData->SetVisibility(bShow);
	}
}

//--------------------------------------------------------
void LC_ClientGamePlayer::PlayAnimation(const StringType& rkAnim,bool bForce /* = false */,float fSequency /* = 1.0f */,bool bAutoAdjustFrequency)
{
	IM_Manager* pkIMManager = IM_Manager::GetSingletonPtr();

	GD_ClientActorModel* pkVisibleData = pkIMManager->GetActor(GetID());

	if(pkVisibleData)
	{
		pkVisibleData->PlayAnimation(rkAnim,bForce,fSequency,bAutoAdjustFrequency);
	}
}
//--------------------------------------------------------
void LC_ClientGamePlayer::PlayWeaponAnimation(const StringType& rkAnim,bool bForce /* = false */,float fSequency /* = 1.0f */,bool bAutoAdjustFrequency)
{
	IM_Manager* pkIMManager = IM_Manager::GetSingletonPtr();

	GD_ClientActorModel* pkVisibleData = pkIMManager->GetActor(GetID());

	if(pkVisibleData && pkVisibleData->GetWeaponModel())
	{
		pkVisibleData->GetWeaponModel()->PlayAnimation(rkAnim,bForce,fSequency,bAutoAdjustFrequency);
	}
}
//--------------------------------------------------------
void LC_ClientGamePlayer::PlayRiderAnimation(const StringType& rkAnim,bool bForce /* = false */,float fSequency /* = 1.0f */,bool bAutoAdjustFrequency)
{
	if(GetRiderCharType() > 0)
	{
		IM_Manager* pkIMManager = IM_Manager::GetSingletonPtr();

		GD_ClientActorModel* pkVisibleData = pkIMManager->GetRiderActor(GetID());

		if(pkVisibleData)
		{
			pkVisibleData->PlayAnimation(rkAnim,bForce,fSequency,bAutoAdjustFrequency);
		}	
	}
}
//-------------------------------------------------------
void LC_ClientGamePlayer::PlayIdleAnimation()
{
	IM_Manager* pkIMManager = IM_Manager::GetSingletonPtr();

	if(GetRiderCharType() > 0)
	{
		if ( IsEquipVehicleWeapon() )
		{
			PlayAnimation(RES_ANM_ACTOR_RIDE_IDLE_HAND);
		}
		else if (IsArtifactWeapon() )
		{
			if ( IsArtifactWeaponIndex(GOD_ITEM_5) )
			{
				bool bAttackReady = m_kPKRecorder.GetFightState() && !GetSkillFlag(SK_DIZZ) && !GetSkillFlag(SK_SLEEP);
				if ( bAttackReady )
					PlayAnimation(RES_ANM_ACTOR_GODWEAPON5_IDLE_ATK);
				else
					PlayAnimation(RES_ANM_ACTOR_GODWEAPON5_IDLE);
			}
			else
				PlayAnimation(RES_ANM_ACTOR_GODWEAPON_IDLE);
		}
		else
		{
			PlayAnimation(RES_ANM_ACTOR_RIDE_IDLE_MENPAI);
		}
		GD_ClientActorModel* pkRiderVisibleData = pkIMManager->GetRiderActor(m_iID);
		if(pkRiderVisibleData)
		{
			pkRiderVisibleData->PlayRandomIdleAnimation();
		}
	}
	else
	{
		GD_ClientActorModel* pkVisibleData = pkIMManager->GetActor(GetID());
		if (pkVisibleData != NULL)
		{
			bool bAttackReady = m_kPKRecorder.GetFightState() && !GetSkillFlag(SK_DIZZ) && !GetSkillFlag(SK_SLEEP);
			if (bAttackReady)
			{
				/*if (GetShapeShiftFlag()&&!IS_CHANGE_SKILL_NPC_TYPE_ID(GetShapeShiftCharTypeID()))
					pkVisibleData->PlayIdleAnimationWithAttackReady();
				else if ( IsArtifactWeapon() )
				{
					if ( IsArtifactWeaponIndex(GOD_ITEM_4))
						pkVisibleData->PlayAnimation(RES_ANIM_ARTIFACTS4_IDLE_ATK);
					else if ( IsArtifactWeaponIndex(GOD_ITEM_5))
						pkVisibleData->PlayAnimation(RES_ANIM_ARTIFACTS5_IDLE_ATK);
					else
						pkVisibleData->PlayAnimation(RES_ANIM_ARTIFACTS_IDLE_ATK);
				}
				else
					pkVisibleData->PlayWeaponedIdleAnimationWithAttackReady(GetCurrentBandWeapon());*/
				pkVisibleData->PlayIdleAnimationWithAttackReady();
			}
			else
			{
				/*if (GetShapeShiftFlag()&&!IS_CHANGE_SKILL_NPC_TYPE_ID(GetShapeShiftCharTypeID()))
				{
					pkVisibleData->PlayRandomIdleAnimation();
				}
				else if ( IsArtifactWeapon() )
				{
					if( IsArtifactWeaponIndex(GOD_ITEM_4) )
						pkVisibleData->PlayAnimation(RES_ANIM_ARTIFACTS4_IDLE);
					else if ( IsArtifactWeaponIndex(GOD_ITEM_5) )
						pkVisibleData->PlayAnimation(RES_ANIM_ARTIFACTS5_IDLE);
					else
						pkVisibleData->PlayAnimation(RES_ANIM_ARTIFACTS_IDLE);
				}
				else
				{
					pkVisibleData->PlayWeaponedRandomIdleAnimation(GetCurrentBandWeapon());
				}*/
				pkVisibleData->PlayRandomIdleAnimation();
			}
		}
	}
}
//-------------------------------------------------------
void LC_ClientGamePlayer::PlayWingAnimation(const StringType& rkAnim,bool bForce /* = false */,float fSequency /* = 1.0f */,bool bAutoAdjustFrequency /* = false */)
{
	IM_Manager* pkIMManager = IM_Manager::GetSingletonPtr();

	GD_ClientActorModel* pkVisibleData = pkIMManager->GetActor(GetID());

	if(pkVisibleData && pkVisibleData->GetWingModel())
	{
		pkVisibleData->GetWingModel()->PlayAnimation(rkAnim,bForce,fSequency,bAutoAdjustFrequency);
	}
}
//-------------------------------------------------------
void LC_ClientGamePlayer::Update( float fCurrentTime,float fDeltaTime )
{	
#pragma TODO("处理打断")
	T_PROFILE_EX("Update::LC_ClientGamePlayer",4.0f);

	if(!m_pkMap)
		return;

	TestArrival();

	m_bEnablePathFinding = true;
	LC_ClientTaskMap* pkClientTaskMap = (LC_ClientTaskMap*)m_pkTaskMap;
#ifndef _MAP_VIEWER_BUILD
	//pkClientTaskMap->Update(fCurrentTime);

	/*if (m_sItemTipMessage != m_kTaskMap.GetItemTipMessage())
	{
	SendLogicInfoMessage(RE_TIP_MESSAGE,m_kTaskMap.GetItemTipMessage().c_str());
	}
	m_sItemTipMessage = m_kTaskMap.GetItemTipMessage();*/

#endif
	
	_notifyInGameToServer();
	_syncToServer(fCurrentTime);
	//_syncFovToServer(fCurrentTime);	//关闭fov查询，服务器应该会推送
	int32_t nOrigCurState = m_iCurrentState;
	int32_t nOrigNextState = m_iNextState;
	int32_t nOrigDefState = m_iDefaultState;
	int32_t nOrigPreState = m_iPreState;
	{
		//T_PROFILE("ClientGamePlayer::UpdateCD_Bullet" );
		LC_PlayerBase::Update(fCurrentTime, fDeltaTime);
	}
	//更新状态机
	m_pkSMController->Update(this, fCurrentTime, fDeltaTime);

	//更新快捷栏上技能、物品是否可用，不可用的slot灰化
	if (GetShortcutBar() != NULL)
		((LC_ClientShortcutBar*)GetShortcutBar())->UpdateShortcut(fCurrentTime, fDeltaTime);

	//这里这样做是由于服务器存储问题会在游戏开始时发生GP和NP交换的问题，但是当时NP还没有创建模型，所以会可能出现GP没有模型的问题
	GD_ClientActorModel* pkVisibleData = IM_Manager::GetSingleton().GetActor(GetID());
	if (pkVisibleData == NULL && !GetDead()) {
		CreateVisibleData();
	}
	else if (pkVisibleData != NULL)
	{
		pkVisibleData->SetProperty_AttachFxDisableCameraShake(false);	// 主角不关闭技能特效的震屏效果
	}

	_updateUnlockRequestExecuteSkill(fCurrentTime);
	_updateWatchedAndLockedTarget();
	_updateTargetSelectEffect();
	_updateNPCFunction();
	_updateLogicAreaInfo();
	_updateTradeState(fCurrentTime);
	_updateVehicleAvatar();
	if ( GetFashionBagAsset().GetDirtyFlag() )
	{
		this->UpdateSpecialEquipData();
		GetFashionBagAsset().SetDirtyFlag(false);
	}
	this->_updateWeaponAvatar();	//普通武器、骑战武器
	this->_updateWeaponVisibility();
	this->_updateBowAvatar(this->GetBowLevel());	//身上背弩炮
	this->_updateWingAvatar();
	this->_updateEyeAvatar();	//天眼
	_updateDisarmState();
	//只有在游戏场中中才更新任务相关数据
	if (UI_UIManager::GetSingleton().IsGameScreen())
	{
		_UpdateProcessingTask(fCurrentTime);
		_UpdateAutoStartTask(fCurrentTime);
	}
	_updateRevealInvisibility(fCurrentTime);
	_updateAutoUpgradeCimelia(fCurrentTime);
	//自动升级技能
	_updateAutoUpgradeSkill(fCurrentTime);
	//与服务器端不同，客户端只需要更新状态的时间，不更新具体的逻辑
	m_kSkillStateMap->UpdateStateTime(fCurrentTime, fDeltaTime);

	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (NULL != pkLogicManager)
	{
		//pkLogicManager->UpdatePlayerFightState(this);
	}

	m_pkFlopManager->Update(fCurrentTime);
	_UpdateTriggerInfo(fCurrentTime);

	
	if(pkVisibleData != NULL)
	{
		int fadeState = pkVisibleData->GetFadeState();
		if (GetSkillFlag(SK_WINDY))
		{
			pkVisibleData->SetAlpha(g_settings.DS_RESPAWN_ALPHA);
		}
		else if ( !GetFlashAlpha() && fadeState == GD_ModelBase::FIOS_INACTIVE)
		{
			if(GetSkillFlag(SK_INVISIBLE))
			{
				//pkVisibleData->IncrementalChangeAlpha(-2.0f * fDeltaTime, 0.5f);
				pkVisibleData->FadeOut(2.0f, 0.7f);
			}
			else
			{
				//pkVisibleData->IncrementalChangeAlpha(2.0f * fDeltaTime);
				pkVisibleData->FadeIn(2.0f, 1.0f);
			}
		}
	}
	

	if(m_pkScriptControllerFunctor)
		(*m_pkScriptControllerFunctor)();

	this->_updateTranscriptionSucceed(fCurrentTime, fDeltaTime);
	UpdateHook(fCurrentTime, fDeltaTime);
	_UpdateReqFriendStatus( fCurrentTime );

	/*static int si = 0;
	SendLogicInfoMessage(RE_TIP_MESSAGE,si++);*/


	//更新投票踢人数据
	LC_PlayerGroupBase* pkGroup = GetPlayerGroup();
	if (NULL != pkGroup)
	{
		pkGroup->UpdateGroup(fCurrentTime);
	}

	GetOpenBagAssert().UpdateRemainSeconds(GetServerTimeVal());

	if(m_eRespawnFadeState != RG_RenderManager::SGFIOS_INACTIVE)
	{
		m_fRespawnTimer -= fDeltaTime;
		if (m_fRespawnTimer < 0)
		{
			RG_RenderManager::GetSingletonPtr()->GreySceneFadeIn(0.5f);
			m_eRespawnFadeState = RG_RenderManager::SGFIOS_INACTIVE;
		}
	}

// 	_updateDirectionFx();
	_updateGameStoryDirectionFx();
	_updateExtraNodeData();

	//_updateJC(); // 隐藏法宝模式
	_updateLC(fCurrentTime, fDeltaTime);
	_updateLCWeapon();
	_updateAnQi();

	//更新角色语音的位置
	UpdateVoiceLocation();

	//道具拾取失败，暂存容器
	m_mapInvalidPickItemTick.tick(fDeltaTime);
	
	if ( m_mapInvalidPickItemTick.IsDown() || !m_mapInvalidPickItemTick.IsActivate() )
	{
		m_mapInvalidPickItem.clear();
		m_mapInvalidPickItemTick.Start(60.0f);
	}															

	//技能超时返回倒计时，因为现在客户端技能执行先释放
	m_skillRltDelayTimer.tick(fDeltaTime);

	m_kDoubleTerrienClearPath.tick(fDeltaTime);

	m_bTriggerTypeTick.tick(fDeltaTime);
	m_kReqTransferInterval.tick(fDeltaTime);
	if ( m_kReqTransferInterval.IsDown() )
	{
		SetAlreadyTransfer(false);
	}

	//操作缓存
	m_kGameOptCache.Update(fDeltaTime);

	//更新一下红名机制
	GetPKRecorder()->UpdatePkSateType(fCurrentTime);

	if ( GetPPAArgRetryID() > 0 )
	{
		CheckPendingTalkNPC(GetPPAArgRetryID(), fCurrentTime);
	}

	{
		//BUG #23002 【优化】【新手提示】玩家连续在线12小时、24小时、36小时等等；在线时间达到12小时倍数都会给予玩家刷新游戏提示，玩家点击后弹框才会消失
		uint32_t iGameTime = static_cast<uint32_t>(fCurrentTime);
		uint32_t iTargetTime = iGameTime / TIME_REMAINDER_INTERVAL;
		if (iTargetTime > 0 && (m_kTimeRemainState.empty() || m_kTimeRemainState.find(iTargetTime) == m_kTimeRemainState.end()))
		{
			SendLogicInfoMessage(RE_TIME_RAMAINER_REFRESH, Int2String(iTargetTime*TIME_REMAINDER_INTERVAL_HOUR).c_str());
			m_kTimeRemainState.insert(iTargetTime);
		}
	}

	//刷新SwapBuddy的计时器
	UpdateSwapCoolDown(fCurrentTime);
	//刷新Buddy复活CD时间
	UpdateRespawnCoolDown(fCurrentTime);

	//更新脚底光圈
	if (GetRiderCharType() > 0)
	{
		HideGroundFx();
	}
	else
	{
		ShowGroundFx();
	}
}
//--------------------------------------------------------
void	LC_ClientGamePlayer::FaceAt(const Utility::UT_Vec3Int& rkTargetDir)
{
	UT_Vec2Float kDirXY;
	kDirXY.x = (float)rkTargetDir.x;
	kDirXY.y = (float)rkTargetDir.y;
	float fDirLength = kDirXY.Unitize();

	if(fDirLength > 0.0f)
	{
		this->SetForwardDir(kDirXY);
		this->SetTargetForwardDir(kDirXY);
	}
}

void	LC_ClientGamePlayer::FaceToLocation(const Utility::UT_Vec3Int& rkTargetLocation)
{
	UT_Vec3Int kIntDir = rkTargetLocation- this->GetCurrentLocation();

	this->FaceAt(kIntDir);
}
//--------------------------------------------------------
void LC_ClientGamePlayer::ShowMessage(int lMessageType)
{
	SendLogicInfoMessage(lMessageType);
}

void LC_ClientGamePlayer::ShowNoticeMessage(int iMsgType, const StringType& sMsg, int iLevel)
{
	SendNotifyMessage(iMsgType, sMsg, iLevel);
}
//--------------------------------------------------------
void LC_ClientGamePlayer::NotifySkillEvent(Skill::SK_SkillEvent* pkSkillEvent)
{
	LC_PlayerBase::NotifySkillEvent(pkSkillEvent);
	m_pkSMController->NotifySkillEvent(this, pkSkillEvent);
}
//--------------------------------------------------------
void	LC_ClientGamePlayer::_onCurrentFightStateChanged(bool bFightState)
{
	if (bFightState)
	{
		//主玩家，进入战斗的提示特效
		//SetMessageToAuxiliaryTab();
		SendLogicInfoMessage(RE_PK_ENTER_FIGHT);
		//SendMessage(CreateMsg_UE_ENTER_FIGHT(this->GetID()));
	}
	else
	{
		//主玩家，退出战斗的提示特效
		//SetMessageToAuxiliaryTab();
		SendLogicInfoMessage(RE_PK_LEAVE_FIGHT);
		//SendMessage(CreateMsg_UE_LEAVE_FIGHT(this->GetID()));
	}

	//清除攻击者列表
	m_kLastAttackerID.clear();

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_CURRENT_FIGHTSTATE_CHANGED);
	kGameEvent.SetEventParam(EP_CURRENT_FIGHTSTATE_CHANGED_IN_FIGHT, (int)bFightState);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);

}

//--------------------------------------------------------
ResultType LC_ClientGamePlayer::CheckUseItemInPackByIndex(int iPackType, int iEntryIndex, int32_t& iCount, int iParam)
{
	return LC_PlayerBase::CheckUseItemInPackByIndex(iPackType, iEntryIndex, iCount, iParam);
}
//--------------------------------------------------------
ResultType LC_ClientGamePlayer::RequestUseItemInPackByIndex(int iPackType, int iEntryIndex, int32_t iCount, int iParam,const StringType &text, int iBuddyCID)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	int lItemID = 0;
	LC_BackPackEntry* pkEntry = GetPackAsset()->GetBackPackEntry(iPackType, iEntryIndex);
	if (pkEntry)
	{
		lItemID = pkEntry->GetTypeID();
	}

	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	LC_ItemBase* pkItem = pkItemFactory->RequestItem(lItemID);
	if(!pkItem)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"试图操作无效的物品[%i]",lItemID);
		return RE_ITEM_INVALID;
	}

	//默认传主控角色的CID
	if(iBuddyCID == 0)
	{
		iBuddyCID = GetBuddyActiveCID(GetCurrentActiveBuddyIndex());
	}
	//生命值满时不能使用药水
	//更换新的buddy血量判断
	int iCategory = pkItem->GetCategory();
	if(ITEM_CATEGORY_CONSUME_CAPACITY == iCategory || ITEM_CATEGORY_CONSUME_DRUGS == iCategory)
	{
		object_id_type buddyObjectID  = GetBuddyObjectID(iBuddyCID);
		LC_PlayerBase* pkBuddy = GetBuddyPlayerBasePtr(buddyObjectID);

		if(pkBuddy == NULL)
			return NULL;

		LC_ActorAttributeMap *buddyAttr = GetBuddyAttributeMap(iBuddyCID);
		int64_t buddyHP = pkBuddy->GetHP();
		int64_t iMaxHp = buddyAttr->GetAttribute(ATT_MAX_HP);
		if(buddyHP == iMaxHp)
		{
			//SendLogicInfoMessage(RE_CANNOT_USEITEM_HP_FULL);
			return RE_CANNOT_USEITEM_HP_FULL;
		}
	}

	ResultType iUseResult = LC_PlayerBase::CheckUseItemInPackByIndex(iPackType, iEntryIndex, iCount, iParam);
	if(iUseResult == RE_SUCCESS)
	{
		/*
		if (pkItem->GetDisassembleUseFuncID() > 0 )
		{
		if (GetPackAsset()->GetPackEmptyEntryCount(PACK_TYPE_TEMP) != TEMP_PACK_NUM)
		{
		SendLogicInfoMessage(RE_TEMP_PACK_NOT_EMPTY);
		return RE_TEMP_PACK_NOT_EMPTY;
		}
		}
		*/

		//卡片的添加走物品使用的流程，不走技能的流程
		if (IS_SKILL_TYPE(pkItem->GetUserTriggerSkillTypeID()))
		{
			if(m_kActionForbid.IsActionForbid(LAFT_ITEM,LAF_ITEM_SKILL_ITEM,lItemID))
			{
				SendLogicInfoMessage(RE_ACTION_FORBID);
				return RE_ACTION_FORBID;
			}

			/*iUseResult = pkItem->UseItem(this,pkEntry,GetLifeTime());*/
			int lSkillTypeID = pkItem->GetUserTriggerSkillTypeID();
			ResultType eSkillResult = CheckPendingSkill(pkItem->GetUserTriggerSkillTypeID());
			if(eSkillResult != RE_SKILL_NONE_ERROR)
			{
				if (eSkillResult != RE_SKILL_SYSTEM_ERROR)
				{
					SendLogicInfoMessage(eSkillResult);
				}
			}
			else
			{
				eSkillResult = WaitPendingSkill(lSkillTypeID,pkItem->GetItemTypeID());
			}

			if (IS_SKILL_BOOK_ID(lItemID))
			{
				LC_GameEvent kGameEvent;
				kGameEvent.SetEventType(GLET_LEARN_SKILL_START);

				UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
				pkUIManager->HandlerGameEvent(kGameEvent);
			}
		}
		else
		{
			if(m_kActionForbid.IsActionForbid(LAFT_ITEM,LAF_ITEM_IMMEDIATE_ITEM,lItemID))
			{
				SendLogicInfoMessage(RE_ACTION_FORBID);
				return RE_ACTION_FORBID;
			}

			//发送网络请求
			MG_REQ_UseItemInPackage reqMsg;
			reqMsg.m_lPlayerID = GetID();
			reqMsg.m_iPackType = iPackType;
			reqMsg.m_iPackageEntryIndex = iEntryIndex;
			reqMsg.m_lItemID = lItemID;
			reqMsg.m_lItemCount = iCount;
			reqMsg.m_iParam = iParam;
			reqMsg.m_nBuddyID = iBuddyCID;
			reqMsg.m_ext = jzUrlEncode(CodeConvert::MbsToUtf8(text).c_str());
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_USE_ITEM_IN_PACKAGE, &reqMsg);
			if(bCreateMsg)
				m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
		}

	}
	else
	{
		int iLevel = 0;
		if(iUseResult >= RE_ITEM_CANT_USE_VEHICLE_LEVEL_LIMIT && iUseResult < (RE_ITEM_CANT_USE_VEHICLE_LEVEL_LIMIT + LC_SUB_TP_END))
		{
			SendLogicInfoMessage(iUseResult, pkItem->GetSubordinateLimit(this, iUseResult - RE_ITEM_CANT_USE_VEHICLE_LEVEL_LIMIT));
		}
		else if(iUseResult == RE_ITEM_CANT_USE_VIPLEVEL_LIMIT)
		{
			SendLogicInfoMessage(RE_ITEM_CANT_USE_VIPLEVEL_LIMIT, pkItem->GetVipLevel(), 0, 0, 0, 0, MDT_DIALOG_CONFIRM_RECHARGE);
		}
		else
		{
			SendLogicInfoMessage(iUseResult);
		}
	}

	AddDirtyFlagRevisionID(DIRTY_FLAG_SHORTCUT);
	return iUseResult;
}

//--------------------------------------------------------
ResultType LC_ClientGamePlayer::RequestUseItemInPackByType(int lItemTypeID, int iParam, int cID, int iCount)
{
	ResultType iResult = CheckUseItemInPackByItemType(lItemTypeID);
	if (RE_SUCCESS != iResult)
	{
		//GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"试图操作不存在物品[%i]",lItemTypeID);
		return iResult;
	}
	
	int iPackType = GetWhichPackAutoPut(lItemTypeID);
	int iPackCapacity = m_kPackAsset.GetPackSize(iPackType);
	for (int i = 0; i < iPackCapacity; ++i)
	{
		LC_BackPackEntry* pkEntry = m_kPackAsset.GetBackPackEntry(iPackType, i);
		if (pkEntry->GetValid() && !pkEntry->GetLocked() && pkEntry->GetTypeID() == lItemTypeID)
		{
			ResultType r = RequestUseItemInPackByIndex(iPackType, i, UT_MathBase::MinInt(pkEntry->GetOverlapCount(), iCount), iParam,"", cID);
			if(RE_SUCCESS != r)
				return r;

			iCount -= pkEntry->GetOverlapCount();
			if(iCount <= 0)
				break;
		}
	}

	return RE_FAIL;
}
//--------------------------------------------------------
ResultType LC_ClientGamePlayer::RequestUsePatchItemInPackByIndex(int iPackType, int iEntryIndex, int32_t iCount, int iParam,const StringType &text, int iBuddyCID)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	int lItemID = 0;
	LC_BackPackEntry* pkEntry = GetPackAsset()->GetBackPackEntry(iPackType, iEntryIndex);
	if (pkEntry)
	{
		lItemID = pkEntry->GetTypeID();
	}

	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	LC_ItemBase* pkItem = pkItemFactory->RequestItem(lItemID);
	if(!pkItem)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"试图操作无效的物品[%i]",lItemID);
		return RE_ITEM_INVALID;
	}

	//默认传主控角色的CID
	if(iBuddyCID == 0)
	{
		iBuddyCID = GetBuddyActiveCID(GetCurrentActiveBuddyIndex());
	}

	LC_ItemDescriptionEntry* pkItemDesc = pkItem->GetItemDescriptionEntry();
	int costValue = UT_MathBase::MaxInt(pkItemDesc->GetUseMultiple(), 1);
	//这里只检查能否使用，不修正使用数量
	int curCount = iCount;
	ResultType iUseResult = LC_PlayerBase::CheckUseItemInPackByIndex(iPackType, iEntryIndex, curCount, iParam);
	//修正使用数量
	int useCount = UT_MathBase::MinInt(GetCanCostItemCount(lItemID)/costValue,iCount);

	if(iUseResult == RE_SUCCESS)
	{
		if(m_kActionForbid.IsActionForbid(LAFT_ITEM,LAF_ITEM_IMMEDIATE_ITEM,lItemID))
		{
			SendLogicInfoMessage(RE_ACTION_FORBID);
			return RE_ACTION_FORBID;
		}

		//发送网络请求
		MG_REQ_UseItemInPackage reqMsg;
		reqMsg.m_lPlayerID = GetID();
		reqMsg.m_iPackType = iPackType;
		reqMsg.m_iPackageEntryIndex = iEntryIndex;
		reqMsg.m_lItemID = lItemID;
		reqMsg.m_lItemCount = useCount;
		reqMsg.m_iParam = iParam;
		reqMsg.m_nBuddyID = iBuddyCID;
		reqMsg.m_ext = jzUrlEncode(CodeConvert::MbsToUtf8(text).c_str());
		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_USE_ITEM_IN_PACKAGE, &reqMsg);
		if(bCreateMsg)
			m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	}
	else
	{
		int iLevel = 0;
		if(iUseResult >= RE_ITEM_CANT_USE_VEHICLE_LEVEL_LIMIT && iUseResult < (RE_ITEM_CANT_USE_VEHICLE_LEVEL_LIMIT + LC_SUB_TP_END))
		{
			SendLogicInfoMessage(iUseResult, pkItem->GetSubordinateLimit(this, iUseResult - RE_ITEM_CANT_USE_VEHICLE_LEVEL_LIMIT));
		}
		else
		{
			SendLogicInfoMessage(iUseResult);
		}
	}

	AddDirtyFlagRevisionID(DIRTY_FLAG_SHORTCUT);
	return iUseResult;
}
//--------------------------------------------------------
void LC_ClientGamePlayer::RequestEquipItem(int iEntryIndex, int cID, int iCategory)
{
	//发送网络请求
	if(IsCommanderEquip(iCategory))
	{
		ReqEquipCommanderEquip(iEntryIndex);
	}
	else if(IsSubEquip(iCategory))
	{
		int iSubType = -1;
		if(IsVehicleEquip(iCategory))
		{
			iSubType = LC_SUB_TP_VEHICLE;
		}
		else if(IsDevaeyeEquip(iCategory))
		{
			iSubType = LC_SUB_TP_DEVA_EYE;
		}
		else if(IsWingEquip(iCategory))
		{
			iSubType = LC_SUB_TP_MUHUN;
		}
		else
		{
			T_ASSERT_CRITICAL_S(false);
		}

		ReqEquipSubEquip(iSubType, iEntryIndex);
	}
}
//--------------------------------------------------------
//---修改代码，全部修改成随身商店，这样代码改动量最小
//BUG #14231 【功能】【兑换商店】兑换商店客户端UI合入
ResultType LC_ClientGamePlayer::RequestBuyGoodsFromNPC(int iIndex, int iNumber, int iPackType, int iPackIndex, int NpcCharType, bool bSendLogicMessage)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	int lItemID = 0;
	int iShopID = -1;
	bool isPlayerShop = NpcCharType == LC_ShopBase::PLAYER_SHOP_NPC_CHARTYPE;
	LC_NPCShopManager* pkNPCManager = LC_NPCShopManager::GetSingletonPtr();
	if (pkNPCManager)
	{
		LC_NPCShop* const pkNPCShop = pkNPCManager->GetNPCShop(NpcCharType,LST_ITEM_HANDLE_SHOP);
		if (pkNPCShop)
		{
			LC_NPCShopItemEntry* const pkShopEntry = pkNPCShop->GetShopEntry(iIndex);
			if (pkShopEntry)
			{
				lItemID = pkShopEntry->GetTypeID();
				iShopID = pkShopEntry->GetShopID();
			}
		}
	}

	if (lItemID <= 0)
	{
		SendLogicInfoMessage(RE_ITEM_SYSTEM_ERROR);
		return RE_ITEM_SYSTEM_ERROR;
	}

	CF_NPCShopItems::DataEntry *pkNpcShopItem = SafeGetCSVFileDataEntryPtr<CF_NPCShopItems>(iShopID);

	if (pkNpcShopItem == NULL)
	{
		SendLogicInfoMessage(RE_ITEM_SYSTEM_ERROR);
		return RE_ITEM_SYSTEM_ERROR;
	}

	if (iPackIndex < 0)
		iPackType = GetWhichPackAutoPut(lItemID);


	ResultType iResult = LC_PlayerBase::CheckBuyGoodsFromNPC(pkNpcShopItem, iNumber,iPackType,iPackIndex,LC_ShopBase::PLAYER_SHOP_NPC_CHARTYPE);


	if(iResult == RE_SUCCESS)
	{
		//发送网络消息

		MG_REQ_NPCShopBuy msgBuy;
		msgBuy.m_iEntryId = iShopID;
		msgBuy.m_lItemID     = lItemID;
		if (isPlayerShop)	
			msgBuy.m_lNPCID	 = 0;	//发0，代表随身商店，和服务器约定一个的特殊ID给服务器
		else
			msgBuy.m_lNPCID  = NpcCharType;	//NPC商店，发NPCID给服务器

		msgBuy.m_lPlayerID   = GetID();
		msgBuy.m_iGoodNum    = iNumber;
		msgBuy.m_iPackType	 = iPackType;
		msgBuy.m_iPackIndex  = iPackIndex;
		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_NPCSHOP_BUY, &msgBuy);
		if(bCreateMsg)
			m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
	else if (bSendLogicMessage)
	{
		SendLogicInfoMessage(iResult);
	}
	return iResult;
}
//------------------------------------------------------------------------
ResultType LC_ClientGamePlayer::RequestBuyGoodsFromGuildNPC(int iShopEntryIndex, int iNumber, int iPackType, int iPackIndex)
{
	LC_NPCManagerBase* pkNPCManager = ENGINE_GET_NPC_MANAGER();
	LC_NPCShopManager* pkShopManager = LC_NPCShopManager::GetSingletonPtr();

	if (!IS_NPC_ID(m_lLockedNpcID))
	{
		return RE_ITEM_SYSTEM_ERROR;
	}

	LC_NPCBase* pkNPC = pkNPCManager->FindNPC(m_lLockedNpcID);
	if (NULL == pkNPC)
	{
		return RE_ITEM_SYSTEM_ERROR;
	}

	LC_NPCShop* pkShop = pkShopManager->GetNPCShop(pkNPC->GetCharType(), LST_GUILD_SHOP);
	if (NULL == pkShop)
	{
		return RE_ITEM_SYSTEM_ERROR;
	}

	LC_NPCShopItemEntry* pkShopEntry = pkShop->GetShopEntry(iShopEntryIndex);
	if (NULL == pkShopEntry || !IS_ITEM_ID(pkShopEntry->GetTypeID()))
	{
		return RE_ITEM_SYSTEM_ERROR;
	}
	//向帮派服务器提交协议
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	MG_REQ_BUY_FROM_GUILDNPC kReqMsg;
	kReqMsg.m_uiPlayerID = GetID();
	kReqMsg.m_uiLockNPCID = m_lLockedNpcID;
	kReqMsg.m_iGuildShopEntryIndex = iShopEntryIndex;
	kReqMsg.m_uiGoodID = pkShopEntry->GetTypeID();
	kReqMsg.m_iGoodCount = iNumber;
	kReqMsg.m_iPackType	= iPackType;
	kReqMsg.m_iPackIndex = iPackIndex;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_BUY_FROM_GUILDNPC, &kReqMsg);
	if (bCreateMsg)
	{
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
	return RE_SUCCESS;
}

//--------------------------------------------------------
void LC_ClientGamePlayer::RequestSaveCash2Guild( int iGold,int iSilver,int iCopper )
{
	MoneyType llCash = LC_Money::MoneyInfo2MoneyNumber(iGold, iSilver, iCopper);
	if (GetUnBindCash().GetMoney() < llCash)
	{
		SendLogicInfoMessage(RE_FAILED_DONATE_NO_ENOUGH_CASH);
		return ;
	}

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	Protocol::MG_REQ_DONATE kDonateMsg;
	kDonateMsg.m_llCash = llCash;

	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_DONATE, &kDonateMsg);
	if (bCreateMsg)
	{
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}

void LC_ClientGamePlayer::RequestRefineItemInPackage(int iEquipPackType, int iEquipIndex, int iItemID, int iAttribIndex)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_RefineItemInPackage reqMsg;
	reqMsg.m_lPlayerID = GetID();
	reqMsg.m_iEquipePackType = iEquipPackType;
	reqMsg.m_iEquipePackEntryIndex = iEquipIndex;
	reqMsg.m_lEquipeItemID = iItemID;
	reqMsg.m_iAttribIndex = iAttribIndex;

	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_ITEMREFINE, &reqMsg);
	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

void LC_ClientGamePlayer::RequestApplyRefineItem(int iEquipPackType, int iEquipIndex, int iItemID, int iUseNew)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_Common reqMsg;
	reqMsg.set("play_id", GetID());
	reqMsg.set("pack_type", iEquipPackType);
	reqMsg.set("pack_index", iEquipIndex);
	reqMsg.set("item_id", iItemID);
	reqMsg.set("use_new", iUseNew);

	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_ITEMREFINE_APPLY, &reqMsg);
	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

//------------------------------------------------------------------------
bool LC_ClientGamePlayer::RequestModifyName(const StringType& sName, int iSlotID)
{
	TStringVector kVector = UT_BaseStringUtility::SplitString(m_kCharName_gb, '.');
	if(kVector.size() >= 2 && kVector[1] == sName)
	{
		SendLogicInfoMessage(RE_LOGIN_CHARNAME_DUPLICATE_NAME);
		return false;
	}

	TT_TextFilter* pkTextFilter = TT_TextFilter::GetSingletonPtr();
	if (NULL != pkTextFilter)
	{
		pkTextFilter->SetText(sName);
		//如果找到非法字符
		if ( pkTextFilter->FindAndFilterForbiddenText(TT_TextFilter::TFT_CHAR_NAME,false,true) )
		{
			SendLogicInfoMessage(RE_LOGIN_CHARNAME_INVALID);
			return false;
		}
	}

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_PB<msg_define::change_name_req> reqMsg;
	reqMsg.m_value.set_pack_type(PACK_TYPE_BASIC);
	reqMsg.m_value.set_pack_index(iSlotID);
	reqMsg.m_value.set_new_name(sName.c_str());
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_CHANGE_CHAR_NAME, &reqMsg);
	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	return true;
}

ResultType LC_ClientGamePlayer::SaveCashToWarehouse(int iGold,int iSilver,int iCopper)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MoneyType llCash = LC_Money::MoneyInfo2MoneyNumber(iGold, iSilver, iCopper);


	ResultType iResult = LC_PlayerBase::SaveCashToWarehouse(llCash);
	if(iResult == RE_WAREHOUSE_SAVECASH_SUCCESS)
	{
		MG_REQ_SaveCashToWarehouse reqMsg;
		reqMsg.m_lPlayerID   = GetID();
		reqMsg.m_llCash = llCash;
		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SAVECASH_TO_WAREHOUSE, &reqMsg);

		if(bCreateMsg)
			m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
	else
	{
		SendLogicInfoMessage(iResult);
	}
	return iResult;
}
//------------------------------------------------------------------------
ResultType LC_ClientGamePlayer::GetCashFromWarehouse(int iGold,int iSilver,int iCopper)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MoneyType llCash = LC_Money::MoneyInfo2MoneyNumber(iGold, iSilver, iCopper);

	ResultType iResult = LC_PlayerBase::GetCashFromWarehouse(llCash);
	if(iResult == RE_WAREHOUSE_GETCASH_SUCCESS)
	{
		MG_REQ_GetCashFromWarehouse reqMsg;
		reqMsg.m_lPlayerID   = GetID();
		reqMsg.m_llCash = llCash;
		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GETCASH_FROM_WAREHOUSE, &reqMsg);

		if(bCreateMsg)
			m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
	else
	{
		SendLogicInfoMessage(iResult);
	}
	return iResult;
}
//------------------------------------------------------------------------
void LC_ClientGamePlayer::RestoreTarget()
{
	_restoreTarget();
}
//------------------------------------------------------------------------
ResultType  LC_ClientGamePlayer::MoveItemInPackage(int iSrcPackType, int iSrcIndex, int iDstPackType, int iDstIndex, int iNum)
{
	int lSrcItemID = 0;
	int lDesItemID = 0;
	int  iSrcItemNum = 0;
	int  iDstItemNum = 0;
	int  iMaxDstItemNum = 0;
	if (iSrcPackType == iDstPackType && iSrcIndex == iDstIndex)
	{
		return RE_FAIL;
	}
	//LC_BackPackEntry* pkSrcEntry = GetBackPack()->GetItemEntryInPackage(iSrcIndex);
	LC_BackPackEntry* pkSrcEntry = GetPackAsset()->GetBackPackEntry(iSrcPackType, iSrcIndex);
	if (pkSrcEntry&&pkSrcEntry->GetValid())
	{
		lSrcItemID = pkSrcEntry->GetTypeID();
		iSrcItemNum = pkSrcEntry->GetOverlapCount();
	}

	//LC_BackPackEntry* pkDesEntry = GetBackPack()->GetItemEntryInPackage(iDstIndex);
	LC_BackPackEntry *pkDesEntry = GetPackAsset()->GetBackPackEntry(iDstPackType, iDstIndex);
	if (pkDesEntry&&pkDesEntry->GetValid())
	{
		lDesItemID = pkDesEntry->GetTypeID();
		iDstItemNum = pkDesEntry->GetOverlapCount();

		LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
		LC_ItemBase* pkItem = pkItemFactory->RequestItem(lDesItemID);
		if(pkItem)
		{
			iMaxDstItemNum = pkItem->GetMaxOverlapCount();
		}
	}

	ResultType iResult = RE_FAIL;	
	if (iSrcItemNum == iNum)
	{
		if (lSrcItemID == lDesItemID && iDstItemNum < iMaxDstItemNum)
		{
			iResult = RequestMergeItemsInPackage(iSrcPackType, iSrcIndex, iDstPackType, iDstIndex);
		}
		else
		{
			iResult = RequestExchangeItemsInPackage(iSrcPackType, iSrcIndex, iDstPackType, iDstIndex);
		}
	}
	else
	{
		if (iSrcPackType == iDstPackType)
		{
			if (lSrcItemID == lDesItemID && iDstItemNum + iNum > iMaxDstItemNum)
			{
				iNum = iMaxDstItemNum - iDstItemNum;
			}

			iResult = RequestSplitItemInPackage(iSrcPackType, iSrcIndex, iDstIndex, iNum);
		}	
		else
		{
			SendLogicInfoMessage(RE_CAN_NOT_SPLIT_IN_DIFF_PACK);
		}
	}

	return iResult;
}
//------------------------------------------------------------------------
ResultType LC_ClientGamePlayer::RequestUnSocketItemInPackage(int iEquipPackType, int iEquipIndex, int lEquipItemID, int iSocketIndex)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_UnSocketItemInPackage reqMsg;
	reqMsg.m_lPlayerID = GetID();
	reqMsg.m_iEquipePackType = iEquipPackType;
	reqMsg.m_iEquipePackEntryIndex = iEquipIndex;
	reqMsg.m_lEquipeItemID = lEquipItemID;
	reqMsg.m_iSocketIndex = iSocketIndex;

	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_ITEMUNSOCKET, &reqMsg);
	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	return bCreateMsg;
}
//------------------------------------------------------------------------
ResultType LC_ClientGamePlayer::RequestSplitItemInPackage(int iPackType, int iSrcIndex, int iDesIndex, int iSplitNum )
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	ResultType iResult = LC_PlayerBase::CheckSplitItemInPackageToAssignEntry(iPackType, iSrcIndex, iDesIndex, iSplitNum);
	if (RE_SUCCESS == iResult)
	{
		MG_REQ_PACK_SplitItem	reqMsg;
		reqMsg.m_lPlayerID		= GetID();
		reqMsg.m_iPackType		= iPackType;
		reqMsg.m_iSrcIndex		= iSrcIndex;
		reqMsg.m_iTargetIndex	= iDesIndex;
		reqMsg.m_iSplitNum		= iSplitNum;

		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_PACK_SPLIT_ITEM, &reqMsg);
		if(bCreateMsg)
			m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
	else
	{
		SendLogicInfoMessage(iResult);
	}	

	return iResult;
}
//--------------------------------------------------------
ResultType LC_ClientGamePlayer::RequestUnbinItemInPackage(int iSrcPackType, int iSrcEntryIndex, int iDstPackType, int iDstEntryIndex)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	ResultType iResult = LC_PlayerBase::CheckUnbindItem(iSrcPackType, iSrcEntryIndex, iDstPackType, iDstEntryIndex);
	if(RE_SUCCESS == iResult)
	{
		MG_REQ_UnbindItem reqMsg;
		reqMsg.m_iSrcPackType = iSrcPackType;
		reqMsg.m_iSrcPackEntryIdx = iSrcEntryIndex;
		reqMsg.m_iDstPackType = iDstPackType;
		reqMsg.m_iDstPackEntryIdx = iDstEntryIndex;

		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_UNBIND_ITEM_IN_PACKAGE, &reqMsg);
		if(bCreateMsg)
			m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
	else
	{
		SendLogicInfoMessage(iResult);
	}

	return iResult;
}
//--------------------------------------------------------

void LC_ClientGamePlayer::RequestEnhanceEquipLevel(int iBuddyID,int iEquipIndex, const StringType & iConsumeItems)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;

	MG_REG_Equip_New_Reinforce reqMsg;
	reqMsg.m_iBuddyId = iBuddyID;
	reqMsg.m_iEquipIndex = iEquipIndex;

	TStringVector ids =  UT_BaseStringUtility::split(iConsumeItems, ';');
	for (TStringVector::iterator it = ids.begin(); it != ids.end(); ++it)
	{
		if ((*it).size() > 0)
		{
			int itemIdx = UT_BaseStringUtility::toInt(*it);
			reqMsg.m_ConsumeEquipIndexs.push_back(itemIdx);
		}
	}
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_EQUIP_NEW_REINFORCE, &reqMsg);
	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

void LC_ClientGamePlayer::RequestEnhanceEquipStar(int iBuddyID,int iEquipIndex)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;

	MG_REG_EquipSlot_Star reqMsg;
	reqMsg.m_iBuddyId = iBuddyID;
	reqMsg.m_iEquipSlotIndex = iEquipIndex;
	
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_EQUIPSLOT_STAR, &reqMsg);
	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

void LC_ClientGamePlayer::RequestReinforceEquipSlot(int iBuddyID,int iEquipIndex,bool bOneKey)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;

	MG_REG_Reinforce_EquipSlot reqMsg;
	reqMsg.m_iBuddyId = iBuddyID;
	reqMsg.m_iSlotID = iEquipIndex;
	reqMsg.m_iReinforeType=bOneKey?1:0;

	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_REINFORCE_EQUIPSLOT, &reqMsg);
	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

int64_t LC_ClientGamePlayer::GetCommanderAllEquipCombatScore()
{
	int64_t iScore = 0;
	EquipVec& equipList = GetCommander()->GetEquipList(CommandPack);
	for (size_t i = 0; i < equipList.size(); ++i)
	{
		LC_BackPackEntry& pkEntry = equipList[i];
		if(pkEntry.GetValid())
		{
			iScore += LC_Helper::GetEquipCombatScore(&pkEntry);
		}
	}

	return iScore;
}
//--------------------------------------------------------

int LC_ClientGamePlayer::GetOpenBagRemainSeconds()
{
	return GetOpenBagAssert().GetRemainSeconds();
}

ResultType LC_ClientGamePlayer::RequestMergeItemsInPackage(int iSrcPackType, int iSrcEntryIndex, int iDstPackType, int iDstEntryIndex)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	ResultType iResult = LC_PlayerBase::CheckMergeItemsInPackage(iSrcPackType, iSrcEntryIndex, iDstPackType, iDstEntryIndex);
	if (RE_SUCCESS == iResult)
	{
		int lItemID = 0;
		//LC_BackPackEntry* pkEntry = GetBackPack()->GetItemEntryInPackage(iSrcEntryIndex);
		LC_BackPackEntry* pkEntry = GetPackAsset()->GetBackPackEntry(iSrcPackType, iSrcEntryIndex);
		if (pkEntry&&pkEntry->GetValid())
		{
			lItemID = pkEntry->GetTypeID();
		}

		//发送网络请求
		MG_REQ_MergeItemInPackage reqMsg;
		reqMsg.m_lPlayerID		= GetID();
		reqMsg.m_iSrcPackType	= iSrcPackType;
		reqMsg.m_iSrcEntryIndex = iSrcEntryIndex;
		reqMsg.m_iDstPackType	= iDstPackType;
		reqMsg.m_iDstEntryIndex = iDstEntryIndex;
		reqMsg.m_lItemID		= lItemID;
		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_MERGE_ITEM_IN_PACKAGE, &reqMsg);

		if(bCreateMsg)
			m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
	else
	{
		SendLogicInfoMessage(iResult);
	}	

	return iResult;
}
//--------------------------------------------------------
ResultType LC_ClientGamePlayer::RequestExchangeItemsInPackage(int iSrcPackType, int iSrcEntryIndex,int iDstPackType, int iDstEntryIndex, bool bNeedEmpty)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	ResultType iResult = LC_PlayerBase::CheckExchangeItemsInPackage(iSrcPackType,iSrcEntryIndex,iDstPackType,iDstEntryIndex);
	if (RE_SUCCESS == iResult)
	{
		MG_REQ_PACK_SwitchItem reqMsg;
		reqMsg.m_lPlayerID = GetID();
		reqMsg.m_iSrcPackType = iSrcPackType;
		reqMsg.m_iSrcIndex = iSrcEntryIndex;
		reqMsg.m_iTargetPackType = iDstPackType;
		reqMsg.m_iTargetIndex = iDstEntryIndex;
		reqMsg.m_bNeedEmpty	= bNeedEmpty;

		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_PACK_SWITCH_ITEM, &reqMsg);
		if(bCreateMsg)
		{
			m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

			//GetPackAsset()->SwichItem(iSrcPackType, iSrcEntryIndex, iDstPackType, iDstEntryIndex);

			LC_BackPackEntry* pkEntry = NULL;
			//源格子
			pkEntry = GetPackAsset()->GetBackPackEntry(iSrcPackType, iSrcEntryIndex);
			if (pkEntry)
				pkEntry->SetLocked(true);
			//源格子
			pkEntry = GetPackAsset()->GetBackPackEntry(iDstPackType, iDstEntryIndex);
			if (pkEntry)
				pkEntry->SetLocked(true);
		}
	}
	else
	{
		SendLogicInfoMessage(iResult);
		return RE_FAIL;
	}

	return RE_ITEM_EXCHANGE_INPACKAGE_SUCCESS;
}
//--------------------------------------------------------
ResultType LC_ClientGamePlayer::RequestTidyItemsInPackage(int iPackType)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	ResultType iResult = LC_PlayerBase::CheckTidyItemsInPackage(iPackType);
	if (RE_SUCCESS == iResult)
	{
		MG_REQ_PACK_Tidy reqMsg;
		reqMsg.m_lPlayerID = GetID();
		reqMsg.m_iPackType = iPackType;
		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_TIDY_ITEM_IN_PACKAGE, &reqMsg);

		if(bCreateMsg)
			m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
	else	
	{
		SendLogicInfoMessage(iResult);
	}

	return RE_ITEM_TIDY_SUCCESS;
}
//------------------------------------------------------------------------
ResultType  LC_ClientGamePlayer::RequestDestroyItemInPackage(int iPackType, int iEntryIndex)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	ResultType iResult = LC_PlayerBase::CheckDestroyItemInPackage(iPackType, iEntryIndex);
	if (RE_SUCCESS == iResult)
	{	
		int lItemID = 0;	
		LC_BackPackEntry* pkEntry = GetPackAsset()->GetBackPackEntry(iPackType, iEntryIndex);
		if (pkEntry&&pkEntry->GetValid())
		{
			lItemID = pkEntry->GetTypeID();
		}

		MG_REQ_DestroyItemInPackage reqMsg;
		reqMsg.m_lPlayerID   = GetID();
		reqMsg.m_iPackType	 = iPackType;
		reqMsg.m_iEntryIndex = iEntryIndex;
		reqMsg.m_lItemID     = lItemID;
		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_DESTROY_ITEM_IN_PACKAGE, &reqMsg);

		if(bCreateMsg)
			m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
	else
	{
		SendLogicInfoMessage(iResult);
	}
	return iResult;
}
//------------------------------------------------------------------------
void LC_ClientGamePlayer::RequestEnhanceEquipmentInPackage(int iPackType, int iEntryIndex, int iLevel0, int iLevel1, int iLevel2, int iLevel3, int iLevel4, int iLevel5, int iLevel6, int iLevel7)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;

	int lItemID = 0;	
	LC_BackPackEntry* pkEntry = GetPackAsset()->GetBackPackEntry(iPackType, iEntryIndex);
	if (pkEntry && pkEntry->GetValid())
	{
		lItemID = pkEntry->GetTypeID();
	}

	MG_REQ_EnhanceItemInPackage reqMsg;
	reqMsg.m_lPlayerID = GetID();
	reqMsg.m_iPackType = iPackType;
	reqMsg.m_iPackageEntryIndex = iEntryIndex;
	reqMsg.m_lItemID = lItemID;

	if(iLevel0 >= 0)
	{
		EnhanceItemInfo eInfo0;
		eInfo0.m_AttribIndex = 0;
		eInfo0.m_AttribLevel = iLevel0;
		reqMsg.m_AttribEnhanceInfos.push_back(eInfo0);
	}

	if(iLevel1 >= 0)
	{
		EnhanceItemInfo eInfo1;
		eInfo1.m_AttribIndex = 1;
		eInfo1.m_AttribLevel = iLevel1;
		reqMsg.m_AttribEnhanceInfos.push_back(eInfo1);
	}

	if(iLevel2 >= 0)
	{
		EnhanceItemInfo eInfo2;
		eInfo2.m_AttribIndex = 2;
		eInfo2.m_AttribLevel = iLevel2;
		reqMsg.m_AttribEnhanceInfos.push_back(eInfo2);
	}

	if(iLevel3 >= 0)
	{
		EnhanceItemInfo eInfo3;
		eInfo3.m_AttribIndex = 3;
		eInfo3.m_AttribLevel = iLevel3;
		reqMsg.m_AttribEnhanceInfos.push_back(eInfo3);
	}

	if(iLevel4 >= 0)
	{
		EnhanceItemInfo eInfo4;
		eInfo4.m_AttribIndex = 4;
		eInfo4.m_AttribLevel = iLevel4;
		reqMsg.m_AttribEnhanceInfos.push_back(eInfo4);
	}

	if(iLevel5 >= 0)
	{
		EnhanceItemInfo eInfo5;
		eInfo5.m_AttribIndex = 5;
		eInfo5.m_AttribLevel = iLevel5;
		reqMsg.m_AttribEnhanceInfos.push_back(eInfo5);
	}

	if(iLevel6 >= 0)
	{
		EnhanceItemInfo eInfo6;
		eInfo6.m_AttribIndex = 6;
		eInfo6.m_AttribLevel = iLevel6;
		reqMsg.m_AttribEnhanceInfos.push_back(eInfo6);
	}

	if(iLevel7 >= 0)
	{
		EnhanceItemInfo eInfo7;
		eInfo7.m_AttribIndex = 7;
		eInfo7.m_AttribLevel = iLevel7;
		reqMsg.m_AttribEnhanceInfos.push_back(eInfo7);
	}

	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_ITEMENHANCE, &reqMsg);
	if(bCreateMsg)
	{
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}
int LC_ClientGamePlayer::GetCanCostItemCount(int iItemID, bool bLmtTime)
{
	LC_PackAsset *pkPackAsset = GetPackAsset();
	if(NULL == pkPackAsset)
		return 0;

	LC_BackPackEntry* pkEntry = pkPackAsset->GetBackPackEntry(PACK_TYPE_BASIC, 0);
	if (pkEntry == NULL)
		return 0;

	int iSize = pkPackAsset->GetPackSize(PACK_TYPE_BASIC); 
	time_t tTime = GetServerTimeVal();

	int iCount  = 0;
	for (int i = 0; i < iSize; ++i)
	{
		if (pkEntry->GetValid()
			&& pkEntry->GetTypeID() == iItemID)
		{
			iCount += pkEntry->GetOverlapCount();
		}
		pkEntry++;
	}

	return iCount;
}

int	LC_ClientGamePlayer::GetItemSlot(int iPackType,int iItemID)
{
	LC_PackAsset *pkPackAsset = GetPackAsset();
	if(NULL == pkPackAsset)
		return 0;
	LC_BackPackEntry* pkEntry = pkPackAsset->GetBackPackEntry(iPackType, 0);
	if (pkEntry == NULL)
		return -1;

	int iSize = pkPackAsset->GetPackSize(iPackType); 
	for (int i = 0; i < iSize; ++i)
	{
		if (pkEntry->GetValid()&& pkEntry->GetTypeID() == iItemID)
		{
			return i;
		}
		pkEntry++;
	}
	return -1;
}

void LC_ClientGamePlayer::RequestUpgradeMiji(int iID, int iPos)
{
	LC_SkillBookData* pkData = GetSkillBookAsset().GetSkillBookData(iID);
	if(!pkData)
		return ;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;

	MG_REQ_SkillBookUpgrade reqMsg;
	reqMsg.m_iID = iID;
	reqMsg.m_iPos = iPos;

	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SKILL_BOOK_UPGRADE, &reqMsg);
	if(bCreateMsg)
	{
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}

void LC_ClientGamePlayer::OnRltUpgradeMiji(ResultType result)
{
	//AddDirtyFlagRevisionID(DIRTY_FLAG_SKILL_BOOK);
	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_SKILLBOOK_UPGRADE_END);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, result);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

//------------------------------------------------------------------------
ResultType LC_ClientGamePlayer::CheckUpgrade(int iType, int iPayIndex)
{
	LC_SubUpgradeData* pkSub = GetSubordinateAsset().GetSubData(iType);
	if(!pkSub)
		return RE_FAIL;

	return pkSub->CheckUpgrade(this, iPayIndex, false);
}

int LC_ClientGamePlayer::GetItemListValueCount(int iType)
{
	LC_SubUpgradeData* pkSub = GetSubordinateAsset().GetSubData(iType);
	if(pkSub)
	{
		int now_lelvel = pkSub->GetStage();

		const LC_SubordinateDataEntry* entry = SafeGetSubDataEntry(iType, now_lelvel);
		if (NULL == entry)
		{
			return 0;
		}


		int iResult = 0;
		UT_SIMDataList::const_iterator cit = entry->_kCostItems.begin();
		for (; cit != entry->_kCostItems.end(); ++cit)
		{
			iResult += GetCanCostItemCount(cit->ID());
		}

		return iResult;
	}

	return 0;
}

int LC_ClientGamePlayer::GetCimeliaNeedLevel(int iType)
{
	LC_SubUpgradeData* pkSub = GetSubordinateAsset().GetSubData(iType);
	if(pkSub != NULL)
	{
		const LC_SubordinateDataEntry* pkEntry = SafeGetSubDataEntry(iType, pkSub->GetStage());
		if(pkEntry != NULL)
		{
			return pkEntry->_controllerLevel;
		}
	}

	return 0;
}

void LC_ClientGamePlayer::RequestUpgradeCimelia(int iType)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	
	MG_REQ_UpgraedeSubordinate reqMsg;
	reqMsg.m_lPlayerID = GetID();
	reqMsg.m_iSubType = iType;
	reqMsg.m_iPayIndex = PAY_INDEX_ITEM;

	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_UPGRADE_SUBORDINATE, &reqMsg);
	if(bCreateMsg)
	{
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}

bool LC_ClientGamePlayer::_shouldAutoUpgradeContinue(int iType, int iLevel, ResultType result)
{
	return result == RE_SUCCESS;
}

void LC_ClientGamePlayer::OnRltUpgradeCimelia(ResultType result, int iType, int iRank, int iPayIndex, const StringType& sModifyAttribs)
{
	//进阶非正常失败时终止自动进阶
	bool bUpgrading = GetIsAutoUpgradeCimelia(iType);
	bool bNormalFail = _shouldAutoUpgradeContinue(iType, iRank, result);
	if(bUpgrading)
	{
		if(!bNormalFail || (result == RE_UPGRAD_SUB_FAIL_ITEM && iPayIndex == 0))
		{
			SetIsAutoUpgradeCimelia(iType, false);
		}
	}

	//可能已经改变需要重新获取
	bUpgrading = GetIsAutoUpgradeCimelia(iType);
	SetIsDoAutoUpgradeCimelia(iType, bUpgrading && bNormalFail);

	//提示信息
	switch(result)
	{
	case RE_UPGRAD_SUB_SUCCESS:
		{	
			if (iType == LC_SUB_TP_TALISMAN)
			{
				SendLogicInfoMessage(RE_UPGRAD_TALISMAN_SUCCESS);
			}
			else if(IS_VALID_SUBORDINATE_ARTIFACT_TYPE(iType) || IS_VALID_SUBORDINATE_SUB_TYPE(iType))
			{
				//此时rank已是进阶后的等级
				StringType sInfo = GetCimeliaDisPlayLevelInfo(iType, iRank - 1);
				TStringVector vector = UT_BaseStringUtility::SplitString(sInfo, ';');
				if(vector.size() >= 3)
				{
					if(atoi(vector[1].c_str()) < atoi(vector[2].c_str()))
					{
						SendLogicInfoMessage(RE_UPGRAD_SUB_STAR_SUCCESS);
					}
					else
					{
						SendLogicInfoMessage(RE_UPGRAD_SUB_SUCCESS);
					}
				}
				else
				{
					SendLogicInfoMessage(RE_UPGRAD_SUB_SUCCESS);
				}
			}
			/*else
			{
				SendLogicInfoMessage(RE_UPGRAD_SUB_SUCCESS);
			}*/
			/*CF_CharType::DataEntry* pkData = CF_CharType::GetSingletonPtr()->GetEntryPtr(SUB_TP_CHARID[iType] + rltMsg.m_iLevel - 1);
			if (pkData != NULL)
			{
				T_ASSERT_CRITICAL_S(rltMsg.m_iLevel <= 20);
				SendLogicInfoMessage(RE_UPGRAD_SUB_SUCCESS, SUB_TP_NAME[iType], NUM_TO_CHN[rltMsg.m_iLevel], pkData->_kCharName);
			}*/

			g_soundMgr.Play2DEx(VARIABLE2STRING(SE_Sys_ItemUpgrade_OK));
		}
		break;
	case RE_UPGRAD_SUB_FAIL_MAX_LEVEL_LIMIT:
		{
			if(iType < LC_SUB_TP_END)
				SendLogicInfoMessage(RE_UPGRAD_SUB_FAIL_MAX_LEVEL_LIMIT, SUB_TP_NAME(iType));
		}		
		break;
	case RE_UPGRAD_SUB_FAIL_ITEM:
		{
			int iItemID = 0;
			const LC_SubordinateDataEntry* pkEntry = SafeGetSubDataEntry(iType, iRank);
			if(NULL != pkEntry)
			{
				iItemID = pkEntry->_iItemTypeID;
			}

			if(iItemID > 0 )
			{
				CF_ItemList::DataEntry *pkItemEntry = SafeGetCSVFileDataEntryPtr<CF_ItemList>(iItemID);
				if(pkItemEntry)
				{
					SendLogicInfoMessage(RE_UPGRAD_SUB_FAIL_ITEM, LC_Helper::GetGradeName(pkItemEntry->_kName, pkItemEntry->_lGrade));
				}
			}
		}
		
		break;
	case RE_UPGRAD_SUB_FAIL_RP:
		{
			/*int iNeedPoint = 0;
			if(iType == LC_SUB_TP_VEHICLE)
			{
				CF_Vehicle::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_Vehicle>(iRank + 1);
				if(pkEntry)
					iNeedPoint = pkEntry->_NeedBlessPoint;
			}
			else if(iType == LC_SUB_TP_ARROW)
			{
				CF_Arrow::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_Arrow>(iRank + 1);
				if(pkEntry)
					iNeedPoint = pkEntry->_NeedBlessPoint;
			}
			else if(iType == LC_SUB_TP_DEVA_EYE)
			{
				CF_DevaEye::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_DevaEye>(iRank + 1);
				if(pkEntry)
					iNeedPoint = pkEntry->_NeedBlessPoint;
			}
			else if(iType == LC_SUB_TP_VEHICLE_FIGHTING)
			{
				CF_VehicleFighting::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_VehicleFighting>(iRank + 1);
				if(pkEntry)
					iNeedPoint = pkEntry->_NeedBlessPoint;
			}

			SendLogicInfoMessage(RE_UPGRAD_SUB_FAIL_RP, iBlessPntDelta, iNeedPoint-iBlessPntDelta-GetCimeliaBlessPoint(iType));*/
			//g_soundMgr.Play2DEx(VARIABLE2STRING(SE_Sys_ItemUpgrade_NG));
		}
		break;
	case RE_CONTROLLER_LEVEL_NOT_ENOUGH_SUB:
		{
			const LC_SubordinateDataEntry* pkEntry = SafeGetSubDataEntry(iType, iRank);
			if(pkEntry != NULL)
			{
				SendLogicInfoMessage(RE_CONTROLLER_LEVEL_NOT_ENOUGH_SUB, pkEntry->_controllerLevel);
			}
		}
		break;
	default:
		//SendLogicInfoMessage(result);
		break;
	}


	LC_SubUpgradeData *pkData = GetSubordinateAsset().GetSubData(iType);
	if(NULL != pkData)
	{
		if(RE_UPGRAD_SUB_SUCCESS == result && iType == LC_SUB_TP_TALISMAN)
		{
			SyncSubordinateNormalTransform(iType);
		}

		AddDirtyFlagRevisionID(DIRTY_FLAG_SUBORDINATE_INFO);
	}

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_UPGRADE_CIMELIA_END);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, result);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, iType);
	//kGameEvent.SetEventParam(EVENT_BASE_PARAM_3, iBlessPntDelta);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_4, iRank);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);

	if(result == RE_SUCCESS)
	{
		LC_ClientGameEvent kCGameEvent;
		kCGameEvent.SetClientEventType(CGET_UPGRADE_CIMELIA_MODIFY_ATTRIBS);

		LC_ClientGameEventUserData_String kUserData(sModifyAttribs);
		kCGameEvent.SetUserData(&kUserData);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kCGameEvent);
	}
}

void LC_ClientGamePlayer::RequestChangeTransform(int iType, int iOpCode, int iSetLevel)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;

	MG_REQ_SubordinateOperation reqMsg;
	reqMsg.m_lPlayerID = GetID();
	reqMsg.m_iSubType = iType;
	reqMsg.m_iOPCode = iOpCode;
	reqMsg.m_iOPParam = iSetLevel;


	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SUBORDINATE_OPERATION, &reqMsg);
	if(bCreateMsg)
	{
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}

}

void LC_ClientGamePlayer::RequestBless(int iType, int iOpCode, int iSetLevel)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;

	MG_REQ_SubordinateOperation reqMsg;
	reqMsg.m_lPlayerID = GetID();
	reqMsg.m_iSubType = iType;
	reqMsg.m_iOPCode = iOpCode;
	reqMsg.m_iOPParam = iSetLevel;


	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SUBORDINATE_OPERATION, &reqMsg);
	if(bCreateMsg)
	{
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}

void LC_ClientGamePlayer::RequestTransformCimelia(int iType, int iOpCode, int iTransformID)
{
	LC_SubUpgradeData *pkData = GetSubordinateAsset().GetSubData(iType);
	if(pkData == NULL)
		return;

	RequestChangeTransform(iType, iOpCode, iTransformID);
}

void LC_ClientGamePlayer::OnRltChangeTransform(ResultType result, int iType, int iCode, int iParam)
{
	if (RE_SUCCESS == result)
	{
		LC_SubUpgradeData *pkData = GetSubordinateAsset().GetSubData(iType);
		if(NULL != pkData && iCode == LC_SUB_OP_SET_TRANSFROM)
		{
			pkData->SetSetTransform(iParam);
			AddDirtyFlagRevisionID(DIRTY_FLAG_SUBORDINATE_INFO);
		}
	}

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_SUBORDINATE_OPERATION);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, result);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, iType);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_3, iCode);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_4, iParam);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void LC_ClientGamePlayer::RequestAutoUpgradeCimelia(bool bStart, int iType)
{
	if(iType < LC_SUB_TP_BEGIN)
		return;

	if(iType < LC_SUB_TP_END)
	{
		if(bStart)
		{
			m_fLastAutoUpgradeCimeliaTime[iType] = 0.0f;
			
			RequestUpgradeCimelia(iType);
		}
		else
		{
			m_bDoAutoUpgradeCimelia[iType] = bStart;
		}
		m_bAutoUpgradeCimelia[iType] = bStart;
	}
}
//------------------------------------------------------------------------
ResultType	LC_ClientGamePlayer::RequestUseItemTriggerTask(int iPackType, int iEntryIndex)
{

	LC_BackPackEntry* pkEntry = GetPackAsset()->GetBackPackEntry(iPackType, iEntryIndex);
	if (pkEntry&&pkEntry->GetValid())
	{
		int lItemID = pkEntry->GetTypeID();
		LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
		LC_ItemBase* pkItem = pkItemFactory->RequestItem(lItemID);
		if(pkItem)
		{
			LC_ItemDescriptionEntry* pkEntry = pkItem->GetItemDescriptionEntry();
			if (pkEntry->GetBackPackRCFunc() == BAGUI_RCLICK_ACCEPTTASK)
			{
				((LC_ClientTaskMap*)m_pkTaskMap)->AddRequestShareTask(pkItem->GetUserTriggerSkillPara1(),0);
				return RE_SUCCESS;
			}
		}
	}

	SendLogicInfoMessage(RE_ITEM_SYSTEM_ERROR);
	return RE_ITEM_SYSTEM_ERROR;
}
//------------------------------------------------------------------------
ResultType LC_ClientGamePlayer::CheckPutIntoWarehouse(int& iTargetIndex, int iTargetPackType)
{	
	//if ( GetNPCFunctionType() != NFT_WAREHOUSE )
	//{
	//	return RE_WAREHOUSE_PUTINTO_FAIL_TOO_FAR;
	//}

	if ( !IS_WAREHOUSE_PACK(iTargetPackType))
	{
		return RE_WAREHOUSE_PUTINTO_FAIL_IS_FULL;
	}

	// 查找一个空的位置
	int iWarehouseSize = GetPackAsset()->GetPackSize(iTargetPackType);

	iTargetIndex = -1;
	for (int i = 0; i < iWarehouseSize; ++i)
	{
		LC_BackPackEntry* pkEntryTemp = GetPackAsset()->GetBackPackEntry(iTargetPackType, i);
		if ( pkEntryTemp && !(pkEntryTemp->GetValid()) && !pkEntryTemp->GetLocked())
		{
			iTargetIndex = i;
			pkEntryTemp->SetLocked(true);
			break;
		}
	}

	if (iTargetIndex < 0)
	{
		return RE_WAREHOUSE_PUTINTO_FAIL_IS_FULL;	// 仓库已满
	}

	return RE_WAREHOUSE_PUTINTO_SUCESS;
}
//------------------------------------------------------------------------
void LC_ClientGamePlayer::RequestPutIntoWarehouse(int iPackType, int iEntryIndex, int iTargetPackType)
{	
	LC_BackPackEntry* pkEntry = GetPackAsset()->GetBackPackEntry(iPackType, iEntryIndex);
	if ( pkEntry && pkEntry->GetValid() )
	{
		int iTargetIndex = -1;

		ResultType iResultType = CheckPutIntoWarehouse(iTargetIndex,iTargetPackType);
		if (iResultType == RE_WAREHOUSE_PUTINTO_SUCESS)
		{
			iResultType = RequestExchangeItemsInPackage(iPackType, iEntryIndex, iTargetPackType, iTargetIndex, true);
		}
		else
		{
			SendLogicInfoMessage(iResultType);
		}
	}
}
//------------------------------------------------------------------------
ResultType LC_ClientGamePlayer::CheckTakeOutFromWarehouse(int iSrcIndex, int iSrcPackType,int& iPackType, int& iTargetIndex)
{
	//if ( GetNPCFunctionType() != NFT_WAREHOUSE )
	//{
	//	return RE_WAREHOUSE_TAKEOUT_FAIL_TOO_FAR;
	//}

	if (!IS_WAREHOUSE_PACK(iSrcPackType))
	{
		return RE_ITEM_PACKAGE_FULL;
	}

	ResultType iResult = _checkTakeItemToPackbag(iSrcIndex, iSrcPackType, iPackType, iTargetIndex);
	if(iResult != RE_SUCCESS)
		return iResult;

	return RE_WAREHOUSE_TAKEOUT_SUCESS;
}
//------------------------------------------------------------------------
void LC_ClientGamePlayer::RequestTakeOutFromWarehouse(int iSrcIndex, int iSrcPackType)
{
	int iPackType = PACK_TYPE_BASIC;
	int iTargetIndex = -1;
	ResultType iResultType = CheckTakeOutFromWarehouse(iSrcIndex, iSrcPackType,iPackType, iTargetIndex);
	if (RE_WAREHOUSE_TAKEOUT_SUCESS == iResultType)
	{
		RequestExchangeItemsInPackage(iSrcPackType, iSrcIndex, iPackType, iTargetIndex, true);
	}
	else
	{
		SendLogicInfoMessage(iResultType);
	}
}

//--------------------------------------------------------
void LC_ClientGamePlayer::RequestTakeOutFromTreasureBag(int packType, int iSrcIndex)
{
	int iSrcPackType = packType;
	int iPackType = PACK_TYPE_BASIC;
	int iTargetIndex = -1;
	ResultType iResultType = _checkTakeItemToPackbag(iSrcIndex, iSrcPackType, iPackType, iTargetIndex);
	if (iResultType == RE_SUCCESS)
	{
		RequestExchangeItemsInPackage(iSrcPackType, iSrcIndex, iPackType, iTargetIndex, true);
	}
	else
	{
		SendLogicInfoMessage(iResultType);
	}
}
//--------------------------------------------------------
ResultType LC_ClientGamePlayer::_checkTakeItemToPackbag(int iSrcIndex, int iSrcPackType,int& iPackType, int& iTargetIndex)
{
	LC_BackPackEntry* pkEntry = GetPackAsset()->GetBackPackEntry(iSrcPackType, iSrcIndex);
	if ( pkEntry && pkEntry->GetValid() && !pkEntry->GetLocked())
	{
		//获取其目标背包
		iPackType = GetWhichPackAutoPut( pkEntry->GetTypeID() );

		bool bFull = GetPackAsset()->IsPackFull(iPackType);
		if (bFull)
		{
			return RE_ITEM_PACKAGE_FULL;
		}		
	}
	else
		return RE_ITEM_SYSTEM_ERROR;

	int iWarehouseSize = GetPackAsset()->GetPackSize(iPackType);
	iTargetIndex = -1;
	for (int i = 0; i < iWarehouseSize; ++i)
	{
		LC_BackPackEntry* pkEntryTemp = GetPackAsset()->GetBackPackEntry(iPackType, i);
		if ( pkEntryTemp && !(pkEntryTemp->GetValid()) && !pkEntryTemp->GetLocked())
		{
			iTargetIndex = i;
			pkEntryTemp->SetLocked(true);
			break;
		}
	}

	if (iTargetIndex < 0)
	{
		return RE_ITEM_PACKAGE_FULL;	// 包裹已满
	}

	return RE_SUCCESS;
}
//--------------------------------------------------------
ResultType LC_ClientGamePlayer::PendingSkillInSkillAsset(int lSkillTypeID)		//技能栏技能触发入口
{
	//if (!m_kSkillAsset.IsLearnedSkill(lSkillTypeID, true)
	//	&& !IsTransformAttackSkill(lSkillTypeID)
	//	&& LC_Helper::GetEmptyHandNormalAttackSkill(this) != lSkillTypeID
	//	&& 21000001 != lSkillTypeID)	//为了兼容拆分武器普通攻击前创建的角色
	//{
	//	return RE_SKILL_IS_NO_EXIST;
	//}

	static float sfCurrent = 0.00f;
	float fCurrent = GET_PROCESS_TIME;
	if ( fCurrent == sfCurrent )
		return RE_SKILL_PENDING_SUCCESS;

	sfCurrent = fCurrent;


	SK_Factory* pkSkillFactory = SK_Factory::GetSingletonPtr();
	SK_SkillExecute* pkSkillExecute = pkSkillFactory->RequestSkillExcute(lSkillTypeID);
	if (NULL == pkSkillExecute)
	{
		return RE_SKILL_SYSTEM_ERROR;
	}

	//现在释放技能的时候，只根据当前锁定的情况，不在再根据watch的目标重新设置锁定目标
// 	object_id_type lWatchTarget = GetWatchTargetID();
// 	object_id_type lLockTarget  = GetLockedTargetID();
// 	if(lWatchTarget != lLockTarget && (IS_PLAYER_CLASS_ID(lWatchTarget) || IS_NPC_ID(lWatchTarget)))	//将查看目标设置到锁定目标上
// 	{
// 		ClearPendingSkill();
// 		SetLockedTargetID(GetWatchTargetID());
// 	}

	//把预锁定目标当作锁定目标
	if(GetPreLockedTargetID() != 0)
	{
		SetLockedTargetID(GetPreLockedTargetID());
	}
	//现在又不需要自动选择目标了
// 	else
// 	{
// 		//不存在预锁定目标，也就不会锁定了，则选择与自己周围最近的目标
// 		LC_MapBase* pkMap = this->GetMap();
// 		if(pkMap)
// 		{
// 			CF_ActorFilterFunc* pkCSVActorFilterFunc = CF_ActorFilterFunc::GetSingletonPtr();
// 			CF_ActorFilterFunc::DataEntry* pkData = NULL;
// 			//排除当前锁定目标和自身
// 			pkData = pkCSVActorFilterFunc->GetEntryPtr(2);
// 
// 			if (pkData)
// 			{
// 				//从地图返回区域内的Actor队列
// 				UT_Circle kCircle;
// 				kCircle.iRadius = CHANGE_LOCKED_DISTANCE;
// 				kCircle.kCenterPoint = GetCurrentLocation();
// 				LC_ActorPtrList kLockResult = pkMap->LockActorsInCircle(kCircle,pkData);
// 
// 				//根据距离对目标进行排序
// 				UT_Vec3Int kPlayerLocation = GetCurrentLocation();
// 				kLockResult.sort(lessThan(kPlayerLocation, m_kLastAttackerID));
// 
// 				//根据过滤设置,过滤Actor队列
// 				object_id_vector kFilterResult = LC_Helper::FilterActors(this,kLockResult,pkData);
// 
// 				if(kFilterResult.size() != 0)
// 				{
// 					SetPreLockedTargetID(kFilterResult.front());
// 					SetLockedTargetID(GetPreLockedTargetID());
// 				}
// 			}
// 		}
// 	}

	ResultType e =  RE_SKILL_SYSTEM_ERROR;
	//方向型瞬发技能
	if (SOT_DIR == pkSkillExecute->GetProperty_OperationType() && pkSkillExecute->GetProperty_ExecuteImmediately())
	{
		RequestExecuteSkill(GET_PROCESS_TIME, pkSkillExecute, 0);
		e = RE_SKILL_PENDING_SUCCESS;
	}
	else
	{
		e = WaitPendingSkill(lSkillTypeID,0);
	}

	if (e == RE_SKILL_PENDING_SUCCESS)
	{
		//LC_LogicObject* pkTarget = this->GetMap()->FindObject(lLockTarget);

		this->OnPendingSkillSucceed(lSkillTypeID, NULL);	//有目标的情况下，按键技能朝向目标施放；PendingSkillInSkillAsset是按键触发的，按键触发的以鼠标方向为准
	}

	return e;
}
//------------------------------------------------------------------------------
bool LC_ClientGamePlayer::TalkToNPC(object_id_type lTargetID)
{
	LC_ClientNPCManager* const pkNPCManager = CLIENT_GET_NPC_MANAGER();
	LC_ClientNPC* const pkNPC = (LC_ClientNPC*)pkNPCManager->FindNPC(lTargetID);

	if(pkNPC->GetNPCType() == CLIENT_NPC_TYPE_PORTAL) //传送门
	{
		long lUserData = pkNPC->GetNPCFuncState(NFT_PORTAL);
		long lMapResID = lUserData >> 8;
		int eType = lUserData & 0xFF;
		if(eType == CLIENT_NPC_PORTAL_TOWN)	//城镇
		{
			MG_PACKET_ENCODE_HANDLER hMsgHandler;
			bool bCreateMsg = false;
			MG_REQ_ReturnToOut reqMsg;
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_RETURNTOOUT, &reqMsg);

			if(bCreateMsg)
				m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
		}
		else if (eType == CLIENT_NPC_PORTAL_RAID)	//副本
		{
			RequestTransfer(TT_TRANSPORT_RAID_MAP, 0, lMapResID);
		}
		else if ( eType == CLIENT_NPC_PORTAL_FREE) //自由传送
		{
			CF_WorldMapList* pkWorldMapList = CF_WorldMapList::GetSingletonPtr();
			if (pkWorldMapList)
			{
				CF_WorldMapList::DataEntry* pkDataEntry = pkWorldMapList->GetEntryPtr(lMapResID);
				if (pkDataEntry)
				{
					SendMessageDialog(RE_DIALOG_CONFIRM_TRANSFER_MAP, 
						pkDataEntry->_kName, "", MDT_DIALOG_CONFIRM_TRANSFER_MAP,
						TT_TRANSPORT_NORMAL_MAP, pkNPC->GetPortalTransferID(), lMapResID);
				}
			}
		}
		else if ( eType == CLIENT_NPC_PORTAL_FREE_NO_CONFIRM ) //自由传送，不显示确认框
		{
			LC_ClientMapTranscriptionManager* pkMapTransMgr = LC_ClientMapTranscriptionManager::GetSingletonPtr();
			if(pkMapTransMgr)
			{
				LC_ClientMapTransDataEntry* pkEntry = pkMapTransMgr->GetMapTransDataEntry(lMapResID);
				//if(pkEntry && GetLevel() < pkEntry->GetLevelLimit())
				if(pkEntry && GetCommanderLevel() < pkEntry->GetLevelLimit())
				{
					SendLogicInfoMessage(RE_TRANS_LEVEL_LOW, pkEntry->GetLevelLimit());
					return false;
				}
			}
			NW_ClientSessionManager* pkSessionManager	= NW_ClientSessionManager::GetSingletonPtr();
			NW_ClientSessionGroup* pkSessionGroup		= pkSessionManager->GetGameSessionGroup();

			//if ( GetServerID() == pkSessionGroup->GetCurGameServer())
			//{
				CF_WorldMapList::DataEntry* pkDataEntry = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(lMapResID);
				T_ASSERT_CRITICAL_S(pkDataEntry != NULL);

				int iTransferType = IS_NORMAL_MAP(pkDataEntry->_iMapLogicType) ? TT_TRANSPORT_NORMAL_MAP : TT_TRANSPORT_RAID_MAP;
				RequestTransfer(iTransferType, pkNPC->GetPortalTransferID(), lMapResID);
			//}
			//else
			//{
			//	GetAlliance()->ReqEnterAlliance(pkSessionGroup->GetCurGameServer(), pkNPC->GetPortalTransferID() );
			//}
		}
		else if (eType == CLIENT_NPC_PORTAL_FREE_NO_CONFIRM_CP )
		{
			NW_ClientSessionManager* pkSessionManager = NW_ClientSessionManager::GetSingletonPtr();
			NW_ClientSessionGroup* pkSessionGroup = pkSessionManager->GetGameSessionGroup();

			RequestTransportAreaTransfer(TT_TRANSPORT_CUR_MAP, pkNPC->GetPortalTransferID(), lMapResID, pkSessionGroup->GetCurGameServer());
		}
		return true;
	}

	if ( !LC_PlayerBase::TalkToNPC(lTargetID) )
	{
		return false;
	}


	if (NULL == pkNPC)
	{
		return false;
	}

	//注意！！写死的ID
	long lCharTypeID = pkNPC->GetCharType();
	if (lCharTypeID == 92000004 || lCharTypeID == 92000013 || lCharTypeID == 92000016)
	{
		CF_CharType::DataEntry *pkCharTypeData = CF_CharType::GetSingletonPtr()->GetEntryPtr(lCharTypeID);
		if (NULL == pkCharTypeData)
		{
			return false;
		}
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_CLIENT_GAMESTORY_TALK_TO_NPC);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_1,pkCharTypeData->_lConversation1);
		UI_UIManager::GetSingletonPtr()->HandlerGameEvent(kGameEvent);
		/*return true;*/
	}

	int iCount = pkNPC->GetNPCOpenFuncCount();

	SetNPCFunctionType(NFT_TALK);

	CF_NPCSoundList* pkNPCSoundList = CF_NPCSoundList::GetSingletonPtr();
	CF_NPCSoundList::DataEntry* pkData = pkNPCSoundList->GetEntryPtr( pkNPC->GetCharType() );

	if (pkData)
	{
		if ( g_soundMgr.IsAudioFile2d(pkData->_kName_1) )
			g_soundMgr.Play2DEx(pkData->_kName_1, AS_ClientSoundManager::SCT_NPC );
		else
			g_soundMgr.Play3DEx(UT_EngineMath::ConvertVec3IntToNiPoint3(pkNPC->GetVisibleLocation()), pkData->_kName_1, AS_ClientSoundManager::SCT_NPC );

		g_soundMgr.OnPlayNPCSound(g_settings.SOUND_LOWER_BGM_FOR_NPCSOUND, pkNPC->GetCharType());
	}
	else
	{
		GfxWriteLog( LOG_TAG_CLIENT_CRITICAL, LOG_SWITCH_CLIENT_CRITICAL , 
			"try to load NPC sound but none, CSV table wrong!");
		return false;
	}

	//LC_GameEvent kGameEvent;
	//kGameEvent.SetEventType(GLET_TALK_TO_NPC);

	//UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	//pkUIManager->HandlerGameEvent(kGameEvent);

	return true;
}
//----------------------------------------------------------------------
void LC_ClientGamePlayer::StopCurrentNPCSound()
{
	g_soundMgr.StopCurrentNPCSound();
}
//------------------------------------------------------------------------
bool LC_ClientGamePlayer::CheckPendingTalkNPC(object_id_type iTargetCharType, float fCurrentTime)
{
	GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI, "LC_ClientGamePlayer::CheckPendingTalkNPC iTargetCharType=[%d]", iTargetCharType);
	if(fCurrentTime <= m_fLastCheckPendingTalkNPCTime + 1.0f)
		return false ;

	m_fLastCheckPendingTalkNPCTime = fCurrentTime;
	LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
	const LC_NPCIDPtrMap& rNPCIDMap = pkNPCManager->GetNPCIDMap();
	LC_NPCIDPtrMapConstIter IterBe = rNPCIDMap.begin();
	LC_NPCIDPtrMapConstIter IterEd = rNPCIDMap.end();
	LC_ClientNPC* pkNPC = NULL;
	PF_Path kMovePath;
	int iDistance;
	PF_PathManager* pkPathManager = PF_PathManager::GetSingletonPtr();
	if(!pkPathManager)
		return false;

	for(; IterBe != IterEd;++IterBe)
	{
		pkNPC = (LC_ClientNPC*)((*IterBe).second);
		if (pkNPC&&pkNPC->GetCharType() != iTargetCharType)
			continue;

		if (pkNPC->CanInteraction())
		{
			LC_ClientNPCInteractionManager* pkInteractionManager = 
				(LC_ClientNPCInteractionManager*)LC_ClientNPCInteractionManager::GetSingletonPtr();

			int lParam1 = 0;
			int lParam2 = 0;

			if (pkInteractionManager&&pkInteractionManager->GetNPCInteractionOPParam(pkNPC,
				IOPT_STATE_CHANG_MOD,lParam1,lParam2))
			{
				if (lParam1 == 0 )
					continue;
			}
		}

		iDistance = UT_MathBase::LineLengthXYZInt(pkNPC->GetVisibleLocation(),GetVisibleLocation());
		if(iDistance >= g_settings.PROFILE_NPC_SELECTED_DISTANCE)
			continue;


		ActiveNPCFunc(pkNPC->GetID());
		/*
		kMovePath.Clear();
		bool bFindPath = pkPathManager->FindPath(kMovePath,GetCurrentLocation(),pkNPC->GetVisibleLocation(),false,true);
		if(bFindPath && kMovePath.GetPathPointCount() > 1)
		{
		int iPathLength = kMovePath.GetPathLength();
		if(iDistance <= iPathLength + 200)
		ActiveNPCFunc(pkNPC->GetID());
		}
		else
		{
		ActiveNPCFunc(pkNPC->GetID());
		}*/
		return true;
	}
	return false;
}
//------------------------------------------------------------------------
void LC_ClientGamePlayer::CheckPendingCollectTreasure(object_id_type lID)
{
	LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
	LC_ClientNPC* pkNPC = (LC_ClientNPC*)pkNPCManager->FindNPC(lID);
	if(pkNPC == NULL)
		return;

	int iDistance = UT_MathBase::LineLengthXYZInt(pkNPC->GetVisibleLocation(),GetVisibleLocation());
	if(iDistance >= g_settings.PROFILE_NPC_SELECTED_DISTANCE)
		return;

	ActiveNPCFunc(lID);
}
//------------------------------------------------------------------------
void LC_ClientGamePlayer::ActiveNPCFunc(object_id_type lID)
{
	LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
	LC_ClientNPC* pkNPC = (LC_ClientNPC*)pkNPCManager->FindNPC(lID);
	if(pkNPC == NULL)
		return;

	ClearPathPendingAction();

	if (pkNPC->CanInteraction())
	{
		//玩具捕捉先判断背包是否已满
		if(pkNPC != NULL && pkNPC->GetNPCFuncState(NET_TOYPET) > 0
			&& GetPackAsset()->IsPackFull(PACK_TYPE_BASIC))
		{
			SendLogicInfoMessage(RE_PRODUCTION_BACKPACK_FULL);
			return;
		}

		LC_ClientNPCInteractionManager* pkInteractionManager = 
			(LC_ClientNPCInteractionManager*)LC_ClientNPCInteractionManager::GetSingletonPtr();

		if (pkInteractionManager->CheckCanInteraction(pkNPC) != RE_SUCCESS)
		{
			pkInteractionManager->HandleResultInfo();
		}
		else
		{
			int lParam1 = 0;
			int lParam2 = 0;
			if (pkInteractionManager->GetNPCInteractionOPParam(	pkNPC,
				IOPT_STATE_INTERATION_SKILL,lParam1,lParam2) == false)
			{
				lParam1 = SKILL_COLLECT;
			}

			if (pkInteractionManager->IsDeflautActive(pkNPC->GetCharType())
				/*||pkNPC->GetNPCFuncState(NFT_COLLECTITEM)*/
				)
			{
				GetTargetMap()->Clear();
				WatchTarget(lID);
				SetLockedTargetID(lID);
				PendingSkill(lParam1,0);
				SetPPAArgRetryID(0);
				return;
			}
		}

		return;
	}
	else
	{
		//if (pkNPC->GetNPCFuncState(NFT_COLLECTITEM) >0)
		//{
		//	SetLockedTargetID(lID);
		//	PendingSkill(SKILL_COLLECT,0);
		//	return;
		//}
	}

	int iCount = pkNPC->GetNPCOpenFuncCount();
	if (iCount <= 0)
		return;

	GetTargetMap()->Clear();
	WatchTarget(lID);
	SetLockedTargetID(lID);
	ResultType result = PendingSkill(SKILL_TALK_TO_NPC,0);
	if(result == RE_SKILL_PENDING_SUCCESS)
	{
		SetPPAArgRetryID(0);
	}
}
//------------------------------------------------------------------------
void LC_ClientGamePlayer::PathToPoint(const Utility::UT_Vec3Int& pos,bool bForceAvenue /*= true*/)
{
	//轻功引导状态下，是不能发生寻路的，否则会出现问题，因为轻功引导最后会强设坐标，如果发生寻路，目标点改变，导致强设坐标异常
	if (GetCurrentStateType() == ST_PLAYER_JUMP_GUIDE)
	{
		return ;
	}

	//打断挂机
	if (IsHooking())
		SetHooking(false);

	if ( IsAutoPicking())
		SetAutoPicking(false);

	//打断当前技能，现在寻路优先级很高需要打断技能
	if(IS_SKILL_TYPE(m_iCurrentState))
	{
// 		ResultType eCheckInterrupt = CheckInterruptSkill(m_iCurrentState,SKI_MOVE);
// 		if(eCheckInterrupt != RE_SKILL_NONE_ERROR)
// 			return;

		float fCurrentTime = GET_PROCESS_TIME;
		RequestInterrputSkill(fCurrentTime,m_iCurrentState,SKI_FORCE);

		//无需服务器回包，先中断技能
		InterruptSkill(fCurrentTime,m_iCurrentState,SKI_FORCE);
	}

	//当在小地图寻路的时候需要把目标点设定到最近的没有阻挡的点上
	//需要进行一个比较大范围的试探搜索
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	Path::PF_PathManager* pkPathMgr = Path::PF_PathManager::GetSingletonPtr();
	UT_MathBase::ClampFloat( Int2Float(pos.x) , 0.0f , GfxGetWorldConfig().CFG_WORLD_MAP_WIDTH );
	UT_MathBase::ClampFloat( Int2Float(pos.y) , 0.0f , GfxGetWorldConfig().CFG_WORLD_MAP_WIDTH );
	UT_Vec3Int kNewTarget = pos;
	if ( pkPathMgr )
	{
		bool bCanMove = pkPathMgr->TestCanMoveTo( pos , false );
		if ( !bCanMove )
		{
			int lSearchStep = 200;
			int lHorizontalRange;
			int lVerticalRange;
			int lMaxDistance;
			if(!bForceAvenue)
			{
				lHorizontalRange = 1000;
				lVerticalRange   = 500;
				lMaxDistance     = 100;
			}
			else
			{
				lHorizontalRange = 10000;
				lVerticalRange   = 7000;
				lMaxDistance     = 500;
			}

			kNewTarget = pkPathMgr->FindClosestMovablePoint( pos , 
				lSearchStep , 100 , lHorizontalRange , lVerticalRange , lMaxDistance );
		}
	}

	if ( kNewTarget != UT_Vec3Int() )	//目标点是否有效
	{
		//判断是否要骑马去？
		int ride = GetRiderCharType();
		if (ride <= 0)
		{
			int dist = UT_MathBase::LineLengthXYInt(this->GetCurrentLocation() , kNewTarget);	//判断到终点直线距离（不是寻路的折线距离之和）
			if (dist > VEHICLE_PATH_TO_DISTANCE)
			{
				long lSkillID = SKILL_CALL_VEHICLE_IMMMEDIATELY;
				if( RE_SKILL_NONE_ERROR == this->CheckPendingSkill(lSkillID)
					&& RE_SKILL_PENDING_SUCCESS == this->PendingSkill(lSkillID, 0))
				{
					//this->OnPendingSkillSucceed(lSkillID, pkTarget);	//这里不需要调OnPendingSkillSucceed

					//这里是一个关键性的小技巧
					this->SetLockedLocation(kNewTarget);	//目标点作为LockedLocation，技能执行时会将LockedLocation设置为TargetLocation
				}
			}
		}

		//////////////////////////////////////////////////////////////////////////
		int oldValue = g_settings.LOD_PATH_QUERY_STEP;
		if (bForceAvenue)
			g_settings.LOD_PATH_QUERY_STEP = 200;
		else
			g_settings.LOD_PATH_QUERY_STEP = 60;

		SetMoveStride(false);
		SetLastPickTerrainTime(-1.0f);
		SetTargetLocation(kNewTarget);

		g_settings.LOD_PATH_QUERY_STEP = oldValue;
	}
	else
	{
		SendLogicInfoMessage(RE_FINDPATH_FAIL_CANT_REACH);
		/*GfxWriteLog( LOG_WARNING_FILELINE , LOG_SWITCH_WARNING ,
		"小地图点击距离可寻路区域过远[%d %d %d]",
		pos.x , pos.y , pos.z );*/
		return;
	}

	m_kSearchPathTargetPos = kNewTarget;
	m_bArriveFlag = false;
	if (LC_MapManager::GetSingletonPtr())
	{
		LC_MapManager::GetSingletonPtr()->GetMiniMap()->CalcWorldPosToMap(
			Int2Float(m_kSearchPathTargetPos.x), Int2Float(m_kSearchPathTargetPos.y),
			m_fSearchPathMiniMapY, m_fSearchPathMiniMapX);
	}
	SetPathFinding(true);
	m_kDoubleTerrienClearPath.ResetTimer();
}
//------------------------------------------------------------------------
void LC_ClientGamePlayer::PathToPointLocation(int iX,int iY,int iZ,bool bForceAvenue /* = true */)
{
	UT_Vec3Int kTargetLocation;
	kTargetLocation.x = iX;
	kTargetLocation.y = iY;
	kTargetLocation.z = iZ;
	if (iZ == 0)
	{
		SE_SceneManager* pkSceneManager = SE_SceneManager::GetSingletonPtr();
		const SE_SceneTerrainLocalInfo& kInfo = pkSceneManager->GetWalkableInfo((float)kTargetLocation.x, (float)kTargetLocation.y);
		kTargetLocation.z = static_cast < int > ( kInfo._fLocalHeight );
	}
	PathToPoint(kTargetLocation,bForceAvenue);
}
//------------------------------------------------------------------------------
void LC_ClientGamePlayer::TestArrival()
{	
	if (!m_bArriveFlag)
	{
		const Utility::UT_Vec3Int& kPlayerPos = GetCurrentLocation();

		int iDist = UT_MathBase::LineLengthXYInt( kPlayerPos, m_kSearchPathTargetPos );
		if ( iDist < PATH_MIN_DISTANCE
			|| (iDist <= g_settings.PROFILE_NPC_COLLECT_DISTANCE && GetPathPendingAction().enmType == PPAT_TALK_TO_NPC)) //NPC对话要离目标点有一段距离
		{
			m_bArriveFlag = true;
			OnMovePathEnd();
		}
	}
}
//------------------------------------------------------------------------------
bool LC_ClientGamePlayer::GetArriveFlag()
{
	return m_bArriveFlag;
}
//------------------------------------------------------------------------------
float LC_ClientGamePlayer::GetSearchPathMiniMapX()
{
	return m_fSearchPathMiniMapX;
}
//------------------------------------------------------------------------------
float LC_ClientGamePlayer::GetSearchPathMiniMapY()
{
	return m_fSearchPathMiniMapY;
}
//------------------------------------------------------------------------------
void LC_ClientGamePlayer::NotInSamePath()
{
	SendLogicInfoMessage(RE_FINDPATH_FAIL_NOT_IN_SAME_PATH);
}
//------------------------------------------------------------------------------
bool LC_ClientGamePlayer::NotifyStateEnd( int iEndState,float fCurrentTime,float fDeltaTime )
{
	LC_PlayerBase::NotifyStateEnd(iEndState,fCurrentTime,fDeltaTime);
	return m_pkSMController->NotifyStateEnd(this, iEndState, fCurrentTime, fDeltaTime);
}
//----------------------------------------------------------------------------
bool LC_ClientGamePlayer::ClearPkDebuffID()
{
	bool ret = false;
	MG_RespClearPK reqMsg;
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_CLEAR_PK, &reqMsg);
	if (bCreateMsg)
	{
		this->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
		ret = true;
	}

	return ret;
}
//----------------------------------------------------------------------------
void LC_ClientGamePlayer::WatchTarget(object_id_type lTargetID)
{
	SetWatchTargetID(lTargetID);
}
//--------------------------------------------------------
StringType LC_ClientGamePlayer::GetCashString()
{
	return LC_Money::MoneyNumber2String( m_kBindCash.GetMoney(),true );
}
//--------------------------------------------------------
StringType LC_ClientGamePlayer::GetUnBindCashString()
{
	return LC_Money::MoneyNumber2String( m_kUnBindCash.GetMoney(),false );
}
//--------------------------------------------------------
StringType LC_ClientGamePlayer::GetWarehouseCashString()
{	
	return LC_Money::MoneyNumber2String(m_kPackAsset.GetWarehousePack().m_kUnBindCash.GetMoney(),true);
}
//--------------------------------------------------------
StringType LC_ClientGamePlayer::GetYuanBaoString()
{
	return LC_Money::MoneyNumber2String( m_kBindYuanBao.GetMoney(),false);
}
//--------------------------------------------------------
StringType LC_ClientGamePlayer::GetUnBindYuanBaoString()
{
	return LC_Money::MoneyNumber2String( m_kUnBindYuanBao.GetMoney(),false );
}

//--------------------------------------------------------
StringType LC_ClientGamePlayer::GetCouragePTString()
{
	return LC_Money::MoneyNumber2String(GetCouragePT().GetMoney(), false);
}
//--------------------------------------------------------
StringType LC_ClientGamePlayer::GetJusticePTString()
{
	return LC_Money::MoneyNumber2String(GetJusticePT().GetMoney(), false);
}
//--------------------------------------------------------
StringType	LC_ClientGamePlayer::GetArenaPTString()
{
	return LC_Money::MoneyNumber2String(GetArenaPT().GetMoney(), false);
}
//--------------------------------------------------------
StringType	LC_ClientGamePlayer::GetMonopolyPTString()
{
	return LC_Money::MoneyNumber2String(GetMonopolyPT().GetMoney(), false);
}
//--------------------------------------------------------
StringType LC_ClientGamePlayer::GetRuneCreamString()
{
	return LC_Money::MoneyNumber2String(GetRuneCream().GetMoney(), false);
}
//--------------------------------------------------------
StringType LC_ClientGamePlayer::GetStarSoulString()
{
	return LC_Money::MoneyNumber2String(GetStarSoul().GetMoney(), false);
}
//--------------------------------------------------------
StringType LC_ClientGamePlayer::GetStarMoneyString()
{
	return LC_Money::MoneyNumber2String(GetPentacle().GetMoney(), false);
}
//--------------------------------------------------------
StringType LC_ClientGamePlayer::GetBuddyExpMoneyString()
{
	return LC_Money::MoneyNumber2String(GetBuddyExp().GetMoney(), false);
}
//--------------------------------------------------------
StringType LC_ClientGamePlayer::GetCourageString()
{
	return LC_Money::MoneyNumber2String(GetMoneyCourage().GetMoney(), false);
}
//--------------------------------------------------------
StringType LC_ClientGamePlayer::GetEnergyString()
{
	return LC_Money::MoneyNumber2String(GetEnergy().GetMoney(), false);
}
//--------------------------------------------------------
StringType LC_ClientGamePlayer::GetTrialString()
{
	return LC_Money::MoneyNumber2String(GetTrial().GetMoney(), false);
}
//--------------------------------------------------------
StringType LC_ClientGamePlayer::GetMagicCubeString()
{
	return LC_Money::MoneyNumber2String(GetMagicCube().GetMoney(), false);
}
//--------------------------------------------------------
StringType LC_ClientGamePlayer::GetActiveMeritString()
{
	return LC_Money::MoneyNumber2String(GetActiveMerit().GetMoney(), false);
}
//--------------------------------------------------------
StringType LC_ClientGamePlayer::GetRewardMedalString()
{
	return LC_Money::MoneyNumber2String(GetRewardMedal().GetMoney(), false);
}
//--------------------------------------------------------
StringType LC_ClientGamePlayer::GetDailyMeritString()
{
	return LC_Money::MoneyNumber2String(GetDailyMerit().GetMoney(), false);
}
//--------------------------------------------------------
StringType LC_ClientGamePlayer::GetGuildMeritString()
{
	return LC_Money::MoneyNumber2String(GetGuildMerit().GetMoney(), false);
}
//--------------------------------------------------------
StringType LC_ClientGamePlayer::GetChapterHonourString()
{
	return LC_Money::MoneyNumber2String(GetChapterHonour().GetMoney(), false);
}
//--------------------------------------------------------
StringType LC_ClientGamePlayer::GetBossJPString()
{
	return LC_Money::MoneyNumber2String(GetBossJP().GetMoney(), false);
}
//--------------------------------------------------------
StringType LC_ClientGamePlayer::GetAccountPointString()
{
	return LC_Money::MoneyNumber2String(GetAccountPoint().GetMoney(), false);
}
//--------------------------------------------------------
StringType LC_ClientGamePlayer::GetSkinPointString()
{
	return LC_Money::MoneyNumber2String(GetSkinPoint().GetMoney(), false);
}

StringType LC_ClientGamePlayer::GetRaidGloryPointString()
{
	return LC_Money::MoneyNumber2String(GetCashType28().GetMoney(), false);
}

StringType LC_ClientGamePlayer::GetYijituxiScoreString()
{
	return LC_Money::MoneyNumber2String(GetCashType29().GetMoney(), false);
}

StringType LC_ClientGamePlayer::GetEntrustTaskPointString()
{
	return LC_Money::MoneyNumber2String(GetCashType30().GetMoney(), false);
}

StringType LC_ClientGamePlayer::GetSkyArenaPointString()
{
	return LC_Money::MoneyNumber2String(GetCashType32().GetMoney(), false);
}

StringType LC_ClientGamePlayer::GetVehiclePointString()
{
	return LC_Money::MoneyNumber2String(GetCashType34().GetMoney(), false);
}

StringType LC_ClientGamePlayer::GetPetPointString()
{
	return LC_Money::MoneyNumber2String(GetCashType35().GetMoney(), false);
}

StringType LC_ClientGamePlayer::GetBackWingPointString()
{
	return LC_Money::MoneyNumber2String(GetCashType36().GetMoney(), false);
}
//--------------------------------------------------------
void LC_ClientGamePlayer::SetBindCash( MoneyType llCash )
{
	GetBindCash().SetMoney(llCash);
	GetBindCash().AddDirtyFlagRevisionID();
}
//--------------------------------------------------------
void LC_ClientGamePlayer::SetUnBindCash( MoneyType llCash )
{
	GetUnBindCash().SetMoney(llCash);
	GetUnBindCash().AddDirtyFlagRevisionID();
}
//--------------------------------------------------------
void LC_ClientGamePlayer::RequestCreatePlayerGroup()
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	if ( GetPlayerGroupID() != INVALID_GROUP_ID )
	{
		SendLogicInfoMessage(RE_GROUP_CREATE_FAIL_EXIST_TEAM);
		return;
	}

	//是否屏蔽该操作
	if(m_kActionForbid.IsActionForbid(LAFT_GROUP,LAF_GROUP_CREATE_GROUP))
	{
		SendLogicInfoMessage(RE_ACTION_FORBID);
		return;
	}

	MG_REQ_CreateGroup reqMsg;

	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_CREATE_GROUP, &reqMsg);
	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
//--------------------------------------------------------
bool LC_ClientGamePlayer::IsNeedUnBindCash( MoneyType llNeedCash )
{
	MoneyType ulCash = llNeedCash;
	if ( ulCash > GetBindCash().GetMoney() )
	{
		MoneyType ulNeedBindPrice = ulCash - GetBindCash().GetMoney();
		if ( ulNeedBindPrice > GetUnBindCash().GetMoney() )
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	return false;
}
//--------------------------------------------------------
void LC_ClientGamePlayer::RequestExitPlayerGroup()
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	if ( GetPlayerGroupID() == INVALID_GROUP_ID )
	{
		SendLogicInfoMessage(RE_GROUP_EXIT_FAIL_NOT_EXIST_TEAM);
		return;
	}

	//是否屏蔽该操作
	if(m_kActionForbid.IsActionForbid(LAFT_GROUP,LAF_GROUP_EXIT))
	{
		SendLogicInfoMessage(RE_ACTION_FORBID);
		return;
	}

	MG_REQ_ExitGroup reqMsg;
	reqMsg.m_lGroupID = GetPlayerGroupID();
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_EXIT_GROUP, &reqMsg);
	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
//--------------------------------------------------------
void LC_ClientGamePlayer::RequestJoinPlayerGroup(unique_id_type lCharID)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	if ( GetPlayerGroupID() != INVALID_GROUP_ID )
	{
		SendLogicInfoMessage(RE_GROUP_JOIN_FAIL_EXIST_TEAM);
		return;
	}

	if(lCharID == GetCommanderUID())
	{
		SendLogicInfoMessage(RE_GROUP_JOIN_FAIL_NOT_EXIST_TEAM,GET_MSG(LC_CLIENTGAMEPLAYER_CPP_001));
		return;
	}

	//是否屏蔽该操作
	if(m_kActionForbid.IsActionForbid(LAFT_GROUP,LAF_GROUP_JOIN))
	{
		SendLogicInfoMessage(RE_ACTION_FORBID);
		return;
	}
	
	if ( lCharID > 0 )
	{
		MG_REQ_JoinGroup reqMsg;
		reqMsg.m_lJoinTargetCharID	= lCharID;
		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_JOIN_GROUP, &reqMsg);

		if(bCreateMsg)
			m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
	
}
//--------------------------------------------------------
void LC_ClientGamePlayer::RequestKickPlayerFromGroup(unique_id_type lCharID)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	if ( GetPlayerGroupID() == INVALID_GROUP_ID )
	{
		SendLogicInfoMessage(RE_GROUP_KICK_FAIL_NOT_EXIST_TEAM);
		return;
	}

	if (GetPlayerGroup() == NULL)
	{
		SendLogicInfoMessage(RE_GROUP_KICK_FAIL_NOT_EXIST_TEAM);
		return;
	}

	//是否屏蔽该操作
	if(m_kActionForbid.IsActionForbid(LAFT_GROUP,LAF_GROUP_KICK_PLAYER))
	{
		SendLogicInfoMessage(RE_ACTION_FORBID);
		return;
	}

	object_id_type lGroupCheifID = GetPlayerGroup()->GetGroupCheifID();
	if(lGroupCheifID != GetMainControllerID())
	{
		SendLogicInfoMessage(RE_GROUP_KICK_FAIL_NOT_CHEIF);		
		return;
	}

	MG_REQ_KickPlayerFromGroup reqMsg;
	reqMsg.m_lGroupID = GetPlayerGroupID();
	reqMsg.m_lKickCharID = lCharID;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_KICK_PLAYER_FROM_GROUP, &reqMsg);

	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
//--------------------------------------------------------
void LC_ClientGamePlayer::RequestDismissPlayerGroup()
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	if ( GetPlayerGroupID() == INVALID_GROUP_ID )
	{
		SendLogicInfoMessage(RE_GROUP_DISMISS_FAIL_NOT_EXIST_TEAM);		
		return;
	}

	if ( GetPlayerGroup() == NULL)
	{
		SendLogicInfoMessage(RE_GROUP_DISMISS_FAIL_NOT_EXIST_TEAM);		
		return;
	}

	object_id_type lGroupCheifID = GetPlayerGroup()->GetGroupCheifID();
	if(lGroupCheifID != GetMainControllerID())
	{
		SendLogicInfoMessage(RE_GROUP_DISMISS_FAIL_NOT_CHEIF);
		return;
	}

	//是否屏蔽该操作
	if(m_kActionForbid.IsActionForbid(LAFT_GROUP,LAF_GROUP_DISMISS_GROUP))
	{
		SendLogicInfoMessage(RE_ACTION_FORBID);
		return;
	}

	MG_REQ_DismissGroup reqMsg;
	reqMsg.m_lGroupID = GetPlayerGroupID();
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_DISMISS_GROUP, &reqMsg);

	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
//--------------------------------------------------------
void LC_ClientGamePlayer::RequestConfrimGroupJoin(int iCandidateIndex,bool bAccept)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	if ( GetPlayerGroupID() == INVALID_GROUP_ID )
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"没有队伍，无法操作");
		return;
	}

	if (GetPlayerGroup() == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"没有队伍，无法操作");
		return;
	}

	object_id_type lGroupCheifID = GetPlayerGroup()->GetGroupCheifID();
	if(lGroupCheifID != GetMainControllerID())
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"不是队长，无法操作");
		return;
	}

	//是否屏蔽该操作
	if(m_kActionForbid.IsActionForbid(LAFT_GROUP,LAF_GROUP_CONFIRM_JOIN))
	{
		SendLogicInfoMessage(RE_ACTION_FORBID);
		return;
	}

	LC_PlayerGroupEntry* pkCandidateEntry = GetPlayerGroup()->GetGroupEntryInCandidateList(iCandidateIndex);
	if(!pkCandidateEntry || !pkCandidateEntry->GetValid())
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"无效数据，无法操作");
		return;
	}

	if ( GetPlayerGroup()->IsInGroupMemeberList(pkCandidateEntry->GetCharID()) )
	{
		return;
	}

	MG_RLT_ConfirmGroupJoin rltMsg;
	rltMsg.m_lJoinTargetCharID = pkCandidateEntry->GetCharID();
	rltMsg.m_lGroupID = GetPlayerGroupID();
	if(bAccept)
		rltMsg.m_iConfirmResult = RE_GROUP_JOIN_CONFIRM_ACCEPT;
	else
		rltMsg.m_iConfirmResult = RE_GROUP_JOIN_CONFIRM_REJECT;

	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_RLT_CONFIRM_GROUP_JOIN, &rltMsg);
	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	//GetPlayerGroup()->RemoveCandidateInfoIndex(iCandidateIndex);
}
//--------------------------------------------------------
void LC_ClientGamePlayer::RequestConfrimGroupJoinByPlayerID(unique_id_type lCharID, bool bAccept)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	if ( GetPlayerGroupID() == INVALID_GROUP_ID )
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"没有队伍，无法操作");
		return;
	}

	if (GetPlayerGroup() == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"没有队伍，无法操作");
		return;
	}

	object_id_type lGroupCheifID = GetPlayerGroup()->GetGroupCheifID();
	if(lGroupCheifID != GetMainControllerID())
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"不是队长，无法操作");
		return;
	}

	//是否屏蔽该操作
	if(m_kActionForbid.IsActionForbid(LAFT_GROUP,LAF_GROUP_CONFIRM_JOIN))
	{
		SendLogicInfoMessage(RE_ACTION_FORBID);
		return;
	}

	if (GetPlayerGroup()->IsInGroupMemeberList(lCharID))
	{
		return;
	}

	MG_RLT_ConfirmGroupJoin rltMsg;
	rltMsg.m_lJoinTargetCharID = lCharID;
	rltMsg.m_lGroupID = GetPlayerGroupID();
	if(bAccept)
		rltMsg.m_iConfirmResult = RE_GROUP_JOIN_CONFIRM_ACCEPT;
	else
		rltMsg.m_iConfirmResult = RE_GROUP_JOIN_CONFIRM_REJECT;

	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_RLT_CONFIRM_GROUP_JOIN, &rltMsg);
	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
//--------------------------------------------------------
void LC_ClientGamePlayer::RequestInvitePlayerToGroup(unique_id_type lCharID)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	if(lCharID == GetCommanderUID())
	{
		SendLogicInfoMessage(RE_GROUP_INVITE_FAIL_TARGET_IS_SELF);
		return;
	}

	//是否屏蔽该操作
	if(m_kActionForbid.IsActionForbid(LAFT_GROUP,LAF_GROUP_INVITE))
	{
		SendLogicInfoMessage(RE_ACTION_FORBID);
		return;
	}

	if ( GetForbidList()->IsForbid(lCharID) )
	{
		SendLogicInfoMessage(RE_GROUP_INVITE_FAIL_TARGET_IN_FORBID);
		return;
	}
	
	//是否处于冷却
	bool bCanSend = true;
	mem::map<unique_id_type,float>::iterator iter = m_InviteCountDown.find(lCharID);
	float fCurTime = GET_PROCESS_TIME;
	if (iter != m_InviteCountDown.end())
	{
		int DeltaTime=fCurTime-(*iter).second;
		bCanSend=DeltaTime>10;
		if (bCanSend)
		{
			(*iter).second = fCurTime;
		}

	}
	else
	{
		m_InviteCountDown.insert(std::pair<unique_id_type,float>(lCharID,fCurTime));
	}

	if (!bCanSend)
	{
		SendLogicInfoMessage(RE_GROUP_INVITE_FAIL_TARGET_IS_BUSY);
		return;
	}

	int lGroupID = 0;
	if ( GetPlayerGroupID() != INVALID_GROUP_ID )
		lGroupID = GetPlayerGroupID();
	
	MG_REQ_InviteToGroup reqMsg;
	reqMsg.m_lGroupID = lGroupID;
	reqMsg.m_lBeInviteCharID = lCharID;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_INVITE_TO_GROUP, &reqMsg);

	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	
	//SendLogicInfoMessage(RE_PLAYER_GROUP_SEND_INVITE,pkNetPlayer->GetCharName().c_str());
}
//--------------------------------------------------------
void LC_ClientGamePlayer::RequestConfirmInviteToGroup(unique_id_type lInvitePlayerID,int lGroupID,unique_id_type lBeInvitePlayerID,bool bAccept)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	if(GetPlayerGroupID() == INVALID_GROUP_ID || GetPlayerGroupID() != lGroupID)
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"没有队伍，无法操作");
		return;
	}

	if(GetPlayerGroup() == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"没有队伍，无法操作");
		return;
	}

	object_id_type lGroupCheifID = GetPlayerGroup()->GetGroupCheifID();
	if(lGroupCheifID != GetMainControllerID())
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"不是队长，无法操作");
		return;
	}

	//是否屏蔽该操作
	if(m_kActionForbid.IsActionForbid(LAFT_GROUP,LAF_GROUP_CONFIRM_INVITE))
	{
		SendLogicInfoMessage(RE_ACTION_FORBID);
		return;
	}

	MG_RLT_ConfirmInviteToGroup rltMsg;
	rltMsg.m_lInviteCharID = lInvitePlayerID;
	rltMsg.m_lGroupID = lGroupID;
	rltMsg.m_lBeInviteCharID = lBeInvitePlayerID;

	if(bAccept)
		rltMsg.m_iConfirmResult = RE_GROUP_INVITE_CHEIF_ACCEPT_JOIN;
	else
		rltMsg.m_iConfirmResult = RE_GROUP_INVITE_CHEIF_DENY_JOIN;

	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_RLT_CONFIRM_INVITE_TO_GROUP, &rltMsg);

	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
//--------------------------------------------------------
void LC_ClientGamePlayer::RequestConfirmBeInviteToGroup(int iGroupIndex,bool bAccept)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	if ( GetPlayerGroupID() != INVALID_GROUP_ID )
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"已有队伍，无法操作");
		return;
	}

	if(iGroupIndex < 0 || iGroupIndex >= UInt2Int(m_kInvitePlayerGroupEntryVector.size()))
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"无法操作该队伍");
		return;
	}

	//是否屏蔽该操作
	if(m_kActionForbid.IsActionForbid(LAFT_GROUP,LAF_GROUP_CONFIRM_BE_INVITE))
	{
		SendLogicInfoMessage(RE_ACTION_FORBID);
		return;
	}

	LC_PlayerGroupInviteEntry& rkInvitePlayerGroupEntry = m_kInvitePlayerGroupEntryVector[iGroupIndex];

	MG_RLT_ConfirmBeInviteToGroup rltMsg;
	rltMsg.m_lInviteCharID = rkInvitePlayerGroupEntry.m_lInviteCharID;
	rltMsg.m_lGroupID = rkInvitePlayerGroupEntry.m_lInvitePlayerGroupID;
	rltMsg.m_lBeInviteCharID = GetCommanderUID();
	if(bAccept)
		rltMsg.m_iConfirmResult = RE_GROUP_INVITE_TARGET_ACCEPT_JOIN;
	else
		rltMsg.m_iConfirmResult = RE_GROUP_INVITE_TARGET_DENY_JOIN;

	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_RLT_CONFIRM_BEINVITE_TO_GROUP, &rltMsg);

	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	//数据删除放在后面，不然rkInvitePlayerGroupEntry数据有可能会乱的哦。。
	RemoveInvitePlayerGroup(rkInvitePlayerGroupEntry.m_lInvitePlayerGroupID);
}

//--------------------------------------------------------
int LC_ClientGamePlayer::GetInviteTeamMapResID(int iGroupIndex)
{
	if ( GetPlayerGroupID() != INVALID_GROUP_ID )
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"已有队伍，无法操作");
		return 0;
	}

	if(iGroupIndex < 0 || iGroupIndex >= UInt2Int(m_kInvitePlayerGroupEntryVector.size()))
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"无法操作该队伍");
		return 0;
	}

	//是否屏蔽该操作
	if(m_kActionForbid.IsActionForbid(LAFT_GROUP,LAF_GROUP_CONFIRM_BE_INVITE))
	{
		SendLogicInfoMessage(RE_ACTION_FORBID);
		return 0;
	}

	LC_PlayerGroupInviteEntry& rkInvitePlayerGroupEntry = m_kInvitePlayerGroupEntryVector[iGroupIndex];

	if (rkInvitePlayerGroupEntry.m_GroupRaidVaild && rkInvitePlayerGroupEntry.m_Combin)
	{
		return rkInvitePlayerGroupEntry.m_GroupRaidResId;
	}

	return 0;
}


void LC_ClientGamePlayer::RequestVIPReward(int mapResID)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;
	MG_REQ_ShopTransferItem Msg;
	Msg.m_iMapResId = mapResID;
	
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_GAW_REQ_SHOP_TRANSFERITEM, &Msg);

	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}






//--------------------------------------------------------
void LC_ClientGamePlayer::RequestConfirmBeInviteToGroupByGroupID(unique_id_type lInviteCharID, int iGroupID, bool bAccept)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	if ( GetPlayerGroupID() != INVALID_GROUP_ID )
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"已有队伍，无法操作");
		return;
	}

	//是否屏蔽该操作
	if(m_kActionForbid.IsActionForbid(LAFT_GROUP,LAF_GROUP_CONFIRM_BE_INVITE))
	{
		SendLogicInfoMessage(RE_ACTION_FORBID);
		return;
	}

	RemoveInvitePlayerGroup(iGroupID);

	MG_RLT_ConfirmBeInviteToGroup rltMsg;
	rltMsg.m_lInviteCharID = lInviteCharID;
	rltMsg.m_lGroupID = iGroupID;
	rltMsg.m_lBeInviteCharID = GetCommanderUID();
	if(bAccept)
		rltMsg.m_iConfirmResult = RE_GROUP_INVITE_TARGET_ACCEPT_JOIN;
	else
		rltMsg.m_iConfirmResult = RE_GROUP_INVITE_TARGET_DENY_JOIN;

	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_RLT_CONFIRM_BEINVITE_TO_GROUP, &rltMsg);

	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
//--------------------------------------------------------
void LC_ClientGamePlayer::RequestAppointmentCheif(unique_id_type lCharID)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	if ( GetPlayerGroupID() == INVALID_GROUP_ID )
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"无法操作,没有队伍");
		return;
	}

	if ( GetPlayerGroup() == NULL )
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"无法操作,没有队伍");
		return;
	}

	if(GetPlayerGroup()->GetGroupCheifID() != GetMainControllerID())
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"无法操作,不是队长");
		return;
	}

	//是否屏蔽该操作
	if(m_kActionForbid.IsActionForbid(LAFT_GROUP,LAF_GROUP_APPOINT_CHEIF))
	{
		SendLogicInfoMessage(RE_ACTION_FORBID);
		return;
	}

	if(lCharID == GetCommanderUID())
	{
		//GfxWriteLog(LOG_TAG_GAME_TIP,LOG_SWITCH_GAME,GFxSystemInfo(RE_PLAYER_GROUP_APPOINTMENT_CHEIF_FAIL).c_str(),GetCharName().c_str());
		SendLogicInfoMessage(RE_GROUP_APPOINTMENT_CHEIF_FAIL_ALREADY_CHEIF);
		return;
	}

	MG_REQ_AppointmentCheif reqMsg;
	reqMsg.m_lGroupID = GetPlayerGroupID();
	reqMsg.m_lBeAppointmentCharID = lCharID;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_APPOINTMENT_CHEIF, &reqMsg);

	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
//--------------------------------------------------------
void LC_ClientGamePlayer::RequestBlockPlayerJoin(int iCandidateIndex)
{
	if ( GetPlayerGroupID() == INVALID_GROUP_ID )
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"没有队伍，无法操作");
		return;
	}

	if( GetPlayerGroup() == NULL )
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"没有队伍，无法操作");
		return;
	}

	object_id_type lGroupCheifID = GetPlayerGroup()->GetGroupCheifID();
	if(lGroupCheifID != GetMainControllerID())
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"不是队长，无法操作");
		return;
	}

	////是否屏蔽该操作
	//if(m_kActionForbid->IsActionForbid(LAFT_GROUP,LAF_GROUP_FORBID_JOIN))
	//{
	//	SendLogicInfoMessage(RE_ACTION_FORBID);
	//	return;
	//}

	LC_PlayerGroupEntry* pkCandidateEntry = GetPlayerGroup()->GetGroupEntryInCandidateList(iCandidateIndex);
	if(!pkCandidateEntry || !pkCandidateEntry->GetValid())
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"无效数据，无法操作");
		return;
	}

	RequestAddForbidPlayer(pkCandidateEntry->GetPlayerID(), pkCandidateEntry->GetName());

	RequestConfrimGroupJoin(iCandidateIndex, false);
}
//--------------------------------------------------------
void LC_ClientGamePlayer::RequestModifyQuality(int iIndex)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	if ( GetPlayerGroupID() == INVALID_GROUP_ID )
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"没有队伍，无法操作");
		return;
	}

	if ( GetPlayerGroup() == NULL )
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"没有队伍，无法操作");
		return;
	}

	if(GetPlayerGroup()->GetGroupCheifID() != GetMainControllerID())
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"不是队长，无法操作");
		return;
	}

	//是否屏蔽该操作
	if(m_kActionForbid.IsActionForbid(LAFT_GROUP,LAF_GROUP_MODIFY_QUALITY))
	{
		SendLogicInfoMessage(RE_ACTION_FORBID);
		return;
	}

	MG_REQ_ModifyQuality reqMsg;
	reqMsg.m_lGroupID = GetPlayerGroupID();
	reqMsg.m_iItemGrade = ITEM_GRADE_NONE+iIndex;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_MODIFY_QUALITY, &reqMsg);

	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
//--------------------------------------------------------
void LC_ClientGamePlayer::RequestForbidJoin(bool bFlag)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	if ( GetPlayerGroupID() == INVALID_GROUP_ID )
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"没有队伍，无法操作");
		return;
	}

	if ( GetPlayerGroup() == NULL )
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"没有队伍，无法操作");
		return;
	}

	if (GetPlayerGroup()->GetForbidJoinFlag() == bFlag)
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"队伍，无用操作");
		return;
	}

	object_id_type lGroupCheifID = GetPlayerGroup()->GetGroupCheifID();
	if(lGroupCheifID != GetMainControllerID())
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"不是队长，无法操作");
		return;
	}

	//是否屏蔽该操作
	if(m_kActionForbid.IsActionForbid(LAFT_GROUP,LAF_GROUP_FORBID_JOIN))
	{
		SendLogicInfoMessage(RE_ACTION_FORBID);
		return;
	}

	MG_REQ_ForbidJion reqMsg;
	reqMsg.m_lGroupID = GetPlayerGroupID();
	reqMsg.m_bForbidJoinFlag = bFlag;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_CHEIF_FORBID_JOIN, &reqMsg);

	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
//--------------------------------------------------------
void LC_ClientGamePlayer::RequestSyncInviteGroupMemberList(int iIndex)
{	 	
	if (iIndex >= 0 && iIndex < GetInvitePlayerGroupCount())
	{
		LC_PlayerGroupBase* pkGroup = GetInvitePlayerGroup(iIndex);
		if (pkGroup && pkGroup->IsMemberListEmpty())
		{
			int lGroupID = m_kInvitePlayerGroupEntryVector.at(iIndex).m_lInvitePlayerGroupID;

			// 这里需要策略控制

			MG_PACKET_ENCODE_HANDLER hMsgHandler;

			MG_REQ_SyncInviteGroupMemberList reqMsg;
			reqMsg.m_uiGroupID = lGroupID;
			bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SYNC_INVITE_GROUP_MEMBER_LIST, &reqMsg);

			if(bCreateMsg)
				m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
		}		
	}
}
//--------------------------------------------------------
void LC_ClientGamePlayer::RequestClearCandidateList()
{
	LC_PlayerGroupBase* pkPlayerGroup = GetPlayerGroup();
	if(NULL == pkPlayerGroup)
		return;

	for(int i = 0; i < MAX_PLAYER_GROUP_CANDIDATE_LIST_SIZE; ++i)
	{
		LC_PlayerGroupEntry* pkCandidateEntry = pkPlayerGroup->GetGroupEntryInCandidateList(i);
		if(pkCandidateEntry && pkCandidateEntry->GetValid())
		{
			RequestConfrimGroupJoin(i,false);
		}
	}
}
//--------------------------------------------------------
void LC_ClientGamePlayer::RequestClearInviteGroupList()
{
	LC_PlayerGroupInviteEntryVector kCopyVector(m_kInvitePlayerGroupEntryVector);

	for(int i = 0; i < kCopyVector.size(); i++)
	{
		RequestConfirmBeInviteToGroup(0,false);
	}	
}
//--------------------------------------------------------
void LC_ClientGamePlayer::AddInvitePlayerGroup(int lGroupID, unique_id_type lInviteCharID, bool bVaild, bool bCombin, int iMapResID)
{
	LC_PlayerGroupInviteEntryVectorIter Iter;
	TRAV(m_kInvitePlayerGroupEntryVector,Iter)
	{
		if(Iter->m_lInvitePlayerGroupID == lGroupID)
		{
			LC_ClientPlayerGroupManager* pkPlayerGroupManager = CLIENT_GET_PLAYER_GROUP_MANAGER();
			if( NULL == pkPlayerGroupManager->FindPlayerGroupByGroupID(lGroupID) )
			{
				LC_PlayerGroupBase* pkPlayerGroup = pkPlayerGroupManager->CreatePlayerGroup(lGroupID);
			}			
			return;
		}
	}

	LC_ClientPlayerGroupManager* pkPlayerGroupManager = CLIENT_GET_PLAYER_GROUP_MANAGER();
	LC_PlayerGroupBase* pkPlayerGroup = pkPlayerGroupManager->CreatePlayerGroup(lGroupID);

	LC_PlayerGroupInviteEntry kEntry;
	kEntry.m_lInviteCharID		= lInviteCharID;
	kEntry.m_lInvitePlayerGroupID	= lGroupID;
	kEntry.m_GroupRaidVaild			= bVaild;
	kEntry.m_Combin					= bCombin;
	kEntry.m_GroupRaidResId			= iMapResID;
	m_kInvitePlayerGroupEntryVector.push_back(kEntry);
}
//--------------------------------------------------------
void LC_ClientGamePlayer::RemoveInvitePlayerGroup(int lGroupID)
{
	LC_PlayerGroupInviteEntryVectorIter Iter;
	TRAV(m_kInvitePlayerGroupEntryVector,Iter)
	{
		if(Iter->m_lInvitePlayerGroupID == lGroupID)
		{
			m_kInvitePlayerGroupEntryVector.erase(Iter);
			return;
		}
	}
}
//--------------------------------------------------------
LC_PlayerGroupBase* LC_ClientGamePlayer::GetInvitePlayerGroup(int iIndex)
{
	if(iIndex >= UInt2Int(m_kInvitePlayerGroupEntryVector.size()))
		return NULL;

	int lGroupID = m_kInvitePlayerGroupEntryVector[iIndex].m_lInvitePlayerGroupID;

	LC_ClientPlayerGroupManager* pkPlayerGroupManager = CLIENT_GET_PLAYER_GROUP_MANAGER();
	LC_PlayerGroupBase* pkPlayerGroup = pkPlayerGroupManager->FindPlayerGroupByGroupID(lGroupID);
	return pkPlayerGroup;

}
//--------------------------------------------------------
int LC_ClientGamePlayer::GetInvitePlayerGroupCount()
{
	return UInt2Int(m_kInvitePlayerGroupEntryVector.size());
}
//--------------------------------------------------------
void LC_ClientGamePlayer::ClearInvitePlayerGroup()
{
	m_kInvitePlayerGroupEntryVector.clear();
}
//--------------------------------------------------------
bool LC_ClientGamePlayer::GetForbidJoinFlag()
{
	if ( GetPlayerGroupID() != INVALID_GROUP_ID )
	{		
		if ( GetPlayerGroup() )
		{
			return GetPlayerGroup()->GetForbidJoinFlag();
		}		
	}

	return false;
}
//--------------------------------------------------------
int LC_ClientGamePlayer::GetTeamMapResID()
{
	int iMapResID = 0;
	LC_PlayerGroupBase* pkPlayerGroup = GetPlayerGroup();
	if (!pkPlayerGroup)
		return iMapResID;

	RaidPortalEntry* pkEntry = pkPlayerGroup->GetRaidPortal();
	if (!pkEntry)
		return iMapResID;

	if (pkEntry->m_Vaild)
		iMapResID = pkEntry->m_Resid;

	return iMapResID;
}
//--------------------------------------------------------
bool LC_ClientGamePlayer::GetTeamHintFlag()
{
	//if (m_bTeamHint)
	//{
	//	if ( GetPlayerGroupCheifFlag() )
	//	{
	//		if ( GetPlayerGroup()->IsCandidateListEmpty() )
	//		{
	//			m_bTeamHint = false;
	//		}			
	//	}
	//	else
	//	{
	//		if ( GetInvitePlayerGroupCount() == 0)
	//		{
	//			m_bTeamHint = false;
	//		}
	//	}		
	//}

	//return m_bTeamHint;


	if ( GetPlayerGroupCheifFlag() )
	{		
		if( GetPlayerGroup() == NULL)
		{
			return false;
		}

		if ( GetPlayerGroup()->IsCandidateListEmpty() )
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		if ( GetPlayerGroup() == NULL )
		{
			if ( GetInvitePlayerGroupCount() == 0 )
			{
				return false;
			}
			else
			{
				return true;
			}
		}
		else
		{
			return false;
		}
	}
}
//--------------------------------------------------------
void LC_ClientGamePlayer::SetTeamHintFlag(bool bFlag)
{	
	m_bTeamHint = bFlag;
}
//--------------------------------------------------------

//--------------------------------------------------------
void LC_ClientGamePlayer::RequestAddForbidPlayer(unique_id_type lID, const StringType& strName)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	ResultType iResultType = AddForbidPlayer(lID,strName);
	if (iResultType == RE_FORBID_ADD_PLAYER_SUCCESS)
	{
		MG_REQ_AddForbidPlayer msg;
		msg.m_lPlayerID = GetID();
		msg.m_kData.lCharID =  lID;
		TPSTR2STDSTR(strName, msg.m_kData.strCharName);
		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_ADDFORBIDPLAYER, &msg);
		if(bCreateMsg)
			m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}

	if (iResultType == RE_SOCIAL_FORBID_PLAYER_IS_EXIST
		|| iResultType == RE_FORBID_ADD_PLAYER_SUCCESS)
	{
		SendLogicInfoMessage(iResultType,strName.c_str());
	}
	else
		SendLogicInfoMessage(iResultType);

}
//------------------------------------------------------------------------
void LC_ClientGamePlayer::RequestRemoveForbidPlayer( unique_id_type lID )
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	StringType strName = "";
	LC_ForbidList* pkList = GetForbidList();
	//if (pkList)
	{
		for (int i = 0;i <MAX_FORBID_NUM ;++i)
		{
			if (pkList->GetForbidPlayerID(i) == lID)
			{
				strName = pkList->GetForbidPlayerName(i);
				break;
			}
		}
	}

	ResultType iResult = RemoveForbidPlayer(lID);
	if(iResult == RE_FORBID_REMOVE_PLAYER_SUCCESS)
	{
		MG_REQ_RemoveForbidPlayer reqMsg;
		reqMsg.m_lPlayerID = GetID();
		reqMsg.m_lTargetCharID = lID;
		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_REMOVEFORBIDPLAYER, &reqMsg);

		if(bCreateMsg)
			m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}

	if (iResult == RE_FORBID_REMOVE_PLAYER_SUCCESS)
	{
		SendLogicInfoMessage(iResult,strName.c_str());
	}
	else
		SendLogicInfoMessage(iResult);


}
//-------------------------------------------------------
object_id_type LC_ClientGamePlayer::GetForbidPlayerID( int iIndex )
{
	return GetForbidList()->GetForbidPlayerID(iIndex);
}
//-------------------------------------------------------
StringType LC_ClientGamePlayer::GetForbidPlayerName( int iIndex )
{
	return GetForbidList()->GetForbidPlayerName(iIndex);
}
//-------------------------------------------------------
bool LC_ClientGamePlayer::IsForbidPlayer( unique_id_type lID )
{
	return GetForbidList()->IsForbid(lID);
}
//-------------------------------------------------------
bool LC_ClientGamePlayer::IsFriend(unique_id_type lTargetCharID)
{
	LC_FriendDetailInfo* pkFriendDetailInfo = GetSocietyAsset()->GetFriendInfo1(lTargetCharID);	
	return pkFriendDetailInfo != NULL;
}
//-------------------------------------------------------
bool LC_ClientGamePlayer::IsEnemy(unique_id_type lTargetCharID)
{
	LC_SocietyBasicInfo* pkSocietyBasicInfo = GetSocietyAsset()->GetEnemy(lTargetCharID);
	return pkSocietyBasicInfo != NULL;
}
//-------------------------------------------------------
void LC_ClientGamePlayer::RequestConfirmSkillRespawn(object_id_type lRespawnSourcePlayerID,bool bAccept)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_RLT_SkillRespawn rltMsg;
	rltMsg.m_lRespawnSourcePlayerID = lRespawnSourcePlayerID;
	if(bAccept)
		rltMsg.m_lConfirmResult = RE_SKILL_ACCEPT_SKILL_RESPAWN;
	else
		rltMsg.m_lConfirmResult = RE_SKILL_DENY_SKILL_RESPAWN;

	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_RLT_SKILL_RESPAWN, &rltMsg);

	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
//--------------------------------------------------------
void LC_ClientGamePlayer::RequestModifyPKSetting()
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	if(m_kPKRecorder.IsJuniorProtected())
	{
		SendLogicInfoMessage(RE_JUNIOR_CANT_MODIFY_PK_SETTING);
		m_kPKRecorder.SetDefault();
		return;
	}

	MG_REQ_ModifyPKMode reqMsg;
	reqMsg.m_lPlayerID = GetID();
	reqMsg.m_lpkmode	= m_kPKRecorder.GetPKMode();
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_MODIFY_PK_SETTING, &reqMsg);

	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	//SendLogicInfoMessage(RE_PKSETTINGS_SUCCESS);

	SetDirtyFlag(DIRTY_FLAG_ATTRIBUTE, true);
	AddDirtyFlagRevisionID(DIRTY_FLAG_ATTRIBUTE);
}
//-------------------------------------------------------
void LC_ClientGamePlayer::RequestModifyPKSettingForce()
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_ModifyPKMode reqMsg;
	reqMsg.m_lPlayerID = GetID();
	reqMsg.m_lpkmode	= m_kPKRecorder.GetPKMode();
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_MODIFY_PK_SETTING, &reqMsg);

	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	SetDirtyFlag(DIRTY_FLAG_ATTRIBUTE, true);
	AddDirtyFlagRevisionID(DIRTY_FLAG_ATTRIBUTE);
}
//-------------------------------------------------------
void LC_ClientGamePlayer::RequestPKValue()
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_GetPKValue reqMsg;
	reqMsg.m_lPlayerID = GetID();
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GET_PK_VALUE, &reqMsg);

	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
//-------------------------------------------------------
void LC_ClientGamePlayer::RequestAgreeAddToEnermy(unique_id_type lPlayerID)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_AddEnemy reqMsg;
	reqMsg.m_lTargetCharID	= lPlayerID;

	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_ADD_ENEMY, &reqMsg);
	if(bCreateMsg)
	{
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}
//-------------------------------------------------------
void LC_ClientGamePlayer::SetChatTargetID( unique_id_type lID )
{
	m_lChatTargetID = lID;
}
//-------------------------------------------------------
PrivateChatSaveSt& LC_ClientGamePlayer::GetSearchingChatTarget()
{
	return m_stPrivateChatSaveSt;
}
//-------------------------------------------------------
bool LC_ClientGamePlayer::IsSearchingChatName(const StringType& name)
{
	return (m_stPrivateChatSaveSt.targetName == name);
}
//-------------------------------------------------------
void LC_ClientGamePlayer::EraseSearchingChatName()
{
	m_stPrivateChatSaveSt.Clear();
}
//-------------------------------------------------------
void LC_ClientGamePlayer::SetChatTargetName(const StringType& strName )
{
	m_strCharTargetName = strName;
}
//-------------------------------------------------------
StringType LC_ClientGamePlayer::GetChatTargetName()
{
	return m_strCharTargetName;
}
//-------------------------------------------------------
bool LC_ClientGamePlayer::SetChatChannel( int iChatChannel )
{
	if (iChatChannel == CCT_GROUP&&GetPlayerGroupID() <= 0)
	{
		SendLogicInfoMessage(RE_CHAT_GROUP_DISMISS);
		return false;
	}

	if (iChatChannel == CCT_GUILD && !IsExistGuild())
	{
		SendLogicInfoMessage(RE_CHAT_GUILD_DISMISS);
		return false;
	}

	m_iCurrentChatChannel = iChatChannel;
	return true;
}
//-------------------------------------------------------
unique_id_type LC_ClientGamePlayer::GetChatTargetID()
{
	return m_lChatTargetID;
}
//-------------------------------------------------------
int LC_ClientGamePlayer::GetChatChannel()
{
	return m_iCurrentChatChannel;
}
//-------------------------------------------------------
bool LC_ClientGamePlayer::SendChatMsg(const StringType& Msg ,int iTextLength, bool bAutoConsume)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	unique_id_type lChatTargetID = GetChatTargetID();
	/*if (lChatTargetID <= 0)
	{
		SendLogicInfoMessage(RE_CHAT_NO_TARGET);
		return false;
	}*/

	if (lChatTargetID == GetCommanderUID())
	{
		SendLogicInfoMessage(RE_CHAT_SELF);
		return false;
	}

	if (Msg.length() > MAX_CHAT_MESSAGE_SIZE)
	{
		SendLogicInfoMessage(RE_CHAT_MSG_TOO_LARGE);
		return false;
	}

	if(GetChatChannel() == CCT_PRIVATE)
	{
		LC_ClientSocietyAsset *pkSocietyAsset = GetSocietyAsset();
		if(/*pkSocietyAsset &&*/ NULL == pkSocietyAsset->GetFriendInfo1(lChatTargetID))
		{
			SendLogicInfoMessage(RE_CHAT_PRIVATE_FAIL_NOT_FRIEND);
			return false;
		}
	}

	bool bVipPrivilege = LC_Helper::GetVipPrivilegeTimesByType(VIP_PC_INFINITE_HORM,this) > 0 && GetVIPAsset().GetPrivilegeTalkTimes() < 20;
	if (GetChatChannel() == CCT_TRUMPET )
	{
		/*if (!m_kPackAsset.IsExistItem(PACK_TYPE_BASIC,TRUMPET_ITEM_ID) && !bAutoConsume && !bVipPrivilege)
		{
			SendLogicInfoMessage(RE_CHAT_NO_TRUMPET);
			return false;
		}*/

		if (iTextLength >60)
		{
			SendLogicInfoMessage(RE_CHAT_MSG_TOO_LARGE);
			return false;
		}
	}


	if (GetChatChannel() == CCT_SUPER_TRUMPET )
	{
		if (!m_kPackAsset.IsExistItem(PACK_TYPE_BASIC,SUPER_TRUMPET_ITEM_ID) && !bAutoConsume )
		{
			SendLogicInfoMessage(RE_CHAT_NO_SUPER_TRUMPET);
			return false;
		}

		if (iTextLength >60)
		{
			SendLogicInfoMessage(RE_CHAT_MSG_TOO_LARGE);
			return false;
		}
	}

	//if (GetChatChannel() == CCT_WORLD)
	//{
	//	if (GetLevel() < 15)
	//	{
	//		SendLogicInfoMessage(RE_CHAT_LEVEL_NOT_ENOUGH);
	//		return false;
	//	}
	//	/*else if (GetMP() < WORLD_CHAT_COST_MP)
	//	{
	//		SendLogicInfoMessage(RE_CHAT_MP_NOT_ENOUGH);
	//		return false;
	//	}*/
	//}

	/*LC_Helper::Trim(Msg);

	StringType sTmp = Msg;
	LC_Helper::ClearTag(sTmp, "(@", ")");
	if (sTmp.empty())
	{
		SendLogicInfoMessage(RE_CHAT_MSG_EMPTY);
		return false;
	}*/

	//if (!m_kChatChannelController.CheckChannelSendFrequency(GetChatChannel(), GetLevel(), GET_PROCESS_TIME))
	float fCurrentTime = GET_PROCESS_TIME;
	if (!m_kChatChannelController.CheckChannelSendFrequency(GetChatChannel(), GetCommanderLevel(), fCurrentTime))
	{
		_checkChatMsgSendFrequency(GetChatChannel(), fCurrentTime);
		return false;
	}

	// 	if (Msg == "变身")
	// 	{
	// 		int lCharTypeID = 41111045;
	// 		RequestShapeShift(lCharTypeID);
	// 	}
	// 
	// 	if (Msg == "替身术")
	// 	{
	// 		RequestShapeShift(41511038);
	// 	}
	// 
	// 	if (Msg == "代表月亮消灭你")
	// 	{
	// 		RequestShapeShift(m_lCharType);
	// 	}


	////检查是否有小喇叭
	//if(NULL == GUISys::getRichTextParse)
	//	return false;

	//GUISys::getRichTextParse->FilterForbiddenText(Msg, Msg, TT_TextFilter::TFT_CHAT);

	MG_REQ_RequestChat reqMsg;
	reqMsg.m_lCharID = GetCommanderUID();
	reqMsg.m_iChatChannel = (int8_t)GetChatChannel();
	reqMsg.m_lTargetCharID = lChatTargetID;
	reqMsg.m_autoConsume = bAutoConsume;
	TPSTR2STDSTR(Msg, reqMsg.m_strChatContent);
	TPSTR2STDSTR(m_strCharTargetName, reqMsg.m_strTargetPlayerName);

	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_CHAT_REQUEST, &reqMsg);
	if(bCreateMsg)
	{
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
	m_kChatChannelController.SetLastSendChatMsgTime(GetChatChannel(),fCurrentTime);
	return true;
}
//-------------------------------------------------------
void LC_ClientGamePlayer::_checkChatMsgSendFrequency(int iChannelType, float fCurrentTime)
{
	if (iChannelType > CCT_MAX_SETTABLE_CHANNEL_COUNT && iChannelType < CCT_MAX_CHANNEL_COUNT)
	{
		return;
	}

	if (iChannelType >= CCT_MAX_SETTABLE_CHANNEL_COUNT || iChannelType < 0)
	{
		return;
	}

	//int iPlayerLevel = GetLevel();
	int iPlayerLevel = GetCommanderLevel();
	bool bLevelLmt = false;
	int iLastChaneel = 0;
	CSVFile::CF_ChatCooldown::DataEntryMapExternIterator kIter = CSVFile::CF_ChatCooldown::GetSingletonPtr()->GetIterator();
	while(kIter.HasMoreElements())
	{
		const CSVFile::CF_ChatCooldown::DataEntry* pkEntry = kIter.PeekNextValuePtr();
		float fLastSendTime = m_kChatChannelController.GetLastSendChatMsgTime(iChannelType);
		bool bLevel = (iPlayerLevel >= pkEntry->_iMinLevel || pkEntry->_iMinLevel == 0) && (iPlayerLevel <= pkEntry->_iMaxLevel || pkEntry->_iMaxLevel == 0);
		bool bTime = (pkEntry->_iCdTime >= 0 && (fCurrentTime - fLastSendTime > pkEntry->_iCdTime));
		if(pkEntry->_iChannel == iChannelType)
		{
			if(bLevel || bLevelLmt)
			{
				if(pkEntry->_iCdTime == -1 || bLevelLmt)
				{
					if(!bLevelLmt)
					{
						bLevelLmt = true;
						iLastChaneel = pkEntry->_iChannel;
					}
					else if(iLastChaneel == pkEntry->_iChannel)
					{
						//不可发言
						if(pkEntry->_iChannel == CCT_PRIVATE)
							SendLogicInfoMessage(RE_CHAT_PRIVATE_REQ_FREQUENTLY_LOW_LEVEL, pkEntry->_iMinLevel);
						else
							SendLogicInfoMessage(RE_CHAT_REQ_FREQUENTLY_LOW_LEVEL, pkEntry->_iMinLevel);

						return;
					}
				}
				else if(!bTime)
				{
					int iLeftTime = UT_MathBase::Floor((pkEntry->_iCdTime - (fCurrentTime - fLastSendTime)));
					if(iLeftTime > 0)
					{
						//聊天CD未到
						SendLogicInfoMessage(RE_CHAT_REQ_FREQUENTLY_CD, iLeftTime);
						return;
					}
				}
			}

			if(bLevel && bTime)
				return;
		}
		kIter.MoveNext();
	}
}
//-------------------------------------------------------
bool LC_ClientGamePlayer::SendSavePrivateChatMsg()
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	unique_id_type lChatTargetID =  GetSearchingChatTarget().targetID;
	const StringType & Msg = GetSearchingChatTarget().msgtxt;
	int channel = GetSearchingChatTarget().channel;
	if (lChatTargetID <= 0)
	{
		SendLogicInfoMessage(RE_CHAT_NO_TARGET);
		return false;
	}

	if (lChatTargetID == GetCommanderUID())
	{
		SendLogicInfoMessage(RE_CHAT_SELF);
		return false;
	}

	if (Msg.length() > MAX_CHAT_MESSAGE_SIZE)
	{
		SendLogicInfoMessage(RE_CHAT_MSG_TOO_LARGE);
		return false;
	}

	StringType sTmp = Msg;
	UT_BaseStringUtility::Trim(sTmp);

	LC_Helper::ClearTag(sTmp, "(@", ")");
	if (sTmp.empty())
	{
		SendLogicInfoMessage(RE_CHAT_MSG_EMPTY);
		return false;
	}

	//if (!m_kChatChannelController.CheckChannelSendFrequency(GetChatChannel(), GetLevel(), GET_PROCESS_TIME))
	float fCurrentTime = GET_PROCESS_TIME;
	if (!m_kChatChannelController.CheckChannelSendFrequency(GetChatChannel(), GetCommanderLevel(), fCurrentTime))
	{
		_checkChatMsgSendFrequency(GetChatChannel(), fCurrentTime);
		return false;
	}

	MG_REQ_RequestChat reqMsg;
	reqMsg.m_lCharID = GetCommanderUID();
	reqMsg.m_iChatChannel = (uint8_t)channel;
	reqMsg.m_lTargetCharID = lChatTargetID;
	TPSTR2STDSTR(Msg, reqMsg.m_strChatContent);
	TPSTR2STDSTR(GetSearchingChatTarget().targetName, reqMsg.m_strTargetPlayerName);
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_CHAT_REQUEST, &reqMsg);
	if(bCreateMsg)
	{
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
	m_kChatChannelController.SetLastSendChatMsgTime(channel,fCurrentTime);
	return true;
}
//-------------------------------------------------------
bool LC_ClientGamePlayer::SendFriendChatMsg(unique_id_type lCharID, const StringType& strMsg)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	StringType strMsgTemp = strMsg;

	UT_BaseStringUtility::Trim(strMsgTemp);
	if (strMsgTemp.empty())
	{
		SendLogicInfoMessage(RE_CHAT_MSG_EMPTY);
		return false;
	}

	//检查是否有小喇叭
	TT_TextFilter* pkTextFilter = TT_TextFilter::GetSingletonPtr();
	if (pkTextFilter)
	{
		pkTextFilter->SetText(strMsgTemp);
		pkTextFilter->FindAndFilterForbiddenText(TT_TextFilter::TFT_CHAT);
		strMsgTemp = pkTextFilter->GetText();
	}

	if(strMsgTemp.length() >= MAX_CHAT_MESSAGE_SIZE)
	{
		SendLogicInfoMessage(RE_CHAT_MSG_TOO_LARGE);
		return false;
	}

	MG_REQ_RequestChat reqMsg;
	reqMsg.m_lCharID = GetCommanderUID();
	reqMsg.m_iChatChannel = CCT_FRIEND;
	reqMsg.m_lTargetCharID = lCharID;
	TPSTR2STDSTR(strMsgTemp, reqMsg.m_strChatContent);

	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_CHAT_REQUEST, &reqMsg);
	if(bCreateMsg)
	{
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
	return true;
}
//-------------------------------------------------------
void LC_ClientGamePlayer::SetChannelLimitFlag( int iChannelType,bool bFlag )
{
	m_kChatChannelController.SetChatChannelFlag(iChannelType,bFlag);
}
//-------------------------------------------------------
void LC_ClientGamePlayer::ConfirmChannelLimitFlag()
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_UPdateChatChannel reqMsg;
	reqMsg.m_lCharID = GetCommanderUID();
	for (int i = 0;i < MAX_SETTABLE_CHANNEL_NUM;i++)
	{
		reqMsg.m_bChannel[i] = m_kChatChannelController.GetChatChannelFlag(i);
	}

	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_UPDATE_CHANNEL, &reqMsg);
	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
void LC_ClientGamePlayer::ReqPlayerMove()
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_Sync_GamePlayerMove msg;

	msg.m_iPlayerID			= GetID();
	msg.m_lMapID			= GetMapLogicID();
	msg.m_lCurrentLocationX = m_kCurrentLocation.x;
	msg.m_lCurrentLocationY = m_kCurrentLocation.y;
	msg.m_lCurrentLocationZ = m_kCurrentLocation.z;

	msg.m_lTargetLocationX	= m_kMoveLineTargetLocation.x;
	msg.m_lTargetLocationY  = m_kMoveLineTargetLocation.y;

	msg.m_lMoveSpeed		= GetMoveSpeed();
	//msg.m_fMoveAccSpeed		= this->GetMoveAccSpeed();
	//msg.m_iVerticalOffset	= this->GetVerticalOffset();
	//msg.m_fVerticalSpeed	= this->GetVerticalSpeed();
	//msg.m_fGravitySpeed		= this->GetGravitySpeed();

	msg.m_sCurrentDirAngle	= (int16_t)this->GetForwardDirAngle();

	//msg.m_lPlayerState = m_iCurrentState;
	msg.m_fClientTime = m_fLastSyncMoveTime;
	msg.m_nActionMode = m_iAnimIndex;
	//GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "SyncToServer: ID: %d; Cur: %d %d; Tgt: %d %d;", GetID(), m_kCurrentLocation.x, m_kCurrentLocation.y, m_kMoveLineTargetLocation.x, m_kMoveLineTargetLocation.y);
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_SYNC_GAME_PLAYER_MOVE, &msg);
	if (bCreateMsg && m_fLastSyncMoveTime - GetTranToTranTick() > 0.5f)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
//--------------------------------------------------------
void LC_ClientGamePlayer::_syncToServer( float fCurrentTime )
{
	T_PROFILE_EX("LC_ClientGamePlayer::_syncToServer",4.0f);
	if( fCurrentTime - m_fLastSyncMoveTime > 0.125f )
	{
		m_fLastSyncMoveTime = fCurrentTime;

		if (GetBExecuteSkill())
		{
			SetBExecuteSkill(false);
			return;
		}

		int iMapLogicId = GetMapLogicID();
		int curSpeed = GetMoveSpeed();
		int16_t curDirAngle =  (int16_t)this->GetForwardDirAngle();
		if(m_kLastLocation.Test(iMapLogicId, m_kCurrentLocation.x, m_kCurrentLocation.y) && UT_MathBase::AbsInt(m_LastDirAngle - curDirAngle) <= 10)
		{
			return;
		}

		m_kLastLocation.Update(iMapLogicId, m_kCurrentLocation.x, m_kCurrentLocation.y);
		m_lLastMoveSpeed = curSpeed;
		m_LastDirAngle = curDirAngle;

		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = false;

		MG_Sync_GamePlayerMove msg;

		msg.m_iPlayerID			= GetID();
		msg.m_lMapID			= iMapLogicId;
		msg.m_lCurrentLocationX = m_kCurrentLocation.x;
		msg.m_lCurrentLocationY = m_kCurrentLocation.y;
		msg.m_lCurrentLocationZ = m_kCurrentLocation.z;

		msg.m_lTargetLocationX	= m_kMoveLineTargetLocation.x;
		msg.m_lTargetLocationY  = m_kMoveLineTargetLocation.y;

		msg.m_lMoveSpeed		= curSpeed;
		//msg.m_fMoveAccSpeed		= this->GetMoveAccSpeed();
		//msg.m_iVerticalOffset	= this->GetVerticalOffset();
		//msg.m_fVerticalSpeed	= this->GetVerticalSpeed();
		//msg.m_fGravitySpeed		= this->GetGravitySpeed();

		msg.m_sCurrentDirAngle	= curDirAngle;

		//msg.m_lPlayerState = m_iCurrentState;
		msg.m_fClientTime = fCurrentTime;
		msg.m_nActionMode = m_iAnimIndex;
		//GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "SyncToServer: ID: %d; Cur: %d %d; Tgt: %d %d;", GetID(), m_kCurrentLocation.x, m_kCurrentLocation.y, m_kMoveLineTargetLocation.x, m_kMoveLineTargetLocation.y);
		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_SYNC_GAME_PLAYER_MOVE, &msg);
		if (bCreateMsg && fCurrentTime - GetTranToTranTick() > 0.5f)
			m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}

	if(fCurrentTime - m_fLastSyncUIOperationTime > 300.0f)
	{
		m_fLastSyncUIOperationTime = fCurrentTime;

		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = false;

		MG_GC_Sync_UIOperation  msg;
		memcpy(msg.m_auiOperationRecords,m_auiUIOperationRecords,sizeof(uint16_t) * 64);

		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_SYNC_UI_OPERATION, &msg);
		if(bCreateMsg)
			m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

		ClearUIOperationRecords();
	}
}
//---------------------------------------------------------------------------------
void LC_ClientGamePlayer::_updateLogicAreaInfo()
{
	int lMapResID = GetMapResID();
	int lMapLogicID = GetMapLogicID();
	LC_LogicAreaInfo* kLogicAreaInfo = LC_LogicMapManager::GetSingletonPtr()->QueryAreaInfo(m_kCurrentLocation,lMapResID);
	if (kLogicAreaInfo == NULL)
		return;

	int areaFuncID		= kLogicAreaInfo->lLogicAreaFuncID;
	int areaID			= kLogicAreaInfo->lLogicAreaID;
	StringType & OriginAreaName	= kLogicAreaInfo->kOriginAreaName;
	StringType & areaName		= kLogicAreaInfo->kAreaName;

	CF_LogicAreaFunc::DataEntry* pkNewLogicAreaFunc = CSVHelper::GetLogicAreaFuncData(areaFuncID);
	if (NULL != pkNewLogicAreaFunc)
	{
		m_pkLogicAreaFunc = pkNewLogicAreaFunc;

		if (m_lCurrentMapResID != lMapResID || m_lCurrentLogicAreaID != areaID || m_lCurrentMapLogicID != lMapLogicID)
		{
			m_lCurrentMapResID = lMapResID;
			m_lCurrentMapLogicID = lMapLogicID;
			m_lCurrentLogicAreaID = areaID;

			if ( m_lCurrentLogicAreaName != areaName )
				SendLogicInfoMessage(RE_CHANGE_LOGIC_AREA,areaName);

			m_lCurrentLogicAreaName = areaName;

			SendMessage(CreateMsg_LOGIC_AREA_SOUND_NAME(m_lCurrentLogicAreaID, m_lCurrentMapResID), true);

			//////////////////////////////////////////////////////////////////////////

			NW_ClientSessionManager* pkSessionManager = NW_ClientSessionManager::GetSingletonPtr();
			int iServerID = pkSessionManager->GetGameSessionGroup()->GetCurGameServer();

			int iMyServerFlag = iServerID == GetServerID() ? 1 : 0;

			int lAreaType = pkNewLogicAreaFunc->_lAreaType;
			int iAllianceFlag = LC_Helper::IsAllianceMap(lMapResID) ? 1 : 0;

			StringStreamType sStream("");
			sStream << OriginAreaName << "#" << lMapResID << "#" << lAreaType << "#" << lMapLogicID << "#" << GetMapLogicType() << "#" << iAllianceFlag << "#" << iServerID << "#" << iMyServerFlag << "#";
			UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
			pkUIManager->HandlerMsgGameEvent(GLET_CLIENT_SHOW_AREA_NAME_INFO, sStream.str());
			if (AT_SAFE_AREA == lAreaType || AT_NON_AREA == lAreaType )
			{
				SendWorldNameInfoMessage(RE_SAFE_AREA_NAME,areaName,SAFE_AREA_TEXT,lMapResID,lAreaType,lMapLogicID);
			}
			else if (AT_FIGHT_AREA == lAreaType)
			{
				SendWorldNameInfoMessage(RE_FIGHT_AREA_NAME,areaName,NO_SAFE_AREA_TEXT,lMapResID,lAreaType,lMapLogicID);
			}
		}
	}
}

void LC_ClientGamePlayer::_notifyInGameToServer()
{
	if (m_NotifyInGame < 0)
		return;
	else if (m_NotifyInGame >= g_settings.DS_INGAME_DELETA_ZHENSHU)
	{
		NotifyServerAlreadyInGame();
		m_NotifyInGame = -1;
	}
	else
		m_NotifyInGame++;
}

//--------------------------------------------------------
void LC_ClientGamePlayer::_updateWatchedAndLockedTarget()
{
#ifndef _MAP_VIEWER_BUILD
	object_id_type lCurrentWatchedID = GetWatchTargetID();
	object_id_type lCurrentLockedID =  GetPreLockedTargetID();
	IM_Manager* pkInteractManager	= IM_Manager::GetSingletonPtr();

	/*bool bFightStatus = GetFightStatus();
	bool bIsFightStatusTimerStart = IsFightStatusTimerStart();
	float fCurrentTime = GET_PROCESS_TIME;

	if(bFightStatus)
	{
	if(lCurrentWatchedID == 0 || IS_NPC_ID(lCurrentWatchedID))
	{
	if(!bIsFightStatusTimerStart)
	StartFightStatusTimer(true,fCurrentTime);
	}

	if(IS_MONSTER_ID(lCurrentWatchedID) && bIsFightStatusTimerStart)
	{
	StartFightStatusTimer(false,0.0f);
	bIsFightStatusTimerStart = false;
	}
	}
	else
	{
	if(IS_MONSTER_ID(lCurrentWatchedID))
	{
	SetFightStatus(true);
	StartFightStatusTimer(false,0.0f);
	bIsFightStatusTimerStart = false;
	SendMessage(CreateMsg_UE_ENTER_FIGHT(GetID()));
	}
	}

	if(bIsFightStatusTimerStart)
	{
	float fStartTime = GetFightStatusTimer();
	if(fCurrentTime > fStartTime + 6.0f)
	{
	SetFightStatus(false);
	StartFightStatusTimer(false,0.0f);
	SendMessage(CreateMsg_UE_LEAVE_FIGHT(GetID()));
	}
	}*/

	GD_ClientActorModel* pkCurrentWatchedActor	= pkInteractManager->GetActor(lCurrentWatchedID);
	if(!pkCurrentWatchedActor || pkCurrentWatchedActor->IsActiveNormalDead() || pkCurrentWatchedActor->IsActiveRagDoll())
		WatchTarget(0);
	else
	{
		NiPoint3 kWatchedTargetLocation = pkCurrentWatchedActor->GetTranslate();
		NiPoint3 kCurrentLocation = UT_EngineMath::ConvertVec3IntToNiPoint3(m_kCurrentLocation);
		float fDistance = UT_EngineMath::LineLengthXYNiPoint(kWatchedTargetLocation,kCurrentLocation);
		
		//如果是失明状态，在失明距离外就无法Watch和Lock目标
		if ( GetSkillFlag(SK_BLIND_EYE) &&  fDistance > BLIND_DISTANCE)
		{
			WatchTarget(0);
		}
		
		//if(fDistance > 1500.0f)
		/*
		if(IS_NPC_ID(lCurrentWatchedID))
		{
		LC_ClientWorldManager* pkWorldManager = CLIENT_GET_WORLD_MANAGER();
		LC_ClientNPC* pNpc = (LC_ClientNPC*)(pkWorldManager->FindObject(lCurrentWatchedID));
		if (pNpc && pNpc->IsFunctionNPC())
		{
		if(fDistance >= g_settings.PROFILE_TALK_DISTANCE)
		WatchTarget(0);
		}
		else if(fDistance >= g_settings.PROFILE_NPC_SELECTED_DISTANCE)
		WatchTarget(0);
		}
		else
		*/
		if(fDistance >= g_settings.PROFILE_NPC_SELECTED_DISTANCE)
			WatchTarget(0);
	}

	if(!m_pkPendingSkill && !IS_SKILL_TYPE(m_iCurrentState))
	{
		//SetLockedTargetID(GetWatchTargetID());
	}


	if(IS_PLAYER_CLASS_ID(lCurrentWatchedID) || IS_NPC_ID(lCurrentWatchedID))
	{
		LC_ActorBase* pkWatchedActor = (LC_ActorBase* )(m_pkMap->FindObject(lCurrentWatchedID));

		//找不到目标
		if (NULL == pkWatchedActor)
		{
			WatchTarget(0);
			SetLockedTargetID(0);
			ClearPendingSkill();
		}
		else if(pkWatchedActor->GetID() != GetID())
		{
			bool bCanWatch = true;

			if(IS_PLAYER_CLASS_ID(lCurrentWatchedID))
			{
				if(!((LC_ClientNetPlayer*)pkWatchedActor)->GetVisibleToGamePlayer())
					bCanWatch = false;
			}

			if(IS_NPC_ID(lCurrentWatchedID))
			{
				if(!((LC_ClientNPC*)pkWatchedActor)->GetVisibleToGamePlayer())
					bCanWatch = false;

				if(!((LC_ClientNPC*)pkWatchedActor)->GetProperty_CanPick())
					bCanWatch = false;
			}

			if (!bCanWatch)
			{
				WatchTarget(0);
				SetLockedTargetID(0);
				ClearPendingSkill();
			}
		}
	}

	if( IS_NPC_ID(lCurrentLockedID) || IS_PLAYER_CLASS_ID(lCurrentLockedID) )
	{
		//找不到锁定目标清除
		LC_ActorBase* pkLockedActor = (LC_ActorBase* )(m_pkMap->FindObject(lCurrentLockedID));
		if(!pkLockedActor)
		{
			FreezeLocked(false);
			SetPreLockedTargetID(0);
			SetLButtonPickActorID(0);
			SetLockedTargetID(GetPreLockedTargetID());
		}
		else if( lCurrentLockedID != GetID())
		{
			//锁定目标死亡，锁定目标临死状态，超过距离清除
			int distance = UT_MathBase::LineLengthXYInt(pkLockedActor->GetCurrentLocation(), GetCurrentLocation());
			if ( (distance > LOCKED_DISTANCE_OVER || pkLockedActor->GetDead() || pkLockedActor->GetDying()) )
			{
				FreezeLocked(false);
				SetPreLockedTargetID(0);
				SetLButtonPickActorID(0);
				SetLockedTargetID(GetPreLockedTargetID());
			}
		}
	}

#endif
}
//--------------------------------------------------------
void LC_ClientGamePlayer::_updateTargetSelectEffect()
{
	LC_FactionManager* pkFactionManager = LC_FactionManager::GetSingletonPtr();
	if (NULL == pkFactionManager)
	{
		return;
	}

	IM_Manager* pkIMManager = IM_Manager::GetSingletonPtr();
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	GD_ModelBase* pkTargetModel = NULL;
	GD_ClientActorModel* pkClientTargetModel = NULL;
	FX_ClientFxManager* pkFXManager = FX_ClientFxManager::GetSingletonPtr();

	if(m_bNeedLockLocation)
	{
		if(m_iLockLocationFXID == 0)
		{
			m_iLockLocationFXID = pkFXManager->CreateFx(RES_FX_LOCK_LOCATION,false);
			pkFXManager->PlayFx(m_iLockLocationFXID,GET_PROCESS_TIME);
		}

		pkFXManager->BindFxToLocation(m_iLockLocationFXID,m_kMouseOverLocation);

		if(pkIMManager)
			pkIMManager->SetPickSwitch(false);
		if(pkUIManager)
			pkUIManager->SetHandleMouseMessage(false);
	}
	else
	{
		if(m_iLockLocationFXID != 0)
		{
			pkFXManager->ReleaseFx(m_iLockLocationFXID);
			m_iLockLocationFXID = 0;
		}

		if(pkIMManager)
			pkIMManager->SetPickSwitch(true);
		if(pkUIManager)
			pkUIManager->SetHandleMouseMessage(true);
	}
	//SK_SkillExecute* pkSkillExecute = GetWaitPendingSkillExecute();
	//if(m_bNeedLockLocation && NULL != pkSkillExecute)
	//{
	//	int iLocationFxType = pkSkillExecute->GetProperty_LockLocationFxType();
	//	//特效资源
	//	StringType kFxName = "";
	//	switch (iLocationFxType)
	//	{
	//	case 1:
	//		kFxName = "O_功能_宽1米长8米1.gfx";
	//		break;
	//	case 2:
	//		kFxName = "O_功能_宽1米长12米1.gfx";
	//		break;
	//	case 3:
	//		kFxName = "O_功能_90度6米1.gfx";
	//		break;
	//	case 4:
	//		kFxName = "O_功能_360度2.5米1.gfx";
	//		break;
	//	default:
	//		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "更新地点选择特效错误，无效的地点锁定类型[%d]", iLocationFxType);
	//		break;
	//	}

	//	//需要改变特效
	//	if (iLocationFxType > 0 && m_iLockLocationFxType > 0 && iLocationFxType != m_iLockLocationFxType)
	//	{
	//		//销毁原有特效
	//		if (m_iLockLocationFXID > 0)
	//		{
	//			pkFXManager->ReleaseFx(m_iLockLocationFXID);
	//			m_iLockLocationFXID = 0;
	//		}
	//		else
	//		{
	//			GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "更新地点选择特效逻辑错误");
	//		}

	//		//产生新特效
	//		m_iLockLocationFXID = pkFXManager->CreateFx(kFxName,false);
	//		m_iLockLocationFxType = iLocationFxType;
	//		pkFXManager->PlayFx(m_iLockLocationFXID,GET_PROCESS_TIME);

	//	}
	//	else if (iLocationFxType > 0 && m_iLockLocationFxType <= 0)	//需要产生特效
	//	{
	//		if (m_iLockLocationFXID <= 0)
	//		{
	//			m_iLockLocationFXID = pkFXManager->CreateFx(kFxName,false);
	//			m_iLockLocationFxType = iLocationFxType;
	//			pkFXManager->PlayFx(m_iLockLocationFXID,GET_PROCESS_TIME);
	//		}
	//		else
	//		{
	//			GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "更新地点选择特效逻辑错误");
	//		}
	//	}

	//	switch (iLocationFxType)
	//	{
	//	case 1:
	//	case 2:
	//	case 3:
	//		{
	//			GD_ClientActorModel* pkGamePlayerModel = pkIMManager->GetActor(GetID());
	//			NiPoint3 kFxBindLocation = pkGamePlayerModel->GetFxDummyNodePosition(GD_ModelBase::FDT_GROUND);
	//			NiPoint3 kFxDir = kFxBindLocation - m_kMouseOverLocation;
	//			pkFXManager->BindFxToLocation(m_iLockLocationFXID,kFxBindLocation, &kFxDir);
	//		}
	//		break;
	//	case 4:
	//		pkFXManager->BindFxToLocation(m_iLockLocationFXID,m_kMouseOverLocation);
	//		break;
	//	}

	//	if(pkIMManager)
	//		pkIMManager->SetPickSwitch(false);
	//	if(pkUIManager)
	//		pkUIManager->SetHandleMouseMessage(false);
	//}
	//else
	//{
	//	if(m_iLockLocationFXID != 0)
	//	{
	//		pkFXManager->ReleaseFx(m_iLockLocationFXID);
	//		m_iLockLocationFXID = 0;
	//		m_iLockLocationFxType = 0;
	//	}

	//	if(pkIMManager)
	//		pkIMManager->SetPickSwitch(true);
	//	if(pkUIManager)
	//		pkUIManager->SetHandleMouseMessage(true);
	//}

	object_id_type lCurrentLocked = GetPreLockedTargetID();
	//if ( GetPreLockedTargetID() != 0) 
	//{
	//	lCurrentLocked = 
	//}

	if(m_lLastMouseOverActorID != m_lMouseOverActorID &&
		m_lLastMouseOverActorID != m_lWatchTargetID &&
		m_lLastMouseOverActorID != lCurrentLocked )
	{
		pkTargetModel = pkIMManager->GetModel(m_lLastMouseOverActorID);
		if(pkTargetModel)
		{
			if(IS_PLAYER_CLASS_ID(m_lLastMouseOverActorID) && m_lLastMouseOverActorID != m_iID)	
				SendMessage( CreateMsg_UE_UPDATE_PLAYER_BLOOD(m_lLastMouseOverActorID, -1) );

			else if(IS_NPC_ID(m_lLastMouseOverActorID))	
				SendMessage( CreateMsg_UE_UPDATE_MONSTER_BLOOD(m_lLastMouseOverActorID, -1) );

			else if(IS_CHEST_ID(m_lLastMouseOverActorID))
			{
				LC_ClientPlaceHolderManager* pkChestManager = CLIENT_GET_PLACEHOLDER_MANAGER();
				if ( pkChestManager )
				{
					LC_ClientPlaceHolder* phb = (LC_ClientPlaceHolder*)pkChestManager->FindPlaceHolder(m_lLastMouseOverActorID);
					if ( phb )
					{
						if ( phb->IsOwner(this->GetMainControllerID()) ) 
							SendMessage(CreateMsg_UE_NAME_FRAME_STATE_NORMAL(m_lLastMouseOverActorID));
						else
							SendMessage(CreateMsg_UE_DETIME_DROPITEM_NAME(m_lLastMouseOverActorID));
					}
				}
			}
		}
	}

	if(/*m_lLastLockTargetID != m_lMouseOverActorID &&
		m_lLastLockTargetID != m_lWatchTargetID &&*/
		m_lLastLockTargetID != lCurrentLocked )
	{
		pkTargetModel = pkIMManager->GetModel(m_lLastLockTargetID);
		if(pkTargetModel)
		{
			if(IS_PLAYER_CLASS_ID(m_lLastLockTargetID) && m_lLastLockTargetID != m_iID)	
				SendMessage( CreateMsg_UE_UPDATE_PLAYER_BLOOD(m_lLastLockTargetID, -1) );

			else if(IS_NPC_ID(m_lLastLockTargetID))	
				SendMessage( CreateMsg_UE_UPDATE_MONSTER_BLOOD(m_lLastLockTargetID, -1) );

			else if(IS_CHEST_ID(m_lLastLockTargetID))
				SendMessage(CreateMsg_UE_NAME_FRAME_STATE_NORMAL(m_lLastLockTargetID));
		}
	}

	if(m_lLastWatchTargetID != m_lMouseOverActorID &&
		m_lLastWatchTargetID != m_lWatchTargetID &&
		m_lLastWatchTargetID != lCurrentLocked )
	{
		pkTargetModel = pkIMManager->GetModel(m_lLastWatchTargetID);
		if(pkTargetModel)
		{
			if(IS_PLAYER_CLASS_ID(m_lLastWatchTargetID) && m_lLastWatchTargetID != m_iID)
				SendMessage( CreateMsg_UE_UPDATE_PLAYER_BLOOD(m_lLastWatchTargetID, -1) );

			else if(IS_NPC_ID(m_lLastWatchTargetID))
				SendMessage( CreateMsg_UE_UPDATE_MONSTER_BLOOD(m_lLastWatchTargetID, -1));

			else if(IS_CHEST_ID(m_lLastWatchTargetID))
				SendMessage(CreateMsg_UE_NAME_FRAME_STATE_NORMAL(m_lLastWatchTargetID));
		}
	}

	if(m_lMouseOverActorID != m_lLastMouseOverActorID &&
		m_lMouseOverActorID != m_lLastWatchTargetID &&
		m_lMouseOverActorID != m_lLastLockTargetID)
	{
		pkTargetModel = pkIMManager->GetModel(m_lMouseOverActorID);
		if(pkTargetModel)
		{
			if(IS_PLAYER_CLASS_ID(m_lMouseOverActorID))
				SendMessage( CreateMsg_UE_UPDATE_PLAYER_BLOOD(m_lMouseOverActorID, 1) );

			else if(IS_NPC_ID(m_lMouseOverActorID))
				SendMessage( CreateMsg_UE_UPDATE_MONSTER_BLOOD(m_lMouseOverActorID, 1));

			else if(IS_CHEST_ID(m_lMouseOverActorID))
			{
				LC_ClientPlaceHolderManager* pkChestManager = CLIENT_GET_PLACEHOLDER_MANAGER();
				if ( pkChestManager )
				{
					LC_ClientPlaceHolder* phb = (LC_ClientPlaceHolder*)pkChestManager->FindPlaceHolder(m_lMouseOverActorID);
					if ( phb )
					{
						if ( phb->IsOwner(this->GetMainControllerID()) ) 
							SendMessage( CreateMsg_UE_NAME_FRAME_STATE_HOVER(m_lMouseOverActorID));
						else
							SendMessage(CreateMsg_UE_RESETTIME_DROPITEM_NAME(m_lMouseOverActorID));
					}
				}
			}
		}
	}

	if(/*lCurrentLocked != m_lLastMouseOverActorID &&
		lCurrentLocked != m_lLastWatchTargetID &&*/
		lCurrentLocked != m_lLastLockTargetID)
	{
		pkTargetModel = pkIMManager->GetModel(lCurrentLocked);
		if(pkTargetModel)
		{
			if(IS_PLAYER_CLASS_ID(lCurrentLocked))
				SendMessage( CreateMsg_UE_UPDATE_PLAYER_BLOOD(lCurrentLocked, 1) );

			else if(IS_NPC_ID(lCurrentLocked))
				SendMessage( CreateMsg_UE_UPDATE_MONSTER_BLOOD(lCurrentLocked, 1));

			else if(IS_CHEST_ID(lCurrentLocked))
				SendMessage( CreateMsg_UE_NAME_FRAME_STATE_HOVER(lCurrentLocked));
		}
	}

	//watch目标特效
	//他们说watch的目标不要显示这个光环特效
	/*if( m_lWatchTargetID != m_lLastWatchTargetID  || m_bWatchTargetFactionChange)
	{
		bool bShowSelectionFx = true;

		LC_ActorBase* pkActor = (LC_ActorBase*)(m_pkMap->FindObject(m_lWatchTargetID));
		if(pkActor)
		{
			CF_CharType::DataEntry* pkData = CF_CharType::GetSingletonPtr()->GetEntryPtr(pkActor->GetCharType());
			if(pkData)
				bShowSelectionFx = (pkData->_lShowFlags & CTSF_SELECTFX) != 0;
		}

		UT_CutSceneManager* pkCutSceneManager = UT_CutSceneManager::GetSingletonPtr();
		if (pkCutSceneManager != NULL)
		{
			bShowSelectionFx = pkCutSceneManager->ShowSelectionFx();
		}

		pkTargetModel = pkIMManager->GetModel(m_lLastWatchTargetID);
		pkClientTargetModel = (GD_ClientActorModel*)pkTargetModel;
		if (pkClientTargetModel && (m_lLastWatchTargetID != lCurrentLocked) )
		{
			pkClientTargetModel->DetachFxExt(GD_ClientActorModel::FDT_DYNAMIC_GROUND, RES_FX_CHOOSE_TARGET_RED);
			pkClientTargetModel->DetachFxExt(GD_ClientActorModel::FDT_DYNAMIC_GROUND, RES_FX_CHOOSE_TARGET_GREEN);
			pkClientTargetModel->DetachFxExt(GD_ClientActorModel::FDT_DYNAMIC_GROUND, RES_FX_CHOOSE_TARGET_YELLOW);

			if(IS_PLAYER_CLASS_ID(m_lLastWatchTargetID) && m_lLastWatchTargetID != m_iID)
				SendMessage( CreateMsg_UE_HIDE_PLAYER_BLOOD(m_lLastWatchTargetID) );

			if(IS_NPC_ID(m_lLastWatchTargetID))
				SendMessage( CreateMsg_UE_UPDATE_MONSTER_BLOOD(m_lLastWatchTargetID, -1) );

			else if(IS_CHEST_ID(m_lLastWatchTargetID))
				SendMessage(CreateMsg_UE_NAME_FRAME_STATE_NORMAL(m_lLastWatchTargetID));

		}

		pkTargetModel = pkIMManager->GetModel(m_lWatchTargetID);
		pkClientTargetModel = (GD_ClientActorModel*)pkTargetModel;
		if(pkClientTargetModel && bShowSelectionFx)
		{
			CF_ActorFilterFunc* pkCSVFilterFunc = CF_ActorFilterFunc::GetSingletonPtr();
			CF_ActorFilterFunc::DataEntry* pkData = pkCSVFilterFunc->GetEntryPtr(2);
			bool bCanAttack = (LC_Helper::IsValidTarget(this,m_lWatchTargetID,pkData) == RE_SKILL_CHECK_VALID_TARGET);

			if(IS_NPC_ID(m_lWatchTargetID))
			{
				if(bCanAttack)
					pkClientTargetModel->AttachFxExt(GD_ClientActorModel::FDT_DYNAMIC_GROUND, 0, false, RES_FX_CHOOSE_TARGET_RED, NULL, NULL, -1.0f);
				else
					pkClientTargetModel->AttachFxExt(GD_ClientActorModel::FDT_DYNAMIC_GROUND, 0, false, RES_FX_CHOOSE_TARGET_GREEN, NULL, NULL, -1.0f);
			}
			else if(IS_PLAYER_CLASS_ID(m_lWatchTargetID))
			{
				if(m_lWatchTargetID == m_iID)
				{
					pkClientTargetModel->AttachFxExt(GD_ClientActorModel::FDT_DYNAMIC_GROUND, 0, false, RES_FX_CHOOSE_TARGET_GREEN, NULL, NULL, -1.0f);
				}
				else
				{
					if(bCanAttack)
						pkClientTargetModel->AttachFxExt(GD_ClientActorModel::FDT_DYNAMIC_GROUND, 0, false, RES_FX_CHOOSE_TARGET_RED, NULL, NULL, -1.0f);
					else
						pkClientTargetModel->AttachFxExt(GD_ClientActorModel::FDT_DYNAMIC_GROUND, 0, false, RES_FX_CHOOSE_TARGET_GREEN, NULL, NULL, -1.0f);
				}
			}

			if(IS_PLAYER_CLASS_ID(m_lWatchTargetID))
				SendMessage( CreateMsg_UE_SHOW_PLAYER_BLOOD(m_lWatchTargetID) );

			else if(IS_NPC_ID(m_lWatchTargetID))
				SendMessage( CreateMsg_UE_UPDATE_MONSTER_BLOOD(m_lWatchTargetID, 1));

			else if(IS_CHEST_ID(m_lWatchTargetID))
				SendMessage( CreateMsg_UE_NAME_FRAME_STATE_HOVER(m_lWatchTargetID));

		}
	}*/

	//锁定目标特效
	//为了支持目标脚下光圈特效颜色动态变化，目标可攻击状态变化，也更新
	CF_ActorFilterFunc* pkCSVFilterFunc = CF_ActorFilterFunc::GetSingletonPtr();
	CF_ActorFilterFunc::DataEntry* pkData = pkCSVFilterFunc->GetEntryPtr(2);
	bool bCanAttack = (LC_Helper::IsValidTarget(this,lCurrentLocked,pkData) == RE_SKILL_CHECK_VALID_TARGET);

	/// 锁定目标切换角色，切换模型（三角色情况下）
	Data::GD_ModelBase * pkCurrentLockedTargetModel = lCurrentLocked ? pkIMManager->GetModel(lCurrentLocked) : NULL;
	if (m_pPreLockedTargetModel != NULL)
	{
		if ( m_pPreLockedTargetModel != pkCurrentLockedTargetModel)
		{
			m_pPreLockedTargetModel = NULL;
			m_lLastLockTargetID = 0;	/// 通过下面的逻辑，强制更新
		}
	}

	if( lCurrentLocked != m_lLastLockTargetID || m_lLastLockTargetNeedGfx != (int)bCanAttack)
	{
		bool bShowSelectionFx = true;

		LC_ActorBase* pkActor = (LC_ActorBase*)(m_pkMap->FindObject(lCurrentLocked));
		if(pkActor)
		{
			CF_CharType::DataEntry* pkData = CF_CharType::GetSingletonPtr()->GetEntryPtr(pkActor->GetCharType());
			if(pkData)
				bShowSelectionFx = (pkData->_lShowFlags & CTSF_SELECTFX) != 0;
		}

		UT_CutSceneManager* pkCutSceneManager = UT_CutSceneManager::GetSingletonPtr();
		if (pkCutSceneManager != NULL)
		{
			bShowSelectionFx = pkCutSceneManager->ShowSelectionFx();
		}

		pkTargetModel = pkIMManager->GetModel(m_lLastLockTargetID);
		pkClientTargetModel = (GD_ClientActorModel*)pkTargetModel;
		if (pkClientTargetModel)
		{
			pkClientTargetModel->DetachFxExt(GD_ClientActorModel::FDT_DYNAMIC_GROUND, RES_FX_CHOOSE_TARGET_RED);
			pkClientTargetModel->DetachFxExt(GD_ClientActorModel::FDT_DYNAMIC_GROUND, RES_FX_CHOOSE_TARGET_GREEN);
			pkClientTargetModel->DetachFxExt(GD_ClientActorModel::FDT_DYNAMIC_GROUND, RES_FX_CHOOSE_TARGET_YELLOW);
			pkClientTargetModel->DetachFxExt(GD_ClientActorModel::FDT_DYNAMIC_GROUND, RES_FX_CHOOSE_TARGET_BOSS);

			if(IS_PLAYER_CLASS_ID(m_lLastLockTargetID) && m_lLastLockTargetID != m_iID)
				SendMessage( CreateMsg_UE_HIDE_PLAYER_BLOOD(m_lLastLockTargetID) );

			if(IS_NPC_ID(m_lLastLockTargetID))
				SendMessage( CreateMsg_UE_UPDATE_MONSTER_BLOOD(m_lLastLockTargetID, -1) );

			else if(IS_CHEST_ID(m_lLastLockTargetID))
				SendMessage(CreateMsg_UE_NAME_FRAME_STATE_NORMAL(m_lLastLockTargetID));

		}

		m_pPreLockedTargetModel = pkCurrentLockedTargetModel;
		pkTargetModel = pkCurrentLockedTargetModel;
		pkClientTargetModel = (GD_ClientActorModel*)pkTargetModel;
		if(pkClientTargetModel && bShowSelectionFx)
		{
			CF_ActorFilterFunc* pkCSVFilterFunc = CF_ActorFilterFunc::GetSingletonPtr();
			CF_ActorFilterFunc::DataEntry* pkData = pkCSVFilterFunc->GetEntryPtr(2);
			bool bCanAttack = (LC_Helper::IsValidTarget(this,lCurrentLocked,pkData) == RE_SKILL_CHECK_VALID_TARGET);
			m_lLastLockTargetNeedGfx = bCanAttack;

			if(IS_NPC_ID(lCurrentLocked))
			{
				bool bIsBoss = false;
				if(bCanAttack)
				{
					LC_ActorBase* pkActor = (LC_ActorBase*)(m_pkMap->FindObject(lCurrentLocked));
					if (pkActor)
					{
						CF_CharType::DataEntry* pkData = CF_CharType::GetSingletonPtr()->GetEntryPtr(pkActor->GetCharType());
						if (pkData != NULL && pkData->_bBoss > 0)
							bIsBoss = true;
					}
					if (bIsBoss)
						pkClientTargetModel->AttachFxExt(GD_ClientActorModel::FDT_DYNAMIC_GROUND, 0, false, RES_FX_CHOOSE_TARGET_BOSS, NULL, NULL, -1.0f);
					else
						pkClientTargetModel->AttachFxExt(GD_ClientActorModel::FDT_DYNAMIC_GROUND, 0, false, RES_FX_CHOOSE_TARGET_RED, NULL, NULL, -1.0f);
				}
				else
					pkClientTargetModel->AttachFxExt(GD_ClientActorModel::FDT_DYNAMIC_GROUND, 0, false, RES_FX_CHOOSE_TARGET_GREEN, NULL, NULL, -1.0f);
			}
			else if(IS_PLAYER_CLASS_ID(lCurrentLocked))
			{
				if(lCurrentLocked == m_iID)
				{
					pkClientTargetModel->AttachFxExt(GD_ClientActorModel::FDT_DYNAMIC_GROUND, 0, false, RES_FX_CHOOSE_TARGET_GREEN, NULL, NULL, -1.0f);
				}
				else
				{
					if(bCanAttack)
						pkClientTargetModel->AttachFxExt(GD_ClientActorModel::FDT_DYNAMIC_GROUND, 0, false, RES_FX_CHOOSE_TARGET_RED, NULL, NULL, -1.0f);
					else
						pkClientTargetModel->AttachFxExt(GD_ClientActorModel::FDT_DYNAMIC_GROUND, 0, false, RES_FX_CHOOSE_TARGET_GREEN, NULL, NULL, -1.0f);
				}
			}

			if(IS_PLAYER_CLASS_ID(lCurrentLocked))
				SendMessage( CreateMsg_UE_SHOW_PLAYER_BLOOD(lCurrentLocked) );

			else if(IS_NPC_ID(lCurrentLocked))
				SendMessage( CreateMsg_UE_UPDATE_MONSTER_BLOOD(lCurrentLocked, 1));

			else if(IS_CHEST_ID(lCurrentLocked))
				SendMessage( CreateMsg_UE_NAME_FRAME_STATE_HOVER(lCurrentLocked));

		}
	}

	//if(lCurrentLocked != m_lLastLockTargetID)
	//{
	//	pkTargetModel = pkIMManager->GetModel(lCurrentLocked);
	//	if(pkTargetModel)
	//	{
	//		pkTargetModel->AttachFx(GD_ClientActorModel::FDT_GROUND, 0, false, RES_FX_CHOOSE_TARGET_RED, NULL, NULL, -1.0f);
	//		
	//	}

	//	pkTargetModel = pkIMManager->GetModel(m_lLastLockTargetID);
	//	if(pkTargetModel)
	//	{
	//		pkTargetModel->DetachFx(GD_ClientActorModel::FDT_GROUND, RES_FX_CHOOSE_TARGET_RED);
	//		
	//	}
	//}

	if(m_lLastMouseOverActorID != m_lMouseOverActorID)
	{
		if (m_lLastMouseOverActorID != 0)
		{
			pkTargetModel = pkIMManager->GetModel(m_lLastMouseOverActorID);
			if(pkTargetModel)
			{
				pkTargetModel->Highlight(false);
				//pkTargetModel->Outline(false);	
			}
		}
		if (m_lMouseOverActorID != 0 && m_bMouseOverTargetArgValid)
		{
			//pkTargetModel = pkIMManager->GetModel(m_lMouseOverActorID);
			//if(pkTargetModel)
			//{
			//	pkTargetModel->Outline(true, m_kMouseOverTargetArg.kOutlineColor);
			//}
		}
		if ( m_lMouseOverActorID != 0) 
		{
			pkTargetModel = pkIMManager->GetModel(m_lMouseOverActorID);
			if(pkTargetModel)
			{
				pkTargetModel->Highlight(true, NiColor(0.2f,0.2f,0.2f));
			}
		}
	}

	if (m_lLastMouseOverInteractiveBtnActorID != m_lMouseOverInteractiveBtnActorID)
	{
		if (IS_PLAYER_CLASS_ID(m_lLastMouseOverInteractiveBtnActorID) || IS_NPC_ID(m_lLastMouseOverInteractiveBtnActorID))
			SendMessage( CreateMsg_UE_NAME_FRAME_STATE_NORMAL(m_lLastMouseOverInteractiveBtnActorID) );
		pkTargetModel = pkIMManager->GetModel(m_lMouseOverInteractiveBtnActorID);
		if(pkTargetModel && (IS_PLAYER_CLASS_ID(m_lMouseOverInteractiveBtnActorID) || IS_NPC_ID(m_lMouseOverInteractiveBtnActorID)))
		{
			SendMessage( CreateMsg_UE_NAME_FRAME_STATE_HOVER(m_lMouseOverInteractiveBtnActorID) );
		}
	}

	//发送目标更换协议
	if( lCurrentLocked != m_lLastLockTargetID)
	{
		MG_PACKET_ENCODE_HANDLER hMsgHandler;

		MG_REQ_Select_Target msg;
		msg.m_lTargetId			= lCurrentLocked;

		bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SELECT_TARGET, &msg);
		if (bCreateMsg)
			m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}

	m_lLastMouseOverInteractiveBtnActorID = m_lMouseOverInteractiveBtnActorID;
	m_lLastMouseOverActorID = m_lMouseOverActorID;
	m_lLastWatchTargetID	= m_lWatchTargetID;
	m_lLastLockTargetID		= lCurrentLocked;
	m_bWatchTargetFactionChange = false;
}
//--------------------------------------------------------
void LC_ClientGamePlayer::_updateWeaponVisibility()
{
	bool bShowWeapon = true;
	bool bShowBow = true;
	bool bShowWing = true;
	int bowSuitPos = GD_ActorModel::WSP_BACK;

	if(GetSkillFlag((int64_t)SK_DIZZ | SK_SLEEP | SK_DISARM /*| SK_WINDY*/))
	{
		bShowWeapon = false;
	}

	if(GetSkillFlag((int64_t)(SK_DISARM_BOW /*| SK_WINDY*/)))
	{
		bShowBow = false;
	}

	switch ( g_settings.DS_NUPAO_WING )
	{
	case UT_GlobalSettings::NPW_BOW_NOWING:
		if ( GetBowLevel() > 0 )
		{
			bShowWing = false;
			bShowBow = true;
		}
		break;
	case UT_GlobalSettings::NPW_NOBOW_WING:
		if ( GetMuhunLevel() > 0 )	
		{
			bShowWing = true;
			bShowBow = false;
		}
		break;
	case UT_GlobalSettings::NPW_BOTH:
		if ( GetBowLevel() > 0 )
			bShowBow = true; 
		if ( GetMuhunLevel() > 0 )
			bShowWing = true;
		break;
	}


	if(IS_SKILL_TYPE(m_iCurrentState))
	{
		SK_SkillExecute* pkSkill = (SK_SkillExecute*)GetCurrentState();
		if (pkSkill != NULL)
		{
			int u = pkSkill->GetProperty_WeaponUsage();
			if(u & SWU_HIDE_WEAPON)
				bShowWeapon = false;
			if(u & SWU_TAKE_BOW)
			{
				bowSuitPos = GD_ActorModel::WSP_HAND;
				bShowBow = true;
			}
		}
	}

	SetWeaponVsible(bShowWeapon);
	SetBowVsible(bShowBow, bowSuitPos);
	SetWingVisible(bShowWing);

	if (GetSkillFlag(SK_DISARM) && !GetTutorialAsset()->GetBitVec(27))		//新手训练第27步，缴械
	{
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_DISARM);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
}
//--------------------------------------------------------
void LC_ClientGamePlayer::_updateDisarmState()
{
	//缴械状态下不能使用武器
	if (GetSkillFlag(SK_DISARM))
	{
		SetCurrentBandWeapon(WT_HAND);
	}
	else	
	{
		//如果已经装备了武器，则设置武器类型
		if (IS_ITEM_ID(GetCurrentEquipItem(ET_WEAPON)))
		{
			SetEquipWeapon(GetCurrentEquipItem(ET_WEAPON));
		}
	}
}
//--------------------------------------------------------
void LC_ClientGamePlayer::_updateNPCFunction()
{
	T_PROFILE("LC_ClientGamePlayer::_updateNPCFunction");
	if(m_eFunctionType == NFT_NONE)
		return;

	object_id_type lNPCID = GetLockedNpcID();
	if(!IS_NPC_ID(lNPCID) && !IS_LOCAL_NPC_ID(lNPCID))
	{
		SetNPCFunctionType(NFT_NONE);
		SetLockedNpcID(0);
		g_soundMgr.StopCurrentNPCSound();
		return;
	}

	LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
	LC_ClientNPC* pkNPC = (LC_ClientNPC*)pkNPCManager->FindNPC(lNPCID);
	if(!pkNPC)
	{
		SetNPCFunctionType(NFT_NONE);
		SetLockedNpcID(0);
		g_soundMgr.StopCurrentNPCSound();
		return;
	}

	int iDistance = UT_MathBase::LineLengthXYInt(GetVisibleLocation(),pkNPC->GetVisibleLocation());
	if(iDistance > (g_settings.PROFILE_TALK_DISTANCE + GetProperty_ShapeRadius() + pkNPC->GetProperty_ShapeRadius()))
	{
		SetNPCFunctionType(NFT_NONE);
		SetLockedNpcID(0);
		g_soundMgr.StopCurrentNPCSound();
		return;
	}

	if (GetDead())
		SetNPCFunctionType(NFT_NONE);

}
//--------------------------------------------------------
void   LC_ClientGamePlayer::_updateTradeState(float fCurrentTime)
{

	if (m_kTradeManager.GetTradeState() != PTS_NO_TRADE)
	{
		if (fCurrentTime - m_fLastUpdateTradeStateTime < 0.5f)
			return;

		m_fLastUpdateTradeStateTime = fCurrentTime;

		if (GetDead())
		{
			RequestInterruptTrade();
			return;
		}

		object_id_type lNetPlayerID = m_kTradeManager.GetTradeTargetID();
		LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
		LC_ClientNetPlayer*	pkNetPlayer = (LC_ClientNetPlayer*)(pkNetPlayerManager->FindPlayer(lNetPlayerID));
		if (pkNetPlayer)
		{
			Utility::UT_Vec3Int NetPlayerPos = pkNetPlayer->GetCurrentLocation();
			int iDistance = UT_MathBase::LineLengthXYInt(GetCurrentLocation(),NetPlayerPos);
			//GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"交易距离 : %d",iDistance);
			if(iDistance > MAX_TRADE_DISTANCE)
			{
				RequestInterruptTrade();
				SendLogicInfoMessage(RE_ITEM_TRADE_NOT_IN_RANGE);
			}
		}
	}
}
//-------------------------------------------------------
void  LC_ClientGamePlayer::_UpdateProcessingTask( float fCurrentTime )
{
	if (fCurrentTime - m_fLastUpdateProcessingTask < 0.5f)
		return;

	m_fLastUpdateProcessingTask = fCurrentTime;

	LC_ClientTaskManager* pkManager = (LC_ClientTaskManager*)LC_ClientTaskManager::GetSingletonPtr();
	if (pkManager == NULL)
		return;

	LC_ProcessingTaskEntryMap kProcessEntryMap = m_pkTaskMap->GetProcessingTaskEntryMap();
	LC_ProcessingTaskEntryMap::iterator it = kProcessEntryMap.begin();

	for (; it!=kProcessEntryMap.end(); ++it)
	{
		//检查进行中任务entry
		LC_ProcessingTaskEntry& kProcessingTaskEntry = it->second;

		int lTaskID = kProcessingTaskEntry.GetTaskID();
		LC_ClientTask* pkTask = (LC_ClientTask*)pkManager->GetTask(lTaskID);
		if (pkTask)
		{
			//#pragma TODO("应该等待服务器应答才继续判断,待修改")
			pkTask->Update(fCurrentTime,this);

			ResultType iResult = pkTask->CanFinish(this,GET_PROCESS_TIME);
			int TaskType = pkTask->GetTaskType();
			
			//BUG #15632 【功能】【任务系统】日常任务和帮会任务增加双倍领取功能
			//BUG #24365 【优化】【讨伐任务】讨伐任务面板调整优化，讨伐任务也需要这个功能
			if (TaskType == STT_DAILY_TASK || TaskType == STT_GUILD_TASK || TaskType == STT_REWARD_TASK || TaskType == STT_FRONTIER || TaskType == STT_BESTGUILD_DAILYTASK)
			{
				mem::map<int,int>::iterator it = m_kTaskState.find(lTaskID);
				if (it != m_kTaskState.end() && it->second != RE_TASK_FINISH_SUCCESS && iResult == RE_TASK_FINISH_SUCCESS)
				{
 					LC_GameEvent kGameEvent;
 					kGameEvent.SetEventType(GLET_CAN_FINISH_TASK);
					kGameEvent.SetEventParam(EP_CAN_FINISH_TASK_ID, lTaskID);
					kGameEvent.SetEventParam(EP_CAN_FINISH_TASK_TYPE, TaskType);
 
 					UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
 					pkUIManager->HandlerGameEvent(kGameEvent);
				}
				m_kTaskState[lTaskID] = iResult;
			}
			int iFinishType = pkTask->GetFinishType();
			int iMapLogicType = GetMapLogicType();
			if ( iFinishType == TFT_AUTO
				|| ((iFinishType == TFT_AUTO_WILDDRAW || iFinishType == TFT_AUTO_WILD) && IS_NORMAL_MAP(iMapLogicType)))
			{
				if (iResult == RE_TASK_FINISH_SUCCESS)
				{
					if ( TaskType == STT_MAIN_SCENARIO && iFinishType != TFT_AUTO_WILDDRAW)
					{
						long lConversationID = pkTask->GetConversationIDByState(NT_CANFINISH);
						//if (lConversationID > 0)
						//{
						LC_GameEvent kGameEvent;
						kGameEvent.SetEventType(GLET_CLIENT_RAID_NPCTALK);
						kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, lConversationID);
						kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, lTaskID);
						kGameEvent.SetEventParam(EVENT_BASE_PARAM_3, NT_CANFINISH);
						UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
						pkUIManager->HandlerGameEvent(kGameEvent);
						//}
					}
					else
					{
						ReqFinishTask(lTaskID,-1);
					}

					LC_ClientTaskMap* pkClientTaskMap = GetClientTaskMap();
					if (NULL != pkClientTaskMap)
					{
						StringType traceInfo = pkClientTaskMap->GetTaskTraceInfo(lTaskID);
						int iObjectDesc = LC_Helper::GetWorldObjecDescID(traceInfo);
						CF_WorldObjectDesc::DataEntry* pkEntry = CF_WorldObjectDesc::GetSingletonPtr()->GetEntryPtr(iObjectDesc);
						if (pkEntry && pkEntry->_lType == 3 && pkEntry->_lLogicID == GetSpecialHookCharType())
						{
							SetSpecialHookCharType(0);
						}
					}
				}
			}

			//1 悬赏任务不判断是否超时，由服务器同步, 2 押镖任务超时不取消，让服务器同步
			if( pkTask->GetTaskType() != STT_REWARD_TASK && pkTask->GetGameStoryID()!=GAMESTORY_ID_BianJingYunBiao && pkTask->GetTimeLimit() > 0)
			{
				ResultType iResult = pkTask->IsOverTime(this,GET_PROCESS_TIME);
				if (iResult == RE_TASK_FINISH_FAILED_TIME )
				{
					ReqCancelTask(lTaskID,true);
					m_pkTaskMap->RemoveTaskProcessing(lTaskID);
					continue;
				}
			}
		}	
	}
}
//------------------------------------------------------------------------
void	LC_ClientGamePlayer::_UpdateAutoStartTask ( float fCurrentTime )
{
	NW_ClientSessionGroup* pkSessionGroup = NW_ClientSessionManager::GetSingleton().GetGameSessionGroup();
	if(pkSessionGroup->m_pkGameStageMachine->GetCurrentStateType() != ST_GAME_STAGE_GAME)
		return;
	//在任务数据完成同步以后才开始处理自动接任务流程。初始状态下，该flag revisionid为0.
	if(m_pkTaskMap->GetDirtyFlagRevisionID(DIRTY_FLAG_FINISHEDTASK) <= 0 || m_pkTaskMap->GetDirtyFlagRevisionID(DIRTY_FLAG_PROCESSINGTASK) <= 0)
		return;

	static float static_last_update_time = 0.0f;
	if (!m_bImmediateTaskStart && fCurrentTime - static_last_update_time < 0.5f)
		return;

	//((LC_ClientTaskMap *)m_pkTaskMap)->UpdateAutoStartList();
	m_bImmediateTaskStart = false;
	static_last_update_time = fCurrentTime;

	LC_ClientTaskManager* pkTaskManager = (LC_ClientTaskManager*)LC_ClientTaskManager::GetSingletonPtr();
	if (pkTaskManager == NULL)
		return;

	//登陆启动自动接取任务
	LC_ClientTaskMap::AutoStartTaskSet& kLoginTaskList = ((LC_ClientTaskMap *)m_pkTaskMap)->GetLoginStartTaskList();
	_DoStartTask(kLoginTaskList);

	//只有在城镇或者野外才能接任务，[策划需求]
	//由于新号，第一次进游戏的时候，是在副本，但是还需要将任务自动接起来，所以需要特殊处理。
	if(!IS_NORMAL_MAP(GetMapLogicType()))
		return;
	LC_ClientTaskMap::AutoStartTaskSet& rAutoTaskList = ((LC_ClientTaskMap *)m_pkTaskMap)->GetAutoStartTaskList();
	_DoStartTask(rAutoTaskList);
}

void LC_ClientGamePlayer::_DoStartTask(LC_ClientTaskMap::AutoStartTaskSet& rAutoTaskList)
{
	//TODO: DOSTARTTASK功能修改
	LC_ClientTaskManager* pkTaskManager = (LC_ClientTaskManager*)LC_ClientTaskManager::GetSingletonPtr();
	if (pkTaskManager == NULL)
		return;
	LC_ClientTaskMap::AutoStartTaskSet delToAutoStarList;
	for (LC_ClientTaskMap::AutoStartTaskSet::iterator it = rAutoTaskList.begin(); it != rAutoTaskList.end(); ++it )
	{
		long lTaskID = *it;
		LC_ClientTask* pkTask = (LC_ClientTask*)pkTaskManager->GetTask(lTaskID);
		if (!GetDead() && pkTask)
		{
			int iResult = m_pkTaskMap->CanAccept(pkTask,this);
			//if (pkTask->GetTaskType() == STT_GUILD_TASK)
			//{
			//	GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"帮会任务DOSTART TASKID = [%d], iResult = [%d], 时间[%f]",lTaskID,iResult,GET_PROCESS_TIME);
			//}
			//int iResult = pkTask->CanStart(this);
			if (iResult == RE_TASK_START_SUCCESS)
			{
				long lConversationID = pkTask->GetConversationIDByState(NT_CANACCEPT);
				if (lConversationID > 0)
				{
					LC_GameEvent kGameEvent;
					kGameEvent.SetEventType(GLET_CLIENT_RAID_NPCTALK);
					kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, lConversationID);
					kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, pkTask->GetTaskID());


					kGameEvent.SetEventParam(EVENT_BASE_PARAM_3, NT_CANACCEPT);
					UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
					pkUIManager->HandlerGameEvent(kGameEvent);
				}
				else
				{
					ReqAcceptTask(lTaskID);
				}
				delToAutoStarList.insert(lTaskID);

				//((LC_ClientTaskMap *)m_pkTaskMap)->DelToCanStartList(lTaskID);
			}
			else if (iResult == RE_TASK_START_FAILED_PROCESSING || iResult == RE_TASK_START_FAILED_FINISHED || iResult == RE_TASK_CONCURRENCE_COUNT_LIMIT)
			{
				delToAutoStarList.insert(lTaskID);
				//((LC_ClientTaskMap *)m_pkTaskMap)->DelToCanStartList(lTaskID);
			}
		}
	}
	for (LC_ClientTaskMap::AutoStartTaskSet::iterator it = delToAutoStarList.begin(); it != delToAutoStarList.end(); ++it )
	{
		((LC_ClientTaskMap *)m_pkTaskMap)->DelToCanStartList(*it);
	}
}

//------------------------------------------------------------------------
void LC_ClientGamePlayer::_UpdateTriggerInfo( float fCurrentTime )
{
	LC_MapBase* pkMap = GetMap();
	if (pkMap == NULL)
		return;

	LC_LogicTriggerInfo objTriggerInfo;
	if (LC_LogicTriggerManager::GetSingletonPtr()->CheckTriggerLogic(GetMapResID(),GetCurrentLocation(),objTriggerInfo))
	{
		if (m_bRequestLogicTrigger == false)
		{
 			if (fCurrentTime - m_fLastLogicTriggerTime > 3.0f)
			{
				switch (objTriggerInfo.m_iTriggerType)
				{
				case LTT_TASK:
					RequestNocticeTaskEvent(0,GLET_PLAYER_ARRIVE_AREA,objTriggerInfo.m_lID,objTriggerInfo.m_lTriggerParam);
					break;
				case LTT_TRANSFER:
					RequestTranscriptionTransfer(objTriggerInfo.m_lTriggerParam);
					break;
				case LIT_SHOW_TRANSCRIPTIONLIST:
					{
						LC_GameEvent kGameEvent;
						kGameEvent.SetEventType(GLET_UI_MAP_TRANSCRIPTION_SHOW);
						kGameEvent.SetEventParam(PARAM_EVENT_PARAM_1, objTriggerInfo.m_lTriggerParam);

						UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
						pkUIManager->HandlerGameEvent(kGameEvent);
					}
					break;
				case LIT_SCREENT_GFX: 
					{
						m_bStartTriggerType = LIT_SCREENT_GFX;
					}
					break;
				}

				m_fLastLogicTriggerTime = fCurrentTime;
			}

			//立即触发
			if ( objTriggerInfo.m_iTriggerType == LIT_JUMP_AREA)
			{
				m_bStartTriggerType = LIT_JUMP_AREA;
				m_bTriggerTypeTick.ResetTimer();
			}
			else if ( objTriggerInfo.m_iTriggerType != LIT_SCREENT_GFX)
			{
				m_bStartTriggerType = 0;
			}

			m_bRequestLogicTrigger = true;
		}
		else if ( m_bStartTriggerType == LIT_SCREENT_GFX )
		{
			if (m_bTriggerTypeTick.IsDown() || !m_bTriggerTypeTick.IsActivate() )
			{
				GD_ClientActorModel* pkVisibleData = IM_Manager::GetSingletonPtr()->GetActor(GetID());
				if (pkVisibleData != NULL)
				{
					pkVisibleData->AttachFx(GD_ModelBase::FDT_INVALID, 0, true, "W_屏幕_光晕.gfx");
				}
				m_bTriggerTypeTick.Start(objTriggerInfo.m_lTriggerParam+objTriggerInfo.m_lTriggerParam*UT_MathBase::RandFloat01());
			}
		}
		else if ( m_bStartTriggerType == LIT_JUMP_AREA)
		{
			UT_Vec3Int sd = objTriggerInfo.m_vecTriggerParam;
			int jt = 0;
			switch ( objTriggerInfo.m_lTriggerParam )
			{
			case 1:
				jt = ST_PLAYER_JUMP1;
				break;
			case 2:
				jt = ST_PLAYER_JUMP2;
				break;
			case 3:
				jt = ST_PLAYER_JUMP3;
				break;
			}

			bool bIsPathFind = IsPathFinding();
			PathPendingActionArg tmp = GetPathPendingAction();
			//使用跳跃
			if (RE_SKILL_NONE_ERROR == CheckPendingJump(ST_PLAYER_JUMP_GUIDE)
				&& RE_SKILL_PENDING_SUCCESS == PendingJump(ST_PLAYER_JUMP_GUIDE, sd))
			{
				if ( bIsPathFind )
				{
					SetjumpTarget(GetTargetLocation());
					JumpPathPendingAction(tmp.enmType, tmp.iTargetCharTypeID);
				}
				SetJumpMapResID(GetMapResID());
				SetNextState(ST_PLAYER_JUMP_GUIDE);
				RequestJump(ST_PLAYER_JUMP_GUIDE, sd,(StateMachine::StateType)jt);
				SetjumpType(jt);
			}
		}
	}
	else if (m_bRequestLogicTrigger == true)
	{
		m_bRequestLogicTrigger = false;
		m_bStartTriggerType = 0;
	}
}

void LC_ClientGamePlayer::PendingJumpPathAction()
{
	if(!IsPathFinding())
		return;

	SetjumpTarget(GetTargetLocation());

	PathPendingActionArg tmp = GetPathPendingAction();
	JumpPathPendingAction(tmp.enmType, tmp.iTargetCharTypeID, tmp.iTargetMapLogicID);
}

//--------------------------------------------------------
void LC_ClientGamePlayer::_resetTriggerInfo()
{
	m_bRequestLogicTrigger = false;
	m_bStartTriggerType = 0;
}
//--------------------------------------------------------
void LC_ClientGamePlayer::_updateRevealInvisibility( float fCurrentTime )
{
	//玩家处于反隐状态才执行后续操作
	if (!GetSkillFlag(SK_REVEAL_INVISIBLE))
	{
		//重置目标身上的反隐标志
		m_kRevealInvisibilityParams.ClearRevealTargetList();
		return;
	}

	//控制反隐频率
	if (fCurrentTime - m_kRevealInvisibilityParams.m_lLastRevealTime < 0.5)
	{
		return;
	}
	m_kRevealInvisibilityParams.m_lLastRevealTime = fCurrentTime;

	//如果只要求一次反隐操作，则结束
	if (m_kRevealInvisibilityParams.m_bRevealOnce && !m_kRevealInvisibilityParams.m_bIsFirstReveal)
	{
		return;
	}
	m_kRevealInvisibilityParams.m_bIsFirstReveal = false;

	//重置目标身上的反隐标志
	m_kRevealInvisibilityParams.ClearRevealTargetList();

	//查找附近的目标
	LC_MapBase* pkMap = GetMap();
	if(!pkMap)
	{
		GfxWriteLog(LOG_WARNING_FILELINE, LOG_SWITCH_WARNING,"脚本:试图用非法目标位置作区域锁定");
		return;
	}
	CF_ActorFilterFunc* pkCSVActorFilterFunc = CF_ActorFilterFunc::GetSingletonPtr();
	CF_ActorFilterFunc::DataEntry* pkData = pkCSVActorFilterFunc->GetEntryPtr(2);
	//从地图返回区域内的Actor队列
	UT_Circle kCircle;
	kCircle.kCenterPoint = m_kRevealInvisibilityParams.m_kCircleCenter;
	kCircle.iRadius = m_kRevealInvisibilityParams.m_iCircleRadius;
	LC_ActorPtrVector* pkLockResult = pkMap->LockActorsInCircle(kCircle,pkData);

	if (!pkLockResult->empty())
	{
		LC_ActorPtrVectorIter iter;
		TRAVP(pkLockResult, iter)
		{
			LC_ActorBase* pkTarget = *iter;
			//对目标设置反隐标志
			if (NULL != pkTarget && pkTarget->GetID() != GetID())
			{
				pkTarget->SetRevealInvisibility(true);
				//保存目标
				m_kRevealInvisibilityParams.m_kLastRevealTargetList.push_back(pkTarget);
			}
		}
	}
}
//--------------------------------------------------------
void LC_ClientGamePlayer::_updateAutoUpgradeCimelia(float fCurrentTime)
{
	CimeliaUpgradeInfoMap::iterator it = m_bDoAutoUpgradeCimelia.begin();
	for (; it!=m_bDoAutoUpgradeCimelia.end(); ++it)
	{
		if(it->second)
		{
			if(m_fLastAutoUpgradeCimeliaTime[it->first] == 0.0f)
			{
				m_fLastAutoUpgradeCimeliaTime[it->first] = fCurrentTime;
			}

			//2次自动升级的间隔
			if (fCurrentTime - m_fLastAutoUpgradeCimeliaTime[it->first] < AUTO_UPGRADE_INTERVAL)
			{
				return;
			}
			m_fLastAutoUpgradeCimeliaTime[it->first] = fCurrentTime;
			//每次请求后就暂停自动进化，等待收到上一次的回应消息
			m_bDoAutoUpgradeCimelia[it->first] = false;

			RequestUpgradeCimelia(it->first);
		}
	}
}
//--------------------------------------------------------
void LC_ClientGamePlayer::_updateAutoUpgradeSkill(float fCurrentTime)
{
	if(m_bDoAutoUpgradeSkill)
	{
		if(m_fLastAutoUpgradeSkillTime == 0.0f)
		{
			m_fLastAutoUpgradeSkillTime = fCurrentTime;
		}

		//2次自动升级的间隔
		if (fCurrentTime - m_fLastAutoUpgradeSkillTime < AUTO_UPGRADE_INTERVAL)
		{
			return;
		}
		m_fLastAutoUpgradeSkillTime = fCurrentTime;
		//每次请求后就暂停，等待收到上一次的回应消息
		m_bDoAutoUpgradeSkill = false;

		RequestSkillUpGrade(m_iCurrentUpgradeSkillID, 0, m_iCurrentUpgradeBuddyID);
	}
}

//--------------------------------------------------------
ResultType LC_ClientGamePlayer::_checkNeedLockTarget(SK_SkillExecute* pkSkillExecute)
{
	////是否需要等待锁定
	//bool bNeedLockLocation = false;
	//bool bNeedLockTarget = false;
	switch(pkSkillExecute->GetProperty_OperationType())
	{
	case SOT_TARGET: //目标型
		{
			int iLockTargetType = pkSkillExecute->GetProperty_LockTargetType();
			CF_ActorFilterFunc* pkCSVActorFilterFunc = CF_ActorFilterFunc::GetSingletonPtr();
			CF_ActorFilterFunc::DataEntry* pkData = pkCSVActorFilterFunc->GetEntryPtr(iLockTargetType);
			if(!pkData)
			{
				GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"技能目标锁定类型格式错误");
				//bNeedLockTarget = true;
				return RE_SKILL_SYSTEM_ERROR;
			}
			else
			{
				//现在修改成只攻击锁定的目标
				//if(m_lWatchTargetID == 0)
				if (GetLockedTargetID() > 0)
				{
					LC_LogicObject* pkTarget = GetMap()->FindObject(GetLockedTargetID());
					if (NULL == pkTarget)
					{
						SetLockedTargetID(0);
					}
				}

				if( GetLockedTargetID() == 0)
				{
					//bNeedLockTarget = true;
					if (!IsHooking())
					{
						LC_ActorPtrVector* pkLockResult = NULL;
						UT_Circle kCircle;
						kCircle.iRadius = GetGlobalSetting.mouseCircleRadius;
						kCircle.kCenterPoint = UT_EngineMath::ConvertNiPoint3ToVec3Int(GetMouseOverLocation());
						pkLockResult = GetMap()->LockActorsInCircle(kCircle, pkData);
						if (pkLockResult->size() > 0)
						{
							object_id_vector kTargets = LC_Helper::FilterActorsSortDist(this, *pkLockResult, pkData);
							if (kTargets.size() > 0)
							{
								object_id_type pickID = kTargets.front();
								SetPreLockedTargetID(pickID);
								ClearChangeLockedFlag();
								SetLockedTargetID(pickID);
								return RE_SKILL_CHECK_NO_NEED_TARGET;
							}
							else
								return RE_SKILL_CHECK_NEED_TARGET;
						}
						else
							return RE_SKILL_CHECK_NEED_TARGET;
					}
					else
						return RE_SKILL_CHECK_NEED_TARGET;
				}
				else
				{
					//if(lSkillTypeID != _findAttackSkill())
					{
						//如果目标合法
						if(LC_Helper::IsValidTarget(this,GetLockedTargetID(),pkData) == RE_SKILL_CHECK_VALID_TARGET)
						{
							//SetLockedTargetID(m_lWatchTargetID);
							return RE_SKILL_CHECK_VALID_TARGET;
						}
						else
						{
							//目标型技能，对不合法的目标是释放技能不取消目标
							//SetLockedTargetID(0);
							return RE_SKILL_CHECK_INVALID_TARGET;
						}
					}
				}
			}
		}
		break;
	case SOT_DIR: //朝向型
		return RE_SKILL_CHECK_NO_NEED_TARGET;
		break;
	case SOT_LOCATION: //地点型 需要锁定地点，设置标志位，下次接收到鼠标消息后会特殊处理
		//bNeedLockLocation = true;
		return RE_SKILL_CHECK_NEED_LOCATION;
		break;
	default:
		{
			GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "错误的目标施放类型[%i]", pkSkillExecute->GetProperty_OperationType());
			return RE_SKILL_SYSTEM_ERROR;
		}
		break;
	}
}
//--------------------------------------------------------
//ResultType LC_ClientGamePlayer::_checkSkillInSafeArea( int lSkillID, object_id_type lTargetID )
//{
//	SK_Factory* pkSkillFactory = SK_Factory::GetSingletonPtr();
//	SK_SkillExecute* pkSkillExecute = pkSkillFactory->RequestSkillExcute(lSkillID);
//	if (NULL == pkSkillExecute)
//	{
//		return RE_SKILL_SYSTEM_ERROR;
//	}
//
//	//技能伤害类型
//	int iSkillDamageType = pkSkillExecute->GetProperty_DamageType();
//
//	LC_MapBase* pkMap = GetMap();
//	LC_LogicAreaInfo kLogicAreaInfo;
//	LC_LogicMapManager* pkMapManager = LC_LogicMapManager::GetSingletonPtr();
//	pkMapManager->QueryAreaInfo(GetCurrentLocation(), pkMap->GetMapID(), kLogicAreaInfo);
//	//玩家是否在安全区
//#pragma TODO("需要判断新逻辑区域与安全区域关系")
//	//bool bPlayerInSafeArea = (kLogicAreaInfo.eType == AT_SAFE_AREA);
//    bool bPlayerInSafeArea = false;
//
//	LC_LogicObject* pkTarget = pkMap->FindObject(lTargetID);
//	if (NULL == pkTarget)
//	{
//		return RE_SKILL_CHECK_INVALID_TARGET;
//	}
//	pkMapManager->QueryAreaInfo(pkTarget->GetCurrentLocation(), pkMap->GetMapID(), kLogicAreaInfo);
//	//目标是否在安全区
//#pragma TODO("需要判断新逻辑区域与安全区的关系")
//	//bool bTargetInSafeArea = (kLogicAreaInfo.eType == AT_SAFE_AREA);
//	bool bTargetInSafeArea = false;
//	if( iSkillDamageType != 0 && (bPlayerInSafeArea || bTargetInSafeArea) )
//	{
//		return RE_SKILL_CHECK_IN_SAFE_AREA;
//	}
//	return RE_SKILL_CHECK_IN_FIGHT_AREA;
//}
//--------------------------------------------------------
//void LC_ClientGamePlayer::_switchTarget( object_id_type lNewTargetID )
//{
//	m_lSaveWatchTargetID = m_lWatchTargetID;
//	m_lSaveLockedTargetID = m_lLockedTargetID;
//	WatchTarget(lNewTargetID);
//	SetLockedTargetID(lNewTargetID);
//}
//--------------------------------------------------------
void LC_ClientGamePlayer::_restoreTarget()
{
	if (m_lSaveWatchTargetID > 0 || m_lSaveLockedTargetID > 0)
	{
		WatchTarget(m_lSaveWatchTargetID);
		SetLockedTargetID(m_lSaveLockedTargetID);
		m_lSaveWatchTargetID = 0;
		m_lSaveLockedTargetID = 0;
	}
}

void LC_ClientGamePlayer::_resetAutoTask()
{
	StopMove();
	Enable2Guding(false);
	SetHooking(false);
	SetRaidHookingFlag(false);
}
//--------------------------------------------------------
// void LC_ClientGamePlayer::LoadNPCTaskInfo(int iNPCID)
// {
// 	LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
// 	LC_ClientNPC* pkNPC = (LC_ClientNPC* )pkNPCManager->FindNPC(iNPCID);
// 	if(!pkNPC)
// 		return ;
// 
// 	CF_CharType* pkCSVCharType = CF_CharType::GetSingletonPtr();
// 	CF_CharType::DataEntry* pkData = pkCSVCharType->GetEntryPtr(pkNPC->GetCharType());
// 	if(!pkData)
// 		return ;
// 	m_kTaskMap.LoadNPCTaskInfo(pkNPC->GetCharType());
// }
//--------------------------------------------------------
void LC_ClientGamePlayer::ReqAcceptTask(int lTaskID,bool bCheckAssign)
{
	//GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"请求接取任务[%d]，时间[%f]",lTaskID,GET_PROCESS_TIME);
	LC_ClientTaskManager* pkTaskManager = (LC_ClientTaskManager*)LC_ClientTaskManager::GetSingletonPtr();
	if (pkTaskManager == NULL)
		return;

	LC_ClientTask*  pkTask = (LC_ClientTask*)pkTaskManager->GetTask(lTaskID);
	if (pkTask == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"不存在该任务[%d]",lTaskID);
		return;
	}

	ResultType iResult = pkTask->CanStart(this,bCheckAssign);
	if (iResult == RE_TASK_START_SUCCESS)
	{
		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = false;

		MG_REQ_AcceptTask reqMsg;
		reqMsg.m_lPlayerID = GetSelfID();
		reqMsg.m_lTaskID = lTaskID;
		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_ACCEPT_TASK, &reqMsg);

		if(bCreateMsg)
		{
			m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
			if(lTaskID == FIRST_TASK_ID)
				commitPatchProgress(EVENT_CLIENT_REQ_ACCEPT_1st_TASK, 0);
		}
	} 
	/*else
	{
		if (iResult == RE_TASK_GROUP_MEMBER_NUM_LIMIT)
		{
			Utility::UT_Vec3Int* pkVec = pkTask->GetTaskStartCondition(TSC_GROUP_MEMBER_COUNT);
			if (pkVec)
				SendLogicInfoMessage(iResult,pkVec->y,pkVec->z);
		}
		else
			SendLogicInfoMessage(iResult);
	}*/

}
//--------------------------------------------------------
bool LC_ClientGamePlayer::ReqFinishTask(int lTaskID,int iIndex)
{
	GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM,"请求完成任务[%d]",lTaskID);
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	LC_ClientTaskManager* pkTaskManager = (LC_ClientTaskManager*)LC_ClientTaskManager::GetSingletonPtr();
	if (pkTaskManager == NULL)
		return false;

	LC_ClientTask*  pkTask = (LC_ClientTask*)pkTaskManager->GetTask(lTaskID);
	if (pkTask == NULL)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"不存在该任务[%d]",lTaskID);
		return false;
	}

	if (pkTask->IsSelectBonus() && iIndex < 0)
	{
		SendLogicInfoMessage(RE_ITEM_SELECT_NEEDED);
		return false;
	}

	/*
	ResultType iBonusResult = pkTask->CanGetBonus(this,iIndex);
	if ( iBonusResult != RE_SUCCESS)
	{
	SendLogicInfoMessage(iBonusResult);
	return false;
	}
	*/
	ResultType iResult = pkTask->CanFinish(this,GET_PROCESS_TIME);

	if (RE_TASK_FINISH_SUCCESS == iResult)
	{
		MG_REQ_FinishTask reqMsg;
		reqMsg.m_lPlayerID = GetSelfID();
		reqMsg.m_lTaskID = lTaskID;
		reqMsg.m_iBonusItemIndex = iIndex;
		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_FINISH_TASK, &reqMsg);

		if(bCreateMsg)
		{
			GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM,"请求完成任务 bCreateMsg success id=[%d]",lTaskID);
			m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
		}

		if (m_pkTaskMap)
		{
			m_pkTaskMap->RemoveTaskProcessing(lTaskID);
		}
	} 
	/*else
	{
		SendLogicInfoMessage(iResult);
	}*/

	return true;

}
//--------------------------------------------------------
void LC_ClientGamePlayer::ReqCancelTask(int lTaskID,bool bRecordFail)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_CancelTask reqMsg;
	reqMsg.m_lPlayerID = GetSelfID();
	reqMsg.m_lTaskID = lTaskID;
	if (bRecordFail)
		reqMsg.m_iCancelType = TCT_FAIL;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_CANCEL_TASK, &reqMsg);

	if(bCreateMsg)
	{
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}
//--------------------------------------------------------
void LC_ClientGamePlayer::ReqShareTask(int lTaskID)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_ShareTask reqMsg;
	reqMsg.m_lPlayerID = GetID();
	reqMsg.m_lTaskID = lTaskID;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SHARE_TASK, &reqMsg);

	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

void LC_ClientGamePlayer::ReqTakeChapterReward(int iChapterID)
{
	MG_REQ_TakeChapterReward reqMsg;
	reqMsg.m_lChapterID = iChapterID;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_TAKECHAPTERREWARD, &reqMsg);
	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

void LC_ClientGamePlayer::ReqTakeReachReward(int iReachID)
{
	MG_REQ_MainTaskReachReward reqMsg;
	reqMsg.m_lReachRewardID = iReachID;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_MAINTASKREACHREWARD, &reqMsg);
	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
//--------------------------------------------------------
void LC_ClientGamePlayer::ObtainRewardFlag(bool kFlag)
{
	m_obtainFlag = kFlag;
}
//--------------------------------------------------------
void LC_ClientGamePlayer::RltAcceptTask(int lTaskID)
{
	//GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"回应接取任务[%d]，时间[%f]",lTaskID,GET_PROCESS_TIME);
	LC_ClientTaskManager* pkTaskManager = (LC_ClientTaskManager*)LC_ClientTaskManager::GetSingletonPtr();
	if (pkTaskManager == NULL)
		return;

	if (m_pkTaskMap == NULL)
		return;

	LC_ClientTask* pkTask = (LC_ClientTask*)pkTaskManager->GetTask(lTaskID);
	if (NULL == pkTask)
		return;

	for(int j = 0 ;j < MAX_TASK_CAN_ACCEPT_COUNT; j++)
	{
		LC_CanStartTaskMapEntry* rkCanStartTaskEntry =((LC_ClientTaskMap*)m_pkTaskMap)->GetCanStartTaskMapEntry(j);

		if (rkCanStartTaskEntry->GetTaskID() == lTaskID)
			rkCanStartTaskEntry->Reset();
	}	
	((LC_ClientTaskMap*)m_pkTaskMap)->SetDirtyFlag(DIRTY_FLAG_CAN_ACCEPT_TASK,true);
	((LC_ClientTaskMap*)m_pkTaskMap)->AddDirtyFlagRevisionID(DIRTY_FLAG_CAN_ACCEPT_TASK);


	m_pkTaskMap->AddTaskToPorcessingMap(lTaskID);
	TaskDeltaRecordData	objTaskDate;
	pkTask->InitDeltaRecordData(objTaskDate);

	((LC_ClientTaskMap*)m_pkTaskMap)->AddToTaskDeltaRecordDate(objTaskDate);

	pkTask->Start(this);
	UpdateNearNPCTaskState();
	((LC_ClientTaskMap*)m_pkTaskMap)->UpdateCanAcceptTask();
	int iTaskType = pkTask->GetTaskType();
	if (iTaskType != STT_INVISIBLE_TASK && iTaskType != STT_DAILY_CHALLENGE && iTaskType != STT_SECRET_UP && iTaskType != STT_RAID_FIRST_LIGHT)
	{
		StringType taskName = pkTask->GetTaskName();
		if (iTaskType == STT_DAILY_TASK || iTaskType == STT_GUILD_TASK || pkTask->GetTaskType() == STT_FRONTIER)
		{
			int iAssignType = LC_ClientTaskManager::GetSingleton().GetAssignType(iTaskType);
			LC_AssignTaskData* pkAssignTaskData = m_pkTaskMap->GetAssignTaskData(iAssignType);
			LC_TaskAssignEntry* pkAssignEntry = LC_ClientTaskManager::GetSingleton().GetTaskAssignEntry(iAssignType);
			if (NULL != pkAssignTaskData && NULL != pkAssignEntry)
			{
				taskName = taskName + "(" + GUISys::Convert::NumToString(pkAssignTaskData->GetRound()) + "/" + GUISys::Convert::NumToString(pkAssignEntry->_iMaxRound) + ")";
			}
		}
		
		SendLogicInfoMessage(RE_TASK_START_SUCCESS,pkTaskManager->GetTaskTypeTitle(pkTask->GetTaskTitleType()).c_str(),taskName.c_str());
		if(lTaskID == FIRST_TASK_ID)
			commitPatchProgress(EVENT_CLIENT_RLT_ACCEPT_1st_TASK, 0);
	}

	LC_TaskExtendParam* pkParam = pkTask->GetTaskStartGetItem();
	if (pkParam == NULL)
		return;

	for (int i = 0;i < pkParam->GetParamCount();i++)
	{
		Utility::UT_Vec3Int* pkVec3Int = pkParam->GetParam(i);
		Utility::UT_Vec3Int* pkVec = pkParam->GetParam(i);
		if (pkVec&&pkVec->x > 0)
		{
			LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
			LC_ItemBase* pkItem = pkItemFactory->RequestItem(pkVec->x);
			if(pkItem)
			{
				LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
				LC_ItemBase* pkItem = pkItemFactory->RequestItem(pkVec->x);
				if(pkItem)
				{
					StringType kItemName	= "";
					char numstr[100];
					ltoa(pkVec->x,numstr,10);
					kItemName = kItemName + "[@" + numstr +"]";

					SendLogicInfoMessage(RE_TASK_START_GET_ITEM,kItemName.c_str(),pkVec->y);
				}
			}
		}
	}
	// 	LC_AssignTaskInfo* pkAssignInfo = m_pkTaskMap->GetAssignTaskInfo();
	// 	if (pkAssignInfo)
	// 		pkAssignInfo->RemoveTask(TAT_NONE,lTaskID);
}
//--------------------------------------------------------
void LC_ClientGamePlayer::RltJoinInTask( int lTaskID )
{
	if (m_pkTaskMap == NULL)
		return;

	LC_ClientTask* pkTask = (LC_ClientTask*)LC_ClientTaskManager::GetSingletonPtr()->GetTask(lTaskID);
	if (NULL == pkTask)
		return;

	for(int j = 0 ;j < MAX_TASK_CAN_ACCEPT_COUNT; j++)
	{
		LC_CanStartTaskMapEntry* rkCanStartTaskEntry =((LC_ClientTaskMap*)m_pkTaskMap)->GetCanStartTaskMapEntry(j);

		if (rkCanStartTaskEntry->GetTaskID() == lTaskID)
			rkCanStartTaskEntry->Reset();
	}	
	((LC_ClientTaskMap*)m_pkTaskMap)->SetDirtyFlag(DIRTY_FLAG_CAN_ACCEPT_TASK,true);
	((LC_ClientTaskMap*)m_pkTaskMap)->AddDirtyFlagRevisionID(DIRTY_FLAG_CAN_ACCEPT_TASK);
	m_pkTaskMap->AddTaskToPorcessingMap(lTaskID);
	TaskDeltaRecordData	objTaskDate;
	pkTask->InitDeltaRecordData(objTaskDate);
	((LC_ClientTaskMap*)m_pkTaskMap)->AddToTaskDeltaRecordDate(objTaskDate);

	pkTask->JoinInMultiuserTask(this);
	UpdateNearNPCTaskState();
	((LC_ClientTaskMap*)m_pkTaskMap)->UpdateCanAcceptTask();

	LC_TaskExtendParam* pkParam = pkTask->GetTaskStartGetItem();
	if (pkParam == NULL)
		return;

	for (int i = 0;i < pkParam->GetParamCount();i++)
	{
		Utility::UT_Vec3Int* pkVec3Int = pkParam->GetParam(i);
		Utility::UT_Vec3Int* pkVec = pkParam->GetParam(i);
		if (pkVec&&pkVec->x > 0)
		{
			LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
			LC_ItemBase* pkItem = pkItemFactory->RequestItem(pkVec->x);
			if(pkItem)
			{
				SendLogicInfoMessage(RE_TASK_START_GET_ITEM,pkItem->GetItemDescriptionEntry()->GetItemName().c_str(),pkVec->y);
			}
		}
	}

	StringType InfoText = GET_MSG(LC_CLIENTGAMEPLAYER_CPP_002) + pkTask->GetTaskName() + GET_MSG(LC_SERVERTASK_CPP_007);
	SendLogicInfoMessage(RE_LOGIC_INFO_TAB_MESSAGE,InfoText.c_str());
	SendLogicInfoMessage(RE_TIP_MESSAGE,InfoText.c_str());
}
//--------------------------------------------------------
void LC_ClientGamePlayer::RltFinishTask(int lTaskID,int iItemIndex)
{
	//GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"回应完成任务[%d]，时间[%f]",lTaskID,GET_PROCESS_TIME);
	if (m_pkTaskMap == NULL)
		return;

	LC_ClientTaskManager* pkTaskManager = (LC_ClientTaskManager*)LC_ClientTaskManager::GetSingletonPtr();
	if (pkTaskManager == NULL)
		return;

	LC_ClientTask* pkTask = (LC_ClientTask*)(pkTaskManager->GetTask(lTaskID));
	if (pkTask == NULL)
		return;

	int iExp  = 0;
	int lBindCash = 0;
	int lUnBindCash = 0;

	LC_ProcessingTaskEntry* pkProcessingTaskEntry = m_pkTaskMap->GetProcessingTaskEntryByID(lTaskID);
	if (pkProcessingTaskEntry)
	{
		iExp  = pkTask->ComputeExpBonus(pkProcessingTaskEntry,this);
		lBindCash= 0;
		lUnBindCash = pkTask->ComputeUnBindGoldBonus(pkProcessingTaskEntry,this);
		m_pkTaskMap->RemoveTaskProcessing(lTaskID);
	}


	int iTaskType = pkTask->GetTaskType();
	if ( iTaskType != STT_BILLBOARD_TASK && iTaskType != STT_DAILY_TASK && iTaskType != STT_GUILD_TASK && iTaskType != STT_FRONTIER)
	{
		m_pkTaskMap->AddToTaskFinishRecord(lTaskID);
	}
	else if(iTaskType == STT_DAILY_TASK || iTaskType == STT_GUILD_TASK || iTaskType == STT_FRONTIER)
	{
		((LC_ClientTaskMap *)m_pkTaskMap)->AddToCanStartList(lTaskID);
	}

	LC_TaskTypeData* pkTypeData = m_pkTaskMap->GetTypeData(iTaskType);
	if (pkTypeData)
	{
		pkTypeData->OnTaskFinished(pkTask,GET_PROCESS_TIME);
	}
	// 	LC_AssignTaskInfo* pkAssignInfo = m_pkTaskMap->GetAssignTaskInfo();
	// 	if (pkAssignInfo&&(iTaskType == STT_GUILD_TASK))
	// 		pkAssignInfo->RemoveTask(TAT_GUILD,lTaskID);

	((LC_ClientTaskMap*)m_pkTaskMap)->RemoveFromTaskForbidTraceList(lTaskID);
	((LC_ClientTaskMap*)m_pkTaskMap)->RemoveTaskDeltaRecordDate(lTaskID);
	AddDirtyFlagRevisionID(DIRTY_FLAG_FINISHEDTASK);
	AddDirtyFlagRevisionID(DIRTY_FLAG_PROCESSINGTASK);


	pkTask->Finish(this,iItemIndex);

	FinishGuideTask(pkTask);

	//pkTask->GetTaskStarLevel() == 2表示是对话任务
	if(iTaskType == STT_MAIN_SCENARIO && pkTask->GetTaskStarLevel() == 2)
		m_bNpcTask = false;

	UpdateNearNPCDynamicCollision();
	UpdateNearNPCTaskState();
	((LC_ClientTaskMap*)m_pkTaskMap)->UpdateCanAcceptTask();

	if (iTaskType != STT_INVISIBLE_TASK && iTaskType != STT_DAILY_CHALLENGE && iTaskType != STT_SECRET_UP && iTaskType != STT_RAID_FIRST_LIGHT)
	{
		StringType taskName = pkTask->GetTaskName();
		if (iTaskType == STT_DAILY_TASK || iTaskType == STT_GUILD_TASK || iTaskType == STT_FRONTIER)
		{
			int iAssignType = LC_ClientTaskManager::GetSingleton().GetAssignType(iTaskType);
			LC_AssignTaskData* pkAssignTaskData = m_pkTaskMap->GetAssignTaskData(iAssignType);
			LC_TaskAssignEntry* pkAssignEntry = LC_ClientTaskManager::GetSingleton().GetTaskAssignEntry(iAssignType);
			if (NULL != pkAssignTaskData && NULL != pkAssignEntry)
			{
				taskName = taskName + "(" + GUISys::Convert::NumToString(pkAssignTaskData->GetRound()) + "/" + GUISys::Convert::NumToString(pkAssignEntry->_iMaxRound) + ")";
			}

			//通知界面: 当前日常/公会任务 10环全完成，弹出最终奖励界面
			LC_AssignTaskInfo* pkAssignTaskInfo = m_pkTaskMap->GetAssignTaskInfo();
			if(/*pkAssignTaskInfo &&*/ pkAssignTaskInfo->IsAssignRoundOff(iAssignType, false) && !pkAssignTaskInfo->IsAssignedReward(iAssignType))
			{
				LC_GameEvent kGameEvent;
				kGameEvent.SetEventType(GLET_CAN_FINISH_TASK);
				kGameEvent.SetEventParam(EP_CAN_FINISH_TASK_ID, lTaskID);
				kGameEvent.SetEventParam(EP_CAN_FINISH_TASK_TYPE, iTaskType);
				kGameEvent.SetEventParam(EVENT_BASE_PARAM_3, iTaskType);

				UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
				pkUIManager->HandlerGameEvent(kGameEvent);
			}
		}

		SendLogicInfoMessage(RE_TASK_FINISH_SUCCESS_NOTICE,pkTaskManager->GetTaskTypeTitle(pkTask->GetTaskTitleType()).c_str(),taskName.c_str());
	}

	// 	if (pkTask->GetTaskBonusEXP() > 0)
	// 	{
	// 		if (iExp <= 0)
	// 			iExp = pkTask->GetTaskBonusEXP();
	// 		SendLogicInfoMessage(RE_TASK_BONUS_EXP,iExp);
	// 	}

	// 	if (lBindCash)
	// 	{
	// 		int64_t iGold,iSilver,iCopper;
	// 		iCopper = lBindCash % 100 ;
	// 		iSilver = (lBindCash % 10000)/100;
	// 		iGold   = lBindCash / 10000;
	// 
	// 		StringType kGold	= "";
	// 		StringType kSilver	= "";
	// 		StringType kCopper	= "";
	// 		if (iCopper > 0) 
	// 		{
	// 			char numstr[100];
	// 			ltoa(iCopper,numstr,10);
	// 			kCopper = kCopper + numstr + "<font val = 8>G</font>";
	// 		}
	// 
	// 		if (iSilver > 0) 
	// 		{
	// 			char numstr[100];
	// 			ltoa(iSilver,numstr,10);
	// 			kSilver = kSilver + numstr + "<font val = 8>F</font>";
	// 		}
	// 
	// 		if (iGold > 0) 
	// 		{
	// 			char numstr[100];
	// 			ltoa(iGold,numstr,10);
	// 			kGold = kGold + numstr + "<font val = 8>E</font>";
	// 		}
	// 
	// 
	// 		SendLogicInfoMessage(RE_TASK_BONUS_CASH,kGold.c_str(),kSilver.c_str(),kCopper.c_str());
	// 	}
	// 
	// 	if (lUnBindCash > 0)
	// 	{
	// 		int64_t iGold,iSilver,iCopper;
	// 		iCopper = lUnBindCash % 100 ;
	// 		iSilver = (lUnBindCash % 10000)/100;
	// 		iGold   = lUnBindCash / 10000;
	// 
	// 		StringType kGold	= "";
	// 		StringType kSilver	= "";
	// 		StringType kCopper	= "";
	// 		if (iCopper > 0) 
	// 		{
	// 			char numstr[100];
	// 			ltoa(iCopper,numstr,10);
	// 			kCopper = kCopper + numstr + "<font val = 8>V</font>";
	// 		}
	// 
	// 		if (iSilver > 0) 
	// 		{
	// 			char numstr[100];
	// 			ltoa(iSilver,numstr,10);
	// 			kSilver = kSilver + numstr + "<font val = 8>U</font>";
	// 		}
	// 
	// 		if (iGold > 0) 
	// 		{
	// 			char numstr[100];
	// 			ltoa(iGold,numstr,10);
	// 			kGold = kGold + numstr + "<font val = 8>T</font>";
	// 		}
	// 
	// 		SendLogicInfoMessage(RE_TASK_BONUS_UNBIND_CASH,kGold.c_str(),kSilver.c_str(),kCopper.c_str());
	// 	}

	LC_TaskExtendParam* pkParam = pkTask->GetTaskBonusItemParam();
	if (pkParam == NULL)
		return;

	if (pkTask->IsSelectBonus())
	{
		if (pkParam->GetParamCount() > 0)
		{
			Utility::UT_Vec3Int* pkVec = pkParam->GetParam(iItemIndex);
			if (pkVec&&pkVec->x > 0)
			{
				LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
				LC_ItemBase* pkItem = pkItemFactory->RequestItem(pkVec->x);
				if(pkItem)
				{
					StringType kItemName	= "";
					char numstr[100];
					ltoa(pkVec->x,numstr,10);
					kItemName = kItemName + "[@" + numstr +"]";

					SendLogicInfoMessage(RE_TASK_BONUS_ITEM,kItemName.c_str(),pkVec->y);
				}
			}		
		}	
	}
	else
	{
		for (int i=0; i < pkParam->GetParamCount(); i++)
		{
			Utility::UT_Vec3Int* pkVec = pkParam->GetParam(i);
			if (pkVec&&pkVec->x > 0)
			{
				LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
				LC_ItemBase* pkItem = pkItemFactory->RequestItem(pkVec->x);
				if(pkItem)
				{
					StringType kItemName	= "";
					char numstr[100];
					ltoa(pkVec->x,numstr,10);
					kItemName = kItemName + "[@" + numstr +"]";

					SendLogicInfoMessage(RE_TASK_BONUS_ITEM,kItemName.c_str(),pkVec->y);
				}
			}
		}
	}

	pkParam = pkTask->GetFininshOperationParam();
	//if (pkParam)
	{
		for (int i = 0;i < pkParam->GetParamCount();i++)
		{
			Utility::UT_Vec3Int* pkVec3Int = pkParam->GetParam(i);
			if (NULL == pkVec3Int)
				continue;

			if (pkVec3Int->x == TFOP_GUILD_MILESTONE_TARGET)
			{
				SendLogicInfoMessage(RE_TASK_BONUS_GUILD_MILESTONE_TARGET,pkVec3Int->y);
			}
			else if (pkVec3Int->x == TFOP_GUILD_EXP)
			{
				SendLogicInfoMessage(RE_TASK_BONUS_GUILD_EXP,pkVec3Int->y);
			}
			else if (pkVec3Int->x == TFOP_GUILD_CONTRIBUTE)
			{
				SendLogicInfoMessage(RE_TASK_BONUS_GUILD_CONTRIBUTE,pkVec3Int->y);
			}
			else if (pkVec3Int->x == TFOP_GUILD_CASH)
			{
				int iGold,iSilver,iCopper;
				int lCash = pkVec3Int->y;
				iCopper = lCash % 100 ;
				iSilver = (lCash % 10000)/100;
				iGold   = lCash / 10000;

				SendLogicInfoMessage(RE_TASK_BONUS_GUILD_CASH,iGold,iSilver,iCopper);
			}
		}
	}

	((LC_ClientTaskMap*)m_pkTaskMap)->RemoveConvoyNPCInfo(lTaskID);
}
//--------------------------------------------------------
void LC_ClientGamePlayer::RltCancelTask(int lTaskID)
{
	if (m_pkTaskMap == NULL)
		return;

	LC_ProcessingTaskEntry* pkProcessEntry = m_pkTaskMap->GetProcessingTaskEntryByID(lTaskID);
	if (pkProcessEntry)
	{
		m_pkTaskMap->RemoveTaskProcessing(lTaskID);
		((LC_ClientTaskMap*)m_pkTaskMap)->RemoveFromTaskForbidTraceList(lTaskID);
	}

	AddDirtyFlagRevisionID(DIRTY_FLAG_PROCESSINGTASK);
	((LC_ClientTaskMap*)m_pkTaskMap)->RemoveTaskDeltaRecordDate(lTaskID);
	LC_ClientTask* pkTask = (LC_ClientTask*)LC_ClientTaskManager::GetSingletonPtr()->GetTask(lTaskID);
	if (pkTask)
		pkTask->Cancel(this);

	UpdateNearNPCTaskState();
	((LC_ClientTaskMap*)m_pkTaskMap)->UpdateCanAcceptTask();
	//BUG #8634 【1.29押镖活动系统体验】
	//这里取消活动相关任务的失败提醒
	if (pkTask->GetGameStoryID() == 0)
		SendLogicInfoMessage(RE_TASK_FAILED,pkTask->GetTaskName().c_str());
	((LC_ClientTaskMap*)m_pkTaskMap)->RemoveConvoyNPCInfo(lTaskID);
}
//--------------------------------------------------------
void LC_ClientGamePlayer::RltShareTask(int lTaskID)
{

}
//--------------------------------------------------------
void LC_ClientGamePlayer::AcceptGuideTask(LC_Task* pkTask)
{
	if ( !pkTask )
	{
		m_opFinishDailyTask = WOGT_NONE;
		return ;
	}

	int iTaskType = pkTask->GetTaskType();

	LC_ClientTaskMap* pkClientTaskMap = (LC_ClientTaskMap*)m_pkTaskMap;
	LC_ClientTaskManager* pkTaskManager = (LC_ClientTaskManager*)LC_ClientTaskManager::GetSingletonPtr();
	GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI, "LC_ClientGamePlayer::AcceptGuideTask m_fGudingTimer=[%f] CommanderLevel=[%d] inRaidMap=[%d] m_bFinishTaskFlag=[%d] m_bTutorialStop=[%d]",
		m_fGudingTimer, GetCommanderLevel(), IS_NORMAL_MAP(GetMapLogicType()), m_bFinishTaskFlag, m_bTutorialStop);
	if ( IsNeedToGuideTask() )
	{
		//原因1：玩家45级完成主线任务的时候，可能会升级到46级，46级时新手全自动引导的分界线，新手全自动引导的时候是不响应任务引导操作的，
		//出现情况：玩家收到45级完成任务消息是，会升级到46级，但是经验不一定下来，导致客户端此时是45级，处于新手全自动引导阶段，此时自动任务不响应，
		//角色接到46级主线，并且升级到46级时，也不会进行自动任务引导，就卡住了。
		//原因2：完成主线任务的时候，如果下一个主线任务等级比当前角色等级高，此时并不知道下一个主线任务能否接上，
		//因为可能任务完成经验刚好升级，或者别的原因导致升级
		//因此：完成主线任务和接到下一个主线任务时间间隔在2.0s内，而且玩家升级了，就认为，是合法的
		//if (m_lastMainTaskTick > 0 && GET_PROCESS_TIME - m_lastMainTaskTick < 2.0f && GetLevel() != m_lastMainTaskLevel )
		if (m_lastMainTaskTick > 0 && GET_PROCESS_TIME - m_lastMainTaskTick < 2.0f && GetCommanderLevel() != m_lastMainTaskLevel )
		{
			LC_Task* ffTask = LC_TaskManager::GetSingleton().GetTask(m_lastMainTaskID);
			if ( ffTask )
			{
				FinishGuideTask(ffTask);
			}
			m_lastMainTaskID = 0;
			m_lastMainTaskTick = 0;
			m_lastMainTaskLevel = 0;
		}

		int pushID = 0;
		//第2章开始，每章节第1个任务接取后不继续自动任务
		bool bStopGuide = pkTask->GetTaskGroupID() > 1 && pkTask->GetSectionID() == 1;

		bool needFind = !bStopGuide && m_opFinishDailyTask != WOGT_NONE;
		bool bIsAlliance = LC_Helper::IsAllianceMap(GetMapResID());

		bool bTargetAlliance = false;
		bTargetAlliance = pkClientTaskMap->TaskTraceInfoMpaIsAlliance(pkTask);

		//主线任务、帮会任务，日常任务完成
		//1. 接到主线任务，继续做
		if (needFind && iTaskType == STT_MAIN_SCENARIO && !bIsAlliance && !bTargetAlliance)
		{
			SetAutoTaskType(iTaskType);
			pushID = pkTask->GetTaskID();
		}
		//2. 没有主线任务做日常任务
		//else if ( needFind && iTaskType == STT_DAILY_TASK && !bIsAlliance)
		//{
		//	bool hasMainTask = pkClientTaskMap->IsExistTaskInProcessingTaskByType(STT_MAIN_SCENARIO);
		//	int mainTaskIndex =  pkClientTaskMap->GetMainTaskPaceIndex();
		//	LC_Task* pkMainTask = pkTaskManager->GetMainTaskByIndex(mainTaskIndex);
		//	bool falliance = pkClientTaskMap->TaskTraceInfoMpaIsAlliance(pkMainTask);
		//	if ( hasMainTask && pkMainTask && !falliance)
		//	{						
		//		pushID = pkTask->GetTaskID();
		//	}
		//	else
		//	{
		//		pushID = pkTask->GetTaskID();
		//	}
		//}
		////3. 没有主线任务，没有日常任务，做帮会任务
		//else if ( needFind && iTaskType == STT_GUILD_TASK && !bIsAlliance)
		//{
		//	StringType idsstr;
		//	bool hasMainTask = pkClientTaskMap->IsExistTaskInProcessingTaskByType(STT_MAIN_SCENARIO);
		//	int mainTaskIndex =  pkClientTaskMap->GetMainTaskPaceIndex();
		//	LC_Task* pkMainTask = pkTaskManager->GetMainTaskByIndex(mainTaskIndex);
		//	bool falliance = pkClientTaskMap->TaskTraceInfoMpaIsAlliance(pkMainTask);
		//	if ( hasMainTask && pkMainTask && !falliance)
		//	{	
		//		pushID = pkTask->GetTaskID();
		//	}
		//	else if ( (idsstr = pkClientTaskMap->GetTaskIDListByTaskType(STT_DAILY_TASK)) != "" )
		//	{
		//		pushID = atoi(idsstr.c_str());
		//	}
		//	else
		//	{
		//		pushID = pkTask->GetTaskID();
		//	}
		//}
		//else if ( needFind && iTaskType == STT_FRONTIER && bIsAlliance)
		//{
		//	StringType idsstr;
		//	if ( (idsstr = pkClientTaskMap->GetTaskIDListByTaskType(STT_FRONTIER)) != "" )
		//	{
		//		pushID = atoi(idsstr.c_str());
		//	}
		//	else
		//	{
		//		pushID = pkTask->GetTaskID();
		//	}
		//}

		//日常/公会接取第1环任务后开始自动任务
		if(iTaskType == STT_DAILY_TASK || iTaskType == STT_GUILD_TASK)
		{
			uint32_t assignType = LC_TaskManager::GetSingleton().GetAssignType(iTaskType);
			LC_AssignTaskData* pkAssignTaskData = pkClientTaskMap->GetAssignTaskInfo()->GetAssignTaskData(assignType);
			if(pkAssignTaskData && pkAssignTaskData->GetRound() == 1)
			{
				SetAutoTaskType(iTaskType);
				pushID  = pkTask->GetTaskID();
			}
		}
		
		if ( pushID > 0 )
		{
			GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI, "LC_ClientGamePlayer::AcceptGuideTask GuildPush pushID=[%d]", pushID);
			((SM_ClientGamePlayerController*)m_pkSMController)->StopAutoGuide();
			((SM_ClientGamePlayerController*)m_pkSMController)->GuildPush(this, pushID);
			m_opFinishDailyTask = WOGT_NONE;
		}
	}
}
//--------------------------------------------------------
void LC_ClientGamePlayer::FinishGuideTask(LC_Task* pkTask)
{
	if ( !pkTask || m_obtainFlag )
	{
		m_obtainFlag = false;
		m_opFinishDailyTask = WOGT_NONE;
		return ;
	}
	LC_ClientTaskManager* pkTaskManager = (LC_ClientTaskManager*)LC_ClientTaskManager::GetSingletonPtr();
	GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI, "LC_ClientGamePlayer::FinishGuideTask m_fGudingTimer=[%f] CommanderLevel=[%d] inRaidMap=[%d]", 
		m_fGudingTimer, GetCommanderLevel(), IS_NORMAL_MAP(GetMapLogicType()));
	if ( IsNeedToGuideTask() )
	{
		int pushID = 0;
		
		//主线任务、帮会任务，日常任务完成
		//1. 下一个主线任务能做 or 存在日常任务能做 or 存在下一个帮会任务，等待下一个任务接上再做(一定能接到下一个任务)
		//2. 下一个主线任务不能接 or 日常任务都做完了 or 班会任务都做完了，进行一次搜索(因为下一个任务一定没法接）

		bool needFind = false;
		bool bIsAlliance = LC_Helper::IsAllianceMap(GetMapResID());

		bool bTargetAlliance = false;
		LC_ClientTaskMap* pkClientTaskMap = (LC_ClientTaskMap*)m_pkTaskMap;
		bTargetAlliance = pkClientTaskMap->TaskTraceInfoMpaIsAlliance(pkTask);

		//判断主线任务
		if ( pkTask->GetTaskType() == STT_MAIN_SCENARIO && !bIsAlliance && !bTargetAlliance )
		{
			LC_Task* ptm = pkTaskManager->GetMainTaskByIndex(pkTask->GetMainIndex()+1);
			//if ( ptm && ptm->GetAcceptLevel() > GetLevel() && GetLevel() >= g_settings.DS_GUIDE_END_LEVEL)
			if ( ptm && ptm->GetAcceptLevel() > GetCommanderLevel() && GetCommanderLevel() >= g_settings.DS_GUIDE_END_LEVEL)
			{
				needFind = true;
			}
			else
			{
				m_opFinishDailyTask = WOGT_MAIN_FIN;
			}
		}
		//判断帮会任务和日常任务
		//else if ( (pkTask->GetTaskType() == STT_DAILY_TASK || pkTask->GetTaskType() == STT_GUILD_TASK) && !bIsAlliance)
		//{
		//	uint32_t assignType = LC_TaskManager::GetSingleton().GetAssignType(pkTask->GetTaskType());
		//	LC_AssignTaskData* assignData = m_pkTaskMap->GetAssignTaskData(assignType);
		//	LC_TaskAssignEntry* pkAssignEntry = LC_ClientTaskManager::GetSingleton().GetTaskAssignEntry(assignType);
		//	if ( assignData && pkAssignEntry)
		//	{
		//		uint32_t rd = assignData->GetRound();
		//		if ( rd < pkAssignEntry->_iMaxRound )
		//		{
		//			m_opFinishDailyTask = pkTask->GetTaskType() == STT_DAILY_TASK ? WOGT_DAILY_PART_FIN : WOGT_GUILD_PART_FIN; 
		//		}
		//		else
		//		{
		//			//m_opFinishDailyTask = pkTask->GetTaskType() == STT_DAILY_TASK ? WOGT_DAILY_WHOLE_FIN :WOGT_GUILD_WHOLE_FIN;
		//			Reset2Guding(true);
		//			needFind = true;
		//		}
		//	}
		//}
		//else if ( pkTask->GetTaskType() == STT_FRONTIER && bIsAlliance)
		//{
		//	uint32_t assignType = LC_TaskManager::GetSingleton().GetAssignType(pkTask->GetTaskType());
		//	LC_AssignTaskData* assignData = m_pkTaskMap->GetAssignTaskData(assignType);
		//	LC_TaskAssignEntry* pkAssignEntry = LC_ClientTaskManager::GetSingleton().GetTaskAssignEntry(assignType);
		//	if ( assignData && pkAssignEntry)
		//	{
		//		uint32_t rd = assignData->GetRound();
		//		if ( rd < pkAssignEntry->_iMaxRound )
		//		{
		//			m_opFinishDailyTask = pkTask->GetTaskType() == STT_FRONTIER ? WOGT_FRONTIER_PART_FIN : WOGT_FRONTIER_WHOLE_FIN; 
		//		}
		//		else
		//		{
		//			//m_opFinishDailyTask = pkTask->GetTaskType() == STT_DAILY_TASK ? WOGT_DAILY_WHOLE_FIN :WOGT_GUILD_WHOLE_FIN;
		//			needFind = false;
		//		}
		//	}
		//}
		else if ( pkTask->GetTaskType() == STT_CRUSADE_TOKEN && !bIsAlliance)
		{
			int lTaskID = GetClientTaskMap()->GetFirstProcessingTaskIDByTaskType(pkTask->GetTaskType());
			LC_Task* newTask = LC_TaskManager::GetSingleton().GetTask(lTaskID);
			if ( newTask )
			{
				((SM_ClientGamePlayerController*)m_pkSMController)->GuildPush(this, lTaskID);
				m_opFinishDailyTask = WOGT_NONE;
				needFind = false;
			}
			else
			{
				needFind = true;
			}
		}
		
		if ( needFind )
		{
			StringType idsstr;
			bool hasMainTask = ((LC_ClientTaskMap*)m_pkTaskMap)->IsExistTaskInProcessingTaskByType(STT_MAIN_SCENARIO);
			int mainTaskIndex =  ((LC_ClientTaskMap*)m_pkTaskMap)->GetMainTaskPaceIndex();
			LC_Task* pkMainTask = pkTaskManager->GetMainTaskByIndex(mainTaskIndex);
			bool falliance = pkClientTaskMap->TaskTraceInfoMpaIsAlliance(pkMainTask);
			if ( hasMainTask && pkMainTask && !falliance)
			{
				pushID = pkMainTask->GetTaskID();
			}
			else if ( (idsstr = ((LC_ClientTaskMap*)m_pkTaskMap)->GetTaskIDListByTaskType(STT_DAILY_TASK)) != "" )
			{
				pushID = atoi(idsstr.c_str());
			}
			else if ( (idsstr = ((LC_ClientTaskMap*)m_pkTaskMap)->GetTaskIDListByTaskType(STT_GUILD_TASK)) != "" )
			{
				pushID = atoi(idsstr.c_str());
			}
			GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI, "LC_ClientGamePlayer::FinishGuideTask GuildPush pushID=[%d]", pushID);
			((SM_ClientGamePlayerController*)m_pkSMController)->GuildPush(this, pushID);
		}
	}

	if ( pkTask->GetTaskType() == STT_MAIN_SCENARIO )
	{
		m_lastMainTaskTick = GET_PROCESS_TIME;
		m_lastMainTaskID = pkTask->GetTaskID();
		//m_lastMainTaskLevel = GetLevel();
		m_lastMainTaskLevel = GetCommanderLevel();
	}
}
//--------------------------------------------------------
bool LC_ClientGamePlayer::IsNeedToGuideTask()
{
	NW_ClientSessionManager* pkSessionManager = NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup* pkGameSessionGroup = pkSessionManager->GetGameSessionGroup();
	int eGameStage = pkGameSessionGroup->m_pkGameStageMachine->GetCurrentStateType();
	//在副本内不执行
	bool inRaidMap = !IS_NORMAL_MAP(GetMapLogicType());
	//return (Get2GudingTime() >= 0 || GetLevel() >= g_settings.DS_GUIDE_END_LEVEL ) && eGameStage == ST_GAME_STAGE_GAME && !inRaidMap;
	return ( m_bFinishTaskFlag || (Get2GudingTime() >= 0 && !m_bTutorialStop) || GetCommanderLevel() >= g_settings.DS_GUIDE_END_LEVEL ) && eGameStage == ST_GAME_STAGE_GAME && !inRaidMap;
}
//--------------------------------------------------------
void LC_ClientGamePlayer::RequestConfirmShareTask(int lPublisherID,int lTaskID,bool bAccept)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_ConfirmShareTask reqMsg;
	reqMsg.m_lPublisherID = lPublisherID;
	reqMsg.m_lTaskID = lTaskID;
	reqMsg.m_lPlayerID = GetID();
	TPSTR2STDSTR(GetCharName(), reqMsg.m_strPlayerName);
	if (bAccept)
	{
		reqMsg.m_iConfirmResult = RE_TASK_ACCEPT_SHARE;
	}
	else
	{
		reqMsg.m_iConfirmResult = RE_TASK_DENY_SHARE;
	}
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_CONFIRM_SHARE_TASK, &reqMsg);
	if(bCreateMsg)
	{
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}
//------------------------------------------------------------------------
void	LC_ClientGamePlayer::RequestNocticeTaskEvent(int lTaskID,int iEventType,int lParam1/*= 0*/,int lParam2/*= 0*/)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_NoticeTaskEvent reqMsg;
	reqMsg.m_lPlayerID = GetID();
	reqMsg.m_lTaskID   = lTaskID;
	reqMsg.m_iTaskEvent= iEventType;
	reqMsg.m_lParam1   = lParam1;
	reqMsg.m_lParam2   = lParam2;

	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_NOTICE_TASK_EVENT, &reqMsg);

	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
//------------------------------------------------------------------------
void	LC_ClientGamePlayer::RequestBillBoardTask(int lBillBoardID)
{
	BillboardTaskData*  pkData = m_pkTaskMap->GetBillboardDataFormCanStarList(lBillBoardID);
	if (pkData == NULL)
	{
		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = false;

		MG_REQ_InitBillBoardTask reqMsg;
		reqMsg.m_lPlayerID		= GetID();
		reqMsg.m_lBillBaordID   = lBillBoardID;

		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_BILLBOARD_TASK_INFO, &reqMsg);

		if(bCreateMsg)
			m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
	else
	{
		((LC_ClientTaskMap*)m_pkTaskMap)->SetShowBillboardFlag(true);
	}

}
//------------------------------------------------------------------------
void    LC_ClientGamePlayer::UpdateNearNPCTaskState()
{
	LC_ClientNPC* pkNPC = NULL;
	LC_ClientTaskManager* pkTaskManager = (LC_ClientTaskManager*)LC_ClientTaskManager::GetSingletonPtr();
	if (pkTaskManager == NULL||m_pkTaskMap == NULL)
		return;

	LC_ClientTaskMap* pkClientTaskMap = (LC_ClientTaskMap*)m_pkTaskMap;
	LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
	const LC_NPCIDPtrMap& rNPCIDMap = pkNPCManager->GetNPCIDMap();
	LC_NPCIDPtrMapConstIter IterBe = rNPCIDMap.begin();
	LC_NPCIDPtrMapConstIter IterEd = rNPCIDMap.end();
	for(; IterBe != IterEd;++IterBe)
	{
		pkNPC = (LC_ClientNPC*)((*IterBe).second);
		pkClientTaskMap->SetNPCTaskState(pkNPC->GetID());
	}
}

void LC_ClientGamePlayer::UpdateNearNPCDynamicCollision(void)
{
	//LC_ClientNPC* pkNPC = NULL;
	//LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
	//const LC_NPCIDPtrMap& rNPCIDMap = pkNPCManager->GetNPCIDMap();
	//LC_NPCIDPtrMapConstIter IterBe = rNPCIDMap.begin();
	//LC_NPCIDPtrMapConstIter IterEd = rNPCIDMap.end();
	//for(; IterBe != IterEd;++IterBe)
	//{
	//	pkNPC = (LC_ClientNPC*)((*IterBe).second);
	//	pkNPC->UpdateDynamicCollision();
	//}
}
//------------------------------------------------------------------------
void	LC_ClientGamePlayer::SetTaskObjectPickState(int lCharTypeID,bool bCanPick)
{
	SectorIndex dwPlayerSector = GetSectorIndex();
	LC_ClientMap* pkMap = (LC_ClientMap*)GetMap();

	SectorIndex dwNPCSector;
	LC_ClientNPC* pkNPC = NULL;

	LC_ClientTaskManager* pkTaskManager = (LC_ClientTaskManager*)LC_ClientTaskManager::GetSingletonPtr();
	if (pkTaskManager == NULL||m_pkTaskMap == NULL)
		return;

	LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
	const LC_NPCIDPtrMap& rNPCIDMap = pkNPCManager->GetNPCIDMap();
	LC_NPCIDPtrMapConstIter IterBe = rNPCIDMap.begin();
	LC_NPCIDPtrMapConstIter IterEd = rNPCIDMap.end();
	//LC_WorldManagerBase* pkWorldManager = CLIENT_GET_WORLD_MANAGER();
	for(; IterBe != IterEd;++IterBe)
	{
		pkNPC = (LC_ClientNPC*)((*IterBe).second);
		if (pkNPC == NULL)
			continue;

		dwNPCSector = pkNPC->GetSectorIndex();
		if(!pkMap->IsInNeighborSector(dwPlayerSector,dwNPCSector))
			continue;

		// 		if(!pkNPC->IsCreateVisibleData())
		// 			continue;

		if (pkNPC->GetCharType() == lCharTypeID)
		{
			//object_id_type lState = pkNPC->GetNPCFuncState(NFT_COLLECTITEM);
			//if (bCanPick)
			//{
			//	if (lState < NFST_TASK_COLLECT)
			//	{
			//		pkNPC->SetNPCFuncState(NFT_COLLECTITEM,NFST_TASK_COLLECT+lState);
			//	}
			//}
			//else
			//{
			//	if (lState >= NFST_TASK_COLLECT)
			//		pkNPC->SetNPCFuncState(NFT_COLLECTITEM,lState - NFST_TASK_COLLECT);
			//}
		}
	}
}
//------------------------------------------------------------------------
void	LC_ClientGamePlayer::SetNPCTaskState(object_id_type lNPCID)
{
	if (m_pkTaskMap )
		((LC_ClientTaskMap*)m_pkTaskMap)->SetNPCTaskState(lNPCID);
}
//------------------------------------------------------------------------
ResultType LC_ClientGamePlayer::PendingSkill(int lSkillTypeID,int lItemID)
{
	//关闭跟随功能
	this->StopFollowTarget(0);

	ResultType eResult = LC_ActorBase::PendingSkill(lSkillTypeID,lItemID);
	GfxWriteLog(LOG_TAG_SKILL, LOG_SWITCH_INFO, "[Skill]PendingSkill = %d, result = %d, FrameID = %d", 
		lSkillTypeID, eResult, UT_TimeManager::GetSingleton().CurrentFrameNo());

	if (lSkillTypeID == m_iReqNoRltExecSkillID)
		eResult = RE_SKILL_SYSTEM_ERROR;

	if(eResult != RE_SKILL_PENDING_SUCCESS)
	{
		if (eResult != RE_SKILL_SYSTEM_ERROR)
		{
			SendLogicInfoMessage(eResult);
		}
		return eResult;
	}

	//设置技能状态为挂起状态，用于技能栏表的表现
	((LC_ClientShortcutBar*)GetShortcutBar())->SetShortcutTriggerState(lSkillTypeID, true);

	//瞬发骑马技能不需要清除 PathPendingAction
	if(lSkillTypeID != SKILL_CALL_VEHICLE_IMMMEDIATELY)
		ClearPathPendingAction();

	return eResult;
}
//------------------------------------------------------------------------
void		LC_ClientGamePlayer::OnPendingSkillSucceed(int lSkillID, LC_LogicObject *pkTarget)
{
	if ( GetAIControl() )
		return ;

	Utility::UT_Vec3Int kTargetLocation = (pkTarget != NULL) ? pkTarget->GetVisibleLocation() : UT_EngineMath::ConvertNiPoint3ToVec3Int(this->GetMouseOverLocation());

	Skill::SK_SkillExecute* pkSkill = SK_Factory::GetSingletonPtr()->RequestSkillExcute(lSkillID);
	if( !pkSkill )
		return ;

	int iOperationType				= pkSkill->GetProperty_OperationType();
	int iSkillExecuteMinDistance	= pkSkill->GetProperty_MinExecuteDistance();
	int iSkillExecuteMaxDistance	= pkSkill->GetProperty_MaxExecuteDistance();
	int	lExecuteDistanceFunc		= pkSkill->GetProperty_ExecuteDistanceFunc();
	switch (iOperationType)
	{
	case SOT_TARGET:	 //0目标 1朝向 2地点
		{
			LC_LogicObject* pkTarget = this->GetMap()->FindObject(GetLockedTargetID());
			if (pkTarget)
			{
				this->FaceToLocation(pkTarget->GetVisibleLocation());
				this->SetLockedLocation(kTargetLocation);
			}
		}
		break;
	case SOT_DIR:
		{
			if (pkSkill->GetProperty_FollowLockedDir())
			{
				LC_LogicObject* pkTarget = this->GetMap()->FindObject(GetLockedTargetID());
				if (pkTarget)
				{
					kTargetLocation = pkTarget->GetVisibleLocation();
				}
			}
			this->FaceToLocation(kTargetLocation);
			
			//朝向型技能在没有目标选中的时候，不会朝鼠标点移动，而是直接释放
			if (GetLockedTargetID() == 0)
			{
				lExecuteDistanceFunc = SEDF_CLAMP;
			}

			switch(lExecuteDistanceFunc)
			{
			case SEDF_NOCHECK:
			case SEDF_CHECK:
				{
					this->SetLockedLocation(kTargetLocation);
				}
				break;

			case SEDF_CLAMP:
			case SEDF_MAX:
				{
					int iDistance = 0;
					UT_Vec3Int kCurrentLocation = this->GetCurrentLocation();
					if (lExecuteDistanceFunc == SEDF_CLAMP)
					{
						iDistance = UT_MathBase::LineLengthXYInt(kCurrentLocation,kTargetLocation);
						iDistance = UT_MathBase::ClampInt(iDistance, iSkillExecuteMinDistance, iSkillExecuteMaxDistance);
					}
					else
					{
						iDistance = iSkillExecuteMaxDistance;
					}

					UT_Vec2Float	 kForwardDirXY	  = this->GetForwardDir();
					UT_Vec2Float	 kMaxMoveLocation = kForwardDirXY * Long2Float(iDistance);

					UT_Vec3Int kLockedLocation = kCurrentLocation;
					kLockedLocation.x += (int)kMaxMoveLocation.x;
					kLockedLocation.y += (int)kMaxMoveLocation.y;

					this->SetLockedLocation(kLockedLocation);
				}
				break;
			}
		}
		break;
	case SOT_LOCATION:
		{
			this->SetLockedLocation(kTargetLocation);
		}
		break;
	}
}
//------------------------------------------------------------------------
ResultType LC_ClientGamePlayer::CheckPendingSkill(int lSkillTypeID)
{
	//如果是召唤解散坐骑技能，需要判断坐骑是否存在
	//if (lSkillTypeID == SKILL_SUMMON_VEHICLE)
	//{
	//	/*LC_VehicleAsset* pkAsset = GetVehicleAsset();
	//	if (pkAsset->GetVehicleCount() <= 0)
	//	{
	//		SendLogicInfoMessage(RE_VEHICLE_CNNT_SUMMON_VEHICLE_WITHOUT_SEED);
	//		return RE_VEHICLE_CNNT_SUMMON_VEHICLE_WITHOUT_SEED;
	//	}*/
	//}
	//else	
	//{		
	//	if(GetRiderCharType() > 0)		
	//	{			
	//		SendLogicInfoMessage(RE_CANNT_ACTION_ON_VEHICLE);			
	//		return RE_CANNT_ACTION_ON_VEHICLE;		
	//	}	
	//}

	if (GetDead())
	{
		return RE_SKILL_SYSTEM_ERROR;
	}
	//技能请求未返回，不能使用新技能
	if (m_iReqNoRltExecSkillID && m_iReqNoRltExecSkillID != lSkillTypeID)
	{
		return RE_SKILL_PENDING_FAIL_INTERRUPT;
	}

	int	curState = this->GetCurrentStateType();
	int	nxtState = this->GetNextStateType();
	//跳状态不能施放技能
	if (IS_PLAYER_JUMP_STATE(curState) && nxtState != ST_PLAYER_STAND)
	{
		return RE_SKILL_SYSTEM_ERROR;
	}
	//被击退状态不能施放技能,被动位移状态
	if (curState == ST_PLAYER_PUNCH_BACK || curState == ST_PLAYER_PASSIVE_MOVE)
	{
		return RE_SKILL_SYSTEM_ERROR;
	}


	/*
	*1、如果释放空手普通攻击，则传入的为空手普通攻击技能；如果释放武器普通攻击，则传入的也是空手普通攻击技能
	*2、空手普通攻击和武器普通攻击的CD均为0，检查技能CD必然正确
	*/
	CF_SkillSpecialAttrib::DataEntry* pkSkillData = SafeGetCSVFileDataEntryPtr<CF_SkillSpecialAttrib>(lSkillTypeID);
	CF_SkillExecuteList::DataEntry* pkSkillInfo = LC_Helper::SafeGetSkillExecuteDataEntryPtr(lSkillTypeID);
	if (NULL == pkSkillData || NULL == pkSkillInfo)
	{
		//SendLogicInfoMessage(RE_SKILL_SYSTEM_ERROR);
		//GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "错误的技能ID[%i]", lSkillTypeID);
		return RE_SKILL_SYSTEM_ERROR;
	}

	SK_Factory* pkSkillFactory = SK_Factory::GetSingletonPtr();
	SK_SkillExecute* pkSkillExecute = pkSkillFactory->RequestSkillExcute(lSkillTypeID);
	if (NULL == pkSkillExecute)
	{
		return RE_SKILL_SYSTEM_ERROR;
	}


	//判断技能是否合法
	if (!CheckSkillValid(pkSkillData, 0))
	{
		return RE_SKILL_SYSTEM_ERROR;
	}


	//检查CD
	//if( pkSkillExecute->IsHasClientCoolDown() && GetSkillCoolDownMap()->GetSkillCurrentCDTime(lSkillTypeID) > 0.0f)
	//{
	//	//SendLogicInfoMessage(RE_SKILL_PENDING_FAIL_CD);
	//	return RE_SKILL_PENDING_FAIL_CD;
	//}

	if( (GetGlobalSetting.nSkillUsableCountMode == 0 && pkSkillExecute->IsHasClientCoolDown() && GetSkillCoolDownMap()->GetSkillCurrentCDTime(lSkillTypeID) > 0.0f) 
		|| (GetGlobalSetting.nSkillUsableCountMode == 1 && GetBuddySkillUsableCount(GetCharType(), lSkillTypeID) < 1))
		return RE_SKILL_PENDING_FAIL_CD;

	/*if (!m_kSkillAsset.IsLearnedSkill(lSkillTypeID))
	{
	return RE_SKILL_IS_NO_EXIST;
	}*/

	if(m_kActionForbid.IsActionForbid(LAFT_SKILL,LAF_SKILL_SKILL_ID,lSkillTypeID))
	{
		return RE_ACTION_FORBID;
	}


	//检查自身SkillFlag是否满足技能释放需求
	ResultType eCheckSkillFlag = pkSkillExecute->CheckActorSkillFlag(GetAllSkillFlag());
	if (eCheckSkillFlag != RE_SKILL_CHECK_SKILLFLAG_SUCCESS)
	{
		//SendLogicInfoMessage(eCheckSkillFlag);
		return eCheckSkillFlag;
	}


	//判断资源
	ResultType eCheckCostResult = LC_Helper::CheckSkillCost(this,pkSkillInfo);
	if(eCheckCostResult != RE_SKILL_ENOUGH_RESOURCE)
	{
		//SendLogicInfoMessage(eCheckCostResult);
		return eCheckCostResult;
	}

	CF_LogicAreaFunc::DataEntry* pkLogicAreaFunc = GetLogicAreaFunc();
	if (NULL != pkLogicAreaFunc)
	{
		//是否能在当前区域使用
		if (!pkSkillExecute->CanUseInCurrentLogicArea(pkLogicAreaFunc->_iID))
		{
			return RE_SKILL_PENDING_FAIL_ILLEGAL_LOGIC_AREA;
		}
	}


	//如果释放技能时按下alt键，暂时将目标切换成自身，待技能释放结束再回滚目标
	/*if (m_bKeyTabPressed)
	{
		m_bKeyTabPressed = false;
		_switchTarget(GetID());
	}*/

	return RE_SKILL_NONE_ERROR;
}
//------------------------------------------------------------------------
bool LC_ClientGamePlayer::CanSendLogicInfoMessageWithCDTime(long lMsgType)
{
	if(lMsgType == RE_CHEST_TOO_FAR_TO_PICK)
		return false;

	if (lMsgType == RE_SKILL_PENDING_FAIL_CD
		|| lMsgType == RE_SKILL_CHECK_CANT_PICK
		|| lMsgType == RE_CHEST_BOX_PROTECTED
		|| lMsgType == RE_CHEST_CANT_PICK_WHEN_WALLOW)
	{
		if ( GET_PROCESS_TIME - m_fSkillCdMessageTick > GetGlobalSetting.fSkillCoolDownTimeTick)
			m_fSkillCdMessageTick = GET_PROCESS_TIME;
		else
			return false;
	}

	return true;
}
int LC_ClientGamePlayer::CheckPendingSkillEx(int lSkillTypeID, bool bContinueNormalAttak)
{
	int lSkillFailReason = 0;
	ResultType iCheckResult = RE_SKILL_NONE_ERROR;
	if (GetDead())
	{
		SetSkillFailReason(lSkillFailReason, SFR_SYSTEM_ERROR, true);
		return lSkillFailReason;
	}

	//AI控制状态下不处理
	if (this->GetAIControl())
	{
		SetSkillFailReason(lSkillFailReason, SFR_SYSTEM_ERROR, true);
		return lSkillFailReason;
	}

	//技能请求未返回，不能使用新技能
	if (m_iReqNoRltExecSkillID && m_iReqNoRltExecSkillID != lSkillTypeID)
	{
		return RE_SKILL_PENDING_FAIL_INTERRUPT;
	}

	int	curState = this->GetCurrentStateType();
	int	nxtState = this->GetNextStateType();
	//跳状态不能施放技能
	if (IS_PLAYER_JUMP_STATE(curState) && nxtState != ST_PLAYER_STAND)
	{
		SetSkillFailReason(lSkillFailReason, SFR_SYSTEM_ERROR, true);
		return RE_SKILL_SYSTEM_ERROR;
	}
	//被击退状态不能施放技能,被动位移状态
	if (curState == ST_PLAYER_PUNCH_BACK || curState == ST_PLAYER_PASSIVE_MOVE)
	{
		SetSkillFailReason(lSkillFailReason, SFR_SYSTEM_ERROR, true);
		return RE_SKILL_SYSTEM_ERROR;
	}

	/*
	*1、如果释放空手普通攻击，则传入的为空手普通攻击技能；如果释放武器普通攻击，则传入的也是空手普通攻击技能
	*2、空手普通攻击和武器普通攻击的CD均为0，检查技能CD必然正确
	*/

	CF_SkillSpecialAttrib::DataEntry* pkSkillAttrib = SafeGetCSVFileDataEntryPtr<CF_SkillSpecialAttrib>(lSkillTypeID);
	CF_SkillExecuteList::DataEntry* pkSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(lSkillTypeID);
	if (NULL == pkSkillAttrib || NULL == pkSkillData)
	{
		SetSkillFailReason(lSkillFailReason, SFR_SYSTEM_ERROR, true);
		return lSkillFailReason;
	}

	if(pkSkillData->_iMinPackCount > 0 
		&& GetPackAsset()->GetPackEmptyEntryCount(PACK_TYPE_BASIC) < pkSkillData->_iMinPackCount)
	{
		return RE_ITEM_PACKAGE_FULL;
	}

	SK_Factory* pkSkillFactory = SK_Factory::GetSingletonPtr();
	SK_SkillExecute* pkSkillExecute = pkSkillFactory->RequestSkillExcute(lSkillTypeID);
	if (NULL == pkSkillExecute)
	{
		SetSkillFailReason(lSkillFailReason, SFR_SYSTEM_ERROR, true);
		return lSkillFailReason;
	}


	//判断技能是否合法
	
	if (!CheckSkillValid(pkSkillAttrib, 0))
	{
		SetSkillFailReason(lSkillFailReason, SFR_SYSTEM_ERROR, true);
		return lSkillFailReason;
	}


	//检查CD
	if( (GetGlobalSetting.nSkillUsableCountMode == 0 && pkSkillExecute->IsHasClientCoolDown() && GetSkillCoolDownMap()->GetSkillCurrentCDTime(lSkillTypeID) > 0.0f) 
		|| (GetGlobalSetting.nSkillUsableCountMode == 1 && GetBuddySkillUsableCount(GetCharType(), lSkillTypeID) < 1))
	//	return RE_SKILL_PENDING_FAIL_CD;
	//if( pkSkillExecute->IsHasClientCoolDown() && GetSkillCoolDownMap()->GetSkillCurrentCDTime(lSkillTypeID) > 0.0f)
	{
		SetSkillFailReason(lSkillFailReason, SFR_CD, true);
		iCheckResult = RE_SKILL_PENDING_FAIL_CD;
	}


	if(m_kActionForbid.IsActionForbid(LAFT_SKILL,LAF_SKILL_SKILL_ID,lSkillTypeID))
	{
		SetSkillFailReason(lSkillFailReason, SFR_ACTION_FORBID, true);
		if (iCheckResult == RE_SKILL_NONE_ERROR)
		{
			iCheckResult = RE_ACTION_FORBID;
		}
	}


	//检查自身SkillFlag是否满足技能释放需求
	ResultType iTempResult = pkSkillExecute->CheckActorSkillFlag(GetAllSkillFlag());
	if (iTempResult != RE_SKILL_CHECK_SKILLFLAG_SUCCESS)
	{
		SetSkillFailReason(lSkillFailReason, SFR_SKILL_FLAG, true);
		if (iCheckResult == RE_SKILL_NONE_ERROR)
		{
			iCheckResult = iTempResult;
		}
	}

	//判断资源
	/*iTempResult = LC_Helper::CheckSkillCost(this,pkSkillData);
	if(iTempResult != RE_SKILL_ENOUGH_RESOURCE)
	{
		SetSkillFailReason(lSkillFailReason, SFR_RESOURCE, true);
		if (iCheckResult == RE_SKILL_NONE_ERROR)
		{
			iCheckResult = iTempResult;
		}
	}*/

	CF_LogicAreaFunc::DataEntry* pkLogicAreaFunc = GetLogicAreaFunc();
	if (NULL != pkLogicAreaFunc)
	{

		//是否能在当前区域使用
		if (!pkSkillExecute->CanUseInCurrentLogicArea(pkLogicAreaFunc->_iID))
		{
			SetSkillFailReason(lSkillFailReason, SFR_LOGIC_AREA, true);
			if (iCheckResult == RE_SKILL_NONE_ERROR)
			{
				iCheckResult = RE_SKILL_PENDING_FAIL_ILLEGAL_LOGIC_AREA;
			}
		}
	}


	//如果释放技能时按下alt键，暂时将目标切换成自身，待技能释放结束再回滚目标
	/*if (m_bKeyTabPressed)
	{
		m_bKeyTabPressed = false;
		_switchTarget(GetID());
	}*/

	//挂起当前技能成功or不需要继续普通攻击
	if (0 == lSkillFailReason || !bContinueNormalAttak)
	{
		return lSkillFailReason;
	}


	/*
	*技能错误的提示信息
	*/
	switch (iCheckResult)
	{
	case RE_SKILL_NOT_ENOUGH_ITEM:
		{
			CF_ItemList* pkItemList = CF_ItemList::GetSingletonPtr();
			if (NULL == pkItemList)
			{
				SetSkillFailReason(lSkillFailReason, SFR_SYSTEM_ERROR, true);
				return lSkillFailReason;
			}

			for(int i = 0; i < MAX_SKILL_COST_ITEM_COUNT; i++)
			{
				UT_Vec3Int kItemCost = CSVHelper::GetSkillItemCost(pkSkillData, i);
				int iCostItemID = kItemCost.x;
				int iCostItemCount = kItemCost.y;
				if(IS_ITEM_ID(iCostItemID))
				{
					if (GetCanCostItemCount(iCostItemID) < iCostItemCount)
					{
						CF_ItemList::DataEntry* pkItemData = pkItemList->GetEntryPtr(iCostItemID);
						if (NULL == pkItemData)
						{
							SetSkillFailReason(lSkillFailReason, SFR_SYSTEM_ERROR, true);
							return lSkillFailReason;
						}
						StringType kTip = GET_MSG(LC_CLIENTGAMEPLAYER_CPP_003);
						kTip = kTip + pkItemData->_kName;
						kTip = kTip + GET_MSG(LC_CLIENTGAMEPLAYER_CPP_004);
						SendLogicInfoMessage(RE_TIP_MESSAGE, kTip.c_str());
					}
				}
			}
		}
		break;
	default:
		{
			if(CanSendLogicInfoMessageWithCDTime(iCheckResult))
				SendLogicInfoMessage(iCheckResult);
		}
		break;
	}


	if (NULL == m_pkMap)
	{
		SetSkillFailReason(lSkillFailReason, SFR_SYSTEM_ERROR, true);
		return lSkillFailReason;
	}
	//目标型并且是伤害技能，才继续普通攻击
	if (pkSkillData->_lOperationType == SOT_TARGET && pkSkillData->_lDamageType != 0)
	{
		if (IS_PLAYER_CLASS_ID(GetLockedTargetID()) || IS_NPC_ID(GetLockedTargetID()))
		{
			//if (NULL != m_pkMap)
			{
				LC_LogicObject* pkTarget = m_pkMap->FindObject(GetLockedTargetID());
				if (NULL != pkTarget)
				{
					int iDistance = UT_MathBase::LineLengthXYInt(GetCurrentLocation(), pkTarget->GetVisibleLocation());
					//1、近程or通用技能，如果距离12米，继续普通攻击
					//2、远程技能，如果距离2米，继续普通攻击
					if (((pkSkillData->_lRangeType == SRT_MELEE || pkSkillData->_lRangeType == SRT_COMMON) && iDistance <= 1200)
						|| (pkSkillData->_lRangeType == SRT_RANGE && iDistance <= 200))
					{
						int lEmptyHandNormalAttack = LC_Helper::GetNormalAttackSkill(this);
						if (RE_SKILL_NONE_ERROR == CheckPendingSkill(lEmptyHandNormalAttack))
						{
							WaitPendingSkill(lEmptyHandNormalAttack, 0);
						}
					}
				}
			}
		}
	}

	//注意：这里返回上一个技能释放失败的原因
	return lSkillFailReason;
}
//------------------------------------------------------------------------
ResultType LC_ClientGamePlayer::CheckExecuteSkill(float fCurrentTime,Skill::SK_SkillExecute* pkSkill,int lItemID)
{
	//if (NULL == pkSkill)
	//{
	//	return RE_SKILL_SYSTEM_ERROR;
	//}

	int lSkillType = pkSkill->GetExecuteID();
	CF_SkillSpecialAttrib::DataEntry* pkSkillAttrib = SafeGetCSVFileDataEntryPtr<CF_SkillSpecialAttrib>(lSkillType);
	CF_SkillExecuteList::DataEntry* pkSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(lSkillType);
	if (NULL == pkSkillData || NULL == pkSkillAttrib)
		return RE_SKILL_SYSTEM_ERROR;

	//如果技能已发送
	if (m_iReqNoRltExecSkillID == lSkillType)
		return RE_SKILL_SYSTEM_ERROR;

	//如果马上就要放这个技能
	if (m_iNextState == lSkillType)
		return RE_SKILL_SYSTEM_ERROR;

	//判断技能是否合法
	if (!CheckSkillValid(pkSkillAttrib, 0))
	{
		return RE_SKILL_SYSTEM_ERROR;
	}

	if(m_kActionForbid.IsActionForbid(LAFT_SKILL,LAF_SKILL_SKILL_ID,lSkillType))
	{
		//在CheckExecuteSkill内部不给提示，在调用CheckExecuteSkill处进行提示
		//SendLogicInfoMessage(RE_ACTION_FORBID);
		return RE_ACTION_FORBID;
	}

	//检查自身SkillFlag是否满足技能释放需求
	ResultType eCheckSkillFlag = pkSkill->CheckActorSkillFlag(GetAllSkillFlag());
	if (eCheckSkillFlag != RE_SKILL_CHECK_SKILLFLAG_SUCCESS)
	{
		//在CheckExecuteSkill内部不给提示，在调用CheckExecuteSkill处进行提示
		//SendLogicInfoMessage(eCheckSkillFlag);
		return eCheckSkillFlag;
	}

	//检查CD
	if( (GetGlobalSetting.nSkillUsableCountMode == 0 && pkSkill->IsHasClientCoolDown() && GetSkillCoolDownMap()->GetSkillCurrentCDTime(lSkillType) > 0.0f) 
		|| (GetGlobalSetting.nSkillUsableCountMode == 1 && GetBuddySkillUsableCount(GetCharType(), lSkillType) < 1))
		return RE_SKILL_PENDING_FAIL_CD;

	if(IS_ITEM_ID(lItemID))
	{
		if (m_kItemCoolDownMap.GetItemCurrentCDTime(lItemID) > 0.0f)
		{
			//GfxWriteLog(LOG_CHECK_FILELINE, LOG_SWITCH_CHECK,"试图使用背包物件还未冷却");
			return RE_ITEM_CANT_USE_CD;
		}
	}

	//判断资源
	ResultType eCheckCostResult = LC_Helper::CheckSkillCost(this,pkSkillData);
	if(eCheckCostResult != RE_SKILL_ENOUGH_RESOURCE)
		return eCheckCostResult;

	CF_LogicAreaFunc::DataEntry* pkLogicAreaFunc = GetLogicAreaFunc();
	if (NULL != pkLogicAreaFunc)
	{
		//是否能在当前区域使用
		if (!pkSkill->CanUseInCurrentLogicArea(pkLogicAreaFunc->_iID))
		{
			return RE_SKILL_PENDING_FAIL_ILLEGAL_LOGIC_AREA;
		}
	}


	//判断触发条件
	{
		int iOperationType			 = pkSkill->GetProperty_OperationType();
		int iLockTargetType			 = pkSkill->GetProperty_LockTargetType();
		int iSkillExecuteMinDistance = pkSkill->GetProperty_MinExecuteDistance();
		int iSkillExecuteMaxDistance = pkSkill->GetProperty_MaxExecuteDistance();
		//	bool bExecuteOnCorpus		 = pkSkill->GetProperty_ExecuteOnCorpus();
		//	bool bNeedTestPath			 = pkSkill->GetProperty_NeedTestPath();
		//	bool bExecuteImmediately	 = pkSkill->GetProperty_ExecuteImmediately();
		int	 lExecuteDistanceFunc	 = pkSkill->GetProperty_ExecuteDistanceFunc();

		UT_Vec3Int kCurrentLocation = GetCurrentLocation();
		UT_Vec3Int kTargetLocation = kCurrentLocation;
		ResultType iResult = 0;

		switch (iOperationType)
		{
		case SOT_TARGET: //目标型
			{
				if(!GetNeedLockTarget())
				{
					CF_ActorFilterFunc* pkCSVActorFilterFunc = CF_ActorFilterFunc::GetSingletonPtr();
					CF_ActorFilterFunc::DataEntry* pkData = pkCSVActorFilterFunc->GetEntryPtr(iLockTargetType);
					if (NULL == pkData)
						return RE_SKILL_SYSTEM_ERROR;

					object_id_type lTargetID = this->GetLockedTargetID();
					LC_LogicObject* pkTarget = this->GetMap()->FindObject(lTargetID);

					if(NULL == pkTarget)
					{
						iResult = RE_SKILL_LOST_TARGET;
					}
					else
					{
						iResult = LC_Helper::IsValidTarget(this,lTargetID,pkData);

						if (iResult == RE_SKILL_CHECK_VALID_TARGET || 
							((iResult == RE_SKILL_PENDING_FAIL_IN_SAFE_AREA  || iResult == RE_SKILL_PENDING_FAIL_ACK_SAFE_AREA) && pkSkillData->_bCanUseInSafeArea) )
						{
							iResult = RE_SKILL_NONE_ERROR;
							UT_Vec3Int kCurrentLocation = this->GetCurrentLocation();
							UT_Vec3Int kTargetLocation  = pkTarget->GetVisibleLocation();
							int iSafeRadius	 = this->GetProperty_ShapeRadius();
							if(IS_PLAYER_CLASS_ID(lTargetID) || IS_NPC_ID(lTargetID) || IS_LOCAL_NPC_ID(lTargetID))
								iSafeRadius	+= ((LC_ActorBase*)pkTarget)->GetProperty_ShapeRadius();
							if (pkTarget->GetType() == LOT_PLACEHOLDER)
								iSafeRadius = 0;
							kTargetLocation = UT_MathBase::ComputeSafeRadiusTargetLocation(kCurrentLocation,kTargetLocation,iSafeRadius);

							int iDistance = UT_MathBase::LineLengthXYInt(kCurrentLocation,kTargetLocation);
							if(iDistance < iSkillExecuteMinDistance)
								iResult = RE_SKILL_DISTANCE_NEAR;
							if(iDistance > iSkillExecuteMaxDistance)
								iResult = RE_SKILL_DISTANCE_FAR;

							if(iResult == RE_SKILL_NONE_ERROR && pkSkill->GetProperty_NeedTestPath())
							{
								int iCurrentDistance = UT_MathBase::LineLengthXYInt(kCurrentLocation,kTargetLocation);
								int iPathLength = PF_PathManager::GetSingleton().GetPathLength(kCurrentLocation,kTargetLocation,true );

								if(iPathLength > iCurrentDistance + 200)
								{
									//SendLogicInfoMessage(RE_TIP_MESSAGE,"该地点无法到达，不能施展技能");
									iResult = RE_SKILL_CANT_REACH_TARGET;
								}
							}
						}
					}
				}
				else
				{
					iResult = RE_SKILL_NEED_LOCK_TARGET;
				}
			}
			break;
		case SOT_DIR: //朝向型
			{
				if ( lExecuteDistanceFunc != SEDF_CHECK || pkSkillData->_iSkillProcessCategory == SPC_INDUCE && !GetAIControl() )	//不检查距离则不做距离检测
				{
					iResult = RE_SKILL_NONE_ERROR;
				}
				else
				{
					object_id_type lTargetID = this->GetLockedTargetID();
					LC_LogicObject* pkTarget = this->GetMap()->FindObject(lTargetID);
					//pkTarget will be NULL when lTargetID == 0

					iResult = RE_SKILL_NONE_ERROR;

					UT_Vec3Int kCurrentLocation = this->GetCurrentLocation();

					//现在主角的的朝向型技能，开始释放时跟随鼠标方向的，不会再计算锁定的目标
					//UT_Vec3Int kTargetLocation  = (NULL != pkTarget) ? pkTarget->GetVisibleLocation() : this->GetLockedLocation();
					//if(NULL != pkTarget)	this->SetLockedLocation(kTargetLocation);	//跟踪目标，将用来设置TargetLocation
					UT_Vec3Int kTargetLocation = this->GetLockedLocation();

					int iSafeRadius	 = this->GetProperty_ShapeRadius();
					if((IS_PLAYER_CLASS_ID(lTargetID) || IS_NPC_ID(lTargetID)) && NULL != pkTarget)
						iSafeRadius	+= ((LC_ActorBase*)pkTarget)->GetProperty_ShapeRadius();
					kTargetLocation = UT_MathBase::ComputeSafeRadiusTargetLocation(kCurrentLocation,kTargetLocation,iSafeRadius);

					int iDistance = UT_MathBase::LineLengthXYInt(kCurrentLocation,kTargetLocation);
					if(iDistance > iSkillExecuteMaxDistance)
						iResult = RE_SKILL_DISTANCE_FAR;
					if(iDistance < iSkillExecuteMinDistance)
						iResult = RE_SKILL_DISTANCE_NEAR;
				}
			}
			break;
		case SOT_LOCATION: //地点型
			{
				if(!GetNeedLockLocation())
				{
					//以地面位置设为目标点
					iResult = RE_SKILL_NONE_ERROR;

					//以地面位置设为目标点
					kTargetLocation = GetLockedLocation();

					int iCurrentDistance = UT_MathBase::LineLengthXYInt(kCurrentLocation,kTargetLocation);
					if(iCurrentDistance > iSkillExecuteMaxDistance && lExecuteDistanceFunc == SEDF_CHECK)
						iResult = RE_SKILL_DISTANCE_FAR;
					if(iCurrentDistance < iSkillExecuteMinDistance)
						iResult = RE_SKILL_DISTANCE_NEAR; 

					//如果技能需要测试是否能直线到达目标点，作一次测试
					if(iResult == RE_SKILL_NONE_ERROR && pkSkill->GetProperty_NeedTestPath())
					{
						bool bCanMoveTo = PF_PathManager::GetSingleton().TestCanMoveTo(kTargetLocation,false);
						if(!bCanMoveTo)
							iResult = RE_SKILL_CANT_REACH_TARGET;
						else
						{
							int iPathLength = PF_PathManager::GetSingleton().GetPathLength(kCurrentLocation,kTargetLocation,false);

							if(iPathLength > iCurrentDistance + 200)
							{
								//SendLogicInfoMessage(RE_TIP_MESSAGE,"该地点无法到达，不能施展技能");
								iResult = RE_SKILL_CANT_REACH_TARGET;
							}
							else if ( iPathLength < 50 )
							{
								GfxWriteLog( LOG_TAG_ERROR , LOG_ERROR_FILELINE, "Path Length is %d" , iPathLength );
							}
						}
					}	
				}
				else
					iResult = RE_SKILL_NEED_LOCK_LOCATION;
			}
			break;
		default:
			break;
		}

		if(iResult != RE_SKILL_NONE_ERROR)
			return iResult;
	}

	return LC_ActorBase::CheckExecuteSkill(fCurrentTime,pkSkill,lItemID);	//will return RE_SKILL_CAN_TAKE_PLACE
}
//------------------------------------------------------------------------
ResultType LC_ClientGamePlayer::PrepareExecuteSkill(Skill::SK_SkillExecute* pkSkill,int lItemID)
{
	//if (NULL == pkSkill)
	//{
	//	return RE_SKILL_SYSTEM_ERROR;
	//}

	int iOperationType				= pkSkill->GetProperty_OperationType();
	//	int iLockTargetType				= pkSkill->GetProperty_LockTargetType();
	int iSkillExecuteMinDistance	= pkSkill->GetProperty_MinExecuteDistance();
	int iSkillExecuteMaxDistance	= pkSkill->GetProperty_MaxExecuteDistance();
	//	bool bExecuteOnCorpus			= pkSkill->GetProperty_ExecuteOnCorpus();
	//	bool bNeedTestPath				= pkSkill->GetProperty_NeedTestPath();

	UT_Vec3Int kCurrentLocation		= this->GetCurrentLocation();

	switch (iOperationType)
	{
	case SOT_TARGET: //目标型
		{
			object_id_type lTargetID = this->GetLockedTargetID();
			LC_MapBase* pkMap = this->GetMap();
			if(!pkMap)
				return RE_SKILL_LOST_TARGET;

			LC_LogicObject* pkTarget = pkMap->FindObject(lTargetID);
			if(!pkTarget)
			{
				return RE_SKILL_LOST_TARGET;
			}
			else
			{
				if ( IS_CHEST_ID(lTargetID) && (pkSkill->GetExecuteID() == SKILL_PICK_CHEST_LEFT || pkSkill->GetExecuteID() == SKILL_PICK_CHEST_RIGHT)
					&& UT_MathBase::LineLengthXYInt(pkTarget->GetCurrentLocation() ,GetCurrentLocation()) < 560 )
				{
					bool bCol = PF_PathManager::GetSingleton().TestPointCollision( pkTarget->GetCurrentLocation() , false );
					if (bCol)
					{
						GetFlopManager()->RequestPickItem(lTargetID);
						ClearPendingSkill();
						return RE_SKILL_NONE_ERROR;
					}
				}

				UT_Vec3Int kTargetLocation  = pkTarget->GetVisibleLocation();
				if (pkSkill->GetExecuteID() != SKILL_TALK_TO_NPC)
				{
					int iSafeRadius	 = GetProperty_ShapeRadius();
					if(IS_PLAYER_CLASS_ID(lTargetID) || IS_NPC_ID(lTargetID))
						iSafeRadius	+= ((LC_ActorBase*)pkTarget)->GetProperty_ShapeRadius();
					if (pkTarget->GetType() == LOT_PLACEHOLDER)
						iSafeRadius = 0;
					kTargetLocation = UT_MathBase::ComputeSafeRadiusTargetLocation(kCurrentLocation,kTargetLocation,iSafeRadius);
				}
				SetTargetLocation(kTargetLocation);
			}
		}
		break;
	case SOT_DIR: //朝向型
		{
			UT_Vec3Int kTargetLocation = this->GetLockedLocation();
			kTargetLocation = UT_MathBase::ComputeSafeRadiusTargetLocation(GetCurrentLocation(),kTargetLocation,GetProperty_ShapeRadius());
			this->SetTargetLocation(kTargetLocation);
		}
		break;
	case SOT_LOCATION: //地点型
		{
			UT_Vec3Int kTargetLocation = this->GetLockedLocation();
			this->SetTargetLocation(kTargetLocation);
		}
		break;
	}

	return RE_SKILL_NONE_ERROR;
}
//------------------------------------------------------------------------
ResultType LC_ClientGamePlayer::ExecuteSkill(float fCurrentTime,Skill::SK_SkillExecute* pkSkill,int lItemID)
{
	//if (NULL == pkSkill)
	//{
	//	return RE_SKILL_SYSTEM_ERROR;
	//}

	////是否要打断技能
	//if(IS_SKILL_TYPE(m_iCurrentState))
	//{
	//	RequestInterrputSkill(GET_PROCESS_TIME,m_iCurrentState,SKI_CHANGE_SKILL);

	//	//无需服务器回包，直接中断技能
	//	InterruptSkill(GET_PROCESS_TIME,m_iCurrentState,SKI_CHANGE_SKILL);
	//}
	int lSkillType		= pkSkill->GetExecuteID();
	CF_SkillExecuteList::DataEntry* pkSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(lSkillType);
	if(!pkSkillData)
		return RE_SKILL_SYSTEM_ERROR;

	this->_doUnlockRequestExecuteSkill(true, lSkillType);

	//互斥骑乘状态
	if(GetRiderCharType() > 0)
	{
		if(pkSkillData->_bMutexWithRider || (pkSkillData->_bMutexWithVehicleLevel && !IsCimeliaEnabled(LC_SUB_TP_VEHICLE_FIGHTING))  )
			SetRiderCharType(0);
	}

	ResultType iExecuteResult = LC_PlayerBase::ExecuteSkill(fCurrentTime,pkSkill,lItemID);

	switch (iExecuteResult)
	{
	case RE_SKILL_EXECUTE_SUCCESS:
		{
			//重丢骰子 
			RecomputeRandNums();

			bool bStopMove = false;
			//1、非引导型技能在启动技能时扣除资源、刷新cd
			//2、引导型的在脚本中扣除资源、刷新cd
			if (SPC_INDUCE != pkSkillData->_iSkillProcessCategory)
			{
				if(pkSkill->GetProperty_CoolDownTime() > 0 || pkSkill->GetProperty_UseCoolDown())
				{
					bool bIgnoreSkillGlobalCD = GetSkillFlag(SK_IGNORE_SKILL_GLOBAL_CD);
					if (!bIgnoreSkillGlobalCD)
					{
						//添加公共CD
						GetSkillCoolDownMap()->AddGlobalCoolDownLevel(pkSkill->GetExecuteID(), fCurrentTime);
					}
					//添加CD
					//GetSkillCoolDownMap()->RefreshSkillCD(pkSkill->GetExecuteID(), bIgnoreSkillGlobalCD);
				}	
			}
			if (pkSkill->GetProperty_CoolDownClientTime() > 0)
			{
				CF_SkillExecuteList::DataEntry* pkSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(pkSkill->GetExecuteID());
				GetSkillCoolDownMap()->SetSkillClientTypeCD(pkSkillData->_lCoolDownTypeClient, pkSkill->GetProperty_CoolDownClientTime());
			}
			//设置技能状态
			((LC_ClientShortcutBar*)GetShortcutBar())->SetShortcutTriggerState(lSkillType,  false);

			//取消自动寻路
			if (!pkSkill->GetProperty_EnableMoveStride())
			{
				// 老的逻辑，武林
				SetMoveStride(false);
				SetLastPickTerrainTime(-1.0f);

				if (!pkSkill->GetProperty_UsePathFinding())
					bStopMove = true;
			}
			else
			{
				//清空寻路
				if(!GetMoveStride() && !GetWASDStride() && !pkSkill->GetProperty_UsePathFinding())
					bStopMove = true;
			}

			if(bStopMove)
			{
				StopMove();
				//StopGuide();
				Reset2Guding(true);
			}

			//SendLogicInfoMessage(RE_TIP_MESSAGE,pkSkill->GetProperty_SkillName().c_str());
		}
		break;
	default:
		SendLogicInfoMessage(iExecuteResult);
	}

	return iExecuteResult;
}
//--------------------------------------------------------
ResultType LC_ClientGamePlayer::ExecuteSkillImmediately(float fCurrentTime,Skill::SK_SkillExecute* pkSkill,int lItemID)
{
	//if (NULL == pkSkill)
	//{
	//	return RE_SKILL_SYSTEM_ERROR;
	//}

	int lSkillType		= pkSkill->GetExecuteID();
	CF_SkillExecuteList::DataEntry* pkSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(lSkillType);
	if(!pkSkillData)
		return RE_SKILL_SYSTEM_ERROR;

	this->_doUnlockRequestExecuteSkill(true, lSkillType);

	//互斥骑乘状态
	if(GetRiderCharType() > 0)
	{
		if(pkSkillData->_bMutexWithRider || (pkSkillData->_bMutexWithVehicleLevel && !IsCimeliaEnabled(LC_SUB_TP_VEHICLE_FIGHTING))  )
			SetRiderCharType(0);
	}


	//1、非引导型技能在启动技能时扣除资源、刷新cd
	//2、引导型的在脚本中扣除资源、刷新cd
	if (SPC_INDUCE != pkSkillData->_iSkillProcessCategory)
	{
		if(pkSkill->GetProperty_CoolDownTime() > 0 || pkSkill->GetProperty_UseCoolDown())
		{
			bool bIgnoreSkillGlobalCD = GetSkillFlag(SK_IGNORE_SKILL_GLOBAL_CD);
			if (!bIgnoreSkillGlobalCD)
			{
				//添加公共CD
				GetSkillCoolDownMap()->AddGlobalCoolDownLevel(pkSkill->GetExecuteID(), fCurrentTime);
			}
			//添加CD
			//GetSkillCoolDownMap()->RefreshSkillCD(pkSkill->GetExecuteID(), bIgnoreSkillGlobalCD);
		}	
	}
	//设置技能状态
	((LC_ClientShortcutBar*)GetShortcutBar())->SetShortcutTriggerState(pkSkill->GetExecuteID(), false);

	return RE_SKILL_EXECUTE_SUCCESS;
}


ResultType LC_ClientGamePlayer::ExecuteLCSkillTargetImmediately( float fCurrentTime,Skill::SK_SkillExecute* pkSkill,object_id_type lLocked )
{
	LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
	LC_ClientNPC* pkLC = (LC_ClientNPC*)pkNPCManager->FindNPC(m_lastLCID);
	if ( pkLC )
	{
		pkLC->SetLockedTargetID(lLocked);
		pkLC->ExecuteSkill(fCurrentTime, pkSkill,0);
		return RE_SKILL_NONE_ERROR;
	}
	return RE_SKILL_SYSTEM_ERROR;
}

ResultType LC_ClientGamePlayer::ExecuteLCSkillImmediately(float fCurrentTime,Skill::SK_SkillExecute* pkSkill
														  ,object_id_type lLocked, const UT_Vec3Int& lockLocation, float dirAngle)
{
	LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
	LC_ClientNPC* pkLC = (LC_ClientNPC*)pkNPCManager->FindNPC(m_lastLCID);
	if ( pkLC )
	{
		pkLC->SetLockedTargetID(lLocked);
		pkLC->SetLockedLocation(lockLocation);
		pkLC->SetForwardDirAngle(dirAngle);
		pkLC->ExecuteSkill(fCurrentTime, pkSkill,0);
		return RE_SKILL_NONE_ERROR;
	}
	return RE_SKILL_SYSTEM_ERROR;
}
//--------------------------------------------------------
void LC_ClientGamePlayer::RequestExecuteSkill(float fCurrentTime,Skill::SK_SkillExecute* pkSkill,int lItemID)
{
	//if (NULL == pkSkill)
	//{
	//	return;
	//}

	//设置物品CD
	if(IS_ITEM_ID(lItemID))
		GetControllerItemCoolDownMap()->RefreshItemCD(lItemID);

	SetBExecuteSkill(true);

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;
	// 改成warning
	if (m_iReqNoRltExecSkillID != 0)
		GfxWriteLog(LOG_TAG_WARNING, LOG_SWITCH_WARNING, "RequestExecuteSkill before skill response, m_iReqNoRltExecSkillID = %d, SkillID = %d", m_iReqNoRltExecSkillID, pkSkill->GetExecuteID());

	//发送网络请求
	switch(pkSkill->GetProperty_OperationType())
	{
	case SOT_TARGET:
		{
			uint32_t lLocalTargetID = GetLockedTargetID(); 
			if (IS_LOCAL_NPC_ID(lLocalTargetID) && pkSkill->GetExecuteID() == SKILL_TALK_TO_NPC)
			{
				TalkToNPC(lLocalTargetID);
			}
			else
			{
				MG_Req_PlayerExecuteSkillTypeTarget msgReqSkill;
				msgReqSkill.m_lRequesterID		= GetID();
				msgReqSkill.m_lLockTargetID		= lLocalTargetID;
				msgReqSkill.m_lSkillExecuteID	= pkSkill->GetExecuteID();
				msgReqSkill.m_lItemTypeID		= lItemID;
				msgReqSkill.m_lCurrentLocationX = m_kCurrentLocation.x;
				msgReqSkill.m_lCurrentLocationY = m_kCurrentLocation.y;
				msgReqSkill.m_lCurrentLocationZ = m_kCurrentLocation.z;
				msgReqSkill.m_fClientTime = GET_PROCESS_TIME;
				UT_Vec3Int kMouseLocation = UT_EngineMath::ConvertNiPoint3ToVec3Int(GetMouseOverLocation());
				msgReqSkill.m_lMouseLocationX = kMouseLocation.x;
				msgReqSkill.m_lMouseLocationY = kMouseLocation.y;
				bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_PLAYER_EXECUTE_SKILL_TYPE_TARGET, &msgReqSkill);

				if ( pkSkill->GetProperty_ExecutePriority() )
				{
					ClearPendingSkill();
					PrepareExecuteSkill(pkSkill,0);
					ExecuteSkill(fCurrentTime,pkSkill,0);
					m_iReqNoRltExecSkillID = 0;
					m_skillRltDelayTimer.Start(SKILL_RLT_DELAY);
				}
			}
		}
		break;
	case SOT_DIR:
		{
			UT_Vec2Float kDirXY = GetForwardDir();

			MG_Req_PlayerExecuteSkillTypeDir msgReqSkill;
			msgReqSkill.m_lRequesterID		= GetID();
			msgReqSkill.m_lLockedTargetID	= GetLockedTargetID();
			msgReqSkill.m_lSkillExecuteID	= pkSkill->GetExecuteID();
			msgReqSkill.m_lItemTypeID		= lItemID;
			msgReqSkill.m_iForwardAngle		= GetForwardDirAngle();
			//msgReqSkill.m_lCurrentLocationX	= m_kCurrentLocation.x;
			//msgReqSkill.m_lCurrentLocationY = m_kCurrentLocation.y;
			msgReqSkill.m_lLockedLocationX	= GetLockedLocation().x;
			msgReqSkill.m_lLockedLocationY  = GetLockedLocation().y;
			msgReqSkill.m_lLockedLocationZ  = GetLockedLocation().z;
			msgReqSkill.m_fClientTime		= GET_PROCESS_TIME;
			UT_Vec3Int kMouseLocation = UT_EngineMath::ConvertNiPoint3ToVec3Int(GetMouseOverLocation());
			msgReqSkill.m_lMouseLocationX = kMouseLocation.x;
			msgReqSkill.m_lMouseLocationY = kMouseLocation.y;
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_PLAYER_EXECUTE_SKILL_TYPE_DIR, &msgReqSkill);

			// FixBug: 把需要连续执行的引导类技能，(双枪普攻/旋风斩)，配置成“等待服务器执行”，疯狂按键的情况下就不会有无动作射击的bug了
			if (pkSkill->GetProperty_ExecutePriority())
			{
				ClearPendingSkill();
				PrepareExecuteSkill(pkSkill,0);
				ExecuteSkill(fCurrentTime,pkSkill,0);
				m_iReqNoRltExecSkillID = 0;

				m_skillRltDelayTimer.Start(SKILL_RLT_DELAY); 
			}
		}
		break;
	case SOT_LOCATION:
		{
			MG_Req_PlayerExecuteSkillTypeLocation msgReqSkill;
			msgReqSkill.m_lRequesterID		= GetID();
			msgReqSkill.m_lSkillExecuteID	= pkSkill->GetExecuteID();
			msgReqSkill.m_lItemTypeID		= lItemID;
			msgReqSkill.m_lCurrentLocationX = GetCurrentLocation().x;
			msgReqSkill.m_lCurrentLocationY	= GetCurrentLocation().y;
			msgReqSkill.m_lCurrentLocationZ	= GetCurrentLocation().z;
			msgReqSkill.m_lLockedLocationX	= GetLockedLocation().x;
			msgReqSkill.m_lLockedLocationY  = GetLockedLocation().y;
			msgReqSkill.m_lLockedLocationZ  = GetLockedLocation().z;
			msgReqSkill.m_fClientTime = GET_PROCESS_TIME;
			UT_Vec3Int kMouseLocation = UT_EngineMath::ConvertNiPoint3ToVec3Int(GetMouseOverLocation());
			msgReqSkill.m_lMouseLocationX = kMouseLocation.x;
			msgReqSkill.m_lMouseLocationY = kMouseLocation.y;
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_PLAYER_EXECUTE_SKILL_TYPE_LOCATION, &msgReqSkill);

			if (pkSkill->GetProperty_ExecutePriority())
			{
				ClearPendingSkill();
				PrepareExecuteSkill(pkSkill,0);
				ExecuteSkill(fCurrentTime,pkSkill,0);
				m_iReqNoRltExecSkillID = 0;

				m_skillRltDelayTimer.Start(SKILL_RLT_DELAY);
			}
		}
	default:
		break;
	}

	if(bCreateMsg)
	{
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
		//mark
		if (m_iReqNoRltExecSkillID == 0 && !pkSkill->GetProperty_IsLCSkill())
		{
			m_iReqNoRltExecSkillID = pkSkill->GetExecuteID();
			m_fReqExecSkillTime = fCurrentTime;
		}

		GfxWriteLog(LOG_SKILL_FILELINE, LOG_SWITCH_INFO, 
			"[Skill]Request, time=%.3f, player=%d, skill_id=%d, FrameID=%d",
			fCurrentTime, GetID(), pkSkill->GetExecuteID(), 
			UT_TimeManager::GetSingleton().CurrentFrameNo()
			);
	}

	//清空寻路
	//if(!pkSkill->GetProperty_UsePathFinding())
	//{
	//	this->StopMove();
	//}

	return;
}

void LC_ClientGamePlayer::RequestExecuteToyPetSkill(float fCurrentTime,Skill::SK_SkillExecute* pkSkill)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	//发送网络请求
	switch(pkSkill->GetProperty_OperationType())
	{
	case SOT_TARGET:
		{
			uint32_t lLocalTargetID = GetLockedTargetID(); 
			if (IS_LOCAL_NPC_ID(lLocalTargetID) && pkSkill->GetExecuteID() == SKILL_TALK_TO_NPC)
			{
				TalkToNPC(lLocalTargetID);
			}
			else
			{
				MG_Req_PlayerExecuteSkillTypeTarget msgReqSkill;
				msgReqSkill.m_lRequesterID		= GetID();
				msgReqSkill.m_lLockTargetID		= lLocalTargetID;
				msgReqSkill.m_lSkillExecuteID	= pkSkill->GetExecuteID();
				msgReqSkill.m_lItemTypeID		= 0;
				msgReqSkill.m_lCurrentLocationX = m_kCurrentLocation.x;
				msgReqSkill.m_lCurrentLocationY = m_kCurrentLocation.y;
				msgReqSkill.m_lCurrentLocationZ = m_kCurrentLocation.z;
				msgReqSkill.m_fClientTime = GET_PROCESS_TIME;
				UT_Vec3Int kMouseLocation = UT_EngineMath::ConvertNiPoint3ToVec3Int(GetMouseOverLocation());
				msgReqSkill.m_lMouseLocationX = kMouseLocation.x;
				msgReqSkill.m_lMouseLocationY = kMouseLocation.y;
				bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_PLAYER_EXECUTE_SKILL_TYPE_TARGET, &msgReqSkill);

				if ( pkSkill->GetProperty_ExecutePriority() )
				{
					ClearPendingSkill();
					PrepareExecuteSkill(pkSkill,0);
					ExecuteSkill(fCurrentTime,pkSkill,0);
					m_skillRltDelayTimer.Start(SKILL_RLT_DELAY);
				}
			}
		}
		break;
	case SOT_DIR:
		{
			UT_Vec2Float kDirXY = GetForwardDir();
			UT_Vec3Int kLocation = GetLockedLocation();
			object_id_type iLockTargeID = GetLockedTargetID();
			UT_Vec3Int kMouseLocation = UT_EngineMath::ConvertNiPoint3ToVec3Int(GetMouseOverLocation());
			if(iLockTargeID == 0 || !((SM_ClientGamePlayerController*)m_pkSMController)->IsTargetAttackable(this, iLockTargeID) )
			{
				kLocation = kMouseLocation;
				iLockTargeID = 0;
			}

			MG_Req_PlayerExecuteSkillTypeDir msgReqSkill;
			msgReqSkill.m_lRequesterID		= GetID();
			msgReqSkill.m_lLockedTargetID	= iLockTargeID;
			msgReqSkill.m_lSkillExecuteID	= pkSkill->GetExecuteID();
			msgReqSkill.m_lItemTypeID		= 0;
			msgReqSkill.m_iForwardAngle		= GetForwardDirAngle();
			//msgReqSkill.m_lCurrentLocationX	= m_kCurrentLocation.x;
			//msgReqSkill.m_lCurrentLocationY = m_kCurrentLocation.y;
			msgReqSkill.m_lLockedLocationX	= kLocation.x;
			msgReqSkill.m_lLockedLocationY  = kLocation.y;
			msgReqSkill.m_lLockedLocationZ  = kLocation.z;
			msgReqSkill.m_fClientTime		= GET_PROCESS_TIME;
			msgReqSkill.m_lMouseLocationX = kMouseLocation.x;
			msgReqSkill.m_lMouseLocationY = kMouseLocation.y;
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_PLAYER_EXECUTE_SKILL_TYPE_DIR, &msgReqSkill);

			if ( pkSkill->GetProperty_ExecutePriority() )
			{
				ClearPendingSkill();
				PrepareExecuteSkill(pkSkill,0);
				ExecuteSkill(fCurrentTime,pkSkill,0);
				m_skillRltDelayTimer.Start(SKILL_RLT_DELAY);
			}
		}
		break;
	case SOT_LOCATION:
		{
			UT_Vec3Int kLocation = GetLockedLocation();
			object_id_type iLockTargeID = GetLockedTargetID();
			UT_Vec3Int kMouseLocation = UT_EngineMath::ConvertNiPoint3ToVec3Int(GetMouseOverLocation());
			if(iLockTargeID == 0 || !((SM_ClientGamePlayerController*)m_pkSMController)->IsTargetAttackable(this, iLockTargeID) )
			{
				kLocation = kMouseLocation;
				iLockTargeID = 0;
			}
			MG_Req_PlayerExecuteSkillTypeLocation msgReqSkill;
			msgReqSkill.m_lRequesterID		= GetID();
			msgReqSkill.m_lSkillExecuteID	= pkSkill->GetExecuteID();
			msgReqSkill.m_lItemTypeID		= 0;
			msgReqSkill.m_lCurrentLocationX = GetCurrentLocation().x;
			msgReqSkill.m_lCurrentLocationY	= GetCurrentLocation().y;
			msgReqSkill.m_lCurrentLocationZ	= GetCurrentLocation().z;
			msgReqSkill.m_lLockedLocationX	= kLocation.x;
			msgReqSkill.m_lLockedLocationY  = kLocation.y;
			msgReqSkill.m_lLockedLocationZ  = kLocation.z;
			msgReqSkill.m_fClientTime = GET_PROCESS_TIME;
			msgReqSkill.m_lMouseLocationX = kMouseLocation.x;
			msgReqSkill.m_lMouseLocationY = kMouseLocation.y;
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_PLAYER_EXECUTE_SKILL_TYPE_LOCATION, &msgReqSkill);

			if ( pkSkill->GetProperty_ExecutePriority() )
			{
				ClearPendingSkill();
				PrepareExecuteSkill(pkSkill,0);
				ExecuteSkill(fCurrentTime,pkSkill,0);
				m_skillRltDelayTimer.Start(SKILL_RLT_DELAY);
			}
		}
	default:
		break;
	}

	if(bCreateMsg)
	{
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

		GfxWriteLog(LOG_SKILL_FILELINE, LOG_SWITCH_INFO, 
			"[Skill]Request ToySkill, time=%.3f, player=%d, skill_id=%d, FrameID=%d",
			fCurrentTime, GetID(), pkSkill->GetExecuteID(), 
			UT_TimeManager::GetSingleton().CurrentFrameNo()
			);
	}
}
//--------------------------------------------------------
void LC_ClientGamePlayer::OnRequestExecuteSkillFailed(int lSkillID)
{
	this->_doUnlockRequestExecuteSkill(false, lSkillID);

	CF_SkillExecuteList::DataEntry* pkSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(GetCurrentStateType());
	SK_SkillExecute* pkSkill = SK_Factory::GetSingletonPtr()->RequestSkillExcute(lSkillID);

	if (!m_skillRltDelayTimer.IsDown())
	{
		if(IS_SKILL_TYPE(GetCurrentStateType()) && !pkSkill->GetProperty_IsLCSkill())
		{
			if (pkSkillData)
			{
				//引导型技能释放，服务器在合理的时间内返回失败，客户端必须强行打断，否则，会出现技能无法停止的情况
				if ( pkSkillData->_iSkillProcessCategory == SPC_INDUCE)
				{
					InterruptSkill(GET_PROCESS_TIME, GetCurrentStateType(), SKI_FORCE);
				}
			}
		}
	}

	if ( pkSkillData )
	{
		//如果执行技能失败，消息返回的时候需要，清除这个技能的CD，因为服务器如果没有执行技能，就不会同步技能CD，但是客户端CD在客户端执行技能的时候就已经计算了
		//引导型技能是脚本内设置CD，这边不能清楚
		LC_SkillCoolDownMap* pkMap = GetSkillCoolDownMap();
		float needcd = pkMap->GetSkillNeedCDTime(lSkillID);
		if ( pkSkillData->_iSkillProcessCategory != SPC_INDUCE )
		{
			pkMap->ClearSkillCD(lSkillID);
		}
	}

}

int LC_ClientGamePlayer::GetCurrentSkillTypeID()
{
	int32_t iID = GetCurrentStateType();
	if (IS_SKILL_TYPE(iID))
		return iID;
	else
		return -1;
}
//--------------------------------------------------------
void LC_ClientGamePlayer::_doUnlockRequestExecuteSkill(bool bExecute, int lSkillID)
{
	if (m_iReqNoRltExecSkillID == lSkillID)	//right response
	{
		_clearUnlockRequestExecuteSkill();
	}
}
void LC_ClientGamePlayer::_updateUnlockRequestExecuteSkill(float fCurrentTime)
{
	if (m_iReqNoRltExecSkillID != 0)
	{
		float t = fCurrentTime;
		if (t - m_fReqExecSkillTime > 1)	//time out
		{
			_clearUnlockRequestExecuteSkill();
		}
	}
}
void LC_ClientGamePlayer::_clearUnlockRequestExecuteSkill(void)
{
	m_iReqNoRltExecSkillID = 0;
	m_fReqExecSkillTime = 0;
}
//--------------------------------------------------------
void LC_ClientGamePlayer::RequestExecuteSkillScript(float fCurrentTime, int lSkillid, int lItemID)
{
	SK_Factory* pkSkillFactory = SK_Factory::GetSingletonPtr();
	SK_SkillExecute* pkSkillExecute = pkSkillFactory->RequestSkillExcute(lSkillid);
	if(pkSkillExecute)
	{
		RequestExecuteSkill(fCurrentTime, pkSkillExecute, lItemID);
	}
}
//--------------------------------------------------------
void LC_ClientGamePlayer::RequestInterrputSkill(float fCurrentTime,int lSkillTypeID,SkillInterruptType eInterruptType)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	//发送网络请求

	MG_Req_PlayerInterruptSkill		reqMsg;
	reqMsg.m_lRequesterID			= GetID();
	reqMsg.m_lSkillExecuteID		= lSkillTypeID;
	reqMsg.m_usInterruptType		= (uint8_t)eInterruptType;
	reqMsg.m_fClientTime			= GET_PROCESS_TIME;

	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_PLAYER_INTERRUPT_SKILL, &reqMsg);

	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	//如果是引导型，技能就马上中断，因为出现了两种引导型技能，客户端无法停止的操作
	CF_SkillExecuteList::DataEntry* skillEntry = LC_Helper::SafeGetSkillExecuteDataEntryPtr(lSkillTypeID);
	if ( skillEntry && skillEntry->_iSkillProcessCategory == SPC_INDUCE && SKI_FORCE_STAGE != eInterruptType)
	{
		this->_doUnlockRequestExecuteSkill(false, lSkillTypeID);
		InterruptSkill(fCurrentTime, lSkillTypeID, eInterruptType);
	}
	GfxWriteLog(LOG_TAG_SKILL, LOG_SWITCH_SYSTEM, "[Skill]RequestInterrputSkill = %d, InterruptType = %d, FrameID = %d", 
		lSkillTypeID, eInterruptType, UT_TimeManager::GetSingleton().CurrentFrameNo());

	EndNormalContinueCheck();
	return;
}

ResultType LC_ClientGamePlayer::InterruptSkill(float fCurrentTime, int lSkillTypeID, SkillInterruptType eInterruptType)
{
	this->_doUnlockRequestExecuteSkill(false, lSkillTypeID);

	return LC_PlayerBase::InterruptSkill(fCurrentTime, lSkillTypeID, eInterruptType);
}

//--------------------------------------------------------
ResultType LC_ClientGamePlayer::WaitPendingSkill(int lSkillTypeID,int lItemID)
{
	SK_SkillExecute* pkSkill = SK_Factory::GetSingleton().RequestSkillExcute(lSkillTypeID);
	if(!pkSkill)
		return RE_SKILL_SYSTEM_ERROR;

	//bool bWaitPendingSkill = false;
	ResultType eResult = _checkNeedLockTarget(pkSkill);
	switch(eResult)
	{
	case RE_SKILL_CHECK_NEED_TARGET:
		{
			if ( GET_PROCESS_TIME - m_needTargetMessgeTick > NEED_TARGET_INTERVAL)
			{
				m_needTargetMessgeTick = GET_PROCESS_TIME;
				SendLogicInfoMessage(eResult, pkSkill->GetProperty_SkillName());
			}//SetNeedLockTarget(true);
			//SetNeedLockLocation(false);
			//bWaitPendingSkill = true;
			return eResult;
		}
		break;
	case RE_SKILL_CHECK_NEED_LOCATION:
		{
			/*
			SetNeedLockTarget(false);
			SetNeedLockLocation(true);
			bWaitPendingSkill = true;

			/*/
			SetNeedLockTarget(false);
			SetNeedLockLocation(true);	//ClearWaitPendingSkill中会设为false
			//*/
		}
		break;
	case RE_SKILL_SYSTEM_ERROR:
		{
			ClearWaitPendingSkill();
			return eResult;
		}
		break;
	default:
		eResult = RE_SKILL_PENDING_SUCCESS;
	}

	/*if(bWaitPendingSkill )
	{
		m_pkWaitPendingSkill = pkSkill;
		m_lWaitPendingSkillItemType = lItemID;
	}
	else*/
	{
		ClearWaitPendingSkill();	//其中会调用SetNeedLockLocation(false)
		eResult = PendingSkill(lSkillTypeID,lItemID);
	}

	return eResult;
}
//--------------------------------------------------------
void LC_ClientGamePlayer::ClearWaitPendingSkill()
{
	if (NULL != m_pkWaitPendingSkill)
	{
		((LC_ClientShortcutBar*)GetShortcutBar())->SetShortcutTriggerState(m_pkWaitPendingSkill->GetExecuteID(), false);
	}

	m_pkWaitPendingSkill = NULL;
	m_lWaitPendingSkillItemType = 0;
	SetNeedLockTarget(false);
	SetNeedLockLocation(false);
}
//------------------------------------------------------------------------------
bool LC_ClientGamePlayer::WaitPendingSkillSuccess(int lSkillID)
{
	if (NULL != m_pkWaitPendingSkill && m_pkWaitPendingSkill->GetExecuteID() == lSkillID)
	{
		return true;
	}
	if (NULL != m_pkPendingSkill && m_pkPendingSkill->GetExecuteID() == lSkillID)
	{
		return true;
	}
	return false;
}
//----------------------------------------------------------
bool LC_ClientGamePlayer::_circleFindPosition(float lth, int angleSplit, const UT_Vec3Int& tarpos)
{
	if (angleSplit <=0 )
		angleSplit = 4;

	if (lth <= 0 )
		lth = 150.0f;

	bool bFlag = false;
	for (int i = 0 ; i < angleSplit; i++)
	{
		UT_Vec2Float ttvf = UT_MathBase::ZRot(UT_Vec2Float(0.0f,1.0f), i * 360.0f/angleSplit);
		UT_Vec3Int ffif( int(ttvf.x * lth) , int(ttvf.y * lth), 0.0f); 
		bool bb = SetTargetLocation( tarpos + ffif) ;
		if ( !bb )
		{
			continue;
		}
		else
		{
			bFlag = true;
			break;
		}
	}
	return bFlag;
}
//------------------------------------------------------------------------------
void LC_ClientGamePlayer::RecordLastSkillID(int lSKillID)
{
	if ( !GetAIControl() )
		m_kGameOptCache.PushSkill(lSKillID);
}
//------------------------------------------------------------------------------
void LC_ClientGamePlayer::SetSkillActivateAll(int cID, bool bActivate)
{
	m_mapActivate[cID] = bActivate;
}
//------------------------------------------------------------------------------
bool LC_ClientGamePlayer::GetSkillActivateAll(int cID)
{
	mem::map<int,bool>::iterator iter = m_mapActivate.find( cID );
	if ( iter != m_mapActivate.end() )
	{
		return iter->second;
	}
	return false;
}
//------------------------------------------------------------------------------
void LC_ClientGamePlayer::SetBuddyRuneLevel(int cID,const mem::vector<int>& runelevel)
{
	mem::vector<int>& kVec = m_vecRunelevel[cID];
	kVec.clear();

	for (int i=0;i<4;i++)
	{		
		kVec.push_back(runelevel[i]);
	}
}
//------------------------------------------------------------------------------
const mem::vector<int>& LC_ClientGamePlayer::GetBuddyRuneLevel(int cID)
{
	mem::vector<int>& runelevel = m_vecRunelevel[cID];
	if (runelevel.empty())
	{
		runelevel.resize(GetGlobalSetting.RunePackSize);
	}
	return runelevel;
}
//------------------------------------------------------------------------------
void LC_ClientGamePlayer::SetBuddyEquipLevel(int cID,const mem::vector<int>& equiplevel)
{
	mem::vector<int>& kVec = m_vecEquiplevel[cID];
	kVec.clear();

	for (int i=0;i<4;i++)
	{		
		kVec.push_back(equiplevel[i]);
	}
}

void LC_ClientGamePlayer::SetBuddyEquipLevel(int cID, int index, int level)
{
	mem::vector<int>& kVec = m_vecEquiplevel[cID];
	if(kVec.empty())
		kVec.resize(GetGlobalSetting.ArtifactPackSize);

	kVec[index] = level;
}
//------------------------------------------------------------------------------
const mem::vector<int>& LC_ClientGamePlayer::GetBuddyEquipLevel(int cID)
{
	return m_vecEquiplevel[cID];
}

void LC_ClientGamePlayer::SetUpBuddyEquipGrade(int cID, int index)
{
	LC_ClientBuddyInstance* pkInst = GetBuddyInstance(cID);
	if(pkInst == NULL)
		return;

	int iGrade = ITEM_GRADE_NONE;
	LC_BackPackEntry* pkItemEntry = pkInst->GetEquipEntry(index);
	if(pkItemEntry->GetValid())
	{
		LC_ItemDescriptionEntry *itemDescript = 
			LC_ItemFactoryBase::GetSingletonPtr()->GetItemDescriptionEntry(pkItemEntry->GetTypeID());
		if(itemDescript != NULL)
			iGrade = itemDescript->GetGrade() - 1;
	}

	mem::vector<int>& kVec = m_vBuddyEquipItemIds[cID];
	if(kVec.empty())
		kVec.resize(GetGlobalSetting.ArtifactPackSize);

	kVec[index] = iGrade;
}

void LC_ClientGamePlayer::SetBuddyEquipGrades(int cID, const mem::vector<int>& equipGrade)
{
	mem::vector<int>& kVec = m_vBuddyEquipItemIds[cID];
	kVec = equipGrade;
}

const mem::vector<int>& LC_ClientGamePlayer::GetBuddyEquipGrades(int cID)
{
	mem::vector<int>& kVec = m_vBuddyEquipItemIds[cID];
	if (kVec.empty())
	{
		kVec.resize(GetGlobalSetting.ArtifactPackSize);
	}
	return kVec;
}
//------------------------------------------------------------------------------
void LC_ClientGamePlayer::SetTestDataBase(const std::vector<int32_t> & topicID)
{
	m_vecTopicID = topicID;
}
//------------------------------------------------------------------------------
StringType LC_ClientGamePlayer::GetTestDataBase()
{
	StringStreamType sStream("");
	for (int i=0;i<m_vecTopicID.size();++i)
	{
		sStream << m_vecTopicID[i] << "#";
	}
	return sStream.str();
}
//------------------------------------------------------------------------------
void LC_ClientGamePlayer::SetRaidBossSpeciesMyRank(const PS_SettleInfo& PlayerInfo)
{
	m_vRaidBossSpeciesMyRank.clear();
	PS_SettleInfo& kInfo = m_vRaidBossSpeciesMyRank;
	kInfo.m_nCharName = PlayerInfo.m_nCharName;
	kInfo.m_nControllerLevel = PlayerInfo.m_nControllerLevel;
	kInfo.m_nDamageData = PlayerInfo.m_nDamageData;
	kInfo.m_nRankIndex = PlayerInfo.m_nRankIndex;
	kInfo.m_nBornType = PlayerInfo.m_nBornType;
	kInfo.m_nCommanderTitleFrameID = PlayerInfo.m_nCommanderTitleFrameID;
	kInfo.m_nCommanderTitle = PlayerInfo.m_nCommanderTitle;
	if (PlayerInfo.m_nRankIndex >= 0)
	{
		kInfo.m_nVecBuddyInfo = PlayerInfo.m_nVecBuddyInfo;
		kInfo.m_nVecBossInfo = PlayerInfo.m_nVecBossInfo;
	}
}

void LC_ClientGamePlayer::SetRaidBossSpeciesMyDamageCount(int iStoryId,int64_t iDamage)
{
	m_vThemeACTRaidBossDamage[iStoryId]=iDamage;
}

StringType LC_ClientGamePlayer::GetRaidBossSpeciesMyDamageCount(int iStoryId)
{
	StringStreamType sStream;
	mem::map<int,int64_t>::iterator iter= m_vThemeACTRaidBossDamage.find(iStoryId);
	if (iter!=m_vThemeACTRaidBossDamage.end())
	{
		sStream<<iter->second;
	}
	else
	{
		sStream<<"0";
	}
	return sStream.str();
}

//------------------------------------------------------------------------------
void LC_ClientGamePlayer::ReSetRaidBossSpeciesRankThree()
{
	m_vRaidBossSpeciesRankThree.clear();
	m_vRaidBossSpeciesRankList.clear();
}
//------------------------------------------------------------------------------
void LC_ClientGamePlayer::SetRaidBossSpeciesRank(const std::vector<PS_SettleInfo>& PlayerInfo, bool rankThree)
{
	m_vRaidBossSpeciesRankList.clear();
	mem::vector<PS_SettleInfo>& kInfo = m_vRaidBossSpeciesRankList;
	for (int i=0;i<PlayerInfo.size();i++)
	{
		if (i>= 5)
			return;
		kInfo.push_back(PlayerInfo[i]);
	}

	if (rankThree)
	{
		m_vRaidBossSpeciesRankThree.clear();
		mem::vector<PS_SettleInfo>& tInfo = m_vRaidBossSpeciesRankThree;
		for (int i=0;i<PlayerInfo.size();i++)
		{
			if (i>= 3)
				return;
			tInfo.push_back(PlayerInfo[i]);
		}
	}
}

void LC_ClientGamePlayer::ReqRaidBossSpeciesDamageData(int32_t iStoryID, uint64_t iCharID)
{
	MG_REQ_YiZhongQinXiDamageData reqMsg;
	reqMsg.m_nStoryID = iStoryID;
	reqMsg.m_nCharID = iCharID;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_YIZHONGQINGXI_DAMAGEDATA, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::ReqRaidBossSpeciesDamageDataDetail(int32_t iStoryID, uint32_t iTime, uint64_t iCharID)
{
	MG_REQ_YiZhongQinXiDetailDamageData reqMsg;
	reqMsg.m_nStoryID = iStoryID;
	reqMsg.m_nTime = iTime;
	reqMsg.m_nCharID = iCharID;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_YIZHONGQINGXI_DETAILDAMAGEDATA, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::RltRaidBossSpeciesDamageData(const MG_RLT_YiZhongQinXiDamageData& msg)
{
	StringStreamType sStream;
	std::vector<PS_YiZhongQinXiDataInfo>::const_iterator iter = msg.m_nDataInfo.begin();
	sStream << msg.m_nCharID << '#';
	sStream << msg.m_nStoryID << '#';
	for(; iter!=msg.m_nDataInfo.end(); ++iter)
	{
		const PS_YiZhongQinXiDataInfo& info = *iter;
		sStream << info.m_nTime << ':' << info.m_nDamageData << ';';
	}
	sStream << '#';

	LC_ClientGameEvent kGameEvent;
	kGameEvent.SetClientEventType(CGET_MSG_RLT_YIZHONGQINXIDAMAGEDATA);

	LC_ClientGameEventUserData_String kUserData(sStream.str());
	kGameEvent.SetUserData(&kUserData);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void LC_ClientGamePlayer::RltRaidBossSpeciesDamageDataDetail(const MG_RLT_YiZhongQinXiDetailDamageData& msg)
{
	StringStreamType sStream;

	const PS_YiZhongQinXiDataInfo& baseInfo = msg.m_nBaseInfo;
	sStream << baseInfo.m_nBornType << '#' << baseInfo.m_nCharName << '#' << baseInfo.m_nControllerLevel << '#' << baseInfo.m_nCommanderTitleFrameID << '#';

	const std::vector<PS_Buddy>& buddyInfo = msg.m_nVecBuddyInfo;
	sStream << genRaidBossSpeciesInfoStr(msg.m_nVecBuddyInfo, msg.m_nVecBossInfo);

	LC_ClientGameEvent kGameEvent;
	kGameEvent.SetClientEventType(CGET_MSG_RLT_YIZHONGQINXI_DETAILDAMAGEDATA);
	LC_ClientGameEventUserData_String kUserData(sStream.str());
	kGameEvent.SetUserData(&kUserData);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

StringType LC_ClientGamePlayer::genRaidBossSpeciesInfoStr(const std::vector<PS_Buddy>& buddyInfo, const std::vector<PS_Boss>& monsterInfo)
{
	StringStreamType sStream;
	std::vector<PS_Buddy>::const_iterator buddyItr = buddyInfo.begin();
	for(; buddyItr!=buddyInfo.end(); ++buddyItr)
	{
		const PS_Buddy& record = *buddyItr;
		const PS_Buddy_Info_BuddyCS& buddyInfo = record.m_nBuddyData;
		sStream <<  buddyInfo.m_nBuddyID << '@' << record.m_nBuddyIndex << '@';
		sStream  << record.m_nDamageData << '@' << record.m_nHealData << '@' << record.m_nHurtData << '@';
		sStream << buddyInfo.m_nBuddyLevel << '@' << buddyInfo.m_nBuddyStarLevel << '@' << buddyInfo.m_bActivateAllSkill << '@';
		sStream << buddyInfo.m_nAllEquipSlotReinforceLevel << '@';
		std::map<int32_t,PS_RuneItemInfo_BuddyCS>::const_iterator itr = buddyInfo.m_nBuddyRuneInfo.begin();
		for (;itr!=buddyInfo.m_nBuddyRuneInfo.end();++itr)
		{
			const PS_RuneItemInfo_BuddyCS& info = itr->second;
			sStream << itr->first << ';' << info.m_nRuneStarLevel << ';' << info.m_nRuneItemID << ';';
		}
		sStream << '@';

		std::map<int32_t,PS_EquipItemInfo_BuddyCS>::const_iterator itr2 = buddyInfo.m_nEquipSlotInfo.begin();
		for (;itr2!=buddyInfo.m_nEquipSlotInfo.end();++itr2)
		{
			sStream << itr2->first << ';' << itr2->second.m_nEquipItemID << ';';
		}
		sStream << "@|";
	}

	sStream << '#';

	std::vector<PS_Boss>::const_iterator monsterItr = monsterInfo.begin();
	for(; monsterItr!=monsterInfo.end(); ++monsterItr)
	{
		const PS_Boss& record = *monsterItr;
		sStream <<  record.m_nCharTypeID << '@';
		sStream  << record.m_nDamageData << '@' << record.m_nHealData << '@' << record.m_nHurtData << '@';

		const CF_CharType::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_CharType>(record.m_nCharTypeID);
		if(pkData != NULL)
		{
			sStream << pkData->_lLevel << '@' << pkData->_kCharName << '@';
			sStream << Int2String(pkData->_elementdamagetype) << '@' << pkData->_kTalkHeadIcon << '@';
		}
		else
		{
			sStream << "0@0@0@";
		}
		sStream << '|';
	}

	return sStream.str();
}
//------------------------------------------------------------------------------
StringType LC_ClientGamePlayer::GetRaidBossSpeciesMyRank()
{
	StringStreamType sStream("");
	PS_SettleInfo& kInfo = m_vRaidBossSpeciesMyRank;
	if (kInfo.m_nRankIndex >= 0)
	{
		sStream << kInfo.m_nCharName << "#";
		sStream << kInfo.m_nControllerLevel << "#";
		sStream << kInfo.m_nDamageData << "#";
		sStream << kInfo.m_nRankIndex << "#";
		sStream << kInfo.m_nBornType << "#";
		for (int j=0;j<kInfo.m_nVecBuddyInfo.size();j++)
		{
			const PS_Buddy& bInfo = kInfo.m_nVecBuddyInfo[j];
			const PS_Buddy_Info_BuddyCS& buddyInfo = bInfo.m_nBuddyData;
			sStream << buddyInfo.m_nBuddyID << "|";
			sStream << buddyInfo.m_nBuddyLevel << "|";
			sStream << buddyInfo.m_nBuddyStarLevel << "|";
			sStream << buddyInfo.m_bActivateAllSkill << "|";
			sStream << buddyInfo.m_nAllEquipSlotReinforceLevel << "|";
			std::map<int32_t,PS_RuneItemInfo_BuddyCS>::const_iterator itor = buddyInfo.m_nBuddyRuneInfo.begin();
			for (;itor!=buddyInfo.m_nBuddyRuneInfo.end();++itor)
			{
				const PS_RuneItemInfo_BuddyCS& info = itor->second;
				sStream << itor->first << ';' << info.m_nRuneStarLevel << ';' << info.m_nRuneItemID << ';';
			}
			sStream << "|";
			
			std::map<int32_t,PS_EquipItemInfo_BuddyCS>::const_iterator itr2 = buddyInfo.m_nEquipSlotInfo.begin();
			for (;itr2!=buddyInfo.m_nEquipSlotInfo.end();++itr2)
			{
				sStream << itr2->first << ';' << itr2->second.m_nEquipItemID << ';';
			}
			sStream << "|";
			sStream << "@";
		}
		sStream << "#";
		sStream << kInfo.m_nCommanderTitleFrameID << "#";
		sStream << kInfo.m_nCommanderTitle << "#";
	}
	return sStream.str();
}
//------------------------------------------------------------------------------
StringType LC_ClientGamePlayer::GetRaidBossSpeciesRank(int index, bool rankThree)
{
	StringStreamType sStream("");
	PS_SettleInfo* kInfo = NULL;
	if (rankThree && index < m_vRaidBossSpeciesRankThree.size())
		kInfo = &m_vRaidBossSpeciesRankThree[index];
	else if(!rankThree && index < m_vRaidBossSpeciesRankList.size())
		kInfo = &m_vRaidBossSpeciesRankList[index];
	if (kInfo != NULL && kInfo->m_nDamageData > 0)
	{
		sStream << kInfo->m_nCharName << "#";
		sStream << kInfo->m_nControllerLevel << "#";
		sStream << kInfo->m_nDamageData << "#";
		sStream << kInfo->m_nRankIndex << "#";
		sStream << kInfo->m_nBornType << "#";

		for (int j=0;j<kInfo->m_nVecBuddyInfo.size();j++)
		{
			PS_Buddy& info = kInfo->m_nVecBuddyInfo[j];
			const PS_Buddy_Info_BuddyCS& bInfo = info.m_nBuddyData;

			sStream << bInfo.m_nBuddyID << "|";
			sStream << bInfo.m_nBuddyLevel << "|";
			sStream << bInfo.m_nBuddyStarLevel << "|";
			sStream << bInfo.m_bActivateAllSkill << "|";
			sStream << bInfo.m_nAllEquipSlotReinforceLevel << "|";

			std::map<int32_t,PS_RuneItemInfo_BuddyCS>::const_iterator itr = bInfo.m_nBuddyRuneInfo.begin();
			for (;itr!=bInfo.m_nBuddyRuneInfo.end();++itr)
			{
				const PS_RuneItemInfo_BuddyCS& info = itr->second;
				sStream << itr->first << ';' << info.m_nRuneStarLevel << ';' << info.m_nRuneItemID << ';';
			}
			sStream << '|';

			std::map<int32_t,PS_EquipItemInfo_BuddyCS>::const_iterator itr2 = bInfo.m_nEquipSlotInfo.begin();
			for (;itr2!=bInfo.m_nEquipSlotInfo.end();++itr2)
			{
				sStream << itr2->first << ';' << itr2->second.m_nEquipItemID << ';';
			}
			sStream << "|@";
		}
		sStream << "#";
		sStream << kInfo->m_nCommanderTitleFrameID << "#";
		sStream << kInfo->m_nCharID << "#";
		sStream << kInfo->m_nCommanderTitle << "#";
	}
	return sStream.str();
}
//------------------------------------------------------------------------------
void LC_ClientGamePlayer::ReqGetRaidBossSpeciesRecord(int index, bool bSelf)
{
	PS_SettleInfo* pkInfo = NULL;
	if(bSelf)
		pkInfo = &m_vRaidBossSpeciesMyRank;
	else if(index < m_vRaidBossSpeciesRankList.size())
		pkInfo = &m_vRaidBossSpeciesRankList[index];

	if(pkInfo == NULL)
		return;

	StringStreamType sStream;
	sStream << pkInfo->m_nBornType << '#';
	sStream << pkInfo->m_nCharName << '#';
	sStream << pkInfo->m_nControllerLevel << '#';
	sStream << pkInfo->m_nCommanderTitleFrameID << '#';
	sStream << genRaidBossSpeciesInfoStr(pkInfo->m_nVecBuddyInfo, pkInfo->m_nVecBossInfo) << "#";

	LC_ClientGameEvent kGameEvent;
	kGameEvent.SetClientEventType(CGET_MSG_RLT_YIZHONGQINXI_DETAILDAMAGEDATA);
	LC_ClientGameEventUserData_String kUserData(sStream.str());
	kGameEvent.SetUserData(&kUserData);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}
//------------------------------------------------------------------------------
void LC_ClientGamePlayer::ClearPendingSkill()
{
	SK_SkillExecute* pkSkillExecute = GetPendingSkillExecute();
	if (NULL != pkSkillExecute)
	{
		LC_ShortcutBar* shortcutBar = GetShortcutBar();
		if (shortcutBar == NULL)
			shortcutBar = m_pkShortcurtBar;
		((LC_ClientShortcutBar*)shortcutBar)->SetShortcutTriggerState(pkSkillExecute->GetExecuteID(), false);
	}
	LC_ActorBase::ClearPendingSkill();
}
//--------------------------------------------------------
//bool LC_ClientGamePlayer::IsTransformAttackSkill( int lSkillID )
//{
//	if (GetShapeShiftFlag())
//	{
//		CF_CharType* pkCharType = CF_CharType::GetSingletonPtr();
//		CF_CharType::DataEntry* pkCharData = pkCharType->GetEntryPtr(GetShapeShiftCharTypeID());
//		if(pkCharData == NULL)
//			return false;
//
//		for (int i = 0;i < 5;++i)
//		{
//			int lAttackSkillID = CSVHelper::GetCharSkillID(pkCharData, i + 1);
//			if (IS_SKILL_TYPE(lAttackSkillID)&& lSkillID == lAttackSkillID)
//				return true;
//		}
//	}
//
//	return false;
//}
//--------------------------------------------------------
void LC_ClientGamePlayer::SetSkillFailReason(int& lSkillFailReason, int iReasonIndex, bool bFail)
{
	if (iReasonIndex <= SFR_NONE || iReasonIndex >= SFR_MAX_COUNT)
	{
		return;
	}
	int lMask = (1 << (iReasonIndex - 1));
	if (bFail)
	{
		lSkillFailReason |= lMask;
	}
	else
	{
		lSkillFailReason &= (~lMask);
	}
}
//--------------------------------------------------------
bool LC_ClientGamePlayer::GetSkillFailReason(int lSkillFailReason, int iReasonIndex)
{
	if (iReasonIndex <= SFR_NONE || iReasonIndex >= SFR_MAX_COUNT)
	{
		return false;
	}
	int lMask = (1 << (iReasonIndex - 1));
	return (lSkillFailReason & lMask) != 0;
}
//--------------------------------------------------------
void	LC_ClientGamePlayer::SetAttackerID(object_id_type lID)
{ 
	LC_PlayerBase::SetAttackerID(lID);

	m_kLastAttackerID.insert(lID);

	////被攻击自动选择目标，且清空标记
	//if (GetLockedTargetID() == 0 && (IS_PLAYER_CLASS_ID(lID) || IS_NPC_ID(lID)) )
	//{
	//	LC_WorldManagerBase* pkWorldManager = ENGINE_GET_WORLD_MANAGER();
	//	LC_ActorBase* pkTarget = (LC_ActorBase*)pkWorldManager->FindObject(lID);
	//	if (pkTarget)
	//	{
	//		CF_ActorFilterFunc* pkCSVActorFilterFunc = CF_ActorFilterFunc::GetSingletonPtr();
	//		CF_ActorFilterFunc::DataEntry* pkData = pkCSVActorFilterFunc->GetEntryPtr(2);
	//		if (pkData && LC_Helper::IsValidTarget(this, pkTarget, pkData) == RE_SKILL_CHECK_VALID_TARGET)
	//		{
	//			SetLockedTargetID(lID);
	//			SetPreLockedTargetID(lID);
	//		}
	//	}
	//}

	////和平模式被动机自动切换成善恶模式
	////在野外，而且不是是力争多在，才进行善恶模式的保护切换
	//// weixin 翻译：
	////和平模式被攻击自动切换成善恶模式
	////在野外，而且不是势力争夺战，才进行善恶模式的保护切换
	//bool bShow = (SC_ClientScriptInterface::GetSingletonPtr()->GetSkillInterface()->IsPkPunishArea(GetMapResID()) == 1);
	//bool bfixmin = SC_ClientScriptInterface::GetSingletonPtr()->GetSkillInterface()->CanChangePKMode(GetMapResID(),PKM_MIN);
	//bool bfixgoodevil = SC_ClientScriptInterface::GetSingletonPtr()->GetSkillInterface()->CanChangePKMode(GetMapResID(),PKM_GOODEVIL);
	//if (bShow && IS_PLAYER_CLASS_ID(lID) && GetPKRecorder()->GetPKMode() == PKM_PEACE && (bfixmin || bfixgoodevil))
	//{
	//	GetPKRecorder()->SetPKMode(PKM_GOODEVIL);
	//	RequestModifyPKSetting();
	//}
}

//--------------------------------------------------------
StateType LC_ClientGamePlayer::FindMoveState()
{
	if(IS_SKILL_TYPE(m_iNextState))
		return (StateType)m_iNextState;

	PF_Path& rkMovePath = GetMovePath();

	/*int iTargetDistance = UT_EngineMath::LineLengthXYInt(m_kCurrentLocation,m_kTargetLocation);*/
	float fPathMoveStep = rkMovePath.GetMoveStep();
	int	 iMovePathPointCount = rkMovePath.GetPathPointCount();

	bool bFrezz = GetSkillFlag(SK_DIZZ) || GetSkillFlag(SK_SLEEP);
	if(bFrezz)
		return ST_PLAYER_STAND;

	if(fPathMoveStep > MOVE_STEP_END_FLAG || iMovePathPointCount <= 1)
	{
		if(GetMoveStraightFlag())
		{
			SetMoveStraightFlag(false);

			//LC_GameEvent kGameEvent;
			//kGameEvent.SetEventType(GLET_ACTIVE_CAREER_STUDY);
			//kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, m_lMoveStraightParam);

			//(CLIENT_GET_WORLD_MANAGER())->HandleGameEvent(kGameEvent);
		}
		else
		{
			SetNextState(ST_PLAYER_STAND);
		}
		return ST_PLAYER_STAND;
	}
	else 
		return ST_PLAYER_RUN;
}
//------------------------------------------------------------------------
ResultType LC_ClientGamePlayer::PendingJump(StateMachine::StateType st, UT_Vec3Int & kTargetLocation)
{
	//关闭跟随功能
	this->StopFollowTarget(0);

	ResultType eResult = LC_PlayerBase::PendingJump(st,kTargetLocation);
	if(eResult != RE_SKILL_PENDING_SUCCESS)
	{
		if (eResult != RE_SKILL_SYSTEM_ERROR)
		{
			SendLogicInfoMessage(eResult);
		}
		return eResult;
	}
	//ClearPathPendingAction();

	return eResult;
}
//------------------------------------------------------------------------
ResultType LC_ClientGamePlayer::CheckPendingJump(StateMachine::StateType st)
{
	//死了
	if (GetDead())
	{
		return RE_SKILL_SYSTEM_ERROR;
	}

	//AI控制状态下不处理
	if (GetAIControl())
	{
		SendLogicInfoMessage(RE_SKILL_PENDING_FAIL_FLAG_NEED);
		return RE_SKILL_SYSTEM_ERROR;
	}

	//技能请求未返回，期间不能使用轻功
	if (m_iReqNoRltExecSkillID)
	{
		SendLogicInfoMessage(RE_SKILL_PENDING_FAIL_INTERRUPT);
		return RE_SKILL_PENDING_FAIL_INTERRUPT;
	}

	int iCurrentState = this->GetCurrentStateType();
	
	switch (iCurrentState)
	{
	case ST_PLAYER_JUMP1:
		return RE_SKILL_SYSTEM_ERROR;
	case ST_PLAYER_PUNCH_BACK:
	case ST_PLAYER_PASSIVE_MOVE:
		SendLogicInfoMessage(RE_SKILL_PENDING_FAIL_INTERRUPT);
		return RE_SKILL_SYSTEM_ERROR;
	default:
		if (IS_SKILL_TYPE(iCurrentState) && LC_Helper::GetNormalAttackSkill(this) == iCurrentState)
		{
			//ToCheck: 应策划要求 移动射击遇到传送点需要传送
			ResultType eCheckResult = CheckInterruptSkill(iCurrentState,SKI_FORCE);
			if(eCheckResult == RE_SKILL_NONE_ERROR)
			{
				RequestInterrputSkill(GET_PROCESS_TIME, iCurrentState, SKI_FORCE);
			}
			else
			{
				SendLogicInfoMessage(RE_SKILL_PENDING_FAIL_INTERRUPT);
				return RE_SKILL_SYSTEM_ERROR;
			}
		}
		break;
	}

	//目标拥有某些SkillFlag其中之一就不能释放该技能
	if ( 0 != (GetAllSkillFlag() & SK_JUMP_NA_FLAGS) )
	{
		SendLogicInfoMessage(RE_SKILL_CHECK_SKILLFLAG_OVERDO);
		return RE_SKILL_CHECK_SKILLFLAG_OVERDO;
	}

	return RE_SKILL_NONE_ERROR;

//下面全没用了
	////被击退状态不能跳, 被动位移状态 (整合到一起了，啰嗦)
	//if (iCurrentState == ST_PLAYER_PUNCH_BACK || iCurrentState == ST_PLAYER_PASSIVE_MOVE)
	//{
	//	SendLogicInfoMessage(RE_SKILL_PENDING_FAIL_INTERRUPT);
	//	return RE_SKILL_SYSTEM_ERROR;
	//}


	//技能状态不能跳跃 (整合到一起了,啰嗦)
	//if (IS_SKILL_TYPE(iCurrentState))
	//{
	//	//ToCheck: 应策划要求 移动射击遇到传送点需要传送
	//	ResultType eCheckResult = CheckInterruptSkill(iCurrentState,SKI_FORCE);
	//	if(eCheckResult == RE_SKILL_NONE_ERROR)
	//		RequestInterrputSkill(GET_PROCESS_TIME, iCurrentState, SKI_FORCE);
	//	else
	//	{
	//		SendLogicInfoMessage(RE_SKILL_PENDING_FAIL_INTERRUPT);
	//		return RE_SKILL_SYSTEM_ERROR;
	//	}
	//}

	//判断当前状态和请求状态是否矛盾 (新版冲刺不再检查)
	/*switch(iCurrentState)
	{
	case ST_PLAYER_JUMP1:
		if (st != ST_PLAYER_JUMP2)		
			return RE_SKILL_SYSTEM_ERROR;
		break;

	case ST_PLAYER_JUMP2:
		if (st != ST_PLAYER_JUMP3)
			return RE_SKILL_SYSTEM_ERROR;
		break;
	case ST_PLAYER_JUMP3:
		return RE_SKILL_SYSTEM_ERROR;	
	case ST_PLAYER_JUMP_GUIDE:
		return RE_SKILL_SYSTEM_ERROR;
	}*/

	//判断状态、耐力、有效高度、垂直速度 (新版冲刺不再检查耐力)
	//int lStamina = this->GetStamina();
	//switch(st)
	//{
	//case StateMachine::ST_PLAYER_JUMP1:
	//	if (iCurrentState == StateMachine::ST_PLAYER_JUMP1)	return RE_SKILL_SYSTEM_ERROR;
	//	if (iCurrentState == StateMachine::ST_PLAYER_JUMP2)	return RE_SKILL_SYSTEM_ERROR;
	//	if (iCurrentState == StateMachine::ST_PLAYER_JUMP3)	return RE_SKILL_SYSTEM_ERROR;
	//	if (lStamina < PLAYER_JUMP1_COST_STAMINA)
	//	{
	//		SendLogicInfoMessage(RE_SKILL_NOT_ENOUGH_STAMINA);
	//		return RE_SKILL_SYSTEM_ERROR;
	//	}
	//	break;
	//case StateMachine::ST_PLAYER_JUMP2:
	//	if (iCurrentState != StateMachine::ST_PLAYER_JUMP1)	return RE_SKILL_SYSTEM_ERROR;
	//	if (this->GetVerticalOffset() < g_settings.DS_JUMP2_LIMIT_HEIGHT)	return RE_SKILL_SYSTEM_ERROR;	//二段跳请求有效的最小高度
	//	if (lStamina < PLAYER_JUMP2_COST_STAMINA)
	//	{
	//		SendLogicInfoMessage(RE_SKILL_NOT_ENOUGH_STAMINA);
	//		return RE_SKILL_SYSTEM_ERROR;
	//	}
	//	break;
	//case StateMachine::ST_PLAYER_JUMP3:
	//	if (iCurrentState != StateMachine::ST_PLAYER_JUMP2)	return RE_SKILL_SYSTEM_ERROR;
	//	if (this->GetVerticalOffset() < 300)	return RE_SKILL_SYSTEM_ERROR; //三段跳请求有效的最小高度
	//	if (this->GetVerticalSpeed() > -10)		return RE_SKILL_SYSTEM_ERROR; //二段跳下落阶段才能三段跳
	//	if (this->GetVerticalSpeed() < -800)	return RE_SKILL_SYSTEM_ERROR;  //三段跳请求有效的最小掉落速度，即只能在刚开始下落的前一段时间内响应三段跳，主要是为了让三段跳准备阶段不能下落太多高度
	//	if (lStamina < PLAYER_JUMP3_COST_STAMINA)
	//	{
	//		SendLogicInfoMessage(RE_SKILL_NOT_ENOUGH_STAMINA);
	//		return RE_SKILL_SYSTEM_ERROR;
	//	}
	//	break;
	//case StateMachine::ST_PLAYER_JUMP_GUIDE:
	//	break;
	//default:				return RE_SKILL_SYSTEM_ERROR;
	//}
}
//------------------------------------------------------------------------
void	LC_ClientGamePlayer::RequestJump(StateMachine::StateType st,UT_Vec3Int & kTargetLocation, int jumpType)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;
	MG_Req_PlayerJump msgReqSkill;

	//发送网络请求
	msgReqSkill.m_lRequesterID		= this->GetID();
	msgReqSkill.m_usJumpStage		= st;
	msgReqSkill.m_usJumpType		= (uint8_t)jumpType;
	msgReqSkill.m_lCurrentLocationX = this->GetCurrentLocation().x;
	msgReqSkill.m_lCurrentLocationY = this->GetCurrentLocation().y;
	msgReqSkill.m_lCurrentLocationZ = this->GetCurrentLocation().z;
	msgReqSkill.m_iVerticalOffset	= Utility::Float2Int(this->GetVerticalOffset());
	msgReqSkill.m_lTargetLocationX  = kTargetLocation.x;
	msgReqSkill.m_lTargetLocationY  = kTargetLocation.y;
	msgReqSkill.m_lTargetLocationZ  = kTargetLocation.z;
	msgReqSkill.m_lSkillID			= GetGlobalSetting.iSprintSkillID;

	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_PLAYER_JUMP, &msgReqSkill);

	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	//GfxWriteLog(LOG_TAG_SKILL, LOG_SWITCH_SKILL_EVENT, "RequestJump, bCreateMsg=%d", bCreateMsg);
}
//------------------------------------------------------------------------
void LC_ClientGamePlayer::MoeveAfterJump()
{
	if ( GetjumpTarget() != UT_Vec3Int() )
	{
		PathToPoint(GetjumpTarget());
		PendingPathAction(GetJumpPathPendingAction().enmType, GetJumpPathPendingAction().iTargetCharTypeID);
		ClearJumpPathPendingAction();
		SetjumpTarget(UT_Vec3Int());
	}
}
//------------------------------------------------------------------------
void    LC_ClientGamePlayer::RequestTrade(unique_id_type lTradeID)
{
	//操作是否被屏蔽
	if(m_kActionForbid.IsActionForbid(LAFT_TRADE,LAF_TRADE_MAX_COUNT))
	{
		SendLogicInfoMessage(RE_ACTION_FORBID);
		return;
	}

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
	LC_ClientNetPlayer*	pkNetPlayer = (LC_ClientNetPlayer*)(pkNetPlayerManager->FindPlayer(lTradeID));
	if (pkNetPlayer)
	{
		Utility::UT_Vec3Int NetPlayerPos = pkNetPlayer->GetCurrentLocation();
		int iDistance = UT_MathBase::LineLengthXYInt(GetCurrentLocation(),NetPlayerPos);
		if(iDistance > MAX_TRADE_DISTANCE)
		{
			SendLogicInfoMessage(RE_ITEM_TRADE_NOT_IN_RANGE);
			return;
		}

		MG_REQ_RequestTrade reqMsg;
		reqMsg.m_lTradeCharID = GetCommanderUID();
		reqMsg.m_lTradeTargetCharID = lTradeID;
		TPSTR2STDSTR(GetCharName(), reqMsg.m_szCharName);
		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_ITEM_REQUEST_TRADE, &reqMsg);
		if(bCreateMsg)
		{
			m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
		}
	}
	else
	{
		SendLogicInfoMessage(RE_ITEM_TRADE_NOT_IN_RANGE);
	}
}
//------------------------------------------------------------------------
void LC_ClientGamePlayer::RltTrade(unique_id_type lTargetID)
{
	LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
	LC_ClientNetPlayer*	pkNetPlayer = (LC_ClientNetPlayer*)(pkNetPlayerManager->FindPlayer(lTargetID));

	if (pkNetPlayer)
	{	
		// 将请求交易的目标玩家加入临时列表
		//AddTempFriend(pkNetPlayer->GetInstance(), pkNetPlayer->GetCharName(), pkNetPlayer->GetCareerType(), pkNetPlayer->GetLevel());
		AddTempFriend(pkNetPlayer->GetInstance(), pkNetPlayer->GetCharName(), pkNetPlayer->GetCareerType(), pkNetPlayer->GetCommanderLevel());
	}
}

//------------------------------------------------------------------------
void    LC_ClientGamePlayer::RequestConfirmReplyTrade(unique_id_type lRequestID,int iReply)
{
	//操作是否被屏蔽
	if(m_kActionForbid.IsActionForbid(LAFT_TRADE,LAF_TRADE_MAX_COUNT))
	{
		SendLogicInfoMessage(RE_ACTION_FORBID);
		return;
	}

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
	LC_ClientNetPlayer*	pkNetPlayer = (LC_ClientNetPlayer*)(pkNetPlayerManager->FindPlayer(lRequestID));
	if (pkNetPlayer)
	{
		Utility::UT_Vec3Int NetPlayerPos = pkNetPlayer->GetCurrentLocation();
		int iDistance = UT_MathBase::LineLengthXYInt(GetCurrentLocation(),NetPlayerPos);
		if(iDistance > MAX_TRADE_DISTANCE)
		{
			iReply = RE_ITEM_TRADE_NOT_IN_RANGE;
		}
		else if (GetDead())
		{
			iReply = RE_ITEM_TRADE_DEAD_CANT;
		}

		if (iReply == RTS_ACCEPT)
		{
			//AddTempFriend( lRequestID, pkNetPlayer->GetCharName(), pkNetPlayer->GetCareerType(), pkNetPlayer->GetLevel() );
			AddTempFriend( lRequestID, pkNetPlayer->GetCharName(), pkNetPlayer->GetCareerType(), pkNetPlayer->GetCommanderLevel() );
		}
	}
	else
	{
		iReply = RE_ITEM_TRADE_NOT_IN_RANGE;
	}

	//为了防止网络延迟可能造成的UI表现和服务器不一致，而造成BUG所以客户
	//端先删除然后服务器再删除
	GetTradeManager()->RemoveFormRequestTradeID(lRequestID);

	MG_REQ_ReplyTrade Msg;
	Msg.m_lTradeTargetCharID = lRequestID;
	Msg.m_iReqlyResult     = iReply;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_ITEM_REPLY_TRADE, &Msg);

	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
//------------------------------------------------------------------------
void   LC_ClientGamePlayer::RequestAddItemToTradePack(int iBackPackIndex,int iTradePackIndex,int iItemType /* = 0*/)
{
	//操作是否被屏蔽
	if(m_kActionForbid.IsActionForbid(LAFT_TRADE,LAF_TRADE_MAX_COUNT))
	{
		SendLogicInfoMessage(RE_ACTION_FORBID);
		return;
	}

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	int iResult = RE_SUCCESS;

	if (m_kTradeManager.GetTradeState() != PTS_HAVE_TRADE)
	{
		iResult = RE_ITEM_TRADE_NOT_HAVE_TRADE;
	}
	else if (m_kTradeManager.GetMyTradeLock())
	{
		iResult = RE_ITEM_TRADE_LOCKED;
	}
	else
	{
		if (iTradePackIndex < 0 )
		{
			iTradePackIndex = m_kTradeManager.FindFirstInvalidEntryIndexInTradePack();
			if (iTradePackIndex < 0)
			{
				iResult = RE_ITEM_TRADE_PACK_FULL;
				GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"交易背包已满:[%d]",iBackPackIndex);
			}
		}

		if (iTradePackIndex >= 0)
		{
			LC_BackPackEntry* pkEntry = GetPackAsset()->GetBackPackEntry(iItemType,iBackPackIndex);
			LC_BackPackEntry* pkTradeEntry = m_kTradeManager.GetItemEntryInMyTradePack(iTradePackIndex);
			if (!pkEntry||!pkTradeEntry)
			{
				iResult = RE_ITEM_TRADE_CANT_UNVALIDE;
				GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"无效背包位Entry:[%d]",iBackPackIndex);
			}
			else if (!pkEntry->GetValid())
			{
				iResult = RE_ITEM_TRADE_CANT_UNVALIDE;
				GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"无效背包位Entry:[%d]",iBackPackIndex);
			}
			else if (pkTradeEntry->GetValid())
			{
				iResult = RE_ITEM_TRADE_ENTRY_ALREADY;
				GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"交易位已经存在交易物品:[%d]",iTradePackIndex);
			} 
			else if (pkEntry->GetBind())
			{
				iResult = RE_ITEM_TRADE_CANT_BINDE;
				GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"物品已经绑定不能交易:[%d]",pkEntry->GetTypeID());
			}
			else if (pkEntry->GetLocked())
			{
				iResult = RE_ITEM_TRADE_ITEM_LOCKED;
				GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"物品已经锁定不能交易:[%d]",pkEntry->GetTypeID());
			}
			else
			{
				MG_REQ_AddItemToTradePack reqMsg;
				reqMsg.m_lTradeTargetCharID     = m_kTradeManager.GetTradeTargetID();
				reqMsg.m_iBackPackIndex     = iBackPackIndex;
				reqMsg.m_usItemType         = iItemType;
				reqMsg.m_iTradePackIndex    = iTradePackIndex;
				reqMsg.m_lTradeItemID       = pkEntry->GetTypeID();
				bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_ITEM_ADD_ITEM_IN_TRADE, &reqMsg);

				if(bCreateMsg)
					m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
			}
		}
	}


	if (iResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(iResult);
	}

}
//------------------------------------------------------------------------
void   LC_ClientGamePlayer::RequestRemoveItemFormTradePack(int iTradePackIndex)
{
	//操作是否被屏蔽
	if(m_kActionForbid.IsActionForbid(LAFT_TRADE,LAF_TRADE_MAX_COUNT))
	{
		SendLogicInfoMessage(RE_ACTION_FORBID);
		return;
	}

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	int iResult = RE_SUCCESS;

	if (m_kTradeManager.GetTradeState() != PTS_HAVE_TRADE)
	{
		iResult = RE_ITEM_TRADE_NOT_HAVE_TRADE;
	}
	else if (m_kTradeManager.GetMyTradeLock())
	{
		iResult = RE_ITEM_TRADE_LOCKED;
	}
	else
	{
		LC_BackPackEntry* pkEntry = m_kTradeManager.GetItemEntryInMyTradePack(iTradePackIndex);
		if (!pkEntry)
		{
			iResult = RE_ITEM_TRADE_CANT_UNVALIDE;
			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"无效背包位Entry:[%d]",iTradePackIndex);
		}
		else if (!pkEntry->GetValid())
		{
			iResult = RE_ITEM_TRADE_CANT_UNVALIDE;
			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"无效背包位Entry:[%d]",iTradePackIndex);
		}
		else 
		{
			MG_REQ_RemoveItemFromTradePack reqMsg;
			reqMsg.m_lTradeTargetCharID     = m_kTradeManager.GetTradeTargetID();
			//reqMsg.m_iBackPackIndex     = iBackPackIndex;
			reqMsg.m_iTradePackIndex    = iTradePackIndex;
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_ITEM_REMOVE_ITEM_IN_TRADE, &reqMsg);

			if(bCreateMsg)
				m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
		}
	}

	if (iResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(iResult);
	}
}
//------------------------------------------------------------------------
void   LC_ClientGamePlayer::RequestUpdateTradeCash(int64_t llCash, int32_t iCashType)
{
	//操作是否被屏蔽
	if(m_kActionForbid.IsActionForbid(LAFT_TRADE,LAF_TRADE_MAX_COUNT))
	{
		SendLogicInfoMessage(RE_ACTION_FORBID);
		return;
	}

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	int iResult = RE_SUCCESS;

	if (m_kTradeManager.GetTradeState() != PTS_HAVE_TRADE)
	{
		iResult = RE_ITEM_TRADE_NOT_HAVE_TRADE;
	}
	else if (m_kTradeManager.GetMyTradeLock())
	{
		iResult = RE_ITEM_TRADE_LOCKED;
	}
	
	if ( !IsCashEnough(llCash, iCashType))
	{
		iResult = RE_ITEM_TRADE_CASH_NOT_ENOUGH;
	}
	else
	{
		MG_REQ_UpdateTradeCash reqMsg;
		reqMsg.m_lTradeTargetCharID     = m_kTradeManager.GetTradeTargetID();
		reqMsg.m_llCash				= llCash;
		reqMsg.m_iCashType			= iCashType;
		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_ITEM_UPDATE_CASH_IN_TRADE, &reqMsg);

		if(bCreateMsg)
			m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}

	if (iResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(iResult);
	}
}
//------------------------------------------------------------------------
void   LC_ClientGamePlayer::RequestLockTradeData()
{
	//操作是否被屏蔽
	if(m_kActionForbid.IsActionForbid(LAFT_TRADE,LAF_TRADE_MAX_COUNT))
	{
		SendLogicInfoMessage(RE_ACTION_FORBID);
		return;
	}

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	int iResult = RE_SUCCESS;

	if (m_kTradeManager.GetTradeState() != PTS_HAVE_TRADE)
	{
		iResult = RE_ITEM_TRADE_NOT_HAVE_TRADE;
	}
	else if (m_kTradeManager.GetMyTradeLock())
	{
		iResult = RE_ITEM_TRADE_LOCKED;
	}
	else
	{
		MG_REQ_LockTrade reqMsg;
		reqMsg.m_lTradeTargetCharID     = m_kTradeManager.GetTradeTargetID();
		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_ITEM_TRADE_LOCK, &reqMsg);

		if(bCreateMsg)
			m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	}

	if (iResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(iResult);
	}
}
//------------------------------------------------------------------------
void   LC_ClientGamePlayer::RequestUnLockTradeData()
{
	//操作是否被屏蔽
	if(m_kActionForbid.IsActionForbid(LAFT_TRADE,LAF_TRADE_MAX_COUNT))
	{
		SendLogicInfoMessage(RE_ACTION_FORBID);
		return;
	}

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	int iResult = RE_SUCCESS;

	if (m_kTradeManager.GetTradeState() == PTS_NO_TRADE)
	{
		iResult = RE_ITEM_TRADE_NOT_HAVE_TRADE;
	}
	else if (!m_kTradeManager.GetMyTradeLock())
	{
		iResult = RE_ITEM_TRADE_UNLOCKED;
	}
	else
	{
		MG_REQ_UnLockTrade reqMsg;
		reqMsg.m_lTradeTargetCharID     = m_kTradeManager.GetTradeTargetID();
		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_ITEM_TRADE_UNLOCK, &reqMsg);

		if(bCreateMsg)
			m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	}

	if (iResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(iResult);
	}
}
//------------------------------------------------------------------------
void   LC_ClientGamePlayer::RequestInterruptTrade()
{
	//操作是否被屏蔽
	if(m_kActionForbid.IsActionForbid(LAFT_TRADE,LAF_TRADE_MAX_COUNT))
	{
		SendLogicInfoMessage(RE_ACTION_FORBID);
		return;
	}

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	int iResult = RE_SUCCESS;

	if (m_kTradeManager.GetTradeState() == PTS_NO_TRADE)
	{
		iResult = RE_ITEM_TRADE_NOT_HAVE_TRADE;
	}
	else
	{
		MG_REQ_InterruptTrade reqMsg;
		reqMsg.m_lTradeTargetCharID     = m_kTradeManager.GetTradeTargetID();
		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_ITEM_TRADE_INTERRUPT, &reqMsg);

		if(bCreateMsg)
			m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
		//防止重复发送中断消息
		m_kTradeManager.SetTradeState(PTS_NO_TRADE);
	}

	if (iResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(iResult);
	}
}
//------------------------------------------------------------------------
void   LC_ClientGamePlayer::RequestFinishTrade()
{
	//操作是否被屏蔽
	if(m_kActionForbid.IsActionForbid(LAFT_TRADE,LAF_TRADE_MAX_COUNT))
	{
		SendLogicInfoMessage(RE_ACTION_FORBID);
		return;
	}

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	int iResult = RE_SUCCESS;

	if (m_kTradeManager.GetTradeState() != PTS_HAVE_TRADE)
	{
		iResult = RE_ITEM_TRADE_NOT_HAVE_TRADE;
	}
	else if (!m_kTradeManager.GetMyTradeLock())
	{
		iResult = RE_ITEM_TRADE_UNLOCKED;
	}
	else
	{
		MG_REQ_FinishTrade reqMsg;
		reqMsg.m_lTradeTargetCharID     = m_kTradeManager.GetTradeTargetID();
		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_ITEM_TRADE_FINISH, &reqMsg);

		if(bCreateMsg)
			m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	}

	if (iResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(iResult);
	}
}
//------------------------------------------------------------------------
void LC_ClientGamePlayer::SetIsNeedLockTradeTarget( bool bISNeedLock )
{
	m_bNeedLockTradeTarget = bISNeedLock;
	if (bISNeedLock)
		SendLogicInfoMessage(RE_ITEM_TRADE_SELECT_TARGET);
}
//------------------------------------------------------------------------
int LC_ClientGamePlayer::GetLastTradeRequesterID()
{
	unique_id_type lID = m_kTradeManager.GetLastRequesterID();

	LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
	LC_ClientNetPlayer* pkNetPlayer = (LC_ClientNetPlayer*)(pkNetPlayerManager->FindPlayer(lID));
	if (pkNetPlayer == NULL)
	{
		return 0;
	}

	return lID;
}
//------------------------------------------------------------------------
StringType LC_ClientGamePlayer::GetCharName()
{
	StringType kOutputName = m_kCharName_gb;
	GUISys::getRichTextParse->FilterForbiddenText(kOutputName, kOutputName, TT_TextFilter::TFT_CHAR_NAME);
	return kOutputName;
}
void LC_ClientGamePlayer::SetProductionLevel(int lProductionLevel)
{
}
//------------------------------------------------------------------------
void LC_ClientGamePlayer::SetCurrentMastery(int lCurrentMastery)
{
}
//------------------------------------------------------------------------
void LC_ClientGamePlayer::SetCurrentEnergy(int lCurrentEnergy)
{
}
//------------------------------------------------------------------------
bool LC_ClientGamePlayer::Respawn( attr_value_type lHP,attr_value_type lMP )
{
	LC_PlayerBase::Respawn(lHP,lMP);
	//SendMessage( CreateMsg_UE_DELTA_HP( GetID(), lHP) );

	StopMove();
	ClearMoveStride();
	ClearPathFinding();

	GD_ClientActorModel* pkVisibleData = IM_Manager::GetSingleton().GetActor(GetID());
	if(pkVisibleData)
	{
		pkVisibleData->AttachFx(GD_ClientActorModel::FDT_GROUND, 0, false, RES_FX_RESPAWN);
	}

	return true;
}

//******************************
bool LC_ClientGamePlayer::Respawn(attr_value_type lHP,int lMP)
{
	if (LC_ActorBase::Respawn(lHP, lMP))
	{
		SetNextState(ST_PLAYER_STAND);
		return true;
	}
	return false;
}

//------------------------------------------------------------------------
void LC_ClientGamePlayer::ChangeMap(int iMapLogicID,int lMapResID,int lMapLogicType,const Utility::UT_Vec3Int& rkLocation, bool bAsBuddy)
{
	if (d_MapLoadInfo.isLoading())
	{
		d_MapLoadInfo.set(iMapLogicID, lMapResID, lMapLogicType, rkLocation);
		return;
	}

	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetGameSessionGroup();
	UI_UIManager *pkUIMessage = UI_UIManager::GetSingletonPtr();
	//if(pkUIMessage)
		pkUIMessage->SetWorldMapID(lMapResID);
	//发送更换地图消息到逻辑端
	pkUIMessage->SetLoadingProcess(false);
	//从loading切到loading更自然
	if(pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType() != ST_GAME_STAGE_LOADING)
	{
		pkClientSessionGroup->m_pkGameStageMachine->SetNextState(ST_GAME_STAGE_LOADING);
		pkClientSessionGroup->m_pkGameStageMachine->Update(0.0f,0.0f);
	}

	//先设置位置，再下载地图，否则在服务器端被认为是新场景，旧位置，可能导致不可预料的问题
	SetCurrentLocation(rkLocation);
	SetTargetLocation(rkLocation);
	SetMoveLineTargetLocation(rkLocation);

	LC_ClientWorldManager* pkWorldManager = CLIENT_GET_WORLD_MANAGER();
	pkWorldManager->RemoveObjectFromMap(this);
	CF_WorldMapList* pkCSVWorldMapList = CF_WorldMapList::GetSingletonPtr();
	CF_WorldMapList::DataEntry* pkData = pkCSVWorldMapList->GetEntryPtr(lMapResID);
	if (pkData != NULL)
	{
		const StringType& rkWorldMapName = pkData->_kFilePath;
		pkWorldManager->SetCurrentMapID(rkWorldMapName, iMapLogicID,lMapResID,lMapLogicType);
		pkWorldManager->AddObjectToMap(pkWorldManager->GetCurrentMapID(),this);
	}

	//判断地图资源是否有效
	if (!TryReadMap(lMapResID))
	{
		d_MapLoadInfo.beginLoad();
		d_MapLoadInfo.set(iMapLogicID, lMapResID, lMapLogicType, rkLocation);
		return;
	}

	AS_ClientSoundManager::GetSingletonPtr()->ClearBossFightInfo();

	if ( OnRltTransferChangePkMode(GetMapResID(), lMapResID) )
		RequestModifyPKSettingForce();

	ChangeMapImp( iMapLogicID, lMapResID, lMapLogicType, rkLocation);
}

void LC_ClientGamePlayer::ChangeMapImp(int iMapLogicID,int lMapResID,int lMapLogicType,const Utility::UT_Vec3Int& rkLocation)
{
	LC_ClientWorldManager* pkWorldManager = CLIENT_GET_WORLD_MANAGER();

	int lCurrentMapLogicID = GetMapLogicID();

	//NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	//NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetGameSessionGroup();

	UI_UIManager *pkUIMessage = UI_UIManager::GetSingletonPtr();
	//if(pkUIMessage)
	//	pkUIMessage->SetWorldMapID(lMapResID);
	//发送更换地图消息到逻辑端
	//pkClientSessionGroup->m_pkGameStageMachine->SetNextState(ST_GAME_STAGE_LOADING);
	//pkClientSessionGroup->m_pkGameStageMachine->Update(0.0f,0.0f);

	int iPreMapResID = GetMapResID();
	/*if(lMapLogicType == MT_NEWBEE)
	{
		if (lMapLogicType != GetMapLogicType() || iPreMapResID != lMapResID)
			SendLogicInfoMessage(RE_ENTER_SUB_SERVER,GetNewPlayerLevel());
	}*/

	if ( iPreMapResID != lMapResID )
	{
		CA_CameraManager::GetSingleton().ResetAllCamera();
	}

	SetMapResID(lMapResID);
	SetMapLogicType(lMapLogicType);
	GetMovePath().Clear();
	SetMoveSpeed(0);
	SetVisibleMoveSpeed(0);
	SetMoveStride(false);
	SetWatchTargetID(0);
	SetLockedTargetID(0);
	FreezeLocked(false);
	SetPreLockedTargetID(0);
	SetLButtonPickActorID(0);
	ClearPendingSkill();
	_clearEyeAvatar();
	DestroySuiteNPC();
	this->_clearUnlockRequestExecuteSkill();

	GfxWriteLog( LOG_TAG_CLIENT_CRITICAL, LOG_SWITCH_CLIENT_CRITICAL , 
		"enter change map func");
	/*if (iMapLogicID != lCurrentMapLogicID)
	{*/
	CF_WorldMapList* pkCSVWorldMapList = CF_WorldMapList::GetSingletonPtr();

	CF_WorldMapList::DataEntry* pkData = pkCSVWorldMapList->GetEntryPtr(lMapResID);
	if ( pkData == NULL )
	{
		GfxWriteLog( LOG_TAG_CLIENT_CRITICAL, LOG_SWITCH_CLIENT_CRITICAL , 
			"change map error can't find map %d ",lMapResID );
		MessageBoxA2W_IDS( IDS_GAMERES_CORRUPTED , L"Warning", MB_OK|MB_ICONWARNING);
		T_ASSERT_CRITICAL_S(false);
	}
	CF_MapEffectConfig::DataEntry* pkEffectCfg = CF_MapEffectConfig::GetSingletonPtr()->GetEntryPtr( lMapResID );
	StringType strDecoration = "";
	if ( pkEffectCfg )
	{
		strDecoration = pkEffectCfg->_kDecoration;
	}
	const StringType& rkWorldMapName = pkData->_kFilePath;

	SE_SceneManager* pkSceneManager = SE_SceneManager::GetSingletonPtr();
	if (pkSceneManager != NULL)
	{

		{
			//卸载当前的地图
			if( pkSceneManager->UnLoadWorldMap() )
			{
				GfxWriteLog( LOG_TAG_CLIENT_CRITICAL, LOG_SWITCH_CLIENT_CRITICAL , 
					"change map unload current map %d",
					pkSceneManager->GetCurrentMapID() );
			}
			else
			{
				GfxWriteLog( LOG_TAG_CLIENT_CRITICAL, LOG_SWITCH_CLIENT_CRITICAL , 
					"change map unload current map %d failed",
					pkSceneManager->GetCurrentMapID() );
			}
		}


#ifdef _MAP_VIEWER_BUILD
		UT_ConfigFile kConfig;
		kConfig.Load(GfxMakeFullPath(CLIENT_PREVIEW_MAP_CONFIG_FILE));
		StringType kMapName = kConfig.GetSetting(CLIENT_MAP_CONFIG_MAP_NAME, 
			CLIENT_MAP_CONFIG_SECTION_NAME, 
			OLD_RES_MAP_SHUXIU);
		StringType kDecorationName = kConfig.GetSetting(CLIENT_MAP_CONFIG_DECORATION_NAME,
			CLIENT_MAP_CONFIG_SECTION_NAME, "");
		SE_SceneManager::GetSingletonPtr()->LoadWorldMap( kMapName , 
			kDecorationName , lMapResID , 
			(int)g_settings.BL_LOADING_WIDTH  );
#else
		//加载当前的地图
		if( pkSceneManager->LoadWorldMap(rkWorldMapName , strDecoration ,
			lMapResID , (int)g_settings.BL_LOADING_WIDTH ) )
		{
			GfxWriteLog( LOG_TAG_CLIENT_CRITICAL, LOG_SWITCH_CLIENT_CRITICAL , 
				"change map load current map %d success",lMapResID );
		}
#endif
		PF_PathManager* pkPathManager = PF_PathManager::GetSingletonPtr();
		if(pkPathManager&&pkData&&pkData->_kMapRadius > 0)
		{
			int iMaxX = pkData->_kCentralPoint.x+pkData->_kMapRadius;
			int iMaxY = pkData->_kCentralPoint.y+pkData->_kMapRadius;
			UT_Vec2Int kMax(iMaxX,iMaxY);
			int iMinX = pkData->_kCentralPoint.x-pkData->_kMapRadius;
			int iMinY = pkData->_kCentralPoint.y-pkData->_kMapRadius;
			UT_Vec2Int kMin(iMinX,iMinY);
			pkPathManager->SetGlobalPathFindingRangeLimits(kMax,kMin);

			FX_ClientFxManager* pkFXManager = FX_ClientFxManager::GetSingletonPtr();

			Utility::UT_Vec3Int vecFXPos[4];
			vecFXPos[0] = pkData->_kCentralPoint - Utility::UT_Vec3Int(pkData->_kMapRadius,0,0);
			vecFXPos[1] = pkData->_kCentralPoint - Utility::UT_Vec3Int(-pkData->_kMapRadius,0,0);
			vecFXPos[2] = pkData->_kCentralPoint - Utility::UT_Vec3Int(0,pkData->_kMapRadius,0);
			vecFXPos[3] = pkData->_kCentralPoint - Utility::UT_Vec3Int(0,-pkData->_kMapRadius,0);

			Utility::UT_Vec3Int kCentralPoint = pkData->_kCentralPoint;

			int Count = Float2Int((pkData->_kMapRadius*2)/1600);
			Count = Count*4;
			bool bShowFxWall = pkSceneManager->GetFxWallShow();
			int iFxID = 0;
			for (int i = 0; i < MAX_FX_WALL_NUM ; ++i)
			{
				if (i >= Count)
					break;

				if(bShowFxWall)
					iFxID =	pkSceneManager->GetFxWallID(i);
				else
					iFxID =	pkFXManager->CreateFx(RES_FX_16M_AIR_WALL,false);

				if (iFxID >= 0)
				{			
					int iSide = i%4; 
					NiPoint3 kFXPoint(UT_EngineMath::ConvertVec3IntToNiPoint3(vecFXPos[iSide]));
					NiPoint3 kDir(UT_EngineMath::ConvertVec3IntToNiPoint3(kCentralPoint - vecFXPos[iSide]));

					int iXBase = 0;
					int iYBase = 0;
					if (i >= 4)
					{
						int iFlag = 1;
						int iStep = Float2Int(i/4);
						if ( iStep%2 == 0)
						{
							iFlag = -1;
							iStep = Float2Int(iStep/2);
						}
						else
						{
							iStep = Float2Int(iStep/2) +1;
						}

						if (iSide == 0)
							iYBase = iFlag*iStep;
						else if (iSide == 1)
							iYBase = iFlag*iStep;
						else if (iSide == 2)
							iXBase = iFlag*iStep;
						else if (iSide == 3)
							iXBase = iFlag*iStep;
					}

					kFXPoint = kFXPoint + NiPoint3(iXBase*1600,iYBase*1600,0);
					pkFXManager->BindFxToLocation( iFxID, kFXPoint,&kDir);

					if(!bShowFxWall)
						pkFXManager->PlayFx(iFxID, GET_PROCESS_TIME);
				}

				pkSceneManager->SetFxWallID(i,iFxID);

			}

			pkSceneManager->SetFxWallShow(true);
		}
		else
		{
			pkPathManager->ClearGlobalPathFindingRangeLimits();
			FX_ClientFxManager* pkFXManager = FX_ClientFxManager::GetSingletonPtr();
			if (pkFXManager)
			{
				for (int i = 0; i < MAX_FX_WALL_NUM ; ++i)
				{
					int iFxID =	pkSceneManager->GetFxWallID(i);
					if (iFxID > 0)		
						pkFXManager->ReleaseFx(iFxID);

					pkSceneManager->SetFxWallID(i,0);
				}
			}

			pkSceneManager->SetFxWallShow(false);

		}

		//< duowan 对接
		report2duowan_async_getreportable(lMapResID, GetCharName().c_str());

		SendMessage( CreateMsg_UI_MAP_CHANGE(iMapLogicID, lMapResID, lMapLogicType, iPreMapResID) );
		if (pkUIMessage)
			SendMessage( CreateMsg_UI_MAP_CHANGE(iMapLogicID, lMapResID, lMapLogicType, iPreMapResID), pkUIMessage );

		//更新玩家挂机信息(由于进入副本设置挂机模式会纪录挂机位置，但是出副本并没有设置挂机模式，没有更新挂机位置，导致玩家预设挂机位置错误，找不到目标)
		{
			const UT_Vec3Int& sd = GetCurrentLocation();
			SetHookLocation(sd);
		}


		//发送网络消息---记录当前已经到达的地图区域
		LC_ClientMapTranscriptionManager* pkMapTransMgr = LC_ClientMapTranscriptionManager::GetSingletonPtr();
		if (pkMapTransMgr)
		{
			MG_PACKET_ENCODE_HANDLER hMsgHandler;
			bool bCreateMsg = false;
			MG_Req_Record_Transport reqMsg;
			reqMsg.m_transportId = pkMapTransMgr->GetMapCategoryTypeByMapResID(lMapResID);
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_RECORD_TRANSPORT, &reqMsg);
			if(bCreateMsg)
				m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
		}

		pkWorldManager->RemoveObjectFromMap(this);
		pkWorldManager->SetCurrentMapID(rkWorldMapName, iMapLogicID,lMapResID,lMapLogicType);

		SetCurrentLocation(rkLocation);
		SetTargetLocation(rkLocation);
		SetMoveLineTargetLocation(rkLocation);

		pkWorldManager->AddObjectToMap(pkWorldManager->GetCurrentMapID(),this);

		LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
		if(pkNPCManager)
		{
			CF_WorldMapList* pkTranscriptionRule = CF_WorldMapList::GetSingletonPtr();
			CF_WorldMapList::DataEntry* pkData=pkTranscriptionRule->GetEntryPtr(lMapResID);

			if(pkData!=NULL && pkData->_iAutoFitLevel == 1)
				pkNPCManager->SetAutoFitFlag(true);
			else
				pkNPCManager->SetAutoFitFlag(false);
		}

		if (LC_Helper::CheckTranscriptionMapIDValidity(lMapResID))
		{
			LC_ClientTranscriptionInfoManager* pkTranscriptionInfoManager = CLIENT_GET_TRANSCRIPTION_INFO_MANAGER();
			if(pkTranscriptionInfoManager)
			{
				pkTranscriptionInfoManager->AddTranscriptionRevisionID();
				pkTranscriptionInfoManager->SetCurrentTranscriptionID(lMapResID);
			}
		}
	}	

	// 地图加载后重新设置玩家的假阴影
	GD_ClientActorModel* pkVisibleData = IM_Manager::GetSingleton().GetActor(GetID());
	if (pkVisibleData != NULL)
	{
		pkVisibleData->EnableFakeShadow(true);
	}
	else
	{
		m_bPendingToSetVisibleDataEnableFakeShadow = true;
	}

	LC_ClientGameStoryManager* pkGameStoryManager = CLIENT_GET_GAMESTORY_MANAGER();
	if(pkGameStoryManager)
		pkGameStoryManager->OnMapChanged(lMapResID);

	LC_ClientPlaceHolderManager *pkPlaceHolderManager = CLIENT_GET_PLACEHOLDER_MANAGER();
	if (pkPlaceHolderManager)
	{
		pkPlaceHolderManager->SetRaidSettle(false);
		pkPlaceHolderManager->ReSetRaidSettleDelayTime();
	}

	/*if (lMapLogicType == MT_PRISON)
	{
	LC_ClientArenaManager* pkArenaManager = LC_ClientArenaManager::GetSingletonPtr();
	if(pkArenaManager)
	{
	pkArenaManager->RequestCancleSignUpArenaInPrison();
	}
	}*/

	//清除当前地图的传送门
	LC_PortalAssert* pkPortalAssert = GetPortalAsset();
	LC_PortalEntry* pkPortalEntry = pkPortalAssert->GetMapPortal(GetMapResID());
	if (pkPortalEntry)
		pkPortalEntry->m_lNPCID = 0;

	//清除netplayer
	LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
	if (pkNetPlayerManager)
		pkNetPlayerManager->DestroyAllPlayer();

	//清除怪物破碎模型
	BM_Manager* pkBMManager = BM_Manager::GetSingletonPtr();
	if (pkBMManager)
		pkBMManager->ReSet();

	//清除npc
	LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
	if (pkNPCManager )	
		pkNPCManager->DestroyAllGfxWallNPC();

	if ((GetMapLogicType() != MT_GUILDWAR && GetGuildWarType() != WarType_DomainWar)
		|| (!SC_ClientScriptInterface::GetSingleton().GetSkillInterface()->IsDomainMap(GetMapResID()) && GetGuildWarType() == WarType_DomainWar))
	{
		SetPowerGuildID(-1);
		SetGuildWarType(WarType_None);
	}

	LC_ClientCombatRecordManager::GetSingleton().Reset();
	GetRaidHookAsset().ResetRaidStageHookAssist();
	FX_ClientFxManager::GetSingleton().ClearDischarGfx();
	ClearJumpPathPendingAction();
	_resetTriggerInfo();
}
//------------------------------------------------------------------------
int LC_ClientGamePlayer::GetCurrentEquipItem( int iEquipType )
{
	return 0;
}
//-------------------------------------------------------
bool LC_ClientGamePlayer::IsEquipVehicleWeapon()
{
	bool flag = false;
	//现在骑战有10阶，要每一阶都遍历过去才可以，不然可能突然他，升级的骑战，导致上次记录的骑战兵器错误
	for(int i = 0 ; i <= 14 ; i++)
	{
		int riderWeapon = LC_Helper::GetRideWeaponItemID(i);	//骑战武器取值范围[0~n)，0表示无效，n表示n阶骑战武器
		if ( riderWeapon != 0 )
		{
			flag = (m_lastWeaponItemID == riderWeapon);
			if( flag)
				break;
		}
	}
	return flag;
}
//-------------------------------------------------------
bool LC_ClientGamePlayer::IsArtifactWeapon()
{
	return m_lastWeaponItemID != 0 && m_lastWeaponItemID == GetStateExtendData(GOD_STATE_ID, GOD_STATE_WEAPONTYPE); 
}
//-------------------------------------------------------
void 	LC_ClientGamePlayer::HideBeforeSuitApplied()
{
	GD_ClientActorModel* pkVisibleData = IM_Manager::GetSingleton().GetActor(GetID());
	if(pkVisibleData)
	{
		pkVisibleData->HideBeforeSuitApplied();
	}
}

void	LC_ClientGamePlayer::ShowAfterSuitApplied()
{
	GD_ClientActorModel* pkVisibleData = IM_Manager::GetSingleton().GetActor(GetID());
	if(pkVisibleData)
	{
		pkVisibleData->ShowAfterSuitApplied();
	}
}

bool   LC_ClientGamePlayer::ChangeAvatar(int lOldEquipID,int lNewEquipID)
{
	if(GetShapeShiftFlag()&&!IS_CHANGE_SKILL_NPC_TYPE_ID(GetShapeShiftCharTypeID()))
		return false;
	
	if (lOldEquipID == lNewEquipID )
	{
		return false;
	}

	GfxWriteLog(LOG_SYSTEM_FILELINE, LOG_SWITCH_SYSTEM, 
		"Logic game-player [%s] ChangeAvatar %d -> %d", Int642String(GetID()).c_str(), lOldEquipID, lNewEquipID);

	float fCurrentTime = GET_PROCESS_TIME;

	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	if (!IS_ITEM_ID(lNewEquipID))
	{
		LC_ClientItem* pkItem = (LC_ClientItem*)pkItemFactory->RequestItem(lOldEquipID);
		if(!pkItem)
		{
			//GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"试图卸载无效物件, %d, %d", lOldEquipID, lNewEquipID);
			return false;
		}

		GD_ClientActorModel* pkVisibleData = IM_Manager::GetSingleton().GetActor(GetID());
		if(pkVisibleData)
		{
			pkItem->UnoperationAvatarFunc(GetCharType(), pkVisibleData,NULL,fCurrentTime);
			return true;
		}
	}
	else
	{
		LC_ClientItem*  pkItem = (LC_ClientItem* )pkItemFactory->RequestItem(lNewEquipID);
		if(!pkItem)
		{
			//GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"试图装备无效物件, %d", lNewEquipID);
			return false;
		}

		GD_ClientActorModel* pkVisibleData = IM_Manager::GetSingleton().GetActor(GetID());
		if(pkVisibleData)
		{
			GfxWriteLog(LOG_SYSTEM_FILELINE, LOG_SWITCH_SYSTEM, 
				"Logic game-player [%s] item OperationAvatarFunc %d -> %d", Int642String(GetID()).c_str(), lOldEquipID, lNewEquipID);

			pkItem->OperationAvatarFunc(GetCharType(), pkVisibleData,NULL,fCurrentTime);
			return true;
		}
	}
	return true;
}
//-------------------------------------------------------
void LC_ClientGamePlayer::ResetLastEquipData()
{
	for(int i = 0 ; i < ET_MAX_COUNT; i++)
	{
		m_currentSpecialEquipt[i] = 0;
	}
}
//-------------------------------------------------------
void LC_ClientGamePlayer::SetLastEquiptID(int equipID, int equiptType)
{
	if( equiptType >= 0 && equiptType < ET_MAX_COUNT)
		m_currentSpecialEquipt[equiptType] = equipID;
}
//-------------------------------------------------------
int LC_ClientGamePlayer::GetLastEquiptID(int equiptType)
{
	if( equiptType >= 0 && equiptType < ET_MAX_COUNT)
		return m_currentSpecialEquipt[equiptType];
	else
		return 0;
}
//-------------------------------------------------------
void LC_ClientGamePlayer::UpdateSpecialEquipData()
{
	//long lOldHeadAvatarItemID	= this->GetLastEquiptID(ET_HEAD);
	//long lOldBodyAvatarItemID	= this->GetLastEquiptID(ET_BODY);
	//long lOldFootAvatarItemID	= this->GetLastEquiptID(ET_FOOT);
	//bool change = false;
	//change = this->ChangeAvatar(lOldHeadAvatarItemID,this->GetCurrentEquipItem(ET_HEAD));
	//if (change)
	//	SetLastEquiptID(this->GetCurrentEquipItem(ET_HEAD),ET_HEAD);
	//change = this->ChangeAvatar(lOldBodyAvatarItemID,this->GetCurrentEquipItem(ET_BODY));
	//if (change)
	//	SetLastEquiptID(this->GetCurrentEquipItem(ET_BODY),ET_BODY);
	//change = this->ChangeAvatar(lOldFootAvatarItemID,this->GetCurrentEquipItem(ET_FOOT));
	//if (change)
	//	SetLastEquiptID(this->GetCurrentEquipItem(ET_FOOT),ET_FOOT);
}
//-------------------------------------------------------
bool   LC_ClientGamePlayer::_updateWeaponAvatar()
{
	static int lastNoVehicleWeapon = 0;
	static int lastVehicleWeapon	= 0;
	int curWeaponItemID = m_lastWeaponItemID;
	if (this->GetRiderCharType() > 0)	//上马使用骑战武器
	{
		//int lNewWeaponLevel = this->GetVehicleFightingLevel();
		//int vehicleWeapon = LC_Helper::GetRideWeaponItemID(lNewWeaponLevel);	//骑战武器取值范围[0~n)，0表示无效，n表示n阶骑战武器

		//if(IS_SKILL_TYPE(m_iCurrentState))
		//{
		//	SK_SkillExecute* pkSkill = (SK_SkillExecute*)GetCurrentState();
		//	if (pkSkill != NULL)
		//	{
		//		int u = pkSkill->GetProperty_WeaponUsage();
		//		if(u & SWU_CHANGE_VF)
		//		{
		//			curWeaponItemID = vehicleWeapon;
		//			lastVehicleWeapon = curWeaponItemID;
		//		}
		//		else if(u & SWU_CHANGE_MENPAI)
		//		{
		//			curWeaponItemID = this->GetCurrentEquipItem(ET_WEAPON);
		//			lastNoVehicleWeapon = curWeaponItemID;
		//		}
		//	}
		//}
		//else
		//{
		//	if (  this->GetCurrentEquipItem(ET_WEAPON) != lastNoVehicleWeapon )
		//	{
		//		curWeaponItemID =  this->GetCurrentEquipItem(ET_WEAPON) ;
		//		lastNoVehicleWeapon = curWeaponItemID;
		//	}
		//	else if (lNewWeaponLevel != 0 && vehicleWeapon != lastVehicleWeapon)
		//	{
		//		curWeaponItemID = vehicleWeapon;
		//		lastVehicleWeapon = curWeaponItemID;
		//	}
		//}		
		//new
		curWeaponItemID = m_iWeaponFashionID > 0 ? m_iWeaponFashionID : m_iWeaponFashionBasic;
		//--new
		lastNoVehicleWeapon = curWeaponItemID;
	}
	else	//下马使用门派武器
	{
//origin
		//curWeaponItemID = this->GetCurrentEquipItem(ET_WEAPON);
//new
		curWeaponItemID = m_iWeaponFashionID > 0 ? m_iWeaponFashionID : m_iWeaponFashionBasic;
//--new
		lastNoVehicleWeapon = curWeaponItemID;
	}

	if ( GetSkillFlag(SK_GOD_THING) )
	{
		curWeaponItemID	= GetStateExtendData(GOD_STATE_ID, GOD_STATE_WEAPONTYPE);
		lastNoVehicleWeapon = curWeaponItemID;
	}

	//切换武器
	if (m_lastWeaponItemID == curWeaponItemID)
	{
		return false;
	}
	bool changed = this->ChangeAvatar(m_lastWeaponItemID, curWeaponItemID);
	if (changed)
	{
		m_lastWeaponItemID = curWeaponItemID;
	}

	return changed;
}
//-------------------------------------------------------
bool LC_ClientGamePlayer::_updateVehicleAvatar()
{
	object_id_type jc = GetCimeliaCharTypeID(LC_SUB_TP_VEHICLE,GetVehicleLevel());
	if ( m_lastVehicleCharType != jc && GetRiderCharType() > 0)
	{
		this->ChangeVehicleAvatar(0);
		DestoryRiderVisibleData();
		CreateRiderVisibleData();
		ChangeVehicleAvatar(this->GetVehicleAvatarID());
		m_lastVehicleCharType = jc;
		RefreshAnQiZFix(jc);
	}

	return true;
}
//-------------------------------------------------------
bool   LC_ClientGamePlayer::_updateBowAvatar(int lNewEquipID)	//取值范围[0~n)，0表示无效，n表示n阶弩炮
{
	if (m_lastBowLevel == lNewEquipID )
	{
		return false;
	}

	long lOldEquipID = m_lastBowLevel;
	long lOldType = LC_Helper::GetArrowItemID(lOldEquipID);
	long lNewType = LC_Helper::GetArrowItemID(lNewEquipID);
	bool changed = this->ChangeAvatar(lOldType, lNewType);
	if (changed)
	{
		m_lastBowLevel = lNewEquipID;
	}
	return changed;
}
//-------------------------------------------------------
bool LC_ClientGamePlayer::_updateWingAvatar()
{
	LC_SubUpgradeData *pkData = GetSubordinateAsset().GetSubData(LC_SUB_TP_MUHUN);
	if ( !pkData )
		return false;

	object_id_type lNewCharType = GetSpecialCimeliaCharTypeID(LC_SUB_TP_MUHUN, pkData->GetStage());
	if (m_lastWingCharType == lNewCharType )
	{
		return false;
	}
	bool changed = this->ChangeAvatar(m_lastWingCharType, lNewCharType);
	if (changed)
	{
		m_lastWingCharType = lNewCharType;
	}
	return changed;
}
//-------------------------------------------------------
void LC_ClientGamePlayer::DoUpdateWing(int llevel)
{
	LC_SubUpgradeData* pkSub = GetMuhun();
	if (pkSub)
	{
		pkSub->SetStage(llevel);
	}
}
//-------------------------------------------------------
void LC_ClientGamePlayer::DoUpdateBowAvatar()
{
	m_lastBowLevel = 0;
}
//-------------------------------------------------------
void LC_ClientGamePlayer::DoUpdateWingAvatar()
{
	m_lastWingCharType = 0;
}
//-------------------------------------------------------
void LC_ClientGamePlayer::DoUpdateAnqiAvatar()
{
	m_lastAnQiCharType = 0;
	m_lastAnQiID = 0;
}
//------------------------------------------------------
void LC_ClientGamePlayer::DoUpdateFabaoAvatar()
{
	m_lastJCCharType = 0;
	m_lastJCLevel = 0;
	m_lastJCSetLevel = 0;
}
//-------------------------------------------------------
void LC_ClientGamePlayer::DoUpdateWeaponAvatar()
{
	m_lastWeaponItemID = 0;
}
//-------------------------------------------------------
void LC_ClientGamePlayer::_updateSuitFollow(LC_ClientNPC* pkNPC)
{
	if ( pkNPC )
	{
		int minDis = g_settings.DS_LC_NORMAL_DIS;
		//根据坐骑ID去取跟随范围
		object_id_type jc = GetCimeliaCharTypeID(LC_SUB_TP_VEHICLE,GetVehicleLevel());
		mem::map<int,int>::iterator ds = g_settings.DS_LC_VEHICLE_DIS_VEC.find(jc);
		if ( ds != g_settings.DS_LC_VEHICLE_DIS_VEC.end() )
		{
			minDis = ds->second;
		}
		pkNPC->SetProperty_MinDistanceToFollow(minDis);			 
		pkNPC->SetProperty_MaxDistanceToFollow(minDis+g_settings.DS_LC_DELTA_DIS);
	}
}
//-------------------------------------------------------
void LC_ClientGamePlayer::RefreshAnQiZFix(object_id_type vehicleCharType)
{
	//暗器坐骑高度修改
	mem::map<int,int>::iterator fd = g_settings.DS_AQ_VEHICLE_Z_FIX.find(vehicleCharType);
	if ( fd != g_settings.DS_AQ_VEHICLE_Z_FIX.end() ) 
		m_vehicleZFix = fd->second;
	else
		m_vehicleZFix = 0;
}
//-------------------------------------------------------
void LC_ClientGamePlayer::DoUpdateFB(int llevel)
{
	LC_SubUpgradeData* pkSub = GetFb();
	if (pkSub)
	{
		pkSub->SetStage(llevel);
		pkSub->SetSetTransform(0);
	}
	m_lastJCSetLevel = 0;
}
//-------------------------------------------------------
void LC_ClientGamePlayer::DoUpdateLC(int lLevel)
{
	LC_SubUpgradeData* pkSub = GetSpiritPet();
	if (pkSub)
	{
		pkSub->SetStage(lLevel);
	}
	m_lastLCCharType = 0;
}
//-------------------------------------------------------
void LC_ClientGamePlayer::DoUpdateLCWeapon(int lLevel)
{
	LC_SubUpgradeData* pkSub = GetSpiritWeapon();
	if (pkSub)
	{
		pkSub->SetStage(lLevel);
	}
	m_lastLCWeaponType = 0;
}
//-------------------------------------------------------
void LC_ClientGamePlayer::DoUpdateAQ(int iLevel)
{
	if ( GetHiddenWeapon() )
		GetHiddenWeapon()->SetStage(iLevel);
}
//-------------------------------------------------------
bool   LC_ClientGamePlayer::_updateEyeAvatar()	//0表示无效
{
	//每帧都去判断Eye是否存在
	LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
	if (m_iEyeNPCID != 0)	//如果已存在天眼NPC
	{
		LC_ClientNPC *pkEye = (LC_ClientNPC *)pkNPCManager->FindNPC(m_iEyeNPCID);
		if (pkEye == NULL)	//且被清除了，比如切换地图
		{
			this->_clearEyeAvatar();	//则重置
		}
		else
		{
			pkEye->SetVerticalOffset(/*GetProperty_ShapeHeight() + */pkEye->GetProperty_ShapeHeight());
		}
	}

	LC_SubUpgradeData* sud = GetSubordinateAsset().GetSubData(LC_SUB_TP_DEVA_EYE);
	if ( !sud )
		return false;

	object_id_type charType = !this->GetDead() ? this->GetCimeliaCharTypeID(LC_SUB_TP_DEVA_EYE, sud->GetStage()) : 0;
	//更换Avatar
	if (m_lastEyeCharType == charType )
	{
		return false;
	}

	if (m_iEyeNPCID != 0)
	{
		pkNPCManager->DestroyEyeNPC(m_iEyeNPCID);
		m_iEyeNPCID = 0;
	}

	if (charType != 0)
	{
		m_iEyeNPCID = pkNPCManager->CreateEyeNPC(charType, this, true);
	}
	m_lastEyeCharType = charType;

	return true;
}
//-------------------------------------------------------
void	LC_ClientGamePlayer::_clearEyeAvatar(void)
{
	m_lastEyeCharType = 0;
	m_iEyeNPCID = 0;
}
//-------------------------------------------------------
void LC_ClientGamePlayer::_updateJC()
{
	if ( !IsCimeliaEnabled(LC_SUB_TP_TALISMAN) )
		return ;

	object_id_type jcCharType = m_lastJCCharType;
	if ( this->GetFBSetLevel() == INVALID_FAOBAO_LEVEL && m_lastJCLevel != INVALID_FAOBAO_LEVEL )
	{
		jcCharType = LC_Helper::GetFBCharTypeID(0);
		m_lastJCLevel = this->GetFBLevel();
	}
	else if ( this->GetFBSetLevel() > INVALID_FAOBAO_LEVEL && m_lastJCSetLevel != this->GetFBSetLevel() )
	{
		CF_SubordinateTransform::DataEntry* dk = SafeGetCSVFileDataEntryPtr<CF_SubordinateTransform>(this->GetFBSetLevel());
		if ( dk )
			jcCharType = dk->_iEscCharType; 
		m_lastJCSetLevel = this->GetFBSetLevel();
	}

	if ( m_lastJCCharType != jcCharType )
	{
		DestroyBallVisibleData();
		m_lastJCCharType = jcCharType;
		m_idleTime = GET_PROCESS_TIME;
		CreateBallVisibleData();
	}

	GD_ClientActorModel* pkRiderVisibleData = (GD_ClientActorModel*)
		IM_Manager::GetSingleton().GetFabaoActor(GetID());

	NiPoint3 sd = UT_EngineMath::ConvertVec3IntToNiPoint3(GetVisibleLocation());
	sd.z += m_iProperty_ShapeHeight + g_settings.DS_FABAO_DELTA;
	if ( pkRiderVisibleData ) 
		pkRiderVisibleData->SetTranslate(sd);

	if ( pkRiderVisibleData && GET_PROCESS_TIME - m_idleTime > g_settings.DS_JC_INTERVAL ) 
	{
		pkRiderVisibleData->PlayAnimation(RES_ANM_ACTOR_IDLE0, true);
		m_idleTime = GET_PROCESS_TIME;
	}	
}
//-------------------------------------------------------
void LC_ClientGamePlayer::_updateLC(float fCurrentTime, float fDeltaTime)
{
	LC_ClientToyPetManager* pkToyPetMgr = LC_ClientToyPetManager::GetSingletonPtr();
	if ( pkToyPetMgr == NULL || pkToyPetMgr->GetCurrentPetInstance() == NULL)
	{
		if(m_lastLCCharType > 0)
		{
			DestroySuiteNPC();
			m_lastLCCharType = 0;
		}
		return ;
	}

	LC_ClientNPCManager* pkmgr = CLIENT_GET_NPC_MANAGER(); 
	LC_ClientNPC* lc = NULL;

	SC_ClientScriptInterface* pkClientInterface = SC_ClientScriptInterface::GetSingletonPtr();
	SC_ClientSkillInterface* pkInterface = pkClientInterface->GetSkillInterface();

	LC_ClientToyPetInstance *pkToyPet = pkToyPetMgr->GetCurrentPetInstance();
	if(pkToyPet == NULL)
		return;

	long lCharType = pkToyPet->GetCharType();
	if ( m_lastLCCharType != lCharType )
	{
		DestroySuiteNPC();
		m_lastLCCharType = lCharType;
		CreateSuiteNPC();
	}
	lc = (LC_ClientNPC*)pkmgr->FindNPC(m_lastLCID);
	if (m_lastLCCharType > 0 && pkInterface->GetGameStage() == ST_GAME_STAGE_GAME && lc == NULL)
	{
		CreateSuiteNPC();
	}

	pkToyPet->GetSkillAsset()->Update(fCurrentTime, fDeltaTime);
}
//-------------------------------------------------------
void LC_ClientGamePlayer::_updateLCWeapon()
{
	if ( !IsCimeliaEnabled(LC_SUB_TP_SPIRIT_WEAPON) )
		return ;

	LC_ClientNPCManager* pkmgr = CLIENT_GET_NPC_MANAGER(); 
	LC_ClientNPC* lc = NULL;

	LC_SubUpgradeData* sud = GetSubordinateAsset().GetSubData(LC_SUB_TP_SPIRIT_WEAPON);
	if ( !sud )
		return ;

	object_id_type charType = this->GetSpecialCimeliaCharTypeID(LC_SUB_TP_SPIRIT_WEAPON, sud->GetStage());
	if ( m_lastLCWeaponType != charType )
	{
		//DestroySuiteNPC();
		m_lastLCWeaponType = charType;
		CreateSuitWeapon();
	}
}
//-------------------------------------------------------
void LC_ClientGamePlayer::_updateAnQi()
{
	if ( !IsCimeliaEnabled(LC_SUB_TP_HIDDEN_WEAPON) )
		return ;

	LC_SubUpgradeData* sud = GetSubordinateAsset().GetSubData(LC_SUB_TP_HIDDEN_WEAPON);
	if ( !sud )
		return ;

	object_id_type ct = GetSpecialCimeliaCharTypeID(LC_SUB_TP_HIDDEN_WEAPON, sud->GetStage());
	if ( m_lastAnQiCharType !=  ct)
	{
		DestroyAnQi();
		m_lastAnQiCharType = ct;
		CreateAnQi();
	}
	
	GD_ClientActorModel* pkRiderVisibleData = (GD_ClientActorModel*)
		IM_Manager::GetSingleton().GetAnQiActor(GetID());

	NiPoint3 sd = UT_EngineMath::ConvertVec3IntToNiPoint3(GetVisibleLocation());
	sd.z += m_vehicleZFix + g_settings.DS_ANQI_DELTA;

	if ( pkRiderVisibleData ) 
		pkRiderVisibleData->SetTranslate(sd);	
}
//-------------------------------------------------------
void	LC_ClientGamePlayer::SetEquipWeapon(int lNewItemID)
{
	if (IS_ITEM_ID(lNewItemID))
	{
		LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
		LC_ClientItem*  pkItem = (LC_ClientItem* )pkItemFactory->RequestItem(lNewItemID);
		if(!pkItem)
		{
			//GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"试图装备无效物件, %d", lNewItemID);
			return ;
		}

		if(pkItem->GetEquipTypeLimit() == ET_WEAPON)
		{
			SetCurrentBandWeapon((WeaponType)pkItem->GetEquiplWeaponType());
		}
	}
	else
	{
		SetCurrentBandWeapon(WT_HAND);
	}
}
//----------------------------------------------------------
bool LC_ClientGamePlayer::IsNeedUnBindCashSkillUpGrade(int lSkillCateGory, int iSlotIndex)
{
	LC_SkillAssetEntry* pkEntry = GetSkillAsset()->GetSkillEntryInAsset(lSkillCateGory, iSlotIndex);
	if(!pkEntry || !pkEntry->GetValid())
	{		
		return false;
	}

	int lSkillID = pkEntry->GetTypeID();
	CF_SkillLevelUp* pkCSVSkillLevelUp = CF_SkillLevelUp::GetSingletonPtr();
	CF_SkillLevelUp::DataEntry* pkData = pkCSVSkillLevelUp->GetEntryPtr(lSkillID);
	if(!pkData)
		return false;

	MoneyType ulCash = pkData->_lNeedCash;
	if ( ulCash > GetBindCash().GetMoney() )
	{
		MoneyType ulNeedBindPrice = ulCash - GetBindCash().GetMoney();
		if ( ulNeedBindPrice > GetUnBindCash().GetMoney() )
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	return false;
}
//----------------------------------------------------------
void LC_ClientGamePlayer::RequestSkillUpGrade(int lSkillID,int iMax, int cID)//, bool bConfirmUseUnBindCash)
{
	if(GetActionForbid()->IsActionForbid(LAFT_GAME,LAF_GAME_SKILL_LEVEL_UP))
	{
		SendLogicInfoMessage(RE_ACTION_FORBID);
		return;
	}

	LC_SkillAsset *pkSkillAsset = GetBuddySkillAsset(cID);
	if(NULL == pkSkillAsset)
	{
		return;
	}

	if(!pkSkillAsset->IsLearnedSkill(lSkillID))
	{
		SendLogicInfoMessage(RE_SKILL_CANNOT_UPGRADE_NOTLEARED_SKILL);
		return;
	}

	LC_SkillAssetEntry* pkEntry = pkSkillAsset->FindLearnedSkill(lSkillID);
	if (NULL == pkEntry)
	{
		return;
	}

	skill_id_type iSkillID = pkEntry->GetTypeID();
	CF_SkillSpecialAttrib::DataEntry *pkSkillInfo = SafeGetCSVFileDataEntryPtr<CF_SkillSpecialAttrib>(iSkillID);
	if(NULL == pkSkillInfo)
		return;

	CF_SkillLevelUp::DataEntry *pkSkillLevelup = SafeGetCSVFileDataEntryPtr<CF_SkillLevelUp>(iSkillID);
	if(NULL == pkSkillLevelup)
		return;

	if(pkSkillInfo->_lCurLevel >= pkSkillInfo->_lTopLevel && !pkSkillLevelup->_bBreak)
	{
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_SKILL_LEVEL_UP);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, pkEntry->GetTypeID());
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_3, RE_SKILL_UPDATE_TO_MAX_LEVEL);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
		return;
	}

	int lSkillCateGory = pkSkillAsset->GetSkillCategory2(lSkillID);
	int iSlotIndex = pkEntry->GetSlotIndex();
	//if (!bConfirmUseUnBindCash)
	//{
	//	if ( IsNeedUnBindCashSkillUpGrade(lSkillCateGory, iSlotIndex ) )
	//	{
	//		// 填出确认框
	//		SendMessageDialog(RE_DIALOG_CONFIRM_USE_UNBINDCASH_SKILL_LEVEL_UP, 
	//			"", "",RE_DIALOG_CONFIRM_USE_UNBINDCASH_SKILL_LEVEL_UP, 
	//			lSkillCateGory, iSlotIndex, 0);

	//		return;			
	//	}
	//}

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_UpdateSkillInSkillAsset reqMsg;
	reqMsg.m_lPlayerID = GetID();
	reqMsg.m_lSkillCategory = lSkillCateGory;
	reqMsg.m_iEntryIndex = iSlotIndex;
	reqMsg.m_iMax = iMax;
	reqMsg.m_iBuddyID = cID;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SKILL_UPDATE, &reqMsg);

	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

void LC_ClientGamePlayer::RequestLearnControllerSkill(int iSkillID, int iSubType)
{
	MG_REQ_LearnControllerSkill reqMsg;
	reqMsg.m_nSkillID = iSkillID;
	reqMsg.m_nSubType = iSubType;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_LEARNCONTROLLERSKILL, &reqMsg);
	if(bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::RequestAutoUpgradeSkill(bool bStart, int iSkillID, int cID)
{
	if(bStart)
	{
		SetCurrentUpgradeSkillID(iSkillID);
		SetCurrentUpgradeBuddyID(cID);
		m_fLastAutoUpgradeSkillTime = 0.0f;
		RequestSkillUpGrade(iSkillID, 0, cID);
	}
	else
	{
		SetCurrentUpgradeSkillID(0);
		SetCurrentUpgradeBuddyID(0);
		m_bDoAutoUpgradeSkill = bStart;
	}
	m_bAutoUpgradeSkill = bStart;
}
//----------------------------------------------------------
void LC_ClientGamePlayer::RequestDeactiveSkillState(int lSkillStateTypeID)
{
	CF_SkillStateList* pkCSVStateList = CF_SkillStateList::GetSingletonPtr();
	if(!pkCSVStateList)
		return;

	CF_SkillStateList::DataEntry* pkData = pkCSVStateList->GetEntryPtr(lSkillStateTypeID);
	if(!pkData)
		return;

	if(!pkData->_bCanManualDeactive)
	{
		//SendLogicInfoMessage(RE_SKILL_STATE_DEACTIVE_FAIL,pkData->_kName.c_str());
		return;
	}

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_DeactiveSkillState reqMsg;
	reqMsg.m_lSkillStateTypeID = lSkillStateTypeID;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_DEACTIVE_SKILL_STATE, &reqMsg);

	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
//----------------------------------------------------------
static bool _IsNeedDetachStateGFX(CF_SkillStateList::DataEntry* pkData, LC_ActorStateEntry* pkStateEntry)
{
	if (!pkData)
		return false;

	return ( (pkData->_eShowMode == SK_State::SSM_ALL_SHOW || pkData->_eShowMode == SK_State::SSM_GAMEPLAYER_SHOW) && pkData->_eOverlapShowMode != SK_State::SOSM_NO_DELETE_GFX
		|| pkStateEntry->GetCurrentLifeTime() > pkStateEntry->GetMaxLifeTime() );
}
struct EndStateGFXOper: public LC_ActorStateOperator
{
	EndStateGFXOper(GD_ClientActorModel* visibleData)
		:pkVisibleData(visibleData)
	{}

	virtual bool operator ()(GameLogic::LC_ActorStateEntry* pkEntry)
	{
		if(pkVisibleData && pkEntry)
		{
			CF_SkillStateList::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_SkillStateList>(pkEntry->GetTypeID());
			if(_IsNeedDetachStateGFX(pkData, pkEntry))
			{
				pkVisibleData->DetachFxExt(GD_ModelBase::FxDummyNodeType(pkData->_lDummyType),pkData->_kGFXName);
				return true;
			}
		}
		return false;
	}
	GD_ClientActorModel* pkVisibleData;
};
void LC_ClientGamePlayer::EndSkillStateGFX()
{
	GD_ClientActorModel* pkVisibleData = IM_Manager::GetSingleton().GetActor(GetID());
	if(!pkVisibleData)
		return;

	EndStateGFXOper oper(pkVisibleData);
	m_kSkillStateMap->ForAllState(&oper);
}
//---------------------------------------
struct BeginStateGFXOperGP: public LC_ActorStateOperator
{
	BeginStateGFXOperGP(GD_ClientActorModel* visibleData)
		:pkVisibleData(visibleData)
	{}

	virtual bool operator ()(GameLogic::LC_ActorStateEntry* pkEntry)
	{
		if(pkVisibleData && pkEntry)
		{
			CF_SkillStateList::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_SkillStateList>(pkEntry->GetTypeID());
			if(pkData && pkData->_eShowMode == 0 || pkData->_eShowMode == 1)
			{
				pkVisibleData->AttachFxExt(GD_ModelBase::FxDummyNodeType(pkData->_lDummyType), pkData->_eBindMode, pkData->_bStayAfterBound, pkData->_kGFXName);
				return true;
			}
		}
		return false;
	}
	GD_ClientActorModel* pkVisibleData;
};
void LC_ClientGamePlayer::BeginSkillStateGFX()
{
	GD_ClientActorModel* pkVisibleData = IM_Manager::GetSingleton().GetActor(GetID());
	if(!pkVisibleData)
		return;

	BeginStateGFXOperGP oper(pkVisibleData);
	m_kSkillStateMap->ForAllState(&oper);
}


inline static uint64_t ConverterSystemTimeToLongTime(const SYSTEMTIME& rkTime)
{
	FILETIME kFileTime;
	memset(&kFileTime, 0, sizeof(kFileTime));

	//将日历时间转换成整形时间
	if (SystemTimeToFileTime(&rkTime, &kFileTime))
	{
		LARGE_INTEGER kLargeInteger;
		kLargeInteger.LowPart = kFileTime.dwLowDateTime;
		kLargeInteger.HighPart = kFileTime.dwHighDateTime;
		return static_cast<uint64_t>( kLargeInteger.QuadPart );
	}
	return 0;
}

//从当前时间到目标时间的时间戳，以秒为单位。
int LC_ClientGamePlayer::GetTimeStampFromCurrent(int iSecond, int iMinute, int iHour, int iDay/* = -1*/, int iMonth/* = -1*/, int iYear/* = -1*/)
{
	//当前时间
	SYSTEMTIME targetSystemTime;
	if(iSecond >= 0)
		targetSystemTime.wSecond = iSecond;
	if(iMinute >= 0)
		targetSystemTime.wMinute = iMinute;
	if(iHour >= 0)
		targetSystemTime.wHour = iHour;
	if(iDay >= 0)
		targetSystemTime.wDay = iDay;
	if(iMonth >= 0)
		targetSystemTime.wMonth = iMonth;
	if(iYear >= 0)
		targetSystemTime.wYear = iYear;

	uint64_t lCurTime = (uint64_t)GetServerTimeVal();
	uint64_t lTargetTime = ConverterSystemTimeToLongTime(targetSystemTime);
	if(lTargetTime > lCurTime)
		return (int)((lTargetTime - lCurTime) / 10000000);	//filetime的单位是100纳秒。
	else
		return -(int)((lCurTime - lTargetTime) / 10000000);	//filetime的单位是100纳秒。
}

int64_t LC_ClientGamePlayer::GetEndOfDayTimeStampFromCurrent(time_t iCurTime, time_t iTargetTime)
{
	return iTargetTime-iCurTime;
}
//---------------------------------------
void LC_ClientGamePlayer::RequestAlliance()
{
	m_kClientAlliance.ReqAllianceInfo();
}
//---------------------------------------
void LC_ClientGamePlayer::RltAlliance(int groupID, Protocol::UnitServerNodeList& li)
{
	m_kClientAlliance.RltAllianceInfo(groupID, li);
}
//---------------------------------------
void LC_ClientGamePlayer::RltEnterAlliance(int retType)
{
	m_kClientAlliance.RltEnterAlliance(retType);
}
//---------------------------------------
void LC_ClientGamePlayer::RequestSoulOperation(int iType, bool bBindYuanbao)
{
	MG_PB<msg_define::bone_soul_operation_req> kReqMsg;
	kReqMsg.m_value.set_type(iType);
	kReqMsg.m_value.set_use_bind_yuanbao(bBindYuanbao);
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_BONE_SOUL_TRAIN, &kReqMsg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}
//---------------------------------------
StringType LC_ClientGamePlayer::GetInvestmentPlanTaskID(int iType)
{
	StringStreamType sStream("");
	CF_InvestmentPlan::DataEntryMapExternIterator kIter = CF_InvestmentPlan::GetSingleton().GetIterator();
	for (; kIter.HasMoreElements(); kIter.MoveNext())
	{
		CF_InvestmentPlan::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		if (pkEntry->_iType == iType)
		{
			StringType sStr;
			FormatDataIFs2String(pkEntry->_sAchievementID, sStr);
			sStream << sStr;
			break;
		}
	}
	return sStream.str();
}
//---------------------------------------
StringType LC_ClientGamePlayer::GetInvestmentPlanScheduleID(int iType)
{
	StringStreamType sStream("");
	CF_InvestmentPlan::DataEntryMapExternIterator kIter = CF_InvestmentPlan::GetSingleton().GetIterator();
	for (; kIter.HasMoreElements(); kIter.MoveNext())
	{
		CF_InvestmentPlan::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		if (pkEntry->_iType == iType)
		{
			sStream << pkEntry->_iSchedule;
			break;
		}
	}
	return sStream.str();
}
//---------------------------------------
bool LC_ClientGamePlayer::_sortFunc(const LC_ItemDescriptionEntry *v1, const LC_ItemDescriptionEntry *v2)
{
	bool b1 = IS_SENIOR_MEDICINE_ITEM_ID(v1->GetTypeID());
	bool b2 = IS_SENIOR_MEDICINE_ITEM_ID(v2->GetTypeID());
	if(b1 == b2)
		return v1->GetLevel() > v2->GetLevel();

	return !b1;
}
//------------------------------------------------------------------------
bool     LC_ClientGamePlayer::IsCurrentInTheArea(int lMap,int lArea)
{
	LC_LogicMapManager*	pkMapManager = LC_LogicMapManager::GetSingletonPtr();
	if (pkMapManager == NULL)
		return false;

	LC_MapBase* pkMap = GetMap();
	if (pkMap == NULL)
		return false;

	LC_LogicAreaInfo* kAreaInfo = pkMapManager->QueryAreaInfo(GetCurrentLocation(),GetMapResID());

	int areaID = kAreaInfo != NULL ? kAreaInfo->lLogicAreaID : 0;
	if (GetMapResID() == lMap && areaID == lArea)
	{
		return true;
	}
	else
		return false;
}
//------------------------------------------------------------------------
bool LC_ClientGamePlayer::CheckCanTransferMap(int iMapResID)
{
	int iRetValue = 0;
	int iResult = LC_ClientHelp::CanEnterMap(iMapResID, this, iRetValue);
	if (RE_SUCCESS != iResult)
	{
		switch(iResult )
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
		
		return false;
	}

	return true;
}

bool LC_ClientGamePlayer::RequestTransportAreaTransfer(int iTransferType, int iTransAreaID, int iMapResID, int iServerID, int iLineNum, bool bTribuMap)
{
	if ( m_bAlreadyReqTransfer )
	{
		return false;
	}
	if (GetDead())
	{
		SendLogicInfoMessage(RE_TRANSFER_CANT_DEAD);
		return false;
	}

	CF_WorldMapList::DataEntry* pkWorldMapData = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(iMapResID);
	if (!pkWorldMapData)
		return false;

	if(!CheckCanTransferMap(iMapResID))
		return false;

	if (IsInCrossRealMapTeam() && !IS_NORMAL_MAP(pkWorldMapData->_iMapLogicType))
	{
		SendLogicInfoMessage(RE_IN_CROSS_MAP_TEAM_CAN_ENTER_AREA);
		return false;
	}

	GfxWriteLog(LOG_TAG_GAME_LOG, LOG_SWITCH_GAME,"[RaidMap]LC_ClientGamePlayer::RequestTransportAreaTransfer iTransferType=%d iTransAreaID=%d iMapResID=%d iServerID=%d iLineNum%d bTribuMap=%d",
		iTransferType, iTransAreaID, iMapResID, iServerID, iLineNum, bTribuMap);

	//发送网络消息
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	MG_REQ_Transfer msgTransfer;
	msgTransfer.m_iTransferType = iTransferType;
	msgTransfer.m_lPlayerID		= GetID();
	msgTransfer.m_iIndex		= iTransAreaID;
	msgTransfer.m_iResId		= iMapResID;
	msgTransfer.m_iServerID		= 0;
	msgTransfer.m_bTribuMap		= bTribuMap;
	msgTransfer.m_iLineNum		= iLineNum;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_TRANSFER, &msgTransfer);

	m_bAlreadyReqTransfer = true;
	m_kReqTransferInterval.Start(REQ_TRANSFER_INTERVAL);

	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	SetTranToTranTick(GET_PROCESS_TIME);
	return true;

}//------------------------------------------------------------------------
bool LC_ClientGamePlayer::RequestTransfer(int iTransferType, int iTransAreaID, int iMapResID, int iServerID, int iLineNum, bool bTribuMap)
{
	LC_ClientGameStoryManager *pkGameStoryMgr = GetGameStoryManager();
	if (pkGameStoryMgr == NULL)
	{
		return false;
	}
	CF_WorldMapList::DataEntry* pkWorldMapData = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(iMapResID);
	if (!pkWorldMapData)
		return false;

	//部分跨服副本进入问题，提示副本已满无法进入，可能由于跨服副本造成
	GameStoryInfo* pkGameStoryInfo = pkGameStoryMgr->GetGameStoryInfo(pkWorldMapData->_iStoryID);
	if (pkGameStoryInfo && pkGameStoryInfo->GetActive() > 0 && pkGameStoryInfo->IsCrossMap(iMapResID))
	{
		return RequestEnterCrossMap(pkWorldMapData->_iStoryID, iMapResID);
	}
	else
	{
		return RequestTransportAreaTransfer(iTransferType, iTransAreaID, iMapResID, iServerID, iLineNum, bTribuMap);
	}
}
//------------------------------------------------------------------------
bool LC_ClientGamePlayer::IsNeedUnBindCashNPCTransfer(int iIndex,int iTransferType)
{
	LC_NPCManagerBase* const pkNPCManager = ENGINE_GET_NPC_MANAGER();
	if (!pkNPCManager)
		return false;

	LC_NPCBase* const pkNPC = pkNPCManager->FindNPC(m_lLockedNpcID);
	if (!pkNPC)
		return false;

	LC_NPCTransferManager* pkTranferManager = LC_NPCTransferManager::GetSingletonPtr();
	if (!pkTranferManager)
		return false;

	LC_TransferBase* const pkNPCTransfer = pkTranferManager->GetNPCTransfer(pkNPC->GetCharType());
	if(!pkNPCTransfer)
		return false;

	MoneyType ulPrice = pkNPCTransfer->GetPrice(iIndex);
	if ( ulPrice > GetBindCash().GetMoney() )
	{
		MoneyType ulNeedBindPrice = ulPrice - GetBindCash().GetMoney();
		if ( ulNeedBindPrice > GetUnBindCash().GetMoney() )
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	return false;
}
//------------------------------------------------------------------------
void LC_ClientGamePlayer::RequestTranscriptionTransfer( int iTriggerIndex )
{
	CF_TransferArea::DataEntry* pkTransferAreaData = SafeGetCSVFileDataEntryPtr<CF_TransferArea>(iTriggerIndex);
	if (!pkTransferAreaData)
		return;

	//if(GetLevel() < pkTransferAreaData->_lPlayerLevel)
	if(GetCommanderLevel() < pkTransferAreaData->_lPlayerLevel)
		return;

	CF_WorldMapList::DataEntry* pkWorldMapData = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(pkTransferAreaData->_lMapID);
	if (!pkWorldMapData)
		return;

	if (IS_NORMAL_MAP(pkWorldMapData->_iMapLogicType))
	{
		RequestTransfer(TT_TRANSPORT_NORMAL_MAP, iTriggerIndex, pkWorldMapData->_iID);
	}
	else
	{
		RequestTransfer(TT_TRANSPORT_RAID_MAP, iTriggerIndex, pkWorldMapData->_iID);
	}
}
//------------------------------------------------------------------------
void	LC_ClientGamePlayer::RequestNPCSetTransferPos()
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	LC_NPCManagerBase* const pkNPCManager = ENGINE_GET_NPC_MANAGER();
	if (pkNPCManager == NULL)
		return;
	LC_NPCBase* const pkNPC = pkNPCManager->FindNPC(m_lLockedNpcID);
	if (pkNPC == NULL)
		return;
	LC_NPCTransferManager* pkTranferManager = LC_NPCTransferManager::GetSingletonPtr();
	if (!pkTranferManager)
		return ;

	LC_TransferBase* const pkNPCTransfer = pkTranferManager->GetNPCTransfer(pkNPC->GetCharType());
	if (pkNPCTransfer == NULL)
		return;

	if (pkNPCTransfer->CanBindTransferPos())
	{
		MG_REQ_SetTransferPos msg;
		msg.m_lPlayerID		= GetID();
		msg.m_lNPCID		= m_lLockedNpcID;

		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SET_TRANSFER_POS, &msg);

		if(bCreateMsg)
			m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
	else
	{
		SendLogicInfoMessage(RE_TRANSFER_NPC_CANT_BIND);
	}
}
////---------------------------------------
//void LC_ClientGamePlayer::InvitePlayerToDuel(object_id_type lID)
//{
//	MG_PACKET_ENCODE_HANDLER hMsgHandler;
//	bool bCreateMsg = false;
//
//	if(IS_PLAYER_ID(lID))
//	{
//		if(lID == m_pkClientLocalSession->GetPlayer()->GetID())
//		{
//			GfxWriteLog(LOG_TAG_GAME_TIP,LOG_SWITCH_GAME,"<color val = 0xFFFF0000>不能跟自己发起决斗邀请</color>","");
//			return;
//		}
//
//		//操作是否被屏蔽
//		if(m_kActionForbid.IsActionForbid(LAFT_DUEL,LAF_DUEL_MAX_COUNT))
//		{
//			SendLogicInfoMessage(RE_ACTION_FORBID);
//			return;
//		}
//
//		LC_PlayerDuelState* pkPlayerDuelState = GetDuelState();
//		if(pkPlayerDuelState->GetDuelState() != LC_PlayerDuelState::DS_NULL)
//		{
//			SendLogicInfoMessage(RE_DUEL_ERROR_INDUEL);
//			return;
//		}
//
//		pkPlayerDuelState->SetDuelID(0);
//		pkPlayerDuelState->SetDuelState(LC_PlayerDuelState::DS_INVITE);
//		pkPlayerDuelState->SetDuelInviter(lID);
//
//		//LC_PlayerDuelFunc::CheckPlayerCanBeInvite(GetID(),lID);
//		MG_REQ_DuelInvite reqMsg;
//		reqMsg.m_lInvitePlayerID = m_pkClientLocalSession->GetPlayer()->GetID();
//		reqMsg.m_lBeInvitePlayerID = lID;
//
//		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_DUELINVITE, &reqMsg);
//		if(bCreateMsg)
//			m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
//	}
//	else
//	{
//		//打印错误提示
//		GfxWriteLog(LOG_TAG_GAME_TIP,LOG_SWITCH_GAME,"<color val = 0xFFFF0000>决斗邀请目标错误</color>","");
//	}
//}
////---------------------------------------
//bool  LC_ClientGamePlayer::IsBeInviteToDuel()
//{
//	LC_PlayerDuelState* pkPlayerDuelState = GetDuelState();
//
//	return pkPlayerDuelState->GetDuelState() == LC_PlayerDuelState::DS_BE_INVITE;
//}
////---------------------------------------
//extern const char* GfxGetPlayerNameTransformed(object_id_type ID,char*);
////---------------------------------------
//StringType   LC_ClientGamePlayer::GetBeInviteInfo()
//{
//	static char acError[] = "";
//	static char strInfoBuffer[256];
//
//	LC_PlayerDuelState* pkPlayerDuelState = GetDuelState();
//
//	if(pkPlayerDuelState)
//	{ 
//		object_id_type IID = pkPlayerDuelState->GetDuelInviter();
//		const char* acPlayerName = GfxGetPlayerNameTransformed(IID,NULL);
//		NiSprintf( strInfoBuffer , 256 , "%s向你发出决斗邀请", acPlayerName);
//		return strInfoBuffer;
//	}
//
//	return acError;
//}
////---------------------------------------
//void  LC_ClientGamePlayer::AcceptDuelInvite(bool bAccpet)
//{
//	//操作是否被屏蔽
//	if(m_kActionForbid.IsActionForbid(LAFT_DUEL,LAF_DUEL_MAX_COUNT))
//	{
//		SendLogicInfoMessage(RE_ACTION_FORBID);
//		return;
//	}
//
//	MG_PACKET_ENCODE_HANDLER hMsgHandler;
//	bool bCreateMsg = false;
//
//	LC_PlayerDuelState* pkPlayerDuelState = GetDuelState();
//
//	if(pkPlayerDuelState)
//	{
//		MG_RLT_DuelInvite rltMsg;
//		rltMsg.m_lInvitePlayerID = pkPlayerDuelState->GetDuelInviter();
//		rltMsg.m_lBeInvitePlayerID = GetID();
//		rltMsg.m_ucType = bAccpet ? MG_RLT_DuelInvite::DR_ACCEPT : MG_RLT_DuelInvite::DR_REFUSE;
//		rltMsg.m_lParam = pkPlayerDuelState->GetDuelID();
//
//		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_RLT_DUELINVITE, &rltMsg);
//
//		if(bCreateMsg)
//			m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
//
//	}
//}
//--------------------------------------------------------
void LC_ClientGamePlayer::StopMove()
{
	//SetPathFinding(false);
	ClearPathFinding();
	m_kDoubleTerrienClearPath.ResetTimer();

	//停止行动
	LC_PlayerBase::StopMove();
	this->GetMovePath().Clear();
}
//--------------------------------------------------------
void LC_ClientGamePlayer::ClearPathFinding()
{
	//跨地图寻路
	LC_AcrossMapPath& rAcrossMapPath = this->GetAcrossMapPath();
	if (rAcrossMapPath.IsFindingAcrossMap())
	{
		ClearPendingPathFind();
		rAcrossMapPath.ClearAcrossMapPath();
	}

	//寻路挂起行为
	this->ClearPathPendingAction();
}
//--------------------------------------------------------
bool LC_ClientGamePlayer::ClearPathFindingByLButton()
{
	bool flag = false;
	if ( m_bIsPathFinding )
	{
		if ( m_kDoubleTerrienClearPath.IsDown() || !m_kDoubleTerrienClearPath.IsActivate() )
		{
			m_kDoubleTerrienClearPath.Start(g_settings.DS_CANCLE_PATH_TIMER);
			SendLogicInfoMessage(RE_FINDPATH_CANCEL);
			flag = true;
		}
		else
		{
			ClearPathFinding();
			m_kDoubleTerrienClearPath.ResetTimer();
		}
	}
	
	return flag;
}
//--------------------------------------------------------
//--------------------------------------------------------
void LC_ClientGamePlayer::OnMovePathEnd()
{
	const PathPendingActionArg	&Action = this->GetPathPendingAction();

	if (Action.bNewBeeSwitch && Action.iTargetMapLogicID >= 0)
	{
		LC_ClientNewBeeManager *pkNewBeeManager = LC_ClientNewBeeManager::GetSingletonPtr();
		if (pkNewBeeManager && Action.iTargetMapLogicID != GetMapLogicID() && pkNewBeeManager->IsMapLogicIDIsMapResID(GetMapResID(),Action.iTargetMapLogicID))
		{
			RequestTransfer(TT_TRANSPORT_GROW_UP,Action.iTargetMapLogicID,GetMapResID());
		}
	}
	switch(Action.enmType)
	{
	case PPAT_TALK_TO_NPC:
		CheckPendingTalkNPC(Action.iTargetCharTypeID, GET_PROCESS_TIME);
		break;
	case PPAT_ATTACK_NPC:
		if (!IsHooking())
		{
			SetHooking(true);
		}
		break;
	case PPAT_TRANSFER:
		{
			RequestTranscriptionTransfer(Action.iTargetCharTypeID);
		}
		break;
	case PPAT_COLLECT_ITEM:
		CheckPendingCollectTreasure(Action.iTargetCharTypeID);
		break;
	default:
		break;
	}
	
	SetHookRespawnPathFlag(false);
	ClearPathPendingAction();
}
//---------------------------------------
bool LC_ClientGamePlayer::SetTargetLocation( const Utility::UT_Vec3Int& rkTarget )
{
	if(GetMoveStraightFlag())
		return false;

	bool canfind = false;
	canfind = LC_ActorBase::SetTargetLocation( rkTarget );

	int iPathPointCount = m_kMovePath.GetPathPointCount(); 
	bool bNeedFind = false;
	if ( iPathPointCount > 1 )
	{
		bNeedFind = ( UT_MathBase::LineLengthXYInt( rkTarget , m_kMovePath.GetPathPoint(iPathPointCount - 1)) > 20 );
	}
	else if ( iPathPointCount == 0 || iPathPointCount == 1 )
	{
		bNeedFind = true;
	}
	if( bNeedFind )
	{
		if (PF_PathManager::GetSingletonPtr() && GfxGetWorldConfig().CFG_ENABLE_PATH_FINDING )
		{
			/*
			注意，这里寻路的目标点和实际生成路径的目标点有可能是不一样的
			所以，是否需要改成把Target当成引用传入进行修改？
			*/
			bool bForceAvenue = false;
			/*if ( g_settings.LOD_PATH_QUERY_STEP > 100 )
			{
				bForceAvenue = true;
			}*/
			int dist = UT_MathBase::LineLengthXYInt(m_kCurrentLocation , rkTarget);	//判断距离
			if (dist > AUTO_AVENUE_PATH_DISTANCE)
			{
				bForceAvenue = true;
			}
			bool bFind = PF_PathManager::GetSingletonPtr()->FindPath(m_kMovePath,m_kCurrentLocation,rkTarget , bForceAvenue , true );
			if (!bFind && bForceAvenue)
			{
				m_kMovePath.Clear();
				PF_PathManager::GetSingletonPtr()->FindPath(m_kMovePath,m_kCurrentLocation,rkTarget , false , true );
			}
			
			if (bFind) 
			{
				canfind = true;
			}
			else
			{
				canfind = false;
			}
		}
		else
		{
			m_kMovePath.AllContentClear();
			m_kMovePath.AddPathPoints(m_kCurrentLocation );
			m_kMovePath.AddPathPoints(rkTarget);
			m_kMovePath.GeneratePath();
			canfind = false;
		}
	}
	else
	{
		canfind = true;
	}
	return canfind;
}
//------------------------------------------------------------------------------
void LC_ClientGamePlayer::SetDead(bool bIsDead)
{
	//if(bIsDead && GetRiderCharType() > 0)
	//{
	//	DismissVehicle();
	//}
	bool old_value = GetDead();
	LC_PlayerBase::SetDead(bIsDead);

	this->_clearUnlockRequestExecuteSkill();		/// 清除已请求未响应的技能记录
	ClearPendingSkill();
	ClearMoveStride();
	if(bIsDead && IsAllBuddyDead())
	{
		ClearPathFinding();
	}

	//死亡特效
	/*IM_Manager* pkIMManager = IM_Manager::GetSingletonPtr();
	if(pkIMManager)
	{
		GD_ActorModel* pkVisibleData = pkIMManager->GetActor(GetID());
		if(pkVisibleData)
		{
			if (bIsDead)
				pkVisibleData->AttachFx(GD_ModelBase::FDT_INVALID, FileSystem::MTF_ModelData::AFBM_TRANSLATE_ROTATE, true, RES_FX_SCREEN_DIE);
			else
				pkVisibleData->DetachFx(GD_ModelBase::FDT_INVALID, RES_FX_SCREEN_DIE, true);
		}
	}*/
}
//--------------------------------------------------------
bool LC_ClientGamePlayer::IsNeedUnBindCashItemEnhance(MoneyType ulPrice)
{
	if ( ulPrice > GetBindCash().GetMoney() )
	{
		MoneyType uiNeedBindPrice = ulPrice - GetBindCash().GetMoney();
		if ( uiNeedBindPrice > GetUnBindCash().GetMoney() )
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	return false;
}
//---------------------------------------
void LC_ClientGamePlayer::LearnProduction(int lProductionType, int lProductionLevel)
{
	return;
	/*
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	CF_ProductionList* pkProductionList = CF_ProductionList::GetSingletonPtr();
	CF_ProductionList::DataEntry* pkProLevelData = pkProductionList->GetEntryPtr( lProductionLevel );
	if (!pkProLevelData)
	return;

	int lNeedCharLevel = pkProLevelData->_lNeedCharLevel;
	if (GetLevel() < lNeedCharLevel)
	{
	SendLogicInfoMessage(RE_PRODUCTION_LEARN_FAIL);
	return;
	}

	if (m_lProductionType != PT_NONE && lProductionType != m_lProductionType)
	{
	SendLogicInfoMessage(RE_PRODUCTION_LEARN_AGAIN);
	return;
	}

	if(lProductionLevel > 1)
	{
	CF_ProductionList::DataEntry* pkLastLevelData = pkProductionList->GetEntryPtr(lProductionLevel - 1);
	if (!pkLastLevelData)
	return;

	int lLastLevelMaxMastery = pkLastLevelData->_lCurLevelMaxMastery;

	if (m_lCurrentMastery < lLastLevelMaxMastery || m_lProductionType == PT_NONE)
	{
	SendLogicInfoMessage(RE_PRODUCTION_MASTERY_NOT_ENOUGH);
	return;
	}
	}

	if (lProductionLevel<= m_lProductionLevel)
	return;

	MG_REQ_LearnProduction reqMsg;
	reqMsg.m_lPlayerID = GetID();
	reqMsg.m_lProductionType = lProductionType;
	reqMsg.m_lProductionLevel = lProductionLevel;

	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_LEARNPRODUCTION, &reqMsg);

	//if (lProductionLevel > 1 && lProductionLevel <= 6)
	//{
	//	MG_REQ_LearnProduction reqMsg;

	//	reqMsg.m_lPlayerID = GetID();
	//	reqMsg.m_lProductionType = lProductionType;
	//	reqMsg.m_lProductionLevel = lProductionLevel;

	//	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_UPGRADEPRODUCTION, &reqMsg);
	//}

	if(bCreateMsg)
	m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	*/
}
//---------------------------------------
bool LC_ClientGamePlayer::SetBitVec(int nIndex, bool bSet)
{
	bool b = GetTutorialAsset()->SetBitVec(nIndex, bSet);
	if (!b)
		return false;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	MG_REQ_SetTutorialData reqMsg;
	reqMsg.m_lPlayerID = GetID();
	reqMsg.m_lIndex = nIndex;

	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SET_TUTORIAL_DATA, &reqMsg);
	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	return true;
}
//---------------------------------------
void LC_ClientGamePlayer::RequestConfirmForgetProduction(int lProductionType, bool bAccept)
{

	if(bAccept)
	{
		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = false;

		MG_REQ_ForgetProduction reqMsg;
		reqMsg.m_lPlayerID = GetID();
		reqMsg.m_lProductionType = lProductionType;
		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_FORGETPRODUCTION, &reqMsg);

		if(bCreateMsg)
			m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}
//---------------------------------------
void LC_ClientGamePlayer::UpgradeProduction(int lProductionType)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_UpgradeProduction reqMsg;
	reqMsg.m_lPlayerID = GetID();
	reqMsg.m_lProductionType = lProductionType;

	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_UPGRADEPRODUCTION, &reqMsg);

	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
//---------------------------------------
void LC_ClientGamePlayer::LearnFormula(int lFormulaID)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_LearnFormula reqMsg;
	reqMsg.m_lPlayerID = GetID();
	reqMsg.m_lFormulaID = lFormulaID;

	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_LEARNFORMULA, &reqMsg);

	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
//---------------------------------------

void LC_ClientGamePlayer::ReUse()
{
	LC_PlayerBase::ReUse();
	m_bIsCreateVisibleData = false;

	m_lUserID = 0;

	//清空寻路数据
	m_kMovePath.Reset();
	ClearMoveStride();
	ClearPathFinding();
	// 重置寻路标志
	m_bArriveFlag = true;

	//清空组队相关数据
	ClearInvitePlayerGroup();

	// 重置好友信息
	m_kClientSocietyAsset.ReUse();
	m_kClientFriendMsgBroad.ReUse();
	m_kClientSearchPlayer.ReUse();
	m_kClientFriendNick.ReUse();

	m_kClientWorldLevelAsset.ReUse();
	m_kPlayerEquipSubSuitPtrMap.clear();
	m_BestGuildTowerInfo.clear();
	m_BestGuildFortressInfo.clear();
	m_BestGuildRoyalInfo.clear();
	m_mBestGuildGoalInfo.clear();
	m_kBossFirstKill.clear();
	m_mBossFirstKillRewardPlayerInfo.clear();
	m_kServerBossFirstKillRecord.clear();
	m_mBossFirstKillRecord.clear();
	m_kEnterMapRecord.clear();
	m_kCanEnterMapIDSet.clear();

	// 重置邮件
	LC_ClientMailManager* pkMailManager = LC_ClientMailManager::GetSingletonPtr();
	if (pkMailManager)
	{
		pkMailManager->Reset();
	}

	// 重置拍卖信息
	LC_ClientAuctionManager* pkAuctionManager = LC_ClientAuctionManager::GetSingletonPtr();
	if (pkAuctionManager)
	{
		pkAuctionManager->Reset();
	}

	//// 重置游戏币交易
	//LC_ClientCurrencyTradeManager* pkCurrencyTradeManager = LC_ClientCurrencyTradeManager::GetSingletonPtr();
	//if (pkCurrencyTradeManager)
	//{
	//	pkCurrencyTradeManager->Reset();
	//}

	// 重置门派信息
	LC_ClientGuildManager* pkGuildManager = LC_ClientGuildManager::GetSingletonPtr();
	if (pkGuildManager)
	{
		pkGuildManager->GetGuildAsset()->ReUse();
	}

	m_pkClientProduction->Reset();

	m_fLastSyncFovTime = 0;

	m_bIsCanStartTutorial = false;
	m_fLastUpdateTime = 0.0f;
	m_fLastUpdateProcessingTask = 0.0f;
	m_fLastUpdateTradeStateTime = 0.0f;
	m_currentGameServerPing = 0;
	m_currentGateWayPing = 0;
	m_currentSocketPing = 0;
	m_token = 0;
	//UI操作记录
	memset(m_auiUIOperationRecords,0,sizeof(uint16_t) * 64);

	m_lLastLockTargetID = 0;
	m_lLastLockTargetNeedGfx = 0;
	m_lWatchTargetID = 0;
	m_lLastWatchTargetID = 0;
	m_lMouseOverActorID = 0;
	m_lLastMouseOverActorID = 0;
	m_bWatchTargetFactionChange = false;
	this->RestoreTarget();
	m_lPreLockedTargetID = 0;
	m_pPreLockedTargetModel = 0;
	m_bIsFreezeLock = 0;

	m_iReqNoRltExecSkillID = 0;
	m_fReqExecSkillTime = 0;
	m_iSkyArenaPoint = 0;
	m_iSkyArenaHonorPoint = 0;
	m_iLastSkyArenaPoint = 0;
	m_iSkyArenaSeason = 0;
	m_iSkyArenaCloseTime = 0;
	m_iSkyArenaNxtTime = 0;

	//反隐的参数
	m_kRevealInvisibilityParams.ReUse();

	m_pkScriptControllerFunctor = NULL;

	m_fLastRequestOwnerShipTime = 0.0f;

	m_iNewPlayerLevel			= 0;

	m_fCurrentHookTime =0.0f;
	m_fCurrentHookUpdateTime = GET_PROCESS_TIME;
	m_fTotalHookTime =3500.0f;
	m_fCurrentHookSecond =0.0f;
	m_fCurrentHookEfficiency =1.0f;
	m_fDeleteHookItemTime = 0.0f;
	m_fCalcExpTime=0.0f;
	m_fCurrentHookTotalTime =0.0f;
	m_bMoveStride = false;
	m_bWASDStride = false;
	m_bTutorialStop = false;

	m_lHookForbidCount = 0;

	//pvp战斗状态下播放屏幕泛红特效的id
	m_iPVPGfxID = 0;

	m_iPowerGuildId = 0;
	m_iGuildWarType = 0;

	//攻击者列表
	m_kLastAttackerID.clear();

	m_lastWeaponItemID =0;
	m_lastBowLevel = 0;
	m_lastWingCharType = 0;
	this->_clearEyeAvatar();
	m_kArenaBattleChallengeInfoAsset.Reset();

	LC_ClientOpenAbilityManager* pkClientOpenAbilityManger = GetOpenAbilityManager();
	if (pkClientOpenAbilityManger != NULL && pkClientOpenAbilityManger->Instance() != NULL)
	{
		pkClientOpenAbilityManger->Reset();
	}
	//if (GetGameStoryManager())
	//	GetGameStoryManager()->Reset();

	LC_ClientToyPetManager* pkToyPetMgr = LC_ClientToyPetManager::GetSingletonPtr();
	if(pkToyPetMgr != NULL)
	{
		pkToyPetMgr->Reset();
	}

	ResetLastEquipData();
	m_iCrossRealMapTeamID = 0;
	m_iCrossRealTeamMapID = -1;

	m_bEnableAfkToMeditation = true;

	m_fStandToHookTimer = 0;
	m_bEnableStandToHook = true;

	m_bAutoPicking = false;

	m_mapInvalidPickItem.clear();
	m_mapInvalidPickItemTick.Start(0.0f);

	m_normalContinueCheck = false;

	m_kDoubleTerrienClearPath.ResetTimer();

	StopGuide();
	Reset2Guding();

	m_iAutoTaskType = STT_NONE;
	m_bStartTriggerType = false;
	m_bRequestLogicTrigger = false;
	m_bAlreadyReqTransfer = false;

	m_kTaskState.clear();

	m_kSpecialHookCharType = 0;

	m_iRaidHookFlag;
	m_kRaidHookAsset.ReUse();

	m_bImmediateTaskStart = false;

	m_bHookGodUsageTime = 0.0f;

	m_lpickMoveFlag = false;

	m_hookRespawnFlag = false;
	m_hookRespawnPathFlag = false;
	m_hookRespawnDeadFlag = false;
	m_bhookRespawnBuyFlag = false;

	m_needTargetMessgeTick = 0.0f;
	m_fSkillCdMessageTick = 0.0f;

	m_obtainFlag = false;
	m_opFinishDailyTask = WOGT_NONE;
	m_bDoingOtherTask = false;
	m_bFinishTaskFlag = false;
	m_iAutoTaskType = STT_NONE;

	m_bInitAttribute = false;  

	SetjumpTarget(UT_Vec3Int());

	ClearJumpPathPendingAction();

	m_kClientAlliance.ReUse();

	m_lastVehicleCharType = 0;

	m_lastJCCharType = 0;
	m_lastJCLevel = INVALID_FAOBAO_LEVEL;
	m_lastJCSetLevel = INVALID_FAOBAO_LEVEL;
	m_idleTime = 0;

	m_lastMainTaskID = 0;
	m_lastMainTaskLevel = 0;
	m_lastMainTaskTick = 0;

	m_lastLCID = 0;
	m_lastLCCharType = 0;

	m_lastLCWeaponType = 0;

	m_lastAnQiCharType = 0;
	m_lastAnQiID = 0;
	m_vehicleZFix = 0;

	m_kSoulRefineAsset.Reset();
	m_kActivityAsset.Reset();
	m_mBuddyCombat.clear();
	m_kFirstDailyRecord.clear();
	m_kSuperShopCompete.clear();
	m_kSuperShopSub.clear();

	m_kPendingVal = 0;
	m_iResourceRetrieveLevelRecord = 0;

	m_platforminfo.clear();

	SetPostEnterGameFlag(true);
	m_kSecretTreasureAsset.Clear();
	m_ActiveCodeUseCount.clear();
	m_ActiveStarVeinIDs.clear();
	m_ActiveStarVeinGoalIDs.clear();
	m_HolydeedUserCount.clear();
	v_InvestmentType.clear();
	v_InvestmentFreeReward.clear();
	v_InvestmentHighReward.clear();
	for(int i=0; i<MAX_ACTIVE_BUDDY_NUM; ++i)
		m_kSkillBookSlotData[i].clear();

	m_kActivityAsset.Clear();

	ResetContronlID();

	m_lSaveDeadTime = 0;

	m_bTranToTranTick = 0;

	m_vActiveBuddyList.clear();
	m_vInactiveBuddyList.clear();
	m_nDailyActivityRewardID.clear();
	m_vAtvChargePoint.clear();
	m_mBuddyInstanceMap.clear();
	m_pkCommander->Reuse();
	m_pkCommander->InitEquipList();
	m_uiCurrentActiveBuddyIndex = -1;
	ClearDirtyFlag();
	ClearDirtyFlagRevisionID();
	m_iUnlockSlotNum = 0;
	m_iUnlockSlotInit = 0;
	m_bActivateBuddyFlag = false;
	m_iPendingSwapIndex = 0;
	m_iPendingActivateCID = -1;
	ResetSwapCoolDown();

	m_kCharacterUpgradGift.clear();
	m_mCommanderEquipSuitCollection.clear();
	m_mResourceRetrieveRecord.clear();
	m_mResourceRetrieveMapRecord.clear();
	m_mSchemeInfoMap.clear();
	m_kRuneRecordMap.clear();
	m_mBossKillerInfo.clear();

	m_dayFlag = 0;
	m_dayTime = 0;

	m_bEnterGameFlag = true;
	m_iSchemeType = 0;
	m_HookingJump.Clear();

	m_bInParalle = false;
	m_fAutoPathSoundStartTime = 0;
	m_iLotteryConsumeUnbindyuanbao = 0;
	m_iCurVipNewLevel=999;
	m_ShowEnemyPlayers.resize(3,0);
	m_mCommanderFrame.clear();
	m_mCommanderHead.clear();

	std::fill(m_vBuddyChainVec.begin(), m_vBuddyChainVec.end(), 0);
	int ChapterCount=CF_GloryExpedition::GetSingleton().GetEntryCount();
	m_iRaidGloryRewardRecord.clear();
	m_iRaidGloryChapterStarNum.clear();
	m_iRaidGloryRewardRecord.resize(ChapterCount*3,false);
	m_iRaidGloryChapterStarNum.resize(ChapterCount,0);
	m_iRaidGloryTotalStarNum=0;
	m_iGloryPointNextRecoverTime=0;
	m_iGloryPointEndRecoverTime=0;
	m_nQuickRaidGloryCountMap.clear();
	m_kraidAssert.Reset();

	int iMaxStage=CF_YiJiTuXiStageReward::GetSingletonPtr()->GetEntryCount();
	m_nYijiTuxiRewardIDRecord.resize(iMaxStage,false);
	iMaxStage=CF_YiJiXunBaoStageReward::GetSingletonPtr()->GetEntryCount();
	m_nYijiXunbaoRewardIDRecord.resize(iMaxStage,false);
	iMaxStage=CF_PetIslandStageReward::GetSingletonPtr()->GetEntryCount();
	m_nToyIslandRewardIDRecord.resize(iMaxStage,false);

	m_iRotaryRewardIdx=-1;
	m_iRotaryRewardValidTimes=0;			//抽奖次数
	m_bChargedToday = false;
	m_RotaryRewardRecord.clear();
	m_bSkipAnimation = false;

	/*m_nNonpareilRecord.clear();
	m_nNonpareilReturnRecord.clear();*/

	m_iChainFlag = 0;
	m_NotifyInGame = -1;

	m_iRegularBossGroupID = 0;
	m_iRegularBossGroupCDTime = 0;
	m_bSwitchLine = 0;
	m_bIsSwitich = false;
	m_iGloryDailRewardDrawed = 0;
	m_bCommanderTitleOpen = false;
	m_processPathFlag = false;

	m_FirstChargeTime=0;
	m_FirstChargeRewardRecord.clear();

	m_iNewAchievementStarNum = 0;
	m_vNewAchievementRewardCord.clear();

	m_bGuildProfitShowName = false;
	m_iGuildProfitLevel = 0;
	m_iGuildProfitLevelExp = 0;
	m_iGuildProfitCrystalLevel = 0;
	m_iGuildProfitCrystalLevelExp = 0;
	m_vGuildProfitCrystal.clear();
	m_mGuildBossActivity.clear();
	m_sGuildBossActivityFinishMap.clear();
	m_vGuildBossActivityRankThree.clear();
	m_vGuildBossActivityRankList.clear();
	m_kSingleCombatData.clear();
	m_kRaidBossDropData.clear();

	m_iAnimIndex = -1;
	m_PickActorID = 0;
	m_iFinishMainTaskID = 0;
	m_vServerPlayerBossFlagInfo.clear();
	m_vMapOffensiveSchemeData.clear();
	m_vMapDefenceSchemeData.clear();
	m_iBattleRecordNum = 0;
	m_iBattleType = 0;
	m_mNpcCount.clear();
	m_BattleLogList.clear();
	m_PeakBattleInfo.clear();
	m_SoloRwd.clear();
	m_SkyArenaSettle.Reset();

	m_iCurrentLineNum = -1;
	m_iSkyArenaRankRewardId=0;
	m_iMaxScore=0; 
	m_iStartTime=0;
	m_iEndTime=0;
	m_stageType=0;
	m_showTime=0;
	m_battleStTime=0;
	m_myRank=0;
	m_zoneId=0;
	m_stageSetEndTime=0;
	m_stageStTime=0;
	m_stageEndTime=0;
	m_nTotalGroupId=0;
	m_summitRedPoint=false ; 
	m_myTopType=0;
	m_bAcceptTaskLogin = false;
	m_isValid=false ; 
	m_invitedType=0;

	m_leftTimes=0; 
	m_buyTimes=0; 
	m_Rate=0 ; 
	againstList.clear();
	allGroupData.clear(); 
	myGroupData.clear() ; 

	m_nBuyNoPayList.clear();
}
//------------------------------------------------------------------------
void	LC_ClientGamePlayer::RequestSetTitle(int iTitleType,int iTitle)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	// 	StringType strTitle = m_kTitleAsset.GetTitle(iTitleType,iTitleIndex);
	// 	if (strTitle.empty())
	// 	{
	// 		SendLogicInfoMessage(RE_TITLE_NOT_EXIST);
	// 		return;
	// 	}

	if (m_kTitleAsset.GetCurrentTitleType() != iTitleType
		|| m_kTitleAsset.GetCurrentTitle() != iTitle)
	{
		MG_REQ_SetTitle msg;
		msg.m_lPlayerID = GetID();
		msg.m_iSetTitleType = iTitleType;
		msg.m_iSetTitleIndex = iTitle;
		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SET_TITLE, &msg);

		if(bCreateMsg)
			m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}

bool LC_ClientGamePlayer::NeedRefreshTitleList()
{
	if(LC_TitleManager::GetSingletonPtr())
		return LC_TitleManager::GetSingleton().NeedRefreshTitleList();
	return false;
}
//------------------------------------------------------------------------
// void	LC_ClientGamePlayer::RequestAddDIYTitle(StringType strDIYName)
// {
// 	MG_PACKET_ENCODE_HANDLER hMsgHandler;
// 	bool bCreateMsg = false;
// 
// 	int iDIYTitle = GetTitleNum(TNT_DIY_TITLE);
// 	if (iDIYTitle >= 10)
// 	{
// 		SendLogicInfoMessage(RE_TITLE_DIY_IS_FULL);
// 		return;
// 	}
// 
// 	TT_TextFilter* pkTextFilter = TT_TextFilter::GetSingletonPtr();
// 	if (pkTextFilter)
// 	{
// 		pkTextFilter->SetText(strDIYName);
// 		if ( pkTextFilter->FindAndFilterForbiddenText(TT_TextFilter::TFT_CHAR_NAME,false,true))
// 		{
// 			SendLogicInfoMessage(RE_TITLE_DIY_FORBID);
// 			return;
// 		}
// 	}
// 
// 	MG_REQ_AddDIYTitle msg;
// 	msg.m_lPlayerID = GetID();
// 	STRNCPY(msg.m_cTitle,strDIYName.c_str(),sizeof(msg.m_cTitle));
// 	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_ADD_DIY_TITLE, &msg);
// 
// 	if(bCreateMsg)
// 		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
// }
//------------------------------------------------------------------------
void  LC_ClientGamePlayer::RequestCollectItem(int lID)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_CollectItem reqMsg;
	reqMsg.m_lPlayerID = GetID();
	reqMsg.m_lOBJID = lID;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_COLLECT_ITEM, &reqMsg);

	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
//------------------------------------------------------------------------
bool LC_ClientGamePlayer::OnEnterPhaseMap(int iMapResID)
{
	CF_WorldMapList::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(iMapResID);
	if(pkData == NULL || !pkData->_bBossBornAction)
	{
		return false;
	}

	SetAIControl(true);
	return true;
}

ResultType LC_ClientGamePlayer::RequestTalkToNPC(object_id_type lTargetID)
{
	if (!IS_NPC_ID(lTargetID) && !IS_LOCAL_NPC_ID(lTargetID))
	{
		return RE_FAIL;
	}

	LC_NPCManagerBase* pkNPCManager = CLIENT_GET_NPC_MANAGER();
	LC_NPCBase* pkNPC = pkNPCManager->FindNPC(lTargetID);
	if (NULL == pkNPC)
	{
		return RE_FAIL;
	}

	if(IS_LOCAL_NPC_ID(lTargetID))
	{
		TalkToNPC(lTargetID);
		return RE_SUCCESS;
	}

	//如果不需要检查npc的归属，则先与npc交互
	if (!pkNPC->GetProperty_NeedCheckOwnerShip())
	{
		TalkToNPC(lTargetID);
	}

	MG_REQ_TalkToNPC reqMsg;
	reqMsg.m_lPlayerID = GetID();
	reqMsg.m_lNPCID = lTargetID;
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_TALKTONPC, &reqMsg);
	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	return RE_SUCCESS;
}
//------------------------------------------------------------------------------
// void LC_ClientGamePlayer::HandlerGameEvent( LC_GameEvent& rGameEvent,StringType& rEventStringParam)
// {
// 	m_kGameEventHandler.HandlerGameEvent(rGameEvent,rEventStringParam,this);
// }
//------------------------------------------------------------------------------
void LC_ClientGamePlayer::AddTempFriend(unique_id_type lCharID, const StringType& strName, int lCareer, int lLevel)
{
	// 直接加入客户端之中
	if(GetSocietyAsset()->GetFriendInfo1(lCharID))
		return;

	PS_SocietyBasicInfo kSocietyInfo;
	kSocietyInfo.uiCharID = lCharID;
	TPSTR2STDSTR(strName, kSocietyInfo.szCharName);
	kSocietyInfo.iCareer = static_cast <int8_t> (lCareer);
	kSocietyInfo.iLevel  = static_cast <int16_t> (lLevel);

	bool bAdd = GetSocietyAsset()->UpdateSociety(ST_TEMP_FRIEND, kSocietyInfo);
	if (bAdd)
	{
		MG_REQ_PlayerDeltaData kMsg;
		kMsg.m_lTargetCharID = lCharID;

		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_PLAYER_DELTADATA, &kMsg);
		if (bCreateMsg)
		{
			m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
		}

		//SendLogicInfoMessage(RE_ADD_TEMP_FRIEND_SUCCESS, strName);

		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_ADD_TEMP_FRIEND_SUCCESS);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
}

void LC_ClientGamePlayer::AddFriend(unique_id_type lCharID, const StringType& strName,const StringType& strAskMsg)
{
	if(lCharID == GetCommanderUID())
	{
		SendLogicInfoMessage(RE_ADD_FRIEND_FAILED_CANT_SELF_ADD);
		return;
	}

	MG_REQ_AddFriend msg;
	msg.m_lTargetCharID = lCharID;
	TPSTR2STDSTR(strName, msg.szCharName);
	TPSTR2STDSTR(strAskMsg, msg.m_szMsg);

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_ADD_FRIEND, &msg);
	if (bCreateMsg)
	{
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}

void LC_ClientGamePlayer::AddEnemy(unique_id_type lCharID, const StringType& strName)
{
	MG_REQ_AddEnemy msg;
	msg.m_lTargetCharID = lCharID;
	TPSTR2STDSTR(strName, msg.szCharName);

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_ADD_ENEMY, &msg);
	if (bCreateMsg)
	{
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}

void LC_ClientGamePlayer::AddBlackList(unique_id_type lCharID, const StringType& strName)
{
	if (GetSocietyAsset()->GetFriendInfo1(lCharID) != NULL)
	{
		DeleteFriend(lCharID);
	}

	MG_REQ_AddBlackList msg;
	msg.m_lTargetCharID = lCharID;
	TPSTR2STDSTR(strName, msg.szCharName);

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_ADD_BLACKLIST, &msg);
	if (bCreateMsg)
	{
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}


void LC_ClientGamePlayer::DeleteTempFriend(unique_id_type lCharID)
{
	StringType strName;
	LC_SocietyBasicInfo* pkFriendInfo = GetSocietyAsset()->GetTempFriendInfo( lCharID);
	if(NULL == pkFriendInfo)
	{
		SendLogicInfoMessage(RE_DEL_TEMP_FRIEND_FAILED_IS_NOT_EXIST, strName);
		return;
	};

	strName = pkFriendInfo->GetCharName();

	ResultType iResult = GetSocietyAsset()->DeleteSociety(ST_TEMP_FRIEND, lCharID);

	SendLogicInfoMessage(iResult, strName);
}

void LC_ClientGamePlayer::DeleteFriend(unique_id_type lCharID)
{
	MG_REQ_DelFriend msg;
	msg.m_lTargetCharID = lCharID;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_DEL_FRIEND, &msg);
	if (bCreateMsg)
	{
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}

void LC_ClientGamePlayer::DeleteEnemy(unique_id_type lCharID)
{
	MG_REQ_DelEnemy msg;
	msg.m_lTargetCharID = lCharID;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_DEL_ENEMY, &msg);
	if (bCreateMsg)
	{
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}

void LC_ClientGamePlayer::DeleteBlackList(unique_id_type lCharID)
{
	MG_REQ_DelBlackList msg;
	msg.m_lTargetCharID = lCharID;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_DEL_BLACKLIST, &msg);
	if (bCreateMsg)
	{
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}

void LC_ClientGamePlayer::ReqPlayerInfoNearBy()
{
	MG_REQ_GetPlayerNearby msg;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GET_PLAYER_NEARBY, &msg);
	if (bCreateMsg)
	{
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}

void LC_ClientGamePlayer::ReqChangeSocietyInfo(int enumChange, const StringType& var1, int var2)
{
	MG_REQ_Change_SocietyInfo msg;
	msg.m_cChangeType = enumChange;
	if ( enumChange == SICT_Mood)
	{
		StringType filterStr = var1;
		TT_TextFilter* pkTextFilter = TT_TextFilter::GetSingletonPtr();
		if (pkTextFilter)
		{
			pkTextFilter->SetText(filterStr);
			pkTextFilter->FindAndFilterForbiddenText(TT_TextFilter::TFT_CHAT);
			filterStr = pkTextFilter->GetText();
		}
		TPSTR2STDSTR(filterStr, msg.m_szValue);
	}
	else if (enumChange == SICT_Status)
	{
		msg.m_Value = var2; 
	}

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_CHANGE_SOCIETY_INFO, &msg);
	if (bCreateMsg )
	{
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}

void LC_ClientGamePlayer::ReqFriendStatus(int iSocietyType)
{
	MG_REQ_GetFriendStatus msg;
	msg.m_uiSocietyType = iSocietyType;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GET_FRIEND_STATUS, &msg);
	if (bCreateMsg)
	{
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}

	ReqSocietyDeltaData(iSocietyType);
}

void LC_ClientGamePlayer::ReqSocietyDeltaData(int iSocietyType)
{
	MG_REQ_SocietyDeltaData kMsg;

	kMsg.m_uiSocietyType = iSocietyType;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SOCIETY_DELTADATA, &kMsg);
	if (bCreateMsg)
	{
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}

void LC_ClientGamePlayer::ReqRecommendFriend()
{
	MG_REQ_Recommend_Friend reqMsg;
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler( hMsgHandler, MGPT_REQ_FRIEND_RECOMMEND_FRIEND, &reqMsg);
	if (bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

ResultType LC_ClientGamePlayer::ReqAddAllRecommendFriends(const StringType& sPlayerUids)
{	
	TStringVector kVector = UT_BaseStringUtility::SplitString(sPlayerUids, '#');
	if(kVector.size() == 0)
		return RE_FAIL;;

	MG_REQ_AddRecommendFriend reqMsg;
	for (TStringVector::iterator it = kVector.begin(); it != kVector.end(); ++it)
	{
		reqMsg.m_lTargetCharIDList.push_back(_atoi64(it->c_str()));
	}

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler( hMsgHandler, MGPT_REQ_ADD_RECOMMEND_FRIEND, &reqMsg);
	if (bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	return RE_SUCCESS;
}

void LC_ClientGamePlayer::ReqFriendDetail(unique_id_type lCharID)
{
	MG_REQ_GetFriendDetail kMsg;
	kMsg.m_uiCharID = lCharID;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GETFRIEND_DETAIL, &kMsg);
	if (bCreateMsg)
	{
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}

void LC_ClientGamePlayer::AddFiendNick(unique_id_type lFriendID, const StringType& strName)
{
	MG_REQ_AddFriendNick msg;
	msg.m_lSrcCharID = GetCommanderUID();
	msg.m_lTargetCharID = lFriendID;
	TPSTR2STDSTR(strName, msg.m_szNick);

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_ADD_FRIENDNICK, &msg);
	if (bCreateMsg)
	{
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}

void LC_ClientGamePlayer::SupportFiendNick(unique_id_type lFriendID, int iIndex)
{
	MG_REQ_SupportFriendNick msg;
	msg.m_lTargetFriendID = lFriendID;
	msg.m_lIndex = iIndex;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SUPPORT_FRIENDNICK, &msg);
	if (bCreateMsg)
	{
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}

void LC_ClientGamePlayer::ReqGetFriendNick(unique_id_type lFriendID)
{
	MG_REQ_GetFriendNick msg;
	msg.m_lTargetFriendID = lFriendID;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GETFRIENDNICK, &msg);
	if (bCreateMsg)
	{
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}

void LC_ClientGamePlayer::CopyName(const StringType& strName, const StringType& strUserID)
{
	if (strName.empty() && strUserID.empty())
	{
		return;
	}

	bool bResult = false;
	if (m_pkSMController->IsLControlPressed())
	{
		bResult = CopyToClipBoard(strUserID);
	}
	else
	{
		bResult = CopyToClipBoard(strName);
	}
	if (bResult)
	{
		SendLogicInfoMessage(RE_COPY_NAME_SUCCESS);
	}
}

bool LC_ClientGamePlayer::CopyToClipBoard(const StringType& ClipStr)
{
	if (ClipStr.empty())
	{
		return false;
	}

	return CodeConvert::copyToClipBoard(ClipStr);
}
void LC_ClientGamePlayer::ReqBeAddFriend(unique_id_type lTargetID)
{
	if (lTargetID == INVALID_ACTOR_ID)
	{
		return;
	}

	MG_REQ_BeAddFriend msg;
	msg.m_lTargetCharID = lTargetID;	
	//STRNCPY(msg.m_szName, GetCharName.c_str(), MAX_CHAR_NAME_LENGTH);

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_BEADDFRIEND, &msg);
	if (bCreateMsg)
	{
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}

void LC_ClientGamePlayer::ConfirmBeAddFriend(unique_id_type lTargetID, bool bAgree, const StringType& TargetName)
{
	MG_RLT_Confirm_BeAddFriend msg;
	msg.m_lTargetCharID = lTargetID;
	msg.m_bAgree = bAgree;
	msg.m_szCharName = TPSTR2STDSTR(TargetName);

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_RLT_CONFIRM_BEADDFRIEND, &msg);
	if (bCreateMsg)
	{
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}

void LC_ClientGamePlayer::AccurateSearch(const StringType& strPlayerName, unique_id_type iCharID, const StringType& strAskMsg)
{	
	if (strPlayerName.length() == 0)
	{
		return;
	}

	NW_ClientSessionManager* pkSessionManager = NW_ClientSessionManager::GetSingletonPtr();
	if(pkSessionManager)
	{
		NW_NetCharSimpleInfo* pkCharSimpleInfo = pkSessionManager->FindNetCharSimpleInfo(strPlayerName);
		if(pkCharSimpleInfo && pkCharSimpleInfo->m_iProfileResult == RE_SUCCESS)
		{
			AddFriend(pkCharSimpleInfo->m_lCharID,strPlayerName, strAskMsg);
		}
		else
		{
			// 加入的需要添加好友的队列
			m_kClientSocietyAsset.AddAccurateSearchName(strPlayerName);
			m_kClientSocietyAsset.AddAccurateSearchMsg(strPlayerName, strAskMsg);
		}
	}
}

bool LC_ClientGamePlayer::PrivateChatSearch(const StringType& strChatMsg, int cLenght)
{
	bool flag = false;
	if (GetChatTargetName().length() == 0)
	{
		return flag;
	}

	NW_ClientSessionManager* pkSessionManager = NW_ClientSessionManager::GetSingletonPtr();
	if(pkSessionManager)
	{
		NW_NetCharSimpleInfo* pkCharSimpleInfo = pkSessionManager->FindNetCharSimpleInfo(GetChatTargetName());
		if(pkCharSimpleInfo && pkCharSimpleInfo->m_iProfileResult == RE_SUCCESS)
		{
			SetChatTargetID(pkCharSimpleInfo->m_lCharID);
			flag = true;
		}
		else
		{
			PrivateChatSaveSt& st = GetSearchingChatTarget();
			st.targetName = GetChatTargetName();
			st.channel = GetChatChannel();
			st.msgtxt = strChatMsg;
			flag = false;
		}
	}
	return flag;
}

void LC_ClientGamePlayer::ResetDetailInfo()
{
	GetViewPlayerInfo()->Reset();
}

void LC_ClientGamePlayer::PendingPathToLocation(int iMapResID, int x, int y, int z, int iMapLogicID)
{
	PendingPathToLocation(iMapResID, UT_Vec3Int(x, y, z), iMapLogicID);
}

void LC_ClientGamePlayer::PendingPathToLocation( int iMapResID , const Utility::UT_Vec3Int& pos , int iMapLogicID)
{
	m_iPendingPathMapResID = iMapResID;
	m_kPendingPathPos	= pos;

	//这里为了不改变原来的逻辑，增加一层保护
	LC_ClientNewBeeManager *pkNewBeeManager = LC_ClientNewBeeManager::GetSingletonPtr();
	if (pkNewBeeManager && pkNewBeeManager->IsMapLogicIDIsMapResID(iMapResID, iMapLogicID))
	{
		m_iPendingPathMapLogicID = iMapLogicID;
	}
	else
	{
		m_iPendingPathMapLogicID = 0;
	}
}

void LC_ClientGamePlayer::PendingPathToVal(int iMapResID , int x, int y, int z, int val, int iMapLogicID)
{
	m_kPendingVal	= val;
	this->PendingPathToLocation(iMapResID, UT_Vec3Int(x, y, z), iMapLogicID);
}

void LC_ClientGamePlayer::ClearPendingPathFind()
{
	m_kPendingVal = 0;
	m_iPendingPathMapResID = 0;
	m_kPendingPathPos.clear();
}

void LC_ClientGamePlayer::SetPathFinding(bool bFinding)
{
	if ( m_bIsPathFinding && !bFinding ) 
	{
		SetPathtoTreasureFlag(false);
	}
	if(!m_bIsPathFinding && bFinding)
		m_fAutoPathSoundStartTime = 0;

	m_bIsPathFinding = bFinding;	
	GetRaidHookAsset().SetRaidHookPathFind(false); 
}

void LC_ClientGamePlayer::PendingPathAction(PathPendingActionType t, int iTargetType, bool bSwitchNewBee /*= false*/, int iTargetMapLogicID /*= 0*/)
{
	m_kPPAArg.enmType = t;
	m_kPPAArg.iTargetCharTypeID = iTargetType;
	m_kPPAArg.bNewBeeSwitch = bSwitchNewBee;
	m_kPPAArg.iTargetMapLogicID = iTargetMapLogicID;
}

void LC_ClientGamePlayer::ClearPathPendingAction(void)
{
	PendingPathAction(PPAT_NA, 0);
	SetPathFinding(false);
	m_kDoubleTerrienClearPath.ResetTimer();
}

void LC_ClientGamePlayer::PlayBossWarningFx()
{
	GD_ClientActorModel* pkVisibleData = IM_Manager::GetSingletonPtr()->GetActor(GetID());
	if (pkVisibleData != NULL)
		pkVisibleData->AttachFx(GD_ClientActorModel::FDT_GROUND, FileSystem::MTF_ModelData::AFBM_TRANSLATE_ROTATE, false, RES_FX_BOSS_WARNING);

	g_soundMgr.Play2DEx( "Boss_warning" , AS_ClientSoundManager::SCT_2D );
}

void LC_ClientGamePlayer::PlayDistortionScreenFx()
{
	GD_ClientActorModel* pkVisibleData = IM_Manager::GetSingletonPtr()->GetActor(GetID());
	if (pkVisibleData != NULL)
	{
		pkVisibleData->AttachFx(GD_ModelBase::FDT_INVALID, FileSystem::MTF_ModelData::AFBM_TRANSLATE_ROTATE, true, RES_FX_SCREEN_DISTORTION);
	}
}

void LC_ClientGamePlayer::MotivateActionForFeiXie()
{
	//飞鞋自动对话的功能
	switch(m_kPPAArgForFeiXie.enmType)
	{
	case PPAT_TALK_TO_NPC:
		{
			bool ptnSuccess = CheckPendingTalkNPC(m_kPPAArgForFeiXie.iTargetCharTypeID, GET_PROCESS_TIME);
			if (!ptnSuccess)
			{
				SetPPAArgRetryID(m_kPPAArgForFeiXie.iTargetCharTypeID);
			}
		}
		break;
	case PPAT_ATTACK_NPC:
		if (!IsHooking())
		{
			SetHooking(true);
		}
		break;
	case PPAT_TRANSFER:
		{
			RequestTranscriptionTransfer(m_kPPAArgForFeiXie.iTargetCharTypeID);
		}
		break;
	}
	ClearPathPendingActionForFeiXie();
}

void LC_ClientGamePlayer::PathToPendingLocation()
{
	SC_ClientScriptInterface* pkClientInterface = SC_ClientScriptInterface::GetSingletonPtr();
	SC_ClientSkillInterface* pkInterface = pkClientInterface->GetSkillInterface();

	if (m_iPendingPathMapResID <= 0)
	{
		if ( SM_WalkTransferManager::GetSingletonPtr()->OnMapChange(GetMapResID()) )
		{
			int curMapResID = 0;
			UT_Vec3Int pt;
			int localNPCID = 0;
			SM_WalkTransferManager::GetSingletonPtr()->GetNextPt(GetMapResID(), curMapResID, pt, localNPCID);
			if ( GetMapResID() == curMapResID )
			{
				PathToPoint(pt);
				if ( localNPCID > 0 )
				{
					PendingPathAction(PPAT_TALK_TO_NPC, localNPCID);
				}
			}
		}
		else if (GetPendingObject() > 0)
		{
			pkInterface->WalkToObject(GetMapResID(), 0, GetPendingObject());
			SetPendingObject(0);
		}
		else
		{
			MotivateActionForFeiXie();
		}
		return;
	}
	
	LC_MapBase* pkMap = GetMap();
	if (pkMap == NULL)
		return;

	if (pkMap->GetMapResID() == m_iPendingPathMapResID)
	{
		if ( m_kPendingVal > 0 )
		{
			pkInterface->PathToObject(m_kPendingVal);
		}
		else
		{
			PathToPoint(m_kPendingPathPos);
			//PendingPathAction(PPAT_ATTACK_NPC,0,false,0);
		}
	}
	else
	{
		UT_Vec3Int pos(0,0,0);
		if( m_kAcrossMapPath.GetTransferPosLocation(pkMap->GetMapLogicID(),pos))
		{
			if ( m_kPendingVal > 0 )
			{
				pkInterface->PathToObject(m_kPendingVal);
			}
			else
			{
				PathToPoint(pos);
			}
		}
		else
			ClearPathFinding();
	}
	ClearPendingPathFind();
}

//Receive Rlt Transcription Msg
void LC_ClientGamePlayer::OnReceiveRLTTranscriptionInfo(Protocol::MG_RLT_ENTER_Transcription &pkMsg)
{
	CF_TranscriptionRule* pkTranscriptionRule = CF_TranscriptionRule::GetSingletonPtr();
	CF_TranscriptionRule::DataEntry* pkData=pkTranscriptionRule->GetEntryPtr(pkMsg.m_lResId);

	if(!pkData)
		return;

	object_id_type lPlayerID = pkMsg.m_lFailurePlayerID;
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkClientPlayer = pkLogicManger->GetOperationPlayer();	

	if(!pkClientPlayer)	return;

	switch(pkMsg.m_iResult)
	{
	case RE_TRANSCRIPTION_GROUP_NUM_TOO_LARGE:
		SendLogicInfoMessage(pkMsg.m_iResult,1,pkData->_iGroupMaxNum);		
		break;
	case RE_TRANSCRIPTION_GROUP_NUM_TOO_LIMITED:
		SendLogicInfoMessage(pkMsg.m_iResult,1,pkData->_iGroupMaxNum);	
		break;
	case RE_SOMEONE_IN_GROUP_ALREADY_IN_TRANSCRIPTION:
		if(pkClientPlayer->GetID() == lPlayerID)
		{
			SendLogicInfoMessage(pkMsg.m_iResult,GET_MSG(LC_CLIENTGAMEPLAYER_CPP_006));
		}
		else
		{			
			SendLogicInfoMessage(pkMsg.m_iResult,STDSTR2TPSTR(pkMsg.m_cFailedPlayerName));
		}
		break;
	case RE_SOMEONE_IS_LOADING_PLEASE_WAIT:
		if(pkClientPlayer->GetID() == lPlayerID)
		{
			SendLogicInfoMessage(pkMsg.m_iResult,GET_MSG(LC_CLIENTGAMEPLAYER_CPP_006));
		}
		else
		{			
			SendLogicInfoMessage(pkMsg.m_iResult,STDSTR2TPSTR(pkMsg.m_cFailedPlayerName));
		}
		break;
	case RE_SOMEONE_IS_TOO_FAR_FROM_CHIEF_PLAYER:
		if(pkClientPlayer->GetID() == lPlayerID)
		{
			SendLogicInfoMessage(pkMsg.m_iResult,GET_MSG(LC_CLIENTGAMEPLAYER_CPP_006));
		}
		else
		{			
			SendLogicInfoMessage(pkMsg.m_iResult,STDSTR2TPSTR(pkMsg.m_cFailedPlayerName));
		}
		break;
	case RE_TRANSCRIPTION_LEVEL_TO_HIGH:
		if(pkClientPlayer->GetMainControllerID() == lPlayerID)
		{
			SendLogicInfoMessage(pkMsg.m_iResult,pkData->_iPlayerMinLevel,pkData->_iPlayerMaxLevel);
		}
		break;
	case RE_TRANSCRIPTION_LEVEL_TO_LOW:
		if(pkClientPlayer->GetMainControllerID() == lPlayerID)
		{
			SendLogicInfoMessage(pkMsg.m_iResult, pkData->_iPlayerMinLevel,pkData->_iPlayerMaxLevel);
		}
		break;
	case RE_ENTER_ERROR_FORTRESS:
		if(pkClientPlayer->GetMainControllerID() == lPlayerID)
		{
			TStringVector kVector = UT_BaseStringUtility::SplitString(pkData->_iEntryID, ';');
			if (kVector.size() > 0)
			{
				SC_ClientScriptInterface* pkClientInterface = SC_ClientScriptInterface::GetSingletonPtr();
				SC_ClientSkillInterface* pkInterface = pkClientInterface->GetSkillInterface();
				StringStreamType sStream;
				for (TStringVector::iterator it = kVector.begin(); it != kVector.end(); ++it)
				{
					sStream << pkInterface->GetNPCName(String2Int(it->c_str()));
					if (it != kVector.end() - 1)
					{
						sStream << "或";
					}
				}
				SendLogicInfoMessage(pkMsg.m_iResult, sStream.str());
			}
		}
		break;
	default:
		SendLogicInfoMessage(pkMsg.m_iResult);
		break;
	}
}

//Receive Rlt Transcription Msg
void LC_ClientGamePlayer::OnReceiveRLTTransferToLocation(Protocol::MG_Rlt_TransferToLocation &pkMsg)
{
	CF_TranscriptionRule* pkTranscriptionRule = CF_TranscriptionRule::GetSingletonPtr();
	CF_TranscriptionRule::DataEntry* pkData=pkTranscriptionRule->GetEntryPtr(pkMsg.m_iMapResID);

	if(!pkData)
		return;
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)(LC_ClientLogicManager::GetSingletonPtr());
	LC_ClientGamePlayer* pkClientPlayer = pkLogicManger->GetOperationPlayer();	

	if(!pkClientPlayer)	return;

	switch(pkMsg.m_uiRslt)
	{
	case RE_TRANSCRIPTION_GROUP_NUM_TOO_LARGE:
		SendLogicInfoMessage(pkMsg.m_uiRslt,1,pkData->_iGroupMaxNum);		
		break;
	case RE_TRANSCRIPTION_GROUP_NUM_TOO_LIMITED:
		SendLogicInfoMessage(pkMsg.m_uiRslt,1,pkData->_iGroupMaxNum);	
		break;
	case RE_SOMEONE_IN_GROUP_ALREADY_IN_TRANSCRIPTION:
		SendLogicInfoMessage(pkMsg.m_uiRslt,GET_MSG(LC_CLIENTGAMEPLAYER_CPP_006));
		break;
	case RE_SOMEONE_IS_LOADING_PLEASE_WAIT:
		SendLogicInfoMessage(pkMsg.m_uiRslt,GET_MSG(LC_CLIENTGAMEPLAYER_CPP_006));
		break;
	case RE_SOMEONE_IS_TOO_FAR_FROM_CHIEF_PLAYER:
		SendLogicInfoMessage(pkMsg.m_uiRslt,GET_MSG(LC_CLIENTGAMEPLAYER_CPP_006));
		break;
	case RE_TRANSCRIPTION_LEVEL_TO_HIGH:
		SendLogicInfoMessage(pkMsg.m_uiRslt, pkData->_iPlayerMinLevel,pkData->_iPlayerMaxLevel);
		break;
	case RE_TRANSCRIPTION_LEVEL_TO_LOW:
		SendLogicInfoMessage(pkMsg.m_uiRslt, pkData->_iPlayerMinLevel,pkData->_iPlayerMaxLevel);
		break;	
	default:
		SendLogicInfoMessage(pkMsg.m_uiRslt);
		break;
	}
}
//Request Leave Transcription
void LC_ClientGamePlayer::RequestLeaveTranscription()
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_QUIT_Transcription msgQuitTranscription;
	msgQuitTranscription.m_lPlayerID				= GetID();
	msgQuitTranscription.m_lTranscriptionMapID		= GetMap()->GetMapLogicID();
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_QUIT_TRANSCRIPTION, &msgQuitTranscription);

	GfxWriteLog(LOG_TAG_GAME_LOG, LOG_SWITCH_GAME,"[RaidMap]LC_ClientGamePlayer::RequestLeaveTranscription CurMapResID=%d", GetMapResID());

	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

void LC_ClientGamePlayer::RequestLeaveArena(int iType, int iOpType)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_Quit_SoloMap kMsg;
	kMsg.m_nPlayType = iType;
	kMsg.m_nOpType = iOpType;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_END_SOLODULE, &kMsg);

	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

//Request Leave Transcription
void LC_ClientGamePlayer::RequestReliveLeaveTranscription()
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_Relive_BeforeEnterMap msgQuitTranscription;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MG_REQ_RELIVE_BEFOREENTERMAP, &msgQuitTranscription);

	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

//---------------------------------------------------------------------------------------------
void LC_ClientGamePlayer::NotifyGameServerAlreadyInCenterInCareerLearnTranscription()
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_NOTIFY_SERVER_ALREADY_IN_CENTER_IN_CAREERLEARN_TRANSCRIPTION pkMsg;
	pkMsg.m_lPlayerID				= GetID();
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_NOTIFY_SERVER_ALREADY_IN_CENTER, &pkMsg);
	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

//---------------------------------------------------------------------------------------------
void LC_ClientGamePlayer::RequestUpgradeNpc(int index)
{
}
//--------------------------------------------------------
void LC_ClientGamePlayer::RequestUpgradeNpcByUI( int index /*= 0*/,int iUpdateType )
{
	if (index < 0 || index >= MAX_TRANSCRIPTION_NPC_UPDATE_INFO_NUM)
		return;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_NPC_UPGRADE msgRequestNpcUpgrade;

	LC_ClientTranscriptionInfoManager* pkTranscriptionInfoManager = CLIENT_GET_TRANSCRIPTION_INFO_MANAGER();
	if(!pkTranscriptionInfoManager)
		return;
	msgRequestNpcUpgrade.m_lPlayerID				= GetID();
	msgRequestNpcUpgrade.m_lNPCID					= pkTranscriptionInfoManager->GetNPCUpdataID(index);
	msgRequestNpcUpgrade.m_lNPCCharType				= pkTranscriptionInfoManager->GetNPCUpdataTypeID(index);
	msgRequestNpcUpgrade.m_lUpdateIndex				= iUpdateType;		
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_NPC_UPGRADE, &msgRequestNpcUpgrade);

	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

//----------------------------------------------------------------------------------------
bool LC_ClientGamePlayer::IsInTranscription()
{	
	int lResMapID = GetMapResID();
	CF_WorldMapList* pkCSVWorldMapList = CF_WorldMapList::GetSingletonPtr();
	CF_WorldMapList::DataEntry* pkData = pkCSVWorldMapList->GetEntryPtr(lResMapID);

	int iMapType = 0;
	if(pkData)
	{
		iMapType =pkData->_iMapLogicType;
	}

	return (iMapType > MT_TRANSCRIPTION_BEGIN && iMapType < MT_TRANSCRIPTION_END);
}
//----------------------------------------------------------------------------------------
bool LC_ClientGamePlayer::IsInSpecialRaidMap()
{	
	SC_ClientScriptInterface* pkClientInterface = SC_ClientScriptInterface::GetSingletonPtr();
	SC_ClientSkillInterface* pkInterface = pkClientInterface->GetSkillInterface();

	LC_ClientMapTransDataEntry* pkMapTransData = pkInterface->GetMapTransDataEntryByMapID(GetMapResID());
	if (!pkMapTransData)
		return false;

	if (LC_Helper::isInTranscript(this) && pkMapTransData->GetIsAllDeadFailed())
		return true;
	else
		return false;
}
//----------------------------------------------------------------------------------------
/*bool LC_ClientGamePlayer::IsNeedUnBindCashComposition(MoneyType ulPrice)
{
if ( ulPrice > GetBindCash().GetMoney() )
{
MoneyType uiNeedBindPrice = ulPrice - GetBindCash().GetMoney();
if ( uiNeedBindPrice > GetUnBindCash().GetMoney() )
{
return false;
}
else
{
return true;
}
}

return false;
}*/
//----------------------------------------------------------------------------------------
void LC_ClientGamePlayer::SetRiderCharType(int lCharType)
{
	if(lCharType == GetRiderCharType())
		return;

	if(GetRiderCharType() > 0)
	{
		this->ChangeVehicleAvatar(0);
		this->DestoryRiderVisibleData();
		m_lastVehicleCharType = 0;
	}

	LC_PlayerBase::SetRiderCharType(lCharType);

	LC_ClientNPCManager* pkmgr = CLIENT_GET_NPC_MANAGER(); 
	LC_ClientNPC* su = (LC_ClientNPC*)pkmgr->FindNPC(m_lastLCID);
	_updateSuitFollow(su);

	if(GetRiderCharType() > 0)
	{
		this->CreateRiderVisibleData();
		this->ChangeVehicleAvatar(this->GetVehicleAvatarID());
		m_lastVehicleCharType = GetCimeliaCharTypeID(LC_SUB_TP_VEHICLE,GetVehicleLevel());
		
	}

	RefreshAnQiZFix(m_lastVehicleCharType);

	if(GetRiderCharType() > 0)
	{
		//显示尘土特效，原本在动画上的
		FX_ClientFxManager* pkFXManager = FX_ClientFxManager::GetSingletonPtr();
		bool bShowSelectionFx = UT_CutSceneManager::GetSingletonPtr()->ShowSelectionFx();
		if(pkFXManager && bShowSelectionFx)
		{
			GD_ClientActorModel* pkVisibleData = IM_Manager::GetSingleton().GetActor(this->GetID());
			if (pkVisibleData)
			{
				int iFXID = pkFXManager->CreateFx(RES_FX_ON_VEHICLE,true);
				pkFXManager->BindFxToDummyNode(iFXID, NiSmartPointerCast(NiAVObject, pkVisibleData->GetRootNode()), false, false);
				pkFXManager->PlayFx(iFXID, GET_PROCESS_TIME);
			}
		}
	}
}
//----------------------------------------------------------------------------------------
void LC_ClientGamePlayer::_initAreaEffectInfo(GD_ClientActorModel& rkVisibleData , bool IsRider )
{
	const CSVFile::CF_AreaEffect::DataEntry* pkAreaEffectData = NULL;
	if ( IsRider )
	{
		object_id_type jc = GetCimeliaCharTypeID(LC_SUB_TP_VEHICLE,GetVehicleLevel());
		pkAreaEffectData = CF_AreaEffect::GetSingleton().GetEntryPtr(jc);	
	}
	else
	{
//ToCheck:
		//原本角色是用charType读表 现在改成默认用1 就避免再配表了
		//pkAreaEffectData = CF_AreaEffect::GetSingleton().GetEntryPtr(m_lCharType);
		pkAreaEffectData = CF_AreaEffect::GetSingleton().GetEntryPtr(1);
	}
	T_ASSERT_CRITICAL(pkAreaEffectData,"坐骑的AreaEffectInfo，没有配置");
	if (pkAreaEffectData) 
	{
		rkVisibleData.SetAreaEffectInfo(MTF_EffectTile::AET_GRASSLAND, 
			pkAreaEffectData->_strGlasslandFx, pkAreaEffectData->_strGlasslandSound);
		rkVisibleData.SetAreaEffectInfo(MTF_EffectTile::AET_SOIL, 
			pkAreaEffectData->_strSoilFx, pkAreaEffectData->_strSoilSound);
		rkVisibleData.SetAreaEffectInfo(MTF_EffectTile::AET_MUD, 
			pkAreaEffectData->_strMudFx, pkAreaEffectData->_strMudSound);
		rkVisibleData.SetAreaEffectInfo(MTF_EffectTile::AET_DESERT, 
			pkAreaEffectData->_strDesertFx, pkAreaEffectData->_strDesertSound);
		rkVisibleData.SetAreaEffectInfo(MTF_EffectTile::AET_SNOWFIELD, 
			pkAreaEffectData->_strSnowfieldFx, pkAreaEffectData->_strSnowfieldSound);
		rkVisibleData.SetAreaEffectInfo(MTF_EffectTile::AET_FLAGSTONE, 
			pkAreaEffectData->_strFlagstoneFx, pkAreaEffectData->_strFlagstoneSound);
		rkVisibleData.SetAreaEffectInfo(MTF_EffectTile::AET_BOARDWALK, 
			pkAreaEffectData->_strBoardwalkFx, pkAreaEffectData->_strBoardwalkSound);
		rkVisibleData.SetAreaEffectInfo(MTF_EffectTile::AET_BAMBOO_BRIDGE, 
			pkAreaEffectData->_strBambooBridgeFx, pkAreaEffectData->_strBambooBridgeSound);
		rkVisibleData.SetAreaEffectInfo(MTF_EffectTile::AET_WATER, 
			pkAreaEffectData->_strWaterFx, pkAreaEffectData->_strWaterSound);
	}
}

void LC_ClientGamePlayer::_clearAreaEffectInfo(GD_ClientActorModel& rkVisibleData)
{
	for (int i = MTF_EffectTile::AET_GRASSLAND; i < MTF_EffectTile::AET_MAX_COUNT; ++i)
	{
		rkVisibleData.SetAreaEffectInfo(MTF_EffectTile::AET_GRASSLAND, "", "");
	}
}

//----------------------------------------------------------------------------------------
void LC_ClientGamePlayer::DisableAreaEffect(void)
{
	if (GetRiderCharType() > 0)
	{
		GD_ClientActorModel* pkRiderVisibleData = IM_Manager::GetSingleton().GetRiderActor(m_iID);
		if (pkRiderVisibleData != NULL)
		{
			pkRiderVisibleData->DisableAreaEffect();
		}
	}
	GD_ClientActorModel* pkVisibleData = IM_Manager::GetSingleton().GetActor(GetID());
	if (pkVisibleData != NULL)
	{
		pkVisibleData->DisableAreaEffect();
	}
}
//----------------------------------------------------------------------------------------
void LC_ClientGamePlayer::MoveToFixedPos(object_id_type npcID, int lParam)
{
	LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
	LC_ClientNPC* pkNPC = (LC_ClientNPC*)pkNPCManager->FindNPC(npcID);
	if(pkNPC)
	{
		/*
		CF_TranscriptionLearn* pkLearnList = CF_TranscriptionLearn::GetSingletonPtr();
		if (pkLearnList)
		{
		CF_TranscriptionLearn::DataEntry * pkEntry = pkLearnList->GetEntryPtr(pkNPC->GetCharType());	
		if(pkEntry)
		{
		SetTargetLocation(pkEntry->_kPos);
		SetMoveStraightFlag(true);
		m_lMoveStraightParam = lParam;
		}
		}
		*/
	}
}

//-----------------------------------------------------------------------------------------
void LC_ClientGamePlayer::ExecuteNPCFuction(int iFuncType, int iFuncID,int iFuncState)
{
	/*if(iFuncType == NFT_ENTER_TYPE_SACREDWEAPON)
	{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_ENTER_SacredWeapon msgTransfer;
	msgTransfer.m_lPlayerID				= GetID();
	msgTransfer.m_lRequestType			= iFuncID;
	msgTransfer.m_lMapLogicType			= iFuncState;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_ENTER_SACREDWEAPON, &msgTransfer);

	if(bCreateMsg)
	m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());	
	}*/
}
//----------------------------------------------------------------------------------------
void LC_ClientGamePlayer::RequestCareerLearn(object_id_type NPCID)
{
	/*
	LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
	LC_ClientNPC* pkNPC = (LC_ClientNPC*)pkNPCManager->FindNPC(NPCID);
	if(pkNPC)
	{
	CF_TranscriptionLearn* pkLearnList = CF_TranscriptionLearn::GetSingletonPtr();
	if (pkLearnList)
	{
	CF_TranscriptionLearn::DataEntry * pkEntry = pkLearnList->GetEntryPtr(pkNPC->GetCharType());	
	if(pkEntry)
	{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_CAREER_SELECTION  msgCareerSelect;
	msgCareerSelect.m_lPlayerID		= GetID();
	msgCareerSelect.m_lNPCID		= pkNPC->GetID();
	msgCareerSelect.m_lLearnType	= pkEntry->_type;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_CAREER_LEARN, &msgCareerSelect);
	if(bCreateMsg)
	m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
	}		
	}
	*/
}

//------------------------------------------------------
object_id_type LC_ClientGamePlayer::GetCareerSelectionNPCID()
{
	object_id_type npcID = _getSelectedNPCIDInCareerLearn();
	return npcID;
}
//-------------------------------------------------------------
int LC_ClientGamePlayer::GetCurrentMapID()
{
	LC_MapBase* pkMap = GetMap();
	if (pkMap == NULL)
		return 0;

	return pkMap->GetMapResID();
}
//-------------------------------------------------------------
int LC_ClientGamePlayer::GetCurrentMapCategoryType()
{
	LC_MapBase* pkMap = GetMap();
	if (pkMap == NULL)
		return -1;

	int lMapResID = pkMap->GetMapResID();
	LC_ClientMapTranscriptionManager* pkMapTransMgr = LC_ClientMapTranscriptionManager::GetSingletonPtr();
	if (NULL == pkMapTransMgr)
	{
		return -1;
	}
	return pkMapTransMgr->GetMapCategoryTypeByMapResID(lMapResID);
}
//
int LC_ClientGamePlayer::GetCurrentMapLogicType()
{
	LC_MapBase* pkMap = GetMap();
	if (pkMap == NULL)
		return -1;

	int lMapResID = pkMap->GetMapResID();
	LC_ClientMapTranscriptionManager* pkMapTransMgr = LC_ClientMapTranscriptionManager::GetSingletonPtr();
	if (NULL == pkMapTransMgr)
	{
		return -1;
	}
	return pkMapTransMgr->GetWorldMapLogicTypeByMapResID(lMapResID);
}
//boss之家用
int LC_ClientGamePlayer::GetWildBossPageByMapID(int lMapResID)
{
	CF_WildPrinces::DataEntryMapExternIterator kIter = CF_WildPrinces::GetSingleton().GetIterator();
	for( ; kIter.HasMoreElements(); kIter.MoveNext() )
	{
		const CF_WildPrinces::DataEntry* pkEntry = kIter.PeekNextValuePtr();
		if (pkEntry->_lMapResID == lMapResID) 
		{
			return pkEntry->_lType;
		}
	}
	return 0;
}
int LC_ClientGamePlayer::GetGamestoryIDByMapID(int lMapResID)
{
	CF_WildPrinces::DataEntryMapExternIterator kIter = CF_WildPrinces::GetSingleton().GetIterator();
	for( ; kIter.HasMoreElements(); kIter.MoveNext() )
	{
		const CF_WildPrinces::DataEntry* pkEntry = kIter.PeekNextValuePtr();
		if (pkEntry->_lMapResID == lMapResID) 
		{
			return pkEntry->_GameStoryID;
		}
	}
	return 0;
}
//-------------------------------------------------------------
ResultType LC_ClientGamePlayer::RequestEnableTalent()
{
	return RE_SUPREMACY_ENABLE_SUCCESS;
}
//-------------------------------------------------------------
ResultType LC_ClientGamePlayer::RequestUpdateTalent(int iFaction, int lFirstLevelTalentID)
{
	return RE_SUPREMACY_UPDATE_TALENT_CHECK_SUCCESS;
}
//-------------------------------------------------------------
ResultType LC_ClientGamePlayer::RequestUpdateAllTalent()
{
	return RE_SUCCESS;
}
//-------------------------------------------------------------
void LC_ClientGamePlayer::_syncFovToServer( float fCurrentTime )
{
	if (fCurrentTime - m_fLastSyncFovTime > 20.0)
	{
		// 		return;
		m_fLastSyncFovTime = fCurrentTime;
		LC_ClientNetPlayerManager* playerManger = CLIENT_GET_NETPLAYER_MANAGER();
		const LC_PlayerIDPtrMap & playerIDMap = playerManger->GetPlayerIDMap();
		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = false;

		if(playerIDMap.size() < MAX_PLAYER_COUNT_IN_FOV - 1)
		{
			MG_GC_REQ_SYNC_PLAYER playerSyncMsg; 
			for (LC_PlayerIDPtrMap::const_iterator it = playerIDMap.begin();
				it != playerIDMap.end(); ++it)
			{
				playerSyncMsg.m_kActorIDs.push_back((*it).first);
			}
			playerSyncMsg.m_kActorIDs.push_back(GetID());

			std::sort(playerSyncMsg.m_kActorIDs.begin(),playerSyncMsg.m_kActorIDs.end());

			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, playerSyncMsg.GetMessageID(), &playerSyncMsg);
			if(bCreateMsg)
				m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
		}

		LC_ClientNPCManager* npcManager =  CLIENT_GET_NPC_MANAGER();
		const LC_NPCIDPtrMap& npcIDMap = npcManager->GetNPCIDMap();

		if(npcIDMap.size() < MAX_NPC_COUNT_IN_FOV - 1)
		{
			MG_GC_REQ_SYNC_NPC npcSyncMsg;
			for (LC_NPCIDPtrMap::const_iterator it = npcIDMap.begin();
				it != npcIDMap.end();++it)
			{
#pragma TODO("临时增加把旗子忽略掉")

				if( (*it).first != (NPC_ID_MAX - 1) )
				{
					npcSyncMsg.m_kActorIDs.push_back((*it).first);
				}
			}

			std::sort(npcSyncMsg.m_kActorIDs.begin(),npcSyncMsg.m_kActorIDs.end());

			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, npcSyncMsg.GetMessageID(), &npcSyncMsg);
			if(bCreateMsg)
				m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
		}
	}
}

//------------------------------------------------------------------------------------------
void LC_ClientGamePlayer::NotifyServerAlreadyInGame()
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_NOTIFY_SERVER_ALREADY_IN_GAME msgNotifyServer;
	msgNotifyServer.m_iPlayerID		= GetID();
	msgNotifyServer.m_iLogicMapID	= GetMapLogicID();
	msgNotifyServer.m_iResMapID = GetMapResID();
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_NOTIFY_SERVER_ALREADY_IN_GAME, &msgNotifyServer);
	
	GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI, "LC_ClientGamePlayer::NotifyServerAlreadyInGame playerID=[%d] logicMapID=[%d] mapResID=[%d]"
		,GetID(), GetMapLogicID(), GetMapResID());
	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	SetPlayBackBegin();

	SetProcessPathFlag(true);
}
//------------------------------------------------------------------------------------------
void LC_ClientGamePlayer::ReqMGPTInGame()
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_IN_GAME, NULL);

	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

}
//------------------------------------------------------------------------------------------
void LC_ClientGamePlayer::SendServerPlayerAntiWallowState(AnitWallowState state )
{
	SetAntiWallowState(state);
	/*
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_Anti_Wallow msgNotifyServer;
	msgNotifyServer.m_anitWallowState= state;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_ANTI_WALLOW, &msgNotifyServer);

	if(bCreateMsg)
	m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	*/
}
//------------------------------------------------------------------------------------------
void LC_ClientGamePlayer::RequestSaveShortcutKey()
{
	LC_ShortcutKeyAsset* pkShortcutKeyAsset = GetShortcutKeyAsset();
	//if (pkShortcutKeyAsset)
	{
		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = false;

		//发送网络消息
		MG_REQ_SaveShortcutKey reqMsg;
		reqMsg.m_lPlayerID		= GetID();
		pkShortcutKeyAsset->SetDataToProtocolStruct(reqMsg.m_kShortcutKeyInfo);
		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SAVE_SHORTCUT_KEY, &reqMsg);
		if(bCreateMsg)
			m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}
//------------------------------------------------------------------------------------------
void LC_ClientGamePlayer::SetWeaponVsible(bool bVisible)
{
	IM_Manager* pkIMManager = IM_Manager::GetSingletonPtr();
	GD_ClientActorModel* pkVisibleData = pkIMManager->GetActor(GetID());
	if (NULL == pkVisibleData)
	{
		return;
	}

	if (GetSkillFlag(SK_DISARM))
	{
		pkVisibleData->SetWeaponVisibility(false);
	}
	else
	{
		pkVisibleData->SetWeaponVisibility(bVisible);
	}
}
//------------------------------------------------------------------------------------------
void LC_ClientGamePlayer::SetBowVsible(bool bVisible, int suitPos)
{
	IM_Manager* pkIMManager = IM_Manager::GetSingletonPtr();
	GD_ClientActorModel* pkVisibleData = pkIMManager->GetActor(GetID());
	if (NULL == pkVisibleData)
	{
		return;
	}

	pkVisibleData->SetBowVisibility(bVisible);
	pkVisibleData->ChangeBowSuitPos((GD_ActorModel::WeaponSuitPos)suitPos);
}
//------------------------------------------------------------------------------------------
void LC_ClientGamePlayer::SetWingVisible(bool bVisible)
{
	IM_Manager* pkIMManager = IM_Manager::GetSingletonPtr();
	GD_ClientActorModel* pkVisibleData = pkIMManager->GetActor(GetID());
	if (NULL == pkVisibleData)
	{
		return;
	}

	pkVisibleData->SetWingVisibility(bVisible);
}
//------------------------------------------------------------------------------------------
void LC_ClientGamePlayer::ReqPing()
{
	_computePing();

	++m_token;
	m_token = m_token % 16;

	DWORD now = GetTickCount();

	PingData kPingData;
	kPingData.uiStartPingTime = now;
	m_pingMap[m_token] = kPingData;

	MG_GC_REQ_SYNC_PING reqMsg;
	reqMsg.m_token = m_token;
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SYNC_PING, &reqMsg);
	BOOL b = false;
	if(bCreateMsg)
	{
		b = m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}

	GfxWriteLog(LOG_TAG_SYSTEM,LOG_SWITCH_SYSTEM,"发送ping消息 result=%d fps=%d", b, GetCurrentFPS());
}
//------------------------------------------------------------------------------------------
void LC_ClientGamePlayer::RltPing(uint16_t usFlag, uint8_t token,uint32_t uiServerTime)
{
	GfxWriteLog(LOG_TAG_SYSTEM,LOG_SWITCH_SYSTEM,"接收ping消息");

	DWORD ping;
	mem::map<uint8_t,PingData>::iterator it = m_pingMap.find(token);

	if (it != m_pingMap.end())
	{
		PingData& rkPingData = (*it).second;

		int now = GetTickCount();
		ping = now - rkPingData.uiStartPingTime;
		DWORD nOrigPing = ping;
		ping /= 2;

		if ( nOrigPing > g_settings.DS_MONITOR_PING_LAGGING )
		{
			GfxWriteLog(LOG_TAG_WARNING, LOG_SWITCH_WARNING,"[%d]Ping Lagging: %u threshold: %u",token, nOrigPing, g_settings.DS_MONITOR_PING_LAGGING);
		}

		if (usFlag == 0x0100)
			rkPingData.uiSocketPingTime = ping;
		else if (usFlag == 0x0200)
			rkPingData.uiGameWayPingTime = ping;
		else
		{
			rkPingData.uiGameServerPingTime = ping;
			TM_ClientTimeManager* pkTimeMgr = (TM_ClientTimeManager*)UT_TimeManager::GetSingletonPtr();
			pkTimeMgr->SyncServerTime(uiServerTime);
		}
		//m_pingMap.erase(it);
	}
	else
	{
		GfxWriteLog(LOG_TAG_SYSTEM,LOG_SWITCH_SYSTEM,"[%d]Ping Token 无效",token);
	}
}
//------------------------------------------------------------------------------------------
void LC_ClientGamePlayer::_computePing()
{
	mem::map<uint8_t,PingData>::iterator IterBe = m_pingMap.begin();
	mem::map<uint8_t,PingData>::iterator IterEd = m_pingMap.end();

	m_currentSocketPing = 0;
	m_currentGateWayPing = 0;
	m_currentGameServerPing = 0;
	for( ; IterBe != IterEd; ++IterBe)
	{
		PingData& rkPingData = (*IterBe).second;
		m_currentSocketPing += rkPingData.uiSocketPingTime;
		m_currentGateWayPing += rkPingData.uiGameWayPingTime;
		m_currentGameServerPing += rkPingData.uiGameServerPingTime;
	}

	if(m_pingMap.size() > 0)
	{
		m_currentSocketPing		= m_currentSocketPing / m_pingMap.size();
		m_currentGateWayPing	= m_currentGateWayPing / m_pingMap.size();
		m_currentGameServerPing = m_currentGameServerPing / m_pingMap.size();
	}
}
//------------------------------------------------------------------------------------------
int LC_ClientGamePlayer::GetCurrentPing(uint16_t usFlag)
{
	if (usFlag == 0x0100)
		return m_currentSocketPing;
	else if (usFlag == 0x0200)
		return m_currentGateWayPing;
	else
		return m_currentGameServerPing;
}
//------------------------------------------------------------------------------------------
StringType LC_ClientGamePlayer::GetServerTime()
{
	TM_ClientTimeManager* pkTimeMgr = (TM_ClientTimeManager*)UT_TimeManager::GetSingletonPtr();
	time_t uiServerTime = (time_t)pkTimeMgr->GetServerTime();
	tm pkCurLocalTime =	* localtime(&uiServerTime);
	StringStreamType kString;
	bool bPm = pkCurLocalTime.tm_hour > 12;
	int iHour = bPm ? pkCurLocalTime.tm_hour - 12 : pkCurLocalTime.tm_hour;
	if(pkCurLocalTime.tm_hour < 10)
		kString << 0;
	kString << iHour;
	kString << ":";
	if(pkCurLocalTime.tm_min < 10)
		kString << 0;
	kString << pkCurLocalTime.tm_min;
	kString << (bPm ? "PM" : "AM");

	return kString.str();
}
int64_t LC_ClientGamePlayer::GetServerTimeVal()
{
	TM_ClientTimeManager* pkTimeMgr = (TM_ClientTimeManager*)UT_TimeManager::GetSingletonPtr();
	if (pkTimeMgr->GetServerTime() == 0)
		return GET_SYSTEM_TIME;
	else
		return pkTimeMgr->GetServerTime();
}
//------------------------------------------------------------------------------------------
int LC_ClientGamePlayer::GetCurrentFPS()
{
	int fps = 0;
	if (UT_StatisticsManager::GetSingletonPtr() != NULL)
	{
		fps = (int)UT_StatisticsManager::GetSingleton().GetFrameRate();
	}
	return fps;
}
//------------------------------------------------------------------------------------------
void LC_ClientGamePlayer::RecordUIOperations(int uiID)
{
	if(uiID < 0 || uiID >= 64)
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"非法的UI操作记录");
		return;
	}

	m_auiUIOperationRecords[uiID] ++;
}
//------------------------------------------------------------------------------------------
void LC_ClientGamePlayer::ClearUIOperationRecords()
{
	memset(m_auiUIOperationRecords,0,sizeof(uint16_t) * 64);
}
//------------------------------------------------------------------------------------------
void LC_ClientGamePlayer::SetRevealInvisibilityParams(const UT_Vec3Int& rkCircleCenter, int iCircleRadius, bool bRevealOnce)
{
	m_kRevealInvisibilityParams.ReUse();
	m_kRevealInvisibilityParams.m_kCircleCenter.x = rkCircleCenter.x;
	m_kRevealInvisibilityParams.m_kCircleCenter.y = rkCircleCenter.y;
	m_kRevealInvisibilityParams.m_kCircleCenter.z = rkCircleCenter.z;
	m_kRevealInvisibilityParams.m_iCircleRadius = iCircleRadius;
	m_kRevealInvisibilityParams.m_bRevealOnce = bRevealOnce;
}
//------------------------------------------------------------------------------------------
void LC_ClientGamePlayer::RequestNPCOwnerShip(object_id_type lNPCID)
{
	float fCurrentTime = GET_PROCESS_TIME;

	if(fCurrentTime - m_fLastRequestOwnerShipTime > 1.0f)
	{
		m_fLastRequestOwnerShipTime = fCurrentTime;
		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = false;

		MG_REQ_NPC_Ownership	reqMsg;
		reqMsg.m_lNPCID			= lNPCID;

		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_NPC_OWNERSHIP, &reqMsg);

		if(bCreateMsg)
			m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}
//------------------------------------------------------------------------------------------
void LC_ClientGamePlayer::LoadControlScript(bool bUseScript,bool bReload)
{

	T_SAFE_DELETE(m_pkScriptControllerFunctor);

	if(!bUseScript)
		return;


	LS_LuaStateFactory* pkLuaStateFactory = LS_LuaStateFactory::GetSingletonPtr();
	LS_LuaState* pkLuaState = pkLuaStateFactory->GetLogicLuaState();

	UT_DirConfig* pkDirConfig = UT_DirConfig::GetSingletonPtr();
	StringType kSkillScriptPath = pkDirConfig->GetDir(CF_NODE_AI_SCRIPT);
	StringType kScriptFileFullName = kSkillScriptPath + "play.lua";
	bool bLoadScript = pkLuaState->DoFile(kScriptFileFullName);

	if(!bLoadScript)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"脚本文件[%s]出错",kScriptFileFullName.c_str());
		return;
	}

	LS_LuaObject* pkLuaObject = NULL;

	StringType kFunctorName = "autotask.execute";
	pkLuaObject = pkLuaState->DirectGet(kFunctorName);
	if(pkLuaObject)
		m_pkScriptControllerFunctor = T_NEW_D LS_LuaVoidFunction(pkLuaObject);
}
//--------------------------------------------------------------------------------------------------
LC_ActorBase* LC_ClientGamePlayer::GetNearFirstActorByCharTypeID( int lCharTypeID )
{
	LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
	const LC_NPCIDPtrMap& rNPCIDMap = pkNPCManager->GetNPCIDMap();
	LC_NPCIDPtrMapConstIter IterBe = rNPCIDMap.begin();
	LC_NPCIDPtrMapConstIter IterEd = rNPCIDMap.end();

	for(; IterBe != IterEd;++IterBe)
	{
		LC_ActorBase* pkActorBase = (LC_ActorBase*)((*IterBe).second);
		if (pkActorBase&& pkActorBase->GetCharType() == lCharTypeID)
			return pkActorBase;
	}

	return NULL;
}
//--------------------------------------------------------------------------------------------------
LC_ClientTranscriptionInfoManager* LC_ClientGamePlayer::GetTranscriptionInfoManager()
{
	LC_ClientTranscriptionInfoManager* pkTranscriptionInfoManager = CLIENT_GET_TRANSCRIPTION_INFO_MANAGER();
	return pkTranscriptionInfoManager;
};

//--------------------------------------------------------------------------------------------------
LC_ClientGameStoryManager* LC_ClientGamePlayer::GetGameStoryManager()
{
	LC_ClientGameStoryManager* pkGameStoryManager = CLIENT_GET_GAMESTORY_MANAGER();
	return pkGameStoryManager;
};

void LC_ClientGamePlayer::SetShapeShiftInfo( int lShapeShiftTypeID )
{
	((LC_ClientShortcutBar*)GetShortcutBar())->ClearBackShortcutBar();
	((LC_ClientShortcutBar*)GetShortcutBar())->SetBackShortActive( lShapeShiftTypeID != 0);
	SetDirtyFlag(DIRTY_FLAG_SHORTCUT, true);
	AddDirtyFlagRevisionID(DIRTY_FLAG_SHORTCUT);

	CF_CharType::DataEntry* pkCharData = CF_CharType::GetSingletonPtr()->GetEntryPtr(lShapeShiftTypeID);
	if(pkCharData == NULL)
		return;

	for (int i = 0;i < 5;++i)
	{
		int lAttackSkillID = CSVHelper::GetCharSkillID(pkCharData, i + 1);
		if (IS_SKILL_TYPE(lAttackSkillID))
			((LC_ClientShortcutBar*)GetShortcutBar())->AddBackShortcutSkill(i,lAttackSkillID);
	}

	BuildTransformSkillCoolDownMap();
}
//--------------------------------------------------------------------------------------------------
//void LC_ClientGamePlayer::CardExchange(int lExchangeItemID)
//{
//	MG_REQ_CardExchange reqMsg;
//	reqMsg.m_lPlayerID = GetID();
//	reqMsg.m_lExchangeItemID = lExchangeItemID;
//	MG_PACKET_ENCODE_HANDLER hMsgHandler;
//	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_CARD_EXCHANGE, &reqMsg);
//	if(bCreateMsg)
//		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
//}
//--------------------------------------------------------------------------------------------------
//void LC_ClientGamePlayer::CardTakeOut(int lCardTypeID, int lTakeOutCount)
//{
//	MG_REQ_CardTakeOut reqMsg;
//	reqMsg.m_lPlayerID = GetID();
//	reqMsg.m_lCardTypeID = lCardTypeID;
//	reqMsg.m_lTakeOutCount = lTakeOutCount;
//	MG_PACKET_ENCODE_HANDLER hMsgHandler;
//	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_CARD_TAKEOUT, &reqMsg);
//	if(bCreateMsg)
//		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
//}
//------------------------------------------------------------------------------------------
void LC_ClientGamePlayer::RequestBeginSeekTreasure()
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_SeekTreasure kMsg;
	kMsg.m_lPlayerID	= GetID();
	bCreateMsg			= CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SEEKTREASURE, &kMsg);
	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
//--------------------------------------------------------------------------------------------
void LC_ClientGamePlayer::RequestDigTreasure()
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_DigTreasure kMsg;
	kMsg.m_lPlayerID	= GetID();
	bCreateMsg			= CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_DIGTREASURE, &kMsg);
	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
//------------------------------------------------------------------------------------------
void LC_ClientGamePlayer::RequestHookInfo()
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_HookInfo msgHookInfo;
	msgHookInfo.m_lPlayerID				= GetID();

	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_HOOK_INFO, &msgHookInfo);

	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
void  LC_ClientGamePlayer::UpdateHook(float fCurrentTime,float fDeltaTime)
{
	float fLastHookUpdateime =	m_fCurrentHookUpdateTime;
	m_fCurrentHookUpdateTime = fCurrentTime;

	fDeltaTime = m_fCurrentHookUpdateTime-fLastHookUpdateime;
	if(m_fCurrentHookTime>0.0f)
	{
		m_fCurrentHookSecond += fDeltaTime;
		m_fCurrentHookTotalTime += fDeltaTime;
		m_fCurrentHookTime-=fDeltaTime;
		m_fTotalHookTime-=fDeltaTime;
		m_fDeleteHookItemTime+=fDeltaTime;
		if(m_fDeleteHookItemTime>600.0f)
		{
			m_fDeleteHookItemTime = 0.0f;
			//扣道具
		}
		if(m_fCurrentHookSecond>1.0f)
		{
			m_fCurrentHookSecond	= 0.0f;

			LC_GameEvent kGameEvent;
			kGameEvent.SetEventType(GLET_CLIENT_HOOK_INFO_UPDATE);

			UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
			pkUIManager->HandlerGameEvent(kGameEvent);
		}

		if(m_fCurrentHookTime<=0.0f)
		{
			m_fCurrentHookEfficiency = 1.0f;
			m_fCurrentHookTime = 0.0f;
			m_fDeleteHookItemTime	= 0.0f;
			m_fCurrentHookSecond = 0.0f;
			m_fCurrentHookTotalTime = 0.0f;
		}
	}
}
//------------------------------------------------------------------------------------------
//void LC_ClientGamePlayer::RequestGetGift(int lGiftID, int lItemTypeID)
//{
//	LC_PackAsset* pkPackAsset = GetPackAsset();
//	if(NULL == pkPackAsset)
//		return;
//
//	if (pkPackAsset->GetPackEmptyEntryCount(PACK_TYPE_TEMP) != TEMP_PACK_NUM)
//	{
//		SendLogicInfoMessage(RE_TEMP_PACK_NOT_EMPTY);
//		return;
//	}
//
//	MG_PACKET_ENCODE_HANDLER hMsgHandler;
//	bool bCreateMsg = false;
//
//	MG_REQ_GET_GIFT msg;
//	msg.m_lPlayerID = GetID();
//	msg.m_lGiftID = lGiftID;
//	msg.m_lItemTypeID = lItemTypeID;
//	
//	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GET_GIFT, &msg);
//
//	if(bCreateMsg)
//		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
//}
//------------------------------------------------------------------------------------------
bool LC_ClientGamePlayer::CheckRecvLuckAward()
{
	return false;
}
//------------------------------------------------------------------------------------------
void LC_ClientGamePlayer::RequestRecvLuckAward()
{
	//发送网络消息
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_RECV_LUCK_AWARD, NULL);

	m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
//------------------------------------------------------------------------------------------
void LC_ClientGamePlayer::_UpdateReqFriendStatus(float fCurrentTime)
{
	float fElapse = fCurrentTime - m_fLastReqFriendStatusTime;
	if( fElapse > 200.0f)
	{
		ReqFriendStatus( ST_FRIEND );
		m_fLastReqFriendStatusTime = fCurrentTime;
	}

}
//------------------------------------------------------------------------------------------
void LC_ClientGamePlayer::RequestLeavePrison()
{

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_LEAVE_PRISON msgLP;
	msgLP.m_lPlayerID = GetID();

	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_LEAVE_PRISON, &msgLP);

	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

}
//---------------------------------------------------------------------------
bool LC_ClientGamePlayer::IsBindCrashEnough()
{
	bool bResult = false;
	//int lLevel = GetLevel();
	int lLevel = GetCommanderLevel();
	CF_LevelUp* pkCSVLevelUp = CF_LevelUp::GetSingletonPtr();
	const CF_LevelUp::DataEntry* pkData = pkCSVLevelUp->GetEntryPtr(lLevel);
	if(!pkData)
		return bResult;
	int lCashValue = pkData->_ReducePKValue.x;
	MoneyType CurCash = GetBindCash().GetMoney();
	if( CurCash < (MoneyType)lCashValue)
	{
		SendMessageDialog( RE_DIALOG_CONFIRM_USE_UNBINDCASH_REGUCE_PK, "", ""
			,MDT_CONFIRM_USE_UNBINDCRASH_REDUCEPK, NO_USE_PARAMETER,NO_USE_PARAMETER, NO_USE_PARAMETER );
		return false;
	}
	return true;
}
void LC_ClientGamePlayer::RequestReducePKValue()
{	MG_PACKET_ENCODE_HANDLER hMsgHandler;
bool bCreateMsg = false;

MG_REQ_REDUCE_PKVALUE msgLP;
msgLP.m_iPlayerID = GetID();

bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_REDUCE_PKVALUE, &msgLP);

if(bCreateMsg)
m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

}
//---------------------------------------------------------------------------
void LC_ClientGamePlayer::SendRequestInfo(int iRequestType,int lParam1,int lParam2,int lParam3)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_RequestInfo msg;
	msg.m_iRequestType = iRequestType;
	msg.m_iRequesterID = GetID();
	msg.m_lParam[0] = lParam1;
	msg.m_lParam[1] = lParam2;
	msg.m_lParam[2] = lParam3;

	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_REQUEST_INFO, &msg);

	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
//---------------------------------------------------------------------------
void LC_ClientGamePlayer::SendConfirmRequestInfo( int iRequestType,int iResult,int lID )
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_ConfirmRequestInfo msg;
	msg.m_iRequestType = iRequestType;
	msg.m_iResult	= iResult;
	msg.m_iID		= lID;

	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_REQUEST_CONFIRM_INFO, &msg);

	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
//---------------------------------------------------------------------------
void LC_ClientGamePlayer::SendRequestExpandWarehouse(int iRowNum)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_ExternWarehouse msg;
	msg.m_lPlayerID = GetID();
	msg.m_iRowNum = iRowNum;

	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_EXPAND_WAREHOUSE, &msg);

	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
//---------------------------------------------------------------------------
int LC_ClientGamePlayer::GetReducePkReqCash()
{

	//int lLevel =GetLevel();
	int lLevel = GetCommanderLevel();
	CF_LevelUp* pkCSVLevelUp = CF_LevelUp::GetSingletonPtr();
	const CF_LevelUp::DataEntry* pkData = pkCSVLevelUp->GetEntryPtr(lLevel);
	if(!pkData)
		return 0;
	int lCashValue = pkData->_ReducePKValue.x;
	return lCashValue;


}
//---------------------------------------------------------------------------
int LC_ClientGamePlayer::GetReucePkResultValue()
{
	//int lLevel =GetLevel();
	int lLevel = GetCommanderLevel();
	CF_LevelUp* pkCSVLevelUp = CF_LevelUp::GetSingletonPtr();
	const CF_LevelUp::DataEntry* pkData = pkCSVLevelUp->GetEntryPtr(lLevel);
	if(!pkData)
		return 0;
	int lReduceValue = pkData->_ReducePKValue.y;
	return lReduceValue;
}
//---------------------------------------------------------------------------
void LC_ClientGamePlayer::SocietyCheckPkEvent( object_id_type lScrPlayerID, const StringType& strScrName, object_id_type lDestPlayerID, const StringType& strDestName )
{
	MG_REQ_Check_PK_Event reqMsg;
	if( lScrPlayerID == lDestPlayerID)
	{
		return;
	}
	reqMsg.m_lScrPlayerID = lScrPlayerID;
	reqMsg.m_lDestPlayerID = lDestPlayerID;
	TPSTR2STDSTR(strScrName, reqMsg.m_szScrCharName);
	TPSTR2STDSTR(strDestName, reqMsg.m_szDestCharName);

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_CHECK_PKEVENT, &reqMsg);
	if (bCreateMsg)
	{
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}
//---------------------------------------------------------------------------
void LC_ClientGamePlayer::StartFollowTarget(object_id_type lTargetID)
{
	m_lFollowedTargetID = lTargetID;
	SendLogicInfoMessage(RE_FOLLOW_TARGET_START);
}
//---------------------------------------------------------------------------
bool LC_ClientGamePlayer::StopFollowTarget(object_id_type err)
{
	object_id_type t = m_lFollowedTargetID;
	m_lFollowedTargetID = 0;

	if (IS_PLAYER_CLASS_ID(t))	//following
	{
		if (err > RE_SUCCESS)
			SendLogicInfoMessage(err);
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
void LC_ClientGamePlayer::UpdateFollowTarget()
{
	if(m_lFollowedTargetID <= 0)
		return;

	UT_Vec3Int kTargetLocation;
	if(IS_PLAYER_CLASS_ID(m_lFollowedTargetID))
	{
		LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
		if(NULL == pkNetPlayerManager)
		{
			this->StopFollowTarget(RE_FOLLOW_TARGET_FAIL_TOO_FAR);
			return;
		}

		LC_PlayerBase* pkNetPlayer = pkNetPlayerManager->FindPlayer(m_lFollowedTargetID);
		if(NULL == pkNetPlayer)
		{
			this->StopFollowTarget(RE_FOLLOW_TARGET_FAIL_TOO_FAR);
			return;
		}

		
		 kTargetLocation = pkNetPlayer->GetCurrentLocation();
	}
	else
	{
		LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();

		if(NULL == pkNPCManager)
		{
			this->StopFollowTarget(RE_FOLLOW_TARGET_FAIL_TOO_FAR);
			return;
		}

		LC_NPCBase* pkNPC = pkNPCManager->FindNPC(m_lFollowedTargetID);
		if(NULL == pkNPC)
		{
			this->StopFollowTarget(RE_FOLLOW_TARGET_FAIL_TOO_FAR);
			return;
		}
		
		kTargetLocation = pkNPC->GetCurrentLocation();
		
	}

	UT_Vec3Int kCurrentLocation = GetVisibleLocation();
	float distance=(kCurrentLocation-kTargetLocation).Length();
	if (distance>1000)
	{
		UT_Vec2Float kDir;
		kDir.x = kTargetLocation.x - kCurrentLocation.x;
		kDir.y = kTargetLocation.y - kCurrentLocation.y;
		kDir.Unitize();

		kTargetLocation.x = kTargetLocation.x - kDir.x * 100;
		kTargetLocation.y = kTargetLocation.y - kDir.y * 100;

		SetTargetLocation(kTargetLocation);
	}
	
}
//---------------------------------------------------------------------------
void LC_ClientGamePlayer::SetMouseOverTargetArg(MouseOverTargetArg *pkArg)
{
	if (0 && pkArg != NULL)	//暂时屏蔽Outline
	{
		m_bMouseOverTargetArgValid = true;
		m_kMouseOverTargetArg = *pkArg;
	}
	else
	{
		m_bMouseOverTargetArgValid = false;
	}
}
//---------------------------------------------------------------------------
void LC_ClientGamePlayer::SetMouseOverTarget(object_id_type lOverID) 
{
	GUISys::WT_Widget* wgt = GUISys::WT_WidgetManager::GetSingleton().FindWidget("UI.GameUIScreen.DropItemTooltip");
	if (m_lMouseOverActorID == lOverID)
	{
		if (!this->IsGravePressed())
		{
			//hide tip
			if (NULL != wgt)
			{
				if(m_bDropItemTipsShowing)
				{
					wgt->HideTooltip();
					m_bDropItemTipsShowing = false;
				}
				wgt->SetTooltipShowing(false);
				wgt->SetCustomId(0);
			}
		}
	}
	m_lMouseOverActorID = lOverID;
	if (NULL == wgt)
		return;

	if (IS_CHEST_ID(lOverID) && this->IsGravePressed())
	{
		LC_PlaceHolderManagerBase* pkPlaceHolderManager = ENGINE_GET_CHEST_MANAGER();
		LC_ClientPlaceHolder* pkPlaceHolder  = (LC_ClientPlaceHolder*)pkPlaceHolderManager->FindPlaceHolder(lOverID);
		if(NULL == pkPlaceHolder)
			return;
		//show tip
		if (wgt->GetCustomId() == pkPlaceHolder->GetItemTypeID() && wgt->IsTooltipShowing())
			return;
		if (pkPlaceHolder->GetItemTypeID() > 0)
		{
			UT_Vec3Int kPosition = pkPlaceHolder->GetVisibleLocation();
			NiPoint3 kPos = UT_EngineMath::ConvertVec3IntToNiPoint3(kPosition);

			NiPoint3 kScreenPos = UE_RenderManager::GetSingleton().ConvertToScreenPos(kPos);
			wgt->SetWinPos(kScreenPos.x - wgt->GetWidth() * 0.5, kScreenPos.y - wgt->GetHeight() * 0.5);
			wgt->SetCustomId(pkPlaceHolder->GetItemTypeID());
			wgt->SetCustomType(1);
			IP_InputManager & input = IP_InputManager::GetSingleton();
			LPARAM lParam = input.GetMousePosition();
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			int iIndex = wgt->ParseDragIndex(x, y, GUISys::UI_PARSE_EXACT);
			wgt->SetCustomTooltip(false);
			wgt->SetTip("NULL");
			wgt->SetCustomTooltip(true);
			wgt->SetTip("NULL");
			wgt->ParseTooltip(iIndex, wgt->GetWinRect(), x, y);
			m_bDropItemTipsShowing = true;
			return;
		}
	}

	else if (IS_LOCAL_NPC_ID(lOverID))
	{
		LC_ClientNPCManager* const pkNPCManager = CLIENT_GET_NPC_MANAGER();
		LC_ClientNPC* const pkNPC = (LC_ClientNPC*)pkNPCManager->FindNPC(lOverID);

		//show tip
		if (pkNPC && wgt->GetCustomId() == pkNPC->GetID() && wgt->IsTooltipShowing())
			return;

		if(pkNPC && pkNPC->GetNPCType() == CLIENT_NPC_TYPE_PORTAL) //传送门
		{
			long lUserData = pkNPC->GetNPCFuncState(NFT_PORTAL);
			long lMapResID = lUserData >> 8;
			int eType = lUserData & 0xFF;
			if(eType == CLIENT_NPC_PORTAL_TOWN)	//城镇
			{
				LC_ClientMapTranscriptionManager* pkMapTransMgr = LC_ClientMapTranscriptionManager::GetSingletonPtr();
				if(pkMapTransMgr)
				{
					LC_ClientMapTransDataEntry* pkData = pkMapTransMgr->GetMapTransDataEntry(lMapResID);
					if(pkData)
					{
						LPARAM lParam = IP_InputManager::GetSingleton().GetMousePosition();
						int x = LOWORD(lParam);
						int y = HIWORD(lParam);
						wgt->SetWinPos(x - wgt->GetWidth() * 0.5, y - wgt->GetHeight() * 0.5);

						wgt->SetCustomTooltip(false);
						wgt->SetCustomId(pkNPC->GetID());
						wgt->SetCustomType(2);
						wgt->SetTip(pkData->GetMapShowName().c_str());
						int iIndex = wgt->ParseDragIndex(x, y, GUISys::UI_PARSE_EXACT);
						wgt->ParseTooltip(iIndex, wgt->GetWinRect(), x, y);
						m_bDropItemTipsShowing = true;
						return;
					}
				}
			}
		}
	}
	//hide tip
	if(m_bDropItemTipsShowing)
	{
		wgt->HideTooltip();
		m_bDropItemTipsShowing = false;
	}
	wgt->SetTooltipShowing(false);
	wgt->SetCustomId(0);
}

void LC_ClientGamePlayer::SetMouseOverInteractiveBtn(object_id_type lOverID)
{
	m_lMouseOverInteractiveBtnActorID = lOverID;
}
//---------------------------------------------------------------------------
void LC_ClientGamePlayer::ManualChangeLockedTarget()
{
	UT_Circle kCircle;
	kCircle.iRadius = CHANGE_LOCKED_DISTANCE;
	kCircle.kCenterPoint = this->GetCurrentLocation();


	LC_MapBase* pkMap = this->GetMap();
	if(!pkMap)
	{
		GfxWriteLog(LOG_WARNING_FILELINE, LOG_SWITCH_WARNING,"脚本:试图用非法目标位置作区域锁定");
		return ;
	}

	CF_ActorFilterFunc* pkCSVActorFilterFunc = CF_ActorFilterFunc::GetSingletonPtr();
	CF_ActorFilterFunc::DataEntry* pkData = pkCSVActorFilterFunc->GetEntryPtr(88);
	if(!pkData)
	{
		GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"找不到目标过滤者%d", 88);
		return ;
	}

	//从地图返回区域内的Actor队列
	LC_ActorPtrVector* pkLockResult = pkMap->LockActorsInCircle(kCircle,pkData);
	if (pkLockResult->empty())
	{
		return;
	}

	//根据过滤设置,过滤Actor队列
	object_id_vector kFilterResult = LC_Helper::FilterActorsSortDist(this,*pkLockResult,pkData);
	
	if (!kFilterResult.empty() )
	{
		LC_TargetMap* pkTargetMap = this->GetTabTargetMap();
		pkTargetMap->Clear();

		//与以前锁定的对象互斥，就是不再锁定以前锁定过的对象，只锁定新的
		//if(bExclusive)
			pkTargetMap->ExclusiveTargets(kFilterResult);	//从kFilterResult中排除pkTargetMap已锁定的对象

		//随机扰动队列顺序
		//if(iRandomIndex >= 0)
		//{
		//	float fRandom = pkScriptCaller->GetRandNum(iRandomIndex);
		//	_randomTargetVector(kFilterResult,fRandom);
		//}

		//删除掉多余的对象
		//while( UInt2Int(kFilterResult.size()) > iMaxLockCount)
		//	kFilterResult.pop_back();

		//将结果添加到目标表中
		pkTargetMap->PushTargets(kFilterResult);

		int iIndex = UT_MathBase::RandInRangeInt(0, kFilterResult.size()-1);
		this->SetLockedTargetID(kFilterResult[iIndex]);

		//int iLockCount = kFilterResult.size();
		/*if(pkScriptCaller->IsListening())
		{
			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"[%i]技能脚本: 锁定目标数量[%i]",
				pkScriptCaller->GetID(),
				iLockCount);

			for(int i = 0; i < iLockCount; i++)
			{
				object_id_type lTargetID = kFilterResult[i];
				GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"[%i]技能脚本: 锁定目标[%i]",
					pkScriptCaller->GetID(),
					lTargetID);
			}
		}	*/
	}
}
//---------------------------------------------------------------------------
void LC_ClientGamePlayer::ManualChangeLockedTargetFunc1()
{
	if( IsFreezeLocked() )
	{
		SendLogicInfoMessage(RE_ALREADY_FIX_TARGET);
		return ;
	}

	LC_MapBase* pkMap = this->GetMap();
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
	kCircle.iRadius = CHANGE_LOCKED_DISTANCE;
	kCircle.kCenterPoint = GetCurrentLocation();
	LC_ActorPtrVector* pkLockResult = pkMap->LockActorsInCircle(kCircle,pkData);
	if (pkLockResult->empty())
	{
		return;
	}

	//根据距离对目标进行排序
	UT_Vec3Int kPlayerLocation = GetCurrentLocation();
	//pkLockResult->sort(lessThan(kPlayerLocation, m_kLastAttackerID));
	std::sort(pkLockResult->begin(), pkLockResult->end(), lessThan(kPlayerLocation, m_kLastAttackerID));

	//根据过滤设置,过滤Actor队列
	object_id_vector kFilterResult = LC_Helper::FilterActors(this, *pkLockResult, pkData);

	if(kFilterResult.empty())
		return;

// 	int selecIndex = -1;
// 	for(int i = 0 ; i < kFilterResult.size(); i++ )
// 	{
// 		object_id_type iid = kFilterResult[i];
// 		mem::map<object_id_type, bool>::iterator findIter = m_flagVec.find(iid);
// 		if(findIter == m_flagVec.end() && iid != GetPreLockedTargetID())
// 		{
// 			selecIndex = i;
// 			m_flagVec.insert(pair<object_id_type,bool>(iid,true));
// 			break;
// 		}
// 	}
// 	
// 	if(selecIndex == -1)
// 	{
// 		m_flagVec.clear();
// 		selecIndex = 0;
// 	}

	object_id_type lSelectTargetID = 0;
	if ( this->GetPreLockedTargetID() != kFilterResult.front() )
	{
		lSelectTargetID = kFilterResult.front();
	}
	else if ( kFilterResult.size() > 1 )
	{
		lSelectTargetID = kFilterResult[1]; 
	}
	else
	{
		return ;
	}

	//this->SetLockedTargetID(lSelectTargetID);
	this->SetPreLockedTargetID(lSelectTargetID);
}
//---------------------------------------------------------------------------
void LC_ClientGamePlayer::ClearChangeLockedFlag()
{
	m_flagVec.clear();
}
//---------------------------------------------------------------------------
bool LC_ClientGamePlayer::IsHookArea()
{
	CF_LogicAreaFunc::DataEntry* pkLogicAreaFunc = GetLogicAreaFunc();
	if (HAT_SAFE != pkLogicAreaFunc->_iID && HAT_FIGHT != pkLogicAreaFunc->_iID)
	{
		return false;
	}
	return true;
}

int LC_ClientGamePlayer::GetHookRadius()
{
	AssitAutoSearchType eSearchType = m_kAssistAsset.GetAutoSearchType();
	switch(eSearchType)
	{
	case AAST_FIX_PT:
		return g_settings.DS_FIX_HOOK_DISTANCE + GetProperty_ShapeRadius();
	case AAST_FULLSCREEN:
		return g_settings.DS_FULLSCREEN_HOOK_DISTANCE;
	case AAST_MAP:
		return 30000;
	default:
		T_ASSERT_CRITICAL_S(false);
		return g_settings.DS_FIX_HOOK_DISTANCE + GetProperty_ShapeRadius();
	}
}

void LC_ClientGamePlayer::SetHooking(bool hFlag)
{
	ClearPendingSkill();

	if (hFlag == false && m_bHooking != hFlag && IS_SKILL_TYPE(GetCurrentStateType()) )
	{
		ResultType eCheckResult = CheckInterruptSkill(GetCurrentStateType(),SKI_MOVE);
		if ( eCheckResult == RE_SKILL_NONE_ERROR)
		{
			RequestInterrputSkill(GET_PROCESS_TIME, GetCurrentStateType(), SKI_MOVE);
			InterruptSkill(GET_PROCESS_TIME, GetCurrentStateType(), SKI_MOVE);
		}
	}

	if (hFlag)
	{
		m_lHookTime = GET_PROCESS_TIME;
		m_kHookLocation = GetCurrentLocation();
		//m_kHookGuideLocation = m_kHookLocation;
		SetAutoPicking(false);
		StopMove();
		ClearPathFinding();

		UI_UIManager::GetSingleton().SetEasyModeTic(g_settings.DS_EASY_MODEL_T);
		UI_UIManager::GetSingleton().SetHookProfitTic(g_settings.DS_HOOK_PROFIT);

		if (GetLockedTargetID() > 0 && GetMap())
		{
			LC_ActorBase* pkTarget = (LC_ActorBase*)GetMap()->FindObject(GetLockedTargetID());
			CF_ActorFilterFunc* pkCSVFilterFunc = CF_ActorFilterFunc::GetSingletonPtr();
			CF_ActorFilterFunc::DataEntry* pkData = pkCSVFilterFunc->GetEntryPtr(2);
			bool bCanAttack = (LC_Helper::IsValidTarget(this,pkTarget,pkData) == RE_SKILL_CHECK_VALID_TARGET);
			if (pkTarget && !pkTarget->GetDead() && bCanAttack)
			{
				SetHookingTargetID(GetLockedTargetID());
			}
		}
	}
	else
	{
		SetSpecialHookCharType(0);
		SetHookingTargetID(0);

		UI_UIManager::GetSingleton().NotifyRestoreEasyModelUI();
		UI_UIManager::GetSingleton().ResetEasyModelTick();
		UI_UIManager::GetSingleton().ResetHookProfitTick();
	}

	m_bHooking = hFlag;
	m_kAssistAsset.SetFlag(AAFB_AUTOATTACK, m_bHooking);
	SynHookingSettingToServer();

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_CLIENT_HOOK_STATE);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, m_bHooking);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void LC_ClientGamePlayer::SetRaidStageHookAssist(int iIndex, int iStage, int MapResID)
{
	CF_SmartHook::DataEntry* pEntry = CF_SmartHook::GetSingleton().GetEntryPtr(MapResID);
	if ( pEntry )
	{
		GetRaidHookAsset().SetRaidStageHookAssist(iIndex, iStage, MapResID);
	}
}

void LC_ClientGamePlayer::SetAutoPicking(bool bFlag)
{
	m_bAutoPicking = bFlag;
	if (m_bAutoPicking)
	{
		SetHooking(false);
	}
	else
	{

	}
}

void LC_ClientGamePlayer::ResetHooking(void)
{
	LC_AssistAsset &kAssistAsset = GetAssistAsset();
	kAssistAsset.Reset();

	SynHookingSettingToServer();
}

void LC_ClientGamePlayer::SynHookingSettingToServer(void)
{
	MG_Assist_Info_Set_Req reqMsg;

	GetAssistAsset().SetDataToProtocolStruct(reqMsg.m_AssistInfo);

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SET_ASSIST_INFO, &reqMsg);

	if(bCreateMsg)
	{
		SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}

void LC_ClientGamePlayer::SetHookingParam(const StringType& info, long lParam, int iValue, bool bSynDataToServer)
{
	LC_AssistAsset &kAssistAsset = GetAssistAsset();
	if (info == "hp")
	{
		if (kAssistAsset.GetHPTypeID() == lParam)
			return;
		kAssistAsset.SetHPTypeID(lParam);
	}
	else if(info == "hppro")
	{
		if (kAssistAsset.GetHPProportion() == lParam)
			return;
		kAssistAsset.SetHPProportion(lParam);
	}
	else if (info == "skill")
	{
		if (kAssistAsset.GetAutoSkillID() == lParam)
			return ;
		kAssistAsset.SetAutoSkillID(lParam);
	}
	else if (info == "search")
	{
		if (kAssistAsset.GetAutoSearchType() == AssitAutoSearchType(lParam))
			return;
		kAssistAsset.SetAutoSearchType(AssitAutoSearchType(lParam));
	}
	else if ( info == "equip" )
	{
		kAssistAsset.SetEquipPickOpt(lParam, iValue > 0);
	}
	else if (info == "flag")
	{
		kAssistAsset.SetFlag(AssistAutoFlagBit(lParam), iValue > 0);
		if (AAFB_RECALL_TIME_OUT == lParam)
		{
			m_lHookTime = GET_PROCESS_TIME;
		}
	}
	else if (info == "time")
	{
		if (kAssistAsset.GetHOOKTimeLen() == lParam)
			return;
		kAssistAsset.SetHOOKTimeLen(lParam);
		m_lHookTime = GET_PROCESS_TIME;
	}
	else if (info == "autobuy")
	{
		if (kAssistAsset.GetAutoBuyItemGrade() == lParam)
			return;
		kAssistAsset.SetAutoBuyItemGrade(lParam);
		m_lHookTime = GET_PROCESS_TIME;
	}
	else if (info == "refine")
	{
		if (kAssistAsset.GetRefineGrade(lParam) == (iValue > 0))
			return;
		kAssistAsset.SetRefineGrade(lParam, iValue > 0);
		m_lHookTime = GET_PROCESS_TIME;
	}
	else if ( info == "hookSkill" )
	{
		kAssistAsset.SetHookSkill(lParam,iValue);
	}
	else if ( info == "hookGodSkill" )
	{
		kAssistAsset.SetHookGodSkill(lParam,iValue);
	}
	else if ( info == "hookSkillCheck")
	{
		kAssistAsset.SetHookSkillCheck(lParam, iValue == 1);
	}
	else if ( info ==  "hookGodSkillCheck" )
	{
		kAssistAsset.SetHookGodSkillCheck(lParam, iValue == 1);
	}

	if (bSynDataToServer)
	{
		SynHookingSettingToServer();
	}
}


int LC_ClientGamePlayer::GetHookingParam(const StringType& info, long lParam)
{
	int iResult = 0;

	LC_AssistAsset &kAssistAsset = GetAssistAsset();
	if (info == "hp")
	{
		iResult = kAssistAsset.GetHPTypeID();
	}
	else if(info == "hppro")
	{
		iResult = kAssistAsset.GetHPProportion();
	}
	else if (info == "skill")
	{
		iResult = kAssistAsset.GetAutoSkillID();
	}
	else if (info == "search")
	{
		iResult = kAssistAsset.GetAutoSearchType();
	}
	else if (info == "equip")
	{
		iResult = (int)kAssistAsset.GetEquipPickOpt(lParam);
	}
	else if (info == "flag")
	{
		iResult = kAssistAsset.IsFlagSet(AssistAutoFlagBit(lParam));
	}
	else if (info == "time")
	{
		iResult = kAssistAsset.GetHOOKTimeLen();
	}
	else if (info == "autobuy")
	{
		iResult = kAssistAsset.GetAutoBuyItemGrade();
	}
	else if (info == "refine")
	{
		iResult = kAssistAsset.GetRefineGrade(lParam);
	}
	else if ( info == "hookSkill" )
	{
		iResult = kAssistAsset.GetHookSkill(lParam);
	}
	else if ( info == "hookGodSkill" )
	{
		iResult = kAssistAsset.GetHookGodSkill(lParam);
	}
	else if ( info == "hookSkillCheck")
	{
		iResult = kAssistAsset.GetHookSkillCheck(lParam);
	}
	else if ( info == "hookGodSkillCheck" )
	{
		iResult = kAssistAsset.GetHookGodSkillCheck(lParam);
	}
	else if ( info == "hpTypeID" )
	{
		iResult = kAssistAsset.GetHPTypeID();
	}

	return iResult;
}

void LC_ClientGamePlayer::AddInvalidPickItem(object_id_type lItemID)
{
	if ( m_mapInvalidPickItem.find(lItemID) == m_mapInvalidPickItem.end() )
		m_mapInvalidPickItem.insert(pair<object_id_type,object_id_type>(lItemID,lItemID));
}

bool LC_ClientGamePlayer::IsInvalidPickItem(object_id_type lItemID)
{
	bool flag = false;
	return m_mapInvalidPickItem.find(lItemID) != m_mapInvalidPickItem.end();
}

LC_ClientRaidHookAsset& LC_ClientGamePlayer::GetRaidHookAsset()
{
	return m_kRaidHookAsset;
}

bool LC_ClientGamePlayer::AccumulateStand2Hook(float fDeltaTime)
{
	if(m_bEnableStandToHook && m_fStandToHookTimer > 0)
	{
		m_fStandToHookTimer -= fDeltaTime;
		if (m_fStandToHookTimer < 0)
		{
			return true;
		}
	}
	return false;
}

bool LC_ClientGamePlayer::Accumulate2Guding(float fDeltaTime)
{
	//寻路状态不进自动任务倒计时
	if(m_bEnableToGuding && !m_bIsPathFinding && m_fGudingTimer > 0)
	{
		m_fGudingTimer -= fDeltaTime;
		if (m_fGudingTimer < 0)
		{
			return true;
		}
	}
	return false;
}

void LC_ClientGamePlayer::Reset2Guding(bool bEnale)
{	
	m_fGudingTimer = g_settings.DS_GUIDE_START;

	if(bEnale && IsNeedToEnalbeGuide())
		Enable2Guding(true);

	if(bEnale)
		m_iAutoTaskType = STT_NONE;
}

void LC_ClientGamePlayer::Reset2GudingFirset()
{
	m_fGudingTimer = g_settings.DS_GUIDE_FIRST_START;
}

void LC_ClientGamePlayer::Enable2Guding(bool b, bool bImmediately)
{
	m_bEnableToGuding = b;
	if(!b)
		((SM_ClientGamePlayerController*)m_pkSMController)->StopAutoGuide();

	if(bImmediately)
		m_fGudingTimer = -1;
}

void LC_ClientGamePlayer::StartGuide(int iTaskID)
{
	if(!IS_NORMAL_MAP(GetMapLogicType()))
	{
		return;
	}

	LC_ClientTaskManager* pkTaskManager = (LC_ClientTaskManager*)LC_ClientTaskManager::GetSingletonPtr();
	LC_Task *pkTask = pkTaskManager->GetTask(iTaskID);
	if(pkTask && pkTask->GetTaskType() == STT_MAIN_SCENARIO || m_iAutoTaskType != STT_MAIN_SCENARIO)
	{
		m_bTutorialStop = false;
		((SM_ClientGamePlayerController*)m_pkSMController)->GuildPush(this, iTaskID);
	}
}

void LC_ClientGamePlayer::StopGuide()
{
	((SM_ClientGamePlayerController*)m_pkSMController)->StopAutoGuide();
}

bool LC_ClientGamePlayer::AccumulateHooking2Guding(float fDeltaTime)
{
	if(m_bEnableHookingToGuding && m_fHooking2GudingTimer > 0)
	{
		m_fHooking2GudingTimer -= fDeltaTime;
		if (m_fHooking2GudingTimer < 0)
		{
			return true;
		}
	}
	return false;
}

void LC_ClientGamePlayer::NoticeShowChingkungUI(bool bShow)
{
	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_SHOW_CHINGKUNG_UI);
	kGameEvent.SetEventParam(EP_SHOW_CHINGKUNG_UI_SHOW, int(bShow));

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

StringType LC_ClientGamePlayer::GetWorld2PtCurrentLocation(void)
{
	NiCameraPtr spCurrentCamera = CA_CameraManager::GetSingleton().GetNiCamera();
	UT_Vec3Int kPos = GetCurrentLocation();

	float x, y;
	spCurrentCamera->WorldPtToScreenPt(NiPoint3(kPos.x, kPos.y, kPos.z), x, y);	

	StringStreamType sStream("");
	sStream << x << "_" << y;
	return sStream.str();
}

void LC_ClientGamePlayer::QuickPick(void)
{
// 	LC_PlaceHolderManagerBase* pkPlaceHolderManager = ENGINE_GET_CHEST_MANAGER();
// 	
// 	LC_PlaceHolderBase* pkBase = pkPlaceHolderManager->FindNearChestHolder(this);
// 	
// 	if(pkBase && UT_MathBase::LineLengthXYInt(GetCurrentLocation(), pkBase->GetCurrentLocation()) < 3000)
// 	{
// 		SetTargetLocation(pkBase->GetCurrentLocation());
// 		SetLockedTargetID(pkBase->GetID());
// 		PendingSkill(SKILL_QUICK_PICK_CHEST, 0);
// 	}
	SetAutoPicking(true);
}

bool LC_ClientGamePlayer::FastPick(void)
{
	LC_PlaceHolderManagerBase* pkPlaceHolderManager = ENGINE_GET_CHEST_MANAGER();

	LC_PlaceHolderBase* pkBase = pkPlaceHolderManager->FindNearChestHolderEx(this);

	if(pkBase && UT_MathBase::LineLengthXYInt(GetCurrentLocation(), pkBase->GetCurrentLocation()) < 3000)
	{
		SetTargetLocation(pkBase->GetCurrentLocation());
		SetLockedTargetID(pkBase->GetID());
		PendingSkill(SKILL_QUICK_PICK_CHEST, 0);
		return true;
	}
	return false;
}

StringType LC_ClientGamePlayer::GetInFovNetPlayerIDs()
{
	LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
	const LC_PlayerIDPtrMap & playerMap = pkNetPlayerManager->GetPlayerIDMap();
	LC_PlayerIDPtrMap::const_iterator iter = playerMap.begin();

	LC_ActorPtrVector vec;
	LC_ActorPtrVectorIter findItr;

	struct ActorDistance 
	{
		int iDis;
		object_id_type iID;
		bool operator < (const ActorDistance& rhs)const
		{
			return iDis < rhs.iDis;
		}
	};

	std::priority_queue<ActorDistance> actorDisQueue;

	ActorDistance actorDis;
	for( ; iter != playerMap.end(); ++iter )
	{
		LC_ClientNetPlayer* pkNetPlayer = (LC_ClientNetPlayer*)(iter->second);

		int d = UT_MathBase::LineLengthXYInt (GetCurrentLocation(),pkNetPlayer->GetCurrentLocation());
		actorDis.iDis = d;
		actorDis.iID = pkNetPlayer->GetID();


		if(actorDisQueue.empty() || actorDisQueue.size() < 10) //限制最多10个
		{
			actorDisQueue.push(actorDis);
		}
		else 
		{
			int iValue = actorDisQueue.top().iDis;
			if(iValue > d)
			{
				actorDisQueue.pop();
				actorDisQueue.push(actorDis);
			}
		}
	}

	StringType sValue = "";
	while(!actorDisQueue.empty())
	{
		actorDis = actorDisQueue.top();
		actorDisQueue.pop();
		if(sValue == "")
			sValue = Utility::Int2String(actorDis.iID);
		else
			sValue = Utility::Int2String(actorDis.iID) + "," + sValue;
	}
	return sValue;
}

void LC_ClientGamePlayer::CallFriend(GameLogic::LC_Attacker* pkScriptCaller, int lCharTypeID)
{
	LC_PlayerBase* pkPlayer = (LC_PlayerBase*)pkScriptCaller;
	if(NULL == pkPlayer)
		return ;

	LC_ActorOperationEntry* pkEntry = pkPlayer->GetActorOperationEntry(LAOT_SUMMOR_FRIEND);

	LC_ActorOperationManagerBase* pkActorOperationManager = LC_ActorOperationManagerBase::GetSingletonPtr();
	if(NULL == pkActorOperationManager)
		return ;

	pkActorOperationManager->ResetOperation(pkEntry, pkPlayer, true);
	pkActorOperationManager->SetOperationParameter(pkEntry, pkPlayer, 0, lCharTypeID);
	pkActorOperationManager->ExecuteOperation(pkEntry, pkPlayer);
}

void LC_ClientGamePlayer::ReqReturnToCity(void)
{
	_toggleSkill(SKILL_RETURN_CITY);
}

void LC_ClientGamePlayer::ImmReturnToCity(void)
{
	MG_Req_ReturnToCity reqMsg;
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_RETURN_TO_CITY, &reqMsg);
	if(bCreateMsg)
		SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

void LC_ClientGamePlayer::ReturnToCity(void)
{
	this->m_bTranscriptionFromReturnToCity = true;
}

void LC_ClientGamePlayer::OnTranscriptionSucceed(void)
{
	m_fTranscriptionCDTimer1 = 0.3f;
	m_fTranscriptionCDTimer2 = 0.5f;

	IM_Manager* pkIMManager = IM_Manager::GetSingletonPtr();
	GD_ClientActorModel* pkVisibleData = pkIMManager->GetActor(GetID());
	if( pkVisibleData && !m_bEnterGameFlag)
		pkVisibleData->SetVisibility(false);

	if (m_bEnterGameFlag)
		m_bEnterGameFlag = false;
}

bool LC_ClientGamePlayer::IsNeedToEnalbeGuide()
{
	//进入挂机荒原需要中断自动任务状态
	//如果当前任务在挂机荒原会开启自动挂机，不受影响
	//第一次进游戏如果在等级范围内则默认开启，可能上线就在 挂机荒原地图 此时符合等级条件也应该开启自动任务
	return GetCommanderLevel() < g_settings.DS_GUIDE_END_LEVEL
		&& (!GetIsFirstEnterGame() || GetMapLogicType() != MT_HOOK_MAP);
}

void LC_ClientGamePlayer::_updateTranscriptionSucceed(float fCurrentTime,float fDeltaTime)
{
	if (m_fTranscriptionCDTimer1 > 0)
	{
		m_fTranscriptionCDTimer1 -= fDeltaTime;		//回城出现特效倒计时
		if (m_fTranscriptionCDTimer1 <= 0)
		{
			//播放回城出现特效
			FX_ClientFxManager* pkFXManager = FX_ClientFxManager::GetSingletonPtr();
			if(pkFXManager)
			{
				int iFXID = pkFXManager->CreateFx(RES_FX_RETURN_TO_CITY_SHOW,true);
				const UT_Vec3Int & rkLocation = this->GetVisibleLocation();
				NiPoint3	kLocationV3d(rkLocation.x, rkLocation.y, rkLocation.z);
				pkFXManager->BindFxToLocation(iFXID,kLocationV3d);
				pkFXManager->PlayFx(iFXID, fCurrentTime);
			}
		}
	}
	if (m_fTranscriptionCDTimer2 > 0)
	{
		m_fTranscriptionCDTimer2 -= fDeltaTime;		//回城出现人物倒计时
		if (m_fTranscriptionCDTimer2 <= 0)
		{
			IM_Manager* pkIMManager = IM_Manager::GetSingletonPtr();
			GD_ClientActorModel* pkVisibleData = pkIMManager->GetActor(GetID());
			if( pkVisibleData )
				pkVisibleData->SetVisibility(true);

			if (this->m_bTranscriptionFromReturnToCity)	//传送回城的时候，往前走一步，其它传送情况不走
			{
				this->m_bTranscriptionFromReturnToCity = false;

				//往前走一步
				Utility::UT_Vec3Int kPos = this->GetCurrentLocation();
				Utility::UT_Vec2Float kMov = this->GetForwardDir() * 200.0f;
				kPos.x += (int)kMov.x;
				kPos.y += (int)kMov.y;
				this->SetTargetLocation(kPos);
			}
		}
	}
}

void LC_ClientGamePlayer::DecompositionEquip(int iPackType, int iEntryIndex)
{
	LC_BackPackEntry *pkEntry = GetPackAsset()->GetBackPackEntry(iPackType, iEntryIndex);
	if (NULL != pkEntry)
	{
		LC_ItemBase *pkItem = LC_ItemFactoryBase::GetSingletonPtr()->RequestItem(pkEntry->GetTypeID());
		T_ASSERT(NULL != pkItem);

		ResultType result = pkItem->CheckDecompositionItem(this, pkEntry);
		if(result == RE_SUCCESS)
		{
			MG_PACKET_ENCODE_HANDLER hMsgHandler;
			bool bCreateMsg = false;

			MG_REQ_DecompositItemInPackage reqMsg;
			reqMsg.m_lPlayerID = GetID();

			DecompositItemInfo info;
			info.m_iPackType = iPackType;
			info.m_iPackageEntryIndex = iEntryIndex;
			info.m_lItemID = pkEntry->GetTypeID();
			reqMsg.m_Items.push_back(info);

			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_ITEMDECOMPOSIT, &reqMsg);
			if(bCreateMsg)
			{
				SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
			}
		}
		else
		{
			SendLogicInfoMessage(result);
		}
	}	
}

int LC_ClientGamePlayer::GetRaidScore(int raidID)
{
	const RaidInfo* pkRaid = m_kraidAssert.FindRaid(raidID);
	if(!pkRaid)
		return -1;
	CF_TranscriptionSettleGrade* pkTranscriptionSettleGrade = CF_TranscriptionSettleGrade::GetSingletonPtr();
	if(!pkTranscriptionSettleGrade)
		return -1;
//*
	#pragma TODO("表改了，这里该怎么办？")
	return pkRaid->Score();
/*/
	// 原先的逻辑
	CF_TranscriptionSettleGrade::DataEntryMapExternIterator kIter = pkTranscriptionSettleGrade->GetIterator();
	while(kIter.HasMoreElements())
	{
		CF_TranscriptionSettleGrade::DataEntry* pkData = kIter.PeekNextValuePtr();
		if(pkData)
		{
			if(pkRaid->Score() >= pkData->_lGradeMin && pkRaid->Score() <= pkData->_lGradeMax)
				return pkRaid->Score();
		}	
		kIter.MoveNext();
	}
	return -1;
//*/
}
int32_t LC_ClientGamePlayer::GetRaidRecord(int raidID)
{
	const RaidInfo* pkRaid = m_kraidAssert.FindRaid(raidID);
	if(!pkRaid)
		return -1;

	return pkRaid->ClearRecord();
}

int LC_ClientGamePlayer::GetRaidGloryHighestMap()
{
	return m_kraidAssert.GetRaidGloryHighestMap();
}

int LC_ClientGamePlayer::GetQuickRaidGloryCount(int iMissionId)
{
	std::map<int, int>::iterator iter = m_nQuickRaidGloryCountMap.find(iMissionId);
	if (iter!=m_nQuickRaidGloryCountMap.end())
	{
		return iter->second;
	}
	return 0;
}

bool LC_ClientGamePlayer::HasNotTakeReward(int iBeginChapter,int iEndChapter)
{
	StringStreamType sStream("");
	SC_ClientScriptInterface* pkClientInterface = SC_ClientScriptInterface::GetSingletonPtr();
	SC_ClientSkillInterface* pkInterface = pkClientInterface->GetSkillInterface();
	for (int i=iBeginChapter;i<=iEndChapter;i++)
	{
		int iCurStarNum=GetRaidGloryChapterStarNum(i);
		StringType InfoStr=pkInterface->GetDataString(DataStringType_GloryExpeditionReward,i,0);
		TStringVector kInfo = UT_BaseStringUtility::SplitString(InfoStr, '|');
		for (int j=1;j<=3;j++)
		{
			TStringVector DataStr=UT_BaseStringUtility::SplitString(kInfo[j], ';');
			int iNeedStarNum =atoi(DataStr[0].c_str());
			int RewardIdx=(i-1)*3+j-1;
			bool bToken=m_iRaidGloryRewardRecord[RewardIdx];
			if (iCurStarNum>=iNeedStarNum && !bToken)
			{
				return true;
			}
		}
	}
	return false;
}

void LC_ClientGamePlayer::ReqGetYijiTuxiStageReward(int iStageId)
{
	MG_REQ_GetYiJiTuXiStageReward reqMsg;
	reqMsg.m_nID=iStageId;
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GETYIJITUXISTAGEREWARD, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );    
	}
	
}

void LC_ClientGamePlayer::ReqGetYijiXunbaoStageReward(int iStageId)
{
	MG_REQ_GetYiJiXunBaoStageReward reqMsg;
	reqMsg.m_nID=iStageId;
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GETYIJIXUNBAOSTAGEREWARD, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::ReqGetToyIslandStageReward(int iStageId)
{
	MG_REQ_GetPetIslandsStageReward reqMsg;
	reqMsg.m_nID=iStageId;
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GETPETISLANDSSTAGEREWARD, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );    
	}

}

int LC_ClientGamePlayer::GetYijiTuxiKillCount()
{
	return m_kCommonStatAsset.GetKillSpecialNpcCount(LC_CMN_STAT_TP_YIJITIXI);
}

int LC_ClientGamePlayer::GetYijiXunbaoKillCount()
{
	return m_kCommonStatAsset.GetKillSpecialNpcCount(LC_CMN_STAT_TP_XIYUTAOBAO);
}

int LC_ClientGamePlayer::GetToyIslandKillCount(int key)
{
	return m_kCommonStatAsset.GetKillSpecialNpcCount(key);
}

void LC_ClientGamePlayer::ReqGetSubValue()
{
	MG_REQ_GetSubBValue reqMsg;

	SC_ClientScriptInterface* pkClientInterface = SC_ClientScriptInterface::GetSingletonPtr();
	SC_ClientSkillInterface* pkInterface = pkClientInterface->GetSkillInterface();
	mem::set<int> iTypeSet;
	CF_ACTSubordinateRewardB::DataEntryMapExternIterator iter =  CF_ACTSubordinateRewardB::GetSingletonPtr()->GetIterator();
	while (iter.HasMoreElements())
	{
		CF_ACTSubordinateRewardB::DataEntry* pkData = iter.PeekNextValuePtr();
		
		mem::set<int>::iterator it = iTypeSet.find(pkData->_iType);
		if (it==iTypeSet.end())
		{
			iTypeSet.insert(pkData->_iType);
			int OverCompete=pkInterface->GetActivityScheduleStat(pkData->_iSchedule);
			if (OverCompete==1)
			{
				reqMsg.m_nTypeVec.push_back(pkData->_iType);
			}
		}
		iter.MoveNext();
	}
	
	if (reqMsg.m_nTypeVec.size()>0)
	{
		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GETSUBBVALUE, &reqMsg);
		if (bCreateMsg)
			SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::UpdateYijiTuxiReward(std::vector<int>& InRewardRecord)
{
	int iMaxStage=CF_YiJiTuXiStageReward::GetSingletonPtr()->GetEntryCount();
	m_nYijiTuxiRewardIDRecord.clear();
	m_nYijiTuxiRewardIDRecord.resize(iMaxStage,false);
	for (int i=0;i<InRewardRecord.size();i++)
	{
		m_nYijiTuxiRewardIDRecord[InRewardRecord[i]-1]=true;
	}
}

void LC_ClientGamePlayer::UpdateYijiTuxiReward(int RewardId)
{
	m_nYijiTuxiRewardIDRecord[RewardId-1]=true;
}

void LC_ClientGamePlayer::UpdateYijiXunbaoReward(std::vector<int>& InRewardRecord)
{
	int iMaxStage=CF_YiJiXunBaoStageReward::GetSingletonPtr()->GetEntryCount();
	m_nYijiXunbaoRewardIDRecord.clear();
	m_nYijiXunbaoRewardIDRecord.resize(iMaxStage,false);
	for (int i=0;i<InRewardRecord.size();i++)
	{
		m_nYijiXunbaoRewardIDRecord[InRewardRecord[i]-1]=true;
	}
}

void LC_ClientGamePlayer::UpdateYijiXunbaoReward(int RewardId)
{
	m_nYijiXunbaoRewardIDRecord[RewardId-1]=true;
}

void LC_ClientGamePlayer::UpdateToyIslandReward(std::vector<int>& InRewardRecord)
{
	int iMaxStage=CF_YiJiTuXiStageReward::GetSingletonPtr()->GetEntryCount();
	m_nToyIslandRewardIDRecord.clear();
	m_nToyIslandRewardIDRecord.resize(iMaxStage,false);
	for (int i=0;i<InRewardRecord.size();i++)
	{
		m_nToyIslandRewardIDRecord[InRewardRecord[i]-1]=true;
	}
}

void LC_ClientGamePlayer::UpdateToyIslandReward(int RewardId)
{
	m_nToyIslandRewardIDRecord[RewardId-1]=true;
}

int LC_ClientGamePlayer::GetYijiTuxiRewardStage()
{
	for (int i=0;i< m_nYijiTuxiRewardIDRecord.size() ;i++)
	{
		if (!m_nYijiTuxiRewardIDRecord[i])
		{
			return i+1;
		}
	}
	return m_nYijiTuxiRewardIDRecord.size()+1;
}

int LC_ClientGamePlayer::GetYijiTuxiMaxStage()
{
	return m_nYijiTuxiRewardIDRecord.size();
}

int LC_ClientGamePlayer::GetYijiXunbaoRewardStage()
{
	for (int i=0;i< m_nYijiXunbaoRewardIDRecord.size() ;i++)
	{
		if (!m_nYijiXunbaoRewardIDRecord[i])
		{
			return i+1;
		}
	}
	return m_nYijiXunbaoRewardIDRecord.size()+1;
}

int LC_ClientGamePlayer::GetYijiXunbaoMaxStage()
{
	return m_nYijiXunbaoRewardIDRecord.size();
}

int LC_ClientGamePlayer::GetToyIslandRewardStage()
{
	for (int i=0;i< m_nToyIslandRewardIDRecord.size() ;i++)
	{
		if (!m_nToyIslandRewardIDRecord[i])
		{
			return i+1;
		}
	}
	return m_nToyIslandRewardIDRecord.size()+1;
}

int LC_ClientGamePlayer::GetToyIslandMaxStage()
{
	return m_nToyIslandRewardIDRecord.size();
}

void LC_ClientGamePlayer::ReqGetRotaryTableReward()
{
	MG_REQ_GetRotaryTableReward reqMsg;
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GETROTARYTABLEREWARD, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );    
	}
}

void LC_ClientGamePlayer::ReqSetRotaryTableAnimation(bool bSkip)
{
	MG_REQ_SetRotaryTableSkipAnimation reqMsg;
	reqMsg.m_bSkipAnimation = bSkip;
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SETROTARYTABLESKIPANIMATION, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );    
	}
}

void LC_ClientGamePlayer::UpdateRotaryRewardIdx(int InRewardIdx)
{
	m_iRotaryRewardIdx=InRewardIdx;
}

void LC_ClientGamePlayer::UpdateRotaryTableIsCharged(bool bIsCharged)
{
	m_bChargedToday = bIsCharged;
}

void LC_ClientGamePlayer::UpdateRotaryTableVaildTimes(int InValidTimes)
{
	m_iRotaryRewardValidTimes = InValidTimes;
}

void LC_ClientGamePlayer::UpdateRotaryRewardRecord(std::vector<int>& InRewardRecord)
{
	m_RotaryRewardRecord.clear();
	m_RotaryRewardRecord.reserve(InRewardRecord.size());
	for(int i=0;i<InRewardRecord.size();i++)
	{
		m_RotaryRewardRecord.push_back(InRewardRecord[i]);
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MGPT_SYNCROTARYTABLEREWARDRECORD index = %d ,id= %d",i,InRewardRecord[i]);
	}
}

void LC_ClientGamePlayer::UpdateRotaryEndTime(int RotaryEndTime)
{
	m_iRotaryEndTime=RotaryEndTime;
}

void LC_ClientGamePlayer::UpdateRotarySkipAnimSetting(bool bSkip)
{
	m_bSkipAnimation = bSkip;
}

bool LC_ClientGamePlayer::GetChargedToday()
{
	return m_bChargedToday;
}

int LC_ClientGamePlayer::GetRotaryRewardIdx()
{
	return m_iRotaryRewardIdx;
}

StringType LC_ClientGamePlayer::GetRotaryRewardRecord()
{
	StringStreamType sStream("");
	for (int i=0;i<m_RotaryRewardRecord.size();i++)
	{
		sStream<<m_RotaryRewardRecord[i]<<"#";
	}
	return sStream.str();
}

int LC_ClientGamePlayer::GetRotaryRewardValidTimes()
{
	return m_iRotaryRewardValidTimes;
}

int LC_ClientGamePlayer::GetRotaryEndTime()
{
	return m_iRotaryEndTime;
}

bool LC_ClientGamePlayer::GetSkipAnimSetting()
{
	return m_bSkipAnimation;
}

//void LC_ClientGamePlayer::ReqBuyNonpareilItem(int id)
//{
//	MG_REQ_BuyNonpareilItem reqMsg;
//	reqMsg.m_nID=id;
//	MG_PACKET_ENCODE_HANDLER hMsgHandler;
//	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_BUYNONPAREILITEM, &reqMsg);
//	if (bCreateMsg)
//	{
//		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );    
//	}
//}
//
//void LC_ClientGamePlayer::ReqGetNonpareilReturn(int id)
//{
//	MG_REQ_GetNonpareilReturn reqMsg;
//	reqMsg.m_nID=id;
//	MG_PACKET_ENCODE_HANDLER hMsgHandler;
//	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GETNONPAREILRETURN, &reqMsg);
//	if (bCreateMsg)
//	{
//		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );    
//	}
//}



//-------------------------------------------------------
bool LC_ClientGamePlayer::HasMapCategoryArrived(int iMapCategoryID)
{
	return m_ktransportAssert.FindTransport(iMapCategoryID);
}
//-------------------------------------------------------
UT_Vec3Int&	LC_ClientGamePlayer::GetNpcTalkUIPos(int xOffset, int iUIHeight)
{
	object_id_type npcID = GetLockedNpcID();
	if (!IS_NPC_ID(npcID))
		return m_kNpcTalkUIPos;
	LC_NPCManagerBase* const pkNPCManager = ENGINE_GET_NPC_MANAGER();
	LC_NPCBase* const pkNPC = pkNPCManager->FindNPC(npcID);
	if (!pkNPC)
	{
		return m_kNpcTalkUIPos;
	}
	const UT_Vec3Int & kSceneLocation = pkNPC->GetVisibleLocation();
	NiPoint3 kPos = UT_EngineMath::ConvertVec3IntToNiPoint3(kSceneLocation);

	NiPoint3 kScreenPos = UE_RenderManager::GetSingleton().ConvertToScreenPos(kPos);
	m_kNpcTalkUIPos.x = kScreenPos.x + xOffset;
	m_kNpcTalkUIPos.y = kScreenPos.y - iUIHeight;
	return m_kNpcTalkUIPos;
}

int LC_ClientGamePlayer::GetCurrentCimeliaRank(int iType)
{
	LC_SubUpgradeData *pkData = GetSubordinateAsset().GetSubData(iType);
	if(NULL != pkData)
		return pkData->GetStage();

	return 0;
}

int LC_ClientGamePlayer::GetCurrentCimeliaAttriLevel(int iType)
{
	LC_SubUpgradeData *pkData = GetSubordinateAsset().GetSubData(iType);
	if(NULL != pkData)
		return pkData->GetAttrLevel();

	return 0;
}

int LC_ClientGamePlayer::GetCimeliaOriginCharTypeID(int iType, int iLevel)
{
	int lCharTypeID = 0;
	const LC_SubordinateDataEntry* pkEntry = SafeGetSubDataEntry(iType, iLevel);
	if(NULL != pkEntry)
		lCharTypeID = pkEntry->_iCharType;

	return lCharTypeID;
}

int LC_ClientGamePlayer::GetVehicleAvatarID(void)
{
	//CF_Vehicle *pkVehicleList = CF_Vehicle::GetSingletonPtr();
	//CF_Vehicle::DataEntry *pkEntry = pkVehicleList->GetEntryPtr(GetVehicleLevel());
	//if(NULL == pkEntry)
		//return 0;

	//1.用到这个函数的地方都是上马之后换马具，既然已经上马了已经知道RiderCharType的ID了
	//2.就不要通过VehicleLevel去查询RiderType。因为如果在骑乘状态下进阶成功，会自动切换坐骑，坐骑等级还没有同步过来的时候，就已经切换了坐骑，这个时候穿上的马具就是上一阶的马具
	
	object_id_type jc = GetCimeliaCharTypeID(LC_SUB_TP_VEHICLE,GetVehicleLevel());

	CF_NPCAvatar::DataEntry *pkAvatarEntry = CF_NPCAvatar::GetSingletonPtr()->GetEntryPtr(jc);
	if(NULL == pkAvatarEntry)
		return 0;

	return pkAvatarEntry->_lExtraID;
}

StringType LC_ClientGamePlayer::GetCimeliaAttributeList(int iType, int iStage)
{
	const LC_SubordinateDataEntry* pkEntry = SafeGetSubDataEntry(iType, iStage);
	if(NULL == pkEntry)
		return "";

	StringStreamType sStream("");
	mem::vector<int>::const_iterator it = pkEntry->_sAttrTypes.begin();
	for ( ; it!=pkEntry->_sAttrTypes.end(); ++it)
	{
		sStream << *it << "#";
	}

	return sStream.str();
}

int64_t LC_ClientGamePlayer::GetCimeliaAttribute(int iType, int iRank, int iAttrib, int iAttribLevel)
{
	return LC_Helper::GetCimeliaAttributeByRank(iType, iRank, iAttrib) * iAttribLevel;
}

int64_t LC_ClientGamePlayer::GetCimeliaAttributeByRank(int iType, int iRank, int iPlayerLevel, int iAttrib, bool withBase)
{
	int64_t iResult = 0;
	LC_SubUpgradeData *pkData = GetSubordinateAsset().GetSubData(iType);
	if(NULL != pkData)
	{
		int curLevel = pkData->GetAttributeLevel(iAttrib);
		for(int i=1; i<=iRank; ++i)
		{
			LC_SubordinateDataEntry* pkEntry = SafeGetSubDataEntry(iType, i);
			if(pkEntry)
			{
				int iLevel = i == iRank ? curLevel : pkEntry->_iAttrMaxLevel;
				iResult += (LC_Helper::GetCimeliaAttributeByRank(iType, i, iAttrib) * iLevel);
			}
		}
	}
	if (withBase)
	{
		LC_ActorAttributeMap attrMap;
		LC_Helper::GetCimeliaTransformAttribInfo(this, iType, attrMap);

		iResult += attrMap.GetAttribute(iAttrib);
	}
	
	return iResult;
}

StringType LC_ClientGamePlayer::GetCimeliaUpgradeItemString(int iType)
{
	int iLevel = GetCurrentCimeliaRank(iType);
	const LC_SubordinateDataEntry* pkEntry = SafeGetSubDataEntry(iType, iLevel);
	if(NULL == pkEntry)
		return "";

	long lItemID = pkEntry->_iItemTypeID;
	int iNeedCount = pkEntry->_iCount;

	StringStreamType sStream("");
	sStream << lItemID << "#" << iNeedCount;
	
	Utility::UT_SIMDataList::const_iterator cit = pkEntry->_kCostItems.begin();
	if(!pkEntry->_kCostItems.empty())
	{
		sStream << "#" << pkEntry->_kCostItems.size() << "#";
		for (int i=0; cit!=pkEntry->_kCostItems.end(); ++cit, ++i)
		{
			sStream << cit->ParamExt(i) << "|";
		}
	}
	else
	{
		sStream << "#";
	}

	sStream << "#" << pkEntry->_iExpireForbid;
	return sStream.str();
}


StringType LC_ClientGamePlayer::GetCimeliaName(int iType, int iLevel,bool bOriginal)
{
	int lCharTypeID = 0;
	if (bOriginal)
	{
		lCharTypeID = GetCimeliaOriginCharTypeID(iType,iLevel);
	}
	else
	{
		lCharTypeID = GetCimeliaCharTypeID(iType, iLevel);
	}

	if(lCharTypeID > 0)
	{
		CF_CharType::DataEntry *pkCharTypeEntry = SafeGetCSVFileDataEntryPtr<CF_CharType>(lCharTypeID);
		if(NULL != pkCharTypeEntry)
		{
			return pkCharTypeEntry->_kCharName;
		}
	}

	return "";
}

StringType LC_ClientGamePlayer::GetCimeliaDisPlayLevelInfo(int iType, int iRank)
{
	StringType sValue = "";
	const LC_SubordinateDataEntry *pkEntry = SafeGetSubDataEntry(iType, iRank);
	if(NULL != pkEntry)
		sValue = pkEntry->_sDisplayLevel;

	return sValue;
}

int LC_ClientGamePlayer::GetCurrentVehicleSkill(void)
{
	LC_SubUpgradeData *pkData = GetSubordinateAsset().GetSubData(LC_SUB_TP_VEHICLE);
	if(NULL == pkData)
		return 0;

	const LC_SubordinateDataEntry *pkEntry = SafeGetSubDataEntry(LC_SUB_TP_VEHICLE, pkData->GetStage());
	if(NULL == pkEntry)
		return false;

	return !pkEntry->_kSkillIDs.empty() ? pkEntry->_kSkillIDs.front().ID() : 0;
}

void LC_ClientGamePlayer::SetIsAutoUpgradeCimelia(int iType, bool bAutoUpgrade)
{
	if(iType >= LC_SUB_TP_BEGIN && iType < LC_SUB_TP_END)
	{
		CimeliaUpgradeInfoMap::iterator it = m_bAutoUpgradeCimelia.find(iType);
		if(it != m_bAutoUpgradeCimelia.end())
		{
			it->second = bAutoUpgrade;
		}
	}
}

bool LC_ClientGamePlayer::GetIsAutoUpgradeCimelia(int iType)
{
	if(iType >= LC_SUB_TP_BEGIN && iType < LC_SUB_TP_END)
	{
		CimeliaUpgradeInfoMap::iterator it = m_bAutoUpgradeCimelia.find(iType);
		if(it != m_bAutoUpgradeCimelia.end())
		{
			return it->second;
		}
	}

	return false;
}

void LC_ClientGamePlayer::SetIsDoAutoUpgradeCimelia(int iType, bool bAutoUpgrade)
{
	if(iType >= LC_SUB_TP_BEGIN && iType < LC_SUB_TP_END)
	{
		CimeliaUpgradeInfoMap::iterator it = m_bDoAutoUpgradeCimelia.find(iType);
		if(it != m_bDoAutoUpgradeCimelia.end())
		{
			it->second = bAutoUpgrade;
		}
	}
}

int LC_ClientGamePlayer::_getCurrentAutoUpgradeCimeliaPayIndex(int iType)
{
	int iRank = GetCurrentCimeliaRank(iType);
	const LC_SubordinateDataEntry *pkEntry = SafeGetSubDataEntry(iType, iRank);
	if(NULL == pkEntry)
	{
		return 0;	
	}

	LC_ShopIDMap pkList1 = pkEntry->_kShop1;
	LC_ShopIDMap pkList2 = pkEntry->_kShop2;

	CimeliaPayIndex iPayIndex = PAY_INDEX_ITEM;
	MoneyType iUnbindYuan = m_kUnBindYuanBao.GetMoney();
	MoneyType iBindYuan = m_kBindYuanBao.GetMoney();
	
	if(pkList1.size() > 0)
	{
		LC_ShopIDMap::iterator it = pkList1.begin();
		for (; it!=pkList1.end(); ++it)
		{
			CF_ShopList::DataEntry *pkShop = SafeGetCSVFileDataEntryPtr<CF_ShopList>(it->first);
			if(pkShop)
			{
				if((pkShop->_ActualPriceList[CT_BIND_YUANBAO] > 0 && iBindYuan >= (pkShop->_ActualPriceList[CT_BIND_YUANBAO]*(it->second)))
					|| (pkShop->_ActualPriceList[CT_UNBIND_YUANBAO] > 0 && iUnbindYuan >= (pkShop->_ActualPriceList[CT_UNBIND_YUANBAO]*(it->second))))
				{
					iPayIndex = PAY_INDEX_TYPE_1;
					break;
				}
			}
		}
	}
	
	if(iPayIndex == PAY_INDEX_ITEM && pkList2.size() > 0)
	{
		LC_ShopIDMap::iterator it = pkList2.begin();
		for (; it!=pkList2.end(); ++it)
		{
			CF_ShopList::DataEntry *pkShop = SafeGetCSVFileDataEntryPtr<CF_ShopList>(it->first);
			if(pkShop)
			{
				if(LC_Helper::GetComMapValue(pkShop->_ActualPriceList, CT_BIND_YUANBAO) > 0 && iBindYuan >= (LC_Helper::GetComMapValue(pkShop->_ActualPriceList, CT_BIND_YUANBAO)*(it->second))
					|| (LC_Helper::GetComMapValue(pkShop->_ActualPriceList, CT_UNBIND_YUANBAO) > 0 && iUnbindYuan >= (LC_Helper::GetComMapValue(pkShop->_ActualPriceList, CT_UNBIND_YUANBAO)*(it->second))))
				{
					iPayIndex = PAY_INDEX_TYPE_2;
					break;
				}
			}
		}
	}
	
	return iPayIndex;
}

bool LC_ClientGamePlayer::GetIsDoAutoUpgradeCimelia(int iType)
{
	CimeliaUpgradeInfoMap::iterator it = m_bDoAutoUpgradeCimelia.find(iType);
	if(it != m_bDoAutoUpgradeCimelia.end())
	{
		return it->second;
	}

	return false;
}

void LC_ClientGamePlayer::_initMuhunInfo()
{
	m_bAutoUpgradeCimelia.clear();
	m_bAutoUpgradeCimelia[LC_SUB_TP_VEHICLE] = false;
	m_bAutoUpgradeCimelia[LC_SUB_TP_DEVA_EYE] = false;
	m_bAutoUpgradeCimelia[LC_SUB_TP_MUHUN] = false;

	m_bDoAutoUpgradeCimelia.clear();
	m_bDoAutoUpgradeCimelia[LC_SUB_TP_VEHICLE] = false;
	m_bDoAutoUpgradeCimelia[LC_SUB_TP_DEVA_EYE] = false;
	m_bDoAutoUpgradeCimelia[LC_SUB_TP_MUHUN] = false;

	m_fLastAutoUpgradeCimeliaTime.clear();
	m_fLastAutoUpgradeCimeliaTime[LC_SUB_TP_VEHICLE] = 0.0f;
	m_fLastAutoUpgradeCimeliaTime[LC_SUB_TP_DEVA_EYE] = 0.0f;
	m_fLastAutoUpgradeCimeliaTime[LC_SUB_TP_MUHUN] = 0.0f;
}
//-------------------------------------------------------

void LC_ClientGamePlayer::_toggleSkill(int lSkillID)
{
	if(GetCurrentStateType() == lSkillID)
	{
		RequestInterrputSkill(0, lSkillID, SKI_EVENT);
	}
	else 
	{
		PendingSkill(lSkillID, 0);
	}
}

ResultType LC_ClientGamePlayer::SummonFriendAvatar(object_id_type FriendCharID)
{
	LC_FriendDetailInfo *pkFrdInfo = GetSocietyAsset()->GetFriendInfo1(FriendCharID);
	if(NULL == pkFrdInfo)
	{
		return RE_SUMMORFRIEND_NOT_FRIEND;
	}

	return RE_SUMMORFRIEND_SUCCESS;
}

bool LC_ClientGamePlayer::CheckAcceptTaskCondition(int taskSystem, int taskID)
{
	switch(taskSystem)
	{
	case TaskSystemType_Sky:
		return m_iSkyArenaSeason > 0 && m_iSkyArenaCloseTime > 0
					&& GetServerTimeVal() < m_iSkyArenaCloseTime;
	}
	
	return true;
}

ResultType LC_ClientGamePlayer::RequestGetGiftDaily(int iLevel)
{
	MG_REQ_Draw_VIP_Time_Gift reqMsg;
	reqMsg.m_uiType = 0;
	reqMsg.m_uiLevel = iLevel;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_DRAW_VIP_TIME_GIFT, &reqMsg);

	if(bCreateMsg)
	{
		SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}

	return RE_SUCCESS;
}

ResultType LC_ClientGamePlayer::RequestGetGfitOneOff(int iLevel)
{
	MG_REQ_Draw_VIP_Oneoff_Gift reqMsg;
	reqMsg.m_uiLevel = iLevel;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_DRAW_VIP_ONEOFF_GIFT, &reqMsg);

	if(bCreateMsg)
	{
		SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}

	return RE_SUCCESS;
}

ResultType LC_ClientGamePlayer::RequestSignInToday(int iType)
{
	MG_SignIn_Req reqMsg;
	reqMsg.m_nType = iType;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SIGNIN, &reqMsg);

	if(bCreateMsg)
	{
		SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}

	return RE_SUCCESS;
}

ResultType LC_ClientGamePlayer::RequestReSignIn(int iType, int iDay)
{
	MG_ReSignIn_Req reqMsg;
	reqMsg.m_nType = iType;
	reqMsg.m_Day = iDay;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_RESIGNIN, &reqMsg);

	if(bCreateMsg)
	{
		SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}

	return RE_SUCCESS;
}

ResultType LC_ClientGamePlayer::RequestGetSignInReward(int iType, int iID)
{
	MG_SignInReward_Req reqMsg;
	reqMsg.m_nType = iType;
	reqMsg.m_nID = iID;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SIGNINREWARD, &reqMsg);

	if(bCreateMsg)
	{
		SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}

	return RE_SUCCESS;
}

int LC_ClientGamePlayer::GetGameStoryIdByMapResID(int iMapResID)
{
	CF_WorldMapList::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(iMapResID);
	if(NULL != pkEntry)
	{
		return pkEntry->_iStoryID;
	}

	return 0;
}

void LC_ClientGamePlayer::_initProductionData(void)
{
	std::vector<LC_ClientProduction::SortProductionFormula> vecFormula;

	CF_ProductionFormula* pkCSVFormula = CF_ProductionFormula::GetSingletonPtr();
	CF_ProductionFormula::DataEntryMapExternIterator iter = pkCSVFormula->GetIterator();

	while (iter.HasMoreElements())
	{
		CF_ProductionFormula::DataEntry* pkData = iter.PeekNextValuePtr();
		int i = pkData->_iIndex;
		if (i <  MAX_PRODUCTION_MASK_COUNT * 32 && i >= 0)
		{
			LC_ClientProduction::SortProductionFormula sortFormula;
			sortFormula.eFormulaID = pkData->_iID;
			sortFormula.eUIPriority = pkData->_lUIPriority;
			vecFormula.push_back(sortFormula);
		}

		iter.MoveNext();
	}

	std::sort(vecFormula.begin(), vecFormula.end(), LC_ClientProduction::Sort);

	int k = 0;
	for (std::vector<LC_ClientProduction::SortProductionFormula>::iterator sortIter = vecFormula.begin();
		sortIter != vecFormula.end(); ++sortIter)
	{
		LC_ProductionAssetEntry* pkProductionEntry = GetClientProduction()->GetProductionAssetEntry(k);

		pkProductionEntry->SetValid(true);
		pkProductionEntry->SetTypeID(sortIter->eFormulaID);

		++k;
	}
}

void LC_ClientGamePlayer::RequestSetAssistInfo(int lTypeID)
{
	LC_AssistAsset &kAssistAsset = GetAssistAsset();
	if(lTypeID == kAssistAsset.GetHPTypeID())
		return;

	//kAssistAsset.SetFlag(AAFB_HP, lTypeID > 0);
	kAssistAsset.SetHPTypeID(lTypeID);
	//kAssistAsset.SetHPProportion(lTypeID > 0 ? 8000 : 0);
	SynHookingSettingToServer();
}

void LC_ClientGamePlayer::RequestGetHookingGuideLocation(void)
{
	MG_Hooking_Guide_Location_Req reqMsg;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GET_HOOKING_GUIDE_LOCATION, &reqMsg);

	if(bCreateMsg)
	{
		SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}
//
//void LC_ClientGamePlayer::_initFisrstChargeData(void)
//{
//	//std::vector<LC_ClientProduction::SortProductionFormula> vecFormula;
//
//	CF_FirstDailyCharge* pkCSVFormula = CF_FirstDailyCharge::GetSingletonPtr();
//	CF_FirstDailyCharge::DataEntryMapExternIterator iter = pkCSVFormula->GetIterator();
//
//	while (iter.HasMoreElements())
//	{
//		CF_FirstDailyCharge::DataEntry* pkData = iter.PeekNextValuePtr();
//		m_kFirstDailyCharge->GetData(pkData);
//
//	}
//}

void LC_ClientGamePlayer::BeginMoveStride(void)
{
	this->SetLastPickTerrainTime(GET_PROCESS_TIME);
	this->SetLastPrepareMoveStrideTime(-1.0f);
}

void LC_ClientGamePlayer::EndMoveStride(void)
{
	this->SetMoveStride(false);
	this->SetMouseLogicModeType(MLMT_NORMAL);
	this->SetLastPickTerrainTime(-1.0f);
	this->SetLastPrepareMoveStrideTime(-1.0f);
}

void LC_ClientGamePlayer::ClearMoveStride(void)
{
	if (this->GetMoveStride())
		this->EndMoveStride();
}

//Begin 竞技场---------------------------------------------------------------------------------------
int LC_ClientGamePlayer::GetArenaBattleDailyRanking(int iServerFlag)
{
	//if (iServerFlag == LOCAL_SERVER)
	//	return this->GetArenaBattleAssert()->GetCurrentDailyIndex();
	//else
	//	return this->GetArenaBattleAssert()->GetDailyGlobalIndex();
	return 0;
}
bool LC_ClientGamePlayer::GetArenaBattleDailyRewardValid(int iServerFlag)
{
	//if (iServerFlag == LOCAL_SERVER)
		//return this->GetArenaBattleAssert()->CheckLocalDailyReward();
	//else
		//return this->GetArenaBattleAssert()->CheckGlobalDailyReward();
	return 0 ;
}
int LC_ClientGamePlayer::GetArenaBattleCurRanking(int iServerFlag)
{
	if (iServerFlag == LOCAL_SERVER)
	{
		return this->GetArenaBattleAssert()->GetLocalIndex();
	}
	else
	{
		return 0;
		//return this->GetArenaBattleAssert()->GetGlobalIndex();
	}
}
int LC_ClientGamePlayer::GetArenaBattleTopRanking(int iType)
{
	return this->GetArenaBattleAssert()->GetLocalTopIndex();
}
int LC_ClientGamePlayer::GetArenaBattleWinStreak(int iType)
{
	return this->GetArenaBattleAssert()->GetLocalWinStreak();
}
int LC_ClientGamePlayer::GetArenaBattleChallengeTimes(int iType)
{
	return this->GetArenaBattleAssert()->GetLocalTotalChallengeTime();
}
StringType LC_ClientGamePlayer::GetArenaBattleChallengeTimesStr(int iType)
{
	return this->GetArenaBattleAssert()->GetLocalTotalChallengeTimeStr();
}
int LC_ClientGamePlayer::GetArenaBattleLocalChallengedTime(int iType)
{
	return this->GetArenaBattleAssert()->GetLocalChallengedTime();
}
int LC_ClientGamePlayer::GetArenaBattleBuyCountCost(int iType)
{
	return this->GetArenaBattleAssert()->GetLocalDailyChallengeBuyCount()*SOLO_DULE_COST/2+SOLO_DULE_COST;
}
int LC_ClientGamePlayer::GetArenaBattleDailyBuyCount(int iType)
{
	return this->GetArenaBattleAssert()->GetLocalDailyChallengeBuyCount();
}
int LC_ClientGamePlayer::GetLocalDailyChallengeMaxTime()
{
	return GetArenaBattleAssert()->GetLocalDailyChallengeMaxTime();
}
LC_ClientArenaBattleChallengeInfoAsset* LC_ClientGamePlayer::GetArenaBattleChallengeInfoAsset()
{
	return &m_kArenaBattleChallengeInfoAsset;
}
bool LC_ClientGamePlayer::CanAddGood(object_id_type uid, int iType, int iServerFlag)
{
	return GetRankAssert()->CanAddGood(uid, getRankIDByType(iType, iServerFlag)) == RE_SUCCESS;
}
int LC_ClientGamePlayer::GetLeftAddGoodCount()
{
	return GetRankAssert()->GetLeftCount();
}

//End 竞技场---------------------------------------------------------------------------------------

void LC_ClientGamePlayer::UpdatePortalData(const Protocol::MG_PortalAssert& kMsg)
{
	//
	LC_PortalAssert* pkPortalAssert = GetPortalAsset();
	if (NULL == pkPortalAssert)
		return;

	LC_PortalEntry* pkPortalEntry = pkPortalAssert->GetMapPortal(GetMapResID());
	if (pkPortalEntry && pkPortalEntry->m_lNPCID > 0)
	{
		LC_ClientNPCManager* pkLocalNpcManager = CLIENT_GET_LOCAL_NPC_MANAGER();
		pkLocalNpcManager->DestroyLocalNPC(pkPortalEntry->m_lNPCID);
	}
	//同步数据
	pkPortalAssert->GetDataFromProtocolStruct(kMsg);

	pkPortalEntry = pkPortalAssert->GetMapPortal(GetMapResID());
	if (pkPortalEntry)
		CreatePortal();
}

void LC_ClientGamePlayer::CreatePortal()
{
	LC_PortalAssert* pkPortalAssert = GetPortalAsset();
	LC_PortalEntry* pkPortalEntry = pkPortalAssert->GetMapPortal(GetMapResID());
	if (!pkPortalEntry)
	{
		return;
	}
	if (pkPortalEntry->m_MapResId != GetMapResID() || pkPortalEntry->m_lNPCID > 0)
		return;

	//创建本地npc
	LC_ClientNPCManager* pkLocalNpcManager = CLIENT_GET_LOCAL_NPC_MANAGER();
	long lNPCID = pkLocalNpcManager->CreateLocalNPC(pkPortalEntry->m_CharType, pkPortalEntry->m_Pos);

	LC_ClientNPC* pkNPC = (LC_ClientNPC*)(pkLocalNpcManager->FindNPC(lNPCID));
	pkNPC->SetNPCType(CLIENT_NPC_TYPE_PORTAL);
	pkNPC->SetNPCFuncState(NFT_PORTAL, (pkPortalEntry->m_TargetMapResId << 8) | CLIENT_NPC_PORTAL_TOWN);
	pkPortalEntry->m_lNPCID = lNPCID;
}

unique_id_type LC_ClientGamePlayer::GetTmpSummorPool(int index)
{
	if (index >= 0 && index < MAX_SUNMOR_FIREND_POOL_CAP)
		return m_summorLis[index];
	else
		return 0;
}

void LC_ClientGamePlayer::SetTmpSummorPool(int index, int val)
{
	if (index >=  0 && index < MAX_SUNMOR_FIREND_POOL_CAP)
		m_summorLis[index] = val;
}

int LC_ClientGamePlayer::GetTmpSummorCurCount()
{
	int count = 4;
	for(unsigned int i = 0 ; i < MAX_SUNMOR_FIREND_POOL_CAP; i++)
	{
		if (m_summorLis[i] == 0 )
		{
			count = i;
			break;
		}
	}
	return count;
}

void LC_ClientGamePlayer::ClearTmpSummorPool()
{
	for(int i = 0 ; i < MAX_SUNMOR_FIREND_POOL_CAP; i++)
	{
		m_summorLis[i]=0;
	}
}

//------------------------------------------------------------------------------
void LC_ClientGamePlayer::PunchMove(SkillMoveActorPositionType Type, const Utility::UT_Vec3Int & kTargetPoint,float fPunchTime)
{
	LC_PlayerBase::PunchMove(Type, kTargetPoint, fPunchTime);

	switch(Type)
	{
	case SMAPT_PUNCHBACK:
		//this->PlayAnimation(LC_Helper::GetPlayerBeAttackAnimName(this));
		break;
	case SMAPT_PASSIVEMOVE:
		this->PlayAnimation(RES_ANM_ACTOR_UNDERATTACK);
		break;
	case SMAPT_HITDOWN:
		//this->PlayAnimation(RES_ANM_ACTOR_HITDOWN);
		break;
	case SMAPT_HITFLY:
		this->PlayAnimation(RES_ANM_ACTOR_HITFLY);
		break;
	default:
		return;
	}

	if ( Type == SMAPT_PUNCHBACK)
	{
		this->FixCurrentState(ST_PLAYER_PUNCH_BACK, GET_PROCESS_TIME, 0);
	}
	else
	{
		this->FixCurrentState(ST_PLAYER_PASSIVE_MOVE, GET_PROCESS_TIME, 0);
	}
}

//------------------------------------------------------------------------------
bool LC_ClientGamePlayer::IsGravePressed() const
{
	return IP_InputManager::GetSingleton().KeyIsDown(NiInputKeyboard::KEY_GRAVE);
}

bool LC_ClientGamePlayer::CheckGameStoryCondition(const GameStoryInfo& kGameStoryInfo, int iMapID)
{
	LC_ClientGameStoryManager* pkGameStoryManager = CLIENT_GET_GAMESTORY_MANAGER();
	if (!pkGameStoryManager)
		return false;

	//判断等级
	int iMinLevel = kGameStoryInfo.GetEnterMinLevel();
	int iMaxLevel = kGameStoryInfo.GetEnterMaxLevel();
	/*int iCurLevel = GetLevel();*/
	int iCurLevel = GetCommanderLevel();
	if (iCurLevel < iMinLevel || iCurLevel > iMaxLevel)
		return false;

	//公会秘境  在活动时间内可开启 开启后持续时间可超出活动时间 因此用boss状态来判断
	int iTabType = kGameStoryInfo.GetTabType();
	if (iTabType == ACTIVITY_TT_GUILDBOSSACTIVITY && iMapID != 0)
	{
		mem::map<int32_t, PS_GuildBossInfo>::iterator iter = m_mGuildBossActivity.find(iMapID);
		if (iter != m_mGuildBossActivity.end())
		{
			return iter->second.m_nState == 2;
		}
	}

	return kGameStoryInfo.GetActive() > 0;
}

void LC_ClientGamePlayer::_updateComboCount(int iOldValue, int iAddCount)
{
	LC_PlayerBase::_updateComboCount(iOldValue, iAddCount);
	int iComboCount = GetComboCount();

	if (iComboCount > MAX_SUMMOR_COMBO_COUNT)
		iComboCount = MAX_SUMMOR_COMBO_COUNT;
	if (iComboCount == 50 || iComboCount == 100 || iComboCount == 500 || iComboCount == 1000 || iComboCount == MAX_SUMMOR_COMBO_COUNT)
	{
		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		MG_NotifyComboCount reqMsg;
		reqMsg.m_ComboCount = iComboCount;

		bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_NOTIFY_COMBO_COUNT, &reqMsg);
		if(bCreateMsg)
			m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}

	if (iComboCount == 0)
		return;

	int iType = 0;
	if (iComboCount >= 1000)
		iType = 3;
	else if (iComboCount >= 100)
		iType = 2;
	else if(iComboCount >= 10)
		iType = 1;
	else if (iComboCount > 1)
		iType = 0;
	MG_MessageCenter::GetSingleton().SendMessage(CreateMsg_UE_PLAYER_SHOW_COMBO(GetID(), iComboCount, iType, m_iComboEffectCount));
}

void LC_ClientGamePlayer::_clearComboCount()
{
	LC_PlayerBase::_clearComboCount();

	m_iComboEffectData = 0;
	m_iComboEffectCount = 0;
}

void LC_ClientGamePlayer::PlayerComboEffect(int iCount)
{
	m_iComboEffectCount = iCount;
	if (iCount == 0)
	{
		return;
	}
	if (m_iComboEffectData == 0x3F)
		return;

	int iLeftCount = COMBO_EFFECT_ROUNT - iCount;
	int iRand = UT_MathBase::RandInRangeInt(0, iLeftCount);

	int iIndex = -1;
	for (int i = 0; i < COMBO_EFFECT_ROUNT; i++)
	{
		int iValue = (1 << i);
		if (int(m_iComboEffectData & iValue) == 0)
		{
			iRand--;
			if (iRand == 0)
			{
				m_iComboEffectData |= iValue;
				iIndex = i;
				break;
			}
		}
	}
	if (iIndex < 0)
		return;

//	static const char* kGfxComboEffectList[] = {RES_FX_SCREEN_BLOODSHED1, RES_FX_SCREEN_BLOODSHED2, RES_FX_SCREEN_BLOODSHED3,
//		RES_FX_SCREEN_BLOODSHED4, RES_FX_SCREEN_BLOODSHED5, RES_FX_SCREEN_BLOODSHED6};
//
//	IM_Manager* pkIMManager = IM_Manager::GetSingletonPtr();
//	if(pkIMManager)
//	{
//		GD_ClientActorModel* pkVisibleData = pkIMManager->GetActor(GetID());
//		if(pkVisibleData)
//		{
//			pkVisibleData->AttachFx(GD_ModelBase::FDT_INVALID, FileSystem::MTF_ModelData::AFBM_TRANSLATE_ROTATE, true, kGfxComboEffectList[iIndex]);
//		}
//	}
}

void LC_ClientGamePlayer::RewardComboCount(int iOldValue, int iAddCount)
{
	m_iComboEffectData = 0;
	_updateComboCount(iOldValue, iAddCount);

	MG_MessageCenter::GetSingleton().SendMessage(CreateMsg_UE_PLAYER_DELTA_COMBO(GetID(), iAddCount));
}

LC_ClientNetPlayer* LC_ClientGamePlayer::GetSummoredPlayerData()
{
	LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();

	if (m_summorFriendCharID != 0 )
	{
		int count = pkNetPlayerManager->GetNetPlayerCount();
		for(int i = 0 ; i < count; i++ )
		{
			LC_ClientNetPlayer* netPlayer = (LC_ClientNetPlayer* )pkNetPlayerManager->GetNetPlayerPtrByIndex(i);
			if (netPlayer)
			{
				if (netPlayer->IsClone() && netPlayer->GetInstance() == m_summorFriendCharID )
				{
					return netPlayer;
				}
			}
		}
		m_summorFriendCharID = 0;
	}

	return NULL;
}

void LC_ClientGamePlayer::SetSummorFriendCharID(uint32_t iID)
{
	m_summorFriendCharID = iID;
}

int LC_ClientGamePlayer::GetFriendOffLineTime(int offLineTick)
{
	uint64_t time = GetServerTimeVal() - offLineTick;
	return time;
}

//每日任务杀怪计数 2014/4/11 林培华
int	LC_ClientGamePlayer::GetDaylyTaskKillCount(int taskid)
{
	mem::map<int,int>::iterator it = m_mapDaylyTaskKillCount.find(taskid);
	if (it != m_mapDaylyTaskKillCount.end())
		return it->second;
	else
		return 0;
}

void LC_ClientGamePlayer::SetDaylyTaskKillCount(int taskid, int icount)
{
	m_mapDaylyTaskKillCount[taskid] = icount;
}

void LC_ClientGamePlayer::AddDirtyFlagRevisionID(int iFlag)
{
	switch(iFlag)
	{
		case DIRTY_FLAG_ACTIVEBUDDYLIST:
			m_DFBuddyActiveList.AddRevisionID();
			break;
		case DIRTY_FLAG_INACTIVEBUDDYLIST:
			m_DFBuddyInActiveList.AddRevisionID();
			break;
		case DIRTY_FLAG_CURRENTINDEX:
			m_DFCurrentIndex.AddRevisionID();
			break;
		case DIRTY_FLAG_STARVEIN:
			m_DFStarVein.AddRevisionID();
			break;
		case DIRTY_FLAG_STARVEIN_GOAL:
			m_DFStarVeinGoal.AddRevisionID();
			break;
		case DIRTY_FLAG_BUDDY_SCHEME_LIST:
			m_DFCurrentSchemeList.AddRevisionID();
			break;
		case DIRTY_FLAG_NEARBY_PLAYERINFO:
			m_DFCurMapPlayerInfo.AddRevisionID();
			break;
		case DIRTY_FLAG_RAIDGLORYINFO:
			m_DFRaidGloryInfo.AddRevisionID();
			break;
		case DIRTY_FLAG_NPCFUNCTION:
			m_DFNPCFunction.AddRevisionID();
			break;
		default:
			if (iFlag > DIRTY_FLAG_PLAYER_DETAIL_BEGIN && iFlag < DIRTY_FLAG_PLAYER_DETAIL_END)
				m_kPlayerDetailInfoData.SetDirtyFlag(iFlag);
			else
				LC_PlayerBase::AddDirtyFlagRevisionID(iFlag);
			break;
	}
}

int LC_ClientGamePlayer::GetDirtyFlagRevisionID(int iFlag)
{
	switch(iFlag)
	{
		case DIRTY_FLAG_ACTIVEBUDDYLIST:
			return m_DFBuddyActiveList.GetRevisionID();
			break;
		case DIRTY_FLAG_INACTIVEBUDDYLIST:
			return m_DFBuddyInActiveList.GetRevisionID();
			break; 
		case DIRTY_FLAG_CURRENTINDEX:
			return m_DFCurrentIndex.GetRevisionID();
			break;
		case DIRTY_FLAG_STARVEIN:
			return m_DFStarVein.GetRevisionID();
			break;
		case DIRTY_FLAG_STARVEIN_GOAL:
			return m_DFStarVeinGoal.GetRevisionID();
			break;
		case DIRTY_FLAG_TIMELIMIT_SHOP:
			return m_DFShopTimeLimit.GetRevisionID();
			break;
		case DIRTY_FLAG_TASK_CHAPTER:
			{
				LC_ClientTaskManager* pkManager = (LC_ClientTaskManager*)LC_ClientTaskManager::GetSingletonPtr();
				return pkManager->GetDirtyFlagRevisionID();
			}
		case DIRTY_FLAG_BUDDY_SCHEME_LIST:
			return m_DFCurrentSchemeList.GetRevisionID();
		case DIRTY_FLAG_COMMANDER_PROPERTY:
			return m_pkCommander->GetDirtyFlagRevisionID();
		case DIRTY_FLAG_NEARBY_PLAYERINFO:
			return m_DFCurMapPlayerInfo.GetRevisionID();
		case DIRTY_FLAG_RAIDGLORYINFO:
			return m_DFRaidGloryInfo.GetRevisionID();	
		case DIRTY_FLAG_NPCFUNCTION:
			return m_DFNPCFunction.GetRevisionID();
		case DIRTY_FLAG_CUSTOM_CASH37:
			{
				LC_Money* pkHolidayMoney = NULL;
				GetCashByCashType(CT_CASH_TYPE_37, &pkHolidayMoney);
				return pkHolidayMoney->GetDirtyFlagRevisionID();
			}
		default:
			if (iFlag > DIRTY_FLAG_PLAYER_DETAIL_BEGIN && iFlag < DIRTY_FLAG_PLAYER_DETAIL_END)
				return m_kPlayerDetailInfoData.GetDirtyFlagRevisionID(iFlag);

			return LC_PlayerBase::GetDirtyFlagRevisionID(iFlag);
			break;
	}
}

void LC_ClientGamePlayer::AcceptTaskAfterLogin()
{
	if (IsCanAcceptTaskLogin())
		return;

	SetAcceptTaskLogin(true);
	AcceptHolidayTasks(HOLIDAYACTIVITY_TYPE_WUYI);
	AcceptSpecialTask(STY_SKYARENA); 
}

LC_ClientRankUserInfo*	LC_ClientGamePlayer::GetPlayerDetailInfoByUserID(unique_id_type iUserID)
{
	return m_kPlayerDetailInfoData.GetUserInfo(iUserID);
}

StringType	LC_ClientGamePlayer::GetGuildWarData()
{
	LC_GuildWarAssert* pkGuildWarAssert = GetGuildWarAssert();
	StringStreamType sStream;
	sStream << pkGuildWarAssert->GetMultiple();
	sStream << "#";
	sStream << pkGuildWarAssert->GetExp();
	sStream << "#";
	sStream << pkGuildWarAssert->GetLiveTime();

	return sStream.str();
}
StringType LC_ClientGamePlayer::GetReturnCityName()
{
	CF_WorldMapList::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(GetMapResID());
	if(NULL == pkData)
		return "";

	int CityMapResID = pkData->_iReturnBackResId;
	pkData = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(CityMapResID);
	if (NULL == pkData)
		return "";
	return pkData->_kName;
}

bool LC_ClientGamePlayer::IsRegistThisMonth()
{
	int32_t iRegTime = GetRegTime();
	int64_t iServerTime = GetServerTimeVal();
	return LC_Helper::CheckIsSameMonth((uint32_t)iRegTime,(uint32_t)iServerTime);
}
StringType LC_ClientGamePlayer::GetRegistTimeInfo()
{
	int32_t iRegTime = GetRegTime();
	time_t tTime1		=iRegTime; 
	tm pkTime	    = *localtime(&tTime1); 

	StringType RegTime = "";

	int day,mon, year; 
	day						=	pkTime.tm_mday; 
	mon						=	pkTime.tm_mon; 
	year					=	pkTime.tm_yday +1900;

	StringType strMon	=	Int2String(mon);
	StringType strDay	=	Int2String(day);
	StringType strYear	=	Int2String(year);
	RegTime = strYear + "#" + strMon + "#" + strDay;
	
	return RegTime;
}

int LC_ClientGamePlayer::GetRegDays()
{
	return LC_Helper::GetDeltaDay(GetRegTime(), GetServerTimeVal()) + 1;
}

bool LC_ClientGamePlayer::IsEquipBetter(LC_BackPackEntry *pkEntry)
{
	int iItemID = pkEntry->GetTypeID();
	if(!IS_EQUIP_ID(iItemID))
		return false;

	CF_ItemList::DataEntry *pkItemInfo = SafeGetCSVFileDataEntryPtr<CF_ItemList>(iItemID);
	if(NULL == pkItemInfo)
		return false;

	CF_ItemEquipLimitFunc::DataEntry *pkItemLmt = SafeGetCSVFileDataEntryPtr<CF_ItemEquipLimitFunc>(pkItemInfo->_lEquipLimitFuncID);
	if(NULL == pkItemLmt)
		return false;

	if(GetCommanderLevel() < pkItemLmt->_lLevelLimit)
		return false;

	int iPackType = pkItemLmt->_lPackType;
	if(iPackType > EquipPack_Count)
		return false;

	//成长系统等级不足
	LC_SubordinateAsset& kSubAsset = GetSubordinateAsset();
	UT_SIMDataList::iterator itLmt = pkItemLmt->_kSubLimits.begin();
	for (; itLmt!=pkItemLmt->_kSubLimits.end(); ++itLmt)
	{
		LC_SubUpgradeData* pkSubData = kSubAsset.GetSubData(itLmt->ID());
		int32_t iLevel = pkSubData && pkSubData->IsEnabled() ? pkSubData->GetStage() : 0;

		if (iLevel < (int32_t)itLmt->ParamA())
		{
			return false;
		}
	}

	SC_ClientScriptInterface* pkClientInterface = SC_ClientScriptInterface::GetSingletonPtr();
	SC_ClientSkillInterface* pkInterface = pkClientInterface->GetSkillInterface();

	combat_score_type iCurScore = 0;
	LC_BackPackEntry *pkCurEquipEntry = GetEquipEntry(pkItemLmt->_lPackType, pkItemLmt->_lEquipType);
	if(pkCurEquipEntry->GetValid())
		iCurScore = pkInterface->GetEquipScoreByItemID(pkCurEquipEntry->GetTypeID());

	combat_score_type iNewEquipScore = pkInterface->GetEquipScoreByItemID(iItemID);
	return iNewEquipScore > iCurScore;
}

void LC_ClientGamePlayer::_onSkillFlagChanged(int64_t llOldAllFlag, int64_t llFlag)
{
	RG_RenderManager* pkRenderManager = RG_RenderManager::GetSingletonPtr();
	GD_ClientActorModel* pkVisibleData = IM_Manager::GetSingleton().GetActor(GetID());
	if (NULL == pkVisibleData)
		return;

	if (!(llOldAllFlag & ((int64_t)SK_WINDY)) && GetSkillFlag((int64_t)SK_WINDY))
	{
		//pkVisibleData->SetClothAndWeaponVisibility(false);
		
		pkVisibleData->SetAlpha(g_settings.DS_RESPAWN_ALPHA);

		float fParamList[4] = {0.0f};
		for (int i = 0; i < 4; i++)
			fParamList[i] = g_settings.FRESNEL_WHITE[i];
		pkVisibleData->SetFresnelParameter(fParamList);

		pkRenderManager->SetGreyScenePostProcActivity(true);
		pkRenderManager->GreySceneFadeOut(1.0f);

		m_fRespawnTimer = 5.0f;
		m_eRespawnFadeState = RG_RenderManager::SGFIOS_FADEOUT;
	}
	else if ( (llOldAllFlag & ((int64_t)SK_WINDY)) && !GetSkillFlag((int64_t)SK_WINDY) && m_eRespawnFadeState != RG_RenderManager::SGFIOS_INACTIVE)
	{
		pkVisibleData->FadeIn(g_settings.DS_RESPAWN_DELTA_ALPHA);

		float fParamList[4] = {0.0f};
		pkVisibleData->SetFresnelParameter(fParamList);
		//pkVisibleData->SetClothAndWeaponVisibility(true);

		pkRenderManager->GreySceneFadeIn(0.5f);

		m_fRespawnTimer = -1.0f;
		m_eRespawnFadeState = RG_RenderManager::SGFIOS_INACTIVE;
	}
}
bool LC_ClientGamePlayer::IsDailyRechargeRewardedToday()
{
	int iDailyRechargeLevel = m_kVipAsset.GetFDepositLevel();
	int iMaxNotDrawedLevel = GetDailyRechargeMaxNotDrawedLevel();
	if(iMaxNotDrawedLevel > DAILY_RECHARGE_REWARD_NUMBER)
	{
		return true;
	}
	else if (iDailyRechargeLevel >= DAILY_RECHARGE_REWARD_NUMBER)
	{
		return false;
	}
	else
	{
		if (!IsRechargeToday())
			return false;
		else if (iDailyRechargeLevel < iMaxNotDrawedLevel)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}
bool LC_ClientGamePlayer::IsRechargeToday()
{
	return m_kVipAsset.IsRechargeToday((uint32_t)GetServerTimeVal());
}
// 老版本写的有点死板，配合lua看 ，似乎只可能返回1，考虑是否要改
int	LC_ClientGamePlayer::GetDailyRechargeMaxNotDrawedLevel()
{
	int iResult = DAILY_RECHARGE_REWARD_NUMBER + 1;
	if (!m_kVipAsset.IsFDLVLDrawed(DAILY_RECHARGE_REWARD_NUMBER))
	{
		iResult = DAILY_RECHARGE_REWARD_NUMBER;
	}
	return iResult;
}

void LC_ClientGamePlayer::RequsetArenaGiftState()
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_Req_ChallengGiftState msg;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_GG_REQ_CHALLENGGIFTSTATE, &msg);

	if (bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

void LC_ClientGamePlayer::RequsetArenaRankList(int iBegin, int iEnd, int serverID)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_RankInfo msg;
	msg.m_RankID = -1;
	msg.m_Begin = iBegin;
	msg.m_End = iEnd;
	msg.m_ServerID = serverID;
	msg.m_nType = 1;  //iType:排行榜类型， 1：竞技场 0：普通排行榜
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_CHALLENG_RANK_REQ, &msg);

	GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"竞技场  发送请求MGPT_CHALLENG_RANK_REQ， m_Begin=%d, m_End=%d", iBegin, iEnd);

	if (bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

//---------------------------------------------------------------------------------------
void LC_ClientGamePlayer::RequestTransferToLocation(int iMapResID, int x, int y, int z, int iMapLogicID, int iServerID, int iLineNum)
{
	NW_ClientSessionManager* pkSessionManager	= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup* pkSessionGroup		= pkSessionManager->GetGameSessionGroup();

	CF_WorldMapList::DataEntry* pkWorldMapData = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(iMapResID);
	if (!pkWorldMapData)
		return;

	if(!CheckCanTransferMap(iMapResID))
		return;

	/*if (iServerID <= 0)
	{
		iServerID = GetServerID();
	}*/
	if (IsInCrossRealMapTeam() && !IS_NORMAL_MAP(pkWorldMapData->_iMapLogicType))
	{
		SendLogicInfoMessage(RE_IN_CROSS_MAP_TEAM_CAN_ENTER_AREA);
		return;
	}

	//距离太近就不用飞鞋，走过去
	if((iServerID == 0 || iServerID == pkSessionGroup->GetCurGameServer()) && GetMapResID() == iMapResID && UT_MathBase::LineLengthXYInt(GetCurrentLocation(), UT_Vec3Int(x,y,z)) < g_settings.DS_FLY_DIS_JUDGEMENT )
	{
		PathToPoint(UT_Vec3Int(x,y,z));
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

	//大恶人不能用飞鞋
	/*if ( IsRedNameInvalidTTT(iMapResID) )
	{
		SendLogicInfoMessage(RE_EVIL_TRANSFER);
		return ;
	}*/

	_resetAutoTask();

	MG_Req_TransferToLocation msg;
	msg.m_iLocX = x;
	msg.m_iLocY = y;
	msg.m_iLocZ = z;
	msg.m_iMapResID = iMapResID;
	msg.m_iMapLogicID = iMapLogicID;
	msg.m_iServerID = iServerID;
	msg.m_iLineNum = iLineNum;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_TRANSFER_TO_LOCATION, &msg);
	if(bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	SetTranToTranTick(GET_PROCESS_TIME);
}
//---------------------------------------------------------------------------------------
bool LC_ClientGamePlayer::IsRedNameInvalidTTT(int iMapResID)
{
	CF_WorldMapList::DataEntry* pkWorldMapData = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(iMapResID);
	if (!pkWorldMapData)
		return false;
	//红名玩家不能在野外之间只用飞鞋
	return (GetPKRecorder()->GetPKStateType() == PST_RED && IS_NORMAL_MAP(pkWorldMapData->_iMapLogicType));
}

void LC_ClientGamePlayer::EquipAdvanceReq(int InType, int InIndex)
{
	MG_CS_EquipAdvance_Req msg;
	msg.m_Type = InType;
	msg.m_Index = InIndex;
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_CS_EquipAdvance_Req, &msg);
	if(bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

void LC_ClientGamePlayer::CimeliaEquipAdvanceReq(int InType, int InIndex)
{
	MG_REQ_SubEquipAdvance msg;
	msg.m_nSubType = InType;
	msg.m_nSlotIndex = InIndex;
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SUBEQUIPADVANCE, &msg);
	if(bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

//---------------------------------------------------------------------------------------
void LC_ClientGamePlayer::RequestMemberAutoAdd(bool bAdd)
{
	MG_REQ_Member_AutoAdd kReqMsg;

	kReqMsg.m_AutoAdd = bAdd;
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_MEMBERAUTOADD_CHANGE, &kReqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

ResultType LC_ClientGamePlayer::CanExitGuild(LC_GuildAsset* pkGuildAsset)
{
	// 是否有门派
	if (GetGuildID() == INVALID_GUILD_ID)
	{
		return RE_EXIT_GUILD_FAILED_NOT_EXIST_GUILD;
	}
	LC_GuildMemberInfo& rkMemberInfo = pkGuildAsset->GetGuildMemberInfo();
	LC_GuildMemberEntry* pkMemberEntry = rkMemberInfo.FindMemeberEntry(GetCommanderUID());
	if (NULL == pkMemberEntry)
	{
		return RE_EXIT_GUILD_FAILED_NOT_EXIST_GUILD;
	}
	// 是否掌门
	if (pkMemberEntry->GetOfficialPosition() == GUILD_MEMBER_MASTER)
	{
		return RE_EXIT_GUILD_FAILED_MASTER_CANT_DO;
	}
	return RE_SUCCESS;
}

ResultType LC_ClientGamePlayer::CanModifyGuildDeclaration(LC_GuildAsset* pkGuildAsset)
{
	// 是否有门派
	if (GetGuildID() == INVALID_GUILD_ID)
	{
		return RE_MODIFY_GUILD_FAILED_NOT_EXIST_GUILD;
	}
	LC_GuildMemberInfo& rkMemberInfo = pkGuildAsset->GetGuildMemberInfo();
	LC_GuildMemberEntry* pkMemberEntry = rkMemberInfo.FindMemeberEntry(GetCommanderUID());
	if (NULL == pkMemberEntry)
	{
		return RE_MODIFY_GUILD_FAILED_NOT_EXIST_GUILD;
	}
	// 是否掌门或者护法
	if (pkMemberEntry->GetOfficialPosition() != GUILD_MEMBER_MASTER && pkMemberEntry->GetOfficialPosition() != GUILD_MEMBER_GUARDIAN)
	{
		return RE_MODIFY_GUILD_FAILED_NOT_MODIFY_POWER;
	}
	return RE_SUCCESS;
}

ResultType LC_ClientGamePlayer::CanKickMemberFromGuild(LC_GuildAsset* pkGuildAsset)
{
	if (GetGuildID() == INVALID_GUILD_ID)
	{
		return RE_GUILD_KICKMEMBER_FAILED_NOT_EXIST_GUILD;
	}
	LC_GuildMemberInfo& rkMemberInfo = pkGuildAsset->GetGuildMemberInfo();
	LC_GuildMemberEntry* pkMemberEntry = rkMemberInfo.FindMemeberEntry(GetCommanderUID());
	if (NULL == pkMemberEntry)
	{
		return RE_GUILD_KICKMEMBER_FAILED_NOT_EXIST_GUILD;
	}
	// 是否掌门
	if (pkMemberEntry->GetOfficialPosition() != GUILD_MEMBER_MASTER && pkMemberEntry->GetOfficialPosition() != GUILD_MEMBER_GUARDIAN && pkMemberEntry->GetOfficialPosition() != GUILD_MEMBER_ELDER)
	{
		return RE_GUILD_KICKMEMBER_FAILED_POWER;
	}
	return RE_SUCCESS;
}

void LC_ClientGamePlayer::SetGuildWarType(int iType)
{
	m_iGuildWarType = iType;

	LC_ClientGuildWarAsset* pkAsset = (LC_ClientGuildWarAsset*)GetGuildWarAssert();
	if(!pkAsset)
		return;

	pkAsset->SetType(iType);
}

void LC_ClientGamePlayer::SetPowerPlayer(unique_id_type iUserID)
{
	LC_ClientGuildWarAsset* pkAsset = (LC_ClientGuildWarAsset*)GetGuildWarAssert();
	if(!pkAsset)
		return;

	pkAsset->SetPowerPlayer(iUserID);
}

unique_id_type LC_ClientGamePlayer::GetPowerPlayer()const
{
	LC_ClientGuildWarAsset* pkAsset = (LC_ClientGuildWarAsset*)m_pkGuildwarAssert;
	if(!pkAsset)
		return 0;

	return pkAsset->GetPowerPlayer();
}

void LC_ClientGamePlayer::SetGuildWarTargetPos(int iPosX, int iPosY)
{
	LC_ClientGuildWarAsset* pkAsset = (LC_ClientGuildWarAsset*)GetGuildWarAssert();
	if(!pkAsset)
		return;
	pkAsset->SetTargetPos(iPosX, iPosY);

//	势力争夺战旗帜 箭头追踪显示
// 	const UT_Vec3Int & rkLocation = GetCurrentLocation();
// 
// 	int dist = UT_MathBase::LineLengthXYInt(rkLocation , UT_Vec3Int(iPosX, iPosY, rkLocation.z));
// 
// 	int iFxID = pkAsset->GetDirectionFxID();
// 	FX_ClientFxManager* pkFXManager = FX_ClientFxManager::GetSingletonPtr();
// 	if(iPosX <= 0 || iPosY <= 0 || dist < DIRECTION_SHOW_DISTANCE || GetPowerPlayer() == GetID())
// 	{
// 		if (iFxID != 0)
// 			pkFXManager->ReleaseFx(iFxID);
// 
// 		pkAsset->SetDirectionFxID(0);
// 	}
// 	else
// 	{
// 		if (iFxID <= 0)
// 		{
// 			iFxID = pkFXManager->CreateFx(RES_FX_DIRECTION, false);
// 			pkFXManager->PlayFx(iFxID,GET_PROCESS_TIME);
// 
// 			pkAsset->SetDirectionFxID(iFxID);
// 		}
// 
// 		NiPoint3 kLocationV3d(rkLocation.x, rkLocation.y, rkLocation.z);
// 		NiPoint3 kTargetV3d(iPosX, iPosY, rkLocation.z);
// 		NiPoint3 dir = kTargetV3d - kLocationV3d;
// 		if (GetRiderCharType() != 0)
// 		{
// 			dir.Unitize();
// 			NiPoint3 shapeLocation;
// 			kLocationV3d = kLocationV3d + dir * GetProperty_ShapeRadius();
// 		}
// 		pkFXManager->BindFxToLocation(iFxID, kLocationV3d, &dir);
// 	}
}

void LC_ClientGamePlayer::_updateDirectionFx()
{
	LC_ClientGuildWarAsset* pkAsset = (LC_ClientGuildWarAsset*)GetGuildWarAssert();
	if(!pkAsset)
		return;

	SC_ClientScriptInterface* pkScriptInterface = SC_ClientScriptInterface::GetSingletonPtr();
	SC_ClientSkillInterface*  pkSkillInterface  = pkScriptInterface->GetSkillInterface();

	bool isGuildWarMap = true;
	if (pkAsset->GetType() == WarType_DomainWar && (!pkSkillInterface->IsDomainMap(GetMapResID())))
		isGuildWarMap = false;
	else if ((pkAsset->GetType() == WarType_GuildWar || pkAsset->GetType() == WarType_CrossRealmGuildWar) && (GetMapLogicType() != MT_GUILDWAR))
		isGuildWarMap = false;
	
	FX_ClientFxManager* pkFXManager = FX_ClientFxManager::GetSingletonPtr();
	int iFxID = pkAsset->GetDirectionFxID();
	
	if(pkAsset->GetType() == WarType_None || !isGuildWarMap || GetPowerPlayer() == GetID())
	{
		if(iFxID > 0)
		{
			pkFXManager->ReleaseFx(iFxID);
		}

		pkAsset->SetDirectionFxID(0);
		return;
	}

	
	int iPosX = pkAsset->GetTargetPosX();
	int iPosY = pkAsset->GetTargetPosY();

	if (iPosX <= 0 || iPosY <= 0)
		return;
	
	const UT_Vec3Int & rkLocation = GetCurrentLocation();
	int dist = UT_MathBase::LineLengthXYInt(rkLocation , UT_Vec3Int(iPosX, iPosY, rkLocation.z));
	if (dist < DIRECTION_SHOW_DISTANCE)
	{
		pkFXManager->ReleaseFx(iFxID);
		pkAsset->SetDirectionFxID(0);
	}
	else
	{
		NiPoint3 kLocationV3d(rkLocation.x, rkLocation.y, rkLocation.z);
		NiPoint3 kTargetV3d(iPosX, iPosY, rkLocation.z);
		NiPoint3 dir = kTargetV3d - kLocationV3d;
		if (GetRiderCharType() != 0)
		{
			dir.Unitize();
			NiPoint3 shapeLocation;
			kLocationV3d = kLocationV3d + dir * GetProperty_ShapeRadius();
		}
		if (iFxID <= 0)
		{
			iFxID = pkFXManager->CreateFx(RES_FX_DIRECTION, false);
			pkFXManager->PlayFx(iFxID,GET_PROCESS_TIME);

			pkAsset->SetDirectionFxID(iFxID);
		}
		
		pkFXManager->BindFxToLocation(iFxID, kLocationV3d, &dir);
	}	
}

void LC_ClientGamePlayer::_updateGameStoryDirectionFx()
{
	LC_ClientGameStoryManager* pkGameStoryManager = (LC_ClientGameStoryManager*)GetGameStoryManager();
	if(!pkGameStoryManager)
		return;

	bool isGameStoryMap = false;
	if (pkGameStoryManager->IsMapResIDInGameStory(GAMESTORY_ID_QianLiHuLiang))
		isGameStoryMap = true;
	else if (pkGameStoryManager->IsMapResIDInGameStory(GAMESTORY_ID_BangHuiYunBiao))
		isGameStoryMap = true;
	else if (pkGameStoryManager->IsMapResIDInGameStory(GAMESTORY_ID_BianJingYunBiao))
		isGameStoryMap = true;

	FX_ClientFxManager* pkFXManager = FX_ClientFxManager::GetSingletonPtr();
	int iFxID = pkGameStoryManager->GetDirectionFxID();
	int iTargetGameStoryID = pkGameStoryManager->GetTargetGameStoryID();
	if((iTargetGameStoryID != GAMESTORY_ID_QianLiHuLiang && iTargetGameStoryID != GAMESTORY_ID_BangHuiYunBiao && iTargetGameStoryID != GAMESTORY_ID_BianJingYunBiao) || !isGameStoryMap)
	{
		if(iFxID > 0)
		{
			pkFXManager->ReleaseFx(iFxID);
		}

		pkGameStoryManager->SetDirectionFxID(0);
		return;
	}


	int iPosX = pkGameStoryManager->GetTargetPosX();
	int iPosY = pkGameStoryManager->GetTargetPosY();

	if (iPosX <= 0 || iPosY <= 0)
		return;

	const UT_Vec3Int & rkLocation = GetCurrentLocation();
	int dist = UT_MathBase::LineLengthXYInt(rkLocation , UT_Vec3Int(iPosX, iPosY, rkLocation.z));
	if (dist < DIRECTION_SHOW_DISTANCE_GAMESTORY)
	{
		pkFXManager->ReleaseFx(iFxID);
		pkGameStoryManager->SetDirectionFxID(0);
	}
	else
	{
		NiPoint3 kLocationV3d(rkLocation.x, rkLocation.y, rkLocation.z);
		NiPoint3 kTargetV3d(iPosX, iPosY, rkLocation.z);
		NiPoint3 dir = kTargetV3d - kLocationV3d;
		if (GetRiderCharType() != 0)
		{
			dir.Unitize();
			NiPoint3 shapeLocation;
			kLocationV3d = kLocationV3d + dir * GetProperty_ShapeRadius();
		}
		if (iFxID <= 0)
		{
			iFxID = pkFXManager->CreateFx(RES_FX_DIRECTION, false);
			pkFXManager->PlayFx(iFxID,GET_PROCESS_TIME);

			pkGameStoryManager->SetDirectionFxID(iFxID);
		}

		pkFXManager->BindFxToLocation(iFxID, kLocationV3d, &dir);
	}	
}

int LC_ClientGamePlayer::GetCanDrawRewardsNumberOfGuildWar(int iGuildWarType)
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if (NULL == pkLogicManager )
		return 0;

	LC_WorldGuildWarAsset* kWorldGuildWarAssert = pkLogicManager->GetGuildWarAssert();
	if ( kWorldGuildWarAssert == NULL )
		return 0;

	if (iGuildWarType == WarType_GuildWar)
	{
		const Protocol::PS_PowerGuildInfo* pkGuildInfo = kWorldGuildWarAssert->GetPowerGuildInfo(GetActiveServer());
		if (NULL == pkGuildInfo)
			return 0;
		if (GetGuildID() != 0 && pkGuildInfo->m_GuildId == GetGuildID() && GetGuildWarAssert()->CheckPowerGuildReward(pkGuildInfo->m_TimeStamp))
			return 1;
	}
	else if (iGuildWarType == WarType_CrossRealmGuildWar)
	{
		const Protocol::PS_CrossRealm_PowerGuildInfo* pkGuildInfo = kWorldGuildWarAssert->GetGlobalPowerGuildInfo(GetGuildGroup());
		if (NULL == pkGuildInfo)
			return 0;
		if (GetGuildID() != 0 &&pkGuildInfo->m_GuildId == GetGuildID() && GetGuildWarAssert()->CheckGlobalPowerGuildReward(pkGuildInfo->m_TimeStamp))
			return 1;
	}
	else if (iGuildWarType == WarType_DomainWar)
	{
		const DomainPowerMap* pkDomainMap = kWorldGuildWarAssert->GetDomainPowerGuildInfo(GetActiveServer());
		if (NULL == pkDomainMap)
			return 0;
		DomainPowerMap::const_iterator it = pkDomainMap->begin();
		int RewardNumber = 0;
		for (; it != pkDomainMap->end(); ++it)
		{
			//if (GetGuildID() != 0 && it->second.m_GuildId == GetGuildID() && GetPlayerGuildAssert()->CheckDomainReward(it->second.m_ResId, it->second.m_TimeStamp) && LC_Helper::CanEnterMap(it->second.m_ResId, GetLevel()) == RE_SUCCESS)
			if (GetGuildID() != 0 && it->second.m_GuildId == GetGuildID() && GetPlayerGuildAssert()->CheckDomainReward(it->second.m_ResId, it->second.m_TimeStamp) && LC_Helper::CanEnterMap(it->second.m_ResId, GetCommanderLevel()) == RE_SUCCESS)
				RewardNumber++;
		}
		return RewardNumber;
	}

	return 0;
}

bool LC_ClientGamePlayer::IsSafeArea()
{
	CF_LogicAreaFunc::DataEntry* pkLogicAreaFunc = GetLogicAreaFunc();
	if (NULL != pkLogicAreaFunc)
	{
		return pkLogicAreaFunc->_lAreaType == AT_SAFE_AREA;
	}
	return false;
}

void LC_ClientGamePlayer::SetGuildName(const StringType& strGuildName)
{
	//GUISys::getRichTextParse->FilterForbiddenText(strGuildName, m_kGuildName, TT_TextFilter::TFT_CHAR_NAME);
	m_kGuildName = strGuildName;
	m_kGuildName_utf8 = CodeConvert::MbsToUtf8(m_kGuildName);
	UpdateDirtyFlag(DIRTY_FLAG_SOCIETY);
}

void LC_ClientGamePlayer::RequestCommonReward(int type, int op_code, int op_para)
{
	MG_PB<msg_define::common_stat_operation_req> kReqMsg;
	kReqMsg.m_value.set_type(type);
	kReqMsg.m_value.set_op_code(op_code);
	kReqMsg.m_value.set_op_para(op_para);
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_COMMON_STAT_OPERATION_REQ, &kReqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
	
	//this->GetCommonStatAsset().RegLoginRewarded(op_para);
	//this->GetCommonStatAsset().UpdateDirtyFlag();
}

bool LC_ClientGamePlayer::IsInFinishAchievementMap(int achievementID)
{
	return GetAchievementAsset().IsInFinishAchievementRecord(achievementID);
}

ResultType LC_ClientGamePlayer::CheckAchievementCanFinish(int achievementID)
{
	return GetAchievementAsset().CheckAchievementCanFinish(achievementID);
}

bool LC_ClientGamePlayer::IsFinishedAchievement(int achievementID)
{
	if(IsInFinishAchievementMap(achievementID))
		return true;

	ResultType result = CheckAchievementCanFinish(achievementID);
	return result == RE_SUCCESS || result == RE_ACHIEVEMENT_REWARD_HAS_RECEIVED;
}

void LC_ClientGamePlayer::RequestAchievementReward(int iAchievementID)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	MG_REQ_AchievementReward kReqMsg;
	kReqMsg.m_nAchievementID = iAchievementID;

	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_ACHIEVEMENT_REWARD, &kReqMsg);
	GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM, "MGPT_REQ_ACHIEVEMENT_REWARD id = %d",iAchievementID);
	if (bCreateMsg)
	{
		GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM, "MGPT_REQ_ACHIEVEMENT_REWARD bCreateMsg success id = %d",iAchievementID);
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::RequestAchievementPointReward(int iRewardID)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	MG_REQ_AchievementPointReward kReqMsg;
	kReqMsg.m_iRewardID = iRewardID;

	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_ACHIEVEMENTPOINT_REWARD, &kReqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::RequestSevenDaysAchievementPointReward(int iRewardID)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	MG_REQ_GetSevenDayReward kReqMsg;
	kReqMsg.m_nID = iRewardID;

	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GETSEVENDAYREWARD, &kReqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::ReqSetAtvChargeRedPoint(int iScheduleId)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	MG_Req_SetAtvCharge_RedPoint kReqMsg;
	kReqMsg.m_nScheduleId = iScheduleId;

	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SET_ATVCHARGE_REDPOINT, &kReqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::UpdateAtvChargeRedPoint(int iScheduleId)
{
	m_vAtvChargePoint.push_back(iScheduleId);
}

bool LC_ClientGamePlayer::GetAtvChargeRedPointState(int iScheduleId)
{
	for (int i = 0; i < m_vAtvChargePoint.size(); ++i)
	{
		if (iScheduleId == m_vAtvChargePoint[i])
			return false;
	}
	return true;
}

void LC_ClientGamePlayer::RequestCommonBaseReward(int type, int op_code, int op_para)
{
	MG_PB<msg_define::common_stat_operation_req> kReqMsg;
	kReqMsg.m_value.set_type(type);
	kReqMsg.m_value.set_op_code(op_code);
	kReqMsg.m_value.set_op_para(op_para);
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_COMMON_STAT_BASIC_OPERATION_REQ, &kReqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}

	//this->GetCommonStatAsset().RegLoginRewarded(op_para);
	//this->GetCommonStatAsset().UpdateDirtyFlag();
}

void LC_ClientGamePlayer::RequestActivityCommonOp(int type, int catagory, int schedule, int opcode, const StringType& params)
{
	MG_PB<msg_define::activity_common_op_req> kReqMsg;
	kReqMsg.m_value.set_type(type);
	kReqMsg.m_value.set_catagory(catagory);
	kReqMsg.m_value.set_schedule(schedule);
	kReqMsg.m_value.set_code(opcode);
	TStringVector kStringVector = UT_BaseStringUtility::SplitString(params, '#');
	for (int i = 0; i < Activity::ACT_COMMON_OP_PARAM_SIZE; ++i)
	{
		if (i < kStringVector.size())
		{
			kReqMsg.m_value.add_params(boost::lexical_cast<int32_t>(kStringVector[i]));
		}
		else
		{
			kReqMsg.m_value.add_params(0);
		}
	}

	kReqMsg.m_value.set_count(kReqMsg.m_value.params(ACT_COMMON_OP_COUINT));
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_ACTIVITY_COMMON_OP_REQ, &kReqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::RequestActivityCommonOpEx(int type, int catagory, int schedule, int opcode, const StringType& params)
{
	MG_PB<msg_define::activity_common_op_req> kReqMsg;
	kReqMsg.m_value.set_type(type);
	kReqMsg.m_value.set_catagory(catagory);
	kReqMsg.m_value.set_schedule(schedule);
	kReqMsg.m_value.set_code(opcode);
	TStringVector kStringVector = UT_BaseStringUtility::SplitString(params, '#');
	for (int i = 0; i < kStringVector.size(); ++i)
	{
		kReqMsg.m_value.add_params(boost::lexical_cast<int32_t>(kStringVector[i]));
	}
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_ACTIVITY_COMMON_OP_REQ, &kReqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::RequestGetRankSubBInfo(int iType, int iGroupID, int iMinIndex, int iMaxIndex)
{
	MG_REQ_GetRankSubBInfo reqMsg;
	reqMsg.m_nType = iType;
	reqMsg.m_nID = iGroupID;
	reqMsg.m_nMinIndex = iMinIndex;
	reqMsg.m_nMaxIndex = iMaxIndex;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GETRANKSUBBINFO, &reqMsg);
	if(bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}

void LC_ClientGamePlayer::RequestShotActivityInfo(const StringType &idx)
{
	MG_PB<msg_define::shot_activity_info_req> kReqMsg;

	TStringVector kStringVector = UT_BaseStringUtility::SplitString(idx, '#');
	for (int i = 0; i < kStringVector.size(); ++i)
	{
		kReqMsg.m_value.add_ids(boost::lexical_cast<int32_t>(kStringVector[i]));
	}
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SHOT_ACTIVITY_INFO, &kReqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::RequestGetNearbyPlayerInfo()
{
	MG_REQ_GetNearbyPlayer kReqMsg;
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GETNEARBYPLAYERINFO, &kReqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::UpdateSpecialTrainingLevel(int InLevel)
{
	m_pkCommander->m_iSpecialTraningLevel=InLevel;
}

void LC_ClientGamePlayer::AddEnemyPlayer(unique_id_type id)
{
	for (int i=0;i<3;i++)
	{
		if(id ==m_ShowEnemyPlayers[i])
		{
			return;
		}
	}
	for (int i=0;i<3;i++)
	{
		if (m_ShowEnemyPlayers[i]==0)
		{
			m_ShowEnemyPlayers[i]=id;
			return;
		}
	}
}

void LC_ClientGamePlayer::RemoveEnemyPlayerByIdx(int EnemyIdx)
{
	if (EnemyIdx>=0&&EnemyIdx<=2)
	{
		m_ShowEnemyPlayers[EnemyIdx]=0;
	}
}

void LC_ClientGamePlayer::RefreshShowEnemyPlayer()
{
	//根据在不在附近更新敌人表的状态
	//3种情况下需要移除该敌人，敌人离开地图，敌人死亡，敌人变成队友
	LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
	bool bInAreana = IS_ARENA_MAP(GetMapLogicType());
	for (int i=0;i<3;i++)
	{
		if (bInAreana)
		{
			m_ShowEnemyPlayers[i]=0;
		}
		if(m_ShowEnemyPlayers[i]==0)
		{
			continue;
		}
		LC_ClientNetPlayer* pkNetPlayer = (LC_ClientNetPlayer*)(pkNetPlayerManager->FindPlayer(m_ShowEnemyPlayers[i]));
		//找不到玩家或者该玩家是队友，就移除该玩家
		if (!pkNetPlayer)
		{
			m_ShowEnemyPlayers[i]=0;
		}
		else
		{
			LC_PlayerGroupBase*	pGroup=GetPlayerGroup();
			if (pGroup&&pGroup->FindGroupEntryInMemberList(m_ShowEnemyPlayers[i])||pkNetPlayer->IsAllBuddyDead())
			{
				m_ShowEnemyPlayers[i]=0;
			}
		}

	}
}

bool LC_ClientGamePlayer::IsTeamMate(object_id_type iTargetId)
{
	LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
	LC_ClientNetPlayer* pkNetPlayer = (LC_ClientNetPlayer*)(pkNetPlayerManager->FindPlayer(iTargetId));
	//找不到玩家或者该玩家是队友，就移除该玩家
	if (!pkNetPlayer)
	{
		return false;
	}
	else
	{
		LC_PlayerGroupBase*	pGroup=GetPlayerGroup();
		return pGroup&&pGroup->FindGroupEntryInMemberList(pkNetPlayer->GetCommanderUID());
	}
}

StringType LC_ClientGamePlayer::GetShowEnemyPlayer()
{
	StringStreamType sStream("");
	LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
	for (int i=0;i<3;i++)
	{
		const unique_id_type uid = m_ShowEnemyPlayers[i];
		if(uid == 0)
			continue;
		
		LC_ClientNetPlayer* pNetPlayer = (LC_ClientNetPlayer*)(pkNetPlayerManager->FindPlayer(uid));
		if (pNetPlayer)
		{
			sStream<<pNetPlayer->GetCommander()->m_nBornCharType<<"#"
				   <<pNetPlayer->GetCommanderLevel()<<"#"
				   <<pNetPlayer->GetCommanderName()<<"#"
				   <<pNetPlayer->GetCommanderMainID();
		}
		sStream<<"|";
	}
	return sStream.str();
}

bool LC_ClientGamePlayer::AutoAttackTarget(object_id_type mainControllerID,object_id_type charType,int targetPkMode)
{
	int pkMode=SC_ClientScriptInterface::GetSingletonPtr()->GetSkillInterface()->GetFixPKMode(GetMapResID());
	if (pkMode==PKM_PEACE)
	{
		return false;
	}
	if (targetPkMode!=0)
	{
		pkMode=targetPkMode;
	}
	else if (pkMode == 0)
	{
		pkMode=PKM_ALL;
	}
	if(m_kPKRecorder.GetPKMode() != pkMode)
	{
		m_kPKRecorder.SetPKMode(pkMode);
		RequestModifyPKSetting();
	}
	CF_ActorFilterFunc* pkCSVFilterFunc = CF_ActorFilterFunc::GetSingletonPtr();
	CF_ActorFilterFunc::DataEntry* pkData = pkCSVFilterFunc->GetEntryPtr(2);
	ResultType res = LC_Helper::IsValidTarget((LC_ActorBase*)this, mainControllerID, pkData);
	if (res != RE_SKILL_CHECK_VALID_TARGET)
	{
		SendLogicInfoMessage(res);
		return false;
	}
	else
	{
		SetSpecialHookCharType(charType);
		SetHooking(true);
		SetHookingTargetID(mainControllerID, true);
	}
	return true;
}


void LC_ClientGamePlayer::ReqMapNpcCount(int storyID, int mapID)
{
	MG_GET_MapNpcInfo kReqMsg;
	kReqMsg.m_nGameStoryID = storyID;
	kReqMsg.m_nResMapID = mapID;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_GET_MAPNPCINFO, &kReqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::SetMapNpcInfo(MG_SyncMapNpcInfo& msg)
{
	m_mNpcCount[msg.m_nResMapID].clear();
	for (int i = 0; i < msg.m_nNpcInfo.size(); ++i)
	{
		int key = msg.m_nNpcInfo[i].key;
		int value = msg.m_nNpcInfo[i].value;
		m_mNpcCount[msg.m_nResMapID][key] = value;
	}
}

int LC_ClientGamePlayer::GetNpcCountByMapID(int mapID, int npcID)
{
	mem::map<int, mem::map<int, int>>::iterator itor = m_mNpcCount.find(mapID);
	if (itor != m_mNpcCount.end())
	{
		mem::map<int, int>::iterator iter =  itor->second.find(npcID);
		if (iter != itor->second.end())
		{
			return iter->second;
		}
	}
	return 0;
}

bool LC_ClientGamePlayer::IsTaskFinished(int iTaskId)
{
	LC_ClientTaskMap* pkTaskMap = GetClientTaskMap();
	if(pkTaskMap  == NULL)
		return false;

	return pkTaskMap->IsInFinishedTaskMap(iTaskId);
}

bool LC_ClientGamePlayer::IsTaskCanFinish(int iTaskId)
{
	LC_ClientTaskMap* pkTaskMap = GetClientTaskMap();
	if(pkTaskMap  == NULL)
		return false;
	
	LC_ProcessingTaskEntry* pkTaskEntry = pkTaskMap->GetProcessingTaskEntryByID(iTaskId);
	if (!pkTaskEntry)
	{
		return false;
	}
	int CurCount=pkTaskEntry->GetFinishState1();
	
	LC_TaskManager* pTaskMgr=LC_TaskManager::GetSingletonPtr();
	LC_Task* pTask = pTaskMgr->GetTask(iTaskId);
	LC_Task_Condition* pCondition=pTask->GetFinishConditionNoParams(0);
	
	int MaxCount = pCondition->GetMaxCount();

	return CurCount>=MaxCount;
}

void LC_ClientGamePlayer::RequestUseActivationCode(const StringType &ActiveCode, int iType)
{
	if (ActiveCode == "")
	{
		return;
	}
	MG_PB<msg_define::use_activation_code_req> kReqMsg;
	kReqMsg.m_value.set_code(ActiveCode.c_str());
	kReqMsg.m_value.set_type(iType);
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_USE_ACTIVATION_CODE, &kReqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::RequestTreasureDerive( int iId )
{
	MG_TreasureChestsOperationREQ kReqMsg;
	kReqMsg.m_iID = iId;
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_TREASURE_CHESTS_OP_REQ, &kReqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::RequestTreasureLog()
{
	MG_TreasureChestsLogsReq kReqMsg;
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_TREASURE_CHESTS_LOGS_REQ, &kReqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

//丹哥宝库
void LC_ClientGamePlayer::RequestSecretTreasure(int iCode, int iParam1,int iParam2)
{
	MG_PB<msg_define::activity_secret_treasure_op_req> reqMsg;
	reqMsg.m_value.set_code(iCode);

	if (iCode == Activity::ACT_SECRET_TREASURE_OP_OPEN)
	{
		reqMsg.m_value.add_params(iParam1);
		reqMsg.m_value.add_params(iParam2);
	}
	else if (iCode == Activity::ACT_SECRET_TREASURE_OP_SHOP)
	{
		reqMsg.m_value.add_params(iParam1);
	}

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_ACTIVITY_SECRET_TREASURE_OP_REQ, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void	LC_ClientGamePlayer::RequestSmelter(int iMeltType,const StringType &operate)
{
	MG_PB<msg_define::item_melt_operation_req> reqMsg;
	reqMsg.m_value.set_melt_type(iMeltType);

	TStringVector kStringVector = UT_BaseStringUtility::SplitString(operate, '#');
	for (int i = 0; i < kStringVector.size()/2; ++i)
	{
		int32_t slotid =  boost::lexical_cast<int32_t>(kStringVector[2*i]);
		int32_t itemCount =  boost::lexical_cast<int32_t>(kStringVector[2*i + 1]);
		
		::msg_define::item_melt_info* kItemInfo = reqMsg.m_value.add_melt_item_info();
		kItemInfo->set_pack_index(slotid);
		kItemInfo->set_count(itemCount);
	}
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_MELT_ITEM, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::RequeseStarRuneOperation(int iCatagory,int iIndex)
{
	MG_PB<msg_define::stars_rune_operation_req> reqMsg;
	reqMsg.m_value.set_category(iCatagory);
	reqMsg.m_value.set_index(iIndex);
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_STARS_RUNE_UPGRADE, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::BeginSkillCast(float fStartTime,float fTotalTime,const StringType& rkSkillCastName)
{
	//如果之前存在功能读条，则打断
	if (m_kSkillCastExtraData != 0)
	{
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_FUNCSKILL_END_CAST);
		kGameEvent.SetEventParam(EP_FUNCSKILLCAST_DATA,m_kSkillCastExtraData);
		kGameEvent.SetEventParam(EP_FUNCSKILLCAST_PROC,1);
		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);	
	}

	LC_ActorBase::BeginSkillCast(fStartTime, fTotalTime, rkSkillCastName);
}

void LC_ClientGamePlayer::InterruptSkillCast(float fInterruptTime)
{
	//如果是功能读条，则发出打断事件
	if (m_kSkillCastExtraData != 0)
	{
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_FUNCSKILL_END_CAST);
		kGameEvent.SetEventParam(EP_FUNCSKILLCAST_DATA,m_kSkillCastExtraData);
		kGameEvent.SetEventParam(EP_FUNCSKILLCAST_PROC,1);
		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);	
	}

	LC_ActorBase::InterruptSkillCast(fInterruptTime);
}

void LC_ClientGamePlayer::EndSkillCast()
{
	//如果是功能读条则发送结束事件
	if (m_kSkillCastExtraData != 0)
	{
 		LC_GameEvent kGameEvent;
 		kGameEvent.SetEventType(GLET_FUNCSKILL_END_CAST);
		kGameEvent.SetEventParam(EP_FUNCSKILLCAST_DATA,m_kSkillCastExtraData);
		kGameEvent.SetEventParam(EP_FUNCSKILLCAST_PROC,2);
 		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
 		pkUIManager->HandlerGameEvent(kGameEvent);	
	}
	LC_ActorBase::EndSkillCast();
}

int LC_ClientGamePlayer::IsItemGem( int itemId )
{
	return GetWhichPackAutoPut(itemId);
}

bool LC_ClientGamePlayer::RequestEnterCrossMap( int iStoryID, int iMapResID )
{
	if (GetDead())
	{
		SendLogicInfoMessage(RE_TRANSFER_CANT_DEAD);
		return false;
	}
	CF_WorldMapList::DataEntry* pkWorldMapData = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(iMapResID);
	if (!pkWorldMapData)
		return false;

	if(!CheckCanTransferMap(iMapResID))
		return false;

	if (IsInCrossRealMapTeam() && !IS_NORMAL_MAP(pkWorldMapData->_iMapLogicType))
	{
		SendLogicInfoMessage(RE_IN_CROSS_MAP_TEAM_CAN_ENTER_AREA);
		return false;
	}

	MG_PACKET_ENCODE_HANDLER hMsgHandler;

	MG_GAMESTORY_ENTER_CROSS_MAP_REQ   reqMsg;
	reqMsg.m_iStoryID =	iStoryID;
	reqMsg.m_iMapResID = iMapResID;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_GAMESTORY_ENTER_CROSS_MAP_REQ, &reqMsg);

	if(bCreateMsg)
	{
		SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}

	return true;
}

void LC_ClientGamePlayer::RequestTransferBattlePlaybackMap(int iMapResID)
{
	//战斗回放 开始观看
	//LC_ClientLogicManager::m_fBeginRecord = -1;
	LC_ClientArenaPlayBackManager::GetSingletonPtr()->DestoryAll();

	MG_PACKET_ENCODE_HANDLER hMsgHandler;

	MG_REQ_TransferBattlePlaybackMap   reqMsg;
	reqMsg.m_iMapResID =	iMapResID;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_TRANSFERBATTLEPLAYBACKMAP, &reqMsg);

	if(bCreateMsg)
	{
		SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}

void LC_ClientGamePlayer::RequestGuildTransport( int transportId )
{
	 MG_Req_Enter_GuildTransport   reqMsg;
	 reqMsg.m_TransportId = transportId;

	 MG_PACKET_ENCODE_HANDLER hMsgHandler;

	 bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GUILDTRANSPORT_ENTER, &reqMsg);

	 if(bCreateMsg)
	 {
		 SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	 }
}

void LC_ClientGamePlayer::ClickGuildTransportIcon(const StringType& name,int transportId)
{
	  SendMessageDialog(RE_GUILD_TRANSPORT, name, "", RE_GUILD_TRANSPORT, transportId, 0, 0);
}

void LC_ClientGamePlayer::AddToCanStartList( int iTaskID )
{
	((LC_ClientTaskMap *)m_pkTaskMap)->AddToCanStartList(iTaskID);
}

ResultType LC_ClientGamePlayer::CheckAddGood(int goodtype, int playerid )
{
	return GetPlayerGuildAssert()->CheckAddGood(goodtype,playerid);
}

StringType LC_ClientGamePlayer::GetCanGoodCountStr()
{
	return GetPlayerGuildAssert()->GetCanGoodCount();
}

int LC_ClientGamePlayer::GetGameStoryScore(int type)
{
	return m_kGameStoryAsset.GetScore(type);
}

void LC_ClientGamePlayer::RequestBuyVigor()
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	MG_Req_BuyVigor reqMsg;
	bool bCreateMsg = false;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_BUY_VIGOR, &reqMsg);
	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

}

int LC_ClientGamePlayer::GetRankByType( int rankType, int serverType )
{
	return GetRankAssert()->GetIndex(getRankIDByType(rankType, serverType));
}

void LC_ClientGamePlayer::_updateExtraNodeData()
{
	UT_Vec3Int pos = GetVisibleLocation();
	GD_ClientActorModel* pkModel = (GD_ClientActorModel*)IM_Manager::GetSingletonPtr()->GetActor(GetID());
	if(pkModel)
	{
		NiPoint3 sd;
		//脚底稳定节点
		sd = UT_EngineMath::ConvertVec3IntToNiPoint3(pos);
		pkModel->FixPos(sd, GD_ModelBase::FDT_STABLE_GROUND);

		//头顶稳定节点
		pos.z += GetProperty_ShapeHeight();
		sd = UT_EngineMath::ConvertVec3IntToNiPoint3(pos);
		pkModel->FixPos(sd, GD_ModelBase::FDT_STABLE_HEAD_TOP);


		//胸口稳定节点
		sd = pkModel->GetFxDummyNodePosition(GD_ModelBase::FDT_HEART);
		NiCameraPtr pkCamera = CA_CameraManager::GetSingletonPtr()->GetNiCamera();
		NiPoint3 kLookAt = pkCamera->GetWorldDirection();
		kLookAt.z = 0;		 kLookAt.Unitize();
		sd -= kLookAt * GetProperty_SfxOffset();
		pkModel->FixPos(sd, GD_ModelBase::FDT_STABLE_FDT_HEART);
	}
}

int LC_ClientGamePlayer::GetCommonStatReserveTimes( int32_t uiType )
{
	LC_CommonStatHistory* pkCmnSTHis = GetCommonStatAsset().GetCommonStatHistory(LC_ST_TP_DAILY);
	LC_CommonStatManager* pkMgr = LC_CommonStatManager::GetSingletonPtr();
	LC_CommonStatConfigDataEntry* pkConfig = pkMgr->GetCmnConfigDataEntry(uiType);

	if(pkCmnSTHis->m_iCharLevel < pkConfig->m_iMinLevel)
	{
		return -1;
	}
	
	LC_CommonStatEntry* pkCmnEntry = GetCommonStatAsset().GetCommonStatEntry(uiType);
	if (!pkCmnEntry)
	{
		return -1;
	}
	
	return pkCmnEntry->GetReplevyKey(this, GET_SERVER_SYSTEM_TIME);
}

bool LC_ClientGamePlayer::IsPurchaseBackOpen()
{
	LC_CommonStatHistory* pkCmnSTHis = GetCommonStatAsset().GetCommonStatHistory(LC_ST_TP_DAILY);
	LC_CommonStatManager* pkMgr = LC_CommonStatManager::GetSingletonPtr();	
	LC_CommonStatConfigDataEntry* pkConfig = NULL;

	CF_CommonStatReplevy::DataEntryMapExternIterator kIter = CF_CommonStatReplevy::GetSingleton().GetIterator();
	while(kIter.HasMoreElements())
	{
		CF_CommonStatReplevy::DataEntry* pkData = kIter.PeekNextValuePtr();
		pkConfig = pkMgr->GetCmnConfigDataEntry(pkData->_iType);

		if(pkCmnSTHis->m_iCharLevel >= pkConfig->m_iMinLevel)
		{
			return true;
		}

		kIter.MoveNext();
	}

	return false;
}

StringType LC_ClientGamePlayer::GetCommonStatCostMoney(int32_t uiType)
{
	LC_CommonStatConfigDataEntry* pkConfig = LC_CommonStatManager::GetSingletonPtr()->GetCmnConfigDataEntry(uiType);
	if(pkConfig == NULL)
		return "";

	mem::map<int32_t, LC_ShopIDMap>& shopIndices = pkConfig->GetShopIndices();
	mem::map<int32_t, LC_ShopIDMap>::iterator iter = shopIndices.begin();
	if(iter != shopIndices.end())
	{
		LC_ShopIDMap& kShopIds = iter->second;
		LC_ShopIDMap::iterator iter2 = kShopIds.begin();
		if(iter2 != kShopIds.end())
		{
			CF_ShopList::DataEntry *pkShopData = SafeGetCSVFileDataEntryPtr<CF_ShopList>(iter2->first);
			if(pkShopData != NULL)
			{
				LC_ComMap::const_iterator cit = pkShopData->_ActualPriceList.begin();
				if(cit != pkShopData->_ActualPriceList.end())
				{
					StringType sMoney;
					UT_BaseStringUtility::format(sMoney, "%d#%d", cit->first, cit->second * iter2->second);
					return sMoney;
				}
			}
		}
	}

	return "";
}

int LC_ClientGamePlayer::GetSkillBookCateGory2(int iPackType, int iSlot)
{
	LC_PackAsset *pkPackAsset = GetPackAsset();
	if(NULL == pkPackAsset)
		return 0;

	LC_BackPackEntry *pkBackPackEntry = pkPackAsset->GetBackPackEntry(iPackType, iSlot);
	if(NULL == pkBackPackEntry)
		return 0;

	CF_ItemList::DataEntry *pkItem = SafeGetCSVFileDataEntryPtr<CF_ItemList>(pkBackPackEntry->GetTypeID());
	if(NULL == pkItem)
		return 0;

	CF_ItemPropertyFunc::DataEntry *pkItemProp = SafeGetCSVFileDataEntryPtr<CF_ItemPropertyFunc>(pkItem->_lPropertyFuncID);
	if(NULL == pkItemProp)
		return 0;

	TStringVector kVect = UT_BaseStringUtility::SplitString(pkItemProp->_kSkills, ';');
	if(kVect.size() == 0)
		return 0;

	int iSkillID = atoi(kVect[0].c_str());
	CF_SkillExecuteList::DataEntry *pkSkill = LC_Helper::SafeGetSkillExecuteDataEntryPtr(iSkillID);
	if(NULL == pkSkill)
		return 0;

	return pkSkill->_lCategory2;
}

int LC_ClientGamePlayer::GetUseSkillBookSlotIndex(int iCategory2)
{
	LC_SkillAsset *pkSkillAsset = GetSkillAsset();
	if(NULL == pkSkillAsset)
		return -1;

	int iSlot = pkSkillAsset->GetFreeSkillSlot(iCategory2);
	return iSlot;
}

void LC_ClientGamePlayer::RequestActiveSkillBook(int iID)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;

	MG_REQ_ActivateSkillBook reqMsg;
	reqMsg.m_nSkillBookID = iID;

	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MG_REQ_ACTIVATE_SKILLBOOK, &reqMsg);
	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

void LC_ClientGamePlayer::RequestUnlockSkillBook(int iIndex, int iSlot)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;

	MG_REQ_Unlock_SkillBookSlot reqMsg;
	reqMsg.m_nIndex = iIndex;
	reqMsg.m_nSlotID = iSlot;

	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MG_REQ_UNLOCK_SKILLBOOKSLOT, &reqMsg);
	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

void LC_ClientGamePlayer::RequestWearSkillBook(int iIndex, int iSlot, int iSkillBookID)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;

	MG_REQ_WearSkillBook reqMsg;
	reqMsg.m_nOperatorType = iSkillBookID > 0 ? 1 : 0;
	reqMsg.m_nIndex = iIndex;
	reqMsg.m_nSlotID = iSlot;
	reqMsg.m_nSkillBookID = iSkillBookID;

	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MG_REQ_WEAR_SKILLBOOK, &reqMsg);
	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

void LC_ClientGamePlayer::SyncSkillBookSlotInfo(const MG_SyncSkillBookSlotInfo& msg)
{
	for(int i=0; i<MAX_ACTIVE_BUDDY_NUM; ++i)
	{
		m_kSkillBookSlotData[i].clear();
		const std::vector<PS_SkillBookSlotData>& kList = msg.m_nSkillBookSlotDataInfo[i];
		std::vector<PS_SkillBookSlotData>::const_iterator iter = kList.begin();
		for(; iter!=kList.end(); ++iter)
		{
			m_kSkillBookSlotData[i].push_back(*iter);
		}
	}
}

int LC_ClientGamePlayer::GetFirstEmptySkillBookSlot(int iIndex)
{
	if(iIndex < 0 || iIndex >= MAX_ACTIVE_BUDDY_NUM)
		return 0;

	int iSlot = 0;
	const mem::vector<PS_SkillBookSlotData>& kList = m_kSkillBookSlotData[iIndex];
	mem::vector<PS_SkillBookSlotData>::const_iterator iter = kList.begin();
	for(; iter!=kList.end(); ++iter)
	{
		const PS_SkillBookSlotData& kData = *iter;
		if(kData.m_nSlotState && kData.m_nSkillBookID == 0)
		{
			iSlot = kData.m_nSlotID;
			break;
		}
	}

	return iSlot;
}

int LC_ClientGamePlayer::GetSkillBookSlotSkillBookId(int iIndex, int iSlot)
{
	if(iIndex < 0 || iIndex >= MAX_ACTIVE_BUDDY_NUM)
		return 0;

	if(iSlot < 0 || iSlot >= m_kSkillBookSlotData[iIndex].size())
		return 0;

	const mem::vector<PS_SkillBookSlotData>& kData = m_kSkillBookSlotData[iIndex];
	mem::vector<PS_SkillBookSlotData>::const_iterator iter = kData.begin();
	for(; iter!=kData.end(); ++iter)
	{
		const PS_SkillBookSlotData& slotData = *iter;
		if(slotData.m_nSlotID == iSlot)
			return slotData.m_nSkillBookID;
	}

	return 0;
}

bool LC_ClientGamePlayer::GetSkillBookSlotActive(int iIndex, int iSlot)
{
	if(iIndex < 0 || iIndex >= MAX_ACTIVE_BUDDY_NUM)
		return false;

	if(iSlot < 0 || iSlot >= m_kSkillBookSlotData[iIndex].size())
		return false;

	const mem::vector<PS_SkillBookSlotData>& kData = m_kSkillBookSlotData[iIndex];
	mem::vector<PS_SkillBookSlotData>::const_iterator iter = kData.begin();
	for(; iter!=kData.end(); ++iter)
	{
		const PS_SkillBookSlotData& slotData = *iter;
		if(slotData.m_nSlotID == iSlot)
			return slotData.m_nSlotState;
	}

	return false;
}

void LC_ClientGamePlayer::HideGroundFx()
{
	//隐藏脚底光圈
	GD_ClientActorModel* gdacm = IM_Manager::GetSingleton().GetActor(GetID());
	if ( gdacm )
	{
		gdacm->DetachFxExt(GD_ClientActorModel::FDT_DYNAMIC_GROUND, RES_FX_CHOOSE_TARGET_YELLOW);
	}
}

void LC_ClientGamePlayer::ShowGroundFx()
{
	//显示脚底光圈
	GD_ClientActorModel* gdacm = IM_Manager::GetSingleton().GetActor(GetID());
	if ( gdacm )
	{
		int eBindMode = !DBG_USE_3D_COORD_AXIS_GFX;
		gdacm->AttachFxExt(GD_ClientActorModel::FDT_DYNAMIC_GROUND, eBindMode, false, RES_FX_CHOOSE_TARGET_YELLOW, NULL, NULL, -1.0f);
	}
}

void LC_ClientGamePlayer::ResetLogicAreaInfo()
{
	m_lCurrentMapResID = 0;
	m_lCurrentLogicAreaID = 0;
	m_lCurrentMapLogicID = 0;
	m_lCurrentLogicAreaName = "";
}

int LC_ClientGamePlayer::GetFirstChargeTime()
{
	return m_FirstChargeTime;
}

bool LC_ClientGamePlayer::IsFirstChargeRewardToken(int iDay)
{
	if (m_FirstChargeRewardRecord.find(iDay)!=m_FirstChargeRewardRecord.end())
	{
		return m_FirstChargeRewardRecord[iDay];
	}
	return false;
}

StringType LC_ClientGamePlayer::GetDrawCumulateReward()
{
	return m_mDrawCardCumlateReward;
}

void LC_ClientGamePlayer::UpdateDrawCardCumulateReward(const StringType& InRewardStr)
{
	m_mDrawCardCumlateReward=InRewardStr;
}

void LC_ClientGamePlayer::ReqVipQuickHook()
{
	MG_REQ_GetVIPExtraReward reqMsg;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GETVIPEXTRAREWARD, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::UpdateVipQuickHookTime(int iTimes)
{
	m_iVipQuickHookTime=iTimes;
}

void LC_ClientGamePlayer::UpdateVipQuickHookReward(std::vector<PS_ItemRecord>& RwdInfo)
{
	m_iVipQuickHookRwd=RwdInfo;
}

StringType LC_ClientGamePlayer::GetVipQuickHookReward()
{
	StringStreamType sStream("");
	for (int i=0;i<m_iVipQuickHookRwd.size();i++)
	{
		sStream<<m_iVipQuickHookRwd[i].m_iItemID<<":"<<m_iVipQuickHookRwd[i].m_iItemCount<<";";
	}
	return sStream.str();
}

int LC_ClientGamePlayer::GetVipQuickHookTime()
{
	return m_iVipQuickHookTime;
}

//===================================================================================================================

void LC_ClientGamePlayer::GetSubEquipSlotMoldingAttr(int subType, LC_ActorAttributeMap* attrMap)
{
	const EquipModingSpiritLvMap& subAttrValueMap = GetCommander()->GetSubEquipMoldingSpiritValueMap();
	EquipModingSpiritLvMap::const_iterator iter = subAttrValueMap.find(subType);
	if(iter == subAttrValueMap.end())
		return;

	const mem::map<int, int>& subMap = iter->second;
	mem::map<int, int>::const_iterator itr = subMap.begin();
	for( ; itr!=subMap.end(); ++itr)
	{
		attrMap->SetAttributeValueFunc(itr->first, itr->second);
	}
}

void LC_ClientGamePlayer::GetSubEquipSlotMoldingSuitPercentAttr(int subType, LC_ActorAttributeMap* attrMap)
{
	const EquipModingSpiritLvMap& subAttrPercentMap = GetCommander()->GetSubEquipSlotMoldingSpiritPercentMap();
	EquipModingSpiritLvMap::const_iterator findIt = subAttrPercentMap.find(subType);
	if(findIt != subAttrPercentMap.end())
	{
		for(mem::map<int, int>::const_iterator it=findIt->second.begin(); it!=findIt->second.end(); ++it)
		{
			attrMap->SetAttributePercentFunc(it->first, it->second);
		}
	}
}

void LC_ClientGamePlayer::GetSubEquipSlotMoldingSuitValueAttr(int subType, LC_ActorAttributeMap* attrMap)
{
	mem::map<int, int> suitInfo;
	GetCommander()->GetSubMoldingSpiritSuitInfo(suitInfo, subType);

	mem::map<int, int> suitAttrPercentInfo;
	LC_Helper::GetSubSuitAttrInfoFromItemEffectFunc(&suitInfo, attrMap, suitAttrPercentInfo, subType);
}

int LC_ClientGamePlayer::GetSubEquipMoldingAttribValue(int iSubType, int iAttribType)
{
	const EquipModingSpiritLvMap& subAttrValueMap = GetCommander()->GetSubEquipMoldingSpiritValueMap();
	EquipModingSpiritLvMap::const_iterator iter = subAttrValueMap.find(iSubType);
	if(iter == subAttrValueMap.end())
		return 0;

	const mem::map<int, int>& attrMap = iter->second;
	mem::map<int, int>::const_iterator itr = attrMap.find(iAttribType);
	if(itr == attrMap.end())
		return 0;

	return itr->second;
}

int LC_ClientGamePlayer::GetSubEquipSuitMoldingAttribValue(int iSubType, int iAttribType)
{
	const EquipModingSpiritLvMap& subAttrValueMap = GetCommander()->GetSubEquipSuitMoldingSpiritValueMap();
	EquipModingSpiritLvMap::const_iterator iter = subAttrValueMap.find(iSubType);
	if(iter == subAttrValueMap.end())
		return 0;

	const mem::map<int, int>& attrMap = iter->second;
	mem::map<int, int>::const_iterator itr = attrMap.find(iAttribType);
	if(itr == attrMap.end())
		return 0;

	return itr->second;
}

int LC_ClientGamePlayer::GetSubEquipSuitMoldingAttribPercent(int iSubType, int iAttribType)
{
	const EquipModingSpiritLvMap& subAttrValueMap = GetCommander()->GetSubEquipSlotMoldingSpiritPercentMap();
	EquipModingSpiritLvMap::const_iterator iter = subAttrValueMap.find(iSubType);
	if(iter == subAttrValueMap.end())
		return 0;

	const mem::map<int, int>& attrMap = iter->second;
	mem::map<int, int>::const_iterator itr = attrMap.find(iAttribType);
	if(itr == attrMap.end())
		return 0;

	return itr->second;
}

//===================================================================================================================

ResultType LC_ClientGamePlayer::ReqItemInPackageOperation(int iPackType, int iEntryIndex, int OperationCode, const StringType &sParams)
{
	MG_REQ_ItemInPackageCommonOperation kMsg;
	kMsg.m_iPackType = iPackType;
	kMsg.m_iEntryIndex	= iEntryIndex;
	kMsg.m_iOPCode = OperationCode;

	TStringVector kStringVector;
	int pos = sParams.find_first_of(';');
	StringType strParam = "";
	if(pos != StringType::npos)
	{
		strParam = sParams.substr(pos + 1);
		kStringVector = UT_BaseStringUtility::SplitString(sParams.substr(0, pos), '#');
	}
	else
	{
		kStringVector = UT_BaseStringUtility::SplitString(sParams, '#');
	}

	kMsg.m_sOPPara = TPSTR2STDSTR(strParam);
	int	ParamsSize = kStringVector.size();
	for (int i = 0; i < MAX_ITEM_OPERATION_PARAM_COUNT; ++i)
	{
		if (i < ParamsSize)
		{
			kMsg.m_kOPPara[i] = atoi(kStringVector[i].c_str());
		}
		else
		{
			kMsg.m_kOPPara[i] = 0;
		}
	}
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_ITEM_IN_PACKAGE_OPERATION, &kMsg);
	if (bCreateMsg)
	{
		SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
		return RE_SUCCESS;
	}
	return RE_FAIL;
}

void LC_ClientGamePlayer::StartNormalContinueCheck()
{
	m_normalContinueCheck = true;
}

void LC_ClientGamePlayer::EndNormalContinueCheck()
{
	m_normalContinueCheck = false;
}

int32_t LC_ClientGamePlayer::GetSubordinateAttrPlusValue(int32_t iSubType, int32_t iAttributeType)
{
	LC_SubUpgradeData* pkSub = GetSubordinateAsset().GetSubData(iSubType);
	if(pkSub)
	{
		return pkSub->GetAttributePlusValue(iAttributeType);
	}

	return 0;
}

int32_t LC_ClientGamePlayer::GetSubordinateAttrPlusPercent(int32_t iSubType, int32_t iAttributeType)
{
	LC_SubUpgradeData* pkSub = GetSubordinateAsset().GetSubData(iSubType);
	if(pkSub)
	{
		return pkSub->GetAttributePlusPercent(iAttributeType);
	}
	return 0;
}

int LC_ClientGamePlayer::GetTalismanUpgradeNeeedLiveness(int iLevel)
{
	const LC_SubordinateDataEntry* entry = SafeGetSubDataEntry(LC_SUB_TP_TALISMAN, iLevel);
	if (NULL == entry)
	{
		return 0;
	}
	return entry->_iLiveness;
}

StringType LC_ClientGamePlayer::GetCimeliaDrugInfo(int iType)
{
	StringStreamType sStream("");
	LC_SubUpgradeData* pkSub = GetSubordinateAsset().GetSubData(iType);
	if(pkSub)
	{		
		const LC_SubordinateDataEntry *pkEntry = SafeGetSubDataEntry(iType, pkSub->GetStage());
		if(pkEntry)
		{
			sStream << pkSub->m_uiAttrValCount << "#" << pkEntry->_iMaxAttrPlusVal << "#" << pkSub->m_uiAttrPerCount << "#" << pkEntry->_iMaxAttrPlusPercent << "#";
			const LC_SubordinateDataEntry* pkNextEntry = SafeGetSubDataEntry(iType, pkSub->GetStage() + 1);
			if(pkNextEntry)
			{
				sStream << pkNextEntry->_iMaxAttrPlusVal << "#" << pkNextEntry->_iMaxAttrPlusPercent << "#";
			}
			else
			{
				sStream << "0#0#";
			}
		}
	}

	return sStream.str();
}

StringType LC_ClientGamePlayer::GetCimeliaAttribPlusInfo(int iType, int iItemID, bool bPercent)
{
	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	if(!pkItemFactory)
		return "";

	LC_ClientItem *pkItem = (LC_ClientItem*)pkItemFactory->RequestItem(iItemID);
	if(!pkItem)
		return "";

	TStringVector kVector = UT_BaseStringUtility::SplitString(pkItem->GetScriptArgs(), ';');
	if(kVector.size() == 0)
		return "";

	StringStreamType sStream("");
	LC_SubUpgradeData* pkSub = GetSubordinateAsset().GetSubData(iType);
	if(pkSub)
	{
		LC_ActorAttributeAppendMap& attrMap = pkSub->AttributePlus();
		sStream << kVector.size() << "@";
		for(int i = 0; i < kVector.size(); ++i)
		{
			TStringVector kDetail = UT_BaseStringUtility::SplitString(kVector[i], ':');
			if(kDetail.size() == 4)
			{
				int iAttrib = String2Int(kDetail[1]);
				int64_t iValue = 0;
				if(bPercent)
					iValue = attrMap.GetAttributePercent(iAttrib);
				else
					iValue = attrMap.GetAttributeValue(iAttrib);

				sStream << iAttrib << "#" << iValue << "|";
			}			
		}
	}

	return sStream.str();
}


StringType LC_ClientGamePlayer::GetCimeliaTransformAttribInfo(int iType)
{
	LC_ActorAttributeMap attrMap;
	LC_Helper::GetCimeliaTransformAttribInfo(this, iType, attrMap);
	
	int iAtk = attrMap.GetAttribute(ATT_MAX_ATK);
	int iAtkRate = attrMap.GetAttribute(ATT_ATK_RATE);
	int iCir = attrMap.GetAttribute(ATT_CIR_RATE);
	int iDef = attrMap.GetAttribute(ATT_DEF);
	int iDogge = attrMap.GetAttribute(ATT_DOG_RATE);
	int iCirDef = attrMap.GetAttribute(ATT_CIR_DEF);
	int iHp = attrMap.GetAttribute(ATT_MAX_HP);
	int iDmgRate = attrMap.GetAttribute(ATT_DMG_RATE);
	int iDmgDerate = attrMap.GetAttribute(ATT_DMG_DERATE);
	int iCirDam = attrMap.GetAttribute(ATT_CIR_DAM);

	StringStreamType sStream("");
	sStream << iAtk << "#" << iAtkRate << "#" << iCir << "#" << iDef << "#" << iDogge << "#" << iCirDef << "#" << iHp << "#" << iDmgRate << "#" << iDmgDerate << "#" << iCirDam;
	return sStream.str();
}

StringType LC_ClientGamePlayer::GetCimeliaTransformInfo(int iType)
{
	CF_SubordinateTransform *pkSubTransform = CF_SubordinateTransform::GetSingletonPtr();
	
	int iPlatform = (int)(getYunYingplatform());
	int iTxSubPlatform = (int)(getTxSubplatform());
	StringStreamType sStream("");
	std::vector<CF_SubordinateTransform::DataEntry*> kVectorNormal;
	std::vector<CF_SubordinateTransform::DataEntry*> kVectorSpecial;
	CF_SubordinateTransform::DataEntryMapExternIterator kIter = pkSubTransform->GetIterator();
	for( ; kIter.HasMoreElements(); kIter.MoveNext() )
	{
		CF_SubordinateTransform::DataEntry *pkEntry = kIter.PeekNextValuePtr();
		if(pkEntry->_iType == iType)
		{
			TStringVector kVectorPlatform = UT_BaseStringUtility::SplitString(pkEntry->_iPlatForm, ';');

			bool bAdd = false;
			if(kVectorPlatform.empty())
			{
				bAdd = true;
			}
			else
			{
				for(int i=0; i<kVectorPlatform.size(); ++i)
				{
					if(atoi(kVectorPlatform[i].c_str()) == iPlatform)
					{
						bAdd = true;
						break;
					}
				}
			}

			TStringVector kVectorExcept = UT_BaseStringUtility::SplitString(pkEntry->_iExceptPlatForm, ';');
			for(int i=0; i<kVectorExcept.size(); ++i)
			{
				if(atoi(kVectorExcept[i].c_str()) == iPlatform)
				{
					bAdd = false;
					break;
				}
			}

			if(bAdd && iPlatform == YYP_TX)
			{
				TStringVector kVectorSub = UT_BaseStringUtility::SplitString(pkEntry->_iTxSubPlatForm, ';');
				if(!kVectorSub.empty())
					bAdd = false;
				
				for(int i=0; i<kVectorSub.size(); ++i)
				{
					if(atoi(kVectorSub[i].c_str()) == iTxSubPlatform)
					{
						bAdd = true;
						break;
					}
				}
			}			

			if(bAdd)
			{
				if(pkEntry->_iSubType == 0)
					kVectorNormal.push_back(pkEntry);
				else
					kVectorSpecial.push_back(pkEntry);
			}
		}
	}

	sStream << kVectorNormal.size() << "|";
	for(std::vector<CF_SubordinateTransform::DataEntry*>::iterator it = kVectorNormal.begin(); it!=kVectorNormal.end(); ++it)
	{
		CF_SubordinateTransform::DataEntry *pkEtnry = *it;
		sStream <<pkEtnry->_iID << "#" << pkEtnry->_iCharType << "#" << pkEtnry->_sDesc << "#" << pkEtnry->_iAttrFunc << "#";
		sStream << pkEtnry->_iLevel << "#" << pkEtnry->_kNamePic << "#" << pkEtnry->_kPath << "#" << pkEtnry->_kAvatarPic << "#";
		sStream << pkEtnry->_iGrade << "#" << pkEtnry->_sFunctionPic << "#" << (int)pkEtnry->_ielementdamagetype << "#";

		UT_SIMDataList::const_iterator cit = pkEtnry->_kItems.begin();
		for (; cit != pkEtnry->_kItems.end(); ++cit)
		{
			sStream << cit->ID() << "_" << cit->ParamA() << "@";
		}
		sStream << "|";
	}

	sStream << "&";
	sStream << kVectorSpecial.size() << "|";
	for(std::vector<CF_SubordinateTransform::DataEntry*>::iterator it = kVectorSpecial.begin(); it!=kVectorSpecial.end(); ++it)
	{
		CF_SubordinateTransform::DataEntry *pkEtnry = *it;
		sStream <<pkEtnry->_iID << "#" << pkEtnry->_iCharType << "#" << pkEtnry->_sDesc << "#" << pkEtnry->_iAttrFunc << "#";
		sStream << pkEtnry->_iLevel << "#" << pkEtnry->_kNamePic << "#" << pkEtnry->_kPath << "#" << pkEtnry->_kAvatarPic << "#";
		sStream << pkEtnry->_iGrade << "#" << pkEtnry->_sFunctionPic << "#" << (int)pkEtnry->_ielementdamagetype << "#";

		UT_SIMDataList::const_iterator cit = pkEtnry->_kItems.begin();
		for (; cit != pkEtnry->_kItems.end(); ++cit)
		{
			sStream << cit->ID() << "$" << cit->ParamA() << "@";
		}
		sStream << "|";
	}

	return sStream.str();
}

StringType LC_ClientGamePlayer::GetCimeliaNamePic(int iType, int iLevel)
{
	LC_SubUpgradeData* pkSub = GetSubordinateAsset().GetSubData(iType);
	if(!pkSub)
		return "";

	int iTransformID = pkSub->GetSetTransform();
	if(iTransformID > 0)
	{
		CF_SubordinateTransform::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_SubordinateTransform>(iTransformID);
		if(pkEntry)
		{
			return pkEntry->_kNamePic;
		}
	}

	return "";
}

bool LC_ClientGamePlayer::IsTransformActive(int iType, int iTransformID)
{
	LC_SubUpgradeData* pkSub = GetSubordinateAsset().GetSubData(iType);
	if(!pkSub)
		return false;

	LC_SubUpgradeData::TransformMap::iterator it = pkSub->m_kTansfroms.find(iTransformID);
	return it != pkSub->m_kTansfroms.end();
}

void LC_ClientGamePlayer::SyncSubordinateNormalTransform(int iSubType)
{
	LC_SubUpgradeData* pkSub = GetSubordinateAsset().GetSubData(iSubType);
	if(pkSub == NULL)
		return;

	int iCurStage = pkSub->GetStage();
	CF_SubordinateTransform::DataEntryMapExternIterator kIter = CF_SubordinateTransform::GetSingletonPtr()->GetIterator();
	while(kIter.HasMoreElements())
	{
		const CF_SubordinateTransform::DataEntry* pkEntry = kIter.PeekNextValuePtr();
		if(pkEntry->_iType == iSubType && pkEntry->_iSubType == 0 && pkEntry->_iLevel == iCurStage)
		{
			LC_SubUpgradeData::TransformMap::iterator it = pkSub->m_kTansfroms.find(pkEntry->_iID);
			if (it == pkSub->m_kTansfroms.end())
				RequestTransformCimelia(iSubType, LC_SUB_OP_ADD_AND_SET_TRANSFORM, pkEntry->_iID);
			break;
		}

		kIter.MoveNext();
	}
}

void LC_ClientGamePlayer::SetFirstSubTransform(int iSubType)
{
	GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM, "进阶线开启======设置初始皮肤 SetFirstSubTransform");
	CF_SubordinateTransform::DataEntryMapExternIterator kIter = CF_SubordinateTransform::GetSingletonPtr()->GetIterator();
	while(kIter.HasMoreElements())
	{
		//进阶线第一级的皮肤申请AddAndSet
		const CF_SubordinateTransform::DataEntry* pkEntry = kIter.PeekNextValuePtr();
		if(pkEntry->_iType == iSubType && pkEntry->_iSubType == 0 && pkEntry->_iLevel == 1)
		{
			RequestTransformCimelia(iSubType, LC_SUB_OP_ADD_AND_SET_TRANSFORM, pkEntry->_iID);
			break;
		}

		kIter.MoveNext();
	}
}


void LC_ClientGamePlayer::UpdateCimeliaGainNew(int iType,int iTransformId,bool bValue)
{
	LC_SubUpgradeData* pkSub = GetSubordinateAsset().GetSubData(iType);
	pkSub->SetTransformGainNew(iTransformId,bValue);
}

bool LC_ClientGamePlayer::GetCimeliaGainNew(int iType,int iTransformId)
{
	LC_SubUpgradeData* pkSub = GetSubordinateAsset().GetSubData(iType);

	const mem::map<int32_t, bool>& transformmap= pkSub->GetTransformMap();
	mem::map<int32_t, bool>::const_iterator iter=transformmap.find(iTransformId);
	if (iter!=transformmap.end())
	{
		return iter->second;
	}
	return false;
}

void LC_ClientGamePlayer::SetCimeliaGainNew(int iType,int iTransformId)
{
	MG_REQ_SetSubTransformFlag msg;
	msg.m_nSubType=iType;
	msg.m_nTransformID=iTransformId;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MG_REQ_SETSUBTRANSFORMFLAG, &msg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}

int LC_ClientGamePlayer::GetSuitableDrugItemID()
{
	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	if(NULL == pkItemFactory)
		return 0;

	LC_ShortcutBar*	pkShortcutBar = GetShortcutBar();
	if(NULL == pkShortcutBar)
		return 0;

	LC_ShortcutEntry *pkItemShortcut = pkShortcutBar->GetShortcutEntry(1, SHORTCUT_INDEX_ITEM);
	if(NULL == pkItemShortcut || !pkItemShortcut->GetValid())
		return 0;

	if(GetCanCostItemCount(pkItemShortcut->GetTypeID()) > 0)
		return 0;

	std::vector<LC_ItemDescriptionEntry*> kVector;
	int iPackCapacity = m_kPackAsset.GetPackSize(PACK_TYPE_BASIC);

	for (int i = 0; i < iPackCapacity; ++i)
	{
		LC_BackPackEntry* pkEntry = m_kPackAsset.GetBackPackEntry(PACK_TYPE_BASIC, i);
		if (pkEntry && pkEntry->GetValid() && !pkEntry->GetLocked())
		{
			int lItemTypeID = pkEntry->GetTypeID();
			LC_ItemDescriptionEntry* pkItemEntry = pkItemFactory->GetItemDescriptionEntry(lItemTypeID);
			if (IS_MEDICINE_ITEM_ID(lItemTypeID) && pkItemEntry != NULL && pkItemFactory->CheckItemUsable(this, lItemTypeID))
			{
				kVector.push_back(pkItemEntry);
			}
		}
	}
	
	if(kVector.size() > 0)
	{
		std::sort(kVector.begin(), kVector.end(),  _sortFunc);
		return kVector[0]->GetTypeID();
	}

	return 0;
}


void LC_ClientGamePlayer::RequestEquipRune(int iSrcBuddyId, int iSrcSlot, int iTarBuddyId, int iTargetSlot)
{
	MG_REQ_EquipRune reqMsg;
	reqMsg.m_iSrcBuddyID = iSrcBuddyId;
	reqMsg.m_iSrcIndex = iSrcSlot;
	reqMsg.m_iDestBuddyID = iTarBuddyId;
	reqMsg.m_iDestIndex = iTargetSlot;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_EQUIP_RUNE, &reqMsg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}

void LC_ClientGamePlayer::RequestOnekeyEquipRune(int iBuddyId)
{
	MG_REQ_OneKeyEquipRune reqMsg;
	reqMsg.m_iBuddyID = iBuddyId;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_ONEKEY_EQUIP_RUNE, &reqMsg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}

void LC_ClientGamePlayer::RequestUnEquipRune(int iBuddyId, int iSlot)
{
	MG_REQ_UnEquipRune reqMsg;
	reqMsg.m_iBuddyID = iBuddyId;
	reqMsg.m_iSrcSlotIndex = iSlot;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_UNEQUIP_RUNE, &reqMsg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}

void LC_ClientGamePlayer::RequestOnekeyUnEquipRune(int iBuddyId)
{
	MG_REQ_OneKeyUnEquipRune reqMsg;
	reqMsg.m_iBuddyID= iBuddyId;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_ONEKEY_UNEQUIP_RUNE, &reqMsg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}

void LC_ClientGamePlayer::RequestLevelUpRune(int iBuddyId, int iSlot, int iTargetLevel)
{
	MG_REQ_RuneUpgrade reqMsg;
	reqMsg.m_iBuddyID = iBuddyId;
	reqMsg.m_iSlotIndex = iSlot;
	reqMsg.m_iTargetLevel = iTargetLevel;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_RUNE_UPGRADE, &reqMsg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}

void LC_ClientGamePlayer::RequestStarUpRune(int iBuddyId, int iSlot)
{
	MG_REQ_RuneStar reqMsg;
	reqMsg.m_iBuddyID = iBuddyId;
	reqMsg.m_iSlotIndex = iSlot;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_RUNE_STAR, &reqMsg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}

void LC_ClientGamePlayer::RequestResetUpRune(int iBuddyId, int iSlot)
{
	MG_REQ_ResetRune reqMsg;
	reqMsg.m_iBuddyID = iBuddyId;
	reqMsg.m_iSlotIndex = iSlot;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_RESET_RUNE, &reqMsg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}

void LC_ClientGamePlayer::RequestRuneReward(int iRewardID)
{
	MG_REQ_RuneReward reqMsg;
	reqMsg.m_nRewardID = iRewardID;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_RUNE_REWARD, &reqMsg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}

LC_BackPackEntry* LC_ClientGamePlayer::GetRuneEntry(int iBuddyId, int iIndex)
{
	LC_ClientBuddyInstance* pkInst = GetBuddyInstance(iBuddyId);
	if(pkInst == NULL)
		return NULL;

	return pkInst->GetRuneEquipEntry(iIndex);
}

void LC_ClientGamePlayer::OnRuneUpdated()
{
	LC_PackAsset* pkPackAsset = GetPackAsset();
	pkPackAsset->SetDirtyFlag(PACK_TYPE_RUNE_EQUIP_SLOT, true);
	pkPackAsset->AddDirtyFlagRevisionID(PACK_TYPE_RUNE_EQUIP_SLOT);
}

void LC_ClientGamePlayer::UpdateRankInfoData(PS_User_Rank_Info &userRankInfo)
{
	std::map<int32_t,PS_Index_Entry>& rankData = userRankInfo.m_AllRank.m_RankData;
	std::map<int32_t,PS_Index_Entry>::iterator it = rankData.begin();
	for(; it!=rankData.end(); ++it)
	{
		m_RankData[getRankTypeByID(it->first)] = it->second;
	}
}

StringType LC_ClientGamePlayer::GetRankInfoDataString(int32_t lRankType)
{
	StringStreamType sStream("");
	std::map<int32_t,PS_Index_Entry>::iterator it = m_RankData.find(lRankType);
	if(it != m_RankData.end())
	{
		PS_Index_Entry &dataEntry = it->second;
		sStream << dataEntry.m_Index << "#";
		sStream << dataEntry.m_SettleIndex << "#";
		sStream << dataEntry.m_SettleGain << "#";
		sStream << dataEntry.m_SettleTimestamp << "#";
		sStream << dataEntry.m_GainTimestamp << "#";
		return sStream.str();
	}
	return "";
}

void LC_ClientGamePlayer::GetDailyCleanDataFormServer(uint16_t RespType, const map<uint32_t, int32_t> &data)
{
	if (RespType == 0)
	{
		m_dailyCleanMap.clear();
		m_dailyCleanMap = data;
	}
	else if (RespType == 1)
	{
		for (map<uint32_t, int32_t>::const_iterator it = data.begin(); it != data.end(); ++it)
		{
			m_dailyCleanMap[it->first] = it->second;
		}
	}
}

platform_info* LC_ClientGamePlayer::GetPlatformInfo(const StringType& sPlatformType)
{
	PlatformMap::iterator it = m_platforminfo.find(sPlatformType);
	if (it != m_platforminfo.end())
		return &it->second;
	return NULL;
}

void LC_ClientGamePlayer::AddPlatformInfo(const StringType& sPlatType, uint8_t platInfo)
{
	platform_info kInfo;
	kInfo.decode_vip(sPlatType, platInfo); 
	m_platforminfo[sPlatType] = kInfo;
}

int LC_ClientGamePlayer::GetPlatformVipLevel(const StringType& sPlatformType)
{
	platform_info* pkDetail = GetPlatformInfo(sPlatformType);
	if (pkDetail)
		return pkDetail->vip_level;
	return 0;
}
bool LC_ClientGamePlayer::IsPlatformYearVip(const StringType& sPlatformType)
{
	platform_info* pkDetail = GetPlatformInfo(sPlatformType);
	if (pkDetail)
		return pkDetail->is_year_vip != 0;
	return false;
}
bool LC_ClientGamePlayer::IsPlatformVip(const StringType& sPlatformType)
{
	platform_info* pkDetail = GetPlatformInfo(sPlatformType);
	if (pkDetail)
		return pkDetail->is_vip != 0;
	return false;
}
bool LC_ClientGamePlayer::IsPlatformHighVip(const StringType& sPlatformType)
{
	platform_info* pkDetail = GetPlatformInfo(sPlatformType);
	if (pkDetail)
		return pkDetail->is_high_vip != 0;
	return false;
}

int LC_ClientGamePlayer::GetPlatformFormatVip(const StringType& sPlatformType)
{
	platform_info* pkDetail = GetPlatformInfo(sPlatformType);
	if (pkDetail)
		return pkDetail->format_vip();
	return 0;
}

int32_t LC_ClientGamePlayer::GetDailyCleanValue(uint32_t nType, uint32_t nID)
{
	uint32_t nKey = _GetDailyCleanKeyID(nType, nID);
	map<uint32_t, int32_t>::iterator iter = m_dailyCleanMap.find(nKey);
	if(iter == m_dailyCleanMap.end())
	{
		return 0;
	}
	return iter->second;
}

void LC_ClientGamePlayer::PostPlayerEnterGame()
{
	if (GetPostEnterGameFlag())
	{
		//SendRequestInfoByType(MGPT_FALLIANCE_BOSS_INFO_REQ,0,0,0,0);
		//SendRequestInfoByType(MGPT_FALLIANCE_BOSS_INFO_REQ,GetServerID(),0,0,0);
		SetPostEnterGameFlag(false);
	}
}

LC_ClientOpenAbilityManager* LC_ClientGamePlayer::GetOpenAbilityManager()
{
	return CLIENT_GET_OPENABIBITY_MANAGER();
}

ResultType LC_ClientGamePlayer::CanCreateGuild( const StringType& strGuildName )
{
	GuildCreateConfig* pCreateCfgMgr = GuildCreateConfig::Instance();
	if(pCreateCfgMgr == NULL)
	{
		return RE_FAIL;
	}

	std::wstring wsName = CodeConvert::Utf8ToWcs(strGuildName);
	return LC_PlayerBase::CanCreateGuild(wsName, pCreateCfgMgr->getLimit());
}

ResultType LC_ClientGamePlayer::CanDismissGuild(LC_GuildAsset* pkGuildAsset)
{
	if (GetGuildID() == INVALID_GUILD_ID)
	{
		return RE_GUILD_DISMISS_FAILED_NOT_EXIST_GUILD;
	}
	LC_GuildMemberInfo& rkMemberInfo = pkGuildAsset->GetGuildMemberInfo();
	LC_GuildMemberEntry* pkMemberEntry = rkMemberInfo.FindMemeberEntry(GetCommanderUID());
	if (NULL == pkMemberEntry)
	{
		return RE_GUILD_DISMISS_FAILED_NOT_EXIST_GUILD;
	}
	// 是否掌门
	if (pkMemberEntry->GetOfficialPosition() != GUILD_MEMBER_MASTER)
	{
		return RE_GUILD_DISMISS_FAILED_POWER;
	}
	// 成员是否为0
	//if(rkMemberInfo.GetValidMemeberCount() > 1)
	//	return RE_GUILD_DISMISS_FAILED_MEMBER_EXIST;
	return RE_SUCCESS;
}

void LC_ClientGamePlayer::ReqUseActivationCount(int iType)
{
	MG_PB<msg_define::activation_code_use_count_req> reqMsg;
	reqMsg.m_value.set_type(iType);
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_USE_ACTIVATION_COUNT, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::SetUseActivationCount(int iType, int iCount)
{
	m_ActiveCodeUseCount[iType] = iCount;
}

void LC_ClientGamePlayer::RequestActiveStarVein(int iID)
{
	MG_Req_ActivateStarVein reqMsg;
	reqMsg.m_nStarVeinID = iID;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_ACTIVATE_STARVEIN, &reqMsg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

void LC_ClientGamePlayer::RequestGetStarVeinGoalReward(int iID)
{
	MG_REQ_GetStarVeinReward reqMsg;
	reqMsg.m_nID = iID;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GET_STARVEIN_REWARD, &reqMsg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

int LC_ClientGamePlayer::GetUseActivationCount(int iType)
{
	mem::map<int, int>::iterator it = m_ActiveCodeUseCount.find(iType);
	if (it != m_ActiveCodeUseCount.end())
	{
		return it->second;
	}
	return -1;
}

void LC_ClientGamePlayer::SetHolydeedUserCount(int iType, int iCount)
{
	m_HolydeedUserCount[iType] = iCount;
}

int LC_ClientGamePlayer::GetHolydeedUserCount(int iType)
{
	mem::map<int, int>::iterator it = m_HolydeedUserCount.find(iType);
	if (it != m_HolydeedUserCount.end())
	{
		return it->second;
	}
	return -1;
}

void LC_ClientGamePlayer::SyncBuddyChain(const MG_SC_BuddyChainSync& msg)
{
	for(size_t i=0; i < msg.m_nCount; ++i)
	{
		m_vBuddyChainVec[i] = msg.m_Data[i];
	}
}

void LC_ClientGamePlayer::UpdateBuddyChain(int iIndex, int iCid)
{
	int iOldCid = m_vBuddyChainVec[iIndex];
	m_vBuddyChainVec[iIndex] = iCid;

	LC_GameEvent GameEvent;
	GameEvent.SetEventType(GLET_SYNC_BUDDYCHAIN_INFO);
	GameEvent.SetEventParam(EVENT_BASE_PARAM_1, iIndex);
	GameEvent.SetEventParam(EVENT_BASE_PARAM_2, iOldCid);
	GameEvent.SetEventParam(EVENT_BASE_PARAM_3, iCid);
	UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(GameEvent);
}

bool LC_ClientGamePlayer::IsBuddyInChain(int iCid)
{
	mem::vector<int>::const_iterator iter = m_vBuddyChainVec.begin();
	for(; iter!=m_vBuddyChainVec.end(); ++iter)
	{
		if(iCid == *iter)
		{
			return true;
		}
	}

	return false;
}

void LC_ClientGamePlayer::ReqSetChainFlag(int iIndex)
{
	MG_REQ_SetChainFlag reqMsg;
	reqMsg.m_nIndex = iIndex;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SETCHAINFLAG, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::ReqAddBuddyChain(int iIndex, int iCid)
{
	MG_CS_AddBuddyChainReq reqMsg;
	reqMsg.m_nIndex = iIndex;
	reqMsg.m_nBuddyID = iCid;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_CS_ADD_BUDDY_CHAIN_REQ, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::ReqDelBuddyChain(int iIndex)
{
	MG_CS_DelBuddyChainReq reqMsg;
	reqMsg.m_nIndex = iIndex;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_CS_DEL_BUDDY_CHAIN_REQ, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::RequestBuyInvesmentCard(int iType)
{
	MG_REQ_BuyInvesmentCard reqMsg;
	reqMsg.m_nInvesmentType = iType;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_BUYINVESMENTCARD, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::RequestAddInvesmentCardReward(int iID)
{
	MG_REQ_AddInvestmentCardReward reqMsg;
	reqMsg.m_nID = iID;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_ADDINVESTMENTCARDREWARD, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::RequestRefreshRewardTask()
{
	MG_REQ_RefershRewardTask reqMsg;
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_REFRESHREWARDTASK, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::ReSetInvestmentType()
{
	v_InvestmentType.clear();
}

void LC_ClientGamePlayer::SetInvestmentType(int iType)
{
	v_InvestmentType.push_back(iType);
}

bool LC_ClientGamePlayer::GetInvestmentTypeHasBuy(int iType)
{
	int len = v_InvestmentType.size();
	for (int i=0;i<len;i++)
	{
		if (v_InvestmentType[i] == iType)
		return true;
	}
	return false;
}

void LC_ClientGamePlayer::ReSetInvestmentFreeReward()
{
	v_InvestmentFreeReward.clear();
}

void LC_ClientGamePlayer::SetInvestmentFreeReward(int iType)
{
	v_InvestmentFreeReward.push_back(iType);
}

bool LC_ClientGamePlayer::GetInvestmentFreeRewardReceive(int iType)
{
	int len = v_InvestmentFreeReward.size();
	for (int i=0;i<len;i++)
	{
		if (v_InvestmentFreeReward[i] == iType)
			return true;
	}
	return false;
}

void LC_ClientGamePlayer::SetInvestmentHighReward(int iType)
{
	v_InvestmentHighReward.push_back(iType);
}

bool LC_ClientGamePlayer::GetInvestmentHighRewardReceive(int iType)
{
	int len = v_InvestmentHighReward.size();
	for (int i=0;i<len;i++)
	{
		if (v_InvestmentHighReward[i] == iType)
			return true;
	}
	return false;
}

void LC_ClientGamePlayer::SyncActiveStarVeinIds(const MG_SyncActivateStarVein& msg)
{
	m_ActiveStarVeinIDs.clear();
	for(size_t i = 0; i < msg.m_nActivateStarVeinID.size(); ++i)
	{
		m_ActiveStarVeinIDs.insert(msg.m_nActivateStarVeinID[i]);
	}

	SetDirtyFlag(DIRTY_FLAG_STARVEIN, true);
	AddDirtyFlagRevisionID(DIRTY_FLAG_STARVEIN);
}

void LC_ClientGamePlayer::SyncActiveStarVeinGoalIds(const MG_SyncStarVeinGoalRecord& msg)
{
	m_ActiveStarVeinGoalIDs.clear();
	for(size_t i = 0; i < msg.m_nStarVeinGoalID.size(); ++i)
	{
		m_ActiveStarVeinGoalIDs.insert(msg.m_nStarVeinGoalID[i]);
	}

	SetDirtyFlag(DIRTY_FLAG_STARVEIN_GOAL, true);
	AddDirtyFlagRevisionID(DIRTY_FLAG_STARVEIN_GOAL);
}

StringType LC_ClientGamePlayer::GetActiveStarVeinIds()
{
	StringStreamType sStream("");
	for(mem::set<int>::iterator iter = m_ActiveStarVeinIDs.begin(); iter != m_ActiveStarVeinIDs.end(); ++iter)
	{
		sStream << *iter << "#";
	}

	return sStream.str();
}

StringType LC_ClientGamePlayer::GetActiveStarVeinGoalIds()
{
	StringStreamType sStream("");
	for(mem::set<int>::iterator iter = m_ActiveStarVeinGoalIDs.begin(); iter != m_ActiveStarVeinGoalIDs.end(); ++iter)
	{
		sStream << *iter << "#";
	}

	return sStream.str();
}

attr_value_type LC_ClientGamePlayer::GetStarVeinScore()
{
	attr_value_type result = 0;
	LC_ActorAttributeMap attrMap;
	for(mem::set<int>::iterator iter = m_ActiveStarVeinIDs.begin(); iter != m_ActiveStarVeinIDs.end(); ++iter)
	{
		CF_StarVeinList::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_StarVeinList>(*iter);
		if(pkData != NULL)
			result += pkData->_lCombatScore;
	}

	return result;
}

void LC_ClientGamePlayer::SetPlayerCommanderName( const StringType& rkCharName )
{
	m_pkCommander->SetPlayerCommanderName(rkCharName);
	if ( m_lastLCID > 0 )
		SendMessage(CreateMsg_UE_NAME_REDRAW(m_lastLCID));
}

void LC_ClientGamePlayer::ReqPeakArenaAchieveInfo()
{
	MG_PB<msg_define::arena_achievement_req> reqMsg;
	reqMsg.m_value.set_type(0);
	reqMsg.m_value.add_charids(GetCommanderUID());

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_SS_GLOBAL_ARENA_ACHIEVE_REQ, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::RltSyncCharacterUpgradGiftData(const MG_SyncBuddyGrowthPackage& rltMsg)
{
	m_kCharacterUpgradGift.clear();
	for (int index =0; index <rltMsg.m_nBuddyGrowthRecords.size();index++)
	{
		m_kCharacterUpgradGift[rltMsg.m_nBuddyGrowthRecords[index].m_nBuddyID] = rltMsg.m_nBuddyGrowthRecords[index];
	}
}

bool LC_ClientGamePlayer::IsBuddyActiveUpgradGift(int cID)
{
	CharacterUpgradGiftIter Iter = m_kCharacterUpgradGift.find(cID);
	if (Iter != m_kCharacterUpgradGift.end())
	{
		return Iter->second.m_nBeActived;
	}
	return false;
}

bool LC_ClientGamePlayer::IsBuddyUpgradGiftOver(int cID)
{
	CharacterUpgradGiftIter Iter = m_kCharacterUpgradGift.find(cID);
	if (Iter != m_kCharacterUpgradGift.end())
	{
		return Iter->second.m_nHasFinished;
	}
	return false;
}

int32_t LC_ClientGamePlayer::GetBuddyUpgradGiftCurrentDay(int cID)
{
	CharacterUpgradGiftIter Iter = m_kCharacterUpgradGift.find(cID);
	if (Iter != m_kCharacterUpgradGift.end())
	{
		return Iter->second.m_nCurrentDay;
	}
	return false;
}

int32_t LC_ClientGamePlayer::GetBuddyUpgradGiftDay(int cID)
{
	CharacterUpgradGiftIter Iter = m_kCharacterUpgradGift.find(cID);
	if (Iter != m_kCharacterUpgradGift.end())
	{
		return Iter->second.m_nDay;
	}
	return false;
}

bool LC_ClientGamePlayer::ReqActiveBuddyUpgradGift(int cID)
{
	if (cID <= 0 )
	{
		return false;
	}

	MG_REQ_BuyGrowthPackage reqMsg;
	reqMsg.m_nBuddyID = cID;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MG_REQ_BUY_GROWTH_PACKAGE, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
	return bCreateMsg;
}

bool LC_ClientGamePlayer::ReqGetBuddyUpgradGift(int cID)
{
	if (cID <= 0 )
	{
		return false;
	}

	MG_REQ_GetGrowthReward reqMsg;
	reqMsg.m_nBuddyID = cID;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MG_REQ_GET_GROWTH_REWARD, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
	return bCreateMsg;
}

void LC_ClientGamePlayer::RltSyncGamePlayerDailyActivity(const MG_SyncDailyActivity& rltMsg)
{
	if (m_nDailyActivity != rltMsg.m_nDailyActivity)
	{
		m_nDailyActivity = rltMsg.m_nDailyActivity;
		
	}
	if (rltMsg.m_nDailyActivityRewardID.size() >= MAX_DAILYACTIVITY_COUNT)
	{
		GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "活跃度宝箱数量异常");
	}
	else
	{
		m_nDailyActivityRewardID.clear();
		for (int index =0; index <rltMsg.m_nDailyActivityRewardID.size();index++)
		{
			m_nDailyActivityRewardID.push_back(rltMsg.m_nDailyActivityRewardID[index]);
		}
	}
}

void LC_ClientGamePlayer::RltSyncGamePlayerDailyCharge(const MG_SyncFDDay& rltMsg)
{
	if (rltMsg.m_dayFlag > 0 && rltMsg.m_dayFlag != m_dayFlag)
	{
		m_dayFlag = rltMsg.m_dayFlag;
	}

}

void LC_ClientGamePlayer::RltSyncGamePlayerDailyChargeTime(const MG_RLT_RewardTime& rltMsg)
{
	if (rltMsg.m_nResult == RE_SUCCESS)
	{
		m_dayTime = rltMsg.m_time;
	}
	else
		m_dayTime = GetServerTimeVal();

}

void LC_ClientGamePlayer::RltSyncGamePlayerDailyChargeRecord(const MG_SyncFDRewardRecord& rltMsg)
{

	m_kFirstDailyRecord.clear();

	for (int i = 0; i < rltMsg.m_FDRewardRecord.size(); i++)
	{
		m_kFirstDailyRecord[rltMsg.m_FDRewardRecord[i].day] = rltMsg.m_FDRewardRecord[i];	
	}
	

}

bool LC_ClientGamePlayer::IsGetDailyChargeNormalReward()
{
	if (m_dayFlag == 0 )
	{
		return false;
	}
	FirstDailyChargeRewardsMapIter Iter = m_kFirstDailyRecord.find(m_dayFlag);
	if (Iter != m_kFirstDailyRecord.end() )
	{
		return Iter->second.freeReward > 0;
	}
	else
	{
		return false;
	}
}

bool LC_ClientGamePlayer::IsGetAllDailyChargeReward()
{
	if (m_dayFlag == 0 )
	{
		return false;
	}
	FirstDailyChargeRewardsMapIter Iter = m_kFirstDailyRecord.find(m_dayFlag);
	if (Iter != m_kFirstDailyRecord.end() )
	{
		return Iter->second.tag;
	}
	else
	{
		return false;
	}
}

int LC_ClientGamePlayer::GetDailyChargeShopBuyCountBuyItemID(int itemID)
{
	int buyTime = 0;
	if (m_dayFlag == 0 )
	{
		return buyTime;
	}

	FirstDailyChargeRewardsMapIter Iter = m_kFirstDailyRecord.find(m_dayFlag);
	if (Iter != m_kFirstDailyRecord.end() )
	{
		for (int index = 0; index < Iter->second.disCountRewards.size(); index++ )
		{
			if (Iter->second.disCountRewards[index].disCountRewardID == itemID)
			{
				buyTime = Iter->second.disCountRewards[index].disCountRewardTimes;
				break;
			}
		}
	}

	return buyTime;
}

void LC_ClientGamePlayer::ReqDailyChargeTime()
{

	MG_REQ_RewardTime reqMsg;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MG_REQ_REWARD_TIME, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::ReqDailyChargeFreeReward(int day, int iID)
{

	MG_REQ_GetFDFreeReward reqMsg;
	reqMsg.m_nDayID = day;
	reqMsg.m_nFreeRewardID = iID;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MG_REQ_GET_FD_FREE_REWARD, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::ReqDailyChargeBuyReward(int day, int iID)
{

	MG_REQ_GetFDDisCountReward reqMsg;
	reqMsg.m_nDayID = day;
	reqMsg.m_DisCountRewardID = iID;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MG_REQ_GET_FD_DISCOUNT_REWARD, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::ReqTakeFirstChargeReward(int iDay)
{
	MG_REQ_GetFirstChargeReward reqMsg;
	reqMsg.m_nID=iDay;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GETFIRSTCHARGEREWARD, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}


void LC_ClientGamePlayer::UpdateFirstChargeRewardRecord(const std::map<int,bool>& Record)
{
	m_FirstChargeRewardRecord.clear();
	std::map<int,bool>::const_iterator iter = Record.begin();
	while(iter!=Record.end())
	{
		m_FirstChargeRewardRecord[iter->first]=iter->second;
		++iter;
	}
}

void LC_ClientGamePlayer::UpdateFirstChargeRewardRecord(int iTokenRecord)
{
	m_FirstChargeRewardRecord[iTokenRecord]=true;
}

void LC_ClientGamePlayer::UpdateFirstChargeTime(int iChargeTime)
{
	m_FirstChargeTime=iChargeTime;
}

void LC_ClientGamePlayer::SyncGamePlayerStarMoney(const MG_SyncPlayerPentacle& rltMsg)
{
	LC_Money & Pentacle = GetPentacle();
	Pentacle.SetMoney(rltMsg.m_lPentacle);
	Pentacle.UpdateDirtyFlag();
}

void LC_ClientGamePlayer::SyncACTSubInfoEnd(const MG_SyncPlayerACTSubInfo& syncMsg)
{
	m_kActSubCompeteEndRecord.clear();
	m_kActSubCompeteEndRecord[Rank_Type_Level] = syncMsg.m_nController_level;
	m_kActSubCompeteEndRecord[Rank_Type_Battle] = syncMsg.m_nController_combatScore;
	m_kActSubCompeteEndRecord[Rank_Type_Vehicle] = syncMsg.m_nSubvehicle_combatscore;
	m_kActSubCompeteEndRecord[Rank_Type_Devaeye] = syncMsg.m_nSubdevaeye_combatscore;
	m_kActSubCompeteEndRecord[Rank_Type_Wing] = syncMsg.m_nSubwing_combatscore;
	m_kActSubCompeteEndRecord[Rank_Type_Pet] = syncMsg.m_nPet_combatscore;
}

int64_t LC_ClientGamePlayer::GetACTSubInfoByRankType(int32_t rankType)
{
	if (rankType < Rank_Type_Begin || rankType > Rank_Type_End)
	{
		return 0;
	}
	ACTSubConpeteEndMapIter Iter = m_kActSubCompeteEndRecord.find(rankType);
	if (Iter != m_kActSubCompeteEndRecord.end())
	{
		return Iter->second;
	}
	else
		return 0;
}

int LC_ClientGamePlayer::GetTotalReforeLevel()
{
	int totalLevel = 0;

	LC_PackAsset *pkPackAsset = GetPackAsset();
	if(NULL == pkPackAsset)
	{
		return false;
	}
	int iSize = pkPackAsset->GetPackCapacity(PACK_TYPE_BASIC);
	for (int i = 0; i < iSize; ++i)
	{
		LC_BackPackEntry *pkEntry = pkPackAsset->GetBackPackEntry(PACK_TYPE_BASIC, i);
		if (pkEntry && pkEntry->GetValid() && IS_EQUIP_ID(pkEntry->GetTypeID()))
		{
			totalLevel += pkEntry->GetEquipLevel();
		}
	}

	for (int index = PACK_TYPE_UNDEFINE; index < PACK_TYPE_MAX_NUM; index ++)
	{
		if (IS_WAREHOUSE_PACK(index))
		{
			int iSize = pkPackAsset->GetPackCapacity(index);
			for (int i = 0; i < iSize; ++i)
			{
				LC_BackPackEntry *pkEntry = pkPackAsset->GetBackPackEntry(index, i);
				if (pkEntry && pkEntry->GetValid() && IS_EQUIP_ID(pkEntry->GetTypeID()))
				{
					totalLevel += pkEntry->GetEquipLevel();
				}
			}

		}
	}

	iSize = GetGlobalSetting.ArtifactPackSize;
	for (int index = 0; index < GetActiveBuddyListSize(); ++index)
	{
		int cID = GetBuddyActiveCID(index);
		
		for (int i = 0; i < iSize; ++i)
		{
			LC_BackPackEntry *pkEntry = GetBuddyEquipEntry(cID, i);
			if (pkEntry && pkEntry->GetValid() && IS_EQUIP_ID(pkEntry->GetTypeID()))
			{
				 totalLevel += pkEntry->GetEquipLevel();
			 }
		}
	}

	for (int index = 0; index < GetTotalBuddyNum(); ++index)
	{
		if (GetBuddyInactiveUnlock(index))
		{
			int cID = GetBuddyInactiveCID(index);
			for (int i = 0; i < iSize; ++i)
			{
				LC_BackPackEntry *pkEntry = GetBuddyEquipEntry(cID, i);
				if (pkEntry && pkEntry->GetValid() && IS_EQUIP_ID(pkEntry->GetTypeID()))
				{
					totalLevel += pkEntry->GetEquipLevel();
				}
			}
		}
	}
	
	return totalLevel;
}

void LC_ClientGamePlayer::RltSyncGamePlayerCollection(const MG_SyncCollectionInfo& rltMsg)
{
	LC_ClientCollectionManager * pkCollectionManager = LC_ClientCollectionManager::GetSingletonPtr();
	if (pkCollectionManager == NULL)
	{
		return ;
	}
	pkCollectionManager->GetDataFromProtocolStruct(rltMsg);

}

void LC_ClientGamePlayer::ReqBiographyReward(int32_t chapterID)
{
	MG_REQ_BiographyPartReward reqMsg;
	reqMsg.m_nBiographyPartID = chapterID;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MG_REQ_BIOGRAPHYPART_REWARD, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}


void	LC_ClientGamePlayer::RltBiographyReward(const MG_RLT_BiographyPartReward& rltMsg)
{
	if (rltMsg.m_nResult != RE_SUCCESS )
	{
		SendLogicInfoMessage(rltMsg.m_nResult);
	}
	else
	{
		m_nBiographyChapters.push_back(rltMsg.m_nBiographyPartID);
		LC_GameEvent GameEvent;
		GameEvent.SetEventType(GLET_NOTIFY_BIOGRAPHY_CHAPTER);
		UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(GameEvent);
		GetTaskMap()->UpdateDirtyFlag(DIRTY_FLAG_PROCESSINGTASK);
	}
}

void LC_ClientGamePlayer::SyncBiographyChapters(const MG_SyncBiographyPartReward& syncMsg)
{
	m_nBiographyChapters.clear();
	for (int i = 0; i < syncMsg.m_nBiographyPartReward.size(); ++i)
	{
		m_nBiographyChapters.push_back(syncMsg.m_nBiographyPartReward[i]);
	}
}
// 暂无必要
void LC_ClientGamePlayer::SyncBiography(const MG_SyncBiography& syncMsg)
{

}

bool	LC_ClientGamePlayer::IsChapterOver(int32_t chapterID)
{
	for (int i = 0; i < m_nBiographyChapters.size(); ++i)
	{
		if (m_nBiographyChapters[i] == chapterID)
		{
			return true;
		}
	}
	return false;
}

void LC_ClientGamePlayer::SyncTimeLimitShop(const MG_SyncPlayerTimeLimitedActivity& syncMsg)
{
	m_kShopTimeLimit.clear();

	std::vector<PS_ActivityTimeLimitedInfo>::const_iterator Iter= syncMsg.m_activetyInfo.begin();
	for ( ;Iter != syncMsg.m_activetyInfo.end(); ++Iter)
	{
		m_kShopTimeLimit.push_back(*Iter);
	}
	UpdateDirtyFlag(DIRTY_FLAG_TIMELIMIT_SHOP);
}

void LC_ClientGamePlayer::RltTimeLimitShop(const MG_SyncTimeLimitedActivityState& rltMsg)
{
	bool  bInVector = false;
	mem::vector<PS_ActivityTimeLimitedInfo>::iterator Iter= m_kShopTimeLimit.begin();
	for (; Iter != m_kShopTimeLimit.end(); )
	{
		if (Iter->m_shopTimeLimitedID == rltMsg.m_activity.m_shopTimeLimitedID )
		{
			bInVector = true;

			if(rltMsg.m_activity.m_bState != ACTIVITY_IN_PROGRESS)
			{
				Iter = m_kShopTimeLimit.erase(Iter);
			}
			else
			{
				Iter->m_bState = rltMsg.m_activity.m_bState;
				Iter->m_nEndTimeStamp = rltMsg.m_activity.m_nEndTimeStamp;
			}

			break;
		}

		++Iter;
	}
	if (!bInVector)
	{
		m_kShopTimeLimit.push_back(rltMsg.m_activity);
		LC_GameEvent GameEvent;
		GameEvent.SetEventParam(EVENT_BASE_PARAM_1, rltMsg.m_activity.m_shopTimeLimitedID);
		GameEvent.SetEventParam(EVENT_BASE_PARAM_2, m_kShopTimeLimit.size());
		GameEvent.SetEventType(GLET_NOTIFY_TIME_SHOP);
		UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(GameEvent);
	}
	else
		UpdateDirtyFlag(DIRTY_FLAG_TIMELIMIT_SHOP);
}

StringType LC_ClientGamePlayer::GetActiveTimeShopInfo()
{
	StringStreamType sStream("");
	mem::vector<PS_ActivityTimeLimitedInfo>::iterator Iter= m_kShopTimeLimit.begin();
	for (; Iter != m_kShopTimeLimit.end(); ++Iter)
	{
		sStream << Iter->m_shopTimeLimitedID << ";";
		sStream << Iter->m_bState << ";";
		sStream << Iter->m_nEndTimeStamp << ";";
		sStream << "#";
	}
	return sStream.str();
}

void LC_ClientGamePlayer::ReqUpgradCollection(int iID)
{
	CF_CollectionActivation::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_CollectionActivation>(iID);
	if(pkData == NULL)
		return;

	MG_REQ_CollectionActivation reqMsg;

	int iStartLevel = pkData->ActivationStarLevel;
	int iActivationId = pkData->ActivationID;
	switch(pkData->Type)
	{
	case COLLECTION_CHARACTER:
		{
			CF_Buddy::DataEntry *pkBuddy = SafeGetCSVFileDataEntryPtr<CF_Buddy>(iActivationId);
			if(pkBuddy == NULL)
			{
				return;
			}

			int iCurLevel = UT_MathBase::MaxInt(pkBuddy->_InitLevel,GetBuddyStarLevel(iActivationId));
			if(iStartLevel >= iCurLevel)
			{
				reqMsg.m_nCollectActivationIDs.push_back(iID);
			}
			else
			{
				CF_CollectionActivation::DataEntryMapExternIterator iter = CF_CollectionActivation::GetSingletonPtr()->GetIterator();
				for(; iter.HasMoreElements(); iter.MoveNext())
				{
					const CF_CollectionActivation::DataEntry *pkEntry = iter.PeekNextValuePtr();
					if(pkEntry->Type == pkData->Type 
						&& pkEntry->ActivationID == iActivationId
						&&pkEntry->ActivationStarLevel >= iStartLevel
						&& pkEntry->ActivationStarLevel <= iCurLevel)
					{
						reqMsg.m_nCollectActivationIDs.push_back(pkEntry->id);
					}
				}
			}
		}
		break;
	case COLLECTION_TOY:
		{
			CF_Pet::DataEntry *pkPet = SafeGetCSVFileDataEntryPtr<CF_Pet>(iActivationId);
			if(pkPet == NULL)
			{
				return;
			}

			LC_ClientToyPetManager * pkToyPetManager = LC_ClientToyPetManager::GetSingletonPtr();
			int iCurLevel = UT_MathBase::MaxInt(pkToyPetManager->GetcIDMaxLevel(iActivationId), pkPet->_InitLevel);
			if(iStartLevel >= iCurLevel)
			{
				reqMsg.m_nCollectActivationIDs.push_back(iID);
			}
			else
			{
				CF_CollectionActivation::DataEntryMapExternIterator iter = CF_CollectionActivation::GetSingletonPtr()->GetIterator();
				for(; iter.HasMoreElements(); iter.MoveNext())
				{
					const CF_CollectionActivation::DataEntry *pkEntry = iter.PeekNextValuePtr();
					if(pkEntry->Type == pkData->Type 
						&& pkEntry->ActivationID == iActivationId
						&& pkEntry->ActivationStarLevel >= iStartLevel
						&& pkEntry->ActivationStarLevel <= iCurLevel)
					{
						reqMsg.m_nCollectActivationIDs.push_back(pkEntry->id);
					}
				}
			}
		}
		break;
	case COLLECTION_RUNE:
		{
			CF_ItemList::DataEntry *pkRune = SafeGetCSVFileDataEntryPtr<CF_ItemList>(iActivationId);
			if(pkRune == NULL)
			{
				return;
			}
			
			LC_BackPackEntry* pkRune2 =GetRuneByItemId(iActivationId);
			if(pkRune2 == NULL)
			{
				return;
			}

			int iCurLevel = UT_MathBase::MaxInt(pkRune2->GetEquipStar(), pkRune->_InitStar);
			if(iStartLevel >= iCurLevel)
			{
				reqMsg.m_nCollectActivationIDs.push_back(iID);
			}
			else
			{
				CF_CollectionActivation::DataEntryMapExternIterator iter = CF_CollectionActivation::GetSingletonPtr()->GetIterator();
				for(; iter.HasMoreElements(); iter.MoveNext())
				{
					const CF_CollectionActivation::DataEntry *pkEntry = iter.PeekNextValuePtr();
					if(pkEntry->Type == pkData->Type 
						&& pkEntry->ActivationID == iActivationId
						&& pkEntry->ActivationStarLevel >= iStartLevel
						&& pkEntry->ActivationStarLevel <= iCurLevel)
					{
						reqMsg.m_nCollectActivationIDs.push_back(pkEntry->id);
					}
				}
			}
		}
		break;
	default:
		reqMsg.m_nCollectActivationIDs.push_back(iID);
		break;
	}

	if(reqMsg.m_nCollectActivationIDs.empty())
		return;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MG_REQ_COLLECTION_ACTIVATION, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::ReqActiveBot(int cID)
{
	MG_CS_BotActivateReq reqMsg;
	reqMsg.m_nResId = cID;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_CS_BOT_ACTIVATE_REQ, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::ReqUpgradBot(int cID)
{
	MG_CS_BotUpgrade reqMsg;
	reqMsg.m_nResId = cID;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_CS_BOT_UPGRADE_REQ, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void	LC_ClientGamePlayer::RltActiveBot(const MG_SC_BotActivateResult& rltMsg)
{
	if (rltMsg.m_nRetCode != RE_SUCCESS )
	{
		SendLogicInfoMessage(rltMsg.m_nRetCode);
	}
	else
	{
		LC_ClientBotManager * pkBotManager = LC_ClientBotManager::GetSingletonPtr();
		pkBotManager->GetDataFromProtocol(rltMsg.m_Data);
		LC_GameEvent GameEvent;
		GameEvent.SetEventType(GLET_NOTIFY_BOTROBOT_ACTORUP_SUCCESS);
		UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(GameEvent);
	}
}

void	LC_ClientGamePlayer::RltUpgradBot(const MG_SC_BotUpgradeResult& rltMsg)
{
	if (rltMsg.m_nErrorCode != RE_SUCCESS )
	{
		SendLogicInfoMessage(rltMsg.m_nErrorCode);
	}
	else
	{
		LC_ClientBotManager * pkBotManager = LC_ClientBotManager::GetSingletonPtr();
		pkBotManager->GetDataFromProtocol(rltMsg.m_Data);
		LC_GameEvent GameEvent;
		GameEvent.SetEventType(GLET_NOTIFY_BOTROBOT_ACTORUP_SUCCESS);
		UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(GameEvent);
	}
}

void LC_ClientGamePlayer::ReqUpgradCollectionSuit(int iID)
{

	MG_REQ_CollectionReward reqMsg;
	reqMsg.m_nCollectRewardID = iID;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MG_REQ_COLLECTION_REWARD, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::ReqGetBotProfit()
{
	MG_CS_BotRewardReq reqMsg;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_CS_BOT_REWARD_REQ, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
	//ReqGetVipNewBonus();
}

void LC_ClientGamePlayer::ReqGetVipNewBonus()
{
	MG_CS_NewVIPRewardReq reqMsg;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_CS_NEWVIP_REWARD_REQ, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::RltBotProfit(const MG_SC_BotRewardResult& rltMsg)
{
	if (rltMsg.m_nError != RE_SUCCESS )
	{
		SendLogicInfoMessage(rltMsg.m_nError);
	}
	else
	{
		LC_ClientBotManager * pkBotManager = LC_ClientBotManager::GetSingletonPtr();
		pkBotManager->AddDirtyFlagRevisionID();

	}
	
}

void LC_ClientGamePlayer::SetCommanderTitleLevel(int iLevel)
{
	if (m_pkCommander->m_iCommanderTitleLevel>=1 && m_pkCommander->m_iCommanderTitleLevel<=26)
	{
		m_pkCommander->m_iCommanderTitleLevel = iLevel;
	}
}

int LC_ClientGamePlayer::GetCommanderTitleLevel()
{
	if (m_pkCommander->m_iCommanderTitleLevel<1||m_pkCommander->m_iCommanderTitleLevel>26)
	{
		return 1;
	}
	return m_pkCommander->m_iCommanderTitleLevel;
}

void LC_ClientGamePlayer::CheckCommanderTitleOpen()
{
	CF_UIOpenLevel::DataEntry* pkEntry = SafeGetCSVFileDataEntryPtr<CF_UIOpenLevel>(COMMANDERTITLE_UIOPENLEVEL_KEY);

	if (m_bCommanderTitleOpen || (pkEntry 
		&& ((pkEntry->_iUnlockLevel > 0 || pkEntry->_iOpenLevel <= 0 || m_pkCommander->m_iLevel >= pkEntry->_iOpenLevel) 
		&& (pkEntry->_iUnlockLevel <= 0 || m_pkCommander->m_iLevel >= pkEntry->_iUnlockLevel)
		&& (pkEntry->_iUnlockTaskID > 0 || pkEntry->_iPreTaskID <= 0 || GetTaskMap()->GetTaskState(pkEntry->_iPreTaskID) == NT_FINISH) 
		&& (pkEntry->_iUnlockTaskID <= 0 || GetTaskMap()->GetTaskState(pkEntry->_iUnlockTaskID) == NT_FINISH)
		&& (pkEntry->_SpecialTrainingID <= 0 || m_pkCommander->m_iSpecialTraningLevel >= pkEntry->_SpecialTrainingID + 1))))
		m_bCommanderTitleOpen = true;
	else
		m_bCommanderTitleOpen = false;
}

bool LC_ClientGamePlayer::GetCommanderTitleOpen()
{
	return m_bCommanderTitleOpen;
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
StringType LC_ClientGamePlayer::GetCommanderTitleCurInfo()
{

	StringStreamType sStream("");	
	
	CF_CommanderTitle::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_CommanderTitle>(m_pkCommander->m_iCommanderTitleLevel);
	if (!pkData)
	{
		return sStream.str();
	}

	StringType TaskInfoStr;
	GetTaskInfoString(pkData->_sTaskID,TaskInfoStr);
	sStream<<TaskInfoStr;

	StringType AchievementInfoStr;
	GetAchievementInfoString(pkData->_sAchievementID,AchievementInfoStr);
	sStream<<AchievementInfoStr;

	return sStream.str();
}


//iTabIdx :1~4;
StringType LC_ClientGamePlayer::GetGuildBattleTaskInfo(int iTabIdx,int iSubIdx)
{
	StringStreamType sStream;
	CF_BestGuildChapterID::DataEntry *pkEntry  = SafeGetCSVFileDataEntryPtr<CF_BestGuildChapterID>(iTabIdx);
	if(pkEntry)
	{
		if (iSubIdx>=pkEntry->_iBestGuildTask.size())
		{
			return sStream.str();
		}
		int BestGuildTaskTableId=pkEntry->_iBestGuildTask[iSubIdx].GetItemId();
		CF_BestGuildTask::DataEntry *pkTaskEntry  = SafeGetCSVFileDataEntryPtr<CF_BestGuildTask>(BestGuildTaskTableId);
		if (pkTaskEntry)
		{
			StringType TaskInfoStr;
			StringType FinishedTaskInfoStr;
			StringType CanTakeRewardTaskInfoStr;
			GetTaskInfoStringSortByFinished(pkTaskEntry->_sTaskID,TaskInfoStr,FinishedTaskInfoStr,CanTakeRewardTaskInfoStr);

			StringType AchievementInfoStr;
			StringType FinishedAchievementInfoStr;
			StringType CanTakeRewardAchievementInfoStr;
			GetAchievementInfoStringSortByFinished(pkTaskEntry->_sAchievementID,AchievementInfoStr,FinishedAchievementInfoStr,CanTakeRewardAchievementInfoStr);

			sStream<<CanTakeRewardTaskInfoStr <<CanTakeRewardAchievementInfoStr
					<< TaskInfoStr<<AchievementInfoStr<<
					FinishedTaskInfoStr<<FinishedAchievementInfoStr;
		}
	}

	return sStream.str();
}

StringType LC_ClientGamePlayer::GetThemeActivityTasks(int iBuddyThemeTaskType,int iTabIdx)
{
	StringStreamType sStream;
	CF_BuddyThemeTaskType::DataEntry* pkEntry = SafeGetCSVFileDataEntryPtr<CF_BuddyThemeTaskType>(iBuddyThemeTaskType);
	if (!pkEntry)
	{
		return "";
	}
	TStringVector kProperties=UT_BaseStringUtility::SplitString(pkEntry->_sChapters, ';');	
	int id=atoi(kProperties[iTabIdx].c_str());
	CF_BuddyThemeTaskList::DataEntry* listEntry=SafeGetCSVFileDataEntryPtr<CF_BuddyThemeTaskList>(id);
	if (!listEntry)
	{
		return "";
	}
	StringType TaskInfoStr;
	StringType FinishedTaskInfoStr;
	StringType CanTakeRewardTaskInfoStr;
	GetTaskInfoStringSortByFinished(listEntry->_sTaskID,TaskInfoStr,FinishedTaskInfoStr,CanTakeRewardTaskInfoStr);
	sStream<<CanTakeRewardTaskInfoStr << TaskInfoStr << FinishedTaskInfoStr;
	return sStream.str();
}


bool LC_ClientGamePlayer::IsThemeActivityTasksPrompt(int iBuddyThemeTaskType,int iTabIdx)
{
	CF_BuddyThemeTaskType::DataEntry* pkEntry = SafeGetCSVFileDataEntryPtr<CF_BuddyThemeTaskType>(iBuddyThemeTaskType);
	if (!pkEntry)
	{
		return false;
	}
	TStringVector kProperties=UT_BaseStringUtility::SplitString(pkEntry->_sChapters, ';');	
	int id=atoi(kProperties[iTabIdx].c_str());
	CF_BuddyThemeTaskList::DataEntry* listEntry=SafeGetCSVFileDataEntryPtr<CF_BuddyThemeTaskList>(id);
	if (!listEntry)
	{
		return false;
	}
	bool bTaskCanFinish=IsTasksCanFinish(listEntry->_sTaskID);
	return bTaskCanFinish;
}

StringType LC_ClientGamePlayer::GetThemeActivityRaidTasksInfos(int iActivityId)
{
	CF_BuddyThemeBossType::DataEntry* pkEntry=SafeGetCSVFileDataEntryPtr<CF_BuddyThemeBossType>(iActivityId);
	if (!pkEntry)
	{
		return "";
	}
	StringStreamType sStream;
	StringType TaskInfoStr;
	StringType FinishedTaskInfoStr;
	StringType CanTakeRewardTaskInfoStr;
	GetTaskInfoStringSortByFinished(pkEntry->_sTaskID,TaskInfoStr,FinishedTaskInfoStr,CanTakeRewardTaskInfoStr);
	sStream<<CanTakeRewardTaskInfoStr<< TaskInfoStr<<FinishedTaskInfoStr;

	return sStream.str();
}

void LC_ClientGamePlayer::UpdateThemeACTRaidBossRewardInfo(std::map<int32_t,std::map<int32_t,bool>>& SingleDamageRewardRecord)
{
	m_nThemeACTRaidBossRwdRecord=SingleDamageRewardRecord;
}

void LC_ClientGamePlayer::AddThemeACTRaidBossRewardInfo(int iMapId,int iRwdIdx)
{
	m_nThemeACTRaidBossRwdRecord[iMapId][iRwdIdx]=true;
}

void LC_ClientGamePlayer::UpdateThemeACTRaidBossFlags(std::vector<int>& iRaidBossFlags)
{
	m_nThemeACTRaidBossFlag=iRaidBossFlags;
}

bool LC_ClientGamePlayer::IsThemeACTRaidBossPrompt(int iThemeMainId)
{
	for (int i=0;i<m_nThemeACTRaidBossFlag.size();i++)
	{
		if (iThemeMainId==m_nThemeACTRaidBossFlag[i])
		{
			return false;
		}
	}
	return true;
}

bool LC_ClientGamePlayer::IsThemeACTRaidBossRewardToken(int iMapId,int iRwdIdx)
{
	std::map<int32_t,std::map<int32_t,bool>>::iterator it1=m_nThemeACTRaidBossRwdRecord.find(iMapId);
	if (it1==m_nThemeACTRaidBossRwdRecord.end())
	{
		return false;
	}
	std::map<int32_t,bool>::iterator it2=it1->second.find(iRwdIdx);
	if (it2==it1->second.end())
	{
		return false;
	}
	return it2->second;
}

void LC_ClientGamePlayer::ReqThemeACTRaidBossRewardInfo(int iMapId,int iRwdIdx)
{
	MG_REQ_GetSingleDamageReward reqMsg;

	reqMsg.m_nMapID=iMapId;
	reqMsg.m_nRewardIndex=iRwdIdx;
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GETSINGLEDAMAGEREWARD, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::ReqThemeACTAutoExchange(int iThemeSecondaryId)
{
	MG_CS_ThemeAutoExchange_Req reqMsg;

	reqMsg.m_Id=iThemeSecondaryId;
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_CS_ThemeAutoExchange_Req, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::ReqThemeACTADAutoShow()
{
	MG_CS_ThemeFlagChange_Req reqMsg;

	reqMsg.m_Id=0;
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_CS_ThemeFlagChange_Req, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::ReqThemeACTRaidBossPrompt(int iThemeMainId)
{
	MG_REQ_SetHuanJingInVaild reqMsg;
		
	reqMsg.m_nID=iThemeMainId;
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SETHUANJINGINVAILD, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

StringType LC_ClientGamePlayer::GetGuildBattleTaskTabNames()
{
	StringStreamType sStream;
	CF_BestGuildChapterID::DataEntryMapExternIterator kIter = CF_BestGuildChapterID::GetSingletonPtr()->GetIterator();
	int iTaskCount=0;
	while(kIter.HasMoreElements())
	{
		const CF_BestGuildChapterID::DataEntry* pkEntry = kIter.PeekNextValuePtr();
		//if (pkEntry)
		{
			for(int i=0;i<pkEntry->_iBestGuildTask.size();i++)
			{
				int TaskChapterId = pkEntry->_iBestGuildTask[i].IID();
				const CF_BestGuildTask::DataEntry* pkTaskChapterEntry=SafeGetCSVFileDataEntryPtr<CF_BestGuildTask>(TaskChapterId);
				if (pkTaskChapterEntry)
				{
					sStream<< pkTaskChapterEntry->_kName<<"#";
					iTaskCount+=pkTaskChapterEntry->_sTaskID.size();
					iTaskCount+=pkTaskChapterEntry->_sAchievementID.size();
				}
			}
			sStream<<"|";
		}
		kIter.MoveNext();
	}
	sStream<<"$"<<iTaskCount;
	return sStream.str();
}

bool LC_ClientGamePlayer::IsGuildBattleTaskPrompt(int iTabIdx,int iSubTabIdx)
{
	CF_BestGuildChapterID::DataEntry *pkEntry  = SafeGetCSVFileDataEntryPtr<CF_BestGuildChapterID>(iTabIdx);
	if(pkEntry)
	{
		if (iSubTabIdx>=pkEntry->_iBestGuildTask.size())
		{
			return false;
		}
		int BestGuildTaskTableId=pkEntry->_iBestGuildTask[iSubTabIdx].GetItemId();
		CF_BestGuildTask::DataEntry *pkTaskEntry  = SafeGetCSVFileDataEntryPtr<CF_BestGuildTask>(BestGuildTaskTableId);
		if (pkTaskEntry)
		{

			bool bTaskCanFinish=IsTasksCanFinish(pkTaskEntry->_sTaskID);
			if (bTaskCanFinish)
			{
				return true;
			} 

			bool bAchievementCanFinish=IsAchievementCanFinish(pkTaskEntry->_sAchievementID);
			if (bAchievementCanFinish)
			{
				return true;
			}
		}
	}
	return false;
}

void LC_ClientGamePlayer::GetOccupiedTowers(mem::vector<int>& OutTowerIds)
{
	LC_ClientGuildManager* pkGuildManager = LC_ClientGuildManager::GetSingletonPtr();
	if (!pkGuildManager)
	{
		return;
	}
	
	guild_id_type MyGuildId = pkGuildManager->GetGuildAsset()->GetGuildInfo().GetGuildID();
	OutTowerIds.clear();
	
	for (mem::map<int, ST_Single_Tower_info>::iterator iter = m_BestGuildTowerInfo.begin(); iter!=m_BestGuildTowerInfo.end();++iter)
	{
		if (iter->second.m_nState == WatchTower_Owner && MyGuildId==iter->second.m_GuildId)
		{
			OutTowerIds.push_back(iter->second.m_uTowerID);
		}
	}
	for (mem::map<int, ST_Single_Tower_info>::iterator iter = m_BestGuildFortressInfo.begin(); iter!=m_BestGuildFortressInfo.end();++iter)
	{
		if (iter->second.m_nState == WatchTower_Owner && MyGuildId==iter->second.m_GuildId)
		{
			OutTowerIds.push_back(iter->second.m_uTowerID);
		}
	}
	for (mem::map<int, ST_Single_Tower_info>::iterator iter = m_BestGuildRoyalInfo.begin(); iter!=m_BestGuildRoyalInfo.end();++iter)
	{
		if (iter->second.m_nState == WatchTower_Owner && MyGuildId==iter->second.m_GuildId)
		{
			OutTowerIds.push_back(iter->second.m_uTowerID);
		}
	}
}

int LC_ClientGamePlayer::GetTotalVipNewBonusPercentIncrease()
{
	int OutValue=0;
	int iVipLevel=GetVIPAsset().GetVIPLevel();
	SC_ClientScriptInterface* pkClientInterface = SC_ClientScriptInterface::GetSingletonPtr();
	SC_ClientSkillInterface* pkInterface = pkClientInterface->GetSkillInterface();
	OutValue += pkInterface->GetVipPrivelegeInfoByTypeAndLevel(VIP_PROFIT_VIP_ADD_RATE,iVipLevel);

	mem::vector<int> TowerIDs;
	GetOccupiedTowers(TowerIDs);
	for (int i=0;i<TowerIDs.size();i++)
	{
		CF_BestGuildBossInfo::DataEntry *pkData=SafeGetCSVFileDataEntryPtr<CF_BestGuildBossInfo>(TowerIDs[i]);
		if (pkData)
		{
			OutValue+=pkData->_iVipRate;
		}
		
	}
	return OutValue;
}

int LC_ClientGamePlayer::GetVipPriviligeAddTimesByType(int iType)
{
	int iVipLevel=GetVIPAsset().GetVIPLevel();
	SC_ClientScriptInterface* pkClientInterface = SC_ClientScriptInterface::GetSingletonPtr();
	SC_ClientSkillInterface* pkInterface = pkClientInterface->GetSkillInterface();
	return pkInterface->GetVipPrivelegeInfoByTypeAndLevel(iType,iVipLevel);
}

void LC_ClientGamePlayer::UpdateEscortTimes(int iValue)
{
	m_iEscortTimes=iValue;
}

void LC_ClientGamePlayer::UpdateRobbingTimes(int iValue)
{
	m_iRobbingTimes=iValue;
}

void LC_ClientGamePlayer::UpdateEscortLevel()
{
	int TaskIds[4]={1100008,1100009,1100010,1100011};
	LC_TaskMap* pTaskMap=GetTaskMap();
	for (int i=0;i<4;i++)
	{
		if (pTaskMap->IsInAssignTaskMap(TaskIds[i]))
		{
			m_iEscortLevel=i+1;
			return;
		}
	}
}

void LC_ClientGamePlayer::UpdateEscortState(int iState)
{
	m_iEscortState=iState;
}

int LC_ClientGamePlayer::GetEscortState()
{
	return m_iEscortState;
}

int LC_ClientGamePlayer::GetEscortTaskID()
{
	int TaskIds[4]={1100008,1100009,1100010,1100011};
	LC_TaskMap* pTaskMap=GetTaskMap();
	for (int i=0;i<4;i++)
	{
		if (pTaskMap->IsInProcessingTaskMap(TaskIds[i]))
		{
			return TaskIds[i];
		}
	}
	return 0;
}

StringType LC_ClientGamePlayer::GetSpecialTrainingTaskInfo(int iLevel)
{
	StringStreamType sStream("");	
	//Test
	//sStream<<"1:9000001:10000030:10:5:6:1:Hello1|";

	CF_SpecialTraining::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_SpecialTraining>(iLevel);
	if (!pkData)
	{
		return sStream.str();
	}
	
	StringType TaskInfoStr;
	GetTaskInfoString(pkData->_taskID,TaskInfoStr);
	sStream<<TaskInfoStr;
	
	StringType AchievementInfoStr;
	GetAchievementInfoString(pkData->_achievementID,AchievementInfoStr);
	sStream<<AchievementInfoStr;
	return sStream.str();
}


int LC_ClientGamePlayer::GetSpecialTrainingLevel()
{
	return m_pkCommander->m_iSpecialTraningLevel;
}


int LC_ClientGamePlayer::GetSpecialTrainingMaxLevel()
{
	return CF_SpecialTraining::GetSingletonPtr()->GetEntryCount();
}


void LC_ClientGamePlayer::GetTaskInfoString(Utility::UT_SIMDataList& InTasks,StringType& OutStr)
{
	StringStreamType sStream("");
	OutStr.clear();
	mem::vector<UT_SIMDataInfo>::iterator it = InTasks.begin();
	while (it != InTasks.end())
	{
		int iTaskID=it->GetTaskID();
		CF_TaskList::DataEntry *pkTaskData=SafeGetCSVFileDataEntryPtr<CF_TaskList>(iTaskID);
		if(!pkTaskData)
		{
			++it;
			continue;
		}

		LC_TaskMap* pkTaskMap = GetTaskMap();
		if (!pkTaskMap)
		{
			++it;
			continue;
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
			ReqAcceptTask(iTaskID);
			CurCount=0;
		}
		else
		{
			CurCount=MaxCount;
		}



		sStream<<"1:"
			<<iTaskID<<":"
			<<kvect[0]<<":"
			<<kvect[1]<<":"
			<<CurCount<<":"
			<<MaxCount<<":"
			<<bFinished<<":"
			<<TraceInfo<<":"
			<<"0"<<"|";
		++it;
	}
	OutStr=sStream.str();
}


void LC_ClientGamePlayer::GetAchievementInfoString(Utility::UT_SIMDataList& InAchievements,StringType& OutStr)
{
	StringStreamType sStream("");
	OutStr.clear();
	mem::vector<UT_SIMDataInfo>::iterator it2 = InAchievements.begin();
	CAchievementAsset& pkAchievementMap = GetAchievementAsset();
	while (it2 !=  InAchievements.end())
	{
		int iAchievementID=it2->GetTaskID();

		CF_AchievementList::DataEntry *pkAchivementData=SafeGetCSVFileDataEntryPtr<CF_AchievementList>(iAchievementID);
		if(!pkAchivementData)
		{
			++it2;
			continue;
		}

		
		//if (!pkAchievementMap)
		//{
		//	++it2;
		//	continue;
		//}
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

		sStream<<"0:"
			<<iAchievementID<<":"
			<<kvect[0]<<":"
			<<kvect[1]<<":"
			<<iProgress<<":"
			<<iMax<<":"
			<<bFinished<<":"
			<<pkAchivementData->_sAchievementinfo<<":"
			<<pkAchivementData->_iLinkID<<"|";
		++it2;
	}
	OutStr=sStream.str();
}


void LC_ClientGamePlayer::GetTaskInfoStringSortByFinished(Utility::UT_SIMDataList& InTasks,StringType& OutStr,StringType& OutStr2,StringType& OutStr3)
{
	StringStreamType sStream("");
	StringStreamType sStream2("");
	StringStreamType sStream3("");
	OutStr.clear();
	OutStr2.clear();
	OutStr3.clear();
	mem::vector<UT_SIMDataInfo>::iterator it = InTasks.begin();
	while (it != InTasks.end())
	{
		int iTaskID=it->GetTaskID();
		CF_TaskList::DataEntry *pkTaskData=SafeGetCSVFileDataEntryPtr<CF_TaskList>(iTaskID);
		if(!pkTaskData)
		{
			++it;
			continue;
		}

		LC_TaskMap* pkTaskMap = GetTaskMap();
		if (!pkTaskMap)
		{
			++it;
			continue;
		}
		bool bFinished=pkTaskMap->IsInFinishedTaskMap(iTaskID);
		LC_ProcessingTaskEntry* pkTaskEntry = pkTaskMap->GetProcessingTaskEntryByID(iTaskID);
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
			ReqAcceptTask(iTaskID);
			CurCount=0;
		}
		else
		{
			CurCount=MaxCount;
		}


		if (bFinished)
		{
			sStream2<<"1$"
				<<iTaskID<<"$"
				<<pkTaskData->_sRewardItemsA<<"$"
				<<"a"<<"$"
				<<CurCount<<"$"
				<<MaxCount<<"$"
				<<bFinished<<"$"
				<<TraceInfo<<"$"
				<<"0"<<"|";
		}
		else if(CurCount>=MaxCount)
		{
			sStream3<<"1$"
				<<iTaskID<<"$"
				<<pkTaskData->_sRewardItemsA<<"$"
				<<"a"<<"$"
				<<CurCount<<"$"
				<<MaxCount<<"$"
				<<bFinished<<"$"
				<<TraceInfo<<"$"
				<<"0"<<"|";
		}
		else
		{
			sStream<<"1$"
				<<iTaskID<<"$"
				<<pkTaskData->_sRewardItemsA<<"$"
				<<"a"<<"$"
				<<CurCount<<"$"
				<<MaxCount<<"$"
				<<bFinished<<"$"
				<<TraceInfo<<"$"
				<<"0"<<"|";
		}
		
		++it;
	}
	OutStr=sStream.str();
	OutStr2=sStream2.str();
	OutStr3=sStream3.str();
}

void LC_ClientGamePlayer::GetAchievementInfoStringSortByFinished(Utility::UT_SIMDataList& InAchievements,StringType& OutStr,StringType& OutStr2,StringType& OutStr3)
{
	StringStreamType sStream("");
	StringStreamType sStream2("");
	StringStreamType sStream3("");
	OutStr.clear();
	OutStr2.clear();
	OutStr3.clear();
	mem::vector<UT_SIMDataInfo>::iterator it2 = InAchievements.begin();
	SC_ClientScriptInterface* pkClientInterface = SC_ClientScriptInterface::GetSingletonPtr();
	SC_ClientSkillInterface* pkInterface = pkClientInterface->GetSkillInterface();
	CAchievementAsset& pkAchievementMap = GetAchievementAsset();
	while (it2 !=  InAchievements.end())
	{
		int iAchievementID=it2->GetTaskID();

		CF_AchievementList::DataEntry *pkAchivementData=SafeGetCSVFileDataEntryPtr<CF_AchievementList>(iAchievementID);
		if(!pkAchivementData)
		{
			++it2;
			continue;
		}

	
		//if (!pkAchievementMap)
		//{
		//	++it2;
		//	continue;
		//}
		bool bFinished=pkAchievementMap.IsInFinishAchievementRecord(iAchievementID);
		int64_t iMax=pkAchivementData->_iConditionThreshold;
		int64_t iProgress = 0;

		int stat=pkInterface->GetActivityScheduleStat(pkAchivementData->_iScheduleID);

		if (stat!=0)
		{
			iProgress=0;
			bFinished=false;
		}
		else if (bFinished)
		{
			iProgress = iMax;
		}
		else
		{
			iProgress = pkAchievementMap.GetAchievementProcessing(iAchievementID);
		}
		if (bFinished)
		{
			sStream2<<"0$"
				<<iAchievementID<<"$"
				<<pkAchivementData->_sRewardItems<<"$"
				<<"a"<<"$"
				<<iProgress<<"$"
				<<iMax<<"$"
				<<bFinished<<"$"
				<<pkAchivementData->_sAchievementinfo<<"$"
				<<pkAchivementData->_iLinkID<<"|";
		}
		else if (iProgress>=iMax)
		{
			sStream3<<"0$"
				<<iAchievementID<<"$"
				<<pkAchivementData->_sRewardItems<<"$"
				<<"a"<<"$"
				<<iProgress<<"$"
				<<iMax<<"$"
				<<bFinished<<"$"
				<<pkAchivementData->_sAchievementinfo<<"$"
				<<pkAchivementData->_iLinkID<<"|";
		}
		else
		{
			sStream<<"0$"
				<<iAchievementID<<"$"
				<<pkAchivementData->_sRewardItems<<"$"
				<<"a"<<"$"
				<<iProgress<<"$"
				<<iMax<<"$"
				<<bFinished<<"$"
				<<pkAchivementData->_sAchievementinfo<<"$"
				<<pkAchivementData->_iLinkID<<"|";
		}
		
		++it2;
	}
	OutStr=sStream.str();
	OutStr2=sStream2.str();
	OutStr3=sStream3.str();
}

void LC_ClientGamePlayer::RewardMapToRewardString(mem::map<int,int>& RwdMap,StringType& RwdStr)
{
	StringStreamType sStream("");
	mem::map<int,int>::iterator it=RwdMap.begin();
	for (;it!=RwdMap.end();++it)
	{
		sStream<<it->first<<":"<<it->second<<";";
	}
	RwdStr=sStream.str();
}

bool LC_ClientGamePlayer::IsTasksCanFinish(Utility::UT_SIMDataList& InTasks)
{
	mem::vector<UT_SIMDataInfo>::iterator it = InTasks.begin();
	LC_TaskManager* pTaskMgr=LC_TaskManager::GetSingletonPtr();
	LC_TaskMap* pkTaskMap = GetTaskMap();
	if (!pkTaskMap)
	{
		return false;
	}
	while (it != InTasks.end())
	{
		int iTaskID=it->GetTaskID();
		CF_TaskList::DataEntry *pkTaskData=SafeGetCSVFileDataEntryPtr<CF_TaskList>(iTaskID);
		if(!pkTaskData)
		{
			++it;
			continue;
		}


		bool bFinished=pkTaskMap->IsInFinishedTaskMap(iTaskID);
		LC_ProcessingTaskEntry* pkTaskEntry = pkTaskMap->GetProcessingTaskEntryByID(iTaskID);
		LC_Task* pTask = pTaskMgr->GetTask(iTaskID);
		LC_Task_Condition* pCondition=pTask->GetFinishConditionNoParams(0);
		int CurCount=0;
		int MaxCount = pCondition->GetMaxCount();
		if (pkTaskEntry)
		{
			CurCount=pkTaskEntry->GetFinishState1();
			if (CurCount>=MaxCount)
			{
				return true;
			}
		}
		++it;
	}
	return false;
}

bool LC_ClientGamePlayer::IsAchievementCanFinish(Utility::UT_SIMDataList& InAchievements)
{
	mem::vector<UT_SIMDataInfo>::iterator it2 = InAchievements.begin();
	SC_ClientScriptInterface* pkClientInterface = SC_ClientScriptInterface::GetSingletonPtr();
	SC_ClientSkillInterface* pkInterface = pkClientInterface->GetSkillInterface();
	CAchievementAsset& pkAchievementMap = GetAchievementAsset();
	while (it2 != InAchievements.end())
	{
		int iAchievementID=it2->GetTaskID();

		CF_AchievementList::DataEntry *pkAchivementData=SafeGetCSVFileDataEntryPtr<CF_AchievementList>(iAchievementID);
		if(!pkAchivementData)
		{
			++it2;
			continue;
		}
		bool bFinished=pkAchievementMap.IsInFinishAchievementRecord(iAchievementID);
		int64_t iMax=pkAchivementData->_iConditionThreshold;
		int64_t iProgress = 0;

		int stat=pkInterface->GetActivityScheduleStat(pkAchivementData->_iScheduleID);

		if (stat==0 && !bFinished)
		{
			iProgress = pkAchievementMap.GetAchievementProcessing(iAchievementID);
			if(iProgress>=iMax)
			{
				return true;
			}
		}
		++it2;
	}
	return false;
}

int LC_ClientGamePlayer::ReqTakeCommanderTitleUpgradeReward()
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	MG_REQ_UpgradeCommanderTitle reqMsg;

	bool res = false;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_UPGRADECOMMANDERTITLE, &reqMsg);
	if (bCreateMsg)
	{
		this->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
		res = true;
	}
	return res;
}


int LC_ClientGamePlayer::ReqTakeSpecialTrainingReward()
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	MG_REQ_GetSpecialTrainingReward reqMsg;
	reqMsg.m_nID =m_pkCommander->m_iSpecialTraningLevel>=1?m_pkCommander->m_iSpecialTraningLevel:1;
	bool res = false;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GETSPECIALTRAININGREWARD, &reqMsg);
	if (bCreateMsg)
	{
		this->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
		res = true;
	}
	return res;
}


void LC_ClientGamePlayer::ReqDeCompositionEquips(StringType equipIndex)
{
	MG_REQ_DecompositionItem reqMsg;
	TStringVector kVector = UT_BaseStringUtility::SplitString(equipIndex, ';');
	for (int i = 0; i < kVector.size(); ++i)
	{
		int32_t index = atoi(kVector[i].c_str());
		if (index >= 0)
		{
			reqMsg.m_DecompositionItemIndexs.push_back(index);
		}
	}
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_DECOMPOSITION_ITEM, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}


void LC_ClientGamePlayer::RltDeCompositionEquips(const MG_RLT_DecompositionItem &rltMsg )
{
	if (rltMsg.m_nResult != RE_SUCCESS )
	{
		SendLogicInfoMessage(rltMsg.m_nResult);
	}
	else
	{
		StringStreamType sStream("");
		for (int i = 0; i < rltMsg.m_nDecompositionResult.size(); i++)
		{
			const PS_ItemInfo* ItemInfo = &rltMsg.m_nDecompositionResult[i];
			sStream << ItemInfo->m_nItemID << ";";
			sStream << ItemInfo->m_nItemNum << "#";
		}
		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerMsgGameEvent(GLET_NOTIFY_REINFORCE_DECOMPOSITION, sStream.str());
	}
}

void LC_ClientGamePlayer::ReqDecompositionSetting(bool bCommander, int iClevel, int iCgrade, bool bVehicle, int iVlevel, int iVgrade, bool bDevaeye, int iDlevel, int iDgrade, bool bWing, int iWlevel, int iWgrade)
{
	MG_REQ_DecompositionSetting reqMsg;

	reqMsg.m_bDecompositionController	= bCommander;
	reqMsg.m_nControllerItemlevel		= iClevel;
	reqMsg.m_nControllerItemGrade		= iCgrade;
	reqMsg.m_bDecompositionVehicle		= bVehicle;
	reqMsg.m_nVehicleItemLevel			= iVlevel;
	reqMsg.m_nVehicleItemGrade			= iVgrade;
	reqMsg.m_bDecompositionDevaeye		= bDevaeye;
	reqMsg.m_nDevaeyeItemLevel			= iDlevel;
	reqMsg.m_nDevaeyeItemGrade			= iDgrade;
	reqMsg.m_bDecompositionWing			= bWing;
	reqMsg.m_nWingItemLevel				= iWlevel;
	reqMsg.m_nWingItemGrade				= iWgrade;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_DECOMPOSITION_SETTING, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::RltDecompositionSetting(const MG_RLT_DecompositionSetting &rltMsg )
{
	StringStreamType sStream("");
	sStream << rltMsg.m_bDecompositionController << "#" << rltMsg.m_nControllerItemlevel << "#" << rltMsg.m_nControllerItemGrade << "#" << rltMsg.m_bDecompositionVehicle << "#" 
			<< rltMsg.m_nVehicleItemLevel << "#" << rltMsg.m_nVehicleItemGrade << "#" << rltMsg.m_bDecompositionWing << "#" << rltMsg.m_nWingItemLevel << "#" 
			<< rltMsg.m_nWingItemGrade << "#" << rltMsg.m_bDecompositionDevaeye << "#" << rltMsg.m_nDevaeyeItemLevel << "#" << rltMsg.m_nDevaeyeItemGrade << "#" 
			<< rltMsg.m_nAutoDecomposition << "#" ;
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerMsgGameEvent(MGPT_RLT_DECOMPOSITION_SETTING, sStream.str());
}

void LC_ClientGamePlayer::ReqQuickRaidNew(int iMapId,int iNum)
{
	if (iNum<=0)
	{
		return;
	}
	MG_REQ_QuickRaid_New reqMsg;
	reqMsg.m_nResMapID=iMapId;
	reqMsg.m_nRaidNum=iNum;
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_QUICKRAID_NEW, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}


void LC_ClientGamePlayer::UpdateRaidGloryTotalStarNum(int iNum)
{
	m_iRaidGloryTotalStarNum=iNum;
	AddDirtyFlagRevisionID(DIRTY_FLAG_RAIDGLORYINFO);
}

void LC_ClientGamePlayer::UpdateRaidGloryChapterStarNum(int iChapterId,int iNum)
{
	m_iRaidGloryChapterStarNum[iChapterId-1]=iNum;
	AddDirtyFlagRevisionID(DIRTY_FLAG_RAIDGLORYINFO);
}

void LC_ClientGamePlayer::UpdateGloryChapterRewardRecord(int iChapterId,std::vector<int>& InRecord)
{
	for (int i=0;i<InRecord.size();i++)
	{
		int idx=(iChapterId-1)*3+InRecord[i]-1;
		m_iRaidGloryRewardRecord[idx]=true;
	}
	AddDirtyFlagRevisionID(DIRTY_FLAG_RAIDGLORYINFO);
}

void LC_ClientGamePlayer::UpdateQuickRaidGloryCount(std::map<int,int>& InRecord)
{
	std::map<int,int>::iterator it=InRecord.begin();
	while(it!=InRecord.end())
	{
		m_nQuickRaidGloryCountMap[it->first]=it->second;
		++it;
	}
	AddDirtyFlagRevisionID(DIRTY_FLAG_RAIDGLORYINFO);
}

void LC_ClientGamePlayer::UpdateRaidGloryPointRecoverTimestamp(int iNextTime,int iEndTime)
{
	m_iGloryPointNextRecoverTime=iNextTime;
	m_iGloryPointEndRecoverTime=iEndTime;
}

int LC_ClientGamePlayer::GetRaidGloryTotalStarNum()
{
	return m_iRaidGloryTotalStarNum;
}

int LC_ClientGamePlayer::GetRaidGloryChapterStarNum(int iChapterId)
{	
	if (m_iRaidGloryChapterStarNum.size()>iChapterId-1)
	{
		return m_iRaidGloryChapterStarNum[iChapterId-1];
	}
	return 0;
}



//iChapterId从1开始
StringType LC_ClientGamePlayer::GetRaidGloryChapterRewardRecord(int iChapterId)
{
	StringStreamType sStream("");
	int iBegin=(iChapterId-1)*3;
	for (int i=iBegin;i<iBegin+3;i++)
	{
		sStream << m_iRaidGloryRewardRecord[i]<<"#";
	}
	return sStream.str();
}

int LC_ClientGamePlayer::GetRaidGloryPointRecoverNextTimestamp()
{
	return m_iGloryPointNextRecoverTime;
}

int LC_ClientGamePlayer::GetRaidGloryPointRecoverEndTimestamp()
{
	return m_iGloryPointEndRecoverTime;
}

void LC_ClientGamePlayer::ReqReceiveGloryDailyReward()
{
	MGPT_REQ_Daily_Reward reqMsg;
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_RECEIVE_DAILY_REWARD, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

StringType LC_ClientGamePlayer::GetRaidGloryCurDailyReward()
{
	StringStreamType sStream;
	int iMapID=GetRaidGloryHighestMap();
	sStream << iMapID << "#";
	if (iMapID > 0)
	{
		CF_GloryExpeditionDailyRewards::DataEntry *pkData=SafeGetCSVFileDataEntryPtr<CF_GloryExpeditionDailyRewards>(iMapID);
		StringType sStr;
		FormatDataIFs2String(pkData->_kReward, sStr);
		sStream << sStr << "#";
	}
	else
	{
		CF_GloryExpeditionDailyRewards::DataEntryMapExternIterator lIter = CF_GloryExpeditionDailyRewards::GetSingleton().GetIterator();
		CF_GloryExpeditionDailyRewards::DataEntry *pkData = lIter.PeekNextValuePtr();
		StringType sStr;
		FormatDataIFs2String(pkData->_kReward, sStr);
		sStream << sStr << "#";
	}
	return sStream.str();
}

StringType LC_ClientGamePlayer::GetRaidGloryMyMissionIdx()
{
	SC_ClientScriptInterface* pkClientInterface = SC_ClientScriptInterface::GetSingletonPtr();
	SC_ClientSkillInterface* pkInterface = pkClientInterface->GetSkillInterface();
	int iMapID=GetRaidGloryHighestMap();
	const RaidInfo* pkRaid = m_kraidAssert.FindRaid(iMapID);
	if (pkRaid)
	{
		StringType NextMissionStr = pkInterface->GetDataString(DataStringType_RaidGloryNextMissionId,iMapID,0);
		TStringVector kStringVector = UT_BaseStringUtility::SplitString(NextMissionStr, '#');
		int iResult=atoi(kStringVector[0].c_str());
		if (iResult!=0)
		{
			iMapID=iResult;
		}
		else
		{
			int iChapterId = pkInterface->GetRaidGloryChapterNum();
			CF_GloryExpedition::DataEntry *pkEntry  = SafeGetCSVFileDataEntryPtr<CF_GloryExpedition>(iChapterId);
			T_ASSERT_CRITICAL_S(pkEntry != NULL);

			StringStreamType sStream;
			sStream << iChapterId << '#' << pkEntry->_iTranscriptionRuleID.size() << "#1";
			return sStream.str();
		}
	}
	return pkInterface->GetDataString(DataStringType_GloryExpeditionMissionIndex,iMapID,0);
	
}

void LC_ClientGamePlayer::ReqGetChapterRaidStarReward(int iChapterId,int iStarIndex)
{
	MG_REQ_GetChapterRaidStarReward reqMsg;
	reqMsg.m_nChapterID=iChapterId;
	reqMsg.m_nRewardIndex=iStarIndex;
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GETCHAPTERRAIDSTARREWARD, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::RltUpgradCollection(const MG_RLT_CollectionActivation& rltMsg)
{
	if (rltMsg.m_nResult != RE_SUCCESS )
	{
		SendLogicInfoMessage(rltMsg.m_nResult);
	}
	else
	{
		LC_ClientCollectionManager * pkCollectionManager = LC_ClientCollectionManager::GetSingletonPtr();
		if (pkCollectionManager == NULL)
		{
			return ;
		}
		
		pkCollectionManager->GetDataFromProtocolCollection(rltMsg.m_nCollectActivationIDs);

		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_COLLECTION_UPGRADE_SUCCESS);
		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
}

void LC_ClientGamePlayer::RltUpgradCollectionSuit(const MG_RLT_CollectionReward& rltMsg)
{
	if (rltMsg.m_nResult != RE_SUCCESS )
	{
		SendLogicInfoMessage(rltMsg.m_nResult);
	}
	else
	{
		LC_ClientCollectionManager * pkCollectionManager = LC_ClientCollectionManager::GetSingletonPtr();
		if (pkCollectionManager == NULL)
		{
			return ;
		}
		pkCollectionManager->GetDataFromProtocolSuit(rltMsg.m_nCollectRewardID);
		
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_COLLECTION_UPGRADE_SUCCESS);
		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
}

void LC_ClientGamePlayer::RltSyncBotData(const MG_SC_BotDataSync& rltMsg)
{
	LC_ClientBotManager * pkBotManager = LC_ClientBotManager::GetSingletonPtr();
	if (pkBotManager == NULL)
	{
		return ;
	}
	pkBotManager->ParseProtocol(rltMsg);
}

void LC_ClientGamePlayer::RltSyncBotProfitData(const MG_SC_BotBonusCacheSync& rltMsg)
{
	LC_ClientBotManager * pkBotManager = LC_ClientBotManager::GetSingletonPtr();
	if (pkBotManager == NULL)
	{
		return ;
	}
	pkBotManager->ParseProfitDataFromProtocol(rltMsg);
}

void LC_ClientGamePlayer::RltSyncVipNewProfitData(const MG_SC_NewVIPBonusCacheSync& rltMsg)
{
	LC_ClientBotManager * pkBotManager = LC_ClientBotManager::GetSingletonPtr();
	if (pkBotManager == NULL)
	{
		return ;
	}
	pkBotManager->ParseVipNewProfitDataFromProtocol(rltMsg);
}

void LC_ClientGamePlayer::RltSyncBotBeginTime(const MG_SC_BotDataSync_BeginTime& rltMsg)
{
	LC_ClientBotManager * pkBotManager = LC_ClientBotManager::GetSingletonPtr();
	if (pkBotManager == NULL)
	{
		return ;
	}
	pkBotManager->GetBeginFromProtocol(rltMsg);
}

//void LC_ClientGamePlayer::UpdateBuyNonpareilRecord(MG_SyncNonpareilRecord& InRecord)
//{
//	m_nNonpareilRecord.clear(); 
//	for(int i=0;i<InRecord.m_nNonpareilRecord.size();i++)
//	{
//		int key = InRecord.m_nNonpareilRecord[i];
//		m_nNonpareilRecord[key] = InRecord.m_nNonpareilReturnTime[i];
//		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MGPT_SYNCNONPAREILRECORD id = %d,time=%d",InRecord.m_nNonpareilRecord[i],InRecord.m_nNonpareilReturnTime[i]);
//	}
//
//}
//
//void LC_ClientGamePlayer::UpdateNonpareilReturnRecord(std::vector<int>& InRecord)
//{
//	m_nNonpareilReturnRecord.clear();
//	m_nNonpareilReturnRecord.reserve(InRecord.size());
//	for(int i=0;i<InRecord.size();i++)
//	{
//		m_nNonpareilReturnRecord.push_back(InRecord[i]);
//		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MGPT_SYNCNONPAREILRETURNRECORD index = %d record=%d",i,InRecord[i]);
//	}
//}
//
//void LC_ClientGamePlayer::UpdateBuyNonpareilEndTime(int iBuyTime,int iReturnTime)
//{
//	m_nNonpareilBuyTime=iBuyTime;	
//	m_nNonpareilReturnTime=iReturnTime;	
//}
//
//int32_t LC_ClientGamePlayer::GetIsNonpareilBought(int id)
//{
//	mem::map<int, int32_t>::iterator itor = m_nNonpareilRecord.find(id);
//	if (itor != m_nNonpareilRecord.end())
//	{
//		return itor->second;
//	}
//	return -1;
//}
//
//bool LC_ClientGamePlayer::GetIsNonpareilReturnRewardToken(int id)
//{
//	for (int i=0;i<m_nNonpareilReturnRecord.size();i++)
//	{
//		if (id==m_nNonpareilReturnRecord[i])
//		{
//			return true;
//		}
//	}
//	return false;
//}
//
//int LC_ClientGamePlayer::GetNonpareilBuyTimeEnd()
//{
//	return m_nNonpareilBuyTime;
//}
//
//int LC_ClientGamePlayer::GetNonpareilReturnTimeEnd()
//{
//	return m_nNonpareilReturnTime;
//}


int LC_ClientGamePlayer::GetBestGuildFinishedTaskNum()
{
	return m_BestGuildFinishedTask;
}

//========================================================================================================
void LC_ClientGamePlayer::RltSyncGamePlayerBossFirstKill(const MG_SyncBossFirstKillInfo& rltMsg)
{
	const PS_BossFirstKillPlayerInfo& tarInfo = rltMsg.m_nKillerInfo;
	PS_BossFirstKillPlayerInfo& info = m_kBossFirstKill[rltMsg.m_nBossID];
	info.m_nPlayerName = tarInfo.m_nPlayerName;
	info.m_nPlayerLevel = tarInfo.m_nPlayerLevel;
	info.m_nBornCharType = tarInfo.m_nBornCharType;
	info.m_nCommanderTitleID = tarInfo.m_nCommanderTitleID;
	info.m_nCommanderTitleFrameID = tarInfo.m_nCommanderTitleFrameID;
	
	if(rltMsg.m_bKillInfo)
	{
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_NOTIFY_BOSSFIRSTKILL_PLAYERNAME);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, rltMsg.m_nBossID);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
	else
	{
		mem::vector<PS_BossFirstKillPlayerInfo>& playerInfo = m_mBossFirstKillRewardPlayerInfo[rltMsg.m_nBossID];
		playerInfo.clear();

		std::vector<PS_BossFirstKillPlayerInfo>::const_iterator iter = rltMsg.m_nRewardPlayerInfo.begin();
		for(; iter!=rltMsg.m_nRewardPlayerInfo.end(); ++iter)
		{
			playerInfo.push_back(*iter);
		}
	}
}

StringType LC_ClientGamePlayer::GetBossFirstKillInfo(int32_t charTypeID)
{
	StringStreamType sStream;

	mem::map<int32_t, PS_BossFirstKillPlayerInfo>::const_iterator iter = m_kBossFirstKill.find(charTypeID);
	if (iter != m_kBossFirstKill.end())
	{
		const PS_BossFirstKillPlayerInfo& info = iter->second;
		sStream << info.m_nPlayerName << '#' << info.m_nCommanderTitleID << '#';
		sStream << info.m_nPlayerLevel << '#' << info.m_nBornCharType << '#' << info.m_nCommanderTitleFrameID;
	}

	return sStream.str();
}

void LC_ClientGamePlayer::SyncFirstKillBossReward(const MG_SyncKillBossReward& msg)
{
	std::map<int32_t,bool>::const_iterator iter = msg.m_nBossRewardInfos.begin();
	for( ; iter!=msg.m_nBossRewardInfos.end(); ++iter)
	{
		m_mBossFirstKillRecord[iter->first] = iter->second;
	}

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_SYNC_BOSSFIRSTKILL_REWARD_STATE);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

bool LC_ClientGamePlayer::IsBossFirstKillRecorded(int charTypeID)
{
	mem::map<int, bool>::const_iterator iter = m_mBossFirstKillRecord.find(charTypeID);
	if(iter == m_mBossFirstKillRecord.end())
		return false;

	return iter->second;
}

bool LC_ClientGamePlayer::IsBossKilledBySelf(int charTypeID)
{
	mem::map<int, bool>::const_iterator iter = m_mBossFirstKillRecord.find(charTypeID);
	return iter != m_mBossFirstKillRecord.end();
}

void LC_ClientGamePlayer::RltSyncServerBossFirstKillRecord(const MG_SyncServerFirstKillBossReward& rltMsg)
{
	m_kServerBossFirstKillRecord.clear();
	std::vector<int32_t>::const_iterator iter = rltMsg.m_nFirstKillBossRewardRecord.begin();
	for( ; iter!=rltMsg.m_nFirstKillBossRewardRecord.end(); ++iter)
	{
		m_kServerBossFirstKillRecord.insert(*iter);
	}

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_SYNC_BOSSFIRSTKILL_REWARD_STATE);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

bool LC_ClientGamePlayer::IsServerBossFirstKillRecorded(int32_t charTypeID)
{
	return m_kServerBossFirstKillRecord.find(charTypeID) != m_kServerBossFirstKillRecord.end();
}

StringType LC_ClientGamePlayer::GetBossFirstKillRewardPlayerInfo(int32_t charTypeID)
{
	FirstKillRewardPlayerMap::const_iterator mapItr = m_mBossFirstKillRewardPlayerInfo.find(charTypeID);
	if(mapItr == m_mBossFirstKillRewardPlayerInfo.end())
		return "";

	StringStreamType sStream;

	const mem::vector<PS_BossFirstKillPlayerInfo>& playerInfo = mapItr->second;
	mem::vector<PS_BossFirstKillPlayerInfo>::const_iterator iter = playerInfo.begin();
	for(; iter!=playerInfo.end(); ++iter)
	{
		const PS_BossFirstKillPlayerInfo& info = *iter;
		sStream << info.m_nPlayerName << '#' << info.m_nCommanderTitleID << '#';
		sStream << info.m_nPlayerLevel << '#' << info.m_nBornCharType << '#' << info.m_nCommanderTitleFrameID << '|';
	}

	return sStream.str();
}

void LC_ClientGamePlayer::RequestDrawFirstKillReward(int iCharType, bool bServer)
{
	MG_REQ_KillBossReward reqMsg;
	reqMsg.m_nNpcCharID = iCharType;
	reqMsg.m_bServerReward = bServer;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_KILLBOSSREWARD, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::RequestAutoDecompositionItem(bool bauto)
{
	MG_REQ_SetAutoDecompositionItem reqMsg;
	reqMsg.m_nAutoDecomposition = bauto;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SETAUTODECOMPOSITIONITEM, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}
//========================================================================================================
void LC_ClientGamePlayer::SetCommanderActiviteFrame(const MG_SyncCommanderTitleRecord& msg)
{
	std::map<int32_t,bool>::const_iterator iter = msg.m_nCommanderTitleRecord.begin();
	for( ; iter!=msg.m_nCommanderTitleRecord.end(); ++iter)
	{
		m_mCommanderFrame[iter->first] = iter->second;
	}

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_SYNC_COMMANDERACTIVITEFRAME);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

int LC_ClientGamePlayer::GetCommanderActiviteFrameCount()
{
	return m_mCommanderFrame.size();
}

int LC_ClientGamePlayer::GetNewCommanderActiviteFrameCount()
{
	int count = 0;
	mem::map<int, bool>::const_iterator iter = m_mCommanderFrame.begin();
	for( ; iter!=m_mCommanderFrame.end(); ++iter)
	{
		if (iter->second)
			++count;
	}
	return count;
}

bool LC_ClientGamePlayer::IsCommanderFrameNew(int index)
{
	mem::map<int, bool>::const_iterator iter = m_mCommanderFrame.find(index);
	if(iter == m_mCommanderFrame.end())
		return false;

	return iter->second;
}

bool LC_ClientGamePlayer::IsCommanderFrameActivite(int index)
{
	mem::map<int, bool>::const_iterator iter = m_mCommanderFrame.find(index);
	return iter != m_mCommanderFrame.end();
}

void LC_ClientGamePlayer::RequestSetCommanderFrameFlag(int index)
{
	MG_REQ_SetCommanderTitleFlag reqMsg;
	reqMsg.m_nTitelID = index;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SETCOMMANDERTITLEFLAG, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::SetCommanderActiviteHead(const MG_SyncProfilePictureRecord& msg)
{
	std::map<int32_t,bool>::const_iterator iter = msg.m_nProfilePictureRecord.begin();
	for( ; iter!=msg.m_nProfilePictureRecord.end(); ++iter)
	{
		m_mCommanderHead[iter->first] = iter->second;
	}

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_SYNC_COMMANDERACTIVITEHEAD);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

int LC_ClientGamePlayer::GetCommanderActiviteHeadCount()
{
	return m_mCommanderHead.size();
}

int LC_ClientGamePlayer::GetNewCommanderActiviteHeadCount()
{
	int count = 0;
	mem::map<int, bool>::const_iterator iter = m_mCommanderHead.begin();
	for( ; iter!=m_mCommanderHead.end(); ++iter)
	{
		if (iter->second)
			++count;
	}
	return count;
}

bool LC_ClientGamePlayer::IsCommanderHeadNew(int index)
{
	mem::map<int, bool>::const_iterator iter = m_mCommanderHead.find(index);
	if(iter == m_mCommanderHead.end())
		return false;

	return iter->second;
}

void LC_ClientGamePlayer::RequestSetCommanderHeadFlag(int index)
{
	MG_REQ_SetProfilePictureFlag reqMsg;
	reqMsg.m_nID = index;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SETPROFILEPICTUREFLAG, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::RequestSetCommanderFrame(int index)
{
	MG_REQ_SetCurrentCommanderTitleID reqMsg;
	reqMsg.m_nCurrentCommanderTitleID = index;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SETCURRENTCOMMANDERTITLEID, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::RequestSetCommanderHead(int index)
{
	MG_REQ_SetProfilePicture reqMsg;
	reqMsg.m_nProfilePictureID = index;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SETPROFILEPICTURE, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::RequestSetBuddyTransform(int itype, int cID, int TransformID)
{
	MG_REQ_BuddyTransformOP reqMsg;
	reqMsg.m_nOpType = itype;
	reqMsg.m_nBuddyID = cID;
	reqMsg.m_nTransformID = TransformID;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_BUDDYTRANSFORMOP, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}
//========================================================================================================
void LC_ClientGamePlayer::RltSyncGamePlayerCompeteReward(const MG_SyncACTActiveRewardInfo& rltMsg)
{
	m_kSuperShopCompete[rltMsg.m_nID] = rltMsg.m_nCount;

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_NOTIFY_SUPERSHOPCOMPETE_REWARD);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void LC_ClientGamePlayer::RltSyncGamePlayerSubReward(const MG_SyncACTActiveRewardInfoB& rltMsg)
{
	m_kSuperShopSub[rltMsg.m_nID] = rltMsg.m_nCount;

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_NOTIFY_SUPERSHOPSUB_REWARD);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

int32_t LC_ClientGamePlayer::GetCompeteRewardCount(int32_t iID)
{
	mem::map<int32_t, int32_t>::iterator iter = m_kSuperShopCompete.find(iID);
	if (iter != m_kSuperShopCompete.end())
	{
		return iter->second;
	}
	else
	{
		return 0;
	}
}

int32_t LC_ClientGamePlayer::GetSubRewardCount(int32_t iID)
{
	mem::map<int32_t, int32_t>::iterator iter = m_kSuperShopSub.find(iID);
	if (iter != m_kSuperShopSub.end())
	{
		return iter->second;
	}

	return 0;
}

void	LC_ClientGamePlayer::RltSyncGamePlayerDrawCardReward(const MG_SyncLotteryItem& rltMsg)
{
	StringStreamType sStream("");
	if (rltMsg.m_nCount <= 0 || rltMsg.m_nCount >= MAX_LOTTERY_ITEM_COUNT)
	{
		return;
	}
	sStream << rltMsg.m_nCategory << "#";
	sStream << int(rltMsg.m_nCount) << "#";
	for (int index = 0; index < rltMsg.m_nCount ; index ++)
	{
		const LotteryItem &pkData = rltMsg.m_Data[index];
		if (pkData.m_nItemId > 0)
		{
			sStream << pkData.m_nItemId << "|" << int(pkData.m_nCount) <<  "|" << int(pkData.m_nAssociation) << "#";
		}
	}

	LC_ClientGameEvent kGameEvent;
	kGameEvent.SetClientEventType(GLET_NOTIFY_DRAWCARD_REWARD);
	
	LC_ClientGameEventUserData_String kUserData(sStream.str());
	kGameEvent.SetUserData(&kUserData);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);


}

StringType LC_ClientGamePlayer::GetDailyActivityString()
{
	if (m_nDailyActivityRewardID.empty())
		return "";

	StringStreamType sStream("");

	sStream << m_nDailyActivityRewardID.size() << "#";
	
	for(int index =0; index < m_nDailyActivityRewardID.size(); index++)
	{
		sStream << m_nDailyActivityRewardID[index] << "#";
	}

	return sStream.str();
	
}

void LC_ClientGamePlayer::RequestGetDailyActivity(int32_t rewardID)
{
	if (rewardID < 0 )
		return ;

	MG_REQ_ReceiveDailyActivityReward reqMsg;
	reqMsg.m_nRewardID = rewardID;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_RECEIVE_DAILY_ACTIVITY_REWARD, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}

}

void LC_ClientGamePlayer::ReqPeakArenaLadderInfo(int32_t index,int32_t size)
{
	MG_PB<msg_define::arena_ladder_info_req> reqMsg;
	reqMsg.m_value.set_type(0);
	reqMsg.m_value.set_index(index);
	reqMsg.m_value.set_size(size);

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_SS_GLOBAL_ARENA_LADDER_REQ, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::ReqDrawArenaSectionReward(int iCode,int32_t iID)
{
	MG_PB<msg_define::arena_opertaion_req> reqMsg;
	reqMsg.m_value.set_type(0);
	reqMsg.m_value.set_op_code(iCode);
	reqMsg.m_value.add_op_params(iID);	

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_ARENA_OPERTAION, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}


StringType LC_ClientGamePlayer::GetAcceptEntrustTasks()
{	
	int64_t ServerTime=GetServerTimeVal();
	StringStreamType sStream("");
	StringStreamType sStream2("");
	for (AcceptTaskVec::iterator iter=m_AccpetedTasks.begin();iter!=m_AccpetedTasks.end() ;++iter)
	{
		const EntrustTask_Base& pInfo=iter->second;
		if (pInfo.m_EndTime==0 || pInfo.m_EndTime<(int)ServerTime)
		{
			sStream<<pInfo.m_Index<<"#";
		}
		else
		{
			sStream2<<pInfo.m_Index<<"#";
		}
		
	}
	sStream<<"|"<<sStream2.str();
	return sStream.str();
}

StringType LC_ClientGamePlayer::GetCanAcceptEntrustTasks()
{
	StringStreamType sStream("");
	for (int i=0;i<m_CanAcceptTasks.size();i++)
	{
		sStream<<m_CanAcceptTasks[i]<<'#';
	}
	return sStream.str();
}

StringType LC_ClientGamePlayer::GetAcceptEntrustTaskInfo(int32_t iEntityId)
{
	StringStreamType sStream("");
	AcceptTaskVec::iterator iter=m_AccpetedTasks.find(iEntityId);
	if (iter!=m_AccpetedTasks.end())
	{
		sStream<<iter->second.m_Id<<"#"<<iter->second.m_EndTime;
	}
	return sStream.str();
}

StringType LC_ClientGamePlayer::GetAcceptEntrustTaskRewards(const std::vector<int32_t>& iEntityIds)
{
	StringStreamType sStream("");
	mem::map<int,int> Rewards;
	for (int i=0;i<iEntityIds.size();i++)
	{
		AcceptTaskVec::iterator iter=m_AccpetedTasks.find(iEntityIds[i]);
		if (iter!=m_AccpetedTasks.end())
		{
			CF_EntrustTask::DataEntry* pkEntry=SafeGetCSVFileDataEntryPtr<CF_EntrustTask>(iter->second.m_Id);
			if (!pkEntry)
			{
				Utility::GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_ERROR,"Wrong EntrustTaskId: %d",iter->second.m_Id);
				continue;
			}
			for (int j=0;j<pkEntry->_kReward.size();j++)
			{
				UT_SIMDataInfo& pInfo=pkEntry->_kReward[j];
				int iItemID=pInfo.GetItemId();
				int iItemNum=pInfo.GetItemNum();
				mem::map<int,int>::iterator it=Rewards.find(iItemID);
				if(it!=Rewards.end())
				{
					it->second+=iItemNum;
				}
				else
				{
					Rewards.insert(std::pair<int,int>(iItemID,iItemNum));
				}
			}
		}
	}
	mem::map<int,int>::iterator it=Rewards.begin();
	while(it!= Rewards.end())
	{
		sStream<<it->first<<":"<<it->second<<";";
		++it;
	}
	return sStream.str();
}

int LC_ClientGamePlayer::GetEntrustTaskRefreshNum()
{
	return m_iEntrustRefreshNum;
}

bool LC_ClientGamePlayer::IsRuneEntrusted(int32_t iRuneId)
{
	return m_iEntrustedRunes.find(iRuneId)!=m_iEntrustedRunes.end();
}

bool LC_ClientGamePlayer::IsBuddyEntrusted(int32_t iBuddyId)
{
	buddyInstanceMap::iterator iter=m_mBuddyInstanceMap.find(iBuddyId);
	if (iter!=m_mBuddyInstanceMap.end())
	{
		return iter->second->GetBeEntrusted();
	}
	return false;
}

bool LC_ClientGamePlayer::HasHighStarLevelTask()
{	
	for (int32_t i=0;i<m_CanAcceptTasks.size();i++)
	{
		CF_EntrustTask::DataEntry* pkEntry=SafeGetCSVFileDataEntryPtr<CF_EntrustTask>(m_CanAcceptTasks[i]);
		if (!pkEntry)
		{
			Utility::GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM,"Wrong EntrustTaskId: %d",m_CanAcceptTasks[i]);
			continue;
		}
		if (pkEntry->_iStar>=4)
		{
			return true;
		}
	}
	return false;
}

void LC_ClientGamePlayer::UpdateEntrustTaskInfo(std::vector<EntrustTask_Base>& AcceptedTasks,std::vector<int32_t>& CanAcceptTasks,int InRefreshTime)
{
	int64_t serverTime=GetServerTimeVal();
	m_AccpetedTasks.clear();
	for (int i=0;i<AcceptedTasks.size();i++)
	{
		const EntrustTask_Base& info=AcceptedTasks[i];
		CF_EntrustTask::DataEntry* pkEntry=SafeGetCSVFileDataEntryPtr<CF_EntrustTask>(info.m_Id);
		if (!pkEntry)
		{
			Utility::GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM,"Wrong EntrustTaskId: %d",info.m_Id);
			continue;
		}
		m_AccpetedTasks[info.m_Index]=info;
		if (info.m_EndTime==0 || info.m_EndTime<serverTime)
		{
			continue;
		}
		if (pkEntry->_iType==1)
		{
			for (int j=0;j<info.m_Vec.size();j++)
			{
				int iBuddyId=info.m_Vec[j];
				m_mBuddyInstanceMap[iBuddyId]->SetBeEntrusted(true);
			}
		}
		else
		{
			for (int j=0;j<info.m_Vec.size();j++)
			{
				int iRuneId=info.m_Vec[j];
				m_iEntrustedRunes.insert(iRuneId);
			}
		}
		
	}
	m_CanAcceptTasks		=	CanAcceptTasks;
	m_iEntrustRefreshNum	=	InRefreshTime;
}

void LC_ClientGamePlayer::UpdateEntrustTaskRefreshNum(int InRefreshTime)
{
	m_iEntrustRefreshNum	=	InRefreshTime;
}

void LC_ClientGamePlayer::OnAcceptEntrustTask(const EntrustTask_Base& AcceptedTask)
{
	//把英雄设为已派遣
	CF_EntrustTask::DataEntry* pkEntry=SafeGetCSVFileDataEntryPtr<CF_EntrustTask>(AcceptedTask.m_Id);
	if (!pkEntry)
	{
		Utility::GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM,"Wrong EntrustTaskId: %d",AcceptedTask.m_Id);
		return;
	}
	m_AccpetedTasks.insert(std::pair<int,EntrustTask_Base>(AcceptedTask.m_Index,AcceptedTask));
	if (pkEntry->_iType==1)
	{
		for (int i=0;i<AcceptedTask.m_Vec.size();i++)
		{
			int iBuddyId=AcceptedTask.m_Vec[i];
			m_mBuddyInstanceMap[iBuddyId]->SetBeEntrusted(true);
		}
	}
	else
	{
		for (int i=0;i<AcceptedTask.m_Vec.size();i++)
		{
			int iRuneId=AcceptedTask.m_Vec[i];
			m_iEntrustedRunes.insert(iRuneId);
		}
	}

	for (std::vector<int32_t>::iterator iter=m_CanAcceptTasks.begin();iter!=m_CanAcceptTasks.end();++iter)
	{
		if ((*iter)==AcceptedTask.m_Id)
		{
			m_CanAcceptTasks.erase(iter);
			break;
		}
	}
}

void LC_ClientGamePlayer::OnFinishedEntrustTask(const EntrustTask_Base& AcceptedTask)
{
	AcceptTaskVec::iterator iter=m_AccpetedTasks.find(AcceptedTask.m_Index);
	if (iter!=m_AccpetedTasks.end())
	{
		EntrustTask_Base& Task=iter->second;
		CF_EntrustTask::DataEntry* pkEntry=SafeGetCSVFileDataEntryPtr<CF_EntrustTask>(AcceptedTask.m_Id);
		if (pkEntry)
		{
			
			if (pkEntry->_iType==1)
			{
				for (int i=0;i<Task.m_Vec.size();i++)
				{
					int iBuddyId=Task.m_Vec[i];
					m_mBuddyInstanceMap[iBuddyId]->SetBeEntrusted(false);
				}
			}
			else
			{
				for (int i=0;i<Task.m_Vec.size();i++)
				{
					int iRuneId=Task.m_Vec[i];
					m_iEntrustedRunes.erase(iRuneId);
				}
				
			}
			Task.m_Id		=	AcceptedTask.m_Id;
			Task.m_EndTime	=	AcceptedTask.m_EndTime;
			Task.m_Vec		=	AcceptedTask.m_Vec;
		}
		else
		{
			Utility::GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM,"Wrong EntrustTaskId: %d",Task.m_Id);
		}
	}
}

void LC_ClientGamePlayer::OnTakeEntrustTaskReward(const std::vector<int32_t>& AcceptedTask)
{
	for (int i=0;i<AcceptedTask.size();i++)
	{
		m_AccpetedTasks.erase(AcceptedTask[i]);
	}
}

void LC_ClientGamePlayer::OnRefreshEntrustTask(const std::vector<int32_t>& AcceptedTask,int InRefreshTime)
{
	m_CanAcceptTasks		=	AcceptedTask;
	m_iEntrustRefreshNum	=	InRefreshTime;
}

void LC_ClientGamePlayer::UpdateEntrustTaskState(int iId, bool bEntrusted)
{
	AcceptTaskVec::iterator iter = m_AccpetedTasks.find(iId);
	if(iter == m_AccpetedTasks.end())
		return;

	const EntrustTask_Base& info = iter->second;
	CF_EntrustTask::DataEntry* pkEntry=SafeGetCSVFileDataEntryPtr<CF_EntrustTask>(info.m_Id);
	if (pkEntry == NULL)
	{
		return;
	}

	if (pkEntry->_iType==1)
	{
		for (size_t i =0; i < info.m_Vec.size(); i++)
		{
			int iBuddyId=info.m_Vec[i];
			m_mBuddyInstanceMap[iBuddyId]->SetBeEntrusted(bEntrusted);
		}
	}
	else
	{
		for (size_t i=0; i < info.m_Vec.size(); i++)
		{
			int iRuneId = info.m_Vec[i];
			if(bEntrusted)
				m_iEntrustedRunes.insert(iRuneId);
			else
				m_iEntrustedRunes.erase(iRuneId);
		}
	}
}


void LC_ClientGamePlayer::ReqRefreshEntrustTask()
{
	MG_CS_UpdateEntrustTask_Req reqMsg;
	reqMsg.type=0;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_CS_UpdateEntrustTask_Req, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::ReqEntrustTask(int iEntityID,const StringType& HeroList)
{
	MG_CS_AccpetEntrustTask_Req reqMsg; 
	reqMsg.m_Id=iEntityID;
	TStringVector kVector = UT_BaseStringUtility::SplitString(HeroList, ':');
	for(int i=0;i<kVector.size();i++)
	{
		reqMsg.m_Vec.push_back(atoi(kVector[i].c_str()));
	}
	
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_CS_AccpetEntrustTask_Req, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::ReqTakeEntrustTaskReward(const StringType& iEntityRwdList)
{
	MG_CS_GetAwardEntrustTask_Req reqMsg;
	TStringVector kVector = UT_BaseStringUtility::SplitString(iEntityRwdList, ':');
	for(int i=0;i<kVector.size();i++)
	{
		reqMsg.m_Indexs.push_back(atoi(kVector[i].c_str()));
	}

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_CS_GetAwardEntrustTask_Req, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::ReqFinishEntrustTaskImmediately(int iEntityID)
{
	MG_CS_SpeedUpEntrustTask_Req reqMsg;
	reqMsg.m_Index=iEntityID;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_CS_SpeedUpEntrustTask_Req, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::SaveDeadTimeForC( int deadTime )
{
	m_lSaveDeadTime = deadTime;
}

int LC_ClientGamePlayer::GetSaveDeadTimeForC()
{
	return m_lSaveDeadTime;
}

void LC_ClientGamePlayer::SetDeadTime( int deadTime )
{
	m_lDeadTime = deadTime;
}
//-------------------------------------------------------------------------------------------
void LC_ClientGamePlayer::UpdateDirtyFlag(int Flag)
{
	switch(Flag)
	{
	case DIRTY_FLAG_ACTIVEBUDDYLIST:
		m_DFBuddyActiveList.Update();
		break;
	case DIRTY_FLAG_INACTIVEBUDDYLIST:
		m_DFBuddyInActiveList.Update();
		break;
	case DIRTY_FLAG_CURRENTINDEX:
		m_DFCurrentIndex.Update();
		break;
	case DIRTY_FLAG_STARVEIN:
		m_DFStarVein.Update();
		break;
	case DIRTY_FLAG_STARVEIN_GOAL:
		m_DFStarVeinGoal.Update();
		break;
	case DIRTY_FLAG_TIMELIMIT_SHOP:
		m_DFShopTimeLimit.Update();
		break;
	case DIRTY_FLAG_BUDDY_SCHEME_LIST:
		m_DFCurrentSchemeList.Update();
		break;
	case DIRTY_FLAG_COMMANDER_PROPERTY:
		m_pkCommander->UpdateDirtyFlag();
		break;
	case DIRTY_FLAG_NPCFUNCTION:
		m_DFNPCFunction.Update();
		break;
	default:
		LC_PlayerBase::UpdateDirtyFlag(Flag);
		break;
	}
}
//--------------------------------------------------------------------------------------------
void LC_ClientGamePlayer::SetDirtyFlag(int Flag, bool bFlag)
{
	switch(Flag)
	{
	case DIRTY_FLAG_ACTIVEBUDDYLIST:
		m_DFBuddyActiveList.SetFlag(bFlag);
		break;
	case DIRTY_FLAG_INACTIVEBUDDYLIST:
		m_DFBuddyInActiveList.SetFlag(bFlag);
		break;
	case DIRTY_FLAG_CURRENTINDEX:
		m_DFCurrentIndex.SetFlag(bFlag);
		break;
	case DIRTY_FLAG_STARVEIN:
		m_DFStarVein.SetFlag(bFlag);
		break;
	case DIRTY_FLAG_STARVEIN_GOAL:
		m_DFStarVeinGoal.SetFlag(bFlag);
		break;
	case DIRTY_FLAG_BUDDY_SCHEME_LIST:
		m_DFCurrentSchemeList.SetFlag(bFlag);
		break;
	case DIRTY_FLAG_COMMANDER_PROPERTY:
		m_pkCommander->SetDirtyFlag(bFlag);
		break;
	case DIRTY_FLAG_NPCFUNCTION:
		m_DFNPCFunction.SetFlag(bFlag);
		break;
	default:
		LC_PlayerBase::SetDirtyFlag(Flag, bFlag);
		break;
	}
}
//-------------------------------------------------------------------------------------------
bool LC_ClientGamePlayer::GetDirtyFlag(int Flag)
{
	switch(Flag)
	{
	case DIRTY_FLAG_ACTIVEBUDDYLIST:
		return m_DFBuddyActiveList.GetFlag();
		break;
	case DIRTY_FLAG_INACTIVEBUDDYLIST:
		return m_DFBuddyInActiveList.GetFlag();
		break;
	case DIRTY_FLAG_CURRENTINDEX:
		return m_DFCurrentIndex.GetFlag();
		break;
	case DIRTY_FLAG_STARVEIN:
		return m_DFStarVein.GetFlag();
	case DIRTY_FLAG_STARVEIN_GOAL:
		return m_DFStarVeinGoal.GetFlag();
	case DIRTY_FLAG_TIMELIMIT_SHOP:
		return m_DFShopTimeLimit.GetFlag();
		break;
	case DIRTY_FLAG_BUDDY_SCHEME_LIST:
		return m_DFCurrentSchemeList.GetFlag();
	case DIRTY_FLAG_COMMANDER_PROPERTY:
		return m_pkCommander->GetDirtyFlag();
	case DIRTY_FLAG_NPCFUNCTION:
		return m_DFNPCFunction.GetFlag();
	default:
		return LC_PlayerBase::GetDirtyFlag(Flag);
		break;
	}
}
//-------------------------------------------------------------------------------------------
void LC_ClientGamePlayer::ClearDirtyFlag()
{
	m_DFBuddyActiveList.Reset();
	m_DFBuddyInActiveList.Reset();
	m_DFCurrentIndex.Reset();
	m_DFStarVein.Reset();
	m_DFStarVeinGoal.Reset();
	m_DFCurrentSchemeList.Reset();
	m_DFNPCFunction.Reset();
	m_pkCommander->ClearDirtyFlag();
	LC_PlayerBase::ClearDirtyFlag();
}
//-------------------------------------------------------------------------------------------
void LC_ClientGamePlayer::SetDirtyFlagRevisionID(int iFlag, int lRevisionID)
{
	switch(iFlag)
	{
	case DIRTY_FLAG_ACTIVEBUDDYLIST:
		m_DFBuddyActiveList.SetRevisionID(lRevisionID);
		break;
	case DIRTY_FLAG_INACTIVEBUDDYLIST:
		m_DFBuddyInActiveList.SetRevisionID(lRevisionID);
		break;
	case DIRTY_FLAG_CURRENTINDEX:
		m_DFCurrentIndex.SetRevisionID(lRevisionID);
		break;
	case DIRTY_FLAG_STARVEIN:
		m_DFStarVein.SetRevisionID(lRevisionID);
		break;
	case DIRTY_FLAG_STARVEIN_GOAL:
		m_DFStarVeinGoal.SetRevisionID(lRevisionID);
		break;
	case DIRTY_FLAG_BUDDY_SCHEME_LIST:
		m_DFCurrentSchemeList.SetRevisionID(lRevisionID);
		break;
	case DIRTY_FLAG_COMMANDER_PROPERTY:
		m_pkCommander->SetDirtyFlagRevisionID(lRevisionID);
		break;
	case DIRTY_FLAG_NPCFUNCTION:
		m_DFNPCFunction.SetRevisionID(lRevisionID);
		break;
	default:
		LC_PlayerBase::SetDirtyFlagRevisionID(iFlag, lRevisionID);
		break;
	}
}
//-------------------------------------------------------------------------------------------
void LC_ClientGamePlayer::ClearDirtyFlagRevisionID()
{
	m_DFBuddyActiveList.ClearRevisionID();
	m_DFBuddyInActiveList.ClearRevisionID();
	m_DFCurrentIndex.ClearRevisionID();
	m_DFStarVein.ClearRevisionID();
	m_DFStarVeinGoal.ClearRevisionID();
	m_DFCurrentSchemeList.ClearRevisionID();
	m_DFNPCFunction.ClearRevisionID();
	LC_PlayerBase::ClearDirtyFlagRevisionID();
}

//*******	三角色系统
//***** active buddy list
bool LC_ClientGamePlayer::CheckTargetActiveBuddyIndex(uint16_t index)
{
	if(index < 0 || index >= MAX_ACTIVE_BUDDY_NUM)
	{
		GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "目标Index无效");
		return false;
	}

	return true;
}

//技能列表 LC_SkillAsset, 物品资源 LC_PackAsset（里面有LC_EquipSlotPack之类的，基本就是全部的背包，仓库之类的物品）, 装扮 LC_FashionBagAsset
//更新出战列表，如果当前操作角色的obid变更，则进行切换角色操作
bool LC_ClientGamePlayer::UpdateActiveBuddyList(MG_SC_ActivateBuddyDataSync& BuddyDataMsg)
{
//get new list and clear old list every time 
	if(CheckTargetActiveBuddyIndex(BuddyDataMsg.m_nCurrentIndex))
	{
		SetCurrentActiveBuddyIndex(BuddyDataMsg.m_nCurrentIndex);
	}
	
	int activateBuddyID = GetActivateBuddyID(BuddyDataMsg);
	m_vActiveBuddyList.clear();
	for(int i=0; i<BuddyDataMsg.m_nCount; ++i)
	{
		AddActiveBuddy(BuddyDataMsg.m_Data[i]);
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI, "[LHX] UpdateActiveBuddyList: ObjectId=%d, TemplateId=%d", BuddyDataMsg.m_Data[i].nObjectId, BuddyDataMsg.m_Data[i].nBuddyTemplateId);
	}
	
	object_id_type curID = BuddyDataMsg.m_Data[m_uiCurrentActiveBuddyIndex].nObjectId;
	UpdateBuddyInstanceList();
//check whether need to swap buddy
//Notify UI
	if(GetID() == curID)
	{
		GfxWriteLog( LOG_TAG_UI, LOG_SWITCH_UI, "[LHX] 不需要交换角色");
		return false;
	}
	else
	{
		SetDirtyFlag(DIRTY_FLAG_CURRENTINDEX, true);
		AddDirtyFlagRevisionID(DIRTY_FLAG_CURRENTINDEX);
		return true;
	}
}

bool LC_ClientGamePlayer::GetActivateBuddyPtr(LC_ActorPtrList& vecPtrList)
{
	LC_ActorBase* pActor = NULL;

	for (int i=0; i<m_vActiveBuddyList.size(); ++i)
	{
		pActor = GetBuddyPlayerBasePtr(m_vActiveBuddyList[i].nObjectId);
		if (pActor)
			vecPtrList.push_back(pActor);
	}

	return true;
}
//--------------------------------------------------------------------------------------------
void LC_ClientGamePlayer::UpdateActiveBuddyRespawnTimeList(const MG_SC_SetBuddyRespawnTimer& msg)
{
	if(msg.m_nIndex < 0 || msg.m_nIndex >= MAX_ACTIVE_BUDDY_NUM)
	{
		GfxWriteLog(LOG_TAG_INFO, LOG_SWITCH_INFO, "[GamePlayer] Error Index! UpdateActiveBuddyRespawnTimeList: %d", msg.m_nIndex);
		return;
	}

	ActiveRespawnCoolDown(msg.m_nIndex, msg.m_fTimer);
}

void LC_ClientGamePlayer::ResetActiveBuddyRespawnTimeList(const MG_SC_ResetSingleBuddyRespawnTimer& msg)
{
	ResetRespawnCoolDown(msg.m_nIndex);
}

void LC_ClientGamePlayer::ResetAllActiveBuddyRespawnTimeList()
{
	for (int i=0; i<MAX_ACTIVE_BUDDY_NUM; ++i)
	{
		ResetRespawnCoolDown(i);
	}
}

//--------------------------------------------------------------------------------------------
void LC_ClientGamePlayer::UpdateInactiveBuddyList(MG_SC_BuddyDataSync& BuddyDataMsg)
{
//get new list and clear old list every time
	m_vInactiveBuddyList.clear();
	for(int i=0; i<BuddyDataMsg.m_nCount; ++i)
	{
		AddInactiveBuddy(BuddyDataMsg.m_Data[i]);
	}
	UpdateBuddyInstanceList();
//Notify UI
	SetDirtyFlag(DIRTY_FLAG_INACTIVEBUDDYLIST, true);
	AddDirtyFlagRevisionID(DIRTY_FLAG_INACTIVEBUDDYLIST);
}
//--------------------------------------------------------------------------------------------
//切换
bool LC_ClientGamePlayer::ReqActivateActiveBuddy(int activeIndex)
{
	if(m_uiCurrentActiveBuddyIndex == activeIndex)
	{
		GfxWriteLog( LOG_INFO_FILELINE, LOG_SWITCH_ERROR, "目标index为当前角色");
		return false;
	}

	if(activeIndex < 0 || activeIndex >= m_vActiveBuddyList.size())
	{
		GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "切换角色index不合法");
		return false;
	}

	if (GetRestSwapCoolDown(activeIndex) > 0)
	{
		SendLogicInfoMessage(RE_BUDDY_ERROR_SWAP_BUDDY_COOLDOWN);
		return false;
	}

	int cID = m_vActiveBuddyList[activeIndex].nBuddyTemplateId;
	object_id_type oID = GetBuddyObjectID(cID);
	LC_PlayerBase* pkPlayer = GetBuddyPlayerBasePtr(oID);

	if (pkPlayer == NULL)
		return false;
	
	if(pkPlayer->GetDead())
	{
		SendLogicInfoMessage(RE_BUDDY_ERROR_SWAP_BUDDY_DEAD);
		return false;
	}
	
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	MG_CS_SetActivateBuddyReq reqMsg;
	reqMsg.m_nIndex = activeIndex;

	bool res = false;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_CS_SET_ACTIVATE_BUDDY_REQ, &reqMsg);
	if (bCreateMsg)
	{
		this->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
		res = true;
	}

	//添加index到pending中，用于刷新切换计时器
	m_iPendingSwapIndex = m_uiCurrentActiveBuddyIndex;

	return res;
}
//--------------------------------------------------------------------------------------------
//出战 -- 发送出战请求
bool LC_ClientGamePlayer::ReqActivateInactiveBuddy(int activeIndex, int buddyID)
{
//Check operation valid
	if(activeIndex < 0 || activeIndex >= MAX_ACTIVE_BUDDY_NUM)
	{
		GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "出战角色index不合法");
		return false;
	}

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	MG_CS_BindBuddyInstanceReq reqMsg;
	reqMsg.m_nIndex = activeIndex;
	reqMsg.m_nBuddyID = buddyID;
	
	SetPendingActivateCID(buddyID);

	bool res = false;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_CS_BIND_BUDDY_INSTANCE_REQ, &reqMsg);
	if (bCreateMsg)
	{
		this->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
		res = true;
	}

	return res;
}
//-----------------------------------------------------------------------------------------------------
//解锁伙伴
void LC_ClientGamePlayer::ReqUnlockBuddy(int cID)
{
	MG_CS_BuyBuddyReq reqMsg;
	reqMsg.m_nTemplateID = cID;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_CS_BUY_BUDDY_REQ, &reqMsg);
	if (bCreateMsg)
		this->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}
//-----------------------------------------------------------------------------------------------------
//复活伙伴
bool LC_ClientGamePlayer::ReqRespawnBuddy(int index)
{
	bool res = false;
	if(index < 0 || index >= m_vActiveBuddyList.size())
	{
		GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "复活角色index不合法");
		return res;
	}
	
	MG_CS_ReliveBuddyReq reqMsg;
	reqMsg.m_nIndex = index;
	
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_CS_RELIVE_BUDDY_REQ, &reqMsg);
	if (bCreateMsg)
	{
		this->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
		res = true;
	}

	return res;
}//--复活伙伴

bool LC_ClientGamePlayer::IsBuddyDead(int index)
{
	if (index < 0 || index >= m_vActiveBuddyList.size())
	{
		return false;
	}

	object_id_type oID = m_vActiveBuddyList[index].nObjectId;
	LC_PlayerBase* playerPtr = GetBuddyPlayerBasePtr(oID);

	return playerPtr != NULL && playerPtr->GetDead();
}

//**************************************************************************************
//交换角色逻辑
//在ClientPlayerManager中 保存着当前角色的obid1 和 角色指针1 (ClientPlayer)
//在NetPlayerManager中 保存着目标角色的obid2 和 角色指针2 (NetPlayer)
//交换方式:
//在ClientPlayerManager中 获得 角色指针1， 删除键值对应， 添加obid2 对应 角色指针1
//在NetPlayerManager中 获得 角色指针2， 删除键值对应， 添加obid1 对应 角色指针2
//在map和 sector中 交换 Obid 与 角色指针的对应 (角色与随从一定在同一地图，但不一定在同一个sector)
//更新在session中保存的当前角色objid （具体查询 HL_FromGameServer_LoginProtocol.inl)
//交换角色指针中 playerBase对应的数据部分

void LC_ClientGamePlayer::SwapBuddyData(int activeIndex)
{
	GfxWriteLog( LOG_TAG_WARNING, LOG_SWITCH_WARNING, "[GamePlayer::SwapBuddyData]Swap, begin, %d", activeIndex);

	if(activeIndex < 0 || activeIndex >= m_vActiveBuddyList.size())
	{
		GfxWriteLog( LOG_TAG_ERROR, LOG_SWITCH_ERROR, "[GamePlayer][Swap] invalid swap index");
		return;
	}

	LC_ClientGamePlayerManager* clientGamePlayerManager = CLIENT_GET_GAMEPLAYER_MANAGER();
	LC_ClientNetPlayerManager* clientNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();

	object_id_type gamePlayerID = GetID();
	object_id_type netPlayerID = GetActiveBuddy(activeIndex)->nObjectId;

	GfxWriteLog(LOG_TAG_INFO, LOG_SWITCH_INFO, "[GamePlayer][Swap] try swap buddy data, gp's ID:%d, np's ID:%d", gamePlayerID, netPlayerID);

	if (netPlayerID == GetID())
	{
		GfxWriteLog( LOG_TAG_GAME_LOG, LOG_SWITCH_GAME, "[GamePlayer][Swap] same swap id, ignore swap");
		return;
	}

	LC_PlayerBase* pNetPlayer = clientNetPlayerManager->FindPlayer(netPlayerID);
	if(pNetPlayer == NULL)
	{
		GfxWriteLog( LOG_TAG_ERROR, LOG_SWITCH_ERROR, "[GamePlayer][Swap] cant find netplayer, gp's ID:%d, np's ID:%d", gamePlayerID, netPlayerID);
		return;
	}

	LC_MapBase* pMap = GetMap();
	LC_MapBase* pNpMap = pNetPlayer->GetMap();
	if (pMap == NULL || pMap != pNpMap)
	{
		GfxWriteLog( LOG_TAG_ERROR, LOG_SWITCH_ERROR, "[GamePlayer][Swap] invalid map, ignore swap, pMap=0x%08x, pNpMap=0x%08x", pMap, pNpMap);
		return;
	}

	unique_id_type netPlayerUID = pNetPlayer->GetInstance().data();
	unique_id_type gamePlayerUID = GetInstance().data();

//reset controller ID，必须在 clientGamePlayerManager->SwapIDPtr 之前，有关联性，LC_PlayerManagerBase::m_kPlayerIDPtrMap
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	pkLogicManager->SetInputControlActorID(0, true);	// clear old

//swap ptrs in player manager
	if(!clientNetPlayerManager->SwapIDPtr(netPlayerID, gamePlayerID))// || !clientNetPlayerManager->SwapUIDPtr(netPlayerUID, gamePlayerUID))
	{
		GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "[GamePlayer][Swap] NetPlayerManager swap error");
	}

	if(!clientGamePlayerManager->SwapIDPtr(gamePlayerID, netPlayerID))// || !clientGamePlayerManager->SwapUIDPtr(gamePlayerUID, netPlayerUID))
	{
		GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "[GamePlayer][Swap] GamePlayerManager swap error");
	}
	/// 实际上rollback可能没用，只是客户端不报错而已，服务器那边已经切换了角色，前后端已经不一致，可能导致各种错
	/// 所以最好是直接崩掉，如果崩掉处理，就不需要什么rollback了，代码更简洁
	/// 包括之前的一堆检查代码，也应该在失败的情况下直接崩掉

//hide ground fx
	HideGroundFx();

//remove from map
	pMap->RemoveObject(this);
	pMap->RemoveObject(pNetPlayer);

//swap base class data
	SwapBaseClassData(this, pNetPlayer);

//add into map
	pMap->AddObject(this);
	pMap->AddObject(pNetPlayer);

//reset controller ID
	pkLogicManager->SetOperationPlayerID(netPlayerID);
	pkLogicManager->SetInputControlActorID(netPlayerID, true);
	pkLogicManager->SetOperationCharID(netPlayerUID);
	
//swap transformID
	int iTempTransform = GetTransformID();
	SetTransformID(pNetPlayer->GetTransformID());
	if(iTempTransform > 0)
		pNetPlayer->SetTransformID(iTempTransform);

//swap PassiveMoveSpeed
	int PassiveMoveSpeed = GetPassiveMoveSpeed();
	SetPassiveMoveSpeed(pNetPlayer->GetPassiveMoveSpeed());
	pNetPlayer->SetPassiveMoveSpeed(PassiveMoveSpeed);

//swap other base data
	SwapOtherBaseData(this, pNetPlayer);
	if(!m_controlIDVec.empty())
	{
		ResetContronlID();
		AddControlID(netPlayerID);
	}
	SetAllOwner(GetCharType(), this);

//attach swap effects
	AttachEffects(this, pNetPlayer);

//refresh eye avatar
	ClearEyeAvatar();

//refresh vehicle
	m_lastVehicleCharType = 0;
	pNetPlayer->DestoryRiderVisibleData();
	static_cast<LC_ClientNetPlayer*>(pNetPlayer)->SetLastVehicle(0);

	GD_ClientActorModel* pkGamePlayerVisibleData = IM_Manager::GetSingleton().GetActor(gamePlayerID);
	GD_ClientActorModel* pkNetPlayerVisibleData = IM_Manager::GetSingleton().GetActor(netPlayerID);
	if(pkGamePlayerVisibleData != NULL)
	{
		pkGamePlayerVisibleData->SetCanPlaySound(false);
		_clearAreaEffectInfo(*pkGamePlayerVisibleData);
	}
	if (pkNetPlayerVisibleData != NULL)
	{
		pkNetPlayerVisibleData->SetCanPlaySound(true);
		_initAreaEffectInfo(*pkNetPlayerVisibleData, false);
	}

	GfxWriteLog( LOG_TAG_WARNING, LOG_SWITCH_WARNING, "[GamePlayer::SwapBuddyData]Swap, end, %d", activeIndex);
} // End of SwapBuddyData



void LC_ClientGamePlayer::SwapBaseClassData(LC_PlayerBase* pSrc, LC_PlayerBase* pDst)
{
	LC_PlayerBase temp;
	temp = *pSrc;
	*pSrc = *pDst;
	*pDst = temp;
}

void LC_ClientGamePlayer::ChangeID(object_id_type iNewObjectID)
{
	object_id_type gamePlayerID = GetID();
	if(gamePlayerID == iNewObjectID)
		return;

	LC_MapBase* pMap = GetMap();
	if (pMap == NULL)
	{
		GfxWriteLog( LOG_TAG_ERROR, LOG_SWITCH_ERROR, "[GamePlayer][ChangeID] invalid map, ignore swap, pMap=0x%08x", pMap);
		return;
	}

	LC_PlayerManagerBase* pkGamePlayerManager = CLIENT_GET_GAMEPLAYER_MANAGER();
	if(!pkGamePlayerManager->SwapIDPtr(gamePlayerID, iNewObjectID))
	{
		GfxWriteLog( LOG_TAG_ERROR, LOG_TAG_ERROR, "[GamePlayer][ChangeID] GamePlayerManager swap error");
		return;
	}

	pMap->RemoveObject(this);
	SetID(iNewObjectID);
	pMap->AddObject(this);

	IM_Manager* pkImMgr = IM_Manager::GetSingletonPtr();
	pkImMgr->ModifyPlayerModeID(gamePlayerID, iNewObjectID);
	pkImMgr->ModifyRiderModeID(gamePlayerID, iNewObjectID);

	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	pkLogicManager->SetOperationPlayerID(iNewObjectID);
	pkLogicManager->SetInputControlActorID(iNewObjectID, false);

	SendMessage( CreateMsg_UE_SHOW_PLAYER_NAME(GetID()) );
	SendMessage( CreateMsg_UE_SHOW_PLAYER_BLOOD(GetID()) );
}

//*********************************************
void LC_ClientGamePlayer::ResetCamera()
{
	CA_CameraManager::GetSingleton().ResetAllCamera();
	//CA_CameraManager::GetSingleton().ResetPlayerCamera();
}

//*********************************************
bool LC_ClientGamePlayer::IsAllBuddyDead()
{
	for (int i=0; i< m_vActiveBuddyList.size(); ++i)
	{
		LC_PlayerBase* pkPlayer = GetBuddyPlayerBasePtr(m_vActiveBuddyList[i].nObjectId);
		if (pkPlayer && !pkPlayer->GetDead())
			return false;
	}
	return true;
}

bool LC_ClientGamePlayer::ReqDeactivateBuddy(int activeIndex)
{
	bool bRes = false;

	if (activeIndex < 0 || activeIndex >= m_vActiveBuddyList.size())
	{
		return bRes;
	}
	
	MG_CS_UnBindBuddyInstanceReq reqMsg;
	reqMsg.m_nIndex = activeIndex;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_CS_UNBIND_BUDDY_INSTANCE_REQ, &reqMsg);
	if (bCreateMsg)
	{
		this->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
		bRes = true;
	}

	return bRes;
}

//*********************************************
bool LC_ClientGamePlayer::IsBuddy(uint32_t id)
{
	bool res = false;

	if (id <= INVALID_ACTOR_ID)
	{
		return res;
	}

	if (id == GetID())
		return true;

	for(int i = 0; i<m_vActiveBuddyList.size(); ++i)
	{
		if(m_vActiveBuddyList[i].nObjectId == id)
		{
			res = true;
			break;
		}
	}

	return res;
}

bool LC_ClientGamePlayer::IsNpcBelongToBuddy(LC_ActorBase* npcPtr)
{
	if (NULL == npcPtr)
	{
		return false;
	}

	object_id_type summonerID = npcPtr->GetSummonOwnerID();

	return IsBuddy(summonerID);
}

//**********************************************
GameLogic::LC_PlayerBase* LC_ClientGamePlayer::GetBuddyPlayerBasePtr(object_id_type id)
{
	LC_PlayerBase* playerPtr = NULL;

	if (id <= INVALID_ACTOR_ID)
	{
		return playerPtr;
	}

	if(id == GetID())
	{
		playerPtr = this;
	}
	else
	{
		LC_ClientNetPlayerManager* clientNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
		playerPtr = clientNetPlayerManager->FindPlayer(id);
	}

	return playerPtr;
}

//**********************************************
GameLogic::LC_ClientBuddyInstance* LC_ClientGamePlayer::GetBuddyInstance(int cID)
{
	m_mBuddyInstanceMapItr iter = m_mBuddyInstanceMap.find(cID);
	if (iter != m_mBuddyInstanceMap.end())
		return iter->second;
	return NULL;
}

//**********************************************
LC_ClientBuddyInstance* LC_ClientGamePlayer::CreateNewBuddyInstance(int cID)
{
	LC_ClientBuddyInstance* ptr = NULL;
	m_mBuddyInstanceMapItr iter = m_mBuddyInstanceMap.find(cID);
	if (iter == m_mBuddyInstanceMap.end())
	{
		ptr = T_NEW_D LC_ClientBuddyInstance;
		ptr->SetCID(cID);
		ptr->InitAttributeMap();
		m_mBuddyInstanceMap[cID] = ptr;
	}
	else
	{
		ptr = iter->second;
	}

	return ptr;
}

int LC_ClientGamePlayer::GetEquipSlotLevel(int icID, int iSlotID)
{
	LC_ClientBuddyInstance *pkInst = GetBuddyInstance(icID);
	if(pkInst == NULL)
		return 0;
	
	return pkInst->GetEquipIntensifyLevel(iSlotID);
}

int LC_ClientGamePlayer::GetEquipSlotStar(int icID, int iSlotID)
{
	LC_ClientBuddyInstance *pkInst = GetBuddyInstance(icID);
	if(pkInst == NULL)
		return 0;

	return pkInst->GetEquipRefineLevel(iSlotID);
}

void LC_ClientGamePlayer::RequestFireTimer(int iTimerID)
{
	MG_CS_FireTimer reqMsg;
	reqMsg.m_nTimerId = iTimerID;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_CS_FIRE_TIMER, &reqMsg);
	if (bCreateMsg)
		SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

//**********************************************
void LC_ClientGamePlayer::SyncRuneRecordInfo(const MG_SyncRuneRecord& msg)
{
	for(std::map<int32_t,bool>::const_iterator iter = msg.m_nRuneRecord.begin(); iter!=msg.m_nRuneRecord.end(); ++ iter)
	{
		m_kRuneRecordMap[iter->first] = iter->second;
	}
}

void LC_ClientGamePlayer::SetRuneRecord(int iID, bool bDrawed)
{
	m_kRuneRecordMap[iID] = bDrawed;
}

bool LC_ClientGamePlayer::GetRuneRewardDrawed(int iID)
{
	mem::map<int,bool>::iterator iter = m_kRuneRecordMap.find(iID);
	if(iter == m_kRuneRecordMap.end())
		return false;

	return iter->second;
}

bool LC_ClientGamePlayer::GetRuneRewardPrompt()
{
	mem::map<int,bool>::iterator iter = m_kRuneRecordMap.begin();
	for(; iter!=m_kRuneRecordMap.end(); ++iter)
	{
		if(!iter->second)
			return true;
	}

	return false;
}

bool LC_ClientGamePlayer::GetRuneRecord(int iID)
{
	mem::map<int,bool>::iterator iter = m_kRuneRecordMap.find(iID);
	return iter != m_kRuneRecordMap.end();
}

bool LC_ClientGamePlayer::UpdateRuneBuddySlotDressState(int iBuddyId, int iSlot, bool bOpen)
{
	mem::map<int, bool>& kSlotInfo = m_kRuneBuddySlotDressStateMap[iBuddyId];
	const bool bOld = kSlotInfo[iSlot];

	kSlotInfo[iSlot] = bOpen;
	return bOpen && !bOld;
}

bool LC_ClientGamePlayer::GetRuneBuddySlotDressState(int iCid, int iIndex)
{
	RuneBuddySlotStatMap::const_iterator iter = m_kRuneBuddySlotDressStateMap.find(iCid);
	if(iter == m_kRuneBuddySlotDressStateMap.end())
		return false;

	const mem::map<int, bool>& kInfo = iter->second;
	mem::map<int, bool>::const_iterator iter2 = kInfo.find(iIndex);
	if(iter2 == kInfo.end())
		return false;

	return iter2->second;
}

//**********************************************
void LC_ClientGamePlayer::AttachEffectToActivateBuddy(int oID)
{
	GD_ClientActorModel* pkVisibleData = IM_Manager::GetSingleton().GetActor(oID);
	if(pkVisibleData)
	{
		CF_Buddy::DataEntry *pkBuddyData = SafeGetCSVFileDataEntryPtr<CF_Buddy>(pkVisibleData->GetCharType());
		if(pkBuddyData != NULL)
		{
			int iSoundID = g_soundMgr.Play3DEx(UT_EngineMath::ConvertVec3IntToNiPoint3(GetCurrentLocation()), pkBuddyData->_sEnterSound, AS_ClientSoundManager::SCT_PLAYER_SKILL);
			ResetVoice(iSoundID);
		}

		pkVisibleData->AttachFx(GD_ClientActorModel::FDT_GROUND, 0, false, RES_FX_ACTIVATE_BUDDY_EFFECT);
		SetActivateBuddyFlag(false);
	}
}

//**********************************************
int LC_ClientGamePlayer::GetActivateBuddyID(MG_SC_ActivateBuddyDataSync& BuddyDataMsg)
{
	if (m_vActiveBuddyList.size() == 0)
		return -1;

	if (BuddyDataMsg.m_nCount > m_vActiveBuddyList.size())
		return BuddyDataMsg.m_Data[BuddyDataMsg.m_nCount-1].nBuddyTemplateId;

	for (int i=0; i<BuddyDataMsg.m_nCount; ++i)
	{
		if (BuddyDataMsg.m_Data[i].nBuddyTemplateId != m_vActiveBuddyList[i].nBuddyTemplateId)
			return BuddyDataMsg.m_Data[i].nBuddyTemplateId;
	}
	
	return -1;
}

//*********************************************************
int LC_ClientGamePlayer::GetSkillUnlockLevel(int cID, skill_id_type skillID)
{
	LC_SkillAsset* pSkillAsset = GetBuddySkillAsset(cID);
	if (pSkillAsset == NULL)
		return -1;

	bool bLearned = pSkillAsset->IsLearnedSkill(skillID);
	if (bLearned)
	{
		return -1;
	}
	else
	{
		CF_Buddy::DataEntry* pkData = CF_Buddy::GetSingleton().GetEntryPtr(cID);
		if (pkData != NULL)
		{
			const StringType & sValue = pkData->_skillid;
			TStringVector kStringVector = UT_BaseStringUtility::SplitString(sValue, ';');
			for (int i = 0; i < kStringVector.size(); ++i)
			{
				TStringVector kVector = UT_BaseStringUtility::SplitString(kStringVector[i], ':');
				if (atoi(kVector[0].c_str()) == skillID)
					return atoi(kVector[1].c_str());
			}
		}
		return -1;
	}
};

void LC_ClientGamePlayer::ClearEyeAvatar()
{
	LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
	if (NULL==pkNPCManager)
		return;
	if (0 != m_iEyeNPCID)
		pkNPCManager->DestroyEyeNPC(m_iEyeNPCID);
	_clearEyeAvatar();
}

int LC_ClientGamePlayer::GetBuddyIndex(object_id_type objID)
{
	int res = -1;

	for (int i=0; i<m_vActiveBuddyList.size(); ++i)
	{
		if (m_vActiveBuddyList[i].nObjectId == objID)
		{
			res = i;
			break;
		}
	}

	return res;
}

//**********************************************
//UI交互
int LC_ClientGamePlayer::GetBuddyLevel(int cID)
{
	m_mBuddyInstanceMapItr iter = m_mBuddyInstanceMap.find(cID);
	if(iter != m_mBuddyInstanceMap.end())
		return iter->second->GetLevel();
	return 0;
}
//**********************************************
int LC_ClientGamePlayer::GetBuddyAllEquipReinforceLevel(int cID)
{
	int level = 0;
	for(size_t i=0; i<GetGlobalSetting.ArtifactPackSize; i++)
	{
		level = level + GetEquipSlotLevel(cID, i);
	}
	return level;
}
//**********************************************
bool LC_ClientGamePlayer::GetBuddyUnlock(int cID)
{
	bool bActive =  !IsBuddyInstance(cID);
	bool bunLock = true;
	if (!bActive)
	{
		int index = -1;
		for (int i=0; i<GetTotalBuddyNum(); i++)
		{
			if (GetBuddyInactiveCID(i) == cID)
			{
				index = i;
				break;
			}
		}
		if (index < 0)
		{
			return false;
		}
		bunLock = GetBuddyInactiveUnlock(index);
	}
	return bunLock;
}
//**********************************************
int LC_ClientGamePlayer::GetBuddyEXP(int cID)
{
	m_mBuddyInstanceMapItr iter = m_mBuddyInstanceMap.find(cID);
	if(iter != m_mBuddyInstanceMap.end())
		return iter->second->GetEXP();
	return 0;
}

//**********************************************
combat_score_type LC_ClientGamePlayer::GetBuddyCombateScore(int cID)
{
	m_mBuddyInstanceMapItr iter = m_mBuddyInstanceMap.find(cID);
	if(iter != m_mBuddyInstanceMap.end())
		return iter->second->GetCombateScore();
	return 0;
}

int LC_ClientGamePlayer::GetCurrentTransform(int cID)
{
	int iTransformID = 0;
	m_mBuddyInstanceMapItr iter = m_mBuddyInstanceMap.find(cID);
	if(iter != m_mBuddyInstanceMap.end())
		iTransformID = iter->second->GetCurrentTransformID();

	if(iTransformID == 0)
	{
		CF_Buddy::DataEntry *pkBuddyData = SafeGetCSVFileDataEntryPtr<CF_Buddy>(cID);
		if(pkBuddyData != NULL)
		{
			iTransformID = pkBuddyData->_iDefaultTransform;
		}
	}
	
	return iTransformID;
}

bool LC_ClientGamePlayer::IsBuddyTransformActive(int cID, int itransformID)
{
	m_mBuddyInstanceMapItr iter = m_mBuddyInstanceMap.find(cID);
	if(iter != m_mBuddyInstanceMap.end())
		return iter->second->GetIsActiveTransform(itransformID);
	return false;
}

int32_t LC_ClientGamePlayer::GetTransformInvaildTime(int cID, int itransformID)
{
	m_mBuddyInstanceMapItr iter = m_mBuddyInstanceMap.find(cID);
	if(iter != m_mBuddyInstanceMap.end())
		return iter->second->GetActiveTransformTime(itransformID);
	return 0;
}

combat_score_type LC_ClientGamePlayer::GetActiveBuddyAllScore()
{
	combat_score_type iScore = 0;
	for (int i=0; i<m_vActiveBuddyList.size(); ++i)
	{
		int iCid = m_vActiveBuddyList[i].nBuddyTemplateId;
		if(iCid > 0)
			iScore += GetBuddyCombateScore(iCid);
	}

	return iScore;
}
//**********************************************
int LC_ClientGamePlayer::GetBuddyStarLevel(int cID)
{
	m_mBuddyInstanceMapItr iter = m_mBuddyInstanceMap.find(cID);
	if(iter != m_mBuddyInstanceMap.end())
		return iter->second->GetStarLevel();
	return 0;
}
//**********************************************
long LC_ClientGamePlayer::GetBuddyUpgradeStarCost(int cID, bool bLast)
{
	int starLevel = GetBuddyStarLevel(cID);
	int index = cID * 100 + starLevel;
	index = bLast ? index - 1 : index;
	CF_BuddyUpgrade::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_BuddyUpgrade>(index);
	if (pkEntry != NULL)
		return pkEntry->_needgold;
	return 0;
}
//**********************************************
int LC_ClientGamePlayer::GetBuddyUpgradeItemID(int cID, bool bLast)
{
	int starLevel = GetBuddyStarLevel(cID);
	int index = cID * 100 + starLevel;
	index = bLast ? index - 1 : index;
	CF_BuddyUpgrade::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_BuddyUpgrade>(index);
	if (pkEntry != NULL)
	{
		TStringVector kVector = UT_BaseStringUtility::SplitString(pkEntry->_needitem, ':');
		if(kVector.size() != 2)
			return 0;

		return String2Int(kVector[0]);
	}
	return 0;
}

int LC_ClientGamePlayer::GetBuddyUpgradeQuality(int cID, int iStarLevel)
{
	int index = cID * 100 + iStarLevel;
	CF_BuddyUpgrade::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_BuddyUpgrade>(index);
	if(pkEntry != NULL)
	{
		return pkEntry->_quality;
	}

	return 0;
}

long LC_ClientGamePlayer::GetBuddyUpgradeUnlockskill(int cID, int iStarLevel)
{
	int index = cID * 100 + iStarLevel;
	CF_BuddyUpgrade::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_BuddyUpgrade>(index);
	if(pkEntry != NULL)
	{
		return pkEntry->_unlockskill;
	}

	return 0;
}

bool LC_ClientGamePlayer::ReqTakeVipNewReward(int iLevel)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	MG_REQ_AddReward reqMsg;
	
	reqMsg.m_nVIPLevel=iLevel;

	bool res = false;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_ADDREWARD, &reqMsg);
	if (bCreateMsg)
	{
		this->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
		res = true;
	}
	return res;
}

int32_t LC_ClientGamePlayer::GetVipNewLevel()
{
	//目前最大是25级，如果策划改表，这里也得修改
	if(m_iCurVipNewLevel<0||m_iCurVipNewLevel>25)
	{
		return 0;
	}
	return m_iCurVipNewLevel;
}

bool LC_ClientGamePlayer::IsVipNewRewardToken(int iLevel)
{
	for(int i=0;i<m_VipNewRewardRecord.size();i++)
	{
		if(m_VipNewRewardRecord[i]==iLevel)
		{
			return true;
		}
	}
	return false;
}

bool LC_ClientGamePlayer::IsVipNewOpen()
{
	return m_bVipNewOpen;
}

bool LC_ClientGamePlayer::HasNottakenVipNewReward()
{
	for(int i=0;i<m_iCurVipNewLevel;i++)
	{
		if(!IsVipNewRewardToken(i))
		{
			return true;
		}
	}
	return false;
}

int LC_ClientGamePlayer::GetVipNewTaskState(int iTaskID)
{
	mem::map<int,int>::iterator it= m_kTaskState.find(iTaskID);
	if(it!= m_kTaskState.end())
	{
		return it->second;
	}
	return -1;
}

void LC_ClientGamePlayer::UpdateVipNewRewardRecord(const std::vector<int32_t>& vecTokenLevels)
{
	m_VipNewRewardRecord.clear();
	m_VipNewRewardRecord.reserve(vecTokenLevels.size());
	for(int i=0;i<vecTokenLevels.size();i++)
	{
		m_VipNewRewardRecord.push_back((int)vecTokenLevels[i]);
	}
}

void LC_ClientGamePlayer::UpdateVipNewRewardRecord(int32_t iTokenLevel)
{
	for(int i=0;i<m_VipNewRewardRecord.size();i++)
	{
		if(m_VipNewRewardRecord[i]==(int)iTokenLevel)
		{
			return ;
		}
	}
	m_VipNewRewardRecord.push_back(iTokenLevel);
}

bool LC_ClientGamePlayer::UpdateVipNewLevel(int32_t iLevel)
{
	bool bLevelUp=iLevel>m_iCurVipNewLevel;
	m_iCurVipNewLevel=iLevel;
	return bLevelUp;
}

void LC_ClientGamePlayer::UpdateVipNewOpen(bool bValue)
{
	m_bVipNewOpen=bValue;
}

void LC_ClientGamePlayer::ReqAcceptVipNewLevelTasks(int32_t iLevel)
{
	CF_VipNew::DataEntry *pkEntry  = SafeGetCSVFileDataEntryPtr<CF_VipNew>(iLevel);
	if (pkEntry)
	{
		mem::vector<UT_SIMDataInfo>::iterator it = pkEntry->_sUpgradeTask.begin();
		while (it != pkEntry->_sUpgradeTask.end())
		{
			ReqAcceptTask(it->GetTaskID());
			++it;
		}
	}
}

void LC_ClientGamePlayer::UpdateNearbyPlayer(const std::vector<PS_NearbyPlayerInfo>& pInfo)
{
	int iVecSize=pInfo.size();
	m_VecNearbyPlayers.clear();
	m_VecNearbyPlayers.reserve(iVecSize);
	for (int i=0;i<iVecSize;i++)
	{
		m_VecNearbyPlayers.push_back(pInfo[i]);
	}
	AddDirtyFlagRevisionID(DIRTY_FLAG_NEARBY_PLAYERINFO);
}

StringType LC_ClientGamePlayer::GetNearbyPlayerInfo()
{
	StringStreamType sStream("");
	for (int i=0;i<m_VecNearbyPlayers.size();i++)
	{
		const PS_NearbyPlayerInfo& pInfo=m_VecNearbyPlayers[i];

		sStream<<pInfo.m_nCharID<<"|"
			<<pInfo.m_nCharName <<"|"
			<<pInfo.m_nControllerLevel <<"|"
			<<pInfo.m_nGroupCount <<"|"
			<<pInfo.m_nMaxGroupCount <<"|"
			<<pInfo.m_nGuildID <<"|"
			<<pInfo.m_nGuildName <<"|"
			<<pInfo.m_nGroupID <<"|"
			<<pInfo.m_nAvtivityBuddyCombatScore;
		sStream<<"#";
	}
	return sStream.str();
}

//**********************************************
void LC_ClientGamePlayer::CommanderLevelUp(uint16_t val)
{
	LC_ClientGamePlayerCommander* commander = GetCommander();
	if (commander->m_iLevel != val)
	{
		commander->m_iLevel = val;
		UpdateDirtyFlag(DIRTY_FLAG_PROPERTY);
	}
	SendLogicInfoMessage(RE_LEVEL_UP_SUCCESS, val);

	GD_ClientActorModel* pkVisibleData = IM_Manager::GetSingleton().GetActor(GetID());
	if(pkVisibleData)
		pkVisibleData->AttachFx(GD_ClientActorModel::FDT_GROUND, 0, false, RES_FX_LEVEL_UP);

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_PLAYER_LEVEL_UP);
	kGameEvent.SetEventParam(EP_LEVEL_UP_PLAYER_ID, GetID());
	kGameEvent.SetEventParam(EP_LEVEL_UP_VALUE, val);
	kGameEvent.SetEventParam(EP_LEVEL_UP_EXP, commander->m_iEXP);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);

	//ToCheck: 刷新任务
	//	LC_TaskMap* pkTaskMap = pkGamePlayer->GetTaskMap();
	//	if (pkTaskMap == NULL)
	//		return;
	//	LC_ClientTaskMap* pkClientTaskMap = (LC_ClientTaskMap*)pkTaskMap;
	//	pkClientTaskMap->UpdateCanAcceptTask();
	//	pkGamePlayer->UpdateNearNPCTaskState();

	//	g_soundMgr.Play2DEx(VARIABLE2STRING(SE_Sys_Player_Upgrade));
}

void LC_ClientGamePlayer::CommanderSetTotalCombateScore(int64_t val)
{
	LC_ClientGamePlayerCommander* commander = GetCommander();
	if (commander->m_iTotalCombateScore != val)
	{
		commander->m_iTotalCombateScore = val;
		UpdateDirtyFlag(DIRTY_FLAG_PROPERTY);
	}
}

//*****************************************************
void LC_ClientGamePlayer::CommanderSetEXP(int64_t val)
{
	LC_ClientGamePlayerCommander* commander = GetCommander();

	int64_t lDeltaExp = val - GetCommanderEXP();
	if (lDeltaExp > 0 && commander->m_iEXP >= 0)
	{
		SendMessage( CreateMsg_UE_DELTA_EXP( GetID(), lDeltaExp) );
		//SendLogicInfoMessage(RE_EXP_ADD, lDeltaExp);
	}

	commander->m_iEXP = val;
	UpdateDirtyFlag(DIRTY_FLAG_PROPERTY);

	if (lDeltaExp != 0)
	{
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_EXP_CHANGED);
		kGameEvent.SetEventParam(EP_EXP_CHANGED_EXP_VALUE,  val);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_3, lDeltaExp);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
	
	//CommanderCheckLevelUp();
}
//***************************************************
//void LC_ClientGamePlayer::CommanderCheckLevelUp()
//{
//	//检查是否可以升级
//	LC_ClientGamePlayerCommander* commander = GetCommander();
//
//	CF_LevelUp* pkCSVLevelUp = CF_LevelUp::GetSingletonPtr();
//	int iMaxLevel = pkCSVLevelUp->GetEntryCount();
//
//	MG_PACKET_ENCODE_HANDLER hMsgHandler;
//	bool bCreateMsg = false;
//	if(commander->m_iLevel < iMaxLevel)
//	{
//		const CF_LevelUp::DataEntry* pkData = pkCSVLevelUp->GetEntryPtr(commander->m_iLevel + 1);
//		if(pkData && commander->m_iEXP >= pkData->_lEXPNeed)
//		{
//			MG_REQ_LevelUp reqMsg;
//			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_LEVEL_UP, &reqMsg);
//			if(bCreateMsg)
//				m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
//		}
//	}
//}

void LC_ClientGamePlayer::UpdateChainCombatScore(int64_t val)
{
	if(m_pkCommander->m_iChainCombateScore != val)
	{
		m_pkCommander->m_iChainCombateScore = val;
		m_pkCommander->UpdateDirtyFlag();
	}
}

//***************************************************
void LC_ClientGamePlayer::PlayCommanderLevelupFx(int iLevel)
{
	SendMessage(CreateMsg_UE_COMMANDER_LEVELUP(GetID(), iLevel));
}

void LC_ClientGamePlayer::PlayTaskFinishFx()
{
	SendMessage(CreateMsg_UE_FINISH_TASK(GetID(), 0));
}

void LC_ClientGamePlayer::ReqRefreshConvoy(bool bAutoBuyMat,bool bForceHighest)
{

	MG_GAMESTORY_OPERATION_REQ reqMsg;
	reqMsg.m_nBuyItem = bAutoBuyMat;
	reqMsg.m_iStoryID = 37;
	if (bForceHighest)
	{
		reqMsg.m_iOPCode = GS_OP_TASK_APPOINT;
		reqMsg.m_kOPPara[0] = 1100011;
	}
	else
	{
		reqMsg.m_iOPCode = GS_OP_TASK_WAVER;
		reqMsg.m_kOPPara[0] = 0;
	}
	
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_GAMESTORY_OPERATION_REQ, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}


void LC_ClientGamePlayer::ReqTakeEscortReward(int iEscortType,int iNpcId)
{
	MG_REQ_AddEscortReward reqMsg;
	reqMsg.m_nEscortType	= iEscortType;
	reqMsg.m_nNpcID			= iNpcId;
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_ADDESCORTREWARD, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

int LC_ClientGamePlayer::GetRobbingUsedCount()
{
	return m_iRobbingTimes;
}

int LC_ClientGamePlayer::GetEscortUsedCount()
{						 
	return m_iEscortTimes;
}

int LC_ClientGamePlayer::GetEscortLevel()
{
	return m_iEscortLevel;
}

StringType LC_ClientGamePlayer::GetBiaocheState()
{
	StringStreamType sStream("");
	int iCharType[4]={92000018,92000019,92000020,92000021};
	LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
	for (int i=0;i<4;i++)
	{
		LC_ClientNPC* pkNPC = (LC_ClientNPC*)(pkNPCManager->FindNPCByCharTypeAndOwner(iCharType[i],GetOwnerObjectID()));
		if(pkNPC)
		{
			double percent=(double)pkNPC->GetHP()/(double)pkNPC->GetMaxHp();
			const Utility::UT_Vec3Int& vLocation= pkNPC->GetCurrentLocation();
			int posX = vLocation.x;
			int posY = vLocation.y;
			sStream<<percent<<'#'<<posX<<"#"<<posY<<'#'<<iCharType[i]<<"#"<<pkNPC->GetOwnerObjectID();
			break;
		}
	}
	return sStream.str();
}

void LC_ClientGamePlayer::StartEscort()
{
	
}

bool LC_ClientGamePlayer::IsGoodsEscortTaskProcessing()
{
	int TaskIds[4]={1100008,1100009,1100010,1100011};
	LC_TaskMap* pTaskMap=GetTaskMap();
	for (int i=0;i<4;i++)
	{
		if (pTaskMap->IsInProcessingTaskMap(TaskIds[i]))
		{
			return true;
		}
	}
	return false;
}

//void LC_ClientGamePlayer::SyncSchemeData(const SingleBuddyActivateSchemeData& syncInfo)
//{
//	mem::vector<int>& cIDs = m_mSchemeInfoMap[syncInfo.nType];
//	cIDs.clear();
//
//	for(size_t i=0; i<MAX_ACTIVE_BUDDY_NUM; ++i)
//	{
//		cIDs.push_back(syncInfo.m_Data[i]);
//	}
//}

//void LC_ClientGamePlayer::SyncAllSchemeData(const MG_SC_MultiBuddyActivateSchemeData& syncInfo)
//{
//	SetSchemeType(syncInfo.m_nCurrentType);
//
//	for(size_t i=0; i<MAX_ACTIVE_BUDDY_SCHEME_NUM; ++i)
//	{
//		SyncSchemeData(syncInfo.m_Data[i]);
//	}
//}

int LC_ClientGamePlayer::GetSchemeCid(int iElementType, int iSlotIdx)
{
	BuddySchemeInfoMap::iterator iter = m_mSchemeInfoMap.find(iElementType);
	if(iter == m_mSchemeInfoMap.end())
		return 0;

	const mem::vector<int>& cIDs = (*iter).second;
	if(iSlotIdx < 0 || iSlotIdx >= cIDs.size())
		return 0;

	return cIDs[iSlotIdx];
}

void LC_ClientGamePlayer::RequestSchemeModify(int iElementType, int iSlotIdx, int iCid)
{
	/*MG_CS_BuddyActivateSchemeModifyReq reqMsg;
	reqMsg.m_nType = iElementType;
	reqMsg.m_nPosIndex = iSlotIdx;
	reqMsg.m_nBuddyID = iCid;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_CS_BUDDY_SCHEME_MODIFY_REQ, &reqMsg);
	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());*/
}

void LC_ClientGamePlayer::RequestSetRankTransferState(int mapID)
{
	MG_REQ_SetRankTransferState reqMsg;
	reqMsg.m_nMapResID = mapID;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SETRANKTRANSFERSTATE, &reqMsg);
	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

void LC_ClientGamePlayer::RequestSchemeSwitch(int iElementType)
{
	/*MG_CS_BuddyActivateSchemeSwitchReq reqMsg;
	reqMsg.m_nCurrentType = iElementType;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_CS_BUDDY_SCHEME_SWITCH_REQ, &reqMsg);
	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());*/
}
//***************************************************
int64_t LC_ClientGamePlayer::GetCommanderBaseAttributeScore()
{
	return LC_Helper::CalcAttributeMapCombatScore(m_pkCommander->GetAttributeMap());
}

int64_t LC_ClientGamePlayer::GetCommanderTitleCombatScore()
{
	int ilevel = GetCommanderTitleLevel();
	CF_CommanderTitle::DataEntry *pkData = SafeGetCSVFileDataEntryPtr<CF_CommanderTitle>(ilevel);
	if(pkData == NULL)
		return 0;

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
	return LC_Helper::CalcAttributeMapCombatScore(&attrMap, false);
}
//***************************************************
void LC_ClientGamePlayer::RequestUpgradeBuddy(int iCid)
{
	MG_CS_BuddyManualLevelUp reqMsg;
	reqMsg.m_nBuddyID = iCid;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_CS_BUDDY_MANUAL_LEVEL_UP, &reqMsg);
	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

void LC_ClientGamePlayer::ReqSetRedPointFlag(int iType, bool bFlag)
{
	MG_REQ_SetRedPointFlag reqMsg;
	reqMsg.m_nParam = iType;
	reqMsg.m_bFlag = bFlag;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SETREDPOINTFLAG, &reqMsg);
	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}


void LC_ClientGamePlayer::UpdateRedPointFlag(int iType,std::vector<KeyValue_Base_Bool>& InVec)
{
	if(iType == SyncType_All)
	{
		m_nRedPointFlag.clear();
	}

	for (int i=0;i<InVec.size();i++)
	{
		m_nRedPointFlag[InVec[i].key]=InVec[i].value;
	}
}

bool LC_ClientGamePlayer::GetRedPointFlag(int iType)
{
	std::map<int,bool>::iterator it =  m_nRedPointFlag.find(iType);
	if (it!=m_nRedPointFlag.end())
	{
		return it->second;
	}
	return false;
}

bool LC_ClientGamePlayer::IsSpecking()
{
	bool bIsPlaying = false;

	if (m_iVoiceID > 0)
	{
		bIsPlaying = g_soundMgr.IsSoundPlaying(m_iVoiceID);
		
		if (!bIsPlaying)
		{
			ResetVoice();
		}
	}

	return bIsPlaying;
}

void LC_ClientGamePlayer::UpdateVoiceLocation()
{
	if (IsSpecking())
	{
		g_soundMgr.UpdateAudioSourcePosition(UT_EngineMath::ConvertVec3IntToNiPoint3(GetCurrentLocation()), m_iVoiceID);
	}
}

void LC_ClientGamePlayer::PlaySuccessVoice()
{
	int iCID = GetBuddyActiveCID(m_uiCurrentActiveBuddyIndex);
	CF_Buddy::DataEntry *pkBuddyData = SafeGetCSVFileDataEntryPtr<CF_Buddy>(iCID);
	if(pkBuddyData != NULL)
	{
		g_soundMgr.Play2DEx(pkBuddyData->_sWinSound);
	}
}

void LC_ClientGamePlayer::PlayFailVoice()
{
	int iCID = GetBuddyActiveCID(m_uiCurrentActiveBuddyIndex);
	CF_Buddy::DataEntry *pkBuddyData = SafeGetCSVFileDataEntryPtr<CF_Buddy>(iCID);
	if(pkBuddyData != NULL)
	{
		g_soundMgr.Play2DEx(pkBuddyData->_sLoseSound);
	}
}
//同步发的是组id
void LC_ClientGamePlayer::SetMapFullSchemeData(MG_SyncBuddyActivateScheme& msg)
{
	m_vMapOffensiveSchemeData.clear();
	m_vMapDefenceSchemeData.clear();
	for (int i = 0; i < msg.m_SchemeData.m_vMapOffensiveSchemeData.size(); ++i)
	{
		uint32_t mapId = msg.m_SchemeData.m_vMapOffensiveSchemeData[i].m_nSchemeID;
		std::vector<PS_SingleSchemeData> schemeData = msg.m_SchemeData.m_vMapOffensiveSchemeData[i].m_vSchemeData;
		m_vMapOffensiveSchemeData[mapId] = schemeData;
	}

	for (int i = 0; i < msg.m_SchemeData.m_vMapDefenceSchemeData.size(); ++i)
	{
		uint32_t mapId = msg.m_SchemeData.m_vMapDefenceSchemeData[i].m_nSchemeID;
		std::vector<PS_SingleSchemeData> schemeData = msg.m_SchemeData.m_vMapDefenceSchemeData[i].m_vSchemeData;
		m_vMapDefenceSchemeData[mapId] = schemeData;
	}
}
//请求回复发的是地图id
void LC_ClientGamePlayer::SetMapFullSchemeData(MG_RESP_SetBuddyActivateScheme& msg)
{
	SC_ClientScriptInterface* pkClientInterface = SC_ClientScriptInterface::GetSingletonPtr();
	SC_ClientSkillInterface* pkInterface = pkClientInterface->GetSkillInterface();
	int SchemeGroupID = pkInterface->GetSchemeGroupID(msg.m_nMapID);
	if (msg.m_nSchemeType == SchemeType_Offensive)
	{
		m_vMapOffensiveSchemeData[SchemeGroupID] = msg.m_vSchemeData;
	}
	else if (msg.m_nSchemeType == SchemeType_Defence)
	{
		m_vMapDefenceSchemeData[SchemeGroupID] = msg.m_vSchemeData;
	}
}
//请求设置默认阵容 发的地图id
void LC_ClientGamePlayer::SetMapFullSchemeData(MG_RESP_SetSpecialMapInitBuddyActivateScheme& msg)
{
	if (!LC_Helper::IsSchemeSpecialMap(msg.m_nMapID))
	{
		return;
	}
	SC_ClientScriptInterface* pkClientInterface = SC_ClientScriptInterface::GetSingletonPtr();
	SC_ClientSkillInterface* pkInterface = pkClientInterface->GetSkillInterface();
	int SchemeGroupID = pkInterface->GetSchemeGroupID(msg.m_nMapID);
	if (msg.m_nSchemeType == SchemeType_Offensive)
	{
		m_vMapOffensiveSchemeData[SchemeGroupID] = msg.m_vSchemeData;
	}
	else if (msg.m_nSchemeType == SchemeType_Defence)
	{
		m_vMapDefenceSchemeData[SchemeGroupID] = msg.m_vSchemeData;
	}
}

void LC_ClientGamePlayer::ReqSetBuddyActivateScheme(int MapGroupID, int SchemeType, StringType& str)
{
	MG_REQ_SetBuddyActivateScheme reqMsg;
	reqMsg.m_nMapID = MapGroupID;
	reqMsg.m_nSchemeType = SchemeType;

	TStringVector kStringVector = UT_BaseStringUtility::SplitString(str, '#');
	if (kStringVector.size()<=0)
		return;

	for(int i=0; i<kStringVector.size(); ++i)
	{
		TStringVector kSchemeVector = UT_BaseStringUtility::SplitString(kStringVector[i], '|');
		PS_SingleSchemeData Data;
		Data.m_nSchemeGroupID = atoi(kSchemeVector[0].c_str());
		for(int j=1; j<kSchemeVector.size(); ++j)
		{
			Data.m_vSchemeData[j-1] = atoi(kSchemeVector[j].c_str());
		}
		reqMsg.m_vSchemeData.push_back(Data);
	}

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SETBUDDYACTIVATESCHEME, &reqMsg);
	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

int LC_ClientGamePlayer::GetBuddySchemeCidByIndex(int MapGroupID, int SchemeType, int GroupID, int Index)
{
	mem::map<uint32_t, std::vector<PS_SingleSchemeData>>::iterator itor;
	if (SchemeType == SchemeType_Offensive)
	{
		itor = m_vMapOffensiveSchemeData.find(MapGroupID);
		if (itor == m_vMapOffensiveSchemeData.end())
			return 0;
	}
	else if (SchemeType == SchemeType_Defence)
	{
		itor = m_vMapDefenceSchemeData.find(MapGroupID);
		if (itor == m_vMapDefenceSchemeData.end())
			return 0;
	}

	for(int i = 0; i < itor->second.size(); ++i)
	{
		PS_SingleSchemeData Data = itor->second[i];
		if (Data.m_nSchemeGroupID == GroupID && Index < MAX_ACTIVE_BUDDY_NUM)
		{
			return Data.m_vSchemeData[Index];
		}
	}

	return 0;
}

void LC_ClientGamePlayer::ReqSetSpecialMapInitBuddyActivateScheme(int MapID, int SchemeType)
{
	if (!LC_Helper::IsSchemeSpecialMap(MapID))
	{
		return;
	}
	MG_REQ_SetSpecialMapInitBuddyActivateScheme reqMsg;
	reqMsg.m_nMapID = MapID;
	reqMsg.m_nSchemeType = SchemeType;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SETSPECIALMAP_INITBUDDYACTIVATESCHEME, &reqMsg);
	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

void LC_ClientGamePlayer::ReqBattleRecord(int iType)
{
	MG_Req_BattlefieldLog_List reqMsg;
	reqMsg.m_BattleType = iType;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_CS_BATTLE_LOG_REQ, &reqMsg);
	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

void LC_ClientGamePlayer::UpdateBattleRecord(MG_Resp_BattlefieldLog_List& msg)
{
	m_BattleLogList.clear();
	m_iBattleType = msg.m_nBattleType;
	m_iBattleRecordNum = msg.m_BattleLogList.m_BattleLogList.size();
	for (int i = m_iBattleRecordNum-1; i >= 0; --i)
	{
		BaseBattlefieldLog DataLog				= msg.m_BattleLogList.m_BattleLogList[i];
		LC_ClientBattleRecordDataBase data;
		data.UniqueID							= DataLog.m_nUniqueId;
		data.m_nOtherUserBaseData				= DataLog.m_nUserBaseData;
		data.m_nResult							= DataLog.m_nResult;
		data.m_CurrentValue						= DataLog.m_nValue.m_nValue;
		data.m_LastValue						= DataLog.m_nValue.m_nValue - DataLog.m_nValue.m_nAddValue;
		data.m_nAddValue						= DataLog.m_nValue.m_nAddValue;
		data.m_nLogTime							= DataLog.m_nLogTime;
		m_BattleLogList.push_back(data);
	}
}

void LC_ClientGamePlayer::ReqBattleRecordDetaiil(int iType, uint64_t index, int clientIndex, uint64_t iCharId)
{
	if (index <= 0)
	{
		SendLogicInfoMessage(RE_ArenaPlayBack_NoVideo);
		return;
	}
	MG_Req_BattlefieldReport reqMsg;
	reqMsg.m_BattleType = iType;
	reqMsg.m_nIndex = index;
	reqMsg.m_nClientIndex = clientIndex;
	reqMsg.m_nCharId = iCharId;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_CS_BATTLEFEILD_REPORT_REQ, &reqMsg);
	if(bCreateMsg)
		m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

void LC_ClientGamePlayer::UpdateBattleDetail(MG_Resp_BattlefieldReport& msg)
{
	if (msg.m_nBattleType != Arena_Type_Peak)
	{
		for (int i = 0; i < m_iBattleRecordNum; ++i)
		{
			LC_ClientBattleRecordDataBase& DataBase = m_BattleLogList[i];
			if (DataBase.GetUniqueID() == msg.m_nIndex)
			{
				BattleUserBaseData UserData = msg.m_BattleReport.m_FightData;
				uint64_t leftID = UserData.m_nLeftUserBaseData.m_CharID;
				bool bMine = leftID == GetCommanderUID();
				DataBase.SetUserBaseData(UserData.m_nLeftUserBaseData, bMine);
				DataBase.SetUserBaseData(UserData.m_nRightUserBaseData, !bMine);

				DataBase.SetDeatil(msg.m_BattleReport.m_ReportList, bMine);
			}
		}
	}
	else
	{
		LC_ClientBattleRecordDataBase DataBase;
		BattleUserBaseData UserData = msg.m_BattleReport.m_FightData;
		DataBase.SetUserBaseData(UserData.m_nLeftUserBaseData, true);
		DataBase.SetUserBaseData(UserData.m_nRightUserBaseData, false);
		DataBase.SetDeatil(msg.m_BattleReport.m_ReportList, true);

		m_PeakBattleInfo[msg.m_nIndex] = DataBase;
	}
}

int LC_ClientGamePlayer::GetBattleRecordTotalNum(int iType)
{
	return m_iBattleRecordNum;
}

LC_ClientBattleRecordDataBase* LC_ClientGamePlayer::GetBattleRecordInfo(int index)
{
	return &m_BattleLogList[index];
}

bool LC_ClientGamePlayer::IsDetailNeedRequest(int index)
{
	LC_ClientBattleRecordDataBase& DataBase = m_BattleLogList[index];
	return DataBase.GetIsDetail();
}

LC_ClientBattleRecordDataBase* LC_ClientGamePlayer::GetPeakDuelBattleRecordInfo(uint64_t id)
{
	mem::map<uint64_t, LC_ClientBattleRecordDataBase>::iterator itor = m_PeakBattleInfo.find(id);
	if (itor != m_PeakBattleInfo.end())
	{
		return &itor->second;
	}

	return NULL;
}

void LC_ClientGamePlayer::SetPlayBackBegin()
{
	//竞技场观战
	CF_WorldMapList::DataEntry* pkDataEntry = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(GetCurrentMapID());
	T_ASSERT_CRITICAL_S(pkDataEntry != NULL);

	if (IS_PLAYBACK_MAP(pkDataEntry->_iMapLogicType))
	{
		LC_ClientArenaPlayBackManager *pkArenaPlayBackManager = LC_ClientArenaPlayBackManager::GetSingletonPtr();
		pkArenaPlayBackManager->Begin();
	}
}
bool LC_ClientGamePlayer::IsPlayBackFlag()
{
	//竞技场观战
	CF_WorldMapList::DataEntry* pkDataEntry = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(GetCurrentMapID());
	T_ASSERT_CRITICAL_S(pkDataEntry != NULL);
	if (IS_PLAYBACK_MAP(pkDataEntry->_iMapLogicType))
		return true;
	return false;
}

void LC_ClientGamePlayer::RemoveAllBullets()
{
	SK_ClientSkillEventManager* skillEventManager = CLIENT_GET_SKILL_EVENT_MANAGER();
	skillEventManager->DeleteAllBulletEvent();

	GD_BulletManager::GetSingleton().ClearAllBullets();
}

void LC_ClientGamePlayer::ClearAllSkillSound()
{
	AS_ClientSoundManager::GetSingletonPtr()->StopPlayerSkillSounds();
	AS_ClientSoundManager::GetSingletonPtr()->StopNPCSkillSounds();
}

void LC_ClientGamePlayer::ChangePhaseMap(int iMapLogicID, int lMapResID, int lMapLogicType)
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetGameSessionGroup();
	UI_UIManager *pkUIMessage = UI_UIManager::GetSingletonPtr();
	//if(pkUIMessage)
		pkUIMessage->SetWorldMapID(lMapResID);
	
	//发送更换地图消息到逻辑端
	pkUIMessage->SetLoadingProcess(false);

	//AS_ClientSoundManager::GetSingletonPtr()->ClearBossFightInfo();
	
	if ( OnRltTransferChangePkMode(GetMapResID(), lMapResID) )
		RequestModifyPKSettingForce();
	
//********************************	
	LC_ClientWorldManager* pkWorldManager = CLIENT_GET_WORLD_MANAGER();

	int lCurrentMapLogicID = GetMapLogicID();
	int iPreMapResID = GetMapResID();

	if ( iPreMapResID != lMapResID )
	{
		CA_CameraManager::GetSingleton().ResetAllCamera();
	}

	LC_ClientLogicMapManager * pkLogicMapMgr = LC_ClientLogicMapManager::GetSingletonPtr();
	pkLogicMapMgr->AddLogicMapData(lMapResID);

	SetMapResID(lMapResID);
	SetMapLogicType(lMapLogicType);
	GetMovePath().Clear();
	SetMoveSpeed(0);
	SetVisibleMoveSpeed(0);
	SetMoveStride(false);
	SetWatchTargetID(0);
	SetLockedTargetID(0);
	FreezeLocked(false);
	SetPreLockedTargetID(0);
	SetLButtonPickActorID(0);
	ClearPendingSkill();
	_clearEyeAvatar();
	DestroySuiteNPC();
	this->_clearUnlockRequestExecuteSkill();

	GfxWriteLog( LOG_TAG_CLIENT_CRITICAL, LOG_SWITCH_CLIENT_CRITICAL , 
		"enter change map func");

	CF_WorldMapList* pkCSVWorldMapList = CF_WorldMapList::GetSingletonPtr();
	CF_WorldMapList::DataEntry* pkData = pkCSVWorldMapList->GetEntryPtr(lMapResID);
	if ( pkData == NULL )
	{
		GfxWriteLog( LOG_TAG_CLIENT_CRITICAL, LOG_SWITCH_CLIENT_CRITICAL , 
			"change map error can't find map %d ",lMapResID );
		MessageBoxA2W_IDS( IDS_GAMERES_CORRUPTED , L"Warning", MB_OK|MB_ICONWARNING);
		T_ASSERT_CRITICAL_S(false);
	}

	SetIsInParalle(pkData->_iShowRaidStage > 0);

	CF_MapEffectConfig::DataEntry* pkEffectCfg = CF_MapEffectConfig::GetSingletonPtr()->GetEntryPtr( lMapResID );
	StringType strDecoration = "";
	if ( pkEffectCfg )
	{
		strDecoration = pkEffectCfg->_kDecoration;
	}
	const StringType& rkWorldMapName = pkData->_kFilePath;

	SE_SceneManager* pkSceneManager = SE_SceneManager::GetSingletonPtr();
	if (pkSceneManager != NULL)
	{
		PF_PathManager* pkPathManager = PF_PathManager::GetSingletonPtr();
		if(pkPathManager&&pkData&&pkData->_kMapRadius > 0)
		{
			int iMaxX = pkData->_kCentralPoint.x+pkData->_kMapRadius;
			int iMaxY = pkData->_kCentralPoint.y+pkData->_kMapRadius;
			UT_Vec2Int kMax(iMaxX,iMaxY);
			int iMinX = pkData->_kCentralPoint.x-pkData->_kMapRadius;
			int iMinY = pkData->_kCentralPoint.y-pkData->_kMapRadius;
			UT_Vec2Int kMin(iMinX,iMinY);
			pkPathManager->SetGlobalPathFindingRangeLimits(kMax,kMin);

			FX_ClientFxManager* pkFXManager = FX_ClientFxManager::GetSingletonPtr();

			Utility::UT_Vec3Int vecFXPos[4];
			vecFXPos[0] = pkData->_kCentralPoint - Utility::UT_Vec3Int(pkData->_kMapRadius,0,0);
			vecFXPos[1] = pkData->_kCentralPoint - Utility::UT_Vec3Int(-pkData->_kMapRadius,0,0);
			vecFXPos[2] = pkData->_kCentralPoint - Utility::UT_Vec3Int(0,pkData->_kMapRadius,0);
			vecFXPos[3] = pkData->_kCentralPoint - Utility::UT_Vec3Int(0,-pkData->_kMapRadius,0);

			Utility::UT_Vec3Int kCentralPoint = pkData->_kCentralPoint;

			int Count = Float2Int((pkData->_kMapRadius*2)/1600);
			Count = Count*4;
			bool bShowFxWall = pkSceneManager->GetFxWallShow();
			int iFxID = 0;
			for (int i = 0; i < MAX_FX_WALL_NUM ; ++i)
			{
				if (i >= Count)
					break;

				if(bShowFxWall)
					iFxID =	pkSceneManager->GetFxWallID(i);
				else
					iFxID =	pkFXManager->CreateFx(RES_FX_16M_AIR_WALL,false);

				if (iFxID >= 0)
				{			
					int iSide = i%4; 
					NiPoint3 kFXPoint(UT_EngineMath::ConvertVec3IntToNiPoint3(vecFXPos[iSide]));
					NiPoint3 kDir(UT_EngineMath::ConvertVec3IntToNiPoint3(kCentralPoint - vecFXPos[iSide]));

					int iXBase = 0;
					int iYBase = 0;
					if (i >= 4)
					{
						int iFlag = 1;
						int iStep = Float2Int(i/4);
						if ( iStep%2 == 0)
						{
							iFlag = -1;
							iStep = Float2Int(iStep/2);
						}
						else
						{
							iStep = Float2Int(iStep/2) +1;
						}

						if (iSide == 0)
							iYBase = iFlag*iStep;
						else if (iSide == 1)
							iYBase = iFlag*iStep;
						else if (iSide == 2)
							iXBase = iFlag*iStep;
						else if (iSide == 3)
							iXBase = iFlag*iStep;
					}

					kFXPoint = kFXPoint + NiPoint3(iXBase*1600,iYBase*1600,0);
					pkFXManager->BindFxToLocation( iFxID, kFXPoint,&kDir);

					if(!bShowFxWall)
						pkFXManager->PlayFx(iFxID, GET_PROCESS_TIME);
				}

				pkSceneManager->SetFxWallID(i,iFxID);

			}

			pkSceneManager->SetFxWallShow(true);
		}
		else
		{
			pkPathManager->ClearGlobalPathFindingRangeLimits();
			FX_ClientFxManager* pkFXManager = FX_ClientFxManager::GetSingletonPtr();
			if (pkFXManager)
			{
				for (int i = 0; i < MAX_FX_WALL_NUM ; ++i)
				{
					int iFxID =	pkSceneManager->GetFxWallID(i);
					if (iFxID > 0)		
						pkFXManager->ReleaseFx(iFxID);

					pkSceneManager->SetFxWallID(i,0);
				}
			}

			pkSceneManager->SetFxWallShow(false);

		}

		SendMessage( CreateMsg_UI_MAP_CHANGE(iMapLogicID, lMapResID, lMapLogicType, iPreMapResID) );
		if (pkUIMessage)
			SendMessage( CreateMsg_UI_MAP_CHANGE(iMapLogicID, lMapResID, lMapLogicType, iPreMapResID), pkUIMessage );

		//更新玩家挂机信息(由于进入副本设置挂机模式会纪录挂机位置，但是出副本并没有设置挂机模式，没有更新挂机位置，导致玩家预设挂机位置错误，找不到目标)
		const UT_Vec3Int& sd = GetCurrentLocation();
		SetHookLocation(sd);

		//发送网络消息---记录当前已经到达的地图区域
		LC_ClientMapTranscriptionManager* pkMapTransMgr = LC_ClientMapTranscriptionManager::GetSingletonPtr();
		if (pkMapTransMgr)
		{
			MG_PACKET_ENCODE_HANDLER hMsgHandler;
			bool bCreateMsg = false;
			MG_Req_Record_Transport reqMsg;
			reqMsg.m_transportId = pkMapTransMgr->GetMapCategoryTypeByMapResID(lMapResID);
			bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_RECORD_TRANSPORT, &reqMsg);
			if(bCreateMsg)
				m_pkClientLocalSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
		}

		pkWorldManager->RemoveObjectFromMap(this);
		pkWorldManager->SetCurrentMapID(rkWorldMapName, iMapLogicID,lMapResID,lMapLogicType);

		SetCurrentLocation(sd);
		SetTargetLocation(sd);
		SetMoveLineTargetLocation(sd);

		pkWorldManager->AddObjectToMap(pkWorldManager->GetCurrentMapID(),this);

		LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
		if(pkNPCManager)
		{
			CF_WorldMapList* pkTranscriptionRule = CF_WorldMapList::GetSingletonPtr();
			CF_WorldMapList::DataEntry* pkData=pkTranscriptionRule->GetEntryPtr(lMapResID);

			if(pkData!=NULL && pkData->_iAutoFitLevel == 1)
				pkNPCManager->SetAutoFitFlag(true);
			else
				pkNPCManager->SetAutoFitFlag(false);
		}

		if (LC_Helper::CheckTranscriptionMapIDValidity(lMapResID))
		{
			LC_ClientTranscriptionInfoManager* pkTranscriptionInfoManager = CLIENT_GET_TRANSCRIPTION_INFO_MANAGER();
			//if(!pkTranscriptionInfoManager)
			//	return;
			if(pkTranscriptionInfoManager)
			{
				pkTranscriptionInfoManager->AddTranscriptionRevisionID();
				pkTranscriptionInfoManager->SetCurrentTranscriptionID(lMapResID);
			}
		}
	}	

	// 地图加载后重新设置玩家的假阴影
	GD_ClientActorModel* pkVisibleData = IM_Manager::GetSingleton().GetActor(GetID());
	if (pkVisibleData != NULL)
	{
		pkVisibleData->EnableFakeShadow(true);
	}
	else
	{
		m_bPendingToSetVisibleDataEnableFakeShadow = true;
	}

	LC_ClientGameStoryManager* pkGameStoryManager = CLIENT_GET_GAMESTORY_MANAGER();
	if(pkGameStoryManager)
		pkGameStoryManager->OnMapChanged(lMapResID);
	
	LC_ClientPlaceHolderManager *pkPlaceHolderManager = CLIENT_GET_PLACEHOLDER_MANAGER();
	if (pkPlaceHolderManager)
	{
		pkPlaceHolderManager->SetRaidSettle(false);
		pkPlaceHolderManager->ReSetRaidSettleDelayTime();
	}
	//清除当前地图的传送门
	LC_PortalAssert* pkPortalAssert = GetPortalAsset();
	LC_PortalEntry* pkPortalEntry = pkPortalAssert->GetMapPortal(GetMapResID());
	if (pkPortalEntry)
		pkPortalEntry->m_lNPCID = 0;

	//清除netplayer
	LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
	if (pkNetPlayerManager)
		pkNetPlayerManager->DestroyAllPlayer();

	//清除npc
	LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
	if (pkNPCManager )	
		pkNPCManager->DestroyAllGfxWallNPC();

	if ((GetMapLogicType() != MT_GUILDWAR && GetGuildWarType() != WarType_DomainWar)
		|| (!SC_ClientScriptInterface::GetSingleton().GetSkillInterface()->IsDomainMap(GetMapResID()) && GetGuildWarType() == WarType_DomainWar))
	{
		SetPowerGuildID(-1);
		SetGuildWarType(WarType_None);
	}

	PlayDistortionScreenFx();
	LC_ClientCombatRecordManager::GetSingleton().Reset();

	GetRaidHookAsset().ResetRaidStageHookAssist();
	//FX_ClientFxManager::GetSingleton().ClearDischarGfx();
	ClearJumpPathPendingAction();
	_resetTriggerInfo();

	LC_ClientNPCManager* pkLocalNpcManager = CLIENT_GET_LOCAL_NPC_MANAGER();
	if (pkLocalNpcManager != NULL)
	{
		pkLocalNpcManager->DestroyAllDeadNPC();
		pkLocalNpcManager->OnMapReloaded();
	}

	CF_WorldMapList::DataEntry* oldPkData = pkCSVWorldMapList->GetEntryPtr(iPreMapResID);
	if (oldPkData->_iShowRaidStage)
	{
		if (!pkData->_iShowRaidStage)
		{
			LC_GameEvent GameEvent;
			GameEvent.SetEventType(GLET_NOTIFY_TRANSFER_FROM_STAGERAID);
			pkUIMessage->HandlerGameEvent(GameEvent);
		}
	}

	NotifyServerAlreadyInGame();
}

//************************************************************
int LC_ClientGamePlayer::GetBuddyObjectID(int cID)
{
	m_vActiveBuddyListItr itr = m_vActiveBuddyList.begin();

	for(; itr != m_vActiveBuddyList.end(); ++itr)
	{
		if(itr->nBuddyTemplateId == cID)
			return itr->nObjectId;
	}

	return -1;
}

//**********************************************
void LC_ClientGamePlayer::SwapOtherBaseData(LC_PlayerBase* srcPlayer, LC_PlayerBase* dstPlayer)
{
//Swap SkillStateMap
	swap(srcPlayer->m_kSkillStateMap, dstPlayer->m_kSkillStateMap);
	srcPlayer->m_kSkillStateMap->SetOwner(srcPlayer);
	dstPlayer->m_kSkillStateMap->SetOwner(dstPlayer);

//Swap Locations
	/*swap(*(srcPlayer->GetCurrentLocationPtr()), *(dstPlayer->GetCurrentLocationPtr()));
	swap(*(srcPlayer->GetVisibleLocationPtr()), *(dstPlayer->GetVisibleLocationPtr()));*/
	
	/*if (srcPlayer->GetMoveSpeed() > 0)
		dstPlayer->SetTargetLocation(srcPlayer->GetTargetLocation());
	else
		dstPlayer->SetTargetLocation(dstPlayer->GetCurrentLocation());

	if (dstPlayer->GetMoveSpeed() > 0)
		SetTargetLocation(dstPlayer->GetTargetLocation());
	else
		SetTargetLocation(GetCurrentLocation());*/

	if (dstPlayer->GetMoveSpeed() > 0)
		SetTargetLocation(dstPlayer->GetTargetLocation());

//Swap Titles
	SwapTitles(dstPlayer);

//Interrupt Normal Attack
	float fCurrentTime = GET_PROCESS_TIME;
	if (dstPlayer->GetCurrentStateType() == LC_Helper::GetNormalAttackSkill(dstPlayer))
	{
		dstPlayer->RequestInterrputSkill(fCurrentTime,dstPlayer->GetCurrentStateType(),SKI_FORCE);
		dstPlayer->InterruptSkill(fCurrentTime,dstPlayer->GetCurrentStateType(),SKI_FORCE);
		dstPlayer->FixCurrentState(ST_NONE, fCurrentTime, 0);
	}

	if (srcPlayer->GetCurrentStateType() == LC_Helper::GetNormalAttackSkill(srcPlayer))
	{
		srcPlayer->RequestInterrputSkill(fCurrentTime,srcPlayer->GetCurrentStateType(),SKI_FORCE);
		srcPlayer->InterruptSkill(fCurrentTime,srcPlayer->GetCurrentStateType(),SKI_FORCE);
		srcPlayer->FixCurrentState(ST_NONE, fCurrentTime, 0);
	}
}

void LC_ClientGamePlayer::SwapTitles(LC_PlayerBase* dstPlayer)
{
	if (dstPlayer != NULL)
		SendMessage( CreateMsg_UE_CHANGE_PLAYER_TITLE(dstPlayer->GetID()));
	SendMessage( CreateMsg_UE_CHANGE_PLAYER_TITLE(GetID()) );
}

void LC_ClientGamePlayer::AttachEffects(LC_PlayerBase* srcPlayer, LC_PlayerBase* dstPlayer)
{
//显示脚底光圈
	srcPlayer->ShowGroundFx();

//添加切换特效
	GD_ClientActorModel* srcVisibleData = IM_Manager::GetSingleton().GetActor(srcPlayer->GetID());
	if(srcVisibleData)
		srcVisibleData->AttachFx(GD_ClientActorModel::FDT_GROUND, 0, false, RES_FX_SWAP_SRCEFFECT);

	if (dstPlayer != NULL)
	{
		GD_ClientActorModel* dstVisibleData = IM_Manager::GetSingleton().GetActor(dstPlayer->GetID());
		if(dstVisibleData)
			dstVisibleData->AttachFx(GD_ClientActorModel::FDT_GROUND, 0, false, RES_FX_SWAP_DSTEFFECT);
	}
}

//********************************************
float LC_ClientGamePlayer::GetRestRespawnCoolDown(int index)
{
	if (index < 0 || index >= MAX_ACTIVE_BUDDY_NUM)
		return 0;

	return m_vActiveBuddyRespawnTimeCounts[index].fRestTime;
}

void LC_ClientGamePlayer::ActiveRespawnCoolDown(int index, float restTime)
{
	if (index < 0 || index >= MAX_ACTIVE_BUDDY_NUM)
		return;
	m_vActiveBuddyRespawnTimeCounts[index].bInCoolDown = true;
	m_vActiveBuddyRespawnTimeCounts[index].fStartTime = GET_PROCESS_TIME;
	m_vActiveBuddyRespawnTimeCounts[index].fLastTime = m_vActiveBuddyRespawnTimeCounts[index].fStartTime;
	m_vActiveBuddyRespawnTimeCounts[index].fRestTime = restTime;
}

void LC_ClientGamePlayer::DeactiveRespawnCoolDown(int index)
{
	if (index < 0 || index >= MAX_ACTIVE_BUDDY_NUM)
		return;
	m_vActiveBuddyRespawnTimeCounts[index].bInCoolDown = false;
	m_vActiveBuddyRespawnTimeCounts[index].fRestTime = 0;
}

void LC_ClientGamePlayer::ResetRespawnCoolDown(int index)
{
	if (index < 0 || index >= MAX_ACTIVE_BUDDY_NUM)
		return;

	m_vActiveBuddyRespawnTimeCounts[index].bInCoolDown = false;
	m_vActiveBuddyRespawnTimeCounts[index].fStartTime = 0;
	m_vActiveBuddyRespawnTimeCounts[index].fLastTime = 0;
	m_vActiveBuddyRespawnTimeCounts[index].fRestTime = 0;
}

void LC_ClientGamePlayer::UpdateRespawnCoolDown(float fCurrentTime)
{
	float fDeltaTime = 0;
	for (int i=0; i<MAX_ACTIVE_BUDDY_NUM; ++i)
	{
		fDeltaTime = fCurrentTime - m_vActiveBuddyRespawnTimeCounts[i].fLastTime;
		m_vActiveBuddyRespawnTimeCounts[i].fRestTime -= fDeltaTime;
		if (m_vActiveBuddyRespawnTimeCounts[i].fRestTime <= 0)
		{
			DeactiveRespawnCoolDown(i);
			continue;
		}
		m_vActiveBuddyRespawnTimeCounts[i].fLastTime = fCurrentTime;
	}
}

//************************************************************
float LC_ClientGamePlayer::GetRestSwapCoolDown(int index)
{
	if (index < 0 || index >= MAX_ACTIVE_BUDDY_NUM)
		return 0;
	return m_vSwapTimeCounts[index].fRestTime;
}

void LC_ClientGamePlayer::SetSwapCoolDown(int index, float restTime)
{
	if (index < 0 || index >= MAX_ACTIVE_BUDDY_NUM)
		return;
	m_vSwapTimeCounts[index].fRestTime = restTime;
}

void LC_ClientGamePlayer::ActiveSwapCoolDown(int index, float restTime)
{
	if (index < 0 || index >= MAX_ACTIVE_BUDDY_NUM)
		return;
	m_vSwapTimeCounts[index].bInCoolDown = true;
	m_vSwapTimeCounts[index].fStartTime = GET_PROCESS_TIME;
	m_vSwapTimeCounts[index].fLastTime = m_vSwapTimeCounts[index].fStartTime;
	m_vSwapTimeCounts[index].fRestTime = restTime;
}

void LC_ClientGamePlayer::DeactiveSwapCoolDown(int index)
{
	if (index < 0 || index >= MAX_ACTIVE_BUDDY_NUM)
		return;
	m_vSwapTimeCounts[index].bInCoolDown = false;
	m_vSwapTimeCounts[index].fRestTime = 0;
}

void LC_ClientGamePlayer::UpdateSwapCoolDown(float fCurrentTime)
{
	float fDeltaTime = 0;
	for (int i=0; i<MAX_ACTIVE_BUDDY_NUM; ++i)
	{
		fDeltaTime = fCurrentTime - m_vSwapTimeCounts[i].fLastTime;
		m_vSwapTimeCounts[i].fRestTime = m_vSwapTimeCounts[i].fRestTime - fDeltaTime;
		if (fCurrentTime > m_vSwapTimeCounts[i].fStartTime + m_fSwitchBuddyCDTime)
		{
			DeactiveSwapCoolDown(i);
			continue;
		}
		m_vSwapTimeCounts[i].fLastTime = fCurrentTime;
	}
}

void LC_ClientGamePlayer::ResetSwapCoolDown()
{
	for (int i=0; i<MAX_ACTIVE_BUDDY_NUM; ++i)
	{
		m_vSwapTimeCounts[i].bInCoolDown = false;
		m_vSwapTimeCounts[i].fStartTime = 0;
		m_vSwapTimeCounts[i].fLastTime = 0;
		m_vSwapTimeCounts[i].fRestTime = 0;
	}
}

void LC_ClientGamePlayer::ResetSwapCoolDownByCID(int cID)
{
	for (int i=0; i<m_vActiveBuddyList.size(); ++i)
	{
		if (m_vActiveBuddyList[i].nBuddyTemplateId == cID)
		{
			DeactiveSwapCoolDown(i);
			return;
		}
	}
}

//************************************************************
bool LC_ClientGamePlayer::IsBuddyInstance(int cID)
{
	for(int i=0;i<m_vActiveBuddyList.size();++i)
	{
		if(cID == m_vActiveBuddyList[i].nBuddyTemplateId)
			return false;
	}
	return true;
}

//************************************************************
void LC_ClientGamePlayer::UpdateBuddyInstanceList()
{
	uint16_t cID = -1;
	for(int i=0; i<m_vInactiveBuddyList.size(); ++i)
	{
		cID = m_vInactiveBuddyList[i].nBuddyTemplateId;
		CreateNewBuddyInstance(cID);
	}

	for(int i=0; i<m_vActiveBuddyList.size(); ++i)
	{
		cID = m_vActiveBuddyList[i].nBuddyTemplateId;
		CreateNewBuddyInstance(cID);
	}
}

//************************************************************
GameLogic::LC_SkillAsset* LC_ClientGamePlayer::GetBuddySkillAsset(int cID)
{
	m_mBuddyInstanceMapItr iter = m_mBuddyInstanceMap.find(cID);
	if(iter != m_mBuddyInstanceMap.end())
		return iter->second->GetSkillAsset();
	return NULL;
}
//***********************************************************
combat_score_type LC_ClientGamePlayer::GetAddEquipScore(LC_BackPackEntry* pkEntry)
{
	int iItemID = pkEntry->GetTypeID();
	CF_ItemList::DataEntry *pkItemInfo = SafeGetCSVFileDataEntryPtr<CF_ItemList>(iItemID);
	if(NULL == pkItemInfo)
		return 0;

	CF_ItemEquipLimitFunc::DataEntry *pkItemLmt = SafeGetCSVFileDataEntryPtr<CF_ItemEquipLimitFunc>(pkItemInfo->_lEquipLimitFuncID);
	if(NULL == pkItemLmt)
		return 0;

	if(GetCommanderLevel() < pkItemLmt->_lLevelLimit)
		return 0;

	//成长系统等级不足
	LC_SubordinateAsset& kSubAsset = GetSubordinateAsset();
	UT_SIMDataList::iterator itLmt = pkItemLmt->_kSubLimits.begin();
	for (; itLmt!=pkItemLmt->_kSubLimits.end(); ++itLmt)
	{
		LC_SubUpgradeData* pkSubData = kSubAsset.GetSubData(itLmt->ID());
		int32_t iLevel = pkSubData && pkSubData->IsEnabled() ? pkSubData->GetStage() : 0;

		if (iLevel < (int32_t)itLmt->ParamA())
		{
			return 0;
		}
	}

	LC_PackAsset *pkPackAsset = GetPackAsset();
	SC_ClientScriptInterface* pkClientInterface = SC_ClientScriptInterface::GetSingletonPtr();
	SC_ClientSkillInterface* pkInterface = pkClientInterface->GetSkillInterface();

	combat_score_type iNewEquipScore = pkInterface->GetEquipScoreByItemID(pkEntry->GetTypeID());
	LC_BackPackEntry *pkCurEquipEntry = GetEquipEntry(pkItemLmt->_lPackType, pkItemLmt->_lEquipType);
	
	combat_score_type iCurScore = 0;
	if(pkCurEquipEntry != NULL && pkCurEquipEntry->GetValid())
		iCurScore = pkInterface->GetEquipScoreByItemID(pkCurEquipEntry->GetTypeID());

	return iNewEquipScore - iCurScore;

}
//***********************************************************
ResultType LC_ClientGamePlayer::CheckEquipItemInPackage(int iPackType, int iEntryIndex, int cID)
{
	LC_PackAsset* packAssetPtr = GetPackAsset();
	// 目前只有普通背包的物品，能直接装备
	if (iPackType != PACK_TYPE_BASIC)
	{
		return RE_FAIL;
	}

	LC_BackPackEntry* pkEntry = m_kPackAsset.GetBackPackEntry(iPackType, iEntryIndex);
	if (!pkEntry)
	{
		return RE_ITEM_SYSTEM_ERROR;
	}
	if (!pkEntry->GetValid())
	{
		return RE_ITEM_SYSTEM_ERROR;
	}
	else if (pkEntry->GetLocked())
	{
		return RE_ITEM_TRADE_ITEM_LOCKED;
	}
	
	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	int lItemTypeID = pkEntry->GetTypeID();
	LC_ItemBase* pkItem = pkItemFactory->RequestItem(lItemTypeID);
	if (!pkItem)
	{
		return RE_ITEM_INVALID;
	}
	
	int iCategory = pkItem->GetCategory();
	int iEquipLimitLevel = pkItem->GetEquipLevelLimit();

	if((IsCommanderEquip(iCategory) && iEquipLimitLevel > GetCommanderLevel()))
	{
		return RE_ITEM_CANT_EQUIP_LEVEL;
	}

	int iTarPackType = pkItem->GetEquipPackLimit();
	int32_t iEquipType = pkItem->GetEquipTypeLimit();

	LC_BackPackEntry* pkOldEquipEntry = GetEquipEntry(iTarPackType, iEquipType);
	if (pkOldEquipEntry->GetValid())
	{
		int lOldItemID = pkOldEquipEntry->GetTypeID();
		LC_ItemBase* pkOldItem = pkItemFactory->RequestItem(lOldItemID);
		if (!pkOldItem)
		{
			return RE_ITEM_INVALID;
		}
		int iResultUnEquip = pkOldItem->CheckUnEquipItem(this, pkOldEquipEntry);
		if (iResultUnEquip != RE_SUCCESS)
		{
			return iResultUnEquip;
		}
	}

	return pkItem->CheckEquipItem(this, pkEntry);
}

//*********************************************************

GameLogic::LC_BackPackEntry* LC_ClientGamePlayer::GetRuneByItemId(int iItemId)
{
	GameLogic::LC_PackAsset* packAsset=GetPackAsset();
	int index=packAsset->GetIndexFromPackAsset(PACK_TYPE_BASIC,iItemId);
	LC_BackPackEntry* pkEntry = packAsset->GetBackPackEntry(PACK_TYPE_BASIC,index);
	if (pkEntry!=NULL)
	{
		return pkEntry;
	}

	m_mBuddyInstanceMapItr itr = m_mBuddyInstanceMap.begin();
	for(; itr != m_mBuddyInstanceMap.end(); ++itr)
	{
		LC_ClientBuddyInstance* pkInst = itr->second;
		for(size_t i=0; i<GetGlobalSetting.RunePackSize; i++)
		{
			pkEntry = pkInst->GetRuneEquipEntry(i);
			if(pkEntry->GetValid() && pkEntry->GetTypeID() == iItemId)
			{
				return pkEntry;
			}
		}
	}
	return NULL;
}

//**********************************************************
GameLogic::LC_ActorAttributeMap* LC_ClientGamePlayer::GetBuddyAttributeMap(int cID)
{
	m_mBuddyInstanceMapItr iter = m_mBuddyInstanceMap.find(cID);
	if(iter != m_mBuddyInstanceMap.end())
		return iter->second->GetAttributeMap();
	return NULL;
}

//**********************************************************
GameLogic::LC_ActorAttributeAppendMap* LC_ClientGamePlayer::GetBuddyAttributeAppendMap(int cID)
{
	m_mBuddyInstanceMapItr iter = m_mBuddyInstanceMap.find(cID);
	if(iter != m_mBuddyInstanceMap.end())
		return iter->second->GetAttributeAppendMap();
	return NULL;
}

//************** get buddy shortcut bar
GameLogic::LC_ShortcutBar* LC_ClientGamePlayer::GetBuddyShortcutBar(int cID)
{
	m_mBuddyInstanceMapItr iter = m_mBuddyInstanceMap.find(cID);
	if(iter != m_mBuddyInstanceMap.end())
		return iter->second->GetShortcutBar();
	return NULL;
}

//************** check buddy activate skill
bool LC_ClientGamePlayer::CheckBuddyActivateSkill(int cID, int skillID)
{
	LC_SkillAsset* buddySkillAsset = GetBuddySkillAsset(cID);
	if (NULL == buddySkillAsset)
		return false;

	LC_SkillAssetEntry* skillEntry= buddySkillAsset->FindLearnedSkill(skillID, false);
	if (skillEntry == NULL)
		return false;

	return (skillEntry->GetLogicValue() > 0);
}

//***** skill usable count, max usable count
int LC_ClientGamePlayer::GetBuddySkillUsableCount(int cID, int skillID)
{
	int count = 0;
	LC_SkillAsset* skillAsset = GetBuddySkillAsset(cID);
	
	if (skillAsset != NULL)
	{

		LC_SkillAssetEntry *skillEntry = skillAsset->FindLearnedSkill(skillID);
		if(skillEntry)
		{
			count = skillEntry->GetUsableCount();
		}
		else
			count = 1;
	}

	return count;
}

int LC_ClientGamePlayer::GetBuddySkillMaxUsableCount(int cID, int skillID)
{
	int count = 0;
	LC_SkillAsset* skillAsset = GetBuddySkillAsset(cID);

	if (skillAsset != NULL)
	{

		LC_SkillAssetEntry *skillEntry = skillAsset->FindLearnedSkill(skillID);
		if(skillEntry)
		{
			count = skillEntry->GetMaxUsableCount();
		}
	}

	return count;
}

//**********************************************************
void LC_ClientGamePlayer::SyncBuddyLevel(int cID)
{
	object_id_type playerID = GetBuddyObjectID(cID);
	LC_PlayerBase* pkPlayer = GetBuddyPlayerBasePtr(playerID);
	int curLevel = GetBuddyLevel(cID);

	if (pkPlayer != NULL)
		pkPlayer->SetLevel(curLevel);
}

//**********************************************************
void LC_ClientGamePlayer::SyncBuddyEXP(int cID)
{
	object_id_type playerID = GetBuddyObjectID(cID);
	LC_PlayerBase* pkPlayer = GetBuddyPlayerBasePtr(playerID);
	int curEXP = GetBuddyEXP(cID);

	if (pkPlayer != NULL)
	{
		
		if(pkPlayer == this)
		{	
			((LC_ClientGamePlayer*)pkPlayer)->SetEXP(curEXP);
		}
		else
			((LC_ClientNetPlayer*)pkPlayer)->SetEXP(curEXP);
	}
}

//**********************************************************
void LC_ClientGamePlayer::SyncBuddyCombate(int cID)
{
	object_id_type playerID = GetBuddyObjectID(cID);
	LC_PlayerBase* pkPlayer = GetBuddyPlayerBasePtr(playerID);
	combat_score_type curEXP = GetBuddyCombateScore(cID);

	if (pkPlayer != NULL)
	{
		
		if(pkPlayer == this)
		{	
			((LC_ClientGamePlayer*)pkPlayer)->SetCombateScore(curEXP);
		}
		else
			((LC_ClientNetPlayer*)pkPlayer)->SetCombateScore(curEXP);
	}
}
//**********************************************************
void LC_ClientGamePlayer::SendBuddyExpAndLevelChange(int oldExp, int oldLevel, int newExp, int newLevel, int cID)
{	
	int totalExp = 0;
	if(newLevel == oldLevel)
	{
		totalExp = newExp - oldExp;
	}
	else
	{
		CF_LevelUp::DataEntryMapExternIterator kIter = CF_LevelUp::GetSingleton().GetIterator();
		for(; kIter.HasMoreElements(); kIter.MoveNext())
		{
			const CF_LevelUp::DataEntry *pkEntry = kIter.PeekNextValuePtr();
			if(pkEntry->_iID == oldLevel + 1)
			{
				totalExp += pkEntry->_lEXPNeed - oldExp;
			}
			else if(pkEntry->_iID > oldLevel + 1 && pkEntry->_iID < newLevel + 1)
			{
				totalExp += pkEntry->_lEXPNeed;
			}
			else if(pkEntry->_iID == newLevel + 1)
			{
				totalExp += newExp;
			}
		}
	}
	if(totalExp > 0)
	{
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_BUDDY_EXP_ADD);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, cID);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, totalExp);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);

		/*object_id_type playerID = GetBuddyObjectID(cID);
		LC_PlayerBase* pkPlayer = GetBuddyPlayerBasePtr(playerID);
		if (pkPlayer != NULL)
		{
			SendLogicInfoMessage(RE_BUDDY_EXP_ADD, pkPlayer->GetCharNameByID() , totalExp);
		}*/
	}
	
	if(newLevel > oldLevel)
	{
		CF_Buddy::DataEntry *pkBuddyData = SafeGetCSVFileDataEntryPtr<CF_Buddy>(this->GetCharType());
		if(pkBuddyData != NULL)
		{
			int iSoundID = g_soundMgr.Play3DEx(UT_EngineMath::ConvertVec3IntToNiPoint3(GetCurrentLocation()), pkBuddyData->_sLevelUpSound, AS_ClientSoundManager::SCT_PLAYER_SKILL);
			ResetVoice(iSoundID);
		}

		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_BUDDY_LEVELUP);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, cID);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, newLevel - oldLevel);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}


}


//**********************************************************
void LC_ClientGamePlayer::SetAllOwner(int cID, LC_PlayerBase* pkPlayer)
{
	GetBuddySkillAsset(cID)->setOwner(pkPlayer);
	GetBuddyShortcutBar(cID)->SetOwner(pkPlayer);
}

//**********************************************************
bool LC_ClientGamePlayer::ReqUpgradeBuddyStar(int cID)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	MG_CS_BuddyStarUpgrade reqMsg;
	reqMsg.m_lPlayerID = 0;
	reqMsg.m_nBuddyID = cID;

	bool res = false;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_CS_BUDDY_BUDDY_STAR_UPGRADE, &reqMsg);
	if (bCreateMsg)
	{
		this->SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
		res = true;
	}
	return res;
}

//*******************************************************

int LC_ClientGamePlayer::GetSingleCombatRank(int key)
{
	mem::map<int, SingleCombatRecord>::iterator itor = m_kSingleCombatData.find(key);
	if (itor != m_kSingleCombatData.end())
	{
		return itor->second.m_iRank;
	}
	return -1;
}

int64_t LC_ClientGamePlayer::GetSingleCombatDamage(int key)
{
	mem::map<int, SingleCombatRecord>::iterator itor = m_kSingleCombatData.find(key);
	if (itor != m_kSingleCombatData.end())
	{
		return itor->second.m_iDamageValue;
	}
	return -1;
}

void LC_ClientGamePlayer::UpdateSingleCombatData(const MG_SC_SingleCombatRecorder& rkMsg)
{
	SingleCombatRecord& data = m_kSingleCombatData[rkMsg.m_nKey];
	data.m_iRank = rkMsg.m_nRank >= SPECIES_RANK_BEGIN ? (rkMsg.m_nRank+1) : rkMsg.m_nRank;
	data.m_iDamageValue = rkMsg.m_nDamageValue;
	data.m_iGameStoryID = rkMsg.m_nKey;
	data.m_iParam = rkMsg.m_nParam;
}

//-----------------------------------------------------------

int32_t LC_ClientGamePlayer::GetRaidBossDropTimes(int iDropType)
{
	mem::map<int, RaidBossDropInfo>::const_iterator iter = m_kRaidBossDropData.find(iDropType);
	if(iter == m_kRaidBossDropData.end())
		return 0;

	return iter->second.m_nDropTimes;
}

uint32_t LC_ClientGamePlayer::GetRaidBossDropNextRevertTime(int iDropType)
{
	mem::map<int, RaidBossDropInfo>::const_iterator iter = m_kRaidBossDropData.find(iDropType);
	if(iter == m_kRaidBossDropData.end())
		return 0;

	return iter->second.m_nNextRevertTime;
}

uint32_t LC_ClientGamePlayer::GetRaidBossDropEndRevertTime(int iDropType)
{
	mem::map<int, RaidBossDropInfo>::const_iterator iter = m_kRaidBossDropData.find(iDropType);
	if(iter == m_kRaidBossDropData.end())
		return 0;

	return iter->second.m_nEndRevertTime;
}


//-----------------------------------------------------------

void LC_ClientGamePlayer::SetBossDropInfo(const MG_SyncBossDropInfo& rkMsg)
{
	RaidBossDropInfo& dropInfo = m_kRaidBossDropData[rkMsg.m_nType];
	
	dropInfo.m_nDropTimes = rkMsg.m_nDropTimes;
	dropInfo.m_nNextRevertTime = rkMsg.m_nNextRevertTime;
	dropInfo.m_nEndRevertTime = rkMsg.m_nEndRevertTime;
}

void LC_ClientGamePlayer::SetBuddyEquipShowLevel(int iCid, int iLevel)
{
	m_mBuddyInstanceMapItr iter = m_mBuddyInstanceMap.find(iCid);
	if(iter != m_mBuddyInstanceMap.end())
		iter->second->SetEquipShowLevel(iLevel);
}

int LC_ClientGamePlayer::GetBuddyEquipShowLevel(int iCid)
{
	m_mBuddyInstanceMapItr iter = m_mBuddyInstanceMap.find(iCid);
	if(iter != m_mBuddyInstanceMap.end())
		return iter->second->GetEquipShowLevel();

	return 0;
}

void LC_ClientGamePlayer::SetBuddyShowRank(int iCid, int iRank)
{
	m_mBuddyInstanceMapItr iter = m_mBuddyInstanceMap.find(iCid);
	if(iter != m_mBuddyInstanceMap.end())
		iter->second->SetBuddyRank(iRank);
}

int LC_ClientGamePlayer::GetBuddyShowRank(int iCid)
{
	m_mBuddyInstanceMapItr iter = m_mBuddyInstanceMap.find(iCid);
	if(iter != m_mBuddyInstanceMap.end())
		return iter->second->GetBuddyRank();

	return -1;
}

void LC_ClientGamePlayer::RequestSetEquipShowLevel(int iCid)
{
	MGPT_REQ_EquipSlotLv reqMsg;
	reqMsg.m_nBuddyId = iCid;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_EQUIP_SLOT_LV, &reqMsg);
	if(bCreateMsg)
		SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

void LC_ClientGamePlayer::RequestSubModingSpirit(int iSubType, int iEquipSlot)
{
	MG_REQ_SubEquipSlotMoldingSpirit reqMsg;
	reqMsg.m_nSubType = iSubType;
	reqMsg.m_nSlotIndex = iEquipSlot;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SUBEQUIPSLOTMOLDINGSPIRIT, &reqMsg);
	if(bCreateMsg)
		SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

void LC_ClientGamePlayer::RequestSetCommanderEquipSuitLevel(int iLevel)
{
	MG_REQ_SetEquipSuitLevel reqMsg;
	reqMsg.m_nEquipSuitLevel = iLevel;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_SETEQUIPSUITLEVEL, &reqMsg);
	if(bCreateMsg)
		SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

void LC_ClientGamePlayer::RequestBuddyCombatComposition(int cid)
{
	MG_REQ_ControllerAndCoreCombatScore reqMsg;
	reqMsg.m_nBuddyID = cid;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_CONTROLLER_AND_CORE_COMBAT_SCORE, &reqMsg);
	if(bCreateMsg)
		SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

void LC_ClientGamePlayer::SetBuddyCombatComposition(int cid, combat_score_type iCommanderScore, combat_score_type iChainScore)
{
	m_mBuddyCombat[cid].m_nCommanderCombatScore = iCommanderScore;
	m_mBuddyCombat[cid].m_nChainCombatScore = iChainScore;
}

combat_score_type LC_ClientGamePlayer::GetBuddyCombatCommanderScore(int cid)
{
	mem::map<int,BuddyCombatScore>::iterator iter = m_mBuddyCombat.find(cid);
	if ( iter != m_mBuddyCombat.end() )
	{
		return iter->second.m_nCommanderCombatScore;
	}
	return 0;
}

combat_score_type LC_ClientGamePlayer::GetBuddyCombatChainScore(int cid)
{
	mem::map<int,BuddyCombatScore>::iterator iter = m_mBuddyCombat.find(cid);
	if ( iter != m_mBuddyCombat.end() )
	{
		return iter->second.m_nChainCombatScore;
	}
	return 0;
}

combat_score_type LC_ClientGamePlayer::GetBuddyRuneSkillCombatScoreBonus(int Cid, const StringType& skillIDs)
{
	LC_ActorAttributeMap* attaddress = GetBuddyAttributeMap(Cid);
	LC_ActorAttributeMap attrMap = *attaddress;
	combat_score_type iScore = LC_Helper::CalcAttributeMapCombatScore(&attrMap, false);
	
	mem::map<int, int> att;
	TStringVector sVec = UT_EngineStringUtility::SplitString(skillIDs, '#');
	for(int i = 0; i < sVec.size(); i++)
	{
		int iSkillID = String2Int(sVec[i]);
		if(iSkillID > 0)
		{
			CF_SkillSpecialAttrib::DataEntry *pkSkill = SafeGetCSVFileDataEntryPtr<CF_SkillSpecialAttrib>(iSkillID);
			if(pkSkill != NULL && pkSkill->Properties != "0")
			{
				TStringVector kVec = UT_EngineStringUtility::SplitString(pkSkill->Properties, ';');
				for (int i = 0 ; i < kVec.size() ; i++ )
				{
					TStringVector pVec = UT_EngineStringUtility::SplitString(kVec[i], ':');
					T_ASSERT_CRITICAL_S(pVec.size() == 4 && String2Int(pVec[1]) == 4);
					int key = String2Int(pVec[2]);
					int val = String2Int(pVec[3]);
					att[key] += val;
				}
			}
		}
	}

	mem::map<int,int>::iterator iter = att.begin();
	for (; iter != att.end(); ++iter)
	{
		attrMap.ModifyAttributeByPart(4, iter->first, iter->second, false);
	}
	attrMap.RecomputeAll();
	combat_score_type cScore = LC_Helper::CalcAttributeMapCombatScore(&attrMap, false);
	return iScore - cScore;
}

void LC_ClientGamePlayer::RequestPlayerPKInfo(const StringType& kPlayerID)
{
	MG_REQ_PlayerPkInfo reqMsg;

	unique_id_type iPlayerID = _atoi64(kPlayerID.c_str());
	reqMsg.m_CharID = iPlayerID;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_PLAYER_PK_INFO, &reqMsg);
	if(bCreateMsg)
		SendToServer(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

void LC_ClientGamePlayer::SetPlayerPKDetailInfo(const MG_RLT_PlayerPkInfo& playerInfo)
{
	m_nMyInfo = playerInfo.m_nMyInfo;
	m_nOtherInfo = playerInfo.m_nOtherInfo;
}

float LC_ClientGamePlayer::GetCombatPKMyProgress(int id)
{
	int64_t myValue = 0;
	std::map<uint32_t,uint64_t>::iterator iter = m_nMyInfo.m_kUserAllCombatScoreMap.find(id);
	if (iter != m_nMyInfo.m_kUserAllCombatScoreMap.end())
		myValue = iter->second;

	int64_t otherValue = 0;
	std::map<uint32_t,uint64_t>::iterator itor = m_nOtherInfo.m_kUserAllCombatScoreMap.find(id);
	if (itor != m_nOtherInfo.m_kUserAllCombatScoreMap.end())
		otherValue = itor->second;

	int64_t sumValue = myValue + otherValue;
	if (sumValue <= 0)
		return 50;
	else
		return (myValue * 100) / (float)sumValue;
}

StringType LC_ClientGamePlayer::GetCombatCompareMyInfo()
{
	StringStreamType sStr;

	sStr << m_nMyInfo.m_sCharName << "|" << m_nMyInfo.m_nLevel << "|" << m_nMyInfo.m_nPictureID << "|" << m_nMyInfo.m_nCommanderTitleFrameID << "|" << m_nMyInfo.m_nCommanderTitleID << "|";
	std::map<uint32_t,uint64_t>::iterator iter = m_nMyInfo.m_kUserAllCombatScoreMap.begin();
	for (; iter != m_nMyInfo.m_kUserAllCombatScoreMap.end(); ++iter)
	{
		sStr << iter->second << "|";
	}
	return sStr.str();
}

StringType LC_ClientGamePlayer::GetCombatCompareOtherInfo()
{
	StringStreamType sStr;

	sStr << m_nOtherInfo.m_sCharName << "|" << m_nOtherInfo.m_nLevel << "|" << m_nOtherInfo.m_nPictureID << "|" << m_nOtherInfo.m_nCommanderTitleFrameID << "|" << m_nOtherInfo.m_nCommanderTitleID << "|";
	std::map<uint32_t,uint64_t>::iterator iter = m_nOtherInfo.m_kUserAllCombatScoreMap.begin();
	for (; iter != m_nOtherInfo.m_kUserAllCombatScoreMap.end(); ++iter)
	{
		sStr << iter->second << "|";
	}
	return sStr.str();
}

void LC_ClientGamePlayer::ReqBuyRareSubDevaEyeTransform(int iShopID)
{
	MG_REQ_BuyRareSubDevaEyeTransform reqMsg;
	reqMsg.m_nShopID = iShopID;
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_BUYRARESUBDEVAEYETRANSFORM, &reqMsg);
	if (bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

ResultType LC_ClientGamePlayer::CheckRareSubState(int iShopID)
{
	LC_ShopIDMap kShopIDMap;
	kShopIDMap[iShopID] = 1;

	return LC_Helper::IsShopAvailable(this, kShopIDMap, false, false);
}

void LC_ClientGamePlayer::ReqBestGuildRankInfo(int iType1, int iType2, int iStartIndex, int iEndIndex, const StringType& str)
{
	GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"请求公会战排行榜信息，iType1=%d, iType2=%d, iStartIndex=%d, iEndIndex=%d, ",iType1, iType2, iStartIndex, iEndIndex);
	MG_REQ_GuildFightRankInfo reqMsg;
	reqMsg.m_nType1 = iType1;
	reqMsg.m_nStroyId = iType2;
	reqMsg.m_nMin	= iStartIndex;
	reqMsg.m_nMax	= iEndIndex;
	TStringVector kVector = UT_BaseStringUtility::SplitString(str, '#');
	for (TStringVector::iterator it = kVector.begin(); it != kVector.end(); ++it)
	{
		reqMsg.m_nStageInfo.push_back(String2Int(it->c_str()));
	}
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GUILDFIGHTRANKINFO, &reqMsg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}

void LC_ClientGamePlayer::ReqBestGuildFinishReward()
{
	MG_REQ_GetBestGuildReward reqMsg;
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GETBESTGUILDREWARD, &reqMsg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}

void LC_ClientGamePlayer::ReqGuildRankChallenge(uint64_t iBeChallengeCharID)
{
	MG_REQ_GuildChallenge reqMsg;
	reqMsg.m_beChallengeCharID = iBeChallengeCharID;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_CS_GUILD_RANK_CHALLENGE_REQ, &reqMsg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}

void LC_ClientGamePlayer::ReqGuildFightRobScore(int iOpType, int iNum)
{
	MG_Req_GuildFightRobScore_Opt reqMsg;
	reqMsg.m_OptType = iOpType;
	reqMsg.m_nBuyNum = iNum;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GUILD_FIGHT_ROB_SCORE, &reqMsg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}

void LC_ClientGamePlayer::ReqGuildFightRobScoreReceive(int iCount)
{
	MG_Req_GuildFightRobScoreReceive reqMsg;
	reqMsg.m_ReceiveNum = iCount;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GUILD_FIGHT_ROB_SCORE_RECEIVE, &reqMsg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}

void LC_ClientGamePlayer::ReqGuildFightMemberUIDs(guild_id_type iGuildID)
{
	MG_Req_GuildFightGuildMemberIdInfo reqMsg;
	reqMsg.m_nGuild = iGuildID;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GUILD_FIGHT_GUILD_MEMBER_ID_INFO, &reqMsg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}

void LC_ClientGamePlayer::ReqGuildFightPlayerInfo(const StringType& sUids, bool bCheckTime)
{
	MG_Req_GuildFightPlayerInfo reqMsg;
	reqMsg.m_checkTime = bCheckTime;

	TStringVector kVec = UT_BaseStringUtility::SplitString(sUids, '#');
	TStringVector::const_iterator iter = kVec.begin();
	for( ; iter!=kVec.end(); ++ iter)
	{
		reqMsg.m_IdVec.push_back(UT_BaseStringUtility::toUInt64(*iter));
	}

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GUILD_FIGHT_PLAYER_INFO, &reqMsg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}

void LC_ClientGamePlayer::ReqGuildBattleFightRecord()
{
	MG_REQ_FightRecord reqMsg;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_FIGHTRECORD, &reqMsg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}

void LC_ClientGamePlayer::ReqGuildBattleBeDefightedFlag()
{
	MG_REQ_GetBeDefightedFlag reqMsg;
	
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GETBEDEFIGHTEDFLAG, &reqMsg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}


void LC_ClientGamePlayer::UpdateBestGuildMapBossInfo(std::vector<ST_Alliance_Boss_Info>& m_rInfoList)
{
	SC_ClientScriptInterface* pkClientInterface = SC_ClientScriptInterface::GetSingletonPtr();
	SC_ClientSkillInterface* pkInterface = pkClientInterface->GetSkillInterface();
	int iServerId=pkInterface->GetCurGameServerNode();
 	for (int i=0;i<m_rInfoList.size();i++)
	{
		if(m_rInfoList[i].m_nServerID == iServerId)
		{
			std::list<ST_Alliance_Boss_StateInfo>& pBossInfoList=m_rInfoList[i].m_rBossList;
			for(std::list<ST_Alliance_Boss_StateInfo>::iterator it=pBossInfoList.begin();it!=pBossInfoList.end();++it)
			{
				if ((*it).m_rLocation.m_iMapID==4201)
				{
					m_BestGuildBossInfoMap[(*it).m_nNpcType]=*it;
				}
			}
			return;
		}
	}
}

void LC_ClientGamePlayer::UpdateBestGuildMapBossInfo(ST_Alliance_Boss_Info& m_rInfo)
{
	SC_ClientScriptInterface* pkClientInterface = SC_ClientScriptInterface::GetSingletonPtr();
	SC_ClientSkillInterface* pkInterface = pkClientInterface->GetSkillInterface();
	int iServerId=pkInterface->GetCurGameServerNode();
	if(m_rInfo.m_nServerID == iServerId)
	{
		std::list<ST_Alliance_Boss_StateInfo>& pBossInfoList=m_rInfo.m_rBossList;
		for(std::list<ST_Alliance_Boss_StateInfo>::iterator it=pBossInfoList.begin();it!=pBossInfoList.end();++it)
		{
			if ((*it).m_rLocation.m_iMapID==4201)
			{
				m_BestGuildBossInfoMap[(*it).m_nNpcType]=*it;
			}
		}
	}
}

void LC_ClientGamePlayer::UpdateBestGuildTaskInfo(int TaskCount,bool BestGuildRewardFlag)
{
	m_BestGuildRewardFlag=BestGuildRewardFlag;
	m_BestGuildFinishedTask=TaskCount;
}

StringType LC_ClientGamePlayer::GetGuildBattleBossState(int iMapID, int iType)
{
	StringStreamType sStream("");
	for(mem::map<int,ST_Alliance_Boss_StateInfo>::iterator it=m_BestGuildBossInfoMap.begin();it!=m_BestGuildBossInfoMap.end();++it)
	{
		const ST_Alliance_Boss_StateInfo& pInfo=it->second;
		CF_BestGuildBossInfo::DataEntry* pkDataEntry=SafeGetCSVFileDataEntryPtr<CF_BestGuildBossInfo>(pInfo.m_nNpcType);
		if (pInfo.m_rLocation.m_iMapID == iMapID && pkDataEntry->_lType == iType)
			sStream << pInfo.m_nNpcType << '#' << pInfo.m_nState << '#' << pInfo.m_nRefreshTime<<"#"<<pkDataEntry->_lType1 << '|';
	}
	return sStream.str();
}

void LC_ClientGamePlayer::ReqGuildBattleTowerInfo()
{
	MG_REQ_Guild_Fight_Tower_Info reqMsg;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GUILD_FIGHT_TOWER_INFO, &reqMsg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}

void LC_ClientGamePlayer::SetGuildBattleTowerState(const std::vector<ST_Single_Tower_info>& m_TowerVec)
{
	int TowerCount=0;
	for (int i = 0; i < m_TowerVec.size(); ++i)
	{
		CF_BestGuildBossInfo::DataEntry *pkData=SafeGetCSVFileDataEntryPtr<CF_BestGuildBossInfo>(m_TowerVec[i].m_uTowerID);
		int key = m_TowerVec[i].m_MapId;
		GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM,"SetGuildBattleTowerState::%d  mapid:%d",m_TowerVec[i].m_uTowerID,key);
		if (pkData && key > 0)
		{
			switch(pkData->_lType)
			{
			case 2:
				m_BestGuildTowerInfo[key] = m_TowerVec[i];
				TowerCount++;
				break;
			case 3:
				m_BestGuildFortressInfo[key] = m_TowerVec[i];
				break;
			case 4:
				m_BestGuildRoyalInfo[key] = m_TowerVec[i];
				break;
			}
		}
	}
	GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM,"TowerCount::%d",TowerCount);
}

ST_Single_Tower_info* LC_ClientGamePlayer::GetGuildBattleStateByKey(int iType, int iMapid)
{
	ST_Single_Tower_info* sState = NULL;
	switch(iType)
	{
	case 2:
		{
			mem::map<int, ST_Single_Tower_info>::iterator it = m_BestGuildTowerInfo.find(iMapid);
			if (it != m_BestGuildTowerInfo.end())
				sState = &it->second;
			break;
		}
	case 3:
		{
			mem::map<int, ST_Single_Tower_info>::iterator it = m_BestGuildFortressInfo.find(iMapid);
			if (it != m_BestGuildFortressInfo.end())
				sState = &it->second;
			break;
		}
	case 4:
		{
			mem::map<int, ST_Single_Tower_info>::iterator it = m_BestGuildRoyalInfo.find(iMapid);
			if (it != m_BestGuildRoyalInfo.end())
				sState = &it->second;
			break;
		}
	}
	return sState;
}

void LC_ClientGamePlayer::SetGuildBattleTowerState(const ST_Single_Tower_info_lite& info)
{
	CF_BestGuildBossInfo::DataEntry *pkData=SafeGetCSVFileDataEntryPtr<CF_BestGuildBossInfo>(info.m_uTowerID);
	int key = info.m_MapId;
	if (pkData && key > 0)
	{
		switch(pkData->_lType)
		{
		case 2:
			{
				mem::map<int, ST_Single_Tower_info>::iterator it = m_BestGuildTowerInfo.find(key);
				if (it != m_BestGuildTowerInfo.end())
					it->second.m_MonsterHP = info.m_MonsterHP;
				break;
			}
		case 3:
			{
				mem::map<int, ST_Single_Tower_info>::iterator it = m_BestGuildFortressInfo.find(key);
				if (it != m_BestGuildFortressInfo.end())
					it->second.m_MonsterHP = info.m_MonsterHP;
				break;
			}
		case 4:
			{
				mem::map<int, ST_Single_Tower_info>::iterator it = m_BestGuildRoyalInfo.find(key);
				if (it != m_BestGuildRoyalInfo.end())
					it->second.m_MonsterHP = info.m_MonsterHP;
				break;
			}
		}
	}
}

StringType LC_ClientGamePlayer::GetGuildBattleTowerState(int MapId)
{
	StringStreamType sStream("");
	if (MapId > 0)
	{
		mem::map<int,ST_Single_Tower_info>::iterator iter = m_BestGuildTowerInfo.find(MapId);
		if (iter != m_BestGuildTowerInfo.end())
			sStream << GetGuildBattleStateInfo(iter->second);
	}
	else
	{
		mem::map<int, ST_Single_Tower_info>::iterator iter = m_BestGuildTowerInfo.begin();
		for (; iter!=m_BestGuildTowerInfo.end();++iter)
		{
			sStream << GetGuildBattleStateInfo(iter->second)<< '|';
		}
	} 
	
	return sStream.str();
}

StringType LC_ClientGamePlayer::GetGuildBattleFortressState(int MapId)
{
	StringStreamType sStream("");
	if (MapId > 0)
	{
		mem::map<int,ST_Single_Tower_info>::iterator iter = m_BestGuildFortressInfo.find(MapId);
		if (iter != m_BestGuildFortressInfo.end())
			sStream << GetGuildBattleStateInfo(iter->second);
	}
	else
	{
		mem::map<int, ST_Single_Tower_info>::iterator iter = m_BestGuildFortressInfo.begin();
		for (; iter!=m_BestGuildFortressInfo.end();++iter)
		{
			sStream << GetGuildBattleStateInfo(iter->second) << '|';
		}
	} 

	return sStream.str();
}

StringType LC_ClientGamePlayer::GetGuildBattleRoyalState(int MapId)
{
	StringStreamType sStream("");
	if (MapId > 0)
	{
		mem::map<int,ST_Single_Tower_info>::iterator iter = m_BestGuildRoyalInfo.find(MapId);
		if (iter != m_BestGuildRoyalInfo.end())
			sStream << GetGuildBattleStateInfo(iter->second);
	}
	else
	{
		mem::map<int, ST_Single_Tower_info>::iterator iter = m_BestGuildRoyalInfo.begin();
		for (; iter!=m_BestGuildRoyalInfo.end();++iter)
		{
			sStream << GetGuildBattleStateInfo(iter->second) << '|';
		}
	} 

	return sStream.str();
}

StringType LC_ClientGamePlayer::GetGuildBattleStateInfo(const ST_Single_Tower_info& m_Info)
{
	StringStreamType sStream("");
	sStream << m_Info.m_MapId << '#' << m_Info.m_MonsterId << '#' << m_Info.m_nState << '#' << m_Info.m_GuildId << '#' << m_Info.m_sGuildName << '#' << m_Info.m_nGuildLeaderInfo.m_nPlayerName << '#' 
		<< m_Info.m_NextTimeStamp << '#' <<m_Info.m_x << '#' << m_Info.m_y << '#' << m_Info.m_z << '#' << m_Info.m_uTowerID << '#' << m_Info.m_nGuildLeaderInfo.m_nLevel << '#' 
		<< m_Info.m_nGuildLeaderInfo.m_nTitleID << '#' << m_Info.m_nGuildLeaderInfo.m_nProfilePictureID << '#' << m_Info.m_nGuildLeaderInfo.m_nCommanderTitleFrameID << '#';
	for (int i = 0; i < m_Info.m_Top3DamagePlayerName.size(); i++)
		sStream << m_Info.m_Top3DamagePlayerName[i] << ';';
	sStream << '#';
	sStream << m_Info.m_MonsterHP << '#';
	return sStream.str();
}

void LC_ClientGamePlayer::ReqGuildBattleDemageRankList(int storyId, uint32_t objId, int iType)
{
	MG_GAMESTORY_ACTOR_ENCOUNTED_STAT_REQ_EX reqMsg;
	reqMsg.m_iStroyID = storyId;
	reqMsg.m_uiID = objId;
	reqMsg.m_nDamageType = iType;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_GAMESTORY_KEY_ECOUNTED_STAT_REQ_EX, &reqMsg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}


void LC_ClientGamePlayer::ReqReealtimeCombatRecorder(uint32_t objId)
{
	MG_REQ_RealtimeCombatRecorder reqMsg;
	reqMsg.m_nNpcObjectID = objId;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_REALTIMECOMBATRECORDER, &reqMsg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}

void LC_ClientGamePlayer::GuildBattlePathtoAction(bool bAttack, int charType)
{
	CF_BestGuildBossInfo::DataEntry* pkDataEntry=SafeGetCSVFileDataEntryPtr<CF_BestGuildBossInfo>(charType);
	Utility::UT_Vec3Int pos = pkDataEntry->_kPos;
	PathToPointLocation(pos.x, pos.y, pos.z, true);
	if (bAttack && charType > 0)
		PendingPathAction(PPAT_ATTACK_NPC, charType, false, 0);
}

void LC_ClientGamePlayer::PendingPathAttack(int charType, int pos_x, int pos_y, int pos_z)
{
	PathToPointLocation(pos_x, pos_y, pos_z, true);
	PendingPathAction(PPAT_ATTACK_NPC, charType, false, 0);
}

/*
O 左上角坐标
X 右上角坐标
Y 左下角坐标
P 当前坐标
OutPos 地图比例坐标类似UV;
*/
static NiPoint2 PointO(432.6f,185.5f);
static NiPoint2 PointX(188.1f,389.6f);
static NiPoint2 PointY(627.5f,423.6f);

static void WorldPosToMapPos(NiPoint2& OutPos,
							 const NiPoint2& O,
							 const NiPoint2& X,
							 const NiPoint2& Y,
							 const NiPoint2& P)
{
	NiPoint2 OP=P-O;
	NiPoint2 OX=X-O;
	NiPoint2 OY=Y-O;
	OutPos.x=OP.Dot(OX)/OX.SqrLength();
	OutPos.y=OP.Dot(OY)/OY.SqrLength();
}

static void MapPosToWorldPos(NiPoint2& OutPos,
							 const NiPoint2& O,
							 const NiPoint2& X,
							 const NiPoint2& Y,
							 const NiPoint2& P)
{
	NiPoint2 OP=P-O;
	NiPoint2 OX=X-O;
	NiPoint2 OY=Y-O;
	OutPos = OX*P.x+OY*P.y+O;
}

StringType LC_ClientGamePlayer::GetBestGuildMapBossInfo()
{
	StringStreamType sStream("");
	for(mem::map<int,ST_Alliance_Boss_StateInfo>::iterator it=m_BestGuildBossInfoMap.begin();it!=m_BestGuildBossInfoMap.end();++it)
	{
		const ST_Alliance_Boss_StateInfo& pInfo=it->second;
		int iIconType=0;
		CF_BestGuildBossInfo::DataEntry* pkDataEntry=SafeGetCSVFileDataEntryPtr<CF_BestGuildBossInfo>(pInfo.m_nNpcType);
		if (!pkDataEntry)
		{
			continue;
		}
		if (pkDataEntry->_lType!=1 && pkDataEntry->_lType!=0)
		{
			continue;
		}
		iIconType=pkDataEntry->_lType;



		NiPoint2 MapPosition(0,0);
		NiPoint2 BossLocation(pInfo.m_rLocation.m_iPosX/100,pInfo.m_rLocation.m_iPosY/100);
		WorldPosToMapPos(MapPosition,PointO,PointX,PointY,BossLocation);
		sStream <<pInfo.m_rLocation.m_iPosX<<'#'
				<<pInfo.m_rLocation.m_iPosY<<'#'
				<<pInfo.m_nRefreshTime<<'#'
				<<iIconType<<'#'
				<<MapPosition.x<<'#'
				<<MapPosition.y<<'#'
				<<pInfo.m_nState<<'#';
		CF_CharType::DataEntry* pCharEntry=SafeGetCSVFileDataEntryPtr<CF_CharType>(pInfo.m_nNpcType);
		if (pCharEntry)
		{
			sStream <<pCharEntry->_kCharName<<'#';
		}
		else
		{
			sStream <<"未知#";
		}
		sStream <<pInfo.m_nNpcType<<"#";
		sStream <<'|';
	}
	return sStream.str(); 
}

StringType LC_ClientGamePlayer::BestGuildConvertWorldPosToMapPos(int x,int y)
{
	StringStreamType sStream("");
	NiPoint2 MapPosition(0,0);
	NiPoint2 WorldPosition(x/100.f,y/100.f);
	WorldPosToMapPos(MapPosition,PointO,PointX,PointY,WorldPosition);
	sStream<<MapPosition.x<<'#';
	sStream<<MapPosition.y;
	return sStream.str();
}

StringType LC_ClientGamePlayer::BestGuildConvertMapPoTosWorldPos(float x,float y)
{
	StringStreamType sStream("");
	NiPoint2 MapPosition(x,y);
	NiPoint2 WorldPosition(0.f,0.f);
	MapPosToWorldPos(WorldPosition,PointO,PointX,PointY,MapPosition);
	sStream<<WorldPosition.x*100<<'#';
	sStream<<WorldPosition.y*100;
	return sStream.str();
}

StringType LC_ClientGamePlayer::GetBestGuildPlayerLocation()
{
	StringStreamType sStream("");
	const Utility::UT_Vec3Int& vLocation=GetCurrentLocation();
	NiPoint2 MapPosition(0,0);
	NiPoint2 CurPosition(vLocation.x/100,vLocation.y/100);
	WorldPosToMapPos(MapPosition,PointO,PointX,PointY,CurPosition);
	sStream <<MapPosition.x<<'#'
			<<MapPosition.y;
	return sStream.str();
}

void LC_ClientGamePlayer::ReqRaidBossPublicKillerInfo(int iCharType, int iMapId)
{
	MG_REQ_GetRegularBossKillerInfo reqMsg;
	reqMsg.m_nCharTypeID = iCharType;
	reqMsg.m_nMapID = iMapId;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GETREGULARBOSSKILLERINFO, &reqMsg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}

void LC_ClientGamePlayer::ReqTakeTaskExtraReward()
{
	MG_REQ_GetTaskExtraReward reqMsg;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GETTASKEXTRAREWARD , &reqMsg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}

void LC_ClientGamePlayer::UpdateTaskExtraReward(SyncType InType,SyncOP InOP,std::vector<KeyValue_Base>& InVec)
{
	if (InType==SyncType_All)
	{
		m_mExtraRewardInfo.clear();
	}
	if (InOP==SyncOP_Add)
	{
		for(int i=0;i<InVec.size();i++)
		{
			KeyValue_Base& it=InVec[i];
			m_mExtraRewardInfo[it.key]+=it.value;
		}
	}
	else if(InOP==SyncOP_Del)
	{
		for(int i=0;i<InVec.size();i++)
		{
			KeyValue_Base& it=InVec[i];
			mem::map<int,int>::iterator iter = m_mExtraRewardInfo.find(it.key);
			if (iter!=m_mExtraRewardInfo.end())
			{
				m_mExtraRewardInfo.erase(iter);
			}
		}
	}
}

void LC_ClientGamePlayer::ClearTaskExtraReward()
{
	m_mExtraRewardInfo.clear();
}

StringType LC_ClientGamePlayer::GetTaskExtraReward()
{
	StringType RwdStr;
	RewardMapToRewardString(m_mExtraRewardInfo,RwdStr);
	return RwdStr;
}

StringType LC_ClientGamePlayer::GetBossKillerInfo(int32_t iMapID, int32_t iCharTypeID)
{
	int64_t nTempScore = iMapID;
	int64_t nResult = (nTempScore << 32) | iCharTypeID;
	mem::map<int64_t, StringType>::iterator iter = m_mBossKillerInfo.find(nResult);
	if (iter != m_mBossKillerInfo.end())
		return iter->second;

	return "";
}

void LC_ClientGamePlayer::UpdateBestGuildGoalRecord(const std::vector<PS_GoalInfo>& m_vinfo)
{
	for (int i = 0;i<m_vinfo.size();i++)
	{
		PS_GoalInfo info = m_vinfo[i];
		m_mBestGuildGoalInfo[info.m_nID] = info;
	}
}

int LC_ClientGamePlayer::GetBestGuildGoalRecordProgress(int iID)
{
	mem::map<int, PS_GoalInfo>::iterator iter = m_mBestGuildGoalInfo.find(iID);
	if (iter != m_mBestGuildGoalInfo.end())
		return (int)iter->second.m_nNum;
	return 0;
}

int64_t LC_ClientGamePlayer::GetBestGuildGoalRecordPoint(int iID)
{
	mem::map<int, PS_GoalInfo>::iterator iter = m_mBestGuildGoalInfo.find(iID);
	if (iter != m_mBestGuildGoalInfo.end())
		return iter->second.m_nAccountPoint;
	return 0;
}

void LC_ClientGamePlayer::SyncCmdEquipSuitCollection(const std::map<uint32_t,SingleCollectSuit>& suitMap)
{
	std::map<uint32_t,SingleCollectSuit>::const_iterator iter = suitMap.begin();
	for(; iter!=suitMap.end(); ++iter)
	{
		m_mCommanderEquipSuitCollection[iter->first] = iter->second;
	}
}

void LC_ClientGamePlayer::ReqCmdEquipSuitCollectReward(int iOpType, int iLevel, int iCount)
{
	MGPT_REQ_Collect_Player_Equip_Suit reqMsg;
	reqMsg.m_uOpType = iOpType;
	reqMsg.m_uStepLevel = iLevel;
	reqMsg.m_uType = iCount;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_COLLECT_PLAYER_EQUIP_SUIT, &reqMsg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}

int LC_ClientGamePlayer::GetCmdEquipSuitCollectState(int iLevel)
{
	mem::map<uint32_t, SingleCollectSuit>::const_iterator iter = m_mCommanderEquipSuitCollection.find(iLevel);
	if(iter == m_mCommanderEquipSuitCollection.end())
		return CET_EquipSuitNull;

	return iter->second.uSuitType;
}

int LC_ClientGamePlayer::GetCmdEquipSuitCollectItemState(int iLevel, int iEquipType)
{
	mem::map<uint32_t, SingleCollectSuit>::const_iterator iter = m_mCommanderEquipSuitCollection.find(iLevel);
	if(iter == m_mCommanderEquipSuitCollection.end())
		return CET_EquipNull;

	const SingleCollectSuit& info = iter->second;
	if(info.uSuitType == CET_EquipSuitActive)
		return CET_EquipGot;

	std::map<uint32_t,SingleCollectItem>::const_iterator itr = info.m_mapSingleStempCollect.find(iEquipType);
	if(itr == info.m_mapSingleStempCollect.end())
		return CET_EquipNull;

	return itr->second.uType;
}

StringType LC_ClientGamePlayer::GetCmdEquipSuitCollectRewardStateInfo(int iLevel)
{
	mem::map<uint32_t, SingleCollectSuit>::const_iterator iter = m_mCommanderEquipSuitCollection.find(iLevel);
	if(iter == m_mCommanderEquipSuitCollection.end())
		return "";

	const SingleCollectSuit& info = iter->second;

	StringStreamType sStream;
	std::map<uint32_t,uint32_t>::const_iterator itr = info.m_mapRewardStempCollect.begin();
	for(; itr!=info.m_mapRewardStempCollect.end(); ++itr)
	{
		sStream << itr->first << '#' << itr->second << '|';
	}

	return sStream.str();
}

bool LC_ClientGamePlayer::_isCmdEquipSuitCollectLevelPrompt(const SingleCollectSuit& suitInfo)
{
	bool bAllGot = true;
	std::map<uint32_t,uint32_t>::const_iterator itr = suitInfo.m_mapRewardStempCollect.begin();
	for( ; itr!=suitInfo.m_mapRewardStempCollect.end(); ++itr)
	{
		if(itr->second == CET_RewardReceiveing)
			return true;
		else if(itr->second == CET_RewardNull)
			bAllGot = false;
	}

	return suitInfo.uSuitType == CET_EquipSuitNull && bAllGot;
}

bool LC_ClientGamePlayer::IsCmdEquipSuitCollectLevelPrompt(int iLevel)
{
	mem::map<uint32_t, SingleCollectSuit>::const_iterator iter = m_mCommanderEquipSuitCollection.find(iLevel);
	if(iter == m_mCommanderEquipSuitCollection.end())
		return false;

	return _isCmdEquipSuitCollectLevelPrompt(iter->second);
}

bool LC_ClientGamePlayer::IsCmdEquipSuitCollectPrompt()
{
	mem::map<uint32_t, SingleCollectSuit>::const_iterator iter = m_mCommanderEquipSuitCollection.begin();
	for( ; iter!=m_mCommanderEquipSuitCollection.end(); ++iter)
	{
		if(_isCmdEquipSuitCollectLevelPrompt(iter->second))
			return true;

		if(iter->second.uSuitType == CET_EquipSuitNull)
			break;
	}

	return false;
}

void LC_ClientGamePlayer::SyncResourceRetrieveRecord(const MG_SyncResourceRetrieverRecord& msg)
{
	const std::map<int, int>& recordMap = msg.m_nResourceRetrieverRecord;
	std::map<int, int>::const_iterator iter = recordMap.begin();
	for( ; iter!=recordMap.end(); ++iter)
	{
		m_mResourceRetrieveRecord[iter->first] = iter->second;
	}

	const std::map<int, int>& mapRecord = msg.m_nRewardMapID;
	std::map<int, int>::const_iterator itr = mapRecord.begin();
	for( ; itr!=mapRecord.end(); ++itr)
	{
		m_mResourceRetrieveMapRecord[itr->first] = itr->second;
	}

	m_iResourceRetrieveLevelRecord = msg.m_nRewardPlayerLevel;
}

void LC_ClientGamePlayer::ReqResourceRetrieveReward(const StringType& sIds, bool bSpecial)
{
	MG_REQ_GetResourceRetrieverReward reqMsg;
	reqMsg.m_nSpecialFind = bSpecial;
	
	TStringVector kVec = UT_BaseStringUtility::SplitString(sIds, '#');
	TStringVector::const_iterator iter = kVec.begin();
	for( ; iter!=kVec.end(); ++iter)
	{
		reqMsg.m_nIDVec.push_back(String2Int(*iter));
	}

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GETRESOURCERETRIEVERREWARD, &reqMsg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}

StringType LC_ClientGamePlayer::GetResourceRetrieveRecordInfo()
{
	StringStreamType sStream;
	mem::map<int, int>::const_iterator iter = m_mResourceRetrieveRecord.begin();
	for( ; iter!=m_mResourceRetrieveRecord.end(); ++iter)
	{
		const int iID = iter->first;
		sStream << iID << '#' << iter->second << '|';
	}
	
	return sStream.str();
}

bool LC_ClientGamePlayer::GetResourceRetrievePrompt()
{
	mem::map<int, int>::const_iterator iter = m_mResourceRetrieveRecord.begin();
	for( ; iter!=m_mResourceRetrieveRecord.end(); ++iter)
	{
		if(iter->second > 0)
			return true;
	}

	return false;
}

int LC_ClientGamePlayer::GetResourceRetrieveRecordMapID(int iId)
{
	mem::map<int, int>::const_iterator iter = m_mResourceRetrieveMapRecord.find(iId);
	if(iter == m_mResourceRetrieveMapRecord.end())
		return 0;

	return iter->second;
}

void LC_ClientGamePlayer::SetNewAchievementRewardCord(const std::vector<int32_t>& m_nID)
{
	m_vNewAchievementRewardCord.clear();
	m_vNewAchievementRewardCord.reserve(m_nID.size());
	for(int i=0;i<m_nID.size();i++)
	{
		m_vNewAchievementRewardCord.push_back(m_nID[i]);
	}
}

StringType LC_ClientGamePlayer::GetNewAchievementRewardCord()
{
	StringStreamType sStream;
	for (int i = 0; i < m_vNewAchievementRewardCord.size(); i++)
	{
		sStream << m_vNewAchievementRewardCord[i] << "#";
	}
	return sStream.str();
}

void LC_ClientGamePlayer::ReqNewAchievementReward(int id)
{
	MG_REQ_GetNewAchievementReward reqMsg;
	reqMsg.m_nID = id;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GETNEWACHIEVEMENTREWARD, &reqMsg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}

void LC_ClientGamePlayer::ReqSetGuildProfitHideName(bool bShow)
{
	MG_SetGuildPresentFlag reqMsg;
	reqMsg.m_nShowGuildPresentName = bShow;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_SETGUILDPRESENTFLAG, &reqMsg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

void LC_ClientGamePlayer::ReqGetGuildProfitReward(uint16_t rewardID)
{
	MG_REQ_GetGuildPresentReward reqMsg;
	reqMsg.m_nIDSet.insert(rewardID);

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GETGUILDPRESENTREWARD, &reqMsg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

void LC_ClientGamePlayer::ReqGetGuildProfitRewardOnKeyReceive()
{
	MG_REQ_GetGuildPresentReward reqMsg;
	int num = 0;
	LC_ClientGuildProfitManager *pkGuildProfitManager = LC_ClientGuildProfitManager::GetSingletonPtr();
	mem::vector<ProfitInfo*>& list = pkGuildProfitManager->GetNormalList();
	for (mem::vector<ProfitInfo*>::iterator iter = list.begin(); iter != list.end(); ++iter)
	{
		if (!(*iter)->m_bIsDrawed)
		{
			reqMsg.m_nIDSet.insert((*iter)->m_iProfitID);
			num++;
			if (num >= GetGlobalSetting.ClientMaxPresentNum)
				break;
		}
		else
			break;
	}
	if (reqMsg.m_nIDSet.size() > 0)
	{
		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GETGUILDPRESENTREWARD, &reqMsg);
		if (bCreateMsg)
			SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
}

void LC_ClientGamePlayer::SyncGuildProfitCrystalInfo(const std::map<uint32_t,PS_GuildCrystalInfo>& Info)
{
	m_vGuildProfitCrystal.clear();
	unique_id_type uid = GetCommanderUID();
	std::map<uint32_t,PS_GuildCrystalInfo>::const_iterator iter = Info.begin();
	for( ; iter!=Info.end(); ++iter)
	{
		std::set<uint64_t>::const_iterator it = iter->second.m_nPlayerCharID.find(uid);
		if (it != iter->second.m_nPlayerCharID.end())
		{
			m_vGuildProfitCrystal[iter->first] = iter->second.m_nInvaildTime;
		}
	}

	LC_GameEvent GameEvent;
	GameEvent.SetEventType(GLET_GUILDPROFIT_CRYSTALINFO);
	UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(GameEvent);
}

void LC_ClientGamePlayer::ReqGetGuildCrystalReward()
{
	MG_REQ_GetGuildCrystalReward reqMsg;
	mem::map<uint32_t, uint32_t>::const_iterator iter = m_vGuildProfitCrystal.begin();
	for (; iter != m_vGuildProfitCrystal.end(); ++iter)
	{
		reqMsg.m_nID.push_back(iter->first);
	}

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GETGUILDCRYSTALREWARD, &reqMsg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}

uint32_t LC_ClientGamePlayer::GetGuildProfitCrystalMinInvaildTime()
{
	uint32_t minTime = 0;
	mem::map<uint32_t, uint32_t>::const_iterator iter = m_vGuildProfitCrystal.begin();
	for (; iter != m_vGuildProfitCrystal.end(); ++iter)
	{
		if (minTime <= 0 || iter->second < minTime)
			minTime = iter->second; 
	}
	return minTime;
}

void LC_ClientGamePlayer::SyncGuildBossActivityInfo(const std::map<int32_t, PS_GuildBossInfo>& m_mInfo)
{
	m_mGuildBossActivity.clear();
	std::map<int32_t, PS_GuildBossInfo>::const_iterator iter = m_mInfo.begin();
	for (; iter != m_mInfo.end(); ++iter)
	{
		m_mGuildBossActivity[iter->first] = iter->second;
	}
}

void LC_ClientGamePlayer::SyncGuildBossActivityFinishMap(const std::set<int32_t>& m_sInfo)
{
	m_sGuildBossActivityFinishMap.clear();
	std::set<int32_t>::const_iterator iter = m_sInfo.begin();
	for( ; iter!=m_sInfo.end(); ++iter)
	{
		m_sGuildBossActivityFinishMap.insert(*iter);
	}
}

void LC_ClientGamePlayer::UpdateGuildBossActivityInfo(int iID, PS_GuildBossInfo state)
{
	m_mGuildBossActivity[iID] = state;
}

void LC_ClientGamePlayer::UpdateGuildBossActivityFinishMap(int32_t m_nFinishMapID)
{
	m_sGuildBossActivityFinishMap.insert(m_nFinishMapID);
}

StringType LC_ClientGamePlayer::GetGuildBossActivityState(int MapID)
{
	StringStreamType sStream;
	mem::map<int32_t, PS_GuildBossInfo>::iterator iter = m_mGuildBossActivity.find(MapID);
	if (iter != m_mGuildBossActivity.end())
	{
		sStream << iter->second.m_nState << '#' << iter->second.m_nInvaildTime << '#' << iter->second.m_nBossHP << '#';
	}
	return sStream.str();
}

bool LC_ClientGamePlayer::GetGuildBossActivityPreMapFinish(int MapID)
{
	mem::set<int32_t>::const_iterator it = m_sGuildBossActivityFinishMap.find(MapID);
	if (it != m_sGuildBossActivityFinishMap.end())
	{
		return true;
	}
	return false;
}

int LC_ClientGamePlayer::GetGuildBossActivityOpen()
{
	mem::map<int32_t, PS_GuildBossInfo>::iterator iter = m_mGuildBossActivity.begin();
	for (; iter != m_mGuildBossActivity.end(); ++iter)
	{
		if (iter->second.m_nState == GuildBossState_Alive)
			return iter->first;
	}
	return 0;
}

void LC_ClientGamePlayer::ReqGuildBossActivityOpen(int iID)
{
	MG_REQ_ActivityGuildBossMap reqMsg;
	reqMsg.m_nMapID = iID;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_ACTIVITYGUILDBOSSMAP, &reqMsg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}

void LC_ClientGamePlayer::ReSetGuildBossActivityRankList()
{
	m_vGuildBossActivityRankThree.clear();
	m_vGuildBossActivityRankList.clear();
}

void LC_ClientGamePlayer::SetGuildBossActivityMyRank(const PS_GuildMapBossPlayerInfo& PlayerInfo)
{
	m_sGuildBossActivityMyRank.clear();
	PS_GuildMapBossPlayerInfo& kInfo = m_sGuildBossActivityMyRank;
	kInfo.m_nCharID = PlayerInfo.m_nCharID;
	kInfo.m_nCharName = PlayerInfo.m_nCharName;
	kInfo.m_nLevel = PlayerInfo.m_nLevel;
	kInfo.m_nTitleFrameID = PlayerInfo.m_nTitleFrameID;
	kInfo.m_nPictureID = PlayerInfo.m_nPictureID;
	kInfo.m_nRankIndex = PlayerInfo.m_nRankIndex;
	kInfo.m_nDamageData = PlayerInfo.m_nDamageData;
}

void LC_ClientGamePlayer::SetGuildBossActivityRank(const std::vector<PS_GuildMapBossPlayerInfo>& PlayerInfo, bool rankThree)
{
	m_vGuildBossActivityRankList.clear();
	mem::vector<PS_GuildMapBossPlayerInfo>& kInfo = m_vGuildBossActivityRankList;
	for (int i=0;i<PlayerInfo.size();i++)
	{
		if (i>= 5)
			return;
		kInfo.push_back(PlayerInfo[i]);
	}

	if (rankThree)
	{
		m_vGuildBossActivityRankThree.clear();
		mem::vector<PS_GuildMapBossPlayerInfo>& tInfo = m_vGuildBossActivityRankThree;
		for (int i=0;i<PlayerInfo.size();i++)
		{
			if (i>= 3)
				return;
			tInfo.push_back(PlayerInfo[i]);
		}
	}
}

StringType LC_ClientGamePlayer::GetGuildBossActivityRank(int index, bool rankThree)
{
	StringStreamType sStream("");
	PS_GuildMapBossPlayerInfo* kInfo = NULL;
	if (rankThree && index < m_vGuildBossActivityRankThree.size())
		kInfo = &m_vGuildBossActivityRankThree[index];
	else if(!rankThree && index < m_vGuildBossActivityRankList.size())
		kInfo = &m_vGuildBossActivityRankList[index];
	if (kInfo != NULL && kInfo->m_nDamageData > 0)
	{
		sStream << kInfo->m_nCharID << "#";
		sStream << kInfo->m_nCharName << "#";
		sStream << kInfo->m_nLevel << "#";
		sStream << kInfo->m_nTitleFrameID << "#";
		sStream << kInfo->m_nPictureID << "#";
		sStream << kInfo->m_nRankIndex << "#";
		sStream << kInfo->m_nDamageData << "#";
	}
	return sStream.str();
}

StringType LC_ClientGamePlayer::GetGuildBossActivityMyRank()
{
	StringStreamType sStream("");
	PS_GuildMapBossPlayerInfo& kInfo = m_sGuildBossActivityMyRank;
	if (kInfo.m_nRankIndex >= 0)
	{
		sStream << kInfo.m_nCharID << "#";
		sStream << kInfo.m_nCharName << "#";
		sStream << kInfo.m_nLevel << "#";
		sStream << kInfo.m_nTitleFrameID << "#";
		sStream << kInfo.m_nPictureID << "#";
		sStream << kInfo.m_nRankIndex << "#";
		sStream << kInfo.m_nDamageData << "#";
	}
	return sStream.str();
}

void LC_ClientGamePlayer::ReqGuildBossActivityRank(int MapID, int MinIndex, int MaxIndex)
{
	MG_REQ_GetGuildMapBossRank reqMsg;
	reqMsg.m_nMapID = MapID;
	reqMsg.m_nMinIndex = MinIndex;
	reqMsg.m_nMaxIndex = MaxIndex;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GETGUILDMAPBOSSRANK, &reqMsg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}

void LC_ClientGamePlayer::ReqGuildBossActivityQuit()
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GUILDMAPBOSSEND, NULL);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}

void LC_ClientGamePlayer::ReqGetTreasurePos()
{
	MG_REQ_GetTreasurePos reqMsg;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GETTREASUREPOS, &reqMsg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}

void LC_ClientGamePlayer::RltGetTreasurePos(const MG_RLT_GetTreasurePos& msg)
{
	m_kTreasurePos.Update(msg.m_nMapID, msg.m_nX, msg.m_nY, msg.m_nZ);
}

StringType LC_ClientGamePlayer::GetTreasurePosInfo()
{
	StringStreamType sStream;
	sStream << m_kTreasurePos.iMapResID << '#' << m_kTreasurePos.iX << '#' << m_kTreasurePos.iY << '#' << m_kTreasurePos.iZ;
	return sStream.str();
}

bool LC_ClientGamePlayer::IsCanDigTreasure()
{
	if(GetMapResID() != m_kTreasurePos.iMapResID)
		return false;

	UT_Vec3Int kTreasurePos(m_kTreasurePos.iX, m_kTreasurePos.iY, m_kTreasurePos.iZ);
	return UT_MathBase::LineLengthXYInt(GetCurrentLocation(), kTreasurePos) <= 300;
}

ResultType LC_ClientGamePlayer::CheckTreasureEvents(int iItemID)
{
	SC_ClientScriptInterface* pkClientInterface = SC_ClientScriptInterface::GetSingletonPtr();
	SC_ClientSkillInterface* pkInterface = pkClientInterface->GetSkillInterface();

	ResultType result = RE_TREASURE_ERROR_6;
	CF_TreasureEvent::DataEntryMapExternIterator kIt = CF_TreasureEvent::GetSingletonPtr()->GetIterator();
	for(; kIt.HasMoreElements(); kIt.MoveNext())
	{
		const CF_TreasureEvent::DataEntry* pkData = kIt.PeekNextValuePtr();
		if( iItemID == pkData->_itemID &&
			(0 == pkData->_iScheduleID || pkInterface->IsScheduleValid(pkData->_iScheduleID)))
		{
			const int iAchievementID = pkData->_achievementID;
			if( iAchievementID == 0
				|| IsInFinishAchievementMap(iAchievementID)
				|| IsFinishedAchievement(iAchievementID) )
			{
				result = RE_SUCCESS;
				break;
			}
		}
	}

	return result;
}



void LC_ClientGamePlayer::ReqGameRestartReward(int iId)
{
	MG_REQ_GetGameRestartReward reqMsg;
	reqMsg.m_nID = iId;
	
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GETGAMERESTARTREWARD, &reqMsg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}

void LC_ClientGamePlayer::ReqLotteryDraw(int iId, int iKey)
{
	MG_CS_Lottery_Draw_Req reqMsg;
	reqMsg.m_Id = iId;
	reqMsg.m_Count = iKey;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_CS_Lottery_Draw_Req, &reqMsg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}

void LC_ClientGamePlayer::ReqLotteryExtraReward(int iId, int iParam)
{
	MG_CS_Lottery_ExtraReward_Req reqMsg;
	reqMsg.m_Id = iId;
	reqMsg.m_Param = iParam;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_CS_Lottery_ExtraReward_Req, &reqMsg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}

void LC_ClientGamePlayer::ReqChargeProfitReward(int iId, int iParam)
{
	MG_CS_ChargeProfit_Reward_Req reqMsg;
	reqMsg.m_Id = iId;
	reqMsg.m_Param = iParam;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_SC_ChargeProfit_Reward_Req, &reqMsg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}

void LC_ClientGamePlayer::ReqEquipCommanderEquip(int iSrcIdx)
{
	MG_CS_EquipCommand_Req reqMsg;
	reqMsg.m_PackIndex = iSrcIdx;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_CS_EquipCommand_Req, &reqMsg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}

void LC_ClientGamePlayer::ReqCommanderEquipIntensify(int iIdx,bool bOneKey)
{
	MG_CS_IntensifyCommandEquip_Req reqMsg;
	reqMsg.m_EquipIndex = iIdx;
	reqMsg.m_Type = bOneKey ? 1 : 0;
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_CS_IntensifyCommandEquip_Req, &reqMsg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}

void LC_ClientGamePlayer::ReqCommanderEquipRefine(int iIdx)
{
	MG_CS_RefineCommandEquip_Req reqMsg;
	reqMsg.m_EquipIndex = iIdx;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_CS_RefineCommandEquip_Req, &reqMsg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}

void LC_ClientGamePlayer::ReqCommanderEquipSoul(int iIdx)
{
	MG_CS_SoulCommandEquip_Req reqMsg;
	reqMsg.m_EquipIndex = iIdx;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_CS_SoulCommandEquip_Req, &reqMsg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}

void LC_ClientGamePlayer::ReqEquipSubEquip(int iSubType, int iIdx)
{
	MG_Req_EquipSubItem reqMsg;
	reqMsg.m_nType = iSubType;
	reqMsg.m_nSrcSlotIndex = iIdx;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_EQUIPSUBITEM, &reqMsg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}

void LC_ClientGamePlayer::ReqUnEquipSubEquip(int iSubType, int iIdx)
{
	MG_Req_UnEquipSubItem reqMsg;
	reqMsg.m_nType = iSubType;
	reqMsg.m_nSrcSlotIndex = iIdx;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_UNEQUIPSUBITEM, &reqMsg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}

LC_BackPackEntry* LC_ClientGamePlayer::GetBuddyEquipEntry(int iCid, int iIndex)
{
	LC_ClientBuddyInstance* pkInst = GetBuddyInstance(iCid);
	if(pkInst == NULL)
		return NULL;

	return pkInst->GetEquipEntry(iIndex);
}

void LC_ClientGamePlayer::OnBuddyEquipUpdated()
{
	LC_PackAsset* pkPackAsset = GetPackAsset();
	pkPackAsset->SetDirtyFlag(PACK_TYPE_EQUIP_SLOT, true);
	pkPackAsset->AddDirtyFlagRevisionID(PACK_TYPE_EQUIP_SLOT);
}

bool LC_ClientGamePlayer::ReqTranscribeFightByUID(unique_id_type uiID, int iMapID)
{
	int64_t id = (int64_t)uiID;
	GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI, "战斗回放开始  uiID=%I64d", id);
	//战斗回放 录像准备
	if(LC_ClientArenaPlayBackManager::GetSingletonPtr()->LoadFile(uiID))
	{
		RequestTransferBattlePlaybackMap(iMapID);
		return true;
	}

	LC_ClientReportManager *pkReportManager = LC_ClientReportManager::GetSingletonPtr();
	int iType = pkReportManager->GetOptionStateById(uiID);
	switch (iType)
	{
	case STATE_NONE:
		{
			if (!pkReportManager->AddReportList(id, false))
			{
				SendLogicInfoMessage(RE_REPORT_OPTION_RESULT_FULL);
			}
		}
		break;
	case STATE_DOWNLOAD:
		SendLogicInfoMessage(RE_REPORT_OPTION_RESULT_DOWNLOADING);
		break;
	case STATE_WAIT:
		SendLogicInfoMessage(RE_REPORT_OPTION_RESULT_WAITING);
		break;
	case STATE_FAIL:
		{
			if (!pkReportManager->AddReportList(id, false))
			{
				SendLogicInfoMessage(RE_REPORT_OPTION_RESULT_FULL);
			}
		}
		break;
	default:
		break;
	}
	return false;
}

void LC_ClientGamePlayer::ReqAddNoviceTutorial(int noviceId)
{
	MG_REQ_AddNoviceTutorial reqMsg;
	reqMsg.m_nNoviceTutorialID = noviceId;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_ADDNOVICETUTORIAL, &reqMsg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}

void LC_ClientGamePlayer::ReqGetNoviceTutorialReward(int noviceId)
{
	MG_REQ_GetNoviceTutorialReward reqMsg;
	reqMsg.m_nNoviceTutorialID = noviceId;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_GETNOVICETUTORIALREWARD, &reqMsg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}

void LC_ClientGamePlayer::SyncNoviceTutorialInfo(const std::vector<uint32_t> &m_vGetRewardNoviceTutorial,const std::vector<uint32_t> &m_vNoviceTutorialRecord)
{
	m_TutorialRewardId = m_vGetRewardNoviceTutorial;
	int max=0 ; 
	if(m_vNoviceTutorialRecord.size()>0)
	{
		 max=m_vNoviceTutorialRecord[0] ; 
		for (int i=0;i<m_vNoviceTutorialRecord.size();++i)
		{
			if(max<m_vNoviceTutorialRecord[i]) {
				max=m_vNoviceTutorialRecord[i] ;
			}
		}
	}
	SetTutorialProgress(max);
}

void LC_ClientGamePlayer::AddRewardRecord(int tutorialId) {

	m_TutorialRewardId.push_back(tutorialId);
}
bool LC_ClientGamePlayer::IsAchievedReward(int tutorialId) {
	for (int i=0;i<m_TutorialRewardId.size();++i)
	{
		if(m_TutorialRewardId[i]==tutorialId) {
			return true ;
		}
	}
	return false ; 
}

void LC_ClientGamePlayer::ReqGetSkyArenaBattleInfo(bool bRefresh, int MapID, int iSchemeType)
{
	SC_ClientScriptInterface* pkClientInterface = SC_ClientScriptInterface::GetSingletonPtr();
	SC_ClientSkillInterface* pkInterface = pkClientInterface->GetSkillInterface();

	MG_Req_SkyArenaBattlePlays reqMsg;
	reqMsg.m_bRefresh = bRefresh;
	reqMsg.m_nMapID = MapID;
	reqMsg.m_nSchemeType = iSchemeType;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_CS_GetSkyArena_BattleInfo, &reqMsg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}

void LC_ClientGamePlayer::ReqSetMapBossFlag(int m_nMapID,int m_nCharTypeID,bool m_bFlag)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_SetMapBossFlag reqMsg;
	reqMsg.m_serverPlayerBossFlagData.m_nMapID  = m_nMapID;
	reqMsg.m_serverPlayerBossFlagData.m_nCharTypeID = m_nCharTypeID;
	reqMsg.m_serverPlayerBossFlagData.m_bFlag = m_bFlag;

	bCreateMsg = CreateMsgPacketHandler(hMsgHandler,MGPT_REQ_SETMAPBOSSFLAG, &reqMsg);

	if(bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::SetBossFlagList(std::vector<PB_ServerPlayerBossFlagData>  &PlayerBossFlagInfo)
{
	m_vServerPlayerBossFlagInfo = PlayerBossFlagInfo;
}
StringType LC_ClientGamePlayer::GetBossFlagList()
{
	StringStreamType sStream("");
	for (int i=0;i<m_vServerPlayerBossFlagInfo.size();++i)
	{
		PB_ServerPlayerBossFlagData &bossFlag = m_vServerPlayerBossFlagInfo[i];
		sStream << bossFlag.m_nMapID << "|" << bossFlag.m_nCharTypeID<<"|"<< bossFlag.m_bFlag <<"#";
	}

	return sStream.str();
}

void LC_ClientGamePlayer::SetMapEnterRecord(int iMapResID, int iCount)
{
	m_kEnterMapRecord[iMapResID] = iCount; 
}

int LC_ClientGamePlayer::GetMapEnterRecord(int iMapResID)
{
	mem::map<int, int>::const_iterator iter = m_kEnterMapRecord.find(iMapResID);
	if(iter == m_kEnterMapRecord.end())
		return 0;

	return iter->second;
}

void LC_ClientGamePlayer::ReqGamestoryBatch(int iMapResID)
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_GET_GameStoryBatch reqMsg;
	reqMsg.m_nResMapID  = iMapResID;

	bCreateMsg = CreateMsgPacketHandler(hMsgHandler,MGPT_GET_GAMESTORYBATCH, &reqMsg);

	if(bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::SyncCanEnterMapIDSet(const std::vector<int32_t>& vec)
{
	m_kCanEnterMapIDSet.clear();
	
	std::vector<int32_t>::const_iterator iter = vec.begin();
	for( ; iter!=vec.end(); ++iter)
	{
		const int32_t iValue = *iter;
		if(iValue > 0)
			m_kCanEnterMapIDSet.insert(iValue);
	}
}

bool LC_ClientGamePlayer::_isInCanEnterMapIDSet(int iMapResID)
{
	mem::set<int>::const_iterator iter = m_kCanEnterMapIDSet.find(iMapResID);
	return iter != m_kCanEnterMapIDSet.end();
}

void LC_ClientGamePlayer::AddCanEnterMapID(int iMapResID)
{
	m_kCanEnterMapIDSet.insert(iMapResID);
}

void LC_ClientGamePlayer::ReqAddCanEnterMapID(int iMapResID)
{
	if(_isInCanEnterMapIDSet(iMapResID))
		return;

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_AddID reqMsg;
	reqMsg.m_nType =  IDType_1;
	reqMsg.m_nValue = iMapResID;

	bCreateMsg = CreateMsgPacketHandler(hMsgHandler,MGPT_REQ_ADDID, &reqMsg);

	if(bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}
void LC_ClientGamePlayer::ReqSkyArenaGradeAward(int m_nGrade) {
	MG_Req_SkyArenaGradeAward reqMsg;
	reqMsg.m_nGrade = m_nGrade;
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_CS_GetSkyArena_GradeAward, &reqMsg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}


void LC_ClientGamePlayer::ReqSkyArenaMyScore() {
	MG_Req_SkyArenaMyScore reqMsg;
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_CS_GetSkyArena_GradeAwardInfo, &reqMsg);
	if (bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}

bool LC_ClientGamePlayer::RewardIsAchieved(int rewardId) {
	int result=m_iSkyArenaRankRewardId&(1<<rewardId);  
    return   result>0 ;  
}

void LC_ClientGamePlayer::ReqSkyArenaCheckBattleStage(unique_id_type uid, int iMapResID, int iSchemeType)
{
	MG_Req_SkyArenaCheckBattleStage reqMsg;
	reqMsg.m_nChallengeId = uid;
	reqMsg.m_nMapID = iMapResID;
	reqMsg.m_nSchemeType = iSchemeType;

	MG_PACKET_ENCODE_HANDLER  hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler,MGPT_CS_SKYARENA_CHECKBATTLESTAGE_REQ, &reqMsg);
	if(bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}


void LC_ClientGamePlayer::ReqGetPlayerSchemeData(int32_t SystemType, int m_nMapID,int m_nSchemeID,int m_nSchemeType, uint64_t m_nCharID)
{
	MG_PACKET_ENCODE_HANDLER  hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_GetPlayerSchemeData  reqMsg;
	reqMsg.m_nSystemType = SystemType;
	reqMsg.m_nMapID  = m_nMapID;
	reqMsg.m_nSchemeID = m_nSchemeID;
	reqMsg.m_nSchemeType = m_nSchemeType;
	reqMsg.m_nCharID = m_nCharID;

	bCreateMsg = CreateMsgPacketHandler(hMsgHandler,MGPT_REQ_GETPLAYERSCHEMEDATA, &reqMsg);

	if(bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::ReqGetRankInfo(uint32_t m_nRankID,uint64_t m_nCharID,int m_nZoneId)
{
	MG_PACKET_ENCODE_HANDLER  hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_GetRankTypeInfo   reqMsg;
	reqMsg.m_nRankID = m_nRankID;
	reqMsg.m_nCharID = m_nCharID;
	reqMsg.m_nZoneId = m_nZoneId;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler,MGPT_REQ_GETRANKTYPEINFO, &reqMsg);

	if(bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::SetSkyArenaSettleReward(std::vector<PS_ItemRecord>& RwdInfo)
{
	m_SoloRwd = RwdInfo;
}

StringType LC_ClientGamePlayer::GetSkyArenaSettleReward()
{
	StringStreamType sStream;
	for (int i=0;i<m_SoloRwd.size();i++)
	{
		sStream<<m_SoloRwd[i].m_iItemID<<":"<<m_SoloRwd[i].m_iItemCount<<";";
	}
	return sStream.str();
}

void LC_ClientGamePlayer::SetSkyArenaPoint(int iPoint)
{
	m_iSkyArenaPoint = iPoint;
	if(m_iLastSkyArenaPoint == 0)
		m_iLastSkyArenaPoint = m_iSkyArenaPoint;
}

void LC_ClientGamePlayer::SetSkyArenaCommanderBaseData(MG_NOTIFY_SkyAllShowInfo& msg)
{
	m_SkyArenaSettle.SetUserBaseData(msg.m_Left.ControllerBase, true);
	m_SkyArenaSettle.SetUserBaseData(msg.m_Right.ControllerBase, false);
}

void LC_ClientGamePlayer::SetSkyArenaSettleReportList(std::vector<BaseBattlefieldReport>& info)
{
	m_SkyArenaSettle.SetDeatil(info, true);
}





//巅峰请求界面信息
void LC_ClientGamePlayer::ReqSummitArenaInfo(){
	MG_Req_SummitArenaInfo reqMsg ; 
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool  bCreateMsg = CreateMsgPacketHandler(hMsgHandler,MGPT_CS_SUMMIT_ARENA_INFO_REQ, &reqMsg);

	if(bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

//巅峰真实参赛列表
void LC_ClientGamePlayer::ReqSummitArenaGroupStageEntryList() {
	MG_Req_SummitArenaGroupStageEntryList reqMsg ; 
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool  bCreateMsg = CreateMsgPacketHandler(hMsgHandler,MGPT_CS_SUMMIT_ARENA_ENTRYLIST_REQ, &reqMsg);

	if(bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}



//巅峰竞技场小组战况 
void LC_ClientGamePlayer::ReqSummitArenaGroupStage() {
MG_Req_SummitArenaGroupStage reqMsg ; 
MG_PACKET_ENCODE_HANDLER hMsgHandler;
bool  bCreateMsg = CreateMsgPacketHandler(hMsgHandler,MGPT_CS_SUMMIT_ARENA_GROUPSTAGE_REQ, &reqMsg);

if(bCreateMsg)
{
	SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}

}

//巅峰竞技场本组战斗
void LC_ClientGamePlayer::ReqSummitArenaMyGroupStage(int mapId,int schemeType) {
	MG_Req_SummitArenaMyGroupStage reqMsg ; 
	reqMsg.m_nMapID=mapId ; 
	reqMsg.m_nSchemeType=schemeType ; 
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool  bCreateMsg = CreateMsgPacketHandler(hMsgHandler,MGPT_CS_SUMMIT_ARENA_MY_INFO_REQ, &reqMsg);

	if(bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

//得到巅峰剩余时间 
void LC_ClientGamePlayer::GetSummitArenaTime(SummitArenaTimeData& timeData) {
	m_showRemainTime=timeData.nShowTime ; 
	m_battleRemainTime=timeData.nStageBattleTime;  
	m_stageEndRemainTime=timeData.nStageEndTime; 
	m_stageSetRemainTime=timeData.nStageSetEndTime;  
}

//得到巅峰相关时间 

void LC_ClientGamePlayer::GetSummitArenaTimeStamp(SummitArenaTimeData& timeData) {
	m_iStartTime=timeData.nStartTime ;  //巅峰竞技场开赛时间
	m_iEndTime=timeData.nEndTime ;//巅峰竞技场结束时间 
	m_stageType= timeData.nStageTimeType ;//时间阶段枚举
	m_battleStTime=timeData.nStageBattleTime ; //战斗开始时间 
	m_stageStTime=timeData.nStageStartTime ; //阶段开始时间 
	m_stageEndTime=timeData.nStageEndTime ; //阶段结束时间 
	m_stageSetEndTime=timeData.nStageSetEndTime ;//阵容设置结束时间 
	m_showTime=timeData.nShowTime ; //展示时间
	m_zoneId=timeData.nZoneId ;   //战区Id  
}

//请求64强 
void LC_ClientGamePlayer::ReqSummitArenaAgainstList(int zoneId,int groupId,int type){
	MG_Req_SummitArenaAgainstList reqMsg ; 
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	reqMsg.nZoneId=zoneId ; 
	reqMsg.nGroupId=groupId ; 
	reqMsg.m_nType=type ; 
	bool  bCreateMsg = CreateMsgPacketHandler(hMsgHandler,MGPT_CS_SUMMIT_ARENA_AGAINSTLIST_REQ, &reqMsg);

	if(bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}



//Set组数据
void LC_ClientGamePlayer::SetAllGroupData(const std::vector<SummitArenaGroupStageData> & group){
	  for (int i=0;i<group.size();++i)
	  {
		  int groupId=group[i].nGroupId ; 
		  allGroupData[groupId]=group[i].groupdata;
	  }
  } 

//Get组的数据
StringType LC_ClientGamePlayer::GetEveryGroupData(int groupId) {
	StringStreamType sStream("");
	//std::vector<SummitArenaGroupStageUserData> & groupData = allGroupData.find(groupId); 
	m_groupDatas::iterator it ;
	it=allGroupData.find(groupId); 
	if (it!=allGroupData.end()) {
std::vector<SummitArenaGroupStageUserData> & groupData =it->second ;
for (int i=0;i<groupData.size();++i)
{
	SummitArenaGroupStageUserData  & data=groupData[i] ; 
	PS_Rank_PlayerBaseData & info=data.m_basedata ;
	sStream<<info.m_Robot<<"#"<<info.m_CommanderTitleID<<"#"<<info.m_CommanderTitleFrameID<<"#"<<info.m_ProfilePictureID<<"#"<<info.m_Level<<"#"<<info.m_CharID<<"#"<<info.m_CommanderScore<<"#"
		<<info.m_CharName<<"#"<<info.m_nGuildName<<"#"<<data.nScore<<"#"<<data.nWinTimes<<"#"<<data.nLostTimes<<"#"<<data.nLeftChallengeTimes<<"#"<<data.nLastSkyArenaRankId<<"#"<<data.nScoreTime<<"|";
}
	}		
	return sStream.str() ; 
}


//设置我的组
void LC_ClientGamePlayer::SetMyGroupData(const std::vector<SummitArenaGroupStageNeedBuddyData> & groupDatas) {
	myGroupData.clear(); 
	for(int i=0;i<groupDatas.size();++i) {
	const SummitArenaGroupStageUserData & data=groupDatas[i].groupdata ; 
	myGroupData.push_back(data) ;
	myGroupData[i].m_basedata.m_CommanderScore=0 ; 
	const PS_AllSchemeBuddyData &  schemeData=groupDatas[i].m_SchemeData ; 
	const PS_Rank_PlayerBaseData & pData=data.m_basedata ;
	const std::vector<PS_SchemeBuddyData> & psData=	schemeData.m_vSchemeData ; 
	for (int j=0;j<psData.size();++j)
	{
		const std::vector<PS_Buddy_Info_BuddyCS>& bData= psData[j].m_nBuddyData ;
		for (int k=0;k<bData.size();++k)
		{
			const PS_Buddy_Info_BuddyCS &cData= bData[k];
			SummitArenaGroupStageUserData& curData= myGroupData[i];
			curData.m_basedata.m_CommanderScore+=cData.m_nBuddyCombatScore ; 
			//pData.m_CommanderScore+=cData.m_nBuddyCombatScore ; 
		}
	}
}
}

//Get我的组 
StringType LC_ClientGamePlayer::GetMyGroupData() {
	StringStreamType sStream("") ; 
	const std::vector<SummitArenaGroupStageUserData> & gData=myGroupData ;
	//std::vector<SummitArenaGroupStageUserData> &  userData=gData.groupdata ; 
		for (int i=0;i<gData.size();++i)
		{
			const SummitArenaGroupStageUserData & data=gData[i] ; 
			const PS_Rank_PlayerBaseData & info=data.m_basedata;
			sStream<<info.m_Robot<<"#"<<info.m_CommanderTitleID<<"#"<<info.m_CommanderTitleFrameID<<"#"<<info.m_ProfilePictureID<<"#"<<info.m_Level<<"#"<<info.m_CharID<<"#"<<info.m_CommanderScore<<"#"
				<<info.m_CharName<<"#"<<info.m_nGuildName<<"#"<<data.nScore<<"#"<<data.nWinTimes<<"#"<<data.nLostTimes<<"#"<<data.nLeftChallengeTimes<<"#"<<data.nLastSkyArenaRankId<<"#"<<data.nScoreTime<<"|"  ;
		}
		return sStream.str() ; 
}
//Get参赛列表 某范围的人 
 
StringType LC_ClientGamePlayer::GetEntryListData() {

	StringStreamType sStream("") ; 
	SummitArenaGroupStageData & data=EntryList ; 
	std::vector<SummitArenaGroupStageUserData> & userData=data.groupdata;  
	for (int i=0;i<userData.size();++i)
	{
		SummitArenaGroupStageUserData & data=userData[i] ; 
		PS_Rank_PlayerBaseData & info=userData[i].m_basedata ;
		sStream<<info.m_Robot<<"#"<<info.m_CommanderTitleID<<"#"<<info.m_CommanderTitleFrameID<<"#"<<info.m_ProfilePictureID<<"#"<<info.m_Level<<"#"<<info.m_CharID<<"#"<<info.m_CommanderScore<<"#"
			<<info.m_CharName<<"#"<<info.m_nGuildName<<"#"<<data.nScore<<"#"<<data.nWinTimes<<"#"<<data.nLostTimes<<"#"<<data.nLeftChallengeTimes<<"#"<<data.nLastSkyArenaRankId<<"#"<<data.nScoreTime<<"|"  ;
	}
	return sStream.str() ; 
}


void LC_ClientGamePlayer::SetAgainstGroup(const std::vector<SummitArenaAgainstList>& againstData,int groupId) {
		 againstList[groupId]=againstData ; 
}

StringType LC_ClientGamePlayer::GetAgainstData(int groupId) {
	StringStreamType sStream("") ; 
	//std::vector<SummitArenaAgainstList> & list=againstList[groupId] ;  
	againstDatas::iterator it ; 
	it= againstList.find(groupId); 
	if(it!=againstList.end()) {
		std::vector<SummitArenaAgainstList> & list =it->second ;
		for (int i=0; i<list.size();++i)
		{
			std::vector<PS_Rank_PlayerBaseData> & data=list[i].m_basedatas ;
			sStream<<list[i].nGroupId<<"#"<<list[i].nLayIndx<<"#"<<list[i].nWinCharId<<"#"<<list[i].m_nUniqueId<<"|" ; 
			for (int j=0;j<data.size();++j)
		 {
			 PS_Rank_PlayerBaseData & pData=data[j];
			 //某一个对阵列表的两个人  
			 sStream<<pData.m_Robot<<"#"<<pData.m_CommanderTitleID<<"#"<<pData.m_CommanderTitleFrameID<<"#"<<pData.m_ProfilePictureID<<"#"<<pData.m_Level<<"#"<<pData.m_CharID<<"#"<<pData.m_CommanderScore<<"#"<<pData.m_CharName<<"#"<<pData.m_nGuildName<<";" ; 
		 }
			sStream<<":" ; 
		}
	}
	 return sStream.str();
}
//巅峰竞技场邀请函

void LC_ClientGamePlayer::GetSummitArenaInvitationValid(int type,bool isValid) {
 	m_invitedType=type ; 
	m_isValid=isValid ; 
}


void LC_ClientGamePlayer::ReqGetSummitSingleData(uint32_t  m_nSummitType)
{
	MG_PACKET_ENCODE_HANDLER  hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_GetSummitSingleData reqMsg;
	reqMsg.m_nSummitType = m_nSummitType;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler,MGPT_REQ_GETSUMMITSINGLEDATA, &reqMsg);

	if(bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::ReqGetSummitSupportInfo(uint32_t  m_nSummitType,uint32_t m_nAgainstID)
{
	MG_PACKET_ENCODE_HANDLER  hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_GetSummitSupportInfo reqMsg;
	reqMsg.m_nSummitType = m_nSummitType;
	reqMsg.m_nAgainstID = m_nAgainstID;

	bCreateMsg = CreateMsgPacketHandler(hMsgHandler,MGPT_REQ_GETSUMMITSUPPORTINFO, &reqMsg);

	if(bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::ReqSetSummitGuessInfo(uint32_t  m_nSummitType,uint32_t m_nAgainstID,uint64_t m_nChoseCharID)
{
	MG_PACKET_ENCODE_HANDLER  hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_SetSummitGuessInfo reqMsg;

	reqMsg.m_nSummitType = m_nSummitType;
	reqMsg.m_nAgainstID = m_nAgainstID;
	reqMsg.m_nChoseCharID = m_nChoseCharID;

	bCreateMsg = CreateMsgPacketHandler(hMsgHandler,MGPT_REQ_SETSUMMITGUESSINFO, &reqMsg);

	if(bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::ReqGetSummitWinner(uint32_t m_nSummitType,uint32_t m_nAgainstID)
{
	MG_PACKET_ENCODE_HANDLER  hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_GetSummitWiner reqMsg;
	reqMsg.m_nSummitType = m_nSummitType;
	reqMsg.m_nAgainstID = m_nAgainstID;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler,MGPT_REQ_GETSUMMITWINER, &reqMsg);

	
	if(bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::SetGuessLog()
{
	LC_ClientGuessDataBase* GuessData = GetGuessBaseData();

	if (GuessData->m_nSummitType != GetStageType())
	{
		std::vector<LC_ClientGuessDataBase>* GuessDataLog = GetGuessLogData();
		GuessDataLog->push_back(*GuessData);
		GuessData->Reset();
	}
	return;
}

void LC_ClientGamePlayer::ReqSummitArenaHonor(int nZoneId,int nIndex)
{
	MG_PACKET_ENCODE_HANDLER  hMsgHandler;
	bool bCreateMsg = false;

	MG_Req_SummitArenaHonorMsg reqMsg;
	reqMsg.nIndex = nIndex;
	reqMsg.nZoneId = nZoneId;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler,MGPT_CS_SUMMIT_ARENA_HONOR_REQ, &reqMsg);

	if(bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::ReqGetMyTopHonor()
{
	MG_PACKET_ENCODE_HANDLER  hMsgHandler;
	bool bCreateMsg = false;

	MG_Req_SummitArenaMyTopHonorMsg reqMsg;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler,MGPT_CS_SUMMITARENA_MY_HONORTOPID_REQ, &reqMsg);

	if(bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

StringType LC_ClientGamePlayer::GetTopPlayerInfo(int index)
{
	if (index < 0 || index >= TopPlayerList.size())
	{
		return "";
	}
	const SummitArenaHonorDataMsg& TopPlayer = TopPlayerList[index];
	const PS_Rank_PlayerBaseData& PlayerInfo = TopPlayer.m_basedata;
	StringStreamType sStream("");
	sStream<<TopPlayer.nStageType<<"|"<<PlayerInfo.m_CharID<<"|"<<PlayerInfo.m_CharName<< "|" <<PlayerInfo.m_CommanderTitleID<<"|"<<PlayerInfo.m_CommanderTitleFrameID<<"|"<<PlayerInfo.m_ProfilePictureID<<"|"<<PlayerInfo.m_Level<<"|";
	return sStream.str();
}

void LC_ClientGamePlayer:: SetPlayerList(std::vector<SummitArenaHonorDataMsg>&	topPlayerList)
{
	TopErr = 0;		//0:无异常
	TopPlayerList.swap(std::vector<SummitArenaHonorDataMsg>());
	if(topPlayerList.size() < 8)
	{
		TopErr = TOPRANKINGERROR_ERROR_NUM;//1:8强数据不足8人
	}
	int count = topPlayerList.size();
	int i;
	for (i = 0 ; i < count; i++)
	{
		if (topPlayerList[i].nStageType == -1)
		{
			if(!topPlayerList[i].m_basedata.m_Robot)	
				TopPlayerList.push_back(topPlayerList[i]);
			break;
		}
	}

	for (i = 0 ; i < count; i++)
	{
		if (topPlayerList[i].nStageType == Top2MatchStartType)
		{
			if(!topPlayerList[i].m_basedata.m_Robot)	
				TopPlayerList.push_back(topPlayerList[i]);
			break;
		}
	}

	for (i = 0 ; i < count; i++)
	{
		if (topPlayerList[i].nStageType == Top4MatchStartType)
		{
			if(!topPlayerList[i].m_basedata.m_Robot)	
				TopPlayerList.push_back(topPlayerList[i]);
		}
	}

	for (i = 0 ; i < count; i++)
	{
		if (topPlayerList[i].nStageType == Top8MatchStartType)
		{
			if(!topPlayerList[i].m_basedata.m_Robot)	
				TopPlayerList.push_back(topPlayerList[i]);
		}
	}

}

int LC_ClientGamePlayer:: GetLogMaxIndex()
{
	int max = -1;
	int count = m_nSummitGuessLog.size();
	for (int i = 0; i < count; i++)
	{
		LC_ClientGuessDataBase& GuessData = m_nSummitGuessLog[i];
		if (GuessData.m_nChoseCharID == 0)
		{
			continue;
		}
		max = i;

	}
	return max;

}
LC_ClientGuessDataBase* LC_ClientGamePlayer::GetGuessLogDataByIndex(int i)
{
	if ( i >= 0 && i < m_nSummitGuessLog.size())
	{
		return &m_nSummitGuessLog[i];
	}
	return NULL;
}
//极品白拿 
void LC_ClientGamePlayer::ReqBuyNoPayItem(int id) {
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_BuyNonpareilItem reqMsg;
	reqMsg.m_nID  = id;

	bCreateMsg = CreateMsgPacketHandler(hMsgHandler,MGPT_REQ_BUYNONPAREILITEM, &reqMsg);

	if(bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}
void LC_ClientGamePlayer::SetBuyNoPayList(const std::vector<KeyValue_Base_Bool> & buyNoPayInfo ) {
	for (int i=0;i<buyNoPayInfo.size();++i)
	{
		 const KeyValue_Base_Bool & data=buyNoPayInfo[i] ; 
		 m_nBuyNoPayList[data.key]=data.value ; 
	}
}

void LC_ClientGamePlayer::UpdateBuyNoPayList(int id,bool isBuy){
	mem::map<int,bool>::iterator it =m_nBuyNoPayList.find(id); 
	if (it == m_nBuyNoPayList.end()) {
		m_nBuyNoPayList.insert(make_pair(id,isBuy)); 
	}
}

void LC_ClientGamePlayer::ReqGetBuyNoPayReturn(int id){
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_GetNonpareilReturn reqMsg;
	reqMsg.m_nID  = id;

	bCreateMsg = CreateMsgPacketHandler(hMsgHandler,MGPT_REQ_GETNONPAREILRETURN, &reqMsg);

	if(bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::UpdateBuyNoPayReturn(int id,bool result) {
	mem::map<int,bool>::iterator it=m_nBuyNoPayList.find(id);
	if(it!=m_nBuyNoPayList.end()) {
		m_nBuyNoPayList[id]=result ; 
	}
}

StringType LC_ClientGamePlayer::GetBuyNoPayResult() {
	StringStreamType sStream("");
	mem::map<int,bool>::iterator it ; 
	for ( it=m_nBuyNoPayList.begin();it!=m_nBuyNoPayList.end();++it)
	{
		sStream<<it->first<<"-"<<it->second<<"|" ; 
	}
	return sStream.str() ; 
}

void LC_ClientGamePlayer::ReqGetGuildDamageRank(int32_t m_nStoryID,int32_t m_nMinIndex,int32_t m_nMaxIndex){
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	MG_REQ_GetGuildDamageRank reqmsg;
	reqmsg.m_nStoryID = m_nStoryID;
	reqmsg.m_nMinIndex = m_nMinIndex;
	reqmsg.m_nMaxIndex = m_nMaxIndex;
	bool bCreateMsg = false;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler,MGPT_REQ_GETGUILDDAMAGERANK, &reqmsg);

	if(bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

void LC_ClientGamePlayer::ReqGetGuilfBossBeFallDemageRankInfo(int32_t iMapID)
{
	MG_REQ_GetDamageRankInfo reqmsg;
	reqmsg.m_nMapResID = iMapID;

	bool bCreateMsg = false;
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler,MGPT_REQ_DAMAGERANKINFO, &reqmsg);
	if(bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}

void LC_ClientGamePlayer::ReqGetGuilfBossBeFallGuildRankInfo(int32_t iMapID)
{
	MG_REQ_GetGuildDamageRankInfo reqmsg;
	reqmsg.m_nMapResID = iMapID;

	bool bCreateMsg = false;
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler,MGPT_REQ_GETGUILDDAMAGERANKINFO, &reqmsg);
	if(bCreateMsg)
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
}

StringType LC_ClientGamePlayer::GetGuildDamageRank()
{
	StringStreamType sStream("");

	std::vector<PS_GuildDamageInfo>::const_iterator iter = m_nGuildInfo.begin();
	for (;iter!=m_nGuildInfo.end();iter++)
	{
		const PS_GuildDamageInfo &info = *iter;
		sStream<<info.m_nRankIndex<<"|";
		sStream<<info.m_nGuildName<<"|";
		sStream<<info.m_nGuildID<<"|";
		sStream<<info.m_nDamage<<"#";
	}
	return sStream.str() ; 

}

//篝火晚会  请求剩余购买次数
void LC_ClientGamePlayer::ReqBonFirePartyLeftBuyItemTimes() {
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_BonFireParty_LeftBuyItemTimes reqMsg;

	bCreateMsg = CreateMsgPacketHandler(hMsgHandler,MGPT_REQ_BONFIREPARTY_BUYLEFTITEMTIMES, &reqMsg);

	if(bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}

//篝火晚会  请求

void LC_ClientGamePlayer::ReqBonFirePartyBuyInfo(int buyTimes) {
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	MG_REQ_BonFireParty_BuyItem reqMsg;
	reqMsg.m_nBuyCount=buyTimes ; 

	bCreateMsg = CreateMsgPacketHandler(hMsgHandler,MGPT_REQ_BONFIREPARTY_BUYLEFTITEM, &reqMsg);

	if(bCreateMsg)
	{
		SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
	}
}


StringType LC_ClientGamePlayer::GetHolidayActivityTasks(int iHolidayActivityTaskType,int iTabIdx)
{
	StringStreamType sStream;
	CF_HolidayActivityTaskType::DataEntry* pkEntry = SafeGetCSVFileDataEntryPtr<CF_HolidayActivityTaskType>(iHolidayActivityTaskType);
	if (!pkEntry)
	{
		return "";
	}
	TStringVector kProperties=UT_BaseStringUtility::SplitString(pkEntry->_sChapters, ';');	
	int id=atoi(kProperties[iTabIdx].c_str());
	CF_HolidayActivityTaskList::DataEntry* listEntry=SafeGetCSVFileDataEntryPtr<CF_HolidayActivityTaskList>(id);
	if (!listEntry)
	{
		return "";
	}
	StringType TaskInfoStr;
	StringType FinishedTaskInfoStr;
	StringType CanTakeRewardTaskInfoStr;
	if(!listEntry->_sTaskID.empty()) {
		GetTaskInfoStringSortByFinished(listEntry->_sTaskID,TaskInfoStr,FinishedTaskInfoStr,CanTakeRewardTaskInfoStr);
	}
	else {
		GetAchievementInfoStringSortByFinished(listEntry->_sAchievementID,TaskInfoStr,FinishedTaskInfoStr,CanTakeRewardTaskInfoStr);
	}
	sStream<<CanTakeRewardTaskInfoStr << TaskInfoStr << FinishedTaskInfoStr;
	return sStream.str();
}

bool LC_ClientGamePlayer::IsHolidayActivityTasksPrompt(int iHolidayActivityTaskType,int iTabIdx)
{
	CF_HolidayActivityTaskType::DataEntry* pkEntry = SafeGetCSVFileDataEntryPtr<CF_HolidayActivityTaskType>(iHolidayActivityTaskType);
	bool CanFinish=false; 
	if (!pkEntry)
	{
		return false;
	}
	TStringVector kProperties=UT_BaseStringUtility::SplitString(pkEntry->_sChapters, ';');	
	int id=atoi(kProperties[iTabIdx].c_str());
	CF_HolidayActivityTaskList::DataEntry* listEntry=SafeGetCSVFileDataEntryPtr<CF_HolidayActivityTaskList>(id);
	if (!listEntry)
	{
		return false;
	}
	if (!listEntry->_sTaskID.empty())
	{
	    CanFinish=IsTasksCanFinish(listEntry->_sTaskID);
	}
	else {
		CanFinish=IsAchievementCanFinish(listEntry->_sAchievementID);
	}
	return CanFinish;
} 


void LC_ClientGamePlayer::AcceptHolidayTasks(int iHolidayActivityTaskType) {
	CF_HolidayActivityTaskType::DataEntry* pkEntry = SafeGetCSVFileDataEntryPtr<CF_HolidayActivityTaskType>(iHolidayActivityTaskType);
	if (!pkEntry)
	{
		return;
	}
	TStringVector kProperties=UT_BaseStringUtility::SplitString(pkEntry->_sChapters, ';');	
	for (int i=0;i<kProperties.size();++i)
	{
		int id=atoi(kProperties[i].c_str());
		CF_HolidayActivityTaskList::DataEntry* listEntry=SafeGetCSVFileDataEntryPtr<CF_HolidayActivityTaskList>(id);
		if (!listEntry)
		{
			return ;
		}
		if(!listEntry->_sTaskID.empty()) {
			AcceptUnFinishedTasks(listEntry->_sTaskID);
		}
	}
	CF_HolidayActivity_PinTuUnlock::DataEntryMapExternIterator iter = CF_HolidayActivity_PinTuUnlock::GetSingletonPtr()->GetIterator();
	LC_TaskMap* pkTaskMap = GetTaskMap();
	for( ; iter.HasMoreElements(); iter.MoveNext())
	{
		CF_HolidayActivity_PinTuUnlock::DataEntry *pkData = iter.PeekNextValuePtr();
		bool bFinished=pkTaskMap->IsInFinishedTaskMap(pkData->_iTaskID);
		LC_ProcessingTaskEntry* pkTaskEntry = pkTaskMap->GetProcessingTaskEntryByID(pkData->_iTaskID);
		if (!pkTaskEntry && !bFinished)
		{
			ReqAcceptTask(pkData->_iTaskID);
		}
	}
}

void LC_ClientGamePlayer::AcceptUnFinishedTasks(Utility::UT_SIMDataList& InTasks){
	mem::vector<UT_SIMDataInfo>::iterator it = InTasks.begin();
	while (it != InTasks.end())
	{
		int iTaskID=it->GetTaskID();
		CF_TaskList::DataEntry *pkTaskData=SafeGetCSVFileDataEntryPtr<CF_TaskList>(iTaskID);
		if(!pkTaskData)
		{
			++it;
			continue;
		}

		LC_TaskMap* pkTaskMap = GetTaskMap();
		if (!pkTaskMap)
		{
			++it;
			continue;
		}
		bool bFinished=pkTaskMap->IsInFinishedTaskMap(iTaskID);
		LC_ProcessingTaskEntry* pkTaskEntry = pkTaskMap->GetProcessingTaskEntryByID(iTaskID);
		LC_TaskManager* pTaskMgr=LC_TaskManager::GetSingletonPtr();
		LC_Task* pTask = pTaskMgr->GetTask(iTaskID);
		LC_Task_Condition* pCondition=pTask->GetFinishConditionNoParams(0);
		int CurCount=0;
		int MaxCount = pCondition->GetMaxCount();
		StringType TraceInfo=pTask->GetTaskTraceInfo(iTaskID);

		if (!pkTaskEntry && !bFinished)
		{
			ReqAcceptTask(iTaskID);
		}
		++it;
	}

}//篝火晚会 获取购买玩家信息 
StringType LC_ClientGamePlayer::GetBuyerInfo(){
	StringStreamType sStream(""); 
	for (int i=0;i<m_BuyerInfo.size();++i)
	{
		//依次为头衔id, 头像框id, 头像id, 等级 ,倍率
		sStream<<m_BuyerInfo[i]<<"#"; 
	}
	return sStream.str() ; 
}



void LC_ClientGamePlayer::AcceptSpecialTask(int specialTaskId){

	CF_SpecialTask::DataEntry* pkEntry = SafeGetCSVFileDataEntryPtr<CF_SpecialTask>(specialTaskId);
	if (!pkEntry)
	{
		return ; 
	}
	LC_TaskMap* pkTaskMap = GetTaskMap();
	mem::vector<int> & taskInfos=pkEntry->_kTaskIDs;
	int size=taskInfos.size();
	for (int i=0;i<size;++i)
	{
		bool bFinished=pkTaskMap->IsInFinishedTaskMap(taskInfos[i]);
		LC_ProcessingTaskEntry* pkTaskEntry = pkTaskMap->GetProcessingTaskEntryByID(taskInfos[i]);
		if (!bFinished && !pkTaskEntry)
		{
			ReqAcceptTask(taskInfos[i]); 
		}
	}

}