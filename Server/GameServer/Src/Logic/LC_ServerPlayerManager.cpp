#include "LC_ServerPlayerManager.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerPlayerEnterLeaveManager.h"
#include "LC_GameStory_Manager.h"
#include "UT_TimeManager.h"
#include "LC_LogicIDManager.h"
#include "NW_Helper.h"

#include "NW_ServerSession.h"

#include "GameServerApp.h"

#include "UT_GameServerProfile.h"

#include "oss_360_define.h"

#ifdef WIN32
#include "XDebugDumper.h"
#endif

using namespace Utility;
using namespace StateMachine;
using namespace Protocol;
using namespace NetWork;
using namespace GameStory;

//#define		REPORT_TIMER		300
#define		REPORT_TIMER		60

//---------------------------------------------------------
LC_ServerPlayerManager::LC_ServerPlayerManager()
: LC_PlayerManagerBase()
, m_fLastUpdateTime(0.0f)
, m_iFreshmanCount(0)
, m_fLastClientCheckTime(0.0f)
, m_iInGamePlayerCount(0)
,m_fLastReportTime(0.00f)
{
	const Utility::MemPoolConfigEntry & plc = GetGlobalSetting.poolPlayer;

	m_pkServerPlayerPool = T_NEW_D Utility::UT_MemoryPool<LC_ServerPlayer>;
	m_pkServerPlayerPool->Init(plc.InitialCount, plc.IncCount, "LC_ServerPlayerManager.m_pkServerPlayerPool");
	m_pkServerPlayerPool->EnableAutoDec(plc.AutoDec, plc.AutoDecInterval, plc.AutoDecCount, plc.AutoDecMinCount);

	m_kUpdateTimer.SetTimer(GET_INTERVAL(CMN_INTER_IMM));
}
//---------------------------------------------------------
LC_ServerPlayerManager::~LC_ServerPlayerManager()
{
	T_SAFE_DELETE(m_pkServerPlayerPool);
}
//---------------------------------------------------------
//---------------------------------------------------------
static bool ProcessPlayer(LC_ServerPlayer * pkServerPlayer,float fCurrentTime, float fDeltaTime, float& m_fLastUpdateTime, int& m_iFreshmanCount, int& m_iInGamePlayerCount)
{
	UT_SERVER_PROFILE(kProfile, "ProcessPlayer", 0.002f);
	if (pkServerPlayer->IsInGame() && pkServerPlayer->GetLevel() < MIN_NOT_NEWBIE_LEVEL)
	{
		++m_iFreshmanCount;
	}

	if(/*!pkServerPlayer->IsInGame() || */NULL == pkServerPlayer->GetMap())
		return true;
	
	++m_iInGamePlayerCount;
	{
		UT_SERVER_PROFILE(kProfile, "UpdateWithServerTime", 0.001f);
		pkServerPlayer->UpdateWithServerTime(Float2Long(fCurrentTime));
	}

	{
		UT_SERVER_PROFILE(kProfile, "LC_ServerPlayer::Update", 0.001f);
		pkServerPlayer->Update(fCurrentTime, fDeltaTime);
		if ( !LC_ServerPlayer::GetUpdateResult() )
			return false;
	}	
	
	{
		pkServerPlayer->UpdateSync(GET_PROCESS_TIME, fCurrentTime);
	}
	return true;
}

