#include "ClientPCH.h"
#include "LC_ClientArenaBattleChallengeInfoAsset.h"
#include "LC_ClientLogicManager.h"

LC_ClientArenaBattleChallengeInfoAsset::LC_ClientArenaBattleChallengeInfoAsset()
{
	Reset();
}

void LC_ClientArenaBattleChallengeInfoAsset::Reset()
{
	m_kChallangeRankInfo.clear();
	m_kUserInfoMap.clear();
	m_kArenaBattleTop3UsersMap.clear();
	m_kArenaBattleUsersMap.clear();
	m_kSkyArenaEnermyVec.clear();
}

void LC_ClientArenaBattleChallengeInfoAsset::UpdateShortInfo(const Protocol::MG_RESP_RankInfoResult& kMsg)
{
	RankInfoMap& infoMap = m_kChallangeRankInfo[kMsg.m_RankID + kMsg.m_nGroupId*1000];
	infoMap.clear();
	for (int i = 0; i < kMsg.m_RankPlayerInfo.size(); ++i)
	{
		const PS_Rank_SinglePlayerInfo& pkPlayerInfo = kMsg.m_RankPlayerInfo[i];
		unique_id_type UID = pkPlayerInfo.m_PlayerData.m_CharID;
		
		infoMap[pkPlayerInfo.m_RankIndex].m_CharID = UID;
		infoMap[pkPlayerInfo.m_RankIndex].m_Value = pkPlayerInfo.m_RankValue;

		m_kUserInfoMap[UID].SetUserInfo(pkPlayerInfo.m_PlayerData);
	}
}

void LC_ClientArenaBattleChallengeInfoAsset::UpdateShortInfo(const std::vector<SkyArenaBattlePlayData>& kbattlePlays)
{
	m_kSkyArenaEnermyVec.clear();

	for (int i = 0; i < kbattlePlays.size(); ++i)
	{
		const SkyArenaBattlePlayData& battlePlayInfo = kbattlePlays[i];
		const PS_Rank_SinglePlayerInfo& pkPlayerInfo = battlePlayInfo.m_nPlayerBaseData;
		unique_id_type UID = pkPlayerInfo.m_PlayerData.m_CharID;

		SkyArenaEnermyInfo info;
		info.m_CharID = UID;
		info.m_iPoint = pkPlayerInfo.m_RankValue;

		m_kSkyArenaEnermyVec.push_back(info);

		LC_ClientRankUserInfo& rankUserInfo = m_kUserInfoMap[UID];
		rankUserInfo.SetUserInfo(battlePlayInfo.m_SchemeData);
		rankUserInfo.SetUserInfo(pkPlayerInfo.m_PlayerData);
	}
}

void LC_ClientArenaBattleChallengeInfoAsset::UpdateDetailInfo(const Protocol::MG_RESP_GetRankPlayerBuddyInfo& kMsg)
{
	m_kUserInfoMap[kMsg.m_CharID].SetUserInfo(kMsg.m_buddyData);
}

void LC_ClientArenaBattleChallengeInfoAsset::UpdateDetailInfo(const Protocol::MG_RESP_GetRankBuddyInfo& kMsg)
{
	m_kUserInfoMap[kMsg.m_CharID].SetUserInfo(kMsg.m_buddyData);
}

void LC_ClientArenaBattleChallengeInfoAsset::UpdateDetailInfo(const Protocol::MG_RESP_GetRankPetInfo& kMsg)
{
	m_kUserInfoMap[kMsg.m_CharID].SetUserInfo(kMsg.m_PetData);
}

void LC_ClientArenaBattleChallengeInfoAsset::UpdateDetailInfo(const Protocol::MG_RESP_GetRankSubInfo& kMsg)
{
	m_kUserInfoMap[kMsg.m_CharID].SetUserInfo(kMsg.m_SubData, kMsg.m_SubRankType);

}



