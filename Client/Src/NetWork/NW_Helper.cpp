
#include "ClientPCH.h"
#include "LM_LocaleManager.h"
#include "NW_Helper.h"
#include "NW_ClientSessionManager.h"

#include <Iphlpapi.h>


using namespace Protocol;
using namespace Utility;
using namespace NetWork;
//------------------------------------------------------------------------
bool  CreateMsgPacketHandler(NW_ClientSessionGroup* pkSessionGroup, uint16_t usFlag, MG_PACKET_ENCODE_HANDLER& rkMsgPacket, uint16_t usMsgType, MG_BODY* pBody)
{
	CS_MG_HEAD kHead;
	kHead.m_usFlag		= usFlag;
	kHead.m_usMsgType	= usMsgType;
	kHead.m_uiUserID	= pkSessionGroup->GetUserID();
	//SIGNATURE不加密
	if(usMsgType == MGPT_SIGNATURE)
	{
		kHead.SetCryptAlgorithm(CRYPT_ALGORITHM_NONE);
	}
	else
	{
		kHead.SetCryptAlgorithm(CRYPT_ALGORITHM_XTEA_ROUND8);
	}
	
	UpdateUiSerial();
	kHead.m_uiSerial = uiSerial;
	
	bool bCreateMsg = false;	
	char szSessionKey[SESSION_KEY_SIZE];
	get_session_key(szSessionKey, SESSION_KEY_SIZE, uiSerial);
	bCreateMsg = rkMsgPacket.EncodeCSMsg(szSessionKey, kHead, pBody, COMPRESS_ALGORITHM_ZIP, true);

	if(bCreateMsg)
	{	
		return true;
	}
	else
	{
		GfxWriteLog(LOG_TAG_ERROR,LOG_SWITCH_ERROR,"Msg encode error|%d\n", usMsgType);
		return false;
	}
}
//------------------------------------------------------------------------
bool  CreateMsgPacketHandler(NW_NetSessionBase* pkSession, uint16_t usFlag, MG_PACKET_ENCODE_HANDLER& rkMsgPacket, uint16_t usMsgType, MG_BODY* pBody)
{
	NW_ClientSessionManager* pkSessionManager = NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup* pkSessionGroup = pkSessionManager->GetSessionGroup(pkSession);
	if(!pkSessionGroup)
	{
		GfxWriteLog(LOG_TAG_ERROR,LOG_SWITCH_ERROR,"找不到SessionGroup,无法组织消息\n");
		return false;
	}

	return CreateMsgPacketHandler(pkSessionGroup, usFlag, rkMsgPacket, usMsgType, pBody);
}
//------------------------------------------------------------------------
bool  CreateMsgPacketHandler(MG_PACKET_ENCODE_HANDLER& rkMsgPacket, uint16_t usMsgType, MG_BODY* pBody)
{
	CS_MG_HEAD kHead;
	kHead.m_usMsgType = usMsgType;

	NW_ClientSessionManager* pkSessionManager = NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup* pkSessionGroup = pkSessionManager->GetGameSessionGroup();
	if(NULL != pkSessionGroup)
	{
		kHead.m_uiUserID = pkSessionGroup->GetUserID();
	}

	UpdateUiSerial();
	kHead.m_uiSerial = uiSerial;

	if(rkMsgPacket.EncodeCSMsg(kHead, pBody, COMPRESS_ALGORITHM_ZIP, true, CRYPT_ALGORITHM_XTEA_ROUND8))
	{	
		return true;
	}
	else
	{
		GfxWriteLog(LOG_TAG_ERROR,LOG_SWITCH_ERROR,"Msg encode error|%d\n", usMsgType);
		return false;
	}
}
#ifndef _USE_SERVER
bool  CreateSCMsgPacketHandler(MG_PACKET_ENCODE_HANDLER& rkMsgPacket, uint16_t usMsgType, MG_BODY* pBody)
{
	CS_MG_HEAD kHead;
	kHead.m_usMsgType	= usMsgType;

	if(rkMsgPacket.EncodeCSMsg(kHead, pBody, COMPRESS_ALGORITHM_NONE, true))
	{	
		return true;
	}
	else
	{
		GfxWriteLog(LOG_TAG_ERROR,LOG_SWITCH_ERROR,"Msg encode error|%d\n", usMsgType);
		return false;
	}
}


#endif


bool Get_All_MAC_Address( char* pMacBuff, uint16_t sBuffSize )
{
	if (NULL == pMacBuff || 200 > sBuffSize)
	{
		return false;
	}

	memset(pMacBuff, 0, sBuffSize);

	IP_ADAPTER_INFO AdapterInfo[32];       // Allocate information for up to 32 NICs   
	DWORD dwBufLen = sizeof(AdapterInfo);  // Save memory size of buffer   
	DWORD dwStatus = GetAdaptersInfo(AdapterInfo, &dwBufLen);

	if(dwStatus != ERROR_SUCCESS)
	{
		return false;
	}

	PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
	char* pBuff = pMacBuff;
	int Count = 0;

	while(pAdapterInfo)    
	{        
		++Count;
		if (8 < Count)
		{
			break;
		}

		sprintf_s(pBuff, 19, "%02X-%02X-%02X-%02X-%02X-%02X;",
			pAdapterInfo->Address[0],
			pAdapterInfo->Address[1],
			pAdapterInfo->Address[2],
			pAdapterInfo->Address[3],
			pAdapterInfo->Address[4],
			pAdapterInfo->Address[5]);

		pBuff += 18;

		pAdapterInfo = pAdapterInfo->Next;  
	}

	return true;
}

int GetUserProfileIntVal(const StringType& rkValName)
{
	Utility::UT_ConfigFile kConfig;
	if ( kConfig.Load(CLIENT_USER_PROFILE) )
		return kConfig.GetIntSetting(rkValName.c_str(), "Client", 1);

	return 0;
}

void SetUserProfileIntVal(const StringType& rkValName,int iVal)
{
	StringStreamType kString;
	kString << iVal;
	::WritePrivateProfileString( "Client" , rkValName.c_str() , kString.str().c_str() , CLIENT_USER_PROFILE );
}

//------------------------------------------------------------------------
StringType GetIPAddress(long lIP)
{
	return GET_MSG(LC_CLIENTSOCIETYASSET_CPP_001);
}

int64_t GetUserProfileInt64Val(const StringType& rkValName)
{
	Utility::UT_ConfigFile kConfig;
	if ( kConfig.Load(CLIENT_USER_PROFILE) )
		return kConfig.GetInt64Setting(rkValName.c_str(), "Client", 0);

	return 0;
}

void SetUserProfileInt64Val(const StringType& rkValName,int64_t iVal)
{
	StringStreamType kString;
	kString << iVal;
	::WritePrivateProfileString( "Client" , rkValName.c_str() , kString.str().c_str() , CLIENT_USER_PROFILE );
}

void UpdateUiSerial(void)
{
	if(uiSerial < MSG_CS_SERIAL_CARRY)
		++uiSerial;
	else
		ResetUiSerial();
}

void ResetUiSerial(void)
{
	uiSerial = 1;
}
