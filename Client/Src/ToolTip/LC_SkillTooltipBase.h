//技能的tooltip数据组织
//使用Build模式控制tooltip的生成

#ifndef _LC_SKILLTOOLTIP_BASE_H
#define _LC_SKILLTOOLTIP_BASE_H

#include "LC_TooltipBase.h"

namespace GameLogic
{
	class LC_SkillTooltipBase : public LC_TooltipBase
	{
	public:
		LC_SkillTooltipBase(void);
		virtual ~LC_SkillTooltipBase(void);

		virtual bool BuildTooltip();
		bool BuildTooltip(bool bCurrentLevelOnly);

		//Begin 构造tooltip需要的参数接口
		void Init(int lSkillID, LC_PlayerBase* pkObserver);
		void SetSkillID(int lSkillID){m_lSkillID = lSkillID;};
		int GetSkillID(){return m_lSkillID;};
		//End

		//构造tooltip的技能施放距离字段
		bool AddTooltipFieldExecuteDistance(TooltipFieldVector& kFieldVector);
		//构造tooltip的技能施放时间字段
		bool AddTooltipFieldExecuteTime(TooltipFieldVector& kFieldVector);
		//构造tooltip的技能冷却时间字段
		bool AddTooltipFieldCDTime(TooltipFieldVector& kFieldVector);
		//构造tooltip的技能使用效果字段
		bool AddTooltipFieldUseResult(TooltipFieldVector& kFieldVector);
		//构造tooltip的技能氛围介绍字段
		bool AddTooltipFieldAtmosDescript(TooltipFieldVector& kFieldVector);

	protected:
		//构造tooltip的技能名称字段
		bool _addTooltipFieldName(TooltipFieldVector& kFieldVector);
		//构造tooltip的主被动类型字段
		bool _addTooltipFieldPosNegType(TooltipFieldVector &kFieldVector);
		//构造tooltip的技能级别字段
		bool _addTooltipFieldLevel(TooltipFieldVector& kFieldVector);
		//构造tooltip的技能下一级别字段(不判断是否为已学技能)
		bool _addTooltipFieldLevelNext(TooltipFieldVector& kFieldVector);
		//构造tooltip的技能副标题字段
		bool _addTooltipFieldDescript(TooltipFieldVector& kFieldVector);
		//构造tooltip的技能类型字段
		bool _addTooltipFieldCategory(TooltipFieldVector& kFieldVector);
		//构造tooltip的当前技能等级字段
		bool _addTooltipFieldCurLevel(TooltipFieldVector& kFieldVector);
		//构造tooltip的技能需求人物等级字段
		bool _addTooltipFieldPlayerLevel(TooltipFieldVector &kFieldVector);
		//构造tooltip的技能消耗字段
		bool _addTooltipFieldConsume(TooltipFieldVector& kFieldVector);
		//构造tooltip的技能施放距离字段
		bool _addTooltipFieldExecuteDistance(TooltipFieldVector& kFieldVector);
		//构造tooltip的技能施放时间字段
		bool _addTooltipFieldExecuteTime(TooltipFieldVector& kFieldVector);
		//构造tooltip的技能冷却时间字段
		bool _addTooltipFieldCDTime(TooltipFieldVector& kFieldVector);
		//构造tooltip的技能物品需求字段
		bool _addTooltipFieldMaterialRequired(TooltipFieldVector& kFieldVector);
		//构造tooltip的技能使用效果字段
		bool _addTooltipFieldUseResult(TooltipFieldVector& kFieldVector);
		//构造tooltip的解锁条件字段
		bool _addTooltipFieldUnlockContent(TooltipFieldVector& kFieldVector);
		//构造tooltip的用法介绍字段
		bool _addTooltipFieldUseDescript(TooltipFieldVector& kFieldVector);
		//构造tooltip的技能氛围介绍字段
		bool _addTooltipFieldAtmosDescript(TooltipFieldVector& kFieldVector);
		//构造tooltip的技能下一重字段
		bool _addTooltipFieldNextLevel(TooltipFieldVector& kFieldVector);
		//门派技能研究需要说明
		bool _addGuildSkillToolTipResearchDecl(TooltipFieldVector& kFieldVector);
		//门派技能研究状态
		bool _addGuildSkillToolTipResearchState(TooltipFieldVector& kFieldVector);

		//根据技能ID获取该技能所有的等级编号
		bool _getSkillLevels(int lSkillID, std::vector<int>& kSkillLevels);


	private:
		int m_lSkillID;			//技能ID
		
	};
}

#endif

