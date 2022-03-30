#ifndef _HL_PACKET_HANDLER_H
#define _HL_PACKET_HANDLER_H

#include "MG_PACKET.h"

class NW_ServerSession;
namespace GameLogic{
	class LC_ActorBase;
}

namespace Protocol
{

	typedef void (* handler_func_type)(Protocol::SS_MG_HEAD& msg_head, NW_ServerSession* session, GameLogic::LC_ActorBase* target, char* msg_body, uint16_t body_size);

	void register_handler(int msg_type, int src_type, handler_func_type handler , const char* filename=NULL, int fileline=0, bool proxy=false);

	void process_packet(NW_ServerSession* session, char* msg_body, uint16_t body_size);

	void process_packet(Protocol::SS_MG_HEAD& msg_head, NW_ServerSession* session, char* msg_body, uint16_t body_size);

	void record_send_packet(int msg_type, uint16_t body_size);

	void dump_packet_process_info();

	void g_registerGameHandler();

	#include "Hl_handler_def.h"
}

#define ON_HANDLE_IMPL(msg_type, src_type, proxy)\
	void this_process_handle_ ## msg_type ## _ ## src_type(Protocol::SS_MG_HEAD& rkMsgHead, NW_ServerSession * pkSession, GameLogic::LC_ActorBase* pkTarget,char* pMsgBody, uint16_t usBodySize)

#define ON_HANDLE(msg_type, src_type, proxy) ON_HANDLE_IMPL(msg_type, src_type, proxy)

#endif
