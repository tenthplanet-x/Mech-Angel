#include "LC_ServerPlayerEnterLeaveManager.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerTaskManager.h"
#include "LC_Define.h"
#include "game_version.h"
#include "MG_Login2Client.h"
#include "NW_Helper.h"
#include "GameServerApp.h"
#include "NW_ServerSession.h"
#include "MG_LoginProtocol.h"
#include "LC_GameStory_Manager.h"
#include "LC_StoryLogic_Manager.h"
#include "MG_AssistProtocol.h"
#include "LC_Dule_Manager.h"
#include "LC_ServerGuildManager.h"
#include "LC_RankManager.h"
#include "UT_ServerHelper.h"
#include "oss_define.h"
#include "oss_internal_define.h"
#include "oss_360_define.h"
#include "LC_ServerMapLogic_Place.h"
#include "LC_ServerPlayerGroup.h"
#include "LC_ServerPlayerGroupManager.h"
#include "LC_ServerWarManager.h"
#include "LC_CrossRealmRaidManager.h"
#include "MG_Common.h"
#include "call_stack.h"
#include "CF_TransferArea.h"
#include "MG_Game2Alliance.h"
#include "gsnode.h"
#include "CF_WorldMapList.h"
#ifdef WIN32
#include "XDebugDumper.h"
#endif

using namespace Utility;
using namespace GameLogic;
using namespace Protocol;
using namespace CSVFile;

#define			MAX_CLIENT_IDLE_TIME		120

//////////////////////////////////////////////////////////////////////////////////
CrossRealmTeamData::CrossRealmTeamData(): m_mapID(0), m_mapTypeId(0), m_GlobalTeamId(0), m_LocalTeamId(0)
{
	m_PlayerInfo.clear();
}

void CrossRealmTeamData::dismissLocalTeam()
{
	if (m_LocalTeamId != INVALID_GROUP_ID)
	{
		LC_ServerPlayerGroupManager* pkPlayerGroupManager = SERVER_GET_PLAYER_GROUP_MANAGER();
		if (pkPlayerGroupManager != NULL)
		{
			LC_ServerPlayerGroup* group = (LC_ServerPlayerGroup*)pkPlayerGroupManager->FindPlayerGroupByGroupID(m_LocalTeamId);
			if (group != NULL)
			{
				pkPlayerGroupManager->Dismiss(group);
			}
		}
		m_LocalTeamId = INVALID_GROUP_ID;
	}
}

void CrossRealmTeamData::leaveLocalTeam(LC_ServerPlayer* player)
{
	if (m_LocalTeamId != INVALID_GROUP_ID)
	{
		//离开队伍
		LC_ServerPlayerGroupManager* pkPlayerGroupManager = SERVER_GET_PLAYER_GROUP_MANAGER();
		if(pkPlayerGroupManager != NULL)
		{
			bool destory = false;
			ResultType ret = pkPlayerGroupManager->Leave(player, destory);
			if (RE_SUCCESS == ret && destory)
			{
				m_LocalTeamId = INVALID_GROUP_ID;
			}
		}
	}
}

