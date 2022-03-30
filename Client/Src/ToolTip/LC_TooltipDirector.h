//tooltip的调用接口类

#ifndef _LC_TOOLTIP_DIRECTOR_H
#define _LC_TOOLTIP_DIRECTOR_H

#include "LC_TooltipBase.h"
#include "LC_ActorAttributeMap.h"
#include "LC_ClientGamePlayer.h"

namespace GameLogic
{
	class LC_ConsumeItemTooltip;
	class LC_EquipItemTooltip;
	class LC_SkillBookItemTooltip;
	class LC_FormulaItemTooltip;
	class LC_MissionItemTooltip;
	class LC_MaterialItemTooltip;
	class LC_VehicleItemTooltip;
	class LC_HobbyItemTooltip;
	class LC_SkillTooltipBase;
	class LC_SkillStateTooltip;
	class LC_SkillLevelUpTooltip;
	class LC_TalentTooltip;
	class LC_CardTooltip;
	class LC_GoodTooltip;
	class LC_MeditateTooltip;
	class LC_GameStoryTooltip;
	class LC_TooltipDirector : public Memory::MM_BaseObject, public Utility::UT_Singleton<LC_TooltipDirector>
	{
	public:
		LC_TooltipDirector(void);
		virtual ~LC_TooltipDirector(void);

		void Init();
		void UnInit();

		//LC_TooltipBase::TooltipFieldIterator ConstructItemTooltip(int lItemID, LC_BackPackEntry* pkBackPackEntry, LC_ActorBase* pkObserver);
		//LC_TooltipBase::TooltipFieldIterator ConstructSkillTooltip(int lItemID, LC_ActorBase* pkObserver);
		//LC_TooltipBase::TooltipFieldIterator ConstructSkillStateTooltip(int lItemID, LC_ActorBase* pkObserver);
		
		LC_TooltipBase::TooltipFieldVector ConstructItemTooltip(int lItemID, LC_BackPackEntry* pkBackPackEntry, LC_NPCShopItemEntry* pkShopEntry, LC_PlayerBase* pkObserver, bool bCompareSelfEquip = false, 
																LC_BackPackEntry *pkCurrentEquip = NULL, LC_ClientRankUserInfo *pkRankUserInfo = NULL, bool bShopOpen = false, int iExistFuncId = 0,
																bool bGuildWarehouse = false, int iStarry = 0);
		LC_TooltipBase::TooltipFieldVector ConstructEquipEnhanceTooltip(int lItemID, LC_BackPackEntry* pkBackPackEntry, LC_PlayerBase* pkObserver);
		LC_TooltipBase::TooltipFieldVector ConstructEquipCompotisionTooltip(int lItemID, int iCompotisionLevel, LC_PlayerBase* pkObserver);
		LC_TooltipBase::TooltipFieldVector ConstructSkillTooltip(int lSkillID, LC_PlayerBase* pkObserver, bool bCurrentLevelOnly = false);
		LC_TooltipBase::TooltipFieldVector ConstructSkillStateTooltip(int lSkillStateID, LC_PlayerBase* pkObserver);
		LC_TooltipBase::TooltipFieldVector ConstructMeditateInfoTooltip(LC_PlayerBase *pkObserver);
		LC_TooltipBase::TooltipFieldVector ConstructGameStoryTooltip(int iGameStoryID, int iTotalCost, int iRaidInfoID, GameStoryInfo *pkGameStoryInfo);

		LC_SkillTooltipBase*			   GetSkillTooltipConstructor(){return m_pkSkillTooltipBase;};
	
		StringType						   GetItemAttributeFuncDescript(LC_ActorAttributeAppendMap& rkBaseAttributeAppendMap,LC_ActorAttributeAppendMap& rkAddAttributeAppendMap);
		StringType						   GetAttributeFuncDescript(int lAttributeFuncID);

		LC_SkillStateTooltip* GetSkillStateTooltip() { return m_pkSkillStateTooltip; }

	protected:
		LC_ConsumeItemTooltip*	 m_pkConsumItemTooltip;			//消耗类物品
		LC_EquipItemTooltip*	 m_pkEquipItemTooltip;			//装备类物品
		LC_SkillBookItemTooltip* m_pkSkillBookTooltip;			//技能书类物品
		LC_MaterialItemTooltip*	 m_pkMaterialTooltip;			//材料物品
		LC_SkillTooltipBase*	 m_pkSkillTooltipBase;			//技能
		LC_SkillStateTooltip*	m_pkSkillStateTooltip;			//技能状态
		LC_MeditateTooltip*		m_pkMeditateTooltip;			//打坐
		LC_GameStoryTooltip*	m_pkGameStoryTooltip;			//活动
	};
}

#endif


