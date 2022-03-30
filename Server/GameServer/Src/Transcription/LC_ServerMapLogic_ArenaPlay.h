#ifndef _LC_SERVERMAPLOGIC_ARENA_PLAY_H
#define _LC_SERVERMAPLOGIC_ARENA_PLAY_H

#include "LC_Dule_Config.h"
#include "LC_ServerMapLogic_PlayBase.h"
#include "LC_Arena_Manager.h"
using namespace GameLogic;
using namespace DungeonConfig;

enum Arena_State
{
	Arena_State_None=0,
	Arena_State_Inited=1,
	Arena_State_Prepare=2,
	Arena_State_Loading=3,
	Arena_State_CountDown=4,
	Arena_State_Battle=5,
	Arena_State_End=6
};

class LC_ServerMapLogic_ArenaPlay : public LC_ServerMapLogic_PlayBase
{
public:
	DECL_RTTI_NAME( LC_ServerMapLogic_ArenaPlay )
public:
	LC_ServerMapLogic_ArenaPlay();
	virtual ~LC_ServerMapLogic_ArenaPlay();
	bool Init(LC_ServerMapLogic_Place* place, LC_Dule_Config* config);
	virtual void UnInit();
	virtual bool Start();
	virtual void End();
	virtual bool Update(float fCurrentTime, float fDeltaTime);
	virtual ResultType CheckEnter(LC_ServerPlayer* player);
	virtual void EventNotify(MapLogicEvent& kEvent);
	virtual void PlayerEventNotify(LC_ServerPlayer* player,MapLogicEvent& kEvent);
	virtual bool CheckEnd();
	virtual bool CheckRun();
	void Settle();

	void ReserveArenaInfo(const Arena_Info& rkArena);
	void ChallengePlayerActionForbid(LC_ServerPlayer* player);
	void ChallengePlayerActionUnForbid(LC_ServerPlayer* player);
	void Prepare();
	void Loading();
	void CountDown();
	void Battle();
	void Reset();
	void BattleResult();

	inline int32_t State(void) const {return m_State;}

	LC_ServerPlayer* FindPlayer(const unique_id_impl& charID);
	void SyncArenaStatus();
public:
	void checkLoaded(void);
	void checkCountdown(void);
	static void SummonPlayerFinish(LC_ServerPlayer* player);
protected:
private:
	LC_Dule_Config*		m_Config;
	Arena_State			m_State;
	
	int64_t				m_ArenaID;
	ArenaCharMap		m_Chars;
	
	int					m_PrePareTime;
	int					m_BattleTime;
	float				m_TimeStamp;
};
#endif