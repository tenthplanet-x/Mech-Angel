#ifndef _LC_COMBAT_RECORDER_MANAGER_H_
#define _LC_COMBAT_RECORDER_MANAGER_H_

#include "LC_CombatRecorder.h"

#include "char_data.pb.h"

namespace GameLogic
{
	class LC_PlayerBase;
}

#define			MAX_COMBAT_RECORDER_COUNT		2

class LC_ServerPlayer;
class LC_ServerNPC;

class LC_CombatRecorderManager
{
public:
	LC_CombatRecorderManager();
	~LC_CombatRecorderManager();
public:
	void Init( object_id_type nObjId );
	void ReUse();
	object_id_type GetOwnerId();
	void Enable(int32_t nMapType);
	void Disable();
	bool CheckEnable();
	const LC_CombatRecorder& GetReadData();
	const LC_CombatRecorder& GetWriteData();
	const LC_CombatRecorder& GetMaxData();
	void FinishRecord();
	void ResetDataIndex();
	bool AddValue( uint64_t nInput, bool checkState=true);
	int32_t GetRank();
	void SetRank( int32_t nRank );
	void SetDataKey( int32_t nKey );
	int32_t GetDataKey();
	int32_t GetMapType();

	void SetYiZhongQinXiDamage(uint64_t data=0);
	uint64_t GetYiZhongQinXiDamage();

	void Save(char_data_define::combat_recorder & data);
	void Load(const char_data_define::combat_recorder & data);
private:
	LC_CombatRecorder m_RecorderData[MAX_COMBAT_RECORDER_COUNT];
	int m_nCurrentWriteIndex;
	int m_nCurrentReadIndex;
	object_id_type m_nOwnerId;
	int m_nStatus;
	int32_t m_nMapType;
	int32_t m_nRank;
	int32_t m_nDataKey;

	uint64_t m_nYiZhongQinXiDamage;//最后一次异种侵袭的伤害
};

#endif	