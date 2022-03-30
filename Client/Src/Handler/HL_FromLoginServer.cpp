#include "ClientPCH.h"
#include "LM_LocaleManager.h"
#include "HL_FromLoginServer.h"

//#include "LC_ClientGamePlayer.h"
#include "LC_ClientLogicManager.h"
#include "LC_ClientGameEvent.h"

#include "SM_GameStageMachine.h"

#include "MG_MessageCenter.h"
#include "MG_MessageBase.h"

#include "UT_LogicInfoMessage.h"
#include "UT_Curl.h"

#include "NW_ClientNetManager.h"
#include "NW_ClientSessionManager.h"
#include "NW_Login2ClientSession.h"
#include "NW_Helper.h"
#include "HL_FromGameServer.h"

#include "UI_UIManager.h"
#include "AS_ClientSoundManager.h"

#include "CF_ItemList.h"
#include "CF_Helper.h"

#include "char_data.pb.h"

#include "AS_ClientSoundManager.h"
extern "C"
{
	#include <Lzma/Include/Types.h>
	#include <Lzma/Include/Lzma86Dec.h>
    #include <zlib.h>
};


using namespace StateMachine;
using namespace Utility;
using namespace Protocol;
using namespace CSVFile;
//-------------------------------------------------------

static std::string uncompressedMsg;
void HL_FromLoginServer::ParsePacket(NW_Login2ClientSession *pkClientSession, char* pMsg, uint16_t wSize )
{
    T_PROFILE("ParsePacket::HL_FromLoginServer");
	MG_PACKET_DECODE_HANDLER hMsgHandler;

	char* pOldMsg = pMsg;
	uint16_t wOldSize = wSize;
	CS_MG_HEAD kMsgHead;
	if (!hMsgHandler.DecodeCSMsg(pMsg, wSize, kMsgHead))
	{
		Utility::GfxWriteLog(LOG_TAG_ERROR, LOG_SWITCH_ERROR,"FromLoginServer hMsgHandler.DecodeCSMsg Fail");
		return;
	}

	bool bCompressed = false;
	if (kMsgHead.GetCompressAlgorithm() > 0)
	{
		//size_t tUnCompressed = 0;
		//if (SZ_OK != Lzma86_GetUnpackSize((const Byte*)pMsg, wSize, (UInt64*)&tUnCompressed))
		//{
		//	return;
		//}

		//char* pUnCompressedBuff = T_ALLOC(char, tUnCompressed, Memory::MEMCATEGORY_GENERAL);
		//if (NULL == pUnCompressedBuff)
		//{
		//	T_FREE(pUnCompressedBuff,  Memory::MEMCATEGORY_GENERAL);
		//	return;
		//}

		//size_t tCompressed = wSize;
		//if (SZ_OK != Lzma86_Decode((Byte*)pUnCompressedBuff, (SizeT *)&tUnCompressed, (const Byte*)pMsg, (SizeT *)&tCompressed))
		//{
		//	T_FREE(pUnCompressedBuff,  Memory::MEMCATEGORY_GENERAL);
		//	return;
		//}

//         size_t tUnCompressed = 1024 * 64;
//         char* pUnCompressedBuff = T_ALLOC(char, tUnCompressed, Memory::MEMCATEGORY_GENERAL);
// 		if (NULL == pUnCompressedBuff)
// 		{
// 			T_FREE(pUnCompressedBuff,  Memory::MEMCATEGORY_GENERAL);
// 			return;
// 		}
//         size_t tCompressed = wSize;
//         if (Z_OK != uncompress((Bytef*)pUnCompressedBuff, (uLongf*)&tUnCompressed, (Bytef*)pMsg, tCompressed))
// 		{
// 			T_FREE(pUnCompressedBuff,  Memory::MEMCATEGORY_GENERAL);
// 			return;
// 		}
// 
// 		pMsg = pUnCompressedBuff;
// 		wSize = (uint16_t)tUnCompressed;
// 		bCompressed = true;

		if(!Protocol::Uncompress(kMsgHead.GetCompressAlgorithm(), pMsg, wSize, uncompressedMsg))
		{
			Utility::GfxWriteLog(LOG_TAG_ERROR, LOG_SWITCH_ERROR,"FromLoginServer Protocol::Uncompress Fail");
			return;
		}
		pMsg = (char*)uncompressedMsg.c_str();
		wSize = (uint16_t)uncompressedMsg.size();
	}

	if ( (kMsgHead.m_usMsgType >= MGPT_LC_BEGIN && kMsgHead.m_usMsgType <= MGPT_LC_END)
	  )
		Utility::GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM,"[网络][收]FromLoginServer :%d", kMsgHead.m_usMsgType);

	switch (kMsgHead.m_usMsgType)
	{
	case MGPT_MESSAGE:
		_handlerMsg(pkClientSession,pMsg,wSize);
		break;
	case MGPT_LC_RLT_LOGIN:
		_handlerRltLogin(pkClientSession,pMsg,wSize,kMsgHead.m_uiUserID);
		break;
	case MGPT_LC_RLT_SERVER_LIST:
		_handlerServerList(pkClientSession, pMsg, wSize);
		break;
	case MGPT_LC_RLT_CHAR_LIST:
		_handlerRltCharList(pkClientSession,pMsg,wSize);
		break;
	case MGPT_LC_RLT_CREATE_CHAR:
		_handlerRltCreateChar(pkClientSession,pMsg,wSize);
		break;
	case MGPT_LC_RLT_DELETE_CHAR:
		_handlerRltDeleteChar(pkClientSession,pMsg,wSize);
		break;
	case MGPT_LC_RLT_RECOVER_CHAR:
		_handlerRltRecoverChar(pkClientSession,pMsg,wSize);
		break;
	case MGPT_LC_RLT_ENTER_GAMESERVER:
		_handlerRltBeforeEnterGameServer(pkClientSession,pMsg,wSize);
		break;
	case MGPT_SERVER_BUSY:
		_handlerRltServerBusy(pkClientSession,pMsg,wSize);
		break;
	case MGPT_LC_RLT_GAME_TIME:
		_handlerRltGameTime(pkClientSession,pMsg,wSize);
		break;
	case MGPT_LC_RLT_RANDOM_CHAR_NAME:
		_handlerRltRandomCharName(pkClientSession,pMsg,wSize);
		break;
	case MGPT_SIGNATURE_RESPONSE:
		_handlerRltSignatureResponse(pkClientSession,pMsg,wSize);
		break;
	case MGPT_NOTIFY_USER_LOGON_ELSEWHERE:
	case MGPT_SS_KICK_PLAYER:
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI, "HL_FromLoginServer::ParsePacket kMsgHead.m_usMsgType = [%d]", kMsgHead.m_usMsgType);
		_handlerSSKickPlayer(pkClientSession,pMsg,wSize);
		break;
	default:
		//T_ASSERT(false);
		HL_FromGameServer::ParsePacket(pkClientSession,pOldMsg,wOldSize);
		break;
	}

