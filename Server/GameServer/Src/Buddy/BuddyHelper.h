#ifndef __BUDDY_HELPER_H__
#define __BUDDY_HELPER_H__

#pragma once

#include "LC_DirtyFlag.h"
#include "LC_Define.h"

#include "char_data.pb.h"
#include "CF_BuddyUpgrade.h"
#include "CF_PetUpgrade.h"
#include "CF_BotUpgrade.h"
#include "BuddyInstanceData.h"

class LC_ServerPlayer;
class BuddyInstance;

class BuddyHelper //: public 
{
public:
	static bool IS_ANTIWALLOW_RELATED(int32_t sys_func_type);

	static const CSVFile::CF_BuddyUpgrade::DataEntry* GetBuddyUpgradeData(int32_t nBuddyID, int32_t nLevel);
	static const CSVFile::CF_PetUpgrade::DataEntry* GetPetUpgradeData(int32_t nPetID, int32_t nLevel);
	static const CSVFile::CF_BotUpgrade::DataEntry* GetBotUpgradeData(int32_t nBotID, int32_t nLevel);
};

#endif //__ACT_ACTIVITY_ASSET_H__
