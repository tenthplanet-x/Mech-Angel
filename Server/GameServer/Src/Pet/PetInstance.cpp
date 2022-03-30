#include "PetInstance.h"
#include "CF_Pet.h"
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
#include "MG_PetProtocol.h"
#include "PetHelper.h"
#include "LC_ShortcutBar.h"
#include "CF_ItemEffect.h"
#include "GlobalSettings.h"
#include "LC_GameEventManager.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerLogicManager.h"
#include "../Buddy/BuddyHelper.h"

using namespace GameLogic;
using namespace Skill;

#define			MAX_PLAYER_LEVEL		60

PetInstance::PetInstance(object_id_type nOwnerPlayerId, int16_t nIndex, PetInstanceMgr* pMgr)
	: m_nOwnerPlayerId(nOwnerPlayerId)
	, m_pConfig( NULL )
	, m_nIndex(nIndex)
	, m_nCombatScore(0)
	, m_pInstanceMgr(pMgr)
{
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_PlayerBase* pOwner = (LC_PlayerBase*)pkWorldManager->FindObject(m_nOwnerPlayerId);
	m_Data.m_Data_Base.m_kSkillAsset.setOwner(pOwner);
	m_vecDropId.clear();
}

PetInstance::~PetInstance()
{
	//T_SAFE_DELETE(m_Data.m_Data_PlayerBase.m_pkShortcutBar);
}

void PetInstance::InitGameData()
{
	LC_ServerPlayer* m_pOwnerPlayer = GetOwnerPlayer();
	if (m_pOwnerPlayer == NULL)
		return;
}

void PetInstance::InitTemplateData(const CSVFile::CF_Pet::DataEntry* pData, bool bInit)
{
	if (pData == NULL)
		return;

	m_pConfig = const_cast<CSVFile::CF_Pet::DataEntry*>(pData);

	m_Data.m_Data_Base.m_nStarLevel = m_pConfig->_InitLevel;
	if (bInit)
		ConvertDropIds( m_pConfig->_DropID );
	InitSkillData();
}

void PetInstance::InitSkillData()
{
	if (m_pConfig == NULL)
		return;

	InitPetSkills( m_pConfig->_skillid );
}

void PetInstance::InitPetSkills(const StringType& sRawSkills)
{
	Utility::UT_SIMDataList kRawSkills;
	Utility::ParseFmtString2DataIFs(sRawSkills, kRawSkills);
	InitPetSkills(kRawSkills);
}

void PetInstance::InitPetSkills(Utility::UT_SIMDataList& kRawSkills)
{
	Utility::UT_SIMDataList::iterator it = kRawSkills.begin();
	for (; it != kRawSkills.end(); ++it)
	{
		UT_SIMDataInfo info = *it;
		int32_t nSkillID = info.GetSkillID();
		int32_t nSkillEnableLevel = info.GetSkillEnableLevel();
		int32_t nSlot = -1;
		if ( GetStarLevel() >= nSkillEnableLevel )
			LearnSkill(nSkillID, nSlot);
	}
}

void PetInstance::Save(char_data_define::pet_single_instance_data& data)
{
	data.set_resid(m_pConfig->_cID);
	data.set_index(m_nIndex);
	SaveBaseData(*data.mutable_base_data());
	SaveFullData(*data.mutable_full_data());
}

void PetInstance::SaveBaseData(char_data_define::pet_base_data& data)
{
	data.set_star_level(m_Data.m_Data_Base.m_nStarLevel);
	data.set_quality(m_pConfig->_quality);
}

void PetInstance::SaveFullData(char_data_define::pet_full_data& data)
{
	SaveFullData_SkillAsset(*data.mutable_skill_info());
}

void PetInstance::SaveFullData_SkillAsset(char_data_define::skill_asset_info& data)
{
	LC_SkillAsset* pkSkillAsset = GetSkillAsset();
	LC_SkillCoolDownMap* pkSkillCDMap = GetSkillCoolDownMap();
	if (NULL != pkSkillAsset && NULL != pkSkillCDMap)
	{
		pkSkillAsset->SetDataToPB(data);
		pkSkillCDMap->SetCDToPB(data);
	}
}


void PetInstance::Load(const char_data_define::pet_single_instance_data& data)
{
	//data.set_resid(m_pConfig->_cID);
	LoadBaseData(data.base_data());
	LoadFullData(data.full_data());
}

void PetInstance::LoadBaseData(const char_data_define::pet_base_data& data)
{
	m_Data.m_Data_Base.m_nStarLevel = data.star_level();
}

void PetInstance::LoadFullData(const char_data_define::pet_full_data& data)
{
	LoadFullData_SkillAsset(data.skill_info());
}

