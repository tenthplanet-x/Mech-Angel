#include "NW_Helper.h"
#include "GameServerApp.h"
#include "NW_ServerSession.h"
#include "LC_ServerPlayer.h"
#include "UT_ServerHelper.h"

using namespace Protocol;
using namespace Utility;

BOOL SendMsgToGameDB(uint16_t usMsgType, MG_BODY* pkMsgBody, uint32_t uiUserID, const unique_id_impl& uiCharID, int32_t iSequence)
{
	return GetGameServerApp()->SendMsgToServerType(SERVER_TYPE_DB, 0, usMsgType, pkMsgBody, uiUserID, uiCharID, iSequence);
}

BOOL SendMsgToMailDB(uint16_t usMsgType, MG_BODY* pkMsgBody, uint32_t uiUserID, const unique_id_impl& uiCharID, int32_t iSequence)
{
	return GetGameServerApp()->SendMsgToServerType(SERVER_TYPE_MAIL_DB, 0, usMsgType, pkMsgBody, uiUserID, uiCharID, iSequence);
}

BOOL SendMsgToGroup(uint16_t usMsgType, MG_BODY* pkMsgBody, uint32_t uiUserID, const unique_id_impl& uiCharID, int32_t iSequence)
{
	return GetGameServerApp()->SendMsgToServerType(SERVER_TYPE_GROUP, 0, usMsgType, pkMsgBody, uiUserID, uiCharID, iSequence);
}

BOOL SendMsgToLogin(uint16_t usMsgType, MG_BODY* pkMsgBody, uint32_t uiUserID, const unique_id_impl& uiCharID, int32_t iSequence, int32_t iReserved)
{
	return GetGameServerApp()->SendMsgToServerType(SERVER_TYPE_LOGIN, 0, usMsgType, pkMsgBody, uiUserID, uiCharID, iSequence, iReserved);
}

BOOL SendMsgToChat(uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody, uint32_t uiUserID, const unique_id_impl& uiCharID, int32_t iSequence, int32_t iReserved, uint16_t nRouteFlag)
{
	return GetGameServerApp()->SendMsgToServerType(SERVER_TYPE_IM, 0, usMsgType, pkMsgBody, uiUserID, uiCharID, iSequence, iReserved, nRouteFlag);
}

BOOL SendMsgToTrade(uint16_t usMsgType, MG_BODY* pkMsgBody, uint32_t uiUserID, const unique_id_impl& uiCharID, int32_t iSequence)
{
	return GetGameServerApp()->SendMsgToServerType(SERVER_TYPE_TRADE, 0, usMsgType, pkMsgBody, uiUserID, uiCharID, iSequence);
}

BOOL SendMsgToSociety(uint16_t usMsgType, MG_BODY* pkMsgBody, uint32_t uiUserID, const unique_id_impl& uiCharID, int32_t iSequence, int32_t iReserved)
{
	return GetGameServerApp()->SendMsgToServerType(SERVER_TYPE_SOCIETY, 0, usMsgType, pkMsgBody, uiUserID, uiCharID, iSequence, iReserved);
}

BOOL SendMsgToGlobal(uint16_t usMsgType, MG_BODY* pkMsgBody, uint32_t uiUserID, const unique_id_impl& uiCharID, int32_t iSequence)
{
	return GetGameServerApp()->SendMsgToServerType(SERVER_TYPE_GLOBAL, 0, usMsgType, pkMsgBody, uiUserID, uiCharID, iSequence);
}

BOOL SendMsgToGame(int32_t iID, uint16_t usMsgType, MG_BODY* pkMsgBody, uint32_t uiUserID, const unique_id_impl& uiCharID, int32_t iSequence)
{
	return GetGameServerApp()->SendMsgToServerType(SERVER_TYPE_GAME, iID, usMsgType, pkMsgBody, uiUserID, uiCharID, iSequence);
}

BOOL SendMsgToGame(int32_t iID, uint16_t usMsgType, const char* pMsg, uint16_t nSize, uint32_t uiUserID, const unique_id_impl& uiCharID, int32_t iSequence)
{
	return GetGameServerApp()->SendMsgToServerType(SERVER_TYPE_GAME, iID, usMsgType, pMsg, nSize, uiUserID, uiCharID, iSequence);
}

