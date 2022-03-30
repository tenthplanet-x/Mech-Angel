#include "ClientPCH.h"
#include "UT_Curl.h"
#include <base64.h>
#include "GlobalSettings.h"
#include "LC_ClientArenaPlayBackManager.h"
#include "LC_ClientToyPetManager.h"
#include "LC_ItemFactoryBase.h"
#include "UT_FileManagerBase.h"
#include "LC_ClientLogicManager.h"
#include "NW_Game2ClientSession.h"
#include "LC_ClientGamePlayer.h"
#include "LC_ClientNPCManager.h"
#include "HL_FromGameServer.h"
#include "UI_UIManager.h"
#include "LC_TranscriptionDataType.h"
#include "LC_ClientWorldManager.h"
#include "LC_ClientToyPetManager.h"
#include "LC_ClientNetPlayerManager.h"
#include "SC_ClientScriptInterface.h"
#include "SC_ClientSkillInterface.h"
#include "LC_ClientReportManager.h"

using namespace GameLogic;
using namespace Utility;

LC_ClientArenaPlayBackManager::LC_ClientArenaPlayBackManager()
:m_isign(-1)
,m_fBeginTime(0.0f)
,m_nUniqueId(0)
{
	ReUse();
}

LC_ClientArenaPlayBackManager::~LC_ClientArenaPlayBackManager()
{
}

void LC_ClientArenaPlayBackManager::Init()
{
	ReUse();
}

void LC_ClientArenaPlayBackManager::UnInit()
{
	ReUse();
}

void LC_ClientArenaPlayBackManager::ReUse()
{
	m_sContentStream.str("");
	m_playbackInfoMsg.clear_pinfo();
	m_nUniqueId = 0;
	m_isign = -1;
	m_fBeginTime = 0.0f;
	m_mobjid.clear();
	m_mPlayerLeft.clear();
	m_mPlayerRight.clear();
}

void LC_ClientArenaPlayBackManager::Packet(uint16_t iType, char* pMsg, uint16_t wSize, float deltime)
{
	

	msg_define::arenaplayback_progress_info* pkinfo = m_playbackInfoMsg.add_pinfo();
	if (pkinfo)
	{
		pkinfo->set_itype(iType);
		pkinfo->set_pmsg(pMsg,wSize);
		pkinfo->set_wsize(wSize);
		pkinfo->set_deltime(deltime);
	}
	
}

void LC_ClientArenaPlayBackManager::FillRuneLevelInfoPB(LC_ClientGamePlayer* player, ::google::protobuf::RepeatedField< ::google::protobuf::int32 > & item_info, ::google::protobuf::RepeatedField< ::google::protobuf::int32 > & level_info)
{
	int cid = player->GetCharType();
	for ( int i = 0; i < 4; i++ )
	{
		LC_BackPackEntry* itemEntry = player->GetRuneEntry(cid, i);
		if (itemEntry == NULL || !itemEntry->GetValid())
		{
			item_info.Add(-1);
			level_info.Add(-1);
		}
		else
		{
			int itemid = itemEntry->GetTypeID();
			item_info.Add(itemid);
			LC_ItemDescriptionEntry* pkItemDesc = LC_ItemFactoryBase::GetSingletonPtr()->GetItemDescriptionEntry(itemid);
			level_info.Add(pkItemDesc->GetLevel());
		}
	}
}

bool LC_ClientArenaPlayBackManager::LoadStream()
{
	const std::string& sStr = m_sContentStream.str();
	m_playbackInfoMsg.ParseFromString(sStr);
	return m_playbackInfoMsg.pinfo_size() > 0;
}