void CrossRealmTeamData::addLocalTeamMember(LC_ServerPlayer* player)
{
	LC_ServerPlayerGroupManager* pkPlayerGroupManager = SERVER_GET_PLAYER_GROUP_MANAGER();
	if (m_LocalTeamId != INVALID_GROUP_ID)
	{
		LC_PlayerGroupBase* group = pkPlayerGroupManager->FindPlayerGroupByGroupID(m_LocalTeamId);
		if (group != NULL)
		{
			group->AddPlayerToMemberList(player);
			player->SetPlayerGroupGlobalInfo(m_mapTypeId, m_GlobalTeamId);
		}
		else
		{
			m_LocalTeamId = INVALID_GROUP_ID;
		}
	}
	else
	{
		LC_ServerPlayerGroup* group = pkPlayerGroupManager->CreateGroup(player, E_PlayerTeamType_Cross);
		if (group == NULL)
		{
			m_LocalTeamId = INVALID_GROUP_ID;
		}
		else
		{
			m_LocalTeamId = group->GetID();
			player->SetPlayerGroupGlobalInfo(UT_ServerHelper::GetServerId(), m_GlobalTeamId);
		}
	}
}
//////////////////////////////////////////////////////////////////////////////////
void LC_UserAccountInfoCache::UpdateUserInfo(LC_ServerPlayer* pkPlayer)
{
	((LC_ServerPlayerManager*)SERVER_GET_PLAYER_MANAGER())->OnPlayerLoginIPSet(pkPlayer, pkPlayer->GetLoginIP(), m_uiLoginIP);
	pkPlayer->SetLoginIP(m_uiLoginIP);

	pkPlayer->SetClientIP(m_uiLastLoginIP);
	pkPlayer->SetClientMAC(m_ulClientMAC);
	//pkPlayer->SetBindPhone(m_bBindPhone > 0);

	pkPlayer->SetUserName(m_szAccount);
	pkPlayer->SetAdult(m_byAdult > 0);

	if (pkPlayer->PropellID() < m_uiCharPropellID)
	{
		pkPlayer->SetPropellID(m_uiCharPropellID);
	}

	pkPlayer->SetAccountLoginTime(m_iLoginTime);
	pkPlayer->setUserUUID(m_userKey);
	pkPlayer->GetPlayerPlatformInfoAsset().SetPlatformType(m_nLoginPlatform);
	pkPlayer->GetPlayerPlatformInfoAsset().SetLoginPlatformPage(m_nLoginPlatformPage);
}
//-------------------------------------------------------------------------------------
LC_ServerPlayerEnterLeaveManager::OfflinePlayerStruct::OfflinePlayerStruct(): m_charID(0)
{

}
//-------------------------------------------------------------------------------------
LC_ServerPlayer* LC_ServerPlayerEnterLeaveManager::OfflinePlayerStruct::GetPlayer()
{
	LC_ServerPlayerManager* serverPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if(serverPlayerManager != NULL)
	{
		return (LC_ServerPlayer*)(serverPlayerManager->FindPlayer(m_charID));
	}
	return NULL;
}
//-------------------------------------------------------------------------------------
LC_ServerPlayerEnterLeaveManager::LC_ServerPlayerEnterLeaveManager(void)
{
	m_lastCheckHoldTime				= 0;
	m_lastCheckPullLoginDataTime	= 0;
	m_lastCheckPullDataTime			= 0;
	m_lastCheckOfflineTime			= 0;
	m_lastCheckWaitingTime			= 0;
	m_lastCheckHeartBeatTime		= 0;
	m_lastCheckAllowEnterPlayerTime = 0;
	m_lastCheckDelayKickoffPlayerTime = 0;
	m_CrossRealmTeamPlayInfoMap.clear();
	m_CrossRealmPlayIndex.clear();
	m_waitingQueues.SetEnterLeaverManager(this);

	m_kUpdateTimer.SetTimer(GET_INTERVAL(CMN_INTER_SLOW));
	m_kCharIDPositionHold.clear();
}
//-------------------------------------------------------------------------------------
LC_ServerPlayerEnterLeaveManager::~LC_ServerPlayerEnterLeaveManager(void)
{
}
//-------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::Update(float fCurrentTime, float fDeltaTime)
{
	if (!m_kUpdateTimer.IsExpired())
	{
		return;
	}
	if (fCurrentTime - m_lastCheckOfflineTime > 60.f)
	{
		m_lastCheckOfflineTime = fCurrentTime;
		_checkOfflinePlayer(fCurrentTime, fDeltaTime);
	}
	if (fCurrentTime - m_lastCheckPullLoginDataTime > 5.0f)
	{
		m_lastCheckPullLoginDataTime = fCurrentTime;
		_checkPullLoginData(fCurrentTime, fDeltaTime);
	}
	if (fCurrentTime - m_lastCheckPullDataTime > 0.5f)
	{
		m_lastCheckPullDataTime = fCurrentTime;
		_updatePullDataFromDB(fCurrentTime, fDeltaTime);
	}
	if (fCurrentTime - m_lastCheckHoldTime > 60.0f)
	{
		m_lastCheckHoldTime = fCurrentTime;
		_checkHolder(fCurrentTime, fDeltaTime);
	}
	if (fCurrentTime - m_lastCheckWaitingTime > 3.0f)
	{
		m_lastCheckWaitingTime = fCurrentTime;
		_checkWaiting(fCurrentTime, fDeltaTime);
	}
	if (fCurrentTime - m_lastCheckHeartBeatTime > 60.0f)
	{
		m_lastCheckHeartBeatTime = fCurrentTime;
		_checkPlayerHeartbeat(fCurrentTime, fDeltaTime);
	}
	if (fCurrentTime - m_lastCheckAllowEnterPlayerTime > 60.0f)
	{
		m_lastCheckAllowEnterPlayerTime = fCurrentTime;
		_checkCrossRealmTeamPlayerList(fCurrentTime, fDeltaTime);
	}
	if (fCurrentTime - m_lastCheckDelayKickoffPlayerTime > 5.0f)
	{
		m_lastCheckDelayKickoffPlayerTime = fCurrentTime;
		_checkDelayKickoffPlayerList(fCurrentTime, fDeltaTime);
	}

	this->UpdateSwitchMapLinePlayer(fDeltaTime);
}
//-------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::UserEnterServer(user_id_type userID, const unique_id_impl& charID, NW_ServerSession* session)
{
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();

	static GameServerManager* pkServerMgr = GetGameServerApp()->GetServerManager();
	if (pkServerMgr->IsServerOpened())
	{
		bool bSuccessEnter = false;
		//断线就恢复
		bSuccessEnter = _enterOffLinePlayer(userID, charID, session);
		if (bSuccessEnter)
		{
			GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, userID, charID, "enter offline");
			return;
		}

		bSuccessEnter = _enterOnlinePlayer(userID, charID, session);
		if (bSuccessEnter)
		{
			GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, userID, charID, "enter duplicate");
			return;
		}
		bSuccessEnter = _enterHolderPlayer(userID, charID, session);
		if (bSuccessEnter)
		{
			GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, userID, charID, "enter holder");
			return;
		}
		bSuccessEnter = _enterNormalPlayer(userID, charID, session);
		if (bSuccessEnter)
		{
			GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, userID, charID, "enter normal");
			return;
		}

		Protocol::MG_GC_RLT_Login rltMsg;
		rltMsg.m_iCharID  = charID;
		rltMsg.m_lResult = RE_LOGIN_SERVER_BUSY;
		SendMsgToClientByGateway(session->PeerID(), MGPT_GC_RLT_LOGIN, &rltMsg, userID, 0);

		LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
		
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkServerPlayerManager->FindPlayer(charID);
		MG_GL_REQ_UserKey pkUserKey;
		if (pkPlayer)
		{
			pkUserKey.m_userKey = pkPlayer->getUserUUID();
		}
		else
		{
			pkUserKey.m_userKey = 0;
		}
		SendMsgToLogin(MGPT_SS_USER_LOGOUT, &pkUserKey, userID, charID, 0, GetGateWayServerID(session));
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "enter busy MGPT_GC_RLT_LOGIN  1 ", userID, charID);
	}
	else
	{
		Protocol::MG_GC_RLT_Login rltMsg;
		rltMsg.m_iCharID  = charID;
		rltMsg.m_lResult = RE_LOGIN_FAIL_WAIT_SERVER_OPEN;
		SendMsgToClientByGateway(session->PeerID(), MGPT_GC_RLT_LOGIN, &rltMsg, userID, 0);
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "enter busy MGPT_GC_RLT_LOGIN  2 ", userID, charID);
	}
}
//-------------------------------------------------------------------------------------
// nError: // ServerKickUserReason Begin // ServerKickUserReason End // SystemError.h
void LC_ServerPlayerEnterLeaveManager::UserLeaveServer(LC_ServerPlayer* player, GameLogic::LC_MapInOutType eInOutType, ResultType nError,bool bSendlogin)
{
#if 0
	if ( eInOutType == LMIOT_OFFLINE && IS_PLAYER_ID(player->GetID())) 
	{
		EXCEPTION_POINTERS* pExcInfo = NULL;
		_GetExceptionPointers( 0, &pExcInfo );
		gstrPrefix = "UserLeave_" + boost::lexical_cast<std::string>(eInOutType) + "_" + boost::lexical_cast<std::string>(player->GetID()) + "_Node_" +boost::lexical_cast<std::string>(UT_ServerHelper::GetServerId()) + "_" ;
		ExceptionFilter( pExcInfo);
	}
#endif

	if ( player == NULL )
		return;
	switch (eInOutType)
	{
		case LMIOT_LEAVE:
			{
				_userLeaveServer(player);
			}
			break;
		case LMIOT_RESET:
			{
				_userResetServer(player,bSendlogin);
			}
			break;
		case LMIOT_JUMP:
			{
				_userJumpServer(player);
			}
			break;
		case LMIOT_OFFLINE:
			{
				_userOffLineServer(player, false);
			}
			break;
		case LMIOT_KICKOFF:
			{
				_userKickOffServer(player);
			}
			break;
		case LMIOT_BEKICKOFFED:
			{
				_userBeKickedOffServer(player, nError);
			}
			break;
		case LMIOT_EXCEPTION:
			{
				_userBeKickedException(player);
			}
			break;
		case LMIOT_CLEANUP:
			{
				_userCleanUpServer(player);
			}
			break;
		case LMIOT_QUIT_ARENA:
			{
				_userQuitArenaServer(player);
			}
			break;
		case LMIOT_GIVEUP_ARENA:
			{
				_userGiveUpArenaServer(player);
			}
			break;
		case LMIOT_DELAY_KICKOFF:
			{
				_userDelayKickoffServer(player);
			}
			break;
		default:
			{
				GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "unkown leave type", __FUNCTION__, eInOutType, player->GetID(), player->GetInstance());
			}
			break;
	}

	// just for deatil log
}
//-------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::UserJumpServer(LC_ServerPlayer* player, int32_t serverID , GameLogic::LC_MapInOutType eInOutType)
{
	IF_A_NA_PLAYER(player){ return; }

	int32_t gatewayID = GetGateWayServerID(player);
	user_id_type userID = player->GetUserID();
	const unique_id_impl& charID = player->GetInstance();
	_notifyOtherServerUserJump(userID, charID, gatewayID);

	// loop for all buddy
	//_doUserLeaveImpl(player, eInOutType);
	mem::vector<int32_t> vecBuddyIDs;
	player->GetActivateBuddyIDs( vecBuddyIDs);
	for (int i = 0; i < vecBuddyIDs.size(); i++)
	{
		LC_ServerPlayer* pBuddy = player->GetActivateBuddyByID(vecBuddyIDs[i]);
		UserLeaveServer(pBuddy, eInOutType);
	}

	//跳线回原服务器
	MG_SS_JumpServer jumpMsg;
	jumpMsg.m_iDestServerID = serverID;
	SendMsgToLogin(MGPT_SS_USER_JUMPER, &jumpMsg, userID, player->GetInstance(), 0, gatewayID);
	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "UserJumpServer", player->GetUserID(), player->GetID(), player->GetInstance(), serverID, eInOutType);
}
//-------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::AllUserLeaveServer()
{
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if(NULL == pkPlayerManager )
		return;
	mem::vector<LC_ServerPlayer*> playerPtrs = pkPlayerManager->GetAllPlayerPtr();
	for (size_t i = 0 ; i < playerPtrs.size(); ++i)
	{
		UserLeaveServer(playerPtrs[i], LMIOT_LEAVE);
	}
}
//-------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::PlayerEnterGame(LC_ServerPlayer* pkPlayer, GameLogic::LC_MapInOutType eInType)
{
	IF_A_NA_PLAYER(pkPlayer){return;}

	user_id_type lUserID = pkPlayer->GetUserID();
	Protocol::MG_GC_RLT_Login rltMsg;
	rltMsg.m_iCharID  = pkPlayer->GetInstance();

	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();

	rltMsg.m_lResult = RE_SUCCESS;
	bool bSwitchMapLine = CheckSwitchMapLinePlayer(pkPlayer->GetUserID());
	if (!bSwitchMapLine)
	{
		pkPlayer->SendMsgToClient(MGPT_GC_RLT_LOGIN, &rltMsg);
		GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, " bSwitchMapLine MGPT_GC_RLT_LOGIN  3 ", lUserID, rltMsg.m_iCharID);
	}

	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "PlayerEnterGame", pkPlayer->GetID(), pkPlayer->GetUserID(), pkPlayer->GetInstance(), eInType);
	ResultType eEnterWorld = RE_FAIL;
	//在此判断是否是跨服进入
	CrossRealmData data;
	bool ret = CheckCrossRealmIndex(pkPlayer->GetID(), data);
	if (ret)
	{
		SetCrossRealmTargetServer(pkPlayer->GetID(), GetGameServerID());
		bool enter = false;
		if (data.m_CrossRealmType == CrossRealmType_Team)
		{
			pkPlayer->SendNoticeEnterGame();
			enter = CrossRealmTeamPlayerEnter(data.m_Parm1, pkPlayer);
			if (!enter)
			{
				CrossRealmTeamPlayerLeave(data.m_Parm1, pkPlayer, true);
			}
		}
		else if (data.m_CrossRealmType == CrossRealmType_GuildWar)
		{
			pkPlayer->SendNoticeEnterGame();
			enter = CrossRealmGuildWarPlayerEnter(pkPlayer);
			if (!enter)
			{
				CrossRealmGuildWarPlayerLeave(pkPlayer);
			}
		}
		else if (data.m_CrossRealmType == CrossRealmType_Raid)
		{
			pkPlayer->SendNoticeEnterGame();
			enter = CrossRealmRaidPlayerEnter(pkPlayer);
			if (!enter)
			{
				CrossRealmRaidPlayerLeave(pkPlayer);
			}
		}
		else if (data.m_CrossRealmType == CrossRealmType_GuildTransport)
		{
			pkPlayer->SendNoticeEnterGame();
			enter = GuildTransportPlayerEnter(pkPlayer);
			if (!enter)
			{
				GuildTransportPlayerLeave(pkPlayer);
			}
		}
		else if (data.m_CrossRealmType == CrossRealmType_Alliance)
		{
			enter = FrontierAlliancePlayerEnter(pkPlayer);
			if (!enter)
			{
				FrontierAlliancePlayerLeave(pkPlayer);
			}
		}
		else{}

		if (enter)
		{
			eEnterWorld = RE_SUCCESS;
		}
	}

	bool bInPlayerHold = false;
	bool bInitGameData = true;
	IF_A_VALID_PLAYER(pkPlayer)
	{
		if(eEnterWorld != RE_SUCCESS)
		{
			UserToken* pPlayerHold = GetPlayerHold(pkPlayer->GetID());
			if (pPlayerHold != NULL)
			{
				if(pPlayerHold->m_holdType == HoldDataType_FrontierAlliance)
				{
					int32_t nCurServerID = UT_ServerHelper::GetServerId();
					if(pPlayerHold->m_Parm1 == nCurServerID)
					{
						//本服
						CF_TransferArea::DataEntry* pkAreaData = CF_TransferArea::GetSingletonPtr()->GetEntryPtr(pPlayerHold->m_Parm2);
						if (pkAreaData != NULL)
						{
							pkPlayer->SendNoticeEnterGame();
							eEnterWorld = pkWorldManager->RequestChangePlayerLocation(pkPlayer, LMIOT_TRANSFER, 0, pkAreaData->_lMapID, pkAreaData->_kPos);
						}
					}
					else
					{
						//非本服
						MG_Req_SS_Alliance_Enter_Cross ssReq;
						ssReq.m_nTargetServerID = pPlayerHold->m_Parm1;
						ssReq.m_nAreaID = pPlayerHold->m_Parm2;
						ssReq.m_nCharID = pkPlayer->GetInstance();
						ssReq.m_nUserID = pkPlayer->GetUserID();
						ssReq.m_nFromServerID = nCurServerID;
						SendMsgToSociety(MGPT_SS_FALLIANCE_ENTER_CROSS_REQ, &ssReq, pkPlayer->GetUserID(), pkPlayer->GetInstance());
						eEnterWorld = RE_SUCCESS;
						bInitGameData = false;
					}
				}
				else
				{
					pkPlayer->SendNoticeEnterGame();
					eEnterWorld = pkWorldManager->RequestEnterServerWorld(pkPlayer, eInType, MT_MAX, 0, 0, UT_Vec3Int());
				}
				ClearPlayerHold(pkPlayer->GetID());
				bInPlayerHold = true;
			}
		}
	}

	IF_A_VALID_PLAYER(pkPlayer)
	{
		bool bSwitchMapLine = CheckSwitchMapLinePlayer(pkPlayer->GetUserID());
		//没有发生跨服等行为
		if(bInPlayerHold == false && ( eEnterWorld != RE_SUCCESS || bSwitchMapLine ) )
		{
			pkPlayer->SendNoticeEnterGame( RE_SUCCESS, bSwitchMapLine);
			if (!pkPlayer->FirstEnterWorld())
			{
				ChgPositionInfo* pPositionInfo = GetCharPositionHold(pkPlayer->GetInstance());
				if(pPositionInfo != NULL)
				{
					if(pPositionInfo->m_bPos == true)
					{
						eEnterWorld = pkWorldManager->RequestChangePlayerLocation(pkPlayer, LMIOT_TRANSFER, 0, pPositionInfo->m_iMapResID, pPositionInfo->m_rTargetPos);
					}
					else
					{
						eEnterWorld = TransferToServerByType(pkPlayer, pPositionInfo->m_iMapResID, pPositionInfo->m_nType, pPositionInfo->m_nIndex);
					}
					ClearCharPositionHold(pkPlayer->GetInstance());
				}
				else
				{
					eEnterWorld = pkWorldManager->RequestEnterServerWorld(pkPlayer, eInType, MT_MAX, 0, 0, UT_Vec3Int());
				}
				if ( bSwitchMapLine )
				{
					//pkPlayer->SetDirtyFlag(DIRTY_FLAG_PROPERTY, true);
					pkPlayer->SyncControllerPropertyToClient();
					pkPlayer->SyncActiviteBuddyProperty();

					pkPlayer->SetDirtyFlag(DIRTY_FLAG_SCORE, true);
				}
			}
			else
			{
				eEnterWorld = RE_SUCCESS;
			}
		}
		
		if (bInitGameData == true && RE_SUCCESS == eEnterWorld)
		{
			if (eInType != LMIOT_OFFLINE)
			{
				pkPlayer->InitGameData(false);
			}
			pkPlayer->GetFriendDataFromDB();
			pkPlayer->GetGuildDataFromDB();
			oss_login(pkPlayer);
			UT_ServerHelper::AddOnLinePlayer(pkPlayer);
			//UT_ServerHelper::ResetOnLinePlayer();
			//公司内部日志，不上报login
			oss_360_log_user_login(pkPlayer);
			time_t curSecond = time(NULL);
			pkPlayer->m_nSendOSSLoginTime = LC_Helper::GetDayEndTimestamp((uint32_t)curSecond, 1);
			pkPlayer->SetSessionOnlineTime(GET_PROCESS_TIME);
			//oss_internal_log_user_server_login(pkPlayer);
			pkPlayer->SetServerViewState(LC_ServerPlayer::SERVER_STATE_ENTERED);
			//通知其他服务器玩家进入
			_notifyOtherServerUserOn(lUserID, pkPlayer->GetInstance(), GetGateWayServerID(pkPlayer), GetGameServerApp()->GetServerManager()->GetActiveId(pkPlayer->GetCitizenship()));
		}
	}

	//进入不成功
	if (eEnterWorld != RE_SUCCESS)
	{
		//here the player memory still valid, only read!!!!!
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "enter fail! char enter  game failed", pkPlayer->GetOwnerCharName().c_str(), pkPlayer->GetUserID(), pkPlayer->GetInstanceCitizen(), pkPlayer->GetInstanceCharID(), eEnterWorld);
		return;
	}
	else
	{
	}
}
//----------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::AddCrossRealmGuildWarPlayerInfo(const unique_id_impl& charID, user_id_type userid, int maplogicid)
{
	AddPlayerHolder(charID, userid, (float)2 * 60, HoldDataType_CrossRealmGuildWar, 0);
	AddCrossRealmIndex(charID, userid, CrossRealmType_GuildWar, maplogicid, 0);
	return;
}
//---------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::DelCrossRealmGuildWarPlayerInfo(object_id_type playerid)
{
	ClearPlayerHold(playerid, HoldDataType_CrossRealmGuildWar);
	ClearCrossRealmIndex(playerid);
	return;
}
//----------------------------------------------------------------------------------------
bool LC_ServerPlayerEnterLeaveManager::CrossRealmGuildWarPlayerEnter(LC_ServerPlayer* player)
{
	IF_A_NA_PLAYER(player){return false;}

	CrossRealmData data;
	if (!CheckCrossRealmIndex(player->GetID(), data))
	{
		return false;
	}

	LC_WarManager* manager = (LC_WarManager*)SERVER_GET_GUILDWAR_MANAGER();
	ResultType ret = manager->EnterCrossRealmGuildWar(player);
	if (ret != RE_SUCCESS)
	{
		PlayerTryLeaveCrossRealm(player);
		return false;
	}
	return true;
}
//----------------------------------------------------------------------------------------
bool LC_ServerPlayerEnterLeaveManager::CrossRealmGuildWarPlayerLeave(LC_ServerPlayer* player, bool back)
{
	IF_A_NA_PLAYER(player){return false;}

	object_id_type playerid = player->GetID();
	DelCrossRealmGuildWarPlayerInfo(playerid);
	int srcserverid = UT_ServerHelper::GetSrcServerId(player);
	if (srcserverid != UT_ServerHelper::GetServerId() && back)
	{
		UserJumpServer(player, srcserverid, LMIOT_JUMP);
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::AddGuildTransportPlayerInfo(const unique_id_impl& charID, user_id_type userid, uint64_t guildid, int transportid, int maplogicid)
{
	AddPlayerHolder(charID, userid, (float)2 * 60, HoldDataType_GuildTransport, guildid, transportid);
	AddCrossRealmIndex(charID, userid, CrossRealmType_GuildTransport, maplogicid, guildid, transportid);
}
//---------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::DelGuildTransportPlayerInfo(object_id_type playerid)
{
	ClearPlayerHold(playerid, HoldDataType_GuildTransport);
	ClearCrossRealmIndex(playerid);
}
//---------------------------------------------------------------------------------------
bool LC_ServerPlayerEnterLeaveManager::GuildTransportPlayerEnter(LC_ServerPlayer* player)
{
	IF_A_NA_PLAYER(player){return false;}

	CrossRealmData data;
	if (!CheckCrossRealmIndex(player->GetID(), data))
	{
		return false;
	}
	LC_ServerGuildManager* manager = (LC_ServerGuildManager*)LC_ServerGuildManager::GetSingletonPtr();
	LC_ServerGuildInfo* guild = manager->GetGuildInfoByID(data.m_Parm1);
	if (guild == NULL)
	{
		PlayerTryLeaveCrossRealm(player);
		return false;
	}
	ResultType ret = guild->EnterTransport_Place(player, data.m_Parm2);
	if (ret != RE_SUCCESS)
	{
		PlayerTryLeaveCrossRealm(player);
		return false;
	}
	return true;
}
//----------------------------------------------------------------------------------------
bool LC_ServerPlayerEnterLeaveManager::GuildTransportPlayerLeave(LC_ServerPlayer* player, bool back)
{
	object_id_type playerid = player->GetID();
	DelGuildTransportPlayerInfo(playerid);
	int srcserverid = UT_ServerHelper::GetSrcServerId(player);
	if (srcserverid != UT_ServerHelper::GetServerId() && back)
	{
		UserJumpServer(player, srcserverid, LMIOT_JUMP);
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::AddCrossRealmRaidPlayerInfo(const unique_id_impl& charID, user_id_type userid, int raidid, int maplogicid)
{
	AddPlayerHolder(charID, userid, (float)2 * 60, HoldDataType_CrossRealmRaid, raidid);
	AddCrossRealmIndex(charID, userid, CrossRealmType_Raid, maplogicid, raidid);
}
//---------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::DelCrossRealmRaidPlayerInfo(object_id_type playerid)
{
	ClearPlayerHold(playerid, HoldDataType_CrossRealmRaid);
	ClearCrossRealmIndex(playerid);
}
//---------------------------------------------------------------------------------------
bool LC_ServerPlayerEnterLeaveManager::CrossRealmRaidPlayerEnter(LC_ServerPlayer* player)
{
	IF_A_NA_PLAYER(player){return false;}

	CrossRealmData data;
	if (!CheckCrossRealmIndex(player->GetID(), data))
	{
		return false;
	}
	LC_CrossRealmRaidManager* manager = (LC_CrossRealmRaidManager*)LC_CrossRealmRaidManager::GetSingletonPtr();
	ResultType ret = manager->EnterCrossRealmRaid_Place(data.m_Parm1, player);
	if (ret != RE_SUCCESS)
	{
		PlayerTryLeaveCrossRealm(player);
		return false;
	}
	return true;
}
//----------------------------------------------------------------------------------------
bool LC_ServerPlayerEnterLeaveManager::CrossRealmRaidPlayerLeave(LC_ServerPlayer* player, bool back)
{
	object_id_type playerid = player->GetID();
	DelCrossRealmRaidPlayerInfo(playerid);
	int srcserverid = UT_ServerHelper::GetSrcServerId(player);
	if (srcserverid != UT_ServerHelper::GetServerId() && back)
	{
		UserJumpServer(player, srcserverid, LMIOT_JUMP);
		return true;
	}
	return false;
}
//----------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::AddCrossRealmTeamPlayerInfo(mem::vector<CrossRealmTeam_Player>& playerData, int mapid, int maptype, int teamid)
{
	CrossRealmTeamData data;
	data.m_mapID = mapid;
	data.m_mapTypeId = maptype;
	data.m_GlobalTeamId = teamid;
	for (mem::vector<CrossRealmTeam_Player>::iterator beg = playerData.begin(); beg != playerData.end(); ++beg)
	{
		UserToken* token = GetPlayerHold(beg->m_playerID);
		if (token != NULL)
		{
			if (token->m_holdType == HoldDataType_CrossRealmTeam)
			{
				DelCrossRealmTeamPlayerInfo(token->m_Parm1, beg->m_playerID, true);
			}
			else if (token->m_holdType == HoldDataType_Team)
			{
				//从队伍中删除
				LC_ServerPlayerGroupManager* pkPlayerGroupManager = SERVER_GET_PLAYER_GROUP_MANAGER();
				bool destory = false;
				pkPlayerGroupManager->DelPlayer(token->m_Parm1, beg->m_playerID, destory);
				ClearPlayerHold(beg->m_playerID);
			}
			else if (token->m_holdType == HoldDataType_CrossRealmGuildWar)
			{
				DelCrossRealmGuildWarPlayerInfo(beg->m_playerID);
			}
			else
			{
				ClearPlayerHold(beg->m_playerID);
			}
		}
		AddPlayerHolder(beg->m_charID, beg->m_userID, (float)2 * 60, HoldDataType_CrossRealmTeam, teamid);
		AddCrossRealmIndex(beg->m_charID, beg->m_userID, CrossRealmType_Team, mapid, teamid);
		beg->m_Enter = false;
		beg->m_Timestamp = GET_CURRENT_TIMESTAMP_IN_SECONDS() + 1 * 60;
		data.m_PlayerInfo[GET_PLAYERID(beg->m_userID)] = *beg;
	}
	m_CrossRealmTeamPlayInfoMap[teamid] = data;
	return;
}
//-------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::DelCrossRealmTeamPlayerInfo(int teamid, object_id_type playerid, bool notify, bool bClearCross)
{
	CrossRealmTeamPlayDataMap::iterator it = m_CrossRealmTeamPlayInfoMap.find(teamid);
	if (it == m_CrossRealmTeamPlayInfoMap.end())
	{
		return;
	}
	CrossRealmTeamData& rTeamData = it->second;
	mem::map<object_id_type, CrossRealmTeam_Player>::iterator playerindex = rTeamData.m_PlayerInfo.find(playerid);
	if (playerindex != rTeamData.m_PlayerInfo.end())
	{
		unique_id_impl charID = playerindex->second.m_charID;
		rTeamData.m_PlayerInfo.erase(playerindex);

		//向组队服务器发送离开通知
		if (notify)
		{
			MG_GG_Notify_Group_CrossRealm_Leave_Team notify;
			notify.m_CharID = charID;
			SendMsgToSociety(MGPT_SS_CROSSREALM_GROUP_PLAYER_LEAVE, &notify);
		}
	}

	if(bClearCross == true)
	{
		ClearPlayerHold(playerid, HoldDataType_CrossRealmTeam);
		ClearCrossRealmIndex(playerid);
	}

	if (rTeamData.m_PlayerInfo.empty())
	{
		m_CrossRealmTeamPlayInfoMap.erase(it);
	}
}

void LC_ServerPlayerEnterLeaveManager::DelCrossRealmTeamAllPlayerInfo(int teamid, LC_ServerPlayer* player, bool notify)
{
	const unique_id_impl& charID = player->GetInstance();
	//处理队伍信息
	CrossRealmTeamData* teamdata = GetCrossRealmTeam(teamid);
	if (teamdata == NULL)
	{
		return;
	}
	teamdata->leaveLocalTeam(player);
	
	bool allleave = false;
	if(teamdata->m_LocalTeamId == INVALID_GROUP_ID)
	{
		allleave = true;
	}

	mem::vector<object_id_type> idList;
	mem::map<object_id_type, CrossRealmTeam_Player>::iterator beg = teamdata->m_PlayerInfo.begin();
	for (; beg != teamdata->m_PlayerInfo.end(); ++beg)
	{
		CrossRealmTeam_Player& rPlayer = beg->second;
		if(allleave == true || rPlayer.m_charID == charID)
		{
			idList.push_back(rPlayer.m_playerID);
		}
	}
	for(size_t i=0;i<idList.size();i++)
	{
		DelCrossRealmTeamPlayerInfo(teamid, idList[i], notify, player->GetID() == idList[i]);
	}
	if (allleave)
	{
		teamdata->m_mapID = 0;
	}
}
//-------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::DelCrossRealmTeam(int teamid)
{
	CrossRealmTeamPlayDataMap::iterator it = m_CrossRealmTeamPlayInfoMap.find(teamid);
	if (it == m_CrossRealmTeamPlayInfoMap.end())
	{
		return;
	}
	CrossRealmTeamData& rTeamData = it->second;
	for (mem::map<object_id_type, CrossRealmTeam_Player>::iterator playerbeg = rTeamData.m_PlayerInfo.begin();
		playerbeg != rTeamData.m_PlayerInfo.end(); ++playerbeg)
	{
		ClearPlayerHold(playerbeg->first, HoldDataType_CrossRealmTeam);
		ClearCrossRealmIndex(playerbeg->first);
	}
	rTeamData.dismissLocalTeam();
	m_CrossRealmTeamPlayInfoMap.erase(it);
}
//-------------------------------------------------------------------------------------
bool LC_ServerPlayerEnterLeaveManager::CrossRealmTeamPlayerEnter(int teamid, LC_ServerPlayer* player)
{
	IF_A_NA_PLAYER(player){return false;}

	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	CrossRealmTeamPlayDataMap::iterator it = m_CrossRealmTeamPlayInfoMap.find(teamid);
	if (it == m_CrossRealmTeamPlayInfoMap.end())
	{
		return false;
	}
	CrossRealmTeamData& rTeamData = it->second;
	mem::map<object_id_type, CrossRealmTeam_Player>::iterator playerindex = rTeamData.m_PlayerInfo.find(player->GetID());
	if (playerindex == rTeamData.m_PlayerInfo.end())
	{
		return false;
	}

	playerindex->second.m_Enter = true;
	playerindex->second.m_Timestamp = 0;
	if (rTeamData.m_mapID != 0)
	{
		m_CrossRealmPlayIndex[player->GetID()].m_data.m_MapLogicId = rTeamData.m_mapID;
		ResultType eResult = pkWorldManager->RequestChangePlayerLocation(player, LMIOT_TRANSFER, rTeamData.m_mapID, rTeamData.m_mapTypeId, UT_Vec3Int());
		if (eResult != RE_SUCCESS)
		{
			PlayerTryLeaveCrossRealm(player);
			return false;
		}
	}
	else
	{
		//创建副本进入
		LC_ServerMapLogic_Place* logic = (LC_ServerMapLogic_Place*)pkWorldManager->CreateServerMapLogic(0, rTeamData.m_mapTypeId, MT_EIGHTEEN_BRONZE_MAN, false);
		if (logic == NULL)
		{
			PlayerTryLeaveCrossRealm(player);
			return false;
		}
		logic->SetCrossRealm(true, CrossRealmType_Team, teamid);
		logic->Start();
		rTeamData.m_mapID = logic->GetMapLogicID();
		m_CrossRealmPlayIndex[player->GetID()].m_data.m_MapLogicId = rTeamData.m_mapID;
		ResultType eResult = pkWorldManager->RequestChangePlayerLocation(player, LMIOT_TRANSFER, rTeamData.m_mapID, rTeamData.m_mapTypeId, UT_Vec3Int());
		if (eResult != RE_SUCCESS)
		{
			PlayerTryLeaveCrossRealm(player);
			return false;
		}
	}
	rTeamData.addLocalTeamMember(player);
	return true;
}
//-------------------------------------------------------------------------------------
bool LC_ServerPlayerEnterLeaveManager::CrossRealmTeamPlayerLeave(int teamid, LC_ServerPlayer* player, bool notify, bool back)
{
	ClearPlayerHold(player->GetID(), HoldDataType_CrossRealmTeam);
	ClearCrossRealmIndex(player->GetID());
	int srcserverid = UT_ServerHelper::GetSrcServerId(player);
	if (srcserverid != UT_ServerHelper::GetServerId() && back)
	{
		UserJumpServer(player, srcserverid, LMIOT_JUMP);
		return true;
	}
	return false;
}
//-------------------------------------------------------------------------------------
CrossRealmTeamData* LC_ServerPlayerEnterLeaveManager::GetCrossRealmTeam(uint64_t teamid)
{
	CrossRealmTeamPlayDataMap::iterator it = m_CrossRealmTeamPlayInfoMap.find(teamid);
	if (it == m_CrossRealmTeamPlayInfoMap.end())
	{
		return NULL;
	}
	return &(it->second);
}
//----------------------------------------------------------------------------------------
bool LC_ServerPlayerEnterLeaveManager::PlayerTryLeaveCrossRealm(LC_ServerPlayer* player, bool notify, bool back)
{
	IF_A_NA_PLAYER(player){return false;}

	LC_ServerWorldManager* pkServerWorldManager = SERVER_GET_WORLD_MANAGER();
	CrossRealmDataGroup dataGroup;
	if (!CheckCrossRealmIndex(player->GetID(), dataGroup))
	{
		return false;
	}

	bool result = false;
	bool bclearGroup = true;
	for(CrossRealmDataList::iterator iter = dataGroup.m_doList.begin(); iter != dataGroup.m_doList.end(); iter ++)
	{
		CrossRealmData& data = *iter;
		if (data.m_MapLogicId != player->GetMapLogicID())
		{
			continue;
		}
		if (data.m_CrossRealmType == CrossRealmType_Team)
		{
			result = CrossRealmTeamPlayerLeave(data.m_Parm1, player, notify, back);
			bclearGroup = false;
		}
		else if (data.m_CrossRealmType == CrossRealmType_GuildWar)
		{
			result = CrossRealmGuildWarPlayerLeave(player, back);
		}
		else if (data.m_CrossRealmType == CrossRealmType_Raid)
		{
			result = CrossRealmRaidPlayerLeave(player, back);
		}
		else if (data.m_CrossRealmType == CrossRealmType_GuildTransport)
		{
			result = GuildTransportPlayerLeave(player, back);
		}
	}

	{
		CrossRealmData& data = dataGroup.m_data;
		if (data.m_MapLogicId != player->GetMapLogicID())
		{
			result = false;
			bclearGroup = false;
		}
		else if (data.m_CrossRealmType == CrossRealmType_Team)
		{
			result = CrossRealmTeamPlayerLeave(data.m_Parm1, player, notify, back);
			bclearGroup = false;
		}
		else if (data.m_CrossRealmType == CrossRealmType_GuildWar)
		{
			result = CrossRealmGuildWarPlayerLeave(player, back);
		}
		else if (data.m_CrossRealmType == CrossRealmType_Raid)
		{
			result = CrossRealmRaidPlayerLeave(player, back);
		}
		else if (data.m_CrossRealmType == CrossRealmType_GuildTransport)
		{
			result = GuildTransportPlayerLeave(player, back);
		}
		else if (data.m_CrossRealmType == CrossRealmType_Alliance)
		{
			result = FrontierAlliancePlayerLeave(player, back);
		}
		else
		{
			result = false;
			bclearGroup = false;
		}
	}

	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "try leave cross realm"
		, player->GetUserID(), player->GetID(), player->GetInstance(), notify, back, result, bclearGroup);
	//跨服到其它服，离开清除所有组队
	if(bclearGroup == true)
	{
		//跨服删除所有组队
		player->ClearPlayerGroup(LC_ServerPlayer::E_PlayerGroupMapOpt_OutMap, LC_ServerPlayer::E_PlayerGroupLimit_All);
	}
	return result;
}
//---------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::PlayerTryEnterCrossRealm(LC_ServerPlayer* player, int targetserver)
{
	IF_A_NA_PLAYER(player){return;}
	
	const unique_id_impl& charID = player->GetInstance();
	if (targetserver != UT_ServerHelper::GetServerId())		//目标服务器不在本机
	{
		static GameServerManager* pkServerMgr = GetGameServerApp()->GetServerManager();
		if (!pkServerMgr->IsServerOpened())
		{
			GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "enter cross not allowed, server not open!");
			return;
		}

		//解散所有组队状态
		player->ClearPlayerGroup(LC_ServerPlayer::E_PlayerGroupMapOpt_InMap, LC_ServerPlayer::E_PlayerGroupLimit_All);
		//添加Holder
		AddPlayerHolder(charID, player->GetUserID(), 120 * 60, HoldDataType_Normal, targetserver);
		//跳服务器
		UserJumpServer(player, targetserver, LMIOT_JUMP);
	}
	else													//目标服务器在本机
	{
		CrossRealmData data;
		if (!CheckCrossRealmIndex(player->GetID(), data))
		{
			return;
		}
		SetCrossRealmTargetServer(player->GetID(), targetserver);
		if (data.m_CrossRealmType == CrossRealmType_Team)
		{
			ClearPlayerHold(player->GetID());
			if(CrossRealmTeamPlayerEnter(data.m_Parm1, player) == false)
			{
				CrossRealmTeamPlayerLeave(data.m_Parm1, player);
			}
		}
		else if (data.m_CrossRealmType == CrossRealmType_GuildWar)
		{
			ClearPlayerHold(player->GetID());
			if(CrossRealmGuildWarPlayerEnter(player) == false)
			{
				CrossRealmGuildWarPlayerLeave(player);
			}
		}
		else if (data.m_CrossRealmType == CrossRealmType_Raid)
		{
			ClearPlayerHold(player->GetID());
			if(CrossRealmRaidPlayerEnter(player) == false)
			{
				CrossRealmRaidPlayerLeave(player);
			}
		}
		else if (data.m_CrossRealmType == CrossRealmType_GuildTransport)
		{
			ClearPlayerHold(player->GetID());
			if(GuildTransportPlayerEnter(player) == false)
			{
				GuildTransportPlayerLeave(player);
			}
		}
	}
}
//-------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::CrossRealmEnd(CrossRealmType crossrealmtype, uint64_t parm1, int parm2, int parm3)
{
	if (crossrealmtype == CrossRealmType_Team)
	{
		CrossRealmTeamData* teamdata = GetCrossRealmTeam(parm1);
		if (teamdata == NULL)
		{
			return;
		}
		mem::set<object_id_type> delplayer;
		mem::map<object_id_type, CrossRealmTeam_Player>::iterator playerbeg = teamdata->m_PlayerInfo.begin();
		for (; playerbeg != teamdata->m_PlayerInfo.end(); ++playerbeg)
		{
			if (!(playerbeg->second.m_Enter))
			{
				delplayer.insert(playerbeg->first);
			}
		}
		mem::set<object_id_type>::iterator actorbeg = delplayer.begin();
		for (; actorbeg != delplayer.end(); ++actorbeg)
		{
			//超时玩家需要剔除gateway
			KickOffCrossRealmPlayer(*actorbeg);
		}
		//发送，结束通知
		MG_GG_Req_Group_Raid_End req;
		req.m_LogicId = teamdata->m_mapID;
		req.m_mapType = teamdata->m_mapTypeId;
		req.m_TeamId = teamdata->m_GlobalTeamId;
		req.m_Success = (parm2 != 0);
		req.m_PassTime = parm3;
		SendMsgToSociety(MGPT_SS_CROSSREALM_TEAM_END_REQ, &req);
	}
	else if (crossrealmtype == CrossRealmType_GuildWar)
	{
		mem::vector<object_id_type> eraseHolder;
		for (UserPlaceHolder::iterator it = m_placeHolders.begin(); it != m_placeHolders.end(); ++it)
		{
			UserToken& user = (*it).second;
			if (user.m_holdType == HoldDataType_CrossRealmGuildWar)
			{
				eraseHolder.push_back((*it).first);
			}
		}
		for (size_t i = 0 ; i < eraseHolder.size(); ++i)
		{
			KickOffCrossRealmPlayer(eraseHolder[i]);
		}
	}
	else if (crossrealmtype == CrossRealmType_Raid)
	{
		mem::vector<object_id_type> eraseHolder;
		for (UserPlaceHolder::iterator it = m_placeHolders.begin(); it != m_placeHolders.end(); ++it)
		{
			UserToken& user = (*it).second;
			if (user.m_holdType == HoldDataType_CrossRealmRaid && user.m_Parm1 == parm1)
			{
				eraseHolder.push_back((*it).first);
			}
		}
		for (size_t i = 0 ; i < eraseHolder.size(); ++i)
		{
			KickOffCrossRealmPlayer(eraseHolder[i]);
		}
		LC_CrossRealmRaidManager* manager = (LC_CrossRealmRaidManager*)LC_CrossRealmRaidManager::GetSingletonPtr();
		manager->CrossRealmRaidEndNotify(parm1);
	}
	else if (crossrealmtype == CrossRealmType_GuildTransport)
	{
		mem::vector<object_id_type> eraseHolder;
		for (UserPlaceHolder::iterator it = m_placeHolders.begin(); it != m_placeHolders.end(); ++it)
		{
			UserToken& user = (*it).second;
			if (user.m_holdType == HoldDataType_GuildTransport && user.m_Parm1 == parm1 && user.m_Parm2 == parm2)
			{
				eraseHolder.push_back((*it).first);
			}
		}
		for (size_t i = 0 ; i < eraseHolder.size(); ++i)
		{
			KickOffCrossRealmPlayer(eraseHolder[i]);
		}
	}
}
//-------------------------------------------------------------------------------------
bool LC_ServerPlayerEnterLeaveManager::AddCrossRealmIndex(const unique_id_impl& charID, user_id_type userid, CrossRealmType crossrealmtype, int maplogicid, uint64_t parm1, int parm2, int parm3)
{
	CrossRealmData data;
	data.m_userID = userid;
	data.m_playerID = GET_PLAYERID(userid);
	data.m_charID = charID;
	data.m_CrossRealmType = crossrealmtype;
	data.m_Parm1 = parm1;
	data.m_Parm2 = parm2;
	data.m_Parm3 = parm3;
	data.m_MapLogicId = maplogicid;

	CrossRealmPlayIndexMap::iterator iter = m_CrossRealmPlayIndex.find(data.m_playerID);
	if(iter == m_CrossRealmPlayIndex.end())
	{
		m_CrossRealmPlayIndex[data.m_playerID].m_data = data;
		m_CrossRealmPlayIndex[data.m_playerID].m_doList.clear();
	}
	else
	{
		CrossRealmDataGroup& rGroup = iter->second;
		rGroup.m_doList.push_back(rGroup.m_data);
		rGroup.m_data = data;
	}
	return true;
}

void LC_ServerPlayerEnterLeaveManager::SetCrossRealmTargetServer(object_id_type playerid, int serverId)
{
	CrossRealmPlayIndexMap::iterator it = m_CrossRealmPlayIndex.find(playerid);
	if (it == m_CrossRealmPlayIndex.end())
	{
		return;
	}
	it->second.m_data.m_TargetServerID = serverId;
}
//----------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::ClearCrossRealmIndex(const unique_id_type& charID)
{
	ClearCrossRealmIndex(QueryPlayerID(charID));
}

void LC_ServerPlayerEnterLeaveManager::ClearCrossRealmIndex(object_id_type playerid)
{
	m_CrossRealmPlayIndex.erase(playerid);
}
//---------------------------------------------------------------------------------------
bool LC_ServerPlayerEnterLeaveManager::IsInCrossRealmIndex(object_id_type playerid)
{
	return m_CrossRealmPlayIndex.find(playerid) != m_CrossRealmPlayIndex.end();
}

bool LC_ServerPlayerEnterLeaveManager::IsInCrossRealmIndex(const unique_id_impl& id)
{
	for(CrossRealmPlayIndexMap::iterator iter = m_CrossRealmPlayIndex.begin(); iter != m_CrossRealmPlayIndex.end(); iter ++)
	{
		CrossRealmDataGroup& rGroup = iter->second;
		if(rGroup.m_data.m_charID == id)
		{
			return true;
		}
	}
	return false;
}

bool LC_ServerPlayerEnterLeaveManager::CheckCrossRealmIndex(object_id_type playerid, CrossRealmDataGroup& data)
{
	CrossRealmPlayIndexMap::iterator it = m_CrossRealmPlayIndex.find(playerid);
	if (it == m_CrossRealmPlayIndex.end())
	{
		return false;
	}
	data = it->second;
	return true;
}

bool LC_ServerPlayerEnterLeaveManager::CheckCrossRealmIndex(object_id_type playerid, CrossRealmData& data)
{
	CrossRealmPlayIndexMap::iterator it = m_CrossRealmPlayIndex.find(playerid);
	if (it == m_CrossRealmPlayIndex.end())
	{
		return false;
	}
	data = it->second.m_data;
	return true;
}
bool LC_ServerPlayerEnterLeaveManager::CheckCrossRealmType(object_id_type playerid, CrossRealmType& crossrealmtype)
{
	CrossRealmPlayIndexMap::iterator it = m_CrossRealmPlayIndex.find(playerid);
	if (it == m_CrossRealmPlayIndex.end())
	{
		return false;
	}
	crossrealmtype = it->second.m_data.m_CrossRealmType;
	return true;
}
//----------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::KickOffCrossRealmPlayer(object_id_type playerid)
{
	CrossRealmData data;
	bool ret = CheckCrossRealmIndex(playerid, data);
	if (!ret)
	{
		return;
	}
	if (data.m_CrossRealmType == CrossRealmType_Team)
	{
		SendMsgToLogin(MGPT_SS_ROLE_LEAVE_GAME, NULL, data.m_userID, data.m_charID);
		DelCrossRealmTeamPlayerInfo(data.m_Parm1, playerid, true);
	}
	else if (data.m_CrossRealmType == CrossRealmType_GuildWar)
	{
		SendMsgToLogin(MGPT_SS_ROLE_LEAVE_GAME, NULL, data.m_userID, data.m_charID);
		DelCrossRealmGuildWarPlayerInfo(playerid);
	}
	else if (data.m_CrossRealmType == CrossRealmType_Raid)
	{
		SendMsgToLogin(MGPT_SS_ROLE_LEAVE_GAME, NULL, data.m_userID, data.m_charID);
		DelCrossRealmRaidPlayerInfo(playerid);
	}
	else if (data.m_CrossRealmType == CrossRealmType_GuildTransport)
	{
		SendMsgToLogin(MGPT_SS_ROLE_LEAVE_GAME, NULL, data.m_userID, data.m_charID);
		DelGuildTransportPlayerInfo(playerid);
	}
}
//-------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::DelCrossRealmPlayer(object_id_type playerid, bool notify)
{
	CrossRealmData data;
	bool ret = CheckCrossRealmIndex(playerid, data);
	if (!ret)
	{
		return;
	}
	if (data.m_CrossRealmType == CrossRealmType_Team)
	{
		DelCrossRealmTeamPlayerInfo(data.m_Parm1, playerid, notify);
	}
	else if (data.m_CrossRealmType == CrossRealmType_GuildWar)
	{
		DelCrossRealmGuildWarPlayerInfo(playerid);
	}
	else if (data.m_CrossRealmType == CrossRealmType_Raid)
	{
		DelCrossRealmRaidPlayerInfo(playerid);
	}
	else if (data.m_CrossRealmType == CrossRealmType_GuildTransport)
	{
		DelGuildTransportPlayerInfo(playerid);
	}
}
//-------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::LoadDataFromDB(LC_ServerPlayer* player, bool success)
{
	Protocol::MG_GC_RLT_Login rltMsg;
	rltMsg.m_iCharID  = player->GetInstance();
	if (!_checkServerIDAndTransServer(player))
	{
		success = false;
	}
	if (success)
	{
		player->SetServerViewState(LC_ServerPlayer::SERVER_STATE_LOGINED);
		if (player->IsInstance())
		{
			rltMsg.m_lResult = RE_SUCCESS;
			//向client返回验证结果
			uint32_t lUserID =  player->GetUserID();
			bool bSwitchMapLine = CheckSwitchMapLinePlayer(lUserID);
			if (!bSwitchMapLine)
			{
				player->SendMsgToClient(MGPT_GC_RLT_LOGIN, &rltMsg);
				GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, " LoadDataFromDB  bSwitchMapLine MGPT_GC_RLT_LOGIN  4 ", lUserID);
			}
			if (GetGameServerApp()->IsBonusRole(player->GetInstanceUniqueID()) &&
				player->GetLastOfflineTime() + 7 * ONE_DAY_SECONDS < GET_CURRENT_TIMESTAMP_IN_SECONDS())
			{
				MG_GL_Freeze_Player reqMsg;
				reqMsg.m_iTime = 2 * 365 * ONE_DAY_SECONDS;
				reqMsg.m_iType = 1;
				SendMsgToLogin(MGPT_GL_SYNC_FREEZE_PLAYER_REQ, &reqMsg, player->GetUserID(), player->GetInstance());

				UserLeaveServer(player, LMIOT_KICKOFF);
				return;
			}
		}
		GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "[%d][LOGIN STEP 3]LoadDataFromDB userid:%d, Instance:%d, success:%d", UT_ServerHelper::GetServerId(), player->GetUserID(), player->GetInstance(),success);
		_pullDataFromDBSuccess(player);
	}
	else
	{
		if (player->IsInstance())
		{
			rltMsg.m_lResult = RE_LOGIN_FAIL;
			player->SendMsgToClient(MGPT_GC_RLT_LOGIN, &rltMsg);
			GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, " LoadDataFromDB  bSwitchMapLine MGPT_GC_RLT_LOGIN  5 ", player->GetUserID());
			UserLeaveServer(player, LMIOT_LEAVE);
		}
	}
}
//-------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::LoadDataFromLogin(LC_ServerPlayer* player)
{
	_pullDataFromLoginSuccess(player);
	if (player->GetLoadDataFromDBFlag())
	{
		Protocol::MG_GC_RLT_Login rltMsg;
		rltMsg.m_iCharID  = player->GetInstance();
		player->SetServerViewState(LC_ServerPlayer::SERVER_STATE_LOGINED);
		rltMsg.m_lResult = RE_SUCCESS;
		//向client返回验证结果
		player->SendMsgToClient(MGPT_GC_RLT_LOGIN, &rltMsg);
		//LoadDataFromDB(player, true);
		GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, player->GetID(), player->GetUserID(), player->GetInstance(), "LoadDataFromLogin already login from db  MGPT_GC_RLT_LOGIN 6 " );
	}
	else
	{
		_pullDataFromDB(player);
	}
}
//-------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::AddPlayerHolder(const unique_id_impl& charID, user_id_type userid, float holdTime, HoldDataType holdtype, uint64_t parm1, int parm2, int parm3)
{
	UserToken userToken;
	userToken.m_playerID = GET_PLAYERID(userid);
	userToken.m_userID = userid;
	userToken.m_charID = charID;
	userToken.m_addTime = GET_PROCESS_TIME;
	userToken.m_holdTime = holdTime;
	userToken.m_holdType = holdtype;
	userToken.m_Parm1 = parm1;
	userToken.m_Parm2 = parm2;
	userToken.m_Parm3 = parm3;

	m_placeHolders[userToken.m_playerID] = userToken;

	m_kPlayerID2CharIDIndices[userToken.m_playerID] = userToken.m_charID;
	m_kCharID2PlayerIDIndices[charID] = userToken.m_playerID;
}
//-------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::UpdatePlayerHolder(const unique_id_impl& charID, user_id_type userid, float holdTime, HoldDataType holdtype)
{
	object_id_type playerID = GET_PLAYERID(userid);
	UpdatePlayerHolder(playerID, holdTime, holdtype);
}

