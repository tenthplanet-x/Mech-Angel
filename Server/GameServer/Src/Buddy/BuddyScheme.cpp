#include "MG_Game2Group.h"
#include "NW_Helper.h"
#include "GameServerApp.h"

#include "BuddyScheme.h"
#include "BuddyInstance.h"
#include "UT_ServerHelper.h"
#include "CF_Scheme.h"
#include "CF_GameStory.h"
#include "LC_ServerPlayer.h"
#include "LC_Helper.h"
#include "LC_Dule_Manager.h"
#include "LC_RankManager.h"
#include "LC_ServerLogicManager.h"
using namespace CSVFile;
using namespace Utility;

BuddySchemeMgr::BuddySchemeMgr() : m_nCurrentSchemeID(-1), m_nCurrentSchemeGroupID(-1), m_nBeChallengeMapID(0)
{
	m_mPlayerOffensiveSchemeDataMap.clear();
	m_mPlayerDefenceSchemeDataMap.clear();
}

BuddySchemeMgr::~BuddySchemeMgr()
{
}


void BuddySchemeMgr::Clear()
{
	m_nBeChallengeMapID = 0;
	m_nCurrentSchemeID = -1;
	m_nCurrentSchemeGroupID = -1;
	m_mPlayerOffensiveSchemeDataMap.clear();
	m_mPlayerDefenceSchemeDataMap.clear();
}

void BuddySchemeMgr::SaveData(char_data_define::activate_scheme_data* pkData)
{
	if(NULL == pkData)
	{
		return;
	}

	pkData->set_currentschemeid(m_nCurrentSchemeID);
	pkData->set_currentschemegroupid(m_nCurrentSchemeGroupID);
	for(PlayerSchemeDataMap::iterator it=m_mPlayerOffensiveSchemeDataMap.begin(); it!=m_mPlayerOffensiveSchemeDataMap.end(); ++it)
	{
		::char_data_define::single_map_activate_scheme_data* pkSchemeDataData = pkData->add_offensiveschemedata();
		if(NULL == pkSchemeDataData)
		{
			return;
		}

		pkSchemeDataData->set_schemeid(it->first);
		const SchemeData& data = it->second;
		for(int i=0; i<data.size(); ++i)
		{
			::char_data_define::single_group_activate_scheme_data* pkSchemedata = pkSchemeDataData->add_schemedata();
			if(NULL == pkSchemedata)
			{
				return;
			}

			pkSchemedata->set_schemegroupid(data[i].m_nSchemeGroupID);
			for(int j=0; j<MAX_ACTIVE_BUDDY_NUM; ++j)
			{
				pkSchemedata->add_schemedata(data[i].m_vSchemeData[j]);
			}
		}		
	}

	for(PlayerSchemeDataMap::iterator it=m_mPlayerDefenceSchemeDataMap.begin(); it!=m_mPlayerDefenceSchemeDataMap.end(); ++it)
	{
		::char_data_define::single_map_activate_scheme_data* pkSchemeDataData = pkData->add_defenceschemedata();
		if(NULL == pkSchemeDataData)
		{
			return;
		}

		pkSchemeDataData->set_schemeid(it->first);
		const SchemeData& data = it->second;
		for(int i=0; i<data.size(); ++i)
		{
			::char_data_define::single_group_activate_scheme_data* pkSchemedata = pkSchemeDataData->add_schemedata();
			if(NULL == pkSchemedata)
			{
				return;
			}

			pkSchemedata->set_schemegroupid(data[i].m_nSchemeGroupID);
			for(int j=0; j<MAX_ACTIVE_BUDDY_NUM; ++j)
			{
				pkSchemedata->add_schemedata(data[i].m_vSchemeData[j]);
			}
		}		
	}
}

