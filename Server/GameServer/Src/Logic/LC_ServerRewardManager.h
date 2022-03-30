#ifndef _LC_SERVER_REWARD_MANAGER_H
#define _LC_SERVER_REWARD_MANAGER_H

#include "UT_Choice.h"
#include "LC_PackStruct.h"
#include "LC_Reward.h"

struct EquipEffectGenEntry
{
	int EffectTypeID;	//���Ե�����ID
	int Choice;
};

typedef mem::vector<EquipEffectGenEntry>	EquipChoiceEffectType;

struct RewardGenFuncEntry
{
	enum {
		EQUIP = 1,	//װ��
		ITEM = 2,	//����
		GOLD = 3,	//���
		UNBINDYUANBAO = 4,//��ʯ
		BOSSJINGPO = 5,//boss����
		//SP = 5,		//����
		RUNECREAM = 6,//���߾���
	};
	int Choice;		//ѡ���Ȩ��
	int Type;		//����
	int Context;
	int Count;
	int BindMode;
	int MFModify;	//MF������ϵ��
	int LifeTime;
	RewardGenFuncEntry():Choice(0),Type(0),Context(0),Count(0),BindMode(0),MFModify(0),LifeTime(300){}
	RewardGenFuncEntry(const RewardGenFuncEntry& rv)
		:Choice(rv.Choice),Type(rv.Type),Context(rv.Context),Count(rv.Count),BindMode(rv.BindMode),MFModify(rv.MFModify), LifeTime(rv.LifeTime)
	{}

	inline bool operator < (const RewardGenFuncEntry& rv){return MFModify < rv.MFModify;}
	inline bool operator < (const RewardGenFuncEntry& rv) const{return MFModify < rv.MFModify;	}
};

struct RewardGenFunc
{
	typedef mem::vector<RewardGenFuncEntry> GenFuncEntryListType;
	GenFuncEntryListType m_infos;
};

struct RewardGenGroupEntry
{
	RewardGenGroupEntry()
		:Count(0),Rand(0),MFModify(0),GenID(0),ItemLevel(0)
	{}

	int	Count;		//�������
	int	Rand;		//���м��ʣ���λ��1/10000
	int MFModify;	//MF���������ϵ��
	int	GenID;		//�����ID
	int	ItemLevel;	//��Ʒ�ȼ�
};

struct RewardGenGroup
{
	typedef mem::vector<RewardGenGroupEntry> EntryListType;
	EntryListType	List;

	void AddEntry(int	Count,int	Rand, int MFModify, int	GenID,int	ItemLevel)
	{
		RewardGenGroupEntry entry;
		entry.Count = Count;
		entry.Rand = Rand;
		entry.MFModify = MFModify;
		entry.GenID = GenID;
		entry.ItemLevel = ItemLevel;
		List.push_back(entry);
	}
};

class RewardGenManager
{
public:
	RewardGenManager(){}
	~RewardGenManager(){}

	void Init();

	void UnInit();

	RewardGenGroup * GetGenGroup(int TypeID)
	{
		GenGroupMap::iterator ret = m_GroupInfos_.find(TypeID);
		if(ret != m_GroupInfos_.end())
		{
			return &ret->second;
		}
		return NULL;
	}

	RewardGenFunc * GetGenFunc(int TypeID)
	{
		GenFuncMap::iterator ret = m_FuncInfos_.find(TypeID);
		if(ret != m_FuncInfos_.end())
		{
			return &ret->second;
		}
		return NULL;
	}

	EquipChoiceEffectType * GetEquipEffectGenFunc(int TypeID)
	{
		EquipChoiceEffectMap::iterator ret = m_EquipEffectInfos_.find(TypeID);
		if(ret != m_EquipEffectInfos_.end())
		{
			return &ret->second;
		}
		return NULL;
	}

private:
	typedef mem::map<int,RewardGenGroup> GenGroupMap;
	typedef mem::map<int,RewardGenFunc> GenFuncMap;
	typedef mem::map<int,EquipChoiceEffectType> EquipChoiceEffectMap;

	GenGroupMap m_GroupInfos_;
	GenFuncMap	m_FuncInfos_;
	EquipChoiceEffectMap m_EquipEffectInfos_;
};

typedef mem::vector<GameLogic::LC_Reward> RewardTypeList;
extern int GetItemGenEffectByIndex(void* gen_entry , int index );
extern int GetItemTotemEffectByIndex(void* gen_entry, int index);

class LC_ServerNPC;
class LC_ServerPlayer;
class LC_ServerNPCOwnerList;
class LC_ServerRewardManager : public Memory::MM_BaseObject, public Utility::UT_Singleton<LC_ServerRewardManager>
{
public:
	LC_ServerRewardManager();
	~LC_ServerRewardManager();

	void						GenerateRewaredFromDropPacketID(int mf_rate , int gf_rate ,int DropPakcetID , RewardTypeList& rward_list);

protected:
	int		_getRewaredFromPacketID(int mf_rate , int gf_rate , int DropPakcetID ,  RewardTypeList& rward_list);
	int		_genReWard(int Type, int Context, int Count, int bind_mode, int gf_rate, int life_time, GameLogic::LC_Reward& reward);

	RewardGenManager m_RewardGenManager_;
};

#endif
