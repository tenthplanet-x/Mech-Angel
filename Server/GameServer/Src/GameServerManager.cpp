#include "GameServerApp.h"
#include "GameServerManager.h"
#include "MG_ProtocolStruct.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerPlayer.h"
#include "MG_Game2Group.h"
#include "MG_Game2Guild.h"
#include "LC_ServerWarManager.h"
#include "CF_CrossRealmMap.h"
#include "NW_ServerSession.h"
#include "LC_RankManager.h"
#include "LC_CrossRealmRaidManager.h"
#include "LC_GameStory_Manager.h"
#include "LC_ShotActivity.h"
#include "MG_Common.h"
#include "LC_ShotActivity.h"
#include "LC_RumorManager.h"
#include "LC_CommandParserBase.h"
#include "UT_Redis.h"
#include "UT_ActivationCode.h"
#include "LC_ServerShopAsset.h"
#include "LC_Arena_Manager.h"

#include "ACT_ActivitySchedule.h"
#include "ACT_SecretTreasure.h"
#include "LC_ServerAllianceManager.h"

#include "ConfigManager.h"
#include "oss_define.h"
#include "GlobalSettings.h"
#include "gsnode.h"

using namespace Utility;
using namespace NetWork;
using namespace Protocol;
using namespace CSVFile;


void CrossGetTopLevel::invoke(int32_t err, std::vector<std::string>& infos)
{
	if (err)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "fail to get cross top level");
		return;
	}
	GameServerManager* pkServerMgr = GetGameServerApp()->GetServerManager();

	m_kServerLevel.clear();
	for (size_t i = 0; i + 1 < infos.size();)
	{
		int32_t key = boost::lexical_cast<int32_t>(infos[i++]);
		int64_t val = boost::lexical_cast<int64_t>(infos[i++]);
		m_kServerLevel[key] = (int32_t)(val);
		GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "get cross top level", key, val);
	}

	ComputeCrossLevel(pkServerMgr);
}
void CrossGetTopLevel::ComputeCrossLevel(GameServerManager* pkServerMgr)
{
	int32_t iServerID = pkServerMgr->GetCurServerId();
	int32_t iGroupID = pkServerMgr->GetCurGuildGroupId();
	int64_t iTotalVal = 0;
	int32_t iValidCnt = 0;

	mem::map<int32_t, int32_t>::iterator it = m_kServerLevel.begin();
	for (; it!=m_kServerLevel.end(); ++it)
	{
		if (it->second < 60)
		{
			continue;
		}
		Protocol::PS_ServerSimInfo* info = pkServerMgr->GetActive(it->first);
		if (it->first==iServerID || (info&&info->m_iGroupID==iGroupID&&info->m_iServerID==it->first))
		{
			iTotalVal += it->second;
			++iValidCnt;
		}
	}

	int32_t iAvgVal = 60;	//初始为60级
	if (iValidCnt > 0)
	{
		iAvgVal = int32_t(iTotalVal / iValidCnt);
	}
	m_iCrossLevel = iAvgVal;
	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "get cross avg level", m_iCrossLevel);
}

GameServerManager::GameServerManager()
:m_Loaded(false), m_ResetGameInitInfoFlag(ResetGameInitInfo_invalid)
, m_CurServerId(0), m_CurServerType(0), m_CurGroupId(0), m_CurGroupIndex(0)
, m_AllServerInfo(), m_Server2Active(), m_CurServerIDs(), m_AllServerType(), m_bUpdateTaskHonorPointInfo(false)
{}

GameServerManager::~GameServerManager()
{

}

bool GameServerManager::Init(int curserverid, int CurServerType)
{
	m_ResetGameInitInfoFlag = ResetGameInitInfo_invalid;
	m_Loaded = false;
	m_GameInit.Clear();
	m_ServerJsonUpdateTime = 0;
	m_CurServerId = curserverid;
	m_CurServerType = CurServerType;
	m_CurGroupId = 0;
	m_CurGroupIndex = 0;
	m_AllServerInfo.clear();
	m_Server2Active.clear();
	m_CurServerIDs.clear();
	//初始化服务器类型
	m_AllServerType.clear();
	m_bUpdateTaskHonorPointInfo = false;
	{
		m_AllServerType.push_back(NST_GAMESERVER_TO_DBSERVER);
		m_AllServerType.push_back(NST_GAMESERVER_TO_LOGINSERVER);
		m_AllServerType.push_back(NST_GAMESERVER_TO_GROUPSERVER);
		m_AllServerType.push_back(NST_GAMESERVER_TO_MAILSERVER);
		m_AllServerType.push_back(NST_GAMESERVER_TO_GATEWAY);
		m_AllServerType.push_back(NST_GAMESERVER_TO_CHATSERVER);
		m_AllServerType.push_back(NST_GAMESERVER_TO_TRADESERVER);
		m_AllServerType.push_back(NST_GAMESERVER_TO_SOCIETYSERVER);
		m_AllServerType.push_back(NST_GAMESERVER_TO_GLOBALSERVER);
	}
	//发起loginserver
	NW_ServerSessionMap result = GetGameServerApp()->GetServerSession(NST_GAMESERVER_TO_LOGINSERVER);
	if (result.empty())
	{
		return false;
	}
	result.begin()->second->TryToConnect();
	m_fLastUpdateTime = 0.0f;

	m_kCrossTimer.SetTimer(5*60);
	return true;
}

