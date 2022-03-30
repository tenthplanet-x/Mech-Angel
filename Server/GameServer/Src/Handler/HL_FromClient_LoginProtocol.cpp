#include "MG_ProtocolStruct.h"
#include "MG_Game2Login.h"
#include "MG_Crypt_XTEA.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerPlayerEnterLeaveManager.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerPlayerQueueManager.h"
#include "NW_ServerSession.h"
#include "LC_ServerLogicManager.h"
#include "GameServerApp.h"
#include "HL_PacketHandler.h"
#include "LC_ServerGuildManager.h"
#include "LC_RankManager.h"
#include "LC_ServerPlayerGroupManager.h"
#include "LC_ServerWarManager.h"
#include "LC_ServerPlayerGroup.h"
#include "MG_LoginProtocol.h"
#include "LC_StoryLogic_Manager.h"
#include "LC_ServerWorldManager.h"
#include "LC_Arena_Manager.h"
#include "NewUserEvent.h"
#include "oss_define.h"
#include "oss_internal_define.h"

using namespace Protocol;
using namespace GameLogic;

namespace Protocol
{
#define this_source_type SERVER_TYPE_CLIENT

//-------------------------------------------------------
ON_HANDLE(MGPT_GC_REQ_LOGIN, this_source_type, false)
{
	MG_GC_REQ_Login kReqMsg;
	if (!kReqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	LC_ServerPlayerEnterLeaveManager* enterLeaveManager = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
	enterLeaveManager->UserEnterServer(rkMsgHead.m_uiUserID, kReqMsg.m_iCharID, pkSession);
	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"client req login","NODE: " , UT_ServerHelper::GetServerId()," userid  " ,rkMsgHead.m_uiUserID," charId:", rkMsgHead.m_uiCharID);
	GetGameServerApp()->GetDailyDataReport().OnLogin(kReqMsg.m_iCharID);	
}
//------------------------------------------------------------------------------
ON_HANDLE(MGPT_GC_REQ_LEAVE_GAME, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	LC_ServerPlayer* pOwner = pkPlayer->GetOwnerPlayer();
	if (pOwner == NULL)
		return;
	MG_REQ_LeaveGame reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_LeaveGame rltMsg;
	pkPlayer->SendMsgToClient(MGPT_GC_RLT_LEAVE_GAME, &rltMsg);
	LC_ServerPlayerEnterLeaveManager* enterLeaveManager = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
	std::vector<LC_ServerPlayer*> vecBuddyPtrPlayer;
	pOwner->GetAllBuddyPtrPlayer( vecBuddyPtrPlayer);
	for (int i = 0; i < vecBuddyPtrPlayer.size(); i++)
	{
		LC_ServerPlayer* pLeaveBuddy = vecBuddyPtrPlayer[i];
		IF_A_NA_PLAYER(pLeaveBuddy)
				continue;
		enterLeaveManager->UserLeaveServer(pLeaveBuddy, LMIOT_LEAVE);
	}
}
//------------------------------------------------------------------------------
ON_HANDLE(MGPT_GC_REQ_RESET_GAME, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	LC_ServerPlayer* pOwner = pkPlayer->GetOwnerPlayer();
	if (pOwner == NULL)
		return;
	MG_REQ_ResetGame reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_ResetGame rltMsg;
	pOwner->SendMsgToClient(MGPT_GC_RLT_RESET_GAME, &rltMsg);
	LC_ServerPlayerEnterLeaveManager* enterLeaveManager = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
	std::vector<LC_ServerPlayer*> vecBuddyPtrPlayer;
	pOwner->GetAllBuddyPtrPlayer( vecBuddyPtrPlayer);
	for (int i = 0; i < vecBuddyPtrPlayer.size(); i++)
	{
		LC_ServerPlayer* pLeaveBuddy = vecBuddyPtrPlayer[i];
		IF_A_NA_PLAYER(pLeaveBuddy)
			continue;
		enterLeaveManager->UserLeaveServer(pLeaveBuddy, LMIOT_RESET);
	}
}
//-------------------------------------------------------
ON_HANDLE(MGPT_SYNC_HEART_BEAT, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	pkPlayer->HeartBeat();
}
//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_IN_GAME, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	IF_A_NA_PLAYER(pkPlayer){return;}

