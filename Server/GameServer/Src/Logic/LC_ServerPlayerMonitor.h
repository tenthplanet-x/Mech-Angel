//��������ҹ��������������ͻ�����Ҷ���
#ifndef _LC_SERVER_PLAYER_MONITOR_H
#define _LC_SERVER_PLAYER_MONITOR_H

#include "MM_BaseObject.h"
#include "CF_Rule.h"
#include "LC_Define.h"
#include "UT_MemoryPool.h"
class LC_BehaviorInfo
{
public:
	LC_BehaviorInfo();
	~LC_BehaviorInfo();

public:
	short m_sType;
	float m_fParameter1;
	float m_fTimestamp;
};

class LC_PlayerBehavior  : public Memory::MM_BaseObject
{
public:
	LC_PlayerBehavior();
	~LC_PlayerBehavior();

	enum
	{
		MAX_PLAYER_BEHAVIOR_INFO_COUNT = 128,
	};

	//��������
	void Clear(void);
	//����ID
	void SetID(object_id_type lID);
	//��ȡID
	object_id_type GetID(void);
	//���һ����Ϊ��¼
	bool Insert(const LC_BehaviorInfo& rkInfo);
	//��ȡ��Ϊ�ռ���ʱ����
	float GetBehaviorFirstTime();
	//������ڵ���Ϊ���
	bool Result(float fNow);

private:
	bool _checkRule(CSVFile::CF_Rule::DataEntry* pkRuleData);

	object_id_type m_lPlayerID;
	size_t m_iCount;
	LC_BehaviorInfo m_akBehaviorInfo[MAX_PLAYER_BEHAVIOR_INFO_COUNT];
};

class LC_ServerPlayerMonitor : public Memory::MM_BaseObject
{
public:
	LC_ServerPlayerMonitor();
	virtual ~LC_ServerPlayerMonitor();

	typedef mem::map<uint32_t, LC_PlayerBehavior*>	LC_PlayerBehaviorMap;
	typedef LC_PlayerBehaviorMap::iterator			LC_PlayerBehaviorMapIter;

	LC_PlayerBehavior* CreatePlayer(object_id_type lID);
	void DestroyPlayer(LC_PlayerBehavior* pkPlayer);
	LC_PlayerBehavior* FindPlayer(object_id_type lID);
	void Update(float fNow);

	bool AddOneBehavior(object_id_type lID, const LC_BehaviorInfo& rkInfo);

private:
	LC_PlayerBehaviorMap	m_kPlayerBehaviorMap;
	Utility::UT_MemoryPool<LC_PlayerBehavior>*		m_pkPlayerBehaviorPool;

	float		 m_fLastCheckTime;
};


#endif // _LC_SERVER_PLAYER_MONITOR_H

