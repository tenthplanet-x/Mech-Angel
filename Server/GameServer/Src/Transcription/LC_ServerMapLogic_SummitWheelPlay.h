#ifndef _LC_SERVERMAPLOGIC_SUMMITWHEELPLAY_H
#define _LC_SERVERMAPLOGIC_SUMMITWHEELPLAY_H

/***************************************************************
	Author:				wy
	Time:				2021-12-18	
	FileName:			LC_ServerMapLogic_SummitWheelPlay
	FileExt:			.h
	Porpose:			巅峰竞技场 车轮战  n队 vs n队
****************************************************************/
#include "LC_Dule_Config.h"
#include "LC_ServerMapLogic_PlayBase.h"
#include "MG_ProtocolStruct.h"
using namespace GameLogic;
using namespace DungeonConfig;

enum SummitWheel_State
{
	SummitWheel_State_None=0,
	SummitWheel_State_Inited=1,
	SummitWheel_State_Prepare=2,
	SummitWheel_State_LoadPlayerInfo=3,
	SummitWheel_State_ShowAllInfo=4,
	SummitWheel_State_PrepareNextTeam=5,
	SummitWheel_State_ShowTeamInfo=6,
	SummitWheel_State_Battle=7,
	SummitWheel_State_BattleEnd=8,
	SummitWheel_State_WaitReward=9,
	SummitWheel_State_End=10
};
struct SingleSummitWheelFightInfo
{
	SingleSummitWheelFightInfo():uIndex(0),uChallengeTeamId(0),uBeChallengeTeamId(0),nFightId(0),bChallenge(false),uWindPlayerId(0),bIsFightValid(true)
	{
		vecChallengeReliveBuddyID.clear();
		vecBeChallengeReliveBuddyID.clear();
	}

	uint32_t uIndex; // 回合 从1开始
	uint32_t uChallengeTeamId;
	uint32_t uBeChallengeTeamId;
	int64_t  nFightId; // 回放id
	bool bChallenge;  // 挑战者是否胜利
	unique_id_impl uWindPlayerId;
	bool         bIsFightValid;
	mem::vector <int> vecChallengeReliveBuddyID;
	mem::vector <int> vecBeChallengeReliveBuddyID;
};
class LC_ServerMapLogic_SummitWheelPlay : public LC_ServerMapLogic_PlayBase
{
public:
	DECL_RTTI_NAME( LC_ServerMapLogic_SummitWheelPlay )
public:
	LC_ServerMapLogic_SummitWheelPlay();
	virtual ~LC_ServerMapLogic_SummitWheelPlay();
	bool Init(LC_ServerMapLogic_Place* place,LC_Dule_Config* config);
	virtual void UnInit();
	virtual bool Start();
	virtual void End();
	virtual bool Update(float fCurrentTime, float fDeltaTime);
	virtual ResultType CheckEnter(LC_ServerPlayer* player);
	virtual Utility::UT_Vec3Int	ComputeEnterLocation(LC_ServerPlayer* player,const Utility::UT_Vec3Int& targetlocation);
	virtual void EventNotify(MapLogicEvent& kEvent);
	virtual void PlayerEventNotify(LC_ServerPlayer* player,MapLogicEvent& kEvent);
	virtual bool CheckEnd();
	virtual bool CheckRun();
	void ClearBuddyOp(LC_ServerPlayer* pPlayer);
	void Settle(bool bLeave = false);
	void SendReward();
	void ChallengePlayerActionForbid(LC_ServerPlayer* pPlayer);
	void ChallengePlayerActionUnForbid(LC_ServerPlayer* pPlayer);
	void ReservaDuleInfo(const unique_id_impl challengeCharID,const unique_id_impl bechallengeCharID,uint64_t uniqueID);
	void Prepare();
	void InitAllBuddyInfo();
	void Battle();
	void BattleEnd(int endType,LC_ServerPlayer* pPlayer,bool timeOut =false);
	bool PrepareNextTeamBattle();
	void SyncFightReportToGroup();
	void updateNullTeamIDFailed();
	void updateRemainTeamFailed(int currIndex,bool bChallengeFailed);
	bool CheckCanNextTeamBattle();
	void PlayerLevave(int opType);
	void InitHatred( LC_ServerPlayer* pPlayerAController, LC_ServerPlayer* pPlayerBController );
	void UpLoadSoloDulePlayInfoToGroup(LC_ServerPlayer* challengePlayer,LC_ServerPlayer* bechallengePlayer);
	static void SummonPlayerFinish(LC_ServerPlayer* player);
	void NotifyShowAllInfoTime();
	void PackSkyRoundTeamDeadInfo(MG_NOTIFY_SkyRoundShowInfo& notifyMsg);
	void NotifyShowTeamInfoTime();
	bool CheckChallengeBeChallengePtr();
	void SendErrorReward();
	bool InitTeamInfo(bool challengeFlag);
	void UpdateBuddyDeadInfo(LC_ServerPlayer* pPlayer,mem::vector <int>& vecReliveBuddyID );
	void UpdateTeamDeadInfo(int nCurrIndex,unique_id_impl uWindPlayerId,bool bChallenge = false);
	void InitTeamFightId(int nCurrIndex);
	void SetWheelNextState(SummitWheel_State eState);
private:
	void SetRoundBattleResult(LC_ServerPlayer* player,bool timeOut =false);
	bool CheckOwnerDead( LC_ServerPlayer* pPlayer, object_id_type& nDeadPlayerID );
	void InitBattle( LC_ServerPlayer* pPlayerAController, LC_ServerPlayer* pPlayerBController );

public:
	int m_nEntryPlayerNum; // 已进入玩家数
private:
	LC_Dule_Config*		m_Config;
	SummitWheel_State		m_State;

	int m_mapID;
	int m_CurrChallengeTeamId;
	int m_CurrBeChallengeTeamId;
	object_id_type		m_ChallengePlayerID;
	unique_id_impl		m_ChallengeCharID;
	unique_id_impl		m_BeChallengeCharID;
	uint64_t m_uniqueID;
	LC_ServerPlayer*	m_BeChallengePlayer;
	LC_ServerPlayer*	m_ChallengePlayer;
	float				m_TimeStamp;
	uint32_t       m_uCurrIndex;  // 当前回合
	int64_t		   m_solofightuniqueId; // 苍穹战场 挑战唯一id
	bool           m_bChallengeWind; // 挑战者是否胜利
	std::vector<SingleSummitWheelFightInfo> m_vecResult;
	
};
#endif