#include "ClientPCH.h"

#include "UT_ClientLogRemoteConsole.h"
#include "NW_ClientSessionManager.h"
#include "NW_Helper.h"

using namespace Utility;
using namespace NetWork;
using namespace Protocol;

UT_ClientLogRemoteConsole::UT_ClientLogRemoteConsole(void)
{
}

UT_ClientLogRemoteConsole::~UT_ClientLogRemoteConsole(void)
{
}

void UT_ClientLogRemoteConsole::LogMessage( const StringType& rkMessage )
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	UT_AutoLock kLock(&m_kLock);

	NW_ClientSessionManager* pkManager = (NW_ClientSessionManager*)(NW_ClientSessionManager::GetSingletonPtr());
	if (0 == pkManager)
	{
		return;
	}

	NW_NetSessionBase* pkSession = pkManager->GetDebugSession();
	if (0 == pkSession)
	{
		return;
	}

	MG_RLT_DebugCommandLine msg;
	strcpy_s(msg.m_strMsg, MAX_DEBUG_MSG_LENGTH, rkMessage.c_str());
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_RLT_DEBUG_COMMANDLINE, &msg);

	pkSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
}