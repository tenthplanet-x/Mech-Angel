#include "ClientPCH.h"
#include "SS_TempSession.h"
#include "SS_ClientSessionFactory.h"
#include "SS_ClientSessionManager.h"
#include "SS_DebuggerRemoteSession.h"

using namespace Utility;
using namespace Session;
using namespace Protocol;
//---------------------------------------------------
SS_TempSession::SS_TempSession()
: SS_SessionBase()
{
	m_eSessionType = ST_TEMP_CLIENT;
}
//---------------------------------------------------
SS_TempSession::~SS_TempSession()
{

}
//---------------------------------------------------
VOID SS_TempSession::OnRecv( BYTE *pMsg, WORD wSize )
{
	switch (MG_PACKET::GetMsgType(pMsg))
	{
	case MGPT_SESSION_TYPE:
		{
			MG_SessionType kMsg;
			if (kMsg.Decompress(pMsg,wSize))
			{
				SessionType eType = (SessionType)(kMsg.m_bSessionType);
				SessionType eNewSessionType;

				switch(eType)
				{
				case ST_DEBUGGER_CLIENT:
					eNewSessionType = ST_DEBUGGER_CLIENT;
					GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"接受到调试器");
					break;
				default:
					T_ASSERT(false);
					break;
				}

				SS_SessionBase * pNewSession = SS_SessionFactoryBase::GetSingletonPtr()->AllocSession( eNewSessionType );
				T_ASSERT(pNewSession);

				pNewSession->Init();

				// 转交权限
				Redirect(pNewSession);

				//if (eType == ST_DEBUGGER_CLIENT)
				//{
				//	SS_ClientSessionManager::GetSingletonPtr()->SetDebugSession((SS_DebuggerRemoteSession*)pNewSession);
				//}

				//SS_ClientSessionManager::GetSingletonPtr()->RemoveSession(m_dwSessionIndex);
				SS_ClientSessionManager::GetSingletonPtr()->RemoveSession(m_dwSessionIndex);
				SS_ClientSessionManager::GetSingletonPtr()->AddSession(pNewSession);


			}


		}
		break;
	default:
		T_ASSERT(false);
		break;
	}
}
//---------------------------------------------------
VOID SS_TempSession::OnConnect( BOOL bSuccess, DWORD dwNetworkIndex )
{
	//T_ASSERT(false && "不应该调用该接口")
	SS_SessionBase::OnConnect(bSuccess,dwNetworkIndex);
}
