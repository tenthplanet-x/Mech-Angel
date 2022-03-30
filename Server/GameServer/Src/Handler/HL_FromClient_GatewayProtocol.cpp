#include "MG_ProtocolStruct.h"
#include "MG_Game2Login.h"

#include "HL_PacketHandler.h"


#include "LC_ServerPlayerEnterLeaveManager.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"

using namespace Protocol;

namespace Protocol
{
#define this_source_type SERVER_TYPE_GATEWAY
//-------------------------------------------------------
ON_HANDLE(MGPT_SS_USER_DISCONNECT, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	GfxWriteFmtLog(LOG_WARNING_FILELINE, LOG_SWITCH_WARNING, "user disconnect, gateway found a disconnected player", pkPlayer->GetUserName().c_str(), rkMsgHead.m_uiUserID, pkPlayer->GetInstance());
	LC_ServerPlayerEnterLeaveManager* enterLeaveManager = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
	LC_ServerPlayer* pOwner = pkPlayer->GetOwnerPlayer();
	if (pOwner == NULL)
		return;

	std::vector<LC_ServerPlayer*> vecBuddyPtrPlayer;
	pOwner->GetAllBuddyPtrPlayer( vecBuddyPtrPlayer);
	for (int i = 0; i < vecBuddyPtrPlayer.size(); i++)
	{
		LC_ServerPlayer* pLeaveBuddy = vecBuddyPtrPlayer[i];
		IF_A_NA_PLAYER(pLeaveBuddy)
			continue;
		enterLeaveManager->UserLeaveServer(pLeaveBuddy, LMIOT_OFFLINE);
	}
}

//-------------------------------------------------------
ON_HANDLE(MGPT_SS_USER_RELOGIN_GAME, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_GL_REQ_UserKey kUserKey;
	if (!kUserKey.Decode(pMsgBody, usBodySize))
	{
		return;
	}
	if (kUserKey.m_userKey != pkPlayer->getUserUUID())
	{
		return ;
	}
	GfxWriteFmtLog(LOG_WARNING_FILELINE, LOG_SWITCH_WARNING, "user relogin, gateway found a relogin player", pkPlayer->GetUserName().c_str(), rkMsgHead.m_uiUserID, pkPlayer->GetInstance(), kUserKey.m_userKey, pkPlayer->getUserUUID());
	LC_ServerPlayerEnterLeaveManager* enterLeaveManager = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
	LC_ServerPlayer* pOwnerPlayer = pkPlayer->GetOwnerPlayer();
	if ( pOwnerPlayer == NULL )
	{
		// ÀÏÁ÷³Ì
		enterLeaveManager->UserLeaveServer(pkPlayer, LMIOT_OFFLINE);
	}
	else
	{
		std::vector<LC_ServerPlayer*> vecBuddyPtrPlayer;
		pOwnerPlayer->GetAllBuddyPtrPlayer( vecBuddyPtrPlayer);
		for (int i = 0; i < vecBuddyPtrPlayer.size(); i++)
		{
			LC_ServerPlayer* pLeaveBuddy = vecBuddyPtrPlayer[i];
			IF_A_NA_PLAYER(pLeaveBuddy)
				continue;
			enterLeaveManager->UserLeaveServer(pLeaveBuddy, LMIOT_OFFLINE);
		}
	}
}



}