void GameServerManager::Uinit()
{
	Clear();
}

void GameServerManager::Clear()
{
	m_ResetGameInitInfoFlag = ResetGameInitInfo_invalid;
	m_Loaded = false;
	m_GameInit.Clear();
	m_ServerJsonUpdateTime = 0;
	m_CurServerId = GetGameServerID();
	m_CurServerType = 0;
	m_AllServerInfo.clear();
	m_Server2Active.clear();
	m_AllServerType.clear();
	m_CurServerIDs.clear();
	m_fLastUpdateTime = 0.0f;
	m_kCrossTimer.StopTimer();
	m_bUpdateTaskHonorPointInfo = false;
}

Protocol::PS_ServerSimInfo* GameServerManager::GetActive(int serverid)
{
	AllServerInfoIndices::iterator it = m_Server2Active.find(serverid);
	if(it == m_Server2Active.end())
	{
		return NULL;
	}
	AllServerInfoMap::iterator iter = m_AllServerInfo.find(it->second);
	if(iter == m_AllServerInfo.end())
	{
		return NULL;
	}
	return &(iter->second);
}

int GameServerManager::GetActiveId(int serverid)
{
	AllServerInfoIndices::iterator it = m_Server2Active.find(serverid);
	return (it != m_Server2Active.end()) ? it->second : INVALID_SERVER_ID;
}

int GameServerManager::GetActiveGroupId(int serverid)
{
	Protocol::PS_ServerSimInfo* info = GetActive(serverid);
	return info ? info->m_iGroupID : 0;
}

bool GameServerManager::GetServerInfo(int serverid, StringType& name)
{
	Protocol::PS_ServerSimInfo* activeinfo = GetActive(serverid);
	if (activeinfo == NULL)
	{
		return false;
	}
	STDSTR2TPSTR(activeinfo->m_szName, name);
	return true;
}

int32_t	GameServerManager::GetGroupServerIDIndex( int32_t nServerID )
{
	if(nServerID == 0)
	{
		nServerID = m_CurServerId;
	}
	else
	{
		nServerID = GetActiveId(nServerID);
	}
	for(size_t i = 0; i < m_CurServerIDs.size(); i++)
	{
		if(m_CurServerIDs[i] == nServerID)
		{
			return (int32_t)i;
		}
	}
	return -1;
}
bool GameServerManager::SetGameInitInfo(const msg_define::game_init_info& kInit)
{
	m_GameInit = kInit;
	int EntryServerID = GetGameServerEntryID();
	const msg_define::server_time_info* p_svr_time = GetServerTimeInfo(EntryServerID);
	if (NULL == p_svr_time)
	{
		GameServerLoadFail();
		return false;
	}

	uint32_t entryNodeStartTime = p_svr_time->start_time();
	if(0 == entryNodeStartTime)
	{
		GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "server init, entryNodeStartTime=0", m_GameInit.group_id(), m_GameInit.server_id(), p_svr_time->start_time(), p_svr_time->conflate_time());
		m_ResetGameInitInfoFlag = ResetGameInitInfo_true;//需要重新设置
		return false;
	}

	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "server init!", m_GameInit.group_id(), m_GameInit.server_id(), p_svr_time->start_time(), p_svr_time->conflate_time());
	for (int i=0; i<m_GameInit.time_infos_size(); ++i)
	{
		msg_define::server_time_info* prk_time_info = m_GameInit.mutable_time_infos(i);
		if(NULL == prk_time_info)
		{
			continue;
		}

		//node的开服和合服时间，用entryServer的开服和合服时间
		int serverid = prk_time_info->server_id();
		if(serverid != EntryServerID)
		{
			int32_t serverEntryid = GSNodeNetwork::Instance().getEntryIdByMergeId(serverid);
			if(serverEntryid == EntryServerID)
			{
				prk_time_info->set_conflate_time(p_svr_time->conflate_time());
				prk_time_info->set_start_time(p_svr_time->start_time());
			}
		}

		GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "server time!", prk_time_info->server_id(), prk_time_info->start_time(), prk_time_info->conflate_time());
	}
	if (m_Loaded)
	{
		Activity::ACT_ScheduleManager::GetSingletonPtr()->Schedule();
	}
	oss_server_info(m_GameInit.server_id(), p_svr_time->start_time(), p_svr_time->conflate_time());

	m_ResetGameInitInfoFlag = ResetGameInitInfo_false;//不需要重新设置
	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "setGameInitInfo success serverId:%d, resetGameInitInfoFlag:%d\n", GetGameServerID(), m_ResetGameInitInfoFlag);
	return true;
}

