#ifndef _LC_NEW_VIP_REWARD_H
#define _LC_NEW_VIP_REWARD_H

#include <map>
#include "CF_VipNew.h"
#include "SystemError.h"
#include "char_data.pb.h"
#include "MG_GameProtocol.h"

namespace GameLogic
{
	class LC_PlayerBase;
}

class LC_ServerPlayer;


class LC_NewVIPReward
{
public:
	LC_NewVIPReward();
	~LC_NewVIPReward();

	void Init( object_id_type nObjId );
	void Update( float fCurrent, float fDelta);
	void ReUse();
	void SetLevel( int32_t nLevel );
	void LoadLevel( int32_t nLevel );
	ResultType TryFetchBonus();
	int FetchNormalBonus( int nSeconds, int nType );

	void Save(char_data_define::new_vip_reward & detail_data);
	void Load(const char_data_define::new_vip_reward & detail_data);
	void SyncToClient();

	void ResetBeginTime(uint32_t beginTime);
	uint32_t GetBeginTime();

	void ResetTimer_Normal(int32_t index, uint32_t time);
	uint32_t GetTimer_Normal(int32_t index);

	void Enable(uint32_t time);
	ResultType GetVIPExtraReward(std::vector<Protocol::PS_ItemRecord>& itemInfo);

	void FillBonusCache(Protocol::MG_SC_NewVIPBonusCacheSync& msg);
	bool CheckUpdateBonus();
private:
	int UpdateReward(float fCurrent, float fDelta);
	int OnUpdate_Normal( int nCount, int nType );
	int TryUpdate_Normal(  uint32_t nNow, uint32_t nMaxTimer, int nType );
	int CheckUpdate( uint32_t nNow );
	int CheckUpdateBonus( uint32_t nBeginTime );
	LC_ServerPlayer* GetOwnerPlayer();
	uint32_t GetNormalBonusTimerByType( int nType );
	int DoNormalReward( LC_ServerPlayer* pOwner,  int nCount, bool bSimple = false, int nType = -1 );
	void ClearBonusCache();
	void SyncBonusCache();
	int GetNormalRewardByType(LC_ServerPlayer* pOwner, int32_t nType, uint32_t& nItemId, uint32_t& nCount, const CSVFile::CF_VipNew::DataEntry* pCfg  );
	void ModifyBonusCache_Simple( uint32_t nItemId, int32_t nCount );
	void ModifyBonusCache( uint32_t nItemId, int32_t nCount );
	void SyncTimeStamp();
	int DoFetchBonus( LC_ServerPlayer* pOwner, ResultType& resType );
	int DoFetchBonus_Normal( LC_ServerPlayer* pOwner );
	int DoFetchBonus_Simple( LC_ServerPlayer* pOwner );
	void ResetBonusTimes( uint32_t nBeginTime );
	void DoResetBonusTimes( uint32_t nBeginTime, uint32_t& nOrigTime, uint32_t nTimer );
	void InitNormalBonusTime( uint32_t nTime, bool bForce = false  );
private:
	object_id_type m_nOwnerId;
	int32_t m_nVIPLevel;
	mem::map<uint32_t, uint32_t> m_bonusCache;
	mem::map<uint32_t, uint32_t> m_bonusCache_Simple;
	uint32_t m_nBeginTime;
	uint32_t m_nTimer_Normal[eNormalBonus_Count];
	uint32_t m_nLastBonusTimer;
};


#endif	