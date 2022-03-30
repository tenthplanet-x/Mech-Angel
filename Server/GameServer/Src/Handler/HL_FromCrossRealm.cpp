#include "GameServerApp.h"
#include "HL_PacketHandler.h"
#include "NW_ServerSession.h"
#include "NW_Helper.h"

#include "LC_ServerPlayer.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerWorldManager.h"

#include "TS_TranManager.h"
#include "TS_TranType.h"

#include "TS_TranAuctionSellTran.h"
#include "TS_TranAuctionBuyTran.h"

#include "MG_CS_Auction.h"
#include "MG_CS_CurrencyTradeProtocol.h"
#include "MG_TeamPlatformProtocol.h"
#include "MG_ArenaProtocol.h"
#include "MG_CS_GuildHome.h"

#include "LC_ServerPlayerGroupManager.h"
#include "LC_ServerPlayerGroup.h"
#include "LC_ServerPlayerEnterLeaveManager.h"

#include "LC_Dule_Manager.h"
#include "LC_RankManager.h"
#include "LC_PackEntryFactory.h"
#include "MG_GroupProtocol.h"
#include "MG_Common.h"
#include "CF_CrossRealmMap.h"
#include "LC_ServerGuildManager.h"

using namespace Protocol;
using namespace GameLogic;
using namespace Utility;

namespace Protocol
{
#define this_source_type SERVER_TYPE_SOCIETY

//----------------------------------------------------------------------------
ON_HANDLE(MGPT_SS_CROSSREALM_MAPLIST_RESP, this_source_type, false)
{
	LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (NULL == pkServerPlayerManager)
	{
		return;
	}

	LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)(pkServerPlayerManager->FindPlayer(rkMsgHead.m_uiCharID));
	if (pkServerPlayer == NULL)
	{
		return;
	}
	MG_GG_RLT_MapList rltMsg;

	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	pkServerPlayer->SendMsgToClient(MGPT_CROSSREALM_MAPLIST_RESP, &rltMsg);
}

ON_HANDLE(MGPT_SS_CROSSREALM_TEAMLIST_RESP, this_source_type, false)
{
	LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (NULL == pkServerPlayerManager)
	{
		return;
	}

	LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)(pkServerPlayerManager->FindPlayer(rkMsgHead.m_uiCharID));
	if (pkServerPlayer == NULL)
	{
		return;
	}
	MG_GG_RLT_TeamList rltMsg;

	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	pkServerPlayer->SendMsgToClient(MGPT_CROSSREALM_TEAMLIST_RESP, &rltMsg);
}

ON_HANDLE(MGPT_SS_CROSSREALM_TEAMDETAIL_RESP, this_source_type, false)
{
	LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (NULL == pkServerPlayerManager)
	{
		return;
	}

	LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)(pkServerPlayerManager->FindPlayer(rkMsgHead.m_uiCharID));
	if (pkServerPlayer == NULL)
	{
		return;
	}
	MG_GG_RLT_TeamDetail rltMsg;

	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	pkServerPlayer->SendMsgToClient(MGPT_CROSSREALM_TEAMDETAIL_RESP, &rltMsg);
}

ON_HANDLE(MGPT_SS_CROSSREALM_ADD2TEAM_RESP, this_source_type, false)
{
	LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (NULL == pkServerPlayerManager)
	{
		return;
	}

	LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)(pkServerPlayerManager->FindPlayer(rkMsgHead.m_uiCharID));
	if (pkServerPlayer == NULL)
	{
		return;
	}
	MG_GG_RLT_Add2Team rltMsg;

	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	pkServerPlayer->SendMsgToClient(MGPT_CROSSREALM_ADD2TEAM_RESP, &rltMsg);
}

ON_HANDLE(MGPT_SS_CROSSREALM_CREATETEAM_RESP, this_source_type, false)
{
	LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (NULL == pkServerPlayerManager)
	{
		return;
	}

	LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)(pkServerPlayerManager->FindPlayer(rkMsgHead.m_uiCharID));
	if (pkServerPlayer == NULL)
	{
		return;
	}
	MG_GG_RLT_CreateTeam rltMsg;

	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	pkServerPlayer->SendMsgToClient(MGPT_CROSSREALM_CREATETEAM_RESP, &rltMsg);
}

ON_HANDLE(MGPT_SS_CROSSREALM_LEAVETEAM_RESP, this_source_type, false)
{
	LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (NULL == pkServerPlayerManager)
	{
		return;
	}

	LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)(pkServerPlayerManager->FindPlayer(rkMsgHead.m_uiCharID));
	if (pkServerPlayer == NULL)
	{
		return;
	}
	MG_GG_RESP_LeaveTeam rltMsg;

	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	pkServerPlayer->SendMsgToClient(MGPT_CROSSREALM_LEAVETEAM_RESP, &rltMsg);
}