void LC_ServerPlayerManager::DestroyPlayer(LC_PlayerBase* pkPlayer)
{
	if (NULL == pkPlayer)
		return;

	if (pkPlayer->IsInstance())
	{
		OnPlayerLoginIPSet(pkPlayer, ((LC_ServerPlayer*)pkPlayer)->GetLoginIP(), 0);
	}
	LC_ServerPlayer* pServerPlayer = (LC_ServerPlayer*)pkPlayer;
	const unique_id_impl& instance = pkPlayer->GetInstance();
	uint32_t uServerId = UT_ServerHelper::GetServerId();
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "<%u> DestroyPlayer:cid:%d,citizenship:%d,uid:%d,id:%d,buddyId:%d ",uServerId,instance.detail(),pServerPlayer->GetCitizenship(),pServerPlayer->GetUserID(),pServerPlayer->GetID(),pServerPlayer->GetBuddyID());
	LC_PlayerManagerBase::DestroyPlayer(pkPlayer);
}
//---------------------------------------------------------
void LC_ServerPlayerManager::Update(float fCurrentTime, float fDeltaTime)
{
	if (!m_kUpdateTimer.IsExpired())
		return;
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	//不作定时Update
	//通过同步消息触发Player Update 
	//如果不满意再进一步调整
	//if(fCurrentTime - m_fLastUpdateTime > 0.2f)
	{
		m_iFreshmanCount = 0;
		m_iInGamePlayerCount = 0;
		time_t curSecond = time(NULL);
		for( LC_PlayerIDPtrMapIter IterBe = m_kPlayerIDPtrMap.begin() ;IterBe != m_kPlayerIDPtrMap.end(); ++IterBe)
		{
			LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)(IterBe->second);
			IF_A_NA_PLAYER(pkServerPlayer)
				continue;
			//发送oss数据
			if(pkServerPlayer && pkServerPlayer->m_nSendOSSLoginTime 
			&& ((int)curSecond > pkServerPlayer->m_nSendOSSLoginTime))
			{
				oss_360_log_user_login(pkServerPlayer);
				pkServerPlayer->m_nSendOSSLoginTime = LC_Helper::GetDayEndTimestamp((uint32_t)curSecond, 1);
			}
			ProcessPlayer(pkServerPlayer,fCurrentTime,fDeltaTime,m_fLastUpdateTime,m_iFreshmanCount, m_iInGamePlayerCount);
		}
	}

	LC_PlayerBasePtrSet to_remove_players;
	LC_PlayerIDPtrMapIter IterBe = m_kPlayerCloneIDPtrMap.begin();
	LC_PlayerIDPtrMapIter IterEd = m_kPlayerCloneIDPtrMap.end();
	for( ;IterBe != IterEd; ++IterBe)
	{
		LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)(IterBe->second);
		IF_A_NA_PLAYER(pkServerPlayer)
				continue;
		ProcessPlayer(pkServerPlayer,fCurrentTime,fDeltaTime,m_fLastUpdateTime,m_iFreshmanCount, m_iInGamePlayerCount);
		if(pkServerPlayer->CheckUnSummorSoon(fCurrentTime))
		{
			pkServerPlayer->SummorFriendTalk(1, fCurrentTime);
		}

		if(pkServerPlayer->CheckUnSummor(fCurrentTime))
		{
			pkWorldManager->RemoveObjectFromMap(pkServerPlayer);	
			to_remove_players.insert(pkServerPlayer);
		}
	}

	for( LC_PlayerBasePtrSet::iterator it = to_remove_players.begin(); it != to_remove_players.end(); ++it)
	{
		DestroyPlayer(*it);
	}

	LC_PlayerManagerBase::Update(fCurrentTime, fDeltaTime);
	m_fLastUpdateTime = fCurrentTime;
	if ( fCurrentTime - m_fLastReportTime >= REPORT_TIMER )
	{
		m_fLastReportTime = fCurrentTime;
		oss_360_log_online_count(GetOnlinePlayerCount());
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "onlineCount:  %d, %d", GetOnlinePlayerCount(), m_kPlayerIDPtrMap.size());
	}


	//< 更新对象池
	m_pkServerPlayerPool->Update(fCurrentTime);
}

