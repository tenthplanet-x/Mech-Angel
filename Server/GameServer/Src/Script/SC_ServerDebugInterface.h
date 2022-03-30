//服务器调试脚本接口
#pragma once

#include "LC_ActorAttributeMap.h"
#include "SystemError.h"
#include "StringDefine.h"
#include <string>
#include "MG_ProtocolStruct.h"

class LC_ServerNPC;
class LC_ServerPlayer;
class SC_ServerDebugInterface : public Memory::MM_BaseObject
{
public:
	SC_ServerDebugInterface();
	virtual ~SC_ServerDebugInterface();
	DECL_RTTI_NAME( SC_ServerDebugInterface )

	//--------------------------------------------------
	void SendToDebugConsole(const StringType& strTemp);
	void LogError(const StringType& strError);
	//--------------------------------------------------

	uint32_t GetCurrentTimestamp(void);

	int GetServerID();
	uint32_t GetServerStartTime();
	int GetBackPackTypeID();

	StringType FindOwerType(object_id_type nID);

	bool SetState(int nState, int nID, const StringType& strOwerType);

	GameLogic::LC_ActorAttributeMap* GetAttributeMap(object_id_type nPlayerID);

	void NPCScript(int nNPCID);

	int GetServerPlayerNum();

	object_id_type GetPlayerID(int nIndex);

	//int GetServerEvilNPCNum();

	//object_id_type GetNPCID(int nIndex);

	int GetServerNPCNum();

	int32_t GetMapPlayerCount(int32_t iMapResID);

	object_id_type GetNPCID(int nIndex);

	int GetNPCShopFuncID(int nNPCID);

	void SetListeningFlag(int nID, bool bIsListening);

	bool SetHP(int nPlayerID, attr_value_type v);
	bool SetMP(int nPlayerID, attr_value_type v);
	bool SetSkillLevelupPoints(int nPlayerID, int v);

	bool SetOwnerMaxMP(object_id_type nPlayerID, int nValue);
	bool ModifyOwnerMaxMP(object_id_type nPlayerID, int nDeltaValue);

	bool SetOwnerMP(object_id_type nPlayerID, int nValue);
	bool ModifyOwnerMP(object_id_type nPlayerID, int nDeltaValue);

	bool SyncOwnerMP(object_id_type nPlayerID);

	bool AddSkill(int nPlayerID, int lSkillID);

	bool RemoveSkill(int nPlayerID, int lSkillID);

	bool SetCombatRecorderEnabled(int nPlayerID, int nStatus);
	bool FinishCombatRecorder(int nPlayerID);

	bool RemoveAllSkill(int nPlayerID);

	bool AddSkillBook(object_id_type nPlayerID, int32_t iSkillBookID);
	bool RemoveSkillBook(object_id_type nPlayerID, int32_t iSkillBookID);
	bool SkillBookNoteSoul(object_id_type nPlayerID, int32_t id, int32_t item1, int32_t item2, int32_t item3);
	bool SkillBookUpgrade(object_id_type nPlayerID, int32_t id, int pos);

	bool QueryUserChallenge(object_id_type nPlayerID);
	bool QueryRankUserInfo(object_id_type nPlayerID, int32_t nRankID);

	bool ActivateStarVein(object_id_type nPlayerID, int32_t nStarVeinID);

	bool GetFDRewardTime(object_id_type nPlayerID);
	bool GetFDFreeReward(object_id_type nPlayerID, int32_t nDay, int32_t nRewardID);
	bool GetFDDisCountReward(object_id_type nPlayerID, int32_t nDay, int32_t nRewardID);

	bool ActivateSub(object_id_type nPlayerID, int32_t subType);

	bool UpgradeSubordinate(object_id_type nPlayerID, int32_t subType, int32_t payIndex);

	bool BuyBuddyGrowthPackage(object_id_type nPlayerID, int32_t buddyID);
	bool GetBuddyGrowthReward(object_id_type nPlayerID, int32_t buddyID);

	bool CollectionActivation(object_id_type nPlayerID, int32_t collectActivateID);
	bool CollectionReward(object_id_type nPlayerID, int32_t collectRewardID);

