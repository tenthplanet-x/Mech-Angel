#ifndef _LC_SERVERMAPLOGIC_SOLODULEPLAY_H
#define _LC_SERVERMAPLOGIC_SOLODULEPLAY_H

/***************************************************************
	Author:				sunny
	Time:				2014-07-25	
	FileName:			LC_ServerMapLogic_SoloDulePlay
	FileExt:			.h
	Porpose:			µ¥ÌôÍæ·¨
****************************************************************/
#include "LC_Dule_Config.h"
#include "LC_ServerMapLogic_PlayBase.h"
using namespace GameLogic;
using namespace DungeonConfig;

enum SoloDule_State
{
	SoloDule_State_None=0,
	SoloDule_State_Inited=1,
	SoloDule_State_Prepare=2,
	SoloDule_State_CountDown=3,
	SoloDule_State_Battle=4,
	SoloDule_State_End=5
};

class LC_ServerMapLogic_SoloDulePlay : public LC_ServerMapLogic_PlayBase
{
public:
	DECL_RTTI_NAME( LC_ServerMapLogic_SoloDulePlay )
public:
	LC_ServerMapLogic_SoloDulePlay();
	virtual ~LC_ServerMapLogic_SoloDulePlay();
	bool Init(LC_ServerMapLogic_Place* place,LC_Dule_Config* config, GameLogic::SOLO_DULE_TYPE eSoloType);
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
	void Settle();
	void ChallengePlayerActionForbid();
	void ChallengePlayerActionUnForbid();
	void ReservaDuleInfo(object_id_type challengeid,const unique_id_impl& bechallengeCharID,int bechallengeindex,int64_t isolofightuniqueId);
	void Prepare();
	void CountDown();
	void Battle();
	void BattleResult();

	void UpLoadSoloDulePlayInfoToGroup(LC_ServerPlayer* challengePlayer,LC_ServerPlayer* bechallengePlayer,int resultint);
public:
	static void SummonPlayerFinish(LC_ServerPlayer* player);
	void Settle_GuildOffline();
private:
	bool CheckOwnerDead( LC_ServerPlayer* pPlayer, object_id_type& nDeadPlayerID );
	void InitBattle_Orig( LC_ServerPlayer* pPlayerAController, LC_ServerPlayer* pPlayerBController );
	void Settle_Arena();
	void PreSettle_GuildOffline();
	void InitHatred( LC_ServerPlayer* pPlayerAController, LC_ServerPlayer* pPlayerBController );
	void InitBattle( LC_ServerPlayer* pPlayerAController, LC_ServerPlayer* pPlayerBController );
private:
	LC_Dule_Config*		m_Config;
	SoloDule_State		m_State;

	GameLogic::SOLO_DULE_TYPE m_eSoloType;
	
	object_id_type		m_ChallengePlayerID;
	unique_id_impl		m_BeChallengeCharID;
	LC_ServerPlayer*	m_BeChallengePlayer;
	int					m_BeChallengeIndex;
	int					m_SoloPrePareTime;
	int					m_SoloBattleTime;
	int64_t				m_isolofightuniqueId;
	float				m_TimeStamp;
	//´æ´¢Íæ¼Òbuff
	std::string			m_PlayerBuffInfo;
};
#endif