// 	if (bCompressed)
// 	{
// 		T_FREE(pMsg,  Memory::MEMCATEGORY_GENERAL);
// 	}
}
//----------------------------------------------------------------------------
void HL_FromLoginServer::_handlerRltBeforeEnterGameServer( NW_Login2ClientSession* pkSession,char* pMsg, uint16_t wSize )
{
	commitPatchProgress(EVENT_CLIENT_RLT_ENTER_GAMESERVER, 0);
	GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM,"[网络][收]开始Sleep 2s");

	//Sleep(2000);
	
	MG_LC_Rlt_Enter_GameServer kRltBody;
	if(!kRltBody.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM, "MG_LC_Rlt_Enter_GameServer协议解析错误");	
		return;
	}

	GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM,"[网络][收]跨线：服务器ID = %d CharID = %d", kRltBody.m_iServerID, kRltBody.m_uiCharID);

	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkSessionGroup			= pkSessionManager->GetSessionGroup(pkSession);
	if (NULL == pkSessionGroup)
		return;

	GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM,"HL_FromLoginServer::_handlerRltBeforeEnterGameServer CurrentCharID = %lld", pkSessionGroup->GetSelectChar().data());

	if(kRltBody.m_iResult != RE_SUCCESS)
	{
		GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM, "[网络][收]MG_LC_Rlt_Enter_GameServer错误, %d", kRltBody.m_iResult);
		SendLogicInfoMessage(kRltBody.m_iResult);
		pkSessionGroup->SetNetState(NLS_NONE);
		return;
	}

	// 分配的GS节点和入口id
	pkSessionGroup->SetCurGameServer(kRltBody.m_iServerEntryID);
	pkSessionGroup->SetCurGameServerNode(kRltBody.m_iServerID);
	GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"收到地图分线信息，设置节点m_iServerID=%d",kRltBody.m_iServerID);
	pkSessionGroup->SetSelectChar(kRltBody.m_uiCharID);
	
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if(NULL != pkLogicManager)
	{
		//这种情况是由于后台服务器触发了跳线流程，需要让客户端清空逻辑对象
// 		if(NULL != pkLogicManager->GetOperationPlayer())
// 		{
// 			pkSessionGroup->m_pkGameStageMachine->SetCloseGameFlag(false);
// 			pkSessionGroup->m_pkGameStageMachine->SetChangeServerFlag(true);
// 			pkSessionGroup->m_pkGameStageMachine->SetNeedResetGameFlag(false);
// 
// 			pkSessionGroup->m_pkGameStageMachine->SetNextState(ST_GAME_STAGE_QUIT);
// 		}
		//这种情况是由于后台服务器触发了跳线流程，需要让客户端清空逻辑对象
// 		if ( pkLogicManager->GetOperationPlayer() )
// 		{
// 			pkSessionGroup->m_pkGameStageMachine->SetNextState(ST_GAME_STAGE_CHANGE_SERVER);
// 		}
	}


	MG_GC_REQ_Login		reqMsg;
	reqMsg.m_iCharID	= kRltBody.m_uiCharID;
	reqMsg.m_fClientTime = GET_PROCESS_TIME;


	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_GC_REQ_LOGIN, &reqMsg);
	if (bCreateMsg)
	{
		pkSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
		commitPatchProgress(EVENT_CLIENT_GC_REQ_LOGIN, 0);
		Utility::GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM,"[网络][发]正在发送登录请求, %d", MGPT_GC_REQ_LOGIN);
	}
}
//----------------------------------------------------------------------------
void HL_FromLoginServer::_handlerMsg(NW_Login2ClientSession* pkSession,char* pMsg, uint16_t wSize)
{
	MG_Message rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"MG_Message无法解析");
		return;
	}

	CF_ItemList* pkCSVItemList = CF_ItemList::GetSingletonPtr();
	if (NULL == pkCSVItemList)
	{
		return;
	}

	switch(rltMsg.m_iMessageID)
	{
	/*case RE_DIALOG_CONFIRM_OFF_LINE_EXCEPTION:
		{
			GfxWriteLog( LOG_TAG_CLIENT_CRITICAL , LOG_SWITCH_CLIENT_CRITICAL, "服务器踢掉线了。呜呜呜~~~~~~~");
			SendMessageDialog(rltMsg.m_iMessageID,0,0,0,rltMsg.m_iMessageID,0,0,0,0,0);
		}
		break;*/

	case RE_ITEM_OPERATION_GET:
		{
			//CF_ItemList::DataEntry* pkItemData = pkCSVItemList->GetEntryPtr(rltMsg.m_iParameter1);

			//StringType kItemName = "";
			//if(pkItemData)
			//{
			//	kItemName = LC_Helper::GetGradeName(pkItemData->_kName, pkItemData->_lGrade);
			//}

			//SendLogicInfoMessage(rltMsg.m_iMessageID,kItemName,rltMsg.m_iParameter2);
		}
		break;
	case RE_DIALOG_CONFIRM_UW_COMPOSITION_SUCCESS:
		SendMessageDialog(RE_DIALOG_CONFIRM_UW_COMPOSITION_SUCCESS,"","",
			RE_DIALOG_CONFIRM_UW_COMPOSITION_SUCCESS,rltMsg.m_iParameter1,NO_USE_PARAMETER,NO_USE_PARAMETER);
		break;
	case RE_ITEM_USE_FAIL_CANT_ADD_NEW_ITEM:
		{
			long lAddNewItemID = rltMsg.m_iParameter2;
			CF_ItemList::DataEntry* pkItemData = pkCSVItemList->GetEntryPtr(lAddNewItemID);
			if (NULL != pkItemData)
			{
				SendLogicInfoMessage(RE_ITEM_USE_FAIL_CANT_ADD_NEW_ITEM, pkItemData->_kName.c_str());
			}
		}
		break;
	case RE_PRODUCTION_SUCCESS:
		{
			long lMadeItemID = rltMsg.m_iParameter1;
			long lMadeItemNum = rltMsg.m_iParameter2;

			CF_ItemList::DataEntry* pkItemData = pkCSVItemList->GetEntryPtr(lMadeItemID);
			if (NULL != pkItemData)
			{
				SendLogicInfoMessage(RE_PRODUCTION_SUCCESS, lMadeItemNum, pkItemData->_kName.c_str());
			}
			int iProductionType = 0;
	
			g_soundMgr.Play2DEx(VARIABLE2STRING(SE_Sys_DiamondComp_OK));

			LC_GameEvent kGameEvent;
			kGameEvent.SetEventType(GLET_PRODUCTION_SUCCESS);
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, iProductionType);

			UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
			pkUIManager->HandlerGameEvent(kGameEvent);
			
		}
		break;
	case RE_ITME_TIME_LIMIT_DESTORY:
		{
			CF_ItemList::DataEntry* pkItemData = pkCSVItemList->GetEntryPtr(rltMsg.m_iParameter1);
			StringType kItemName = "";
			if(pkItemData)
			{
				char numstr[100];
				ltoa(rltMsg.m_iParameter1,numstr,10);
				kItemName = kItemName + "[@" + numstr +"]";
			}

			SendLogicInfoMessage(rltMsg.m_iMessageID,kItemName);
		}
		break;
	case RE_ITEM_TIME_LIMIT_NOTIFY:
		{
			CF_ItemList::DataEntry* pkItemData = pkCSVItemList->GetEntryPtr(rltMsg.m_iParameter1);
			StringType kItemName = "";
			if(pkItemData)
			{
				char numstr[100];
				ltoa(rltMsg.m_iParameter1,numstr,10);
				kItemName = kItemName + "[@" + numstr +"]";
			}

			StringStreamType kTime;
			if(rltMsg.m_iParameter2 == 60)
				kTime << GET_MSG(HL_FROMLOGINSERVER_CPP_001);
			else
				kTime << rltMsg.m_iParameter2 << GET_MSG(LC_HELPER_CPP_011);

			SendLogicInfoMessage(rltMsg.m_iMessageID,kItemName,kTime.str());
		}
		break;
	case RE_TOKEN_COMPOSITE_SUCCESS:
		{
			CF_ItemList::DataEntry* pkItemData = pkCSVItemList->GetEntryPtr(rltMsg.m_iParameter1);
			StringType kItemName = "";
			if(pkItemData)
			{
				char numstr[100];
				ltoa(rltMsg.m_iParameter1, numstr, 10);
				kItemName = "[@";
				kItemName.append(numstr);
				kItemName.append("]");
			}
			SendLogicInfoMessage(rltMsg.m_iMessageID, kItemName, rltMsg.m_iParameter2);
		}		
		break;
	case RE_TOKEN_COMPOSITE_FAIL_FOR_BACKPACK_FULL:
		{
			SendLogicInfoMessage(rltMsg.m_iMessageID);
		}		
		break;
	case RE_TOKEN_COMPOSITE_FAIL:
		{
			SendLogicInfoMessage(rltMsg.m_iMessageID);
		}		
		break;
	case RE_SAFE_SYSTEM_SECOND_PASSWORD_ACTIVE:
		{
			SendLogicInfoMessage(RE_SAFE_SYSTEM_SECOND_PASSWORD_ACTIVE);

			LC_GameEvent kGameEvent;
			kGameEvent.SetEventType(GLET_SAFE_SYSTEM_SECOND_PASSWORD_ACTIVE);

			UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
			pkUIManager->HandlerGameEvent(kGameEvent);
		}
		break;
	case RE_SAFE_SYSTEM_TRY_SECOND_PASSWORD_SUCCESS:
		{
			SendLogicInfoMessage(RE_SAFE_SYSTEM_TRY_SECOND_PASSWORD_SUCCESS);

			LC_GameEvent kGameEvent;
			kGameEvent.SetEventType(GLET_SAFE_SYSTEM_SECOND_PASSWORD_DEACTIVE);

			UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
			pkUIManager->HandlerGameEvent(kGameEvent);
		}
		break;
	case RE_SAFE_SYSTEM_TRY_SECOND_PASSWORD_FAIL_WRONG:
		{
			SendLogicInfoMessage(RE_SAFE_SYSTEM_TRY_SECOND_PASSWORD_FAIL_WRONG,rltMsg.m_iParameter1);
		}
		break;
	case RE_SAFE_SYSTEM_SET_SAFE_LOCK_TIME_SUCCESS:
		{
			long lHour = rltMsg.m_iParameter1 / 3600;
			long lMin  = (rltMsg.m_iParameter1 - lHour * 3600) / 60;

			StringStreamType kTimeString;
			if(lHour > 0)
				kTimeString << lHour << GET_MSG(LC_HELPER_CPP_010);
			if(lMin > 0)
				kTimeString << lMin << GET_MSG(LC_HELPER_CPP_011);
			//else
			//	kTimeString << 1 << "分钟";

			SendLogicInfoMessage(rltMsg.m_iMessageID,kTimeString.str());
		}
		break;
	case RE_SAFE_SYSTEM_ITEM_SAFE_UNLOCK_SUCCESS:
		{
			CF_ItemList::DataEntry* pkItemData = pkCSVItemList->GetEntryPtr(rltMsg.m_iParameter1);
			StringType kItemName = "";
			if(pkItemData)
			{
				char numstr[100];
				ltoa(rltMsg.m_iParameter1, numstr, 10);
				kItemName = kItemName + "[@" + numstr +"]";
			}
			SendLogicInfoMessage(rltMsg.m_iMessageID, kItemName, rltMsg.m_iParameter2);
		}
		break;
	case RE_COOK_DISH_SUCCESS:
	case RE_COOK_DISH_FAIL:
		{
			LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
			LC_ItemBase* pkItem = pkItemFactory->RequestItem(rltMsg.m_iParameter1);
			if(pkItem)
			{
				SendLogicInfoMessage(rltMsg.m_iMessageID,pkItem->GetItemDescriptionEntry()->GetItemName().c_str());	
			}
			LC_GameEvent kGameEvent;
			kGameEvent.SetEventType(GLET_CLIENT_EXCHANGE_GROUP_UPDATE);
			UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
			pkUIManager->HandlerGameEvent(kGameEvent);

		}
		break;
	case  RE_EXCHANGE_SUCCESS:
		{
			int iOptTimes = rltMsg.m_iParameter1;
			long lItemID = rltMsg.m_iParameter2;
			LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
			LC_ItemBase* pkItem = pkItemFactory->RequestItem(lItemID);
			if( pkItem )
			{
				StringType kStrName = "";
				char numstr[100];
				ltoa(lItemID,numstr,10);
				kStrName = kStrName + "[@" + numstr +"]";
				SendLogicInfoMessage(RE_TASK_START_GET_ITEM,kStrName.c_str(),iOptTimes);
			}
		}
		break;
	case RE_TRANSCRIPTION_FULL:
		{
			int iMapLogicID = rltMsg.m_iParameter1;
			int iMapResID = rltMsg.m_iParameter2;
			SendLogicInfoMessage(RE_TRANSCRIPTION_FULL, Int2String(iMapLogicID).c_str(), Int2String(iMapResID).c_str());
		}
		break;
	default:
		SendLogicInfoMessage(rltMsg.m_iMessageID);
		break;
	}

}
//------------------------------------------------------------------------
void HL_FromLoginServer::_handlerRltLogin(NW_Login2ClientSession *pkSession, char* pMsg, uint16_t wSize , uint32_t uid)
{
	commitPatchProgress(EVENT_CLIENT_RLT_USER_LOGIN, 0);

	MG_LC_RLT_User_Login kRltBody;
	if (!kRltBody.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_TAG_ERROR, LOG_SWITCH_ERROR, "[网络][收]MG_LC_RLT_User_Login解析错误");
		return;
	}

	if (RE_SUCCESS != kRltBody.m_iResult)
	{
		if(kRltBody.m_iResult == RE_LOGIN_SERVER_BUSY)
			SendMessageDialog(RE_LOGIN_SERVER_BUSY,0,0,0,RE_LOGIN_SERVER_BUSY,0,0,0,0,0);
		else
			SendLogicInfoMessage(kRltBody.m_iResult);

		GfxWriteLog(LOG_TAG_ERROR,LOG_SWITCH_ERROR,"[网络][收]LoginServer Error, %d", kRltBody.m_iResult);
		return;
	}


	NW_ClientSessionManager* pkSessionManager = NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup* pkSessionGroup = pkSessionManager->GetSessionGroup(pkSession);
	if(!pkSessionGroup)
	{
		GfxWriteLog(LOG_TAG_ERROR,LOG_SWITCH_ERROR,"[网络][收]找不到SessionGroup,无法拿到签名\n");
		return;
	}

	if( pkSessionGroup->GetUserID() != uid )
	{
		pkSessionGroup->SetUserID(uid);
	}

	in_addr kLastLoginIPStru;
	if(kRltBody.m_uiLastLoginIP == 0)
		kLastLoginIPStru.S_un.S_addr = htonl(kRltBody.m_uiClientIP);
	else
		kLastLoginIPStru.S_un.S_addr = htonl(kRltBody.m_uiLastLoginIP);

	in_addr kCurLoginIPStru;
	kCurLoginIPStru.S_un.S_addr = htonl(kRltBody.m_uiClientIP);

	pkSessionGroup->SetLastLoginIP(inet_ntoa(kLastLoginIPStru));
	pkSessionGroup->SetCurLoginIP(inet_ntoa(kCurLoginIPStru));
	
	pkSessionGroup->SetLastLoginAddress(GetIPAddress(htonl(kRltBody.m_uiLastLoginIP)));
	pkSessionGroup->SetCurLoginAddress(GetIPAddress(htonl(kRltBody.m_uiClientIP)));


	NW_Login2ClientSession* m_pkLoginSession	= pkSessionGroup->m_pkLoginSession;
	StringType sUserId = ClientCmdlineArg::GetValueSafe(CMD_ARG_UID);
	if(sUserId != "")
	{
		pkSessionGroup->ClearCharacters();

		Protocol::MG_PACKET_ENCODE_HANDLER hMsgHandler;
		Protocol::MG_GC_REQ_CharList kMsgReq;
		kMsgReq.m_dwUserID = pkSessionGroup->m_dwUserID;
		kMsgReq.m_dwServerID = pkSessionGroup->m_iCurCitizenShip;
		bool bCreateMsg = CreateMsgPacketHandler(pkSessionGroup, Protocol::CRYPT_ALGORITHM_NONE, hMsgHandler, MGPT_LC_REQ_CHAR_LIST,&kMsgReq);

		commitPatchProgress(EVENT_CLIENT_REQ_CHAR_LIST, 0);
		Utility::GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM,"[网络][发]正在请求角色列表 GameServer:%d, %d", kMsgReq.m_dwServerID, MGPT_LC_REQ_CHAR_LIST);

		if(bCreateMsg)
		{
			pkSessionGroup->m_pkLoginSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

			pkSessionGroup->SetNetState(NLS_WAIT_CHAR_LIST);
		}

		//检查防沉迷状态，发消息给UI
		pkSessionGroup->NotifyAntiWallowStateToUIInLogin();
	}
	//外网也需要请求服务器列表用于跨服活动
	/*else
	{*/
		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = false;
		bCreateMsg = CreateMsgPacketHandler(pkSessionGroup, CRYPT_ALGORITHM_NONE, hMsgHandler, MGPT_LC_REQ_SERVER_LIST, NULL);
		pkSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
		Utility::GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM,"[网络][发]正在请求服务器列表, %d", MGPT_LC_REQ_SERVER_LIST);
	//}
}
//------------------------------------------------------------------------
void HL_FromLoginServer:: _handlerServerList(NW_Login2ClientSession *pkClientSession, char* pMsg, uint16_t wSize)
{
	MG_LC_RLT_ServerList kServerListMsg;
	if (!kServerListMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "服务器列表协议解析错误");
		return;
	}

	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkClientSession);

	//删除所有的服务器列表
	pkSessionManager->ClearGameServerInfo();
	
	GfxWriteLog(LOG_SYSTEM_FILELINE,LOG_SWITCH_SYSTEM,"Fetch GameServerList");
	//如果请求服务器列表不成功
	if (kServerListMsg.m_iResult != RE_LOGIN_REQ_SERVER_LIST_SUCCESS)
	{
		SendLogicInfoMessage(kServerListMsg.m_iResult);
		if(pkClientSessionGroup->GetNetState() == NLS_WAIT_SERVER_LIST)
		{
			pkClientSessionGroup->SetNetState(NLS_FAIL_SERVER_LIST);

			//获取服务器列表失败，断开连接
			pkClientSession->Disconnect();
		}
	}
	else
	{
		//在客户端保存服务器列表
		/*for (std::vector<PS_ServerInfo>::reverse_iterator iter = kServerListMsg.m_vecServerList.rbegin(); iter != kServerListMsg.m_vecServerList.rend(); ++ iter)
		{
			PS_ServerInfo& rkGameServerInfo = (*iter);
			pkSessionManager->AddGameServerInfo(rkGameServerInfo);
		}*/

		
		for (PS_ServerSimInfoVector::iterator iter = kServerListMsg.m_vecServerList.begin(); iter != kServerListMsg.m_vecServerList.end(); ++ iter)
		{
			PS_GameSvrInfo kSvrInfo;
			kSvrInfo.m_iType = iter->m_iType;
			kSvrInfo.m_cStatus = iter->m_cStatus;
			kSvrInfo.m_iServerID = iter->m_iServerID;
			kSvrInfo.m_iGroupID = iter->m_iGroupID;
			kSvrInfo.m_szName = iter->m_szName;
			kSvrInfo.m_kSideline = iter->m_kSideline;
			for (PS_ServerStatInfoVector::iterator Statiter = kServerListMsg.m_vecStatList.begin(); Statiter != kServerListMsg.m_vecStatList.end(); ++Statiter)
			{
				if (Statiter->m_iServerID == kSvrInfo.m_iServerID)
				{
					kSvrInfo.m_iServerOpenTime = Statiter->m_uiStartTime;
					kSvrInfo.m_iServerConflateTime = Statiter->m_uiConflateTime;
					break;
				}
			}
			pkSessionManager->AddGameServerInfo(kSvrInfo);
		}

#if !defined(CLIENT_DLL) || defined(CLIENT_DLL_INTERNAL)
		//获取服务器列表成功后清空网络状态
		pkClientSessionGroup->SetNetState(NLS_NONE);

		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_GET_SERVERLIST_END);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);

		NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
		NW_ClientSessionGroup*	 pkSessionGroup	= pkSessionManager->GetSessionGroup(pkClientSession);
		if(!pkSessionGroup)
		{
			GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"无法找到SessionGroup");
			return;
		}
