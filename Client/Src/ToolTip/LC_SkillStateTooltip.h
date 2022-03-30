//技能状态的tooltip数据组织

#ifndef _LC_SKILLSTATETOOLTIP_H
#define _LC_SKILLSTATETOOLTIP_H

#include "LC_TooltipBase.h"


namespace GameLogic
{
	class LC_SkillStateTooltip: public LC_TooltipBase
	{
	public:
		LC_SkillStateTooltip(void);
		virtual ~LC_SkillStateTooltip(void);
		
		virtual bool BuildTooltip();

		//Begin 构造tooltip需要的参数接口
		void Init(int lSkillStateID, LC_PlayerBase* pkObserver);
		//End

	protected:
		//构造tooltip的技能状态使用效果字段
		bool _addTooltipFieldUseResult(TooltipFieldVector& kFieldVector);
		//构造tooltip的技能状态名字段
		bool _addTooltipFieldStateName(TooltipFieldVector& kFieldVector);

	private:
		int m_lSkillStateID;				//技能状态ID
	};
}

#endif

