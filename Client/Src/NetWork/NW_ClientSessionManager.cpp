
#include "ClientPCH.h"
#include "LM_LocaleManager.h"

#include "LC_ClientLogicManager.h"
#include "LC_ClientGamePlayer.h"

#include "NW_ClientSessionManager.h"
#include "NW_ClientSessionFactory.h"
#include "NW_ClientNetManager.h"
#include "NW_Login2ClientSession.h"
#include "NW_Game2ClientSession.h"
#include "NW_Helper.h"

#include "MG_MessageBase.h"

#include "UE_UiEffectManager.h"
#include "IM_Manager.h"

#include "SM_GameStageMachine.h"
#include "SE_SceneManager.h"

#include "UI_UIManager.h"
#include "UI_AvatarManager.h"

#include "UT_LogicInfoMessage.h"
#include "UT_FileManagerBase.h"

#include "AS_ClientSoundManager.h"
#include "LC_ClientCollectionManager.h"

#include "tinyxml.h"

using namespace Protocol;
using namespace NetWork;
using namespace StateMachine;
using namespace Utility;
using namespace CSVFile;


//---------------------------------------------------------------------------------
NW_ClientSessionGroup::NW_ClientSessionGroup()
: m_pkLoginSession(NULL)
, m_pkGameSession(NULL)
, m_strUser("")
, m_strPwd("")
, m_strSessionKey("")
, m_dwUserID(0)
, m_lCharID(0)
, m_pkGameStageMachine(NULL)
, m_nCurGameServerID(0)
, m_nCurGameServerNodeID(0)
, m_nSelectCharID(0)
, m_bRequestExitGame(false)
, m_sSignatureBuffLength(0)
, m_eNetState(NLS_NONE)
, m_fLastNetStateTime(0.0f)
, m_eMessage(RE_FAIL)
, m_bInGameFlag(false)
, m_iCreateCharServerID(0)
{
	m_isFcm = false;
	m_onlineTime = 0;
	m_isOnline = false;
	m_lastCalOnlineTime = -100.0;

	m_iLastWaitUserCount = 0;
	m_fLastWaitUserCountTime = 0.0f;
}
NW_ClientSessionGroup::~NW_ClientSessionGroup()
{

}
//---------------------------------------------------------------------------------
void NW_ClientSessionGroup::UpdateNetState(float fCurrentTime)
{
	switch(m_eNetState)
	{
	case NLS_WAIT_SIGNATURE:
		{
			if(fCurrentTime - m_fLastNetStateTime > 5.0f)
				SetNetState(NLS_FAIL_SIGNATURE);
		}
		break;
	case NLS_WAIT_VERIFY:
		{
			if(fCurrentTime - m_fLastNetStateTime > 5.0f)
				SetNetState(NLS_FAIL_VERIFY);
		}
		break;
	case NLS_WAIT_LOGIN:
		{
			if(fCurrentTime - m_fLastNetStateTime > 20.0f)
				SetNetState(NLS_FAIL_LOGIN);
		}
		break;
	case NLS_WAIT_SERVER_LIST:
		{
			if(fCurrentTime - m_fLastNetStateTime > 20.0f)
				SetNetState(NLS_FAIL_SERVER_LIST);
		}
		break;
	case NLS_WAIT_GAME_SERVER:
		{
			if(fCurrentTime - m_fLastNetStateTime > 20.0f)
				SetNetState(NLS_FAIL_GAME_SERVER);
		}
		break;
	case NLS_WAIT_CHAR_LIST:
		{
			if(fCurrentTime - m_fLastNetStateTime > 30.0f)
				SetNetState(NLS_FAIL_CHAR_LIST);
		}
		break;
	case NLS_WAIT_CREATE_CHAR:
		{
			if(fCurrentTime - m_fLastNetStateTime > 30.0f)
			{
				SendLogicInfoMessage(RE_LOGIN_CREATE_CHAR_FAIL, "", "", "", "", "", RE_LOGIN_CREATE_CHAR_FAIL);
				SetNetState(NLS_FAIL_CREATE_CHAR);
			}
		}
		break;
	case NLS_WAIT_DELETE_CHAR:
		{
			if(fCurrentTime - m_fLastNetStateTime > 30.0f)
				SetNetState(NLS_FAIL_DELETE_CHAR);
		}
		break;
	case NLS_WAIT_RECOVER_CHAR:
		{
			if(fCurrentTime - m_fLastNetStateTime > 30.0f)
				SetNetState(NLS_FAIL_RECOVER_CHAR);
		}
		break;
	case NLS_HANGING_ENTER_GAME:
		{
			UpdateWaitEnterGame(fCurrentTime);
		}
		break;
	//case NLS_WAIT_CHANGE_SERVER:
	//	{
	//		UpdateChangeServer(fCurrentTime);
	//	}
	//	break;
	case NLS_WAIT_ENTER_GAME:
		{
			if(fCurrentTime - m_fLastNetStateTime > 10.0f)
				SetNetState(NLS_FAIL_ENTER_GAME);
		}
		break;
	}

	CheckAntiWallowState(fCurrentTime);
}
//---------------------------------------------------------------------------------
void NW_ClientSessionGroup::SetNetState(int iNetState)
{
	m_fLastNetStateTime = GET_PROCESS_TIME;
	
	if(iNetState == m_eNetState)
		return;

	switch(iNetState)
	{
	case NLS_NONE:
		SendLogicInfoMessage(RE_LOGIN_NORMAL_STATE,"NULL");
		break;
	case NLS_WAIT_SIGNATURE:
		SendLogicInfoMessage(RE_LOGIN_CONNECTING_SERVER);
		break;
	case NLS_FAIL_SIGNATURE:
		GfxWriteLog(LOG_TAG_ERROR, LOG_SWITCH_ERROR, "[FAIL] NLS_FAIL_SIGNATURE");
		SendLogicInfoMessage(RE_LOGIN_CONNECT_SERVER_TIMEOUT);
		break;
	case NLS_WAIT_VERIFY:
		SendLogicInfoMessage(RE_LOGIN_INVALIDATING_USER);
		break;
	case NLS_FAIL_VERIFY:
		GfxWriteLog(LOG_TAG_ERROR, LOG_SWITCH_ERROR, "[FAIL] NLS_FAIL_VERIFY");
		SendLogicInfoMessage(RE_LOGIN_CONNECT_SERVER_TIMEOUT);
		SetNetState(NLS_NONE);
		break;
	case NLS_WAIT_LOGIN:
		SendLogicInfoMessage(RE_LOGIN_CONNECTING_SERVER);
		break;
	case NLS_FAIL_LOGIN:
		GfxWriteLog(LOG_TAG_ERROR, LOG_SWITCH_ERROR, "[FAIL] NLS_FAIL_LOGIN");
		SendLogicInfoMessage(RE_LOGIN_CONNECT_SERVER_TIMEOUT);
		m_pkLoginSession->Disconnect();
		break;
	case NLS_WAIT_SERVER_LIST:
		//SendLogicInfoMessage(RE_LOGIN_REQUEST_GAMESERVER_LIST);
		break;
	case NLS_FAIL_SERVER_LIST:
		GfxWriteLog(LOG_TAG_ERROR, LOG_SWITCH_ERROR, "[FAIL] NLS_FAIL_SERVER_LIST");
		SendLogicInfoMessage(RE_LOGIN_CONNECT_SERVER_TIMEOUT);
		m_pkLoginSession->Disconnect();
		break;
	case NLS_WAIT_GAME_SERVER:
		SendLogicInfoMessage(RE_LOGIN_CONNECTING_SERVER);
		break;
	case NLS_HANGING_ENTER_GAME:
		//SendLogicInfoMessage(RE_ENTER_GAME_WAIT_FROM_A_MOMENT);
		m_fLastCheckEnterGameTime = 0.0f;
		m_iLastWaitUserCount = -1;
		m_fLastWaitUserCountTime = GET_PROCESS_TIME;
		//m_pkGameSession->Disconnect();
		break;
	case NLS_FAIL_GAME_SERVER:
		GfxWriteLog(LOG_TAG_ERROR, LOG_SWITCH_ERROR, "[FAIL] NLS_FAIL_GAME_SERVER");
		SendLogicInfoMessage(RE_LOGIN_CONNECT_SERVER_TIMEOUT);
		//m_pkGameSession->Disconnect();
		break;
	case NLS_WAIT_CHAR_LIST:
		SendLogicInfoMessage(RE_LOGIN_REQUEST_CHAR_LIST);
		break;
	case NLS_FAIL_CHAR_LIST:
		GfxWriteLog(LOG_TAG_ERROR, LOG_SWITCH_ERROR, "[FAIL] NLS_FAIL_CHAR_LIST");
		SendLogicInfoMessage(RE_LOGIN_REQ_CHAR_LIST_FAIL, "", "", "", "", "", RE_LOGIN_REQ_CHAR_LIST_FAIL);
		m_pkGameSession->Disconnect();
		break;
	case NLS_WAIT_CREATE_CHAR:
		SendLogicInfoMessage(RE_LOGIN_REQUEST_CREATE_CHAR);
		break;
	case NLS_FAIL_CREATE_CHAR:
		GfxWriteLog(LOG_TAG_ERROR, LOG_SWITCH_ERROR, "[FAIL] NLS_FAIL_CREATE_CHAR");
		SendLogicInfoMessage(m_eMessage);
		break;
	case NLS_WAIT_DELETE_CHAR:
		SendLogicInfoMessage(RE_LOGIN_REQUEST_DELETE_CHAR);
		break;
	case NLS_FAIL_DELETE_CHAR:
		SendLogicInfoMessage(RE_LOGIN_DELETE_CHAR_FAIL);
		break;
	case NLS_WAIT_RECOVER_CHAR:
		SendLogicInfoMessage(RE_LOGIN_REQUEST_RECOVER_CHAR);
		break;
	case NLS_FAIL_RECOVER_CHAR:
		SendLogicInfoMessage(RE_LOGIN_RECOVER_CHAR_FAIL);
		break;
	case NLS_WAIT_ENTER_GAME:
		SendLogicInfoMessage(RE_LOGIN_REQUEST_ENTER_GAME);
		break;
	case NLS_FAIL_ENTER_GAME:
		GfxWriteLog(LOG_TAG_ERROR, LOG_SWITCH_ERROR, "[FAIL] NLS_FAIL_ENTER_GAME");
		SendLogicInfoMessage(RE_LOGIN_CONNECT_SERVER_TIMEOUT);
		SetNetState(NLS_NONE);
		break;
	}
	

	m_eNetState = (NW_NetState)iNetState;
}
//---------------------------------------------------------------------------------
int NW_ClientSessionGroup::GetNetState()
{
	return m_eNetState;
}
//---------------------------------------------------------------------------------
void NW_ClientSessionGroup::AddCharacter(PS_CharViewInfo& rkCharacter)
{
	m_kCharBaseInfoList.push_back(rkCharacter);
}
//---------------------------------------------------------------------------------
void NW_ClientSessionGroup::DeleteCharacter(const unique_id_impl& iCharID, int iDeleteTime )
{
	for (PS_CharBaseInfoList::iterator iter = m_kCharBaseInfoList.begin(); iter != m_kCharBaseInfoList.end(); ++iter)
	{
		if ( (*iter).m_uiCharID == iCharID.detail() && (*iter).m_iCitizenship == iCharID.catagory() )
		{
			//if (bForever)
			//{
			//	//永久删除角色
			//	delete (*iter);
			//	pkCharList->erase(iter);
			//}
			//else
			//{
			//	//设置角色暂时被删除
			//	NW_CharInfo* pkCharInfo = (*iter);
			//	pkCharInfo->m_ucCharDeleteFlag = 1;
			//}
			PS_CharViewInfo& rkCharInfo = (*iter);
			rkCharInfo.m_byDeleteFlag	= true;
			rkCharInfo.m_iDeleteTime	= iDeleteTime;
			return;
		}
	}
}
//---------------------------------------------------------------------------------
void NW_ClientSessionGroup::DeleteCharacter(const unique_id_impl& iCharID )
{
	for (PS_CharBaseInfoList::iterator iter = m_kCharBaseInfoList.begin();iter != m_kCharBaseInfoList.end(); ++iter)
	{
		if ( (*iter).m_uiCharID == iCharID.detail() && (*iter).m_iCitizenship == iCharID.catagory() )
		{
			m_kCharBaseInfoList.erase(iter);
			return;
		}
	}
}
//---------------------------------------------------------------------------------
void NW_ClientSessionGroup::ClearCharacters()
{
	m_kCharBaseInfoList.clear();

	LC_ClientGamePlayer* pkPlayer = NULL;
	if(m_pkGameSession && (pkPlayer = m_pkGameSession->GetPlayer()) != NULL)
	{
		m_pkGameSession->SetPlayer(NULL);
		pkPlayer->SetSession(NULL);
	}
}
//---------------------------------------------------------------------------------
void NW_ClientSessionGroup::RecoverCharacter(const unique_id_impl& iCharID )
{
	for (PS_CharBaseInfoList::iterator iter = m_kCharBaseInfoList.begin();iter != m_kCharBaseInfoList.end(); ++iter)
	{
		if ( (*iter).m_uiCharID == iCharID.detail() && (*iter).m_iCitizenship == iCharID.catagory())
		{
			//恢复暂时删除的角色
			PS_CharViewInfo& rkCharInfo = (*iter);
			rkCharInfo.m_iDeleteTime	= 0;
			rkCharInfo.m_byDeleteFlag	= false;
			return;
		}
	}
}
//---------------------------------------------------------------------------------
PS_CharViewInfo* NW_ClientSessionGroup::GetCharacter(const unique_id_impl& iCharID )
{
	for (PS_CharBaseInfoList::iterator iter = m_kCharBaseInfoList.begin();iter != m_kCharBaseInfoList.end(); ++iter)
	{
		if ((*iter).m_uiCharID == iCharID.detail() && (*iter).m_iCitizenship == iCharID.catagory())
		{
			return &(*iter);
		}
	}

	return NULL;
}
//---------------------------------------------------------------------------------
PS_CharBaseInfoList& NW_ClientSessionGroup::GetCharList()
{
	return m_kCharBaseInfoList;
}
//---------------------------------------------------------------------------------
NW_ItemIDList* NW_ClientSessionGroup::GetCreateCharItemList( NW_CharItemType kItemType )
{
	switch (kItemType)
	{
	case CIT_HEAD_EQUIP:
		return &m_kHeadEquipList;
		break;
	case CIT_BODY_EQUIP:
		return &m_kBodyEquipList;
		break;
	case CIT_FOOT_EQUIP:
		return &m_kFootEquipList;
		break;
	case CIT_HEAD_PIC:
		return &m_kHeadPicList;
		break;
	case CIT_WEAPON:
		return &m_kWeaponList;
	default:
		return NULL;
	}
}
//---------------------------------------------------------------------------------
void NW_ClientSessionGroup::AddCreateCharItem( NW_CharItemType kItemType, long lItemID )
{
	NW_ItemIDList* pkItemList = GetCreateCharItemList(kItemType);
	if (NULL == pkItemList)
	{
		return;
	}
	pkItemList->push_back(lItemID);
}
//---------------------------------------------------------------------------------
void NW_ClientSessionGroup::DeleteCreateItem( NW_CharItemType kItemType )
{
	NW_ItemIDList* pkItemList = GetCreateCharItemList(kItemType);
	if (NULL == pkItemList)
	{
		return;
	}
	pkItemList->clear();
}
//---------------------------------------------------------------------------------
void NW_ClientSessionGroup::SetFcm( uint8_t isAdult )
{
	if (isAdult)
	{
		m_isFcm = true;
	}
	else
	{
		m_isFcm = false;
	}
}
//---------------------------------------------------------------------------------
bool NW_ClientSessionGroup::IsFcm()
{
	return m_isFcm;
}
//---------------------------------------------------------------------------------
uint16_t NW_ClientSessionGroup::GetOnlineTime() const
{
	return m_onlineTime;
}
//---------------------------------------------------------------------------------
void NW_ClientSessionGroup::SetOnlineTime( uint16_t time )
{
	m_onlineTime = time;
}

