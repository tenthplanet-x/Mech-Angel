#include "LC_ServerPlayerQueueManager.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerMapLogic_NewBee.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerPlayerEnterLeaveManager.h"
#include "GameServerApp.h"
#include "MG_LoginProtocol.h"

#include <algorithm>

using namespace Utility;
using namespace GameLogic;

LC_ServerPlayerQueueManager::LC_ServerPlayerQueueManager()
:m_enterLeaveManager(NULL)
{
	m_lastCheckTime = 0;
}

LC_ServerPlayerQueueManager::~LC_ServerPlayerQueueManager(void)
{
}

void LC_ServerPlayerQueueManager::CheckWaitingQueue(float fCurrentTime,float fDeltaTime)
{
	unique_id_set	eraseUsers;
	mem::vector<LC_ServerPlayer*> newBeePlayers;
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerWorldManager*	pkWorldManager	= SERVER_GET_WORLD_MANAGER(); 
	for (mem::list<PlayerEnterData>::iterator it = m_waitingQueue.begin(); it != m_waitingQueue.end();++it)
	{
		PlayerEnterData& data = *it;
		LC_ServerPlayer* player = (LC_ServerPlayer*)(pkPlayerManager->FindPlayer(data.charID));
		IF_A_VALID_PLAYER(player)
		{
			//只要地图配置了分流，所有玩家都支持地图分流
			newBeePlayers.push_back(player);
		}
		else
		{
			eraseUsers.insert(data.charID);
			GfxWriteFmtLog(LOG_WARNING_FILELINE, LOG_SWITCH_WARNING, "unvalid waiting player!", data.userID, data.charID);
		}
	}
	CheckNewbee(newBeePlayers, eraseUsers);

	for (mem::list<PlayerEnterData>::iterator it = m_waitingQueue.begin(); it != m_waitingQueue.end(); )
	{
		PlayerEnterData& data = *it;
		if (data.charID==INVALID_UNIQUE_ID || eraseUsers.find(data.charID)!=eraseUsers.end())
		{
			GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "remove waiting!", data.userID, data.charID);
			m_waitingQueue.erase(it++);
		}
		else
		{
			++it;
		}
	}

	//0 xinshou, 1 putong
	int normalWaitingCount = 0;
	int newbeeWaitingCount = 0;
	for (mem::list<PlayerEnterData>::iterator it = m_waitingQueue.begin(); it != m_waitingQueue.end();++it)
	{
		PlayerEnterData& data = *it;
		LC_ServerPlayer* player = (LC_ServerPlayer*)(pkPlayerManager->FindPlayer(data.charID));
		if (player)
		{
			if (pkWorldManager->IsNewbee(player))
			{
				PlayerWaitingInfo(player, newbeeWaitingCount, 0);
				++newbeeWaitingCount;
			}
			else
			{
				PlayerWaitingInfo(player, normalWaitingCount, 1);
				++normalWaitingCount;
			}
		}
	}
}

size_t LC_ServerPlayerQueueManager::GetWaitingQueueLenth()
{
	return m_waitingQueue.size();
}

void LC_ServerPlayerQueueManager::AddWaitingQueue(user_id_type userID, const unique_id_impl& charID)
{
	bool isFind = false;
	for(mem::list<PlayerEnterData>::iterator it = m_waitingQueue.begin(); it != m_waitingQueue.end(); ++it)
	{
		PlayerEnterData& userData = *it;
		if (userData.charID == charID)
		{
			isFind = true;
			break;
		}
	}
	if (!isFind)
	{
		PlayerEnterData userData;
		userData.userID = userID;
		userData.charID = charID;
		userData.lastActiveTime = GET_PROCESS_TIME;
		m_waitingQueue.push_back(userData);
	}
}

int LC_ServerPlayerQueueManager::IsAlreadyInWaitingQueue(user_id_type userID, const unique_id_impl& charID)
{
	int inQueueLenth = 0;
	
	bool isFind = false;
	for(mem::list<PlayerEnterData>::iterator it = m_waitingQueue.begin(); it != m_waitingQueue.end(); ++it)
	{
		++inQueueLenth;
		PlayerEnterData& userData = *it;
		if (userData.charID == charID)
		{
			isFind = true;
			break;
		}
	}
	if (!isFind)
	{
		inQueueLenth = 0;
	}
	return inQueueLenth;
}

