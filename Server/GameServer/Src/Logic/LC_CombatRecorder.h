#ifndef _LC_COMBAT_RECORDER_H
#define _LC_COMBAT_RECORDER_H


namespace GameLogic
{
	class LC_PlayerBase;
}

class LC_ServerPlayer;
class LC_ServerNPC;


class LC_CombatRecorder
{
public:
	LC_CombatRecorder();
	~LC_CombatRecorder();

	void Init( object_id_type nObjId, int nIndex );
	void ReUse();
	bool GetValid();
	void Enable();
public:
	void AddValue( uint64_t nInput );
	uint64_t GetValue() const;
	object_id_type GetOwnerId();
	void Restart();
private:
	uint64_t m_nValue;
	object_id_type m_nOwnerId;
	bool m_bValid;
	int m_nIndex;
};


#endif	