void LC_ClientArenaBattleChallengeInfoAsset::UpdateDetailInfo(const Protocol::MG_Resp_SummitArenaAgainstList& kMsg)
{
	const std::vector<SummitArenaAgainstList>&  data =kMsg.m_AgainstLists ; 
	for (int i=0;i<data.size();++i)
	{
		const std::vector<PS_Rank_PlayerBaseData>& pData=data[i].m_basedatas ;
		for (int j=0;j<pData.size();++j)
		{
		const PS_Rank_PlayerBaseData & playerData=pData[j]; 
			m_kUserInfoMap[playerData.m_CharID].SetUserInfo(playerData) ;
		}
	}
}


void LC_ClientArenaBattleChallengeInfoAsset::UpdateDetailInfo(const Protocol::MG_Resp_SummitArenaMyGroupStage & kMsg) {
	const std::vector<SummitArenaGroupStageNeedBuddyData> & groupDatas=kMsg.groupDatas ; 
for (int i=0; i<groupDatas.size();++i)
{
	const SummitArenaGroupStageNeedBuddyData & data=groupDatas[i] ;
	const PS_Rank_PlayerBaseData & pData=data.groupdata.m_basedata ; 
	uint64_t charId=pData.m_CharID ; 

	LC_ClientRankUserInfo& rankUserInfo = m_kUserInfoMap[charId];
	rankUserInfo.SetUserInfo(data.m_SchemeData);
	rankUserInfo.SetUserInfo(pData);

}

}

void LC_ClientArenaBattleChallengeInfoAsset::UpdateDetailInfo(const Protocol::MG_Resp_SummitArenaGroupStageEntryList & kMsg) {

	const SummitArenaGroupStageData & data=kMsg.m_EntryList ;   
	const std::vector<SummitArenaGroupStageUserData>& allData=data.groupdata ; 
	for (int i=0;i<allData.size();++i)
	{
		const PS_Rank_PlayerBaseData & pData=allData[i].m_basedata ;  
		m_kUserInfoMap[pData.m_CharID].SetUserInfo(pData) ; 
	}

}
void LC_ClientArenaBattleChallengeInfoAsset::UpdateDetailInfo(const Protocol::MG_Resp_SummitArenaGroupStage & kMsg) {

	const std::vector<SummitArenaGroupStageData> & groupData=  kMsg.groupDatas ; 
	for (int i=0 ; i<groupData.size();++i)
	{
		const std::vector<SummitArenaGroupStageUserData>& data=groupData[i].groupdata ; 
		for (int j=0;j<data.size();++j)
		{
			const PS_Rank_PlayerBaseData & pData=data[j].m_basedata ; 
			m_kUserInfoMap[pData.m_CharID].SetUserInfo(pData) ; 
		}
	}

}




void LC_ClientArenaBattleChallengeInfoAsset::UpdateDetailInfo(const Protocol::MG_GG_RESP_RankTopNPlayerInfo& kMsg)
{
	for (int i = 0; i < kMsg.m_RankTopNInfo.size(); ++ i)
	{
		PS_RankTopNInfo pkInfo = kMsg.m_RankTopNInfo[i];
		for (int j = 0; j < pkInfo.m_PlayerInfo.size(); ++j)
		{
			PS_RankTopNPlayerSingleInfo pkPlayerInfo = pkInfo.m_PlayerInfo[j];
			unique_id_type UID = pkPlayerInfo.m_PlayerData.m_CharID;
			RankInfoMap& infoMap = m_kChallangeRankInfo[pkInfo.m_RankType];
			infoMap[j].m_CharID = UID;

			m_kUserInfoMap[UID].SetUserInfo(pkPlayerInfo);
		}
	}
}

void LC_ClientArenaBattleChallengeInfoAsset::UpdateDetailInfo(const Protocol::MG_RESP_GetBuddyRankIndex& kMsg)
{
	m_kUserInfoMap[kMsg.m_CharID].SetUserInfo(kMsg.m_bBuddyRankIndex);
}

