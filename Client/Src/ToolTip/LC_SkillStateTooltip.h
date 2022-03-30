//����״̬��tooltip������֯

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

		//Begin ����tooltip��Ҫ�Ĳ����ӿ�
		void Init(int lSkillStateID, LC_PlayerBase* pkObserver);
		//End

	protected:
		//����tooltip�ļ���״̬ʹ��Ч���ֶ�
		bool _addTooltipFieldUseResult(TooltipFieldVector& kFieldVector);
		//����tooltip�ļ���״̬���ֶ�
		bool _addTooltipFieldStateName(TooltipFieldVector& kFieldVector);

	private:
		int m_lSkillStateID;				//����״̬ID
	};
}

#endif

