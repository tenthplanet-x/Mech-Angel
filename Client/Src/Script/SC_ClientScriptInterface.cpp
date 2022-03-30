#include "ClientPCH.h"
#include "ClientScriptPCH.h"
#include "SC_ClientScriptInterface.h"
//
#include "SK_SkillExecute.h"
#include "TT_TextManager.h"
#include "SE_SceneManager.h"
#include "LC_TooltipManager.h"
#include "LC_MapManager.h"
#include "LC_MiniMap.h"
#include "LC_RadarMap.h"
#include "LC_TownMap.h"
#include "LC_Task.h"
#include "LC_TaskManager.h"
#include "LC_PlayerGroupBase.h"
#include "LC_PKRecorderBase.h"
#include "LC_ClientShopCity.h"
#include "LC_ItemFactoryBase.h"
#include "LC_PackAsset.h"
#include "LC_PlayerFlopManager.h"
#include "UT_GlobalSettings.h"
#include "GlobalSettings.h"
#include "LC_ClientTask.h"
#include "LC_PlayerHatred.h"
#include "LC_ClientTaskMap.h"
#include "LC_ClientDialogManager.h"
#include "LC_ClientDialogInfo.h"
#include "LC_ClientMailManager.h"
#include "LC_ClientSocietyAsset.h"
#include "LC_ClientViewPlayerInfo.h"
#include "LC_ClientFriendMsgBroad.h"
#include "LC_ClientFriendNick.h"
#include "LC_ClientAuctionManager.h"
#include "LC_ClientTranscriptionInfoManager.h"
#include "LC_ClientGameStoryManager.h"
#include "LC_ClientArenaManager.h"
#include "LC_TitleAsset.h"
//#include "LC_ClientExchangeManager.h"
#include "LC_ClientCurrencyTradeManager.h"
#include "LC_ClientMapTransDataEntry.h"
//#include "LC_SafeSystemAsset.h"
#include "LC_ClientGameEvent.h"
#include "LC_ClientArenaBattleChallengeInfoAsset.h"
#include "LC_ClientOpenAbilityManager.h"
#include "LC_ClientShotActivity.h"
#include "LC_ClientRumorManager.h"
#include "LC_ClientWorldLevelAsset.h"
#include "LC_ClientSoulRefineAsset.h"
#include "LC_ClientActivityManager.h"
#include "LC_AchievementManager.h"
#include "LC_ClientToyPetManager.h"
#include "LC_ClientToyPetInstance.h"
#include "LC_ClientCollectionManager.h"
#include "LC_ClientBotManager.h"
#include "LC_ClientCombatRecordManager.h"
#include "LC_ClientGuildProfitManager.h"
#include "LC_ClientArenaPlayBackManager.h"
#include "LC_ClientTaskManager.h"
#include "LC_ClientReportManager.h"

using namespace Base;
using namespace Script;
using namespace GameLogic;
using namespace Utility;
using namespace CSVFile;
using namespace Skill;
using namespace Protocol;
using namespace Path;