	if (LC_ServerPlayer::SERVER_STATE_INWORLD == pkPlayer->GetServerViewState())
	{
	}
	else 
	{
		if (NULL == pkPlayer->GetMap())
		{
			bool ret = (SERVER_GET_WORLD_MANAGER())->ClearPlayerIndex(pkPlayer->GetID());
			GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "player enter with null map fatal! ", pkPlayer->GetOwnerCharName().c_str(), pkPlayer->GetSpawnMapID(), pkPlayer->GetID(), pkPlayer->GetUserID(), pkPlayer->GetInstance(), ret);
			return;
		}
		pkPlayer->SetServerViewState(LC_ServerPlayer::SERVER_STATE_INWORLD);
		pkPlayer->ClearDirtyFlag();
		pkPlayer->SyncAllInfo();
		//发送公会等信息
		LC_ServerGuildManager* pkGuildMgr = LC_ServerGuildManager::GetSingletonPtr();
		pkGuildMgr->CharEnter(pkPlayer);
		LC_WarManager* manager = (LC_WarManager*)SERVER_GET_GUILDWAR_MANAGER();	
		manager->CharEnter(pkPlayer);
		LC_ServerPlayerGroupManager* pkPlayerGroupManager = (LC_ServerPlayerGroupManager*)SERVER_GET_PLAYER_GROUP_MANAGER();
		if (pkPlayerGroupManager)
		{
			pkPlayerGroupManager->UpdatePlayerState(pkPlayer->GetID(), PLAYER_ON_LINE);
		}

		//玩家登陆时将GameStory的内容和野外刷怪、野外刷boss等活动通知
		GameStory::LC_StoryLogic_Manager* pkGameStoryManager = SERVER_GET_STORYLOGIC_MANAGER;
		pkGameStoryManager->PostEnterGame(pkPlayer);

		LC_RankManager* rank_mgr = SERVER_GET_RANK_MANAGER();
		if (rank_mgr)
		{
			rank_mgr->PostEnterGame(pkPlayer);
		}

		static LC_ServerPlayerManager* pkSvrPlayerManager = SERVER_GET_PLAYER_MANAGER();
		if(pkSvrPlayerManager)
		{
			pkSvrPlayerManager->PostPlayerOnline(pkPlayer);
		}

		static LC_Arena_Manager* pkArenaMgr = SERVER_GET_ARENA_MANAGER();
		if (pkArenaMgr)
		{
			pkArenaMgr->CharEnter(pkPlayer);
		}
		GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "player enter game", pkPlayer->GetOwnerCharName().c_str(), "map", pkPlayer->GetMapResID(), pkPlayer->GetID(), pkPlayer->GetUserID(), pkPlayer->GetInstance());
	}
	pkPlayer->SetEnterServerMode(ENTER_SERVER_MODE::eEnterServerMode_Normal);
	//正式进入游戏
	oss_internal_log_enter_state(pkPlayer);
}
//-------------------------------------------------------
ON_HANDLE(MGPT_GC_REQ_JUMPER, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_REQ_Jumper reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	MG_RLT_Jumper rltMsg;
	ResultType iChangeResult = RE_CHANGE_SERVER_SUCCESS;
	MoneyType llNeedCash	 = LC_Helper::GetChangeServerCash(pkPlayer->GetLevel());

	if (!IS_VALID_CHAR_MONEY(llNeedCash))
	{
		return;
	}

	if (iChangeResult == RE_CHANGE_SERVER_SUCCESS)
	{
		if (reqMsg.m_iServerID == GetGameServerApp()->GetServerInfo().m_iServerID)
		{
			iChangeResult = RE_CHANGE_SERVER_FAIL_SAME_SERVER;
		}
	}

	if (iChangeResult == RE_CHANGE_SERVER_SUCCESS)
	{
		int lChangeServerCDTime = pkPlayer->GetChangeServerCDTime();
		if (lChangeServerCDTime > 0)
		{
			iChangeResult = RE_CHANGE_SERVER_FAIL_NEED_CD;
			rltMsg.m_uiCDTime = lChangeServerCDTime;
		}
	}

	if (iChangeResult == RE_CHANGE_SERVER_SUCCESS)
	{
		if (!pkPlayer->IsCashEnough(llNeedCash, CT_UNBIND_CASH))
		{
			iChangeResult = RE_CHANGE_SERVER_FAIL_NEED_CASH;
		}
	}

	rltMsg.m_iResult = iChangeResult;
	rltMsg.m_uiCharID = rkMsgHead.m_uiCharID;
	pkPlayer->SendMsgToClient(MGPT_GC_RLT_JUMPER, &rltMsg);

	if (iChangeResult == RE_CHANGE_SERVER_SUCCESS)
	{
		pkPlayer->SetChangeServerCDTime(5 * 60);
		pkPlayer->ReduceCash(llNeedCash, CT_UNBIND_CASH, SYS_FUNC_TP_TRANSFER, reqMsg.m_iServerID);

		LC_ServerPlayerEnterLeaveManager* enterLeaveManager = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
		if(enterLeaveManager)
		{
			enterLeaveManager->UserJumpServer(pkPlayer, reqMsg.m_iServerID, LMIOT_JUMP);
		}
	}
}


}