	bool RuneReward(object_id_type nPlayerID, int32_t runeItemID);
	bool RefreshRewardTask(object_id_type nPlayerID);

	bool WriteCharbaseTest(object_id_type nPlayerID, int32_t size);

	bool WriteRaidDataTest(object_id_type nPlayerID, int32_t size);

	bool TurnPlayer(object_id_type nPlayerID, int16_t nForward);

	bool GainOfflineEXP(object_id_type nPlayerID, int32_t gainType);
	
	bool LeaveGame(object_id_type nPlayerID);

	bool Console(object_id_type nPlayerID, int bEnabled);

	bool ActivateSkillBook(object_id_type nPlayerID, int skillBookID);
	bool UnlockSkillBookSlot(object_id_type nPlayerID, int index, int slotID);
	bool WearSkillBook(object_id_type nPlayerID, int index, int slotID, int skillBookID);	

	bool ReliveBeforeEnterMap(object_id_type nPlayerID);

	bool TakeChapterReward(object_id_type nPlayerID, int chapterID);

	bool BuyItem(object_id_type nPlayerID, int32_t shopID, int32_t count);

	bool AddItem(int nPlayerID, int lItemID, int nItemNum);

	GameLogic::LC_ActorBase* GetActor(int nActorID);

	void	CreatePlayer(object_id_type lPlayerID,int lCharType);

	//ReloadSkillScript
	void ReloadSkillScript(object_id_type lPlayerID);

	//AddCash
	bool AddCash(object_id_type lPlayerID, MoneyType llAddCashValue);
	bool AddBindCash(object_id_type lPlayerID, MoneyType lAddCash);
	bool AddUnBindCash(object_id_type lPlayerID, MoneyType lAddCash);

	bool AddRuneCream(object_id_type lPlayerID, MoneyType value);
	bool AddStarSoul(object_id_type lPlayerID, MoneyType value);
	bool AddPentacle(object_id_type lPlayerID, MoneyType value);

	bool BroadcastMsg(object_id_type lPlayerID, int nMsgType, int nMsgLevel);

	bool AddCouragePoint(object_id_type lPlayerID, int lNum);
	bool AddJusticePoint(object_id_type lPlayerID, int lNum);

	bool AddMoneyCourage(object_id_type lPlayerID, int lNum);
	bool AddEnergy(object_id_type lPlayerID, int lNum);

	bool BuyInvestmentCard(object_id_type lPlayerID, int typeID);
	bool AddInvestmentCardReward(object_id_type lPlayerID, int id);

	bool GetStarVeinGoalReward(object_id_type lPlayerID, int goldID);

	bool OneKeyEquipRune(object_id_type lPlayerID, int buddyID);
	bool OneKeyUnEquipRune(object_id_type lPlayerID, int buddyID);

	//AddExp
	bool AddExp(object_id_type lPlayerID, int64_t lAddExpValue);
	// AddExpToActivateBuddyByID
	bool AddExpToBuddy(object_id_type lPlayerID, int nBuddyId, int64_t lAddExpValue );
	bool AddExpToActivateBuddy(object_id_type lPlayerID, int64_t lAddExpValue);
	bool AddExpToAllBuddy(object_id_type lPlayerID, int64_t lAddExpValue);

	bool SetBaseDataStr( object_id_type lPlayerID, int nIndex, const StringType& strInput );
	bool EchoBaseDataStr( object_id_type lPlayerID, int nIndex );

	bool SetBaseDataInt( object_id_type lPlayerID, int nIndex, int nValue );
	bool EchoBaseDataInt( object_id_type lPlayerID, int nIndex );

	// CombatEvent
	bool RegisterCombatEvent(object_id_type lPlayerID, int eEventType, int nKey, int nValue);
	bool UnregisterCombatEvent(object_id_type lPlayerID, int eEventType, int nKey);

	bool GMDead(object_id_type lPlayerID, int nVal);

	//AddLevel
	bool AddLevel(object_id_type lPlayerID, int lAddLevelValue);
	bool AddBuddyLevel(object_id_type lPlayerID, int lAddLevelValue, int nBuddyIndex);
	bool BuddyLevelUp(object_id_type lPlayerID, int nBuddyID);

