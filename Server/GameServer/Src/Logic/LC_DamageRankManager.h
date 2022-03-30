#ifndef _LC_DAMAGERANK_MAMAGER_H_
#define _LC_DAMAGERANK_MAMAGER_H_

#include "LC_Define.h"
#include "IdDefine.h"
#include "UT_Singleton.h"
#include "SystemError.h"
#include "MG_ProtocolStruct.h"

class LC_ServerPlayer;
struct PlayerData
{
	PlayerData() : value(0), name("") {}

	uint64_t value;
	string name;
};

struct SingleContainer
{
	SingleContainer() : m_bSort(false), m_nTotalValue(0) 
	{
		m_nValue.clear(); 
		m_kSortList.clear(); 
	}

	void				Clear();
	void				UpdateValue(uint64_t key, const string& name, uint64_t detalValue);
	uint64_t		GetValue(uint64_t key);
	ResultType	GetRankIndex(uint64_t key, int32_t& keyIndex, uint64_t& value);
	ResultType	FillRankDetailInfo(uint64_t key, Protocol::PS_PlayerInfo& selfInfo, std::vector<Protocol::PS_PlayerInfo>& playerInfo, int num);
	void				DoSort();

	bool												m_bSort;//是否已经排序
	uint64_t										m_nTotalValue;//公会总伤害
	std::map<uint64_t, PlayerData>	m_nValue;//key:玩家的单独伤害
	uint64_object_id_vector				m_kSortList;//按照玩家的伤害，排序的玩家id
};

struct SingleContainerCompare
{
	SingleContainerCompare(SingleContainer& data) : m_data(data){}

	bool operator()(uint64_t aID, uint64_t bID)
	{
		uint64_t value1 = m_data.GetValue(aID);
		uint64_t value2 = m_data.GetValue(bID);
		return value1>value2;
	}

	SingleContainer m_data;
};

typedef mem::map<uint64_t, SingleContainer> EncountedStatMap;
typedef EncountedStatMap::iterator EncountedStatMapIter;

struct EncountedSortContainer
{
	EncountedSortContainer() : m_bSort(false)
	{
		m_kSortList.clear(); 
		m_kStatMap.clear();
		m_kStatValue.Clear();
	}

	void Clear();

	void				UpdateValue(uint64_t key1, uint64_t key2, const string& name, uint64_t value);
	uint64_t		GetValue(uint64_t key1);
	uint64_t		GetValue(uint64_t key1, uint64_t key2);
	uint64_t		GetTotalValue(uint64_t key);
	ResultType	GetRankIndex(uint64_t key1, uint64_t key2, int32_t& key1RankIndex, uint64_t& key1Value, int32_t& key2RankIndex, uint64_t& key2Value);
	ResultType	FillRankDetailInfo(uint64_t key1, uint64_t key2, Protocol::PS_PlayerInfo& selfInfo, std::vector<Protocol::PS_PlayerInfo>& playerInfo);

	void				DoSort();

	bool										m_bSort;//是否已经排序
	uint64_object_id_vector		m_kSortList;
	EncountedStatMap				m_kStatMap;

	SingleContainer					m_kStatValue;
};

struct EncountedSortContainerCompare
{
	EncountedSortContainerCompare(EncountedSortContainer& data) : m_data(data){}

	bool operator()(uint64_t aID, uint64_t bID)
	{
		uint64_t value1 = m_data.GetValue(aID);
		uint64_t value2 = m_data.GetValue(bID);
		return value1>value2;
	}

	EncountedSortContainer m_data;
};

class CDamageRankManager : public Memory::MM_BaseObject, public Utility::UT_Singleton<CDamageRankManager>
{
public:
	CDamageRankManager();
	~CDamageRankManager();

	void Clear();
	void Init();
	void Unit();

	void				UpdatePlayerDamage(int32_t mapID, int64_t guildID, uint64_t playerID, const string& name, uint64_t detalDamage);
	uint64_t		GetPlayerDamage(int32_t mapID, int64_t guildID, uint64_t playerID);
	uint64_t		GetGuildTotalDamage(int32_t mapID, int64_t guildID);
	ResultType	FillRankIndex(int32_t mapID, uint64_t guildID, uint64_t playerID, int32_t& selfGuildRankIndex, uint64_t& selfGuildTotalDamage, int32_t& selfRankIndex, uint64_t& selfTotalDamage);
	ResultType	FillGuildDamageRankInfo(int32_t mapID, LC_ServerPlayer* pkOwnerPlayer, Protocol::PS_PlayerInfo& selfInfo, std::vector<Protocol::PS_PlayerInfo>& playerInfo);
protected:

private:
	std::map<int32_t, EncountedSortContainer> m_EncountedSortContainer;
};

#endif
