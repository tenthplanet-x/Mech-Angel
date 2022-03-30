#include "ClientPCH.h"
#include "CSV_Client_Helper.h"

#include "CF_ActorFilterFunc.h"
#include "CF_AreaBarrier.h"
#include "CF_AreaEffect.h"
#include "CF_Subordinate.h"
#include "CF_SubordinateCluster.h"
#include "CF_SubordinateTransform.h"
#include "CF_SubRelateAttri.h"
#include "CF_AuctionCategory.h"
#include "CF_AutoFitCharType.h"
#include "CF_CharType.h"
#include "CF_CreateCharParam.h"
#include "CF_Conversation.h"
#include "CF_CutScenes.h"
#include "CF_CrossRealmMap.h"
#include "CF_EffectList.h"
#include "CF_EXPList.h"
#include "CF_Faction.h"
#include "CF_ForbiddenWordsList.h"
#include "CF_FriendMsgBroadContent.h"
#include "CF_GameStory.h"
#include "CF_GameStoryClientUI.h"
#include "CF_GameStoryRaidList.h"
#include "CF_GameStoryMessage.h"
#include "CF_GuildLevel.h"
#include "CF_GuildCreateLimit.h"
#include "CF_GuildWarBuff.h"
#include "CF_GuildGood.h"
#include "CF_GlobalPowerGuildReward.h"
#include "CF_GemExp.h"
#include "CF_GemReinforceLevel.h"
#include "CF_HittedSoundList.h"
#include "CF_ItemAssembleUseFunc.h"
#include "CF_ItemAvartarFunc.h"
#include "CF_ItemCategory.h"
#include "CF_ItemDecompositionFunc.h"
#include "CF_ItemDisassembleUseFunc.h"
#include "CF_ItemEffectFunc.h"
#include "CF_ItemEffect.h"
#include "CF_ItemEnduranceFunc.h"
#include "CF_ItemEquipLimitFunc.h"
#include "CF_ItemExistenceFunc.h"
#include "CF_ItemGenEffect.h"
#include "CF_ItemGen.h"
#include "CF_ItemAttrActive.h"
#include "CF_ItemList.h"
#include "CF_ItemOperationFunc.h"
#include "CF_ItemPropertyFunc.h"
#include "CF_ItemSuit.h"
#include "CF_ItemUIAssembleUseFunc.h"
#include "CF_ItemUseLimitFunc.h"
#include "CF_ItemReportFunc.h"
#include "CF_LevelUp.h"
#include "CF_LoadingTip.h"
#include "CF_LogicAreaChunkIDList.h"
#include "CF_LogicAreaFunc.h"
#include "CF_LogicTriggerList.h"
#include "CF_MailCharge.h"
#include "CF_MainTaskList.h"
#include "CF_MillionWeapons.h"
#include "CF_MonsterAIFunc.h"
#include "CF_NPCAvatar.h"
#include "CF_NPCFunction.h"
#include "CF_NPCInteractionConfig.h"
#include "CF_NPCShopList.h"
#include "CF_NPCSoundList.h"
#include "CF_NPCTransferList.h"
#include "CF_OpenBag.h"
#include "CF_OpenLevel.h"
#include "CF_ProductionFormula.h"
#include "CF_CompositionFormula.h"
#include "CF_RankReward.h"
#include "CF_ShopList.h"
#include "CF_SignInReward.h"
#include "CF_SocketItemFunc.h"
#include "CF_SkillBookLevelUp.h"
#include "CF_SkillBulletFunc.h"
#include "CF_SkillCDModifyFunc.h"
#include "CF_SkillExecuteList.h"
#include "CF_SkillSpecialAttrib.h"
#include "CF_SkillLevelUp.h"
#include "CF_SkillStageFunc.h"
#include "CF_SkillStateList.h"
#include "CF_SPLevelup.h"
#include "CF_SystemTitleList.h"
#include "CF_TaskCondition.h"
#include "CF_TaskList.h"
#include "CF_TextColor.h"
#include "CF_TextConfig.h"
#include "CF_TranscriptionRule.h"
#include "CF_TransferArea.h"
#include "CF_Tutorial.h"
#include "CF_TranscriptionSettleGrade.h"
#include "CF_UIControlRelation.h"
#include "CF_UISound.h"
#include "CF_Vigor.h"
#include "CF_WarehouseCost.h"
#include "CF_GodSuiteTask.h"
#include "CF_QQGirls.h"
#include "CF_WorldObjectDesc.h"
#include "CF_VIP.h"
#include "CF_VIPProfit.h"
#include "CF_DailyLiveness.h"
#include "CF_SD_Call.h"
#include "CF_TaskParam.h"
#include "CF_TaskAssign.h"
#include "CF_CharPromote.h"
#include "CF_VIPFDeposit.h"
#include "CF_CharAttributeFunc.h"
#include "CF_GuildWarCatapult.h"
#include "CF_GuildWarCrontab.h"
#include "CF_GuildWarActivity.h"
#include "CF_TreasureChests.h"
#include "CF_TreasurePool.h"
#include "CF_ForcesBattle.h"
#include "CF_PowerUp.h"
#include "CF_PowerUpFunc.h"
#include "CF_CommonStatReward.h"
#include "CF_CommonStatReplevy.h"
#include "CF_CommonStatConfig.h"
#include "CF_CommonStatCompensate.h"
#include "CF_CommonStatBasicReward.h"
#include "CF_ActivitySchedule.h"
#include "CF_SecretTreasureSchedule.h"
#include "CF_SecretTreasureShop.h"
#include "CF_SecretTreasureChests.h"
#include "CF_SecretTreasureReward.h"
#include "CF_SecretTreasureTimes.h"
#include "CF_ACTSubordinateReward.h"
#include "CF_ACTSubordinateRewardA.h"
#include "CF_ACTSubordinateRewardB.h"
#include "CF_ACTChargeProfit.h"
#include "CF_ACTChargeProfitA.h"
#include "CF_ACTChargeProfitB.h"
#include "CF_ACTChargeProfitBReward.h"
#include "CF_ACTChargeProfitType.h"
#include "CF_ACTShop.h"
#include "CF_ACTShopA.h"
#include "CF_ACTShot.h"
#include "CF_ACTShotA.h"
#include "CF_ACTLottery.h"
#include "CF_ACTLotteryReward.h"
#include "CF_ArenaSetting.h"
#include "CF_ArenaSection.h"
#include "CF_ArenaTimes.h"
#include "CF_YaZhiLevel.h"
#include "CF_GuildRaid.h"
#include "CF_GuildDonate.h"
#include "CF_WildPrinces.h"
#include "CF_TaskType.h"
#include "CF_Mental.h"
#include "CF_UIOpenLevel.h"
#include "CF_CharTypeFilterMiniMap.h"
#include "CF_AchievementList.h"
#include "CF_StoneList.h"
#include "CF_BossShowPosition.h"
#include "CF_MailItemList.h"
#include "CF_ShotActivity.h"
#include "CF_ItemOperation.h"
#include "CF_NPCShopItems.h"
#include "CF_FlowerConfig.h"
#include "CF_FlowerTitleCfg.h"
#include "CF_FlowerGiftCfg.h"
#include "CF_FlowerSendLimitCfg.h"
#include "CF_LocalSpawnPoint.h"
#include "CF_LogicAreaSound.h"
#include "CF_MapEffectConfig.h"
#include "CF_SoundObjectList.h"
#include "CF_SpawnPointList.h"
#include "CF_SystemError.h"
#include "CF_WorldMapList.h"
#include "CF_UITemplate.h"
#include "CF_MapCategory.h"
#include "CF_NPCFunctionType.h"
#include "CF_SoundSequence.h"
#include "CF_TreasurePanel.h"
#include "CF_SacrificeReward.h"
#include "CF_GuildClash.h"
#include "CF_SpecialEquipAvatar.h"
#include "CF_ResidentImage.h"
#include "CF_ClientNPCIdleAction.h"
#include "CF_FunctionPreview.h"
#include "CF_WalkTransferMap.h"
#include "CF_SmartHook.h"
#include "CF_SecretSkillClient.h"
#include "CF_UIFontColor.h"
#include "CF_UINameFilter.h"
#include "CF_UIAreaNameFilter.h"
#include "CF_OffLineExp.h"
#include "CF_OffLineExpRatio.h"
#include "CF_MysticalShop.h"
#include "CF_MysticalShopReset.h"
#include "CF_MysticalShopVip.h"
#include "CF_MysticalShopRefresh.h"
#include "CF_CameraCut.h"
#include "CF_GameNotice.h"
#include "CF_GfxList.h"
#include "CF_CSVSchedule.h"
#include "CF_ChatCooldown.h"
#include "CF_VIPPrivilegeCard.h"
#include "CF_Fashion.h"
#include "CF_FashionSuit.h"
#include "CF_FashionBag.h"
#include "CF_SoulRefinePage.h"
#include "CF_SoulRefineLevelUp.h"
#include "CF_SoulRefineMoster.h"
#include "CF_MasterBliss.h"
#include "CF_MasterBlissScore.h"
#include "CF_MasterBlissBuff.h"
#include "CF_MasterBlissPosition.h"
#include "CF_YunYingPlatformConfig.h"
#include "CF_YunYingOpenUrlConfig.h"
#include "CF_TwToZn.h"
#include "CF_ItemMelt.h"
#include "CF_MeltLevelUp.h"
#include "CF_EffectTrigger.h"
#include "CF_ACTSubordinateConsume.h"
#include "CF_Buddy.h"
#include "CF_BuddyUpgrade.h"
#include "CF_CharSkillWASDAnim.h"
#include "CF_EquipStar.h"
#include "CF_EquipSlotStar.h"
#include "CF_BuddyUnlock.h"
#include "CF_BuddyLimit.h"
#include "CF_GfxSmoothRelease.h"
#include "CF_DailyActivity.h"
#include "CF_AchievementRewards.h"
#include "CF_SignInDailyReward.h"
#include "CF_StarVeinList.h"
#include "CF_FirstDailyCharge.h"
#include "CF_SkillBookInitAttribute.h"
#include "CF_BuddyGrowGift.h"
#include "CF_Pet.h"
#include "CF_PetType.h"
#include "CF_PetQuality.h"
#include "CF_PetUpgrade.h"
#include "CF_CollectionActivation.h"
#include "CF_CollectionReward.h"
#include "CF_ElementDamage.h"
#include "CF_BossFirstKill.h"
#include "CF_SevenDaysReward.h"
#include "CF_SkillBulletFuncRandom.h"
#include "CF_SkillBookUnlock.h"
#include "CF_EndlessTower.h"
#include "CF_TaskChapters.h"
#include "CF_TaskRankRewards.h"
#include "CF_Bot.h"
#include "CF_BotUpgrade.h"
#include "CF_EquipSlotReinforce.h"
#include "CF_RuneLevelUp.h"
#include "CF_RuneStar.h"
#include "CF_Biography.h"
#include "CF_BiographyTask.h"
#include "CF_RuneList.h"
#include "CF_TaskReachRewards.h"
#include "CF_TimeLimitedActivity.h"
#include "CF_ACTShopTimeLimited.h"
#include "CF_HolyDeed.h"
#include "CF_StarVeinGoalList.h"
#include "CF_SevenDaysTask.h"
#include "CF_RankRewardGrade.h"
#include "CF_ShopGroupOpenSetting.h"
#include "CF_VIPProfitDescribe.h"
#include "CF_InvestmentPlan.h"
#include "CF_InvestmentCardTask.h"
#include "CF_InvestmentCard.h"
#include "CF_BossInvadeRankReward.h"
#include "CF_BossInvade.h"
#include "CF_VipNew.h"
#include "CF_HookMaps.h"
#include "CF_RecommendHero.h"
#include "CF_TestDatabase.h"
#include "CF_TestAccPointRewards.h"
#include "CF_TestRankReward.h"
#include "CF_TestRankGuildReward.h"
#include "CF_PromoteCombat.h"
#include "CF_ExportSoloDuleRobotInfo.h"
#include "CF_ExportRobotBuddyTemplateInfo.h"
#include "CF_ExportSoloDuleRobotProperty.h"
#include "CF_ShotActivityLimitRankRewards.h"
#include "CF_CommanderTitle.h"
#include "CF_SevenDaysTotalReward.h"
#include "CF_SpecialTraining.h"
#include "CF_SevenDaysLoginReward.h"
#include "CF_SoulConnect.h"
#include "CF_HeadFrame.h"
#include "CF_GloryExpedition.h"
#include "CF_EquipUpgrade.h"
#include "CF_BuddyRuneRecommend.h"
#include "CF_EquipCommanderJinglian.h"
#include "CF_EquipCommanderSlotReinforce.h"
#include "CF_EquipCommanderJinglianSuit.h"
#include "CF_EquipCommanderSlotReinforceSuit.h"
#include "CF_YiJiTuXiStageReward.h"
#include "CF_YiJiXunBaoStageReward.h"
#include "CF_CommanderCombatPK.h"
#include "CF_RotaryTable.h"
#include "CF_RotaryTableReward.h"
#include "CF_NonpareilItem.h"
#include "CF_BestGuild.h"
#include "CF_BestGuildTask.h"
#include "CF_BestGuildGuildRankReward.h"
#include "CF_BestGuildPersonalRankReward.h"
#include "CF_BestGuildBossRankRewards.h"
#include "CF_BestGuildBossInfo.h"
#include "CF_BestGuildRobReward.h"
#include "CF_BestGuildDailyTask.h"
#include "CF_RegularBoss.h"
#include "CF_EquipCommanderSlotMoldingSpirit.h"
#include "CF_EquipCommanderSlotMoldingSpiritSuit.h"
#include "CF_PetIslandStageReward.h"
#include "CF_EscortReward.h"
#include "CF_BestGuildGoal.h"
#include "CF_RankOpen.h"
#include "CF_GloryExpeditionDailyRewards.h"
#include "CF_BestGuildChapterID.h"
#include "CF_PromoteLevel.h"
#include "CF_GloryRewardLink.h"
#include "CF_ComeBack.h"
#include "CF_EquipCommanderSuitCollection.h"
#include "CF_NewAchievementReward.h"
#include "CF_TreasureAddAward.h"
#include "CF_BuddyTransform.h"
#include "CF_YiJiTuXiRankRewards.h"
#include "CF_GuildCrystal.h"
#include "CF_GuildGift.h"
#include "CF_TreasurePos.h"
#include "CF_AchievementTypeBig.h"
#include "CF_AchievementTypeSmall.h"
#include "CF_TreasureEvent.h"
#include "CF_TreasureItem.h"
#include "CF_EntrustTask.h"
#include "CF_SpecialBuy.h"
#include "CF_GuildTower.h"
#include "CF_GuildTowerRankReward.h"
#include "CF_BuddyThemeMain.h"
#include "CF_BuddyThemeSecondary.h"
#include "CF_GameRestartNotice.h"
#include "CF_BuddyThemeTaskType.h"
#include "CF_BuddyThemeBossRankReward.h"
#include "CF_BuddyThemeBossType.h"
#include "CF_BuddyThemeTaskList.h"
#include "CF_ACTTreasureHunt.h"
#include "CF_ACTTreasureHuntStageReward.h"
#include "CF_BossCamera.h"
#include "CF_BossCameraDetail.h"
#include "CF_HookPos.h"
#include "CF_Charge.h"
#include "CF_MailInfo.h"
#include "CF_TutorialRewards.h"
#include "CF_SubSystemInfo.h"
#include "CF_SkyArenaRank.h"
#include "CF_SkyArenaRankReward.h"
#include "CF_SpecialTask.h"
#include "CF_Guess.h"
#include "CF_SummitArenaMail.h"
#include "CF_SummitArenaMatchName.h"
#include "CF_SummitArenaRankRewards.h"
#include "CF_Scheme.h"
#include "CF_YiZhongJiangLinRankReward.h"
#include "CF_YiZhongJiangLinBossInformation.h"
#include "CF_HolidayActivityMain.h"
#include "CF_HolidayActivitySecondary.h"
#include "CF_ActivityRankReward.h"
#include "CF_HolidayActivity_ChargeContinuous.h"
#include "CF_HolidayActivity_PinTuUnlock.h"
#include "CF_HolidayActivity_PinTuReward.h"
#include "CF_HolidayActivityTaskType.h"
#include "CF_HolidayActivityTaskList.h"
#include "CF_RankCondition.h"
#include "CF_ACTShopB.h"
#include "CF_ActivityTime.h"

using namespace CSVFile;

