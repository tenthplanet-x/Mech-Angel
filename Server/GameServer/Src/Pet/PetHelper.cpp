#include "PetHelper.h"
#include "PetInstance.h"
#include "CF_Pet.h"
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


//bool PetHelper::IS_ANTIWALLOW_RELATED(int32_t sys_func_type)
//{
//	switch (sys_func_type)
//	{
//	case SYS_FUNC_TP_SIT:
//	case SYS_FUNC_TP_FARM:
//	case SYS_FUNC_TP_PICKUP:
//	case SYS_FUNC_TP_TASK_FINISH:
//	case SYS_FUNC_TP_TASK_LIVENESS:
//	case SYS_FUNC_TP_RAID_SETTLE:
//	case SYS_FUNC_TP_PROMOTE:
//	case SYS_FUNC_TP_STORY_REWARD:
//	case SYS_FUNC_TP_CMN_STAT_REWARD:
//	case SYS_FUNC_TP_CMN_STAT_REPLEVY:
//	case SYS_FUNC_TP_OFFLINE:
//		return true;
//		break;
//	default:
//		break;
//	}
//	return false;
//}
