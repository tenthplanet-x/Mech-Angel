#ifndef __BUDDY_INSTANCE_H__
#define __BUDDY_INSTANCE_H__

#pragma once

#include "LC_DirtyFlag.h"
#include "LC_Define.h"
#include "CF_SystemTitleList.h"
#include "CF_LevelUp.h"
#include "CF_BuddyUpgrade.h"
#include "BuddyAttrManager.h"
#include "char_data.pb.h"

#include "LC_BuddyBase.h"

#include "BuddyInstanceData.h"
#include "UT_CSVHelpper.h"

class LC_ServerPlayer;
class LC_ServerItem;
//namespace GameLogic

class BuddyInstance : public LC_BuddyBase
{
public:
	BuddyInstance(object_id_type nOwnerPlayerId);
	~BuddyInstance();
public:
	virtual int32_t OnActivateSkill_LearnSkill( int32_t nSkillId );
	virtual int32_t OnAddSkill(int nSkillId);
	virtual void OnActivateSkill( int32_t nSkillId );
	int32_t GetTotalActivatedSkillCount();
public:
	void InitSoloDuleBuddyData(LC_ServerPlayer* pkPlayer, int buddyID, int buddyLevel, int buddyStarLevel, int propertyID);
	void InitSoloDuleBuddyAttrMap(LC_ServerPlayer* pkPlayer, int32_t propertyID);

	int InitTemplateData(const CSVFile::CF_Buddy::DataEntry* pData, bool bInit);
	void InitInstanceData();
	void LoadDBData(const char_data_define::buddy_summary_single_data & summary);//, const char_data_define::buddy_single_instance_data & detail);
	void Clear(void){}

	void SaveData(char_data_define::buddy_single_instance_data& data);
	void LoadData(const char_data_define::buddy_single_instance_data& data);

	void InitGameData();

	LC_ServerPlayer* GetOwnerPlayerByBuddyInstance();
	LC_ServerPlayer* GetAssignedPlayer();
	object_id_type GetAssignedPlayerId();

	void SetLockAssignedPlayerId( bool bFlag );

	//填充PS_Buddy_Info_BuddyCS数据结构
	void FillBuddyCSInfo(PS_Buddy_Info_BuddyCS& buddyData);

	void SyncAllDataToClient();
	// Detail Sync
	//增量同步buddy纹章槽位修改
	void SyncBuddyEquipSlotData(int slotIndex);
	//同步buddy纹章槽位
	void SyncBuddyEquipSlotPack();
	void SyncEquipGradeLevel();
	void SyncSkillAsset(bool bForce = false);
	void SyncBaseData();
	void SyncShortcutBar();
	void SyncBuddyAttributes(uint32_t uSyncType,LC_ActorAttributeMap&SyncAttributeMap, bool login, uint64_t nCombatScore, int32_t ReasonType=SPART_None);
	//void Sync
	bool LearnSkill(int lSkillID, int& iSlotIdx, bool send= true, int32_t runeSystemType=-1, int32_t runeSystemParam=-1, GameLogic::LC_SkillIDSet* pkReplacedSkills = NULL, bool bSyncLearnSkillEvent=true);
	virtual bool RemoveSkill(int lSkillID);

	int64_t GetDetalExp();

	//刷新buddy纹章属性
	void RefreshBuddyEquipAttrMap();
	//获得buddy装备槽位
	CBuddyEquipPack& GetEquipPack();

	//通过槽位index，获得槽位等级
	int32_t GetEquipSlotLevelBySlotIndex(int slotIndex);
	//获得buddy纹章总等级
	int32_t GetEquipSlotTotalLevel();
	//通过槽位index，获得槽位星级
	int32_t GetEquipSlotStarLevelBySlotIndex(int slotIndex);
	//获得buddy纹章总星级
	int32_t GetEquipSlotTotalStarLevel();

	//获得buddy纹章数据 key:slotindex, value:itemid slotlevel slotstarlevel
	void GetEquipInfo(std::map<int32_t,PS_EquipItemInfo_BuddyCS>& info);

	//获得现在穿戴的itemGrade颜色的装备的数量
	int32_t GetEquipGradeEquipCount(int32_t itemGrade);

