#ifndef LC_SERVER_GUILD_COMMON_H
#define LC_SERVER_GUILD_COMMON_H

#include "StringDefine.h"
#include <map>
#include "LC_Define.h"
#include "MG_Game2Alliance.h"
#include "MG_CS_Guild.h"
#include "LC_ServerGuildManager.h"
//enum WatchTowerType
//{
//	WatchTower_Protocted,//未开启争夺
//	WatchTower_AllKilled,//开启争夺
//	WatchTower_Temp,//临时占领
//	WatchTower_Owner,//确认占领
//};

 
using namespace Protocol;
struct WatchTowerGuildInfo 
{
	uint64_t	m_Id;				// 高32位 storyId  低32位怪物id  //哨塔id
	uint64_t	m_GuildId;				//公会id
	int			m_MonsterId;
	int64_t			m_nMonsterHP;
	int			m_nState;				//状态 / 未开启状态 开启争夺 临时占领 确认占领
	//std::string m_sGuildLeaderName;	// 公会会长名字
	std::vector<std::string> m_Top3DamagePlayerName;//伤害前3名玩家的名字

	//PS_Damage_PlayerInfo            m_nGuildLeaderInfo;//公会会长信息
	std::string m_nPlayerName;//公会会长名字
	int32_t		m_nLevel;//公会会长等级
	int32_t		m_nTitleID;//公会会长头衔id
	int32_t		m_nProfilePictureID;//公会会长头像id
	int32_t		m_nCommanderTitleFrameID;//公会会长头像框id

	std::string m_sGuildName;		// 公会名字
	std::vector<uint64_t> m_nDamagePlayerID;//公会伤害玩家排名
	int m_nEndTimerID;//临时占领状态结束的timerID
	int m_x;
	int m_y;
	int m_z;
	int m_MapId;

	//uint32_t m_SafeTimeStamp;		// 安全期（未开启时间戳） 争夺中 此时间戳无效 （保护时间一到 立马清除 m_GuildId 状态切换为 开启争夺）
	//uint32_t m_TempTimeStamp;		// 临时占领 时间戳  本公会人员不允许攻击。临时占领时间一结束， 立马刷新  新的安全期m_SafeTimeStamp
	uint32_t m_NextTimeStamp;		// 下一个状态时间戳
	uint32_t m_RewardTimeStamp;		//占领期 下一次发放奖励时间戳

	WatchTowerGuildInfo():m_Id(0),m_GuildId(0),m_nState(0), m_MonsterId(0), m_RewardTimeStamp(0),m_NextTimeStamp(0),m_nEndTimerID(0),m_x(0),m_y(0),m_z(0),m_MapId(0)
		, m_nMonsterHP(0), m_nLevel(0), m_nTitleID(0), m_nProfilePictureID(0), m_nCommanderTitleFrameID(0)
	{	
		m_sGuildName.clear();
		m_nDamagePlayerID.clear();
		m_Top3DamagePlayerName.clear();
		m_nPlayerName.clear();
	}
	WatchTowerGuildInfo(int nStoryID,int nTowerId );
	//新增活动
	//void CreateNew(int nStoryID,int nTowerId ,uint32_t uStartTimeStamp);
	void InitId(uint64_t Id){m_Id = Id;}
	void InitXYZ(int x,int y,int z){m_x = x; m_y = y; m_z = z;};
	void init();
	void Clear();
	uint32_t GetTowerID();
	uint32_t GetStoryID();
	uint64_t GetGuildID(){return m_GuildId;}
	//bool IsAllowAttack(uint32_t GuildId);
	//void SetSafeTime(uint32_t FutureTimeStamp){ m_SafeTimeStamp = FutureTimeStamp;}
	void EntryTempOwer(uint64_t GuildId,std::string & sGuildName);
	void SetGuildLeaderInfo(GuildServerMember* pkMaster);
	void UpdateGuildDamagePlayerInfo(GuildServerMember* pkMaster);

	void SetMonsterHP(int64_t hp);

	void ClearTempOwer();
	//void CheckTimeStamp(uint32_t CurrentTimeStamp);
	//void CheckSafeTimeStamp(uint32_t CurrentTimeStamp);
	//void CheckTempTimeStamp(uint32_t CurrentTimeStamp);
	void CheckSendOwerReward();
	void SetState(int nState,int nMapId,int nMonsterId,uint32_t uNextTimeStamp = 0);
	int GetMapId(){return m_MapId;}
	int GetState(){return m_nState;}
	int GetMonsterCharTypeID(){return m_MonsterId;}
	void UpdateGuildName(std::string &sGuildName,std::string &sGuildLeaderName);
	
	void SendOfflineReward(int vaildTime, bool updateTime=false);
	//void SendSingleReward();
	
	//void UpdateTowerIdToGuild(bool Add = true);

	void ClearDamageInfo();
	void UpdateDamagePlayerID(unique_id_type playerID);
	const std::vector<uint64_t>& GetDamagePlayerVector();
	void PackTowerInfo(ST_Single_Tower_info & info);
	int GetTimerID();
	void SetTimerID(int timerID);

	void SendDamageReward();
	//void LoadWatchTowerToRedis();
	//void SaveWatchTowerToRedis();
};
AJSON(WatchTowerGuildInfo, (m_Id)(m_GuildId)(m_MonsterId)(m_nMonsterHP)(m_nState)(m_Top3DamagePlayerName)(m_nPlayerName)(m_nLevel)(m_nTitleID)(m_nProfilePictureID)(m_nCommanderTitleFrameID)(m_sGuildName)(m_nDamagePlayerID)(m_x)(m_y)(m_z)(m_MapId)(m_NextTimeStamp)(m_RewardTimeStamp))


typedef mem::map<uint64_t, WatchTowerGuildInfo > WatchTowerGuildInfoMap;
#endif
