#include "LC_ServerMapLogic_WatchTowerPlace.h"
#include "GameServerManager.h"
#include "GameServerApp.h"
#include "LC_ActorBase.h"
#include "LC_ServerNPC.h"
#include "LC_ServerGuildManager.h"
#include "CF_BestGuildBossRankRewards.h"
#include "MG_Game2Alliance.h"
#include "LC_PlayerManagerBase.h"
#include "LC_PlayerBase.h"
#include "LC_LogicManagerBase.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "UT_ServerHelper.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerLogicManager.h"
#include "MG_CS_Guild.h"
#include "LC_ServerNPCManager.h"
LC_ServerMapLogic_WatchTowerPlace::LC_ServerMapLogic_WatchTowerPlace() : LC_ServerMapLogic_Place(), m_bSettle(false), m_nDamageNpcLogicID(0), m_nLoadFlag(false), m_nFirstLoad(false)
{
	m_nBestGuildInfo.clear();
}

LC_ServerMapLogic_WatchTowerPlace::~LC_ServerMapLogic_WatchTowerPlace()
{

}

void LC_ServerMapLogic_WatchTowerPlace::Init()
{
	LC_ServerMapLogic_Place::Init();
	Clear();
}
void LC_ServerMapLogic_WatchTowerPlace::UnInit()
{
	LC_ServerMapLogic_Place::UnInit();
	Clear();
}
void LC_ServerMapLogic_WatchTowerPlace::Clear()
{
	m_nLoadFlag = false;
	m_bSettle = false;
	m_nDamageNpcLogicID = 0;
	m_nBestGuildInfo.clear();
	m_nFirstLoad = false;
	SetMapLogicType(MT_WATCHTOWER);
}
void LC_ServerMapLogic_WatchTowerPlace::Update(float fCurrentTime, float fDeltaTime)
{
	GameServerManager* pkAppMgr = GetGameServerApp()->GetServerManager();
	if (!pkAppMgr->isLoaded())
	{
		return;
	}

	LC_ServerMapLogic_Place::Update(fCurrentTime, fDeltaTime);
	for (WatchTowerGuildInfoMap::iterator itc = m_nBestGuildInfo.begin(); itc != m_nBestGuildInfo.end(); ++itc)
		itc->second.CheckSendOwerReward();

	//非global节点，第一次要向redis请求哨塔数据
	if(GetMapResID()==GetGlobalSetting.BestGuildMapID && false==GetGameServerApp()->IsNodeType_Global() && false==m_nFirstLoad)
	{
		UpdateWatchTowerFromRedis();
	}
}

