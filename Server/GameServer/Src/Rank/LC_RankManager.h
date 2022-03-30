#ifndef _LC_RANK_MANAGER_H
#define _LC_RANK_MANAGER_H

#include "MG_ArenaProtocol.h"
#include "MM_BaseObject.h"
#include "MM_MemoryAllocatorConfig.h"
#include "MG_Game2Group.h"
#include "UT_ServerHelper.h"
#include <map>

template<class KEY, class T> 
class rankMap:public mem::multimap<KEY, T, std::greater<KEY> >{};

typedef rankMap<int64_t, unique_id_type> PS_Rank_Member_InfoSortMap;
typedef PS_Rank_Member_InfoSortMap::iterator rankSortMapIt;
//////////////////////////////////////////////////////////////////////////////
class LC_ServerMap;
struct FlowerGainWeight
{
	uint32_t	m_nID;
	uint32_t	m_nWeight;
};

typedef mem::vector<uint32_t>	LC_FlowerGiftList;

class LC_FlowerGiftGain
{
public:
	LC_FlowerGiftGain();
	~LC_FlowerGiftGain();

	LC_FlowerGiftList	GainGiftID();
	void				AddGainWeight(uint32_t nGainType, uint32_t nWeight, uint32_t nID);
private:
	uint32_t						m_nGainType;
	uint32_t						m_nTotalWeight;
	mem::vector<FlowerGainWeight>	m_gainWeightArr;
};
typedef mem::map<uint32_t, LC_FlowerGiftGain> LC_FlowerGiftGainMap;
///////////////////////////////////////////////////////////////////////////
struct FlowerTitleLevel
{
	uint32_t	m_nID;
	uint32_t	m_nValue;

	bool operator < (const FlowerTitleLevel &m)const 
	{
		return m_nValue < m.m_nValue;
	}
};
typedef mem::map<uint32_t, mem::vector<FlowerTitleLevel> > LC_FlowerTitleLvlMap;
struct	FlowerTitleCondition
{
	LC_FlowerTitleLvlMap			m_accumulateTitleMap;
	mem::map<uint32_t, uint32_t>	m_rankTitleMap;
};
///////////////////////////////////////////////////////////////////////////
class LC_RankManager: public Memory::MM_BaseObject
{
public:
	LC_RankManager();
	virtual ~LC_RankManager();
	DECL_RTTI_NAME(LC_RankManager)
		bool		init();
	void		fin();

	void		Update(float fCurrentTime);

	LC_ServerPlayer* GetPlayer(const unique_id_impl& charID);

	void				NoticeUserDataToGroup(LC_ServerPlayer* pkPlayer);
	void				FillPlayerRankValue(LC_ServerPlayer* pkOwnerPlayer, std::vector<uint64_t>& rankValue);
	void				FillPlayerData(LC_ServerPlayer* pkOwnerPlayer, PS_Rank_User_Info& data);
	bool				FillRobotData(uint32_t robotID, Protocol::PS_Rank_Robot_Info_New& data);
	
	ResultType	QueryRankInfo(LC_ServerPlayer* pkPlayer, Protocol::MG_REQ_RankInfo& ReqMsg);

	ResultType QueryChallengRankInfo(LC_ServerPlayer* pkPlayer, int begin, int end, int nServerID);

	ResultType	QueryUserChallenge(LC_ServerPlayer* player);
	void		QueryUserChallengeResponse(const unique_id_impl& charID, Protocol::MG_GG_RESP_Query_User_Challenge& resp);

	void		Add2CrossRealmRank(LC_ServerPlayer* player, int rankId);

	void		QueryBaseInfoResponse(const unique_id_impl& charID, Protocol::MG_GG_RESP_Query_User_Base_Rank& resp);

	ResultType	QueryBattleInfo(LC_ServerPlayer* player, int rankID);
	void		QueryBattleResponse(const unique_id_impl& charID, Protocol::MG_GG_RESP_Query_User_Battle& resp);

