#ifndef _LC_SERVER_REWARD_MANAGER_H
#define _LC_SERVER_REWARD_MANAGER_H

#include "UT_Choice.h"
#include "LC_PackStruct.h"
#include "LC_Reward.h"

struct EquipEffectGenEntry
{
	int EffectTypeID;	//属性的类型ID
	int Choice;
};

typedef mem::vector<EquipEffectGenEntry>	EquipChoiceEffectType;

struct RewardGenFuncEntry
{
	enum {
		EQUIP = 1,	//装备
		ITEM = 2,	//道具
		GOLD = 3,	//金币
		UNBINDYUANBAO = 4,//钻石
		BOSSJINGPO = 5,//boss精魄
		//SP = 5,		//真气
		RUNECREAM = 6,//宝具精华
	};
	int Choice;		//选择的权重
	int Type;		//类型
	int Context;
	int Count;
	int BindMode;
	int MFModify;	//MF的修正系数
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

	int	Count;		//参与次数
	int	Rand;		//命中几率，单位是1/10000
	int MFModify;	//MF对其的修正系数
	int	GenID;		//掉落库ID
	int	ItemLevel;	//物品等级
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
