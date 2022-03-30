#include "BuddyInstance.h"
#include "CF_Buddy.h"
#include "CF_SkillLevelUp.h"
#include "CF_SkillExecuteList.h"
#include "CF_WorldMapList.h"
#include "CF_LevelUp.h"
#include "CF_ItemGen.h"
#include "CF_ItemList.h"
#include "CF_Helper.h"
#include "CF_ItemEquipLimitFunc.h"
#include "LC_ServerPlayer.h"
#include "LC_ItemFactoryBase.h"
#include "LC_ServerItem.h"
#include "LC_GemReinforceAsset.h"
#include "LC_ItemSuitManager.h"
#include "SK_SkillExecute.h"
#include "SK_Factory.h"
#include "MG_Stream.h"
#include "MG_PB.h"
#include "MG_AssistProtocol.h"
#include "MG_CS_FriendProtocol.h"
#include "MG_BackPackProtocol.h"
#include "MG_ShortcutKeyProtocol.h"
#include "MG_Game2Guild.h"
#include "MG_Common.h"
#include "MG_Forbid.h"
#include "MG_TaskProtocol.h"
#include "BuddyHelper.h"
#include "LC_ShortcutBar.h"
#include "CF_ItemEffect.h"
#include "GlobalSettings.h"
#include "LC_GameEventManager.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerLogicManager.h"
#include "CF_BuddyGrowGift.h"
#include "UT_ServerHelper.h"
#include "CF_EquipSlotReinforce.h"
#include "CF_EquipSlotStar.h"
#include "CF_ExportSoloDuleRobotProperty.h"
#include "CF_BuddyTransform.h"
#include "UT_OperateLog.h"

#include "LC_PackEntryFactory.h"
#include <map>
using namespace GameLogic;
using namespace Skill;
using namespace  OPERATELOG;

#define			MAX_PLAYER_LEVEL		60

typedef mem::list<std::pair<int32_t, int32_t> > local_suit_list;
typedef mem::map<int32_t, local_suit_list> local_suit_list_map;
static int32_t _st_get_suit_func_weight(int32_t iID)
{
	static ItemSuitManager * inst = SafeGetSingleton<ItemSuitManager>();
	ItemSuitInfoType * pkSuitData = inst->GetSuitInfo(iID);
	return pkSuitData ? pkSuitData->m_iWeight : 0;
}
static int32_t _st_get_suit_func_catagory(int32_t iID)
{
	static ItemSuitManager * inst = SafeGetSingleton<ItemSuitManager>();
	ItemSuitInfoType * pkSuitData = inst->GetSuitInfo(iID);
	return pkSuitData ? pkSuitData->m_iCatagory : 0;
}

static bool _st_suit_comp(const local_suit_list::value_type& a, const local_suit_list::value_type& b)
{
	return _st_get_suit_func_weight(a.first) > _st_get_suit_func_weight(b.first);
}

BuddyInstance::BuddyInstance(object_id_type nOwnerPlayerId)
	: m_nOwnerPlayerIdByBuddyInstance(nOwnerPlayerId)
	, m_nAssignedPlayerId(INVALID_ACTOR_ID)
	, m_pConfig( NULL )
	, m_bInitGame(false)
	, m_pOwnerPlayer_Cache(NULL)
{
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_PlayerBase* pOwner = (LC_PlayerBase*)pkWorldManager->FindObject(m_nOwnerPlayerIdByBuddyInstance);
	m_Data.m_Data_PlayerBase.m_pkShortcutBar = T_NEW_D LC_ShortcutBar((LC_PlayerBase*)pOwner);
	m_Data.m_Data_ActorBase.m_kSkillStateMap = T_NEW_D LC_ActorStateMap;
	m_Data.m_Data_ActorBase.m_kSkillStateMap->SetOwner(pOwner);
	m_Data.m_Data_PlayerBase.m_kSkillAsset.setOwner(pOwner);
	m_Data.m_Data_ActorBase.m_kStateCache.clear();
	m_pOwnerPlayer_Cache = (LC_ServerPlayer*)pOwner;
	m_Data.m_Data_ServerPlayer.m_lHPFromDB = 0;
	m_Data.m_Data_ServerPlayer.m_lMPFromDB = 0;
}

BuddyInstance::~BuddyInstance()
{
	T_SAFE_DELETE(m_Data.m_Data_PlayerBase.m_pkShortcutBar);
	T_SAFE_DELETE(m_Data.m_Data_ActorBase.m_kSkillStateMap);
	m_pOwnerPlayer_Cache = NULL;
}

void BuddyInstance::LoadDBData(const char_data_define::buddy_summary_single_data & summary)//, const char_data_define::buddy_single_instance_data & detail)
{
	SetLevel(summary.level(), true);
}

void BuddyInstance::SaveData(char_data_define::buddy_single_instance_data& data)
{
	data.set_buddyid(m_pConfig->_cID);
	SaveData_BaseData(*data.mutable_base_data());
	SaveData_FullData(*data.mutable_full_data());
}

void BuddyInstance::LoadData(const char_data_define::buddy_single_instance_data& data)
{
	int32_t nBuddyId = data.buddyid();
	LoadData_BaseData(data.base_data());
	LoadData_FullData(data.full_data());
	RefreshSkillUsableCount();
}

void BuddyInstance::SaveData_BaseData(char_data_define::buddy_base_data& data)
{
	SaveData_BaseData_Common(data);

	GetEquipPack().SaveData(data.mutable_buddyequip());
	GetRunePack().SaveData(data.mutable_buddyruneequip());
}

void BuddyInstance::LoadData_BaseData(const char_data_define::buddy_base_data& data)
{
	LoadData_BaseData_Common(data);

	GetEquipPack().LoadData(data.buddyequip());
	GetRunePack().LoadData(data.buddyruneequip());
}


void BuddyInstance::SaveData_BaseData_Common(char_data_define::buddy_base_data& data)
{
	data.set_exp(m_Data.m_Data_ActorBase.m_lEXP);
	data.set_hp(m_Data.m_Data_ActorBase.m_lHP);
	//data.set_hp(0);
	data.set_mp(m_Data.m_Data_ActorBase.m_lMP);
	data.set_star_level(m_Data.m_Data_Base.m_nStarLevel);
	data.set_corecombatscore(GetBuddyAttrMgr().GetBuddyCombatScoreByType(BCST_BuddyGetCore) );
	data.set_buddycombatscore(GetBuddyAttrMgr().GetBuddyCombatScoreByType(BCST_FinalBuddy));
	
	data.set_currenttransformid(m_Data.m_Data_Base.m_nCurrentTransformID);
	for(int i=0; i<m_Data.m_Data_Base.m_nActiveTransformRecord.size(); ++i)
	{
		data.add_activetransformrecord(m_Data.m_Data_Base.m_nActiveTransformRecord[i]);
	}
	for(int i=0; i<m_Data.m_Data_Base.m_nInvaildTransformTime.size(); ++i)
	{
		::char_data_define::char_BuddyTransformInvaildTime* info = data.add_buddytransforminvaildtime();
		info->set_transformid(m_Data.m_Data_Base.m_nInvaildTransformTime[i].m_nTransformID);
		info->set_invaildtime(m_Data.m_Data_Base.m_nInvaildTransformTime[i].m_nInvaildTime);
	}
}

void BuddyInstance::LoadData_BaseData_Common(const char_data_define::buddy_base_data& data)
{
	m_Data.m_Data_ActorBase.m_lEXP = data.exp();
	m_Data.m_Data_ActorBase.m_lHP = data.hp();
	m_Data.m_Data_ActorBase.m_lMP = data.mp();
	SetHPFromDB(data.hp());
	SetMPFromDB(data.mp());
	m_Data.m_Data_Base.m_nStarLevel = data.star_level();
	OnBothLevelData(1, m_Data.m_Data_ActorBase.m_lLevel, 1, m_Data.m_Data_Base.m_nStarLevel, true);
	//AutoSkillLevelUp(false);
	m_Data.m_Data_PlayerBase.m_kSkillAsset.SetDirtyFlag( true );
	//CalcBuddySkill();

	m_Data.m_Data_Base.m_nCurrentTransformID = data.currenttransformid();
	for(int i=0; i<data.activetransformrecord_size(); ++i)
	{
		m_Data.m_Data_Base.m_nActiveTransformRecord.push_back(data.activetransformrecord(i));
	}
	for(int i=0; i<data.buddytransforminvaildtime_size(); ++i)
	{
		BuddyTransformInvaildTime info;
		info.m_nTransformID = data.buddytransforminvaildtime(i).transformid();
		info.m_nInvaildTime = data.buddytransforminvaildtime(i).invaildtime();
		m_Data.m_Data_Base.m_nInvaildTransformTime.push_back(info);
	}

	if(m_Data.m_Data_Base.m_nCurrentTransformID == 0)
	{
		ActiveBuddyTransform(m_pConfig->_iDefaultTransform, false, false, false);
		SetBuddyCurrentTransformID(m_pConfig->_iDefaultTransform, false, false);
	}
}

void BuddyInstance::SaveData_FullData(char_data_define::buddy_full_data& data)
{
	SaveData_FullData_SkillAsset(*data.mutable_skill_info());
	SaveData_FullData_ShortcutBar(*data.mutable_shortcut_info());
	SaveData_FullData_GrowthPackageInfo(*data.mutable_growth_package_info());
	SaveData_FullData_ActivateSkillID(*data.mutable_activateskill());
	SaveData_FullData_UpgradeSkillID(data.mutable_upgradeskill());

	data.set_totalactivatedskillcount(m_Data.m_Data_Base.m_nTotalActivatedSkillCount);
}

void BuddyInstance::LoadData_FullData(const char_data_define::buddy_full_data& data)
{
	LoadData_FullData_SkillAsset(data.skill_info());
	LoadData_FullData_ShortcutBar(data.shortcut_info());
	LoadData_FullData_GrowthPackageInfo(data.growth_package_info());
	LoadData_FullData_ActivateSkillID(data.activateskill());
	LoadData_FullData_UpgradeSkillID(data.upgradeskill());
	
	m_Data.m_Data_Base.m_nTotalActivatedSkillCount = data.totalactivatedskillcount();
}

void BuddyInstance::SaveData_FullData_SkillAsset(char_data_define::skill_asset_info& data)
{
	LC_SkillAsset* pkSkillAsset = GetSkillAsset();
	LC_SkillCoolDownMap* pkSkillCDMap = GetSkillCoolDownMap();
	if (NULL != pkSkillAsset && NULL != pkSkillCDMap)
	{
		pkSkillAsset->SetDataToPB(data);
		pkSkillCDMap->SetCDToPB(data);
	}
}

void BuddyInstance::LoadData_FullData_SkillAsset(const char_data_define::skill_asset_info& data)
{
	//LC_SkillAsset* pkSkillAsset = GetSkillAsset();

	// Move to LoadBuddyData_SingleInstance
	//根据技能信息构造CD表
	BuildSkillCoolDownMap();
	//获取技能CD信息
	LC_SkillCoolDownMap* pkSkillCDMap = GetSkillCoolDownMap();
	if (NULL != pkSkillCDMap)
	{
		pkSkillCDMap->GetCDFromPB(data);
	
		mem::map<int,GameLogic::LC_SkillCoolDownMap::LC_SkillCoolDownData>::const_iterator it = pkSkillCDMap->GetSkillIDCoolDownDataMap().begin();
		for(; it!=pkSkillCDMap->GetSkillIDCoolDownDataMap().end(); ++it)
		{
			int skillID = it->first;
			if (IS_SKILL_TYPE(skillID))
			{
				int curCount = GetSkillUsableCount(skillID);
				int maxCount = GetSkillMaxUsableCount(skillID);
				if(curCount < maxCount)
				{
					pkSkillCDMap->SetSkillIdleState(skillID, false);
				}
			}
		}
	}
	OnLoadSkillData();
}

void BuddyInstance::SaveData_FullData_ShortcutBar(char_data_define::shortcutbar_info& data)
{
	LC_ShortcutBar* pkShortcutBar = GetShortcutBar();
	if (NULL == pkShortcutBar)
	{
		return;
	}

	pkShortcutBar->SetDataToPB(data);
}

void BuddyInstance::LoadData_FullData_ShortcutBar(const char_data_define::shortcutbar_info& data)
{
	LC_ShortcutBar* pkShortBar = GetShortcutBar();
	if (!pkShortBar)
	{
		return;
	}

	pkShortBar->GetDataFromPB(data);
}

void BuddyInstance::SaveData_FullData_GrowthPackageInfo(char_data_define::growth_package_info& data)
{
	data.set_be_actived(m_Data.m_Data_PlayerBase.m_nBeActived);
	data.set_active_time(m_Data.m_Data_PlayerBase.m_nActiveTime);
	data.set_day(m_Data.m_Data_PlayerBase.m_nDay);
}

void BuddyInstance::LoadData_FullData_GrowthPackageInfo(const char_data_define::growth_package_info& data)
{
	m_Data.m_Data_PlayerBase.m_nBeActived = data.be_actived();
	m_Data.m_Data_PlayerBase.m_nActiveTime = data.active_time();
	m_Data.m_Data_PlayerBase.m_nDay = data.day();
}

void BuddyInstance::SaveData_FullData_ActivateSkillID(char_data_define::buddy_activate_skill_info& data)
{
	for(int i=0; i<m_Data.m_Data_Base.m_ActivateSkillID.size(); ++i)
	{
		data.add_skillid(m_Data.m_Data_Base.m_ActivateSkillID[i]);
	}
}

void BuddyInstance::LoadData_FullData_ActivateSkillID(const char_data_define::buddy_activate_skill_info& data)
{
	for(int i=0; i<data.skillid_size(); ++i)
	{
		m_Data.m_Data_Base.m_ActivateSkillID.push_back(data.skillid(i));
	}

	FlushActivateSkillID();
}

void BuddyInstance::SaveData_FullData_UpgradeSkillID(char_data_define::buddy_upgrade_skill_info* pkData)
{
	if(NULL == pkData)
	{
		return;
	}

	for(mem::set<int32_t>::iterator it=m_Data.m_Data_Base.m_UpgradeSkillID.begin(); it!=m_Data.m_Data_Base.m_UpgradeSkillID.end(); ++it)
	{
		pkData->add_skillid(*it);
	}
}

void BuddyInstance::LoadData_FullData_UpgradeSkillID(const char_data_define::buddy_upgrade_skill_info& data)
{
	for(int i=0; i<data.skillid_size(); ++i)
	{
		m_Data.m_Data_Base.m_UpgradeSkillID.insert(data.skillid(i));
	}

	FlushBuddyUpgradeSkill();
}

void BuddyInstance::InitGameData()
{
	LC_ServerPlayer* m_pOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if (m_pOwnerPlayer == NULL)
		return;
	m_bInitGame = true;
	m_bInitGame = false;
}

DamageElementType BuddyInstance::GetDamageElementType()
{
	return static_cast<DamageElementType>(m_pConfig->_ielementdamagetype);
}

void BuddyInstance::FlushActivateSkillID()
{
	for(int i=0; i<m_Data.m_Data_Base.m_ActivateSkillID.size(); ++i)
	{
		ActivateSkill(m_Data.m_Data_Base.m_ActivateSkillID[i], true);
	}
}

void BuddyInstance::UpdateActivateSkillID(int32_t nSkillId)
{
	mem::vector<int32_t>::iterator findIt = find(m_Data.m_Data_Base.m_ActivateSkillID.begin(), m_Data.m_Data_Base.m_ActivateSkillID.end(), nSkillId);
	if(findIt == m_Data.m_Data_Base.m_ActivateSkillID.end())
	{
		m_Data.m_Data_Base.m_ActivateSkillID.push_back(nSkillId);
	}	
}

int BuddyInstance::ActivateSkill(int32_t nSkillId, bool init)
{
	LC_ServerPlayer* pOwner = GetOwnerPlayerByBuddyInstance();
	if (pOwner == NULL)
	{
		return RE_SKILL_ACTIVATE_SYSTEM;
	}

	LC_SkillAsset* pkSkillAsset = GetSkillAsset();
	if (NULL == pkSkillAsset)
	{
		return RE_SKILL_ACTIVATE_SYSTEM;
	}
	LC_SkillAssetData* pSkillData =  pkSkillAsset->GetSkillAssetData();
	LC_SkillAssetEntry* pSkillEntry = NULL;
	int nSize = pkSkillAsset->GetSkillAssetData()->GetSkillCount((skill_cat_type)SKILL_TYPE_SECT);
	for ( int i = 0; i < nSize; i++ )
	{
		LC_SkillAssetEntry* pNextSkillEntry= pkSkillAsset->GetSkillAssetData()->GetSkillAssetEntry((skill_cat_type)SKILL_TYPE_SECT, i);
		if (pNextSkillEntry == NULL)
			continue;

		if (GET_SKILL_INDEX(pNextSkillEntry->GetTypeID()) == nSkillId)
		{
			pSkillEntry = pNextSkillEntry;
			break;
		}
	}
	if (pSkillEntry == NULL)
	{
		return RE_SKILL_ACTIVATE_NO_SKILL;
	}

	if ( pSkillEntry->GetLogicValue() )
	{
		return RE_SKILL_ACTIVATE_ALREADY_DONE;
	}

	int nOrigMax = pSkillEntry->GetMaxUsableCount();
	pSkillEntry->SetLogicValue( 1, this );
	MG_Sync_SingleSkillLogicValue actSkill;
	actSkill.m_nBuddyId = m_pConfig->_cID;
	actSkill.m_nSkillId = nSkillId;
	pOwner->SendMsgToClient(MGPT_SYNC_SINGLE_SKILL_LOGIC_VALUE, &actSkill);

	if(false == init)
	{
		int nNewMax = pSkillEntry->GetMaxUsableCount();
		if (nNewMax != nOrigMax)
		{
			MG_Sync_SkillMaxUsableCount data;
			data.m_nBuddyId = m_pConfig->_cID;
			data.m_nSkillId = nSkillId;
			data.m_nMaxUsableCount = nNewMax;

			pOwner->SendMsgToClient(MGPT_SYNC_SKILL_MAX_USABLE_COUNT, &data);
		}

		UpdateActivateSkillID(nSkillId);
	}

	return RE_SKILL_ACTIVATE_SUCCESS;
}

int BuddyInstance::ModifySkillUsableCount(int32_t nSkillId, int32_t nDeltaCount, LC_ServerPlayer* pOwner )
{
	int nResult = -1;
	LC_SkillAssetEntry* pEntry = GetSkillAsset()->FindLearnedSkill(nSkillId, true);
	if (pEntry == NULL)
		return nResult;

	if ( pOwner == NULL )
		pOwner = GetOwnerPlayerByBuddyInstance();
	if ( pOwner == NULL)
		return nResult;

	if (pEntry->GetEnableCount() == false)
		return 1;

	float fDebugNow = GET_PROCESS_TIME;
	int nMaxCount = pEntry->GetMaxUsableCount();
	int nCurCount = pEntry->GetUsableCount();
	if (nCurCount >= nMaxCount && nDeltaCount > 0)
	{
		nCurCount = nMaxCount;
		return 1;
	}

	nResult = 0;
	int nCount = pEntry->ModifyUsableCount(nDeltaCount);
	
	MG_Sync_SkillUsableCount data;
	data.m_nBuddyId = m_pConfig->_cID;
	data.m_nSkillId = nSkillId;
	data.m_nUsableCount = nCount;
	//data.m_nNextCD = 8000;
	float fCur = GET_PROCESS_TIME;

	pOwner->SendMsgToClient(MGPT_SYNC_SKILL_USABLE_COUNT, &data, pOwner);

	if (nMaxCount == nCount)
		nResult = 1;
	return nResult;
}

int BuddyInstance::ModifySkillMaxUsableCount(int32_t nSkillId, int32_t nDeltaCount)
{
	int nResult = false;
	LC_SkillAssetEntry* pEntry = GetSkillAsset()->FindLearnedSkill(nSkillId, true);
	if (pEntry == NULL)
		return nResult;

	LC_ServerPlayer* pOwner = GetOwnerPlayerByBuddyInstance();
	if (pOwner == NULL)
		return nResult;

	int nOrigCount = pEntry->GetUsableCount();
	int nCount = pEntry->ModifyMaxUsableCount(nDeltaCount);

	int nNewCount = pEntry->GetUsableCount();
	if ( nNewCount != nOrigCount )
	{
		MG_Sync_SkillUsableCount data2;
		data2.m_nBuddyId = m_pConfig->_cID;
		data2.m_nSkillId = nSkillId;
		data2.m_nUsableCount = nNewCount;

		pOwner->SendMsgToClient(MGPT_SYNC_SKILL_USABLE_COUNT, &data2, pOwner);
	}

	MG_Sync_SkillMaxUsableCount data;
	data.m_nBuddyId = m_pConfig->_cID;
	data.m_nSkillId = nSkillId;
	data.m_nMaxUsableCount = nCount;

	pOwner->SendMsgToClient(MGPT_SYNC_SKILL_MAX_USABLE_COUNT, &data, pOwner);

	nResult = true;
	return nResult;
}

int32_t BuddyInstance::GetSkillUsableCount(int32_t nSkillId, LC_ServerPlayer* pOwner)
{
	int nResult = -1;
	LC_SkillAssetEntry* pEntry = GetSkillAsset()->FindLearnedSkill(nSkillId, true);
	if (pEntry == NULL)
		return 1;

	nResult = pEntry->GetUsableCount();
	return nResult;
}

int32_t BuddyInstance::GetSkillEnableUsableCount(int32_t nSkillId, LC_ServerPlayer* pOwner)
{
	int nResult = -1;
	LC_SkillAssetEntry* pEntry = GetSkillAsset()->FindLearnedSkill(nSkillId, true);
	if (pEntry == NULL)
		return nResult;

	nResult = pEntry->GetEnableCount();
	return nResult;
}

int32_t BuddyInstance::GetSkillMaxUsableCount(int32_t nSkillId, LC_ServerPlayer* pOwner)
{
	int nResult = -1;
	LC_SkillAssetEntry* pEntry = GetSkillAsset()->FindLearnedSkill(nSkillId, true);
	if (pEntry == NULL)
		return nResult;

	nResult = pEntry->GetMaxUsableCount();
	return nResult;
}

int32_t BuddyInstance::Update(float fCurrentTime, float fDeltaTime)
{
	if ( m_nAssignedPlayerId != INVALID_ACTOR_ID )
		Update_SkillAsset(fCurrentTime, fDeltaTime);
	if ( m_Data.m_Data_Base.m_bAttrDirty )
		m_Data.m_Data_Base.m_bAttrSync = true;
	ResetAttrSyncFlag();

	CheckBuddyTransformInvaild();
	return true;
}

