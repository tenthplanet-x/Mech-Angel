#include "LC_ServerRewardManager.h"
#include "LC_ServerPlayerGroup.h"
#include "LC_ServerPlayerGroupManager.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerNPC.h"
#include "GameServerApp.h"
#include "CF_ItemGenMonsterDrop.h"
#include "CF_ItemGenMonsterDropFunc.h"
#include "CF_ItemGenEffect.h"
#include "CF_ItemGenFunc.h"
#include "CF_ItemGen.h"
#include "CF_ItemEffect.h"
#include "LC_ServerPlaceHolder.h"
#include "LC_ServerPlaceHolderManager.h"
#include "LC_ServerPackEntryFactory.h"
#include "LC_ServerItem.h"
#include "LC_ServerNPCManager.h"
#include "CF_AutoFitCharType.h"

#include "CF_Helper.h"
#include "OSSDefine.h"
#include "oss_define.h"

using namespace Utility;
using namespace GameLogic;
using namespace CSVFile;

//--------------------------------------------------------
LC_ServerRewardManager::LC_ServerRewardManager()
{
	m_RewardGenManager_.Init();
}
//--------------------------------------------------------
LC_ServerRewardManager::~LC_ServerRewardManager()
{
	m_RewardGenManager_.UnInit();
}
//--------------------------------------------------------
void RewardGenManager::Init()
{
	{
		CF_ItemGenEffect * inst = CF_ItemGenEffect::GetSingletonPtr();
		{
			CF_ItemGenEffect::DataEntryMapExternIterator iter = inst->GetIterator();
			while(iter.HasMoreElements())
			{
				const CF_ItemGenEffect::DataEntry& entry = iter.Next();
				EquipChoiceEffectType * the_info = GetEquipEffectGenFunc(entry._lItemDropGenID);
				if(NULL == the_info)
				{
					EquipChoiceEffectType the_new;
					m_EquipEffectInfos_.insert(std::make_pair(entry._lItemDropGenID,the_new));
					the_info = GetEquipEffectGenFunc(entry._lItemDropGenID);
				}
				EquipEffectGenEntry func_entry;
				func_entry.EffectTypeID = entry._lItemEffectID;
				func_entry.Choice = entry._lChoice;
				the_info->push_back(func_entry);
			}
		}
	}
	{
		CF_ItemGen * inst = CF_ItemGen::GetSingletonPtr();
		{
			CF_ItemGen::DataEntryMapExternIterator iter = inst->GetIterator();
			while(iter.HasMoreElements())
			{
				const CF_ItemGen::DataEntry& entry = iter.Next();
				if(entry.GenEffect1>0)
				{
					if(NULL == GetEquipEffectGenFunc(entry.GenEffect1))
					{
						GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "ItemGen.csv gets foreign data ERROR: In ItemGenEffect.csv _lID==%ld GenEffect1==%ld is not found!", entry._iID , entry.GenEffect1);
					}
				}				
				if(entry.GenEffect2>0)
				{
					if(NULL == GetEquipEffectGenFunc(entry.GenEffect2))
					{
						GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "ItemGen.csv gets foreign data ERROR: In ItemGenEffect.csv _lID==%ld GenEffect2==%ld is not found!", entry._iID , entry.GenEffect2);
					}
				}				
				if(entry.GenEffect3>0)
				{
					if(NULL == GetEquipEffectGenFunc(entry.GenEffect3))
					{
						GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "ItemGen.csv gets foreign data ERROR: In ItemGenEffect.csv _lID==%ld GenEffect3==%ld is not found!", entry._iID , entry.GenEffect3);
					}
				}				
				if(entry.GenEffect4>0)
				{
					if(NULL == GetEquipEffectGenFunc(entry.GenEffect4))
					{
						GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "ItemGen.csv gets foreign data ERROR: In ItemGenEffect.csv _lID==%ld GenEffect4==%ld is not found!", entry._iID , entry.GenEffect4);
					}
				}				
				if(entry.GenEffect5>0)
				{
					if(NULL == GetEquipEffectGenFunc(entry.GenEffect5))
					{
						GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "ItemGen.csv gets foreign data ERROR: In ItemGenEffect.csv _lID==%ld GenEffect5==%ld is not found!", entry._iID , entry.GenEffect5);
					}
				}				
				if(entry.GenEffect6>0)
				{
					if(NULL == GetEquipEffectGenFunc(entry.GenEffect6))
					{
						GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "ItemGen.csv gets foreign data ERROR: In ItemGenEffect.csv _lID==%ld GenEffect6==%ld is not found!", entry._iID , entry.GenEffect6);
					}
				}				
				if(entry.GenEffect7>0)
				{
					if(NULL == GetEquipEffectGenFunc(entry.GenEffect7))
					{
						GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "ItemGen.csv gets foreign data ERROR: In ItemGenEffect.csv _lID==%ld GenEffect7==%ld is not found!", entry._iID , entry.GenEffect7);
					}
				}				
				if(entry.GenEffect8>0)
				{
					if(NULL == GetEquipEffectGenFunc(entry.GenEffect8))
					{
						GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "ItemGen.csv gets foreign data ERROR: In ItemGenEffect.csv _lID==%ld GenEffect8==%ld is not found!", entry._iID , entry.GenEffect8);
					}
				}				
			}
		}

	}
	{
		CF_ItemGenFunc * inst = CF_ItemGenFunc::GetSingletonPtr();
		{
			CF_ItemGenFunc::DataEntryMapExternIterator iter = inst->GetIterator();
			while(iter.HasMoreElements())
			{
				const CF_ItemGenFunc::DataEntry& entry = iter.Next();
				RewardGenFunc * the_info = GetGenFunc(entry._lItemDropGenID);
				if(NULL == the_info)
				{
					RewardGenFunc the_new;
					m_FuncInfos_.insert(std::make_pair(entry._lItemDropGenID,the_new));
					the_info = GetGenFunc(entry._lItemDropGenID);
				}
				RewardGenFuncEntry func_entry;
				func_entry.Choice = entry._lChoice;
				func_entry.Type = entry._lType;
				func_entry.Context = entry._lDropContex;
				func_entry.Count = entry._lDropCount;
				func_entry.BindMode = entry._lBindMode;
				func_entry.MFModify = entry._lMFModify;
				func_entry.LifeTime = entry._lLifeTime;
				the_info->m_infos.push_back(func_entry);
				if(1 == func_entry.Type)
				{
					CF_ItemGen * inst = CF_ItemGen::GetSingletonPtr();
					{
						CF_ItemGen::DataEntry * the_entry = inst->GetEntryPtr(func_entry.Context);
						if(NULL == the_entry)
						{
							GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "ItemGenFunc.csv gets foreign data ERROR: In ItemGen.csv _lItemDropGenID==%ld _lDropContex==%ld is not found!", entry._lItemDropGenID , entry._lDropContex);
						}
					}
				}
			}
			for(GenFuncMap::iterator i = m_FuncInfos_.begin(); i != m_FuncInfos_.end(); ++i )
			{
				RewardGenFunc& entry = i->second;
				std::sort(entry.m_infos.begin(),entry.m_infos.end());
			}
		}
	}
	{
		CF_ItemGenMonsterDropFunc * inst = CF_ItemGenMonsterDropFunc::GetSingletonPtr();
		{
			CF_ItemGenMonsterDropFunc::DataEntryMapExternIterator iter = inst->GetIterator();
			while(iter.HasMoreElements())
			{
				const CF_ItemGenMonsterDropFunc::DataEntry& entry = iter.Next();				
				RewardGenGroup * the_info = GetGenGroup(entry._lDropPackID);				
				if(NULL == the_info)
				{
					RewardGenGroup the_new;
					m_GroupInfos_.insert(std::make_pair(entry._lDropPackID,the_new));
					the_info = GetGenGroup(entry._lDropPackID);
				}
				
				const Utility::UT_SIMDataList& dropInfo = entry._lDrop;
				for(Utility::UT_SIMDataList::const_iterator it=dropInfo.begin(); it!=dropInfo.end(); ++it)
				{
					the_info->AddEntry(it->ParamB(),it->ParamA(), it->ParamC(), it->IID(), it->ParamD());
					if( NULL == GetGenFunc(it->IID()))
					{
						GfxWriteLog( LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "ItemGenMonsterDropFunc.csv gets foreign data ERROR: In ItemGenFunc.csv _lDropPackID==%ld _lDropGenID==%ld is not found!", entry._lDropPackID , it->IID());
					}
				}
			}
		}
	}
}