#endif
	}
}

void HL_FromLoginServer::_handlerRltCharList( NW_Login2ClientSession* pkSession,char* pMsg,uint16_t wSize )
{
	commitPatchProgress(EVENT_CLIENT_RLT_CHAR_LIST, 0);

	MG_GC_RLT_CharList CharListMsg;
	if (!CharListMsg.Decode(pMsg,wSize))
	{
		commitPatchProgress(EVENT_CLIENT_ERROR_9996, 0);
		GfxWriteLog(LOG_TAG_ERROR, LOG_SWITCH_ERROR, "[网络][收]MG_GC_RLT_CharList解析错误");
		return;
	}

	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);

	//删除原有的角色列表
	pkClientSessionGroup->ClearCharacters();

	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if(pkLogicManager != NULL)
	{
		pkLogicManager->SetOperationPlayerID(0);
		pkLogicManager->SetOperationCharID(INVALID_UNIQUE_ID);
	}

	//获取角色列表失败
	DWORD dwUserID = pkClientSessionGroup->m_dwUserID;
	bool bSuccess = CharListMsg.m_iResult == RE_LOGIN_REQ_CHAR_LIST_SUCCESS;
	if (CharListMsg.m_dwUserID != dwUserID || !bSuccess)
	{
		////提示获取角色列表失败
		pkClientSessionGroup->SetNetState(NLS_FAIL_CHAR_LIST);
		if (CharListMsg.m_dwUserID != dwUserID)
			commitPatchProgress(EVENT_CLIENT_ERROR_9997, 0);
		else
			commitPatchProgress(EVENT_CLIENT_ERROR_9998, 0);
		Utility::GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM,"[网络][收]获取角色列表失败");

		if(!bSuccess)
			SendLogicInfoMessage(CharListMsg.m_iResult);
		return;
	}
	else
	{
		pkClientSessionGroup->SetNetState(NLS_NONE);

		//将角色按照创建的先后顺序排序（即按照CharID排序）
		mem::map<int, int> kSortChar;
		int vecSize = CharListMsg.m_vecCharBaseInfo.size();
		for (int i = 0; i < vecSize; i ++)
		{
			int iCharID = CharListMsg.m_vecCharBaseInfo[i].Info.m_uiCharID;
			kSortChar.insert(std::make_pair(iCharID, i));
		}

		unique_id_impl iCharID = INVALID_UNIQUE_ID;
		//按照排序结果，在本地保存当前用户的角色列表
		if(!kSortChar.empty())
		{
			int iCharIndex = kSortChar.begin()->second;
			PS_CharViewInfo rkCharInfo;
			char_data_define::char_base_info info;
			info.ParseFromString(CharListMsg.m_vecCharBaseInfo[iCharIndex].BlobBuffer);
			rkCharInfo  = CharListMsg.m_vecCharBaseInfo[iCharIndex].Info;
			iCharID = GEN_UNIQUE_ID(getYunYingplatform(), rkCharInfo.m_iCitizenship, rkCharInfo.m_uiCharID);

			rkCharInfo.m_LastMapResID = info.last_map_res_id();
			rkCharInfo.m_curMapResID = info.cur_map_res_id();
			for (int i = 0; i < info.tutorial_info_size() && i < MG_TUTORIALDATA_LENGTH; i++)
			{
				int value = info.tutorial_info(i);
				rkCharInfo.m_Tutorial[i] = (byte)value;
			}
			//0-4是前4个装备：Head, Body, Foot, Weapon
			pkClientSessionGroup->AddCharacter(rkCharInfo);


			Utility::GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM,"[网络][收]获取角色列表成功 当前角色CharID:%d", iCharID.detail());

			//进入游戏
			DWORD dwUserID					= pkClientSessionGroup->m_dwUserID;				
			
			//设置进入游戏的角色
			pkClientSessionGroup->SetSelectChar(iCharID);
			LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
			pkLogicManager->SetOperationPlayerID(dwUserID);
			pkLogicManager->SetOperationCharID(iCharID);

			int lServerID = -1;

			//特殊登录流程
			int iLoginType = pkSessionManager->GetLoginType();
			if(iLoginType == NW_ClientSessionManager::LT_NORMAL)
				lServerID = pkClientSessionGroup->GetCurCitizenShip();
			else
				lServerID = UT_BaseStringUtility::toInt(ClientCmdlineArg::GetValueSafe(CMD_ARG_SID));
			
			MG_LC_Req_Enter_GameServer	reqMsg;
			reqMsg.m_uiCharID	= pkClientSessionGroup->GetSelectChar();
			reqMsg.m_iServerID = lServerID;

			MG_PACKET_ENCODE_HANDLER hMsgHandler;
			bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_LC_REQ_ENTER_GAMESERVER, &reqMsg);
			if (bCreateMsg)
			{
				pkClientSessionGroup->m_pkLoginSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
				Utility::GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM,"[网络][发]请求进入GameServer:%d, CharID:0x%I64x, msg=%d", lServerID, reqMsg.m_uiCharID, MGPT_LC_REQ_ENTER_GAMESERVER);
			}
			pkClientSessionGroup->SetNetState(NLS_WAIT_ENTER_GAME);

			//打点: 点击进入游戏按钮，请求进入游戏
			commitPatchProgress(EVENT_CLIENT_REQ_ENTER_GAME, 0);
			MG_MessageCenter::GetSingleton().SendMessage( CreateMsg_PLAYER_LOGIN(&rkCharInfo), true);	//必须true
		}
		else
		{
			//打点: 服务器返回角色列表（首次拉取账户下的角色信息判断是否需要新建角色）
			commitPatchProgress(EVENT_CLIENT_ROLE_LIST, 0);
			Utility::GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM,"[网络][收]当前没有角色，转到创角流程");

			pkClientSessionGroup->m_pkGameStageMachine->SetNextState(ST_GAME_STAGE_CREATE_CHARACTER);
			pkClientSessionGroup->m_pkGameStageMachine->Update(0.0f,0.0f);
		}
	}
}

