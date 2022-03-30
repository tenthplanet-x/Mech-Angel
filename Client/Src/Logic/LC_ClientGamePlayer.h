#ifndef _LC_CLIENT_GAME_PLAYER_H
#define _LC_CLIENT_GAME_PLAYER_H
//�ͻ���ר��Game Player��
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
	NiColor		kOutlineColor;	//������ɫ

	MouseOverTargetArg(void)
		: kOutlineColor(NiColor::WHITE)
	{}

	MouseOverTargetArg(NiColor & a)
		: kOutlineColor(a)
	{}
};

struct PathPendingActionArg	//Ѱ·������Ϊ
{
	PathPendingActionType	enmType;
	int			iTargetCharTypeID;	//Ŀ���CharType
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

typedef activateBuddyList::iterator			m_vActiveBuddyListItr;			//��վ��ָ��
typedef inactiveBuddyList::iterator			m_vInactiveBuddyListItr;		//����վ���ָ��
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

	//ת����ɫ����
	void					FaceAt(const Utility::UT_Vec3Int& rkTargetDir);
	void					FaceToLocation(const Utility::UT_Vec3Int& rkTargetLocation);
	//�������
	void					SetMouseLogicModeType(MouseLogicModeType eType){m_eMouseLogicModeType = eType;};
	MouseLogicModeType		GetMouseLogicModeType(){return m_eMouseLogicModeType;};

	void					ShowMessage(int lMessageType);
	void					ShowNoticeMessage(int iMsgType, const StringType& sMsg, int iLevel);

	LC_PlayerEquipSuitPtrMap& GetPlayerEquipSubSuitMap(){ return m_kPlayerEquipSubSuitPtrMap; }

	//Begin ��Ʒ�����ӿ�
	virtual GameLogic::LC_PackAsset* GetPackAsset() {	return LC_PlayerBase::GetPackAsset();}
	//��������:ʹ����Ʒ
	virtual ResultType		CheckUseItemInPackByIndex(int iPackType, int iEntryIndex, int32_t& iCount, int iParam);
	virtual ResultType		RequestUseItemInPackByIndex(int iPackType, int iEntryIndex, int32_t iCount, int iParam,const StringType &text, int iBuddyCID = 0);
	virtual ResultType		RequestUseItemInPackByType(int lItemTypeID, int iParam, int cID = 0, int iCount=1);
	virtual ResultType		RequestUsePatchItemInPackByIndex(int iPackType, int iEntryIndex, int32_t iCount, int iParam,const StringType &text, int iBuddyCID = 0);
	//��������:װ����Ʒ
	void					RequestEquipItem(int iEntryIndex, int cID, int iCategory);
	//���������������Ʒ
	virtual ResultType		RequestSplitItemInPackage(int iPackType, int iSrcIndex, int iDesIndex, int iSplitNum );
	//�����������ϲ���Ʒ
	virtual ResultType		RequestMergeItemsInPackage(int iSrcPackType, int iSrcEntryIndex, int iDstPackType, int iDstEntryIndex);
	//����������������Ʒλ��
	virtual ResultType		RequestExchangeItemsInPackage(int iSrcPackType, int iSrcEntryIndex,int iDstPackType, int iDstEntryIndex, bool bNeedEmpty = false);
	//����������������Ʒ
	virtual ResultType		RequestTidyItemsInPackage(int iPackType);
	
	//��������������
	virtual ResultType		RequestDestroyItemInPackage(int iPackType, int iEntryIndex);
	//����ǿ���������װ��
	void					RequestEnhanceEquipmentInPackage(int iPackType, int iEntryIndex, int iLevel0, int iLevel1, int iLevel2, int iLevel3, int iLevel4, int iLevel5, int iLevel6, int iLevel7);
	//��������Ʒ
	ResultType				RequestUnbinItemInPackage(int iSrcPackType, int iSrcEntryIndex, int iDstPackType, int iDstEntryIndex);

	// ǿ��װ��&����
	void					RequestEnhanceEquipLevel(int iBuddyID,int iEquipIndex, const StringType & iConsumeItems );
	void					RequestEnhanceEquipStar(int iBuddyID,int iEquipIndex);
	void					RequestReinforceEquipSlot(int iBuddyID,int iEquipIndex,bool bOneKey);

	int64_t					GetCommanderAllEquipCombatScore();

	int						GetOpenBagRemainSeconds();

	int						GetCanCostItemCount(int iItemID, bool bLmtTime = false);
	//������Ʒid��ȡλ��index
	int						GetItemSlot(int iPackType,int iItemID);
	ResultType				CheckUpgrade(int iType, int iPayIndex);
	int						GetItemListValueCount(int iType);
	int						GetCimeliaNeedLevel(int iType);
	//��������ɳ�ϵͳ
	void					RequestUpgradeCimelia(int iType);
	void					RequestAutoUpgradeCimelia(bool bStart, int iType);

	//���Ž���
	void					RequestUpgradeMiji(int iID, int iPos);
	void					OnRltUpgradeMiji(ResultType result);
	//�յ����׻�Ӧ
	void					OnRltUpgradeCimelia(ResultType result, int iType, int iRank, int iPayIndex, const StringType& sModifyAttribs="");

	//�û�����						
	void					RequestChangeTransform(int iType, int iOpCode, int iSetLevel);
	void					RequestTransformCimelia(int iType, int iOpCode, int iTransformID);
	void					OnRltChangeTransform(ResultType result, int iType, int iCode, int iParam);
	void					RequestBless(int iType, int iOpCode, int iSetLevel);

	//��û����
	void					RequestCommonReward(int type, int op_code, int op_para);
	void					RequestCommonBaseReward(int type, int op_code, int op_para);
	void					RequestActivityCommonOp(int type, int catagory, int schedule, int opcode, const StringType& params);
	void					RequestActivityCommonOpEx(int type, int catagory, int schedule, int opcode, const StringType& params);
	void					RequestGetRankSubBInfo(int iType, int iGroupID, int iMinIndex, int iMaxIndex);

	//����������Ϣ
	void					RequestShotActivityInfo(const StringType &idx);
	

	//����ʹ�ü�����
	void					RequestUseActivationCode(const StringType &ActiveCode, int iType);
	//������
	void					RequestTreasureDerive(int iId);
	void					RequestTreasureLog();
	//���籦��
	ACT_SecretTreasureAsset& GetSecretTreasureAsset(void){ return m_kSecretTreasureAsset;};
	void					RequestSecretTreasure(int iCode, int iParam1,int iParam2);
	//����¯
	void					RequestSmelter(int iMeltType,const StringType &operate);
	void					RequestBuyVigor();
	//�ǳ�����
	void					RequeseStarRuneOperation(int iCatagory,int iIndex);

	//�ɾ�
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

	//����װ��-------
	//void							ReqEquipVehicleEquipment( int iPackType, int iEntryIndex, int iVehicleIdx );
	//void							ReqUnEquipVehicleEquipment( int iVehicleIdx );
	//void							UpdateVehicleEquip();
	void							ChangeVehicleAvatar(int lNewEquipID);
	//void							ChangeVehicleAvatar( int lOldEquipID, int lNewEquipID );
	//int							GetCurVehicleEquipID();
	//-------

	//������ؽӿ�
	//int								GetVehicleGrownupValue(int iLevel);//����ɳ�Ǳ��
	int								GetVehicleAvatarID(void);
	//int							GetVehicleCurrentLevel(void);	//��������ƶ���PlayerBase���棬��ΪServer��BuffҪ�õ�����ӿ�

	//�ɳ�ϵͳ 
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
	//�ǻ���ؽӿ�
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


	//��ȡ��ǰ��������ID
	int								GetCurrentVehicleSkill(void);

	//�Զ��������
	void							SetIsAutoUpgradeCimelia(int iType, bool bAutoUpgrade);
	bool							GetIsAutoUpgradeCimelia(int iType);

	void							SetIsDoAutoUpgradeCimelia(int iType, bool bAutoUpgrade);
	bool							GetIsDoAutoUpgradeCimelia(int iType);

	//�Զ���������
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

	//�����ʯ
	ResultType						RequestUnSocketItemInPackage(int iEquipPackType, int iEquipIndex, int lEquipItemID, int iSocketIndex);

	ResultType						RequestUseItemTriggerTask(int iPackType, int iEntryIndex);

	// ��������������ֿ�
	ResultType						CheckPutIntoWarehouse(int& iTargetIndex, int iTargetPackType);
	void							RequestPutIntoWarehouse(int iPackType, int iEntryIndex, int iTargetPackType);
	// �����������Ӳֿ���ȡ��
	ResultType						CheckTakeOutFromWarehouse(int iSrcIndex, int iSrcPackType,int& iPackType, int& iTargetIndex);
	void							RequestTakeOutFromWarehouse(int iSrcIndex, int iSrcPackType);
	// �����������ӱ��������ȡ��
	void							RequestTakeOutFromTreasureBag(int iPackType, int iSrcIndex);
	//End ��Ʒ�����ӿ�
	virtual ResultType				RequestBuyGoodsFromNPC(int iIndex, int iNumber, int iPackType, int iPackIndex, int NpcCharType, bool bSendLogicMessage);

	//��԰������ʱ
	virtual ResultType				RequestBuyGoodsFromGuildNPC(int iShopEntryIndex, int iNumber, int iPackType, int iPackIndex);
	void							RequestSaveCash2Guild(int iGold,int iSilver,int iCopper);

	//ϴ��װ��
	void							RequestRefineItemInPackage(int iEquipPackType, int iEquipIndex, int iItemID, int iAttribIndex);
	//ѡ��ϴ������
	void							RequestApplyRefineItem(int iEquipPackType, int iEquipIndex, int iItemID, int iUseNew);
	//����
	bool							RequestModifyName(const StringType& sName, int iSlotID);
	// �ֿ����
	ResultType						SaveCashToWarehouse(int iGold,int iSilver,int iCopper);
	ResultType						GetCashFromWarehouse(int iGold,int iSilver,int iCopper);

	//Begin ״̬����
	void							SetEnablePathFinding(bool bEnable){m_bEnablePathFinding = bEnable;};
	bool							GetNeedLockTarget(){return m_bNeedLockTarget;};	//Ŀ���ͼ��ܣ�����0�������־λ������˼��
	void							SetNeedLockTarget(bool bNeed){m_bNeedLockTarget = bNeed;};
	bool							GetNeedLockLocation(){return m_bNeedLockLocation;};	//�����ͼ��ܣ��ص��ͼ��ܣ�����2�����ǳ����ͼ��ܡ������־λ������˼��
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

	//��װ�ռ�
	void							SyncCmdEquipSuitCollection(const std::map<uint32_t,SingleCollectSuit>& suitMap);
	void							ReqCmdEquipSuitCollectReward(int iOpType, int iLevel, int iCount);
	//��װ״̬
	int								GetCmdEquipSuitCollectState(int iLevel);
	//����װ��״̬
	int								GetCmdEquipSuitCollectItemState(int iLevel, int iEquipType);
	//��װ������ȡ״̬
	StringType						GetCmdEquipSuitCollectRewardStateInfo(int iLevel);
	//��װ�������
	bool							IsCmdEquipSuitCollectLevelPrompt(int iLevel);
	bool							IsCmdEquipSuitCollectPrompt();

	//��Դ�һ�
	void							SyncResourceRetrieveRecord(const MG_SyncResourceRetrieverRecord& msg);
	void							ReqResourceRetrieveReward(const StringType& sIds, bool bSpecial);
	StringType						GetResourceRetrieveRecordInfo();
	bool							GetResourceRetrievePrompt();
	int								GetResourceRetrieveRecordCommanderLevel() { return m_iResourceRetrieveLevelRecord; }
	int								GetResourceRetrieveRecordMapID(int iId);

	//< �����������
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
	//> �����������
	//< WASD��������
	bool							GetWASDStride(){return m_bWASDStride;};
	void							SetWASDStride(bool bMoveStride){m_bWASDStride = bMoveStride;};
	//> WASD��������

	virtual void					SetAttackerID(object_id_type lID);
	virtual StateMachine::StateType	FindMoveState();
	//End	״̬����

	//Begin ��Ʒ���׽ӿ�
	void                    RequestTrade(unique_id_type lTradeID);
	void					RltTrade(unique_id_type lTargetID);			//���������������׽��
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
	//End   ��Ʒ���׽ӿ�

