#ifndef _LC_CLIENT_GAME_PLAYER_H
#define _LC_CLIENT_GAME_PLAYER_H
//客户端专用Game Player类
#include "ClientDefine.h"
#include "MG_MessageHandlerInterface.h"
#include "LC_GameEventHandler.h"
#include "LC_ClientSocietyAsset.h"
#include "LC_ClientFriendMsgBroad.h"
#include "LC_ClientSearchPlayer.h"
#include "LC_ClientViewPlayerInfo.h"
#include "LC_ClientFriendNick.h"
#include "LC_AcrossMapPath.h"
#include "LC_ClientTranscriptionInfoManager.h"
#include "LC_ClientGameStoryManager.h"
#include "LC_ClientArenaBattleChallengeInfoAsset.h"
#include "LC_ClientTaskManager.h"
#include "LC_PlayerDetailManager.h"
#include "LC_ClientWorldLevelAsset.h"
#include "LC_ClientTaskMap.h"
#include "LC_ClientRaidHookAsset.h"
#include "LC_ClientAlliance.h"
#include "CR_UIType.h"
#include "LC_ClientOpenAbilityManager.h"
#include "MG_GameProtocol.h"
#include "MG_BuddyProtocol.h"
#include <map>
#include "UT_TimeUtil.h"
#include "LC_ClientSoulRefineAsset.h"
#include "LC_SecretTreasureAsset.h"
#include "YunYingPlatform.h"
#include "LC_ClientActivityManager.h"
#include "LC_ClientBuddyInstance.h"
#include "LC_ClientGamePlayerCommander.h"
#include "GlobalSettings.h"

class GD_ClientActorModel;
class LC_ClientPlayerGroup;
class LC_ClientTaskMap;
class LC_PlayerFlopManager;
class NW_Game2ClientSession;
class LC_ClientProduction;
class LC_ClientNetPlayer;
class LC_ClientNPC;

struct MouseOverTargetArg
{
	NiColor		kOutlineColor;	//轮廓颜色

	MouseOverTargetArg(void)
		: kOutlineColor(NiColor::WHITE)
	{}

	MouseOverTargetArg(NiColor & a)
		: kOutlineColor(a)
	{}
};

struct PathPendingActionArg	//寻路挂起行为
{
	PathPendingActionType	enmType;
	int			iTargetCharTypeID;	//目标的CharType
	bool		bNewBeeSwitch;
	int			iTargetMapLogicID;
};

class HookingJump
{
public:
	HookingJump()
	{
		Clear();
	}

	void Clear()
	{
		fStartTime = 0;
		fWaitTime = 0;
		bStarted = false;
	}

	bool IsValid(float fCurrentTime)
	{
		if (fCurrentTime - fStartTime >= fWaitTime)
		{
			return true;
		}

		return false;
	}

	void Start(float fCurrentTime)
	{
		fStartTime = fCurrentTime;
		fWaitTime = UT_MathBase::RandInRangeFloat(GetGlobalSetting.fHookingJumpMinTime, GetGlobalSetting.fHookingJumpMaxTime);
		bStarted = true;
	}

	bool IsStarted()
	{
		return bStarted;
	}

private:
	float fStartTime;
	float fWaitTime;
	bool bStarted;
};

typedef mem::map<int32_t, bool> CimeliaUpgradeInfoMap;
typedef mem::map<int32_t, float> CimeliaUpgradeTimeMap;
typedef mem::map<int32_t, int> CimeliaUpgradePayIndexMap;


class GameOptCache : public Memory::MM_BaseObject
{
public:

	GameOptCache() { m_lSkillID = 0; };
	virtual ~GameOptCache() {};

	void Update(float fDelta)
	{
		ti.tick(fDelta);
		if ( ti.IsDown() )
		{
			 ClearData();
		}
	}

	void PushSkill(int lSkillID)
	{
		m_lSkillID = lSkillID; 
		m_kWASD = UT_Vec3Int(0,0,0);
		ti.Start(1.0f);
	}
	void PushWalkData(UT_Vec3Int& wa)
	{
		m_kWASD = wa;
		m_lSkillID = 0;
		ti.Start(1.0f);
	}

	void ClearData()
	{
		m_kWASD = UT_Vec3Int(0,0,0);
		m_lSkillID = 0;
	}

	int GetSkillID() { return m_lSkillID; }
	const UT_Vec3Int & GetWalkData() { return m_kWASD; }

private:
	UT_Vec3Int m_kWASD;
	int m_lSkillID;
	TickCount ti;
};

struct PrivateChatSaveSt
{
	PrivateChatSaveSt()
	{
		Clear();
	}
	void Clear()
	{
		targetName = "";
		targetID = 0;
		channel = 0;
		msgtxt = "";
		
	}

	StringType targetName;
	unique_id_type targetID;
	uint8_t		channel;
	StringType msgtxt;
};

#define	ClientGamePlayerLoadingMapKey	"ClientGamePlayerLoadingMapKey"
typedef mem::vector<SingleActivateBuddyData>			activateBuddyList;
typedef mem::vector<SingleBuddyData>					inactiveBuddyList;
typedef mem::map<uint16_t, LC_ClientBuddyInstance*>		buddyInstanceMap;

typedef activateBuddyList::iterator			m_vActiveBuddyListItr;			//出站表指针
typedef inactiveBuddyList::iterator			m_vInactiveBuddyListItr;		//待出站表的指针
typedef	buddyInstanceMap::iterator			m_mBuddyInstanceMapItr;

