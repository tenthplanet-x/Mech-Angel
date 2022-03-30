#include "static_singleton.hpp"
#include "HL_PacketHandler.h"
#include "UT_TimeManager.h"
#include "UT_Log.h"
#include "MG_TYPES.h"
#include <boost/chrono/chrono.hpp>

#include "LC_ServerWorldManager.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerPlayerEnterLeaveManager.h"
#include "GameServerApp.h"

#ifndef WIN32
	#include "coredump.h"
#else
#include "XDebugDumper.h"
#endif

using namespace Utility;

namespace Protocol
{
	struct handler_info
	{
		int					type;
		int					source_type;
		bool				proxy;
		int					file_line;
		handler_func_type	func;
		char*				file_name;

		handler_info(void) 
			: type(0), source_type(0), proxy(false), file_line(0), func(0), file_name(0)
		{}
		inline bool proxy_enable(void) const {return proxy;}
	};

	struct handler_info_arr
	{
		inline void reset(int itp)
		{
			type = itp;
			msecs = 0;
			count = 0;
			size = 0;
			scount = 0;
			ssize = 0;
			handlerList.clear();
		}
		int	type;
		uint64_t msecs;
		int	count;
		uint64_t size;
		int	scount;
		uint64_t ssize;
		mem::list<handler_info>	handlerList;
	};

	struct handler_info_mgr
	{
		handler_info_arr handlers[MGPT_END];
		handler_info_mgr()
		{
			for (int i = 0 ; i < MGPT_END; ++i)
			{
				handlers[i].reset(i);
			}
		}
	};

	typedef Utility::static_singleton<handler_info_mgr> handler_info_mgr_singleton_type;

	void register_handler(int msg_type, int source_type, bool proxy, handler_func_type handler, const char* filename , int fileline)
	{
		//std::cerr<<"msg_type:"<<msg_type<<",source_type:"<<source_type<<std::endl;
		handler_info_arr* handlers = handler_info_mgr_singleton_type::instance().handlers;
		if (msg_type <= 0 || msg_type >= MGPT_END)
		{
			GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "bad msg type", msg_type, filename, fileline);
			return;
		}
		handler_info_arr& arr = handlers[msg_type];
		handler_info hd;
		hd.file_name = (char*)filename;
		hd.file_line = fileline;
		hd.func = handler;
		hd.type = msg_type;
		hd.source_type = source_type;
		hd.proxy = proxy;
		arr.handlerList.push_back(hd);
	}
	static LC_ServerPlayer* gs_Player = NULL;
	void route_msg(Protocol::SS_MG_HEAD& msg_head, NW_ServerSession* session, handler_info& handler, char* msg_body, uint16_t body_size)
	{
		static LC_ServerWorldManager* pkWorldMgr = SERVER_GET_WORLD_MANAGER();
		gs_Player = NULL;
		if (handler.proxy_enable())
		{
			if (msg_head.GetProxyFlag() && msg_head.m_iReserved)
			{
				GameLogic::LC_ActorBase* target = (GameLogic::LC_ActorBase*)pkWorldMgr->FindObject(msg_head.m_iReserved);
				handler.func(msg_head, session, target, msg_body, body_size);
			}
			else if (msg_head.m_uiCharID)
			{
				static LC_ServerPlayerManager* pkPlayerMgr = SERVER_GET_PLAYER_MANAGER();
				LC_ServerPlayer* player = NULL;
				if (SERVER_TYPE_CLIENT == msg_head.m_cSrcType)
				{
					player = (LC_ServerPlayer*)pkPlayerMgr->GetPlayerBySession(msg_head.m_uiUserID, msg_head.m_uiCharID, session);
				}
				else
				{
					player = (LC_ServerPlayer*)pkPlayerMgr->FindPlayer(msg_head.m_uiCharID);
				}
				gs_Player = player;
				if (player && player->GetAsAControler().IsValid())
				{
					static GameServerApp* pkApp = GetGameServerApp(); 
					Protocol::SS_MG_HEAD route_head = msg_head;
					route_head.SetProxyFlag();
					route_head.m_uiUserID = player->GetControleePeer().UserID();
					route_head.m_uiCharID = player->GetControleePeer().CharID();
					/*route_head.m_cDstPlat = player->GetControleePeer().ServerPlatform();
					route_head.m_cDstType = player->GetControleePeer().ServerType();*/
					route_head.m_iDstID = player->GetControleePeer().ServerID();
					route_head.m_iReserved = player->GetControleePeer().ObjectID();
					pkApp->SendMsgToServerType(route_head, msg_body, body_size);
				}
				else
				{
					handler.func(msg_head, session, player, msg_body, body_size);
				}
			}
			else
			{
				handler.func(msg_head, session, NULL, msg_body, body_size);
			}
		}
		else
		{
			static LC_ServerPlayerManager* pkPlayerMgr = SERVER_GET_PLAYER_MANAGER();
			LC_ServerPlayer* player = NULL;
			if (SERVER_TYPE_CLIENT == msg_head.m_cSrcType)
			{
				player = (LC_ServerPlayer*)pkPlayerMgr->GetPlayerBySession(msg_head.m_uiUserID, msg_head.m_uiCharID, session);
			}
			else
			{
				player = (LC_ServerPlayer*)pkPlayerMgr->FindPlayer(msg_head.m_uiCharID);
			}
			gs_Player = player;
			handler.func(msg_head, session, player, msg_body, body_size);
		}
	}

	void route_msg_SEH_shell(Protocol::SS_MG_HEAD& msg_head, NW_ServerSession* session, handler_info& handler, char* msg_body, uint16_t body_size)
	{
//#ifdef WIN32
#if 0
		__try
		{
			route_msg(msg_head, session, handler, msg_body, body_size);
		}
		__except(CrashHandler(GetExceptionInformation()))
		{
			//LC_ServerPlayerEnterLeaveManager* pMgr = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
			//if ( pMgr && gs_Player)
			//	pMgr->UserLeaveServer(gs_Player,LMIOT_EXCEPTION);
		}
#else
		route_msg(msg_head, session, handler, msg_body, body_size);
#endif
	}

