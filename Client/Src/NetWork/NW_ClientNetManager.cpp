#include "ClientPCH.h"

#include "NW_ClientNetManager.h"
#include "NW_ClientSessionManager.h"
#include "NW_ClientSessionFactory.h"

#include "MG_MessageBase.h"

#include "UT_LogicInfoMessage.h"

using namespace NetWork;
using namespace NetBase;
using namespace Utility;

// ���� Client Listener
NetworkObject* CreateClientSideAcceptedObject()
{
	return (NetworkObject*)NW_ClientSessionFactory::GetSingleton().AllocSession(NST_TEMP);
}

VOID DestroyClientSideAcceptedObject(NetworkObject* pNetworkObject)
{
    NW_NetSessionBase* pkSession = (NW_NetSessionBase*)pNetworkObject;
    if (pkSession != NULL)
    {
        NW_ClientSessionManager::GetSingleton().RemoveSession(pkSession->GetSessionIndex());
    }
}

VOID DestroyClientSideConnectedObject( NetworkObject * pNetworkObject )
{
	T_ASSERT(false && "���õ���");
}

NetworkObject* CreateAcceptedObject()
{
	return NULL;	
}

VOID DestroyAcceptedObject( NetworkObject *pNetworkObject )
{
}

VOID DestroyConnectedObject( NetworkObject *pNetworkObject )
{
}

NW_ClientNetManager::NW_ClientNetManager()
: NW_NetManagerBase()
, m_bCheckLogin(false)
, m_fLoginStartTime(-1.0)
{
}

NW_ClientNetManager::~NW_ClientNetManager()
{
}

bool NW_ClientNetManager::Init(void)
{
#ifndef _MAP_VIEWER_BUILD
    GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "���� IOCP");

	IOHANDLER_DESC desc[1];
// 	desc[0].dwIoHandlerKey				= CLIENT_IOHANDLER;
// 	desc[0].dwMaxConnectSession			= 0;
// 	desc[0].dwSendBufferSize			= 65536 * 2;
// 	desc[0].dwRecvBufferSize			= 65536 * 2;
// 	desc[0].dwTimeOut					= 30000;
// 	desc[0].dwMaxPacketSize				= 65536;
// 	desc[0].fnDestroyConnectedObject	= DestroyClientSideConnectedObject;
	desc[0].dwIoHandlerKey				= SERVER_IOHANDLER;
	desc[0].dwMaxConnectSession			= 3;
	desc[0].dwSendBufferSize			= 0x800000;
	desc[0].dwRecvBufferSize			= 0x800000;
	desc[0].dwTimeOut					= 180000;
	desc[0].dwMaxPacketSize			    = 0x20000;
	desc[0].fnDestroyConnectedObject	= DestroyConnectedObject;

	m_pkIOCPServer = T_NEW_E( NW_IOCPServer , Memory::MEMCATEGORY_GENERAL );
	if (!m_pkIOCPServer->Init(desc, 1))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "��ʹ���������ʧ��");
		T_ASSERT(false);
		return false;
	}
#endif

	return true;
}

void NW_ClientNetManager::UnInit()
{
	T_DELETE_E(m_pkIOCPServer, NW_IOCPServer , Memory::MEMCATEGORY_GENERAL )
}

void NW_ClientNetManager::Update()
{
    if (m_pkIOCPServer)
    {
		T_PROFILE("Update::NW_ClientNetManager::IOCPServer");

        m_pkIOCPServer->Update();
    }
}

void NW_ClientNetManager::StartCheckLogin(bool bCheck)
{
	m_bCheckLogin = bCheck;										//��������¼״̬
	if (m_bCheckLogin)
	{
		m_fLoginStartTime = GET_PROCESS_TIME;	//��¼������¼��ʱ��
	}
}