	bool SwitchLine(object_id_type lPlayerID, int nLineIndex);

	bool GuildOfflineSolo(object_id_type lPlayerID, uint64_t nTargetCharId);

	// NewVIP Level
	bool SetNewVIPLevel(object_id_type lPlayerID, int lNewVIPLevel);

	// buddy scheme
	bool SetBuddySchemeData( object_id_type lPlayerID, int mapID, int schemeType);
	bool SwitchToBuddyScheme( object_id_type lPlayerID, int nType);
	bool ResetBuddyScheme( object_id_type lPlayerID, int nType );

	// buddy chain
	bool AddBuddyChain( object_id_type lPlayerID, int nIndex, int nBuddyId );
	bool DelBuddyChain( object_id_type lPlayerID, int nIndex);
	bool SwitchBuddyChain( object_id_type lPlayerID, int nIndex, int nBuddyId );
	bool SetBuddyChainCount( object_id_type lPlayerID, int nCount );
	bool SyncBuddyChain( object_id_type lPlayerID );

	// Data switch between player and buddyinstance
	bool BindBuddyToPlayer(object_id_type lPlayerID, int nIndex, int nBuddyID);

	bool FireTimerById( object_id_type lPlayerID, int timer_id );

	// Save Player
	bool SavePlayer(object_id_type lPlayerID, int nIsLeave);
	bool SaveBuddy(object_id_type lPlayerID, int nBuddyID,int nIsLeave);

	// AotuSwitch
	bool SwitchBuddy(object_id_type lPlayerID);

	bool UpgradeBuddyStar(object_id_type lPlayerID, int nBuddyID);

	//FinishRaid
	bool FinishRaid(object_id_type lPlayerID);

	//开始行会战
	bool StartGuildWar(int span);

	//结束行会战
	bool EndGuildWar();

	//开始跨服行会战
	bool StartCrossRealmGuildWar(int span);

	//结束跨服行会战
	bool EndCrossRealmGuildWar();

	bool ClearGuildRaid(unique_id_type lCharID);

	//设置盟主
	bool SetPowerGuild(const StringType& strOwerType);

	bool StartDomainWar(int span);
	bool EndDomainWar();

	//AddSkillPoint
	bool AddSkillPoint(object_id_type lPlayerID, attr_value_type lAddSkillPoint);

	//ChangedCareerType
	void ChangedCareerType(object_id_type lPlayerID, int nCareerType);

	//ChangedWeaponType
	void ChangedWeaponType(object_id_type lPlayerID, int nWeaponType);

	bool AssignTask(object_id_type lPlayerID, int lTaskID);

	//AcceptTask
	bool AcceptTask(object_id_type lPlayerID, int lTaskID);
	
	//FinishTask
	bool FinishTask(object_id_type lPlayerID, int lTaskID);

	//CancelTask
	bool CancelTask(object_id_type lPlayerID, int lTaskID);

	//DeleteTask
	bool DeleteTask(object_id_type lPlayerID, int lTaskID);

	bool JumpTask(object_id_type lPlayerID, int lTaskID);

	bool SetBuddyRuneSlotState(object_id_type lPlayerID);

	//SpawnNPC
	bool SpawnNPC(object_id_type lPlayerID,int lNPCType);
	bool SpawnNPCByCount(object_id_type lPlayerID,int lNPCType, int nCount);
	bool SpawnNPCDir(object_id_type lPlayerID, int lNPCType, int dir);
	bool SpawnNPCByLocation(int lNPCType,int iMapID,int x,int y,int z,int iFactionType);
	bool SpawnAutoFitNPC(object_id_type lPlayerID,int lNPCType, int iLevel);
	bool SpawnGuildNPC(object_id_type lPlayerID, int lNPCType, uint64_t iGuildId);
	bool SpawnServerNPC(object_id_type lPlayerID, int lNPCType, int iServerId);

	bool TransferToTarget(object_id_type lPlayerID, int eTargetType);
	// Buddy system
	bool SetActivateBuddy(object_id_type lPlayerID, int nIndex);
	bool ReliveBuddy(object_id_type lPlayerID, int nIndex);
	bool BuyBuddy(object_id_type lPlayerID, int nTemplateId, int nForce);