void BuddySchemeMgr::LoadData(const char_data_define::activate_scheme_data& data)
{
	m_nCurrentSchemeID = data.currentschemeid();
	m_nCurrentSchemeGroupID = data.has_currentschemegroupid();
	for(int i=0; i<data.offensiveschemedata_size(); ++i)
	{
		SchemeData schemeData;
		schemeData.reserve(data.offensiveschemedata(i).schemedata_size());

		int schemeID = data.offensiveschemedata(i).schemeid();
		for(int j=0; j<data.offensiveschemedata(i).schemedata_size(); ++j)
		{
			PS_SingleSchemeData singleData;
			singleData.m_nSchemeGroupID = data.offensiveschemedata(i).schemedata(j).schemegroupid();

			if(data.offensiveschemedata(i).schemedata(j).schemedata_size() != MAX_ACTIVE_BUDDY_NUM)
			{
				GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "BuddySchemeMgr::LoadData error!");
				return;
			}
			for(int k=0; k<data.offensiveschemedata(i).schemedata(j).schemedata_size(); ++k)
			{
				singleData.m_vSchemeData[k] = data.offensiveschemedata(i).schemedata(j).schemedata(k);
			}
			schemeData.push_back(singleData);
		}
		m_mPlayerOffensiveSchemeDataMap[schemeID] = schemeData;
	}

	for(int i=0; i<data.defenceschemedata_size(); ++i)
	{
		SchemeData schemeData;
		schemeData.reserve(data.defenceschemedata(i).schemedata_size());

		int schemeID = data.defenceschemedata(i).schemeid();
		for(int j=0; j<data.defenceschemedata(i).schemedata_size(); ++j)
		{
			PS_SingleSchemeData singleData;
			singleData.m_nSchemeGroupID = data.defenceschemedata(i).schemedata(j).schemegroupid();

			if(data.defenceschemedata(i).schemedata(j).schemedata_size() != MAX_ACTIVE_BUDDY_NUM)
			{
				GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "BuddySchemeMgr::LoadData error!");
				return;
			}
			for(int k=0; k<data.defenceschemedata(i).schemedata(j).schemedata_size(); ++k)
			{
				singleData.m_vSchemeData[k] = data.defenceschemedata(i).schemedata(j).schemedata(k);
			}
			schemeData.push_back(singleData);
		}
		m_mPlayerDefenceSchemeDataMap[schemeID] = schemeData;
	}

	// 1 确认上线不会在任何存在特殊设置阵营的地图中
	// 2 这样能处理服务器崩溃等最临界的数据正确，确保上线后阵营方案正确
	if(m_nCurrentSchemeID!=0 && m_nCurrentSchemeGroupID!=1 && !CheckSchemeReady(m_nCurrentSchemeID, m_nCurrentSchemeGroupID, SchemeType_Offensive))
	{
		SetCurrentSchemeInfo(INIT_SCHEME_ID, INIT_SCHEME_GROUPID);
	}

	return;
}

void BuddySchemeMgr::SetSpecialMapInitBuddyActivateScheme_Client(LC_ServerPlayer* pkOwnerPlayer, uint32_t mapID, uint32_t schemeType)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return;
	}

	MG_RESP_SetSpecialMapInitBuddyActivateScheme rltMsg;
	rltMsg.m_nMapID = mapID;
	rltMsg.m_nSchemeType = schemeType;
	rltMsg.m_nResultType = SetMapInitScheme(pkOwnerPlayer, mapID, schemeType, rltMsg.m_vSchemeData);
	if(RE_BUDDY_SCHEME_ERROR_OK == rltMsg.m_nResultType)
	{
		pkOwnerPlayer->SaveCharPB(false);
	}
	pkOwnerPlayer->SendMsgToClient(MGPT_RESP_SETSPECIALMAP_INITBUDDYACTIVATESCHEME, &rltMsg);	
}

void BuddySchemeMgr::SetBuddyActivateScheme_Client(LC_ServerPlayer* pkOwnerPlayer, uint32_t mapID, uint32_t schemeType, const std::vector<PS_SingleSchemeData>& schemeData)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return;
	}

	MG_RESP_SetBuddyActivateScheme rltMsg;
	rltMsg.m_nMapID = mapID;
	rltMsg.m_nSchemeType = schemeType;
	rltMsg.m_vSchemeData = schemeData;

	do 
	{
		//参数检测
		if(schemeType!=SchemeType_Offensive && schemeType!=SchemeType_Defence)
		{
			rltMsg.m_nResultType = RE_FAIL;
			break;
		}

		rltMsg.m_nResultType = CheckSchemeDataVaild_Client(pkOwnerPlayer, mapID, schemeType, schemeData);
		if(RE_SUCCESS != rltMsg.m_nResultType)
		{
			break;
		}

		rltMsg.m_nResultType = SetAllSchemeDataBySchemeID(pkOwnerPlayer, mapID, schemeType, schemeData);
	} while (false);
	
	pkOwnerPlayer->SendMsgToClient(MGPT_RESP_SETBUDDYACTIVATESCHEME, &rltMsg);
}

