//���ܵ�tooltip������֯
//ʹ��Buildģʽ����tooltip������

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

		//Begin ����tooltip��Ҫ�Ĳ����ӿ�
		void Init(int lSkillID, LC_PlayerBase* pkObserver);
		void SetSkillID(int lSkillID){m_lSkillID = lSkillID;};
		int GetSkillID(){return m_lSkillID;};
		//End

		//����tooltip�ļ���ʩ�ž����ֶ�
		bool AddTooltipFieldExecuteDistance(TooltipFieldVector& kFieldVector);
		//����tooltip�ļ���ʩ��ʱ���ֶ�
		bool AddTooltipFieldExecuteTime(TooltipFieldVector& kFieldVector);
		//����tooltip�ļ�����ȴʱ���ֶ�
		bool AddTooltipFieldCDTime(TooltipFieldVector& kFieldVector);
		//����tooltip�ļ���ʹ��Ч���ֶ�
		bool AddTooltipFieldUseResult(TooltipFieldVector& kFieldVector);
		//����tooltip�ļ��ܷ�Χ�����ֶ�
		bool AddTooltipFieldAtmosDescript(TooltipFieldVector& kFieldVector);

	protected:
		//����tooltip�ļ��������ֶ�
		bool _addTooltipFieldName(TooltipFieldVector& kFieldVector);
		//����tooltip�������������ֶ�
		bool _addTooltipFieldPosNegType(TooltipFieldVector &kFieldVector);
		//����tooltip�ļ��ܼ����ֶ�
		bool _addTooltipFieldLevel(TooltipFieldVector& kFieldVector);
		//����tooltip�ļ�����һ�����ֶ�(���ж��Ƿ�Ϊ��ѧ����)
		bool _addTooltipFieldLevelNext(TooltipFieldVector& kFieldVector);
		//����tooltip�ļ��ܸ������ֶ�
		bool _addTooltipFieldDescript(TooltipFieldVector& kFieldVector);
		//����tooltip�ļ��������ֶ�
		bool _addTooltipFieldCategory(TooltipFieldVector& kFieldVector);
		//����tooltip�ĵ�ǰ���ܵȼ��ֶ�
		bool _addTooltipFieldCurLevel(TooltipFieldVector& kFieldVector);
		//����tooltip�ļ�����������ȼ��ֶ�
		bool _addTooltipFieldPlayerLevel(TooltipFieldVector &kFieldVector);
		//����tooltip�ļ��������ֶ�
		bool _addTooltipFieldConsume(TooltipFieldVector& kFieldVector);
		//����tooltip�ļ���ʩ�ž����ֶ�
		bool _addTooltipFieldExecuteDistance(TooltipFieldVector& kFieldVector);
		//����tooltip�ļ���ʩ��ʱ���ֶ�
		bool _addTooltipFieldExecuteTime(TooltipFieldVector& kFieldVector);
		//����tooltip�ļ�����ȴʱ���ֶ�
		bool _addTooltipFieldCDTime(TooltipFieldVector& kFieldVector);
		//����tooltip�ļ�����Ʒ�����ֶ�
		bool _addTooltipFieldMaterialRequired(TooltipFieldVector& kFieldVector);
		//����tooltip�ļ���ʹ��Ч���ֶ�
		bool _addTooltipFieldUseResult(TooltipFieldVector& kFieldVector);
		//����tooltip�Ľ��������ֶ�
		bool _addTooltipFieldUnlockContent(TooltipFieldVector& kFieldVector);
		//����tooltip���÷������ֶ�
		bool _addTooltipFieldUseDescript(TooltipFieldVector& kFieldVector);
		//����tooltip�ļ��ܷ�Χ�����ֶ�
		bool _addTooltipFieldAtmosDescript(TooltipFieldVector& kFieldVector);
		//����tooltip�ļ�����һ���ֶ�
		bool _addTooltipFieldNextLevel(TooltipFieldVector& kFieldVector);
		//���ɼ����о���Ҫ˵��
		bool _addGuildSkillToolTipResearchDecl(TooltipFieldVector& kFieldVector);
		//���ɼ����о�״̬
		bool _addGuildSkillToolTipResearchState(TooltipFieldVector& kFieldVector);

		//���ݼ���ID��ȡ�ü������еĵȼ����
		bool _getSkillLevels(int lSkillID, std::vector<int>& kSkillLevels);


	private:
		int m_lSkillID;			//����ID
		
	};
}

#endif

