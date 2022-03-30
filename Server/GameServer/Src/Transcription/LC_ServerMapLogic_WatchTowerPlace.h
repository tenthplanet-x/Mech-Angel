#pragma once

#include "LC_ServerMapLogic_Place.h"
#include "LC_Server_Guild_Common.h"
class LC_ServerPlayer;
class LC_ServerMapLogic_WatchTowerPlace : public LC_ServerMapLogic_Place
{
public:
	LC_ServerMapLogic_WatchTowerPlace();
	~LC_ServerMapLogic_WatchTowerPlace();

	virtual void			Init();
	virtual void			UnInit();
	void						Clear();
	virtual void			Update(float fCurrentTime, float fDeltaTime);
	virtual void			SetWatchTowerState(int storyID, int bossMapID, int towerID, int bossID, int state, int vaildTime, int endTimerID);
	virtual void			SetTowerLocation(int storyID, int towerID, int x, int y, int z);
	virtual uint64_t		GetBossGuildID(int storyID, int npcID);
	virtual int				GetTowerID(int storyID, int npcID);
	virtual void			SetDamageNpcLogicID(int logicID);
	virtual int				GetDamageNpcLogicID();
	virtual bool			CheckAddKillControllerReward(int mapID);
	virtual void			BroadcastTowerInfo_Lite(ST_Single_Tower_info_lite& input);
	virtual void			BroadcastWatchTowerInfo();
	virtual void			SyncWatchTowerInfoToPlayer(LC_ServerPlayer * pkPlayer);
	virtual void			SetLoadFlag();
	virtual bool			GetLoadFlag();
	virtual bool			SetMonsterHP(int charType, int64_t hp, int32_t nMapResId);
	virtual void			UpdateWatchTowerFromSaveState(uint64_t id, std::string str);
	//非global节点，向redis请求哨塔数据
	virtual void			UpdateWatchTowerFromRedis();
	
	
	void			UpdateStateToRedis(uint64_t id);
	void			SetMonsterGuildID(int mapID, int monsterID, uint64_t guildID);
	void			SetMonsterCurrentHP(int mapID, int monsterID, int hp);
private:
	bool m_bSettle;
	int m_nDamageNpcLogicID;
	bool m_nLoadFlag;
	WatchTowerGuildInfoMap m_nBestGuildInfo;
	bool m_nFirstLoad;
};