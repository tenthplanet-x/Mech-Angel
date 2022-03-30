//客户端专用脚本接口
#ifndef _SC_CLIENT_SKILL_INTERFACE_H
#define _SC_CLIENT_SKILL_INTERFACE_H

#include "SC_SkillInterface.h"
#include "LC_ClientItem.h"
#include "LC_ClientGamePlayer.h"
#include "LC_ClientNPC.h"
#include "LC_ClientPlaceHolder.h"
#include "LC_ClientNetPlayer.h"
#include "LC_ClientComposition.h"
#include "LC_ClientTask.h"
#include "LC_ClientMapTransDataEntry.h"
#include "LC_ClientLogicManager.h"
#include "MG_MessageHandlerInterface.h"
#include "WT_Widget.h"
#include "CF_Buddy.h"
#include "CF_EquipCommanderSlotReinforce.h"

namespace Utility
{
	struct UT_PickResult;
}
struct ShotBulletArg;
class SC_ClientSkillInterface : public Script::SC_SkillInterface,
								public MG_MessageHandlerInterface
{
public:
	SC_ClientSkillInterface();
	virtual ~SC_ClientSkillInterface();
	DECL_RTTI_NAME( SC_ClientSkillInterface )

	struct SortItemEffect
	{  
		int lSortID;
		int lIndex;
		Protocol::EquipAttrib kEquipAttrib;
		int lVipLevel;
		StringType sOperatinoInfo;
		int iCustom;
	}; 

	/*-----------------		Begin		UI  ---------------------------------*/

	//获取当前游戏时间
	float								GetGameTime();
	float								GetGameDeltaTime();
	double								GetProfileTimeStamp();
	StringType							GetCalendarToday(void);
	StringType							GetTimeStamp(time_type iBeginTime, time_type iEndTime);
	StringType							GetTimeString(time_type iBeginTime, int iType = 1);
	StringType							GetCurrentDayTime();
	int									GetCurrentSystemTime(){ return GET_SYSTEM_TIME;};
	int64_t								GetActivityScheduleDeltaTimeStamp(int iScheduleID);
	int									GetActivityScheduleStat(int iScheduleID);// -1 开启前 0 开启中 1 开启后

	//获取登录方式
	int									GetLoginType();

	//获取游戏状态
	int									GetGameStage();

	//获取当前玩家
	LC_ClientGamePlayer*				GetPlayerPtr();

	LC_ClientTaskManager*				GetTaskMgr();

	LC_ClientNetPlayer*					GetNetPlayerPtr(unique_id_type lCharID);
	LC_ClientNetPlayer*					GetNetPlayerPtrByPlayerID(object_id_type lPlayerID);
	LC_ClientNPC*						GetNPCPtr(object_id_type lNPCID);
	LC_ClientPlaceHolder*				GetChestPtr(object_id_type lPlaceHolderID);
	LC_ClientTask*						GetTaskPtr(int lTaskID);
	int									GetNeighborPlayerCount();
	int									GetTrueNeighborPlayerCount();//拿到真正的玩家层数量
	LC_ClientNetPlayer*					GetNeighborPlayerPtrByIndex(int iIndex);
	LC_ClientNPC*						FindNPCPtrInCircleByCharType(LC_Attacker* pkCaller,int lCharType,int iRadius);
	bool								FindNPCbylID(object_id_type lID);
	LC_NPCBase*							FindNPCByCharType(object_id_type lCharType);
	int									FindNPCByNumberCharType(object_id_type lCharType);
	int									PathtoTreasure(object_id_type lCharType);
	int									PathtoTreasureToy(object_id_type id);
	int									GetTaskBuddyExpBonus(int iTaskID, int iCommanderLevel);
	int									GetTaskCommanderExpBonus(int iTaskID, int iCommanderLevel);
	int									GetTaskGoldBonus(int iTaskID, int iCommanderLevel);
	int									GetTaskGuildPointBonus(int iTaskID, int iCommanderLevel);
	void								SetPostTransferLocation(GameLogic::LC_Attacker* pkScriptCaller);



	//任务章节
	int		GetTaskCurChapter();
	StringType GetAllChapterInfo();
	StringType GetAllTaskReachInfo();
	StringType GetTaskReachName(int iTaskCount);
	StringType GetChapterTaskInfo(int iChapterID);
	StringType GetAllTaskRankInfo();
	StringType GetTaskRankInfo(int iRank);
	int GetCurTaskRank();
	int64_t GetTaskChapterRewardEndTime();
	StringType GetReachTaskChapterInfo();
	StringType GetTaskReward(int iTaskId);

	//竞技场里要从NetPlayerList中查找NetPlayer
	int									GetNetPlayerCount();
	LC_ClientNetPlayer*					GetNetPlayerByIndex(int iIndex);

	//获取NPC
	int									GetNPCCount();
	LC_ClientNPC*						GetNPCByIndex(int iIndex);

	//玩家回城
	virtual void						ReturnToCity(GameLogic::LC_Attacker* pkScriptCaller);

	//获取当前观察的目标
	object_id_type								GetWatchedTargetID();

	//获取当前锁定的目标(只是鼠标选中的）
	object_id_type								GetPreLockedTargetID();

	//设置当前锁定目标							
	void										SetLockedTargetID(object_id_type lID);

	//禁止切换目标
	void										FreezeLockTargetID(bool flag);
	bool										IsFreezeLockTargetID();

	//清除玩家最近攻击者列表
	void										CleanChangeLockedFlag();

	//获取角色列表中的图标
	bool								GetCharShowRank( int lCharTypeID );
	bool								GetIsWildBoss( int lCharTypeID );
	int									GetElementDamageType( int lCharTypeID );

	//获取技能图标
	StringType							GetSkillIconPath(int lSkillID);
	//获取技能描述
	StringType							GetSkillUseResult(int lSkillID);

	//获取升级需要的经验
	int64_t								GetLevelExp(int lLevel);
	int64_t								GetBuddyLevelExp(int lLevel);

	//获取换服所需金钱
	int								GetChangeServerCash(int lLevel);

	//获取人物当前级别基础属性
	int									GetAttributeBase(int lLevel, int iAttributeType);
	double								GetAttributeRateBase(int lLevel, int iAttributeType);
	double								GetAttributeRate(LC_ActorBase* pkActor, int iAttributeType);
	double								GetAttributeRateUpgrade(LC_ActorBase* pkActor, int iAttributeType);
	double								GetNetAttributeRate(int lPlayerLevel, int lAttribute, int iAttributeType);
	double								GetNetAttributeRateUpgrade(int lPlayerLevel, int lAttribute, int iAttributeType);

	UT_Vec4Int							GetLoadingPercent();


    void                                ResetDownloadInfo();
	StringType                          GetDownloadInfo();
	float								GetDownloadPercent(const StringType &sKey);

	//获得指定ID活动所需要的金钱
	int									GetYuanbaoOriginalPrice(int iShopID);
	//获得指定ID活动所需要的金钱
	int									GetYuanbaoCurPrice(int iShopID);
	//获得价格不定的商店ID下次购买需要的货币类型和数量
	StringType							GetAccessWayBuyPrice(int iShopID);
	//根据shopId获取对应的物品id
	int									GetItemidInShopList(int iShopID);
	//根据shopId获取对应的物品id和count
	StringType							GetItemInShopList(int iShopID);

	//获得快捷栏
	GameLogic::LC_ShortcutBar*		   GetShortcutBarPtr();

	//获得NPC身上的商店
	GameLogic::LC_ShopBase*			   GetNPCShopPtr(int lNpcCharTypeID,int iShopType);

	//获得NPC身上的驿站
	GameLogic::LC_TransferBase*		   GetNPCTransferPtr(object_id_type lNPCID);
	
	//获得技能书
	GameLogic::LC_SkillCoolDownMap*		GetSkillCoolDownMap();

	//获得物品CD表
	GameLogic::LC_ItemCoolDownMapBase*	GetItemCoolDownMap();

	StringType						GetNpcConversationHeadIcon(int lConversationID, int iCurSeq);

	//获得区域名字
	StringType						GetLogicAreaName(int lMapID, int lAreaID);
	UT_Vec3Int&						GetLogicAreaSpawnPos(int lMapID, int lAreaID);

	int							   GetItemID(const StringType& kItemName);
	int							   GetItemCategoryID(int lItemID);
	
	int							   GetDakTransferAreaNum();
	int							   GetTransferMapID(int iIndex);
	int							   GetTransferAreaID(int iIndex);
	ResultType						   CheckCanUseItem(int lItemID, int iParam);

	//超链接相关
	int							   GetLinkInfoID(const StringType& strLinkKeyWord);
	int							   GetLinkInfoIDByLogicID(object_id_type lLogicID);
	int							   GetLinkType(int iLinkInfoID);
	int							   GetLinkObjectTypeID(int iLinkInfoID);
	int							   GetLinkMapResID(int iLinkInfoID);
	StringType					   GetLinkKeyWord(int iLinkInfoID);
	int							   GetLinkLevelInfo(int iLinkInfoID);
	UT_Vec3Int&					   GetLinkPosition(int iLinkInfoID);
	bool						   IsTravelBoots(int iLinkInfoID);
	bool						   IsNoGainJudge(int iLinkInfoID);
	StringType					   GetTransferPosition(int iMapResID);
	StringType						   GetLocationTip(object_id_type lObjID);
	void							   PathToObject(object_id_type lObjID, int iTaskID=0);
	void							PathToLocation(int iMapResID, int iX, int iY, int iZ, int tarMapLogicID = 0);
	bool							WalkToObject(int curMapResID, int tarMapResID, object_id_type lObjID);
	bool							WalkToLocation(int curMapResID, int tarMapResID, int _x, int _y, int _z, int tarMapLogicID = 0);
	StringType						GetPlayerLocation(void);
	bool							IsSameMapGroup(int iMapResID1, int iMapResID2);

	object_id_type					GetNPCID(int lCharTypeID);
	StringType						GetNPCName(int lCharTypeID);
	StringType						GetNPCNameColor(int lCharTypeID);
	StringType						GetNPCHeadIcon(int lCharTypeID);
	int								GetNPClElite(int lCharTypeID);
	int64_t							GetNPCMaxHp(int lCharTypeID);
	/*
	*登录流程
	*/	

	void							OnGotoUserLogin();									//进入登录界面
	void							OnGotoUrl(const StringType& rkUrl);					//前往网站
	void							OnshowWebWindow(const StringType& rkUrl, int width, int height);
	void							ShowShunWangWebWindow(const char* rkUrl, const char* rkJumpUrl, int width, int height);
	void							OnHideWebWindow();
	void							OnResetWebWindow(int width, int height, int x, int y);

	bool							ScriptJzCmdlineHasValue(const StringType& argument);
	StringType						ScriptJzCmdlineGetValue(const StringType& argument);
	
	//输入用户名密码界面
	void							OnCheckForceLogin();
	void							OnUserLogin(const StringType& strUser, const StringType& strPwd, bool bForceLogin, bool bRemember);//登录LoginServer
	//选择服务器界面
	void							OnGotoSelServer();
	void							SelectLoginZone(int iServerID);
	void							OnGotoSelChar();									//进入角色选择界面
	void							OnQuitLogin();										//退出登录LoginServer
	int								GetLastLoginServerID();								//上次登录的服务器ID
	void							SaveLastLoginServerID(int iServerID);		//保持上次登录的服务器ID
	unique_id_type					GetLastLoginCharID();
	void							SaveLastLoginCharID(unique_id_type iCharID);
	void							SaveAutoCommitTeamRequest(int iAuto);				//保存是否自动接受组队申请
	int								GetAutoCommitTeamRequest(void);						//是否自动接受组队申请
	void							SaveAutoCommitTeamInvite(int iAuto);				//保存是否自动接受组队邀请
	int								GetAutoCommitTeamInvite(void);						//是否自动接受组队邀请
	void							SaveShowBattleBuddyEquip(int m_iShow);				//保存是否勾选仅显示上阵英雄得装备红点
	int								GetShowBattleBuddyEquip(void);						//是否勾选仅显示上阵英雄得装备红点
	void							SaveShowBattleBuddyRune(int m_iShow);				//保存是否勾选仅显示穿戴得宝具红点
	int								GetShowBattleBuddyRune(void);						//是否勾选仅显示穿戴得宝具红点
	
	//选择角色界面
	void							OnGotoCreateChar(void);								//进入角色创建界面
	void							OnEnterGame(unique_id_type iPlayerID);				//进入游戏界面
	void							OnLoadingToGameStage();
	int								GetPreStateType();
	bool							PlayVideo(int iVideoType, const StringType& kVideoName);		//播放视频
	void							EndVideo(int iVideoType);										//终止视频播放
	//创建角色界面
	bool							OnCreateChar(										//创建新角色
										const StringType& strCharName, int iCid, 
										int lHeadEquipID, int lBodyEquipID, int lFootEquipID, 
										int lWeaponID, int iWeaponType, int lServerID);
	int								GetCreateCharServerID();

	//相位副本战力不足确认
	void							OnLowScoreConfirm(combat_score_type iCurSocre, combat_score_type iNeedScore, int iParam);

	StringType						GetNamePwdFromConfig();								//从配置文件中获取用户名和密码
	void							SaveNamePwdToConfig(const StringType& kName, const StringType& kRemember);//把用户名保存到配置文件中
	StringType						GetInfoFromConfig(const StringType& strTite, const StringType& strKey);	//从配置文件中获取信息
	void							SaveInfoToConfig(const StringType& strTite, const StringType& strKey, const StringType& kValue);//保存到配置文件中

	bool							GetSuperShopCustomizeTodayPrompt(const StringType& strTitle);//开服争霸榜今日结算红点是否已记录

	StringType						GetLoginServerIDList(int iZoneID);			//获取LoginServer列表
	StringType						GetLoginZoneIDList();
	StringType						GetLoginServerInfo(int iZoneID, int iIndex);					//获取LoginServer信息
	StringType						GetLoginZoneInfo(int iZoneID);
	
	int								GetGameServerCount();								//获取GameServer数量
	StringType						GetGameServerList(bool bIncludeOffServer);			//获取GameServer列表
	StringType						GetGameServerInfo(int iServerID);					//获取GameServer信息
	StringType						GetGameServerName(int iServerID);
	int								GetCurGameServer();									//获取当前选中的GameServer信息
	int								GetCurGameServerNode();

	int								GetUserCharCount();									//获取每个用户名下角色的数量
	StringType						GetCurCharList();									//获取当前所有角色列表
	StringType						GetCharInfo(unique_id_type iCharID);							//获取指定角色的信息
	StringType						GetParamForCreateChar();						//获取创建角色需要的信息

	void							RotateChar( int lCharType,int iAvatarIndex,float fAngle);//旋转角色，传入参数为角度（不是弧度）
	void							RotateAvatarModel(long lCharType, int iIndex, int iCameraMode, float fRotAngle);
	void							SetModelRotation(long lCharType, int iIndex, int iCameraMode, float fRotAngleX, float fRotAngleY, float fRotAngleZ);
	void							SetShaderConstF(const StringType& strCharName, float v);
	float							GetShaderConstF(const StringType& strCharName);
	bool							UpdateAvatarBloomParams(int lCharType);

	void							DeteachAllAttachFxAnim(GameLogic::LC_ActorBase* pkActor);

	void							SendReqServerListMsg();								//发送请求服务器列表消息
	bool							WorldMapLoaded();									//判断当前3d场景是否已经加载完成

	void							ConfirmAction(int iActionType, int lParam);		//提示用户确认动作后执行相关动作

	void							StartCountDown(bool bLeaveGame);
	void							CancleCountDown();
	void							RequestLeaveGame();				//退出游戏
	void							RequestResetGame();									//退出游戏到选人界面
	void							RequestReLoginGame();								//退出游戏到登录界面
	void							ResetUINameArrage();								//清理场景掉落物

	void							RequestRandomPlayerName(int iSex);					//请求随机名字 性别(0: 女 1: 男)

	//网络状态
	int								GetGameNetState();

	//复活
	void							RequestRespawnOnCurrentLocation(bool bAuto = false);
	void							RequestRespawnOnSafeLocation();
	void							RequestRespawnByItem();
	bool							CheckCanRespawnByItem();

	bool							CanRespawnOnSafe(int mapResID);
	long							GetRespawnProtectTime(int mapResID);
	StringType						GetEnterMapItemInfo(int mapResID);

	//机器人复活
	void							RobotRequestRespawnOnCurrentLocation(GameLogic::LC_ActorBase* pkMachineOwner);
	//机器人
	void							Change2NextPos(int nPosX, int nPosY, int nPosZ);
	bool							IsSolodule_Robot_PlayerId(uint64_t uiID);//判断是否是机器人
	//npc 特殊泡泡接口
	virtual void					StampASpecialBubble(GameLogic::LC_ActorBase* pkActor, const StringType& kBubbleMsg, int lShownTime);
	//根据一个ActorBase ID获得屏幕坐标
	void							UpdateUIScreenPosByLogicObject(GUISys::WT_Widget* pkWidget,uint64_t lObjectID,int iXOffset,int iYOffset,int iWidth,int iHeight);

	object_id_type					GetLogicIDByCharType(int lCharTypeID);
	object_id_type					GetChestIDByCharType(int lCharType);

	void							ChangeLockedTarget();

	//升级
	//void							RequestLevelUp();

	long							GetSkillUpGradeNeedCash(long lSkillID);
	int								GetSkillUpgradeNeedBuddyStar(long lSkillID);
	//技能能否升级
	bool							GetSkillCanUpGrade(int lSkillID, int iCID);
	//技能能否突破
	bool							GetSkillCanWakeUp(int lSkillID, int iCID);

	virtual int						CheckActivateSkill(GameLogic::LC_Attacker* pkScriptCaller, int lSkillTypeID);
	virtual void					NotifyTreasureSkillInterrupt();

	//获得怪物等级类型
	GameLogic::NPCLevelType			GetNPCLevelType(object_id_type lNPCID);
	GameLogic::NPCLevelType			GetNPCLevelTypeByCharType(int iCharType);
	//获得怪物精英类型
	GameLogic::NPCEliteType			GetNPCEliteType(object_id_type lNpcID);

	//任务
	bool							CanGetTaskItem(int lItemID);
	bool							IsTaskInCurrentArea(int lTaskID);
	int							GetItemEquipLimit(int lItemID);
	//特训
	int							GetGetSpecialTrainingCount();
	//强化
	int							GetEnhanceLimitFuncID(int nIndex);
	int							GetEnhanceNeedMaterialID(int nIndex);
	int							GetEnhanceNeedMaterialNum(int nIndex);
	int							GetItemEnhanceLevel(int nIndex);
	int							GetItemEnhanceLevelLimit(int nIndex);
	int							GetEnhanceNeedRuneNum(int nIndex);
	bool							StartEnhanceSkill();
	bool							StartIdentifySkill(int iBackPackType,int iBackPackIndex);	
	int							GetEnhanceFuncID(object_id_type lNPCID);
	StringType						GetItemEnhanceFuncDescript(int lItemEnhanceNextLevel);
	bool							IsEnhanceRune(int lItemTypeID);
	bool							CheckNeedUnBindCashDialog(int iItemID, bool bConfirmUseUnBindCash);
	//------------------------------------------------------------------------------------------------------
	StringType					GetEnhanceEquipLevelProperty(int iEqupSlotIdx, int iEquipReinforceLevel );		// 装备强化到这级所提供的增强属性，查配表
	StringType					GetBreakEquipDescription(int iEqupSlotIdx, int iEquipBreakLevel );		// 装备升星需要的消耗，查配表
	StringType					GetEnhanceEquipCost(int iEqupSlotIdx, int iEquipReinforceLevel );		// 装备强化需要的消耗，查配表
	int							GetEnhanceEquipNeedLevel(int iEquipReinforceLevel );					// 装备强化需要的伙伴等级，查配表
	int							GetEnhanceEquipNeedEquipLevel(int iEquipReinforceLevel);
	int							GetEquipSlotMaxLevel();
	StringType					GetBreakEquipLevelProperty(int iEqupSlotIdx, int iEquipStar);
	int							GetBreakEquipPropertySpecialChange(int iEqupSlotIdx, int iEquipStar);
	StringType					GetBreakEquipProperty(LC_BackPackEntry* pkEquipEntry, int iEqupSlotIdx, int iEquipStar);
	StringType					GetItemBaseAttributes(int iItemID);
	int64_t						GetEquipSlotScore(int iEqupSlotIdx, int iEquipReinforceLevel, int iEquipStar);			//装备格子战力，包括强化，升星，格子内装备的总战力

	//指挥官装备
	StringType					GetCommanderReinforceEquipProperty(int iEqupSlotIdx, int iEquipReinforceLevel );		// 装备强化到这级所提供的增强属性，查配表
	StringType					GetCommanderReinforceEquipCost(int iEqupSlotIdx, int iEquipReinforceLevel );			// 装备强化需要的消耗，查配表
	int							GetCommanderReinforceEquipNeedLevel(int iEquipReinforceLevel );							// 装备强化需要的指挥官等级，查配表
	int							GetCommanderEquipSlotMaxLevel();
	StringType					GetCommanderRefineEquipInfo(int iEqupSlotIdx, int iEquipRefineLevel );
	StringType					GetCommanderRefineEquipCostInfo(int iEqupSlotIdx, int iEquipRefineLevel);
	StringType					GetCommanderModingSpiritEquipCostInfo(int iPackType, int iEqupSlotIdx, int iLevel);
	StringType					GetReinforceSuitInfo();
	StringType					GetRefineSuitInfo();
	StringType					GetModingSpiritSuitInfo();
	combat_score_type			GetCommanderEquipScore(int iEquipSlotIdx);
	
	int							GetMaxValidLevelInVIPProfit(int iType);

	StringType					GetItemNonpareilEffect(int iItemID);
	StringType					GetItemNonpareilEffectBase(int iItemID);
	StringType					GetGuildBattleBossRankReward(int chartypeID);
	StringType					GetGuildBattleBossBaseReward(int chartypeID);
	int							GetGuildBattleTowerBuff(int chartypeID);
	int							GetGuildBattleTowerIntegralAdd(int chartypeID);
	int							GetGuildBattleTowerVipBonusPercent(int chartypeID);
	StringType					GetGuildBattleBossInfoByType(int iType);
	StringType					GetGuildBattleBossInfoByCharType(int iCharType);


	StringType					GetGuildBattleRobRewardInfo();

	int							GetWeekDayByTime(uint32_t itime);
	int							GetSkillDirection(int lSkillID);
	StringType					GetBestGuildRankStageInfo(int itype);
	int							GetBestGuildGoalCount(int iID);
	int32_t						GetSevendaysRewardEndTime(int32_t beginTime, int32_t itime);
	StringType					GetToyIsLandBossLink(int mapID);
	StringType					GetExpAccessWayInfo();

	int							GetCurrentProcessId();

	//任务追踪栏荣光远征链接信息
	StringType					GetGloryRewardLinkInfo(int iTransRuleID);

	//套装收集
	StringType					GetCommanderEquipSuitCollectInfo();

	//资源找回
	StringType					GetResourceRetrieveInfo();
	StringType					GetRatioByLevel(int iLevel);
	StringType					GetAchievementTypeInfo(int iType);

	StringType					GetBuddyTransformInfoByCid(int iCid);
	StringType					GetBuddyTransformName(int iID);
	int							GetBuddyTransformGrade(int iID);
	int							GetBuddyTransformBuddyId(int iID);
	
	StringType					GetAchievementBigType();
	StringType					GetAchievementSmallTypeName(int iID);
	int							GetGuildProfitMaxExp(int ilevel);
	int							GetGuildCrystalQuality(int ilevel);
	int							GetGuildCrystalMaxExp(int igrade);
	int							GetGuildProfitMaxLevel();

	//------------------------------------------------------------------------------------------------------

	//生产
	StringType					GetProductionTypeName(int lProductionType);
	StringType					GetProductionInfo(int lFormulaID);

	StringType					GetItemName(int lItemID);
	int							GetCurLevelMaxMastery(int lProductionLevel);
	int							GetProductionNeedLevel(int lProductionLevel);
	int							GetCanProductionNum(int lFormulaID);
	int							GetMadeItemGrade(int lItemTypeID);
	void						AddCanLearnFormulaEntry();
	void						RequestInterruptProduction();

	void							StartProduceSkill(GameLogic::LC_Attacker* pkScriptCaller,GameLogic::LC_ActorOperationEntry* pkOperation);
	void							LearnProduction(int lProductionType, int lProductionLevel);
	bool							ForgetProduction();
	void							UpgradeProduction(int lProductionType);
	bool							LearnBaseFormula(int lFormulaID);
	bool							IsLearnedProduction();
    //烹饪
	bool                            CanContinueCookDish();
	void                            RequstInterruptCooking();
	//新手训练
	bool							SetBitVec(int nIndex, bool bSet);
	bool							IsEquipID(int lItemID);
	bool							IsMedicineID(int iItemID);
	bool							IsQinggongSkillID(int iSkillID);
	bool							IsGemID(int lItemID);
	bool							IsSkillBookID(int lItemID);
	void							TriggerNPCFunc(int nTaskState);
	bool							IsCanLevelUp(int64_t lEXP);
	void							InsertNPCFuncEvent();
	bool							IsCanEnChantItem(int lItemID);
	//是否为Npc，非怪物
	bool							IsNpc(object_id_type iObjId);

	//屏蔽接口
	void							SwitchShieldVisibility(int iIndex, bool visiable);
	
	bool							IsIMEOpen();	

	// 邮件确认
	void							ConfirmSendMail(bool bPayFlag, const StringType& kName, const StringType& kCash);	
	// 门派确认
	void							ConfirmExitGuild();
	void							ConfirmDismissGuild();

	combat_score_type				GetSkillStateCombatScoreBySkillID(int SkillID);
	StringType						GetSkillStateUseResult(int iID);
	StringType						GetSevenDaysRewardInfo(int iID);

	// 使用物品确认  
	void							UseItemInPackByIndex(int iPackType, int iEntryIndex, int iParam, int iCid = 0, int iCount=1);
	void							EquipItemInPackage(int iEntryIndex, int cID);
	int								GetItemUseUIOpenFuncID(int iItemID);
	bool							GetItembShowUseIconByItemID(int iItemID);
	StringType						GetItemPropertyFuncSkills(int iItemID);

	void							PickItemInChestByIndex(object_id_type lPlaceHolderID, int nIndex);
	void							RollButtonDown(int nButtonType, int nRollWndIndex);
	void							OPItemInTempPack(int iOPType, int nIndex, int iDstPackType, int iDstIndex);

	ResultType						CheckOperationValid(GameLogic::LC_ActorOperationEntry* pkOperation);
	ResultType						CheckOperationExecute(GameLogic::LC_ActorOperationEntry* pkOperation);

	virtual ResultType				ExecuteOperation(GameLogic::LC_Attacker* pkScriptCaller,GameLogic::LC_ActorOperationEntry* pkOperation);
	virtual ResultType				SetOperationParameter(GameLogic::LC_Attacker* pkScriptCaller,GameLogic::LC_ActorOperationEntry* pkOperation,int iParameterIndex,int lVal);
	void							ReqSetOperationParameter(int lOperationType, int iParameterIndex,int lVal, int iCid=0);
	void							ReqResetOperationParameter(int lOperationType, bool bClearParameters);
	virtual void					SetUIMouseLogicType(GameLogic::LC_Attacker* pkScriptCaller, int iMouseLogicType);

	int							GetHookItemCostID(int idx);
	int							GetHookItemCostAmount(int idx);
	int							GetHookItemCostTime();
	/*-----------------		End		UI  ---------------------------------*/


	/* ----------------  Begin    属性数值 ---------------------------*/
	virtual void ModifyTargetAttributeByValue(GameLogic::LC_Attacker* pkScriptCaller,int iTargetIndex,int iAttributeType,int64_t iDeltaValue,bool bRevert){};
	virtual void ModifyTargetAttributeByPercent(GameLogic::LC_Attacker* pkScriptCaller,int iTargetIndex,int iAttributeType,double fDeltaPercent,bool bRevert){};
	/* ----------------  End    属性数值 ---------------------------*/


	
	/* ----------------  Begin    技能 ---------------------------*/
	virtual const StringType	GetNormalAttackAnimName(GameLogic::LC_Attacker* pkScriptCaller,int iIndex);

	virtual void		PlayTargetAnimation(GameLogic::LC_Attacker* pkScriptCaller,int iTargetIndex,const StringType& rkAnimName);
	virtual void		PlayTargetAnimationFreq(GameLogic::LC_Attacker* pkScriptCaller,int iTargetIndex,const StringType& rkAnimName, float fSequency);
	virtual void		PlayTargetWeaponAnimation(GameLogic::LC_Attacker* pkScriptCaller,int iTargetIndex,const StringType& rkAnimName);
	virtual void		PlayManPaiAnimation(GameLogic::LC_Attacker* pkScriptCaller,int iTargetIndex);
	virtual void		FaceToTarget(GameLogic::LC_Attacker* pkScriptCaller,int iTargetIndex);
	virtual void		SetTargetLocation(GameLogic::LC_Attacker* pkScriptCaller,int iTargetIndex);
	virtual void		SetLockedLocationByDir(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iFaceDistance);
	virtual void		SetLockedLocationByMouseLocation(GameLogic::LC_Attacker* pkScriptCaller, bool bLockedTarget);
	virtual void		SetLockedLocation(GameLogic::LC_Attacker* pkScriptCaller, int iFaceDistance, float fDeltaAngle);

	//trick 直接传三个值进去走，没办法了
	virtual void				SetTargetLocationValue(GameLogic::LC_Attacker* pkScriptCaller, int posx, int posy, int posz);
	virtual void				SetClosestTargetLocation(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex);

	virtual void		SetAlphaDead(GameLogic::LC_Attacker* pkScriptCaller, bool kFlag);

	//计算技能伤害
	/*
	iDamageTargetIndex :	伤害目标
	lSkillTypeID :			技能类型
	iDamageRandomIndex :	伤害力骰子
	iAttackRateRandomIndex :命中骰子
	iCriticalRateRandomIndex :暴击骰子
	lDamageSourceActorID :	发动技能者的ID
	iHPCost				 :	本次技能的HP消耗，用于计算伤害，该参数暂时无用
	iMPCost				 :	本次技能的MP消耗，用于计算伤害，如果传入0，函数内自动计算技能MP消耗（默认MP消耗和额外消耗）
	*/
	virtual void		GenerateSkillDamage(GameLogic::LC_Attacker* pkScriptCaller,int iDamageTargetIndex,	int lSkillTypeID,int iDamageRandomIndex, int iAttackRateRandomIndex,int iCriticalRateRandomIndex,object_id_type lDamageSourceActorID, int iHPCost, int iMPCost);
	virtual void		GenerateSkillDamageMultiple(GameLogic::LC_Attacker* pkScriptCaller,int iDamageTargetIndex, int lSkillTypeID,int iDamageRandomIndex, int iAttackRateRandomIndex,int iCriticalRateRandomIndex, object_id_type lDamageSourceActorID, int iHPCost, int iMPCost, int Multiple, int fPercent);
	virtual void		GenerateSkillDamageMainTarget( GameLogic::LC_Attacker* pkScriptCaller,int iDamageTargetIndex, int lSkillTypeID,int iDamageRandomIndex, int iAttackRateRandomIndex,int iCriticalRateRandomIndex,object_id_type lDamageSourceActorID, int iHPCost, int iMPCost,int fPercent );
	
	//计算绝对伤害
	/*
	iDamageTargetIndex :	目标说明
	lDamageLogicTypeID :	产生伤害的技能编号
	lDamageType :			伤害的类型
	iDamage :				伤害点数
	lDamageSourceActorID :	发起伤害者的编号
	*/
	virtual void		GenerateDamage(GameLogic::LC_Attacker* pkScriptCaller,int iDamageTargetIndex,int lDamageLogicTypeID,int lDamageType,int iDamage,object_id_type lDamageSourceActorID);

	//使用打坐技能恢复HP/MP，回复完成结束打坐技能
	//bool SitForRecovery(GameLogic::LC_ActorBase* pkMachineOwner);

	//给目标绑定一个特效
    //virtual void AttachTargetGfx(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iDummyType, const StringType& strFxFileName);
	virtual void AttachClientTargetGfx(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iDummyType, const StringType& strFxFileName, bool bStayStatic);
	virtual void AttachClientTargetGfxSacle(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iDummyType, const StringType& strFxFileName, bool bStayStatic,float fScale);
	virtual void AttachClientTargetGfxWithDir(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iDummyType, const StringType& strFxFileName, bool bStayStatic, int eBindMode, float dirAngle, float posx,float posy,float posz);
	virtual void AttachClientTargetGfxWithDirScale(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iDummyType, const StringType& strFxFileName, bool bStayStatic, int eBindMode, float dirAngle, float posx,float posy,float posz, float scale);
	virtual void AttachClientTargetGfxWith3DScale(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iDummyType, const StringType& strFxFileName, bool bStayStatic, int eBindMode, float xAngle=0, float yAngle=0, float zAngle=0, float posx=0, float posy=0, float posz=0, float scale=1.0f);
	//连接两个目标的特效
	virtual void AttachClientTargetPairGfx(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iDummyType1, int iPairTargetIndex, int iDummyType2, const StringType& strFxFileName);
	virtual void AttachClientTargetPairGfxLink(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iDummyType1, int iPairTargetIndex, int iDummyType2, const StringType& strFxFileName);

	virtual void AttachClientTargetBowGfx(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iDummyType, const StringType& strFxFileName, bool bStayStatic, int eBindMode, float fScale);
	virtual void AttachClientTargetBowGfxWithDir(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iDummyType, const StringType& strFxFileName, bool bStayStatic, int eBindMode, float xAngle, float yAngle, float zAngle, float posx,float posy,float posz, float fScale);	

	virtual int AttachClientTargetSound(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, const StringType& kFileName, int iSoundChannel, int iSoundType);
	virtual int	AttachClientTargetSoundCount(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, const StringType& kFileName, int iSoundChannel, int iSoundType, bool isLoop);
	virtual void StopClientTargetSound(GameLogic::LC_Attacker* pkScriptCaller, int SoundID, bool isFade, float fadeTime);
	virtual void UpdateClientTargetSoundPosition(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int soundID);
	//角色语音系统用 (自身 3D 语音)
	virtual int AttachClientTargetVoice(GameLogic::LC_Attacker* pkScriptCaller, const StringType& kFileName, int iSoundChannel);
	static bool CheckAttachSkillSound(LC_ClientGamePlayer* pkGamePlayer, GameLogic::LC_Attacker* pkScriptCaller);
	
	virtual void AttachClientNPCSoundSeq(const StringType& kFileName);

	virtual  void AttachClientGameEvent(int eventID);

	virtual void OnPlayNPCSound(object_id_type npcID);


	//给目标取消一个绑定特效
    virtual void DetachTargetGfx(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iDummyType, const StringType& strFxFileName);
	virtual void DetachTargetBowGfx(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iDummyType, const StringType& strFxFileName);

	StringType GetSitRecoverVipGfx(GameLogic::LC_Attacker* pkScriptCaller);

	//发散子弹
	virtual void ShotBulletToTarget(GameLogic::LC_Attacker* pkScriptCaller,int iTargetIndex,int lSkillTypeID,int iBulletFuncIdx,int iPenetrateCount,int iDummyType);
	virtual void ShotBulletToTargetAngularDeflection(GameLogic::LC_Attacker* pkScriptCaller,int iTargetIndex,int lSkillTypeID,int iBulletFuncIdx,int iPenetrateCount,int iDummyType, float fAngle);
	virtual void ShotBulletToTargetFromLocation(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lSkillTypeID, int iBulletFuncID, int iPenetrateCount, int iDummyType, int iLocationIndex, float fDeltaAngle, int64_t lBulletID);
	virtual void ShotMultiBulletToTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lSkillTypeID, int iBulletFuncIdx, int iPenetrateCount, int iDummyType, int iScatteringAngle, int iScatteringCnt);
	virtual void ShotBulletAroundTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lMinRadius, int lMaxRadius, int lSkillTypeID, int iBulletFuncID, int iPenetrateCount, int iDummyType);
	virtual void ShotMultiBulletToTargetFromLocation(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lSkillTypeID, int iBulletFuncID, int iPenetrateCount, int iDummyType, int iScatteringAngle, int iScatteringCnt, int iLocationIndex, float fDeltaAngle, int64_t lBulletID);
	virtual void ShotMultiBulletToTargetFromMultiLocation(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lMinRadius, int lMaxRadius, int lSkillTypeID, int iBulletFuncID, int iPenetrateCount, int iDummyType, int iScatteringAngle, int iScatteringCnt, int iLocationIndex, float fDeltaAngle, int64_t lBulletID);

	//打开宝箱
	virtual void OpenPlaceHolder(GameLogic::LC_Attacker* pkScriptCaller,int iTargetIndex, bool bNeedOpenFirst);

	//与NPC交互
	virtual void TalkToNPC(GameLogic::LC_Attacker* pkScriptCaller,int iTargetIndex);

	//采集物品
	virtual void CollectItem(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex);

	//开启区域内反隐功能
	virtual void StartRevealInvisibleTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iCircleRadius, bool bRevealOnce);

	/* ----------------  End    技能 ---------------------------*/

	/*---------------------begin 其他 ------------------------*/
	void StartDebugConsole(const char* szPath);
	/*---------------------End 其他--------------------------*/

	//从脚本弹出提示框，目前用于检查到技能不能释放时给出提示
	virtual void SendInfoMessage(int lSkillID, int lMsgID);

	//从UI抛出提示消息
	virtual void ShowUIError(const StringType& kString);
	virtual void ShowUIErrorByCode(int iErrorCode);
	void ShowNoticeMessage(int iID);
	//从UI抛出提示消息
	virtual void ShowMouseTips(const StringType& kString);
	//从UI跑出提示信息--活动提示信息
	virtual void ShowNotifyMessage(const StringType& kString, int iLevel);
	//根据TextConfigid提示
	virtual void ShowNotifyMessageWithType(long iMsgType,const StringType& kString, int iLevel);
	//从UI抛出系统提示
	void ShowSystemTip(const StringType& kString);

	virtual void ShowAttriActive(int kAttribute, int val);

	//获取调试信息
	StringType GetActorHatredList(object_id_type lActorID);
	StringType GetActorSkillList(object_id_type lActorID);

	//robot change avatar  execute for a while
	int m_iCount;
	bool m_bEquip;
	int m_iClothesCount;

    void Snapshot(bool bWithUI, bool bTiled);

	void	NoticeMessage( int lMSG_ID, int lValue);
	void	AddNoticeTask( int lMSG_ID, int lLeftTime, int lIntervalTime, int lValue, int lDeltValue);
	void	RemoveNoticeTask( int lMSG_ID );

    // 强制天气开关
    virtual	void StartForcibleWeather(float fDensity, float fSpeed, float fLength, float fWidth, float fAlpha, float fWind);
    virtual	void StopForcibleWeather(void);

    // 全屏效果开关
    virtual	void StartFullScreenEffect(int eFullScreenEffectType);
    virtual	void StopFullScreenEffect(void);

	virtual void StopCurrentBGM(void);

	//取系统时间
	StringType	GetComputerTime();

	//烹饪
	void	   CookingDish();

	//声音	   
	void	   PlaySound(const StringType& strKeyWord, int iChannelType = 0, bool bFadeToGain = false, int nPlayCount = 1 , float fAutoStop = 0.0f);

	int	   GetShowBloodBarType(GameLogic::LC_Attacker* pkScriptCaller);

	//题字
	StringType GetItemText1CharFuncDescript(int lTextItemID1,int lLevel);
	StringType GetItemText2CharFuncDescript(int lTextItemID1,int lLevel1,int lTextItemID2,int lLevel2);
	StringType GetItemText4CharFuncDescript(int lTextItemID1,int lLevel1,int lTextItemID2,int lLevel2,int lTextItemID3,int lLevel3,int lTextItemID4,int lLevel4);
	//爱好
	//StringType GetItemHobbyAttributeFuncDescript(int lHobbyItemID,char cHobbyQualityValue,char cHobbyQualityLevel);

	StringType GetAttributeFuncDescript(int lAttributeFuncID);

	//获得跨服战胜利公会ID
	guild_id_type GetGlobleWarGuildID();
	//获得势力战占领帮会数量
	bool IsDomainWarGuild(guild_id_type GuildID);

	//令牌合成
	int		GetTokenCompositeItemSlot(int lItemID);
	int		GetTokenCompositedItemID(int lItemIDSlot0, int lItemIDSlot1);
	void		RequestInterruptTokenComposite();


	int		GetMaxItemEnduranceValue(int lItemID);

	StringType	GetMapNameByMapResID(int iMapResID);
	int			GetMapLineCount(int iMapResID);

	//获取地图进入条件信息
	StringType	GetMapEnterConditionInfo(int iMapResID);

	bool		IsNeedBind(GameLogic::LC_BackPackEntry* pkBackPackEntry, int iBindType);

	//显示/隐藏UI
	void		ShowUIFrame(bool bShow);
	virtual		void ShowRichUI(GameLogic::LC_Attacker* pkScriptCaller, int iTargetType,int iRichUIID, int bShow);

	//物品UI交互
	void		ShowUIContentFunc(int lItemTypeID);
	int		GetShowUITypeID(int lItemTypeID, int nIndex);
	StringType	GetShowUIIconDesc(int lItemTypeID, int nIndex);
	int		GetDescriptType(int lTypeID);

	//根据npc商店功能ID获取商店名称
	StringType  GetNPCShopNameByFuncID(int lNPCShopFuncID);

	//begin 副本地图传送
	LC_ClientMapTransDataEntry* GetMapTransDataEntryByMapID(int lMapResID);
	//end 副本地图传送
	void						StopUICutScenes(void);
	float						GetUICutScenesPlayTime(int ID);
	float						GetUICutScenesTimeStep(int ID);
	const StringType&			GetUICutScenesIcon(int ID);

	//OpenLevel信息相关
	StringType					GetSkillOpenLevelInfo(LC_PlayerBase *pkPlayer);
	StringType					GetTwToZn(const StringType &sName);
	//通过快捷键位置获取第一个技能
	int							GetFirstSkillIDByShortcutSlot(int iSlotID);

	//是否能装备(使用)
	int							CanEquipOrUseItem(LC_ClientGamePlayer *pkPlayer, int lItemID, int iCid);
	//是否能装备(进阶线装备)
	bool						CanEquipSubItem(LC_PlayerBase *pkPlayer, int lItemID);
	//是否能拆解
	bool						CanDecomposition(int lItemID);
	//获取礼包列表字符串
	StringType					GetItemDisassembleUseFuncResult(int lItemID, bool bProbability = false, int32_t iChoice = 0);
	//获取掉落物
	StringType					GetItemDisassembleMustDropItems(int _iID);
	//获取多开礼包的选择
	StringType					GetMultiSelectGiftChoices(int lItemID);	
	//获取多开礼包的选择
	StringType					GetMultiSelectGiftChoicesCost(int lItemID);	
	//获取物品SHOWID
	int							GetItemShowID(int lItemID);
	StringType					GetBuddyExpDrugs();

	//获得竞技场奖励每日金钱、经验、勇气奖励系数
	float						GetArenaDailyRewardParam(int iRank, int iRewardType);

	//获得创建公会所需等级
	int							GetGuildCreateRequireLevel(void);
	//获得创建公会所需金币
	int							GetGuildCreateRequireMoney(void);
	//获得创建公会所需元宝
	int							GetGuildCreateRequireYuanbao(void);
	//
	int							GetGuildCreateRequireVip(void);
	//获取boss id列表
	StringType					GetBossIdList(void);
	//是否为解绑用道具
	bool						IsUnbindItem(int iItemID);
	//是否为解绑用道具
	bool						CanBeBatchUsed(int iItemID);
	//获取地图表现类型
	int							GetMapShowType(long lMapResID);
	//获取UI地图显示类型
	int							GetUIMapShowType(long lMapResID);
	//获取地图阵容组别id
	int							GetSchemeGroupID(long lMapResID);
	//获取地图是否隐藏boss血条显示伤害进度条
	int64_t						GetHarmQuantity(long lMapResID);
	//获取地图所属活动id
	int							GetMapBelongStoryID(long lMapResID);
	//获取地图阵容可出战队伍数量
	int							GetMapSchemeTeamCount(int iMapResID);
	//获取变身信息(仅适用于NPC)
	int							GetShapeShiftInfo(GameLogic::LC_Attacker* pkScriptCaller);
	//从该地图出来后是否自动主线任务
	bool						GetMapAutoTask(int iMapResID);

	//获取VIP权限字符串
	StringType					GetVipAuthorityString(int iVipLevel);
	StringType					GetVipNameString(int iVipLevel);
	StringType					GetVipPrivelegeInfo();
	StringType					GetVipPrivelegeInfoByType(int iType);
	int							GetVipPrivelegeInfoByTypeAndLevel(int iType,int iLevel);
	StringType					GetShowVipPrivelegeInfoByType(int iType);
	StringType					GetFlowerSendInfo(int iID);
	//获取公会列表每页最大数目
	int							GetMaxGuildSearchNum(void);
	//Begin 每日挑战任务活跃度相关
	int							GetLivenessByLevel(int iLevel);
	//End 每日挑战任务活跃度相关

	StringType					GetChoosePetIds();
	StringType					GetIconByNPCFuntionType(int iNPCFunctionType);

	//查找当前进度对应的bossID， boss名字，地图名字
	StringType					GetProgressInfo(int iStep);
	//获取地图章节信息
	StringType					GetMapCateGoryInfo(void);
	StringType					GetMapRwdInfo(int iMapResID);
	int							GetShowRaidStage(int iMapResID);
	//获取副本复活替身草人数量
	int							GetRespawnItemNum(int iMapResID);
	//判断位置是否可行走
	bool						CanScreenPosMoveTo(float x, float y);
	StringType					URLEncode(const StringType &sIn);
	//获取组队副本信息
	StringType					GetCrossRealmMapInfo(int iMapTypeID);
	int							GetCrossRealmMapTargetType(int iMapTypeID);
	unique_id_type				GetKungFuMasterID();
	unique_id_type				GetCrossKungFuMasterID();
	int							GetCrossRealWarServerID();
	//获取跨服，本服公会战套装奖励装备ID
	int							GetKungFuMasterDress(unique_id_type CharID, int prevItemID, int EquipType, bool ignoreFashion);
	//获取跨服，本服公会战套装奖励装备ID(不进行角色ID的判断，直接取数据
	int							GetKungFuMasterDress2(int prevItemID, int EquipType, int index, bool ignoreFashion);

	guild_id_type				GetPowerGuildID();
	int							GetDirtyFlagRevisionID(int iFlag);
	StringType					GetMapDomainInfo(int iMapResID,int iServerID);
	bool						IsDomainMap(int iMapResID);
	StringType					GetDomainFlagPos(int iMapResID);
	int							GetDomainMapResIDByIndex(int iIndex);
	bool						IsDomainActive();
	int							CheckDomainReward(int iMapResID);	//判断主角能不能领取势力争夺战奖励 0（不能领）1（领过了）2（可以领）
	int64_t						NumberPlus(int64_t iNumber1, int64_t iNumber2);
	//获取玩家战斗力
	int64_t						GetPlayerCombatScore(LC_ClientGamePlayer *pkPlayer);
	//判断玩家是否是武林盟主
	bool						IsKungFuMasterID(unique_id_type iUserID);
	StringType					GetGuildWarCatapultList(int iPlayerLevel);

	//该地图是否是组队副本
	bool						IsCrossRealmMap(int iMapResID);
	//判断邮件附件是否是特殊ID
	bool						IsSpecialAffixItemID(int itemID);
	//是否是帮会boss地图
	bool						IsGuildRaidMap(int iMapResID);
	//获取talkicon
	StringType					GetCharTalkIcon(int iCharID);
	//获取char名字颜色
	uint32_t							GetCharNameColor(LC_PlayerBase* pkWatcher, LC_ActorBase* pkTarget, int iPowerGuildID = -1);

	//获取副本时间(时间结束时，被拉出副本)
	int							GetDungeonTime(int iMapResID);
	bool						CanEnterMap(int iMapResID);
	int							CanEnterMapWithPrompt(int iMapResID);
	int							GetMapNeedLevel(int iMapResID);
	StringType					GetMapPreTaskName(int iMapResID);
	StringType					GetHookMapsInfo();

	//节日活动 消费比拼
	StringType					GetHolidayRankRewardsInfo(int iType, int iParam);

	//开服争霸榜奖励信息
	StringType					GetGamePlayerSubRewardsInfo(int iGroupId);
	//战斗力
	combat_score_type							GetCimeliaTotalScore(LC_PlayerBase *pkPlayer, int iType, int iLevel);
	combat_score_type							GetCharAttributeCombatScore(long lAttribFunc);
	combat_score_type							GetVehicleFightingTotalScore(LC_PlayerBase *pkPlayer, int iLevel);
	combat_score_type						GetEquipTotalScore(LC_PlayerBase *pkPlayer);
	combat_score_type							GetStandardCombatScore(LC_PlayerBase *pkPlayer);
	combat_score_type							GetSkillTotalScore(LC_PlayerBase *pkPlayer);
	combat_score_type					GetSkillScore(int iSkillID);
	combat_score_type					GetEquipScore(LC_BackPackEntry *pkBackPackEntry);
	combat_score_type							GetEquipScoreI3(int iItemID, int iEquipReinforceLevel, int iEquipReinforceStar, int iSkillID);
	combat_score_type					GetEquipScoreByItemID(int iItemID);
	combat_score_type					GetEquipReinforceScore(LC_BackPackEntry *pkBackPackEntry,bool bHasNext);
	combat_score_type							GetItemSuitTotalScore(int iSuitID, int iPara);
	combat_score_type							GetRuneEquipBaseScore(int iItemID);
	combat_score_type						GetRuneEquipScore(const LC_BackPackEntry *pkItemEntry);
	combat_score_type							GetBuddyLevelUpCombatScoreBonus(int iLevel);
	combat_score_type							GetBuddyOriginalCombatScore(int Cid);
	combat_score_type							GetBuddyStarUpCombatScoreBonus(int Cid, int Starlevel, int iLevel);
	combat_score_type						GetItemCombatScore(int iItemId);

	//获取坐骑当前等级
	int							GetVehicleCurrentLevel(int iPlyerLevel, int iVehicleLevel);
	//获取坐骑等战斗力不含零时属性
	combat_score_type							GetCimeliaOriginalScore(int iType, int iLevel);

	StringType					GetOpenBagData(int iTargetIndex);
	int							GetExternWarehouseData(int iCurrentIndex, int iTargetIndex);
	//装备强化相关
	StringType					GetBaseEffectInfo(LC_BackPackEntry *pkEntry);
	//获取商城物品信息
	StringType					GetShopItemInfo(int iItemID, int iShopType);
	//获取物品出售价格信息
	int							GetItemUnBindPrice(int iItemID);
	//获取变强信息
	StringType					GetPowerUpInfo(int iLevel);
	//该装备是否为当前角色最适装备(是否为最适等级需求)
	bool						IsEquipMostSuitable(int iItemID);
	//获取当前核心系统等级所需升级道具数量
	StringType					GetCimeliaItemInfo(int iLevel, int iCimeliaType);
	StringType					GetCimeliaSkillInfo(int iCimeliaType, int iLevel);
	int							GetCimeliaDisplaySkillID(int iCimeliaType, int iLevel);
	//获取当前等级可学技能数量
	int							GetCurrentCanLearnSkillCount(LC_PlayerBase *pkPlayer);
	
	//获取EFFECT属性
	StringType					GetEffectDesp(int iEffectID, int iEffectLevel);
	//获取Effect属性描述信息
	StringType					GetItemEffectFuncInfo(int iEffectFuncID);

	//镜头控制
	virtual int PlayCameraTrackAnim(int trackIdx);
	virtual int	PlayCameraMaxTrackAnim(int lCharType);
	virtual void StopCameraTrackAnim();
	virtual void RestoreFollowCamera();
	virtual void MoveCameraToNpcCharType(int lNpcCharTypeID, float go, float back, float stop, bool follow);
	virtual void MoveCameraToTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, float go, float back, float stop, bool follow);
	virtual void MoveCameraToMC(void);
	virtual void MoveCameraToNoUnLock();

	virtual StringType GetAnimModelPath();

	virtual void SceeenFadeIn(float keepTime, float rampTime);
	virtual void SceeenFadeOut(float rampTime, float keepTime);

	virtual void SetModelVisibility(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, float bAlpha , bool bFlash );

	//特殊功能
	virtual void Crash();

	virtual void EyeExecuteSkill(GameLogic::LC_ActorBase* pkCaller, int lSkillID, object_id_type lTargetID);

	// begin npc function
	int	GetNPCConversationSequenceCount(int lConversationID) ;
	int	GetNPCConversationSequenceCount(int lConversationID) const;
	int GetNPCConversationCharID(int lConversationID, int iCurSeq) ;
	int GetNPCConversationCharID(int lConversationID, int iCurSeq) const;
	const StringType& GetNPCConversationContent(int lConversationID, int iCurSeq) ;
	const StringType& GetNPCConversationContent(int lConversationID, int iCurSeq) const;
	const StringType& GetNPCConversationName(int lConversationID) ;
	const StringType& GetNPCConversationName(int lConversationID) const;
	int GetConversationTypeByID(int lConversationID);
	float GetConversationTime(int lConversationID, int iCurSeq);
	StringType GetConversationSoundKey(int lConversationID, int iCurSeq);
	// end npc function

	StringType	GetDataString(int iDataStringType, int iID, int iIndex);
	StringType	GetWildPrincesData(int iDataStringType, int iMapResID, int iStage);
	StringType	GetAchievementData(int iDataStringType, int iClassID, int iSubClassID);
	StringType	GetCommonStatRewardList(int iDataStringType, int iType, int iKey);

	int GetSystemTitlePeriod(int iTitleId);
	StringType GetGuildClashBossName(int index);
	int GetGuildClashBossLocationID(int index);
	int GetNPCLevelByCharType(int lCharType);
	int GetRaidBossPublicLinkID(int lCharType);
	int GetGuildBossLinkID(int MapID);

	int GetSubordinateAttriLevel(int openID, int stage, bool bNext);

	void  RequestUseGuildTransportItem(int index);
	void  RequestGuildWorship(int worshipType, unique_id_type targetID);
	
	int GetItemGuildMoneyValue(int itemId);

	int GetGuildWorshipReward(int worshipType);
	int GetGuildWorshipCost(int worshipType);
	int GetGuildWorshipVipLevel(int worshipType);

	void ShowMessage(int messagetype, const StringType& para1, const StringType& para2);
	StringType GetErrorString(int iErrorId);

	//跟制定NPC的距离
	int GetDistanceFromNpc(LC_ActorBase *pkNpc);
	//是否在微端中运行
	int IsRunInMiniLauncher(void);
	//打点
	void CommitPatchProgress(int iEvent, int iCode);
	//地图是否可使用帮会召集令
	bool IsMapCanGuildTransport(int iMapResID);

	int GetCurrentNPCTalkTaskFunction();
	
	int GetVipProfitDefineExtraBeginID(int iID, int iLevel );

	StringType GetWorld2RadarPositionInfo(int iMapResID, int iX, int iY);

	//rumormanager
	LC_ClientRumorManager * GetClientRumorManager();

	//物品是否自动使用
	bool	IsItemAutoUseByPlayer(long ItemID);
	bool	IsItemAutoShowByPlayer(long ItemID);
	bool	IsItemTrueAutoUseByPlayer(long ItemID);

	int		GetUseGranuLimitCount(long ItemID);

	//通过cronTime获取下一个时间
	int64_t		GetNextColdDownTimeByCronStr(const StringType& cronStr);

	//瞬移到目标面前
	virtual void	TransferToTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, bool bUseSafeRadius);
	virtual void	TransferBackToTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, bool bUseSafeRadius);

	bool	RaidMapCanRecordPK(int mapResID);
	bool	RaidMapCanPk(int mapResID);

	//中断下载UI资源请求
	void	StopDownloadImageResources(const StringType &sWidgetPath);

	bool	IsNeedShowFly(int iVal);

	StringType GetOpenLevelInfoByKeyID(int lSkillID, int iType);

	int GetWorldObjecDescID(const StringType& info);

	void SetClientModeCull(GameLogic::LC_ActorBase* pkCaller, bool isCull);

	int IsPkPunishArea(int mapResID);

	bool CanChangePKMode(int mapResID, int iPkState);

	int GetFixPKMode(int mapResID);
	int GetEnterPKMode(int mapResID);

	bool CanReturnToCity(GameLogic::LC_Attacker* pkScriptCaller);

	//获取当前当前显卡的默认视频等级
	long			GetDefaultConfigLevelOfGPU();

	//根据id获取OpenLevel需要在线的时间
	int				GetOpenLevelOnlineTimeByID(int iID);

	// 根据openlevel表id获取已在线总时间
	uint32_t		GetOpenLvelTotoalTime(int iID);

	uint32_t		GetCimeliaLevelInOpenLevelByOpenID(int iOpenID,int iSubID);

	bool			IsItemCategoryShowTipAnim(int iCategory2);
	//根据物品id获取物品属性
	StringType		GetItemAttributeString(int iItemID, int iEquipStar);

	//根据物品id获取批量使用时背包最少格子数目
	uint32_t		GetBatchMaxItemCountByID(int iItemID);

	//获取法宝人物charTypeID
	int				GetDisplayCharTypeID(int iType, int GetDisplayCharTypeID);

	//获取密室宝藏对应元宝数
	int				GetTreasureCostByID(int iID);  

	//获取
	StringType	GetGameStoryRaidRewardByMapresID(int iMapResID);  

	//获取帮贡信息	
	StringType	GetGuildDonateInfo();

	StringType  GetActTimeShopInfoByID(int ID);	

	//根据技能id获取秘籍id 绝学表
	uint32_t		GetSkillBookID(int iSkillID);
	uint32_t		GetSecretTypeByBookID(int iBookID);

	//获取玩具信息
	long				GetToyPetCharTypeBycID(int32_t cID);
	StringType			GetToyPetInfoByCID(int32_t cID);

	StringType			GetShopCityGroupInfo(int iGroupID);

	//获取美女客服数量
	uint32_t		GetQQGirlsCount();

	uint32_t		GetFabaoIdByCharType(int charTypeID);
	uint32_t		GetCharTypeByFabaoID(int fabaoID);
	int				GetSubTransformSkillID(int iTransformID);
	int				GetSubTransformType(int iTransformID);

	//圣火令和屠龙刀
	uint32_t		GetGuildWarEndTime(int iWarType,int OpenType);
	uint32_t		CheckGuildWarTime(int iWarType,int OpenType);
	//判断今天是否在活动时间
	StringType		CheckActivitySchedule(int iCatagory);
	int				CheckSecretPoolValueID(int iItemID,int iItemCount,int iCatogory,int iRewardIndex);
	StringType		CheckRichManTime(int iChestID,int iTime);
	int				GetScretTreasureTotalTimes(int iChestID,int iCurValue);
	int				GetScretTimesValue(int iChestID);
	int				GetScretLimitTimes(int iChestID);

	//< tx对接，通过网页购买物品，显示web窗口
	bool			showWebWindowBuyItem(int payitem_id, int payitem_price, int payitem_num, const StringType & goodsmeta, const StringType & goodsurl, const StringType & aux);
	bool			showWebWindowYellowVip(int tokentype, const StringType & discountid, bool bYearOrMonth);
	bool			showWebWindowBlueVip(int tokentype, const StringType & discountid, bool bYearOrMonth);

	int				GetYunyingPlatform();
	int				GetYunyingPlatformPage();
	int				GetYunyingTxSubPlatform();

	StringType		GetCompositionFormulaInfo(int lFormulaID);
	StringType		GetDecompositionInfo(int lFormulaID);

	StringType		GetArenaRankReward(int iEntryIndex, bool bSucessed);
	int32_t			GetInt32tBy2Int16tUnion(int16_t a, int16_t b);

	//装备合成相关
	void			SetItemCompositionTargetInfo(LC_BackPackEntry *SrcEntry, int TargetID);
	LC_BackPackEntry* GetItemCompositionTargetInfo();
	void			ResetItemCompositionTargetInfo();

	int64_t			GetAuctionSellPrice(int iItemID, int iMoneyType);

	bool			IsEquipReinStar(int itemID, uint16_t equipStar);
	//分线机制相关
	int				GetMapIndexByResIDAndLogicID(int lMapResID, int lMapLogicID);
	bool			IsMapIsNewBeeMap(int lMapResID);

	int				GetForceBattleMapIndex(int lMapResID);

	//64位相关，进行uniqueID合成
	unique_id_type	GetUniqueID(object_id_type playerID, object_id_type ServerID);


	inline bool		IsAllianceMap(int iMapResID) {return LC_Helper::IsAllianceMap(iMapResID);}
	int				GetDaysInterval(int64_t iBeginTimes, int64_t iEndTimes);

	int				GetServerOpenTime(bool bHour);

	bool			IsInSelfServer();

	bool			CheckVipPrivilege(int type);
	StringType	GetVipPrivilegeIcon(int type);
	StringType	GetVipPrivilegeDespTips(int type);
	StringType	GetVipPrivilegeDespTitle(int type);

	int   GetShopIDShopType(int iShopID);

	int32_t	  GetItemCompositionFuncIDByType(int iItemID, int iType);
	int32_t	  GetItemDeCompositionFuncIDByType(int iItemID);
	StringType GetEquipUpgradeCompositionInfo(int iFormulaType);
	int GetNxtEquipUpgradeItemID(int iItemID, int iFormulaType);

	int		GetHookStatePosY();

	int		GetBaseLevelExp(int iLevel);

	int		GetSkillLevel(skill_id_type id);

	int		GetPlayerMaxLevel();

	//时装
	int		GetFashionSuitCount();
	
	//
	StringType GetAchievementInfo(int iID);

	//decode platform vip
	StringType DecodePlatformVipInfo(int formatvip);

	//获取当前进程名
	StringType GetProcessName();
	//导出ShellExecute接口
	void LuaShellExecute(const StringType& lpOperation, const StringType& lpFile, const StringType&  lpParameters, const StringType& lpDirectory, int nShowCmd);
	
	int GetGuildBattleTaskOpenSchedule(int iTabIdx);

	bool CheckLocation(int lID,int iDistance);
	//获得同组服务器ID
	int	GetGameServerInfoIndex(int iServerID, int iIndex);

	//获取活动奖励
	StringType GetActivityRewardInfo(int32_t ActivityType, int32_t iCatagory, int32_t iSchedule, int32_t ikey);
	StringType GetActivityRewardInfoWithChoice(int32_t ActivityType, int32_t iCatagory, int32_t iSchedule, int32_t ikey, bool bProbability, int32_t iChoice);
	//资源找回系统物品奖励
	int GetGameStoryCurrentMapResID(int32_t iGameStoryID);
	StringType GetGameStoryBossInfo(int iMapResID);

	//秘笈相关
	StringType GetAllSkillBooks();
	StringType GetEverySkillBooks();	//获取表中所有圣痕，包括未激活的
	StringType GetSkillBookNodeInfo(int iID);
	StringType GetSkillBookInfo(int iID);
	StringType GetSkillBookAttribs(int iSkillBookID,int iPos = -1);
	StringType GetSkillBookAttribsByNodeLevel(int iSkillBookID,int iPos, int iLevel);
	StringType GetSkillBookItemChangeAttribs(int iSkillBookID,int iLevel);
	StringType GetSkillBookItemAttribs(const StringType& items);
	StringType GetCategorySkills(int ilCategory1);
	int64_t	   GetSkillBookCombatScore(int iSkillBookID, int iLevel);
	ResultType CheckSkillBookUpgrade(int iSkillBookID,int iPos);
	StringType GetSkillBookGridInfo();	//圣痕佩戴栏位信息
	StringType GetCimeliaInitAttribInfo(int iType);//进阶线初始属性，第一个普通皮肤附带属性
	StringType GetCimeliaTransformAttribInfo(int iTransformID);
	StringType GetCimeliaTransformElementAttribInfo(int iTransformID);
	//开启秘笈
	virtual int	CheckUseItemOpenSkillBook(GameLogic::LC_ActorBase* pkScriptCaller, int32_t iID);

	int64_t			GetSubordinateTransformCombatScoreByItemId(int iItemId);//通过道具id查进阶线皮肤战斗力
	void	   ClickOnScreen(int posX,int PosY);
	void	   FilterCronTimestampSameDay(const StringType& cronStr, mem::vector<uint32_t>& vecTime);
	StringType GetNextActiveGameStoryTimeSlot(long lStoryID);
	//serverunionid
	int32_t	   GetServerUnionID();

	void	   PlayVipLevelupFx(int iVipLevel);
	void	   PlayVipNewLevelupFx(int iVipLevel);

	bool	   PlayArenaTimerFx();
	bool	   PlaySkyArenaTimerFx(bool bBig);

	void	   SetLockLevel(int level);
	void	   LockLockedLevel(bool IsLocked);

	StringType GetPeakArenaLevelByScore(int iScore);
	combat_score_type		   GetPeakArenaSectionScore(combat_score_type iScore);
	StringType GetPeakLevelAttr(int level);
	int64_t	   GetPercentAttrCombatScore(double hpPercent, double ackPercent, double defPercent);

	//获取字符串的MD5值
	StringType GetStringMd5(const StringType& str);

	StringType GetBoneSoulSkillIconAndTip(int iLevel, int iIndex, int iStar);
	StringType GetBoneSoulCost(int iLevel, int iCount);

	bool	   IsItemMelt(int16_t meltType,int lItemID);
	int64_t	   GetDateTimeStamp(int iSecond, int iMinute, int iHour, int iDay, int iMonth, int iYear);

	int32_t	   CanStarsRune(int iCatagory,int iIndex);
	StringType GetStarsRuneCost(int iCatagory,int iIndex);

	//武林棋阵
	int		   GetChessMatrixActiveScheduleID();
	int		   ScriptActionExecute(const StringType& cronStr,int value);

	//核芯
	StringType GetCoreInfo();
	StringType GetCoreAttribs(int iID, bool bPercent);
	StringType GetCommanderTitleAttribs(int iID, bool bPercent);
	StringType GetCommanderTitleName(int iID);
	bool IsCommanderTitleCanShow(int iID);
	StringType GetCoreGoalInfo();
	StringType GetStigmasUnlockInfo(int iID);

	StringType	GetDrawCardRewardDisplay(int iShopType,int iScheduleID);
	//屏蔽字
	StringType	FindAndFileterForbiddenText(const StringType& sInfo, int eType);

	//显示xml ui
	void ShowGameUIByKeyName(const StringType& sKeyName);
	void ShowGameUIByKeyNameAndPath(const StringType& sKeyName, const StringType& sPath);
	void DestroyGameUI(GUISys::WT_Widget* pkWgt);
	int GetBackPackEmptyEntry(int iPackType);

	bool IsBoss(object_id_type lCharType);
	StringType GetItemEffectValueStr(int TypeID, int iValue);

	//三角色相关
	StringType GetBuddyInfo(int iCid);
	StringType GetBuddyInfoList();
	StringType GetBuddyUpgradeInfo(int iCid, int iLevel, int iStar);
	int		   GetBuddyGridUnlockLevel(int index);
	StringType GetBuddyTalent(int iCid);
	int GetBuddyMaxLevel(int iCid, int iStar);
	StringType GetBuddyAttributeShow(int iCid, int iStar);
	int GetBuddySpareItemID(int iCid, int iStar);
	int		   GetActiveSkillItemID(int iSkillID);
	StringType GetBuddyChainOpenInfo();

	StringType GetGuildLevelRewards();
	void	   PlayFailTaskUeFx();
	void	   PlayTaskChapterOpenFx(int iChapter);
	int		   GetMapIDByStoryID(int storyID);
	StringType GetTestDataBaseByIndex(int index);
	int		   GetDELTodayEndTimeStamp();
	bool	   IsTimestampEndToday(int64_t iTimeStamp);
	int		   GetDeltaDay(uint32_t timeA, uint32_t timeB);
	int		   GetSevendaysDetailCidByIndex(int index);
	int			GetMapLeaveCountDown(int InMapId);
	StringType	GetTreasureAddAward(int iId,int iLevel);

	int			GetEntrustTaskStarLevel(int iTaskId);
	//星魂相关
	StringType GetRuneInfo(int iItemID);
	StringType GetLevelUpRuneCost(int iLevel);
	int GetRuneCommanderLevelLimit(int iLevel);
	bool GetIsRuneSkillUp(int iLevel);
	StringType GetRuneCollectionInfo();
	StringType GetRuneAttributes(int itemID, int iEquipLevel, int iStarLevel);
	int GetRuneMaxLevel(int iStarUpType, int iStarLevel);
	StringType GetRuneStarInfo(int iStarUpType, int iStarLevel);
	int GetRuneStarQuality(int iStarUpType, int iStarLevel);
	int GetEquipStarQuality(int iStarUpType, int iStarLevel);
	int GetRuneEquipNxtLevelOfAffiliatedSkill(int iEquipLevel, int iSkillLevelID);
	int GetRuneEquipSkillIDByItemIDAndItemLevel(int itemID, int itemLevel);
	StringType GetRuneMaxLevelInfo(int iItemID);
	int GetBuddyShowTime(int iCID);
	StringType GetCharShowRewards(int iCID);
	
	StringType GetBuyRareSubInfo();
	StringType GetMustBuyInfo();
	StringType GetGuildBossRankReward(int iType);
	StringType GetRestraintJob(int iID);
	int					GetRaidGloryChapterNum();
	int					GetVipNewMaxNum();
	int					GetBestGuildItemLinkID(int id);

	bool IsScheduleValid(int iScheduleID);
	StringType			GetGameDir();
	//主题活动
	StringType			GetThemeMainActivityInfo();
	StringType			GetThemeSubActivityInfo(int iId);
	StringType			GetACTShopAInfo(int iId);
	StringType			GetThemeActivityRaidBossRankRwd(int iActivityId);
	StringType			GetThemeRaidBossType(int iActivityId);
	int					GetThemeRaidBossTypeSchedule(int iActivityId);

	const char*			GetTimeStrByTimeStamp(uint32_t iTimeStamp);

	int					GetChargeRate(int iType);

	int					GetChargeProfitCashType(int iType);

	StringType			GetMailText(int iId);

	//机器人表
	int					GetRobotScore(uint64_t iId);
	int					GetRobotHead(uint64_t iId);
	int					GetRobotLevel(uint64_t iId);
	StringType			GetRobotName(uint64_t iId);
	bool				IsSoloduleRobotPlayerId(uint64_t iId);

	StringType			GetDecodeScore(uint64_t nInput, bool bTime);

	//转成wstring,再取substring
	StringType			GetSubCharString(const StringType& sContent, int iBegin, int iEnd);
	//转成wstring 获取size
	int					GetCharCount(const StringType& sContent);

	int32_t				GetOpenTimeByID(int ID);
	int32_t				GetUnLockTimeByID(int ID);

	//新手帮助
	StringType			GetTutorialRewardInfo();

	//苍穹奖励 
	StringType          GetSkyArenaRankRewardInfo();
	StringType			GetSkyArenaRankInfo();

	StringType			GetSpecialTaskInfo();

	//节日活动
	StringType			GetHolidayMainActivityInfo();
	StringType			GetHolidaySubActivityInfo(int iId);
	int                 GetHolidayActivityTaskScheduleId(int iHolidayActivityTaskType);
	StringType          GetHolidayActivityTaskTabNames(int iHolidayActivityTaskType); 

	//巅峰战场赛事名称
	StringType			GetSummitArenaMatchName(int iID);
	//巅峰战场排名奖励
	StringType			GetSummitArenaRankReward();

	//竞猜赌注
	StringType			GetGuessCost(int ID);
	StringType			GetGuessProfit(int ID);
	//荣耀巅峰榜
	int					GetHight32(int64_t value){return int(value>>32);};
	int					GetLow32(int64_t value){return int((value<<32)>>32);};
	//BuddyThemeTaskType
	StringType			GetThemeActivityTaskTabNames(int iBuddyThemeTaskType);
	int					GetThemeActivityTaskScheduleId(int iBuddyThemeTaskType);

	StringType			GetCustomCommandList();

	//获得前置事件ID
	int					GetPreStoryId(int storyId);
	int64_t				GetNextStoryTime(long lStoryID);//获得下一次开启时间

	//假日活动拼图任务
	StringType GetHAPTTaskInfo();
	StringType GetHAPTReward();
	void GetTaskInfoStringById(int iTaskID,StringType& OutStr);
	void GetAchievementInfoStringById(int iAchievementID ,StringType& OutStr);

	//节日活动连冲返利 
	StringType GetContinousChargeInfo(); 

	/*变强系统
	***英雄,宝具,装备,幻兽,萌宠,背饰,玩具
	***英雄，宝具，装备的分数不是战斗力，通过promotecombat表算出来的评分
	*/
	void CalculateBeBetterMaxScores();								//总值在等级发生变化时需重新计算

	//变强系统-英雄-总值--------------------------------------
	combat_score_type GetAllHeroLevelMaxScore();
	combat_score_type GetAllHeroStarMaxScore(){return m_AllHeroStarScore;};
	combat_score_type GetAllHeroSkillMaxScore(){return m_AllHeroSkillScore;};
	int GetAllHeroCount();
	//变强系统-英雄-当前值	
	combat_score_type GetOwnedHeroLevelScore();
	combat_score_type GetOwnedHeroStarScore();
	combat_score_type GetOwnedHeroSkillScore();
	int GetOwnedHeroCount();
	//变强系统-英雄-初始化
	void CalculateAllHeroLevelMaxScore();
	void CalculateAllHeroStarMaxScore();
	void CalculateAllHeroSkillMaxScore();
	//变强系统-宝具-总值--------------------------------------
	combat_score_type GetAllRuneLevelMaxScore(){return m_AllRuneLevelScore;};
	combat_score_type GetAllRuneStarMaxScore(){return m_AllRuneStarScore;};
	int GetAllRuneCount();
	//变强系统-宝具-当前值	
	combat_score_type GetOwnedRuneLevelScore();
	combat_score_type GetOwnedRuneStarScore();
	int GetOwnedRuneCount();
	//变强系统-宝具-初始化
	void CalculateAllRuneLevelMaxScore();
	void CalculateAllRuneStarMaxScore();

	//变强系统-装备-总值--------------------------------------
	combat_score_type GetAllEquipMaxScore(){return m_AllHeroEquipScore;};
	combat_score_type GetAllEquipLevelMaxScore(){return m_AllHeroEquipLevelScore;};
	combat_score_type GetAllEquipStarMaxScore(){return m_AllHeroEquipStarScore;};
	//变强系统-装备-当前值	
	combat_score_type GetOwnedEquipScore();
	combat_score_type GetOwnedEquipLevelScore();
	combat_score_type GetOwnedEquipStarScore();
	//变强系统-装备-初始化
	void CalculateAllEquipMaxScore();
	void CalculateAllEquipLevelMaxScore();
	void CalculateAllEquipStarMaxScore();	
	
	//变强系统-玩具-总值--------------------------------------
	combat_score_type GetAllToyPetMaxScore(){return m_AllToyPetScore;};
	int GetAllToyPetCount();
	//变强系统-玩具-当前值	
	combat_score_type GetOwnedToyPetScore();
	int GetOwnedToyPetCount();
	//变强系统-玩具-初始化
	void CalculateToyPetMaxScore();

	//变强系统-幻兽-总值--------------------------------------
	combat_score_type GetVehicleAttrLevelMaxScore(){return m_VehicleAttrLevelMaxScore;};
	combat_score_type GetVehicleSkinMaxScore(){return m_VehicleSkinMaxScore;};
	combat_score_type GetVehicleEquipMaxScore(){return m_VehicleEquipMaxScore;};
	combat_score_type GetVehicleDrugMaxScore(){return m_VehicleDrugMaxScore;};
	//变强系统-幻兽-当前值	
	combat_score_type GetOwnedVehicleAttrScore();
	combat_score_type GetOwnedVehicleSkinScore();
	combat_score_type GetOwnedVehicleEquipScore() { return _getOwnedCimeliaEquipScore(MountPack); }	//幻兽当前装备战力
	combat_score_type GetOwnedVehicleDrugScore();
	
	//变强系统-幻兽-初始化
	void CalculateVehicleAttrMaxScore();
	void CalculateVehicleSkinMaxScore();
	void CalculateVehicleEquipMaxScore();
	void CalculateVehicleDrugMaxScore();

	//变强系统-萌宠-总值--------------------------------------
	combat_score_type GetDevaEyeAttrLevelMaxScore(){return m_DevaEyeAttrLevelMaxScore;};
	combat_score_type GetDevaEyeSkinMaxScore(){return m_DevaEyeSkinMaxScore;};
	combat_score_type GetDevaEyeEquipMaxScore(){return m_DevaEyeEquipMaxScore;};
	combat_score_type GetDevaEyeDrugMaxScore(){return m_DevaEyeDrugMaxScore;};
	//变强系统-萌宠-当前值	
	combat_score_type GetOwnedDevaEyeAttrScore();
	combat_score_type GetOwnedDevaEyeSkinScore();
	combat_score_type GetOwnedDevaEyeEquipScore(){ return _getOwnedCimeliaEquipScore(PetPack); }	//萌宠装备加成战力当前值
	combat_score_type GetOwnedDevaEyeDrugScore();
	//变强系统-萌宠-初始化
	void CalculateDevaEyeAttrMaxScore();
	void CalculateDevaEyeSkinMaxScore();
	void CalculateDevaEyeEquipMaxScore();
	void CalculateDevaEyeDrugMaxScore();

	//变强系统-背饰-总值--------------------------------------
	combat_score_type GetWingAttrLevelMaxScore(){return m_WingAttrLevelMaxScore;};
	combat_score_type GetWingSkinMaxScore(){return m_WingSkinMaxScore;};
	combat_score_type GetWingEquipMaxScore(){return m_WingEquipMaxScore;};
	combat_score_type GetWingDrugMaxScore(){return m_WingDrugMaxScore;};
	//变强系统-背饰-当前值	
	combat_score_type GetOwnedWingAttrScore();
	combat_score_type GetOwnedWingSkinScore();
	combat_score_type GetOwnedWingEquipScore(){ return _getOwnedCimeliaEquipScore(WingPack); }	//背饰装备加成战力当前值
	combat_score_type GetOwnedWingDrugScore();
	//变强系统-背饰-初始化
	void CalculateWingAttrMaxScore();
	void CalculateWingSkinMaxScore();
	void CalculateWingEquipMaxScore();
	void CalculateWingDrugMaxScore();
	
	//变强系统-指挥官装备-总值--------------------------------------
	combat_score_type GetCommanderEquipAttrMaxScore(){return m_CommanderEquipMaxScore;};
	combat_score_type GetCommanderEquipStrengthenMaxScore(){return m_CommanderEquipStrengthenMaxScore;};
	combat_score_type GetCommanderEquipStarMaxScore(){return m_CommanderEquipStarMaxScore;};
	//变强系统-指挥官装备-当前值	
	combat_score_type GetOwnedCommanderEquipAttrScore();
	combat_score_type GetOwnedCommanderEquipStrengthenScore();
	combat_score_type GetOwnedCommanderEquipStarScore();
	//变强系统-指挥官装备-初始化
	void CalculateCommanderEquipAttrMaxScore();
	void CalculateCommanderEquipStrengthenMaxScore();
	void CalculateCommanderEquipStarMaxScore();

	//藏宝图
	StringType GetTreasureItemInfo();

	//总值 总值从所有英雄变成已拥有，并且按当前指挥官等级来计算
	combat_score_type m_AllHeroLevelScore;
	combat_score_type m_AllHeroStarScore;
	combat_score_type m_AllHeroSkillScore;
	combat_score_type m_AllRuneLevelScore;
	combat_score_type m_AllRuneStarScore;
	combat_score_type m_AllHeroEquipScore;				
	combat_score_type m_AllHeroEquipLevelScore;			
	combat_score_type m_AllHeroEquipStarScore;	
	combat_score_type m_AllToyPetScore;
	//进阶线 幻兽
	combat_score_type m_VehicleAttrLevelMaxScore;
	combat_score_type m_VehicleSkinMaxScore;
	combat_score_type m_VehicleEquipMaxScore;
	combat_score_type m_VehicleDrugMaxScore;
	//进阶线 萌宠
	combat_score_type m_DevaEyeAttrLevelMaxScore;
	combat_score_type m_DevaEyeSkinMaxScore;
	combat_score_type m_DevaEyeEquipMaxScore;
	combat_score_type m_DevaEyeDrugMaxScore;
	//进阶线 背饰
	combat_score_type m_WingAttrLevelMaxScore;
	combat_score_type m_WingSkinMaxScore;
	combat_score_type m_WingEquipMaxScore;
	combat_score_type m_WingDrugMaxScore;
	//指挥官装备
	combat_score_type m_CommanderEquipMaxScore;				
	combat_score_type m_CommanderEquipStrengthenMaxScore;			
	combat_score_type m_CommanderEquipStarMaxScore;	

	//当前值--------------------------------------
	//进阶线 幻兽 这几个在别的里用到
	combat_score_type m_OwnedVehicleAttrLevelScore;
	//进阶线 萌宠
	combat_score_type m_OwnedDevaEyeAttrLevelScore;
	//进阶线 背饰
	combat_score_type m_OwnedWingAttrLevelScore;

	combat_score_type m_EquipAttrCurScore;
	/*
	***************变强系统结束**************
	*/

	//开启调试界面相关接口
	//当前内存中贴图数量、列表，文件大小 IG_MaterialManager
	StringType GetDebugMaterialListInfo();

	//当前内存中模型数量 GD_ModelContentManager
	//当前当前内存中特效数量 FX_FxManager
	//当前特效内存池数量、列表 FX_ClientFxManager

