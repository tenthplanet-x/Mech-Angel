#include "ClientPCH.h"

#include "LC_ClientGamePlayer.h"

#include "HL_FromGameServer.h"

#include "SM_GameStageMachine.h"

#include "NW_Game2ClientSession.h"
#include "NW_ClientSessionManager.h"
#include "NW_Helper.h"

using namespace StateMachine;
using namespace Script;
using namespace Utility;
using namespace Protocol;
//-----------------------------------------------------
NW_Game2ClientSession::NW_Game2ClientSession()
: NW_NetSessionBase()
, m_bAutoReconnect(false)
, m_pkPlayer(NULL)
{
	m_eSessionType = NST_CLIENT_TO_GAMESERVER;
}
//-----------------------------------------------------
NW_Game2ClientSession::~NW_Game2ClientSession()
{
}
//-----------------------------------------------------
void NW_Game2ClientSession::OnConnect( BOOL bSuccess, DWORD dwNetworkIndex )
{
	NW_NetSessionBase::OnConnect(bSuccess,dwNetworkIndex);

	NW_ClientSessionManager* pkSessionManager = NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup* pkSessionGroup = pkSessionManager->GetSessionGroup(this);
	if(!pkSessionGroup)
	{
		GfxWriteLog(LOG_TAG_ERROR,LOG_SWITCH_ERROR,"找不到SessionGroup,无法拿到签名\n");
		return;
	}

	if(!bSuccess)
	{
		m_bAutoReconnect = false;
		Utility::GfxWriteLog(LOG_TAG_ERROR, Utility::LOG_SWITCH_ERROR,"[网络][收]无法连接 GameServer");
		Clear();
		pkSessionGroup->SetNetState(NLS_FAIL_GAME_SERVER);
	}
	else
	{
		Utility::GfxWriteLog(LOG_TAG_SYSTEM, Utility::LOG_SWITCH_INFO, "[网络][收]成功连接 GameServer");
		if(pkSessionGroup->GetNetState() != NLS_WAIT_CHANGE_SERVER)
			pkSessionGroup->SetNetState(NLS_NONE);

		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = false;

		MG_Signature kMsgSignature;
		kMsgSignature.m_sBuffLength = pkSessionGroup->m_sSignatureBuffLength;
		kMsgSignature.m_iClientVersion = 0;
		kMsgSignature.m_uiUserID = pkSessionGroup->m_dwUserID;
		kMsgSignature.m_nLoginPlatform = getYunYingplatform();
		kMsgSignature.m_nLoginPlatformPage = getYunYingPlatformPage();

		memcpy(kMsgSignature.m_szSigBuff,pkSessionGroup->m_szSigatureBuff,pkSessionGroup->m_sSignatureBuffLength);

		bCreateMsg = CreateMsgPacketHandler(pkSessionGroup, CRYPT_ALGORITHM_NONE, hMsgHandler, MGPT_SIGNATURE, &kMsgSignature);
		if(bCreateMsg)
			Send(hMsgHandler.GetBuff(),hMsgHandler.GetMsgSize());
	}
}
//-----------------------------------------------------
void NW_Game2ClientSession::OnDisconnect()
{
	_setConnected(FALSE);
	m_bIsTriedToConnect = FALSE;

    NW_ClientSessionManager* pkSessionManager = NW_ClientSessionManager::GetSingletonPtr();

    pkSessionManager->RemoveSession(GetSessionIndex());

	NW_ClientSessionGroup*   pkSessionGroup = pkSessionManager->GetSessionGroup(this);
	if(pkSessionGroup->m_pkGameStageMachine->GetChangeServerFlag())
	{
		if (m_pkPlayer)
			m_pkPlayer->OnDisconnect(); 

		pkSessionGroup->SetNetState(NLS_WAIT_CHANGE_SERVER);
	}
	else
	{
		if (!pkSessionGroup->m_bRequestExitGame)
		{
			if (m_pkPlayer)
				m_pkPlayer->OnDisconnect(); 

			pkSessionManager->ResetSessionGroup(this);
		}
		/*if (_isConnector() && m_bAutoReconnect)
		{
			TryToConnect();
		}*/
	}


	Utility::GfxWriteLog(LOG_TAG_SYSTEM, Utility::LOG_SWITCH_INFO, "[网络] 与 GameServer 断开连接");
}
//-----------------------------------------------------
void NW_Game2ClientSession::OnRecv( char* pMsg, uint16_t wSize )
{
	HL_FromGameServer::ParsePacket(this,pMsg,wSize);
}
//-----------------------------------------------------
void NW_Game2ClientSession::SetAutoReconnect(bool bAutoReconnect /*= true*/)
{
	m_bAutoReconnect = bAutoReconnect;
}
//-----------------------------------------------------
void NW_Game2ClientSession::SetPlayer( LC_ClientGamePlayer* pkPlayer )
{
	m_pkPlayer = pkPlayer;
}
//-----------------------------------------------------
LC_ClientGamePlayer* NW_Game2ClientSession::GetPlayer()
{
	return m_pkPlayer;
}
