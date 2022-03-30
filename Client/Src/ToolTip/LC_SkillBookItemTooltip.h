//技能书的tooltip数据组织

#ifndef _LC_SkillBookItemTooltip_H
#define _LC_SkillBookItemTooltip_H

#include "LC_ItemTooltipBase.h"

namespace GameLogic
{
	class LC_SkillBookItemTooltip : public LC_ItemTooltipBase
	{
	public:
		LC_SkillBookItemTooltip(void);
		virtual ~LC_SkillBookItemTooltip(void);

		virtual bool BuildTooltip();

	protected:
		//构造tooltip的avatar字段
		bool _addTooltipFieldAvatar(TooltipFieldVector& kFieldVector);
		//构造tooltip的使用时限
		bool _addTooltipTimeLimit(TooltipFieldVector& kFieldVector);
		//构造tooltip的物品技能书学习字段
		bool _addTooltipFieldLearned(TooltipFieldVector& kFieldVector);
		//构造tooltip的物品技能书使用效果字段
		bool _addTooltipFieldUseResult(TooltipFieldVector& kFieldVector);
		//构造tooltip的物品技能书需求材料字段
		bool _addTooltipFieldMaterialRequied(TooltipFieldVector& kFieldVector);
		//构造tooltip的成长系统需求字段
		bool _addTooltipFieldCimeliaLimit(TooltipFieldVector& kFieldVector);
		//构造tooltip的视频字段
		bool _addTooltipVideo(TooltipFieldVector& kFieldVector);
		//构造tooltip的攻击力
		bool _addTooltipScore(TooltipFieldVector& kFieldVector);
		//构造tooltip的属性
		bool _addTooltipAttrib(TooltipFieldVector& kFieldVector);
		//构造tooltip描述
		bool _addTooltipSkillDes(TooltipFieldVector& kFieldVector);
		bool _addTooltipDescript(TooltipFieldVector& kFieldVector);
		//构造tooltip的天赋描述字段
		bool _addTooltipFieldTalentInfo(TooltipFieldVector& kFieldVector);
		//构造tooltip的技能描述字段
		bool _addTooltipFieldSkillInfo(TooltipFieldVector& kFieldVector);
		//构造tooltip的初始星级字段
		bool _addTooltipFieldStarInfo(TooltipFieldVector& kFieldVector);

		void _getAttribValue(const UT_SIMDataInfo& data, int& iParam, int& iParamPercent);
	};
}

#endif