	ResultType	SendGroupChallengeReq(LC_ServerPlayer* player, const unique_id_impl& bechallengeCharID, int bechallengeindex);
	void		ChallengeResponse(const unique_id_impl& charID, Protocol::MG_GG_RESP_Challenge& resp);

	ResultType	SendGroupChallengeSummitTeamReq(LC_ServerPlayer* player, const unique_id_impl& bechallengeCharID,int eType);
	void		SummitTeamChallengeResponse(const unique_id_impl& charID, Protocol::MG_Resp_G2G_SummitArenaCheckChallenge& resp);
	ResultType	ChallengeReq_GuildOffline(LC_ServerPlayer* player, const unique_id_impl& bechallengeCharID, int bechallengeindex);
	ResultType	ChallengeResponse_GuildOffline(const unique_id_impl& charID, uint64_t nBeChallengeCharID, ResultType iResult);
	ResultType  ChallengeResponse_Sky(const unique_id_impl& charID, uint64_t nBeChallengeCharID, ResultType iResult,int nScore);
	ResultType	SetChallengeResult(LC_ServerPlayer* pkPlayer, Protocol::PS_Rank_User_Info& beChallenge, bool sucess);

	void		BeChallengeNotify(const unique_id_impl& charID, Protocol::MG_GG_Rank_Notify_beChallenge& resp, GameLogic::SOLO_DULE_TYPE eType);

	void		PostEnterGame(LC_ServerPlayer* pkPlayer);
	void		GainFlowerTitle(const unique_id_impl& nCharID, const StringType& strName, uint32_t nType, uint32_t nValue, uint32_t nLastValue);
protected:
	void		InitFlowerGiftMap();
	void		_GainFlowerGiftGroup(const unique_id_impl& nCharID, const StringType& strName, uint32_t nGroupID, const StringType& mailTitle, const StringType& mailContant, uint32_t nGuildID = 0);
	void		_GainFlowerTitle(const unique_id_impl& nCharID, const StringType& strName, uint32_t nTitleID, uint32_t nGuildID = 0);
	void		_GetItemListByGroupID(int nGroupID, UT_ServerHelper::LC_ItemCntList& rItemList);
public:
	void				FillRankPlayerBaseData(LC_ServerPlayer* pkOwnerPlayer, PS_Rank_PlayerBaseData& data);
	void				FillRankAllBuddyData(LC_ServerPlayer* pkOwnerPlayer, PS_Rank_BuddyData& data);
	void				FillRankPetData(LC_ServerPlayer* pkOwnerPlayer, PS_Rank_PetData& data);
	void				FillRankSubData(LC_ServerPlayer* pkOwnerPlayer, int32_t subType, PS_Rank_SubData& data);
	void				FillBuddyCombinedData(LC_ServerPlayer* pkOwnerPlayer, std::map<int8_t, BuddyID_Vec>& data);
	void				FillPlayerSchemeData(LC_ServerPlayer* pkOwnerPlayer, PS_Player_SchemeData& data);
	void				SetRankOpenTime(RankTimeData& timeData);
	void				SetSummitArenaOpenTime(SummitArenaTimeData& timeData);
	void				GetRankOpenTime(RankTimeData& timeData) { timeData = nRankOpenTimeInfo;};
	void				GetSummitArenaOpenTime(SummitArenaTimeData& timeData) { timeData = nSummitArenaTime;};
	bool				IsInRankTime();
	void				FillRobotBuddyData(std::vector<PS_Buddy_Info_BuddyCS>& allBuddyData, const Utility::UT_SIMDataList& baseData);
	void				FillRobotSchemeData(int32_t robotType, PS_Player_SchemeData& schemeData, const Utility::UT_SIMDataList& groupInfo);
protected:
	LC_FlowerGiftGainMap	m_kFlowerGiftGainMap;
	FlowerTitleCondition	m_kFlowerGiftCondition;
	RankTimeData    nRankOpenTimeInfo;
	SummitArenaTimeData nSummitArenaTime;//
};
#endif
