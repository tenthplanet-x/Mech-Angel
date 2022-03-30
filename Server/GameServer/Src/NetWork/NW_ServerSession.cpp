#include "SystemInclude.h"
#include "NW_ServerSession.h"
#ifndef ACTIVATION_CODE
#include "GameServerApp.h"
#include "NW_Helper.h"
#include "LC_ServerPlayerEnterLeaveManager.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerWorldManager.h"
#include "HL_PacketHandler.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerPlayer.h"
#include "CF_CrossRealmMap.h"
#include "oss_define.h"
#include "MG_Game2Guild.h"
#include "LC_ServerWarManager.h"
using namespace CSVFile;
#else
#include "ACGameServerApp.h"
#endif
using namespace Utility;
using namespace NetWork;
using namespace Protocol;

const char* GetServerDesc(MG_SERVER_TYPES server_type)
{
	switch (server_type)
	{
		case SERVER_TYPE_LOGIN:
			return "login server";
		case SERVER_TYPE_GAME:
			return "game server";
		case SERVER_TYPE_DB:
			return "db server";
		case SERVER_TYPE_GROUP:
			return "group server";
		case SERVER_TYPE_MAIL_DB:
			return "mail server";
		case SERVER_TYPE_CLIENT:
			return "client";
		case SERVER_TYPE_IM:
			return "chat server";
		case SERVER_TYPE_GATEWAY:
			return "gateway server";
		case SERVER_TYPE_INSTANCE:
			return "instance server";
		case SERVER_TYPE_TRADE:
			return "trade server";
		case SERVER_TYPE_SOCIETY:
			return "society server";
		case SERVER_TYPE_GLOBAL:
			return "global server";
		default:
			break;
	}
	return "unkown server";
}
//---------------------------------------------------
NW_ServerSession::NW_ServerSession()
	: NW_NetSessionBase(),
	  m_OnConnectHandler(NULL),
	  m_OnDisConnectHandler(NULL),
	  m_messageRecorder(NULL)
{}
//---------------------------------------------------
NW_ServerSession::~NW_ServerSession()
{
}
#ifndef ACTIVATION_CODE
void NW_ServerSession::SetServerInfo(const Protocol::PS_ServerInfo& rkSvrInfo)
{
	m_SelfServerInfo = rkSvrInfo;
	const PS_ServerInfo& local = GetGameServerApp()->GetServerInfo();
	SetPeerInfo(local.m_iType, local.m_iServerID, m_SelfServerInfo.m_iType, m_SelfServerInfo.m_iServerID, GetServerDesc((MG_SERVER_TYPES)m_SelfServerInfo.m_iType));
	SetConnectIPAddress(m_SelfServerInfo.m_szIP.c_str(), m_SelfServerInfo.m_iPort);
}
void NW_ServerSession::Init(NetSessionType session_type , MG_SERVER_TYPES server_type)
{
	m_eSessionType = session_type;
	SetHeartBeatFrequency(HEARTBEART_SERVER_TO_SERVER);
	SetServerInfo(m_SelfServerInfo);
	m_OnConnectHandler = NULL;
}

BOOL NW_ServerSession::Send(char* pMsg, WORD wSize)
{
	return NW_NetSessionBase::Send(pMsg, wSize);
}

//---------------------------------------------------
VOID NW_ServerSession::OnRecv(char* pMsg, WORD wSize)
{
	if (false == m_decodeHandler.DecodeSSMsg(pMsg, wSize, m_ssMessageHead))
	{
		return;
	}
	process_packet(m_ssMessageHead, this, pMsg, wSize);
}

//---------------------------------------------------
VOID NW_ServerSession::OnDisconnect()
{
	GfxWriteFmtLog(LOG_SYSTEM_FILELINE, LOG_SWITCH_SYSTEM, "disconnected with", GetServerDesc((MG_SERVER_TYPES)m_cPeerServerType) , m_kConnectIP.c_str());
	if (!m_bHasBeenConnected)
	{
		return;
	}
	m_bHasBeenConnected = false;
	NW_NetSessionBase::OnDisconnect();
	if (m_OnDisConnectHandler)
	{
		m_OnDisConnectHandler(this);
	}
}