	//更新纹章槽位等级
	bool UpdataEquipSlotLevel(int slotIndex, int slotLevel);
	//更新纹章槽位星级
	bool UpdataEquipSlotStarLevel(int slotIndex, int slotStarLevel);

	ResultType BuyGrowthPackage();
	ResultType GetGrowthReward();
	void SetDataToProtocolStruct(PS_SingleBuddyGrowthPackage& msg);

	// Game logic data structure function interfaces
	LC_SkillAsset* GetSkillAsset();
	LC_ShortcutBar* GetShortcutBar();
	LC_SkillCoolDownMap* GetSkillCoolDownMap();
	void BuildSkillCoolDownMap();
	// Skill Upgrade
	ResultType UpdateSkillInSkillAsset(int iCategory, int& iEntryIndex, int iMax);
	// actor BaseData
	void SetLevel(int lLevel, bool bInit);
	int GetLevel();
	int	GetMaxLevel();
	bool AddExp( attr_value_type lExp, int32_t eSrcType, int lSrcTypeID = 0, bool bIgnoreEXPRate = false );
	bool SetEXP(attr_value_type lEXP);
	attr_value_type GetEXP();

	int GetTemplateID();

	void SetHPFromDB(attr_value_type nValue);
	attr_value_type GetHPFromDB();
	void SetMPFromDB(attr_value_type nValue);
	attr_value_type GetMPFromDB();

	int GetShapeShiftCharTypeID();
	void SetShapeShiftCharTypeID(int lCharTypeID);

	void OnCreate();
	void OnBind( object_id_type nAssignedPlayerId );
	void OnUnBind();
	bool IsBind();
	void OnRefreshOwnerLevel();
	// new buddy base Data, like star
	int32_t GetStarLevel();
	// 因为所有的check and reducing都在Player做了，那边有完整的上下文，所以，这里的UpgradeStar仅仅就是个"Do"
	ResultType DoUpgradeStar();

	LC_ActorAttributeMap* GetFinalAttributeMap();


	void OnLevelData(int nOrigLevel, int lLevel, bool bInit);

	
	int SetHP(attr_value_type lHP);
	attr_value_type GetHP();

	int SetMP(attr_value_type  lMP);
	attr_value_type GetMP();

	void SetOldHP(attr_value_type lOldHP);
	attr_value_type GetOldHP();

	bool GetLevelUpFlag();
	void SetLevelUpFlag(bool tag);
	ResultType TryManualLevelUp();
	void RefreshLevelAttrMap(LC_ServerPlayer* pkPlayer,BuddyInstance*pBuddy){ m_BuddyAttr.RefreshLevelAttrMap(pkPlayer,pBuddy);}

	void CheckFlushRuneSlotDressState();
	int32_t GetCanDressRuneSlotCount();

	LC_ActorStateMap* GetStateMap();

	int32_t GetBuddyCurrentTransformID();
	ResultType SetBuddyCurrentTransformID(int id, bool force, bool sync=true);
	ResultType ActiveBuddyTransform(int id, bool useItem, bool sync, bool addAttr);
	ResultType AddBuddyTransformVaildTime(int id);
	void DeactiveBuddyTransform(int id);
	void CheckBuddyTransformInvaild();
	void NoticeActiveBuddyTransfromID(int id);
	int32_t GetTransformNum_NotInitID();
	void GetBuddyTransformID(mem::vector<int32_t>& transformID);


	void AddEquipSlotAttr();

	void EquipAllItem(bool login);

	ResultType FlushControllerSkillState(LC_ActorStateMap& input);
	ResultType ActiveSkillState(int lStateTypeID, int lStateLifeTime, object_id_type lStateAttacherID, float fPara0 = 0.0f, float fPara1 = 0.0f, float fPara2 = 0.0f, float fPara3 = 0.0f, float fPara4 = 0.0f);
	ResultType ActiveSkillState(Skill::SkillStateParam& rParam);
	ResultType DeactiveSkillState(int lStateTypeID);
	int DeactiveSkillState(int iBuffType, int iKind, int iLevel, int iDeactiveMaxCount);

	void FixMaxHPMP();

	DamageElementType GetDamageElementType();