void LC_ServerMapLogic_WatchTowerPlace::SetWatchTowerState(int storyID, int bossMapID, int towerID, int bossID, int state, int vaildTime, int endTimerID)
{
	if(false==GetGameServerApp()->IsNodeType_Global())
	{
		GfxWriteLog( LOG_INFO_FILELINE, LOG_SWITCH_INFO, "LC_ServerMapLogic_WatchTowerPlace::SetWatchTowerState serverid:%d, storyID:%d, bossMapID:%d, towerID:%d, bossID:%d, state:%d r1", 
			GetGameServerID(), storyID, bossMapID, towerID, bossID, state);
		return;
	}

	if(GetLoadFlag() && state==WatchTower_Protocted)
	{
		GfxWriteLog( LOG_INFO_FILELINE, LOG_SWITCH_INFO, "LC_ServerMapLogic_WatchTowerPlace::SetWatchTowerState serverid:%d, storyID:%d, bossMapID:%d, towerID:%d, bossID:%d, state:%d r2", 
			GetGameServerID(), storyID, bossMapID, towerID, bossID, state);
		return;
	}

	if(state<WatchTower_Protocted || state>=WatchTower_End)
	{
		GfxWriteLog( LOG_INFO_FILELINE, LOG_SWITCH_INFO, "LC_ServerMapLogic_WatchTowerPlace::SetWatchTowerState serverid:%d, storyID:%d, bossMapID:%d, towerID:%d, bossID:%d, state:%d r3", 
			GetGameServerID(), storyID, bossMapID, towerID, bossID, state);
		return;
	}

	LC_ServerNPCManager* pkNPCManager = SERVER_GET_NPC_MANAGER();

	uint64_t id = LC_Helper::EncodeScore(storyID, towerID);
	WatchTowerGuildInfoMap::iterator findIt = m_nBestGuildInfo.find(id);
	if(findIt == m_nBestGuildInfo.end())
	{
		WatchTowerGuildInfo info(storyID, towerID);
		m_nBestGuildInfo.insert(std::make_pair(id, info));
	}

	findIt = m_nBestGuildInfo.find(id);
	if(findIt == m_nBestGuildInfo.end())
	{
		GfxWriteLog( LOG_INFO_FILELINE, LOG_SWITCH_INFO, "LC_ServerMapLogic_WatchTowerPlace::SetWatchTowerState serverid:%d, storyID:%d, bossMapID:%d, towerID:%d, bossID:%d, state:%d r4", 
			GetGameServerID(), storyID, bossMapID, towerID, bossID, state);
		return;
	}

	int lastState = findIt->second.GetState();
	//lua设置顺序不确定，可能先设置WatchTower_AllKilled，再设置WatchTower_Protocted
	if(lastState==WatchTower_AllKilled && state==WatchTower_Protocted)
	{
		GfxWriteLog( LOG_INFO_FILELINE, LOG_SWITCH_INFO, "LC_ServerMapLogic_WatchTowerPlace::SetWatchTowerState serverid:%d, storyID:%d, bossMapID:%d, towerID:%d, bossID:%d, state:%d lastState:%d r5", 
			GetGameServerID(), storyID, bossMapID, towerID, bossID, state, lastState);
		return;
	}

	if(vaildTime <= 0)
	{
		CF_CharType::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_CharType>(bossID);
		if(NULL != pkData)
		{
			findIt->second.SetMonsterHP(pkData->_lMaxHP);
		}		
	}

	//哨塔处于保护状态
	if(0 == vaildTime)
	{
		int32_t currentTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();
		int32_t startTime = UT_ServerHelper::GetStoryStartTime(storyID);
		if(startTime > currentTime)
		{
			vaildTime = startTime-currentTime;
		}
	}

	if(state==WatchTower_Protocted || state==WatchTower_AllKilled)
	{
		findIt->second.SetState(state,bossMapID,bossID, vaildTime);
	}
	else if(state == WatchTower_Temp)
	{
		LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
		LC_ServerMapLogicPlaceVector* logicMaps = pkWorldManager->GetMapLogicByRes(bossMapID);
		if(NULL==logicMaps || logicMaps->size()!=1)
		{
			GfxWriteLog( LOG_INFO_FILELINE, LOG_SWITCH_INFO, "LC_ServerMapLogic_WatchTowerPlace::SetWatchTowerState serverid:%d, storyID:%d, bossMapID:%d, towerID:%d, bossID:%d, state:%d r6", 
				GetGameServerID(), storyID, bossMapID, towerID, bossID, state);
			return;
		}

		LC_ServerMapLogicPlaceVector::iterator kIt=logicMaps->begin();
		LC_ServerMapLogic_Place* Place = *kIt;
		if(NULL == Place)
		{
			GfxWriteLog( LOG_INFO_FILELINE, LOG_SWITCH_INFO, "LC_ServerMapLogic_WatchTowerPlace::SetWatchTowerState serverid:%d, storyID:%d, bossMapID:%d, towerID:%d, bossID:%d, state:%d r7", 
				GetGameServerID(), storyID, bossMapID, towerID, bossID, state);
			return;
		}

		//先把上次状态的结束定时器取消掉
		int timerID = findIt->second.GetTimerID();
		UT_TimerManager* pkTimerMgr = UT_TimerManager::GetSingletonPtr();
		if(0 != timerID)
		{
			pkTimerMgr->CancelTimer(timerID);
		}

		//发送神将被击杀公告
		uint64_t lastGuildID = findIt->second.GetGuildID();		
		if(lastGuildID != 0)
		{
			int towerID = findIt->second.GetTowerID();

			UT_ServerHelper::_GameNoticeFmort noticeFmort;
			noticeFmort.SetMsgID(22);
			noticeFmort.AddCharType(towerID);
			UT_ServerHelper::BrocastNoticeMsg(noticeFmort, lastGuildID);
		}

		findIt->second.ClearDamageInfo();
		findIt->second.SetState(state,bossMapID,bossID, vaildTime);
		findIt->second.SetTimerID(endTimerID);

		int npcLogicID = Place->GetDamageNpcLogicID();
		LC_ServerNPC* pkNpc = (LC_ServerNPC*)pkNPCManager->FindNPC(npcLogicID);
		if(NULL == pkNpc)
		{
			GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "SetWatchTowerState pkNpc null, npcLogicID:%d, mapID:%d", npcLogicID, bossMapID);
		}
		else
		{
			int npcID = pkNpc->GetCharType();
			LC_EncountedStatMgr& kStatMgr = pkNpc->GetEncountedStatMgr();
			LC_EncountedStat* pkStat = kStatMgr.GetStat(LC_ECT_TP_Damage);
			if(NULL == pkStat)
			{
				GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "SetWatchTowerState kStatMgr null, npcID:%d, mapID:%d", npcID, bossMapID);
			}
			else
			{
				const LC_EncountedStatEntry* rkGuildEntry = pkStat->GetTopEntry(LC_ECT_CAT_GUILD);
				if (NULL== rkGuildEntry)
				{
					GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "SetWatchTowerState rkGuildEntry null, npcID:%d, mapID:%d", npcID, bossMapID);
				}
				else
				{
					uint64_t guildID = rkGuildEntry->m_iGuildID;
					LC_ServerGuildInfo* pkGuildInfo = LC_ServerGuildManager::GetSingletonPtr()->GetGuildInfoByID(guildID);
					if(NULL == pkGuildInfo)
					{
						GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "SetWatchTowerState pkGuildInfo null, guildID:%lld, mapID:%d", guildID, bossMapID);
					}
					else
					{
						//设置公会信息
						findIt->second.EntryTempOwer(guildID, TPSTR2STDSTR(pkGuildInfo->GetGuildName()));

						//设置公会会长信息
						GuildServerMember* pkMaster = pkGuildInfo->GetMasterInfo();
						if(NULL == pkMaster)
						{
							GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "SetWatchTowerState pkMaster null, guildID:%lld, mapID:%d", guildID, bossMapID);
						}
						else
						{
							findIt->second.SetGuildLeaderInfo(pkMaster);
						}

						LC_EncountedStatContainer* pkStatPlayerCon = pkStat->GetEncountedStat(LC_ECT_CAT_ACTOR);
						if(NULL==pkStatPlayerCon)
						{
							GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "SetWatchTowerState pkStatPlayerCon null, guildID:%lld, mapID:%d", guildID, bossMapID);
						}
						else if(NULL != pkGuildInfo)
						{
							int i = 0;
							const uint64_object_id_vector& rkPlayerList = pkStatPlayerCon->GetSortedIDs();
							for(uint64_object_id_vector::const_iterator it=rkPlayerList.begin(); it!=rkPlayerList.end(); ++it)
							{
								unique_id_type playerID = pkStatPlayerCon->GetStatCharID(*it);									
								if(pkGuildInfo->GetMember(playerID))
								{
									//把该公会内，有伤害的人都存起来
									findIt->second.UpdateDamagePlayerID(playerID);

									if(i < 3)
									{
										//设置伤害玩家信息
										GuildServerMember* pkGuildPlayerInfo = pkGuildInfo->GetMember(playerID);
										if(NULL != pkGuildPlayerInfo)
										{
											findIt->second.UpdateGuildDamagePlayerInfo(pkGuildPlayerInfo);
											++i;
										}
									}
								}
							}
						}
					}
				}
			}			
		}
	}
	else if(state == WatchTower_Owner)
	{
		findIt->second.SetState(state,bossMapID,bossID, vaildTime);

		uint64_t guildID = findIt->second.GetGuildID();
		if(0 == guildID)
		{
			GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "SetWatchTowerState guildID=0, state:%d, guildID:%lld, mapID:%d", WatchTower_Owner, guildID, bossMapID);
		}

		//发送确认占领公告
		UT_ServerHelper::_GameNoticeFmort noticeFmort;
		noticeFmort.SetMsgID(23);
		noticeFmort.AddString(STDSTR2TPSTR(findIt->second.m_sGuildName));
		noticeFmort.AddCharType(towerID);//哨塔id
		UT_ServerHelper::BrocastNoticeMsg(noticeFmort);

		//给伤害列表中的人发奖
		findIt->second.SendDamageReward();
	}

	UpdateStateToRedis(id);

	MG_Notify_Guild_Fight_Tower_Info notify;
	ST_Single_Tower_info info;
	findIt->second.PackTowerInfo(info);
	notify.m_TowerVec.push_back(info);
	GetGameServerApp()->BroadCastMsgToClient(MGPT_GUILD_FIGHT_TOWER_INFO_NOTIFY, &notify);
}