void LC_ServerPlayerEnterLeaveManager::UpdatePlayerHolder(object_id_type playerID, float holdTime, HoldDataType holdtype)
{
	UserPlaceHolder::iterator it = m_placeHolders.find(playerID);
	if (it != m_placeHolders.end())
	{
		UserToken& userToken = it->second;
		if(userToken.m_holdType == holdtype)
		{
			userToken.m_addTime = GET_PROCESS_TIME;
			userToken.m_holdTime = holdTime;
		}
	}
}
//-------------------------------------------------------------------------------------
LC_ServerPlayerEnterLeaveManager::UserToken* LC_ServerPlayerEnterLeaveManager::GetPlayerHold(object_id_type playerID)
{
	UserPlaceHolder::iterator it = m_placeHolders.find(playerID);
	if (it == m_placeHolders.end())
	{
		return NULL;
	}
	return &(it->second);
}
//-----------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::ClearPlayerHold(const unique_id_type& charID, HoldDataType holdtype)
{
	ClearPlayerHold(QueryPlayerID(charID), holdtype);
}

void LC_ServerPlayerEnterLeaveManager::ClearPlayerHold(object_id_type playerID, HoldDataType holdtype)
{
	UserPlaceHolder::iterator it = m_placeHolders.find(playerID);
	if (it == m_placeHolders.end())
	{
		return;
	}
	if (holdtype != HoldDataType_All && it->second.m_holdType != holdtype)
	{
		return;
	}
	m_placeHolders.erase(it);
}
//---------------------------------------------------------------------------------------
int LC_ServerPlayerEnterLeaveManager::GetHoldCount(HoldDataType holdtype)
{
	int count = 0;
	if (holdtype == HoldDataType_All)
	{
		count = (int)m_placeHolders.size();
	}
	else
	{
		UserPlaceHolder::iterator beg = m_placeHolders.begin();
		for (; beg != m_placeHolders.end(); ++beg)
		{
			if (beg->second.m_holdType == holdtype)
			{
				++count;
			}
		}
	}
	return count;
}
object_id_type LC_ServerPlayerEnterLeaveManager::QueryPlayerID(const unique_id_impl& charID)
{
	CharID2PlayerIDIndices::iterator it = m_kCharID2PlayerIDIndices.find(charID);
	return (it!=m_kCharID2PlayerIDIndices.end()) ? it->second : INVALID_ACTOR_ID;
}
unique_id_type LC_ServerPlayerEnterLeaveManager::QueryCharID(object_id_type playerID)
{
	PlayerID2CharIDIndices::iterator it = m_kPlayerID2CharIDIndices.find(playerID);
	return (it!=m_kPlayerID2CharIDIndices.end()) ? it->second : INVALID_ACTOR_ID;
}