#define ISGAMECLASSTYPE(x) (x==SERVER_TYPE_GAME || x==SERVER_TYPE_INSTANCE)
	void process_packet(Protocol::SS_MG_HEAD& msg_head, NW_ServerSession* session, char* msg_body, uint16_t body_size)
	{
		handler_info_arr* handlers = handler_info_mgr_singleton_type::instance().handlers;
		uint16_t msg_type = msg_head.m_usMsgType;
		if (msg_type < 0 || msg_type >= MGPT_END)
		{
			return;
		}
		handler_info_arr& arr = handlers[msg_type];
		if(arr.type != msg_type)
		{
			return;
		}

		if (msg_head.GetRouterGClientFlag())
		{
			SendMsgToClientByGateway(msg_head.m_iReserved, msg_head.m_usMsgType, msg_body, body_size, msg_head.m_uiUserID, msg_head.m_uiCharID, msg_head.m_iSequence);
		}
		else if(msg_head.GetMulticastFlag())
		{
			static MulticastMsgHelper& rkHelper = GetGameServerApp()->GetMulticastMsgHelper();
			rkHelper.Begin(msg_head.m_usMsgType, NULL);
			if (rkHelper.multiCastMessage.Decode(msg_body, body_size))
			{
				Protocol::PS_UserSocketIndices kIndices = rkHelper.multiCastMessage.m_akUserInfo;
				Protocol::PS_UserSocketIndices::iterator it = kIndices.begin();
				for (; it!=kIndices.end(); ++it)
				{
					rkHelper.AddUserIndices(*it);
				}
				rkHelper.End();
			}
		}
		else
		{
			for(mem::list<handler_info>::iterator iter = arr.handlerList.begin(); iter != arr.handlerList.end(); iter++)
			{
				handler_info& handler = *iter;
				if(handler.source_type != msg_head.m_cSrcType)
				{
					if (!ISGAMECLASSTYPE(handler.source_type) || !ISGAMECLASSTYPE(msg_head.m_cSrcType))
					{
						continue;
					}
				}
				if (NULL == handler.func)
				{
					continue;
				}

				//std::cerr<<"process_packet, msg_type:"<<msg_type<<",body_size:"<<body_size<<std::endl;
				++arr.count;
				//boost::chrono::system_clock::time_point t1 = boost::chrono::system_clock::now();
				try
				{
					route_msg_SEH_shell(msg_head, session, handler, msg_body, body_size);
				}
				catch (...)
				{
					GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "process_packet except", msg_type, msg_head.m_cSrcType, body_size);
#ifndef WIN32
					saveBackTrace(0, "gameExcept");
#endif
				}
				//boost::chrono::system_clock::time_point t2 = boost::chrono::system_clock::now();
				//boost::chrono::system_clock::duration diff = t2 - t1;
				//boost::chrono::microseconds ms =  boost::chrono::duration_cast<boost::chrono::microseconds>(diff);
				//arr.msecs += ms.count();
				//arr.size += body_size;
			}
		}
	}

	void record_send_packet(int msg_type, uint16_t body_size)
	{
		handler_info_arr* handlers = handler_info_mgr_singleton_type::instance().handlers;
		if (msg_type < 0 || msg_type >= MGPT_END)
		{
			return;
		}
		handler_info_arr* handle = &handlers[msg_type];
		++handle->scount;
		handle->ssize += body_size;
	}

	void process_packet(NW_ServerSession* session, char* msg_body, uint16_t body_size)
	{
		Protocol::SS_MG_HEAD kMsgHead;
		Protocol::MG_PACKET_DECODE_HANDLER kHandler;
		if (false == kHandler.DecodeSSMsg(msg_body, body_size, kMsgHead))
		{
			return;
		}
		process_packet(kMsgHead, session, msg_body, body_size);
	}

	bool cmp_count(const handler_info_arr* a , const handler_info_arr* b)
	{
		return a->count > b->count;
	}

	bool cmp_scount(const handler_info_arr* a , const handler_info_arr* b)
	{
		return a->scount > b->scount;
	}

	bool cmp_ssize(const handler_info_arr* a , const handler_info_arr* b)
	{
		return a->ssize > b->ssize;
	}

	bool cmp_time(const handler_info_arr* a , const handler_info_arr* b)
	{
		return a->msecs > b->msecs;
	}

	bool cmp_pretime(const handler_info_arr* a , const handler_info_arr* b)
	{
		return (a->msecs / a->count) > (b->msecs / b->count);
	}

	void dump_packet_process_info()
	{
		handler_info_arr* handlers = handler_info_mgr_singleton_type::instance().handlers;
		GfxWriteLog(LOG_TAG_RES, LOG_SWITCH_RES, "---------------------- dump packet process -------------------------------");
		static const int __st_sz = 255;
		handler_info_arr* hs[__st_sz] = { 0 };
		handler_info_arr* hs1[__st_sz] = { 0 };
		int count = 0 , count1 = 0;

		int64_t iTotalRecvSize = 0;
		int64_t iTotalRecvCount = 0;
		int64_t iTotalSendSize = 0;
		int64_t iTotalSendCount = 0;

		for (int i = 0 ; i < MGPT_END; ++i)
		{
			handler_info_arr* handle = &handlers[i];
			if (handle->count)
			{
				iTotalRecvSize += handle->size;
				iTotalRecvCount += handle->count;
			}
			if (handle->scount)
			{
				iTotalSendSize += handle->ssize;
				iTotalSendCount += handle->scount;
			}
			if ((handle->count && handle->msecs > 1000)  && count < __st_sz)
			{
				hs[count++] = handle;
			}
			if ((handle->scount) > 0 && count1 < __st_sz)
			{
				hs1[count1++] = handle;
			}
		}
		StringStreamType ss;
		ss << std::endl;

		if (iTotalRecvCount > 0)
		{
			ss << "handle msg recv total:" << iTotalRecvSize << "count:" << iTotalRecvCount << "avg:" << iTotalRecvSize / iTotalRecvCount << "\n";
		}

		if (iTotalSendCount > 0)
		{
			ss << "handle msg send total:" << iTotalSendSize << "count:" << iTotalSendCount << "avg:" << iTotalSendSize / iTotalSendCount << "\n";
		}

		std::sort(&hs[0], &hs[count], cmp_count);
		ss << "[process_handle_info sort by count] type:\n";
		for (int i = 0 ; i < 20 && i < count; ++i)
		{
			handler_info_arr* handle = hs[i];
			uint64_t pre_msec = handle->msecs / handle->count;
			ss << " type = " << handle->type << " count = " << handle->count << " mircosec = " << handle->msecs << " per_msec = " << pre_msec << " size = " << handle->size << std::endl;
		}
		StringType str = ss.str();
		GfxWriteLog(LOG_TAG_RES, LOG_SWITCH_RES, str.c_str());
		ss.str("");
		std::sort(&hs1[0], &hs1[count1], cmp_scount);
		ss << "------------------------------------------------------\n";
		ss << "[process_handle_info sort by send count] type:\n";
		for (int i = 0 ; i < 20 && i < count1; ++i)
		{
			handler_info_arr* handle = hs1[i];
			ss << " type = " << handle->type << " count = " << handle->scount << " size = " << handle->ssize << std::endl;
		}
		str = ss.str();
		GfxWriteLog(LOG_TAG_RES, LOG_SWITCH_RES, str.c_str());
		ss.str("");
		std::sort(&hs1[0], &hs1[count1], cmp_ssize);
		ss << "------------------------------------------------------\n";
		ss << "[process_handle_info sort by send size] type:\n";
		for (int i = 0 ; i < 20 && i < count1; ++i)
		{
			handler_info_arr* handle = hs1[i];
			ss << " type = " << handle->type << " count = " << handle->scount << " size = " << handle->ssize << std::endl;
		}
		str = ss.str();
		GfxWriteLog(LOG_TAG_RES, LOG_SWITCH_RES, str.c_str());
		ss.str("");
		std::sort(&hs[0], &hs[count], cmp_pretime);
		ss << "------------------------------------------------------\n";
		ss << "[process_handle_info sort by pertime] type:\n";
		for (int i = 0 ; i < 20 && i < count; ++i)
		{
			handler_info_arr* handle = hs[i];
			uint64_t pre_msec = handle->msecs / handle->count;
			ss << " type = " << handle->type << " count = " << handle->count << " mircosec = " << handle->msecs << " per_msec = " << pre_msec << " size = " << handle->size << std::endl;
		}
		str = ss.str();
		GfxWriteLog(LOG_TAG_RES, LOG_SWITCH_RES, str.c_str());
		ss.str("");
		ss << "------------------------------------------------------\n";
		std::sort(&hs[0], &hs[count], cmp_time);
		ss << "[process_handle_info sort by totaltime] type:\n";
		for (int i = 0 ; i < 20 && i < count; ++i)
		{
			handler_info_arr* handle = hs[i];
			uint64_t pre_msec = handle->msecs / handle->count;
			ss << " type = " << handle->type << " count = " << handle->count << " mircosec = " << handle->msecs << " per_msec = " << pre_msec << " size = " << handle->size << std::endl;
		}
		str = ss.str();
		GfxWriteLog(LOG_TAG_RES, LOG_SWITCH_RES, str.c_str());
		for (int i = 0 ; i < count; ++i)
		{
			handler_info_arr* handle = hs[i];
			handle->count = 0;
			handle->size = 0;
			handle->msecs = 0;
			handle->scount = 0;
			handle->ssize = 0;
		}
	}

	void g_registerGameHandler()
	{
		#include "Hl_handler_reg.h"
	}
}