void LC_ServerMapLogic_WatchTowerPlace::BroadcastTowerInfo_Lite(ST_Single_Tower_info_lite& input)
{
	MG_Notify_Guild_Fight_Tower_Info_lite notify;

	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();

	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerMapLogicPlaceVector* pkLogicMaps = pkWorldManager->GetMapLogicByRes(GetMapResID());
	if(NULL == pkLogicMaps)
		return;
	if(pkLogicMaps->size() != 1)
		return;

	LC_ServerMapLogicPlaceVector::iterator it=pkLogicMaps->begin();
	LC_ServerMapLogic_Place* pkPlace = *it;
	if(NULL == pkPlace)
		return;

	LC_ServerMapLogic_ActorManger& ActorMgr1 = pkPlace->GetActorManger();
	object_id_vector& retPlayer1 = ActorMgr1.GetPlayerList();
	notify.towerData.m_MapId = input.m_MapId;
	notify.towerData.m_MonsterId = input.m_MonsterId;
	notify.towerData.m_MonsterHP = input.m_MonsterHP;
	notify.towerData.m_uTowerID = input.m_uTowerID;
	GetGameServerApp()->BroadCastMsgToClient(MGPT_GUILD_FIGHT_TOWER_INFO_NOTIFY_LITE, &notify);
}

