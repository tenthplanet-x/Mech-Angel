#include "ClientPCH.h"

#include "HL_FromLoginServer.h"

#include "NW_ClientSessionManager.h"
#include "NW_Login2ClientSession.h"
#include "NW_Helper.h"

#include "SM_GameStageMachine.h"
#include "UT_LogicInfoMessage.h"

#include "MG_Crypt_XTEA.h"
#include "MG_Stream.h"

#include <Iphlpapi.h>

using namespace Protocol;
using namespace Utility;
using namespace NetWork;
using namespace StateMachine;

int64_t Get_MAC_Address(void)
{
	IP_ADAPTER_INFO AdapterInfo[32];       // Allocate information for up to 32 NICs   
	DWORD dwBufLen = sizeof(AdapterInfo);  // Save memory size of buffer   
	DWORD dwStatus = GetAdaptersInfo(AdapterInfo, &dwBufLen);
	  
	if(dwStatus != ERROR_SUCCESS)
	{
		return 0;
	}

	PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;

	int64_t llMAC = 0;

	while(pAdapterInfo)    
	{        
		if(MIB_IF_TYPE_ETHERNET == pAdapterInfo->Type && strstr(pAdapterInfo->Description,"PCI") >0 )      
		{	
			int64_t lad0 = pAdapterInfo->Address[0];
			int64_t lad1 = pAdapterInfo->Address[1];
			int64_t lad2 = pAdapterInfo->Address[2];
			int64_t lad3 = pAdapterInfo->Address[3];
			int64_t lad4 = pAdapterInfo->Address[4];
			int64_t lad5 = pAdapterInfo->Address[5];
			
			llMAC = (lad0 << 40) + (lad1 << 32) + (lad2 << 24) + (lad3 << 16) + (lad4 << 8) + lad5;
		}

		if (0 != llMAC)
		{
			break;
		}

		pAdapterInfo = pAdapterInfo->Next;  
	}

	return llMAC;
}

//------------------------------------------------------------------------------
NW_Login2ClientSession::NW_Login2ClientSession()
: NW_Game2ClientSession()//NW_NetSessionBase()
{
	m_eSessionType = NST_CLIENT_TO_LOGINSERVER;
	SetHeartBeatFrequency(15);
}
//------------------------------------------------------------------------------
NW_Login2ClientSession::~NW_Login2ClientSession()
{
}
//------------------------------------------------------------------------------

static char szT6SessionKey[16] = {'T','6','@','N','e','w','c','o','.','c','o','m','C','M','M','#'};

//#define		CURRET_PLATFORM			"360wan"