void BuddySchemeMgr::SetBuddyActivateScheme(LC_ServerPlayer* pkOwnerPlayer, uint32_t mapID, uint32_t groupID, uint32_t schemeType, const mem::vector<int32_t>& buddyVec)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return;
	}

	if(RE_SUCCESS != CheckSchemeDataVaild(pkOwnerPlayer, mapID, groupID, schemeType, buddyVec))
	{
		return;
	}

	int32_t schemeID = LC_Helper::GetSchemeIDFromMapID(mapID);
	if(-1 == schemeID)
	{
		return;
	}

	for(int i=0; i<buddyVec.size(); ++i)
	{
		SetSchemeData(pkOwnerPlayer, schemeType, schemeID, groupID, i, buddyVec[i], true);
	}
}

ResultType BuddySchemeMgr::SetCurrentScheme(LC_ServerPlayer* pkOwnerPlayer, int32_t index, int32_t buddyID)
{
	ResultType res = CheckModifyCurrentScheme(pkOwnerPlayer, index, buddyID);
	if(RE_BUDDY_SCHEME_ERROR_OK != res)
	{
		return res;
	}

	if(index<0 || index>=MAX_ACTIVE_BUDDY_NUM)
	{
		return RE_FAIL;
	}

	SetSchemeData(pkOwnerPlayer, SchemeType_Offensive, m_nCurrentSchemeID, m_nCurrentSchemeGroupID, index, buddyID, true);
	if(INIT_SCHEME_ID==m_nCurrentSchemeID && INIT_SCHEME_GROUPID==m_nCurrentSchemeGroupID)
	{
		SetSchemeData(pkOwnerPlayer, SchemeType_Defence, m_nCurrentSchemeID, m_nCurrentSchemeGroupID, index, buddyID, true);
	}

	return RE_SUCCESS;
}

void BuddySchemeMgr::SyncBuddyActivateScheme(LC_ServerPlayer* pkOwnerPlayer)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return;
	}

	MG_SyncBuddyActivateScheme syncMsg;
	GetAllSchemeData(syncMsg.m_SchemeData);
	pkOwnerPlayer->SendMsgToClient(MGPT_SYNC_BUDDYACTIVATESACHEME, &syncMsg);
}

int32_t BuddySchemeMgr::GetBuddySchemeData(LC_ServerPlayer* pkOwnerPlayer, uint32_t mapID, uint32_t groupID, uint32_t schemeType, mem::vector<int>& buddyIDVec)
{
	buddyIDVec.clear();
	buddyIDVec.reserve(MAX_ACTIVE_BUDDY_NUM);

	if(groupID < 1 || NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return -1;
	}

	int32_t groupNumCondition = UT_ServerHelper::GetSchemeGroupNum(pkOwnerPlayer, mapID);
	if(groupID > groupNumCondition)
	{
		return -1;
	}

	int32_t schemeID = 0;
	BuddySchemeMgr::PlayerSchemeDataMap::iterator findIt;
	bool tag = GetPlayerSchemeData(mapID, schemeType, schemeID, &findIt);
	if(!tag)
	{
		return -1;
	}

	const SchemeData& data = findIt->second;
	if(data.empty())
	{
		return -1;
	}

	for(int i=0; i<data.size(); ++i)
	{
		if(data[i].m_nSchemeGroupID == groupID)
		{
			for(int j=0; j<MAX_ACTIVE_BUDDY_NUM; ++j)
			{
				int buddyID = data[i].m_vSchemeData[j];
				if(0 != buddyID)
				{
					buddyIDVec.push_back(buddyID);
				}
			}
			break;
		}
	}

	return schemeID;
}