	void FlushActivateSkillID();
	void UpdateActivateSkillID(int32_t nSkillId);
	int ActivateSkill(int32_t nSkillId, bool init=false);
	int CheckActivateSkill( int32_t nSkillId );

	int ModifySkillUsableCount(int32_t nSkillId, int32_t nDeltaCount, LC_ServerPlayer* pOwner = NULL );
	int ModifySkillMaxUsableCount(int32_t nSkillId, int32_t nDeltaCount);

	int32_t GetSkillUsableCount(int32_t nSkillId, LC_ServerPlayer* pOwner = NULL);
	int32_t GetSkillMaxUsableCount(int32_t nSkillId, LC_ServerPlayer* pOwner = NULL);
	int32_t GetSkillEnableUsableCount(int32_t nSkillId, LC_ServerPlayer* pOwner = NULL);

	int32_t Update(float fCurrentTime, float fDeltaTime);

	uint64_t GetAllBaseAttributeCombatScore();

	uint64_t GetBuddyEquipCombatScore();

	ResultType		OneKeyReinforceEquipSlot(int32_t equipSlotIndex, int32_t& newLevel);
	ResultType		ReinforceEquipSlot(int32_t equipSlotIndex, int32_t& newLevel);
	ResultType		EquipSlotStar(int32_t equipSlotIndex, int32_t& newStarLevel);

	void SyncBuddyRuneStarLevelInfo();

	void GetBuddyRuneInfo(std::map<int32_t,PS_RuneItemInfo_BuddyCS>& info);

	uint64_t	GetBuddySkillCombatScore();

	void					GetEquipSlotStarAttribiteMap(int slotIndex, int level, LC_ActorAttributeMap& attrMap);
	int32_t				GetEquipSlotStarAttribiteChange(int slotIndex, int level);
	
	void OnIncActivatedSkillCount();

	void AutoSkillLevelUp(bool nNotice = true);

	void FlushBuddyUpgradeSkill();
	void UpdateBuddyUpgradeSkill(int32_t newSkill);
public:
	//获得宝具pack
	CRuneEquipPack& GetRunePack();
	//刷新宝具属性
	void RefreshBuddyRuneAttrMap();
	//初始化buddy穿戴的宝具技能
	void EquipAllRuneItem(bool login);
	//同步宝具
	void SyncRunePackToClient();

	//buddy宝具升级
	void RuneUpgrade(int32_t slotIIndex, int32_t targetLevel);
	//buddy宝具升星
	void BuddyRuneStar(int32_t slotIIndex);
	//buddy宝具重置
	void BuddyRuneReset(int32_t slotIIndex);

	//获得宝具技能id
	void GetRuneSkillID(mem::vector<int32_t>& skillVec);
	//获得宝具的技能属性
	void GetRuneSkillAttr(Utility::UT_SIMDataList& skillAttr);	
	//获得宝具的属性
	void GetBuddyRuneAttrMap(LC_ActorAttributeMap& attrMap);
	//获得宝具的战力
	uint64_t GetBuddyRuneScore();

	//获得宝具的总等级
	int32_t GetBuddyRuneTotalLevel();
	int32_t GetBuddyRuneTotalStarLevel();
	//获取buddy宝具槽位里的宝具数据
	const LC_BackPackEntry* GetRuneDataFromBuddyRune(int32_t itemID);
	//获取slotIndex槽位的宝具数据
	LC_BackPackEntry* GetRuneItemByIndex(int32_t slotIndex);
	//获得buddy宝具的itemid和等级
	void GetAllBuddyRuneItemLevelData(mem::map<int32_t, int32_t>& itemID2LevelMap);
	//获得buddy宝具的itemid和星级
	void GetAllBuddyRuneItemStarLevelData(mem::map<int32_t, int32_t>& itemID2StarLevelMap);

	//宝具槽位是否可以穿戴
	bool GetRuneSlotDressState(int32_t slotIndex);
	//设置宝具槽位可以穿戴
	void SetRuneSlotDressState(int32_t slotIndex);

