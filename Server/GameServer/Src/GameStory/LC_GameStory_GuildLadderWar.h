#pragma once
//公会天梯战
#include "LC_ServerPlayer.h"
#include "LC_GameStoryDataDefine.h"
#include "LC_GameStoryBase.h"
#include "SystemDefine.h"

namespace GameStory
{
	struct GSGuildLadderOccupationInfo
	{
		GSGuildLadderOccupationInfo()
		:m_iOccupateTime(0)
		, m_iFaction(FACTION_NONE)
		, m_iIndexFlag(-1)
		, m_iScoreDeltaTime(0)
		, m_iAddFactionScore(0)
		, m_iAddActorScore(0)
		{
		}
		float m_iOccupateTime;
		int32_t m_iFaction;
		int32_t m_iIndexFlag;
		int32_t m_iScoreDeltaTime;
		int32_t m_iAddFactionScore;
		int32_t m_iAddActorScore;
	};
	enum E_GSGuildLaddderScoreType
	{
		E_GSGuildLaddderScore_None = -1,
		E_GSGuildLaddderScore_Kill = 0,
		E_GSGuildLaddderScore_Occupation = 1,
		E_GSGuildLaddderScore_Gather = 2,
	};
	typedef mem::map<int32_t, int32_t> GSGuildLadderOccupationRecordMap;
	struct GSGuildLadderFactionScoreRecord
	{
		GSGuildLadderFactionScoreRecord() :m_iTotalScore(0), m_iKillScore(0), m_iGatherScore(0)
		{
			m_kOccupationScore.clear();
		}
		int32_t m_iTotalScore;
		int32_t m_iKillScore;
		GSGuildLadderOccupationRecordMap m_kOccupationScore;
		int32_t m_iGatherScore;
	};

	typedef mem::map<int32_t, GSGuildLadderFactionScoreRecord> GSGuildLadderFactionScoreMap;
	typedef mem::map<unique_id_impl, int32_t> GSGuildLadderActorScoreMap;
	typedef mem::map<object_id_type, GSGuildLadderOccupationInfo> GSGuildLadderOccupationMap;
	struct GSGuildLadderInfo
	{
		GSGuildLadderInfo()
		{
			Clear();
		}
		void Clear()
		{
			m_bEnd = false;
			m_kFactionScore.clear();
			m_kActorScore.clear();
			m_kOccupation.clear();
		}

		void AddFactionScore(int32_t iFaction, int32_t iVal, E_GSGuildLaddderScoreType iScoreType, int32_t iParam)
		{
			GSGuildLadderFactionScoreRecord &rkRecord = m_kFactionScore[iFaction];
			switch (iScoreType)
			{
			case E_GSGuildLaddderScore_Kill:
				rkRecord.m_iKillScore += iVal;
				break;
			case E_GSGuildLaddderScore_Gather:
				rkRecord.m_iGatherScore += iVal;
				break;
			case E_GSGuildLaddderScore_Occupation:
				rkRecord.m_kOccupationScore[iParam] += iVal;
				break;
			default:
				return;
			}
			rkRecord.m_iTotalScore += iVal;
			SetWarInfoDirty(true);
		}
		int32_t GetFactionScore(int32_t iFaction)
		{
			return m_kFactionScore[iFaction].m_iTotalScore;
		}
		void AddActorScore(const unique_id_impl& charID, int32_t iVal)
		{
			m_kActorScore[charID] += iVal;
		}
		int32_t GetActorScore(const unique_id_impl& charID)
		{
			GSGuildLadderActorScoreMap::iterator iter = m_kActorScore.find(charID);
			if (iter == m_kActorScore.end())
			{
				return 0;
			}
			return iter->second;
		}

		void	SetEnd(){ m_bEnd = true; }
		bool	IsEnd()const{ return m_bEnd; }

		bool	GetWarInfoDirty()const{ return m_kWarInfoDirty.GetFlag(); }
		void	SetWarInfoDirty(bool bFlag){ m_kWarInfoDirty.SetFlag(bFlag); }

	
		GSGuildLadderFactionScoreMap	m_kFactionScore;
		GSGuildLadderActorScoreMap		m_kActorScore;
		GSGuildLadderOccupationMap		m_kOccupation;//旗帜占领

		bool	m_bEnd;
		LC_DirtyFlag					m_kWarInfoDirty;
	};

	typedef mem::map<GameStoryScheduleKey, GSGuildLadderInfo> GSGuildLadderInfoMap;
	class LC_GameStory_GuildLadderWar : public LC_Server_GameStory_Base
	{
	public:

		LC_GameStory_GuildLadderWar(int32_t logicID, GameStoryRecord *pkRecord);
		virtual ~LC_GameStory_GuildLadderWar();

		virtual void	Init();
		virtual void	UnInit();

		virtual void 	Update(float fCurrentTime, float fDeltaTime);
		virtual void	ActorBeKilled(GameLogic::LC_ActorBase* pkKiller, GameLogic::LC_ActorBase*  pkInjured);

		GSGuildLadderInfo* GetGuildLadderInfoPtr(GameStoryScheduleKey iKey);

		virtual void	AddFactionScore(GameStoryScheduleKey iKey, int32_t iType, int32_t iVal, GameLogic::LC_ActorBase* pkKiller, GameLogic::LC_ActorBase* pkInjured);
		virtual int32_t	GetFactionScore(GameStoryScheduleKey iKey, int32_t iType);

		virtual void	AddActorScore(GameStoryScheduleKey iKey, int32_t iVal, GameLogic::LC_ActorBase* pkKiller, GameLogic::LC_ActorBase* pkInjured);
		virtual void	FactionOccupation(GameStoryScheduleKey iKey, GameLogic::LC_ActorBase* pkPlayer, GameLogic::LC_ActorBase* pkBeOccupied);

	protected:
		virtual void	_onDeactived(void);
		virtual void	PostPlayerEnterMap(GameLogic::LC_PlayerBase* pkPlayer, LC_ServerMapLogicBase* pkPlace);
		virtual void	_clearBatchSchedule(GameStoryScheduleKey iKey, GameStoryConfig* pkConfig);
		void	_guildLadderWarSettle(GameStoryScheduleKey iKey);
		void	_addOccupationFactionScore(GameStoryScheduleKey iKey, int32_t iFaction, int32_t iVal, int32_t iIndexFlag);
		BOOL	_checkFactionScoreToEnd(GameStoryScheduleKey ikey, int32_t iFaction);
		void	_syncWarInfo(GameStoryScheduleKey iKey, GameLogic::LC_PlayerBase* pkPlayer);
		void	_syncActorScore(GameStoryScheduleKey ikey, GameLogic::LC_PlayerBase* pkPlayer);

	protected:


		float							m_fLastSyncScoreTime;

		GSGuildLadderInfoMap			m_kAllGuildLadderInfo;
	};


}
