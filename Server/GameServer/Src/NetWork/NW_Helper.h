#ifndef _NW_HELPER_H
#define _NW_HELPER_H

#include "MG_PACKET.h"
#include "MM_MemoryAllocatorConfig.h"
#include "LC_Define.h"
#include "LC_ActorBase.h"
#include "LC_ServerPlayer.h"

class LC_ServerPlayer;
class NW_ServerSession;
typedef mem::vector<LC_ServerPlayer*>	LC_PlayerPtrList;

BOOL SendMsgToGameDB(uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody = NULL, uint32_t uiUserID = 0, const unique_id_impl& uiCharID = 0, int32_t iSequence = 0);
BOOL SendMsgToMailDB(uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody = NULL, uint32_t uiUserID = 0, const unique_id_impl& uiCharID = 0, int32_t iSequence = 0);
BOOL SendMsgToGroup(uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody = NULL, uint32_t uiUserID = 0, const unique_id_impl& uiCharID = 0, int32_t iSequence = 0);
BOOL SendMsgToLogin(uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody = NULL, uint32_t uiUserID = 0, const unique_id_impl& uiCharID = 0, int32_t iSequence = 0, int32_t iReserved = 0);
BOOL SendMsgToChat(uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody = NULL, uint32_t uiUserID = 0, const unique_id_impl& uiCharID = 0, int32_t iSequence = 0, int32_t iReserved = 0, uint16_t nRouteFlag = 0);
BOOL SendMsgToTrade(uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody = NULL, uint32_t uiUserID = 0, const unique_id_impl& uiCharID = 0, int32_t iSequence = 0);
BOOL SendMsgToSociety(uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody = NULL, uint32_t uiUserID = 0, const unique_id_impl& uiCharID = 0, int32_t iSequence = 0, int32_t iReserved = 0);
BOOL SendMsgToGlobal(uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody = NULL, uint32_t uiUserID = 0, const unique_id_impl& uiCharID = 0, int32_t iSequence = 0);

BOOL SendMsgToGame(int32_t iID, uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody = NULL, uint32_t uiUserID = 0, const unique_id_impl& uiCharID = 0, int32_t iSequence = 0);
BOOL SendMsgToGame(int32_t iID, uint16_t usMsgType, const char* pMsg, uint16_t nSize, uint32_t uiUserID = 0, const unique_id_impl& uiCharID = 0, int32_t iSequence = 0);
BOOL SendMsgToGateway(int32_t iID, uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody = NULL, uint32_t uiUserID = 0, const unique_id_impl& uiCharID = 0, int32_t iSequence = 0);
BOOL SendMsgToClientByGateway(int32_t iID, uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody = NULL, uint32_t uiUserID = 0, const unique_id_impl& uiCharID = 0, int32_t iSequence = 0);
BOOL SendMsgToClientByGateway(int32_t iID, uint16_t usMsgType, const char* pMsg, uint16_t nSize, uint32_t uiUserID = 0, const unique_id_impl& uiCharID = 0, int32_t iSequence = 0);
BOOL SendMsgFightTranscribeRecord(int32_t iID, uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody = NULL, uint32_t uiUserID = 0, const unique_id_impl& uiCharID = 0, int32_t iSequence = 0);


BOOL RouteMsgToClientByGame(int32_t iID, int32_t iGateID, uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody = NULL, uint32_t uiUserID = 0, const unique_id_impl& uiCharID = 0, int32_t iSequence = 0);
BOOL RouteMsgToClientByGame(int32_t iID, int32_t iGateID, uint16_t usMsgType, const char* pMsg, uint16_t nSize, uint32_t uiUserID = 0, const unique_id_impl& uiCharID = 0, int32_t iSequence = 0);
BOOL MulticastMsgToGateway(LC_PlayerPtrList& playerList, uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody = NULL, GameLogic::LC_PlayerOperator* pkFilter = NULL);
BOOL MulticastMsgToGateway(const GameLogic::LC_PlayerIDPtrMap& playerList, uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody = NULL, GameLogic::LC_PlayerOperator* pkFilter = NULL);
BOOL MulticastMsgToGateway(mem::vector<const GameLogic::LC_PlayerIDPtrMap*>& playerList, uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody = NULL, GameLogic::LC_PlayerOperator* pkFilter = NULL);

BOOL BroadcastMsgToClient(uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody = NULL, bool bGlobal = false, int32_t nServerID = 0);

int32_t GetGateWayServerID(LC_ServerPlayer* player);
int32_t GetGateWayServerID(NW_ServerSession* session);

#endif