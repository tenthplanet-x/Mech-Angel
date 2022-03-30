//������Ϣ��tooltip������֯

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
		bool _addTooltipFieldTitle(TooltipFieldVector &kFieldVector);				//��������
		bool _addTooltipFieldTime(TooltipFieldVector &kFieldVector);				//�ۼ�ʱ��
		bool _addTooltipFieldSpRecoverRate(TooltipFieldVector &kFieldVector);		//������ȡ�ٶ�
		bool _addTooltipFieldSkillPointLevel(TooltipFieldVector &kFieldVector);		//�����ȼ�
		bool _addTooltipFieldSkillPointGained(TooltipFieldVector &kFieldVector);	//�������
		bool _addTooltipFieldTotalSkillPoint(TooltipFieldVector &kFieldVector);		//������ֵ
		bool _addTooltipFieldExpGained(TooltipFieldVector &kFieldVector);	//��þ���
	};
}

#endif