void LC_ServerPlayerEnterLeaveManager::SyncCrossData(const unique_id_impl& charID)
{
	object_id_type playerID = QueryPlayerID(charID);
	if (INVALID_ACTOR_ID == playerID)
	{
		return;
	}

	user_id_type userID = INVALID_USER_ID;
	CrossRealmData data;
	if (CheckCrossRealmIndex(playerID, data))
	{
		userID = data.m_userID;
	}
	else
	{
		LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(charID);
		if (NULL == pkPlayer)
		{
			return;
		}
		userID = pkPlayer->GetUserID();
	}
}
//-------------------------------------------------------------------------------------
bool LC_ServerPlayerEnterLeaveManager::IsServerFull(void)
{
	return (int)GetGameServerApp()->GetServerInfo().m_iCapacity <= GetServerPlayerCount();
}
//-------------------------------------------------------------------------------------
int LC_ServerPlayerEnterLeaveManager::GetServerPlayerCount()
{
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	return pkPlayerManager->GetPlayerCount() + (int)m_placeHolders.size();
}
//-------------------------------------------------------------------------------------
int LC_ServerPlayerEnterLeaveManager::GetServerRealPlayerCount()
{
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	return pkPlayerManager->GetPlayerCount();
}
//-------------------------------------------------------------------------------------
int LC_ServerPlayerEnterLeaveManager::GetServerOnlinePlayerCount()
{
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	int iOnlinePlayerCount = 0;
	const LC_PlayerIDPtrMap& allPlayers = pkPlayerManager->GetPlayerIDMap();
	LC_PlayerIDPtrMap::const_iterator cit = allPlayers.begin();
	for (; cit != allPlayers.end(); ++cit)
	{
		LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)cit->second;
		if (pkServerPlayer && !pkServerPlayer->IsOffline())
		{
			++iOnlinePlayerCount;
		}
	}
	return iOnlinePlayerCount;
}
//-------------------------------------------------------------------------------------
int LC_ServerPlayerEnterLeaveManager::GetWaitingCount()
{
	return (int)m_waitingQueues.GetWaitingQueueLenth();
}


