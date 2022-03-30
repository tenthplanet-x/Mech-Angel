#include "LM_LocaleManager.h"
#include "HL_PacketHandler.h"
#include "NW_ServerSession.h"
#include "LC_ServerNPCInteractionManager.h"
#include "LC_ServerPlayerEnterLeaveManager.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "GameServerApp.h"
#include "LC_SequenceManager.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerNPCManager.h"
#include "LC_ServerNPC.h"
#include "LC_StoryLogic_Manager.h"
#include "LC_NPCTransferManager.h"
#include "LC_ServerActorOperationManager.h"
#include "time.h"
#include "UT_ServerHelper.h"
#include "LC_ServerWarManager.h"
#include "LC_ServerGuildManager.h"
#include "LC_ServerMapLogic_Place.h"
#include "LC_ServerMapLogic_BatchPlay.h"
#include "LC_RankManager.h"
#include "oss_define.h"
#include "str2val.h"
#include "TS_TranManager.h"
#include "LC_CommonStatAsset.h"
#include "LC_ServerMapLogic_WarPlay.h"
#include "LC_ServerMapLogic_SoloDulePlay.h"
#include "LC_GameStory_Manager.h"
#include <boost/algorithm/string/replace.hpp>
#include "LC_GuildRaidDataManager.h"
#include "UT_Timer.h"
#include "LC_ActivationCodeAsset.h"
#include "UT_ActivationCode.h"
#include "LC_ShotActivity.h"
#include "LC_ShortcutBar.h"
#include "LC_RumorManager.h"
#include "LC_ServerMapLogic_NewBee.h"
#include "LC_TaskManager.h"
#include "LC_ServerShopAsset.h"
#include "LC_Arena_Manager.h"

#include "SC_ScriptAction.h"
#include "SC_ServerScriptInterface.h"
#include "SC_ServerSkillInterface.h"
#include "SC_ServerDebugInterface.h"
#include "SC_ServerScriptInterfaceHeader.h"

#include "LC_GameEventManager.h"

#include "msg.pb.h"
#include "redis.pb.h"
#include "MG_Common.h"
#include "MG_PB.h"
#include "MG_AssistProtocol.h"
#include "MG_CS_FriendProtocol.h"
#include "MG_ShortcutKeyProtocol.h"

#include "LC_TransferBase.h"
#include "LC_PackEntryFactory.h"
#include "LC_ItemFactoryBase.h"
#include "LC_ServerTools.h"
#include "publish_define.h"

#include "CF_VIP.h"
#include "CF_ForcesBattle.h"
#include "CF_GlobalPowerGuildReward.h"
#include "CF_GuildRaid.h"
#include "CF_WorldMapList.h"
#include "CF_LevelUp.h"
#include "CF_ActivationCode.h"
#include "CF_GuildRaid.h"
#include "CF_VIP.h"
#include "CF_TransferArea.h"
#include "CF_ForcesBattle.h"
#include "CF_FriendMsgBroad.h"
#include "ConfigManager.h"
#include "LC_HttpHandler.h"

using namespace CSVFile;
using namespace Protocol;
using namespace GameLogic;
using namespace StateMachine;

namespace Protocol
{
#define this_source_type SERVER_TYPE_CLIENT

//-------------------------------------------------------
ON_HANDLE(MGPT_CS_BUDDY_BUDDY_STAR_UPGRADE, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;

	MG_CS_BuddyStarUpgrade msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	pkPlayer->UpgradeBuddyInstanceStar(msg.m_nBuddyID);
	pkPlayer->SyncBackPackToClient(PACK_TYPE_BASIC);
}
//-------------------------------------------------------
ON_HANDLE(MGPT_CS_BIND_BUDDY_INSTANCE_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_CS_BindBuddyInstanceReq msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	//msg.m_nIndex
	pkPlayer->BindBuddyInstance( msg.m_nIndex, msg.m_nBuddyID );
	pkPlayer->SetAllPlayerBaseLevel(pkPlayer->GetCommanderLevel());
}

ON_HANDLE(MGPT_CS_UNBIND_BUDDY_INSTANCE_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_CS_UnBindBuddyInstanceReq msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	//msg.m_nIndex
	pkPlayer->UnBindBuddyInstance( msg.m_nIndex );
}

ON_HANDLE(MGPT_CS_SET_ACTIVATE_BUDDY_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_CS_SetActivateBuddyReq msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer->SetCurrentBuddy(msg.m_nIndex);
}

ON_HANDLE(MGPT_CS_BUY_BUDDY_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_CS_BuyBuddyReq msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	// Buy buddy
	pkPlayer->BuyBuddy(msg.m_nTemplateID);
	pkPlayer->SyncBackPackToClient(PACK_TYPE_BASIC);
}

ON_HANDLE(MGPT_CS_RELIVE_BUDDY_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_CS_ReliveBuddyReq msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (pkPlayer == NULL)
		return;

	// 司令官死亡的情况下，禁止请求单个复活
	if (pkPlayer->CheckOwnerDead())
		return;
	pkPlayer->ReliveBuddy(msg.m_nIndex);
}

ON_HANDLE(MGPT_CS_BUDDY_MANUAL_LEVEL_UP, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_CS_BuddyManualLevelUp msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	pkPlayer->TryBuddyLevelUp( msg.m_nBuddyID );
}

ON_HANDLE(MGPT_REQ_SETBUDDYACTIVATESCHEME, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_SetBuddyActivateScheme msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer->GetBuddySchemeMgr().SetBuddyActivateScheme_Client(pkPlayer, msg.m_nMapID, msg.m_nSchemeType, msg.m_vSchemeData);
}

ON_HANDLE(MGPT_REQ_SETSPECIALMAP_INITBUDDYACTIVATESCHEME, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_SetSpecialMapInitBuddyActivateScheme msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer->GetBuddySchemeMgr().SetSpecialMapInitBuddyActivateScheme_Client(pkPlayer, msg.m_nMapID, msg.m_nSchemeType);
}

ON_HANDLE(MGPT_CS_ADD_BUDDY_CHAIN_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_CS_AddBuddyChainReq msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	pkPlayer->BuddyChain_Add( msg.m_nIndex, msg.m_nBuddyID );
}

ON_HANDLE(MGPT_CS_DEL_BUDDY_CHAIN_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_CS_DelBuddyChainReq msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	pkPlayer->BuddyChain_Del( msg.m_nIndex );
}

ON_HANDLE(MGPT_CS_SWITCH_BUDDY_CHAIN_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_CS_SwitchBuddyChainReq msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	pkPlayer->BuddyChain_Switch( msg.m_nIndex, msg.m_nBuddyID );
}

}

