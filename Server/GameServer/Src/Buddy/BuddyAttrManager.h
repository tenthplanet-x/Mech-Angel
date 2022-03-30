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
	// ��ս ����
	LC_ActorAttributeMap* GetNewFinalAttributeMap(){ return &m_NewFinalAttributeMap; }
	void RefreshNewFinalAttrMap(LC_ServerPlayer* pkPlayer,BuddyInstance*pBuddy, bool login, int32_t ReasonType=SPART_None);
	//��ȡ ��Ӣ�� �ṩ�������������
	LC_ActorAttributeMap* GetHeroGiveToOtherCoreAttrMap(){ return &m_GiveToOtherCoreAttributeMap; }
	void RefreshHeroGiveToOtherCoreAttrMap(LC_ServerPlayer* pkPlayer,bool bRefresh = true);
	//��ȡ ��Ӣ�� ��������
	LC_ActorAttributeMap* GetHeroAllAttributeMap(){ return &m_HeroAllAttributeMap; }
	void RefreshHeroAllAttributeMap(LC_ServerPlayer* pkPlayer,BuddyInstance*pBuddy);
	
	//��ȡ ��Ӣ�� ��õ������������
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
	//��ȡ�ȼ� �Ǽ��ṩ������ 
	void GetBaseAttrMap(LC_ActorAttributeMap & attrMap);
	LC_ActorAttributeMap* GetLevelNotBaseAttrMap(){ return &m_LevelAttributeMap; }
	// refreshLevel
	void RefreshLevelAttrMap(LC_ServerPlayer* pkPlayer,BuddyInstance*pBuddy);
	
	//���߽��� +�������� +����( ��ʽ ��������*����*����)
	LC_ActorAttributeMap* GetRuneAttrMap(){ return &m_RuneAttributeMap; }
	
	void RefreshRuneAttrMap(BuddyInstance* pBuddy);

	// ���»���+���+����
	LC_ActorAttributeMap* GetEquipAttrMap(){ return &m_EquipAttributeMap; }
	
	void RefreshEquipAttrMap(LC_ServerPlayer* pkPlayer,BuddyInstance*pBuddy);

	// Ƥ������
	LC_ActorAttributeMap* GetSkinAttrMap(){ return &m_SkinAttributeMap; }
	// RefreshSkinAttrMap
	void RefreshSkinAttrMap(LC_ServerPlayer* pkPlayer,BuddyInstance*pBuddy);

	uint64_t GetBuddyCombatScoreByType(uint8_t uType);
	void SetBuddyCombatScoreByType(LC_ServerPlayer* pkPlayer, uint8_t uType,uint64_t nValue);

	void RefreshBuddyRankCombatScore(BuddyInstance* pkBuddy);
private:
	int32_t m_configId; // buddyid 
	CSVFile::CF_Buddy::DataEntry* m_pConfig;
	LC_ActorAttributeMap				m_NewFinalAttributeMap;  // ��Ӣ�۳�ս����
	LC_ActorAttributeMap				m_HeroAllAttributeMap;  // ��Ӣ��ȫ��ϵͳ������
	LC_ActorAttributeMap				m_GiveToOtherCoreAttributeMap;  // ���ṩ�����Ӣ�۵����������
	LC_ActorAttributeMap				m_GetChainAllAttributeMap;  // ������ṩ���ҵ����������
	LC_ActorAttributeMap				m_LevelAttributeMap;	// Ӣ������+�Ǽ� ����������
	LC_ActorAttributeMap				m_RuneAttributeMap;		// ���߽��� +�������� +����( ��ʽ ��������*����*����)
	LC_ActorAttributeMap				m_EquipAttributeMap;	// ���»���+���+����
	LC_ActorAttributeMap				m_SkinAttributeMap;		//Ƥ���ṩ�Ĺ̶�����

	uint64_t								m_nBuddyCombatScore[BUDDY_COBAT_SCORE]; // Ӣ������ϸ��ս������
};

#endif //__BUDDY_ATTR_MANAGER_H__
