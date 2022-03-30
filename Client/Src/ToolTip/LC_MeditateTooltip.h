//打坐信息的tooltip数据组织

#ifndef _LC_SKILLPOINTTOOLTIP_H
#define _LC_SKILLPOINTTOOLTIP_H

#include "LC_TooltipBase.h"

namespace GameLogic
{
	class LC_PlayerBase;
	class LC_MeditateTooltip : public LC_TooltipBase
	{
	public:
		LC_MeditateTooltip(void);
		virtual ~LC_MeditateTooltip(void);
		virtual bool BuildTooltip(void);

		StringType _formatTimeStr(float fTime);

	protected:
		bool _addTooltipFieldTitle(TooltipFieldVector &kFieldVector);				//打坐标题
		bool _addTooltipFieldTime(TooltipFieldVector &kFieldVector);				//累计时间
		bool _addTooltipFieldSpRecoverRate(TooltipFieldVector &kFieldVector);		//真气获取速度
		bool _addTooltipFieldSkillPointLevel(TooltipFieldVector &kFieldVector);		//真气等级
		bool _addTooltipFieldSkillPointGained(TooltipFieldVector &kFieldVector);	//获得真气
		bool _addTooltipFieldTotalSkillPoint(TooltipFieldVector &kFieldVector);		//真气总值
		bool _addTooltipFieldExpGained(TooltipFieldVector &kFieldVector);	//获得经验
	};
}

#endif