	//�������
	void					SetEquipWeapon(int lNewItemID);		//���õ�ǰ���е���������
	bool					ChangeAvatar(int lOldEquipID,int lNewEquipID);
	void					ResetLastEquipData();
	void					SetLastEquiptID(int equipID, int equiptType);
	int						GetLastEquiptID(int equiptType);

	//���¹��������װAvatar
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

	/// ͳһ��װǰ�����
	void					HideBeforeSuitApplied();
	void					ShowAfterSuitApplied();

	//����
	bool					_updateEyeAvatar();
	void					_clearEyeAvatar(void);

	//����
	void					_updateJC();

	//���
	void					_updateLC(float fCurrentTime, float fDeltaTime);

	//����
	void					_updateLCWeapon();

	//����
	void					_updateAnQi();

	//Begin ����������
	//virtual GameLogic::LC_SkillAsset*	GetSkillAsset(){return LC_PlayerBase::GetSkillAsset();};
	virtual GameLogic::LC_SkillBookAsset&		GetSkillBookAsset(){return LC_PlayerBase::GetSkillBookAsset();};

	ResultType							PendingSkillInSkillAsset(int lSkillTypeID);
	//End ����������

	void                    RequestCollectItem(int lID);

	//������λ����������boss��д��صĴ���
	bool					OnEnterPhaseMap(int iMapResID);
	
	//������npc����ʱ��Ҫ�ж�npc�Ĺ���
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
	//�۲�Ŀ��
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
// 			//�ҵ���ǰ��
// 			if ( find1  && !find2 )
// 			{
// 				return true;
// 			}
// 			else if ( !find1 && find2 )
// 			{
// 				return false;
// 			}
// 			//���ҵ������ǰ��
// 			else
			if ( Is1Player && !Is2Player)
			{
				return true;
			}
			//���ҵ������ǰ��
			else if (!Is1Player && Is2Player)
			{
				return false;
			}
			//���Ҳ����������ж�
			else
			{
				return (iDistance1 < iDistance2 );
			}
		}
		Utility::UT_Vec3Int m_kPlayerLocation;
		LC_IDSet			m_kAttackerIDSet;
	};


	void					ManualChangeLockedTarget();		//�������
	void					ManualChangeLockedTargetFunc1(); //������򣬼������
	void					ClearChangeLockedFlag();	//���ѡ�еı�־

	//PVPĿ��
	void					SetPVPTargetID(object_id_type lTargetID){m_lPVPTargetID = lTargetID;}
	object_id_type			GetPVPTargetID(void){return m_lPVPTargetID;}

	virtual bool			NotifyStateEnd(int iEndState,float fCurrentTime,float fDeltaTime);

	//pkbuff
	bool					ClearPkDebuffID();


	//-------------         Begin ����         ---------------------//
	virtual void			NotifySkillEvent(Skill::SK_SkillEvent* pkSkillEvent);
	virtual void			_onCurrentFightStateChanged(bool bFightState);

	//�����ܣ��Լ��������ܵ���Ʒ
	virtual ResultType			PendingSkill(int lSkillTypeID,int lItemID);
	virtual	void				ClearPendingSkill();
	//���ܹ���ɹ����趨���﷽������λ�õ�
	virtual void				OnPendingSkillSucceed(int lSkillID, LC_LogicObject *pkTarget);	//pkTarget can be NULL
	//��鼼���Ƿ���Թ���
	virtual ResultType			CheckPendingSkill(int lSkillTypeID);
	//����Ƿ���Թ����ܣ����ʧ�ܣ�����ʹ����ͨ�������ýӿ���Ҫ���ڿ����UI
	int							CheckPendingSkillEx(int lSkillTypeID, bool bContinueNormalAttak);
	//��鼼���Ƿ��ܹ�ʹ�ã��Լ�������Ʒ�Ƿ���Ч
	virtual ResultType			CheckExecuteSkill(float fCurrentTime,Skill::SK_SkillExecute* pkSkill,int lItemID);	
	//׼��ִ�м���
	virtual ResultType			PrepareExecuteSkill(Skill::SK_SkillExecute* pkSkill,int lItemID);
	//ִ�м��ܣ�������״̬��������ִ��Update
	virtual ResultType			ExecuteSkill(float fCurrentTime,Skill::SK_SkillExecute* pkSkill,int lItemID);
	//������˲�����ܣ����޸�״̬����ǰ״̬
	ResultType					ExecuteSkillImmediately(float fCurrentTime,Skill::SK_SkillExecute* pkSkill,int lItemID);
	//Ŀ������輼�ܣ����޸�״̬����ǰ״̬
	ResultType					ExecuteLCSkillTargetImmediately(float fCurrentTime,Skill::SK_SkillExecute* pkSkill,object_id_type lLocked);
	//��������輼�ܣ����޸�״̬����ǰ״̬
	ResultType					ExecuteLCSkillImmediately(float fCurrentTime,Skill::SK_SkillExecute* pkSkill,object_id_type lLocked, const UT_Vec3Int& lockLocation, float dirAngle);
	//����ִ�м���
	void						RequestExecuteSkill(float fCurrentTime,Skill::SK_SkillExecute* pkSkill,int lItemID);
	void						RequestExecuteToyPetSkill(float fCurrentTime,Skill::SK_SkillExecute* pkSkill);
	void						OnRequestExecuteSkillFailed(int lSkillID);

	int							GetCurrentSkillTypeID();
	//��ǰ����ִ�еļ��ܼ�¼�����������غ������־
	void						_doUnlockRequestExecuteSkill(bool bExecute, int lSkillID);
	void						_updateUnlockRequestExecuteSkill(float fCurrentTime);
	void						_clearUnlockRequestExecuteSkill(void);
	//�ű�����ִ�м���			
	void						RequestExecuteSkillScript(float fCurrentTime, int lSkillid, int lItemID);
	//�����жϼ���
	virtual void				RequestInterrputSkill(float fCurrentTime,int lSkillTypeID,SkillInterruptType eInterruptType);
	virtual ResultType			InterruptSkill(float fCurrentTime, int lSkillTypeID, SkillInterruptType eInterruptType);
	//�ȴ�����ļ��ܣ���Ҫ����Ŀ���ص����ܹ���
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

	//������Ϯս������
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


	//��¼����ļ���
	void						RecordLastSkillID(int lSKillID); 

	//��Ծ
	virtual ResultType		CheckPendingJump(StateMachine::StateType st);
	virtual ResultType		PendingJump(StateMachine::StateType st, UT_Vec3Int & kTargetLocation);
	//����ִ�м���
	void					RequestJump(StateMachine::StateType st,UT_Vec3Int & kTargetLocation, int jumpType);

	virtual void			MoeveAfterJump();

	void					QuickPick();			//���ʰȡ
	bool					FastPick();
	StringType				GetInFovNetPlayerIDs();
	void					CallFriend(GameLogic::LC_Attacker* pkScriptCaller, int lCharTypeID);
	void					ReqReturnToCity(void);		//�����سǣ�ִ�лسǼ��ܣ����ܽ�������SC_ClientSkillInterface::ReturnToCity
	void					ReturnToCity(void);			//�سǼ���ִ�гɹ�
	void					ImmReturnToCity(void);		//�����س�
	void					OnTranscriptionSucceed(void);	//���ͽ�������ͼload��
	void					_updateTranscriptionSucceed(float fCurrentTime,float fDeltaTime);	//���ͽ�������ͼload��
	bool					IsNeedToEnalbeGuide();	//�Ƿ���Ҫ�����Զ�����

	//bool					IsTransformAttackSkill(int lSkillID);

	//�����ͷ�ʧ��ԭ��
	void					SetSkillFailReason(int& lSkillFailReason, int iReasonIndex, bool bFail);
	bool					GetSkillFailReason(int lSkillFailReason, int iReasonIndex);

	void					SetBExecuteSkill(bool bExecute) { m_bExecuteSkill = bExecute;}
	bool					GetBExecuteSkill() { return m_bExecuteSkill; }

	
	//-------------         End ����         ---------------------//

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

	//���ˡ�����
	virtual void			PunchMove(SkillMoveActorPositionType Type, const Utility::UT_Vec3Int & kTargetPoint,float fTime);

	// ��Ǯ�ӿ�
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


	//Begin ��ӽӿ�
	void							RequestCreatePlayerGroup();
	void							RequestExitPlayerGroup();
	void							RequestJoinPlayerGroup(unique_id_type lCharID);
	void							RequestKickPlayerFromGroup(unique_id_type lCharID);
	void							RequestDismissPlayerGroup();
	void							RequestConfrimGroupJoin(int iCandidateIndex,bool bAccept); // �ӳ�ͬ������,����index
	void							RequestConfrimGroupJoinByPlayerID(unique_id_type lPlayerID, bool bAccept);// �ӳ�ͬ�����룬 ����playerid
	void							RequestInvitePlayerToGroup(unique_id_type lPlayerID);		//  ������������
	void							RequestConfirmInviteToGroup(unique_id_type lInvitePlayerID,int lGroupID,unique_id_type lBeInvitePlayerID,bool bAccept);// �ӳ�ȷ���Ƿ�����������
	void							RequestConfirmBeInviteToGroup(int iGroupIndex,bool bAccept);	//  ���ȷ�ϱ��������
	void							RequestConfirmBeInviteToGroupByGroupID(unique_id_type lInviteCharID, int iGroupID, bool bAccept); //���ȷ�ϱ��������,ͨ��groupid
	void							RequestAppointmentCheif(unique_id_type lCharID);
	void							RequestBlockPlayerJoin(int iCandidateIndex);
	void							RequestModifyQuality(int iIndex);
	void							RequestForbidJoin(bool bFlag);
	void							RequestSyncInviteGroupMemberList(int iIndex);		// �����������Ķ���
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
	//End ��ӽӿ�

	//���νӿ�
	void					RequestAddForbidPlayer(unique_id_type lID, const StringType& strName);
	void					RequestRemoveForbidPlayer(unique_id_type lID);
	object_id_type			GetForbidPlayerID(int iIndex);
	StringType				GetForbidPlayerName(int iIndex);
	bool					IsForbidPlayer(unique_id_type lID);

	virtual bool			IsFriend(unique_id_type lTargetCharID);
	virtual bool			IsEnemy(unique_id_type lTargetCharID);

	//���ܸ���ӿ�
	void					RequestConfirmSkillRespawn(object_id_type lRespawnSourcePlayerID,bool bAccept);
	//~���ܸ���ӿ�
	
	//Begin PK�ӿ�
	void					RequestModifyPKSetting();
	void					RequestModifyPKSettingForce();
	void					RequestPKValue();
	//ͬ�����Ŀ�굽�����б�
	void					RequestAgreeAddToEnermy(unique_id_type lPlayerID);
	//End PK�ӿ�

	//Begin ����ӿ�
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
	//End   ����ӿ�

	//�����ӿ�
	//void              InvitePlayerToDuel(object_id_type lID);
	//bool              IsBeInviteToDuel();
	//StringType        GetBeInviteInfo();
	//void              AcceptDuelInvite(bool bAccpet);
	//end �����ӿ�

	//������ؽӿ�
	LC_PlayerFlopManager*	GetFlopManager(){return m_pkFlopManager;}
	//~������ؽӿ�

	//���������ӿ�
	bool					IsNeedUnBindCashSkillUpGrade(int lSkillCateGory, int iSlotIndex);
	void					RequestSkillUpGrade(int lSkillID,int iMax, int cID);//, bool bConfirmUseUnBindCash);
	//�����Զ���������
	void					RequestAutoUpgradeSkill(bool bStart, int iSkillID, int cID);
	//����ѧϰָ�ӹټ���
	void					RequestLearnControllerSkill(int iSkillID, int iSubType);

	//~�����������ӿ�

 	//����״̬ȡ���ӿ�		
	void					RequestDeactiveSkillState(int lSkillStateTypeID);

	//����״̬��Ч�ӿ�
	void					EndSkillStateGFX();
	void					BeginSkillStateGFX();

	int						GetTimeStampFromCurrent(int iSecond, int iMinute, int iHour, int iDay/* = -1*/, int iMonth/* = -1*/, int iYear/* = -1*/);
	int64_t					GetEndOfDayTimeStampFromCurrent(time_t iCurTime, time_t iTargetTime);

	//����߾���ͼ
	void					RequestAlliance();
	void					RltAlliance(int groupID, Protocol::UnitServerNodeList& li);
	void					RltEnterAlliance(int retType);
	LC_ClientAlliance*		GetAlliance()	{	return &m_kClientAlliance;	}

	//����
	void					RequestSoulOperation(int iType, bool bBindYuanbao);

	StringType				GetInvestmentPlanTaskID(int iType);
	StringType				GetInvestmentPlanScheduleID(int iType);