void BuddySchemeMgr::GetMapSchemeGroupData(LC_ServerPlayer* pkOwnerPlayer, uint32_t mapID, uint32_t schemeType, mem::vector<uint32_t>& groupIDVec)
{
	groupIDVec.clear();
	groupIDVec.reserve(MAX_ACTIVE_BUDDY_GROUP_SCHEME_NUM);

	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return;
	}

	int32_t groupNumCondition = UT_ServerHelper::GetSchemeGroupNum(pkOwnerPlayer, mapID);
	if(groupNumCondition < 0)
	{
		return;
	}

	int32_t schemeID = 0;
	BuddySchemeMgr::PlayerSchemeDataMap::iterator findIt;
	bool tag = GetPlayerSchemeData(mapID, schemeType, schemeID, &findIt);
	if(!tag)
	{
		return;
	}

	const SchemeData& data = findIt->second;
	if(data.empty())
	{
		return;
	}

	for(uint32_t i=0; i<data.size()&&i<groupNumCondition; ++i)
	{
		groupIDVec.push_back(data[i].m_nSchemeGroupID);	
	}
}

void BuddySchemeMgr::GetAllSchemeData(PS_Player_SchemeData& schemeData)
{
	schemeData.m_vMapOffensiveSchemeData.clear();
	schemeData.m_vMapOffensiveSchemeData.reserve(m_mPlayerOffensiveSchemeDataMap.size());
	for(PlayerSchemeDataMap::iterator it=m_mPlayerOffensiveSchemeDataMap.begin(); it!=m_mPlayerOffensiveSchemeDataMap.end(); ++it)
	{
		PS_MapFullSchemeData data;
		data.m_nSchemeID = it->first;
		for(int i=0; i<it->second.size(); ++i)
		{
			data.m_vSchemeData.push_back(it->second[i]);
		}
		schemeData.m_vMapOffensiveSchemeData.push_back(data);
	}

	schemeData.m_vMapDefenceSchemeData.clear();
	schemeData.m_vMapDefenceSchemeData.reserve(m_mPlayerDefenceSchemeDataMap.size());
	for(PlayerSchemeDataMap::iterator it=m_mPlayerDefenceSchemeDataMap.begin(); it!=m_mPlayerDefenceSchemeDataMap.end(); ++it)
	{
		PS_MapFullSchemeData data;
		data.m_nSchemeID = it->first;
		for(int i=0; i<it->second.size(); ++i)
		{
			data.m_vSchemeData.push_back(it->second[i]);
		}
		schemeData.m_vMapDefenceSchemeData.push_back(data);
	}
}

void BuddySchemeMgr::SetRobotSchemeData(const Utility::UT_SIMDataList& groupData)
{
	if(groupData.empty())
	{
		return;
	}

	m_mPlayerDefenceSchemeDataMap.clear();
	SchemeData schemeData;
	for(int i=0; i<groupData.size(); ++i)
	{
		PS_SingleSchemeData data;
		data.m_nSchemeGroupID = groupData[i].IID();
		data.m_vSchemeData[0] = groupData[i].ParamA();
		data.m_vSchemeData[1] = groupData[i].ParamB();
		data.m_vSchemeData[2] = groupData[i].ParamC();
		schemeData.push_back(data);
	}

	SetRobotSchemeDataByMapID(SKY_SOLO_DULE_RES_ID, schemeData);
	SetRobotSchemeDataByMapID(SOLO_DULE_RES_ID, schemeData);
	SetRobotSchemeDataByMapID(SUMMIT_TEAM_SOLO_DULE_RES_ID, schemeData);
	SetRobotSchemeDataByMapID(SUMMIT_ARENA_SOLO_DULE_RES_ID, schemeData);
}

void BuddySchemeMgr::SetRobotSchemeDataByMapID(uint32_t mapID, const SchemeData& schemeData)
{
	int32_t schemeID = LC_Helper::GetSchemeIDFromMapID(mapID);
	if(-1 != schemeID)
	{
		m_mPlayerDefenceSchemeDataMap.insert(std::make_pair(schemeID, schemeData));
		m_mPlayerOffensiveSchemeDataMap.insert(std::make_pair(schemeID, schemeData));
	}
}

bool BuddySchemeMgr::CheckNeedSwitchBuddyOffensiveScheme(uint32_t mapID, uint32_t groupID)
{
	uint32_t currentSchemeID = 0;
	uint32_t currentGroupID = 0;
	GetCurrentSchemeInfo(currentSchemeID, currentGroupID);

	int32_t schemeID = LC_Helper::GetSchemeIDFromMapID(mapID);
	if(-1 == schemeID)
	{
		return false;
	}

	if(schemeID==currentSchemeID && groupID==currentGroupID)
	{
		return false;
	}

	return true;
}