void BuddyInstance::FlushBuddyUpgradeSkill()
{
	Utility::UT_SIMDataList kRawSkills;
	Utility::ParseFmtString2DataIFs(m_pConfig->_skillid, kRawSkills);
	for(int i=0; i<kRawSkills.size(); ++i)
	{
		int32_t skillID = kRawSkills[i].GetSkillID();
		int32_t slotIndex = i;

		CF_SkillExecuteList::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_SkillExecuteList>(skillID);
		if(NULL == pkData)
		{
			continue;
		}

		LC_SkillAssetEntry* pkEntry = m_Data.m_Data_PlayerBase.m_kSkillAsset.GetSkillEntryInAsset(pkData->_lCategory2, slotIndex);
		if(NULL == pkEntry)
		{
			continue;
		}

		CF_SkillLevelUp::DataEntry* pkUpgradeData = SafeGetCSVFileDataEntryPtr<CF_SkillLevelUp>(skillID);
		if(NULL == pkUpgradeData)
		{
			continue;
		}

		int newSkillID = pkUpgradeData->_lNextLevelSkillTypeID;
		while(IS_SKILL_TYPE(newSkillID) && m_Data.m_Data_Base.m_UpgradeSkillID.find(newSkillID) != m_Data.m_Data_Base.m_UpgradeSkillID.end())
		{
			LC_SkillAssetEntry* pkSkillEntry = m_Data.m_Data_PlayerBase.m_kSkillAsset.FindLearnedSkill(newSkillID);
			if(NULL == pkSkillEntry)
			{
				continue;
			}

			int32_t oldSkillID = pkSkillEntry->GetTypeID();
			if (oldSkillID >= newSkillID)
			{
				continue;
			}

			m_Data.m_Data_PlayerBase.m_kSkillAsset.GetSkillAssetData()->RemoveSkill(oldSkillID);
			m_Data.m_Data_PlayerBase.m_kSkillAsset.GetSkillAssetData()->AddSkill(newSkillID, slotIndex, NULL, pkEntry->GetLogicValue(), pkEntry->GetUsableCount(), this);
			m_Data.m_Data_PlayerBase.m_pkShortcutBar->UpdateShortcutWhenSkillUpgrading(oldSkillID, newSkillID);

			pkUpgradeData = SafeGetCSVFileDataEntryPtr<CF_SkillLevelUp>(newSkillID);
			if(NULL == pkUpgradeData)
			{
				break;
			}
			newSkillID = pkUpgradeData->_lNextLevelSkillTypeID;
		}
	}
}

void BuddyInstance::UpdateBuddyUpgradeSkill( int32_t newSkill)
{
	m_Data.m_Data_Base.m_UpgradeSkillID.insert(newSkill);
}

CRuneEquipPack& BuddyInstance::GetRunePack()
{
	return m_Data.m_Data_PlayerBase.m_RuneEquipPack;
}

void BuddyInstance::RefreshBuddyRuneAttrMap()
{
	GetRunePack().UpdateAttributeMap(this);
}

void BuddyInstance::EquipAllRuneItem(bool login)
{
	LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if(NULL != pkOwnerPlayer)
	{
		mem::vector<int32_t> runeSkill;
		GetRunePack().GetRuneSkillID(runeSkill);
		for(int i=0; i<runeSkill.size(); ++i)
		{
			int32_t index = -1;
			LearnSkill(runeSkill[i], index);
		}

		//刷新属性
		{
			GetBuddyAttrMgr().RefreshRuneAttrMap(this);
			GetBuddyAttrMgr().RefreshHeroAllAttributeMap(pkOwnerPlayer, this);
			//刷新此英雄 提供的灵魂链属性
			GetBuddyAttrMgr().RefreshHeroGiveToOtherCoreAttrMap(pkOwnerPlayer);
			//刷新灵魂链上玩家获得灵魂链属性
			pkOwnerPlayer->RefreshChainBuddyCoreAttrByOnChain(GetTemplateID(), login, SPART_Buddy_7);
			GetBuddyAttrMgr().RefreshNewFinalAttrMap(pkOwnerPlayer, this, login, SPART_Buddy_7);
		}
	}
}

void BuddyInstance::SyncRunePackToClient()
{
	LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if(NULL != pkOwnerPlayer)
	{
		MG_RLT_RuneEquipSlotPackData msg;
		msg.m_nBuddyID = GetTemplateID();
		GetRunePack().GetRunePackData(msg.m_RuneEquip);
		pkOwnerPlayer->SendMsgToClient(MGPT_SYNC_RUNE_EQUIP_PACK, &msg);
	}
}

void BuddyInstance::RuneUpgrade(int32_t slotIIndex, int32_t targetLevel)
{
	LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if(NULL != pkOwnerPlayer)
	{
		//属性要移到每个系统中
		bool tag = GetRunePack().Upgrade(pkOwnerPlayer, slotIIndex, targetLevel);
		if(tag)
		{
			GetBuddyAttrMgr().RefreshRuneAttrMap(this);
			GetBuddyAttrMgr().RefreshHeroAllAttributeMap(pkOwnerPlayer, this);
			//刷新此英雄 提供的灵魂链属性
			GetBuddyAttrMgr().RefreshHeroGiveToOtherCoreAttrMap(pkOwnerPlayer);
			//刷新灵魂链上玩家获得灵魂链属性
			pkOwnerPlayer->RefreshChainBuddyCoreAttrByOnChain(GetTemplateID(), false, SPART_Buddy_8);
			GetBuddyAttrMgr().RefreshNewFinalAttrMap(pkOwnerPlayer, this, false, SPART_Buddy_8);
		}	
	}
}

void BuddyInstance::BuddyRuneStar(int32_t slotIIndex)
{
	LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if(NULL != pkOwnerPlayer)
	{
		//属性要移到每个系统中
		bool tag = GetRunePack().Star(pkOwnerPlayer, slotIIndex);
		if(tag)
		{
			GetBuddyAttrMgr().RefreshRuneAttrMap(this);
			GetBuddyAttrMgr().RefreshHeroAllAttributeMap(pkOwnerPlayer, this);
			//刷新此英雄 提供的灵魂链属性
			GetBuddyAttrMgr().RefreshHeroGiveToOtherCoreAttrMap(pkOwnerPlayer);
			//刷新灵魂链上玩家获得灵魂链属性
			pkOwnerPlayer->RefreshChainBuddyCoreAttrByOnChain(GetTemplateID(), false, SPART_Buddy_9);
			GetBuddyAttrMgr().RefreshNewFinalAttrMap(pkOwnerPlayer, this, false, SPART_Buddy_9);
		}
	}
}

void BuddyInstance::BuddyRuneReset(int32_t slotIIndex)
{
	LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if(NULL != pkOwnerPlayer)
	{
		//属性要移到每个系统中
		bool tag = GetRunePack().Reset(pkOwnerPlayer, slotIIndex);
		if(tag)
		{
			GetBuddyAttrMgr().RefreshRuneAttrMap(this);
			GetBuddyAttrMgr().RefreshHeroAllAttributeMap(pkOwnerPlayer, this);
			//刷新此英雄 提供的灵魂链属性
			GetBuddyAttrMgr().RefreshHeroGiveToOtherCoreAttrMap(pkOwnerPlayer);
			//刷新灵魂链上玩家获得灵魂链属性
			pkOwnerPlayer->RefreshChainBuddyCoreAttrByOnChain(GetTemplateID(), false);
			GetBuddyAttrMgr().RefreshNewFinalAttrMap(pkOwnerPlayer, this, false);
		}
	}
}

void BuddyInstance::GetRuneSkillID(mem::vector<int32_t>& skillVec)
{
	LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if(NULL != pkOwnerPlayer)
	{
		GetRunePack().GetRuneSkillID(skillVec);
	}
}

void BuddyInstance::GetRuneSkillAttr(Utility::UT_SIMDataList& skillAttr)
{
	LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if(NULL != pkOwnerPlayer)
	{
		 GetRunePack().GetRuneSkillAttr(skillAttr);
	}
}

void BuddyInstance::GetBuddyRuneAttrMap(LC_ActorAttributeMap& attrMap)
{
	LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if(NULL != pkOwnerPlayer)
	{
		GetRunePack().GetRuneAttrMap(attrMap);
	}
}

uint64_t BuddyInstance::GetBuddyRuneScore()
{
	LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if(NULL != pkOwnerPlayer)
	{
		return GetRunePack().GetBuddyRuneScore();
	}
	return 0;
}

int32_t BuddyInstance::GetBuddyRuneTotalLevel()
{
	return GetRunePack().GetRuneTotalLevel();
}

int32_t BuddyInstance::GetBuddyRuneTotalStarLevel()
{
	return GetRunePack().GetRuneTotalStarLevel();
}

const LC_BackPackEntry* BuddyInstance::GetRuneDataFromBuddyRune(int32_t itemID)
{
	return GetRunePack().GetRuneDataFromBuddyRune(itemID);
}

LC_BackPackEntry* BuddyInstance::GetRuneItemByIndex(int32_t slotIndex)
{
	return GetRunePack().GetRuneItemByIndex(slotIndex);
}

void BuddyInstance::GetAllBuddyRuneItemLevelData(mem::map<int32_t, int32_t>& itemID2LevelMap)
{
	return GetRunePack().GetAllBuddyRuneItemLevelData(itemID2LevelMap);
}

void BuddyInstance::GetAllBuddyRuneItemStarLevelData(mem::map<int32_t, int32_t>& itemID2StarLevelMap)
{
	return GetRunePack().GetAllBuddyRuneItemStarLevelData(itemID2StarLevelMap);
}

bool BuddyInstance::GetRuneSlotDressState(int32_t slotIndex)
{
	return GetRunePack().GetRuneSlotDressState(slotIndex);
}

void BuddyInstance::SetRuneSlotDressState(int32_t slotIndex)
{
	GetRunePack().SetRuneSlotDressState(slotIndex);
	SyncRunePackToClient();
}

ResultType BuddyInstance::UnEquipRuneFromBuddy(int32_t slotIndex)
{
	LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if(NULL == pkOwnerPlayer)
	{
		return RE_FAIL;
	}

	ResultType res = GetRunePack().UnEquipRuneItem(pkOwnerPlayer, this, slotIndex);
	if(RE_SUCCESS == res)
	{
		pkOwnerPlayer->SyncBackPackToClient(PACK_TYPE_BASIC);

		GetBuddyAttrMgr().RefreshRuneAttrMap(this);
		GetBuddyAttrMgr().RefreshHeroAllAttributeMap(pkOwnerPlayer, this);
		//刷新此英雄 提供的灵魂链属性
		GetBuddyAttrMgr().RefreshHeroGiveToOtherCoreAttrMap(pkOwnerPlayer);
		//刷新灵魂链上玩家获得灵魂链属性
		pkOwnerPlayer->RefreshChainBuddyCoreAttrByOnChain(GetTemplateID(), false);
		GetBuddyAttrMgr().RefreshNewFinalAttrMap(pkOwnerPlayer, this, false);

		if(IsBind())
		{
			pkOwnerPlayer->BroadcastBuddyRuneStarLevel(GetAssignedPlayerId(), GetTemplateID(), slotIndex, 0, 0);
		}
	}

	return res;
}

ResultType BuddyInstance::SwitchBuddyEquipRune(BuddyInstance* pkDestBuddy, int32_t srcIndex, int32_t destIndex)
{
	if(NULL == pkDestBuddy)
	{
		return RE_FAIL;
	}

	LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if(NULL == pkOwnerPlayer)
	{
		return RE_FAIL;
	}

	//源不能无效
	LC_BackPackEntry* srcPackEntry = GetRunePack().GetRuneItemByIndex(srcIndex);
	if(NULL==srcPackEntry || !srcPackEntry->GetBackPackValueByType(LPSFET_VALID))
	{
		return RE_FAIL;
	}

	//目的可以无效
	LC_BackPackEntry* destPackEntry = pkDestBuddy->GetRunePack().GetRuneItemByIndex(destIndex);
	if(NULL == srcPackEntry)
	{
		return RE_FAIL;
	}

	int srcOldSkillID = srcPackEntry->GetBackPackValueByType(LCSPET_EQUIP_AFFILIATED_SKILL);
	if(0 != srcOldSkillID)
	{
		RemoveSkill(srcOldSkillID);

		int32_t slotIndex = -1;
		pkDestBuddy->LearnSkill(srcOldSkillID, slotIndex);
	}

	int destOldSkillID = destPackEntry->GetBackPackValueByType(LCSPET_EQUIP_AFFILIATED_SKILL);
	if(0 != destOldSkillID)
	{
		pkDestBuddy->RemoveSkill(destOldSkillID);

		int32_t slotIndex = -1;
		LearnSkill(destOldSkillID, slotIndex);
	}

	//互换
	LC_BackPackEntry tempEntry;
	tempEntry.Clone(srcPackEntry);
	srcPackEntry->Clone(destPackEntry);
	destPackEntry->Clone(tempEntry);

	{
		pkDestBuddy->GetBuddyAttrMgr().RefreshRuneAttrMap(pkDestBuddy);
		pkDestBuddy->GetBuddyAttrMgr().RefreshHeroAllAttributeMap(pkOwnerPlayer, pkDestBuddy);
		//刷新此英雄 提供的灵魂链属性
		pkDestBuddy->GetBuddyAttrMgr().RefreshHeroGiveToOtherCoreAttrMap(pkOwnerPlayer);
		//刷新灵魂链上玩家获得灵魂链属性
		pkOwnerPlayer->RefreshChainBuddyCoreAttrByOnChain(pkDestBuddy->GetTemplateID(), false, SPART_Buddy_7);
		pkDestBuddy->GetBuddyAttrMgr().RefreshNewFinalAttrMap(pkOwnerPlayer, pkDestBuddy, false, SPART_Buddy_7);

		if(pkDestBuddy->IsBind())
		{
			pkOwnerPlayer->BroadcastBuddyRuneStarLevel(pkDestBuddy->GetAssignedPlayerId(), pkDestBuddy->GetTemplateID(), destIndex, destPackEntry->GetBackPackValueByType(LPSFET_TYPE_ID), destPackEntry->GetBackPackValueByType(LCSPET_EQUIP_STAR));
		}
	}

	{
		GetBuddyAttrMgr().RefreshRuneAttrMap(this);
		GetBuddyAttrMgr().RefreshHeroAllAttributeMap(pkOwnerPlayer, this);
		//刷新此英雄 提供的灵魂链属性
		GetBuddyAttrMgr().RefreshHeroGiveToOtherCoreAttrMap(pkOwnerPlayer);
		//刷新灵魂链上玩家获得灵魂链属性
		pkOwnerPlayer->RefreshChainBuddyCoreAttrByOnChain(GetTemplateID(), false, SPART_Buddy_7);
		GetBuddyAttrMgr().RefreshNewFinalAttrMap(pkOwnerPlayer, this, false, SPART_Buddy_7);

		if(IsBind())
		{
			pkOwnerPlayer->BroadcastBuddyRuneStarLevel(GetAssignedPlayerId(), GetTemplateID(), srcIndex, srcPackEntry->GetBackPackValueByType(LPSFET_TYPE_ID), srcPackEntry->GetBackPackValueByType(LCSPET_EQUIP_STAR));
		}
	}

	return RE_SUCCESS;
}

ResultType BuddyInstance::SwitchBasicRuneEquipRune(LC_BackPackEntry* srcRuneItem, int32_t destIndex)
{
	if(NULL==srcRuneItem || !srcRuneItem->GetBackPackValueByType(LPSFET_VALID))
	{
		return RE_FAIL;
	}

	int itemID = srcRuneItem->GetBackPackValueByType(LPSFET_TYPE_ID);
	int starLevel = srcRuneItem->GetBackPackValueByType(LCSPET_EQUIP_STAR);
	LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if(NULL == pkOwnerPlayer)
	{
		return RE_FAIL;
	}

	LC_BackPackEntry* destPackEntry = GetRunePack().GetRuneItemByIndex(destIndex);
	if(!destPackEntry->GetBackPackValueByType(LPSFET_VALID))
	{
		return RE_FAIL;
	}

	int destOldSkillID = destPackEntry->GetBackPackValueByType(LCSPET_EQUIP_AFFILIATED_SKILL);
	if(0 != destOldSkillID)
	{
		RemoveSkill(destOldSkillID);
	}

	int newOldSkillID = srcRuneItem->GetBackPackValueByType(LCSPET_EQUIP_AFFILIATED_SKILL);
	if(0 != newOldSkillID)
	{
		int32_t slotIndex = -1;
		LearnSkill(newOldSkillID, slotIndex);
	}

	//互换
	LC_BackPackEntry tempEntry;
	tempEntry.Clone(srcRuneItem);
	srcRuneItem->Clone(destPackEntry);
	destPackEntry->Clone(tempEntry);

	{
		GetBuddyAttrMgr().RefreshRuneAttrMap(this);
		GetBuddyAttrMgr().RefreshHeroAllAttributeMap(pkOwnerPlayer, this);
		//刷新此英雄 提供的灵魂链属性
		GetBuddyAttrMgr().RefreshHeroGiveToOtherCoreAttrMap(pkOwnerPlayer);
		//刷新灵魂链上玩家获得灵魂链属性
		pkOwnerPlayer->RefreshChainBuddyCoreAttrByOnChain(GetTemplateID(), false, SPART_Buddy_7);
		GetBuddyAttrMgr().RefreshNewFinalAttrMap(pkOwnerPlayer, this, false, SPART_Buddy_7);

		if(IsBind())
		{
			pkOwnerPlayer->BroadcastBuddyRuneStarLevel(GetAssignedPlayerId(), GetTemplateID(), destIndex, itemID, starLevel);
		}
	}

	return RE_SUCCESS;
}

ResultType BuddyInstance::EquipRuneToBuddy(int32_t slotIndex, LC_BackPackEntry* pkItem)
{
	if(NULL == pkItem)
	{
		return RE_FAIL;
	}

	LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if(NULL == pkOwnerPlayer)
	{
		return RE_FAIL;
	}

	int itemID = pkItem->GetBackPackValueByType(LPSFET_TYPE_ID);
	int starLevel = pkItem->GetBackPackValueByType(LCSPET_EQUIP_STAR);
	ResultType res = GetRunePack().EquipRuneItem(this, slotIndex, pkItem);
	if(RE_SUCCESS == res)
	{		
		GetBuddyAttrMgr().RefreshRuneAttrMap(this);
		GetBuddyAttrMgr().RefreshHeroAllAttributeMap(pkOwnerPlayer, this);
		//刷新此英雄 提供的灵魂链属性
		GetBuddyAttrMgr().RefreshHeroGiveToOtherCoreAttrMap(pkOwnerPlayer);
		//刷新灵魂链上玩家获得灵魂链属性
		pkOwnerPlayer->RefreshChainBuddyCoreAttrByOnChain(GetTemplateID(), false, SPART_Buddy_7);
		GetBuddyAttrMgr().RefreshNewFinalAttrMap(pkOwnerPlayer, this, false, SPART_Buddy_7);

		if(IsBind())
		{
			pkOwnerPlayer->BroadcastBuddyRuneStarLevel(GetAssignedPlayerId(), GetTemplateID(), slotIndex, itemID, starLevel);
		}
	}

	return res;
}

int32_t BuddyInstance::Update_SkillAsset(float fCurrentTime, float fDeltaTime)
{
	m_Data.m_Data_PlayerBase.m_kSkillAsset.Update(fCurrentTime, fDeltaTime);
	return true;
}


int BuddyInstance::CheckActivateSkill( int32_t nSkillId )
{
	LC_SkillAsset* pkSkillAsset = GetSkillAsset();
	if (NULL == pkSkillAsset)
	{
		return -1;
	}
	LC_SkillAssetData* pSkillData =  pkSkillAsset->GetSkillAssetData();

	int32_t nIndex = pkSkillAsset->GetEntryIndexInAsset(nSkillId);
	LC_SkillAssetEntry* pSkillEntry= pSkillData->GetSkillAssetEntry((skill_cat_type)SKILL_TYPE_SECT, nIndex);
	if (pSkillEntry == NULL)
	{
		return -1;
	}

	return (pSkillEntry->GetLogicValue() > 0);
}

int32_t BuddyInstance::OnActivateSkill_LearnSkill( int32_t nSkillId )
{
	int nSlot = -1;
	LearnSkill(nSkillId, nSlot);
	return true;
}

int32_t BuddyInstance::OnAddSkill(int nSkillId)
{
	GetSkillCoolDownMap()->BuildIndexToIDCache( nSkillId );
	return true;
}

void BuddyInstance::OnActivateSkill( int32_t nSkillId )
{
	LC_ServerPlayer* pOwner = GetOwnerPlayerByBuddyInstance();
	if ( pOwner == NULL )
		return;
	pOwner->OnIncActivatedSkillCount();
	OnIncActivatedSkillCount();
	//SyncBuddyRuneStarLevelInfo();
}

int32_t BuddyInstance::GetTotalActivatedSkillCount()
{
	return m_Data.m_Data_Base.m_nTotalActivatedSkillCount;
}

void BuddyInstance::InitSoloDuleBuddyData(LC_ServerPlayer* pkPlayer, int buddyID, int buddyLevel, int buddyStarLevel, int propertyID)
{
	if(NULL == pkPlayer)
	{
		return;
	}

	CF_Buddy::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_Buddy>(buddyID);
	if(NULL == pkData)
		return;
	m_pConfig = pkData;
	m_BuddyAttr.init(m_pConfig);
	m_Data.Init(m_pConfig->_cID);

	m_Data.m_Data_PlayerBase.m_kSkillAsset.SetOwnerBuddyID(m_pConfig->_cID);
	m_Data.m_Data_PlayerBase.m_kSkillAsset.PostInit();

	m_Data.m_Data_ActorBase.m_lLevel = buddyLevel;
	m_Data.m_Data_Base.m_nStarLevel = buddyStarLevel;

	InitSoloDuleBuddyAttrMap(pkPlayer, propertyID);
	attr_value_type nMaxHPValue = GetFinalAttributeMap()->GetAttribute(ATT_MAX_HP);
	SetHP(nMaxHPValue);
	SetMP((int)GetFinalAttributeMap()->GetAttribute(ATT_MAX_MP));
	InitSkillData();
}

