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
//	WatchTower_Protocted,//δ��������
//	WatchTower_AllKilled,//��������
//	WatchTower_Temp,//��ʱռ��
//	WatchTower_Owner,//ȷ��ռ��
//};

 
using namespace Protocol;
struct WatchTowerGuildInfo 
{
	uint64_t	m_Id;				// ��32λ storyId  ��32λ����id  //����id
	uint64_t	m_GuildId;				//����id
	int			m_MonsterId;
	int64_t			m_nMonsterHP;
	int			m_nState;				//״̬ / δ����״̬ �������� ��ʱռ�� ȷ��ռ��
	//std::string m_sGuildLeaderName;	// ����᳤����
	std::vector<std::string> m_Top3DamagePlayerName;//�˺�ǰ3����ҵ�����

	//PS_Damage_PlayerInfo            m_nGuildLeaderInfo;//����᳤��Ϣ
	std::string m_nPlayerName;//����᳤����
	int32_t		m_nLevel;//����᳤�ȼ�
	int32_t		m_nTitleID;//����᳤ͷ��id
	int32_t		m_nProfilePictureID;//����᳤ͷ��id
	int32_t		m_nCommanderTitleFrameID;//����᳤ͷ���id

	std::string m_sGuildName;		// ��������
	std::vector<uint64_t> m_nDamagePlayerID;//�����˺��������
	int m_nEndTimerID;//��ʱռ��״̬������timerID
	int m_x;
	int m_y;
	int m_z;
	int m_MapId;

	//uint32_t m_SafeTimeStamp;		// ��ȫ�ڣ�δ����ʱ����� ������ ��ʱ�����Ч ������ʱ��һ�� ������� m_GuildId ״̬�л�Ϊ �������ᣩ
	//uint32_t m_TempTimeStamp;		// ��ʱռ�� ʱ���  ��������Ա������������ʱռ��ʱ��һ������ ����ˢ��  �µİ�ȫ��m_SafeTimeStamp
	uint32_t m_NextTimeStamp;		// ��һ��״̬ʱ���
	uint32_t m_RewardTimeStamp;		//ռ���� ��һ�η��Ž���ʱ���

	WatchTowerGuildInfo():m_Id(0),m_GuildId(0),m_nState(0), m_MonsterId(0), m_RewardTimeStamp(0),m_NextTimeStamp(0),m_nEndTimerID(0),m_x(0),m_y(0),m_z(0),m_MapId(0)
		, m_nMonsterHP(0), m_nLevel(0), m_nTitleID(0), m_nProfilePictureID(0), m_nCommanderTitleFrameID(0)
	{	
		m_sGuildName.clear();
		m_nDamagePlayerID.clear();
		m_Top3DamagePlayerName.clear();
		m_nPlayerName.clear();
	}
	WatchTowerGuildInfo(int nStoryID,int nTowerId );
	//�����
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
