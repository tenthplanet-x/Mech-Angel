//技能细节的数据组织，用于技能UI中对技能的详细描述
//使用Adapter模式调用了LC_SkillTooltipBase中的相关接口

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
		MEMBER_INT(TypeID);				//技能ID
		MEMBER_STRING(SkillName);			//技能名称
		MEMBER_INT(Level);					//技能等级
		MEMBER_STRING(IconPath);			//图标名称	
		MEMBER_STRING(SkillDescription);	//技能描述
		MEMBER_STRING(SkillAttri1);			//技能属性左半部分
		MEMBER_STRING(SkillAttri2);			//技能属性右半部分
		MEMBER_INT(PosNegType);			//主动，被动？
		MEMBER_INT(Category);				//技能类型
		MEMBER_INT(RangeType);				//近程，远程
		MEMBER_INT(MinExecuteDistance);	//最小攻击距离
		MEMBER_INT(MaxExecuteDistance);	//最大攻击距离
		MEMBER_FLOAT(ExecuteTime);			//施法时间
		MEMBER_FLOAT(CoolDownTime);			//冷却时间
		MEMBER_INT(MPValueCost);			//内息消耗
		MEMBER_STRING(NeedItems);			//使用技能所需要的物品
		MEMBER_STRING(UseResult);			//技能使用效果
		MEMBER_STRING(UseResult2);			//技能使用效果2 (符文激活效果)
		MEMBER_INT(ActorLevel);			//技能升级需要的等级
		MEMBER_INT(ActorStarLevel);		//技能升级需要的星级
		MEMBER_INT(SP);					//升级真气
		MEMBER_INT(Money);					//升级金钱
		MEMBER_STRING(Item);				//升级道具
		MEMBER_STRING(SkillBook);			//升级秘籍需求
		MEMBER_STRING(PreTask);				//前置任务
		MEMBER_INT(TopLevel);				//最大等级
		MEMBER_BOOL(IsLearned);				//是否已学技能
		MEMBER_INT(Category2);				//类型2
		MEMBER_STRING(ConstParameter);		//参数
		MEMBER_INT(UpRate);				//升级成功几率
		MEMBER_STRING(VideoName);			//技能视频名称
		MEMBER_INT(SkillBookID);			//技能书ID
		MEMBER_BOOL(IsBreakThrough);		//是否可突破
		MEMBER_STRING(Subordinate);			//升级需要进阶线等级信息
		MEMBER_INT(StarLevel);				//升级需要英雄星级
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
		//构造观察者当前等级字段
		bool _addTooltipFieldObserverLevel(std::vector<int>& kSkillLevels, TooltipFieldVector& kFieldVector);
		//构造技能升级需求字段
		bool _addTooltipFieldUpdateRequest(TooltipFieldVector& kFieldVector);
		//构造技能成就字段
		bool _addTooltipFieldAchieve(TooltipFieldVector& kFieldVector);
		//根据技能编号获取技能主名称（剪除技能名称后的数字编号）
		StringType _getSkillMainName(const StringType& kSkillName);

		SkillDetailVector	m_kSkillDetails;				//技能细节数据
	};
}

#endif