//-------------------------------------------------------------------------------------
bool LC_ServerPlayerEnterLeaveManager::DestoryPlayerNotSaveDB(LC_ServerPlayer* player, GameLogic::LC_MapInOutType eOutType)
{
	const unique_id_impl& instance = player->GetInstance();
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "<%d>LC_ServerPlayerEnterLeaveManager::_DestoryPlayerNotSaveDB! c_cid:%d, c_citizenship:%d, c_uid:%d, id:%d, type: %d",
		UT_ServerHelper::GetServerId(), instance.detail(), instance.catagory(), player->GetUserID(), player->GetID(), eOutType);
	IF_A_NA_PLAYER(player)
	{
		return false;
	}

	IF_A_VALID_PLAYER(player)
	{
		player->SetNormalQuit(true);
		player->m_kSkillStateMap->ClearStatesOnLeaveGame();

		_removePlayerFromServerWorld(player, eOutType);

		LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
		pkPlayerManager->DestroyPlayer(player);
	}
	return true;
}

//-------------------------------------------------------------------------------------
bool LC_ServerPlayerEnterLeaveManager::_doUserLeaveImpl(LC_ServerPlayer* player, GameLogic::LC_MapInOutType eOutType, ResultType nError)
{
	const unique_id_impl& instance = player->GetInstance();
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "<%d>LC_ServerPlayerEnterLeaveManager::_doUserLeaveImpl! c_cid:%d, c_citizenship:%d, c_uid:%d, id:%d, eOutType:%d, reason:%d", 
		UT_ServerHelper::GetServerId(), instance.detail(), instance.catagory(), player->GetUserID(), player->GetID(),  eOutType, nError);
	IF_A_NA_PLAYER(player)
	{
		return false;
	}

	// TODO:
	// This logic depends on GetOwnerPlayer, check it carefully!
	if (player->IsMainController())
	{
		player->ForceSaveSystemData();

		UT_ServerHelper::RemoveOffLinePlayer(player);
		oss_logout(player);
		oss_360_log_user_logout( player );
		oss_internal_log_user_server_logout(player);
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "_doUserLeaveImpl! c_cid:%d, c_citizenship:%d, c_uid:%d, id:%d, eOutType:%d",
			instance.detail(), instance.catagory(), player->GetUserID(), player->GetID(), eOutType);
	}
	IF_A_VALID_PLAYER(player)
		player->LeaveGame( eOutType == LMIOT_EXCEPTION,nError );

	_removePlayerFromServerWorld(player, eOutType);

	if (player->IsMainController())
	{		
		//玩家下线，从gamesetory删除统计数据
		GameStory::LC_GameStory_Manager *pkGSMgr = SERVER_GET_GAMESTORY_MANAGER;
		pkGSMgr->RemovePlayerCmnStats(player);
	}

	IF_A_VALID_PLAYER(player)
	{
		
		LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
		pkPlayerManager->DestroyPlayer(player);
	}
	return true;
}
//-------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::_userLeaveServer(LC_ServerPlayer* player)
{
	IF_A_NA_PLAYER(player){return;}

	int32_t gatewayID = GetGateWayServerID(player);
	_notifyOtherServerUserOff(player->GetUserID(), player->GetInstance(), gatewayID);	

	_doUserLeaveImpl(player, LMIOT_LEAVE);
}
//-------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::_userResetServer(LC_ServerPlayer* player,bool bSendlogin)
{
	IF_A_NA_PLAYER(player){return;}
	if(bSendlogin)
	{
		// 兼容老接口 加的参数， 新的切线流程 不发送，等db回复 走新的发送
		int32_t gatewayID = GetGateWayServerID(player);
		_notifyOtherServerUserReset(player->GetUserID(), player->GetInstance(), gatewayID);
	}
	
	
	_doUserLeaveImpl(player, LMIOT_RESET);
}
//-------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::_userJumpServer(LC_ServerPlayer* player)
{
	IF_A_NA_PLAYER(player){return;}

	_doUserLeaveImpl(player, LMIOT_JUMP);
}
//-------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::_userOffLineServer(LC_ServerPlayer* player, bool bForceLeave)
{
	IF_A_VALID_PLAYER(player)
	{
		if (!player->IsNormalQuit())
		{
			LC_ServerWorldManager* pkServerWorldManager = SERVER_GET_WORLD_MANAGER();
			ResultType eLeaveResult = pkServerWorldManager->CheckLeaveServerWorld(player, LMIOT_OFFLINE, 0.0f);
			if (eLeaveResult != RE_SUCCESS && !bForceLeave)
			{
				player->CancelCurrentSkill();
				_addOfflinePlayer(player->GetInstance(), player);
			}
			else
			{
				int32_t gatewayID = GetGateWayServerID(player);
				_notifyOtherServerUserOff(player->GetUserID(), player->GetInstance(), gatewayID);
				_doUserLeaveImpl(player, LMIOT_OFFLINE);
			}
		}
		GfxWriteFmtLog(LOG_WARNING_FILELINE, LOG_SWITCH_WARNING, "server offline user ", player->GetID(), player->GetUserID(), player->GetInstance(), player->IsNormalQuit(), bForceLeave);
	}
}
//-------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::_userKickOffServer(LC_ServerPlayer* player, ResultType nError)
{
	IF_A_NA_PLAYER(player){return;}

	user_id_type userID = player->GetUserID();
	const unique_id_impl& charID = player->GetInstance();

	MG_GL_REQ_UserKey pkUserKey;
	pkUserKey.m_userKey = player->getUserUUID();
	_userBeKickedOffServer(player);
	SendMsgToLogin(MGPT_SS_KICK_PLAYER, &pkUserKey, userID, charID, 0, GetGateWayServerID(player));

	GfxWriteFmtLog(LOG_WARNING_FILELINE, LOG_SWITCH_WARNING, "server kick user", player->GetID(), player->GetUserID(), player->GetInstance());
}
//-------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::_userBeKickedOffServer(LC_ServerPlayer* player, ResultType nError)
{
	IF_A_NA_PLAYER(player)
	{
		return;
	}

	int32_t gatewayID = GetGateWayServerID(player);
	_notifyOtherServerUserOffWithoutLogin(player->GetUserID(), player->GetInstance(), gatewayID);

	MG_ServerKickUser kick;
	kick.nReason = nError;
	player->SendMsgToClient(MGPT_SS_KICK_PLAYER, &kick);

	_doUserLeaveImpl(player, LMIOT_KICKOFF, nError);
}
void LC_ServerPlayerEnterLeaveManager::_userBeKickedException(LC_ServerPlayer* player)
{
	IF_A_NA_PLAYER(player){return;}
	return;

	MG_Message msg;
	msg.m_iMessageID = RE_DIALOG_CONFIRM_OFF_LINE_EXCEPTION;
	player->SendMsgToClient(MGPT_MESSAGE, &msg);
#if 0 
//#ifdef WIN32
	EXCEPTION_POINTERS* pExcInfo = NULL;
	_GetExceptionPointers( 0, &pExcInfo );
	gstrPrefix = "Exception_";
	ExceptionFilter( pExcInfo );
#endif

	int32_t gatewayID = GetGateWayServerID(player);
	_notifyOtherServerUserOffWithoutLogin(player->GetUserID(), player->GetInstance(), gatewayID);

	_doUserLeaveImpl(player, LMIOT_EXCEPTION);
}
//-------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::_userCleanUpServer(LC_ServerPlayer* player)
{
	IF_A_NA_PLAYER(player){return;}

	int32_t gatewayID = GetGateWayServerID(player);
	_notifyOtherServerUserOff(player->GetUserID(), player->GetInstance(), gatewayID);

	_doUserLeaveImpl(player, LMIOT_CLEANUP);
}
//-------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::_userQuitArenaServer(LC_ServerPlayer* player)
{
	_userJumpServer(player);
}
//-------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::_userGiveUpArenaServer(LC_ServerPlayer* player)
{
	_userJumpServer(player);
}
//-------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::_userDelayKickoffServer(LC_ServerPlayer* player)
{
	IF_A_NA_PLAYER(player){return;}

	m_kDelayKickOffPlayerIDList.push_back(player->GetID());
}
//-------------------------------------------------------------------------------------
LC_ServerPlayer* LC_ServerPlayerEnterLeaveManager::_findOfflinePlayer(const unique_id_impl& charID)
{
	LC_ServerPlayer* player = NULL;
	OfflinePlayerMap::iterator it = m_offlinePlayers.find(charID);
	if (it != m_offlinePlayers.end())
	{
		player = (*it).second.GetPlayer();
	}
	return player;
}
//-------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::_addOfflinePlayer(const unique_id_impl& charID, LC_ServerPlayer* player)
{
	IF_A_NA_PLAYER(player){return;}

	OfflinePlayerMap::iterator it = m_offlinePlayers.find(charID);
	player->Offline();
	player->HeartBeat();
	if (it == m_offlinePlayers.end())
	{
		OfflinePlayerStruct offlinePlayer;
		offlinePlayer.m_playerID = player->GetID();
		offlinePlayer.m_charID = player->GetInstance();
		m_offlinePlayers[charID] = offlinePlayer;
		GfxWriteFmtLog(LOG_WARNING_FILELINE, LOG_SWITCH_WARNING, "add offline, userid, charid", player->GetUserID(), player->GetInstance());
	}
	else
	{
		GfxWriteFmtLog(LOG_WARNING_FILELINE, LOG_SWITCH_WARNING, "add offline duplicate, userid charid", player->GetUserID(), player->GetInstance());
	}
}
//-------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::_removeOfflinePlayer(const unique_id_impl& charID)
{
	m_offlinePlayers.erase(charID);
}
//-------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::_checkOfflinePlayer(float fCurrentTime, float fDeltaTime)
{
	LC_ServerWorldManager* pkServerWorldManager = SERVER_GET_WORLD_MANAGER();
	for (OfflinePlayerMap::iterator it = m_offlinePlayers.begin(); it != m_offlinePlayers.end();)
	{
		LC_ServerPlayer* player = (*it).second.GetPlayer();
		if (NULL == player)
		{
			m_offlinePlayers.erase(it++);
			continue;
		}
		float offlineTime = fCurrentTime - player->GetOfflineTime();
		if (offlineTime > 60.0f)
		{
			ResultType eLeaveResult = pkServerWorldManager->CheckLeaveServerWorld(player, LMIOT_OFFLINE, offlineTime);
			if (eLeaveResult == RE_SUCCESS)
			{
				_userOffLineServer(player, true);
				m_offlinePlayers.erase(it++);
			}
			else
			{
				if (fCurrentTime - player->GetLastHeartBeatTime() > 60.0f)
				{
					player->HeartBeat();
				}
				++it;
			}
		}
		else
		{
			if (fCurrentTime - player->GetLastHeartBeatTime() > 60.0f)
			{
				player->HeartBeat();
			}
			++it;
		}
	}
}
//-------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::_checkHolder(float fCurrentTime, float fDeltaTime)
{
	mem::vector<object_id_type> eraseHolder;
	for (UserPlaceHolder::iterator it = m_placeHolders.begin(); it != m_placeHolders.end(); ++it)
	{
		UserToken& user = (*it).second;
		if (fCurrentTime - user.m_addTime > user.m_holdTime)
		{
			eraseHolder.push_back((*it).first);
		}
	}
	for (size_t i = 0 ; i < eraseHolder.size(); ++i)
	{
		UserPlaceHolder::iterator it = m_placeHolders.find(eraseHolder[i]);
		if (it != m_placeHolders.end())
		{
			UserToken& rUserToken = it->second;
			switch(rUserToken.m_holdType)
			{
			case HoldDataType_CrossRealmTeam:
			case HoldDataType_CrossRealmGuildWar:
			case HoldDataType_CrossRealmRaid:
			case HoldDataType_GuildTransport:
				{
					KickOffCrossRealmPlayer(rUserToken.m_playerID);
				}
				break;
			case HoldDataType_Team:
			case HoldDataType_FrontierAlliance:
			default:
				{
					m_placeHolders.erase(it);
				}
				break;
			}
		}
	}
}
//-------------------------------------------------------------------------------------
bool LC_ServerPlayerEnterLeaveManager::_holderPlayerEnter(LC_ServerPlayer* player)
{
	IF_A_NA_PLAYER(player){return false;}

	bool isHoldPlayer = false;
	UserPlaceHolder::iterator it = m_placeHolders.find(player->GetID());
	if (it != m_placeHolders.end())
	{
		GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "_holderPlayerEnter", player->GetID(), player->GetUserID(), player->GetInstance());
		PlayerEnterGame(player, LMIOT_RECOVER);
		if (it->second.m_holdType == HoldDataType_Team)
		{
			LC_ServerPlayerGroupManager* pkPlayerGroupManager = (LC_ServerPlayerGroupManager*)SERVER_GET_PLAYER_GROUP_MANAGER();
			pkPlayerGroupManager->PlayerRecover(player, it->second.m_Parm1);
		}
		ClearPlayerHold(player->GetID());
		isHoldPlayer = true;
	}
	return isHoldPlayer;
}
//-------------------------------------------------------------------------------------
bool LC_ServerPlayerEnterLeaveManager::_addWaitingPlayer(LC_ServerPlayer* player)
{
	IF_A_NA_PLAYER(player){return false;}

	bool enterWaiting = false;
	if (!m_waitingQueues.IsAlreadyInWaitingQueue(player->GetUserID(), player->GetInstance()))
	{
		m_waitingQueues.AddWaitingQueue(player->GetUserID(), player->GetInstance());
		enterWaiting = true;
		_checkWaiting(0.00f, 0.00f);
	}
	return enterWaiting;
}
//-------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::_checkWaiting(float fCurrentTime, float fDeltaTime)
{
	m_waitingQueues.CheckWaitingQueue(fCurrentTime, fDeltaTime);
}
//-------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::_pullDataFromLogin(LC_ServerPlayer* player)
{
	IF_A_NA_PLAYER(player){return;}

	UserToken userToken;
	userToken.m_userID		= player->GetUserID();
	userToken.m_playerID	= player->GetID();
	userToken.m_charID		= player->GetInstance();
	userToken.m_addTime		= GET_PROCESS_TIME;
	userToken.m_holdTime	= 10.0f;
	m_pullLoginDataHolders[userToken.m_playerID] = userToken;

	m_kPlayerID2CharIDIndices[userToken.m_playerID] = userToken.m_charID;
	m_kCharID2PlayerIDIndices[userToken.m_charID] = userToken.m_playerID;

	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, player->GetID(), player->GetUserID(), player->GetInstance(), "_pullDataFromLogin begin");

	UserAccoutInfoCacheMap::iterator it = m_kUserAccoutInfoCacheMap.find(player->GetInstance());
	if (it != m_kUserAccoutInfoCacheMap.end())
	{
		it->second.UpdateUserInfo(player);
		m_kUserAccoutInfoCacheMap.erase(it);
		LoadDataFromLogin(player);
	}
	else
	{
		if (player->GetLoadDataFromLoginFlag())
		{
			GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, player->GetID(), player->GetUserID(), player->GetInstance(), "GetLoadDataFromLoginFlag Already True");
			LoadDataFromLogin(player);
			return;
		}
	}
}
//-------------------------------------------------------------------------------------