void NW_Login2ClientSession::OnConnect( BOOL bSuccess, DWORD dwNetworkIndex )
{
	commitPatchProgress(EVENT_CLIENT_ON_CONNECT, (int)bSuccess);

	NW_NetSessionBase::OnConnect(bSuccess,dwNetworkIndex,false);

	NW_ClientSessionManager* pkSessionManager = NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup* pkSessionGroup = pkSessionManager->GetSessionGroup(this);
	if(!pkSessionGroup)
	{
		GfxWriteLog(LOG_TAG_ERROR,LOG_SWITCH_ERROR,"[网络][连]找不到SessionGroup,无法拿到签名\n");
		return;
	}

	if(!bSuccess)
	{
		//Clear();
		pkSessionGroup->SetNetState(NLS_FAIL_LOGIN);
		Utility::GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM,"[网络][连]无法连接LoginServer");
	}
	else
	{
		Utility::GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM,"[网络][连]成功连接LoginServer");
		//打点: 用户验证登录成功

		MG_Signature kMsgSignature;
		StringType sUserId = ClientCmdlineArg::GetValueSafe(CMD_ARG_UID);
		StringType sIntUId = ClientCmdlineArg::GetValueSafe(CMD_ARG_UID32);
		StringType sSvrId  = ClientCmdlineArg::GetValueSafe(CMD_ARG_SID);
		StringType sPlatformName  = ClientCmdlineArg::GetValueSafe(CMD_ARG_PLATFORM);
		// 20200417 360 login string simulator
		// put the following string to Command Argument: 
		// platform=360_2020 sid=6 uid=9527
		if( !sUserId.empty() && !sIntUId.empty() )
		{
			//if ( sPlatformName != CURRET_PLATFORM )
			//{
			//	//sUserId += "_" + sSvrId;
			//	Clear();
			//	//pkSessionGroup->m_kSignatureInfo.m_uiUserID = find_iter->second;
			//	char sig_buff[Protocol::SESSION_KEY_SIZE] = {0};
			//	memcpy(pkSessionGroup->m_kSignatureInfo.m_SessionKey,sig_buff, Protocol::SESSION_KEY_SIZE);

			//	pkSessionGroup->m_kSignatureInfo.m_uiValidTime = (uint32_t(time(NULL))+30);
			//	char ext_buff[EXTINFO_SIZE]={0};
			//	memcpy(pkSessionGroup->m_kSignatureInfo.m_ExtInfo, ext_buff, EXTINFO_SIZE);

			//	TPSTR2STDSTR(sUserId, pkSessionGroup->m_kSignatureInfo.m_szAccount);
			//	TPSTR2STDSTR(ClientCmdlineArg::GetValueSafe(CMD_ARG_SIGN), pkSessionGroup->m_kSignatureInfo.m_Signature);

			//	kMsgSignature.m_uiUserID = 0;

			//	if(!sSvrId.empty())
			//	{
			//		pkSessionGroup->SetCurCitizenShip(atoi(sSvrId.c_str()));
			//	}


			//	pkSessionGroup->SetNetState(NLS_WAIT_SIGNATURE);
			//	kMsgSignature.m_sLoginMode = LOGINMODE_ONLINE;

			//	MG_OUTPUT_CDR kOutStream(pkSessionGroup->m_szSigatureBuff,  sizeof(pkSessionGroup->m_szSigatureBuff));
			//	pkSessionGroup->m_kSignatureInfo.Compress(&kOutStream); 
			//	pkSessionGroup->m_sSignatureBuffLength	= kOutStream.GetMsgLength();
			//	pkSessionGroup->m_dwUserID = 0;
			//}
			//else 
			{
				//sUserId += "_" + sSvrId;
				Clear();
				//pkSessionGroup->m_kSignatureInfo.m_uiUserID = find_iter->second;
				char sig_buff[Protocol::SESSION_KEY_SIZE] = {0};
				memcpy(pkSessionGroup->m_kSignatureInfo.m_SessionKey,sig_buff, Protocol::SESSION_KEY_SIZE);

				pkSessionGroup->m_kSignatureInfo.m_uiValidTime = (uint32_t(time(NULL))+30);
				char ext_buff[EXTINFO_SIZE]={0};
				memcpy(pkSessionGroup->m_kSignatureInfo.m_ExtInfo, ext_buff, EXTINFO_SIZE);

				TPSTR2STDSTR(sUserId, pkSessionGroup->m_kSignatureInfo.m_szAccount);
				TPSTR2STDSTR(ClientCmdlineArg::GetValueSafe(CMD_ARG_SIGN), pkSessionGroup->m_kSignatureInfo.m_Signature);

				uint32_t nUserId = UT_BaseStringUtility::toUlong(sIntUId);
				//kMsgSignature.m_uiUserID = 0;
				kMsgSignature.m_uiUserID = nUserId;
				pkSessionGroup->m_kSignatureInfo.m_uiUserID = nUserId;
				pkSessionGroup->m_dwUserID = nUserId;
				if(!sSvrId.empty())
				{
					pkSessionGroup->SetCurCitizenShip(atoi(sSvrId.c_str()));
				}


				pkSessionGroup->SetNetState(NLS_WAIT_SIGNATURE);
				kMsgSignature.m_sLoginMode = LOGINMODE_360;

				MG_OUTPUT_CDR kOutStream(pkSessionGroup->m_szSigatureBuff,  sizeof(pkSessionGroup->m_szSigatureBuff));
				pkSessionGroup->m_kSignatureInfo.Compress(&kOutStream); 
				pkSessionGroup->m_sSignatureBuffLength	= kOutStream.GetMsgLength();
				//pkSessionGroup->m_dwUserID = 0;
			}
			
		}
		else
		{
			//STRNCPY(pkSessionGroup->m_kSignatureInfo.m_szAccount,sUserId.c_str(), sizeof(pkSessionGroup->m_kSignatureInfo.m_szAccount));
			pkSessionGroup->SetNetState(NLS_WAIT_SERVER_LIST);
			kMsgSignature.m_uiUserID = pkSessionGroup->m_dwUserID;
			kMsgSignature.m_sLoginMode = LOGINMODE_NORMAL;
		}


		kMsgSignature.m_nLoginPlatform = getYunYingplatform();
		kMsgSignature.m_nLoginPlatformPage = getYunYingPlatformPage();

		kMsgSignature.m_sBuffLength = pkSessionGroup->m_sSignatureBuffLength;
		kMsgSignature.m_iClientVersion = 0;
		memcpy(kMsgSignature.m_szSigBuff,pkSessionGroup->m_szSigatureBuff,pkSessionGroup->m_sSignatureBuffLength);

		//获取Mac地址
		kMsgSignature.m_ulClientMAC = Get_MAC_Address();
		
		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = false;
		bCreateMsg = CreateMsgPacketHandler(pkSessionGroup, CRYPT_ALGORITHM_NONE, hMsgHandler, MGPT_SIGNATURE, &kMsgSignature);
		Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

		commitPatchProgress(EVENT_CLIENT_REQ_SIGNATURE, 0);
		Utility::GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM,"[网络][发][%s]发送MGPT_SIGNATURE请求, 连接GameServer id: %d, %d", Int642String(kMsgSignature.m_uiUserID).c_str(), pkSessionGroup->GetCurCitizenShip(), MGPT_SIGNATURE);
		ResetUiSerial();
	}
}
//------------------------------------------------------------------------------
void NW_Login2ClientSession::OnRecv( char *pMsg, uint16_t wSize )
{
	HL_FromLoginServer::ParsePacket(this, pMsg, wSize);
}