void BuddyInstance::InitSoloDuleBuddyAttrMap(LC_ServerPlayer* pkPlayer, int32_t propertyID)
{
	if(NULL == pkPlayer)
	{
		return;
	}

	// 这里只初始化 机器人属性
	CF_ExportSoloDuleRobotProperty::DataEntry* pkPropertyData = SafeGetCSVFileDataEntryPtr<CF_ExportSoloDuleRobotProperty>(propertyID);
	if(NULL == pkPropertyData)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "CF_ExportSoloDuleRobotProperty propertyID:[%d] error1\n", propertyID);
		return;
	}
	LC_ActorAttributeMap* pkRobotAttrMap = GetNewFinalAttributeMap();

	int cirDAMDate = pkPropertyData->_lCirDAM<10000 ? 10000 : pkPropertyData->_lCirDAM;
	pkRobotAttrMap->SetAttributeBase(ATT_MAX_HP,	pkPropertyData->_lMaxHP);
	pkRobotAttrMap->SetAttributeBase(ATT_MAX_ATK, pkPropertyData->_lMaxPHA);
	pkRobotAttrMap->SetAttributeBase(ATT_DEF, pkPropertyData->_lPHD);
	pkRobotAttrMap->SetAttributeBase(ATT_CIR_DEF, pkPropertyData->_lCirDefAdd);
	pkRobotAttrMap->SetAttributeBase(ATT_DOG_RATE, pkPropertyData->_lDogRATE);
	pkRobotAttrMap->SetAttributeBase(ATT_CIR_RATE, pkPropertyData->_lCirRATE);
	pkRobotAttrMap->SetAttributeBase(ATT_CIR_DAM, cirDAMDate);
	pkRobotAttrMap->SetAttributeBase(ATT_ATK_RATE, pkPropertyData->_lATKRateAdd);
	pkRobotAttrMap->SetAttributeBase(ATT_DMG_RATE, pkPropertyData->_lDMG_RATE);
	pkRobotAttrMap->SetAttributeBase(ATT_DMG_DERATE, pkPropertyData->_lDMG_DERATE );
	pkRobotAttrMap->SetAttributeBase(ATT_IGNORE_DEF, pkPropertyData->_lIGNORE_DEF );
	pkRobotAttrMap->SetAttributeBase(ATT_DMG_REBOUND, pkPropertyData->_lDMG_REBOUND);
	pkRobotAttrMap->SetAttributeBase(ATT_CRI_DMG_DERATE, pkPropertyData->_lCRI_DMG_DERATE);
	pkRobotAttrMap->SetAttributeBase(ATT_HP_HURT, pkPropertyData->_HpHurt);
	pkRobotAttrMap->SetAttributeBase(ATT_MOVE_SPEED, (int)CHAR_MOVE_SPEED);
	pkRobotAttrMap->RecomputeAll();

	GetBuddyAttrMgr().SetBuddyCombatScoreByType(pkPlayer, BCST_FinalBuddy,pkPropertyData->combat_score);
}

int BuddyInstance::InitTemplateData(const CSVFile::CF_Buddy::DataEntry* pData, bool bInit)
{
	if (pData == NULL)
		return false;

	ResetAttrSyncFlag();

	m_pConfig = const_cast<CSVFile::CF_Buddy::DataEntry*>(pData);
	m_Data.Init(m_pConfig->_cID);
	m_BuddyAttr.init(m_pConfig);

	m_Data.m_Data_PlayerBase.m_kSkillAsset.SetOwnerBuddyID(m_pConfig->_cID);
	m_Data.m_Data_PlayerBase.m_kSkillAsset.PostInit();

	int32_t lLevel = 1;
	CF_LevelUp* pkCSVLevelUp = CF_LevelUp::GetSingletonPtr();
	const CF_LevelUp::DataEntry* pkLevelData = pkCSVLevelUp->GetEntryPtr(lLevel);
	if (pkLevelData == NULL)
		return false;
	/*
	_lMaxPHA *
	_lMaxHP *
	_lPHD *
	_lATKRateAdd *
	_lDogRATE *
	_lCirRATE *
	_lCirDefAdd *
	*/
	int nStarLevel = 0;
	if (bInit)
	{
		nStarLevel = m_pConfig->_InitLevel;
		m_Data.m_Data_Base.m_nStarLevel = m_pConfig->_InitLevel;
		ActiveBuddyTransform(m_pConfig->_iDefaultTransform, false, false, true);
		SetBuddyCurrentTransformID(m_pConfig->_iDefaultTransform, false, false);	
	}

	const CF_BuddyUpgrade::DataEntry* pCurStarData = this->UpdateBuddyUpgradeData(m_pConfig->_cID, nStarLevel);
	if (pCurStarData == NULL)
		return false;
	int nSlot = -1;
	LearnSkill(pCurStarData->_unlockskill, nSlot, false);
	LC_ServerPlayer* pOwner = GetOwnerPlayerByBuddyInstance();
	if(NULL == pOwner)
		return false;
	attr_value_type nMaxHPValue = GetFinalAttributeMap()->GetAttribute(ATT_MAX_HP);
	// need a flag also
	// 非升级
	//if (!bInit)
	SetHP(nMaxHPValue);
	SetMP((int)GetFinalAttributeMap()->GetAttribute(ATT_MAX_MP));

	InitSkillData();
	return true;
}


bool BuddyInstance::InitSkillData()
{
	if (m_pConfig == NULL)
		return false;

	return InitBuddySkills( m_pConfig->_skillid );
}

bool BuddyInstance::InitBuddySkills(const StringType& sRawSkills)
{
	Utility::UT_SIMDataList kRawSkills;
	Utility::ParseFmtString2DataIFs(sRawSkills, kRawSkills);
	return InitBuddySkills(kRawSkills);
}

bool BuddyInstance::InitBuddySkills(Utility::UT_SIMDataList& kRawSkills)
{
	bool bResult = false;
	bool tag = GetLevelUpFlag();
	Utility::UT_SIMDataList::iterator it = kRawSkills.begin();
	for (; it != kRawSkills.end(); ++it)
	{
		UT_SIMDataInfo info = *it;
		int32_t nSkillID = info.GetSkillID();
		int32_t nSkillEnableLevel = info.GetSkillEnableLevel();
		int32_t nSlot = -1;
		if ( GetLevel() >= nSkillEnableLevel )
		{
			LearnSkill(nSkillID, nSlot, tag);
			bResult = true;
		}
	}
	//AutoSkillLevelUp();
	if(tag)
	{
		SetLevelUpFlag(false);
	}
	return bResult;
}

void BuddyInstance::OnLoadSkillData()
{	
	LC_SkillAsset* pkSkillAsset = GetSkillAsset();
	if (NULL == pkSkillAsset)
		return;

	LC_SkillAssetData* pSkillAssistData = pkSkillAsset->GetSkillAssetData();

	const LC_SkillCategoryMap& kCatagoryMap = pSkillAssistData->GetSkillCatagoryMap();
	for(LC_SkillCategoryMap::const_iterator cit = kCatagoryMap.begin(); cit!=kCatagoryMap.end(); ++cit)
	{
		const LC_SkillCategoryData& rCategoryData = cit->second;
		const LC_SkillIDVector& skillVec = rCategoryData.GetSkillSlots();
		for (int i = 0; i<(int)skillVec.size(); ++i)
		{
			const LC_SkillAssetEntry* pEntry = rCategoryData.GetSkillAssetEntry(i);
			if(NULL == pEntry)
			{
				return;
			}
		}
	}
}

bool BuddyInstance::LearnSkill(int lSkillID, int& iSlotIdx, bool send, int32_t runeSystemType, int32_t runeSystemParam, GameLogic::LC_SkillIDSet* pkReplacedSkills, bool bSyncLearnSkillEvent)
{
	if (!IS_SKILL_TYPE(lSkillID))
		return false;

	LC_ServerPlayer* m_pOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if ( m_pOwnerPlayer == NULL )
		return false;
	LC_ServerPlayer* m_pSelfPlayer = GetAssignedPlayer();

	int skillID = m_Data.m_Data_PlayerBase.m_kSkillAsset.GetAlreadyLearnSkillID(lSkillID);

	//已经学会了该等级的技能  或者学会了更高等级的该类技能
	if(skillID > lSkillID)
	{
		return false;
	}
	// a little ugly
	bool bRefreshStateOnly = false;
	if (skillID == lSkillID)
		bRefreshStateOnly = true;

	

	LC_SkillIDSet kSkillIDSet;
	ResultType result = m_Data.m_Data_PlayerBase.m_kSkillAsset.AddSkillToAsset(lSkillID, iSlotIdx, &kSkillIDSet, 0, DEFAULT_SKILL_USABLE_COUNT, this);
	if(result == RE_SKILL_IS_LEARNED && !bRefreshStateOnly)
		return true;
	if (result != RE_SUCCESS && !bRefreshStateOnly)
		return false;

	//如果是被动技能，立即激活
	SK_SkillExecute* pkSkillExecute = SK_Factory::GetSingletonPtr()->RequestSkillExcute(lSkillID);
	if (pkSkillExecute == NULL)
		return false;
	CF_SkillSpecialAttrib::DataEntry* pkSkillAttrib = pkSkillExecute->GetAttrDataEntryPtr();
	CF_SkillExecuteList::DataEntry* pkSkillData = pkSkillExecute->GetDataEntryPtr();
	if (pkReplacedSkills)
	{
		*pkReplacedSkills = kSkillIDSet;
	}
	int32_t shorcutidx = -1;
	LC_SkillIDSet::iterator it = kSkillIDSet.begin();
	for (; it != kSkillIDSet.end(); ++it)
	{
		shorcutidx = GetShortcutBar()->FindFirstEntryIndexByTypeID(*it);
		if (shorcutidx >= 0)
		{
			break;
		}
	}
	if (pkSkillData->_lPosNegType == SPNT_NEGTIVE)
	{
		{
			if (pkSkillExecute->HasPassive())
			{
				pkSkillExecute->StartPassive(m_pOwnerPlayer);
			}
			else
			{
				//if ( add to buddy )
				if ( !pkSkillData->_bAddToController )
				{
					if (m_pSelfPlayer)
					{
						m_pSelfPlayer->ActiveSkillState(
							//m_Data.m_Data_ActorBase->m_kSkillStateMap->act
							pkSkillAttrib->_lSkillConstParameter1, 
							0, m_nOwnerPlayerIdByBuddyInstance, 
							Utility::Int2Float(pkSkillAttrib->_lSkillConstParameter2), 
							Utility::Int2Float(pkSkillAttrib->_lSkillConstParameter3), 
							Utility::Int2Float(pkSkillAttrib->_lSkillConstParameter4), 
							Utility::Int2Float(pkSkillAttrib->_lSkillConstParameter5), 
							Utility::Int2Float(pkSkillAttrib->_lSkillConstParameter6)
							);
					}
					StateCacheData cache;
					cache.lStateTypeID = pkSkillAttrib->_lSkillConstParameter1;
					cache.lStateAttacherID = m_nOwnerPlayerIdByBuddyInstance;
					cache.nPara0 = pkSkillAttrib->_lSkillConstParameter2;
					cache.nPara1 = pkSkillAttrib->_lSkillConstParameter3;
					cache.nPara2 = pkSkillAttrib->_lSkillConstParameter4;
					cache.nPara3 = pkSkillAttrib->_lSkillConstParameter5;
					cache.nPara4 = pkSkillAttrib->_lSkillConstParameter6;
					m_Data.m_Data_ActorBase.m_kStateCache.insert(std::make_pair<int, StateCacheData>(cache.lStateTypeID, cache));
				}
				//else add to controller
				else
				{
					m_pOwnerPlayer->ActiveSkillStateToAllBuddyInstance(
						pkSkillAttrib->_lSkillConstParameter1, 
						0, m_nOwnerPlayerIdByBuddyInstance, 
						Utility::Int2Float(pkSkillAttrib->_lSkillConstParameter2), 
						Utility::Int2Float(pkSkillAttrib->_lSkillConstParameter3), 
						Utility::Int2Float(pkSkillAttrib->_lSkillConstParameter4), 
						Utility::Int2Float(pkSkillAttrib->_lSkillConstParameter5), 
						Utility::Int2Float(pkSkillAttrib->_lSkillConstParameter6)
						);
				}
			}
		}
	}
	else
	{
		if (shorcutidx >= 0)
		{
			GetShortcutBar()->ShortcutSkill(shorcutidx / MAX_SINGLE_SHORTCUTBAR_SLOT_COUNT, shorcutidx % MAX_SINGLE_SHORTCUTBAR_SLOT_COUNT, lSkillID, true);
		}
		else if (pkSkillData->_AutoShortcutIdx > 0)
		{
			LC_ShortcutEntry* entry = GetShortcutBar()->GetShortcutEntry(pkSkillData->_AutoShortcutPage, pkSkillData->_AutoShortcutIdx - 1);
			if (entry && !entry->GetValid())
			{
				GetShortcutBar()->ShortcutSkill(pkSkillData->_AutoShortcutPage, pkSkillData->_AutoShortcutIdx - 1, lSkillID);
			}
		}
	}

	//GetShortcutBar()->RefreshShortcutBar();
	//重新生成CD表
	GetSkillAsset()->BuildSkillCoolDownMap();
	//刷新buddy要上榜的基础战力
	m_BuddyAttr.RefreshBuddyRankCombatScore(this);
	// 变身技能，暂时屏蔽，weixin
	if (bSyncLearnSkillEvent && !bRefreshStateOnly && pkSkillData->_bPrompt == 1 && pkSkillData->_iID!= GetGlobalSetting.iSprintSkillID && send)
	{
		LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
		LC_GameEvent* pkGameEvent = pkEventManager->AllocGameEvent();
		if (NULL != pkGameEvent)
		{
			pkGameEvent->SetEventType(GLET_LEARN_SKILL_END);
			pkGameEvent->SetEventParam(EVENT_SEND_TYPE_PARAM, GENT_SELF);
			pkGameEvent->SetEventParam(EP_LEARN_SKILL_PLAYER_ID, m_nOwnerPlayerIdByBuddyInstance);
			pkGameEvent->SetEventParam(EP_LEARN_SKILL_ID, lSkillID);
			pkGameEvent->SetEventParam(EP_LEARN_SKILL_SLOT, iSlotIdx);
			pkGameEvent->SetEventParam(EP_LEARN_BUDDY_ID, m_pConfig->_cID);
			//EP_LEARN_SKILL_STATE		= EVENT_BASE_PARAM_5,//0升级  1学会  
			pkGameEvent->SetEventParam(EP_LEARN_SKILL_STATE, 1);
			pkEventManager->InsertGameEvent(pkGameEvent);
		}
	}
	return true;
}

bool BuddyInstance::RemoveSkill(int lSkillID)
{
	if (IS_SKILL_TYPE(lSkillID))
	{
		LC_ServerPlayer* m_pOwnerPlayer = GetOwnerPlayerByBuddyInstance();
		LC_ServerPlayer* m_pSelfPlayer = GetAssignedPlayer();


		if (RE_SUCCESS == m_Data.m_Data_PlayerBase.m_kSkillAsset.DeleteSkillFromAsset(lSkillID))
		{
			//如果是被动技能，立即激活
			SK_SkillExecute* pkSkillExecute = SK_Factory::GetSingletonPtr()->RequestSkillExcute(lSkillID);
			if (pkSkillExecute == NULL)
			{
				return false;
			}
			CF_SkillExecuteList::DataEntry* pkSkillData = pkSkillExecute->GetDataEntryPtr();
			if (pkSkillData->_lPosNegType == SPNT_NEGTIVE)
			{
				if (m_pOwnerPlayer)
				{
					if (pkSkillExecute->HasPassive())
					{
						pkSkillExecute->StartPassive(m_pOwnerPlayer);
					}
					else
					{
						CF_SkillSpecialAttrib::DataEntry* pkAttrib = pkSkillExecute->GetAttrDataEntryPtr();
						if(pkAttrib)
						{
							if (m_pSelfPlayer)
								m_pSelfPlayer->DeactiveSkillState(pkAttrib->_lSkillConstParameter1);
							m_Data.m_Data_ActorBase.m_kStateCache.erase(pkAttrib->_lSkillConstParameter1);
						}
					}
				}
			}
			//删除该CD信息
			GetSkillCoolDownMap()->RemoveSkillCDSlot(lSkillID);
			return true;
		}
	}
	return true;
}

int64_t BuddyInstance::GetDetalExp()
{
	int64_t result = 0;
	LC_ServerPlayer* m_pOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if (m_pOwnerPlayer == NULL)
		return result;

	CF_LevelUp* pkCSVLevelUp = CF_LevelUp::GetSingletonPtr();
	int max_level = pkCSVLevelUp->GetCurrentMaxID();
	int CurrentLevel = GetLevel();
	int OwnerLevel = m_pOwnerPlayer->GetLevel();

	const CF_LevelUp::DataEntry* pkData = NULL;
	for(int i=CurrentLevel; i<=OwnerLevel+1&&i<max_level; ++i)
	{
		pkData = pkCSVLevelUp->GetEntryPtr(i);
		if(NULL != pkData)
		{
			result += pkData-> _lEXPNeed;
		}
	}

	int64_t currentBuddyExp = m_Data.m_Data_ActorBase.m_lEXP;
	result -= currentBuddyExp;
	return result;
}

void BuddyInstance::RefreshBuddyEquipAttrMap()
{
	return GetEquipPack().UpdateAttributeMap();
}

CBuddyEquipPack& BuddyInstance::GetEquipPack()
{
	return m_Data.m_Data_PlayerBase.m_BuddyEquipPack;
}

int32_t BuddyInstance::GetEquipSlotLevelBySlotIndex(int slotIndex)
{
	return GetEquipPack().GetEquipSlotLevelBySlotIndex(slotIndex);
}

int32_t BuddyInstance::GetEquipSlotTotalLevel()
{
	return GetEquipPack().GetEquipSlotTotalLevel();
}

int32_t BuddyInstance::GetEquipSlotStarLevelBySlotIndex(int slotIndex)
{
	return GetEquipPack().GetEquipSlotStarLevelBySlotIndex(slotIndex);
}

int32_t BuddyInstance::GetEquipSlotTotalStarLevel()
{
	return GetEquipPack().GetEquipSlotTotalStarLevel();
}

void BuddyInstance::GetEquipInfo(std::map<int32_t,PS_EquipItemInfo_BuddyCS>& info)
{
	GetEquipPack().GetEquipInfo(info);
}

int32_t BuddyInstance::GetEquipGradeEquipCount(int32_t itemGrade)
{
	return GetEquipPack().GetEquipGradeEquipCount(itemGrade);
}

bool BuddyInstance::UpdataEquipSlotLevel(int slotIndex, int slotLevel)
{
	return GetEquipPack().SetEquipSlotLevel(slotIndex, slotLevel);
}

bool BuddyInstance::UpdataEquipSlotStarLevel(int slotIndex, int slotStarLevel)
{
	return GetEquipPack().SetEquipSlotStarLevel(slotIndex, slotStarLevel);
}

ResultType BuddyInstance::BuyGrowthPackage()
{
	if(m_Data.m_Data_PlayerBase.m_nBeActived)
	{
		return RE_BUDDY_ALREADY_ACTIVE_GROWTH_PACKAGE;
	}

	LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if(NULL == pkOwnerPlayer)
	{
		return RE_FAIL;
	}

	CF_Buddy::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_Buddy>(m_pConfig->_cID);
	if(NULL == pkData)
	{
		return RE_FAIL;
	}

	CF_ShopList::DataEntry* pkShopData  = CF_ShopList::GetSingletonPtr()->GetEntryPtr(pkData->_kFireShopIDs);
	if(pkShopData == NULL) 
	{
		return RE_FAIL;
	}

	ResultType res =  LC_Helper::CheckCashMap(pkOwnerPlayer, pkShopData->_ActualPriceList);
	if(RE_SUCCESS != res)
	{
		return res;
	}

	res = LC_Helper::PayCashMap(pkOwnerPlayer, pkShopData->_ActualPriceList, SYS_FUNC_TP_BUDDY_BUY_GROWTH_PACKAGE, m_pConfig->_cID);
	if(RE_SUCCESS != res)
	{
		return res;
	}

	m_Data.m_Data_PlayerBase.m_nBeActived = true;
	m_Data.m_Data_PlayerBase.m_nActiveTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	m_Data.m_Data_PlayerBase.m_nDay = 0;

	return RE_SUCCESS;
}

ResultType BuddyInstance::GetGrowthReward()
{
	if(!m_Data.m_Data_PlayerBase.m_nBeActived)
	{
		return RE_BUDDY_NOT_ACTIVE_GROWTH_PACKAGE;
	}

	int currentDay = LC_Helper::GetDeltaDay(m_Data.m_Data_PlayerBase.m_nActiveTime, GET_CURRENT_TIMESTAMP_IN_SECONDS())+1;
	if(m_Data.m_Data_PlayerBase.m_nDay==30)
	{
		return RE_BUDDY_GROWTH_PACKAGE_FINISHED;
	}

	if(currentDay < m_Data.m_Data_PlayerBase.m_nDay)
	{
		return RE_BUDDY_GROWTH_PACKAGE_TIME_INVALID;
	}
	else if(currentDay == m_Data.m_Data_PlayerBase.m_nDay)
	{
		return RE_BUDDY_GROWTH_REWARD_HAS_RECEIVED;
	}

	currentDay = currentDay>30 ? 30 : currentDay;

	ResultType res = RE_FAIL;
	LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if(NULL == pkOwnerPlayer)
	{
		return res;
	}

	CF_BuddyGrowGift::DataEntryMapExternIterator kIter = CF_BuddyGrowGift::GetSingletonPtr()->GetIterator();
	while(kIter.HasMoreElements())
	{
		CF_BuddyGrowGift::DataEntry* pkEntry = kIter.PeekNextValuePtr();

		bool flag = true;
		for(int i=m_Data.m_Data_PlayerBase.m_nDay+1; i<=currentDay; ++i)
		{
			if(pkEntry->_cID==m_pConfig->_cID && pkEntry->_iDay==i)
			{
				StringType mailTitle;
				StringType mailDesc;
				res = UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, pkEntry->_kReward, pkOwnerPlayer, SYS_FUNC_TP_GET_BUDDY_GROWTH_REWARD, pkEntry->ID, mailTitle, mailDesc, false);
				if(RE_SUCCESS != res)
				{
					flag = false;
					break;
				}
				else
				{
					m_Data.m_Data_PlayerBase.m_nDay = i;
				}
			}
		}

		//背包满了，则直接break;
		if(!flag)
		{
			res = RE_BUDDY_GROWTH_REWARD_PACKET_HAS_FULL;
			break;
		}

		kIter.MoveNext();
	}	

	return res;
}