	// Bott system
	bool AddBot(object_id_type lPlayerID, int nBotTemplateId);
	bool UpgradeBot(object_id_type lPlayerID, int nBotTemplateId);
	bool SyncBot(object_id_type lPlayerID);
	bool BotBonus(object_id_type lPlayerID);
	bool BotBonusSimple(object_id_type lPlayerID, int nType, int nSeconds);
	bool BotBonusNormal(object_id_type lPlayerID, int nCount);
	bool BotBonusTalent(object_id_type lPlayerID, int nCount);
	// Pet system
	bool AddPet(object_id_type lPlayerID, int nPetTemplateId);
	bool EnablePet(object_id_type lPlayerID, int nPetIndex);
	bool DisablePet(object_id_type lPlayerID);
	bool DecomposePet(object_id_type lPlayerID, int nPetIndex);
	bool UpgradePetStarLevel(object_id_type lPlayerID, int nPetIndex);
	bool SyncPet(object_id_type lPlayerID);
	bool AddDrop(object_id_type lPlayerID, int32_t nDropID);

	bool SubScore(object_id_type lPlayerID, int32_t nSubType);

	bool ReleaseAllUIOpenLevel(object_id_type lPlayerID);
	bool ClearChallengeCD(object_id_type lPlayerID);
	// Skill Debug
	bool SetDenyDead(object_id_type lPlayerID, int bDenyDead);
	bool SetDenyBuddyDead(object_id_type lPlayerID, int nIndex, int bDenyDead);
	bool CastSkill( object_id_type lPlayerID, int32_t nSkillId );
	bool CastBuddySkillBySkillIndex(object_id_type lPlayerID, int32_t buddyIndex, int32_t nSkillIndex);

	// 全服限量活动相关测试
	bool ActBegin( object_id_type lPlayerID, int nActID );
	bool ActGet( object_id_type lPlayerID, int nActID );
	bool ActSet( object_id_type lPlayerID, int nActID, int nValue );

	// 全服限量活动相关测试
	bool RedisBeginData(object_id_type lPlayerID, const StringType& strNameSpace, int nKey1, int nKey2, int nKey3);
	bool RedisGetDataAsString(object_id_type lPlayerID, const StringType& strNameSpace, int nKey1, int nKey2, int nKey3);
	bool RedisSetDataAsString(object_id_type lPlayerID, const StringType& strNameSpace, int nKey1, int nKey2, int nKey3, const StringType& strInput);
	// redis rank test
	bool RedisReadElement(object_id_type lPlayerID, const StringType& strNameSpace, uint64_t nKey);
	bool RedisWriteElement(object_id_type lPlayerID, const StringType& strNameSpace, uint64_t nKey, uint32_t nScore, uint32_t nParam);
	bool RedisReadRange(object_id_type lPlayerID, const StringType& strNameSpace, int32_t nTopCount);
	bool RedisCreateSnapshot(object_id_type lPlayerID, const StringType& strNameSpace);

	bool BuddyDebug( object_id_type lPlayerID, int nMode );

	bool BuddyDebugMode( object_id_type lPlayerID,  int nIndex, int nMode );

	bool ActivateSkill( object_id_type lPlayerID, int32_t nBuddyId, int32_t nSkillId );

	bool ModifySkillUsableCount( object_id_type lPlayerID, int32_t nBuddyId, int32_t nSkillId, int32_t nUsableCount );
	bool GetSkillUsableCount( object_id_type lPlayerID, int32_t nBuddyId, int32_t nSkillId );
	bool ModifySkillMaxUsableCount( object_id_type lPlayerID, int32_t nBuddyId, int32_t nSkillId, int32_t nUsableCount );
	bool GetSkillMaxUsableCount( object_id_type lPlayerID, int32_t nBuddyId, int32_t nSkillId );

	//装备槽强化
	bool EquipSlotReinforce(object_id_type lPlayerID, int buddyId, int equipSlotIndex);
	//装备槽升星
	bool EquipSlotStar(object_id_type lPlayerID, int buddyId, int32_t equipIndex);

