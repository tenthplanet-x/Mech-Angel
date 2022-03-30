#ifndef _LC_SERVER_MAP_LOGIC_BASE_H
#define _LC_SERVER_MAP_LOGIC_BASE_H

#include "LC_SpawnManagerBase.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerMap.h"
#include "LC_ServerMapLogic_MonsterConfig.h"
#include "time.h"
#include "LC_MapLogicBase.h"
#include "CF_WorldMapList.h"

#define BAOHU_BUFF 39120002


void GenRandomPosition(const Utility::UT_Vec3Int& pkReferencePos,mem::vector<Utility::UT_Vec3Int>& posVec,int count,int resId=-1);
struct PlayerRespawnData
{
	int m_PlayerId;
	int m_DeadTime;
	int m_CurRespawnTime;
	int m_SafeRespawnTime;
	time_t m_TimeStamp;
};
class LC_ServerMapLogicBase : public GameLogic::LC_MapLogicBase
{
public:
	typedef mem::set<int> EliteMap;

	LC_ServerMapLogicBase();
	~LC_ServerMapLogicBase(void);
	DECL_RTTI_NAME(LC_ServerMapLogicBase)
	virtual void				Update(float fCurrentTime,float fDeltaTime);
	virtual void				UnInit();
	virtual void				Init();
	
	//计算玩家进入的坐标值
	virtual Utility::UT_Vec3Int	ComputeEnterLocation (GameLogic::LC_PlayerBase* pkPlayer,GameLogic::LC_MapInOutType eInOutType, const Utility::UT_Vec3Int& rkTargetLocation);
	virtual ResultType			RequestChangeLocation(GameLogic::LC_PlayerBase* pkPlayer,GameLogic::LC_MapInOutType eInOutType,const Utility::UT_Vec3Int& rkTargetLocation);
	//进入地图
	virtual ResultType			RequestEnterMap(GameLogic::LC_PlayerBase* pkPlayer,GameLogic::LC_MapInOutType eInOutType,const Utility::UT_Vec3Int& rkTargetLocation);
	//离开地图
	virtual ResultType			RequestLeaveMap(GameLogic::LC_PlayerBase* pkPlayer,GameLogic::LC_MapInOutType eInOutType, CSVFile::CF_WorldMapList::DataEntry* pkDataEntry);
	virtual void				PlayerChangeLogicArea(LC_ServerPlayer* player,int prearea,int curarea);

	//Deal with NPC Dead,for example AwardExperience
	virtual void				NPCDead(LC_ServerNPC* pkServerNPC);
	virtual void				DropItemsOnNPCDead(LC_ServerNPC* pkServerNPC);
	//Dealwith player dead
	virtual void				PlayerBeKilled(LC_ServerPlayer* pkServerPlayer, object_id_type lSrcActorID);

	virtual bool RestartLogic();

	inline bool IsActive(void){ return m_bActive;}
	void SetActive(bool flag);
	inline LC_ServerMap* GetServerMap(void){ return (LC_ServerMap*)m_pkMap;}
	inline void SetRecordPK(bool recordpk){ m_RecordPk=recordpk;}
	inline bool GetRecordPK(void){ return m_RecordPk;}

	void BroadcastMsgToClient(uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody=NULL);
	void BroadcastMsgToServerClient(int32_t nServerId, uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody=NULL);

	bool BroadCastGameEventToClient(GameLogic::TranscriptionEventType pkSendType,int64_t lEventValue1 = 0,int64_t lEventValue2 = 0,int64_t lEventValue3 = 0,int64_t lEventValue4 = 0,int64_t lEventValue5 = 0, int64_t lEventValue6 = 0);
	bool NotifyGameEventToClient(LC_ServerPlayer* pkServerPlayer, GameLogic::TranscriptionEventType pkSendType,int64_t lEventValue1 = 0,int64_t lEventValue2 = 0,int64_t lEventValue3 = 0,int64_t lEventValue4 = 0,int64_t lEventValue5 = 0,int64_t lEventValue6 = 0);

	//玩家复活相关
	virtual void UpdatePlayerRespawn(object_id_type playerid);
	PlayerRespawnData* GetPlayerRespawn(object_id_type playerid);
	void GetRespawnTime(object_id_type playerid, int& currespawntime,int& saferespawntime);
	bool CheckPlayerRespawn(object_id_type playerid, GameLogic::LC_MapInOutType eInOutType);

	inline void SetOfflineTime(int offtime){m_offlinetime=offtime;}
	inline int GetOfflineTime(void){return m_offlinetime;}
protected:
	bool m_bActive;
	int	m_offlinetime;
	bool m_RecordPk;
	float m_fLastUpdateInfoTime;
	//玩家复活记录
	mem::map<int,PlayerRespawnData> m_PlayerRespawnMap;
	
	
};

#endif