bool GameServerManager::ResetGameInitInfo(const msg_define::game_init_info& kInit)
{
	if(m_ResetGameInitInfoFlag != ResetGameInitInfo_true)
	{
		GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "ResetGameInitInfo false serverId:%d, resetGameInitInfoFlag:%d\n", GetGameServerID(), m_ResetGameInitInfoFlag);
		return false;
	}

	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "ResetGameInitInfo start serverId:%d, resetGameInitInfoFlag:%d\n", GetGameServerID(), m_ResetGameInitInfoFlag);
	m_GameInit = kInit;
	int EntryServerID = GetGameServerEntryID();
	const msg_define::server_time_info* p_svr_time = GetServerTimeInfo(EntryServerID);
	if (NULL == p_svr_time)
	{
		GameServerLoadFail();
		return false;
	}

	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "reset server init!", m_GameInit.group_id(), m_GameInit.server_id(), p_svr_time->start_time(), p_svr_time->conflate_time());
	for (int i=0; i<m_GameInit.time_infos_size(); ++i)
	{
		const msg_define::server_time_info& rk_time_info = m_GameInit.time_infos(i);
		GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "reset server time!", rk_time_info.server_id(), rk_time_info.start_time(), rk_time_info.conflate_time());
		if (!LC_Helper::CheckIsSameDay(p_svr_time->start_time(), rk_time_info.start_time()))
		{
			GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "reset server start time conflicate!", rk_time_info.server_id(), rk_time_info.start_time());
		}

		if (!LC_Helper::CheckIsSameDay(p_svr_time->conflate_time(), rk_time_info.conflate_time()))
		{
			GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "reset server conflate time conflicate!", rk_time_info.server_id(), rk_time_info.start_time());
		}
	}
	if (m_Loaded)
	{
		Activity::ACT_ScheduleManager::GetSingletonPtr()->Schedule();
	}
	oss_server_info(m_GameInit.server_id(), p_svr_time->start_time(), p_svr_time->conflate_time());

	m_ResetGameInitInfoFlag = ResetGameInitInfo_false;//不需要重新设置
	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "resetGameInitInfo success serverId:%d, resetGameInitInfoFlag:%d\n", GetGameServerID(), m_ResetGameInitInfoFlag);
	return true;
}

bool GameServerManager::IsServerOpened(void)
{
	const msg_define::server_time_info* p_svr_time = GetServerTimeInfo(m_CurServerId);
	if (NULL == p_svr_time)
	{
		return false;
	}
	uint32_t timestamp = GET_CURRENT_TIMESTAMP_IN_SECONDS();	
	return timestamp >= p_svr_time->start_time();
}
const msg_define::server_time_info* GameServerManager::GetServerTimeInfo(int32_t iServerID)
{
	for (int i=0; i<m_GameInit.time_infos_size(); ++i)
	{
		const msg_define::server_time_info& rk_time_info = m_GameInit.time_infos(i);
		if (rk_time_info.server_id() == iServerID)
		{
			return &rk_time_info;
		}
	}
	return NULL;
}

uint32_t GameServerManager::GetServerStartTimeAbsolute()
{
	uint32_t nResult = GetServerStartTime();//服务器开服时间 年月日时分秒
	uint32_t nTime2 = nResult % ONE_DAY_SECONDS;//服务器开服时间 时分秒
	// 计算出到当天8点的时间
	nResult -= nTime2;
	// 计算出到当天0点的时间
	nResult -= 28800;
	return nResult;
}

uint32_t GameServerManager::GetActivityStartTime(int32_t nStarCoolTime)
{
	uint32_t serverTime = GetServerStartTimeAbsolute();//开服当天，0点的时间戳
	return serverTime + nStarCoolTime*ONE_DAY_SECONDS;
}

bool GameServerManager::GetServerStartTimeAbsoluteIntervalLoopInfo(const std::vector<uint32_t>& nLoopTimer, int32_t nStarCoolTime, uint32_t& startTime, uint32_t& nRemainTime, uint32_t& index )
{
	if (nLoopTimer.empty() || nStarCoolTime < 0)
		return false;

	startTime = GetActivityStartTime(nStarCoolTime);
	
	uint32_t totalTime = 0;
	for(uint32_t i=0; i<nLoopTimer.size(); ++i)
	{
		totalTime += nLoopTimer[i];
	}

	uint32_t currentTime = time(NULL);
	uint32_t nInterval = currentTime>startTime ? currentTime-startTime : 0;
	
	//已经过去的时间
	uint32_t usedTime = nInterval % totalTime;

	//剩余的时间
	//nRemainTime = totalTime - usedTime;

	//目前处于第几个活动
	for(uint32_t j=0; j<nLoopTimer.size(); ++j)
	{
		index = j;
		if(usedTime > nLoopTimer[j])
		{
			usedTime -= nLoopTimer[j];
		}
		else
		{
			//剩余的时间
			nRemainTime = nLoopTimer[j] - usedTime;
			break;
		}
	}

	return index<nLoopTimer.size();
}

