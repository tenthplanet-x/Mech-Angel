#include "NW_ServerSession.h"
#include "HL_PacketHandler.h"
#include "MG_CS_Alliance.h"
#include "LC_ServerAllianceManager.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "MG_Game2Alliance.h"
#include "UT_ServerHelper.h"
#include "CF_TransferArea.h"
#include "LC_ServerWorldManager.h"
#include "LC_Arena_Manager.h"
#include "NW_Helper.h"
#include "LC_ServerPlayerEnterLeaveManager.h"
#include "GameServerApp.h"
#include "GameServerManager.h"

using namespace Protocol;
using namespace GameLogic;
using namespace CSVFile;

namespace Protocol
{
#define this_source_type SERVER_TYPE_CLIENT
//--------------------------------------------
ON_HANDLE(MGPT_ALLIANCE_SERVER_LIST_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_Req_Alliance_Server_List reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerAllianceManager* pAllianceManager = LC_ServerAllianceManager::GetSingletonPtr();
	if(pAllianceManager == NULL)
	{
		return;
	}

	if(reqMsg.m_nGroupID <= 0)
	{
		reqMsg.m_nGroupID = pAllianceManager->GetSelfGroupID();
	}

	MG_Resp_Alliance_Server_List respMsg;
	respMsg.m_nGroupID = reqMsg.m_nGroupID;
	pAllianceManager->FillAllianceGroupInfo(reqMsg.m_nGroupID, respMsg.m_ServerList);
	pkPlayer->SendMsgToClient(MGPT_ALLIANCE_SERVER_LIST_RESP, &respMsg);
}

ON_HANDLE(MGPT_ALLIANCE_ENTER_CROSS_MAP_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_Req_Alliance_Enter_CrossMap reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerAllianceManager* pAllianceManager = LC_ServerAllianceManager::GetSingletonPtr();
	if(pAllianceManager == NULL)
	{
		return;
	}

	MG_Resp_Alliance_Enter_CrossMap respMsg;
	respMsg.m_nServerID = reqMsg.m_nServerID;
	respMsg.m_nAreaID = reqMsg.m_nAreaID;
	respMsg.m_nResult = RE_SUCCESS;

	int32_t nSelfGroup = pAllianceManager->GetSelfGroupID();
	int32_t nTargetGroup = pAllianceManager->GetGroupIDByServerID(reqMsg.m_nServerID);
	if(nSelfGroup != nTargetGroup)
	{
		respMsg.m_nResult = RE_ALLIANCE_ENTER_CROSS_GROUP_SAME;
	}

	static GameServerManager* pkServerMgr = GetGameServerApp()->GetServerManager();
	if (!pkServerMgr->IsServerOpened())
	{
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "enter cross not allowed, server not open!");
		respMsg.m_nResult = RE_LOGIN_FAIL_WAIT_SERVER_OPEN;
	}

	static LC_Arena_Manager* pkArenaMgr = SERVER_GET_ARENA_MANAGER();
	if (pkArenaMgr->IsCharSignValid(pkPlayer->GetInstance(), GET_PROCESS_TIME))
	{
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "alliance enter cross failed! in arena sign!");
		respMsg.m_nResult = RE_ARENA_IN_SIGN;
	}

	if(respMsg.m_nResult == RE_SUCCESS)
	{
		//检查本服
		if(reqMsg.m_nServerID == UT_ServerHelper::GetServerId())
		{
			//请求的目的地是本服
			CF_TransferArea::DataEntry* pkAreaData = CF_TransferArea::GetSingletonPtr()->GetEntryPtr(reqMsg.m_nAreaID);
			if (pkAreaData != NULL)
			{
				respMsg.m_nResult = (SERVER_GET_WORLD_MANAGER())->RequestChangePlayerLocation(pkPlayer, LMIOT_TRANSFER, 0, pkAreaData->_lMapID, pkAreaData->_kPos);
			}
			if(respMsg.m_nResult == RE_SUCCESS)
			{
				LC_ServerPlayerEnterLeaveManager* enterLeaveManager = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
				if (enterLeaveManager != NULL)
				{
					enterLeaveManager->ClearFrontierAlliancePlayerInfo(GET_PLAYERID(pkPlayer->GetUserID()));
				}
			}
		}
		else
		{
			//非本服，请求跨服通知
			MG_Req_SS_Alliance_Enter_Cross ssReq;
			ssReq.m_nTargetServerID = reqMsg.m_nServerID;
			ssReq.m_nAreaID = reqMsg.m_nAreaID;
			ssReq.m_nCharID = pkPlayer->GetInstance();
			ssReq.m_nUserID = pkPlayer->GetUserID();
			ssReq.m_nFromServerID = rkMsgHead.m_iDstID;
			SendMsgToSociety(MGPT_SS_FALLIANCE_ENTER_CROSS_REQ, &ssReq, pkPlayer->GetUserID(), pkPlayer->GetInstance());
		}
	}
	else
	{
		pkPlayer->SendMsgToClient(MGPT_ALLIANCE_ENTER_CROSS_MAP_RESP, &respMsg);
	}
}

ON_HANDLE(MGPT_FALLIANCE_GUILD_WIN_INFO_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	SendMsgToSociety(MGPT_REQ_GUILD_WAR_WINNERINFO, NULL, pkPlayer->GetUserID(), pkPlayer->GetInstance());
}

ON_HANDLE(MGPT_FALLIANCE_BOSS_INFO_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "MGPT_FALLIANCE_BOSS_INFO_REQ r1!");
		return;
	}

	MG_Req_Alliance_Boss_Info reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MGPT_FALLIANCE_BOSS_INFO_REQ decode error!");
		return;
	}

	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "handle MGPT_FALLIANCE_BOSS_INFO_REQ request!");
	SendMsgToSociety(MGPT_SS_FALLIANCE_BOSS_RFINFO_REQ, &reqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());
}

ON_HANDLE(MGPT_ALLIANCE_UNIT_INFO_REQ, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	LC_ServerAllianceManager* pAllianceManager = LC_ServerAllianceManager::GetSingletonPtr();
	if(pAllianceManager == NULL)
	{
		return;
	}

	MG_Resp_Alliance_Unit_Info respMsg;
	pAllianceManager->FillAllianceUnitInfo(respMsg);
	pkPlayer->SendMsgToClient(MGPT_ALLIANCE_UNIT_INFO_RSP, &respMsg);
}

}