	//给某个槽位穿上宝具
	ResultType EquipRuneToBuddy(int32_t slotIndex, LC_BackPackEntry* pkItem);
	//从某个槽位脱下宝具
	ResultType UnEquipRuneFromBuddy(int32_t slotIndex);
	//互换buddy槽位的宝具
	ResultType SwitchBuddyEquipRune(BuddyInstance* pkDestBuddy, int32_t srcIndex, int32_t destIndex);
	//用基础背包槽位的宝具替换buddy的宝具
	ResultType SwitchBasicRuneEquipRune(LC_BackPackEntry* srcRuneItem, int32_t destIndex);
private:
	int32_t Update_SkillAsset(float fCurrentTime, float fDeltaTime);

	ResultType	_operationSuitAttribute(int iFuncID, int iSuitNum, mem::set<int32_t>* pkHappenedKeys);
	ResultType	_unoperationSuitAttribute(int iFuncID, int iSuitNum, mem::set<int32_t>* pkHappenedKeys);

	void RefreshSkillUsableCount();

	ResultType		TryReinforceEquipSlot(int itemID, int equipSlotIndex, int& newLevel, bool sync=true);
	ResultType		TryEquipSlotStar(int equipSlotIndex, int& newStarLevel);

	void LoadDBData_SkillAsset(const char_data_define::skill_asset_info& kSkillInfo);
	void LoadDBData_ShortcutBar(const char_data_define::shortcutbar_info& info);
	void LoadDBData_Attributes();

	// Moveo to private later
	void SaveData_BaseData(char_data_define::buddy_base_data& data);
	void LoadData_BaseData(const char_data_define::buddy_base_data& data);

	void SaveData_BaseData_Common(char_data_define::buddy_base_data& data);
	void LoadData_BaseData_Common(const char_data_define::buddy_base_data& data);

	void SaveData_FullData(char_data_define::buddy_full_data& data);
	void LoadData_FullData(const char_data_define::buddy_full_data& data);

	void SaveData_FullData_SkillAsset(char_data_define::skill_asset_info& data);
	void LoadData_FullData_SkillAsset(const char_data_define::skill_asset_info& data);

	void SaveData_FullData_ShortcutBar(char_data_define::shortcutbar_info& data);
	void LoadData_FullData_ShortcutBar(const char_data_define::shortcutbar_info& data);

	void SaveData_FullData_GrowthPackageInfo(char_data_define::growth_package_info& data);
	void LoadData_FullData_GrowthPackageInfo(const char_data_define::growth_package_info& data);

	void SaveData_FullData_ActivateSkillID(char_data_define::buddy_activate_skill_info& data);
	void LoadData_FullData_ActivateSkillID(const char_data_define::buddy_activate_skill_info& data);

	void SaveData_FullData_UpgradeSkillID(char_data_define::buddy_upgrade_skill_info* pkData);
	void LoadData_FullData_UpgradeSkillID(const char_data_define::buddy_upgrade_skill_info& data);

	void SaveData_FullData_RuneItemInfo(::google::protobuf::RepeatedPtrField< ::char_data_define::char_runeInfo >& data);

	void InitInstanceData_ServerPlayer();
	void InitInstanceData_ActorBase();

	void SetLevel_BaseActor(int lLevel);

	int TryAutoLevelUp( bool bCheckMaster = false );
	void ResetAttrSyncFlag();

	void HandleExpBound();
	bool CheckExpBound();

	
	void OnBothLevelData(int nOrigLevel, int lLevel, int nOrigStarLevel, int nStarLevel, bool bInit, bool bEnterGame = false,bool bLoad = false);
	void OnBothLevelData_Attribute(LC_ActorAttributeMap& input, const CSVFile::CF_Buddy::DataEntry* pConfig, const CF_LevelUp::DataEntry* pLevelData, const CF_BuddyUpgrade::DataEntry* pStarData, bool bForScoreCalc);
	
	void OnBind_RevertSkillState();

public:
	// 新增 直接穿戴物品 by id 不走背包
	ResultType EquipItemById(int nItemId, int iEquipSlotIndex);
	ResultType UnEquipItemInEquipSlot_Base(int iSlotIndex, bool nBuddyCompositionFlag=false);
	ResultType CheckUnEquipItemInEquipSlot(int iSlotIndex);

	bool CheckEquipUpgradeConfig();
	void EquipUpgrade();
	int GetQuality();

private:
	void OnSetLevel( int nCurrentLevel );