bool GameServerManager::LoadServerListJson(uint32_t nUpdateTime)
{
	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "GameServerManager load server list json update time: ", nUpdateTime, m_Loaded);
	if(m_ServerJsonUpdateTime != nUpdateTime || nUpdateTime == 0)
	{
		m_ServerJsonUpdateTime = nUpdateTime == 0 ? GET_CURRENT_TIMESTAMP_IN_SECONDS() : nUpdateTime;
		//加载数据
		Utility::Redis_Handler* pkRedisHandler = GetGameServerApp()->GetRedisHandler();
		if (NULL == pkRedisHandler)
		{
			return false;
		}

		int ret = 0;
		std::string strServerList;
		UT_REDIS_CMD_WITH_REPLY(ret, pkRedisHandler, strServerList, get, GetGameServerApp()->GetServerListPrefix() + GLOBAL_GROUP_SERVER_SKEY);
		if(ret != 0 || strServerList.length() <= 0)
		{
			GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "LoadServerListJson Failed from redis", ret);
			return false;
		}

		GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "GameServerManager load server list json", strServerList);

		Js_ActiveServerMap gameServerList;
		std::string err_msg;
		if (boost::ajson::load_from_buff(gameServerList, strServerList.c_str(), err_msg) == false)
		{
			GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "LoadServerListJson Failed parse string", err_msg);
			return false;
		}

		if(gameServerList.size() <= 0)
		{
			return false;
		}

		m_AllServerInfo.clear();
		m_Server2Active.clear();
		m_CurServerIDs.clear();

		if(gameServerList.find(m_CurServerId) == gameServerList.end())
		{
			GameServerLoadFail();
			return false;
		}

		for (Js_ActiveServerMap::iterator it = gameServerList.begin(); it != gameServerList.end(); ++it)
		{
			_updateActive(it->second);
		}

		Protocol::PS_ServerSimInfo* curserverinfo = GetCurServerInfo();
		if (NULL == curserverinfo)
		{
			m_AllServerInfo.clear();
			m_Server2Active.clear();
			m_CurServerIDs.clear();
			GameServerLoadFail();
			return false;
		}

		if(!m_Loaded)
		{
			m_Loaded = true;
			if(ResetGameInitInfo_false == m_ResetGameInitInfoFlag)
			{
				GameServerLoadSuccess();
			}
		}
		else
		{
			SendServerInfo2AllServer();
		}
	}
	return true;
}

void GameServerManager::GameServerLoadSuccess()
{
	LC_ServerLogicManager* pkServerLogicManager = SERVER_GET_LOGIC_MANAGER();
	LC_WorldGuildWarAsset* pWorldGuildWarAsset = pkServerLogicManager->GetGuildWarAssert();
	if(pWorldGuildWarAsset == NULL)
	{
		return ;
	}

	pWorldGuildWarAsset->SetCurServerId((uint32_t)m_CurServerId);
	pWorldGuildWarAsset->SetGroupId((uint32_t)GetCurGuildGroupId());
	ConnectAllServer();

	m_kCrossLevel.ComputeCrossLevel(this);

	((ServerCSVSchedule*)ServerCSVSchedule::GetSingletonPtr())->OnGameServerLoaded((uint32_t)GetServerStartTime());
	SERVER_GET_GAMESTORY_MANAGER->OnGameServerLoaded();
	GameLogic::LC_RumorManager::GetSingletonPtr()->OnGameServerLoaded();

	LC_ServerShopManager::GetSingletonPtr()->loadGlobalLimitFromRedis();

	//activity
	Activity::ACT_ScheduleManager::GetSingletonPtr()->Schedule();
	Activity::ACT_SecretTreasureManager::GetSingletonPtr()->OnGameServerLoaded();
	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "server load successfully!");

	if(true==GetGameServerApp()->IsNodeType_Global())
	{
		UT_ServerHelper::UpdateWatchTowerFromSaveState();
	}
}

void GameServerManager::GameServerLoadFail()
{
	GfxWriteFmtLog(LOG_SYSTEM_FILELINE, LOG_SWITCH_SYSTEM, "server load failed! exit!");
	CloseAllServer();
	exit(3);
}

void GameServerManager::OnGroupServerReady()
{
	ReportServerRealm(NST_GAMESERVER_TO_GROUPSERVER);
	InitRankRelated();
	_doInit();
	// Try execute app's tool mode
	GetGameServerApp()->TryExecuteBotOP();
}

void GameServerManager::OnGroupServerLost()
{
}

void GameServerManager::ConnectAllServer()
{
	mem::vector<NetSessionType>::iterator beg = m_AllServerType.begin();
	for (; beg != m_AllServerType.end(); ++beg)
	{
		if (*beg != NST_GAMESERVER_TO_LOGINSERVER)
		{
			NW_ServerSessionMap result = GetGameServerApp()->GetServerSession(*beg);
			NW_ServerSessionMap::iterator sessionbeg = result.begin();
			for (; sessionbeg != result.end(); ++sessionbeg)
			{
				if (!sessionbeg->second->IsConnected())
				{
					sessionbeg->second->TryToConnect();
				}
				else
				{
					ReportServerRealm(*beg);
				}
			}
		}
	}
}

void GameServerManager::CloseAllServer()
{
	mem::vector<NetSessionType>::iterator beg = m_AllServerType.begin();
	for (; beg != m_AllServerType.end(); ++beg)
	{
		NW_ServerSessionMap result = GetGameServerApp()->GetServerSession(*beg);
		NW_ServerSessionMap::iterator sessionbeg = result.begin();
		for (; sessionbeg != result.end(); ++sessionbeg)
		{
			sessionbeg->second->Disconnect();
		}
	}
}