void LC_ClientArenaBattleChallengeInfoAsset::UpdateDetailInfo(const Protocol::MG_RESP_GetPlayerSchemeData& kMsg)
{
	m_kUserInfoMap[kMsg.m_nCharID].SetUserInfo(kMsg.m_nPlayerBaseData);
	m_kUserInfoMap[kMsg.m_nCharID].SetUserInfo(kMsg.m_SchemeData);
}

void LC_ClientArenaBattleChallengeInfoAsset::UpdateDetailInfo(const Protocol::MG_NOTIFY_SkyAllShowInfo & kMsg)
{
	unique_id_type UID = kMsg.m_Left.ControllerBase.m_CharID;
	m_kUserInfoMap[UID].SetUserInfo(kMsg.m_Left.ControllerBase);
	m_kUserInfoMap[UID].SetUserInfo(kMsg.m_Left.m_nAllTeamData);

	UID = kMsg.m_Right.ControllerBase.m_CharID;
	m_kUserInfoMap[UID].SetUserInfo(kMsg.m_Right.ControllerBase);
	m_kUserInfoMap[UID].SetUserInfo(kMsg.m_Right.m_nAllTeamData);
}

void LC_ClientArenaBattleChallengeInfoAsset::UpdateDetailInfo(const Protocol::MG_RESP_GetSummitSingleData & kMsg)
{
	m_kUserInfoMap[kMsg.m_nBattleInfo.m_PlayerData[0].m_CharID].SetUserInfo(kMsg.m_nBattleInfo.m_PlayerData[0]);
	m_kUserInfoMap[kMsg.m_nBattleInfo.m_PlayerData[1].m_CharID].SetUserInfo(kMsg.m_nBattleInfo.m_PlayerData[1]);
	
}

LC_ClientRankUserInfo* LC_ClientArenaBattleChallengeInfoAsset::GetUserRankInfoByUserID(unique_id_type iCharID)
{
	mem::map<unique_id_type, LC_ClientRankUserInfo>::iterator it = m_kUserInfoMap.find(iCharID);
	if (it != m_kUserInfoMap.end())
		return &(it->second);
	
	return NULL;
}

LC_ClientRankUserInfo* LC_ClientArenaBattleChallengeInfoAsset::GetUserRankInfoByIndex(int iType, int index)
{
	mem::map<int, RankInfoMap>::iterator itor = m_kChallangeRankInfo.find(iType);
	if (itor != m_kChallangeRankInfo.end())
	{
		RankInfoMap::iterator iter = itor->second.find(index);
		if (iter != itor->second.end())
		{
			return GetUserRankInfoByUserID(iter->second.m_CharID);
		}
	}

	return NULL;
}

int64_t LC_ClientArenaBattleChallengeInfoAsset::GetUserRankValueByIndex(int iType, int index)
{
	mem::map<int, RankInfoMap>::iterator itor = m_kChallangeRankInfo.find(iType);
	if (itor != m_kChallangeRankInfo.end())
	{
		RankInfoMap::iterator iter = itor->second.find(index);
		if (iter != itor->second.end())
		{
			return iter->second.m_Value;
		}
	}

	return 0;
}

int LC_ClientArenaBattleChallengeInfoAsset::GetRankUserSize(int iType)
{
	int index = 0;
	mem::map<int, RankInfoMap>::iterator itor = m_kChallangeRankInfo.find(iType);
	if (itor != m_kChallangeRankInfo.end())
	{
		RankInfoMap::iterator iter = itor->second.begin();
		for (; iter != itor->second.end(); ++iter)
			index++;
	}

	return index;
}

