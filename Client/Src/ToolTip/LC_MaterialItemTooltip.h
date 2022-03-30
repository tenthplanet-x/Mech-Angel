//生产材料的tooltip数据组织

#pragma once

#include "LC_ItemTooltipBase.h"

namespace GameLogic
{
	class LC_MaterialItemTooltip : public LC_ItemTooltipBase
	{
	public:
		LC_MaterialItemTooltip(void);
		virtual ~LC_MaterialItemTooltip(void);

		virtual bool BuildTooltip();

	private:
		//构造tooltip的用途字段
		bool _addTooltipFieldUse(TooltipFieldVector& kFieldVector);
		//构造镶嵌提示字段
		bool _addTooltipFieldSocketInfo(TooltipFieldVector &kFieldVector);
	};
}