class LC_ClientGamePlayer : public GameLogic::LC_PlayerBase,
							public MG_MessageHandlerInterface
{
public:
	LC_ClientGamePlayer();
	~LC_ClientGamePlayer();
	DECL_RTTI_NAME( LC_ClientGamePlayer )

	virtual void			InitLogicData();
	virtual void			ReUse();
	virtual bool			RecvMessage(MG_MessageBase& rkMessage);
	void					SetSession(NW_Game2ClientSession* pkSession){m_pkClientLocalSession = pkSession;};
	NW_Game2ClientSession*  GetSession(){return m_pkClientLocalSession;};
	void                    SendToServer(char *pMsg, uint16_t wSize);
	void					OnDisconnect();

	void					SetUserID(user_id_type lUserID){m_lUserID = lUserID;};
	user_id_type			GetUserID(){return m_lUserID;};

	virtual inline object_id_type	GetID()
	{
		return m_controlIDVec.empty() ? m_iID : m_controlIDVec.front();
	}
	virtual inline void				ResetContronlID()
	{
		m_controlIDVec.clear();
	}
	virtual inline void				AddControlID(object_id_type iID)
	{
		m_controlIDVec.push_back(iID);
	}

	virtual void			SetHP(attr_value_type iHP);
	virtual void			SetEXP(int64_t iEXP);
	virtual void			SetMP(int iMP);
	virtual void			SetLevel(int lLevel);
	virtual void			SetSkillLevelupPoints(int64_t Value);

	virtual void			SetProductionLevel(int lProductionLevel);
	virtual void			SetCurrentMastery(int lCurrentMastery);
	virtual void			SetCurrentEnergy(int lCurrentEnergy);
	virtual bool 			Respawn(attr_value_type lHP,attr_value_type lMP);	
	virtual void			ChangeMap(int iMapLogicID,int lMapResID,int lMapLogicType,const Utility::UT_Vec3Int& rkLocation, bool bAsBuddy = false);
	void					ChangeMapImp(int iMapLogicID,int lMapResID,int lMapLogicType,const Utility::UT_Vec3Int& rkLocation);
	void					SetMapLoaded() { d_MapLoadInfo.setLoaded(); }
	virtual bool			SetTargetLocation( const Utility::UT_Vec3Int& rkTarget );
	virtual void			SetDead(bool bIsDead);
	void					SaveDeadTimeForC(int deadTime);
	int						GetSaveDeadTimeForC();
	void					SetDeadTime(int deadTime);

	StringType				GetCharName();

	virtual void			CreateVisibleData();
	virtual void			DestoryVisibleData();
	virtual void			ChangeVisibleData(int lCharTypeID);
	void					SetShapeShiftInfo(int lShapeShiftTypeID);
	virtual void			CreateRiderVisibleData();
	virtual	void			DestoryRiderVisibleData();

	virtual void			CreateBallVisibleData();
	virtual void			DestroyBallVisibleData();

	void					ShowAllBuddyVisibleData(bool bShow);
			
	void					CreateAnQi();
	void					DestroyAnQi();

	virtual void			CreateSuiteNPC();
	virtual void			DestroySuiteNPC();

	virtual void			CreateSuitWeapon();

	virtual void			PlayAnimation(const StringType& rkAnim,bool bForce = false,float fSequency = 1.0f,bool bAutoAdjustFrequency = false);
	virtual void			PlayWeaponAnimation(const StringType& rkAnim,bool bForce = false,float fSequency = 1.0f,bool bAutoAdjustFrequency = false);
	virtual void			PlayRiderAnimation(const StringType& rkAnim,bool bForce = false,float fSequency = 1.0f,bool bAutoAdjustFrequency = false);
	virtual void			PlayIdleAnimation();
	virtual void			PlayWingAnimation(const StringType& rkAnim,bool bForce = false,float fSequency = 1.0f,bool bAutoAdjustFrequency = false);

	virtual void			Update(float fCurrentTime,float fDeltaTime); 

	//转动角色方向
	void					FaceAt(const Utility::UT_Vec3Int& rkTargetDir);
	void					FaceToLocation(const Utility::UT_Vec3Int& rkTargetLocation);
	//鼠标类型
	void					SetMouseLogicModeType(MouseLogicModeType eType){m_eMouseLogicModeType = eType;};
	MouseLogicModeType		GetMouseLogicModeType(){return m_eMouseLogicModeType;};

	void					ShowMessage(int lMessageType);
	void					ShowNoticeMessage(int iMsgType, const StringType& sMsg, int iLevel);

	LC_PlayerEquipSuitPtrMap& GetPlayerEquipSubSuitMap(){ return m_kPlayerEquipSubSuitPtrMap; }

	//Begin 物品操作接口
	virtual GameLogic::LC_PackAsset* GetPackAsset() {	return LC_PlayerBase::GetPackAsset();}
	//背包操作:使用物品
	virtual ResultType		CheckUseItemInPackByIndex(int iPackType, int iEntryIndex, int32_t& iCount, int iParam);
	virtual ResultType		RequestUseItemInPackByIndex(int iPackType, int iEntryIndex, int32_t iCount, int iParam,const StringType &text, int iBuddyCID = 0);
	virtual ResultType		RequestUseItemInPackByType(int lItemTypeID, int iParam, int cID = 0, int iCount=1);
	virtual ResultType		RequestUsePatchItemInPackByIndex(int iPackType, int iEntryIndex, int32_t iCount, int iParam,const StringType &text, int iBuddyCID = 0);
	//背包操作:装备物品
	void					RequestEquipItem(int iEntryIndex, int cID, int iCategory);
	//背包操作：拆分物品
	virtual ResultType		RequestSplitItemInPackage(int iPackType, int iSrcIndex, int iDesIndex, int iSplitNum );
	//背包操作：合并物品
	virtual ResultType		RequestMergeItemsInPackage(int iSrcPackType, int iSrcEntryIndex, int iDstPackType, int iDstEntryIndex);
	//背包操作：交换物品位置
	virtual ResultType		RequestExchangeItemsInPackage(int iSrcPackType, int iSrcEntryIndex,int iDstPackType, int iDstEntryIndex, bool bNeedEmpty = false);
	//背包操作：整理物品
	virtual ResultType		RequestTidyItemsInPackage(int iPackType);
	
	//背包操作：销毁
	virtual ResultType		RequestDestroyItemInPackage(int iPackType, int iEntryIndex);
	//请求强化包裹里的装备
	void					RequestEnhanceEquipmentInPackage(int iPackType, int iEntryIndex, int iLevel0, int iLevel1, int iLevel2, int iLevel3, int iLevel4, int iLevel5, int iLevel6, int iLevel7);
	//请求解绑定物品
	ResultType				RequestUnbinItemInPackage(int iSrcPackType, int iSrcEntryIndex, int iDstPackType, int iDstEntryIndex);

	// 强化装备&升星
	void					RequestEnhanceEquipLevel(int iBuddyID,int iEquipIndex, const StringType & iConsumeItems );
	void					RequestEnhanceEquipStar(int iBuddyID,int iEquipIndex);
	void					RequestReinforceEquipSlot(int iBuddyID,int iEquipIndex,bool bOneKey);

	int64_t					GetCommanderAllEquipCombatScore();

	int						GetOpenBagRemainSeconds();

	int						GetCanCostItemCount(int iItemID, bool bLmtTime = false);
	//根据物品id获取位置index
	int						GetItemSlot(int iPackType,int iItemID);
	ResultType				CheckUpgrade(int iType, int iPayIndex);
	int						GetItemListValueCount(int iType);
	int						GetCimeliaNeedLevel(int iType);
	//请求进化成长系统
	void					RequestUpgradeCimelia(int iType);
	void					RequestAutoUpgradeCimelia(bool bStart, int iType);

	//秘笈进阶
	void					RequestUpgradeMiji(int iID, int iPos);
	void					OnRltUpgradeMiji(ResultType result);
	//收到进阶回应
	void					OnRltUpgradeCimelia(ResultType result, int iType, int iRank, int iPayIndex, const StringType& sModifyAttribs="");

	//幻化请求						
	void					RequestChangeTransform(int iType, int iOpCode, int iSetLevel);
	void					RequestTransformCimelia(int iType, int iOpCode, int iTransformID);
	void					OnRltChangeTransform(ResultType result, int iType, int iCode, int iParam);
	void					RequestBless(int iType, int iOpCode, int iSetLevel);

	//获得活动奖励
	void					RequestCommonReward(int type, int op_code, int op_para);
	void					RequestCommonBaseReward(int type, int op_code, int op_para);
	void					RequestActivityCommonOp(int type, int catagory, int schedule, int opcode, const StringType& params);
	void					RequestActivityCommonOpEx(int type, int catagory, int schedule, int opcode, const StringType& params);
	void					RequestGetRankSubBInfo(int iType, int iGroupID, int iMinIndex, int iMaxIndex);

	//请求冲榜类活动信息
	void					RequestShotActivityInfo(const StringType &idx);
	

	//请求使用激活码
	void					RequestUseActivationCode(const StringType &ActiveCode, int iType);
	//开宝箱
	void					RequestTreasureDerive(int iId);
	void					RequestTreasureLog();
	//丹哥宝库
	ACT_SecretTreasureAsset& GetSecretTreasureAsset(void){ return m_kSecretTreasureAsset;};
	void					RequestSecretTreasure(int iCode, int iParam1,int iParam2);
	//炼化炉
	void					RequestSmelter(int iMeltType,const StringType &operate);
	void					RequestBuyVigor();
	//星辰符文
	void					RequeseStarRuneOperation(int iCatagory,int iIndex);

	//成就
	void					RequestAchievementReward(int iAchievementID);
	void					RequestAchievementPointReward(int iRewardID);
	void					RequestSevenDaysAchievementPointReward(int iRewardID);

	void					ReqSetAtvChargeRedPoint(int iScheduleId);
	void					UpdateAtvChargeRedPoint(int iScheduleId);
	bool					GetAtvChargeRedPointState(int iScheduleId);

	virtual bool			IsInFinishAchievementMap(int achievementID);
	virtual ResultType		CheckAchievementCanFinish(int achievementID);
	bool					IsFinishedAchievement(int achievementID);

	virtual void			BeginSkillCast(float fStartTime,float fTotalTime,const StringType& rkSkillCastName);
	virtual void			InterruptSkillCast(float fInterruptTime);
	virtual void			EndSkillCast();

	//坐骑装备-------
	//void							ReqEquipVehicleEquipment( int iPackType, int iEntryIndex, int iVehicleIdx );
	//void							ReqUnEquipVehicleEquipment( int iVehicleIdx );
	//void							UpdateVehicleEquip();
	void							ChangeVehicleAvatar(int lNewEquipID);
	//void							ChangeVehicleAvatar( int lOldEquipID, int lNewEquipID );
	//int							GetCurVehicleEquipID();
	//-------

	//坐骑相关接口
	//int								GetVehicleGrownupValue(int iLevel);//坐骑成长潜力
	int								GetVehicleAvatarID(void);
	//int							GetVehicleCurrentLevel(void);	//这个函数移动到PlayerBase里面，因为Server的Buff要用到这个接口

	//成长系统 
	int								GetCurrentCimeliaRank(int iType);
	int								GetCurrentCimeliaAttriLevel(int iType);
	int								GetCimeliaOriginCharTypeID(int iType, int iLevel);
	StringType						GetCimeliaAttributeList(int iType, int iStage);
	int64_t							GetCimeliaAttribute(int iType, int iRank, int iAttrib, int iAttribLevel);
	int64_t							GetCimeliaAttributeByRank(int iType, int iRank, int iPlayerLevel, int iAttrib, bool withBase = false);
	StringType						GetCimeliaUpgradeItemString(int iType);
	StringType						GetCimeliaName(int iType, int iLevel,bool bOriginal);
	StringType						GetCimeliaDisPlayLevelInfo(int iType, int iRank);
	int32_t							GetSubordinateAttrPlusValue(int32_t iSubType, int32_t iAttributeType);
	int32_t							GetSubordinateAttrPlusPercent(int32_t iSubType, int32_t iAttributeType);
	int								GetTalismanUpgradeNeeedLiveness(int iLevel);
	StringType						GetCimeliaDrugInfo(int iType);
	StringType						GetCimeliaAttribPlusInfo(int iType, int iItemID, bool bPercent);
	StringType						GetCimeliaTransformAttribInfo(int iType);
	StringType						GetCimeliaTransformInfo(int iType);
	StringType						GetCimeliaNamePic(int iType, int iLevel);
	bool							IsTransformActive(int iType, int iTransformID);
	void							SyncSubordinateNormalTransform(int iSubType);
	void							SetFirstSubTransform(int iSubType);

	void							UpdateCimeliaGainNew(int iType,int iTransformId,bool bValue);
	bool							GetCimeliaGainNew(int iType,int iTransformId);
	void							SetCimeliaGainNew(int iType,int iTransformId);
	//星魂相关接口
	void							RequestEquipRune(int iSrcBuddyId, int iSrcSlot, int iTarBuddyId, int iTargetSlot);
	void							RequestOnekeyEquipRune(int iBuddyId);
	void							RequestUnEquipRune(int iBuddyId, int iSlot);
	void							RequestOnekeyUnEquipRune(int iBuddyId);
	void							RequestLevelUpRune(int iBuddyId, int iSlot, int iTargetLevel);
	void							RequestStarUpRune(int iBuddyId, int iSlot);
	void							RequestResetUpRune(int iBuddyId, int iSlot);
	void							RequestRuneReward(int iRewardID);
	LC_BackPackEntry*				GetRuneEntry(int iBuddyId, int iIndex);
	void							OnUpdateRune(int iItemID, LC_BackPackEntry* pkEntry);
	void							OnRuneUpdated();


	//获取当前的上马技能ID
	int								GetCurrentVehicleSkill(void);

	//自动升级相关
	void							SetIsAutoUpgradeCimelia(int iType, bool bAutoUpgrade);
	bool							GetIsAutoUpgradeCimelia(int iType);

	void							SetIsDoAutoUpgradeCimelia(int iType, bool bAutoUpgrade);
	bool							GetIsDoAutoUpgradeCimelia(int iType);

	//自动升级技能
	void							SetIsAutoUpgradeSkill(bool bUpgrade) { m_bAutoUpgradeSkill = bUpgrade; }
	bool							GetIsAutoUpgradeSkill(void) { return m_bAutoUpgradeSkill; }
	void							SetIsDoAutoUpgradeSkill(bool bUpgrade) { m_bDoAutoUpgradeSkill = bUpgrade; }
	bool							GetIsDoAutoUpgradeSkill(void) { return m_bDoAutoUpgradeSkill; }
	void							SetCurrentUpgradeSkillID(int iSkillID) { m_iCurrentUpgradeSkillID = iSkillID; }
	int								GetCurrentUpgradeSkillID(void){return m_iCurrentUpgradeSkillID;}
	void							SetCurrentUpgradeBuddyID(int BuddyID) { m_iCurrentUpgradeBuddyID = BuddyID; }
	int								GetCurrentUpgradeBuddyID(void){return m_iCurrentUpgradeBuddyID;}

	ResultType						MoveItemInPackage(int iSrcPackType, int iSrcIndex, int iDstPackType, int iDstIndex, int iNum);
	int								IsItemGem(int itemId);

	//拆除宝石
	ResultType						RequestUnSocketItemInPackage(int iEquipPackType, int iEquipIndex, int lEquipItemID, int iSocketIndex);

	ResultType						RequestUseItemTriggerTask(int iPackType, int iEntryIndex);

	// 背包操作：放入仓库
	ResultType						CheckPutIntoWarehouse(int& iTargetIndex, int iTargetPackType);
	void							RequestPutIntoWarehouse(int iPackType, int iEntryIndex, int iTargetPackType);
	// 背包操作：从仓库中取出
	ResultType						CheckTakeOutFromWarehouse(int iSrcIndex, int iSrcPackType,int& iPackType, int& iTargetIndex);
	void							RequestTakeOutFromWarehouse(int iSrcIndex, int iSrcPackType);
	// 背包操作：从宝箱包裹中取出
	void							RequestTakeOutFromTreasureBag(int iPackType, int iSrcIndex);
	//End 物品操作接口
	virtual ResultType				RequestBuyGoodsFromNPC(int iIndex, int iNumber, int iPackType, int iPackIndex, int NpcCharType, bool bSendLogicMessage);

	//家园操作临时
	virtual ResultType				RequestBuyGoodsFromGuildNPC(int iShopEntryIndex, int iNumber, int iPackType, int iPackIndex);
	void							RequestSaveCash2Guild(int iGold,int iSilver,int iCopper);

	//洗练装备
	void							RequestRefineItemInPackage(int iEquipPackType, int iEquipIndex, int iItemID, int iAttribIndex);
	//选择洗练属性
	void							RequestApplyRefineItem(int iEquipPackType, int iEquipIndex, int iItemID, int iUseNew);
	//改名
	bool							RequestModifyName(const StringType& sName, int iSlotID);
	// 仓库操作
	ResultType						SaveCashToWarehouse(int iGold,int iSilver,int iCopper);
	ResultType						GetCashFromWarehouse(int iGold,int iSilver,int iCopper);

	//Begin 状态控制
	void							SetEnablePathFinding(bool bEnable){m_bEnablePathFinding = bEnable;};
	bool							GetNeedLockTarget(){return m_bNeedLockTarget;};	//目标型技能，类型0。这个标志位神马意思？
	void							SetNeedLockTarget(bool bNeed){m_bNeedLockTarget = bNeed;};
	bool							GetNeedLockLocation(){return m_bNeedLockLocation;};	//方向型技能；地点型技能，类型2，不是朝向型技能。这个标志位神马意思？
	void							SetNeedLockLocation(bool bNeed){m_bNeedLockLocation = bNeed;};
	bool							GetNeedLockTradeTarget(){return m_bNeedLockTradeTarget;};
	void							SetNeedLockTradeTarget(bool bNeed){m_bNeedLockTradeTarget = bNeed;};
	void							SetMoveLineTargetLocation(const UT_Vec3Int& kLocation){m_kMoveLineTargetLocation = kLocation;};
	UT_Vec3Int						GetMoveLineTargetLocation(){return m_kMoveLineTargetLocation;};
	void							SetMouseOverLocation(const NiPoint3& rkMouseLocation ){m_kMouseOverLocation = rkMouseLocation; };
	const NiPoint3	&				GetMouseOverLocation(){return m_kMouseOverLocation;};
	int								GetSaveWatchTargetID(){return m_lSaveWatchTargetID;};
	int								GetSavedLockedTargetID(){return m_lSaveLockedTargetID;};
	void							RestoreTarget();
	void							SetLbPickMoveFlag(bool kFlag) {	m_lpickMoveFlag	= kFlag; }
	bool							GetLbPickMoveFlag()	{	return m_lpickMoveFlag; }
	void							SetKeyTabPressed(bool bPressed){m_bKeyTabPressed = bPressed;};

	void							SetResourceRetrieve(bool bResourceRetrieve) { m_bResourceRetrieve = bResourceRetrieve; }
	bool							GetResourceRetrieve() { return m_bResourceRetrieve; }

	//套装收集
	void							SyncCmdEquipSuitCollection(const std::map<uint32_t,SingleCollectSuit>& suitMap);
	void							ReqCmdEquipSuitCollectReward(int iOpType, int iLevel, int iCount);
	//套装状态
	int								GetCmdEquipSuitCollectState(int iLevel);
	//单个装备状态
	int								GetCmdEquipSuitCollectItemState(int iLevel, int iEquipType);
	//套装奖励领取状态
	StringType						GetCmdEquipSuitCollectRewardStateInfo(int iLevel);
	//套装奖励红点
	bool							IsCmdEquipSuitCollectLevelPrompt(int iLevel);
	bool							IsCmdEquipSuitCollectPrompt();

	//资源找回
	void							SyncResourceRetrieveRecord(const MG_SyncResourceRetrieverRecord& msg);
	void							ReqResourceRetrieveReward(const StringType& sIds, bool bSpecial);
	StringType						GetResourceRetrieveRecordInfo();
	bool							GetResourceRetrievePrompt();
	int								GetResourceRetrieveRecordCommanderLevel() { return m_iResourceRetrieveLevelRecord; }
	int								GetResourceRetrieveRecordMapID(int iId);

	//< 鼠标引导功能
	bool							GetMoveStride(){return m_bMoveStride;};
	void							SetMoveStride(bool bMoveStride){m_bMoveStride = bMoveStride;};
	void							BeginMoveStride(void);
	void							EndMoveStride(void);
	void							ClearMoveStride(void);
	float							GetLastPickTerrainTime(){return m_fLastPickTerrainTime;};
	void							SetLastPickTerrainTime(float fTime){m_fLastPickTerrainTime = fTime;};
	float							GetLastSceneMouseOverTime(){return m_fLastSceneMouseOverTime;};
	void							SetLastSceneMouseOverTime(float fTime){m_fLastSceneMouseOverTime = fTime;};
	bool							GetLastSceneMouseOver(){return m_bLastSceneMouseOver;};
	void							SetLastSceneMouseOver(bool b){m_bLastSceneMouseOver = b;}
	float							GetLastPrepareMoveStrideTime(){return m_fLastPrepareMoveStrideTime;};
	void							SetLastPrepareMoveStrideTime(float fTime){m_fLastPrepareMoveStrideTime = fTime;};
	//> 鼠标引导功能
	//< WASD引导功能
	bool							GetWASDStride(){return m_bWASDStride;};
	void							SetWASDStride(bool bMoveStride){m_bWASDStride = bMoveStride;};
	//> WASD引导功能

	virtual void					SetAttackerID(object_id_type lID);
	virtual StateMachine::StateType	FindMoveState();
	//End	状态控制

	//Begin 物品交易接口
	void                    RequestTrade(unique_id_type lTradeID);
	void					RltTrade(unique_id_type lTargetID);			//服务器返回请求交易结果
	void                    RequestConfirmReplyTrade(unique_id_type lRequestID,int iReply);
	void                    RequestAddItemToTradePack(int iBackPackIndex,int iTradePackIndex,int iItemType = 0);
	void                    RequestRemoveItemFormTradePack(int iTradePackIndex);
	void                    RequestUpdateTradeCash(int64_t llCash, int32_t iCashType);
	void                    RequestLockTradeData();
	void                    RequestUnLockTradeData();
	void                    RequestFinishTrade();
	void                    RequestInterruptTrade();
	void					SetIsNeedLockTradeTarget(bool bISNeedLock);
	int						GetLastTradeRequesterID();
	//End   物品交易接口

	//武器相关
	void					SetEquipWeapon(int lNewItemID);		//设置当前手中的武器种类
	bool					ChangeAvatar(int lOldEquipID,int lNewEquipID);
	void					ResetLastEquipData();
	void					SetLastEquiptID(int equipID, int equiptType);
	int						GetLastEquiptID(int equiptType);

	//更新公会特殊服装Avatar
	void					UpdateSpecialEquipData();

	bool					_updateWeaponAvatar();   
	bool					_updateVehicleAvatar();
	bool					_updateBowAvatar(int lNewEquipID);
	bool					_updateWingAvatar();
	void					DoUpdateWing(int llevel);
	void					DoUpdateBowAvatar();
	void					DoUpdateWingAvatar();  
	void					DoUpdateAnqiAvatar();
	void					DoUpdateFabaoAvatar();
	void					DoUpdateWeaponAvatar();
	void					_updateSuitFollow(LC_ClientNPC* pkNPC);
	void					RefreshAnQiZFix(object_id_type vehicleCharType);

	void					DoUpdateFB(int llevel);

	void					DoUpdateLC(int lLevel);

	void					DoUpdateLCWeapon(int lLevel);

	void					DoUpdateAQ(int iLevel);

	virtual int				GetCurrentEquipItem(int iEquipType);
	virtual bool			IsEquipVehicleWeapon();
	virtual bool			IsArtifactWeapon();

	/// 统一换装前后调用
	void					HideBeforeSuitApplied();
	void					ShowAfterSuitApplied();

	//天眼
	bool					_updateEyeAvatar();
	void					_clearEyeAvatar(void);

	//剑池
	void					_updateJC();

	//灵宠
	void					_updateLC(float fCurrentTime, float fDeltaTime);

	//灵武
	void					_updateLCWeapon();

	//暗器
	void					_updateAnQi();

	//Begin 技能树操作
	//virtual GameLogic::LC_SkillAsset*	GetSkillAsset(){return LC_PlayerBase::GetSkillAsset();};
	virtual GameLogic::LC_SkillBookAsset&		GetSkillBookAsset(){return LC_PlayerBase::GetSkillBookAsset();};

	ResultType							PendingSkillInSkillAsset(int lSkillTypeID);
	//End 技能树操作

	void                    RequestCollectItem(int lID);

	//进入相位副本，播放boss特写相关的处理
	bool					OnEnterPhaseMap(int iMapResID);
	
	//与特殊npc交互时需要判断npc的归属
	ResultType				RequestTalkToNPC(object_id_type lTargetID);
	virtual	bool			TalkToNPC(object_id_type lTargetID);
	void                    StopCurrentNPCSound();
	bool					CheckPendingTalkNPC(object_id_type iTargetCharType, float fCurrentTime);
	void					CheckPendingCollectTreasure(object_id_type lID);
	void					ActiveNPCFunc(object_id_type lID);
	void					PathToPoint(const Utility::UT_Vec3Int& pos, bool bForceAvenue = true);
	void					PathToPointLocation(int iX,int iY,int iZ,bool bForceAvenue = true);
	void					TestArrival();
	bool					GetArriveFlag();
	float					GetSearchPathMiniMapX();
	float					GetSearchPathMiniMapY();
	void					NotInSamePath();

	//void					LoadNPCTaskInfo(int iNPCID);
	//观察目标
	void					SetWatchTargetID(object_id_type lTargetID){m_lWatchTargetID = lTargetID;};
	object_id_type			GetWatchTargetID(){return m_lWatchTargetID;}
	void					WatchTarget(object_id_type lLockID);
	void					SetMouseOverTarget(object_id_type lOverID);
	void					SetMouseOverInteractiveBtn(object_id_type lOverID);
	object_id_type			GetMouseOverTarget(void) {return m_lMouseOverActorID;}
	void					SetWatchTargetFactionChange(bool bChange){m_bWatchTargetFactionChange = bChange;};
	void					SetMouseOverTargetArg(MouseOverTargetArg *pkArg = NULL);

	object_id_type			GetPreLockedTargetID() {	return m_lPreLockedTargetID; }
	void					SetPreLockedTargetID(object_id_type lID)
	{
		if( !IsFreezeLocked() ) 
		{
			if (m_lPreLockedTargetID != lID)
				m_pPreLockedTargetModel = NULL;

			m_lPreLockedTargetID = lID;
		}
	}

	virtual	void			SetLockedTargetID(object_id_type lID)	
	{
		LC_ActorBase::SetLockedTargetID(lID);
	}

	bool					IsFreezeLocked()	{	return (m_bIsFreezeLock && GetPreLockedTargetID() != 0); }
	void					FreezeLocked(bool flag) {m_bIsFreezeLock = flag;}

	struct lessThan : public std::greater<GameLogic::LC_ActorBase*>
	{
		lessThan(Utility::UT_Vec3Int kPlayerLocation, LC_IDSet attackIdSet)
		{
			m_kPlayerLocation = kPlayerLocation;
			m_kAttackerIDSet = attackIdSet;
		}
		bool operator()(GameLogic::LC_ActorBase*& pkActor1, GameLogic::LC_ActorBase*& pkActor2) const
		{
			int iDistance1 = Utility::UT_MathBase::LineLengthXYInt(m_kPlayerLocation, pkActor1->GetCurrentLocation());
			int iDistance2 = Utility::UT_MathBase::LineLengthXYInt(m_kPlayerLocation, pkActor2->GetCurrentLocation());

			bool find1 = false;
			bool find2 = false;

			find1 = m_kAttackerIDSet.find(pkActor1->GetID()) != m_kAttackerIDSet.end();
			find2 = m_kAttackerIDSet.find(pkActor2->GetID()) != m_kAttackerIDSet.end();

			bool Is1Player = IS_PLAYER_ID(pkActor1->GetID());
			bool Is2Player = IS_PLAYER_ID(pkActor2->GetID());
// 
// 			//找到在前面
// 			if ( find1  && !find2 )
// 			{
// 				return true;
// 			}
// 			else if ( !find1 && find2 )
// 			{
// 				return false;
// 			}
// 			//都找到玩家在前面
// 			else
			if ( Is1Player && !Is2Player)
			{
				return true;
			}
			//都找到玩家在前面
			else if (!Is1Player && Is2Player)
			{
				return false;
			}
			//都找不到，距离判断
			else
			{
				return (iDistance1 < iDistance2 );
			}
		}
		Utility::UT_Vec3Int m_kPlayerLocation;
		LC_IDSet			m_kAttackerIDSet;
	};


	void					ManualChangeLockedTarget();		//单纯随机
	void					ManualChangeLockedTargetFunc1(); //特殊规则，计算距离
	void					ClearChangeLockedFlag();	//清除选中的标志

	//PVP目标
	void					SetPVPTargetID(object_id_type lTargetID){m_lPVPTargetID = lTargetID;}
	object_id_type			GetPVPTargetID(void){return m_lPVPTargetID;}

	virtual bool			NotifyStateEnd(int iEndState,float fCurrentTime,float fDeltaTime);

	//pkbuff
	bool					ClearPkDebuffID();


	//-------------         Begin 技能         ---------------------//
	virtual void			NotifySkillEvent(Skill::SK_SkillEvent* pkSkillEvent);
	virtual void			_onCurrentFightStateChanged(bool bFightState);

	//挂起技能，以及触发技能的物品
	virtual ResultType			PendingSkill(int lSkillTypeID,int lItemID);
	virtual	void				ClearPendingSkill();
	//技能挂起成功后，设定人物方向、锁定位置等
	virtual void				OnPendingSkillSucceed(int lSkillID, LC_LogicObject *pkTarget);	//pkTarget can be NULL
	//检查技能是否可以挂起，
	virtual ResultType			CheckPendingSkill(int lSkillTypeID);
	//检查是否可以挂起技能，如果失败，则尝试使用普通攻击，该接口主要用于快捷栏UI
	int							CheckPendingSkillEx(int lSkillTypeID, bool bContinueNormalAttak);
	//检查技能是否能够使用，以及触发物品是否有效
	virtual ResultType			CheckExecuteSkill(float fCurrentTime,Skill::SK_SkillExecute* pkSkill,int lItemID);	
	//准备执行技能
	virtual ResultType			PrepareExecuteSkill(Skill::SK_SkillExecute* pkSkill,int lItemID);
	//执行技能，并设置状态机，但不执行Update
	virtual ResultType			ExecuteSkill(float fCurrentTime,Skill::SK_SkillExecute* pkSkill,int lItemID);
	//方向型瞬发技能，不修改状态机当前状态
	ResultType					ExecuteSkillImmediately(float fCurrentTime,Skill::SK_SkillExecute* pkSkill,int lItemID);
	//目标型灵宠技能，不修改状态机当前状态
	ResultType					ExecuteLCSkillTargetImmediately(float fCurrentTime,Skill::SK_SkillExecute* pkSkill,object_id_type lLocked);
	//方向型灵宠技能，不修改状态机当前状态
	ResultType					ExecuteLCSkillImmediately(float fCurrentTime,Skill::SK_SkillExecute* pkSkill,object_id_type lLocked, const UT_Vec3Int& lockLocation, float dirAngle);
	//请求执行技能
	void						RequestExecuteSkill(float fCurrentTime,Skill::SK_SkillExecute* pkSkill,int lItemID);
	void						RequestExecuteToyPetSkill(float fCurrentTime,Skill::SK_SkillExecute* pkSkill);
	void						OnRequestExecuteSkillFailed(int lSkillID);

	int							GetCurrentSkillTypeID();
	//当前请求执行的技能记录，服务器返回后清除标志
	void						_doUnlockRequestExecuteSkill(bool bExecute, int lSkillID);
	void						_updateUnlockRequestExecuteSkill(float fCurrentTime);
	void						_clearUnlockRequestExecuteSkill(void);
	//脚本请求执行技能			
	void						RequestExecuteSkillScript(float fCurrentTime, int lSkillid, int lItemID);
	//请求中断技能
	virtual void				RequestInterrputSkill(float fCurrentTime,int lSkillTypeID,SkillInterruptType eInterruptType);
	virtual ResultType			InterruptSkill(float fCurrentTime, int lSkillTypeID, SkillInterruptType eInterruptType);
	//等待挂起的技能，需要输入目标或地点后才能挂起
	ResultType					WaitPendingSkill(int lSkillTypeID,int lItemID);
	Skill::SK_SkillExecute*		GetWaitPendingSkillExecute(){return m_pkWaitPendingSkill;};
	int							GetWaitPendingSkillItemType(){return m_lWaitPendingSkillItemType;};
	virtual void				ClearWaitPendingSkill();
	bool						WaitPendingSkillSuccess(int lSkillID);

	bool						_circleFindPosition(float lth, int angleSplit,const UT_Vec3Int& tarpos);

	void                        SetSkillActivateAll(int cID, bool bActivate);
	bool                        GetSkillActivateAll(int cID);
	
	void						ReSetRaidBossSpeciesRankThree();
	void						SetRaidBossSpeciesMyRank(const PS_SettleInfo& PlayerInfo);
	void						SetRaidBossSpeciesMyDamageCount(int iStoryId,int64_t iDamage);
	StringType					GetRaidBossSpeciesMyDamageCount(int iStoryId);
	void						SetRaidBossSpeciesRank(const std::vector<PS_SettleInfo>& PlayerInfo, bool rankThree);
	StringType					GetRaidBossSpeciesRank(int index, bool rankThree);
	StringType					GetRaidBossSpeciesMyRank();
	void						ReqGetRaidBossSpeciesRecord(int index, bool bSelf);

	//异种侵袭战斗数据
	void						RltRaidBossSpeciesDamageData(const MG_RLT_YiZhongQinXiDamageData& msg);
	void						RltRaidBossSpeciesDamageDataDetail(const MG_RLT_YiZhongQinXiDetailDamageData& msg);
	void						ReqRaidBossSpeciesDamageData(int32_t iStoryID, uint64_t iCharID);
	void						ReqRaidBossSpeciesDamageDataDetail(int32_t iStoryID, uint32_t iTime, uint64_t iCharID);

	void						SetBuddyRuneLevel(int cID, const mem::vector<int>& runelevel);
	const mem::vector<int>&		GetBuddyRuneLevel(int cID);

	void						SetBuddyEquipLevel(int cID, const mem::vector<int>& equiplevel);
	void						SetBuddyEquipLevel(int cID, int index, int level);
	const mem::vector<int>&		GetBuddyEquipLevel(int cID);

	void						SetUpBuddyEquipGrade(int cID, int index);
	void						SetBuddyEquipGrades(int cID, const mem::vector<int>& equipGrade);
	const mem::vector<int>&		GetBuddyEquipGrades(int cID);

	bool						CanSendLogicInfoMessageWithCDTime(long lMsgType);

	void						SetTestDataBase(const std::vector<int32_t> & topicID);
	StringType					GetTestDataBase();


	//记录缓存的技能
	void						RecordLastSkillID(int lSKillID); 

	//跳跃
	virtual ResultType		CheckPendingJump(StateMachine::StateType st);
	virtual ResultType		PendingJump(StateMachine::StateType st, UT_Vec3Int & kTargetLocation);
	//请求执行技能
	void					RequestJump(StateMachine::StateType st,UT_Vec3Int & kTargetLocation, int jumpType);

	virtual void			MoeveAfterJump();

	void					QuickPick();			//快捷拾取
	bool					FastPick();
	StringType				GetInFovNetPlayerIDs();
	void					CallFriend(GameLogic::LC_Attacker* pkScriptCaller, int lCharTypeID);
	void					ReqReturnToCity(void);		//触发回城，执行回城技能，技能结束调用SC_ClientSkillInterface::ReturnToCity
	void					ReturnToCity(void);			//回城技能执行成功
	void					ImmReturnToCity(void);		//立即回城
	void					OnTranscriptionSucceed(void);	//传送结束，地图load完
	void					_updateTranscriptionSucceed(float fCurrentTime,float fDeltaTime);	//传送结束，地图load完
	bool					IsNeedToEnalbeGuide();	//是否需要开启自动任务

	//bool					IsTransformAttackSkill(int lSkillID);

	//技能释放失败原因
	void					SetSkillFailReason(int& lSkillFailReason, int iReasonIndex, bool bFail);
	bool					GetSkillFailReason(int lSkillFailReason, int iReasonIndex);

	void					SetBExecuteSkill(bool bExecute) { m_bExecuteSkill = bExecute;}
	bool					GetBExecuteSkill() { return m_bExecuteSkill; }

	
	//-------------         End 技能         ---------------------//

	LC_ClientTaskMap* GetClientTaskMap() {	return (LC_ClientTaskMap*)m_pkTaskMap;	}
	void	ReqAcceptTask(int lTaskID,bool bCheckAssign=true);
	bool	ReqFinishTask(int lTaskID,int iIndex);
	void	ReqCancelTask(int lTaskID,bool bRecordFail = false);
	void	ReqShareTask(int lTaskID);
	void	ReqTakeChapterReward(int iChapterID);
	void	ReqTakeReachReward(int iReachID);
	void	ObtainRewardFlag(bool kFlag);

	void	RltAcceptTask(int lTaskID);
	void	RltJoinInTask(int lTaskID);
	void	RltFinishTask(int lTaskID,int iItemIndex);
	void	RltCancelTask(int lTaskID);
	void	RltShareTask(int lTaskID);

	void	AcceptGuideTask(LC_Task* pkTask);
	void	FinishGuideTask(LC_Task* pkTask);
	bool	IsNeedToGuideTask();
	void	RequestConfirmShareTask(int lPublisherID,int lTaskID,bool bAccept);
	void	RequestNocticeTaskEvent(int lTaskID,int iEventType,int lParam1 = 0, int lParam2 = 0);
	void	RequestBillBoardTask(int lBillBoardID);
	void	UpdateNearNPCTaskState();
	void	UpdateNearNPCDynamicCollision();
	void	SetTaskObjectPickState(int lCharTypeID,bool bCanPick);
	void	SetNPCTaskState(object_id_type lNPCID);

	//击退、击飞
	virtual void			PunchMove(SkillMoveActorPositionType Type, const Utility::UT_Vec3Int & kTargetPoint,float fTime);

	// 金钱接口
	StringType				GetCashString();
	void					SetBindCash(MoneyType llCash);
	void					SetUnBindCash(MoneyType llCash);
	StringType				GetUnBindCashString();
	StringType				GetWarehouseCashString();	
	StringType				GetYuanBaoString();
	StringType				GetUnBindYuanBaoString();
	StringType				GetJusticePTString();
	StringType				GetCouragePTString();
	StringType				GetArenaPTString();
	StringType				GetMonopolyPTString();
	StringType				GetRuneCreamString();
	StringType				GetStarSoulString();
	StringType				GetStarMoneyString();
	StringType				GetBuddyExpMoneyString();
	StringType				GetCourageString();
	StringType				GetEnergyString();
	StringType				GetTrialString();
	StringType				GetMagicCubeString();
	StringType				GetActiveMeritString();
	StringType				GetRewardMedalString();
	StringType				GetDailyMeritString();
	StringType				GetGuildMeritString();
	StringType				GetChapterHonourString();
	StringType				GetBossJPString();
	StringType				GetAccountPointString();
	StringType				GetSkinPointString();
	StringType				GetRaidGloryPointString();
	StringType				GetYijituxiScoreString();
	StringType				GetEntrustTaskPointString();
	StringType				GetSkyArenaPointString();
	StringType				GetVehiclePointString();
	StringType				GetPetPointString();
	StringType				GetBackWingPointString();
	bool					IsNeedUnBindCash(MoneyType llNeedCash);

	int						GetSkyArenaPoint() { return m_iSkyArenaPoint; }
	void					SetSkyArenaPoint(int iPoint);
	int						GetSkyArenaHonorPoint() { return m_iSkyArenaHonorPoint; }
	void					SetSkyArenaHonorPoint(int iPoint) { m_iSkyArenaHonorPoint = iPoint; }
	int						GetLastSkyArenaPoint() { return m_iLastSkyArenaPoint; }
	void					SetLastSkyArenaPoint(int iPoint) { m_iLastSkyArenaPoint = iPoint; }
	int						GetSkyArenaSeason() { return m_iSkyArenaSeason; }
	void					SetSkyArenaSeason(int iSeason) { m_iSkyArenaSeason = iSeason; }
	uint32_t				GetSkyArenaCloseTime() { return m_iSkyArenaCloseTime; }
	void					SetSkyArenaCloseTime(uint32_t iTime) { m_iSkyArenaCloseTime = iTime; }
	uint32_t				GetSkyArenaNxtTime() { return m_iSkyArenaNxtTime; }
	void					SetSkyArenaNxtTime(uint32_t iTime) { m_iSkyArenaNxtTime = iTime; }
	int						GetGroupID() { return m_iGroupId; }
	void					SetGroupID(int iGroupID) { m_iGroupId = iGroupID; }

	void					ReqGetSkyArenaBattleInfo(bool bRefresh, int MapID, int iSchemeType);

	void					SetCommissionBuyYuanBaoNum(int lValue)		{ m_lCommissionBuyYuanBaoNum = lValue; }
	void					SetCommissionSellYuanBaoNum(int lValue)	{ m_lCommissionSellYuanBaoNum = lValue; }
	void					SetQuickBuyYuanBaoNum(int lValue)			{ m_lQuickBuyYuanBaoNum = lValue; }
	void					SetQuickSellYuanBaoNum(int lValue)			{ m_lQuickSellYuanBaoNum = lValue; }

	int						GetCommissionBuyYuanBaoNum() const			{ return m_lCommissionBuyYuanBaoNum; }
	int						GetCommissionSellYuanBaoNum() const			{ return m_lCommissionSellYuanBaoNum; }
	int						GetQuickBuyYuanBaoNum() const				{ return m_lQuickBuyYuanBaoNum; }
	int						GetQuickSellYuanBaoNum() const				{ return m_lQuickSellYuanBaoNum; }

	void					SetCommissionBuyYuanBaoPrice(int lValue)	{ m_lCommissionBuyYuanBaoPrice = lValue; }
	void					SetCommissionSellYuanBaoPrice(int lValue)	{ m_lCommissionSellYuanBaoPrice = lValue; }
	void					SetQuickBuyYuanBaoPrice(int lValue)		{ m_lQuickBuyYuanBaoPrice = lValue; }
	void					SetQuickSellYuanBaoPrice(int lValue)		{ m_lQuickSellYuanBaoPrice = lValue; }

	int						GetCommissionBuyYuanBaoPrice() const		{ return m_lCommissionBuyYuanBaoPrice; }
	int						GetCommissionSellYuanBaoPrice() const		{ return m_lCommissionSellYuanBaoPrice; }
	int						GetQuickBuyYuanBaoPrice() const				{ return m_lQuickBuyYuanBaoPrice; }
	int						GetQuickSellYuanBaoPrice() const			{ return m_lQuickSellYuanBaoPrice; }


	//Begin 组队接口
	void							RequestCreatePlayerGroup();
	void							RequestExitPlayerGroup();
	void							RequestJoinPlayerGroup(unique_id_type lCharID);
	void							RequestKickPlayerFromGroup(unique_id_type lCharID);
	void							RequestDismissPlayerGroup();
	void							RequestConfrimGroupJoin(int iCandidateIndex,bool bAccept); // 队长同意申请,传入index
	void							RequestConfrimGroupJoinByPlayerID(unique_id_type lPlayerID, bool bAccept);// 队长同意申请， 传入playerid
	void							RequestInvitePlayerToGroup(unique_id_type lPlayerID);		//  邀请玩家入队伍
	void							RequestConfirmInviteToGroup(unique_id_type lInvitePlayerID,int lGroupID,unique_id_type lBeInvitePlayerID,bool bAccept);// 队长确认是否可以邀请玩家
	void							RequestConfirmBeInviteToGroup(int iGroupIndex,bool bAccept);	//  玩家确认被邀请组队
	void							RequestConfirmBeInviteToGroupByGroupID(unique_id_type lInviteCharID, int iGroupID, bool bAccept); //玩家确认被邀请组队,通过groupid
	void							RequestAppointmentCheif(unique_id_type lCharID);
	void							RequestBlockPlayerJoin(int iCandidateIndex);
	void							RequestModifyQuality(int iIndex);
	void							RequestForbidJoin(bool bFlag);
	void							RequestSyncInviteGroupMemberList(int iIndex);		// 请求被邀请加入的队伍
	void							RequestClearCandidateList();
	void							RequestClearInviteGroupList();

	void							AddInvitePlayerGroup(int lGroupID, unique_id_type lInvitePlayerID, bool bVaild, bool bCombin, int iMapResID);
	void							RemoveInvitePlayerGroup(int lGroupID);
	int								GetInviteTeamMapResID(int iGroupIndex);
	GameLogic::LC_PlayerGroupBase*	GetInvitePlayerGroup(int iIndex);
	int								GetInvitePlayerGroupCount();
	void							ClearInvitePlayerGroup();
	bool							GetForbidJoinFlag();

	int								GetTeamMapResID();
	bool							GetTeamHintFlag();
	void							SetTeamHintFlag(bool bFlag);

	ResultType				CheckTranscriptionKickPlayer(object_id_type lPlayerID);
	ResultType				RequestTranscriptionKickPlayer(object_id_type lPlayerID, const StringType& kKickDescript);
	ResultType				AgreeTranscriptionKickPlayer(bool bAgree);
	//End 组队接口

	//屏蔽接口
	void					RequestAddForbidPlayer(unique_id_type lID, const StringType& strName);
	void					RequestRemoveForbidPlayer(unique_id_type lID);
	object_id_type			GetForbidPlayerID(int iIndex);
	StringType				GetForbidPlayerName(int iIndex);
	bool					IsForbidPlayer(unique_id_type lID);

	virtual bool			IsFriend(unique_id_type lTargetCharID);
	virtual bool			IsEnemy(unique_id_type lTargetCharID);

	//技能复活接口
	void					RequestConfirmSkillRespawn(object_id_type lRespawnSourcePlayerID,bool bAccept);
	//~技能复活接口
	
	//Begin PK接口
	void					RequestModifyPKSetting();
	void					RequestModifyPKSettingForce();
	void					RequestPKValue();
	//同意添加目标到仇人列表
	void					RequestAgreeAddToEnermy(unique_id_type lPlayerID);
	//End PK接口

	//Begin 聊天接口
	void					SetChatTargetID(unique_id_type lID);
	bool					SetChatChannel(int iChatChannel);
	unique_id_type			GetChatTargetID();
	int						GetChatChannel();
	bool					SendChatMsg(const StringType& Msg,int iTextLength, bool bAutoConsume = false);
	bool					SendFriendChatMsg(unique_id_type lCharID, const StringType& strMsg);
	bool					SendSavePrivateChatMsg();
	void					SetChannelLimitFlag(int iChannelType,bool bFlag);
	void					ConfirmChannelLimitFlag();
	void					SetChatTargetName(const StringType& strName );
	StringType				GetChatTargetName();
											  
	PrivateChatSaveSt&		GetSearchingChatTarget();
	bool					IsSearchingChatName(const StringType& name);
	void					EraseSearchingChatName();
	//End   聊天接口

	//决斗接口
	//void              InvitePlayerToDuel(object_id_type lID);
	//bool              IsBeInviteToDuel();
	//StringType        GetBeInviteInfo();
	//void              AcceptDuelInvite(bool bAccpet);
	//end 决斗接口

	//宝箱相关接口
	LC_PlayerFlopManager*	GetFlopManager(){return m_pkFlopManager;}
	//~宝箱相关接口

	//技能升级接口
	bool					IsNeedUnBindCashSkillUpGrade(int lSkillCateGory, int iSlotIndex);
	void					RequestSkillUpGrade(int lSkillID,int iMax, int cID);//, bool bConfirmUseUnBindCash);
	//请求自动升级技能
	void					RequestAutoUpgradeSkill(bool bStart, int iSkillID, int cID);
	//请求学习指挥官技能
	void					RequestLearnControllerSkill(int iSkillID, int iSubType);

	//~技能设升级接口

 	//技能状态取消接口		
	void					RequestDeactiveSkillState(int lSkillStateTypeID);

	//技能状态特效接口
	void					EndSkillStateGFX();
	void					BeginSkillStateGFX();

	int						GetTimeStampFromCurrent(int iSecond, int iMinute, int iHour, int iDay/* = -1*/, int iMonth/* = -1*/, int iYear/* = -1*/);
	int64_t					GetEndOfDayTimeStampFromCurrent(time_t iCurTime, time_t iTargetTime);

	//请求边境地图
	void					RequestAlliance();
	void					RltAlliance(int groupID, Protocol::UnitServerNodeList& li);
	void					RltEnterAlliance(int retType);
	LC_ClientAlliance*		GetAlliance()	{	return &m_kClientAlliance;	}

	//命骨
	void					RequestSoulOperation(int iType, bool bBindYuanbao);

	StringType				GetInvestmentPlanTaskID(int iType);
	StringType				GetInvestmentPlanScheduleID(int iType);

protected:
	static bool				_sortFunc(const LC_ItemDescriptionEntry *v1, const LC_ItemDescriptionEntry *v2);
	StringType				genRaidBossSpeciesInfoStr(const std::vector<PS_Buddy>& buddyInfo, const std::vector<PS_Boss>& monsterInfo);

public:

	//~技能状态特效接口
	void					RecvLogicAreaTraceFromGameServer(Protocol::ProtocolAreaTraceInfo& rkInfo);

	//传送功能
	bool					IsCurrentInTheArea(int lMap,int lArea);
	bool					IsNeedUnBindCashNPCTransfer(int iIndex,int iTransferType);
	void					RequestTranscriptionTransfer(int iTriggerIndex);
	void					RequestNPCSetTransferPos();
	void					SetRequestTriggerTransfer(bool bFlag){m_bRequestLogicTrigger = bFlag;};
	bool					RequestTransportAreaTransfer(int iTransferType, int iTransAreaID, int iMapResID, int iServerID = 0, int iLineNum = 0, bool bTribuMap = false);
	bool					RequestTransfer(int iTransferType, int iTransAreaID, int iMapResID, int iServerID = 0, int iLineNum = 0, bool bTribuMap = false);
	bool					RequestEnterCrossMap(int iStoryID, int iMapResID);
	void					RequestTransferBattlePlaybackMap(int iMapResID);
	void					RequestTransferToLocation(int iMapResID, int x, int y, int z, int iMapLogicID = 0, int iServerID = 0, int iLineNum = 0);

	bool					CheckCanTransferMap(int iMapResID);
	void					SetIsSwitchLine(int8_t bSwitchLine){m_bSwitchLine = bSwitchLine;};
	int8_t					GetIsSwitchLine(){return m_bSwitchLine;};
	void					SetIsSwitich(bool bIsSwitich) { m_bIsSwitich = bIsSwitich;};
	bool					GetIsSwitich() {return m_bIsSwitich;};

	//称号
	void					RequestSetTitle(int iTitleType,int iTitleIndex);
	bool					NeedRefreshTitleList();

	bool					IsNeedUnBindCashItemEnhance(MoneyType ulPrice);

	//生产
	LC_ClientProduction*	GetClientProduction(){return m_pkClientProduction;};
	void					LearnProduction(int lProductionType, int lProductionLevel);
	void					RequestConfirmForgetProduction(int lProductionType, bool bAccept);
	void					UpgradeProduction(int lProductionType);
	void					LearnFormula(int lFormulaID);

	//新手训练
	bool					SetBitVec(int nIndex, bool bSet);

	//void					HandlerGameEvent(GameLogic::LC_GameEvent& rGameEvent,StringType& rEventStringParam);

	//社交系统
	void					AddTempFriend(unique_id_type lCharID, const StringType& strName, int lCareer, int lLevel);
	void					AddFriend(unique_id_type lCharID, const StringType& strName, const StringType& strAskMsg);
	void					AddEnemy(unique_id_type lCharID, const StringType& strName);
	void					AddBlackList(unique_id_type lCharID, const StringType& strName);

	void					DeleteTempFriend(unique_id_type lCharID);
	void					DeleteFriend(unique_id_type lCharID);
	void					DeleteEnemy(unique_id_type lCharID);
	void					DeleteBlackList(unique_id_type lCharID);

	//请求同局域网的玩家信息
	void					ReqPlayerInfoNearBy();

	//请求心情改变
	void					ReqChangeSocietyInfo(int enumChange, const StringType& var1, int var2);

	void					ReqFriendStatus(int iSocietyType);
	void					ReqSocietyDeltaData(int iSocietyType);
	void					ReqRecommendFriend();
	ResultType				ReqAddAllRecommendFriends(const StringType& sPlayerUids);

	void					ReqFriendDetail(unique_id_type lCharID);

	void					AddFiendNick(unique_id_type lFriendID, const StringType& strName);
	void					SupportFiendNick(unique_id_type lFriendID, int iIndex);
	void					ReqGetFriendNick(unique_id_type lFriendID);

	void					CopyName(const StringType& strName, const StringType& strUserID);
	bool					CopyToClipBoard(const StringType& ClipStr);

	void					ReqBeAddFriend(unique_id_type lTargetID);
	void					ConfirmBeAddFriend(unique_id_type lTargetID, bool bAgree, const StringType& TargetName);
	void					SocietyCheckPkEvent( object_id_type lScrPlayerID, const StringType& strScrName, object_id_type lDestPlayerID, const StringType& strDestName );

	// 查询系统
	void					AccurateSearch(const StringType& strPlayerName, unique_id_type iCharID,const StringType& strAskMsg);
	bool					PrivateChatSearch(const StringType& strChatMsg, int cLenght);
	void					ResetDetailInfo();

	LC_ClientSocietyAsset*	GetSocietyAsset() { return &m_kClientSocietyAsset; }
	LC_ClientFriendMsgBroad*GetFriendMsgBroad() { return &m_kClientFriendMsgBroad; }
	LC_ClientSearchPlayer*	GetSearchPlayer() { return &m_kClientSearchPlayer; }
	LC_ClientViewPlayerInfo*GetViewPlayerInfo() { return &m_kClientViewPlayerInfo; }
	LC_ClientFriendNick*	GetFriendNick() { return &m_kClientFriendNick; }
	LC_ClientWorldLevelAsset* GetWorldLevelAsset() {	return &m_kClientWorldLevelAsset;	}

	//寻路相关
	virtual void			StopMove();
	void					OnMovePathEnd();
	void					ClearPathFinding(void);
	bool					ClearPathFindingByLButton(void);

	const UT_Vec3Int &		GetSearchPathTarget(void) const { return m_kSearchPathTargetPos; }
	//void					SetAutoFindPathFlag(bool bFlag){m_bAutoFindPathFlag = bFlag;};
	//bool					GetAutoFindPathFlag(){return m_bAutoFindPathFlag;};
	bool					GetMoveStraightFlag(){return m_bMoveStraightFlag;}
	void					SetMoveStraightFlag(bool flag){m_bMoveStraightFlag = flag;}
	Path::PF_Path&			GetMovePath() {return m_kMovePath;}
	LC_AcrossMapPath&		GetAcrossMapPath(){return m_kAcrossMapPath;};
	void					PendingPathToLocation(int iMapResID, int x, int y, int z, int iMapLogicID = 0);
	void					PendingPathToLocation(int iMapResID , const Utility::UT_Vec3Int& pos, int iMapLogicID = 0);	//挂起寻路地点
	void					PendingPathToVal(int iMapResID , int x, int y, int z, int val, int iMapLogicID);
	void					PathToPendingLocation();	//寻路到挂起的寻路地点
	void					ClearPendingPathFind();
	int						GetCurrentGudingMapResID()	{	return m_iPendingPathMapResID; }

	//寻路挂起行为
	const PathPendingActionArg	&	GetPathPendingAction(void) { return m_kPPAArg; }
	bool	HasPathPendingAction(void) { return m_kPPAArg.enmType != PPAT_NA; }
	void	PendingPathAction(PathPendingActionType t, int iTargetType, bool bSwitchNewBee = false, int iTargetMapLogicID = 0);
	void	ClearPathPendingAction(void);

	const PathPendingActionArg  &	GetPathPendingActionForFeiXie() {	return m_kPPAArgForFeiXie; }
	void	PendingPathActionForFeiXie(PathPendingActionType t, int iTargetType){
		m_kPPAArgForFeiXie.enmType = t;
		m_kPPAArgForFeiXie.iTargetCharTypeID = iTargetType;
	}

	void PlayBossWarningFx();
	void PlayDistortionScreenFx();
	void MotivateActionForFeiXie();

	void ClearPathPendingActionForFeiXie() {	PendingPathActionForFeiXie(PPAT_NA, 0);}

	const PathPendingActionArg & GetJumpPathPendingAction() {	return m_kJumpPPAA;	}
	void JumpPathPendingAction(PathPendingActionType t, int iTargetType, int iTargetMapLogicID=0)
	{
		m_kJumpPPAA.enmType = t;
		m_kJumpPPAA.iTargetCharTypeID = iTargetType;
		m_kJumpPPAA.iTargetMapLogicID = iTargetMapLogicID;
	}
	void	ClearJumpPathPendingAction() {	JumpPathPendingAction(PPAT_NA, 0);}

	virtual void PendingJumpPathAction();
	void	SetPPAArgRetryID(object_id_type iID)	{	m_kPPAArgRetryTalkID = iID; }
	object_id_type GetPPAArgRetryID()	{	return m_kPPAArgRetryTalkID;	}

	void	SetPendingObject(int val)	{	m_pendingOject = val;	}
	int		GetPendingObject()	{	return	m_pendingOject;}

	bool							IsPathFinding()const{return m_bIsPathFinding;}
	void							SetPathFinding(bool bFinding);

	bool							IsFindingPath() {return m_bIsPathFinding;}

	//Request Enter/Quit Transcription----sky add new interface
	void					OnReceiveRLTTranscriptionInfo(Protocol::MG_RLT_ENTER_Transcription &pkMsg);
	void					RequestLeaveTranscription();
	void					RequestLeaveArena(int iType, int iOpType);
	void					RequestReliveLeaveTranscription();
	void					SetHookRespawnFlag(bool flag) {	m_hookRespawnFlag = flag; }
	bool					HookRespawnFlag()	{	return m_hookRespawnFlag; }
	void					SetHookRespawnDeadFlag(bool flag) {	m_hookRespawnDeadFlag = flag; }
	bool					GetHookRespawnDeadFlag()	{	return m_hookRespawnDeadFlag; }
	void					SetHookRespawnPathFlag(bool flag) {	m_hookRespawnPathFlag = flag; }
	bool					GetHookRespawnPathFlag()	{	return m_hookRespawnPathFlag; }
	void					SetHookRespawnBuyFlag(bool flag) { m_bhookRespawnBuyFlag = flag; }
	bool					GetHookRespawnBuyFlag() { return m_bhookRespawnBuyFlag;}

	void					NotifyGameServerAlreadyInCenterInCareerLearnTranscription();
	void					ExecuteNPCFuction(int iFuncType, int iFuncID,int iFuncState);

	//
	void					OnReceiveRLTTransferToLocation(Protocol::MG_Rlt_TransferToLocation &pkMsg);

	void					RequestCareerLearn(object_id_type NPCID);
	void					SendRequestInfo(int iRequestType,int lParam1,int lParam2,int lParam3);
	void					SendConfirmRequestInfo(int iRequestType,int iResult,int lID);
	object_id_type			GetCareerSelectionNPCID();
	void					MoveToFixedPos(object_id_type npcID, int lParam);
	void					SendRequestExpandWarehouse(int iRowNum);

	void					RequestUpgradeNpc(int index = 0);
	void					RequestUpgradeNpcByUI(int index ,int iUpdateType);
	bool					IsInTranscription();
	bool					IsInSpecialRaidMap();

	void					RequestBeginSeekTreasure();
	void					RequestDigTreasure();
	
	//Request Enter/Quit Transcription----sky add new interface end

	//副本相关信息 ---临时版本
	///-----------------------------------------------------------------
	int						GetCurrentMapID();
	int						GetCurrentMapCategoryType();
	int						GetCurrentMapLogicType();
	int						GetWildBossPageByMapID(int lMapResID);
	int						GetGamestoryIDByMapID(int lMapResID);
	LC_ClientTranscriptionInfoManager* GetTranscriptionInfoManager();
	LC_ClientGameStoryManager* GetGameStoryManager();
	void					RequestVIPReward(int mapResID);
	///-----------------------------------------------------------------

	//合成真系统
	//bool					IsNeedUnBindCashComposition(MoneyType ulPrice);

	//至尊系统
	ResultType				RequestEnableTalent();
	ResultType				RequestUpdateTalent(int iFaction, int lFirstLevelTalentID);
	ResultType				RequestUpdateAllTalent();

	//骑乘
	virtual void			SetRiderCharType(int lCharType);

    void					EnableAreaEffect(void);
    void					DisableAreaEffect(void);

	//新手
	void					SetIsCanStartTutorialFlag(bool bIsCanStartTutorial){m_bIsCanStartTutorial = bIsCanStartTutorial;};
	bool					GetIsCanStartTutorialFlag(){return m_bIsCanStartTutorial;};

	void					SendServerPlayerAntiWallowState(AnitWallowState state);

	//快捷键
	void					SetWaitingShortcutKey(bool bWaitingShortcutKey){m_bWaitingShortcutKey = bWaitingShortcutKey;};
	bool					GetWaitingshortcutKey(){return m_bWaitingShortcutKey;};

	void					SetIsSaveShortcutKey(bool bIsSaveShortcutKey){m_bIsSaveShortcutKey = bIsSaveShortcutKey;};
	bool					GetIsSaveShortcutKey(){return m_bIsSaveShortcutKey;};

	void					RequestSaveShortcutKey();

	//设置武器不可见
	void					SetWeaponVsible(bool bVisible);
	void					SetBowVsible(bool bVisible, int suitPos);
	void					SetWingVisible(bool bVisible);

	//记录UI操作
	void					RecordUIOperations(int uiID);
	void					ClearUIOperationRecords();

	//设置反隐参数
	void					SetRevealInvisibilityParams(const UT_Vec3Int& rkCircleCenter, int iCircleRadius, bool bRevealOnce);
	
	//查询归属
	void					RequestNPCOwnerShip(object_id_type lNPCID);

	//加载挂机脚本
	void					LoadControlScript(bool bUseScript,bool bReload);

	//换线
	//void					RequestChangeServer(int iServerID);
	
	//分流
	void					SetNewPlayerLevel(int iLevel){m_iNewPlayerLevel = iLevel;};
	int						GetNewPlayerLevel(){return m_iNewPlayerLevel;};

	LC_ActorBase*			GetNearFirstActorByCharTypeID(int lCharTypeID);

	//离开监狱
	void                    RequestLeavePrison();
	bool                    IsBindCrashEnough();
	void                    RequestReducePKValue();
	int                     GetReducePkReqCash();
	int                     GetReucePkResultValue();

	//挂机
	void					RequestHookInfo();
	void					UpdateHook(float fCurrentTime,float fDeltaTime);
	void					SetHookForbidCount(int lHookForbidCount){m_lHookForbidCount = lHookForbidCount;};
	int						GetHookForbidCount(){return m_lHookForbidCount;};
	bool					IsHookArea();
	int						GetHookRadius();

	bool					IsHooking(){return m_bHooking;};
	void					SetHooking(bool hFlag);
	void					SetRaidHookingFlag(bool bFlag) { m_iRaidHookFlag = bFlag; }
	bool					GetRaidHookingFlag()	{	return m_iRaidHookFlag;	}
	void					SetPathtoTreasureFlag(bool bFlag) { m_bPathtoTreasure = bFlag; }
	bool					GetPathtoTreasureFlag()  { return m_bPathtoTreasure; }
	void					SetRaidStageHookAssist(int iIndex, int iStage, int MapResID);
	bool					IsAutoPicking() {	return m_bAutoPicking;}
	void					SetAutoPicking(bool bFlag);
	void					ResetHooking(void);
	int						GetHookTime(void){return m_lHookTime;};
	void					SetHookingParam(const StringType& info, long lParam, int iValue, bool bSynDataToServer);
	int						GetHookingParam(const StringType& info, long lParam);
	void					SynHookingSettingToServer(void);
	Utility::UT_Vec3Int&	GetHookLocation(void){return m_kHookLocation;};
	void					SetHookLocation(const UT_Vec3Int& loc){ m_kHookLocation = loc; }
	Utility::UT_Vec3Int&	GetHookGuideLocation(void){return m_kHookGuideLocation;};
	void					SetHookGuideLocation(const UT_Vec3Int& loc){m_kHookGuideLocation = loc;};
	void					SetHookingTargetID(object_id_type id, bool bStopOnTargetDead = false){
		m_iHookingTargetID = id;
		m_bStopHookOnTargetDead = bool(id != 0 && bStopOnTargetDead);
	}
	object_id_type			GetHookingTargetID(void){return m_iHookingTargetID;};
	bool					IsStopHookOnTargetDead(void){return m_bStopHookOnTargetDead;};

	void					SetSpecialHookCharType(object_id_type lCharTypeID) { m_kSpecialHookCharType = lCharTypeID; }
	object_id_type			GetSpecialHookCharType()	{	return m_kSpecialHookCharType; }

	void					SetHookGodSkillUsage(float fTime)	{	m_bHookGodUsageTime = fTime;	}
	float					GetHookGodSkillUsage()			{	return m_bHookGodUsageTime;		}

	LC_ClientRaidHookAsset&	GetRaidHookAsset();

	void					SetAssistCDTime(float cdTime){m_fAssistCDTime = cdTime;};
	float					GetAssistCDTime(void){return m_fAssistCDTime;};

	void					AddInvalidPickItem(object_id_type lItemID);
	bool					IsInvalidPickItem(object_id_type lItemID);
	
	LC_IDSet&				GetLastAttackerID(){return m_kLastAttackerID;};
	//更新最近攻击者的目标
	float					GetLastAttackerIDTick() { return m_fLastUpdateTime;}
	void					SetLastAttackerIDTick(float dTime) {m_fLastUpdateTime = dTime;}

	//礼包
	//void					RequestGetGift(int lGiftID, int lItemTypeID);

	//幸运日常
	bool					CheckRecvLuckAward();
	void					RequestRecvLuckAward();

	//跟随
	void					StartFollowTarget(object_id_type lTargetID);
	bool					StopFollowTarget(object_id_type err);	//true if following before stop, else false
	void					UpdateFollowTarget();
	//每日任务杀怪计数 2014/4/11 林培华
	int						GetDaylyTaskKillCount(int taskid);
	void					SetDaylyTaskKillCount(int taskid, int icount);
	//pvp战斗状态下播放屏幕泛红特效的id
	void					SetPVPGfxID(int iGfxID){m_iPVPGfxID = iGfxID;};
	int						GetPVPGfxID(){return m_iPVPGfxID;};

	void					RequestUseDoubleExpInfo();

	//------------begin 跳完地图后，lua发送给c++需要做的事情------------//
	void					ResponseFromLuaAfterMapChanged(int type);
	//------------end 跳完地图后，lua发送给c++需要做的事情------------//

	//帮会传送令传送
	void					RequestGuildTransport(int transportId);
	void					ClickGuildTransportIcon(const StringType& name,int transportId);
	
	//站立进入挂机时间
	bool					AccumulateStand2Hook(float fDeltaTime);
	void					ResetStand2Hook() { m_fStandToHookTimer = AFK_TO_COUNTER;}
	void					EnableStand2Hook(bool b) { m_bEnableAfkToMeditation = b;}
	float					GetStand2HookTime()	{	return m_fStandToHookTimer;	}

	//藏宝图
	void					ReqGetTreasurePos();
	void					RltGetTreasurePos(const MG_RLT_GetTreasurePos& msg);
	StringType				GetTreasurePosInfo();
	bool					IsCanDigTreasure();
	ResultType				CheckTreasureEvents(int iItemID);

	//公告
	void					ReqGameRestartReward(int iId);

	//新手全自动
	bool					Accumulate2Guding(float fDeltaTime);
	void					Reset2Guding(bool bEnale=false);
	void					Reset2GudingFirset();
	void					Enable2Guding(bool b, bool bImmediately=false);
	void					SetGuidingEnable(bool b) { m_bEnableToGuding = b; }
	bool					GetGudingEnable()	{	return m_bEnableToGuding;	}
	float					Get2GudingTime()	{	return m_fGudingTimer;	}
	void					StartGuide(int iTaskID);
	void					SetTutorialStop(bool b) { m_bTutorialStop = b; }

	void					SetIsDoingOtherTask(bool b) { m_bDoingOtherTask = b; m_iAutoTaskType = b ? STT_NONE : STT_MAIN_SCENARIO; }
	bool					GetIsDoingOtherTask() { return m_bDoingOtherTask; }

	int						GetAutoTaskType() { return m_iAutoTaskType; }
	void					SetAutoTaskType(int iTaskType) { m_iAutoTaskType = iTaskType; }

	bool					GetFinishTaskFlag() { return m_bFinishTaskFlag; }
	void					SetFinishTaskFlag(bool b) { m_bFinishTaskFlag = b; }

	void					StopGuide();

// 	//挂机进入全自动
	bool					AccumulateHooking2Guding(float fDeltaTime);
	void					ResetHooking2Guding() { m_fHooking2GudingTimer = AFK_TO_GUDING_PROCESS;}
	void					EnableHooking2Guding(bool b) { m_bEnableHookingToGuding = b;}
	float					GetHooking2GudingTime()	{	return m_fHooking2GudingTimer;	}

	//挂机技能失败
	float					GetInvalidHookSkillTime()	{ return m_fInvalidTime; }
	void					SetInvalidHookSkillTime(float fT)	{ m_fInvalidTime = fT;	}

	const UT_Vec3Int &		GetInvalidHookPos() { return m_kInvalidPos;}
	void					SetInvalidHookPos(const UT_Vec3Int& pos) { m_kInvalidPos = pos;}

	object_id_type			GetInvalidHookTargetID()	{	return m_fLastInvalidTarget; }
	void					SetInvalidHookTargetID(object_id_type iID)	{	m_fLastInvalidTarget = iID;}

	//轻功
	void					NoticeShowChingkungUI(bool bShow);

	//获取角色当前屏幕位置
	StringType				GetWorld2PtCurrentLocation(void);

	//拆解装备
	void					DecompositionEquip(int iPackType, int iEntryIndex);

	//获取玩家副本评分
	int						GetRaidScore(int raidID);
	int32_t					GetRaidRecord(int raidID);
	//判断玩家是否到过该区域的地图
	bool					HasMapCategoryArrived(int iMapCategoryID);
	UT_Vec3Int&				GetNpcTalkUIPos(int xOffset, int iUIHeight);

	void					RequestSetAssistInfo(int lTypeID);
	void					RequestGetHookingGuideLocation(void);
	void					SendRequestInfoByType(int iType, uint32_t lID, int iParam1, int iParam2, int iParam3=0);
	void					SendRequestInfoByTypeWithStringPram(int iType, const StringType& kParam);
	void					SendRequestRaidBossSpeciesRankInfo(int StoryID, int MinIndex, int MaxIndex);

	void					RequestRankInfo(int iRankID, int iBegin, int iEnd, int serverID, int areaID=0);  //请求排行榜列表  areaID=0表示本服
	void					RequestPlayerBuddyRankInfo(int iType, uint64_t iCharID, int serverID); //请求详细信息 buddy组合类型（上阵buddy:1  三英雄buddy:2）
	void					RequestBuddyRankInfo(int cID, uint64_t iCharID, int serverID);		//请求详细信息 单英雄排行榜
	void					RequestToyPetRankInfo(uint64_t iCharID, int serverID);				//请求详细信息  玩具
	void					RequestSubRankInfo(int iType, uint64_t iCharID, int serverID);		//请求详细信息  进阶线
	void					RequestRankTopnPlayerInfo(const StringType& kParam, int iTop, int iType); //请求多个榜的前n名 iType是buddy组合类型（上阵buddy:1  三英雄buddy:2） 名人堂用
	void					RequestBuddyRankIndex(uint64_t iCharID);					//请求一个玩家所有buddy的排名

	void					RequsetArenaGiftState();				//请求竞技场奖励状态
	void					RequsetArenaRankList(int iBegin, int iEnd, int serverID);

	void					RequestPlayerDetail(unique_id_type iUid, int iSystemType=PIS_NORMAL);
	bool					IsRedNameInvalidTTT(int iMapResID);
	void					SendRequestSameLocationDifLine(int MapID, int ServerID, int Line);
	void					SendRequestCommitAnswer(int topicIndex, int choiceIndex);
	void					SendRequestAnswerRankInfo(int MinIndex, int MaxIndex,int rankType);
	void					SendRequestYiJiTuXiRankInfo(int MinIndex, int MaxIndex, bool bFlag);

	//指挥官装备进阶
	void					EquipAdvanceReq(int InType, int InIndex);

	//进阶线装备进阶
	void					CimeliaEquipAdvanceReq(int InType, int InIndex);
	//玩家自动同意任意帮主向我发送的入帮邀请
	void					RequestMemberAutoAdd(bool bAdd);

	virtual ResultType		CanExitGuild(LC_GuildAsset* pkGuildAsset);
	virtual ResultType		CanModifyGuildDeclaration(LC_GuildAsset* pkGuildAsset);
	virtual ResultType		CanKickMemberFromGuild(LC_GuildAsset* pkGuildAsset);

	//Begin 竞技场
	int						GetArenaBattleDailyRanking(int iServerFlag);
	bool					GetArenaBattleDailyRewardValid(int iServerFlag);
	int						GetArenaBattleCurRanking(int iServerFlag);
	int						GetArenaBattleTopRanking(int iServerFlag);
	int						GetArenaBattleWinStreak(int iServerFlag);
	int						GetArenaBattleChallengeTimes(int iServerFlag);
	StringType				GetArenaBattleChallengeTimesStr(int iServerFlag);
	int						GetArenaBattleLocalChallengedTime(int iServerFlag);
	int						GetArenaBattleBuyCountCost(int iServerFlag);
	int						GetArenaBattleDailyBuyCount(int iServerFlag);
	int						GetLocalDailyChallengeMaxTime();
	LC_ClientArenaBattleChallengeInfoAsset* GetArenaBattleChallengeInfoAsset();
	bool					CanAddGood(object_id_type uid, int iType, int iServerFlag);
	int						GetLeftAddGoodCount();
	//End 竞技场相关

	//获得排名
	int						GetRankByType(int rankType, int serverType);

	void					CreatePortal();//创建传送门
	void					UpdatePortalData(const Protocol::MG_PortalAssert& kMsg);
	bool					IsGravePressed()const;
	bool					CheckGameStoryCondition(const GameStoryInfo& kGameStoryInfo, int iMapID = 0);
	
	//好友召唤设置接口
	unique_id_type			GetTmpSummorPool(int index);
	void					SetTmpSummorPool(int index, int val);
	int						GetTmpSummorCurCount();
	void					ClearTmpSummorPool();
	void					PlayerComboEffect(int iCount);
	void					RewardComboCount(int iOldValue, int iAddCount);
	LC_ClientNetPlayer*		GetSummoredPlayerData();
	void					SetSummorFriendCharID(uint32_t iID);
	//计算好友离线时间
	int						GetFriendOffLineTime(int offLineTick);
	//当前是否为禁止按键状态
	bool					 IsForbbidenKey(void) { return m_bForbbidenKey; }
	void					SetForbbidenKey(bool bForbbiden) { m_bForbbidenKey = bForbbiden; }

	bool					IsCanAcceptTaskLogin() { return m_bAcceptTaskLogin; }
	void					SetAcceptTaskLogin(bool bAccept) { m_bAcceptTaskLogin = bAccept; }
	void					AcceptTaskAfterLogin();

	LC_ClientRankUserInfo*	GetPlayerDetailInfoByUserID(unique_id_type iUserID);
	StringType				GetGuildWarData();
	//回城技能目标城市名字
	StringType				GetReturnCityName();
	//获得注册时间
	bool					IsRegistThisMonth();
	StringType				GetRegistTimeInfo();
	int						GetRegDays();
	
	//检查膜拜限制
	ResultType CheckAddGood(int goodtype,int playerid);
	StringType GetCanGoodCountStr();

	//获得活动积分
	int GetGameStoryScore( int type);

	bool					IsEquipBetter(LC_BackPackEntry *pkEntry);

	LC_TargetMap*			GetTabTargetMap() {	return &m_tabLockedMap;}
	
	int64_t					GetGuildTransportStamp(){ return m_lGuildTransportStamp;}
	void					SetGuildTransportStamp(int64_t stamp){ m_lGuildTransportStamp = stamp; }
	uint32_t				GetGuildTransportResId(){return m_lResId;}
	void					SetGuildTransportResId(uint32_t iResId){ m_lResId = iResId; }

	int						GetCommonStatReserveTimes(int32_t uiType);	//获得昨天未完成的次数
	bool					IsPurchaseBackOpen();					//获得资源购回的最低等级
	StringType				GetCommonStatCostMoney(int32_t uiType);	//获得消耗货币

	//新手帮助相关
	void                    SetTutorialProgress(int tutorialId){m_iTutorialProgress = tutorialId;};
	int                     GetTutorialProgress(){return m_iTutorialProgress ; };
	void                    AddRewardRecord(int tutorialId);
	bool                    IsAchievedReward(int tutorialId) ; 

	//苍穹段位奖励相关 
	void                    SetSkyArenaRankRewardAchievedId(int achievedId){m_iSkyArenaRankRewardId=achievedId;} ;
	bool                    RewardIsAchieved(int rewardId) ; 

	//篝火晚会 
	void                     SetLeftTimes(int leftTimes){m_leftTimes=leftTimes;}; 
	void                     SetBuyTimes(int buyTimes){m_buyTimes=buyTimes;}; 
	void                     SetRate(int rate){m_Rate=rate;}; 
	int                      GetLeftTimes(){return m_leftTimes;}; 
	int                      GetBuyTimes(){return m_buyTimes;}; 
	int                      GetRate(){return m_Rate;}; 
	StringType               GetBuyerInfo();  
	void                     SetBuyerInfo(const std::vector<int32_t> &numArgs){m_BuyerInfo=numArgs;} ; 
	StringType               GetBuyerName(){return m_BuyerName;};
	void                     SetBuyerName(StringType &name){m_BuyerName=name;};

	//巅峰竞技场 
	int                     GetSeasonStartTime(){return m_iStartTime ;} ; 
	int                     GetSeasonEndTime(){return m_iEndTime;}; 
	int                     GetStageType(){return m_stageType;} ; 
	int                     GetShowTime(){return m_showRemainTime;} ; 
	int                     GetStageTime(){return m_stageStTime;} ; 
	int                     GetStageEndTime(){return m_stageEndRemainTime;} ; 
	int                     GetBattleTime(){return m_battleRemainTime;} ;  
	void                    SetBattleTime(int time){m_battleRemainTime=time;} ; 
	int                     GetBattleTimeStamp(){return m_battleStTime;}; 
	int                     GetShowTimeStamp(){return m_showTime;} ; 
	void                    SetShowTime(int time){m_showRemainTime=time;} ;
	void                    SetOwnRank(int myRank){m_myRank=myRank ;} ; 
	int                     GetOwnRank(){return m_myRank;}; 
	int                     GetStageSetTime(){return m_stageSetRemainTime;}; 
	StringType              GetEveryGroupData(int groupId) ;
    StringType			    GetMyGroupData() ; 
	void                    SetAllGroupData(const std::vector<SummitArenaGroupStageData>& group); 
	void                    SetMyGroupData(const std::vector<SummitArenaGroupStageNeedBuddyData> & groupDatas) ; 
	void                    SetTotalGroupId(int totalGroupId){m_nTotalGroupId=totalGroupId;} ; 
	int                     GetTotalGroupId(){return m_nTotalGroupId;} ; 
	void                    SetAgainstGroup(const std::vector<SummitArenaAgainstList>& againstData,int groupId);  
	void                    SetCanEnterSummitArena(bool  RedPoint){m_summitRedPoint=RedPoint; } ; 
	bool                    GetCanEnterSummitArena(){return m_summitRedPoint;} ; 
	void                    SetMyTopType(int myType){m_myTopType=myType;}; 
	int                     GetMyTopType(){return m_myTopType;}; 
	void                    SetEntryList(SummitArenaGroupStageData& data){EntryList=data;} ; 
	StringType              GetEntryListData(); 
	StringType              GetAgainstData(int groupId); 
	void                    GetSummitArenaInvitationValid(int type,bool isValid); 
	bool                    GetInviteIsValid(){return m_isValid;};
	int                     GetInvitedType(){return m_invitedType;}; 
	//荣耀之巅
	void					ReqSummitArenaHonor(int nZoneId,int nIndex);
	void					ReqGetMyTopHonor();
	void					SetPlayerList(std::vector<SummitArenaHonorDataMsg>&	topPlayerList);
	StringType				GetTopPlayerInfo(int index);
	int                     GetZoneId(){return m_zoneId;};   //m_zoneId ;   //赛区Id 
	int						GetTopErr(){return TopErr;};
	void					SetTopErr(int topErr){ TopErr = topErr;};
	int						GetTopPlayersSize(){return TopPlayerList.size();};
	//苍穹最高分 
	int                     GetSkyArenaMaxPoint(){return m_iMaxScore;} ; 
	void                    SetSkyArenaMaxPoint(int maxScore){m_iMaxScore=maxScore;} ; 

	      



	//获取技能书对应技能的类型
	int						GetSkillBookCateGory2(int iPackType, int iSlot);
	//获取技能书当前学习对应的技能格索引信息
	int						GetUseSkillBookSlotIndex(int iCategory2);

	//圣痕相关
	void					RequestActiveSkillBook(int iID);
	void					RequestUnlockSkillBook(int iIndex, int iSlot);
	void					RequestWearSkillBook(int iIndex, int iSlot, int iSkillBookID);
	void					SyncSkillBookSlotInfo(const MG_SyncSkillBookSlotInfo& msg);
	int						GetSkillBookSlotSkillBookId(int iIndex, int iSlot);		//栏位里的圣痕ID
	bool					GetSkillBookSlotActive(int iIndex, int iSlot);			//栏位是否开启
	int						GetFirstEmptySkillBookSlot(int iIndex);			//获取第一个空位

	virtual void			HideGroundFx();
	virtual void			ShowGroundFx();

	void					ResetLogicAreaInfo();

	ResultType				CanCreateGuild(const StringType& strGuildName);
	virtual uint16_t		GetPlayerPlatformType() { return getYunYingplatform();}
	ResultType				CanDismissGuild(LC_GuildAsset* pkGuildAsset);

	void					SetIsNpcTask(bool bTask) { m_bNpcTask = bTask; }
	bool					GetIsNpcTask() { return m_bNpcTask; }
	void					ReqPlayerMove();
	void					SetAnimIndex(int8_t index) { m_iAnimIndex = index;}
	int8_t					GetAnimIndex() { return m_iAnimIndex; }
	void					SetLButtonPickActorID(object_id_type id) {m_PickActorID = id;}
	object_id_type			GetLButtonPickActorID() { return m_PickActorID; }

	bool					ReqTranscribeFightByUID(unique_id_type uiID, int iMapID);

	//异种降临
	void					SetGuildDamageRank(std::vector<PS_GuildDamageInfo> GuildInfo){m_nGuildInfo = GuildInfo;};
	StringType				GetGuildDamageRank();
	void					ReqGetGuildDamageRank(int32_t m_nStoryID,int32_t m_nMinIndex,int32_t m_nMaxIndex);
	void					ReqGetGuilfBossBeFallDemageRankInfo(int32_t iMapID);
	void					ReqGetGuilfBossBeFallGuildRankInfo(int32_t iMapID);

protected:
	ResultType				_checkTakeItemToPackbag(int iSrcIndex, int iSrcPackType,int& iPackType, int& iTargetIndex);
	void					_syncToServer( float fCurrentTime );
	void					_updateLogicAreaInfo();
	void					_notifyInGameToServer();

	void					_updateWatchedAndLockedTarget();
	void					_updateTargetSelectEffect();
	void					_updateWeaponVisibility();
	void					_updateDisarmState();
	void					_updateNPCFunction();
	void                    _updateTradeState(float fCurrentTime);
	void					_UpdateProcessingTask( float fCurrentTime );
	void					_UpdateAutoStartTask ( float fCurrentTime );
	void					_DoStartTask(LC_ClientTaskMap::AutoStartTaskSet& rAutoTaskList);
	void					_UpdateTriggerInfo( float fCurrentTime );
	void					_updateRevealInvisibility( float fCurrentTime );
	void					_updateAutoUpgradeCimelia(float fCurrentTime);
	void					_updateAutoUpgradeSkill(float fCurrentTime);
	ResultType				_checkNeedLockTarget(Skill::SK_SkillExecute* pkSkillExecute);	//检查当前是否已经锁定目标
	//ResultType				_checkSkillInSafeArea(int lSkillID, int lTargetID);			//判断是否在安全区内释放技能

	bool					_isCmdEquipSuitCollectLevelPrompt(const SingleCollectSuit& suitInfo);

	//当前自动进阶当前应该使用哪种payindex
	int						_getCurrentAutoUpgradeCimeliaPayIndex(int iType);

	void					_resetTriggerInfo();

	//释放技能时按下alt键，系统将自身设定为目标，并暂存原先的目标，
	//释放完技能后需要恢复原先的目标
	//void					_switchTarget(object_id_type lNewTargetID);
	void					_restoreTarget();
	void					_resetAutoTask();

	void					_syncFovToServer(float fCurrentTime);
    void					_initAreaEffectInfo(GD_ClientActorModel& rkVisibleData, bool IsRider);
	void					_clearAreaEffectInfo(GD_ClientActorModel& rkVisibleData);


	user_id_type					m_lUserID;

	int8_t							m_iAnimIndex;

	object_id_type					m_PickActorID;

	mem::vector<object_id_type>		m_controlIDVec;

	NW_Game2ClientSession*			m_pkClientLocalSession;
	int								m_lLastMoveSpeed;
	int16_t							m_LastDirAngle;
	float							m_fLastSyncMoveTime;
	float							m_fLastSyncUIOperationTime;
	float							m_fLastSyncFovTime;
	float							m_fLastRequestOwnerShipTime;

	MouseLogicModeType				m_eMouseLogicModeType;
	
	//上一次自动升级
	CimeliaUpgradeTimeMap			m_fLastAutoUpgradeCimeliaTime;
	//是否正在自动升级
	CimeliaUpgradeInfoMap			m_bAutoUpgradeCimelia;
	//执行下一次自动升级
	CimeliaUpgradeInfoMap			m_bDoAutoUpgradeCimelia;

	//一键升级技能
	//上一次自动升级时间
	float							m_fLastAutoUpgradeSkillTime;
	//是否正在自动升级技能
	bool							m_bAutoUpgradeSkill;
	//是否执行下一次自动升级技能
	bool							m_bDoAutoUpgradeSkill;
	bool							m_bNpcTask;

	int								m_iCurrentUpgradeSkillID;
	int								m_iCurrentUpgradeBuddyID;

	int								m_iGroupId;			//战区ID
	int								m_iSkyArenaPoint;	//苍穹战场荣誉值
	int								m_iSkyArenaHonorPoint; //苍穹战场本赛季已获得荣誉值(不会减少)
	int								m_iLastSkyArenaPoint;//打开界面时缓存的上一次荣誉值

	int								m_iSkyArenaSeason;	//苍穹战场当前赛季
	uint32_t						m_iSkyArenaCloseTime;//苍穹战场当前赛季结算时间
	uint32_t						m_iSkyArenaNxtTime;	//苍穹战场下赛季开始时间

	//每日任务杀怪计数 2014/4/11林培华
	mem::map<int,int>				m_mapDaylyTaskKillCount;

	/*int								m_lLastLogicAreaID;
	int								m_lLastMapID;
	int									m_iAreaLogicType;*/

	//帮会召集令cd
	int64_t						m_lGuildTransportStamp;

	//新手帮助相关
	int							m_iTutorialProgress;
	std::vector<uint32_t>       m_TutorialRewardId; 

	//苍穹段位奖励相关
	int                        m_iSkyArenaRankRewardId ; 
	int                        m_iMaxScore ; 

	//篝火晚会
	int                        m_leftTimes ; //剩余购买次数 
	int                        m_buyTimes ; //购买次数 
	int                        m_Rate;  //倍率 
	std::vector<int32_t>       m_BuyerInfo ; //购买玩家信息  
	StringType                 m_BuyerName; //购买玩家名字


	//巅峰竞技场 
	int                        m_iStartTime ;//巅峰竞技场开赛时间 
	int                        m_iEndTime ; //巅峰竞技场结束时间
	int                        m_stageType ; //阶段枚举
	int                        m_showTime ; //展示时间
	int                        m_battleStTime;//战斗开始时间
	int                        m_myRank ;  //本人当前赛季苍穹排名
	int                        m_zoneId ;   //赛区Id 
	int                        m_stageSetEndTime ; //阵容设置时间
	int                        m_stageStTime ; //阶段开始时间 
	int                        m_stageEndTime ; //阶段结束时间
	int                        m_nTotalGroupId; //总组数 
	bool                       m_summitRedPoint;   //巅峰红点   
	int                        m_myTopType;    //我打到的最高阶段  
	int                        m_showRemainTime ; //剩余展示时间 
	int                        m_battleRemainTime ; //剩余战斗时间 
	int                        m_stageSetRemainTime ; //剩余设置阵容时间 
	int                        m_stageEndRemainTime ; //阶段结束剩余时间 
	int                        m_invitedType; //邀请函广告的类型 
	bool                       m_isValid; //邀请函是否发送 

	typedef mem::map<int,std::vector<SummitArenaGroupStageUserData>>     m_groupDatas   ;  //组战况数组
	typedef mem::map<int,std::vector<SummitArenaAgainstList>>  againstDatas ;  
	againstDatas                              againstList;  
	m_groupDatas                              allGroupData ; 
	/*SummitArenaGroupStageData                 myGroupData ;*/  //我自己的组 数组
	std::vector<SummitArenaGroupStageUserData>  myGroupData ; 
	SummitArenaGroupStageData                 EntryList ; //参赛列表 
	std::vector<SummitArenaHonorDataMsg>	  TopPlayerList;//荣耀之巅
	int										  TopErr;//荣耀之巅错误码
	
	//异种降临公会排行榜
	std::vector<PS_GuildDamageInfo>			m_nGuildInfo;

	//目标
	object_id_type				m_lPVPTargetID;
	object_id_type				m_lLastLockTargetID;
	int							m_lLastLockTargetNeedGfx;
	object_id_type				m_lWatchTargetID;
	object_id_type				m_lLastWatchTargetID;
	object_id_type				m_lMouseOverActorID;
	object_id_type				m_lMouseOverInteractiveBtnActorID;
	object_id_type				m_lLastMouseOverInteractiveBtnActorID;
	bool						m_bDropItemTipsShowing;
	object_id_type				m_lLastMouseOverActorID;
	bool						m_bWatchTargetFactionChange;
	bool						m_bMouseOverTargetArgValid;	//是否要使用m_kMouseOverTargetArg，比如鼠标移到传送门、特效墙身上不需要高亮
	MouseOverTargetArg			m_kMouseOverTargetArg;		//MouseOverTarget时要用到的参数，比如轮廓颜色，高亮颜色等，由外部设置

	Data::GD_ModelBase*			m_pPreLockedTargetModel;	// 选中的目前切换角色的时候，id不变，但是visibleData会变换
	object_id_type				m_lPreLockedTargetID;
	bool						m_bIsFreezeLock;

	bool						m_bEnablePathFinding;

	bool						m_bNeedLockTarget;
	bool						m_bNeedLockLocation;
	bool						m_bNeedLockTradeTarget;
	int							m_iLockLocationFXID;
	int							m_iLockLocationFxType;
	NiPoint3					m_kMouseOverLocation;

	LC_IDSet					m_kLastAttackerID;			//最近攻击我的敌人
	float						m_fAutoSearchEnemyCDTime;	//自动搜索周围敌人的cd时间
	float						m_fLastSceneMouseOverTime;
	float						m_fLastPickTerrainTime;
	float						m_fLastPrepareMoveStrideTime;
	bool						m_bLastSceneMouseOver;		// 配合 m_fLastSceneMouseOverTime
	bool						m_bMoveStride;		//鼠标引导玩家移动
	bool						m_bWASDStride;		// wasd引导玩家移动

	bool						m_bPathtoTreasure;		//正在寻路到宝箱标志位

	Utility::UT_Vec3Int			m_kMoveLineTargetLocation;

	//生产
	LC_ClientProduction*		m_pkClientProduction;

	//藏宝图坐标
	struct TreasurePosInfo
	{
		TreasurePosInfo()
		{
			iMapResID = 0;
			iX = 0;
			iY = 0;
			iZ = 0;
		}

		void Update(int mapID, int x, int y, int z=0)
		{
			iMapResID = mapID;
			iX = x;
			iY = y;
			iZ = z;
		}

		bool Test(int mapID, int x, int y)
		{
			return iMapResID == mapID && x == iX && y == iY;
		}

		int				iMapResID;
		int				iX;
		int				iY;
		int				iZ;
	};

	TreasurePosInfo				m_kLastLocation;

	TreasurePosInfo				m_kTreasurePos;

	//组队邀请数据
	struct LC_PlayerGroupInviteEntry
	{
		int				m_lInvitePlayerGroupID;
		unique_id_type	m_lInviteCharID;
		bool			m_GroupRaidVaild;
		bool			m_Combin;
		int				m_GroupRaidResId;
	};
	typedef mem::vector<LC_PlayerGroupInviteEntry>			LC_PlayerGroupInviteEntryVector;
	typedef LC_PlayerGroupInviteEntryVector::iterator		LC_PlayerGroupInviteEntryVectorIter;

	LC_PlayerGroupInviteEntryVector							m_kInvitePlayerGroupEntryVector;	// 邀请组队列表

	typedef mem::vector<int>								LC_PlayerGroupBlockPlayerVector;
	LC_PlayerGroupBlockPlayerVector							m_vecBlockPlayerID;


	//反隐身功能相关参数
	struct LC_RevealInvisibilityParams
	{
		//启动参数
		UT_Vec3Int		m_kCircleCenter;	//反隐区域中心
		int				m_iCircleRadius;	//反隐区域半径
		bool			m_bRevealOnce;		//只反隐一次

		//保存数据
		int				m_lLastRevealTime;	//上次反隐时间
		bool			m_bIsFirstReveal;	//是否第一次反隐
		LC_ActorPtrList	m_kLastRevealTargetList;

		void ReUse()
		{
			m_kCircleCenter.x = -1;
			m_kCircleCenter.y = -1;
			m_kCircleCenter.z = -1;
			m_iCircleRadius = -1;
			m_bRevealOnce = true;
			m_lLastRevealTime = -1;
			m_bIsFirstReveal = true;
			ClearRevealTargetList();
		}

		void ClearRevealTargetList()
		{
			//重置目标身上的反隐标志
			LC_ActorPtrListIter iter;
			TRAV(m_kLastRevealTargetList, iter)
			{
				LC_ActorBase* pkTarget = *iter;
				if (NULL != pkTarget)
				{
					pkTarget->SetRevealInvisibility(false);
				}
			}
			m_kLastRevealTargetList.clear();
		}
	};
	LC_RevealInvisibilityParams		m_kRevealInvisibilityParams;
	
	struct SingleCombatRecord
	{
		int32_t m_iRank;
		int64_t m_iDamageValue;
		int32_t m_iGameStoryID;
		uint64_t m_iParam;
	};

	struct RaidBossDropInfo
	{
		RaidBossDropInfo()
		{
			ReUse();
		}
		
		void ReUse()
		{
			m_nDropTimes = 0;
			m_nNextRevertTime = 0;
			m_nEndRevertTime = 0;
		}

		int32_t                 m_nDropTimes;
		uint32_t                m_nNextRevertTime;
		uint32_t                m_nEndRevertTime;
	};

	unique_id_type				m_lChatTargetID;
	StringType					m_strCharTargetName;
	int							m_iCurrentChatChannel;

	PrivateChatSaveSt			m_stPrivateChatSaveSt;

	bool						m_bKeyTabPressed;				//记录tab键是否已经按下
	object_id_type				m_lSaveLockedTargetID;			//按下tab键时暂存锁定的目标
	object_id_type				m_lSaveWatchTargetID;			//按下tab键时暂存锁定的目标

	LC_PlayerFlopManager*		m_pkFlopManager;
	StringType					m_sItemTipMessage;

	bool						m_bTeamHint;

	//LC_GameEventHandler			m_kGameEventHandler;

	LC_ClientSocietyAsset			m_kClientSocietyAsset;
	LC_ClientFriendMsgBroad			m_kClientFriendMsgBroad;
	LC_ClientSearchPlayer			m_kClientSearchPlayer;
	LC_ClientViewPlayerInfo			m_kClientViewPlayerInfo;
	LC_ClientFriendNick				m_kClientFriendNick;

	LC_ClientWorldLevelAsset		m_kClientWorldLevelAsset;

	LC_ClientAlliance				m_kClientAlliance;
	
	bool							m_bRequestLogicTrigger;
	//int							m_iLastTriggerAreaID;
	float							m_fLastLogicTriggerTime;

	//触发器特效逻辑
	int								m_bStartTriggerType;
	TickCount						m_bTriggerTypeTick;

	//等待挂起的技能
	bool							m_bExecuteSkill;
	Skill::SK_SkillExecute*			m_pkWaitPendingSkill;
	int								m_lWaitPendingSkillItemType;

	User_Pk_Show_Info				m_nMyInfo;
	User_Pk_Show_Info				m_nOtherInfo;

	mem::map<int,bool>              m_mapActivate;
	mem::map<int,mem::vector<int>>	m_vecRunelevel;
	mem::map<int,mem::vector<int>>	m_vecEquiplevel;
	mem::map<int,mem::vector<int>>	m_vBuddyEquipItemIds;

	std::vector<int32_t>			m_vecTopicID;

	PS_SettleInfo					m_vRaidBossSpeciesMyRank;
	mem::map<int,int64_t>			m_vThemeACTRaidBossDamage;//storyid , damage
	mem::vector<PS_SettleInfo>		m_vRaidBossSpeciesRankThree;
	mem::vector<PS_SettleInfo>		m_vRaidBossSpeciesRankList;
	//已请求执行、服务器未返回的技能，客户端锁
	int								m_iReqNoRltExecSkillID;	//requested and not responsed skill
	float							m_fReqExecSkillTime;	//time stamp

	//寻路相关
	UT_Vec3Int						m_kSearchPathTargetPos;
	bool							m_bArriveFlag;			//! 是否到达
	float							m_fSearchPathMiniMapX;	// 寻路目标点X;
	float							m_fSearchPathMiniMapY;	// 寻路目标点Y;

	//寻路相关
	//bool							m_bAutoFindPathFlag;	//自动寻路标志
	Path::PF_Path					m_kMovePath;
	float							m_fLastCheckPendingTalkNPCTime;
	PathPendingActionArg			m_kPPAArg;				//寻路目标行为
	bool							m_bIsPathFinding;		//是否正在寻路

	PathPendingActionArg			m_kPPAArgForFeiXie;		//飞鞋的寻路
	object_id_type					m_kPPAArgRetryTalkID;	//飞鞋对话失败，不管尝试对话ID

	PathPendingActionArg			m_kJumpPPAA;		//飞鞋的寻路

	int								m_pendingOject;			//目标obc

	//寻路相关
	LC_AcrossMapPath				m_kAcrossMapPath;
	int								m_iPendingPathMapResID;	//寻路到其它地图
	Utility::UT_Vec3Int				m_kPendingPathPos;		//寻路到其它地图的目标点
	int								m_kPendingVal;
	int								m_iPendingPathMapLogicID;//寻路到其他地图的LogicID
	TickCount						m_kDoubleTerrienClearPath;

	int								m_lMoveStraightParam;
	bool							m_lpickMoveFlag;
	//一直跑到某个点的标记
	bool							m_bMoveStraightFlag;

	//新手引导开始标记
	bool							m_bIsCanStartTutorial;
	//资源找回是否开启
	bool							m_bResourceRetrieve;

	float							m_fLastUpdateTime;
	float							m_fLastUpdateTradeStateTime;
	float							m_fLastUpdateProcessingTask;

	//等待设置快捷键
	bool							m_bWaitingShortcutKey;
	bool							m_bIsSaveShortcutKey;

	//自动化脚本
	Utility::LS_LuaVoidFunction*	m_pkScriptControllerFunctor;

	float							m_fCurrentHookSecond;
	// 游戏币交易系统
	int								m_lCommissionBuyYuanBaoNum;			//	玩家委托收购元宝数量
	int								m_lCommissionSellYuanBaoNum;		//	玩家委托出售元宝数量
	int								m_lQuickBuyYuanBaoNum;				//	玩家快速收购元宝数量
	int								m_lQuickSellYuanBaoNum;				//	玩家快速出售元宝数量

	int								m_lCommissionBuyYuanBaoPrice;		//	玩家委托收购元宝价格
	int								m_lCommissionSellYuanBaoPrice;		//	玩家委托出售元宝价格
	int								m_lQuickBuyYuanBaoPrice;			//	玩家快速收购元宝价格
	int								m_lQuickSellYuanBaoPrice;			//	玩家快速出售元宝价格

	int								m_lHookForbidCount;

	bool							m_bHooking;
	int								m_lHookTime;
	UT_Vec3Int						m_kHookLocation;
	float							m_fAssistCDTime;			//自动检测挂机系统的时间
	UT_Vec3Int						m_kHookGuideLocation;
	bool							m_bStopHookOnTargetDead;	// 特定目标怪死亡或不能攻击后，结束挂机
	object_id_type					m_iHookingTargetID;			// 挂机时针对的特定目标怪
	object_id_type					m_kSpecialHookCharType;		// 挂机时针对的特定目标类型

	float							m_bHookGodUsageTime;

	//副本挂机
	LC_ClientRaidHookAsset			m_kRaidHookAsset;
	bool							m_iRaidHookFlag;

	std::map<object_id_type,object_id_type>	m_mapInvalidPickItem;
	Utility::TickCount						m_mapInvalidPickItemTick;

	bool							m_bAutoPicking;

	//最近攻击者的自动更新时间
	float							m_fLastAttackerTime;

	float							m_fTranscriptionCDTimer1;	//切换场景以后播放出现特效倒计时
	float							m_fTranscriptionCDTimer2;	//切换场景以后播放出现人物倒计时
	bool							m_bTranscriptionFromReturnToCity;	//传送来自‘回城’技能

	float							m_fLastReqFriendStatusTime;

	int								m_lResId;   //帮会召集令传送目标地图id

	void                _UpdateReqFriendStatus(float fCurrentTime);

	//好友召唤设置列表
	unique_id_type		m_summorLis[MAX_SUNMOR_FIREND_POOL_CAP];
	uint32_t			m_summorFriendCharID;

	bool				m_bForbbidenKey;							//当前是否为禁止按键状态

	bool				m_bAcceptTaskLogin;

	LC_TargetMap					m_tabLockedMap;

protected:
	object_id_type			m_lSelectedNPCIDInCareerLearn;
	void					_setSelectedNPCIDInCareerLearn(object_id_type NPCID){m_lSelectedNPCIDInCareerLearn = NPCID;}
	object_id_type			_getSelectedNPCIDInCareerLearn(){return m_lSelectedNPCIDInCareerLearn;}
public:
	void					NotifyServerAlreadyInGame();
	void					SetNotifyInGameFlag(int id) {m_NotifyInGame = id;};
	int						GetNotifyInGameFlag() {return m_NotifyInGame; };
	void					ReqMGPTInGame();
public:
	void					ReqPing();
	void					RltPing(uint16_t usFlag, uint8_t token,uint32_t uiServerTime);
	int						GetCurrentPing(uint16_t usFlag);
	int						GetCurrentFPS();
	StringType				GetServerTime();
	int64_t					GetServerTimeVal();
	//< online time
	void					SetEnterGameTime(void) { m_enterGameTimeStamp = ::timeGetTime(); }
	unsigned int			GetOnlineTime(void) { return (::timeGetTime() - m_enterGameTimeStamp) / 1000; }

	virtual ResultType		SummonFriendAvatar(object_id_type FriendCharID);
	virtual bool CheckAcceptTaskCondition(int taskSystem, int taskID);

	ResultType				RequestGetGiftDaily(int iLevel);
	ResultType				RequestGetGfitOneOff(int iLevel);

	//今日请求签到
	ResultType				RequestSignInToday(int iType);
	//请求补签
	ResultType				RequestReSignIn(int iType, int iDay);
	//请求领取签到奖励
	ResultType				RequestGetSignInReward(int iType, int iID);
	//获取gamestoryid
	int						GetGameStoryIdByMapResID(int iMapResID);
	//将任务加入可以开始列表
	void					AddToCanStartList(int iTaskID);

	LC_PlayerDetailManager* GetPlayerDetailMgrPtr(){return &m_kPlayerDetailInfoData;}

	void					SetPowerGuildID(int iID){m_iPowerGuildId = iID;}
	int						GetPowerGuildID(void){return m_iPowerGuildId;}
	void					SetGuildWarType(int iType);
	int						GetGuildWarType(void){return m_iGuildWarType;}
	void					SetGuildWarTargetPos(int iPosX, int iPosY);
	void					SetPowerPlayer(unique_id_type iUserID);
	unique_id_type			GetPowerPlayer()const;
	void					_updateDirectionFx();
	void					_updateGameStoryDirectionFx();
	int						GetCanDrawRewardsNumberOfGuildWar(int iGuildWarType);

	bool					IsSafeArea();

	bool					GetIsFirstEnterGame() {return m_bFirstEnterGame;}
	void					SetIsFirstEnterGame(bool b){m_bFirstEnterGame = b;}

	bool					GetGuildRewardIsValid() { return m_bGuildRewardIsVaild; }
	void					SetGuildRewardIsValid(bool b) { m_bGuildRewardIsVaild = b; }

	virtual void			SetGuildName(const StringType& strGuildName);

	//活动奖励物品入口
	LC_CommonStatRewardDataEntry* GetStatRewardDataEntry(int iType, int iKey){return LC_CommonStatManager::GetSingletonPtr()->GetCmnRewardDataEntry(iType,iKey);}

	//绝学
	LC_VIPFDepositEntry*	GetVIPFDepositDataEntry(int32_t level){return LC_VIPCmnManager::GetSingletonPtr()->GetVIPFDepositDataEntry(level);}
	bool					IsDailyRechargeRewardedToday();
	bool					IsRechargeToday();
	int						GetDailyRechargeMaxNotDrawedLevel();
	LC_ClientOpenAbilityManager* GetOpenAbilityManager();
	
	//宠物
	object_id_type			GetEyeCharTypeID() {return m_iEyeNPCID;}
	//组队副本接口
	void					SetCrossRealMapTeamID(int iID){m_iCrossRealMapTeamID = iID;}
	void                    SetCrossRealTeamMapID(int MapID){m_iCrossRealTeamMapID = MapID;}
	int                     GetCrossRealTeamMapID(){return m_iCrossRealTeamMapID;}
	bool					IsInCrossRealMapTeam(){return m_iCrossRealMapTeamID > 0;}

	//体力值
	virtual void			SetVigor(int Value) { m_Vigor = Value; }  
	virtual void			SetAlreadyTransfer(bool kFlag)	{ m_bAlreadyReqTransfer = kFlag;  m_kReqTransferInterval.ResetTimer();}
	bool					GetAlreadyTransfer() { return m_bAlreadyReqTransfer; }

	virtual void			GetSubEquipSlotMoldingSuitPercentAttr(int subType, LC_ActorAttributeMap* attrMap);
	virtual void			GetSubEquipSlotMoldingAttr(int subType, LC_ActorAttributeMap* attrMap);
	virtual void			GetSubEquipSlotMoldingSuitValueAttr(int subType, LC_ActorAttributeMap* attrMap);

	int						GetSubEquipSuitMoldingAttribValue(int iSubType, int iAttribType);
	int						GetSubEquipSuitMoldingAttribPercent(int iSubType, int iAttribType);
	int						GetSubEquipMoldingAttribValue(int iSubType, int iAttribType);

	//装备操作
	virtual ResultType		ReqItemInPackageOperation(int iPackType, int iEntryIndex, int OperationCode, const StringType& sParams);

	void					StartNormalContinueCheck();
	void					EndNormalContinueCheck();
	bool					IsNormalContinueCheck() {	return m_normalContinueCheck;}

	GameOptCache&			GetGameOptCache()	{	return m_kGameOptCache;}

	int					GetSuitableDrugItemID();

	void				UpdateRankInfoData(PS_User_Rank_Info &userRankInfo);
	StringType			GetRankInfoDataString(int32_t lRankType);

	void				SetImmediateTaskStart(bool bFlag){m_bImmediateTaskStart = bFlag;}

	virtual bool		GetIsInitAttribute(void) {return m_bInitAttribute;}
	virtual void		SetInitAttribute(bool b) {m_bInitAttribute = b;}

	platform_info*		GetPlatformInfo(const StringType& sPlatformType);
	void				AddPlatformInfo(const StringType& sPlatType, uint8_t platInfo);

	int					GetPlatformVipLevel(const StringType& sPlatformType);
	bool				IsPlatformYearVip(const StringType& sPlatformType);
	bool				IsPlatformVip(const StringType& sPlatformType);
	bool				IsPlatformHighVip(const StringType& sPlatformType);
	int					GetPlatformFormatVip(const StringType& sPlatformType);

	//dailycleandata相关
	void				GetDailyCleanDataFormServer(uint16_t RespType, const std::map<uint32_t, int32_t> &data);
	int32_t				GetDailyCleanValue(uint32_t nType, uint32_t nID);

	uint32_t			GetServerOpenTime(){return (uint32_t)GET_SERVER_OPEN_TIME;}

	uint32_t			GetServerConflateTime(){return (uint32_t)GET_SERVER_CONFLAT_TIME;}

	void				SetLastOffLineTime(uint32_t timeStamp){m_uiLastOffLineTime = timeStamp;}
	uint32_t			GetastOffLineTime(){return m_uiLastOffLineTime;}

	void				SetCimeliaAutoPicking(bool bAutoPicking) { m_bCimeliaAutoPicking = bAutoPicking; }
	bool				IsCimeliaAutoPicking() { return m_bCimeliaAutoPicking; }

	void				SetAutoIntelligentTask(bool bAuto){m_bIntelligentTask = bAuto;}
	bool				GetAutoIntelligentTask(){return m_bIntelligentTask;}

	void				SetLotteryConsumeUnbindyuanbao(int iYuanbao) { m_iLotteryConsumeUnbindyuanbao = iYuanbao; }
	int					GetLotteryConsumeUnbindyuanbao() { return m_iLotteryConsumeUnbindyuanbao; }

	void				ReqLotteryDraw(int iId, int iKey);
	void				ReqLotteryExtraReward(int iId, int iParam);

	void				ReqChargeProfitReward(int iId, int iParam);

	//玩家第一次进入游戏，由于部分函数需要玩家属性，所以在进入游戏后第一次SyncPlayerProperty后进行
	void				PostPlayerEnterGame();
	void				SetPostEnterGameFlag(bool bFlag){m_bPostEnterGameFlag = bFlag;}
	bool				GetPostEnterGameFlag(){return m_bPostEnterGameFlag;}

	void				SetRuneBuddySlotOpen(bool bOpen) { m_bRuneBuddySlotOpen = bOpen; }
	bool				GetRuneBuddySlotOpen() { return m_bRuneBuddySlotOpen; }
	void				SetRuneSubSlotOpen(bool bOpen) { m_bRuneSubSlotOpen = bOpen; }
	bool				GetRuneSubSlotOpen() { return m_bRuneSubSlotOpen; }
	void				SyncRuneRecordInfo(const MG_SyncRuneRecord& msg);
	void				SetRuneRecord(int iID, bool bDrawed);
	bool				GetRuneRecord(int iID);
	bool				GetRuneRewardDrawed(int iID);
	bool				GetRuneRewardPrompt();
	int					GetRuneRecordCount() {return m_kRuneRecordMap.size();}
	bool				UpdateRuneBuddySlotDressState(int iCid, int iSlot, bool bOpen);
	bool				GetRuneBuddySlotDressState(int iCid, int iIndex);

	LC_BackPackEntry*	GetBuddyEquipEntry(int iCid, int iIndex);
	void				OnBuddyEquipUpdated();

	void				SetSevenDaysActivityEndTime(int iTime) { m_iSevenDaysEndTime1 = iTime; }
	int					GetSevenDaysActivityEndTime() { return m_iSevenDaysEndTime1; }
	void				SetSevenDaysRewardEndTime(int iTime) { m_iSevenDaysEndTime2 = iTime; }
	int					GetSevenDaysRewardEndTime() { return m_iSevenDaysEndTime2; }
	void				SetOfferTaskRefreshTimes(int iTime) { m_iOfferTaskRefreshTimes = iTime; }
	int					GetOfferTaskRefreshTimes() { return m_iOfferTaskRefreshTimes; }
	void				SetOfferTaskAcceptTimes(int iTime) { m_iOfferTaskAcceptTimes = iTime; }
	int					GetOfferTaskAcceptTimes() { return m_iOfferTaskAcceptTimes; }
	void				SetFirstDayLoginRewardTime(int iTime) { m_iFirstDayLoginRewardTime = iTime; }
	int					GetFirstDayLoginRewardTime() { return m_iFirstDayLoginRewardTime; }
	void				SetCollectBossJPCount(int icount) { m_iCollectBossJP = icount; }
	int					GetCollectBossJPCount() {return m_iCollectBossJP; }

	void				SetBuddyEquipShowLevel(int iCid, int iLevel);
	int					GetBuddyEquipShowLevel(int iCid);
	void				SetBuddyShowRank(int iCid, int iRank);
	int					GetBuddyShowRank(int iCid);
	void				RequestSetEquipShowLevel(int iCid);

	int					GetCommanderEquipReinforceLevel(int iEquipType) { return m_pkCommander->GetEquipReinforceLevel(iEquipType); }
	int					GetCommanderEquipRefineLevel(int iEquipType) { return m_pkCommander->GetEquipRefineLevel(iEquipType); }
	int					GetCommanderEquipModingSpiritLevel(int iPackType, int iEquipType) { return m_pkCommander->GetEquipModingSpiritLevel(iPackType, iEquipType); }
	void				RequestCommanderModingSpirit(int iEquipType);
	void				RequestSetCommanderEquipSuitLevel(int iLevel);
	void				RequestSubModingSpirit(int iSubType, int iEquipSlot);

	void				RequestBuddyCombatComposition(int cid);
	void				SetBuddyCombatComposition(int cid, combat_score_type iCommanderScore, combat_score_type iChainScore);
	combat_score_type	GetBuddyCombatCommanderScore(int cid);
	combat_score_type	GetBuddyCombatChainScore(int cid);
	combat_score_type	GetBuddyRuneSkillCombatScoreBonus(int Cid, const StringType& skillIDs);
	void				RequestPlayerPKInfo(const StringType& kPlayerID);
	void				SetPlayerPKDetailInfo(const MG_RLT_PlayerPkInfo& playerInfo);
	float				GetCombatPKMyProgress(int id);
	StringType			GetCombatCompareMyInfo();
	StringType			GetCombatCompareOtherInfo();

	void				ReqBuyRareSubDevaEyeTransform(int iShopID);
	ResultType			CheckRareSubState(int iShopID);

	//公会战相关
	void				ReqBestGuildRankInfo(int iType1, int iType2, int iStartIndex, int iEndIndex, const StringType& str);
	void				ReqBestGuildFinishReward();
	void				ReqGuildRankChallenge(uint64_t iBeChallengeCharID);
	void				ReqGuildFightRobScore(int iOpType, int iNum);
	void				ReqGuildFightRobScoreReceive(int iCount);
	void				ReqGuildFightMemberUIDs(guild_id_type iGuildID);
	void				ReqGuildFightPlayerInfo(const StringType& sUids, bool bCheckTime);
	void				UpdateBestGuildGoalRecord(const std::vector<PS_GoalInfo>& m_vinfo);
	int					GetBestGuildGoalRecordProgress(int iID);
	int64_t				GetBestGuildGoalRecordPoint(int iID);
	void				ReqGuildBattleFightRecord();
	void				ReqGuildBattleBeDefightedFlag();
	/**/
	void				UpdateBestGuildMapBossInfo(std::vector<ST_Alliance_Boss_Info>& m_rInfoList);
	void				UpdateBestGuildMapBossInfo(ST_Alliance_Boss_Info& m_rInfo);
	void				UpdateBestGuildTaskInfo(int TaskCount,bool BestGuildRewardFlag);
	
	/**/
	StringType			GetBestGuildMapBossInfo();	
	StringType			BestGuildConvertWorldPosToMapPos(int x,int y);	
	StringType			BestGuildConvertMapPoTosWorldPos(float x,float y);	
	StringType			GetGuildBattleBossState(int iMapID, int iType);
	void				ReqGuildBattleTowerInfo();
	void				ReqGuildBattleDemageRankList(int storyId, uint32_t objId, int iType);
	void				ReqReealtimeCombatRecorder(uint32_t objId);
	void				SetGuildBattleTowerState(const std::vector<ST_Single_Tower_info>& m_TowerVec);
	ST_Single_Tower_info* GetGuildBattleStateByKey(int iType, int iMapid);
	void				SetGuildBattleTowerState(const ST_Single_Tower_info_lite& info);
	StringType			GetGuildBattleTowerState(int MapId);
	StringType			GetGuildBattleFortressState(int MapId);
	StringType			GetGuildBattleRoyalState(int MapId);
	StringType			GetGuildBattleStateInfo(const ST_Single_Tower_info& m_Info);

	void				GuildBattlePathtoAction(bool bAttack, int charType);
	void				PendingPathAttack(int charType, int pos_x, int pos_y, int pos_z);
	StringType			GetBestGuildPlayerLocation();
	int					GetBestGuildFinishedTaskNum();			//这里包含了成就+任务的数量，这是服务器单独同步过来的
	StringType			GetGuildBattleTaskInfo(int iTabIdx,int iSubIdx);
	StringType			GetGuildBattleTaskTabNames();
	bool				IsGuildBattleTaskRewardToken(){return m_BestGuildRewardFlag;};
	bool				IsGuildBattleTaskPrompt(int iTabIdx,int iSubTabIdx);

	bool				IsTasksCanFinish(Utility::UT_SIMDataList& InTasks);
	bool				IsAchievementCanFinish(Utility::UT_SIMDataList& InAchievements);

	void				GetOccupiedTowers(mem::vector<int>& OutTowerIds);					//获得已占领哨塔


	//主题活动
	StringType			GetThemeActivityTasks(int iBuddyThemeTaskType,int iTabIdx);
	bool				IsThemeActivityTasksPrompt(int iBuddyThemeTaskType,int iTabIdx);
	StringType			GetThemeActivityRaidTasksInfos(int iActivityId);

	void				UpdateThemeACTRaidBossRewardInfo(std::map<int32_t,std::map<int32_t,bool>>& SingleDamageRewardRecord);
	void				AddThemeACTRaidBossRewardInfo(int iMapId,int iRwdIdx);
	void				UpdateThemeACTRaidBossFlags(std::vector<int>& iRaidBossFlags);
	bool				IsThemeACTRaidBossPrompt(int iRaidBossFlags);
	bool				IsThemeACTRaidBossRewardToken(int iMapId,int iRwdIdx);
	void				ReqThemeACTRaidBossRewardInfo(int iMapId,int iRwdIdx);
	void				ReqThemeACTAutoExchange(int iThemeSecondaryId);
	void				ReqThemeACTADAutoShow();//策划要求，活动期间第一次登录需要弹出广告界面，这里客户端请求是为了控制通知的时机。
	void				ReqThemeACTRaidBossPrompt(int iThemeMainId);
	//特权等级+所有已占领哨塔 百分比加成
	int					GetTotalVipNewBonusPercentIncrease();	
	//GetVipPriviligeAddTime
	int					GetVipPriviligeAddTimesByType(int iType);	
	//首充奖励系统
	//void					_initFisrstChargeData(void);

	void				ReqRaidBossPublicKillerInfo(int iCharType, int iMapId);

	//主线任务特权双倍领取
	void				ReqTakeTaskExtraReward();
	void				UpdateTaskExtraReward(SyncType InType,SyncOP InOP,std::vector<KeyValue_Base>& InVec);
	void				ClearTaskExtraReward();
	StringType			GetTaskExtraReward();


	//炼魂系统
	LC_ClientSoulRefineAsset* GetClientSoulRefineAsset(){return &m_kSoulRefineAsset;}
	//新版本活动
	LC_ClientActivityAsset* GetClientActivityAsset(){return &m_kActivityAsset;}

	//激活码使用状态
	void				ReqUseActivationCount(int iType);
	void				SetUseActivationCount(int iType, int iCount);
	int					GetUseActivationCount(int iType);
	
	void				SetHolydeedUserCount(int iType, int iCount);
	int                 GetHolydeedUserCount(int iType);
	void				SetPlayerCommanderName(const StringType& rkCharName);

	void				ReSetInvestmentType();
	void				SetInvestmentType(int iType);
	bool				GetInvestmentTypeHasBuy(int iType);
	void				ReSetInvestmentFreeReward();
	void				SetInvestmentFreeReward(int iID);
	bool				GetInvestmentFreeRewardReceive(int iID);
	void				SetInvestmentHighReward(int iID);
	bool				GetInvestmentHighRewardReceive(int iID);
	void				RequestBuyInvesmentCard(int iType);
	void				RequestAddInvesmentCardReward(int iID);
	void				RequestRefreshRewardTask();

	//灵魂连锁
	void				SyncBuddyChain(const MG_SC_BuddyChainSync& msg);
	void				UpdateBuddyChain(int iIndex, int iCid);
	void				ReqAddBuddyChain(int iIndex, int iCid);
	void				ReqDelBuddyChain(int iIndex);
	bool				IsBuddyInChain(int iCid);
	int					GetBuddyChainCid(int iIndex) { return m_vBuddyChainVec[iIndex]; }
	void				SetBuddyChainCid(int iIndex, int iCid) { m_vBuddyChainVec[iIndex] = iCid; }
	void				SetChainFlag(uint32_t iFlag) { m_iChainFlag = iFlag; }
	bool				IsChainFlag(int iIndex) { return (m_iChainFlag & (1<<iIndex)) > 0; }
	void				ReqSetChainFlag(int iIndex);

	void				SetRegularBossGroupID(int32_t groupid){ m_iRegularBossGroupID = groupid; };
	int					GetRegularBossGroupID(){ return m_iRegularBossGroupID; };
	void				SetRegularBossGroupCDTime(uint32_t itime){ m_iRegularBossGroupCDTime = itime; };
	uint32_t			GetRegularBossGroupCDTime(){ return m_iRegularBossGroupCDTime; };

	void				SetBossKillerInfo(MG_SyncBossKillerInfo info) {m_mBossKillerInfo[info.m_nKey] = STDSTR2TPSTR(info.m_nKillerName);};
	StringType			GetBossKillerInfo(int32_t iMapID, int32_t iCharTypeID);

	//核芯
	void				SyncActiveStarVeinIds(const MG_SyncActivateStarVein& msg);
	void				SyncActiveStarVeinGoalIds(const MG_SyncStarVeinGoalRecord& msg);
	StringType			GetActiveStarVeinIds();
	StringType			GetActiveStarVeinGoalIds();
	attr_value_type		GetStarVeinScore();  
	void				RequestActiveStarVein(int iID);
	void				RequestGetStarVeinGoalReward(int iID);

	//每日活跃度相关
	void                RltSyncGamePlayerDailyActivity(const MG_SyncDailyActivity& rltMsg);
	int32_t				GetDailyActivity(){ return m_nDailyActivity; };
	StringType			GetDailyActivityString();
	void				RequestGetDailyActivity(int32_t rewardID); // 申请领取阅历奖励
		
	//角色成长礼包相关
	void				RltSyncCharacterUpgradGiftData(const MG_SyncBuddyGrowthPackage& rltMsg);
	bool				IsBuddyActiveUpgradGift(int cID);
	bool				IsBuddyUpgradGiftOver(int cID);
	int32_t			GetBuddyUpgradGiftCurrentDay(int cID);
	int32_t			GetBuddyUpgradGiftDay(int cID);
	bool             ReqGetBuddyUpgradGift(int cID);
	bool				ReqActiveBuddyUpgradGift(int cID);

	//首充奖励七日相关
	void                RltSyncGamePlayerDailyCharge(const MG_SyncFDDay& rltMsg);
	void                RltSyncGamePlayerDailyChargeTime(const MG_RLT_RewardTime& rltMsg);
	void				RltSyncGamePlayerDailyChargeRecord(const MG_SyncFDRewardRecord& rltMsg);
	int					GetDailyChargeDay() { return m_dayFlag;}
	int					GetDailyChargeDayLeftTime() { return m_dayTime;}
	bool				IsGetDailyChargeNormalReward();
	bool				IsGetAllDailyChargeReward();
	int					GetDailyChargeShopBuyCountBuyItemID(int itemID);
	void				ReqDailyChargeTime();
	void				ReqDailyChargeFreeReward(int day, int iID);
	void				ReqDailyChargeBuyReward(int day,int iID);

	//首充
	void				ReqTakeFirstChargeReward(int iDay);
	void				UpdateFirstChargeTime(int iChargeTime);
	void				UpdateFirstChargeRewardRecord(const std::map<int,bool>& Record);
	void				UpdateFirstChargeRewardRecord(int iTokenRecord);
	int					GetFirstChargeTime();
	bool				IsFirstChargeRewardToken(int iDay);

	//寻宝累计
	StringType			GetDrawCumulateReward();
	void				UpdateDrawCardCumulateReward(const StringType& InRewardStr);
	//vip快速挂机
	void				ReqVipQuickHook();
	void				UpdateVipQuickHookTime(int iTimes);
	void				UpdateVipQuickHookReward(std::vector<PS_ItemRecord>& RwdInfo);
	StringType			GetVipQuickHookReward();
	int					GetVipQuickHookTime();

	//新成就系统相关
	void			SetNewAchievementStarNum(int inum) {m_iNewAchievementStarNum = inum;};
	int				GetNewAchievementStarNum() {return m_iNewAchievementStarNum;};
	void			SetNewAchievementRewardCord(const std::vector<int32_t>& m_nID);
	StringType		GetNewAchievementRewardCord();
	void			ReqNewAchievementReward(int id);

	//公会礼物相关
	void			ReqSetGuildProfitHideName(bool bShow);
	void			ReqGetGuildProfitReward(uint16_t rewardID);
	void			ReqGetGuildProfitRewardOnKeyReceive();
	void			SetGuildProfitShowName(bool bShow) {m_bGuildProfitShowName = bShow;};
	bool			GetGuildProfitShowName() {return m_bGuildProfitShowName;};
	void			SetGuildProfitLevel(uint32_t ilevel) {m_iGuildProfitLevel = ilevel;};
	uint32_t		GetGuildProfitLevel() {return m_iGuildProfitLevel;};
	void			SetGuildProfitLevelExp(uint32_t iExp) {m_iGuildProfitLevelExp = iExp;};
	uint32_t		GetGuildProfitLevelExp() {return m_iGuildProfitLevelExp;};
	void			SetGuildProfitCrystalLevel(uint32_t ilevel) {m_iGuildProfitCrystalLevel = ilevel;};
	uint32_t		GetGuildProfitCrystalLevel() {return m_iGuildProfitCrystalLevel;};
	void			SetGuildProfitCrystalLevelExp(uint32_t iExp) {m_iGuildProfitCrystalLevelExp = iExp;};
	uint32_t		GetGuildProfitCrystalLevelExp() {return m_iGuildProfitCrystalLevelExp;};
	void			SyncGuildProfitCrystalInfo(const std::map<uint32_t,PS_GuildCrystalInfo>& Info);
	void			ReqGetGuildCrystalReward();
	int				GetGuildProfitCrystalNumber(){return m_vGuildProfitCrystal.size();};
	uint32_t		GetGuildProfitCrystalMinInvaildTime();

	//公会boss相关
	void			SyncGuildBossActivityInfo(const std::map<int32_t, PS_GuildBossInfo>& m_mInfo);
	void			SyncGuildBossActivityFinishMap(const std::set<int32_t>& m_sInfo);
	void			UpdateGuildBossActivityInfo(int iID, PS_GuildBossInfo state);
	void			UpdateGuildBossActivityFinishMap(int32_t m_nFinishMapID);
	StringType		GetGuildBossActivityState(int MapID);
	bool			GetGuildBossActivityPreMapFinish(int MapID);
	int				GetGuildBossActivityOpen();
	void			ReqGuildBossActivityOpen(int iID);
	void			ReSetGuildBossActivityRankList();
	void			SetGuildBossActivityMyRank(const PS_GuildMapBossPlayerInfo& PlayerInfo);
	void			SetGuildBossActivityRank(const std::vector<PS_GuildMapBossPlayerInfo>& PlayerInfo, bool rankThree);
	StringType		GetGuildBossActivityRank(int index, bool rankThree);
	StringType		GetGuildBossActivityMyRank();
	void			ReqGuildBossActivityRank(int MapID, int MinIndex, int MaxIndex);
	void			ReqGuildBossActivityQuit();

	//竞技榜相关
	int				GetTotalReforeLevel();
	void				SyncACTSubInfoEnd(const MG_SyncPlayerACTSubInfo& syncMsg);
	int64_t			GetACTSubInfoByRankType(int32_t rankType);

	//
	void				SyncGamePlayerStarMoney(const MG_SyncPlayerPentacle& rltMsg);

	//传记奖励相关
	void				ReqBiographyReward(int32_t chapterID);
	void				RltBiographyReward(const MG_RLT_BiographyPartReward& rltMsg);
	void				SyncBiographyChapters(const MG_SyncBiographyPartReward& syncMsg);
	void				SyncBiography(const MG_SyncBiography& syncMsg);
	bool				IsChapterOver(int32_t chapterID);

	//限时礼包相关
	void				SyncTimeLimitShop(const MG_SyncPlayerTimeLimitedActivity& syncMsg);
	void				RltTimeLimitShop(const MG_SyncTimeLimitedActivityState& rltMsg);
	StringType	GetActiveTimeShopInfo();

	//收藏相关
	void				RltSyncGamePlayerCollection(const MG_SyncCollectionInfo& rltMsg);
	void				ReqUpgradCollection(int iID);
	void				ReqUpgradCollectionSuit(int iID);
	void				RltUpgradCollection(const MG_RLT_CollectionActivation& rltMsg);
	void				RltUpgradCollectionSuit(const MG_RLT_CollectionReward& rltMsg);

	//挂机机器人相关
	void				RltSyncBotData(const MG_SC_BotDataSync& rltMsg);
	void				RltSyncBotProfitData(const MG_SC_BotBonusCacheSync& rltMsg);
	void				RltSyncVipNewProfitData(const MG_SC_NewVIPBonusCacheSync& rltMsg);
	void				RltSyncBotBeginTime(const MG_SC_BotDataSync_BeginTime& rltMsg);
	void				ReqActiveBot(int cID);
	void				ReqUpgradBot(int cID);
	void				RltActiveBot(const MG_SC_BotActivateResult& rltMsg);
	void				RltUpgradBot(const MG_SC_BotUpgradeResult& rltMsg);
	void				ReqGetBotProfit();
	void				ReqGetVipNewBonus();
	void				RltBotProfit(const MG_SC_BotRewardResult& rltMsg);


	//指挥官头衔
	void				SetCommanderTitleLevel(int iLevel);
	int					GetCommanderTitleLevel();
	bool				GetCommanderTitleOpen(); 
	void				CheckCommanderTitleOpen(); 
	StringType			GetCommanderTitleCurInfo();

	bool				GetProcessPathFlag() { return m_processPathFlag; }
	void				SetProcessPathFlag(bool bFlag) { m_processPathFlag = bFlag; }

	int					ReqTakeCommanderTitleUpgradeReward();//领取晋升奖励

	//特训
	int					ReqTakeSpecialTrainingReward();//领取晋升奖励
	/**/
	void				UpdateSpecialTrainingLevel(int InLevel);
	/**/
	StringType			GetSpecialTrainingTaskInfo(int iLevel);
	int					GetSpecialTrainingLevel();
	int					GetSpecialTrainingMaxLevel();

	void				GetTaskInfoString(Utility::UT_SIMDataList& InTasks,StringType& OutStr);
	void				GetAchievementInfoString(Utility::UT_SIMDataList& InAchievements,StringType& OutStr);

	/*
	OutStr:未完成
	OutStr2:已领奖
	OutStr3:完成未领奖
	*/
	void				GetTaskInfoStringSortByFinished(Utility::UT_SIMDataList& InTasks,StringType& OutStr,StringType& OutStr2,StringType& OutStr3);
	void				GetAchievementInfoStringSortByFinished(Utility::UT_SIMDataList& InAchievements,StringType& OutStr,StringType& OutStr2,StringType& OutStr3);
	void				RewardMapToRewardString(mem::map<int,int>& RwdMap,StringType& RwdStr);
	// 分解装备
	void				ReqDeCompositionEquips(StringType equipIndex);
	void				RltDeCompositionEquips(const MG_RLT_DecompositionItem &rltMsg );

	//分解装备等阶设置
	void                ReqDecompositionSetting(bool bCommander, int iClevel, int iCgrade, bool bVehicle, int iVlevel, int iVgrade, bool bDevaeye, int iDlevel, int iDgrade, bool bWing, int iWlevel, int iWgrade);
	void				RltDecompositionSetting(const MG_RLT_DecompositionSetting &rltMsg);


	//荣光远征
	void				ReqQuickRaidNew(int iMapId,int iNum);							//扫荡
	void				ReqGetChapterRaidStarReward(int iChapterId,int iStarIndex);		//领取章节奖励
	/**/
	void				UpdateRaidGloryTotalStarNum(int iNum);
	void				UpdateRaidGloryChapterStarNum(int iChapterId,int iNum);
	void				UpdateGloryChapterRewardRecord(int iChapterId,std::vector<int>& InRecord);
	void				UpdateQuickRaidGloryCount(std::map<int,int>& InRecord);//扫荡次数
	void				UpdateRaidGloryPointRecoverTimestamp(int iNextTime,int iEndTime);
	/**/
	int					GetRaidGloryTotalStarNum();
	int					GetRaidGloryChapterStarNum(int iChapterId);
	StringType			GetRaidGloryChapterRewardRecord(int iChapterId);
	int					GetRaidGloryPointRecoverNextTimestamp();
	int					GetRaidGloryPointRecoverEndTimestamp();
	void				ReqReceiveGloryDailyReward();
	void				SetGloryDailRewardDrawed(int iDrawed) { m_iGloryDailRewardDrawed = iDrawed;};
	int					GetGloryDailRewardDrawed() {return m_iGloryDailRewardDrawed;};
	StringType			GetRaidGloryCurDailyReward();
	StringType			GetRaidGloryMyMissionIdx();
	int					GetRaidGloryHighestMap();
	int					GetQuickRaidGloryCount(int iMissionId);//扫荡次数
	bool				HasNotTakeReward(int iBeginChapter,int iEndChapter);

	//遗迹突袭寻宝
	void				ReqGetYijiTuxiStageReward(int iStageId);
	void				ReqGetYijiXunbaoStageReward(int iStageId);
	void				ReqGetToyIslandStageReward(int iStageId);
	/**/
	void				UpdateYijiTuxiReward(int RewardId);
	void				UpdateYijiTuxiReward(std::vector<int>& InRewardRecord);
	void				UpdateYijiXunbaoReward(int RewardId);
	void				UpdateYijiXunbaoReward(std::vector<int>& InRewardRecord);
	void				UpdateToyIslandReward(int RewardId);
	void				UpdateToyIslandReward(std::vector<int>& InRewardRecord);
	/**/
	int					GetYijiTuxiRewardStage();
	int					GetYijiTuxiMaxStage();
	int					GetYijiXunbaoRewardStage();
	int					GetYijiXunbaoMaxStage();
	int					GetToyIslandRewardStage();
	int					GetToyIslandMaxStage();
	int					GetYijiTuxiKillCount();
	int					GetYijiXunbaoKillCount();
	int					GetToyIslandKillCount(int key);


	//开服争霸榜
	void				ReqGetSubValue();


	//幸运转盘
	void				ReqGetRotaryTableReward();
	void				ReqSetRotaryTableAnimation(bool bSkip);
	/**/
	void				UpdateRotaryRewardIdx(int InRewardIdx);
	void				UpdateRotaryTableIsCharged(bool bIsCharged);
	void				UpdateRotaryTableVaildTimes(int InValidTimes);
	void				UpdateRotaryRewardRecord(std::vector<int>& InRewardRecord);
	void				UpdateRotaryEndTime(int InRotaryEndTime);
	void				UpdateRotarySkipAnimSetting(bool bSkip);
	/**/
	bool				GetChargedToday();
	int					GetRotaryRewardIdx();
	StringType			GetRotaryRewardRecord();
	int					GetRotaryRewardValidTimes();
	int					GetRotaryEndTime();
	bool				GetSkipAnimSetting();

	//极品白拿
	//void				ReqBuyNonpareilItem(int id);
	//void				ReqGetNonpareilReturn(int id);
	///**/
	//void				UpdateBuyNonpareilRecord(MG_SyncNonpareilRecord& InRecord);
	//void				UpdateNonpareilReturnRecord(std::vector<int>& InRecord);
	//void				UpdateBuyNonpareilEndTime(int iBuyTime,int iReturnTime);
	///**/
	//int32_t				GetIsNonpareilBought(int id);
	//bool				GetIsNonpareilReturnRewardToken(int id);
	//int					GetNonpareilBuyTimeEnd();
	//int					GetNonpareilReturnTimeEnd();
	void                ReqBuyNoPayItem(int id) ; 
	void                SetBuyNoPayList(const std::vector<KeyValue_Base_Bool> & buyNoPayInfo ); 
	void                UpdateBuyNoPayList(int id ,bool isBuy); 
	void                ReqGetBuyNoPayReturn(int id); 
	void                UpdateBuyNoPayReturn(int id,bool result); 
	StringType          GetBuyNoPayResult() ; 
	
	//Boss首杀相关
	void				RltSyncGamePlayerBossFirstKill(const MG_SyncBossFirstKillInfo& rltMsg);
	StringType			GetBossFirstKillInfo(int32_t charTypeID);
	void				SyncFirstKillBossReward(const MG_SyncKillBossReward& msg);
	bool				IsBossFirstKillRecorded(int charTypeID);
	bool				IsBossKilledBySelf(int charTypeID);	//个人是否已击杀boss
	void				RltSyncServerBossFirstKillRecord(const MG_SyncServerFirstKillBossReward& rltMsg);
	bool				IsServerBossFirstKillRecorded(int32_t charTypeID);
	StringType			GetBossFirstKillRewardPlayerInfo(int32_t charTypeID);
	void				RequestDrawFirstKillReward(int iCharType, bool bServer);
	void				RequestAutoDecompositionItem(bool bauto);

	//设置头像、头像框相关
	void				SetCommanderActiviteFrame(const MG_SyncCommanderTitleRecord& msg);
	int					GetCommanderActiviteFrameCount();
	int					GetNewCommanderActiviteFrameCount();
	bool				IsCommanderFrameNew(int index);
	bool				IsCommanderFrameActivite(int index);
	void				SetCommanderActiviteHead(const MG_SyncProfilePictureRecord& msg);
	int					GetCommanderActiviteHeadCount();
	int					GetNewCommanderActiviteHeadCount();
	bool				IsCommanderHeadNew(int index);
	void				RequestSetCommanderFrameFlag(int index);
	void				RequestSetCommanderHeadFlag(int index);
	void				RequestSetCommanderFrame(int index);
	void				RequestSetCommanderHead(int index);

	void				RequestSetBuddyTransform(int itype, int cID, int TransformID);
	//竞技榜相关
	void				RltSyncGamePlayerCompeteReward(const MG_SyncACTActiveRewardInfo& rltMsg);
	int32_t				GetCompeteRewardCount(int32_t iID);
	//开服争霸榜相关
	void				RltSyncGamePlayerSubReward(const MG_SyncACTActiveRewardInfoB& rltMsg);
	int32_t				GetSubRewardCount(int32_t iID);

	//抽卡相关
	void				RltSyncGamePlayerDrawCardReward(const MG_SyncLotteryItem& rltMsg);

	//巅峰战场相关
	void				ReqPeakArenaAchieveInfo();
	void				ReqPeakArenaLadderInfo(int32_t index,int32_t size);
	void				ReqDrawArenaSectionReward(int iCode,int32_t iID);

	//篝火晚会相关
	void                ReqBonFirePartyLeftBuyItemTimes(); 
	void                ReqBonFirePartyBuyInfo(int buyTimes);  

	//节日活动 
	StringType          GetHolidayActivityTasks(int iHolidayActivityTaskType,int iTabIdx); 
	bool                IsHolidayActivityTasksPrompt(int iHolidayActivityTaskType,int iTabIdx);
	void                AcceptHolidayTasks(int iHolidayActivityTaskType); 
	void                AcceptUnFinishedTasks(Utility::UT_SIMDataList& InTasks);


	//委派任务相关
	StringType			GetAcceptEntrustTasks();	
	StringType			GetCanAcceptEntrustTasks();
	StringType			GetAcceptEntrustTaskInfo(int32_t iEntityId);
	StringType			GetAcceptEntrustTaskRewards(const std::vector<int32_t>& iEntityIds);
	int					GetEntrustTaskRefreshNum();
	bool				IsRuneEntrusted(int32_t iRuneId);	
	bool				IsBuddyEntrusted(int32_t iBuddyId);	
	bool				HasHighStarLevelTask();

	void				UpdateEntrustTaskInfo(std::vector<EntrustTask_Base>& AcceptedTasks,std::vector<int32_t>& CanAcceptTasks,int InRefreshTime);
	void				UpdateEntrustTaskRefreshNum(int InRefreshTime);
	void				OnAcceptEntrustTask(const EntrustTask_Base& AcceptedTask);
	void				OnFinishedEntrustTask(const EntrustTask_Base& AcceptedTask);
	void				OnTakeEntrustTaskReward(const std::vector<int32_t>& AcceptedTask);
	void				OnRefreshEntrustTask(const std::vector<int32_t>& AcceptedTask,int InRefreshTime);
	void				UpdateEntrustTaskState(int iId, bool bEntrusted);

	void				ReqRefreshEntrustTask();
	void				ReqEntrustTask(int iEntityID,const StringType& HeroList);
	void				ReqTakeEntrustTaskReward(const StringType& iEntityRwdList);
	void				ReqFinishEntrustTaskImmediately(int iEntityID);
private:
	void					_computePing();
	//开关技能接口
	void					_toggleSkill(int lSkillID);
	//坐骑/弩炮相关
	void					_updateComboCount(int iOldValue, int iAddCount);
	void					_clearComboCount();
	void					_onSkillFlagChanged(int64_t llOldAllFlag, int64_t llFlag);
	void					_initMuhunInfo(void);
	//自动进阶是否要继续进行
	bool					_shouldAutoUpgradeContinue(int iType, int iLevel, ResultType result);

	void					_updateExtraNodeData();

	//加载生产数据
	void					_initProductionData(void);
	//聊天CD错误消息
	void					_checkChatMsgSendFrequency(int iChannelType, float fCurrentTime);

	//核芯dirtyflag
	LC_DirtyFlag			m_DFStarVein;
	LC_DirtyFlag			m_DFStarVeinGoal; 


/************************************************************************/
/*                                                                      */
/************************************************************************/
//三角色系统 BEGIN
public:
	void							SetCurrentActiveBuddyIndex(uint16_t index) {m_uiCurrentActiveBuddyIndex = index;}
	bool							CheckTargetActiveBuddyIndex(uint16_t index);
	
	SingleActivateBuddyData*		GetActiveBuddy(int index) { return index >= 0 && index < m_vActiveBuddyList.size() ? &m_vActiveBuddyList[index] : NULL; }
	void							AddActiveBuddy(SingleActivateBuddyData& buddy) { m_vActiveBuddyList.push_back(buddy); }
	bool							UpdateActiveBuddyList(MG_SC_ActivateBuddyDataSync& BuddyDataMsg);
	int								GetActiveBuddyListSize() { return m_vActiveBuddyList.size(); }
	virtual bool					GetActivateBuddyPtr(LC_ActorPtrList& vecPtrList);

	SingleBuddyData*				GetInactiveBuddy(int index) { return index >= 0 && index < m_vInactiveBuddyList.size() ? &m_vInactiveBuddyList[index] : NULL; }
	void							AddInactiveBuddy(SingleBuddyData& buddy){ m_vInactiveBuddyList.push_back(buddy); }
	void							UpdateInactiveBuddyList(MG_SC_BuddyDataSync& BuddyDataMsg);

	void							UpdateActiveBuddyRespawnTimeList(const MG_SC_SetBuddyRespawnTimer& msg);
	void							ResetActiveBuddyRespawnTimeList(const MG_SC_ResetSingleBuddyRespawnTimer& msg);
	void							ResetAllActiveBuddyRespawnTimeList();

	//解锁角色
	void							ReqUnlockBuddy(int cID);

	//切换角色 && 交换数据
	bool							ReqActivateActiveBuddy(int activeIndex);

	void							SwapBuddyData(int activeIndex);
	void							SwapBaseClassData(LC_PlayerBase* pSrc, LC_PlayerBase* pDst);
	void							SwapOtherBaseData(LC_PlayerBase* srcPlayer, LC_PlayerBase* dstPlayer);
	void							SwapTitles(LC_PlayerBase* dstPlayer);
	void							ChangeID(object_id_type iNewObjectID);

	void							AttachEffects(LC_PlayerBase* srcPlayer, LC_PlayerBase* dstPlayer); //显示脚底光圈，添加出战特效
	
	float							GetRestSwapCoolDown(int index);
	void							SetSwapCoolDown(int index, float restTime);
	void							ActiveSwapCoolDown(int index, float restTime);
	void							DeactiveSwapCoolDown(int index);
	void							UpdateSwapCoolDown(float fCurrentTime);
	void							ResetSwapCoolDown();
	void							ResetSwapCoolDownByCID(int cID);
	int								GetPendingSwapIndex() { return m_iPendingSwapIndex; }
	float							GetMaxSwapCD() { return SWITCH_BUDDY_CD_TIME; }

	float							GetRestRespawnCoolDown(int index);
	void							ActiveRespawnCoolDown(int index, float restTime);
	void							DeactiveRespawnCoolDown(int index);
	void							ResetRespawnCoolDown(int index);
	void							UpdateRespawnCoolDown(float fCurrentTime);

	//复活伙伴
	bool							ReqRespawnBuddy(int index);
	bool							IsBuddyDead(int index);
	bool							IsAllBuddyDead();
	virtual bool					Respawn(attr_value_type lHP,int lMP);
	
	//重置镜头
	void							ResetCamera();

	//出战角色/下阵角色
	bool							ReqActivateInactiveBuddy(int activeIndex, int buddyID);
	bool							IsBuddyInstance(int cID);
	void							UpdateBuddyInstanceList();
	int								GetUnlockSlotNum() { return m_iUnlockSlotNum; }
	void							SetUnlockSlotNum(int num) { m_iUnlockSlotNum = num; }
	bool							GetUnlockSlotInit() { return m_iUnlockSlotInit == 1; }
	void							AccumulateUnlockSlotInit() { ++m_iUnlockSlotInit; }
	bool							ReqDeactivateBuddy(int activeIndex);

	//三角色工具
	bool							IsBuddy(uint32_t id);
	bool							IsNpcBelongToBuddy(LC_ActorBase* npcPtr);
	LC_PlayerBase*					GetBuddyPlayerBasePtr(object_id_type id);		//获得角色的PlayerBase pointer
	int								GetBuddyObjectID(int cID);						//获得角色的object ID;
	LC_ClientBuddyInstance*			GetBuddyInstance(int cID);
	LC_ClientBuddyInstance*			CreateNewBuddyInstance(int cID);
	bool							GetActivateBuddyFlag() const { return m_bActivateBuddyFlag; }
	void							SetActivateBuddyFlag(bool flag) { m_bActivateBuddyFlag = flag; }
	void							AttachEffectToActivateBuddy(int oID);
	int								GetActivateBuddyID(MG_SC_ActivateBuddyDataSync& BuddyDataMsg);		//获取出战角色的CID
	int								GetSkillUnlockLevel(int cID, skill_id_type skillID);				//查看某个技能是否学到了，如果有返回-1，没有返回学习所需等级
	void							SetPendingActivateCID(int cID) { m_iPendingActivateCID = cID; }
	int								GetPendingActivateCID() { return m_iPendingActivateCID; }
	void							ClearEyeAvatar();
	activateBuddyList				GetActivateBuddyList() { return m_vActiveBuddyList; }
	int								GetBuddyIndex(object_id_type objID);

	//阵容相关
	void							SetSchemeType(int iType) { m_iSchemeType = iType; }
	int								GetSchemeType() { return m_iSchemeType; }
	//void							SyncSchemeData(const SingleBuddyActivateSchemeData& syncInfo);
	//void							SyncAllSchemeData(const MG_SC_MultiBuddyActivateSchemeData& syncInfo);
	int								GetSchemeCid(int iElementType, int iSlotIdx);
	void							RequestSchemeModify(int iElementType, int iSlotIdx, int iCid);
	void							RequestSchemeSwitch(int iElementType);
	void							RequestSetRankTransferState(int mapID);

	
	//交互UI
	int								GetCurrentActiveBuddyIndex() { return m_uiCurrentActiveBuddyIndex; }	
	int								GetBuddyActiveCID(int index) { return index >= 0 && index < m_vActiveBuddyList.size() ? m_vActiveBuddyList[index].nBuddyTemplateId : 0; }
	int								GetBuddyInactiveCID(int index) { return index >= 0 && index < m_vInactiveBuddyList.size() ? m_vInactiveBuddyList[index].nBuddyTemplateId : 0; }							
	bool							GetBuddyInactiveUnlock(int index) { return index >= 0 && index < m_vInactiveBuddyList.size() ? m_vInactiveBuddyList[index].bUnlock : false; }
	bool							GetBuddyUnlock(int cID);
	int								GetTotalBuddyNum() { return m_vInactiveBuddyList.size(); }
	int								GetBuddyLevel(int cID);
	int								GetBuddyAllEquipReinforceLevel(int cID);
	int								GetBuddyEXP(int cID);
	combat_score_type				GetBuddyCombateScore(int cID);
	int								GetCurrentTransform(int cID);
	bool							IsBuddyTransformActive(int cID, int itransformID);
	int32_t							GetTransformInvaildTime(int cID, int itransformID);
	combat_score_type				GetActiveBuddyAllScore();

	void							AddDirtyFlagRevisionID(int iFlag);
	int	  							GetDirtyFlagRevisionID(int iFlag);
	virtual void					UpdateDirtyFlag(int Flag);
	virtual void					SetDirtyFlag(int Flag, bool bFlag);
	virtual bool					GetDirtyFlag(int Flag);
	virtual void					ClearDirtyFlag();
	virtual void					SetDirtyFlagRevisionID(int iFlag, int lRevisionID);
	virtual void					ClearDirtyFlagRevisionID();

	virtual int32_t					GetControllerEquipSlotReinforceLevel(int slotIndex) { return GetCommanderEquipReinforceLevel(slotIndex); }
	virtual int32_t					GetControllerEquipSlotRefineLevel(int slotIndex) { return GetCommanderEquipRefineLevel(slotIndex); }

	ResultType						CheckEquipItemInPackage(int iPackType, int iEntryIndex, int cID);
	LC_BackPackEntry*				GetRuneByItemId(int iItemId);

	//BuddyInstance相关
	LC_SkillAsset*					GetBuddySkillAsset(int cID);
	LC_ActorAttributeMap*			GetBuddyAttributeMap(int cID);
	LC_ActorAttributeAppendMap*		GetBuddyAttributeAppendMap(int cID);
	LC_ShortcutBar*					GetBuddyShortcutBar(int cID);
	bool							CheckBuddyActivateSkill(int cID, int skillID);
	bool                            CheckBuddyActivateSkillAll(int cID);

	int								GetBuddySkillUsableCount(int cID, int skillID);
	int								GetBuddySkillMaxUsableCount(int cID, int skillID);

	void							SyncBuddyLevel(int cID);									//同步角色的等级
	void							SyncBuddyEXP(int cID);										//同步角色的经验
	void							SyncBuddyCombate(int cID);									//同步角色的战斗力
	void							SetAllOwner(int cID, LC_PlayerBase* pkPlayer);
	void                            SendBuddyExpAndLevelChange(int oldExp, int oldLevel, int newExp, int newLevel, int cID);
	combat_score_type				GetAddEquipScore(LC_BackPackEntry* pkEntry);
	
	//角色升星
	bool							ReqUpgradeBuddyStar(int cID);
	int								GetBuddyStarLevel(int cID);
	long							GetBuddyUpgradeStarCost(int cID, bool bLast = true/*是否取前一级的数据*/);
	int								GetBuddyUpgradeItemID(int cID, bool bLast/*是否取前一级的数据*/);
	int								GetBuddyUpgradeQuality(int cID, int iStarLevel);
	long                            GetBuddyUpgradeUnlockskill(int cID, int iStarLevel);

	//新VIP 导出到lua
	bool							ReqTakeVipNewReward(int iLevel);
	/**/
	void							UpdateVipNewRewardRecord(const std::vector<int32_t>& vecTokenLevels);
	void							UpdateVipNewRewardRecord(int32_t iNewRewardTokenLevel);
	bool							UpdateVipNewLevel(int32_t iLevel);//返回是否是升级，初始化的时候Level是999，999->0不是升级
	void							UpdateVipNewOpen(bool bValue);
	void							ReqAcceptVipNewLevelTasks(int32_t iLevel);
	/**/
	bool							HasNottakenVipNewReward();
	bool							IsVipNewRewardToken(int iLevel);
	bool							IsVipNewOpen();
	int								GetVipNewTaskState(int iTaskID);
	int								GetVipNewLevel();

	//本地图玩家信息
	void							RequestGetNearbyPlayerInfo();
	/**/
	void							UpdateNearbyPlayer(const std::vector<PS_NearbyPlayerInfo>& pInfo);
	void							AddEnemyPlayer(unique_id_type id);
	void							RemoveEnemyPlayerByIdx(int EnemyIdx);
	void							RefreshShowEnemyPlayer();

	bool							IsTeamMate(object_id_type iTargetId);

	/**/
	StringType						GetNearbyPlayerInfo();
	StringType						GetShowEnemyPlayer();
	bool							AutoAttackTarget(object_id_type mainControllerID,object_id_type charType,int targetPkMode=0);

	//活动地图怪物数量
	void							ReqMapNpcCount(int storyID, int mapID);
	void							SetMapNpcInfo(MG_SyncMapNpcInfo& msg);
	int								GetNpcCountByMapID(int mapID, int npcID);

	//任务相关
	bool							IsTaskFinished(int iTaskId);
	bool							IsTaskCanFinish(int iTaskId);
	
	//玩家级相关
	LC_ClientGamePlayerCommander*	GetCommander() { return m_pkCommander; }
	virtual StringType				GetCommanderName() { return m_pkCommander->m_cPlayerControllerName; }
	virtual int						GetCommanderLevel() { return m_pkCommander->m_iLevel; }
	int64_t							GetCommanderEXP() { return m_pkCommander->m_iEXP; }
	int64_t							GetCommanderTotalScore(){ return m_pkCommander->m_iTotalCombateScore; }
	object_id_type					GetMainControllerID() { return m_pkCommander->GetMainControllerID(); }
	unique_id_type					GetCommanderUID() { return m_pkCommander->GetUID(); }
	virtual object_id_type			GetOwnerObjectID() { return GetMainControllerID(); }
	int32_t							GetBornCharType() { return m_pkCommander->m_iBornCharType; }
	int32_t							GetCommanderTitleFrameID() { return m_pkCommander->m_nCommanderTitleFrameID; }
	void							RequestUpgradeBuddy(int iCid);
	virtual int						GetControllerLevel() { return GetCommanderLevel(); }
	LC_ActorAttributeMap*			GetCommanderAttributeMap() { return m_pkCommander->GetAttributeMap(); }
	LC_ActorAttributeMap*			GetCommanderChainAttributeMap() { return m_pkCommander->GetChainAttributeMap(); }
	LC_ActorAttributeAppendMap*		GetCommanderAttributeAppendMap() { return m_pkCommander->GetAttributeAppendMap(); }
	int64_t							GetCommanderBaseAttributeScore();
	int64_t							GetCommanderChainScore() { return m_pkCommander->m_iChainCombateScore; }
	int								GetCommanderGetRevisionID() { return m_pkCommander->GetDirtyFlagRevisionID(); }
	int64_t							GetCommanderTitleCombatScore();
	int32_t							GetCommanderEquipSuitLevel() { return m_pkCommander->GetEquipSuitLevel(); }
	void							UpdateCommanderEquipScore(int64_t iValue){m_pkCommander->UpdateCommanderEquipScore(iValue);};
	combat_score_type				GetCommanderEquipScore(){return m_pkCommander->GetCommanderEquipScore();};

	virtual inline uint32_t			GetRegTime(void) const{ return m_pkCommander->m_iCharCreateTime; }
	inline  uint32_t				GetRegTime() { return m_pkCommander->m_iCharCreateTime; }

	//新装备协议相关
	LC_BackPackEntry*				GetEquipEntry(int iPackType, int iIndex) { return m_pkCommander->GetEquipEntry(iPackType, iIndex); }
	bool							IsExistSubEquip(int iItemID) { return m_pkCommander->IsExistSubEquip(iItemID); }
	void							ReqEquipCommanderEquip(int iSrcIdx);
	void							ReqCommanderEquipIntensify(int iIdx,bool bOneKey);
	void							ReqCommanderEquipRefine(int iIdx);
	void							ReqCommanderEquipSoul(int iIdx);
	void							ReqEquipSubEquip(int iSubType, int iIdx);
	void							ReqUnEquipSubEquip(int iSubType, int iIdx);

	int32_t							GetOwnerCurrentMP() { return m_pkCommander?m_pkCommander->GetCurMP():0; }
	int32_t							GetOwnerMaxMP() { return m_pkCommander?m_pkCommander->GetMaxMP():0; }
	uint32_t						GetOwnerTaskHonorPoint() { return m_pkCommander?m_pkCommander->GetTaskHonorPoint():0; }
	uint32_t						GetOwnerMainTaskFinishCount() { return m_pkCommander?m_pkCommander->GetMainTaskFinishCount():0; }

	void							CommanderLevelUp(uint16_t val);
	void							CommanderSetEXP(int64_t val);
	void							CommanderSetTotalCombateScore(int64_t val);
	//void							CommanderCheckLevelUp();
	void							UpdateChainCombatScore(int64_t val);

	void							PlayCommanderLevelupFx(int iLevel);
	void							PlayTaskFinishFx();

	//物资护送
	void							ReqRefreshConvoy(bool bAutoBuyMat,bool bForceHighest);
	void							ReqTakeEscortReward(int iEscortType,int iNpcId);
	int								GetEscortUsedCount();
	int								GetRobbingUsedCount();
	void							UpdateEscortTimes(int iValue);
	void							UpdateRobbingTimes(int iValue);
	void							UpdateEscortLevel();
	void							UpdateEscortState(int iState);
	int								GetEscortState();
	int								GetEscortTaskID();
	int								GetEscortLevel();
	StringType						GetBiaocheState();
	void							StartEscort();
	bool							IsGoodsEscortTaskProcessing();
	//buddy装备槽信息
	int								GetEquipSlotLevel(int icID, int iSlotID);
	int								GetEquipSlotStar(int icID, int iSlotID);

	void							RequestFireTimer(int iTimerID);

	void							SetCommanderLotteryCount(int32_t iCount) { m_pkCommander->SetLotteryCount(iCount); }
	int32_t							GetCommanderLotteryCount() { return m_pkCommander->GetLotteryCount(); }
	int32_t							GetCommanderMaxRuneStarLevel() { return m_pkCommander->GetMaxRuneStarLevel(); }
	int32_t							GetCommanderMaxRuneLevel() { return m_pkCommander->GetMaxRuneLevel(); }

	//GamePlayer特例化 (与BuddyInstance关联起来)
	virtual LC_SkillAsset*					GetSkillAsset() { return GetBuddySkillAsset(GetCharType()); }
	virtual LC_ActorAttributeMap*			GetAttributeMap() { return GetBuddyAttributeMap(GetCharType()); }
	virtual LC_ActorAttributeAppendMap*		GetAttributeAppendMap() { return GetBuddyAttributeAppendMap(GetCharType()); }
	virtual LC_ShortcutBar*					GetShortcutBar() { return GetBuddyShortcutBar(GetCharType()); }

	void							UpdateSingleCombatData(const MG_SC_SingleCombatRecorder& rkMsg);
	void							SetBossDropInfo(const MG_SyncBossDropInfo& rkMsg);

	void							SetSingleCombatRank(int key, int32_t iRank) { m_kSingleCombatData[key].m_iRank = iRank;}
	int								GetSingleCombatRank(int key);
	void							SetSingleCombatDamage(int key, int64_t value){m_kSingleCombatData[key].m_iDamageValue = value;}
	int64_t							GetSingleCombatDamage(int key);

	int32_t							GetRaidBossDropTimes(int iDropType=RecoverType_BossDrop);
	uint32_t						GetRaidBossDropNextRevertTime(int iDropType);
	uint32_t						GetRaidBossDropEndRevertTime(int iDropType);

	void							SetCurrentLineNum(int iNum) { m_iCurrentLineNum = iNum; };
	int								GetCurrentLineNum() { return m_iCurrentLineNum; };

	void							SetFinishMainTaskID(int iTaskID) { m_iFinishMainTaskID = iTaskID; };
	int								GetFinishMainTaskID() { return m_iFinishMainTaskID; };

	//红点
	void							ReqSetRedPointFlag(int iType, bool bFlag);
	void							UpdateRedPointFlag(int iType,std::vector<KeyValue_Base_Bool>& InVec);
	bool							GetRedPointFlag(int iType);

	HookingJump&					GetHookingJump() {return m_HookingJump;}

	void							SetIsInParalle(bool b) { m_bInParalle = b; }
	bool							GetIsInParalle() { return m_bInParalle; }

	//新手帮助: 
	void                            ReqAddNoviceTutorial(int noviceId) ; 
	void                            ReqGetNoviceTutorialReward(int noviceId) ;
	void                            SyncNoviceTutorialInfo(const std::vector<uint32_t> &m_vGetRewardNoviceTutorial,const std::vector<uint32_t> &m_vNoviceTutorialRecord);

   //苍穹奖励: 
	void                            ReqSkyArenaGradeAward(int m_nGrade) ; 
	void                            ReqSkyArenaMyScore() ; 
	void							ReqSkyArenaCheckBattleStage(unique_id_type uid, int iMapResID, int iSchemeType);
	void                            AcceptSpecialTask(int specialTaskId); 

	//巅峰竞技场 
	void                           ReqSummitArenaInfo() ; //界面信息
	void                           ReqSummitArenaGroupStageEntryList(); //参赛列表
	void                           ReqSummitArenaGroupStage() ; //小组战况 
    void                           ReqSummitArenaMyGroupStage(int mapId,int schemeType) ; //本组战斗
	void                           ReqSummitArenaAgainstList(int zoneId,int groupId,int type) ;  //64强
	void                           GetSummitArenaTime(SummitArenaTimeData& timeData) ; //拿剩余时间 
	void                           GetSummitArenaTimeStamp(SummitArenaTimeData& timeData) ; //上线时间戳 

	
	
//***角色语音 BEGIN
	virtual bool					IsSpecking();
	virtual void					UpdateVoiceLocation();

	void							PlaySuccessVoice();
	void							PlayFailVoice();

	float							m_fAutoPathSoundStartTime;
	const buddyInstanceMap&			GetBuddyInstanceMap(){return m_mBuddyInstanceMap;}

//***角色语音 END

//阵容配置
	void							SetMapFullSchemeData(MG_SyncBuddyActivateScheme& msg);
	void							SetMapFullSchemeData(MG_RESP_SetBuddyActivateScheme& msg);
	void							SetMapFullSchemeData(MG_RESP_SetSpecialMapInitBuddyActivateScheme& msg);
	void							ReqSetBuddyActivateScheme(int MapGroupID, int SchemeType, StringType& str);
	int								GetBuddySchemeCidByIndex(int MapGroupID, int SchemeType, int GroupID, int Index);
	void							ReqSetSpecialMapInitBuddyActivateScheme(int MapID, int SchemeType);
//黄金首领


//苍穹查看阵容
	void							ReqGetPlayerSchemeData(int32_t SystemType, int m_nMapID,int m_nSchemeID,int m_nSchemeType, uint64_t m_nCharID);
	void							ReqGetRankInfo(uint32_t m_nRankID,uint64_t m_nCharID,int m_nZoneId);

//苍穹战场结算

	void							SetSkyArenaSettleReward(std::vector<PS_ItemRecord>& RwdInfo);
	StringType						GetSkyArenaSettleReward();	
	void							SetSkyArenaCommanderBaseData(MG_NOTIFY_SkyAllShowInfo& msg);
	void							SetSkyArenaSettleReportList(std::vector<BaseBattlefieldReport>& info);
	LC_ClientBattleRecordDataBase*	GetSkyArenaSettleReportList() { return &m_SkyArenaSettle; };


//战斗记录
	void							ReqBattleRecord(int iType);
	void							UpdateBattleRecord(MG_Resp_BattlefieldLog_List& msg);
	void							ReqBattleRecordDetaiil(int iType, uint64_t index, int clientIndex, uint64_t iCharId);
	void							UpdateBattleDetail(MG_Resp_BattlefieldReport& msg);
	int								GetBattleRecordTotalNum(int iType);
	LC_ClientBattleRecordDataBase*	GetBattleRecordInfo(int index);
	LC_ClientBattleRecordDataBase*	GetPeakDuelBattleRecordInfo(uint64_t id);
	bool							IsDetailNeedRequest(int index);

	void							SetPlayBackBegin();
	bool							IsPlayBackFlag();

	void								SetBossFlagList(std::vector<PB_ServerPlayerBossFlagData>  &PlayerBossFlagInfo);
	StringType							GetBossFlagList();
	void								ReqSetMapBossFlag(int m_nMapID,int m_nCharTypeID,bool m_bFlag);

//进入地图次数
	void								SetMapEnterRecord(int iMapResID, int iCount);
	int									GetMapEnterRecord(int iMapResID);
	void								ReqGamestoryBatch(int iMapResID);

	void								SyncCanEnterMapIDSet(const std::vector<int32_t>& vec);
	void								AddCanEnterMapID(int iMapResID);
	void								ReqAddCanEnterMapID(int iMapResID);

//竞猜
	LC_ClientGuessDataBase*			GetGuessBaseData(){return &m_GuessData;};
	std::vector<LC_ClientGuessDataBase>* GetGuessLogData(){return &m_nSummitGuessLog;};
	int								GetLogSize(){return m_nSummitGuessLog.size();};
	int								GetLogMaxIndex();
	LC_ClientGuessDataBase*			GetGuessLogDataByIndex(int i);
	void							ReqGetSummitSingleData(uint32_t  m_nSummitType);
	void							ReqGetSummitSupportInfo(uint32_t  m_nSummitType,uint32_t m_nAgainstID);
	void							ReqSetSummitGuessInfo(uint32_t  m_nSummitType,uint32_t m_nAgainstID,uint64_t m_nChoseCharID);
	void							ReqGetSummitWinner(uint32_t m_nSummitType,uint32_t m_nAgainstID);
	uint32_t						GetGuessGroupID(){return m_nGroupID;};
	void							SetGuessGroupID(uint32_t GroupID){m_nGroupID = GroupID;};
	void							SetGuessLog();

private:
	bool								_isInCanEnterMapIDSet(int iMapResID);
private:
	LC_ClientGamePlayerCommander*				m_pkCommander;					//玩家Commander

	activateBuddyList							m_vActiveBuddyList;				//出战表
	inactiveBuddyList							m_vInactiveBuddyList;			//待出战表
	buddyInstanceMap							m_mBuddyInstanceMap;			//BuddyInstance表
	
	typedef mem::map<int, mem::vector<int>> BuddySchemeInfoMap;
	BuddySchemeInfoMap m_mSchemeInfoMap;
	
	mem::map<uint32_t, SingleCollectSuit>		m_mCommanderEquipSuitCollection;//套装收集信息

	mem::map<int, int>							m_mResourceRetrieveRecord;		//资源找回记录
	mem::map<int, int>							m_mResourceRetrieveMapRecord;	//锁定 地图通关记录
	int											m_iResourceRetrieveLevelRecord;	//锁定 指挥官等级

	uint16_t									m_uiCurrentActiveBuddyIndex;	//当前出战Index
	int											m_iUnlockSlotNum;				//已解锁的出战格子
	int											m_iUnlockSlotInit;				//是不是刚登陆的同步格子数量，如果不是UI需要做解锁格子的操作

	bool										m_bActivateBuddyFlag;			//用来检测是否下一个buddybasicdata为出战角色
	int											m_iPendingActivateCID;			//因为服务器不会通知，所以需要客户端自己保存目标新角色的CID
	bool										m_bEnterGameFlag;				//创建时为true,用于显示切换后的原GP模型
	int											m_iSchemeType;

	//相位副本flag
	bool										m_bInParalle;

	//荣光远征
	int											m_iRaidGloryTotalStarNum;
	mem::vector<int>							m_iRaidGloryChapterStarNum;
	std::vector<bool>							m_iRaidGloryRewardRecord;
	std::map<int,int>							m_nQuickRaidGloryCountMap;	
	int											m_iGloryDailRewardDrawed;
	int											m_iGloryPointNextRecoverTime;	//下次恢复时间
	int											m_iGloryPointEndRecoverTime;	//全部恢复时间

	//阵容设置
	mem::map<uint32_t, std::vector<PS_SingleSchemeData>> m_vMapOffensiveSchemeData;					//攻击阵容
	mem::map<uint32_t, std::vector<PS_SingleSchemeData>> m_vMapDefenceSchemeData;        			//防守阵容

	//战斗记录
	int															m_iBattleRecordNum;
	uint32_t													m_iBattleType;
	mem::vector<LC_ClientBattleRecordDataBase>					m_BattleLogList;
	mem::map<uint64_t, LC_ClientBattleRecordDataBase>			m_PeakBattleInfo;

	//竞猜
	LC_ClientGuessDataBase						m_GuessData;			//当前竞猜
	std::vector<LC_ClientGuessDataBase> 		m_nSummitGuessLog;		//竞猜记录
	uint32_t									m_nGroupID;				//当前的竞猜组

	typedef mem::map<int32_t,EntrustTask_Base> AcceptTaskVec;
	//委派任务相关
	AcceptTaskVec								m_AccpetedTasks;                					//已经接受的任务 <iEntityId,data>
	std::vector<int32_t>						m_CanAcceptTasks;                        			//可以接受的任务
	int32_t										m_iEntrustRefreshNum;                          		//可刷新次数
	mem::set<int32_t>							m_iEntrustedRunes;                          		//已被派遣的宝具

	//遗迹突袭寻宝
	std::vector<bool>							m_nYijiTuxiRewardIDRecord;								//
	std::vector<bool>							m_nYijiXunbaoRewardIDRecord;							//
	std::vector<bool>							m_nToyIslandRewardIDRecord;
	//幸运转盘
	int											m_iRotaryRewardIdx;
	int											m_iRotaryRewardValidTimes;			//抽奖次数
	bool										m_bChargedToday;
	mem::vector<int>							m_RotaryRewardRecord;
	int											m_iRotaryEndTime;					//活动结束时间
	bool										m_bSkipAnimation;					//是否跳过动画
	//极品白拿
	//int											m_nNonpareilBuyTime;				//购买结束时间
	//int											m_nNonpareilReturnTime;				//领奖结束时间
	//mem::map<int, int32_t>						m_nNonpareilRecord;					//购买记录
	//mem::vector<int>							    m_nNonpareilReturnRecord;			//已领奖记录
	mem::map<int,bool>                              m_nBuyNoPayList ; 

	//首充
	int											m_FirstChargeTime;
	std::map<int,bool>							m_FirstChargeRewardRecord;
	//切换计时结构体
	struct SwapTimeCount
	{
		SwapTimeCount()
		{
			bInCoolDown = false;
			fStartTime = 0;
			fLastTime = 0;
			fRestTime = 0;
		}

		bool bInCoolDown;
		float fStartTime;
		float fLastTime;
		float fRestTime;
	};

	SwapTimeCount								m_vSwapTimeCounts[MAX_ACTIVE_BUDDY_NUM]; //计时表每次update刷新
	int											m_iPendingSwapIndex;

	SwapTimeCount								m_vActiveBuddyRespawnTimeCounts[MAX_ACTIVE_BUDDY_NUM];		//出战复活CD表

	//空格冲刺
	HookingJump									m_HookingJump;

	//通知UI用的Dirty_Flag
	LC_DirtyFlag								m_DFBuddyActiveList;
	LC_DirtyFlag								m_DFBuddyInActiveList;
	LC_DirtyFlag								m_DFCurrentIndex;
	LC_DirtyFlag								m_DFCurrentSchemeList;
	LC_DirtyFlag								m_DFCurMapPlayerInfo;
	LC_DirtyFlag								m_DFRaidGloryInfo;
	LC_DirtyFlag								m_DFNPCFunction;
//***三角色系统 End


//***相位副本 BEGIN
public:
	//相位副本
	void RemoveAllBullets();
	void ClearAllSkillSound();
	void ChangePhaseMap(int iMapLogicID, int lMapResID, int lMapLogicType);
//***相位副本 END
private:
	struct PingData
	{
		PingData()
		{
			uiStartPingTime = 0;
			uiSocketPingTime = 0;
			uiGameWayPingTime = 0;
			uiGameServerPingTime = 0;
		};

		uint32_t uiStartPingTime;
		uint32_t uiSocketPingTime;
		uint32_t uiGameWayPingTime;
		uint32_t uiGameServerPingTime;
	};

	mem::map<uint8_t,PingData>	m_pingMap;

	DWORD				m_currentGameServerPing;
	DWORD				m_currentGateWayPing;
	DWORD				m_currentSocketPing;

	uint8_t				m_token;
	uint16_t			m_auiUIOperationRecords[64];	//记录主玩家UI操作

	int					m_iNewPlayerLevel;

	bool				m_bPendingToSetVisibleDataEnableFakeShadow;

	//pvp战斗状态下播放屏幕泛红特效的id
	int					m_iPVPGfxID;
	UT_Vec3Int			m_kNpcTalkUIPos;

	int					m_NotifyInGame;

	//站立进入挂机倒计时
	float				m_fStandToHookTimer;

	//AFK后打坐倒计时
	bool				m_bEnableAfkToMeditation;
	bool				m_bEnableStandToHook;
	//站立进入全自动倒计时
	bool				m_bEnableToGuding;
	bool				m_bDoingOtherTask;	//是否正在进行非主线任务，进行其他任务时不能自动主线
	float				m_fGudingTimer;
	int					m_iAutoTaskType;	//当前自动任务类型
	bool				m_bFinishTaskFlag;

	//挂机进入全自动倒计时
	float				m_fHooking2GudingTimer;
	bool				m_bEnableHookingToGuding;
	bool				m_bTutorialStop;

	//挂机无效技能时间判断
	float				m_fInvalidTime;
	UT_Vec3Int			m_kInvalidPos;
	object_id_type		m_fLastInvalidTarget;

	//胯下坐骑
	object_id_type		m_lastVehicleCharType;		//上一次坐骑的CharType
	////手中武器
	//int					m_lastWeaponItemID;		//武器itemid，包括门派武器和骑战武器
	////弩炮
	//int					m_lastBowLevel;
	////天眼
	//int					m_lastEyeCharType;
	//int					m_iEyeNPCID;
	////羽翼
	//object_id_type		m_lastWingCharType;
	////法宝等级		
	//object_id_type		m_lastJCCharType;
	//int					m_lastJCLevel;
	//object_id_type		m_lastJCSetLevel;
	//int					m_idleTime;
	////灵宠
	//int					m_lastLCCharType;
	//object_id_type		m_lastLCID;
	////灵武
	//int					m_lastLCWeaponType;
	////暗器等级		
	//object_id_type		m_lastAnQiCharType;
	//object_id_type		m_lastAnQiID;
	//int					m_vehicleZFix;

	//连击特效数据
	int					m_iComboEffectData;
	//公会战守卫方的公会ID
	int					m_iPowerGuildId;
	//公会战当前圣火令所属玩家ID
	int					m_iGuildWarType;
	//死亡复活计时器
	float				m_fRespawnTimer;
	int					m_eRespawnFadeState;

	//跳转地图后，继续自动寻路坐标点
	int					m_iTransferTargetMapResID;
	UT_Vec3Int			m_kTransferTargetPoint;

	bool						m_bFirstEnterGame;

	std::vector<int>			m_vAtvChargePoint;

	//是否领过公会每日福利
	bool						m_bGuildRewardIsVaild;
	//玩家每日活跃度
	int32_t						m_nDailyActivity;
	mem::vector<int32_t>        m_nDailyActivityRewardID;
	
	//传记
	mem::vector<int32_t>  m_nBiographyChapters;
	
	//首充七日奖励
	int32_t						m_dayFlag;
	int							m_dayTime;
	
	//死亡状态下进入巅峰战场保存死亡时间
	int							m_lSaveDeadTime;
	//组队副本队伍数据
	int				m_iCrossRealMapTeamID;
	int             m_iCrossRealTeamMapID; // -1 无，队伍的mapID
	LC_ClientArenaBattleChallengeInfoAsset	m_kArenaBattleChallengeInfoAsset;
	LC_PlayerDetailManager m_kPlayerDetailInfoData;	//查询的网络玩家数据管理器。

	LC_PlayerEquipSuitPtrMap m_kPlayerEquipSubSuitPtrMap;

	public:
		void						SetTranToTranTick(float tick)	{	m_bTranToTranTick = tick;}
		float						GetTranToTranTick()	{	return m_bTranToTranTick;	}
		private:
		float						m_bTranToTranTick;

		bool TryReadMap(int lMapResID);
		struct	tagMapLoadInfo
		{
		public:
		//////////////////////////////////////////////////////////////////////////
		bool	m_bLoadingMap;
		bool	m_bLoadedMap;
		int		m_iMapLogicIDSave;
		int		m_lMapResIDSave;
		int		m_lMapLogicTypeSave;
		Utility::UT_Vec3Int m_rkLocationSave;

	public:
		//////////////////////////////////////////////////////////////////////////

		tagMapLoadInfo() { reset(); }

		void	reset()	{
			m_bLoadingMap = false;
			m_bLoadedMap  = false;
			m_iMapLogicIDSave   = 0;
			m_lMapResIDSave     = 0;
			m_lMapLogicTypeSave = 0;
			m_rkLocationSave.clear();
		}

		void	set(int iMapLogicID,int lMapResID,int lMapLogicType,const Utility::UT_Vec3Int& rkLocation);
		//////////////////////////////////////////////////////////////////////////

		bool	isLoading(void) const { return m_bLoadingMap; }
		bool	isLoaded(void) const { return m_bLoadedMap; }
		void	setLoaded(void)  { m_bLoadedMap = true; }
		void	beginLoad(void)  { m_bLoadingMap = true; m_bLoadedMap = false; }
		void	endLoad(void)  { m_bLoadingMap = false; m_bLoadedMap = false; }

	}d_MapLoadInfo;

	//玩家排行数据
	std::map<int32_t,PS_Index_Entry> m_RankData;
	
	int									m_currentSpecialEquipt[ET_MAX_COUNT];		//当前特殊时装功能
	
	//tab锁定功能记录标志
	mem::map<object_id_type, bool> m_flagVec;

	//发出技能请求之后计时，如果返回超时，除了引导型技能，其他都丢弃这次回包
	Utility::TickCount	m_skillRltDelayTimer;

	//需要目标提示间隔
	float m_needTargetMessgeTick;
	//技能CD提示间隔
	float m_fSkillCdMessageTick;


	//因为现在技能有客户端的假CD，为了客户端表现CD动画，普攻还有会逻辑
	bool m_normalContinueCheck;

	bool m_bAlreadyReqTransfer;
	TickCount m_kReqTransferInterval;

	GameOptCache m_kGameOptCache;

	//BUG #15632 【功能】【任务系统】日常任务和帮会任务增加双倍领取功能
	//要求日常任务和帮会任务完成时，弹出任务对应的任务面板
	//这里通过存储任务的状态(从PROCESSING 到 CANFINISH的时候发送一个时间)
	mem::map<int,int> m_kTaskState;

	bool	m_bImmediateTaskStart;
	bool	m_hookRespawnFlag;
	bool	m_hookRespawnPathFlag;		//挂机复活后，寻路标志
	bool	m_hookRespawnDeadFlag;		//挂机复活后，寻路过程中死亡标志
	bool	m_bhookRespawnBuyFlag;		//挂机原地复活请求标志

	PlatformMap	m_platforminfo; //平台登录的一些玩家信息

	//< online time
	unsigned int m_enterGameTimeStamp;	//< 进入游戏的时刻

	//bool	m_bInitAttribute;
	ACT_SecretTreasureAsset			m_kSecretTreasureAsset;
	//dailycleandata
	//服务器说接下来每日清空的相关数据都会在这里进行处理
	//主要跟周涛进行对接的相关工作
	std::map<uint32_t, int32_t>		m_dailyCleanMap;
	inline	uint32_t	_GetDailyCleanKeyID(uint32_t nType, uint32_t nID) { return nType * 1000 + nID; }

	mem::set<uint32_t>  m_kTimeRemainState;

	int				m_lastMainTaskID;
	int				m_lastMainTaskLevel;
	float			m_lastMainTaskTick;

	uint32_t		m_uiLastOffLineTime;


	enum WeakOpGuideType
	{
		WOGT_NONE = 0,
		WOGT_DAILY_PART_FIN,
		WOGT_DAILY_WHOLE_FIN,
		WOGT_GUILD_PART_FIN,
		WOGT_GUILD_WHOLE_FIN,
		WOGT_FRONTIER_PART_FIN,
		WOGT_FRONTIER_WHOLE_FIN,
		WOGT_MAIN_FIN,
	};
	WeakOpGuideType	m_opFinishDailyTask;
	bool			m_obtainFlag;

	//特殊皮肤激活后 自动拾取功能
	bool			m_bCimeliaAutoPicking;

	bool			m_bIntelligentTask;

	bool			m_bPostEnterGameFlag;

	//星魂背包是否开启 伙伴，进阶线
	bool			m_bRuneBuddySlotOpen;
	bool			m_bRuneSubSlotOpen;
	mem::map<int, bool> m_kRuneRecordMap;



	typedef mem::map<int, mem::map<int, bool>> RuneBuddySlotStatMap;
	RuneBuddySlotStatMap m_kRuneBuddySlotDressStateMap;

	mem::map<int, int> m_ActiveCodeUseCount;
	mem::set<int> m_ActiveStarVeinIDs;
	mem::set<int> m_ActiveStarVeinGoalIDs;
	mem::map<int, int> m_HolydeedUserCount;

	mem::vector<int> v_InvestmentType;
	mem::vector<int> v_InvestmentFreeReward;
	mem::vector<int> v_InvestmentHighReward;

	//灵魂连锁信息
	mem::vector<int> m_vBuddyChainVec;
	uint32_t		 m_iChainFlag;

	int32_t          m_iRegularBossGroupID;
	uint32_t         m_iRegularBossGroupCDTime;

	mem::map<int64_t, StringType>	m_mBossKillerInfo;

	int				m_iLotteryConsumeUnbindyuanbao;

	int				m_iNewAchievementStarNum;
	mem::vector<int32_t>	m_vNewAchievementRewardCord;

	bool			m_bGuildProfitShowName;
	uint32_t		m_iGuildProfitLevel;
	uint32_t		m_iGuildProfitLevelExp;
	uint32_t		m_iGuildProfitCrystalLevel;
	uint32_t		m_iGuildProfitCrystalLevelExp;
	mem::map<uint32_t, uint32_t> m_vGuildProfitCrystal;

	mem::map<int32_t, PS_GuildBossInfo> m_mGuildBossActivity;
	mem::set<int32_t> m_sGuildBossActivityFinishMap;
	PS_GuildMapBossPlayerInfo		m_sGuildBossActivityMyRank;
	mem::vector<PS_GuildMapBossPlayerInfo>		m_vGuildBossActivityRankThree;
	mem::vector<PS_GuildMapBossPlayerInfo>		m_vGuildBossActivityRankList;

	//角色成长礼包
	mem::map<int32_t, PS_SingleBuddyGrowthPackage> m_kCharacterUpgradGift;
	typedef mem::map<int32_t, PS_SingleBuddyGrowthPackage>::iterator CharacterUpgradGiftIter;

	struct BuddyCombatScore
	{
		BuddyCombatScore()
		{
			m_nCommanderCombatScore = 0;
			m_nChainCombatScore = 0;
		};

		combat_score_type m_nCommanderCombatScore;
		combat_score_type m_nChainCombatScore;
	};

	mem::map<int,BuddyCombatScore>	m_mBuddyCombat;

	//竞技排行榜礼包
	mem::map<int32_t, int32_t> m_kSuperShopCompete;
	//开服争霸排行榜礼包
	mem::map<int32_t, int32_t> m_kSuperShopSub;
	
	//玩家炼魂系统
	LC_ClientSoulRefineAsset	m_kSoulRefineAsset;
	
	//玩家活动
	LC_ClientActivityAsset	m_kActivityAsset;
	
	//物资护送
	int						m_iEscortTimes;
	int						m_iRobbingTimes;
	int						m_iEscortLevel;
	int						m_iEscortState;//1正在护送 2受到攻击 3停止护送

	mem::map<int, PS_GoalInfo> m_mBestGuildGoalInfo;
	
	//Boss首杀活动
	typedef mem::map<int32_t, mem::vector<PS_BossFirstKillPlayerInfo>> FirstKillRewardPlayerMap;
	mem::map<int32_t, PS_BossFirstKillPlayerInfo> m_kBossFirstKill;
	FirstKillRewardPlayerMap m_mBossFirstKillRewardPlayerInfo;	//Boss首杀奖励已领取玩家信息
	mem::set<int> m_kServerBossFirstKillRecord;									//全服Boss首杀奖励已领取记录，存的是boss的chartype
	mem::map<int, bool> m_mBossFirstKillRecord;									//个人Boss首杀奖励已领取记录，存的是boss的chartype
	
	//七日试炼活动
	int				m_iSevenDaysEndTime1;		//活动持续时间
	int				m_iSevenDaysEndTime2;		//领奖持续时间

	int				m_iFirstDayLoginRewardTime;

	mem::map<int32_t,bool> m_mCommanderFrame;		//已激活的头像框，bool值表示是否查看过
	mem::map<int32_t,bool> m_mCommanderHead;		//已激活的头像，bool值表示是否查看过

	//悬赏任务刷新次数
	int				m_iOfferTaskRefreshTimes;
	int				m_iOfferTaskAcceptTimes;

	StringType				m_mDrawCardCumlateReward;		//寻宝累计抽奖奖励
	//新VIP礼包 true，已领取
	mem::vector<int>		m_VipNewRewardRecord;
	int32_t					m_iCurVipNewLevel;
	bool					m_bVipNewOpen;

	int						m_iCollectBossJP;

	mem::map<int,bool>		m_nRedPointPlusFlag;
	
	//公会战数据
	mem::map<int, ST_Alliance_Boss_StateInfo> m_BestGuildBossInfoMap;
	mem::map<int, ST_Single_Tower_info> m_BestGuildTowerInfo;
	mem::map<int, ST_Single_Tower_info> m_BestGuildFortressInfo;
	mem::map<int, ST_Single_Tower_info> m_BestGuildRoyalInfo;
	bool									m_BestGuildRewardFlag;//结算奖励是否领取
	int										m_BestGuildFinishedTask;//结算完成奖励数量
	//本地图内的玩家
	mem::vector<PS_NearbyPlayerInfo> m_VecNearbyPlayers;

	//邀请冷却时间 玩家ID，上一次邀请的时间戳
	mem::map<unique_id_type,float>	m_InviteCountDown;

	//攻击过我的玩家  ID,是否在附近
	mem::vector<unique_id_type>		m_ShowEnemyPlayers;

	//vip当日已快速挂机次数
	int								m_iVipQuickHookTime;
	std::vector<PS_ItemRecord>		m_iVipQuickHookRwd;
	
	mem::vector<PS_SkillBookSlotData> m_kSkillBookSlotData[MAX_ACTIVE_BUDDY_NUM];

	mem::map<int, SingleCombatRecord>					m_kSingleCombatData;//storyid >>  SingleCombatRecord

	mem::map<int, RaidBossDropInfo>						m_kRaidBossDropData;

	typedef mem::map<int32_t,PS_FirstDailyChargeReward> FirstDailyChargeRewardsMap;
	typedef FirstDailyChargeRewardsMap::iterator FirstDailyChargeRewardsMapIter;
	FirstDailyChargeRewardsMap m_kFirstDailyRecord;

	typedef mem::map<int32_t, int64_t> ACTSubConpeteEndMap;
	typedef ACTSubConpeteEndMap::iterator ACTSubConpeteEndMapIter;
	ACTSubConpeteEndMap m_kActSubCompeteEndRecord;

	LC_DirtyFlag        m_DFShopTimeLimit;
	mem::vector<PS_ActivityTimeLimitedInfo> m_kShopTimeLimit;

	bool							m_bCommanderTitleOpen;
	bool							m_processPathFlag;

	bool				m_bIsSwitich;
	int					m_iCurrentLineNum;
	int8_t				m_bSwitchLine;
	
	//活动地图中怪物数量
	mem::map<int, mem::map<int, int>>		m_mNpcCount;

	//主题活动
	std::map<int32_t,std::map<int32_t,bool>> m_nThemeACTRaidBossRwdRecord;
	std::vector<int32_t>					m_nThemeACTRaidBossFlag;//在列表里说明点过了，不需要红点

	std::map<int,bool>						m_nRedPointFlag;
	//主线任务奖励特权双倍领取 ---------就是遗落宝箱
	mem::map<int,int>						m_mExtraRewardInfo;
	int										m_iFinishMainTaskID;
	
	//黄金boss
	std::vector<PB_ServerPlayerBossFlagData>	m_vServerPlayerBossFlagInfo;
	int											m_nResult;
	PB_ServerPlayerBossFlagData					m_serverPlayerBossFlagData;

	//进入地图次数记录
	mem::map<int, int>							m_kEnterMapRecord;
	mem::set<int>								m_kCanEnterMapIDSet;

	//苍穹战场结算界面							
	std::vector<PS_ItemRecord>					m_SoloRwd;
	LC_ClientBattleRecordDataBase				m_SkyArenaSettle;
};	

#endif