#include "SC_ClientScriptInterface.inl"
////---------------------------------------------------
bool SC_ClientScriptInterface::Init()
{
	SC_GameLogicScriptInterface::Init();

	m_pkClientSkillInterface = T_NEW_D SC_ClientSkillInterface;
	m_pkClientDebugInterface = T_NEW_D SC_ClientDebugInterface;

	LS_LuaState* pkState = LS_LuaStateFactory::GetSingleton().GetLogicLuaState();
	UT_DirConfig* pkDirConfig = UT_DirConfig::GetSingletonPtr();
	if(!pkDirConfig)
		return false;

	StringType kScriptClientPCHFileName = pkDirConfig->GetDir(CF_NODE_SCRIPT_CLIENT_PCH);
	pkState->DoFile(kScriptClientPCHFileName);
	return true;
}
////---------------------------------------------------
bool SC_ClientScriptInterface::UnInit()
{
	T_SAFE_DELETE(m_pkClientSkillInterface);
	T_SAFE_DELETE(m_pkClientDebugInterface);
	return true;
}
////---------------------------------------------------
void SC_ClientScriptInterface::RegisterToLua()
{
	SC_GameLogicScriptInterface::RegisterToLua();
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( UT_Vec2Int);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( UT_Vec3Int );
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( UT_Vec4Int );
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_LogicObject );
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_Attacker );
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_ActorBase );
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_PlayerBase );
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_NPCBase );
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( SK_SkillEvent );
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_ClientGamePlayer);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_ClientNetPlayer);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_ClientNPC);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_SkillInfoEntry);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_ActorAttributeMap );
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_ActorAttributeAppendMap );
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_BackPackEntry );
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_ItemFactoryBase );
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_ItemOperationDataEntry );
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_ItemDescriptionEntry );
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_ItemAppendAttributeFormulaEntry );
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_ItemUIContentEntry );
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_PackAsset);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_NPCShopItemEntry);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_ShopBase);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_TransferBase);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_SkillCoolDownMap);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_SkillAssetEntry);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_SkillDescriptionEntry);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_SkillAsset);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_ActorStateEntry);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_StateDescriptionEntry);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_ActorStateMap);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_ShortcutEntry );
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_ShortcutBar );
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( TT_TextElement);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( TT_TextManager );	
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_MapManager);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_MiniMap);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_RadarMap );
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_TownMap );
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_TooltipManager);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_ObjectDescription);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_RadarObjectDescription);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_TownIconDescription);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_PathDesc);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_ItemCoolDownMapBase);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_TaskManager );
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_TaskMap);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_ClientTaskMap);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_CanStartTaskMapEntry);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_ProcessingTaskEntry);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_FinishedTaskRecord );
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_TaskScriptImportParam );
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_Task_Condition );
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_Task );
	//REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_NPCTask );
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_ClientTask );
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_MainTaskDescriptEntry );
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_MainTask );
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_DailyLivenessEntry );
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_PlayerGroupEntry );	
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_PlayerGroupBase );
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_PKRecorderBase );
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_ClientShopCity);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_ShopCityGoodsDescriptionEntry);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_TradeManager);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_PlayerFlopManager);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_ProductionAssetEntry );
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_ClientProduction );
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_TutorialEntry);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_TutorialAsset);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_ShortcutKeyAsset);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_ShortcutKeyAssetEntry);

	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( SC_ClientScriptInterface );
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( SC_ClientSkillInterface );
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( SC_ClientDebugInterface );
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( PF_Path );
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(UT_GlobalSettings);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(GlobalSettings);

	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_DialogNodeData);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_DialogSet);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientDialogInfo);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientDialogManager);

	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientMailManager);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_MailAsset);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_MailInfo);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientItemAffixInfo);

	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientAuctionManager);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientAddItemInfo);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientAuctionBuyData);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientAuctionSellData);

	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientGuildManager);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_GuildAsset);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_GuildInfo);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientGuildMilestoneStruct);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientGuildSkillInfoStruct);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientGuildSingleSkill);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_GuildMemberEntry);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_GuildMemberInfo);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_GuildApplyEntry);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_GuildApplyInfo);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_GuildSearchInfo);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientMapTransDataEntry);


	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientSocietyAsset);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_SocietyBasicInfo);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_FriendDetailInfo);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientViewPlayerInfo);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientFriendMsgBroad);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientFriendMsgBroadStruct);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientFriendNick);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_FriendNick);

	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_GameEventBase);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_GameEvent);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientTranscriptionInfoManager);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientGameStoryManager);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientOpenAbilityManager);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(GameStoryInfo);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(GameStoryRaidInfo);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientArenaPlayerDataEntry);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientArenaManager);

	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_ArenaEntry );
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_ArenaAsset );
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_ActorOperationEntry );

	//REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_ExchangeManager );
	//REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_ClientExchangeManager );
	//REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( ExchangeData );

	//REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( NPCExchangeData );

	//REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientCurrencyTradeManager);	
	//REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_CurrencyTradeQuickTradeItem);
	//REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_CurrencyTradeCommssionItem);
	//REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_CurrencyTradeRecordItem);

	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( GuildWareHousePack );
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC( LC_ClientGuildHomeMgr );
	
	//REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_GiftAsset);
	//REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_GiftAssetEntry);

	//REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(UT_UIHelpSystem);
	//REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_SafeSystemAsset);

	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(UI_UIManager);

	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientGameEvent);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientGameEventUserDataBase);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientGameEventUserData_RaidSettle);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientGameEventUserData_String);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientGameEventUserData_ArenaBattle_BattleInfo);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientGameEventUserData_ArenaSettleInfo);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientGameEventUserData_Common);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientArenaBattleChallengeInfoAsset);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientRankUserInfo);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientBattleRecordDataBase);

	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_VIPAsset);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_SignInAssert);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_RaidAssert);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_TaskParams);//2014_4_3
    REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_TaskAssignEntry);//2014_4_3
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_AssignTaskData);//2014_4_3
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_AssignTaskInfo);//2014_4_9
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_GreatKungFuAsset);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_CrossRealmTeamAsset);	
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_VIPFDepositEntry);

	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_CommonStatAsset);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_CommonStatRewardDataEntry);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_CommonStatEntry);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_TreasureChestsAsset);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(ACT_SecretTreasureAsset);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_SkillBookAsset);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_CommonStatManager);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_OpenAbilityAsset);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_SubordinateAsset);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientRumorManager);

	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientWorldLevelAsset);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientAlliance);

	//REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientSoulRefineManager);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientSoulRefineAsset);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientShotActivityManager);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_FashionBagAsset);

	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientActivityManager);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientActivityAsset);

	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_MeltAsset);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_BoneSoulAsset);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_StarsRuneAsset);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_AchievementManager);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientToyPetManager);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientToyPetInstance);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientCollectionManager);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientBotManager);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientCombatRecordManager);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientGuildProfitManager);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientArenaPlayBackManager);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientTaskManager);
	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientReportManager);

	REGISTER_INIT_LUA_EXPORT_OBJECT_FUNC(LC_ClientGuessDataBase);
}