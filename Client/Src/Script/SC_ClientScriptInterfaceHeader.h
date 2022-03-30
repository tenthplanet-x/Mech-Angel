////////////////////////////////////////////////
//由Perl脚本自动生成的Lua绑定代码，请勿手动修改！//
//这些函数实现Lua调用和返回时对象的自动压栈
////////////////////////////////////////////////
#include "LS_LuaRegister.h"
#include "MM_BaseObject.h"
	
namespace GameLogic
{
	class LC_LogicObject;
};
namespace Utility
{
	class UT_Vec2Int;
};
namespace Utility
{
	class UT_Vec3Int;
};
namespace Utility
{
	class UT_Vec4Int;
};
namespace GameLogic
{
	class LC_Attacker;
};
namespace Skill
{
	class SK_SkillEvent;
};
namespace GameLogic
{
	class LC_ActorBase;
};
namespace GameLogic
{
	class LC_PlayerBase;
};
class LC_ClientNetPlayer;
namespace GameLogic
{
	class LC_NPCBase;
};
class LC_ClientNPC;
class LC_ClientGamePlayer;
class LC_ClientGameStoryManager;
class LC_ClientOpenAbilityManager;
class LC_ClientTranscriptionInfoManager;
class GameStoryRaidInfo;
class GameStoryInfo;
namespace GameLogic
{
	class LC_ActorAttributeMap;
};
namespace GameLogic
{
	class LC_ActorAttributeAppendMap;
};
namespace GameLogic
{
	class LC_BackPackEntry;
};
namespace GameLogic
{
	class LC_PackAsset;
};
namespace GameLogic
{
	class LC_GreatKungFuAsset;
};
namespace GameLogic
{
	class LC_ClientAlliance;
};
namespace GameLogic
{
	class LC_ItemDescriptionEntry;
};
namespace GameLogic
{
	class LC_CrossRealmTeamAsset;
};
namespace GameLogic
{
	class LC_ItemAppendAttributeFormulaEntry;
};
namespace GameLogic
{
	class LC_ItemUIContentEntry;
};
namespace GameLogic
{
	class LC_ItemFactoryBase;
};
namespace GameLogic
{
	class LC_ItemOperationDataEntry;
};
namespace GameLogic
{
	class LC_Task_Condition;
};
namespace GameLogic
{
	class LC_TaskManager;
};
class LC_ClientTaskManager;
namespace GameLogic
{
	class LC_ProcessingTaskEntry;
};
class LC_CanStartTaskMapEntry;
namespace GameLogic
{
	class LC_FinishedTaskRecord;
};
namespace GameLogic
{
	class LC_MainTaskDescriptEntry;
};
namespace GameLogic
{
	class LC_MainTask;
};
namespace GameLogic
{
	class LC_TaskParams;
};
namespace GameLogic
{
	class LC_Task;
};
namespace GameLogic
{
	class LC_DailyLivenessEntry;
};
class LC_ClientTask;
namespace GameLogic
{
	class LC_TaskMap;
};
namespace GameLogic
{
	class LC_AssignTaskInfo;
};
namespace GameLogic
{
	class LC_TaskAssignEntry;
};
namespace GameLogic
{
	class LC_AssignTaskData;
};
class LC_ClientTaskMap;
namespace GameLogic
{
	class LC_TradeManager;
};
namespace GameLogic
{
	class LC_NPCShopItemEntry;
};
namespace GameLogic
{
	class LC_ShopBase;
};
namespace GameLogic
{
	class LC_TransferBase;
};
namespace GameLogic
{
	class LC_SkillCoolDownMap;
};
namespace GameLogic
{
	class LC_ItemCoolDownMapBase;
};
namespace GameLogic
{
	class LC_SkillAssetEntry;
};
namespace Skill
{
	class LC_SkillDescriptionEntry;
};
namespace GameLogic
{
	class LC_SkillAsset;
};
namespace GameLogic
{
	class LC_ActorStateEntry;
};
namespace Skill
{
	class LC_StateDescriptionEntry;
};
namespace GameLogic
{
	class LC_ActorStateMap;
};
namespace GameLogic
{
	class LC_ShortcutEntry;
};
namespace GameLogic
{
	class LC_ShortcutBar;
};
class TT_TextManager;
class LC_TooltipManager;
class LC_MapManager;
class LC_MiniMap;
class LC_TownMap;
class LC_ObjectDescription;
class LC_RadarObjectDescription;
class LC_TownIconDescription;
class LC_PathDesc;
class LC_RadarMap;
namespace GameLogic
{
	class LC_PlayerGroupEntry;
};
namespace GameLogic
{
	class LC_PlayerGroupBase;
};
namespace GameLogic
{
	class LC_ShopCityGoodsDescriptionEntry;
};
namespace GameLogic
{
	class LC_ClientShopCity;
};
namespace GameLogic
{
	class LC_PKRecorderBase;
};
class SC_ClientScriptInterface;
class SC_ClientSkillInterface;
class SC_ClientDebugInterface;
namespace Path
{
	class PF_Path;
};
namespace GameLogic
{
	class LC_SkillInfoEntry;
};
class LC_PlayerFlopManager;
namespace Utility
{
	class UT_GlobalSettings;
};
namespace Utility
{
	class GlobalSettings;
};
class LC_ProductionAssetEntry;
class LC_ClientProduction;
class LC_ClientDialogManager;
class LC_ClientDialogInfo;
class LC_DialogSet;
class LC_DialogNodeData;
namespace GameLogic
{
	class LC_ClientMailManager;
};
namespace GameLogic
{
	class LC_MailAsset;
};
namespace GameLogic
{
	class LC_MailInfo;
};
namespace GameLogic
{
	class LC_ClientItemAffixInfo;
};
namespace GameLogic
{
	class LC_ClientAuctionManager;
};
namespace GameLogic
{
	class LC_ClientAddItemInfo;
};
namespace GameLogic
{
	class LC_ClientAuctionBuyData;
};
namespace GameLogic
{
	class LC_ClientAuctionSellData;
};
namespace GameLogic
{
	class LC_ClientGuildManager;
};
namespace GameLogic
{
	class LC_GuildAsset;
};
namespace GameLogic
{
	class LC_GuildInfo;
};
namespace GameLogic
{
	class LC_ClientGuildMilestoneStruct;
};
namespace GameLogic
{
	class LC_ClientGuildSkillInfoStruct;
};
namespace GameLogic
{
	class LC_ClientGuildSingleSkill;
};
namespace GameLogic
{
	class LC_GuildMemberEntry;
};
namespace GameLogic
{
	class LC_GuildMemberInfo;
};
namespace GameLogic
{
	class LC_GuildApplyEntry;
};
namespace GameLogic
{
	class LC_GuildApplyInfo;
};
namespace GameLogic
{
	class LC_GuildSearchInfo;
};
namespace GameLogic
{
	class LC_TutorialEntry;
};
namespace GameLogic
{
	class LC_TutorialAsset;
};
namespace GameLogic
{
	class LC_GameEventBase;
};
namespace GameLogic
{
	class LC_GameEvent;
};
class LC_ClientGameEvent;
namespace GameLogic
{
	class LC_TaskScriptImportParam;
};
class LC_ClientMapTransDataEntry;
class LC_ClientSocietyAsset;
namespace GameLogic
{
	class LC_ClientWorldLevelAsset;
};
namespace GameLogic
{
	class LC_SocietyBasicInfo;
};
namespace GameLogic
{
	class LC_FriendDetailInfo;
};
class LC_ClientFriendMsgBroadStruct;
class LC_ClientFriendMsgBroad;
namespace GameLogic
{
	class LC_ClientViewPlayerInfo;
};
class LC_ClientFriendNick;
class LC_FriendNick;
class TT_TextElement;
class LC_ShortcutKeyAssetEntry;
class LC_ShortcutKeyAsset;
class LC_ClientArenaPlayerDataEntry;
class LC_ClientArenaManager;
namespace GameLogic
{
	class LC_ArenaEntry;
};
namespace GameLogic
{
	class LC_ArenaAsset;
};
namespace GameLogic
{
	class LC_VIPAsset;
};
namespace GameLogic
{
	class LC_VIPFDepositEntry;
};
namespace GameLogic
{
	class LC_SignInAssert;
};
namespace GameLogic
{
	class LC_RaidAssert;
};
namespace GameLogic
{
	class LC_OpenAbilityAsset;
};
namespace GameLogic
{
	class LC_SubordinateAsset;
};
namespace GameLogic
{
	class LC_MeltAsset;
};
namespace GameLogic
{
	class LC_BoneSoulAsset;
};
namespace GameLogic
{
	class LC_StarsRuneAsset;
};
namespace GameLogic
{
	class LC_CommonStatRewardDataEntry;
};
namespace GameLogic
{
	class LC_ActorOperationEntry;
};
namespace GameLogic
{
	class GuildWareHousePack;
};
namespace GameLogic
{
	class LC_ClientGuildHomeMgr;
};
class UI_UIManager;
class LC_ClientArenaBattleChallengeInfoAsset;
class LC_ClientGameEventUserDataBase;
class LC_ClientGameEventUserData_RaidSettle;
class LC_ClientGameEventUserData_String;
class LC_ClientRankUserInfo;
class LC_ClientBattleRecordDataBase;
class LC_ClientGuessDataBase;
class LC_ClientGameEventUserData_ArenaBattle_BattleInfo;
class LC_ClientGameEventUserData_ArenaSettleInfo;
class LC_ClientGameEventUserData_Common;
namespace GameLogic
{
	class LC_CommonStatAsset;
};
namespace GameLogic
{
	class LC_CommonStatEntry;
};
namespace GameLogic
{
	class LC_TreasureChestsAsset;
};
class ACT_SecretTreasureAsset;
class LC_SkillBookAsset;
namespace GameLogic
{
	class LC_CommonStatManager;
};
namespace GameLogic
{
	class LC_ClientSoulRefineAsset;
};
namespace GameLogic
{
	class LC_ClientShotActivityManager;
};
namespace GameLogic
{
	class LC_FashionBagAsset;
};
namespace GameLogic
{
	class LC_ClientRumorManager;
};
namespace GameLogic
{
	class LC_ClientActivityManager;
};
namespace GameLogic
{
	class LC_ClientActivityAsset;
};
class LC_AchievementManager;
class LC_ClientToyPetManager;
class LC_ClientCombatRecordManager;
class LC_ClientGuildProfitManager;
class LC_ClientArenaPlayBackManager;
class LC_ClientReportManager;
class LC_ClientToyPetInstance;
class LC_ClientCollectionManager;
class LC_ClientBotManager;
namespace Utility
{
	
#ifndef H_SCRIPT_HANDGameLogicLC_LogicObject
#define H_SCRIPT_HANDGameLogicLC_LogicObject
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_LogicObject
	void	LuaPushValue(lua_State* L, GameLogic::LC_LogicObject* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_LogicObject*>, lua_State* L, int idx);
	GameLogic::LC_LogicObject*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_LogicObject*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDUtilityUT_Vec2Int
#define H_SCRIPT_HANDUtilityUT_Vec2Int
#define SC_ClientScriptInterface_SCRIPT_HANDUtilityUT_Vec2Int
	void	LuaPushValue(lua_State* L, Utility::UT_Vec2Int* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<Utility::UT_Vec2Int*>, lua_State* L, int idx);
	Utility::UT_Vec2Int*	LuaGetValue(Utility::LuaTypeWrapper<Utility::UT_Vec2Int*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDUtilityUT_Vec3Int
#define H_SCRIPT_HANDUtilityUT_Vec3Int
#define SC_ClientScriptInterface_SCRIPT_HANDUtilityUT_Vec3Int
	void	LuaPushValue(lua_State* L, Utility::UT_Vec3Int* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<Utility::UT_Vec3Int*>, lua_State* L, int idx);
	Utility::UT_Vec3Int*	LuaGetValue(Utility::LuaTypeWrapper<Utility::UT_Vec3Int*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDUtilityUT_Vec4Int
#define H_SCRIPT_HANDUtilityUT_Vec4Int
#define SC_ClientScriptInterface_SCRIPT_HANDUtilityUT_Vec4Int
	void	LuaPushValue(lua_State* L, Utility::UT_Vec4Int* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<Utility::UT_Vec4Int*>, lua_State* L, int idx);
	Utility::UT_Vec4Int*	LuaGetValue(Utility::LuaTypeWrapper<Utility::UT_Vec4Int*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_Attacker
#define H_SCRIPT_HANDGameLogicLC_Attacker
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_Attacker
	void	LuaPushValue(lua_State* L, GameLogic::LC_Attacker* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_Attacker*>, lua_State* L, int idx);
	GameLogic::LC_Attacker*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_Attacker*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDSkillSK_SkillEvent
#define H_SCRIPT_HANDSkillSK_SkillEvent
#define SC_ClientScriptInterface_SCRIPT_HANDSkillSK_SkillEvent
	void	LuaPushValue(lua_State* L, Skill::SK_SkillEvent* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<Skill::SK_SkillEvent*>, lua_State* L, int idx);
	Skill::SK_SkillEvent*	LuaGetValue(Utility::LuaTypeWrapper<Skill::SK_SkillEvent*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_ActorBase
#define H_SCRIPT_HANDGameLogicLC_ActorBase
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ActorBase
	void	LuaPushValue(lua_State* L, GameLogic::LC_ActorBase* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ActorBase*>, lua_State* L, int idx);
	GameLogic::LC_ActorBase*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ActorBase*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_PlayerBase
#define H_SCRIPT_HANDGameLogicLC_PlayerBase
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_PlayerBase
	void	LuaPushValue(lua_State* L, GameLogic::LC_PlayerBase* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_PlayerBase*>, lua_State* L, int idx);
	GameLogic::LC_PlayerBase*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_PlayerBase*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_ClientNetPlayer
#define H_SCRIPT_HANDLC_ClientNetPlayer
#define SC_ClientScriptInterface_SCRIPT_HANDLC_ClientNetPlayer
	void	LuaPushValue(lua_State* L, LC_ClientNetPlayer* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientNetPlayer*>, lua_State* L, int idx);
	LC_ClientNetPlayer*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientNetPlayer*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_NPCBase
#define H_SCRIPT_HANDGameLogicLC_NPCBase
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_NPCBase
	void	LuaPushValue(lua_State* L, GameLogic::LC_NPCBase* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_NPCBase*>, lua_State* L, int idx);
	GameLogic::LC_NPCBase*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_NPCBase*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_ClientNPC
#define H_SCRIPT_HANDLC_ClientNPC
#define SC_ClientScriptInterface_SCRIPT_HANDLC_ClientNPC
	void	LuaPushValue(lua_State* L, LC_ClientNPC* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientNPC*>, lua_State* L, int idx);
	LC_ClientNPC*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientNPC*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_ClientGamePlayer
#define H_SCRIPT_HANDLC_ClientGamePlayer
#define SC_ClientScriptInterface_SCRIPT_HANDLC_ClientGamePlayer
	void	LuaPushValue(lua_State* L, LC_ClientGamePlayer* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientGamePlayer*>, lua_State* L, int idx);
	LC_ClientGamePlayer*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientGamePlayer*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_ClientGameStoryManager
#define H_SCRIPT_HANDLC_ClientGameStoryManager
#define SC_ClientScriptInterface_SCRIPT_HANDLC_ClientGameStoryManager
	void	LuaPushValue(lua_State* L, LC_ClientGameStoryManager* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientGameStoryManager*>, lua_State* L, int idx);
	LC_ClientGameStoryManager*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientGameStoryManager*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_ClientOpenAbilityManager
#define H_SCRIPT_HANDLC_ClientOpenAbilityManager
#define SC_ClientScriptInterface_SCRIPT_HANDLC_ClientOpenAbilityManager
	void	LuaPushValue(lua_State* L, LC_ClientOpenAbilityManager* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientOpenAbilityManager*>, lua_State* L, int idx);
	LC_ClientOpenAbilityManager*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientOpenAbilityManager*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_ClientTranscriptionInfoManager
#define H_SCRIPT_HANDLC_ClientTranscriptionInfoManager
#define SC_ClientScriptInterface_SCRIPT_HANDLC_ClientTranscriptionInfoManager
	void	LuaPushValue(lua_State* L, LC_ClientTranscriptionInfoManager* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientTranscriptionInfoManager*>, lua_State* L, int idx);
	LC_ClientTranscriptionInfoManager*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientTranscriptionInfoManager*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameStoryRaidInfo
#define H_SCRIPT_HANDGameStoryRaidInfo
#define SC_ClientScriptInterface_SCRIPT_HANDGameStoryRaidInfo
	void	LuaPushValue(lua_State* L, GameStoryRaidInfo* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameStoryRaidInfo*>, lua_State* L, int idx);
	GameStoryRaidInfo*	LuaGetValue(Utility::LuaTypeWrapper<GameStoryRaidInfo*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameStoryInfo
#define H_SCRIPT_HANDGameStoryInfo
#define SC_ClientScriptInterface_SCRIPT_HANDGameStoryInfo
	void	LuaPushValue(lua_State* L, GameStoryInfo* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameStoryInfo*>, lua_State* L, int idx);
	GameStoryInfo*	LuaGetValue(Utility::LuaTypeWrapper<GameStoryInfo*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_ActorAttributeMap
#define H_SCRIPT_HANDGameLogicLC_ActorAttributeMap
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ActorAttributeMap
	void	LuaPushValue(lua_State* L, GameLogic::LC_ActorAttributeMap* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ActorAttributeMap*>, lua_State* L, int idx);
	GameLogic::LC_ActorAttributeMap*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ActorAttributeMap*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_ActorAttributeAppendMap
#define H_SCRIPT_HANDGameLogicLC_ActorAttributeAppendMap
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ActorAttributeAppendMap
	void	LuaPushValue(lua_State* L, GameLogic::LC_ActorAttributeAppendMap* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ActorAttributeAppendMap*>, lua_State* L, int idx);
	GameLogic::LC_ActorAttributeAppendMap*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ActorAttributeAppendMap*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_BackPackEntry
#define H_SCRIPT_HANDGameLogicLC_BackPackEntry
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_BackPackEntry
	void	LuaPushValue(lua_State* L, GameLogic::LC_BackPackEntry* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_BackPackEntry*>, lua_State* L, int idx);
	GameLogic::LC_BackPackEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_BackPackEntry*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_PackAsset
#define H_SCRIPT_HANDGameLogicLC_PackAsset
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_PackAsset
	void	LuaPushValue(lua_State* L, GameLogic::LC_PackAsset* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_PackAsset*>, lua_State* L, int idx);
	GameLogic::LC_PackAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_PackAsset*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_GreatKungFuAsset
#define H_SCRIPT_HANDGameLogicLC_GreatKungFuAsset
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_GreatKungFuAsset
	void	LuaPushValue(lua_State* L, GameLogic::LC_GreatKungFuAsset* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_GreatKungFuAsset*>, lua_State* L, int idx);
	GameLogic::LC_GreatKungFuAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_GreatKungFuAsset*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_ClientAlliance
#define H_SCRIPT_HANDGameLogicLC_ClientAlliance
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ClientAlliance
	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientAlliance* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientAlliance*>, lua_State* L, int idx);
	GameLogic::LC_ClientAlliance*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientAlliance*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_ItemDescriptionEntry
#define H_SCRIPT_HANDGameLogicLC_ItemDescriptionEntry
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ItemDescriptionEntry
	void	LuaPushValue(lua_State* L, GameLogic::LC_ItemDescriptionEntry* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ItemDescriptionEntry*>, lua_State* L, int idx);
	GameLogic::LC_ItemDescriptionEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ItemDescriptionEntry*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_CrossRealmTeamAsset
#define H_SCRIPT_HANDGameLogicLC_CrossRealmTeamAsset
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_CrossRealmTeamAsset
	void	LuaPushValue(lua_State* L, GameLogic::LC_CrossRealmTeamAsset* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_CrossRealmTeamAsset*>, lua_State* L, int idx);
	GameLogic::LC_CrossRealmTeamAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_CrossRealmTeamAsset*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_ItemAppendAttributeFormulaEntry
#define H_SCRIPT_HANDGameLogicLC_ItemAppendAttributeFormulaEntry
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ItemAppendAttributeFormulaEntry
	void	LuaPushValue(lua_State* L, GameLogic::LC_ItemAppendAttributeFormulaEntry* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ItemAppendAttributeFormulaEntry*>, lua_State* L, int idx);
	GameLogic::LC_ItemAppendAttributeFormulaEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ItemAppendAttributeFormulaEntry*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_ItemUIContentEntry
#define H_SCRIPT_HANDGameLogicLC_ItemUIContentEntry
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ItemUIContentEntry
	void	LuaPushValue(lua_State* L, GameLogic::LC_ItemUIContentEntry* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ItemUIContentEntry*>, lua_State* L, int idx);
	GameLogic::LC_ItemUIContentEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ItemUIContentEntry*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_ItemFactoryBase
#define H_SCRIPT_HANDGameLogicLC_ItemFactoryBase
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ItemFactoryBase
	void	LuaPushValue(lua_State* L, GameLogic::LC_ItemFactoryBase* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ItemFactoryBase*>, lua_State* L, int idx);
	GameLogic::LC_ItemFactoryBase*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ItemFactoryBase*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_ItemOperationDataEntry
#define H_SCRIPT_HANDGameLogicLC_ItemOperationDataEntry
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ItemOperationDataEntry
	void	LuaPushValue(lua_State* L, GameLogic::LC_ItemOperationDataEntry* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ItemOperationDataEntry*>, lua_State* L, int idx);
	GameLogic::LC_ItemOperationDataEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ItemOperationDataEntry*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_Task_Condition
#define H_SCRIPT_HANDGameLogicLC_Task_Condition
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_Task_Condition
	void	LuaPushValue(lua_State* L, GameLogic::LC_Task_Condition* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_Task_Condition*>, lua_State* L, int idx);
	GameLogic::LC_Task_Condition*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_Task_Condition*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_TaskManager
#define H_SCRIPT_HANDGameLogicLC_TaskManager
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_TaskManager
	void	LuaPushValue(lua_State* L, GameLogic::LC_TaskManager* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_TaskManager*>, lua_State* L, int idx);
	GameLogic::LC_TaskManager*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_TaskManager*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_ClientTaskManager
#define H_SCRIPT_HANDLC_ClientTaskManager
#define SC_ClientScriptInterface_SCRIPT_HANDLC_ClientTaskManager
	void	LuaPushValue(lua_State* L, LC_ClientTaskManager* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientTaskManager*>, lua_State* L, int idx);
	LC_ClientTaskManager*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientTaskManager*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_ProcessingTaskEntry
#define H_SCRIPT_HANDGameLogicLC_ProcessingTaskEntry
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ProcessingTaskEntry
	void	LuaPushValue(lua_State* L, GameLogic::LC_ProcessingTaskEntry* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ProcessingTaskEntry*>, lua_State* L, int idx);
	GameLogic::LC_ProcessingTaskEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ProcessingTaskEntry*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_CanStartTaskMapEntry
#define H_SCRIPT_HANDLC_CanStartTaskMapEntry
#define SC_ClientScriptInterface_SCRIPT_HANDLC_CanStartTaskMapEntry
	void	LuaPushValue(lua_State* L, LC_CanStartTaskMapEntry* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_CanStartTaskMapEntry*>, lua_State* L, int idx);
	LC_CanStartTaskMapEntry*	LuaGetValue(Utility::LuaTypeWrapper<LC_CanStartTaskMapEntry*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_FinishedTaskRecord
#define H_SCRIPT_HANDGameLogicLC_FinishedTaskRecord
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_FinishedTaskRecord
	void	LuaPushValue(lua_State* L, GameLogic::LC_FinishedTaskRecord* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_FinishedTaskRecord*>, lua_State* L, int idx);
	GameLogic::LC_FinishedTaskRecord*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_FinishedTaskRecord*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_MainTaskDescriptEntry
#define H_SCRIPT_HANDGameLogicLC_MainTaskDescriptEntry
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_MainTaskDescriptEntry
	void	LuaPushValue(lua_State* L, GameLogic::LC_MainTaskDescriptEntry* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_MainTaskDescriptEntry*>, lua_State* L, int idx);
	GameLogic::LC_MainTaskDescriptEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_MainTaskDescriptEntry*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_MainTask
#define H_SCRIPT_HANDGameLogicLC_MainTask
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_MainTask
	void	LuaPushValue(lua_State* L, GameLogic::LC_MainTask* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_MainTask*>, lua_State* L, int idx);
	GameLogic::LC_MainTask*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_MainTask*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_TaskParams
#define H_SCRIPT_HANDGameLogicLC_TaskParams
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_TaskParams
	void	LuaPushValue(lua_State* L, GameLogic::LC_TaskParams* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_TaskParams*>, lua_State* L, int idx);
	GameLogic::LC_TaskParams*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_TaskParams*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_Task
#define H_SCRIPT_HANDGameLogicLC_Task
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_Task
	void	LuaPushValue(lua_State* L, GameLogic::LC_Task* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_Task*>, lua_State* L, int idx);
	GameLogic::LC_Task*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_Task*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_DailyLivenessEntry
#define H_SCRIPT_HANDGameLogicLC_DailyLivenessEntry
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_DailyLivenessEntry
	void	LuaPushValue(lua_State* L, GameLogic::LC_DailyLivenessEntry* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_DailyLivenessEntry*>, lua_State* L, int idx);
	GameLogic::LC_DailyLivenessEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_DailyLivenessEntry*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_ClientTask
#define H_SCRIPT_HANDLC_ClientTask
#define SC_ClientScriptInterface_SCRIPT_HANDLC_ClientTask
	void	LuaPushValue(lua_State* L, LC_ClientTask* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientTask*>, lua_State* L, int idx);
	LC_ClientTask*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientTask*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_TaskMap
#define H_SCRIPT_HANDGameLogicLC_TaskMap
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_TaskMap
	void	LuaPushValue(lua_State* L, GameLogic::LC_TaskMap* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_TaskMap*>, lua_State* L, int idx);
	GameLogic::LC_TaskMap*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_TaskMap*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_AssignTaskInfo
#define H_SCRIPT_HANDGameLogicLC_AssignTaskInfo
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_AssignTaskInfo
	void	LuaPushValue(lua_State* L, GameLogic::LC_AssignTaskInfo* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_AssignTaskInfo*>, lua_State* L, int idx);
	GameLogic::LC_AssignTaskInfo*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_AssignTaskInfo*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_TaskAssignEntry
#define H_SCRIPT_HANDGameLogicLC_TaskAssignEntry
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_TaskAssignEntry
	void	LuaPushValue(lua_State* L, GameLogic::LC_TaskAssignEntry* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_TaskAssignEntry*>, lua_State* L, int idx);
	GameLogic::LC_TaskAssignEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_TaskAssignEntry*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_AssignTaskData
#define H_SCRIPT_HANDGameLogicLC_AssignTaskData
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_AssignTaskData
	void	LuaPushValue(lua_State* L, GameLogic::LC_AssignTaskData* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_AssignTaskData*>, lua_State* L, int idx);
	GameLogic::LC_AssignTaskData*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_AssignTaskData*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_ClientTaskMap
#define H_SCRIPT_HANDLC_ClientTaskMap
#define SC_ClientScriptInterface_SCRIPT_HANDLC_ClientTaskMap
	void	LuaPushValue(lua_State* L, LC_ClientTaskMap* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientTaskMap*>, lua_State* L, int idx);
	LC_ClientTaskMap*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientTaskMap*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_TradeManager
#define H_SCRIPT_HANDGameLogicLC_TradeManager
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_TradeManager
	void	LuaPushValue(lua_State* L, GameLogic::LC_TradeManager* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_TradeManager*>, lua_State* L, int idx);
	GameLogic::LC_TradeManager*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_TradeManager*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_NPCShopItemEntry
#define H_SCRIPT_HANDGameLogicLC_NPCShopItemEntry
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_NPCShopItemEntry
	void	LuaPushValue(lua_State* L, GameLogic::LC_NPCShopItemEntry* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_NPCShopItemEntry*>, lua_State* L, int idx);
	GameLogic::LC_NPCShopItemEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_NPCShopItemEntry*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_ShopBase
#define H_SCRIPT_HANDGameLogicLC_ShopBase
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ShopBase
	void	LuaPushValue(lua_State* L, GameLogic::LC_ShopBase* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ShopBase*>, lua_State* L, int idx);
	GameLogic::LC_ShopBase*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ShopBase*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_TransferBase
#define H_SCRIPT_HANDGameLogicLC_TransferBase
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_TransferBase
	void	LuaPushValue(lua_State* L, GameLogic::LC_TransferBase* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_TransferBase*>, lua_State* L, int idx);
	GameLogic::LC_TransferBase*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_TransferBase*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_SkillCoolDownMap
#define H_SCRIPT_HANDGameLogicLC_SkillCoolDownMap
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_SkillCoolDownMap
	void	LuaPushValue(lua_State* L, GameLogic::LC_SkillCoolDownMap* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_SkillCoolDownMap*>, lua_State* L, int idx);
	GameLogic::LC_SkillCoolDownMap*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_SkillCoolDownMap*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_ItemCoolDownMapBase
#define H_SCRIPT_HANDGameLogicLC_ItemCoolDownMapBase
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ItemCoolDownMapBase
	void	LuaPushValue(lua_State* L, GameLogic::LC_ItemCoolDownMapBase* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ItemCoolDownMapBase*>, lua_State* L, int idx);
	GameLogic::LC_ItemCoolDownMapBase*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ItemCoolDownMapBase*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_SkillAssetEntry
#define H_SCRIPT_HANDGameLogicLC_SkillAssetEntry
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_SkillAssetEntry
	void	LuaPushValue(lua_State* L, GameLogic::LC_SkillAssetEntry* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_SkillAssetEntry*>, lua_State* L, int idx);
	GameLogic::LC_SkillAssetEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_SkillAssetEntry*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDSkillLC_SkillDescriptionEntry
#define H_SCRIPT_HANDSkillLC_SkillDescriptionEntry
#define SC_ClientScriptInterface_SCRIPT_HANDSkillLC_SkillDescriptionEntry
	void	LuaPushValue(lua_State* L, Skill::LC_SkillDescriptionEntry* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<Skill::LC_SkillDescriptionEntry*>, lua_State* L, int idx);
	Skill::LC_SkillDescriptionEntry*	LuaGetValue(Utility::LuaTypeWrapper<Skill::LC_SkillDescriptionEntry*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_SkillAsset
#define H_SCRIPT_HANDGameLogicLC_SkillAsset
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_SkillAsset
	void	LuaPushValue(lua_State* L, GameLogic::LC_SkillAsset* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_SkillAsset*>, lua_State* L, int idx);
	GameLogic::LC_SkillAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_SkillAsset*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_ActorStateEntry
#define H_SCRIPT_HANDGameLogicLC_ActorStateEntry
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ActorStateEntry
	void	LuaPushValue(lua_State* L, GameLogic::LC_ActorStateEntry* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ActorStateEntry*>, lua_State* L, int idx);
	GameLogic::LC_ActorStateEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ActorStateEntry*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDSkillLC_StateDescriptionEntry
#define H_SCRIPT_HANDSkillLC_StateDescriptionEntry
#define SC_ClientScriptInterface_SCRIPT_HANDSkillLC_StateDescriptionEntry
	void	LuaPushValue(lua_State* L, Skill::LC_StateDescriptionEntry* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<Skill::LC_StateDescriptionEntry*>, lua_State* L, int idx);
	Skill::LC_StateDescriptionEntry*	LuaGetValue(Utility::LuaTypeWrapper<Skill::LC_StateDescriptionEntry*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_ActorStateMap
#define H_SCRIPT_HANDGameLogicLC_ActorStateMap
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ActorStateMap
	void	LuaPushValue(lua_State* L, GameLogic::LC_ActorStateMap* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ActorStateMap*>, lua_State* L, int idx);
	GameLogic::LC_ActorStateMap*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ActorStateMap*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_ShortcutEntry
#define H_SCRIPT_HANDGameLogicLC_ShortcutEntry
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ShortcutEntry
	void	LuaPushValue(lua_State* L, GameLogic::LC_ShortcutEntry* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ShortcutEntry*>, lua_State* L, int idx);
	GameLogic::LC_ShortcutEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ShortcutEntry*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_ShortcutBar
#define H_SCRIPT_HANDGameLogicLC_ShortcutBar
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ShortcutBar
	void	LuaPushValue(lua_State* L, GameLogic::LC_ShortcutBar* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ShortcutBar*>, lua_State* L, int idx);
	GameLogic::LC_ShortcutBar*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ShortcutBar*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDTT_TextManager
#define H_SCRIPT_HANDTT_TextManager
#define SC_ClientScriptInterface_SCRIPT_HANDTT_TextManager
	void	LuaPushValue(lua_State* L, TT_TextManager* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<TT_TextManager*>, lua_State* L, int idx);
	TT_TextManager*	LuaGetValue(Utility::LuaTypeWrapper<TT_TextManager*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_TooltipManager
#define H_SCRIPT_HANDLC_TooltipManager
#define SC_ClientScriptInterface_SCRIPT_HANDLC_TooltipManager
	void	LuaPushValue(lua_State* L, LC_TooltipManager* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_TooltipManager*>, lua_State* L, int idx);
	LC_TooltipManager*	LuaGetValue(Utility::LuaTypeWrapper<LC_TooltipManager*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_MapManager
#define H_SCRIPT_HANDLC_MapManager
#define SC_ClientScriptInterface_SCRIPT_HANDLC_MapManager
	void	LuaPushValue(lua_State* L, LC_MapManager* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_MapManager*>, lua_State* L, int idx);
	LC_MapManager*	LuaGetValue(Utility::LuaTypeWrapper<LC_MapManager*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_MiniMap
#define H_SCRIPT_HANDLC_MiniMap
#define SC_ClientScriptInterface_SCRIPT_HANDLC_MiniMap
	void	LuaPushValue(lua_State* L, LC_MiniMap* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_MiniMap*>, lua_State* L, int idx);
	LC_MiniMap*	LuaGetValue(Utility::LuaTypeWrapper<LC_MiniMap*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_TownMap
#define H_SCRIPT_HANDLC_TownMap
#define SC_ClientScriptInterface_SCRIPT_HANDLC_TownMap
	void	LuaPushValue(lua_State* L, LC_TownMap* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_TownMap*>, lua_State* L, int idx);
	LC_TownMap*	LuaGetValue(Utility::LuaTypeWrapper<LC_TownMap*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_ObjectDescription
#define H_SCRIPT_HANDLC_ObjectDescription
#define SC_ClientScriptInterface_SCRIPT_HANDLC_ObjectDescription
	void	LuaPushValue(lua_State* L, LC_ObjectDescription* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ObjectDescription*>, lua_State* L, int idx);
	LC_ObjectDescription*	LuaGetValue(Utility::LuaTypeWrapper<LC_ObjectDescription*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_RadarObjectDescription
#define H_SCRIPT_HANDLC_RadarObjectDescription
#define SC_ClientScriptInterface_SCRIPT_HANDLC_RadarObjectDescription
	void	LuaPushValue(lua_State* L, LC_RadarObjectDescription* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_RadarObjectDescription*>, lua_State* L, int idx);
	LC_RadarObjectDescription*	LuaGetValue(Utility::LuaTypeWrapper<LC_RadarObjectDescription*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_TownIconDescription
#define H_SCRIPT_HANDLC_TownIconDescription
#define SC_ClientScriptInterface_SCRIPT_HANDLC_TownIconDescription
	void	LuaPushValue(lua_State* L, LC_TownIconDescription* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_TownIconDescription*>, lua_State* L, int idx);
	LC_TownIconDescription*	LuaGetValue(Utility::LuaTypeWrapper<LC_TownIconDescription*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_PathDesc
#define H_SCRIPT_HANDLC_PathDesc
#define SC_ClientScriptInterface_SCRIPT_HANDLC_PathDesc
	void	LuaPushValue(lua_State* L, LC_PathDesc* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_PathDesc*>, lua_State* L, int idx);
	LC_PathDesc*	LuaGetValue(Utility::LuaTypeWrapper<LC_PathDesc*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_RadarMap
#define H_SCRIPT_HANDLC_RadarMap
#define SC_ClientScriptInterface_SCRIPT_HANDLC_RadarMap
	void	LuaPushValue(lua_State* L, LC_RadarMap* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_RadarMap*>, lua_State* L, int idx);
	LC_RadarMap*	LuaGetValue(Utility::LuaTypeWrapper<LC_RadarMap*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_PlayerGroupEntry
#define H_SCRIPT_HANDGameLogicLC_PlayerGroupEntry
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_PlayerGroupEntry
	void	LuaPushValue(lua_State* L, GameLogic::LC_PlayerGroupEntry* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_PlayerGroupEntry*>, lua_State* L, int idx);
	GameLogic::LC_PlayerGroupEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_PlayerGroupEntry*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_PlayerGroupBase
#define H_SCRIPT_HANDGameLogicLC_PlayerGroupBase
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_PlayerGroupBase
	void	LuaPushValue(lua_State* L, GameLogic::LC_PlayerGroupBase* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_PlayerGroupBase*>, lua_State* L, int idx);
	GameLogic::LC_PlayerGroupBase*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_PlayerGroupBase*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_ShopCityGoodsDescriptionEntry
#define H_SCRIPT_HANDGameLogicLC_ShopCityGoodsDescriptionEntry
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ShopCityGoodsDescriptionEntry
	void	LuaPushValue(lua_State* L, GameLogic::LC_ShopCityGoodsDescriptionEntry* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ShopCityGoodsDescriptionEntry*>, lua_State* L, int idx);
	GameLogic::LC_ShopCityGoodsDescriptionEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ShopCityGoodsDescriptionEntry*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_ClientShopCity
#define H_SCRIPT_HANDGameLogicLC_ClientShopCity
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ClientShopCity
	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientShopCity* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientShopCity*>, lua_State* L, int idx);
	GameLogic::LC_ClientShopCity*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientShopCity*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_PKRecorderBase
#define H_SCRIPT_HANDGameLogicLC_PKRecorderBase
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_PKRecorderBase
	void	LuaPushValue(lua_State* L, GameLogic::LC_PKRecorderBase* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_PKRecorderBase*>, lua_State* L, int idx);
	GameLogic::LC_PKRecorderBase*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_PKRecorderBase*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDSC_ClientScriptInterface
#define H_SCRIPT_HANDSC_ClientScriptInterface
#define SC_ClientScriptInterface_SCRIPT_HANDSC_ClientScriptInterface
	void	LuaPushValue(lua_State* L, SC_ClientScriptInterface* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<SC_ClientScriptInterface*>, lua_State* L, int idx);
	SC_ClientScriptInterface*	LuaGetValue(Utility::LuaTypeWrapper<SC_ClientScriptInterface*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDSC_ClientSkillInterface
#define H_SCRIPT_HANDSC_ClientSkillInterface
#define SC_ClientScriptInterface_SCRIPT_HANDSC_ClientSkillInterface
	void	LuaPushValue(lua_State* L, SC_ClientSkillInterface* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<SC_ClientSkillInterface*>, lua_State* L, int idx);
	SC_ClientSkillInterface*	LuaGetValue(Utility::LuaTypeWrapper<SC_ClientSkillInterface*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDSC_ClientDebugInterface
#define H_SCRIPT_HANDSC_ClientDebugInterface
#define SC_ClientScriptInterface_SCRIPT_HANDSC_ClientDebugInterface
	void	LuaPushValue(lua_State* L, SC_ClientDebugInterface* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<SC_ClientDebugInterface*>, lua_State* L, int idx);
	SC_ClientDebugInterface*	LuaGetValue(Utility::LuaTypeWrapper<SC_ClientDebugInterface*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDPathPF_Path
#define H_SCRIPT_HANDPathPF_Path
#define SC_ClientScriptInterface_SCRIPT_HANDPathPF_Path
	void	LuaPushValue(lua_State* L, Path::PF_Path* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<Path::PF_Path*>, lua_State* L, int idx);
	Path::PF_Path*	LuaGetValue(Utility::LuaTypeWrapper<Path::PF_Path*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_SkillInfoEntry
#define H_SCRIPT_HANDGameLogicLC_SkillInfoEntry
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_SkillInfoEntry
	void	LuaPushValue(lua_State* L, GameLogic::LC_SkillInfoEntry* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_SkillInfoEntry*>, lua_State* L, int idx);
	GameLogic::LC_SkillInfoEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_SkillInfoEntry*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_PlayerFlopManager
#define H_SCRIPT_HANDLC_PlayerFlopManager
#define SC_ClientScriptInterface_SCRIPT_HANDLC_PlayerFlopManager
	void	LuaPushValue(lua_State* L, LC_PlayerFlopManager* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_PlayerFlopManager*>, lua_State* L, int idx);
	LC_PlayerFlopManager*	LuaGetValue(Utility::LuaTypeWrapper<LC_PlayerFlopManager*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDUtilityUT_GlobalSettings
#define H_SCRIPT_HANDUtilityUT_GlobalSettings
#define SC_ClientScriptInterface_SCRIPT_HANDUtilityUT_GlobalSettings
	void	LuaPushValue(lua_State* L, Utility::UT_GlobalSettings* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<Utility::UT_GlobalSettings*>, lua_State* L, int idx);
	Utility::UT_GlobalSettings*	LuaGetValue(Utility::LuaTypeWrapper<Utility::UT_GlobalSettings*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDUtilityGlobalSettings
#define H_SCRIPT_HANDUtilityGlobalSettings
#define SC_ClientScriptInterface_SCRIPT_HANDUtilityGlobalSettings
	void	LuaPushValue(lua_State* L, Utility::GlobalSettings* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<Utility::GlobalSettings*>, lua_State* L, int idx);
	Utility::GlobalSettings*	LuaGetValue(Utility::LuaTypeWrapper<Utility::GlobalSettings*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_ProductionAssetEntry
#define H_SCRIPT_HANDLC_ProductionAssetEntry
#define SC_ClientScriptInterface_SCRIPT_HANDLC_ProductionAssetEntry
	void	LuaPushValue(lua_State* L, LC_ProductionAssetEntry* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ProductionAssetEntry*>, lua_State* L, int idx);
	LC_ProductionAssetEntry*	LuaGetValue(Utility::LuaTypeWrapper<LC_ProductionAssetEntry*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_ClientProduction
#define H_SCRIPT_HANDLC_ClientProduction
#define SC_ClientScriptInterface_SCRIPT_HANDLC_ClientProduction
	void	LuaPushValue(lua_State* L, LC_ClientProduction* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientProduction*>, lua_State* L, int idx);
	LC_ClientProduction*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientProduction*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_ClientDialogManager
#define H_SCRIPT_HANDLC_ClientDialogManager
#define SC_ClientScriptInterface_SCRIPT_HANDLC_ClientDialogManager
	void	LuaPushValue(lua_State* L, LC_ClientDialogManager* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientDialogManager*>, lua_State* L, int idx);
	LC_ClientDialogManager*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientDialogManager*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_ClientDialogInfo
#define H_SCRIPT_HANDLC_ClientDialogInfo
#define SC_ClientScriptInterface_SCRIPT_HANDLC_ClientDialogInfo
	void	LuaPushValue(lua_State* L, LC_ClientDialogInfo* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientDialogInfo*>, lua_State* L, int idx);
	LC_ClientDialogInfo*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientDialogInfo*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_DialogSet
#define H_SCRIPT_HANDLC_DialogSet
#define SC_ClientScriptInterface_SCRIPT_HANDLC_DialogSet
	void	LuaPushValue(lua_State* L, LC_DialogSet* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_DialogSet*>, lua_State* L, int idx);
	LC_DialogSet*	LuaGetValue(Utility::LuaTypeWrapper<LC_DialogSet*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_DialogNodeData
#define H_SCRIPT_HANDLC_DialogNodeData
#define SC_ClientScriptInterface_SCRIPT_HANDLC_DialogNodeData
	void	LuaPushValue(lua_State* L, LC_DialogNodeData* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_DialogNodeData*>, lua_State* L, int idx);
	LC_DialogNodeData*	LuaGetValue(Utility::LuaTypeWrapper<LC_DialogNodeData*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_ClientMailManager
#define H_SCRIPT_HANDGameLogicLC_ClientMailManager
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ClientMailManager
	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientMailManager* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientMailManager*>, lua_State* L, int idx);
	GameLogic::LC_ClientMailManager*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientMailManager*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_MailAsset
#define H_SCRIPT_HANDGameLogicLC_MailAsset
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_MailAsset
	void	LuaPushValue(lua_State* L, GameLogic::LC_MailAsset* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_MailAsset*>, lua_State* L, int idx);
	GameLogic::LC_MailAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_MailAsset*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_MailInfo
#define H_SCRIPT_HANDGameLogicLC_MailInfo
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_MailInfo
	void	LuaPushValue(lua_State* L, GameLogic::LC_MailInfo* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_MailInfo*>, lua_State* L, int idx);
	GameLogic::LC_MailInfo*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_MailInfo*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_ClientItemAffixInfo
#define H_SCRIPT_HANDGameLogicLC_ClientItemAffixInfo
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ClientItemAffixInfo
	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientItemAffixInfo* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientItemAffixInfo*>, lua_State* L, int idx);
	GameLogic::LC_ClientItemAffixInfo*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientItemAffixInfo*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_ClientAuctionManager
#define H_SCRIPT_HANDGameLogicLC_ClientAuctionManager
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ClientAuctionManager
	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientAuctionManager* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientAuctionManager*>, lua_State* L, int idx);
	GameLogic::LC_ClientAuctionManager*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientAuctionManager*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_ClientAddItemInfo
#define H_SCRIPT_HANDGameLogicLC_ClientAddItemInfo
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ClientAddItemInfo
	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientAddItemInfo* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientAddItemInfo*>, lua_State* L, int idx);
	GameLogic::LC_ClientAddItemInfo*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientAddItemInfo*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_ClientAuctionBuyData
#define H_SCRIPT_HANDGameLogicLC_ClientAuctionBuyData
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ClientAuctionBuyData
	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientAuctionBuyData* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientAuctionBuyData*>, lua_State* L, int idx);
	GameLogic::LC_ClientAuctionBuyData*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientAuctionBuyData*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_ClientAuctionSellData
#define H_SCRIPT_HANDGameLogicLC_ClientAuctionSellData
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ClientAuctionSellData
	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientAuctionSellData* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientAuctionSellData*>, lua_State* L, int idx);
	GameLogic::LC_ClientAuctionSellData*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientAuctionSellData*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_ClientGuildManager
#define H_SCRIPT_HANDGameLogicLC_ClientGuildManager
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ClientGuildManager
	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientGuildManager* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientGuildManager*>, lua_State* L, int idx);
	GameLogic::LC_ClientGuildManager*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientGuildManager*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_GuildAsset
#define H_SCRIPT_HANDGameLogicLC_GuildAsset
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_GuildAsset
	void	LuaPushValue(lua_State* L, GameLogic::LC_GuildAsset* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_GuildAsset*>, lua_State* L, int idx);
	GameLogic::LC_GuildAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_GuildAsset*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_GuildInfo
#define H_SCRIPT_HANDGameLogicLC_GuildInfo
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_GuildInfo
	void	LuaPushValue(lua_State* L, GameLogic::LC_GuildInfo* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_GuildInfo*>, lua_State* L, int idx);
	GameLogic::LC_GuildInfo*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_GuildInfo*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_ClientGuildMilestoneStruct
#define H_SCRIPT_HANDGameLogicLC_ClientGuildMilestoneStruct
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ClientGuildMilestoneStruct
	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientGuildMilestoneStruct* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientGuildMilestoneStruct*>, lua_State* L, int idx);
	GameLogic::LC_ClientGuildMilestoneStruct*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientGuildMilestoneStruct*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_ClientGuildSkillInfoStruct
#define H_SCRIPT_HANDGameLogicLC_ClientGuildSkillInfoStruct
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ClientGuildSkillInfoStruct
	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientGuildSkillInfoStruct* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientGuildSkillInfoStruct*>, lua_State* L, int idx);
	GameLogic::LC_ClientGuildSkillInfoStruct*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientGuildSkillInfoStruct*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_ClientGuildSingleSkill
#define H_SCRIPT_HANDGameLogicLC_ClientGuildSingleSkill
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ClientGuildSingleSkill
	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientGuildSingleSkill* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientGuildSingleSkill*>, lua_State* L, int idx);
	GameLogic::LC_ClientGuildSingleSkill*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientGuildSingleSkill*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_GuildMemberEntry
#define H_SCRIPT_HANDGameLogicLC_GuildMemberEntry
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_GuildMemberEntry
	void	LuaPushValue(lua_State* L, GameLogic::LC_GuildMemberEntry* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_GuildMemberEntry*>, lua_State* L, int idx);
	GameLogic::LC_GuildMemberEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_GuildMemberEntry*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_GuildMemberInfo
#define H_SCRIPT_HANDGameLogicLC_GuildMemberInfo
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_GuildMemberInfo
	void	LuaPushValue(lua_State* L, GameLogic::LC_GuildMemberInfo* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_GuildMemberInfo*>, lua_State* L, int idx);
	GameLogic::LC_GuildMemberInfo*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_GuildMemberInfo*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_GuildApplyEntry
#define H_SCRIPT_HANDGameLogicLC_GuildApplyEntry
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_GuildApplyEntry
	void	LuaPushValue(lua_State* L, GameLogic::LC_GuildApplyEntry* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_GuildApplyEntry*>, lua_State* L, int idx);
	GameLogic::LC_GuildApplyEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_GuildApplyEntry*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_GuildApplyInfo
#define H_SCRIPT_HANDGameLogicLC_GuildApplyInfo
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_GuildApplyInfo
	void	LuaPushValue(lua_State* L, GameLogic::LC_GuildApplyInfo* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_GuildApplyInfo*>, lua_State* L, int idx);
	GameLogic::LC_GuildApplyInfo*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_GuildApplyInfo*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_GuildSearchInfo
#define H_SCRIPT_HANDGameLogicLC_GuildSearchInfo
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_GuildSearchInfo
	void	LuaPushValue(lua_State* L, GameLogic::LC_GuildSearchInfo* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_GuildSearchInfo*>, lua_State* L, int idx);
	GameLogic::LC_GuildSearchInfo*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_GuildSearchInfo*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_TutorialEntry
#define H_SCRIPT_HANDGameLogicLC_TutorialEntry
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_TutorialEntry
	void	LuaPushValue(lua_State* L, GameLogic::LC_TutorialEntry* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_TutorialEntry*>, lua_State* L, int idx);
	GameLogic::LC_TutorialEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_TutorialEntry*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_TutorialAsset
#define H_SCRIPT_HANDGameLogicLC_TutorialAsset
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_TutorialAsset
	void	LuaPushValue(lua_State* L, GameLogic::LC_TutorialAsset* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_TutorialAsset*>, lua_State* L, int idx);
	GameLogic::LC_TutorialAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_TutorialAsset*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_GameEventBase
#define H_SCRIPT_HANDGameLogicLC_GameEventBase
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_GameEventBase
	void	LuaPushValue(lua_State* L, GameLogic::LC_GameEventBase* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_GameEventBase*>, lua_State* L, int idx);
	GameLogic::LC_GameEventBase*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_GameEventBase*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_GameEvent
#define H_SCRIPT_HANDGameLogicLC_GameEvent
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_GameEvent
	void	LuaPushValue(lua_State* L, GameLogic::LC_GameEvent* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_GameEvent*>, lua_State* L, int idx);
	GameLogic::LC_GameEvent*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_GameEvent*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_ClientGameEvent
#define H_SCRIPT_HANDLC_ClientGameEvent
#define SC_ClientScriptInterface_SCRIPT_HANDLC_ClientGameEvent
	void	LuaPushValue(lua_State* L, LC_ClientGameEvent* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientGameEvent*>, lua_State* L, int idx);
	LC_ClientGameEvent*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientGameEvent*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_TaskScriptImportParam
#define H_SCRIPT_HANDGameLogicLC_TaskScriptImportParam
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_TaskScriptImportParam
	void	LuaPushValue(lua_State* L, GameLogic::LC_TaskScriptImportParam* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_TaskScriptImportParam*>, lua_State* L, int idx);
	GameLogic::LC_TaskScriptImportParam*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_TaskScriptImportParam*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_ClientMapTransDataEntry
#define H_SCRIPT_HANDLC_ClientMapTransDataEntry
#define SC_ClientScriptInterface_SCRIPT_HANDLC_ClientMapTransDataEntry
	void	LuaPushValue(lua_State* L, LC_ClientMapTransDataEntry* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientMapTransDataEntry*>, lua_State* L, int idx);
	LC_ClientMapTransDataEntry*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientMapTransDataEntry*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_ClientSocietyAsset
#define H_SCRIPT_HANDLC_ClientSocietyAsset
#define SC_ClientScriptInterface_SCRIPT_HANDLC_ClientSocietyAsset
	void	LuaPushValue(lua_State* L, LC_ClientSocietyAsset* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientSocietyAsset*>, lua_State* L, int idx);
	LC_ClientSocietyAsset*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientSocietyAsset*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_ClientWorldLevelAsset
#define H_SCRIPT_HANDGameLogicLC_ClientWorldLevelAsset
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ClientWorldLevelAsset
	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientWorldLevelAsset* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientWorldLevelAsset*>, lua_State* L, int idx);
	GameLogic::LC_ClientWorldLevelAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientWorldLevelAsset*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_SocietyBasicInfo
#define H_SCRIPT_HANDGameLogicLC_SocietyBasicInfo
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_SocietyBasicInfo
	void	LuaPushValue(lua_State* L, GameLogic::LC_SocietyBasicInfo* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_SocietyBasicInfo*>, lua_State* L, int idx);
	GameLogic::LC_SocietyBasicInfo*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_SocietyBasicInfo*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_FriendDetailInfo
#define H_SCRIPT_HANDGameLogicLC_FriendDetailInfo
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_FriendDetailInfo
	void	LuaPushValue(lua_State* L, GameLogic::LC_FriendDetailInfo* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_FriendDetailInfo*>, lua_State* L, int idx);
	GameLogic::LC_FriendDetailInfo*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_FriendDetailInfo*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_ClientFriendMsgBroadStruct
#define H_SCRIPT_HANDLC_ClientFriendMsgBroadStruct
#define SC_ClientScriptInterface_SCRIPT_HANDLC_ClientFriendMsgBroadStruct
	void	LuaPushValue(lua_State* L, LC_ClientFriendMsgBroadStruct* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientFriendMsgBroadStruct*>, lua_State* L, int idx);
	LC_ClientFriendMsgBroadStruct*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientFriendMsgBroadStruct*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_ClientFriendMsgBroad
#define H_SCRIPT_HANDLC_ClientFriendMsgBroad
#define SC_ClientScriptInterface_SCRIPT_HANDLC_ClientFriendMsgBroad
	void	LuaPushValue(lua_State* L, LC_ClientFriendMsgBroad* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientFriendMsgBroad*>, lua_State* L, int idx);
	LC_ClientFriendMsgBroad*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientFriendMsgBroad*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_ClientViewPlayerInfo
#define H_SCRIPT_HANDGameLogicLC_ClientViewPlayerInfo
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ClientViewPlayerInfo
	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientViewPlayerInfo* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientViewPlayerInfo*>, lua_State* L, int idx);
	GameLogic::LC_ClientViewPlayerInfo*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientViewPlayerInfo*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_ClientFriendNick
#define H_SCRIPT_HANDLC_ClientFriendNick
#define SC_ClientScriptInterface_SCRIPT_HANDLC_ClientFriendNick
	void	LuaPushValue(lua_State* L, LC_ClientFriendNick* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientFriendNick*>, lua_State* L, int idx);
	LC_ClientFriendNick*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientFriendNick*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_FriendNick
#define H_SCRIPT_HANDLC_FriendNick
#define SC_ClientScriptInterface_SCRIPT_HANDLC_FriendNick
	void	LuaPushValue(lua_State* L, LC_FriendNick* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_FriendNick*>, lua_State* L, int idx);
	LC_FriendNick*	LuaGetValue(Utility::LuaTypeWrapper<LC_FriendNick*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDTT_TextElement
#define H_SCRIPT_HANDTT_TextElement
#define SC_ClientScriptInterface_SCRIPT_HANDTT_TextElement
	void	LuaPushValue(lua_State* L, TT_TextElement* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<TT_TextElement*>, lua_State* L, int idx);
	TT_TextElement*	LuaGetValue(Utility::LuaTypeWrapper<TT_TextElement*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_ShortcutKeyAssetEntry
#define H_SCRIPT_HANDLC_ShortcutKeyAssetEntry
#define SC_ClientScriptInterface_SCRIPT_HANDLC_ShortcutKeyAssetEntry
	void	LuaPushValue(lua_State* L, LC_ShortcutKeyAssetEntry* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ShortcutKeyAssetEntry*>, lua_State* L, int idx);
	LC_ShortcutKeyAssetEntry*	LuaGetValue(Utility::LuaTypeWrapper<LC_ShortcutKeyAssetEntry*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_ShortcutKeyAsset
#define H_SCRIPT_HANDLC_ShortcutKeyAsset
#define SC_ClientScriptInterface_SCRIPT_HANDLC_ShortcutKeyAsset
	void	LuaPushValue(lua_State* L, LC_ShortcutKeyAsset* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ShortcutKeyAsset*>, lua_State* L, int idx);
	LC_ShortcutKeyAsset*	LuaGetValue(Utility::LuaTypeWrapper<LC_ShortcutKeyAsset*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_ClientArenaPlayerDataEntry
#define H_SCRIPT_HANDLC_ClientArenaPlayerDataEntry
#define SC_ClientScriptInterface_SCRIPT_HANDLC_ClientArenaPlayerDataEntry
	void	LuaPushValue(lua_State* L, LC_ClientArenaPlayerDataEntry* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientArenaPlayerDataEntry*>, lua_State* L, int idx);
	LC_ClientArenaPlayerDataEntry*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientArenaPlayerDataEntry*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_ClientArenaManager
#define H_SCRIPT_HANDLC_ClientArenaManager
#define SC_ClientScriptInterface_SCRIPT_HANDLC_ClientArenaManager
	void	LuaPushValue(lua_State* L, LC_ClientArenaManager* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientArenaManager*>, lua_State* L, int idx);
	LC_ClientArenaManager*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientArenaManager*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_ArenaEntry
#define H_SCRIPT_HANDGameLogicLC_ArenaEntry
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ArenaEntry
	void	LuaPushValue(lua_State* L, GameLogic::LC_ArenaEntry* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ArenaEntry*>, lua_State* L, int idx);
	GameLogic::LC_ArenaEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ArenaEntry*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_ArenaAsset
#define H_SCRIPT_HANDGameLogicLC_ArenaAsset
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ArenaAsset
	void	LuaPushValue(lua_State* L, GameLogic::LC_ArenaAsset* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ArenaAsset*>, lua_State* L, int idx);
	GameLogic::LC_ArenaAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ArenaAsset*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_VIPAsset
#define H_SCRIPT_HANDGameLogicLC_VIPAsset
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_VIPAsset
	void	LuaPushValue(lua_State* L, GameLogic::LC_VIPAsset* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_VIPAsset*>, lua_State* L, int idx);
	GameLogic::LC_VIPAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_VIPAsset*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_VIPFDepositEntry
#define H_SCRIPT_HANDGameLogicLC_VIPFDepositEntry
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_VIPFDepositEntry
	void	LuaPushValue(lua_State* L, GameLogic::LC_VIPFDepositEntry* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_VIPFDepositEntry*>, lua_State* L, int idx);
	GameLogic::LC_VIPFDepositEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_VIPFDepositEntry*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_SignInAssert
#define H_SCRIPT_HANDGameLogicLC_SignInAssert
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_SignInAssert
	void	LuaPushValue(lua_State* L, GameLogic::LC_SignInAssert* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_SignInAssert*>, lua_State* L, int idx);
	GameLogic::LC_SignInAssert*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_SignInAssert*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_RaidAssert
#define H_SCRIPT_HANDGameLogicLC_RaidAssert
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_RaidAssert
	void	LuaPushValue(lua_State* L, GameLogic::LC_RaidAssert* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_RaidAssert*>, lua_State* L, int idx);
	GameLogic::LC_RaidAssert*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_RaidAssert*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_OpenAbilityAsset
#define H_SCRIPT_HANDGameLogicLC_OpenAbilityAsset
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_OpenAbilityAsset
	void	LuaPushValue(lua_State* L, GameLogic::LC_OpenAbilityAsset* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_OpenAbilityAsset*>, lua_State* L, int idx);
	GameLogic::LC_OpenAbilityAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_OpenAbilityAsset*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_SubordinateAsset
#define H_SCRIPT_HANDGameLogicLC_SubordinateAsset
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_SubordinateAsset
	void	LuaPushValue(lua_State* L, GameLogic::LC_SubordinateAsset* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_SubordinateAsset*>, lua_State* L, int idx);
	GameLogic::LC_SubordinateAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_SubordinateAsset*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_MeltAsset
#define H_SCRIPT_HANDGameLogicLC_MeltAsset
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_MeltAsset
	void	LuaPushValue(lua_State* L, GameLogic::LC_MeltAsset* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_MeltAsset*>, lua_State* L, int idx);
	GameLogic::LC_MeltAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_MeltAsset*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_BoneSoulAsset
#define H_SCRIPT_HANDGameLogicLC_BoneSoulAsset
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_BoneSoulAsset
	void	LuaPushValue(lua_State* L, GameLogic::LC_BoneSoulAsset* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_BoneSoulAsset*>, lua_State* L, int idx);
	GameLogic::LC_BoneSoulAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_BoneSoulAsset*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_StarsRuneAsset
#define H_SCRIPT_HANDGameLogicLC_StarsRuneAsset
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_StarsRuneAsset
	void	LuaPushValue(lua_State* L, GameLogic::LC_StarsRuneAsset* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_StarsRuneAsset*>, lua_State* L, int idx);
	GameLogic::LC_StarsRuneAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_StarsRuneAsset*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_CommonStatRewardDataEntry
#define H_SCRIPT_HANDGameLogicLC_CommonStatRewardDataEntry
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_CommonStatRewardDataEntry
	void	LuaPushValue(lua_State* L, GameLogic::LC_CommonStatRewardDataEntry* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_CommonStatRewardDataEntry*>, lua_State* L, int idx);
	GameLogic::LC_CommonStatRewardDataEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_CommonStatRewardDataEntry*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_ActorOperationEntry
#define H_SCRIPT_HANDGameLogicLC_ActorOperationEntry
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ActorOperationEntry
	void	LuaPushValue(lua_State* L, GameLogic::LC_ActorOperationEntry* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ActorOperationEntry*>, lua_State* L, int idx);
	GameLogic::LC_ActorOperationEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ActorOperationEntry*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicGuildWareHousePack
#define H_SCRIPT_HANDGameLogicGuildWareHousePack
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicGuildWareHousePack
	void	LuaPushValue(lua_State* L, GameLogic::GuildWareHousePack* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::GuildWareHousePack*>, lua_State* L, int idx);
	GameLogic::GuildWareHousePack*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::GuildWareHousePack*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_ClientGuildHomeMgr
#define H_SCRIPT_HANDGameLogicLC_ClientGuildHomeMgr
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ClientGuildHomeMgr
	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientGuildHomeMgr* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientGuildHomeMgr*>, lua_State* L, int idx);
	GameLogic::LC_ClientGuildHomeMgr*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientGuildHomeMgr*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDUI_UIManager
#define H_SCRIPT_HANDUI_UIManager
#define SC_ClientScriptInterface_SCRIPT_HANDUI_UIManager
	void	LuaPushValue(lua_State* L, UI_UIManager* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<UI_UIManager*>, lua_State* L, int idx);
	UI_UIManager*	LuaGetValue(Utility::LuaTypeWrapper<UI_UIManager*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_ClientArenaBattleChallengeInfoAsset
#define H_SCRIPT_HANDLC_ClientArenaBattleChallengeInfoAsset
#define SC_ClientScriptInterface_SCRIPT_HANDLC_ClientArenaBattleChallengeInfoAsset
	void	LuaPushValue(lua_State* L, LC_ClientArenaBattleChallengeInfoAsset* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientArenaBattleChallengeInfoAsset*>, lua_State* L, int idx);
	LC_ClientArenaBattleChallengeInfoAsset*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientArenaBattleChallengeInfoAsset*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_ClientGameEventUserDataBase
#define H_SCRIPT_HANDLC_ClientGameEventUserDataBase
#define SC_ClientScriptInterface_SCRIPT_HANDLC_ClientGameEventUserDataBase
	void	LuaPushValue(lua_State* L, LC_ClientGameEventUserDataBase* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientGameEventUserDataBase*>, lua_State* L, int idx);
	LC_ClientGameEventUserDataBase*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientGameEventUserDataBase*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_ClientGameEventUserData_RaidSettle
#define H_SCRIPT_HANDLC_ClientGameEventUserData_RaidSettle
#define SC_ClientScriptInterface_SCRIPT_HANDLC_ClientGameEventUserData_RaidSettle
	void	LuaPushValue(lua_State* L, LC_ClientGameEventUserData_RaidSettle* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientGameEventUserData_RaidSettle*>, lua_State* L, int idx);
	LC_ClientGameEventUserData_RaidSettle*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientGameEventUserData_RaidSettle*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_ClientGameEventUserData_String
#define H_SCRIPT_HANDLC_ClientGameEventUserData_String
#define SC_ClientScriptInterface_SCRIPT_HANDLC_ClientGameEventUserData_String
	void	LuaPushValue(lua_State* L, LC_ClientGameEventUserData_String* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientGameEventUserData_String*>, lua_State* L, int idx);
	LC_ClientGameEventUserData_String*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientGameEventUserData_String*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_ClientRankUserInfo
#define H_SCRIPT_HANDLC_ClientRankUserInfo
#define SC_ClientScriptInterface_SCRIPT_HANDLC_ClientRankUserInfo
	void	LuaPushValue(lua_State* L, LC_ClientRankUserInfo* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientRankUserInfo*>, lua_State* L, int idx);
	LC_ClientRankUserInfo*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientRankUserInfo*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_ClientBattleRecordDataBase
#define H_SCRIPT_HANDLC_ClientBattleRecordDataBase
#define SC_ClientScriptInterface_SCRIPT_HANDLC_ClientBattleRecordDataBase
	void	LuaPushValue(lua_State* L, LC_ClientBattleRecordDataBase* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientBattleRecordDataBase*>, lua_State* L, int idx);
	LC_ClientBattleRecordDataBase*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientBattleRecordDataBase*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_ClientGuessDataBase
#define H_SCRIPT_HANDLC_ClientGuessDataBase
#define SC_ClientScriptInterface_SCRIPT_HANDLC_ClientGuessDataBase
	void	LuaPushValue(lua_State* L, LC_ClientGuessDataBase* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientGuessDataBase*>, lua_State* L, int idx);
	LC_ClientGuessDataBase*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientGuessDataBase*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_ClientGameEventUserData_ArenaBattle_BattleInfo
#define H_SCRIPT_HANDLC_ClientGameEventUserData_ArenaBattle_BattleInfo
#define SC_ClientScriptInterface_SCRIPT_HANDLC_ClientGameEventUserData_ArenaBattle_BattleInfo
	void	LuaPushValue(lua_State* L, LC_ClientGameEventUserData_ArenaBattle_BattleInfo* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientGameEventUserData_ArenaBattle_BattleInfo*>, lua_State* L, int idx);
	LC_ClientGameEventUserData_ArenaBattle_BattleInfo*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientGameEventUserData_ArenaBattle_BattleInfo*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_ClientGameEventUserData_ArenaSettleInfo
#define H_SCRIPT_HANDLC_ClientGameEventUserData_ArenaSettleInfo
#define SC_ClientScriptInterface_SCRIPT_HANDLC_ClientGameEventUserData_ArenaSettleInfo
	void	LuaPushValue(lua_State* L, LC_ClientGameEventUserData_ArenaSettleInfo* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientGameEventUserData_ArenaSettleInfo*>, lua_State* L, int idx);
	LC_ClientGameEventUserData_ArenaSettleInfo*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientGameEventUserData_ArenaSettleInfo*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_ClientGameEventUserData_Common
#define H_SCRIPT_HANDLC_ClientGameEventUserData_Common
#define SC_ClientScriptInterface_SCRIPT_HANDLC_ClientGameEventUserData_Common
	void	LuaPushValue(lua_State* L, LC_ClientGameEventUserData_Common* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientGameEventUserData_Common*>, lua_State* L, int idx);
	LC_ClientGameEventUserData_Common*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientGameEventUserData_Common*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_CommonStatAsset
#define H_SCRIPT_HANDGameLogicLC_CommonStatAsset
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_CommonStatAsset
	void	LuaPushValue(lua_State* L, GameLogic::LC_CommonStatAsset* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_CommonStatAsset*>, lua_State* L, int idx);
	GameLogic::LC_CommonStatAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_CommonStatAsset*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_CommonStatEntry
#define H_SCRIPT_HANDGameLogicLC_CommonStatEntry
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_CommonStatEntry
	void	LuaPushValue(lua_State* L, GameLogic::LC_CommonStatEntry* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_CommonStatEntry*>, lua_State* L, int idx);
	GameLogic::LC_CommonStatEntry*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_CommonStatEntry*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_TreasureChestsAsset
#define H_SCRIPT_HANDGameLogicLC_TreasureChestsAsset
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_TreasureChestsAsset
	void	LuaPushValue(lua_State* L, GameLogic::LC_TreasureChestsAsset* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_TreasureChestsAsset*>, lua_State* L, int idx);
	GameLogic::LC_TreasureChestsAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_TreasureChestsAsset*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDACT_SecretTreasureAsset
#define H_SCRIPT_HANDACT_SecretTreasureAsset
#define SC_ClientScriptInterface_SCRIPT_HANDACT_SecretTreasureAsset
	void	LuaPushValue(lua_State* L, ACT_SecretTreasureAsset* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<ACT_SecretTreasureAsset*>, lua_State* L, int idx);
	ACT_SecretTreasureAsset*	LuaGetValue(Utility::LuaTypeWrapper<ACT_SecretTreasureAsset*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_SkillBookAsset
#define H_SCRIPT_HANDLC_SkillBookAsset
#define SC_ClientScriptInterface_SCRIPT_HANDLC_SkillBookAsset
	void	LuaPushValue(lua_State* L, LC_SkillBookAsset* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_SkillBookAsset*>, lua_State* L, int idx);
	LC_SkillBookAsset*	LuaGetValue(Utility::LuaTypeWrapper<LC_SkillBookAsset*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_CommonStatManager
#define H_SCRIPT_HANDGameLogicLC_CommonStatManager
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_CommonStatManager
	void	LuaPushValue(lua_State* L, GameLogic::LC_CommonStatManager* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_CommonStatManager*>, lua_State* L, int idx);
	GameLogic::LC_CommonStatManager*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_CommonStatManager*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_ClientSoulRefineAsset
#define H_SCRIPT_HANDGameLogicLC_ClientSoulRefineAsset
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ClientSoulRefineAsset
	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientSoulRefineAsset* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientSoulRefineAsset*>, lua_State* L, int idx);
	GameLogic::LC_ClientSoulRefineAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientSoulRefineAsset*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_ClientShotActivityManager
#define H_SCRIPT_HANDGameLogicLC_ClientShotActivityManager
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ClientShotActivityManager
	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientShotActivityManager* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientShotActivityManager*>, lua_State* L, int idx);
	GameLogic::LC_ClientShotActivityManager*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientShotActivityManager*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_FashionBagAsset
#define H_SCRIPT_HANDGameLogicLC_FashionBagAsset
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_FashionBagAsset
	void	LuaPushValue(lua_State* L, GameLogic::LC_FashionBagAsset* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_FashionBagAsset*>, lua_State* L, int idx);
	GameLogic::LC_FashionBagAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_FashionBagAsset*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_ClientRumorManager
#define H_SCRIPT_HANDGameLogicLC_ClientRumorManager
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ClientRumorManager
	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientRumorManager* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientRumorManager*>, lua_State* L, int idx);
	GameLogic::LC_ClientRumorManager*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientRumorManager*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_ClientActivityManager
#define H_SCRIPT_HANDGameLogicLC_ClientActivityManager
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ClientActivityManager
	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientActivityManager* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientActivityManager*>, lua_State* L, int idx);
	GameLogic::LC_ClientActivityManager*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientActivityManager*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDGameLogicLC_ClientActivityAsset
#define H_SCRIPT_HANDGameLogicLC_ClientActivityAsset
#define SC_ClientScriptInterface_SCRIPT_HANDGameLogicLC_ClientActivityAsset
	void	LuaPushValue(lua_State* L, GameLogic::LC_ClientActivityAsset* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<GameLogic::LC_ClientActivityAsset*>, lua_State* L, int idx);
	GameLogic::LC_ClientActivityAsset*	LuaGetValue(Utility::LuaTypeWrapper<GameLogic::LC_ClientActivityAsset*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_AchievementManager
#define H_SCRIPT_HANDLC_AchievementManager
#define SC_ClientScriptInterface_SCRIPT_HANDLC_AchievementManager
	void	LuaPushValue(lua_State* L, LC_AchievementManager* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_AchievementManager*>, lua_State* L, int idx);
	LC_AchievementManager*	LuaGetValue(Utility::LuaTypeWrapper<LC_AchievementManager*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_ClientToyPetManager
#define H_SCRIPT_HANDLC_ClientToyPetManager
#define SC_ClientScriptInterface_SCRIPT_HANDLC_ClientToyPetManager
	void	LuaPushValue(lua_State* L, LC_ClientToyPetManager* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientToyPetManager*>, lua_State* L, int idx);
	LC_ClientToyPetManager*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientToyPetManager*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_ClientCombatRecordManager
#define H_SCRIPT_HANDLC_ClientCombatRecordManager
#define SC_ClientScriptInterface_SCRIPT_HANDLC_ClientCombatRecordManager
	void	LuaPushValue(lua_State* L, LC_ClientCombatRecordManager* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientCombatRecordManager*>, lua_State* L, int idx);
	LC_ClientCombatRecordManager*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientCombatRecordManager*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_ClientGuildProfitManager
#define H_SCRIPT_HANDLC_ClientGuildProfitManager
#define SC_ClientScriptInterface_SCRIPT_HANDLC_ClientGuildProfitManager
	void	LuaPushValue(lua_State* L, LC_ClientGuildProfitManager* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientGuildProfitManager*>, lua_State* L, int idx);
	LC_ClientGuildProfitManager*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientGuildProfitManager*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_ClientArenaPlayBackManager
#define H_SCRIPT_HANDLC_ClientArenaPlayBackManager
#define SC_ClientScriptInterface_SCRIPT_HANDLC_ClientArenaPlayBackManager
	void	LuaPushValue(lua_State* L, LC_ClientArenaPlayBackManager* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientArenaPlayBackManager*>, lua_State* L, int idx);
	LC_ClientArenaPlayBackManager*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientArenaPlayBackManager*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_ClientReportManager
#define H_SCRIPT_HANDLC_ClientReportManager
#define SC_ClientScriptInterface_SCRIPT_HANDLC_ClientReportManager
	void	LuaPushValue(lua_State* L, LC_ClientReportManager* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientReportManager*>, lua_State* L, int idx);
	LC_ClientReportManager*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientReportManager*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_ClientToyPetInstance
#define H_SCRIPT_HANDLC_ClientToyPetInstance
#define SC_ClientScriptInterface_SCRIPT_HANDLC_ClientToyPetInstance
	void	LuaPushValue(lua_State* L, LC_ClientToyPetInstance* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientToyPetInstance*>, lua_State* L, int idx);
	LC_ClientToyPetInstance*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientToyPetInstance*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_ClientCollectionManager
#define H_SCRIPT_HANDLC_ClientCollectionManager
#define SC_ClientScriptInterface_SCRIPT_HANDLC_ClientCollectionManager
	void	LuaPushValue(lua_State* L, LC_ClientCollectionManager* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientCollectionManager*>, lua_State* L, int idx);
	LC_ClientCollectionManager*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientCollectionManager*>, lua_State* L, int idx);
#endif

#ifndef H_SCRIPT_HANDLC_ClientBotManager
#define H_SCRIPT_HANDLC_ClientBotManager
#define SC_ClientScriptInterface_SCRIPT_HANDLC_ClientBotManager
	void	LuaPushValue(lua_State* L, LC_ClientBotManager* value);
	bool	LuaTypeCheck(Utility::LuaTypeWrapper<LC_ClientBotManager*>, lua_State* L, int idx);
	LC_ClientBotManager*	LuaGetValue(Utility::LuaTypeWrapper<LC_ClientBotManager*>, lua_State* L, int idx);
#endif

}