void LC_ServerPlayerEnterLeaveManager::_pullDataFromLoginSuccess(LC_ServerPlayer* player)
{
	IF_A_NA_PLAYER(player)
	{
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, player->GetID(), player->GetUserID(), player->GetInstance(), "_pullDataFromLogin Error" );
		return;
	}

	UserPlaceHolder::iterator it = m_pullLoginDataHolders.find(player->GetID());
	if (it != m_pullLoginDataHolders.end())
	{
		m_pullLoginDataHolders.erase(it);
	}

	player->SetLoadDataFromLoginFlag(true);
	player->PullGMInfo();
}
//-------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::_checkPullLoginData(float fCurrentTime, float fDeltaTime)
{
	mem::vector<object_id_type> eraseHolder;
	for (UserPlaceHolder::iterator it = m_pullLoginDataHolders.begin(); it != m_pullLoginDataHolders.end(); ++it)
	{
		UserToken& user = (*it).second;
		if (fCurrentTime - user.m_addTime > user.m_holdTime)
		{
			eraseHolder.push_back((*it).first);
		}
	}

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	for (size_t i = 0 ; i < eraseHolder.size(); ++i)
	{
		LC_ServerPlayer* player = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(eraseHolder[i]);
		if (player)
		{
			Protocol::MG_GC_RLT_Login rltMsg;
			rltMsg.m_iCharID  = eraseHolder[i];
			rltMsg.m_lResult = RE_LOGIN_FAIL;
			player->SendMsgToClient(MGPT_GC_RLT_LOGIN, &rltMsg);
			GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, " _checkPullLoginData MGPT_GC_RLT_LOGIN  7 ", player->GetUserID());
			pkPlayerManager->DestroyPlayer(player);
		}
		UserPlaceHolder::iterator it = m_pullLoginDataHolders.find(eraseHolder[i]);
		if (it != m_pullLoginDataHolders.end())
		{
			m_pullLoginDataHolders.erase(it);
		}
	}

	{
		UserAccoutInfoCacheMap::iterator it = m_kUserAccoutInfoCacheMap.begin();
		while (it != m_kUserAccoutInfoCacheMap.end())
		{
			if (fCurrentTime > it->second.m_fTimestamp + 5 * 60.0f)
			{
				m_kUserAccoutInfoCacheMap.erase(it++);
			}
			else
			{
				++it;
			}
		}
	}
}
//-------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::_pullDataFromDB(LC_ServerPlayer* player)
{
	IF_A_NA_PLAYER(player){return;}

	if (player->GetLoadDataFromDBFlag())
	{
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, player->GetInstance(), "pull data from db repeated!");
	}

	if (!player->IsProxy())
	{
		UserToken token;
		token.m_userID = player->GetUserID();
		token.m_playerID = player->GetID();
		token.m_charID = player->GetInstance();
		m_pullDBDataQueue.push_back(token);
	}

	player->SetServerViewState(LC_ServerPlayer::SERVER_STATE_LOGINING);

	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, player->GetID(), player->GetUserID(), player->GetInstance(), "_pullDataFromDB enqueue");
}
//-------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::_pullDataFromDBSuccess(LC_ServerPlayer* player)
{
	IF_A_NA_PLAYER(player){return;}

	if (player->IsClone())
	{
		if (!_holderPlayerEnter(player))
		{
			player->AllocateBuddy(player->GetAsAControlee().MapLogicID(), true);

			player->SwitchBuddyScheme(player->GetBuddySchemeMgr().GetBeChallangeMapID(), INIT_SCHEME_GROUPID, SchemeType_Defence, false, true);
			player->GetBuddySchemeMgr().SetBeChallangeMapID(INIT_SCHEME_ID);

			std::vector<LC_ServerPlayer*> vecBuddyPtrPlayer;
			player->GetAllBuddyPtrPlayer(vecBuddyPtrPlayer);
			bool bSummonFinishHandle = false;
			for (int i = 0; i < vecBuddyPtrPlayer.size(); i++)
			{
				LC_ServerPlayer* pkBuddyPlayer = vecBuddyPtrPlayer[i];
				IF_A_NA_PLAYER(pkBuddyPlayer)
					continue;
				pkBuddyPlayer->FinishPlayerCloneLoad(i, false, SchemeType_Defence,1);
			}
			player->ExecuteSummonFinishHandle();
		}
	}
	else if (player->IsInstance())
	{
		if (!_holderPlayerEnter(player))
		{
			GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "[%d][LOGIN STEP 4] get all info from db", UT_ServerHelper::GetServerId(), player->GetUserID(), player->GetInstance());
			_addWaitingPlayer(player);
		}
	}

	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, player->GetID(), player->GetUserID(), player->GetInstance(), "_pullDataFromDBSuccess");
}
//-------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::_updatePullDataFromDB(float fCurrentTime, float fDeltaTime)
{
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	int count = 0;
	object_id_set has_handle_ids;
	for (;;)
	{
		if (m_pullDBDataQueue.empty())
			break;
		if (count > m_pullDataPerTimeCount)
			break;
		UserToken user = m_pullDBDataQueue.front();
		m_pullDBDataQueue.pop_front();

		//to prevent repull a db data, only in once
		if (has_handle_ids.find(user.m_playerID) != has_handle_ids.end())
		{
			continue;
		}
		has_handle_ids.insert(user.m_playerID);

		
		LC_ServerPlayer* player = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(user.m_playerID);
		if (player && !player->GetLoadDataFromDBFlag())
		{
			MG_GD_Load_Char_All_Info msg;
			msg.player_id = user.m_playerID;
			msg.instance = player->GetInstance();
			SendMsgToGameDB(MGPT_GD_REQ_ALL_CHAR_INFO, &msg, user.m_userID, player->GetInstance());
			GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,"user can login, pulling db infos register serverId : ",player->GetInstanceCitizen()," enter serverId：" ,UT_ServerHelper::GetServerId()," userid: " ,user.m_userID," ucharId: ", user.m_charID);
			++count;
		}
	}
}
//-------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::_checkPlayerHeartbeat(float fCurrentTime, float fDeltaTime)
{
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	const LC_PlayerIDPtrMap& allPlayers = pkPlayerManager->GetPlayerIDMap();
	mem::vector<LC_ServerPlayer*> kLeavePlayers;
	object_id_set kBadPlayerIDs;
	{
		LC_PlayerIDPtrMap::const_iterator cit = allPlayers.begin();
		for (; cit != allPlayers.end(); ++cit)
		{
			LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)cit->second;
			if (pkServerPlayer)
			{
				float fLastTime = pkServerPlayer->GetLastHeartBeatTime();
				float fTimeout = GET_PROCESS_TIME - fLastTime;
				if (fLastTime > 0.00f && fTimeout > MAX_CLIENT_IDLE_TIME
						&& !pkServerPlayer->IsOffline())
				{
					GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "GameServer Found Client heartbeat timeout!",  UT_ServerHelper::GetServerId(),  cit->first, pkServerPlayer->GetID(), pkServerPlayer->GetUserID(), pkServerPlayer->GetInstance(), pkServerPlayer->GetOwnerCharName().c_str(), fTimeout);
#if 0
					{
						EXCEPTION_POINTERS* pExcInfo = NULL;
						_GetExceptionPointers( 0, &pExcInfo );
						gstrPrefix = "HeartBeat_" + boost::lexical_cast<std::string>(pkServerPlayer->GetID()) + "_UID_" + boost::lexical_cast<std::string>(pkServerPlayer->GetUserID()) + "_CID_" + boost::lexical_cast<std::string>(pkServerPlayer->GetInstance()) + "_Node_" +boost::lexical_cast<std::string>(UT_ServerHelper::GetServerId()) + "_" ;
						ExceptionFilter( pExcInfo);
					}
#endif
					kLeavePlayers.push_back(pkServerPlayer);
					if (cit->first != pkServerPlayer->GetID())
					{
						kBadPlayerIDs.insert(cit->first);
					}
				}
			}
		}
	}

	{
		mem::vector<LC_ServerPlayer*>::iterator it = kLeavePlayers.begin();
		for (; it != kLeavePlayers.end(); ++it)
		{
			LC_ServerPlayer* pPlayer = *it;
			LC_ServerPlayer* pOwner = pPlayer->GetOwnerPlayer();
			if (pOwner == NULL)
				continue;
			mem::vector<int32_t> vecBuddyIDs;
			pOwner->GetActivateBuddyIDs( vecBuddyIDs);
			for (int i = 0; i < vecBuddyIDs.size(); i++)
			{
				LC_ServerPlayer* pBuddy = pOwner->GetActivateBuddyByID(vecBuddyIDs[i]);
				UserLeaveServer(pBuddy, LMIOT_OFFLINE);
			}
			//UserLeaveServer(pPlayer, LMIOT_OFFLINE);
		}
	}

	{
		object_id_set::iterator it = kBadPlayerIDs.begin();
		for (; it != kBadPlayerIDs.end(); ++it)
		{
			pkPlayerManager->ClearPlayerIndex(*it);
		}
	}
}
//-------------------------------------------------------------------------------------
bool LC_ServerPlayerEnterLeaveManager::_enterOffLinePlayer(user_id_type userID, const unique_id_impl& charID, NW_ServerSession* session)
{
	bool offlinePlayer = false;
	LC_ServerPlayer* pkPlayer = _findOfflinePlayer(charID);
	//如果找到就是断线玩家
	IF_A_VALID_PLAYER(pkPlayer)
	{
		pkPlayer->SetSession(session);
		pkPlayer->Online();
		pkPlayer->SetServerViewState(LC_ServerPlayer::SERVER_STATE_ENTERED);
		pkPlayer->SetLastMoveCheckLocation(pkPlayer->GetCurrentLocation());

		_removeOfflinePlayer(charID);

		Protocol::MG_GC_RLT_Login rltMsg;
		rltMsg.m_iCharID  = charID;
		rltMsg.m_lResult = RE_SUCCESS;
		pkPlayer->SendMsgToClient(MGPT_GC_RLT_LOGIN, &rltMsg);
		GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, " _enterOffLinePlayer MGPT_GC_RLT_LOGIN  8 ", pkPlayer->GetUserID());
		pkPlayer->SendNoticeEnterGame();

		
		pkPlayer->GetCommonStatAsset().ForceDirty();
		pkPlayer->GetSubordinateAsset().ForceDirty();
		pkPlayer->GetACTScheduleAsset().ForceDirty();
		pkPlayer->SyncLocationInfo();

		//重新拉取组级业务数据
		{
			pkPlayer->GetFriendDataFromDB();
			pkPlayer->GetGuildDataFromDB();
		}

		pkPlayer->GetGameStoryAsset().UpdateDirtyFlag();

		LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
		LC_MapLogicBase* mapLogic = pkWorldManager->GetMapLogic(pkPlayer->GetMapLogicID());
		if (mapLogic)
		{
			mapLogic->NotifyEnterOfflinePlayer(pkPlayer);
			offlinePlayer = true;
		}
	}
	return offlinePlayer;
}
//-------------------------------------------------------------------------------------
LC_ServerPlayer* LC_ServerPlayerEnterLeaveManager::ClonePlayer(LC_ServerPlayer* Owner, user_id_type userID, object_id_type objectID, const unique_id_impl& charID, uint32_t bits, bool isRobot)
{
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* player = NULL;
	if(isRobot)
	{
		CF_ExportSoloDuleRobotInfo::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_ExportSoloDuleRobotInfo>(charID);
		if(NULL!=pkData && !pkData->_buddyBaseInfo.empty())
		{
			int buddyID = pkData->_buddyBaseInfo[0].IID();
			player = (LC_ServerPlayer*)pkPlayerManager->CreatePlayer(objectID, charID, buddyID, (3<<24) + bits);
		}
	}

	if(NULL == player)
	{
		player = (LC_ServerPlayer*)pkPlayerManager->CreatePlayer(objectID, charID, 1, (4<<24) + bits);
	}

	if (player)
	{
		if (player->IsClone() && !player->IsBuddy())
		{
			player->SetAIControl(true);
			player->RefreshSMController();
		}

		if(NULL != Owner)
		{
			player->SetOwnerPlayerId(Owner->GetID()); // 竞技场
		}
		else
		{
			player->SetLevel(1);
		}

		//player->InitLogicData();

		player->SetUserID(userID);
		player->SetInstance(charID);
		if(isRobot)
		{
		}
		else if (!IS_FLAG_VALID( bits, PLY_DEF_FG_BUDDY ))
		{
			//是真的玩家，从数据库拉数据
			_pullDataFromDB(player);
		}
	}
	return player;
}
//-------------------------------------------------------------------------------------
bool LC_ServerPlayerEnterLeaveManager::_enterOnlinePlayer(user_id_type userID, const unique_id_impl& charID, NW_ServerSession* session)
{
	bool isOnlinePlayer = false;
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* player = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(charID);
	IF_A_NA_PLAYER(player){return isOnlinePlayer;}
	if (!player->IsOffline())
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "%s, Found player already online, c_cid:%d, c_citizenship:%d, c_uid:%d, id:%d", __FUNCTION__, 
			charID.detail(), charID.catagory(), player->GetUserID(), player->GetID());

		isOnlinePlayer = true;
		Protocol::MG_GC_RLT_Login rltMsg;
		rltMsg.m_iCharID  = charID;
		rltMsg.m_lResult = RE_LOGIN_USER_ONLINE_ALREADY;
		SendMsgToClientByGateway(session->PeerID(), MGPT_GC_RLT_LOGIN, &rltMsg, userID, charID);
		//GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, " _enterOnlinePlayer MGPT_GC_RLT_LOGIN  9 ", userID);
		LC_ServerPlayer* pOwner = player->GetOwnerPlayer();
		{
#if 0
//#ifdef WIN32
			EXCEPTION_POINTERS* pExcInfo = NULL;
			_GetExceptionPointers( 0, &pExcInfo );
			gstrPrefix = "enterOnlinePlayer_" + boost::lexical_cast<std::string>(userID) + "_Node_" + boost::lexical_cast<std::string>(UT_ServerHelper::GetServerId()) + "_" ;
			ExceptionFilter( pExcInfo);
#endif

		}
		if ( pOwner == NULL )
		{
			UserLeaveServer(player, LMIOT_OFFLINE);
			return isOnlinePlayer;
		}
		mem::vector<int32_t> vecBuddyIDs;
		pOwner->GetActivateBuddyIDs( vecBuddyIDs);
		for (int i = 0; i < vecBuddyIDs.size(); i++)
		{
			LC_ServerPlayer* pBuddy = pOwner->GetActivateBuddyByID(vecBuddyIDs[i]);
			UserLeaveServer(pBuddy, LMIOT_OFFLINE);
		}
		
	}
	return isOnlinePlayer;
}
//-------------------------------------------------------------------------------------

bool LC_ServerPlayerEnterLeaveManager::_enterHolderPlayer(user_id_type userID, const unique_id_impl& charID, NW_ServerSession* session)
{
	bool holdPlayer = false;
	UserPlaceHolder::iterator it = m_placeHolders.find(GET_PLAYERID(userID));
	if (it != m_placeHolders.end())
	{
		LC_ServerPlayer* player = (SERVER_GET_PLAYER_MANAGER())->CreatePlayerBySession(userID, charID, session);
		IF_A_VALID_PLAYER (player)
		{
			_pullDataFromLogin(player);
			holdPlayer = true;
		}
	}
	return holdPlayer;
}
//-------------------------------------------------------------------------------------
bool LC_ServerPlayerEnterLeaveManager::_enterNormalPlayer(user_id_type userID, const unique_id_impl& charID, NW_ServerSession* session)
{
	bool canEnter = false;
	if (!IsServerFull())
	{
		canEnter = true;
		if (canEnter)
		{
			LC_ServerPlayer* player = (SERVER_GET_PLAYER_MANAGER())->CreatePlayerBySession(userID, charID, session);
			if (player)
			{
				_pullDataFromLogin(player);
			}
			else
			{
				LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
				player = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(charID);
				IF_A_VALID_PLAYER (player)
				{
					player->SetSession(session);
				}
			}
		}
	}
	return canEnter;
}