void BuddyInstance::SetDataToProtocolStruct(PS_SingleBuddyGrowthPackage& msg)
{
	msg.m_nBeActived = m_Data.m_Data_PlayerBase.m_nBeActived;
	if(msg.m_nBeActived)
	{
		msg.m_nBuddyID = m_pConfig->_cID;
		msg.m_nDay = m_Data.m_Data_PlayerBase.m_nDay;

		int currentDay = LC_Helper::GetDeltaDay(m_Data.m_Data_PlayerBase.m_nActiveTime, GET_CURRENT_TIMESTAMP_IN_SECONDS())+1;
		if(currentDay < msg.m_nDay)
		{
			GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "PS_SingleBuddyGrowthPackage time error! buddyID:[%d]  deltaDay:[%d]", msg.m_nBuddyID, currentDay);
		}

		msg.m_nCurrentDay = currentDay>30 ? 30 : currentDay;
		if(m_Data.m_Data_PlayerBase.m_nDay==30)
		{
			msg.m_nHasFinished = true;
		}
		else
		{
			msg.m_nHasFinished = false;
		}
	}
}

void BuddyInstance::InitInstanceData()
{
	InitInstanceData_ActorBase();
	InitInstanceData_ServerPlayer();
}

LC_ServerPlayer* BuddyInstance::GetOwnerPlayerByBuddyInstance()
{
	if ( m_pOwnerPlayer_Cache == NULL )
	{
		LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
		m_pOwnerPlayer_Cache = (LC_ServerPlayer*)pkWorldManager->FindObject(m_nOwnerPlayerIdByBuddyInstance);
	}
	return m_pOwnerPlayer_Cache;
}

LC_ServerPlayer* BuddyInstance::GetAssignedPlayer()
{
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerPlayer* pPlayer = (LC_ServerPlayer*)pkWorldManager->FindObject(m_nAssignedPlayerId);
	return pPlayer;
}

object_id_type BuddyInstance::GetAssignedPlayerId()
{
	return m_nAssignedPlayerId;
}

void BuddyInstance::SetLockAssignedPlayerId( bool bFlag )
{
	m_Data.m_Data_Base.m_bLockAssignedPlayerId = bFlag;
}

void BuddyInstance::FillBuddyCSInfo(PS_Buddy_Info_BuddyCS& buddyData)
{
	buddyData.m_nBuddyID = GetTemplateID();
	buddyData.m_nBuddyLevel = GetLevel();
	buddyData.m_nBuddyStarLevel = GetStarLevel();
	buddyData.m_nBuddyCombatScore = GetBuddyAttrMgr().GetBuddyCombatScoreByType(BCST_FinalBuddy);
	buddyData.m_nBuddyTransformID = GetBuddyCurrentTransformID();
	buddyData.m_bActivateAllSkill = GetTotalActivatedSkillCount()==4 ? true:false;	
	buddyData.m_nAllEquipSlotReinforceLevel = GetEquipSlotTotalLevel();
	GetBuddyRuneInfo(buddyData.m_nBuddyRuneInfo);
	GetEquipInfo(buddyData.m_nEquipSlotInfo);
}

void BuddyInstance::SyncAllDataToClient()
{
	SyncBaseData();
	SyncBuddyEquipSlotPack();
	SyncSkillAsset();
	SyncShortcutBar();
	SyncEquipGradeLevel();
}

void BuddyInstance::SyncBuddyEquipSlotData(int slotIndex)
{
	LC_ServerPlayer* m_pOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if (m_pOwnerPlayer == NULL)
		return;

	MG_RLT_BuddyEquipSlotData msg;
	msg.m_nBuddyID = GetTemplateID();
	msg.m_nSlotIndex = slotIndex;
	GetEquipPack().EquipSlotToEquipBaseByIndex(slotIndex, msg.m_EquipSlotData);
	m_pOwnerPlayer->SendMsgToClient(MGPT_SYNC_BUDDYEQUIPSLOTDATA, &msg, m_pOwnerPlayer);
}

void BuddyInstance::SyncBuddyEquipSlotPack()
{
	LC_ServerPlayer* m_pOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if (m_pOwnerPlayer == NULL)
		return;

	MG_RLT_BuddyEquipSlotPackData msg;
	msg.m_nBuddyID = GetTemplateID();
	GetEquipPack().EquipSlotToEquipBase(msg.m_EquipData);
	m_pOwnerPlayer->SendMsgToClient(MGPT_SYNC_BUDDYEQUIPSLOTPACKDATA, &msg, m_pOwnerPlayer);
}
void BuddyInstance::SyncEquipGradeLevel()
{
	LC_ServerPlayer* m_pOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if (m_pOwnerPlayer == NULL)
		return;

	MG_SyncEquipSlotLv msgEquipGradeLevel;
	msgEquipGradeLevel.m_nLv = GetEquipPack().GetEquipGradeLevel();
	msgEquipGradeLevel.m_nBuddyId = m_pConfig->_cID;
	m_pOwnerPlayer->SendMsgToClient(MGPT_SYNC_EQUIP_SLOT_LV, &msgEquipGradeLevel, m_pOwnerPlayer);
}
void BuddyInstance::SyncSkillAsset(bool bForce)
{
	LC_ServerPlayer* m_pOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if (m_pOwnerPlayer == NULL)
		return;
	bool bFlag = m_Data.m_Data_PlayerBase.m_kSkillAsset.GetDirtyFlag();
	if (bFlag == false && bForce == false)
		return;

	m_Data.m_Data_PlayerBase.m_kSkillAsset.ClearDirtyFlag();
	MG_RLT_SkillBackPack data;
	data.m_iBuddyID = m_pConfig->_cID;
	m_Data.m_Data_PlayerBase.m_kSkillAsset.SetDataToProtocolStruct(data.m_kCharSkillInfo);
	m_pOwnerPlayer->GetControllerSkillAsset()->SetDataToProtocolStruct(data.m_kCharSkillInfo);
	m_pOwnerPlayer->SendMsgToClient(MGPT_SYNC_SKILL_BACKPACK, &data, m_pOwnerPlayer);
}

void BuddyInstance::SyncShortcutBar()
{
	LC_ServerPlayer* m_pOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if (m_pOwnerPlayer == NULL)
		return;

	MG_SyncShortcutBar data;
	data.m_lPlayerID = m_nOwnerPlayerIdByBuddyInstance;
	data.m_nBuddyID = m_pConfig->_cID;
	m_Data.m_Data_PlayerBase.m_pkShortcutBar->SetDataToProtocolStruct(data.m_akShortcutSlots);

	//data.
	m_pOwnerPlayer->SendMsgToClient(MGPT_SYNC_SHORTCUTBAR_INFO, &data, m_pOwnerPlayer);
}
//////////////////
void BuddyInstance::SyncBuddyAttributes(uint32_t uSyncType,LC_ActorAttributeMap&SyncAttributeMap, bool login, uint64_t nCombatScore, int32_t ReasonType)
{
	LC_ServerPlayer* pOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if (NULL == pOwnerPlayer)
		return;
	if (SyncAttributeMap.GetAttribute(ATT_MAX_HP) <= 0)
		return;
	MG_SyncPlayerAttribute data;
	msg_define::player_attribute_info& sync_value = data.m_value;
	sync_value.set_login(login);
	sync_value.set_reasontype(ReasonType);
	sync_value.set_id(m_nOwnerPlayerIdByBuddyInstance);
	sync_value.set_career(m_Data.m_Data_PlayerBase.m_eCareerType);
	sync_value.set_weapon(m_Data.m_Data_ActorBase.m_eWeaponAttribute);
	sync_value.set_flag(m_Data.m_Data_PlayerBase.m_uiPlayerFlag);
	sync_value.set_buddy(m_pConfig->_cID);
	sync_value.set_usynctype(uSyncType);
	sync_value.set_ucombatscore(nCombatScore);
	uint64_t score = pOwnerPlayer->GetCommanderAttrMgr().GetCommanderCombatScoreByType(CST_ControllerEquip);
	sync_value.set_ucommanderequipscore(score);
	SyncAttributeMap.SetDataToPB(*sync_value.mutable_attrs());
	std::stringstream logStr;
	logStr<<"<"<<uSyncType<<">";
	SyncAttributeMap.PrintAttrMap(logStr.str());
	pOwnerPlayer->SendMsgToClient(data.GetMessageID(), &data, pOwnerPlayer);
}

void BuddyInstance::SyncBaseData()
{
	LC_ServerPlayer* m_pOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if (m_pOwnerPlayer == NULL)
		return;

	MG_SC_BuddyBaseDataSync data;
	//data.
	data.m_Data.nBuddyTemplateId = m_pConfig->_cID;
	data.m_Data.nExp = m_Data.m_Data_ActorBase.m_lEXP;
	data.m_Data.nHP = m_Data.m_Data_ActorBase.m_lHP;
	data.m_Data.nLevel = m_Data.m_Data_ActorBase.m_lLevel;
	data.m_Data.nStarLevel = m_Data.m_Data_Base.m_nStarLevel;
	data.m_Data.nCurrentTransformID = m_Data.m_Data_Base.m_nCurrentTransformID;
	
	int size1 = m_Data.m_Data_Base.m_nActiveTransformRecord.size();
	if(size1 > 0)
	{
		data.m_Data.nActiveTransformRecord.reserve(size1);
		for(int i=0; i<size1; ++i)
		{
			data.m_Data.nActiveTransformRecord.push_back(m_Data.m_Data_Base.m_nActiveTransformRecord[i]);
		}
	}
	
	int size2 = m_Data.m_Data_Base.m_nInvaildTransformTime.size();
	if(size2 > 0)
	{
		data.m_Data.m_nBuddyTransformInvaildTime.reserve(size2);
		for(int i=0; i<size2; ++i)
		{
			data.m_Data.m_nBuddyTransformInvaildTime.push_back(m_Data.m_Data_Base.m_nInvaildTransformTime[i]);
		}
	}

	m_pOwnerPlayer->SendMsgToClient(MGPT_SC_BUDDY_BASE_DATA_SYNC, &data, m_pOwnerPlayer);
}


LC_SkillAsset* BuddyInstance::GetSkillAsset()
{
	return &(m_Data.m_Data_PlayerBase.m_kSkillAsset);
}

LC_ShortcutBar* BuddyInstance::GetShortcutBar()
{
	return m_Data.m_Data_PlayerBase.m_pkShortcutBar;
}

LC_SkillCoolDownMap* BuddyInstance::GetSkillCoolDownMap()
{
	return GetSkillAsset()->GetSkillCoolDownMap();
}

void BuddyInstance::BuildSkillCoolDownMap()
{
	int lShapeShiftTypeID = GetShapeShiftCharTypeID();
	if (lShapeShiftTypeID  > 0)
	{
		CF_CharType* pkCharType = CF_CharType::GetSingletonPtr();
		CF_CharType::DataEntry* pkCharData = pkCharType->GetEntryPtr(lShapeShiftTypeID);
		if (pkCharData == NULL)
		{
			return;
		}
		LC_SkillCoolDownMap* pkSkillCoolDownMap = GetSkillCoolDownMap();
		for (int i = 0; i < 5; ++i)
		{
			int lAttackSkillID = CSVHelper::GetCharSkillID(pkCharData, i + 1);
			if (IS_SKILL_TYPE(lAttackSkillID))
			{
				pkSkillCoolDownMap->AddSkillCDSlot(lAttackSkillID);
			}
		}
	}
	GetSkillAsset()->BuildSkillCoolDownMap();
}

ResultType BuddyInstance::UpdateSkillInSkillAsset_PlayerBase(int iCategory, int& iEntryIndex, int iMax)
{
	LC_ServerPlayer* m_pOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if (m_pOwnerPlayer == NULL)
		return RE_FAIL;

	ResultType ret = RE_SKILL_UPDATE_SUCCESS;
	LC_SkillAssetEntry* pkEntry = m_Data.m_Data_PlayerBase.m_kSkillAsset.GetSkillEntryInAsset(iCategory, iEntryIndex);
	if (!pkEntry || !pkEntry->GetValid())
	{
		return RE_SKILL_SYSTEM_ERROR;
	}

	skill_id_type lSkillID = pkEntry->GetTypeID();
	CF_SkillLevelUp* pkCSVSkillLevelUp = CF_SkillLevelUp::GetSingletonPtr();
	CF_SkillLevelUp::DataEntry* pkData = pkCSVSkillLevelUp->GetEntryPtr(lSkillID);
	if (!pkData)
	{
		return RE_SKILL_SYSTEM_ERROR;
	}

	skill_id_type lNextSkillID = -1;
	int32_t lNeedSp = 0;
	int32_t lNeedCash = 0;
	int32_t iMaxLevel = (iMax>0) ? iMax : 10;

	CF_SkillLevelUp::DataEntry* pkSkillData = pkData;
	ResultType rslt = RE_SUCCESS;
	while (pkSkillData && --iMaxLevel>=0)
	{
		if (!IS_SKILL_TYPE(pkSkillData->_lNextLevelSkillTypeID))
		{
			rslt = RE_SKILL_UPDATE_TO_MAX_LEVEL;
			break;
		}
		if (pkSkillData->_lNeedLevel > GetLevel())
		{
			rslt = RE_SKILL_UPDATE_NEED_LEVEL;
			break;
		}

		if(pkSkillData->_level > GetStarLevel())
		{
			rslt = RE_SKILL_STAR_LEVEL_FAIL;
			break;
		}
		if ( !pkSkillData->_bBreak)
		{
			break;
		}

		if ((uint32_t)(pkSkillData->_lNeedSP+lNeedSp) > m_pOwnerPlayer->GetSkillLevelupPoints())
		{
			rslt = RE_SKILL_UPDATE_NEED_SP;
			break;
		}

		if (pkSkillData->_lNeedCash+lNeedCash > m_pOwnerPlayer->GetOwnerPlayer()->GetUnBindCash().GetMoney())
		{
			rslt = RE_SKILL_UPDATE_NEED_CASH;
			break;
		}

		if (IS_SKILL_TYPE(lNextSkillID) && (!pkSkillData->_kNeedItems.empty() || !pkSkillData->_kNeedTasks.empty() 
			|| !pkSkillData->_kNeedAchievements.empty() || !pkSkillData->_kNeedSkills.empty() || pkSkillData->_lUpgradePro>0))
		{
			break;
		}

		lNeedSp += pkSkillData->_lNeedSP;
		lNeedCash += pkSkillData->_lNeedCash;

		lNextSkillID = pkSkillData->_lNextLevelSkillTypeID;	
		pkSkillData = pkCSVSkillLevelUp->GetEntryPtr(lNextSkillID);
	}

	if (!IS_SKILL_TYPE(lNextSkillID))
	{
		return rslt;
	}
	if(pkData->_level > GetStarLevel())
		return RE_SKILL_STAR_LEVEL_FAIL;
	PackOperateInfoList kConsumeOPs;
	if (!pkData->_kNeedItems.empty())
	{
		LC_Helper::BuildConsumeItemOPs(pkData->_kNeedItems, kConsumeOPs);
		if (RE_SUCCESS != LC_Helper::CheckItemOPs(m_pOwnerPlayer->GetPackAsset(), kConsumeOPs))
		{
			return RE_SKILL_UPDATE_NEED_ITEM;
		}
	}
	if (!pkData->_kNeedTasks.empty())
	{
		LC_TaskMap* pkTaskMap = GetOwnerPlayerByBuddyInstance()->GetTaskMap();
		if (pkTaskMap)
		{
			Utility::UT_SSIMDataList::iterator itTask = pkData->_kNeedTasks.begin();
			for (; itTask != pkData->_kNeedTasks.end(); ++itTask)
			{
				if (!pkTaskMap->IsInFinishedTaskMap(itTask->ID()))
				{
					return RE_SKILL_UPDATE_NEED_PRE_TASK;
				}
			}
		}
	}
	if (!pkData->_kNeedAchievements.empty())
	{
	}
	if (!pkData->_kNeedSkills.empty())
	{
		Utility::UT_SSIMDataList::iterator itSkill = pkData->_kNeedSkills.begin();
		for (; itSkill != pkData->_kNeedSkills.end(); ++itSkill)
		{
			LC_SkillAssetEntry* pSkillEntry = m_Data.m_Data_PlayerBase.m_kSkillAsset.FindLearnedSkill(itSkill->ID(), false);
			if (!pSkillEntry || pSkillEntry->GetTypeID() < (int32_t)itSkill->ID())
			{
				return RE_SKILL_UPDATE_NEED_PRE_SKILL;
			}
		}
	}

	if (!pkData->_kNeedSubLevels.empty())
	{
		//TypeNumList::iterator it = pkData->_kNeedSubLevels.begin();
		//for (; it != pkData->_kNeedSubLevels.end(); ++it)
		//{
		//	if (it->getType() >= 0 && GetSubordinateAsset().GetSubLevel(it->getType()) < (int32_t)it->getNum())
		//	{
		//		return RE_SKILL_UPDATE_NEED_SUB_LEVEL;
		//	}
		//}
	}
	rslt = RE_SKILL_UPDATE_FAIL;
	if (pkData->_lUpgradePro <= 0 || UT_MathBase::RandInRangeInt(0, 10000) <= pkData->_lUpgradePro)
	{
		int32_t iSlotIndex = pkEntry->GetSlotIndex();

		m_Data.m_Data_PlayerBase.m_kSkillAsset.AddSkillToAsset(lNextSkillID, iSlotIndex, NULL, pkEntry->GetLogicValue(), pkEntry->GetUsableCount(), this);
		m_Data.m_Data_PlayerBase.m_pkShortcutBar->UpdateShortcutWhenSkillUpgrading(lSkillID, lNextSkillID);
		iEntryIndex = iSlotIndex;
		rslt = RE_SKILL_UPDATE_SUCCESS;
	}

	m_pOwnerPlayer->ReduceSP(lNeedSp, SYS_FUNC_TP_SKILL_UPGRADE);
	m_pOwnerPlayer->ReduceCash(lNeedCash, CT_UNBIND_CASH, SYS_FUNC_TP_SKILL_UPGRADE, lSkillID);
	if (!kConsumeOPs.empty())
	{
		LC_Helper::DoItemOPs(m_pOwnerPlayer, kConsumeOPs, SYS_FUNC_TP_SKILL_UPGRADE, lSkillID);
	}

	if (RE_SKILL_UPDATE_SUCCESS == rslt)
	{
		UpdateBuddyUpgradeSkill(lNextSkillID);
		SyncSkillAsset();
		SyncShortcutBar();
		GetBuddyAttrMgr().RefreshNewFinalAttrMap(m_pOwnerPlayer, this, false, SPART_None);
	}
	return rslt;

}


ResultType BuddyInstance::UpdateSkillInSkillAsset(int iCategory, int& iEntryIndex, int iMax)
{
	LC_SkillAssetEntry* pkSDEntry = m_Data.m_Data_PlayerBase.m_kSkillAsset.GetSkillEntryInAsset(iCategory, iEntryIndex);
	if (!pkSDEntry || !pkSDEntry->GetValid())
	{
		return RE_SKILL_SYSTEM_ERROR;
	}

	skill_id_type preSkillID = pkSDEntry->GetTypeID();
	CF_SkillLevelUp* pkCSVSkillLevelUp = CF_SkillLevelUp::GetSingletonPtr();
	CF_SkillLevelUp::DataEntry* pkSDData = pkCSVSkillLevelUp->GetEntryPtr(preSkillID);
	if (!pkSDData)
	{
		return RE_SKILL_SYSTEM_ERROR;
	}
	ResultType ret = RE_SKILL_UPDATE_SUCCESS;

	ret = UpdateSkillInSkillAsset_PlayerBase(iCategory, iEntryIndex, iMax);
	//ret = LC_PlayerBase::UpdateSkillInSkillAsset(iCategory, iEntryIndex, iMax);
	if (ret != RE_SKILL_UPDATE_SUCCESS)
	{
		return ret;
	}

	// 开始通知客户端事件
	int lNextSkillID = pkSDData->_lNextLevelSkillTypeID;
	CF_SkillExecuteList::DataEntry* pkNextSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(lNextSkillID);
	if (pkNextSkillData == NULL)
	{
		return ret;
	}

	if (pkNextSkillData->_bPrompt == 1)
	{
		LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
		LC_GameEvent* pkGameEvent = pkEventManager->AllocGameEvent();
		if (NULL != pkGameEvent )
		{
			pkGameEvent->SetEventType(GLET_LEARN_SKILL_END);
			pkGameEvent->SetEventParam(EVENT_SEND_TYPE_PARAM, GENT_SELF);
			pkGameEvent->SetEventParam(EP_LEARN_SKILL_PLAYER_ID, m_nOwnerPlayerIdByBuddyInstance);
			pkGameEvent->SetEventParam(EP_LEARN_SKILL_ID, lNextSkillID);
			pkGameEvent->SetEventParam(EP_LEARN_SKILL_SLOT, iEntryIndex);
			pkGameEvent->SetEventParam(EP_LEARN_BUDDY_ID, m_pConfig->_cID);
			//EP_LEARN_SKILL_STATE		= EVENT_BASE_PARAM_5,//0升级  1学会  
			pkGameEvent->SetEventParam(EP_LEARN_SKILL_STATE, 0);
			pkEventManager->InsertGameEvent(pkGameEvent);
		}
	}
	// 结束通知客户端事件
	//处理被动技能
	LC_SkillAssetEntry* pkEntry = m_Data.m_Data_PlayerBase.m_kSkillAsset.GetSkillEntryInAsset(iCategory, iEntryIndex);
	skill_id_type curskillid = pkEntry ? pkEntry->GetTypeID() : 0;
	if (preSkillID == curskillid)
	{
		return ret;
	}

	SK_SkillExecute* pkSkillExecute = SK_Factory::GetSingletonPtr()->RequestSkillExcute(preSkillID);
	if (pkSkillExecute == NULL)
	{
		return ret;
	}
	CF_SkillSpecialAttrib::DataEntry* pkSkillAttrib = pkSkillExecute->GetAttrDataEntryPtr();
	CF_SkillExecuteList::DataEntry* pkSkillData = pkSkillExecute->GetDataEntryPtr();
#if 0
	if (pkSkillData->_lPosNegType == SPNT_NEGTIVE)
	{
		if (pkSkillExecute->HasPassive())
		{
			pkSkillExecute->EndPassive(this);
		}
		else
		{
			this->DeactiveSkillState(pkSkillAttrib->_lSkillConstParameter1);
		}
	}
#endif
	pkSkillExecute = SK_Factory::GetSingletonPtr()->RequestSkillExcute(curskillid);
	if (pkSkillExecute == NULL)
	{
		return ret;
	}
	pkSkillAttrib = pkSkillExecute->GetAttrDataEntryPtr();
	pkSkillData = pkSkillExecute->GetDataEntryPtr();
#if 0
	if (pkSkillData->_lPosNegType == SPNT_NEGTIVE)
	{
		if (pkSkillExecute->HasPassive())
		{
			pkSkillExecute->StartPassive(this);
		}
		else
		{
			this->ActiveSkillState(pkSkillAttrib->_lSkillConstParameter1, 0, this->GetID(),
				Utility::Int2Float(pkSkillAttrib->_lSkillConstParameter2),
				Utility::Int2Float(pkSkillAttrib->_lSkillConstParameter3),
				Utility::Int2Float(pkSkillAttrib->_lSkillConstParameter4),
				Utility::Int2Float(pkSkillAttrib->_lSkillConstParameter5),
				Utility::Int2Float(pkSkillAttrib->_lSkillConstParameter6));
		}
	}

	oss_role_sk_lvup(this, curskillid, pkSkillData->_lCategory2, pkSDData->_lNeedSP, pkSDData->_lNeedCash);
#endif
	return ret;
}

