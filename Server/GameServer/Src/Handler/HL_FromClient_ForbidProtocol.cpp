#include "NW_ServerSession.h"
#include "HL_PacketHandler.h"
#include "MG_Forbid.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "GameServerApp.h"
#include "LC_SequenceManager.h"
#include "MG_Common.h"

using namespace Protocol;
using namespace GameLogic;

namespace Protocol
{
#define this_source_type SERVER_TYPE_CLIENT
//------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_ADDFORBIDPLAYER, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_AddForbidPlayer msg;
	if (msg.Decode(pMsgBody, usBodySize))
	{
		pkPlayer = pkPlayer->GetOwnerPlayer();
		if (NULL == pkPlayer)
		{
			return;
		}

		/*
		object_id_type lPlayerID = msg.m_lPlayerID;
		if (lPlayerID != pkPlayer->GetID())
		{
			return;
		}
		*/
		ResultType iResult = pkPlayer->AddForbidPlayer(msg.m_kData.lCharID, msg.m_kData.strCharName.c_str());
		if (iResult != RE_FORBID_ADD_PLAYER_SUCCESS)
		{
			MG_Message errorMsg;
			errorMsg.m_iMessageID = iResult;
			pkPlayer->SendMsgToClient(MGPT_MESSAGE, &errorMsg);
		}
	}
}
//------------------------------------------------------------------------------
ON_HANDLE(MGPT_REQ_REMOVEFORBIDPLAYER, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
	{
		return;
	}
	MG_REQ_RemoveForbidPlayer reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (NULL == pkPlayer)
	{
		return;
	}

	/*
	object_id_type lPlayerID = reqMsg.m_lPlayerID;
	if (lPlayerID != pkPlayer->GetID())
	{
		return;
	}
	*/

	ResultType iResult = pkPlayer->RemoveForbidPlayer(reqMsg.m_lTargetCharID);
	if (iResult != RE_FORBID_REMOVE_PLAYER_SUCCESS)
	{
		MG_Message errorMsg;
		errorMsg.m_iMessageID = iResult;
		pkPlayer->SendMsgToClient(MGPT_MESSAGE, &errorMsg);
	}
}

}