void LC_ServerPlayerQueueManager::CheckNewbee( mem::vector<LC_ServerPlayer*>& newbeeList, unique_id_set& enterPlayerList)
{
	size_t oneTimeEnterCount = 0;
	LC_ServerPlayerManager* playerManager	= SERVER_GET_PLAYER_MANAGER();
	LC_ServerWorldManager*	pkWorldManager	= SERVER_GET_WORLD_MANAGER(); 
	int inGamePlayerCount = playerManager->GetInGamePlayerCount();
	int allowMaxEnterCount = GetGameServerApp()->GetServerInfo().m_iWaitCount;
	for (size_t i = 0; i < newbeeList.size(); ++i)
	{
		LC_ServerPlayer* newbee = newbeeList[i];
		//地图分流用newbee做，这里oneTimeEnterCount需要修改 20->50(参考CheckNormal)
		if (oneTimeEnterCount < 50 && allowMaxEnterCount > inGamePlayerCount)
		{
			LC_WorldLocationAsset* pkWorldLocationAsset = newbee->GetWorldLocationAsset();
			LC_WorldLocationAssetEntry* pkFreelMapLocationAssetEntry = pkWorldLocationAsset->GetWorldLocationAssetEntryByIndex(WLT_PERMANENT);

			// 是不是切线玩家
			user_id_type uid = newbee->GetUserID();
			bool bSwitchLine = m_enterLeaveManager->CheckSwitchMapLinePlayer(uid);

			LC_ServerMapLogic_Place* newbeeLogic = pkWorldManager->FindFreeNewbeeMapLogic(pkFreelMapLocationAssetEntry->GetMapResID(), !bSwitchLine);
			if (NULL == newbeeLogic)
			{
				GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "CheckNewbee Failed!", newbee->GetInstance(),  "MapResID", pkFreelMapLocationAssetEntry->GetMapResID());
			}
			else
			{
				if(pkFreelMapLocationAssetEntry->GetMapLogicID() > 0)
					pkFreelMapLocationAssetEntry->SetMapLogicID(newbeeLogic->GetMapLogicID());
			}
			
			{
				PlayerEnterWorld(newbee);
				enterPlayerList.insert(newbee->GetInstance());
				++oneTimeEnterCount; 
				++inGamePlayerCount;
			}
		}
		else
		{
			GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "CheckNewbee", newbee->GetID(), newbee->GetInstance(), oneTimeEnterCount, allowMaxEnterCount, inGamePlayerCount);
		}
	}
}

void LC_ServerPlayerQueueManager::CheckNormal( mem::vector<LC_ServerPlayer*>& normalList, unique_id_set& enterPlayerList)
{
	size_t oneTimeEnterCount = 0;
	LC_ServerPlayerManager* playerManager = SERVER_GET_PLAYER_MANAGER();
	int inGamePlayerCount = playerManager->GetInGamePlayerCount();
	int allowMaxEnterCount = GetGameServerApp()->GetServerInfo().m_iWaitCount;
	for (size_t i = 0; i < normalList.size(); ++i)
	{
		LC_ServerPlayer* normalPlayer = normalList[i];
		if (allowMaxEnterCount > inGamePlayerCount && oneTimeEnterCount < 50)
		{
			PlayerEnterWorld(normalPlayer);
			enterPlayerList.insert(normalPlayer->GetInstance());
			++oneTimeEnterCount; 
			++inGamePlayerCount;
		}
		else
		{
			GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "CheckNormal Full!", inGamePlayerCount, allowMaxEnterCount);
		}
	}
}

void LC_ServerPlayerQueueManager::PlayerEnterWorld( LC_ServerPlayer* player )
{
	if (m_enterLeaveManager && player)
	{
		GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "PlayerEnterWorld", player->GetID(), player->GetUserID(), player->GetInstance());
		m_enterLeaveManager->PlayerEnterGame(player, LMIOT_RECOVER);
	}
}

void LC_ServerPlayerQueueManager::PlayerWaitingInfo( LC_ServerPlayer* player, int waitingCount, int type)
{
	//0 xinshou, 1 putong
	Protocol::MG_RLT_Waiting_Info waitingInfo;
	waitingInfo.m_waitingCount = waitingCount;
	waitingInfo.m_waitingType = type;
	player->SendMsgToClient(MGPT_GC_RLT_WAITING_INFO,&waitingInfo);
}

void LC_ServerPlayerQueueManager::SetEnterLeaverManager( LC_ServerPlayerEnterLeaveManager* manager )
{
	m_enterLeaveManager = manager;
}

int LC_ServerPlayerQueueManager::FindNormalPlayerCount()
{
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	return pkPlayerManager->GetInGamePlayerCountLessLevel(MIN_NOT_NEWBIE_LEVEL);
}