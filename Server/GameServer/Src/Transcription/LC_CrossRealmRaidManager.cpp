#include "GameServerApp.h"
#include "LC_CrossRealmRaidManager.h"
#include "MG_Game2Guild.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerPlayerEnterLeaveManager.h"
#include "UT_ServerHelper.h"

using namespace GameLogic;
using namespace Skill;
using namespace Protocol;
using namespace CSVFile;
using namespace Utility;
LC_CrossRealmRaidManager::LC_CrossRealmRaidManager():m_CrossRealmRaid(), m_CrossRealmPlaceRaid(), m_LogicId2RaidId()
{

}

LC_CrossRealmRaidManager::~LC_CrossRealmRaidManager()
{
	Clear();
}

void LC_CrossRealmRaidManager::Clear()
{
	m_CrossRealmRaid.clear();
	m_CrossRealmPlaceRaid.clear();
	m_LogicId2RaidId.clear();
}

bool LC_CrossRealmRaidManager::Init()
{
	m_CrossRealmRaid.clear();
	m_CrossRealmPlaceRaid.clear();
	m_LogicId2RaidId.clear();
	return true;
}

void LC_CrossRealmRaidManager::UnInit()
{
	Clear();
}

int LC_CrossRealmRaidManager::CreateCrossRealmRaid(int resid, RaidCallBack createdcallback, RaidCallBack failedcallback, RaidCallBack destorycallback)
{
	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "CreateCrossRealmRaid", resid);
	//·¢ËÍÏûÏ¢
	MG_SS_CreateRaid_REQ req;
	req.m_ResId = resid;
	if (!SendMsgToSociety(MGPT_GGU_CREATE_RAID_REQ, &req))
	{
		return -1;
	}
	LC_CrossRealmRaidInfo& info = m_CrossRealmRaid[resid];
	info.m_ResId = resid;
	info.m_CreatedCallBack = createdcallback;
	info.m_CreateFailedCallBack = failedcallback;
	info.m_RaidDestoryCallBack = destorycallback;
	info.m_TimeStamp = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	return 0;
}

void LC_CrossRealmRaidManager::CreateCrossRealmRaidResp(ResultType result, int64_t raidid, uint32_t serverid, int32_t nResId)
{
	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "CreateCrossRealmRaidResp", result, raidid, serverid, nResId);
	mem::map<int, LC_CrossRealmRaidInfo>::iterator it = m_CrossRealmRaid.find(nResId);
	if (it == m_CrossRealmRaid.end())
	{
		if (result == RE_SUCCESS && serverid == (uint32_t)UT_ServerHelper::GetServerId())
		{
			__SendDestoryCrossRealmRaid(raidid, nResId);
		}
		return;
	}
	LC_CrossRealmRaidInfo& rRaidInfo = it->second;
	if (result != RE_SUCCESS)
	{
		rRaidInfo.m_RaidId = 0;
		rRaidInfo.m_RaidServerId = 0;
		rRaidInfo.m_CreateFailedCallBack(raidid, serverid, nResId);
	}
	else
	{
		rRaidInfo.m_RaidId = raidid;
		rRaidInfo.m_RaidServerId = serverid;
		rRaidInfo.m_CreatedCallBack(raidid, serverid, nResId);
	}
}

void LC_CrossRealmRaidManager::DestoryCrossRealmRaid(int iMapResID)
{
	mem::map<int, LC_CrossRealmRaidInfo>::iterator it = m_CrossRealmRaid.find(iMapResID);
	if (it == m_CrossRealmRaid.end())
	{
		return;
	}
	int64_t globalraidid = it->second.m_RaidId;
	__SendDestoryCrossRealmRaid(globalraidid, iMapResID);
}