//------------------------------------------------------------------------------
void NW_Login2ClientSession::OnDisconnect()
{
	NW_ClientSessionManager* pkSessionManager = NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup* pkSessionGroup = pkSessionManager->GetSessionGroup(this);
	pkSessionGroup->SetCanCalOnlineTime(false);

	NW_NetSessionManager::GetSingletonPtr()->RemoveSession( GetSessionIndex() );

	_setConnected(FALSE);
    m_bIsTriedToConnect = FALSE;
	
	int iState = pkSessionGroup->m_pkGameStageMachine->GetCurrentStateType();
	if (iState == ST_GAME_STAGE_GAME || iState == ST_GAME_STAGE_LOADING)
	{
		int iErrCode = NetBase::GetOnDisconnectFuncErrorCode();
		GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM, "NW_Login2ClientSession::OnDisconnect errorCode = [%d]", iErrCode);
		NetBase::CallOnDisconnectFunc(iErrCode);
	}
	
	GfxWriteLog(LOG_TAG_SYSTEM,LOG_SWITCH_SYSTEM,"与LoginServer断开连接\n");
}
//------------------------------------------------------------------------------
void NW_Login2ClientSession::KeepAlive(float fCurrentTime)
{
	if(fCurrentTime - m_fLastHeartbeatTime > float(m_iHeartBeatFrequency))
	{
		m_fLastHeartbeatTime = fCurrentTime;

		NW_ClientSessionManager* pkSessionManager = NW_ClientSessionManager::GetSingletonPtr();
		NW_ClientSessionGroup* pkSessionGroup = pkSessionManager->GetSessionGroup(this);
		if(!pkSessionGroup)
		{
			GfxWriteLog(LOG_TAG_ERROR,LOG_SWITCH_ERROR,"找不到SessionGroup,无法拿到签名\n");
			return;
		}

		if(IsOnIdle())
		{
			GfxWriteLog(Utility::LOG_TAG_SYSTEM, Utility::LOG_SWITCH_SYSTEM, "客户端超时3分钟，掉线！！！！！！！！\n");
			Disconnect(TRUE);
			return;
		}

		MG_PACKET_ENCODE_HANDLER kHandler;
		bool bCreateMsg = CreateMsgPacketHandler(pkSessionGroup, CRYPT_ALGORITHM_NONE, kHandler, MGPT_SYNC_HEART_BEAT, NULL);
		if( bCreateMsg )
		{
			BOOL bSend = Send(kHandler.GetBuff(), kHandler.GetMsgSize());
			GfxWriteLog(LOG_TAG_GAME_LOG, LOG_SWITCH_GAME,"向LoginServer发送心跳包 %d\n", bSend);
		}
		else
		{
			GfxWriteLog(LOG_TAG_ERROR,LOG_SWITCH_ERROR,"无法向LoginServer发送心跳包\n");
		}
	}
}
//------------------------------------------------------------------------------
static uint32_t get_local_user_id_help(const StringType& userName, const StringType& preFix, uint32_t idSize, uint32_t startId)
{
	if (userName.substr(0, preFix.size()) != preFix || (userName.size() != (preFix.size()+idSize)))
	{
		return 0;
	}
	uint32_t id = (uint32_t)Utility::String2Int(userName.substr(preFix.size(), idSize));
	return id > 0 ? startId + id : 0;
}

