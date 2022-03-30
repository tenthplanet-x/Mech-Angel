//�������ϵ�tooltip������֯

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
		//����tooltip����;�ֶ�
		bool _addTooltipFieldUse(TooltipFieldVector& kFieldVector);
		//������Ƕ��ʾ�ֶ�
		bool _addTooltipFieldSocketInfo(TooltipFieldVector &kFieldVector);
	};
}