void HL_FromLoginServer::_handlerRltCreateChar( NW_Login2ClientSession *pkClientSession, char* pMsg, uint16_t wSize )
{
	commitPatchProgress(EVENT_CLIENT_RLT_CREATE_ROLE, 0);

	MG_LC_Rlt_CreateChar CreateCharMsg;
	if (!CreateCharMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_LC_Rlt_CreateChar解析错误");
		return;
	}

	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkClientSession);

	DWORD dwUserID = pkClientSessionGroup->m_dwUserID;
	if (CreateCharMsg.m_dwUserID == dwUserID && CreateCharMsg.m_iResult == RE_LOGIN_CREATE_CHAR_SUCCESS)
	{
		pkClientSessionGroup->SetNetState(NLS_NONE);

		//Add Charactor base info
		PS_CharViewInfo rkCharInfo;
		char_data_define::char_base_info info;
		info.ParseFromString(CreateCharMsg.m_kCharBaseInfo.BlobBuffer);
		rkCharInfo  = CreateCharMsg.m_kCharBaseInfo.Info;
		unique_id_impl iCharID = GEN_UNIQUE_ID(getYunYingplatform(), rkCharInfo.m_iCitizenship, rkCharInfo.m_uiCharID);

		rkCharInfo.m_LastMapResID = info.last_map_res_id();
		rkCharInfo.m_curMapResID = info.cur_map_res_id();
		for (int i = 0; i < info.tutorial_info_size() && i < MG_TUTORIALDATA_LENGTH; i++)
		{
			int value = info.tutorial_info(i);
			rkCharInfo.m_Tutorial[i] = (byte)value;
		}
		
		pkClientSessionGroup->AddCharacter(rkCharInfo);
		pkClientSessionGroup->SetSelectChar(iCharID);

		//默认选择新角色
		SetUserProfileInt64Val("LastLoginCharID", CreateCharMsg.m_kCharBaseInfo.Info.m_uiCharID);

		//创建角色成功直接进入游戏，不需要再跳转到选择角色SCREEN

		//打点: 新用户创建角色成功（成功生成角色信息）
		commitPatchProgress(EVENT_CLIENT_CREATE_ROLE_OK, 0);
		GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM,"创建角色成功, CharID:%d Name:%s WeaponType:%d", rkCharInfo.m_uiCharID, rkCharInfo.m_szCharName, rkCharInfo.m_byWeapon);
		MG_MessageCenter::GetSingleton().SendMessage( CreateMsg_NEW_ROLE(&rkCharInfo), true);	//必须true，要优先于 SC_ClientSkillInterface::OnEnterGame，它会触发Login上报
	}
	else if (CreateCharMsg.m_dwUserID == dwUserID && CreateCharMsg.m_iResult == RE_LOGIN_NAME_ALREADY_USED)
	{
		//pkClientSessionGroup->SetMessage(RE_LOGIN_NAME_ALREADY_USED);
		//pkClientSessionGroup->SetNetState(NLS_FAIL_CREATE_CHAR);
		SendLogicInfoMessage(RE_LOGIN_NAME_ALREADY_USED, "", "", "", "", "", RE_LOGIN_NAME_ALREADY_USED);
	}
	else
	{
		pkClientSessionGroup->SetMessage(CreateCharMsg.m_iResult);
		pkClientSessionGroup->SetNetState(NLS_FAIL_CREATE_CHAR);
	}

	if(CreateCharMsg.m_iResult != RE_LOGIN_CREATE_CHAR_SUCCESS)
	{
		PS_CharViewInfo rkCharInfo;
		char_data_define::char_base_info info;
		info.ParseFromString(CreateCharMsg.m_kCharBaseInfo.BlobBuffer);
		rkCharInfo  = CreateCharMsg.m_kCharBaseInfo.Info;

		GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_ERROR,"创建角色失败 Result:%d UserID:%d CitizenShip:%d CharID:%d Name:%s WeaponType:%d", CreateCharMsg.m_iResult, CreateCharMsg.m_dwUserID, rkCharInfo.m_iCitizenship, rkCharInfo.m_uiCharID, rkCharInfo.m_szCharName, rkCharInfo.m_byWeapon);
	}

	LC_ClientGameEvent kGameEvent;
	kGameEvent.SetClientEventType(GameLogic::CGET_NOTIFY_CREATE_CHAR_RESULT);

	StringStreamType sStream;
	sStream << CreateCharMsg.m_iResult << "#";
	sStream << CreateCharMsg.m_kCharBaseInfo.Info.m_uiCharID << "#" << CreateCharMsg.m_kCharBaseInfo.Info.m_iCitizenship;
	LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(sStream.str());
	kGameEvent.SetUserData(&kUserData);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);	//< 这里会触发 SC_ClientSkillInterface::OnEnterGame
}