protected:
	void _changeAvatarItem(GD_ClientActorModel *pkActorModel,int iChangeType,int lItemID,int lCharType);
	void _selectWatchedTarget(object_id_type* alTargetList, int iTargetListSize, 
			int& iSelectTargetIndex, int& lUpdateTargetListTime, const object_id_vector& rkSurroundTargetVector);

	int _getAutoShortcutKeyInOpenLevelBySkillID(int lSkillID);
	StringType _getCimeliaLevelInOpenLevelBySkillID(int iSkillID);

	bool _isLeapYear(int iYear);	//是否为闰年
	StringType _getCalendarString(int iYear, int iMonth, int iDate);	//获取某日所在月份的日历信息字符串

	//装备随机属性范围字符串
	StringType _makeItemEffectRangeDesc(int TypeID, int iBaseValue, int iBonusValue);
	StringType _makeAttribString(LC_BackPackEntry *pkEntry, const SortItemEffect &kEffctInfo, bool bRefine=true);
	int _getAttribCount(int iItemID);

	combat_score_type _getCombatScoreStandard(int iLevel, int iIndex);
	static bool _sortFunc(const SortItemEffect &v1, const SortItemEffect &v2);
	static bool _equipAttribSortFunc(const SortItemEffect &v1, const SortItemEffect &v2);

	int _getRefineAttribVipLimit(const CSVFile::CF_VIPProfit::DataEntry *pkVipProfit);
	bool _isItemAttributeValid(int iItemOpCode, int iAttribIdx);

	StringType _getRewardInfo(LC_ClientGamePlayer* pkGamePlayer, const Utility::UT_SIMDataList &ItemList, bool bProbability = false, int32_t iChoice = 0);
	StringType _getChoiceInfo(LC_ClientGamePlayer* pkGamePlayer, const Utility::UT_SIMDataList &ItemList);

	StringType _getBuddyCsvInfo(const CSVFile::CF_Buddy::DataEntry *pkEntry);
	int _getBuddyAttrib(int iBase, int iLevel, int iRate);

	void		 _implShotBullet(GameLogic::LC_ActorBase* pkActor, ShotBulletArg & arg);
	void		 _createLocalBullet(GameLogic::LC_ActorBase* pkActor, const ShotBulletArg & arg, 
	unsigned int iBulletClientID, const UT_Vec3Int & kShotRay, const Utility::UT_PickResult & kPickResult);
	//<< 发射子弹

	//圣痕(Skillbook)
	StringType _getAttribString(LC_ActorAttributeMap *attrMap);
	StringType _getPercentAttribString(LC_ActorAttributeMap *attrMap);

	//圣痕
	void _getSkillBookAttribs(LC_ActorAttributeMap* attrMap, int iSkillBookID, int iPos, int iLevel);

	//指挥官装备
	int _getCommanderEquipAttribID(const CF_EquipCommanderSlotReinforce::DataEntry* pkEntry, int iEquipSlotIdx);

	//进阶线装备战力
	combat_score_type _getOwnedCimeliaEquipScore(int iPackType);
private:
		mem::vector<UT_Vec3Int> m_kLocVec;
		LC_BackPackEntry		m_TmpBackPackEntry[4];
		LC_BackPackEntry		m_TmpItemCompositionPackEntry;
};

#endif