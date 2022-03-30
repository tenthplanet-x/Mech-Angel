//����ϸ�ڵ�������֯�����ڼ���UI�жԼ��ܵ���ϸ����
//ʹ��Adapterģʽ������LC_SkillTooltipBase�е���ؽӿ�

#ifndef _LC_SKILLDELAIL_H
#define _LC_SKILLDELAIL_H

#include "LC_SkillTooltipBase.h"

namespace GameLogic
{
	class LC_SkillInfoEntry : public Memory::MM_BaseObject
	{
	public:
		void Reset();
		DECL_RTTI_NAME( LC_SkillInfoEntry )
		MEMBER_INT(TypeID);				//����ID
		MEMBER_STRING(SkillName);			//��������
		MEMBER_INT(Level);					//���ܵȼ�
		MEMBER_STRING(IconPath);			//ͼ������	
		MEMBER_STRING(SkillDescription);	//��������
		MEMBER_STRING(SkillAttri1);			//����������벿��
		MEMBER_STRING(SkillAttri2);			//���������Ұ벿��
		MEMBER_INT(PosNegType);			//������������
		MEMBER_INT(Category);				//��������
		MEMBER_INT(RangeType);				//���̣�Զ��
		MEMBER_INT(MinExecuteDistance);	//��С��������
		MEMBER_INT(MaxExecuteDistance);	//��󹥻�����
		MEMBER_FLOAT(ExecuteTime);			//ʩ��ʱ��
		MEMBER_FLOAT(CoolDownTime);			//��ȴʱ��
		MEMBER_INT(MPValueCost);			//��Ϣ����
		MEMBER_STRING(NeedItems);			//ʹ�ü�������Ҫ����Ʒ
		MEMBER_STRING(UseResult);			//����ʹ��Ч��
		MEMBER_STRING(UseResult2);			//����ʹ��Ч��2 (���ļ���Ч��)
		MEMBER_INT(ActorLevel);			//����������Ҫ�ĵȼ�
		MEMBER_INT(ActorStarLevel);		//����������Ҫ���Ǽ�
		MEMBER_INT(SP);					//��������
		MEMBER_INT(Money);					//������Ǯ
		MEMBER_STRING(Item);				//��������
		MEMBER_STRING(SkillBook);			//�����ؼ�����
		MEMBER_STRING(PreTask);				//ǰ������
		MEMBER_INT(TopLevel);				//���ȼ�
		MEMBER_BOOL(IsLearned);				//�Ƿ���ѧ����
		MEMBER_INT(Category2);				//����2
		MEMBER_STRING(ConstParameter);		//����
		MEMBER_INT(UpRate);				//�����ɹ�����
		MEMBER_STRING(VideoName);			//������Ƶ����
		MEMBER_INT(SkillBookID);			//������ID
		MEMBER_BOOL(IsBreakThrough);		//�Ƿ��ͻ��
		MEMBER_STRING(Subordinate);			//������Ҫ�����ߵȼ���Ϣ
		MEMBER_INT(StarLevel);				//������ҪӢ���Ǽ�
	};

	class LC_SkillDetail: public LC_SkillTooltipBase, public Utility::UT_Singleton<LC_SkillDetail>
	{
	public:
		LC_SkillDetail(void);
		virtual ~LC_SkillDetail(void);

		typedef mem::map<StringType, TooltipFieldVector>		SkillDetailMap;
		typedef mem::vector<SkillDetailMap>						SkillDetailVector;
		typedef Utility::UT_VectorIterator<SkillDetailVector>	SkillDetailIterator;
		
		SkillDetailIterator GetIterator(){return SkillDetailIterator(m_kSkillDetails.begin(), m_kSkillDetails.end());}
		SkillDetailVector&	GetSkillDetails(){return m_kSkillDetails;}

		//adapter method
		StringType			GetSkillUseResults(int lSkillID);
		StringType			GetSkillUseResults2(int lSkillID);
		
	protected:
		//����۲��ߵ�ǰ�ȼ��ֶ�
		bool _addTooltipFieldObserverLevel(std::vector<int>& kSkillLevels, TooltipFieldVector& kFieldVector);
		//���켼�����������ֶ�
		bool _addTooltipFieldUpdateRequest(TooltipFieldVector& kFieldVector);
		//���켼�ܳɾ��ֶ�
		bool _addTooltipFieldAchieve(TooltipFieldVector& kFieldVector);
		//���ݼ��ܱ�Ż�ȡ���������ƣ������������ƺ�����ֱ�ţ�
		StringType _getSkillMainName(const StringType& kSkillName);

		SkillDetailVector	m_kSkillDetails;				//����ϸ������
	};
}

#endif