bool LC_ServerPlayerEnterLeaveManager::_enterBuddyPlayer(user_id_type userID, const unique_id_impl& charID, LC_ServerPlayer* pOwner)
{
	return true;
}
//-------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::_notifyOtherServerUserOn(user_id_type userID, const unique_id_impl& charID, int32_t gatewayId, int32_t realmid)
{
	SendMsgToGameDB(MGPT_SS_ROLE_ENTER_GAME, NULL, userID, charID);
	SendMsgToMailDB(MGPT_SS_ROLE_ENTER_GAME, NULL, userID, charID);
	SendMsgToSociety(MGPT_SS_ROLE_ENTER_GAME, NULL, userID, charID, realmid, gatewayId);
	SendMsgToLogin(MGPT_SS_ROLE_ENTER_GAME, NULL, userID, charID, 0, gatewayId);
	SendMsgToGroup(MGPT_SS_ROLE_ENTER_GAME, NULL, userID, charID);
	SendMsgToChat(MGPT_SS_ROLE_ENTER_GAME, NULL, userID, charID, realmid, gatewayId);
	SendMsgToTrade(MGPT_SS_ROLE_ENTER_GAME, NULL, userID, charID);
	m_loginInUsers[GET_PLAYERID(userID)] = true;
}
//-------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::_notifyOtherServerUserOff(user_id_type userID, const unique_id_impl& charID, int32_t gatewayId)
{
	LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (NULL == pkServerPlayerManager)
	{
		return;
	}

	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkServerPlayerManager->FindPlayer(charID);
	if (NULL == pkPlayer)
	{
		return;
	}
	
	UserPullLoginMap::iterator it = m_loginInUsers.find(pkPlayer->GetID());
	if (it != m_loginInUsers.end())
	{
		//通知DB 该角色离线
		SendMsgToGameDB(MGPT_SS_ROLE_LEAVE_GAME, NULL, userID, charID);
		SendMsgToMailDB(MGPT_SS_ROLE_LEAVE_GAME, NULL, userID, charID);
		SendMsgToSociety(MGPT_SS_ROLE_LEAVE_GAME, NULL, userID, charID);
		SendMsgToTrade(MGPT_SS_ROLE_LEAVE_GAME, NULL, userID, charID);
		MG_GL_REQ_UserKey pkUserKey;
		pkUserKey.m_userKey = pkPlayer->getUserUUID();

		SendMsgToLogin(MGPT_SS_USER_LOGOUT, &pkUserKey, userID, charID, 0, gatewayId);
		SendMsgToGroup(MGPT_SS_ROLE_LEAVE_GAME, NULL, userID, charID);
		SendMsgToChat(MGPT_SS_ROLE_LEAVE_GAME, NULL, userID, charID, 0, gatewayId);
		GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, userID, charID, "_notifyOtherServerUserOff");
		m_loginInUsers.erase(it);
	}
}
//-------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::_notifyOtherServerUserOffWithoutLogin(user_id_type userID, const unique_id_impl& charID, int32_t gatewayId)
{
	UserPullLoginMap::iterator it = m_loginInUsers.find(GET_PLAYERID(userID));
	if (it != m_loginInUsers.end())
	{
		//通知DB 该角色离线
		SendMsgToGameDB(MGPT_SS_ROLE_LEAVE_GAME, NULL, userID, charID);
		SendMsgToMailDB(MGPT_SS_ROLE_LEAVE_GAME, NULL, userID, charID);
		SendMsgToSociety(MGPT_SS_ROLE_LEAVE_GAME, NULL, userID, charID);
		SendMsgToGroup(MGPT_SS_ROLE_LEAVE_GAME, NULL, userID, charID);
		SendMsgToChat(MGPT_SS_ROLE_LEAVE_GAME, NULL, userID, charID, 0, gatewayId);
		SendMsgToTrade(MGPT_SS_ROLE_LEAVE_GAME, NULL, userID, charID);
		m_loginInUsers.erase(it);
	}
}
//-------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::_notifyOtherServerUserReset(user_id_type userID, const unique_id_impl& charID, int32_t gatewayId)
{
	UserPullLoginMap::iterator it = m_loginInUsers.find(GET_PLAYERID(userID));
	if (it != m_loginInUsers.end())
	{
		//通知DB 该角色离线
		SendMsgToGameDB(MGPT_SS_ROLE_LEAVE_GAME, NULL, userID, charID);
		SendMsgToMailDB(MGPT_SS_ROLE_LEAVE_GAME, NULL, userID, charID);
		SendMsgToSociety(MGPT_SS_ROLE_LEAVE_GAME, NULL, userID, charID);
		SendMsgToLogin(MGPT_SS_ROLE_LEAVE_GAME, NULL, userID, charID, 0, gatewayId);
		SendMsgToGroup(MGPT_SS_ROLE_LEAVE_GAME, NULL, userID, charID);
		SendMsgToChat(MGPT_SS_ROLE_LEAVE_GAME, NULL, userID, charID , 0, gatewayId);
		SendMsgToTrade(MGPT_SS_ROLE_LEAVE_GAME, NULL, userID, charID);
		m_loginInUsers.erase(it);
	}
}
//-------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::_notifyOtherServerUserJump(user_id_type userID, const unique_id_impl& charID, int32_t gatewayId)
{
	UserPullLoginMap::iterator it = m_loginInUsers.find(GET_PLAYERID(userID));
	if (it != m_loginInUsers.end())
	{
		//通知DB 该角色离线
		SendMsgToGameDB(MGPT_SS_ROLE_LEAVE_GAME, NULL, userID, charID);
		SendMsgToMailDB(MGPT_SS_ROLE_LEAVE_GAME, NULL, userID, charID);
		SendMsgToSociety(MGPT_SS_ROLE_LEAVE_GAME, NULL, userID, charID);
		SendMsgToGroup(MGPT_SS_ROLE_LEAVE_GAME, NULL, userID, charID);
		SendMsgToChat(MGPT_SS_ROLE_LEAVE_GAME, NULL, userID, charID , 0, gatewayId);
		SendMsgToTrade(MGPT_SS_ROLE_LEAVE_GAME, NULL, userID, charID);
		m_loginInUsers.erase(it);
	}
}
//-------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::_removePlayerFromServerWorld(LC_ServerPlayer* player, LC_MapInOutType eOutType)
{
	IF_A_NA_PLAYER(player){return;}
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "remove player from world by type", player->GetPlayerFlag(), player->GetID(), player->GetInstance(), player->GetMapResID(), eOutType);
	pkWorldManager->RequestLeaveServerWorld(player, eOutType);
}

//-------------------------------------------------------------------------------------
bool LC_ServerPlayerEnterLeaveManager::_checkServerIDAndTransServer(LC_ServerPlayer* player)
{
	return true;
}

void LC_ServerPlayerEnterLeaveManager::_checkCrossRealmTeamPlayerList(float fCurrentTime, float fDeltaTime)
{
	uint64_t curtime = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	mem::set<object_id_type> timeOutplayerid;
	for (CrossRealmTeamPlayDataMap::iterator it = m_CrossRealmTeamPlayInfoMap.begin(); it != m_CrossRealmTeamPlayInfoMap.end(); ++it)
	{
		CrossRealmTeamData& data = (*it).second;
		mem::map<object_id_type, CrossRealmTeam_Player>::iterator playerbeg = data.m_PlayerInfo.begin();
		for (; playerbeg != data.m_PlayerInfo.end(); ++playerbeg)
		{
			if (!playerbeg->second.m_Enter && curtime >= playerbeg->second.m_Timestamp)
			{
				timeOutplayerid.insert(playerbeg->first);
			}
		}
	}
	mem::set<object_id_type>::iterator actorbeg = timeOutplayerid.begin();
	for (; actorbeg != timeOutplayerid.end(); ++actorbeg)
	{
		//超时玩家需要剔除gateway
		KickOffCrossRealmPlayer(*actorbeg);
	}
}

void LC_ServerPlayerEnterLeaveManager::_checkDelayKickoffPlayerList(float fCurrentTime, float fDeltaTime)
{
	LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if (NULL == pkServerPlayerManager)
	{
		return;
	}
	object_id_vector_iter IterBe = m_kDelayKickOffPlayerIDList.begin();
	object_id_vector_iter IterEd = m_kDelayKickOffPlayerIDList.end();
	for (; IterBe != IterEd; ++IterBe)
	{
		object_id_type lPlayerID = *IterBe;
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(pkServerPlayerManager->FindPlayer(lPlayerID));
		if (NULL == pkPlayer)
		{
			continue;
		}
		_userKickOffServer(pkPlayer);
	}
	m_kDelayKickOffPlayerIDList.clear();
}
//-------------------------------------------------------------------------------------
ResultType LC_ServerPlayerEnterLeaveManager::PlayerTryEnterAlliance(LC_ServerPlayer* player, int32_t nServerID, int32_t nAreaID, bool bSendEnter)
{
	IF_A_NA_PLAYER(player){ return RE_FAIL; }

	int32_t nResult = RE_SUCCESS;
	if (nServerID != UT_ServerHelper::GetServerId())
	{
		//跳转进入其它服边境地图
		//解除组队状态
		player->ClearPlayerGroup(LC_ServerPlayer::E_PlayerGroupMapOpt_InMap, LC_ServerPlayer::E_PlayerGroupLimit_All);
		//跳服务器
		UserJumpServer(player, nServerID, LMIOT_JUMP);
	}
	else
	{
		if(bSendEnter == true)
		{
			player->SendNoticeEnterGame();
		}
		//进入本机边境地图
		CF_TransferArea::DataEntry* pkAreaData = CF_TransferArea::GetSingletonPtr()->GetEntryPtr(nAreaID);
		if (pkAreaData != NULL)
		{
			nResult = (SERVER_GET_WORLD_MANAGER())->RequestChangePlayerLocation(player, LMIOT_TRANSFER, 0, pkAreaData->_lMapID, pkAreaData->_kPos);
		}
		else
		{
			nResult = RE_ALLIANCE_ENTER_CROSS_AreaErr;
		}
		//将进入结果返回请求服务器
		bool bInCross = UT_ServerHelper::CheckLocal(player);
		if( bInCross == false)
		{
			//设置跨服标记
			player->SetCrossServerType(CrossRealmType_Alliance);

			MG_Resp_SS_Alliance_Enter_Cross resp;
			resp.m_nTargetServerID = nServerID;
			resp.m_nAreaID = nAreaID;
			resp.m_nCharID = player->GetInstance();
			resp.m_nUserID = player->GetUserID();
			resp.m_nFromServerID = player->GetCitizenship();
			resp.m_nResult = nResult;
			SendMsgToSociety(MGPT_SS_FALLIANCE_ENTER_CROSS_SUCC, &resp, player->GetUserID(), player->GetInstance());
		}
		
		if(nResult == RE_SUCCESS)
		{
			//清除本服的标记,仅清除Hold
			ClearPlayerHold(player->GetID());
			if(bInCross == true)
			{
				ClearCrossRealmIndex(player->GetID());
			}
		}
	}
	return nResult;
}
//-------------------------------------------------------------------------------------
bool LC_ServerPlayerEnterLeaveManager::AddFrontierAlliancePlayerInfo(const unique_id_impl& charID, user_id_type userID, int32_t nAreaID)
{
	object_id_type playerID = GET_PLAYERID(userID);
	ClearFrontierAlliancePlayerInfo(playerID);
	AddPlayerHolder(charID, userID, (float)2 * 60, HoldDataType_FrontierAlliance, nAreaID);
	return AddCrossRealmIndex(charID, userID, CrossRealmType_Alliance, nAreaID);
}
//-------------------------------------------------------------------------------------
void LC_ServerPlayerEnterLeaveManager::ClearFrontierAlliancePlayerInfo(object_id_type playerID)
{
	UserToken* token = GetPlayerHold(playerID);
	if (token != NULL)
	{
		if (token->m_holdType == HoldDataType_CrossRealmTeam)
		{
			DelCrossRealmTeamPlayerInfo(token->m_Parm1, playerID, true);
		}
		else if (token->m_holdType == HoldDataType_Team)
		{
			LC_ServerPlayerGroupManager* pkPlayerGroupManager = SERVER_GET_PLAYER_GROUP_MANAGER();
			bool destory = false;
			pkPlayerGroupManager->DelPlayer(token->m_Parm1, playerID, destory);
			ClearPlayerHold(playerID);
		}
		else if (token->m_holdType == HoldDataType_CrossRealmGuildWar)
		{
			DelCrossRealmGuildWarPlayerInfo(playerID);
		}
		else
		{
			ClearPlayerHold(playerID);
		}
	}
}
//-------------------------------------------------------------------------------------
bool LC_ServerPlayerEnterLeaveManager::DelFrontierAlliancePlayerInfo(object_id_type playerID)
{
	ClearPlayerHold(playerID);
	ClearCrossRealmIndex(playerID);
	return true;
}
//-------------------------------------------------------------------------------------
bool LC_ServerPlayerEnterLeaveManager::FrontierAlliancePlayerEnter(LC_ServerPlayer* player)
{
	IF_A_NA_PLAYER(player){ return false; }

	CrossRealmData data;
	if (!CheckCrossRealmIndex(player->GetID(), data))
	{
		return false;
	}
	if (PlayerTryEnterAlliance(player, UT_ServerHelper::GetServerId(), data.m_MapLogicId, true) != RE_SUCCESS)
	{
		PlayerTryLeaveCrossRealm(player);
		return false;
	}
	return true;
}
//-------------------------------------------------------------------------------------
bool LC_ServerPlayerEnterLeaveManager::FrontierAlliancePlayerLeave(LC_ServerPlayer* player, bool back)
{	
	object_id_type playerid = player->GetID();
	DelFrontierAlliancePlayerInfo(playerid);
	int srcserverid = UT_ServerHelper::GetSrcServerId(player);
	if (srcserverid != UT_ServerHelper::GetServerId() && back)
	{
		UserJumpServer(player, srcserverid, LMIOT_JUMP);
		return true;
	}
	return false;
}
//-------------------------------------------------------------------------------------
uint32_t LC_ServerPlayerEnterLeaveManager::RequstCrossChangeLocation(LC_ServerPlayer* pkPlayer, int nServerID, int iTargetMapResID, const Utility::UT_Vec3Int& rkTargetLocation)
{
	IF_A_NA_PLAYER(pkPlayer){ return RE_FAIL; }

	uint32_t nRel = LC_Helper::CanEnterMap(iTargetMapResID, pkPlayer);
	if(nRel != RE_SUCCESS)
	{
		return nRel;
	}
	
	MG_Req_SS_Alliance_Back_Server resp;
	resp.m_nCharID = pkPlayer->GetInstance();
	resp.m_nUserID = pkPlayer->GetUserID();
	resp.m_nTargetServerID = nServerID;
	resp.m_nMapResID = iTargetMapResID;
	resp.m_nFromServerID = UT_ServerHelper::GetServerId();
	resp.m_bPos = true;
	resp.m_posInfo.m_nPosX = rkTargetLocation.x;
	resp.m_posInfo.m_nPosY = rkTargetLocation.y;
	resp.m_posInfo.m_nPosZ = rkTargetLocation.z;
	SendMsgToSociety(MGPT_SS_FALLIANCE_BACK_SERVER_REQ, &resp, pkPlayer->GetUserID(), pkPlayer->GetInstance());
	return RE_SUCCESS;
}

uint32_t LC_ServerPlayerEnterLeaveManager::RequstCrossChangeLocation(LC_ServerPlayer* pkPlayer, int nServerID, int iTargetMapResID, int32_t nType, int32_t nIndex)
{
	IF_A_NA_PLAYER(pkPlayer){ return RE_FAIL; }

	uint32_t nRel = LC_Helper::CanEnterMap(iTargetMapResID, pkPlayer);
	if(nRel != RE_SUCCESS)
	{
		return nRel;
	}

	MG_Req_SS_Alliance_Back_Server resp;
	resp.m_nCharID = pkPlayer->GetInstance();
	resp.m_nUserID = pkPlayer->GetUserID();
	resp.m_nTargetServerID = nServerID;
	resp.m_nMapResID = iTargetMapResID;
	resp.m_nFromServerID = UT_ServerHelper::GetServerId();
	resp.m_bPos = false;
	resp.m_indexInfo.m_nType = nType;
	resp.m_indexInfo.m_nIndex = nIndex;
	SendMsgToSociety(MGPT_SS_FALLIANCE_BACK_SERVER_REQ, &resp, pkPlayer->GetUserID(), pkPlayer->GetInstance());
	return RE_SUCCESS;
}