int64_t GameServerManager::GetServerStartTime(void)
{ 
	int EntryServerID = GetGameServerEntryID();
	const msg_define::server_time_info* p_svr_time = GetServerTimeInfo(EntryServerID);
	return p_svr_time ? p_svr_time->start_time() : 0;
}

int64_t GameServerManager::GetServerConflateTime(void)
{
	int EntryServerID = GetGameServerEntryID();
	const msg_define::server_time_info* p_svr_time = GetServerTimeInfo(EntryServerID);
	return p_svr_time ? p_svr_time->conflate_time() : 0;
}

int32_t GameServerManager::GetEntryServerStartTime()
{
	int entryID = GetGameServerEntryID();

	const msg_define::server_time_info* p_svr_time = GetServerTimeInfo(entryID);
	return p_svr_time ? p_svr_time->start_time() : 0;
}

int64_t GameServerManager::GetEntryServerConflateTime()
{
	int entryID = GetGameServerEntryID();

	const msg_define::server_time_info* p_svr_time = GetServerTimeInfo(entryID);
	return p_svr_time ? p_svr_time->conflate_time() : 0;
}
void GameServerManager::SendMsg2Server(NetSessionType session_type, uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody, uint32_t uiUserID, const unique_id_impl& uiCharID, int32_t iSequence, int32_t iReserved)
{
	switch (session_type)
	{
		case NST_GAMESERVER_TO_GROUPSERVER:
			{
				SendMsgToGroup(usMsgType, pkMsgBody, uiUserID, uiCharID, iSequence);
			}
			break;
		case NST_GAMESERVER_TO_SOCIETYSERVER:
			{
				SendMsgToSociety(usMsgType, pkMsgBody, uiUserID, uiCharID, iSequence);
			}
			break;
		case NST_GAMESERVER_TO_DBSERVER:
			{
				SendMsgToGameDB(usMsgType, pkMsgBody, uiUserID, uiCharID, iSequence);
			}
			break;
		case NST_GAMESERVER_TO_TRADESERVER:
			{
				SendMsgToTrade(usMsgType, pkMsgBody, uiUserID, uiCharID, iSequence);
			}
			break;
		case NST_GAMESERVER_TO_CHATSERVER:
			{
				SendMsgToChat(usMsgType, pkMsgBody, uiUserID, uiCharID, iSequence, iReserved);
			}
			break;
		case NST_GAMESERVER_TO_GLOBALSERVER:
			{
				SendMsgToGlobal(usMsgType, pkMsgBody, uiUserID, uiCharID, iSequence);
			}	
			break;
		default:
			break;
	}
}

NetSessionType gSessionList[] = { NST_GAMESERVER_TO_GROUPSERVER, NST_GAMESERVER_TO_SOCIETYSERVER, NST_GAMESERVER_TO_TRADESERVER, NST_GAMESERVER_TO_CHATSERVER, NST_GAMESERVER_TO_GLOBALSERVER};
void GameServerManager::SendServerInfo2AllServer()
{
	ReportServerRealm(gSessionList, sizeof(gSessionList)/sizeof(NetSessionType));
}

void GameServerManager::Update(float fCurrTime)
{
	if(fCurrTime > m_fLastUpdateTime && (fCurrTime - m_fLastUpdateTime >= 5.0f || m_fLastUpdateTime == 0.0f))	//5秒更新一次
	{
		ReportServerInfo(gSessionList, sizeof(gSessionList)/sizeof(NetSessionType));
		m_fLastUpdateTime = fCurrTime;
	}

	if (m_kCrossTimer.IsExpired())
	{
		SyncCrossServer();
	}
}

bool GameServerManager::_getServerRealm(msg_define::server_realm& rk_realm)
{
	Protocol::PS_ServerSimInfo* activeinfo = GetCurServerInfo();
	if (NULL == activeinfo || m_Loaded == false)
	{
		return false;
	}
	rk_realm.Clear();
	rk_realm.set_union_(GetGameUnionID());
	rk_realm.set_region(GetGameRegionID());
	rk_realm.set_group(activeinfo->m_iGroupID);
	
	const msg_define::server_time_info* p_svr_time = GetServerTimeInfo(m_CurServerId);
	if (p_svr_time)
	{
		rk_realm.mutable_time_info()->CopyFrom(*p_svr_time);
	}
	
	std::vector<uint16_t>::iterator beg = activeinfo->m_kSideline.begin();
	for (; beg != activeinfo->m_kSideline.end(); ++beg)
	{
		rk_realm.add_contains(*beg);
	}
	return true;
}

void GameServerManager::ReportServerRealm(NetSessionType nSession)
{
	Protocol::MG_PB<msg_define::server_realm> realmreq;
	if(_getServerRealm(realmreq.m_value) == false)
	{
		return;
	}
	SendMsg2Server(nSession, MGPT_GL_REPORT_SERVERREALM, &realmreq);
}

