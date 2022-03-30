#include "CF_ChatMonitor.h"
#include "HL_PacketHandler.h"
#include "NW_ServerSession.h"
#include "MG_ChatProtocol.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "GameServerApp.h"
#include "LC_SequenceManager.h"
#include "MG_Common.h"
#include "oss_define.h"

using namespace Protocol;
using namespace GameLogic;
using namespace CSVFile;

namespace Protocol
{
#define this_source_type SERVER_TYPE_CLIENT
//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_CHAT_REQUEST, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	
	MG_REQ_RequestChat reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	if (rkMsgHead.m_uiCharID != reqMsg.m_lCharID)
	{
		return;
	}

	/*
	//如果是好友消息，判断Target是否在线
	if(CCT_PRIVATE == reqMsg.m_iChatChannel)
	{
		LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
		LC_ServerPlayer* pkTargetPlayer = (LC_ServerPlayer*)(pkServerPlayerManager->FindPlayer(reqMsg.m_lTargetCharID));
		if (!pkTargetPlayer)
		{
			MG_RLT_RequestChat rltMsg;
			rltMsg.m_lTargetCharID = reqMsg.m_lTargetCharID;
			rltMsg.m_iChatChannel = reqMsg.m_iChatChannel;
			rltMsg.m_iResult = RE_CHAT_FAIL_TARGET_OFFLINE;
			pkPlayer->SendMsgToClient(MGPT_RLT_CHAT_REQUEST, &rltMsg);
			return;
		}
	}
	*/

	if (!pkPlayer->IsInGame())
	{
		return;
	}
	LC_ChatChannelController* pkChatController = pkPlayer->GetChatChannelController();

	int32_t timestamp = (int32_t)GET_CURRENT_TIMESTAMP_IN_SECONDS();
	if (timestamp >= pkPlayer->GetGagTime())
	{
		pkPlayer->SetGagTime(0);
	}
	if (pkPlayer->GetGagTime() != 0)
	{
		MG_Message rltMsg;
		rltMsg.m_iMessageID = RE_SYSTEM_FORBID_CHAT;
		pkPlayer->SendMsgToClient(MGPT_MESSAGE, &rltMsg);
		return;
	}

	bool bResetMonitor = false, bStopProcessing = false;
	do {
		CF_ChatMonitor* pkCSVChatMonitor = CF_ChatMonitor::GetSingletonPtr();
		CF_ChatMonitor::DataEntry* pkEntry = pkCSVChatMonitor->GetEntryPtr(pkChatController->GetChatMonitorLevel());
		if (!pkEntry)
		{
			break;
		}
		bResetMonitor = pkEntry->_bResetLevel;
		bStopProcessing = pkEntry->_bStopProcessing;
	} while(0);

	if (timestamp >= pkChatController->GetSilentGagTime())
	{
		pkChatController->SetSilentGagTime(0);
		if (bResetMonitor)
		{
			pkChatController->ResetChatMonitorLevel();
		}
	}

	if ((bStopProcessing && pkChatController->GetSilentGagTime() > timestamp) ||
		!GetGameServerApp()->GetTextFilterChannel().running())
	{
		LC_ServerLogicManager* pkServerLogicManager = SERVER_GET_LOGIC_MANAGER();
		LC_ChatMessageManager* pkChatMessageManager =
			pkServerLogicManager->GetChatMessageManager();
		if (!pkChatMessageManager)
		{
			return;
		}

		bool bSilentGag = pkChatController->GetSilentGagTime() > timestamp ? true : false;
		MG_RLT_RequestChat rltMsg;
		rltMsg.m_lTargetCharID = reqMsg.m_lTargetCharID;
		rltMsg.m_iChatChannel = reqMsg.m_iChatChannel;
		rltMsg.m_iResult = pkChatMessageManager->TranslateChatMessage(pkPlayer, reqMsg, bSilentGag, reqMsg.m_autoConsume);
		if (reqMsg.m_iChatChannel == CCT_TRUMPET ||
			reqMsg.m_iChatChannel == CCT_SUPER_TRUMPET)
		{
			TranslateChatInfo& rInfo = pkChatMessageManager->GetTranslateChatInfo();
			rltMsg.m_iShowTime = rInfo.m_iShowTime;
		}

		pkPlayer->SendMsgToClient(MGPT_RLT_CHAT_REQUEST, &rltMsg);
	}
	else
	{
		GetGameServerApp()->GetTextFilterChannel().BounedPush(reqMsg);
	}
}
//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_UPDATE_CHANNEL, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_UPdateChatChannel reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	if (reqMsg.m_lCharID != pkPlayer->GetInstance())
	{
		return;
	}
	ResultType iResult;
	LC_ChatChannelController* pkChatChannelController = pkPlayer->GetChatChannelController();
	{
		for (int iChannelType = 0 ; iChannelType < MAX_SETTABLE_CHANNEL_NUM ; iChannelType++)
		{
			pkChatChannelController->SetChatChannelFlag(iChannelType, reqMsg.m_bChannel[iChannelType]);
		}
		iResult = RE_CHAT_UPDATE_CHANNEL_SUCCESS;
	}
	MG_RLT_UPdateChatChannel rltMsg;
	rltMsg.m_lCharID = reqMsg.m_lCharID;
	rltMsg.m_iResult   = iResult;
	pkPlayer->SendMsgToClient(MGPT_RLT_UPDATE_CHANNEL, &rltMsg);
}

}