void LC_CrossRealmRaidManager::DestoryCrossRealmRaidGameNotify(int64_t nRaidID, int iMapResID, int nServerID)
{
	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "DestoryCrossRealmRaidGameNotify", nRaidID, iMapResID, nServerID);
	mem::map<int, LC_CrossRealmRaidInfo>::iterator it = m_CrossRealmRaid.find(iMapResID);
	if (it != m_CrossRealmRaid.end())
	{
		it->second.m_RaidDestoryCallBack(nRaidID, nServerID, iMapResID);
	}

	mem::map<int64_t, LC_CrossRealmPlaceRaidInfo>::iterator raidit = m_CrossRealmPlaceRaid.find(nRaidID);
	if (raidit != m_CrossRealmPlaceRaid.end())
	{
		LC_CrossRealmPlaceRaidInfo& rRaidInfo = raidit->second;
		rRaidInfo.m_Destory = true;
		LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
		LC_ServerMapLogic_Place* logic = pkWorldManager->GetMapLogic(rRaidInfo.m_LogicId);
		if (logic != NULL)
		{
			logic->Finsh();
		}
	}
}

ResultType LC_CrossRealmRaidManager::EnterCrossRealmRaid(LC_ServerPlayer* player, int iMapResID)
{
	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "EnterCrossRealmRaid", iMapResID);
	mem::map<int, LC_CrossRealmRaidInfo>::iterator it = m_CrossRealmRaid.find(iMapResID);
	if (it == m_CrossRealmRaid.end())
	{
		return RE_FAIL;
	}
	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "EnterCrossRealmRaid1", iMapResID);
	MG_SS_EnterRaid_REQ req;
	req.m_CharID = player->GetInstance();
	req.m_RaidId = it->second.m_RaidId;
	req.m_UserId = player->GetUserID();
	req.m_Level = player->GetLevel();
	if (!SendMsgToSociety(MGPT_GGU_ENTER_RAID_REQ, &req))
	{
		return RE_FAIL;
	}
	return RE_SUCCESS;
}

void LC_CrossRealmRaidManager::EnterCrossRealmRaidResp(LC_ServerPlayer* player, ResultType result, uint32_t targetserverid)
{
	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "EnterCrossRealmRaidResp", result, targetserverid);
	if (result != RE_SUCCESS)
	{
		return;
	}
	LC_ServerPlayerEnterLeaveManager* enterLeaveManager = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
	enterLeaveManager->PlayerTryEnterCrossRealm(player, targetserverid);
}

ResultType LC_CrossRealmRaidManager::PrepareCrossRealmRaid(int64_t raidid, int resid, int toplevel)
{
	static GameServerManager* pkServerMgr = GetGameServerApp()->GetServerManager();
	if (!pkServerMgr->IsServerOpened())
	{
		return RE_LOGIN_FAIL_WAIT_SERVER_OPEN;
	}

	mem::map<int64_t, LC_CrossRealmPlaceRaidInfo>::iterator raidit = m_CrossRealmPlaceRaid.find(raidid);
	if (raidit != m_CrossRealmPlaceRaid.end())
	{
		return RE_SUCCESS;
	}
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerMapLogic_Place* logic = (LC_ServerMapLogic_Place*)pkWorldManager->CreateServerMapLogic(0, resid, MT_EIGHTEEN_BRONZE_MAN, false);
	if (logic == NULL)
	{
		return RE_FAIL;
	}
	logic->SetCrossRealm(true, CrossRealmType_Raid, logic->GetMapLogicID());
	logic->SetMonsterLevel(toplevel);
	logic->Start();
	LC_CrossRealmPlaceRaidInfo& temp = m_CrossRealmPlaceRaid[raidid];
	temp.m_RaidId = raidid;
	temp.m_LogicId = logic->GetMapLogicID();
	m_LogicId2RaidId[temp.m_LogicId] = raidid;
	return RE_SUCCESS;
}

void LC_CrossRealmRaidManager::CrossRealmRaidEndNotify(int logicid)
{
	mem::map<int, int64_t>::iterator it = m_LogicId2RaidId.find(logicid);
	if (it == m_LogicId2RaidId.end())
	{
		return;
	}
	int64_t raidid = it->second;
	mem::map<int64_t, LC_CrossRealmPlaceRaidInfo>::iterator raidit = m_CrossRealmPlaceRaid.find(raidid);
	if (raidit == m_CrossRealmPlaceRaid.end())
	{
		return;
	}
	if (!raidit->second.m_Destory)
	{
		MG_SS_RaidDestory_Place_Report req;
		req.m_RaidId = raidid;
		SendMsgToSociety(MGPT_GGU_DESTROY_RAID_PLACE_REPORT, &req);
	}
	m_LogicId2RaidId.erase(raidit->second.m_LogicId);
	m_CrossRealmPlaceRaid.erase(raidit);
}