ON_HANDLE(MGPT_SS_CROSSREALM_TEAMINFOEDIT_RESP, this_source_type, false)
{
	LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (NULL == pkServerPlayerManager)
	{
		return;
	}

	LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)(pkServerPlayerManager->FindPlayer(rkMsgHead.m_uiCharID));
	if (pkServerPlayer == NULL)
	{
		return;
	}
	MG_GG_RESP_TeamInfoEdit rltMsg;

	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	pkServerPlayer->SendMsgToClient(MGPT_CROSSREALM_TEAMINFOEDIT_RESP, &rltMsg);
}

ON_HANDLE(MGPT_SS_CROSSREALM_KICKMEMBER_RESP, this_source_type, false)
{
	LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (NULL == pkServerPlayerManager)
	{
		return;
	}

	LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)(pkServerPlayerManager->FindPlayer(rkMsgHead.m_uiCharID));
	if (pkServerPlayer == NULL)
	{
		return;
	}
	MG_GG_RESP_Kick_Member rltMsg;

	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	pkServerPlayer->SendMsgToClient(MGPT_CROSSREALM_KICKMEMBER_RESP, &rltMsg);
}

ON_HANDLE(MGPT_SS_CROSSREALM_READYMEMBER_RESP, this_source_type, false)
{
	LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (NULL == pkServerPlayerManager)
	{
		return;
	}

	LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)(pkServerPlayerManager->FindPlayer(rkMsgHead.m_uiCharID));
	if (pkServerPlayer == NULL)
	{
		return;
	}
	MG_GG_RESP_Ready_Member rltMsg;

	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	pkServerPlayer->SendMsgToClient(MGPT_CROSSREALM_READYMEMBER_RESP, &rltMsg);
}

ON_HANDLE(MGPT_SS_CROSSREALM_AUTOADD_RESP, this_source_type, false)
{
	LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (NULL == pkServerPlayerManager)
	{
		return;
	}

	LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)(pkServerPlayerManager->FindPlayer(rkMsgHead.m_uiCharID));
	if (pkServerPlayer == NULL)
	{
		return;
	}
	MG_GG_RESP_AutoAdd_Team rltMsg;

	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	pkServerPlayer->SendMsgToClient(MGPT_CROSSREALM_AUTOADD_RESP, &rltMsg);
}

ON_HANDLE(MGPT_SS_CROSSREALM_ADD2TEAM_NOTIFY, this_source_type, false)
{
	LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (NULL == pkServerPlayerManager)
	{
		return;
	}

	LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)(pkServerPlayerManager->FindPlayer(rkMsgHead.m_uiCharID));
	if (pkServerPlayer == NULL)
	{
		return;
	}
	MG_NOTIFY_CrossRealm_Add2Team rltMsg;

	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	pkServerPlayer->SendMsgToClient(MGPT_CROSSREALM_ADD2TEAM_NOTIFY, &rltMsg);
}

ON_HANDLE(MGPT_SS_CROSSREALM_PLAYINFOCHANGE_NOTIFY, this_source_type, false)
{
	LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (NULL == pkServerPlayerManager)
	{
		return;
	}

	LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)(pkServerPlayerManager->FindPlayer(rkMsgHead.m_uiCharID));
	if (pkServerPlayer == NULL)
	{
		return;
	}
	MG_GG_NOTIFY_PlayInfoChange rltMsg;

	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	pkServerPlayer->SendMsgToClient(MGPT_CROSSREALM_PLAYINFOCHANGE_NOTIFY, &rltMsg);
}

ON_HANDLE(MGPT_SS_CROSSREALM_TEAMINFOCHANGE_NOTIFY, this_source_type, false)
{
	LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (NULL == pkServerPlayerManager)
	{
		return;
	}

	LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)(pkServerPlayerManager->FindPlayer(rkMsgHead.m_uiCharID));
	if (pkServerPlayer == NULL)
	{
		return;
	}
	MG_GG_NOTIFY_TeamInfoChange rltMsg;

	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	pkServerPlayer->SendMsgToClient(MGPT_CROSSREALM_TEAMINFOCHANGE_NOTIFY, &rltMsg);
}

ON_HANDLE(MGPT_SS_CROSSREALM_LEAVETEAM_NOTIFY, this_source_type, false)
{
	LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (NULL == pkServerPlayerManager)
	{
		return;
	}

	LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)(pkServerPlayerManager->FindPlayer(rkMsgHead.m_uiCharID));
	if (pkServerPlayer == NULL)
	{
		return;
	}
	MG_GG_Notify_LeaveTeam rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	pkServerPlayer->SendMsgToClient(MGPT_CROSSREALM_LEAVETEAM_NOTIFY, &rltMsg);
}