bool LC_ClientArenaPlayBackManager::LoadFile(unique_id_type uiID)
{
	char szFileName[64] = {0};
	sprintf(szFileName,"%s/%lld.data", STR_ARENAPLAYBACK_DIR, uiID);

	FILE* f = fopen(szFileName, "rb");
	if (NULL == f)
		return false;
	
	fseek(f, 0, SEEK_END);
	unsigned int sz = ftell(f);
	fseek(f, 0, SEEK_SET);

	std::vector<char> sVecContent;
	sVecContent.resize(sz);
	fread(&sVecContent[0],sz, 1, f);

	fclose(f);
	
	m_playbackInfoMsg.ParseFromArray(&sVecContent[0],sz);


	return m_playbackInfoMsg.pinfo_size() > 0;
}
bool LC_ClientArenaPlayBackManager::NewLoadFile(int userId,int64_t nFightId)
{
	/* SinglePlayerTranscribeFight * p_back = GameLogic::LC_Helper::GetTranscribeFightInfoByUserIDFightId(userId,nFightId);
	 if(p_back)
	 {
		m_playbackInfoMsg.CopyFrom(p_back->m_playbackInfoMsg);
		m_nUniqueId = nFightId;
		return true;
	 }*/
	 return false;
}
void LC_ClientArenaPlayBackManager::SaveFile(int64_t uID)
{
	/*if (uID != m_nUniqueId)
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI, "LC_ClientArenaPlayBackManager::SaveFile id前后不一致 uID=%I64d, m_nUniqueId=%I64d", uID, m_nUniqueId);
		return;
	}
	::CreateDirectory(STR_ARENAPLAYBACK_DIR , NULL );

	std::string sContent;
	m_playbackInfoMsg.SerializeToString(&sContent);

	char szFileName[64] = {0};
	sprintf(szFileName,"%s/%lld.data", STR_ARENAPLAYBACK_DIR, uID);

	FILE* f = fopen(szFileName,"wb+");
	if(!f)
	{
		DWORD errorValue = GetLastError();
		return;
	}

	fwrite(sContent.c_str(),sContent.size(), 1, f);
	fclose(f);*/
}

void LC_ClientArenaPlayBackManager::Update(float fCurrentTime,float fDeltaTime)
{
	if(m_isign < 0)
		return;
	int isize = m_playbackInfoMsg.pinfo_size();
	if ( m_isign < isize)
	{
		LC_ClientGamePlayer* pkGamePlayer = (CLIENT_GET_LOGIC_MANAGER())->GetOperationPlayer();
		if (pkGamePlayer->IsCreateVisibleData())
		{
			pkGamePlayer->ShowAllBuddyVisibleData(false);
		}
		for (;m_isign < isize; ++m_isign)
		{
			const ::msg_define::arenaplayback_progress_info& pkinfo = m_playbackInfoMsg.pinfo(m_isign);
			float deltime = pkinfo.deltime();
			if (fCurrentTime - m_fBeginTime >= deltime)
			{
				int iType = pkinfo.itype();
				if(iType == MGPT_RLT_SKILL_EVENT)
					GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI, "战斗回放播放  usMsgType=%d wSize=%d", pkinfo.itype(), pkinfo.wsize());
				_ParsePacket(pkinfo.itype(), (char*)pkinfo.pmsg().c_str(), pkinfo.wsize());
			}
			else
				break;
		}
	}
	else 
	{
		ReUse();
	}
}

void GameLogic::LC_ClientArenaPlayBackManager::DestoryAll()
{
	LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
	if (pkNetPlayerManager)
		pkNetPlayerManager->DestroyAllPlayer();
	LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
	if (pkNPCManager )	
		pkNPCManager->DestroyAllNPC();
}

bool GameLogic::LC_ClientArenaPlayBackManager::CheckFileValid(int64_t id)
{
	char szFileName[64] = {0};
	sprintf(szFileName,"%s/%lld.data", STR_ARENAPLAYBACK_DIR, id);

	if (Utility::IsFileExist(szFileName))
	{
		return true;
	}

	return false;
}

void GameLogic::LC_ClientArenaPlayBackManager::Begin()
{
	m_isign = 0;
	m_fBeginTime = GET_PROCESS_TIME;
	m_mobjid.clear();
	m_mPlayerLeft.clear();
	m_mPlayerRight.clear();
}

