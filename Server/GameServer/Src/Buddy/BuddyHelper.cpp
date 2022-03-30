#include "BuddyHelper.h"
#include "BuddyInstance.h"
#include "CF_Buddy.h"
#include "CF_SkillLevelUp.h"
#include "CF_SkillExecuteList.h"
#include "LC_ServerPlayer.h"
#include "LC_ItemFactoryBase.h"
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

using namespace GameLogic;
using namespace Skill;

bool BuddyHelper::IS_ANTIWALLOW_RELATED(int32_t sys_func_type)
{
	switch (sys_func_type)
	{
	case SYS_FUNC_TP_SIT:
	case SYS_FUNC_TP_FARM:
	case SYS_FUNC_TP_PICKUP:
	case SYS_FUNC_TP_TASK_FINISH:
	case SYS_FUNC_TP_TASK_LIVENESS:
	case SYS_FUNC_TP_RAID_SETTLE:
	case SYS_FUNC_TP_PROMOTE:
	case SYS_FUNC_TP_STORY_REWARD:
	case SYS_FUNC_TP_CMN_STAT_REWARD:
	case SYS_FUNC_TP_CMN_STAT_REPLEVY:
	case SYS_FUNC_TP_OFFLINE:
		return true;
		break;
	default:
		break;
	}
	return false;
}

const CSVFile::CF_BuddyUpgrade::DataEntry* BuddyHelper::GetBuddyUpgradeData(int32_t nBuddyID, int32_t nLevel)
{
	CSVFile::CF_BuddyUpgrade::DataEntry* pResult = NULL;
	CF_BuddyUpgrade* pConfig = CF_BuddyUpgrade::GetSingletonPtr();

	CF_BuddyUpgrade::DataEntryMapExternIterator it = pConfig->GetIterator();
	while( it.HasMoreElements() )
	{
		CF_BuddyUpgrade::DataEntry* pkData = it.PeekNextValuePtr();
		if ( nBuddyID == pkData->_cID && nLevel == pkData->_level )
		{
			pResult = pkData;
			break;
		}

		it.MoveNext();
	}
	return pResult;
}

const CSVFile::CF_PetUpgrade::DataEntry* BuddyHelper::GetPetUpgradeData(int32_t nPetID, int32_t nLevel)
{
	CSVFile::CF_PetUpgrade::DataEntry* pResult = NULL;
	CF_PetUpgrade* pConfig = CF_PetUpgrade::GetSingletonPtr();

	CF_PetUpgrade::DataEntryMapExternIterator it = pConfig->GetIterator();
	while( it.HasMoreElements() )
	{
		CF_PetUpgrade::DataEntry* pkData = it.PeekNextValuePtr();
		if ( nPetID == pkData->_cID && nLevel == pkData->_level )
		{
			pResult = pkData;
			break;
		}

		it.MoveNext();
	}
	return pResult;
}

const CSVFile::CF_BotUpgrade::DataEntry* BuddyHelper::GetBotUpgradeData(int32_t nBotID, int32_t nLevel)
{
	CSVFile::CF_BotUpgrade::DataEntry* pResult = NULL;
	CF_BotUpgrade* pConfig = CF_BotUpgrade::GetSingletonPtr();

	CF_BotUpgrade::DataEntryMapExternIterator it = pConfig->GetIterator();
	while( it.HasMoreElements() )
	{
		CF_BotUpgrade::DataEntry* pkData = it.PeekNextValuePtr();
		if ( nBotID == pkData->_cID && nLevel == pkData->_iLevel )
		{
			pResult = pkData;
			break;
		}

		it.MoveNext();
	}
	return pResult;
}