void LC_ServerMapLogic_WatchTowerPlace::BroadcastWatchTowerInfo()
{
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "BroadcastWatchTowerInfo_start... serverid:%d\n", GetGameServerID());

	MG_Notify_Guild_Fight_Tower_Info notify;
	//std::stringstream ss;
	//int i = 0;
	for(WatchTowerGuildInfoMap::iterator findIt = m_nBestGuildInfo.begin();findIt != m_nBestGuildInfo.end();++findIt)
	{
		//ss<<"index:"<<i<<" id:"<<findIt->second.m_Id<<" guildID:"<<findIt->second.m_GuildId<<" monsterID:"<<findIt->second.m_MonsterId<<" state:"<<findIt->second.m_nState<<" guilaName:"<<findIt->second.m_sGuildName<<" mapID:"<<findIt->second.m_MapId<<" nextTime:"<<findIt->second.m_NextTimeStamp<<std::endl;

		//++i;
		ST_Single_Tower_info info;
		findIt->second.PackTowerInfo(info);
		notify.m_TowerVec.push_back(info);
	}

	if(notify.m_TowerVec.size() == 0)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "BroadcastWatchTowerInfo towerSize=0 serverid:%d\n", GetGameServerID());
		return;
	}

	GetGameServerApp()->BroadCastMsgToClient(MGPT_GUILD_FIGHT_TOWER_INFO_NOTIFY, &notify);
	
	//std::string data = ss.str().c_str();
	//GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "BroadcastWatchTowerInfo_success... serverid:%d, value:%s\n", GetGameServerID(), data.c_str());
}