bool BuddySchemeMgr::CheckSchemeReady(uint32_t mapID, uint32_t groupID, uint32_t schemeType)
{
	int32_t schemeID = 0;
	BuddySchemeMgr::PlayerSchemeDataMap::iterator findIt;
	bool tag = GetPlayerSchemeData(mapID, schemeType, schemeID, &findIt);
	if(!tag)
	{
		return -1;
	}

	const SchemeData& data = findIt->second;
	if(data.empty())
	{
		return false;
	}

	for(int i=0; i<data.size(); ++i)
	{
		if(data[i].m_nSchemeGroupID == groupID)
		{
			//Q不能为NULL
			if(INVALID_ACTOR_ID == data[i].m_vSchemeData[0])
			{
				return false;
			}

			return true;
		}
	}

	return false;
}

ResultType BuddySchemeMgr::CheckModifyCurrentScheme(LC_ServerPlayer* pkOwnerPlayer, int32_t index, int32_t buddyID)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return RE_FAIL;
	}

	if(index<0 || index>=MAX_ACTIVE_BUDDY_NUM)
	{
		return RE_FAIL;
	}

	PlayerSchemeDataMap::iterator findIt = m_mPlayerOffensiveSchemeDataMap.find(m_nCurrentSchemeID);
	if (findIt == m_mPlayerOffensiveSchemeDataMap.end())
	{
		return RE_BUDDY_SCHEME_ERROR_PARAM;
	}

	if(0 == buddyID)
	{
		return RE_BUDDY_SCHEME_ERROR_OK;
	}

	BuddyInstance* pkBuddy = pkOwnerPlayer->GetBuddyInstanceByID(buddyID);
	if(NULL == pkBuddy)
	{
		return RE_BUDDY_SCHEME_ERROR_NO_INSTANCE;
	}

	mem::set<uint32_t> buddyIDVec;
	buddyIDVec.insert(buddyID);

	const SchemeData& data = findIt->second;
	for(int i=0; i<data.size(); ++i)
	{
		if(data[i].m_nSchemeGroupID == 1)
		{
			for(int j=0; j<MAX_ACTIVE_BUDDY_NUM; ++j)
			{
				int cid = data[i].m_vSchemeData[j];
				if(0 != cid && !buddyIDVec.insert(cid).second)
				{
					return RE_BUDDY_SCHEME_ERROR_EXIST;
				}		
			}
			break;
		}
	}

	return RE_BUDDY_SCHEME_ERROR_OK;
}

void BuddySchemeMgr::GetCurrentSchemeInfo(uint32_t& schemeID, uint32_t& groupID)
{
	schemeID = m_nCurrentSchemeID;
	groupID = m_nCurrentSchemeGroupID;
}

void BuddySchemeMgr::SetCurrentSchemeInfo(uint32_t schemeID, uint32_t groupID)
{
	m_nCurrentSchemeID = schemeID;
	m_nCurrentSchemeGroupID = groupID;
}

void BuddySchemeMgr::SetBeChallangeMapID(uint32_t mapID)
{
	m_nBeChallengeMapID = mapID;
}

uint32_t BuddySchemeMgr::GetBeChallangeMapID()
{
	return m_nBeChallengeMapID;
}

ResultType BuddySchemeMgr::SetMapInitScheme(LC_ServerPlayer* pkOwnerPlayer, uint32_t mapID, uint32_t schemeType, std::vector<PS_SingleSchemeData>& schemeData)
{
	schemeData.clear();
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return RE_FAIL;
	}

	//参数检测
	if(schemeType != SchemeType_Defence)
	{
		return RE_ERROR_PARAM;
	}

	//是否是特殊阵容
	bool tag = LC_Helper::IsSchemeSpecialMap(mapID);
	if(!tag)
	{
		return RE_ERROR_PARAM;
	}

	CF_WorldMapList::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(mapID);
	if(NULL == pkData)
	{
		return RE_ERROR_PARAM;
	}

	//最大设置的阵容数量
	int32_t groupNumCondition = UT_ServerHelper::GetSchemeGroupNum(pkOwnerPlayer, mapID);
	if(groupNumCondition <1)
	{
		return RE_ERROR_PARAM;
	}

	int32_t mapSchemeID = LC_Helper::GetSchemeIDFromMapID(mapID);
	if(-1 == mapSchemeID)
	{
		return RE_FAIL;
	}

	PlayerSchemeDataMap& schemeDataMap = (schemeType==SchemeType_Offensive) ? m_mPlayerOffensiveSchemeDataMap : m_mPlayerDefenceSchemeDataMap;
	PlayerSchemeDataMap::iterator findIt = schemeDataMap.find(mapSchemeID);
	if(findIt != schemeDataMap.end())
	{
		return RE_BUDDY_SCHEME_ERROR_2;
	}

	int32_t setSchemeID = 0;
	BuddySchemeMgr::PlayerSchemeDataMap::iterator kIt;
	bool flag = GetPlayerSchemeData(mapID, schemeType, setSchemeID, &kIt);
	if(!flag)
	{
		return RE_FAIL;
	}

	const SchemeData& data = kIt->second;
	if(data.empty())
	{
		return RE_FAIL;
	}

	schemeData.reserve(groupNumCondition);
	for(int i=0; i<data.size()&&i<groupNumCondition; ++i)
	{
		schemeData.push_back(data[i]);
	}

	return SetAllSchemeDataBySchemeID(pkOwnerPlayer, mapID, schemeType, schemeData);
}