protected:
	static bool				_sortFunc(const LC_ItemDescriptionEntry *v1, const LC_ItemDescriptionEntry *v2);
	StringType				genRaidBossSpeciesInfoStr(const std::vector<PS_Buddy>& buddyInfo, const std::vector<PS_Boss>& monsterInfo);

public:

	//~����״̬��Ч�ӿ�
	void					RecvLogicAreaTraceFromGameServer(Protocol::ProtocolAreaTraceInfo& rkInfo);

	//���͹���
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

	//�ƺ�
	void					RequestSetTitle(int iTitleType,int iTitleIndex);
	bool					NeedRefreshTitleList();

	bool					IsNeedUnBindCashItemEnhance(MoneyType ulPrice);

	//����
	LC_ClientProduction*	GetClientProduction(){return m_pkClientProduction;};
	void					LearnProduction(int lProductionType, int lProductionLevel);
	void					RequestConfirmForgetProduction(int lProductionType, bool bAccept);
	void					UpgradeProduction(int lProductionType);
	void					LearnFormula(int lFormulaID);

	//����ѵ��
	bool					SetBitVec(int nIndex, bool bSet);

	//void					HandlerGameEvent(GameLogic::LC_GameEvent& rGameEvent,StringType& rEventStringParam);

	//�罻ϵͳ
	void					AddTempFriend(unique_id_type lCharID, const StringType& strName, int lCareer, int lLevel);
	void					AddFriend(unique_id_type lCharID, const StringType& strName, const StringType& strAskMsg);
	void					AddEnemy(unique_id_type lCharID, const StringType& strName);
	void					AddBlackList(unique_id_type lCharID, const StringType& strName);

	void					DeleteTempFriend(unique_id_type lCharID);
	void					DeleteFriend(unique_id_type lCharID);
	void					DeleteEnemy(unique_id_type lCharID);
	void					DeleteBlackList(unique_id_type lCharID);

	//����ͬ�������������Ϣ
	void					ReqPlayerInfoNearBy();

	//��������ı�
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

	// ��ѯϵͳ
	void					AccurateSearch(const StringType& strPlayerName, unique_id_type iCharID,const StringType& strAskMsg);
	bool					PrivateChatSearch(const StringType& strChatMsg, int cLenght);
	void					ResetDetailInfo();

	LC_ClientSocietyAsset*	GetSocietyAsset() { return &m_kClientSocietyAsset; }
	LC_ClientFriendMsgBroad*GetFriendMsgBroad() { return &m_kClientFriendMsgBroad; }
	LC_ClientSearchPlayer*	GetSearchPlayer() { return &m_kClientSearchPlayer; }
	LC_ClientViewPlayerInfo*GetViewPlayerInfo() { return &m_kClientViewPlayerInfo; }
	LC_ClientFriendNick*	GetFriendNick() { return &m_kClientFriendNick; }
	LC_ClientWorldLevelAsset* GetWorldLevelAsset() {	return &m_kClientWorldLevelAsset;	}

	//Ѱ·���
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
	void					PendingPathToLocation(int iMapResID , const Utility::UT_Vec3Int& pos, int iMapLogicID = 0);	//����Ѱ·�ص�
	void					PendingPathToVal(int iMapResID , int x, int y, int z, int val, int iMapLogicID);
	void					PathToPendingLocation();	//Ѱ·�������Ѱ·�ص�
	void					ClearPendingPathFind();
	int						GetCurrentGudingMapResID()	{	return m_iPendingPathMapResID; }

	//Ѱ·������Ϊ
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

	//���������Ϣ ---��ʱ�汾
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

	//�ϳ���ϵͳ
	//bool					IsNeedUnBindCashComposition(MoneyType ulPrice);

	//����ϵͳ
	ResultType				RequestEnableTalent();
	ResultType				RequestUpdateTalent(int iFaction, int lFirstLevelTalentID);
	ResultType				RequestUpdateAllTalent();

	//���
	virtual void			SetRiderCharType(int lCharType);

    void					EnableAreaEffect(void);
    void					DisableAreaEffect(void);

	//����
	void					SetIsCanStartTutorialFlag(bool bIsCanStartTutorial){m_bIsCanStartTutorial = bIsCanStartTutorial;};
	bool					GetIsCanStartTutorialFlag(){return m_bIsCanStartTutorial;};

	void					SendServerPlayerAntiWallowState(AnitWallowState state);

	//��ݼ�
	void					SetWaitingShortcutKey(bool bWaitingShortcutKey){m_bWaitingShortcutKey = bWaitingShortcutKey;};
	bool					GetWaitingshortcutKey(){return m_bWaitingShortcutKey;};

	void					SetIsSaveShortcutKey(bool bIsSaveShortcutKey){m_bIsSaveShortcutKey = bIsSaveShortcutKey;};
	bool					GetIsSaveShortcutKey(){return m_bIsSaveShortcutKey;};

	void					RequestSaveShortcutKey();

	//�����������ɼ�
	void					SetWeaponVsible(bool bVisible);
	void					SetBowVsible(bool bVisible, int suitPos);
	void					SetWingVisible(bool bVisible);

	//��¼UI����
	void					RecordUIOperations(int uiID);
	void					ClearUIOperationRecords();

	//���÷�������
	void					SetRevealInvisibilityParams(const UT_Vec3Int& rkCircleCenter, int iCircleRadius, bool bRevealOnce);
	
	//��ѯ����
	void					RequestNPCOwnerShip(object_id_type lNPCID);

	//���عһ��ű�
	void					LoadControlScript(bool bUseScript,bool bReload);

	//����
	//void					RequestChangeServer(int iServerID);
	
	//����
	void					SetNewPlayerLevel(int iLevel){m_iNewPlayerLevel = iLevel;};
	int						GetNewPlayerLevel(){return m_iNewPlayerLevel;};

	LC_ActorBase*			GetNearFirstActorByCharTypeID(int lCharTypeID);

	//�뿪����
	void                    RequestLeavePrison();
	bool                    IsBindCrashEnough();
	void                    RequestReducePKValue();
	int                     GetReducePkReqCash();
	int                     GetReucePkResultValue();

	//�һ�
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
	//������������ߵ�Ŀ��
	float					GetLastAttackerIDTick() { return m_fLastUpdateTime;}
	void					SetLastAttackerIDTick(float dTime) {m_fLastUpdateTime = dTime;}

	//���
	//void					RequestGetGift(int lGiftID, int lItemTypeID);

	//�����ճ�
	bool					CheckRecvLuckAward();
	void					RequestRecvLuckAward();

	//����
	void					StartFollowTarget(object_id_type lTargetID);
	bool					StopFollowTarget(object_id_type err);	//true if following before stop, else false
	void					UpdateFollowTarget();
	//ÿ������ɱ�ּ��� 2014/4/11 ���໪
	int						GetDaylyTaskKillCount(int taskid);
	void					SetDaylyTaskKillCount(int taskid, int icount);
	//pvpս��״̬�²�����Ļ������Ч��id
	void					SetPVPGfxID(int iGfxID){m_iPVPGfxID = iGfxID;};
	int						GetPVPGfxID(){return m_iPVPGfxID;};

	void					RequestUseDoubleExpInfo();

	//------------begin �����ͼ��lua���͸�c++��Ҫ��������------------//
	void					ResponseFromLuaAfterMapChanged(int type);
	//------------end �����ͼ��lua���͸�c++��Ҫ��������------------//

	//��ᴫ�����
	void					RequestGuildTransport(int transportId);
	void					ClickGuildTransportIcon(const StringType& name,int transportId);
	
	//վ������һ�ʱ��
	bool					AccumulateStand2Hook(float fDeltaTime);
	void					ResetStand2Hook() { m_fStandToHookTimer = AFK_TO_COUNTER;}
	void					EnableStand2Hook(bool b) { m_bEnableAfkToMeditation = b;}
	float					GetStand2HookTime()	{	return m_fStandToHookTimer;	}

	//�ر�ͼ
	void					ReqGetTreasurePos();
	void					RltGetTreasurePos(const MG_RLT_GetTreasurePos& msg);
	StringType				GetTreasurePosInfo();
	bool					IsCanDigTreasure();
	ResultType				CheckTreasureEvents(int iItemID);

	//����
	void					ReqGameRestartReward(int iId);

	//����ȫ�Զ�
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