bool GameLogic::LC_ClientArenaPlayBackManager::RecvMessage(MG_MessageBase& rkMessage)
{
	MG_MessageHandlerInterface::RecvMessage(rkMessage);

	BEGIN_MESSAGE_HANDLE
		MESSAGE_HANDLE(MGT_TIMER_TICK,_event_TimerTick);
	END_MESSAGE_HANDLE

	return true;
}

object_id_type GameLogic::LC_ClientArenaPlayBackManager::GetRealID(object_id_type id)
{
	mem::map<object_id_type, object_id_type>::iterator iter = m_mobjid.begin();
	for (;iter!=m_mobjid.end();++iter)
	{
		if (iter->second == id)
		{
			return iter->first;
		}
	}
	return 0;
}

object_id_type GameLogic::LC_ClientArenaPlayBackManager::GetTemID(object_id_type id)
{
	mem::map<object_id_type, object_id_type>::iterator iter = m_mobjid.find(id);
	if (iter != m_mobjid.end())
	{
		return iter->second;
	}
	return 0;
}

object_id_type GameLogic::LC_ClientArenaPlayBackManager::_ObjectIDTransform(object_id_type id)
{
	object_id_type newid = id;
	LC_ClientLogicManager* pkLogicManger = (LC_ClientLogicManager*)LC_ClientLogicManager::GetSingletonPtr();
	LC_ClientGamePlayer* pkPlayer = pkLogicManger->GetOperationPlayer();
	if (id == pkPlayer->GetID() || pkPlayer->IsBuddy(id))
	{
		newid += 100;
		mem::map<object_id_type, object_id_type>::iterator iter = m_mobjid.begin();
		for (;iter!=m_mobjid.end();++iter)
		{
			if (iter->second == newid)
			{
				newid += 100;
				break;
			}
		}
	}
	m_mobjid.insert(make_pair(id, newid));
	return newid;
}

void GameLogic::LC_ClientArenaPlayBackManager::SetPlayerMap(LC_ClientNetPlayer* player, bool bLeft)
{
	int chartype = player->GetCharType();
	if (bLeft)
	{
		int size = m_mPlayerLeft.size();
		m_mPlayerLeft.push_back(player);
	}
	else
	{
		int size = m_mPlayerRight.size();
		m_mPlayerRight.push_back(player);
	}
}

void GameLogic::LC_ClientArenaPlayBackManager::ErasePlayer(object_id_type objID)
{
	mem::vector<LC_ClientNetPlayer*>::iterator itor;
	for (itor = m_mPlayerLeft.begin(); itor != m_mPlayerLeft.end(); ++itor)
	{
		LC_ClientNetPlayer* pkPlayer = *itor;
		if (pkPlayer->GetID() == objID)
		{
			m_mPlayerLeft.erase(itor);
			return;
		}
	}

	for (itor = m_mPlayerRight.begin(); itor != m_mPlayerRight.end(); ++itor)
	{
		LC_ClientNetPlayer* pkPlayer = *itor;
		if (pkPlayer->GetID() == objID)
		{
			m_mPlayerRight.erase(itor);
			return;
		}
	}
}

bool GameLogic::LC_ClientArenaPlayBackManager::CheckPlayerSideByOwnerID(int ownerid)
{
	if (m_mPlayerLeft.size() <= 0)
	{
		return true;
	}
	LC_ClientNetPlayer* player = m_mPlayerLeft[0];
	object_id_type id = player->GetOwnerId();
	return ownerid == id;
}

bool GameLogic::LC_ClientArenaPlayBackManager::CheckPlayerSideByID(object_id_type objID)
{
	for (int i = 0; i < m_mPlayerLeft.size(); ++i)
	{
		LC_ClientNetPlayer* pkPlayer = m_mPlayerLeft[i];
		if (pkPlayer->GetID() == objID)
		{
			return true;
		}
	}
	return false;
}