ResultType BuddySchemeMgr::SetAllSchemeDataBySchemeID(LC_ServerPlayer* pkOwnerPlayer, uint32_t mapID, uint32_t schemeType, const std::vector<PS_SingleSchemeData>& schemeData)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return RE_FAIL;
	}

	int32_t schemeID = LC_Helper::GetSchemeIDFromMapID(mapID);
	if(-1 == schemeID)
	{
		return RE_FAIL;
	}

	for(int i=0; i<schemeData.size(); ++i)
	{
		for(int j=0; j<MAX_ACTIVE_BUDDY_NUM; ++j)
		{
			SetSchemeData(pkOwnerPlayer, schemeType, schemeID, schemeData[i].m_nSchemeGroupID, j, schemeData[i].m_vSchemeData[j], false);
		}
	}

	UpdateSchemeDataToGroup(pkOwnerPlayer, schemeID, schemeType);
	pkOwnerPlayer->SaveCharPB(false);

	return RE_BUDDY_SCHEME_ERROR_OK;
}

void BuddySchemeMgr::SetSchemeData(LC_ServerPlayer* pkOwnerPlayer, uint32_t schemeType, uint32_t schemeID, uint32_t groupID, uint32_t index, int32_t buddyID, bool updateToGroup)
{
	PlayerSchemeDataMap& schemeDataMap = (schemeType==SchemeType_Offensive) ? m_mPlayerOffensiveSchemeDataMap : m_mPlayerDefenceSchemeDataMap;
	PlayerSchemeDataMap::iterator findIt = schemeDataMap.find(schemeID);
	if(findIt == schemeDataMap.end())
	{
		SchemeData data;
		schemeDataMap[schemeID] = data;
		findIt = schemeDataMap.find(schemeID);
	}

	bool tag = false;
	SchemeData& data = findIt->second;
	for(int i=0; i<data.size(); ++i)
	{
		if(data[i].m_nSchemeGroupID == groupID)
		{
			tag = true;
			data[i].m_vSchemeData[index] = buddyID;
			break;
		}
	}

	if(!tag)
	{
		PS_SingleSchemeData info;
		info.m_nSchemeGroupID = groupID;
		info.m_vSchemeData[index] = buddyID;
		data.push_back(info);
	}

	if(updateToGroup)
	{
		UpdateSchemeDataToGroup(pkOwnerPlayer, schemeID, schemeType);
		pkOwnerPlayer->SaveCharPB(false);
	}
}