bool NW_ClientSessionGroup::IsCalOnlineTime()
{
	return m_isOnline;
}
//---------------------------------------------------------------------------------
void NW_ClientSessionGroup::SetCanCalOnlineTime( bool flag )
{
	m_isOnline = flag;
}
//---------------------------------------------------------------------------------
GameLogic::AnitWallowState NW_ClientSessionGroup::GetPlayerWallowState()
{
	AnitWallowState eState = HEALTHINESS;
	if (m_isFcm)
	{
		if (m_onlineTime < 180)
		{
			eState = HEALTHINESS;
		}
		else if (m_onlineTime < 300)
		{
			eState = WEARINESS;
		}
		else
		{
			eState = WALLOW;
		}
	}
	return eState;
}
//---------------------------------------------------------------------------------
void NW_ClientSessionGroup::CheckAntiWallowState( float fCurrentTime )
{
	if (m_isOnline)
	{
		if (fCurrentTime - m_lastCalOnlineTime > 60.0f && (m_pkGameStageMachine->GetCurrentStateType() == ST_GAME_STAGE_SEL_CHARACTER || m_pkGameStageMachine->GetCurrentStateType() == ST_GAME_STAGE_GAME) )
		{
			++m_onlineTime;
			m_lastCalOnlineTime = fCurrentTime;

			//NotifyAntiWallowStateToGameServer();
			NotifyAntiWallowStateToUIInGame();
		}
	}
}
//---------------------------------------------------------------------------------
void NW_ClientSessionGroup::NotifyAntiWallowStateToGameServer()
{
	if (m_isOnline)
	{
		AnitWallowState eState = GetPlayerWallowState();
		if(m_pkGameSession && m_pkGameSession->GetPlayer())
		{
			m_pkGameSession->GetPlayer()->SendServerPlayerAntiWallowState(eState);
			m_pkGameSession->GetPlayer()->SetAntiWallowState(eState);
		}
	}
}
//---------------------------------------------------------------------------------
void NW_ClientSessionGroup::NotifyAntiWallowStateToUIInGame()
{
	if (m_isOnline)
	{
		AnitWallowState eState = GetPlayerWallowState();
		switch (eState)
		{
		case HEALTHINESS:
			NotifyHealthinessStateToUIInGame();
			break;
		case WEARINESS:
			NotifyWearinessStateToUIInGame();
			break;
		case WALLOW:
			NotifyWallowStateToUIInGame();
			break;
		default:
			break;
		}
	}
}
//---------------------------------------------------------------------------------
void NW_ClientSessionGroup::NotifyHealthinessStateToUIInGame()
{
	//健康状态
	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_ANTI_WALLOW_IN_GAME);
	kGameEvent.SetEventParam(EP_ANTIWALLOW_STATE, HEALTHINESS);
	kGameEvent.SetEventParam(EP_ANTIWALLOW_ONLINE_TIME,m_onlineTime);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_3, m_isFcm);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}
