#include "LC_GameStory_FactionWar.h"
#include "LC_GameStoryDataDefine.h"

using namespace GameLogic;
using namespace GameStory;
using namespace Utility;

GSFactionScore::GSFactionScore() : bDirty(false)
{
	Init();
}

//---------------------------------------------------------------
LC_GameStory_FactionWar::LC_GameStory_FactionWar(int32_t logicID, GameStoryRecord *pkRecord)
:LC_Server_GameStory_Base(logicID, pkRecord), m_fLastSyncScoreTime(0)
,m_iWinFactionID(FACTION_NONE)
{
}

LC_GameStory_FactionWar::~LC_GameStory_FactionWar()
{
	ClearFactionScores();
}

void LC_GameStory_FactionWar::Init()
{
	LC_Server_GameStory_Base::Init();	
	GameStoryConfig* pkConfig = GetConfig();
	if (NULL == pkConfig)
	{
		return;
	}
	UT_SIMDataList& kAttackTargets = pkConfig->m_kFactionWar.kAttackTargets;
	UT_SIMDataList::iterator it = kAttackTargets.begin();
	for (; it != kAttackTargets.end(); ++it)
	{
		UT_SIMDataInfo& kDataInfo = *it;
		FactionWarTarget kTarget;
		kTarget.iType = kDataInfo.IID();
		kTarget.iID = kDataInfo.ParamA();
		kTarget.iNum = 0;
		kTarget.iThreshold = kDataInfo.ParamB();
		m_kAttackTargetMap[kTarget.GetUniqueID()] = kTarget;
	}
	ClearFactionScores();
}

void LC_GameStory_FactionWar::UnInit()
{
	LC_Server_GameStory_Base::UnInit();
	m_kAttackTargetMap.clear();
	ClearFactionScores();
	m_kAttackers.clear();
	m_kDefenders.clear();
}

void LC_GameStory_FactionWar::SetWinFactionID(FactionType iFactionID)
{
	if (iFactionID != FACTION_ATTACKER && iFactionID != FACTION_DEFENDER)
	{
		return;
	}
	m_iWinFactionID = iFactionID;
}

void LC_GameStory_FactionWar::InitFactionScore(GameStoryScheduleKey iKey)
{
	FactionScoreMap::iterator it = m_kFactionScoreMap.find(iKey);
	if (it == m_kFactionScoreMap.end())
	{
		GSFactionScore kScores;
		kScores.bDirty = true;
		m_kFactionScoreMap[iKey] = kScores;
	}
	else
	{
		it->second.Clear();
		it->second.bDirty = true;
	}
}

void LC_GameStory_FactionWar::AddFactionScore(GameStoryScheduleKey iKey, int32_t iType, int32_t iVal, GameLogic::LC_ActorBase* pkKiller, GameLogic::LC_ActorBase* pkInjured)
{
	if (iType < 0 || iType >= GS_FACTION_SCORE_MAX)
	{
		return;
	}
	GSFactionScore& kScores = m_kFactionScoreMap[iKey];
	kScores.kFactionScores[iType] += iVal;
	kScores.bDirty = true;

	CheckTriConditions(iKey);
}

int32_t LC_GameStory_FactionWar::GetFactionScore(GameStoryScheduleKey iKey, int32_t iType)
{
	FactionScoreMap::const_iterator it = m_kFactionScoreMap.find(iKey);
	if (it == m_kFactionScoreMap.end())
	{
		return 0;
	}
	return (iType < 0 || iType >= GS_FACTION_SCORE_MAX) ? 0 : it->second.kFactionScores[iType];
}

void LC_GameStory_FactionWar::SetFactionScoreDirty(GameStoryScheduleKey iKey)
{
	FactionScoreMap::iterator it = m_kFactionScoreMap.find(iKey);
	if (it == m_kFactionScoreMap.end())
	{
		return;
	}
	it->second.bDirty = true;
}

void LC_GameStory_FactionWar::CheckSyncFactionScore(float fCurrentTime)
{
	if (fCurrentTime < m_fLastSyncScoreTime + 0.5)
	{
		return;
	}
	FactionScoreMap::iterator it = m_kFactionScoreMap.begin();
	for (; it != m_kFactionScoreMap.end(); ++it)
	{
		GSFactionScore& kScores = it->second;
		if (!kScores.bDirty)
		{
			continue;
		}
		SyncFactionScore(it->first);
		kScores.bDirty = false;
	}
}

void LC_GameStory_FactionWar::ClearFactionScores()
{
	FactionScoreMap::iterator it = m_kFactionScoreMap.begin();
	for (; it != m_kFactionScoreMap.end(); ++it)
	{
		it->second.Clear();
	}
	m_kFactionScoreMap.clear();
}

void LC_GameStory_FactionWar::ClearFactionScore(GameStoryScheduleKey iKey)
{
	FactionScoreMap::iterator it = m_kFactionScoreMap.find(iKey);
	if (it == m_kFactionScoreMap.end())
	{
		return;
	}
	it->second.Clear();
}

void LC_GameStory_FactionWar::AttackTargetStat(int32_t iType, int32_t iID, int32_t iNum)
{
	if (iType <= FactionWarTarget_None || iType >= FactionWarTarget_End)
	{
		return;
	}
	GameStoryConfig* pkConfig = GetConfig();
	if (NULL == pkConfig)
	{
		return;
	}
	int64_t iUniqueID = (int64_t)iType << 32 | iID;
	FactionWarTargetMap::iterator it = m_kAttackTargetMap.find(iUniqueID);
	if (it == m_kAttackTargetMap.end())
	{
		return;
	}
	it->second.iNum += iNum;
	if (CheckAttackTarget())
	{
		SetWinFactionID(FACTION_ATTACKER);
	}
}

bool LC_GameStory_FactionWar::CheckAttackTarget()
{
	FactionWarTargetMap::iterator it = m_kAttackTargetMap.begin();
	for (; it != m_kAttackTargetMap.end(); ++it)
	{
		if (it->second.iNum < it->second.iThreshold)
		{
			return false;
		}
	}
	return true;
}

void LC_GameStory_FactionWar::OnPlayerInWar(LC_PlayerBase* pkPlayer)
{
	if (NULL == pkPlayer)
	{
		return;
	}
	if (pkPlayer->GetFactionID() == FACTION_ATTACKER)
	{
		m_kAttackers[pkPlayer->GetInstanceUniqueID()] = pkPlayer->GetOwnerCharName();
	}
	else if (pkPlayer->GetFactionID() == FACTION_DEFENDER)
	{
		m_kDefenders[pkPlayer->GetInstanceUniqueID()] = pkPlayer->GetOwnerCharName();
	}
}