ResultType BuddySchemeMgr::CheckSchemeDataVaild_Client(LC_ServerPlayer* pkOwnerPlayer, uint32_t mapID, uint32_t schemeType, const std::vector<PS_SingleSchemeData>& schemeData)
{
	if(schemeType==SchemeType_Defence && UT_ServerHelper::IsSummitArenaMap(mapID))
	{
		int32_t currentTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();
		SummitArenaTimeData timeData;
		LC_RankManager* pkRankManager = SERVER_GET_RANK_MANAGER();
		pkRankManager->GetSummitArenaOpenTime(timeData);
		//小组赛阶段，设置防守阵容，不检测时间
		if(GroupStageStartType!=timeData.nStageTimeType && currentTime>=timeData.nStageSetEndTime && currentTime<=timeData.nStageEndTime)
		{
			return RE_BUDDY_SCHEME_ERROR_TIMEOUT;
		}
	}

	int32_t schemeID = LC_Helper::GetSchemeIDFromMapID(mapID);
	if(schemeID < 1)
	{
		return RE_FAIL;
	}

	//申请设置的队伍数量是否符合要求的队伍数量
	int32_t groupNumCondition = UT_ServerHelper::GetSchemeGroupNum(pkOwnerPlayer, mapID);
	if(groupNumCondition==-1 || schemeData.size() > groupNumCondition)
	{
		return RE_BUDDY_SCHEME_ERROR_ERROR_1;
	}

	CF_WorldMapList::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(mapID);
	if(pkData == NULL)
	{
		return RE_FAIL;
	}

	CF_GameStory::DataEntry* pkGameStoryData = NULL;
	if(pkData->_iStoryID != 0)
	{
		pkGameStoryData = SafeGetCSVFileDataEntryPtr<CF_GameStory>(pkData->_iStoryID);
		if(NULL == pkGameStoryData)
		{
			return RE_FAIL;
		}
	}

	//队伍是否重复  队伍中的buddy是否重复
	mem::set<uint32_t> groupIDSet;
	mem::set<int32_t> buddyCIDSet;
	for(int i=0; i<schemeData.size(); ++i)
	{
		uint32_t groupID = schemeData[i].m_nSchemeGroupID;
		if(groupID > MAX_ACTIVE_BUDDY_GROUP_SCHEME_NUM)
		{
			return RE_FAIL;
		}

		//队id重复
		if(!groupIDSet.insert(groupID).second)
		{
			return RE_FAIL;
		}

		//Q不能未空
		if (schemeData[i].m_vSchemeData[0]==INVALID_ACTOR_ID && (schemeData[i].m_vSchemeData[1]!=INVALID_ACTOR_ID || schemeData[i].m_vSchemeData[2]!=INVALID_ACTOR_ID) )
		{
			return RE_BUDDY_SCHEME_ERROR_Q_INVALID;
		}

		for(int j=0; j<MAX_ACTIVE_BUDDY_NUM; ++j)
		{
			uint32_t buddyID = schemeData[i].m_vSchemeData[j];
			if(0 != buddyID)
			{
				BuddyInstance* pkBuddy = pkOwnerPlayer->GetBuddyInstanceByID(buddyID);
				if(NULL == pkBuddy)
				{
					return RE_BUDDY_SCHEME_ERROR_NO_INSTANCE;
				}

				if(NULL!=pkGameStoryData && pkGameStoryData->_ielementdamagetype!=0 && pkBuddy->GetDamageElementType()!=pkGameStoryData->_ielementdamagetype)
				{
					return RE_BUDDY_SHCEME_ERROR_ELEMTYPE_ERROR;
				}

				//buddyID设置重复
				if(!buddyCIDSet.insert(buddyID).second)
				{
					return RE_FAIL;
				}
			}
		}
	}
	return RE_SUCCESS;
}