BOOL SendMsgToGateway(int32_t iID, uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody, uint32_t uiUserID, const unique_id_impl& uiCharID, int32_t iSequence)
{
	return GetGameServerApp()->SendMsgToServerType(SERVER_TYPE_GATEWAY, iID, usMsgType, pkMsgBody, uiUserID, uiCharID, iSequence);
}

BOOL SendMsgToClientByGateway(int32_t iID, uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody, uint32_t uiUserID, const unique_id_impl& uiCharID, int32_t iSequence)
{
	return GetGameServerApp()->SendMsgToClientByGateway(iID, usMsgType, pkMsgBody, uiUserID, uiCharID, iSequence);
}
BOOL SendMsgFightTranscribeRecord(int32_t iID, uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody, uint32_t uiUserID, const unique_id_impl& uiCharID, int32_t iSequence)
{
	return GetGameServerApp()->SendMsgFightTranscribeRecord(iID, usMsgType, pkMsgBody, uiUserID, uiCharID, iSequence);
}

BOOL SendMsgToClientByGateway(int32_t iID,uint16_t usMsgType, const char* pMsg, uint16_t nSize, uint32_t uiUserID, const unique_id_impl& uiCharID, int32_t iSequence)
{
	return GetGameServerApp()->SendMsgToClientByGateway(iID, usMsgType, pMsg, nSize, uiUserID, uiCharID, iSequence);
}

BOOL RouteMsgToClientByGame(int32_t iID, int32_t iGateID, uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody, uint32_t uiUserID, const unique_id_impl& uiCharID, int32_t iSequence)
{
	return GetGameServerApp()->SendMsgToServerType(SERVER_TYPE_GAME, iID, usMsgType, pkMsgBody, uiUserID, uiCharID, iSequence, iGateID, MG_SERVER_ROUTER_FLAG_CLIENT);
}

BOOL RouteMsgToClientByGame(int32_t iID, int32_t iGateID, uint16_t usMsgType, const char* pMsg, uint16_t nSize, uint32_t uiUserID, const unique_id_impl& uiCharID, int32_t iSequence)
{
	return GetGameServerApp()->SendMsgToServerType(SERVER_TYPE_GAME, iID, usMsgType, pMsg, nSize, uiUserID, uiCharID, iSequence, iGateID, MG_SERVER_ROUTER_FLAG_CLIENT);
}

BOOL MulticastMsgToGateway(LC_PlayerPtrList& playerList, uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody, GameLogic::LC_PlayerOperator* pkFilter)
{
	return GetGameServerApp()->MulticastMsgToGateway(playerList, usMsgType, pkMsgBody, pkFilter);
}

BOOL MulticastMsgToGateway(const GameLogic::LC_PlayerIDPtrMap& playerList, uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody, GameLogic::LC_PlayerOperator* pkFilter)
{
	return GetGameServerApp()->MulticastMsgToGateway(playerList, usMsgType, pkMsgBody, pkFilter);
}

BOOL MulticastMsgToGateway(mem::vector<const GameLogic::LC_PlayerIDPtrMap*>& playerList, uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody, GameLogic::LC_PlayerOperator* pkFilter)
{
	return GetGameServerApp()->MulticastMsgToGateway(playerList, usMsgType, pkMsgBody, pkFilter);
}

BOOL BroadcastMsgToClient(uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody, bool bGlobal, int32_t nServerID)
{
	if(nServerID == 0)
	{
		nServerID = UT_ServerHelper::GetServerId();
	}
	if(bGlobal == true)
	{
		return SendMsgToChat(usMsgType, pkMsgBody, 0, 0, nServerID, 0, MG_SERVER_ROUTER_FLAG_GROUP);
	}
	else
	{
		return SendMsgToChat(usMsgType, pkMsgBody, 0, 0, nServerID, 0, MG_SERVER_ROUTER_FLAG_SINGLE);
	}
}

int32_t GetGateWayServerID(LC_ServerPlayer* player)
{
	int32_t serverID = 0;
	if (player && player->GetSession())
	{
		return player->GetSession()->PeerID();
	}
	return serverID;
}

int32_t GetGateWayServerID(NW_ServerSession* session)
{
	int32_t serverID = 0;
	if (session)
	{
		return session->PeerID();
	}
	return serverID;
}