// for subordinate logic.
// commented by weixin
void BuddyInstance::OnCreate()
{
	LC_ServerPlayer* m_pOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if (m_pOwnerPlayer == NULL)
		return;
	m_pOwnerPlayer->FlushSkillStateToOneBuddyInstance(m_pConfig->_cID);
}

bool BuddyInstance::IsBind()
{
	return (m_nAssignedPlayerId != INVALID_ACTOR_ID);
}

void BuddyInstance::OnBind( object_id_type nAssignedPlayerId )
{
	// Revert state
	m_nAssignedPlayerId = nAssignedPlayerId;
	OnBind_RevertSkillState();

	//切换出战buddy的时候，出战的战力会发生变化，所以要更新FillRankAllBuddyData
	LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if(NULL != pkOwnerPlayer)
	{
		pkOwnerPlayer->UpdateRankUserValueByRankType(Rank_Type_Battle);
	}
}

void BuddyInstance::OnUnBind()
{
	// Revert state
	if ( m_Data.m_Data_Base.m_bLockAssignedPlayerId == false )
		m_nAssignedPlayerId = INVALID_ACTOR_ID;
	//OnBind_RevertSkillState();

	//切换出战buddy的时候，出战的战力会发生变化，所以要更新FillRankAllBuddyData
	LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if(NULL != pkOwnerPlayer)
	{
		pkOwnerPlayer->UpdateRankUserValueByRankType(Rank_Type_Battle);
	}
}

void BuddyInstance::OnBind_RevertSkillState()
{
	LC_ServerPlayer* m_pSelfPlayer = GetAssignedPlayer();
	if (m_pSelfPlayer == NULL)
		return;
	mem::map<int, StateCacheData>::iterator it = m_Data.m_Data_ActorBase.m_kStateCache.begin();
	for ( ; it != m_Data.m_Data_ActorBase.m_kStateCache.end(); ++it )
	{
		StateCacheData& stData = it->second;
		m_pSelfPlayer->ActiveSkillState(
			//m_Data.m_Data_ActorBase->m_kSkillStateMap->act
			stData.lStateTypeID, 
			0, stData.lStateAttacherID, 
			stData.nPara0,stData.nPara1,stData.nPara2,stData.nPara3,stData.nPara4
			);
	}
}


void BuddyInstance::OnRefreshOwnerLevel()
{
	TryAutoLevelUp();
}

void BuddyInstance::LoadDBData_SkillAsset(const char_data_define::skill_asset_info& kSkillInfo)
{
	m_Data.m_Data_PlayerBase.m_kSkillAsset.GetDataFromPB(kSkillInfo);
}

void BuddyInstance::LoadDBData_ShortcutBar(const char_data_define::shortcutbar_info& info)
{
	m_Data.m_Data_PlayerBase.m_pkShortcutBar->GetDataFromPB(info);
}

void BuddyInstance::LoadDBData_Attributes()
{
}

void BuddyInstance::InitInstanceData_ServerPlayer()
{

}


void BuddyInstance::InitInstanceData_ActorBase()
{
	if (m_pConfig == NULL)
		return;
	m_Data.m_Data_ActorBase.m_kCharName_utf8 = m_Data.m_Data_ActorBase.m_kCharName_gb;//CodeConvert::MbsToUtf8(m_Data.m_Data_ActorBase.m_kCharName_gb);
	m_Data.m_Data_ActorBase.m_lPunchFlyDistance		 = 0;//m_pConfig->_lPunchFlyDistance;
	m_Data.m_Data_ActorBase.m_fPunchFlyTime			 = 0;//m_pConfig->_fPunchFlyTime;
	m_Data.m_Data_ActorBase.m_lMaxLevel				 = MAX_PLAYER_LEVEL;
}


LC_ActorAttributeMap* BuddyInstance::GetFinalAttributeMap()
{
	return GetNewFinalAttributeMap();
}

ResultType BuddyInstance::EquipItemById(int nItemId, int iEquipSlotIndex)
{
	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	LC_ServerItem* pkItem = (LC_ServerItem*)(pkItemFactory->RequestItem(nItemId));
	if (!pkItem)
		return RE_ITEM_INVALID;
	if (pkItem->GetEquipPackLimit() != EmblemPack)
		return RE_ITEM_INVALID;
	int iEquipType = pkItem->GetEquipTypeLimit();
	if(iEquipSlotIndex != iEquipType)
	{
		return RE_FAIL;
	}

	LC_PackEntryFactory* pkPackEntryFactory = LC_PackEntryFactory::GetSingletonPtr();
	if (NULL == pkPackEntryFactory)
		return RE_FAIL;
	LC_BackPackEntryList kEntryList;
	pkPackEntryFactory->CreateItemEntry(nItemId, 1, CIET_NONE, kEntryList, -1, 0, 0, true);
	if(kEntryList.size() <=  0)
		return RE_FAIL;

	LC_BackPackEntry* pkEntry = GetEquipPack().GetEquipEntry_NotConst(iEquipSlotIndex);
	if (NULL == pkEntry)
		return RE_FAIL;
	pkEntry->Clone(kEntryList[0]);
	pkEntry->SetBackPackValueByType(LPSFET_VALID, true);
	ResultType iEquipResult	 = OnEquipItem(pkItem, &kEntryList[0], iEquipSlotIndex, GET_PROCESS_TIME, true);

	if (iEquipResult != RE_ITEM_EQUIP_SUCCESS)
		return iEquipResult;
	
	SyncBuddyEquipSlotData(iEquipSlotIndex);
	return RE_ITEM_EQUIP_SUCCESS;
}

ResultType BuddyInstance::CheckUnEquipItemInEquipSlot(int iSlotIndex)
{
	LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if(NULL == pkOwnerPlayer)
	{
		return RE_FAIL;
	}

	LC_BackPackEntry* pkEntry = GetEquipPack().GetEquipEntry_NotConst(iSlotIndex);
	if (!pkEntry)
	{
		return RE_ITEM_SYSTEM_ERROR;
	}
	if (!pkEntry->GetBackPackValueByType(LPSFET_VALID))
	{
		return RE_ITEM_SYSTEM_ERROR;
	}
	else if (pkEntry->GetBackPackValueByType(LPSFET_LOCKED))
	{
		return RE_ITEM_TRADE_ITEM_LOCKED;
	}
	int lItemTypeID = pkEntry->GetBackPackValueByType(LPSFET_TYPE_ID);
	int iTarPackType = GetWhichPackAutoPut(lItemTypeID);

	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	LC_ItemBase* pkItem = pkItemFactory->RequestItem(lItemTypeID);
	if (!pkItem)
	{
		return RE_ITEM_INVALID;
	}
	if (!pkItem->CanInBackPack())
	{
		return RE_ITEM_CANT_PUT_PACKAGE;
	}
	if (pkItem->IsOwnUnique())
	{
		if (pkEntry->GetBackPackValueByType(LPSFET_OVERLAP_COUNT) > 1)
		{
			return RE_ITEM_OWN_UNIQUE;
		}
		if (GetEquipPack().IsExistItem(lItemTypeID))
		{
			return RE_ITEM_OWN_UNIQUE;
		}
	}
	return pkItem->CheckUnEquipItem(pkOwnerPlayer, pkEntry);
}

ResultType BuddyInstance::UnEquipItemInEquipSlot_Base(int iSlotIndex, bool nBuddyCompositionFlag)
{
	ResultType iResult = CheckUnEquipItemInEquipSlot(iSlotIndex);
	if (RE_SUCCESS != iResult)
	{
		return iResult;
	}

	LC_BackPackEntry* pkEntry = GetEquipPack().GetEquipEntry_NotConst(iSlotIndex);
	if (!pkEntry || !pkEntry->GetBackPackValueByType(LPSFET_VALID))
	{
		return RE_ITEM_SYSTEM_ERROR;
	}

	int lItemTypeID = pkEntry->GetBackPackValueByType(LPSFET_TYPE_ID);
	LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
	LC_ServerItem* pkItem = (LC_ServerItem*)(pkItemFactory->RequestItem(lItemTypeID));
	if (!pkItem)
	{
		return RE_ITEM_INVALID;
	}

	//判断背包是否满了
	ResultType iEquipResult = RE_ITEM_UNEQUIP_SUCCESS;
	iEquipResult = OnUnEquipItem(pkItem, pkEntry, iSlotIndex, GET_PROCESS_TIME, true);
	if (iEquipResult != RE_ITEM_UNEQUIP_SUCCESS)
	{
		return iEquipResult;
	}

	pkEntry->Reset();
	return RE_ITEM_UNEQUIP_SUCCESS;
}

void BuddyInstance::SetLevel_BaseActor(int lLevel)
{
	// Base
	if (lLevel != m_Data.m_Data_ActorBase.m_lLevel)
	{
		m_Data.m_Data_ActorBase.m_lLevel = lLevel;
		//UpdateDirtyFlag(DIRTY_FLAG_PROPERTY);
		m_Data.m_Data_Base.m_nLevel_Orig = lLevel;
	}
}


void BuddyInstance::HandleExpBound()
{
#if 0
	// 这样处理的原因是，流程应该这样，只是目前妥协
	CF_LevelUp* pkCSVLevelUp = CF_LevelUp::GetSingletonPtr();
	int max_level = pkCSVLevelUp->GetCurrentMaxID();
	int nOrigLevel = GetLevel();
	LC_ServerPlayer* pOwner = m_pOwnerPlayer->GetOwnerPlayerByBuddyInstance();
	if (pOwner == NULL)
		return;

	int nOwnerLevel = pOwner->GetLevel();

	const CF_LevelUp::DataEntry* pkData = pkCSVLevelUp->GetEntryPtr(nOrigLevel + 1);

	if ( ( nOrigLevel >= nOwnerLevel ) || ( nOrigLevel >= max_level ) )
	{
		if (m_Data.m_Data_ActorBase.m_lEXP > pkData->_lEXPNeed - 1)
			m_Data.m_Data_ActorBase.m_lEXP = pkData->_lEXPNeed - 1;
	}
#endif
}

bool BuddyInstance::CheckExpBound()
{
	LC_ServerPlayer* m_pOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if (m_pOwnerPlayer == NULL)
		return false;

	CF_LevelUp* pkCSVLevelUp = CF_LevelUp::GetSingletonPtr();
	int max_level = pkCSVLevelUp->GetCurrentMaxID();
	int nOrigLevel = GetLevel();

	int nOwnerLevel = m_pOwnerPlayer->GetLevel();

	const CF_LevelUp::DataEntry* pkData = pkCSVLevelUp->GetEntryPtr(nOrigLevel + 1);
	if (pkData == NULL)
		return false;

	if ( ( nOrigLevel >= nOwnerLevel ) || ( nOrigLevel >= max_level ) )
	{
		if (m_Data.m_Data_ActorBase.m_lEXP >= pkData->_lEXPNeed - 1)
			return true;
	}
	return false;
}

bool BuddyInstance::GetLevelUpFlag()
{
	return m_Data.m_Data_PlayerBase.m_nLevelUpFlag;
}

void BuddyInstance::SetLevelUpFlag(bool tag)
{
	m_Data.m_Data_PlayerBase.m_nLevelUpFlag = tag;
}

const CSVFile::CF_BuddyUpgrade::DataEntry* BuddyInstance::UpdateBuddyUpgradeData(int32_t nBuddyID, int32_t nStarLevel)
{
	/// 缓存 BuddyHelper::GetBuddyUpgradeData 查询结果
	if (m_kCurStarData.test(nBuddyID, nStarLevel))
	{
		const CF_BuddyUpgrade::DataEntry* pCurStarData = BuddyHelper::GetBuddyUpgradeData(nBuddyID, nStarLevel);
		m_kCurStarData.set(pCurStarData, nBuddyID, nStarLevel);
	}
	return m_kCurStarData.value;
}

ResultType BuddyInstance::TryManualLevelUp()
{
	int nResult = RE_FAIL;
	LC_ServerPlayer* pOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if (pOwnerPlayer == NULL)
		return false;

	//LC_Money* pkMoney = NULL;
	LC_Money& exp_val = pOwnerPlayer->GetBuddyExp();
	MoneyType llCurrentExp = exp_val.GetMoney();

	CF_LevelUp* pkCSVLevelUp = CF_LevelUp::GetSingletonPtr();
	int max_level = pkCSVLevelUp->GetCurrentMaxID();
	int PreLevel = GetLevel();
	int nOrigLevel = GetLevel();

	int nOwnerLevel = pOwnerPlayer->GetLevel();
	//*
	const CF_BuddyUpgrade::DataEntry* pCurStarData = this->UpdateBuddyUpgradeData(m_pConfig->_cID, m_Data.m_Data_Base.m_nStarLevel);
	/*/
	const CF_BuddyUpgrade::DataEntry* pCurStarData = BuddyHelper::GetBuddyUpgradeData(m_pConfig->_cID, m_Data.m_Data_Base.m_nStarLevel);
	//*/
	if ( pCurStarData == NULL )
		return nResult;
	int nBuddyStarMaxLevel = pCurStarData->_iBuddyMaxLevel;

	const CF_LevelUp::DataEntry* pkData = pkCSVLevelUp->GetEntryPtr(PreLevel + 1);
	const CF_LevelUp::DataEntry* pkOwnerData = pkCSVLevelUp->GetEntryPtr(nOwnerLevel);
	if(NULL==pkData || NULL==pkOwnerData)
		return nResult;

	if ( ( PreLevel >= nOwnerLevel ) || ( PreLevel >= max_level ) || (PreLevel >= nBuddyStarMaxLevel))
		return nResult;

	LC_PackAsset* pkOwnerBasicAsset = pOwnerPlayer->GetPackAsset();
	if(NULL == pkOwnerBasicAsset)
	{
		return RE_FAIL;
	}

	int cID = GetTemplateID();

	mem::vector<int32_t> runeSkill;
	GetRuneSkillID(runeSkill);
	
	if (pkData && llCurrentExp >= pkData->_fBuddyEXPNeed)
	{
		//升级前先忘掉技能
		for(mem::vector<int32_t>::iterator it=runeSkill.begin(); it!=runeSkill.end(); ++it)
		{
			pOwnerPlayer->RemoveSkillToBuddy(cID, *it);
		}

		llCurrentExp -= pkData->_fBuddyEXPNeed;
		//ReduceCash(lTotalNeedCash, CT_UNBIND_CASH, SYS_FUNC_TP_ITEM_OPERATION, LC_ITEM_OP_CODE_SOCKET_INLAY);
		vector<int> nParams;
		nParams.clear();
		nParams.push_back(cID);
		nParams.push_back(PreLevel);
		nParams.push_back(PreLevel+1);
		int32_t nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, BUDDY_OPERATE, OPERATE_LOGTYPE2);
		pOwnerPlayer->ReduceCash( pkData->_fBuddyEXPNeed, CT_BUDDY_EXP, SYS_FUNC_TP_BUDDY_UPGRADE_LEVEL, PreLevel + 1 ,nLogCode, nParams);
		SetLevelUpFlag(true);

		SetLevel(PreLevel + 1, false);
		pOwnerPlayer->OnRefreshBuddyLevel( m_pConfig->_cID, PreLevel + 1);
			
		//buddy升级后，通知成就系统
		AchievementEvent kBuddyEvent;
		kBuddyEvent.SetConditionInfo(GetLevel(), GetQuality());
		kBuddyEvent.SetNum(1);
		pOwnerPlayer->UpdateAchievementRecord(LC_ACHIEVEMENT_TYPE_BUDDY_LEVEL, kBuddyEvent);
		
		AchievementEvent kUpgradeEvent;
		kUpgradeEvent.SetConditionInfo(GetTemplateID());
		kUpgradeEvent.SetNum(GetLevel());
		pOwnerPlayer->UpdateAchievementRecord(LC_ACHIEVEMENT_TYPE_UPGRADE_BUDDY_LEVEL, kUpgradeEvent);

		nResult = RE_SUCCESS;

		for(mem::vector<int32_t>::iterator it=runeSkill.begin(); it!=runeSkill.end(); ++it)
		{
			//宝具技能的属性与buddy的基础属性有关，升星升级后，重新学习技能
			int32_t slotID = -1;
			pOwnerPlayer->LearnSkillToBuddy(cID, *it, slotID);
		}
	}

	if ( nOrigLevel != GetLevel() )
	{
		SyncBaseData();
		LC_ServerPlayer* pSelfPlayer = GetAssignedPlayer();
		if ( pSelfPlayer )
		{
			pSelfPlayer->SyncPropertyToClient(true);
		}
	}
	return nResult;
}


void BuddyInstance::CheckFlushRuneSlotDressState()
{
	LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if(NULL == pkOwnerPlayer)
	{
		return;
	}

	bool tag = pkOwnerPlayer->GetBuddyRuneEquipState();
	if(!tag)
	{
		return;
	}

	int currentBuddyLevel = GetLevel();
	int count = GetCanDressRuneSlotCount();
	if(count < GetGlobalSetting.RunePackSize)
	{
		if(currentBuddyLevel >= GetGlobalSetting.runeSlotOpenLevel_0)
		{
			SetRuneSlotDressState(0);
		}

		if(currentBuddyLevel >= GetGlobalSetting.runeSlotOpenLevel_1)
		{
			SetRuneSlotDressState(1);
		}
		
		if(currentBuddyLevel >= GetGlobalSetting.runeSlotOpenLevel_2)
		{
			SetRuneSlotDressState(2);
		}

		if(currentBuddyLevel >= GetGlobalSetting.runeSlotOpenLevel_3)
		{
			SetRuneSlotDressState(3);
		}
	}
}

int32_t BuddyInstance::GetCanDressRuneSlotCount()
{
	int32_t count = 0;
	return GetRunePack().GetCanDressRuneSlotCount();
}

void BuddyInstance::ResetAttrSyncFlag()
{
	m_Data.m_Data_Base.m_bAttrDirty = false;
	m_Data.m_Data_Base.m_bAttrSync = false;
}
void BuddyInstance::AutoSkillLevelUp(bool nNotice /*= true*/)
{
	//for循环 遍历身上的四个技能看是否可以升级
	LC_SkillAsset* pkSkillAsset = GetSkillAsset();
	LC_SkillAssetData* pSkillData =  pkSkillAsset->GetSkillAssetData();
	LC_ServerPlayer* pSelfPlayer = GetOwnerPlayerByBuddyInstance();
	if ( !pSelfPlayer )
		return;
	LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
	int nSize = pSkillData->GetSkillCount((skill_cat_type)SKILL_TYPE_SECT);
	for ( int i = 0; i < nSize; i++ )
	{
		LC_SkillAssetEntry* pkEntry = pkSkillAsset->GetSkillEntryInAsset((skill_cat_type)SKILL_TYPE_SECT, i);
		if (NULL == pkEntry || !pkEntry->GetValid())
			continue;
		int iResultBuddy = UpdateSkillInSkillAsset((skill_cat_type)SKILL_TYPE_SECT, i, 0);
		if (iResultBuddy != RE_SKILL_UPDATE_SUCCESS)
			continue;
		pkEntry = pkSkillAsset->GetSkillEntryInAsset((skill_cat_type)SKILL_TYPE_SECT, i);
		if (NULL == pkEntry)
			continue;
		int lSkillID = pkEntry->GetTypeID();
		if(nNotice) 
		{
			MG_RLT_UpdateSkill kRltMsg;
			kRltMsg.m_iBuddyID = GetTemplateID();
			kRltMsg.m_lSkillIDAfterUpdate = lSkillID;
			kRltMsg.m_lUpdateSkillResult = iResultBuddy;
			pSelfPlayer->SendMsgToClient(MGPT_RLT_SKILL_UPDATE, &kRltMsg);
		}

		//技能突破传闻
		LC_GameEvent* pkGameEvent = pkEventManager->AllocGameEvent();
		if (NULL == pkGameEvent)
			continue;

		pkGameEvent->SetEventType(GLET_SKILL_LEVEL_UP);
		pkGameEvent->SetEventParam(EVENT_SEND_TYPE_PARAM, GENT_SELF);
		pkGameEvent->SetEventParam(EP_SKILL_LEVEL_UP_PLAYER_ID, pSelfPlayer->GetID());
		pkGameEvent->SetEventParam(EP_SKILL_LEVEL_UP_SKILL_ID, lSkillID);
		pkEventManager->InsertGameEvent(pkGameEvent);
	}
}
int BuddyInstance::TryAutoLevelUp( bool bCheckMaster )
{
	int nResult = RE_SUCCESS;
	LC_ServerPlayer* pOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if (pOwnerPlayer == NULL)
		return RE_FAIL;
	CF_LevelUp* pkCSVLevelUp = CF_LevelUp::GetSingletonPtr();
	int max_level = pkCSVLevelUp->GetCurrentMaxID();
	int PreLevel = GetLevel();
	int nOrigLevel = GetLevel();
	int nOwnerLevel = pOwnerPlayer->GetLevel();
	const CF_LevelUp::DataEntry* pkData = pkCSVLevelUp->GetEntryPtr(PreLevel + 1);
	if(NULL==pkData)
		return RE_FAIL;
	const CF_BuddyUpgrade::DataEntry* pCurStarData = this->UpdateBuddyUpgradeData(m_pConfig->_cID, m_Data.m_Data_Base.m_nStarLevel);
	if ( pCurStarData == NULL )
		return RE_FAIL;
	LC_PackAsset* pkOwnerBasicAsset = pOwnerPlayer->GetPackAsset();
	if(NULL == pkOwnerBasicAsset)
		return RE_FAIL;

	int cID = GetTemplateID();
	mem::vector<int32_t> runeSkill;
	GetRuneSkillID(runeSkill);

	bool bUpdate = false;
	while (pkData && m_Data.m_Data_ActorBase.m_lEXP >= pkData->_lEXPNeed)
	{
		if ( ( PreLevel >= nOwnerLevel ) || ( PreLevel >= max_level ) || ( PreLevel >= pCurStarData->_iBuddyMaxLevel ))
		{
			if (m_Data.m_Data_ActorBase.m_lEXP > pkData->_lEXPNeed - 1)
				m_Data.m_Data_ActorBase.m_lEXP = pkData->_lEXPNeed - 1;
			break;
		}

		for(mem::vector<int32_t>::iterator it=runeSkill.begin(); it!=runeSkill.end(); ++it)
		{
			pOwnerPlayer->RemoveSkillToBuddy(cID, *it);
		}

		m_Data.m_Data_ActorBase.m_lEXP -= pkData->_lEXPNeed;
		SetLevelUpFlag(true);
		SetLevel(PreLevel + 1, false);
		pOwnerPlayer->OnRefreshBuddyLevel( m_pConfig->_cID, PreLevel + 1);
	
		//buddy升级后，通知成就系统
		AchievementEvent kEvent;
		kEvent.SetConditionInfo(GetLevel(), GetQuality());
		kEvent.SetNum(1);
		pOwnerPlayer->UpdateAchievementRecord(LC_ACHIEVEMENT_TYPE_BUDDY_LEVEL, kEvent);

		AchievementEvent kUpgradeEvent;
		kUpgradeEvent.SetConditionInfo(GetTemplateID());
		kUpgradeEvent.SetNum(GetLevel());
		pOwnerPlayer->UpdateAchievementRecord(LC_ACHIEVEMENT_TYPE_UPGRADE_BUDDY_LEVEL, kUpgradeEvent);

		PreLevel = GetLevel();
		pkData = pkCSVLevelUp->GetEntryPtr(PreLevel + 1);

		//宝具技能的属性与buddy的基础属性有关，升星升级后，重新学习技能
		for(mem::vector<int32_t>::iterator it=runeSkill.begin(); it!=runeSkill.end(); ++it)
		{
			int32_t slotIndex = -1;
			pOwnerPlayer->LearnSkillToBuddy(cID, *it, slotIndex);
		}

		bUpdate = true;
	}

	if( bUpdate )
	{			
		SyncBaseData();
		LC_ServerPlayer* pSelfPlayer = GetAssignedPlayer();
		if ( pSelfPlayer )
			pSelfPlayer->SyncPropertyToClient(true);
		return RE_SUCCESS;
	}
	return RE_FAIL;
}

