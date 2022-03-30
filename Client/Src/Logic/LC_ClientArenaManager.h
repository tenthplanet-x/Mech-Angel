#ifndef _LC_CLIENT_ARENA_MANAGER_H
#define _LC_CLIENT_ARENA_MANAGER_H
#include "LC_ClientGameEvent.h"
#define SIGNUPLEVEL 40

#define PEAKARENA_RANKINGLIST_TOP  5
#define PEAKARENA_RANKINGLIST_TOPTAL 10



class LC_ClientArenaPlayerDataEntry : public Memory::MM_BaseObject
{
public:
	LC_ClientArenaPlayerDataEntry();
	~LC_ClientArenaPlayerDataEntry();
	
	DECL_RTTI_NAME( LC_ClientArenaPlayerDataEntry )

	void Reset()
	{
		m_bValid				= false;
		m_lID					= 0;
		m_kPlayerName			= "";
		m_iPlayerInArenaState	= 0;
		m_iKillNum				= 0;
		m_iDeadNum				= 0;
		m_iGainScore			= 0;
		m_iGainExp				= 0;
	};

	void		GetDataFromProtocolStruct(Protocol::PS_ArenaPlayerStateData& rkStruct);
	void		GetDataFromProtocolStruct(Protocol::PS_ArenaPlayerResultData& rkStruct);

	
	MEMBER_BOOL(Valid);
	MEMBER_LONG(ID);
	MEMBER_STRING(PlayerName);
	MEMBER_INT(PlayerInArenaState);
	MEMBER_INT(KillNum);
	MEMBER_INT(DeadNum);
	MEMBER_INT(GainScore);
	MEMBER_INT(GainExp);
};

class LC_ClientArenaManager : public Memory::MM_BaseObject,
								public Utility::UT_Singleton<LC_ClientArenaManager>
{
public:
	LC_ClientArenaManager();
	~LC_ClientArenaManager();

	DECL_RTTI_NAME( LC_ClientArenaManager )

	void		Reset();
	void		SetArenaState(int iState);
	int			GetArenaState(){return m_eArenaState;};
	bool		CanRespawn();

	void		SetArenaStage(int iStage);
	int			GetArenaStage(){return m_eArenaStage;};
	
	bool		GetArenaModeOpen(int iArenaType);
	StringType  GetArenaModeName(int iArenaType);
	StringType	GetArenaModeDescript(int iArenaType);

	long		GetEXPNeedToArenaLevelUp(int iLevel);
	StringType  GetArenaLevelName(int iLevel);

	int			GetArenaSignUpUserCount(){return m_iArenaSignUpUserCount;};
	int			GetArenaType(){return m_eArenaType;};
	float		GetStartWaitEnterArenaTime(){return m_fStartWaitEnterArenaTime;};
	float		GetNeedWaitEnterArenaTime(){return m_fNeedWaitEnterArenaTime;};
	float		GetStartWaitFinishArenaTime(){return m_fStartWaitFinishArenaTime;};
	float		GetNeedWaitFinishArenaTime(){return m_fNeedWaitFinishAreanTime;};
	float		GetArenaCurrentTurnLeftTime(){return m_fArenaCurrentTurnLeftTime;};
	long		GetArenaTurnCount();
	long		GetArenaGroupID(int iArenaPlayerGroupType);
	long		GetArenaWinGroupType();
	long		GetPlayerArenaGroupType();
	long		GetArenaGroupScore(int iArenaPlayerGroupType);
	long		GetArenaGroupToken(int iArenaPlayerGroupType);


	LC_ClientArenaPlayerDataEntry* GetPlayerDataEntryInGroup(int iArenaPlayerGroupType,int iIndex);


	void		RequestSignUpArena(int iArenaType);
	void		RequestCancelSignUpArena();
	void		RequestCancleSignUpArenaInPrison();
	void		RequestLeaveArena();
	void		RequestArenaState();
	void		RequestGiveUpArena();

	//void		RecvSignUpArena(Protocol::MG_RLT_SignUpArena& rkMsg);
	void		RecvCancelSignUpArena(Protocol::MG_RLT_CancelArena& rkMsg);
	void		RecvMatchArena(Protocol::MG_RLT_MatchArena& rkMsg);
	void		RecvEnterArena(Protocol::MG_RLT_EnterArena& rkMsg);
// 	void		RecvLeaveArena(Protocol::MG_RLT_LeaveArena& rkMsg);
	void		RecvArenaState(Protocol::MG_RLT_ArenaState& rkMsg);
	void		RecvArenaResult(Protocol::MG_RLT_ArenaResult& rkMsg);
	void		RecvArenaDoorOpen();
	void		RecvArenaDoorClose();
	void		RecvArenaTip(Protocol::MG_RLT_ArenaTip& rkMsg);
// 	void		RecvGiveUpArena(Protocol::MG_RLT_LeaveArena& rkMsg);
	void		RecvArenaMatchAgain();
	void		RecvArenaConfirmCancel();

	//竞技场排行奖励信息
	StringType	GetArenaRankRewardGrade();

	//巅峰战场相关协议
	void		RequestSignUpPeakArena();
	void		RequestLeavePeakArena();
	void		ResetPeakLadderData();
	LC_ClientRankUserInfo*		GetLadderUserInfo(int iIndex);
	LC_ClientRankUserInfo*		GetTopLadderUserInfo(int iRanking);
	LC_ClientRankUserInfo*		GetLadderRankUserInfo(int iRanking);

protected:

	GameLogic::LC_ArenaState			m_eArenaState;
	GameLogic::LC_ArenaStage			m_eArenaStage;
	GameLogic::LC_ArenaType				m_eArenaType;
	int									m_iArenaSignUpUserCount;

	float								m_fStartWaitEnterArenaTime;
	float								m_fNeedWaitEnterArenaTime;
	float								m_fLastRequestArenaStateTime;
	float								m_fStartWaitFinishArenaTime;
	float								m_fNeedWaitFinishAreanTime;
	float								m_fArenaCurrentTurnLeftTime;
	 
	long								m_lArenaTurnCount;
	long								m_lRedGroupID;
	long								m_lBlueGroupID;
	long								m_lWinGroupID;
	long								m_lRedGroupScore;
	long								m_lBlueGroupScore;
	long								m_lRedGroupToken;
	long								m_lBlueGroupToken;

	long								m_lRedGroupDoorID;
	long								m_lBlueGroupDoorID;
	
	long								m_lSignUpLevel;

	LC_ClientArenaPlayerDataEntry		m_akRedGroupPlayerData[MAX_PLAYER_GROUP_MEMBER_LIST_SIZE];
	LC_ClientArenaPlayerDataEntry		m_akBlueGroupPlayerData[MAX_PLAYER_GROUP_MEMBER_LIST_SIZE];

	mem::map<int32_t, LC_ClientRankUserInfo>	m_kPeakArenaLadderTopUsersMap;
	mem::map<int32_t, LC_ClientRankUserInfo>	m_akPeakArenaLadderRankUsers;
};


#endif