void HL_FromLoginServer::_handlerRltDeleteChar( NW_Login2ClientSession *pkClientSession, char* pMsg, uint16_t wSize )
{
	MG_LC_Rlt_DeleteChar DeleteCharMsg;
	if(!DeleteCharMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"请求删除角色协议解析错误");
		return;
	}

	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkClientSession);

	DWORD dwUserID = pkClientSessionGroup->m_dwUserID;
	if(DeleteCharMsg.m_dwUserID != dwUserID || DeleteCharMsg.m_iResult != RE_LOGIN_DELETE_CHAR_SUCCESS)
	{
		//提示删除角色失败
		pkClientSessionGroup->SetNetState(NLS_FAIL_DELETE_CHAR);

		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"请求删除角色失败");
		return;
	}
	else
	{
		pkClientSessionGroup->SetNetState(NLS_NONE);

		//提示删除角色成功
		SendLogicInfoMessage(DeleteCharMsg.m_iResult);

		//在本地暂时删除角色
		int iDeleteTime = DeleteCharMsg.m_iDeleteTime;
		pkClientSessionGroup->DeleteCharacter(DeleteCharMsg.m_iCharID, iDeleteTime);

		//重新请求角色列表
		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		Protocol::MG_GC_REQ_CharList kMsgReq;
		kMsgReq.m_dwUserID = pkClientSessionGroup->m_dwUserID;
		kMsgReq.m_dwServerID = pkClientSessionGroup->m_iCurCitizenShip;
		bool bCreateMsg = CreateMsgPacketHandler(pkClientSessionGroup, CRYPT_ALGORITHM_NONE, hMsgHandler, MGPT_LC_REQ_CHAR_LIST, &kMsgReq);
		if (bCreateMsg)
			pkClientSession->Send( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );


	}
}
//-------------------------------------------------------------------------------------------
void HL_FromLoginServer::_handlerRltRecoverChar( NW_Login2ClientSession* pkClientSession,char* pMsg, uint16_t wSize )
{
	MG_LC_Rlt_RecoverChar RecoverCharMsg;
	if (!RecoverCharMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "RecoverCharMsg解析错误");
		return;
	}

	NW_ClientSessionManager* pkSessionManager = NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkClientSession);

	DWORD dwUserID = pkClientSessionGroup->m_dwUserID;
	if (RecoverCharMsg.m_dwUserID != dwUserID || RecoverCharMsg.m_iResult != RE_LOGIN_RECOVER_CHAR_SUCCESS)
	{
		//提示恢复角色出错
		pkClientSessionGroup->SetNetState(NLS_FAIL_RECOVER_CHAR);
		return;
	}
	else
	{
		//提示恢复角色成功
		pkClientSessionGroup->SetNetState(NLS_NONE);
		SendLogicInfoMessage(RecoverCharMsg.m_iResult);
		pkClientSessionGroup->RecoverCharacter(RecoverCharMsg.m_iCharID);

		//重新请求角色列表
		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		Protocol::MG_GC_REQ_CharList kMsgReq;
		kMsgReq.m_dwUserID = pkClientSessionGroup->m_dwUserID;
		kMsgReq.m_dwServerID = pkClientSessionGroup->m_iCurCitizenShip;
		bool bCreateMsg = CreateMsgPacketHandler(pkClientSessionGroup, CRYPT_ALGORITHM_NONE, hMsgHandler, MGPT_LC_REQ_CHAR_LIST, &kMsgReq);
		if (bCreateMsg)
			pkClientSession->Send( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );

	}
}
//-------------------------------------------------------------------------------------------
void HL_FromLoginServer::_handlerRltServerBusy(NW_Login2ClientSession* pkClientSession,char* pkMsg,uint16_t wSize)
{
	SendLogicInfoMessage(RE_LOGIN_SERVER_BUSY);
}
//------------------------------------------------------------------------

