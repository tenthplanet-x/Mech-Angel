//�ͻ���ר�ýű��ӿ�
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

	//��ȡ��ǰ��Ϸʱ��
	float								GetGameTime();
	float								GetGameDeltaTime();
	double								GetProfileTimeStamp();
	StringType							GetCalendarToday(void);
	StringType							GetTimeStamp(time_type iBeginTime, time_type iEndTime);
	StringType							GetTimeString(time_type iBeginTime, int iType = 1);
	StringType							GetCurrentDayTime();
	int									GetCurrentSystemTime(){ return GET_SYSTEM_TIME;};
	int64_t								GetActivityScheduleDeltaTimeStamp(int iScheduleID);
	int									GetActivityScheduleStat(int iScheduleID);// -1 ����ǰ 0 ������ 1 ������

	//��ȡ��¼��ʽ
	int									GetLoginType();

	//��ȡ��Ϸ״̬
	int									GetGameStage();

	//��ȡ��ǰ���
	LC_ClientGamePlayer*				GetPlayerPtr();

	LC_ClientTaskManager*				GetTaskMgr();

	LC_ClientNetPlayer*					GetNetPlayerPtr(unique_id_type lCharID);
	LC_ClientNetPlayer*					GetNetPlayerPtrByPlayerID(object_id_type lPlayerID);
	LC_ClientNPC*						GetNPCPtr(object_id_type lNPCID);
	LC_ClientPlaceHolder*				GetChestPtr(object_id_type lPlaceHolderID);
	LC_ClientTask*						GetTaskPtr(int lTaskID);
	int									GetNeighborPlayerCount();
	int									GetTrueNeighborPlayerCount();//�õ���������Ҳ�����
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



	//�����½�
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

	//��������Ҫ��NetPlayerList�в���NetPlayer
	int									GetNetPlayerCount();
	LC_ClientNetPlayer*					GetNetPlayerByIndex(int iIndex);

	//��ȡNPC
	int									GetNPCCount();
	LC_ClientNPC*						GetNPCByIndex(int iIndex);

	//��һس�
	virtual void						ReturnToCity(GameLogic::LC_Attacker* pkScriptCaller);

	//��ȡ��ǰ�۲��Ŀ��
	object_id_type								GetWatchedTargetID();

	//��ȡ��ǰ������Ŀ��(ֻ�����ѡ�еģ�
	object_id_type								GetPreLockedTargetID();

	//���õ�ǰ����Ŀ��							
	void										SetLockedTargetID(object_id_type lID);

	//��ֹ�л�Ŀ��
	void										FreezeLockTargetID(bool flag);
	bool										IsFreezeLockTargetID();

	//����������������б�
	void										CleanChangeLockedFlag();

	//��ȡ��ɫ�б��е�ͼ��
	bool								GetCharShowRank( int lCharTypeID );
	bool								GetIsWildBoss( int lCharTypeID );
	int									GetElementDamageType( int lCharTypeID );

	//��ȡ����ͼ��
	StringType							GetSkillIconPath(int lSkillID);
	//��ȡ��������
	StringType							GetSkillUseResult(int lSkillID);

	//��ȡ������Ҫ�ľ���
	int64_t								GetLevelExp(int lLevel);
	int64_t								GetBuddyLevelExp(int lLevel);

	//��ȡ���������Ǯ
	int								GetChangeServerCash(int lLevel);

	//��ȡ���ﵱǰ�����������
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

	//���ָ��ID�����Ҫ�Ľ�Ǯ
	int									GetYuanbaoOriginalPrice(int iShopID);
	//���ָ��ID�����Ҫ�Ľ�Ǯ
	int									GetYuanbaoCurPrice(int iShopID);
	//��ü۸񲻶����̵�ID�´ι�����Ҫ�Ļ������ͺ�����
	StringType							GetAccessWayBuyPrice(int iShopID);
	//����shopId��ȡ��Ӧ����Ʒid
	int									GetItemidInShopList(int iShopID);
	//����shopId��ȡ��Ӧ����Ʒid��count
	StringType							GetItemInShopList(int iShopID);

	//��ÿ����
	GameLogic::LC_ShortcutBar*		   GetShortcutBarPtr();

	//���NPC���ϵ��̵�
	GameLogic::LC_ShopBase*			   GetNPCShopPtr(int lNpcCharTypeID,int iShopType);

	//���NPC���ϵ���վ
	GameLogic::LC_TransferBase*		   GetNPCTransferPtr(object_id_type lNPCID);
	
	//��ü�����
	GameLogic::LC_SkillCoolDownMap*		GetSkillCoolDownMap();

	//�����ƷCD��
	GameLogic::LC_ItemCoolDownMapBase*	GetItemCoolDownMap();

	StringType						GetNpcConversationHeadIcon(int lConversationID, int iCurSeq);

	//�����������
	StringType						GetLogicAreaName(int lMapID, int lAreaID);
	UT_Vec3Int&						GetLogicAreaSpawnPos(int lMapID, int lAreaID);

	int							   GetItemID(const StringType& kItemName);
	int							   GetItemCategoryID(int lItemID);
	
	int							   GetDakTransferAreaNum();
	int							   GetTransferMapID(int iIndex);
	int							   GetTransferAreaID(int iIndex);
	ResultType						   CheckCanUseItem(int lItemID, int iParam);

	//���������
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
	*��¼����
	*/	

	void							OnGotoUserLogin();									//�����¼����
	void							OnGotoUrl(const StringType& rkUrl);					//ǰ����վ
	void							OnshowWebWindow(const StringType& rkUrl, int width, int height);
	void							ShowShunWangWebWindow(const char* rkUrl, const char* rkJumpUrl, int width, int height);
	void							OnHideWebWindow();
	void							OnResetWebWindow(int width, int height, int x, int y);

	bool							ScriptJzCmdlineHasValue(const StringType& argument);
	StringType						ScriptJzCmdlineGetValue(const StringType& argument);
	
	//�����û����������
	void							OnCheckForceLogin();
	void							OnUserLogin(const StringType& strUser, const StringType& strPwd, bool bForceLogin, bool bRemember);//��¼LoginServer
	//ѡ�����������
	void							OnGotoSelServer();
	void							SelectLoginZone(int iServerID);
	void							OnGotoSelChar();									//�����ɫѡ�����
	void							OnQuitLogin();										//�˳���¼LoginServer
	int								GetLastLoginServerID();								//�ϴε�¼�ķ�����ID
	void							SaveLastLoginServerID(int iServerID);		//�����ϴε�¼�ķ�����ID
	unique_id_type					GetLastLoginCharID();
	void							SaveLastLoginCharID(unique_id_type iCharID);
	void							SaveAutoCommitTeamRequest(int iAuto);				//�����Ƿ��Զ������������
	int								GetAutoCommitTeamRequest(void);						//�Ƿ��Զ������������
	void							SaveAutoCommitTeamInvite(int iAuto);				//�����Ƿ��Զ������������
	int								GetAutoCommitTeamInvite(void);						//�Ƿ��Զ������������
	void							SaveShowBattleBuddyEquip(int m_iShow);				//�����Ƿ�ѡ����ʾ����Ӣ�۵�װ�����
	int								GetShowBattleBuddyEquip(void);						//�Ƿ�ѡ����ʾ����Ӣ�۵�װ�����
	void							SaveShowBattleBuddyRune(int m_iShow);				//�����Ƿ�ѡ����ʾ�����ñ��ߺ��
	int								GetShowBattleBuddyRune(void);						//�Ƿ�ѡ����ʾ�����ñ��ߺ��
	
	//ѡ���ɫ����
	void							OnGotoCreateChar(void);								//�����ɫ��������
	void							OnEnterGame(unique_id_type iPlayerID);				//������Ϸ����
	void							OnLoadingToGameStage();
	int								GetPreStateType();
	bool							PlayVideo(int iVideoType, const StringType& kVideoName);		//������Ƶ
	void							EndVideo(int iVideoType);										//��ֹ��Ƶ����
	//������ɫ����
	bool							OnCreateChar(										//�����½�ɫ
										const StringType& strCharName, int iCid, 
										int lHeadEquipID, int lBodyEquipID, int lFootEquipID, 
										int lWeaponID, int iWeaponType, int lServerID);
	int								GetCreateCharServerID();

	//��λ����ս������ȷ��
	void							OnLowScoreConfirm(combat_score_type iCurSocre, combat_score_type iNeedScore, int iParam);

	StringType						GetNamePwdFromConfig();								//�������ļ��л�ȡ�û���������
	void							SaveNamePwdToConfig(const StringType& kName, const StringType& kRemember);//���û������浽�����ļ���
	StringType						GetInfoFromConfig(const StringType& strTite, const StringType& strKey);	//�������ļ��л�ȡ��Ϣ
	void							SaveInfoToConfig(const StringType& strTite, const StringType& strKey, const StringType& kValue);//���浽�����ļ���

	bool							GetSuperShopCustomizeTodayPrompt(const StringType& strTitle);//�������԰���ս������Ƿ��Ѽ�¼

	StringType						GetLoginServerIDList(int iZoneID);			//��ȡLoginServer�б�
	StringType						GetLoginZoneIDList();
	StringType						GetLoginServerInfo(int iZoneID, int iIndex);					//��ȡLoginServer��Ϣ
	StringType						GetLoginZoneInfo(int iZoneID);
	
	int								GetGameServerCount();								//��ȡGameServer����
	StringType						GetGameServerList(bool bIncludeOffServer);			//��ȡGameServer�б�
	StringType						GetGameServerInfo(int iServerID);					//��ȡGameServer��Ϣ
	StringType						GetGameServerName(int iServerID);
	int								GetCurGameServer();									//��ȡ��ǰѡ�е�GameServer��Ϣ
	int								GetCurGameServerNode();

	int								GetUserCharCount();									//��ȡÿ���û����½�ɫ������
	StringType						GetCurCharList();									//��ȡ��ǰ���н�ɫ�б�
	StringType						GetCharInfo(unique_id_type iCharID);							//��ȡָ����ɫ����Ϣ
	StringType						GetParamForCreateChar();						//��ȡ������ɫ��Ҫ����Ϣ

	void							RotateChar( int lCharType,int iAvatarIndex,float fAngle);//��ת��ɫ���������Ϊ�Ƕȣ����ǻ��ȣ�
	void							RotateAvatarModel(long lCharType, int iIndex, int iCameraMode, float fRotAngle);
	void							SetModelRotation(long lCharType, int iIndex, int iCameraMode, float fRotAngleX, float fRotAngleY, float fRotAngleZ);
	void							SetShaderConstF(const StringType& strCharName, float v);
	float							GetShaderConstF(const StringType& strCharName);
	bool							UpdateAvatarBloomParams(int lCharType);

	void							DeteachAllAttachFxAnim(GameLogic::LC_ActorBase* pkActor);

	void							SendReqServerListMsg();								//��������������б���Ϣ
	bool							WorldMapLoaded();									//�жϵ�ǰ3d�����Ƿ��Ѿ��������

	void							ConfirmAction(int iActionType, int lParam);		//��ʾ�û�ȷ�϶�����ִ����ض���

	void							StartCountDown(bool bLeaveGame);
	void							CancleCountDown();
	void							RequestLeaveGame();				//�˳���Ϸ
	void							RequestResetGame();									//�˳���Ϸ��ѡ�˽���
	void							RequestReLoginGame();								//�˳���Ϸ����¼����
	void							ResetUINameArrage();								//������������

	void							RequestRandomPlayerName(int iSex);					//����������� �Ա�(0: Ů 1: ��)

	//����״̬
	int								GetGameNetState();

	//����
	void							RequestRespawnOnCurrentLocation(bool bAuto = false);
	void							RequestRespawnOnSafeLocation();
	void							RequestRespawnByItem();
	bool							CheckCanRespawnByItem();

	bool							CanRespawnOnSafe(int mapResID);
	long							GetRespawnProtectTime(int mapResID);
	StringType						GetEnterMapItemInfo(int mapResID);

	//�����˸���
	void							RobotRequestRespawnOnCurrentLocation(GameLogic::LC_ActorBase* pkMachineOwner);
	//������
	void							Change2NextPos(int nPosX, int nPosY, int nPosZ);
	bool							IsSolodule_Robot_PlayerId(uint64_t uiID);//�ж��Ƿ��ǻ�����
	//npc �������ݽӿ�
	virtual void					StampASpecialBubble(GameLogic::LC_ActorBase* pkActor, const StringType& kBubbleMsg, int lShownTime);
	//����һ��ActorBase ID�����Ļ����
	void							UpdateUIScreenPosByLogicObject(GUISys::WT_Widget* pkWidget,uint64_t lObjectID,int iXOffset,int iYOffset,int iWidth,int iHeight);

	object_id_type					GetLogicIDByCharType(int lCharTypeID);
	object_id_type					GetChestIDByCharType(int lCharType);

	void							ChangeLockedTarget();

	//����
	//void							RequestLevelUp();

	long							GetSkillUpGradeNeedCash(long lSkillID);
	int								GetSkillUpgradeNeedBuddyStar(long lSkillID);
	//�����ܷ�����
	bool							GetSkillCanUpGrade(int lSkillID, int iCID);
	//�����ܷ�ͻ��
	bool							GetSkillCanWakeUp(int lSkillID, int iCID);

	virtual int						CheckActivateSkill(GameLogic::LC_Attacker* pkScriptCaller, int lSkillTypeID);
	virtual void					NotifyTreasureSkillInterrupt();

	//��ù���ȼ�����
	GameLogic::NPCLevelType			GetNPCLevelType(object_id_type lNPCID);
	GameLogic::NPCLevelType			GetNPCLevelTypeByCharType(int iCharType);
	//��ù��ﾫӢ����
	GameLogic::NPCEliteType			GetNPCEliteType(object_id_type lNpcID);

	//����
	bool							CanGetTaskItem(int lItemID);
	bool							IsTaskInCurrentArea(int lTaskID);
	int							GetItemEquipLimit(int lItemID);
	//��ѵ
	int							GetGetSpecialTrainingCount();
	//ǿ��
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
	StringType					GetEnhanceEquipLevelProperty(int iEqupSlotIdx, int iEquipReinforceLevel );		// װ��ǿ�����⼶���ṩ����ǿ���ԣ������
	StringType					GetBreakEquipDescription(int iEqupSlotIdx, int iEquipBreakLevel );		// װ��������Ҫ�����ģ������
	StringType					GetEnhanceEquipCost(int iEqupSlotIdx, int iEquipReinforceLevel );		// װ��ǿ����Ҫ�����ģ������
	int							GetEnhanceEquipNeedLevel(int iEquipReinforceLevel );					// װ��ǿ����Ҫ�Ļ��ȼ��������
	int							GetEnhanceEquipNeedEquipLevel(int iEquipReinforceLevel);
	int							GetEquipSlotMaxLevel();
	StringType					GetBreakEquipLevelProperty(int iEqupSlotIdx, int iEquipStar);
	int							GetBreakEquipPropertySpecialChange(int iEqupSlotIdx, int iEquipStar);
	StringType					GetBreakEquipProperty(LC_BackPackEntry* pkEquipEntry, int iEqupSlotIdx, int iEquipStar);
	StringType					GetItemBaseAttributes(int iItemID);
	int64_t						GetEquipSlotScore(int iEqupSlotIdx, int iEquipReinforceLevel, int iEquipStar);			//װ������ս��������ǿ�������ǣ�������װ������ս��

	//ָ�ӹ�װ��
	StringType					GetCommanderReinforceEquipProperty(int iEqupSlotIdx, int iEquipReinforceLevel );		// װ��ǿ�����⼶���ṩ����ǿ���ԣ������
	StringType					GetCommanderReinforceEquipCost(int iEqupSlotIdx, int iEquipReinforceLevel );			// װ��ǿ����Ҫ�����ģ������
	int							GetCommanderReinforceEquipNeedLevel(int iEquipReinforceLevel );							// װ��ǿ����Ҫ��ָ�ӹٵȼ��������
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

	//����׷�����ٹ�Զ��������Ϣ
	StringType					GetGloryRewardLinkInfo(int iTransRuleID);

	//��װ�ռ�
	StringType					GetCommanderEquipSuitCollectInfo();

	//��Դ�һ�
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

	//����
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
    //���
	bool                            CanContinueCookDish();
	void                            RequstInterruptCooking();
	//����ѵ��
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
	//�Ƿ�ΪNpc���ǹ���
	bool							IsNpc(object_id_type iObjId);

	//���νӿ�
	void							SwitchShieldVisibility(int iIndex, bool visiable);
	
	bool							IsIMEOpen();	

	// �ʼ�ȷ��
	void							ConfirmSendMail(bool bPayFlag, const StringType& kName, const StringType& kCash);	
	// ����ȷ��
	void							ConfirmExitGuild();
	void							ConfirmDismissGuild();

	combat_score_type				GetSkillStateCombatScoreBySkillID(int SkillID);
	StringType						GetSkillStateUseResult(int iID);
	StringType						GetSevenDaysRewardInfo(int iID);

	// ʹ����Ʒȷ��  
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


	/* ----------------  Begin    ������ֵ ---------------------------*/
	virtual void ModifyTargetAttributeByValue(GameLogic::LC_Attacker* pkScriptCaller,int iTargetIndex,int iAttributeType,int64_t iDeltaValue,bool bRevert){};
	virtual void ModifyTargetAttributeByPercent(GameLogic::LC_Attacker* pkScriptCaller,int iTargetIndex,int iAttributeType,double fDeltaPercent,bool bRevert){};
	/* ----------------  End    ������ֵ ---------------------------*/


	
	/* ----------------  Begin    ���� ---------------------------*/
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

	//trick ֱ�Ӵ�����ֵ��ȥ�ߣ�û�취��
	virtual void				SetTargetLocationValue(GameLogic::LC_Attacker* pkScriptCaller, int posx, int posy, int posz);
	virtual void				SetClosestTargetLocation(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex);

	virtual void		SetAlphaDead(GameLogic::LC_Attacker* pkScriptCaller, bool kFlag);

	//���㼼���˺�
	/*
	iDamageTargetIndex :	�˺�Ŀ��
	lSkillTypeID :			��������
	iDamageRandomIndex :	�˺�������
	iAttackRateRandomIndex :��������
	iCriticalRateRandomIndex :��������
	lDamageSourceActorID :	���������ߵ�ID
	iHPCost				 :	���μ��ܵ�HP���ģ����ڼ����˺����ò�����ʱ����
	iMPCost				 :	���μ��ܵ�MP���ģ����ڼ����˺����������0���������Զ����㼼��MP���ģ�Ĭ��MP���ĺͶ������ģ�
	*/
	virtual void		GenerateSkillDamage(GameLogic::LC_Attacker* pkScriptCaller,int iDamageTargetIndex,	int lSkillTypeID,int iDamageRandomIndex, int iAttackRateRandomIndex,int iCriticalRateRandomIndex,object_id_type lDamageSourceActorID, int iHPCost, int iMPCost);
	virtual void		GenerateSkillDamageMultiple(GameLogic::LC_Attacker* pkScriptCaller,int iDamageTargetIndex, int lSkillTypeID,int iDamageRandomIndex, int iAttackRateRandomIndex,int iCriticalRateRandomIndex, object_id_type lDamageSourceActorID, int iHPCost, int iMPCost, int Multiple, int fPercent);
	virtual void		GenerateSkillDamageMainTarget( GameLogic::LC_Attacker* pkScriptCaller,int iDamageTargetIndex, int lSkillTypeID,int iDamageRandomIndex, int iAttackRateRandomIndex,int iCriticalRateRandomIndex,object_id_type lDamageSourceActorID, int iHPCost, int iMPCost,int fPercent );
	
	//��������˺�
	/*
	iDamageTargetIndex :	Ŀ��˵��
	lDamageLogicTypeID :	�����˺��ļ��ܱ��
	lDamageType :			�˺�������
	iDamage :				�˺�����
	lDamageSourceActorID :	�����˺��ߵı��
	*/
	virtual void		GenerateDamage(GameLogic::LC_Attacker* pkScriptCaller,int iDamageTargetIndex,int lDamageLogicTypeID,int lDamageType,int iDamage,object_id_type lDamageSourceActorID);

	//ʹ�ô������ָܻ�HP/MP���ظ���ɽ�����������
	//bool SitForRecovery(GameLogic::LC_ActorBase* pkMachineOwner);

	//��Ŀ���һ����Ч
    //virtual void AttachTargetGfx(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iDummyType, const StringType& strFxFileName);
	virtual void AttachClientTargetGfx(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iDummyType, const StringType& strFxFileName, bool bStayStatic);
	virtual void AttachClientTargetGfxSacle(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iDummyType, const StringType& strFxFileName, bool bStayStatic,float fScale);
	virtual void AttachClientTargetGfxWithDir(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iDummyType, const StringType& strFxFileName, bool bStayStatic, int eBindMode, float dirAngle, float posx,float posy,float posz);
	virtual void AttachClientTargetGfxWithDirScale(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iDummyType, const StringType& strFxFileName, bool bStayStatic, int eBindMode, float dirAngle, float posx,float posy,float posz, float scale);
	virtual void AttachClientTargetGfxWith3DScale(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iDummyType, const StringType& strFxFileName, bool bStayStatic, int eBindMode, float xAngle=0, float yAngle=0, float zAngle=0, float posx=0, float posy=0, float posz=0, float scale=1.0f);
	//��������Ŀ�����Ч
	virtual void AttachClientTargetPairGfx(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iDummyType1, int iPairTargetIndex, int iDummyType2, const StringType& strFxFileName);
	virtual void AttachClientTargetPairGfxLink(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iDummyType1, int iPairTargetIndex, int iDummyType2, const StringType& strFxFileName);

	virtual void AttachClientTargetBowGfx(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iDummyType, const StringType& strFxFileName, bool bStayStatic, int eBindMode, float fScale);
	virtual void AttachClientTargetBowGfxWithDir(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iDummyType, const StringType& strFxFileName, bool bStayStatic, int eBindMode, float xAngle, float yAngle, float zAngle, float posx,float posy,float posz, float fScale);	

	virtual int AttachClientTargetSound(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, const StringType& kFileName, int iSoundChannel, int iSoundType);
	virtual int	AttachClientTargetSoundCount(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, const StringType& kFileName, int iSoundChannel, int iSoundType, bool isLoop);
	virtual void StopClientTargetSound(GameLogic::LC_Attacker* pkScriptCaller, int SoundID, bool isFade, float fadeTime);
	virtual void UpdateClientTargetSoundPosition(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int soundID);
	//��ɫ����ϵͳ�� (���� 3D ����)
	virtual int AttachClientTargetVoice(GameLogic::LC_Attacker* pkScriptCaller, const StringType& kFileName, int iSoundChannel);
	static bool CheckAttachSkillSound(LC_ClientGamePlayer* pkGamePlayer, GameLogic::LC_Attacker* pkScriptCaller);
	
	virtual void AttachClientNPCSoundSeq(const StringType& kFileName);

	virtual  void AttachClientGameEvent(int eventID);

	virtual void OnPlayNPCSound(object_id_type npcID);


	//��Ŀ��ȡ��һ������Ч
    virtual void DetachTargetGfx(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iDummyType, const StringType& strFxFileName);
	virtual void DetachTargetBowGfx(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iDummyType, const StringType& strFxFileName);

	StringType GetSitRecoverVipGfx(GameLogic::LC_Attacker* pkScriptCaller);

	//��ɢ�ӵ�
	virtual void ShotBulletToTarget(GameLogic::LC_Attacker* pkScriptCaller,int iTargetIndex,int lSkillTypeID,int iBulletFuncIdx,int iPenetrateCount,int iDummyType);
	virtual void ShotBulletToTargetAngularDeflection(GameLogic::LC_Attacker* pkScriptCaller,int iTargetIndex,int lSkillTypeID,int iBulletFuncIdx,int iPenetrateCount,int iDummyType, float fAngle);
	virtual void ShotBulletToTargetFromLocation(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lSkillTypeID, int iBulletFuncID, int iPenetrateCount, int iDummyType, int iLocationIndex, float fDeltaAngle, int64_t lBulletID);
	virtual void ShotMultiBulletToTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lSkillTypeID, int iBulletFuncIdx, int iPenetrateCount, int iDummyType, int iScatteringAngle, int iScatteringCnt);
	virtual void ShotBulletAroundTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lMinRadius, int lMaxRadius, int lSkillTypeID, int iBulletFuncID, int iPenetrateCount, int iDummyType);
	virtual void ShotMultiBulletToTargetFromLocation(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lSkillTypeID, int iBulletFuncID, int iPenetrateCount, int iDummyType, int iScatteringAngle, int iScatteringCnt, int iLocationIndex, float fDeltaAngle, int64_t lBulletID);
	virtual void ShotMultiBulletToTargetFromMultiLocation(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lMinRadius, int lMaxRadius, int lSkillTypeID, int iBulletFuncID, int iPenetrateCount, int iDummyType, int iScatteringAngle, int iScatteringCnt, int iLocationIndex, float fDeltaAngle, int64_t lBulletID);

	//�򿪱���
	virtual void OpenPlaceHolder(GameLogic::LC_Attacker* pkScriptCaller,int iTargetIndex, bool bNeedOpenFirst);

	//��NPC����
	virtual void TalkToNPC(GameLogic::LC_Attacker* pkScriptCaller,int iTargetIndex);

	//�ɼ���Ʒ
	virtual void CollectItem(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex);

	//���������ڷ�������
	virtual void StartRevealInvisibleTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iCircleRadius, bool bRevealOnce);

	/* ----------------  End    ���� ---------------------------*/

	/*---------------------begin ���� ------------------------*/
	void StartDebugConsole(const char* szPath);
	/*---------------------End ����--------------------------*/

	//�ӽű�������ʾ��Ŀǰ���ڼ�鵽���ܲ����ͷ�ʱ������ʾ
	virtual void SendInfoMessage(int lSkillID, int lMsgID);

	//��UI�׳���ʾ��Ϣ
	virtual void ShowUIError(const StringType& kString);
	virtual void ShowUIErrorByCode(int iErrorCode);
	void ShowNoticeMessage(int iID);
	//��UI�׳���ʾ��Ϣ
	virtual void ShowMouseTips(const StringType& kString);
	//��UI�ܳ���ʾ��Ϣ--���ʾ��Ϣ
	virtual void ShowNotifyMessage(const StringType& kString, int iLevel);
	//����TextConfigid��ʾ
	virtual void ShowNotifyMessageWithType(long iMsgType,const StringType& kString, int iLevel);
	//��UI�׳�ϵͳ��ʾ
	void ShowSystemTip(const StringType& kString);

	virtual void ShowAttriActive(int kAttribute, int val);

	//��ȡ������Ϣ
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

    // ǿ����������
    virtual	void StartForcibleWeather(float fDensity, float fSpeed, float fLength, float fWidth, float fAlpha, float fWind);
    virtual	void StopForcibleWeather(void);

    // ȫ��Ч������
    virtual	void StartFullScreenEffect(int eFullScreenEffectType);
    virtual	void StopFullScreenEffect(void);

	virtual void StopCurrentBGM(void);

	//ȡϵͳʱ��
	StringType	GetComputerTime();

	//���
	void	   CookingDish();

	//����	   
	void	   PlaySound(const StringType& strKeyWord, int iChannelType = 0, bool bFadeToGain = false, int nPlayCount = 1 , float fAutoStop = 0.0f);

	int	   GetShowBloodBarType(GameLogic::LC_Attacker* pkScriptCaller);

	//����
	StringType GetItemText1CharFuncDescript(int lTextItemID1,int lLevel);
	StringType GetItemText2CharFuncDescript(int lTextItemID1,int lLevel1,int lTextItemID2,int lLevel2);
	StringType GetItemText4CharFuncDescript(int lTextItemID1,int lLevel1,int lTextItemID2,int lLevel2,int lTextItemID3,int lLevel3,int lTextItemID4,int lLevel4);
	//����
	//StringType GetItemHobbyAttributeFuncDescript(int lHobbyItemID,char cHobbyQualityValue,char cHobbyQualityLevel);

	StringType GetAttributeFuncDescript(int lAttributeFuncID);

	//��ÿ��սʤ������ID
	guild_id_type GetGlobleWarGuildID();
	//�������սռ��������
	bool IsDomainWarGuild(guild_id_type GuildID);

	//���ƺϳ�
	int		GetTokenCompositeItemSlot(int lItemID);
	int		GetTokenCompositedItemID(int lItemIDSlot0, int lItemIDSlot1);
	void		RequestInterruptTokenComposite();


	int		GetMaxItemEnduranceValue(int lItemID);

	StringType	GetMapNameByMapResID(int iMapResID);
	int			GetMapLineCount(int iMapResID);

	//��ȡ��ͼ����������Ϣ
	StringType	GetMapEnterConditionInfo(int iMapResID);

	bool		IsNeedBind(GameLogic::LC_BackPackEntry* pkBackPackEntry, int iBindType);

	//��ʾ/����UI
	void		ShowUIFrame(bool bShow);
	virtual		void ShowRichUI(GameLogic::LC_Attacker* pkScriptCaller, int iTargetType,int iRichUIID, int bShow);

	//��ƷUI����
	void		ShowUIContentFunc(int lItemTypeID);
	int		GetShowUITypeID(int lItemTypeID, int nIndex);
	StringType	GetShowUIIconDesc(int lItemTypeID, int nIndex);
	int		GetDescriptType(int lTypeID);

	//����npc�̵깦��ID��ȡ�̵�����
	StringType  GetNPCShopNameByFuncID(int lNPCShopFuncID);

	//begin ������ͼ����
	LC_ClientMapTransDataEntry* GetMapTransDataEntryByMapID(int lMapResID);
	//end ������ͼ����
	void						StopUICutScenes(void);
	float						GetUICutScenesPlayTime(int ID);
	float						GetUICutScenesTimeStep(int ID);
	const StringType&			GetUICutScenesIcon(int ID);

	//OpenLevel��Ϣ���
	StringType					GetSkillOpenLevelInfo(LC_PlayerBase *pkPlayer);
	StringType					GetTwToZn(const StringType &sName);
	//ͨ����ݼ�λ�û�ȡ��һ������
	int							GetFirstSkillIDByShortcutSlot(int iSlotID);

	//�Ƿ���װ��(ʹ��)
	int							CanEquipOrUseItem(LC_ClientGamePlayer *pkPlayer, int lItemID, int iCid);
	//�Ƿ���װ��(������װ��)
	bool						CanEquipSubItem(LC_PlayerBase *pkPlayer, int lItemID);
	//�Ƿ��ܲ��
	bool						CanDecomposition(int lItemID);
	//��ȡ����б��ַ���
	StringType					GetItemDisassembleUseFuncResult(int lItemID, bool bProbability = false, int32_t iChoice = 0);
	//��ȡ������
	StringType					GetItemDisassembleMustDropItems(int _iID);
	//��ȡ�࿪�����ѡ��
	StringType					GetMultiSelectGiftChoices(int lItemID);	
	//��ȡ�࿪�����ѡ��
	StringType					GetMultiSelectGiftChoicesCost(int lItemID);	
	//��ȡ��ƷSHOWID
	int							GetItemShowID(int lItemID);
	StringType					GetBuddyExpDrugs();

	//��þ���������ÿ�ս�Ǯ�����顢��������ϵ��
	float						GetArenaDailyRewardParam(int iRank, int iRewardType);

	//��ô�����������ȼ�
	int							GetGuildCreateRequireLevel(void);
	//��ô�������������
	int							GetGuildCreateRequireMoney(void);
	//��ô�����������Ԫ��
	int							GetGuildCreateRequireYuanbao(void);
	//
	int							GetGuildCreateRequireVip(void);
	//��ȡboss id�б�
	StringType					GetBossIdList(void);
	//�Ƿ�Ϊ����õ���
	bool						IsUnbindItem(int iItemID);
	//�Ƿ�Ϊ����õ���
	bool						CanBeBatchUsed(int iItemID);
	//��ȡ��ͼ��������
	int							GetMapShowType(long lMapResID);
	//��ȡUI��ͼ��ʾ����
	int							GetUIMapShowType(long lMapResID);
	//��ȡ��ͼ�������id
	int							GetSchemeGroupID(long lMapResID);
	//��ȡ��ͼ�Ƿ�����bossѪ����ʾ�˺�������
	int64_t						GetHarmQuantity(long lMapResID);
	//��ȡ��ͼ�����id
	int							GetMapBelongStoryID(long lMapResID);
	//��ȡ��ͼ���ݿɳ�ս��������
	int							GetMapSchemeTeamCount(int iMapResID);
	//��ȡ������Ϣ(��������NPC)
	int							GetShapeShiftInfo(GameLogic::LC_Attacker* pkScriptCaller);
	//�Ӹõ�ͼ�������Ƿ��Զ���������
	bool						GetMapAutoTask(int iMapResID);

	//��ȡVIPȨ���ַ���
	StringType					GetVipAuthorityString(int iVipLevel);
	StringType					GetVipNameString(int iVipLevel);
	StringType					GetVipPrivelegeInfo();
	StringType					GetVipPrivelegeInfoByType(int iType);
	int							GetVipPrivelegeInfoByTypeAndLevel(int iType,int iLevel);
	StringType					GetShowVipPrivelegeInfoByType(int iType);
	StringType					GetFlowerSendInfo(int iID);
	//��ȡ�����б�ÿҳ�����Ŀ
	int							GetMaxGuildSearchNum(void);
	//Begin ÿ����ս�����Ծ�����
	int							GetLivenessByLevel(int iLevel);
	//End ÿ����ս�����Ծ�����

	StringType					GetChoosePetIds();
	StringType					GetIconByNPCFuntionType(int iNPCFunctionType);

	//���ҵ�ǰ���ȶ�Ӧ��bossID�� boss���֣���ͼ����
	StringType					GetProgressInfo(int iStep);
	//��ȡ��ͼ�½���Ϣ
	StringType					GetMapCateGoryInfo(void);
	StringType					GetMapRwdInfo(int iMapResID);
	int							GetShowRaidStage(int iMapResID);
	//��ȡ�������������������
	int							GetRespawnItemNum(int iMapResID);
	//�ж�λ���Ƿ������
	bool						CanScreenPosMoveTo(float x, float y);
	StringType					URLEncode(const StringType &sIn);
	//��ȡ��Ӹ�����Ϣ
	StringType					GetCrossRealmMapInfo(int iMapTypeID);
	int							GetCrossRealmMapTargetType(int iMapTypeID);
	unique_id_type				GetKungFuMasterID();
	unique_id_type				GetCrossKungFuMasterID();
	int							GetCrossRealWarServerID();
	//��ȡ�������������ս��װ����װ��ID
	int							GetKungFuMasterDress(unique_id_type CharID, int prevItemID, int EquipType, bool ignoreFashion);
	//��ȡ�������������ս��װ����װ��ID(�����н�ɫID���жϣ�ֱ��ȡ����
	int							GetKungFuMasterDress2(int prevItemID, int EquipType, int index, bool ignoreFashion);

	guild_id_type				GetPowerGuildID();
	int							GetDirtyFlagRevisionID(int iFlag);
	StringType					GetMapDomainInfo(int iMapResID,int iServerID);
	bool						IsDomainMap(int iMapResID);
	StringType					GetDomainFlagPos(int iMapResID);
	int							GetDomainMapResIDByIndex(int iIndex);
	bool						IsDomainActive();
	int							CheckDomainReward(int iMapResID);	//�ж������ܲ�����ȡ��������ս���� 0�������죩1������ˣ�2�������죩
	int64_t						NumberPlus(int64_t iNumber1, int64_t iNumber2);
	//��ȡ���ս����
	int64_t						GetPlayerCombatScore(LC_ClientGamePlayer *pkPlayer);
	//�ж�����Ƿ�����������
	bool						IsKungFuMasterID(unique_id_type iUserID);
	StringType					GetGuildWarCatapultList(int iPlayerLevel);

	//�õ�ͼ�Ƿ�����Ӹ���
	bool						IsCrossRealmMap(int iMapResID);
	//�ж��ʼ������Ƿ�������ID
	bool						IsSpecialAffixItemID(int itemID);
	//�Ƿ��ǰ��boss��ͼ
	bool						IsGuildRaidMap(int iMapResID);
	//��ȡtalkicon
	StringType					GetCharTalkIcon(int iCharID);
	//��ȡchar������ɫ
	uint32_t							GetCharNameColor(LC_PlayerBase* pkWatcher, LC_ActorBase* pkTarget, int iPowerGuildID = -1);

	//��ȡ����ʱ��(ʱ�����ʱ������������)
	int							GetDungeonTime(int iMapResID);
	bool						CanEnterMap(int iMapResID);
	int							CanEnterMapWithPrompt(int iMapResID);
	int							GetMapNeedLevel(int iMapResID);
	StringType					GetMapPreTaskName(int iMapResID);
	StringType					GetHookMapsInfo();

	//���ջ ���ѱ�ƴ
	StringType					GetHolidayRankRewardsInfo(int iType, int iParam);

	//�������԰�����Ϣ
	StringType					GetGamePlayerSubRewardsInfo(int iGroupId);
	//ս����
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

	//��ȡ���ﵱǰ�ȼ�
	int							GetVehicleCurrentLevel(int iPlyerLevel, int iVehicleLevel);
	//��ȡ�����ս����������ʱ����
	combat_score_type							GetCimeliaOriginalScore(int iType, int iLevel);

	StringType					GetOpenBagData(int iTargetIndex);
	int							GetExternWarehouseData(int iCurrentIndex, int iTargetIndex);
	//װ��ǿ�����
	StringType					GetBaseEffectInfo(LC_BackPackEntry *pkEntry);
	//��ȡ�̳���Ʒ��Ϣ
	StringType					GetShopItemInfo(int iItemID, int iShopType);
	//��ȡ��Ʒ���ۼ۸���Ϣ
	int							GetItemUnBindPrice(int iItemID);
	//��ȡ��ǿ��Ϣ
	StringType					GetPowerUpInfo(int iLevel);
	//��װ���Ƿ�Ϊ��ǰ��ɫ����װ��(�Ƿ�Ϊ���ʵȼ�����)
	bool						IsEquipMostSuitable(int iItemID);
	//��ȡ��ǰ����ϵͳ�ȼ�����������������
	StringType					GetCimeliaItemInfo(int iLevel, int iCimeliaType);
	StringType					GetCimeliaSkillInfo(int iCimeliaType, int iLevel);
	int							GetCimeliaDisplaySkillID(int iCimeliaType, int iLevel);
	//��ȡ��ǰ�ȼ���ѧ��������
	int							GetCurrentCanLearnSkillCount(LC_PlayerBase *pkPlayer);
	
	//��ȡEFFECT����
	StringType					GetEffectDesp(int iEffectID, int iEffectLevel);
	//��ȡEffect����������Ϣ
	StringType					GetItemEffectFuncInfo(int iEffectFuncID);

	//��ͷ����
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

	//���⹦��
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

	//���ƶ�NPC�ľ���
	int GetDistanceFromNpc(LC_ActorBase *pkNpc);
	//�Ƿ���΢��������
	int IsRunInMiniLauncher(void);
	//���
	void CommitPatchProgress(int iEvent, int iCode);
	//��ͼ�Ƿ��ʹ�ð���ټ���
	bool IsMapCanGuildTransport(int iMapResID);

	int GetCurrentNPCTalkTaskFunction();
	
	int GetVipProfitDefineExtraBeginID(int iID, int iLevel );

	StringType GetWorld2RadarPositionInfo(int iMapResID, int iX, int iY);

	//rumormanager
	LC_ClientRumorManager * GetClientRumorManager();

	//��Ʒ�Ƿ��Զ�ʹ��
	bool	IsItemAutoUseByPlayer(long ItemID);
	bool	IsItemAutoShowByPlayer(long ItemID);
	bool	IsItemTrueAutoUseByPlayer(long ItemID);

	int		GetUseGranuLimitCount(long ItemID);

	//ͨ��cronTime��ȡ��һ��ʱ��
	int64_t		GetNextColdDownTimeByCronStr(const StringType& cronStr);

	//˲�Ƶ�Ŀ����ǰ
	virtual void	TransferToTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, bool bUseSafeRadius);
	virtual void	TransferBackToTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, bool bUseSafeRadius);

	bool	RaidMapCanRecordPK(int mapResID);
	bool	RaidMapCanPk(int mapResID);

	//�ж�����UI��Դ����
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

	//��ȡ��ǰ��ǰ�Կ���Ĭ����Ƶ�ȼ�
	long			GetDefaultConfigLevelOfGPU();

	//����id��ȡOpenLevel��Ҫ���ߵ�ʱ��
	int				GetOpenLevelOnlineTimeByID(int iID);

	// ����openlevel��id��ȡ��������ʱ��
	uint32_t		GetOpenLvelTotoalTime(int iID);

	uint32_t		GetCimeliaLevelInOpenLevelByOpenID(int iOpenID,int iSubID);

	bool			IsItemCategoryShowTipAnim(int iCategory2);
	//������Ʒid��ȡ��Ʒ����
	StringType		GetItemAttributeString(int iItemID, int iEquipStar);

	//������Ʒid��ȡ����ʹ��ʱ�������ٸ�����Ŀ
	uint32_t		GetBatchMaxItemCountByID(int iItemID);

	//��ȡ��������charTypeID
	int				GetDisplayCharTypeID(int iType, int GetDisplayCharTypeID);

	//��ȡ���ұ��ض�ӦԪ����
	int				GetTreasureCostByID(int iID);  

	//��ȡ
	StringType	GetGameStoryRaidRewardByMapresID(int iMapResID);  

	//��ȡ�ﹱ��Ϣ	
	StringType	GetGuildDonateInfo();

	StringType  GetActTimeShopInfoByID(int ID);	

	//���ݼ���id��ȡ�ؼ�id ��ѧ��
	uint32_t		GetSkillBookID(int iSkillID);
	uint32_t		GetSecretTypeByBookID(int iBookID);

	//��ȡ�����Ϣ
	long				GetToyPetCharTypeBycID(int32_t cID);
	StringType			GetToyPetInfoByCID(int32_t cID);

	StringType			GetShopCityGroupInfo(int iGroupID);

	//��ȡ��Ů�ͷ�����
	uint32_t		GetQQGirlsCount();

	uint32_t		GetFabaoIdByCharType(int charTypeID);
	uint32_t		GetCharTypeByFabaoID(int fabaoID);
	int				GetSubTransformSkillID(int iTransformID);
	int				GetSubTransformType(int iTransformID);

	//ʥ�����������
	uint32_t		GetGuildWarEndTime(int iWarType,int OpenType);
	uint32_t		CheckGuildWarTime(int iWarType,int OpenType);
	//�жϽ����Ƿ��ڻʱ��
	StringType		CheckActivitySchedule(int iCatagory);
	int				CheckSecretPoolValueID(int iItemID,int iItemCount,int iCatogory,int iRewardIndex);
	StringType		CheckRichManTime(int iChestID,int iTime);
	int				GetScretTreasureTotalTimes(int iChestID,int iCurValue);
	int				GetScretTimesValue(int iChestID);
	int				GetScretLimitTimes(int iChestID);

	//< tx�Խӣ�ͨ����ҳ������Ʒ����ʾweb����
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

	//װ���ϳ����
	void			SetItemCompositionTargetInfo(LC_BackPackEntry *SrcEntry, int TargetID);
	LC_BackPackEntry* GetItemCompositionTargetInfo();
	void			ResetItemCompositionTargetInfo();

	int64_t			GetAuctionSellPrice(int iItemID, int iMoneyType);

	bool			IsEquipReinStar(int itemID, uint16_t equipStar);
	//���߻������
	int				GetMapIndexByResIDAndLogicID(int lMapResID, int lMapLogicID);
	bool			IsMapIsNewBeeMap(int lMapResID);

	int				GetForceBattleMapIndex(int lMapResID);

	//64λ��أ�����uniqueID�ϳ�
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

	//ʱװ
	int		GetFashionSuitCount();
	
	//
	StringType GetAchievementInfo(int iID);

	//decode platform vip
	StringType DecodePlatformVipInfo(int formatvip);

	//��ȡ��ǰ������
	StringType GetProcessName();
	//����ShellExecute�ӿ�
	void LuaShellExecute(const StringType& lpOperation, const StringType& lpFile, const StringType&  lpParameters, const StringType& lpDirectory, int nShowCmd);
	
	int GetGuildBattleTaskOpenSchedule(int iTabIdx);

	bool CheckLocation(int lID,int iDistance);
	//���ͬ�������ID
	int	GetGameServerInfoIndex(int iServerID, int iIndex);

	//��ȡ�����
	StringType GetActivityRewardInfo(int32_t ActivityType, int32_t iCatagory, int32_t iSchedule, int32_t ikey);
	StringType GetActivityRewardInfoWithChoice(int32_t ActivityType, int32_t iCatagory, int32_t iSchedule, int32_t ikey, bool bProbability, int32_t iChoice);
	//��Դ�һ�ϵͳ��Ʒ����
	int GetGameStoryCurrentMapResID(int32_t iGameStoryID);
	StringType GetGameStoryBossInfo(int iMapResID);

	//�������
	StringType GetAllSkillBooks();
	StringType GetEverySkillBooks();	//��ȡ��������ʥ�ۣ�����δ�����
	StringType GetSkillBookNodeInfo(int iID);
	StringType GetSkillBookInfo(int iID);
	StringType GetSkillBookAttribs(int iSkillBookID,int iPos = -1);
	StringType GetSkillBookAttribsByNodeLevel(int iSkillBookID,int iPos, int iLevel);
	StringType GetSkillBookItemChangeAttribs(int iSkillBookID,int iLevel);
	StringType GetSkillBookItemAttribs(const StringType& items);
	StringType GetCategorySkills(int ilCategory1);
	int64_t	   GetSkillBookCombatScore(int iSkillBookID, int iLevel);
	ResultType CheckSkillBookUpgrade(int iSkillBookID,int iPos);
	StringType GetSkillBookGridInfo();	//ʥ�������λ��Ϣ
	StringType GetCimeliaInitAttribInfo(int iType);//�����߳�ʼ���ԣ���һ����ͨƤ����������
	StringType GetCimeliaTransformAttribInfo(int iTransformID);
	StringType GetCimeliaTransformElementAttribInfo(int iTransformID);
	//��������
	virtual int	CheckUseItemOpenSkillBook(GameLogic::LC_ActorBase* pkScriptCaller, int32_t iID);

	int64_t			GetSubordinateTransformCombatScoreByItemId(int iItemId);//ͨ������id�������Ƥ��ս����
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

	//��ȡ�ַ�����MD5ֵ
	StringType GetStringMd5(const StringType& str);

	StringType GetBoneSoulSkillIconAndTip(int iLevel, int iIndex, int iStar);
	StringType GetBoneSoulCost(int iLevel, int iCount);

	bool	   IsItemMelt(int16_t meltType,int lItemID);
	int64_t	   GetDateTimeStamp(int iSecond, int iMinute, int iHour, int iDay, int iMonth, int iYear);

	int32_t	   CanStarsRune(int iCatagory,int iIndex);
	StringType GetStarsRuneCost(int iCatagory,int iIndex);

	//��������
	int		   GetChessMatrixActiveScheduleID();
	int		   ScriptActionExecute(const StringType& cronStr,int value);

	//��о
	StringType GetCoreInfo();
	StringType GetCoreAttribs(int iID, bool bPercent);
	StringType GetCommanderTitleAttribs(int iID, bool bPercent);
	StringType GetCommanderTitleName(int iID);
	bool IsCommanderTitleCanShow(int iID);
	StringType GetCoreGoalInfo();
	StringType GetStigmasUnlockInfo(int iID);

	StringType	GetDrawCardRewardDisplay(int iShopType,int iScheduleID);
	//������
	StringType	FindAndFileterForbiddenText(const StringType& sInfo, int eType);

	//��ʾxml ui
	void ShowGameUIByKeyName(const StringType& sKeyName);
	void ShowGameUIByKeyNameAndPath(const StringType& sKeyName, const StringType& sPath);
	void DestroyGameUI(GUISys::WT_Widget* pkWgt);
	int GetBackPackEmptyEntry(int iPackType);

	bool IsBoss(object_id_type lCharType);
	StringType GetItemEffectValueStr(int TypeID, int iValue);

	//����ɫ���
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
	//�ǻ����
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
	//����
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

	//�����˱�
	int					GetRobotScore(uint64_t iId);
	int					GetRobotHead(uint64_t iId);
	int					GetRobotLevel(uint64_t iId);
	StringType			GetRobotName(uint64_t iId);
	bool				IsSoloduleRobotPlayerId(uint64_t iId);

	StringType			GetDecodeScore(uint64_t nInput, bool bTime);

	//ת��wstring,��ȡsubstring
	StringType			GetSubCharString(const StringType& sContent, int iBegin, int iEnd);
	//ת��wstring ��ȡsize
	int					GetCharCount(const StringType& sContent);

	int32_t				GetOpenTimeByID(int ID);
	int32_t				GetUnLockTimeByID(int ID);

	//���ְ���
	StringType			GetTutorialRewardInfo();

	//��񷽱�� 
	StringType          GetSkyArenaRankRewardInfo();
	StringType			GetSkyArenaRankInfo();

	StringType			GetSpecialTaskInfo();

	//���ջ
	StringType			GetHolidayMainActivityInfo();
	StringType			GetHolidaySubActivityInfo(int iId);
	int                 GetHolidayActivityTaskScheduleId(int iHolidayActivityTaskType);
	StringType          GetHolidayActivityTaskTabNames(int iHolidayActivityTaskType); 

	//�۷�ս����������
	StringType			GetSummitArenaMatchName(int iID);
	//�۷�ս����������
	StringType			GetSummitArenaRankReward();

	//���¶�ע
	StringType			GetGuessCost(int ID);
	StringType			GetGuessProfit(int ID);
	//��ҫ�۷��
	int					GetHight32(int64_t value){return int(value>>32);};
	int					GetLow32(int64_t value){return int((value<<32)>>32);};
	//BuddyThemeTaskType
	StringType			GetThemeActivityTaskTabNames(int iBuddyThemeTaskType);
	int					GetThemeActivityTaskScheduleId(int iBuddyThemeTaskType);

	StringType			GetCustomCommandList();

	//���ǰ���¼�ID
	int					GetPreStoryId(int storyId);
	int64_t				GetNextStoryTime(long lStoryID);//�����һ�ο���ʱ��

	//���ջƴͼ����
	StringType GetHAPTTaskInfo();
	StringType GetHAPTReward();
	void GetTaskInfoStringById(int iTaskID,StringType& OutStr);
	void GetAchievementInfoStringById(int iAchievementID ,StringType& OutStr);

	//���ջ���巵�� 
	StringType GetContinousChargeInfo(); 

	/*��ǿϵͳ
	***Ӣ��,����,װ��,����,�ȳ�,����,���
	***Ӣ�ۣ����ߣ�װ���ķ�������ս������ͨ��promotecombat�������������
	*/
	void CalculateBeBetterMaxScores();								//��ֵ�ڵȼ������仯ʱ�����¼���

	//��ǿϵͳ-Ӣ��-��ֵ--------------------------------------
	combat_score_type GetAllHeroLevelMaxScore();
	combat_score_type GetAllHeroStarMaxScore(){return m_AllHeroStarScore;};
	combat_score_type GetAllHeroSkillMaxScore(){return m_AllHeroSkillScore;};
	int GetAllHeroCount();
	//��ǿϵͳ-Ӣ��-��ǰֵ	
	combat_score_type GetOwnedHeroLevelScore();
	combat_score_type GetOwnedHeroStarScore();
	combat_score_type GetOwnedHeroSkillScore();
	int GetOwnedHeroCount();
	//��ǿϵͳ-Ӣ��-��ʼ��
	void CalculateAllHeroLevelMaxScore();
	void CalculateAllHeroStarMaxScore();
	void CalculateAllHeroSkillMaxScore();
	//��ǿϵͳ-����-��ֵ--------------------------------------
	combat_score_type GetAllRuneLevelMaxScore(){return m_AllRuneLevelScore;};
	combat_score_type GetAllRuneStarMaxScore(){return m_AllRuneStarScore;};
	int GetAllRuneCount();
	//��ǿϵͳ-����-��ǰֵ	
	combat_score_type GetOwnedRuneLevelScore();
	combat_score_type GetOwnedRuneStarScore();
	int GetOwnedRuneCount();
	//��ǿϵͳ-����-��ʼ��
	void CalculateAllRuneLevelMaxScore();
	void CalculateAllRuneStarMaxScore();

	//��ǿϵͳ-װ��-��ֵ--------------------------------------
	combat_score_type GetAllEquipMaxScore(){return m_AllHeroEquipScore;};
	combat_score_type GetAllEquipLevelMaxScore(){return m_AllHeroEquipLevelScore;};
	combat_score_type GetAllEquipStarMaxScore(){return m_AllHeroEquipStarScore;};
	//��ǿϵͳ-װ��-��ǰֵ	
	combat_score_type GetOwnedEquipScore();
	combat_score_type GetOwnedEquipLevelScore();
	combat_score_type GetOwnedEquipStarScore();
	//��ǿϵͳ-װ��-��ʼ��
	void CalculateAllEquipMaxScore();
	void CalculateAllEquipLevelMaxScore();
	void CalculateAllEquipStarMaxScore();	
	
	//��ǿϵͳ-���-��ֵ--------------------------------------
	combat_score_type GetAllToyPetMaxScore(){return m_AllToyPetScore;};
	int GetAllToyPetCount();
	//��ǿϵͳ-���-��ǰֵ	
	combat_score_type GetOwnedToyPetScore();
	int GetOwnedToyPetCount();
	//��ǿϵͳ-���-��ʼ��
	void CalculateToyPetMaxScore();

	//��ǿϵͳ-����-��ֵ--------------------------------------
	combat_score_type GetVehicleAttrLevelMaxScore(){return m_VehicleAttrLevelMaxScore;};
	combat_score_type GetVehicleSkinMaxScore(){return m_VehicleSkinMaxScore;};
	combat_score_type GetVehicleEquipMaxScore(){return m_VehicleEquipMaxScore;};
	combat_score_type GetVehicleDrugMaxScore(){return m_VehicleDrugMaxScore;};
	//��ǿϵͳ-����-��ǰֵ	
	combat_score_type GetOwnedVehicleAttrScore();
	combat_score_type GetOwnedVehicleSkinScore();
	combat_score_type GetOwnedVehicleEquipScore() { return _getOwnedCimeliaEquipScore(MountPack); }	//���޵�ǰװ��ս��
	combat_score_type GetOwnedVehicleDrugScore();
	
	//��ǿϵͳ-����-��ʼ��
	void CalculateVehicleAttrMaxScore();
	void CalculateVehicleSkinMaxScore();
	void CalculateVehicleEquipMaxScore();
	void CalculateVehicleDrugMaxScore();

	//��ǿϵͳ-�ȳ�-��ֵ--------------------------------------
	combat_score_type GetDevaEyeAttrLevelMaxScore(){return m_DevaEyeAttrLevelMaxScore;};
	combat_score_type GetDevaEyeSkinMaxScore(){return m_DevaEyeSkinMaxScore;};
	combat_score_type GetDevaEyeEquipMaxScore(){return m_DevaEyeEquipMaxScore;};
	combat_score_type GetDevaEyeDrugMaxScore(){return m_DevaEyeDrugMaxScore;};
	//��ǿϵͳ-�ȳ�-��ǰֵ	
	combat_score_type GetOwnedDevaEyeAttrScore();
	combat_score_type GetOwnedDevaEyeSkinScore();
	combat_score_type GetOwnedDevaEyeEquipScore(){ return _getOwnedCimeliaEquipScore(PetPack); }	//�ȳ�װ���ӳ�ս����ǰֵ
	combat_score_type GetOwnedDevaEyeDrugScore();
	//��ǿϵͳ-�ȳ�-��ʼ��
	void CalculateDevaEyeAttrMaxScore();
	void CalculateDevaEyeSkinMaxScore();
	void CalculateDevaEyeEquipMaxScore();
	void CalculateDevaEyeDrugMaxScore();

	//��ǿϵͳ-����-��ֵ--------------------------------------
	combat_score_type GetWingAttrLevelMaxScore(){return m_WingAttrLevelMaxScore;};
	combat_score_type GetWingSkinMaxScore(){return m_WingSkinMaxScore;};
	combat_score_type GetWingEquipMaxScore(){return m_WingEquipMaxScore;};
	combat_score_type GetWingDrugMaxScore(){return m_WingDrugMaxScore;};
	//��ǿϵͳ-����-��ǰֵ	
	combat_score_type GetOwnedWingAttrScore();
	combat_score_type GetOwnedWingSkinScore();
	combat_score_type GetOwnedWingEquipScore(){ return _getOwnedCimeliaEquipScore(WingPack); }	//����װ���ӳ�ս����ǰֵ
	combat_score_type GetOwnedWingDrugScore();
	//��ǿϵͳ-����-��ʼ��
	void CalculateWingAttrMaxScore();
	void CalculateWingSkinMaxScore();
	void CalculateWingEquipMaxScore();
	void CalculateWingDrugMaxScore();
	
	//��ǿϵͳ-ָ�ӹ�װ��-��ֵ--------------------------------------
	combat_score_type GetCommanderEquipAttrMaxScore(){return m_CommanderEquipMaxScore;};
	combat_score_type GetCommanderEquipStrengthenMaxScore(){return m_CommanderEquipStrengthenMaxScore;};
	combat_score_type GetCommanderEquipStarMaxScore(){return m_CommanderEquipStarMaxScore;};
	//��ǿϵͳ-ָ�ӹ�װ��-��ǰֵ	
	combat_score_type GetOwnedCommanderEquipAttrScore();
	combat_score_type GetOwnedCommanderEquipStrengthenScore();
	combat_score_type GetOwnedCommanderEquipStarScore();
	//��ǿϵͳ-ָ�ӹ�װ��-��ʼ��
	void CalculateCommanderEquipAttrMaxScore();
	void CalculateCommanderEquipStrengthenMaxScore();
	void CalculateCommanderEquipStarMaxScore();

	//�ر�ͼ
	StringType GetTreasureItemInfo();

	//��ֵ ��ֵ������Ӣ�۱����ӵ�У����Ұ���ǰָ�ӹٵȼ�������
	combat_score_type m_AllHeroLevelScore;
	combat_score_type m_AllHeroStarScore;
	combat_score_type m_AllHeroSkillScore;
	combat_score_type m_AllRuneLevelScore;
	combat_score_type m_AllRuneStarScore;
	combat_score_type m_AllHeroEquipScore;				
	combat_score_type m_AllHeroEquipLevelScore;			
	combat_score_type m_AllHeroEquipStarScore;	
	combat_score_type m_AllToyPetScore;
	//������ ����
	combat_score_type m_VehicleAttrLevelMaxScore;
	combat_score_type m_VehicleSkinMaxScore;
	combat_score_type m_VehicleEquipMaxScore;
	combat_score_type m_VehicleDrugMaxScore;
	//������ �ȳ�
	combat_score_type m_DevaEyeAttrLevelMaxScore;
	combat_score_type m_DevaEyeSkinMaxScore;
	combat_score_type m_DevaEyeEquipMaxScore;
	combat_score_type m_DevaEyeDrugMaxScore;
	//������ ����
	combat_score_type m_WingAttrLevelMaxScore;
	combat_score_type m_WingSkinMaxScore;
	combat_score_type m_WingEquipMaxScore;
	combat_score_type m_WingDrugMaxScore;
	//ָ�ӹ�װ��
	combat_score_type m_CommanderEquipMaxScore;				
	combat_score_type m_CommanderEquipStrengthenMaxScore;			
	combat_score_type m_CommanderEquipStarMaxScore;	

	//��ǰֵ--------------------------------------
	//������ ���� �⼸���ڱ�����õ�
	combat_score_type m_OwnedVehicleAttrLevelScore;
	//������ �ȳ�
	combat_score_type m_OwnedDevaEyeAttrLevelScore;
	//������ ����
	combat_score_type m_OwnedWingAttrLevelScore;

	combat_score_type m_EquipAttrCurScore;
	/*
	***************��ǿϵͳ����**************
	*/

	//�������Խ�����ؽӿ�
	//��ǰ�ڴ�����ͼ�������б��ļ���С IG_MaterialManager
	StringType GetDebugMaterialListInfo();

	//��ǰ�ڴ���ģ������ GD_ModelContentManager
	//��ǰ��ǰ�ڴ�����Ч���� FX_FxManager
	//��ǰ��Ч�ڴ���������б� FX_ClientFxManager