LC_ServerPlayer *LC_ServerPlayerManager::SummorControlPlayer(const unique_id_impl& charID, logic_id_type iMapLogicID, const Utility::UT_Vec3Int& pos, uint32_t bits, bool isRobot, LC_ServerPlayer* Owner, user_id_type userID)
{
	static LC_ServerPlayerEnterLeaveManager* inst = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
	static LC_LogicIDManager* pkLogicIDManager	= LC_LogicIDManager::GetSingletonPtr();
	object_id_type objectID = pkLogicIDManager->RequestLogicID(LOT_PLAYER_CLONE);
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "serverid:<%u> %s CreatePlayer id[%d], c_cid[%d], c_citizenship[%d]", UT_ServerHelper::GetServerId(), __FUNCTION__, objectID, charID.detail(), charID.catagory());
	LC_ServerPlayer * player = inst->ClonePlayer(Owner, userID, objectID, charID, bits, isRobot);
	if(player!=NULL)
	{
		player->GetAsAControlee().m_InitPos = pos;
		player->GetAsAControlee().m_MapLogicID = iMapLogicID;
	}
	return player;
}
LC_ServerPlayer *LC_ServerPlayerManager::NewSummorControlPlayer(const unique_id_impl& charID, uint32_t bits, bool isRobot, LC_ServerPlayer* Owner, user_id_type userID)
{
	static LC_ServerPlayerEnterLeaveManager* inst = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
	static LC_LogicIDManager* pkLogicIDManager	= LC_LogicIDManager::GetSingletonPtr();
	object_id_type objectID = pkLogicIDManager->RequestLogicID(LOT_PLAYER_CLONE);
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "<%u>%s CreatePlayer c_cid:%d, c_citizenship:%d, id:%d", UT_ServerHelper::GetServerId(), __FUNCTION__, charID.detail(), charID.catagory(), userID);
	LC_ServerPlayer * player = inst->ClonePlayer(Owner, userID, objectID, charID, bits, isRobot);
	return player;
}

//---------------------------------------------------------
//---------------------------------------------------------
void LC_ServerPlayerManager::BroadCast(uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody, GameLogic::LC_PlayerOperator* pkFilter)
{
	MulticastMsgToGateway(m_kPlayerIDPtrMap, usMsgType, pkMsgBody, pkFilter);
}
//---------------------------------------------------------
LC_PlayerBase* LC_ServerPlayerManager::_allocPlayer()
{
	return m_pkServerPlayerPool->Alloc();
}
//---------------------------------------------------------
void LC_ServerPlayerManager::_freePlayer( LC_PlayerBase* pkPlayer )
{
	if(IS_PLAYER_CLONE_ID(pkPlayer->GetID()))
	{
		LC_LogicIDManager::GetSingletonPtr()->ReleaseLogicID(LOT_PLAYER_CLONE,pkPlayer->GetID());
	}
	if (pkPlayer->GetBuddyAllocateId() != 0)
	{
		LC_LogicIDManager::GetSingletonPtr()->ReleaseLogicID(LOT_BUDDY,pkPlayer->GetBuddyAllocateId());
	}
	pkPlayer->ReUse();
	m_pkServerPlayerPool->Free((LC_ServerPlayer*)pkPlayer);
	pkPlayer = NULL;
}

int LC_ServerPlayerManager::GetInGamePlayerCountLessLevel(int level)
{
	m_iFreshmanCount = 0;
	for( LC_PlayerIDPtrMapIter IterBe = m_kPlayerIDPtrMap.begin();IterBe != m_kPlayerIDPtrMap.end(); ++IterBe)
	{
		LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)(IterBe->second);
		IF_A_NA_PLAYER(pkServerPlayer)
			continue;
		if (pkServerPlayer->IsInGame() && pkServerPlayer->GetLevel() <= level)
			++m_iFreshmanCount;
	}
	return m_iFreshmanCount;
}


mem::vector<LC_ServerPlayer*> LC_ServerPlayerManager::GetAllPlayerPtr()
{
	mem::vector<LC_ServerPlayer*> playerPtrs;
	playerPtrs.reserve(m_kPlayerIDPtrMap.size());
	for (LC_PlayerIDPtrMapIter it = m_kPlayerIDPtrMap.begin(); it != m_kPlayerIDPtrMap.end(); ++it)
	{
		LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)((*it).second);
		playerPtrs.push_back(pkServerPlayer);
	}
	return playerPtrs;
}

int LC_ServerPlayerManager::GetControllerPlayerCount()
{
	return m_kPlayerMainControllerUIDPtrMap.size();
}

void LC_ServerPlayerManager::GetAllPlayerCharID(unique_id_set& charIDs)
{
	for (LC_PlayerIDPtrMapIter it = m_kPlayerIDPtrMap.begin(); it != m_kPlayerIDPtrMap.end(); ++it)
	{
		LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)((*it).second);
		charIDs.insert(pkServerPlayer->GetInstance());
	}
}