void GameServerManager::ReportServerRealm(NetSessionType* pArr, int nSize)
{
	if (pArr == NULL || nSize <= 0)
	{
		return;
	}
	Protocol::MG_PB<msg_define::server_realm> realmreq;
	if(_getServerRealm(realmreq.m_value) == false)
	{
		return;
	}
	for(int i=0;i<nSize;i++)
	{
		SendMsg2Server(pArr[i], MGPT_GL_REPORT_SERVERREALM, &realmreq);
	}
}

void GameServerManager::ReportServerInfo(NetSessionType* pArr, int nSize)
{
	if (m_Loaded == false || pArr == NULL || nSize <= 0)
	{
		return;
	}
	static LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	if(NULL == pkPlayerManager)
	{
		return;
	}
	Protocol::MG_Report_GameServerInfo info;
	info.m_PlayerCount = pkPlayerManager->GetPlayerCount();
	for(int i=0;i<nSize;i++)
	{
		SendMsg2Server(pArr[i], MGPT_GL_REPORT_SERVERUPDATE, &info);
	}
}

void GameServerManager::RequestServerAllianceInfo()
{
	Protocol::MG_Req_Alliance_Frontier_Info reqMsg;
	reqMsg.m_nAllianceId = m_CurGroupId;
	SendMsg2Server(NST_GAMESERVER_TO_SOCIETYSERVER, MGPT_SS_FALLIANCE_INFO_REQ, &reqMsg);
}

void GameServerManager::RegisterCrossRealmGuildWar()
{
	if (m_Loaded == false)
	{
		return;
	}
	if (GetCurServerType() != SERVER_TYPE_GAME)
	{
		return;
	}
	LC_WarManager* manager = (LC_WarManager*)SERVER_GET_GUILDWAR_MANAGER();
	if (manager != NULL)
	{
		manager->ReportCrossRealmGuildWarInfo();
	}
}

void GameServerManager::SocietyServerConnect()
{
	ReportServerRealm(NST_GAMESERVER_TO_SOCIETYSERVER);
	RegisterCrossRealmGuildWar();
	RegisterCrossRealmTeamMap();
	RegisterServerOnLineUser(NST_GAMESERVER_TO_SOCIETYSERVER);
	RequestServerAllianceInfo();
}

void GameServerManager::SocietyServerDisConnect()
{
	LC_WarManager* manager = (LC_WarManager*)SERVER_GET_GUILDWAR_MANAGER();
	manager->SocietyServerShutDown();
	LC_CrossRealmRaidManager* crossmanager = (LC_CrossRealmRaidManager*)LC_CrossRealmRaidManager::GetSingletonPtr();
	crossmanager->SocietyServerShutDown();
}

void GameServerManager::DBServerConnect()
{
	ReportServerRealm(NST_GAMESERVER_TO_DBSERVER);
}

void GameServerManager::DBServerDisConnect()
{

}

void GameServerManager::TradeServerConnect()
{
	ReportServerRealm(NST_GAMESERVER_TO_TRADESERVER);
}

void GameServerManager::TradeServerDisConnect()
{

}

void GameServerManager::ChatServerConnect()
{
	ReportServerRealm(NST_GAMESERVER_TO_CHATSERVER);
	RegisterServerOnLineUser(NST_GAMESERVER_TO_CHATSERVER);
}

void GameServerManager::RegisterServerOnLineUser(NetSessionType nType)
{
	LC_PlayerManagerBase* pkPlayerManager = ENGINE_GET_GAMEPLAYER_MANAGER();
	const LC_PlayerIDPtrMap& kPlayerIDPtrMap = pkPlayerManager->GetPlayerIDMap();
	for (LC_PlayerIDPtrMap::const_iterator Iter = kPlayerIDPtrMap.begin();Iter != kPlayerIDPtrMap.end();++Iter)
	{
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer *)Iter->second;
		if (pkPlayer != NULL && pkPlayer->IsOffline() == false && pkPlayer->IsMainController())
		{
			SendMsg2Server(nType, MGPT_SS_ROLE_ENTER_GAME, NULL, pkPlayer->GetUserID(), pkPlayer->GetInstance(), pkPlayer->GetCitizenship(), GetGateWayServerID(pkPlayer));
		}
	}
}

void GameServerManager::ChatServerDisConnect()
{

}

void GameServerManager::GlobalServerConnect()
{
	ReportServerRealm(NST_GAMESERVER_TO_GLOBALSERVER);
	SendServerInfo(SERVER_TYPE_GLOBAL);
}

void GameServerManager::GlobalServerDisConnect()
{

}
void GameServerManager::GameServerConnect(int32_t iID)
{

}
void GameServerManager::GameServerDisConnect(int32_t iID)
{
	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, __FUNCTION__, iID);
	static LC_Arena_Manager* pkArenaMgr = SERVER_GET_ARENA_MANAGER();
	pkArenaMgr->ResetArenaOnServer(iID);
}
void GameServerManager::RegisterCrossRealmTeamMap()
{
	Protocol::MG_GG_Register_CrossRealmTeamMap msg;
	CF_CrossRealmMap* thecrossmap = CF_CrossRealmMap::GetSingletonPtr();
	CF_CrossRealmMap::DataEntryMapExternIterator beg = thecrossmap->GetIterator();
	CF_CrossRealmMap::DataEntry* pkEntry = NULL;
	int i = 0;
	while (beg.HasMoreElements())
	{
		pkEntry = beg.PeekNextValuePtr();
		msg.m_MapTypeList[i] = pkEntry->_iID;
		msg.m_MapTypeRecordCount[i] = pkEntry->_FastPassCount;
		++i;
		beg.MoveNext();
	}
	msg.m_Size = i;
	SendMsgToSociety(MGPT_SS_CROSSREALM_TEAM_MAP_REGISTER, &msg);
}

