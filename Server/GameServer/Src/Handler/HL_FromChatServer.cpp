#include "MG_PACKET.h"
#include "HL_PacketHandler.h"
#include "NW_ServerSession.h"
#include "NW_Helper.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerPlayerManager.h"
#include "MG_CS_FriendProtocol.h"
#include "LC_RumorManager.h"

using namespace Protocol;
using namespace GameLogic;
using namespace Utility;

namespace Protocol
{
#define this_source_type SERVER_TYPE_IM

ON_HANDLE(MGPT_RLT_UPDATE_PLAYERSTATE, this_source_type, false)
{
	MG_RLT_UpdatePlayerState rltMsg;
	if (!rltMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(rkMsgHead.m_uiCharID);
	if (!pkPlayer)
	{
		return;
	}
	if (QUERY_FRIEND_PLAYERSTATE == rltMsg.m_cType)
	{
		MG_RLT_GetFriendStatus rltToClientMsg;
		int iCount = 0;
		for (int i = 0; i < rltMsg.m_usTargetSize; i++)
		{
			if (i >= MAX_FRIEND_NUM)
			{
				GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "arr too big-1! i:%d", i);
				return;
			}
			bool bOnline = (rltMsg.m_aOnlineInfo[i] > 0);
			if (bOnline)
			{
				if (iCount >= MAX_FRIEND_NUM)
				{
					GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "arr too big-2! iCount:%d", iCount);
					return;
				}
				int lServerID = rltMsg.m_aServerIDInfo[i];
				rltToClientMsg.m_lOnlinePlayerCharID[iCount] = rltMsg.m_aTargetCharIDInfo[i];
				rltToClientMsg.m_lOnlinePlayerServerID[iCount] = lServerID;
				iCount++;
			}
		}
		rltToClientMsg.m_uiSocietyType = ST_FRIEND;
		rltToClientMsg.m_bClearStatus = FALSE;
		rltToClientMsg.m_currentArraySize = iCount;
		pkPlayer->SendMsgToClient(MGPT_RLT_GET_FRIEND_STATUS, &rltToClientMsg);
	}
	else if (QUERY_GUILD_PLAYERSTATE == rltMsg.m_cType)
	{
		MG_RLT_OnlinePlayerState rltToClientMsg;
		int iCount = 0;
		for (int i = 0; i < rltMsg.m_usTargetSize; i++)
		{
			if (i >= MAX_FRIEND_NUM)
			{
				GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "arr too big-3! i:%d", i);
				return;
			}
			bool bOnline = (rltMsg.m_aOnlineInfo[i] > 0);
			if (bOnline)
			{
				if (iCount >= MAX_FRIEND_NUM)
				{
					GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "arr too big-4! count:%d", iCount);
					return;
				}
				int lServerID = rltMsg.m_aServerIDInfo[i];
				rltToClientMsg.m_uiOnlineCharID[iCount] = rltMsg.m_aTargetCharIDInfo[i];
				rltToClientMsg.m_uiOnlineCharServerID[iCount] = lServerID;
				iCount++;
			}
		}
		rltToClientMsg.m_uiCrtSize = iCount;
		pkPlayer->SendMsgToClient(MGPT_RLT_ONLINE_PLAYER_STATE, &rltToClientMsg);
	}
}

ON_HANDLE(MGPT_G2G_UPDATE_RESTARTREWARDPLAYERID, this_source_type, false)
{
	MG_G2G_Update_RestartRewardPlayerID msg;
	if (!msg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	UT_ServerHelper::UpdatePlayerRestartRewardState(msg.m_nCharID);
}

ON_HANDLE(MGPT_GC_RUMOR_MESSAGE, this_source_type, false)
{
// 	Protocol::MG_GC_RumorMessage rltMsg;
// 	if (!rltMsg.Decode(pMsgBody, usBodySize))
// 	{
// 		return;
// 	}
// 
// 	GameLogic::LC_RumorLog kRumorLog;
// 	kRumorLog.m_uiTimestamp = rltMsg.m_uiTimestamp;
// 	kRumorLog.m_sInfo = rltMsg.m_strInfo.c_str();
// 	kRumorLog.UpdateDirtyFlag();
// 
// 	GameLogic::LC_RumorManager::GetSingletonPtr()->AddLog(rltMsg.m_iType, kRumorLog, rltMsg.m_bGlobal);
}
}
