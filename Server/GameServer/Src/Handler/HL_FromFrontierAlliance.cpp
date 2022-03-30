#include "GameServerApp.h"
#include "MG_TYPES.h"
#include "HL_PacketHandler.h"

#include "NW_Helper.h"
#include "NW_ServerSession.h"
#include "LC_ServerAllianceManager.h"
#include "UT_ServerHelper.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerPlayerEnterLeaveManager.h"
#include "LC_ServerPlayerManager.h"
#include "MG_Game2Guild.h"

using namespace Protocol;
using namespace GameLogic;
using namespace Utility;
using namespace CSVFile;

namespace Protocol
{
#define this_source_type SERVER_TYPE_SOCIETY

ON_HANDLE(MGPT_SS_FALLIANCE_GROUPINFO_RESP, this_source_type, false)
{
	MG_Resp_Alliance_Group_Info respMsg;
	if (!respMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerAllianceManager::GetSingleton().SyncAllianceGroupInfo(respMsg);
}

ON_HANDLE(MGPT_SS_FALLIANCE_INFO_RESP, this_source_type, false)
{
	MG_Resp_Alliance_Frontier_Info respMsg;
	if (!respMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerAllianceManager::GetSingleton().SyncAllianceUnitInfo(respMsg.m_rAllianceUnit);
}

ON_HANDLE(MGPT_SS_FALLIANCE_ENTER_CROSS_REQ, this_source_type, false)			//请求跨服准备,其它服务器发送过来
{
	MG_Req_SS_Alliance_Enter_Cross reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_Resp_SS_Alliance_Enter_Cross respMsg;
	respMsg.m_nAreaID = reqMsg.m_nAreaID;
	respMsg.m_nCharID = reqMsg.m_nCharID;
	respMsg.m_nUserID = reqMsg.m_nUserID;
	respMsg.m_nTargetServerID = reqMsg.m_nTargetServerID;
	respMsg.m_nFromServerID = reqMsg.m_nFromServerID;

	static GameServerManager* pkServerMgr = GetGameServerApp()->GetServerManager();
	if (!pkServerMgr->IsServerOpened())
	{
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "enter cross req not allowed, server not open!");
		respMsg.m_nResult = RE_LOGIN_FAIL_WAIT_SERVER_OPEN;
	}
	else if(reqMsg.m_nTargetServerID != UT_ServerHelper::GetServerId())
	{
		respMsg.m_nResult = RE_ALLIANCE_ENTER_CROSS_GAMEID;
	}
	else
	{
		respMsg.m_nResult = LC_ServerAllianceManager::GetSingleton().PreEnter(reqMsg.m_nCharID, reqMsg.m_nUserID, reqMsg.m_nAreaID, reqMsg.m_nFromServerID);
	}
	SendMsgToSociety(MGPT_SS_FALLIANCE_ENTER_CROSS_RESP, &respMsg);
}

ON_HANDLE(MGPT_SS_FALLIANCE_ENTER_CROSS_RESP, this_source_type, false)			//跨服准备返回，其它服务器发送过来的结果
{
	MG_Resp_SS_Alliance_Enter_Cross respMsg;
	if (!respMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(SERVER_GET_PLAYER_MANAGER())->FindPlayer(respMsg.m_nCharID);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_Resp_Alliance_Enter_CrossMap respClient;
	respClient.m_nAreaID = respMsg.m_nAreaID;
	respClient.m_nServerID = respMsg.m_nTargetServerID;
	respClient.m_nResult = respMsg.m_nResult;
	pkPlayer->SendMsgToClient(MGPT_ALLIANCE_ENTER_CROSS_MAP_RESP, &respClient);

	if(respMsg.m_nResult == RE_SUCCESS)
	{
		LC_ServerPlayerEnterLeaveManager* enterLeaveManager = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
		if (enterLeaveManager != NULL)
		{
			enterLeaveManager->PlayerTryEnterAlliance(pkPlayer, respMsg.m_nTargetServerID, respMsg.m_nAreaID);
		}
	}
}

ON_HANDLE(MGPT_SS_FALLIANCE_ENTER_CROSS_SUCC, this_source_type, false)		//跨服汇报
{
	MG_Resp_SS_Alliance_Enter_Cross respMsg;
	if (!respMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	if(respMsg.m_nResult == RE_SUCCESS)
	{
		//边境跨服成功
		//添加Holder
		LC_ServerPlayerEnterLeaveManager* enterLeaveManager = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
		if (enterLeaveManager != NULL)
		{
			enterLeaveManager->AddPlayerHolder(respMsg.m_nCharID, respMsg.m_nUserID, 600, HoldDataType_FrontierAlliance, respMsg.m_nTargetServerID, respMsg.m_nAreaID);
		}
	}
}

ON_HANDLE(MGPT_SS_FALLIANCE_ENTER_CROSS_SYNC, this_source_type, false)		//定时跨服同步
{
	LC_ServerPlayerEnterLeaveManager* enterLeaveManager = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
	if (enterLeaveManager != NULL)
	{
		enterLeaveManager->UpdatePlayerHolder(rkMsgHead.m_uiCharID, rkMsgHead.m_uiUserID, 300, HoldDataType_FrontierAlliance);
	}
}

ON_HANDLE(MGPT_RLT_GUILD_WAR_WINNERINFO, this_source_type, false)
{
	MG_SS_GUILD_WAR_WIN_INFO respMsg;
	if (!respMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(SERVER_GET_PLAYER_MANAGER())->FindPlayer(rkMsgHead.m_uiCharID);
	if (NULL == pkPlayer)
	{
		return;
	}
	pkPlayer->SendMsgToClient(MGPT_FALLIANCE_GUILD_WIN_INFO_RSP, &respMsg);
}

ON_HANDLE(MGPT_SS_FALLIANCE_BOSS_RFINFO_RESP, this_source_type, false)
{
	MG_Resp_Alliance_Boss_Info respMsg;
	if (!respMsg.Decode(pMsgBody, usBodySize))
	{
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MGPT_SS_FALLIANCE_BOSS_RFINFO_RESP decode error!");
		return;
	}

	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(SERVER_GET_PLAYER_MANAGER())->FindPlayer(rkMsgHead.m_uiCharID);
	if (NULL == pkPlayer)
	{
		GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "MGPT_SS_FALLIANCE_BOSS_RFINFO_RESP r1!",  usBodySize);
		return;
	}

	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "handle MGPT_SS_FALLIANCE_BOSS_RFINFO_RESP request!",  usBodySize);
	pkPlayer->SendMsgToClient(MGPT_FALLIANCE_BOSS_INFO_RSP, &respMsg);
}

ON_HANDLE(MGPT_SS_FALLIANCE_BACK_SERVER_REQ, this_source_type, false)
{
	MG_Req_SS_Alliance_Back_Server reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_Resp_SS_Alliance_Back_Server respMsg;
	respMsg.m_nCharID = reqMsg.m_nCharID;
	respMsg.m_nTargetServerID = reqMsg.m_nTargetServerID;
	respMsg.m_nFromServerID = reqMsg.m_nFromServerID;

	LC_ServerPlayerEnterLeaveManager* enterLeaveManager = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
	if(UT_ServerHelper::GetServerId() != reqMsg.m_nFromServerID)
		respMsg.m_nResult = enterLeaveManager->DealCrossChangeLocationRequst(reqMsg.m_nCharID, reqMsg.m_nUserID, reqMsg);
	else
		respMsg.m_nResult = RE_SUCCESS;
	SendMsgToSociety(MGPT_SS_FALLIANCE_BACK_SERVER_RESP, &respMsg);
}

ON_HANDLE(MGPT_SS_FALLIANCE_BACK_SERVER_RESP, this_source_type, false)
{
	MG_Resp_SS_Alliance_Back_Server respMsg;
	if (!respMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(SERVER_GET_PLAYER_MANAGER())->FindPlayer(respMsg.m_nCharID);
	if (NULL == pkPlayer)
	{
		return;
	}

	if(respMsg.m_nResult != RE_SUCCESS)
	{
		MG_Message errorMsg;
		errorMsg.m_iMessageID = respMsg.m_nResult;
		pkPlayer->SendMsgToClient(MGPT_MESSAGE, &errorMsg);
	}
	else
	{
		LC_ServerPlayerEnterLeaveManager* enterLeaveManager = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
		if(enterLeaveManager != NULL)
		{
			enterLeaveManager->StartCrossChangeLocation(pkPlayer, respMsg.m_nTargetServerID);
		}
	}
}

}