void RewardGenManager::UnInit()
{

}

typedef mem::set<int>   effect_set_type;
typedef mem::map<int,effect_set_type> has_effect_type;

bool GenEquipEffect(int EffectGenGroupID , ItemEffect *& item_effect , Protocol::EquipAttrib& attrib ,RewardGenManager& mgr , has_effect_type& effects)
{
	if(0 >= EffectGenGroupID)
	{
		return false;
	}
	EquipChoiceEffectType * the_effects = mgr.GetEquipEffectGenFunc(EffectGenGroupID);
	if(the_effects)
	{
		effect_set_type * effect_set = NULL;
		has_effect_type::iterator find_has_effect_iter = effects.find(EffectGenGroupID);
		if( find_has_effect_iter == effects.end() )
		{
			effects[EffectGenGroupID] = effect_set_type();
		}
		effect_set = &effects[EffectGenGroupID];
		Utility::Choice<int> the_choice(the_effects->size());
		for( EquipChoiceEffectType::iterator i = the_effects->begin() ; i != the_effects->end(); ++i )
		{
			if( effect_set->find(i->EffectTypeID) == effect_set->end() )
			{
				the_choice.Add(i->Choice, i->EffectTypeID);
			}
		}
		int * effect_id = the_choice.make();
		if(effect_id)
		{
			ItemEffectManage * inst = SafeGetSingleton<ItemEffectManage>();
			ItemEffect * effect = inst->FindEffect(*effect_id);
			if(effect && effect->GetUnits() > 0)
			{
				item_effect = effect;
				attrib.m_TypeID = (uint16_t)(*effect_id);
				attrib.m_Level = effect->ResolveLevel();
				effect_set->insert(attrib.m_TypeID);
				return true;
			}
		}
	}
	return false;
}