ResultType BuddySchemeMgr::CheckSchemeDataVaild(LC_ServerPlayer* pkOwnerPlayer, uint32_t mapID, uint32_t groupID, uint32_t schemeType, const mem::vector<int32_t>& buddyVec)
{
	int32_t schemeID = LC_Helper::GetSchemeIDFromMapID(mapID);
	if(-1 == schemeID)
	{
		return RE_FAIL;
	}

	CF_Scheme::DataEntry* pkSchemeData = SafeGetCSVFileDataEntryPtr<CF_Scheme>(schemeID);
	if(NULL == pkSchemeData)
	{
		return RE_FAIL;
	}

	PlayerSchemeDataMap& schemeDataMap = (schemeType==SchemeType_Offensive) ? m_mPlayerOffensiveSchemeDataMap : m_mPlayerDefenceSchemeDataMap;
	PlayerSchemeDataMap::iterator findIt = schemeDataMap.find(schemeID);
	if(findIt == schemeDataMap.end())
	{
		return RE_SUCCESS;
	}

	if(buddyVec.size() != MAX_ACTIVE_BUDDY_NUM)
	{
		return RE_BUDDY_SCHEME_ERROR_PARAM;
	}

	//Q不能未空
	if (buddyVec[0]==INVALID_ACTOR_ID && (buddyVec[1]!=INVALID_ACTOR_ID || buddyVec[2]!=INVALID_ACTOR_ID) )
	{
		return RE_BUDDY_SCHEME_ERROR_Q_INVALID;
	}

	std::set<int32_t> buddyCIDSet;
	for(int j=0; j<buddyVec.size(); ++j)
	{
		uint32_t buddyID = buddyVec[j];
		if(0 != buddyID)
		{
			BuddyInstance* pkBuddy = pkOwnerPlayer->GetBuddyInstanceByID(buddyID);
			if(NULL == pkBuddy)
			{
				return RE_BUDDY_SCHEME_ERROR_NO_INSTANCE;
			}

			//buddyID设置重复
			if(!buddyCIDSet.insert(buddyID).second)
			{
				return RE_FAIL;
			}
		}
	}

	const SchemeData& data = findIt->second;
	for(int i=0; i<data.size(); ++i)
	{
		if(data[i].m_nSchemeGroupID == groupID)
		{
			for(int j=0; j<MAX_ACTIVE_BUDDY_NUM; ++j)
			{
				//buddyID设置重复
				int cid = data[i].m_vSchemeData[j];
				if(0 != cid && !buddyCIDSet.insert(cid).second)
				{
					return RE_FAIL;
				}
			}
			break;
		}
	}

	return RE_SUCCESS;
}

bool BuddySchemeMgr::GetPlayerSchemeData(uint32_t mapID, uint32_t schemeType, int32_t& schemeID, BuddySchemeMgr::PlayerSchemeDataMap::iterator* pkFindIter)
{
	if(NULL == pkFindIter)
	{
		return false;
	}

	schemeID = LC_Helper::GetSchemeIDFromMapID(mapID);
	if(-1 == schemeID)
	{
		return false;
	}

	CF_Scheme::DataEntry* pkSchemeData = SafeGetCSVFileDataEntryPtr<CF_Scheme>(schemeID);
	if(NULL == pkSchemeData)
	{
		return false;
	}

	PlayerSchemeDataMap& schemeDataMap = (schemeType==SchemeType_Offensive) ? m_mPlayerOffensiveSchemeDataMap : m_mPlayerDefenceSchemeDataMap;
	PlayerSchemeDataMap::iterator findIt = schemeDataMap.end();
	do
	{
		findIt = schemeDataMap.find(schemeID);
		if(findIt == schemeDataMap.end())
		{
			schemeID = pkSchemeData->_schemeLinkID;
			findIt = schemeDataMap.find(schemeID);
			pkSchemeData = SafeGetCSVFileDataEntryPtr<CF_Scheme>(schemeID);
		}

		if(findIt!=schemeDataMap.end() || pkSchemeData==NULL || pkSchemeData->_schemeLinkID==-1)
		{
			break;
		}
	}while(true);

	if(findIt != schemeDataMap.end())
	{
		*pkFindIter = findIt;
		return true;
	}

	return false;
}

void BuddySchemeMgr::UpdateSchemeDataToGroup(LC_ServerPlayer* pkOwnerPlayer, uint32_t schemeID, uint32_t schemeType)
{
	if(NULL==pkOwnerPlayer || !pkOwnerPlayer->IsMainController())
	{
		return;
	}

	PlayerSchemeDataMap& schemeDataMap = (schemeType==SchemeType_Offensive) ? m_mPlayerOffensiveSchemeDataMap : m_mPlayerDefenceSchemeDataMap;
	PlayerSchemeDataMap::iterator findIt = schemeDataMap.find(schemeID);
	if(findIt == schemeDataMap.end() || 0==findIt->second.size())
	{
		return;
	}

	MG_GG_REQ_UpdateScheme msg;
	msg.m_ServerID = GetGameServerEntryID();
	msg.m_SchemeType = schemeType;
	msg.m_SchemeData.m_nSchemeID = schemeID;
	msg.m_SchemeData.m_vSchemeData.reserve(findIt->second.size());
	for(int i=0; i<findIt->second.size(); ++i)
	{
		msg.m_SchemeData.m_vSchemeData.push_back(findIt->second[i]);
	}

	SendMsgToGroup(MGPT_SS_UPDATE_SCHEMEDATA, &msg, pkOwnerPlayer->GetUserID(), pkOwnerPlayer->GetInstance());
}