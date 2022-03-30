#ifndef __PET_INSTANCE_H__
#define __PET_INSTANCE_H__

#pragma once

#include "LC_DirtyFlag.h"
#include "LC_Define.h"
#include "CF_SystemTitleList.h"
#include "CF_LevelUp.h"
#include "CF_Pet.h"
#include "CF_PetUpgrade.h"

#include "char_data.pb.h"

#include "PetInstanceData.h"

#define			DEFAULT_PET_STAR_LEVEL		1

class LC_ServerPlayer;

//namespace GameLogic
namespace Pet
{
class PetInstanceMgr;
class PetInstance : public Memory::MM_BaseObject
{
public:
	PetInstance(object_id_type nOwnerPlayerId, int16_t nIndex, PetInstanceMgr* pMgr);
	virtual ~PetInstance();
public:
	void InitTemplateData(const CSVFile::CF_Pet::DataEntry* pData, bool bInit);
	
	void Clear(void){}
	void InitGameData();

	LC_ServerPlayer* GetOwnerPlayer();

	void SyncAllDataToClient();
	void SyncSkillAsset();
	// Detail Sync
	//void Sync
	bool LearnSkill(int lSkillID, int& iSlotIdx, GameLogic::LC_SkillIDSet* pkReplacedSkills = NULL);
	bool RemoveSkill(int lSkillID);

	void OnCreate();
	// Data fetch
	int16_t GetIndex();
	uint32_t GetTemplateId();
	int16_t GetStarLevel();
	CSVFile::CF_Pet::DataEntry* GetConfig();
	int UpgradeStarLevel();
	int GetQuality();
	uint64_t GetCombatScore(){return m_nCombatScore;};
	void SetCombatScore(uint64_t score){m_nCombatScore = score; }
	const mem::vector<raw_attr_value_type>&  GetAttributeArray();
	int32_t GetCurrentDecomposeID();
	PetInstanceData &GetPetInstanceData(){return m_Data;}
private:
	// Create logic
	void InitSkillData();
	void InitPetSkills(const StringType& sRawSkills);
	void InitPetSkills(Utility::UT_SIMDataList& kRawSkills);
	void Save(char_data_define::pet_single_instance_data& data);
	void SaveBaseData(char_data_define::pet_base_data& data);
	void SaveFullData(char_data_define::pet_full_data& data);
	void SaveFullData_SkillAsset(char_data_define::skill_asset_info& data);

	void Load(const char_data_define::pet_single_instance_data& data);
	void LoadBaseData(const char_data_define::pet_base_data& data);
	void LoadFullData(const char_data_define::pet_full_data& data);
	void LoadFullData_SkillAsset(const char_data_define::skill_asset_info& data);


	LC_SkillAsset* GetSkillAsset();
	LC_SkillCoolDownMap* GetSkillCoolDownMap();
private:
	// 因为所有的check and reducing都在Manager做了，那边有完整的上下文，所以，这里的UpgradeStar仅仅就是个"Do"
	int DoUpgradeStar();
	void ConvertDropIds(StringType& strInput);
private:
	void OnLevelData( int nOrigStarLevel, int nStarLevel, bool bInit);
private:
	object_id_type m_nOwnerPlayerId;
	CSVFile::CF_Pet::DataEntry* m_pConfig;
	PetInstanceData m_Data;
	int16_t m_nIndex;
	uint64_t m_nCombatScore;
	PetInstanceMgr* m_pInstanceMgr;
	mem::vector<int32_t> m_vecDropId;

	friend class PetHelper;
	friend class PetInstanceMgr;
};
}

using namespace Pet;

#endif //__ACT_ACTIVITY_ASSET_H__