protected:
	void _changeAvatarItem(GD_ClientActorModel *pkActorModel,int iChangeType,int lItemID,int lCharType);
	void _selectWatchedTarget(object_id_type* alTargetList, int iTargetListSize, 
			int& iSelectTargetIndex, int& lUpdateTargetListTime, const object_id_vector& rkSurroundTargetVector);

	int _getAutoShortcutKeyInOpenLevelBySkillID(int lSkillID);
	StringType _getCimeliaLevelInOpenLevelBySkillID(int iSkillID);

	bool _isLeapYear(int iYear);	//�Ƿ�Ϊ����
	StringType _getCalendarString(int iYear, int iMonth, int iDate);	//��ȡĳ�������·ݵ�������Ϣ�ַ���

	//װ��������Է�Χ�ַ���
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
	//<< �����ӵ�

	//ʥ��(Skillbook)
	StringType _getAttribString(LC_ActorAttributeMap *attrMap);
	StringType _getPercentAttribString(LC_ActorAttributeMap *attrMap);

	//ʥ��
	void _getSkillBookAttribs(LC_ActorAttributeMap* attrMap, int iSkillBookID, int iPos, int iLevel);

	//ָ�ӹ�װ��
	int _getCommanderEquipAttribID(const CF_EquipCommanderSlotReinforce::DataEntry* pkEntry, int iEquipSlotIdx);

	//������װ��ս��
	combat_score_type _getOwnedCimeliaEquipScore(int iPackType);
private:
		mem::vector<UT_Vec3Int> m_kLocVec;
		LC_BackPackEntry		m_TmpBackPackEntry[4];
		LC_BackPackEntry		m_TmpItemCompositionPackEntry;
};

#endif