	//宝具
	bool RuneUpgrade(object_id_type lPlayerID, int32_t buddyIndex, int32_t slotIndex, int32_t targetLevel);
	bool RuneStar(object_id_type lPlayerID, int32_t slotType, int32_t runeIndex);
	bool	ResetRune(object_id_type lPlayerID, int32_t buddyID, int32_t runeIndex);
	bool EquipRune(object_id_type lPlayerID, int32_t buddyID, int32_t srcSlotIndex, int32_t descSlotIndex);
	bool UnEquipRune(object_id_type lPlayerID, int buddyID, int32_t slotIndex);

	bool BiographyPartReward(object_id_type lPlayerID, int32_t biographyID);

	bool Charge(object_id_type lPlayerID, int RMB);

	bool GetShopItem(object_id_type lPlayerID, int shopType);

	bool MainTaskReachReward(object_id_type lPlayerID, int reachRewardID);

	bool ResetShopTimes(object_id_type lPlayerID, int shopType);
	bool GetShopResetTimes(object_id_type lPlayerID, int shopType);

	bool DecompositionItem(object_id_type lPlayerID, int index1, int index2, int index3, int index4, int index5, int index6);
	
	bool SetDecompositionSetting(object_id_type lPlayerID, int level);
	
	bool RecommendFriend(object_id_type IPlayerID);

	void ExternWarehouse(object_id_type lPlayerID, int32_t rowNum);

	bool GetBuyMapStepInfo(object_id_type IPlayerID, uint32_t storyID);

	bool BuyVIPTimes(object_id_type IPlayerID, uint32_t storyID, uint32_t buyType, uint32_t buyTimes);
	bool QuickRaidAdvance(object_id_type IPlayerID, uint32_t mapID);

	void GuildList(object_id_type IPlayerID, int index, int sortType, int sortCol, int autoAdd, int searchNum);

	//进入副本
	bool EnterTranscript(object_id_type lPlayerID, int transferType, int resId, int index, int serverId, int tribuMap);

	//设置掉落概率系数
	void SetFlopProbability(float fProb);

	//坐骑系统
	bool SummonVehicle(object_id_type lPlayerID);

	//解散坐骑
	bool DismissVehicle(object_id_type lPlayerID);

	bool AddSystemTilte(object_id_type lPlayerID, int lSytemTitleID);
	void RemoveSystemTitle(object_id_type lPlayerID, int lSystemTitleID);
	void SetSystemTitle(object_id_type lPlayerID, int lSystemTitleID);
	void GetAchievement(unique_id_type charID, StringType sAchievementIDs);
	void AddAchievement(object_id_type lPlayerID, int lAchievementID);
	void RemoveAchievement(object_id_type lPlayerID, int lAchievementID);

	//踢人
	void KickPlayer(unique_id_type lCharID);

	// 增加天赋点
	void AddVigor(object_id_type lPlayerID, int lPoint);

	//传送
	void SetCurrentLocation(object_id_type lPlayerID,int iMapLogicID,int lX,int lY);

	//高级传送
	void ChangeCurrentLocation(object_id_type lPlayerID,int iMapInOutType,int iMapLogicType,int iMapLogicID,int iMapResID,int iX,int iY);

	//设置交互值
	void SetInteractionValue(object_id_type lNPCID,int lValue);

	//竞技场
	void SetArenaExp(object_id_type lPlayerID, int lEXP);
	void SetArenaScore(object_id_type lPlayerID, int lScore);
	void SetArenaTotalData(object_id_type lPlayerID, int lWinnum, int lLossNum, int lDrawNum);
	void SetArenaQuarterData(object_id_type lPlayerID, int lWinnum, int lLossNum, int lDrawNum);

	//pk
	bool AddPKValue( object_id_type lPlayerID, int lPKValue );

	//添加耐久度
	void SetEquipEndurance(object_id_type lPlayerID, int EquipSlotIndex, int iEndurance);

	//添加快捷
	void AddShortcut(object_id_type lPlayerID, int lShortcutTypeID);

	//缴械目标
	void DisarmTarget(object_id_type lPlayerID, int iDurationTime);

