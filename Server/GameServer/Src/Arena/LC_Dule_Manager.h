#ifndef _LC_DUEL_MANAGER_H
#define _LC_DUEL_MANAGER_H
/***************************************************************
	Author:				sunny
	Time:				2013-10-23			
	FileName:			Lc_Dule_Rank.h
	FileExt:			.h
	Porpose:			竞技场排行榜
****************************************************************/
#include "MG_Game2Group.h"
#include "LC_Define.h"
#include "MG_CS_Guild.h"

struct Solo_Duel_Info :public Memory::MM_BaseObject
{
	unique_id_impl	m_CharID;
	int				m_UserIndex;
	unique_id_impl	m_beChallengeCharID;
	int				m_beChallengeIndex;
	uint64_t		m_solofightuniqueId;
	int				m_uScore;
	Solo_Duel_Info():m_CharID(0),m_UserIndex(0),m_beChallengeCharID(0),m_beChallengeIndex(0),m_solofightuniqueId(0),m_uScore(0)
	{}
};

class LC_Dule_Manager;
class LC_Solo_Duel_Manager:public Memory::MM_BaseObject
{
public:
	typedef mem::map<unique_id_type, Solo_Duel_Info*> SoloDuelInfoMap;
	
	LC_Solo_Duel_Manager();
	virtual ~LC_Solo_Duel_Manager();
	DECL_RTTI_NAME( LC_Solo_Duel_Manager )
	
	bool			init(LC_Dule_Manager* parent, GameLogic::SOLO_DULE_TYPE eType,MapType eMapType,int nMapResId);
	void			fin();
	Solo_Duel_Info* GetSoloDuelInfo(const unique_id_impl& charID);

	void			AddSoloDuelInfo(const unique_id_impl& charID,const unique_id_impl& bechallengeCharID,int challengeindex,int bechallengeindex);
	void			DelSoloDuelInfo(const unique_id_impl& charID);
	LC_ServerPlayer* GetPlayer(const unique_id_impl& charID);
	
	ResultType		ChallengeCheck(LC_ServerPlayer* pkPlayer,const unique_id_impl& bechallengeCharID,int bechallengeindex);
	ResultType		ChallengeCheckResult(const unique_id_impl& charID,uint64_t nBeChallengeCharID, ResultType iResult,uint64_t solofightuniqueId = 0,int nScore = 0);
	ResultType		ChallengeAskClientRes(LC_ServerPlayer* player,int resid);
	ResultType		ChallengeAnswerClientRes(LC_ServerPlayer* player,int resid,bool result);

	ResultType		Challenge(LC_ServerPlayer* pkPlayer,const unique_id_impl& bechallengeCharID,int beChallengeindex,uint64_t solofightuniqueId = 0);
	ResultType		SetChallengeResultResponse(const unique_id_impl& charID,Protocol::MG_GG_RESP_Challenge_Result& resp);
	
	ResultType		BeChallengeNotify(const unique_id_impl& charID,Protocol::MG_GG_Rank_Notify_beChallenge& resp);

	ResultType		AddChallengeTime(LC_ServerPlayer* pkPlayer,int nLogCode = 0, const vector<int>& nParams = vector<int>());
	ResultType		GetChallengeCD(LC_ServerPlayer* pkPlayer);
	ResultType		ClearChallengeCD(LC_ServerPlayer* pkPlayer,int nLogCode = 0, const vector<int>& nParams = vector<int>());
	ResultType		GetDailyReward(LC_ServerPlayer* pkPlayer, int rankID, bool bGet, Protocol::MG_GG_RTL_Gain_Rank_Gift* pGainRtl = NULL,int nLogCode = 0, const vector<int>& nParams = vector<int>());

	ResultType		GuildOfflineChallengeTimeOP(LC_ServerPlayer* pkPlayer, int op,int nBuyNum, Protocol::MG_Resp_GuildFightRobScore_Opt& msg);

	ResultType		SetChallengeResult_Arena(LC_ServerPlayer* pkPlayer, uint64_t beChallengeCharID, Protocol::PS_Rank_User_Info& beChallenge, bool sucess);
	ResultType		SetChallengeResult_GuildOffline(LC_ServerPlayer* pkPlayer, uint64_t beChallengeCharID, uint64_t beChallengeGuildID, const StringType& beChallengeCharName, bool sucess);
private:
	ResultType		__CheckChallenge(LC_ServerPlayer* pkPlayer);
private:
	GameLogic::SOLO_DULE_TYPE m_eType; // 程序中 副本枚举
	MapType m_eMapType; // 配置表中 枚举 
	int32_t m_nMapResId;
	LC_Dule_Manager* m_Parent;
	SoloDuelInfoMap m_SoloDuelMap;
};



class LC_Dule_Manager:public Memory::MM_BaseObject
{
public:
	LC_Dule_Manager();
	virtual ~LC_Dule_Manager();
	DECL_RTTI_NAME( LC_Dule_Manager )
	bool init();
	void fin();
	LC_Solo_Duel_Manager* GetSoloDuleManager( int nType);
private:
	LC_Solo_Duel_Manager m_soloDuleManager;
	LC_Solo_Duel_Manager m_soloDuleManager_Sky;
	LC_Solo_Duel_Manager m_soloDuleManager_SummitTeam;
	LC_Solo_Duel_Manager m_soloDuleManager_SummitWheel;
	LC_Solo_Duel_Manager m_soloDuleManager_GuildOffline;
};
#endif