void GameServerManager::SetServerIdFromArgv( int32_t nInput )
{
	m_nServerIdFromArgv = nInput;
}

int32_t GameServerManager::GetServerIdFromArgv()
{
	return m_nServerIdFromArgv;
}

int GameServerManager::GetCorssLevel(void)
{
	int32_t redis_ret = 0;
	std::string key = GetGameServerApp()->GetRedisGroupPrefix() + UT_REDIS_KEY_SERVER_RANK_TOPS + ":" + boost::lexical_cast<std::string>(Rank_Type_Level);
	
	std::vector<std::string> levels;
	Utility::Redis_Handler* pkRedisHandler = GetGameServerApp()->GetRedisHandler();
	redispp::MultiBulkEnumerator kLevels;
	UT_REDIS_CMD_BEGIN(redis_ret, pkRedisHandler);
	kLevels = pkRedisConn->hgetAll(key);

	std::string _elm;
	while (kLevels.next(&_elm))
	{
		levels.push_back(_elm);
	}

	UT_REDIS_CMD_END(redis_ret, pkRedisHandler);

	m_kCrossLevel.invoke(redis_ret, levels);
	return redis_ret;
}
int32_t	GameServerManager::GetGroupServerTopLevel(void)
{
	LC_ServerAllianceManager* pAllianceManager = LC_ServerAllianceManager::GetSingletonPtr();
	if(NULL == pAllianceManager)
	{
		return 0;
	}
	mem::vector<int32_t> servers;
	pAllianceManager->GetSelfGroupServers(servers);
	int32_t iTopLevel = 0;
	for (mem::vector<int32_t>::iterator iter = servers.begin(); iter != servers.end(); ++iter)
	{
		int32_t iLevel = GetCrossServerLevel(*iter);
		if (iLevel > iTopLevel)
		{
			iTopLevel = iLevel;
		}
	}
	return iTopLevel;
}
void GameServerManager::SyncCrossServer(void)
{
	boost::function<void(int32_t, std::vector<std::string>&)> cb = boost::bind(&CrossGetTopLevel::invoke, &m_kCrossLevel, _1, _2);
	std::string key = GetGameServerApp()->GetRedisGroupPrefix() + UT_REDIS_KEY_SERVER_RANK_TOPS + ":" + boost::lexical_cast<std::string>(Rank_Type_Level);
	int32_t redis_ret = 0;
	ASYNC_REDIS_REQUEST(redis_ret, GetGameServerApp()->GetRedisChannel(), &redispp::Connection::hgetAll, cb, key);
}

void GameServerManager::InitRankRelated()
{
	LC_ShotActivityManager* pkShotMgr = LC_ShotActivityManager::GetSingletonPtr();
	pkShotMgr->OnGameServerLoaded();
}

void GameServerManager::_CopyData(Protocol::PS_ServerSimInfo& rNode, const Js_ActiveServerInfo& rData)
{
	rNode.m_iGroupID = rData.groupid;
	rNode.m_iServerID = rData.serverid;
	rNode.m_szName = rData.name;

	rNode.m_kSideline.clear();
	for(size_t i=0;i<rData.contain.size();i++)
	{
		const Js_ServerSimInfo& rSim = rData.contain[i];
		rNode.m_kSideline.push_back(rSim.serverid);
	}
}

Protocol::PS_ServerSimInfo* GameServerManager::_updateActive(const Js_ActiveServerInfo& kActive)
{
	Protocol::PS_ServerSimInfo& rActiveNode = m_AllServerInfo[kActive.serverid];
	_CopyData(rActiveNode, kActive);
	_addIndex(kActive.serverid, &rActiveNode);
	for (Js_ServerSimInfoVector::const_iterator itCon = kActive.contain.begin(); itCon != kActive.contain.end(); ++itCon)
	{
		_addIndex(itCon->serverid, &rActiveNode);
	}
	if(kActive.serverid == m_CurServerId)
	{
		m_CurGroupId = kActive.groupid;
	}
	if(m_CurGroupId == kActive.groupid)
	{
		m_CurServerIDs.clear();
		for(AllServerInfoMap::iterator iter = m_AllServerInfo.begin(); iter != m_AllServerInfo.end(); iter++)
		{
			if(iter->second.m_iGroupID == m_CurGroupId)
			{
				m_CurServerIDs.push_back(iter->first);
			}
		}
		std::sort(m_CurServerIDs.begin(), m_CurServerIDs.end(), std::less<int32_t>());
		m_CurGroupIndex = GetGroupServerIDIndex();
	}
	return &rActiveNode;
}