//---------------------------------------------------------------------------------
void NW_ClientSessionGroup::NotifyWearinessStateToUIInGame()
{
	//进入疲劳状态
	if(m_onlineTime >= 180 && m_onlineTime <  300)
	{
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_ANTI_WALLOW_IN_GAME);
		kGameEvent.SetEventParam(EP_ANTIWALLOW_STATE, WEARINESS);
		kGameEvent.SetEventParam(EP_ANTIWALLOW_ONLINE_TIME,m_onlineTime);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_3, m_isFcm);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
}
//---------------------------------------------------------------------------------
void NW_ClientSessionGroup::NotifyWallowStateToUIInGame()
{
	//进入防沉迷状态
	if(m_onlineTime > 300)
	{
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_ANTI_WALLOW_IN_GAME);
		kGameEvent.SetEventParam(EP_ANTIWALLOW_STATE, WALLOW);
		kGameEvent.SetEventParam(EP_ANTIWALLOW_ONLINE_TIME,m_onlineTime);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_3, m_isFcm);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
}
//---------------------------------------------------------------------------------
void NW_ClientSessionGroup::NotifyAntiWallowStateToUIInLogin()
{
	if (m_isOnline)
	{
		AnitWallowState eState = GetPlayerWallowState();
		switch (eState)
		{
		case HEALTHINESS:
			NotifyHealthinessStateToUIInLogin();
			break;
		case WEARINESS:
			NotifyWearinessStateToUIInLogin();
			break;
		case WALLOW:
			NotifyWallowStateToUIInLogin();
			break;
		default:
			break;
		}
	}
}
//---------------------------------------------------------------------------------
void NW_ClientSessionGroup::NotifyHealthinessStateToUIInLogin()
{
	if (m_pkGameSession)
	{
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_ANTI_WALLOW_IN_LOGIN);
		kGameEvent.SetEventParam(EP_ANTIWALLOW_STATE, HEALTHINESS);
		kGameEvent.SetEventParam(EP_ANTIWALLOW_ONLINE_TIME,m_onlineTime);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
}
//---------------------------------------------------------------------------------
void NW_ClientSessionGroup::NotifyWearinessStateToUIInLogin()
{
	if (m_pkGameSession)
	{
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_ANTI_WALLOW_IN_LOGIN);
		kGameEvent.SetEventParam(EP_ANTIWALLOW_STATE, WEARINESS);
		kGameEvent.SetEventParam(EP_ANTIWALLOW_ONLINE_TIME,m_onlineTime);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
}
//---------------------------------------------------------------------------------
void NW_ClientSessionGroup::NotifyWallowStateToUIInLogin()
{
	if (m_pkGameSession)
	{
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_ANTI_WALLOW_IN_LOGIN);
		kGameEvent.SetEventParam(EP_ANTIWALLOW_STATE, WALLOW);
		kGameEvent.SetEventParam(EP_ANTIWALLOW_ONLINE_TIME,m_onlineTime);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
}
//--------------------------------------------------------------------------------------------------------------
void NW_ClientSessionGroup::UpdateWaitEnterGame(float fCurrentTime)
{
	//if(fCurrentTime > m_fLastCheckEnterGameTime + 60.0f)
	//{
	//	SetNetState(NST_FAIL)
	///*	if(fCurrentTime > m_fLastWaitUserCountTime + 60.0f)
	//	{
	//		m_pkLoginSession->Disconnect();
	//		m_pkGameStageMachine->SetNextState(ST_GAME_STAGE_LOGIN);
	//		return;
	//	}
	//	
	//	NW_ClientSessionManager* pkSessionManager = NW_ClientSessionManager::GetSingletonPtr();
	//	int iServerID = GetCurGameServer();
	//

	//	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	//	MG_LC_Req_Enter_GameServer reqMsg;
	//	reqMsg.m_uiCharID = GetSelectChar();
	//	reqMsg.m_iServerID = GetCurCitizenShip();

	//	bool bCreateMsg = CreateMsgPacketHandler(this, CRYPT_ALGORITHM_NONE, hMsgHandler, MGPT_LC_REQ_ENTER_GAMESERVER, &reqMsg);
	//	if(bCreateMsg)
	//		m_pkLoginSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());

	//	GfxWriteLog(LOG_TAG_SYSTEM,LOG_SWITCH_SYSTEM,"发送排队请求[%f][%d]",fCurrentTime,reqMsg.m_iServerID);*/
	//}
}
//---------------------------------------------------------------------------------
NW_ClientSessionManager::NW_ClientSessionManager(void)
{
    m_pkClientSessionFactory = T_NEW_D NW_ClientSessionFactory;
	m_bUseCAS = 0;
	m_iPort = 0;

	m_iLoginType = LT_NORMAL;

	m_fLastUpdateProfileTime = 0.0f;
	m_fLastUpdateServerListTime = 0.0f;

	//获取版本号
	m_iServerUnionID = 0;

	NetBase::SetOnDisconnectFunc(_disconnectFunc);
}
//---------------------------------------------------------------------------------
NW_ClientSessionManager::~NW_ClientSessionManager(void)
{
	T_SAFE_DELETE(m_pkClientSessionFactory);
}
//---------------------------------------------------------------------------------
bool NW_ClientSessionManager::Init()
{
	LoadServerList();

	NW_ClientSessionGroup* pkClientSessionGroup = NULL;
	pkClientSessionGroup = _createSessionGroup();
	
	m_fLastUpdateProfileTime = 0.0f;
	m_fLastUpdateServerListTime = 0.0f;

	Protocol::MsgZipThreshold(0x30);
	return true;
}
//---------------------------------------------------------------------------------
bool NW_ClientSessionManager::UnInit()
{
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if(pkLogicManager)
	{
		LC_ClientGamePlayer* pkPlayer = pkLogicManager->GetOperationPlayer();
		if(pkPlayer)
			pkPlayer->SetSession(NULL);
	}
	
	NW_Login2ClientSession*		pkLoginSession = NULL;
	NW_Game2ClientSession*		pkGameSession = NULL;

	for (NW_ClientSessionGroupList::iterator itr = m_kClientSessionGroupList.begin(); 
		itr != m_kClientSessionGroupList.end(); ++itr)
	{
		pkLoginSession	   = (*itr)->m_pkLoginSession;
		pkGameSession	   = (*itr)->m_pkGameSession;
		RemoveSession(pkLoginSession->GetSessionIndex());
		RemoveSession(pkGameSession->GetSessionIndex());
		m_pkClientSessionFactory->FreeSession(pkLoginSession);
		m_pkClientSessionFactory->FreeSession(pkGameSession);
		T_SAFE_DELETE((*itr)->m_pkGameStageMachine);
		T_SAFE_DELETE (*itr);
	}
	m_kClientSessionGroupList.clear();
	m_kClientSessionGroupMap.clear();

	//清理服务器列表
	ClearLoginServerInfo();
	ClearGameServerInfo();

	return true;
}
//---------------------------------------------------------------------------------
void NW_ClientSessionManager::LoadServerList()
{
	TiXmlDocument kDocument;
	if ( !kDocument.LoadFile("Data\\Config\\ServerList.xml") )
		return;

	const char*	  pcStringAttribute;
	int			  kIntAttribute;
	
	TiXmlElement* pkLoginConfigElement = kDocument.FirstChildElement("loginconfig");
	if (pkLoginConfigElement)
	{
		//是否使用CAS平台登录
		if (pkLoginConfigElement->QueryIntAttribute("useCAS", &kIntAttribute) == TIXML_SUCCESS)
			m_bUseCAS	=	(kIntAttribute > 0);

		//登录方式
		//if (pkLoginConfigElement->QueryIntAttribute("type", &kIntAttribute) == TIXML_SUCCESS)
		//	m_iLoginType	=	kIntAttribute;
	}

	m_iLoginType = LT_NORMAL;

	TiXmlElement* pkFirstZoneElement = kDocument.FirstChildElement("zone");
	TiXmlElement* pkSelectZoneElement = pkFirstZoneElement;
	int iLastLoginZoneID = GetUserProfileIntVal("LastLoginZoneID");
	while(pkFirstZoneElement)
	{
		if (pkFirstZoneElement->QueryIntAttribute("id", &kIntAttribute) == TIXML_SUCCESS)
		{
			if(kIntAttribute == iLastLoginZoneID)
			{
				pkSelectZoneElement = pkFirstZoneElement;
				break;
			}
		}
		pkFirstZoneElement = pkFirstZoneElement->NextSiblingElement("zone");
	}

	if(NULL == pkSelectZoneElement)
		return;

	TiXmlElement* pkLoginServerElement = pkSelectZoneElement->FirstChildElement("login");
	while (pkLoginServerElement)
	{
		PS_GameSvrInfo kLoginZoneInfo;
		
		//区名称
		pcStringAttribute = pkLoginServerElement->Attribute("name");
		if(pcStringAttribute)
		{
			kLoginZoneInfo.m_szName = pcStringAttribute;
		}
	
		//区ID
		if (pkLoginServerElement->QueryIntAttribute("id", &kIntAttribute) == TIXML_SUCCESS)
		{
			kLoginZoneInfo.m_iServerID	=	kIntAttribute;
		}

		//区网关配置1
		TiXmlElement* pkGateWayElement1 = pkLoginServerElement->FirstChildElement("gateway1");
		if(pkGateWayElement1)
		{
			//IP 
			pcStringAttribute = pkGateWayElement1->Attribute("ip");
			if(pcStringAttribute)
			{
				kLoginZoneInfo.m_szIP = pcStringAttribute;
			}
			//Port
			if (pkGateWayElement1->QueryIntAttribute("port", &kIntAttribute) == TIXML_SUCCESS)
			{
				kLoginZoneInfo.m_iPort	=	kIntAttribute;
			}
		}
	
		AddLoginZoneInfo(kLoginZoneInfo);

		TiXmlElement* pkGameElement = pkLoginServerElement->FirstChildElement("game");
		while(pkGameElement)
		{
			PS_GameSvrInfo kLoginServerInfo;
			//服务器名称
			pcStringAttribute = pkGameElement->Attribute("name");
			if(pcStringAttribute)
			{
				kLoginServerInfo.m_szName = pcStringAttribute;
			}
			//服务器ID
			if (pkGameElement->QueryIntAttribute("id", &kIntAttribute) == TIXML_SUCCESS)
			{
				kLoginServerInfo.m_iServerID	=	kIntAttribute;
			}
			
			AddLoginServerInfo(kLoginZoneInfo.m_iServerID, kLoginServerInfo);
			pkGameElement = pkGameElement->NextSiblingElement("game");
		}
		pkLoginServerElement = pkLoginServerElement->NextSiblingElement("login");
	}
}
//---------------------------------------------------------------------------------
bool NW_ClientSessionManager::RecvMessage(MG_MessageBase& rkMessage)
{
	MG_MessageHandlerInterface::RecvMessage(rkMessage);

	BEGIN_MESSAGE_HANDLE
	MESSAGE_HANDLE(MGT_TIMER_TICK, _eventTimerTick)
	END_MESSAGE_HANDLE

	return true;
}
void NW_ClientSessionManager::AddLoginZoneInfo(const PS_GameSvrInfo& rkZoneInfo)
{
	m_kLoginZoneInfoList.push_back(rkZoneInfo);
}
//---------------------------------------------------------------------------------
void NW_ClientSessionManager::AddLoginServerInfo(int iZoneID, const PS_GameSvrInfo& rkServerInfo)
{
	m_kLoginServerInfoMap[iZoneID].push_back(rkServerInfo);

	m_kLoginServerIDMap[rkServerInfo.m_iServerID] = std::make_pair(iZoneID, m_kLoginServerInfoMap[iZoneID].size() - 1);
}
//---------------------------------------------------------------------------------
PS_GameSvrInfo* NW_ClientSessionManager::GetLoginServerInfo( int iServerID )
{
	if (m_kLoginServerIDMap.find(iServerID) == m_kLoginServerIDMap.end())
		return NULL;
	int iZoneID = m_kLoginServerIDMap[iServerID].first;
	int iServerIndex = m_kLoginServerIDMap[iServerID].second;

	return &m_kLoginServerInfoMap[iZoneID][iServerIndex];

	GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"无法找到LoginServer信息");
	return NULL;
}
//---------------------------------------------------------------------------------
PS_GameSvrInfo* NW_ClientSessionManager::GetLoginZoneInfo(int iZoneID)
{
	PS_ServerInfoListIter Iter;
	TRAV(m_kLoginZoneInfoList,Iter)
	{
		PS_GameSvrInfo& rkInfo = *Iter;
		if(rkInfo.m_iServerID == iZoneID)
			return &rkInfo;
	}

	GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"无法找到LoginServer信息");
	return NULL;
}
//---------------------------------------------------------------------------------
PS_ServerInfoList& NW_ClientSessionManager::GetLoginServerInfoList(int iZoneID)
{
	return m_kLoginServerInfoMap[iZoneID];
}
//---------------------------------------------------------------------------------
void NW_ClientSessionManager::ClearLoginServerInfo()
{
	m_kLoginZoneInfoList.clear();
	m_kLoginServerInfoMap.clear();
	m_kLoginServerIDMap.clear();
}
//---------------------------------------------------------------------------------
void NW_ClientSessionManager::ClearLoginServerStateData()
{
	for(int i = 0; i < m_kLoginZoneInfoList.size(); i++)
	{
		PS_GameSvrInfo& rkServerInfo = m_kLoginZoneInfoList[i];
		rkServerInfo.m_iCapacity = 0;
	}
}
//---------------------------------------------------------------------------------
void NW_ClientSessionManager::ClearLoginServerCharData()
{
	for(int i = 0; i < m_kLoginZoneInfoList.size(); i++)
	{
		PS_GameSvrInfo& rkServerInfo = m_kLoginZoneInfoList[i];
		rkServerInfo.m_iWaitCount = 0;
	}
}
//---------------------------------------------------------------------------------
void NW_ClientSessionManager::AddGameServerInfo(PS_GameSvrInfo& rkServerInfo)
{
	m_kGameServerInfoList.push_back(rkServerInfo);

	_updateRecommendServerInfo(rkServerInfo);
}
//---------------------------------------------------------------------------------
PS_GameSvrInfo* NW_ClientSessionManager::GetGameServerInfo( int iServerID )
{
	PS_ServerInfoListIter Iter;
	TRAV(m_kGameServerInfoList,Iter)
	{
		PS_GameSvrInfo& rkInfo = *Iter;
		if(rkInfo.m_iServerID == iServerID)
			return &rkInfo;

		for(std::vector<uint16_t>::const_iterator it = rkInfo.m_kSideline.begin(); it != rkInfo.m_kSideline.end(); ++it)
		{
			if((*it) == iServerID)
			{
				return &rkInfo;
			}
		}
	}
	
	//GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"无法找到GameServer信息");
	return NULL;
}
//---------------------------------------------------------------------------------
PS_ServerInfoList& NW_ClientSessionManager::GetGameServerInfoList()
{
	return m_kGameServerInfoList;
}
//---------------------------------------------------------------------------------
void NW_ClientSessionManager::ClearGameServerInfo()
{
	m_kGameServerInfoList.clear();
}
//---------------------------------------------------------------------------------
int NW_ClientSessionManager::GetGameServerInfoIndex(int iServerID,int iIndex)
{
	int ii = 0;
	int iGroup = 0;
	PS_ServerInfoListIter Iter;
	TRAV(m_kGameServerInfoList,Iter)
	{
		PS_GameSvrInfo& rkInfo = *Iter;
		if(rkInfo.m_iServerID == iServerID)
		{
			iGroup = rkInfo.m_iGroupID;
			break;
		}
	}
	
	if ( iGroup > 0 )
	{
		TRAV(m_kGameServerInfoList,Iter)
		{
			PS_GameSvrInfo& rkInfo = *Iter;
			if ( rkInfo.m_iGroupID == iGroup )
			{
				if ( iIndex == ii )
					return rkInfo.m_iServerID;
				
				ii++;	
			}
		}
	}
	return 0;
}
//---------------------------------------------------------------------------------
bool NW_ClientSessionManager::ResetSessionGroup(NetWork::NW_NetSessionBase* pkSession)
{
	NW_ClientSessionGroup* pkSessionGroup = GetSessionGroup(pkSession);
	int	eCurrentGameStateType = pkSessionGroup->m_pkGameStageMachine->GetCurrentStateType();
	
	
	//if(eCurrentGameStateType == ST_GAME_STAGE_GAME)
	{
		//从游戏中退出，直接退回登录界面
		if(IM_Manager::GetSingletonPtr())
		{
			IM_Manager::GetSingletonPtr()->Reset();
		}
		if(SE_SceneManager::GetSingletonPtr())
		{
			if( SE_SceneManager::GetSingletonPtr()->UnLoadWorldMap() )
			{
				GfxWriteLog( LOG_TAG_CLIENT_CRITICAL, LOG_SWITCH_CLIENT_CRITICAL , 
					"reset session unload current map %d",
					SE_SceneManager::GetSingleton().GetCurrentMapID() );
			}
		}
		
		//if(UE_UiEffectManager::GetSingletonPtr())UE_UiEffectManager::GetSingletonPtr()->UnInit();
		if (UI_AvatarManager::GetSingletonPtr())
		{
			UI_AvatarManager::GetSingletonPtr()->Reset();
		}
		if (UI_UIManager::GetSingletonPtr())
		{
			UI_UIManager::GetSingletonPtr()->Reset();
		}
		if(LC_ClientLogicManager::GetSingletonPtr())
		{
			LC_ClientLogicManager::GetSingletonPtr()->UnInit();
		}
		if(LC_ClientCollectionManager::GetSingletonPtr())
		{
			LC_ClientCollectionManager::GetSingletonPtr()->Reset();
		}
		if (LS_LuaStateFactory::GetSingletonPtr())
		{
			LS_LuaState* pkState = LS_LuaStateFactory::GetSingletonPtr()->GetLogicLuaState();
			if(UT_DirConfig::GetSingletonPtr()&&pkState)
			{
				StringType kScriptClientUILUAResstFileName = UT_DirConfig::GetSingletonPtr()->GetDir(CF_NODE_SCRIPT_UI_LUA_DATA_REST);
				pkState->DoFile(kScriptClientUILUAResstFileName);
			}
		}

		pkSessionGroup->SetInGameFlag(false);
		pkSessionGroup->m_pkLoginSession->Disconnect();
		pkSessionGroup->m_lastCalOnlineTime = -100.0f;

		pkSessionGroup->m_pkGameStageMachine->SetNextState(StateMachine::ST_GAME_STAGE_START);
		pkSessionGroup->m_pkGameStageMachine->Update(0, 0);
	}

	return true;
}
//------------------------------------------------------------------------
bool NW_ClientSessionManager::ExitGameSession(NW_Game2ClientSession* pkClientGameSession)
{
	NW_ClientSessionGroup* pkSessionGroup = GetSessionGroup(pkClientGameSession);
	if(!pkSessionGroup)
	{
		return false;
	}
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;
	MG_REQ_LeaveGame kMsg;
	bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_GC_REQ_LEAVE_GAME, &kMsg);
	if(bCreateMsg)
	{
		pkClientGameSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
	}
	pkSessionGroup->m_bRequestExitGame = true;
	return true;
}
//-------------------------------------------------------------------------
NW_ClientSessionGroup* NW_ClientSessionManager::GetSessionGroup(NW_NetSessionBase* pkSession)
{
	NW_ClientSessionGroupMapIter IterFind = m_kClientSessionGroupMap.find(pkSession->GetSessionIndex());
	if(IterFind == m_kClientSessionGroupMap.end())
	{
		return NULL;
	}
	else
	{
		return IterFind->second;
	}
}
//-------------------------------------------------------------------------
NW_ClientSessionGroup* NW_ClientSessionManager::GetGameSessionGroup()
{
	T_ASSERT(m_kClientSessionGroupList.size() == 1);
	if (m_kClientSessionGroupList.empty())
		return NULL;
	return (*m_kClientSessionGroupList.begin());
}
//------------------------------------------------------------------------
NW_NetCharSimpleInfo* NW_ClientSessionManager::FindNetCharSimpleInfo(const StringType& rkCharName, bool bNeedReverse)
{
	if(rkCharName.empty())
		return NULL;

	NW_NetCharSimpleInfoMapIter IterFind = m_kNetCharSimpleInfoMap.find(rkCharName);
	
	_addWaitProfileNetChar(rkCharName, bNeedReverse);
	
	if(IterFind != m_kNetCharSimpleInfoMap.end())
		return &(IterFind->second);
	else
		return NULL;
}
//------------------------------------------------------------------------
void NW_ClientSessionManager::AddNetCharSimpleInfo(const StringType& rkCharName,const NW_NetCharSimpleInfo& rkInfo)
{
	m_kNetCharSimpleInfoMap[rkCharName] = rkInfo;
}
//------------------------------------------------------------------------
void NW_ClientSessionManager::_addWaitProfileNetChar(const StringType& rkCharName, bool bNeedReverse)
{
	StringListIter IterFind = std::find(m_kWaitProfileNetCharNameList.begin(),m_kWaitProfileNetCharNameList.end(),rkCharName);
	if(IterFind != m_kWaitProfileNetCharNameList.end())
		return;

	if (bNeedReverse)
	{
		m_kWaitProfileNetCharNameList.push_front(rkCharName);
	}
	else
	{
		m_kWaitProfileNetCharNameList.push_back(rkCharName);
	}
}
//------------------------------------------------------------------------
void NW_ClientSessionManager::_updateWaitProfileNetCharList(float fCurrentTime)
{
	if(m_kWaitProfileNetCharNameList.empty())
		return;

	if(fCurrentTime < m_fLastUpdateProfileTime + 3.0f)
		return;

	m_fLastUpdateProfileTime = fCurrentTime;

	StringType kWaitCharName = m_kWaitProfileNetCharNameList.front();

	MG_REQ_Char_Profile reqMsg;	
	TPSTR2STDSTR(kWaitCharName, reqMsg.m_szCharName);

	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_REQ_CHAR_PROFILE,&reqMsg);
	if (bCreateMsg)
	{
		LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
		LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();

		if (NULL != pkGamePlayer)
		{
			pkGamePlayer->SendToServer( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize() );
			m_kWaitProfileNetCharNameList.pop_front();
		}
	}
}
//------------------------------------------------------------------------
void NW_ClientSessionManager::_updateServerList(float fCurrentTime)
{
	//更新服务器状态
	if(fCurrentTime > m_fLastUpdateServerListTime + 30.0f)
	{
		m_fLastUpdateServerListTime = fCurrentTime;

		MG_PACKET_ENCODE_HANDLER hMsgHandler;
		bool bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_LC_REQ_SERVER_LIST, NULL);
		if(bCreateMsg)
		{
			NW_ClientSessionGroup* pkGameSessionGroup = GetGameSessionGroup();
			if(pkGameSessionGroup)
			{
				if(pkGameSessionGroup->m_pkLoginSession)
					pkGameSessionGroup->m_pkLoginSession->Send(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
			}
		}
	}
}
//------------------------------------------------------------------------
void NW_ClientSessionManager::_eventTimerTick(MG_MessageBase& rkMessage)
{
	T_PROFILE("Update::NW_ClientSessionManager");

    NW_ClientNetManager::GetSingleton().Update();

	float fCurrentTime  = rkMessage.m_fParam1,
          fDeltaTime    = rkMessage.m_fParam2;
    {
		T_PROFILE("UPdate::NW_ClientSessionManager::SessionManagerUpdate");

        Update(fCurrentTime, fDeltaTime);
		_updateWaitProfileNetCharList(fCurrentTime);

		NW_ClientSessionGroup* pkGameSessionGroup = GetGameSessionGroup();
		if(pkGameSessionGroup)
		{
			pkGameSessionGroup->UpdateNetState(fCurrentTime);
			pkGameSessionGroup->m_pkGameStageMachine->Update(fCurrentTime,fDeltaTime);
		}
    }
}
//--------------------------------------------------------------------------------------------------------------
NW_ClientSessionGroup* NW_ClientSessionManager::_createSessionGroup(void)
{
	NW_ClientSessionGroup* pkClientSessionGroup   = T_NEW_D NW_ClientSessionGroup;
	pkClientSessionGroup->m_pkLoginSession        = (NW_Login2ClientSession*)m_pkClientSessionFactory->AllocSession(NST_CLIENT_TO_LOGINSERVER);
	pkClientSessionGroup->m_pkGameSession         = (NW_Game2ClientSession*)m_pkClientSessionFactory->AllocSession(NST_CLIENT_TO_GAMESERVER);
	pkClientSessionGroup->m_strSessionKey         = "T6SessionKey";
	pkClientSessionGroup->m_pkGameStageMachine    = T_NEW_D SM_GameStageMachine;
	pkClientSessionGroup->m_pkGameStageMachine->Init();
	pkClientSessionGroup->m_pkGameStageMachine->Start(StateMachine::ST_GAME_STAGE_START, GET_PROCESS_TIME, 0.0f);
	pkClientSessionGroup->m_pkGameStageMachine->SetSessionGroup(pkClientSessionGroup);

	m_kClientSessionGroupList.push_back(pkClientSessionGroup);
	
	m_kClientSessionGroupMap[pkClientSessionGroup->m_pkLoginSession->GetSessionIndex()]		= pkClientSessionGroup;
    m_kClientSessionGroupMap[pkClientSessionGroup->m_pkGameSession->GetSessionIndex()]		= pkClientSessionGroup;

	pkClientSessionGroup->m_pkLoginSession->SetConnectIPAddress(const_cast<char*>(m_strIP.c_str()), m_iPort);

	return pkClientSessionGroup;
}
//---------------------------------------------------------------------------------
PS_GameSvrInfo NW_ClientSessionManager::GetGateWayInfoByUserID(int iLoginServerID,long lUserID)
{
	PS_GameSvrInfo kGateWayInfo;
	//获取网关信息
	PS_GameSvrInfo* pkLoginServerInfo = GetLoginZoneInfo(iLoginServerID);
	if(NULL != pkLoginServerInfo)
	{
		kGateWayInfo.m_szIP = pkLoginServerInfo->m_szIP;
		kGateWayInfo.m_iPort =	pkLoginServerInfo->m_iPort;
	}
	return kGateWayInfo;
}

