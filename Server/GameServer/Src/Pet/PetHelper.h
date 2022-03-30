#ifndef __PET_HELPER_H__
#define __PET_HELPER_H__

#pragma once

#include "LC_DirtyFlag.h"
#include "LC_Define.h"

#include "char_data.pb.h"
#include "CF_PetUpgrade.h"
#include "PetInstanceData.h"

class LC_ServerPlayer;
//class PetInstance;

class PetHelper //: public 
{
public:
	//static bool IS_ANTIWALLOW_RELATED(int32_t sys_func_type);

	static const CSVFile::CF_PetUpgrade::DataEntry* GetPetUpgradeData(int32_t nPetID, int32_t nLevel);
};

#endif //__ACT_ACTIVITY_ASSET_H__