bool is_socket_attrib(int attrib_type)
{
	CF_ItemEffect::DataEntry * sentry = SafeGetCSVFileDataEntryPtr<CF_ItemEffect>(attrib_type,true);
	if(sentry->_lItemEffectID == 31)
	{
		return true;
	}
	return false;
}

int GetItemGenEffectByIndex(void* entry , int index )
{
	 CF_ItemGen::DataEntry * gen_entry = (CF_ItemGen::DataEntry *)entry;
	switch(index)
	{
	case 0:
		return gen_entry->GenEffect1;
	case 1:
		return gen_entry->GenEffect2;
	case 2:
		return gen_entry->GenEffect3;
	case 3:
		return gen_entry->GenEffect4;
	case 4:
		return gen_entry->GenEffect5;
	case 5:
		return gen_entry->GenEffect6;
	case 6:
		return gen_entry->GenEffect7;
	case 7:
		return gen_entry->GenEffect8;
	default:
		break;
	}
	return 0;
}

int GetItemTotemEffectByIndex(void* entry, int index)
{
	CF_ItemGen::DataEntry* gen_entry = (CF_ItemGen::DataEntry*)entry;
	if (index >= gen_entry->TotemEffect.size())
	{
		return 0;
	}
	return gen_entry->TotemEffect.at(index);
}
int  LC_ServerRewardManager::_genReWard(int Type, int Context, int Count, int bind_mode, int gf_rate, int life_time, GameLogic::LC_Reward& reward)
{
	reward.SetLifeTime((float)life_time);
	switch(Type)
	{
	case RewardGenFuncEntry::EQUIP:
	case RewardGenFuncEntry::ITEM:
		{
			LC_PackEntryFactory* pkPackEntryFactory = LC_ServerPackEntryFactory::GetSingletonPtr();
			LC_BackPackEntryList rkBackPackEntryList;
			pkPackEntryFactory->CreateItemEntry(Context, Count, CIET_NONE, rkBackPackEntryList, bind_mode);
			if(!rkBackPackEntryList.empty())
			{
				LC_BackPackEntry& entry = rkBackPackEntryList.front();
				reward.SetItem(entry);
				return RE_SUCCESS;
			}
			else
			{
				GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR, "Could not Create ItemTypeID == %d", Context);
				return RE_FAIL;
			}
			break;
		}
	case RewardGenFuncEntry::GOLD:
		{
			if(Context==0 || Count==0)
			{
				return RE_FAIL;
			}
			reward.SetGoldCount(Context, Count);
			return RE_SUCCESS;
			break;
		}
	case RewardGenFuncEntry::BOSSJINGPO:
		{
			if(Context==0 || Count==0)
			{
				return RE_FAIL;
			}
			reward.SetBOSSJPCount(Context, Count);
			return RE_SUCCESS;
			break;
		}
	case RewardGenFuncEntry::RUNECREAM:
		{
			if(Context==0 || Count==0)
			{
				return RE_FAIL;
			}
			reward.SetRuneCreamCount(Context, Count);
			return RE_SUCCESS;
			break;
		}
	case RewardGenFuncEntry::UNBINDYUANBAO:
		{
			if(Context==0 || Count==0)
			{
				return RE_FAIL;
			}
			reward.SetUnbindyuanbaoCount(Context, Count);
			return RE_SUCCESS;
		}
		break;
	default:
		{
			break;
		}
	}
	return RE_FAIL;
}