	//消耗指定位置的物品
	void ConsumeItemByIndex(object_id_type lPlayerID,int iPackType,int iPackIndex,int iCount);

	//消耗指定类型的物品
	void ConsumeItemByID(object_id_type lPlayerID,int lItemTypeID,int iCount);

	//增加弩炮能量
	void SetBow(object_id_type lPlayerID, int nVal);
// 
// 	void AddUnbindYuanbao(object_id_type lPlayerID, int lYuanbao);

	StringType GetVIPInfo(object_id_type lPlayerID);


	BOOL SendToPlayerConsole(object_id_type playerID, const StringType& msg);

	StringType DrawVIPGift(object_id_type playerID, int type);

	void SignIn(object_id_type playerID, int32_t iCnt);

	void ScheduleGameStory(int32_t iStoryID, const StringType& cronStr);
	void EndGameStory(int32_t iStoryID);
	void SetGameStoryRealDay(int32_t iStoryID, int32_t iDay);

	void GMLogin(object_id_type playerID, const StringType& usr, const StringType& pwd);
	void GMCharLists();
	void SetGmLevel(unique_id_type charID, const StringType& name, uint32_t uiPriority);

	void OpenAbility(object_id_type playerID, int32_t iAbilityID);
	bool OpenPetAbility(object_id_type playerID, int32_t iAbilityID, int32_t petID);

	LC_ServerPlayer*	FindPlayerByName(const StringType& rkPlayerName);
	LC_ServerPlayer*	FindPlayerByID(object_id_type lPlayerID);
	void OpenAllMaps(object_id_type playerID, bool bOpen);
	void GetServerInfo(object_id_type playerID);
	void Announceinfo(const StringType& msg);
	void AnnounceinfoUTF8(const StringType& msg);
	
	void ChangeVehicle(object_id_type playerID , int iLevel, int lCharTypeID);
	void SetSubLevel(object_id_type playerID , int iSubType, int iSubLevel);
	void ClearPack(object_id_type playerID, int type);

	void SetCrossRealmServer(int serverid);
	void CrossRaidServerList(object_id_type playerID);

	void GetServerConnectTime(object_id_type playerID);

	void Closeserver(object_id_type playerID,int32_t serverid);

	void ExportAllRobot(object_id_type playerID);

	void ExportSoloDuleRobot(object_id_type playerID,int nType, int robotType);
	void WheelFightReq(uint64_t uChallengeId,uint64_t uBeChallengeId);
	bool GetNearbyPlayer(object_id_type playerID);

	void RefreshSkillCD(object_id_type playerID, int iPage, int iSlot);
	void RefreshSkillIDCD(object_id_type playerID, int iSkillID);
	bool SetSkillCD(object_id_type playerID, int32_t nSkillID, int32_t nCDTimeInMs);

	void ForbidTalking(unique_id_type charID, int itime);
	void AddLoginDays(object_id_type playerID, int idays);

	void SetGMCommandResult(ResultType rslt,const StringType& reltmsg, const StringType& datamsg);
	void GetOnlineUser();
	void GetOnlineUserCount();
	void GetRoleInfo(object_id_type playerID);
	void SendMailToOnline(const StringType& subject,const StringType& content,const StringType& msg);

	void SendMailToOneFunc(unique_id_type charID,const StringType& subject,const StringType& content, const StringType& msg, bool bGb2312);
	void SendMailToOne(unique_id_type charID,const StringType& subject,const StringType& content, const StringType& msg);	//编码gb2312
	void SendMailToOneUtf(unique_id_type charID,const StringType& subject,const StringType& content, const StringType& msg);//编码utf-8

	void SendMailToAllFunc(const StringType& subject,const StringType& content,const StringType& msg);
	void SendMailToAll(const StringType& subject,const StringType& content,const StringType& msg);							//编码gb2312
	
	void RechargeMailToOneUtf(unique_id_type charID, const StringType& subject, const StringType& content, const StringType& iteminfo, const StringType& token);

