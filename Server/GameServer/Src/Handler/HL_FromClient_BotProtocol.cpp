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
#include "MG_BotProtocol.h"

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
ON_HANDLE(MGPT_CS_BOT_ACTIVATE_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;

	MG_CS_BotActivateReq msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	pkPlayer->ActivateBot (msg.m_nResId);
}

ON_HANDLE(MGPT_CS_BOT_UPGRADE_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;

	MG_CS_BotUpgrade msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	pkPlayer->UpgradeBot(msg.m_nResId);
}

ON_HANDLE(MGPT_CS_BOT_REWARD_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;

	MG_CS_BotRewardReq msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	//pkPlayer->ActivateBot(msg.m_nResId);
	pkPlayer->TryFetchBotBonus();
}

}