void LC_ServerMapLogic_WatchTowerPlace::SyncWatchTowerInfoToPlayer(LC_ServerPlayer * pkPlayer)
{
	//std::stringstream logStr;
	//logStr << "SyncWatchTowerInfoToPlayer  serverId[" << GetGameServerID() << "]";
	MG_Notify_Guild_Fight_Tower_Info notify;
	//int i= 0;
	for(WatchTowerGuildInfoMap::iterator findIt = m_nBestGuildInfo.begin();findIt != m_nBestGuildInfo.end();++findIt)
	{
		//logStr<<"index:"<<i<<" id:"<<findIt->second.m_Id<<" guildID:"<<findIt->second.m_GuildId<<" monsterID:"<<findIt->second.m_MonsterId<<" state:"<<findIt->second.m_nState<<" mapID:"<<findIt->second.m_MapId<<" nextTime:"<<findIt->second.m_NextTimeStamp<<std::endl;
		//++i;
		ST_Single_Tower_info info;
		findIt->second.PackTowerInfo(info);
		notify.m_TowerVec.push_back(info);
	}

	//logStr <<" towerSize:[" << notify.m_TowerVec.size() << "]";
	//GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "%s",logStr.str().c_str());

	if(notify.m_TowerVec.size() != 0)
	{
		pkPlayer->SendMsgToClient(MGPT_GUILD_FIGHT_TOWER_INFO_NOTIFY, &notify);
	}
}


void LC_ServerMapLogic_WatchTowerPlace::SetTowerLocation(int storyID, int towerID, int x, int y, int z)
{
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "SetTowerLocation storyID:%d,  towerID:%d, x:%d, y:%d, z:%d\n");
	uint64_t id = LC_Helper::EncodeScore(storyID, towerID);
	WatchTowerGuildInfoMap::iterator findIt = m_nBestGuildInfo.find(id);
	if(findIt == m_nBestGuildInfo.end())
	{
		WatchTowerGuildInfo info(storyID, towerID);
		m_nBestGuildInfo.insert(std::make_pair(id, info));

		findIt = m_nBestGuildInfo.find(id);
	}
	
	findIt->second.InitXYZ(x,y,z);
}

uint64_t LC_ServerMapLogic_WatchTowerPlace::GetBossGuildID(int storyID, int npcID)
{
	for(WatchTowerGuildInfoMap::iterator it = m_nBestGuildInfo.begin(); it!=m_nBestGuildInfo.end(); ++it)
	{
		uint32_t id = it->second.GetStoryID();
		if(storyID==id && npcID==it->second.GetMonsterCharTypeID())
		{
			return it->second.GetGuildID();
		}
	}
	return 0;
}

int LC_ServerMapLogic_WatchTowerPlace::GetTowerID(int storyID, int npcID)
{
	for(WatchTowerGuildInfoMap::iterator it = m_nBestGuildInfo.begin(); it!=m_nBestGuildInfo.end(); ++it)
	{
		uint32_t id = it->second.GetStoryID();
		if(storyID==id && npcID==it->second.GetMonsterCharTypeID())
		{
			return it->second.GetTowerID();
		}
	}
	return 0;
}

void LC_ServerMapLogic_WatchTowerPlace::SetDamageNpcLogicID(int logicID)
{
	m_nDamageNpcLogicID = logicID;
}

int LC_ServerMapLogic_WatchTowerPlace::GetDamageNpcLogicID()
{
	return m_nDamageNpcLogicID;
}

bool LC_ServerMapLogic_WatchTowerPlace::CheckAddKillControllerReward(int mapID)
{
	for(WatchTowerGuildInfoMap::iterator it=m_nBestGuildInfo.begin(); it!=m_nBestGuildInfo.end(); ++it)
	{
		if(it->second.m_MapId==mapID && (it->second.GetState()==WatchTower_AllKilled || it->second.GetState()==WatchTower_Temp))
		{
			return true;
		}
	}

	return false;
}

void LC_ServerMapLogic_WatchTowerPlace::SetLoadFlag()
{
	m_nLoadFlag = true;
}

bool LC_ServerMapLogic_WatchTowerPlace::GetLoadFlag()
{
	return m_nLoadFlag;
}

bool LC_ServerMapLogic_WatchTowerPlace::SetMonsterHP(int charType, int64_t hp, int32_t nMapResId)
{
	if(hp < 0)
	{
		hp = 0;
	}

	for(WatchTowerGuildInfoMap::iterator it=m_nBestGuildInfo.begin(); it!=m_nBestGuildInfo.end(); ++it)
	{
		if(it->second.m_MonsterId == charType)
		{
			if(it->second.m_nMonsterHP != hp)
			{
				it->second.SetMonsterHP(hp);
				UpdateStateToRedis(it->first);
				ST_Single_Tower_info_lite liteData;
				liteData.m_MapId = nMapResId;
				liteData.m_MonsterId = charType;
				liteData.m_MonsterHP = hp;
				liteData.m_uTowerID = it->second.GetTowerID();
				BroadcastTowerInfo_Lite( liteData );
			}
			return true;
		}
	}

	return false;
}