ON_HANDLE(MGPT_SS_CROSSREALM_BEKICK_NOTIFY, this_source_type, false)
{
	LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (NULL == pkServerPlayerManager)
	{
		return;
	}

	LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)(pkServerPlayerManager->FindPlayer(rkMsgHead.m_uiCharID));
	if (pkServerPlayer == NULL)
	{
		return;
	}
	MG_GG_Notify_BeKick rltMsg;

	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	pkServerPlayer->SendMsgToClient(MGPT_CROSSREALM_BEKICK_NOTIFY, &rltMsg);
}

ON_HANDLE(MGPT_SS_CROSSREALM_ENTER_RESP, this_source_type, false)
{
	LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (NULL == pkServerPlayerManager)
	{
		return;
	}

	LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)(pkServerPlayerManager->FindPlayer(rkMsgHead.m_uiCharID));
	if (pkServerPlayer == NULL)
	{
		return;
	}
	MG_GG_RESP_CrossRealm_Enter rltMsg;

	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	pkServerPlayer->SendMsgToClient(MGPT_CROSSREALM_ENTER_RESP, &rltMsg);
}

ON_HANDLE(MGPT_SS_CROSSREALM_ENTER_NOTIFY, this_source_type, false)
{
	LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (NULL == pkServerPlayerManager)
	{
		return;
	}

	LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)(pkServerPlayerManager->FindPlayer(rkMsgHead.m_uiCharID));
	if (pkServerPlayer == NULL)
	{
		return;
	}
	MG_GG_Notify_Team_ReadyEnter rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	pkServerPlayer->SendMsgToClient(MGPT_CROSSREALM_ENTER_NOTIFY, &rltMsg);
}

ON_HANDLE(MGPT_SS_CROSSREALM_SERVER_USERENTER, this_source_type, false)
{
	MG_GG_Notify_Server_UserCrossRealm_Team rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerPlayerEnterLeaveManager* enterLeaveManager = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
	if (enterLeaveManager == NULL)
	{
		return;
	}
	mem::vector<CrossRealmTeam_Player> allplayer;
	for (int i = 0; i < rltMsg.m_playerCount; ++i)
	{
		CrossRealmTeam_Player temp;
		temp.m_charID = rltMsg.m_CharIDs[i];
		temp.m_userID = rltMsg.m_UserIDs[i];
		temp.m_playerID = GET_PLAYERID(temp.m_userID);
		temp.m_SrcServerId = rltMsg.m_charServerIDs[i];
		temp.m_Enter = false;
		temp.m_Timestamp = 0;
		allplayer.push_back(temp);
	}
	enterLeaveManager->AddCrossRealmTeamPlayerInfo(allplayer, rltMsg.m_mapID, rltMsg.m_mapType, rltMsg.m_TeamId);
}

ON_HANDLE(MGPT_SS_CROSSREALM_USER_ENTER, this_source_type, false)
{
	MG_GG_Notify_EnterMap_Team rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (NULL == pkServerPlayerManager)
	{
		return;
	}

	LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)(pkServerPlayerManager->FindPlayer(rkMsgHead.m_uiCharID));
	if (pkServerPlayer == NULL)
	{
		return;
	}
	LC_ServerPlayerEnterLeaveManager* enterLeaveManager = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
	if (enterLeaveManager == NULL)
	{
		return;
	}
	enterLeaveManager->PlayerTryEnterCrossRealm(pkServerPlayer, rltMsg.m_TargetServerId);
}

ON_HANDLE(MGPT_SS_CROSSREALM_GAME_PLAYER_LEAVE, this_source_type, false)
{
	MG_GG_Notify_Game_CrossRealm_Leave_Team rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerPlayerEnterLeaveManager* enterLeaveManager = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
	if (enterLeaveManager == NULL)
	{
		return;
	}
	for (int i = 0; i < rltMsg.m_Size; ++i)
	{
		enterLeaveManager->DelCrossRealmPlayer(UT_ServerHelper::QueryPlayerID(rltMsg.m_CharID[i]), false);
	}
}

ON_HANDLE(MGPT_SS_CROSSREALM_FAST_NOTIFY, this_source_type, false)
{
	MG_GG_Notify_CrossRealmTeamFast rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	BroadcastMsgToClient(MGPT_CROSSREALM_FAST_NOTIFY, &rltMsg);
}

ON_HANDLE(MGPT_SS_CROSSREALM_PLAYER_TEAMINFO_RESP, this_source_type, false)
{
	MG_GG_RLT_PlayerTeamDetail rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)(pkServerPlayerManager->FindPlayer(rkMsgHead.m_uiCharID));
	if (pkServerPlayer == NULL)
	{
		return;
	}
	pkServerPlayer->SendMsgToClient(MGPT_CROSSREALM_PLAYER_TEAMINFO_RESP, &rltMsg);
}

