#ifndef _LC_SERVERMAPLOGIC_SUMMITTEAMPLAY_H
#define _LC_SERVERMAPLOGIC_SUMMITTEAMPLAY_H

/***************************************************************
	Author:				wy
	Time:				2021-12-14	
	FileName:			LC_ServerMapLogic_SummitTeamPlay
	FileExt:			.h
	Porpose:			�۷徺����С����   n�� vs n��
****************************************************************/
#include "LC_Dule_Config.h"
#include "LC_ServerMapLogic_PlayBase.h"
#include "MG_ProtocolStruct.h"
using namespace GameLogic;
using namespace DungeonConfig;

enum SummitTeam_State
{
	SummitTeam_State_None=0,
	SummitTeam_State_Inited=1,
	SummitTeam_State_Prepare=2,
	SummitTeam_State_LoadPlayerInfo=3,
	SummitTeam_State_ShowAllInfo=4,
	SummitTeam_State_PrepareNextTeam=5,
	SummitTeam_State_ShowTeamInfo=6,
	SummitTeam_State_Battle=7,
	SummitTeam_State_BattleEnd=8,
	SummitTeam_State_WaitReward=9,
	SummitTeam_State_End=10
};
struct SingleSummitTeamFightInfo
{
	SingleSummitTeamFightInfo():uIndex(0),uChallengeTeamId(0),uBeChallengeTeamId(0),nFightId(0),bChallenge(false),uWindPlayerId(0),bIsFightValid(true)
	{
		vecChallengeReliveBuddyID.clear();
		vecBeChallengeReliveBuddyID.clear();
	}
	
	uint32_t uIndex; // �غ� ��1��ʼ
	uint32_t uChallengeTeamId;
	uint32_t uBeChallengeTeamId;
	int64_t nFightId; // �ط�id
	bool bChallenge;  // ��ս���Ƿ�ʤ��
	unique_id_impl uWindPlayerId;
	bool  bIsFightValid;//ս���Ƿ���Ч
	mem::vector <int> vecChallengeReliveBuddyID;
	mem::vector <int> vecBeChallengeReliveBuddyID;
};
class LC_ServerMapLogic_SummitTeamPlay : public LC_ServerMapLogic_PlayBase
{
public:
	DECL_RTTI_NAME( LC_ServerMapLogic_SummitTeamPlay )
public:
	LC_ServerMapLogic_SummitTeamPlay();
	virtual ~LC_ServerMapLogic_SummitTeamPlay();
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
	void ReservaDuleInfo(object_id_type challengeid,const unique_id_impl& bechallengeCharID);
	void Prepare();
	void InitAllBuddyInfo();
	void Battle();
	void BattleEnd(int endType,LC_ServerPlayer* pPlayer,bool timeOut =false);
	bool PrepareNextTeamBattle();
	void SyncFightReportToGroup();
	void updateNullTeamIDFailed();
	bool CheckCanNextTeamBattle();
	void updateRemainTeamFailed(int currIndex,bool bChallengeFailed);
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
	void SetSummitTeamNextState(SummitTeam_State eState);
private:
	void SetRoundBattleResult(LC_ServerPlayer* player,bool timeOut =false);
	bool CheckOwnerDead( LC_ServerPlayer* pPlayer, object_id_type& nDeadPlayerID );
	void InitBattle( LC_ServerPlayer* pPlayerAController, LC_ServerPlayer* pPlayerBController );

public:
	int m_nEntryPlayerNum; // �ѽ��������
private:
	LC_Dule_Config*		m_Config;
	SummitTeam_State		m_State;
	int m_mapID;
	int m_CurrChallengeTeamId;
	int m_CurrBeChallengeTeamId;
	object_id_type		m_ChallengePlayerID;
	unique_id_impl		m_BeChallengeCharID;
	LC_ServerPlayer*	m_BeChallengePlayer;
	LC_ServerPlayer*	m_ChallengePlayer;
	float				m_TimeStamp;
	uint32_t       m_uCurrIndex;  // ��ǰ�غ�
	uint64_t		m_solofightuniqueId; // ���ս�� ��սΨһid
	bool           m_bChallengeWind; // ��ս���Ƿ�ʤ��
	std::vector<SingleSummitTeamFightInfo> m_vecResult;
	//�洢���buff
	std::string			m_PlayerBuffInfo;
};
#endif