bool Init_Client_CSV_File()
{
	CSVDataSourceBase::ClearIndices();
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/ActorFilterFunc.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ActorFilterFunc* obj = T_NEW_D CF_ActorFilterFunc();
		CSVDataSourceBase::SetIndex("ActorFilterFunc", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/ActorFilterFunc.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/ActorFilterFunc.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/AreaBarrier.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_AreaBarrier* obj = T_NEW_D CF_AreaBarrier();
		CSVDataSourceBase::SetIndex("AreaBarrier", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/AreaBarrier.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/AreaBarrier.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/AreaEffect.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_AreaEffect* obj = T_NEW_D CF_AreaEffect();
		CSVDataSourceBase::SetIndex("AreaEffect", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/AreaEffect.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/AreaEffect.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Subordinate/Subordinate.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_Subordinate* obj = T_NEW_D CF_Subordinate();
		CSVDataSourceBase::SetIndex("Subordinate", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Subordinate/Subordinate.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Subordinate/Subordinate.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Subordinate/SubordinateCluster.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SubordinateCluster* obj = T_NEW_D CF_SubordinateCluster();
		CSVDataSourceBase::SetIndex("SubordinateCluster", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Subordinate/SubordinateCluster.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Subordinate/SubordinateCluster.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Subordinate/SubordinateTransform.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SubordinateTransform* obj = T_NEW_D CF_SubordinateTransform();
		CSVDataSourceBase::SetIndex("SubordinateTransform", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Subordinate/SubordinateTransform.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Subordinate/SubordinateTransform.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Subordinate/SubRelateAttri.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SubRelateAttri* obj = T_NEW_D CF_SubRelateAttri();
		CSVDataSourceBase::SetIndex("SubRelateAttri", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Subordinate/SubRelateAttri.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Subordinate/SubRelateAttri.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/AuctionCategory.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_AuctionCategory* obj = T_NEW_D CF_AuctionCategory();
		CSVDataSourceBase::SetIndex("AuctionCategory", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/AuctionCategory.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/AuctionCategory.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/AutoFitCharType.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_AutoFitCharType* obj = T_NEW_D CF_AutoFitCharType();
		CSVDataSourceBase::SetIndex("AutoFitCharType", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/AutoFitCharType.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/AutoFitCharType.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/CharType.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_CharType* obj = T_NEW_D CF_CharType();
		CSVDataSourceBase::SetIndex("CharType", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/CharType.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/CharType.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/CreateCharParam.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_CreateCharParam* obj = T_NEW_D CF_CreateCharParam();
		CSVDataSourceBase::SetIndex("CreateCharParam", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/CreateCharParam.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/CreateCharParam.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Conversation.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_Conversation* obj = T_NEW_D CF_Conversation();
		CSVDataSourceBase::SetIndex("Conversation", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Conversation.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Conversation.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/CutScenes.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_CutScenes* obj = T_NEW_D CF_CutScenes();
		CSVDataSourceBase::SetIndex("CutScenes", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/CutScenes.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/CutScenes.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/CrossRealmMap.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_CrossRealmMap* obj = T_NEW_D CF_CrossRealmMap();
		CSVDataSourceBase::SetIndex("CrossRealmMap", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/CrossRealmMap.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/CrossRealmMap.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/EffectList.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_EffectList* obj = T_NEW_D CF_EffectList();
		CSVDataSourceBase::SetIndex("EffectList", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/EffectList.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/EffectList.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/EXPList.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_EXPList* obj = T_NEW_D CF_EXPList();
		CSVDataSourceBase::SetIndex("EXPList", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/EXPList.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/EXPList.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Faction.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_Faction* obj = T_NEW_D CF_Faction();
		CSVDataSourceBase::SetIndex("Faction", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Faction.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Faction.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/ForbiddenWordsList.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ForbiddenWordsList* obj = T_NEW_D CF_ForbiddenWordsList();
		CSVDataSourceBase::SetIndex("ForbiddenWordsList", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/ForbiddenWordsList.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/ForbiddenWordsList.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/FriendMsgBroadContent.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_FriendMsgBroadContent* obj = T_NEW_D CF_FriendMsgBroadContent();
		CSVDataSourceBase::SetIndex("FriendMsgBroadContent", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/FriendMsgBroadContent.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/FriendMsgBroadContent.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/GameStory.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_GameStory* obj = T_NEW_D CF_GameStory();
		CSVDataSourceBase::SetIndex("GameStory", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/GameStory.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/GameStory.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/GameStoryClientUI.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_GameStoryClientUI* obj = T_NEW_D CF_GameStoryClientUI();
		CSVDataSourceBase::SetIndex("GameStoryClientUI", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/GameStoryClientUI.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/GameStoryClientUI.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/GameStoryRaidList.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_GameStoryRaidList* obj = T_NEW_D CF_GameStoryRaidList();
		CSVDataSourceBase::SetIndex("GameStoryRaidList", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/GameStoryRaidList.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/GameStoryRaidList.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/GameStoryMessage.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_GameStoryMessage* obj = T_NEW_D CF_GameStoryMessage();
		CSVDataSourceBase::SetIndex("GameStoryMessage", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/GameStoryMessage.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/GameStoryMessage.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Guild/GuildLevel.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_GuildLevel* obj = T_NEW_D CF_GuildLevel();
		CSVDataSourceBase::SetIndex("GuildLevel", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Guild/GuildLevel.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Guild/GuildLevel.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Guild/GuildCreateLimit.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_GuildCreateLimit* obj = T_NEW_D CF_GuildCreateLimit();
		CSVDataSourceBase::SetIndex("GuildCreateLimit", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Guild/GuildCreateLimit.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Guild/GuildCreateLimit.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Guild/GuildWarBuff.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_GuildWarBuff* obj = T_NEW_D CF_GuildWarBuff();
		CSVDataSourceBase::SetIndex("GuildWarBuff", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Guild/GuildWarBuff.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Guild/GuildWarBuff.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Guild/GuildGood.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_GuildGood* obj = T_NEW_D CF_GuildGood();
		CSVDataSourceBase::SetIndex("GuildGood", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Guild/GuildGood.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Guild/GuildGood.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Guild/GlobalPowerGuildReward.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_GlobalPowerGuildReward* obj = T_NEW_D CF_GlobalPowerGuildReward();
		CSVDataSourceBase::SetIndex("GlobalPowerGuildReward", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Guild/GlobalPowerGuildReward.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Guild/GlobalPowerGuildReward.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/GemExp.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_GemExp* obj = T_NEW_D CF_GemExp();
		CSVDataSourceBase::SetIndex("GemExp", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/GemExp.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/GemExp.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/GemReinforceLevel.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_GemReinforceLevel* obj = T_NEW_D CF_GemReinforceLevel();
		CSVDataSourceBase::SetIndex("GemReinforceLevel", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/GemReinforceLevel.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/GemReinforceLevel.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/HittedSoundList.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_HittedSoundList* obj = T_NEW_D CF_HittedSoundList();
		CSVDataSourceBase::SetIndex("HittedSoundList", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/HittedSoundList.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/HittedSoundList.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/ItemAssembleUseFunc.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ItemAssembleUseFunc* obj = T_NEW_D CF_ItemAssembleUseFunc();
		CSVDataSourceBase::SetIndex("ItemAssembleUseFunc", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/ItemAssembleUseFunc.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/ItemAssembleUseFunc.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/ItemAvartarFunc.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ItemAvartarFunc* obj = T_NEW_D CF_ItemAvartarFunc();
		CSVDataSourceBase::SetIndex("ItemAvartarFunc", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/ItemAvartarFunc.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/ItemAvartarFunc.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/ItemCategory.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ItemCategory* obj = T_NEW_D CF_ItemCategory();
		CSVDataSourceBase::SetIndex("ItemCategory", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/ItemCategory.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/ItemCategory.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/ItemDecompositionFunc.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ItemDecompositionFunc* obj = T_NEW_D CF_ItemDecompositionFunc();
		CSVDataSourceBase::SetIndex("ItemDecompositionFunc", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/ItemDecompositionFunc.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/ItemDecompositionFunc.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/ItemDisassembleUseFunc.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ItemDisassembleUseFunc* obj = T_NEW_D CF_ItemDisassembleUseFunc();
		CSVDataSourceBase::SetIndex("ItemDisassembleUseFunc", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/ItemDisassembleUseFunc.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/ItemDisassembleUseFunc.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/ItemEffectFunc.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ItemEffectFunc* obj = T_NEW_D CF_ItemEffectFunc();
		CSVDataSourceBase::SetIndex("ItemEffectFunc", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/ItemEffectFunc.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/ItemEffectFunc.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/ItemEffect.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ItemEffect* obj = T_NEW_D CF_ItemEffect();
		CSVDataSourceBase::SetIndex("ItemEffect", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/ItemEffect.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/ItemEffect.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/ItemEnduranceFunc.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ItemEnduranceFunc* obj = T_NEW_D CF_ItemEnduranceFunc();
		CSVDataSourceBase::SetIndex("ItemEnduranceFunc", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/ItemEnduranceFunc.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/ItemEnduranceFunc.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/ItemEquipLimitFunc.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ItemEquipLimitFunc* obj = T_NEW_D CF_ItemEquipLimitFunc();
		CSVDataSourceBase::SetIndex("ItemEquipLimitFunc", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/ItemEquipLimitFunc.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/ItemEquipLimitFunc.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/ItemExistenceFunc.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ItemExistenceFunc* obj = T_NEW_D CF_ItemExistenceFunc();
		CSVDataSourceBase::SetIndex("ItemExistenceFunc", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/ItemExistenceFunc.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/ItemExistenceFunc.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/ItemGenEffect.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ItemGenEffect* obj = T_NEW_D CF_ItemGenEffect();
		CSVDataSourceBase::SetIndex("ItemGenEffect", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/ItemGenEffect.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/ItemGenEffect.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/ItemGen.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ItemGen* obj = T_NEW_D CF_ItemGen();
		CSVDataSourceBase::SetIndex("ItemGen", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/ItemGen.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/ItemGen.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/ItemAttrActive.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ItemAttrActive* obj = T_NEW_D CF_ItemAttrActive();
		CSVDataSourceBase::SetIndex("ItemAttrActive", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/ItemAttrActive.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/ItemAttrActive.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/ItemList.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ItemList* obj = T_NEW_D CF_ItemList();
		CSVDataSourceBase::SetIndex("ItemList", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/ItemList.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/ItemList.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/ItemOperationFunc.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ItemOperationFunc* obj = T_NEW_D CF_ItemOperationFunc();
		CSVDataSourceBase::SetIndex("ItemOperationFunc", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/ItemOperationFunc.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/ItemOperationFunc.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/ItemPropertyFunc.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ItemPropertyFunc* obj = T_NEW_D CF_ItemPropertyFunc();
		CSVDataSourceBase::SetIndex("ItemPropertyFunc", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/ItemPropertyFunc.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/ItemPropertyFunc.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/ItemSuit.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ItemSuit* obj = T_NEW_D CF_ItemSuit();
		CSVDataSourceBase::SetIndex("ItemSuit", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/ItemSuit.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/ItemSuit.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/ItemUIAssembleUseFunc.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ItemUIAssembleUseFunc* obj = T_NEW_D CF_ItemUIAssembleUseFunc();
		CSVDataSourceBase::SetIndex("ItemUIAssembleUseFunc", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/ItemUIAssembleUseFunc.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/ItemUIAssembleUseFunc.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/ItemUseLimitFunc.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ItemUseLimitFunc* obj = T_NEW_D CF_ItemUseLimitFunc();
		CSVDataSourceBase::SetIndex("ItemUseLimitFunc", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/ItemUseLimitFunc.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/ItemUseLimitFunc.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/ItemReportFunc.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ItemReportFunc* obj = T_NEW_D CF_ItemReportFunc();
		CSVDataSourceBase::SetIndex("ItemReportFunc", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/ItemReportFunc.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/ItemReportFunc.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/LevelUp.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_LevelUp* obj = T_NEW_D CF_LevelUp();
		CSVDataSourceBase::SetIndex("LevelUp", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/LevelUp.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/LevelUp.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/LoadingTip.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_LoadingTip* obj = T_NEW_D CF_LoadingTip();
		CSVDataSourceBase::SetIndex("LoadingTip", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/LoadingTip.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/LoadingTip.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/LogicAreaChunkIDList.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_LogicAreaChunkIDList* obj = T_NEW_D CF_LogicAreaChunkIDList();
		CSVDataSourceBase::SetIndex("LogicAreaChunkIDList", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/LogicAreaChunkIDList.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/LogicAreaChunkIDList.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/LogicAreaFunc.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_LogicAreaFunc* obj = T_NEW_D CF_LogicAreaFunc();
		CSVDataSourceBase::SetIndex("LogicAreaFunc", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/LogicAreaFunc.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/LogicAreaFunc.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/LogicTriggerList.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_LogicTriggerList* obj = T_NEW_D CF_LogicTriggerList();
		CSVDataSourceBase::SetIndex("LogicTriggerList", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/LogicTriggerList.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/LogicTriggerList.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/MailCharge.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_MailCharge* obj = T_NEW_D CF_MailCharge();
		CSVDataSourceBase::SetIndex("MailCharge", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/MailCharge.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/MailCharge.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/MainTaskList.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_MainTaskList* obj = T_NEW_D CF_MainTaskList();
		CSVDataSourceBase::SetIndex("MainTaskList", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/MainTaskList.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/MainTaskList.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/MillionWeapons.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_MillionWeapons* obj = T_NEW_D CF_MillionWeapons();
		CSVDataSourceBase::SetIndex("MillionWeapons", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/MillionWeapons.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/MillionWeapons.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/MonsterAIFunc.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_MonsterAIFunc* obj = T_NEW_D CF_MonsterAIFunc();
		CSVDataSourceBase::SetIndex("MonsterAIFunc", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/MonsterAIFunc.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/MonsterAIFunc.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/NPCAvatar.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_NPCAvatar* obj = T_NEW_D CF_NPCAvatar();
		CSVDataSourceBase::SetIndex("NPCAvatar", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/NPCAvatar.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/NPCAvatar.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/NPCFunction.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_NPCFunction* obj = T_NEW_D CF_NPCFunction();
		CSVDataSourceBase::SetIndex("NPCFunction", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/NPCFunction.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/NPCFunction.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/NPCInteractionConfig.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_NPCInteractionConfig* obj = T_NEW_D CF_NPCInteractionConfig();
		CSVDataSourceBase::SetIndex("NPCInteractionConfig", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/NPCInteractionConfig.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/NPCInteractionConfig.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/NPCShopList.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_NPCShopList* obj = T_NEW_D CF_NPCShopList();
		CSVDataSourceBase::SetIndex("NPCShopList", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/NPCShopList.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/NPCShopList.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/NPCSoundList.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_NPCSoundList* obj = T_NEW_D CF_NPCSoundList();
		CSVDataSourceBase::SetIndex("NPCSoundList", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/NPCSoundList.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/NPCSoundList.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/NPCTransferList.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_NPCTransferList* obj = T_NEW_D CF_NPCTransferList();
		CSVDataSourceBase::SetIndex("NPCTransferList", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/NPCTransferList.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/NPCTransferList.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/OpenBag.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_OpenBag* obj = T_NEW_D CF_OpenBag();
		CSVDataSourceBase::SetIndex("OpenBag", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/OpenBag.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/OpenBag.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/OpenLevel.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_OpenLevel* obj = T_NEW_D CF_OpenLevel();
		CSVDataSourceBase::SetIndex("OpenLevel", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/OpenLevel.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/OpenLevel.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/ProductionFormula.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ProductionFormula* obj = T_NEW_D CF_ProductionFormula();
		CSVDataSourceBase::SetIndex("ProductionFormula", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/ProductionFormula.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/ProductionFormula.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/CompositionFormula.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_CompositionFormula* obj = T_NEW_D CF_CompositionFormula();
		CSVDataSourceBase::SetIndex("CompositionFormula", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/CompositionFormula.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/CompositionFormula.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/RankReward.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_RankReward* obj = T_NEW_D CF_RankReward();
		CSVDataSourceBase::SetIndex("RankReward", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/RankReward.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/RankReward.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/ShopList.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ShopList* obj = T_NEW_D CF_ShopList();
		CSVDataSourceBase::SetIndex("ShopList", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/ShopList.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/ShopList.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/SignInReward.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SignInReward* obj = T_NEW_D CF_SignInReward();
		CSVDataSourceBase::SetIndex("SignInReward", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/SignInReward.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/SignInReward.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/SocketItemFunc.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SocketItemFunc* obj = T_NEW_D CF_SocketItemFunc();
		CSVDataSourceBase::SetIndex("SocketItemFunc", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/SocketItemFunc.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/SocketItemFunc.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/SkillBookLevelUp.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SkillBookLevelUp* obj = T_NEW_D CF_SkillBookLevelUp();
		CSVDataSourceBase::SetIndex("SkillBookLevelUp", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/SkillBookLevelUp.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/SkillBookLevelUp.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/SkillBulletFunc.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SkillBulletFunc* obj = T_NEW_D CF_SkillBulletFunc();
		CSVDataSourceBase::SetIndex("SkillBulletFunc", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/SkillBulletFunc.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/SkillBulletFunc.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/SkillCDModifyFunc.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SkillCDModifyFunc* obj = T_NEW_D CF_SkillCDModifyFunc();
		CSVDataSourceBase::SetIndex("SkillCDModifyFunc", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/SkillCDModifyFunc.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/SkillCDModifyFunc.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/SkillExecuteList.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SkillExecuteList* obj = T_NEW_D CF_SkillExecuteList();
		CSVDataSourceBase::SetIndex("SkillExecuteList", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/SkillExecuteList.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/SkillExecuteList.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/SkillSpecialAttrib.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SkillSpecialAttrib* obj = T_NEW_D CF_SkillSpecialAttrib();
		CSVDataSourceBase::SetIndex("SkillSpecialAttrib", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/SkillSpecialAttrib.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/SkillSpecialAttrib.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/SkillLevelUp.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SkillLevelUp* obj = T_NEW_D CF_SkillLevelUp();
		CSVDataSourceBase::SetIndex("SkillLevelUp", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/SkillLevelUp.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/SkillLevelUp.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/SkillStageFunc.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SkillStageFunc* obj = T_NEW_D CF_SkillStageFunc();
		CSVDataSourceBase::SetIndex("SkillStageFunc", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/SkillStageFunc.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/SkillStageFunc.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/SkillStateList.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SkillStateList* obj = T_NEW_D CF_SkillStateList();
		CSVDataSourceBase::SetIndex("SkillStateList", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/SkillStateList.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/SkillStateList.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/SPLevelup.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SPLevelup* obj = T_NEW_D CF_SPLevelup();
		CSVDataSourceBase::SetIndex("SPLevelup", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/SPLevelup.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/SPLevelup.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/SystemTitleList.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SystemTitleList* obj = T_NEW_D CF_SystemTitleList();
		CSVDataSourceBase::SetIndex("SystemTitleList", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/SystemTitleList.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/SystemTitleList.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/TaskCondition.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_TaskCondition* obj = T_NEW_D CF_TaskCondition();
		CSVDataSourceBase::SetIndex("TaskCondition", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/TaskCondition.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/TaskCondition.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/TaskList.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_TaskList* obj = T_NEW_D CF_TaskList();
		CSVDataSourceBase::SetIndex("TaskList", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/TaskList.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/TaskList.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/TextColor.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_TextColor* obj = T_NEW_D CF_TextColor();
		CSVDataSourceBase::SetIndex("TextColor", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/TextColor.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/TextColor.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/TextConfig.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_TextConfig* obj = T_NEW_D CF_TextConfig();
		CSVDataSourceBase::SetIndex("TextConfig", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/TextConfig.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/TextConfig.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/TranscriptionRule.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_TranscriptionRule* obj = T_NEW_D CF_TranscriptionRule();
		CSVDataSourceBase::SetIndex("TranscriptionRule", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/TranscriptionRule.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/TranscriptionRule.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/TransferArea.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_TransferArea* obj = T_NEW_D CF_TransferArea();
		CSVDataSourceBase::SetIndex("TransferArea", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/TransferArea.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/TransferArea.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Tutorial.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_Tutorial* obj = T_NEW_D CF_Tutorial();
		CSVDataSourceBase::SetIndex("Tutorial", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Tutorial.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Tutorial.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/TranscriptionSettleGrade.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_TranscriptionSettleGrade* obj = T_NEW_D CF_TranscriptionSettleGrade();
		CSVDataSourceBase::SetIndex("TranscriptionSettleGrade", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/TranscriptionSettleGrade.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/TranscriptionSettleGrade.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/UIControlRelation.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_UIControlRelation* obj = T_NEW_D CF_UIControlRelation();
		CSVDataSourceBase::SetIndex("UIControlRelation", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/UIControlRelation.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/UIControlRelation.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/UISound.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_UISound* obj = T_NEW_D CF_UISound();
		CSVDataSourceBase::SetIndex("UISound", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/UISound.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/UISound.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Vigor.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_Vigor* obj = T_NEW_D CF_Vigor();
		CSVDataSourceBase::SetIndex("Vigor", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Vigor.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Vigor.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/WarehouseCost.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_WarehouseCost* obj = T_NEW_D CF_WarehouseCost();
		CSVDataSourceBase::SetIndex("WarehouseCost", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/WarehouseCost.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/WarehouseCost.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/GodSuiteTask.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_GodSuiteTask* obj = T_NEW_D CF_GodSuiteTask();
		CSVDataSourceBase::SetIndex("GodSuiteTask", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/GodSuiteTask.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/GodSuiteTask.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/QQGirls.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_QQGirls* obj = T_NEW_D CF_QQGirls();
		CSVDataSourceBase::SetIndex("QQGirls", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/QQGirls.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/QQGirls.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/WorldObjectDesc.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_WorldObjectDesc* obj = T_NEW_D CF_WorldObjectDesc();
		CSVDataSourceBase::SetIndex("WorldObjectDesc", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/WorldObjectDesc.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/WorldObjectDesc.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/VIP.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_VIP* obj = T_NEW_D CF_VIP();
		CSVDataSourceBase::SetIndex("VIP", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/VIP.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/VIP.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/VIPProfit.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_VIPProfit* obj = T_NEW_D CF_VIPProfit();
		CSVDataSourceBase::SetIndex("VIPProfit", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/VIPProfit.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/VIPProfit.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/DailyLiveness.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_DailyLiveness* obj = T_NEW_D CF_DailyLiveness();
		CSVDataSourceBase::SetIndex("DailyLiveness", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/DailyLiveness.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/DailyLiveness.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/SD_Call.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SD_Call* obj = T_NEW_D CF_SD_Call();
		CSVDataSourceBase::SetIndex("SD_Call", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/SD_Call.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/SD_Call.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/TaskParam.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_TaskParam* obj = T_NEW_D CF_TaskParam();
		CSVDataSourceBase::SetIndex("TaskParam", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/TaskParam.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/TaskParam.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/TaskAssign.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_TaskAssign* obj = T_NEW_D CF_TaskAssign();
		CSVDataSourceBase::SetIndex("TaskAssign", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/TaskAssign.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/TaskAssign.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/CharPromote.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_CharPromote* obj = T_NEW_D CF_CharPromote();
		CSVDataSourceBase::SetIndex("CharPromote", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/CharPromote.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/CharPromote.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/VIPFDeposit.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_VIPFDeposit* obj = T_NEW_D CF_VIPFDeposit();
		CSVDataSourceBase::SetIndex("VIPFDeposit", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/VIPFDeposit.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/VIPFDeposit.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/CharAttributeFunc.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_CharAttributeFunc* obj = T_NEW_D CF_CharAttributeFunc();
		CSVDataSourceBase::SetIndex("CharAttributeFunc", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/CharAttributeFunc.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/CharAttributeFunc.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Guild/GuildWarCatapult.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_GuildWarCatapult* obj = T_NEW_D CF_GuildWarCatapult();
		CSVDataSourceBase::SetIndex("GuildWarCatapult", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Guild/GuildWarCatapult.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Guild/GuildWarCatapult.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Guild/GuildWarCrontab.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_GuildWarCrontab* obj = T_NEW_D CF_GuildWarCrontab();
		CSVDataSourceBase::SetIndex("GuildWarCrontab", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Guild/GuildWarCrontab.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Guild/GuildWarCrontab.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Guild/GuildWarActivity.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_GuildWarActivity* obj = T_NEW_D CF_GuildWarActivity();
		CSVDataSourceBase::SetIndex("GuildWarActivity", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Guild/GuildWarActivity.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Guild/GuildWarActivity.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/TreasureChests.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_TreasureChests* obj = T_NEW_D CF_TreasureChests();
		CSVDataSourceBase::SetIndex("TreasureChests", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/TreasureChests.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/TreasureChests.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/TreasurePool.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_TreasurePool* obj = T_NEW_D CF_TreasurePool();
		CSVDataSourceBase::SetIndex("TreasurePool", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/TreasurePool.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/TreasurePool.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Guild/ForcesBattle.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ForcesBattle* obj = T_NEW_D CF_ForcesBattle();
		CSVDataSourceBase::SetIndex("ForcesBattle", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Guild/ForcesBattle.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Guild/ForcesBattle.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/PowerUp.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_PowerUp* obj = T_NEW_D CF_PowerUp();
		CSVDataSourceBase::SetIndex("PowerUp", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/PowerUp.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/PowerUp.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/PowerUpFunc.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_PowerUpFunc* obj = T_NEW_D CF_PowerUpFunc();
		CSVDataSourceBase::SetIndex("PowerUpFunc", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/PowerUpFunc.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/PowerUpFunc.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/CommonStat/CommonStatReward.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_CommonStatReward* obj = T_NEW_D CF_CommonStatReward();
		CSVDataSourceBase::SetIndex("CommonStatReward", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/CommonStat/CommonStatReward.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/CommonStat/CommonStatReward.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/CommonStat/CommonStatReplevy.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_CommonStatReplevy* obj = T_NEW_D CF_CommonStatReplevy();
		CSVDataSourceBase::SetIndex("CommonStatReplevy", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/CommonStat/CommonStatReplevy.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/CommonStat/CommonStatReplevy.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/CommonStat/CommonStatConfig.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_CommonStatConfig* obj = T_NEW_D CF_CommonStatConfig();
		CSVDataSourceBase::SetIndex("CommonStatConfig", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/CommonStat/CommonStatConfig.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/CommonStat/CommonStatConfig.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/CommonStat/CommonStatCompensate.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_CommonStatCompensate* obj = T_NEW_D CF_CommonStatCompensate();
		CSVDataSourceBase::SetIndex("CommonStatCompensate", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/CommonStat/CommonStatCompensate.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/CommonStat/CommonStatCompensate.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/CommonStat/CommonStatBasicReward.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_CommonStatBasicReward* obj = T_NEW_D CF_CommonStatBasicReward();
		CSVDataSourceBase::SetIndex("CommonStatBasicReward", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/CommonStat/CommonStatBasicReward.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/CommonStat/CommonStatBasicReward.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Activity/ActivitySchedule.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ActivitySchedule* obj = T_NEW_D CF_ActivitySchedule();
		CSVDataSourceBase::SetIndex("ActivitySchedule", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Activity/ActivitySchedule.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Activity/ActivitySchedule.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Activity/SecretTreasure/SecretTreasureSchedule.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SecretTreasureSchedule* obj = T_NEW_D CF_SecretTreasureSchedule();
		CSVDataSourceBase::SetIndex("SecretTreasureSchedule", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Activity/SecretTreasure/SecretTreasureSchedule.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Activity/SecretTreasure/SecretTreasureSchedule.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Activity/SecretTreasure/SecretTreasureShop.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SecretTreasureShop* obj = T_NEW_D CF_SecretTreasureShop();
		CSVDataSourceBase::SetIndex("SecretTreasureShop", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Activity/SecretTreasure/SecretTreasureShop.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Activity/SecretTreasure/SecretTreasureShop.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Activity/SecretTreasure/SecretTreasureChests.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SecretTreasureChests* obj = T_NEW_D CF_SecretTreasureChests();
		CSVDataSourceBase::SetIndex("SecretTreasureChests", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Activity/SecretTreasure/SecretTreasureChests.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Activity/SecretTreasure/SecretTreasureChests.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Activity/SecretTreasure/SecretTreasureReward.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SecretTreasureReward* obj = T_NEW_D CF_SecretTreasureReward();
		CSVDataSourceBase::SetIndex("SecretTreasureReward", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Activity/SecretTreasure/SecretTreasureReward.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Activity/SecretTreasure/SecretTreasureReward.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Activity/SecretTreasure/SecretTreasureTimes.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SecretTreasureTimes* obj = T_NEW_D CF_SecretTreasureTimes();
		CSVDataSourceBase::SetIndex("SecretTreasureTimes", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Activity/SecretTreasure/SecretTreasureTimes.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Activity/SecretTreasure/SecretTreasureTimes.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Activity/Subordinate/ACTSubordinateReward.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ACTSubordinateReward* obj = T_NEW_D CF_ACTSubordinateReward();
		CSVDataSourceBase::SetIndex("ACTSubordinateReward", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Activity/Subordinate/ACTSubordinateReward.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Activity/Subordinate/ACTSubordinateReward.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Activity/Subordinate/ACTSubordinateRewardA.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ACTSubordinateRewardA* obj = T_NEW_D CF_ACTSubordinateRewardA();
		CSVDataSourceBase::SetIndex("ACTSubordinateRewardA", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Activity/Subordinate/ACTSubordinateRewardA.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Activity/Subordinate/ACTSubordinateRewardA.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Activity/Subordinate/ACTSubordinateRewardB.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ACTSubordinateRewardB* obj = T_NEW_D CF_ACTSubordinateRewardB();
		CSVDataSourceBase::SetIndex("ACTSubordinateRewardB", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Activity/Subordinate/ACTSubordinateRewardB.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Activity/Subordinate/ACTSubordinateRewardB.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Activity/ChargeProfit/ACTChargeProfit.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ACTChargeProfit* obj = T_NEW_D CF_ACTChargeProfit();
		CSVDataSourceBase::SetIndex("ACTChargeProfit", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Activity/ChargeProfit/ACTChargeProfit.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Activity/ChargeProfit/ACTChargeProfit.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Activity/ChargeProfit/ACTChargeProfitA.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ACTChargeProfitA* obj = T_NEW_D CF_ACTChargeProfitA();
		CSVDataSourceBase::SetIndex("ACTChargeProfitA", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Activity/ChargeProfit/ACTChargeProfitA.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Activity/ChargeProfit/ACTChargeProfitA.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Activity/ChargeProfit/ACTChargeProfitB.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ACTChargeProfitB* obj = T_NEW_D CF_ACTChargeProfitB();
		CSVDataSourceBase::SetIndex("ACTChargeProfitB", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Activity/ChargeProfit/ACTChargeProfitB.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Activity/ChargeProfit/ACTChargeProfitB.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Activity/ChargeProfit/ACTChargeProfitBReward.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ACTChargeProfitBReward* obj = T_NEW_D CF_ACTChargeProfitBReward();
		CSVDataSourceBase::SetIndex("ACTChargeProfitBReward", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Activity/ChargeProfit/ACTChargeProfitBReward.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Activity/ChargeProfit/ACTChargeProfitBReward.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Activity/ChargeProfit/ACTChargeProfitType.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ACTChargeProfitType* obj = T_NEW_D CF_ACTChargeProfitType();
		CSVDataSourceBase::SetIndex("ACTChargeProfitType", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Activity/ChargeProfit/ACTChargeProfitType.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Activity/ChargeProfit/ACTChargeProfitType.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Activity/Shop/ACTShop.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ACTShop* obj = T_NEW_D CF_ACTShop();
		CSVDataSourceBase::SetIndex("ACTShop", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Activity/Shop/ACTShop.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Activity/Shop/ACTShop.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Activity/Shop/ACTShopA.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ACTShopA* obj = T_NEW_D CF_ACTShopA();
		CSVDataSourceBase::SetIndex("ACTShopA", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Activity/Shop/ACTShopA.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Activity/Shop/ACTShopA.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Activity/Shot/ACTShot.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ACTShot* obj = T_NEW_D CF_ACTShot();
		CSVDataSourceBase::SetIndex("ACTShot", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Activity/Shot/ACTShot.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Activity/Shot/ACTShot.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Activity/Shot/ACTShotA.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ACTShotA* obj = T_NEW_D CF_ACTShotA();
		CSVDataSourceBase::SetIndex("ACTShotA", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Activity/Shot/ACTShotA.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Activity/Shot/ACTShotA.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Activity/Lottery/ACTLottery.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ACTLottery* obj = T_NEW_D CF_ACTLottery();
		CSVDataSourceBase::SetIndex("ACTLottery", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Activity/Lottery/ACTLottery.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Activity/Lottery/ACTLottery.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Activity/Lottery/ACTLotteryReward.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ACTLotteryReward* obj = T_NEW_D CF_ACTLotteryReward();
		CSVDataSourceBase::SetIndex("ACTLotteryReward", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Activity/Lottery/ACTLotteryReward.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Activity/Lottery/ACTLotteryReward.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Arena/ArenaSetting.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ArenaSetting* obj = T_NEW_D CF_ArenaSetting();
		CSVDataSourceBase::SetIndex("ArenaSetting", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Arena/ArenaSetting.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Arena/ArenaSetting.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Arena/ArenaSection.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ArenaSection* obj = T_NEW_D CF_ArenaSection();
		CSVDataSourceBase::SetIndex("ArenaSection", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Arena/ArenaSection.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Arena/ArenaSection.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Arena/ArenaTimes.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ArenaTimes* obj = T_NEW_D CF_ArenaTimes();
		CSVDataSourceBase::SetIndex("ArenaTimes", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Arena/ArenaTimes.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Arena/ArenaTimes.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/YaZhiLevel.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_YaZhiLevel* obj = T_NEW_D CF_YaZhiLevel();
		CSVDataSourceBase::SetIndex("YaZhiLevel", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/YaZhiLevel.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/YaZhiLevel.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Guild/GuildRaid.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_GuildRaid* obj = T_NEW_D CF_GuildRaid();
		CSVDataSourceBase::SetIndex("GuildRaid", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Guild/GuildRaid.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Guild/GuildRaid.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Guild/GuildDonate.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_GuildDonate* obj = T_NEW_D CF_GuildDonate();
		CSVDataSourceBase::SetIndex("GuildDonate", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Guild/GuildDonate.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Guild/GuildDonate.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/WildPrinces.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_WildPrinces* obj = T_NEW_D CF_WildPrinces();
		CSVDataSourceBase::SetIndex("WildPrinces", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/WildPrinces.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/WildPrinces.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/TaskType.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_TaskType* obj = T_NEW_D CF_TaskType();
		CSVDataSourceBase::SetIndex("TaskType", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/TaskType.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/TaskType.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Mental.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_Mental* obj = T_NEW_D CF_Mental();
		CSVDataSourceBase::SetIndex("Mental", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Mental.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Mental.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/UIOpenLevel.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_UIOpenLevel* obj = T_NEW_D CF_UIOpenLevel();
		CSVDataSourceBase::SetIndex("UIOpenLevel", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/UIOpenLevel.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/UIOpenLevel.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/CharTypeFilterMiniMap.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_CharTypeFilterMiniMap* obj = T_NEW_D CF_CharTypeFilterMiniMap();
		CSVDataSourceBase::SetIndex("CharTypeFilterMiniMap", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/CharTypeFilterMiniMap.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/CharTypeFilterMiniMap.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/AchievementList.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_AchievementList* obj = T_NEW_D CF_AchievementList();
		CSVDataSourceBase::SetIndex("AchievementList", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/AchievementList.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/AchievementList.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/StoneList.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_StoneList* obj = T_NEW_D CF_StoneList();
		CSVDataSourceBase::SetIndex("StoneList", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/StoneList.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/StoneList.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/BossShowPosition.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_BossShowPosition* obj = T_NEW_D CF_BossShowPosition();
		CSVDataSourceBase::SetIndex("BossShowPosition", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/BossShowPosition.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/BossShowPosition.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/MailItemList.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_MailItemList* obj = T_NEW_D CF_MailItemList();
		CSVDataSourceBase::SetIndex("MailItemList", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/MailItemList.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/MailItemList.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/ShotActivity.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ShotActivity* obj = T_NEW_D CF_ShotActivity();
		CSVDataSourceBase::SetIndex("ShotActivity", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/ShotActivity.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/ShotActivity.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/ItemOperation.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ItemOperation* obj = T_NEW_D CF_ItemOperation();
		CSVDataSourceBase::SetIndex("ItemOperation", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/ItemOperation.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/ItemOperation.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/NPCShopItems.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_NPCShopItems* obj = T_NEW_D CF_NPCShopItems();
		CSVDataSourceBase::SetIndex("NPCShopItems", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/NPCShopItems.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/NPCShopItems.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/flower/FlowerSend.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_FlowerConfig* obj = T_NEW_D CF_FlowerConfig();
		CSVDataSourceBase::SetIndex("FlowerSend", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/flower/FlowerSend.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/flower/FlowerSend.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/flower/FlowerTitle.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_FlowerTitleCfg* obj = T_NEW_D CF_FlowerTitleCfg();
		CSVDataSourceBase::SetIndex("FlowerTitle", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/flower/FlowerTitle.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/flower/FlowerTitle.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/flower/FlowerGift.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_FlowerGiftCfg* obj = T_NEW_D CF_FlowerGiftCfg();
		CSVDataSourceBase::SetIndex("FlowerGift", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/flower/FlowerGift.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/flower/FlowerGift.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/flower/FlowerCountLimit.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_FlowerSendLimitCfg* obj = T_NEW_D CF_FlowerSendLimitCfg();
		CSVDataSourceBase::SetIndex("FlowerCountLimit", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/flower/FlowerCountLimit.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/flower/FlowerCountLimit.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/ProgramCSV/LocalSpawnPoint.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_LocalSpawnPoint* obj = T_NEW_D CF_LocalSpawnPoint();
		CSVDataSourceBase::SetIndex("LocalSpawnPoint", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/ProgramCSV/LocalSpawnPoint.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/ProgramCSV/LocalSpawnPoint.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/ProgramCSV/LogicAreaSound.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_LogicAreaSound* obj = T_NEW_D CF_LogicAreaSound();
		CSVDataSourceBase::SetIndex("LogicAreaSound", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/ProgramCSV/LogicAreaSound.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/ProgramCSV/LogicAreaSound.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/ProgramCSV/MapEffectConfig.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_MapEffectConfig* obj = T_NEW_D CF_MapEffectConfig();
		CSVDataSourceBase::SetIndex("MapEffectConfig", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/ProgramCSV/MapEffectConfig.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/ProgramCSV/MapEffectConfig.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/ProgramCSV/SoundObjectList.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SoundObjectList* obj = T_NEW_D CF_SoundObjectList();
		CSVDataSourceBase::SetIndex("SoundObjectList", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/ProgramCSV/SoundObjectList.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/ProgramCSV/SoundObjectList.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/ProgramCSV/SpawnPointList.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SpawnPointList* obj = T_NEW_D CF_SpawnPointList();
		CSVDataSourceBase::SetIndex("SpawnPointList", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/ProgramCSV/SpawnPointList.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/ProgramCSV/SpawnPointList.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/ProgramCSV/SystemError.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SystemError* obj = T_NEW_D CF_SystemError();
		CSVDataSourceBase::SetIndex("SystemError", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/ProgramCSV/SystemError.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/ProgramCSV/SystemError.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/ProgramCSV/WorldMapList.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_WorldMapList* obj = T_NEW_D CF_WorldMapList();
		CSVDataSourceBase::SetIndex("WorldMapList", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/ProgramCSV/WorldMapList.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/ProgramCSV/WorldMapList.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/ProgramCSV/UITemplate.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_UITemplate* obj = T_NEW_D CF_UITemplate();
		CSVDataSourceBase::SetIndex("UITemplate", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/ProgramCSV/UITemplate.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/ProgramCSV/UITemplate.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/ProgramCSV/MapCategory.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_MapCategory* obj = T_NEW_D CF_MapCategory();
		CSVDataSourceBase::SetIndex("MapCategory", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/ProgramCSV/MapCategory.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/ProgramCSV/MapCategory.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/ProgramCSV/NPCFunctionType.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_NPCFunctionType* obj = T_NEW_D CF_NPCFunctionType();
		CSVDataSourceBase::SetIndex("NPCFunctionType", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/ProgramCSV/NPCFunctionType.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/ProgramCSV/NPCFunctionType.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/SoundSequence.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SoundSequence* obj = T_NEW_D CF_SoundSequence();
		CSVDataSourceBase::SetIndex("SoundSequence", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/SoundSequence.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/SoundSequence.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/TreasurePanel.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_TreasurePanel* obj = T_NEW_D CF_TreasurePanel();
		CSVDataSourceBase::SetIndex("TreasurePanel", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/TreasurePanel.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/TreasurePanel.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/SacrificeReward.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SacrificeReward* obj = T_NEW_D CF_SacrificeReward();
		CSVDataSourceBase::SetIndex("SacrificeReward", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/SacrificeReward.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/SacrificeReward.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Guild/GuildClash.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_GuildClash* obj = T_NEW_D CF_GuildClash();
		CSVDataSourceBase::SetIndex("GuildClash", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Guild/GuildClash.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Guild/GuildClash.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/SpecialEquipAvatar.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SpecialEquipAvatar* obj = T_NEW_D CF_SpecialEquipAvatar();
		CSVDataSourceBase::SetIndex("SpecialEquipAvatar", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/SpecialEquipAvatar.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/SpecialEquipAvatar.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/ResidentImage.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ResidentImage* obj = T_NEW_D CF_ResidentImage();
		CSVDataSourceBase::SetIndex("ResidentImage", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/ResidentImage.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/ResidentImage.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/ClientNPCIdleAction.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ClientNPCIdleAction* obj = T_NEW_D CF_ClientNPCIdleAction();
		CSVDataSourceBase::SetIndex("ClientNPCIdleAction", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/ClientNPCIdleAction.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/ClientNPCIdleAction.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/FunctionPreview.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_FunctionPreview* obj = T_NEW_D CF_FunctionPreview();
		CSVDataSourceBase::SetIndex("FunctionPreview", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/FunctionPreview.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/FunctionPreview.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/WalkTransferMap.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_WalkTransferMap* obj = T_NEW_D CF_WalkTransferMap();
		CSVDataSourceBase::SetIndex("WalkTransferMap", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/WalkTransferMap.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/WalkTransferMap.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/SmartHook.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SmartHook* obj = T_NEW_D CF_SmartHook();
		CSVDataSourceBase::SetIndex("SmartHook", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/SmartHook.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/SmartHook.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/SecretSkillClient.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SecretSkillClient* obj = T_NEW_D CF_SecretSkillClient();
		CSVDataSourceBase::SetIndex("SecretSkillClient", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/SecretSkillClient.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/SecretSkillClient.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/UIFontColor.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_UIFontColor* obj = T_NEW_D CF_UIFontColor();
		CSVDataSourceBase::SetIndex("UIFontColor", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/UIFontColor.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/UIFontColor.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/UINameFilter.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_UINameFilter* obj = T_NEW_D CF_UINameFilter();
		CSVDataSourceBase::SetIndex("UINameFilter", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/UINameFilter.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/UINameFilter.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/UIAreaNameFilter.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_UIAreaNameFilter* obj = T_NEW_D CF_UIAreaNameFilter();
		CSVDataSourceBase::SetIndex("UIAreaNameFilter", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/UIAreaNameFilter.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/UIAreaNameFilter.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/OfflineExp/OffLineExp.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_OffLineExp* obj = T_NEW_D CF_OffLineExp();
		CSVDataSourceBase::SetIndex("OffLineExp", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/OfflineExp/OffLineExp.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/OfflineExp/OffLineExp.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/OfflineExp/OffLineExpRatio.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_OffLineExpRatio* obj = T_NEW_D CF_OffLineExpRatio();
		CSVDataSourceBase::SetIndex("OffLineExpRatio", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/OfflineExp/OffLineExpRatio.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/OfflineExp/OffLineExpRatio.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/MysticalShop/MysticalShop.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_MysticalShop* obj = T_NEW_D CF_MysticalShop();
		CSVDataSourceBase::SetIndex("MysticalShop", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/MysticalShop/MysticalShop.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/MysticalShop/MysticalShop.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/MysticalShop/MysticalShopReset.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_MysticalShopReset* obj = T_NEW_D CF_MysticalShopReset();
		CSVDataSourceBase::SetIndex("MysticalShopReset", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/MysticalShop/MysticalShopReset.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/MysticalShop/MysticalShopReset.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/MysticalShop/MysticalShopVip.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_MysticalShopVip* obj = T_NEW_D CF_MysticalShopVip();
		CSVDataSourceBase::SetIndex("MysticalShopVip", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/MysticalShop/MysticalShopVip.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/MysticalShop/MysticalShopVip.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/MysticalShop/MysticalShopRefresh.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_MysticalShopRefresh* obj = T_NEW_D CF_MysticalShopRefresh();
		CSVDataSourceBase::SetIndex("MysticalShopRefresh", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/MysticalShop/MysticalShopRefresh.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/MysticalShop/MysticalShopRefresh.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/CameraCut.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_CameraCut* obj = T_NEW_D CF_CameraCut();
		CSVDataSourceBase::SetIndex("CameraCut", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/CameraCut.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/CameraCut.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/GameNotice.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_GameNotice* obj = T_NEW_D CF_GameNotice();
		CSVDataSourceBase::SetIndex("GameNotice", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/GameNotice.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/GameNotice.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/GfxList.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_GfxList* obj = T_NEW_D CF_GfxList();
		CSVDataSourceBase::SetIndex("GfxList", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/GfxList.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/GfxList.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Schedule/CSVSchedule.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_CSVSchedule* obj = T_NEW_D CF_CSVSchedule();
		CSVDataSourceBase::SetIndex("CSVSchedule", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Schedule/CSVSchedule.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Schedule/CSVSchedule.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/ChatCooldown.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ChatCooldown* obj = T_NEW_D CF_ChatCooldown();
		CSVDataSourceBase::SetIndex("ChatCooldown", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/ChatCooldown.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/ChatCooldown.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/VIPPrivilegeCard.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_VIPPrivilegeCard* obj = T_NEW_D CF_VIPPrivilegeCard();
		CSVDataSourceBase::SetIndex("VIPPrivilegeCard", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/VIPPrivilegeCard.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/VIPPrivilegeCard.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/fashion/Fashion.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_Fashion* obj = T_NEW_D CF_Fashion();
		CSVDataSourceBase::SetIndex("Fashion", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/fashion/Fashion.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/fashion/Fashion.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/fashion/FashionSuit.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_FashionSuit* obj = T_NEW_D CF_FashionSuit();
		CSVDataSourceBase::SetIndex("FashionSuit", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/fashion/FashionSuit.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/fashion/FashionSuit.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/fashion/FashionBag.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_FashionBag* obj = T_NEW_D CF_FashionBag();
		CSVDataSourceBase::SetIndex("FashionBag", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/fashion/FashionBag.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/fashion/FashionBag.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/SoulRefine/SoulRefinePage.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SoulRefinePage* obj = T_NEW_D CF_SoulRefinePage();
		CSVDataSourceBase::SetIndex("SoulRefinePage", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/SoulRefine/SoulRefinePage.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/SoulRefine/SoulRefinePage.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/SoulRefine/SoulRefineLevelUp.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SoulRefineLevelUp* obj = T_NEW_D CF_SoulRefineLevelUp();
		CSVDataSourceBase::SetIndex("SoulRefineLevelUp", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/SoulRefine/SoulRefineLevelUp.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/SoulRefine/SoulRefineLevelUp.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/SoulRefine/SoulRefineMoster.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SoulRefineMoster* obj = T_NEW_D CF_SoulRefineMoster();
		CSVDataSourceBase::SetIndex("SoulRefineMoster", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/SoulRefine/SoulRefineMoster.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/SoulRefine/SoulRefineMoster.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/MasterBliss/MasterBliss.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_MasterBliss* obj = T_NEW_D CF_MasterBliss();
		CSVDataSourceBase::SetIndex("MasterBliss", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/MasterBliss/MasterBliss.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/MasterBliss/MasterBliss.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/MasterBliss/MasterBlissScore.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_MasterBlissScore* obj = T_NEW_D CF_MasterBlissScore();
		CSVDataSourceBase::SetIndex("MasterBlissScore", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/MasterBliss/MasterBlissScore.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/MasterBliss/MasterBlissScore.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/MasterBliss/MasterBlissBuff.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_MasterBlissBuff* obj = T_NEW_D CF_MasterBlissBuff();
		CSVDataSourceBase::SetIndex("MasterBlissBuff", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/MasterBliss/MasterBlissBuff.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/MasterBliss/MasterBlissBuff.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/MasterBliss/MasterBlissPosition.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_MasterBlissPosition* obj = T_NEW_D CF_MasterBlissPosition();
		CSVDataSourceBase::SetIndex("MasterBlissPosition", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/MasterBliss/MasterBlissPosition.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/MasterBliss/MasterBlissPosition.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/YunYingPlatformConfig.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_YunYingPlatformConfig* obj = T_NEW_D CF_YunYingPlatformConfig();
		CSVDataSourceBase::SetIndex("YunYingPlatformConfig", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/YunYingPlatformConfig.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/YunYingPlatformConfig.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/YunYingOpenUrlConfig.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_YunYingOpenUrlConfig* obj = T_NEW_D CF_YunYingOpenUrlConfig();
		CSVDataSourceBase::SetIndex("YunYingOpenUrlConfig", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/YunYingOpenUrlConfig.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/YunYingOpenUrlConfig.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/TwToZn.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_TwToZn* obj = T_NEW_D CF_TwToZn();
		CSVDataSourceBase::SetIndex("TwToZn", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/TwToZn.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/TwToZn.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/ItemMelt.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ItemMelt* obj = T_NEW_D CF_ItemMelt();
		CSVDataSourceBase::SetIndex("ItemMelt", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/ItemMelt.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/ItemMelt.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/MeltLevelUp.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_MeltLevelUp* obj = T_NEW_D CF_MeltLevelUp();
		CSVDataSourceBase::SetIndex("MeltLevelUp", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/MeltLevelUp.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/MeltLevelUp.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/EffectTrigger.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_EffectTrigger* obj = T_NEW_D CF_EffectTrigger();
		CSVDataSourceBase::SetIndex("EffectTrigger", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/EffectTrigger.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/EffectTrigger.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Activity/SubordinateConsume/ACTSubordinateConsume.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ACTSubordinateConsume* obj = T_NEW_D CF_ACTSubordinateConsume();
		CSVDataSourceBase::SetIndex("ACTSubordinateConsume", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Activity/SubordinateConsume/ACTSubordinateConsume.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Activity/SubordinateConsume/ACTSubordinateConsume.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Buddy.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_Buddy* obj = T_NEW_D CF_Buddy();
		CSVDataSourceBase::SetIndex("Buddy", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Buddy.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Buddy.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/BuddyUpgrade.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_BuddyUpgrade* obj = T_NEW_D CF_BuddyUpgrade();
		CSVDataSourceBase::SetIndex("BuddyUpgrade", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/BuddyUpgrade.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/BuddyUpgrade.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/CharSkillWASDAnim.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_CharSkillWASDAnim* obj = T_NEW_D CF_CharSkillWASDAnim();
		CSVDataSourceBase::SetIndex("CharSkillWASDAnim", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/CharSkillWASDAnim.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/CharSkillWASDAnim.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/EquipStar.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_EquipStar* obj = T_NEW_D CF_EquipStar();
		CSVDataSourceBase::SetIndex("EquipStar", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/EquipStar.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/EquipStar.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/EquipSlotStar.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_EquipSlotStar* obj = T_NEW_D CF_EquipSlotStar();
		CSVDataSourceBase::SetIndex("EquipSlotStar", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/EquipSlotStar.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/EquipSlotStar.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/BuddyUnlock.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_BuddyUnlock* obj = T_NEW_D CF_BuddyUnlock();
		CSVDataSourceBase::SetIndex("BuddyUnlock", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/BuddyUnlock.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/BuddyUnlock.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/BuddyLimit.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_BuddyLimit* obj = T_NEW_D CF_BuddyLimit();
		CSVDataSourceBase::SetIndex("BuddyLimit", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/BuddyLimit.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/BuddyLimit.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/GfxSmoothRelease.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_GfxSmoothRelease* obj = T_NEW_D CF_GfxSmoothRelease();
		CSVDataSourceBase::SetIndex("GfxSmoothRelease", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/GfxSmoothRelease.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/GfxSmoothRelease.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/DailyActivity.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_DailyActivity* obj = T_NEW_D CF_DailyActivity();
		CSVDataSourceBase::SetIndex("DailyActivity", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/DailyActivity.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/DailyActivity.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/AchievementRewards.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_AchievementRewards* obj = T_NEW_D CF_AchievementRewards();
		CSVDataSourceBase::SetIndex("AchievementRewards", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/AchievementRewards.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/AchievementRewards.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/SignInDailyReward.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SignInDailyReward* obj = T_NEW_D CF_SignInDailyReward();
		CSVDataSourceBase::SetIndex("SignInDailyReward", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/SignInDailyReward.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/SignInDailyReward.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/StarVeinList.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_StarVeinList* obj = T_NEW_D CF_StarVeinList();
		CSVDataSourceBase::SetIndex("StarVeinList", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/StarVeinList.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/StarVeinList.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/FirstDailyCharge.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_FirstDailyCharge* obj = T_NEW_D CF_FirstDailyCharge();
		CSVDataSourceBase::SetIndex("FirstDailyCharge", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/FirstDailyCharge.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/FirstDailyCharge.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/SkillBookInitAttribute.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SkillBookInitAttribute* obj = T_NEW_D CF_SkillBookInitAttribute();
		CSVDataSourceBase::SetIndex("SkillBookInitAttribute", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/SkillBookInitAttribute.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/SkillBookInitAttribute.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/BuddyGrowGift.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_BuddyGrowGift* obj = T_NEW_D CF_BuddyGrowGift();
		CSVDataSourceBase::SetIndex("BuddyGrowGift", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/BuddyGrowGift.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/BuddyGrowGift.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Pet.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_Pet* obj = T_NEW_D CF_Pet();
		CSVDataSourceBase::SetIndex("Pet", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Pet.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Pet.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/PetType.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_PetType* obj = T_NEW_D CF_PetType();
		CSVDataSourceBase::SetIndex("PetType", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/PetType.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/PetType.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/PetQuality.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_PetQuality* obj = T_NEW_D CF_PetQuality();
		CSVDataSourceBase::SetIndex("PetQuality", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/PetQuality.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/PetQuality.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/PetUpgrade.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_PetUpgrade* obj = T_NEW_D CF_PetUpgrade();
		CSVDataSourceBase::SetIndex("PetUpgrade", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/PetUpgrade.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/PetUpgrade.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/CollectionActivation.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_CollectionActivation* obj = T_NEW_D CF_CollectionActivation();
		CSVDataSourceBase::SetIndex("CollectionActivation", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/CollectionActivation.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/CollectionActivation.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/CollectionReward.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_CollectionReward* obj = T_NEW_D CF_CollectionReward();
		CSVDataSourceBase::SetIndex("CollectionReward", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/CollectionReward.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/CollectionReward.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/ElementDamage.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ElementDamage* obj = T_NEW_D CF_ElementDamage();
		CSVDataSourceBase::SetIndex("ElementDamage", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/ElementDamage.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/ElementDamage.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/BossFirstKill.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_BossFirstKill* obj = T_NEW_D CF_BossFirstKill();
		CSVDataSourceBase::SetIndex("BossFirstKill", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/BossFirstKill.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/BossFirstKill.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/SevenDaysReward.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SevenDaysReward* obj = T_NEW_D CF_SevenDaysReward();
		CSVDataSourceBase::SetIndex("SevenDaysReward", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/SevenDaysReward.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/SevenDaysReward.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/SkillBulletFuncRandom.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SkillBulletFuncRandom* obj = T_NEW_D CF_SkillBulletFuncRandom();
		CSVDataSourceBase::SetIndex("SkillBulletFuncRandom", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/SkillBulletFuncRandom.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/SkillBulletFuncRandom.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/SkillBookUnlock.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SkillBookUnlock* obj = T_NEW_D CF_SkillBookUnlock();
		CSVDataSourceBase::SetIndex("SkillBookUnlock", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/SkillBookUnlock.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/SkillBookUnlock.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/EndlessTower.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_EndlessTower* obj = T_NEW_D CF_EndlessTower();
		CSVDataSourceBase::SetIndex("EndlessTower", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/EndlessTower.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/EndlessTower.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/TaskChapters.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_TaskChapters* obj = T_NEW_D CF_TaskChapters();
		CSVDataSourceBase::SetIndex("TaskChapters", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/TaskChapters.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/TaskChapters.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/TaskRankRewards.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_TaskRankRewards* obj = T_NEW_D CF_TaskRankRewards();
		CSVDataSourceBase::SetIndex("TaskRankRewards", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/TaskRankRewards.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/TaskRankRewards.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Bot.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_Bot* obj = T_NEW_D CF_Bot();
		CSVDataSourceBase::SetIndex("Bot", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Bot.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Bot.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/BotUpgrade.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_BotUpgrade* obj = T_NEW_D CF_BotUpgrade();
		CSVDataSourceBase::SetIndex("BotUpgrade", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/BotUpgrade.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/BotUpgrade.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/EquipSlotReinforce.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_EquipSlotReinforce* obj = T_NEW_D CF_EquipSlotReinforce();
		CSVDataSourceBase::SetIndex("EquipSlotReinforce", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/EquipSlotReinforce.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/EquipSlotReinforce.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/RuneLevelUp.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_RuneLevelUp* obj = T_NEW_D CF_RuneLevelUp();
		CSVDataSourceBase::SetIndex("RuneLevelUp", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/RuneLevelUp.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/RuneLevelUp.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/RuneStar.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_RuneStar* obj = T_NEW_D CF_RuneStar();
		CSVDataSourceBase::SetIndex("RuneStar", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/RuneStar.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/RuneStar.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Biography.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_Biography* obj = T_NEW_D CF_Biography();
		CSVDataSourceBase::SetIndex("Biography", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Biography.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Biography.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/BiographyTask.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_BiographyTask* obj = T_NEW_D CF_BiographyTask();
		CSVDataSourceBase::SetIndex("BiographyTask", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/BiographyTask.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/BiographyTask.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/RuneList.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_RuneList* obj = T_NEW_D CF_RuneList();
		CSVDataSourceBase::SetIndex("RuneList", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/RuneList.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/RuneList.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/TaskReachRewards.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_TaskReachRewards* obj = T_NEW_D CF_TaskReachRewards();
		CSVDataSourceBase::SetIndex("TaskReachRewards", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/TaskReachRewards.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/TaskReachRewards.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/TimeLimitedActivity.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_TimeLimitedActivity* obj = T_NEW_D CF_TimeLimitedActivity();
		CSVDataSourceBase::SetIndex("TimeLimitedActivity", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/TimeLimitedActivity.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/TimeLimitedActivity.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Activity/TimeShop/ACTShopTimeLimited.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ACTShopTimeLimited* obj = T_NEW_D CF_ACTShopTimeLimited();
		CSVDataSourceBase::SetIndex("ACTShopTimeLimited", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Activity/TimeShop/ACTShopTimeLimited.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Activity/TimeShop/ACTShopTimeLimited.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/HolyDeed.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_HolyDeed* obj = T_NEW_D CF_HolyDeed();
		CSVDataSourceBase::SetIndex("HolyDeed", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/HolyDeed.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/HolyDeed.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/StarVeinGoalList.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_StarVeinGoalList* obj = T_NEW_D CF_StarVeinGoalList();
		CSVDataSourceBase::SetIndex("StarVeinGoalList", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/StarVeinGoalList.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/StarVeinGoalList.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/SevenDaysTask.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SevenDaysTask* obj = T_NEW_D CF_SevenDaysTask();
		CSVDataSourceBase::SetIndex("SevenDaysTask", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/SevenDaysTask.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/SevenDaysTask.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/RankRewardGrade.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_RankRewardGrade* obj = T_NEW_D CF_RankRewardGrade();
		CSVDataSourceBase::SetIndex("RankRewardGrade", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/RankRewardGrade.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/RankRewardGrade.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/ShopGroupOpenSetting.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ShopGroupOpenSetting* obj = T_NEW_D CF_ShopGroupOpenSetting();
		CSVDataSourceBase::SetIndex("ShopGroupOpenSetting", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/ShopGroupOpenSetting.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/ShopGroupOpenSetting.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/VIPProfitDescribe.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_VIPProfitDescribe* obj = T_NEW_D CF_VIPProfitDescribe();
		CSVDataSourceBase::SetIndex("VIPProfitDescribe", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/VIPProfitDescribe.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/VIPProfitDescribe.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/InvestmentPlan.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_InvestmentPlan* obj = T_NEW_D CF_InvestmentPlan();
		CSVDataSourceBase::SetIndex("InvestmentPlan", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/InvestmentPlan.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/InvestmentPlan.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/InvestmentCardTask.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_InvestmentCardTask* obj = T_NEW_D CF_InvestmentCardTask();
		CSVDataSourceBase::SetIndex("InvestmentCardTask", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/InvestmentCardTask.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/InvestmentCardTask.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/InvestmentCard.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_InvestmentCard* obj = T_NEW_D CF_InvestmentCard();
		CSVDataSourceBase::SetIndex("InvestmentCard", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/InvestmentCard.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/InvestmentCard.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/BossInvadeRankReward.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_BossInvadeRankReward* obj = T_NEW_D CF_BossInvadeRankReward();
		CSVDataSourceBase::SetIndex("BossInvadeRankReward", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/BossInvadeRankReward.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/BossInvadeRankReward.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/BossInvade.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_BossInvade* obj = T_NEW_D CF_BossInvade();
		CSVDataSourceBase::SetIndex("BossInvade", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/BossInvade.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/BossInvade.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/VipNew.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_VipNew* obj = T_NEW_D CF_VipNew();
		CSVDataSourceBase::SetIndex("VipNew", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/VipNew.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/VipNew.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/HookMaps.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_HookMaps* obj = T_NEW_D CF_HookMaps();
		CSVDataSourceBase::SetIndex("HookMaps", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/HookMaps.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/HookMaps.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/RecommendHero.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_RecommendHero* obj = T_NEW_D CF_RecommendHero();
		CSVDataSourceBase::SetIndex("RecommendHero", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/RecommendHero.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/RecommendHero.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/TestDatabase.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_TestDatabase* obj = T_NEW_D CF_TestDatabase();
		CSVDataSourceBase::SetIndex("TestDatabase", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/TestDatabase.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/TestDatabase.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/TestAccPointRewards.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_TestAccPointRewards* obj = T_NEW_D CF_TestAccPointRewards();
		CSVDataSourceBase::SetIndex("TestAccPointRewards", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/TestAccPointRewards.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/TestAccPointRewards.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/TestRankReward.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_TestRankReward* obj = T_NEW_D CF_TestRankReward();
		CSVDataSourceBase::SetIndex("TestRankReward", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/TestRankReward.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/TestRankReward.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/TestRankGuildReward.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_TestRankGuildReward* obj = T_NEW_D CF_TestRankGuildReward();
		CSVDataSourceBase::SetIndex("TestRankGuildReward", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/TestRankGuildReward.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/TestRankGuildReward.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/PromoteCombat.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_PromoteCombat* obj = T_NEW_D CF_PromoteCombat();
		CSVDataSourceBase::SetIndex("PromoteCombat", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/PromoteCombat.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/PromoteCombat.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/ExportSoloDuleRobotInfo.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ExportSoloDuleRobotInfo* obj = T_NEW_D CF_ExportSoloDuleRobotInfo();
		CSVDataSourceBase::SetIndex("ExportSoloDuleRobotInfo", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/ExportSoloDuleRobotInfo.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/ExportSoloDuleRobotInfo.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/ExportRobotBuddyTemplateInfo.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ExportRobotBuddyTemplateInfo* obj = T_NEW_D CF_ExportRobotBuddyTemplateInfo();
		CSVDataSourceBase::SetIndex("ExportRobotBuddyTemplateInfo", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/ExportRobotBuddyTemplateInfo.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/ExportRobotBuddyTemplateInfo.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/ExportSoloDuleRobotProperty.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ExportSoloDuleRobotProperty* obj = T_NEW_D CF_ExportSoloDuleRobotProperty();
		CSVDataSourceBase::SetIndex("ExportSoloDuleRobotProperty", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/ExportSoloDuleRobotProperty.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/ExportSoloDuleRobotProperty.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/ShotActivityLimitRankRewards.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ShotActivityLimitRankRewards* obj = T_NEW_D CF_ShotActivityLimitRankRewards();
		CSVDataSourceBase::SetIndex("ShotActivityLimitRankRewards", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/ShotActivityLimitRankRewards.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/ShotActivityLimitRankRewards.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/CommanderTitle.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_CommanderTitle* obj = T_NEW_D CF_CommanderTitle();
		CSVDataSourceBase::SetIndex("CommanderTitle", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/CommanderTitle.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/CommanderTitle.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/SevenDaysTotalReward.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SevenDaysTotalReward* obj = T_NEW_D CF_SevenDaysTotalReward();
		CSVDataSourceBase::SetIndex("SevenDaysTotalReward", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/SevenDaysTotalReward.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/SevenDaysTotalReward.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/SpecialTraining.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SpecialTraining* obj = T_NEW_D CF_SpecialTraining();
		CSVDataSourceBase::SetIndex("SpecialTraining", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/SpecialTraining.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/SpecialTraining.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/SevenDaysLoginReward.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SevenDaysLoginReward* obj = T_NEW_D CF_SevenDaysLoginReward();
		CSVDataSourceBase::SetIndex("SevenDaysLoginReward", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/SevenDaysLoginReward.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/SevenDaysLoginReward.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/SoulConnect.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SoulConnect* obj = T_NEW_D CF_SoulConnect();
		CSVDataSourceBase::SetIndex("SoulConnect", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/SoulConnect.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/SoulConnect.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/HeadFrame.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_HeadFrame* obj = T_NEW_D CF_HeadFrame();
		CSVDataSourceBase::SetIndex("HeadFrame", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/HeadFrame.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/HeadFrame.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/GloryExpedition.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_GloryExpedition* obj = T_NEW_D CF_GloryExpedition();
		CSVDataSourceBase::SetIndex("GloryExpedition", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/GloryExpedition.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/GloryExpedition.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/EquipUpgrade.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_EquipUpgrade* obj = T_NEW_D CF_EquipUpgrade();
		CSVDataSourceBase::SetIndex("EquipUpgrade", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/EquipUpgrade.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/EquipUpgrade.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/BuddyRuneRecommend.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_BuddyRuneRecommend* obj = T_NEW_D CF_BuddyRuneRecommend();
		CSVDataSourceBase::SetIndex("BuddyRuneRecommend", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/BuddyRuneRecommend.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/BuddyRuneRecommend.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/EquipCommanderJinglian.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_EquipCommanderJinglian* obj = T_NEW_D CF_EquipCommanderJinglian();
		CSVDataSourceBase::SetIndex("EquipCommanderJinglian", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/EquipCommanderJinglian.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/EquipCommanderJinglian.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/EquipCommanderSlotReinforce.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_EquipCommanderSlotReinforce* obj = T_NEW_D CF_EquipCommanderSlotReinforce();
		CSVDataSourceBase::SetIndex("EquipCommanderSlotReinforce", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/EquipCommanderSlotReinforce.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/EquipCommanderSlotReinforce.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/EquipCommanderJinglianSuit.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_EquipCommanderJinglianSuit* obj = T_NEW_D CF_EquipCommanderJinglianSuit();
		CSVDataSourceBase::SetIndex("EquipCommanderJinglianSuit", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/EquipCommanderJinglianSuit.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/EquipCommanderJinglianSuit.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/EquipCommanderSlotReinforceSuit.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_EquipCommanderSlotReinforceSuit* obj = T_NEW_D CF_EquipCommanderSlotReinforceSuit();
		CSVDataSourceBase::SetIndex("EquipCommanderSlotReinforceSuit", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/EquipCommanderSlotReinforceSuit.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/EquipCommanderSlotReinforceSuit.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/YiJiTuXiStageReward.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_YiJiTuXiStageReward* obj = T_NEW_D CF_YiJiTuXiStageReward();
		CSVDataSourceBase::SetIndex("YiJiTuXiStageReward", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/YiJiTuXiStageReward.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/YiJiTuXiStageReward.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/YiJiXunBaoStageReward.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_YiJiXunBaoStageReward* obj = T_NEW_D CF_YiJiXunBaoStageReward();
		CSVDataSourceBase::SetIndex("YiJiXunBaoStageReward", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/YiJiXunBaoStageReward.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/YiJiXunBaoStageReward.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/CommanderCombatPK.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_CommanderCombatPK* obj = T_NEW_D CF_CommanderCombatPK();
		CSVDataSourceBase::SetIndex("CommanderCombatPK", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/CommanderCombatPK.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/CommanderCombatPK.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/RotaryTable.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_RotaryTable* obj = T_NEW_D CF_RotaryTable();
		CSVDataSourceBase::SetIndex("RotaryTable", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/RotaryTable.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/RotaryTable.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/RotaryTableReward.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_RotaryTableReward* obj = T_NEW_D CF_RotaryTableReward();
		CSVDataSourceBase::SetIndex("RotaryTableReward", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/RotaryTableReward.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/RotaryTableReward.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/NonpareilItem.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_NonpareilItem* obj = T_NEW_D CF_NonpareilItem();
		CSVDataSourceBase::SetIndex("NonpareilItem", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/NonpareilItem.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/NonpareilItem.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/BestGuild/BestGuild.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_BestGuild* obj = T_NEW_D CF_BestGuild();
		CSVDataSourceBase::SetIndex("BestGuild", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/BestGuild/BestGuild.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/BestGuild/BestGuild.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/BestGuild/BestGuildTask.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_BestGuildTask* obj = T_NEW_D CF_BestGuildTask();
		CSVDataSourceBase::SetIndex("BestGuildTask", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/BestGuild/BestGuildTask.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/BestGuild/BestGuildTask.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/BestGuild/BestGuildGuildRankReward.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_BestGuildGuildRankReward* obj = T_NEW_D CF_BestGuildGuildRankReward();
		CSVDataSourceBase::SetIndex("BestGuildGuildRankReward", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/BestGuild/BestGuildGuildRankReward.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/BestGuild/BestGuildGuildRankReward.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/BestGuild/BestGuildPersonalRankReward.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_BestGuildPersonalRankReward* obj = T_NEW_D CF_BestGuildPersonalRankReward();
		CSVDataSourceBase::SetIndex("BestGuildPersonalRankReward", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/BestGuild/BestGuildPersonalRankReward.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/BestGuild/BestGuildPersonalRankReward.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/BestGuild/BestGuildBossRankRewards.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_BestGuildBossRankRewards* obj = T_NEW_D CF_BestGuildBossRankRewards();
		CSVDataSourceBase::SetIndex("BestGuildBossRankRewards", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/BestGuild/BestGuildBossRankRewards.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/BestGuild/BestGuildBossRankRewards.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/BestGuild/BestGuildBossInfo.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_BestGuildBossInfo* obj = T_NEW_D CF_BestGuildBossInfo();
		CSVDataSourceBase::SetIndex("BestGuildBossInfo", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/BestGuild/BestGuildBossInfo.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/BestGuild/BestGuildBossInfo.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/BestGuild/BestGuildRobReward.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_BestGuildRobReward* obj = T_NEW_D CF_BestGuildRobReward();
		CSVDataSourceBase::SetIndex("BestGuildRobReward", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/BestGuild/BestGuildRobReward.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/BestGuild/BestGuildRobReward.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/BestGuild/BestGuildDailyTask.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_BestGuildDailyTask* obj = T_NEW_D CF_BestGuildDailyTask();
		CSVDataSourceBase::SetIndex("BestGuildDailyTask", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/BestGuild/BestGuildDailyTask.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/BestGuild/BestGuildDailyTask.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/RegularBoss.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_RegularBoss* obj = T_NEW_D CF_RegularBoss();
		CSVDataSourceBase::SetIndex("RegularBoss", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/RegularBoss.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/RegularBoss.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/EquipCommanderSlotMoldingSpirit.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_EquipCommanderSlotMoldingSpirit* obj = T_NEW_D CF_EquipCommanderSlotMoldingSpirit();
		CSVDataSourceBase::SetIndex("EquipCommanderSlotMoldingSpirit", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/EquipCommanderSlotMoldingSpirit.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/EquipCommanderSlotMoldingSpirit.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/EquipCommanderSlotMoldingSpiritSuit.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_EquipCommanderSlotMoldingSpiritSuit* obj = T_NEW_D CF_EquipCommanderSlotMoldingSpiritSuit();
		CSVDataSourceBase::SetIndex("EquipCommanderSlotMoldingSpiritSuit", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/EquipCommanderSlotMoldingSpiritSuit.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/EquipCommanderSlotMoldingSpiritSuit.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/PetIslandStageReward.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_PetIslandStageReward* obj = T_NEW_D CF_PetIslandStageReward();
		CSVDataSourceBase::SetIndex("PetIslandStageReward", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/PetIslandStageReward.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/PetIslandStageReward.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/EscortReward.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_EscortReward* obj = T_NEW_D CF_EscortReward();
		CSVDataSourceBase::SetIndex("EscortReward", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/EscortReward.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/EscortReward.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/BestGuild/BestGuildGoal.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_BestGuildGoal* obj = T_NEW_D CF_BestGuildGoal();
		CSVDataSourceBase::SetIndex("BestGuildGoal", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/BestGuild/BestGuildGoal.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/BestGuild/BestGuildGoal.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/RankOpen.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_RankOpen* obj = T_NEW_D CF_RankOpen();
		CSVDataSourceBase::SetIndex("RankOpen", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/RankOpen.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/RankOpen.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/GloryExpeditionDailyRewards.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_GloryExpeditionDailyRewards* obj = T_NEW_D CF_GloryExpeditionDailyRewards();
		CSVDataSourceBase::SetIndex("GloryExpeditionDailyRewards", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/GloryExpeditionDailyRewards.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/GloryExpeditionDailyRewards.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/BestGuild/BestGuildChapterID.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_BestGuildChapterID* obj = T_NEW_D CF_BestGuildChapterID();
		CSVDataSourceBase::SetIndex("BestGuildChapterID", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/BestGuild/BestGuildChapterID.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/BestGuild/BestGuildChapterID.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/PromoteLevel.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_PromoteLevel* obj = T_NEW_D CF_PromoteLevel();
		CSVDataSourceBase::SetIndex("PromoteLevel", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/PromoteLevel.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/PromoteLevel.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/GloryRewardLink.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_GloryRewardLink* obj = T_NEW_D CF_GloryRewardLink();
		CSVDataSourceBase::SetIndex("GloryRewardLink", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/GloryRewardLink.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/GloryRewardLink.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/ComeBack.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ComeBack* obj = T_NEW_D CF_ComeBack();
		CSVDataSourceBase::SetIndex("ComeBack", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/ComeBack.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/ComeBack.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/EquipCommanderSuitCollection.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_EquipCommanderSuitCollection* obj = T_NEW_D CF_EquipCommanderSuitCollection();
		CSVDataSourceBase::SetIndex("EquipCommanderSuitCollection", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/EquipCommanderSuitCollection.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/EquipCommanderSuitCollection.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/NewAchievementReward.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_NewAchievementReward* obj = T_NEW_D CF_NewAchievementReward();
		CSVDataSourceBase::SetIndex("NewAchievementReward", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/NewAchievementReward.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/NewAchievementReward.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/TreasureAddAward.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_TreasureAddAward* obj = T_NEW_D CF_TreasureAddAward();
		CSVDataSourceBase::SetIndex("TreasureAddAward", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/TreasureAddAward.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/TreasureAddAward.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/BuddyTransform.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_BuddyTransform* obj = T_NEW_D CF_BuddyTransform();
		CSVDataSourceBase::SetIndex("BuddyTransform", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/BuddyTransform.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/BuddyTransform.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/YiJiTuXiRankRewards.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_YiJiTuXiRankRewards* obj = T_NEW_D CF_YiJiTuXiRankRewards();
		CSVDataSourceBase::SetIndex("YiJiTuXiRankRewards", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/YiJiTuXiRankRewards.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/YiJiTuXiRankRewards.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Guild/GuildCrystal.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_GuildCrystal* obj = T_NEW_D CF_GuildCrystal();
		CSVDataSourceBase::SetIndex("GuildCrystal", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Guild/GuildCrystal.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Guild/GuildCrystal.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Guild/GuildGift.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_GuildGift* obj = T_NEW_D CF_GuildGift();
		CSVDataSourceBase::SetIndex("GuildGift", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Guild/GuildGift.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Guild/GuildGift.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/TreasurePos.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_TreasurePos* obj = T_NEW_D CF_TreasurePos();
		CSVDataSourceBase::SetIndex("TreasurePos", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/TreasurePos.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/TreasurePos.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/AchievementTypeBig.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_AchievementTypeBig* obj = T_NEW_D CF_AchievementTypeBig();
		CSVDataSourceBase::SetIndex("AchievementTypeBig", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/AchievementTypeBig.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/AchievementTypeBig.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/AchievementTypeSmall.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_AchievementTypeSmall* obj = T_NEW_D CF_AchievementTypeSmall();
		CSVDataSourceBase::SetIndex("AchievementTypeSmall", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/AchievementTypeSmall.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/AchievementTypeSmall.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/TreasureEvent.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_TreasureEvent* obj = T_NEW_D CF_TreasureEvent();
		CSVDataSourceBase::SetIndex("TreasureEvent", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/TreasureEvent.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/TreasureEvent.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/TreasureItem.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_TreasureItem* obj = T_NEW_D CF_TreasureItem();
		CSVDataSourceBase::SetIndex("TreasureItem", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/TreasureItem.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/TreasureItem.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/EntrustTask.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_EntrustTask* obj = T_NEW_D CF_EntrustTask();
		CSVDataSourceBase::SetIndex("EntrustTask", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/EntrustTask.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/EntrustTask.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/SpecialBuy.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SpecialBuy* obj = T_NEW_D CF_SpecialBuy();
		CSVDataSourceBase::SetIndex("SpecialBuy", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/SpecialBuy.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/SpecialBuy.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Guild/GuildTower.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_GuildTower* obj = T_NEW_D CF_GuildTower();
		CSVDataSourceBase::SetIndex("GuildTower", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Guild/GuildTower.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Guild/GuildTower.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Guild/GuildTowerRankReward.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_GuildTowerRankReward* obj = T_NEW_D CF_GuildTowerRankReward();
		CSVDataSourceBase::SetIndex("GuildTowerRankReward", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Guild/GuildTowerRankReward.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Guild/GuildTowerRankReward.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/BuddyThemeActivity/BuddyThemeMain.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_BuddyThemeMain* obj = T_NEW_D CF_BuddyThemeMain();
		CSVDataSourceBase::SetIndex("BuddyThemeMain", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/BuddyThemeActivity/BuddyThemeMain.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/BuddyThemeActivity/BuddyThemeMain.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/BuddyThemeActivity/BuddyThemeSecondary.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_BuddyThemeSecondary* obj = T_NEW_D CF_BuddyThemeSecondary();
		CSVDataSourceBase::SetIndex("BuddyThemeSecondary", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/BuddyThemeActivity/BuddyThemeSecondary.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/BuddyThemeActivity/BuddyThemeSecondary.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/GameRestartNotice.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_GameRestartNotice* obj = T_NEW_D CF_GameRestartNotice();
		CSVDataSourceBase::SetIndex("GameRestartNotice", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/GameRestartNotice.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/GameRestartNotice.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/BuddyThemeActivity/BuddyThemeTaskType.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_BuddyThemeTaskType* obj = T_NEW_D CF_BuddyThemeTaskType();
		CSVDataSourceBase::SetIndex("BuddyThemeTaskType", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/BuddyThemeActivity/BuddyThemeTaskType.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/BuddyThemeActivity/BuddyThemeTaskType.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/BuddyThemeActivity/BuddyThemeBossRankReward.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_BuddyThemeBossRankReward* obj = T_NEW_D CF_BuddyThemeBossRankReward();
		CSVDataSourceBase::SetIndex("BuddyThemeBossRankReward", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/BuddyThemeActivity/BuddyThemeBossRankReward.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/BuddyThemeActivity/BuddyThemeBossRankReward.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/BuddyThemeActivity/BuddyThemeBossType.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_BuddyThemeBossType* obj = T_NEW_D CF_BuddyThemeBossType();
		CSVDataSourceBase::SetIndex("BuddyThemeBossType", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/BuddyThemeActivity/BuddyThemeBossType.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/BuddyThemeActivity/BuddyThemeBossType.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/BuddyThemeActivity/BuddyThemeTaskList.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_BuddyThemeTaskList* obj = T_NEW_D CF_BuddyThemeTaskList();
		CSVDataSourceBase::SetIndex("BuddyThemeTaskList", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/BuddyThemeActivity/BuddyThemeTaskList.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/BuddyThemeActivity/BuddyThemeTaskList.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Activity/TreasureHunt/ACTTreasureHunt.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ACTTreasureHunt* obj = T_NEW_D CF_ACTTreasureHunt();
		CSVDataSourceBase::SetIndex("ACTTreasureHunt", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Activity/TreasureHunt/ACTTreasureHunt.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Activity/TreasureHunt/ACTTreasureHunt.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Activity/TreasureHunt/ACTTreasureHuntStageReward.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ACTTreasureHuntStageReward* obj = T_NEW_D CF_ACTTreasureHuntStageReward();
		CSVDataSourceBase::SetIndex("ACTTreasureHuntStageReward", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Activity/TreasureHunt/ACTTreasureHuntStageReward.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Activity/TreasureHunt/ACTTreasureHuntStageReward.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/BossCamera.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_BossCamera* obj = T_NEW_D CF_BossCamera();
		CSVDataSourceBase::SetIndex("BossCamera", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/BossCamera.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/BossCamera.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/BossCameraDetail.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_BossCameraDetail* obj = T_NEW_D CF_BossCameraDetail();
		CSVDataSourceBase::SetIndex("BossCameraDetail", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/BossCameraDetail.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/BossCameraDetail.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/HookPos.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_HookPos* obj = T_NEW_D CF_HookPos();
		CSVDataSourceBase::SetIndex("HookPos", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/HookPos.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/HookPos.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Charge.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_Charge* obj = T_NEW_D CF_Charge();
		CSVDataSourceBase::SetIndex("Charge", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Charge.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Charge.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/MailInfo.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_MailInfo* obj = T_NEW_D CF_MailInfo();
		CSVDataSourceBase::SetIndex("MailInfo", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/MailInfo.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/MailInfo.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/TutorialRewards.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_TutorialRewards* obj = T_NEW_D CF_TutorialRewards();
		CSVDataSourceBase::SetIndex("TutorialRewards", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/TutorialRewards.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/TutorialRewards.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Subordinate/SubSystemInfo.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SubSystemInfo* obj = T_NEW_D CF_SubSystemInfo();
		CSVDataSourceBase::SetIndex("SubSystemInfo", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Subordinate/SubSystemInfo.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Subordinate/SubSystemInfo.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/SkyArenaRank.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SkyArenaRank* obj = T_NEW_D CF_SkyArenaRank();
		CSVDataSourceBase::SetIndex("SkyArenaRank", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/SkyArenaRank.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/SkyArenaRank.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/SkyArenaRankReward.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SkyArenaRankReward* obj = T_NEW_D CF_SkyArenaRankReward();
		CSVDataSourceBase::SetIndex("SkyArenaRankReward", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/SkyArenaRankReward.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/SkyArenaRankReward.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/SpecialTask.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SpecialTask* obj = T_NEW_D CF_SpecialTask();
		CSVDataSourceBase::SetIndex("SpecialTask", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/SpecialTask.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/SpecialTask.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Guess.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_Guess* obj = T_NEW_D CF_Guess();
		CSVDataSourceBase::SetIndex("Guess", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Guess.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Guess.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/SummitArenaMail.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SummitArenaMail* obj = T_NEW_D CF_SummitArenaMail();
		CSVDataSourceBase::SetIndex("SummitArenaMail", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/SummitArenaMail.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/SummitArenaMail.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/SummitArenaMatchName.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SummitArenaMatchName* obj = T_NEW_D CF_SummitArenaMatchName();
		CSVDataSourceBase::SetIndex("SummitArenaMatchName", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/SummitArenaMatchName.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/SummitArenaMatchName.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/SummitArenaRankRewards.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_SummitArenaRankRewards* obj = T_NEW_D CF_SummitArenaRankRewards();
		CSVDataSourceBase::SetIndex("SummitArenaRankRewards", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/SummitArenaRankRewards.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/SummitArenaRankRewards.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Scheme.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_Scheme* obj = T_NEW_D CF_Scheme();
		CSVDataSourceBase::SetIndex("Scheme", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Scheme.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Scheme.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/YiZhongJiangLinRankReward.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_YiZhongJiangLinRankReward* obj = T_NEW_D CF_YiZhongJiangLinRankReward();
		CSVDataSourceBase::SetIndex("YiZhongJiangLinRankReward", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/YiZhongJiangLinRankReward.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/YiZhongJiangLinRankReward.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/YiZhongJiangLinBossInformation.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_YiZhongJiangLinBossInformation* obj = T_NEW_D CF_YiZhongJiangLinBossInformation();
		CSVDataSourceBase::SetIndex("YiZhongJiangLinBossInformation", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/YiZhongJiangLinBossInformation.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/YiZhongJiangLinBossInformation.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/HolidayActivity/HolidayActivityMain.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_HolidayActivityMain* obj = T_NEW_D CF_HolidayActivityMain();
		CSVDataSourceBase::SetIndex("HolidayActivityMain", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/HolidayActivity/HolidayActivityMain.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/HolidayActivity/HolidayActivityMain.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/HolidayActivity/HolidayActivitySecondary.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_HolidayActivitySecondary* obj = T_NEW_D CF_HolidayActivitySecondary();
		CSVDataSourceBase::SetIndex("HolidayActivitySecondary", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/HolidayActivity/HolidayActivitySecondary.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/HolidayActivity/HolidayActivitySecondary.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/ActivityRankReward.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ActivityRankReward* obj = T_NEW_D CF_ActivityRankReward();
		CSVDataSourceBase::SetIndex("ActivityRankReward", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/ActivityRankReward.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/ActivityRankReward.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/HolidayActivity/HolidayActivity_ChargeContinuous.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_HolidayActivity_ChargeContinuous* obj = T_NEW_D CF_HolidayActivity_ChargeContinuous();
		CSVDataSourceBase::SetIndex("HolidayActivity_ChargeContinuous", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/HolidayActivity/HolidayActivity_ChargeContinuous.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/HolidayActivity/HolidayActivity_ChargeContinuous.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/HolidayActivity/HolidayActivity_PinTuUnlock.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_HolidayActivity_PinTuUnlock* obj = T_NEW_D CF_HolidayActivity_PinTuUnlock();
		CSVDataSourceBase::SetIndex("HolidayActivity_PinTuUnlock", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/HolidayActivity/HolidayActivity_PinTuUnlock.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/HolidayActivity/HolidayActivity_PinTuUnlock.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/HolidayActivity/HolidayActivity_PinTuReward.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_HolidayActivity_PinTuReward* obj = T_NEW_D CF_HolidayActivity_PinTuReward();
		CSVDataSourceBase::SetIndex("HolidayActivity_PinTuReward", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/HolidayActivity/HolidayActivity_PinTuReward.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/HolidayActivity/HolidayActivity_PinTuReward.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/HolidayActivity/HolidayActivityTaskType.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_HolidayActivityTaskType* obj = T_NEW_D CF_HolidayActivityTaskType();
		CSVDataSourceBase::SetIndex("HolidayActivityTaskType", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/HolidayActivity/HolidayActivityTaskType.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/HolidayActivity/HolidayActivityTaskType.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/HolidayActivity/HolidayActivityTaskList.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_HolidayActivityTaskList* obj = T_NEW_D CF_HolidayActivityTaskList();
		CSVDataSourceBase::SetIndex("HolidayActivityTaskList", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/HolidayActivity/HolidayActivityTaskList.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/HolidayActivity/HolidayActivityTaskList.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/RankCondition.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_RankCondition* obj = T_NEW_D CF_RankCondition();
		CSVDataSourceBase::SetIndex("RankCondition", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/RankCondition.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/RankCondition.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Activity/Shop/ACTShopB.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ACTShopB* obj = T_NEW_D CF_ACTShopB();
		CSVDataSourceBase::SetIndex("ACTShopB", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Activity/Shop/ACTShopB.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Activity/Shop/ACTShopB.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	{
#ifdef _SERVER_BUILD
		UT_SERVER_PROFILE(kProfile, "./Data/CSV/CSVServer/DesignCSV/Activity/ActivityTime.csv", 0.1f);
#endif //_SERVER_BUILD
		CF_ActivityTime* obj = T_NEW_D CF_ActivityTime();
		CSVDataSourceBase::SetIndex("ActivityTime", obj);
#ifdef _SERVER_BUILD
		if (obj->Load("./Data/CSV/CSVServer/DesignCSV/Activity/ActivityTime.csv") == false)
#else
		if (obj->Load("./Data/CSV/CSVClient/DesignCSV/Activity/ActivityTime.csv") == false)
#endif //_SERVER_BUILD
		{
			return false;
		}
	}
	return true;
}

void UnInit_Client_CSV_File()
{
	CSVDataSourceBase::ClearIndices();
	{
		CF_ActorFilterFunc* obj = CF_ActorFilterFunc::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_AreaBarrier* obj = CF_AreaBarrier::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_AreaEffect* obj = CF_AreaEffect::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_Subordinate* obj = CF_Subordinate::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SubordinateCluster* obj = CF_SubordinateCluster::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SubordinateTransform* obj = CF_SubordinateTransform::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SubRelateAttri* obj = CF_SubRelateAttri::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_AuctionCategory* obj = CF_AuctionCategory::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_AutoFitCharType* obj = CF_AutoFitCharType::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_CharType* obj = CF_CharType::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_CreateCharParam* obj = CF_CreateCharParam::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_Conversation* obj = CF_Conversation::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_CutScenes* obj = CF_CutScenes::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_CrossRealmMap* obj = CF_CrossRealmMap::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_EffectList* obj = CF_EffectList::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_EXPList* obj = CF_EXPList::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_Faction* obj = CF_Faction::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ForbiddenWordsList* obj = CF_ForbiddenWordsList::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_FriendMsgBroadContent* obj = CF_FriendMsgBroadContent::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_GameStory* obj = CF_GameStory::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_GameStoryClientUI* obj = CF_GameStoryClientUI::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_GameStoryRaidList* obj = CF_GameStoryRaidList::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_GameStoryMessage* obj = CF_GameStoryMessage::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_GuildLevel* obj = CF_GuildLevel::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_GuildCreateLimit* obj = CF_GuildCreateLimit::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_GuildWarBuff* obj = CF_GuildWarBuff::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_GuildGood* obj = CF_GuildGood::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_GlobalPowerGuildReward* obj = CF_GlobalPowerGuildReward::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_GemExp* obj = CF_GemExp::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_GemReinforceLevel* obj = CF_GemReinforceLevel::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_HittedSoundList* obj = CF_HittedSoundList::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ItemAssembleUseFunc* obj = CF_ItemAssembleUseFunc::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ItemAvartarFunc* obj = CF_ItemAvartarFunc::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ItemCategory* obj = CF_ItemCategory::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ItemDecompositionFunc* obj = CF_ItemDecompositionFunc::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ItemDisassembleUseFunc* obj = CF_ItemDisassembleUseFunc::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ItemEffectFunc* obj = CF_ItemEffectFunc::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ItemEffect* obj = CF_ItemEffect::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ItemEnduranceFunc* obj = CF_ItemEnduranceFunc::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ItemEquipLimitFunc* obj = CF_ItemEquipLimitFunc::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ItemExistenceFunc* obj = CF_ItemExistenceFunc::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ItemGenEffect* obj = CF_ItemGenEffect::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ItemGen* obj = CF_ItemGen::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ItemAttrActive* obj = CF_ItemAttrActive::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ItemList* obj = CF_ItemList::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ItemOperationFunc* obj = CF_ItemOperationFunc::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ItemPropertyFunc* obj = CF_ItemPropertyFunc::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ItemSuit* obj = CF_ItemSuit::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ItemUIAssembleUseFunc* obj = CF_ItemUIAssembleUseFunc::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ItemUseLimitFunc* obj = CF_ItemUseLimitFunc::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ItemReportFunc* obj = CF_ItemReportFunc::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_LevelUp* obj = CF_LevelUp::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_LoadingTip* obj = CF_LoadingTip::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_LogicAreaChunkIDList* obj = CF_LogicAreaChunkIDList::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_LogicAreaFunc* obj = CF_LogicAreaFunc::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_LogicTriggerList* obj = CF_LogicTriggerList::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_MailCharge* obj = CF_MailCharge::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_MainTaskList* obj = CF_MainTaskList::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_MillionWeapons* obj = CF_MillionWeapons::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_MonsterAIFunc* obj = CF_MonsterAIFunc::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_NPCAvatar* obj = CF_NPCAvatar::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_NPCFunction* obj = CF_NPCFunction::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_NPCInteractionConfig* obj = CF_NPCInteractionConfig::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_NPCShopList* obj = CF_NPCShopList::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_NPCSoundList* obj = CF_NPCSoundList::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_NPCTransferList* obj = CF_NPCTransferList::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_OpenBag* obj = CF_OpenBag::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_OpenLevel* obj = CF_OpenLevel::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ProductionFormula* obj = CF_ProductionFormula::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_CompositionFormula* obj = CF_CompositionFormula::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_RankReward* obj = CF_RankReward::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ShopList* obj = CF_ShopList::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SignInReward* obj = CF_SignInReward::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SocketItemFunc* obj = CF_SocketItemFunc::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SkillBookLevelUp* obj = CF_SkillBookLevelUp::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SkillBulletFunc* obj = CF_SkillBulletFunc::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SkillCDModifyFunc* obj = CF_SkillCDModifyFunc::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SkillExecuteList* obj = CF_SkillExecuteList::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SkillSpecialAttrib* obj = CF_SkillSpecialAttrib::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SkillLevelUp* obj = CF_SkillLevelUp::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SkillStageFunc* obj = CF_SkillStageFunc::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SkillStateList* obj = CF_SkillStateList::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SPLevelup* obj = CF_SPLevelup::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SystemTitleList* obj = CF_SystemTitleList::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_TaskCondition* obj = CF_TaskCondition::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_TaskList* obj = CF_TaskList::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_TextColor* obj = CF_TextColor::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_TextConfig* obj = CF_TextConfig::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_TranscriptionRule* obj = CF_TranscriptionRule::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_TransferArea* obj = CF_TransferArea::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_Tutorial* obj = CF_Tutorial::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_TranscriptionSettleGrade* obj = CF_TranscriptionSettleGrade::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_UIControlRelation* obj = CF_UIControlRelation::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_UISound* obj = CF_UISound::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_Vigor* obj = CF_Vigor::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_WarehouseCost* obj = CF_WarehouseCost::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_GodSuiteTask* obj = CF_GodSuiteTask::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_QQGirls* obj = CF_QQGirls::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_WorldObjectDesc* obj = CF_WorldObjectDesc::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_VIP* obj = CF_VIP::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_VIPProfit* obj = CF_VIPProfit::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_DailyLiveness* obj = CF_DailyLiveness::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SD_Call* obj = CF_SD_Call::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_TaskParam* obj = CF_TaskParam::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_TaskAssign* obj = CF_TaskAssign::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_CharPromote* obj = CF_CharPromote::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_VIPFDeposit* obj = CF_VIPFDeposit::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_CharAttributeFunc* obj = CF_CharAttributeFunc::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_GuildWarCatapult* obj = CF_GuildWarCatapult::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_GuildWarCrontab* obj = CF_GuildWarCrontab::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_GuildWarActivity* obj = CF_GuildWarActivity::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_TreasureChests* obj = CF_TreasureChests::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_TreasurePool* obj = CF_TreasurePool::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ForcesBattle* obj = CF_ForcesBattle::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_PowerUp* obj = CF_PowerUp::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_PowerUpFunc* obj = CF_PowerUpFunc::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_CommonStatReward* obj = CF_CommonStatReward::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_CommonStatReplevy* obj = CF_CommonStatReplevy::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_CommonStatConfig* obj = CF_CommonStatConfig::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_CommonStatCompensate* obj = CF_CommonStatCompensate::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_CommonStatBasicReward* obj = CF_CommonStatBasicReward::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ActivitySchedule* obj = CF_ActivitySchedule::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SecretTreasureSchedule* obj = CF_SecretTreasureSchedule::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SecretTreasureShop* obj = CF_SecretTreasureShop::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SecretTreasureChests* obj = CF_SecretTreasureChests::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SecretTreasureReward* obj = CF_SecretTreasureReward::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SecretTreasureTimes* obj = CF_SecretTreasureTimes::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ACTSubordinateReward* obj = CF_ACTSubordinateReward::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ACTSubordinateRewardA* obj = CF_ACTSubordinateRewardA::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ACTSubordinateRewardB* obj = CF_ACTSubordinateRewardB::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ACTChargeProfit* obj = CF_ACTChargeProfit::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ACTChargeProfitA* obj = CF_ACTChargeProfitA::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ACTChargeProfitB* obj = CF_ACTChargeProfitB::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ACTChargeProfitBReward* obj = CF_ACTChargeProfitBReward::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ACTChargeProfitType* obj = CF_ACTChargeProfitType::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ACTShop* obj = CF_ACTShop::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ACTShopA* obj = CF_ACTShopA::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ACTShot* obj = CF_ACTShot::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ACTShotA* obj = CF_ACTShotA::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ACTLottery* obj = CF_ACTLottery::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ACTLotteryReward* obj = CF_ACTLotteryReward::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ArenaSetting* obj = CF_ArenaSetting::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ArenaSection* obj = CF_ArenaSection::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ArenaTimes* obj = CF_ArenaTimes::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_YaZhiLevel* obj = CF_YaZhiLevel::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_GuildRaid* obj = CF_GuildRaid::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_GuildDonate* obj = CF_GuildDonate::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_WildPrinces* obj = CF_WildPrinces::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_TaskType* obj = CF_TaskType::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_Mental* obj = CF_Mental::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_UIOpenLevel* obj = CF_UIOpenLevel::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_CharTypeFilterMiniMap* obj = CF_CharTypeFilterMiniMap::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_AchievementList* obj = CF_AchievementList::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_StoneList* obj = CF_StoneList::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_BossShowPosition* obj = CF_BossShowPosition::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_MailItemList* obj = CF_MailItemList::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ShotActivity* obj = CF_ShotActivity::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ItemOperation* obj = CF_ItemOperation::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_NPCShopItems* obj = CF_NPCShopItems::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_FlowerConfig* obj = CF_FlowerConfig::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_FlowerTitleCfg* obj = CF_FlowerTitleCfg::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_FlowerGiftCfg* obj = CF_FlowerGiftCfg::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_FlowerSendLimitCfg* obj = CF_FlowerSendLimitCfg::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_LocalSpawnPoint* obj = CF_LocalSpawnPoint::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_LogicAreaSound* obj = CF_LogicAreaSound::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_MapEffectConfig* obj = CF_MapEffectConfig::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SoundObjectList* obj = CF_SoundObjectList::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SpawnPointList* obj = CF_SpawnPointList::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SystemError* obj = CF_SystemError::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_WorldMapList* obj = CF_WorldMapList::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_UITemplate* obj = CF_UITemplate::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_MapCategory* obj = CF_MapCategory::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_NPCFunctionType* obj = CF_NPCFunctionType::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SoundSequence* obj = CF_SoundSequence::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_TreasurePanel* obj = CF_TreasurePanel::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SacrificeReward* obj = CF_SacrificeReward::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_GuildClash* obj = CF_GuildClash::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SpecialEquipAvatar* obj = CF_SpecialEquipAvatar::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ResidentImage* obj = CF_ResidentImage::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ClientNPCIdleAction* obj = CF_ClientNPCIdleAction::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_FunctionPreview* obj = CF_FunctionPreview::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_WalkTransferMap* obj = CF_WalkTransferMap::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SmartHook* obj = CF_SmartHook::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SecretSkillClient* obj = CF_SecretSkillClient::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_UIFontColor* obj = CF_UIFontColor::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_UINameFilter* obj = CF_UINameFilter::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_UIAreaNameFilter* obj = CF_UIAreaNameFilter::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_OffLineExp* obj = CF_OffLineExp::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_OffLineExpRatio* obj = CF_OffLineExpRatio::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_MysticalShop* obj = CF_MysticalShop::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_MysticalShopReset* obj = CF_MysticalShopReset::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_MysticalShopVip* obj = CF_MysticalShopVip::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_MysticalShopRefresh* obj = CF_MysticalShopRefresh::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_CameraCut* obj = CF_CameraCut::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_GameNotice* obj = CF_GameNotice::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_GfxList* obj = CF_GfxList::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_CSVSchedule* obj = CF_CSVSchedule::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ChatCooldown* obj = CF_ChatCooldown::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_VIPPrivilegeCard* obj = CF_VIPPrivilegeCard::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_Fashion* obj = CF_Fashion::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_FashionSuit* obj = CF_FashionSuit::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_FashionBag* obj = CF_FashionBag::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SoulRefinePage* obj = CF_SoulRefinePage::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SoulRefineLevelUp* obj = CF_SoulRefineLevelUp::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SoulRefineMoster* obj = CF_SoulRefineMoster::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_MasterBliss* obj = CF_MasterBliss::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_MasterBlissScore* obj = CF_MasterBlissScore::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_MasterBlissBuff* obj = CF_MasterBlissBuff::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_MasterBlissPosition* obj = CF_MasterBlissPosition::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_YunYingPlatformConfig* obj = CF_YunYingPlatformConfig::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_YunYingOpenUrlConfig* obj = CF_YunYingOpenUrlConfig::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_TwToZn* obj = CF_TwToZn::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ItemMelt* obj = CF_ItemMelt::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_MeltLevelUp* obj = CF_MeltLevelUp::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_EffectTrigger* obj = CF_EffectTrigger::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ACTSubordinateConsume* obj = CF_ACTSubordinateConsume::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_Buddy* obj = CF_Buddy::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_BuddyUpgrade* obj = CF_BuddyUpgrade::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_CharSkillWASDAnim* obj = CF_CharSkillWASDAnim::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_EquipStar* obj = CF_EquipStar::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_EquipSlotStar* obj = CF_EquipSlotStar::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_BuddyUnlock* obj = CF_BuddyUnlock::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_BuddyLimit* obj = CF_BuddyLimit::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_GfxSmoothRelease* obj = CF_GfxSmoothRelease::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_DailyActivity* obj = CF_DailyActivity::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_AchievementRewards* obj = CF_AchievementRewards::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SignInDailyReward* obj = CF_SignInDailyReward::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_StarVeinList* obj = CF_StarVeinList::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_FirstDailyCharge* obj = CF_FirstDailyCharge::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SkillBookInitAttribute* obj = CF_SkillBookInitAttribute::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_BuddyGrowGift* obj = CF_BuddyGrowGift::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_Pet* obj = CF_Pet::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_PetType* obj = CF_PetType::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_PetQuality* obj = CF_PetQuality::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_PetUpgrade* obj = CF_PetUpgrade::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_CollectionActivation* obj = CF_CollectionActivation::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_CollectionReward* obj = CF_CollectionReward::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ElementDamage* obj = CF_ElementDamage::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_BossFirstKill* obj = CF_BossFirstKill::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SevenDaysReward* obj = CF_SevenDaysReward::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SkillBulletFuncRandom* obj = CF_SkillBulletFuncRandom::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SkillBookUnlock* obj = CF_SkillBookUnlock::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_EndlessTower* obj = CF_EndlessTower::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_TaskChapters* obj = CF_TaskChapters::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_TaskRankRewards* obj = CF_TaskRankRewards::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_Bot* obj = CF_Bot::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_BotUpgrade* obj = CF_BotUpgrade::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_EquipSlotReinforce* obj = CF_EquipSlotReinforce::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_RuneLevelUp* obj = CF_RuneLevelUp::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_RuneStar* obj = CF_RuneStar::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_Biography* obj = CF_Biography::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_BiographyTask* obj = CF_BiographyTask::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_RuneList* obj = CF_RuneList::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_TaskReachRewards* obj = CF_TaskReachRewards::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_TimeLimitedActivity* obj = CF_TimeLimitedActivity::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ACTShopTimeLimited* obj = CF_ACTShopTimeLimited::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_HolyDeed* obj = CF_HolyDeed::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_StarVeinGoalList* obj = CF_StarVeinGoalList::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SevenDaysTask* obj = CF_SevenDaysTask::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_RankRewardGrade* obj = CF_RankRewardGrade::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ShopGroupOpenSetting* obj = CF_ShopGroupOpenSetting::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_VIPProfitDescribe* obj = CF_VIPProfitDescribe::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_InvestmentPlan* obj = CF_InvestmentPlan::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_InvestmentCardTask* obj = CF_InvestmentCardTask::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_InvestmentCard* obj = CF_InvestmentCard::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_BossInvadeRankReward* obj = CF_BossInvadeRankReward::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_BossInvade* obj = CF_BossInvade::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_VipNew* obj = CF_VipNew::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_HookMaps* obj = CF_HookMaps::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_RecommendHero* obj = CF_RecommendHero::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_TestDatabase* obj = CF_TestDatabase::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_TestAccPointRewards* obj = CF_TestAccPointRewards::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_TestRankReward* obj = CF_TestRankReward::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_TestRankGuildReward* obj = CF_TestRankGuildReward::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_PromoteCombat* obj = CF_PromoteCombat::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ExportSoloDuleRobotInfo* obj = CF_ExportSoloDuleRobotInfo::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ExportRobotBuddyTemplateInfo* obj = CF_ExportRobotBuddyTemplateInfo::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ExportSoloDuleRobotProperty* obj = CF_ExportSoloDuleRobotProperty::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ShotActivityLimitRankRewards* obj = CF_ShotActivityLimitRankRewards::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_CommanderTitle* obj = CF_CommanderTitle::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SevenDaysTotalReward* obj = CF_SevenDaysTotalReward::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SpecialTraining* obj = CF_SpecialTraining::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SevenDaysLoginReward* obj = CF_SevenDaysLoginReward::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SoulConnect* obj = CF_SoulConnect::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_HeadFrame* obj = CF_HeadFrame::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_GloryExpedition* obj = CF_GloryExpedition::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_EquipUpgrade* obj = CF_EquipUpgrade::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_BuddyRuneRecommend* obj = CF_BuddyRuneRecommend::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_EquipCommanderJinglian* obj = CF_EquipCommanderJinglian::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_EquipCommanderSlotReinforce* obj = CF_EquipCommanderSlotReinforce::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_EquipCommanderJinglianSuit* obj = CF_EquipCommanderJinglianSuit::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_EquipCommanderSlotReinforceSuit* obj = CF_EquipCommanderSlotReinforceSuit::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_YiJiTuXiStageReward* obj = CF_YiJiTuXiStageReward::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_YiJiXunBaoStageReward* obj = CF_YiJiXunBaoStageReward::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_CommanderCombatPK* obj = CF_CommanderCombatPK::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_RotaryTable* obj = CF_RotaryTable::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_RotaryTableReward* obj = CF_RotaryTableReward::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_NonpareilItem* obj = CF_NonpareilItem::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_BestGuild* obj = CF_BestGuild::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_BestGuildTask* obj = CF_BestGuildTask::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_BestGuildGuildRankReward* obj = CF_BestGuildGuildRankReward::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_BestGuildPersonalRankReward* obj = CF_BestGuildPersonalRankReward::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_BestGuildBossRankRewards* obj = CF_BestGuildBossRankRewards::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_BestGuildBossInfo* obj = CF_BestGuildBossInfo::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_BestGuildRobReward* obj = CF_BestGuildRobReward::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_BestGuildDailyTask* obj = CF_BestGuildDailyTask::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_RegularBoss* obj = CF_RegularBoss::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_EquipCommanderSlotMoldingSpirit* obj = CF_EquipCommanderSlotMoldingSpirit::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_EquipCommanderSlotMoldingSpiritSuit* obj = CF_EquipCommanderSlotMoldingSpiritSuit::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_PetIslandStageReward* obj = CF_PetIslandStageReward::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_EscortReward* obj = CF_EscortReward::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_BestGuildGoal* obj = CF_BestGuildGoal::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_RankOpen* obj = CF_RankOpen::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_GloryExpeditionDailyRewards* obj = CF_GloryExpeditionDailyRewards::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_BestGuildChapterID* obj = CF_BestGuildChapterID::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_PromoteLevel* obj = CF_PromoteLevel::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_GloryRewardLink* obj = CF_GloryRewardLink::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ComeBack* obj = CF_ComeBack::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_EquipCommanderSuitCollection* obj = CF_EquipCommanderSuitCollection::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_NewAchievementReward* obj = CF_NewAchievementReward::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_TreasureAddAward* obj = CF_TreasureAddAward::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_BuddyTransform* obj = CF_BuddyTransform::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_YiJiTuXiRankRewards* obj = CF_YiJiTuXiRankRewards::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_GuildCrystal* obj = CF_GuildCrystal::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_GuildGift* obj = CF_GuildGift::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_TreasurePos* obj = CF_TreasurePos::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_AchievementTypeBig* obj = CF_AchievementTypeBig::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_AchievementTypeSmall* obj = CF_AchievementTypeSmall::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_TreasureEvent* obj = CF_TreasureEvent::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_TreasureItem* obj = CF_TreasureItem::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_EntrustTask* obj = CF_EntrustTask::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SpecialBuy* obj = CF_SpecialBuy::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_GuildTower* obj = CF_GuildTower::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_GuildTowerRankReward* obj = CF_GuildTowerRankReward::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_BuddyThemeMain* obj = CF_BuddyThemeMain::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_BuddyThemeSecondary* obj = CF_BuddyThemeSecondary::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_GameRestartNotice* obj = CF_GameRestartNotice::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_BuddyThemeTaskType* obj = CF_BuddyThemeTaskType::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_BuddyThemeBossRankReward* obj = CF_BuddyThemeBossRankReward::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_BuddyThemeBossType* obj = CF_BuddyThemeBossType::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_BuddyThemeTaskList* obj = CF_BuddyThemeTaskList::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ACTTreasureHunt* obj = CF_ACTTreasureHunt::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ACTTreasureHuntStageReward* obj = CF_ACTTreasureHuntStageReward::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_BossCamera* obj = CF_BossCamera::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_BossCameraDetail* obj = CF_BossCameraDetail::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_HookPos* obj = CF_HookPos::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_Charge* obj = CF_Charge::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_MailInfo* obj = CF_MailInfo::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_TutorialRewards* obj = CF_TutorialRewards::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SubSystemInfo* obj = CF_SubSystemInfo::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SkyArenaRank* obj = CF_SkyArenaRank::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SkyArenaRankReward* obj = CF_SkyArenaRankReward::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SpecialTask* obj = CF_SpecialTask::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_Guess* obj = CF_Guess::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SummitArenaMail* obj = CF_SummitArenaMail::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SummitArenaMatchName* obj = CF_SummitArenaMatchName::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_SummitArenaRankRewards* obj = CF_SummitArenaRankRewards::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_Scheme* obj = CF_Scheme::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_YiZhongJiangLinRankReward* obj = CF_YiZhongJiangLinRankReward::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_YiZhongJiangLinBossInformation* obj = CF_YiZhongJiangLinBossInformation::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_HolidayActivityMain* obj = CF_HolidayActivityMain::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_HolidayActivitySecondary* obj = CF_HolidayActivitySecondary::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ActivityRankReward* obj = CF_ActivityRankReward::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_HolidayActivity_ChargeContinuous* obj = CF_HolidayActivity_ChargeContinuous::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_HolidayActivity_PinTuUnlock* obj = CF_HolidayActivity_PinTuUnlock::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_HolidayActivity_PinTuReward* obj = CF_HolidayActivity_PinTuReward::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_HolidayActivityTaskType* obj = CF_HolidayActivityTaskType::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_HolidayActivityTaskList* obj = CF_HolidayActivityTaskList::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_RankCondition* obj = CF_RankCondition::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ACTShopB* obj = CF_ACTShopB::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
	{
		CF_ActivityTime* obj = CF_ActivityTime::GetSingletonPtr();
		T_SAFE_DELETE(obj);
	}
}

bool Reload_Client_CSV_File(CSVFile::CSVDataLoader* pkReloadOP)
{
	bool bReload = false;
	{
		CF_ActorFilterFunc* obj = CF_ActorFilterFunc::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_AreaBarrier* obj = CF_AreaBarrier::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_AreaEffect* obj = CF_AreaEffect::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_Subordinate* obj = CF_Subordinate::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SubordinateCluster* obj = CF_SubordinateCluster::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SubordinateTransform* obj = CF_SubordinateTransform::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SubRelateAttri* obj = CF_SubRelateAttri::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_AuctionCategory* obj = CF_AuctionCategory::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_AutoFitCharType* obj = CF_AutoFitCharType::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_CharType* obj = CF_CharType::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_CreateCharParam* obj = CF_CreateCharParam::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_Conversation* obj = CF_Conversation::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_CutScenes* obj = CF_CutScenes::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_CrossRealmMap* obj = CF_CrossRealmMap::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_EffectList* obj = CF_EffectList::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_EXPList* obj = CF_EXPList::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_Faction* obj = CF_Faction::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ForbiddenWordsList* obj = CF_ForbiddenWordsList::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_FriendMsgBroadContent* obj = CF_FriendMsgBroadContent::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_GameStory* obj = CF_GameStory::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_GameStoryClientUI* obj = CF_GameStoryClientUI::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_GameStoryRaidList* obj = CF_GameStoryRaidList::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_GameStoryMessage* obj = CF_GameStoryMessage::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_GuildLevel* obj = CF_GuildLevel::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_GuildCreateLimit* obj = CF_GuildCreateLimit::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_GuildWarBuff* obj = CF_GuildWarBuff::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_GuildGood* obj = CF_GuildGood::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_GlobalPowerGuildReward* obj = CF_GlobalPowerGuildReward::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_GemExp* obj = CF_GemExp::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_GemReinforceLevel* obj = CF_GemReinforceLevel::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_HittedSoundList* obj = CF_HittedSoundList::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ItemAssembleUseFunc* obj = CF_ItemAssembleUseFunc::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ItemAvartarFunc* obj = CF_ItemAvartarFunc::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ItemCategory* obj = CF_ItemCategory::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ItemDecompositionFunc* obj = CF_ItemDecompositionFunc::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ItemDisassembleUseFunc* obj = CF_ItemDisassembleUseFunc::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ItemEffectFunc* obj = CF_ItemEffectFunc::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ItemEffect* obj = CF_ItemEffect::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ItemEnduranceFunc* obj = CF_ItemEnduranceFunc::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ItemEquipLimitFunc* obj = CF_ItemEquipLimitFunc::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ItemExistenceFunc* obj = CF_ItemExistenceFunc::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ItemGenEffect* obj = CF_ItemGenEffect::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ItemGen* obj = CF_ItemGen::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ItemAttrActive* obj = CF_ItemAttrActive::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ItemList* obj = CF_ItemList::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ItemOperationFunc* obj = CF_ItemOperationFunc::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ItemPropertyFunc* obj = CF_ItemPropertyFunc::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ItemSuit* obj = CF_ItemSuit::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ItemUIAssembleUseFunc* obj = CF_ItemUIAssembleUseFunc::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ItemUseLimitFunc* obj = CF_ItemUseLimitFunc::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ItemReportFunc* obj = CF_ItemReportFunc::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_LevelUp* obj = CF_LevelUp::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_LoadingTip* obj = CF_LoadingTip::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_LogicAreaChunkIDList* obj = CF_LogicAreaChunkIDList::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_LogicAreaFunc* obj = CF_LogicAreaFunc::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_LogicTriggerList* obj = CF_LogicTriggerList::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_MailCharge* obj = CF_MailCharge::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_MainTaskList* obj = CF_MainTaskList::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_MillionWeapons* obj = CF_MillionWeapons::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_MonsterAIFunc* obj = CF_MonsterAIFunc::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_NPCAvatar* obj = CF_NPCAvatar::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_NPCFunction* obj = CF_NPCFunction::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_NPCInteractionConfig* obj = CF_NPCInteractionConfig::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_NPCShopList* obj = CF_NPCShopList::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_NPCSoundList* obj = CF_NPCSoundList::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_NPCTransferList* obj = CF_NPCTransferList::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_OpenBag* obj = CF_OpenBag::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_OpenLevel* obj = CF_OpenLevel::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ProductionFormula* obj = CF_ProductionFormula::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_CompositionFormula* obj = CF_CompositionFormula::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_RankReward* obj = CF_RankReward::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ShopList* obj = CF_ShopList::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SignInReward* obj = CF_SignInReward::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SocketItemFunc* obj = CF_SocketItemFunc::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SkillBookLevelUp* obj = CF_SkillBookLevelUp::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SkillBulletFunc* obj = CF_SkillBulletFunc::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SkillCDModifyFunc* obj = CF_SkillCDModifyFunc::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SkillExecuteList* obj = CF_SkillExecuteList::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SkillSpecialAttrib* obj = CF_SkillSpecialAttrib::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SkillLevelUp* obj = CF_SkillLevelUp::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SkillStageFunc* obj = CF_SkillStageFunc::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SkillStateList* obj = CF_SkillStateList::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SPLevelup* obj = CF_SPLevelup::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SystemTitleList* obj = CF_SystemTitleList::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_TaskCondition* obj = CF_TaskCondition::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_TaskList* obj = CF_TaskList::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_TextColor* obj = CF_TextColor::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_TextConfig* obj = CF_TextConfig::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_TranscriptionRule* obj = CF_TranscriptionRule::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_TransferArea* obj = CF_TransferArea::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_Tutorial* obj = CF_Tutorial::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_TranscriptionSettleGrade* obj = CF_TranscriptionSettleGrade::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_UIControlRelation* obj = CF_UIControlRelation::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_UISound* obj = CF_UISound::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_Vigor* obj = CF_Vigor::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_WarehouseCost* obj = CF_WarehouseCost::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_GodSuiteTask* obj = CF_GodSuiteTask::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_QQGirls* obj = CF_QQGirls::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_WorldObjectDesc* obj = CF_WorldObjectDesc::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_VIP* obj = CF_VIP::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_VIPProfit* obj = CF_VIPProfit::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_DailyLiveness* obj = CF_DailyLiveness::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SD_Call* obj = CF_SD_Call::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_TaskParam* obj = CF_TaskParam::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_TaskAssign* obj = CF_TaskAssign::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_CharPromote* obj = CF_CharPromote::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_VIPFDeposit* obj = CF_VIPFDeposit::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_CharAttributeFunc* obj = CF_CharAttributeFunc::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_GuildWarCatapult* obj = CF_GuildWarCatapult::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_GuildWarCrontab* obj = CF_GuildWarCrontab::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_GuildWarActivity* obj = CF_GuildWarActivity::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_TreasureChests* obj = CF_TreasureChests::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_TreasurePool* obj = CF_TreasurePool::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ForcesBattle* obj = CF_ForcesBattle::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_PowerUp* obj = CF_PowerUp::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_PowerUpFunc* obj = CF_PowerUpFunc::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_CommonStatReward* obj = CF_CommonStatReward::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_CommonStatReplevy* obj = CF_CommonStatReplevy::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_CommonStatConfig* obj = CF_CommonStatConfig::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_CommonStatCompensate* obj = CF_CommonStatCompensate::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_CommonStatBasicReward* obj = CF_CommonStatBasicReward::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ActivitySchedule* obj = CF_ActivitySchedule::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SecretTreasureSchedule* obj = CF_SecretTreasureSchedule::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SecretTreasureShop* obj = CF_SecretTreasureShop::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SecretTreasureChests* obj = CF_SecretTreasureChests::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SecretTreasureReward* obj = CF_SecretTreasureReward::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SecretTreasureTimes* obj = CF_SecretTreasureTimes::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ACTSubordinateReward* obj = CF_ACTSubordinateReward::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ACTSubordinateRewardA* obj = CF_ACTSubordinateRewardA::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ACTSubordinateRewardB* obj = CF_ACTSubordinateRewardB::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ACTChargeProfit* obj = CF_ACTChargeProfit::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ACTChargeProfitA* obj = CF_ACTChargeProfitA::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ACTChargeProfitB* obj = CF_ACTChargeProfitB::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ACTChargeProfitBReward* obj = CF_ACTChargeProfitBReward::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ACTChargeProfitType* obj = CF_ACTChargeProfitType::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ACTShop* obj = CF_ACTShop::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ACTShopA* obj = CF_ACTShopA::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ACTShot* obj = CF_ACTShot::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ACTShotA* obj = CF_ACTShotA::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ACTLottery* obj = CF_ACTLottery::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ACTLotteryReward* obj = CF_ACTLotteryReward::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ArenaSetting* obj = CF_ArenaSetting::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ArenaSection* obj = CF_ArenaSection::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ArenaTimes* obj = CF_ArenaTimes::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_YaZhiLevel* obj = CF_YaZhiLevel::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_GuildRaid* obj = CF_GuildRaid::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_GuildDonate* obj = CF_GuildDonate::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_WildPrinces* obj = CF_WildPrinces::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_TaskType* obj = CF_TaskType::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_Mental* obj = CF_Mental::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_UIOpenLevel* obj = CF_UIOpenLevel::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_CharTypeFilterMiniMap* obj = CF_CharTypeFilterMiniMap::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_AchievementList* obj = CF_AchievementList::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_StoneList* obj = CF_StoneList::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_BossShowPosition* obj = CF_BossShowPosition::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_MailItemList* obj = CF_MailItemList::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ShotActivity* obj = CF_ShotActivity::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ItemOperation* obj = CF_ItemOperation::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_NPCShopItems* obj = CF_NPCShopItems::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_FlowerConfig* obj = CF_FlowerConfig::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_FlowerTitleCfg* obj = CF_FlowerTitleCfg::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_FlowerGiftCfg* obj = CF_FlowerGiftCfg::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_FlowerSendLimitCfg* obj = CF_FlowerSendLimitCfg::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_LocalSpawnPoint* obj = CF_LocalSpawnPoint::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_LogicAreaSound* obj = CF_LogicAreaSound::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_MapEffectConfig* obj = CF_MapEffectConfig::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SoundObjectList* obj = CF_SoundObjectList::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SpawnPointList* obj = CF_SpawnPointList::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SystemError* obj = CF_SystemError::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_WorldMapList* obj = CF_WorldMapList::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_UITemplate* obj = CF_UITemplate::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_MapCategory* obj = CF_MapCategory::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_NPCFunctionType* obj = CF_NPCFunctionType::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SoundSequence* obj = CF_SoundSequence::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_TreasurePanel* obj = CF_TreasurePanel::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SacrificeReward* obj = CF_SacrificeReward::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_GuildClash* obj = CF_GuildClash::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SpecialEquipAvatar* obj = CF_SpecialEquipAvatar::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ResidentImage* obj = CF_ResidentImage::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ClientNPCIdleAction* obj = CF_ClientNPCIdleAction::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_FunctionPreview* obj = CF_FunctionPreview::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_WalkTransferMap* obj = CF_WalkTransferMap::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SmartHook* obj = CF_SmartHook::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SecretSkillClient* obj = CF_SecretSkillClient::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_UIFontColor* obj = CF_UIFontColor::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_UINameFilter* obj = CF_UINameFilter::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_UIAreaNameFilter* obj = CF_UIAreaNameFilter::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_OffLineExp* obj = CF_OffLineExp::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_OffLineExpRatio* obj = CF_OffLineExpRatio::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_MysticalShop* obj = CF_MysticalShop::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_MysticalShopReset* obj = CF_MysticalShopReset::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_MysticalShopVip* obj = CF_MysticalShopVip::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_MysticalShopRefresh* obj = CF_MysticalShopRefresh::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_CameraCut* obj = CF_CameraCut::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_GameNotice* obj = CF_GameNotice::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_GfxList* obj = CF_GfxList::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_CSVSchedule* obj = CF_CSVSchedule::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ChatCooldown* obj = CF_ChatCooldown::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_VIPPrivilegeCard* obj = CF_VIPPrivilegeCard::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_Fashion* obj = CF_Fashion::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_FashionSuit* obj = CF_FashionSuit::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_FashionBag* obj = CF_FashionBag::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SoulRefinePage* obj = CF_SoulRefinePage::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SoulRefineLevelUp* obj = CF_SoulRefineLevelUp::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SoulRefineMoster* obj = CF_SoulRefineMoster::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_MasterBliss* obj = CF_MasterBliss::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_MasterBlissScore* obj = CF_MasterBlissScore::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_MasterBlissBuff* obj = CF_MasterBlissBuff::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_MasterBlissPosition* obj = CF_MasterBlissPosition::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_YunYingPlatformConfig* obj = CF_YunYingPlatformConfig::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_YunYingOpenUrlConfig* obj = CF_YunYingOpenUrlConfig::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_TwToZn* obj = CF_TwToZn::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ItemMelt* obj = CF_ItemMelt::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_MeltLevelUp* obj = CF_MeltLevelUp::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_EffectTrigger* obj = CF_EffectTrigger::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ACTSubordinateConsume* obj = CF_ACTSubordinateConsume::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_Buddy* obj = CF_Buddy::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_BuddyUpgrade* obj = CF_BuddyUpgrade::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_CharSkillWASDAnim* obj = CF_CharSkillWASDAnim::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_EquipStar* obj = CF_EquipStar::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_EquipSlotStar* obj = CF_EquipSlotStar::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_BuddyUnlock* obj = CF_BuddyUnlock::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_BuddyLimit* obj = CF_BuddyLimit::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_GfxSmoothRelease* obj = CF_GfxSmoothRelease::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_DailyActivity* obj = CF_DailyActivity::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_AchievementRewards* obj = CF_AchievementRewards::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SignInDailyReward* obj = CF_SignInDailyReward::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_StarVeinList* obj = CF_StarVeinList::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_FirstDailyCharge* obj = CF_FirstDailyCharge::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SkillBookInitAttribute* obj = CF_SkillBookInitAttribute::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_BuddyGrowGift* obj = CF_BuddyGrowGift::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_Pet* obj = CF_Pet::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_PetType* obj = CF_PetType::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_PetQuality* obj = CF_PetQuality::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_PetUpgrade* obj = CF_PetUpgrade::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_CollectionActivation* obj = CF_CollectionActivation::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_CollectionReward* obj = CF_CollectionReward::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ElementDamage* obj = CF_ElementDamage::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_BossFirstKill* obj = CF_BossFirstKill::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SevenDaysReward* obj = CF_SevenDaysReward::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SkillBulletFuncRandom* obj = CF_SkillBulletFuncRandom::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SkillBookUnlock* obj = CF_SkillBookUnlock::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_EndlessTower* obj = CF_EndlessTower::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_TaskChapters* obj = CF_TaskChapters::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_TaskRankRewards* obj = CF_TaskRankRewards::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_Bot* obj = CF_Bot::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_BotUpgrade* obj = CF_BotUpgrade::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_EquipSlotReinforce* obj = CF_EquipSlotReinforce::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_RuneLevelUp* obj = CF_RuneLevelUp::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_RuneStar* obj = CF_RuneStar::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_Biography* obj = CF_Biography::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_BiographyTask* obj = CF_BiographyTask::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_RuneList* obj = CF_RuneList::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_TaskReachRewards* obj = CF_TaskReachRewards::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_TimeLimitedActivity* obj = CF_TimeLimitedActivity::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ACTShopTimeLimited* obj = CF_ACTShopTimeLimited::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_HolyDeed* obj = CF_HolyDeed::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_StarVeinGoalList* obj = CF_StarVeinGoalList::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SevenDaysTask* obj = CF_SevenDaysTask::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_RankRewardGrade* obj = CF_RankRewardGrade::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ShopGroupOpenSetting* obj = CF_ShopGroupOpenSetting::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_VIPProfitDescribe* obj = CF_VIPProfitDescribe::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_InvestmentPlan* obj = CF_InvestmentPlan::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_InvestmentCardTask* obj = CF_InvestmentCardTask::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_InvestmentCard* obj = CF_InvestmentCard::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_BossInvadeRankReward* obj = CF_BossInvadeRankReward::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_BossInvade* obj = CF_BossInvade::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_VipNew* obj = CF_VipNew::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_HookMaps* obj = CF_HookMaps::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_RecommendHero* obj = CF_RecommendHero::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_TestDatabase* obj = CF_TestDatabase::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_TestAccPointRewards* obj = CF_TestAccPointRewards::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_TestRankReward* obj = CF_TestRankReward::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_TestRankGuildReward* obj = CF_TestRankGuildReward::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_PromoteCombat* obj = CF_PromoteCombat::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ExportSoloDuleRobotInfo* obj = CF_ExportSoloDuleRobotInfo::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ExportRobotBuddyTemplateInfo* obj = CF_ExportRobotBuddyTemplateInfo::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ExportSoloDuleRobotProperty* obj = CF_ExportSoloDuleRobotProperty::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ShotActivityLimitRankRewards* obj = CF_ShotActivityLimitRankRewards::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_CommanderTitle* obj = CF_CommanderTitle::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SevenDaysTotalReward* obj = CF_SevenDaysTotalReward::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SpecialTraining* obj = CF_SpecialTraining::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SevenDaysLoginReward* obj = CF_SevenDaysLoginReward::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SoulConnect* obj = CF_SoulConnect::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_HeadFrame* obj = CF_HeadFrame::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_GloryExpedition* obj = CF_GloryExpedition::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_EquipUpgrade* obj = CF_EquipUpgrade::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_BuddyRuneRecommend* obj = CF_BuddyRuneRecommend::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_EquipCommanderJinglian* obj = CF_EquipCommanderJinglian::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_EquipCommanderSlotReinforce* obj = CF_EquipCommanderSlotReinforce::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_EquipCommanderJinglianSuit* obj = CF_EquipCommanderJinglianSuit::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_EquipCommanderSlotReinforceSuit* obj = CF_EquipCommanderSlotReinforceSuit::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_YiJiTuXiStageReward* obj = CF_YiJiTuXiStageReward::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_YiJiXunBaoStageReward* obj = CF_YiJiXunBaoStageReward::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_CommanderCombatPK* obj = CF_CommanderCombatPK::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_RotaryTable* obj = CF_RotaryTable::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_RotaryTableReward* obj = CF_RotaryTableReward::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_NonpareilItem* obj = CF_NonpareilItem::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_BestGuild* obj = CF_BestGuild::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_BestGuildTask* obj = CF_BestGuildTask::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_BestGuildGuildRankReward* obj = CF_BestGuildGuildRankReward::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_BestGuildPersonalRankReward* obj = CF_BestGuildPersonalRankReward::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_BestGuildBossRankRewards* obj = CF_BestGuildBossRankRewards::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_BestGuildBossInfo* obj = CF_BestGuildBossInfo::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_BestGuildRobReward* obj = CF_BestGuildRobReward::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_BestGuildDailyTask* obj = CF_BestGuildDailyTask::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_RegularBoss* obj = CF_RegularBoss::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_EquipCommanderSlotMoldingSpirit* obj = CF_EquipCommanderSlotMoldingSpirit::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_EquipCommanderSlotMoldingSpiritSuit* obj = CF_EquipCommanderSlotMoldingSpiritSuit::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_PetIslandStageReward* obj = CF_PetIslandStageReward::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_EscortReward* obj = CF_EscortReward::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_BestGuildGoal* obj = CF_BestGuildGoal::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_RankOpen* obj = CF_RankOpen::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_GloryExpeditionDailyRewards* obj = CF_GloryExpeditionDailyRewards::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_BestGuildChapterID* obj = CF_BestGuildChapterID::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_PromoteLevel* obj = CF_PromoteLevel::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_GloryRewardLink* obj = CF_GloryRewardLink::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ComeBack* obj = CF_ComeBack::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_EquipCommanderSuitCollection* obj = CF_EquipCommanderSuitCollection::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_NewAchievementReward* obj = CF_NewAchievementReward::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_TreasureAddAward* obj = CF_TreasureAddAward::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_BuddyTransform* obj = CF_BuddyTransform::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_YiJiTuXiRankRewards* obj = CF_YiJiTuXiRankRewards::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_GuildCrystal* obj = CF_GuildCrystal::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_GuildGift* obj = CF_GuildGift::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_TreasurePos* obj = CF_TreasurePos::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_AchievementTypeBig* obj = CF_AchievementTypeBig::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_AchievementTypeSmall* obj = CF_AchievementTypeSmall::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_TreasureEvent* obj = CF_TreasureEvent::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_TreasureItem* obj = CF_TreasureItem::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_EntrustTask* obj = CF_EntrustTask::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SpecialBuy* obj = CF_SpecialBuy::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_GuildTower* obj = CF_GuildTower::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_GuildTowerRankReward* obj = CF_GuildTowerRankReward::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_BuddyThemeMain* obj = CF_BuddyThemeMain::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_BuddyThemeSecondary* obj = CF_BuddyThemeSecondary::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_GameRestartNotice* obj = CF_GameRestartNotice::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_BuddyThemeTaskType* obj = CF_BuddyThemeTaskType::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_BuddyThemeBossRankReward* obj = CF_BuddyThemeBossRankReward::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_BuddyThemeBossType* obj = CF_BuddyThemeBossType::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_BuddyThemeTaskList* obj = CF_BuddyThemeTaskList::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ACTTreasureHunt* obj = CF_ACTTreasureHunt::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ACTTreasureHuntStageReward* obj = CF_ACTTreasureHuntStageReward::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_BossCamera* obj = CF_BossCamera::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_BossCameraDetail* obj = CF_BossCameraDetail::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_HookPos* obj = CF_HookPos::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_Charge* obj = CF_Charge::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_MailInfo* obj = CF_MailInfo::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_TutorialRewards* obj = CF_TutorialRewards::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SubSystemInfo* obj = CF_SubSystemInfo::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SkyArenaRank* obj = CF_SkyArenaRank::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SkyArenaRankReward* obj = CF_SkyArenaRankReward::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SpecialTask* obj = CF_SpecialTask::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_Guess* obj = CF_Guess::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SummitArenaMail* obj = CF_SummitArenaMail::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SummitArenaMatchName* obj = CF_SummitArenaMatchName::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_SummitArenaRankRewards* obj = CF_SummitArenaRankRewards::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_Scheme* obj = CF_Scheme::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_YiZhongJiangLinRankReward* obj = CF_YiZhongJiangLinRankReward::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_YiZhongJiangLinBossInformation* obj = CF_YiZhongJiangLinBossInformation::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_HolidayActivityMain* obj = CF_HolidayActivityMain::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_HolidayActivitySecondary* obj = CF_HolidayActivitySecondary::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ActivityRankReward* obj = CF_ActivityRankReward::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_HolidayActivity_ChargeContinuous* obj = CF_HolidayActivity_ChargeContinuous::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_HolidayActivity_PinTuUnlock* obj = CF_HolidayActivity_PinTuUnlock::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_HolidayActivity_PinTuReward* obj = CF_HolidayActivity_PinTuReward::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_HolidayActivityTaskType* obj = CF_HolidayActivityTaskType::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_HolidayActivityTaskList* obj = CF_HolidayActivityTaskList::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_RankCondition* obj = CF_RankCondition::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ACTShopB* obj = CF_ACTShopB::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	{
		CF_ActivityTime* obj = CF_ActivityTime::GetSingletonPtr();
		if (obj != NULL)
		{
			if(pkReloadOP)
			{
				bReload = (*pkReloadOP)(obj) || bReload;
			}
			else
			{
				bReload = obj->Load() || bReload;
			}
		}
	}
	return bReload;
}
