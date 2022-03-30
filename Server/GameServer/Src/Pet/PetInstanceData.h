#ifndef __PET_INSTANCE_DATA_H__
#define __PET_INSTANCE_DATA_H__

#pragma once

#include "LC_DirtyFlag.h"
#include "LC_Define.h"

#include "char_data.pb.h"

#include <vector>
#include "MG_DatabaseStructMg.h"
#include "MG_ProtocolStruct.h"
#include "LC_SkillAsset.h"

using namespace GameLogic;

typedef mem::map<int, int>					LC_PlayerEquipSuitPtrMap;
typedef LC_PlayerEquipSuitPtrMap::iterator	LC_PlayerEquipSuitPtrMapIter;

class PetInstanceData_Base
{
public:
	PetInstanceData_Base()
		: m_nStarLevel(0)
	{
		m_nArrayProperty.resize(ATT_MAX_COUNT);
		for (int i = 0; i < ATT_MAX_COUNT; ++i )
			m_nArrayProperty[i] = 0;
	}
	~PetInstanceData_Base()
	{
	}
public:
	int8_t m_nStarLevel;
	mem::vector<raw_attr_value_type> m_nArrayProperty;
	LC_SkillAsset m_kSkillAsset;
};

class LC_ServerPlayer;

class PetInstanceData //: public GameLogic::LC_ServerPlayer
{
public:
	PetInstanceData(void){}
	~PetInstanceData(){};

	void Init(int32_t iID){}
	void Clear(void){}

	//int LoadFromDB();
	//int LoadFromTemplate(int32_t nTemplateId);

	//const PetInstance* Get();
	//int SaveData();
	// DBLogic
	// for base data
	PetInstanceData_Base m_Data_Base;
};

#endif //__ACT_ACTIVITY_ASSET_H__