void LC_ServerMapLogic_WatchTowerPlace::UpdateStateToRedis(uint64_t id)
{
	//global节点给redis存储哨塔数据
	if(false == GetGameServerApp()->IsNodeType_Global())
	{
		return;
	}

	WatchTowerGuildInfoMap::iterator findIt = m_nBestGuildInfo.find(id);
	if(findIt != m_nBestGuildInfo.end())
	{
		int mapID = findIt->second.m_MapId;
		if(mapID == 0)
		{
			if(findIt->second.m_nState != 0)
			{
				GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "UpdateStateToRedis mapID=0, state:%d, monsterID:%d", findIt->second.m_nState, findIt->second.m_MonsterId);
			}
			return;
		}

		std::stringstream ss;
		boost::ajson::save_to_buff(findIt->second, ss);
		std::string data = ss.str().c_str();

		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "UpdateStateToRedis state:%d, id:%d, towerID:%d, monsterID:%d, nextStateTime:%d, guildID:%lld, mapID:%d, x:%d, y:%d, z:%d", 
			findIt->second.m_nState, id, findIt->second.GetTowerID(), findIt->second.m_MonsterId, findIt->second.m_NextTimeStamp, findIt->second.m_GuildId, findIt->second.m_MapId, findIt->second.m_x, findIt->second.m_y, findIt->second.m_z);

		UT_ServerHelper::UpdateWatchTowerInfoCache(id, data);
	}
}

void LC_ServerMapLogic_WatchTowerPlace::SetMonsterGuildID(int mapID, int monsterID, uint64_t guildID)
{
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerMapLogicPlaceVector* pkPlaceVec = pkWorldManager->GetMapLogicByRes(mapID);
	if(NULL==pkPlaceVec || pkPlaceVec->size()!=1)
	{
		return;
	}

	LC_ServerMapLogicPlaceVector::iterator it=pkPlaceVec->begin();
	LC_ServerMapLogic_Place* pkPlace = *it;
	if(NULL == pkPlace)
	{
		return;
	}

	LC_ServerNPCManager* pkNPCManager = SERVER_GET_NPC_MANAGER();

	LC_ServerMapLogic_ActorManger& ActorMgr = pkPlace->GetActorManger();
	object_id_vector& npcList = ActorMgr.GetNpcList();
	for(int i=0; i<npcList.size(); ++i)
	{
		LC_ServerNPC* pkNpc = (LC_ServerNPC*)pkNPCManager->FindNPC(npcList[i]);
		if(NULL!=pkNpc && pkNpc->GetCharType()==monsterID)
		{
			pkNpc->SetGuildID(guildID);
			break;
		}
	}
}

void LC_ServerMapLogic_WatchTowerPlace::SetMonsterCurrentHP(int mapID, int monsterID, int hp)
{
	if(hp <= 0)
	{
		return;
	}

	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerMapLogicPlaceVector* pkPlaceVec = pkWorldManager->GetMapLogicByRes(mapID);
	if(NULL==pkPlaceVec || pkPlaceVec->size()!=1)
	{
		return;
	}

	LC_ServerMapLogicPlaceVector::iterator it=pkPlaceVec->begin();
	LC_ServerMapLogic_Place* pkPlace = *it;
	if(NULL == pkPlace)
	{
		return;
	}

	LC_ServerNPCManager* pkNPCManager = SERVER_GET_NPC_MANAGER();

	LC_ServerMapLogic_ActorManger& ActorMgr = pkPlace->GetActorManger();
	object_id_vector& npcList = ActorMgr.GetNpcList();
	for(int i=0; i<npcList.size(); ++i)
	{
		LC_ServerNPC* pkNpc = (LC_ServerNPC*)pkNPCManager->FindNPC(npcList[i]);
		if(NULL!=pkNpc && pkNpc->GetCharType()==monsterID)
		{
			pkNpc->SetHP(hp);
			break;
		}
	}
}