	// Create logic
	bool InitSkillData();
	bool InitBuddySkills(const StringType& sRawSkills);
	bool InitBuddySkills(Utility::UT_SIMDataList& kRawSkills);
	void OnLoadSkillData();

	//Equip Events.
	ResultType OnEquipItem(LC_ServerItem* pItemBase, LC_BackPackEntry* pkItemEntry, int slotIndex, float fCurrentTime, bool bAttrRefresh, bool initGame=false);
	ResultType OnEnableEquipItem(LC_ServerItem* pItemBase,LC_BackPackEntry* pkItemEntry, int slotIndex, float fCurrentTime, bool bAttrRefresh);
	ResultType OnUnEquipItem(LC_ServerItem* pItemBase,LC_BackPackEntry* pkItemEntry, int slotIndex, float fCurrentTime, bool bAttrRefresh);
	ResultType OnDisableEquipItem(LC_ServerItem* pItemBase,LC_BackPackEntry* pkItemEntry, int slotIndex, float fCurrentTime, bool bAttrRefresh);

	ResultType UpdateSkillInSkillAsset_PlayerBase(int iCategory, int& iEntryIndex, int iMax);
	const CF_BuddyUpgrade::DataEntry* UpdateBuddyUpgradeData(int32_t nBuddyID, int32_t nStarLevel);
public:
	LC_ActorAttributeMap* GetBuddyGiveCore(){return m_BuddyAttr.GetHeroGiveToOtherCoreAttrMap();}
	void SetChainAllAttrMap(LC_ServerPlayer* pPlayer,LC_ActorAttributeMap *ChainAllAttributeMap, bool login, int32_t reasonType){m_BuddyAttr.SetChainAllAttrMap(pPlayer,this,ChainAllAttributeMap, login, reasonType);}
	LC_ActorAttributeMap* GetNewFinalAttributeMap(){return m_BuddyAttr.GetNewFinalAttributeMap();}
	BuddyAttrMgr& GetBuddyAttrMgr(){return  m_BuddyAttr;}
	BuddyInstanceData& GetBuddyInstanceData(){return m_Data;}
private:
	object_id_type m_nOwnerPlayerIdByBuddyInstance;
	object_id_type m_nAssignedPlayerId;
	bool m_bInitGame;
	LC_ServerPlayer* m_pOwnerPlayer_Cache;
	CSVFile::CF_Buddy::DataEntry* m_pConfig;
	CSVFile::Condition2PtrPair<CF_BuddyUpgrade, int32_t, int32_t>	m_kCurStarData;		// 当前查表项缓存
	BuddyInstanceData m_Data;
	BuddyAttrMgr m_BuddyAttr;   // 英雄所有属性管理
	
	friend class BuddyHelper;
};



struct Buddy_FixHPMP_Util
{
	Buddy_FixHPMP_Util(BuddyInstance* actor)
		: m_old_hp_(actor->GetNewFinalAttributeMap()->GetAttribute(ATT_MAX_HP)),
		m_old_mp_(actor->GetNewFinalAttributeMap()->GetAttribute(ATT_MAX_MP)),
		m_actor_(actor),
		m_rollback_(false)
	{
	}
	~Buddy_FixHPMP_Util()
	{
		if (!m_rollback_)
		{
			attr_value_type delta_hp = m_actor_->GetNewFinalAttributeMap()->GetAttribute(ATT_MAX_HP) - m_old_hp_;
			attr_value_type delta_mp = m_actor_->GetNewFinalAttributeMap()->GetAttribute(ATT_MAX_MP) - m_old_mp_;
			if (delta_hp != 0)
			{
				m_actor_->SetHP(Utility::UT_MathBase::MaxInt64(1, m_actor_->GetHP() + delta_hp));
			}
			if (delta_mp != 0)
			{
				m_actor_->SetMP((int)Utility::UT_MathBase::MaxInt64(1, m_actor_->GetMP() + delta_mp));
			}
		}
	}
	inline void rollback() { m_rollback_ = true; }
private:
	attr_value_type			m_old_hp_;
	attr_value_type			m_old_mp_;
	BuddyInstance* 			m_actor_;
	bool					m_rollback_;
};


#endif //__ACT_ACTIVITY_ASSET_H__
