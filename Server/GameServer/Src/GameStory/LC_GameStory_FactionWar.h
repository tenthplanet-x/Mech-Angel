#pragma once
//ÕóÓªÕ½
#include "LC_ServerPlayer.h"
#include "LC_GameStoryDataDefine.h"
#include "LC_GameStoryBase.h"
#include "SystemDefine.h"

namespace GameStory
{
struct GSFactionScore
{
	GSFactionScore();
	void Init()
	{
		memset(kFactionScores, 0, GS_FACTION_SCORE_MAX*sizeof(int32_t));
	}
	void Clear()
	{
		bDirty = false;
		Init();
	}

	bool	bDirty;
	int32_t kFactionScores[GS_FACTION_SCORE_MAX];
};
typedef mem::map<GameStoryScheduleKey, GSFactionScore> FactionScoreMap;

enum FactionWarTargetType
{
	FactionWarTarget_None = -1,
	FactionWarTarget_Npc = 0,
	FactionWarTarget_End,
};

struct FactionWarTarget
{
	FactionWarTarget() : iType(FactionWarTarget_None), iID(0), iNum(0), iThreshold(0) {}
	int64_t GetUniqueID() const { return (int64_t)iType << 32 | iID; }

	int32_t iType;
	int32_t iID;
	int32_t iNum;
	int32_t iThreshold;
};

typedef mem::map<int64_t, FactionWarTarget> FactionWarTargetMap;
class LC_GameStory_FactionWar : public LC_Server_GameStory_Base
{
public:
	enum DIRTY_FLAG_TYPE {
		DIRTY_FLAG_FACTION_SCORE = 0,
	};

	LC_GameStory_FactionWar(int32_t logicID, GameStoryRecord *pkRecord);
	virtual ~LC_GameStory_FactionWar();

	virtual void	Init();
	virtual void	UnInit();

	virtual FactionType	GetWinFactionID() const { return m_iWinFactionID; }
	virtual	void		SetWinFactionID(FactionType iFactionID);

	virtual void	InitFactionScore(GameStoryScheduleKey iKey);
	virtual void	AddFactionScore(GameStoryScheduleKey iKey, int32_t iType, int32_t iVal, GameLogic::LC_ActorBase* pkKiller, GameLogic::LC_ActorBase* pkInjured);
	virtual int32_t	GetFactionScore(GameStoryScheduleKey iKey, int32_t iType);
	virtual void	SetFactionScoreDirty(GameStoryScheduleKey iKey);
	virtual void	CheckSyncFactionScore(float fCurrentTime);
	virtual void	ClearFactionScores();
	virtual void	ClearFactionScore(GameStoryScheduleKey iKey);

	virtual void	AttackTargetStat(int32_t iType, int32_t iID, int32_t iNum);
	virtual bool	CheckAttackTarget();

	virtual	void	OnPlayerInWar(LC_PlayerBase* pkPlayer);
	virtual	GSServerReward::MailPlayerMap* GetAttackers() { return &m_kAttackers; }
	virtual GSServerReward::MailPlayerMap* GetDefenders() { return &m_kDefenders; }

protected:
	float							m_fLastSyncScoreTime;
	FactionType						m_iWinFactionID;
	FactionScoreMap					m_kFactionScoreMap;
	FactionWarTargetMap				m_kAttackTargetMap;
	GSServerReward::MailPlayerMap	m_kAttackers;
	GSServerReward::MailPlayerMap	m_kDefenders;
};
}
