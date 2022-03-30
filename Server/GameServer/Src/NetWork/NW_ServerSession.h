
#ifndef _NW_SERVER_SESSION_H
#define _NW_SERVER_SESSION_H

#include "SystemInclude.h"
#include "simple_io.hpp"
#include <boost/serialization/singleton.hpp>

#include "NW_NetSessionBase.h"
#include "NW_MessageRecorder.h"
#include "MG_ProtocolStruct.h"
#include "RecvBuffer.h"
#include "MG_TYPES.h"

#include "UT_Log.h"

class NW_ServerSession;

typedef void (* on_connect_handler)( NW_ServerSession * session);

class NW_ServerSession : public NetWork::NW_NetSessionBase
{
public:
	NW_ServerSession();
	virtual ~NW_ServerSession();
#ifndef ACTIVATION_CODE
	virtual BOOL	Send( char *pMsg, WORD wSize);
	virtual VOID	OnRecv( char *pMsg, WORD wSize );
	virtual VOID	OnDisconnect();

	virtual VOID	OnConnect( BOOL bSuccess, DWORD dwNetworkIndex );

	inline Protocol::PS_ServerInfo&    GetServerInfo(){ return m_SelfServerInfo; }
	void SetServerInfo(const Protocol::PS_ServerInfo& rkSvrInfo);

	void Init(NetSessionType session_type , MG_SERVER_TYPES server_type);

	void SetONConnectHandler(on_connect_handler handler){ m_OnConnectHandler = handler; }
	void SetONDisConnectHandler(on_connect_handler handler){ m_OnDisConnectHandler = handler; }
	inline void SetMessageRecorder(NW_MessageRecorder* messageRecorder){m_messageRecorder = messageRecorder;}
#else
	virtual VOID	OnRecv( char *pMsg, WORD wSize ) {}
#endif

protected:
	Protocol::PS_ServerInfo m_SelfServerInfo;
	on_connect_handler      m_OnConnectHandler;
	on_connect_handler      m_OnDisConnectHandler;

	NW_MessageRecorder* m_messageRecorder;
	Protocol::MG_PACKET_DECODE_HANDLER m_decodeHandler;
	Protocol::SS_MG_HEAD m_ssMessageHead;
};

typedef mem::map<unique_id_type, NW_ServerSession*>	 NW_ServerSessionMap;

class NW_SIOSession;
struct server_io_session : public sio::io_session<server_io_session>
{
	server_io_session(boost::asio::io_service& io_service, uint32_t recv_buf_sz, uint32_t send_buf_sz, uint32_t max_pack_sz)
		: sio::io_session<server_io_session>(io_service, recv_buf_sz, send_buf_sz, max_pack_sz)
		, parent(NULL)
	{
		//GfxWriteFmtLog(LOG_INFO_FILELINE, Utility::LOG_SWITCH_INFO, "server_io_session constuct", *this);
	}
	virtual ~server_io_session(void)
	{
		//GfxWriteFmtLog(LOG_INFO_FILELINE, Utility::LOG_SWITCH_INFO, "server_io_session destruct", *this);
	}

	virtual bool recv(void);
	virtual void on_opened(void);
	virtual void on_closed(void);

	inline NW_SIOSession* Session(void){return parent;}
	NW_SIOSession* parent;
};

class NW_SIOSession : public NW_ServerSession
{
public:
	NW_SIOSession(void):serial(0)
	{}
	virtual ~NW_SIOSession(void){};

	//设置连接IP
	virtual void	TryToConnect(void);
	virtual	VOID	Disconnect( BOOL bGracefulDisconnect = TRUE );

	virtual VOID	OnDisconnect();
	virtual VOID	OnConnect( BOOL bSuccess, DWORD dwNetworkIndex );

	virtual BOOL	Send(char *pMsg, WORD wSize);

	sio::io_service_manager<server_io_session>::io_session_ptr sio_session_;
	int serial;
};

class NW_FakeSIOSession : public NW_ServerSession
{
public:
	NW_FakeSIOSession(uint32_t buf_sz, uint32_t max_pack_sz)
	{
		local_buf_.create(buf_sz, max_pack_sz);
	}

	//设置连接IP
	virtual void	TryToConnect(void);
	virtual	VOID	Disconnect( BOOL bGracefulDisconnect = TRUE );

	virtual VOID	OnDisconnect();
	virtual VOID	OnConnect( BOOL bSuccess, DWORD dwNetworkIndex );

	virtual BOOL	Send(char *pMsg, WORD wSize);
	BOOL			Recv(uint16_t max_pack_sz_);
protected:
	virtual char*	peek_packet(uint16_t& pack_size);
	sio::circular_buffer<char>  local_buf_;
};

class NW_SIOSessionManager : public sio::io_service_manager<server_io_session>
	, public Utility::UT_Singleton<NW_SIOSessionManager>
{
public:
	NW_SIOSessionManager(std::size_t io_service_pool_size
		, bool check_internal=true, uint32_t recv_buf_size=0x7FFFF, uint32_t send_buf_size=0x7FFFF, uint32_t max_pack_size=0xFFFF)
		: sio::io_service_manager<server_io_session>(io_service_pool_size, check_internal, recv_buf_size, send_buf_size, max_pack_size)
		, local_(recv_buf_size, max_pack_size)
	{}

	void SetLocalInfo(const Protocol::PS_ServerInfo& psSvrInfo);

	virtual void update(void)
	{
		local_.Recv(max_pack_sz_);
		sio::io_service_manager<server_io_session>::update();
	}
protected:
	NW_FakeSIOSession local_;
};


#ifndef ACTIVATION_CODE
void SendServerInfo(MG_SERVER_TYPES servertype, int id=0);

NW_ServerSession * CreateServerSession(NetSessionType session_type , const Protocol::PS_ServerInfo& server_info);
#endif
void DestoryServerSession(NW_ServerSession * session);
#endif