void BuddyInstance::SetLevel(int lLevel, bool bInit)
{
	LC_ServerPlayer* pOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if (pOwnerPlayer == NULL)
		return;

	int PreLevel = GetLevel();
	if (PreLevel == lLevel)
	{
		OnSetLevel(lLevel);
		return;
	}
	//m_dataChangeFlag.GetBroadCastFlag().SetProperty();
	CF_LevelUp* pkCSVLevelUp = CF_LevelUp::GetSingletonPtr();
	int max_level = pkCSVLevelUp->GetCurrentMaxID();
	if (lLevel >= max_level)
	{
		lLevel = max_level - 2;
	}

	const CF_LevelUp::DataEntry* pkData = pkCSVLevelUp->GetEntryPtr(lLevel);
	if (!pkData)
	{
		return;
	}

	SetLevel_BaseActor(lLevel);

	RefreshLevelAttrMap(pOwnerPlayer, this);
	GetBuddyAttrMgr().RefreshHeroAllAttributeMap(pOwnerPlayer, this);
	//刷新此英雄 提供的灵魂链属性
	GetBuddyAttrMgr().RefreshHeroGiveToOtherCoreAttrMap(pOwnerPlayer);
	//刷新灵魂链上玩家获得灵魂链属性
	pOwnerPlayer->RefreshChainBuddyCoreAttrByOnChain(GetTemplateID(), bInit, SPART_Buddy_2);
	GetBuddyAttrMgr().RefreshNewFinalAttrMap(pOwnerPlayer, this, bInit, SPART_Buddy_2);

	OnLevelData(PreLevel, lLevel, bInit);
	
	bool bNewSkill = InitSkillData();
	if (!bInit && bNewSkill)
		SyncSkillAsset();
	SyncShortcutBar();
	OnSetLevel(lLevel);
}

int BuddyInstance::GetLevel()
{
	return m_Data.m_Data_ActorBase.m_lLevel;
}

int	BuddyInstance::GetMaxLevel()
{
	return m_Data.m_Data_ActorBase.m_lMaxLevel;
}

bool BuddyInstance::SetEXP(attr_value_type lEXP)
{
	LC_ServerPlayer* m_pOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if (m_pOwnerPlayer == NULL)
		return false;
	if (CheckExpBound())
		return false;
	lEXP = UT_MathBase::MaxInt64(0, lEXP);
	int64_t lNeedLevelUpExp = LC_Helper::GetLevelUpExp(GetLevel() + 1);
	lEXP = UT_MathBase::MinInt64(lEXP, lNeedLevelUpExp + MAX_EXP_CACHE_LIMIT);
	if (lEXP != m_Data.m_Data_ActorBase.m_lEXP)
	{
		m_Data.m_Data_ActorBase.m_lEXP = lEXP;

		HandleExpBound();

		TryAutoLevelUp(true);
	}
	return true;
}

attr_value_type BuddyInstance::GetEXP() 
{
	return m_Data.m_Data_ActorBase.m_lEXP;
}

int BuddyInstance::GetTemplateID()
{
	return m_pConfig->_cID;
}

void BuddyInstance::SetHPFromDB(attr_value_type nValue)
{
	m_Data.m_Data_ServerPlayer.m_lHPFromDB = nValue;
	m_Data.m_Data_ActorBase.m_lHP = nValue;
}

attr_value_type BuddyInstance::GetHPFromDB()
{
	return m_Data.m_Data_ServerPlayer.m_lHPFromDB;
}

void BuddyInstance::SetMPFromDB(attr_value_type nValue)
{
	m_Data.m_Data_ServerPlayer.m_lMPFromDB = nValue;
}

attr_value_type BuddyInstance::GetMPFromDB()
{
	return m_Data.m_Data_ServerPlayer.m_lMPFromDB;
}

int BuddyInstance::GetShapeShiftCharTypeID()
{
	return m_Data.m_Data_ActorBase.m_lShapeShiftCharTypeID;
}

void BuddyInstance::SetShapeShiftCharTypeID(int lCharTypeID)
{
	m_Data.m_Data_ActorBase.m_lShapeShiftCharTypeID = lCharTypeID;
}

bool BuddyInstance::AddExp( attr_value_type lExp, int32_t eSrcType, int lSrcTypeID, bool bIgnoreEXPRate )
{
	LC_ServerPlayer* m_pOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if (m_pOwnerPlayer == NULL)
		return false;

	attr_value_type real_exp = lExp;

	//有些经验获得的途径是不计入加成的
	switch (eSrcType)
	{
	case SYS_FUNC_TP_FARM:
		{
			real_exp = Utility::Round2Int64(real_exp *  LC_Helper::GetAttributeRate(m_pOwnerPlayer, ATT_EXP_RATE));
		}
		break;
	default:
		bIgnoreEXPRate = true;
		break;
	}

	if (BuddyHelper::IS_ANTIWALLOW_RELATED(eSrcType))
	{
		real_exp = Utility::Round2Int64(real_exp * m_pOwnerPlayer->GetAntiWallowStateQuotiety());
	}
	if (real_exp <= 0)
	{
		if (lExp > 0)
		{
			if (WALLOW == m_pOwnerPlayer->GetAntiWallowState())
			{
				real_exp = 0;
			}
			else
			{
				real_exp = 1;
			}
		}
		else
		{
			real_exp = 0;
		}
	}
	if (real_exp > 0)
	{
		m_pOwnerPlayer->SetDBSaveMaskFlag(LPDST_CHAR_BASE_INFO, true);
		if (!SetEXP(GetEXP() + real_exp))
			return false;
		m_pOwnerPlayer->m_kResourceAlert.AddResource(SPEC_ITEM_TID_EXP, eSrcType, real_exp);

		if (IS_PROFIT_STAT_RELATED(eSrcType))
		{
			//profit stat
			CF_WorldMapList::DataEntry* pkMapEntry = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(m_pOwnerPlayer->GetMapResID());
			if (pkMapEntry && pkMapEntry->_ProfitStat > 0)
			{
				m_pOwnerPlayer->GetCommonStatAsset().OnGotExp(real_exp, eSrcType);
			}
		}
	}
	return true;
}

int32_t BuddyInstance::GetStarLevel()
{
	return m_Data.m_Data_Base.m_nStarLevel;
}

ResultType BuddyInstance::DoUpgradeStar()
{
	LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if(NULL == pkOwnerPlayer)
	{
		return RE_FAIL;
	}

	LC_PackAsset* pkOwnerBasicAsset = pkOwnerPlayer->GetPackAsset();
	if(NULL == pkOwnerBasicAsset)
	{
		return RE_FAIL;
	}

	int cID = GetTemplateID();
	
	//宝具技能的属性与buddy的基础属性有关，升星升级前，先忘记技能
	mem::vector<int32_t> runeSkill;
	GetRuneSkillID(runeSkill);
	for(mem::vector<int32_t>::iterator it=runeSkill.begin(); it!=runeSkill.end(); ++it)
	{
		pkOwnerPlayer->RemoveSkillToBuddy(cID, *it);
	}

	ResultType nRet = RE_SUCCESS;
	int32_t nOrigStarLevel = m_Data.m_Data_Base.m_nStarLevel;

	m_Data.m_Data_Base.m_nStarLevel++;
	//  技能自动升级 ，放在 m_Data.m_Data_Base.m_nStarLevel++; 之后做
	//修改为：技能升级和buddy升星没有关系，技能升级通过协议驱动
	//AutoSkillLevelUp();
	int buddyID = GetTemplateID();
	//升星公告
	if(m_Data.m_Data_Base.m_nStarLevel >= GetGlobalSetting.buddyNotifyStarLevel)
	{
		UT_ServerHelper::_GameNoticeFmort noticeFmort;
		noticeFmort.SetMsgID(17);
		noticeFmort.AddString(pkOwnerPlayer->GetOwnerCharName());//玩家名
		noticeFmort.AddBuddyCID(buddyID);//buddyID
		noticeFmort.AddBuddyStarLevel(m_Data.m_Data_Base.m_nStarLevel);//星级
		UT_ServerHelper::BrocastNoticeMsg(noticeFmort, pkOwnerPlayer->GetCitizenship());
	}

	OnBothLevelData(m_Data.m_Data_ActorBase.m_lLevel, m_Data.m_Data_ActorBase.m_lLevel, nOrigStarLevel, m_Data.m_Data_Base.m_nStarLevel, false);
	
	//宝具技能的属性与buddy的基础属性有关，升星升级后，重新学习技能
	for(mem::vector<int32_t>::iterator it=runeSkill.begin(); it!=runeSkill.end(); ++it)
	{
		int32_t index = -1;
		pkOwnerPlayer->LearnSkillToBuddy(cID, *it, index);
	}

	{
		//升星后，通知成就系统
		AchievementEvent kEvent1;
		kEvent1.SetConditionInfo(GetStarLevel(), GetQuality());
		kEvent1.SetNum(1);
		pkOwnerPlayer->UpdateAchievementRecord(LC_ACHIEVEMENT_TYPE_BUDDY_STARLEVEL, kEvent1);

		AchievementEvent kEvent2;
		kEvent2.SetConditionInfo(GetTemplateID());
		kEvent2.SetNum(GetStarLevel());
		pkOwnerPlayer->UpdateAchievementRecord(LC_ACHIEVEMENT_TYPE_UPGRADE_BUDDY_STARLEVEL, kEvent2);
	}

	SyncBaseData();
	//先发maxHP
	//再发currentHP
	LC_ServerPlayer* pkAssignedPlayer = GetAssignedPlayer();
	if(NULL != pkAssignedPlayer)
	{
		pkAssignedPlayer->SyncPropertyToClient();
	}
	pkOwnerPlayer->UpdateRankInfoToRedis(Sub_Reward_Type_BuddyStarLevel,  false, buddyID);
	return nRet;
}





int BuddyInstance::SetHP(attr_value_type lHP)
{
	attr_value_type maxHp = GetFinalAttributeMap()->GetAttribute(ATT_MAX_HP);
	lHP = lHP<maxHp ? lHP : maxHp;

	int nResult = false;
	attr_value_type oldHP = m_Data.m_Data_ActorBase.m_lHP;
	if (oldHP != lHP)
	{
		//m_dataChangeFlag.SetHPMP();
		nResult = true;
	}
	m_Data.m_Data_ActorBase.m_lHP = lHP;
	return nResult;
}


attr_value_type BuddyInstance::GetHP()
{
	return m_Data.m_Data_ActorBase.m_lHP;
}

int BuddyInstance::SetMP(attr_value_type  lMP)
{
	int nResult = false;
	attr_value_type oldMP = m_Data.m_Data_ActorBase.m_lMP;
	if (oldMP != lMP)
	{
		//m_dataChangeFlag.SetHPMP();
		nResult = true;
	}
	m_Data.m_Data_ActorBase.m_lMP = lMP;
	return nResult;
}

attr_value_type BuddyInstance::GetMP()
{
	return m_Data.m_Data_ActorBase.m_lMP;
}

void BuddyInstance::SetOldHP(attr_value_type lOldHP)
{
	m_Data.m_Data_ActorBase.m_lOldHP = lOldHP;
}

attr_value_type BuddyInstance::GetOldHP()
{
	return m_Data.m_Data_ActorBase.m_lOldHP;
}

LC_ActorStateMap* BuddyInstance::GetStateMap()
{
	return m_Data.m_Data_ActorBase.m_kSkillStateMap;
}


void BuddyInstance::OnBothLevelData_Attribute(LC_ActorAttributeMap& input, const CSVFile::CF_Buddy::DataEntry* pConfig, const CF_LevelUp::DataEntry* pLevelData, const CF_BuddyUpgrade::DataEntry* pStarData, bool bForScoreCalc)
{
	input.SetAttributeBase(ATT_MAX_HP,	pConfig->_lMaxHP + pLevelData->_lMaxHPAdd * pStarData->_lMaxHP);
	input.SetAttributeBase(ATT_MAX_ATK,	pConfig->_lMaxPHA + pLevelData->_lMaxATKAdd * pStarData->_lMaxPHA);
	input.SetAttributeBase(ATT_DEF,		pConfig->_lPHD + pLevelData->_lPHDAdd * pStarData->_lPHD);
	input.SetAttributeBase(ATT_DOG_RATE,	pConfig->_lDogRATE + pLevelData->_lDogRateAdd * pStarData->_lDogRATE);
	input.SetAttributeBase(ATT_CIR_RATE,	pConfig->_lCirRATE + pLevelData->_lCirRateAdd * pStarData->_lCirRATE);
	input.SetAttributeBase(ATT_MOVE_SPEED, (int)CHAR_MOVE_SPEED);
	input.SetAttributeBase(ATT_ATK_RATE, pConfig->_lATKRateAdd + pLevelData->_lATKRateAdd * pStarData->_lATKRateAdd);
	input.SetAttributeBase(ATT_CIR_DEF, pConfig->_lCirDefAdd + pLevelData->_lCirDefAdd * pStarData->_lCirDefAdd);
	input.SetAttributeBase(ATT_HP_RECOVER,	pLevelData->_lHPReg);
	if (bForScoreCalc)
	{
		input.SetAttributeBase(ATT_MAX_MP,	0);
		input.SetAttributeBase(ATT_RES_FREEZ, 0);
		input.SetAttributeBase(ATT_RES_SLOW, 0);
		input.SetAttributeBase(ATT_RES_SILENCE, 0);
		input.SetAttributeBase(ATT_RES_DIZZ, 0);
		input.SetAttributeBase(ATT_RES_SLEEP, 0);
		// Move from _PlayerBase
		input.SetAttributeBase(ATT_MAX_AMM,	0);
		input.SetAttributeBase(ATT_MAX_STM,	0);
		input.SetAttributeBase(ATT_MP_RECOVER,	0);
		input.SetAttributeBase(ATT_CIR_DAM, pConfig->_lCirDAM);
		input.SetAttributeBase(ATT_MOVE_SPEED, CHAR_MOVE_SPEED);
		input.SetAttributeBase(ATT_PICK_CRASH_RNG, GetGlobalSetting.iGoldPickRadius);

		input.AddAttributePercentFunc(ATT_MAX_HP, 0);//m_pConfig->_lMaxHPRate);
		input.AddAttributePercentFunc(ATT_MAX_ATK, 0);//m_pConfig->_lMaxAtkRate);
		input.AddAttributePercentFunc(ATT_DEF, 0);//m_pConfig->_lMaxDefRate);

		//这里指的是比率的改变
		input.SetAttributeBase(ATT_MP_COST_RATE, 0);
		input.SetAttributeBase(ATT_HP_COST_RATE, 0);
		input.SetAttributeBase(ATT_MP_RECOVER_RATE, 0);
		input.SetAttributeBase(ATT_HP_RECOVER_RATE, 0);
		input.SetAttributeBase(ATT_SP_RECOVER_RATE, 10000);
		input.SetAttributeBase(ATT_STM_RECOVER_RATE, 0);//m_pConfig->_lSTMReg);
		input.SetAttributeBase(ATT_AMM_RECOVER_RATE, 0);//m_pConfig->_lAMMReg);
		//这里指的是百分比*10000（存成整数）
		input.SetAttributeBase(ATT_EXP_RATE, 10000);
		input.SetAttributeBase(ATT_CASH_RATE, 10000);
		input.SetAttributeBase(ATT_MF_RATE, 0);//m_pConfig->_lMFRate + 10000);

		//伤害增益的参数
		input.SetAttributeBase(ATT_DMG_RATE, 10000);
		// 目前设计千分制，原机制万分制
		input.SetAttributeBase(ATT_DMG_RATE, pConfig->_lDMG_RATE);
		input.SetAttributeBase(ATT_DMG_DERATE,pConfig->_lDMG_DERATE );
		input.SetAttributeBase(ATT_IGNORE_DEF, pConfig->_lIGNORE_DEF );
		input.SetAttributeBase(ATT_DMG_REBOUND, pConfig->_lDMG_REBOUND);
		input.SetAttributeBase(ATT_CRI_DMG_DERATE, pConfig->_lCRI_DMG_DERATE);
		input.SetAttributeBase(ATT_HP_HURT, pConfig->_HpHurt);
	}
	input.RecomputeAll();
}

void BuddyInstance::OnBothLevelData(int nOrigLevel, int lLevel, int nOrigStarLevel, int nStarLevel, bool bInit, bool bEnterGame,bool bLoad)
{
	CF_LevelUp* pkCSVLevelUp = CF_LevelUp::GetSingletonPtr();
	int max_level = pkCSVLevelUp->GetCurrentMaxID();
	if (lLevel >= max_level)
	{
		lLevel = max_level - 2;
	}

	int nDelta  =lLevel - nOrigLevel + nStarLevel - nOrigStarLevel;

	//重新计算属性

	// TODO: Move to Buddy data initialize.
	const CF_LevelUp::DataEntry* pkPreData = pkCSVLevelUp->GetEntryPtr(nOrigLevel);
	const CF_LevelUp::DataEntry* pkLevelData = pkCSVLevelUp->GetEntryPtr(lLevel);
	
	const CF_BuddyUpgrade::DataEntry* pOrigStarData = BuddyHelper::GetBuddyUpgradeData(m_pConfig->_cID, nOrigStarLevel);
	const CF_BuddyUpgrade::DataEntry* pCurStarData = BuddyHelper::GetBuddyUpgradeData(m_pConfig->_cID, nStarLevel);

	if (pOrigStarData == NULL || pCurStarData == NULL)
		return;

	int nSlot = -1;
	RemoveSkill(pOrigStarData->_unlockskill);
	LearnSkill(pCurStarData->_unlockskill, nSlot, false);
	if (!bEnterGame && !bInit)
		SyncSkillAsset();
	LC_ActorAttributeMap attr_for_score;
	OnBothLevelData_Attribute(attr_for_score, m_pConfig, pkLevelData, pCurStarData, true);
	
	
	
	
	attr_value_type nMaxHPValue = GetFinalAttributeMap()->GetAttribute(ATT_MAX_HP);
	// Need a flag like bool bInit
	if (!bInit)
		SetHP(nMaxHPValue);
}

void BuddyInstance::OnLevelData(int nOrigLevel, int lLevel, bool bInit)
{
	if(!bInit)
		OnBothLevelData(nOrigLevel, lLevel, m_Data.m_Data_Base.m_nStarLevel, m_Data.m_Data_Base.m_nStarLevel, bInit);
	
}


ResultType BuddyInstance::OnEquipItem(LC_ServerItem* pItemBase, LC_BackPackEntry* pkItemEntry, int slotIndex, float fCurrentTime, bool bAttrRefresh, bool initGame)
{
	ResultType eResult = RE_ITEM_EQUIP_SUCCESS;
	CF_ItemList* pkCSVItemList = CF_ItemList::GetSingletonPtr();

	ResultType iResult = RE_SUCCESS;

	iResult = OnEnableEquipItem(pItemBase, pkItemEntry, slotIndex, fCurrentTime, bAttrRefresh);
	if ( iResult != RE_SUCCESS)
		return iResult;	

	LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if(NULL !=pkOwnerPlayer && true!=initGame)
	{
		int buddyId = GetTemplateID();
		pkOwnerPlayer->BroadcastBuddyEquipItem(GetAssignedPlayerId(), buddyId, slotIndex, pkItemEntry->GetBackPackValueByType(LPSFET_TYPE_ID));

		pkOwnerPlayer->UpdateRankInfoToRedis(Sub_Reward_Type_BuddyEquipSlotCombatScore, false, buddyId);

		//佩戴装备，通知成就系统
		int itemID = pkItemEntry->GetBackPackValueByType(LPSFET_TYPE_ID);
		CF_ItemList::DataEntry* pkItemData = SafeGetCSVFileDataEntryPtr<CF_ItemList>(itemID);
		if(NULL != pkItemData)
		{
			AchievementEvent kEvent;
			kEvent.SetConditionInfo(pkItemData->_InitStar, pkItemData->_lGrade);
			kEvent.SetNum(1);
			pkOwnerPlayer->UpdateAchievementRecord(LC_ACHIEVEMENT_TYPE_BUDDY_EQUIP, kEvent);
		}
	}

	return eResult;
}