ResultType LC_CrossRealmRaidManager::PreEnterCrossRealmRaid_Place(int64_t raidid, const unique_id_impl& charID, user_id_type userid)
{
	mem::map<int64_t, LC_CrossRealmPlaceRaidInfo>::iterator raidit = m_CrossRealmPlaceRaid.find(raidid);
	if (raidit == m_CrossRealmPlaceRaid.end())
	{
		return RE_FAIL;
	}
	int logicid = raidit->second.m_LogicId;
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerMapLogic_Place* thelogic = (LC_ServerMapLogic_Place*)pkWorldManager->GetMapLogic(logicid);
	if (thelogic == NULL)
	{
		return RE_FAIL;
	}

	LC_ServerPlayerEnterLeaveManager* enterLeaveManager = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
	if (thelogic->GetMaxPlayer() == 0)
	{
		enterLeaveManager->AddCrossRealmRaidPlayerInfo(charID, userid, raidit->second.m_LogicId, logicid);
	}
	else
	{
		int playercount = thelogic->GetAllRealPlayerSize();
		int holdcount = enterLeaveManager->GetHoldCount(HoldDataType_CrossRealmRaid);
		int totalcount = playercount + holdcount;
		if (totalcount >= thelogic->GetMaxPlayer())
		{
			return RE_CROSSREALM_PLAYER_FULL;
		}
		enterLeaveManager->AddCrossRealmRaidPlayerInfo(charID, userid, raidit->second.m_LogicId, logicid);
	}
	return RE_SUCCESS;
}

ResultType LC_CrossRealmRaidManager::EnterCrossRealmRaid_Place(int logicid, LC_ServerPlayer* player)
{
	mem::map<int, int64_t>::iterator it = m_LogicId2RaidId.find(logicid);
	if (it == m_LogicId2RaidId.end())
	{
		return RE_FAIL;
	}
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerMapLogic_Place* thelogic = (LC_ServerMapLogic_Place*)pkWorldManager->GetMapLogic(logicid);
	if (thelogic == NULL)
	{
		return RE_FAIL;
	}
	return pkWorldManager->RequestChangePlayerLocation(player, LMIOT_TRANSFER, logicid, thelogic->GetMapResID(), UT_Vec3Int());
}

void LC_CrossRealmRaidManager::SocietyServerShutDown()
{
	mem::map<int, LC_CrossRealmRaidInfo>::iterator beg = m_CrossRealmRaid.begin();
	for (; beg != m_CrossRealmRaid.end(); ++beg)
	{
		LC_CrossRealmRaidInfo& rRaidInfo = beg->second;
		rRaidInfo.m_RaidDestoryCallBack(rRaidInfo.m_RaidId, rRaidInfo.m_RaidServerId, rRaidInfo.m_ResId);
	}
	
	mem::map<int64_t, LC_CrossRealmPlaceRaidInfo>::iterator raidbeg = m_CrossRealmPlaceRaid.begin();
	for (; raidbeg != m_CrossRealmPlaceRaid.end(); ++raidbeg)
	{
		raidbeg->second.m_Destory = true;
		LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
		LC_ServerMapLogic_Place* logic = pkWorldManager->GetMapLogic(raidbeg->second.m_LogicId);
		if (logic != NULL)
		{
			logic->Finsh();
		}
	}
}

LC_CrossRealmPlaceRaidInfo* LC_CrossRealmRaidManager::GetCrossRealmPlaceRaid(int64_t nRaidId)
{
	mem::map<int64_t,LC_CrossRealmPlaceRaidInfo>::iterator iter = m_CrossRealmPlaceRaid.find(nRaidId);
	if(iter == m_CrossRealmPlaceRaid.end())
	{
		return NULL;
	}
	return &(iter->second);
}

void LC_CrossRealmRaidManager::__SendDestoryCrossRealmRaid(int64_t raidid, int iMapResID)
{
	MG_SS_DestroyRaid_REQ req;
	req.m_RaidId = raidid;
	req.m_MapResId = iMapResID;
	SendMsgToSociety(MGPT_GGU_DESTROY_RAID_REQ, &req);
}