ON_HANDLE(MGPT_CROSSREALM_TEAM_INFO_NOTIFY, this_source_type, false)
{
	/*
	MG_GG_NOTIFY_TeamInfoChange notifyMsg;
	if (!notifyMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	BroadcastMsgToClient(MGPT_CROSSREALM_TEAM_INFO_NOTIFY, &notifyMsg);

	LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (NULL == pkServerPlayerManager)
	{
		return;
	}

	LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)(pkServerPlayerManager->FindPlayer(notifyMsg.m_TeamInfo.m_MasterId));
	if (pkServerPlayer == NULL)
	{
		return;
	}

	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();

	LC_ServerGuildInfo* guild = pkGuildManager->GetPlayerGuild(pkServerPlayer->GetInstance());
	if(NULL != guild)
	{
		CF_CrossRealmMap::DataEntry *pkEntry = SafeGetCSVFileDataEntryPtr<CF_CrossRealmMap>(notifyMsg.m_MapTypeId);
		StringType strNameInfo("");
		strNameInfo = strNameInfo + "<color val = 0xfffc13>" + STDSTR2TPSTR(notifyMsg.m_TeamInfo.m_TeamName) + "</color>";
		strNameInfo = "<link val = N" + Int642String(notifyMsg.m_TeamInfo.m_MasterId) + "#" + STDSTR2TPSTR(notifyMsg.m_TeamInfo.m_TeamName) + "#SERVERID" + Int2String(notifyMsg.m_TeamInfo.m_ServerId) + ">" + strNameInfo + "</link>";
		StringType strMapName("");
		if(NULL != pkEntry)
		{
			strMapName = "<color val = 0xfffc13>【" + pkEntry->_kName + "】</color>";
		}
		StringType strScore("");
		//attr_value_type myScore = LC_Helper::GetTotalScore(pkServerPlayer);
		uint64_t myScore = pkServerPlayer->GetControllerCombatScore();
		if (notifyMsg.m_TeamInfo.m_TotalScore > 0)
		{
			if (myScore >= notifyMsg.m_TeamInfo.m_TotalScore)
			{
				strScore = "要求战斗力：<color val = 0xff00ff00>" + Int2String(notifyMsg.m_TeamInfo.m_TotalScore) + "</color>，";
			}
			else
			{
				strScore = "要求战斗力：<color val = 0xffDE1F00>" + Int2String(notifyMsg.m_TeamInfo.m_TotalScore) + "</color>，";
			}
		}
		StringType strJoin("");
		strJoin = "<color val = 0xff00ff00>立即加入</color>";
		strJoin = "<link val = R" + Int2String(notifyMsg.m_MapTypeId) + "#" + Int2String(notifyMsg.m_TeamInfo.m_TeamId) + "#>" + strJoin + "</link>";

		StringType notifystr = strNameInfo+GET_MSG(HL_FROMGUILDSERVER_CPP_007)+strMapName+GET_MSG(HL_FROMGUILDSERVER_CPP_008)+strScore+strJoin;

		MG_REQ_RequestChat reqMsg;
		reqMsg.m_lCharID = pkServerPlayer->GetInstance();
		reqMsg.m_iChatChannel = CCT_GUILD;
		reqMsg.m_iGuildID = guild->GetId();
		reqMsg.m_autoConsume = false;
		reqMsg.m_strChatContent = TPSTR2STDSTR(notifystr);

		LC_ChatChannelController* pkChatController = pkServerPlayer->GetChatChannelController();
		LC_ServerLogicManager* pkServerLogicManager = SERVER_GET_LOGIC_MANAGER();
		LC_ChatMessageManager* pkChatMessageManager = pkServerLogicManager->GetChatMessageManager();
		if (!pkChatMessageManager)
		{
			return;
		}

		int32_t timestamp = (int32_t)GET_CURRENT_TIMESTAMP_IN_SECONDS();
		bool bSilentGag = pkChatController->GetSilentGagTime() > timestamp ? true : false;
		pkChatMessageManager->TranslateChatMessage(pkServerPlayer, reqMsg, bSilentGag, reqMsg.m_autoConsume);
	}
	*/
}

ON_HANDLE(MGPT_CROSSREALM_TEAM_OPERATION_RESP, this_source_type, false)
{
	LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)(pkServerPlayerManager->FindPlayer(rkMsgHead.m_uiCharID));
	if (pkServerPlayer == NULL)
	{
		return;
	}

	MG_RESP_CrossRealmTeamOperation notifyMsg;
	if (!notifyMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	pkServerPlayer->SendMsgToClient(MGPT_CROSSREALM_TEAM_OPERATION_RESP, &notifyMsg);
}

}
