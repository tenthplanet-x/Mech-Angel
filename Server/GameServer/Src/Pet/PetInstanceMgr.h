#ifndef __PET_INSTANCE_MGR_H__
#define __PET_INSTANCE_MGR_H__

#pragma once

#include "LC_DirtyFlag.h"
#include "LC_Define.h"

#include "SystemError.h"
#include "char_data.pb.h"
#include "CF_Pet.h"

//using namespace CSVFile;
class LC_ServerPlayer;
namespace Pet
{
	class PetInstance;
	class PetInstanceMgr
	{
	public:
		PetInstanceMgr();
		~PetInstanceMgr();

		void ReUse();
		void Init(object_id_type nOwnerPlayerId);
		void Clear();

		ResultType Activate(int petID=0);
		bool IsActivated();

		// GameLogic
		int CapturePet(int32_t nPetResId, bool bLottery = false);
		int EnablePet(int16_t nIndex);
		int DisablePet(bool bAuto);
		int Decompose(int16_t nIndex);
		int UpgradeStarLevel(int16_t nIndex, bool bForce = false);

		int GetCount();
		PetInstance* GetEnabledPetInstance();
		int32_t GetEnabledPetInstanceResId();
		int16_t GetEnabledPetInstanceIndex();
		PetInstance* GetPetInstance( int16_t nIndex );
		object_id_type GetOwnerPlayerId();
		LC_ServerPlayer* GetOwnerPlayer();
		// DBLogic
		PetInstance* CreatePetInstance( CSVFile::CF_Pet::DataEntry* pResData, int16_t nIndex );
		void SaveAll(char_data_define::pet_detail_data & detail_data);
		void LoadAll(const char_data_define::pet_detail_data & detail_data);
		void SyncAllPetData();
		void SyncPetBaseData();
		void SyncPetFullData();
		PetInstance* GetMaxCombatScoreInstance();
		int16_t GetMaxCombatScoreIndexByTemplate(int32_t nResId);
		uint64_t GetTotalCombatScore();
		const mem::map<int16_t, PetInstance*>& GetRawData();
	private:
		bool FindPetInstanceByTemplateId( CSVFile::CF_Pet::DataEntry* pResData );
		PetInstance* LoadSingleInstance( const char_data_define::pet_single_instance_data& data );
		void ReUse_PetInstance();
		PetInstance* AddPet( int32_t nPetResId );
		PetInstance* AddPet_Impl(CSVFile::CF_Pet::DataEntry* pResData, int nIndex = -1);
		int RemovePet( PetInstance* pInstance);
		int GenPetTypeData();
		int16_t TryAllocateIndex();
		int16_t AllocateIndex(int16_t nIndex);
		int16_t DeallocateIndex(int16_t nIndex);
		int DecomposePetInstance(int16_t nIndex, PetInstance* pInstance);
		int DropDecomposeItem(int nDropID);
		bool TryRefreshMaxCombatScoreByTemplate(int16_t nInputIndex);
		bool CheckDecompose(int16_t nIndex);
		
		void SetPetMaxCombatScoreInfo(int petID, int petIndex);
	private:
		bool m_nActivateState;
		object_id_type m_nOwnerPlayerId;
		int16_t m_nEnabledIndex;
		int16_t m_nMaxCombatScoreIndex;
		int16_t m_nMaxCombatScorePetID;
		mem::map<int16_t, PetInstance*> m_ppPetInstance;
		mem::set<int32_t> m_setPetIds;
		bool m_bIndexArray[MAX_PET_COUNT];
		bool m_bDecompose;
		friend class PetInstance;
	};
}

using namespace Pet;

#endif //__ACT_ACTIVITY_ASSET_H__