	void AddItemToAllUtf(const StringType& iteminfo, const StringType& subject, const StringType& content, const StringType& token);	
	void AddItemToOneUtf(unique_id_type charID, const StringType& iteminfo, const StringType& subject, const StringType& content, const StringType& token);

	void SendMailToOneMoney(unique_id_type charID,const StringType& subject,const StringType& content, const StringType& msg, const StringType& order);		//编码gb2312
	void GetItemList();
	void GetConfigList(const StringType& type);
	void GetMatchIPCharList(const char * charip);
	void GetRoleBaseInfo(object_id_type playerID);
	void GetRoleBaseInfoByUID(unique_id_type charID);
	void GetRoleVehicleInfo(object_id_type playerID);
	void GetRoleVehicleInfoByUID(unique_id_type charID);
	void GetRoleArrowInfo(object_id_type playerID);
	void GetRoleArrowInfoByUID(unique_id_type charID);
	void GetRoleDevaeyeInfo(object_id_type playerID);
	void GetRoleDevaeyeInfoByUID(unique_id_type charID);
	void GetRoleVehicleFightingInfo(object_id_type playerID);
	void GetRoleVehicleFightingInfoByUID(unique_id_type charID);
	void GetRoleSectSkillInfo(object_id_type playerID);
	void GetRoleSectSkillInfoByUID(unique_id_type charID);
	void GetRoleMentalSkillInfo(object_id_type playerID);
	void GetRoleMentalSkillInfoByUID(unique_id_type charID);
	void GetRoleInfolist();
	void GetRoleInfoBytype(unique_id_type charID,const StringType& type);
	void GetTaskInfolistBytype(int type);
	void SetFCM(object_id_type playerID,int type);

	void GetUnionRegUserCount();
	void GetUnionOnLineUserCount();
	void GetUnionServerRecharge();
	void GetUnionUserCountByKey(const std::string& key);
	void ResetPlayerGuildRaid(object_id_type playerID);

	void SendGM2Server(object_id_type playerID, int serverType, int gmType, const StringType& strInfo);
	void SendGM2TransServer(object_id_type playerID, int serverType, int gmType, const StringType& strInfo);
	void SetInternal(object_id_type playerID, int nStatus);
	void SetFreezeAcc(object_id_type userID, uint32_t nFreeze);
	void SetFreezeRol(object_id_type userID, unique_id_type charID, uint32_t nFreeze);
	void GetItemListByPage(int nPage, int nItemID, int nCount);

	void ReloadCSV(const StringType& rkCSVFile, const StringType& sVersion);
	void ReloadStoryScript(const StringType& rkScriptFile, const StringType& sVersion);

	void PlatformInfoDirty(unique_id_type charID);
	void SetServerPlatformInfo(object_id_type playerID, const StringType& platform);
	void PlatformClientMsg(unique_id_type charID, const StringType& pt, const StringType& type, const StringType& content);
	void PlatformTest(object_id_type playerID, const StringType& platform, const StringType& platformVip, int is_vip, int is_year_vip, int vip_level, int is_high_vip, int is_expand_vip);
	void SetLoginPlatformInfo(object_id_type playerID, const StringType& platform);

	void DetachShield(object_id_type playerID, int stateID, object_id_type targetID );

	void AddForbidWord(const StringType& wordlist);
	void DelForbidWord(const StringType& wordlist);
	void ShowForbidWordList(object_id_type playerID);
	void ShowChatMonitorInfo(object_id_type playerID, unique_id_type targetCharID);
	void SetChatMonitorLevel(unique_id_type charID, int level);
	void SetSilentGagTime(unique_id_type charID, int time);

	void GetGuildList(int32_t page);
	void DismissGuild(uint64_t guildid);
	void RandomGuildName(uint64_t iGuildID);

	void ActivateFashion(object_id_type playerID, uint32_t iFashionID);
	void DeactivateFashion(object_id_type playerID, uint32_t iFashionID);

	void AddBonusRole(unique_id_type charID);
	void RemoveBonusRole(unique_id_type charID);
	void ResetBonus();

	void SendItemToRole(unique_id_type charID, uint32_t itemid, int itemnum, int bind, const StringType& sTitle, const StringType& sContent, const StringType& sSource, const StringType& sSerial);