int	LC_ServerRewardManager::_getRewaredFromPacketID(int mf_rate , int gf_rate , int DropPakcetID ,  RewardTypeList& rward_list)
{
	if (DropPakcetID > 0)
	{
		float mf_effect = UT_MathBase::Sqrt((float)mf_rate * 0.0001f);
		RewardGenGroup * group = m_RewardGenManager_.GetGenGroup(DropPakcetID);
		if(group)
		{
			for(RewardGenGroup::EntryListType::iterator i = group->List.begin() ; i != group->List.end(); ++i)
			{
				RewardGenGroupEntry& group_entry = *i;
				RewardGenFunc * func_entry = m_RewardGenManager_.GetGenFunc(group_entry.GenID);
				if(func_entry)
				{
					size_t nCount = func_entry->m_infos.size();
					Utility::Choice<int> genfunc_choice(nCount);
					long total_old = 0;
					for( std::size_t pos = 0; pos < nCount; ++ pos )
					{
						RewardGenFuncEntry & entry = func_entry->m_infos[pos];
						total_old += entry.Choice;
						int choice = (int)(entry.Choice * ( 1 + (mf_effect*entry.MFModify/10000.0)));
						genfunc_choice.Add(choice,(int)pos);
					}
					for( int c = 0 ; c < group_entry.Count; ++c)
					{
						int chance = UT_MathBase::RandInRangeInt(1,10000);
						if(chance > group_entry.Rand * (1.0f + mf_rate* 0.0001f *group_entry.MFModify))
						{
							continue;
						}
						int * index = genfunc_choice.make(total_old);
						if(index)
						{
							RewardGenFuncEntry * select_func_entry = &func_entry->m_infos[*index];
							LC_Reward reward;
							if(0==_genReWard(select_func_entry->Type,select_func_entry->Context,select_func_entry->Count, select_func_entry->BindMode,gf_rate,select_func_entry->LifeTime,reward))
							{
								rward_list.push_back(reward);
							}
						}
					}
				}
				else
				{
					GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"_getRewaredFromPacketID-error-ItemDropGenID == %d in ItemGenFunc.csv",group_entry.GenID);
				}
			}
		}
		else
		{
			GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"_getRewaredFromPacketID-error-DropPacketID == %d in ItemGenMonsterDropFunc.csv",DropPakcetID);
		}
	}
	return 0;
}

void LC_ServerRewardManager::GenerateRewaredFromDropPacketID(int mf_rate , int gf_rate,int DropPakcetID,  RewardTypeList& rward_list)
{
	_getRewaredFromPacketID(mf_rate,gf_rate,DropPakcetID,rward_list);
}