// 	//�һ�����ȫ�Զ�
	bool					AccumulateHooking2Guding(float fDeltaTime);
	void					ResetHooking2Guding() { m_fHooking2GudingTimer = AFK_TO_GUDING_PROCESS;}
	void					EnableHooking2Guding(bool b) { m_bEnableHookingToGuding = b;}
	float					GetHooking2GudingTime()	{	return m_fHooking2GudingTimer;	}

	//�һ�����ʧ��
	float					GetInvalidHookSkillTime()	{ return m_fInvalidTime; }
	void					SetInvalidHookSkillTime(float fT)	{ m_fInvalidTime = fT;	}

	const UT_Vec3Int &		GetInvalidHookPos() { return m_kInvalidPos;}
	void					SetInvalidHookPos(const UT_Vec3Int& pos) { m_kInvalidPos = pos;}

	object_id_type			GetInvalidHookTargetID()	{	return m_fLastInvalidTarget; }
	void					SetInvalidHookTargetID(object_id_type iID)	{	m_fLastInvalidTarget = iID;}

	//�Ṧ
	void					NoticeShowChingkungUI(bool bShow);

	//��ȡ��ɫ��ǰ��Ļλ��
	StringType				GetWorld2PtCurrentLocation(void);

	//���װ��
	void					DecompositionEquip(int iPackType, int iEntryIndex);

	//��ȡ��Ҹ�������
	int						GetRaidScore(int raidID);
	int32_t					GetRaidRecord(int raidID);
	//�ж�����Ƿ񵽹�������ĵ�ͼ
	bool					HasMapCategoryArrived(int iMapCategoryID);
	UT_Vec3Int&				GetNpcTalkUIPos(int xOffset, int iUIHeight);

	void					RequestSetAssistInfo(int lTypeID);
	void					RequestGetHookingGuideLocation(void);
	void					SendRequestInfoByType(int iType, uint32_t lID, int iParam1, int iParam2, int iParam3=0);
	void					SendRequestInfoByTypeWithStringPram(int iType, const StringType& kParam);
	void					SendRequestRaidBossSpeciesRankInfo(int StoryID, int MinIndex, int MaxIndex);

	void					RequestRankInfo(int iRankID, int iBegin, int iEnd, int serverID, int areaID=0);  //�������а��б�  areaID=0��ʾ����
	void					RequestPlayerBuddyRankInfo(int iType, uint64_t iCharID, int serverID); //������ϸ��Ϣ buddy������ͣ�����buddy:1  ��Ӣ��buddy:2��
	void					RequestBuddyRankInfo(int cID, uint64_t iCharID, int serverID);		//������ϸ��Ϣ ��Ӣ�����а�
	void					RequestToyPetRankInfo(uint64_t iCharID, int serverID);				//������ϸ��Ϣ  ���
	void					RequestSubRankInfo(int iType, uint64_t iCharID, int serverID);		//������ϸ��Ϣ  ������
	void					RequestRankTopnPlayerInfo(const StringType& kParam, int iTop, int iType); //���������ǰn�� iType��buddy������ͣ�����buddy:1  ��Ӣ��buddy:2�� ��������
	void					RequestBuddyRankIndex(uint64_t iCharID);					//����һ���������buddy������

	void					RequsetArenaGiftState();				//���󾺼�������״̬
	void					RequsetArenaRankList(int iBegin, int iEnd, int serverID);

	void					RequestPlayerDetail(unique_id_type iUid, int iSystemType=PIS_NORMAL);
	bool					IsRedNameInvalidTTT(int iMapResID);
	void					SendRequestSameLocationDifLine(int MapID, int ServerID, int Line);
	void					SendRequestCommitAnswer(int topicIndex, int choiceIndex);
	void					SendRequestAnswerRankInfo(int MinIndex, int MaxIndex,int rankType);
	void					SendRequestYiJiTuXiRankInfo(int MinIndex, int MaxIndex, bool bFlag);

	//ָ�ӹ�װ������
	void					EquipAdvanceReq(int InType, int InIndex);

	//������װ������
	void					CimeliaEquipAdvanceReq(int InType, int InIndex);
	//����Զ�ͬ������������ҷ��͵��������
	void					RequestMemberAutoAdd(bool bAdd);

	virtual ResultType		CanExitGuild(LC_GuildAsset* pkGuildAsset);
	virtual ResultType		CanModifyGuildDeclaration(LC_GuildAsset* pkGuildAsset);
	virtual ResultType		CanKickMemberFromGuild(LC_GuildAsset* pkGuildAsset);

	//Begin ������
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
	//End ���������

	//�������
	int						GetRankByType(int rankType, int serverType);

	void					CreatePortal();//����������
	void					UpdatePortalData(const Protocol::MG_PortalAssert& kMsg);
	bool					IsGravePressed()const;
	bool					CheckGameStoryCondition(const GameStoryInfo& kGameStoryInfo, int iMapID = 0);
	
	//�����ٻ����ýӿ�
	unique_id_type			GetTmpSummorPool(int index);
	void					SetTmpSummorPool(int index, int val);
	int						GetTmpSummorCurCount();
	void					ClearTmpSummorPool();
	void					PlayerComboEffect(int iCount);
	void					RewardComboCount(int iOldValue, int iAddCount);
	LC_ClientNetPlayer*		GetSummoredPlayerData();
	void					SetSummorFriendCharID(uint32_t iID);
	//�����������ʱ��
	int						GetFriendOffLineTime(int offLineTick);
	//��ǰ�Ƿ�Ϊ��ֹ����״̬
	bool					 IsForbbidenKey(void) { return m_bForbbidenKey; }
	void					SetForbbidenKey(bool bForbbiden) { m_bForbbidenKey = bForbbiden; }

	bool					IsCanAcceptTaskLogin() { return m_bAcceptTaskLogin; }
	void					SetAcceptTaskLogin(bool bAccept) { m_bAcceptTaskLogin = bAccept; }
	void					AcceptTaskAfterLogin();

	LC_ClientRankUserInfo*	GetPlayerDetailInfoByUserID(unique_id_type iUserID);
	StringType				GetGuildWarData();
	//�سǼ���Ŀ���������
	StringType				GetReturnCityName();
	//���ע��ʱ��
	bool					IsRegistThisMonth();
	StringType				GetRegistTimeInfo();
	int						GetRegDays();
	
	//���Ĥ������
	ResultType CheckAddGood(int goodtype,int playerid);
	StringType GetCanGoodCountStr();

	//��û����
	int GetGameStoryScore( int type);

	bool					IsEquipBetter(LC_BackPackEntry *pkEntry);

	LC_TargetMap*			GetTabTargetMap() {	return &m_tabLockedMap;}
	
	int64_t					GetGuildTransportStamp(){ return m_lGuildTransportStamp;}
	void					SetGuildTransportStamp(int64_t stamp){ m_lGuildTransportStamp = stamp; }
	uint32_t				GetGuildTransportResId(){return m_lResId;}
	void					SetGuildTransportResId(uint32_t iResId){ m_lResId = iResId; }

	int						GetCommonStatReserveTimes(int32_t uiType);	//�������δ��ɵĴ���
	bool					IsPurchaseBackOpen();					//�����Դ���ص���͵ȼ�
	StringType				GetCommonStatCostMoney(int32_t uiType);	//������Ļ���

	//���ְ������
	void                    SetTutorialProgress(int tutorialId){m_iTutorialProgress = tutorialId;};
	int                     GetTutorialProgress(){return m_iTutorialProgress ; };
	void                    AddRewardRecord(int tutorialId);
	bool                    IsAchievedReward(int tutorialId) ; 

	//����λ������� 
	void                    SetSkyArenaRankRewardAchievedId(int achievedId){m_iSkyArenaRankRewardId=achievedId;} ;
	bool                    RewardIsAchieved(int rewardId) ; 

	//������� 
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

	//�۷徺���� 
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
	//��ҫ֮��
	void					ReqSummitArenaHonor(int nZoneId,int nIndex);
	void					ReqGetMyTopHonor();
	void					SetPlayerList(std::vector<SummitArenaHonorDataMsg>&	topPlayerList);
	StringType				GetTopPlayerInfo(int index);
	int                     GetZoneId(){return m_zoneId;};   //m_zoneId ;   //����Id 
	int						GetTopErr(){return TopErr;};
	void					SetTopErr(int topErr){ TopErr = topErr;};
	int						GetTopPlayersSize(){return TopPlayerList.size();};
	//�����߷� 
	int                     GetSkyArenaMaxPoint(){return m_iMaxScore;} ; 
	void                    SetSkyArenaMaxPoint(int maxScore){m_iMaxScore=maxScore;} ; 

	      



	//��ȡ�������Ӧ���ܵ�����
	int						GetSkillBookCateGory2(int iPackType, int iSlot);
	//��ȡ�����鵱ǰѧϰ��Ӧ�ļ��ܸ�������Ϣ
	int						GetUseSkillBookSlotIndex(int iCategory2);

	//ʥ�����
	void					RequestActiveSkillBook(int iID);
	void					RequestUnlockSkillBook(int iIndex, int iSlot);
	void					RequestWearSkillBook(int iIndex, int iSlot, int iSkillBookID);
	void					SyncSkillBookSlotInfo(const MG_SyncSkillBookSlotInfo& msg);
	int						GetSkillBookSlotSkillBookId(int iIndex, int iSlot);		//��λ���ʥ��ID
	bool					GetSkillBookSlotActive(int iIndex, int iSlot);			//��λ�Ƿ���
	int						GetFirstEmptySkillBookSlot(int iIndex);			//��ȡ��һ����λ

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

	//���ֽ���
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
	ResultType				_checkNeedLockTarget(Skill::SK_SkillExecute* pkSkillExecute);	//��鵱ǰ�Ƿ��Ѿ�����Ŀ��
	//ResultType				_checkSkillInSafeArea(int lSkillID, int lTargetID);			//�ж��Ƿ��ڰ�ȫ�����ͷż���

	bool					_isCmdEquipSuitCollectLevelPrompt(const SingleCollectSuit& suitInfo);

	//��ǰ�Զ����׵�ǰӦ��ʹ������payindex
	int						_getCurrentAutoUpgradeCimeliaPayIndex(int iType);

	void					_resetTriggerInfo();

	//�ͷż���ʱ����alt����ϵͳ�������趨ΪĿ�꣬���ݴ�ԭ�ȵ�Ŀ�꣬
	//�ͷ��꼼�ܺ���Ҫ�ָ�ԭ�ȵ�Ŀ��
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
	
	//��һ���Զ�����
	CimeliaUpgradeTimeMap			m_fLastAutoUpgradeCimeliaTime;
	//�Ƿ������Զ�����
	CimeliaUpgradeInfoMap			m_bAutoUpgradeCimelia;
	//ִ����һ���Զ�����
	CimeliaUpgradeInfoMap			m_bDoAutoUpgradeCimelia;

	//һ����������
	//��һ���Զ�����ʱ��
	float							m_fLastAutoUpgradeSkillTime;
	//�Ƿ������Զ���������
	bool							m_bAutoUpgradeSkill;
	//�Ƿ�ִ����һ���Զ���������
	bool							m_bDoAutoUpgradeSkill;
	bool							m_bNpcTask;

	int								m_iCurrentUpgradeSkillID;
	int								m_iCurrentUpgradeBuddyID;

	int								m_iGroupId;			//ս��ID
	int								m_iSkyArenaPoint;	//���ս������ֵ
	int								m_iSkyArenaHonorPoint; //���ս���������ѻ������ֵ(�������)
	int								m_iLastSkyArenaPoint;//�򿪽���ʱ�������һ������ֵ

	int								m_iSkyArenaSeason;	//���ս����ǰ����
	uint32_t						m_iSkyArenaCloseTime;//���ս����ǰ��������ʱ��
	uint32_t						m_iSkyArenaNxtTime;	//���ս����������ʼʱ��

	//ÿ������ɱ�ּ��� 2014/4/11���໪
	mem::map<int,int>				m_mapDaylyTaskKillCount;

	/*int								m_lLastLogicAreaID;
	int								m_lLastMapID;
	int									m_iAreaLogicType;*/

	//����ټ���cd
	int64_t						m_lGuildTransportStamp;

	//���ְ������
	int							m_iTutorialProgress;
	std::vector<uint32_t>       m_TutorialRewardId; 

	//����λ�������
	int                        m_iSkyArenaRankRewardId ; 
	int                        m_iMaxScore ; 

	//�������
	int                        m_leftTimes ; //ʣ�๺����� 
	int                        m_buyTimes ; //������� 
	int                        m_Rate;  //���� 
	std::vector<int32_t>       m_BuyerInfo ; //���������Ϣ  
	StringType                 m_BuyerName; //�����������


	//�۷徺���� 
	int                        m_iStartTime ;//�۷徺��������ʱ�� 
	int                        m_iEndTime ; //�۷徺��������ʱ��
	int                        m_stageType ; //�׶�ö��
	int                        m_showTime ; //չʾʱ��
	int                        m_battleStTime;//ս����ʼʱ��
	int                        m_myRank ;  //���˵�ǰ�����������
	int                        m_zoneId ;   //����Id 
	int                        m_stageSetEndTime ; //��������ʱ��
	int                        m_stageStTime ; //�׶ο�ʼʱ�� 
	int                        m_stageEndTime ; //�׶ν���ʱ��
	int                        m_nTotalGroupId; //������ 
	bool                       m_summitRedPoint;   //�۷���   
	int                        m_myTopType;    //�Ҵ򵽵���߽׶�  
	int                        m_showRemainTime ; //ʣ��չʾʱ�� 
	int                        m_battleRemainTime ; //ʣ��ս��ʱ�� 
	int                        m_stageSetRemainTime ; //ʣ����������ʱ�� 
	int                        m_stageEndRemainTime ; //�׶ν���ʣ��ʱ�� 
	int                        m_invitedType; //���뺯�������� 
	bool                       m_isValid; //���뺯�Ƿ��� 

	typedef mem::map<int,std::vector<SummitArenaGroupStageUserData>>     m_groupDatas   ;  //��ս������
	typedef mem::map<int,std::vector<SummitArenaAgainstList>>  againstDatas ;  
	againstDatas                              againstList;  
	m_groupDatas                              allGroupData ; 
	/*SummitArenaGroupStageData                 myGroupData ;*/  //���Լ����� ����
	std::vector<SummitArenaGroupStageUserData>  myGroupData ; 
	SummitArenaGroupStageData                 EntryList ; //�����б� 
	std::vector<SummitArenaHonorDataMsg>	  TopPlayerList;//��ҫ֮��
	int										  TopErr;//��ҫ֮�۴�����
	
	//���ֽ��ٹ������а�
	std::vector<PS_GuildDamageInfo>			m_nGuildInfo;

	//Ŀ��
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
	bool						m_bMouseOverTargetArgValid;	//�Ƿ�Ҫʹ��m_kMouseOverTargetArg����������Ƶ������š���Чǽ���ϲ���Ҫ����
	MouseOverTargetArg			m_kMouseOverTargetArg;		//MouseOverTargetʱҪ�õ��Ĳ���������������ɫ��������ɫ�ȣ����ⲿ����

	Data::GD_ModelBase*			m_pPreLockedTargetModel;	// ѡ�е�Ŀǰ�л���ɫ��ʱ��id���䣬����visibleData��任
	object_id_type				m_lPreLockedTargetID;
	bool						m_bIsFreezeLock;

	bool						m_bEnablePathFinding;

	bool						m_bNeedLockTarget;
	bool						m_bNeedLockLocation;
	bool						m_bNeedLockTradeTarget;
	int							m_iLockLocationFXID;
	int							m_iLockLocationFxType;
	NiPoint3					m_kMouseOverLocation;

	LC_IDSet					m_kLastAttackerID;			//��������ҵĵ���
	float						m_fAutoSearchEnemyCDTime;	//�Զ�������Χ���˵�cdʱ��
	float						m_fLastSceneMouseOverTime;
	float						m_fLastPickTerrainTime;
	float						m_fLastPrepareMoveStrideTime;
	bool						m_bLastSceneMouseOver;		// ��� m_fLastSceneMouseOverTime
	bool						m_bMoveStride;		//�����������ƶ�
	bool						m_bWASDStride;		// wasd��������ƶ�

	bool						m_bPathtoTreasure;		//����Ѱ·�������־λ

	Utility::UT_Vec3Int			m_kMoveLineTargetLocation;

	//����
	LC_ClientProduction*		m_pkClientProduction;

	//�ر�ͼ����
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

	//�����������
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

	LC_PlayerGroupInviteEntryVector							m_kInvitePlayerGroupEntryVector;	// ��������б�

	typedef mem::vector<int>								LC_PlayerGroupBlockPlayerVector;
	LC_PlayerGroupBlockPlayerVector							m_vecBlockPlayerID;


	//����������ز���
	struct LC_RevealInvisibilityParams
	{
		//��������
		UT_Vec3Int		m_kCircleCenter;	//������������
		int				m_iCircleRadius;	//��������뾶
		bool			m_bRevealOnce;		//ֻ����һ��

		//��������
		int				m_lLastRevealTime;	//�ϴη���ʱ��
		bool			m_bIsFirstReveal;	//�Ƿ��һ�η���
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
			//����Ŀ�����ϵķ�����־
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

	bool						m_bKeyTabPressed;				//��¼tab���Ƿ��Ѿ�����
	object_id_type				m_lSaveLockedTargetID;			//����tab��ʱ�ݴ�������Ŀ��
	object_id_type				m_lSaveWatchTargetID;			//����tab��ʱ�ݴ�������Ŀ��

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

	//��������Ч�߼�
	int								m_bStartTriggerType;
	TickCount						m_bTriggerTypeTick;

	//�ȴ�����ļ���
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
	//������ִ�С�������δ���صļ��ܣ��ͻ�����
	int								m_iReqNoRltExecSkillID;	//requested and not responsed skill
	float							m_fReqExecSkillTime;	//time stamp

	//Ѱ·���
	UT_Vec3Int						m_kSearchPathTargetPos;
	bool							m_bArriveFlag;			//! �Ƿ񵽴�
	float							m_fSearchPathMiniMapX;	// Ѱ·Ŀ���X;
	float							m_fSearchPathMiniMapY;	// Ѱ·Ŀ���Y;

	//Ѱ·���
	//bool							m_bAutoFindPathFlag;	//�Զ�Ѱ·��־
	Path::PF_Path					m_kMovePath;
	float							m_fLastCheckPendingTalkNPCTime;
	PathPendingActionArg			m_kPPAArg;				//Ѱ·Ŀ����Ϊ
	bool							m_bIsPathFinding;		//�Ƿ�����Ѱ·

	PathPendingActionArg			m_kPPAArgForFeiXie;		//��Ь��Ѱ·
	object_id_type					m_kPPAArgRetryTalkID;	//��Ь�Ի�ʧ�ܣ����ܳ��ԶԻ�ID

	PathPendingActionArg			m_kJumpPPAA;		//��Ь��Ѱ·

	int								m_pendingOject;			//Ŀ��obc

	//Ѱ·���
	LC_AcrossMapPath				m_kAcrossMapPath;
	int								m_iPendingPathMapResID;	//Ѱ·��������ͼ
	Utility::UT_Vec3Int				m_kPendingPathPos;		//Ѱ·��������ͼ��Ŀ���
	int								m_kPendingVal;
	int								m_iPendingPathMapLogicID;//Ѱ·��������ͼ��LogicID
	TickCount						m_kDoubleTerrienClearPath;

	int								m_lMoveStraightParam;
	bool							m_lpickMoveFlag;
	//һֱ�ܵ�ĳ����ı��
	bool							m_bMoveStraightFlag;

	//����������ʼ���
	bool							m_bIsCanStartTutorial;
	//��Դ�һ��Ƿ���
	bool							m_bResourceRetrieve;

	float							m_fLastUpdateTime;
	float							m_fLastUpdateTradeStateTime;
	float							m_fLastUpdateProcessingTask;

	//�ȴ����ÿ�ݼ�
	bool							m_bWaitingShortcutKey;
	bool							m_bIsSaveShortcutKey;

	//�Զ����ű�
	Utility::LS_LuaVoidFunction*	m_pkScriptControllerFunctor;

	float							m_fCurrentHookSecond;
	// ��Ϸ�ҽ���ϵͳ
	int								m_lCommissionBuyYuanBaoNum;			//	���ί���չ�Ԫ������
	int								m_lCommissionSellYuanBaoNum;		//	���ί�г���Ԫ������
	int								m_lQuickBuyYuanBaoNum;				//	��ҿ����չ�Ԫ������
	int								m_lQuickSellYuanBaoNum;				//	��ҿ��ٳ���Ԫ������

	int								m_lCommissionBuyYuanBaoPrice;		//	���ί���չ�Ԫ���۸�
	int								m_lCommissionSellYuanBaoPrice;		//	���ί�г���Ԫ���۸�
	int								m_lQuickBuyYuanBaoPrice;			//	��ҿ����չ�Ԫ���۸�
	int								m_lQuickSellYuanBaoPrice;			//	��ҿ��ٳ���Ԫ���۸�

	int								m_lHookForbidCount;

	bool							m_bHooking;
	int								m_lHookTime;
	UT_Vec3Int						m_kHookLocation;
	float							m_fAssistCDTime;			//�Զ����һ�ϵͳ��ʱ��
	UT_Vec3Int						m_kHookGuideLocation;
	bool							m_bStopHookOnTargetDead;	// �ض�Ŀ����������ܹ����󣬽����һ�
	object_id_type					m_iHookingTargetID;			// �һ�ʱ��Ե��ض�Ŀ���
	object_id_type					m_kSpecialHookCharType;		// �һ�ʱ��Ե��ض�Ŀ������

	float							m_bHookGodUsageTime;

	//�����һ�
	LC_ClientRaidHookAsset			m_kRaidHookAsset;
	bool							m_iRaidHookFlag;

	std::map<object_id_type,object_id_type>	m_mapInvalidPickItem;
	Utility::TickCount						m_mapInvalidPickItemTick;

	bool							m_bAutoPicking;

	//��������ߵ��Զ�����ʱ��
	float							m_fLastAttackerTime;

	float							m_fTranscriptionCDTimer1;	//�л������Ժ󲥷ų�����Ч����ʱ
	float							m_fTranscriptionCDTimer2;	//�л������Ժ󲥷ų������ﵹ��ʱ
	bool							m_bTranscriptionFromReturnToCity;	//�������ԡ��سǡ�����

	float							m_fLastReqFriendStatusTime;

	int								m_lResId;   //����ټ����Ŀ���ͼid

	void                _UpdateReqFriendStatus(float fCurrentTime);

	//�����ٻ������б�
	unique_id_type		m_summorLis[MAX_SUNMOR_FIREND_POOL_CAP];
	uint32_t			m_summorFriendCharID;

	bool				m_bForbbidenKey;							//��ǰ�Ƿ�Ϊ��ֹ����״̬

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

	//��������ǩ��
	ResultType				RequestSignInToday(int iType);
	//����ǩ
	ResultType				RequestReSignIn(int iType, int iDay);
	//������ȡǩ������
	ResultType				RequestGetSignInReward(int iType, int iID);
	//��ȡgamestoryid
	int						GetGameStoryIdByMapResID(int iMapResID);
	//�����������Կ�ʼ�б�
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

	//�������Ʒ���
	LC_CommonStatRewardDataEntry* GetStatRewardDataEntry(int iType, int iKey){return LC_CommonStatManager::GetSingletonPtr()->GetCmnRewardDataEntry(iType,iKey);}

	//��ѧ
	LC_VIPFDepositEntry*	GetVIPFDepositDataEntry(int32_t level){return LC_VIPCmnManager::GetSingletonPtr()->GetVIPFDepositDataEntry(level);}
	bool					IsDailyRechargeRewardedToday();
	bool					IsRechargeToday();
	int						GetDailyRechargeMaxNotDrawedLevel();
	LC_ClientOpenAbilityManager* GetOpenAbilityManager();
	
	//����
	object_id_type			GetEyeCharTypeID() {return m_iEyeNPCID;}
	//��Ӹ����ӿ�
	void					SetCrossRealMapTeamID(int iID){m_iCrossRealMapTeamID = iID;}
	void                    SetCrossRealTeamMapID(int MapID){m_iCrossRealTeamMapID = MapID;}
	int                     GetCrossRealTeamMapID(){return m_iCrossRealTeamMapID;}
	bool					IsInCrossRealMapTeam(){return m_iCrossRealMapTeamID > 0;}

	//����ֵ
	virtual void			SetVigor(int Value) { m_Vigor = Value; }  
	virtual void			SetAlreadyTransfer(bool kFlag)	{ m_bAlreadyReqTransfer = kFlag;  m_kReqTransferInterval.ResetTimer();}
	bool					GetAlreadyTransfer() { return m_bAlreadyReqTransfer; }

	virtual void			GetSubEquipSlotMoldingSuitPercentAttr(int subType, LC_ActorAttributeMap* attrMap);
	virtual void			GetSubEquipSlotMoldingAttr(int subType, LC_ActorAttributeMap* attrMap);
	virtual void			GetSubEquipSlotMoldingSuitValueAttr(int subType, LC_ActorAttributeMap* attrMap);

	int						GetSubEquipSuitMoldingAttribValue(int iSubType, int iAttribType);
	int						GetSubEquipSuitMoldingAttribPercent(int iSubType, int iAttribType);
	int						GetSubEquipMoldingAttribValue(int iSubType, int iAttribType);

	//װ������
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

	//dailycleandata���
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

	//��ҵ�һ�ν�����Ϸ�����ڲ��ֺ�����Ҫ������ԣ������ڽ�����Ϸ���һ��SyncPlayerProperty�����
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

	//����ս���
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
	int					GetBestGuildFinishedTaskNum();			//��������˳ɾ�+��������������Ƿ���������ͬ��������
	StringType			GetGuildBattleTaskInfo(int iTabIdx,int iSubIdx);
	StringType			GetGuildBattleTaskTabNames();
	bool				IsGuildBattleTaskRewardToken(){return m_BestGuildRewardFlag;};
	bool				IsGuildBattleTaskPrompt(int iTabIdx,int iSubTabIdx);

	bool				IsTasksCanFinish(Utility::UT_SIMDataList& InTasks);
	bool				IsAchievementCanFinish(Utility::UT_SIMDataList& InAchievements);

	void				GetOccupiedTowers(mem::vector<int>& OutTowerIds);					//�����ռ������


	//����
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
	void				ReqThemeACTADAutoShow();//�߻�Ҫ�󣬻�ڼ��һ�ε�¼��Ҫ���������棬����ͻ���������Ϊ�˿���֪ͨ��ʱ����
	void				ReqThemeACTRaidBossPrompt(int iThemeMainId);
	//��Ȩ�ȼ�+������ռ������ �ٷֱȼӳ�
	int					GetTotalVipNewBonusPercentIncrease();	
	//GetVipPriviligeAddTime
	int					GetVipPriviligeAddTimesByType(int iType);	
	//�׳佱��ϵͳ
	//void					_initFisrstChargeData(void);

	void				ReqRaidBossPublicKillerInfo(int iCharType, int iMapId);

	//����������Ȩ˫����ȡ
	void				ReqTakeTaskExtraReward();
	void				UpdateTaskExtraReward(SyncType InType,SyncOP InOP,std::vector<KeyValue_Base>& InVec);
	void				ClearTaskExtraReward();
	StringType			GetTaskExtraReward();


	//����ϵͳ
	LC_ClientSoulRefineAsset* GetClientSoulRefineAsset(){return &m_kSoulRefineAsset;}
	//�°汾�
	LC_ClientActivityAsset* GetClientActivityAsset(){return &m_kActivityAsset;}

	//������ʹ��״̬
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

	//�������
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

	//��о
	void				SyncActiveStarVeinIds(const MG_SyncActivateStarVein& msg);
	void				SyncActiveStarVeinGoalIds(const MG_SyncStarVeinGoalRecord& msg);
	StringType			GetActiveStarVeinIds();
	StringType			GetActiveStarVeinGoalIds();
	attr_value_type		GetStarVeinScore();  
	void				RequestActiveStarVein(int iID);
	void				RequestGetStarVeinGoalReward(int iID);

	//ÿ�ջ�Ծ�����
	void                RltSyncGamePlayerDailyActivity(const MG_SyncDailyActivity& rltMsg);
	int32_t				GetDailyActivity(){ return m_nDailyActivity; };
	StringType			GetDailyActivityString();
	void				RequestGetDailyActivity(int32_t rewardID); // ������ȡ��������
		
	//��ɫ�ɳ�������
	void				RltSyncCharacterUpgradGiftData(const MG_SyncBuddyGrowthPackage& rltMsg);
	bool				IsBuddyActiveUpgradGift(int cID);
	bool				IsBuddyUpgradGiftOver(int cID);
	int32_t			GetBuddyUpgradGiftCurrentDay(int cID);
	int32_t			GetBuddyUpgradGiftDay(int cID);
	bool             ReqGetBuddyUpgradGift(int cID);
	bool				ReqActiveBuddyUpgradGift(int cID);

	//�׳佱���������
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

	//�׳�
	void				ReqTakeFirstChargeReward(int iDay);
	void				UpdateFirstChargeTime(int iChargeTime);
	void				UpdateFirstChargeRewardRecord(const std::map<int,bool>& Record);
	void				UpdateFirstChargeRewardRecord(int iTokenRecord);
	int					GetFirstChargeTime();
	bool				IsFirstChargeRewardToken(int iDay);

	//Ѱ���ۼ�
	StringType			GetDrawCumulateReward();
	void				UpdateDrawCardCumulateReward(const StringType& InRewardStr);
	//vip���ٹһ�
	void				ReqVipQuickHook();
	void				UpdateVipQuickHookTime(int iTimes);
	void				UpdateVipQuickHookReward(std::vector<PS_ItemRecord>& RwdInfo);
	StringType			GetVipQuickHookReward();
	int					GetVipQuickHookTime();

	//�³ɾ�ϵͳ���
	void			SetNewAchievementStarNum(int inum) {m_iNewAchievementStarNum = inum;};
	int				GetNewAchievementStarNum() {return m_iNewAchievementStarNum;};
	void			SetNewAchievementRewardCord(const std::vector<int32_t>& m_nID);
	StringType		GetNewAchievementRewardCord();
	void			ReqNewAchievementReward(int id);

	//�����������
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

	//����boss���
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

	//���������
	int				GetTotalReforeLevel();
	void				SyncACTSubInfoEnd(const MG_SyncPlayerACTSubInfo& syncMsg);
	int64_t			GetACTSubInfoByRankType(int32_t rankType);

	//
	void				SyncGamePlayerStarMoney(const MG_SyncPlayerPentacle& rltMsg);

	//���ǽ������
	void				ReqBiographyReward(int32_t chapterID);
	void				RltBiographyReward(const MG_RLT_BiographyPartReward& rltMsg);
	void				SyncBiographyChapters(const MG_SyncBiographyPartReward& syncMsg);
	void				SyncBiography(const MG_SyncBiography& syncMsg);
	bool				IsChapterOver(int32_t chapterID);

	//��ʱ������
	void				SyncTimeLimitShop(const MG_SyncPlayerTimeLimitedActivity& syncMsg);
	void				RltTimeLimitShop(const MG_SyncTimeLimitedActivityState& rltMsg);
	StringType	GetActiveTimeShopInfo();

	//�ղ����
	void				RltSyncGamePlayerCollection(const MG_SyncCollectionInfo& rltMsg);
	void				ReqUpgradCollection(int iID);
	void				ReqUpgradCollectionSuit(int iID);
	void				RltUpgradCollection(const MG_RLT_CollectionActivation& rltMsg);
	void				RltUpgradCollectionSuit(const MG_RLT_CollectionReward& rltMsg);

	//�һ����������
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


	//ָ�ӹ�ͷ��
	void				SetCommanderTitleLevel(int iLevel);
	int					GetCommanderTitleLevel();
	bool				GetCommanderTitleOpen(); 
	void				CheckCommanderTitleOpen(); 
	StringType			GetCommanderTitleCurInfo();

	bool				GetProcessPathFlag() { return m_processPathFlag; }
	void				SetProcessPathFlag(bool bFlag) { m_processPathFlag = bFlag; }

	int					ReqTakeCommanderTitleUpgradeReward();//��ȡ��������

	//��ѵ
	int					ReqTakeSpecialTrainingReward();//��ȡ��������
	/**/
	void				UpdateSpecialTrainingLevel(int InLevel);
	/**/
	StringType			GetSpecialTrainingTaskInfo(int iLevel);
	int					GetSpecialTrainingLevel();
	int					GetSpecialTrainingMaxLevel();

	void				GetTaskInfoString(Utility::UT_SIMDataList& InTasks,StringType& OutStr);
	void				GetAchievementInfoString(Utility::UT_SIMDataList& InAchievements,StringType& OutStr);

	/*
	OutStr:δ���
	OutStr2:���콱
	OutStr3:���δ�콱
	*/
	void				GetTaskInfoStringSortByFinished(Utility::UT_SIMDataList& InTasks,StringType& OutStr,StringType& OutStr2,StringType& OutStr3);
	void				GetAchievementInfoStringSortByFinished(Utility::UT_SIMDataList& InAchievements,StringType& OutStr,StringType& OutStr2,StringType& OutStr3);
	void				RewardMapToRewardString(mem::map<int,int>& RwdMap,StringType& RwdStr);
	// �ֽ�װ��
	void				ReqDeCompositionEquips(StringType equipIndex);
	void				RltDeCompositionEquips(const MG_RLT_DecompositionItem &rltMsg );

	//�ֽ�װ���Ƚ�����
	void                ReqDecompositionSetting(bool bCommander, int iClevel, int iCgrade, bool bVehicle, int iVlevel, int iVgrade, bool bDevaeye, int iDlevel, int iDgrade, bool bWing, int iWlevel, int iWgrade);
	void				RltDecompositionSetting(const MG_RLT_DecompositionSetting &rltMsg);


	//�ٹ�Զ��
	void				ReqQuickRaidNew(int iMapId,int iNum);							//ɨ��
	void				ReqGetChapterRaidStarReward(int iChapterId,int iStarIndex);		//��ȡ�½ڽ���
	/**/
	void				UpdateRaidGloryTotalStarNum(int iNum);
	void				UpdateRaidGloryChapterStarNum(int iChapterId,int iNum);
	void				UpdateGloryChapterRewardRecord(int iChapterId,std::vector<int>& InRecord);
	void				UpdateQuickRaidGloryCount(std::map<int,int>& InRecord);//ɨ������
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
	int					GetQuickRaidGloryCount(int iMissionId);//ɨ������
	bool				HasNotTakeReward(int iBeginChapter,int iEndChapter);

	//�ż�ͻϮѰ��
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


	//�������԰�
	void				ReqGetSubValue();


	//����ת��
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

	//��Ʒ����
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
	
	//Boss��ɱ���
	void				RltSyncGamePlayerBossFirstKill(const MG_SyncBossFirstKillInfo& rltMsg);
	StringType			GetBossFirstKillInfo(int32_t charTypeID);
	void				SyncFirstKillBossReward(const MG_SyncKillBossReward& msg);
	bool				IsBossFirstKillRecorded(int charTypeID);
	bool				IsBossKilledBySelf(int charTypeID);	//�����Ƿ��ѻ�ɱboss
	void				RltSyncServerBossFirstKillRecord(const MG_SyncServerFirstKillBossReward& rltMsg);
	bool				IsServerBossFirstKillRecorded(int32_t charTypeID);
	StringType			GetBossFirstKillRewardPlayerInfo(int32_t charTypeID);
	void				RequestDrawFirstKillReward(int iCharType, bool bServer);
	void				RequestAutoDecompositionItem(bool bauto);

	//����ͷ��ͷ������
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
	//���������
	void				RltSyncGamePlayerCompeteReward(const MG_SyncACTActiveRewardInfo& rltMsg);
	int32_t				GetCompeteRewardCount(int32_t iID);
	//�������԰����
	void				RltSyncGamePlayerSubReward(const MG_SyncACTActiveRewardInfoB& rltMsg);
	int32_t				GetSubRewardCount(int32_t iID);

	//�鿨���
	void				RltSyncGamePlayerDrawCardReward(const MG_SyncLotteryItem& rltMsg);

	//�۷�ս�����
	void				ReqPeakArenaAchieveInfo();
	void				ReqPeakArenaLadderInfo(int32_t index,int32_t size);
	void				ReqDrawArenaSectionReward(int iCode,int32_t iID);

	//����������
	void                ReqBonFirePartyLeftBuyItemTimes(); 
	void                ReqBonFirePartyBuyInfo(int buyTimes);  

	//���ջ 
	StringType          GetHolidayActivityTasks(int iHolidayActivityTaskType,int iTabIdx); 
	bool                IsHolidayActivityTasksPrompt(int iHolidayActivityTaskType,int iTabIdx);
	void                AcceptHolidayTasks(int iHolidayActivityTaskType); 
	void                AcceptUnFinishedTasks(Utility::UT_SIMDataList& InTasks);


	//ί���������
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
	//���ؼ��ܽӿ�
	void					_toggleSkill(int lSkillID);
	//����/�������
	void					_updateComboCount(int iOldValue, int iAddCount);
	void					_clearComboCount();
	void					_onSkillFlagChanged(int64_t llOldAllFlag, int64_t llFlag);
	void					_initMuhunInfo(void);
	//�Զ������Ƿ�Ҫ��������
	bool					_shouldAutoUpgradeContinue(int iType, int iLevel, ResultType result);

	void					_updateExtraNodeData();

	//������������
	void					_initProductionData(void);
	//����CD������Ϣ
	void					_checkChatMsgSendFrequency(int iChannelType, float fCurrentTime);

	//��оdirtyflag
	LC_DirtyFlag			m_DFStarVein;
	LC_DirtyFlag			m_DFStarVeinGoal; 


/************************************************************************/
/*                                                                      */
/************************************************************************/
//����ɫϵͳ BEGIN
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

	//������ɫ
	void							ReqUnlockBuddy(int cID);

	//�л���ɫ && ��������
	bool							ReqActivateActiveBuddy(int activeIndex);

	void							SwapBuddyData(int activeIndex);
	void							SwapBaseClassData(LC_PlayerBase* pSrc, LC_PlayerBase* pDst);
	void							SwapOtherBaseData(LC_PlayerBase* srcPlayer, LC_PlayerBase* dstPlayer);
	void							SwapTitles(LC_PlayerBase* dstPlayer);
	void							ChangeID(object_id_type iNewObjectID);

	void							AttachEffects(LC_PlayerBase* srcPlayer, LC_PlayerBase* dstPlayer); //��ʾ�ŵ׹�Ȧ����ӳ�ս��Ч
	
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

	//������
	bool							ReqRespawnBuddy(int index);
	bool							IsBuddyDead(int index);
	bool							IsAllBuddyDead();
	virtual bool					Respawn(attr_value_type lHP,int lMP);
	
	//���þ�ͷ
	void							ResetCamera();

	//��ս��ɫ/�����ɫ
	bool							ReqActivateInactiveBuddy(int activeIndex, int buddyID);
	bool							IsBuddyInstance(int cID);
	void							UpdateBuddyInstanceList();
	int								GetUnlockSlotNum() { return m_iUnlockSlotNum; }
	void							SetUnlockSlotNum(int num) { m_iUnlockSlotNum = num; }
	bool							GetUnlockSlotInit() { return m_iUnlockSlotInit == 1; }
	void							AccumulateUnlockSlotInit() { ++m_iUnlockSlotInit; }
	bool							ReqDeactivateBuddy(int activeIndex);

	//����ɫ����
	bool							IsBuddy(uint32_t id);
	bool							IsNpcBelongToBuddy(LC_ActorBase* npcPtr);
	LC_PlayerBase*					GetBuddyPlayerBasePtr(object_id_type id);		//��ý�ɫ��PlayerBase pointer
	int								GetBuddyObjectID(int cID);						//��ý�ɫ��object ID;
	LC_ClientBuddyInstance*			GetBuddyInstance(int cID);
	LC_ClientBuddyInstance*			CreateNewBuddyInstance(int cID);
	bool							GetActivateBuddyFlag() const { return m_bActivateBuddyFlag; }
	void							SetActivateBuddyFlag(bool flag) { m_bActivateBuddyFlag = flag; }
	void							AttachEffectToActivateBuddy(int oID);
	int								GetActivateBuddyID(MG_SC_ActivateBuddyDataSync& BuddyDataMsg);		//��ȡ��ս��ɫ��CID
	int								GetSkillUnlockLevel(int cID, skill_id_type skillID);				//�鿴ĳ�������Ƿ�ѧ���ˣ�����з���-1��û�з���ѧϰ����ȼ�
	void							SetPendingActivateCID(int cID) { m_iPendingActivateCID = cID; }
	int								GetPendingActivateCID() { return m_iPendingActivateCID; }
	void							ClearEyeAvatar();
	activateBuddyList				GetActivateBuddyList() { return m_vActiveBuddyList; }
	int								GetBuddyIndex(object_id_type objID);

	//�������
	void							SetSchemeType(int iType) { m_iSchemeType = iType; }
	int								GetSchemeType() { return m_iSchemeType; }
	//void							SyncSchemeData(const SingleBuddyActivateSchemeData& syncInfo);
	//void							SyncAllSchemeData(const MG_SC_MultiBuddyActivateSchemeData& syncInfo);
	int								GetSchemeCid(int iElementType, int iSlotIdx);
	void							RequestSchemeModify(int iElementType, int iSlotIdx, int iCid);
	void							RequestSchemeSwitch(int iElementType);
	void							RequestSetRankTransferState(int mapID);

	
	//����UI
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

	//BuddyInstance���
	LC_SkillAsset*					GetBuddySkillAsset(int cID);
	LC_ActorAttributeMap*			GetBuddyAttributeMap(int cID);
	LC_ActorAttributeAppendMap*		GetBuddyAttributeAppendMap(int cID);
	LC_ShortcutBar*					GetBuddyShortcutBar(int cID);
	bool							CheckBuddyActivateSkill(int cID, int skillID);
	bool                            CheckBuddyActivateSkillAll(int cID);

	int								GetBuddySkillUsableCount(int cID, int skillID);
	int								GetBuddySkillMaxUsableCount(int cID, int skillID);

	void							SyncBuddyLevel(int cID);									//ͬ����ɫ�ĵȼ�
	void							SyncBuddyEXP(int cID);										//ͬ����ɫ�ľ���
	void							SyncBuddyCombate(int cID);									//ͬ����ɫ��ս����
	void							SetAllOwner(int cID, LC_PlayerBase* pkPlayer);
	void                            SendBuddyExpAndLevelChange(int oldExp, int oldLevel, int newExp, int newLevel, int cID);
	combat_score_type				GetAddEquipScore(LC_BackPackEntry* pkEntry);
	
	//��ɫ����
	bool							ReqUpgradeBuddyStar(int cID);
	int								GetBuddyStarLevel(int cID);
	long							GetBuddyUpgradeStarCost(int cID, bool bLast = true/*�Ƿ�ȡǰһ��������*/);
	int								GetBuddyUpgradeItemID(int cID, bool bLast/*�Ƿ�ȡǰһ��������*/);
	int								GetBuddyUpgradeQuality(int cID, int iStarLevel);
	long                            GetBuddyUpgradeUnlockskill(int cID, int iStarLevel);

	//��VIP ������lua
	bool							ReqTakeVipNewReward(int iLevel);
	/**/
	void							UpdateVipNewRewardRecord(const std::vector<int32_t>& vecTokenLevels);
	void							UpdateVipNewRewardRecord(int32_t iNewRewardTokenLevel);
	bool							UpdateVipNewLevel(int32_t iLevel);//�����Ƿ�����������ʼ����ʱ��Level��999��999->0��������
	void							UpdateVipNewOpen(bool bValue);
	void							ReqAcceptVipNewLevelTasks(int32_t iLevel);
	/**/
	bool							HasNottakenVipNewReward();
	bool							IsVipNewRewardToken(int iLevel);
	bool							IsVipNewOpen();
	int								GetVipNewTaskState(int iTaskID);
	int								GetVipNewLevel();

	//����ͼ�����Ϣ
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

	//���ͼ��������
	void							ReqMapNpcCount(int storyID, int mapID);
	void							SetMapNpcInfo(MG_SyncMapNpcInfo& msg);
	int								GetNpcCountByMapID(int mapID, int npcID);

	//�������
	bool							IsTaskFinished(int iTaskId);
	bool							IsTaskCanFinish(int iTaskId);
	
	//��Ҽ����
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

	//��װ��Э�����
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

	//���ʻ���
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
	//buddyװ������Ϣ
	int								GetEquipSlotLevel(int icID, int iSlotID);
	int								GetEquipSlotStar(int icID, int iSlotID);

	void							RequestFireTimer(int iTimerID);

	void							SetCommanderLotteryCount(int32_t iCount) { m_pkCommander->SetLotteryCount(iCount); }
	int32_t							GetCommanderLotteryCount() { return m_pkCommander->GetLotteryCount(); }
	int32_t							GetCommanderMaxRuneStarLevel() { return m_pkCommander->GetMaxRuneStarLevel(); }
	int32_t							GetCommanderMaxRuneLevel() { return m_pkCommander->GetMaxRuneLevel(); }

	//GamePlayer������ (��BuddyInstance��������)
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

	//���
	void							ReqSetRedPointFlag(int iType, bool bFlag);
	void							UpdateRedPointFlag(int iType,std::vector<KeyValue_Base_Bool>& InVec);
	bool							GetRedPointFlag(int iType);

	HookingJump&					GetHookingJump() {return m_HookingJump;}

	void							SetIsInParalle(bool b) { m_bInParalle = b; }
	bool							GetIsInParalle() { return m_bInParalle; }

	//���ְ���: 
	void                            ReqAddNoviceTutorial(int noviceId) ; 
	void                            ReqGetNoviceTutorialReward(int noviceId) ;
	void                            SyncNoviceTutorialInfo(const std::vector<uint32_t> &m_vGetRewardNoviceTutorial,const std::vector<uint32_t> &m_vNoviceTutorialRecord);

   //��񷽱��: 
	void                            ReqSkyArenaGradeAward(int m_nGrade) ; 
	void                            ReqSkyArenaMyScore() ; 
	void							ReqSkyArenaCheckBattleStage(unique_id_type uid, int iMapResID, int iSchemeType);
	void                            AcceptSpecialTask(int specialTaskId); 

	//�۷徺���� 
	void                           ReqSummitArenaInfo() ; //������Ϣ
	void                           ReqSummitArenaGroupStageEntryList(); //�����б�
	void                           ReqSummitArenaGroupStage() ; //С��ս�� 
    void                           ReqSummitArenaMyGroupStage(int mapId,int schemeType) ; //����ս��
	void                           ReqSummitArenaAgainstList(int zoneId,int groupId,int type) ;  //64ǿ
	void                           GetSummitArenaTime(SummitArenaTimeData& timeData) ; //��ʣ��ʱ�� 
	void                           GetSummitArenaTimeStamp(SummitArenaTimeData& timeData) ; //����ʱ��� 

	
	
//***��ɫ���� BEGIN
	virtual bool					IsSpecking();
	virtual void					UpdateVoiceLocation();

	void							PlaySuccessVoice();
	void							PlayFailVoice();

	float							m_fAutoPathSoundStartTime;
	const buddyInstanceMap&			GetBuddyInstanceMap(){return m_mBuddyInstanceMap;}

//***��ɫ���� END

//��������
	void							SetMapFullSchemeData(MG_SyncBuddyActivateScheme& msg);
	void							SetMapFullSchemeData(MG_RESP_SetBuddyActivateScheme& msg);
	void							SetMapFullSchemeData(MG_RESP_SetSpecialMapInitBuddyActivateScheme& msg);
	void							ReqSetBuddyActivateScheme(int MapGroupID, int SchemeType, StringType& str);
	int								GetBuddySchemeCidByIndex(int MapGroupID, int SchemeType, int GroupID, int Index);
	void							ReqSetSpecialMapInitBuddyActivateScheme(int MapID, int SchemeType);
//�ƽ�����


//���鿴����
	void							ReqGetPlayerSchemeData(int32_t SystemType, int m_nMapID,int m_nSchemeID,int m_nSchemeType, uint64_t m_nCharID);
	void							ReqGetRankInfo(uint32_t m_nRankID,uint64_t m_nCharID,int m_nZoneId);

//���ս������

	void							SetSkyArenaSettleReward(std::vector<PS_ItemRecord>& RwdInfo);
	StringType						GetSkyArenaSettleReward();	
	void							SetSkyArenaCommanderBaseData(MG_NOTIFY_SkyAllShowInfo& msg);
	void							SetSkyArenaSettleReportList(std::vector<BaseBattlefieldReport>& info);
	LC_ClientBattleRecordDataBase*	GetSkyArenaSettleReportList() { return &m_SkyArenaSettle; };


//ս����¼
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

//�����ͼ����
	void								SetMapEnterRecord(int iMapResID, int iCount);
	int									GetMapEnterRecord(int iMapResID);
	void								ReqGamestoryBatch(int iMapResID);

	void								SyncCanEnterMapIDSet(const std::vector<int32_t>& vec);
	void								AddCanEnterMapID(int iMapResID);
	void								ReqAddCanEnterMapID(int iMapResID);

//����
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
	LC_ClientGamePlayerCommander*				m_pkCommander;					//���Commander

	activateBuddyList							m_vActiveBuddyList;				//��ս��
	inactiveBuddyList							m_vInactiveBuddyList;			//����ս��
	buddyInstanceMap							m_mBuddyInstanceMap;			//BuddyInstance��
	
	typedef mem::map<int, mem::vector<int>> BuddySchemeInfoMap;
	BuddySchemeInfoMap m_mSchemeInfoMap;
	
	mem::map<uint32_t, SingleCollectSuit>		m_mCommanderEquipSuitCollection;//��װ�ռ���Ϣ

	mem::map<int, int>							m_mResourceRetrieveRecord;		//��Դ�һؼ�¼
	mem::map<int, int>							m_mResourceRetrieveMapRecord;	//���� ��ͼͨ�ؼ�¼
	int											m_iResourceRetrieveLevelRecord;	//���� ָ�ӹٵȼ�

	uint16_t									m_uiCurrentActiveBuddyIndex;	//��ǰ��սIndex
	int											m_iUnlockSlotNum;				//�ѽ����ĳ�ս����
	int											m_iUnlockSlotInit;				//�ǲ��Ǹյ�½��ͬ�������������������UI��Ҫ���������ӵĲ���

	bool										m_bActivateBuddyFlag;			//��������Ƿ���һ��buddybasicdataΪ��ս��ɫ
	int											m_iPendingActivateCID;			//��Ϊ����������֪ͨ��������Ҫ�ͻ����Լ�����Ŀ���½�ɫ��CID
	bool										m_bEnterGameFlag;				//����ʱΪtrue,������ʾ�л����ԭGPģ��
	int											m_iSchemeType;

	//��λ����flag
	bool										m_bInParalle;

	//�ٹ�Զ��
	int											m_iRaidGloryTotalStarNum;
	mem::vector<int>							m_iRaidGloryChapterStarNum;
	std::vector<bool>							m_iRaidGloryRewardRecord;
	std::map<int,int>							m_nQuickRaidGloryCountMap;	
	int											m_iGloryDailRewardDrawed;
	int											m_iGloryPointNextRecoverTime;	//�´λָ�ʱ��
	int											m_iGloryPointEndRecoverTime;	//ȫ���ָ�ʱ��

	//��������
	mem::map<uint32_t, std::vector<PS_SingleSchemeData>> m_vMapOffensiveSchemeData;					//��������
	mem::map<uint32_t, std::vector<PS_SingleSchemeData>> m_vMapDefenceSchemeData;        			//��������

	//ս����¼
	int															m_iBattleRecordNum;
	uint32_t													m_iBattleType;
	mem::vector<LC_ClientBattleRecordDataBase>					m_BattleLogList;
	mem::map<uint64_t, LC_ClientBattleRecordDataBase>			m_PeakBattleInfo;

	//����
	LC_ClientGuessDataBase						m_GuessData;			//��ǰ����
	std::vector<LC_ClientGuessDataBase> 		m_nSummitGuessLog;		//���¼�¼
	uint32_t									m_nGroupID;				//��ǰ�ľ�����

	typedef mem::map<int32_t,EntrustTask_Base> AcceptTaskVec;
	//ί���������
	AcceptTaskVec								m_AccpetedTasks;                					//�Ѿ����ܵ����� <iEntityId,data>
	std::vector<int32_t>						m_CanAcceptTasks;                        			//���Խ��ܵ�����
	int32_t										m_iEntrustRefreshNum;                          		//��ˢ�´���
	mem::set<int32_t>							m_iEntrustedRunes;                          		//�ѱ���ǲ�ı���

	//�ż�ͻϮѰ��
	std::vector<bool>							m_nYijiTuxiRewardIDRecord;								//
	std::vector<bool>							m_nYijiXunbaoRewardIDRecord;							//
	std::vector<bool>							m_nToyIslandRewardIDRecord;
	//����ת��
	int											m_iRotaryRewardIdx;
	int											m_iRotaryRewardValidTimes;			//�齱����
	bool										m_bChargedToday;
	mem::vector<int>							m_RotaryRewardRecord;
	int											m_iRotaryEndTime;					//�����ʱ��
	bool										m_bSkipAnimation;					//�Ƿ���������
	//��Ʒ����
	//int											m_nNonpareilBuyTime;				//�������ʱ��
	//int											m_nNonpareilReturnTime;				//�콱����ʱ��
	//mem::map<int, int32_t>						m_nNonpareilRecord;					//�����¼
	//mem::vector<int>							    m_nNonpareilReturnRecord;			//���콱��¼
	mem::map<int,bool>                              m_nBuyNoPayList ; 

	//�׳�
	int											m_FirstChargeTime;
	std::map<int,bool>							m_FirstChargeRewardRecord;
	//�л���ʱ�ṹ��
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

	SwapTimeCount								m_vSwapTimeCounts[MAX_ACTIVE_BUDDY_NUM]; //��ʱ��ÿ��updateˢ��
	int											m_iPendingSwapIndex;

	SwapTimeCount								m_vActiveBuddyRespawnTimeCounts[MAX_ACTIVE_BUDDY_NUM];		//��ս����CD��

	//�ո���
	HookingJump									m_HookingJump;

	//֪ͨUI�õ�Dirty_Flag
	LC_DirtyFlag								m_DFBuddyActiveList;
	LC_DirtyFlag								m_DFBuddyInActiveList;
	LC_DirtyFlag								m_DFCurrentIndex;
	LC_DirtyFlag								m_DFCurrentSchemeList;
	LC_DirtyFlag								m_DFCurMapPlayerInfo;
	LC_DirtyFlag								m_DFRaidGloryInfo;
	LC_DirtyFlag								m_DFNPCFunction;
//***����ɫϵͳ End


//***��λ���� BEGIN
public:
	//��λ����
	void RemoveAllBullets();
	void ClearAllSkillSound();
	void ChangePhaseMap(int iMapLogicID, int lMapResID, int lMapLogicType);
//***��λ���� END
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
	uint16_t			m_auiUIOperationRecords[64];	//��¼�����UI����

	int					m_iNewPlayerLevel;

	bool				m_bPendingToSetVisibleDataEnableFakeShadow;

	//pvpս��״̬�²�����Ļ������Ч��id
	int					m_iPVPGfxID;
	UT_Vec3Int			m_kNpcTalkUIPos;

	int					m_NotifyInGame;

	//վ������һ�����ʱ
	float				m_fStandToHookTimer;

	//AFK���������ʱ
	bool				m_bEnableAfkToMeditation;
	bool				m_bEnableStandToHook;
	//վ������ȫ�Զ�����ʱ
	bool				m_bEnableToGuding;
	bool				m_bDoingOtherTask;	//�Ƿ����ڽ��з��������񣬽�����������ʱ�����Զ�����
	float				m_fGudingTimer;
	int					m_iAutoTaskType;	//��ǰ�Զ���������
	bool				m_bFinishTaskFlag;

	//�һ�����ȫ�Զ�����ʱ
	float				m_fHooking2GudingTimer;
	bool				m_bEnableHookingToGuding;
	bool				m_bTutorialStop;

	//�һ���Ч����ʱ���ж�
	float				m_fInvalidTime;
	UT_Vec3Int			m_kInvalidPos;
	object_id_type		m_fLastInvalidTarget;

	//��������
	object_id_type		m_lastVehicleCharType;		//��һ�������CharType
	////��������
	//int					m_lastWeaponItemID;		//����itemid������������������ս����
	////����
	//int					m_lastBowLevel;
	////����
	//int					m_lastEyeCharType;
	//int					m_iEyeNPCID;
	////����
	//object_id_type		m_lastWingCharType;
	////�����ȼ�		
	//object_id_type		m_lastJCCharType;
	//int					m_lastJCLevel;
	//object_id_type		m_lastJCSetLevel;
	//int					m_idleTime;
	////���
	//int					m_lastLCCharType;
	//object_id_type		m_lastLCID;
	////����
	//int					m_lastLCWeaponType;
	////�����ȼ�		
	//object_id_type		m_lastAnQiCharType;
	//object_id_type		m_lastAnQiID;
	//int					m_vehicleZFix;

	//������Ч����
	int					m_iComboEffectData;
	//����ս�������Ĺ���ID
	int					m_iPowerGuildId;
	//����ս��ǰʥ�����������ID
	int					m_iGuildWarType;
	//���������ʱ��
	float				m_fRespawnTimer;
	int					m_eRespawnFadeState;

	//��ת��ͼ�󣬼����Զ�Ѱ·�����
	int					m_iTransferTargetMapResID;
	UT_Vec3Int			m_kTransferTargetPoint;

	bool						m_bFirstEnterGame;

	std::vector<int>			m_vAtvChargePoint;

	//�Ƿ��������ÿ�ո���
	bool						m_bGuildRewardIsVaild;
	//���ÿ�ջ�Ծ��
	int32_t						m_nDailyActivity;
	mem::vector<int32_t>        m_nDailyActivityRewardID;
	
	//����
	mem::vector<int32_t>  m_nBiographyChapters;
	
	//�׳����ս���
	int32_t						m_dayFlag;
	int							m_dayTime;
	
	//����״̬�½����۷�ս����������ʱ��
	int							m_lSaveDeadTime;
	//��Ӹ�����������
	int				m_iCrossRealMapTeamID;
	int             m_iCrossRealTeamMapID; // -1 �ޣ������mapID
	LC_ClientArenaBattleChallengeInfoAsset	m_kArenaBattleChallengeInfoAsset;
	LC_PlayerDetailManager m_kPlayerDetailInfoData;	//��ѯ������������ݹ�������

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

	//�����������
	std::map<int32_t,PS_Index_Entry> m_RankData;
	
	int									m_currentSpecialEquipt[ET_MAX_COUNT];		//��ǰ����ʱװ����
	
	//tab�������ܼ�¼��־
	mem::map<object_id_type, bool> m_flagVec;

	//������������֮���ʱ��������س�ʱ�����������ͼ��ܣ�������������λذ�
	Utility::TickCount	m_skillRltDelayTimer;

	//��ҪĿ����ʾ���
	float m_needTargetMessgeTick;
	//����CD��ʾ���
	float m_fSkillCdMessageTick;


	//��Ϊ���ڼ����пͻ��˵ļ�CD��Ϊ�˿ͻ��˱���CD�������չ����л��߼�
	bool m_normalContinueCheck;

	bool m_bAlreadyReqTransfer;
	TickCount m_kReqTransferInterval;

	GameOptCache m_kGameOptCache;

	//BUG #15632 �����ܡ�������ϵͳ���ճ�����Ͱ����������˫����ȡ����
	//Ҫ���ճ�����Ͱ���������ʱ�����������Ӧ���������
	//����ͨ���洢�����״̬(��PROCESSING �� CANFINISH��ʱ����һ��ʱ��)
	mem::map<int,int> m_kTaskState;

	bool	m_bImmediateTaskStart;
	bool	m_hookRespawnFlag;
	bool	m_hookRespawnPathFlag;		//�һ������Ѱ·��־
	bool	m_hookRespawnDeadFlag;		//�һ������Ѱ·������������־
	bool	m_bhookRespawnBuyFlag;		//�һ�ԭ�ظ��������־

	PlatformMap	m_platforminfo; //ƽ̨��¼��һЩ�����Ϣ

	//< online time
	unsigned int m_enterGameTimeStamp;	//< ������Ϸ��ʱ��

	//bool	m_bInitAttribute;
	ACT_SecretTreasureAsset			m_kSecretTreasureAsset;
	//dailycleandata
	//������˵������ÿ����յ�������ݶ�����������д���
	//��Ҫ�����ν��жԽӵ���ع���
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

	//����Ƥ������� �Զ�ʰȡ����
	bool			m_bCimeliaAutoPicking;

	bool			m_bIntelligentTask;

	bool			m_bPostEnterGameFlag;

	//�ǻ걳���Ƿ��� ��飬������
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

	//���������Ϣ
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

	//��ɫ�ɳ����
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

	//�������а����
	mem::map<int32_t, int32_t> m_kSuperShopCompete;
	//�����������а����
	mem::map<int32_t, int32_t> m_kSuperShopSub;
	
	//�������ϵͳ
	LC_ClientSoulRefineAsset	m_kSoulRefineAsset;
	
	//��һ
	LC_ClientActivityAsset	m_kActivityAsset;
	
	//���ʻ���
	int						m_iEscortTimes;
	int						m_iRobbingTimes;
	int						m_iEscortLevel;
	int						m_iEscortState;//1���ڻ��� 2�ܵ����� 3ֹͣ����

	mem::map<int, PS_GoalInfo> m_mBestGuildGoalInfo;
	
	//Boss��ɱ�
	typedef mem::map<int32_t, mem::vector<PS_BossFirstKillPlayerInfo>> FirstKillRewardPlayerMap;
	mem::map<int32_t, PS_BossFirstKillPlayerInfo> m_kBossFirstKill;
	FirstKillRewardPlayerMap m_mBossFirstKillRewardPlayerInfo;	//Boss��ɱ��������ȡ�����Ϣ
	mem::set<int> m_kServerBossFirstKillRecord;									//ȫ��Boss��ɱ��������ȡ��¼�������boss��chartype
	mem::map<int, bool> m_mBossFirstKillRecord;									//����Boss��ɱ��������ȡ��¼�������boss��chartype
	
	//���������
	int				m_iSevenDaysEndTime1;		//�����ʱ��
	int				m_iSevenDaysEndTime2;		//�콱����ʱ��

	int				m_iFirstDayLoginRewardTime;

	mem::map<int32_t,bool> m_mCommanderFrame;		//�Ѽ����ͷ���boolֵ��ʾ�Ƿ�鿴��
	mem::map<int32_t,bool> m_mCommanderHead;		//�Ѽ����ͷ��boolֵ��ʾ�Ƿ�鿴��

	//��������ˢ�´���
	int				m_iOfferTaskRefreshTimes;
	int				m_iOfferTaskAcceptTimes;

	StringType				m_mDrawCardCumlateReward;		//Ѱ���ۼƳ齱����
	//��VIP��� true������ȡ
	mem::vector<int>		m_VipNewRewardRecord;
	int32_t					m_iCurVipNewLevel;
	bool					m_bVipNewOpen;

	int						m_iCollectBossJP;

	mem::map<int,bool>		m_nRedPointPlusFlag;
	
	//����ս����
	mem::map<int, ST_Alliance_Boss_StateInfo> m_BestGuildBossInfoMap;
	mem::map<int, ST_Single_Tower_info> m_BestGuildTowerInfo;
	mem::map<int, ST_Single_Tower_info> m_BestGuildFortressInfo;
	mem::map<int, ST_Single_Tower_info> m_BestGuildRoyalInfo;
	bool									m_BestGuildRewardFlag;//���㽱���Ƿ���ȡ
	int										m_BestGuildFinishedTask;//������ɽ�������
	//����ͼ�ڵ����
	mem::vector<PS_NearbyPlayerInfo> m_VecNearbyPlayers;

	//������ȴʱ�� ���ID����һ�������ʱ���
	mem::map<unique_id_type,float>	m_InviteCountDown;

	//�������ҵ����  ID,�Ƿ��ڸ���
	mem::vector<unique_id_type>		m_ShowEnemyPlayers;

	//vip�����ѿ��ٹһ�����
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
	
	//���ͼ�й�������
	mem::map<int, mem::map<int, int>>		m_mNpcCount;

	//����
	std::map<int32_t,std::map<int32_t,bool>> m_nThemeACTRaidBossRwdRecord;
	std::vector<int32_t>					m_nThemeACTRaidBossFlag;//���б���˵������ˣ�����Ҫ���

	std::map<int,bool>						m_nRedPointFlag;
	//������������Ȩ˫����ȡ ---------�������䱦��
	mem::map<int,int>						m_mExtraRewardInfo;
	int										m_iFinishMainTaskID;
	
	//�ƽ�boss
	std::vector<PB_ServerPlayerBossFlagData>	m_vServerPlayerBossFlagInfo;
	int											m_nResult;
	PB_ServerPlayerBossFlagData					m_serverPlayerBossFlagData;

	//�����ͼ������¼
	mem::map<int, int>							m_kEnterMapRecord;
	mem::set<int>								m_kCanEnterMapIDSet;

	//���ս���������							
	std::vector<PS_ItemRecord>					m_SoloRwd;
	LC_ClientBattleRecordDataBase				m_SkyArenaSettle;
};	

#endif