bool GameLogic::LC_ClientArenaPlayBackManager::IsPlayerInList(object_id_type id)
{
	mem::map<object_id_type, object_id_type>::iterator iter = m_mobjid.find(id);
	return iter != m_mobjid.end();
}

int GameLogic::LC_ClientArenaPlayBackManager::GetPlayerCount(bool bLeft)
{
	if (bLeft)
		return m_mPlayerLeft.size();
	else
		return m_mPlayerRight.size();
}

LC_ClientNetPlayer* GameLogic::LC_ClientArenaPlayBackManager::GetNetPlayerBySideAndIndex(bool bLeft, int index)
{
	if (bLeft && index < m_mPlayerLeft.size())
	{
		return m_mPlayerLeft[index];
	}
	else if (!bLeft && index < m_mPlayerRight.size())
	{
		return m_mPlayerRight[index];
	}
	return NULL;
}

void GameLogic::LC_ClientArenaPlayBackManager::_event_TimerTick(MG_MessageBase& rkMessage)
{
	float fCurrentTime = rkMessage.m_fParam1;
	float fDeltaTime = rkMessage.m_fParam2;

	Update(fCurrentTime, fDeltaTime);
}

void GameLogic::LC_ClientArenaPlayBackManager::_ParsePacket(int iType, char* pMsg, uint16_t wSize)
{
	LC_ClientGamePlayer* pkGamePlayer = (CLIENT_GET_LOGIC_MANAGER())->GetOperationPlayer();
	NW_Game2ClientSession* pkSession = pkGamePlayer->GetSession();

	switch (iType)
	{
	case MGPT_RLT_GAME_EVENT:
		_handlerRltGameEvent(pkSession, pMsg, wSize);
		break;
	case MGPT_SC_SKY_ALL_SHOW_INFO:
		HL_FromGameServer::_handlerSkyAllShowInfo(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_SKY_ROUND_SHOW_INFO:
		HL_FromGameServer::_handlerSkyRoundShowInfo(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_PLAYER_CONTROLLERS_PROPERTY_IN_FOV:
		HL_FromGameServer::_handlerSYNCPlayerControllersPropertyInFOV(pkSession,pMsg,wSize, true);
		break;
	case MGPT_SYNC_PLAYERS_ENTER_IN_FOV:
		HL_FromGameServer::_handlerSyncPlayersEnter(pkSession, pMsg, wSize, true);
		break;
	case MGPT_SYNC_PLAYERS_LEAVE_IN_FOV:
		HL_FromGameServer::_handlerSyncPlayersLeave(pkSession, pMsg, wSize, true);
		break;
	case MGPT_SYNC_PLAYERS_PROPERTY_IN_FOV:
		HL_FromGameServer::_handlerSyncPlayerPropertyInFOV(pkSession, pMsg, wSize, true);
		break;
	case MGPT_SYNC_PLAYER_PROPERTY:
		HL_FromGameServer::_handlerSyncPlayerProperty(pkSession,pMsg,wSize, true);
		break;
	case MGPT_SYNC_PLAYERS_HPMPCHANGE_IN_FOV:
		HL_FromGameServer::_handlerSyncPlayersHPMPChange(pkSession,pMsg,wSize, true);
		break;
	case MGPT_SYNC_PLAYERS_PROPERTY_IN_FOV_FAST:
		HL_FromGameServer::_handlerSyncPlayerFastPropertyInFOV(pkSession, pMsg, wSize, true);
		break;
	case MGPT_SYNC_PLAYERS_LOCATION_IN_FOV:
		HL_FromGameServer::_handlerSyncPlayersLocationChange(pkSession, pMsg, wSize, true);
		break;
	case MGPT_SYNC_PLAYERS_ANGLE_IN_FOV:
		HL_FromGameServer::_handlerSyncPlayersAngleChange(pkSession, pMsg, wSize, true);
		break;
	case MGPT_SYNC_PLAYERS_SOCIETY_IN_FOV:
		HL_FromGameServer::_handlerSyncPlayerSocietyInFOV(pkSession,pMsg,wSize, true);
		break;
	case MGPT_SYNC_PLAYERS_SKILL_STATE_IN_FOV:
		HL_FromGameServer::_handlerSyncPlayerSkillStateInFOV(pkSession,pMsg,wSize, true);
		break;
	case MGPT_SYNC_PLAYER_SUB_INFO_IN_FOV:
		HL_FromGameServer::_handlerSyncPlayerSubordinateInFOV(pkSession,pMsg,wSize, true);
		break;
	case MGPT_SYNC_PLAYERS_PVECHANGE_IN_FOV:
		HL_FromGameServer::_handlerSyncPlayersPVEStateChange(pkSession, pMsg, wSize, true);
		break;
	case MGPT_SYNC_PLAYERS_STATECHANGE_IN_FOV:
		HL_FromGameServer::_handlerSyncPlayersStateChange(pkSession, pMsg, wSize, true);
		break;
	case MGPT_RLT_PLAYER_JUMP:
		HL_FromGameServer::_handlerSyncGamePlayerJump(pkSession,pMsg,wSize, true);
		break;
	case MGPT_PUNCH_MOVE_PLAYER:
		HL_FromGameServer::_handlerPunchMovePlayer(pkSession,pMsg,wSize, true);
		break;
	case MGPT_SYNC_SKILL_STATE:
		HL_FromGameServer::_handlerSyncSkillState(pkSession,pMsg,wSize, true);
		break;
	case MGPT_RLT_SKILL_EVENT:
		HL_FromGameServer::_handlerRltSkillEvent(pkSession, pMsg, wSize, true);
		break;
	case MGPT_SYNCBULLETINFO:
		HL_FromGameServer::_handlerRltSkillEventMerged(pkSession, pMsg, wSize, true);
		break;
	case MGPT_RLT_SKILL_EVENT_EX:
		HL_FromGameServer::_handlerRltSkillEventEx(pkSession, pMsg, wSize, true);
		break;
	case MGPT_RLT_PLAYER_EXECUTE_SKILL_TYPE_TARGET:
		HL_FromGameServer::_handlerPlayerExecuteSkillTypeTarget(pkSession,pMsg,wSize, true);
		break;
	case MGPT_RLT_PLAYER_EXECUTE_SKILL_TYPE_DIR:
		HL_FromGameServer::_handlerPlayerExecuteSkillTypeDir(pkSession,pMsg,wSize, true);
		break;
	case MGPT_RLT_PLAYER_EXECUTE_SKILL_TYPE_LOCATION:
		HL_FromGameServer::_handlerPlayerExecuteSkillTypeLocation(pkSession,pMsg,wSize, true);
		break;
	case MGPT_RLT_PLAYER_INTERRUPT_SKILL:
		HL_FromGameServer::_handlerPlayerInterruptSkill(pkSession,pMsg,wSize, true);
		break;
	case MGPT_SYNC_PLAYER_EXECUTE_SKILL_TYPE_TARGET:
		HL_FromGameServer::_handlerSyncPlayerExecuteSkillTypeTarget(pkSession,pMsg,wSize, true);
		break;
	case MGPT_RLT_PLAYER_DEAD:
		HL_FromGameServer::_handlerRltPlayerDead(pkSession,pMsg,wSize, true);
		break;
	case MGPT_SYNC_NPCS_ENTER_IN_FOV:
		HL_FromGameServer::_handlerSyncNpcsEnter(pkSession,pMsg,wSize, true);
		break;
	case MGPT_SYNC_NPCS_LEAVE_IN_FOV:
		HL_FromGameServer::_handlerSyncNpcsLeave(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_NPCS_MOVE_IN_FOV:
		HL_FromGameServer::_handlerSyncNPCMoveInFOV(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_NPCS_STATE_IN_FOV:
		HL_FromGameServer::_handlerSyncNPCStateFOV(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_NPCS_ANGLE_IN_FOV:
		HL_FromGameServer::_handlerSyncNPCAngleFOV(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_NPCS_PROPERT_IN_FOV:
		HL_FromGameServer::_handlerSyncNPCPropertyInFOV(pkSession,pMsg,wSize, true);
		break;
	case MGPT_REFRESH_NPC_LEVEL:
		HL_FromGameServer::_handlerRefreshNPCLevel(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_NPCS_SKILL_STATE_IN_FOV:
		HL_FromGameServer::_handlerSyncNPCSkillStateInFOV(pkSession,pMsg,wSize);
		break;
	case MGPT_PUNCH_MOVE_NPC:
		HL_FromGameServer::_handlerPunchMoveNpc(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_NPC_EXECUTE_SKILL_TYPE_LOCATION:
		HL_FromGameServer::_handlerNPCExecuteSkillTypeLocation(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_NPC_EXECUTE_SKILL_TYPE_TARGET:
		HL_FromGameServer::_handlerNPCExecuteSkillTypeTarget(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_NPC_EXECUTE_SKILL_TYPE_DIR:
		HL_FromGameServer::_handlerNPCExecuteSkillTypeDir(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_NPC_INTERRUPT_SKILL:
		HL_FromGameServer::_handlerNPCInterruptSkill(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_NPC_DEAD:
		HL_FromGameServer::_handlerRltNPCDead(pkSession,pMsg,wSize, true);
		break;
	case MGPT_SYNC_AICONTROL_LOCATION:
		_handlerSyncGamePlayerAIControlLocation(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_SET_ACTIVATE_BUDDY_REPLY:
		_handlerSCSetActivateBuddyReply(pkSession,pMsg,wSize);
		break;
	case MGPT_CONTROLLER_PLAYER_INFO:
		HL_FromGameServer::_handlerNetPlayerActiveBuddy(pkSession,pMsg,wSize, true);
		break;
	case MGPT_SC_BUDDY_REPLY:
		HL_FromGameServer::_handlerBuddyReply(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_RESPAWN:
		HL_FromGameServer::_handlerRltRespawn(pkSession,pMsg,wSize);
		break;
	case MGPT_CS_BIND_BUDDY_INSTANCE_REQ:
		HL_FromGameServer::_handlerRltRespawn(pkSession,pMsg,wSize);
		break;
	case MGPT_REFRESH_PLAYER_BIND_BUDDY:
		HL_FromGameServer::_handlerRefreshPlayerBindBuddy(pkSession,pMsg,wSize, true);
		break;
	default:
		break;
	}
}

void GameLogic::LC_ClientArenaPlayBackManager::_handlerRltGameEvent(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	MG_PB<msg_define::game_event> pbMsg;
	if (pbMsg.Decode(pMsg,wSize))
	{
		LC_GameEvent kGameEvent;
		kGameEvent.GetFromProtocol(pbMsg.m_value);
		StringType	 kStringParam = STDSTR2TPSTR(pbMsg.m_value.string_param());
		HandlerGameEvent(kGameEvent,kStringParam);

		pkUIManager->HandlerGameEvent(kGameEvent);
	}
}

void GameLogic::LC_ClientArenaPlayBackManager::HandlerGameEvent(GameLogic::LC_GameEvent& rGameEvent,StringType& rEventStringParam)
{
	int iGameEventType = rGameEvent.GetEventType();

	switch (iGameEventType)
	{
	case GLET_TRANSCRIPTION_EVENT:
		_handlerTranscriptionEvent(rGameEvent);
		break;
	default:
		break;
	}
}

void GameLogic::LC_ClientArenaPlayBackManager::_handlerTranscriptionEvent(GameLogic::LC_GameEvent& rGameEvent)
{
	long lEventType = rGameEvent.GetEventParam(EP_TRANSCRIPTION_EVENT_TYPE);
	long lValue		= rGameEvent.GetEventParam(EP_TRANSCRIPTION_EVENT_PARAM_1);

	switch (lEventType)
	{
	case EVENT_TYPE_SOLODULE_PREPARE:
		{
			LC_GameEvent kGameEvent;
			kGameEvent.SetEventType(GLET_ARENA_TIME_PREPARE);
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_1,lValue);
			UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
			pkUIManager->HandlerGameEvent(kGameEvent);
		}
		break;
	case EVENT_TYPE_SOLODULE_BATTLE:
		{
			LC_GameEvent kGameEvent;
			kGameEvent.SetEventType(GLET_ARENA_TIME_BATTLE);
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_1,lValue);
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_2,1);
			UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
			pkUIManager->HandlerGameEvent(kGameEvent);
		}
		break;
	case EVENT_TYPE_FIGHT_RECORD_END:
		{
			LC_ClientGamePlayer* pkGamePlayer = (CLIENT_GET_LOGIC_MANAGER())->GetOperationPlayer();
			pkGamePlayer->RequestLeaveTranscription();

			DestoryAll();
		}
		break;
	default:
		break;
	}
}

void GameLogic::LC_ClientArenaPlayBackManager::_handlerSyncGamePlayerAIControlLocation(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	PlayerAILocationEntry msg;
	if(!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SyncPlayerLocation::PlayerLocationEntry解析错误");
		return;
	}

	object_id_type lID = msg.m_lPlayerID;
	if (!IsPlayerInList(lID))
	{
		return;
	}

	lID = GetTemID(lID);

	LC_ClientNetPlayerManager* pkPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
	LC_PlayerBase* pkPlayer = pkPlayerManager->FindPlayer(lID);
	if (NULL != pkPlayer)
	{
		const SE_SceneTerrainLocalInfo& rkTerrainInfo1 = SE_SceneManager::GetSingletonPtr()->GetWalkableInfo(Int2Float(msg.m_lCurrentLocationX), Int2Float(msg.m_lCurrentLocationY));
		const SE_SceneTerrainLocalInfo& rkTerrainInfo2 = SE_SceneManager::GetSingletonPtr()->GetWalkableInfo(Int2Float(msg.m_lTargetLocationX), Int2Float(msg.m_lTargetLocationY));

		UT_Vec2Float kDir = UT_MathBase::GetZRotDir(LC_Helper::Angle_Char2Float(msg.m_chCurrentDirAngle));
		UT_Vec3Int kCurrentLocation(msg.m_lCurrentLocationX,msg.m_lCurrentLocationY,rkTerrainInfo1._fLocalHeight);
		UT_Vec3Int kTargetLocation(msg.m_lTargetLocationX,msg.m_lTargetLocationY,rkTerrainInfo2._fLocalHeight);

		pkPlayer->SetCurrentLocation(kCurrentLocation);
		pkPlayer->SetTargetLocation(kTargetLocation);
		pkPlayer->SetForwardDir(kDir);
		pkPlayer->SetTargetForwardDir(kDir);
	}
}

void GameLogic::LC_ClientArenaPlayBackManager::_handlerSCSetActivateBuddyReply(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SC_SetActivateBuddyReply BuddyDataMsg;
	if(!BuddyDataMsg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "MG_SC_SetActivateBuddyReply解析错误");
		return;
	}

	if (BuddyDataMsg.m_nIndex >= m_mPlayerLeft.size())
		return;

	for (int i = 0; i < m_mPlayerLeft.size(); ++i)
	{
		LC_ClientNetPlayerCommander* commander = m_mPlayerLeft[i]->GetCommander();
		if (commander != NULL) 
		{
			commander->m_lCurrentPlayerID = m_mPlayerLeft[BuddyDataMsg.m_nIndex]->GetID();
		}

	}
}



