#include "NW_ServerSession.h"
#include "HL_PacketHandler.h"
#include "MG_PlaceHolderProtocol.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "GameServerApp.h"
#include "LC_SequenceManager.h"
#include "LC_ServerPlaceHolderManager.h"
#include "LC_ServerPlaceHolder.h"

using namespace Protocol;
using namespace GameLogic;

namespace Protocol
{
#define this_source_type SERVER_TYPE_CLIENT

//-------------------------------------------------------
ON_HANDLE(MGPT_REQ_CHEST_PICKUP, this_source_type, false)
{
	LC_ServerPlayer* pkPlayer = dynamic_cast<LC_ServerPlayer*>(pkTarget);
	if (NULL == pkPlayer)
		return;


	MG_REQ_PickUpPlaceHolder reqMsg;
	if (!reqMsg.Decode(pMsgBody, usBodySize))
	{
		return;
	}

	LC_ServerPlaceHolderManager* pkPlaceHolderManager = SERVER_GET_PLACEHOLDER_MANAGER();
	if (!pkPlaceHolderManager)
		return;

	MG_RLT_PickUpPlaceHolder kMsg;

	for (std::vector<uint32_t>::iterator i = reqMsg.m_lIDs.begin(); i != reqMsg.m_lIDs.end() ; ++i)
	{
		LC_ServerPlaceHolder* pkPlaceHolder = (LC_ServerPlaceHolder*)(pkPlaceHolderManager->FindPlaceHolder(*i));
		if (!pkPlaceHolder)
			return;
		pkPlaceHolder->RequestPickupItem(pkPlayer, kMsg, reqMsg.m_nCheckCD);
	}
	pkPlayer->SendMsgToClient(MGPT_RLT_CHEST_PICKUP, &kMsg);

}

}