ResultType BuddyInstance::OnEnableEquipItem(LC_ServerItem* pItemBase, LC_BackPackEntry* pkItemEntry, int slotIndex, float fCurrentTime, bool bAttrRefresh)
{
	ResultType iResult = RE_SUCCESS;
	if (pItemBase->GetEnduranceFuncID() > 0)
		return iResult;

	//增加装备附属技能
	int32_t skillId = pkItemEntry->GetBackPackValueByType(LCSPET_EQUIP_AFFILIATED_SKILL);
	if(skillId != 0)
	{
		int32_t slot = -1;
		LearnSkill(skillId, slot);
	}
	//计算物品基本属性加成
	//Attribute基础属性 ( 后续观察下 ，什么逻辑， 删除)
	//iResult = pItemBase->_operationAttributeFunc(0, this, pkItemEntry, slotIndex);

	if(iResult != RE_SUCCESS)
		return iResult;

	//物品修改技能CD功能
	if (pItemBase->GetSkillCDModifyFuncID() > 0)
	{
		iResult = pItemBase->_operationSkillCDModifyFunc(pItemBase->GetSkillCDModifyFuncID(), this);
		if (iResult != RE_SUCCESS)
			return iResult;
	}
	

	//修正currentHP
	if (m_bInitGame == false)
		FixMaxHPMP();

	return iResult;
}

ResultType BuddyInstance::OnUnEquipItem(LC_ServerItem* pItemBase, LC_BackPackEntry* pkItemEntry, int slotIndex, float fCurrentTime, bool bAttrRefresh)
{
	ResultType iResult = RE_SUCCESS;

	iResult = OnDisableEquipItem(pItemBase, pkItemEntry, slotIndex, fCurrentTime, bAttrRefresh);

	if ( iResult != RE_SUCCESS)
		return iResult;

	return RE_ITEM_UNEQUIP_SUCCESS;
}

ResultType BuddyInstance::OnDisableEquipItem(LC_ServerItem* pItemBase, LC_BackPackEntry* pkItemEntry, int slotIndex, float fCurrentTime, bool bAttrRefresh)
{
	ResultType iResult = RE_SUCCESS;

	if (pItemBase->GetEnduranceFuncID() > 0)
		return RE_SUCCESS;

	CF_ItemEquipLimitFunc* pkCSVEquipLimitFunc = CF_ItemEquipLimitFunc::GetSingletonPtr();
	CF_ItemEquipLimitFunc::DataEntry* pkEquipLimitData = pkCSVEquipLimitFunc->GetEntryPtr(pItemBase->GetEquipLimitFuncID());
	if(!pkEquipLimitData)
	{
		return RE_ITEM_CANT_UNEQUIP;
	}

	//删除装备附属技能
	int32_t skillId = pkItemEntry->GetBackPackValueByType(LCSPET_EQUIP_AFFILIATED_SKILL);
	if(skillId != 0)
	{
		RemoveSkill(skillId);
	}

	//修改装备等级和装备星级对属性的增加
	iResult = pItemBase->_unoperationAttributeFunc(0, this, pkItemEntry, slotIndex);
	if(iResult != RE_SUCCESS)
		return iResult;

	//物品修改技能CD功能
	if (pItemBase->GetSkillCDModifyFuncID() > 0)
	{
		iResult = pItemBase->_unoperationSkillCDModifyFunc(pItemBase->GetSkillCDModifyFuncID(), this);
		if (iResult != RE_SUCCESS)
		{
			return iResult;
		}
	}

	//修正currentHP
	FixMaxHPMP();

	return iResult;
}

int32_t BuddyInstance::GetBuddyCurrentTransformID()
{
	return m_Data.m_Data_Base.m_nCurrentTransformID;
}

ResultType BuddyInstance::SetBuddyCurrentTransformID(int id, bool force, bool sync)
{
	if(force)
	{
		m_Data.m_Data_Base.m_nCurrentTransformID = id;
		return RE_SUCCESS;
	}

	if(id == m_Data.m_Data_Base.m_nCurrentTransformID)
	{
		return RE_FAIL;
	}

	bool tag = false;
	for(int i=0; i<m_Data.m_Data_Base.m_nActiveTransformRecord.size(); ++i)
	{
		if(m_Data.m_Data_Base.m_nActiveTransformRecord[i] == id)
		{
			tag = true;
			break;
		}
	}
	
	if(false == tag)
	{
		return RE_FAIL;
	}
	
	m_Data.m_Data_Base.m_nCurrentTransformID = id;

	if(true == sync)
	{
		SyncBaseData();
	}

	LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if(NULL !=pkOwnerPlayer)
	{
		pkOwnerPlayer->BroadcastBuddyCurrentTransformID(GetAssignedPlayerId(), m_pConfig->_cID, id);
	}

	return RE_SUCCESS;
}

ResultType BuddyInstance::ActiveBuddyTransform(int id, bool useItem, bool sync, bool addAttr)
{
	LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if(NULL == pkOwnerPlayer)
		return RE_FAIL;

	CF_BuddyTransform::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_BuddyTransform>(id);
	if(NULL == pkData)
		return RE_FAIL;
	if(pkData->_buddy != GetTemplateID())
		return RE_BUDDYTRANSFORM_ERROR_2;
	for(int i=0; i<m_Data.m_Data_Base.m_nActiveTransformRecord.size(); ++i)
	{
		if(m_Data.m_Data_Base.m_nActiveTransformRecord[i] == id)
			return RE_BUDDYTRANSFORM_ERROR_1;
	}

	//useItem：使用皮肤卡道具激活
	//useItem=false，检测皮肤碎片道具
	//埋点日志
	vector<int> nParams;
	nParams.clear();
	nParams.push_back(id);
	int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, BUDDY_OPERATE,OPERATE_LOGTYPE4);
	if(false==useItem && !pkData->_kItems.empty())
	{
		PackOperateInfoList objOPList;
		LC_Helper::BuildConsumeItemOPs(pkData->_kItems, objOPList);
		ResultType res = LC_Helper::DoItemOPs(pkOwnerPlayer, objOPList, SYS_FUNC_TP_ACTIVE_BUDDY_TRANSFORM, id, true, true, nLogCode, nParams);
		if (RE_SUCCESS != res)
			return res;
	}
	
	m_Data.m_Data_Base.m_nActiveTransformRecord.push_back(id);
	
	int32_t totalTransfromNum = pkOwnerPlayer->GetTransformNum_NotInitID();
	if(totalTransfromNum > 0)
	{
		AchievementEvent kEvent;
		kEvent.SetNum(totalTransfromNum);
		pkOwnerPlayer->UpdateAchievementRecord(LC_ACHIEVEMENT_TYPE_ADD_BUDDY_TRANSFORM, kEvent);
	}

	AchievementEvent kEvent1;
	kEvent1.SetConditionInfo(id);
	kEvent1.SetNum(1);
	pkOwnerPlayer->UpdateAchievementRecord(LC_ACHIEVEMENT_TYPE_GET_TRANSFORM_ID, kEvent1);

	
	if(pkData->_iTime != -1)
	{
		int32_t currentTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();

		BuddyTransformInvaildTime info;
		info.m_nTransformID = id;
		info.m_nInvaildTime = currentTime+pkData->_iTime*ONE_HOUR_SECONDS;
		m_Data.m_Data_Base.m_nInvaildTransformTime.push_back(info);
	}
	
	if(sync)
	{
		NoticeActiveBuddyTransfromID(id);
		SyncBaseData();
	}

	return RE_SUCCESS;
}

ResultType BuddyInstance::AddBuddyTransformVaildTime(int id)
{
	LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if(NULL == pkOwnerPlayer)
	{
		return RE_FAIL;
	}

	CF_BuddyTransform::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_BuddyTransform>(id);
	if(NULL==pkData || pkData->_iTime==-1)
	{
		return RE_FAIL;
	}

	if(pkData->_buddy != GetTemplateID())
	{
		return RE_BUDDYTRANSFORM_ERROR_2;
	}

	bool tag = false;
	for(int i=0; i<m_Data.m_Data_Base.m_nActiveTransformRecord.size(); ++i)
	{
		if(m_Data.m_Data_Base.m_nActiveTransformRecord[i] == id)
		{
			tag = true;
		}
	}
	
	if(false == tag)
	{
		return RE_BUDDYTRANSFORM_ERROR_5;
	}

	bool flag = false;
	for(mem::vector<BuddyTransformInvaildTime>::iterator it=m_Data.m_Data_Base.m_nInvaildTransformTime.begin(); it!=m_Data.m_Data_Base.m_nInvaildTransformTime.end(); ++it)
	{
		if(it->m_nTransformID == id)
		{
			flag = true;
			it->m_nInvaildTime += pkData->_iTime*ONE_HOUR_SECONDS;
			break;
		}
	}

	if(true == flag)
	{
		SyncBaseData();
		return RE_SUCCESS;
	}

	return RE_BUDDYTRANSFORM_ERROR_6;
}

void BuddyInstance::DeactiveBuddyTransform(int id)
{
	//如果现在佩戴的是失效的皮肤，更换皮肤为默认皮肤
	int transfromID = GetBuddyCurrentTransformID();
	if(transfromID == id)
	{
		SetBuddyCurrentTransformID(m_pConfig->_iDefaultTransform, false, false);
	}

	//删除有效皮肤
	for(mem::vector<int32_t>::iterator it=m_Data.m_Data_Base.m_nActiveTransformRecord.begin(); it!=m_Data.m_Data_Base.m_nActiveTransformRecord.end(); )
	{
		if(*it == id)
		{
			it = m_Data.m_Data_Base.m_nActiveTransformRecord.erase(it);
		}
		else
		{
			++it;
		}
	}
}


void BuddyInstance::CheckBuddyTransformInvaild()
{
	bool tag = false;
	int32_t currentTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	for(mem::vector<BuddyTransformInvaildTime>::iterator it=m_Data.m_Data_Base.m_nInvaildTransformTime.begin(); it!=m_Data.m_Data_Base.m_nInvaildTransformTime.end();)
	{
		if(currentTime >= (*it).m_nInvaildTime)
		{
			tag = true;
			DeactiveBuddyTransform((*it).m_nTransformID);
			it = m_Data.m_Data_Base.m_nInvaildTransformTime.erase(it);
		}
		else
		{
			++it;
		}
	}

	if(true == tag)
	{
		SyncBaseData();
	}
}

void BuddyInstance::NoticeActiveBuddyTransfromID(int id)
{
	LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if(NULL == pkOwnerPlayer)
	{
		return;
	}

	MG_RLT_BuddyTransformOP rltMsg;
	rltMsg.m_nOpType = BuddyTransform_Active;
	rltMsg.m_nBuddyID = m_pConfig->_cID;
	rltMsg.m_nTransformID = id;
	rltMsg.m_nResult = RE_SUCCESS;
	pkOwnerPlayer->SendMsgToClient(MGPT_RLT_BUDDYTRANSFORMOP, &rltMsg);
}

int32_t BuddyInstance::GetTransformNum_NotInitID()
{
	int32_t num = m_Data.m_Data_Base.m_nActiveTransformRecord.size();
	return num>=1 ? num: 0;
}

void BuddyInstance::GetBuddyTransformID(mem::vector<int32_t>& transformID)
{
	transformID.clear();
	transformID.reserve(m_Data.m_Data_Base.m_nActiveTransformRecord.size());
	transformID = m_Data.m_Data_Base.m_nActiveTransformRecord;
}

ResultType BuddyInstance::_operationSuitAttribute(int iFuncID, int iSuitNum, mem::set<int32_t>* pkHappenedKeys)
{
	ItemSuitManager * inst = SafeGetSingleton<ItemSuitManager>();
	ItemSuitInfoType * pkSuitData = inst->GetSuitInfo(iFuncID);
	if (!pkSuitData)
	{
		return RE_ITEM_SYSTEM_ERROR;
	}

	LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if(NULL ==  pkOwnerPlayer)
	{
		return RE_FAIL;
	}

	//GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "buddy add attribute by operationsuit! playerAccount:[%s], buddy:[%d], funcID:[%d], suitNum:[%d]!", pkOwnerPlayer->GetUserName_utf8().c_str(), GetTemplateID(), iFuncID, iSuitNum);
	ItemEffectManage * ie_inst = ItemEffectManage::GetSingletonPtr();
	ItemSuitInfoType::ItemSuitEffectTypes effect;
	if(pkSuitData->GetEfectTypes(iSuitNum, effect, pkHappenedKeys))
	{
		for(ItemSuitInfoType::ItemSuitEffectTypes::iterator i = effect.begin(); i != effect.end(); ++i)
		{
			LC_ServerItem::_operationEquipAttrib(this, *i, 0, ie_inst, NULL);
		}
	}
	return RE_SUCCESS;
}

ResultType BuddyInstance::_unoperationSuitAttribute(int iFuncID, int iSuitNum, mem::set<int32_t>* pkHappenedKeys)
{
	ItemSuitManager * inst = SafeGetSingleton<ItemSuitManager>();
	ItemSuitInfoType * pkSuitData = inst->GetSuitInfo(iFuncID);
	if (!pkSuitData)
	{
		return RE_ITEM_SYSTEM_ERROR;
	}

	LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if(NULL ==  pkOwnerPlayer)
	{
		return RE_FAIL;
	}

	//GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "buddy remove attribute by unoperationsuit! playerAccount:[%s], buddy:[%d], funcID:[%d], suitNum:[%d]!", pkOwnerPlayer->GetUserName_utf8().c_str(), GetTemplateID(), iFuncID, iSuitNum);

	ItemEffectManage * ie_inst = ItemEffectManage::GetSingletonPtr();

	ItemSuitInfoType::ItemSuitEffectTypes effect;
	if(pkSuitData->GetEfectTypes(iSuitNum, effect, pkHappenedKeys))
	{
		for(ItemSuitInfoType::ItemSuitEffectTypes::iterator i = effect.begin(); i != effect.end(); ++i)
		{
			LC_ServerItem::_unoperationEquipAttrib(this, *i,0,ie_inst, NULL);
		}
	}
	return RE_SUCCESS;
}

void BuddyInstance::RefreshSkillUsableCount()
{
	m_Data.m_Data_PlayerBase.m_kSkillAsset.GetSkillAssetData()->RefreshSkillUsableCount();
}

ResultType BuddyInstance::TryReinforceEquipSlot(int itemID, int equipSlotIndex, int& newLevel, bool sync)
{
	int oldLevel = GetEquipSlotLevelBySlotIndex(equipSlotIndex);
	newLevel = oldLevel;
	int level = oldLevel + 1;
	CF_EquipSlotReinforce::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_EquipSlotReinforce>(level);
	if(NULL == pkData)
	{
		return RE_REINFORCE_EQUIPSLOT_ERROR_3;
	}
	
	LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	int buddyLevel = GetLevel();	
	if(NULL == pkOwnerPlayer)
	{
		return RE_FAIL;
	}

	if(buddyLevel < pkData->_iBuddyLevel)
	{
		return RE_REINFORCE_EQUIPSLOT_BUDDYLEVEL_NOT_ENOUGH;
	}

	CF_ItemList::DataEntry* pkEntry = SafeGetCSVFileDataEntryPtr<CF_ItemList>(itemID);
	if(NULL == itemID)
	{
		return RE_FAIL;
	}

	if(pkEntry->_lLevel < pkData->_iEquipLevel)
	{
		return RE_REINFORCE_EQUIPSLOT_ERROR_2;
	}
	
	UT_SIMDataInfo info;
	int32_t nItemId;
	int32_t nCount;
	Utility::UT_SIMDataList::iterator itItem=pkData->_kItem.begin(); 
	for(; itItem!=pkData->_kItem.end(); ++itItem)
	{
		nItemId = itItem->GetItemId();
		nCount = itItem->GetItemNum();
		ResultType res = pkOwnerPlayer->TryDeleteItem(nItemId, nCount);
		if(RE_SUCCESS != res)
		{
			return res;
		}
	}
	int buddyID = GetTemplateID();
	//纹章雕刻--埋点日志
	vector<int> nParams;
	nParams.clear();
	nParams.push_back(buddyID);
	nParams.push_back(itemID);
	nParams.push_back(oldLevel);
	nParams.push_back(level);
	int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, WENZHANG_OPERATE, OPERATE_LOGTYPE2);
	for(itItem=pkData->_kItem.begin(); itItem!=pkData->_kItem.end(); ++itItem)
	{
		nItemId = itItem->GetItemId();
		nCount = itItem->GetItemNum();
		LC_Helper::DeleteItemByItemId(pkOwnerPlayer->GetPackAsset(),nItemId, nCount, true,nLogCode, nParams);
		//pkOwnerPlayer->DeleteItem(nItemId, nCount, nLogCode, nParams);
	}

	newLevel = level;
	UpdataEquipSlotLevel(equipSlotIndex, newLevel);

	if(sync)
	{
		pkOwnerPlayer->SyncBackPackToClient(PACK_TYPE_BASIC);
	}
	
	return RE_SUCCESS;
}

ResultType BuddyInstance::TryEquipSlotStar(int equipSlotIndex, int& newStarLevel)
{
	ResultType res = RE_FAIL;
	LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if(NULL == pkOwnerPlayer)
	{
		return res;
	}

	int buddyLevel = GetLevel();
	int buddyID = GetTemplateID();
	int oldStarLevel = GetEquipSlotStarLevelBySlotIndex(equipSlotIndex);
	int nextStarLevel = oldStarLevel + 1;
	bool tag = false;
	int nItemId = 0;
	CF_EquipSlotStar::DataEntry* pkEntry = NULL;
	for(CF_EquipSlotStar::DataEntryMapExternIterator kIter=CF_EquipSlotStar::GetSingletonPtr()->GetIterator(); kIter.HasMoreElements(); kIter.MoveNext())
	{
		pkEntry = kIter.PeekNextValuePtr();
		if(pkEntry->_StarUpType==equipSlotIndex && pkEntry->_StarLevel==oldStarLevel)
		{
			tag = true;
			break;
		}
	}

	bool flag = false;
	for(CF_EquipSlotStar::DataEntryMapExternIterator iter=CF_EquipSlotStar::GetSingletonPtr()->GetIterator(); iter.HasMoreElements(); iter.MoveNext())
	{
		CF_EquipSlotStar::DataEntry* pkData = iter.PeekNextValuePtr();
		if(pkData->_StarUpType==equipSlotIndex && pkData->_StarLevel==nextStarLevel)
		{
			flag = true;
			break;
		}
	}
	
	if(!flag)
	{
		return RE_EQUIPSLOT_STAR_LEVEL_BE_LIMIT;
	}

	//check消耗
	if(NULL!=pkEntry && tag)
	{
		//等级是否达到
		if(buddyLevel < pkEntry->_iBuddyLevel)
		{
			return RE_EQUIPSLOT_STAR_ERROR_BUDDYLEVEL_NOT_ENOUGH;
		}

		//check道具是否足够
		for(Utility::UT_SIMDataList::iterator it=pkEntry->_kItem.begin(); it!=pkEntry->_kItem.end(); ++it)
		{
			res = pkOwnerPlayer->TryDeleteItem(it->IID(), it->ParamA());
			if(RE_SUCCESS != res)
			{
				return RE_EQUIPSLOT_STAR_ERROR_LIMIT_ITEM;
			}
		}

		//check金币是否足够
		if(pkEntry->_ConsumeMoney > 0)
		{
			bool flag = pkOwnerPlayer->IsCashEnough(pkEntry->_ConsumeMoney, CT_UNBIND_CASH);
			if(!flag)
			{
				return RE_CASH_NOT_ENOUGH;
			}
		}
	}
	//纹章淬炼--埋点日志
	vector<int> nParams;
	nParams.clear();
	nParams.push_back(buddyID);
	const LC_BackPackEntry* pkEquipEntry = GetEquipPack().GetEquipEntry(equipSlotIndex);
	nItemId = pkEquipEntry->GetBackPackValueByType(LPSFET_TYPE_ID);
	nParams.push_back(nItemId);
	nParams.push_back(oldStarLevel);
	nParams.push_back(nextStarLevel);
	int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, WENZHANG_OPERATE, OPERATE_LOGTYPE3);
	//消耗
	if(NULL!=pkEntry && tag)
	{
		for(Utility::UT_SIMDataList::iterator it=pkEntry->_kItem.begin(); it!=pkEntry->_kItem.end(); ++it)
		{
			res = pkOwnerPlayer->DeleteItem(it->IID(), it->ParamA(),nLogCode, nParams);
			if(RE_SUCCESS != res)
			{
				return RE_EQUIPSLOT_STAR_ERROR_LIMIT_ITEM;
			}
		}

		if(pkEntry->_ConsumeMoney > 0)
		{
			bool flag = pkOwnerPlayer->ReduceCash(pkEntry->_ConsumeMoney, CT_UNBIND_CASH, SYS_FUNC_TP_EQUIPSLOT_STAR, buddyID,nLogCode, nParams);
			if(!flag)
			{
				return RE_CASH_NOT_ENOUGH;
			}
		}
	}

	newStarLevel = oldStarLevel + 1;
	UpdataEquipSlotStarLevel(equipSlotIndex, newStarLevel);

	AchievementEvent kEvent;
	kEvent.SetNum(1);
	pkOwnerPlayer->UpdateAchievementRecord(LC_ACHIEVEMENT_TYPE_UPDATE_BUDDY_EQUIP_STARLEVEL, kEvent);

	pkOwnerPlayer->SyncBackPackToClient(PACK_TYPE_BASIC);
	return RE_SUCCESS;
}


void BuddyInstance::OnIncActivatedSkillCount()
{
	m_Data.m_Data_Base.m_nTotalActivatedSkillCount++;
}

void BuddyInstance::GetEquipSlotStarAttribiteMap(int slotIndex, int level, LC_ActorAttributeMap& attrMap)
{
	for(CF_EquipSlotStar::DataEntryMapExternIterator kIter=CF_EquipSlotStar::GetSingletonPtr()->GetIterator(); kIter.HasMoreElements(); kIter.MoveNext())
	{
		CF_EquipSlotStar::DataEntry* pkEntry = kIter.PeekNextValuePtr();
		if(pkEntry->_StarUpType==slotIndex && pkEntry->_StarLevel==level)
		{
			const Utility::UT_SIMDataList& attr = pkEntry->Attribute;
			for(Utility::UT_SIMDataList::const_iterator it=attr.begin(); it!=attr.end(); ++it)
			{
				attrMap.ModifyAttributeByPart(it->ParamA(), it->ParamB(), it->ParamC(), true);
			}
			break;
		}
	}
}