	void Act360PriviTest(object_id_type playerID, const StringType& sUserName, int32_t index, int32_t dailygift);
	void Set360PriviGMTest(int32_t flag);

	void GainDefenderGift(unique_id_type charID, const StringType& platform);

	void ClearAreaKeyMonster(int32_t iStoryID, int32_t iKey, int32_t iAreaIndex);

	void DumpScore(object_id_type playerID);

	void DumpBuddyScore(object_id_type playerID, int32_t nBuddyId);

	void GetResourceAlertConfig();
	void UpdateResourceAlertConfig(const StringType& sConfig);

	void SetPreActive(object_id_type playerID, int32_t active);

	void LoadUpdateBulletin();

	void SendFBReward(unique_id_type charID, int32_t iType, int32_t iID);
	
	void DebugCommand(object_id_type playerID, const StringType& command, const StringType& params);

	void HandleAsset(unique_id_type charID, bool bSetResult);

	void BoneSoulTrain(object_id_type lPlayerID, int32_t iType);

	void AddMonopolyCount(object_id_type lPlayerID, int32_t iCount);

	bool SetRankTransferFailed(object_id_type lPlayerID, int32_t mapResID);

	bool AddRankRedisData(object_id_type lPlayerID, int32_t storyID, int32_t value);
	bool ClearRankRedisData(object_id_type lPlayerID, int32_t storyID);
	bool GetRankRedisData(object_id_type lPlayerID, int32_t storyID);

	bool TransferMap(object_id_type lPlayerID, int32_t mapResID);

	bool ReqPlayerDetail(object_id_type lPlayerID, unique_id_type findPlayerID);

	bool CommitAnswer(object_id_type lPlayerID, int topicIndex, int choice);

	bool GetAnswerRankInfo(object_id_type lPlayerID, int32_t minIndex, int maxIndex);

	bool GetRankSubBInfo(object_id_type lPlayerID, int type, int param, int minIndex, int maxIndex);

	bool UpgradeCommanderTitle(object_id_type lPlayerID);

	bool GetYiZhongQinXiDamageData(object_id_type lPlayerID, int storyID, uint64_t charID);
	bool GetYiZhongQinXiDetailDamageData(object_id_type lPlayerID, int storyID, uint32_t time, uint64_t charID);

	bool GetChapterRaidStarReward(object_id_type playerID, int32_t chapterID, int32_t rewardIndex);
	bool QuickRaidNew(object_id_type playerID, int32_t mapid, int32_t raidTimes);
	bool TestEquipGradeLevel(object_id_type playerID, int32_t nBuddyId);

	bool ControllerEquipSlotReinforce(object_id_type playerID, int32_t slotIndex);
	bool ControllerEquipSlotRefine(object_id_type playerID, int32_t slotIndex);

	bool EquipItemInPackage(object_id_type playerID, int32_t srcType, int32_t srcIndex, int32_t destType, int32_t destIndex, int32_t itemID);
	bool UnEquipItemInEquipSlot(object_id_type playerID, int32_t srcType, int32_t srcIndex, int32_t itemID);
	
	bool GetOtherOfflineInfo(object_id_type playerID, uint64_t uCharID);
	bool TestGuildFight(object_id_type playerID,uint8_t uType, int32_t nStoryId,uint32_t uPoint);
	bool TestRobScore(object_id_type playerID,uint8_t uType,int nWonNum);
	bool TestGuildMemberPointInfo(object_id_type playerID );

	bool TestCombatScore(object_id_type playerID, uint64_t buddyID, int32_t type, const StringType& value);
	bool AddAuction(object_id_type playerID, int32_t count);
	bool SetSkyArenaScore(object_id_type lPlayerID, int nCount);
protected:
	LC_ServerPlayer*	_findPlayer(object_id_type lPlayerID);
	LC_ServerPlayer*	_findPlayerByUID(unique_id_type lCharID);
	void				_kickPlayer(unique_id_type lCharID);
	void				_sendGM2Server(object_id_type playerID, int serverType, Protocol::PS_ServerGMInfo& reqInfo, const StringType& strInfo);
protected:
	static bool m_gsGMDead;
};
