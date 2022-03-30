#ifndef _LC_ClientArenaBattle_ChallengeInfoAsset_H
#define _LC_ClientArenaBattle_ChallengeInfoAsset_H

#include "MG_ProtocolStruct.h"
#include "LC_ClientGameEvent.h"

using namespace GameLogic;

class LC_ClientArenaBattleChallengeInfoAsset : public Memory::MM_BaseObject
{
public:
	DECL_RTTI_NAME( LC_ClientArenaBattleChallengeInfoAsset );
	LC_ClientArenaBattleChallengeInfoAsset();
	void Reset();

	void UpdateShortInfo(const Protocol::MG_RESP_RankInfoResult& kMsg);
	void UpdateShortInfo(const std::vector<SkyArenaBattlePlayData>& kbattlePlays);
	void UpdateDetailInfo(const Protocol::MG_RESP_GetRankPlayerBuddyInfo& kMsg);
	void UpdateDetailInfo(const Protocol::MG_RESP_GetRankBuddyInfo& kMsg);
	void UpdateDetailInfo(const Protocol::MG_RESP_GetRankPetInfo& kMsg);
	void UpdateDetailInfo(const Protocol::MG_RESP_GetRankSubInfo& kMsg);
	void UpdateDetailInfo(const Protocol::MG_GG_RESP_RankTopNPlayerInfo& kMsg);
	void UpdateDetailInfo(const Protocol::MG_RESP_GetBuddyRankIndex& kMsg);
	void UpdateDetailInfo(const Protocol::MG_RESP_GetPlayerSchemeData & kMsg);
	void UpdateDetailInfo(const Protocol::MG_NOTIFY_SkyAllShowInfo & kMsg);
	void UpdateDetailInfo(const Protocol::MG_RESP_GetSummitSingleData & kMsg);

	void UpdateDetailInfo(const Protocol::MG_Resp_SummitArenaAgainstList &kMsg); 
	void UpdateDetailInfo(const Protocol::MG_Resp_SummitArenaMyGroupStage & kMsg) ; 
	void UpdateDetailInfo(const Protocol::MG_Resp_SummitArenaGroupStageEntryList & kMsg) ; 
	void UpdateDetailInfo(const Protocol::MG_Resp_SummitArenaGroupStage & kMsg) ; 



	LC_ClientRankUserInfo* GetUserRankInfoByUserID(unique_id_type iCharID); 
	LC_ClientRankUserInfo* GetUserRankInfoByIndex(int iType, int index); 
	int64_t GetUserRankValueByIndex(int iType, int index); 
	int GetRankUserSize(int iType);

	void UpdateArenaBattleInfo(const Protocol::MG_GG_RESP_Query_User_Challenge& kMsg);
	void UpdateArenaRankListInfo(const Protocol::MG_GG_RESP_QueryChalleng_Rank& kMsg);
	LC_ClientRankUserInfo* GetArenaTopRankUserInfoByIndex(int index);
	LC_ClientRankUserInfo* GetArenaRankUserInfoByIndex(int index);
	int GetArenaUserRankIndexByIndex(int index);
	int GetArenaUserSize();

	LC_ClientRankUserInfo* GetSkyArenaEnermyInfoByIndex(int index);
	uint64_t GetSkyArenaEnermyPointByIndex(int index);
	int GetSkyArenaEnermyCount() { return m_kSkyArenaEnermyVec.size(); }

private:
	struct ShortInfo
	{
		ShortInfo()
		{
			Clear();
		}

		inline void Clear(void)
		{
			m_CharID = 0;
			m_Value = 0;
		}

		unique_id_type					m_CharID;
		int64_t                         m_Value;                    		
	};
	typedef mem::map<int, ShortInfo> RankInfoMap;

	struct ArenaShortInfo
	{
		ArenaShortInfo()
		{
			Clear();
		}

		inline void Clear(void)
		{
			index = 0;
			m_CharID = 0;
		}

		int								index;
		unique_id_type                  m_CharID;
	};

	struct SkyArenaEnermyInfo
	{
		SkyArenaEnermyInfo()
		{
			Clear();
		}

		inline void Clear(void)
		{
			m_iPoint = 0;
			m_CharID = 0;
		}

		uint64_t						m_iPoint;
		unique_id_type                  m_CharID;
	};

	mem::map<int, RankInfoMap> m_kChallangeRankInfo;	//每个榜的排名uid列表

	mem::map<unique_id_type, LC_ClientRankUserInfo> m_kUserInfoMap;

	mem::vector<unique_id_type> m_kArenaBattleTop3UsersMap;			//竞技场前三名uid列表						
	mem::vector<ArenaShortInfo> m_kArenaBattleUsersMap;  //竞技场可挑战uid列表

	mem::vector<SkyArenaEnermyInfo> m_kSkyArenaEnermyVec;  //苍穹战场可挑战列表

	static bool _sortArenaBattleUsers(ArenaShortInfo infoA, ArenaShortInfo infoB);
};

#endif