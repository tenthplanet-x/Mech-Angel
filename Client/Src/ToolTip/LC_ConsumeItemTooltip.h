//消耗类物品的tooltip数据组织
#ifndef _LC_CONSUMEITEMTOOLTIP_H
#define _LC_CONSUMEITEMTOOLTIP_H


#include "LC_ItemTooltipBase.h"

namespace GameLogic
{
	class LC_ConsumeItemTooltip : public LC_ItemTooltipBase
	{
	public:
		LC_ConsumeItemTooltip(void);
		virtual ~LC_ConsumeItemTooltip(void);

		virtual bool BuildTooltip();
		bool BuildTooltip(bool bCompareSelfEquip);
		bool BuildMultiGiftTooltip(bool bCompareSelfEquip);

	protected:

	};
}

#endif
