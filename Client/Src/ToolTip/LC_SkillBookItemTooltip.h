//�������tooltip������֯

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
		//����tooltip��avatar�ֶ�
		bool _addTooltipFieldAvatar(TooltipFieldVector& kFieldVector);
		//����tooltip��ʹ��ʱ��
		bool _addTooltipTimeLimit(TooltipFieldVector& kFieldVector);
		//����tooltip����Ʒ������ѧϰ�ֶ�
		bool _addTooltipFieldLearned(TooltipFieldVector& kFieldVector);
		//����tooltip����Ʒ������ʹ��Ч���ֶ�
		bool _addTooltipFieldUseResult(TooltipFieldVector& kFieldVector);
		//����tooltip����Ʒ��������������ֶ�
		bool _addTooltipFieldMaterialRequied(TooltipFieldVector& kFieldVector);
		//����tooltip�ĳɳ�ϵͳ�����ֶ�
		bool _addTooltipFieldCimeliaLimit(TooltipFieldVector& kFieldVector);
		//����tooltip����Ƶ�ֶ�
		bool _addTooltipVideo(TooltipFieldVector& kFieldVector);
		//����tooltip�Ĺ�����
		bool _addTooltipScore(TooltipFieldVector& kFieldVector);
		//����tooltip������
		bool _addTooltipAttrib(TooltipFieldVector& kFieldVector);
		//����tooltip����
		bool _addTooltipSkillDes(TooltipFieldVector& kFieldVector);
		bool _addTooltipDescript(TooltipFieldVector& kFieldVector);
		//����tooltip���츳�����ֶ�
		bool _addTooltipFieldTalentInfo(TooltipFieldVector& kFieldVector);
		//����tooltip�ļ��������ֶ�
		bool _addTooltipFieldSkillInfo(TooltipFieldVector& kFieldVector);
		//����tooltip�ĳ�ʼ�Ǽ��ֶ�
		bool _addTooltipFieldStarInfo(TooltipFieldVector& kFieldVector);

		void _getAttribValue(const UT_SIMDataInfo& data, int& iParam, int& iParamPercent);
	};
}

#endif