void LC_ServerMapLogic_WatchTowerPlace::UpdateWatchTowerFromSaveState(uint64_t id, std::string str)
{
	WatchTowerGuildInfo info;
	std::string errorMsg;
	if(!boost::ajson::load_from_buff(info, str.c_str(), errorMsg))
	{
		GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_INFO, "LoadStateFromRedis Failed parse string", errorMsg);
		return;
	}

	//修改数据

	//1 活动有效时间
	uint32_t currentTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	int rewardTime = 0;
	if(info.m_NextTimeStamp!=0 && currentTime >= info.m_NextTimeStamp)
	{
		bool tag = true;
		uint32_t storyID = 0;
		uint32_t npcID = 0;
		int vaildTime1 = 0;
		int vaildTime2 = 0;
		//这一波已经结束了
		switch(info.m_nState)
		{
		case WatchTower_Protocted://未开启争夺
			info.Clear();
			info.m_nState = WatchTower_AllKilled;
			info.m_NextTimeStamp = 0;
			break;
		case WatchTower_AllKilled://开启争夺
			//开启争夺状态info.m_NextTimeStamp=0，这个case不应该命中
			GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_INFO, "UpdateWatchTowerFromSaveState state error! state:%d, endTime:%d", info.m_nState, info.m_NextTimeStamp);
			break;
		case WatchTower_Temp://临时占领
			LC_Helper::DecodeScore(info.m_Id, storyID, npcID);			
			tag = GetGlobalSetting.GetBestGuildVaildTimeByStoryID(storyID, vaildTime1, vaildTime2);
			if(!tag)
			{
				GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_INFO, "GetBestGuildVaildTimeByStoryID error! towerID:%d, m_GuildId:%lld, storyid:%d, vaildTime1:%d, vaildTime2:%d", info.GetTowerID(), info.m_GuildId, storyID, vaildTime1, vaildTime2);
			}

			//1 确认占领中....
			if(currentTime <  info.m_NextTimeStamp+vaildTime2)
			{
				//补发确认占领的奖励
				rewardTime = currentTime-info.m_NextTimeStamp;
				GfxWriteLog( LOG_TAG_INFO, LOG_SWITCH_ERROR, "WatchTower_Temp->WatchTower_Owner-ing, towerID:%d, Reissue reward m_GuildId:%lld, currentTime:%d, lastStateEndTime:%d, rewardTime: %d", info.GetTowerID(), info.m_GuildId, currentTime, info.m_NextTimeStamp, rewardTime);
				info.SendOfflineReward(rewardTime, true);
				
				//发送确认占领邮件
				info.SendDamageReward();

				info.m_nState = WatchTower_Owner;
				info.m_NextTimeStamp = info.m_NextTimeStamp+vaildTime2;	
				// 补发 buffid
				UT_ServerHelper::AddSendRewardCache(GuildLoadRewardCacheType::GLRCT_BUFF,info.GetTowerID(), info.GetGuildID(), 3,info.m_NextTimeStamp,true);
				
			}
			else
			{
				//2 确认占领已结束 补发确认占领的奖励
				GfxWriteLog( LOG_TAG_INFO, LOG_SWITCH_ERROR, "WatchTower_Temp->WatchTower_Owner-end, towerID:%d, Reissue reward m_GuildId:%lld, rewardTime: %d", info.GetTowerID(), info.m_GuildId, vaildTime2);
				info.SendOfflineReward(vaildTime2);
				// 清除towerId
				UT_ServerHelper::AddSendRewardCache(GuildLoadRewardCacheType::GLRCT_BUFF,info.GetTowerID(), info.GetGuildID(), 4,info.m_NextTimeStamp,false);
				info.Clear();
				info.m_nState = WatchTower_AllKilled;
				info.m_NextTimeStamp = 0;			
			}
			break;
		case WatchTower_Owner://确认占领
			//补发确认占领期间的奖励
			rewardTime = info.m_NextTimeStamp-info.m_RewardTimeStamp;
			GfxWriteLog( LOG_TAG_INFO, LOG_SWITCH_ERROR, "WatchTower_Owner->WatchTower_Owner-end, towerID:%d, Reissue reward m_GuildId:%lld, stateEndTime:%d, lastRewardTime:%d, rewardTime:%d", info.GetTowerID(), info.m_GuildId, info.m_NextTimeStamp, info.m_RewardTimeStamp, rewardTime);
			info.SendOfflineReward(rewardTime);
			// 清除towerId
			UT_ServerHelper::AddSendRewardCache(GuildLoadRewardCacheType::GLRCT_BUFF,info.GetTowerID(), info.GetGuildID(), 5,info.m_NextTimeStamp,false);
			info.Clear();
			info.m_nState = WatchTower_AllKilled;
			info.m_NextTimeStamp = 0;
			break;
		default:
			return;
		}
	}
	else
	{
		SetMonsterCurrentHP(info.m_MapId, info.m_MonsterId, info.m_nMonsterHP);
		//没有结束
		switch(info.m_nState)
		{
		case WatchTower_Protocted://未开启争夺
			break;
		case WatchTower_AllKilled://开启争夺
			break;
		case WatchTower_Temp://临时占领
			//处于临时占有状态，修改怪的guildID
			SetMonsterGuildID(info.m_MapId, info.m_MonsterId, info.m_GuildId);
			break;
		case WatchTower_Owner://确认占领
			//补发确认占领期间的奖励
			rewardTime = currentTime-info.m_RewardTimeStamp;
			GfxWriteLog( LOG_TAG_INFO, LOG_SWITCH_ERROR, "WatchTower_Owner->WatchTower_Owner, towerID:%d, Reissue reward m_GuildId:%lld, rewardTime: %d", info.GetTowerID(), info.m_GuildId, rewardTime);
			info.SendOfflineReward(rewardTime, true);
			// 补发 towerid
			UT_ServerHelper::AddSendRewardCache(GuildLoadRewardCacheType::GLRCT_BUFF,info.GetTowerID(), info.GetGuildID(), 6, info.m_NextTimeStamp,true);
			break;
		default:
			return;
		}
	}

	WatchTowerGuildInfoMap::iterator findIt = m_nBestGuildInfo.find(id);
	if(findIt != m_nBestGuildInfo.end())
	{
		findIt->second = info;
	}
	else
	{
		m_nBestGuildInfo.insert(std::make_pair(id, info));
	}

	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "UpdateWatchTowerFromSaveState serverID:%d, towerID:%d, m_GuildId:%lld, mapID:%d, x:%d, y:%d, z:%d", GetGameServerID(), info.GetTowerID(), info.m_GuildId, info.m_MapId, info.m_x, info.m_y, info.m_z);

	UpdateStateToRedis(id);
}