void LC_ClientArenaBattleChallengeInfoAsset::UpdateArenaBattleInfo(const Protocol::MG_GG_RESP_Query_User_Challenge& kMsg)
{
	m_kArenaBattleTop3UsersMap.clear();
	for (int i = 0; i < 3; ++i)
	{
		PS_Rank_SoloDule_User_Info info = kMsg.m_Top3UserInfos[i];
		unique_id_type mCharID = info.m_nPlayerBaseData.m_CharID;
		if (mCharID != 0)
		{
			m_kArenaBattleTop3UsersMap.push_back(mCharID);

			m_kUserInfoMap[mCharID].SetUserInfo(info.m_nPlayerBaseData);
			m_kUserInfoMap[mCharID].SetUserInfo(info.m_nBuddyData);
		}
	}

	m_kArenaBattleUsersMap.clear();
	for (int i = 0; i < kMsg.m_Size; ++i)
	{
		PS_Rank_SoloDule_User_Info info = kMsg.m_UserInfos[i];
		int iRank = info.m_Index;
		unique_id_type UID = info.m_nPlayerBaseData.m_CharID;
		ArenaShortInfo shortInfo;
		shortInfo.index = iRank;
		shortInfo.m_CharID = UID;
		m_kArenaBattleUsersMap.push_back(shortInfo);

		m_kUserInfoMap[UID].SetUserInfo(info.m_nPlayerBaseData);
		m_kUserInfoMap[UID].SetUserInfo(info.m_nBuddyData);
	}
	std::sort(m_kArenaBattleUsersMap.begin(), m_kArenaBattleUsersMap.end(), _sortArenaBattleUsers);
}

void LC_ClientArenaBattleChallengeInfoAsset::UpdateArenaRankListInfo(const Protocol::MG_GG_RESP_QueryChalleng_Rank& kMsg)
{
	for (int i = 0; i < kMsg.m_RankPlayerInfo.size(); ++i)
	{
		PS_Rank_SoloDule_User_Info pkPlayerInfo = kMsg.m_RankPlayerInfo[i];
		unique_id_type UID = pkPlayerInfo.m_nPlayerBaseData.m_CharID;
		RankInfoMap& infoMap = m_kChallangeRankInfo[-1];
		infoMap[pkPlayerInfo.m_Index].m_CharID = UID;
		infoMap[pkPlayerInfo.m_Index].m_Value = 0;

		m_kUserInfoMap[UID].SetUserInfo(pkPlayerInfo.m_nPlayerBaseData);
		m_kUserInfoMap[UID].SetUserInfo(pkPlayerInfo.m_nBuddyData);
	}
}

LC_ClientRankUserInfo* LC_ClientArenaBattleChallengeInfoAsset::GetArenaTopRankUserInfoByIndex(int index)
{
	if (index < m_kArenaBattleTop3UsersMap.size())
	{
		return GetUserRankInfoByUserID(m_kArenaBattleTop3UsersMap[index]);
	}
	return NULL;
}

LC_ClientRankUserInfo* LC_ClientArenaBattleChallengeInfoAsset::GetArenaRankUserInfoByIndex(int index)
{
	if (index < m_kArenaBattleUsersMap.size())
	{
		return GetUserRankInfoByUserID(m_kArenaBattleUsersMap[index].m_CharID);
	}
	return NULL;
}

int LC_ClientArenaBattleChallengeInfoAsset::GetArenaUserRankIndexByIndex(int index)
{
	if (index < m_kArenaBattleUsersMap.size())
	{
		return m_kArenaBattleUsersMap[index].index;
	}
	return -1;
}

int LC_ClientArenaBattleChallengeInfoAsset::GetArenaUserSize()
{
	return m_kArenaBattleUsersMap.size();
}

bool LC_ClientArenaBattleChallengeInfoAsset::_sortArenaBattleUsers(ArenaShortInfo infoA, ArenaShortInfo infoB)
{
	return infoA.index < infoB.index;
}


LC_ClientRankUserInfo* LC_ClientArenaBattleChallengeInfoAsset::GetSkyArenaEnermyInfoByIndex(int index)
{
	if (index < m_kSkyArenaEnermyVec.size())
	{
		return GetUserRankInfoByUserID(m_kSkyArenaEnermyVec[index].m_CharID);
	}
	return NULL;
}

uint64_t LC_ClientArenaBattleChallengeInfoAsset::GetSkyArenaEnermyPointByIndex(int index)
{
	if (index < m_kSkyArenaEnermyVec.size())
	{
		return m_kSkyArenaEnermyVec[index].m_iPoint;
	}
	return 0;
}