static uint32_t get_local_user_id(const StringType& userName)
{
	if (get_local_user_id_help(userName, "dev", 3, 9999999) > 0)
	{
		return get_local_user_id_help(userName, "dev", 3, 9999999);
	}
	else if (get_local_user_id_help(userName, "mk", 3, 9999999+999) > 0)
	{
		return get_local_user_id_help(userName, "mk", 3, 9999999+999);
	}
	else if (get_local_user_id_help(userName, "jz", 3, 9999999+999*2) > 0)
	{
		return get_local_user_id_help(userName, "jz", 3, 9999999+999*2);
	}
	else if (get_local_user_id_help(userName, "mktest", 4, 9999999+999*3) > 0)
	{
		return get_local_user_id_help(userName, "mktest", 4, 9999999+999*3);
	}
	else if (get_local_user_id_help(userName, "debug", 6, 0) > 0)
	{
		return get_local_user_id_help(userName, "debug", 6, 0);
	}
	return 0;
}

void NW_Login2ClientSession::FakeAuth()
{
	NW_ClientSessionManager* pkSessionManager = NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup* pkSessionGroup = pkSessionManager->GetSessionGroup(this);

	uint32_t userID = get_local_user_id(pkSessionGroup->m_strUser);

	if (userID <= 0)
	{
		SendLogicInfoMessage(RE_LOGIN_INVALIDATING_USER);
		return;
	}
	pkSessionGroup->m_kSignatureInfo.m_uiUserID		= userID;
	TPSTR2STDSTR(pkSessionGroup->m_strUser, pkSessionGroup->m_kSignatureInfo.m_szAccount);
	char sig_buff[Protocol::SESSION_KEY_SIZE] = {0};
	memcpy(pkSessionGroup->m_kSignatureInfo.m_SessionKey,sig_buff, Protocol::SESSION_KEY_SIZE);
	pkSessionGroup->m_kSignatureInfo.m_uiValidTime = (uint32_t(time(NULL))+30);
	char ext_buff[EXTINFO_SIZE]={0};
	memcpy(pkSessionGroup->m_kSignatureInfo.m_ExtInfo, ext_buff, EXTINFO_SIZE);

	MG_OUTPUT_CDR kOutStream(pkSessionGroup->m_szSigatureBuff,  sizeof(pkSessionGroup->m_szSigatureBuff));

	pkSessionGroup->m_kSignatureInfo.Compress(&kOutStream); 

	pkSessionGroup->m_sSignatureBuffLength	= kOutStream.GetMsgLength();

	pkSessionGroup->m_dwUserID				= userID;

	//防沉迷
	//pkSessionGroup->SetAdult(true);
	//pkSessionGroup->SetCanCalOnlineTime(true);

	//进入选择服务器界面
	pkSessionGroup->SetNetState(NLS_NONE);
	pkSessionGroup->m_pkGameStageMachine->SetNextState(ST_GAME_STAGE_SEL_SERVER);
	pkSessionGroup->m_pkGameStageMachine->Update(0.0f,0.0f);
}