void HL_FromLoginServer::_handlerRltGameTime(NW_Login2ClientSession* pkClientSeesion,char* pkMsg,uint16_t wSize)
{
	MG_LC_RLT_GameTime rltMsg;
	if(!rltMsg.Decode(pkMsg,wSize))
		return;

	NW_ClientSessionManager* pkSessionManager = NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkClientSeesion);
	pkClientSessionGroup->SetOnlineTime(rltMsg.m_uiOnlineTime / 60);
}

void HL_FromLoginServer::_handlerRltRandomCharName(NW_Login2ClientSession* pkClientSeesion,char* pkMsg,uint16_t wSize)
{
	MG_LC_Rlt_RandomCharName rltMsg;
	if(!rltMsg.Decode(pkMsg, wSize))
	{
		return;
	}

	if(rltMsg.m_iResult != RE_SUCCESS)
	{
		GfxWriteLog(LOG_TAG_ERROR, LOG_SWITCH_ERROR, "random name error result:%d, UserID:%u",rltMsg.m_iResult, rltMsg.m_dwUserID);
		SendLogicInfoMessage(rltMsg.m_iResult);
		return;
	}
	LC_ClientGameEvent kGameEvent;
	kGameEvent.SetClientEventType(GameLogic::CGET_RANDOM_CHAR_NAME_NOTIFY);

	StringType res = CodeConvert::MbsToUtf8(STDSTR2TPSTR(rltMsg.m_szCharName), CP_gbk);
	GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM, "收到随机名字 random name result:%d, UserID:%u, CharName:%s",rltMsg.m_iResult, rltMsg.m_dwUserID, res.c_str());
	LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(res);
	kGameEvent.SetUserData(&kUserData);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromLoginServer::_handlerRltSignatureResponse(NW_Login2ClientSession* pkClientSeesion,char* pkMsg,uint16_t wSize)
{
	MG_SignatureResponse rltMsg;
	if(!rltMsg.Decode(pkMsg, wSize))
	{
		return;
	}

	if(RE_USER_BAN == rltMsg.m_Result)
		SendLogicInfoMessage(RE_USER_BAN, "", "", "", "", "", RE_USER_BAN);
}


void HL_FromLoginServer::_handlerSSKickPlayer(NW_Login2ClientSession* pkClientSeesion,char* pkMsg,uint16_t wSize)
{
	MG_ServerKickUser kRltBody;
	if(!kRltBody.Decode(pkMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM, "HL_FromLoginServer::_handlerSSKickPlayer Decode Error");
		return;
	}

	GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM, "HL_FromLoginServer::_handlerSSKickPlayer Reason=[%d]", kRltBody.nReason);
	NetBase::SetOnDisconnectFuncReason(kRltBody.nReason);
}