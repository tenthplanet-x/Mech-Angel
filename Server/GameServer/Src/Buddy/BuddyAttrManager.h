#ifndef __BUDDY_ATTR_MANAGER_H__
#define __BUDDY_ATTR_MANAGER_H__
#include "LC_ActorAttributeMap.h"
#include "SystemError.h"
#include "IdDefine.h"
#include "LC_Define.h"
#include "SystemDefine.h"
#include "CF_Buddy.h"
#include "LC_PackAsset.h"
#include "BuddyInstanceData.h"




#include <map>
class LC_ServerPlayer;
class BuddyInstance;
class BuddyAttrMgr
{
public:
	BuddyAttrMgr();
	~BuddyAttrMgr();
	void init(CSVFile::CF_Buddy::DataEntry* _pConfig);
	void Clear();
	void RefreshAllAttrMap(LC_ServerPlayer* pkPlayer,BuddyInstance*pBuddy);
	// 出战 属性
	LC_ActorAttributeMap* GetNewFinalAttributeMap(){ return &m_NewFinalAttributeMap; }
	void RefreshNewFinalAttrMap(LC_ServerPlayer* pkPlayer,BuddyInstance*pBuddy, bool login, int32_t ReasonType=SPART_None);
	//获取 本英雄 提供的灵魂锁链属性
	LC_ActorAttributeMap* GetHeroGiveToOtherCoreAttrMap(){ return &m_GiveToOtherCoreAttributeMap; }
	void RefreshHeroGiveToOtherCoreAttrMap(LC_ServerPlayer* pkPlayer,bool bRefresh = true);
	//获取 本英雄 所有属性
	LC_ActorAttributeMap* GetHeroAllAttributeMap(){ return &m_HeroAllAttributeMap; }
	void RefreshHeroAllAttributeMap(LC_ServerPlayer* pkPlayer,BuddyInstance*pBuddy);
	
	//获取 本英雄 获得的灵魂锁链属性
	LC_ActorAttributeMap* GetChainAllCoreAttrMap(){ return &m_GetChainAllAttributeMap; }
	void ClearChainAllAttrMap(LC_ServerPlayer* pkPlayer,BuddyInstance*pBuddy)
	{
		m_GetChainAllAttributeMap.Clear();
	}
	void SetChainAllAttrMap(LC_ServerPlayer* pkPlayer,BuddyInstance*pBuddy,LC_ActorAttributeMap *GetChainAllAttributeMap, bool login, int32_t reasonType)
	{
		m_GetChainAllAttributeMap.Clear(); 
		m_GetChainAllAttributeMap.CopyFrom(GetChainAllAttributeMap);
		m_GetChainAllAttributeMap.RecomputeAll();
		m_GetChainAllAttributeMap.PrintAttrMap("m_GetChainAllAttributeMap");
		RefreshNewFinalAttrMap(pkPlayer,pBuddy, login, reasonType);
	}
	//获取等级 星级提供的属性 
	void GetBaseAttrMap(LC_ActorAttributeMap & attrMap);
	LC_ActorAttributeMap* GetLevelNotBaseAttrMap(){ return &m_LevelAttributeMap; }
	// refreshLevel
	void RefreshLevelAttrMap(LC_ServerPlayer* pkPlayer,BuddyInstance*pBuddy);
	
	//宝具进化 +升级属性 +基础( 公式 基础属性*进化*升级)
	LC_ActorAttributeMap* GetRuneAttrMap(){ return &m_RuneAttributeMap; }
	
	void RefreshRuneAttrMap(BuddyInstance* pBuddy);

	// 纹章基础+雕刻+淬炼
	LC_ActorAttributeMap* GetEquipAttrMap(){ return &m_EquipAttributeMap; }
	
	void RefreshEquipAttrMap(LC_ServerPlayer* pkPlayer,BuddyInstance*pBuddy);

	// 皮肤属性
	LC_ActorAttributeMap* GetSkinAttrMap(){ return &m_SkinAttributeMap; }
	// RefreshSkinAttrMap
	void RefreshSkinAttrMap(LC_ServerPlayer* pkPlayer,BuddyInstance*pBuddy);

	uint64_t GetBuddyCombatScoreByType(uint8_t uType);
	void SetBuddyCombatScoreByType(LC_ServerPlayer* pkPlayer, uint8_t uType,uint64_t nValue);

	void RefreshBuddyRankCombatScore(BuddyInstance* pkBuddy);
private:
	int32_t m_configId; // buddyid 
	CSVFile::CF_Buddy::DataEntry* m_pConfig;
	LC_ActorAttributeMap				m_NewFinalAttributeMap;  // 本英雄出战属性
	LC_ActorAttributeMap				m_HeroAllAttributeMap;  // 本英雄全部系统的属性
	LC_ActorAttributeMap				m_GiveToOtherCoreAttributeMap;  // 我提供给别的英雄的灵魂链属性
	LC_ActorAttributeMap				m_GetChainAllAttributeMap;  // 灵魂链提供给我的灵魂链属性
	LC_ActorAttributeMap				m_LevelAttributeMap;	// 英雄升级+星级 不包含基础
	LC_ActorAttributeMap				m_RuneAttributeMap;		// 宝具进化 +升级属性 +基础( 公式 基础属性*进化*升级)
	LC_ActorAttributeMap				m_EquipAttributeMap;	// 纹章基础+雕刻+淬炼
	LC_ActorAttributeMap				m_SkinAttributeMap;		//皮肤提供的固定属性

	uint64_t								m_nBuddyCombatScore[BUDDY_COBAT_SCORE]; // 英雄所有细分战力集合
};

#endif //__BUDDY_ATTR_MANAGER_H__