void GameServerManager::_doInit(void)
{
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "SERVER CHECK INIT AND DO BEGIN!");
	const init_setting& _init_set = GetGameServerApp()->InitSetting();

	Utility::Redis_Handler* pkRedisHandler = GetGameServerApp()->GetRedisHandler();
	if (NULL == pkRedisHandler)
	{
		return;
	}

	int ret = 0;
	const std::string& sServerPrefix = GetGameServerApp()->GetRedisServerPredix();

	std::string curVersion;
	// weixin TODO: wtf
	//return;
	UT_REDIS_CMD_WITH_REPLY(ret, pkRedisHandler, curVersion, get, sServerPrefix + UT_REDIS_KEY_INIT_VERSION);

	uint32_t iCurVesion = 0;
	if (0 == ret && !curVersion.empty())
	{
		iCurVesion = boost::lexical_cast<uint32_t>(curVersion);
	}

	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "SERVER CHECK INIT AND DO VER:[%d] CVER[%d]!", iCurVesion, _init_set.version);
	if (iCurVesion <= _init_set.version)
	{
		do
		{
			if (!_init_set.activation_codes.empty())
			{
				Utility::Redis_Handler* pkGlobalRedisHandler = GetGameServerApp()->GetGlobalRedisHandler();
				GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "SERVER CHECK INIT AND DO, ACTIVATION CODE!");
				mem::vector<std::string>::const_iterator cit = _init_set.activation_codes.begin();
				for (; cit != _init_set.activation_codes.end(); ++cit)
				{
					std::ifstream file(cit->c_str(), std::ios::binary);
					if (!file.is_open())
					{
						GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "opening activation code file error! %s", cit->c_str());
						continue;
					}
					std::stringstream ss;
					ss << file.rdbuf();
					file.close();
					redis_define::activation_code_define kcodedefine;
					if (!kcodedefine.ParseFromString(ss.str()))
					{
						GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "Unable to parse activation code file! %s\n", cit->c_str());
						continue;
					}
					const redis_define::encrypt_key_entry& kkey = kcodedefine.encrypt_key();
					if (kcodedefine.id() <= 0 || !Utility::CheckCodeKeyValid(kkey, false))
					{
						GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "The full activation code file is not defined!%s\n", cit->c_str());
						continue;
					}
					Utility::EmbedActivationCode(pkGlobalRedisHandler, kcodedefine);
				}
			}

			if (!_init_set.gm_accounts.empty())
			{
				GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "SERVER CHECK INIT AND DO, GM ACCOUNT!");
				redispp::StringReply kGMReply;
				UT_REDIS_CMD_WITH_REPLY(ret, pkRedisHandler, kGMReply, get, GetGameServerApp()->GetRedisServerPredix() + UT_REDIS_KEY_GM_ACCOUNTS);

				redis_define::gm_account_setting k_gm_setting;
				if (0 == ret && kGMReply.result())
				{
					k_gm_setting.ParseFromString(kGMReply);
				}

				mem::vector<gm_account>::const_iterator cit = _init_set.gm_accounts.begin();
				for (; cit != _init_set.gm_accounts.end(); ++cit)
				{
					const StringType& usr = cit->user;
					const StringType& pwd = cit->pwd;
					redis_define::gm_account_info* pkaccount = NULL;
					for (int i = 0; i < k_gm_setting.accounts_size(); ++i)
					{
						if (k_gm_setting.accounts(i).account().compare(usr.c_str()) == 0)
						{
							pkaccount = k_gm_setting.mutable_accounts(i);
						}
					}

					if (NULL == pkaccount)
					{
						pkaccount = k_gm_setting.add_accounts();
						pkaccount->set_account(usr.c_str());
					}

					if (pkaccount)
					{
						pkaccount->set_password(pwd.c_str());
						pkaccount->clear_servers();
						redis_define::gm_server_info* pkserver = pkaccount->add_servers();

						pkserver->set_server(0);
						pkserver->set_priority(cit->priority);
					}
				}
				UT_REDIS_CMD(ret, pkRedisHandler, set, GetGameServerApp()->GetRedisServerPredix() + UT_REDIS_KEY_GM_ACCOUNTS, k_gm_setting.SerializeAsString());
			}

			if (!_init_set.cmds.empty())
			{
				GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "SERVER CHECK INIT AND DO, GM COMMONDS!");
				mem::vector<StringType>::const_iterator cit = _init_set.cmds.begin();
				for (; cit != _init_set.cmds.end(); ++cit)
				{
					Utility::UT_CommandLine  cmdLine;
					cmdLine.SplitLine(*cit);
					Utility::DebugCommandResult rslt = Utility::LC_CommandParserBase::GetSingletonPtr()->ParseCommand(cmdLine, 0);
				}
			}
			UT_REDIS_CMD(ret, pkRedisHandler, set, sServerPrefix + UT_REDIS_KEY_INIT_VERSION, boost::lexical_cast<std::string>(_init_set.version));
			break;
		} while (1);
	}
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "SERVER CHECK INIT AND DO END!");
}