LC_ServerPlayer* LC_ServerPlayerManager::CreatePlayerBySession(user_id_type lUserID, const unique_id_impl& charID, NW_ServerSession* session)
{
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "<%u>%s CreatePlayer c_cid:%d, c_citizenship:%d, id:%d", UT_ServerHelper::GetServerId(), __FUNCTION__, charID.detail(), charID.catagory(), lUserID);
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)CreatePlayer(GET_PLAYERID(lUserID), charID, 1, (6<<24) + PLY_DEF_INSTANCE_BITS);
	if(pkPlayer)
	{
		pkPlayer->SetSession(session);
		pkPlayer->SetUserID(lUserID);
		pkPlayer->SetInstance(charID);
	}
	return pkPlayer;
}

LC_ServerPlayer* LC_ServerPlayerManager::GetPlayerBySession(user_id_type lUserID, const unique_id_impl& charID, NW_ServerSession* session)
{
	LC_ServerPlayer * pkPlayer = (LC_ServerPlayer*)FindPlayer(charID);
	if(NULL==pkPlayer || pkPlayer->GetSession() != session)
	{
		return NULL;
	}
	return pkPlayer;

}
StringType LC_ServerPlayerManager::GetRTInfo(void)
{
	StringStreamType sStream;
	sStream << "----[LC_ServerPlayerManager Begin]----\n";
	sStream << m_pkServerPlayerPool->GetRTInfo();
	sStream << "----[LC_ServerPlayerManager End]----\n";
	return sStream.str();
}
void LC_ServerPlayerManager::OnPlayerLoginIPSet(GameLogic::LC_PlayerBase*pkPlayer, uint32_t preIP, uint32_t postIP)
{
	if (NULL==pkPlayer || preIP==postIP)
	{
		return;
	}

	if (preIP)
	{
		LC_IP2IDSIndices::iterator it = m_kIP2IDSIndices.find(preIP);
		if (it != m_kIP2IDSIndices.end())
		{
			it->second.erase(pkPlayer->GetID());
			if (it->second.empty())
			{
				m_kIP2IDSIndices.erase(it);
			}
		}
	}

	if (postIP)
	{
		m_kIP2IDSIndices[postIP].insert(pkPlayer->GetID());
	}
}

uint32_t LC_ServerPlayerManager::GetOnlinePlayerCount() const
{
	uint32_t onlineCount = 0;
	LC_IP2IDSIndices::const_iterator cit = m_kIP2IDSIndices.begin();
	for (; cit != m_kIP2IDSIndices.end(); ++cit)
	{
		onlineCount += (uint32_t)cit->second.size();
	}
	return onlineCount;
}

const object_id_set& LC_ServerPlayerManager::GetIDSetAcrdLoginIP(uint32_t ip)
{
	return m_kIP2IDSIndices[ip];
}
void LC_ServerPlayerManager::PostPlayerOnline(GameLogic::LC_PlayerBase* pkPlayer)
{
	for( LC_PlayerIDPtrMapIter IterBe = m_kPlayerIDPtrMap.begin();IterBe != m_kPlayerIDPtrMap.end(); ++IterBe)
	{
		LC_PlayerBase* pkServerPlayer = (LC_PlayerBase*)((*IterBe).second);
		IF_A_NA_PLAYER(pkServerPlayer)
			continue;
		if ( pkServerPlayer == pkPlayer)
			continue;
		pkServerPlayer->GetSocietyAsset()->RegDirtyIDs(pkPlayer->GetID());
	}
}
void LC_ServerPlayerManager::PrePlayerOffline(GameLogic::LC_PlayerBase* pkPlayer)
{
	for( LC_PlayerIDPtrMapIter IterBe = m_kPlayerIDPtrMap.begin();IterBe != m_kPlayerIDPtrMap.end(); ++IterBe)
	{
		LC_PlayerBase* pkServerPlayer = (LC_PlayerBase*)((*IterBe).second);
		IF_A_NA_PLAYER(pkServerPlayer)
			continue;
		if ( pkServerPlayer == pkPlayer)
			continue;
		pkServerPlayer->GetSocietyAsset()->RegDirtyIDs(pkPlayer->GetID());
	}
}