uint32_t LC_ServerPlayerEnterLeaveManager::DealCrossChangeLocationRequst(const unique_id_impl& charID, user_id_type userID, const Protocol::MG_Req_SS_Alliance_Back_Server& reqInfo)
{
	if( 0 ==  reqInfo.m_nTargetServerID )
		return RE_FAIL;
	if(GSNodeNetwork::Instance().getEntryIdByMergeId(reqInfo.m_nTargetServerID) != GSNodeNetwork::Instance().getEntryIdByMergeId(charID.catagory()))
	{
		return RE_CANNOT_TRANS_TO_NO_SELF_MAP;
	}

	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	
	const LC_ServerMapLogicPlaceVector* pkPlaces = pkWorldManager->GetMapLogicByRes(reqInfo.m_nMapResID);
	if (pkPlaces == NULL || pkPlaces->size() <= 0)
	{
		return RE_CANNOT_TRANS_TO_NO_MAP;
	}

	//清除本服的所有hold状态
	ClearPlayerHold(charID, HoldDataType_All);
	ClearCrossRealmIndex(charID);

	//设置将要到达的地图位置
	if(reqInfo.m_bPos == true)
	{
		Utility::UT_Vec3Int rPos;
		rPos.x = reqInfo.m_posInfo.m_nPosX;
		rPos.y = reqInfo.m_posInfo.m_nPosY;
		rPos.z = reqInfo.m_posInfo.m_nPosZ;
		AddCharPositionHold(charID, reqInfo.m_nMapResID, rPos);
	}
	else
	{
		AddCharPositionHold(charID, reqInfo.m_nMapResID, reqInfo.m_indexInfo.m_nType, reqInfo.m_indexInfo.m_nIndex);
	}
	return RE_SUCCESS;
}

uint32_t LC_ServerPlayerEnterLeaveManager::StartCrossChangeLocation(LC_ServerPlayer* pkPlayer, int nTargetServer)
{
	IF_A_NA_PLAYER(pkPlayer){ return RE_FAIL; }
	//离开所有的跨服
	PlayerTryLeaveCrossRealm(pkPlayer);
	pkPlayer->SetCrossServerType(CrossRealmType_Max);
	UserJumpServer(pkPlayer, nTargetServer, LMIOT_JUMP);
	return RE_SUCCESS;
}

void LC_ServerPlayerEnterLeaveManager::AddCharPositionHold(const unique_id_impl& charID, int nMapRes, const Utility::UT_Vec3Int& rPos)
{
	ChgPositionInfo& rPosition = m_kCharIDPositionHold[charID.data()];
	rPosition.m_charID = charID;
	rPosition.m_iMapResID = nMapRes;
	rPosition.m_bPos = true;
	rPosition.m_rTargetPos = rPos;
}

void LC_ServerPlayerEnterLeaveManager::AddCharPositionHold(const unique_id_impl& charID, int nMapRes, int32_t nType, int32_t nIndex)
{
	ChgPositionInfo& rPosition = m_kCharIDPositionHold[charID.data()];
	rPosition.m_charID = charID;
	rPosition.m_iMapResID = nMapRes;
	rPosition.m_bPos = false;
	rPosition.m_nType = nType;
	rPosition.m_nIndex = nIndex;
}

void LC_ServerPlayerEnterLeaveManager::ClearCharPositionHold(const unique_id_impl& charID)
{
	m_kCharIDPositionHold.erase(charID.data());
}

bool LC_ServerPlayerEnterLeaveManager::AddSwitchMapLinePlayer( user_id_type nUserId, const PlayerPrepareSwitchLineArg & arg)
{
	/// 有玩家要切线过来，延迟目标地图自动销毁
	LC_ServerWorldManager* pkWorldMgr = SERVER_GET_WORLD_MANAGER();
	int32_t iMapLogicID = arg.m_iMapLogicID;
	if (iMapLogicID == 0 || pkWorldMgr->DelayMapLogicAutoRelease(iMapLogicID))
	{
		m_SwitchMapLinePlayerSet[nUserId] = arg;
		return true;
	}
	else
	{
		m_SwitchMapLinePlayerSet.erase(nUserId);
		return false;
	}
}

void LC_ServerPlayerEnterLeaveManager::DelSwitchMapLinePlayer( user_id_type nUserId )
{
	m_SwitchMapLinePlayerSet.erase( nUserId );
}

bool LC_ServerPlayerEnterLeaveManager::CheckSwitchMapLinePlayer( user_id_type nUserId )
{
	SWITCH_MAP_LINE_PLAYER_MAP::iterator it = m_SwitchMapLinePlayerSet.find( nUserId );
	if ( it == m_SwitchMapLinePlayerSet.end() )
		return false;
	return true;
}

void LC_ServerPlayerEnterLeaveManager::UpdateSwitchMapLinePlayer(float fDeltaTime)
{
	/// 更新切线请求，看看是否过期
	for (SWITCH_MAP_LINE_PLAYER_MAP::iterator it = m_SwitchMapLinePlayerSet.begin(), itEnd = m_SwitchMapLinePlayerSet.end(); it != itEnd; )
	{
		float & fReqTime = it->second.m_fReqTime;
		fReqTime += fDeltaTime;

		if (fReqTime >= 15.0f)	/// 这个时间，最好比Map自动清理的延迟时间晚，见 LC_ServerWorldManager::Update
		{
			/// out of date, delete this request
			it = m_SwitchMapLinePlayerSet.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void LC_ServerPlayerEnterLeaveManager::AddOnlinePlayer( unique_id_type nCharId )
{
	m_OnlinePlayerSnapshot.insert( nCharId );
}

void LC_ServerPlayerEnterLeaveManager::DelOnlinePlayer( unique_id_type nCharId )
{
	m_OnlinePlayerSnapshot.erase( nCharId );
}

bool LC_ServerPlayerEnterLeaveManager::CheckOnlinePlayer( unique_id_type nCharId )
{
	ONLINE_PLAYER_SNAPSHOT::iterator it = m_OnlinePlayerSnapshot.find( nCharId );
	if ( it == m_OnlinePlayerSnapshot.end() )
		return false;
	return true;
}

LC_ServerPlayerEnterLeaveManager::ChgPositionInfo* LC_ServerPlayerEnterLeaveManager::GetCharPositionHold(const unique_id_impl& charID)
{
	CharPositionHoldMap::iterator iter = m_kCharIDPositionHold.find(charID.data());
	if(iter == m_kCharIDPositionHold.end())
	{
		return NULL;
	}
	return &(iter->second);
}

ResultType LC_ServerPlayerEnterLeaveManager::TransferToServerByType(LC_ServerPlayer* pkPlayer, int nMapRes, int32_t nType, int32_t nIndex, bool bForceTribuMap, int32_t nLineIndex)
{
	if(pkPlayer == NULL)
	{
		return RE_FAIL;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (NULL == pkPlayer)
	{
		return RE_FAIL;
	}

	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	
	switch (nType)
	{
	case TT_TRANSPORT_RAID_MAP:
		{
			LC_ActionForbid* actionForbid = pkPlayer->GetActionForbid();
			if (actionForbid->IsActionForbid(LAFT_TRANSCRIPTION, LAF_TRANSCRIPTION_MAX_COUNT))
			{
				MG_Message errorMsg;
				errorMsg.m_iMessageID = RE_REQ_ENTER_ENTERTRANSCRIPTION_STATE_FAIL;
				pkPlayer->SendMsgToClient(MGPT_MESSAGE, &errorMsg);
				return RE_FAIL;
			}
			int	iMapLogicType	= 0;
			CF_WorldMapList::DataEntry* data = CF_WorldMapList::GetSingletonPtr()->GetEntryPtr(nMapRes);
			if (data == NULL)
			{
				return RE_FAIL;
			}
			if(data->_CanChgLocation <= 0 && UT_ServerHelper::IsInCross(pkPlayer) == true)
			{
				MG_Message errorMsg;
				errorMsg.m_iMessageID = RE_CANNOT_TRANS_TO_NORMAL_MAP;
				pkPlayer->SendMsgToClient(MGPT_MESSAGE, &errorMsg);
				return RE_FAIL;
			}
			iMapLogicType = data->_iMapLogicType;

			LC_RaidAssert*	raidAsset = pkPlayer->GetRaidAsset();

			//检测是否已经通关该副本
			//data->_iChallengeAgain=1，通关可以重复挑战
			const RaidInfo* raidInfo = raidAsset->FindRaid(nMapRes);
			if(NULL != raidInfo && 0==data->_iChallengeAgain)
			{
				return RE_TRANSFER_HAS_PASSED;
			}

			//荣光远征，3星就不能再挑战了
			if(NULL != raidInfo)
			{				
				int score = raidInfo->Score();
				if(3 == score)
				{
					CF_TranscriptionRule::DataEntry* pkEntry = SafeGetCSVFileDataEntryPtr<CF_TranscriptionRule>(nMapRes);
					if(NULL!=pkEntry && !pkEntry->_Stage.empty())
					{
						return RE_STARNUM_BE_LIMITED;
					}
				}
			}
			
			//检测出战角色是否符合gamestory的要求
			bool res = pkPlayer->GetBuddySchemeMgr().CheckSchemeReady(nMapRes, INIT_SCHEME_GROUPID, SchemeType_Offensive);
			if(!res)
			{
				return RE_TRANSFER_SCHEME_NOT_READY;
			}

			/*
			int lineNum = 0;
			if(nIndex != 0)
			{
				LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
				LC_ServerMapLogic_Place* pkMapLogic = pkWorldManager->GetMapLogic(nIndex);
				if(NULL != pkMapLogic)
				{
					lineNum = pkMapLogic->GetMap()->GetLocalLineNum();
				}
			}
			
			pkWorldManager->RequestChangePlayerLocation(pkPlayer, LMIOT_TRANSFER, 0, nMapRes, UT_Vec3Int(), lineNum);
			*/
			pkWorldManager->RequestChangePlayerLocation(pkPlayer, LMIOT_TRANSFER, 0, nMapRes, UT_Vec3Int(), nLineIndex);
		}
		break;
	case TT_TRANSPORT_NORMAL_MAP:
		{
			CF_TransferArea* pkTransferArea = CF_TransferArea::GetSingletonPtr();
			CF_TransferArea::DataEntry* pkAreaData = pkTransferArea->GetEntryPtr(nIndex);
			if (!pkAreaData)
			{
				return RE_FAIL;
			}
			CF_WorldMapList::DataEntry* pkMapEntry = CF_WorldMapList::GetSingletonPtr()->GetEntryPtr(pkAreaData->_lMapID);
			if (pkMapEntry == NULL)
			{
				return RE_FAIL;
			}
			if(pkMapEntry->_CanChgLocation <= 0 && UT_ServerHelper::IsInCross(pkPlayer) == true)
			{
				MG_Message errorMsg;
				errorMsg.m_iMessageID = RE_CANNOT_TRANS_TO_NORMAL_MAP;
				pkPlayer->SendMsgToClient(MGPT_MESSAGE, &errorMsg);
				return RE_FAIL;
			}
			pkWorldManager->RequestChangePlayerLocation(pkPlayer, LMIOT_TRANSFER, 0, pkAreaData->_lMapID, pkAreaData->_kPos);
		}
		break;
	case TT_TRANSPORT_GROW_UP:
		{
			ResultType nRetCode = _checkSwitchMapLine(pkPlayer, nMapRes, nIndex, bForceTribuMap);
			if ( nRetCode != RE_SUCCESS )
				return RE_FAIL;
			_trySwitchMapLine( pkPlayer, nMapRes, nIndex, nLineIndex );
		}
		break;
	case TT_TRANSPORT_CUR_MAP:
		{
			CF_TransferArea* pkTransferArea = CF_TransferArea::GetSingletonPtr();
			CF_TransferArea::DataEntry* pkAreaData = pkTransferArea->GetEntryPtr(nIndex);
			if (!pkAreaData)
			{
				return RE_FAIL;
			}
			if (pkAreaData->_lMapID != pkPlayer->GetMapResID())
			{
				return RE_FAIL;
			}
			pkWorldManager->RequestChangePlayerLocation(pkPlayer, LMIOT_TRANSFER, pkPlayer->GetMapLogicID(), pkPlayer->GetMapResID(), pkAreaData->_kPos);
		}
		break;
	default:
		break;
	}
	return RE_SUCCESS;
}

ResultType LC_ServerPlayerEnterLeaveManager::_checkSwitchMapLine( LC_ServerPlayer* pkPlayer, int32_t nMapResId, int32_t nMapLogicId, bool bForceTribuMap )
{
	LC_MapBase* pkMap = pkPlayer->GetMap();
	if (NULL == pkMap || 
				(
				!IS_COMMON_MAP(pkMap->GetMapLogicType()) 
				&& pkMap->GetMapLogicType() != MT_HOOK_MAP 
				)
				&& ( pkMap->GetMapLogicType() == MT_WATCHTOWER && pkMap->GetMapResID() != GetGlobalSetting.BestGuildMapID)
		)
	{
		return RE_FAIL;
	}
	CF_WorldMapList::DataEntry* pkMapEntry = CF_WorldMapList::GetSingletonPtr()->GetEntryPtr(nMapResId);
	if (pkMapEntry == NULL)
	{
		return RE_FAIL;
	}
	if(pkMapEntry->_CanChgLocation <= 0 && UT_ServerHelper::IsInCross(pkPlayer) == true)
	{
		MG_Message errorMsg;
		errorMsg.m_iMessageID = RE_CANNOT_TRANS_TO_NORMAL_MAP;
		pkPlayer->SendMsgToClient(MGPT_MESSAGE, &errorMsg);
		return RE_FAIL;
	}
	return RE_SUCCESS;
}

ResultType LC_ServerPlayerEnterLeaveManager::TrySwitchMapLine( LC_ServerPlayer* pkPlayer, int32_t nMapResId, int32_t nMapLogicId, int32_t nLineIndex )
{
	return _trySwitchMapLine( pkPlayer, nMapResId, nMapLogicId, nLineIndex );
}

ResultType LC_ServerPlayerEnterLeaveManager::_trySwitchMapLine( LC_ServerPlayer* pkPlayer, int32_t nMapResId, int32_t nMapLogicId, int32_t nLineIndex)
{
	pkPlayer->SaveCharPB(true);
	pkPlayer->SetEnterServerMode( ENTER_SERVER_MODE::eEnterServerMode_SwitchNode );

	MG_SS_Switch_Map_Line_Req reqData;
	
	reqData.nMapLine = nLineIndex;
	reqData.nMapResId = nMapResId;
	reqData.nPosX = pkPlayer->GetCurrentLocation().x;
	reqData.nPosY = pkPlayer->GetCurrentLocation().y;
	reqData.nPosZ = pkPlayer->GetCurrentLocation().z;
	reqData.nInOutType = LMIOT_TRANSFER;
	reqData.m_iPlayerGuildID = pkPlayer->GetGuildID();

	SendMsgToLogin(MGPT_SS_SWITCH_MAP_LINE_REQ, &reqData, pkPlayer->GetUserID(), pkPlayer->GetInstance());

	const unique_id_impl& instance = pkPlayer->GetInstance();
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "<%u>MGPT_SS_SWITCH_MAP_LINE_REQ c_cid:%d, c_citizenship:%d, c_uid:%d, MapId:%d, LineIdx:%d",
		UT_ServerHelper::GetServerId(), instance.detail(), instance.catagory(), pkPlayer->GetUserID(), nMapResId, nLineIndex);
	return RE_SUCCESS;
}