int32_t BuddyInstance::GetEquipSlotStarAttribiteChange(int slotIndex, int level)
{
	int32_t attrChange = 0;
	for(CF_EquipSlotStar::DataEntryMapExternIterator kIter=CF_EquipSlotStar::GetSingletonPtr()->GetIterator(); kIter.HasMoreElements(); kIter.MoveNext())
	{
		CF_EquipSlotStar::DataEntry* pkEntry = kIter.PeekNextValuePtr();
		if(pkEntry->_StarUpType==slotIndex && pkEntry->_StarLevel==level)
		{
			attrChange = pkEntry->_PropertySpecialChange;
			break;
		}
	}

	return attrChange;
}

void BuddyInstance::EquipAllItem(bool login)
{
	LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if(NULL == pkOwnerPlayer)
	{
		return;
	}

	for(int i=0; i<GetGlobalSetting.ArtifactPackSize; ++i)
	{
		const LC_BackPackEntry* pkEquipEntry = GetEquipPack().GetEquipEntry(i);
		if(pkEquipEntry && pkEquipEntry->GetBackPackValueByType(LPSFET_VALID))
		{	
			//增加装备附属技能
			int32_t skillId = pkEquipEntry->GetBackPackValueByType(LCSPET_EQUIP_AFFILIATED_SKILL);
			if(skillId != 0)
			{
				int32_t slot = -1;
				LearnSkill(skillId, slot);
			}
		}
	}

	GetBuddyAttrMgr().RefreshEquipAttrMap(pkOwnerPlayer, this);
	GetBuddyAttrMgr().RefreshHeroAllAttributeMap(pkOwnerPlayer, this);
	//刷新此英雄 提供的灵魂链属性
	GetBuddyAttrMgr().RefreshHeroGiveToOtherCoreAttrMap(pkOwnerPlayer);
	//刷新灵魂链上玩家获得灵魂链属性
	pkOwnerPlayer->RefreshChainBuddyCoreAttrByOnChain(GetTemplateID(), login, SPART_Buddy_4);
	GetBuddyAttrMgr().RefreshNewFinalAttrMap(pkOwnerPlayer, this, login, SPART_Buddy_4);
}

ResultType BuddyInstance::FlushControllerSkillState(LC_ActorStateMap& input)
{
	char_data_define::save_skill_state_info info;
	input.SetSaveStateToPB(info);
	m_Data.m_Data_ActorBase.m_kSkillStateMap->GetSaveStateFromPB(info);
	return RE_SUCCESS;
}

ResultType BuddyInstance::ActiveSkillState(int lStateTypeID, int lStateLifeTime, object_id_type lStateAttacherID, float fPara0, float fPara1, float fPara2, float fPara3, float fPara4)
{
	SK_Factory* pkFactory = SK_Factory::GetSingletonPtr();
	SK_State* pkSkillState = pkFactory->RequestSkillState(lStateTypeID);
	if (NULL == pkSkillState)
	{
		return RE_SKILL_STATE_ADD_FAIL;
	}
#if 0
	// BuddyInstance的buff不需要后续逻辑
	if ((GetDead() || GetDying()) && !pkSkillState->GetProperty_AttachOnDead())
	{
		return RE_SKILL_STATE_ADD_FAIL_FULL;
	}
	ResultType iResult = pkSkillState->CheckActorSkillFlag(GetAllSkillFlag());

	if (iResult == RE_SKILL_CHECK_GOD_CANT  )
	{
		if ( GOD_ITEM_4 == GetStateExtendData(GOD_STATE_ID, GOD_STATE_ITEMTYPE))
			return RE_SKILL_CHECK_GOD_CANT;
		else
			iResult = RE_SKILL_CHECK_SKILLFLAG_SUCCESS;
	}

	if (iResult != RE_SKILL_CHECK_SKILLFLAG_SUCCESS)
	{
#ifndef T6_CLIENT
		SK_SkillEventManagerBase* pkEventManager = ENGINE_GET_SKILL_EVENT_MANAGER();
		SK_SkillEvent* pkEvent = pkEventManager->AllocEvent();
		if (NULL != pkEvent)
		{
			pkEvent->SetEventType(SETE_ATTACH_STATE);
			pkEvent->SetSrcActorID(lStateAttacherID);
			pkEvent->SetDstActorID(GetID());
			pkEvent->SetLogicID(0);
			pkEvent->SetParam(SK_AttachStateEvent::SEP_STATE_ID, lStateTypeID);
			pkEvent->SetParam(SK_AttachStateEvent::SEP_RESULT, SK_AttachStateEvent::SASR_SKILLFLAG_EXCLUDE);
			pkEventManager->AddEvent(pkEvent);
		}
#endif
		return RE_SKILL_STATE_ADD_FAIL;
	}
	iResult = pkSkillState->StatePreCheck(this, lStateAttacherID, lStateLifeTime, fPara0, fPara1, fPara2, fPara3, fPara4);
	if (iResult != RE_SKILL_SCRIPT_NORMAL)
	{
		return RE_SKILL_STATE_ADD_FAIL;
	}
#endif
	ResultType eResultType = m_Data.m_Data_ActorBase.m_kSkillStateMap->AddSkillStateToStateMap(lStateTypeID, lStateLifeTime, lStateAttacherID, fPara0, fPara1, fPara2, fPara3, fPara4);
	return eResultType;
}

ResultType BuddyInstance::ActiveSkillState(Skill::SkillStateParam& rParam)
{
	SK_Factory* pkFactory = SK_Factory::GetSingletonPtr();
	SK_State* pkSkillState = pkFactory->RequestSkillState(rParam.lStateTypeID);
	if (NULL == pkSkillState)
	{
		return RE_SKILL_STATE_ADD_FAIL;
	}
#if 0
	// BuddyInstance的buff不需要后续逻辑
	if ((GetDead() || GetDying()) && !pkSkillState->GetProperty_AttachOnDead())
	{
		return RE_SKILL_STATE_ADD_FAIL_FULL;
	}
	ResultType iResult = pkSkillState->CheckActorSkillFlag(GetAllSkillFlag());
	if (iResult != RE_SKILL_CHECK_SKILLFLAG_SUCCESS)
	{
#ifndef T6_CLIENT
		SK_SkillEventManagerBase* pkEventManager = ENGINE_GET_SKILL_EVENT_MANAGER();
		SK_SkillEvent* pkEvent = pkEventManager->AllocEvent();
		if (NULL != pkEvent)
		{
			pkEvent->SetEventType(SETE_ATTACH_STATE);
			pkEvent->SetSrcActorID(rParam.lStateAttacherID);
			pkEvent->SetDstActorID(GetID());
			pkEvent->SetLogicID(0);
			pkEvent->SetParam(SK_AttachStateEvent::SEP_STATE_ID, rParam.lStateTypeID);
			pkEvent->SetParam(SK_AttachStateEvent::SEP_RESULT, SK_AttachStateEvent::SASR_SKILLFLAG_EXCLUDE);
			pkEventManager->AddEvent(pkEvent);
		}
#endif
		return RE_SKILL_STATE_ADD_FAIL;
	}
	iResult = pkSkillState->StatePreCheck(this, rParam);
	if (iResult != RE_SKILL_SCRIPT_NORMAL)
	{
		return RE_SKILL_STATE_ADD_FAIL;
	}
#endif
	ResultType eResultType = m_Data.m_Data_ActorBase.m_kSkillStateMap->AddSkillStateToStateMap(rParam);
	return eResultType;
}

ResultType BuddyInstance::DeactiveSkillState(int lStateTypeID)
{
	return m_Data.m_Data_ActorBase.m_kSkillStateMap->RemoveSkillStateInStateMap(lStateTypeID);
}

int BuddyInstance::DeactiveSkillState(int iBuffType, int iKind, int iLevel, int iDeactiveMaxCount)
{
	FilerStateOPer oper(iBuffType, iKind, iLevel);
	m_Data.m_Data_ActorBase.m_kSkillStateMap->ForAllState(&oper);

	LC_SkillIDSet::iterator it = oper.kStates.begin();
	for (; it != oper.kStates.end(); ++it)
	{
		m_Data.m_Data_ActorBase.m_kSkillStateMap->RemoveSkillStateInStateMap(*it);
	}
	return (int)oper.kStates.size();
}

void BuddyInstance::FixMaxHPMP()
{
	LC_ServerPlayer* m_pOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if (m_pOwnerPlayer == NULL)
		return;

	LC_ServerPlayer* pPlayer = m_pOwnerPlayer->GetActivateBuddyByID(m_pConfig->_cID);
	if ( pPlayer == NULL )
		return;

	pPlayer->FixMaxHPMP();
}

uint64_t BuddyInstance::GetAllBaseAttributeCombatScore()
{
	return GetBuddyAttrMgr().GetBuddyCombatScoreByType(BCST_BuddyRank);
}

void BuddyInstance::SyncBuddyRuneStarLevelInfo()
{
	LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if(NULL == pkOwnerPlayer)
		return;

	int totalActivedSkillCount = GetTotalActivatedSkillCount();
	MG_SyncBuddyRuneStarLevelInfo syncMsg;
	syncMsg.m_nBuddyID = m_pConfig->_cID;
	syncMsg.m_bActivateAllSkill = totalActivedSkillCount==4 ? true : false;
	GetBuddyRuneInfo(syncMsg.m_nBuddyRuneStarLevelInfo);
	pkOwnerPlayer->SendMsgToClient(MGPT_SYNC_BUDDYRUNESTARLEVELINFO, &syncMsg, pkOwnerPlayer);
}

void BuddyInstance::GetBuddyRuneInfo(std::map<int32_t,PS_RuneItemInfo_BuddyCS>& info)
{
	GetRunePack().GetBuddyRuneInfo(info);
}

uint64_t BuddyInstance::GetBuddyEquipCombatScore()
{
	return GetBuddyAttrMgr().GetBuddyCombatScoreByType(BCST_BuddyEquip);
}

ResultType BuddyInstance::OneKeyReinforceEquipSlot(int32_t equipSlotIndex, int32_t& newLevel)
{
	ResultType res = RE_FAIL;
	LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if(NULL == pkOwnerPlayer)
	{
		return res;
	}

	const LC_BackPackEntry* pkEquipEntry = GetEquipPack().GetEquipEntry(equipSlotIndex);
	if(NULL == pkEquipEntry || false==pkEquipEntry->GetBackPackValueByType(LPSFET_VALID))
	{
		return RE_REINFORCE_EQUIPSLOT_ERROR_1;
	}

	int oldLevel = GetEquipSlotLevelBySlotIndex(equipSlotIndex);
	int maxLevel = CF_EquipSlotReinforce::GetSingletonPtr()->GetCurrentMaxID();
	for(int i=oldLevel; i<maxLevel; ++i)
	{
		res = TryReinforceEquipSlot(pkEquipEntry->GetBackPackValueByType(LPSFET_TYPE_ID), equipSlotIndex, newLevel, false);
		if(RE_SUCCESS != res)
		{
			break;
		}
	}

	if(oldLevel == newLevel)
	{
		return res;
	}

	//更新装备槽的属性
	GetBuddyAttrMgr().RefreshEquipAttrMap(pkOwnerPlayer,this);
	GetBuddyAttrMgr().RefreshHeroAllAttributeMap(pkOwnerPlayer,this);
	//刷新此英雄 提供的灵魂链属性
	GetBuddyAttrMgr().RefreshHeroGiveToOtherCoreAttrMap(pkOwnerPlayer);
	//刷新灵魂链上玩家获得灵魂链属性
	pkOwnerPlayer->RefreshChainBuddyCoreAttrByOnChain(GetTemplateID(), false, SPART_Buddy_5);
	GetBuddyAttrMgr().RefreshNewFinalAttrMap(pkOwnerPlayer,this, false, SPART_Buddy_5);

	int buddyID = GetTemplateID();
	int currentSlotlevel = GetEquipSlotLevelBySlotIndex(equipSlotIndex);
	int equipSlotTotalLevel = GetEquipSlotTotalLevel();
	pkOwnerPlayer->BroadcastBuddyEquipSlotLevelCount(GetAssignedPlayerId(), buddyID, equipSlotTotalLevel);
	pkOwnerPlayer->UpdateRankInfoToRedis(Sub_Reward_Type_BuddyEquipSlotCombatScore,  false, buddyID);

	//achievement
	{
		AchievementEvent kEvent;
		kEvent.SetConditionInfo(buddyID, currentSlotlevel);
		kEvent.SetNum(1);
		pkOwnerPlayer->UpdateAchievementRecord(LC_ACHIEVEMENT_TYPE_BUDDY_EQUIPSLOT_LEVEL, kEvent);

		AchievementEvent kEvent2;
		kEvent2.SetConditionInfo(buddyID, equipSlotIndex);
		kEvent2.SetNum(currentSlotlevel);
		pkOwnerPlayer->UpdateAchievementRecord(LC_ACHIEVEMENT_TYPE_UPGRADE_BUDDY_EQUIPSLOT, kEvent2);

		AchievementEvent kEvent3;
		kEvent3.SetConditionInfo(buddyID);
		kEvent3.SetNum(equipSlotTotalLevel);
		pkOwnerPlayer->UpdateAchievementRecord(LC_ACHIEVEMENT_TYPE_BUDDY_EQUIPSLOT_TOTALLEVEL, kEvent3);
	}

	return RE_SUCCESS;
}

ResultType BuddyInstance::ReinforceEquipSlot(int equipSlotIndex, int32_t& newLevel)
{
	ResultType res = RE_FAIL;
	LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if(NULL == pkOwnerPlayer)
	{
		return res;
	}

	const LC_BackPackEntry* pkEquipEntry = GetEquipPack().GetEquipEntry(equipSlotIndex);
	if(NULL == pkEquipEntry || false==pkEquipEntry->GetBackPackValueByType(LPSFET_VALID))
	{
		return RE_REINFORCE_EQUIPSLOT_ERROR_1;
	}

	int oldLevel = GetEquipSlotLevelBySlotIndex(equipSlotIndex);
	res = TryReinforceEquipSlot(pkEquipEntry->GetBackPackValueByType(LPSFET_TYPE_ID), equipSlotIndex, newLevel);
	if(RE_SUCCESS == res)
	{
		//更新装备槽的属性
		GetBuddyAttrMgr().RefreshEquipAttrMap(pkOwnerPlayer,this);
		GetBuddyAttrMgr().RefreshHeroAllAttributeMap(pkOwnerPlayer,this);
		//刷新此英雄 提供的灵魂链属性
		GetBuddyAttrMgr().RefreshHeroGiveToOtherCoreAttrMap(pkOwnerPlayer);
		//刷新灵魂链上玩家获得灵魂链属性
		pkOwnerPlayer->RefreshChainBuddyCoreAttrByOnChain(GetTemplateID(), false, SPART_Buddy_5);
		GetBuddyAttrMgr().RefreshNewFinalAttrMap(pkOwnerPlayer,this, false, SPART_Buddy_5);

		int buddyID = GetTemplateID();
		int currentSlotlevel = GetEquipSlotLevelBySlotIndex(equipSlotIndex);
		int equipSlotTotalLevel = GetEquipSlotTotalLevel();

		pkOwnerPlayer->BroadcastBuddyEquipSlotLevelCount(GetAssignedPlayerId(), buddyID, equipSlotTotalLevel);
		pkOwnerPlayer->UpdateRankInfoToRedis(Sub_Reward_Type_BuddyEquipSlotCombatScore,  false, buddyID);
		
		//achievement
		{
			AchievementEvent kEvent;
			kEvent.SetConditionInfo(buddyID, currentSlotlevel);
			kEvent.SetNum(1);
			pkOwnerPlayer->UpdateAchievementRecord(LC_ACHIEVEMENT_TYPE_BUDDY_EQUIPSLOT_LEVEL, kEvent);

			AchievementEvent kEvent2;
			kEvent2.SetConditionInfo(buddyID, equipSlotIndex);
			kEvent2.SetNum(currentSlotlevel);
			pkOwnerPlayer->UpdateAchievementRecord(LC_ACHIEVEMENT_TYPE_UPGRADE_BUDDY_EQUIPSLOT, kEvent2);

			AchievementEvent kEvent3;
			kEvent3.SetConditionInfo(buddyID);
			kEvent3.SetNum(equipSlotTotalLevel);
			pkOwnerPlayer->UpdateAchievementRecord(LC_ACHIEVEMENT_TYPE_BUDDY_EQUIPSLOT_TOTALLEVEL, kEvent3);
		}
	}

	return res;
}

ResultType BuddyInstance::EquipSlotStar(int equipSlotIndex, int32_t& newStarLevel)
{
	ResultType res = RE_FAIL;
	LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayerByBuddyInstance();
	if(NULL == pkOwnerPlayer)
	{
		return res;
	}
	
	bool open = pkOwnerPlayer->GetBuddyEquipSlotStarState();
	if(!open)
	{
		return RE_EQUIPSLOT_STAR_NOT_OPEN;
	}

	const LC_BackPackEntry* pkItemData = GetEquipPack().GetEquipEntry(equipSlotIndex);
	if (!pkItemData || !pkItemData->GetBackPackValueByType(LPSFET_VALID))
	{
		return RE_EQUIPSLOT_STAR_INDEX_VAILD;
	}

	int oldStarLevel = GetEquipSlotStarLevelBySlotIndex(equipSlotIndex);
	res = TryEquipSlotStar(equipSlotIndex, newStarLevel);
	if(RE_SUCCESS == res)
	{
		//更新属性
		// 装备升星前战力
		GetBuddyAttrMgr().RefreshEquipAttrMap(pkOwnerPlayer,this);
		GetBuddyAttrMgr().RefreshHeroAllAttributeMap(pkOwnerPlayer,this);
		//刷新此英雄 提供的灵魂链属性
		GetBuddyAttrMgr().RefreshHeroGiveToOtherCoreAttrMap(pkOwnerPlayer);
		//刷新灵魂链上玩家获得灵魂链属性
		pkOwnerPlayer->RefreshChainBuddyCoreAttrByOnChain(GetTemplateID(), false, SPART_Buddy_6);
		GetBuddyAttrMgr().RefreshNewFinalAttrMap(pkOwnerPlayer,this, false, SPART_Buddy_6);

		//achievement
		{
			int level = GetEquipSlotStarLevelBySlotIndex(equipSlotIndex);
			AchievementEvent kEvent;
			kEvent.SetConditionInfo(GetTemplateID(), level);
			kEvent.SetNum(1);
			pkOwnerPlayer->UpdateAchievementRecord(LC_ACHIEVEMENT_TYPE_BUDDY_EQUIPSLOT_STARLEVEL, kEvent);
		}

		pkOwnerPlayer->BroadcastBuddyEquipSlotStarLevel(GetAssignedPlayerId(), GetTemplateID(), equipSlotIndex, newStarLevel);
		pkOwnerPlayer->UpdateRankInfoToRedis(Sub_Reward_Type_BuddyEquipSlotCombatScore,  false, GetTemplateID());
	}
	return res;
}

uint64_t BuddyInstance::GetBuddySkillCombatScore()
{
	LC_SkillAsset* skillAsset = GetSkillAsset();
	if(NULL == skillAsset)
	{
		return 0;
	}

	return skillAsset->GetSkillCombatScore();
}

int BuddyInstance::GetQuality()
{
	// 字段修改之后这里直接返回 
	// return m_pConfig->_quelity;
	int nResult = eBuddyQuelity_R;
	if (m_pConfig->_quality == "R" )
		nResult = eBuddyQuelity_R;
	else if (m_pConfig->_quality == "SR" )
		nResult = eBuddyQuelity_SR;
	if (m_pConfig->_quality == "SSR" )
		nResult = eBuddyQuelity_SSR;
	if (m_pConfig->_quality == "UR" )
		nResult = eBuddyQuelity_UR;

	return nResult;
}


bool BuddyInstance::CheckEquipUpgradeConfig()
{
	int nUpgradeLevel = GetEquipPack().GetEquipGradeLevel();
	const LC_BackPackEntry* pkEntry_0 = GetEquipPack().GetEquipEntry(ET_HEAD);
	const LC_BackPackEntry* pkEntry_1 = GetEquipPack().GetEquipEntry(ET_BODY);
	const LC_BackPackEntry* pkEntry_2 = GetEquipPack().GetEquipEntry(ET_FOOT);
	const LC_BackPackEntry* pkEntry_3 = GetEquipPack().GetEquipEntry(ET_NECKLACE);
	if( NULL == pkEntry_0 || NULL == pkEntry_1||NULL == pkEntry_2||NULL == pkEntry_3) 
		return false;

	int nTypeId_0 = pkEntry_0->GetBackPackValueByType(LPSFET_TYPE_ID);
	int nTypeId_1 = pkEntry_1->GetBackPackValueByType(LPSFET_TYPE_ID);
	int nTypeId_2 = pkEntry_2->GetBackPackValueByType(LPSFET_TYPE_ID);
	int nTypeId_3 = pkEntry_3->GetBackPackValueByType(LPSFET_TYPE_ID);

	if( 0 >= nTypeId_0 ||0 >= nTypeId_1 ||0 >= nTypeId_2 ||0 >= nTypeId_3 )
		return false;

	CF_ItemList* pkCSVItemList = CF_ItemList::GetSingletonPtr();
	CF_ItemList::DataEntry* pkData_0 = pkCSVItemList->GetEntryPtr(nTypeId_0);
	CF_ItemList::DataEntry* pkData_1 = pkCSVItemList->GetEntryPtr(nTypeId_1);
	CF_ItemList::DataEntry* pkData_2 = pkCSVItemList->GetEntryPtr(nTypeId_2);
	CF_ItemList::DataEntry* pkData_3 = pkCSVItemList->GetEntryPtr(nTypeId_3);
	if (NULL == pkData_0 || NULL == pkData_1 || NULL == pkData_2 || NULL == pkData_3)
		return false;

	int nLevel_0 = pkData_0->_lLevel;
	int nLevel_1 = pkData_1->_lLevel;
	int nLevel_2 = pkData_2->_lLevel;
	int nLevel_3 = pkData_3->_lLevel;
	if( nUpgradeLevel == nLevel_0
		&& nUpgradeLevel == nLevel_1
		&& nUpgradeLevel == nLevel_2
		&& nUpgradeLevel == nLevel_3)
		return true;
	return false;
}
void BuddyInstance::EquipUpgrade()
{
	bool tag = GetEquipPack().AddEquipGradeLevel();
	if(tag)
	{
		SyncEquipGradeLevel();
	}
}
void BuddyInstance::OnSetLevel( int nCurrentLevel )
{
	// TODO: any logic
	CheckFlushRuneSlotDressState();
}