void LC_ServerMapLogic_WatchTowerPlace::UpdateWatchTowerFromRedis()
{
	m_nFirstLoad = true;

  	mem::map<uint64_t, std::string> watchTowerInfo;
	UT_ServerHelper::RedisGetData_StringRange(UT_REDIS_KEY_WATCH_TOWER_INFO, watchTowerInfo);
	if(watchTowerInfo.empty())
	{
		GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "UpdateWatchTowerFromRedis_isEmpty! serverid:", GetGameServerID());
		return;
	}
	
	for(mem::map<uint64_t, std::string>::iterator it=watchTowerInfo.begin(); it!=watchTowerInfo.end(); ++it)
	{
		WatchTowerGuildInfo info;
		std::string errorMsg;
		if(!boost::ajson::load_from_buff(info, it->second.c_str(), errorMsg))
		{
			GfxWriteFmtLog(LOG_ERROR_FILELINE, LOG_SWITCH_INFO, "UpdateWatchTowerFromRedis Failed parse string", errorMsg);
			continue;
		}

		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "UpdateWatchTowerFromRedis... serverid:%d, id:%lld, value:%s\n", GetGameServerID(), it->first, it->second.c_str());
		WatchTowerGuildInfoMap::iterator findIt = m_nBestGuildInfo.find(it->first);
		if(findIt != m_nBestGuildInfo.end())
		{
			findIt->second = info;
		}
		else
		{
			m_nBestGuildInfo.insert(std::make_pair(it->first, info));
		}
	}
}