void PetInstance::LoadFullData_SkillAsset(const char_data_define::skill_asset_info& data)
{
	LC_SkillAsset* pkSkillAsset = GetSkillAsset();
	if (NULL != pkSkillAsset)
	{
		pkSkillAsset->GetDataFromPB(data);
	}

	//获取技能CD信息
	LC_SkillCoolDownMap* pkSkillCDMap = GetSkillCoolDownMap();
	if (NULL != pkSkillCDMap)
	{
		pkSkillCDMap->GetCDFromPB(data);
	}
}


LC_SkillAsset* PetInstance::GetSkillAsset()
{
	return &(m_Data.m_Data_Base.m_kSkillAsset);
}

LC_SkillCoolDownMap* PetInstance::GetSkillCoolDownMap()
{
	return GetSkillAsset()->GetSkillCoolDownMap();
}

bool PetInstance::LearnSkill(int lSkillID, int& iSlotIdx, GameLogic::LC_SkillIDSet* pkReplacedSkills)
{
	if (!IS_SKILL_TYPE(lSkillID))
		return false;

	LC_SkillIDSet kSkillIDSet;
	ResultType result = m_Data.m_Data_Base.m_kSkillAsset.AddSkillToAsset(lSkillID, iSlotIdx, &kSkillIDSet);
	if(result == RE_SKILL_IS_LEARNED)
	{
		return false;
	}

	return true;
}

bool PetInstance::RemoveSkill(int lSkillID)
{
	return true;
}



LC_ServerPlayer* PetInstance::GetOwnerPlayer()
{
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerPlayer* pPlayer = (LC_ServerPlayer*)pkWorldManager->FindObject(m_nOwnerPlayerId);
	return pPlayer;
}

void PetInstance::SyncAllDataToClient()
{
	SyncSkillAsset();
}

void PetInstance::SyncSkillAsset()
{
	LC_ServerPlayer* pOwnerPlayer = GetOwnerPlayer();
	if (pOwnerPlayer == NULL)
		return;

	MG_SC_SyncPetSkill data;
	data.m_nIndex = GetIndex();
	m_Data.m_Data_Base.m_kSkillAsset.SetDataToProtocolStruct(data.m_kCharSkillInfo);
	pOwnerPlayer->SendMsgToClient(MGPT_SC_PET_SYNC_SKILLASSET, &data);
}

// for subordinate logic.
// commented by weixin
void PetInstance::OnCreate()
{
#if 0
	LC_ServerPlayer* m_pOwnerPlayer = GetOwnerPlayer();
	if (m_pOwnerPlayer == NULL)
		return;

	//m_pOwnerPlayer->GetSubordinateAsset()._disapplyReg(m_pOwnerPlayer);
	//m_pOwnerPlayer->GetSubordinateAsset()._applyReg(m_pOwnerPlayer);
	m_pOwnerPlayer->FlushSkillStateToOnePetInstance(m_pConfig->_cID);
	m_pOwnerPlayer->RefreshTitleAttribute();
	m_pOwnerPlayer->FlushAllControllerAttributeToOnePetInstance( m_pConfig->_cID, true, true);
	RecomputePetCombatScore();
#endif
}

int16_t PetInstance::GetIndex()
{
	return m_nIndex;
}

uint32_t PetInstance::GetTemplateId()
{
	if (m_pConfig == NULL)
		return INVALID_ACTOR_ID;
	return m_pConfig->_cID;
}

int16_t PetInstance::GetStarLevel()
{
	return m_Data.m_Data_Base.m_nStarLevel;
}

CSVFile::CF_Pet::DataEntry* PetInstance::GetConfig()
{
	return m_pConfig;
}

int PetInstance::UpgradeStarLevel()
{
	return true;
}

void PetInstance::ConvertDropIds(StringType& strInput)
{
	Utility::ParseFmtString2DataIFs(strInput, m_vecDropId);
	return;
}

int PetInstance::DoUpgradeStar()
{
	m_Data.m_Data_Base.m_nStarLevel++;

	LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayer();
	if (pkOwnerPlayer != NULL)
	{
		//顺序不能换
		pkOwnerPlayer->GetCommanderAttrMgr().RefreshPetAttributeMap(pkOwnerPlayer, false, SPART_Pet_2);
		m_pInstanceMgr->TryRefreshMaxCombatScoreByTemplate(GetIndex());
		pkOwnerPlayer->UpdateRankUserValueByRankType(Rank_Type_Pet);
	}

	InitSkillData();
	SyncSkillAsset();

	return true;
}


int PetInstance::GetQuality()
{
	// 字段修改之后这里直接返回 
	 return m_pConfig->_quality;
}


const mem::vector<raw_attr_value_type>&  PetInstance::GetAttributeArray()
{
	return m_Data.m_Data_Base.m_nArrayProperty;
}

int32_t PetInstance::GetCurrentDecomposeID()
{
	if (m_Data.m_Data_Base.m_nStarLevel < 0)
		return 0;
	if ( m_vecDropId.size() <= m_Data.m_Data_Base.m_nStarLevel )
		return 0;
	return m_vecDropId[m_Data.m_Data_Base.m_nStarLevel];
}