PS_GameSvrInfo NW_ClientSessionManager::GetGateWayInfoByUserID(char *szIP, int iPort, long lUserID)
{
	PS_GameSvrInfo kGateWayInfo;
	//获取网关信息, iPort == 8086
	kGateWayInfo.m_szIP = szIP;
	kGateWayInfo.m_iPort =	iPort;
	return kGateWayInfo;
}

//---------------------------------------------------------------------------------
void NW_ClientSessionManager::_updateRecommendServerInfo(const PS_GameSvrInfo& rkInfo)
{
	if (m_kLoginServerIDMap.find(rkInfo.m_iServerID) == m_kLoginServerIDMap.end())
		return;
	int iZoneID = m_kLoginServerIDMap[rkInfo.m_iServerID].first;
	int iServerIndex = m_kLoginServerIDMap[rkInfo.m_iServerID].second;

	PS_GameSvrInfo& rkServerInfo = m_kLoginServerInfoMap[iZoneID][iServerIndex];
	rkServerInfo.m_cStatus = rkInfo.m_cStatus;
	rkServerInfo.m_iCapacity = rkInfo.m_iCapacity;
	rkServerInfo.m_iServerID = rkInfo.m_iServerID;
	rkServerInfo.m_iType = rkInfo.m_iType;
	rkServerInfo.m_iUserCount = rkInfo.m_iUserCount;
	rkServerInfo.m_iWaitCount = rkInfo.m_iWaitCount;
}

int NW_ClientSessionManager::_disconnectFunc(int reason, void* param)
{
	GfxWriteLog( LOG_TAG_ERROR , LOG_SWITCH_ERROR ,	"[网络]检测到与服务器断开连接！" );
	long lParam = (long)param;
	if(lParam == 0 )
		lParam = RE_SERVER_KICK_USER_NORMAL;

	GfxWriteLog( LOG_TAG_SYSTEM , LOG_SWITCH_SYSTEM ,	"NW_ClientSessionManager::_disconnectFunc reason=[%d] param=[%d]",reason, lParam );
	SendMessageDialog(lParam,0,0,0,RE_DIALOG_CONFIRM_OFF_LINE,reason,lParam,0,0,0);
	return 1;
}