VOID NW_ServerSession::OnConnect(BOOL bSuccess, DWORD dwNetworkIndex)
{
	NW_NetSessionBase::OnConnect(bSuccess, dwNetworkIndex);
	if (bSuccess)
	{
		if (!m_bHasBeenConnected)
		{
			//发送服务器初始化消息
			m_bHasBeenConnected = true;
			if (m_OnConnectHandler)
			{
				m_OnConnectHandler(this);
			}
		}
		GfxWriteFmtLog(LOG_SYSTEM_FILELINE, LOG_SWITCH_SYSTEM, "connected with", GetServerDesc((MG_SERVER_TYPES)m_cPeerServerType) , m_kConnectIP.c_str());
	}
	else
	{
		m_bHasBeenConnected = false;
	}
}
//---------------------------------------------------
bool server_io_session::recv(void)
{
	char* packet = NULL;
	uint16_t pack_size = 0;
	packet = peek_packet(pack_size);
	while( packet && pack_size > 0)
	{
		if( pack_size > max_pack_sz_)
		{
			GfxWriteFmtLog(LOG_SYSTEM_FILELINE, Utility::LOG_SWITCH_SYSTEM, "recv max packet" , pack_size);
			return false;
		}

		//handle
		if (Session())
		{
			Session()->OnRecv(packet, pack_size);
		}
		else 
		{
			Protocol::SS_MG_HEAD kMsgHead;
			if (kMsgHead.Decode(packet, pack_size) && MGPT_SESSION_TYPE == kMsgHead.m_usMsgType)
			{
				Protocol::PS_ServerInfo rkSvr;
				rkSvr.m_iType = kMsgHead.m_cSrcType;
				rkSvr.m_iServerID = kMsgHead.m_iSrcID;
				NW_SIOSession* pkGameSession = dynamic_cast<NW_SIOSession*>(GetGameServerApp()->RegisterGameServer(rkSvr));
				if (pkGameSession)
				{
					pkGameSession->sio_session_ = shared_from_this();
					parent = pkGameSession;
				}
				else
				{
					close();
					return false;
				}
			}
			else
			{
				close();
				return false;
			}
		}
		
		read_buf_.dequeue(NULL, pack_size);

		pack_size = 0;
		packet = peek_packet(pack_size);
	}
	return true;
}
void server_io_session::on_opened(void)
{
	//init
	if (Session())
	{
		Session()->OnConnect(TRUE, 0);
	}
	else
	{

	}
}
void server_io_session::on_closed(void)
{
	if (Session())
	{
		Session()->OnDisconnect();
	}
}
void NW_SIOSession::TryToConnect(void)
{
	//id 大的链接小的
	if (!sio_session_ && GetConnectPort())
	{
		//id 小的链接大的
		static GameServerApp* pkApp = GetGameServerApp();
		if (pkApp->IsBranchNode() && GetGameServerID() < GetServerInfo().m_iServerID)
		{
			return;
		}

		static NW_SIOSessionManager* pkSIOMgr = NW_SIOSessionManager::GetSingletonPtr();
		sio_session_ = pkSIOMgr->connect(GetConnectIP().c_str(), GetConnectPort());
		sio_session_->shared_from_this()->parent = this;
	}
}
VOID NW_SIOSession::Disconnect( BOOL bGracefulDisconnect)
{
	if (sio_session_)
	{
		sio_session_->close();
		OnDisconnect();
	}
}
VOID NW_SIOSession::OnDisconnect()
{
	NW_ServerSession::OnDisconnect();
	if (sio_session_)
	{
		sio_session_->shared_from_this()->parent = NULL;
		sio_session_.reset();
	}
}
VOID NW_SIOSession::OnConnect( BOOL bSuccess, DWORD dwNetworkIndex )
{
	NW_ServerSession::OnConnect(bSuccess, dwNetworkIndex);
}
BOOL NW_SIOSession::Send(char *pMsg, WORD wSize) 
{ 
	if (sio_session_)
	{
		sio_session_->send(pMsg, wSize, serial++); 
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
//---------------------------------------------------
void NW_FakeSIOSession::TryToConnect(void)
{}
VOID NW_FakeSIOSession::Disconnect( BOOL bGracefulDisconnect)
{}
VOID NW_FakeSIOSession::OnDisconnect()
{}
VOID NW_FakeSIOSession::OnConnect( BOOL bSuccess, DWORD dwNetworkIndex )
{}
BOOL NW_FakeSIOSession::Send(char *pMsg, WORD wSize) 
{ 
	return local_buf_.enqueue(pMsg, wSize) ? TRUE : FALSE;
}
BOOL NW_FakeSIOSession::Recv(uint16_t max_pack_sz_)
{
	char* packet = NULL;
	uint16_t pack_size = 0;
	packet = peek_packet(pack_size);
	while( packet && pack_size > 0)
	{
		if( pack_size > max_pack_sz_)
		{
			GfxWriteFmtLog(LOG_SYSTEM_FILELINE, Utility::LOG_SWITCH_SYSTEM, "recv max packet" , pack_size);
			return FALSE;
		}

		//handle
		OnRecv(packet, pack_size);

		local_buf_.dequeue(NULL, pack_size);

		pack_size = 0;
		packet = peek_packet(pack_size);
	}
	return TRUE;
}
char* NW_FakeSIOSession::peek_packet(uint16_t& pack_size)
{
	uint16_t tmp_size = 0;
	if (!local_buf_.peek((char*)&tmp_size, sizeof(uint16_t)))
	{
		return NULL;
	}
	pack_size = ntohs(tmp_size);
	if (local_buf_.length()<pack_size)
	{
		pack_size = 0;
		return NULL;
	}
	uint32_t readable_size = local_buf_.readable_length();
	if( readable_size<pack_size && !local_buf_.transfer_extra(pack_size - readable_size)) 
	{
		pack_size = NULL;
		return NULL;
	}
	return local_buf_.read_ptr();
}
//---------------------------------------------------
void NW_SIOSessionManager::SetLocalInfo(const Protocol::PS_ServerInfo& psSvrInfo)
{
	local_.Init(NST_GAMESERVER_TO_GAMESERVER, SERVER_TYPE_GAME);
	local_.SetServerInfo(psSvrInfo);
	GetGameServerApp()->AddGameServer(&local_);
}
//---------------------------------------------------
void SendServerInfo(MG_SERVER_TYPES servertype, int id)
{
	//发送GameServer的ID到LoginServer
	MG_GL_GameServer_Info kSvrInfo;
	LC_ServerPlayerEnterLeaveManager* serverPlayerEnterLeaveManager = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
	kSvrInfo.m_kServerInfo.m_iServerID		= GetGameServerApp()->GetServerInfo().m_iServerID;
	kSvrInfo.m_kServerInfo.m_iUserCount		= serverPlayerEnterLeaveManager->GetServerRealPlayerCount();
	kSvrInfo.m_kServerInfo.m_iCapacity		= GetGameServerApp()->GetServerInfo().m_iCapacity;
	kSvrInfo.m_kServerInfo.m_cStatus		= SERVER_STATUS_ON;

	kSvrInfo.m_kServerInfo.m_szIP			= GetGameServerApp()->GetServerInfo().m_szIP;
	kSvrInfo.m_kServerInfo.m_iPort			= GetGameServerApp()->GetServerInfo().m_iPort;

	GetGameServerApp()->SendMsgToServerType(servertype, id, MGPT_GL_REQ_UPDATE_SERVER_INFO, &kSvrInfo);
}

void on_db_server_connect(NW_ServerSession* session)
{
	GetGameServerApp()->GetServerManager()->DBServerConnect();
}

void on_login_server_connect(NW_ServerSession* session)
{
	SendMsgToLogin(MGPT_SERVER_INIT);
	SendServerInfo(SERVER_TYPE_LOGIN);
	// Report init map-line info to loginsvrd
	LC_ServerWorldManager* pkServerWorldManager = SERVER_GET_WORLD_MANAGER();
	if ( pkServerWorldManager )
	{
		pkServerWorldManager->ReportOrigMapLineInfo();
	}
}

void on_group_server_connect(NW_ServerSession* session)
{
	SendMsgToGroup(MGPT_SERVER_INIT);
	GetGameServerApp()->GetServerManager()->OnGroupServerReady();
}

void on_mail_server_connect(NW_ServerSession* session)
{
	SendMsgToMailDB(MGPT_SERVER_INIT);
}

void on_friend_server_connect(NW_ServerSession* session)
{
	SendMsgToSociety(MGPT_SERVER_INIT);
}

void on_chat_server_connect(NW_ServerSession* session)
{
	GameServerManager* pServerManager = GetGameServerApp()->GetServerManager();
	//if(pServerManager !=  NULL)
	{
		SendMsgToChat(MGPT_SERVER_INIT, NULL, 0, 0, pServerManager->GetActiveGroupId(pServerManager->GetCurServerId()));
	}
	GetGameServerApp()->GetServerManager()->ChatServerConnect();
}

void on_gateway_server_connect(NW_ServerSession* session)
{
	SendMsgToGateway(session->PeerID(), MGPT_SERVER_INIT);
}

void on_guild_server_connect(NW_ServerSession* session)
{
	GetGameServerApp()->GetServerManager()->SocietyServerConnect();
}

void on_trade_server_connect(NW_ServerSession* session)
{
	SendMsgToTrade(MGPT_SERVER_INIT);
	GetGameServerApp()->GetServerManager()->TradeServerConnect();
}

void on_society_server_connect(NW_ServerSession* session)
{
	SendMsgToSociety(MGPT_SERVER_INIT);
	GetGameServerApp()->GetServerManager()->SocietyServerConnect();
}
void on_global_server_connect(NW_ServerSession* session)
{
	SendMsgToGlobal(MGPT_SERVER_INIT);
	GetGameServerApp()->GetServerManager()->GlobalServerConnect();
}
void on_game_server_connect(NW_ServerSession* session)
{
	SendMsgToGame(session->PeerID(), MGPT_SERVER_INIT);
	GetGameServerApp()->GetServerManager()->GameServerConnect(session->PeerID());
	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "game server connected", session->PeerID());
}
//---------------------------------------------------------
void on_db_server_disconnect(NW_ServerSession* session)
{
	GetGameServerApp()->GetServerManager()->DBServerDisConnect();
}

void on_login_server_disconnect(NW_ServerSession* session)
{

}

void on_group_server_disconnect(NW_ServerSession* session)
{
	GetGameServerApp()->GetServerManager()->OnGroupServerLost();
}

void on_mail_server_disconnect(NW_ServerSession* session)
{

}

void on_friend_server_disconnect(NW_ServerSession* session)
{

}

void on_chat_server_disconnect(NW_ServerSession* session)
{

}

void on_gateway_server_disconnect(NW_ServerSession* session)
{

}

void on_gm_server_disconnect(NW_ServerSession* session)
{

}

void on_guild_server_disconnect(NW_ServerSession* session)
{
	GetGameServerApp()->GetServerManager()->SocietyServerDisConnect();
}

void on_award_server_disconnect(NW_ServerSession* session)
{

}

void on_trade_server_disconnect(NW_ServerSession* session)
{
	GetGameServerApp()->GetServerManager()->TradeServerDisConnect();
}

void on_society_server_disconnect(NW_ServerSession* session)
{
	GetGameServerApp()->GetServerManager()->SocietyServerDisConnect();
}

void on_global_server_disconnect(NW_ServerSession* session)
{
	GetGameServerApp()->GetServerManager()->GlobalServerDisConnect();
}

void on_game_server_disconnect(NW_ServerSession* session)
{
	GetGameServerApp()->GetServerManager()->GameServerDisConnect(session->PeerID());
}

NW_ServerSession* CreateServerSession(NetSessionType session_type , const Protocol::PS_ServerInfo& server_info)
{
	NW_ServerSession* session = NULL;
	switch (session_type)
	{
		case NST_GAMESERVER_TO_DBSERVER:
			{
				session = T_NEW_D NW_ServerSession;
				if (session)
				{
					session->GetServerInfo() = server_info;
					session->Init(session_type, SERVER_TYPE_DB);
					session->SetONConnectHandler(on_db_server_connect);
					session->SetONDisConnectHandler(on_db_server_disconnect);
				}
			}
			break;
		case NST_GAMESERVER_TO_LOGINSERVER:
			{
				session = T_NEW_D NW_ServerSession;
				if (session)
				{
					session->GetServerInfo() = server_info;
					session->Init(session_type, SERVER_TYPE_LOGIN);
					session->SetONConnectHandler(on_login_server_connect);
					session->SetONDisConnectHandler(on_login_server_disconnect);
				}
			}
			break;
		case NST_GAMESERVER_TO_GROUPSERVER:
			{
				session = T_NEW_D NW_ServerSession;
				if (session)
				{
					session->GetServerInfo() = server_info;
					session->Init(session_type, SERVER_TYPE_GROUP);
					session->SetONConnectHandler(on_group_server_connect);
					session->SetONDisConnectHandler(on_group_server_disconnect);
				}
			}
			break;
		case NST_GAMESERVER_TO_MAILSERVER:
			{
				session = T_NEW_D NW_ServerSession;
				if (session)
				{
					session->GetServerInfo() = server_info;
					session->Init(session_type, SERVER_TYPE_MAIL_DB);
					session->SetONConnectHandler(on_mail_server_connect);
					session->SetONDisConnectHandler(on_mail_server_disconnect);
				}
			}
			break;
		case NST_GAMESERVER_TO_CHATSERVER:
			{
				session = T_NEW_D NW_ServerSession;
				if (session)
				{
					session->GetServerInfo() = server_info;
					session->Init(session_type, SERVER_TYPE_IM);
					session->SetONConnectHandler(on_chat_server_connect);
					session->SetONDisConnectHandler(on_chat_server_disconnect);
				}
			}
			break;
		case NST_GAMESERVER_TO_GATEWAY:
			{
				session = T_NEW_D NW_ServerSession;
				if (session)
				{
					session->GetServerInfo() = server_info;
					session->Init(session_type, SERVER_TYPE_GATEWAY);
					session->SetONConnectHandler(on_gateway_server_connect);
					session->SetONDisConnectHandler(on_gateway_server_disconnect);
				}
			}
			break;
		case NST_GAMESERVER_TO_TRADESERVER:
			{
				session = T_NEW_D NW_ServerSession;
				if (session)
				{
					session->GetServerInfo() = server_info;
					session->Init(session_type, SERVER_TYPE_TRADE);
					session->SetONConnectHandler(on_trade_server_connect);
					session->SetONDisConnectHandler(on_trade_server_disconnect);
				}
			}
			break;
		case NST_GAMESERVER_TO_SOCIETYSERVER:
			{
				session = T_NEW_D NW_ServerSession;
				if (session)
				{
					session->GetServerInfo() = server_info;
					session->Init(session_type, SERVER_TYPE_SOCIETY);
					session->SetONConnectHandler(on_society_server_connect);
					session->SetONDisConnectHandler(on_society_server_disconnect);
				}
			}
			break;
		case NST_GAMESERVER_TO_GLOBALSERVER:
			{
				session = T_NEW_D NW_ServerSession;
				if (session)
				{
					session->GetServerInfo() = server_info;
					session->Init(session_type, SERVER_TYPE_GLOBAL);
					session->SetONConnectHandler(on_global_server_connect);
					session->SetONDisConnectHandler(on_global_server_disconnect);
				}
			}
			break;
		case NST_GAMESERVER_TO_GAMESERVER:
			{
				session = T_NEW_D NW_SIOSession;
				if (session)
				{
					session->GetServerInfo() = server_info;
					session->Init(session_type, SERVER_TYPE_GAME);
					session->SetONConnectHandler(on_game_server_connect);
					session->SetONDisConnectHandler(on_game_server_disconnect);
				}
			}
			break;
		default:
			break;
	}
	return session;
}
#endif

void DestoryServerSession(NW_ServerSession* session)
{
	T_SAFE_DELETE(session);
}
